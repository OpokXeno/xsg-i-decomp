#!/usr/bin/env python3
"""Whole-file SHA-256 gate for TU-mode EE builds (docs/tu-build.md).

Given rebuilt file images and the original ELFs, compute the whole-file SHA-256
of every target and compare. A TU links into one or more files: main TUs into
SLUS_204.69, overlay TUs into their OVnn.OVL, and OV02 TUs into OV02.OVL *and*
SLUS_204.69 (the main ELF embeds a byte copy of the ov02 section), so `--tu`
demands every file that links the TU (the dual check).

On a mismatch every differing byte range is first split at the attribution
boundaries it crosses (section, scaffold blob, TU, function and data-piece
edges), so a range spanning two TUs is credited to both. Each piece is then
mapped, using config/tu-build.json (TU text ranges, function lists, per-section
data ownership, unit layouts and scaffold blobs) and the original ELF section
headers, to

  file offset -> section -> VA -> function -> TU (text) or data-owner candidates
  (data: the C/asm owner of the exact carve piece, else the map's anchored
  evidence, else the neighbour window), recovered VU microprogram, or scaffold blob.

A recovered VU microprogram is not a TU and not scaffolding: it is one contiguous
container in one file (config/vu-build.json, docs/vu-microcode-unit.md), so
`--program ID` gates that file and reports the differing bytes inside the container.

This is the proto compare.py logic (.work/tu-migration/proto/tools/compare.py,
ROM-style whole-file mode) driven by the manifest instead of the splat tree.
Nothing is masked or normalized: the gate is the SHA-256 of the complete file.
The mapping is a diagnostic aid; only the hash decides.

Usage:
  python3 -B tools/elf_gate.py --image main=build/main/main.bin --image ov02=build/ov02/ov02.bin
  python3 -B tools/elf_gate.py --tu ov02/tu002 --image main=... --image ov02=... --report gate.json
  python3 -B tools/elf_gate.py --program main/vu0-microcode --image main=... --report gate.json

Exit status: 0 every gated file identical; 1 at least one mismatch; 2 usage or
input error (missing image for a required target, original hash mismatch...).
"""
import argparse
import bisect
import hashlib
import json
import os
from pathlib import Path
import struct
import sys

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_MANIFEST = 'config/originals.json'
BLOCK = 4096
SHT_NOBITS = 8
SHF_ALLOC = 2


class Section:
    __slots__ = ('index', 'name', 'type', 'flags', 'addr', 'offset', 'size', 'link', 'info',
                 'align', 'entsize', 'name_off')

    def __init__(self, index, fields):
        (self.name_off, self.type, self.flags, self.addr, self.offset, self.size, self.link,
         self.info, self.align, self.entsize) = fields
        self.index = index
        self.name = ''


class Elf:
    """Minimal read-only ELF32 little-endian parser (header, program and section headers)."""

    def __init__(self, data):
        if data[:4] != b'\x7fELF' or data[4] != 1 or data[5] != 1:
            raise ValueError('not an ELF32 little-endian file')
        self.data = data
        (self.e_type, self.e_machine, self.e_version, self.entry, self.phoff, self.shoff,
         self.flags, self.ehsize, self.phentsize, self.phnum, self.shentsize, self.shnum,
         self.shstrndx) = struct.unpack_from('<HHIIIIIHHHHHH', data, 16)
        self.sections = [Section(i, struct.unpack_from('<IIIIIIIIII', data, self.shoff + i * self.shentsize))
                         for i in range(self.shnum)]
        names = self.sections[self.shstrndx]
        for section in self.sections:
            end = data.index(b'\0', names.offset + section.name_off)
            section.name = data[names.offset + section.name_off:end].decode('latin-1')

    def section(self, name):
        return next((s for s in self.sections if s.name == name), None)

    def file_sections(self):
        """Sections that occupy file bytes, in file order."""
        return sorted((s for s in self.sections if s.type != SHT_NOBITS and s.size), key=lambda s: s.offset)


