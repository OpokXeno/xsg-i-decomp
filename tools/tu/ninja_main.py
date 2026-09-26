#!/usr/bin/env python3
"""Phase 2 (MAIN): build.ninja + linker scripts from the TU manifest (canonical carve base).

    configure.py            (after gen_skeleton.py and splat; run from anywhere)

Writes, under main/:
  include/include_asm.h, include/common.h   standard splat INCLUDE_ASM form
  build/gen/tu/<tu>.s        asm-mode object of a GCC TU: every nonmatchings
                             function .s of the TU in address order, assembled
                             by modern GAS (the INCLUDE_ASM bodies, = the
                             `expected` all-asm object)
  build/gen/shim/<tu>.c      C-mode TU made of accepted unit sources (imports)
  main.rom.ld                ROM-style link (whole-file image) in manifest modes
  main.asmonly.rom.ld        same with every TU from asm (restructuring gate)
  main.elf.ld                ELF-style link (original sections, VMAs, PHDRS)
  aliases.ld                 names accepted units use for asm-owned addresses
  build.ninja

Every output section is pinned at its original VMA (and file offset for the
ROM-style image).  Inside a section the inputs follow that section's own
object order from the manifest.  When an asm-owned piece follows a C-owned
one, the location counter is set to the piece's cut ("pin"): the asm scaffold
does not know the original object's alignment, the linker gap is then the
original alignment padding; a C object that is too big fails the link, one
that is too small leaves a zero gap that the whole-file hash checks.
"""
import argparse
import json
import os
import re
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
from elfinfo import Elf  # noqa: E402
from toolchain import Toolchain  # noqa: E402
import tail_align  # noqa: E402
import data_carve  # noqa: E402


def resolve_tools(root):
    """The linker, objcopy and modern GAS, and the interpreter of the disassembler.

    `config/toolchain-identity.json` says which ones by version and SHA-256;
    where they are on this machine is a local setting (XENO_TOOLCHAIN_DIR or
    config/toolchain-local.json).  Every payload is hashed before the generated
    build.ninja names it.
    """
    tc = Toolchain(root)
    return (Path(tc.dir("ps2dev-binutils")) / "mips64r5900el-ps2-elf-",
            tc.splat_python(),
            Path(tc.file("dvp-as-3eb45ea")))

# tools/tu/ninja_main.py -> the repository root is two levels up; --root and
# --unit-dir rebind both so the promoted tool serves any checkout.
ROOT = Path(__file__).resolve().parents[2]
UNIT_DIR = ROOT / "build/main"
BIN = PY = DVPAS = None             # resolved in main(), once ROOT is known
ASFLAGS = "-EL -march=r5900 -mabi=eabi -mgp64 -G0 -no-pad-sections -mno-fix-r5900"
# config/vu-build.json contracts["vu-dvp-as-v1"].assembler.argv, verbatim: any
# message from the DVP assembler is a failure, and -no-abicalls -64 are what keep
# the EE link silent (docs/vu-microcode-unit.md, BUILD-INTEGRATION section 2a).
DVPASFLAGS = "--fatal-warnings -no-abicalls -64"
# PacketSizeVu0MicroCode .. PacketBottomVu0MicroCode; see tools/tu/gen_main.py.
VU0_PACKET = 0x730
T = "timeout -k 5 30"
DATA_SECTIONS = [".data", ".rodata", ".lit4", ".sdata", ".sbss", ".bss"]
C_SECTIONS = [".text", ".data", ".rodata", ".lit4", ".sdata", ".sbss", ".bss"]
DISCARD = ["*(.reginfo)", "*(.MIPS.abiflags)", "*(.pdr)", "*(.gnu.attributes)", "*(.mdebug*)",
           "*(.comment)", "*(.note*)", "*(.gptab.*)",
           # The DVP object's own sections.  The microprogram's real bytes travel
           # inside the packet in .vudata; .vutext/.vubss are empty, and the
           # overlay table, its string table and the zero-filled overlay
           # placeholder are already retail blobs of the image.  Placing them
           # would duplicate the payload and move every following file offset.
           "*(.vutext)", "*(.vubss)", "*(.DVP.ovlytab)", "*(.DVP.ovlystrtab)",
           "*(.DVP.overlay.*)"]

INCLUDE_ASM_H = r'''#ifndef INCLUDE_ASM_H
#define INCLUDE_ASM_H

/*
 * INCLUDE_ASM(FOLDER, NAME): generated scaffold (splat nonmatchings), no credit.
 * ACCEPTED_ASM(FOLDER, NAME): a previously accepted, reviewed standalone ee-asm
 *   function kept as tracked .s under src/<unit>/<tu>/ (exact_asm), never a
 *   generated nonmatching.  Same expansion, its own directory.
 * INCLUDE_RODATA(FOLDER, NAME): rodata that stays scaffold inside a C TU.
 * The bodies are assembled by the TU's own assembler (legacy ee-as / LA29),
 * so they must use that assembler's dialect (include/labels.inc).
 */
#if !defined(M2CTX) && !defined(PERMUTER) && !defined(SKIP_ASM)

#ifndef INCLUDE_ASM
#define INCLUDE_ASM(FOLDER, NAME) \
    __asm__( \
        ".section .text\n" \
        "    .set noat\n" \
        "    .set noreorder\n" \
        "    .include \"" FOLDER "/" #NAME ".s\"\n" \
        "    .set reorder\n" \
        "    .set at\n" \
    )
#endif
#ifndef ACCEPTED_ASM
/*
 * INCLUDE_ASM's expansion, plus:
 *  - `.align 3`: every accepted block starts at an 8-byte-aligned original address,
 *    while the tracked accepted sources start with `.align 2`; without it the
 *    alignment padding of the preceding function is lost as soon as that function
 *    becomes C (cc1 emits no trailing padding) and everything after it shifts.
 *  - `.set macro` after the file: accepted sources declare `.set nomacro`.
 */
#define ACCEPTED_ASM(FOLDER, NAME) \
    __asm__( \
        ".section .text\n" \
        "    .align 3\n" \
        "    .set noat\n" \
        "    .set noreorder\n" \
        "    .include \"" FOLDER "/" #NAME ".s\"\n" \
        "    .set reorder\n" \
        "    .set at\n" \
        "    .set macro\n" \
    )
#endif
#ifndef INCLUDE_RODATA
#define INCLUDE_RODATA(FOLDER, NAME) \
    __asm__( \
        ".section .rodata\n" \
        "    .include \"" FOLDER "/" #NAME ".s\"\n" \
        ".section .text" \
    )
#endif

__asm__(".include \"include/labels.inc\"\n");

#else

#ifndef INCLUDE_ASM
#define INCLUDE_ASM(FOLDER, NAME)
#endif
#ifndef ACCEPTED_ASM
#define ACCEPTED_ASM(FOLDER, NAME)
#endif
#ifndef INCLUDE_RODATA
#define INCLUDE_RODATA(FOLDER, NAME)
#endif

#endif /* !defined(M2CTX) && !defined(PERMUTER) && !defined(SKIP_ASM) */

#endif /* INCLUDE_ASM_H */
'''

