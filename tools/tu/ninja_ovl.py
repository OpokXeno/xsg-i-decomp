#!/usr/bin/env python3
"""Phase 2 (overlays) configure: build.ninja for one per-TU overlay split.

    configure_ovl.py <unit_dir> <unit> --manifest compile-manifest.json

asm TUs and data subsegments: pinned modern GAS (as the prototype).  C TUs: the
per-TU contract from the compile manifest (cc_tu.sh: cpp -> cc1 -> legacy as).
ROM-style link (splat script, overlay .bss made part of the file image: the OVL
files carry their .bss as zero bytes inside the single PROGBITS section) +
objcopy -O binary + whole-file comparison; ELF-style link + segment comparison.
"""
import argparse
import json
import re
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
# tools/tu/ninja_ovl.py -> tools/tu -> tools -> repository root.
ROOT = HERE.parents[1]
sys.path.insert(0, str(HERE))
sys.path.insert(1, str(ROOT / "tools/tu"))
from toolchain import Toolchain  # noqa: E402
import tail_align  # noqa: E402
import data_carve  # noqa: E402

# tools/tu/<tool>.py -> the repository root is two levels up.
PY = BIN = None                     # resolved in main(), once ROOT is known
BSS_RECORD = None
CARVE_REGISTRY = None
OVERLAY_LINK = HERE / "overlay_link.py"


def resolve_tools(root):
    """The linker, objcopy and modern GAS, and the interpreter of the disassembler.

    `config/toolchain-identity.json` says which ones by version and SHA-256;
    where they are on this machine is a local setting (XENO_TOOLCHAIN_DIR or
    config/toolchain-local.json).  Every payload is hashed before the generated
    build.ninja names it.
    """
    tc = Toolchain(root)
    return (tc.splat_python(),
            Path(tc.dir("ps2dev-binutils")) / "mips64r5900el-ps2-elf-")
ASFLAGS = "-EL -march=r5900 -mabi=eabi -mgp64 -G0 -no-pad-sections -mno-fix-r5900"
TIMEOUT = "timeout -k 5 30"


def fix_bss(ld_text, unit):
    """splat writes the top-level `pad` <unit>_bss_file_image as an empty output
    section (a top-level pad emits no linker entry).  It stands for the overlay's
    .bss bytes, which the OVL file carries as zeros (filesz == memsz): give it the
    size of the NOLOAD bss section so the ROM position advances over them and
    objcopy -O binary writes the zero gap.  The only hand adjustment to the splat
    linker script."""
    sec = f".{unit}_bss_file_image : AT({unit}_bss_file_image_ROM_START) SUBALIGN(16)\n    {{\n        FILL(0x00000000);\n"
    assert ld_text.count(sec) == 1, "unexpected splat bss_file_image section"
    return ld_text.replace(sec, sec + f"        . += SIZEOF(.{unit}_bss); /* ovl-tu: bss bytes of the file image */\n")


TAIL_ALIGN_MARK = "declared tail alignment of "


def fix_tail_align(ld_text, unit):
    """Place the declared alignment after the `.text` of every TU whose
    original object pads its `.text` after the last
    function (tools/tu/tail_align.py: config/tu-build.json text.code_end <
    text.end). While a TU's object still ends at text.end it is a no-op; once the
    last function is C (cc1 emits no trailing padding) it restores the declared
    padding, independently of the next object's own section alignment. Earlier
    insertions are dropped first, so the script always follows the manifest."""
    names = {t["name"]: (t["id"], tail_align.declared_tail_align(t["text"]))
             for t in tail_align.declaring_tus(ROOT) if t["unit"] == unit}
    obj = re.compile(r"^(\s*)build/(?:scaffold/)?src/" + re.escape(unit) + r"/([\w\-]+)\.o\(\.text\);\s*$")
    out = []
    for line in ld_text.splitlines(keepends=True):
        if TAIL_ALIGN_MARK in line:
            continue
        out.append(line)
        m = obj.match(line)
        if m and m.group(2) in names:
            tu_id, alignment = names[m.group(2)]
            out.append(f"{m.group(1)}{tail_align.ld_statement(alignment)}; "
                       f"{tail_align.ld_comment(tu_id)}\n")
    return "".join(out)


