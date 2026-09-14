#!/usr/bin/env python3
"""Print the README progress table from the tracked object tables and the source.

The report answers one question: how much of the original game code does the
source in this repository actually reproduce? A function enters the numerator
only when three independent checks agree:

  1. the tracked object tables list it as an in-scope game function --
     `config/objects/main.objects.json` for SLUS_204.69 (each object's
     `functions` list), `config/objects/overlays.compile.json` plus
     `config/symbols/<ovl>.txt` for the five overlays (the symbols whose address
     falls in the object's text range), `config/objects/iop.json` for the two
     IOP modules, which have no build yet and therefore no numerator;
  2. the published source of that object really carries the block -- a C
     definition, or an `ACCEPTED_ASM(...)` line, read with the TU parser of
     `tools/tu_edit.py` and bound to the tracked function list, so a renamed
     local or a shared accepted `.s` covering several functions still resolves
     to the original names;
  3. the whole-file gate of the unit passes: `build/<unit>/status.json`, written
     by `ninja gate`. A gate that says "not identical" withdraws the unit's
     functions; a gate that was never run is reported as such, never as a pass.

Anything the three do not agree on is reported by name and never counted, and
`--check` exits 1 on any such disagreement as well as on a stale README.

WHAT A CLONE CAN VERIFY (tier A, always computed)

  The in-scope denominators, the numerator, the split into `exact_c`,
  `exact_c_with_asm` and `exact_asm`, and the gate state per unit. All of it
  comes from files a clone has: `config/objects/`, `config/symbols/`,
  `config/originals.json`, the source tree and `build/`.

WHAT A CLONE CANNOT VERIFY (tier B, maintainer tree only)

  `exact_vu_microcode`, how many counted functions have the 8-byte `jr ra; nop`
  original body, the accepted out-of-scope SDK/runtime source, and the
  cross-check against the per-function claims of the ledger. These need
  `config/tu/` (the per-TU ledger), `config/units/` (the acceptance records) and
  `config/vu-build.json` with the VU ledgers it names, which stay on the
  maintainer's machine. `exact_vu_microcode` is reported in the microprogram's
  own units -- programs, instruction pairs, bytes and entry points -- because a
  VU microprogram is not an EE function and has none to be counted as: reporting
  it as a function count printed a recovered 224-pair microprogram as `0`, which
  reads as "nothing recovered" rather than "not a function". Above all, a clone cannot confirm that
  each counted function matches its accepted record under its recorded contract
  and was independently reviewed: that evidence is not in the clone. When tier B
  is unavailable the report says so in one explicit stanza, and the note block it
  writes into README.md says so too -- an absent fact is never printed as a zero.

The four exactness categories are reported separately and only `exact_c` is
pure C (AGENTS.md).

`--legacy` restores the previous report: accepted unit records
(`config/units/*.json`) over the `config/tu-manifest.json` recovery scope. Both
of those are maintainer-only, so legacy mode exits 2 in a clone.

This is bookkeeping, not acceptance: a function is in the source because a
reviewer accepted it, and this tool only adds the bytes up. It never proves
module or full-game build/runtime verification (AGENTS.md).

Python 3 standard library only.

    python3 -B tools/coverage_report.py                # the README table
    python3 -B tools/coverage_report.py --json         # the same numbers as JSON
    python3 -B tools/coverage_report.py --check        # exit 1 if stale or inconsistent
    python3 -B tools/coverage_report.py --check --require-gate   # ... and demand a gate run
    python3 -B tools/coverage_report.py --write        # rewrite the README table
    python3 -B tools/coverage_report.py --legacy       # the previous unit-record report
"""
import argparse
import json
from pathlib import Path
import re
import sys

sys.dont_write_bytecode = True

ROOT = Path(__file__).resolve().parents[1]
UNITS = ('main', 'ov01', 'ov02', 'ov10', 'ov11', 'ov12', 'iop_ssd', 'iop_rssd')
EE_UNITS = ('main', 'ov01', 'ov02', 'ov10', 'ov11', 'ov12')
OVERLAY_UNITS = ('ov01', 'ov02', 'ov10', 'ov11', 'ov12')
IOP_UNITS = ('iop_ssd', 'iop_rssd')
VERSION = 'NTSC-U'

# Tracked inputs: a clone has all of these.
ORIGINALS = 'config/originals.json'
MAIN_OBJECTS = 'config/objects/main.objects.json'
OVERLAY_OBJECTS = 'config/objects/overlays.compile.json'
IOP_OBJECTS = 'config/objects/iop.json'
SYMBOLS_DIR = 'config/symbols'
BUILD_DIR = 'build'
README = 'README.md'

# Maintainer-only inputs: untracked, so tier B is computed only when present.
LEDGER_DIR = 'config/tu'
UNIT_RECORDS = 'config/units'
MANIFEST = 'config/tu-manifest.json'      # legacy mode only
VU_BUILD = 'config/vu-build.json'         # the VU programs and their ledgers

# The README table. Progress is expressed in FUNCTIONS (user direction,
# 2026-09-12): a recovered function is the unit of work, while text bytes weight
# the count by function size and read as more or less progress than there is.
# The byte figures stay in `--json` as diagnostics.
TABLE_HEADER = '| Version | Target | Functions | Progress |'
LEGACY_TABLE_HEADER = '| Version | Target | Bytes | Progress |'
NOTES_BEGIN = '<!-- coverage-report:begin -->'
NOTES_END = '<!-- coverage-report:end -->'

# The TU parser states that can be counted, and the categories each one admits.
STATE_CATEGORIES = {
    'c': ('exact_c', 'exact_c_with_asm', 'exact_vu_microcode'),
    'accepted_asm': ('exact_asm',),
}
CATEGORIES = ('exact_c', 'exact_c_with_asm', 'exact_asm', 'exact_vu_microcode')
# Tier A derives three of the four from the source itself; `exact_vu_microcode`
# is a claim about a separate VU microcode body and only an acceptance record
# asserts it, so a clone reports it as not verifiable rather than as zero.
TIER_A_CATEGORIES = ('exact_c', 'exact_c_with_asm', 'exact_asm')

# What the tracked object tables say a function's accepted language is, and the
# categories that language admits. A source classified against this is checked,
# not trusted: a contradiction is a problem, not a reclassification.
LANGUAGE_CATEGORIES = {
    'c': ('exact_c', 'exact_c_with_asm'),
    'c-asm': ('exact_c_with_asm',),
    'ee-asm': ('exact_asm',),
    'asm': ('exact_asm',),
    'vu': ('exact_vu_microcode',),
}

# `asm("...")`, `__asm__ volatile (...)` and the spellings in between: a C block
# holding one of these is `exact_c_with_asm`, not pure C (AGENTS.md).
INLINE_ASM = re.compile(r'\b(?:asm|__asm|__asm__)\s*(?:__volatile__|volatile)?\s*\(')

