#!/usr/bin/env python3
"""C-owned data runs carved out of a TU's scaffold data piece (jump tables, literals).

A recovered C function emits its own anonymous constants: the jump table of a
`switch`, a float/double literal, a string literal.  While the TU's data section
is scaffold-owned (config/tu-build.json `data_ownership[sec].owner == "asm"`) the
scaffold data piece provides those bytes and the unit link places nothing of the
C object's own `.rodata`/`.lit4`, so the C code's table relocation cannot land on
the original table.  A *carve* hands exactly the items the TU's C functions
generate over to the C object: the scaffold piece is cut at item boundaries, the
run between the cuts comes from the C object at its original address, and the
neighbours stay scaffold.  The byte gate is unchanged: the rebuilt files must be
whole-file identical and `tools/tu_audit.py` must pass (AGENTS.md).

This is the per-symbol split the accepted main import derived once
(`tools/tu/split_plan.py`, `config/split/main.data-splits.json`, the four
import-era runs of docs/tu-build.md "Data ownership"), made available to every
new recovery.

The declaration (`config/tu/data-carves.json`, schema `tu-data-carves/1`):

    {"schema": "tu-data-carves/1",
     "tus": {"main/tu170": {".rodata": [
         {"range": ["0x004c67d0", "0x004c6bc0"],          # padded run: item boundaries
          "generated": ["0x004c67d0", "0x004c6bc0"],      # what the C object emits
          "generated_by": ["MenuCharactor"],             # the C functions that emit it
          "c_owned_symbols": ["D_004C67D0", "D_004C67F0", "jtbl_004C6800"],
          "also_referenced_by_c": [],                    # items the C names, left scaffold
          "basis": {...}}]}}}                            # who proved it, with which gate

Several runs per TU and section (2026-09-26, user approval "arregla el carve").
Adjacent tables of several C functions form one run (their order in the C object
is the source order, which is the original order).  C-generated items separated
by scaffold items (the tables of functions that are still INCLUDE_ASM) form
several runs, one list entry each, in address order.  A C object has one input
section per section, so for a MAIN TU with more than one run in a section the
build links `build/c/<tu>.carved.o` instead of `build/c/<tu>.o`: `split` (below)
cuts that section of the compiled object into one input section per run
(`.rodata`, `.rodata.carve.1`, ...), each a byte-for-byte slice of the compiled
section, and the linker script pins each one at its run's original address.  No
section content changes: relocations against the split section are re-pointed
at a local base symbol of the piece they reach (value = minus the piece's
offset in the compiled section, so symbol + in-place addend is unchanged), and
named symbols move with their piece.  The compiled object stays the one
`tools/tu_audit.py`, the worker diagnostics and `derive` read.  A single run
builds exactly as before (no split object).  Overlays still take one run per
section (tools/tu/ninja_ovl.py links splat's object line).

Which compiled offset starts each run (`plan_split`): the C layout is the
original layout minus the scaffold items, so run k starts after run k-1's last
item, at an offset the C object references (an anchor: a relocation or a
symbol into the section) whose bytes are those of run k's first item in the
original (relocated words are compared by the gate, not here).  Alignment fill
the C layout carries between two runs, where the original had a scaffold item
instead, is left out of the pieces only when it is zero and holds no
relocation or symbol.

Who reads it:
  * `tools/tu/ninja_main.py` (MAIN) and `tools/tu/ninja_ovl.py` (overlays) apply
    the declared runs to the configured unit directory at generation time: the
    carved piece files are written under `<unit_dir>/carve/`, the published
    splat output is never edited, and a unit without declared runs generates
    exactly the build it generated before;
  * `tools/tu_audit.py` treats a declared run as a C-owned range of the TU;
  * `tools/tu_build_manifest.py` carries the published runs into
    config/tu-build.json `data_ownership` (`partial`, `c_runs`) and so into the
    per-TU ledger.
Who writes it:
  * `tools/worker.py form` derives the runs from the compiled candidate
    (`derive`, below) and records them in the attempt's PRIVATE copy;
  * `tools/review_stage.py` carries that entry into the review's private tree
    and re-derives it from its own build (`check`);
  * `tools/tu_publish.py` publishes the reviewed entry with its basis.

Derivation (no guessing; every input is a file of the build):
  * items of a piece = the `nonmatching` blocks of its generated data file, each
    running to the next block (trailing padding stays with its item);
  * the TU's C functions are its functions without an INCLUDE_ASM/ACCEPTED_ASM
    line; an item is C-generated when the original assembly of a C function
    references one of its labels and the C object neither references nor
    defines it by name (a name the C object references stays scaffold:
    `also_referenced_by_c`);
  * an item that only another C-generated item of the section references (the
    strings a function-local `char *tbl[] = {...}` template points at) is
    C-generated too (`reached_through` names the referrer); it must not also be
    referenced by a scaffold function or a scaffold item;
  * consecutive generated items form one run, no scaffold (INCLUDE_ASM or
    ACCEPTED_ASM) function may reference them, and each run's slice of the C
    object's section must fit the run: longer than the run minus its last item
    and not longer than the run;
  * the C object's bytes, with its relocations resolved against the TU's
    original addresses, are compared with the original bytes of the runs as a
    diagnostic (`bytes`); the whole-file gate stays the verdict.

    data_carve.py derive --root R --unit U --unit-dir D --tu ID --source SRC --obj OBJ [--json-out F]
    data_carve.py check  --root R --unit U --unit-dir D --tu ID --source SRC --obj OBJ
    data_carve.py show   --root R [--tu ID]
    data_carve.py split  --root R --unit main --unit-dir D --tu ID --obj OBJ --out OUT
"""
import argparse
import copy
import json
import os
import re
import struct
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
from elfinfo import Elf  # noqa: E402

REGISTRY = 'config/tu/data-carves.json'
SCHEMA = 'tu-data-carves/1'
# Sections a compiler fills with anonymous constants of a function: jump tables
# and string/float/double literals (.rodata), gp-relative float literals (.lit4)
# and double literals (.lit8), and under -G8 the small (<= 8 byte) string
# literals and initialised-template copies GCC puts in .sdata (MAIN only: the
# overlay links carve .rodata alone).
SECTIONS = ('.rodata', '.lit4', '.lit8', '.sdata')
MAIN_ONLY_SECTIONS = ('.sdata',)
CARVE_DIR = 'carve'
# Input section of run k > 0 in a split object, and the object a split TU links.
SPLIT_SUFFIX = '.carve.'
SPLIT_OBJECT = '.carved.o'
LD_MARK = '/* data-carve */'
LD_ORIG = '/* data-carve-orig: '

NONMATCHING = re.compile(r'^nonmatching\s+(\S+?)(?:,.*)?\s*$')
ALIGN = re.compile(r'^\.align\s+\d+\s*$')
LABEL = re.compile(r'^\s*(?:dlabel|glabel|jlabel|alabel|ehlabel)\s+(\S+?)(?:,.*)?\s*$')
ADDR = re.compile(r'^\s*/\* ([0-9A-F]+) ([0-9A-F]{8}) ')
INC = re.compile(r'^\s*(INCLUDE_ASM|ACCEPTED_ASM)\(\s*"([^"]+)"\s*,\s*([^)\s]+)\s*\)\s*;')
IDENT = re.compile(r'[A-Za-z_.$][\w.$]*')
PROVIDE = re.compile(r'PROVIDE\(\s*"?([^"\s=]+)"?\s*=\s*"?([^"\s+)]+)"?\s*\+\s*(0x[0-9A-Fa-f]+|\d+)\s*\)')


class Misplaced(Exception):
    """`plan_split(..., check_items=True)`: item `start` of run `k` does not lie in the C
    object where its run's offset puts it (the C layout aligns it further): `derive`
    starts a new run at that item."""
    def __init__(self, k, start):
        super().__init__(f'run {k}: the item at {h8(start)} is not where the run places it')
        self.k, self.start = k, start


class CarveError(Exception):
    pass


def h8(value):
    return f'0x{value:08x}'


def hx(value):
    return int(value, 16) if isinstance(value, str) else int(value)


# ---------------------------------------------------------------------------
# the declaration
# ---------------------------------------------------------------------------

def registry_path(root):
    return Path(root) / REGISTRY


def load_registry(root):
    """{'schema', 'tus': {tu_id: {section: [run]}}}; an absent file declares nothing."""
    path = registry_path(root)
    if not path.is_file():
        return dict(schema=SCHEMA, tus={})
    data = json.loads(path.read_bytes())
    if data.get('schema') != SCHEMA:
        raise CarveError(f'{path}: schema {data.get("schema")!r} is not {SCHEMA}')
    data.setdefault('tus', {})
    return data


def unit_runs(registry, unit):
    """{tu_id: {section: [run]}} of one unit."""
    return {tu: secs for tu, secs in sorted(registry.get('tus', {}).items())
            if tu.split('/')[0] == unit and secs}


