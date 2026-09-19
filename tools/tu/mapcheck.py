#!/usr/bin/env python3
"""Gate check of a main link against the TU manifest (placement + C-TU data audit).

    mapcheck.py --map build/main.rom.elf.map --report out.json [--carve] [--rom]

Placement (every build):
  * every TU piece (text, data, NOBITS) starts at its cut; scaffold pieces
    (INCLUDE_ASM/hand-asm objects, splat data objects) fill it exactly;
  * the COMMON-tail pieces (linker/scommon, linker/common), the fixed blobs
    (.reginfo/.ctors/.dtors/.eh_frame/.vudata/ov02 data) and, in a ROM-style
    link, the retail bin blobs sit at their manifest addresses with their sizes
    (tu-main-skel review R3);
  * scaffold code objects contribute .text only; nothing is allocated through
    *(COMMON)/*(.scommon).
C-TU audit (every C-mode TU; tu-main-skel review R1/R2):
  * a C-owned piece may end early only by alignment padding: the gap is
    smaller than the alignment of the next cut and no original symbol starts
    inside it;
  * the C object defines every original .symtab symbol that starts inside a
    C-owned piece, at the original final address, with the original size and
    binding (NOBITS pieces included);
  * a c_aliases.ld PROVIDE that the link uses for an original symbol of a
    C-owned piece fails unless the C object itself defines that symbol at the
    same address (PROVIDE may only re-export, never stand in for a definition);
  * every COMMON/SCOMMON symbol of a C object names an original COMMON-tail
    symbol (.scommon <-> .sbss tail, COMMON <-> .bss tail) with the original
    size and an alignment that the original address satisfies.
The whole-file hash remains the byte gate; this report is required clean too.
"""
import argparse
import json
import re
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
from elfinfo import Elf  # noqa: E402

SEC_RE = re.compile(r"^ (\.\w+|COMMON)\s*(?:\s+0x([0-9a-f]+)\s+0x([0-9a-f]+)\s+(\S+))?\s*$")
CONT_RE = re.compile(r"^\s+0x([0-9a-f]+)\s+0x([0-9a-f]+)\s+(\S+)\s*$")
PROVIDE_RE = re.compile(r"^\s+(\[!provide\]|0x[0-9a-f]+)\s+PROVIDE \(([^ =]+) = ")
SHN_COMMON, SHN_MIPS_SCOMMON, SHN_LORESERVE = 0xFFF2, 0xFF03, 0xFF00
LINKER_RESERVED = {"eprol", "etext", "_gp", "edata", "_fbss", "_fdata", "_ftext", "end", "_gp_disp"}


def parse(map_text):
    """-> ({(object, input_section): (addr, size)}, {provide_name: used_addr|None})"""
    out, provides = {}, {}
    lines = map_text.split("Linker script and memory map", 1)[1].splitlines()
    pending = None
    for line in lines:
        p = PROVIDE_RE.match(line)
        if p:
            provides[p[2].strip('"')] = None if p[1] == "[!provide]" else int(p[1], 16)
            continue
        m = SEC_RE.match(line)
        if m:
            if m[2]:
                out.setdefault((m[4], m[1]), (int(m[2], 16), int(m[3], 16)))
                pending = None
            else:
                pending = m[1]
            continue
        if pending:
            c = CONT_RE.match(line)
            if c:
                out.setdefault((c[3], pending), (int(c[1], 16), int(c[2], 16)))
            pending = None
    return out, provides


