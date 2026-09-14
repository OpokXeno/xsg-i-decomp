#!/usr/bin/env python3
"""Per-C-TU overlay of the scaffold asm a C TU pulls in (tu-asmcompat review F3).

In a C TU, the C-emitted declaration must govern a small-data symbol: every
`.extern SYM, N` line of an INCLUDE_ASM/ACCEPTED_ASM body is dropped when the
compiler output of the TU itself declares SYM (`.extern SYM, size`) or defines it
(label, `.comm`, `.lcomm`).  A symbol the C only mentions without cc1 emitting a
declaration keeps the scaffold line (otherwise the legacy assembler would lose
the small-data size and expand the access through $at).  The
shared generated files stay untouched (the all-INCLUDE_ASM `expected` object of
the same TU still needs them); the filtered copies go to an overlay directory
that the C TU's assembler searches first (build.ninja: cd <overlay>, -I
<overlay> before -I <main>).

    cinc.py --i build/c/<tu>.o.i --s build/c/<tu>.o.s --root <main dir> --dst build/cinc/<tu> --report out.json
"""
import argparse
import json
import re
from pathlib import Path

INCLUDE = re.compile(r'^\s*\.include\s+"([^"]+)"', re.M)
EXTERN = re.compile(r"^\.extern ([A-Za-z_.$][A-Za-z0-9_.$]*), \d+\s*$")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--i", type=Path, required=True)
    ap.add_argument("--s", type=Path, required=True, help="cc1 output of the TU")
    ap.add_argument("--root", type=Path, required=True)
    ap.add_argument("--dst", type=Path, required=True)
    ap.add_argument("--report", type=Path, required=True)
    a = ap.parse_args()
    cc = a.s.read_text()
    includes = [p for p in INCLUDE.findall(cc) if p.endswith(".s")]
    idents = set(re.findall(r"^\s*\.extern\s+([A-Za-z_.$][\w.$]*)\s*,", cc, re.M))
    idents |= set(re.findall(r"^([A-Za-z_.$][\w.$]*):", cc, re.M))
    idents |= set(re.findall(r"^\s*\.(?:l?comm)\s+([A-Za-z_.$][\w.$]*)\s*,", cc, re.M))
    dropped = {}
    a.dst.mkdir(parents=True, exist_ok=True)
    for rel in includes:
        lines = (a.root / rel).read_text().splitlines()
        keep = [l for l in lines if not (EXTERN.match(l) and EXTERN.match(l)[1] in idents)]
        if len(keep) != len(lines):
            dropped[rel] = sorted(EXTERN.match(l)[1] for l in lines if EXTERN.match(l) and l not in keep)
        out = a.dst / rel
        out.parent.mkdir(parents=True, exist_ok=True)
        new = "\n".join(keep) + "\n"
        if not out.exists() or out.read_text() != new:
            out.write_text(new)
    a.report.write_text(json.dumps(dict(includes=includes, dropped_extern=dropped), indent=1) + "\n")


if __name__ == "__main__":
    main()