def normalize_runs(runs):
    """Only the facts the build and the gate depend on, for comparisons."""
    out = {}
    for sec, items in sorted((runs or {}).items()):
        if not items:
            continue
        out[sec] = [dict(range=[h8(hx(r['range'][0])), h8(hx(r['range'][1]))],
                         c_owned_symbols=sorted(r.get('c_owned_symbols') or []))
                    for r in items]
    return out


def write_registry(root, tu_id, runs, basis=None):
    """Replace (or, with empty `runs`, remove) one TU's entry; returns True when it changed.

    Temp file + rename, and a read-only private copy is made writable first (a
    worker's staged configuration is 0444)."""
    path = registry_path(root)
    data = load_registry(root)
    old = data['tus'].get(tu_id)
    new = None
    if runs:
        new = {}
        for sec, items in sorted(runs.items()):
            new[sec] = []
            for r in items:
                entry = {k: r[k] for k in ('range', 'generated', 'generated_by', 'c_owned_symbols',
                                            'also_referenced_by_c', 'reached_through') if k in r}
                if basis is not None:
                    entry['basis'] = basis
                elif r.get('basis') is not None:
                    entry['basis'] = r['basis']
                new[sec].append(entry)
    if old == new:
        return False
    if new is None:
        data['tus'].pop(tu_id, None)
    else:
        data['tus'][tu_id] = new
    data['tus'] = {k: data['tus'][k] for k in sorted(data['tus'])}
    doc = dict(schema=SCHEMA,
               doc=('C-owned data runs carved out of scaffold data pieces: the jump tables and '
                    'literals a recovered C function generates (tools/tu/data_carve.py, '
                    'docs/tu-build.md "Data ownership"). Written by tools/worker.py (private), '
                    'tools/review_stage.py (private) and tools/tu_publish.py (published).'),
               tus=data['tus'])
    path.parent.mkdir(parents=True, exist_ok=True)
    if path.exists() and not os.access(path, os.W_OK):
        path.chmod(0o644)
    tmp = path.with_name(path.name + '.tmp')
    tmp.write_text(json.dumps(doc, indent=1) + '\n')
    os.replace(tmp, path)
    return True


# ---------------------------------------------------------------------------
# scaffold data files
# ---------------------------------------------------------------------------

def parse_data_file(path):
    """(header lines, [block]) of a generated data file.

    A block is one `nonmatching` item (with the `.align` line splat puts before it)
    and everything up to the next one; `start` is its first address, `labels`
    every data label inside it."""
    lines = Path(path).read_text().splitlines()
    starts = []
    for i, line in enumerate(lines):
        if NONMATCHING.match(line):
            j = i - 1 if i and ALIGN.match(lines[i - 1]) else i
            starts.append(j)
    if not starts:
        return lines, []
    header = lines[:starts[0]]
    blocks = []
    for k, s in enumerate(starts):
        e = starts[k + 1] if k + 1 < len(starts) else len(lines)
        body = lines[s:e]
        start, labels = None, []
        for line in body:
            m = LABEL.match(line)
            if m:
                labels.append(m.group(1))
                continue
            a = ADDR.match(line)
            if a and start is None:
                start = (int(a.group(2), 16), int(a.group(1), 16))
        if start is None:
            raise CarveError(f'{path}: item {labels[:1] or body[:2]} has no address line')
        blocks.append(dict(lines=body, start=start[0], offset=start[1], labels=labels))
    return header, blocks


def piece_items(path, start, end):
    """[item] of one piece: name, labels, start, end (the next item or the piece end)."""
    header, blocks = parse_data_file(path)
    if not blocks:
        raise CarveError(f'{path}: no data item')
    if blocks[0]['start'] != start:
        raise CarveError(f'{path}: first item at {h8(blocks[0]["start"])}, piece starts at {h8(start)}')
    items = []
    for k, b in enumerate(blocks):
        e = blocks[k + 1]['start'] if k + 1 < len(blocks) else end
        items.append(dict(name=b['labels'][0] if b['labels'] else f'D_{b["start"]:08X}',
                          labels=b['labels'], start=b['start'], end=e, block=b))
    return header, items


def write_piece_file(path, header, items):
    lines = header + [line for it in items for line in it['block']['lines']]
    while lines and not lines[-1].strip():
        lines.pop()                    # as splat ends a data file: no trailing blank line
    text = '\n'.join(lines) + '\n'
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    if not path.is_file() or path.read_text() != text:
        path.write_text(text)


# ---------------------------------------------------------------------------
# re-piecing a section of one TU
# ---------------------------------------------------------------------------

def repiece(tu_name, pieces, runs, read_items):
    """New pieces of one TU section for its C runs.

    `pieces`: [dict(name, start, end, c_split, file)] in address order (the
    section's current split, import-era c_split pieces included); `runs`:
    [(lo, hi)] in address order (one run: a (lo, hi) tuple is accepted too);
    `read_items(piece)` -> (header, items).  Every cut lies on an item boundary;
    the pieces inside one run merge into one C-owned piece; an import-era
    c_split piece outside every run is refused (the declaration must cover it).
    Returns [dict(name, start, end, c_split, run, header, items, carved)]; `run`
    is the index of the run a C-owned piece holds (None for scaffold)."""
    if runs and isinstance(runs[0], int):
        runs = [tuple(runs)]
    runs = [tuple(r) for r in runs]
    first, last = pieces[0]['start'], pieces[-1]['end']
    for lo, hi in runs:
        if not (first <= lo < hi <= last):
            raise CarveError(f'{tu_name}: run {h8(lo)}-{h8(hi)} is outside the TU piece {h8(first)}-{h8(last)}')

    def run_of(a, b):
        return next((k for k, (lo, hi) in enumerate(runs) if lo <= a and b <= hi), None)

    segments = []                                  # (start, end, header, items, source piece, carved)
    for p in pieces:
        header, items = read_items(p)
        if p['c_split'] and run_of(p['start'], p['end']) is None:
            raise CarveError(f'{tu_name}: the import-era C run {h8(p["start"])}-{h8(p["end"])} is not inside '
                             f'a declared run ({", ".join(f"{h8(lo)}-{h8(hi)}" for lo, hi in runs)})')
        cuts = sorted({b for r in runs for b in r if p['start'] < b < p['end']})
        bounds = [p['start']] + cuts + [p['end']]
        starts = {it['start'] for it in items}
        for b in cuts:
            if b not in starts:
                raise CarveError(f'{tu_name}: {h8(b)} is not an item boundary of {p["name"]}')
        for a, b in zip(bounds, bounds[1:]):
            sub = [it for it in items if a <= it['start'] < b]
            segments.append(dict(start=a, end=b, header=header, items=sub, piece=p,
                                 carved=bool(cuts)))
    out = []
    for seg in segments:
        k = run_of(seg['start'], seg['end'])
        inside = k is not None
        if inside and out and out[-1]['c_split'] and out[-1]['run'] == k:
            prev = out[-1]
            prev.update(end=seg['end'], items=prev['items'] + seg['items'], carved=True)
            continue
        p = seg['piece']
        # An untouched piece keeps its splat name and file; a piece whose extent or
        # ownership changes is written to <unit_dir>/carve/ under `carve/<name>`, so
        # its object never shares a name with the pristine splat piece.
        untouched = seg['start'] == p['start'] and not seg['carved'] and inside == bool(p['c_split'])
        base = p['name'] if seg['start'] == p['start'] else f'{tu_name}__{seg["start"]:08X}'
        out.append(dict(name=base, start=seg['start'], end=seg['end'], c_split=inside, run=k,
                        header=seg['header'], items=seg['items'], carved=not untouched, source=p))
    for p in out:
        if p['carved']:
            p['name'] = f'{CARVE_DIR}/{p["name"].split("/")[-1]}'
    got = [p['run'] for p in out if p['c_split']]
    if got != list(range(len(runs))):
        raise CarveError(f'{tu_name}: the runs {", ".join(f"{h8(lo)}-{h8(hi)}" for lo, hi in runs)} do not '
                         f'form one piece each')
    return out


def declared_runs(tu_id, sec, runs, several=True):
    """[(lo, hi)] of one TU section's declaration, in address order and disjoint.

    `several=False` (overlays): exactly one run, as the overlay link places one
    input section per C object and section."""
    if not runs:
        raise CarveError(f'{tu_id} {sec}: an empty run list is declared')
    if len(runs) != 1 and not several:
        raise CarveError(
            f'{tu_id} {sec}: {len(runs)} C runs declared; an overlay TU takes one C run per section (the '
            f'several-run split object is built for MAIN only): recover the functions whose items lie in '
            f'between, or keep this section scaffold-owned')
    out = [(hx(r['range'][0]), hx(r['range'][1])) for r in runs]
    for (lo, hi), nxt in zip(out, out[1:] + [None]):
        if not lo < hi or (nxt is not None and hi > nxt[0]):
            raise CarveError(f'{tu_id} {sec}: the declared runs are not disjoint and in address order: '
                             f'{[(h8(a), h8(b)) for a, b in out]}')
    return out


