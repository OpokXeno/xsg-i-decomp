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

One run per TU and section: a C object has one input section per section, so the
run is one contiguous extent; several adjacent tables of several C functions
form one run (their order in the C object is the source order, which is the
original order).  Tables separated by scaffold items cannot be carved (the
refusal names the items in between and the functions that reference them).

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
  * the generated items must be consecutive, no scaffold (INCLUDE_ASM or
    ACCEPTED_ASM) function may reference them, and the C object's section must
    fit the run: longer than the run minus its last item and not longer than
    the run;
  * the C object's bytes, with its relocations resolved against the TU's
    original addresses, are compared with the original bytes of the run as a
    diagnostic (`bytes`); the whole-file gate stays the verdict.

    data_carve.py derive --root R --unit U --unit-dir D --tu ID --source SRC --obj OBJ [--json-out F]
    data_carve.py check  --root R --unit U --unit-dir D --tu ID --source SRC --obj OBJ
    data_carve.py show   --root R [--tu ID]
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
# and double literals (.lit8).
SECTIONS = ('.rodata', '.lit4', '.lit8')
CARVE_DIR = 'carve'
LD_MARK = '/* data-carve */'
LD_ORIG = '/* data-carve-orig: '

NONMATCHING = re.compile(r'^nonmatching\s+(\S+?)(?:,.*)?\s*$')
ALIGN = re.compile(r'^\.align\s+\d+\s*$')
LABEL = re.compile(r'^\s*(?:dlabel|glabel|jlabel|alabel|ehlabel)\s+(\S+?)(?:,.*)?\s*$')
ADDR = re.compile(r'^\s*/\* ([0-9A-F]+) ([0-9A-F]{8}) ')
INC = re.compile(r'^\s*(INCLUDE_ASM|ACCEPTED_ASM)\(\s*"([^"]+)"\s*,\s*([^)\s]+)\s*\)\s*;')
IDENT = re.compile(r'[A-Za-z_.$][\w.$]*')
PROVIDE = re.compile(r'PROVIDE\(\s*"?([^"\s=]+)"?\s*=\s*"?([^"\s+)]+)"?\s*\+\s*(0x[0-9A-Fa-f]+|\d+)\s*\)')


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
                                            'also_referenced_by_c') if k in r}
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

def repiece(tu_name, pieces, run, read_items):
    """New pieces of one TU section for one C run.

    `pieces`: [dict(name, start, end, c_split, file)] in address order (the
    section's current split, import-era c_split pieces included); `run`: (lo, hi);
    `read_items(piece)` -> (header, items).  Every cut lies on an item boundary;
    the pieces inside the run merge into one C-owned piece; an import-era
    c_split piece outside the run is refused (the declaration must cover it).
    Returns [dict(name, start, end, c_split, header, items, carved)]."""
    lo, hi = run
    first, last = pieces[0]['start'], pieces[-1]['end']
    if not (first <= lo < hi <= last):
        raise CarveError(f'{tu_name}: run {h8(lo)}-{h8(hi)} is outside the TU piece {h8(first)}-{h8(last)}')
    segments = []                                  # (start, end, header, items, source piece, carved)
    for p in pieces:
        header, items = read_items(p)
        if p['c_split'] and not (lo <= p['start'] and p['end'] <= hi):
            raise CarveError(f'{tu_name}: the import-era C run {h8(p["start"])}-{h8(p["end"])} is not inside '
                             f'the declared run {h8(lo)}-{h8(hi)}')
        cuts = sorted({b for b in (lo, hi) if p['start'] < b < p['end']})
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
        inside = lo <= seg['start'] and seg['end'] <= hi
        if inside and out and out[-1]['c_split']:
            prev = out[-1]
            prev.update(end=seg['end'], items=prev['items'] + seg['items'], carved=True)
            continue
        p = seg['piece']
        # An untouched piece keeps its splat name and file; a piece whose extent or
        # ownership changes is written to <unit_dir>/carve/ under `carve/<name>`, so
        # its object never shares a name with the pristine splat piece.
        untouched = seg['start'] == p['start'] and not seg['carved'] and inside == bool(p['c_split'])
        base = p['name'] if seg['start'] == p['start'] else f'{tu_name}__{seg["start"]:08X}'
        out.append(dict(name=base, start=seg['start'], end=seg['end'], c_split=inside,
                        header=seg['header'], items=seg['items'], carved=not untouched, source=p))
    for p in out:
        if p['carved']:
            p['name'] = f'{CARVE_DIR}/{p["name"].split("/")[-1]}'
    if sum(1 for p in out if p['c_split']) != 1:
        raise CarveError(f'{tu_name}: the run {h8(lo)}-{h8(hi)} does not form exactly one piece')
    return out