def sha256_bytes(data):
    return hashlib.sha256(data).hexdigest()


def diff_ranges(a, b):
    """Half-open [start, end) ranges where a and b differ, incl. a size-mismatch tail."""
    out, start = [], None
    common = min(len(a), len(b))
    ma, mb = memoryview(a), memoryview(b)
    pos = 0
    while pos < common:
        end = min(pos + BLOCK, common)
        if ma[pos:end] == mb[pos:end]:
            if start is not None:
                out.append((start, pos))
                start = None
            pos = end
            continue
        for i in range(pos, end):
            if a[i] != b[i]:
                if start is None:
                    start = i
            elif start is not None:
                out.append((start, i))
                start = None
        pos = end
    if len(a) != len(b):
        if start is None:
            start = common
        out.append((start, max(len(a), len(b))))
    elif start is not None:
        out.append((start, common))
    return out


def hexint(value):
    return int(value, 16) if isinstance(value, str) else int(value)


class UnitIndex:
    """Manifest-driven lookup of function, TU and data owner for one unit."""

    def __init__(self, unit, unit_record, tus):
        self.unit = unit
        self.record = unit_record
        self.text = sorted(((hexint(t['text']['start']), hexint(t['text']['end']), t) for t in tus
                            if hexint(t['text']['end']) > hexint(t['text']['start'])), key=lambda x: x[0])
        self.text_starts = [x[0] for x in self.text]
        funcs = []
        for t in tus:
            for f in t.get('functions', []):
                funcs.append((hexint(f['va']), hexint(f['va']) + max(int(f['size']), 1), f['name'], t['id']))
        funcs.sort()
        self.funcs = funcs
        self.func_starts = [f[0] for f in funcs]
        self.data = {}
        for t in tus:
            for section, own in (t.get('data_ownership') or {}).items():
                self.data.setdefault(section, []).append((t['id'], own))

    def function(self, va):
        i = bisect.bisect_right(self.func_starts, va) - 1
        if i >= 0 and self.funcs[i][0] <= va < self.funcs[i][1]:
            return dict(name=self.funcs[i][2], va='0x%08x' % self.funcs[i][0], tu=self.funcs[i][3])
        return None

    def text_tu(self, va):
        i = bisect.bisect_right(self.text_starts, va) - 1
        if i >= 0 and self.text[i][0] <= va < self.text[i][1]:
            return self.text[i][2]['id']
        return None

    def family(self, va):
        """Overlay single-section units: .text/.data/.rodata/.bss family of a VA."""
        layout = self.record.get('layout')
        if not layout:
            return None
        if hexint(layout['text_start']) <= va < hexint(layout['text_end']):
            return '.text'
        for fam in layout.get('families', []):
            if hexint(fam['start']) <= va < hexint(fam['end']):
                return fam['family']
        return None

    def data_owners(self, section, va):
        """Owner candidates for a data VA: the exact carve piece first, then the map's
        anchored evidence, then the neighbour window. Each entry carries the section's
        owner, so a C-owned piece is not reported as scaffolding."""
        out = dict(carve=[], evidence=[], window=[])
        for tu_id, own in self.data.get(section, []):
            spans = [('carve', own.get('range'))]
            spans += [('carve', run['range']) for run in own.get('c_runs', []) if run['range'] != own.get('range')]
            spans += [('evidence', own.get('evidence')), ('window', own.get('window'))]
            for key, span in spans:
                if span and hexint(span[0]) <= va < hexint(span[1]):
                    # Outside its C run, a partially owned section is still scaffolding.
                    owner = own.get('owner', 'asm')
                    if key != 'carve' and own.get('partial'):
                        owner = 'asm'
                    out[key].append(dict(tu=tu_id, owner=owner))
                    break
        return {k: sorted(v, key=lambda e: e['tu']) for k, v in out.items() if v}


