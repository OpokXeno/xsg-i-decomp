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
POINTER_PREFIX = r'(?:\*\s*(?:(?:const|volatile)\s+)*)*'
DECLARATOR = re.compile(r'\s*(' + POINTER_PREFIX + r')([A-Za-z_]\w*)\s*((?:\[[^\]]*\]\s*)*)\s*$')
LOCAL_INCLUDE = re.compile(r'^\s*#\s*include\s+"([^"]+)"', re.M)
# Project scalar widths and the evidenced native EE long ABI; never host widths.
SCALARS = {
    'char': ('b', 1), 'signed char': ('b', 1), 'unsigned char': ('B', 1),
    's8': ('b', 1), 'u8': ('B', 1), 'short': ('h', 2), 'short int': ('h', 2),
    'unsigned short': ('H', 2), 'unsigned short int': ('H', 2),
    's16': ('h', 2), 'u16': ('H', 2), 'int': ('i', 4), 'signed int': ('i', 4),
    'unsigned': ('I', 4), 'unsigned int': ('I', 4), 's32': ('i', 4), 'u32': ('I', 4),
    'long long': ('q', 8), 'unsigned long long': ('Q', 8), 's64': ('q', 8), 'u64': ('Q', 8),
    'float': ('f', 4), 'double': ('d', 8),
    # Native EE contract: long[24] emits 192 bytes; compiled consumers use
    # eight-byte sd/ld and strides. These are not the execution host's widths.
    'long': ('q', 8), 'long int': ('q', 8), 'signed long': ('q', 8),
    'unsigned long': ('Q', 8), 'unsigned long int': ('Q', 8),
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


def declaration_index(root, tus, include_closures=None):
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
        # Pointer probes need actual source typedefs too. A genuine named tag
        # permits an incomplete declaration; an anonymous record must retain
        # its existing definition, since introducing a tag changes its type.
        for kind, tag, body, name in re.findall(
                r'\btypedef\s+(struct|union)\s+([A-Za-z_]\w*)?\s*'
                r'\{([^{}]*)\}\s*([A-Za-z_]\w*)\s*;', code, re.S):
            typedefs[name].add(f'{kind} {tag}' if tag else f'{kind} {{ {body} }}')
        for result, name, parameters in re.findall(
                r'\btypedef\s+([^;{}]+?)\(\s*\*\s*(\w+)\s*\)\s*\(([^;{}]*)\)\s*;', code):
            typedefs[name].add(clean_type(result) + ' (*_DATA_TYPE_NAME_)(' + parameters + ')')
        for body in re.findall(r'\btypedef\s+([^;{}()]+);', code):
            match = re.fullmatch(r'\s*(.+?)\s+(' + POINTER_PREFIX +
                                 r'[A-Za-z_]\w*\s*(?:\[[^\]]*\]\s*)+)\s*', body)
            if match:
                declarator = DECLARATOR.fullmatch(match[2])
                if declarator:
                    typedefs[declarator[2]].add(clean_type(match[1] + ' ' + declarator[1]) +
                                              '_DATA_TYPE_NAME_' + declarator[3])
        # An array bound spelled with an object-like integer macro of this
        # file is the same explicit constant (`#define N 0x80` ... `x[N]`).
        macros = dict(re.findall(r'^[ \t]*#[ \t]*define[ \t]+([A-Za-z_]\w*)[ \t]+(0[xX][0-9A-Fa-f]+|\d+)[ \t]*$', code, re.M))
        bounded = re.sub(r'\[\s*([A-Za-z_]\w*)\s*\]',
                         lambda m: '[' + macros[m[1]] + ']' if m[1] in macros else m[0], code) if macros else code
        for body in DECL.findall(bounded):
            parts = body.split(',')
            first = re.fullmatch(r'\s*(.+?)\s+(' + POINTER_PREFIX + r'[A-Za-z_]\w*\s*(?:\[[^\]]*\]\s*)*)\s*', parts[0])
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
        for result, name, dims, parameters in re.findall(
                r'\bextern\s+([^;{}]+?)\(\s*\*\s*(\w+)\s*((?:\[[^\]]*\]\s*)*)\)'
                r'\s*\(([^;{}]*)\)\s*;', code):
            declarations[name].append(dict(type=clean_type(result) + ' (*_DATA_TYPE_NAME_)(' + parameters + ')',
                                           dimensions=dims.strip(), path=str(path.relative_to(root))))
        for definition in re.finditer(
                r'^\s*(?:static\s+)?((?:const\s+)?(?:char|unsigned\s+char|signed\s+char|u8|s8))'
                r'\s+(\w+)\s*((?:\[[^\]]*\]\s*)+)\s*=\s*("?)', code, re.M):
            ctype, name, bounds, string = definition.groups()
            declarations[name].append(dict(type=clean_type(ctype),dimensions=bounds.strip(),
                 path=str(path.relative_to(root)),source_definition=True,string_definition=bool(string)))
        for ctype, name, bounds in re.findall(
                r'^\s*(?:static\s+)?((?:const\s+)?float)\s+(\w+)\s*((?:\[[^\]]*\]\s*)+)\s*=', code, re.M):
            declarations[name].append(dict(type=clean_type(ctype), dimensions=bounds.strip(),
                                           path=str(path.relative_to(root)), source_definition=True))
        for alias, local, count in re.findall(r'\b(\w+)\s*\*\s*(\w+)\s*\[(\d+)\]\s*;', code):
            templates = typedefs.get(alias, set())
            if len(templates) != 1 or not re.fullmatch(r'const char\s*\*_DATA_TYPE_NAME_\[2\]', next(iter(templates))):
                continue
            copies = re.findall(r'\b__builtin_memcpy\s*\(\s*' + re.escape(local) +
                                r'\s*,\s*(\w+)\s*,\s*sizeof\s*\(\s*' + re.escape(local) + r'\s*\)\s*\)', code)
            if not re.search(r'\(\s*\*\s*' + re.escape(local) + r'\s*\[[^\]]+\]\s*\)\s*\[', code):
                continue
            for name in copies:
                for declaration in declarations.get(name, []):
                    if clean_type(declaration['type']) == alias + ' *const':
                        declaration.update(data_role='character_array_views', array_alias=alias,
                                           consumer_count=int(count), pointee_count=2,
                                           basis='existing array typedef, copied pointer-array extent, and C dereference/index of each two-string array')
        for name in microprogram_address_tables(code):
            for declaration in declarations.get(name, []):
                if declaration['type'] == 'u32' and declaration['dimensions']:
                    declaration.update(data_role='vu_microprogram_byte_addresses',
                                       basis='C indexes this address table and shifts the scalar into the VIF MSCAL immediate')
        sends = re.finditer(r'\b(sceVif1PkAddDataN|xglDmaDirectNormal|nmlModelDirectSend)'
                            r'\s*\([^,;]+,\s*(?:\(\s*(?:u8|unsigned\s+char)\s*\*\s*\)\s*)?'
                            r'(\w+)\s*,\s*(\d+)\s*\)', code)
        for send in sends:
            function, name, amount = send.groups()
            owners = [name]
            # A short, uninterrupted source assignment chain also evidences
            # the array passed through a local packet pointer.
            for match in re.finditer(r'\b' + re.escape(name) + r'\s*=\s*(\w+)\s*;', code[:send.start()]):
                between = code[match.end():send.start()]
                if not re.search(r'[{}]', between) and between.count(';') <= 3:
                    owners.append(match[1])
            for owner in owners:
                for declaration in declarations.get(owner, []):
                    if declaration['dimensions'] and '*' not in declaration['type']:
                        declaration.update(data_role='vif_gif_numeric_packet',
                            consumer_bytes=int(amount) * (4 if function == 'sceVif1PkAddDataN' else 16),
                            consumer_call=clean_type(send[0]),
                            basis='C sends the explicitly typed array with this exact word/quadword extent')
        # Closed menu idiom: copied unsigned-byte table entries are tested
        # numerically and assigned to the state machine's nextState field.
        # Printable byte values do not turn an evidenced state table into text.
        for local, count in re.findall(r'\bunsigned\s+char\s+(\w+)\s*\[(\d+)\]\s*;', code):
            copies = re.findall(r'\b__builtin_memcpy\s*\(\s*' + re.escape(local) +
                                r'\s*,\s*(\w+)\s*,\s*sizeof\s*\(\s*' +
                                re.escape(local) + r'\s*\)\s*\)', code)
            selections = re.findall(re.escape(local) + r'\s*\[([^\]]+)\]\s*!=\s*0', code)
            for index in selections:
                assignment = (r'\b\w+\s*\.\s*nextState\s*=\s*' + re.escape(local) +
                              r'\s*\[\s*' + re.escape(index.strip()) + r'\s*\]\s*;')
                if not re.search(assignment, code):
                    continue
                for name in copies:
                    for declaration in declarations.get(name, []):
                        if declaration['type'] == 'unsigned char' and declaration['dimensions']:
                            declaration.update(data_role='menu_option_states', consumer_count=int(count),
                                               basis='C copies unsigned bytes, tests the selected byte against zero, and assigns it to nextState')
        for call in re.finditer(r'\b(?:xglFontPrint|xglFontDebugPrintf|xglFontPrintDirect|xglFontPrintDirectOT|eTagFontSet|strcat|strcpy)'
                                r'\s*\(([^;]+)\)\s*;', code):
            for name in set(re.findall(r'\b[A-Za-z_]\w*\b', call[1])):
                for declaration in declarations.get(name, []):
                    if (declaration['dimensions'] and declaration['type'].count('*') == 1 and
                            base_type(declaration['type'].replace('*', '')) in {'char', 'unsigned char', 'u8', 's8'}):
                        declaration.update(data_role='character_stream_pointer_table',
                                           basis='C passes a character-pointer table element to its character/font API')
                    if (declaration['dimensions'] and '*' not in declaration['type'] and
                            base_type(declaration['type']) in {'char', 'unsigned char', 'u8', 's8'}):
                        declaration.update(data_role='font_character_stream',
                                           basis='existing character array is consumed by the C font string API')
        for name, records in declarations.items():
            for declaration in records:
                if not declaration['dimensions'] or '*' in declaration['type']:
                    if (not declaration['dimensions'] and base_type(declaration['type']) in {'char', 'unsigned char', 'u8', 's8'} and
                            re.search(r'\b' + re.escape(name) + r'\s*(?:[+^|&]=|==|=)', code)):
                        declaration.update(data_role='numeric_byte_scalar',
                                           basis='C reads or updates the explicitly typed scalar byte')
                    continue
                if base_type(declaration['type']) not in {'char', 'unsigned char', 'u8', 's8'}:
                    continue
                indexed = re.escape(name) + r'\s*\[[^\]]+\]'
                if re.search(indexed + r'\s*&\s*(?:0[xX][0-9a-fA-F]+|\d+)', code):
                    declaration.update(data_role='numeric_byte_flags',
                                       basis='C indexes the explicitly typed byte array and tests numeric flag bits')
                elif (re.search(r'\bstate\s*=\s*' + indexed, code) or
                      re.search(r'-\s*\(\s*int\s*\)\s*' + indexed, code)):
                    declaration.update(data_role='numeric_byte_states',
                                       basis='C assigns indexed byte values to a numeric state or signed error result')
                elif re.search(r'\[\s*' + indexed + r'(?:\s*\[[^\]]+\])*\s*\]', code):
                    declaration.update(data_role='numeric_byte_indices',
                                       basis='C uses the explicitly typed byte array elements as array indices')
                for pointer in re.findall(r'\b(\w+)\s*=\s*' + re.escape(name) + r'\s*;', code):
                    if (re.search(r'<\s*' + re.escape(pointer) + r'\s*\[\s*0\s*\]', code) and
                            re.search(r'\b(\w+)\s*\+=\s*2\b', code)):
                        offsets = re.findall(re.escape(pointer) + r'\s*\[\s*(\w+)\s*\+\s*1\s*\]', code)
                        if any(re.search(re.escape(pointer) + r'\s*\[\s*' + re.escape(offset) + r'\s*\]', code)
                               and re.search(r'\b' + re.escape(offset) + r'\s*\+=\s*2\b', code) for offset in offsets):
                            declaration.update(data_role='counted_xy_bytes',
                                               basis='C reads the byte count and advances through indexed x/y byte pairs')
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
        if include_closures is not None:
            include_closures[tu_id] = sorted(visited)
    return indexes, read, aliases


def probe_typedefs(ctype, aliases):
    """Carry evidenced typedefs or genuine tag forwards into the probe."""
    result, active = [], set()

    def visit(name):
        choices = aliases.get(name, set())
        if not choices:
            return
        require(len(choices) == 1 and name not in active, 'divergent or recursive scalar typedef')
        active.add(name)
        base = next(iter(choices))
        # In "typedef struct T T", the tag T is a separate C namespace,
        # not an alias dependency or a recursive scalar typedef.
        if not re.fullmatch(r'(?:struct|union)\s+[A-Za-z_]\w*', base):
            for token in re.findall(r'\b[A-Za-z_]\w*\b', base):
                visit(token)
        declaration = ('typedef ' + base.replace('_DATA_TYPE_NAME_', name) + ';'
                       if '_DATA_TYPE_NAME_' in base else f'typedef {base} {name};')
        if declaration not in result:
            result.append(declaration)
        active.remove(name)

    for token in re.findall(r'\b[A-Za-z_]\w*\b', ctype):
        visit(token)
    return result


def scalar_type(ctype, aliases):
    """Resolve only explicit scalar typedef chains to established widths."""
    value = clean_type(re.sub(r'\b(?:const|volatile)\b', '', ctype))
    chain = set()
    while value not in SCALARS:
        if not IDENT.fullmatch(value) or value not in aliases:
            return None
        require(value not in chain and len(aliases[value]) == 1,
                'divergent or recursive scalar typedef')
        chain.add(value)
        value = clean_type(next(iter(aliases[value])))
    return SCALARS[value]


def array_typedef_width(ctype, aliases):
    """Width of one object of a source array typedef of scalars
    (`typedef float Matrix4[4][4];`), for NOBITS storage only."""
    value = clean_type(re.sub(r'\b(?:const|volatile)\b', '', ctype))
    if not IDENT.fullmatch(value) or len(aliases.get(value, ())) != 1:
        return None
    match = re.fullmatch(r'(.+?)\s*_DATA_TYPE_NAME_((?:\[\d+\])+)', clean_type(next(iter(aliases[value]))))
    if not match or '*' in match[1]:
        return None
    scalar = scalar_type(match[1], aliases)
    if not scalar:
        return None
    width = scalar[1]
    for bound in re.findall(r'\[(\d+)\]', match[2]):
        width *= int(bound)
    return width


def pointer_type(ctype, aliases):
    """An explicit pointer or a source function-pointer typedef has EE width four."""
    value = clean_type(re.sub(r'\b(?:const|volatile)\b', '', ctype))
    if '*' in value:
        return True
    seen = set()
    while IDENT.fullmatch(value) and value in aliases:
        require(value not in seen and len(aliases[value]) == 1, 'divergent or recursive pointer typedef')
        seen.add(value)
        value = clean_type(next(iter(aliases[value])))
        if '*' in value and '[' not in value:
            return True
    return False


def function_pointer_type(ctype, aliases):
    templates = [ctype] + list(aliases.get(ctype, set()))
    return any(re.search(r'\(\s*\*_DATA_TYPE_NAME_\s*\)\s*\(', t) for t in templates)


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
    require(len(dims) <= 4, 'array rank exceeds bounded automatic dimensions')
    require(extent % width == 0, 'object extent is not a whole element count')
    count = extent // width
    if not dims:
        require(count == 1, 'scalar declaration does not cover the original object')
        return '', count
    shape = []
    for index, value in enumerate(dims):
        try:
            bound = int(value, 0) if value.strip() else None
        except ValueError:
            raise ValueError('array bound is not an explicit integer') from None
        require(bound is None and index == 0 or bound is not None and bound > 0,
                'only the leading array dimension may be unspecified')
        shape.append(bound)
    tail = math.prod(shape[1:])
    if shape[0] is None:
        require(count % tail == 0, 'array extent does not contain complete rows')
        shape[0] = count // tail
    require(math.prod(shape) == count, 'declared array bound differs from original extent')
    return ''.join(f'[{n}]' for n in shape), count


def array_initializer(entries, suffix):
    """Keep actual multidimensional row boundaries in readable C."""
    if not suffix:
        require(len(entries) == 1, 'scalar initializer has multiple elements')
        return entries[0]
    shape = [int(n) for n in re.findall(r'\[(\d+)\]', suffix)]
    def grouped(values, dims):
        if len(dims) == 1:
            return '{ ' + ', '.join(values) + ' }'
        width = math.prod(dims[1:])
        return '{ ' + ', '.join(grouped(values[i:i+width], dims[1:])
                               for i in range(0, len(values), width)) + ' }'
    return grouped(entries, shape)


def validate_vif_gif_packet(payload, evidence):
    """VIF DIRECTHL + one GIF PACKED tag (NLOOP >= 1) numeric descriptor grammar."""
    require(payload is not None and len(payload) == evidence['consumer_bytes'] and
            len(payload) >= 48 and len(payload) % 16 == 0, 'GS packet differs from actual C send extent')
    words = struct.unpack_from('<4I', payload)
    tag, descriptors = struct.unpack_from('<QQ', payload, 16)
    nreg = (tag >> 60) & 15 or 16
    nloop = tag & 0x7fff
    require(nloop >= 1 and len(payload) == 16 * (2 + nloop * nreg),
            'GS packet extent is not the complete supported PACKED descriptor')
    require(words[:3] == (0, 0, 0) and words[3] == 0x51000000 | (1 + nloop * nreg),
            'GS packet VIF DIRECTHL command/count differs')
    # EOP may be clear: the packet then continues a GIF chain (user 2026-10-31:
    # relax the GS packet grammar where the bytes are an ordinary GIF tag).
    require(tag & ((1 << 46) - 1) & ~0x8000 == nloop and ((tag >> 58) & 3) == 0,
            'GS packet GIF tag is not PACKED with zero reserved bits')
    require(nreg == 16 or descriptors >> (4 * nreg) == 0, 'GS packet has unused nonzero register descriptors')
    decoded = []
    names = {0:'PRIM', 1:'RGBA', 2:'STQ', 3:'UV', 4:'XYZF2', 5:'XYZ2', 6:'TEX0_1', 7:'TEX0_2', 8:'CLAMP_1',
             9:'CLAMP_2', 10:'FOG', 12:'XYZF3', 13:'XYZ3', 14:'A+D', 15:'NOP'}
    for loop in range(nloop):
        for index in range(nreg):
            descriptor = (descriptors >> (index * 4)) & 15
            offset = 32 + (loop * nreg + index) * 16
            if descriptor == 14:
                value, register = struct.unpack_from('<QQ', payload, offset)
                require(register < 0x80, 'GS A+D packet register/address word is not a GS register')
                decoded.append(dict(index=loop * nreg + index, descriptor='A+D', register=f'0x{register:02x}',
                                    value=f'0x{value:016x}'))
            else:
                decoded.append(dict(index=loop * nreg + index, descriptor=names.get(descriptor, hex(descriptor))))
    return dict(vif_command='DIRECTHL', direct_quadwords=1 + nloop * nreg, gif_mode='PACKED',
                loops=nloop, registers=nreg, descriptor=f'0x{descriptors:x}', entries=decoded)


def callback_signature(prototype, direct=False):
    if direct:
        match = re.fullmatch(r'\s*(.+?)\(\s*\*_DATA_TYPE_NAME_\s*\)\s*\((.*)\)\s*', prototype, re.S)
    else:
        match = re.fullmatch(r'\s*extern\s+(.+?)\s+\w+\s*\((.*)\)\s*;', prototype, re.S)
    require(match is not None, 'callback signature lacks an explicit supported source prototype')
    parameters = []
    for value in match[2].split(','):
        value = clean_type(value)
        if value in ('', 'void'):
            continue
        require('(' not in value and '[' not in value and '...' not in value,
                'callback parameter declarator needs a source interface review')
        value = re.sub(r'\s+([A-Za-z_]\w*)$', lambda m: '' if m[1] not in
                       {'int','long','short','char','float','double','void'} else m[0], value)
        value = re.sub(r'(\*)\s*[A-Za-z_]\w*$', r'\1', value)
        parameters.append(re.sub(r'\s+', '', value))
    return re.sub(r'\s+', '', match[1]), tuple(parameters)


def original_data_identities(original, name, address):
    """Resolve original DATA names, including evidenced Splat local aliases."""
    def matching_name(symbol):
        normalized = re.sub(r'[^A-Za-z_0-9]', '_', symbol.name)
        # Splat may append the original VA to distinguish a local alias. The
        # suffix is evidence only when it equals this exact original address.
        suffix = name[len(normalized) + 1:] if name.startswith(normalized + '_') else ''
        return (symbol.name == name or normalized == name or
                bool(re.fullmatch(r'[0-9A-Fa-f]{8}', suffix) and int(suffix, 16) == address))
    identities=[s for s in original.symbols if s.value==address and s.type in (0, 1) and
                0 < s.shndx < len(original.sections) and
                original.sections[s.shndx].name in SECTIONS and matching_name(s)]
    require(not identities or len({s.bind for s in identities})==1,
            'original symbol aliases have conflicting bindings')
    require(not identities or identities[0].bind in (0, 1),
            'original DATA binding requires a supported source declaration')
    return identities


def original_local(original, name, address):
    """Preserve compiler-local labels renamed by splat (e.g. i1.52)."""
    identities = original_data_identities(original, name, address)
    return bool(identities and identities[0].bind==0)


def definition(row, block, original, declarations, aliases=None, records=None):
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
    original_objects = [s for s in original_data_identities(original, name, address) if s.type == 1 and s.size > 0]
    if len(original_objects) == 1 and original_objects[0].size < extent:
        size = original_objects[0].size
        scalar = scalar_type(ctype, aliases or {})
        tail = dc.va_bytes(original, address + size, extent - size)
        if scalar and 0 < extent - size <= 15 and tail is not None and not any(tail):
            # A positive-size original OBJECT proves the meaningful extent;
            # alignment remains original scaffolding rather than enlarged C.
            dimensions(decl, size, scalar[1])
            row.update(mapped_end=row['end'], end=dc.h8(address + size),
                       object_size=size, zero_padding_tail=extent-size)
            end, extent = address + size, size
    byte_scalar = [d for d in declarations.get(name, []) if d.get('data_role') == 'numeric_byte_scalar']
    identities = original_data_identities(original, name, address)
    if (byte_scalar and not dims and 1 < extent <= 16 and len(identities) == 1 and
            (identities[0].type, identities[0].bind, identities[0].size) == (0, 0, 0) and
            row['section'] in {'.bss', '.sbss'}):
        row.update(mapped_end=row['end'], end=dc.h8(address + 1), object_size=1,
                   zero_padding_tail=extent-1)
        end, extent = address + 1, 1
    large_storage = (row['section'] in {'.bss', '.sbss'} and row.get('grouped_items') and
                     row.get('storage_owner', {}).get('size') == extent)
    require(0 < extent <= (16 * 1024 * 1024 if large_storage else 65536),
            'extent exceeds bounded automatic object size')
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
    if records is not None:
        record = dt.record_definition(row, block, original, declarations, records,
                                      aliases or {}, numeric_literal, storage=storage)
        if record is not None:
            return record
    if provenance.get('data_role') == 'menu_option_states':
        require(character and payload is not None and
                provenance['consumer_count'] <= extent <= provenance['consumer_count'] + 3 and
                not any(payload[provenance['consumer_count']:]),
                'state table consumer extent or zero alignment differs from original')
        suffix, count = dimensions(decl, extent, 1)
        return (prefix + suffix + ' = { ' + ', '.join(f'0x{x:02x}' for x in payload) + ' };',
                'numeric', provenance)
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
    if provenance.get('data_role') == 'font_character_stream' and row['section'] not in {'.bss', '.sbss'}:
        require(character and payload is not None and payload.endswith(b'\0'),
                'font character stream lacks an original terminal zero')
        suffix, count = dimensions(decl, extent, 1)
        require(len(re.findall(r'\[', suffix)) == 1, 'multidimensional font streams need row boundaries')
        return prefix + suffix + ' = ' + literal(payload.rstrip(b'\0')) + ';', 'string', provenance
    if character and payload is not None and payload.endswith(b'\0') and dt.encoded_text(payload):
        payload = dc.va_bytes(original, address, extent)
        require(payload is not None, 'string is not inside original initialized data')
        # Font control parameters include zero bytes inside a text value.
        # The explicit character type and readable text establish this format;
        # preserve every byte rather than treating the first zero as its end.
        suffix, count = dimensions(decl, extent, 1)
        return prefix + suffix + ' = ' + literal(payload.rstrip(b'\0')) + ';', 'string', provenance
    scalar = scalar_type(ctype, aliases or {})
    packet = None
    if provenance.get('data_role') == 'vif_gif_numeric_packet':
        require(scalar and scalar[1] in (4, 8), 'GS packet requires its actual 32/64-bit scalar array type')
        packet = validate_vif_gif_packet(payload, provenance)
        provenance = dict(provenance, numeric_packet_proof=packet)
    if kinds <= {'space', 'zero'} and row['section'] in {'.bss', '.sbss'}:
        pointer = pointer_type(ctype, aliases or {})
        width = 4 if pointer else scalar[1] if scalar else array_typedef_width(ctype, aliases or {})
        require(width, 'uninitialized storage needs a scalar/array type and NOBITS section')
        if not (pointer or scalar):
            alias = clean_type(re.sub(r'\b(?:const|volatile)\b', '', ctype))
            text = 'typedef ' + clean_type(next(iter(aliases[alias]))).replace('_DATA_TYPE_NAME_', ' ' + alias).replace('  ', ' ') + ';'
            provenance = dict(provenance, probe_typedefs=list(provenance.get('probe_typedefs', [])) + [text])
        suffix, count = dimensions(decl, extent, width)
        return prefix + suffix + ';', 'storage', provenance
    if provenance.get('data_role') == 'character_array_views':
        count = provenance['consumer_count']
        size = count * 4
        require(kinds <= {'word', 'long', '4byte'} and payload is not None and
                size <= extent <= size + 15 and (address + extent) % 16 == 0 and
                not any(payload[size:]), 'pointer-array consumer extent or original zero alignment differs')
        require(len(values) == extent // 4 and all(value in {'0', '0x0', '0x00000000'}
                for kind, value in values[count:]), 'pointer-array alignment has symbolic or mixed payload')
        initializers, needed = [], []
        for kind, value in values[:count]:
            require(IDENT.fullmatch(value), 'array view needs a named original target without an addend')
            targets = declarations.get(value, [])
            require(targets and all(d['type'] == 'const char *' and d['dimensions'] for d in targets),
                    'array view target is not an existing compatible const-character pointer array')
            symbols = [s for s in original_data_identities(original, value, struct.unpack_from('<I', payload, len(initializers)*4)[0])
                       if s.type == 1 and s.size == 4 * provenance['pointee_count']]
            require(len(symbols) == 1, 'array view target lacks its exact original two-pointer OBJECT extent')
            for declaration in targets:
                dimensions(declaration, symbols[0].size, 4)
            initializers.append('&' + value)
            needed.append('extern const char *' + value + '[2];')
        if extent != size:
            row.update(mapped_end=row['end'], end=dc.h8(address + size),
                       object_size=size, zero_padding_tail=extent-size)
        evidence = dict(provenance, pointer_declarations=list(dict.fromkeys(needed)))
        return prefix + f'[{count}] = {{ ' + ', '.join(initializers) + ' };', 'pointer_array', evidence
    callback = function_pointer_type(ctype, aliases or {})
    if '*' in ctype or callback:
        require((callback or ctype.count('*') == 1) and kinds <= {'word', 'long', '4byte'}, 'pointer table needs a single-pointer type and word directives')
        suffix, count = dimensions(decl, extent, 4)
        require(len(values) == count, 'pointer table has unexplained alignment or mixed payload')
        initializers = []
        for kind, value in values:
            if value in {'0', '0x0', '0x00000000'}:
                initializers.append('0')
                continue
            require(IDENT.fullmatch(value), 'pointer addends/numeric addresses need a pointee-layout hypothesis')
            if callback:
                targets = [s for s in original.symbols if s.name == value and s.type == 2 and s.shndx != 0]
                require(len(targets) == 1 and declarations.get(value),
                        'callback target lacks original function identity and actual source prototype')
                template = ctype if '_DATA_TYPE_NAME_' in ctype else next(iter((aliases or {})[ctype]))
                require(callback_signature(template, direct=True) ==
                        callback_signature(declarations[value][0]['function_prototype']),
                        'callback target source prototype is incompatible with its dispatcher type')
                initializers.append(value)
                continue
            target = declarations.get(value, [])
            require(target and all(d['dimensions'] and base_type(d['type']) == base_type(ctype.replace('*', '')) for d in target), 'pointer target lacks a compatible declared array type')
            initializers.append(value)
        provenance = dict(provenance, pointer_declarations=[
            declarations[v][0]['function_prototype'] if callback else
            f"extern {declarations[v][0]['type']} {v}{declarations[v][0]['dimensions']};"
            for v in set(initializers) if v != '0'])
        value = array_initializer(initializers, suffix)
        if '_DATA_TYPE_NAME_' in ctype:
            return storage + ctype.replace('_DATA_TYPE_NAME_', name + suffix) + ' = ' + value + ';', 'pointer_array', provenance
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
    if symbolic and packet is None:
        require(payload is not None and (scalar[1] in (1, 2, 8) or len(payload) % 4 == 0),
                'symbolic words need a proven numeric layout')
        # User 2026-10-31: the declared numeric C type decides; splat prints
        # numbers that fall inside the image as labels. Refuse only a 32-bit
        # element equal to the start of a named original DATA object (a real
        # pointer needs a pointer type).
        if scalar[1] == 4:
            words = struct.unpack('<' + 'I'*(len(payload)//4), payload)
            starts = {s.value for s in original.symbols if s.type == 1 and s.size > 0 and s.name}
            require(not any(w in starts for w in words),
                    'numeric declaration contains original-image addresses needing a pointee hypothesis')
        provenance = dict(provenance, disassembler_association='numbers of the declared C type; splat label spellings ignored')
    fmt, width = scalar
    suffix, count = dimensions(decl, extent, width)
    byte_role = provenance.get('data_role') in {'numeric_byte_flags', 'numeric_byte_states', 'numeric_byte_indices', 'counted_xy_bytes'}
    payload = dc.va_bytes(original, address, extent)
    require(payload is not None, 'numeric object is not in original initialized data')
    plain_char = clean_type(re.sub(r'\b(?:const|volatile)\b', '', ctype)) == 'char' and '*' not in ctype
    if (suffix and width == 1 and plain_char and len(re.findall(r'\[', suffix)) == 1 and
            sum(32 <= v < 127 or v >= 0xa1 for v in payload.rstrip(b'\0')) * 2 >= len(payload.rstrip(b'\0')) > 0):
        # A character array of text (font control bytes included): a string
        # literal of the exact array size, zero tail implicit or exact fit.
        return prefix + suffix + ' = ' + literal(payload.rstrip(b'\0')) + ';', 'string', provenance
    if provenance.get('data_role') == 'counted_xy_bytes':
        used = 2 + payload[0] * 2
        require(used <= len(payload) and len(payload) - used <= 3 and not any(payload[used:]),
                'counted x/y pairs differ from the original extent or zero alignment')
    numbers = struct.unpack('<' + fmt * count, payload)
    entries = [numeric_literal(v, ctype, fmt) for v in numbers]
    if packet is not None or (symbolic and fmt not in ('f', 'd') and width >= 4):
        entries = [f'0x{v & ((1 << (width * 8)) - 1):0{width * 2}x}' +
                   ('ULL' if width == 8 else 'U') for v in numbers]
    value = array_initializer(entries, suffix)
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
        if not any(t['unit'] == unit for t in tus.values()):
            continue
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


def objdiff_scope(root, tus, report, unit, code_complete):
    """Select original game owners from a pinned native objdiff report."""
    require(not code_complete or report, '--code-complete requires --objdiff-report')
    selected = {k for k, t in tus.items() if unit is None or t['unit'] == unit}
    evidence = dict(unit=unit, code_complete=code_complete)
    if not code_complete:
        return selected, evidence
    document = read_json(report)
    require(isinstance(document.get('units'), list), 'objdiff report has no unit list')
    matches, seen = {}, set()
    for row in document['units']:
        measures = row.get('measures', {})
        if int(measures.get('total_code', 0)) <= 0 or measures.get('matched_code_percent') != 100:
            continue
        name, source = row.get('name'), row.get('metadata', {}).get('source_path')
        candidates = [k for k in selected if
                      name == tus[k]['unit'] + '/' + tus[k]['name'] or source == tus[k]['path']]
        if not candidates:
            # SDK/runtime and other requested-image owners are outside recovery scope.
            continue
        require(len(candidates) == 1, f'ambiguous objdiff owner: {name}/{source}')
        key = candidates[0]
        require(name in (None, tus[key]['unit'] + '/' + tus[key]['name']) and
                source in (None, tus[key]['path']), f'objdiff name/source owner mismatch: {name}/{source}')
        require(key not in seen, f'duplicate objdiff owner: {key}')
        seen.add(key)
        matches[key] = dict(name=name, source_path=source, total_code=measures['total_code'],
                            matched_code_percent=measures['matched_code_percent'],
                            total_data=measures.get('total_data', 0),
                            matched_data_percent=measures.get('matched_data_percent', 0))
    evidence.update(objdiff_report=str(Path(report).resolve()), objdiff_report_sha256=sha(report),
                    owners=matches)
    return set(matches), evidence


def scaffold_inventory(root, metadata, inputs):
    """Inventory every mapped original item, including unreferenced/manual DATA."""
    pieces, covered = metadata[:2]
    rows, seen = [], set()
    for path, piece in sorted(pieces.items()):
        inputs[path] = sha(path)
        _, items = dc.piece_items(path, piece['start'], piece['end'])
        for item in items:
            if any(lo < item['end'] and item['start'] < hi for lo, hi in
                   covered[(piece['unit'], piece['section'])]):
                continue
            key = piece['unit'], item['start']
            require(key not in seen, 'overlapping original DATA piece identities')
            seen.add(key)
            rows.append(dict(unit=piece['unit'], tu=piece['tu'], section=piece['section'],
                             address=dc.h8(item['start']), end=dc.h8(item['end']), scope='game',
                             names=sorted(item['labels']), source=str(path.relative_to(root)),
                             category='all_scaffold',
                             unreferenced_padding='unreferenced pad' in '\n'.join(item['block']['lines'])))
    return dict(schema='remaining-scaffold-function-users/1', remaining=rows,
                counts=dict(all_scaffold=len(rows)))


_SIZEOF = {}


def compiler_sizeof(root, tu, ctype):
    """sizeof(ctype) as compiled in TU's own source with its contract compiler."""
    if (tu, ctype) in _SIZEOF:
        return _SIZEOF[(tu, ctype)]
    import tempfile, subprocess
    size = None
    try:
        tus = {t['id']: t for t in read_json(root / 'config/tu-build.json')['tus']}
        source = (root / tus[tu]['path']).resolve()
        cc = root / '.work/toolchains/ee-gcc2.96-realconv-lp7-fsv12-v1/bin'
        with tempfile.TemporaryDirectory() as folder:
            probe = Path(folder) / ('sizeof_' + source.name)
            probe.write_text(source.read_text(errors='surrogateescape') +
                             f'\nint __data_recover_sizeof = sizeof({ctype});\n')
            try:
                out = Path(folder) / 'o.s'
                result = subprocess.run(['timeout', '-k', '5', '30', str(cc / 'ee-gcc'), '-B' + str(cc) + '/',
                                         '-nostdinc', '-fno-builtin', '-S', '-O2', '-G8', '-Iinclude', '-I.',
                                         '-I' + str(source.parent), str(probe), '-o', str(out)],
                                        cwd=root, capture_output=True, text=True)
                if result.returncode == 0:
                    text = out.read_text()
                    found = re.search(r'^__data_recover_sizeof:\s*\n\s*\.word\s+(\d+)', text, re.M)
                    size = int(found[1]) if found else None
            finally:
                probe.unlink(missing_ok=True)
    except (OSError, KeyError, ValueError):
        size = None
    _SIZEOF[(tu, ctype)] = size
    return size


def grouped_storage(selected, indexes, aliases, pieces, root, inputs, binary):
    """Rejoin synthetic NOBITS labels within an explicitly declared C array."""
    by_key = {(r['unit'], dc.hx(r['address'])): r for r in selected}
    consumed, result = set(), []
    for row in sorted(selected, key=lambda r: (r['unit'], dc.hx(r['address']))):
        key = row['unit'], dc.hx(row['address'])
        if key in consumed:
            continue
        grouped = None
        if row['section'] in {'.bss', '.sbss'} and len(row['names']) == 1:
            choices = indexes[row['tu']].get(row['names'][0], [])
            shapes = {(d['type'], d['dimensions']) for d in choices}
            if len(shapes) == 1:
                ctype, dims = next(iter(shapes))
                bound = re.fullmatch(r'\[\s*(0[xX][0-9A-Fa-f]+|[0-9]+)\s*\]', dims)
                scalar = scalar_type(ctype, aliases[row['tu']])
                # An array of data pointers has the EE's four-byte element
                # (2026-10-31, main/tu106 s_apBlock spans synthetic labels).
                if scalar is None and re.fullmatch(r'[A-Za-z_][\w\s]*\*', clean_type(ctype)):
                    scalar = ('pointer', 4)
                # An array of a named record type: its element size is the
                # contract compiler's sizeof in this TU (the carve and the
                # whole-file gate verify the resulting storage).
                record_type = clean_type(re.sub(r'\b(?:const|volatile)\b', '', ctype))
                if scalar is None and bound and re.fullmatch(r'(?:(?:struct|union)\s+)?[A-Za-z_]\w*', record_type):
                    size = compiler_sizeof(root, row['tu'], record_type)
                    if size:
                        scalar = ('record', size)
                if bound and scalar:
                    start = key[1]
                    end = start + int(bound[1], 0) * scalar[1]
                    path = (root / row['source']).resolve()
                    piece = pieces[path]
                    if dc.hx(row['end']) < end <= piece['end'] and end - start <= 16 * 1024 * 1024:
                        members, cursor = [], start
                        while cursor < end:
                            item = by_key.get((row['unit'], cursor))
                            if (not item or any(item[k] != row[k] for k in ('tu', 'section', 'source'))):
                                break
                            if dc.hx(item['end']) > end and not (dc.hx(item['end']) - end <= 63 and
                                                               dc.hx(item['end']) % 64 == 0):
                                break
                            members.append(item)
                            cursor = dc.hx(item['end'])
                        meaningful = [s for s in binary(row['unit']).symbols if s.type == 1 and s.size > 0 and
                                      start < s.value < end and 0 < s.shndx < len(binary(row['unit']).sections) and
                                      binary(row['unit']).sections[s.shndx].name == row['section']]
                        if cursor >= end and len(members) > 1 and not meaningful:
                            grouped = dict(row, end=dc.h8(end), grouped_items=members,
                                           storage_owner=dict(name=row['names'][0], address=row['address'], size=end-start))
                            if cursor > end:
                                grouped.update(mapped_end=dc.h8(cursor), object_size=end-start,
                                               zero_padding_tail=cursor-end)
                            consumed.update((r['unit'], dc.hx(r['address'])) for r in members)
        result.append(grouped or row)
    return result


def build_plan(root, inventory=None, unit=None, objdiff_report=None, code_complete=False,
               all_scaffold=False):
    manifest = read_json(root / 'config/tu-build.json')
    tus = {t['id']: t for t in manifest['tus'] if t.get('in_scope') and t['unit'] in UNITS
           and (unit is None or t['unit'] == unit)}
    selected_owners, scope = objdiff_scope(root, tus, objdiff_report, unit, code_complete)
    scope.update(all_scaffold=all_scaffold, selected_tus=sorted(selected_owners))
    require(not (all_scaffold and inventory), '--all-scaffold cannot use a reachable-only inventory')
    include_closures = {}
    indexes, inputs, aliases = declaration_index(root, tus, include_closures)
    inputs[root / 'config/tu-build.json'] = sha(root / 'config/tu-build.json')
    inputs[root / 'config/originals.json'] = sha(root / 'config/originals.json')
    for path in (Path(__file__).resolve(), HERE / 'data_carve.py', HERE / 'elfinfo.py',
                 HERE / 'toolchain.py', HERE / 'data_types.py', HERE / 'cc_tu.sh', root / 'config/toolchain-identity.json'):
        inputs[path] = sha(path)
    metadata = scaffold_map(root, tus, inputs)
    document = (scaffold_inventory(root, metadata, inputs) if all_scaffold else
                read_json(inventory) if inventory else function_inventory(root, tus, inputs, metadata,indexes))
    require(document.get('schema') == 'remaining-scaffold-function-users/1', 'expected a frozen function-user scaffold inventory')
    require(not document.get('errors') and not document.get('mapping_gaps') and not document.get('function_mismatches'), 'function-user inventory has unresolved classification errors')
    selected = [r for r in document['remaining'] if r['tu'] in selected_owners and
                (all_scaffold or r['category'] in {'shared', 'recovered_c_only'})]
    if objdiff_report:
        inputs[Path(objdiff_report).resolve()] = sha(objdiff_report)
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

    selected = grouped_storage(selected, indexes, aliases, pieces, root, inputs, binary)
    # A disassembler's padding comment is not authority over a real C use.
    # Keep that comment in provenance, but honor explicit source references.
    c_references = collections.defaultdict(set)
    character_addresses = collections.defaultdict(set)
    for tu in tus.values():
        source = root / tu['path']
        if not source.is_file():
            continue
        code = re.sub(r'/\*.*?\*/|//[^\n]*', '', source.read_text(), flags=re.S)
        code = re.sub(r'\bextern\s+[^;]+;', '', code)
        code = re.sub(r'"(?:\\.|[^"\\])*"', '""', code)
        c_references[tu['unit']].update(re.findall(r'\b[A-Za-z_]\w*\b', code))
        character_addresses[tu['unit']].update(int(v, 16) for v in
            re.findall(r'\(\s*(?:const\s+)?char\s*\*\s*\)\s*(0[xX][0-9A-Fa-f]+)', code))
    pointer_references = collections.defaultdict(set)
    stream_pointer_references = collections.defaultdict(set)
    for path, piece in pieces.items():
        declarations = indexes[piece['tu']]
        if not any(d['type'].count('*') == 1 and
                   base_type(d['type'].replace('*', '')) in {'char', 'unsigned char', 'u8', 's8'}
                   for values in declarations.values() for d in values):
            continue
        inputs[path] = sha(path)
        _, items = dc.piece_items(path, piece['start'], piece['end'])
        for block in items:
            declared = [d for name in block['labels'] for d in declarations.get(name, [])]
            if not any(d['type'].count('*') == 1 and
                       base_type(d['type'].replace('*', '')) in {'char', 'unsigned char', 'u8', 's8'}
                       for d in declared):
                continue
            assembly = re.sub(r'/\*.*?\*/', '', '\n'.join(block['block']['lines']), flags=re.S)
            for operands in re.findall(r'^\s*\.(?:word|long|4byte)\s+([^\n#]+)',
                                       assembly, re.M):
                names = set(re.findall(r'\b[A-Za-z_]\w*\b', operands))
                pointer_references[piece['unit']].update(names)
                if any(d.get('data_role') == 'character_stream_pointer_table' for d in declared):
                    stream_pointer_references[piece['unit']].update(names)
    for item in selected:
        if item.get('unreferenced_padding') and (
                set(item['names']) & (c_references[item['unit']] | pointer_references[item['unit']]) or
                dc.hx(item['address']) in character_addresses[item['unit']]):
            item['disassembler_padding_comment'] = True
            item['unreferenced_padding'] = False
    # Keep unnamed/padding items visible as deferred rows. Type propagation may
    # consult another owner's pointee, but it never expands this publication scope.
    deferred = [r for r in selected if not r['names'] or r.get('unreferenced_padding')]
    selected = [r for r in selected if r not in deferred]
    dt.expand(root, tus, selected, pieces, indexes, inputs, binary, sha)
    outside_scope = [r for r in selected if r['tu'] not in selected_owners]
    selected = [r for r in selected if r['tu'] in selected_owners] + deferred

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
        if item.get('grouped_items'):
            cursor, lines = dc.hx(item['address']), []
            for member in item['grouped_items']:
                block = blocks[path][dc.hx(member['address'])]
                require(block['start'] == cursor and block['end'] == dc.hx(member['end']) and
                        set(block['labels']) == set(member['names']), 'grouped storage item identity or tiling differs')
                require(not any(lo < block['end'] and cursor < hi for lo, hi in covered[(unit, item['section'])]),
                        'grouped original storage overlaps C-linked DATA')
                lines.extend(block['block']['lines'])
                cursor = block['end']
            require(cursor == dc.hx(item.get('mapped_end', item['end'])),
                    'grouped storage extent differs from explicit array bound and original alignment')
            return path, dict(lines=lines)
        require(match['end'] == dc.hx(item['end']) and set(match['labels']) == set(item['names']), 'inventory bounds/labels differ from original scaffold')
        require(not any(lo < match['end'] and match['start'] < hi for lo, hi in covered[(unit, item['section'])]), 'original item already overlaps C-linked DATA')
        return path, match['block']

    # Borrow only consistent explicit consumer declarations for a uniquely
    # owned GLOBAL original datum, or a registered anonymous literal label.
    foreign = collections.defaultdict(list)
    for provider, declarations in indexes.items():
        for name, declarations_ in declarations.items():
            foreign[(tus[provider]['unit'], name)].extend((provider, d) for d in declarations_)
    for item in selected:
        for name in item['names']:
            own = indexes[item['tu']]
            if own.get(name):
                continue
            candidates = foreign.get((item['unit'], name), [])
            identities = original_data_identities(binary(item['unit']), name, dc.hx(item['address']))
            permitted = bool(identities and all(s.bind == 1 for s in identities))
            anonymous = bool(re.fullmatch(r'D_[0-9A-Fa-f]{8}', name) and len(item['names']) == 1)
            types = {d['type'] for provider, d in candidates}
            if not candidates or len(types) != 1 or not (permitted or anonymous):
                continue
            own[name] = [dict(d, foreign_declaration=True,
                              identity_basis='original GLOBAL DATA' if permitted else 'registered anonymous scaffold label')
                         for provider, d in candidates]
            for provider, declaration in candidates:
                pending, copied = re.findall(r'\b[A-Za-z_]\w*\b', declaration['type']), set()
                while pending:
                    alias = pending.pop()
                    if alias in copied or alias not in aliases[provider]:
                        continue
                    copied.add(alias)
                    choices = aliases[provider][alias]
                    aliases[item['tu']][alias].update(choices)
                    pending.extend(token for base in choices for token in re.findall(r'\b[A-Za-z_]\w*\b', base))
                include_closures[item['tu']] = sorted(set(include_closures[item['tu']]) | set(include_closures[provider]))
    record_catalogs = {tu: dt.record_catalog(root, include_closures[tu], inputs, sha)
                       for tu in {r['tu'] for r in selected}}
    for item in selected:
        if len(item['names']) != 1:
            continue
        name, tu = item['names'][0], item['tu']
        if indexes[tu].get(name):
            continue
        identities = original_data_identities(binary(item['unit']), name, dc.hx(item['address']))
        if len(identities) != 1 or identities[0].type != 1 or identities[0].size <= 0:
            continue
        symbol = identities[0]
        raw = dc.va_bytes(binary(item['unit']), symbol.value, symbol.size)
        if raw is None:
            continue
        culling = re.fullmatch(r'(s_aMapCullingVok)\d+', name)
        siblings = [n for n, values in indexes[tu].items() if culling and
                    re.fullmatch(re.escape(culling[1]) + r'\d+', n) and
                    any(d.get('source_definition') and d['type'] == 'const float' and
                        d['dimensions'] == '[27]' for d in values)]
        if culling and symbol.bind == 0 and symbol.size == 108 and len(siblings) >= 2:
            values = struct.unpack('<27f', raw)
            if (all(math.isfinite(v) for v in values) and values[18:] ==
                    (0., 0., 0., 0., 0., 0., -1., -1., -1.) and values[8] == values[17] == 1.0):
                indexes[tu][name] = [dict(type='const float', dimensions='[27]', path=tus[tu]['path'],
                    data_role='existing_float_table_family', sibling_tables=sorted(siblings),
                    basis='original LOCAL OBJECT108 and existing const-float[27] nine-field record family with identical terminal record')]
        if re.fullmatch(r'Tab\w+_Ang', name) and symbol.bind == 1 and symbol.size == 32:
            expected = [struct.unpack('<I', struct.pack('<f', i * math.pi / 4))[0] for i in range(8)]
            angle_steps = all(abs(actual - ideal) <= 1 for actual, ideal in zip(struct.unpack('<8I', raw), expected))
            neighbors = [s for s in binary(item['unit']).symbols if s.type == 1 and s.value == symbol.value + 32 and
                         any(d.get('source_definition') and d['type'] == 'float' for d in indexes[tu].get(s.name, []))]
            if angle_steps and len(neighbors) == 1:
                indexes[tu][name] = [dict(type='float', dimensions='[8]', path=tus[tu]['path'],
                    data_role='existing_float_angle_family', adjacent_table=neighbors[0].name,
                    basis='original GLOBAL OBJECT32 contains IEEE-float pi/4 angle steps within one ULP beside an existing typed float angle table; initializer preserves original values')]
    # Cross-owner character declarations include already-published C strings;
    # consulting them never adds an allocation outside selected DATA owners.
    scaffold_names = collections.defaultdict(list)
    for datum in document['remaining']:
        for name in datum['names']:
            scaffold_names[(datum['unit'], name)].append(datum)
    for item in selected:
        path, block = original_item(item)
        typed = [d for name in item['names'] for d in indexes[item['tu']].get(name, [])]
        if not any(d['type'].count('*') == 1 or pointer_type(d['type'], aliases[item['tu']]) for d in typed):
            continue
        assembly = re.sub(r'/\*.*?\*/', '', '\n'.join(block['lines']), flags=re.S)
        targets = set(re.findall(r'^\s*\.(?:word|long|4byte)\s+([A-Za-z_]\w*)\s*$', assembly, re.M))
        for name in targets:
            own = indexes[item['tu']]
            if own.get(name):
                continue
            candidates = foreign.get((item['unit'], name), [])
            if candidates and len({d['type'] for provider, d in candidates}) == 1:
                own[name] = [dict(d, foreign_declaration=True) for provider, d in candidates]
                continue
            locations = scaffold_names.get((item['unit'], name), [])
            if len(locations) != 1 or len(locations[0]['names']) != 1:
                continue
            datum = locations[0]
            character_types = [d['type'].replace('*', '').strip() for d in typed if d['type'].count('*') == 1 and
                               base_type(d['type'].replace('*', '')) in {'char', 'unsigned char', 'u8', 's8'}]
            if not character_types or datum['section'] not in {'.rodata', '.sdata', '.data'}:
                continue
            payload = dc.va_bytes(binary(datum['unit']), dc.hx(datum['address']),
                                  dc.hx(datum['end']) - dc.hx(datum['address']))
            if payload is None or len(payload) > 4096 or not payload.endswith(b'\0') or not dt.encoded_text(payload):
                continue
            own[name] = [dict(type=character_types[0], dimensions=f'[{len(payload)}]', path=datum['source'],
                             data_role='original_scaffold_character_pointee', declaration_only=True,
                             basis='typed character-pointer table targets this unique bounded original character item')]
    source_functions = collections.defaultdict(list)
    for tu in tus.values():
        source = root / tu['path']
        if not source.is_file():
            continue
        code = re.sub(r'/\*.*?\*/|//[^\n]*', '', source.read_text(), flags=re.S)
        inputs[source] = sha(source)
        for returns, name, parameters in re.findall(
                r'^\s*(?:static\s+|extern\s+)?([A-Za-z_][\w\s*]+?)\s+(\w+)'
                r'\s*\(([^;{}]*)\)\s*(?:;|\{)', code, re.M):
            source_functions[(tu['unit'], name)].append('extern ' + clean_type(returns) +
                                                       ' ' + name + '(' + parameters + ');')
    for item in selected:
        for name in item['names']:
            for declaration in indexes[item['tu']].get(name, []):
                ctype = declaration['type']
                if not function_pointer_type(ctype, aliases[item['tu']]):
                    continue
                path, block = original_item(item)
                assembly = re.sub(r'/\*.*?\*/', '', '\n'.join(block['lines']), flags=re.S)
                for target in re.findall(r'^\s*\.(?:word|long|4byte)\s+([A-Za-z_]\w*)\s*$', assembly, re.M):
                    prototypes = source_functions.get((item['unit'], target), [])
                    if prototypes:
                        indexes[item['tu']][target] = [dict(type='function', dimensions='',
                            path=tus[item['tu']]['path'], function_prototype=prototypes[0])]
    if 'main' in {t['unit'] for t in tus.values()}:
        original = binary('main')
        readers = [s for s in original.symbols if s.name == 'RES_GetLeaderSeName' and s.type == 2 and s.size == 108]
        prototypes = source_functions.get(('main', 'RES_GetLeaderSeName'), [])
        if len(readers) == 1 and any(re.search(r'\(\s*char\s*\*\s*\w*\s*\)', p) for p in prototypes):
            raw = dc.va_bytes(original, readers[0].value, readers[0].size)
            words = struct.unpack('<27I', raw)
            def instruction(index, opcode, base, target, immediate):
                word = words[index]
                return (word >> 26, (word >> 21) & 31, (word >> 16) & 31, word & 0xffff) == (opcode, base, target, immediate)
            # Actual original reader copies table-selected bytes 0/1 into
            # its char* output and appends ASCII '_' at byte two.
            copied_prefix = (instruction(3, 9, 0, 5, ord('_')) and
                             instruction(9, 36, 6, 3, 0) and instruction(10, 40, 7, 3, 0) and
                             instruction(11, 36, 6, 2, 1) and instruction(13, 40, 7, 5, 2) and
                             instruction(14, 40, 7, 2, 1) and
                             words[6] >> 26 == 15 and ((words[6] >> 16) & 31) == 6 and
                             words[8] >> 26 == 35 and ((words[8] >> 21) & 31) == 6 and
                             ((words[8] >> 16) & 31) == 6)
            if copied_prefix:
                low = words[8] & 0xffff
                address = ((words[6] & 0xffff) << 16) + (low if low < 0x8000 else low - 0x10000)
                tables = [s for s in original.symbols if s.value == address and s.type == 1 and s.size == 28]
                if len(tables) == 1:
                    targets = struct.unpack('<7I', dc.va_bytes(original, address, 28))
                    for item in selected:
                        if item['unit'] != 'main' or dc.hx(item['address']) not in targets or len(item['names']) != 1:
                            continue
                        name = item['names'][0]
                        payload = dc.va_bytes(original, dc.hx(item['address']), dc.hx(item['end'])-dc.hx(item['address']))
                        if (not indexes[item['tu']].get(name) and payload is not None and 3 <= len(payload) <= 16 and
                                all(chr(v).isascii() and chr(v).isalnum() for v in payload[:2]) and not any(payload[2:])):
                            indexes[item['tu']][name] = [dict(type='const char', dimensions=f'[{len(payload)}]',
                                path=tus[item['tu']]['path'], data_role='original_name_prefix',
                                reader='RES_GetLeaderSeName', reader_sha256=hashlib.sha256(raw).hexdigest(),
                                pointer_table=dc.h8(address),
                                basis='original char-name API reads two table-selected character bytes and appends an underscore')]
    for item in selected:
        if len(item['names']) != 1 or item['section'] not in {'.rodata', '.sdata', '.data'}:
            continue
        name = item['names'][0]
        if name not in pointer_references[item['unit']] or indexes[item['tu']].get(name):
            continue
        payload = dc.va_bytes(binary(item['unit']), dc.hx(item['address']),
                              dc.hx(item['end']) - dc.hx(item['address']))
        font_color = bool(payload and payload[0] == 12 and len(payload) >= 5 and not any(payload[4:]))
        proved_stream = name in stream_pointer_references[item['unit']]
        if payload is not None and len(payload) <= 4096 and payload.endswith(b'\0') and (dt.encoded_text(payload) or font_color or proved_stream):
            indexes[item['tu']][name] = [dict(type='const char', dimensions='[]',
                path=item['source'], data_role='font_character_stream' if font_color or proved_stream else 'character_pointer_pointee',
                basis='an explicitly typed character-pointer table references this bounded original terminated character item')]
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
            require(row['names'], 'unnamed original item requires a source identity/layout hypothesis')
            require(not row.get('unreferenced_padding'), 'unreferenced padding is not an evidenced C datum')
            if unit not in binaries:
                original = tc.game_dir() / originals[unit]['file']
                require(sha(original) == originals[unit]['sha256'], 'original binary SHA-256 differs from the build identity')
                inputs[original] = originals[unit]['sha256']
                binaries[unit] = Elf(original.read_bytes())
            original = binaries[unit]
            source, kind, evidence = definition(row, block, original, indexes[tu_id], aliases[tu_id], record_catalogs[tu_id])
            types = [evidence['type']] + evidence.get('pointer_declarations', []) + evidence.get('probe_typedefs', [])
            typedefs = (evidence.get('probe_typedefs', []) if evidence.get('data_role') == 'existing_typed_record' else
                        [t for ctype in types for t in probe_typedefs(ctype, aliases[tu_id])] + evidence.get('probe_typedefs', []))
            evidence = dict(evidence, probe_typedefs=list(dict.fromkeys(typedefs)))
            if evidence.get('object_extent'):
                row['mapped_end'] = row['end']
                row['end'] = dc.h8(address + evidence['object_extent'])
                row['object_size'] = evidence['object_extent']
                row['zero_padding_tail'] = evidence.get('uncredited_alignment_extent', 0)
            row.update(status='generated_candidate', kind=kind, definition=source, type_evidence=evidence,
                       expected_binding=0 if original_local(original, row['names'][0], address) else 1,
                       byte_sha256=None if kind == 'storage' else hashlib.sha256(dc.va_bytes(original, address, dc.hx(row['end']) - address)).hexdigest())
        except (ValueError, KeyError, dc.CarveError) as error:
            row.update(status='manual', reason=str(error))
        rows.append(row)
    return dict(schema='automatic-data-plan/1', accepted=False, root=str(root),
                inventory=str(Path(inventory).resolve()) if inventory else
                'all-mapped-scaffold' if all_scaffold else 'native-function-relocations',
                scope=scope, out_of_scope_dependencies=outside_scope,
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
        # The probe is the complete candidate TU (2026-10-31): hand-typed rows
        # use the TU's own types, headers and same-TU pointees, so an isolated
        # common.h probe cannot compile them.  It is built the way
        # data_integrate.publish appends definitions.
        (folder / 'probe.c').write_text(full_candidate_source(base.read_text(), rows))
        results.append(dict(tu=tu_id, folder=str(folder), additions_sha256=sha(folder / 'additions.c'),
                            probe_sha256=sha(folder / 'probe.c'),
                            baseline_source=str(base), baseline_sha256=sha(base) if base.is_file() else None, locations=len(rows)))
    report = dict(schema='automatic-data-candidates/1', accepted=False, scope='source_candidates_only', plan_sha256=hashlib.sha256(json.dumps(plan, sort_keys=True).encode()).hexdigest(), candidates=results)
    lease.json(output / 'prepared.json', report)
    return report


def full_candidate_source(text, rows):
    """The TU source with `rows` applied as data_integrate.publish applies them."""
    import header_harvest
    for row in rows:
        for r in row['type_evidence'].get('source_replacements', []):
            require(text.count(r['old']) == 1, 'source type correction does not have one occurrence')
            text = text.replace(r['old'], r['new'])
    definitions = {}
    for row in sorted(rows, key=lambda r: dc.hx(r['address'])):
        d = row['definition']
        if row['kind'] != 'storage' and row['section'] in ('.data', '.rodata'):
            declaration, initializer = d.split('=', 1)
            d = declaration.rstrip() + ' __attribute__((section("' + row['section'] + '"))) =' + initializer
        definitions[row['names'][0]] = d
    for name, d in definitions.items():
        if not d.startswith('static '):
            continue
        declaration = d.split('=', 1)[0].strip().rstrip(';')
        declaration = re.sub(r'\s*__attribute__\(\(section\("[^"]*"\)\)\)', '', declaration) + ';'
        pattern = r'\b(?:static\s+)?extern\s+[^;{}\n]*\b' + re.escape(name) + r'\s*(?:\[[^\]]*\]\s*)*;'
        view, _, _ = header_harvest.mask_source(text)
        functions = [f for f in header_harvest.split_top_level(view) if f.has_body]
        edits, forwards = [], []
        for match in re.finditer(pattern, text):
            enclosing = [f for f in functions if f.start <= match.start() < f.stop]
            (forwards.append(enclosing[0].start) if enclosing else edits.append((match.start(), match.end(), declaration)))
        if forwards and not any(a < min(forwards) for a, b, v in edits):
            edits.append((min(forwards), min(forwards), declaration + '\n\n'))
        for a, b, v in sorted(edits, reverse=True):
            text = text[:a] + v + text[b:]
    local = {n for n, d in definitions.items() if d.startswith('static ')}
    declarations = list(dict.fromkeys(d for r in rows for d in r['type_evidence'].get('pointer_declarations', [])))
    declarations = [re.sub(r'^extern\b', 'static', d) if any(re.search(r'\b' + re.escape(n) + r'\s*\[', d) for n in local)
                    else d for d in declarations]
    prelude = list(dict.fromkeys(p for r in rows for p in r['type_evidence'].get('source_prelude', [])))
    return (text.rstrip() + '\n\n' + '\n'.join(prelude + declarations) + '\n\n' +
            '\n\n'.join(definitions.values()) + '\n')


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
    # The probe assembles with the TU contract's own assembler (2026-10-27: game C TUs use the
    # single swap-mask release with -g); the sha check below still binds it to the contract.
    assemblers = {sha(tc.file(name)): name for name in ('ee-as-la29-vsqrt-swap', 'ee-as-la29-vsqrt')}
    results = []
    for tu_id, rows in grouped(plan).items():
        facts = plan['tus'][tu_id]
        flags = facts['contract']['flags']
        require(flags in (['-O2', '-G0'], ['-O2', '-G8']), 'automatic probe refuses an unsupported compile contract')
        assembler = tc.file(assemblers.get(facts['contract'].get('assembler_sha256'), 'ee-as-la29-vsqrt'))
        as_options = ' '.join(facts['contract'].get('assembler_options') or [])
        require(facts['contract']['compiler_profile'] in ('ee-gcc2_96-realconv', 'ee-gcc2_96-realconv-lp7') and
                sha(compiler / 'cc1') == facts['contract']['cc1_sha256'] and
                sha(assembler) == facts['contract']['assembler_sha256'], 'probe tools differ from the original TU contract')
        folder = output / tu_id.replace('/', '_')
        lease.check([folder / 'compile.log', folder / 'probe.o', folder / 'probe.i', folder / 'probe.s', folder / 'probe.o.asmsg'])
        # compile next to the TU source so its local includes resolve
        source_copy = (root / facts['path']).with_name('.probe_' + str(os.getpid()) + '_' + Path(facts['path']).name)
        source_copy.write_bytes((folder / 'probe.c').read_bytes())
        try:
            code = run(['env', 'XENO_EE_AS_OPTIONS=' + as_options, 'bash', root / 'tools/tu/cc_tu.sh', source_copy, folder / 'probe.o', compiler, assembler, flags[-1], root / 'build' / facts['unit'], root], root / 'build' / facts['unit'], folder / 'compile.log')
        finally:
            source_copy.unlink(missing_ok=True)
        report = dict(tu=tu_id, returncode=code, verified=[], requires_linked_gate=[], problems=[])
        if code == 0:
            elf = Elf((folder / 'probe.o').read_bytes())
            symbols = {s.name: s for s in elf.symbols if s.name}
            relocs = dc.read_relocations(elf)
            for row in rows:
                name = row['names'][0]
                try:
                    symbol = symbols[name]
                    require(symbol.bind == row.get('expected_binding',
                            0 if row['definition'].startswith('static ') else 1),
                            'compiler DATA binding differs from the original source identity')
                    extent = dc.hx(row['end']) - dc.hx(row['address'])
                    if row['kind'] == 'storage':
                        require(symbol.shndx in (0xfff2, 0xff03) or (symbol.shndx < len(elf.sections) and elf.sections[symbol.shndx].type == 8), 'storage is not COMMON/NOBITS')
                        if symbol.type == 0 and symbol.bind == 0 and symbol.size == 0:
                            emission=emitted_storage((folder/'probe.s').read_text()).get(name,{})
                            originals=read_json(root/'config/originals.json')['units']
                            original=Elf((tc.game_dir()/originals[facts['unit']]['file']).read_bytes())
                            identities=original_data_identities(original,name,dc.hx(row['address']))
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
    p.add_argument('--unit', choices=UNITS, help='restrict DATA owners to this image')
    p.add_argument('--objdiff-report', type=Path, help='pin a native objdiff JSON report')
    p.add_argument('--code-complete', action='store_true', help='select report owners with nonempty code at exactly 100 percent')
    p.add_argument('--all-scaffold', action='store_true', help='include unreferenced mapped DATA and visible deferred items')
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
        result = build_plan(root, args.inventory, args.unit, args.objdiff_report,
                            args.code_complete, args.all_scaffold)
        if args.output:
            lease.json(args.output, result)
        print(json.dumps(dict(counts=result['counts'], total=len(result['locations']),
                              selected_tus=len(result['scope']['selected_tus']),
                              generated_tus=len(grouped(result))), indent=2))
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