# `name = 0xVA; // type:func size:0xN` in config/symbols/<unit>.txt.
SYMBOL_LINE = re.compile(
    r'^\s*(\S+)\s*=\s*(0[xX][0-9A-Fa-f]+)\s*;\s*//\s*type:func\s+size:(0[xX][0-9A-Fa-f]+)')

# Gate states. "not run" is deliberately distinct from "failing": a build that
# was never run proves nothing either way and must not read as a failure.
GATE_PASSING, GATE_FAILING, GATE_NOT_RUN = 'passing', 'failing', 'not run'
# The two IOP modules have no build at all yet, which is a different statement
# from a gate that was not run: there is nothing here to run.
GATE_NO_BUILD = 'not built'


class MissingInput(Exception):
    """A required input is absent or unreadable; reported, never a traceback."""


def load_required(root, rel, what):
    """Parse a tracked JSON input, or raise MissingInput naming the file."""
    path = Path(root) / rel
    try:
        return json.loads(path.read_bytes())
    except FileNotFoundError:
        raise MissingInput(f'{rel} is missing ({what}); a clone of this '
                           f'repository tracks it, so this tree is incomplete')
    except (OSError, ValueError) as exc:
        raise MissingInput(f'{rel} is unreadable ({exc})')


def read_json(path):
    """Parse a JSON file; return (value, error) instead of raising."""
    try:
        return json.loads(Path(path).read_bytes()), None
    except FileNotFoundError:
        return None, 'missing'
    except (OSError, ValueError) as exc:
        return None, f'unreadable ({exc})'


def relative_to(root, path):
    """`path` spelled relative to the root when it lies inside it."""
    root, path = Path(root).resolve(), Path(path).resolve()
    try:
        return str(path.relative_to(root))
    except ValueError:
        return str(path)


# --------------------------------------------------------------------------
# legacy mode: the previous report, unchanged
# --------------------------------------------------------------------------
def require_maintainer_tree(root):
    """Legacy mode reads untracked inputs; say so plainly instead of failing."""
    for rel, what in ((MANIFEST, 'the legacy recovery scope'),
                      (UNIT_RECORDS, 'the accepted unit records')):
        if not (Path(root) / rel).exists():
            raise MissingInput(
                f'--legacy needs {rel} ({what}), which this repository no longer '
                'tracks: it exists only in a maintainer tree. Run the default '
                'report, which works from the tracked object tables.')


def denominators(root):
    """Recovery scope per unit: target file name and total ELF function bytes."""
    manifest = json.loads((Path(root) / MANIFEST).read_bytes())
    names = {unit: Path(record['path']).name
             for unit, record in manifest['units'].items()}
    total = dict.fromkeys(UNITS, 0)
    functions = dict.fromkeys(UNITS, 0)
    for entry in manifest['entries']:
        # `internal-entry` rows are CFG entries inside another function's extent:
        # they carry no size and are not independently counted functions.
        if entry.get('kind') != 'elf-function' or 'size' not in entry:
            continue
        unit = entry['unit']
        total[unit] = total.get(unit, 0) + entry['size']
        functions[unit] = functions.get(unit, 0) + 1
    return names, total, functions


def accepted(root):
    """Accepted bytes and function count per unit, de-duplicated by (unit, VA)."""
    extents = {}
    records = 0
    for path in sorted((Path(root) / UNIT_RECORDS).glob('*.json')):
        record = json.loads(path.read_bytes())
        records += 1
        unit = record['unit']
        for function in record['functions']:
            extents.setdefault((unit, function['va']), function['size'])
    total = dict.fromkeys(UNITS, 0)
    count = dict.fromkeys(UNITS, 0)
    for (unit, _), size in extents.items():
        total[unit] = total.get(unit, 0) + size
        count[unit] = count.get(unit, 0) + 1
    return total, count, records


def rows(root=ROOT):
    names, scope, scope_functions = denominators(root)
    done, done_functions, records = accepted(root)
    table = []
    for unit in UNITS:
        total = scope.get(unit, 0)
        bytes_done = done.get(unit, 0)
        table.append(dict(
            unit=unit, target=names.get(unit, unit), version=VERSION,
            bytes=bytes_done, bytes_total=total,
            functions=done_functions.get(unit, 0),
            functions_total=scope_functions.get(unit, 0),
            percent=(100.0 * bytes_done / total) if total else 0.0))
    return table, records


def render(table):
    """The README progress table: recovered functions over in-scope functions."""
    lines = [TABLE_HEADER, '| --- | --- | ---: | ---: |']
    for row in table:
        total = row['functions_total']
        percent = (100.0 * row['functions'] / total) if total else 0.0
        lines.append(f'| {row["version"]} | `{row["target"]}` | '
                     f'{row["functions"]:,} / {total:,} | '
                     f'{percent:.3f}% |')
    return '\n'.join(lines)


def render_legacy(table):
    """The byte table of the pre-TU report; `--legacy` must not change it."""
    lines = [LEGACY_TABLE_HEADER, '| --- | --- | ---: | ---: |']
    for row in table:
        lines.append(f'| {row["version"]} | `{row["target"]}` | '
                     f'{row["bytes"]:,} / {row["bytes_total"]:,} | '
                     f'{row["percent"]:.3f}% |')
    return '\n'.join(lines)


def readme_table(text):
    """The existing table block of README.md, or None.

    Either header is recognised so that `--check` reports a byte-based table as
    stale (and `--write` replaces it) instead of failing to find one at all.
    """
    return re.search(
        r'^\| Version \| Target \| (?:Functions|Bytes) \| Progress \|\n(?:\|.*\n?)+',
        text, re.M)


def legacy_main(args):
    """The report as it was before TU mode: `--legacy` must not change it."""
    require_maintainer_tree(args.root)
    table, records = rows(args.root)
    if args.json:
        print(json.dumps(dict(version=VERSION, unit_records=records, rows=table),
                         indent=2, sort_keys=True))
        return 0
    rendered = render_legacy(table)
    if not (args.check or args.write):
        print(rendered)
        totals_done = sum(row['bytes'] for row in table)
        totals_all = sum(row['bytes_total'] for row in table)
        print(f'\n{records} accepted unit record(s), '
              f'{sum(row["functions"] for row in table)} function(s), '
              f'{totals_done:,} / {totals_all:,} bytes '
              f'({100.0 * totals_done / totals_all:.3f}%) over the '
              f'{MANIFEST} recovery scope.')
        return 0
    path = Path(args.root) / README
    text = path.read_text(encoding='utf-8')
    match = readme_table(text)
    if match is None:
        print(f'coverage_report: no progress table found in {path}', file=sys.stderr)
        return 2
    current = match.group(0).rstrip('\n')
    # The README presents progress in functions (user direction, 2026-09-12).
    # Legacy mode still reports its byte numbers, but it never replaces a
    # function-based table with a byte-based one.
    if current.startswith(TABLE_HEADER):
        print('coverage_report: README.md carries the function-based table; '
              'legacy mode reports but does not rewrite it (use --legacy --json '
              'for the byte numbers)', file=sys.stderr)
        print(rendered)
        return 0 if args.write else 1
    if current == rendered:
        print('coverage_report: README.md progress table is up to date')
        return 0
    if args.check:
        print('coverage_report: README.md progress table is stale; run '
              '`python3 -B tools/coverage_report.py --legacy --write`', file=sys.stderr)
        print(rendered)
        return 1
    path.write_text(text[:match.start()] + rendered + '\n' + text[match.end():],
                    encoding='utf-8')
    print(f'coverage_report: rewrote the progress table in {path}')
    return 0


