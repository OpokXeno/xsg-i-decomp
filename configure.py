#!/usr/bin/env python3
"""Configure the per-TU build of the six EE files.

    ./configure.py && ninja          # build and compare all six originals
    ./configure.py && ninja gate     # + map check, accepted-asm provenance, elf_gate

The work is done by `tools/configure.py`; this entry point only keeps the usual
`./configure.py && ninja` flow at the repository root.  See docs/tu-build.md.
"""
import runpy
import sys
from pathlib import Path

sys.dont_write_bytecode = True

sys.argv[0] = str(Path(__file__).resolve().parent / "tools/configure.py")
runpy.run_path(sys.argv[0], run_name="__main__")