def scaffold_aliases(unit_dir, ctus, orig, registry=None, unit=None, linker_script_text=None):
    """PROVIDE the splat spelling of every original LOCAL (or splat-renamed)
    function of a C TU, at its original offset from a GLOBAL function of the
    same TU.

    Inside a C TU the INCLUDE_ASM bodies define such a function under its original
    name with its original binding (`glabel X, local`, tools/tu/gen_ovl_src.py),
    so a reference from another scaffold object -- a TU's own `.data`
    function-pointer table is a separate splat data object -- cannot bind to it.
    This is main's mechanism (tools/tu/post_split.py label_fidelity and
    `scaffold_aliases.ld` in tools/tu/ninja_main.py): PROVIDE defines the name only
    for an otherwise undefined reference and never overrides a definition.  Main
    anchors on a TU start symbol in its linker script; here the anchor is the
    nearest GLOBAL function of the same TU, which the object itself defines, so the
    offset is exact even where the object's `.text` alignment inserts padding
    before it, and a function that moves inside its TU moves the pointer with it.
    Scaffolding, linker scripts and bindings are unchanged."""
    splat = {}
    for line in (unit_dir / "symbol_addrs.txt").read_text().splitlines():
        if "=" not in line:
            continue
        n, v = line.split("//")[0].split("=")
        splat.setdefault(int(v.strip().rstrip(";"), 16), n.strip())
    text_syms = sorted((s.value, s.name, s.bind) for s in orig.symbols if s.shndx == 1 and s.type == 2)
    out = ["/* splat spellings of original LOCAL/renamed functions of the C TUs, anchored on a GLOBAL",
           "   function of the same TU (generated by tools/tu/ninja_ovl.py) */"]
    seen = set()
    for t in sorted(ctus.values(), key=lambda t: int(t["text"][0], 16)):
        lo, hi = int(t["text"][0], 16), int(t["text"][1], 16)
        funcs = [(va, name, bind) for va, name, bind in text_syms if lo <= va < hi]
        anchors = [(va, name) for va, name, bind in funcs if bind != 0 and splat.get(va, name) == name]
        for va, name, bind in funcs:
            alias = splat.get(va, name)
            if (bind != 0 and alias == name) or alias in seen:
                continue
            if not anchors:
                raise SystemExit(f"ninja_ovl: {t['name']}: no GLOBAL function to anchor {alias} on")
            seen.add(alias)
            ava, aname = min(anchors, key=lambda a: (abs(va - a[0]), a[0]))
            delta = va - ava
            out.append(f'PROVIDE("{alias}" = {aname} {"+" if delta >= 0 else "-"} 0x{abs(delta):X});')
    if registry is not None and unit is not None:
        output_base = None
        has_storage_aliases = any(
            tu_id.startswith(unit + "/") and any(
                run.get("c_storage_aliases")
                for runs in sections.values() for run in runs)
            for tu_id, sections in registry.get("tus", {}).items())
        if has_storage_aliases:
            match = re.search(r'^\s*\.' + re.escape(unit) + r'\s+0x([0-9A-Fa-f]+)\s*:',
                              linker_script_text or "", re.M)
            if not match:
                raise SystemExit(f"ninja_ovl: {unit}.ld has no output-section base for C storage aliases")
            output_base = int(match.group(1), 16)
        for tu_id, sections in registry.get("tus", {}).items():
            if not tu_id.startswith(unit + "/"):
                continue
            tu = next((t for t in ctus.values() if t['id'].split('_')[0] == tu_id), None)
            compiler_input = None
            if tu:
                paths = [unit_dir/'build'/folder/unit/(tu['name']+'.o')
                         for folder in ('scaffold/src', 'src')]
                compiler_path = next((p for p in paths if p.is_file()), None)
                if compiler_path:
                    compiler_input = data_carve.Elf(compiler_path.read_bytes())
            for section, runs in sections.items():
                for run in runs:
                    lo, hi = (int(x, 16) for x in run["range"])
                    owners = set(run.get("c_owned_symbols") or [])
                    # A retained scaffold pointer table cannot bind directly
                    # to a LOCAL DATA symbol now defined in the separate C
                    # object. Preserve its original spelling as an uncredited
                    # linker alias, only for an original LOCAL OBJECT wholly
                    # supplied by an explicit compiler input span. Retail
                    # overlay symbols may have zero sizes; the raw compiler
                    # OBJECT and mapped span must prove the positive extent.
                    # The alias provides no storage or recovered-data credit.
                    if section not in (".bss", ".sbss"):
                        for symbol in orig.symbols:
                            if (symbol.bind != 0 or symbol.type != 1
                                    or symbol.shndx == 0 or symbol.name not in owners
                                    or not lo <= symbol.value < hi
                                    or symbol.name in seen):
                                continue
                            parts = [part for part in run.get("c_input_spans", [])
                                     if symbol.name in (part.get("symbols") or [])
                                     and int(part["range"][0], 16) <= symbol.value
                                     and symbol.value + symbol.size <= int(part["range"][1], 16)]
                            if (len(parts) != 1 or compiler_input is None
                                    or any(s.name == symbol.name and s.bind != 0 for s in orig.symbols)):
                                continue
                            part = parts[0]
                            raw = [s for s in compiler_input.symbols if s.name == symbol.name
                                   and s.bind == 0 and s.type == 1 and s.size > 0
                                   and 0 < s.shndx < len(compiler_input.sections)]
                            if len(raw) != 1:
                                continue
                            compiled = raw[0]
                            original_start, original_end = map(data_carve.hx, part['range'])
                            input_start, input_end = map(data_carve.hx, part['object_range'])
                            if (compiler_input.sections[compiled.shndx].name != part['section']
                                    or not input_start <= compiled.value < compiled.value + compiled.size <= input_end
                                    or original_start + compiled.value - input_start != symbol.value
                                    or symbol.value + compiled.size > original_end
                                    or symbol.size not in (0, compiled.size)):
                                continue
                            seen.add(symbol.name)
                            out.append(f'/* uncredited scaffold reference to original LOCAL C DATA: {symbol.name} */')
                            out.append(f'PROVIDE("{symbol.name}" = 0x{symbol.value:08X});')
                    for row in run.get("c_storage_aliases", []):
                        alias = row["original_name"]
                        address = int(row["address"], 16)
                        owner = row["storage_owner"]
                        owner_name = owner["name"]
                        owner_address = int(owner["address"], 16)
                        owner_size = int(owner["size"])
                        if (not alias or owner_name not in owners or owner_size <= 0
                                or not lo <= owner_address or owner_address + owner_size > hi
                                or not owner_address <= address < owner_address + owner_size):
                            raise SystemExit(f"ninja_ovl: {tu_id} {section}: invalid C storage alias {alias}")
                        if owner_address < output_base:
                            raise SystemExit(f"ninja_ovl: {tu_id} {section}: owner VA precedes {unit} output section")
                        if section in (".bss", ".sbss"):
                            owner_parts = [part for part in run.get("c_input_spans", [])
                                           if owner_name in (part.get("symbols") or [])
                                           and int(part["range"][0], 16) <= owner_address
                                           and owner_address + owner_size <= int(part["range"][1], 16)]
                            if len(owner_parts) != 1:
                                raise SystemExit(
                                    f"ninja_ovl: {tu_id} {section}: BSS alias {alias} lacks one proven c_input_spans owner")
                        if alias in seen:
                            raise SystemExit(f"ninja_ovl: duplicate overlay alias {alias}")
                        seen.add(alias)
                        delta = address - owner_address
                        # These storage aliases name an original mapped VA.
                        # The ROM script calls the overlay output section
                        # `.<unit>`, while the ELF-style linker script uses the
                        # original ELF section name `<unit>`; a section-relative
                        # ADDR expression therefore fails in one of the two
                        # otherwise equivalent link routes. Keep the exact
                        # original VA as the PROVIDE value, which is already
                        # enforced by the mapped-section and whole-file gates.
                        out.append(f'PROVIDE("{alias}" = 0x{address:08X});')
        # Splitting a TU's original local BSS labels into byte-preserving
        # scaffold objects makes them invisible to undefined references from
        # that TU's separately linked C/INCLUDE_ASM objects. Restore only the
        # original linker name as an uncredited alias when the exact local
        # NOTYPE/zero-size identity lies in the mapped BSS family and outside
        # every C-owned run. This does not define storage or move scaffold
        # bytes; the normal whole-overlay gate remains authoritative.
        if output_base is None:
            match = re.search(r'^\s*\.' + re.escape(unit) + r'\s+0x([0-9A-Fa-f]+)\s*:',
                              linker_script_text or "", re.M)
            if match:
                output_base = int(match.group(1), 16)
        if output_base is not None:
            layout = json.loads((unit_dir / "layout.json").read_bytes())
            bss_spans = [
                (data_carve.hx(row[1]), data_carve.hx(row[2]))
                for section in (".bss", ".sbss")
                for row in (layout.get("families", {}).get(section) or [])
            ]
            c_spans = [
                (int(run["range"][0], 16), int(run["range"][1], 16))
                for tu_id, sections in registry.get("tus", {}).items()
                if tu_id.startswith(unit + "/")
                for section, runs in sections.items()
                if section in (".bss", ".sbss")
                for run in runs
            ]
            local_bss = {}
            for symbol in orig.symbols:
                if (symbol.bind == 0 and symbol.type == 0 and symbol.size == 0
                        and symbol.shndx == 1 and symbol.name
                        and any(lo <= symbol.value < hi for lo, hi in bss_spans)):
                    local_bss.setdefault(symbol.name, set()).add(symbol.value)
            all_names = {symbol.name for symbol in orig.symbols if symbol.name}
            for alias, addresses in sorted(local_bss.items()):
                if alias in seen or len(addresses) != 1:
                    continue
                address = next(iter(addresses))
                if address < output_base or any(lo <= address < hi for lo, hi in c_spans):
                    continue
                # A non-local original identity with this spelling is never
                # converted into a local-scaffold fallback.
                if any(symbol.name == alias and symbol.bind != 0 for symbol in orig.symbols):
                    continue
                seen.add(alias)
                out.append(f'/* uncredited retained scaffold BSS identity: {alias} @ {address:#010x} */')
                # As above, these retained scaffold aliases refer to exact
                # original storage addresses in both ROM and ELF link scripts.
                out.append(f'PROVIDE("{alias}" = 0x{address:08X});')
    text = "\n".join(out) + "\n"
    path = unit_dir / "scaffold_aliases.ld"
    if not path.is_file() or path.read_text() != text:
        path.write_text(text)


