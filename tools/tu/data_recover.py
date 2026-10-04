#!/usr/bin/env python3
"""Generate typed C DATA candidates, then use the existing linked gates.

Planning reads only explicit inventory, TU-manifest and source paths. Preparation
is additive and private. Compiler probes are diagnostics, never recovery credit;
`gate` delegates the whole-file and compiler-provenance verdicts to elf_gate.py
and data_gate.py. No opcode arrays, source ASM, object patches or compiler flag
changes are generated.
"""
import argparse
import bisect
import collections
import hashlib
import json
import math
import os
from pathlib import Path
import re
import signal
import sqlite3
import struct
import subprocess
import sys
import time

sys.dont_write_bytecode = True
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import data_carve as dc
import data_types as dt
from data_gate import emitted_storage
from elfinfo import Elf
from toolchain import Toolchain

ROOT = HERE.parents[1]
SECTIONS = {'.data', '.rodata', '.sdata', '.lit4', '.bss', '.sbss'}
UNITS = ('main', 'ov01', 'ov02', 'ov10', 'ov11', 'ov12')
IDENT = re.compile(r'^[A-Za-z_]\w*$')
DECL = re.compile(r'\bextern\s+([^;{}=()]+);')
DECLARATOR = re.compile(r'\s*(\**\s*)([A-Za-z_]\w*)\s*((?:\[[^\]]*\]\s*)*)\s*$')
LOCAL_INCLUDE = re.compile(r'^\s*#\s*include\s+"([^"]+)"', re.M)
# Only ABI-independent project scalar widths; plain long and enum are excluded.
SCALARS = {
    'char': ('b', 1), 'signed char': ('b', 1), 'unsigned char': ('B', 1),
    's8': ('b', 1), 'u8': ('B', 1), 'short': ('h', 2), 'short int': ('h', 2),
    'unsigned short': ('H', 2), 'unsigned short int': ('H', 2),
    's16': ('h', 2), 'u16': ('H', 2), 'int': ('i', 4), 'signed int': ('i', 4),
    'unsigned': ('I', 4), 'unsigned int': ('I', 4), 's32': ('i', 4), 'u32': ('I', 4),
    'long long': ('q', 8), 'unsigned long long': ('Q', 8), 's64': ('q', 8), 'u64': ('Q', 8),
    'float': ('f', 4), 'double': ('d', 8),
}


def require(value, message):
    if not value:
        raise ValueError(message)


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def read_json(path):
    return json.loads(Path(path).read_text())


def clean_type(value):
    return ' '.join(value.strip().split())


def base_type(value):
    return clean_type(re.sub(r'\b(?:const|volatile|signed)\b', '', value))


def microprogram_address_tables(code):
    """Recognize address metadata consumed by MSCAL, not VU instruction bodies.

    Require the complete local C idiom: a row of u32 array pointers, every
    column read through a scalar subscript, and that scalar shifted into the
    MSCAL immediate. A symbol's name alone supplies no such evidence.
    """
    result = set()
    macros = re.findall(r'^\s*#\s*define\s+(\w+)\s+0x14000000(?:[uUlL]*)\b', code, re.M)
    rows = re.findall(r'typedef\s+struct\s+\w*\s*\{\s*u32\s*\*\s*(\w+)'
                      r'\s*\[(\d+)\]\s*;\s*\}\s*(\w+)\s*;', code)
    for member, width, row_type in rows:
        tables = re.findall(r'\b' + re.escape(row_type) + r'\s+(\w+)\s*\[[^\]]+\]'
                            r'\s*=\s*\{(.*?)\}\s*;', code, re.S)
        for table, initializer in tables:
            columns = set()
            pointers = re.findall(r'\b(\w+)\s*=\s*' + re.escape(table) +
                                  r'\s*\[[^\]]+\]\s*\.\s*' + re.escape(member) +
                                  r'\s*\[(\d+)\]\s*;', code)
            for pointer, column in pointers:
                if not re.search(r'\bu32\s*\*\s*' + re.escape(pointer) + r'\s*;', code):
                    continue
                scalars = re.findall(r'\b(\w+)\s*=\s*' + re.escape(pointer) +
                                     r'\s*\[[^\]]+\]\s*;', code)
                for scalar in scalars:
                    if not re.search(r'\bu32\s+' + re.escape(scalar) + r'\s*;', code):
                        continue
                    for macro in macros:
                        call = (r'\bsceVif1PkAddCode\s*\([^,;]+,\s*\(\s*' +
                                re.escape(scalar) + r'\s*>>\s*3\s*\)\s*\|\s*' +
                                re.escape(macro) + r'\s*\)')
                        if re.search(call, code):
                            columns.add(int(column))
            if columns != set(range(int(width))):
                continue
            if not re.fullmatch(r'[\s{},A-Za-z_0-9]+', initializer):
                continue
            result.update(re.findall(r'\b[A-Za-z_]\w*\b', initializer))
    return result


class Lease:
    """Validate the canonical queue and literal destinations before every write."""
    def __init__(self, root, task=None, owner=None, token_file=None):
        self.root = root.resolve()
        self.task, self.owner = task, owner
        self.token = Path(token_file).read_text().strip() if token_file else None

    def check(self, paths):
        require(self.task and self.owner and self.token, 'writes require --task, --owner and --token-file')
        db = self.root / '.work/queue.sqlite3'
        with sqlite3.connect(f'file:{db}?mode=ro', uri=True, timeout=10) as conn:
            row = conn.execute('SELECT status,owner,token,lease_until FROM tasks WHERE id=?', (self.task,)).fetchone()
            require(row and row[:3] == ('claimed', self.owner, self.token) and row[3] > time.time(), 'no live matching queue lease')
            held = {r[0] for r in conn.execute('SELECT resource FROM locks WHERE task_id=?', (self.task,))}
        for path in paths:
            absolute = Path(path).resolve()
            choices = {'file:' + str(absolute)}
            if absolute.is_relative_to(self.root):
                choices.add('file:' + str(absolute.relative_to(self.root)))
            require(choices & held, f'missing literal reservation for {absolute}')

    def json(self, path, value):
        self.check([path])
        Path(path).parent.mkdir(parents=True, exist_ok=True)
        Path(path).write_text(json.dumps(value, indent=2) + '\n')

    def reserve_generated_headers(self, paths):
        """Add exact generated-header destinations to this live integration.

        Type propagation can create an owning-TU header which did not exist
        when the batch was planned. Reserve each literal name atomically and
        audit the expansion; never take another task's reservation.
        """
        self.check([])
        resources=[]
        for path in paths:
            absolute=Path(path).resolve()
            require(absolute.is_relative_to(self.root/'include') and absolute.suffix=='.h',
                    'dynamic reservation is restricted to generated include headers')
            resources.append('file:'+str(absolute.relative_to(self.root)))
        if not resources:
            return
        import workqueue
        with sqlite3.connect(f'file:{self.root/".work/queue.sqlite3"}?mode=rw',uri=True,timeout=10) as conn:
            conn.execute('BEGIN IMMEDIATE')
            row=conn.execute('SELECT status,owner,token,lease_until FROM tasks WHERE id=?',(self.task,)).fetchone()
            require(row and row[:3]==('claimed',self.owner,self.token) and row[3]>time.time(),
                    'generated header reservation requires a live integration claim')
            require(conn.execute('SELECT 1 FROM locks WHERE task_id=? AND resource=?',
                    (self.task,'baseline:integration')).fetchone(), 'generated header reservation needs baseline:integration')
            added=[]
            for resource in sorted(set(resources)):
                held=conn.execute('SELECT task_id FROM locks WHERE resource=?',(resource,)).fetchone()
                require(not held or held[0]==self.task, 'generated header is reserved by another task: '+resource)
                if not held:
                    conn.execute('INSERT OR IGNORE INTO resources(task_id,resource) VALUES(?,?)',(self.task,resource))
                    conn.execute('INSERT INTO locks(resource,task_id) VALUES(?,?)',(resource,self.task))
                    added.append(resource)
            if added:
                workqueue.audit(conn,time.time(),'reserve-generated-headers',self.task,self.owner,resources=added)
        self.check(paths)