# --------------------------------------------------------------------------
# tier A: the in-scope objects, from the tracked object and symbol tables
# --------------------------------------------------------------------------
def import_tu_edit(root):
    """The TU parser of `tools/tu_edit.py`, imported as a module.

    The parser is never re-implemented here: block states (`asm`,
    `accepted_asm`, `c`) come from it. The tool's own directory is tried first
    so a published copy works without `--root`, the repository's `tools/` next.
    """
    for candidate in (Path(__file__).resolve().parent, Path(root) / 'tools'):
        if not (candidate / 'tu_edit.py').is_file():
            continue
        if str(candidate) not in sys.path:
            sys.path.insert(0, str(candidate))
        import tu_edit  # noqa: E402  (deliberately late, after the path fix)
        return tu_edit
    raise MissingInput('tools/tu_edit.py not found next to this tool or under '
                       f'{Path(root) / "tools"}')


def read_symbols(root, unit):
    """The `type:func` symbols of a unit as (name, va, size), lowest VA first."""
    path = Path(root) / SYMBOLS_DIR / f'{unit}.txt'
    try:
        text = path.read_text(encoding='utf-8')
    except FileNotFoundError:
        raise MissingInput(f'{SYMBOLS_DIR}/{unit}.txt is missing (the symbol '
                           f'table naming the functions of {unit})')
    except (OSError, ValueError) as exc:
        raise MissingInput(f'{SYMBOLS_DIR}/{unit}.txt is unreadable ({exc})')
    out = []
    for line in text.splitlines():
        match = SYMBOL_LINE.match(line)
        if match:
            out.append((match.group(1), int(match.group(2), 16), int(match.group(3), 16)))
    if not out:
        raise MissingInput(f'{SYMBOLS_DIR}/{unit}.txt lists no `type:func` symbol')
    out.sort(key=lambda item: item[1])
    return out


def main_stubs(tu):
    """Accepted language per function name, from an object's accepted records.

    These are stubs of the acceptance records, not the records themselves: they
    say which language a function was accepted in, so a source that classifies
    the function differently is a contradiction worth reporting. A record names
    its functions directly (`source_functions`) or by the VA of its text.
    """
    stubs = {}
    by_va = {int(str(f['va']), 16): f['name'] for f in tu.get('functions', [])}
    for record in (tu.get('accepted') or {}).get('records', []):
        language = record.get('language')
        if language not in LANGUAGE_CATEGORIES:
            continue
        named = list(record.get('source_functions') or [])
        if not named:
            va = (record.get('text') or {}).get('va')
            name = by_va.get(int(str(va), 16)) if va else None
            named = [name] if name else []
        for name in named:
            stubs.setdefault(name, language)
    return stubs


def overlay_stubs(tu):
    """Accepted language per function name for an overlay object.

    The overlay tables split the accepted functions into `c_functions` and
    `accepted_asm_functions`; `accepted_functions` alone says only that the
    function was accepted, which constrains nothing and is left out.
    """
    stubs = {}
    for name in tu.get('c_functions') or []:
        stubs.setdefault(name, 'c')
    for name in tu.get('accepted_asm_functions') or []:
        stubs.setdefault(name, 'ee-asm')
    return stubs


def tracked_scope(root):
    """Every in-scope original object, with its functions and its source path.

    Returns (objects, targets, iop_bytes, revision). An object is a dict:

        unit      the EE unit or IOP module it belongs to
        id        the object id of the tracked table
        source    the published source path, or None
        functions [(name, va, size)] in original order
        stubs     {name: accepted language} from the tracked table
    """
    originals = load_required(root, ORIGINALS, 'the six original EE files')
    targets = {unit: Path(record['file']).name
               for unit, record in originals.get('units', {}).items()}
    objects = []

    # main: the per-object manifest lists the original functions by name.
    main = load_required(root, MAIN_OBJECTS, 'the main per-object manifest')
    for tu in main.get('tus', []):
        if not tu.get('in_scope'):
            continue
        functions = [(f['name'], int(str(f['va']), 16), int(f['size']))
                     for f in tu.get('functions', [])]
        objects.append(dict(unit='main', id=tu['id'], source=tu.get('path'),
                            functions=functions, stubs=main_stubs(tu)))

    # overlays: the compile manifest gives each object's text range and how many
    # functions it holds; the names come from the unit's symbol table.
    overlays = load_required(root, OVERLAY_OBJECTS, 'the overlay compile manifest')
    for unit in OVERLAY_UNITS:
        unit_record = overlays.get('units', {}).get(unit)
        if unit_record is None:
            raise MissingInput(f'{OVERLAY_OBJECTS} has no unit {unit}')
        symbols = read_symbols(root, unit)
        for tu in unit_record.get('tus', []):
            if not tu.get('in_scope'):
                continue
            start, end = (int(str(x), 16) for x in tu['text'])
            functions = [(name, va, size) for name, va, size in symbols
                         if start <= va < end]
            objects.append(dict(unit=unit, id=tu['id'], source=tu.get('source'),
                                functions=functions, stubs=overlay_stubs(tu),
                                declared=tu.get('functions')))

    # IOP: in the recovery scope, but no build exists, so no numerator.
    iop = load_required(root, IOP_OBJECTS, 'the IOP module scope')
    iop_functions, iop_bytes = {}, {}
    for unit, record in iop.get('units', {}).items():
        targets[unit] = Path(record['file']).name
        iop_functions[unit] = int(record['functions'])
        iop_bytes[unit] = 0
    for group in iop.get('groups', []):
        code = group.get('code') or {}
        if 'start' in code and 'end' in code:
            unit = group['unit']
            iop_bytes[unit] = iop_bytes.get(unit, 0) + \
                (int(str(code['end']), 16) - int(str(code['start']), 16))
    return objects, targets, iop_functions, iop_bytes, originals.get('revision')