def resolve_manifest(root, explicit):
    """Which manifest describes the originals.

    `config/originals.json` is tracked and names the six files, their sizes and
    their SHA-256: that is all the gate needs to decide identical or not.
    `config/tu-build.json` is the project's private per-TU manifest; when it is
    present the gate also attributes every differing byte to a section, a TU, a
    function and a data owner.  A clone has the first and not the second, so the
    verdict is the same and the diagnosis is shorter.
    """
    if explicit:
        path = Path(explicit)
        return path if path.is_absolute() else root / path
    detailed = root / 'config/tu-build.json'
    return detailed if detailed.is_file() else root / DEFAULT_MANIFEST


def load_manifest(path, root):
    data = json.loads(Path(path).read_bytes())
    schema = data.get('schema')
    if schema == 'tu-build/1':
        return data, 'full'
    if schema == 'originals/1':
        game = game_dir(root)
        units = {name: dict(rec, file=str(game / rec['file']))
                 for name, rec in data['units'].items()}
        return dict(schema='tu-build/1', units=units, tus=[]), 'hashes only'
    raise ValueError(f'{path}: not a tu-build/1 or originals/1 manifest')


def game_dir(root):
    """Where your original files are; see tools/tu/toolchain.py."""
    env = os.environ.get('XENO_GAME_DIR')
    if env:
        return Path(env).expanduser()
    local = root / 'config/toolchain-local.json'
    if local.is_file():
        value = json.loads(local.read_bytes()).get('game_dir')
        if value:
            return Path(value).expanduser()
    return root / 'game/extracted-iso'