def objects(ld_text):
    return sorted(set(re.findall(r"(build/[\w/.\-]+\.o)\(", ld_text)))


def bss_allocations(unit, unit_dir, unit_text, ctus, record_path=None):
    """Raw linker object -> ld -r -d output for the explicitly owned BSS TUs."""
    record_path = Path(BSS_RECORD if record_path is None else record_path)
    if not record_path.is_file():
        return {}
    record = json.loads(record_path.read_text())
    record = record.get('overlay_bss_records', {}).get(unit, record)
    names = {t["id"]: name for name, t in ctus.items()}
    # Campaign location records use the stable ovNN/tuNNN spelling, while
    # TU-build rows carry the address-qualified identity. Accept only that
    # exact ordinal prefix as the short form; do not infer by symbol/address.
    names.update({t["id"].split("_")[0]: name for name, t in ctus.items()})
    by_raw = {}
    for loc in record.get("recovered_location_candidates", []):
        if loc.get("section") not in (".bss", ".sbss"):
            continue
        tu_id = loc.get("evidence", {}).get("original_tu")
        name = names.get(tu_id)
        if not name:
            raise SystemExit(f"overlay BSS record names a non-C or foreign TU: {tu_id}")
        match = re.search(r"(build/(?:scaffold/)?src/" + re.escape(unit) + "/" +
                          re.escape(name) + r"\.o)\(\.text\);", unit_text)
        if not match:
            raise SystemExit(f"overlay BSS record TU {tu_id} has no C .text object in {unit}.ld")
        raw = match.group(1)
        allocated = f"build/c/{unit}/{name}.allocated.o"
        prior = by_raw.get(raw)
        if prior and prior != allocated:
            raise SystemExit(f"overlay BSS record maps {raw} to conflicting allocated objects")
        by_raw[raw] = allocated
    return by_raw