def check_one_run(tu_id, sec, runs):
    """(lo, hi) of a section with exactly one declared run (the overlay rule)."""
    return declared_runs(tu_id, sec, runs, several=False)[0]


def split_section_name(sec, k):
    """Input section of run k of a split section: `.rodata`, `.rodata.carve.1`, ..."""
    return sec if k == 0 else f'{sec}{SPLIT_SUFFIX}{k}'


def base_section_name(name):
    """The section a split input section belongs to (`.rodata.carve.2` -> `.rodata`)."""
    return name.split(SPLIT_SUFFIX, 1)[0]


def split_sections(runs_by_section):
    """{section: run count} of the sections a TU declares with more than one run."""
    return {sec: len(runs) for sec, runs in sorted((runs_by_section or {}).items()) if len(runs or []) > 1}


# ---------------------------------------------------------------------------
# MAIN: the TU manifest (tools/tu/ninja_main.py, tools/tu/mapcheck.py)
# ---------------------------------------------------------------------------

def main_piece_file(unit_dir, piece, sec):
    return Path(unit_dir) / (piece.get('file') or f'asm/main/data/{piece["name"]}.{sec[1:]}.s')


def apply_main(root, unit_dir, manifest, registry=None, write=True):
    """(effective manifest, report) with every declared MAIN run carved.

    The manifest is not modified in place and never written back: the tracked
    `config/objects/main.objects.json` stays what splat was configured with.
    With nothing declared for MAIN the same manifest object is returned."""
    registry = load_registry(root) if registry is None else registry
    declared = unit_runs(registry, 'main')
    if not declared:
        return manifest, []
    m = copy.deepcopy(manifest)
    by_id = {t['id']: t for t in m['tus']}
    report = []
    for tu_id, secs in declared.items():
        t = by_id.get(tu_id)
        if t is None:
            raise CarveError(f'{REGISTRY}: {tu_id} is not a TU of the MAIN manifest')
        split = split_sections(secs)
        if split:
            # several runs in a section: the TU links its split object (`split`)
            t['c_link_object'] = f'build/c/{t["name"]}{SPLIT_OBJECT}'
            t['c_split_sections'] = split
        for sec, runs in sorted(secs.items()):
            spans = declared_runs(tu_id, sec, runs)
            if sec not in t['sections']:
                raise CarveError(f'{tu_id} has no {sec} piece')
            ts = t['sections'][sec]
            pieces = [dict(name=p['name'], start=hx(p['start']), end=hx(p['end']), c_split=p.get('c_split', False),
                           file=p.get('file'))
                      for p in ts.get('pieces') or [dict(name=t['name'], start=ts['start'], end=ts['end'])]]

            def read(p, sec=sec):
                return piece_items(main_piece_file(unit_dir, p, sec), p['start'], p['end'])

            new = repiece(t['name'], pieces, spans, read)
            out_pieces = []
            for p in new:
                entry = dict(c_split=p['c_split'], end=f'0x{p["end"]:08X}', name=p['name'],
                             start=f'0x{p["start"]:08X}')
                if p['c_split'] and sec in split:
                    entry['c_section'] = split_section_name(sec, p['run'])
                if p['carved']:
                    rel = f'{CARVE_DIR}/main/{p["name"].split("/")[-1]}.{sec[1:]}.s'
                    entry['file'] = rel
                    if write:
                        write_piece_file(Path(unit_dir) / rel, p['header'], p['items'])
                elif p['source'].get('file'):
                    entry['file'] = p['source']['file']
                out_pieces.append(entry)
            ts['pieces'] = out_pieces
            t['owners'][sec] = 'split'
            order = m['sections'][sec]['order']
            idx = [i for i, o in enumerate(order) if o['tu'] == t['name']]
            if not idx or idx != list(range(idx[0], idx[-1] + 1)):
                raise CarveError(f'{tu_id} {sec}: the section order does not hold the TU\'s pieces contiguously')
            repl = [dict(c_split=p['c_split'], end=p['end'], ordinal=t['ordinal'], piece=p['name'],
                         start=p['start'], tu=t['name'], **({'file': p['file']} if p.get('file') else {}),
                         **({'c_section': p['c_section']} if p.get('c_section') else {}))
                    for p in out_pieces]
            order[idx[0]:idx[-1] + 1] = repl
            report.append(dict(tu=tu_id, section=sec, run=[h8(spans[0][0]), h8(spans[-1][1])],
                               runs=[[h8(lo), h8(hi)] for lo, hi in spans],
                               pieces=[(p['name'], p['start'], p['end'], p['c_split']) for p in out_pieces]))
    return m, report


# ---------------------------------------------------------------------------
# overlays: the splat linker script (tools/tu/ninja_ovl.py, tools/tu/link_elf.py)
# ---------------------------------------------------------------------------

def ovl_restore(ld_text):
    """The splat linker script without any earlier carve (idempotent regeneration)."""
    out = []
    for line in ld_text.splitlines(keepends=True):
        if LD_MARK in line:
            continue
        if LD_ORIG in line:
            indent = line[:len(line) - len(line.lstrip())]
            body = line.split(LD_ORIG, 1)[1].rsplit('*/', 1)[0].strip()
            obj, sec = re.match(r'(\S+)\[(\.\w+)\]', body).groups()
            out.append(f'{indent}{obj}({sec});\n')
            continue
        out.append(line)
    return ''.join(out)


def ovl_tu_names(root, unit):
    """{tu id: TU name} of an overlay (config/tu-build.json path stems)."""
    build = json.loads((Path(root) / 'config/tu-build.json').read_bytes())
    return {t['id']: Path(t['path']).stem for t in build['tus'] if t['unit'] == unit}


def ovl_piece(unit_dir, unit, name, sec='.rodata'):
    layout = json.loads((Path(unit_dir) / 'layout.json').read_bytes())
    fam = {x[0]: (hx(x[1]), hx(x[2])) for x in layout['families'].get(sec, [])}
    key = f'{unit}/{name}'
    if key not in fam:
        return None
    start, end = fam[key]
    return dict(name=f'{unit}/{name}', start=start, end=end, c_split=False,
                file=f'asm/data/{unit}/{name}{sec}.s')


def apply_overlay(root, unit_dir, unit, ld_text, registry=None, write=True):
    """(linker script, report) with every declared run of this overlay carved.

    The TU's splat data line is kept as a comment and replaced by: the scaffold
    piece before the run, the C object's section, a pin to the run end, the
    scaffold piece after it.  Idempotent: an earlier carve is undone first, so a
    script without declared runs is exactly splat's."""
    text = ovl_restore(ld_text)
    registry = load_registry(root) if registry is None else registry
    declared = unit_runs(registry, unit)
    if not declared:
        return text, []
    names = ovl_tu_names(root, unit)
    base = re.search(r'^\s*\.' + re.escape(unit) + r' 0x([0-9A-Fa-f]+)\s*:', text, re.M)
    if not base:
        raise CarveError(f'{unit}.ld: no .{unit} output section')
    base = int(base.group(1), 16)
    report = []
    for tu_id, secs in declared.items():
        name = names.get(tu_id)
        if name is None:
            raise CarveError(f'{REGISTRY}: {tu_id} is not a TU of {unit}')
        for sec, runs in sorted(secs.items()):
            if sec != '.rodata':
                raise CarveError(f'{tu_id}: an overlay carve covers .rodata only (declared {sec})')
            lo, hi = check_one_run(tu_id, sec, runs)
            piece = ovl_piece(unit_dir, unit, name, sec)
            if piece is None:
                raise CarveError(f'{tu_id} has no {sec} piece in {unit}/layout.json')
            text_line = re.search(r'^(\s*)(build/(?:scaffold/)?src/' + re.escape(unit) + '/'
                                  + re.escape(name) + r'\.o)\(\.text\);\s*$', text, re.M)
            if not text_line:
                raise CarveError(f'{tu_id}: {unit}.ld links no C object for {name} (is the TU C?)')
            c_obj = text_line.group(2)
            if re.search(re.escape(c_obj) + r'\(' + re.escape(sec) + r'\)', text):
                raise CarveError(f'{tu_id}: {unit}.ld already places {c_obj}({sec})')
            data_obj = f'build/asm/data/{unit}/{name}{sec}.o'
            line = re.search(r'^(\s*)' + re.escape(data_obj) + r'\(' + re.escape(sec) + r'\);[^\n]*\n', text, re.M)
            if not line:
                raise CarveError(f'{tu_id}: {unit}.ld does not place {data_obj}({sec})')

            def read(p):
                return piece_items(Path(unit_dir) / p['file'], p['start'], p['end'])

            new = repiece(name, [piece], (lo, hi), read)
            indent = line.group(1)
            out = [f'{indent}{LD_ORIG}{data_obj}[{sec}] */\n']
            for p in new:
                if p['c_split']:
                    out.append(f'{indent}{c_obj}({sec}); {LD_MARK}\n')
                    out.append(f'{indent}. = 0x{p["end"] - base:X}; {LD_MARK} /* pin: end of the C run of {tu_id} */\n')
                    continue
                stem = p['name'].split('/')[-1]
                rel = f'{CARVE_DIR}/{unit}/{stem}{sec}'
                if write:
                    write_piece_file(Path(unit_dir) / f'{rel}.s', p['header'], p['items'])
                out.append(f'{indent}build/{rel}.o({sec}); {LD_MARK}\n')
            text = text[:line.start()] + ''.join(out) + text[line.end():]
            report.append(dict(tu=tu_id, section=sec, run=[h8(lo), h8(hi)], c_object=c_obj,
                               pieces=[(p['name'], h8(p['start']), h8(p['end']), p['c_split']) for p in new]))
    return text, report


