#!/usr/bin/env python3
"""Mark as complete only the objdiff units whose code and data both fully match.

objdiff counts every byte of a unit flagged `complete` as fully linked, whatever its
match. objdiff_project.py can only tell whether a unit still has INCLUDE_ASM functions,
so this second step reads a first report and clears the flag on units that fall short.

usage: objdiff_complete.py --project DIR --report REPORT.json
Then generate the report again: the second one is the one to publish.
"""
import argparse
import json
from pathlib import Path


def measure(unit, name):
    # objdiff omits measures whose value is zero.
    return int(unit['measures'].get(name, 0) or 0)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--project', required=True, type=Path)
    parser.add_argument('--report', required=True, type=Path)
    args = parser.parse_args()
    path = args.project / 'objdiff.json'
    project = json.loads(path.read_text())
    report = {unit['name']: unit for unit in json.loads(args.report.read_text())['units']}
    cleared, kept = 0, 0
    for unit in project['units']:
        metadata = unit.get('metadata', {})
        if not metadata.get('complete'):
            continue
        measured = report[unit['name']]
        if (measure(measured, 'matched_code') == measure(measured, 'total_code')
                and measure(measured, 'matched_data') == measure(measured, 'total_data')):
            kept += 1
        else:
            metadata['complete'] = False
            cleared += 1
    path.write_text(json.dumps(project, indent=2) + '\n')
    print(json.dumps(dict(complete=kept, cleared=cleared)))


if __name__ == '__main__':
    main()