# --------------------------------------------------------------------------
# tier A: what the published source claims
# --------------------------------------------------------------------------
def classify(text, block):
    """The exactness category a source block stands for.

    `accepted_asm` is standalone EE assembler under a recorded contract; a C
    block holding inline assembly is `exact_c_with_asm`; anything else is the
    pure C that alone counts towards `exact_c` (AGENTS.md).
    """
    if block.state == 'accepted_asm':
        return 'exact_asm'
    return ('exact_c_with_asm'
            if INLINE_ASM.search(text[block.core_start:block.core_end])
            else 'exact_c')


def count_object(root, obj, tu_edit, problems):
    """What the published source of one object really carries.

    Returns {va: (name, category)} for the functions the source defines, and
    appends every disagreement between the source and the tracked tables to
    `problems`. An object with no published source yet is simply not counted;
    that is the normal state of an unrecovered object, not a problem.
    """
    counted = {}
    source = obj['source']
    if not source or Path(source).suffix != '.c':
        return counted
    path = Path(root) / source
    if not path.is_file():
        return counted
    where = dict(unit=obj['unit'], object=obj['id'], source=source)
    try:
        text = path.read_text(encoding='utf-8')
    except (OSError, ValueError) as exc:
        problems.append(dict(where, reason=f'source {source} is unreadable ({exc})'))
        return counted
    names = [name for name, _, _ in obj['functions']]
    address = {name: va for name, va, _ in obj['functions']}
    try:
        # Binding against the tracked function list is what resolves splat
        # renames and a shared accepted `.s` that covers several functions. A
        # source that will not bind is reported, never counted from a looser
        # parse: an unbound block cannot be tied to an original function.
        tu = tu_edit.parse(text, path=str(path), functions=names or None,
                           unit_dir=str(Path(root)))
    except tu_edit.ParseError as exc:
        problems.append(dict(where, reason=(
            f'source {source} does not parse against the {len(names)} function(s) '
            f'the tracked tables give {obj["id"]} ({exc})')))
        return counted
    for block in tu.blocks:
        if block.role != 'function' or block.state not in STATE_CATEGORIES:
            continue
        claimed = [block.key] + list(block.covers)
        if block.conditional:
            problems.append(dict(where, function=block.key, reason=(
                f'source {source} defines {block.key} inside a preprocessor '
                'conditional, which the audit refuses to claim')))
            continue
        category = classify(text, block)
        for name in claimed:
            va = address.get(name)
            if va is None:
                problems.append(dict(where, function=name, reason=(
                    f'source {source} claims {name}, which the tracked function '
                    f'list of {obj["id"]} does not contain')))
                continue
            language = obj['stubs'].get(name)
            if language and category not in LANGUAGE_CATEGORIES[language]:
                problems.append(dict(where, function=name, reason=(
                    f'source {source} carries {name} as {category}, while the '
                    f'tracked table records it as accepted {language!r}')))
                continue
            counted[va] = (name, category)
    return counted


# --------------------------------------------------------------------------
# tier A: the unit gate
# --------------------------------------------------------------------------
class Gates:
    """Whole-file gate state per unit, resolved once and cached.

    `build/<unit>/status.json`, written by `ninja gate`, is the tracked-tree
    answer and wins when it exists. In a maintainer tree the per-TU ledger may
    also name a gate in `last_verified.gate`, in either of two shapes:

    - a path to a gate report (schema `elf-gate/1`: `result` plus a per-target
      `identical`), which is read and judged;
    - a gate descriptor such as `whole_file+mapcheck+accepted_asm`, naming the
      checks the ledger recorded as passing rather than a file. The unit then
      passes only when no ledger record of that unit reports the whole-file gate
      as broken (a `whole_file` blocker).

    A ledger hint is a fallback, never a requirement: with neither a status file
    nor a hint the state is `not run`, which is reported as such. A `failing`
    gate withdraws the unit's functions from the count; a gate that was never run
    leaves the source-derived count standing but unverified, and contributes
    nothing to the gate-verified figure the report prints beside it.
    """

    def __init__(self, root, build_dir):
        self.root = Path(root)
        self.build_dir = Path(build_dir)
        self.hints = {}
        self.broken = {}
        self.state = {}

    def observe(self, unit, record):
        """Note what a ledger record says about its unit's whole-file gate."""
        if not unit or not isinstance(record, dict):
            return
        hint = (record.get('last_verified') or {}).get('gate')
        if hint:
            self.hints.setdefault(unit, []).append(str(hint))
        tu_id = record.get('id')
        for blocker in record.get('blockers') or []:
            if isinstance(blocker, dict) and blocker.get('kind') == 'whole_file':
                self.broken.setdefault(unit, []).append(
                    f'{tu_id}: {blocker.get("detail", "whole-file blocker")}')

    @staticmethod
    def _is_path(hint):
        """A gate report path, as opposed to a descriptor of the checks run."""
        return hint.endswith('.json') or '/' in hint or '\\' in hint

    def resolve(self, unit):
        if unit in self.state:
            return self.state[unit]
        status = self.build_dir / unit / 'status.json'
        hints = sorted(set(self.hints.get(unit, [])))
        broken = sorted(set(self.broken.get(unit, [])))
        if status.is_file():
            result = self._judge(status, unit, self._relative(status))
            result['from_status'] = True
        elif not hints:
            result = dict(unit=unit, state=GATE_NOT_RUN, source=None,
                          reason='the whole-file gate has not been run in this tree',
                          from_status=False)
        elif self._is_path(hints[0]):
            result = self._judge(self.root / hints[0], unit, hints[0])
            result['from_status'] = False
            if len(hints) > 1:
                result['other_gates'] = hints[1:]
        elif broken:
            result = dict(unit=unit, state=GATE_FAILING, source=hints[0],
                          from_status=False,
                          reason=f'the ledger reports the {unit} whole-file gate as '
                                 f'not identical ({"; ".join(broken)})')
        else:
            result = dict(unit=unit, state=GATE_PASSING, source=hints[0],
                          reason=None, from_status=False)
            if len(hints) > 1:
                result['other_gates'] = hints[1:]
        result['passing'] = result['state'] == GATE_PASSING
        self.state[unit] = result
        return result

    def _relative(self, path):
        return relative_to(self.root, path)

    def _judge(self, path, unit, label):
        """Read a gate report or build status and decide whether it passes."""
        report, error = read_json(path)
        if error == 'missing':
            # A named report that is not there says nothing about the gate: the
            # build was not run (or not kept), which is not a failure.
            return dict(unit=unit, state=GATE_NOT_RUN, source=label,
                        reason=f'gate report {label} is missing')
        if error is not None:
            return dict(unit=unit, state=GATE_FAILING, source=label,
                        reason=f'gate report {label} is {error}')
        # `elf-gate/1`: `result` is pass/fail and `targets.<name>.identical`
        # says which files were compared. A build status.json is accepted with
        # the same keys, or a plain `ok`/`identical` boolean.
        result = report.get('result')
        ok = report.get('ok')
        identical = report.get('identical')
        passing = (result == 'pass') or (ok is True) or (identical is True)
        if result == 'fail' or ok is False or identical is False:
            passing = False
        if passing and isinstance(report.get('targets'), dict):
            targets = report['targets']
            if unit not in targets:
                return dict(unit=unit, state=GATE_FAILING, source=label,
                            reason=f'gate report {label} does not cover {unit}')
            if targets[unit].get('identical') is False:
                return dict(unit=unit, state=GATE_FAILING, source=label,
                            reason=f'gate report {label} reports {unit} as not identical')
        if not passing:
            return dict(unit=unit, state=GATE_FAILING, source=label,
                        reason=f'gate report {label} does not report a passing whole-file gate')
        return dict(unit=unit, state=GATE_PASSING, source=label, reason=None)