# ---------------------------------------------------------------------------
# derivation from a compiled candidate
# ---------------------------------------------------------------------------

def source_scaffold_names(source):
    """({INCLUDE_ASM function}, {ACCEPTED_ASM function: folder}) of a TU source."""
    inc, acc = set(), {}
    for line in Path(source).read_text(encoding='utf-8', errors='surrogateescape').splitlines():
        m = INC.match(line)
        if not m:
            continue
        if m.group(1) == 'ACCEPTED_ASM':
            acc[m.group(3)] = m.group(2)
        else:
            inc.add(m.group(3))
    return inc, acc


def identifiers(paths):
    out = set()
    for p in paths:
        if Path(p).is_file():
            out |= set(IDENT.findall(Path(p).read_text(errors='replace')))
    return out


def read_relocations(elf):
    """{section index: [(offset, type, symbol index)]} (REL and RELA)."""
    out = {}
    for s in elf.sections:
        if s.type not in (4, 9) or s.info >= len(elf.sections):
            continue
        size = 12 if s.type == 4 else 8
        for i in range(s.size // size):
            off, info = struct.unpack_from('<II', elf.data, s.offset + i * size)
            out.setdefault(s.info, []).append((off, info & 0xff, info >> 8))
    return out


def va_bytes(elf, va, size):
    for s in elf.sections:
        if s.type != 8 and s.flags & 2 and s.addr <= va and va + size <= s.addr + s.size:
            return elf.data[s.offset + va - s.addr:s.offset + va - s.addr + size]
    return None


def tu_context(root, unit, unit_dir, tu_id):
    """Pieces per section, function asm files and text start of one TU (unit-specific)."""
    unit_dir = Path(unit_dir)
    if unit == 'main':
        m = json.loads((unit_dir / 'tu-manifest.json').read_bytes())
        t = next((x for x in m['tus'] if x['id'] == tu_id), None)
        if t is None:
            raise CarveError(f'{tu_id} is not in {unit_dir}/tu-manifest.json')
        name = t['name']
        pieces, placed, imported = {}, set(), {}
        for sec, ts in t['sections'].items():
            if sec in SECTIONS:
                pieces[sec] = [dict(name=p['name'], start=hx(p['start']), end=hx(p['end']),
                                    c_split=p.get('c_split', False), file=p.get('file'))
                               for p in ts.get('pieces') or [dict(name=name, start=ts['start'], end=ts['end'])]]
                owner = (t.get('owners') or {}).get(sec)
                if owner == 'c':
                    placed.add(sec)            # the whole piece already comes from the C object
                elif owner == 'split':
                    c = [p for p in pieces[sec] if p['c_split']]
                    if c:
                        imported[sec] = (c[0]['start'], c[-1]['end'])
        fdirs = [unit_dir / f'asm/main/{d}/{name}' for d in ('nonmatchings', 'matchings')]
        text_start = hx(t['text']['splat_range'][0]) if t['text'].get('splat_range') else hx(t['text']['start'])
        orig = unit_dir / 'orig/SLUS_204.69'

        def path_of(p, sec):
            return main_piece_file(unit_dir, p, sec)
    else:
        name = ovl_tu_names(root, unit).get(tu_id)
        if name is None:
            raise CarveError(f'{tu_id} is not a TU of {unit}')
        pieces, placed, imported = {}, set(), {}
        p = ovl_piece(unit_dir, unit, name, '.rodata')
        if p:
            pieces['.rodata'] = [p]
        ld = Path(unit_dir) / f'{unit}.ld'
        if ld.is_file():
            text = ovl_restore(ld.read_text())
            obj = re.search(r'(build/(?:scaffold/)?src/' + re.escape(unit) + '/' + re.escape(name)
                            + r'\.o)\(\.text\);', text)
            if obj and f'{obj.group(1)}(.rodata);' in text:
                placed.add('.rodata')          # splat already links the C object's .rodata
        fdirs = [unit_dir / f'asm/{d}/{unit}/{name}' for d in ('nonmatchings', 'matchings')]
        layout = json.loads((unit_dir / 'layout.json').read_bytes())
        text_start = hx(layout['text'][f'{unit}/{name}'][0])
        target = re.search(r'target_path: (\S+)', (unit_dir / 'splat.yaml').read_text()).group(1)
        orig = unit_dir / target

        def path_of(p, sec):
            return Path(unit_dir) / p['file']
    return dict(name=name, pieces=pieces, fdirs=fdirs, text_start=text_start, orig=orig, path_of=path_of,
                placed=placed, imported=imported)


def tu_object(unit_dir, unit, name):
    """The object a unit's build links for a C TU, or None (not built, or not C)."""
    unit_dir = Path(unit_dir)
    if unit == 'main':
        path = unit_dir / f'build/c/{name}.o'
        return path if path.is_file() else None
    ld = unit_dir / f'{unit}.ld'
    if not ld.is_file():
        return None
    m = re.search(r'(build/(?:scaffold/)?src/' + re.escape(unit) + '/' + re.escape(name) + r'\.o)\(\.text\);',
                  ld.read_text())
    return unit_dir / m.group(1) if m and (unit_dir / m.group(1)).is_file() else None


def aliases(unit_dir):
    """{name: (target, offset)} from the unit's aliases.ld (MAIN)."""
    path = Path(unit_dir) / 'aliases.ld'
    out = {}
    if path.is_file():
        for m in PROVIDE.finditer(path.read_text()):
            out[m.group(1)] = (m.group(2), int(m.group(3), 0))
    return out


def section_items(ctx, sec):
    """[item] of every piece of one TU section, in address order."""
    items = []
    for p in ctx['pieces'].get(sec) or []:
        _, its = piece_items(ctx['path_of'](p, sec), p['start'], p['end'])
        items += its
    return items


COMMENT = re.compile(r'/\*.*?\*/')


def item_references(item):
    """Identifiers an item's own data names (the labels a pointer table points at)."""
    names = set()
    for line in item['block']['lines']:
        if LABEL.match(line):
            continue
        names |= set(IDENT.findall(COMMENT.sub(' ', line)))
    return names - set(item['labels'])


def run_specs(items, spans):
    """[dict(lo, hi, last, first_end)] of declared runs over the section's items."""
    out = []
    for lo, hi in spans:
        its = [it for it in items if lo <= it['start'] < hi]
        if not its or its[0]['start'] != lo:
            raise CarveError(f'the run {h8(lo)}-{h8(hi)} does not start at an item')
        out.append(dict(lo=lo, hi=hi, last=its[-1]['start'], first_end=its[0]['end'],
                        items=[(it['start'], it['end']) for it in its]))
    return out


def _sext16(value):
    value &= 0xffff
    return value - 0x10000 if value & 0x8000 else value


def _rel_entries(elf, rs):
    """[(offset, type, symbol index, explicit addend or None)] of one REL/RELA section."""
    esz = 12 if rs.type == 4 else 8
    out = []
    for i in range(rs.size // esz):
        off, info = struct.unpack_from('<II', elf.data, rs.offset + i * esz)
        add = struct.unpack_from('<i', elf.data, rs.offset + i * esz + 8)[0] if rs.type == 4 else None
        out.append((off, info & 0xff, info >> 8, add))
    return out


def gp0(elf):
    """The object's ri_gp_value (.reginfo): GAS writes a gp-relative in-place addend
    against a local symbol relative to it, and the linker adds it back."""
    reg = next((s for s in elf.sections if s.type == 0x70000006 and s.size >= 24), None)
    return struct.unpack_from('<i', elf.data, reg.offset + 20)[0] if reg else 0


def section_references(elf, csec):
    """[ref] of every relocation of the object that addresses `csec`.

    ref = dict(rel, entry, offset, type, sym, target): `target` is the offset in
    `csec` the relocation reaches (symbol value + addend).  An in-place (REL)
    addend is decoded as the MIPS rules pair it: R_MIPS_HI16 with the next
    R_MIPS_LO16 against the same symbol; an unpaired R_MIPS_LO16 carries the low
    half, which names one offset of a section smaller than 64 KiB."""
    syms = elf.symbols
    refs = []
    gp_base = gp0(elf)
    for rs in elf.sections:
        if rs.type not in (4, 9) or not rs.size or rs.info >= len(elf.sections):
            continue
        tgt = elf.sections[rs.info]
        tdata = elf.section_bytes(tgt)
        entries = _rel_entries(elf, rs)
        paired = {}
        for i, (off, rtype, symi, add) in enumerate(entries):
            if symi >= len(syms) or syms[symi].shndx != csec.index:
                continue
            sym = syms[symi]
            base = 0 if sym.type == 3 else sym.value
            where = f'{tgt.name}+0x{off:x}'
            if add is not None:
                addend = add
            else:
                if off + 4 > len(tdata):
                    raise CarveError(f'relocation at {where} lies outside {tgt.name}')
                word = struct.unpack_from('<I', tdata, off)[0]
                if rtype == 2:                                   # R_MIPS_32
                    addend = word - (1 << 32) if word & 0x80000000 else word
                elif rtype == 5:                                 # R_MIPS_HI16
                    j = next((j for j in range(i + 1, len(entries))
                              if entries[j][1] == 6 and entries[j][2] == symi), None)
                    if j is None:
                        raise CarveError(f'R_MIPS_HI16 at {where} against {csec.name} has no R_MIPS_LO16 partner')
                    low = struct.unpack_from('<I', tdata, entries[j][0])[0]
                    addend = ((word & 0xffff) << 16) + _sext16(low)
                    paired[j] = addend
                elif rtype == 6:                                 # R_MIPS_LO16
                    addend = paired.get(i)
                    if addend is None:
                        if csec.size >= 0x10000:
                            raise CarveError(f'unpaired R_MIPS_LO16 at {where}: {csec.name} is 64 KiB or larger')
                        addend = _sext16(word)
                        if not 0 <= base + addend <= csec.size:
                            addend = word & 0xffff
                elif rtype in (7, 8):                            # R_MIPS_GPREL16, R_MIPS_LITERAL
                    addend = _sext16(word) + (gp_base if sym.bind == 0 else 0)
                else:
                    raise CarveError(f'relocation type {rtype} at {where} against {csec.name} is not supported '
                                     f'by the split')
            target = base + addend
            if not 0 <= target <= csec.size:
                raise CarveError(f'relocation at {where} reaches {csec.name}+{target:#x}, outside the section '
                                 f'(0x{csec.size:x} bytes)')
            refs.append(dict(rel=rs.index, entry=i, offset=off, type=rtype, sym=symi, target=target))
    return refs


def plan_split(elf, csec, specs, want, check_items=False):
    """[dict(offset, length, lo, hi)]: the slice of `csec` that fills each run.

    `specs` from `run_specs`, in address order; `want(va, n)` gives original
    bytes.  One run takes the whole section (the rule of a single run).  Run k
    starts after the first byte of run k-1's last item, no later than run k-1's
    extent plus 15 alignment bytes, at an anchor (an offset a relocation or a
    symbol addresses) whose bytes are those of run k's first item in the
    original (trailing zero bytes and relocated words are not compared); among
    several such anchors the lowest.  C alignment fill between two runs beyond
    the original extent is left out of the pieces only when it is zero and no
    relocation or symbol lies in it.

    `check_items` (derive): once a run's offset is fixed, every further item of the
    run must hold its original bytes at the same distance from it (relocated words
    and trailing zeros not compared); the first that does not raises `Misplaced`,
    and `derive` starts a new run there. A C object whose run of several items
    starts at another alignment phase than the original (strings after an 8-byte
    aligned jump table, then a 16-byte aligned table) pads inside the run; each
    run then takes its own slice and the padding is the fill left out between
    them. The build (`split_plans`) plans the declared runs without it."""
    size = csec.size
    data = elf.section_bytes(csec)
    refs = section_references(elf, csec)
    own = [s for s in elf.symbols if s.shndx == csec.index and s.type not in (3, 4)]
    anchors = {r['target'] for r in refs} | {s.value for s in own}
    relocated = set()
    for rs in elf.sections:
        if rs.type in (4, 9) and rs.info == csec.index:
            for off, _, _, _ in _rel_entries(elf, rs):
                relocated.update(range(off, off + 4))

    def compatible(at, ref):
        for i, b in enumerate(ref):
            pos = at + i
            if pos >= size:
                return False
            if pos not in relocated and data[pos] != b:
                return False
        return True

    def check_run(k):
        o, r = offsets[k], specs[k]
        for start, end in (r.get('items') or [])[1:]:
            ref = bytes(want(start, end - start) or b'').rstrip(b'\0')
            if not compatible(o + start - r['lo'], ref):
                raise Misplaced(k, start)

    offsets, notes = [0], []
    if check_items and specs:
        check_run(0)
    for k in range(1, len(specs)):
        prev, cur = specs[k - 1], specs[k]
        low = offsets[-1] + (prev['last'] - prev['lo']) + 1
        high = offsets[-1] + (prev['hi'] - prev['lo']) + 15
        cands = sorted(a for a in anchors if low <= a <= high and a < size)
        ref = bytes(want(cur['lo'], cur['first_end'] - cur['lo']) or b'').rstrip(b'\0')
        fit = [a for a in cands if compatible(a, ref)]
        if not fit:
            raise CarveError(
                f'{csec.name}: no offset of the C object starts the run {h8(cur["lo"])}-{h8(cur["hi"])} '
                f'(anchors {[hex(a) for a in cands]} in +0x{low:x}..+0x{high:x}; none holds the bytes of its '
                f'first item): the C functions do not emit the items of that run where the original has them')
        if len(fit) > 1:
            notes.append(f'run {h8(cur["lo"])}: offsets {[hex(a) for a in fit]} fit; +0x{fit[0]:x} taken')
        offsets.append(fit[0])
        if check_items:
            check_run(k)
    pieces = []
    for k, r in enumerate(specs):
        o = offsets[k]
        end = offsets[k + 1] if k + 1 < len(specs) else size
        length, cap = end - o, r['hi'] - r['lo']
        if k + 1 < len(specs) and length > cap:
            # The C layout holds more between run k and run k+1 than the original
            # run k: alignment fill (the next C item is aligned further than the
            # scaffold item that followed in the original), or a C copy of a
            # constant the original TU emitted once as the scaffold item right
            # after the run (a string literal a C and an INCLUDE_ASM function
            # share). Either is left out of the pieces: zero fill nothing points
            # at, or bytes equal to the original's at the same place, whose
            # references then resolve to the scaffold copy at that address.
            lo_fill, hi_fill = o + cap, end
            same = want(r['hi'], hi_fill - lo_fill)
            fill = range(lo_fill, hi_fill)
            if any(x in relocated for x in fill) or any(s.value in fill for s in own):
                raise CarveError(f'{csec.name}: +0x{lo_fill:x}..+0x{hi_fill:x} of the C object lies between the '
                                 f'runs {h8(r["lo"])}-{h8(r["hi"])} and {h8(specs[k + 1]["lo"])} and holds a '
                                 f'relocation or a symbol')
            pointed = sorted({ref['target'] for ref in refs if lo_fill <= ref['target'] < hi_fill})
            if same is not None and bytes(data[lo_fill:hi_fill]) == bytes(same) and \
                    hi_fill - lo_fill <= specs[k + 1]['lo'] - r['hi']:
                notes.append(f'run {h8(r["lo"])}: +0x{lo_fill:x}..+0x{hi_fill:x} of the C object equals the '
                             f'original {h8(r["hi"])}..{h8(r["hi"] + hi_fill - lo_fill)} (scaffold); left out'
                             + (f', {len(pointed)} reference(s) resolve there' if pointed else ''))
            elif not any(data[lo_fill:hi_fill]) and not pointed:
                notes.append(f'run {h8(r["lo"])}: C alignment fill +0x{lo_fill:x}..+0x{hi_fill:x} left out')
            else:
                raise CarveError(f'{csec.name}: +0x{lo_fill:x}..+0x{hi_fill:x} of the C object lies between the '
                                 f'runs {h8(r["lo"])}-{h8(r["hi"])} and {h8(specs[k + 1]["lo"])}; it is neither '
                                 f'unreferenced zero fill nor the original bytes that follow the run')
            length = cap
        if not r['last'] - r['lo'] < length <= cap:
            raise CarveError(
                f'{csec.name}: the C object gives the run {h8(r["lo"])}-{h8(r["hi"])} 0x{length:x} bytes '
                f'(+0x{o:x}); it needs more than 0x{r["last"] - r["lo"]:x} and at most 0x{cap:x}')
        pieces.append(dict(offset=o, length=length, end=end, lo=r['lo'], hi=r['hi']))
    return dict(pieces=pieces, notes=notes)


def piece_of(pieces, x, what):
    """Index of the piece whose slice of the compiled section holds offset `x`.

    A piece reaches up to the next piece's offset (the part past its length is
    what `plan_split` left out between two runs, whose references resolve at
    the same distance from the run start) and the last one to the section end."""
    for k in range(len(pieces) - 1, -1, -1):
        p = pieces[k]
        if p['offset'] <= x:
            if x <= p.get('end', p['offset'] + p['length']):
                return k
            break
    raise CarveError(f'{what} reaches +0x{x:x}, which no run of the split holds')


def _lowbit(value):
    return value & -value if value else 1 << 30


def split_object(data, plans):
    """Bytes of the relocatable object `data` with each section of `plans` split.

    `plans`: {section name: [dict(offset, length, lo)]} (from `plan_split`).
    Piece 0 keeps the section (its size shrinks to the slice); piece k > 0
    becomes `<section>.carve.<k>` with its relocations in `.rel<section>.carve.<k>`.
    Section contents are copied, never changed.  A symbol defined in a later
    piece moves there with its value rebased; a relocation that reaches a later
    piece through the section symbol is re-pointed at a local symbol
    `.Lcarve<section>.<k>` of that piece whose value is minus the piece's offset
    (mod 2**32), so the linker's symbol + in-place addend is the item's address."""
    data = bytes(data)
    elf = Elf(data)
    (ident, e_type, machine, version, entry, phoff, shoff, eflags, ehsize, phentsize, phnum,
     shentsize, shnum, shstrndx) = struct.unpack_from('<16sHHIIIIIHHHHHH', data, 0)
    if e_type != 1:
        raise CarveError('split: not a relocatable object')
    sh = [list(struct.unpack_from('<IIIIIIIIII', data, shoff + i * shentsize)) for i in range(shnum)]
    body = [bytearray(b'' if s[1] == 8 else data[s[4]:s[4] + s[5]]) for s in sh]
    symtab = next(i for i, s in enumerate(sh) if s[1] == 2)
    strtab = sh[symtab][6]
    syms = [list(struct.unpack_from('<IIIBBH', body[symtab], 16 * i)) for i in range(sh[symtab][5] // 16)]
    first_global = sh[symtab][7]
    rels = {i: [list(e) for e in _rel_entries(elf, elf.sections[i])]
            for i, s in enumerate(sh) if s[1] in (4, 9) and s[5]}

    def add_name(index, name):
        off = len(body[index])
        body[index] += name.encode() + b'\0'
        return off

    new_syms = []                      # [symbol fields], inserted after the last local
    retarget = {}                      # (rel index, entry) -> index into new_syms
    moved_rel = {}                     # (rel index, entry) -> (new rel section index, new offset)
    for name, pieces in sorted(plans.items()):
        idx = [i for i, s in enumerate(elf.sections) if s.name == name]
        if len(idx) != 1:
            raise CarveError(f'split: the object has {len(idx)} {name} sections')
        r = idx[0]
        csec = elf.sections[r]
        if sum(p['length'] for p in pieces) > csec.size or pieces[0]['offset'] != 0:
            raise CarveError(f'split: the plan of {name} does not fit its 0x{csec.size:x} bytes')
        align = sh[r][8] or 1
        new_index = {}
        for k, p in enumerate(pieces):
            a = min(align, _lowbit(p['lo']), _lowbit(p['offset']) if k else align)
            if k == 0:
                sh[r][5], sh[r][8] = p['length'], a
                body[r] = body[r][:p['length']]
                continue
            sname = split_section_name(name, k)
            new_index[k] = len(sh)
            sh.append([add_name(shstrndx, sname), sh[r][1], sh[r][2], 0, 0, p['length'], 0, 0, a, sh[r][9]])
            body.append(bytearray(data[csec.offset + p['offset']:csec.offset + p['offset'] + p['length']]))
            new_syms.append([add_name(strtab, f'.Lcarve{name}.{k}'), (-p['offset']) & 0xffffffff, 0, 0, 0,
                             new_index[k]])
        base_sym = {k: len(new_syms) - (len(pieces) - 1) + (k - 1) for k in new_index}
        # the relocations of the split section itself go with their piece
        for ri in [i for i in list(rels) if sh[i][7] == r]:
            per = {}
            for e, ent in enumerate(rels[ri]):
                k = piece_of(pieces, ent[0], f'the relocation at {name}+0x{ent[0]:x}')
                if ent[0] + 4 > pieces[k]['offset'] + pieces[k]['length']:
                    raise CarveError(f'split: the relocation at {name}+0x{ent[0]:x} crosses a piece end')
                if k:
                    per.setdefault(k, []).append(e)
            for k, entries in sorted(per.items()):
                rname = ('.rela' if sh[ri][1] == 4 else '.rel') + split_section_name(name, k)
                new_rel = len(sh)
                sh.append([add_name(shstrndx, rname), sh[ri][1], sh[ri][2], 0, 0, 0, sh[ri][6], new_index[k],
                           sh[ri][8], sh[ri][9]])
                body.append(bytearray())
                rels[new_rel] = []
                for e in entries:
                    moved_rel[(ri, e)] = (new_rel, rels[ri][e][0] - pieces[k]['offset'])
        # named symbols move with their piece
        sym_piece = {}
        for si, s in enumerate(syms):
            if s[5] == r and (s[3] & 15) not in (3, 4):
                k = piece_of(pieces, s[1], f'the symbol {elf.symbols[si].name}')
                sym_piece[si] = k
                if k:
                    s[5], s[1] = new_index[k], s[1] - pieces[k]['offset']
        # relocations that address the section
        for ref in section_references(elf, csec):
            k = piece_of(pieces, ref['target'], f'the relocation at +0x{ref["offset"]:x} of section {ref["rel"]}')
            if elf.symbols[ref['sym']].type == 3:
                if k:
                    retarget[(ref['rel'], ref['entry'])] = base_sym[k]
            elif sym_piece.get(ref['sym'], 0) != k:
                raise CarveError(f'split: a relocation against {elf.symbols[ref["sym"]].name} reaches another run')
    # symbol table: new locals after the last local, globals shifted
    shift = len(new_syms)

    def remap(i):
        return i if i < first_global else i + shift

    table = syms[:first_global] + new_syms + syms[first_global:]
    body[symtab] = bytearray(b''.join(struct.pack('<IIIBBH', *s) for s in table))
    sh[symtab][5], sh[symtab][7] = len(body[symtab]), first_global + shift
    out_rel = {i: [] for i in rels}
    for ri, entries in rels.items():
        for e, (off, rtype, symi, add) in enumerate(entries):
            symi = first_global + retarget[(ri, e)] if (ri, e) in retarget else remap(symi)
            dest, off = moved_rel.get((ri, e), (ri, off))
            out_rel[dest].append((off, rtype, symi, add))
    for ri, entries in out_rel.items():
        buf = bytearray()
        for off, rtype, symi, add in entries:
            buf += struct.pack('<II', off, (symi << 8) | rtype)
            if sh[ri][1] == 4:
                buf += struct.pack('<i', add)
        body[ri], sh[ri][5] = buf, len(buf)
    for i in (shstrndx, strtab):
        sh[i][5] = len(body[i])
    # layout: header, section contents in index order, section header table
    out = bytearray(ehsize)
    for i in range(1, len(sh)):
        a = max(sh[i][8], 1)
        if sh[i][1] != 8:
            out += b'\0' * (-len(out) % a)
            sh[i][4] = len(out)
            out += body[i]
        else:
            sh[i][4] = len(out)
    out += b'\0' * (-len(out) % 4)
    new_shoff = len(out)
    for s in sh:
        out += struct.pack('<IIIIIIIIII', *s)
    out[:ehsize] = data[:ehsize]
    struct.pack_into('<I', out, 32, new_shoff)
    struct.pack_into('<H', out, 48, len(sh))
    return bytes(out)


def split_plans(root, unit, unit_dir, tu_id, obj_elf, registry=None):
    """{section: plan} for the sections a MAIN TU declares with several runs."""
    registry = load_registry(root) if registry is None else registry
    secs = registry['tus'].get(tu_id) or {}
    split = split_sections(secs)
    if not split:
        return {}
    ctx = tu_context(root, unit, unit_dir, tu_id)
    orig = Elf(Path(ctx['orig']).read_bytes())
    plans = {}
    for sec in split:
        csec = next((s for s in obj_elf.sections if s.name == sec and s.size), None)
        if csec is None:
            raise CarveError(f'{tu_id}: the C object emits no {sec}, but {len(secs[sec])} runs are declared')
        specs = run_specs(section_items(ctx, sec), declared_runs(tu_id, sec, secs[sec]))
        plans[sec] = plan_split(obj_elf, csec, specs, lambda va, n: va_bytes(orig, va, n))
    return plans


def derive(root, unit, unit_dir, tu_id, source, obj):
    """{'runs': {section: [run]}, 'sections': {section: detail}, 'problems': [..]} for one candidate."""
    ctx = tu_context(root, unit, unit_dir, tu_id)
    inc, acc = source_scaffold_names(source)
    files = {}
    for d in ctx['fdirs']:
        if d.is_dir():
            for f in sorted(d.glob('*.s')):
                files.setdefault(f.stem, f)
    c_funcs = sorted(n for n in files if n not in inc and n not in acc)
    scaffold_files = [files[n] for n in files if n in inc]
    for fn, folder in acc.items():
        scaffold_files.append(Path(unit_dir) / folder / f'{fn}.s')
    c_refs = identifiers(files[n] for n in c_funcs)
    per_func = {n: identifiers([files[n]]) for n in c_funcs}
    asm_refs = identifiers(scaffold_files)
    elf = Elf(Path(obj).read_bytes())
    orig = None
    names_ref = {s.name for s in elf.symbols if s.name and s.shndx == 0}
    names_def = {s.name for s in elf.symbols if s.name and 0 < s.shndx < 0xff00 and s.type in (0, 1)}
    al = aliases(unit_dir)
    extern = set(names_ref)
    for n in names_ref:
        if n in al:
            extern.add(al[n][0])
    out = dict(tu=tu_id, unit=unit, source=str(source), object=str(obj), c_functions=c_funcs,
               runs={}, sections={}, problems=[])
    # 1. the sections to carve and their items; the items the original assembly
    # of the C functions names (or the C object defines)
    work = {}
    for sec in SECTIONS:
        csec = next((s for s in elf.sections if s.name == sec and s.size), None)
        pieces = ctx['pieces'].get(sec)
        if csec is None or (unit != 'main' and sec in MAIN_ONLY_SECTIONS):
            continue
        detail = dict(object_size=csec.size)
        out['sections'][sec] = detail
        if sec in ctx['placed']:
            detail['note'] = 'the build already links the C object\'s whole section: nothing to carve'
            continue
        if not pieces:
            out['problems'].append(f'{sec}: the C object emits {csec.size} bytes but the TU has no {sec} piece')
            continue
        items = section_items(ctx, sec)
        gen, also = set(), set()
        for i, it in enumerate(items):
            labels = set(it['labels'])
            if labels & names_def:
                gen.add(i)
            elif labels & c_refs:
                (also if labels & extern else gen).add(i)
        work[sec] = dict(csec=csec, detail=detail, items=items, gen=gen, also=also, reached={},
                         refs=[item_references(it) for it in items])
    # 2. items only C-generated items point at, in any carved section (the strings
    # of a function-local `char *tbl[] = {...}` template, short ones in .sdata
    # under -G8): the C object emits them too
    todo = [(sec, i) for sec, w in work.items() for i in w['gen']]
    while todo:
        sec, i = todo.pop()
        names = work[sec]['refs'][i]
        for tsec, w in work.items():
            for j, it in enumerate(w['items']):
                if j in w['gen'] or j in w['also'] or not set(it['labels']) & names:
                    continue
                if set(it['labels']) & extern:
                    w['also'].add(j)
                    continue
                w['gen'].add(j)
                w['reached'][j] = (sec, i)
                todo.append((tsec, j))
    # 3. the runs of each section and the C object's slice for each
    for sec, w in work.items():
        csec, detail, items = w['csec'], w['detail'], w['items']
        gen, also = sorted(w['gen']), sorted(w['also'])
        detail['also_referenced_by_c'] = [items[i]['name'] for i in also]
        if not gen:
            out['problems'].append(
                f'{sec}: the C object emits {csec.size} bytes, but no {sec} item of the TU is referenced by the '
                f'original assembly of its C functions ({", ".join(c_funcs) or "none"}) or by an item they '
                f'generate; the data must be one the original functions generate (a literal the original named '
                f'elsewhere stays an extern)')
            continue
        # An item the C generates may also be named by scaffold code or data: a
        # constant GCC emitted once for the whole TU (the same string literal in a
        # C and an INCLUDE_ASM function). The C object emits it at its original
        # address and the scaffold reaches the label through c_aliases.ld; the
        # whole-file gate verifies the bytes. Reported, not refused.
        shared = sorted({items[i]['name'] for i in gen if set(items[i]['labels']) & asm_refs}
                        | {items[i]['name'] for i in gen for tsec, x in work.items()
                           for j in range(len(x['items'])) if j not in x['gen']
                           and set(items[i]['labels']) & x['refs'][j]})
        if shared:
            detail['shared_with_scaffold'] = shared
        groups = []
        for i in gen:
            if groups and groups[-1][-1] == i - 1:
                groups[-1].append(i)
            else:
                groups.append([i])
        if len(groups) > 1:
            detail['scaffold_between_runs'] = [
                dict(item=items[i]['name'],
                     referenced_by=sorted(n for n in files if n not in c_funcs
                                          and set(items[i]['labels']) & identifiers([files[n]])))
                for a, b in zip(groups, groups[1:]) for i in range(a[-1] + 1, b[0])]
        if len(groups) > 1 and unit != 'main':
            out['problems'].append(
                f'{sec}: the C-generated items form {len(groups)} runs separated by scaffold items '
                f'({", ".join(x["item"] for x in detail["scaffold_between_runs"])}); an overlay TU takes one C '
                f'run per section: recover the functions whose items lie in between, or keep this section '
                f'scaffold-owned')
            continue
        if orig is None:
            orig = Elf(Path(ctx['orig']).read_bytes())
        # A run whose C layout pads between two of its items (another alignment
        # phase than the original) is cut there into two runs (plan_split
        # check_items); every cut is recorded.
        plan, regrouped = None, []
        for _ in range(len(gen) + 1):
            spans = [(items[g[0]]['start'], items[g[-1]]['end']) for g in groups]
            try:
                plan = plan_split(elf, csec, run_specs(items, spans), lambda va, n: va_bytes(orig, va, n),
                                  check_items=True)
                break
            except Misplaced as exc:
                g = groups[exc.k]
                cut = next(n for n, i in enumerate(g) if items[i]['start'] == exc.start)
                groups[exc.k:exc.k + 1] = [g[:cut], g[cut:]]
                regrouped.append(dict(run=h8(items[g[0]]['start']), cut_at=items[g[cut]]['name']))
            except CarveError as exc:
                out['problems'].append(f'{sec}: {exc}')
                break
        if plan is None:
            if not out['problems'] or not out['problems'][-1].startswith(f'{sec}: '):
                out['problems'].append(f'{sec}: the runs cannot be planned')
            continue
        if regrouped:
            detail['cut_by_c_alignment'] = regrouped
            if unit != 'main':
                out['problems'].append(
                    f'{sec}: the C object pads inside the C-generated items ({regrouped}), which would take '
                    f'{len(groups)} runs; an overlay TU takes one C run per section')
                continue
        if plan['notes']:
            detail['split_notes'] = plan['notes']

        def root_functions(tsec, i):
            while i in work[tsec]['reached']:
                tsec, i = work[tsec]['reached'][i]
            return {n for n in c_funcs if set(work[tsec]['items'][i]['labels']) & per_func[n]}

        runs = []
        for k, (g, p) in enumerate(zip(groups, plan['pieces'])):
            lo, hi = spans[k]
            nxt = spans[k + 1][0] if k + 1 < len(spans) else 1 << 32
            run = dict(range=[h8(lo), h8(hi)], generated=[h8(lo), h8(lo + p['length'])],
                       generated_by=sorted(set().union(*(root_functions(sec, i) for i in g))),
                       c_owned_symbols=[items[i]['name'] for i in g],
                       also_referenced_by_c=[items[i]['name'] for i in also
                                             if (lo if k else 0) <= items[i]['start'] < nxt])
            through = {items[i]['name']: work[w['reached'][i][0]]['items'][w['reached'][i][1]]['name']
                       for i in g if i in w['reached']}
            if through:
                run['reached_through'] = through
            runs.append(run)
        detail['bytes'] = compare_bytes(elf, csec, plan['pieces'], ctx)
        if len(runs) == 1 and ctx['imported'].get(sec) == spans[0]:
            # exactly the import-era run the tracked split already cuts (docs/tu-build.md)
            detail['import_split'] = runs[0]
            continue
        out['runs'][sec] = runs
    return out


def symbolize(root, unit, unit_dir, tu_id, obj):
    """{(code section, offset): (label, immediate)} for the object's references into a declared run.

    The C object reaches a carved item through its own section symbol plus an
    in-place addend (`%hi/%lo(.rodata+A)`, `%gp_rel(.lit4+A)`), where the
    all-INCLUDE_ASM object names the item (`%hi/%lo(jtbl_...)`). Both link to the
    same address exactly when the item's address plus the same residual is what
    the addend reaches (through the run's slice of the section, `plan_split`),
    so each such relocation is re-expressed against the item label with the
    immediate the assembler would have written for it. Used only by the worker's
    per-function diagnostic; the whole-file gate compares the linked bytes."""
    declared = (load_registry(root)['tus'].get(tu_id) or {})
    if not declared:
        return {}
    ctx = tu_context(root, unit, unit_dir, tu_id)
    elf = Elf(Path(obj).read_bytes())
    orig = Elf(Path(ctx['orig']).read_bytes())
    runs = {}
    for sec, entries in declared.items():
        if sec not in ctx['pieces']:
            continue
        csec = next((x for x in elf.sections if x.name == sec and x.size), None)
        if csec is None:
            continue
        items = section_items(ctx, sec)
        try:
            spans = declared_runs(tu_id, sec, entries)
            plan = plan_split(elf, csec, run_specs(items, spans), lambda va, n: va_bytes(orig, va, n))
        except CarveError:
            continue
        labels = [(it['start'], it['name']) for it in items if any(lo <= it['start'] < hi for lo, hi in spans)]
        if labels:
            runs[csec.index] = (plan['pieces'], sorted(labels))
    out = {}
    if not runs:
        return out
    gp_base = gp0(elf)
    for code_index, relocs in read_relocations(elf).items():
        code = elf.sections[code_index]
        data = elf.data[code.offset:code.offset + code.size]
        pending_hi = {}
        entries = []
        for off, rtype, symi in relocs:
            sym = elf.symbols[symi]
            if sym.type != 3 or sym.shndx not in runs or off + 4 > len(data):
                continue
            imm = struct.unpack_from('<I', data, off)[0] & 0xffff
            if rtype == 5:                                  # R_MIPS_HI16: paired with the next LO16
                pending_hi[sym.shndx] = (off, imm)
                entries.append([off, rtype, sym.shndx, None])
            elif rtype in (6, 7, 8):                        # R_MIPS_LO16 / R_MIPS_GPREL16 / R_MIPS_LITERAL
                low = imm - 0x10000 if imm & 0x8000 else imm
                if rtype == 6 and sym.shndx in pending_hi:
                    addend = (pending_hi[sym.shndx][1] << 16) + low
                    for e in entries:
                        if e[2] == sym.shndx and e[1] == 5 and e[3] is None:
                            e[3] = addend
                else:
                    addend = low + (gp_base if rtype in (7, 8) else 0)
                entries.append([off, rtype, sym.shndx, addend])
        for off, rtype, shndx, addend in entries:
            if addend is None:
                continue
            pieces, labels = runs[shndx]
            try:
                k = piece_of(pieces, addend, 'a reference')
            except CarveError:
                continue
            va = pieces[k]['lo'] + addend - pieces[k]['offset']
            start, label = max(((a, n) for a, n in labels if a <= va), default=(None, None))
            if label is None:
                continue
            residual = va - start
            imm = ((residual + 0x8000) >> 16) & 0xffff if rtype == 5 else residual & 0xffff
            out[(code.name, off)] = (label, imm)
    return out


def compare_bytes(elf, csec, pieces, ctx):
    """The C section with its relocations resolved at the TU's original addresses vs the original runs.

    `pieces`: the plan of `plan_split` (or, for one run, the run start as an int)."""
    data = bytearray(elf.section_bytes(csec))
    if isinstance(pieces, int):
        pieces = [dict(offset=0, length=len(data), lo=pieces)]
    orig = Elf(Path(ctx['orig']).read_bytes())
    text = next((s for s in elf.sections if s.name == '.text'), None)
    unresolved, diff = 0, []
    wants = []
    for p in pieces:
        want = va_bytes(orig, p['lo'], p['length'])
        if want is None:
            return dict(checked=False, reason='a run is not inside an allocated section of the original')
        wants.append(want)
    for off, rtype, symi in read_relocations(elf).get(csec.index, []):
        if off + 4 > len(data):
            unresolved += 1
            continue
        k = next((k for k, p in enumerate(pieces) if p['offset'] <= off < p['offset'] + p['length']), None)
        if k is None:
            continue
        sym = elf.symbols[symi]
        pos = off - pieces[k]['offset']
        if rtype != 2 or sym.shndx not in ((text.index if text else -1), csec.index):
            unresolved += 1
            data[off:off + 4] = wants[k][pos:pos + 4]      # an external address: not checked here
            continue
        target = sym.value + struct.unpack_from('<I', data, off)[0]
        if sym.shndx == csec.index:
            try:
                j = piece_of(pieces, target, 'a pointer')
            except CarveError:
                unresolved += 1
                continue
            value = pieces[j]['lo'] + target - pieces[j]['offset']
        else:
            value = ctx['text_start'] + target
        struct.pack_into('<I', data, off, value & 0xffffffff)
    for p, want in zip(pieces, wants):
        got = data[p['offset']:p['offset'] + p['length']]
        diff += [h8(p['lo'] + i) for i in range(0, len(got), 4) if got[i:i + 4] != want[i:i + 4]]
    return dict(checked=True, equal=not diff, differing_words=diff[:8], unresolved_relocations=unresolved)


# ---------------------------------------------------------------------------
# command line
# ---------------------------------------------------------------------------

def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest='cmd', required=True)
    for name in ('derive', 'check'):
        p = sub.add_parser(name)
        p.add_argument('--root', type=Path, required=True)
        p.add_argument('--unit', required=True)
        p.add_argument('--unit-dir', type=Path, required=True)
        p.add_argument('--tu', required=True)
        p.add_argument('--source', type=Path, required=True)
        p.add_argument('--obj', type=Path, required=True)
        p.add_argument('--json-out', type=Path)
    sub.choices['derive'].add_argument(
        '--write', action='store_true',
        help='record the derived runs as this TU\'s entry of ROOT/config/tu/data-carves.json '
             '(a private root: a worker sandbox or a test tree; publication is tools/tu_publish.py)')
    sub.choices['derive'].add_argument('--basis', help='JSON object recorded as the entry\'s basis')
    p = sub.add_parser('show')
    p.add_argument('--root', type=Path, required=True)
    p.add_argument('--tu')
    p = sub.add_parser('split', help='write the object a MAIN TU with several runs in a section links')
    p.add_argument('--root', type=Path, required=True)
    p.add_argument('--unit', default='main')
    p.add_argument('--unit-dir', type=Path, required=True)
    p.add_argument('--tu', required=True)
    p.add_argument('--obj', type=Path, required=True)
    p.add_argument('--out', type=Path, required=True)
    a = ap.parse_args(argv)
    if a.cmd == 'show':
        reg = load_registry(a.root)
        tus = reg['tus'] if not a.tu else {a.tu: reg['tus'].get(a.tu)}
        print(json.dumps(tus, indent=1))
        return 0
    if a.cmd == 'split':
        tmp = a.out.with_name(a.out.name + '.tmp')
        try:
            data = a.obj.read_bytes()
            plans = split_plans(a.root, a.unit, a.unit_dir, a.tu, Elf(data))
            if not plans:
                raise CarveError(f'{a.tu} declares no section with several runs: link {a.obj} itself')
            tmp.write_bytes(split_object(data, {sec: p['pieces'] for sec, p in plans.items()}))
            os.replace(tmp, a.out)
        except CarveError as exc:
            if tmp.exists():
                tmp.unlink()
            print(f'data_carve split {a.tu}: {exc}', file=sys.stderr)
            return 1
        report = dict(tu=a.tu, object=str(a.obj), out=str(a.out),
                      sections={sec: dict(pieces=[dict(section=split_section_name(sec, k), offset=hex(p['offset']),
                                                       length=hex(p['length']), va=h8(p['lo']), run_end=h8(p['hi']))
                                                  for k, p in enumerate(plan['pieces'])],
                                          notes=plan['notes'])
                                for sec, plan in plans.items()})
        a.out.with_name(a.out.name + '.json').write_text(json.dumps(report, indent=1) + '\n')
        return 0
    try:
        got = derive(a.root, a.unit, a.unit_dir, a.tu, a.source, a.obj)
    except CarveError as exc:
        got = dict(tu=a.tu, runs={}, problems=[str(exc)], error=True)
    if a.cmd == 'check':
        declared = load_registry(a.root)['tus'].get(a.tu) or {}
        got['declared'] = declared
        got['consistent'] = normalize_runs(declared) == normalize_runs(got['runs']) and not got['problems']
    if a.cmd == 'derive' and a.write and not got['problems']:
        basis = json.loads(a.basis) if a.basis else dict(kind='derived', tool='tools/tu/data_carve.py derive',
                                                           object=str(a.obj))
        got['written'] = write_registry(a.root, a.tu, got['runs'], basis=basis)
    if a.json_out:
        a.json_out.write_text(json.dumps(got, indent=1) + '\n')
    print(json.dumps(got, indent=1))
    if a.cmd == 'check':
        return 0 if got['consistent'] else 1
    return 1 if got['problems'] else 0


if __name__ == '__main__':
    raise SystemExit(main())