def declaration_index(root, tus):
    """Follow bounded local/header includes; do not scan generated work trees."""
    read = {}
    parsed = {}

    def parse(path):
        path = path.resolve()
        if path in parsed:
            return parsed[path]
        require(path.is_relative_to(root), 'source include escapes repository')
        require(len(read) < 3000, 'bounded source/header inventory exceeded')
        text = path.read_text(errors='surrogateescape')
        read[path] = sha(path)
        code = re.sub(r'/\*.*?\*/|//[^\n]*', '', text, flags=re.S)
        declarations = collections.defaultdict(list)
        typedefs = collections.defaultdict(set)
        for ctype, name in re.findall(r'\btypedef\s+([^;{}*()\[\]]+?)\s+([A-Za-z_]\w*)\s*;', code):
            typedefs[name].add(clean_type(ctype))
        for body in DECL.findall(code):
            parts = body.split(',')
            first = re.fullmatch(r'\s*(.+?)\s+(\**\s*[A-Za-z_]\w*\s*(?:\[[^\]]*\]\s*)*)\s*', parts[0])
            if not first:
                continue
            ctype = first[1].strip()
            if '*' in ctype:
                ctype, stars = ctype.split('*', 1)
                parts[0] = '*' + stars + first[2]
            else:
                parts[0] = first[2]
            for part in parts:
                m = DECLARATOR.fullmatch(part)
                if m:
                    declarations[m[2]].append(dict(type=clean_type(ctype + ' ' + m[1]), dimensions=m[3].strip(), path=str(path.relative_to(root))))
        for definition in re.finditer(
                r'^\s*(?:static\s+)?((?:const\s+)?(?:char|unsigned\s+char|signed\s+char|u8|s8))'
                r'\s+(\w+)\s*((?:\[[^\]]*\]\s*)+)\s*=\s*("?)', code, re.M):
            ctype, name, bounds, string = definition.groups()
            declarations[name].append(dict(type=clean_type(ctype),dimensions=bounds.strip(),
                 path=str(path.relative_to(root)),source_definition=True,string_definition=bool(string)))
        for name in microprogram_address_tables(code):
            for declaration in declarations.get(name, []):
                if declaration['type'] == 'u32' and declaration['dimensions']:
                    declaration.update(data_role='vu_microprogram_byte_addresses',
                                       basis='C indexes this address table and shifts the scalar into the VIF MSCAL immediate')
        uv_table = re.search(r'\bUvClutEntry\s+uv_clut\s*\[\d+\]\s*=\s*\{(.*?)\}\s*;', code, re.S)
        uv_reader = re.search(r'INCLUDE_ASM\("([^"]+)",\s*subPrintSprite\)', code)
        if uv_table and uv_reader and re.search(
                r'const\s+unsigned\s+char\s*\*\s*uv\s*;\s*int\s+clut\s*;', code):
            unit = path.relative_to(root).parts[1]
            assembly = root/'build'/unit/uv_reader[1]/'subPrintSprite.s'
            if assembly.is_file():
                reader = assembly.read_text()
                # Original reader indexes eight-byte records, loads four
                # halfwords, and adds width/height to the UV coordinates.
                if all(re.search(pattern, reader) for pattern in (
                        r'sll\s+\$2,\s*\$2,\s*3',
                        r'lh\s+\$8,\s*0x0\(\$2\)', r'lh\s+\$3,\s*0x2\(\$2\)',
                        r'lh\s+\$12,\s*0x4\(\$2\)', r'lh\s+\$13,\s*0x6\(\$2\)',
                        r'addu\s+\$8,\s*\$8,\s*\$12', r'addu\s+\$3,\s*\$3,\s*\$13')):
                    read[assembly] = sha(assembly)
                    for name in re.findall(r'\{\s*(\w+)\s*,', uv_table[1]):
                        for declaration in declarations.get(name, []):
                            declaration.update(data_role='sprite_uv_rectangles',
                                               basis='original sprite reader indexes u/v/width/height halfwords in eight-byte records',
                                               format_reader=str(assembly.relative_to(root)))
        includes = []
        for include in LOCAL_INCLUDE.findall(code):
            require('..' not in Path(include).parts, f'unsupported parent include {include}')
            for candidate in (path.parent / include, root / 'include' / include):
                if candidate.is_file():
                    includes.append(candidate.resolve())
                    break
        parsed[path] = declarations, includes, typedefs
        return parsed[path]

    # An extern is evidence only in the owning TU's actual include closure.
    # Equal names/addresses in separate overlays never merge their types.
    indexes, aliases = {}, {}
    for tu_id, tu in tus.items():
        declarations = collections.defaultdict(list)
        typedefs = collections.defaultdict(set)
        pending, visited = [root / tu['path']], set()
        while pending:
            path = pending.pop().resolve()
            if path in visited or not path.is_file():
                continue
            visited.add(path)
            own, includes, types = parse(path)
            for name, records in own.items():
                declarations[name].extend(records)
            pending.extend(includes)
            for name, choices in types.items():
                typedefs[name].update(choices)
        indexes[tu_id] = declarations
        aliases[tu_id] = typedefs
    return indexes, read, aliases


def probe_typedefs(ctype, aliases):
    """Carry evidenced simple typedefs into the separate compiler probe."""
    result, active = [], set()

    def visit(name):
        choices = aliases.get(name, set())
        if not choices:
            return
        require(len(choices) == 1 and name not in active, 'divergent or recursive scalar typedef')
        active.add(name)
        base = next(iter(choices))
        for token in re.findall(r'\b[A-Za-z_]\w*\b', base):
            visit(token)
        declaration = f'typedef {base} {name};'
        if declaration not in result:
            result.append(declaration)
        active.remove(name)

    for token in re.findall(r'\b[A-Za-z_]\w*\b', ctype):
        visit(token)
    return result


def literal(payload):
    out = []
    escapes = {9: '\\t', 10: '\\n', 13: '\\r', 34: '\\"', 63: '\\?', 92: '\\\\'}
    for value in payload:
        out.append(escapes.get(value, chr(value) if 32 <= value < 127 else f'\\{value:03o}'))
    # Three-digit octal escapes cannot swallow the next hexadecimal character.
    return '"' + ''.join(out) + '"'


