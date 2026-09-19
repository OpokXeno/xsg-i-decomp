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
sys.path.insert(0, str(HERE))
from toolchain import Toolchain  # noqa: E402
import tail_align  # noqa: E402

# tools/tu/<tool>.py -> the repository root is two levels up.
ROOT = HERE.parents[1]
PY = BIN = None                     # resolved in main(), once ROOT is known


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
    """Place `. = ALIGN(8);` after the `.text` of every TU of this unit whose
    original object pads its `.text` to an 8-byte boundary after the last
    function (tools/tu/tail_align.py: config/tu-build.json text.code_end <
    text.end). While a TU's object still ends at text.end it is a no-op; once the
    last function is C (cc1 emits no trailing padding) it restores the declared
    padding, independently of the next object's own section alignment. Earlier
    insertions are dropped first, so the script always follows the manifest."""
    names = {t["name"]: t["id"] for t in tail_align.declaring_tus(ROOT) if t["unit"] == unit}
    obj = re.compile(r"^(\s*)build/(?:scaffold/)?src/" + re.escape(unit) + r"/([\w\-]+)\.o\(\.text\);\s*$")
    out = []
    for line in ld_text.splitlines(keepends=True):
        if TAIL_ALIGN_MARK in line:
            continue
        out.append(line)
        m = obj.match(line)
        if m and m.group(2) in names:
            out.append(f"{m.group(1)}{tail_align.ld_statement()}; {tail_align.ld_comment(names[m.group(2)])}\n")
    return "".join(out)


def scaffold_aliases(unit_dir, ctus, orig):
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
    text = "\n".join(out) + "\n"
    path = unit_dir / "scaffold_aliases.ld"
    if not path.is_file() or path.read_text() != text:
        path.write_text(text)


def objects(ld_text):
    return sorted(set(re.findall(r"(build/[\w/.\-]+\.o)\(", ld_text)))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("unit_dir", type=Path)
    ap.add_argument("unit")
    ap.add_argument("--root", type=Path, default=None)
    ap.add_argument("--manifest", type=Path, required=True)
    a = ap.parse_args()
    global ROOT, PY, BIN
    if a.root is not None:
        ROOT = a.root.resolve()
    PY, BIN = resolve_tools(ROOT)
    unit_dir, unit = a.unit_dir.resolve(), a.unit
    man = json.loads(a.manifest.read_text())["units"][unit]
    # Objects are matched by TU name, not by the manifest's object path: splat
    # writes the generated skeletons under `scaffold/src/`, so the linker script
    # names `build/scaffold/src/<unit>/<tu>.o` for a TU with no published source
    # and `build/src/<unit>/<tu>.o` for one with.
    ctus = {t["name"]: t for t in man["tus"] if t["kind"] == "c"}
    common = unit_dir / "include/common.h"
    if not common.exists():
        common.write_text("#ifndef COMMON_H\n#define COMMON_H\n\n#include \"include_asm.h\"\n\n#endif\n")
    ld = unit_dir / f"{unit}.ld"
    text = ld.read_text()
    if "ovl-tu: bss bytes of the file image" not in text:
        text = fix_bss(text, unit)
        ld.write_text(text)
    aligned = fix_tail_align(text, unit)
    if aligned != text:
        text = aligned
        ld.write_text(text)
    yaml_text = (unit_dir / "splat.yaml").read_text()
    target = re.search(r"target_path: (\S+)", yaml_text)[1]
    sys.path.insert(0, str(HERE))
    from elfinfo import Elf
    from link_elf import generate
    entry = Elf((unit_dir / target).read_bytes()).entry
    generate(unit_dir, unit, "modern")
    scaffold_aliases(unit_dir, ctus, Elf((unit_dir / target).read_bytes()))
    objs = objects(text)
    lines = [
        "# Generated by configure_ovl.py (TU-migration phase 2, overlays)",
        f"as = {TIMEOUT} {BIN}as",
        f"asflags = {ASFLAGS}",
        f"ld = {TIMEOUT} {BIN}ld",
        f"objcopy = {TIMEOUT} {BIN}objcopy",
        f"py = {PY} -B",
        f"cc = {HERE}/cc_tu.sh",
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
        f"  command = $py {HERE}/compare.py --elf --original $orig --rebuilt $in --report $out",
        "rule compare",
        f"  command = $py {HERE}/compare.py --original $orig --rebuilt $in --report $out",
        "",
    ]
    for obj in objs:
        rel = obj[len("build/"):-2]
        if rel.startswith("assets/"):
            lines.append(f"build {obj}: bin {rel}.bin")
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
            lines += [f"build {obj}: cc {source} | {deps}",
                      f"  ccdir = {ROOT / t['compiler']['dir']}",
                      f"  cas = {ROOT / t['assembler']['path']}",
                      f"  gflag = {t['flags'][1]}"]
        else:
            lines.append(f"build {obj}: as {rel}.s")
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