COMMON_H = '#ifndef COMMON_H\n#define COMMON_H\n\n#include "include_asm.h"\n\n#endif\n'


def h(v):
    return f"0x{v:X}"


def first_vram(path):
    for line in path.read_text().splitlines():
        m = re.match(r"\s*/\* [0-9A-F]+ ([0-9A-F]{8}) ", line)
        if m:
            return int(m[1], 16)
    raise SystemExit(f"no address in {path}")


def tu_functions_in_order(unit_dir, name):
    """Every function asm file of a TU (nonmatchings, plus matchings for functions the TU
    already has as C or ACCEPTED_ASM), in address order."""
    files = [*(unit_dir / "asm/main/nonmatchings" / name).glob("*.s"), *(unit_dir / "asm/main/matchings" / name).glob("*.s")]
    return sorted(files, key=first_vram)


def unit_includes(rec):
    """Include roots equivalent to tools/match.py staging (-Istaged/include -Istaged)."""
    roots = []
    for hd in rec["headers"]:
        real, staged = Path(hd["path"]), Path(hd["staged_path"])
        if staged.parts[0] == "include" and len(staged.parts) > 1:
            rel = Path(*staged.parts[1:])
            assert str(real).endswith(str(rel)), (real, staged)
            root = Path(str(real)[: -len(str(rel))].rstrip("/"))
        else:
            root = real.parent
        if root not in roots:
            roots.append(root)
    return roots


def write_if_changed(path, text):
    path = Path(path)
    if not path.exists() or path.read_text() != text:
        path.write_text(text)


def tu_sym(name):
    return "__t_" + re.sub(r"\W", "_", name)


def piece_sym(name, sec):
    # `sec` may be the input section of a split run (`.rodata.carve.1`, tools/tu/data_carve.py)
    return "__c_" + re.sub(r"\W", "_", name) + "_" + re.sub(r"\W", "_", sec[1:])


WORD_REASON = ("scaffolding .word: the 16-byte _maxval literal inside SDK _copyRefImage is reached by fall-through "
               "and executes as four reserved-field DSRA32-to-$zero no-ops; the pinned assemblers cannot encode "
               "0x00FF00FF (tu-asmcompat review F2); SDK object, out of game scope")
INC_LINE = re.compile(r'^\s*(?:INCLUDE_ASM|ACCEPTED_ASM)\("([^"]+)", ([^)]+)\);')


def included_asm(cfile):
    """.s files a C TU pulls in through INCLUDE_ASM/ACCEPTED_ASM (ninja implicit deps)."""
    out = []
    for line in Path(cfile).read_text().splitlines():
        mm = INC_LINE.match(line)
        if mm:
            out.append(f"{mm[1]}/{mm[2].strip()}.s")
    return out


