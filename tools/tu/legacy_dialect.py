#!/usr/bin/env python3
"""Rewrite splat/spimdisasm function asm into the GNU as 2.9-ee-991111 dialect
(target: ee-legacy-binutils-la29-vsqrt-v1, the user-approved single assembler for
game and runtime objects; also valid for LA29/legacy except vsqrt, see below).

Spelling-only transformations over asm/nonmatchings/**/*.s (every instruction
keeps its original-word comment; the rewritten operand denotes the same
encoding, proven by tools/sweep.py and tools/verify_full.py against the original
bytes):

  gp   `OP REG, %gp_rel(SYM[ + OFF])($28)`  (legacy as has no %gp_rel operator)
         symbolic (default): `OP REG, SYM[ + OFF]` plus `.extern SYM, N` at the
             top of the file, i.e. exactly how GCC 2.96 -G8 spells a small-data
             access; the legacy assembler selects the one-instruction $gp form and
             emits R_MIPS_GPREL16 against SYM.  Requires the translation unit to be
             assembled with -G >= N (N <= 8 is chosen).  `addiu REG, $28, %gp_rel(X)`
             becomes `la REG, X` (legacy: addiu REG,$gp,X + GPREL16).
         numeric: `OP REG, <SYM+OFF-_gp>($28)`: no macro, no relocation; valid
             under any -G, but the symbol binding is lost (fallback only).  Also
             used, line by line, for linker-reserved names (_fbss, _gp, end, ...)
             that the 2.9-ee assembler never addresses off $gp.
  vsqrt  `vsqrt $Q, $vfNf` (splat hook spelling for GAS 2.45) is rejected by the
         2.9-ee-991111 parser (the Q operand is the bare letter).
         --vsqrt mnemonic (default): `vsqrt Q, $vfNf`.  Encodes the original word
             (bit 21 set, 0x4A2xxxBD) with the user-approved single game/runtime
             assembler ee-legacy-binutils-la29-vsqrt-v1 (vsqrt base 0x4a2003bd) and
             with GNU as 2.10-ee-001003-1; flagged `needs_vsqrt_table_fix`.
         --vsqrt word: only for the unrepaired legacy/LA29 assemblers, whose
             entry {"vsqrt","Q,7",0x4a0003bd,0xfe60ffff} cannot set FSF bit 21
             (user policy 2026-09-11: `.word` only where the pinned assembler cannot
             encode the original instruction): `.word 0x<orig>` with the semantic
             instruction and reason in a comment; sites in manifest `word_sites`.
  vu     `$ACC`/`$Q`/`$I`/`$R` operands (splat asm-subsegment spelling) -> bare
         `ACC`/`Q`/`I`/`R` (splat's C-subsegment output already uses the bare form).
  c1     `c1 0xIMM` emitted by rabbitizer for the R5900 `sqrt.s fd, ft` encoding
         (COP1 fmt S, funct 4, fs 0) -> `sqrt.s $fD, $fT` (legacy table entry
         "D,T" for the R5900), removing a raw-cofun opcode spelling.
  labels include/labels.inc: `alabel` without `.aent` (unknown to the legacy
         assembler; .aent only feeds .mdebug/.pdr, never section bytes), and
         `jlabel` defaults to global like splat's modern macro.inc (jump tables
         in separately assembled .rodata reference those labels).

    legacy_dialect.py --src units --dst dialect-sym [--gp symbolic|numeric] [--G 8] [--vsqrt mnemonic|word]
"""
import argparse
import json
import re
import shutil
from pathlib import Path

GP = 0x004DFB70
GEN = re.compile(r"^(?:D|func|jtbl|\.L)_?([0-9A-Fa-f]+)$")
LOADSTORE = re.compile(r"^(?P<pre>\s*/\*[^*]*\*/\s+)(?P<op>[a-z0-9.]+)(?P<sp>\s+)(?P<reg>\$[a-z0-9]+), "
                       r"%gp_rel\((?P<sym>[A-Za-z_.$][A-Za-z0-9_.$]*)(?: \+ (?P<off>0x[0-9A-Fa-f]+))?\)\(\$28\)\s*$")
ADDIU = re.compile(r"^(?P<pre>\s*/\*[^*]*\*/\s+)addiu(?P<sp>\s+)(?P<reg>\$[0-9]+), \$28, "
                   r"%gp_rel\((?P<sym>[A-Za-z_.$][A-Za-z0-9_.$]*)(?: \+ (?P<off>0x[0-9A-Fa-f]+))?\)\s*$")