def text_payload(payload, minimum=1):
    """A single terminated printable text value, followed only by zero alignment."""
    require(b'\0' in payload, 'text has no terminator')
    text, rest = payload.split(b'\0', 1)
    require(len(text) >= minimum and not rest.strip(b'\0'), 'not a single text value plus zero alignment')
    try:
        decoded = text.decode('ascii')
    except UnicodeDecodeError:
        try:
            decoded = text.decode('euc_jp')
        except UnicodeDecodeError:
            raise ValueError('text does not have a supported printable encoding') from None
    require(all(c.isprintable() or c in '\n\r\t' for c in decoded) and any(c.isalnum() for c in decoded), 'payload is not printable text')
    return text


def numeric_literal(value, ctype, fmt):
    if fmt in ('f', 'd'):
        require(math.isfinite(value), 'non-finite floating value needs a source hypothesis')
        text = format(value, '.17g')
        if '.' not in text and 'e' not in text:
            text += '.0'
        return text + ('f' if fmt == 'f' else '')
    if fmt == 'q' and value == -(1 << 63):
        return '(-9223372036854775807LL - 1LL)'
    suffix = 'ULL' if fmt == 'Q' else 'LL' if fmt == 'q' else 'U' if fmt == 'I' else ''
    return str(value) + suffix


def dimensions(decl, extent, width):
    dims = re.findall(r'\[([^\]]*)\]', decl['dimensions'])
    require(len(dims) <= 1, 'multidimensional/record data needs its layout')
    require(extent % width == 0, 'object extent is not a whole element count')
    count = extent // width
    if not dims:
        require(count == 1, 'scalar declaration does not cover the original object')
        return '', count
    if dims[0].strip():
        try:
            declared = int(dims[0], 0)
        except ValueError:
            raise ValueError('array bound is not an explicit integer') from None
        require(declared == count, 'declared array bound differs from original extent')
    return f'[{count}]', count


def original_local(original, name, address):
    """Preserve compiler-local labels renamed by splat (e.g. i1.52)."""
    identities=[s for s in original.symbols if s.value==address and s.shndx!=0 and
                (s.name==name or (s.type==1 and re.sub(r'[^A-Za-z_0-9]', '_', s.name)==name))]
    require(not identities or len({s.bind for s in identities})==1,
            'original symbol aliases have conflicting bindings')
    return bool(identities and identities[0].bind==0)


