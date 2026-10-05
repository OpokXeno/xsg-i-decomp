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
builds exactly as before (no split object). Overlays use the same split for
several `.rodata` runs (tools/tu/ninja_ovl.py links the split object).

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
import ast
import copy
import hashlib
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
PUBLIC_REGISTRY = 'config/objects/data-carves.json'
SCHEMA = 'tu-data-carves/1'
# Sections a compiler fills with anonymous constants of a function: jump tables
# and string/float/double literals (.rodata), gp-relative float literals (.lit4)
# and double literals (.lit8), and under -G8 the small (<= 8 byte) string
# literals and initialised-template copies GCC puts in .sdata (MAIN only: the
# overlays carve .rodata and initialized .data).
# .data includes function-local initialized arrays such as dispatch tables.
SECTIONS = ('.rodata', '.lit4', '.lit8', '.sdata', '.data', '.sbss', '.bss')
OVERLAY_SECTIONS = ('.rodata', '.data')
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
ADDR = re.compile(r'^\s*/\* (?:(?:[0-9A-F]{4,6}) )?([0-9A-F]{8})(?: [0-9A-F]{8})? \*/')
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


def registry_input_path(root):
    private = registry_path(root)
    return private if private.is_file() else Path(root) / PUBLIC_REGISTRY


def load_registry(root):
    """{'schema', 'tus': {tu_id: {section: [run]}}}; an absent file declares nothing."""
    path = registry_input_path(root)
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
        out[sec] = []
        for r in items:
            row = dict(range=[h8(hx(r['range'][0])), h8(hx(r['range'][1]))],
                       c_owned_symbols=sorted(r.get('c_owned_symbols') or []))
            if r.get('uncredited_c_symbols'):
                row['uncredited_c_symbols'] = sorted(set(r['uncredited_c_symbols']))
            if r.get('uncredited_scaffold_ranges'):
                row['uncredited_scaffold_ranges'] = [dict(
                    range=[h8(hx(x['range'][0])), h8(hx(x['range'][1]))],
                    credit=x.get('credit', 'uncredited_dependency'),
                    **({'evidence_path': x['evidence_path']} if x.get('evidence_path') else {}),
                    **({'evidence_sha256': x['evidence_sha256']} if x.get('evidence_sha256') else {}))
                    for x in r['uncredited_scaffold_ranges']]
            # NOBITS input sections have no bytes from which to infer ownership.
            # Carry the exact raw compiler-section placement proof through the
            # private manifest instead of assuming every .sbss run is SCOMMON.
            if r.get('c_input_spans'):
                row['c_input_spans'] = [dict(
                    section=x['section'],
                    range=[h8(hx(x['range'][0])), h8(hx(x['range'][1]))],
                    symbols=sorted(x.get('symbols') or []),
                    **({'object_range': [int(v) for v in x['object_range']]}
                       if x.get('object_range') is not None else {}),
                    **({'credit': x['credit']} if x.get('credit') is not None else {}),
                    **({'anonymous_emission': True} if x.get('anonymous_emission') is True else {}))
                    for x in r['c_input_spans']]
            if r.get('c_storage_aliases'):
                row['c_storage_aliases'] = sorted((dict(
                    original_name=x['original_name'], address=h8(hx(x['address'])),
                    storage_owner=dict(name=x['storage_owner']['name'],
                                       address=h8(hx(x['storage_owner']['address'])),
                                       size=int(x['storage_owner']['size'])),
                    **({'storage_member': x['storage_member']} if x.get('storage_member') else {}))
                    for x in r['c_storage_aliases']), key=lambda x: (x['address'], x['original_name']))
            out[sec].append(row)
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
                                            'also_referenced_by_c', 'reached_through',
                                            'c_input_spans', 'c_storage_aliases',
                                            'uncredited_c_symbols', 'uncredited_scaffold_ranges') if k in r}
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
                start = (int(a.group(1), 16), 0)
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


def _data_directives(item):
    """[(address, natural alignment, line index)] for addressed data directives."""
    aligns = {'byte': 1, 'ascii': 1, 'asciz': 1, 'space': 1, 'zero': 1,
              'half': 2, 'short': 2, '2byte': 2,
              'word': 4, 'long': 4, '4byte': 4, 'float': 4,
              'dword': 8, 'quad': 8, '8byte': 8, 'double': 8}
    found = []
    for i, line in enumerate(item['block']['lines']):
        addr = ADDR.match(line)
        directive = re.search(r'\*/\s*\.([A-Za-z0-9]+)\b', line)
        if addr and directive and directive.group(1) in aligns:
            found.append((int(addr.group(1), 16), aligns[directive.group(1)], i))
    return found

def _split_item_at(item, cut, line_index, symbol_size=None):
    """Split one data-label block at a directive boundary, retaining symbol extent."""
    labels = item['labels']
    if len(labels) > 1 or not item['start'] < cut < item['end']:
        raise CarveError(f'{item["name"]}: cannot split an interior phase boundary without one named item')
    label = labels[0] if labels else None
    lines = item['block']['lines']
    prefix, suffix = lines[:line_index], lines[line_index:]
    if label and not any(LABEL.match(line) and LABEL.match(line).group(1) == label for line in prefix):
        raise CarveError(f'{item["name"]}: phase boundary precedes its owning label')
    # The symbol's bytes remain contiguous in the linked image, though the
    # natural-alignment split uses two input sections. Preserve the original
    # symbol extent explicitly in the prefix object; the suffix has no duplicate
    # label and emits the untouched remaining data directives.
    prefix = [line for line in prefix if not re.match(r'^\s*(?:enddlabel|\.size)\b', line)]
    suffix = [line for line in suffix if not re.match(r'^\s*(?:enddlabel|\.size)\b', line)]
    if label:
        extent = item['end'] - item['start'] if symbol_size is None else symbol_size
        prefix.extend([f'enddlabel {label}', f'.size {label}, {extent}'])
    marker = f'nonmatching {label or item["name"]}__phase_{cut:08X}'
    suffix.insert(0, marker)
    left = dict(item, name=label or item['name'], end=cut, block=dict(item['block'], lines=prefix))
    right = dict(item, name=f'D_{cut:08X}', labels=[], start=cut,
                 block=dict(item['block'], lines=suffix))
    return left, right


def _anonymous_span_names_are_scaffold(items, names, lo, hi):
    """Require credited original labels to lie inside the exact scaffold span."""
    if not names:
        return False
    labels = {label for item in items if lo <= item['start'] < hi for label in item.get('labels', [])}
    return set(names) <= labels

# ---------------------------------------------------------------------------
# re-piecing a section of one TU
# ---------------------------------------------------------------------------

def split_space_items(items, cuts, tu_name):
    """Split NOBITS `.space` items at exact proven owner boundaries.

    This is used only while carving a MAIN `.bss`/`.sbss` scaffold. The source
    `.s` file is left untouched; generated fragments retain the original
    `.space` total, and only the first fragment retains the original label.
    """
    out = []
    for it in items:
        interior = sorted(c for c in cuts if it['start'] < c < it['end'])
        if not interior:
            out.append(it)
            continue
        lines = it['block']['lines']
        spaces = [(i, re.match(r'^(\s*/\*\s*[0-9A-Fa-f]{8}\s*\*/\s*)\.space\s+(0x[0-9A-Fa-f]+|\d+)\s*$', line))
                  for i, line in enumerate(lines)]
        spaces = [(i, m) for i, m in spaces if m]
        if len(spaces) != 1 or int(spaces[0][1].group(2), 0) != it['end'] - it['start']:
            raise CarveError(f"{tu_name}: NOBITS cut inside {it['name']} at {', '.join(h8(c) for c in interior)} "
                             "does not split one exact `.space` item")
        space_index, space_match = spaces[0]
        bounds = [it['start'], *interior, it['end']]
        for k, (lo, hi) in enumerate(zip(bounds, bounds[1:])):
            size = hi - lo
            if k == 0:
                block_lines = list(lines)
                for i, line in enumerate(block_lines):
                    if NONMATCHING.match(line):
                        block_lines[i] = f"nonmatching {it['name']}, 0x{size:X}"
                prefix = space_match.group(1)
            else:
                # The split point has no original ELF label. This marker only
                # bounds the generated scaffold fragment and emits no symbol.
                block_lines = [f"nonmatching D_{lo:08X}, 0x{size:X}", ""]
                prefix = f"    /* {lo:08X} */ "
            block_lines[space_index if k == 0 else -1] = prefix + f".space 0x{size:X}"
            labels = list(it['labels']) if k == 0 else []
            out.append(dict(name=it['name'] if k == 0 else f'D_{lo:08X}', labels=labels,
                            start=lo, end=hi,
                            block=dict(start=lo, offset=0, labels=labels, lines=block_lines)))
    return out


def split_initialized_items(items, cuts, run_ends, tu_name):
    """Split PROGBITS data items only at exact directive boundaries or .space extents."""
    out = []
    for item in items:
        interior = sorted(c for c in cuts if item['start'] < c < item['end'])
        if not interior:
            out.append(item)
            continue
        lines = item['block']['lines']
        spaces = [(i, re.match(r'^\s*(?:/\*\s*[0-9A-Fa-f]{8}\s*\*/\s*)?\.space\s+(0x[0-9A-Fa-f]+|\d+)\s*$', line))
                  for i, line in enumerate(lines)]
        spaces = [(i, m) for i, m in spaces if m]
        if len(spaces) == 1 and int(spaces[0][1].group(1), 0) == item['end'] - item['start']:
            out.extend(split_space_items([item], interior, tu_name))
            continue
        pieces = [item]
        for cut in interior:
            index = next((i for i, p in enumerate(pieces) if p['start'] < cut < p['end']), None)
            if index is None:
                continue
            current = pieces.pop(index)
            directive = next(((addr, line_index) for addr, _, line_index in _data_directives(current)
                              if addr == cut), None)
            if directive is None:
                # A C string can end immediately before scaffold alignment
                # padding. Permit that exact boundary while leaving the .align
                # directive (and its padding) in the scaffold suffix.
                for line_index, line in enumerate(current['block']['lines']):
                    align_line = re.sub(r'^/\*\s*[0-9A-Fa-f]{8}\s*\*/\s*', '', line).strip()
                    if not ALIGN.match(align_line):
                        continue
                    preceding = [(addr, index, current['block']['lines'][index])
                                 for addr, _, index in _data_directives(current) if index < line_index]
                    if not preceding:
                        continue
                    data_addr, data_index, data_line = preceding[-1]
                    asciz = re.search(r'\*/\s*\.asciz\s+("(?:[^"\\]|\\.)*")\s*$', data_line)
                    if not asciz:
                        continue
                    try:
                        literal_size = len(ast.literal_eval(asciz.group(1)).encode('latin1')) + 1
                    except (SyntaxError, ValueError, UnicodeEncodeError):
                        continue
                    if data_addr + literal_size == cut:
                        directive = (cut, line_index)
                        break
            if directive is None:
                # Splat can combine a final halfword and zero alignment bytes
                # into one numeric .word. Separate only that proven zero tail
                # in the generated scaffold; retain every original byte and
                # leave the canonical scaffold file untouched.
                addressed = _data_directives(current)
                for addr, _, line_index in addressed:
                    line = current['block']['lines'][line_index]
                    word = re.search(r'\*/\s*\.word\s+(0x[0-9A-Fa-f]+|\d+)\s*$', line)
                    if (cut not in run_ends or not word or
                            not addr < cut < addr + 4):
                        continue
                    value = int(word.group(1), 0)
                    if not 0 <= value <= 0xffffffff:
                        continue
                    raw = value.to_bytes(4, 'little')
                    if any(raw[cut - addr:]):
                        continue
                    following = [(a, current['block']['lines'][i])
                                 for a, _, i in addressed if a > addr]
                    if any(not re.search(r'\*/\s*\.word\s+(?:0x0+|0)\s*$', text)
                           for _, text in following):
                        continue
                    prefix = ', '.join(f'0x{v:02X}' for v in raw[:cut - addr])
                    suffix = ', '.join('0x00' for _ in raw[cut - addr:])
                    changed = list(current['block']['lines'])
                    changed[line_index:line_index + 1] = [
                        f'    /* {addr:08X} */ .byte {prefix}',
                        f'    /* {cut:08X} */ .byte {suffix}']
                    current = dict(current, block=dict(current['block'], lines=changed))
                    directive = (cut, line_index + 1)
                    break
            if directive is None:
                raise CarveError(f"{tu_name}: initialized cut inside {item['name']} at {h8(cut)} "
                                 "is not an exact data-directive boundary")
            size = cut - item['start'] if cut in run_ends else None
            left, right = _split_item_at(current, cut, directive[1], symbol_size=size)
            pieces[index:index] = [left, right]
        out.extend(pieces)
    return out


def repiece(tu_name, pieces, runs, read_items, allow_space_cuts=False):
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
            overlaps = [(lo, hi) for lo, hi in runs if p['start'] < hi and lo < p['end']]
            if not (len(overlaps) == 1 and
                    (overlaps[0][0] == p['start'] or overlaps[0][1] == p['end'])):
                raise CarveError(f'{tu_name}: the import-era C run {h8(p["start"])}-{h8(p["end"])} is not inside '
                                 f'a declared run ({", ".join(f"{h8(lo)}-{h8(hi)}" for lo, hi in runs)})')
        cuts = sorted({b for r in runs for b in r if p['start'] < b < p['end']})
        bounds = [p['start']] + cuts + [p['end']]
        starts = {it['start'] for it in items}
        if allow_space_cuts and any(b not in starts for b in cuts):
            items = split_initialized_items(items, cuts, {hi for _, hi in runs}, tu_name)
            starts = {it['start'] for it in items}
        for b in cuts:
            if b not in starts:
                raise CarveError(f'{tu_name}: {h8(b)} is not an item boundary of {p["name"]}')
        for a, b in zip(bounds, bounds[1:]):
            sub = [it for it in items if a <= it['start'] < b]
            segments.append(dict(start=a, end=b, header=header, items=sub, piece=p,
                                 carved=bool(cuts)))
    # A scaffold tail can begin at a different phase than an alignment
    # requirement later in the section. If left as one input section, ELF
    # sh_addralign rounds the *start* of the tail and shifts the first item off
    # its original address. Split at the first safe item or directive boundary
    # that restores that phase. This also handles a single labeled item such as
    # `.byte` followed by `.short`: the byte stays in an align-1 prefix, while
    # the short starts in an align-2 suffix. Source data directives are retained
    # verbatim, and any named symbol extent stays on its prefix.
    aligned_segments = []
    pending_segments = list(segments)
    while pending_segments:
        seg = pending_segments.pop(0)
        if run_of(seg['start'], seg['end']) is not None or not seg['items']:
            aligned_segments.append(seg)
            continue
        powers = [int(m.group(1)) for it in seg['items']
                  for line in it['block']['lines']
                  if (m := re.match(r'\s*\.align\s+(\d+)\b', line))]
        directives = [(it, addr, align, line_index)
                      for it in seg['items']
                      for addr, align, line_index in _data_directives(it)]
        required = max([1 << max(powers, default=0)] + [align for _, _, align, _ in directives])
        if seg['start'] % required == 0:
            aligned_segments.append(seg)
            continue
        # Prefer a labeled item boundary. If one labeled item contains data
        # directives with increasing natural alignment, allow an intermediate
        # phase boundary (for example byte at +1, short at +2, word at +4).
        # Re-evaluate the suffix after every cut: a section beginning at the
        # short may still need a second split before the word.
        item_cuts = [(i, it['start']) for i, it in enumerate(seg['items'][1:], 1)
                     if it['start'] % required == 0]
        directive_cuts = [(it, addr, line_index) for it, addr, align, line_index in directives
                          if seg['start'] < addr < seg['end'] and align > 1 and
                          (addr % required == 0 or addr % align == 0)]
        item_cut = min(item_cuts, key=lambda pair: pair[1]) if item_cuts else None
        directive_cut = min(directive_cuts, key=lambda row: row[1]) if directive_cuts else None
        if item_cut is None and directive_cut is None:
            aligned_segments.append(seg)
            continue
        if item_cut is not None and (directive_cut is None or item_cut[1] <= directive_cut[1]):
            cut_at, cut = item_cut
            left_items, right_items = seg['items'][:cut_at], seg['items'][cut_at:]
        else:
            item, cut, line_index = directive_cut
            cut_at = seg['items'].index(item)
            left_items = seg['items'][:cut_at]
            right_items = seg['items'][cut_at:]
            left_item, right_item = _split_item_at(item, cut, line_index)
            left_items = left_items + [left_item]
            right_items = [right_item] + right_items[1:]
        pending_segments[0:0] = [
            dict(start=seg['start'], end=cut, header=seg['header'], items=left_items,
                 piece=seg['piece'], carved=True),
            dict(start=cut, end=seg['end'], header=seg['header'], items=right_items,
                 piece=seg['piece'], carved=True),
        ]
    segments = aligned_segments
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