def check_one_run(tu_id, sec, runs):
    if len(runs) != 1:
        raise CarveError(
            f'{tu_id} {sec}: {len(runs)} C runs declared; a C object has one {sec} input section, so the '
            f'C-generated items of one section must form one contiguous run (recover the functions whose '
            f'items lie in between, or keep the section scaffold-owned)')
    r = runs[0]
    return hx(r['range'][0]), hx(r['range'][1])


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
        for sec, runs in sorted(secs.items()):
            lo, hi = check_one_run(tu_id, sec, runs)
            if sec not in t['sections']:
                raise CarveError(f'{tu_id} has no {sec} piece')
            ts = t['sections'][sec]
            pieces = [dict(name=p['name'], start=hx(p['start']), end=hx(p['end']), c_split=p.get('c_split', False),
                           file=p.get('file'))
                      for p in ts.get('pieces') or [dict(name=t['name'], start=ts['start'], end=ts['end'])]]

            def read(p, sec=sec):
                return piece_items(main_piece_file(unit_dir, p, sec), p['start'], p['end'])

            new = repiece(t['name'], pieces, (lo, hi), read)
            out_pieces = []
            for p in new:
                entry = dict(c_split=p['c_split'], end=f'0x{p["end"]:08X}', name=p['name'],
                             start=f'0x{p["start"]:08X}')
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
                         start=p['start'], tu=t['name'], **({'file': p['file']} if p.get('file') else {}))
                    for p in out_pieces]
            order[idx[0]:idx[-1] + 1] = repl
            report.append(dict(tu=tu_id, section=sec, run=[h8(lo), h8(hi)],
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
    names_ref = {s.name for s in elf.symbols if s.name and s.shndx == 0}
    names_def = {s.name for s in elf.symbols if s.name and 0 < s.shndx < 0xff00 and s.type in (0, 1)}
    al = aliases(unit_dir)
    extern = set(names_ref)
    for n in names_ref:
        if n in al:
            extern.add(al[n][0])
    out = dict(tu=tu_id, unit=unit, source=str(source), object=str(obj), c_functions=c_funcs,
               runs={}, sections={}, problems=[])
    for sec in SECTIONS:
        csec = next((s for s in elf.sections if s.name == sec and s.size), None)
        pieces = ctx['pieces'].get(sec)
        if csec is None:
            continue
        detail = dict(object_size=csec.size)
        out['sections'][sec] = detail
        if sec in ctx['placed']:
            detail['note'] = 'the build already links the C object\'s whole section: nothing to carve'
            continue
        if not pieces:
            out['problems'].append(f'{sec}: the C object emits {csec.size} bytes but the TU has no {sec} piece')
            continue
        items = []
        for p in pieces:
            _, its = piece_items(ctx['path_of'](p, sec), p['start'], p['end'])
            items += its
        gen, also = [], []
        for i, it in enumerate(items):
            labels = set(it['labels'])
            if labels & names_def:
                gen.append(i)
            elif labels & c_refs:
                (also if labels & extern else gen).append(i)
        detail['also_referenced_by_c'] = [items[i]['name'] for i in also]
        if not gen:
            out['problems'].append(
                f'{sec}: the C object emits {csec.size} bytes, but no {sec} item of the TU is referenced by the '
                f'original assembly of its C functions ({", ".join(c_funcs) or "none"}); the data must be one the '
                f'original functions generate (a literal the original named elsewhere stays an extern)')
            continue
        gaps = [i for i in range(gen[0], gen[-1] + 1) if i not in gen]
        if gaps:
            who = {items[i]['name']: sorted(n for n in files if n not in c_funcs
                                             and set(items[i]['labels']) & identifiers([files[n]]))
                   for i in gaps}
            out['problems'].append(
                f'{sec}: the C-generated items are not contiguous; scaffold items lie in between: '
                + '; '.join(f'{k} (referenced by {", ".join(v) or "no function"})' for k, v in who.items())
                + '. A C object has one input section per section: recover those functions as C too, or '
                  'keep this section scaffold-owned')
            continue
        clash = sorted({items[i]['name'] for i in gen if set(items[i]['labels']) & asm_refs})
        if clash:
            out['problems'].append(f'{sec}: items the C generates are also referenced by scaffold functions: '
                                   f'{clash}')
            continue
        lo, hi = items[gen[0]]['start'], items[gen[-1]]['end']
        last = items[gen[-1]]['start']
        if not (last - lo < csec.size <= hi - lo):
            out['problems'].append(
                f'{sec}: the C object emits {csec.size} bytes, the run {h8(lo)}-{h8(hi)} of '
                f'{[items[i]["name"] for i in gen]} needs more than {last - lo} and at most {hi - lo}')
            continue
        by = sorted(n for n in c_funcs if any(set(items[i]['labels']) & per_func[n] for i in gen))
        run = dict(range=[h8(lo), h8(hi)], generated=[h8(lo), h8(lo + csec.size)], generated_by=by,
                   c_owned_symbols=[items[i]['name'] for i in gen],
                   also_referenced_by_c=[items[i]['name'] for i in also])
        detail['bytes'] = compare_bytes(elf, csec, lo, ctx)
        if ctx['imported'].get(sec) == (lo, hi):
            # exactly the import-era run the tracked split already cuts (docs/tu-build.md)
            detail['import_split'] = run
            continue
        out['runs'][sec] = [run]
    return out


def symbolize(root, unit, unit_dir, tu_id, obj):
    """{(code section, offset): (label, immediate)} for the object's references into a declared run.

    The C object reaches a carved item through its own section symbol plus an
    in-place addend (`%hi/%lo(.rodata+A)`, `%gp_rel(.lit4+A)`), where the
    all-INCLUDE_ASM object names the item (`%hi/%lo(jtbl_...)`). Both link to the
    same address exactly when run start + A is that item plus the same residual,
    so each such relocation is re-expressed against the item label with the
    immediate the assembler would have written for it. Used only by the worker's
    per-function diagnostic; the whole-file gate compares the linked bytes."""
    declared = (load_registry(root)['tus'].get(tu_id) or {})
    if not declared:
        return {}
    ctx = tu_context(root, unit, unit_dir, tu_id)
    elf = Elf(Path(obj).read_bytes())
    runs = {}
    for sec, items in declared.items():
        if len(items) != 1 or sec not in ctx['pieces']:
            continue
        lo, hi = hx(items[0]['range'][0]), hx(items[0]['range'][1])
        labels = []
        for p in ctx['pieces'][sec]:
            _, its = piece_items(ctx['path_of'](p, sec), p['start'], p['end'])
            labels += [(it['start'], it['name']) for it in its if lo <= it['start'] < hi]
        csec = next((x for x in elf.sections if x.name == sec), None)
        if csec is not None and labels:
            runs[csec.index] = (lo, sorted(labels))
    out = {}
    if not runs:
        return out
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
            elif rtype in (6, 7):                           # R_MIPS_LO16 / R_MIPS_GPREL16
                low = imm - 0x10000 if imm & 0x8000 else imm
                if rtype == 6 and sym.shndx in pending_hi:
                    addend = (pending_hi[sym.shndx][1] << 16) + low
                    for e in entries:
                        if e[2] == sym.shndx and e[1] == 5 and e[3] is None:
                            e[3] = addend
                else:
                    addend = low
                entries.append([off, rtype, sym.shndx, addend])
        for off, rtype, shndx, addend in entries:
            if addend is None:
                continue
            lo, labels = runs[shndx]
            va = lo + addend
            start, label = max(((a, n) for a, n in labels if a <= va), default=(None, None))
            if label is None:
                continue
            residual = va - start
            imm = ((residual + 0x8000) >> 16) & 0xffff if rtype == 5 else residual & 0xffff
            out[(code.name, off)] = (label, imm)
    return out


def compare_bytes(elf, csec, lo, ctx):
    """The C section with its relocations resolved at the TU's original addresses vs the original run."""
    data = bytearray(elf.section_bytes(csec))
    orig = Elf(Path(ctx['orig']).read_bytes())
    want = va_bytes(orig, lo, len(data))
    if want is None:
        return dict(checked=False, reason='the run is not inside an allocated section of the original')
    unresolved, bases = 0, {}
    text = next((s for s in elf.sections if s.name == '.text'), None)
    if text is not None:
        bases[text.index] = ctx['text_start']
    bases[csec.index] = lo
    for off, rtype, symi in read_relocations(elf).get(csec.index, []):
        if rtype != 2 or off + 4 > len(data):            # R_MIPS_32
            unresolved += 1
            continue
        sym = elf.symbols[symi]
        if sym.shndx not in bases:
            unresolved += 1
            data[off:off + 4] = want[off:off + 4]          # an external address: not checked here
            continue
        addend = struct.unpack_from('<I', data, off)[0]
        struct.pack_into('<I', data, off, (bases[sym.shndx] + sym.value + addend) & 0xffffffff)
    diff = [f'+0x{i:x}' for i in range(0, len(data), 4) if data[i:i + 4] != want[i:i + 4]]
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
    a = ap.parse_args(argv)
    if a.cmd == 'show':
        reg = load_registry(a.root)
        tus = reg['tus'] if not a.tu else {a.tu: reg['tus'].get(a.tu)}
        print(json.dumps(tus, indent=1))
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