# --------------------------------------------------------------------------
# tier B: the maintainer-only ledger and acceptance records
# --------------------------------------------------------------------------
def ledger_paths(ledger_dir):
    """Every `config/tu/<unit>/<tu>.json` record, in a stable order."""
    # `config/tu/<unit>/tu<NNN>.json` only: the directory also holds the
    # per-unit build directives (imports.json) and other generated inputs,
    # which are not ledger records and must not be read as one.
    return sorted(p for p in Path(ledger_dir).glob('*/tu[0-9][0-9][0-9].json'))


def read_ledger(root, ledger_dir, gates, problems):
    """The per-function claims of the ledger, keyed by (unit, VA).

    Every record also feeds the gate its `last_verified.gate` hint, so a
    maintainer tree without a build still resolves a gate.
    """
    claims = {}
    records = 0
    for path in ledger_paths(ledger_dir):
        record, error = read_json(path)
        rel = relative_to(root, path)
        if error is not None:
            problems.append(dict(record=rel, reason=f'ledger record is {error}'))
            continue
        records += 1
        unit = record.get('unit')
        gates.observe(unit, record)
        for function in record.get('functions', []):
            state = function.get('state')
            if state not in STATE_CATEGORIES:
                continue
            va = function.get('va')
            if va is None:
                problems.append(dict(record=rel, unit=unit,
                                     function=function.get('name'),
                                     reason='ledger claim carries no VA'))
                continue
            claims[(unit, int(str(va), 16))] = dict(
                name=function.get('name'), state=state,
                category=function.get('category'),
                empty_body=bool(function.get('empty_body')),
                records=list(function.get('records') or []),
                tu=record.get('id') or rel, record=rel)
    return claims, records


def cross_check(claims, counted, problems):
    """Assert the ledger's per-function claims equal the source-derived ones."""
    for key in sorted(set(claims) - set(counted)):
        unit, va = key
        claim = claims[key]
        problems.append(dict(unit=unit, tu=claim['tu'], function=claim['name'],
                             reason=(f'the ledger claims {claim["name"]} at 0x{va:08x} '
                                     f'as {claim["category"]}, but the counted source '
                                     'does not carry it')))
    for key in sorted(set(counted) - set(claims)):
        unit, va = key
        name, category = counted[key]
        problems.append(dict(unit=unit, function=name,
                             reason=(f'the source carries {name} at 0x{va:08x} as '
                                     f'{category}, but no ledger record claims it')))
    for key in sorted(set(counted) & set(claims)):
        unit, va = key
        name, category = counted[key]
        claim = claims[key]
        if claim['category'] != category:
            problems.append(dict(unit=unit, tu=claim['tu'], function=name,
                                 reason=(f'the source carries {name} as {category}, '
                                         f'the ledger claims {claim["category"]}')))


def out_of_scope_source(root, claims, counted):
    """Accepted out-of-scope source that is kept and verified, counted for the record.

    Out-of-scope claims (SDK, newlib/fdlibm/libgcc runtime, hand-written
    assembler objects) are outside the game recovery scope: their accepted
    source is kept and built and the whole-file gate proves it, but it is never
    part of the numerator or the denominator. A function is counted here only
    when the acceptance record the ledger cites exists and the source that
    record names is present, so the line reports source that is really there.
    """
    functions, tus = 0, set()
    for key, claim in claims.items():
        if key in counted or claim['state'] != 'c':
            continue
        for rel in claim['records']:
            path = Path(root) / rel
            if not path.is_file():
                continue
            record, error = read_json(path)
            if error is not None:
                continue
            source = (record.get('source') or {}).get('path')
            if source and (Path(root) / source).is_file():
                functions += 1
                tus.add(claim['tu'])
                break
    return dict(functions=functions, tus=len(tus))