PAD_MARK = 'Automatically generated and unreferenced pad'
ZERO_DIRECTIVE = re.compile(r'\*/\s*\.(byte|short|half|2byte|word|4byte|space|zero)\s+(.+?)\s*$')


def is_alignment_pad(item):
    """True for a spimdisasm alignment pad: marked unreferenced, zero bytes only.

    spimdisasm emits the zero bytes between two data items as an item of its
    own. The compiler that emitted the neighbouring items emits the same bytes
    as alignment, so such an item is not data of its own."""
    lines = item['block']['lines']
    if not any(PAD_MARK in line for line in lines) or len(item.get('labels') or []) > 1:
        return False
    for line in lines:
        if not ADDR.match(line):
            continue
        directive = ZERO_DIRECTIVE.search(line)
        if not directive:
            return False
        if directive.group(1) not in ('space', 'zero'):
            try:
                if any(int(value.strip(), 0) for value in directive.group(2).split(',')):
                    return False
            except ValueError:
                return False
    return True


def absorb_padding(tu_id, sec, runs, items):
    """The runs to build, each extended over the spimdisasm alignment pads after it.

    The registry declares the C-owned items; the pads that follow them stay
    outside every declared run, so the build links them as scaffold pieces and
    a TU whose data is all C still takes bytes from assembly. Each run here takes
    the pads that run from its end to the next run of the section, or to the end
    of the TU's section, when they are shorter than the alignment of that end.
    Runs are never merged: every run keeps its own input
    section pinned at its original address, so a C layout whose alignment differs
    from the original moves nothing. The pad bytes then come from the C object's
    own alignment when its slice reaches them, or else from linker fill (the pin
    of the next run or input, see ninja_main.py); mapcheck bounds that gap by the
    alignment of the run end, and the whole-file comparison stays the verdict.

    Runs with explicit input routing or uncredited spans, NOBITS sections and a
    section without readable items are returned unchanged. An extended run
    records its pads in `absorbed_padding` ([lo, hi]); the last run of the
    section also in `padding_tail`."""
    if (not runs or items is None or sec in ('.bss', '.sbss') or
            any(r.get('c_input_spans') or r.get('uncredited_scaffold_ranges') or
                r.get('uncredited_c_symbols') for r in runs)):
        return runs
    items = sorted(items, key=lambda it: it['start'])
    if not items:
        return runs
    runs = sorted((copy.deepcopy(r) for r in runs), key=lambda r: hx(r['range'][0]))
    limits = [hx(r['range'][0]) for r in runs[1:]] + [items[-1]['end']]
    for k, (run, limit) in enumerate(zip(runs, limits)):
        hi = hx(run['range'][1])
        pads = []
        for it in items:
            if it['start'] < hi:
                continue
            if it['start'] != (pads[-1]['end'] if pads else hi) or it['end'] > limit \
                    or not is_alignment_pad(it):
                break
            pads.append(it)
        if not pads:
            continue
        # Pads that stop short of the next run (a scaffold item follows)
        # stay scaffold: only a run directly followed by C or the TU end grows.
        if pads[-1]['end'] != limit:
            continue
        # Only alignment: zero bytes shorter than the alignment of their end
        # (the bound mapcheck applies to a C piece gap). A longer zero span may
        # be an unreferenced zero-filled object and stays scaffold.
        if limit - hi >= min(limit & -limit, 16):
            continue
        span = [run['range'][1], h8(limit)]
        run['range'] = [run['range'][0], h8(limit)]
        run['absorbed_padding'] = [span]
        if k == len(runs) - 1:
            run['padding_tail'] = span
    return runs


def check_one_run(tu_id, sec, runs):
    """(lo, hi) of a section with exactly one declared run (the overlay rule)."""
    return declared_runs(tu_id, sec, runs, several=False)[0]


def split_section_name(sec, k):
    """Input section of run k of a split section: `.rodata`, `.rodata.carve.1`, ..."""
    return sec if k == 0 else f'{sec}{SPLIT_SUFFIX}{k}'


def compiled_section_name(sec):
    """Input section emitted by standard `ld -r -d` common allocation."""
    return '.scommon' if sec == '.sbss' else sec


def raw_section_name(unit, sec):
    """Raw C section name; MAIN native NOBITS stays `.sbss` before allocation."""
    return sec if unit == 'main' and sec == '.sbss' else compiled_section_name(sec)


def base_section_name(name):
    """The section a split input section belongs to (`.rodata.carve.2` -> `.rodata`)."""
    return name.split(SPLIT_SUFFIX, 1)[0]


def split_sections(runs_by_section):
    """{section: piece count} for multi-run or explicitly routed input sections."""
    out = {}
    for sec, runs in sorted((runs_by_section or {}).items()):
        runs = runs or []
        parts = [part for run in runs for part in (run.get('c_input_spans') or [])]
        explicit = any(part.get('object_range') is not None for part in parts)
        if len(runs) > 1 or explicit:
            out[sec] = len(runs)
    return out


# ---------------------------------------------------------------------------
# MAIN: the TU manifest (tools/tu/ninja_main.py, tools/tu/mapcheck.py)
# ---------------------------------------------------------------------------

def main_piece_file(unit_dir, piece, sec):
    return Path(unit_dir) / (piece.get('file') or f'asm/main/data/{piece["name"]}.{sec[1:]}.s')


def main_section_items(unit_dir, t, sec):
    """[item] of one MAIN TU section from its manifest pieces, or None without one."""
    ts = (t.get('sections') or {}).get(sec)
    if ts is None:
        return None
    pieces = ts.get('pieces') or [dict(name=t['name'], start=ts['start'], end=ts['end'])]
    items = []
    for p in pieces:
        p = dict(p, start=hx(p['start']), end=hx(p['end']))
        try:
            items += piece_items(main_piece_file(unit_dir, p, sec), p['start'], p['end'])[1]
        except (OSError, CarveError):
            return None
    return items