def align_of(addr, cap=128):
    a = addr & -addr if addr else cap
    return min(a, cap)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--map", type=Path, required=True)
    ap.add_argument("--report", type=Path, required=True)
    ap.add_argument("--manifest", type=Path, default=Path("tu-manifest.json"))
    ap.add_argument("--orig", type=Path, default=Path("orig/SLUS_204.69"))
    ap.add_argument("--carve", action="store_true", help="every TU from its INCLUDE_ASM skeleton")
    ap.add_argument("--rom", action="store_true", help="ROM-style link: also check the retail bin blobs")
    a = ap.parse_args()
    m = json.loads(a.manifest.read_bytes())
    placed, provides = parse(a.map.read_text())
    orig = Elf(a.orig.read_bytes())
    S = {x.name: x for x in orig.sections}
    problems, c_report = [], {}
    tus = m["tus"]

    def mode(t):
        return "asm" if a.carve else t["mode"]

    def check(objname, sec, start, end, exact, label):
        got = placed.get((objname, sec))
        if got is None:
            if end > start:
                problems.append(dict(kind="missing", object=objname, section=sec, piece=label))
            return None
        addr, size = got
        rec = dict(addr=f"0x{addr:08X}", size=size, expected_start=f"0x{start:08X}", expected_end=f"0x{end:08X}")
        if addr != start:
            problems.append(dict(kind="misplaced", object=objname, section=sec, piece=label, **rec))
        elif exact and size != end - start:
            problems.append(dict(kind="size", object=objname, section=sec, piece=label, **rec))
        elif size > end - start:
            problems.append(dict(kind="overflow", object=objname, section=sec, piece=label, **rec))
        rec["gap"] = end - start - size
        return rec

    checked = 0
    code_objs = set()
    for t in tus:
        if t.get("contract_override") and not a.carve:
            problems.append(dict(kind="TU built under a contract override", tu=t["name"], **t["contract_override"]))
    for t in tus:
        n = t["name"]
        c_obj = f"build/c/{n}.o"
        if t["text"]["splat"]:
            s, e = (int(x, 16) for x in t["text"]["splat_range"])
            if mode(t) == "c":
                c_report.setdefault(n, {})[".text"] = check(c_obj, ".text", s, e, False, n)
            else:
                o = f"build/expected/{n}.o" if t["text"]["splat"] == "c" else f"build/hasm/{n}.o"
                code_objs.add(o)
                check(o, ".text", s, e, True, n)
            checked += 1
        for sec, p in t["sections"].items():
            owner = t["owners"].get(sec)
            for pc in p.get("pieces", [dict(name=n, start=p["start"], end=p["end"], c_split=False)]):
                s, e = int(pc["start"], 16), int(pc["end"], 16)
                own = "c" if mode(t) == "c" and (owner == "c" or (owner == "split" and pc["c_split"])) else "asm"
                if own == "c":
                    c_report.setdefault(n, {})[sec] = check(c_obj, sec, s, e, False, pc["name"])
                else:
                    check(f"build/data/{pc['name']}.{sec[1:]}.o", sec, s, e, True, pc["name"])
                checked += 1
        if mode(t) == "c":
            for (o, sec), (addr, size) in placed.items():
                if o == c_obj and size and t["owners"].get(sec) not in ("c", "split") and sec != "COMMON":
                    problems.append(dict(kind="unowned C contribution", object=o, section=sec, size=size,
                                         addr=f"0x{addr:08X}"))
    for (o, sec), (addr, size) in placed.items():
        if o in code_objs and sec != ".text" and size:
            problems.append(dict(kind="scaffold object data contribution", object=o, section=sec, size=size,
                                 addr=f"0x{addr:08X}"))
        if sec in ("COMMON", ".scommon") and size:
            problems.append(dict(kind="COMMON allocation", object=o, section=sec, size=size, addr=f"0x{addr:08X}"))

    # ---- R3: COMMON tails, fixed blobs, retail bins
    for sec, v in m["sections"].items():
        tail = v.get("common_tail")
        if tail:
            check(f"build/data/{tail['name']}.{sec[1:]}.o", sec, int(tail["start"], 16), int(tail["end"], 16), True,
                  tail["name"])
            checked += 1
    # One object per fixed blob.  A blob whose original section holds more than one
    # (.vudata: the recovered VU0 packet and the VU1 remainder) names its own object
    # and that object's input section in the manifest; the rest are one per section.
    blob_obj = {".reginfo": "build/data/blobs/reginfo.o", ".ctors": "build/data/blobs/ctors.o",
                ".dtors": "build/data/blobs/dtors.o", ".eh_frame": "build/data/blobs/eh_frame.o",
                ".vudata": "build/data/blobs/vudata.o"}
    for b in m["fixed_blobs"]:
        va = int(b["va"], 16)
        check(b.get("object") or blob_obj[b["section"]], b.get("object_section", ".data"),
              va, va + b["size"], True, b["name"])
        checked += 1
    ov = S["ov02"]
    ov_text_end = max(int(t["text"]["splat_range"][1], 16) for t in tus if t["text"].get("section") == "ov02")
    check("build/ov02/data.o", ".data", ov_text_end, ov.addr + ov.size - ov.size % 4, True, "ov02/data")
    checked += 1
    if a.rom:
        unit = a.manifest.resolve().parent
        for line in (unit / "splat.yaml").read_text().splitlines():
            mm = re.match(r"  - \[0x([0-9A-F]+), bin, blobs/(\w+)\]", line)
            if mm:
                off = int(mm[1], 16)
                size = (unit / f"assets/main/blobs/{mm[2]}.bin").stat().st_size
                check(f"build/assets/{mm[2]}.o", ".data", 0x10000000 + off, 0x10000000 + off + size, True, mm[2])
                checked += 1

    # ---- R1/R2: C-TU symbol and data audit
    audit = {}
    osyms = [s for s in orig.symbols if s.name and s.type in (0, 1, 2) and 0 < s.shndx < SHN_LORESERVE
             and s.name not in LINKER_RESERVED]
    sec_by_index = {x.index: x.name for x in orig.sections}
    tails = {sec: (int(v["common_tail"]["start"], 16), int(v["common_tail"]["end"], 16))
             for sec, v in m["sections"].items() if v.get("common_tail")}
    tail_syms = {s.name: s for s in osyms for sec, (lo, hi) in tails.items()
                 if sec_by_index[s.shndx] == sec and lo <= s.value < hi}
    # Object starts inside another TU's .text piece.  A TU without functions whose
    # zero text bytes are carried as trailing padding of the previous TU
    # (empty_002f0c64 after jni: `splat_range` of jni runs to 0x002F0C68) has no
    # piece of its own, yet its compiler stamps (`gcc2_compiled.`,
    # `__gnu_compiled_c`) sit at its own start inside that padding.  Those
    # symbols belong to the next object, not to the C TU whose piece carries
    # the padding, so the symbol audit of a C piece stops at the first other
    # object start it contains.
    text_starts = sorted((int(t["text"]["start"], 16), t["name"]) for t in tus if t["text"].get("start"))
    for t in tus:
        if mode(t) != "c":
            continue
        n = t["name"]
        c_obj = f"build/c/{n}.o"
        elf = Elf((a.manifest.resolve().parent / c_obj).read_bytes())
        csec = {x.index: x.name for x in elf.sections}
        cdefs = {}
        commons = []
        for s in elf.symbols:
            if not s.name or s.type in (3, 4):
                continue
            if s.shndx in (SHN_COMMON, SHN_MIPS_SCOMMON):
                commons.append(s)
            elif 0 < s.shndx < SHN_LORESERVE:
                secname = csec[s.shndx]
                base = placed.get((c_obj, secname))
                if base:
                    cdefs.setdefault(s.name, []).append(dict(addr=base[0] + s.value, size=s.size, bind=s.bind,
                                                             section=secname))
        tu_audit = dict(symbols_checked=0, commons=[], pieces={})
        owned = {}
        for sec, p in t["sections"].items():
            owner = t["owners"].get(sec)
            for pc in p.get("pieces", [dict(start=p["start"], end=p["end"], c_split=False)]):
                if owner == "c" or (owner == "split" and pc["c_split"]):
                    owned[sec] = (int(pc["start"], 16), int(pc["end"], 16))
        if t["text"]["splat"]:
            owned[".text"] = tuple(int(x, 16) for x in t["text"]["splat_range"])
        for sec, (lo, hi) in owned.items():
            pl = placed.get((c_obj, sec))
            c_end = pl[0] + pl[1] if pl else lo
            gap = hi - c_end
            own_hi, carried = hi, None
            if sec == ".text":
                carried = next(((va, name) for va, name in text_starts if lo < va < hi and name != n), None)
                if carried:
                    own_hi = carried[0]
            inside = [s for s in osyms if sec_by_index[s.shndx] == sec and lo <= s.value < own_hi]
            piece = dict(start=f"0x{lo:08X}", end=f"0x{hi:08X}", c_end=f"0x{c_end:08X}", gap=gap,
                         original_symbols=len(inside))
            if carried:
                piece["carried_object"] = dict(
                    tu=carried[1], start=f"0x{carried[0]:08X}",
                    symbols=[dict(symbol=s.name, va=f"0x{s.value:08X}") for s in osyms
                             if sec_by_index[s.shndx] == sec and carried[0] <= s.value < hi])
            if gap < 0 or (gap > 0 and gap >= align_of(hi, 16 if sec != ".text" else 8)):
                problems.append(dict(kind="C piece gap is not alignment padding", tu=n, section=sec, **piece))
            for s in inside:
                # .text: the strict rule covers functions and the compiler markers; other
                # original labels retained inside a scaffold body (asm loop labels like
                # `_$mc64_loop`, and $L labels) are scaffold facts, not C definitions
                if sec == ".text" and s.type == 0 and s.name not in ("gcc2_compiled.", "__gnu_compiled_c") \
                        and not any(d["addr"] == s.value for d in cdefs.get(s.name, [])):
                    tu_audit.setdefault("scaffold_labels", []).append(dict(symbol=s.name, va=f"0x{s.value:08X}"))
                    continue
                if c_end <= s.value < hi:
                    problems.append(dict(kind="original symbol inside the gap of a C-owned piece", tu=n,
                                         section=sec, symbol=s.name, va=f"0x{s.value:08X}"))
                    continue
                tu_audit["symbols_checked"] += 1
                got = [d for d in cdefs.get(s.name, []) if d["addr"] == s.value]
                if not got:
                    alt = cdefs.get(s.name)
                    problems.append(dict(kind="original symbol not defined by the C object", tu=n, section=sec,
                                         symbol=s.name, va=f"0x{s.value:08X}",
                                         c_definitions=[dict(d, addr=f"0x{d['addr']:08X}") for d in alt or []]))
                    continue
                d = got[0]
                if (s.size and d["size"] != s.size) or d["bind"] != s.bind:
                    problems.append(dict(kind="C symbol differs from original", tu=n, section=sec, symbol=s.name,
                                         va=f"0x{s.value:08X}", original=dict(size=s.size, bind=s.bind),
                                         c=dict(size=d["size"], bind=d["bind"])))
                used = provides.get(s.name)
                if used is not None and used != s.value:
                    problems.append(dict(kind="c_aliases PROVIDE used for an original symbol at a different address",
                                         tu=n, symbol=s.name, provide=f"0x{used:08X}"))
            tu_audit["pieces"][sec] = piece
        # PROVIDEs used for original symbols of this TU's C-owned pieces that the C object does not define
        for name, used in provides.items():
            if used is None:
                continue
            for sec, (lo, hi) in owned.items():
                if lo <= used < hi and any(s.name == name and s.value == used for s in osyms) and \
                        not any(d["addr"] == used for d in cdefs.get(name, [])):
                    problems.append(dict(kind="c_aliases PROVIDE stands in for a missing C definition", tu=n,
                                         symbol=name, va=f"0x{used:08X}"))
        # R1: tentative definitions
        for s in commons:
            kind = ".sbss" if s.shndx == SHN_MIPS_SCOMMON else ".bss"
            o = tail_syms.get(s.name)
            rec = dict(symbol=s.name, section=kind, size=s.size, align=s.value)
            tu_audit["commons"].append(rec)
            if o is None:
                problems.append(dict(kind="COMMON symbol is not an original COMMON-tail symbol", tu=n, **rec))
            elif sec_by_index[o.shndx] != kind or o.size != s.size or (s.value and o.value % s.value):
                problems.append(dict(kind="COMMON symbol differs from the original tail symbol", tu=n, **rec,
                                     original=dict(section=sec_by_index[o.shndx], size=o.size, va=f"0x{o.value:08X}")))
        audit[n] = tu_audit
    rep = dict(map=str(a.map), carve=a.carve, rom=a.rom, pieces_checked=checked, ok=not problems,
               problems=problems, c_tus=c_report, c_tu_audit=audit)
    a.report.write_text(json.dumps(rep, indent=1) + "\n")
    print(json.dumps(dict(ok=rep["ok"], pieces_checked=checked, problems=len(problems),
                          c_symbols_checked=sum(v["symbols_checked"] for v in audit.values()))))


if __name__ == "__main__":
    main()