def restore_bss_inputs(unit, text, ctus, record_path):
    """Undo the prior generated .text substitution before an idempotent re-carve."""
    if not Path(record_path).is_file():
        return text
    record = json.loads(Path(record_path).read_text())
    record = record.get('overlay_bss_records', {}).get(unit, record)
    for loc in record.get("recovered_location_candidates", []):
        if loc.get("section") not in (".bss", ".sbss"):
            continue
        tu_id = loc.get("evidence", {}).get("original_tu", "")
        short_id = tu_id.split("_")[0]
        match = next(((name, t) for name, t in ctus.items()
                      if tu_id in (t["id"], t["id"].split("_")[0]) or
                      short_id == t["id"].split("_")[0]), None)
        if match is None:
            continue
        name, t = match
        allocated = f"build/c/{unit}/{name}.allocated.o"
        raw = loc.get("evidence", {}).get("link_object")
        # Some older sidecars use the final overlay image name in
        # `link_object`; only an exact C-object path may be used to restore a
        # previous allocator substitution. Otherwise restore the TU's normal
        # scaffold input, which is also the path bss_allocations pins.
        if not isinstance(raw, str) or not re.fullmatch(
                r"build/(?:scaffold/)?src/" + re.escape(unit) + "/" + re.escape(name) + r"\.o", raw):
            raw = f"build/scaffold/src/{unit}/{name}.o"
        # A previous BSS carve can leave the split allocated object in the
        # linker script. Restore either generated form to the raw C object
        # before applying the current registry, so regeneration is idempotent.
        for allocated_input in (allocated, allocated.removesuffix(".o") + data_carve.SPLIT_OBJECT):
            text = text.replace(allocated_input + "(", raw + "(")
    return text


def fix_elf_bss_pins(unit_dir, unit, carve_report):
    """Translate BSS-relative ROM-script pins into the ELF script's unit origin.

    The ROM script has a separate .<unit>_bss output section beginning at the
    layout BSS base. The ELF script places .bss inside the single overlay output
    section beginning at the text base, so the same absolute end needs the
    added BSS-to-text origin delta.
    """
    rows = [r for r in carve_report if r['section'] in ('.bss', '.sbss')]
    if not rows:
        return
    layout = json.loads((Path(unit_dir) / 'layout.json').read_bytes())
    fam = layout.get('families', {})
    text_rows = [[name, span[0], span[1]] for name, span in layout.get('text', {}).items()]
    bss_rows = fam.get('.bss', []) + fam.get('.sbss', [])
    if not text_rows or not bss_rows:
        raise SystemExit(f"ELF BSS pin adjustment requires text and BSS layout origins for {unit}")
    text_base = min(data_carve.hx(row[1]) for row in text_rows)
    bss_base = min(data_carve.hx(row[1]) for row in bss_rows)
    delta = bss_base - text_base
    path = Path(unit_dir) / f"{unit}.elf.modern.ld"
    script = path.read_text()
    for row in rows:
        input_section = "scommon" if row['section'] == '.sbss' else "bss"
        pattern = re.compile(
            r"(" + re.escape(row['link_object']) + r"\(\." + input_section +
            r"(?:\.carve\.\d+)?\);\s*\n\s*\.\s*=\s*0x)([0-9A-Fa-f]+)(;)")
        script, n = pattern.subn(lambda m: f"{m.group(1)}{int(m.group(2),16)+delta:X}{m.group(3)}", script)
        if n == 0:
            raise SystemExit(f"ELF BSS pin adjustment found no {input_section} pins for {row['tu']} in {path}")
    path.write_text(script)


