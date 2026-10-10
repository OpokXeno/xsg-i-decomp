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
  * scaffold code objects contribute .text only; tentative definitions stay
    unallocated unless a C-owned BSS carve has passed the exact-span proof.
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
  * a differently named initialized C owner may cover an original same-address
    OBJECT alias only when address, size, binding, ELF type and complete linked
    owner extent all agree; the explicit manifest alias row pins that mapping;
  * each allocated .scommon input from a C object maps to one exact C-owned
    .sbss manifest span; an unowned or partial allocation remains an error.
    Raw COMMON symbols continue through the existing tail audit.
The whole-file hash remains the byte gate; this report is required clean too.
"""
import argparse
import hashlib
import json
import re
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
from elfinfo import Elf  # noqa: E402

# input section names include the split runs `.rodata.carve.<k>` (tools/tu/data_carve.py)
SEC_RE = re.compile(r"^ (\.\w+(?:\.carve\.\d+)?|COMMON)\s*(?:\s+0x([0-9a-f]+)\s+0x([0-9a-f]+)\s+(\S+))?\s*$")
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


def original_zero_fill(orig, sec, lo, hi):
    """True when the original image holds only zero bytes in [lo, hi) of `sec`."""
    for x in orig.sections:
        if x.name == sec and x.type != 8 and x.addr <= lo and hi <= x.addr + x.size:
            off = x.offset + lo - x.addr
            return not any(orig.data[off:off + hi - lo])
    return False


def byte_array_member(source, owner, member):
    """Layout of a member in a C record containing only character arrays.

    These arrays have alignment one on the EE ABI. Reject any other field,
    attribute, macro extent or ambiguous declaration rather than guessing.
    """
    text = re.sub(r'/\*.*?\*/|//[^\n]*', '', source, flags=re.S)
    declarations = re.findall(r'\bstatic\s+(\w+)\s+' + re.escape(owner) + r'\s*[;=]', text)
    if len(set(declarations)) != 1:
        return None
    typename = declarations[0]
    records = re.findall(r'\btypedef\s+struct(?:\s+\w+)?\s*\{([^{}]*)\}\s*'
                         + re.escape(typename) + r'\s*;', text)
    if len(records) != 1:
        return None
    fields = records[0].split(';')
    offset, match = 0, None
    for field in fields:
        if not field.strip():
            continue
        parsed = re.fullmatch(r'\s*((?:unsigned\s+|signed\s+)?char)\s+(\w+)\s*'
                              r'\[\s*([1-9][0-9]*)\s*\]\s*', field)
        if not parsed:
            return None
        size = int(parsed[3])
        if parsed[2] == member:
            if match is not None:
                return None
            match = dict(member=member, offset=offset, size=size, type=parsed[1])
        offset += size
    return dict(match, owner_size=offset) if match else None


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
    linked_path = a.map.resolve().with_suffix("")
    linked = Elf(linked_path.read_bytes()) if linked_path.is_file() else None
    linked_sections = {x.index: x for x in linked.sections} if linked else {}
    scaffold_cache = {}
    problems, c_report = [], {}
    tus = m["tus"]

    def mode(t):
        return "asm" if a.carve else t["mode"]

    def c_owned_piece(t, sec, piece):
        owner = t['owners'].get(sec)
        return mode(t) == 'c' and (owner == 'c' or (owner == 'split' and piece.get('c_split'))
                                  or (piece.get('c_split') and piece.get('c_section')
                                      and piece.get('c_input_spans')))

    def c_pieces(t):
        """Explicit compiler inputs, including independently owned COMMON tails."""
        for sec, section in t['sections'].items():
            for piece in section.get('pieces', [dict(name=t['name'], start=section['start'],
                                                     end=section['end'], c_split=False)]):
                if c_owned_piece(t, sec, piece):
                    yield sec, piece
        if mode(t) != 'c':
            return
        obj = t.get('c_link_object') or f"build/c/{t['name']}.o"
        for sec, section in m['sections'].items():
            for piece in (section.get('common_tail') or {}).get('native_common_pieces', []):
                if piece['kind'] == 'c' and piece['object'] == obj:
                    yield sec, dict(name=piece['name'], start=piece['start'], end=piece['end'],
                                    c_owned_symbols=[piece['name']],
                                    c_input_spans=[dict(section=piece['input_section'],
                                                       range=[piece['start'], piece['end']],
                                                       symbols=[piece['name']])])

    def linked_up_to_padding(link, span):
        """The linked C section fills its owned span, up to original zero alignment.

        A C run that absorbed the alignment pads after it (data_carve.absorb_padding)
        ends where its C content ends; the rest of the span is zero fill in the
        original, shorter than the alignment of the span end (the C piece gap rule)."""
        sec, lo, hi = span
        link_end = link[0] + link[1]
        gap = hi - link_end
        return gap == 0 or (0 < gap < align_of(hi, 16) and original_zero_fill(orig, sec, link_end, hi))

    def check(objname, sec, start, end, exact, label, scaffold_alignment=False):
        got = placed.get((objname, sec))
        if got is None:
            if end > start:
                problems.append(dict(kind="missing", object=objname, section=sec, piece=label))
            return None
        addr, size = got
        rec = dict(addr=f"0x{addr:08X}", size=size, expected_start=f"0x{start:08X}", expected_end=f"0x{end:08X}")
        leading_alignment = False
        if scaffold_alignment and start < addr <= end and size == end - addr:
            path = a.manifest.resolve().parent / objname
            if path.is_file():
                input_section = Elf(path.read_bytes()).section(sec)
                leading_alignment = (addr == (start + input_section.align - 1) // input_section.align
                                     * input_section.align
                                     and original_zero_fill(orig, sec, start, addr))
        if leading_alignment:
            rec['leading_scaffold_alignment'] = dict(start=f'0x{start:08X}', size=addr - start)
        elif addr != start:
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
        # a TU with several C runs in one data section links its split object, whose
        # run k > 0 is the input section `<sec>.carve.<k>` (tools/tu/data_carve.py)
        c_obj = t.get("c_link_object") or f"build/c/{n}.o"
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
                own = "c" if c_owned_piece(t, sec, pc) else "asm"
                if own == "c" and pc.get("c_input_spans"):
                    for part in pc["c_input_spans"]:
                        csec = part["section"]
                        lo, hi = (int(x, 16) for x in part["range"])
                        c_report.setdefault(n, {})[csec] = check(c_obj, csec, lo, hi, True, pc["name"])
                elif own == "c":
                    csec = pc.get("c_section") or (".scommon" if sec == ".sbss" else sec)
                    c_report.setdefault(n, {})[csec] = check(c_obj, csec, s, e, False, pc["name"])
                else:
                    check(f"build/data/{pc['name']}.{sec[1:]}.o", sec, s, e, True, pc["name"],
                          scaffold_alignment=True)
                checked += 1

    def allocated_bss_owner(obj, sec, addr, size):
        """Prove one allocated NOBITS input maps to an exact C-owned manifest span."""
        base_sec = sec.split('.carve.', 1)[0]
        original_sec = ".sbss" if base_sec == ".scommon" else base_sec
        if original_sec not in (".sbss", ".bss"):
            return None
        for t in tus:
            if mode(t) != "c" or obj != (t.get("c_link_object") or f"build/c/{t['name']}.o"):
                continue
            for family, piece in c_pieces(t):
                if family != original_sec:
                    continue
                if piece.get("c_input_spans"):
                    for part in piece["c_input_spans"]:
                        if part["section"] != sec:
                            continue
                        lo, hi = (int(x, 16) for x in part["range"])
                        if addr == lo and size == hi - lo:
                            return dict(tu=t["name"], original_section=original_sec,
                                        compiler_section=sec, start=lo, end=hi)
                else:
                    lo, hi = int(piece["start"], 16), int(piece["end"], 16)
                    csec = piece.get("c_section") or (".scommon" if original_sec == ".sbss" else original_sec)
                    if sec == csec and addr == lo and size == hi - lo:
                        return dict(tu=t["name"], original_section=original_sec,
                                    compiler_section=sec, start=lo, end=hi)
        return None

    tu_by_c_object = {(t.get("c_link_object") or f"build/c/{t['name']}.o"): t for t in tus}

    def stray_common_origin(obj, sec):
        """The TU and the symbols of one unplaced C COMMON/.scommon/NOBITS input."""
        t = tu_by_c_object.get(obj)
        path = a.manifest.resolve().parent / obj
        symbols = []
        if path.is_file():
            elf = Elf(path.read_bytes())
            names = {x.index: x.name for x in elf.sections}
            for s in elf.symbols:
                if not s.name or s.type in (3, 4):
                    continue
                common = s.shndx == (SHN_MIPS_SCOMMON if sec == ".scommon" else SHN_COMMON)
                if common or (0 < s.shndx < SHN_LORESERVE and names.get(s.shndx) == sec):
                    symbols.append(s.name)
        return dict(tu=(t or {}).get("id"), tu_name=(t or {}).get("name"), symbols=sorted(symbols))

    for (o, sec), (addr, size) in placed.items():
        if o in code_objs and sec != ".text" and size:
            problems.append(dict(kind="scaffold object data contribution", object=o, section=sec, size=size,
                                 addr=f"0x{addr:08X}"))
        if (sec in ("COMMON", ".scommon") or (o.startswith("build/c/") and sec in (".sbss", ".bss"))) and size:
            owner = allocated_bss_owner(o, sec, addr, size)
            if owner:
                c_report.setdefault(owner["tu"], {}).setdefault(sec, dict(start=f"0x{addr:08X}", size=size,
                                                                          owner=owner))
            else:
                # A C tentative definition with no explicit place (the link's
                # stray_common_guard normally stops it first): name its TU and symbols.
                origin = stray_common_origin(o, sec) if o.startswith("build/c/") else {}
                problems.append(dict(kind="COMMON allocation", object=o, section=sec, size=size,
                                     addr=f"0x{addr:08X}", reason="no exact C-owned original BSS span",
                                     **origin))

    # ---- R3: COMMON tails, fixed blobs, retail bins
    for sec, v in m["sections"].items():
        tail = v.get("common_tail")
        if tail:
            native = tail.get('native_common_pieces') if not a.carve else None
            if native:
                for piece in native:
                    input_sec = piece['input_section'] if piece['kind'] == 'c' else sec
                    check(piece['object'], input_sec, int(piece['start'], 16), int(piece['end'], 16),
                          True, piece['name'], scaffold_alignment=piece['kind'] == 'scaffold')
                    checked += 1
            else:
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

    def scaffold_common_resolution(t, c_obj, common, kind):
        """Prove a raw tentative C definition is overridden by exact ASM storage.

        This is uncredited scaffold ownership. It is accepted only when the
        original TU manifest leaves the section ASM-owned, its placed scaffold
        object fills that exact span, the object carries a same-name strong
        object and an exact-size-or-larger .NON_MATCHING witness, and the final
        linked name resolves to that scaffold address without allocating the
        corresponding C COMMON input section.
        """
        sec = ".sbss" if kind == ".sbss" else ".bss"
        if t.get("owners", {}).get(sec) != "asm":
            return None
        section_record = t.get("sections", {}).get(sec)
        if not section_record:
            return None
        pieces = section_record.get("pieces", [])
        if len(pieces) != 1 or pieces[0].get("c_split"):
            return None
        lo, hi = int(pieces[0]["start"], 16), int(pieces[0]["end"], 16)
        data_obj = f"build/data/{t['name']}.{sec[1:]}.o"
        data_sec = placed.get((data_obj, sec))
        if data_sec != (lo, hi - lo):
            return None
        data_path = a.manifest.resolve().parent / data_obj
        if not data_path.is_file():
            return None
        if data_obj not in scaffold_cache:
            scaffold_cache[data_obj] = Elf(data_path.read_bytes())
        data_elf = scaffold_cache[data_obj]
        data_sections = {x.index: x.name for x in data_elf.sections}
        defs = [x for x in data_elf.symbols
                if x.name == common.name and x.bind == 1 and x.type == 1
                and data_sections.get(x.shndx) == sec]
        witnesses = [x for x in data_elf.symbols
                     if x.name == common.name + ".NON_MATCHING" and x.bind == 1 and x.type == 1
                     and data_sections.get(x.shndx) == sec]
        match = next(((d, w) for d in defs for w in witnesses
                      if d.value == w.value and w.size >= common.size), None)
        if not match:
            return None
        definition, witness = match
        expected_va = data_sec[0] + definition.value
        common_input_sec = "COMMON" if kind == ".bss" else ".scommon"
        common_contribution = placed.get((c_obj, common_input_sec))
        if common_contribution and common_contribution[1] != 0:
            return None
        if not linked:
            return None
        final = [s for s in linked.symbols if s.name == common.name and s.value == expected_va
                 and s.bind == 1 and s.type == 1
                 and linked_sections.get(s.shndx) and linked_sections[s.shndx].name == sec]
        if not final:
            return None
        return dict(symbol=common.name, section=sec, size=common.size, alignment=common.value,
                    common_object=c_obj, common_section=common_input_sec,
                    common_contribution=({"start": f"0x{common_contribution[0]:08X}",
                                          "size": common_contribution[1]}
                                         if common_contribution else None),
                    scaffold_object=data_obj, scaffold_span_start=f"0x{lo:08X}",
                    scaffold_span_end=f"0x{hi:08X}", scaffold_symbol_offset=definition.value,
                    scaffold_witness_size=witness.size, linked_address=f"0x{expected_va:08X}",
                    linked_symbol_size=final[0].size, credited_as_c=False)

    nested_cache = {}

    def nested_function_identity(t, s, cdefs):
        """A registered GNU nested function whose cc1 counter differs from the original.

        cc1 names every nested function (and function-local static) `name.N`, N
        being the count of such private names emitted before it in the TU. While
        functions that own earlier private names are still INCLUDE_ASM, the C
        object numbers the nested function lower than the original. The identity
        is the registered gnu-nested-function mapping (config/gnu-nested-functions.json
        and config/tu/source-classes.json, the mapping tools/tu_audit.py checks):
        accept exactly one LOCAL FUNC `source_name.M` of the C object at the
        original address with the original size, and report both names."""
        root = a.manifest.resolve().parent.parent.parent
        tu_id = t.get("id")
        if tu_id not in nested_cache:
            sys.path.insert(0, str(root / "tools"))
            import tu_edit
            nested_cache[tu_id] = tu_edit.gnu_nested_specs(root, tu_id)
        for parent, spec in nested_cache[tu_id].items():
            for item in spec.get("nested_functions") or []:
                if (item.get("original_name") != s.name or int(item.get("va", "0"), 16) != s.value
                        or item.get("size") != s.size or item.get("binding") != "LOCAL"
                        or item.get("type") != "FUNC"):
                    continue
                pattern = re.escape(item["source_name"]) + r"\.[0-9]+"
                found = [(name, d) for name, defs in cdefs.items() if re.fullmatch(pattern, name)
                         for d in defs if d["addr"] == s.value]
                if (len(found) == 1 and found[0][1]["size"] == s.size and found[0][1]["bind"] == 0
                        and found[0][1]["type"] == 2 and found[0][1]["section"] == ".text"):
                    return dict(original_symbol=s.name, original_va=f"0x{s.value:08X}", size=s.size,
                                parent=parent, source_name=item["source_name"],
                                c_symbol=found[0][0], c_address=f"0x{found[0][1]['addr']:08X}")
        return None

    for t in tus:
        if mode(t) != "c":
            continue
        n = t["name"]
        c_obj = t.get("c_link_object") or f"build/c/{n}.o"
        c_obj_path = a.manifest.resolve().parent / c_obj
        c_obj_bytes = c_obj_path.read_bytes()
        c_obj_sha256 = hashlib.sha256(c_obj_bytes).hexdigest()
        elf = Elf(c_obj_bytes)
        csec = {x.index: x.name for x in elf.sections}
        csections = {x.name: x for x in elf.sections}
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
                                                             type=s.type, section=secname))
        tu_audit = dict(symbols_checked=0, commons=[], pieces={}, nobits_storage_proofs=[],
                        commons_resolved_by_scaffold=[])
        proven_aliases = set()
        assembly_path = a.manifest.resolve().parent / f'build/c/{n}.o.s'
        native_storage = {}
        if assembly_path.is_file():
            from data_gate import emitted_storage
            native_storage = emitted_storage(assembly_path.read_text())
        owned = {}        # input section of the C object -> (original section, lo, hi)
        storage_symbols = {}  # cc1-proven local/global NOBITS identities -> exact input span
        storage_aliases = {}  # packet-backed original labels -> explicit C storage owner
        c_owned = {".bss": set(), ".sbss": set()}
        for sec, pc in c_pieces(t):
            c_owned.setdefault(sec, set()).update(pc.get("c_owned_symbols", []))
            if pc.get("c_input_spans"):
                for part in pc["c_input_spans"]:
                    csec = part["section"]
                    lo, hi = (int(x, 16) for x in part["range"])
                    owned[csec] = (sec, lo, hi)
                    for name in part.get("symbols", []):
                        storage_symbols[name] = (csec, lo, hi)
            else:
                csec = pc.get("c_section") or (".scommon" if sec == ".sbss" else sec)
                owned[csec] = (sec, int(pc["start"], 16), int(pc["end"], 16))
            for alias in pc.get("c_storage_aliases", []):
                storage_aliases[alias["original_name"]] = alias
        if t["text"]["splat"]:
            owned[".text"] = (".text",) + tuple(int(x, 16) for x in t["text"]["splat_range"])
        for (o, sec), (addr, size) in placed.items():
            if o == c_obj and size and sec not in owned and sec != 'COMMON':
                problems.append(dict(kind='unowned C contribution', object=o, section=sec,
                                     size=size, addr=f'0x{addr:08X}'))
        for csec_name, (sec, lo, hi) in owned.items():
            pl = placed.get((c_obj, csec_name))
            c_end = pl[0] + pl[1] if pl else lo
            gap = hi - c_end
            if sec in (".bss", ".sbss") and pl:
                input_sec = csections.get(csec_name)
                if (pl[0] != lo or pl[1] != hi - lo or not input_sec or input_sec.type != 8
                        or lo % max(1, input_sec.align)):
                    problems.append(dict(kind="C NOBITS input section does not fill its proven span", tu=n,
                                         section=sec, compiler_section=csec_name,
                                         expected_start=f"0x{lo:08X}", expected_size=hi - lo,
                                         compiler_section_type=input_sec.type if input_sec else None,
                                         compiler_align=input_sec.align if input_sec else None,
                                         linked_start=f"0x{pl[0]:08X}", linked_size=pl[1]))
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
                # A .text piece that ends on a 16-byte boundary may carry up to
                # 15 bytes of zero fill in the original (the next object is
                # 16-aligned); that is alignment padding exactly when the original
                # bytes of the gap are all zero. An original symbol inside the gap
                # is still reported below, and the whole-file gate compares the bytes.
                if sec == ".text" and 0 < gap < align_of(hi, 16) and original_zero_fill(orig, sec, c_end, hi):
                    piece["zero_fill_16"] = True
                else:
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
                if not got and sec == ".text" and s.type == 2 and s.bind == 0:
                    proof = nested_function_identity(t, s, cdefs)
                    if proof:
                        tu_audit.setdefault("nested_function_identity_proofs", []).append(proof)
                        tu_audit["symbols_checked"] += 1
                        continue
                if not got:
                    alias = storage_aliases.get(s.name)
                    storage_owner = alias.get("storage_owner") if alias else None
                    owner_name = storage_owner.get("name") if storage_owner else None
                    alias_owner = cdefs.get(owner_name, []) if owner_name else []
                    alias_va = int(alias["address"], 16) if alias else None
                    owner_va = int(storage_owner["address"], 16) if storage_owner else None
                    owner_size = int(storage_owner["size"]) if storage_owner else None
                    alias_def = next((x for x in alias_owner if x["addr"] == owner_va), None)
                    alias_storage = storage_symbols.get(owner_name) if owner_name else None
                    alias_link = placed.get((c_obj, alias_storage[0])) if alias_storage else None
                    native_extent = native_storage.get(owner_name, {})
                    native_alias = bool(
                        sec in ('.bss', '.sbss') and alias_def and s.type == 0 and s.size == 0
                        and alias_def['type'] == 0 and alias_def['size'] == 0
                        and alias_def['bind'] == s.bind == 0
                        and native_extent.get('extent_kind') == 'label-space'
                        and native_extent.get('size') == owner_size
                        and alias_storage and csections[alias_storage[0]].type == 8)
                    typed_alias = bool(alias_def and alias_def['size'] == owner_size
                                       and alias_def['type'] == 1)
                    if (sec in ('.bss', '.sbss') and alias and alias_va == s.value
                            and owner_size and owner_size > 0
                            and owner_name in c_owned.get(sec, set())
                            and (typed_alias or native_alias)
                            and alias_storage and alias_storage[1] <= owner_va
                            and owner_va + owner_size <= alias_storage[2]
                            and owner_va <= alias_va < owner_va + owner_size
                            and alias_link and alias_link[0] == alias_storage[1]
                            and alias_link[1] == alias_storage[2] - alias_storage[1]
                            and alias_storage[1] % max(1, csections[alias_storage[0]].align) == 0):
                        tu_audit["nobits_storage_proofs"].append(dict(
                            symbol=s.name, original_va=f"0x{s.value:08X}",
                            storage_owner=owner_name, storage_owner_address=f"0x{owner_va:08X}",
                            storage_owner_size=owner_size,
                            compiler_section=alias_storage[0], compiler_address=f"0x{alias_def['addr']:08X}",
                            compiler_binding=alias_def["bind"], compiler_type=alias_def["type"],
                            span_start=f"0x{alias_storage[1]:08X}", span_end=f"0x{alias_storage[2]:08X}",
                            linked_section_start=f"0x{alias_link[0]:08X}", linked_section_size=alias_link[1],
                            alias_record=alias, native_local=native_alias,
                            compiler_storage=native_extent if native_alias else None))
                        proven_aliases.add(s.name)
                        continue
                    # Initialized same-address aliases are accepted only when
                    # the named C owner is the exact meaningful object identity
                    # from this original symbol: address, size, binding and ELF
                    # type all agree, and its complete emitted extent is inside
                    # the linked C-owned section. The linker PROVIDE only keeps
                    # scaffold references resolvable; it is not the identity proof.
                    if sec not in (".bss", ".sbss"):
                        owner_definitions = cdefs.get(owner_name, []) if owner_name else []
                        owner_def = next((x for x in owner_definitions
                                          if x["addr"] == owner_va and x["size"] == owner_size
                                          and x["bind"] == s.bind and x["type"] == s.type), None)
                        owner_section = owner_def["section"] if owner_def else None
                        owner_span = owned.get(owner_section) if owner_section else None
                        owner_link = placed.get((c_obj, owner_section)) if owner_section else None
                        source_path = a.manifest.resolve().parent.parent.parent / t.get('path', '')
                        member = (byte_array_member(source_path.read_text(), owner_name, alias['storage_member'])
                                  if alias and alias.get('storage_member') and source_path.is_file() else None)
                        exact_object = owner_va == s.value and owner_size == s.size
                        exact_member = bool(member and member['owner_size'] == owner_size
                                            and owner_va + member['offset'] == s.value
                                            and member['size'] == s.size)
                        if (alias and alias_va == s.value and owner_size
                                and (exact_object or exact_member)
                                and owner_name in c_owned.get(sec, set())
                                and s.type == 1 and owner_def and owner_span
                                and owner_span[0] == sec and owner_span[1] <= owner_va
                                and owner_va + owner_size <= owner_span[2]
                                and owner_link and owner_link[0] == owner_span[1]
                                and owner_va + owner_size <= owner_link[0] + owner_link[1]
                                and linked_up_to_padding(owner_link, owner_span)):
                            proven_aliases.add(s.name)
                            source_sha256 = (hashlib.sha256(source_path.read_bytes()).hexdigest()
                                             if source_path.is_file() else None)
                            tu_audit.setdefault("initialized_symbol_alias_proofs", []).append(dict(
                                original_symbol=s.name, original_address=f"0x{s.value:08X}",
                                original_size=s.size, original_binding=s.bind, original_type=s.type,
                                c_owner=owner_name, c_owner_address=f"0x{owner_va:08X}",
                                c_owner_size=owner_size, c_owner_binding=owner_def["bind"],
                                c_owner_type=owner_def["type"], c_owner_section=owner_section,
                                linked_section_start=f"0x{owner_link[0]:08X}",
                                linked_section_size=owner_link[1], alias_record=alias,
                                storage_member_proof=member if exact_member else None,
                                source_path=str(source_path) if source_sha256 else None,
                                source_sha256=source_sha256, c_object_path=str(c_obj_path),
                                c_object_sha256=c_obj_sha256))
                            continue
                    alt = cdefs.get(s.name)
                    problems.append(dict(kind="original symbol not defined by the C object", tu=n, section=sec,
                                         symbol=s.name, va=f"0x{s.value:08X}",
                                         c_definitions=[dict(d, addr=f"0x{d['addr']:08X}") for d in alt or []]))
                    continue
                d = got[0]
                # Original NOBITS labels can be local NOTYPE symbols with size 0.
                # In that case the ELF provides only the name and address; the
                # C object plus its exact linked manifest span proves storage and
                # extent. Do not compare invented original size/binding metadata.
                nobits = sec in (".bss", ".sbss")
                proven_nobits_object = False
                if nobits and s.type == 0 and s.size == 0:
                    claim = storage_symbols.get(s.name)
                    csec = claim[0] if claim else (".scommon" if sec == ".sbss" else ".bss")
                    owner = owned.get(csec)
                    pl = placed.get((c_obj, csec))
                    input = csections.get(csec)
                    # Traditional allocated COMMON/SCOMMON objects are typed,
                    # sized globals. Native static storage may instead be a
                    # cc1-emitted LOCAL NOTYPE label over .space; accept that
                    # only when an explicit c_input_spans record names it and
                    # the complete NOBITS input section exactly fills its span.
                    typed_global = bool(
                        owner and owner[0] == sec and owner[1] <= s.value < owner[2]
                        and d["section"] == csec and d["type"] == 1 and d["bind"] == 1
                        and d["size"] > 0 and pl and pl[0] == owner[1]
                        and pl[1] == owner[2] - owner[1]
                    )
                    native_local = bool(
                        claim and s.name in c_owned.get(sec, set())
                        and owner and owner[0] == sec and owner[1] <= s.value < owner[2]
                        and d["section"] == csec and d["bind"] == s.bind and d["type"] == 0
                        and d["size"] == 0 and d["addr"] == s.value
                        and pl and pl[0] == owner[1] and pl[1] == owner[2] - owner[1]
                        and input and owner[1] % max(1, input.align) == 0
                    )
                    proven_nobits_object = typed_global or native_local
                    if proven_nobits_object:
                        tu_audit["nobits_storage_proofs"].append(dict(
                            symbol=s.name, original_va=f"0x{s.value:08X}",
                            compiler_section=csec, compiler_size=d["size"],
                            manifest_section=sec, span_start=f"0x{owner[1]:08X}",
                            span_end=f"0x{owner[2]:08X}", linked_section_start=f"0x{pl[0]:08X}",
                            linked_section_size=pl[1], compiler_binding=d["bind"],
                            compiler_type=d["type"], native_local=native_local))
                if ((s.size and d["size"] != s.size) or d["bind"] != s.bind) and not proven_nobits_object:
                    problems.append(dict(kind="C symbol differs from original", tu=n, section=sec, symbol=s.name,
                                         va=f"0x{s.value:08X}", original=dict(size=s.size, bind=s.bind),
                                         c=dict(size=d["size"], bind=d["bind"]),
                                         nobits_storage_proof=proven_nobits_object))
                used = provides.get(s.name)
                if used is not None and used != s.value:
                    problems.append(dict(kind="c_aliases PROVIDE used for an original symbol at a different address",
                                         tu=n, symbol=s.name, provide=f"0x{used:08X}"))
            tu_audit["pieces"][csec_name] = piece
        for name, (csec_name, lo, hi) in storage_symbols.items():
            defs = [d for d in cdefs.get(name, []) if d["section"] == csec_name
                    and lo <= d["addr"] < hi]
            if len(defs) != 1:
                problems.append(dict(kind="C NOBITS storage owner identity is missing or ambiguous", tu=n,
                                     symbol=name, compiler_section=csec_name,
                                     span_start=f"0x{lo:08X}", span_end=f"0x{hi:08X}",
                                     definitions=[dict(d, addr=f"0x{d['addr']:08X}") for d in defs]))
                continue
            d = defs[0]
            if d["size"] and d["addr"] + d["size"] > hi:
                problems.append(dict(kind="C NOBITS storage owner exceeds its proven span", tu=n,
                                     symbol=name, compiler_section=csec_name,
                                     address=f"0x{d['addr']:08X}", size=d["size"],
                                     span_start=f"0x{lo:08X}", span_end=f"0x{hi:08X}"))
                continue
            tu_audit.setdefault("nobits_input_symbols", []).append(dict(
                symbol=name, compiler_section=csec_name, address=f"0x{d['addr']:08X}",
                size=d["size"], binding=d["bind"], type=d["type"],
                span_start=f"0x{lo:08X}", span_end=f"0x{hi:08X}",
                zero_size_label=(d["type"] == 0 and d["size"] == 0)))
        # PROVIDEs used for original symbols of this TU's C-owned pieces that the C object does not define
        for name, used in provides.items():
            if used is None:
                continue
            for sec, lo, hi in owned.values():
                if lo <= used < hi and any(s.name == name and s.value == used for s in osyms) and \
                        not any(d["addr"] == used for d in cdefs.get(name, [])) and \
                        name not in proven_aliases:
                    problems.append(dict(kind="c_aliases PROVIDE stands in for a missing C definition", tu=n,
                                         symbol=name, va=f"0x{used:08X}"))
        # R1: tentative definitions
        for s in commons:
            kind = ".sbss" if s.shndx == SHN_MIPS_SCOMMON else ".bss"
            o = tail_syms.get(s.name)
            rec = dict(symbol=s.name, section=kind, size=s.size, align=s.value)
            tu_audit["commons"].append(rec)
            if o is None:
                scaffold_resolution = scaffold_common_resolution(t, c_obj, s, kind)
                if scaffold_resolution:
                    tu_audit["commons_resolved_by_scaffold"].append(scaffold_resolution)
                else:
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