class Target:
    """One gated file: original ELF, its manifest unit record(s) and lookups."""

    def __init__(self, name, manifest, root):
        units = manifest['units']
        if name not in units:
            raise ValueError(f'unknown target {name!r}; known: {sorted(units)}')
        self.name = name
        self.record = units[name]
        path = root / self.record['file']
        self.original_path = path
        data = path.read_bytes()
        digest = sha256_bytes(data)
        if digest != self.record['sha256']:
            raise ValueError(f'original {path} sha256 {digest} != manifest {self.record["sha256"]}')
        self.original = data
        self.elf = Elf(data)
        by_unit = {}
        for t in manifest['tus']:
            by_unit.setdefault(t['unit'], []).append(t)
        self.indexes = {u: UnitIndex(u, units[u], by_unit.get(u, [])) for u in units}
        # Sections of this file that carry another unit's bytes (main embeds ov02).
        self.section_unit = {e['section']: e['unit'] for e in self.record.get('embeds', [])}
        self.blobs = sorted(((hexint(b['file_offset']), hexint(b['file_offset']) + int(b['size']), b['name'])
                             for b in self.record.get('scaffold_blobs', [])))
        # Recovered VU microprograms (config/vu-build.json).  Their container bytes are
        # not scaffolding and not part of any TU, so they get their own attribution.
        self.vu = sorted(((hexint(p['file_offset']), hexint(p['file_offset']) + int(p['size']), p['id'])
                          for p in self.record.get('vu_programs', [])))
        self.cuts = self._cuts()

    def offset_of(self, va):
        """File offset of a VA in this file, or None (NOBITS and unmapped VAs)."""
        for s in self.elf.file_sections():
            if s.flags & SHF_ALLOC and s.addr <= va < s.addr + s.size:
                return s.offset + va - s.addr
        return None

    def _cuts(self):
        """Sorted file offsets where the attribution of a byte can change.

        A differing range is split at these before it is mapped, so a range crossing a
        TU, function, section or blob boundary is credited to every part it touches
        (review finding R2) instead of only to the one it starts in.
        """
        cuts = {0, len(self.original)}
        for s in self.elf.file_sections():
            cuts.update((s.offset, s.offset + s.size))
        for start, end, _ in self.blobs:
            cuts.update((start, end))
        for start, end, _ in self.vu:
            cuts.update((start, end))
        cuts.update((self.elf.shoff, self.elf.shoff + self.elf.shnum * self.elf.shentsize,
                     max(self.elf.ehsize, self.elf.phoff + self.elf.phnum * self.elf.phentsize)))
        for unit, index in self.indexes.items():
            if unit != self.name and unit not in self.section_unit.values():
                continue
            for start, end, _ in index.text:
                cuts.update(o for o in (self.offset_of(start), self.offset_of(end)) if o is not None)
            for start, end, _, _ in index.funcs:
                cuts.update(o for o in (self.offset_of(start), self.offset_of(end)) if o is not None)
            for layout in (index.record.get('layout') or {}).get('families', []):
                cuts.update(o for o in (self.offset_of(hexint(layout['start'])),
                                        self.offset_of(hexint(layout['end']))) if o is not None)
            for entries in index.data.values():
                for _, own in entries:
                    for span in (own.get('range'), own.get('evidence'), own.get('window')):
                        if span:
                            cuts.update(o for o in (self.offset_of(hexint(span[0])),
                                                    self.offset_of(hexint(span[1]))) if o is not None)
        return sorted(c for c in cuts if 0 <= c <= len(self.original))

    def split(self, start, end):
        """Split one differing range at the attribution boundaries it crosses."""
        pieces, pos = [], start
        i = bisect.bisect_right(self.cuts, start)
        while i < len(self.cuts) and self.cuts[i] < end:
            pieces.append((pos, self.cuts[i]))
            pos = self.cuts[i]
            i += 1
        pieces.append((pos, end))
        return pieces

    def locate(self, offset):
        """Map one file offset of the original layout to section/VA/function/TU."""
        where = dict(file_offset='0x%X' % offset)
        blob = next((b[2] for b in self.blobs if b[0] <= offset < b[1]), None)
        if blob:
            where['scaffold_blob'] = blob
        program = next((v[2] for v in self.vu if v[0] <= offset < v[1]), None)
        if program:
            where['vu_program'] = program
        elf = self.elf
        if offset < max(elf.ehsize, elf.phoff + elf.phnum * elf.phentsize):
            where['region'] = 'elf_header'
        if elf.shoff <= offset < elf.shoff + elf.shnum * elf.shentsize:
            where['region'] = 'section_headers'
            return where
        section = next((s for s in elf.file_sections() if s.offset <= offset < s.offset + s.size), None)
        if section is None:
            where.setdefault('region', 'unowned_file_bytes')
            return where
        where['section'] = section.name
        if not section.flags & SHF_ALLOC:
            return where
        va = section.addr + offset - section.offset
        where['va'] = '0x%08x' % va
        unit = self.section_unit.get(section.name, self.name)
        index = self.indexes[unit]
        where['unit'] = unit
        family = section.name if unit == 'main' and section.name.startswith('.') else index.family(va)
        if family:
            where['family'] = family
        function = index.function(va)
        if function:
            where['function'] = function['name']
            where['function_va'] = function['va']
        if family == '.text' or function:
            tu = function['tu'] if function else index.text_tu(va)
            if tu is None:
                tu = index.text_tu(va)
            where['tu'] = tu
        elif family:
            owners = index.data_owners(family, va)
            if owners:
                where['data_owner_candidates'] = owners
                owned = owners.get('carve') or owners.get('evidence') or []
                if len(owned) == 1:
                    where['data_owner'] = owned[0]['tu']
                    where['data_owner_kind'] = owned[0]['owner']
        return where


