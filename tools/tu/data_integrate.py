#!/usr/bin/env python3
"""Publish typed DATA under the integration lease, using the native build/gates.

This module is the canonical-checkout integration half of data_recover.py. It
retains the pre-publication sources, compiler objects and registries, appends
definitions, and records raw compiler section offsets in the existing carve
registry. It never modifies a compiler object or the original scaffold.
"""
import copy
import importlib.util
import json
import os
from pathlib import Path
import re
import signal
import sqlite3
import subprocess
import sys
import time

import data_carve as dc
from elfinfo import Elf
from toolchain import Toolchain
from data_recover import grouped, read_json, require, sha, SCALARS, dimensions, numeric_literal, original_local
from data_gate import emitted_storage
import struct


def write(path, contents, lease):
    lease.check([path])
    Path(path).parent.mkdir(parents=True, exist_ok=True)
    Path(path).write_bytes(contents if isinstance(contents, bytes) else contents.encode())


def command(argv, cwd, log, lease, timeout=600):
    lease.check([log])
    Path(log).parent.mkdir(parents=True, exist_ok=True)
    with Path(log).open('w') as stream:
        process = subprocess.Popen(list(map(str, argv)), cwd=cwd, stdout=stream,
                                   stderr=subprocess.STDOUT, start_new_session=True)
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
    require(code == 0, f'command failed ({code}); see {log}')


def integration_lease(lease):
    lease.check([])
    with sqlite3.connect(f'file:{lease.root / ".work/queue.sqlite3"}?mode=ro', uri=True) as conn:
        held = {r[0] for r in conn.execute('SELECT resource FROM locks WHERE task_id=?', (lease.task,))}
    require('baseline:integration' in held, 'direct publication requires baseline:integration')


def native_paths(root, facts):
    unit, name = facts['unit'], facts['name']
    folder = root / 'build' / unit
    obj = folder / ('build/c/' + name + '.o' if unit == 'main' else
                    'build/scaffold/src/' + unit + '/' + name + '.o')
    asm = Path(str(obj) + '.s') if unit == 'main' else obj.with_suffix('.s')
    preprocessed = Path(str(obj) + '.i') if unit == 'main' else obj.with_suffix('.i')
    return obj, asm, preprocessed


def effective_plan(plan, root):
    """Separate original positive-size objects from following scaffold padding."""
    result = copy.deepcopy(plan)
    tc = Toolchain(root)
    originals = read_json(root / 'config/originals.json')['units']
    binaries = {}
    corrections = []
    for row in result['locations']:
        if row['status'] != 'generated_candidate':
            continue
        unit, name = row['unit'], row['names'][0]
        if unit not in binaries:
            binaries[unit] = Elf((tc.game_dir() / originals[unit]['file']).read_bytes())
        elf = binaries[unit]
        start, end = dc.hx(row['address']), dc.hx(row['end'])
        matches = [s for s in elf.symbols if s.name == name and s.value == start and
                   s.type == 1 and s.size > 0 and 0 < s.shndx < len(elf.sections)]
        require(len(matches) <= 1, 'ambiguous original DATA object')
        if not matches or matches[0].size == end - start:
            continue
        size = matches[0].size
        require(row['kind'] == 'numeric' and 0 < size < end - start and
                not any(dc.va_bytes(elf, start + size, end - start - size)),
                f'{name}: original OBJECT size needs a manual layout')
        ctype = row['type_evidence']['type']
        scalar = SCALARS[re.sub(r'\b(?:const|volatile)\s*', '', ctype).strip()]
        suffix, count = dimensions(row['type_evidence'], size, scalar[1])
        payload = dc.va_bytes(elf, start, size)
        entries = [numeric_literal(v, ctype, scalar[0]) for v in
                   struct.unpack('<' + scalar[0] * count, payload)]
        storage = 'static ' if matches[0].bind == 0 else ''
        row['definition'] = f'{storage}{ctype} {name}{suffix} = {{ ' + ', '.join(entries) + ' };'
        row['object_size'] = size
        row['zero_padding_tail'] = end - start - size
        corrections.append(dict(tu=row['tu'], name=name, object_size=size,
                                scaffold_extent=end - start))
    result['integration_object_size_corrections'] = corrections
    return result