def apply_main(root, unit_dir, manifest, registry=None, write=True):
    """(effective manifest, report) with every declared MAIN run carved.

    The manifest is not modified in place and never written back: the tracked
    `config/objects/main.objects.json` stays what splat was configured with.
    With nothing declared for MAIN the same manifest object is returned."""
    registry = load_registry(root) if registry is None else registry
    declared = unit_runs(registry, 'main')
    if not declared and not registry.get('common_tail'):
        return manifest, []
    m = copy.deepcopy(manifest)
    by_id = {t['id']: t for t in m['tus']}
    # A proved DATA object can correct the provisional TU attribution of an
    # initialized run.  Keep this exception private and narrow: the route must
    # name its scaffold owner, be a single explicit .data run, and occupy the
    # exact suffix of one original owner piece.  Split the scaffold prefix into
    # a generated private piece and transfer only the C-owned suffix to the
    # actual source TU; frozen source maps and contracts are never changed.
    for tu_id, tu_secs in declared.items():
        target = by_id.get(tu_id)
        if target is None:
            continue
        for sec, runs in tu_secs.items():
            if sec in target.get('sections', {}):
                continue
            if sec != '.data' or len(runs) != 1:
                continue
            run = runs[0]
            correction = (run.get('basis') or {}).get('owner_correction') or {}
            inputs = run.get('c_input_spans') or []
            if correction.get('from_tu') is None or len(inputs) != 1 or \
                    inputs[0].get('section') != '.data' or inputs[0].get('object_range') is None:
                continue
            lo, hi = (hx(v) for v in run['range'])
            declared_owner = correction['from_tu']
            sec_order = m.get('sections', {}).get(sec, {}).get('order') or []
            matches = [i for i, piece in enumerate(sec_order)
                       if piece['tu'] == declared_owner and hx(piece['start']) <= lo < hi == hx(piece['end'])]
            if len(matches) != 1:
                raise CarveError(f'{tu_id} {sec}: owner correction does not identify one exact scaffold suffix')
            index = matches[0]
            old_order_piece = sec_order[index]
            owner_tu = next((x for x in m['tus'] if x['name'] == declared_owner), None)
            if owner_tu is None or sec not in owner_tu.get('sections', {}):
                raise CarveError(f'{tu_id} {sec}: owner correction source TU is absent from the manifest')
            owner_section = owner_tu['sections'][sec]
            owner_pieces = owner_section.get('pieces') or []
            owner_piece_matches = [i for i, p in enumerate(owner_pieces)
                                   if p['name'] == old_order_piece['piece'] and
                                   hx(p['start']) <= lo < hi == hx(p['end'])]
            if len(owner_piece_matches) != 1:
                raise CarveError(f'{tu_id} {sec}: owner correction suffix has no unique source piece')
            piece_index = owner_piece_matches[0]
            source_piece = owner_pieces[piece_index]
            if lo <= hx(source_piece['start']):
                raise CarveError(f'{tu_id} {sec}: owner correction must retain a scaffold prefix')
            source_file = main_piece_file(unit_dir, source_piece, sec)
            header, source_items = piece_items(source_file, hx(source_piece['start']), hx(source_piece['end']))
            if lo not in {item['start'] for item in source_items}:
                raise CarveError(f'{tu_id} {sec}: owner correction start {h8(lo)} is not a scaffold item boundary')
            prefix_items = [item for item in source_items if item['start'] < lo]
            suffix_items = [item for item in source_items if item['start'] >= lo]
            rel = f'{CARVE_DIR}/main/{declared_owner}__{hx(source_piece["start"]):08X}.{sec[1:]}.s'
            if write:
                write_piece_file(Path(unit_dir) / rel, header,
                                 [dict(item, lines=list(item['block']['lines'])) for item in prefix_items])
            moved_rel = f'{CARVE_DIR}/main/{target["name"]}__{lo:08X}.{sec[1:]}.s'
            if write:
                write_piece_file(Path(unit_dir) / moved_rel, header,
                                 [dict(item, lines=list(item['block']['lines'])) for item in suffix_items])
            prefix_name = f'{CARVE_DIR}/{declared_owner}__{hx(source_piece["start"]):08X}'
            prefix = dict(source_piece, name=prefix_name, end=h8(lo), file=rel, c_split=False)
            owner_pieces[piece_index] = prefix
            owner_section['pieces'] = owner_pieces
            owner_section['end'] = h8(lo)
            sec_order[index:index + 1] = [dict(old_order_piece, piece=prefix_name, end=h8(lo),
                                               file=rel, c_split=False)]
            moved = dict(source_piece, name=source_piece['name'], start=h8(lo), end=h8(hi),
                         file=moved_rel,
                         c_split=False)
            target['sections'][sec] = dict(start=h8(lo), end=h8(hi), rule='private owner correction',
                                           evidence_source='explicit original-object route', pieces=[moved])
            sec_order.insert(index + 1, dict(c_split=False, end=h8(hi), ordinal=target['ordinal'],
                                             piece=moved['name'], start=h8(lo), tu=target['name'],
                                             **({'file': moved['file']} if moved.get('file') else {})))
    report = []
    # The split object is keyed by raw compiler input section, not by the
    # original scaffold family.  Precompute the same object-offset order used
    # by split_plans so a raw .data slice can be placed into an original
    # .rodata run without relabeling either coordinate system.
    explicit_input_ordinals = {}
    input_parts = {}
    for tu_id, tu_secs in declared.items():
        for target_sec, target_runs in tu_secs.items():
            if target_sec in ('.bss', '.sbss'):
                continue
            for run_index, run in enumerate(target_runs):
                for part_index, part in enumerate(run.get('c_input_spans') or []):
                    input_sec = base_section_name(part.get('section', ''))
                    obj_range = part.get('object_range')
                    if obj_range is None or len(obj_range) != 2:
                        continue
                    input_parts.setdefault((tu_id, input_sec), []).append(
                        (int(obj_range[0]), int(obj_range[1]), target_sec, run_index, part_index))
    for (tu_id, input_sec), parts in input_parts.items():
        for ordinal, (_, _, target_sec, run_index, part_index) in enumerate(sorted(parts)):
            explicit_input_ordinals[(tu_id, target_sec, run_index, part_index)] = ordinal
    for tu_id, secs in declared.items():
        t = by_id.get(tu_id)
        if t is None:
            raise CarveError(f'{REGISTRY}: {tu_id} is not a TU of the MAIN manifest')
        secs = {sec: absorb_padding(tu_id, sec, runs, main_section_items(unit_dir, t, sec))
                for sec, runs in secs.items()}
        split = split_sections(secs)
        if split:
            # several runs in a section: the TU links its split object (`split`)
            t['c_link_object'] = f'build/c/{t["name"]}{SPLIT_OBJECT}'
            t['c_split_sections'] = split
        for sec, runs in sorted(secs.items()):
            spans = declared_runs(tu_id, sec, runs)
            nobits_input_names = {}
            if sec in ('.sbss', '.bss') and any(
                    part.get('object_range') is not None
                    for run in runs for part in run.get('c_input_spans') or []):
                ordered_inputs = {}
                for ri, run in enumerate(runs):
                    parts = run.get('c_input_spans') or []
                    if not parts or any(part.get('object_range') is None for part in parts):
                        raise CarveError(f'{tu_id} {sec}: multiple NOBITS runs require explicit object_range spans')
                    for pi, part in enumerate(parts):
                        base = base_section_name(part['section'])
                        ordered_inputs.setdefault(base, []).append(
                            (int(part['object_range'][0]), ri, pi))
                for base, inputs in ordered_inputs.items():
                    for index, (_, ri, pi) in enumerate(sorted(inputs)):
                        nobits_input_names[(ri, pi)] = split_section_name(base, index)
            if sec not in t['sections']:
                raise CarveError(f'{tu_id} has no {sec} piece')
            ts = t['sections'][sec]
            pieces = [dict(name=p['name'], start=hx(p['start']), end=hx(p['end']), c_split=p.get('c_split', False),
                           file=p.get('file'))
                      for p in ts.get('pieces') or [dict(name=t['name'], start=ts['start'], end=ts['end'])]]

            def read(p, sec=sec):
                return piece_items(main_piece_file(unit_dir, p, sec), p['start'], p['end'])

            new = repiece(t['name'], pieces, spans, read,
                          allow_space_cuts=(sec in ('.sbss', '.bss') or
                                            any(run.get('c_input_spans') for run in runs)))
            out_pieces = []
            for p in new:
                entry = dict(c_split=p['c_split'], end=f'0x{p["end"]:08X}', name=p['name'],
                             start=f'0x{p["start"]:08X}')
                if p['c_split'] and runs[p['run']].get('padding_tail'):
                    # the next input is pinned: these pad bytes are linker fill
                    entry['padding_tail'] = runs[p['run']]['padding_tail']
                if p['c_split'] and sec in split and sec not in ('.sbss', '.bss'):
                    run = runs[p['run']]
                    input_spans = run.get('c_input_spans') or []
                    if input_spans and input_spans[0].get('object_range') is not None:
                        lo, hi = spans[p['run']]
                        cursor = lo
                        normalized_inputs = []
                        for part_index, part in enumerate(input_spans):
                            part_lo, part_hi = (hx(v) for v in part['range'])
                            ordinal = explicit_input_ordinals.get((tu_id, sec, p['run'], part_index))
                            raw_sec = base_section_name(part.get('section', ''))
                            obj_range = [int(v) for v in part['object_range']]
                            if (ordinal is None or part_lo != cursor
                                    or part_hi <= part_lo or part_hi - part_lo != obj_range[1] - obj_range[0]):
                                raise CarveError(f'{tu_id} {sec}: explicit input subspans must exactly and contiguously cover the target run')
                            normalized_inputs.append(dict(section=split_section_name(raw_sec, ordinal),
                                range=[h8(part_lo), h8(part_hi)], object_range=obj_range,
                                symbols=sorted(set(part.get('symbols') or [])),
                                **({'anonymous_emission': True} if part.get('anonymous_emission') is True else {}),
                                **({'support_only': True} if part.get('support_only') is True else {}),
                                **({'credit': part['credit']} if part.get('credit') else {}),
                                **({'zero_padding_tail': int(part['zero_padding_tail'])} if part.get('zero_padding_tail') else {})))
                            cursor = part_hi
                        if cursor != hi:
                            raise CarveError(f'{tu_id} {sec}: explicit input subspans end at {h8(cursor)}, expected {h8(hi)}')
                        entry['c_section'] = normalized_inputs[0]['section']
                        entry['c_input_spans'] = normalized_inputs
                        lo, hi = spans[p['run']]
                        cursor = lo
                        normalized_inputs = []
                        for part_index, part in enumerate(input_spans):
                            part_lo, part_hi = (hx(v) for v in part['range'])
                            ordinal = explicit_input_ordinals.get((tu_id, sec, p['run'], part_index))
                            raw_sec = base_section_name(part.get('section', ''))
                            obj_range = [int(v) for v in part['object_range']]
                            if (ordinal is None or part_lo != cursor
                                    or part_hi <= part_lo or part_hi - part_lo != obj_range[1] - obj_range[0]):
                                raise CarveError(f'{tu_id} {sec}: explicit input subspans must exactly and contiguously cover the target run')
                            normalized_inputs.append(dict(section=split_section_name(raw_sec, ordinal),
                                range=[h8(part_lo), h8(part_hi)], object_range=obj_range,
                                symbols=sorted(set(part.get('symbols') or [])),
                                **({'anonymous_emission': True} if part.get('anonymous_emission') is True else {}),
                                **({'support_only': True} if part.get('support_only') is True else {}),
                                **({'credit': part['credit']} if part.get('credit') else {})))
                            cursor = part_hi
                        if cursor != hi:
                            raise CarveError(f'{tu_id} {sec}: explicit input subspans end at {h8(cursor)}, expected {h8(hi)}')
                        entry['c_section'] = normalized_inputs[0]['section']
                        entry['c_input_spans'] = normalized_inputs
                    else:
                        entry['c_section'] = split_section_name(sec, p['run'])
                    aliases = run.get('c_storage_aliases') or []
                    if aliases:
                        lo, hi = spans[p['run']]
                        c_aliases = [dict(
                            original_name=x['original_name'], address=h8(hx(x['address'])),
                            storage_owner=dict(name=x['storage_owner']['name'],
                                               address=h8(hx(x['storage_owner']['address'])),
                                               size=int(x['storage_owner']['size'])),
                            **({'storage_member': x['storage_member']} if x.get('storage_member') else {}))
                            for x in aliases]
                        owners = set(run.get('c_owned_symbols') or [])
                        for alias in c_aliases:
                            address = hx(alias['address'])
                            owner = alias['storage_owner']
                            owner_address, owner_size = hx(owner['address']), owner['size']
                            if (not alias['original_name'] or owner['name'] not in owners
                                    or owner_size <= 0 or not lo <= owner_address
                                    or owner_address + owner_size > hi
                                    or not owner_address <= address < owner_address + owner_size):
                                raise CarveError(
                                    f'{tu_id} {sec}: initialized c_storage_aliases must name an owner wholly inside the carved run')
                        entry['c_owned_symbols'] = sorted(owners)
                        entry['c_storage_aliases'] = c_aliases
                elif p['c_split'] and sec in ('.sbss', '.bss'):
                    run = runs[p['run']]
                    in_spans = run.get('c_input_spans')
                    aliases = run.get('c_storage_aliases')
                    if in_spans:
                        lo, hi = spans[p['run']]
                        cursor = lo
                        normalized = []
                        # MAIN original .bss owners can come from compiler .sbss
                        # or COMMON input when the selected C contract emits
                        # those storage classes; preserve the exact input name.
                        allowed = {'.bss', '.sbss', '.scommon'} if sec == '.bss' else {'.sbss', '.scommon'}
                        for pi, part in enumerate(in_spans):
                            part_lo, part_hi = hx(part['range'][0]), hx(part['range'][1])
                            base_input_section = base_section_name(part['section'])
                            if base_input_section not in allowed or part_lo != cursor or part_hi <= part_lo:
                                raise CarveError(
                                    f'{tu_id} {sec}: c_input_spans must be ordered, nonempty, and cover the run')
                            part_symbols = sorted(set(part.get('symbols') or []))
                            if not part_symbols or not set(part_symbols) <= set(run.get('c_owned_symbols') or []):
                                raise CarveError(f'{tu_id} {sec}: each c_input_spans entry needs named C owners')
                            cpart = dict(section=nobits_input_names.get((p['run'], pi), part['section']),
                                         range=[h8(part_lo), h8(part_hi)], symbols=part_symbols)
                            if part.get('object_range') is not None:
                                cpart['object_range'] = [int(v) for v in part['object_range']]
                            normalized.append(cpart)
                            cursor = part_hi
                        if cursor != hi:
                            raise CarveError(f'{tu_id} {sec}: c_input_spans end at {h8(cursor)}, expected {h8(hi)}')
                        entry['c_input_spans'] = normalized
                        entry['c_owned_symbols'] = sorted(run.get('c_owned_symbols') or [])
                        if aliases:
                            c_aliases = [dict(
                                original_name=x['original_name'], address=h8(hx(x['address'])),
                                storage_owner=dict(name=x['storage_owner']['name'],
                                                   address=h8(hx(x['storage_owner']['address'])),
                                                   size=int(x['storage_owner']['size']))) for x in aliases]
                            for alias in c_aliases:
                                address = hx(alias['address'])
                                owner = alias['storage_owner']
                                owner_address, owner_size = hx(owner['address']), owner['size']
                                owner_part = next((x for x in normalized if owner['name'] in x['symbols']), None)
                                if (not alias['original_name'] or not owner['name'] or owner_size <= 0
                                        or not owner_part or not hx(owner_part['range'][0]) <= owner_address
                                        or owner_address + owner_size > hx(owner_part['range'][1])
                                        or not owner_address <= address < owner_address + owner_size):
                                    raise CarveError(f'{tu_id} {sec}: c_storage_aliases must bind to an exact named owner span')
                            entry['c_storage_aliases'] = c_aliases
                    else:
                        entry['c_section'] = raw_section_name('main', sec)
                elif p['c_split'] and sec not in ('.sbss', '.bss'):
                    run = runs[p['run']]
                    aliases = run.get('c_storage_aliases') or []
                    if aliases:
                        lo, hi = spans[p['run']]
                        c_aliases = [dict(
                            original_name=x['original_name'], address=h8(hx(x['address'])),
                            storage_owner=dict(name=x['storage_owner']['name'],
                                               address=h8(hx(x['storage_owner']['address'])),
                                               size=int(x['storage_owner']['size'])),
                            **({'storage_member': x['storage_member']} if x.get('storage_member') else {}))
                            for x in aliases]
                        owners = set(run.get('c_owned_symbols') or [])
                        for alias in c_aliases:
                            address = hx(alias['address'])
                            owner = alias['storage_owner']
                            owner_address, owner_size = hx(owner['address']), owner['size']
                            if (not alias['original_name'] or owner['name'] not in owners
                                    or owner_size <= 0 or not lo <= owner_address
                                    or owner_address + owner_size > hi
                                    or not owner_address <= address < owner_address + owner_size):
                                raise CarveError(
                                    f'{tu_id} {sec}: initialized c_storage_aliases must name an owner wholly inside the carved run')
                        entry['c_owned_symbols'] = sorted(owners)
                        entry['c_storage_aliases'] = c_aliases
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
                         **({'c_section': p['c_section']} if p.get('c_section') else {}),
                         **({'c_input_spans': p['c_input_spans']} if p.get('c_input_spans') else {}),
                         **({'c_owned_symbols': p['c_owned_symbols']} if p.get('c_owned_symbols') else {}),
                         **({'c_storage_aliases': p['c_storage_aliases']}
                            if p.get('c_storage_aliases') else {}),
                         **({'padding_tail': p['padding_tail']} if p.get('padding_tail') else {}))
                    for p in out_pieces]
            order[idx[0]:idx[-1] + 1] = repl
            report.append(dict(tu=tu_id, section=sec, run=[h8(spans[0][0]), h8(spans[-1][1])],
                               runs=[[h8(lo), h8(hi)] for lo, hi in spans],
                               pieces=[(p['name'], p['start'], p['end'], p['c_split']) for p in out_pieces]))

    # A compiler-owned tentative definition in the linker-allocated MAIN
    # COMMON tail must replace the scaffold bytes at that exact address. Keep
    # the surrounding generated scaffold items as independent inputs and place
    # the selected C object's NOBITS section between them. This prevents a
    # strong C definition from colliding with the monolithic common.bss.o.
    for sec, linker_name in (('.sbss', 'linker/scommon'), ('.bss', 'linker/common')):
        rows = []
        for row in registry.get('common_tail', {}).get(sec, []):
            if not row.get('c_input_span'):
                continue
            tu = by_id.get(row.get('tu'))
            if tu is None or tu.get('mode') != 'c':
                raise CarveError(f'{row.get("tu")} {sec}: common-tail owner has no compiled C TU')
            # Configure runs before compilation in a clean checkout. Plan the
            # substitution from tracked ownership, never from cached objects.
            # The carvesplit edge validates each fresh object's NOBITS section,
            # owner symbol, offset and extent in split_plans() before linking;
            # mapcheck and the whole-file comparison remain required afterward.
            rows.append(row)
        if not rows:
            continue
        tail = m['sections'][sec].get('common_tail')
        if not tail:
            raise CarveError(f'{sec}: common-tail rows exist without a manifest common tail')
        rows = sorted(rows, key=lambda r: hx(r['range'][0]))
        spans = []
        for row in rows:
            lo, hi = map(hx, row['range'])
            part = row['c_input_span']
            obj_range = part.get('object_range')
            owner = row.get('storage_owner') or {}
            allowed_input = {sec} if sec == '.bss' else {'.sbss', '.scommon'}
            if (part.get('section') not in allowed_input or obj_range is None or len(obj_range) != 2
                    or not owner.get('name') or hx(owner.get('address', '-1')) != lo
                    or int(owner.get('size', -1)) != hi - lo
                    or int(obj_range[0]) < 0
                    or int(obj_range[1]) - int(obj_range[0]) != hi - lo
                    or not (hx(tail['start']) <= lo < hi <= hx(tail['end']))):
                raise CarveError(f'{row.get("tu")} {sec}: common-tail input span is not an exact in-bounds NOBITS owner')
            if spans and lo < spans[-1][1]:
                raise CarveError(f'{sec}: common-tail C-owned spans overlap')
            spans.append((lo, hi))

        source = Path(unit_dir) / f'asm/main/data/{linker_name}.{sec[1:]}.s'
        header, all_items = piece_items(source, hx(tail['start']), hx(tail['end']))
        common_input_parts = {}
        for row in rows:
            part = row['c_input_span']
            input_section = part.get('section', sec)
            common_input_parts.setdefault((row['tu'], input_section), []).append(
                (int(part['object_range'][0]), row))
        common_output_sections = {}
        for (owner_tu, input_section), ordered in common_input_parts.items():
            for ordinal, (offset, row) in enumerate(sorted(ordered, key=lambda item: item[0])):
                common_output_sections[(owner_tu, input_section, offset)] = split_section_name(input_section, ordinal)
            tu = by_id[owner_tu]
            tu['c_link_object'] = f'build/c/{tu["name"]}{SPLIT_OBJECT}'
            tu['c_common_tail_splits'] = True
            tu['common_tail_native'] = True
            tu.setdefault('c_split_sections', {})
        native = []
        cursor = hx(tail['start'])
        for index, (row, (lo, hi)) in enumerate(zip(rows, spans)):
            for a, b, kind in ((cursor, lo, 'scaffold'), (lo, hi, 'c')):
                if a == b:
                    continue
                if kind == 'c':
                    tu = by_id[row['tu']]
                    obj = tu.get('c_link_object') or f'build/c/{tu["name"]}.o'
                    part = row['c_input_span']
                    input_section = part.get('section', sec)
                    input_offset = int(part['object_range'][0])
                    native.append(dict(kind='c', name=row['storage_owner']['name'],
                                       start=h8(a), end=h8(b), object=obj,
                                       input_section=common_output_sections.get(
                                           (row['tu'], input_section, input_offset), input_section)))
                else:
                    item_slice = [item for item in all_items if a <= item['start'] < b]
                    if (not item_slice or item_slice[0]['start'] != a
                            or (item_slice[-1]['end'] != b and b != hx(tail['end']))):
                        raise CarveError(f'{sec}: common-tail scaffold split {h8(a)}-{h8(b)} is not on item boundaries')
                    stem = f'{linker_name.replace("/", "_")}__{a:08X}'
                    rel = f'{CARVE_DIR}/main/{stem}.{sec[1:]}.s'
                    if write:
                        write_piece_file(Path(unit_dir) / rel, header,
                                         [item for item in item_slice if item['end'] <= b])
                    native.append(dict(kind='scaffold', name=stem, start=h8(a), end=h8(b),
                                       object=f'build/data/{stem}.{sec[1:]}.o', source=rel))
            cursor = hi
        if cursor < hx(tail['end']):
            item_slice = [item for item in all_items if cursor <= item['start'] < hx(tail['end'])]
            if not item_slice or item_slice[0]['start'] != cursor:
                raise CarveError(f'{sec}: common-tail suffix does not start on an item boundary')
            stem = f'{linker_name.replace("/", "_")}__{cursor:08X}'
            rel = f'{CARVE_DIR}/main/{stem}.{sec[1:]}.s'
            if write:
                write_piece_file(Path(unit_dir) / rel, header, item_slice)
            native.append(dict(kind='scaffold', name=stem, start=h8(cursor),
                               end=h8(hx(tail['end'])), object=f'build/data/{stem}.{sec[1:]}.o',
                               source=rel))
        tail['native_common_pieces'] = native
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
    text = ''.join(out)
    text = re.sub(r'(build/(?:scaffold/)?src/[\w-]+/[\w-]+)\.carved\.o(?=\()', r'\1.o', text)
    return re.sub(r'(build/c/[\w-]+/[\w-]+)\.allocated\.carved\.o(?=\()',
                  r'\1.allocated.o', text)


def ovl_tu_names(root, unit):
    """{tu id: TU name} of an overlay (config/tu-build.json path stems)."""
    private = Path(root) / 'config/tu-build.json'
    if private.is_file():
        build = json.loads(private.read_bytes())
        return {t['id']: Path(t['path']).stem for t in build['tus'] if t['unit'] == unit}
    build = json.loads((Path(root) / 'config/objects/overlays.compile.json').read_bytes())
    names = {t['id']: t['name'] for t in build['units'][unit]['tus']}
    names.update({t['id'].split('_')[0]: t['name'] for t in build['units'][unit]['tus']})
    return names


def ovl_piece(unit_dir, unit, name, sec='.rodata'):
    layout = json.loads((Path(unit_dir) / 'layout.json').read_bytes())
    fam = {x[0]: (hx(x[1]), hx(x[2])) for x in layout['families'].get(sec, [])}
    key = f'{unit}/{name}'
    if key not in fam:
        return None
    start, end = fam[key]
    return dict(name=f'{unit}/{name}', start=start, end=end, c_split=False,
                file=f'asm/data/{unit}/{name}{sec}.s')


def overlay_section_items(unit_dir, unit, name, sec):
    """[item] of one overlay TU section from its layout piece, or None without one."""
    if sec not in OVERLAY_SECTIONS:
        return None
    p = ovl_piece(unit_dir, unit, name, sec)
    if p is None:
        return None
    try:
        return piece_items(Path(unit_dir) / p['file'], p['start'], p['end'])[1]
    except (OSError, CarveError):
        return None