def gate_target(target, built_path, max_ranges):
    built = built_path.read_bytes()
    result = dict(original=dict(path=target.record['file'], sha256=target.record['sha256'],
                                size=len(target.original)),
                  built=dict(path=str(built_path), sha256=sha256_bytes(built), size=len(built)))
    result['identical'] = result['built']['sha256'] == result['original']['sha256']
    if result['identical']:
        return result
    ranges = diff_ranges(target.original, built)
    by_section, by_tu, by_data_owner, by_function, by_vu_program = {}, {}, {}, {}, {}
    detail = []
    pieces = [piece for a, b in ranges for piece in target.split(a, b)]
    for n, (a, b) in enumerate(pieces):
        where = target.locate(a)
        key = where.get('section') or where.get('region') or 'unknown'
        stats = by_section.setdefault(key, dict(pieces=0, bytes=0))
        stats['pieces'] += 1
        stats['bytes'] += b - a
        if where.get('tu'):
            tstats = by_tu.setdefault(where['tu'], dict(pieces=0, bytes=0))
            tstats['pieces'] += 1
            tstats['bytes'] += b - a
        for kind in ('carve', 'evidence'):
            for entry in (where.get('data_owner_candidates') or {}).get(kind, []):
                dstats = by_data_owner.setdefault(entry['tu'], dict(pieces=0, bytes=0, c_pieces=0, c_bytes=0))
                dstats['pieces'] += 1
                dstats['bytes'] += b - a
                if entry['owner'] == 'c':
                    dstats['c_pieces'] += 1
                    dstats['c_bytes'] += b - a
            if (where.get('data_owner_candidates') or {}).get(kind):
                break
        if where.get('vu_program'):
            vstats = by_vu_program.setdefault(where['vu_program'], dict(pieces=0, bytes=0))
            vstats['pieces'] += 1
            vstats['bytes'] += b - a
        if where.get('function'):
            fkey = '%s:%s' % (where.get('unit'), where['function'])
            by_function[fkey] = by_function.get(fkey, 0) + 1
        if n < max_ranges:
            entry = dict(where, length=b - a)
            entry['original'] = target.original[a:min(b, a + 16)].hex()
            entry['rebuilt'] = built[a:min(b, a + 16)].hex()
            detail.append(entry)
    result['diff'] = dict(range_count=len(ranges), piece_count=len(pieces),
                          byte_count=sum(b - a for a, b in ranges),
                          first_pieces=detail, truncated=len(pieces) > max_ranges,
                          by_section=dict(sorted(by_section.items())),
                          by_tu=dict(sorted(by_tu.items())),
                          by_data_owner=dict(sorted(by_data_owner.items())),
                          by_vu_program=dict(sorted(by_vu_program.items())),
                          functions=len(by_function))
    return result


def attribute_programs(report, program_ids):
    """Per requested VU microprogram: the differing bytes inside its container.

    A VU program is not a TU: it owns one contiguous container in one file, so the
    attribution is the container, not a text/data pair.
    """
    out = {}
    for program_id in program_ids:
        entry = {}
        for name, target in report['targets'].items():
            diff = target.get('diff')
            if not diff:
                entry[name] = dict(identical=True)
                continue
            entry[name] = dict(identical=False,
                               container=diff['by_vu_program'].get(program_id, dict(pieces=0, bytes=0)),
                               total_ranges=diff['range_count'], total_pieces=diff['piece_count'])
        out[program_id] = entry
    return out


def attribute(report, tu_ids):
    """Per requested TU: differing text ranges it owns and data ranges inside its evidence spans."""
    out = {}
    for tu_id in tu_ids:
        entry = {}
        for name, target in report['targets'].items():
            diff = target.get('diff')
            if not diff:
                entry[name] = dict(identical=True)
                continue
            entry[name] = dict(identical=False, text=diff['by_tu'].get(tu_id, dict(pieces=0, bytes=0)),
                               data=diff['by_data_owner'].get(tu_id, dict(pieces=0, bytes=0, c_pieces=0, c_bytes=0)),
                               total_ranges=diff['range_count'], total_pieces=diff['piece_count'])
        out[tu_id] = entry
    return out


