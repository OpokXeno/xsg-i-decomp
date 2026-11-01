"""Evidenced DATA type propagation and byte-oriented format initializers.

These helpers interpret source declarations and original DATA identities. They
do not build objects, change compile contracts, or provide recovery credit.
"""
import collections
import ast
import re
import struct

import data_carve as dc


def encoded_text(payload):
    """Recognize text with font parameters, preserving embedded zero bytes."""
    body = payload.rstrip(b'\0')
    if not body:
        return True
    # Character storage is separately established by C types. This check
    # prevents an untyped binary payload being inferred as a string.
    chunks = re.findall(rb'[ -~]{3,}', body)
    if chunks and sum(map(len, chunks)) >= len(body) // 2:
        return True
    try:
        text = body.decode('euc_jp')
    except UnicodeDecodeError:
        return False
    return all(c.isprintable() or c in '\n\r\t\u3000' for c in text)


def expand(root, tus, selected, pieces, indexes, inputs, binary, sha):
    """Propagate character pointees through the evidenced resource hierarchy.

    ResourceKeyEntry's three pointer fields are already modeled in recovered
    C. A hierarchy node has another record table in its second field; a leaf
    has the string-pointer list the existing aliases field describes. The
    void pointer view accommodates both while retaining the 12-byte layout.
    """
    blocks = {}
    by_unit = collections.defaultdict(list)
    for path, piece in pieces.items():
        by_unit[piece['unit']].append((piece['start'], piece['end'], path, piece))

    def location(unit, va):
        hits = [p for p in by_unit[unit] if p[0] <= va < p[1]]
        if len(hits) != 1:
            raise ValueError('pointer target has no unique original DATA piece')
        lo, hi, path, piece = hits[0]
        if path not in blocks:
            inputs[path] = sha(path)
            _, items = dc.piece_items(path, lo, hi)
            blocks[path] = {item['start']: item for item in items}
        item = blocks[path].get(va)
        if item is None or len(item['labels']) != 1:
            raise ValueError('pointer target needs an interior/alias layout')
        return dict(unit=unit, tu=piece['tu'], section=piece['section'],
                    address=va, end=item['end'], name=item['labels'][0])

    def payload(item):
        return dc.va_bytes(binary(item['unit']), item['address'], item['end']-item['address'])

    def suggest(item, ctype, dimensions, role, basis):
        choices = indexes[item['tu']].setdefault(item['name'], [])
        if not choices:
            choices.append(dict(type=ctype, dimensions=dimensions,
                                path=basis, data_role=role,
                                basis='C resource pointer-field type propagated to its original pointee'))
        return item

    def string_target(unit, va, basis):
        item = location(unit, va)
        raw = payload(item)
        if raw is None or len(raw) > 4096 or not raw.endswith(b'\0') or not encoded_text(raw):
            raise ValueError('character pointee lacks a bounded terminated text layout')
        return suggest(item, 'const char', '[]', 'resource_string', basis)

    def string_list(unit, va, basis):
        item = location(unit, va)
        raw = payload(item)
        if raw is None or len(raw) % 4 or len(raw) > 65536:
            raise ValueError('resource alias list lacks a pointer-array extent')
        targets = []
        for word in struct.unpack('<'+'I'*(len(raw)//4), raw):
            targets.append(None if word == 0 else string_target(unit, word, basis))
        suggest(item, 'const char *', '[]', 'resource_alias_list', basis)
        return item, targets

    roots = {}
    for tu, declarations in indexes.items():
        path = tus[tu]['path']
        if not (root/path).is_file():
            continue
        code = (root/path).read_text()
        record = re.search(r'typedef\s+struct\s+ResourceKeyEntry\s*\{(.*?)\}\s*ResourceKeyEntry\s*;', code, re.S)
        if not record or not re.fullmatch(
                r'\s*const\s+char\s*\*\s*key\s*;\s*const\s+char\s*\*\s*const\s*\*\s*aliases\s*;'
                r'\s*const\s+void\s*\*\s*unmodeled_08\s*;\s*', record[1]):
            continue
        names = [r['names'][0] for r in selected if r['tu']==tu and
                 any(d['type']=='const char *' and d['dimensions'] for d in declarations.get(r['names'][0],[]))]
        if not names:
            continue
        typedef = re.sub(r'const\s+char\s*\*\s*const\s*\*\s*aliases\s*;',
                         'const void *aliases;', record[0])
        replacements = [dict(old=record[0],new=typedef)]
        # Split grouped declarations before changing the array element type.
        # The root record tables and their leaf string lists have different
        # types even though both were initially declared as char-pointer arrays.
        for declaration in re.findall(r'\bextern\s+[^;{}]+;', code):
            declared = re.findall(r'\b([A-Za-z_]\w*)\s*\[\s*\]', declaration)
            if declared and set(declared) <= set(names):
                replacements.append(dict(old=declaration,new='\n'.join(
                    'extern ResourceKeyEntry '+n+'[];' for n in declared)))
        for name in names:
            item = next(r for r in selected if r['tu']==tu and r['names'][0]==name)
            raw=dc.va_bytes(binary(item['unit']),dc.hx(item['address']),dc.hx(item['end'])-dc.hx(item['address']))
            if len(raw)%12:
                continue
            entries=[]; dependencies=[]
            try:
                for offset in range(0,len(raw),12):
                    key,aliases,tail=struct.unpack_from('<III',raw,offset)
                    a=None if not key else string_target(item['unit'],key,path)
                    b=None if not aliases else string_list(item['unit'],aliases,path)[0]
                    c=None if not tail else string_target(item['unit'],tail,path)
                    entries.append([a,b,c]);dependencies.extend(x for x in (a,b,c) if x)
            except (ValueError, dc.CarveError):
                continue
            roots[(tu,name)]=dict(entries=entries,dependencies=dependencies,typedef=typedef,
                                   source_replacements=replacements)
            for target in dependencies:
                declarations.setdefault(target['name'], indexes[target['tu']][target['name']])
            declarations[name]=[dict(type='ResourceKeyEntry',dimensions='[]',path=path,
                                     data_role='resource_record_array',resource_record=roots[(tu,name)])]
        # Bring pointee declarations into the referring TU, while definitions
        # remain with their original owners. Include closure indexes do not
        # otherwise know about newly recovered cross-TU strings.
        for item in selected:
            if item['tu'] != tu:
                continue
            name=item['names'][0]
            evidence=declarations.get(name, [])
            if not any(d.get('data_role') == 'resource_alias_list' for d in evidence):
                continue
            raw=dc.va_bytes(binary(item['unit']),dc.hx(item['address']),dc.hx(item['end'])-dc.hx(item['address']))
            for va in struct.unpack('<'+'I'*(len(raw)//4),raw):
                if not va:
                    continue
                target=location(item['unit'],va)
                declarations.setdefault(target['name'], indexes[target['tu']][target['name']])

    selected_keys = {(r['unit'], dc.hx(r['address'])) for r in selected}
    for tu, facts in tus.items():
        path = facts['path']
        if not (root/path).is_file():
            continue
        code = (root/path).read_text()
        # Explicit character-pointer constants retain the original pointee
        # type even when no named extern appeared in the recovered table.
        for va in re.findall(r'\(\s*(?:const\s+)?char\s*\*\s*\)\s*(0x[0-9A-Fa-f]+)', code):
            address = int(va,16)
            if (facts['unit'],address) in selected_keys:
                string_target(facts['unit'],address,path)

        if not re.search(r'func_51\s*\[\s*selection\s*-\s*1\s*\]\s*\(',code):
            continue
        header = root/'src/main/xgl_menu.h'
        if facts['unit'] != 'main' or not header.is_file():
            continue
        inputs[header] = sha(header)
        types=header.read_text()
        row_type=re.search(r'typedef struct XglMenuRow\s*\{.*?\}\s*XglMenuRow;',types,re.S)
        node_type=re.search(r'struct XglMenuNode\s*\{.*?\};',types,re.S)
        if not row_type or not node_type:
            continue
        typedefs=['typedef struct XglMenuNode XglMenuNode;',row_type[0],node_type[0]]
        action='#define XGL_MENU_ACTION(id) ((XglMenuNode *)(id))'
        prelude=['#include "xgl_menu.h"',
                 '/* Leaf rows encode the action ID in the menu next-node slot. */\n'+action]
        for name in re.findall(r'xglMenuOpen\s*\(\s*-1\s*,\s*&\s*(\w+)\s*\)',code):
            candidates=[]
            for lo,hi,original,piece in by_unit[facts['unit']]:
                if piece['tu']!=tu or piece['section']!='.data':
                    continue
                if original not in blocks:
                    inputs[original]=sha(original)
                    _,items=dc.piece_items(original,lo,hi)
                    blocks[original]={it['start']:it for it in items}
                candidates.extend(it for it in blocks[original].values() if it['labels']==[name])
            if len(candidates)!=1 or candidates[0]['end']-candidates[0]['start']!=24:
                continue
            menu=dc.va_bytes(binary(facts['unit']),candidates[0]['start'],24)
            node=location(facts['unit'],struct.unpack_from('<I',menu,16)[0])
            if (node['unit'],node['address']) not in selected_keys:
                continue
            node_raw=payload(node)
            if len(node_raw)!=28:
                continue
            rows=location(facts['unit'],struct.unpack_from('<I',node_raw,24)[0])
            raw=payload(rows)
            if len(raw)!=node_raw[0]*8:
                continue
            dependencies=[];entries=[]
            for label,selector in struct.iter_unpack('<II',raw):
                if not 1<=selector<=node_raw[0]:
                    raise ValueError('menu leaf selector is outside its evidenced dispatch range')
                target=string_target(facts['unit'],label,path)
                indexes[tu].setdefault(target['name'],indexes[target['tu']][target['name']])
                dependencies.append(target)
                entries.append('    { '+target['name']+', XGL_MENU_ACTION('+str(selector)+') }')
            pointers=[f"extern {indexes[d['tu']][d['name']][0]['type']} {d['name']}[];" for d in dependencies]
            row_evidence=dict(type='XglMenuRow',dimensions='[]',path=path,data_role='menu_leaf_rows',
                              record_size=8,initializer='{\n'+',\n'.join(entries)+'\n}',
                              probe_typedefs=typedefs+[action],source_prelude=prelude,
                              pointer_declarations=pointers,
                              basis='original xglMenuNode rows layout and recovered selection-minus-one dispatch')
            indexes[tu][rows['name']]=[row_evidence]
            selected_byte=node_raw[4] if node_raw[4]<128 else node_raw[4]-256
            initializer='{ '+str(node_raw[0])+', { '+', '.join(map(str,node_raw[1:4]))+' }, '+str(selected_byte)+', { '+', '.join(map(str,node_raw[5:24]))+' }, '+rows['name']+' }'
            indexes[tu][node['name']]=[dict(type='XglMenuNode',dimensions='',path=path,
                 data_role='menu_node',record_size=28,initializer=initializer,
                 probe_typedefs=typedefs,source_prelude=prelude,
                 pointer_declarations=['extern XglMenuRow '+rows['name']+'[];'],
                 basis='original 28-byte menu node and recovered XglMenuNode partial layout')]

    # Recover the static class descriptor with the same partial view as the
    # owning C allocator. Its original mapped extent and xmalloc size establish the
    # four-byte unmodeled tail omitted from the previous caller-only view.
    for row in selected:
        if row['section'] not in ('.bss','.sbss') or row['names']!=['_dummyClass']:
            continue
        tu=row['tu'];path=tus[tu]['path'];header=root/'src/main/find_native_method.h'
        code=(root/path).read_text();header_code=header.read_text()
        allocation=re.search(r'ClassEntry\s*\*\s*newClass\s*\(void\).*?xmalloc\(\s*(0x[0-9A-Fa-f]+)\s*,',code,re.S)
        record=re.search(r'typedef struct ClassEntry\s*\{.*?\}\s*ClassEntry;',header_code,re.S)
        if not allocation or not record:
            continue
        extent=dc.hx(row['end'])-dc.hx(row['address'])
        if extent!=int(allocation[1],16) or extent!=64:
            continue
        addition='    u8 unmodeled_3c[4]; /* Remaining bytes of the 0x40-byte class allocation. */\n'
        if 'unmodeled_3c' in record[0]:
            expanded=record[0];edits=[]
        else:
            expanded=record[0].replace('} ClassEntry;',addition+'} ClassEntry;')
            edits=[dict(path=str(header.relative_to(root)),old=record[0],new=expanded)]
        indexes[tu]['_dummyClass']=[dict(type='ClassEntry',dimensions='',path=path,
               data_role='class_descriptor_storage',record_size=extent,
               probe_typedefs=[expanded],support_edits=edits,
               basis='original 0x40-byte mapped storage and recovered ClassEntry xmalloc allocation')]
    _correct_record_views(root, tus, selected, indexes, inputs, binary, sha)
    return roots


def _correct_record_views(root, tus, selected, indexes, inputs, binary, sha):
    """Correct two closed reader-proven record views, never enlarge padding.

    The current consumers already pass the vector objects through Vector4
    pointers, and their original callees copy all sixteen bytes. The defocus
    packet has the same fixed GIF/VIF layout as its existing 64-byte header.
    Corrections are candidate declaration replacements, not published edits.
    """
    def source(path):
        path = root / path
        inputs[path] = sha(path)
        return path.read_text()

    def function_words(original, name):
        functions = [s for s in original.symbols if s.name == name and s.type == 2 and s.size]
        if len(functions) != 1 or functions[0].size % 4:
            return []
        symbol = functions[0]
        raw = dc.va_bytes(original, symbol.value, symbol.size)
        return list(struct.unpack('<' + 'I' * (len(raw) // 4), raw)) if raw else []

    def has_quad_copy(words):
        # Original lq/sq through one register, zero offsets. This is only
        # reader evidence; no assembly or opcodes are emitted as DATA.
        for load, store in zip(words, words[1:]):
            if (load >> 26 == 0x1e and store >> 26 == 0x1f and
                    load & 0xffff == 0 and store & 0xffff == 0 and
                    (load >> 16) & 31 == (store >> 16) & 31):
                return True
        return False

    for row in selected:
        tu = row['tu']
        declarations = indexes[tu].get(row['names'][0], []) if len(row['names']) == 1 else []
        if not declarations:
            continue
        name = row['names'][0]
        owner_path = tus[tu]['path']
        original = binary(row['unit'])
        address, extent = dc.hx(row['address']), dc.hx(row['end']) - dc.hx(row['address'])
        identities = [s for s in original.symbols if s.value == address and s.type == 1 and s.size == extent]
        if len(identities) != 1:
            continue
        if (owner_path == 'src/main/set_path.c' and extent == 16 and
                all(d['type'] == 'SetPathEffectVector' and not d['dimensions'] for d in declarations)):
            code = source(owner_path)
            call = (r'\bsefCreateEffectCf\s*\(\s*\w+\s*,\s*'
                    r'\(\s*const\s+Vector4\s*\*\s*\)\s*&\s*(\w+)\s*,\s*'
                    r'\(\s*const\s+Vector4\s*\*\s*\)\s*&\s*(\w+)\s*\)')
            if not any(name in pair for pair in re.findall(call, code)):
                continue
            vector = source('include/shared.h')
            header = source('include/main/sef.h')
            if not (re.search(r'typedef\s+struct\s+Vector4\s*\{\s*float\s+x\s*;\s*float\s+y\s*;\s*float\s+z\s*;\s*float\s+w\s*;\s*\}\s*Vector4\s*;', vector) and
                    re.search(r'sefCreateEffectCf\s*\(\s*int\s+\w+\s*,\s*const\s+Vector4\s*\*\s*\w+\s*,\s*const\s+Vector4\s*\*\s*\w+\s*\)', header) and
                    has_quad_copy(function_words(original, 'sefCreateEffectCf2')) and
                    has_quad_copy(function_words(original, 'sefCreateScheduler2'))):
                continue
            old = 'extern SetPathEffectVector ' + name + ';'
            if code.count(old) != 1:
                continue
            indexes[tu][name] = [dict(declarations[0], type='Vector4',
                source_replacements=[dict(old=old, new='extern Vector4 ' + name + ';')],
                basis='original 16-byte OBJECT, existing Vector4 casts and prototype, original position/orientation lq/sq consumers',
                reader_evidence=['sefCreateEffectCf2', 'sefCreateScheduler2'])]
        elif (owner_path == 'src/main/game_defocus.c' and extent == 64 and
                all(d['type'] == 'DefocusTexturedHeader' and not d['dimensions'] for d in declarations)):
            code = source(owner_path)
            old = 'extern DefocusTexturedHeader ' + name + ';'
            raw = dc.va_bytes(original, address, extent)
            if raw is None or code.count(old) != 1:
                continue
            words = struct.unpack('<16I', raw)
            # Closed fixed register-write header: original VIF/GIF prefix,
            # three register/value pairs and zero high halves. Keep the
            # selector 0x44 numeric rather than inventing its source name.
            expected = (0, 0, 0, 0x51000003, 0x8001, 0x20000000, 0xee, 0,
                        0x71001, 0, 0x47, 0, 0x44, 0, 0x42, 0)
            read = r'\bsceVif1PkAddDataN\s*\(\s*\w+\s*,\s*&\s*' + re.escape(name) + r'\s*,\s*16\s*\)'
            if (words != expected or not re.search(read, code) or
                    not re.search(r'typedef\s+struct\s+DefocusSolidPacketHeader\s*\{', code) or
                    not re.search(r'static\s+DefocusSolidPacketHeader\s+defocus_solid_packet_header\s*=\s*\{', code)):
                continue
            indexes[tu][name] = [dict(declarations[0], type='DefocusSolidPacketHeader',
                source_replacements=[dict(old=old, new='extern DefocusSolidPacketHeader ' + name + ';')],
                basis='original 64-byte OBJECT and 16-word packet reader match the existing fixed GIF/VIF header layout; selector 0x44 differs from the solid template')]


def sprite_uv_definition(prefix, payload):
    """Keep the byte API while naming each evidenced u/v/width/height record."""
    if len(payload)%8:
        raise ValueError('sprite UV extent does not contain whole eight-byte records')
    macro = ('#define SPRITE_UV_BYTES(u, v, width, height) \\\n'
             '    (u) & 255, (u) >> 8, (v) & 255, (v) >> 8, \\\n'
             '    (width) & 255, (width) >> 8, (height) & 255, (height) >> 8')
    entries=[f'    SPRITE_UV_BYTES({u}, {v}, {w}, {h})' for u,v,w,h in
             struct.iter_unpack('<HHHH',payload)]
    source=prefix+f'[{len(payload)}] = {{\n'+',\n'.join(entries)+'\n};'
    return source,macro


class RecordCatalog(dict):
    """Existing definitions plus pinned source text for bounded use evidence."""


def record_catalog(root, include_closure_paths, inputs, sha):
    """Read complete, existing record definitions from one TU's include closure.

    The catalog does not invent fields or infer a layout from the payload.
    Unsupported C declarators remain unsupported; compiler probes and the
    linked DATA gate still decide whether a generated initializer is exact.
    """
    catalog = collections.defaultdict(list)
    source_texts = []
    for path in sorted(set(include_closure_paths), key=str):
        if not path.is_file():
            continue
        inputs[path] = sha(path)
        code = re.sub(r'/\*.*?\*/|//[^\n]*', '', path.read_text(), flags=re.S)
        source_texts.append(code)
        pattern = r'\b(typedef\s+)?(struct|union)\s*([A-Za-z_]\w*)?\s*\{'
        for match in re.finditer(pattern, code):
            depth, end = 1, match.end()
            while end < len(code) and depth:
                depth += (code[end] == '{') - (code[end] == '}')
                end += 1
            if depth:
                continue
            tail = re.match(r'\s*([A-Za-z_]\w*)?\s*;', code[end:])
            if not tail:
                continue
            typedef, kind, tag = match.groups()
            alias = tail[1] if typedef else None
            # A non-typedef with a declarator is an object declaration, not
            # a reusable named record definition.
            if not alias and (not tag or tail[1]):
                continue
            declaration = code[match.start():end + tail.end()]
            item = dict(kind=kind, body=code[match.end():end-1],
                        declaration=declaration, path=str(path.relative_to(root)),
                        tag=tag, alias=alias)
            for name in ([alias] if alias else []) + ([kind + ' ' + tag] if tag else []):
                if not any(x['declaration'] == declaration for x in catalog[name]):
                    catalog[name].append(item)
    result = RecordCatalog(catalog)
    result.source_texts = source_texts
    return result


class RecordLayoutError(ValueError):
    """A source record needs a more complete source/layout hypothesis."""


def _record_split(text, delimiter):
    depth, start, parts = 0, 0, []
    for index, char in enumerate(text):
        depth += char in '{[('
        depth -= char in '}])'
        if char == delimiter and depth == 0:
            parts.append(text[start:index].strip())
            start = index + 1
    tail = text[start:].strip()
    if tail:
        parts.append(tail)
    return parts


def _record_bound(text):
    """Evaluate only bounded integer arithmetic appearing literally in C."""
    text = re.sub(r'(?<=\d)[uUlL]+\b', '', text.strip())
    def value(node):
        if isinstance(node, ast.Constant) and type(node.value) is int:
            return node.value
        if isinstance(node, ast.UnaryOp) and isinstance(node.op, (ast.UAdd, ast.USub)):
            return value(node.operand) * (-1 if isinstance(node.op, ast.USub) else 1)
        if isinstance(node, ast.BinOp) and isinstance(node.op, (ast.Add, ast.Sub, ast.Mult)):
            a, b = value(node.left), value(node.right)
            return a+b if isinstance(node.op, ast.Add) else a-b if isinstance(node.op, ast.Sub) else a*b
        raise RecordLayoutError('record bound is not explicit integer arithmetic')
    try:
        result = value(ast.parse(text, mode='eval').body)
    except (SyntaxError, ValueError, TypeError, RecursionError):
        raise RecordLayoutError('record bound is not explicit integer arithmetic') from None
    if not 0 < result <= 65536:
        raise RecordLayoutError('record bound is outside the bounded layout')
    return result


def record_definition(row, block, original, declarations, records, aliases,
                      numeric_literal, storage=''):
    """Decode already modeled numeric records, preserving their named fields.

    Accepts complete structs, their arrays, and a union's existing first
    member when that member covers the union extent. No pointer or bitfield
    initializer inference, padding fields, opaque payload types, or assembler
    is generated. Explicit pointer fields are admitted only for uninitialized
    storage, where the established EE ABI supplies their four-byte extent.
    Returns None for a scalar/non-record declaration and raises
    RecordLayoutError for an evidenced record whose layout is unsupported.
    """
    if len(row['names']) != 1:
        return None
    name = row['names'][0]
    evidence = declarations.get(name, [])
    choices = {(d['type'], d['dimensions']) for d in evidence}
    if not choices or len({t for t, _ in choices}) != 1:
        return None
    ctype = next(iter(choices))[0]
    base = re.sub(r'\b(?:const|volatile)\b', '', ctype).strip()
    if base not in records:
        return None
    dims = {d for _, d in choices if d not in ('[]', '')}
    if len(dims) > 1 or len({bool(d) for _, d in choices}) != 1:
        raise RecordLayoutError('record has incompatible source array bounds')
    bounds = next(iter(dims)) if dims else next(iter(choices))[1]
    required, active = [], set()
    scalar_formats = {
        'char': ('b', 1), 'signed char': ('b', 1), 'unsigned char': ('B', 1),
        'short': ('h', 2), 'signed short': ('h', 2), 'unsigned short': ('H', 2),
        'short int': ('h', 2), 'unsigned short int': ('H', 2),
        'int': ('i', 4), 'signed int': ('i', 4), 'unsigned': ('I', 4),
        'unsigned int': ('I', 4), 'long long': ('q', 8),
        'signed long long': ('q', 8), 'unsigned long long': ('Q', 8),
        'float': ('f', 4), 'double': ('d', 8),
    }
    def append(declaration):
        if declaration not in required:
            required.append(declaration)
    def align(offset, alignment):
        return (offset + alignment - 1) // alignment * alignment
    def layout(type_name):
        type_name = re.sub(r'\b(?:const|volatile)\b', '', type_name).strip()
        type_name = ' '.join(type_name.split())
        if '*' in type_name:
            if type_name.count('*') != 1:
                raise RecordLayoutError('initialized pointer field requires a pointee hypothesis')
            target = type_name.replace('*', '').strip()
            if row['section'] not in ('.bss', '.sbss'):
                # Initialized: each word must name an existing C-declared
                # original object (checked per value in decode()).
                return dict(kind='pointer', size=4, alignment=4, target=target)
            # NOBITS storage needs only the pointer width; a pointer to a
            # tagged record is complete C without the pointee's definition.
            if re.fullmatch(r'(?:struct|union)\s+[A-Za-z_]\w*', target) and target not in records:
                return dict(kind='pointer', size=4, alignment=4)
            if target != 'void':
                # Retain a genuine source forward alias or the existing full
                # definition. Never introduce a tag for an anonymous type.
                if target in records:
                    layout(target)
                elif target in aliases and len(aliases[target]) == 1:
                    base_type = next(iter(aliases[target]))
                    if '_DATA_TYPE_NAME_' in base_type:
                        # pointer to a source array typedef: only its spelling is needed
                        append('typedef ' + base_type.replace('_DATA_TYPE_NAME_', ' ' + target) + ';')
                        return dict(kind='pointer', size=4, alignment=4)
                    if not re.fullmatch(r'(?:struct|union)\s+[A-Za-z_]\w*', base_type):
                        layout(base_type)
                    append(f'typedef {base_type} {target};')
                else:
                    layout(target)
            return dict(kind='pointer', size=4, alignment=4)
        if type_name in scalar_formats:
            fmt, width = scalar_formats[type_name]
            return dict(kind='scalar', fmt=fmt, size=width, alignment=width,
                        ctype=type_name)
        if type_name in active:
            raise RecordLayoutError('record has a recursive or divergent value type')
        active.add(type_name)
        try:
            if type_name in records:
                definitions = records[type_name]
                if len(definitions) != 1:
                    raise RecordLayoutError('record has divergent source definitions')
                definition = definitions[0]
                result = body_layout(definition['kind'], definition['body'])
                append(definition['declaration'])
                return result
            alternatives = aliases.get(type_name, set())
            if len(alternatives) != 1:
                raise RecordLayoutError('record field type has no unique explicit definition: ' + type_name + (' ' + repr(sorted(alternatives)) if alternatives else ''))
            target = next(iter(alternatives))
            array_alias = re.fullmatch(r'(.+?)\s*_DATA_TYPE_NAME_((?:\[\d+\])+)', target)
            if array_alias and '*' not in target and '__attribute__' not in target:
                # A source array typedef (`typedef T Name[N];`) as a field type.
                item = layout(array_alias[1])
                append(f'typedef {array_alias[1].strip()} {type_name}{array_alias[2]};')
                for bound in reversed(re.findall(r'\[(\d+)\]', array_alias[2])):
                    count = int(bound)
                    item = dict(kind='array', element=item, count=count,
                                size=item['size'] * count, alignment=item['alignment'])
                return item
            if '*' in target or '__attribute__' in target:
                raise RecordLayoutError('record field uses an unsupported pointer or ABI attribute')
            result = layout(target)
            # A genuine tagged definition is already carried in full by the
            # catalog; retain the source alias needed to refer to that tag.
            append(f'typedef {target} {type_name};')
            return result
        finally:
            active.remove(type_name)
    def field_layout(text):
        callback = re.fullmatch(r'\s*(.+?)\(\s*\*\s*([A-Za-z_]\w*)\s*\)\s*\((.*)\)\s*', text, re.S)
        if callback and row['section'] not in ('.bss', '.sbss') and ':' not in text and '__attribute__' not in text:
            # One function-pointer member; each word must name a TU function
            # of exactly this signature (checked in decode()).
            return [dict(name=callback[2], layout=dict(kind='function', size=4, alignment=4,
                                                         returns=callback[1], parameters=callback[3]))]
        if ':' in text or '__attribute__' in text or '(' in text:
            raise RecordLayoutError('record field needs a bitfield, function pointer, or attribute model')
        nested = re.match(r'(struct|union)\s*\{', text)
        if nested:
            depth, end = 1, nested.end()
            while end < len(text) and depth:
                depth += (text[end] == '{') - (text[end] == '}')
                end += 1
            if depth:
                raise RecordLayoutError('record field has an incomplete nested definition')
            value = body_layout(nested[1], text[nested.end():end-1])
            declarators = text[end:].strip()
        else:
            declarator_parts = _record_split(text, ',')
            match = re.fullmatch(r'(.+?)\s+(\**\s*[A-Za-z_]\w*\s*(?:\[[^\]]+\]\s*)*)', declarator_parts[0])
            if not match:
                raise RecordLayoutError('record field has an unsupported declarator')
            field_type = match[1]
            value = None
            declarators = ', '.join([match[2]] + declarator_parts[1:])
        fields = []
        if nested and not declarators:
            # GNU anonymous member: its storage belongs to the enclosing record.
            # Only NOBITS storage (no initializer to spell) uses it.
            if row['section'] not in ('.bss', '.sbss'):
                raise RecordLayoutError('anonymous record member needs an initializer model')
            return [dict(name='', layout=value)]
        for declarator in _record_split(declarators, ','):
            match = re.fullmatch(r'(\**\s*)([A-Za-z_]\w*)\s*((?:\[[^\]]+\]\s*)*)', declarator)
            if not match:
                raise RecordLayoutError('record field has an unsupported declarator')
            item = value if nested else layout(field_type + ' ' + match[1])
            if nested and match[1].strip():
                raise RecordLayoutError('nested record pointer declarator needs a tag model')
            for bound in reversed(re.findall(r'\[([^\]]+)\]', match[3])):
                count = _record_bound(bound)
                item = dict(kind='array', element=item, count=count,
                            size=item['size'] * count, alignment=item['alignment'])
            if item['size'] > 65536:
                raise RecordLayoutError('record field exceeds the bounded layout')
            fields.append(dict(name=match[2], layout=item))
        return fields
    def body_layout(kind, body):
        fields = [field for text in _record_split(body, ';') for field in field_layout(text)]
        if not fields:
            raise RecordLayoutError('empty record has no modeled layout')
        alignment = max(field['layout']['alignment'] for field in fields)
        offset = 0
        for field in fields:
            offset = align(offset, field['layout']['alignment']) if kind == 'struct' else 0
            field['offset'] = offset
            offset += field['layout']['size']
        size = align(offset if kind == 'struct' else max(f['layout']['size'] for f in fields), alignment)
        if size > 65536:
            raise RecordLayoutError('record exceeds the bounded layout')
        # Only an initializer is spelled through the first member; NOBITS
        # storage needs just the union's size.
        if kind == 'union' and fields[0]['layout']['size'] != size and row['section'] not in ('.bss', '.sbss'):
            raise RecordLayoutError('union first member does not cover its original storage')
        return dict(kind=kind, fields=fields, size=size, alignment=alignment)
    tokens = {}
    for line in block.get('lines', []) if isinstance(block, dict) else []:
        found = re.match(r'\s*/\*\s*[0-9A-Fa-f]+\s+([0-9A-Fa-f]{8})\s+[0-9A-Fa-f]+\s*\*/\s*\.word\s+(.+?)\s*$', line)
        if found:
            tokens[int(found[1], 16)] = found[2]
    pointees = []
    def pointer_value(item, payload, at):
        value = struct.unpack('<I', payload)[0]
        if value == 0:
            return '0'
        token = tokens.get(at, '')
        if not re.fullmatch(r'[A-Za-z_]\w*', token):
            raise RecordLayoutError('initialized pointer field word is not a named original object')
        targets = declarations.get(token, [])
        if not targets:
            # The pointee may be defined (not declared extern) by this TU.
            found = set()
            for code in getattr(records, 'source_texts', []):
                for kind, dims in re.findall(
                        r'^[ \t]*(?:static[ \t]+)?((?:const[ \t]+)?[A-Za-z_][\w \t]*?)[ \t]+' + re.escape(token) +
                        r'[ \t]*((?:\[\d+\])*)[ \t]*(?:__attribute__[ \t]*\(\([^;=]*\)\))?[ \t]*[;=]', code, re.M):
                    found.add((' '.join(kind.split()), dims))
            targets = [dict(type=t, dimensions=d) for t, d in found]
        # An unsized `extern T x[];` and the sized definition `T x[N]` are one object.
        sized = {d['type'] for d in targets if d['dimensions'] not in ('', '[]')}
        targets = [d for d in targets if not (d['dimensions'] == '[]' and d['type'] in sized)]
        if not targets or len({(d['type'], d['dimensions']) for d in targets}) != 1:
            raise RecordLayoutError('initialized pointer target lacks a unique C declaration')
        target = targets[0]
        if '(' in target['type'] or target.get('function_prototype'):
            raise RecordLayoutError('initialized pointer target is a function')
        want = re.sub(r'\b(?:const|volatile)\b', '', item['target']).split()
        have = re.sub(r'\b(?:const|volatile)\b', '', target['type']).split()
        if want != ['void'] and want != have:
            raise RecordLayoutError('initialized pointer target type differs from the field pointee')
        symbols = [s for s in original.symbols if s.name == token]
        if symbols and all(s.value != value for s in symbols):
            raise RecordLayoutError('initialized pointer word differs from its target address')
        pointees.append(f"extern {target['type']} {token}{target['dimensions']};")
        return token if target['dimensions'] else '&' + token
    def signature(returns, parameters):
        types = []
        for parameter in parameters.split(','):
            parameter = ' '.join(parameter.replace('*', ' * ').split())
            words = parameter.split()
            if len(words) > 1 and re.fullmatch(r'[A-Za-z_]\w*', words[-1]) and words[-1] not in (
                    'int', 'char', 'short', 'long', 'float', 'double', 'void', 'unsigned', 'signed'):
                words = words[:-1]
            types.append(' '.join(words))
        if types == ['void']:
            types = []
        return ' '.join(returns.replace('*', ' * ').split()), tuple(types)
    def function_value(item, payload, at):
        if struct.unpack('<I', payload)[0] == 0:
            return '0'
        token = tokens.get(at, '')
        if not re.fullmatch(r'[A-Za-z_]\w*', token):
            raise RecordLayoutError('function-pointer field word is not a named original function')
        want = signature(item['returns'], item['parameters'])
        found = set()
        for code in getattr(records, 'source_texts', []):
            for head, returns, parameters in re.findall(
                    r'^((?:static[ \t]+)?([A-Za-z_][\w \t*]*?)[ \t*]*\b' + re.escape(token) + r'[ \t]*\(([^()]*)\))[ \t]*\n?[ \t]*[{;]',
                    code, re.M):
                stars = head[:head.index(token)].count('*') - returns.count('*')
                found.add((' '.join(head.split()), signature(returns + ' *' * stars, parameters)))
        if len({sig for head, sig in found}) != 1 or next(iter(found))[1] != want:
            raise RecordLayoutError('function-pointer target lacks a unique source prototype of the field signature')
        pointees.append(next(iter(found))[0] + ';')
        return token
    def decode(item, payload, at=0):
        if len(payload) != item['size']:
            raise RecordLayoutError('record decoder extent mismatch')
        kind = item['kind']
        if kind == 'pointer':
            return pointer_value(item, payload, at)
        if kind == 'function':
            return function_value(item, payload, at)
        if kind == 'scalar':
            value = struct.unpack('<' + item['fmt'], payload)[0]
            return numeric_literal(value, item['ctype'], item['fmt'])
        if kind == 'array':
            stride = item['element']['size']
            if (item['element']['kind'] == 'scalar' and item['element']['ctype'] == 'char' and
                    payload.endswith(b'\0') and encoded_text(payload)):
                escapes = {9: '\\t', 10: '\\n', 13: '\\r', 34: '\\"', 63: '\\?', 92: '\\\\'}
                return '"' + ''.join(escapes.get(v, chr(v) if 32 <= v < 127 else f'\\{v:03o}')
                                     for v in payload.rstrip(b'\0')) + '"'
            return '{ ' + ', '.join(decode(item['element'], payload[i:i+stride], at + i)
                                    for i in range(0, len(payload), stride)) + ' }'
        fields = item['fields'][:1] if kind == 'union' else item['fields']
        entries, cursor = [], 0
        for field in fields:
            offset, width = field['offset'], field['layout']['size']
            if any(payload[cursor:offset]):
                raise RecordLayoutError('original record contains nonzero implicit ABI padding')
            entries.append('.' + field['name'] + ' = ' + decode(field['layout'], payload[offset:offset+width], at + offset))
            cursor = offset + width
        if any(payload[cursor:]):
            raise RecordLayoutError('original record has unexplained trailing ABI padding')
        return '{ ' + ', '.join(entries) + ' }'
    item = layout(base)
    extent = dc.hx(row['end']) - dc.hx(row['address'])
    if not 0 < extent <= 65536:
        raise RecordLayoutError('original record exceeds the bounded layout')
    if bounds:
        if extent % item['size']:
            raise RecordLayoutError('original extent does not contain complete source records')
        dimensions = re.findall(r'\[([^\]]*)\]', bounds)
        if not dimensions:
            raise RecordLayoutError('record array lacks explicit source dimensions')
        if dimensions == ['']:
            dimensions = [str(extent // item['size'])]
        for bound in reversed(dimensions):
            count = _record_bound(bound)
            item = dict(kind='array', element=item, count=count,
                        size=item['size'] * count, alignment=item['alignment'])
        suffix = ''.join('[' + str(_record_bound(b)) + ']' for b in dimensions)
    else:
        suffix = ''
    mapped_end = row['end']
    alignment_tail = 0
    if item['size'] != extent:
        # A zero-size NOTYPE label is not a sizeof declaration. Its scaffold
        # item may extend through alignment before the next object. Preserve
        # only an independently evidenced complete C record; the remaining
        # original bytes stay uncredited scaffolding, not invented fields.
        address = dc.hx(row['address'])
        size = item['size']
        original_objects = [s for s in original.symbols if s.value == address and s.type == 1 and s.size]
        positive_size_proof = original_objects and all(s.size == size for s in original_objects)
        if original_objects and not positive_size_proof:
            raise RecordLayoutError('record source size differs from a sized original OBJECT')
        named_fields = [f['name'] for f in item.get('fields', [])]
        use_proof = False
        for code in getattr(records, 'source_texts', []):
            if any(re.search(r'\b' + re.escape(name) + r'\s*\.\s*' + re.escape(field) + r'\b', code)
                   for field in named_fields):
                use_proof = True
            for local in re.findall(r'\b' + re.escape(base) + r'\s*\*\s*(\w+)\s*=\s*&\s*' + re.escape(name) + r'\b', code):
                use_proof |= any(re.search(r'\b' + re.escape(local) + r'\s*->\s*' + re.escape(field) + r'\b', code)
                                 for field in named_fields)
            for local in re.findall(r'\b' + re.escape(base) + r'\s+(\w+)\s*;', code):
                use_proof |= bool(re.search(r'\b' + re.escape(local) + r'\s*=\s*' + re.escape(name) + r'\s*;', code) and
                                  any(re.search(r'\b' + re.escape(local) + r'\s*\.\s*' + re.escape(field) + r'\b', code)
                                      for field in named_fields))
        aligned_end = any((address + size + boundary - 1) // boundary * boundary == address + extent
                          for boundary in (2, 4, 8, 16))
        if not (0 < extent - size <= 15 and aligned_end and (positive_size_proof or use_proof)):
            raise RecordLayoutError('record source type does not cover the exact original extent')
        if row['section'] not in ('.bss', '.sbss'):
            tail = dc.va_bytes(original, address + size, extent - size)
            if tail is None or any(tail):
                raise RecordLayoutError('record alignment tail contains nonzero original bytes')
        alignment_tail, extent = extent - size, size
    provenance = dict(evidence[0], data_role='existing_typed_record',
                      record_size=extent, probe_typedefs=required,
                      record_sources=sorted({d['path'] for definitions in records.values() for d in definitions
                                             if d['declaration'] in required}),
                      basis='existing named C fields and exact original DATA extent; numeric fields decoded from original bytes')
    if evidence[0].get('source_replacements'):
        provenance['declaration_correction_basis'] = evidence[0].get('basis')
    if alignment_tail:
        provenance.update(object_extent=extent, mapped_end=mapped_end,
                          uncredited_alignment_extent=alignment_tail,
                          basis='existing typed record field consumers establish sizeof; original trailing zero alignment remains scaffolding')
    prefix = storage + ctype + ' ' + name + suffix
    if row['section'] in ('.bss', '.sbss'):
        return prefix + ';', 'storage', provenance
    payload = dc.va_bytes(original, dc.hx(row['address']), extent)
    if payload is None:
        raise RecordLayoutError('record is outside original initialized DATA')
    value = decode(item, payload, dc.hx(row['address']))
    if pointees:
        provenance = dict(provenance, pointer_declarations=list(dict.fromkeys(pointees)))
    return prefix + ' = ' + value + ';', 'record_array', provenance
