#!/usr/bin/env python3
"""Write the decomp.dev progress report (objdiff report schema v2, JSON).

decomp.dev reads an objdiff `Report` from a GitHub Actions artifact named
`<version>_report` (here `SLUS_204.69_report`). This tool produces that report
from the same tracked inputs and the same counting rule as
`tools/coverage_report.py`, so the README table and decomp.dev always agree:

  - one report unit per in-scope original object, named `<unit>/<source stem>`,
    with one item per original function;
  - a function is matched (100%) when the published source of its object carries
    it as C, C with constrained inline asm, or accepted asm; otherwise 0%. There
    is no partial credit: similarity scores are diagnostics, never progress;
  - one progress category per original file (SLUS_204.69, the five overlays and
    the two IOP modules). The IOP modules have no build yet, so they appear as
    one unit per debug-file group with their function counts and 0%.

Out-of-scope objects (SCE SDK, runtime) are left out, as in the README.

Build-free: it needs neither the originals nor the toolchain, so CI runs it with
Python alone. A unit whose `build/<unit>/status.json` reports a failing
whole-file gate is withdrawn, exactly as the README count does.

Python 3 standard library only.

    python3 -B tools/objdiff_report.py                     # build/report.json
    python3 -B tools/objdiff_report.py -o path/report.json
    python3 -B tools/objdiff_report.py --stdout
"""
import argparse
import json
from pathlib import Path
import sys

sys.dont_write_bytecode = True
sys.path.insert(0, str(Path(__file__).resolve().parent))

import coverage_report as cov  # noqa: E402

REPORT_VERSION = 2


def percent(part, whole):
    return 100.0 * part / whole if whole else 0.0


def measures(code, matched_code, functions, matched_functions,
             complete_code=0, units=0, complete_units=0):
    """A `Measures` message. proto3 JSON: u64 fields are strings."""
    return dict(
        fuzzy_match_percent=percent(matched_code, code),
        total_code=str(code),
        matched_code=str(matched_code),
        matched_code_percent=percent(matched_code, code),
        total_functions=functions,
        matched_functions=matched_functions,
        matched_functions_percent=percent(matched_functions, functions),
        complete_code=str(complete_code),
        complete_code_percent=percent(complete_code, code),
        total_units=units,
        complete_units=complete_units)


class Tally:
    """Running totals for the whole report or one category."""

    def __init__(self):
        self.code = self.matched_code = self.complete_code = 0
        self.functions = self.matched_functions = 0
        self.units = self.complete_units = 0

    def add(self, code, matched_code, functions, matched_functions, complete):
        self.code += code
        self.matched_code += matched_code
        self.functions += functions
        self.matched_functions += matched_functions
        self.units += 1
        if complete:
            self.complete_code += code
            self.complete_units += 1

    def measures(self):
        return measures(self.code, self.matched_code, self.functions,
                        self.matched_functions, self.complete_code,
                        self.units, self.complete_units)


def unit_name(obj, taken):
    """`<unit>/<source stem>`, falling back to the object id when ambiguous."""
    name = obj['id']
    if obj['source']:
        name = f'{obj["unit"]}/{Path(obj["source"]).stem}'
    if name in taken:
        name = obj['id']
    taken.add(name)
    return name


def build_report(root, build_dir):
    tu_edit = cov.import_tu_edit(root)
    objects, targets, iop_functions, _, _ = cov.tracked_scope(root)
    gates = cov.Gates(root, build_dir)
    failing = {unit for unit in cov.EE_UNITS
               if gates.resolve(unit)['state'] == cov.GATE_FAILING}

    total = Tally()
    tallies = {unit: Tally() for unit in cov.UNITS}
    units, taken, problems = [], set(), []

    for obj in objects:
        unit = obj['unit']
        counted = ({} if unit in failing
                   else cov.count_object(root, obj, tu_edit, problems))
        functions = []
        code = matched_code = matched = 0
        for name, va, size in obj['functions']:
            done = va in counted
            functions.append(dict(
                name=name, size=str(size),
                fuzzy_match_percent=100.0 if done else 0.0,
                metadata=dict(virtual_address=str(va))))
            code += size
            if done:
                matched_code += size
                matched += 1
        complete = bool(functions) and matched == len(functions)
        metadata = dict(complete=complete, module_name=targets.get(unit, unit),
                        progress_categories=[unit])
        if obj['source']:
            metadata['source_path'] = obj['source']
        units.append(dict(
            name=unit_name(obj, taken),
            measures=measures(code, matched_code, len(functions), matched,
                              code if complete else 0, 1, int(complete)),
            functions=functions,
            metadata=metadata))
        for tally in (total, tallies[unit]):
            tally.add(code, matched_code, len(functions), matched, complete)

    # The IOP modules: in scope, no build yet. One unit per debug-file group.
    iop = cov.load_required(root, cov.IOP_OBJECTS, 'the IOP module scope')
    for group in iop.get('groups', []):
        unit = group['unit']
        span = group.get('code') or {}
        code = (int(str(span['end']), 16) - int(str(span['start']), 16)
                if 'start' in span and 'end' in span else 0)
        count = int(group.get('function_count', 0))
        units.append(dict(
            name=group['id'],
            measures=measures(code, 0, count, 0, 0, 1, 0),
            metadata=dict(complete=False, module_name=targets.get(unit, unit),
                          progress_categories=[unit])))
        for tally in (total, tallies[unit]):
            tally.add(code, 0, count, 0, False)
    for unit in cov.IOP_UNITS:
        declared = iop_functions.get(unit, 0)
        if tallies[unit].functions != declared:
            problems.append(dict(unit=unit, reason=(
                f'{cov.IOP_OBJECTS} declares {declared} function(s), its groups '
                f'sum to {tallies[unit].functions}')))

    categories = [dict(id=unit, name=targets.get(unit, unit),
                       measures=tallies[unit].measures())
                  for unit in cov.UNITS]
    report = dict(measures=total.measures(), units=units,
                  version=REPORT_VERSION, categories=categories)
    return report, problems


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    parser.add_argument('-o', '--output', default='build/report.json',
                        help='where to write the report (default: build/report.json)')
    parser.add_argument('--stdout', action='store_true', help='print the report instead')
    parser.add_argument('--root', default=str(cov.ROOT), help=argparse.SUPPRESS)
    parser.add_argument('--build-dir', default=None, help=argparse.SUPPRESS)
    args = parser.parse_args(argv)
    root = Path(args.root)
    build_dir = Path(args.build_dir) if args.build_dir else root / cov.BUILD_DIR
    try:
        report, problems = build_report(root, build_dir)
    except cov.MissingInput as exc:
        print(f'objdiff_report: {exc}', file=sys.stderr)
        return 2
    for problem in problems:
        print(f'objdiff_report: {problem.get("unit", "")} '
              f'{problem.get("object", "")}: {problem["reason"]}', file=sys.stderr)
    text = json.dumps(report, indent=1) + '\n'
    if args.stdout:
        sys.stdout.write(text)
    else:
        out = Path(args.output)
        if not out.is_absolute():
            out = root / out
        out.parent.mkdir(parents=True, exist_ok=True)
        out.write_text(text, encoding='utf-8')
        m = report['measures']
        print(f'objdiff_report: {m["matched_functions"]:,} / {m["total_functions"]:,} '
              f'function(s) ({m["matched_functions_percent"]:.3f}%), '
              f'{int(m["matched_code"]):,} / {int(m["total_code"]):,} code bytes, '
              f'{len(report["units"])} unit(s) -> {out}')
    return 1 if problems else 0


if __name__ == '__main__':
    sys.exit(main())