VSQRT = re.compile(r"^(?P<pre>\s*/\*[^*]*\*/\s+vsqrt\s+)\$Q,")
VSQRT_FULL = re.compile(r"^(?P<lead>\s*/\* (?P<rom>[0-9A-F]+) (?P<va>[0-9A-F]+) (?P<le>[0-9A-F]{8}) \*/\s+)"
                        r"vsqrt\s+\$Q, (?P<ft>\$vf\d+[xyzw])\s*$")
VUSPECIAL = re.compile(r"^(?P<pre>\s*/\*[^*]*\*/\s+v[a-z0-9.]+\s+)(?P<ops>.*\$(?:ACC|Q|I|R)\b.*)$")
C1 = re.compile(r"^(?P<pre>\s*/\*[^*]*\*/\s+)c1(?P<sp>\s+)0x(?P<imm>[0-9A-Fa-f]+)\s*$")
WIDTH = {"lb": 1, "lbu": 1, "sb": 1, "lh": 2, "lhu": 2, "sh": 2, "lw": 4, "sw": 4, "lwc1": 4,
         "swc1": 4, "ld": 8, "sd": 8, "addiu": 1}
RESERVED = {"eprol", "etext", "_gp", "edata", "_fbss", "_fdata", "_ftext", "end", "_gp_disp"}


def load_symbols(unit_dir):
    syms, sizes = {}, {}
    for name in ("symbol_addrs.txt", "undefined_syms_auto.txt", "undefined_funcs_auto.txt"):
        p = unit_dir / name
        if p.exists():
            for line in p.read_text().splitlines():
                m = re.match(r"\s*([^\s=]+)\s*=\s*(0x[0-9A-Fa-f]+|\d+)\s*;(.*)", line)
                if m:
                    syms.setdefault(m[1], int(m[2], 0))
                    sz = re.search(r"size:(0x[0-9A-Fa-f]+|\d+)", m[3])
                    if sz:
                        sizes.setdefault(m[1], int(sz[1], 0))
    return syms, sizes


def addr_of(sym, table):
    if sym in table:
        return table[sym]
    m = GEN.match(sym)
    return int(m[1], 16) if m else None


def convert(text, mode, G, table, sizes, stats, vsqrt_mode="word", sites=None):
    out, externs, flags = [], {}, set()
    for line in text.splitlines():
        m = LOADSTORE.match(line) or ADDIU.match(line)
        if m:
            op = "addiu" if m.re is ADDIU else m["op"]
            sym, off = m["sym"], int(m["off"], 16) if m["off"] else 0
            if sym in RESERVED:
                # tc-mips.c nopic_need_relax never addresses these linker names off $gp;
                # the real fix is a splat symbol input naming the variable (see report),
                # this numeric fallback only keeps such input safe.
                flags.add("reserved_gp_symbol_numeric:" + sym)
            if mode == "symbolic" and sym not in RESERVED:
                expr = sym + (f" + 0x{off:X}" if off else "")
                line = (f"{m['pre']}la{m['sp']}{m['reg']}, {expr}" if op == "addiu"
                        else f"{m['pre']}{op}{m['sp']}{m['reg']}, {expr}")
                size = sizes.get(sym, 0)
                n = size if 0 < size <= G else min(WIDTH[op], G)
                externs[sym] = max(externs.get(sym, 0), n)
            else:
                a = addr_of(sym, table)
                if a is None:
                    raise SystemExit(f"numeric gp: unresolved {sym}")
                v = a + off - GP
                assert -0x8000 <= v < 0x8000, (sym, hex(v))
                num = f"-0x{-v:X}" if v < 0 else f"0x{v:X}"
                line = (f"{m['pre']}addiu{m['sp']}{m['reg']}, $28, {num}" if op == "addiu"
                        else f"{m['pre']}{op}{m['sp']}{m['reg']}, {num}($28)")
            stats["gp_rel"] += 1
        elif VSQRT.match(line):
            if vsqrt_mode == "word":
                mm = VSQRT_FULL.match(line)
                assert mm, line
                word = int.from_bytes(bytes.fromhex(mm["le"]), "little")
                sem = f"vsqrt Q, {mm['ft']}"
                reason = ("GNU as 2.9-ee-991111 (legacy/LA29) vsqrt entry Q,7 (match 0x4a0003bd, "
                          "mask 0xfe60ffff) cannot set FSF bit 21 of the original word")
                line = f"{mm['lead']}.word 0x{word:08X} /* {sem}: {reason} */"
                if sites is not None:
                    sites.append(dict(va=f"0x{mm['va']}", file_offset=f"0x{mm['rom']}", word=f"0x{word:08X}",
                                      instruction=sem, reason=reason))
                flags.add("word_site:vsqrt")
            else:
                line = VSQRT.sub(lambda mm: mm["pre"] + "Q,", line)
                flags.add("needs_vsqrt_table_fix:vsqrt")
            stats["vsqrt"] += 1
        elif VUSPECIAL.match(line):
            # asm-subsegment (named-register) spelling: the 2.9-ee parser takes the
            # VU special registers as bare letters (tc-mips.c operand U/Q/J/X).
            m = VUSPECIAL.match(line)
            line = m["pre"] + re.sub(r"\$(ACC|Q|I|R)\b", r"\1", m["ops"])
            stats["vu_special_dollar"] = stats.get("vu_special_dollar", 0) + 1
        else:
            m = C1.match(line)
            if m:
                imm = int(m["imm"], 16)
                word = 0x46000000 | imm
                fmt, ft, fs, fd, funct = (word >> 21) & 31, (word >> 16) & 31, (word >> 11) & 31, (word >> 6) & 31, word & 63
                if fmt == 16 and funct == 4 and fs == 0:
                    line = f"{m['pre']}sqrt.s{m['sp']}$f{fd}, $f{ft}"
                    stats["c1_sqrt_s"] += 1
                else:
                    flags.add(f"unhandled_c1:0x{imm:X}")
        out.append(line)
    if externs:
        if G <= 0:
            raise SystemExit("symbolic gp spelling needs -G > 0")
        head = [f"/* legacy dialect: small-data declarations for the gp-relative accesses below (-G{G}) */"]
        head += [f".extern {s}, {n}" for s, n in sorted(externs.items())]
        out = head + out
    return "\n".join(out) + "\n", flags