def fix_rom_bss_pins(unit_dir, unit, script, carve_report):
    """Pin the NOLOAD BSS origin and normalize legacy text-relative carve pins.

    Current data_carve emits BSS-relative offsets. Older producers emitted the
    same endpoints relative to the overlay text origin. Normalize only values
    proven by this carve report, so a large legitimate BSS offset is never
    classified by a numeric threshold and already-relative pins are preserved.
    """
    rows = [r for r in carve_report if r['section'] in ('.bss', '.sbss')]
    if not rows:
        return script
    layout = json.loads((Path(unit_dir) / 'layout.json').read_bytes())
    fam = layout.get('families', {})
    bss_rows = fam.get('.bss', []) + fam.get('.sbss', [])
    if not bss_rows:
        raise SystemExit(f"ROM BSS pin adjustment requires a BSS layout origin for {unit}")
    bss_base = min(data_carve.hx(row[1]) for row in bss_rows)
    text_rows = [[name, span[0], span[1]] for name, span in layout.get('text', {}).items()]
    if not text_rows:
        raise SystemExit(f"ROM BSS pin adjustment requires a text layout origin for {unit}")
    text_base = min(data_carve.hx(row[1]) for row in text_rows)
    # The linker otherwise assigns the NOLOAD output section's start from its
    # strictest input alignment.  That can move the whole overlay BSS past its
    # mapped origin (for example, A13654 -> A13658 when a later input is align8)
    # and make every subsequent absolute pin four bytes late.  Pin the output
    # section itself to the original BSS family start; individual input
    # sections retain their own natural alignments within that address space.
    section = re.compile(
        r"(^\s*\." + re.escape(unit) + r"_bss)(?:\s+0x[0-9A-Fa-f]+)?"
        r"(\s+\(NOLOAD\)\s*:)", re.M)
    script, section_count = section.subn(
        lambda m: f"{m.group(1)} 0x{bss_base:08X}{m.group(2)}", script)
    if section_count != 1:
        raise SystemExit(f"ROM BSS output section address pin found {section_count} matches for {unit}.ld")
    # Normalize only generated carve pins whose value matches the exact
    # endpoint-minus-text-origin value from the report. Current producers emit
    # endpoint-minus-BSS-origin already, so those pins remain unchanged.
    legacy_to_current = {}
    for row in rows:
        ends = []
        for _, end in row.get('runs', []):
            ends.append(data_carve.hx(end))
        for span in row.get('c_input_spans', []):
            ends.append(data_carve.hx(span['range'][1]))
        for end_va in ends:
            old = end_va - text_base
            new = end_va - bss_base
            if old == new:
                continue
            prior = legacy_to_current.get(old)
            if prior is not None and prior != new:
                raise SystemExit(f"ambiguous legacy BSS pin 0x{old:X} for {unit}")
            legacy_to_current[old] = new
    if legacy_to_current:
        section_start = section.search(script)
        body_start = script.find('{', section_start.end())
        depth = 1
        pos = body_start + 1
        while depth and pos < len(script):
            if script[pos] == '{':
                depth += 1
            elif script[pos] == '}':
                depth -= 1
            pos += 1
        if depth:
            raise SystemExit(f"unterminated ROM BSS output section for {unit}")
        body = script[body_start + 1:pos - 1]
        pin_re = re.compile(r"(\.\s*=\s*0x)([0-9A-Fa-f]+)(;[^\n]*data-carve[^\n]*)")
        def normalize_pin(match):
            value = int(match.group(2), 16)
            replacement = legacy_to_current.get(value)
            if replacement is None:
                return match.group(0)
            return f"{match.group(1)}{replacement:X}{match.group(3)}"
        body = pin_re.sub(normalize_pin, body)
        script = script[:body_start + 1] + body + script[pos - 1:]
    return script