def apply_overlay(root, unit_dir, unit, ld_text, registry=None, write=True,
                  c_object_overrides=None):
    """(linker script, report) with every declared run of this overlay carved.

    The TU's splat data line is kept as a comment and replaced by: the scaffold
    piece before the run, the C object's section, a pin to the run end, the
    scaffold piece after it.  Idempotent: an earlier carve is undone first, so a
    script without declared runs is exactly splat's."""
    text = ovl_restore(ld_text)
    c_object_overrides = c_object_overrides or {}
    registry = load_registry(root) if registry is None else registry
    declared = unit_runs(registry, unit)
    if not declared:
        return text, []
    names = ovl_tu_names(root, unit)
    base = re.search(r'^\s*\.' + re.escape(unit) + r' 0x([0-9A-Fa-f]+)\s*:', text, re.M)
    if not base:
        raise CarveError(f'{unit}.ld: no .{unit} output section')
    base = int(base.group(1), 16)
    layout = json.loads((Path(unit_dir) / 'layout.json').read_bytes())
    bss_families = layout.get('families', {})
    bss_starts = [hx(row[1]) for sec in ('.bss', '.sbss')
                  for row in bss_families.get(sec, [])]
    bss_base = min(bss_starts) if bss_starts else base
    report = []
    explicit_input_ordinals = {}
    input_parts = {}
    for tu_id, tu_secs in declared.items():
        for target_sec, target_runs in tu_secs.items():
            if target_sec in ('.bss', '.sbss'):
                continue
            for run_index, run in enumerate(target_runs):
                for part_index, part in enumerate(run.get('c_input_spans') or []):
                    input_sec = base_section_name(part.get('section', ''))
                    obj_range = part.get('object_range')
                    if obj_range is None or len(obj_range) != 2:
                        continue
                    input_parts.setdefault((tu_id, input_sec), []).append(
                        (int(obj_range[0]), int(obj_range[1]), target_sec, run_index, part_index))
    for (tu_id, input_sec), parts in input_parts.items():
        for ordinal, (_, _, target_sec, run_index, part_index) in enumerate(sorted(parts)):
            explicit_input_ordinals[(tu_id, target_sec, run_index, part_index)] = ordinal
    for tu_id, secs in declared.items():
        name = names.get(tu_id)
        if name is None:
            raise CarveError(f'{REGISTRY}: {tu_id} is not a TU of {unit}')
        text_line = re.search(r'^(\s*)(build/(?:scaffold/)?src/' + re.escape(unit) + '/'
                              + re.escape(name) + r'\.o)\(\.text\);\s*$', text, re.M)
        allocated_line = re.search(r'^(\s*)(build/c/' + re.escape(unit) + '/'
                                   + re.escape(name) + r'\.allocated\.o)\(\.text\);\s*$', text, re.M)
        if not text_line and not allocated_line:
            raise CarveError(f'{tu_id}: {unit}.ld links no C object for {name} (is the TU C?)')
        c_obj = text_line.group(2) if text_line else allocated_line.group(2)
        secs = {sec: absorb_padding(tu_id, sec, runs, overlay_section_items(unit_dir, unit, name, sec))
                for sec, runs in secs.items()}
        # Every section of this TU must use the same object. A preceding
        # multi-run carve may already have replaced the .text object below.
        allocated_obj = c_object_overrides.get(c_obj, c_obj)
        linked_obj = (allocated_obj[:-2] + SPLIT_OBJECT
                      if split_sections(secs) else allocated_obj)
        for sec, runs in sorted(secs.items()):
            # Overlay BSS is eligible only through the guarded allocation path
            # supplied by ninja_ovl.py. Its record and allocated object are
            # checked there; this path only substitutes C for the exact
            # zero-filled scaffold run.
            if sec not in OVERLAY_SECTIONS + ('.bss',):
                raise CarveError(f'{tu_id}: an overlay carve covers {OVERLAY_SECTIONS} plus guarded .bss only (declared {sec})')
            spans = declared_runs(tu_id, sec, runs)
            alias_rows = []
            if sec not in ('.bss', '.sbss'):
                for run_index, run in enumerate(runs):
                    owners = set(run.get('c_owned_symbols') or [])
                    lo, hi = spans[run_index]
                    for alias in run.get('c_storage_aliases') or []:
                        owner = alias.get('storage_owner') or {}
                        address = hx(alias.get('address', '0'))
                        owner_address = hx(owner.get('address', '0'))
                        owner_size = int(owner.get('size', 0))
                        owner_name = owner.get('name')
                        if (not alias.get('original_name') or not owner_name
                                or owner_name not in owners or owner_size <= 0
                                or not lo <= owner_address or owner_address + owner_size > hi
                                or not owner_address <= address < owner_address + owner_size):
                            raise CarveError(
                                f'{tu_id} {sec}: initialized c_storage_aliases must name an owner wholly inside the carved run')
                        alias_rows.append(dict(section=sec, original_name=alias['original_name'],
                                               address=h8(address), storage_owner=dict(
                                                   name=owner_name, address=h8(owner_address), size=owner_size),
                                               run=[h8(lo), h8(hi)]))
            piece = ovl_piece(unit_dir, unit, name, sec)
            if piece is None:
                raise CarveError(f'{tu_id} has no {sec} piece in {unit}/layout.json')
            if re.search(re.escape(c_obj) + r'\(' + re.escape(sec) + r'\)', text):
                raise CarveError(f'{tu_id}: {unit}.ld already places {c_obj}({sec})')
            data_obj = f'build/asm/data/{unit}/{name}{sec}.o'
            line = re.search(r'^(\s*)' + re.escape(data_obj) + r'\(' + re.escape(sec) + r'\);[^\n]*\n', text, re.M)
            if not line:
                raise CarveError(f'{tu_id}: {unit}.ld does not place {data_obj}({sec})')

            def read(p):
                return piece_items(Path(unit_dir) / p['file'], p['start'], p['end'])

            # Explicit initialized input spans are checked against the raw
            # object and original bytes before this point.  They may end at a
            # directive boundary inside one scaffold symbol (for example a
            # 2-byte C owner followed by a 12-byte scaffold-only suffix), so
            # let repiece preserve that boundary just as it does for BSS.
            # Without an explicit span, initialized scaffold items remain
            # indivisible and continue to fail closed at interior cuts.
            new = repiece(name, [piece], spans, read,
                          allow_space_cuts=(sec == '.bss' or
                                            any(run.get('c_input_spans') for run in runs)))
            bss_input_sections = {}
            if sec in ('.bss', '.sbss') and any(run.get('c_input_spans') for run in runs):
                # A single compiler .bss can contain several named owners whose
                # original VA order differs from their compiler-object order.
                # split_plans names each NOBITS slice by object offset; map each
                # original-address span back to that exact input section here.
                inputs_by_base = {}
                for run_index, run in enumerate(runs):
                    for part_index, part in enumerate(run.get('c_input_spans') or []):
                        if part.get('object_range') is None:
                            raise CarveError(
                                f'{tu_id} .bss: each split owner span needs an explicit object_range')
                        input_base = base_section_name(part['section'])
                        object_offset, object_end = (int(v) for v in part['object_range'])
                        if object_offset < 0 or object_end <= object_offset:
                            raise CarveError(f'{tu_id} .bss: invalid owner object_range')
                        inputs_by_base.setdefault(input_base, []).append(
                            (object_offset, object_end, run_index, part_index))
                for input_base, inputs in inputs_by_base.items():
                    ordered = sorted(inputs)
                    if len({(lo, hi) for lo, hi, _, _ in ordered}) != len(ordered):
                        raise CarveError(f'{tu_id} .bss: duplicate owner object ranges for {input_base}')
                    for ordinal, (_, _, run_index, part_index) in enumerate(ordered):
                        bss_input_sections[(run_index, part_index)] = split_section_name(input_base, ordinal)
            indent = line.group(1)
            out = [f'{indent}{LD_ORIG}{data_obj}[{sec}] */\n']
            for p in new:
                if p['c_split']:
                    if sec in ('.bss', '.sbss') and runs[p['run']].get('c_input_spans'):
                        run_index = p['run']
                        cursor = p['start']
                        for part_index, part in enumerate(runs[run_index].get('c_input_spans') or []):
                            part_start, part_end = (hx(v) for v in part['range'])
                            input_sec = bss_input_sections.get((run_index, part_index))
                            if input_sec is None or part_start != cursor or part_end <= part_start:
                                raise CarveError(
                                    f'{tu_id} .bss: owner spans do not exactly cover run {h8(p["start"])}..{h8(p["end"])}')
                            out.append(f'{indent}{linked_obj}({input_sec}); {LD_MARK}\n')
                            pin_base = bss_base if sec in ('.bss', '.sbss') else base
                            out.append(f'{indent}. = 0x{part_end - pin_base:X}; {LD_MARK} '
                                       f'/* pin: end of {part.get("symbols", ["C owner"])[0]} */\n')
                            cursor = part_end
                        if cursor != p['end']:
                            raise CarveError(
                                f'{tu_id} .bss: owner spans end at {h8(cursor)}, expected {h8(p["end"])}')
                        continue
                    input_sec = sec
                    if sec not in ('.bss', '.sbss'):
                        target_run = runs[p['run']]
                        input_spans = target_run.get('c_input_spans') or []
                        if input_spans:
                            cursor = p['start']
                            for part_index, part in enumerate(input_spans):
                                part_start, part_end = (hx(v) for v in part['range'])
                                raw_sec = base_section_name(part['section'])
                                ordinal = explicit_input_ordinals.get((tu_id, sec, p['run'], part_index))
                                if (ordinal is None or part_start != cursor
                                        or part_end <= part_start):
                                    raise CarveError(f'{tu_id} {sec}: initialized input subspans must contiguously cover their target run')
                                input_sec = split_section_name(raw_sec, ordinal) if ordinal else raw_sec
                                out.append(f'{indent}{linked_obj}({input_sec}); {LD_MARK}\n')
                                out.append(f'{indent}. = 0x{part_end - base:X}; {LD_MARK} /* pin: end of initialized C subspan */\n')
                                cursor = part_end
                            if cursor != p['end']:
                                raise CarveError(f'{tu_id} {sec}: initialized input subspans end at {h8(cursor)}, expected {h8(p["end"])}')
                            continue
                        else:
                            input_sec = split_section_name(sec, p['run']) if len(spans) > 1 else sec
                    out.append(f'{indent}{linked_obj}({input_sec}); {LD_MARK}\n')
                    pin_base = bss_base if sec in ('.bss', '.sbss') else base
                    out.append(f'{indent}. = 0x{p["end"] - pin_base:X}; {LD_MARK} /* pin: end of the C run of {tu_id} */\n')
                    continue
                stem = p['name'].split('/')[-1]
                rel = f'{CARVE_DIR}/{unit}/{stem}{sec}'
                if write:
                    write_piece_file(Path(unit_dir) / f'{rel}.s', p['header'], p['items'])
                out.append(f'{indent}build/{rel}.o({sec}); {LD_MARK}\n')
            text = text[:line.start()] + ''.join(out) + text[line.end():]
            if linked_obj != c_obj:
                text = text.replace(c_obj + '(', linked_obj + '(')
            if allocated_obj != c_obj:
                text = text.replace(allocated_obj + '(', linked_obj + '(')
            report.append(dict(tu=tu_id, section=sec, run=[h8(spans[0][0]), h8(spans[-1][1])],
                               runs=[[h8(lo), h8(hi)] for lo, hi in spans], c_object=c_obj,
                               allocated_object=allocated_obj if allocated_obj != c_obj else None,
                               link_object=linked_obj,
                               **({'c_owned_symbols': sorted(set().union(*[
                                   set(r.get('c_owned_symbols') or []) for r in runs]))}
                                  if any(r.get('c_owned_symbols') for r in runs) else {}),
                               **({'c_storage_aliases': alias_rows} if alias_rows else {}),
                               **({'uncredited_scaffold_ranges': [x for r in runs
                                   for x in r.get('uncredited_scaffold_ranges', [])]}
                                  if any(r.get('uncredited_scaffold_ranges') for r in runs) else {}),
                               **({'c_input_spans': [x for r in runs for x in r.get('c_input_spans', [])]}
                                  if any(r.get('c_input_spans') for r in runs) else {}),
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
        for sec in OVERLAY_SECTIONS:
            p = ovl_piece(unit_dir, unit, name, sec)
            if p:
                pieces[sec] = [p]
        ld = Path(unit_dir) / f'{unit}.ld'
        if ld.is_file():
            raw_text = ld.read_text()
            text = ovl_restore(raw_text)
            obj = re.search(r'(build/(?:scaffold/)?src/' + re.escape(unit) + '/' + re.escape(name)
                            + r'\.o)\(\.text\);', text)
            allocated_carved = re.search(r'(build/c/' + re.escape(unit) + '/' + re.escape(name)
                                         + r'\.allocated\.carved\.o)\(\.text\);', raw_text)
            allocated = re.search(r'(build/c/' + re.escape(unit) + '/' + re.escape(name)
                                  + r'\.allocated\.o)\(\.text\);', raw_text)
            for sec in OVERLAY_SECTIONS:
                allocated_sections = (sec, '.scommon') if sec == '.sbss' else (sec,)
                if ((obj and f'{obj.group(1)}({sec});' in text) or
                        (allocated_carved and any(f'{allocated_carved.group(1)}({s});' in raw_text
                                                  for s in allocated_sections)) or
                        (allocated and any(f'{allocated.group(1)}({s});' in raw_text
                                           for s in allocated_sections))):
                    placed.add(sec)            # splat already links this C section
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
    raw_text = ld.read_text()
    allocated_carved = re.search(r'(build/c/' + re.escape(unit) + '/' + re.escape(name)
                                 + r'\.allocated\.carved\.o)\(\.[\w.]+\);', raw_text)
    if allocated_carved and (unit_dir / allocated_carved.group(1)).is_file():
        return unit_dir / allocated_carved.group(1)
    allocated = re.search(r'(build/c/' + re.escape(unit) + '/' + re.escape(name)
                          + r'\.allocated\.o)\(\.[\w.]+\);', raw_text)
    if allocated and (unit_dir / allocated.group(1)).is_file():
        return unit_dir / allocated.group(1)
    m = re.search(r'(build/(?:scaffold/)?src/' + re.escape(unit) + '/' + re.escape(name) + r'\.o)\(\.text\);',
                  ovl_restore(raw_text))
    return unit_dir / m.group(1) if m and (unit_dir / m.group(1)).is_file() else None


def overlay_cc1_sidecar(unit_dir, unit, name):
    """Pinned raw cc1 provenance for an overlay allocation, if present."""
    unit_dir = Path(unit_dir)
    alloc = (unit_dir / f'build/c/{unit}/{name}.allocated.o').resolve()
    report_path = alloc.with_name(alloc.name + '.report.json')
    if not alloc.is_file() or not report_path.is_file():
        return None
    try:
        report = json.loads(report_path.read_bytes())
    except (OSError, json.JSONDecodeError):
        return None

    def digest(path):
        h = hashlib.sha256()
        with Path(path).open('rb') as stream:
            for block in iter(lambda: stream.read(1024 * 1024), b''):
                h.update(block)
        return h.hexdigest()

    expected_alloc = report.get('output', {}).get('sha256')
    if not expected_alloc or digest(alloc) != expected_alloc:
        return None
    rows = [loc.get('evidence', {}) for loc in report.get('recovered_location_candidates', [])
            if loc.get('evidence', {}).get('link_allocation_path') == str(alloc)
            and loc.get('evidence', {}).get('assembly_path')
            and loc.get('evidence', {}).get('assembly_sha256')]
    if not rows:
        return None
    pairs = {(row['assembly_path'], row['assembly_sha256']) for row in rows}
    if len(pairs) != 1:
        return None
    assembly_path, assembly_sha256 = next(iter(pairs))
    assembly = Path(assembly_path)
    if not assembly.is_file() or digest(assembly) != assembly_sha256:
        return None
    return dict(allocation_report=str(report_path), allocation_object=str(alloc),
                allocation_sha256=expected_alloc, raw_cc1_assembly=str(assembly),
                raw_cc1_assembly_sha256=assembly_sha256,
                raw_cc1_object=rows[0].get('object_path'),
                raw_cc1_object_sha256=rows[0].get('object_sha256'))


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


def run_specs(items, spans, run_records=None):
    """[dict(lo, hi, last, first_end)] of declared runs over the section's items."""
    records_by_lo = {hx(record['range'][0]): record for record in (run_records or [])}
    out = []
    for lo, hi in spans:
        its = [it for it in items if lo <= it['start'] < hi]
        if not its or its[0]['start'] != lo:
            raise CarveError(f'the run {h8(lo)}-{h8(hi)} does not start at an item')
        record = records_by_lo.get(lo) or {}
        # Alignment pads a run absorbed (absorb_padding) are not items the C
        # object must reach; its slice may end where they begin.
        padding = [(hx(a), hx(b)) for a, b in record.get('absorbed_padding') or []]
        its = [it for it in its if not any(a <= it['start'] < b for a, b in padding)] or its
        out.append(dict(lo=lo, hi=hi, last=its[-1]['start'], first_end=its[0]['end'],
                        content_hi=its[-1]['end'], first_name=its[0]['name'],
                        c_owned_symbols=record.get('c_owned_symbols') or [],
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


def delay_slot_hi(elf, text, data, entries, offset, symbol_index, base_register):
    """Prove the HI16 delivered by direct incoming branches to a LO16.

    A switch default can jump over its cases with a LUI in the branch's delay
    slot. Linear register liveness loses that LUI while scanning the skipped
    cases. Admit only matching incoming branches and a closed fallthrough path;
    retain rejection for unknown or divergent bases.
    """
    owners = [s for s in elf.symbols if s.type == 2 and s.shndx == text.index
              and s.value <= offset < s.value + s.size]
    if len(owners) != 1:
        return None
    start, end = owners[0].value, owners[0].value + owners[0].size
    relocations = {off: (kind, index) for off, kind, index, _ in entries}
    incoming, targets = [], set()
    for pc in range(start, end - 4, 4):
        word = struct.unpack_from('<I', data, pc)[0]
        opcode = word >> 26
        if opcode in (1, 4, 5, 6, 7, 20, 21, 22, 23) or (
                opcode in (17, 18) and (word >> 21) & 31 == 8):
            target = pc + 4 + 4 * _sext16(word)
            targets.add(target)
            if target == offset:
                delay = struct.unpack_from('<I', data, pc + 4)[0]
                if (delay >> 26 != 15 or (delay >> 16) & 31 != base_register
                        or relocations.get(pc + 4) != (5, symbol_index)):
                    return None
                incoming.append((delay & 0xffff) << 16)
    if not incoming or len(set(incoming)) != 1:
        return None
    # Native switch-table entries and direct jumps can also enter padding.
    # Treat every text relocation target as live, independent of reachability.
    for relsec in elf.sections:
        if relsec.type not in (4, 9) or relsec.info >= len(elf.sections):
            continue
        payload = elf.section_bytes(elf.sections[relsec.info])
        for at, kind, index, add in _rel_entries(elf, relsec):
            symbol = elf.symbols[index]
            if symbol.shndx != text.index or kind not in (2, 4):
                continue
            raw = struct.unpack_from('<I', payload, at)[0]
            value = add if add is not None else (raw if kind == 2 else (raw & 0x03ffffff) << 2)
            target = symbol.value + value
            if target == offset:
                return None             # another entry with an unproved GPR
            targets.add(target)
    # Require a direct unconditional transfer immediately before the target's
    # padding (at most four nop words), with no other entry into that padding.
    cursor = offset - 4
    padding = []
    while cursor >= start and struct.unpack_from('<I', data, cursor)[0] == 0 and len(padding) < 4:
        padding.append(cursor)
        cursor -= 4
    branch_at = cursor - 4
    if branch_at < start or set(padding) & targets:
        return None
    branch = struct.unpack_from('<I', data, branch_at)[0]
    opcode = branch >> 26
    if opcode != 2 and not (opcode == 4 and (branch >> 21) & 31 == (branch >> 16) & 31):
        return None
    if cursor in targets:
        return None
    return incoming[0]


def section_references(elf, csec, allow_negative_base=False):
    """[ref] of every relocation of the object that addresses `csec`.

    ref = dict(rel, entry, offset, type, sym, target): `target` is the offset in
    `csec` the relocation reaches (symbol value + addend).  An in-place (REL)
    addend is decoded as the MIPS rules pair it: R_MIPS_HI16 with the next
    R_MIPS_LO16 against the same symbol; an unpaired R_MIPS_LO16 carries the low
    half, which names one offset of a section smaller than 64 KiB, or reuses a
    previously linked HI16 base when the emitted instruction addresses the
    same symbol through the same still-live GPR."""
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
        reusable_hi = []
        reusable_lo = {
            entry[0]: entry[2]
            for entry in entries
            if entry[1] == 6
        }

        def writes_gpr(word, register):
            """Conservatively detect a GPR destination for ordinary MIPS ops."""
            opcode = word >> 26
            rs_field = (word >> 21) & 31
            rt_field = (word >> 16) & 31
            rd_field = (word >> 11) & 31
            if opcode == 0:
                funct = word & 63
                # Arithmetic/shift/move instructions write rd.  These special
                # functions only write control state or read/jump registers.
                return rd_field == register and funct not in (8, 12, 13, 16, 18, 24, 25, 26, 27)
            if opcode == 1:
                return register == 31 and ((rt_field >> 4) & 3) == 1
            if opcode == 3:
                return register == 31
            if 8 <= opcode <= 15:
                return rt_field == register
            if opcode in (16, 17, 18, 19):
                return rs_field in (0, 2) and rt_field == register
            if opcode in (32, 33, 34, 35, 36, 37, 38, 39, 48, 52, 55, 56, 59):
                return rt_field == register
            return False

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
                    # A hand-written sequence may use one `%hi(symbol)` result
                    # as a stable base for several later `%lo(symbol)` adds.
                    # Keep the fully resolved target and destination register
                    # so those later LO16 references can be attributed without
                    # guessing from the low 16 bits of a large section offset.
                    if word >> 26 == 0x0f:
                        # A LUI result can feed several LO16 relocations with
                        # different signed immediates.  Keep the unadjusted
                        # high half; each later LO contributes its own sign-
                        # extended low half below.
                        reusable_hi.append((off, (word >> 16) & 31, symi,
                                            (word & 0xffff) << 16))
                elif rtype == 6:                                 # R_MIPS_LO16
                    addend = paired.get(i)
                    if addend is None:
                        if csec.size >= 0x10000:
                            lo_base = (word >> 21) & 31
                            candidates = [h for h in reusable_hi
                                          if h[0] < off and h[2] == symi]
                            candidates.sort(reverse=True)
                            reused = None
                            for hi_off, hi_reg, _, hi_addend in candidates:
                                live_bases = {hi_reg}
                                for pc in range(hi_off + 4, off, 4):
                                    insn = struct.unpack_from('<I', tdata, pc)[0]
                                    opcode = insn >> 26
                                    rs_field = (insn >> 21) & 31
                                    rt_field = (insn >> 16) & 31
                                    rd_field = (insn >> 11) & 31
                                    funct = insn & 63
                                    # Preserve only register copies that are
                                    # explicit MIPS move aliases (addu/or/daddu
                                    # with $zero).  This covers cc1's saved
                                    # pointer copy while keeping arbitrary
                                    # arithmetic from widening the proof.
                                    move_alias = (opcode == 0 and funct in (0x21, 0x25, 0x2d)
                                                  and rt_field == 0 and rs_field in live_bases
                                                  and rd_field != 0)
                                    if move_alias:
                                        live_bases.add(rd_field)
                                        continue
                                    # A repeated LO16 against the same symbol
                                    # may update the same address register; it
                                    # does not invalidate the retained LUI base.
                                    repeated_lo = (
                                        reusable_lo.get(pc) == symi
                                        and insn >> 26 == 9
                                        and rs_field in live_bases
                                        and rt_field == rs_field
                                    )
                                    if repeated_lo:
                                        continue
                                    live_bases = {reg for reg in live_bases
                                                  if not writes_gpr(insn, reg)}
                                if lo_base in live_bases:
                                    reused = hi_addend + _sext16(word)
                                    break
                            if reused is None and tgt.name == '.text':
                                branch_hi = delay_slot_hi(elf, tgt, tdata, entries, off, symi, lo_base)
                                if branch_hi is not None:
                                    reused = branch_hi + _sext16(word)
                            if reused is None:
                                raise CarveError(f'unpaired R_MIPS_LO16 at {where}: {csec.name} is 64 KiB or larger')
                            addend = reused
                        else:
                            addend = _sext16(word)
                            if not 0 <= base + addend <= csec.size:
                                addend = word & 0xffff
                elif rtype in (7, 8):                            # R_MIPS_GPREL16, R_MIPS_LITERAL
                    addend = _sext16(word) + (gp_base if sym.bind == 0 else 0)
                else:
                    raise CarveError(f'relocation type {rtype} at {where} against {csec.name} is not supported '
                                     f'by the split')
            target = base + addend
            # GCC can bias a static array's section base below zero before
            # adding its runtime index. Preserve that native addend when the
            # splitter keeps the first native owner at section offset zero.
            negative_base = (allow_negative_base and csec.type in (1, 8) and
                             target < 0 and sym.type == 3 and
                             any(s.shndx == csec.index and s.value == 0 and
                                 s.name and s.type not in (3, 4) for s in syms))
            if not 0 <= target <= csec.size and not negative_base:
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

    def exact_owned_anchor(run):
        """An exact object anchor named by the data-carve record, if proven."""
        names = set(run.get('c_owned_symbols') or [])
        first_name = run.get('first_name')
        if not first_name or first_name not in names:
            return None
        extent = run.get('content_hi', run['hi']) - run['lo']   # without absorbed padding
        matches = [s for s in own if s.name == first_name and s.type == 1 and s.size == extent]
        if len(matches) != 1:
            return None
        sym = matches[0]
        ref = bytes(want(run['lo'], run['first_end'] - run['lo']) or b'').rstrip(b'\0')
        return sym.value if compatible(sym.value, ref) else None

    offsets, notes = [0], []
    if specs:
        first_owner = exact_owned_anchor(specs[0])
        if first_owner:
            # A nonzero object offset leaves a prefix outside every declared
            # run. It is safe to discard only when the prefix is unreferenced,
            # has no named owner, and consists entirely of zero fill. A real
            # earlier C datum belongs to source ordering/another registered
            # run, not to this first scaffold span.
            prefix_has_reference = any(0 <= ref['target'] < first_owner for ref in refs)
            prefix_has_symbol = any(0 <= s.value < first_owner for s in own)
            if prefix_has_reference or prefix_has_symbol or any(data[:first_owner]):
                first_owner = None
        if first_owner is not None:
            offsets[0] = first_owner
            if first_owner:
                notes.append(f'run {h8(specs[0]["lo"])}: exact C-owned OBJECT '
                             f'{specs[0]["first_name"]} selects +0x{first_owner:x}')
    if check_items and specs:
        check_run(0)
    for k in range(1, len(specs)):
        prev, cur = specs[k - 1], specs[k]
        low = offsets[-1] + (prev['last'] - prev['lo']) + 1
        high = offsets[-1] + (prev['hi'] - prev['lo']) + 15
        cands = sorted(a for a in anchors if low <= a <= high and a < size)
        ref = bytes(want(cur['lo'], cur['first_end'] - cur['lo']) or b'').rstrip(b'\0')
        fit = [a for a in cands if compatible(a, ref)]
        named = exact_owned_anchor(cur)
        if named is not None and low <= named <= high and named < size:
            offsets.append(named)
            notes.append(f'run {h8(cur["lo"])}: exact C-owned OBJECT {cur["first_name"]} '
                         f'selects +0x{named:x}')
            if check_items:
                check_run(k)
            continue
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
    if len(data) < 18 or struct.unpack_from('<H', data, 16)[0] != 1:
        raise CarveError('split: not a relocatable object')
    # Complete NOBITS input sections already have their final routing shape.
    # Carry those inputs through without rewriting their relocations.
    if plans:
        identity = True
        for name, pieces in plans.items():
            matches = [section for section in elf.sections if section.name == name]
            if (len(matches) != 1 or matches[0].type != 8 or len(pieces) != 1 or
                    pieces[0].get('offset') != 0 or pieces[0].get('length') != matches[0].size or
                    pieces[0].get('end') != matches[0].size):
                identity = False
                break
        if identity:
            return data
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
        if (csec.type == 8 and len(pieces) == 1 and pieces[0]['length'] == csec.size
                and pieces[0].get('end') == csec.size):
            continue
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
        for ref in section_references(elf, csec, allow_negative_base=pieces[0]['offset'] == 0):
            k = (0 if ref['target'] < 0 else
                 piece_of(pieces, ref['target'], f'the relocation at +0x{ref["offset"]:x} of section {ref["rel"]}'))
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


def original_character_fields(root, unit, ctx, orig, sym, address, record):
    """Prove original OBJECT identities for contiguous C character-array fields.

    The native object must have the complete struct extent and the same binding
    as every original field. Field names, lengths and addresses are pinned and
    re-read from the current source declaration; the ordinary raw-byte and
    whole-file comparisons still apply. The record's source_sha256 is
    provenance only, so editing an unrelated function in the TU keeps the
    proof valid. This admits no padding, casts, assembler storage or anonymous
    byte blobs.
    """
    if not record or sym.bind != 0:
        return False
    source = Path(root) / 'src' / unit / (ctx['name'] + '.c')
    if not source.is_file():
        return False
    text = source.read_text()
    typename = record.get('type', '')
    if not re.fullmatch(r'[A-Za-z_]\w*', typename):
        return False
    definition = re.search(r'typedef\s+struct\s+\w+\s*\{([^{}]+)\}\s*' +
                           re.escape(typename) + r'\s*;', text)
    if not definition or not re.search(r'\bstatic\s+' + re.escape(typename) +
                                      r'\s+' + re.escape(sym.name) + r'\s*=', text):
        return False
    declaration = re.compile(r'(?:unsigned|signed)\s+char\s+(\w+)\s*\[\s*(\d+)\s*\]\s*;')
    members = declaration.findall(definition.group(1))
    fields = record.get('fields') or []
    if (len(fields) < 2 or len(members) != len(fields) or
            declaration.sub('', definition.group(1)).strip()):
        return False
    offset = 0
    for (member, length), field in zip(members, fields):
        size = int(length)
        if field.get('member') != member or field.get('size') != size or size <= 0:
            return False
        identities = [s for s in orig.symbols if s.name == field.get('original_name') and
                      s.shndx != 0 and s.type == 1 and s.bind == sym.bind and
                      s.value == address + offset and s.size == size]
        if len(identities) != 1:
            return False
        offset += size
    return offset == sym.size


def split_plans(root, unit, unit_dir, tu_id, obj_elf, registry=None):
    """{section: plan} for the sections a TU declares with several runs."""
    registry = load_registry(root) if registry is None else registry
    secs = registry['tus'].get(tu_id) or {}
    if any(len(runs or []) > 1 for runs in secs.values()):
        # the same runs apply_main/apply_overlay laid out (absorb_padding)
        pad_ctx = tu_context(root, unit, unit_dir, tu_id)

        def pad_items(sec):
            if sec not in pad_ctx['pieces']:
                return None
            try:
                return section_items(pad_ctx, sec)
            except (OSError, CarveError):
                return None
        secs = {sec: absorb_padding(tu_id, sec, runs, pad_items(sec)) for sec, runs in secs.items()}
    split = split_sections(secs)
    common_rows = [row for rows in (registry.get('common_tail') or {}).values()
                   for row in rows if row.get('tu') == tu_id and
                   (row.get('c_input_span') or {}).get('section') in ('.bss', '.sbss', '.scommon')]
    if not split and not common_rows:
        return {}
    ctx = tu_context(root, unit, unit_dir, tu_id)
    orig = Elf(Path(ctx['orig']).read_bytes())
    plans = {}
    # Initialized placement records describe two coordinate systems: the
    # registry key/range is the original scaffold family and VA, while each
    # c_input_spans entry identifies the raw C input section and object offset.
    # They can differ (for example, a mutable C object can occupy bytes in the
    # original .rodata family).  Split the raw object by its input section and
    # let apply_overlay place each resulting input slice in its target family.
    explicit_inputs = {}
    explicit_seen = False
    for target_sec, target_runs in secs.items():
        if target_sec in ('.bss', '.sbss'):
            continue
        if target_sec not in split:
            continue
        for run_index, run in enumerate(target_runs):
            parts = run.get('c_input_spans') or []
            if not parts:
                continue
            explicit_seen = True
            run_lo, run_hi = (hx(v) for v in run['range'])
            cursor = run_lo
            for part_index, part in enumerate(parts):
                input_sec = base_section_name(part.get('section', ''))
                if input_sec in ('.bss', '.sbss', '.scommon'):
                    continue
                obj_range = part.get('object_range')
                if obj_range is None or len(obj_range) != 2:
                    raise CarveError(f'{tu_id} {target_sec}: initialized input span needs object_range')
                obj_lo, obj_hi = (int(v) for v in obj_range)
                va_lo, va_hi = (hx(v) for v in part.get('range', run['range']))
                if va_lo != cursor or va_hi <= va_lo or va_hi > run_hi or obj_lo < 0 or obj_hi <= obj_lo \
                        or obj_hi - obj_lo != va_hi - va_lo:
                    raise CarveError(f'{tu_id} {target_sec}: explicit input/target extents disagree')
                explicit_inputs.setdefault(input_sec, []).append(dict(
                    offset=obj_lo, length=obj_hi - obj_lo, end=obj_hi, lo=va_lo, hi=va_hi,
                    target_section=target_sec, run_index=run_index, part_index=part_index,
                    symbols=sorted(set(part.get('symbols') or [])),
                    credit=('uncredited_dependency' if part.get('support_only') is True
                            else part.get('credit', 'verified')),
                    support_only=part.get('support_only') is True,
                    anonymous_emission=part.get('anonymous_emission') is True,
                    original_fields=part.get('original_fields'),
                    zero_padding_tail=int(part.get('zero_padding_tail', 0))))
                cursor = va_hi
            if cursor != run_hi:
                raise CarveError(f'{tu_id} {target_sec}: explicit input subspans do not cover the target run')
    if explicit_seen:
        scaffold_items = {}
        for target_sec in secs:
            if target_sec in ('.bss', '.sbss'):
                continue
            try:
                scaffold_items[target_sec] = section_items(ctx, target_sec)
            except (OSError, CarveError):
                scaffold_items[target_sec] = []
        credited_by_run = {}
        for input_sec, pieces in explicit_inputs.items():
            csec = next((s for s in obj_elf.sections if s.name == input_sec and s.size), None)
            if csec is None or csec.type == 8:
                raise CarveError(f'{tu_id}: explicit input map requires populated raw {input_sec}')
            pieces.sort(key=lambda p: (p['offset'], p['end']))
            intervals = []
            for p in pieces:
                cursor = p['offset']
                c_owned = set((secs[p['target_section']][p['run_index']].get('c_owned_symbols') or []))
                uncredited = set((secs[p['target_section']][p['run_index']].get('uncredited_c_symbols') or []))
                if p['credit'] not in ('verified', 'uncredited_dependency'):
                    raise CarveError(f'{tu_id} {input_sec}: invalid span credit value {p["credit"]!r}')
                if p.get('support_only'):
                    if p['symbols'] or c_owned:
                        raise CarveError(f'{tu_id} {input_sec}: support_only spans cannot own credited identities')
                    named = [sym.name for sym in obj_elf.symbols
                             if sym.shndx == csec.index and sym.type == 1
                             and p['offset'] <= sym.value < p['end']]
                    if named:
                        raise CarveError(f'{tu_id} {input_sec}: support_only span contains named C objects {named}')
                elif p['credit'] == 'verified':
                    run_key = (p['target_section'], p['run_index'])
                    if p['symbols']:
                        if not set(p['symbols']) <= c_owned:
                            raise CarveError(f'{tu_id} {input_sec}: credited raw names exceed c_owned_symbols')
                        raw_names = {sym.name for sym in obj_elf.symbols
                                     if sym.shndx == csec.index and sym.type == 1
                                     and p['offset'] <= sym.value < p['end']}
                        if not set(p['symbols']) <= raw_names:
                            raise CarveError(f'{tu_id} {input_sec}: credited raw object names differ from c_input_spans')
                        credited_by_run.setdefault(run_key, set()).update(p['symbols'])
                    else:
                        items = scaffold_items.get(p['target_section'], [])
                        if not (p['anonymous_emission'] and
                                _anonymous_span_names_are_scaffold(items, c_owned, p['lo'], p['hi'])):
                            raise CarveError(f'{tu_id} {input_sec}: anonymous credited span lacks exact scaffold identity evidence')
                        credited_by_run.setdefault(run_key, set()).update(c_owned)
                if p['credit'] == 'uncredited_dependency' and not p.get('support_only'):
                    if p['symbols'] and (set(p['symbols']) != uncredited or set(p['symbols']) & c_owned):
                        raise CarveError(f'{tu_id} {input_sec}: uncredited named span must be isolated from credited names')
                    if not p['symbols']:
                        # An anonymous uncredited span has no symbol to prove
                        # it by, and no registry declares one; such a span
                        # needs a structural proof before it can be admitted.
                        raise CarveError(
                            f'{tu_id} {input_sec}: anonymous uncredited span lacks an exact scaffold disposition')
                named_symbols = []
                for name in p['symbols']:
                    matches = [s for s in obj_elf.symbols if s.name == name and s.shndx == csec.index
                               and s.type == 1]
                    if len(matches) != 1:
                        raise CarveError(f'{tu_id} {input_sec}: expected one raw OBJECT {name}, found {len(matches)}')
                    named_symbols.append(matches[0])
                if not p['symbols']:
                    overlapping = [s for s in obj_elf.symbols if s.shndx == csec.index
                                   and s.type not in (3, 4) and p['offset'] <= s.value < p['end']]
                    if p['credit'] == 'verified' and p['anonymous_emission']:
                        overlapping = [s for s in overlapping if not (s.bind == 0 and s.type == 0)]
                    if overlapping:
                        raise CarveError(
                            f'{tu_id} {input_sec}: anonymous span contains named raw symbols '
                            f'{[s.name for s in overlapping]}')
                    # Anonymous, uncredited compiler pools can carry relocations
                    # (for example cc1 literal pools). Their exact range and
                    # bytes are independently pinned by the dependency record;
                    # keep the relocation-bearing input bytes in the split.
                    cursor = p['end']
                for sym in sorted(named_symbols, key=lambda s: (s.value, s.name)):
                    name = sym.name
                    if sym.value != cursor or sym.size <= 0 or sym.value + sym.size > p['end']:
                        raise CarveError(f'{tu_id} {input_sec}: raw OBJECT {name} does not tile its input span')
                    target_va = p['lo'] + sym.value - p['offset']
                    original_at_va = [s for s in orig.symbols if s.value == target_va and s.shndx != 0
                                      and s.type == 1]
                    positive = [s for s in original_at_va if s.size > 0]
                    if positive:
                        identities = [s for s in positive if s.bind == sym.bind and s.size == sym.size]
                        if len(identities) != 1 and not original_character_fields(
                                root, unit, ctx, orig, sym, target_va, p.get('original_fields')):
                            raise CarveError(
                                f'{tu_id} {input_sec}: raw owner {name} violates positive-size original OBJECT '
                                f'identity at {h8(target_va)}')
                    elif original_at_va:
                        identities = [s for s in original_at_va if s.bind == sym.bind]
                        if len(identities) != 1:
                            raise CarveError(
                                f'{tu_id} {input_sec}: raw owner {name} violates original zero-size OBJECT '
                                f'binding at {h8(target_va)}')
                        items = scaffold_items.get(p['target_section'], [])
                        exact_item = [it for it in items if it['start'] == target_va
                                      and name in it['labels'] and it['end'] - it['start'] == sym.size]
                        # The retail identity can use a different label
                        # spelling; accept an exact same-address item only
                        # when its alias is the original ELF symbol.
                        if not exact_item:
                            orig_names = {s.name for s in identities}
                            exact_item = [it for it in items if it['start'] == target_va
                                          and orig_names.intersection(it['labels'])
                                          and it['end'] - it['start'] == sym.size]
                        if not exact_item:
                            raise CarveError(
                                f'{tu_id} {input_sec}: zero-size original OBJECT lacks an exact scaffold extent '
                                f'for {name} at {h8(target_va)}')
                    else:
                        items = scaffold_items.get(p['target_section'], [])
                        exact_item = [it for it in items if it['start'] == target_va
                                      and it['end'] - it['start'] == sym.size]
                        if not exact_item and p.get('zero_padding_tail', 0):
                            exact_item = [it for it in items if it['start'] == p['lo']
                                          and it['end'] == p['hi'] and name in it['labels']]
                        if not exact_item and not p.get('zero_padding_tail', 0):
                            # Some generated data labels include following alignment
                            # fill in their assembler item, while the original C
                            # OBJECT ends exactly at its own bytes. Preserve that
                            # fill in the scaffold when one emitted OBJECT exactly
                            # tiles the routed extent and its raw bytes match retail.
                            raw_extent = obj_elf.section_bytes(csec)[p['offset']:p['end']]
                            original_extent = va_bytes(orig, p['lo'], p['hi'] - p['lo'])
                            extent_relocs = [off for off, _, _ in read_relocations(obj_elf).get(csec.index, [])
                                             if p['offset'] <= off < p['end']]
                            if (p['symbols'] == [name] and sym.value == p['offset']
                                    and sym.size == p['end'] - p['offset']
                                    and p['hi'] - p['lo'] == sym.size
                                    and original_extent == raw_extent and not extent_relocs):
                                exact_item = [dict(start=p['lo'], end=p['hi'], labels=[name])]
                        if not exact_item:
                            raise CarveError(
                                f'{tu_id} {input_sec}: no original OBJECT or exact scaffold item proves '
                                f'{name} at {h8(target_va)} size 0x{sym.size:x}')
                    cursor += sym.size
                padding = p.get('zero_padding_tail', 0)
                if padding:
                    target_items = scaffold_items.get(p['target_section'], [])
                    exact_item = [it for it in target_items if it['start'] == p['lo']
                                  and it['end'] == p['hi'] and
                                  set(p['symbols']).intersection(it['labels'])]
                    data = obj_elf.section_bytes(csec)
                    pad_lo = p['end'] - padding
                    pad_relocs = [off for off, _, _ in read_relocations(obj_elf).get(csec.index, [])
                                  if pad_lo <= off < p['end']]
                    pad_symbols = [s for s in obj_elf.symbols if s.shndx == csec.index
                                   and s.type not in (3, 4) and pad_lo <= s.value < p['end']]
                    if (padding <= 0 or cursor != pad_lo or len(exact_item) != 1
                            or any(data[pad_lo:p['end']]) or pad_relocs or pad_symbols):
                        raise CarveError(f'{tu_id} {input_sec}: trailing zero padding lacks exact scaffold-item proof')
                elif cursor != p['end']:
                    raise CarveError(f'{tu_id} {input_sec}: raw object names do not cover mapped span')
                byte_piece = dict(offset=p['offset'], length=p['length'], end=p['end'], lo=p['lo'], hi=p['hi'])
                equal = compare_bytes(obj_elf, csec, [byte_piece], ctx,
                                      target_pieces=explicit_inputs)
                if not equal.get('checked') or not equal.get('equal'):
                    raise CarveError(f'{tu_id} {input_sec}: raw mapped bytes differ from original scaffold bytes '
                                     f'for VA {h8(p["lo"])}-{h8(p["hi"])} at {input_sec}+0x{p["offset"]:x}: {equal}')
                intervals.append(p)
            last = 0
            data = obj_elf.section_bytes(csec)
            relocs = [off for off, _, _ in read_relocations(obj_elf).get(csec.index, [])]
            syms = [s for s in obj_elf.symbols if s.shndx == csec.index and s.type not in (3, 4)]
            split_pieces = []
            for p in pieces:
                if p['offset'] < last:
                    raise CarveError(f'{tu_id} {input_sec}: raw spans overlap')
                if any(data[last:p['offset']]):
                    raise CarveError(f'{tu_id} {input_sec}: unclaimed raw gap {last:#x}..{p["offset"]:#x} is nonzero')
                if any(last <= off < p['offset'] for off in relocs) or \
                        any(last <= s.value < p['offset'] for s in syms):
                    raise CarveError(f'{tu_id} {input_sec}: unclaimed raw gap contains relocation or symbol')
                split_pieces.append(dict(offset=p['offset'], length=p['length'], end=p['end'], lo=p['lo'], hi=p['hi']))
                last = p['end']
            if pieces[0]['offset'] != 0 or pieces[-1]['end'] != csec.size:
                raise CarveError(f'{tu_id} {input_sec}: explicit mapping must account for section endpoints')
            plans[input_sec] = dict(pieces=split_pieces,
                                    notes=['explicit raw-input to original-scaffold spans validated by symbols, bytes, and exact gaps'])
        for target_sec, target_runs in secs.items():
            if target_sec in ('.bss', '.sbss'):
                continue
            for run_index, run in enumerate(target_runs):
                if not run.get('c_input_spans'):
                    continue
                expected = set(run.get('c_owned_symbols') or [])
                observed = credited_by_run.get((target_sec, run_index), set())
                if observed != expected:
                    raise CarveError(f'{tu_id} {target_sec}: credited raw owners {sorted(observed)} '
                                     f'differ from declared c_owned_symbols {sorted(expected)}')
    for sec in split:
        if sec in plans:
            continue
        if sec not in ('.bss', '.sbss') and any(run.get('c_input_spans') for run in secs[sec]):
            # A target family can contain slices from a different raw input
            # section. Those spans were validated together above by raw
            # section (including cross-family ranges); never reinterpret them
            # as if their target section were also their object section.
            if any(run.get('c_input_spans') for run in secs[sec]):
                continue
            csec = next((s for s in obj_elf.sections if s.name == raw_section_name(unit, sec) and s.size), None)
            if csec is None or csec.type == 8:
                raise CarveError(f'{tu_id}: explicit {sec} input spans need one populated PROGBITS section')
            orig = Elf(Path(ctx['orig']).read_bytes())
            run_rows = []
            input_intervals = []
            for run_index, run in enumerate(secs[sec]):
                lo, hi = (hx(v) for v in run['range'])
                parts = run.get('c_input_spans') or []
                cursor = lo
                for part in parts:
                    if base_section_name(part.get('section', '')) != csec.name:
                        raise CarveError(f'{tu_id} {sec}: input span section does not match raw {csec.name}')
                    va_lo, va_hi = (hx(v) for v in part.get('range', []))
                    obj_range = part.get('object_range')
                    if obj_range is None or len(obj_range) != 2:
                        raise CarveError(f'{tu_id} {sec}: explicit initialized span needs object_range')
                    obj_lo, obj_hi = (int(v) for v in obj_range)
                    if (va_lo != cursor or va_hi <= va_lo or va_hi > hi or obj_lo < 0 or obj_hi <= obj_lo
                            or obj_hi > csec.size or obj_hi - obj_lo != va_hi - va_lo):
                        raise CarveError(f'{tu_id} {sec}: explicit object/VA subspan extents disagree')
                    cursor = va_hi
                if cursor != hi:
                    raise CarveError(f'{tu_id} {sec}: explicit object/VA subspans do not cover the target run')
                # Validate each exact owner slice separately, retaining its own
                # target range, raw-object interval and ownership disposition.
                covered_owners = set()
                for part in parts:
                    va_lo, va_hi = (hx(v) for v in part['range'])
                    obj_lo, obj_hi = (int(v) for v in part['object_range'])
                    names = sorted(set(part.get('symbols') or []))
                    support_only = part.get('support_only') is True
                    credit = ('uncredited_dependency' if support_only
                              else part.get('credit', 'verified'))
                    if credit not in ('verified', 'uncredited_dependency'):
                        raise CarveError(f'{tu_id} {sec}: unknown initialized input-span credit state {credit!r}')
                    credited = credit == 'verified'
                    run_owners = set(run.get('c_owned_symbols') or [])
                    uncredited_owners = set(run.get('uncredited_c_symbols') or [])
                    if credited and (not set(names) or not set(names) <= run_owners):
                        scaffold = section_items(ctx, sec)
                        if not (part.get('anonymous_emission') is True and not names and
                                _anonymous_span_names_are_scaffold(scaffold, run_owners, va_lo, va_hi)):
                            raise CarveError(f'{tu_id} {sec}: credited input-span names differ from c_owned_symbols')
                    if not credited and not names and not support_only:
                        dispositions = run.get('uncredited_scaffold_ranges') or []
                        if not any(hx(x['range'][0]) == va_lo and hx(x['range'][1]) == va_hi for x in dispositions):
                            raise CarveError(f'{tu_id} {sec}: anonymous input bytes lack an exact uncredited disposition')
                    if not credited and not support_only and (set(names) != uncredited_owners or set(names) & run_owners):
                        raise CarveError(f'{tu_id} {sec}: uncredited input span must be isolated from c_owned_symbols')
                    if support_only and names:
                        raise CarveError(f'{tu_id} {sec}: support_only input spans cannot claim names')
                    covered_owners.update(names)
                    cursor = obj_lo
                    if credited and not names:
                        raw_named = [s.name for s in obj_elf.symbols
                                     if s.shndx == csec.index and s.type == 1
                                     and obj_lo <= s.value < obj_hi]
                        if raw_named:
                            raise CarveError(f'{tu_id} {sec}: anonymous emission contains raw OBJECT symbols {raw_named}')
                        cursor = obj_hi
                    for name in names:
                        matches = [s for s in obj_elf.symbols if s.name == name and s.shndx == csec.index and s.type == 1]
                        if len(matches) != 1:
                            raise CarveError(f'{tu_id} {sec}: expected one raw OBJECT {name}, found {len(matches)}')
                        sym = matches[0]
                        if sym.value != cursor or sym.size <= 0 or sym.value + sym.size > obj_hi:
                            raise CarveError(f'{tu_id} {sec}: raw OBJECT {name} does not exactly tile its input span')
                        originals = [s for s in orig.symbols if s.name == name and s.shndx != 0
                                     and s.type == 1 and s.value == va_lo + sym.value - obj_lo]
                        meaningful = [s for s in originals if s.size > 1]
                        if len(meaningful) != 1 or meaningful[0].bind != sym.bind:
                            raise CarveError(f'{tu_id} {sec}: {name} lacks a unique same-address original OBJECT')
                        if credited and meaningful[0].size != sym.size:
                            raise CarveError(f'{tu_id} {sec}: {name} lacks exact original OBJECT extent')
                        if not credited and meaningful[0].size < sym.size:
                            raise CarveError(f'{tu_id} {sec}: uncredited {name} exceeds its original owner extent')
                        cursor += sym.size
                    if cursor != obj_hi:
                        raise CarveError(f'{tu_id} {sec}: named raw OBJECTs do not cover the input span')
                    piece = dict(offset=obj_lo, length=obj_hi - obj_lo, end=obj_hi, lo=va_lo, hi=va_hi)
                    byte_check = compare_bytes(obj_elf, csec, [piece], ctx)
                    if not byte_check.get('checked') or not byte_check.get('equal'):
                        raise CarveError(f'{tu_id} {sec}: explicit span bytes fail relocation-normalized original comparison')
                    run_rows.append(piece)
                    input_intervals.append((obj_lo, obj_hi, run_index))
                if covered_owners != run_owners:
                    raise CarveError(f'{tu_id} {sec}: initialized subspans do not cover exactly c_owned_symbols')
            input_intervals.sort()
            if not input_intervals or input_intervals[0][0] != 0 or input_intervals[-1][1] != csec.size:
                raise CarveError(f'{tu_id} {sec}: explicit input spans must cover section endpoints')
            relocs = {off for off, _, _ in read_relocations(obj_elf).get(csec.index, [])}
            syms = [s for s in obj_elf.symbols if s.shndx == csec.index and s.type not in (3, 4)]
            cursor = 0
            for lo, hi, _ in input_intervals:
                if lo < cursor:
                    raise CarveError(f'{tu_id} {sec}: explicit input spans overlap or are out of order')
                if any(obj_elf.section_bytes(csec)[cursor:lo]):
                    raise CarveError(f'{tu_id} {sec}: unclaimed raw input gap {cursor:#x}..{lo:#x} is nonzero')
                if any(cursor < off + 4 and off < lo for off in relocs) or any(cursor <= s.value < lo for s in syms):
                    raise CarveError(f'{tu_id} {sec}: unclaimed raw input gap contains a relocation or symbol')
                cursor = hi
            plan_pieces = [dict(offset=p['offset'], length=p['length'], end=p['end'], lo=p['lo'], hi=p['hi'])
                           for p in run_rows]
            plans[sec] = dict(pieces=plan_pieces, notes=['explicit object_range spans validated against named raw and original OBJECTs'])
            continue
        if sec in ('.bss', '.sbss'):
            input_runs = {}
            for run in secs[sec]:
                for part in run.get('c_input_spans') or []:
                    if part.get('object_range') is None:
                        raise CarveError(f'{tu_id} {sec}: a split NOBITS run needs c_input_spans.object_range')
                    base = base_section_name(part['section'])
                    lo, hi = (hx(x) for x in part['range'])
                    obj_lo, obj_hi = (int(x) for x in part['object_range'])
                    if base not in ('.bss', '.sbss', '.scommon') or obj_lo < 0 or obj_hi <= obj_lo:
                        raise CarveError(f'{tu_id} {sec}: invalid NOBITS object_range for {base}')
                    if obj_hi - obj_lo != hi - lo:
                        raise CarveError(f'{tu_id} {sec}: compiler NOBITS slice length differs from its original span')
                    input_runs.setdefault(base, []).append(dict(offset=obj_lo, length=obj_hi - obj_lo,
                                                                 end=obj_hi, lo=lo, hi=hi,
                                                                 symbols=part.get('symbols') or []))
            for base, pieces in sorted(input_runs.items()):
                pieces.sort(key=lambda p: p['offset'])
                csec = next((s for s in obj_elf.sections if s.name == base and s.size), None)
                if csec is None or csec.type != 8 or pieces[0]['offset'] != 0:
                    raise CarveError(f'{tu_id}: split NOBITS input {base} must be allocated SHT_NOBITS from offset zero')
                if len(pieces) == 1:
                    piece = pieces[0]
                    if piece['end'] != csec.size or piece['length'] != csec.size:
                        continue
                    if piece['hi'] - piece['lo'] != csec.size or not piece['symbols']:
                        raise CarveError(f'{tu_id}: identity NOBITS input {base} lacks an exact full-section owner span')
                    for name in piece['symbols']:
                        matches = [sym for sym in obj_elf.symbols
                                   if sym.name == name and sym.shndx == csec.index]
                        if (len(matches) != 1 or not 0 <= matches[0].value < csec.size
                                or matches[0].value + matches[0].size > csec.size):
                            raise CarveError(f'{tu_id}: identity NOBITS input {base} has no unique in-bounds owner {name}')
                    plans[base] = dict(pieces=[dict(offset=0, length=csec.size, end=csec.size,
                                                     lo=piece['lo'], hi=piece['hi'])],
                                       notes=['complete raw NOBITS input section'])
                    continue
                cursor = 0
                for piece in pieces:
                    if piece['offset'] < cursor or piece['end'] > csec.size:
                        raise CarveError(f'{tu_id}: split NOBITS object ranges overlap or exceed {base}')
                    if not piece['symbols']:
                        raise CarveError(f'{tu_id}: split NOBITS spans must name compiler owners')
                    for name in piece['symbols']:
                        matches = [sym for sym in obj_elf.symbols
                                   if sym.name == name and sym.shndx == csec.index]
                        if len(matches) != 1 or not piece['offset'] <= matches[0].value < piece['end']:
                            raise CarveError(f'{tu_id}: {base} slice has no unique named owner {name} in its range')
                    cursor = piece['end']
                plans[base] = dict(pieces=pieces, notes=[])
            continue
        c_name = raw_section_name(unit, sec)
        csec = next((s for s in obj_elf.sections if s.name == c_name and s.size), None)
        if csec is None:
            raise CarveError(f'{tu_id}: the C object emits no {sec}, but {len(secs[sec])} runs are declared')
        specs = run_specs(section_items(ctx, sec), declared_runs(tu_id, sec, secs[sec]), secs[sec])
        plans[sec] = plan_split(obj_elf, csec, specs, lambda va, n: va_bytes(orig, va, n))
    # Split naturally allocated COMMON storage by exact native symbol offsets.
    # Small COMMON and ordinary COMMON use different input sections; both can
    # contain independently addressed owners in the original linker tail.
    # Alignment holes remain with the existing scaffold pieces.
    common_inputs = {}
    for row in common_rows:
        common_inputs.setdefault(row['c_input_span']['section'], []).append(row)
    for input_sec, input_rows in common_inputs.items():
        csec = next((s for s in obj_elf.sections if s.name == input_sec), None)
        if csec is None or csec.type != 8:
            raise CarveError(f'{tu_id}: common-tail split requires exact raw SHT_NOBITS {input_sec}')
        existing = plans.get(input_sec, {})
        pieces = list(existing.get('pieces', [])) if isinstance(existing, dict) else list(existing)
        symbols = [s for s in obj_elf.symbols if s.shndx == csec.index and s.type not in (3, 4)]
        for row in sorted(input_rows, key=lambda r: int((r.get('c_input_span') or {}).get(
                'object_range', [-1])[0])):
            owner = row.get('storage_owner') or {}
            part = row['c_input_span']
            obj_range = part.get('object_range') or []
            lo, hi = (hx(v) for v in row['range'])
            if len(obj_range) != 2:
                raise CarveError(f'{tu_id}: common-tail owner lacks a bounded raw {input_sec} range')
            off, end = map(int, obj_range)
            matches = [s for s in symbols if s.name == owner.get('name') and s.value == off and
                       s.size == end - off and s.size == hi - lo and s.bind != 0]
            if len(matches) != 1 or off < 0 or end > csec.size:
                raise CarveError(f'{tu_id}: common-tail {input_sec} span lacks exact raw owner {owner.get("name")}')
            pieces.append(dict(offset=off, length=end-off, end=end, lo=lo, hi=hi,
                               symbols=[owner['name']]))
        pieces.sort(key=lambda p: p['offset'])
        cursor = 0
        rel_offsets = [off for off, _, _ in read_relocations(obj_elf).get(csec.index, [])]
        for piece in pieces:
            if piece['offset'] < cursor or any(cursor <= s.value < piece['offset'] for s in symbols) or \
                    any(cursor <= off < piece['offset'] for off in rel_offsets):
                raise CarveError(f'{tu_id}: raw {input_sec} owner spans overlap or leave owned gap bytes')
            cursor = piece['end']
        if cursor != csec.size:
            if any(s.value >= cursor for s in symbols) or any(off >= cursor for off in rel_offsets):
                raise CarveError(f'{tu_id}: raw {input_sec} trailing bytes contain an unselected owner/relocation')
        plans[input_sec] = dict(pieces=pieces,
                                notes=['common-tail owners split by exact raw offsets; alignment gaps remain scaffold'])
    return plans


def derive(root, unit, unit_dir, tu_id, source, obj):
    """{'runs': {section: [run]}, 'sections': {section: detail}, 'problems': [..]} for one candidate."""
    ctx = tu_context(root, unit, unit_dir, tu_id)
    declared = (load_registry(root)['tus'].get(tu_id) or {})
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
    if unit != 'main':
        raw_cc1 = overlay_cc1_sidecar(unit_dir, unit, ctx['name'])
        if raw_cc1:
            out['raw_cc1_provenance'] = raw_cc1
    # When an immutable record supplies exact raw-object offsets for initialized
    # owners, validate those coordinates directly.  The legacy heuristic below
    # assumes the raw input section and original scaffold family have the same
    # name and that target runs appear in raw object order; neither assumption
    # holds for all OVL data.  The explicit path keeps those coordinate systems
    # separate and still checks the exact raw symbols, original identity/bytes,
    # and every unclaimed input gap through split_plans.
    explicit_sections = {sec: runs for sec, runs in declared.items()
                         if sec not in ('.bss', '.sbss') and any(r.get('c_input_spans') for r in runs)}
    if explicit_sections:
        if any(not run.get('c_input_spans') for runs in explicit_sections.values() for run in runs):
            out['problems'].append('explicit initialized mappings cannot mix mapped and heuristic runs in one section')
            return out
        try:
            initialized_registry = copy.deepcopy(load_registry(root))
            initialized_registry['tus'][tu_id] = {sec:runs for sec,runs in declared.items()
                                                if sec not in ('.bss', '.sbss')}
            initialized_registry.pop('common_tail', None)
            plans = split_plans(root, unit, unit_dir, tu_id, elf, registry=initialized_registry)
            asm_path = Path(obj).with_suffix('.s')
            if not asm_path.is_file() and Path(str(obj) + '.s').is_file():
                asm_path = Path(str(obj) + '.s')
            if not asm_path.is_file():
                raise CarveError(f'{tu_id}: raw cc1 assembly is unavailable next to {obj}')
            app = False
            asm_labels = set()
            for line in asm_path.read_text(encoding='utf-8', errors='replace').splitlines():
                token = line.strip()
                if token == '#APP':
                    app = True
                    continue
                if token == '#NO_APP':
                    app = False
                    continue
                if not app:
                    m = re.match(r'^([A-Za-z_.$][A-Za-z0-9_.$]*):(?:\s|$)', token)
                    if m:
                        asm_labels.add(m.group(1))
            raw_owner_names = {n for runs in explicit_sections.values() for run in runs
                               for part in run.get('c_input_spans') or [] for n in part.get('symbols') or []}
            missing_cc1 = sorted(raw_owner_names - asm_labels)
            if missing_cc1:
                raise CarveError(f'{tu_id}: raw cc1 has no outside-#APP C storage labels: {", ".join(missing_cc1)}')
            for sec, runs in explicit_sections.items():
                out_runs = []
                total = 0
                source_sections = set()
                for run in runs:
                    lo, hi = (hx(v) for v in run['range'])
                    parts = run.get('c_input_spans') or []
                    cursor = lo
                    owners = set(run.get('c_owned_symbols') or [])
                    seen_owners = set()
                    for part in parts:
                        input_sec = base_section_name(part.get('section', ''))
                        p_lo, p_hi = (hx(v) for v in part.get('range', []))
                        obj_range = part.get('object_range')
                        if p_lo != cursor or p_hi <= p_lo or p_hi > hi or obj_range is None or len(obj_range) != 2:
                            raise CarveError(f'{tu_id} {sec}: malformed or discontinuous explicit raw-input subspan')
                        names = sorted(set(part.get('symbols') or []))
                        support_only = part.get('support_only') is True
                        credited = part.get('credit', 'verified') == 'verified' and not support_only
                        anonymous_names = set()
                        if credited and not names and part.get('anonymous_emission') is True:
                            items = section_items(ctx, sec)
                            if not _anonymous_span_names_are_scaffold(items, owners, p_lo, p_hi):
                                raise CarveError(f'{tu_id} {sec}: anonymous subspan lacks original scaffold identities')
                            anonymous_names = owners
                        elif credited and (not names or not set(names) <= owners):
                            raise CarveError(f'{tu_id} {sec}: credited subspan names differ from c_owned_symbols')
                        if support_only and names:
                            raise CarveError(f'{tu_id} {sec}: support_only subspan cannot claim identities')
                        if not credited and not support_only and set(names) != set(run.get('uncredited_c_symbols') or []):
                            raise CarveError(f'{tu_id} {sec}: dependency-only subspan is not isolated from credited names')
                        if input_sec not in plans:
                            raise CarveError(f'{tu_id} {sec}: explicit raw section {input_sec} has no split plan')
                        source_sections.add(input_sec)
                        if credited:
                            total += int(obj_range[1]) - int(obj_range[0])
                        seen_owners.update(names)
                        seen_owners.update(anonymous_names)
                        cursor = p_hi
                    if cursor != hi or seen_owners != owners:
                        raise CarveError(f'{tu_id} {sec}: explicit subspans do not cover the target run and owners')
                    uncredited = set(run.get('uncredited_c_symbols') or [])
                    out_run = dict(range=[h8(lo), h8(hi)], generated=[h8(lo), h8(hi)], generated_by=[],
                                   c_owned_symbols=sorted(set(run.get('c_owned_symbols') or [])),
                                   c_input_spans=parts)
                    if not credited:
                        out_run['uncredited_scaffold_ranges'] = run.get('uncredited_scaffold_ranges') or []
                    if uncredited:
                        out_run['uncredited_c_symbols'] = sorted(uncredited)
                    if run.get('c_storage_aliases'):
                        out_run['c_storage_aliases'] = run['c_storage_aliases']
                    out_runs.append(out_run)
                out['runs'][sec] = out_runs
                out['sections'][sec] = dict(input_sections=sorted(source_sections),
                                             c_owned_bytes=total,
                                             **({'uncredited_dependency_bytes': sum(
                                                 hx(run['range'][1]) - hx(run['range'][0])
                                                 for run in runs if any(
                                                     p.get('credit') == 'uncredited_dependency'
                                                     for p in (run.get('c_input_spans') or [])))}
                                                if any(any(p.get('credit') == 'uncredited_dependency'
                                                           for p in (run.get('c_input_spans') or []))
                                                       for run in runs) else {}),
                                             bytes=dict(checked=True, equal=True,
                                                        method='raw object_range to original VA bytes with relocation normalization'))
        except CarveError as exc:
            out['problems'].append(str(exc))
            return out
        return out
    # 1. the sections to carve and their items; the items the original assembly
    # of the C functions names (or the C object defines)
    work = {}
    for sec in SECTIONS:
        if unit != 'main' and sec in ('.sbss', '.bss'):
            continue
        c_name = raw_section_name(unit, sec)
        csec = next((s for s in elf.sections if s.name == c_name and s.size), None)
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
        explicit_starts = {hx(r['range'][0]) for r in (declared.get(sec) or [])
                           if r.get('c_input_spans')}
        if explicit_starts:
            split_groups = []
            for group in groups:
                current = []
                for i in group:
                    if current and items[i]['start'] in explicit_starts:
                        split_groups.append(current)
                        current = []
                    current.append(i)
                if current:
                    split_groups.append(current)
            groups = split_groups
        if len(groups) > 1:
            detail['scaffold_between_runs'] = [
                dict(item=items[i]['name'],
                     referenced_by=sorted(n for n in files if n not in c_funcs
                                          and set(items[i]['labels']) & identifiers([files[n]])))
                for a, b in zip(groups, groups[1:]) for i in range(a[-1] + 1, b[0])]
        if orig is None:
            orig = Elf(Path(ctx['orig']).read_bytes())
        # A run whose C layout pads between two of its items (another alignment
        # phase than the original) is cut there into two runs (plan_split
        # check_items); every cut is recorded.
        plan, regrouped = None, []
        for _ in range(len(gen) + 1):
            spans = [(items[g[0]]['start'], items[g[-1]]['end']) for g in groups]
            group_records = [r for r in (declared.get(sec) or [])
                             if hx(r['range'][0]) in {lo for lo, _ in spans}]
            if csec.type == 8:
                syms = [s for s in elf.symbols if s.shndx == csec.index]
                owned = [(i, s) for i in gen for s in syms
                         if s.name in set(items[i]['labels']) and s.type == 1 and s.bind == 1]
                if len(owned) == 1:
                    i, sym = owned[0]
                    lo = items[i]['start']
                    spans = [(lo, lo + sym.size)]
                if len(spans) > 1 and len(group_records) == len(spans):
                    target_pieces = []
                    raw_ranges = []
                    proof = []
                    valid = True
                    for (lo, hi), record in zip(spans, group_records):
                        parts = record.get('c_input_spans') or []
                        names = set(record.get('c_owned_symbols') or [])
                        if (len(parts) != 1 or not names or
                                parts[0].get('section') != csec.name or
                                tuple(hx(v) for v in parts[0].get('range', [])) != (lo, hi)):
                            valid = False
                            break
                        obj_range = parts[0].get('object_range')
                        raw_names = set(parts[0].get('symbols') or [])
                        if (obj_range is None or len(obj_range) != 2 or
                                raw_names != names or int(obj_range[1]) - int(obj_range[0]) != hi - lo):
                            valid = False
                            break
                        off, end = map(int, obj_range)
                        if off < 0 or end > csec.size:
                            valid = False
                            break
                        raw_ranges.append((off, end))
                        target_pieces.append(dict(offset=off, length=end-off, end=end, lo=lo, hi=hi))
                        for name in sorted(names):
                            raw = [s for s in syms if s.name == name]
                            retail = [s for s in orig.symbols if s.name == name and s.shndx != 0]
                            if (len(raw) != 1 or len(retail) != 1 or
                                    raw[0].value != off or retail[0].value != lo or
                                    raw[0].bind != retail[0].bind or raw[0].type != retail[0].type):
                                valid = False
                                break
                            sym = raw[0]
                            if sym.size:
                                if sym.size != hi-lo or retail[0].size != hi-lo:
                                    valid = False
                                    break
                            elif (sym.type != 0 or sym.bind != 0 or retail[0].size != 0):
                                valid = False
                                break
                            proof.append(dict(name=name,
                                              bind='LOCAL' if sym.bind == 0 else 'GLOBAL',
                                              type='NOTYPE' if sym.type == 0 else 'OBJECT',
                                              input_section=csec.name, offset=sym.value,
                                              size=sym.size, mapped_address=h8(lo), extent=hi-lo))
                        if not valid:
                            break
                    if valid:
                        raw_ranges.sort()
                        cursor = 0
                        for off, end in raw_ranges:
                            if off != cursor:
                                valid = False
                                break
                            cursor = end
                        valid = valid and cursor == csec.size
                    if valid:
                        for lo, hi in spans:
                            if not any(p['start'] <= lo < hi <= p['end']
                                       for p in ctx['pieces'].get(sec, [])):
                                valid = False
                                break
                    if valid:
                        detail['nobits_ownership'] = proof
                        detail['covered_scaffold_items'] = [it['name'] for it in items
                                                           if any(lo <= it['start'] < hi for lo, hi in spans)]
                        detail['bytes'] = dict(checked=False,
                                               reason='NOBITS has no byte payload; exact raw section tiling and original symbol identity prove each mapped span')
                        plan = dict(pieces=target_pieces,
                                    notes=['explicit NOBITS raw ranges tile the complete compiler section and map by original symbol identity'])
                        break
                    out['problems'].append(f'{sec}: explicit multi-span NOBITS owner mapping failed exact symbol/range proof')
                    break
                if len(spans) != 1:
                    out['problems'].append(f'{sec}: NOBITS placement currently requires one contiguous owned span')
                    break
                lo, hi = spans[0]
                bounds = ctx['pieces'].get(sec, [])
                piece_end = max((p['end'] for p in bounds), default=lo)
                if hi > piece_end:
                    out['problems'].append(f'{sec}: NOBITS compiler object extends past the mapped TU piece')
                    break
                if hi - lo != csec.size or lo % max(1, csec.align):
                    out['problems'].append(
                        f'{sec}: NOBITS section size/alignment does not prove the scaffold span '
                        f'{h8(lo)}-{h8(hi)} (input size 0x{csec.size:x}, alignment 0x{csec.align:x})')
                    break
                proof = []
                if len(owned) == 1:
                    i, sym = owned[0]
                    if sym.value != 0 or sym.size != hi - lo:
                        out['problems'].append(f'{sec}: NOBITS owner {sym.name} does not span exactly {h8(lo)}-{h8(hi)}')
                    else:
                        proof.append(dict(name=sym.name, bind='GLOBAL', type='OBJECT',
                                          input_section=csec.name, offset=sym.value, size=sym.size))
                else:
                    local_notypes = []
                    for i in gen:
                        it = items[i]
                        labels = set(it['labels'])
                        matches = [s for s in syms if s.name in labels and s.type == 1 and s.bind == 1]
                        if len(matches) == 1 and matches[0].value == it['start'] - lo and \
                                matches[0].size == it['end'] - it['start']:
                            sym = matches[0]
                            proof.append(dict(name=sym.name, bind='GLOBAL', type='OBJECT',
                                              input_section=csec.name, offset=sym.value, size=sym.size))
                            continue
                        local = [s for s in syms if s.name in labels and s.type == 0 and s.bind == 0]
                        if len(local) != 1:
                            out['problems'].append(
                                f'{sec}: NOBITS symbol ownership does not prove {it["name"]} '
                                f'at {h8(it["start"])} with size 0x{it["end"] - it["start"]:x}')
                            break
                        sym = local[0]
                        raw_matches = [s for s in syms if s.name == sym.name]
                        target = it['start']
                        orig_matches = [s for s in orig.symbols if s.name == sym.name and s.shndx != 0]
                        if (len(raw_matches) != 1 or sym.value != it['start'] - lo or sym.size != 0
                                or len(orig_matches) != 1 or orig_matches[0].value != target
                                or orig_matches[0].bind != 0 or orig_matches[0].type != 0
                                or orig_matches[0].size != 0):
                            out['problems'].append(
                                f'{sec}: LOCAL NOTYPE storage identity {sym.name} does not match '
                                f'the original zero-size local label at {h8(target)}')
                            break
                        local_notypes.append((it, sym))
                    if not out['problems'] or not out['problems'][-1].startswith(f'{sec}: NOBITS'):
                        for it, sym in local_notypes:
                            proof.append(dict(name=sym.name, bind='LOCAL', type='NOTYPE',
                                              input_section=csec.name, offset=sym.value, size=0,
                                              mapped_address=h8(it['start']),
                                              extent=it['end'] - it['start'],
                                              extent_basis='adjacent original section-item boundaries'))
                if out['problems'] and out['problems'][-1].startswith(f'{sec}: NOBITS'):
                    break
                detail['nobits_ownership'] = proof
                detail['covered_scaffold_items'] = [it['name'] for it in items if lo <= it['start'] < hi]
                detail['bytes'] = dict(checked=False,
                                       reason='NOBITS has no byte payload; symbol size, section placement, and linked ownership are the evidence')
                plan = dict(pieces=[dict(offset=0, length=csec.size, end=csec.size, lo=lo, hi=hi)], notes=[])
                break
            try:
                current_registry = load_registry(root)
                split_plans(root, unit, unit_dir, tu_id, elf, registry=current_registry)
                explicit_rows = [record for record in group_records if record.get('c_input_spans')]
                if explicit_rows:
                    target_pieces = []
                    for record in explicit_rows:
                        parts = record.get('c_input_spans') or []
                        target_pieces.extend(dict(offset=int(part['object_range'][0]),
                                                  length=int(part['object_range'][1]) - int(part['object_range'][0]),
                                                  end=int(part['object_range'][1]),
                                                  lo=hx(part['range'][0]), hi=hx(part['range'][1]))
                                              for part in parts)
                    if len(explicit_rows) != len(spans) or any(
                            not record.get('c_input_spans') or
                            (hx(record['c_input_spans'][0]['range'][0]),
                             hx(record['c_input_spans'][-1]['range'][1])) != span
                            for record, span in zip(explicit_rows, spans)):
                        raise CarveError(f'{sec}: explicit raw-input spans differ from compiler-derived scaffold runs')
                    for k, record in enumerate(group_records):
                        parts = record.get('c_input_spans') or []
                        raw_owners = {name for part in parts if part.get('credit', 'verified') == 'verified'
                                      for name in (part.get('symbols') or [])}
                        declared_owners = set(record.get('c_owned_symbols') or [])
                        anonymous_owner = (
                            len(parts) == 1 and parts[0].get('anonymous_emission') is True
                            and not (parts[0].get('symbols') or []) and bool(declared_owners)
                            and _anonymous_span_names_are_scaffold(
                                section_items(ctx, sec), declared_owners,
                                hx(parts[0]['range'][0]), hx(parts[0]['range'][1])))
                        if raw_owners != declared_owners and not anonymous_owner:
                            raise CarveError(
                                f'{sec}: explicit run {h8(spans[k][0])} owner names differ from raw C symbols')
                    plan = dict(pieces=target_pieces,
                                notes=['explicit target spans are tied to validated raw input ranges'])
                else:
                    explicit = split_plans(root, unit, unit_dir, tu_id, elf,
                                            registry=current_registry).get(sec)
                    if explicit is not None:
                        plan = explicit
                        if len(group_records) != len(spans) or len(plan['pieces']) != len(spans) or any(
                                (p['lo'], p['hi']) != span
                                for p, span in zip(plan['pieces'], spans)):
                            raise CarveError(f'{sec}: explicit input spans differ from compiler-derived scaffold runs')
                    else:
                        plan = plan_split(elf, csec, run_specs(items, spans, group_records),
                                          lambda va, n: va_bytes(orig, va, n),
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
            record = group_records[k] if k < len(group_records) else {}
            uncredited = set(record.get('uncredited_c_symbols') or [])
            run = dict(range=[h8(lo), h8(hi)], generated=[h8(lo), h8(lo + p['length'])],
                       generated_by=sorted(set().union(*(root_functions(sec, i) for i in g))),
                       c_owned_symbols=[items[i]['name'] for i in g if items[i]['name'] not in uncredited],
                       also_referenced_by_c=[items[i]['name'] for i in also
                                             if (lo if k else 0) <= items[i]['start'] < nxt])
            if uncredited:
                run['uncredited_c_symbols'] = sorted(uncredited)
            if record.get('c_input_spans'):
                run['c_input_spans'] = record['c_input_spans']
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
            plan = plan_split(elf, csec, run_specs(items, spans, entries), lambda va, n: va_bytes(orig, va, n))
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


def compare_bytes(elf, csec, pieces, ctx, target_pieces=None):
    """The C section with its relocations resolved at the TU's original addresses vs the original runs.

    `pieces`: the plan of `plan_split` (or, for one run, the run start as an int)."""
    if csec.type == 8:
        return dict(checked=False, reason='NOBITS has no byte payload; prove the named compiler input object and exact allocated span')
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
        mapped_section = (elf.sections[sym.shndx].name
                          if 0 < sym.shndx < len(elf.sections) else None)
        mapped_targets = (target_pieces or {}).get(mapped_section)
        if rtype != 2 or (not mapped_targets and
                          sym.shndx not in ((text.index if text else -1), csec.index)):
            unresolved += 1
            data[off:off + 4] = wants[k][pos:pos + 4]      # an external address: not checked here
            continue
        target = sym.value + struct.unpack_from('<I', data, off)[0]
        if mapped_targets:
            # A table can point to another explicit slice of its raw input
            # section, or to a different C section. Resolve against the entire
            # validated raw-to-original mapping, not only the table slice
            # whose bytes this invocation compares.
            targets = [p for p in mapped_targets if p['offset'] <= target < p['end']]
            if not targets:
                # Preserve piece_of's existing one-past-end address rule. A
                # compiler pool can address the immediately following scaffold
                # item through the end of its routed slice.
                targets = [p for p in mapped_targets if target == p['end']]
            if len(targets) != 1:
                unresolved += 1
                continue
            mapped = targets[0]
            value = mapped['lo'] + target - mapped['offset']
        elif sym.shndx == csec.index:
            try:
                j = piece_of(pieces, target, 'a pointer')
            except CarveError:
                unresolved += 1
                continue
            value = pieces[j]['lo'] + target - pieces[j]['offset']
        else:
            raw_funcs = [s for s in elf.symbols if s.shndx == text.index and s.type == 2
                         and s.value <= target < s.value + max(s.size, 1)]
            if len(raw_funcs) != 1:
                unresolved += 1
                continue
            raw_func = raw_funcs[0]
            original_funcs = [s for s in orig.symbols if s.name == raw_func.name and s.shndx != 0
                              and s.type == 2 and s.bind == raw_func.bind]
            if len(original_funcs) != 1:
                unresolved += 1
                continue
            value = original_funcs[0].value + target - raw_func.value
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
    p = sub.add_parser('split', help='write the object a TU with several runs in a section links')
    p.add_argument('--root', type=Path, required=True)
    p.add_argument('--unit', default='main')
    p.add_argument('--unit-dir', type=Path, required=True)
    p.add_argument('--tu', required=True)
    p.add_argument('--obj', type=Path, required=True)
    p.add_argument('--out', type=Path, required=True)
    p.add_argument('--registry', type=Path,
                   help='explicit data ownership registry; otherwise use the private/root default')
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
            registry = json.loads(a.registry.read_bytes()) if a.registry else None
            plans = split_plans(a.root, a.unit, a.unit_dir, a.tu, Elf(data), registry=registry)
            if not plans:
                raise CarveError(f'{a.tu} declares no section with several runs: link {a.obj} itself')
            # Older/native NOBITS planners may return a bare piece list for a
            # section, while initialized section planners return
            # {pieces, notes}. Normalize both without discarding either plan.
            plans = {sec: (p if isinstance(p, dict) else dict(pieces=p, notes=[]))
                     for sec, p in plans.items()}
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
