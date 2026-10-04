"""Evidenced DATA type propagation and byte-oriented format initializers.

These helpers interpret source declarations and original DATA identities. They
do not build objects, change compile contracts, or provide recovery credit.
"""
import collections
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
    return roots


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