LABELS_ALABEL = re.compile(r"(\.macro alabel[^\n]*\n(?:[^\n]*\n)*?)\s*\.aent \\label\n")


def legacy_labels(src_text):
    t = LABELS_ALABEL.sub(lambda m: m[1], src_text, count=1)
    t = t.replace(".macro jlabel label, visibility=local", ".macro jlabel label, visibility=global", 1)
    assert ".aent" not in t and "jlabel label, visibility=global" in t
    return ("# Legacy (GNU as 2.9-ee-991111) variant generated by asmcompat/tools/legacy_dialect.py:\n"
            "# alabel without .aent; jlabel global by default.\n" + t)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--src", type=Path, required=True)
    ap.add_argument("--dst", type=Path, required=True)
    ap.add_argument("--gp", choices=["symbolic", "numeric"], default="symbolic")
    ap.add_argument("--G", type=int, default=8)
    ap.add_argument("--vsqrt", choices=["word", "mnemonic"], default="mnemonic")
    ap.add_argument("--units", default="main,ov01,ov02,ov10,ov11,ov12")
    a = ap.parse_args()
    manifest = dict(mode=a.gp, G=a.G, vsqrt=a.vsqrt, units={}, flags={}, word_sites=[])
    for u in a.units.split(","):
        table, sizes = load_symbols(a.src / u)
        stats = dict(files=0, gp_rel=0, vsqrt=0, c1_sqrt_s=0)
        sbase, dbase = a.src / u / "asm/nonmatchings", a.dst / u / "asm/nonmatchings"
        for f in sorted(sbase.rglob("*.s")):
            rel = f.relative_to(sbase)
            sites = []
            text, flags = convert(f.read_text(), a.gp, a.G, table, sizes, stats, a.vsqrt, sites)
            for site in sites:
                manifest["word_sites"].append(dict(unit=u, function_file=rel.as_posix(), **site))
            d = dbase / rel
            d.parent.mkdir(parents=True, exist_ok=True)
            d.write_text(text)
            stats["files"] += 1
            if flags:
                manifest["flags"][f"{u}:{rel.as_posix()}"] = sorted(flags)
        inc = a.dst / u / "include"
        inc.mkdir(parents=True, exist_ok=True)
        (inc / "labels.inc").write_text(legacy_labels((a.src / u / "include/labels.inc").read_text()))
        manifest["units"][u] = stats
    (a.dst / "manifest.json").write_text(json.dumps(manifest, indent=1))
    print(json.dumps(dict(units=manifest["units"], flagged=len(manifest["flags"])), indent=1))


if __name__ == "__main__":
    main()