def old_mapping(root, tu, facts, obj, registry):
    """Freeze ordinary raw offsets before appending definitions to this TU."""
    elf = Elf(obj.read_bytes())
    unit_dir = root / 'build' / facts['unit']
    ctx = dc.tu_context(root, facts['unit'], unit_dir, tu)
    orig = Elf(Path(ctx['orig']).read_bytes())
    initialized = copy.deepcopy(registry)
    initialized['tus'][tu] = {s:r for s,r in registry['tus'].get(tu, {}).items()
                              if s not in ('.bss', '.sbss')}
    initialized.pop('common_tail', None)
    mappings = dc.split_plans(root, facts['unit'], unit_dir, tu, elf, registry=initialized)
    result = copy.deepcopy(registry['tus'].get(tu, {}))
    for section, runs in result.items():
        if section in ('.bss', '.sbss') or all(r.get('c_input_spans') for r in runs):
            continue
        csec = next((s for s in elf.sections if s.name == dc.raw_section_name(facts['unit'], section) and s.size), None)
        require(csec is not None, f'{tu}: old {section} input missing')
        spans = [tuple(map(dc.hx, r['range'])) for r in runs]
        specs = dc.run_specs(dc.section_items(ctx, section), spans, runs)
        mapping = mappings.get(csec.name) or dc.plan_split(
            elf, csec, specs, lambda va, length: dc.va_bytes(orig, va, length))
        for run, piece in zip(runs, mapping['pieces']):
            run['integration_old_input'] = dict(section=csec.name, offset=piece['offset'],
                                                length=piece['length'])
    return result


def snapshot(plan, candidates, output, lease):
    root = lease.root
    # Tool implementation can evolve between the private probe and integration;
    # every data/build/source input must still be exactly the pinned input.
    for path, digest in plan['inputs'].items():
        if Path(path).resolve() == root / 'tools/tu/data_recover.py':
            continue
        require(Path(path).is_file() and sha(path) == digest, f'stale candidate input: {path}')
    prepared = read_json(candidates / 'prepared.json')
    require(read_json(candidates / 'compiler.json')['problem_count'] == 0, 'compiler candidates have unresolved problems')
    by_tu = {r['tu']: r for r in prepared['candidates']}
    registry = dc.load_registry(root)
    bindings = {}
    for tu, rows in grouped(plan).items():
        facts = plan['tus'][tu]
        source = root / facts['path']
        candidate = by_tu[tu]
        require(sha(source) == candidate['baseline_sha256'], f'{tu}: source changed after the probe')
        folder = Path(candidate['folder'])
        require(sha(folder / 'additions.c') == candidate['additions_sha256'] and
                sha(folder / 'probe.c') == candidate['probe_sha256'], 'immutable probe source changed')
        obj, asm, preprocessed = native_paths(root, facts)
        stem = output / 'baseline' / tu.replace('/', '_')
        for suffix, path in (('.c', source), ('.o', obj), ('.s', asm), ('.i', preprocessed)):
            require(path.is_file(), f'missing baseline native input {path}')
            write(stem.with_suffix(suffix), path.read_bytes(), lease)
        bindings[tu] = old_mapping(root, tu, facts, obj, registry)
    for short, path in (('public', root / 'config/objects/data-carves.json'),
                        ('private', root / 'config/tu/data-carves.json'),
                        ('script', root / 'tools/tu/data_recover.py')):
        suffix = '.py' if short == 'script' else '.json'
        write(output / 'baseline' / (short + suffix), path.read_bytes(), lease)
    lease.json(output / 'baseline/bindings.json', bindings)
    support = {}
    for rows in grouped(plan).values():
        for row in rows:
            for edit in row['type_evidence'].get('support_edits', []):
                path = root/edit['path']
                lease.check([path])
                support[edit['path']] = dict(text=path.read_text(),sha256=sha(path))
    if support:
        lease.json(output / 'baseline/support-headers.json', support)
    lease.json(output / 'plan.json', effective_plan(plan, root))