def main(argv=None):
    global ROOT, UNIT_DIR, BIN, PY, DVPAS
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--root", type=Path, default=None)
    ap.add_argument("--unit-dir", type=Path, default=None)
    a = ap.parse_args(argv)
    if a.root is not None:
        ROOT = a.root.resolve()
    BIN, PY, DVPAS = resolve_tools(ROOT)
    UNIT_DIR = (a.unit_dir.resolve() if a.unit_dir is not None else ROOT / "build/main")
    unit_dir = UNIT_DIR
    m = json.loads((unit_dir / "tu-manifest.json").read_bytes())
    # Declared C-owned data runs (config/tu/data-carves.json, tools/tu/data_carve.py):
    # the jump tables and literals recovered C functions generate, cut out of their
    # TU's scaffold piece. Nothing declared: `m` is the tracked manifest unchanged.
    m, carve_report = data_carve.apply_main(ROOT, unit_dir, m)
    manifest_name = "tu-manifest.json"
    if carve_report:
        manifest_name = "tu-manifest.carved.json"
        write_if_changed(unit_dir / manifest_name, json.dumps(m, indent=1) + "\n")
    elif (unit_dir / "tu-manifest.carved.json").exists():
        (unit_dir / "tu-manifest.carved.json").unlink()
    orig = Elf((unit_dir / "orig/SLUS_204.69").read_bytes())
    S = {x.name: x for x in orig.sections}
    tus = m["tus"]
    main_tus = [t for t in tus if t["text"].get("section", ".text") == ".text"]
    ov_tus = [t for t in tus if t["text"].get("section") == "ov02"]
    by_name = {t["name"]: t for t in tus}
    (unit_dir / "include").mkdir(exist_ok=True)
    write_if_changed(unit_dir / "include/include_asm.h", INCLUDE_ASM_H)
    write_if_changed(unit_dir / "include/common.h", COMMON_H)
    gen = unit_dir / "build/gen"
    (gen / "shim").mkdir(parents=True, exist_ok=True)
    # The acceptance records are not part of the repository.  Two things here
    # used to read them: the import shims (no TU uses one since the sources were
    # published) and `externals.ld` (the addresses an accepted record takes from
    # another unit), which is a tracked build input now.  When the records are at
    # hand they are still used, and externals.ld is regenerated from them.
    records_available = (ROOT / "config/units").is_dir()
    units = {p.stem: json.loads(p.read_bytes()) for p in (ROOT / "config/units").glob("*.json")}

    # skeleton for GCC objects without functions (splat writes none): data-only TUs
    for t in tus:
        if t["kind"] == "data-only":
            f = unit_dir / t["scaffold_path"]
            if not f.exists():
                f.parent.mkdir(parents=True, exist_ok=True)
                secs = ", ".join(t["sections"]) or "none"
                f.write_text(f"/* {t['name']} ({t['id']}): original GCC object without functions.\n"
                             f" * Data sections ({secs}) are scaffold pieces asm/main/data/{t['name']}.<sec>.s\n"
                             " * until the importer defines them here. */\n#include \"common.h\"\n")

    def cc_edge(out, cfile, con, flags, incs, deps, overlay=None):
        cc_dir = con["cc_dir"] if con["cc_dir"].startswith("/") else str(ROOT / con["cc_dir"])
        lines = [f"build {out}: {'ccas_c' if overlay else 'ccas'} {cfile} | {' '.join(sorted(set(deps)))}",
                 f"  ccdir = {cc_dir}", f"  flags = {' '.join(flags)}", f"  incs = {incs}",
                 f"  eeas = {ROOT / con['assembler_path']}", f"  gflag = {flags[1]}"]
        if overlay:
            lines.append(f"  overlay = {overlay}")
        return lines

    # ------------------------------------------------ objects per TU
    obj = {}          # (name, "exp"|"hasm"|"c") -> object path
    link_obj = {}     # name -> the object the link takes instead of obj[(name, "c")] (split data runs)
    base_tus = {t["name"]: t for t in json.loads((unit_dir / "tu-manifest.json").read_bytes())["tus"]}

    def base_manifest_pieces(name, sec):
        """The splat pieces of a section before the carve (what `data_carve.py split` reads)."""
        ts = base_tus[name]["sections"][sec]
        return ts.get("pieces") or [dict(name=name, start=ts["start"], end=ts["end"])]
    edges = []
    word_sites = []   # `.word` inside the text scaffold (reported)
    common_deps = ["include/include_asm.h", "include/common.h", "include/labels.inc"]
    for t in tus:
        n = t["name"]
        con = t["contract"]
        if t["text"]["splat"] == "c":
            # empty carve: the skeleton (all INCLUDE_ASM / ACCEPTED_ASM) under the object's contract
            deps = included_asm(unit_dir / t["scaffold_path"]) + common_deps
            for d in deps:
                if d.startswith("asm/"):
                    for i, l in enumerate((unit_dir / d).read_text().splitlines()):
                        if re.search(r"\*/\s+\.word\b", l):
                            word_sites.append(dict(tu=n, file=d, line=i + 1, text=l.strip(), reason=WORD_REASON))
            o = f"build/expected/{n}.o"
            edges += cc_edge(o, t["scaffold_path"], con, con["flags"], "-Iinclude -I.", deps)
            obj[(n, "exp")] = o
        elif t["text"]["splat"] == "hasm":
            o = f"build/hasm/{n}.o"
            edges += [f"build {o}: eeas_s {t['scaffold_path']} | include/labels.inc",
                      f"  eeas = {ROOT / con['assembler_path']}", f"  gflag = {con['flags'][0]}"]
            obj[(n, "hasm")] = o
            for i, l in enumerate((unit_dir / t["scaffold_path"]).read_text().splitlines()):
                if re.search(r"^\s*(/\*.*\*/)?\s*\.word\b", l):
                    word_sites.append(dict(tu=n, file=t["path"], line=i + 1, text=l.strip()))
        if t["mode"] == "c":
            imp = t["import"]
            src = imp["source"]
            flags = imp.get("flags", con["flags"])
            assert con["language"] == "c" and con["compiler_profile"] != "unassigned", n
            incs = []
            if src["kind"] == "units-shim":
                body = [f"/* Generated import shim for TU {n} ({t['id']}): one translation unit made of the",
                        " * accepted unit sources below, in address order (TU-plumbing proof; the importer",
                        f" * writes the merged source to {t['path']}). */"]
                if any("include_asm" in it for it in src["items"]):
                    body.append('#include "include_asm.h"')
                deps = list(common_deps)
                for it in src["items"]:
                    if "unit" in it:
                        if it["unit"] not in units:
                            raise SystemExit(
                                f"ninja_main: TU {n} builds from acceptance record "
                                f"{it['unit']}, which is not in this tree "
                                "(config/units is not tracked)")
                        rec = units[it["unit"]]
                        # `source_override`: another file compiled with this unit's include
                        # interface (negative controls / importer experiments)
                        p = Path(it["source_override"]) if "source_override" in it else ROOT / rec["source"]["path"]
                        body.append(f'#include "{p}"')
                        deps.append(str(p))
                        for r in unit_includes(rec):
                            if r not in incs:
                                incs.append(r)
                        deps += [str(ROOT / hd["path"]) for hd in rec["headers"]]
                    elif "accepted_asm" in it:
                        body.append(f'ACCEPTED_ASM("src/main/{n}", {it["accepted_asm"]});')
                        deps.append(f"src/main/{n}/{it['accepted_asm']}.s")
                    else:
                        fn = it["include_asm"]
                        body.append(f'INCLUDE_ASM("asm/main/nonmatchings/{n}", {fn});')
                        deps.append(f"asm/main/nonmatchings/{n}/{fn}.s")
                cfile = f"build/gen/shim/{n.replace('/', '__')}.c"
                write_if_changed(unit_dir / cfile, "\n".join(body) + "\n")
            else:
                # importer fragment: source path relative to main/ (src/main/<tu>.c or
                # ../tus/<...>.c), include_dirs repository-relative; "include" means the
                # staged shared headers in main/include
                # The published TU source is named by its path relative to the
                # unit directory, not through the unit's `src` link: cc1 records
                # the input path in `.file`, and `tools/tu_audit.py` spells the
                # same file as `relpath(source, unit_dir)`, so naming it that way
                # here makes the build's emission and the audit's identical.
                cfile = (os.path.relpath(ROOT / src["path"], unit_dir)
                         if src["path"].startswith("src/") else src["path"])
                incs = [Path(x) for x in src.get("include_dirs", []) if str(x) != "include"]
                deps = included_asm(unit_dir / cfile) + common_deps + src.get("deps", [])
                # the staged shared headers: include/shared.h and the per-TU
                # public headers include/<unit>/<tu>.h (task tu-header-layout)
                deps += [str(p) for p in sorted((unit_dir / "include").rglob("*.h"))
                         if p.name not in ("common.h", "include_asm.h")]
                local_h = (ROOT / src["path"]).with_suffix(".h") if src["path"].startswith("src/") else None
                if local_h is not None and local_h.is_file():
                    deps.append(str(local_h))
            inc_flags = " ".join(f"-I{ROOT / r}" for r in incs) + " -Iinclude -I."
            obj[(n, "c")] = f"build/c/{n}.o"
            edges += cc_edge(obj[(n, "c")], cfile, con, flags, inc_flags, deps + [str(HERE / "cinc.py")],
                             overlay=f"build/cinc/{n}")
            if t.get("c_link_object"):
                # several C runs in one data section (tools/tu/data_carve.py): the link takes
                # the compiled object with that section cut into one input section per run
                split_deps = sorted({str(ROOT / data_carve.REGISTRY), str(HERE / "data_carve.py"),
                                     str(HERE / "elfinfo.py"), "tu-manifest.json", "orig/SLUS_204.69"}
                                    | {pc.get("file") or f"asm/main/data/{pc['name']}.{sec[1:]}.s"
                                       for sec in t["c_split_sections"]
                                       for pc in base_manifest_pieces(n, sec)})
                edges += [f"build {t['c_link_object']}: carvesplit {obj[(n, 'c')]} | {' '.join(split_deps)}",
                          f"  tu = {t['id']}"]
                link_obj[n] = t["c_link_object"]

    # data pieces, blobs (modern GAS, splat modern dialect: include/macro.inc)
    def dobj(name, sec):
        return f"build/data/{name}.{sec[1:]}.o"

    for sec, v in m["sections"].items():
        for pc in v["order"]:
            src = pc.get("file") or f"asm/main/data/{pc['piece']}.{sec[1:]}.s"
            edges.append(f"build {dobj(pc['piece'], sec)}: as {src}")
    for sec, name in ((".sbss", "linker/scommon"), (".bss", "linker/common")):
        edges.append(f"build {dobj(name, sec)}: as asm/main/data/{name}.{sec[1:]}.s")
    for b in ("reginfo", "ctors", "dtors", "eh_frame"):
        edges.append(f"build build/data/blobs/{b}.o: as asm/main/data/blobs/{b}.data.s")
    # The VU0 microprogram: assembled from the recovered source by the DVP
    # assembler, not a blob.  `blobs/vu0packet` stays a generated databin, as the
    # reference tools/vu_audit.py compares the object against; it is not linked.
    edges.append("build build/vu0/Vu0MicroCode.o: dvpas src/main/vu0/Vu0MicroCode.dvp")
    edges.append("build build/data/blobs/vudata_rest.o: as asm/main/data/blobs/vudata_rest.s"
                 " | assets/main/blobs/vudata_rest.databin.bin")
    edges.append("build build/ov02/data.o: as asm/main/data/ov02/data.data.s")
    bins = sorted(p.stem for p in (unit_dir / "assets/main/blobs").glob("*.bin") if "databin" not in p.name)
    for b in bins:
        edges.append(f"build build/assets/{b}.o: bin assets/main/blobs/{b}.bin")

    # ------------------------------------------------ section contents
    def c_link(name):
        return link_obj.get(name, obj[(name, "c")])

    def text_obj(t, carve):
        if t["mode"] == "c" and not carve:
            return c_link(t["name"]), True
        return obj[(t["name"], "hasm" if t["text"]["splat"] == "hasm" else "exp")], False

    # TUs whose original object pads its .text to an 8-byte boundary after the
    # last function (tools/tu/tail_align.py). A C TU's object loses that padding
    # once its last function is C, so the script restores the declared property;
    # while the object still ends at text.end the statement is a no-op.
    tail_aligned = {(t["unit"], t["name"]) for t in tail_align.declaring_tus(ROOT)}

    def text_contents(tlist, base, carve, unit="main"):
        out, prev_c = [], False
        for t in tlist:
            if not t["text"]["splat"]:
                continue
            o, is_c = text_obj(t, carve)
            if prev_c and not is_c:
                out.append(f". = 0x{int(t['text']['splat_range'][0], 16) - base:X}; /* pin: {t['name']} after a C TU */")
            out.append(f"{tu_sym(t['name'])} = .;")
            if is_c:
                out.append(f"{piece_sym(t['name'], '.text')} = .;")
            out.append(f"{o}(.text);")
            if is_c and (unit, t["name"]) in tail_aligned:
                out.append(f"{tail_align.ld_statement()}; {tail_align.ld_comment(unit + '/' + t['name'])}")
            prev_c = is_c
        return out

    def contents(sec, carve):
        if sec == ".text":
            return text_contents(main_tus, S[".text"].addr, carve)
        out, prev_c = [], False
        for p in m["sections"][sec]["order"]:
            n = p["tu"]
            t = by_name[n]
            owner = t["owners"].get(sec)
            own = "c" if (t["mode"] == "c" and not carve
                          and (owner == "c" or (owner == "split" and p["c_split"]))) else "asm"
            if own == "c":
                csec = p.get("c_section")
                if csec:
                    # one run of a split section: pinned at its original address
                    out.append(f". = 0x{int(p['start'], 16) - S[sec].addr:X}; /* pin: C run {p['piece']} */")
                out.append(f"{piece_sym(n, csec or sec)} = .;")
                out.append(f"{c_link(n)}({csec or sec});")
                prev_c = True
                continue
            if prev_c:
                out.append(f". = 0x{int(p['start'], 16) - S[sec].addr:X}; /* pin: {p['piece']} after a C-owned piece */")
            out.append(f"{dobj(p['piece'], sec)}({sec});")
            prev_c = False
        tail = m["sections"][sec].get("common_tail")
        if tail:
            if prev_c:
                out.append(f". = 0x{int(tail['start'], 16) - S[sec].addr:X}; /* pin: common tail after a C-owned piece */")
            out.append(f"{dobj(tail['name'], sec)}({sec});")
            out.append("*(.scommon);" if sec == ".sbss" else "*(COMMON);")
        # every other contribution of this section (C/INCLUDE_ASM objects' unowned sections): must be empty
        out.append(f"build/expected/*({sec}) build/hasm/*({sec}) build/c/*({sec}); "
                   "/* code objects' other contributions: must stay empty (mapcheck) */")
        return out

    def ov02_contents(carve):
        return text_contents(ov_tus, S["ov02"].addr, carve, unit="ov02") + ["build/ov02/data.o(.data);"]

    # The VU0 microprogram's EE entry points.  Generated from the assembled object
    # by tools/vu_audit.py --emit-ld and tracked as config/symbols/main.vu0-symbols.ld.
    # It is required, not a convenience, and it cannot lag the /DISCARD/ of the DVP
    # sections: the object's own Vu0Call* symbols live in a discarded section, so
    # without the fragment an EE caller that references one fails the link
    # ("defined in discarded section").
    VU0_LD = f"INCLUDE {ROOT}/config/symbols/main.vu0-symbols.ld"

    def rom_script(carve):
        L = [VU0_LD,
             "/* Generated by tools/configure.py: ROM-style whole-file link" + (" (empty carve)" if carve else "") + " */",
             f"_gp = 0x{orig.gp:08X};", "SECTIONS", "{"]
        L += ["  .blob_elf_header 0x10000000 : AT(0) { build/assets/elf_header.o(.data); }"]
        L += [f"  .text 0x{S['.text'].addr:X} : AT(0x{S['.text'].offset:X})", "  {"]
        L += ["    " + x for x in contents(".text", carve)] + ["  }"]
        L += [f"  .blob_reginfo 0x{S['.reginfo'].addr:X} : AT(0x{S['.reginfo'].offset:X}) {{ build/data/blobs/reginfo.o(.data); }}"]
        L += [f"  .data 0x{S['.data'].addr:X} : AT(0x{S['.data'].offset:X})", "  {"] + \
             ["    " + x for x in contents(".data", carve)] + ["  }"]
        for b in ("ctors", "dtors", "eh_frame"):
            x = S["." + b]
            L += [f"  .blob_{b} 0x{x.addr:X} : AT(0x{x.offset:X}) {{ build/data/blobs/{b}.o(.data); }}"]
        x = S[".vudata"]
        L += [f"  .vu0packet 0x{x.addr:X} : AT(0x{x.offset:X}) {{ build/vu0/Vu0MicroCode.o(.vudata); }}"]
        L += [f"  .blob_vudata 0x{x.addr + VU0_PACKET:X} : AT(0x{x.offset + VU0_PACKET:X})"
              " { build/data/blobs/vudata_rest.o(.data); }"]
        for sec in (".rodata", ".lit4", ".sdata"):
            L += [f"  {sec} 0x{S[sec].addr:X} : AT(0x{S[sec].offset:X})", "  {"] + \
                 ["    " + x for x in contents(sec, carve)] + ["  }"]
        for sec in (".sbss", ".bss"):
            L += [f"  {sec} 0x{S[sec].addr:X} (NOLOAD) :", "  {"] + \
                 ["    " + x for x in contents(sec, carve)] + ["  }"]
        ov = S["ov02"]
        L += [f"  ov02 0x{ov.addr:X} : AT(0x{ov.offset:X})", "  {"] + ["    " + x for x in ov02_contents(carve)] + ["  }"]
        sd = S[".sdata"]
        L += [f"  .blob_pad_before_ov02 0x{0x10000000 + sd.offset + sd.size:X} : AT(0x{sd.offset + sd.size:X}) "
              f"{{ build/assets/pad_before_ov02.o(.data); }}"]
        for line in (unit_dir / "splat.yaml").read_text().splitlines():
            mm = re.match(r"  - \[0x([0-9A-F]+), bin, blobs/(\w+)\]", line)
            if mm and mm[2] not in ("elf_header", "pad_before_ov02"):
                offv = int(mm[1], 16)
                L += [f"  .blob_{mm[2]} 0x{0x10000000 + offv:X} : AT(0x{offv:X}) {{ build/assets/{mm[2]}.o(.data); }}"]
        L += ["  /DISCARD/ : { " + " ".join(DISCARD) + " }", "}", ""]
        return "\n".join(L)

    def elf_script():
        """ELF-style: original section names/VMAs/LMAs/PHDRS (manifest modes)."""
        L = [VU0_LD,
             "/* Generated by tools/configure.py: ELF-style link */",
             f"_gp = 0x{orig.gp:08X};", "PHDRS", "{"]
        for i, p in enumerate(orig.phdrs):
            kind = {1: "PT_LOAD", 0x70000000: "0x70000000"}[p.type]
            L.append(f"  seg{i} {kind} FLAGS({p.flags});")
        L += ["}", "SECTIONS", "{"]
        body = {".text": contents(".text", False), ".reginfo": ["build/data/blobs/reginfo.o(.data);"],
                ".ctors": ["build/data/blobs/ctors.o(.data);"], ".dtors": ["build/data/blobs/dtors.o(.data);"],
                ".eh_frame": ["build/data/blobs/eh_frame.o(.data);"], ".vudata": [f"build/vu0/Vu0MicroCode.o(.vudata);",
                            f". = 0x{VU0_PACKET:X};",
                            "build/data/blobs/vudata_rest.o(.data);"],
                "ov02": ov02_contents(False)}
        for sec in DATA_SECTIONS:
            body[sec] = contents(sec, False)
        for s in sorted((s for s in orig.sections if s.flags & 2), key=lambda s: (s.addr, s.index)):
            segs = [i for i, p in enumerate(orig.phdrs) if p.vaddr <= s.addr <= p.vaddr + p.memsz
                    and (p.type == 1 or (s.name == ".reginfo" and s.addr == p.vaddr))]
            loads = [i for i in segs if orig.phdrs[i].type == 1]
            ph = orig.phdrs[loads[0]]
            lma = ph.paddr + s.addr - ph.vaddr
            qual = " (NOLOAD)" if s.type == 8 else (" (TYPE = 0x70000006)" if s.name == ".reginfo" else "")
            at = (f" AT(0x{lma:X})" if s.type != 8 else "") + f" ALIGN({s.align})"
            L.append(f"  {s.name} 0x{s.addr:X}{qual} :{at}")
            L.append("  {")
            L += ["    " + x for x in body.get(s.name, [". = .;  /* keep the empty original section */"])]
            order = [loads[0]] + [i for i in segs if i != loads[0]]
            L.append("  } " + " ".join(f":seg{i}" for i in order))
        L += ["  /DISCARD/ : { " + " ".join(DISCARD) + " }", "}", ""]
        return "\n".join(L)

    # aliases: names accepted units use for asm-owned addresses (never for C-owned ones)
    alias_lines = ["/* names accepted unit records use for scaffold-owned addresses (generated) */"]
    for al in m["symbols"]["aliases"]:
        if al.get("target") and al["reason"] != "name used elsewhere":
            alias_lines.append(f"PROVIDE({al['name']} = {al['target']} + {al.get('offset', 0)});")
    write_if_changed(unit_dir / "aliases.ld", "\n".join(alias_lines) + "\n")

    # scaffold labels inside C-owned pieces: other (scaffold) objects may still reference
    # them (static data, jump-table labels, base+index addresses symbolized relative to a
    # neighbour); resolve them as <piece start> + original offset (PROVIDE: never overrides
    # a definition of the C object)
    c_alias = ["/* scaffold labels of C-owned pieces, relative to the linked C object (generated) */"]
    for t in tus:
        if t["mode"] != "c":
            continue
        n = t["name"]
        owned = []
        for sec, p in t["sections"].items():
            for pc in p.get("pieces", [dict(name=n, start=p["start"], c_split=False)]):
                if t["owners"].get(sec) == "c" or (t["owners"].get(sec) == "split" and pc["c_split"]):
                    owned.append((pc.get("c_section") or sec, int(pc["start"], 16),
                                  [unit_dir / (pc.get("file") or f"asm/main/data/{pc['name']}.{sec[1:]}.s")]))
        for sec, base, files in owned:
            for f in files:
                cur = None
                for line in f.read_text().splitlines():
                    mm = re.match(r"\s*(?:glabel|jlabel|dlabel|alabel|ehlabel)\s+(\S+?)(?:,.*)?$", line)
                    if mm:
                        cur = mm[1]
                        continue
                    am = re.match(r"\s*/\* [0-9A-F]+ ([0-9A-F]{8}) ", line)
                    if cur and am:
                        c_alias.append(f'PROVIDE("{cur}" = {piece_sym(n, sec)} + 0x{int(am[1], 16) - base:X});')
                        cur = None
    write_if_changed(unit_dir / "c_aliases.ld", "\n".join(c_alias) + "\n")
    # scaffold references to functions that INCLUDE_ASM now defines under their original
    # (local or un-suffixed) name: PROVIDE the splat spelling relative to the TU text start
    lab = json.loads((unit_dir / "scaffold-labels.json").read_bytes())
    sl = ["/* splat spellings of original LOCAL/duplicated function names (generated) */"]
    seen_alias = set()
    for x in lab:
        alias = x["splat_name"] if x["splat_name"] != x["name"] else x["name"]
        if alias in seen_alias:
            continue
        seen_alias.add(alias)
        off = int(x["va"], 16) - int(x["text_start"], 16)
        sl.append(f'PROVIDE("{alias}" = {tu_sym(x["tu"])} + 0x{off:X});')
    write_if_changed(unit_dir / "scaffold_aliases.ld", "\n".join(sl) + "\n")
    if records_available:
        ext = ["/* symbols accepted unit records take from other units (witnessed overlay addresses) */"]
        seen_ext = set()
        for rec in units.values():
            if rec.get("unit") != "main":
                continue
            for nm, ref in rec.get("references", {}).items():
                if ref.get("unit") != "main" and nm not in seen_ext:
                    seen_ext.add(nm)
                    ext.append(f"PROVIDE({nm} = {ref['va']}); /* {ref['unit']} */")
        # A published TU-mode source has no acceptance record, so the overlay
        # addresses it calls are added to the tracked file by hand: keep every
        # tracked PROVIDE the records do not already give, in tracked order.
        tracked = ROOT / "config/symbols/main.externals.ld"
        if tracked.is_file():
            for line in tracked.read_text().splitlines():
                provided = re.match(r"PROVIDE\((\w+)\s*=", line)
                if provided and provided.group(1) not in seen_ext:
                    seen_ext.add(provided.group(1))
                    ext.append(line)
        write_if_changed(unit_dir / "externals.ld", "\n".join(ext) + "\n")
    elif not (unit_dir / "externals.ld").is_file():
        raise SystemExit("ninja_main: externals.ld is missing; it is a tracked build "
                         "input (config/symbols/main.externals.ld) that "
                         "tools/tu/tracked_inputs.py stages before this runs")

    write_if_changed(unit_dir / "main.rom.ld", rom_script(False))
    write_if_changed(unit_dir / "main.carve.rom.ld", rom_script(True))
    write_if_changed(unit_dir / "main.elf.ld", elf_script())
    for old in ("main.asmonly.rom.ld",):
        if (unit_dir / old).exists():
            (unit_dir / old).unlink()

    def ld_objs(script):
        return sorted(set(re.findall(r"(build/[\w/.\-]+\.o)\(", (unit_dir / script).read_text())))

    rom_objs, carve_objs, elf_objs = ld_objs("main.rom.ld"), ld_objs("main.carve.rom.ld"), ld_objs("main.elf.ld")
    entry = orig.entry
    strict = ("( {T} $eeas -EL -m5900 -mabi=eabi $gflag -I . -I include -o $out {src} > $out.asmsg 2>&1 ) "
              "&& ! test -s $out.asmsg || {{ cat $out.asmsg 2>/dev/null; rm -f $out; false; }}")
    N = ["# Generated by tools/configure.py (main per-TU canonical carve base)",
         f"as = {T} {BIN}as", f"asflags = {ASFLAGS}", f"ld = {T} {BIN}ld", f"objcopy = {T} {BIN}objcopy",
         f"dvpas = {DVPAS}", f"py = {PY} -B", "",
         "rule as", "  command = $as $asflags -I . -I include -o $out $in", "  description = AS $in",
         "rule bin", "  command = printf '.section .data, \"wa\"\\n.incbin \"%s\"\\n' $in | $as $asflags -o $out -",
         "  description = BIN $in",
         "# VU microcode: the DVP assembler, with the contract's argv verbatim",
         "# (config/vu-build.json contracts[vu-dvp-as-v1].assembler.argv).",
         "rule dvpas", f"  command = {T} $dvpas {DVPASFLAGS} -o $out $in", "  description = DVPAS $in",
         "# C TU: cpp -> cc1 -> legacy assembler; ANY assembler message is an error (tu-asmcompat rule)",
         "# Header dependencies: cpp writes them (-Wp,-MD) naming the object by its basename;",
         "# the target is rewritten to $out and ninja records them (deps = gcc), so an edit to",
         "# any header a TU includes -- one created after configure too -- recompiles it. The",
         "# preprocessed text, and so every output, is unchanged by the option.",
         "rule ccas", f"  command = {T} $ccdir/ee-gcc -B$ccdir/ -nostdinc -fno-builtin -E $flags $incs $in -o $out.i "
         f"-Wp,-MD,$out.d.raw && sed -e '1s|^[^:]*:|$out:|' $out.d.raw > $out.d && rm -f $out.d.raw "
         f"&& {T} $ccdir/ee-gcc -B$ccdir/ -nostdinc -fno-builtin -S $flags $out.i -o $out.s && "
         + strict.format(T=T, src="$out.s"),
         "  depfile = $out.d", "  deps = gcc",
         "  description = CC $in",
         "# C-mode TU: the scaffold asm it includes comes from a per-TU overlay without `.extern` lines for",
         "# symbols the TU declares/defines in C (tools/cinc.py, review finding F3)",
         "rule ccas_c", f"  command = {T} $ccdir/ee-gcc -B$ccdir/ -nostdinc -fno-builtin -E $flags $incs $in -o $out.i "
         f"-Wp,-MD,$out.d.raw && sed -e '1s|^[^:]*:|$out:|' $out.d.raw > $out.d && rm -f $out.d.raw "
         f"&& {T} $ccdir/ee-gcc -B$ccdir/ -nostdinc -fno-builtin -S $flags $out.i -o $out.s && "
         f"{T} $py {HERE}/cinc.py --i $out.i --s $out.s --root . --dst $overlay --report $out.cinc.json && "
         f"( cd $overlay && {T} $eeas -EL -m5900 -mabi=eabi $gflag -I . -I {unit_dir} -I {unit_dir}/include "
         f"-o {unit_dir}/$out {unit_dir}/$out.s > {unit_dir}/$out.asmsg 2>&1 ) "
         "&& ! test -s $out.asmsg || { cat $out.asmsg 2>/dev/null; rm -f $out; false; }",
         "  depfile = $out.d", "  deps = gcc",
         "  description = CC(c) $in",
         "rule eeas_s", "  command = " + strict.format(T=T, src="$in"), "  description = EE-AS $in",
         "# several C runs in one data section: one input section per run (tools/tu/data_carve.py split)",
         "rule carvesplit", f"  command = {T} $py {HERE}/data_carve.py split --root {ROOT} --unit main "
         f"--unit-dir {unit_dir} --tu $tu --obj $in --out $out", "  description = CARVE-SPLIT $out",
         "# splat's undefined_*_auto.txt minus every name a linked object defines (review finding F8)",
         "rule undeffilter", f"  command = {T} $py {HERE}/undef_filter.py --objects @$out.rsp --out $out --report $out.json",
         "  rspfile = $out.rsp", "  rspfile_content = $objs", "  description = UNDEF-FILTER $out",
         "rule ld", "  command = $ld -EL -m elf32lr5900 --no-undefined -T $undef "
         "-T aliases.ld -T scaffold_aliases.ld $extra -T $script -Map $out.map -o $out", "  description = LD $out",
         "rule ld_elf", "  command = $ld -EL -m elf32lr5900 --no-undefined -z max-page-size=4096 -e $entry "
         "-T $undef -T aliases.ld -T scaffold_aliases.ld $extra -T $script -Map $out.map -o $out",
         "  description = LD(elf) $out",
         "rule flat", "  command = $objcopy -O binary $in $out", "  description = OBJCOPY $out",
         "rule compare", f"  command = $py {HERE}/compare.py $mode --original orig/SLUS_204.69 --rebuilt $in --report $out",
         "  description = COMPARE $in",
         "rule mapcheck", f"  command = {T} $py {HERE}/mapcheck.py --map $in.map --report $out --rom $carve"
         + (f" --manifest {manifest_name}" if carve_report else ""),
         "  description = MAPCHECK $in",
         "# ACCEPTED_ASM/INCLUDE_ASM provenance (tu-main-v2 review finding 3)",
         "rule provenance",
         f"  command = {T} $py {HERE}/accepted_asm_gate.py --root {ROOT} --unit-dir {unit_dir} --report $out",
         "  description = PROVENANCE $out", ""]
    N += edges
    prov_deps = sorted({t["scaffold_path"] for t in tus if t["text"]["splat"] == "c"} |
                       {(f"build/gen/shim/{t['name'].replace('/', '__')}.c"
                         if t["import"]["source"]["kind"] == "units-shim" else t["import"]["source"]["path"])
                        for t in tus if t["mode"] == "c"} |
                       {str(p.relative_to(unit_dir)) for p in (unit_dir / "src/main").rglob("*.s")})
    # The accepted records the gate compares those includes against. They are an
    # input of the check like the .s files are, and in a worker sandbox they are
    # private copies the allocation may narrow (tools/worker.py tu_stage_config),
    # so a record edited on its own has to re-run the gate rather than leave a
    # stale verdict standing.
    prov_deps += sorted({os.path.relpath(ROOT / r["unit_record"], unit_dir)
                         for t in tus for r in t["accepted"]["records"]
                         if r["language"] == "ee-asm" and (ROOT / r["unit_record"]).is_file()})
    # the generated VU0 entry-point fragment is an input of every link that
    # includes it, so an --emit-ld refresh re-links instead of leaving a stale image
    common = f"aliases.ld scaffold_aliases.ld {ROOT}/config/symbols/main.vu0-symbols.ld"
    auto = "undefined_syms_auto.txt undefined_funcs_auto.txt"
    N += ["",
          f"build build/main.undefined.ld: undeffilter | {' '.join(sorted(set(rom_objs) | set(elf_objs)))} {auto}",
          f"  objs = {' '.join(sorted(set(rom_objs) | set(elf_objs)))}",
          f"build build/carve/main.undefined.ld: undeffilter | {' '.join(carve_objs)} {auto}",
          f"  objs = {' '.join(carve_objs)}",
          f"build build/main.rom.elf: ld | {' '.join(rom_objs)} main.rom.ld {common} c_aliases.ld externals.ld build/main.undefined.ld",
          "  script = main.rom.ld", "  extra = -T c_aliases.ld -T externals.ld", "  undef = build/main.undefined.ld",
          "build build/main.bin: flat build/main.rom.elf",
          "build build/main.compare.json: compare build/main.bin", "  mode =",
          f"build build/accepted_asm.json: provenance | tu-manifest.json {HERE}/accepted_asm_gate.py " + " ".join(prov_deps),
          f"build build/main.mapcheck.json: mapcheck build/main.rom.elf | {manifest_name} {HERE}/mapcheck.py " + " ".join([o for (n, k), o in obj.items() if k == "c"] + sorted(link_obj.values())), "  carve =",
          f"build build/carve/main.rom.elf: ld | {' '.join(carve_objs)} main.carve.rom.ld {common} build/carve/main.undefined.ld",
          "  script = main.carve.rom.ld", "  extra =", "  undef = build/carve/main.undefined.ld",
          "build build/carve/main.bin: flat build/carve/main.rom.elf",
          "build build/carve/main.compare.json: compare build/carve/main.bin", "  mode =",
          f"build build/carve/main.mapcheck.json: mapcheck build/carve/main.rom.elf | {manifest_name} {HERE}/mapcheck.py",
          "  carve = --carve",
          f"build build/main.elf: ld_elf | {' '.join(elf_objs)} main.elf.ld {common} c_aliases.ld externals.ld build/main.undefined.ld",
          "  script = main.elf.ld", "  extra = -T c_aliases.ld -T externals.ld", "  undef = build/main.undefined.ld",
          f"  entry = 0x{entry:X}",
          "build build/main.elf.compare.json: compare build/main.elf", "  mode = --elf",
          "build expected: phony " + " ".join(o for (n, k), o in sorted(obj.items()) if k in ("exp", "hasm")),
          "build carve: phony build/carve/main.compare.json build/carve/main.mapcheck.json",
          "build elf: phony build/main.elf.compare.json",
          "default build/main.compare.json build/main.mapcheck.json build/carve/main.compare.json "
          "build/carve/main.mapcheck.json build/main.elf.compare.json build/accepted_asm.json", ""]
    write_if_changed(unit_dir / "build.ninja", "\n".join(N))
    write_if_changed(unit_dir / "build/gen/word_sites.json", json.dumps(word_sites, indent=1) + "\n")
    print(json.dumps(dict(objects_rom=len(rom_objs), objects_carve=len(carve_objs), objects_elf=len(elf_objs),
                          c_tus=[t["name"] for t in tus if t["mode"] == "c"], word_sites=len(word_sites))))


if __name__ == "__main__":
    main()