# --------------------------------------------------------------------------
# the report
# --------------------------------------------------------------------------
def evaluate(root, build_dir, ledger_dir, records_dir, tu_edit):
    """Tier A always, tier B when the maintainer files are there.

    Returns the whole report as a dict: the rows, the categories, the gate state
    per unit, what tier B could and could not say, and every problem found.
    """
    objects, targets, iop_functions, iop_bytes, revision = tracked_scope(root)
    problems = []

    # (1) and (2): the tracked function lists, and what the source carries.
    func_total = dict.fromkeys(UNITS, 0)
    byte_total = dict.fromkeys(UNITS, 0)
    object_total = dict.fromkeys(UNITS, 0)
    counted = {}     # (unit, VA) -> (name, category), the functions the source has
    sizes = {}       # (unit, VA) -> original size, for the byte diagnostics
    for obj in objects:
        unit = obj['unit']
        func_total[unit] += len(obj['functions'])
        byte_total[unit] += sum(size for _, _, size in obj['functions'])
        object_total[unit] += 1
        declared = obj.get('declared')
        if declared is not None and int(declared) != len(obj['functions']):
            # The overlay tables count an object's functions; the names come
            # from the symbol table. The two must describe the same object.
            problems.append(dict(unit=unit, object=obj['id'], reason=(
                f'{OVERLAY_OBJECTS} gives {obj["id"]} {int(declared)} function(s), '
                f'but {len(obj["functions"])} `type:func` symbol(s) of '
                f'{SYMBOLS_DIR}/{unit}.txt fall in its text range')))
        for _, va, size in obj['functions']:
            sizes[(unit, va)] = size
        for va, found in count_object(root, obj, tu_edit, problems).items():
            counted[(unit, va)] = found
    for unit in IOP_UNITS:
        func_total[unit] = iop_functions.get(unit, 0)
        byte_total[unit] = iop_bytes.get(unit, 0)

    # tier B: the ledger and the acceptance records, when this tree has them.
    ledger_dir, records_dir = Path(ledger_dir), Path(records_dir)
    have_ledger = ledger_dir.is_dir()
    have_records = records_dir.is_dir()
    gates = Gates(root, build_dir)
    claims, ledger_records = ({}, 0)
    if have_ledger:
        claims, ledger_records = read_ledger(root, ledger_dir, gates, problems)

    # (3): the unit gate. A failing gate withdraws the unit's functions; a gate
    # that was never run leaves them counted but unverified, and says so.
    gate_state = {unit: gates.resolve(unit) for unit in EE_UNITS}
    withdrawn = {unit for unit, gate in gate_state.items()
                 if gate['state'] == GATE_FAILING}
    for unit in sorted(withdrawn):
        # One line per unit, not per function: the gate is a statement about the
        # whole file, and 300 identical lines would bury the reason.
        lost = sum(1 for key in counted if key[0] == unit)
        problems.append(dict(unit=unit, reason=(
            f'{lost} function(s) of {unit} are not counted: '
            f'{gate_state[unit]["reason"]}')))
    counted = {key: value for key, value in counted.items() if key[0] not in withdrawn}

    if have_ledger:
        cross_check(claims, counted, problems)

    # Aggregate.
    done_functions = dict.fromkeys(UNITS, 0)
    done_bytes = dict.fromkeys(UNITS, 0)
    verified_functions = dict.fromkeys(UNITS, 0)
    categories = {name: dict(functions=0, bytes=0) for name in TIER_A_CATEGORIES}
    for (unit, va), (name, category) in counted.items():
        size = sizes.get((unit, va), 0)
        done_functions[unit] += 1
        done_bytes[unit] += size
        if gate_state.get(unit, {}).get('state') == GATE_PASSING:
            verified_functions[unit] += 1
        bucket = categories.setdefault(category, dict(functions=0, bytes=0))
        bucket['functions'] += 1
        bucket['bytes'] += size

    # tier B facts, or an honest absence.
    tier_b = dict(ledger=have_ledger, unit_records=have_records,
                  ledger_records=ledger_records, missing=[])
    if not have_ledger:
        tier_b['missing'].append(LEDGER_DIR)
    if not have_records:
        tier_b['missing'].append(UNIT_RECORDS)
    if have_ledger:
        tier_b['empty_body'] = dict(
            functions=sum(1 for key in counted if claims.get(key, {}).get('empty_body')),
            names=sorted(claims[key]['name'] for key in counted
                         if claims.get(key, {}).get('empty_body')))
        vu = sum(1 for key in counted
                 if claims.get(key, {}).get('category') == 'exact_vu_microcode')
        categories['exact_vu_microcode'] = dict(
            functions=vu,
            bytes=sum(sizes.get(key, 0) for key in counted
                      if claims.get(key, {}).get('category') == 'exact_vu_microcode'))
    else:
        tier_b['empty_body'] = None
        categories['exact_vu_microcode'] = None
    # The EE count above answers "how many EE functions are classified `vu`",
    # which is not the same question as "is the VU microcode recovered".  The
    # microprogram has no EE function to be counted as, so it is read from its
    # own ledger and reported in its own units.
    tier_b['vu_microcode'] = vu_microcode_state(root)
    if tier_b['vu_microcode'] is None:
        tier_b['missing'].append(VU_BUILD)
    tier_b['out_of_scope'] = (out_of_scope_source(root, claims, counted)
                              if (have_ledger and have_records) else None)

    table = []
    for unit in UNITS:
        total = func_total.get(unit, 0)
        table.append(dict(
            unit=unit, target=targets.get(unit, unit), version=VERSION,
            functions=done_functions.get(unit, 0), functions_total=total,
            bytes=done_bytes.get(unit, 0), bytes_total=byte_total.get(unit, 0),
            objects=object_total.get(unit, 0),
            gate=gate_state.get(unit, {}).get('state', GATE_NO_BUILD),
            gate_verified=verified_functions.get(unit, 0),
            # `percent` is the headline the README shows: functions over
            # in-scope functions. `percent_bytes` stays as a diagnostic.
            percent=(100.0 * done_functions.get(unit, 0) / total) if total else 0.0,
            percent_bytes=(100.0 * done_bytes.get(unit, 0) / byte_total[unit])
            if byte_total.get(unit) else 0.0))

    problems.sort(key=lambda p: (p.get('unit') or '', p.get('object') or '',
                                 p.get('function') or '', p.get('reason') or ''))
    return dict(
        mode='tu',
        version=VERSION,
        revision=revision,
        rows=table,
        categories=categories,
        objects=sum(object_total.values()),
        gates=gate_state,
        tier_b=tier_b,
        disagreements=problems)


def vu_microcode_state(root):
    """What the VU ledgers say about the recovered microcode, or None.

    A VU microprogram is not an EE function and is not in the EE denominator: it
    is a separate unit kind with its own build, audit and ledger (docs/vu-
    microcode-unit.md).  Counting it as `exact_vu_microcode` *functions* reported
    a recovered 224-pair microprogram as zero, which read as "nothing recovered"
    rather than "not a function".  So it is reported in its own terms -- programs,
    instruction pairs, bytes and entry points -- and only when accepted.
    """
    build = root / VU_BUILD
    if not build.is_file():
        return None
    try:
        spec = json.loads(build.read_text())
    except (OSError, ValueError):
        return None
    programs, pairs, byte_total, entries = [], 0, 0, 0
    for program in spec.get('programs') or []:
        if not program.get('in_scope'):
            continue
        record = program.get('ledger') or program.get('record')
        if not record:
            continue
        path = root / record
        if not path.is_file():
            continue
        try:
            state = json.loads(path.read_text())
        except (OSError, ValueError):
            continue
        if state.get('state') != 'accepted':
            continue
        totals = state.get('totals') or {}
        programs.append(state.get('program') or program.get('id') or record)
        pairs += int(totals.get('pairs') or 0)
        byte_total += int(totals.get('bytes') or 0)
        entries += int(totals.get('entries') or 0)
    if not programs:
        return None
    return dict(programs=sorted(programs), pairs=pairs, bytes=byte_total,
                entries=entries)