def publish(plan, output, lease):
    bindings=read_json(output/'baseline/bindings.json')
    support_path = output/'baseline/support-headers.json'
    if support_path.is_file():
        support = read_json(support_path)
        edits = {edit['path']: edit for rows in grouped(plan).values() for row in rows
                 for edit in row['type_evidence'].get('support_edits', [])}
        for path, original in support.items():
            edit = edits[path]
            require(original['text'].count(edit['old']) == 1, 'ambiguous partial record expansion')
            updated = original['text'].replace(edit['old'],edit['new'])
            require((lease.root/path).read_text() in (original['text'],updated), 'support header changed since snapshot')
            write(lease.root/path,updated,lease)
    for tu, rows in grouped(plan).items():
        path = lease.root / plan['tus'][tu]['path']
        text = (output/'baseline'/(tu.replace('/', '_')+'.c')).read_text()
        facts=plan['tus'][tu]
        old=Elf((output/'baseline'/(tu.replace('/', '_')+'.o')).read_bytes())
        ctx=dc.tu_context(lease.root,facts['unit'],lease.root/'build'/facts['unit'],tu)
        original=Elf(Path(ctx['orig']).read_bytes())
        definitions={r['names'][0]:r['definition'] for r in rows}
        for row in rows:
            name=row['names'][0]
            if original_local(original,name,dc.hx(row['address'])) and not definitions[name].startswith('static '):
                definitions[name]='static '+definitions[name]
        replacements = {r['old']: r['new'] for row in rows
                        for r in row['type_evidence'].get('source_replacements', [])}
        for before, after in replacements.items():
            require(text.count(before) == 1, f'{tu}: source type correction no longer has one pinned occurrence')
            text = text.replace(before, after)
        # Convert grouped extern declarations into individual declarations
        # before correcting local bindings. Replacing an entire group for its
        # last member would otherwise silently drop the other declarations.
        def split_extern(match):
            body = match[1]
            if ',' not in body:
                return match[0]
            parts = body.split(',')
            first = re.fullmatch(r'\s*(.+?)\s+(\**\s*[A-Za-z_]\w*\s*(?:\[[^\]]*\]\s*)*)\s*', parts[0])
            if not first:
                return match[0]
            ctype = first[1].strip()
            parts[0] = first[2]
            return '\n'.join('extern '+ctype+' '+p.strip()+';' for p in parts)
        text = re.sub(r'\bextern\s+([^;{}()]+);', split_extern, text)
        for section,runs in bindings[tu].items():
            for run in runs:
                raw=run.get('integration_old_input')
                if not raw:
                    continue
                csec=next(s for s in old.sections if s.name==raw['section'])
                items=dc.section_items(ctx,section)
                for sym in old.symbols:
                    if sym.shndx==csec.index and sym.type==1 and sym.size>0 and (
                            raw['offset']<=sym.value<raw['offset']+raw['length']):
                        va=dc.hx(run['range'][0])+sym.value-raw['offset']
                        item=next((it for it in items if it['start']==va),None)
                        meaningful=[s for s in original.symbols if s.value==va and s.type==1 and s.size>0 and s.shndx!=0]
                        if item and not meaningful and sym.value+sym.size==raw['offset']+raw['length'] and (
                                item['end']>va+sym.size and item['end']<=dc.hx(run['range'][1])):
                            payload=dc.va_bytes(original,va,item['end']-va)
                            pattern=r'^(const\s+(?:unsigned\s+|signed\s+)?char\s+'+re.escape(sym.name)+r')\s*\[[^\]\n]*\]\s*='
                            if re.search(pattern,text,re.M):
                                require(payload is not None and not any(payload[sym.size:]) and
                                        old.section_bytes(csec)[sym.value:sym.value+sym.size]==payload[:sym.size],
                                        f'{tu}: original character-buffer zero tail is not exact')
                                text,count=re.subn(pattern,lambda m:m[1]+f'[{item["end"]-va}] =',text,flags=re.M)
                                require(count==1,f'{tu}: ambiguous character-buffer definition')
                    if sym.shndx!=csec.index or sym.type!=1 or sym.bind!=1 or not (
                            raw['offset']<=sym.value<raw['offset']+raw['length']):
                        continue
                    va=dc.hx(run['range'][0])+sym.value-raw['offset']
                    identities=[s for s in original.symbols if s.value==va and s.type==1 and s.size>0 and s.shndx!=0]
                    if len(identities)!=1 or identities[0].size!=sym.size or identities[0].bind!=0:
                        continue
                    require(re.fullmatch(r'[A-Za-z_]\w*',sym.name),f'{tu}: non-C binding correction')
                    pattern=r'^((?!(?:static|extern)\b)[A-Za-z_][^;={}\n]*?\b'+re.escape(sym.name)+r'\s*(?:\[[^\]\n]*\]\s*)*)\s*='
                    text,count=re.subn(pattern,lambda m:'static '+m[1]+' =',text,flags=re.M)
                    require(count==1,f'{tu}/{sym.name}: expected one original local DATA definition')
        for row in rows:
            if definitions[row['names'][0]].startswith('static '):
                name = row['names'][0]
                declaration = definitions[name].split('=', 1)[0].strip() + ';'
                pattern = r'\b(?:static\s+)?extern\s+[^;{}\n]*\b' + re.escape(name) + r'\s*(?:\[[^\]]*\]\s*)*;'
                text = re.sub(pattern, lambda m: declaration, text)
        declarations = list(dict.fromkeys(d for r in rows
                                         for d in r['type_evidence'].get('pointer_declarations', [])))
        local = {name for name,definition in definitions.items() if definition.startswith('static ')}
        declarations = [re.sub(r'^extern\b', 'static', d) if
                        any(re.search(r'\b'+re.escape(n)+r'\s*\[', d) for n in local) else d
                        for d in declarations]
        prelude = list(dict.fromkeys(p for r in rows for p in r['type_evidence'].get('source_prelude', [])))
        additions = '\n'.join(prelude + declarations) + '\n\n' + '\n\n'.join(
            definitions[r['names'][0]] for r in sorted(rows, key=lambda r: dc.hx(r['address'])))
        write(path, text.rstrip() + '\n\n' + additions + '\n', lease)