def definition(row, block, original, declarations):
    names = row['names']
    require(len(names) == 1 and IDENT.fullmatch(names[0]), 'aliases or non-C symbol names need an ownership mapping')
    name = names[0]
    address_metadata = [d for d in declarations.get(name, [])
                        if d.get('data_role') == 'vu_microprogram_byte_addresses']
    require(not re.search(r'ucode|microcode', name, re.I) or address_metadata,
            'microcode-named object needs address-table use evidence or the VU/DVP recovery route')
    choices = {(d['type'], d['dimensions']) for d in declarations.get(name, [])}
    require(choices and len({t for t, dims in choices}) == 1, 'missing or divergent typed C declarations')
    explicit_dims = {d for t, d in choices if d not in ('[]', '')}
    require(len(explicit_dims) <= 1 and len({bool(d) for t, d in choices}) == 1, 'incompatible scalar/array declarations')
    ctype = next(iter(choices))[0]
    dims = next(iter(explicit_dims)) if explicit_dims else next(iter(choices))[1]
    decl = dict(type=ctype, dimensions=dims)
    address, end = dc.hx(row['address']), dc.hx(row['end'])
    extent = end - address
    require(0 < extent <= 65536, 'extent exceeds bounded automatic object size')
    text = re.sub(r'/\*.*?\*/', '', '\n'.join(block['lines']), flags=re.S)
    directives = re.findall(r'^\s*\.(\w+)\s+([^\n]*)', text, re.M)
    values = [(kind, value.split('#', 1)[0].strip()) for kind, value in directives
              if kind not in {'align', 'balign', 'p2align', 'size', 'type', 'globl', 'global', 'set'}]
    kinds = {kind for kind, value in values}
    require(kinds, 'empty data item')
    storage = 'static ' if original_local(original,name,address) else ''
    provenance = address_metadata[0] if address_metadata else declarations[name][0]
    prefix = f'{storage}{ctype} {name}'
    character = base_type(ctype) in {'char', 'unsigned char', 'u8', 's8'} and '*' not in ctype and dims
    payload = dc.va_bytes(original, address, extent)
    if provenance.get('data_role') in {'menu_leaf_rows', 'menu_node'}:
        suffix, count = dimensions(decl, extent, provenance['record_size'])
        return prefix + suffix + ' = ' + provenance['initializer'] + ';', 'record_array', provenance
    if provenance.get('data_role') == 'class_descriptor_storage':
        require(row['section'] in {'.bss', '.sbss'} and kinds <= {'space', 'zero'} and
                extent == provenance['record_size'], 'class descriptor storage has a different original extent')
        return prefix + ';', 'storage', provenance
    if provenance.get('data_role') == 'resource_record_array':
        record = provenance['resource_record']
        suffix, count = dimensions(decl, extent, 12)
        require(count == len(record['entries']), 'resource record count differs from original extent')
        entries = ['    { ' + ', '.join(target['name'] if target else '0' for target in entry) + ' }'
                   for entry in record['entries']]
        declarations_needed = []
        for target in record['dependencies']:
            records = declarations.get(target['name'], [])
            require(records, 'resource target lacks its propagated C type')
            declarations_needed.append(f"extern {records[0]['type']} {target['name']}{records[0]['dimensions']};")
        evidence = dict(provenance, probe_typedefs=[record['typedef']],
                        source_replacements=record['source_replacements'],
                        pointer_declarations=list(dict.fromkeys(declarations_needed)))
        return prefix + suffix + ' = {\n' + ',\n'.join(entries) + '\n};', 'record_array', evidence
    if provenance.get('data_role') == 'sprite_uv_rectangles':
        require(character and payload is not None, 'UV format requires original byte storage')
        dimensions(decl, extent, 1)
        source, macro = dt.sprite_uv_definition(prefix, payload)
        evidence = dict(provenance, probe_typedefs=[macro], source_prelude=[macro])
        return source, 'byte_records', evidence
    if character and payload is not None and payload.endswith(b'\0') and dt.encoded_text(payload):
        payload = dc.va_bytes(original, address, extent)
        require(payload is not None, 'string is not inside original initialized data')
        # Font control parameters include zero bytes inside a text value.
        # The explicit character type and readable text establish this format;
        # preserve every byte rather than treating the first zero as its end.
        suffix, count = dimensions(decl, extent, 1)
        return prefix + suffix + ' = ' + literal(payload.rstrip(b'\0')) + ';', 'string', provenance
    scalar = SCALARS.get(clean_type(re.sub(r'\b(?:const|volatile)\b', '', ctype)))
    if kinds <= {'space', 'zero'} and row['section'] in {'.bss', '.sbss'}:
        require(scalar and '*' not in ctype, 'uninitialized storage needs a scalar/array type and NOBITS section')
        suffix, count = dimensions(decl, extent, scalar[1])
        return prefix + suffix + ';', 'storage', provenance
    if '*' in ctype:
        require(ctype.count('*') == 1 and kinds <= {'word', 'long', '4byte'}, 'pointer table needs a single-pointer type and word directives')
        suffix, count = dimensions(decl, extent, 4)
        require(len(values) == count, 'pointer table has unexplained alignment or mixed payload')
        initializers = []
        for kind, value in values:
            if value in {'0', '0x0', '0x00000000'}:
                initializers.append('0')
                continue
            require(IDENT.fullmatch(value), 'pointer addends/numeric addresses need a pointee-layout hypothesis')
            target = declarations.get(value, [])
            require(target and all(d['dimensions'] and base_type(d['type']) == base_type(ctype.replace('*', '')) for d in target), 'pointer target lacks a compatible declared array type')
            initializers.append(value)
        provenance = dict(provenance, pointer_declarations=[f"extern {declarations[v][0]['type']} {v}{declarations[v][0]['dimensions']};" for v in set(initializers) if v != '0'])
        value = '{ ' + ', '.join(initializers) + ' }' if suffix else initializers[0]
        return prefix + suffix + ' = ' + value + ';', 'pointer_array', provenance
    require(scalar and '*' not in ctype, 'numeric block needs an explicit supported scalar/array type')
    # Disassembler spelling does not change an evidenced C scalar type.
    # Numeric arrays may be printed partly as strings or zero fills; decode
    # their elements from the original ELF and verify compiler emission later.
    numeric_directives = {'word', 'long', 'short', 'half', 'hword', 'byte', 'dword',
                          'quad', 'float', 'double', '2byte', '4byte', '8byte'}
    require(kinds <= numeric_directives | {'ascii', 'asciz', 'string', 'space', 'zero'},
            'mixed payload needs a source layout')
    symbolic = any(re.search(r'\b[A-Za-z_]\w*\b', re.sub(r'0[xX][0-9A-Fa-f]+|[eE][+-]?\d+', '0', value))
                   for kind, value in values if kind in numeric_directives)
    if symbolic:
        require(scalar[1] == 4 and payload is not None and len(payload) % 4 == 0,
                'symbolic words need a proven numeric layout')
        words = struct.unpack('<' + 'I'*(len(payload)//4), payload)
        require(not any(s.flags & 2 and s.addr <= w < s.addr+s.size
                        for w in words for s in original.sections),
                'numeric declaration contains original-image addresses needing a pointee hypothesis')
        provenance = dict(provenance, disassembler_association='words are typed numbers outside every original allocated section')
    fmt, width = scalar
    suffix, count = dimensions(decl, extent, width)
    require(not suffix or width >= 2, 'numeric byte arrays are not automatically substituted for binary payloads')
    payload = dc.va_bytes(original, address, extent)
    require(payload is not None, 'numeric object is not in original initialized data')
    numbers = struct.unpack('<' + fmt * count, payload)
    entries = [numeric_literal(v, ctype, fmt) for v in numbers]
    value = '{ ' + ', '.join(entries) + ' }' if suffix else entries[0]
    return prefix + suffix + ' = ' + value + ';', 'numeric', provenance


def scaffold_map(root, tus, inputs):
    """Original piece bounds and current C allocations from explicit manifests."""
    def load(path):
        inputs[path] = sha(path)
        return read_json(path)

    original = load(root / 'build/main/tu-manifest.json')
    native = load(root / 'build/main/tu-manifest.carved.json')
    registry = load(root / 'config/objects/data-carves.json')
    pieces, covered = {}, collections.defaultdict(list)
    for tu in original['tus']:
        if tu['id'] not in tus:
            continue
        for section, value in tu['sections'].items():
            if section not in SECTIONS:
                continue
            for piece in value.get('pieces') or [dict(name=tu['name'], start=value['start'], end=value['end'])]:
                path = dc.main_piece_file(root / 'build/main', piece, section).resolve()
                pieces[path] = dict(tu=tu['id'], unit='main', section=section,
                                    start=dc.hx(piece['start']), end=dc.hx(piece['end']))
    by_name = {t['name']: t for t in native['tus']}
    for section, value in native['sections'].items():
        for run in value.get('order', []):
            if run.get('c_split') or by_name[run['tu']].get('owners', {}).get(section) == 'c':
                covered[('main', section)].append((dc.hx(run['start']), dc.hx(run['end'])))
        for run in (value.get('common_tail') or {}).get('native_common_pieces', []):
            if run.get('kind') == 'c':
                covered[('main', section)].append((dc.hx(run['start']), dc.hx(run['end'])))
    for unit in UNITS[1:]:
        layout = load(root / 'build' / unit / 'layout.json')
        owners = {t['name']: t for t in tus.values() if t['unit'] == unit}
        for section, family in layout['families'].items():
            if section not in SECTIONS:
                continue
            for name, start, end in family:
                tu = owners.get(name.split('/')[-1])
                if tu:
                    path = (root / 'build' / unit / 'asm/data' / (name + section + '.s')).resolve()
                    pieces[path] = dict(tu=tu['id'], unit=unit, section=section,
                                        start=dc.hx(start), end=dc.hx(end))
    for tu_id, sections in registry['tus'].items():
        unit = tu_id.split('/')[0]
        if unit != 'main':
            for section, runs in sections.items():
                covered[(unit, section)].extend(tuple(map(dc.hx, r['range'])) for r in runs)
    return pieces, covered, native, registry


def function_inventory(root, tus, inputs, metadata, declarations=None):
    """Find DATA reachable from actual compiler-emitted, ledger C functions.

    Only manifest-listed files are read. Original symbolic DATA and native raw
    input relocations form the graph; DATA definitions are never graph roots.
    """
    pieces, covered, native, registry = metadata
    if declarations is None:
        declarations, type_inputs, _ = declaration_index(root,tus)
        inputs.update(type_inputs)
    locations, labels = {}, collections.defaultdict(set)
    graph = collections.defaultdict(set)
    for path, piece in sorted(pieces.items()):
        inputs[path] = sha(path)
        _, items = dc.piece_items(path, piece['start'], piece['end'])
        for item in items:
            if not item['labels'] or 'unreferenced pad' in '\n'.join(item['block']['lines']):
                continue
            key = (piece['unit'], item['start'])
            require(key not in locations, 'overlapping original DATA piece identities')
            text = re.sub(r'/\*.*?\*/', '', '\n'.join(item['block']['lines']), flags=re.S)
            refs = set()
            for operands in re.findall(r'^\s*\.(?:word|long|short|half|hword|byte|dword|quad|2byte|4byte|8byte)\s+([^\n]*)', text, re.M):
                refs.update(re.findall(r'\b[A-Za-z_]\w*\b', re.sub(r'0[xX][0-9a-fA-F]+', '0', operands.split('#')[0])))
            linked = any(lo < item['end'] and item['start'] < hi for lo, hi in covered[(piece['unit'], piece['section'])])
            locations[key] = dict(unit=piece['unit'], tu=piece['tu'], section=piece['section'],
                                  address=dc.h8(item['start']), end=dc.h8(item['end']), scope='game',
                                  names=sorted(item['labels']), references=sorted(refs - set(item['labels'])),
                                  source=str(path.relative_to(root)), linked_from_c=linked)
            for name in item['labels']:
                labels[(piece['unit'], name)].add(key)
    addresses = {unit: sorted(a for u, a in locations if u == unit) for unit in UNITS}

    def at_address(unit, address):
        index = bisect.bisect_right(addresses[unit], address) - 1
        if index >= 0:
            key = unit, addresses[unit][index]
            if address < dc.hx(locations[key]['end']):
                return {key}
        return set()

    def named(unit, name):
        return labels.get((unit, name)) or labels.get(('main', name), set())

    discarded_associations = []
    for key, row in locations.items():
        types = [d for n in row['names'] for d in declarations[row['tu']].get(n, [])]
        primitive = row['linked_from_c'] and any(
            (d.get('source_definition') and d.get('string_definition')) or
            d.get('data_role') == 'sprite_uv_rectangles' for d in types)
        if primitive:
            if row['references']:
                discarded_associations.append(dict(unit=row['unit'],tu=row['tu'],
                     address=row['address'],names=row['names'],references=row['references'],
                     reason='compiler-emitted character literal or evidenced UV coordinates; disassembler word associations are not pointers'))
            continue
        for name in row['references']:
            graph[key].update(named(row['unit'], name))
    roots = {'c': set(), 'asm': set()}
    examples = collections.defaultdict(lambda: collections.defaultdict(set))
    statistics = collections.defaultdict(collections.Counter)
    native_tus = {t['id']: t for t in native['tus']}
    for tu_id, tu in sorted(tus.items()):
        if not tu.get('function_count'):
            continue
        unit, name = tu['unit'], tu['name']
        directory = root / 'build' / unit / 'build'
        options = [directory / p / (name + '.o') for p in ('c', 'expected', 'hasm')] if unit == 'main' else [directory / p / unit / (name + '.o') for p in ('scaffold/src', 'src')]
        obj = next((p for p in options if p.is_file()), None)
        require(obj is not None, f'no retained native compiler input for {tu_id}; build first')
        assembly = next((p for p in (obj.with_name(obj.name + '.s'), obj.with_suffix('.s')) if p.is_file()), None)
        ledger_path = root / 'config/tu' / (tu_id + '.json')
        inputs[obj], inputs[ledger_path] = sha(obj), sha(ledger_path)
        records = {f['name']: f for f in read_json(ledger_path)['functions']}
        elf = Elf(obj.read_bytes())
        emitted = set()
        if assembly:
            inputs[assembly] = sha(assembly)
            emitted = set(re.findall(r'^\s*\.ent\s+(\S+)', assembly.read_text(), re.M))
        funcs = [s for s in elf.symbols if s.type == 2 and s.size > 0 and 0 < s.shndx < len(elf.sections)]
        for symbol in funcs:
            require((records.get(symbol.name, {}).get('state') == 'c') == (symbol.name in emitted), f'C ledger/compiler definition mismatch: {tu_id}/{symbol.name}')
        c_funcs = {s.index for s in funcs if s.name in emitted}
        statistics[unit].update(compiled_tus=1, c_functions=len(c_funcs), asm_functions=len(funcs) - len(c_funcs))
        spans = collections.defaultdict(list)
        runs = registry['tus'].get(tu_id, {})
        for section, rows in runs.items():
            for run in rows:
                for part in run.get('c_input_spans', []):
                    if part.get('object_range') is not None:
                        lo, hi = map(dc.hx, part['object_range'])
                        spans[part['section']].append((lo, hi, dc.hx(part['range'][0])))
                if not run.get('c_input_spans'):
                    canon = lambda v: re.sub(r'_[0-9A-Fa-f]{8}$', '', v.replace('.', '_'))
                    names = {canon(v) for v in run.get('c_owned_symbols', [])}
                    owners = [s for s in elf.symbols if s.type == 1 and s.size > 0 and 0 < s.shndx < len(elf.sections) and canon(s.name) in names]
                    if owners and len({s.shndx for s in owners}) == 1:
                        raw = elf.sections[owners[0].shndx]
                        lo = min(s.value for s in owners)
                        size = dc.hx(run['range'][1]) - dc.hx(run['range'][0])
                        if lo + size <= raw.size:
                            spans[raw.name].append((lo, lo + size, dc.hx(run['range'][0])))
            if not any(r.get('c_input_spans') for r in rows):
                raw = next((s for s in elf.sections if s.name == section), None)
                if raw and raw.size == sum(dc.hx(r['range'][1]) - dc.hx(r['range'][0]) for r in rows):
                    cursor = 0
                    for run in rows:
                        size = dc.hx(run['range'][1]) - dc.hx(run['range'][0])
                        spans[section].append((cursor, cursor + size, dc.hx(run['range'][0])))
                        cursor += size
        if unit == 'main':
            nt = native_tus[tu_id]
            for section, owner in nt.get('owners', {}).items():
                raw = next((s for s in elf.sections if s.name == section), None)
                if owner == 'c' and section in nt['sections'] and raw:
                    value = nt['sections'][section]
                    # The original piece may include linker alignment after
                    # the raw C section. Padding cannot contain relocations.
                    if dc.hx(value['end']) - dc.hx(value['start']) >= raw.size:
                        spans[section].append((0, raw.size, dc.hx(value['start'])))
            for value in native['sections'].values():
                for run in value.get('order', []):
                    if run['tu'] == name and run.get('c_split'):
                        for part in run.get('c_input_spans', []):
                            if part.get('object_range') is not None:
                                lo, hi = map(dc.hx, part['object_range'])
                                spans[part['section']].append((lo, hi, dc.hx(part['range'][0])))

        def map_raw(section, offset):
            matches = set()
            for lo, hi, address in spans[section]:
                if lo <= offset < hi:
                    matches.update(at_address(unit, address + offset - lo))
            return matches

        def resolve(symbol, addend):
            targets = named(unit, symbol.name) if symbol.name else set()
            if targets:
                result = set(targets)
                for key in targets:
                    if addend:
                        result.update(at_address(key[0], key[1] + addend))
                return result
            if 0 < symbol.shndx < len(elf.sections):
                result = map_raw(elf.sections[symbol.shndx].name, symbol.value + addend)
                if result:
                    return result
            for rows in runs.values():
                for run in rows:
                    for alias in run.get('c_storage_aliases', []):
                        owner = alias['storage_owner']
                        if owner['name'] == symbol.name:
                            return at_address(unit, dc.hx(owner['address']) + addend)
            return set()

        for section_index, relocs in dc.read_relocations(elf).items():
            section, pending_hi = elf.sections[section_index], {}
            for offset, kind, symbol_index in relocs:
                if offset + 4 > section.size or section.type == 8:
                    continue
                word = struct.unpack_from('<I', elf.data, section.offset + offset)[0]
                low = word & 0xffff
                signed_low = low if low < 0x8000 else low - 0x10000
                if kind == 5:
                    pending_hi[symbol_index] = low
                    addend = 0
                elif kind == 6:
                    addend = (pending_hi.get(symbol_index, 0) << 16) + signed_low
                elif kind == 2:
                    addend = word
                elif kind in (7, 9):
                    addend = signed_low
                else:
                    addend = 0
                targets = resolve(elf.symbols[symbol_index], addend)
                if not targets:
                    continue
                owners = [s for s in funcs if s.shndx == section_index and s.value <= offset < s.value + s.size]
                if owners:
                    for symbol in owners:
                        category = 'c' if symbol.index in c_funcs else 'asm'
                        roots[category].update(targets)
                        for key in targets:
                            examples[key][category].add(tu_id + '/' + symbol.name)
                elif section.name in SECTIONS:
                    origins = map_raw(section.name, offset)
                    require(origins, f'unmapped native DATA relocation: {tu_id}/{section.name}+{offset:#x}')
                    for key in origins:
                        graph[key].update(targets)

    def closure(seed):
        result, pending = set(seed), list(seed)
        while pending:
            for target in graph[pending.pop()]:
                if target not in result:
                    result.add(target)
                    pending.append(target)
        return result

    used = {kind: closure(seed) for kind, seed in roots.items()}
    remaining = []
    for key, row in sorted(locations.items()):
        if row['linked_from_c']:
            continue
        c, asm = key in used['c'], key in used['asm']
        category = 'shared' if c and asm else 'recovered_c_only' if c else 'unrecovered_only' if asm else 'unattributed'
        remaining.append(dict(row, category=category, direct_c_examples=sorted(examples[key]['c'])[:8], direct_asm_examples=sorted(examples[key]['asm'])[:8]))
    return dict(schema='remaining-scaffold-function-users/1',
                method='Unique in-scope unit/address locations; roots are compiler-emitted ledger C functions or remaining ASM functions; symbolic DATA and native input relocations followed transitively.',
                counts=dict(collections.Counter(r['category'] for r in remaining)),
                discarded_original_associations=discarded_associations,
                statistics={u: dict(s) for u, s in statistics.items()}, remaining=remaining)


def build_plan(root, inventory=None):
    manifest = read_json(root / 'config/tu-build.json')
    tus = {t['id']: t for t in manifest['tus'] if t.get('in_scope') and t['unit'] in UNITS}
    indexes, inputs, aliases = declaration_index(root, tus)
    inputs[root / 'config/tu-build.json'] = sha(root / 'config/tu-build.json')
    inputs[root / 'config/originals.json'] = sha(root / 'config/originals.json')
    for path in (Path(__file__).resolve(), HERE / 'data_carve.py', HERE / 'elfinfo.py',
                 HERE / 'toolchain.py', HERE / 'data_types.py', HERE / 'cc_tu.sh', root / 'config/toolchain-identity.json'):
        inputs[path] = sha(path)
    metadata = scaffold_map(root, tus, inputs)
    document = read_json(inventory) if inventory else function_inventory(root, tus, inputs, metadata,indexes)
    require(document.get('schema') == 'remaining-scaffold-function-users/1', 'expected a frozen function-user scaffold inventory')
    require(not document.get('errors') and not document.get('mapping_gaps') and not document.get('function_mismatches'), 'function-user inventory has unresolved classification errors')
    selected = [r for r in document['remaining'] if r['category'] in {'shared', 'recovered_c_only'}]
    if inventory:
        inputs[Path(inventory).resolve()] = sha(inventory)
    originals = read_json(root / 'config/originals.json')['units']
    tc = Toolchain(root)
    binaries = {}
    blocks = {}
    rows = []
    seen = set()
    pieces, covered = metadata[:2]

    def binary(unit):
        if unit not in binaries:
            path = tc.game_dir() / originals[unit]['file']
            require(sha(path) == originals[unit]['sha256'], 'original binary SHA-256 differs from the build identity')
            inputs[path] = originals[unit]['sha256']
            binaries[unit] = Elf(path.read_bytes())
        return binaries[unit]

    dt.expand(root, tus, selected, pieces, indexes, inputs, binary, sha)

    def original_item(item):
        unit, tu_id = item['unit'], item['tu']
        path = (root / item['source']).resolve()
        require(path in pieces, 'scaffold file is not an in-scope original piece')
        piece = pieces[path]
        require((piece['unit'], piece['tu'], piece['section']) == (unit, tu_id, item['section']), 'inventory owner/section differs from original piece')
        if path not in blocks:
            _, parsed = dc.piece_items(path, piece['start'], piece['end'])
            blocks[path] = {b['start']: b for b in parsed}
            inputs[path] = sha(path)
        match = blocks[path][dc.hx(item['address'])]
        require(match['end'] == dc.hx(item['end']) and set(match['labels']) == set(item['names']), 'inventory bounds/labels differ from original scaffold')
        require(not any(lo < match['end'] and match['start'] < hi for lo, hi in covered[(unit, item['section'])]), 'original item already overlaps C-linked DATA')
        return path, match['block']
    # The C tables may encode original string addresses numerically and thus
    # have no named extern for their pointees. Infer only anonymous original
    # literal labels, actual .asciz/.string blocks, and a single printable text
    # value in .rodata/.sdata. Never interpret .data hardware payloads as text.
    for item in selected:
        unit = item['unit']
        declarations = indexes.get(item['tu'], {})
        if item['section'] not in {'.rodata', '.sdata'} or len(item['names']) != 1:
            continue
        name = item['names'][0]
        if name in declarations or not re.fullmatch(r'D_[0-9A-Fa-f]{8}', name):
            continue
        path, block = original_item(item)
        code = re.sub(r'/\*.*?\*/', '', '\n'.join(block['lines']), flags=re.S)
        directives = {kind for kind in re.findall(r'^\s*\.(\w+)', code, re.M) if kind not in {'size', 'type', 'align', 'balign', 'p2align', 'globl', 'global', 'set'}}
        if not directives or not directives <= {'asciz', 'string'}:
            continue
        if unit not in binaries:
            original = tc.game_dir() / originals[unit]['file']
            require(sha(original) == originals[unit]['sha256'], 'original binary SHA-256 differs from the build identity')
            inputs[original] = originals[unit]['sha256']
            binaries[unit] = Elf(original.read_bytes())
        payload = dc.va_bytes(binaries[unit], dc.hx(item['address']), dc.hx(item['end']) - dc.hx(item['address']))
        try:
            require(payload is not None, 'no original text bytes')
            text_payload(payload, minimum=3)
        except ValueError:
            continue
        declarations.setdefault(name, []).append(dict(type='const char', dimensions='[]', path=str(path.relative_to(root)),
                                                      basis='original literal directive, printable terminated text, and recovered function-user inventory'))
    for item in selected:
        row = dict(item)
        unit, tu_id = row['unit'], row['tu']
        address = dc.hx(row['address'])
        require((unit, address) not in seen, 'inventory duplicates an address')
        seen.add((unit, address))
        try:
            require(tu_id in tus and tus[tu_id]['unit'] == unit, 'data owner is outside the game TU map')
            path, block = original_item(row)
            if unit not in binaries:
                original = tc.game_dir() / originals[unit]['file']
                require(sha(original) == originals[unit]['sha256'], 'original binary SHA-256 differs from the build identity')
                inputs[original] = originals[unit]['sha256']
                binaries[unit] = Elf(original.read_bytes())
            original = binaries[unit]
            source, kind, evidence = definition(row, block, original, indexes[tu_id])
            types = [evidence['type']] + evidence.get('pointer_declarations', []) + evidence.get('probe_typedefs', [])
            typedefs = [t for ctype in types for t in probe_typedefs(ctype, aliases[tu_id])] + evidence.get('probe_typedefs', [])
            evidence = dict(evidence, probe_typedefs=list(dict.fromkeys(typedefs)))
            row.update(status='generated_candidate', kind=kind, definition=source, type_evidence=evidence,
                       byte_sha256=None if kind == 'storage' else hashlib.sha256(dc.va_bytes(original, address, dc.hx(row['end']) - address)).hexdigest())
        except (ValueError, KeyError, dc.CarveError) as error:
            row.update(status='manual', reason=str(error))
        rows.append(row)
    return dict(schema='automatic-data-plan/1', accepted=False, root=str(root),
                inventory=str(Path(inventory).resolve()) if inventory else 'native-function-relocations',
                inventory_counts=document.get('counts'), inputs={str(p): h for p, h in sorted(inputs.items())},
                tus={k: dict(path=tus[k]['path'], name=tus[k]['name'], unit=tus[k]['unit'], contract=tus[k]['contract']) for k in {r['tu'] for r in rows}},
                counts=dict(collections.Counter(r.get('kind', 'manual') for r in rows)), locations=rows)


def validate_plan(plan):
    require(plan.get('schema') == 'automatic-data-plan/1', 'invalid automatic DATA plan')
    for path, digest in plan['inputs'].items():
        require(Path(path).is_file() and sha(path) == digest, f'stale plan input: {path}')


def grouped(plan):
    groups = collections.defaultdict(list)
    for row in plan['locations']:
        if row['status'] == 'generated_candidate':
            groups[row['tu']].append(row)
    return dict(sorted(groups.items()))


def output_paths(plan, output):
    paths = [output / 'prepared.json', output / 'compiler.json']
    for tu_id in grouped(plan):
        folder = output / tu_id.replace('/', '_')
        paths.extend(folder / name for name in ('additions.c', 'probe.c', 'probe.o', 'probe.i', 'probe.s', 'probe.o.asmsg', 'compile.log'))
    return paths


def prepare(plan, output, lease):
    validate_plan(plan)
    paths = output_paths(plan, output)
    lease.check(paths)
    require(not any(p.exists() for p in paths), 'use a fresh output directory; retained candidates are immutable')
    root = Path(plan['root'])
    results = []
    for tu_id, rows in grouped(plan).items():
        folder = output / tu_id.replace('/', '_')
        folder.mkdir(parents=True, exist_ok=True)
        additions = '\n\n'.join(r['definition'] for r in sorted(rows, key=lambda r: dc.hx(r['address']))) + '\n'
        base = root / plan['tus'][tu_id]['path']
        (folder / 'additions.c').write_text(additions)
        # Probe the definitions separately under the exact TU compiler contract.
        # This does not claim that the complete candidate has passed its gate.
        declarations, typedefs = [], []
        for row in rows:
            typedefs.extend(row['type_evidence'].get('probe_typedefs', []))
            declarations.extend(row['type_evidence'].get('pointer_declarations', []))
        # A local DATA symbol needs a local forward declaration in the probe
        # as well as in the published source.
        local = {row['names'][0] for row in rows if row['definition'].startswith('static ')}
        declarations = [re.sub(r'^extern\b', 'static', d) if
                        any(re.search(r'\b'+re.escape(n)+r'\s*\[', d) for n in local) else d
                        for d in declarations]
        prelude = '\n'.join(dict.fromkeys(typedefs)) + '\n' + '\n'.join(dict.fromkeys(declarations))
        (folder / 'probe.c').write_text('#include "common.h"\n' + prelude + '\n' + additions)
        results.append(dict(tu=tu_id, folder=str(folder), additions_sha256=sha(folder / 'additions.c'),
                            probe_sha256=sha(folder / 'probe.c'),
                            baseline_source=str(base), baseline_sha256=sha(base) if base.is_file() else None, locations=len(rows)))
    report = dict(schema='automatic-data-candidates/1', accepted=False, scope='source_candidates_only', plan_sha256=hashlib.sha256(json.dumps(plan, sort_keys=True).encode()).hexdigest(), candidates=results)
    lease.json(output / 'prepared.json', report)
    return report


def run(argv, cwd, log, timeout=120):
    with Path(log).open('w') as stream:
        process = subprocess.Popen([str(x) for x in argv], cwd=cwd, stdout=stream, stderr=subprocess.STDOUT, start_new_session=True)
        try:
            code = process.wait(timeout=timeout)
        except subprocess.TimeoutExpired:
            os.killpg(process.pid, signal.SIGTERM)
            try:
                process.wait(timeout=5)
            except subprocess.TimeoutExpired:
                os.killpg(process.pid, signal.SIGKILL)
                process.wait()
            code = 124
    return code


def check_compiler(plan, output, lease):
    validate_plan(plan)
    lease.check(output_paths(plan, output))
    require(not (output / 'compiler.json').exists(), 'use a fresh output directory for another compiler attempt')
    prepared = read_json(output / 'prepared.json')
    require(prepared['plan_sha256'] == hashlib.sha256(json.dumps(plan, sort_keys=True).encode()).hexdigest(), 'prepared candidates belong to a different plan')
    for candidate in prepared['candidates']:
        folder = Path(candidate['folder'])
        require(folder == output / candidate['tu'].replace('/', '_'), 'prepared candidate folder differs from output')
        require(sha(folder / 'additions.c') == candidate['additions_sha256'] and sha(folder / 'probe.c') == candidate['probe_sha256'], 'prepared candidate source was changed')
    root = Path(plan['root'])
    tc = Toolchain(root)
    compiler = tc.dir('ee-gcc2.96-realconv-lp7')
    assembler = tc.file('ee-as-la29-vsqrt')
    results = []
    for tu_id, rows in grouped(plan).items():
        facts = plan['tus'][tu_id]
        flags = facts['contract']['flags']
        require(flags in (['-O2', '-G0'], ['-O2', '-G8']), 'automatic probe refuses an unsupported compile contract')
        require(facts['contract']['compiler_profile'] == 'ee-gcc2_96-realconv' and
                sha(compiler / 'cc1') == facts['contract']['cc1_sha256'] and
                sha(assembler) == facts['contract']['assembler_sha256'], 'probe tools differ from the original TU contract')
        folder = output / tu_id.replace('/', '_')
        lease.check([folder / 'compile.log', folder / 'probe.o', folder / 'probe.i', folder / 'probe.s', folder / 'probe.o.asmsg'])
        code = run(['bash', root / 'tools/tu/cc_tu.sh', folder / 'probe.c', folder / 'probe.o', compiler, assembler, flags[-1], root / 'build' / facts['unit'], root], root / 'build' / facts['unit'], folder / 'compile.log')
        report = dict(tu=tu_id, returncode=code, verified=[], requires_linked_gate=[], problems=[])
        if code == 0:
            elf = Elf((folder / 'probe.o').read_bytes())
            symbols = {s.name: s for s in elf.symbols if s.name}
            relocs = dc.read_relocations(elf)
            for row in rows:
                name = row['names'][0]
                try:
                    symbol = symbols[name]
                    extent = dc.hx(row['end']) - dc.hx(row['address'])
                    if row['kind'] == 'storage':
                        require(symbol.shndx in (0xfff2, 0xff03) or (symbol.shndx < len(elf.sections) and elf.sections[symbol.shndx].type == 8), 'storage is not COMMON/NOBITS')
                        if symbol.type == 0 and symbol.bind == 0 and symbol.size == 0:
                            emission=emitted_storage((folder/'probe.s').read_text()).get(name,{})
                            originals=read_json(root/'config/originals.json')['units']
                            original=Elf((tc.game_dir()/originals[facts['unit']]['file']).read_bytes())
                            identities=[s for s in original.symbols if s.name==name and s.shndx!=0]
                            require(len(identities)==1 and identities[0].value==dc.hx(row['address']) and
                                    (identities[0].type,identities[0].bind,identities[0].size)==(0,0,0),
                                    'local compiler storage differs from its original NOTYPE identity')
                            require(emission.get('extent_kind')=='label-space' and emission.get('size')==extent and
                                    emission.get('section')==elf.sections[symbol.shndx].name and
                                    symbol.value+extent<=elf.sections[symbol.shndx].size,
                                    'compiler label/space does not prove the exact storage extent')
                        else:
                            require(symbol.type==1 and symbol.size==extent, 'compiler storage OBJECT size differs')
                    else:
                        require(symbol.type == 1, 'compiler did not emit an OBJECT symbol')
                        require(symbol.size == extent, 'compiler symbol size differs from original object extent')
                        section = elf.sections[symbol.shndx]
                        if any(symbol.value <= off < symbol.value + extent for off, kind, index in relocs.get(section.index, [])):
                            require(row['kind'] in {'pointer_array', 'record_array'}, 'unexpected relocation in non-pointer candidate')
                            report['requires_linked_gate'].append(name)
                            continue
                        payload = elf.section_bytes(section)[symbol.value:symbol.value + extent]
                        require(hashlib.sha256(payload).hexdigest() == row['byte_sha256'], 'compiler DATA bytes differ from original')
                    report['verified'].append(name)
                except (KeyError, ValueError) as error:
                    report['problems'].append(dict(name=name, reason=str(error)))
        else:
            report['problems'].append(dict(reason='compiler/assembler failed', log=str(folder / 'compile.log')))
        results.append(report)
        print(json.dumps(dict(tu=tu_id, verified=len(report['verified']), requires_linked_gate=len(report['requires_linked_gate']), problems=len(report['problems']))), flush=True)
    result = dict(schema='automatic-data-compiler-probe/1', accepted=False, scope='compiler_candidate_only',
                  verified_locations=sum(len(r['verified']) for r in results), tus=results)
    result['requires_linked_gate_locations'] = sum(len(r['requires_linked_gate']) for r in results)
    result['problem_count'] = sum(len(r['problems']) for r in results)
    lease.json(output / 'compiler.json', result)
    return result


def gate(root, project, manifest, report, lease):
    """Use the existing full-image/data gates on an already integrated private build."""
    require(project.resolve() != root.resolve(), 'automatic gate requires a private integration project')
    batch = read_json(manifest)
    targets = batch['linked_targets']
    require(targets and len(targets) == len(set(targets)) and set(targets) <= set(UNITS), 'batch must name its affected EE linked targets')
    require(batch.get('candidates'), 'linked DATA gate requires an existing candidate provenance manifest')
    files = [report, report.with_suffix('.elf.json'), report.with_suffix('.elf.log'), report.with_suffix('.data.log')]
    require(len(set(files)) == len(files), 'gate output must not collide with its sidecars')
    lease.check(files)
    require(not any(p.exists() for p in files), 'use fresh gate output paths to preserve evidence')
    report.parent.mkdir(parents=True, exist_ok=True)
    argv = [sys.executable, '-B', root / 'tools/elf_gate.py', '--root', project, '--report', files[1]]
    for unit in targets:
        argv += ['--image', f'{unit}={project / "build" / unit / "build" / (unit + ".bin")}']
    require(run(argv, root, files[2], timeout=120) == 0, 'existing whole-file gate failed')
    argv = [sys.executable, '-B', root / 'tools/tu/data_gate.py', '--root', project, '--tools-root', project,
            '--manifest', manifest, '--gate', files[1], '--output', report]
    require(run(argv, root, files[3], timeout=120) == 0, 'existing compiler-provenance/linked-data gate failed')
    return read_json(report)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root', type=Path, default=ROOT)
    parser.add_argument('--task')
    parser.add_argument('--owner')
    parser.add_argument('--token-file', type=Path)
    commands = parser.add_subparsers(dest='command', required=True)
    p = commands.add_parser('plan')
    p.add_argument('--inventory', type=Path, help='optional frozen inventory; otherwise derive it from native inputs')
    p.add_argument('--output', type=Path)
    for name in ('resources', 'prepare', 'check'):
        p = commands.add_parser(name)
        p.add_argument('--plan', type=Path, required=True)
        p.add_argument('--output', type=Path, required=True)
    p = commands.add_parser('gate')
    p.add_argument('--project', type=Path, required=True)
    p.add_argument('--manifest', type=Path, required=True)
    p.add_argument('--output', type=Path, required=True)
    p = commands.add_parser('integrate', help='publish all candidates in the canonical checkout under baseline:integration')
    p.add_argument('--plan', type=Path, required=True)
    p.add_argument('--candidates', type=Path, required=True)
    p.add_argument('--output', type=Path, required=True)
    args = parser.parse_args(argv)
    root = args.root.resolve()
    lease = Lease(root, args.task, args.owner, args.token_file)
    if args.command == 'integrate':
        from data_integrate import integrate
        print(json.dumps(integrate(args.plan.resolve(), args.candidates.resolve(), args.output.resolve(), lease)))
    elif args.command == 'plan':
        result = build_plan(root, args.inventory)
        if args.output:
            lease.json(args.output, result)
        print(json.dumps(dict(counts=result['counts'], total=len(result['locations']), generated_tus=len(grouped(result))), indent=2))
    elif args.command == 'resources':
        result = read_json(args.plan)
        for path in output_paths(result, args.output.resolve()):
            print('file:' + str(path))
    elif args.command == 'prepare':
        result = prepare(read_json(args.plan), args.output.resolve(), lease)
        print(json.dumps(dict(tus=len(result['candidates']), locations=sum(r['locations'] for r in result['candidates']))))
    elif args.command == 'check':
        result = check_compiler(read_json(args.plan), args.output.resolve(), lease)
        print(json.dumps(dict(verified_locations=result['verified_locations'], requires_linked_gate=result['requires_linked_gate_locations'], problems=result['problem_count'], accepted=False)))
        return 1 if result['problem_count'] else 0
    else:
        print(json.dumps(gate(root, args.project.resolve(), args.manifest.resolve(), args.output.resolve(), lease)))
    return 0


if __name__ == '__main__':
    try:
        raise SystemExit(main())
    except (ValueError, OSError, dc.CarveError) as error:
        print(f'data_recover: {error}', file=sys.stderr)
        raise SystemExit(1)