def tier_b_note_lines(result):
    """The note lines that only a maintainer tree can state, or their absence."""
    categories = result['categories']
    tier_b = result['tier_b']
    lines = []
    vu = categories.get('exact_vu_microcode')
    micro = tier_b.get('vu_microcode')
    if vu is None and micro is None:
        lines.append(('exact_vu_microcode',
                      [f'- `exact_vu_microcode`: not verifiable here '
                       f'(needs {UNIT_RECORDS} and {VU_BUILD}, untracked)']))
    else:
        block = []
        if micro is None:
            block.append('- `exact_vu_microcode`: no accepted VU microprogram')
        else:
            names = ', '.join(micro['programs'])
            block.append(
                f'- `exact_vu_microcode`: {len(micro["programs"]):,} VU microprogram(s) '
                f'({names}): {micro["pairs"]:,} instruction pair(s), '
                f'{micro["bytes"]:,} bytes, {micro["entries"]:,} entry point(s)')
            block.append('  A VU microprogram is not an EE function: it is counted in '
                         'its own')
            block.append('  units and enters no EE function denominator.')
        if vu is not None and vu['functions']:
            block.append(f'  Plus {vu["functions"]:,} EE function(s) whose accepted '
                         'language is `vu`.')
        lines.append(('exact_vu_microcode', block))
    empty = tier_b['empty_body']
    if empty is None:
        lines.append(('empty_body',
                      [f'- empty-body functions: not verifiable here (the 8-byte '
                       f'`jr ra; nop` bodies are a fact about the original, '
                       f'recorded in {LEDGER_DIR}, untracked)']))
    else:
        lines.append(('empty_body',
                      [f'- empty-body functions: {empty["functions"]:,} of the counted '
                       'function(s) are 8-byte `jr ra; nop` bodies']))
    out = tier_b['out_of_scope']
    if out is None:
        pass  # covered by the closing stanza; no number to state either way
    elif out['functions'] or out['tus']:
        # Since 2026-09-12 there is none: the SDK and runtime recoveries are out of
        # recovery scope and their sources were archived (task tu-retire-legacy-src),
        # so their TUs are assembly again. The line reappears if an out-of-scope TU is
        # ever built from accepted C again.
        lines.append(('out_of_scope',
                      [f'- out-of-scope accepted source (kept and verified, outside '
                       f'numerator and denominator): {out["functions"]:,} function(s) '
                       f'in {out["tus"]:,} TU(s)']))
    if tier_b['missing']:
        lines.append(('tier_b', [
            '- These counts come from the tracked object tables, the tracked symbol',
            '  tables and the published source. This tree cannot check that each',
            '  counted function matches its accepted record under its recorded',
            f'  contract, independently reviewed: {" and ".join(tier_b["missing"])} '
            'are not tracked.']))
    return lines


def render_notes(result):
    """The plain-text lines README.md keeps above the table, between markers."""
    categories = result['categories']
    total_functions = sum(row['functions'] for row in result['rows'])
    scope_functions = sum(row['functions_total'] for row in result['rows'])
    percent = (100.0 * total_functions / scope_functions) if scope_functions else 0.0
    lines = [
        f'Recovered {total_functions:,} of {scope_functions:,} in-scope game '
        f'function(s) ({percent:.3f}%).',
        '',
        f'- `exact_c` (pure C): {categories["exact_c"]["functions"]:,} function(s)',
        f'- `exact_c_with_asm`: {categories["exact_c_with_asm"]["functions"]:,} function(s)',
        f'- `exact_asm`: {categories["exact_asm"]["functions"]:,} function(s)',
    ]
    for _, block in tier_b_note_lines(result):
        lines.extend(block)
    return '\n'.join(lines)


# The note lines a clone cannot state. When tier B is unavailable, `--check`
# neither demands nor rejects them: a tree that cannot compute a fact must not
# call a README carrying it stale.
TIER_B_PREFIXES = ('- `exact_vu_microcode`', '- empty-body functions:',
                   '- out-of-scope accepted source', '- These counts come from')


def tier_a_lines(block_text):
    """The note lines of a block that tier A alone can state and check."""
    kept, skipping = [], False
    for line in block_text.split('\n'):
        if line.startswith(TIER_B_PREFIXES):
            skipping = True
            continue
        if skipping and line.startswith('  '):
            continue            # continuation of a tier B line
        skipping = False
        kept.append(line)
    return kept


def readme_notes(text):
    """The existing marker block of README.md, or None."""
    return re.search(re.escape(NOTES_BEGIN) + r'\n.*?\n' + re.escape(NOTES_END),
                     text, re.S)


def notes_block(result):
    return f'{NOTES_BEGIN}\n{render_notes(result)}\n{NOTES_END}'


def notes_are_current(text, result):
    """True when the README marker block agrees with what this tree can check."""
    notes = readme_notes(text)
    if notes is None:
        return False
    wanted = notes_block(result)
    if notes.group(0) == wanted:
        return True
    if not result['tier_b']['missing']:
        return False
    # Tier B is unavailable: compare only the lines this tree derived itself.
    return tier_a_lines(notes.group(0)) == tier_a_lines(wanted)


def update_readme(text, result, rendered_table):
    """README text with both the marker block and the table brought up to date."""
    match = readme_table(text)
    if match is None:
        return None
    block = notes_block(result)
    notes = readme_notes(text)
    if notes is not None and notes.end() <= match.start():
        text = text[:notes.start()] + block + text[notes.end():]
        match = readme_table(text)
    elif notes is not None:
        # A stray block below the table: drop it and re-insert above.
        text = text[:notes.start()] + text[notes.end():].lstrip('\n')
        match = readme_table(text)
        text = text[:match.start()] + block + '\n\n' + text[match.start():]
        match = readme_table(text)
    else:
        text = text[:match.start()] + block + '\n\n' + text[match.start():]
        match = readme_table(text)
    return text[:match.start()] + rendered_table + '\n' + text[match.end():]


def readme_is_current(text, result, rendered_table):
    """True when README.md holds this table and notes this tree can confirm.

    `--check` asks this question: a tree that cannot compute a tier B fact has
    no standing to call a README carrying that fact stale.
    """
    match = readme_table(text)
    if match is None:
        return False
    if match.group(0).rstrip('\n') != rendered_table:
        return False
    return notes_are_current(text, result)


def readme_is_exact(text, result, rendered_table):
    """True when README.md already holds exactly this table and these notes.

    `--write` asks this question instead, so that writing in a tree without the
    maintainer files replaces a tier B number with the honest "not verifiable
    here" line rather than leaving a fact standing that nothing here checked.
    """
    match = readme_table(text)
    if match is None:
        return False
    if match.group(0).rstrip('\n') != rendered_table:
        return False
    notes = readme_notes(text)
    return notes is not None and notes.group(0) == notes_block(result)


def render_gates(result):
    """One line per EE unit: what the whole-file gate says, or that it never ran."""
    lines = []
    for unit in EE_UNITS:
        gate = result['gates'].get(unit)
        if gate is None:
            continue
        if gate['state'] == GATE_NOT_RUN:
            lines.append(f'  gate {unit}: not run (run ./configure.py && ninja gate)')
        elif gate['state'] == GATE_PASSING:
            lines.append(f'  gate {unit}: passing ({gate["source"]})')
        else:
            lines.append(f'  gate {unit}: NOT passing ({gate["reason"]})')
    return lines