def compile_sources(root, plan, folder, lease):
    jobs = read_json(root / 'config/scaling-policy.json')['limits']['ninja_jobs']
    for unit in sorted({f['unit'] for tu, f in plan['tus'].items() if tu in grouped(plan)}):
        targets = [str(native_paths(root, plan['tus'][tu])[0].relative_to(root / 'build' / unit))
                   for tu in grouped(plan) if plan['tus'][tu]['unit'] == unit]
        command(['ninja', '-j', jobs, *targets], root / 'build' / unit,
                folder / (unit + '.build.log'), lease)
        print(json.dumps(dict(stage='native_compile', unit=unit, tus=len(targets))), flush=True)


def carved_run(section, lo, hi, raw_section, off, end, names, anonymous=False, padding=0):
    part = dict(section=raw_section, object_range=[off, end], range=[dc.h8(lo), dc.h8(hi)],
                symbols=names, credit='verified')
    if anonymous:
        part['anonymous_emission'] = True
    if padding:
        part['zero_padding_tail'] = padding
    return dict(range=part['range'], generated=part['range'], generated_by=[],
                c_owned_symbols=names, also_referenced_by_c=[], c_input_spans=[part])


def previous_input_proof(tu, rows, bindings, elf, old, ctx):
    """Check changed pointer addends against retail, without ignoring bytes.

    Defining a previously external pointer target changes the relocatable
    word from zero to a C-section offset. Both symbolic expressions must
    resolve to the exact original word at the table's original address.
    """
    mappings = []
    for section, runs in bindings.items():
        if section in ('.bss','.sbss'):
            continue
        for run in runs:
            if run.get('c_input_spans'):
                for p in run['c_input_spans']:
                    mappings.append((p['section'],int(p['object_range'][0]),
                                     int(p['object_range'][1]),dc.hx(p['range'][0])))
            else:
                p=run['integration_old_input']
                mappings.append((p['section'],p['offset'],p['offset']+p['length'],dc.hx(run['range'][0])))
    original_names={n:dc.hx(r['address']) for r in rows for n in r['names']}
    for row in rows:
        symbols=[s for s in elf.symbols if s.name==row['names'][0] and
                 0<s.shndx<len(elf.sections) and (s.type==1 or
                 (row['kind']=='storage' and s.type==0 and s.bind==0 and s.size==0 and
                  elf.sections[s.shndx].type==8))]
        require(len(symbols)==1,f'{tu}: missing new pointer target owner')
        s=symbols[0]
        size=s.size or dc.hx(row['end'])-dc.hx(row['address'])
        require(s.value+size<=elf.sections[s.shndx].size, f'{tu}: new target exceeds compiler section')
        mappings.append((elf.sections[s.shndx].name,s.value,s.value+size,dc.hx(row['address'])))
    original=Elf(Path(ctx['orig']).read_bytes())
    checked=0
    for before_section in old.sections:
        if before_section.name not in ('.data','.rodata','.sdata','.lit4') or not before_section.size:
            continue
        after_section=next(s for s in elf.sections if s.name==before_section.name)
        before=old.section_bytes(before_section)
        after=elf.section_bytes(after_section)[:len(before)]
        require(len(before)==len(after),f'{tu}: prior raw section shrank')
        old_reloc={off:(typ,old.symbols[si]) for off,typ,si in dc.read_relocations(old).get(before_section.index,[])}
        new_reloc={off:(typ,elf.symbols[si]) for off,typ,si in dc.read_relocations(elf).get(after_section.index,[])}
        changes={i//4*4 for i,(a,b) in enumerate(zip(before,after)) if a!=b}
        for off in changes:
            require(off+4<=len(before) and off in old_reloc and off in new_reloc,
                    f'{tu}: non-relocated prior compiler bytes changed at {before_section.name}+{off:#x}')
            old_type,old_sym=old_reloc[off];new_type,new_sym=new_reloc[off]
            require(old_type==new_type==2 and old_sym.shndx==0 and old_sym.name in original_names,
                    f'{tu}: changed prior bytes are not a newly defined R_MIPS_32 pointer')
            old_value=original_names[old_sym.name]+struct.unpack_from('<I',before,off)[0]
            require(0<new_sym.shndx<len(elf.sections),f'{tu}: new pointer is not section-defined')
            target=new_sym.value+struct.unpack_from('<I',after,off)[0]
            target_section=elf.sections[new_sym.shndx].name
            targets=[p for p in mappings if p[0]==target_section and p[1]<=target<p[2]]
            source=[p for p in mappings if p[0]==before_section.name and p[1]<=off< p[2]]
            require(len(targets)==len(source)==1,f'{tu}: pointer lacks one exact input/retail mapping')
            new_value=targets[0][3]+target-targets[0][1]
            source_va=source[0][3]+off-source[0][1]
            expected=dc.va_bytes(original,source_va,4)
            require(expected is not None and old_value==new_value==struct.unpack('<I',expected)[0],
                    f'{tu}: changed pointer does not equal its original retail value at {source_va:#x}')
            checked+=1
    return checked


def allocations(plan, output, folder, lease):
    root = lease.root
    bindings = read_json(output / 'baseline/bindings.json')
    registries = {kind: read_json(output / 'baseline' / (kind + '.json')) for kind in ('public', 'private')}
    allocation_tus = {}
    for tu, rows in grouped(plan).items():
        facts = plan['tus'][tu]
        obj, asm, _ = native_paths(root, facts)
        elf = Elf(obj.read_bytes())
        old = Elf((output / 'baseline' / (tu.replace('/', '_') + '.o')).read_bytes())
        ctx = dc.tu_context(root, facts['unit'], root / 'build' / facts['unit'], tu)
        previous_input_proof(tu,rows,bindings[tu],elf,old,ctx)
        sections = copy.deepcopy(bindings[tu])
        # Explicit mappings keep prior definitions and compiler pools intact.
        # Convert heuristic runs at original item boundaries, leaving native
        # zero alignment gaps to the existing splitter and scaffold allocator.
        for section, runs in list(sections.items()):
            if section in ('.bss', '.sbss'):
                continue
            converted = []
            items = dc.section_items(ctx, section)
            for run in runs:
                if run.get('c_input_spans'):
                    for part in run['c_input_spans']:
                        if part.get('original_fields'):
                            fields=part['original_fields']
                            baseline=output/'baseline'/(tu.replace('/','_')+'.c')
                            current=root/facts['path']
                            require(fields['source_sha256']==sha(baseline) and
                                    current.read_bytes().startswith(baseline.read_bytes().rstrip()+b'\n\n'),
                                    f'{tu}: prior structure source changed; cannot refresh its current input pin')
                            fresh=copy.deepcopy(fields)
                            fresh['source_sha256']=sha(current)
                            original=Elf(Path(ctx['orig']).read_bytes())
                            owners=[s for s in elf.symbols if s.name in part['symbols'] and s.type==1]
                            require(len(owners)==1 and dc.original_character_fields(
                                root,facts['unit'],ctx,original,owners[0],dc.hx(part['range'][0]),fresh),
                                f'{tu}: current struct fields do not prove original OBJECT identities')
                            sym=owners[0]
                            require(elf.section_bytes(elf.sections[sym.shndx])[sym.value:sym.value+sym.size]
                                    ==dc.va_bytes(original,dc.hx(part['range'][0]),sym.size),
                                    f'{tu}: current structure bytes differ from original')
                            fields['source_sha256']=fresh['source_sha256']
                    converted.append(run)
                    continue
                raw = run.pop('integration_old_input')
                csec = next(s for s in elf.sections if s.name == raw['section'])
                oldsec = next(s for s in old.sections if s.name == raw['section'])
                base = dc.hx(run['range'][0])
                stop = base + raw['length']
                for item in items:
                    if not base <= item['start'] < stop:
                        continue
                    lo, hi = item['start'], min(item['end'], stop)
                    off = raw['offset'] + lo - base
                    symbols = [s for s in elf.symbols if s.shndx == csec.index and s.type == 1 and s.value == off]
                    if symbols:
                        require(len(symbols) == 1, f'{tu}: ambiguous prior raw owner')
                        sym = symbols[0]
                        extent = hi - lo
                        if sym.size>extent and sym.size==item['end']-lo:
                            # A character buffer that ended before its original
                            # zero tail now explicitly covers that mapped item.
                            hi=item['end']
                            extent=hi-lo
                        require(sym.size <= extent, f'{tu}: old object exceeds original item')
                        tail = extent - sym.size
                        if hi < item['end'] or (tail and (any(elf.section_bytes(csec)[off+sym.size:off+extent]) or
                                any(s.value == off+sym.size for s in elf.symbols if s.shndx == csec.index and s.type == 1))):
                            extent, tail = sym.size, 0
                        converted.append(carved_run(section, lo, lo+extent, csec.name, off,
                                                    off+extent, [sym.name], padding=tail))
                    else:
                        part = carved_run(section, lo, hi, csec.name, off, off+hi-lo, [], anonymous=True)
                        part['c_owned_symbols'] = [n for n in run['c_owned_symbols'] if n in item['labels']]
                        require(part['c_owned_symbols'], f'{tu}: anonymous old input has no original identity')
                        converted.append(part)
            sections[section] = converted
        storage = emitted_storage(asm.read_text())
        names = {s.name: s for s in elf.symbols if s.name and
                 (s.type == 1 or (s.type == 0 and s.bind == 0 and s.size == 0 and
                  storage.get(s.name,{}).get('extent_kind')=='label-space')) and
                 0 < s.shndx < len(elf.sections)}
        for row in rows:
            name = row['names'][0]
            require(name in names, f'{tu}: full TU compiler did not emit {name}')
            sym = names[name]
            section = elf.sections[sym.shndx]
            lo, hi = dc.hx(row['address']), dc.hx(row['end'])
            size=sym.size
            if row['kind']=='storage' and sym.type==0:
                size=storage[name]['size']
                original=Elf(Path(ctx['orig']).read_bytes())
                identities=[s for s in original.symbols if s.name==name and s.shndx!=0]
                require(len(identities)==1 and identities[0].value==lo and
                        (identities[0].bind,identities[0].type,identities[0].size)==(0,0,0) and
                        storage[name]['section']==section.name,
                        f'{tu}/{name}: LOCAL NOTYPE storage does not match original and compiler directives')
            require(size == row.get('object_size', hi-lo), f'{tu}/{name}: compiler extent differs')
            pad = row.get('zero_padding_tail', 0)
            require(sym.value+size+pad <= section.size, f'{tu}/{name}: native alignment is unavailable')
            run = carved_run(row['section'], lo, hi, section.name, sym.value,
                             sym.value+size+pad, [name], padding=pad)
            run['basis'] = dict(kind='automatic-typed-data', source_sha256=sha(root/facts['path']),
                                object_sha256=sha(obj), assembly_sha256=sha(asm))
            sections.setdefault(row['section'], []).append(run)
        for section, runs in sections.items():
            runs.sort(key=lambda r: dc.hx(r['range'][0]))
            for a, b in zip(runs, runs[1:]):
                require(dc.hx(a['range'][1]) <= dc.hx(b['range'][0]), f'{tu}: overlapping DATA allocation')
        allocation_tus[tu] = sections
        for registry in registries.values():
            registry['tus'][tu] = sections
        # The native splitter validates identities, bytes, relocations and gaps
        # before either public registry is replaced.
        initialized = copy.deepcopy(registries['private'])
        initialized['tus'][tu] = {s:r for s,r in sections.items() if s not in ('.bss','.sbss')}
        initialized.pop('common_tail', None)
        dc.split_plans(root, facts['unit'], root/'build'/facts['unit'], tu, elf,
                       registry=initialized)
        print(json.dumps(dict(stage='allocation', tu=tu, locations=len(rows))), flush=True)
    lease.json(folder / 'registry.json', allocation_tus)
    for kind, path in (('public', root/'config/objects/data-carves.json'),
                       ('private', root/'config/tu/data-carves.json')):
        lease.json(path, registries[kind])


def harvest(root, folder, lease):
    spec = importlib.util.spec_from_file_location('automatic_header_harvest', root/'tools/header_harvest.py')
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    outputs = module.build_outputs(str(root)).outputs
    removed = module.existing_generated(str(root))-set(outputs)
    destinations = [root/relative for relative,text in outputs.items()
                    if not (root/relative).is_file() or (root/relative).read_text()!=text]
    lease.reserve_generated_headers(destinations+[root/relative for relative in removed])
    changed = []
    for relative, text in outputs.items():
        path = root / relative
        if not path.is_file() or path.read_text() != text:
            write(path, text, lease)
            changed.append(relative)
    for relative in removed:
        path = root/relative
        lease.check([path])
        path.unlink()
    lease.json(folder/'headers.json', dict(changed=changed))
    command([sys.executable, '-B', root/'tools/header_harvest.py', '--check'], root,
            folder/'harvest.log', lease, timeout=120)
    # The native MAIN preprocessor reads its staged header layer. Stage only
    # the explicit include tree; never enumerate generated attempt trees.
    for unit in ('main','ov01','ov02','ov10','ov11','ov12'):
        target = root/'build'/unit/'include'
        if target.resolve() == (root/'include').resolve():
            continue
        for path in (root/'include').rglob('*.h'):
            dest = target/path.relative_to(root/'include')
            if not dest.is_file() or dest.read_bytes() != path.read_bytes():
                dest.parent.mkdir(parents=True, exist_ok=True)
                dest.write_bytes(path.read_bytes())


def refresh_storage_inputs(plan, output, lease):
    """Refresh current allocator inputs only after proving storage is unchanged.

    Original owner extents/addresses and historical allocation evidence remain
    intact. The allocator will verify the newly pinned native inputs again.
    """
    root=lease.root
    path=root/'config/objects/data-carves.json'
    registry=read_json(path)
    baseline=read_json(output/'baseline/public.json')
    groups=grouped(plan)
    for unit,record in registry.get('overlay_bss_records',{}).items():
        prior=baseline['overlay_bss_records'][unit]
        for index,row in enumerate(record.get('recovered_location_candidates',[])):
            tu=row.get('evidence',{}).get('original_tu','').split('_')[0]
            if tu not in groups:
                continue
            facts=plan['tus'][tu];obj,asm,_=native_paths(root,facts)
            stem=output/'baseline'/tu.replace('/','_')
            old=Elf(stem.with_suffix('.o').read_bytes());new=Elf(obj.read_bytes())
            def storage(elf):
                return sorted((s.name,s.value,s.size,s.type,s.bind,
                              elf.sections[s.shndx].name if s.shndx<len(elf.sections) else s.shndx)
                              for s in elf.symbols if s.name and (s.shndx in (0xfff2,0xff03) or
                              (0<s.shndx<len(elf.sections) and elf.sections[s.shndx].type==8)))
            require(storage(old)==storage(new),f'{tu}: compiler storage identities changed')
            owner=row['storage_owner']
            previous=prior['recovered_location_candidates'][index]
            require(owner==previous['storage_owner'],f'{tu}: original storage owner changed')
            expected=dict(source_sha256=sha(stem.with_suffix('.c')),
                          object_sha256=sha(stem.with_suffix('.o')),
                          raw_input_object_sha256=sha(stem.with_suffix('.o')),
                          assembly_sha256=sha(stem.with_suffix('.s')))
            current=dict(source_sha256=sha(root/facts['path']),object_sha256=sha(obj),
                         raw_input_object_sha256=sha(obj),assembly_sha256=sha(asm))
            for key in ('evidence','compiler_storage'):
                before=previous.get(key,{})
                fields=row.get(key,{})
                for pin,digest in current.items():
                    if pin in before:
                        require(before[pin]==expected[pin],f'{tu}: baseline {pin} differs from prior allocator evidence')
                        fields[pin]=digest
    lease.json(path,registry)


def rebuild(root, folder, lease):
    tc = Toolchain(root)
    os.environ['XENO_TOOLCHAIN_DIR'] = str(tc.toolchain_root)
    os.environ['XENO_GAME_DIR'] = str(tc.game_dir())
    os.environ['XENO_SPLAT_PYTHON'] = str(tc.splat_python())
    jobs = read_json(root/'config/scaling-policy.json')['limits']['ninja_jobs']
    for unit in ('main','ov01','ov02','ov10','ov11','ov12'):
        unit_dir = root/'build'/unit
        if unit == 'main':
            argv = [sys.executable,'-B',root/'tools/tu/ninja_main.py','--root',root,'--unit-dir',unit_dir]
        else:
            argv = [sys.executable,'-B',root/'tools/tu/ninja_ovl.py',unit_dir,unit,'--root',root,
                    '--manifest',unit_dir/'compile-manifest.json',
                    '--bss-record',root/'config/objects/data-carves.json']
        command(argv, root, folder/'configure.log', lease, timeout=120)
        command(['ninja','-j',jobs,'build/'+unit+'.bin'], unit_dir,
                folder/(unit+'.build.log'), lease, timeout=1200)
        print(json.dumps(dict(stage='linked_build', unit=unit)), flush=True)


def gates(plan, folder, lease):
    root = lease.root
    rows = [r for r in plan['locations'] if r['status']=='generated_candidate']
    control = dict(schema='data-recovery-allocation/1', task_id=lease.task,
                   tus=list(grouped(plan)), identities=[{k:r[k] for k in
                   ('unit','tu','section','address','names')} for r in rows])
    lease.json(folder/'control.json', control)
    manifest = dict(candidates=[], linked_targets=['main','ov01','ov02','ov10','ov11','ov12'])
    for tu, locations in grouped(plan).items():
        facts=plan['tus'][tu]
        source=root/facts['path']; obj,asm,_=native_paths(root,facts)
        record=dict(unit=facts['unit'],allocation_task=lease.task,
                    files={facts['path']:dict(path=str(source),sha256=sha(source))},
                    object=dict(path=str(obj),sha256=sha(obj)),
                    assembly=dict(path=str(asm),sha256=sha(asm)),
                    recovered_location_candidates=[{k:r[k] for k in
                    ('unit','tu','section','address','names')} for r in locations])
        path=folder/(tu.replace('/','_')+'.record.json')
        lease.json(path,record)
        manifest['candidates'].append(dict(tu=tu,record=str(path),sha256=sha(path)))
    lease.json(folder/'manifest.json',manifest)
    argv=[sys.executable,'-B',root/'tools/elf_gate.py','--root',root,'--report',folder/'elf.json']
    for unit in manifest['linked_targets']:
        argv+=['--image',f'{unit}={root/"build"/unit/"build"/(unit+".bin")}']
    command(argv,root,folder/'elf.log',lease,timeout=120)
    command([sys.executable,'-B',root/'tools/tu/data_gate.py','--root',root,'--tools-root',root,
             '--manifest',folder/'manifest.json','--allocation-control',folder/'control.json',
             '--gate',folder/'elf.json','--output',folder/'data.json'],root,folder/'data.log',lease,timeout=600)


def integrate(plan_path, candidates, output, lease):
    integration_lease(lease)
    state_path=output/'progress.json'
    state=read_json(state_path) if state_path.exists() else dict(stage='new',accepted=False)
    folder=output/'batch001'
    def advance(stage):
        state.update(stage=stage,updated_at=time.time())
        lease.json(state_path,state)
    if state['stage']=='new':
        snapshot(read_json(plan_path),candidates,output,lease)
        advance('snapshotted')
    plan=read_json(output/'plan.json')
    state.update(locations=sum(map(len,grouped(plan).values())),tus=len(grouped(plan)))
    if state['stage']=='snapshotted':
        publish(plan,output,lease)
        advance('published')
    if state['stage']=='published':
        publish(plan,output,lease)
        compile_sources(lease.root,plan,folder,lease)
        advance('compiled')
    if state['stage']=='compiled':
        publish(plan,output,lease)
        harvest(lease.root,folder,lease)
        compile_sources(lease.root,plan,folder,lease)
        allocations(plan,output,folder,lease)
        advance('allocated')
    if state['stage']=='allocated':
        harvest(lease.root,folder,lease)
        advance('headers')
    if state['stage']=='headers':
        refresh_storage_inputs(plan,output,lease)
        rebuild(lease.root,folder,lease)
        advance('linked')
    if state['stage']=='linked':
        gates(plan,folder,lease)
        state['accepted']=True
        advance('complete')
    return state