def main():
    global ROOT, PY, BIN, BSS_RECORD, CARVE_REGISTRY
    ap = argparse.ArgumentParser()
    ap.add_argument("unit_dir", type=Path)
    ap.add_argument("unit")
    ap.add_argument("--root", type=Path, default=None)
    ap.add_argument("--manifest", type=Path, required=True)
    ap.add_argument("--bss-record", type=Path, default=None,
                    help="explicit evidence record for guarded overlay .bss/.sbss allocation")
    ap.add_argument("--carve-registry", type=Path, default=None,
                    help="private TU data-carve registry (defaults to ROOT/config/tu/data-carves.json)")
    ap.add_argument("--emit-scaffold-aliases", action="store_true",
                    help="refresh guarded scaffold aliases after compiling the C objects")
    a = ap.parse_args()
    if a.root is not None:
        ROOT = a.root.resolve()
    # The stable C data-carve registry is the safe default. A separate
    # compiler-storage evidence record is supplied explicitly only for
    # overlays that need guarded .bss/.sbss allocation.
    if a.bss_record is None:
        BSS_RECORD = data_carve.registry_input_path(ROOT)
    else:
        record = a.bss_record
        BSS_RECORD = (record if record.is_absolute() else ROOT / record).resolve()
    CARVE_REGISTRY = (a.carve_registry.resolve() if a.carve_registry is not None
                      else data_carve.registry_input_path(ROOT))
    PY, BIN = resolve_tools(ROOT)
    unit_dir, unit = a.unit_dir.resolve(), a.unit
    man = json.loads(a.manifest.read_text())["units"][unit]
    # Objects are matched by TU name, not by the manifest's object path: splat
    # writes the generated skeletons under `scaffold/src/`, so the linker script
    # names `build/scaffold/src/<unit>/<tu>.o` for a TU with no published source
    # and `build/src/<unit>/<tu>.o` for one with.
    ctus = {t["name"]: t for t in man["tus"] if t["kind"] == "c"}
    if a.emit_scaffold_aliases:
        target = re.search(r"target_path: (\S+)", (unit_dir / "splat.yaml").read_text())[1]
        scaffold_aliases(unit_dir, ctus, data_carve.Elf((unit_dir / target).read_bytes()),
                         json.loads(CARVE_REGISTRY.read_text()), unit,
                         (unit_dir / f"{unit}.ld").read_text())
        return
    common = unit_dir / "include/common.h"
    if not common.exists():
        common.write_text("#ifndef COMMON_H\n#define COMMON_H\n\n#include \"include_asm.h\"\n\n#endif\n")
    ld = unit_dir / f"{unit}.ld"
    text = data_carve.ovl_restore(ld.read_text())
    text = restore_bss_inputs(unit, text, ctus, BSS_RECORD)
    if "ovl-tu: bss bytes of the file image" not in text:
        text = fix_bss(text, unit)
        ld.write_text(text)
    aligned = fix_tail_align(text, unit)
    if aligned != text:
        text = aligned
        ld.write_text(text)
    # Declared C-owned data runs (config/tu/data-carves.json, tools/tu/data_carve.py):
    # a C TU's constants or initialized arrays cut out of its scaffold data piece and
    # placed from its object. An earlier carve is undone first; nothing declared
    # leaves splat's script unchanged.
    allocated_inputs = bss_allocations(unit, unit_dir, text, ctus)
    carved, carve_report = data_carve.apply_overlay(
        ROOT, unit_dir, unit, text, registry=json.loads(CARVE_REGISTRY.read_text()),
        c_object_overrides=allocated_inputs)
    carved_bss = {next((name for name, t in ctus.items()
                        if r['tu'] in (t['id'], t['id'].split('_')[0])), r['tu'])
                   for r in carve_report if r['section'] in ('.bss', '.sbss')}
    bss_record = json.loads(BSS_RECORD.read_text()) if BSS_RECORD.is_file() else {}
    bss_record = bss_record.get('overlay_bss_records', {}).get(unit, bss_record)
    bss_locations = bss_record.get('recovered_location_candidates', [])
    expected_bss = {name for name, t in ctus.items()
                    if any(loc.get('evidence', {}).get('original_tu') in
                           (t['id'], t['id'].split('_')[0])
                           and loc.get('section') in ('.bss', '.sbss')
                           for loc in bss_locations)}
    if expected_bss - carved_bss:
        raise SystemExit(f"overlay BSS ownership record has no matching data-carve run for {sorted(expected_bss - carved_bss)}")
    if any(r['section'] in ('.bss', '.sbss') and not r.get('allocated_object') for r in carve_report):
        missing_alloc = [dict(tu=r['tu'], c_object=r['c_object']) for r in carve_report
                         if r['section'] in ('.bss', '.sbss') and not r.get('allocated_object')]
        raise SystemExit(f"overlay .bss/.sbss data-carve run has no explicit guarded allocation record: {missing_alloc}")
    # The pinned linker materializes MIPS SCOMMON as `.scommon`. A single C
    # run can consume that section directly. Splitting multiple original
    # `.sbss` runs requires first splitting/renaming `.scommon` itself; emitting
    # the same whole input section at several addresses would duplicate it.
    if any(r['section'] == '.sbss' and len(r['runs']) > 1 for r in carve_report):
        raise SystemExit("multi-run overlay .sbss allocation needs an explicit .scommon split plan")
    split_objects = {r['link_object']: r for r in carve_report
                     if r['link_object'].endswith(data_carve.SPLIT_OBJECT)}
    if unit == "ov12":
        # OV12's rg_main scaffold leaves one zero word at A4F4C4, exactly at
        # the text/data boundary. The following C .data input is 8-byte
        # aligned and begins at A4F4C8; retaining the scaffold word after
        # DATA_START makes the linker align the C input twice and overruns its
        # exact A4F4C8..A4F520 run by eight bytes. Configure recreates the
        # carve tree, so the generated fragment object is not available here.
        # Guard this linker adjustment with the original ELF bytes, generated
        # scaffold item, and pinned C route instead; ordinary 8-byte input
        # alignment then leaves the same zero word at A4F4C4 and starts C data
        # at C8. The native object is intentionally unavailable during configure.
        from elfinfo import Elf
        scaffold_data = unit_dir / "asm/data/ov12/rg_main.data.s"
        scaffold_text = scaffold_data.read_text() if scaffold_data.is_file() else ""
        item = re.search(
            r"(?ms)^dlabel D_00A4F4C4\s*$\n(.*?)^enddlabel D_00A4F4C4\s*$",
            scaffold_text)
        exact_zero_word = bool(item and re.search(
            r"/\*\s*[0-9A-Fa-f]+\s+00A4F4C4\s+00000000\s*\*/\s*\.word\s+0x0+\b",
            item.group(1)))
        data_runs = json.loads(CARVE_REGISTRY.read_text()).get("tus", {}).get(
            "ov12/tu001", {}).get(".data", [])
        exact_run = any(run.get("range") == ["0x00a4f4c8", "0x00a4f520"]
                        for run in data_runs)
        yaml_text = (unit_dir / "splat.yaml").read_text()
        target = re.search(r"target_path: (\S+)", yaml_text)[1]
        original = Elf((unit_dir / target).read_bytes())
        original_data = original.section("ov12")
        original_offset = 0x00A4F4C4 - 0x00A00000
        original_zero_word = (original_data.addr == 0x00A00000
                              and original_offset + 4 <= original_data.size
                              and original.section_bytes(original_data)[original_offset:original_offset + 4]
                              == b"\0" * 4)
        fragment_input = "        build/carve/ov12/rg_main.data.o(.data); /* data-carve */\n"
        if (original_zero_word and exact_zero_word and exact_run
                and carved.count(fragment_input) == 1):
            carved = carved.replace(fragment_input, "")
    if carved != text:
        text = carved
    text = fix_rom_bss_pins(unit_dir, unit, text, carve_report)
    if text != ld.read_text():
        ld.write_text(text)
    yaml_text = (unit_dir / "splat.yaml").read_text()
    target = re.search(r"target_path: (\S+)", yaml_text)[1]
    sys.path.insert(0, str(HERE))
    from elfinfo import Elf
    from link_elf import generate
    entry = Elf((unit_dir / target).read_bytes()).entry
    generate(unit_dir, unit, "modern")
    fix_elf_bss_pins(unit_dir, unit, carve_report)
    scaffold_aliases(unit_dir, ctus, Elf((unit_dir / target).read_bytes()),
                     json.loads(CARVE_REGISTRY.read_text()), unit, text)
    objs = objects(text)
    lines = [
        "# Generated by configure_ovl.py (TU-migration phase 2, overlays)",
        f"as = {TIMEOUT} {BIN}as",
        f"asflags = {ASFLAGS}",
        f"ld = {TIMEOUT} {BIN}ld",
        f"objcopy = {TIMEOUT} {BIN}objcopy",
        f"py = {PY} -B",
        f"cc = {ROOT}/tools/tu/cc_tu.sh",
        "",
        "rule as",
        "  command = $as $asflags -I . -I include -o $out $in",
        "  description = AS $in",
        "rule bin",
        "  command = printf '.section .data, \"wa\"\\n.incbin \"%s\"\\n' $in | $as $asflags -o $out -",
        "  description = BIN $in",
        "# Header dependencies (cc_tu.sh writes $out.d when XENO_TU_DEPFILE names it): an edit to",
        "# any header the TU includes, one created after configure too, recompiles it. The",
        "# unit and root directories stay the last two arguments (tools/tu_audit.py reads them).",
        "rule cc",
        f"  command = XENO_TU_DEPFILE=$out.d $cc $in $out $ccdir $cas $gflag {unit_dir} {ROOT}",
        "  depfile = $out.d",
        "  deps = gcc",
        "  description = CC $in",
        "rule carvesplit",
        f"  command = {TIMEOUT} $py {HERE}/data_carve.py split --root {ROOT} --unit {unit} "
        f"--unit-dir {unit_dir} --registry {CARVE_REGISTRY} --tu $tu --obj $in --out $out",
        "  description = CARVE-SPLIT $out",
        "rule allocate_bss",
        f"  command = {TIMEOUT} $py {OVERLAY_LINK} allocate --root {ROOT} --record {BSS_RECORD} "
        "--tu $tu --raw $in --out $alloc_out --report $report",
        "  description = LD -r -d $out",
        "rule scaffold_aliases",
        f"  command = {TIMEOUT} $py {HERE}/ninja_ovl.py {unit_dir} {unit} --root {ROOT} "
        f"--manifest {a.manifest.resolve()} --carve-registry {CARVE_REGISTRY} --emit-scaffold-aliases",
        "  description = SCAFFOLD-ALIASES $out",
        "  restat = 1",
        "rule ld",
        "  command = $ld -EL -m elf32lr5900 --no-undefined -T undefined_syms_auto.txt -T undefined_funcs_auto.txt "
        "-T main_symbols.ld -T scaffold_aliases.ld -T $script -Map $out.map -o $out",
        "  description = LD $out",
        "rule flat",
        "  command = $objcopy -O binary $in $out",
        "rule ld_elf",
        "  command = $ld -EL -m elf32lr5900 --no-undefined -z max-page-size=4096 -e $entry "
        "-T undefined_syms_auto.txt -T undefined_funcs_auto.txt -T main_symbols.ld -T scaffold_aliases.ld -T $script -Map $out.map -o $out",
        "  description = LD(elf) $out",
        "rule compare_elf",
        f"  command = $py {ROOT}/tools/tu/compare.py --elf --original $orig --rebuilt $in --report $out",
        "rule compare",
        f"  command = $py {ROOT}/tools/tu/compare.py --original $orig --rebuilt $in --report $out",
        "",
    ]
    raw_alloc_inputs = {allocated: raw for raw, allocated in allocated_inputs.items()}
    emitted_cc = set()
    compile_input = ROOT / 'config/tu-build.json'
    if not compile_input.is_file():
        compile_input = ROOT / 'config/objects/overlays.compile.json'

    def emit_cc(raw_obj, name):
        if raw_obj in emitted_cc:
            return
        emitted_cc.add(raw_obj)
        t = ctus[name]
        shared = [str(p) for p in sorted((ROOT / "include").rglob("*.h"))
                  if p.name not in ("common.h", "include_asm.h")]
        local_h = ROOT / f"src/{unit}/{name}.h"
        if local_h.is_file():
            shared.append(str(local_h))
        deps = " ".join(t["include_asm_files"] + [x["file"] for x in t["accepted_asm_files"]]
                        + ["include/labels.inc", "include/include_asm.h"] + shared)
        published = f"src/{unit}/{name}.c"
        source = published if (unit_dir / published).is_file() else f"scaffold/src/{unit}/{name}.c"
        lines.extend([f"build {raw_obj}: cc {source} | {deps}",
                      f"  ccdir = {ROOT / t['compiler']['dir']}",
                      f"  cas = {ROOT / t['assembler']['path']}",
                      f"  gflag = {t['flags'][1]}"])

    for obj in objs:
        split = split_objects.get(obj)
        raw_obj = split['c_object'] if split else raw_alloc_inputs.get(obj)
        if raw_obj is None and split:
            raw_obj = split['c_object']
        allocated_obj = (split.get('allocated_object') if split else obj if obj in raw_alloc_inputs else None)
        compiled_obj = allocated_obj or raw_obj or obj
        rel = compiled_obj[len("build/"):-2]
        if rel.startswith("assets/"):
            lines.append(f"build {obj}: bin {rel}.bin")
        elif allocated_obj:
            name = Path(raw_obj).stem
            emit_cc(raw_obj, name)
            if allocated_obj != obj:
                lines += [f"build {allocated_obj} | {allocated_obj}.report.json: allocate_bss {raw_obj} | {OVERLAY_LINK} {BSS_RECORD}",
                          f"  alloc_out = {allocated_obj}",
                          f"  report = {allocated_obj}.report.json",
                          f"  tu = {ctus[name]['id']}"]
            if split:
                split_deps = [str(CARVE_REGISTRY), str(HERE / 'data_carve.py'),
                              str(ROOT / 'tools/tu/elfinfo.py'), str(compile_input), 'layout.json', target]
                split_deps += [f'asm/data/{unit}/{name}{r["section"]}.s'
                               for r in carve_report if r['tu'] == split['tu']]
                lines += [f"build {obj}: carvesplit {compiled_obj} | {' '.join(split_deps)}",
                          f"  tu = {split['tu']}"]
            elif allocated_obj == obj:
                lines += [f"build {allocated_obj} | {allocated_obj}.report.json: allocate_bss {raw_obj} | {OVERLAY_LINK} {BSS_RECORD}",
                          f"  alloc_out = {allocated_obj}",
                          f"  report = {allocated_obj}.report.json",
                          f"  tu = {ctus[name]['id']}"]
        elif Path(rel).stem in ctus and (rel.startswith("src/") or rel.startswith("scaffold/src/")):
            t = ctus[Path(rel).stem]
            # The repository's shared headers are on the overlay include path
            # (cc_tu.sh: -I include -I $ROOT/include), so an overlay C TU depends
            # on include/shared.h, the per-TU public headers it reads and its own
            # TU-local header (task tu-header-layout).
            shared = [str(p) for p in sorted((ROOT / "include").rglob("*.h"))
                      if p.name not in ("common.h", "include_asm.h")]
            local_h = ROOT / f"src/{unit}/{Path(rel).stem}.h"
            if local_h.is_file():
                shared.append(str(local_h))
            deps = " ".join(t["include_asm_files"] + [x["file"] for x in t["accepted_asm_files"]]
                            + ["include/labels.inc", "include/include_asm.h"] + shared)
            # The published TU source wins over the generated skeleton: `src` in
            # the unit directory is a link to the repository's src/ tree, and a
            # TU without a published file builds from scaffold/src/.
            published = f"src/{unit}/{Path(rel).stem}.c"
            source = published if (unit_dir / published).is_file() else f"{rel}.c"
            lines += [f"build {compiled_obj}: cc {source} | {deps}",
                      f"  ccdir = {ROOT / t['compiler']['dir']}",
                      f"  cas = {ROOT / t['assembler']['path']}",
                      f"  gflag = {t['flags'][1]}"]
            emitted_cc.add(compiled_obj)
            if split:
                compile_input = ROOT / 'config/tu-build.json'
                if not compile_input.is_file():
                    compile_input = ROOT / 'config/objects/overlays.compile.json'
                split_deps = [str(CARVE_REGISTRY), str(HERE / 'data_carve.py'),
                              str(ROOT / 'tools/tu/elfinfo.py'), str(compile_input),
                              'layout.json', target]
                split_deps += [f'asm/data/{unit}/{t["name"]}{r["section"]}.s'
                               for r in carve_report if r['tu'] == split['tu']]
                lines += [f"build {obj}: carvesplit {compiled_obj} | {' '.join(split_deps)}",
                          f"  tu = {split['tu']}"]
        else:
            lines.append(f"build {obj}: as {rel}.s")
    lines += [f"build scaffold_aliases.ld: scaffold_aliases | {' '.join(sorted(emitted_cc))} "
              f"{HERE}/ninja_ovl.py {CARVE_REGISTRY} {a.manifest.resolve()} {unit}.ld symbol_addrs.txt {target}"]
    elf_objs = [o for o in objs if not o.startswith("build/assets/")]
    lines += ["",
              f"build build/{unit}.rom.elf: ld | {' '.join(objs)} {unit}.ld undefined_syms_auto.txt undefined_funcs_auto.txt main_symbols.ld scaffold_aliases.ld",
              f"  script = {unit}.ld",
              f"build build/{unit}.bin: flat build/{unit}.rom.elf",
              f"build build/{unit}.compare.json: compare build/{unit}.bin",
              f"  orig = {target}",
              f"build build/{unit}.elf: ld_elf | {' '.join(elf_objs)} {unit}.elf.modern.ld main_symbols.ld scaffold_aliases.ld",
              f"  script = {unit}.elf.modern.ld",
              f"  entry = 0x{entry:X}",
              f"build build/{unit}.elf.compare.json: compare_elf build/{unit}.elf",
              f"  orig = {target}",
              f"default build/{unit}.compare.json build/{unit}.elf.compare.json", ""]
    (unit_dir / "build.ninja").write_text("\n".join(lines))
    print(f"{unit}: build.ninja with {len(objs)} objects ({len(ctus)} C TUs)")


if __name__ == "__main__":
    main()
