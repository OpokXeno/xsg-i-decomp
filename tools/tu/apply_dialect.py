#!/usr/bin/env python3
"""Apply the asmcompat legacy-dialect rewrite to the INCLUDE_ASM bodies of a tree.

    apply_dialect.py <out-root> <unit> [<unit> ...]

Runs tools/legacy_dialect.py (unmodified copy of
.work/tu-migration/asmcompat/tools/legacy_dialect.py, sha256 pinned below) with its
defaults (--gp symbolic --vsqrt mnemonic) over <out-root>/<unit>/asm/nonmatchings
into <out-root>/_dialect, then replaces each unit's asm/nonmatchings and
include/labels.inc with the rewritten files.  The rewrite is spelling-only
(vsqrt `$Q` -> `Q`, `$ACC/$Q/$I/$R` -> bare, `c1 0x..` -> sqrt.s, %gp_rel ->
symbolic + .extern; labels.inc: alabel without .aent, jlabel global); its
manifest is kept as <out-root>/legacy-dialect-manifest.json.
"""
import hashlib
import shutil
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
TOOL = HERE / "legacy_dialect.py"
TOOL_SHA = "8fe8cec8eb7ba2048d796d6bf276a53dad017da2b9fcb84aafe9ef365af94464"


def main():
    out, units = Path(sys.argv[1]).resolve(), sys.argv[2:]
    assert hashlib.sha256(TOOL.read_bytes()).hexdigest() == TOOL_SHA, "legacy_dialect.py differs from the asmcompat pin"
    tmp = out / "_dialect"
    if tmp.exists():
        shutil.rmtree(tmp)
    subprocess.run(["timeout", "-k", "5", "120", sys.executable, "-B", str(TOOL), "--src", str(out), "--dst", str(tmp),
                    "--units", ",".join(units)], check=True)
    for u in units:
        dst = out / u / "asm/nonmatchings"
        shutil.rmtree(dst)
        shutil.copytree(tmp / u / "asm/nonmatchings", dst)
        shutil.copyfile(tmp / u / "include/labels.inc", out / u / "include/labels.inc")
    shutil.copyfile(tmp / "manifest.json", out / "legacy-dialect-manifest.json")
    shutil.rmtree(tmp)


if __name__ == "__main__":
    main()
