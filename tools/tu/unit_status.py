#!/usr/bin/env python3
"""Summarise one unit's gate into `build/<unit>/status.json`.

    unit_status.py --unit main --unit-dir build/main --out build/main/status.json

The whole-file gate of a unit is the conjunction of what its own build already
produced:

  <unit>.compare.json          the ROM-style image is byte-identical
  carve/<unit>.compare.json    the same build with every TU taken from its
                               generated skeleton (main only): it must never
                               change, and it proves the C TUs are the only
                               difference
  <unit>.mapcheck.json         every C-owned section lands at its original
                               address with the original size (main only)
  accepted_asm.json            every ACCEPTED_ASM names a tracked .s under
                               src/<unit>/ that hashes to its accepted record,
                               and every INCLUDE_ASM names a generated
                               nonmatching (main only)

`tools/coverage_report.py` reads the `result` of this file as check (3): a
function is only counted when its unit's gate passes. A missing input is a
failure, never a silent pass, and this file is a report — not acceptance.
"""
import argparse
import json
import sys
from pathlib import Path

sys.dont_write_bytecode = True


def read(path):
    try:
        return json.loads(Path(path).read_bytes()), None
    except FileNotFoundError:
        return None, "missing"
    except ValueError as error:
        return None, f"unreadable ({error})"


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--unit", required=True)
    ap.add_argument("--unit-dir", type=Path, required=True)
    ap.add_argument("--out", type=Path, required=True)
    a = ap.parse_args(argv)

    build = a.unit_dir / "build"
    checks = {}
    failures = []

    def check(name, rel, key, optional=False):
        report, error = read(build / rel)
        if report is None:
            if optional:
                return
            checks[name] = dict(report=str(rel), ok=False, reason=error)
            failures.append(f"{name}: {error}")
            return
        ok = bool(report.get(key))
        checks[name] = dict(report=str(rel), ok=ok,
                            sha256=report.get("rebuilt_sha256"),
                            problems=len(report.get("problems", [])))
        if not ok:
            failures.append(f"{name}: {key} is false")

    check("whole_file", f"{a.unit}.compare.json", "whole_file_identical")
    check("carve", f"carve/{a.unit}.compare.json", "whole_file_identical", optional=True)
    check("mapcheck", f"{a.unit}.mapcheck.json", "ok", optional=True)
    check("accepted_asm", "accepted_asm.json", "ok", optional=True)

    status = dict(schema="unit-status/1", unit=a.unit,
                  result="pass" if not failures else "fail",
                  identical=checks.get("whole_file", {}).get("ok", False),
                  checks=checks, failures=failures,
                  note="gate report only; acceptance needs the TU audit and an "
                       "independent review (AGENTS.md)")
    a.out.parent.mkdir(parents=True, exist_ok=True)
    a.out.write_text(json.dumps(status, indent=1, sort_keys=True) + "\n")
    print(json.dumps(dict(unit=a.unit, result=status["result"], failures=failures)))
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