def render_tier_b_stanza(result):
    """The single stanza that names what this tree could not compute."""
    tier_b = result['tier_b']
    if not tier_b['missing']:
        return []
    lines = ['coverage_report: maintainer-only facts not computed in this tree:']
    if not tier_b['ledger']:
        lines.append(f'  - {LEDGER_DIR} (the per-TU ledger) is absent: no empty-body '
                     'count and no cross-check of the per-function claims.')
    if not tier_b['unit_records']:
        lines.append(f'  - {UNIT_RECORDS} (the acceptance records) is absent: no '
                     'exact_vu_microcode count and no out-of-scope accepted-source line.')
    if tier_b.get('vu_microcode') is None:
        lines.append(f'  - {VU_BUILD} (the VU programs) is absent or names no accepted '
                     'in-scope program: the VU microcode line states no recovery.')
    lines.append('  Verified here: the in-scope denominators, which functions the '
                 'published')
    lines.append('  source carries and how they classify, and the unit gate where the '
                 'build')
    lines.append('  has been run. Not verified here: that each counted function matches '
                 'its')
    lines.append('  accepted record under its recorded contract, independently reviewed.')
    return lines


def print_report(result):
    """The terminal report: the table, the notes, the gates, then any problem."""
    print(render(result['rows']))
    print()
    print(render_notes(result))
    print(f'\n{result["objects"]} in-scope original object(s) over the tracked '
          f'{MAIN_OBJECTS}, {OVERLAY_OBJECTS} and {IOP_OBJECTS} scope.')
    if result['tier_b']['ledger']:
        print(f'  cross-checked against {result["tier_b"]["ledger_records"]:,} per-TU '
              f'ledger record(s) in {LEDGER_DIR}.')
    for line in render_gates(result):
        print(line)
    verified = sum(row['gate_verified'] for row in result['rows'])
    total = sum(row['functions'] for row in result['rows'])
    if verified != total:
        print(f'  {verified:,} of the {total:,} counted function(s) sit in a unit '
              'whose whole-file gate passed here; the rest are unverified.')
    for line in render_tier_b_stanza(result):
        print(line)
    report_problems(result)


def report_problems(result):
    """Print the disagreements; return True when there is any."""
    if not result['disagreements']:
        return False
    print(f'coverage_report: {len(result["disagreements"])} disagreement(s) between the '
          'tracked tables, the published source and the unit gate:', file=sys.stderr)
    for item in result['disagreements']:
        where = '/'.join(x for x in (item.get('object') or item.get('tu')
                                     or item.get('unit'),
                                     item.get('function')) if x) \
            or item.get('record', '?')
        print(f'  {where}: {item["reason"]}', file=sys.stderr)
    return True


def gate_failure(result, require_gate):
    """With --require-gate, every EE unit needs a passing build/<unit>/status.json."""
    if not require_gate:
        return []
    missing = []
    for unit in EE_UNITS:
        gate = result['gates'].get(unit) or {}
        if not (gate.get('from_status') and gate.get('state') == GATE_PASSING):
            missing.append(f'{unit}: {gate.get("state", GATE_NOT_RUN)}'
                           f'{"" if gate.get("from_status") else " (no build/%s/status.json)" % unit}')
    return missing


def tu_main(args):
    ledger_dir = Path(args.ledger_dir) if args.ledger_dir else Path(args.root) / LEDGER_DIR
    records_dir = Path(args.records_dir) if args.records_dir \
        else Path(args.root) / UNIT_RECORDS
    build_dir = Path(args.build_dir) if args.build_dir else Path(args.root) / BUILD_DIR
    tu_edit = import_tu_edit(args.root)
    result = evaluate(args.root, build_dir, ledger_dir, records_dir, tu_edit)
    rendered = render(result['rows'])
    ungated = gate_failure(result, args.require_gate)

    if args.json:
        print(json.dumps(result, indent=2, sort_keys=True))
        if not args.check:
            return 0
        # With --check the JSON form answers the same question as the text one:
        # the README must be current and the sources must agree.
        readme = Path(args.root) / README
        text = readme.read_text(encoding='utf-8') if readme.is_file() else ''
        return 0 if (readme_is_current(text, result, rendered)
                     and not result['disagreements'] and not ungated) else 1
    if not (args.check or args.write):
        print_report(result)
        return 0

    path = Path(args.root) / README
    try:
        text = path.read_text(encoding='utf-8')
    except (OSError, ValueError) as exc:
        raise MissingInput(f'{README} is unreadable ({exc})')
    if readme_table(text) is None:
        print(f'coverage_report: no progress table found in {path}', file=sys.stderr)
        return 2
    current = (readme_is_current(text, result, rendered) if args.check
               else readme_is_exact(text, result, rendered))
    failed = report_problems(result)
    for line in render_tier_b_stanza(result):
        print(line)
    if ungated:
        print('coverage_report: --require-gate: the whole-file gate has not passed '
              'for every unit in this tree:', file=sys.stderr)
        for line in ungated:
            print(f'  {line}', file=sys.stderr)
    if args.check:
        if not current:
            print('coverage_report: README.md progress table is stale; run '
                  '`python3 -B tools/coverage_report.py --write`', file=sys.stderr)
            print(rendered)
            print()
            print(render_notes(result))
        elif not (failed or ungated):
            print('coverage_report: README.md progress table is up to date'
                  + ('' if not result['tier_b']['missing']
                     else ' (tier A; the gate and the acceptance evidence are '
                          'reported above, not verified here)'))
        return 1 if (failed or ungated or not current) else 0
    if current:
        print('coverage_report: README.md progress table is up to date')
        return 1 if (failed or ungated) else 0
    updated = update_readme(text, result, rendered)
    if updated is None:
        print(f'coverage_report: no progress table found in {path}', file=sys.stderr)
        return 2
    path.write_text(updated, encoding='utf-8')
    print(f'coverage_report: rewrote the progress table in {path}')
    return 1 if (failed or ungated) else 0


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--root', type=Path, default=ROOT)
    parser.add_argument('--legacy', action='store_true',
                        help='the previous report: config/units records over '
                             'config/tu-manifest.json (maintainer tree only)')
    parser.add_argument('--ledger-dir', type=Path,
                        help=f'per-TU ledger directory (default <root>/{LEDGER_DIR}); '
                             'maintainer-only, absent in a clone')
    parser.add_argument('--records-dir', type=Path,
                        help=f'acceptance record directory (default <root>/{UNIT_RECORDS}); '
                             'maintainer-only, absent in a clone')
    parser.add_argument('--build-dir', type=Path,
                        help=f'build directory holding <unit>/status.json '
                             f'(default <root>/{BUILD_DIR})')
    parser.add_argument('--json', action='store_true', help='print the numbers as JSON')
    parser.add_argument('--check', action='store_true',
                        help='exit 1 when README.md is stale or the tracked tables, '
                             'the published source and the unit gate disagree')
    parser.add_argument('--require-gate', action='store_true',
                        help='with --check, also demand a passing '
                             'build/<unit>/status.json for all six EE units')
    parser.add_argument('--write', action='store_true',
                        help='rewrite the README.md table in place')
    args = parser.parse_args(argv)
    try:
        if args.legacy:
            return legacy_main(args)
        return tu_main(args)
    except MissingInput as exc:
        print(f'coverage_report: {exc}', file=sys.stderr)
        return 2


if __name__ == '__main__':
    sys.exit(main())
