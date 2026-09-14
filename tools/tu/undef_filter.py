#!/usr/bin/env python3
"""Filter splat's undefined_*_auto.txt against the objects of one link
(tu-asmcompat review finding F8).

A linker-script assignment overrides an object's definition of the same name,
so a misplaced label would be invisible to the whole-file gate.  Every name that
a linked object defines (global/weak, in a real section) is dropped from the
assignments; the dropped list is reported.

    undef_filter.py --objects @rspfile --out build/x.undefined.ld --report build/x.undefined.json
"""
import argparse
import json
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from elfinfo import Elf  # noqa: E402

ASSIGN = re.compile(r"^\s*([^\s=]+)\s*=\s*(0x[0-9A-Fa-f]+|\d+)\s*;")
SHN_UNDEF, SHN_LORESERVE = 0, 0xFF00


def defined(obj):
    e = Elf(Path(obj).read_bytes())
    return {s.name for s in e.symbols if s.name and s.bind in (1, 2) and SHN_UNDEF < s.shndx < SHN_LORESERVE}


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--objects", required=True)
    ap.add_argument("--out", type=Path, required=True)
    ap.add_argument("--report", type=Path, required=True)
    ap.add_argument("inputs", nargs="*", default=["undefined_syms_auto.txt", "undefined_funcs_auto.txt"])
    a = ap.parse_args()
    objs = Path(a.objects[1:]).read_text().split() if a.objects.startswith("@") else a.objects.split()
    names = {}
    for o in objs:
        for n in defined(o):
            names.setdefault(n, o)
    keep, dropped = [], []
    for inp in a.inputs:
        for line in Path(inp).read_text().splitlines():
            m = ASSIGN.match(line)
            if m and m[1] in names:
                dropped.append(dict(name=m[1], value=m[2], file=inp, defined_by=names[m[1]]))
                continue
            keep.append(line)
    a.out.write_text("/* undefined_*_auto.txt minus names defined by the linked objects (generated) */\n"
                     + "\n".join(keep) + "\n")
    a.report.write_text(json.dumps(dict(objects=len(objs), kept=sum(1 for l in keep if ASSIGN.match(l)),
                                        dropped=dropped), indent=1) + "\n")


if __name__ == "__main__":
    main()
