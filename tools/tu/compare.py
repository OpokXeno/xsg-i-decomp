#!/usr/bin/env python3
"""Compare a rebuilt file image (or real ELF) with the original ELF.

--rebuilt is a flat objcopy image (whole-file comparison) unless --elf is given, in
which case the loadable segments / allocated sections of a real linked ELF are
compared at their original VAs and the header/section-table differences listed.
"""
import argparse
import hashlib
import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from elfinfo import Elf  # noqa: E402


def ranges(a, b, base=0):
    out, start = [], None
    n = max(len(a), len(b))
    for i in range(n):
        d = i >= len(a) or i >= len(b) or a[i] != b[i]
        if d and start is None:
            start = i
        elif not d and start is not None:
            out.append((base + start, base + i))
            start = None
    if start is not None:
        out.append((base + start, base + n))
    return out


def owner(orig, off):
    for s in orig.sections:
        if s.type != 8 and s.size and s.offset <= off < s.offset + s.size:
            return s
    return None


def func_at(orig, va, shndx):
    best = None
    for s in orig.symbols:
        if s.shndx == shndx and s.type == 2 and s.value <= va < s.value + max(s.size, 1):
            best = s
    return best.name if best else None


def compare_flat(orig, data, rebuilt):
    diffs = ranges(orig.data, rebuilt)
    report = dict(mode="whole-file", original_sha256=hashlib.sha256(orig.data).hexdigest(),
                  rebuilt_sha256=hashlib.sha256(rebuilt).hexdigest(),
                  original_size=len(orig.data), rebuilt_size=len(rebuilt),
                  whole_file_identical=orig.data == rebuilt, diff_range_count=len(diffs))
    per_section = []
    for s in orig.sections:
        if s.type == 8 or not s.size:
            continue
        a = orig.data[s.offset:s.offset + s.size]
        b = rebuilt[s.offset:s.offset + s.size]
        per_section.append(dict(index=s.index, name=s.name, offset=s.offset, size=s.size, identical=a == b))
    report["sections"] = per_section
    detail = []
    for a, b in diffs[:400]:
        s = owner(orig, a)
        entry = dict(file_offset=f"0x{a:X}", length=b - a, section=s.name if s else None)
        if s and s.flags & 2:
            va = s.addr + a - s.offset
            entry["va"] = f"0x{va:08X}"
            entry["function"] = func_at(orig, va, s.index)
            entry["original"] = orig.data[a:min(b, a + 16)].hex()
            entry["rebuilt"] = rebuilt[a:min(b, a + 16)].hex()
        detail.append(entry)
    report["diffs"] = detail
    return report


def compare_elf(orig, new):
    report = dict(mode="elf", original_sha256=hashlib.sha256(orig.data).hexdigest(),
                  rebuilt_sha256=hashlib.sha256(new.data).hexdigest(),
                  whole_file_identical=orig.data == new.data)
    keys = ("type", "offset", "vaddr", "paddr", "filesz", "memsz", "flags", "align")
    report["program_headers_identical"] = [tuple(getattr(p, k) for k in keys) for p in orig.phdrs] == \
        [tuple(getattr(p, k) for k in keys) for p in new.phdrs]
    report["program_headers"] = dict(original=[{k: getattr(p, k) for k in keys} for p in orig.phdrs],
                                     rebuilt=[{k: getattr(p, k) for k in keys} for p in new.phdrs])
    segs = []
    for p in orig.phdrs:
        a = orig.data[p.offset:p.offset + p.filesz]
        q = next((q for q in new.phdrs if q.type == p.type and q.vaddr == p.vaddr), None)
        b = new.data[q.offset:q.offset + q.filesz] if q else b""
        segs.append(dict(type=p.type, vaddr=f"0x{p.vaddr:08X}", filesz=p.filesz, identical=a == b,
                         diff_ranges=len(ranges(a, b))))
    report["segments"] = segs
    hdr = ("e_type", "e_machine", "e_version", "entry", "phoff", "shoff", "flags", "ehsize",
           "phentsize", "phnum", "shentsize", "shnum", "shstrndx")
    report["elf_header_differences"] = {k: [getattr(orig, k), getattr(new, k)] for k in hdr
                                        if getattr(orig, k) != getattr(new, k)}
    byname = {s.name: s for s in new.sections}
    fields = ("type", "flags", "addr", "offset", "size", "align", "entsize", "link", "info")
    secs = []
    for s in orig.sections:
        if not s.name:
            continue
        t = byname.get(s.name)
        if t is None:
            secs.append(dict(name=s.name, present=False))
            continue
        fd = {k: [getattr(s, k), getattr(t, k)] for k in fields if getattr(s, k) != getattr(t, k)}
        secs.append(dict(name=s.name, present=True, index=[s.index, t.index],
                         bytes_identical=orig.section_bytes(s) == new.section_bytes(t) if s.flags & 2 else None,
                         field_differences=fd))
    report["sections"] = secs
    report["extra_sections"] = [t.name for t in new.sections if t.name and t.name not in
                                {s.name for s in orig.sections}]
    alloc = [x for x in secs if x.get("bytes_identical") is not None]
    report["allocated_sections_bytes_identical"] = all(x["bytes_identical"] for x in alloc)
    report["allocated_section_fields_identical"] = all(not x["field_differences"] for x in alloc)
    return report


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--original", required=True, type=Path)
    ap.add_argument("--rebuilt", required=True, type=Path)
    ap.add_argument("--report", required=True, type=Path)
    ap.add_argument("--elf", action="store_true")
    a = ap.parse_args()
    orig = Elf(a.original.read_bytes())
    data = a.rebuilt.read_bytes()
    rep = compare_elf(orig, Elf(data)) if a.elf else compare_flat(orig, None, data)
    a.report.write_text(json.dumps(rep, indent=2) + "\n")
    summary = {k: rep[k] for k in rep if k in ("mode", "whole_file_identical", "rebuilt_sha256", "diff_range_count",
                                             "program_headers_identical", "allocated_sections_bytes_identical",
                                             "allocated_section_fields_identical")}
    print(json.dumps(summary))
    # Always exit 0 so the report is kept; the evidence tool reads the flags.
    return 0


if __name__ == "__main__":
    sys.exit(main())