def parse_images(values):
    images = {}
    for value in values:
        name, sep, path = value.partition('=')
        if not sep or not name or not path:
            raise ValueError(f'--image expects TARGET=PATH, got {value!r}')
        if name in images:
            raise ValueError(f'target {name!r} given twice')
        images[name] = Path(path)
    return images


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--manifest', default=None,
                        help='manifest of the originals (default: config/tu-build.json when this '
                             'tree has it, else the tracked config/originals.json)')
    parser.add_argument('--root', type=Path, default=ROOT, help='repository root for original paths')
    parser.add_argument('--image', action='append', default=[], metavar='TARGET=PATH',
                        help='rebuilt whole-file image of a target (main, ov01, ov02, ov10, ov11, ov12)')
    parser.add_argument('--tu', action='append', default=[], metavar='ID',
                        help='gate every file that links this TU (repeatable); missing images are an error')
    parser.add_argument('--program', action='append', default=[], metavar='ID',
                        help='gate the file that holds this VU microprogram (repeatable); '
                             'missing images are an error')
    parser.add_argument("--max-ranges", type=int, default=64, help="differing pieces listed per target")
    parser.add_argument('--report', type=Path, help='write the JSON report here as well as to stdout')
    args = parser.parse_args(argv)

    root = args.root.resolve()
    manifest_path = resolve_manifest(root, args.manifest)
    try:
        manifest, attribution = load_manifest(manifest_path, root)
        images = parse_images(args.image)
        by_id = {t['id']: t for t in manifest['tus']}
        by_program = {p['id']: unit for unit, rec in manifest.get('units', {}).items()
                      for p in rec.get('vu_programs', [])}
        required = []
        for tu_id in args.tu:
            if tu_id not in by_id:
                raise ValueError(f'unknown TU id {tu_id!r}')
            for target in by_id[tu_id]['linked_into']:
                if target not in required:
                    required.append(target)
        for program_id in args.program:
            if program_id not in by_program:
                raise ValueError(f'unknown VU program id {program_id!r}')
            # A VU program's container lives in exactly one file: the unit that holds it.
            if by_program[program_id] not in required:
                required.append(by_program[program_id])
        missing = [t for t in required if t not in images]
        if missing:
            raise ValueError(f'TU(s) {args.tu} and VU program(s) {args.program} link into '
                             f'{required}; no --image for {missing}')
        if not images:
            raise ValueError('no --image given')
        for name, path in images.items():
            if not path.is_file():
                raise ValueError(f'image for {name} not found: {path}')
        order = required + sorted(t for t in images if t not in required)
        report = dict(schema='elf-gate/1',
                      manifest=dict(path=str(manifest_path.relative_to(root)) if manifest_path.is_relative_to(root)
                                    else str(manifest_path), sha256=sha256_bytes(manifest_path.read_bytes())),
                      attribution=attribution,
                      tus=list(args.tu), programs=list(args.program),
                      required_targets=required, targets={})
        if attribution != 'full' and (args.tu or args.program):
            raise ValueError('--tu/--program needs config/tu-build.json, the per-TU manifest, '
                             'which this tree does not have; the whole-file gate still runs '
                             'without it')
        for name in order:
            target = Target(name, manifest, root)
            report['targets'][name] = gate_target(target, images[name], args.max_ranges)
    except (ValueError, OSError, KeyError) as exc:
        print(json.dumps(dict(schema='elf-gate/1', result='error', error=str(exc))))
        return 2
    identical = all(t['identical'] for t in report['targets'].values())
    if args.tu:
        report['tu_attribution'] = attribute(report, args.tu)
    if args.program:
        report['vu_program_attribution'] = attribute_programs(report, args.program)
    report['result'] = 'pass' if identical else 'fail'
    text = json.dumps(report, indent=2) + '\n'
    if args.report:
        args.report.write_text(text)
    sys.stdout.write(text)
    return 0 if identical else 1


if __name__ == '__main__':
    sys.exit(main())
