"""Native IOP pairs backed by fresh C emission and complete fixed-layout IRX builds.

Original references are assembled from original instructions and evidenced data
ranges. Compiler objects are only debug-stripped, never patched or normalized.
Unattributed original data stays in explicit target-only report buckets.
Strict defaults require whole-module equality; explicit reporting records actual
linked hashes while retaining container, symbol and relocation identities.
"""
import hashlib
import json
from pathlib import Path
import re
import struct
import subprocess
import sys

from tu import verify_report as verification
from tu.toolchain import Toolchain


def run(command, root):
    return subprocess.run(['timeout', '-k', '2', '30', *map(str, command)], cwd=root,
                          check=True, capture_output=True).stdout.decode()


# 20 SSD code FDRs + the data-only ssd_data.c FDR, and 4 RSSD FDRs.
ORIGINAL_OBJECTS = 25


def load_evidence(root, report_path, *, reporting=False):
    sys.path.insert(0, str(root))
    from tools import iop_module
    manifest = verification.read(root / 'config/iop-build.json')
    registry = verification.read(root / 'config/objects/iop.json')
    registered = {(item['unit'], item.get('fdr_index', int(item['id'].rsplit('-', 1)[1]))): item
                  for item in registry['groups']}
    report = verification.read(report_path)
    verification.require(report.get('mode', 'exact') == ('reporting' if reporting else 'exact'),
                         'IOP evidence reporting mode differs')
    groups_expected = sum(len(entries) for entries in manifest['units'].values())
    verification.require(groups_expected == len(registry['groups']) == ORIGINAL_OBJECTS,
                         'IOP manifest/registry object count differs from the original FDRs')
    verification.require(report.get('ok' if reporting else 'gate_passed') is True and report.get('functions_expected') == 495 and
                         report.get('unique_source_groups') == groups_expected and
                         report.get('manifest_sha256') == verification.digest(root / 'config/iop-build.json'),
                         'IOP report is not a current complete original-object build')
    contract, tools, pins = iop_module.load_contract(root)
    verification.require(report.get('compiler_profile_sha256') ==
                         verification.digest(root / manifest['compiler_profile']), 'IOP report profile changed')
    emissions = {(entry['unit'], entry['source']): entry for entry in report['source_emission']}
    groups = []
    identities = set()
    for unit, entries in manifest['units'].items():
        module = report['module_reports'][unit]
        original = iop_module.Elf(root / 'game/extracted-iso/IOP' /
                                  ('SSD.IRX' if unit == 'iop_ssd' else 'RSSD.IRX'))
        output = Path(report['whole_irx'][unit]['path'])
        verification.require(module.get('build_passed' if reporting else 'gate_passed') is True and
                             (reporting or output.read_bytes() == original.raw) and
                             verification.digest(output) == report['whole_irx'][unit]['sha256'],
                             unit + ': complete raw IRX proof changed')
        if reporting:
            iop_module.reporting_comparison(module['comparison'])
        current = iop_module.Elf(output)
        verification.require(verification.digest(Path(module['linked_elf'])) == module['linked_sha256'],
                             unit + ': actual linked ELF changed')
        identical = current.raw == original.raw
        verification.require(report['whole_irx'][unit]['byte_identical'] == identical and
                             report['whole_irx'][unit]['original_sha256'] == verification.digest(
                                 root / 'game/extracted-iso/IOP' /
                                 ('SSD.IRX' if unit == 'iop_ssd' else 'RSSD.IRX')),
                             unit + ': actual IOP comparison flags/hashes differ')
        inputs = {tuple((f['name'], f['offset'], f['size']) for f in c['functions']): c
                  for c in module['compiler_inputs']}
        for group in entries:
            scope = tuple((f['name'], f['offset'], f['size']) for f in group['functions'])
            record = registered[(unit, group['fdr_index'])]
            # A data-only original FDR (ssd_data.c) has no procedures or code extent.
            verification.require(group['fdr_filename'] in record['attested_source_files'] and
                                 len(scope) == record['function_count'] and
                                 (min(f[1] for f in scope) == int(record['code']['start'], 16) and
                                  max(f[1]+f[2] for f in scope) == int(record['code']['end'], 16)
                                  if scope else record.get('code') is None and group.get('generated_data')) and
                                 record.get('source', group['source']) == group['source'],
                                 'IOP build group differs from the original FDR registry')
            emitted = emissions[(unit, group['source'])]
            candidate = dict(inputs[scope])
            candidate['linked_functions'] = {}
            verification.require(verification.digest(root / group['source']) == group['source_sha256'] ==
                                 emitted['source_sha256'], 'IOP source changed: ' + group['source'])
            for kind in ('object', 'assembly'):
                verification.require(verification.digest(Path(candidate[kind])) ==
                                     candidate[kind + '_sha256'] == emitted[kind + '_sha256'],
                                     'IOP compiler artifact changed: ' + candidate[kind])
            verification.require(all(f['compiler_emitted'] and f['binding_equal'] and f['size_equal']
                                     for f in emitted['functions']), 'IOP compiler emission identity failed')
            for name, offset, size in scope:
                identity = unit, offset, name
                verification.require(identity not in identities, 'duplicate IOP function identity')
                identities.add(identity)
                found = original.named_symbol(name)
                verification.require((found['value'], found['size'], found['type']) == (offset, size, 2),
                                     'IOP original function identity changed')
                actual = current.named_symbol(name)
                verification.require(all(actual[key] == found[key] for key in
                                         ('name', 'value', 'size', 'type', 'binding', 'other')),
                                     'IOP current linked FUNC identity changed')
                before = iop_module.function_bytes(original, found)
                after = iop_module.function_bytes(current, actual)
                candidate['linked_functions'][name] = dict(
                    sha256=hashlib.sha256(after).hexdigest(),
                    original_sha256=hashlib.sha256(before).hexdigest(), exact=before == after,
                    va=actual['value'], size=actual['size'])
                if reporting:
                    audits = [f for f in module['functions'] if f['name'] == name]
                    proof = candidate['linked_functions'][name]
                    verification.require(len(audits) == 1 and
                                         audits[0].get('linked_sha256') == proof['sha256'] and
                                         audits[0].get('original_sha256') == proof['original_sha256'] and
                                         audits[0].get('exact') == proof['exact'],
                                         'IOP actual function bytes differ from fresh module-link evidence')
            groups.append((unit, group, candidate, original))
    verification.require(len(groups) == ORIGINAL_OBJECTS and len(identities) == 495, 'IOP original-object scope drift')
    return groups, contract, tools, report


def original_bss_owners(original):
    """Use ECOFF EXTR ifd ownership only for exact sized original ELF objects."""
    data = original.payload('.mdebug')
    h = struct.unpack_from('<HH23I', data)[2:]
    # ECOFF offsets in these IRXs are absolute file offsets.
    count, offset, names_size, names_offset = h[21], h[22], h[15], h[16]
    verification.require(offset + count * 16 <= len(original.raw) and
                         names_offset + names_size <= len(original.raw), 'invalid IOP EXTR bounds')
    names = original.raw[names_offset:names_offset + names_size]
    owners = {}
    for index in range(count):
        _, fdr, name_at, value, bits = struct.unpack_from('<HhIII', original.raw, offset + index * 16)
        if fdr < 0 or bits & 63 not in (1, 2) or (bits >> 6) & 31 != 3:
            continue
        end = names.find(b'\0', name_at)
        verification.require(0 <= name_at <= end < len(names), 'invalid IOP EXTR name')
        name = names[name_at:end].decode('ascii')
        found = [s for s in original.symbols if s['name'] == name and s['type'] == 1 and s['size']]
        if len(found) != 1:
            continue
        symbol = found[0]
        section = original.sections[symbol['section_index']]
        verification.require(section['name'] == '.bss' and symbol['value'] == section['address'] + value,
                             'IOP EXTR/ELF BSS identity differs: ' + name)
        owners.setdefault(fdr, []).append(dict(section='.bss', offset=symbol['value'],
                                               size=symbol['size'], name=name))
    return owners


def instruction_rows(original, tools, root):
    output = run([tools['objdump'], '-D', '-z', '-j', '.text', '--no-show-raw-insn',
                  '-M', 'no-aliases,gpr-names=numeric', original.path], root)
    rows = {}
    for line in output.splitlines():
        m = re.match(r'^\s*([0-9a-f]+):\s+(\S+)(?:\s+(.*))?$', line)
        if m:
            rows[int(m[1], 16)] = m[2], (m[3] or '').split('#', 1)[0].strip()
    return rows


def contents_symbol(section):
    return f'_iop_{section[1:]}_contents'


def contents_symbol_sections(elf):
    """Loaded data sections without a symbol: objdiff compares no bytes in those.

    Both sides of the comparison get the same section-start symbol, so the
    report really compares the anonymous pools ($LC strings) of a source file.
    """
    covered = {elf.sections[s['section_index']]['name'] for s in elf.symbols
               if s['name'] and s['type'] != 3 and 0 < s['section_index'] < len(elf.sections)}
    return {section['name'] for section in elf.sections
            if section['name'] in ('.data', '.rodata') and section['size'] and section['name'] not in covered}


def assemble_reference(root, destination, group, original, rows, contract, tools, bss, source_object):
    from tools import iop_module
    # Original globals the compiled object also defines; excludes linker symbols such as _fdata.
    compiled = iop_module.Elf(source_object)
    defined = {s['name'] for s in compiled.symbols if s['section_index'] > 0}
    destination.mkdir(parents=True, exist_ok=True)
    funcs = group['functions']
    start = min((f['offset'] for f in funcs), default=0)
    end = max((f['offset'] + f['size'] for f in funcs), default=0)
    pools = [p for p in group.get('generated_data', []) if p['section'] != '.bss']
    # Source-mapped BSS supersedes the EXTR owner rows it contains.
    mapped_bss = [p for p in group.get('generated_data', []) if p['section'] == '.bss']
    bss = [b for b in bss if not any(m['offset'] <= b['offset'] < m['offset'] + m['size'] for m in mapped_bss)]
    bss = bss + [dict(section='.bss', offset=m['offset'], size=m['size'], name=None) for m in mapped_bss]
    original_globals = [s for s in original.symbols if s['name'] and s['binding'] in (1, 2) and
                        0 < s['section_index'] < len(original.sections) and
                        re.fullmatch(r'[A-Za-z_.$][A-Za-z_0-9.$]*', s['name'])]
    externals = {}
    def expression(address):
        for index, pool in enumerate(pools):
            if pool['offset'] <= address < pool['offset'] + pool['size']:
                return f'.L_iop_pool_{index}+0x{address-pool["offset"]:x}'
        exact = [s for s in original_globals if s['value'] == address]
        enclosing = [s for s in original_globals if s['size'] and
                     s['value'] <= address < s['value'] + s['size']]
        preceding = [s for s in original_globals if s['value'] <= address and
                     original.sections[s['section_index']]['address'] <= address <
                     original.sections[s['section_index']]['address'] + original.sections[s['section_index']]['size']]
        choices = exact or enclosing or preceding
        if choices:
            symbol = max(choices, key=lambda s: (s['value'], s['type'] in (1, 2)))
            externals[symbol['name']] = symbol['value']
            return symbol['name'] + (f'+0x{address-symbol["value"]:x}' if address != symbol['value'] else '')
        name = f'_iop_external_{address:08x}'
        externals[name] = address
        return name
    relocations = iop_module.relocation_expressions(original)
    labels = {f['offset']: f for f in funcs}
    def code_or_data_expression(address):
        return f'_iop_target_{address:08x}' if start <= address < end else expression(address)
    source = ['/* Original IOP instructions; reference only, no recovered-source credit. */',
              '.text', '.set noreorder', '.set noat', '.align 4']
    if not funcs:
        source = source[:1]
    for address in range(start, end, 4):
        function = labels.get(address)
        if function:
            symbol = original.named_symbol(function['name'])
            source.extend([('.globl ' if symbol['binding'] == 1 else '.local ') + function['name'],
                           '.type ' + function['name'] + ',@function', function['name'] + ':',
                           f'.size {function["name"]},{function["size"]}'])
        source.append(f'_iop_target_{address:08x}:')
        opcode, operands = rows[address]
        verification.require(opcode not in ('.word', '.byte', '.4byte', '...'), 'undecoded IOP reference instruction')
        relocated = relocations.get(('.text', address))
        if relocated:
            value = int(re.search(r'\+0x([0-9a-f]+)', relocated)[1], 16)
            target = expression(value)
            if relocated.startswith('%hi'):
                operands = operands.rsplit(',', 1)[0] + ',%hi(' + target + ')'
            elif relocated.startswith('%lo'):
                value = '%lo(' + target + ')'
                operands = (re.sub(r'[^,()]+(?=\([^()]+\)$)', lambda _: value, operands)
                            if '(' in operands else operands.rsplit(',', 1)[0] + ',' + value)
            else:
                operands = target
        else:
            def branch(match):
                value = int(match[1], 16)
                return f'_iop_target_{value:08x}' if start <= value < end else expression(value)
            operands = re.sub(r'(?:0x)?([0-9a-f]+)\s+<[^>]+>', branch, operands)
        source.append(opcode + (' ' + operands if operands else ''))
    spans = list(pools) + bss
    pool_names, labelled = set(), set()
    section_origins = {'.text': start} if funcs else {}
    for index, pool in enumerate(pools):
        section = original.by_name[pool['section']]
        section_origins[pool['section']] = pool['offset']
        payload = original.payload(section)[pool['offset']-section['address']:
                                             pool['offset']-section['address']+pool['size']]
        flags = 'aw' if pool['section'] == '.data' else 'a'
        source.extend([f'.section {pool["section"]},"{flags}",@progbits', f'.L_iop_pool_{index}:'])
        if pool['section'] in contents_symbol_sections(compiled) and pool['section'] not in labelled:
            labelled.add(pool['section'])
            source.extend([f'.type {contents_symbol(pool["section"])},@object', contents_symbol(pool['section']) + ':'])
        low, high = pool['offset'], pool['offset'] + pool['size']
        # Named data keeps its original symbol and a loader-relocated word stays
        # a relocation, as in the compiled object. The reference link below still
        # proves the resulting bytes against the original IRX.
        named = {}
        for symbol in original_globals:
            if (symbol['name'] in defined and low <= symbol['value'] < high and
                    original.sections[symbol['section_index']]['name'] == pool['section']):
                named.setdefault(symbol['value'], []).append(symbol)
                pool_names.add(symbol['name'])
        words = {offset for target, offset in relocations if target == pool['section'] and low <= offset < high}
        at = low
        for stop in sorted({*named, *words, high}):
            for begin in range(at, stop, 32):
                source.append('.byte ' + ','.join(map(str, payload[begin-low:min(begin+32, stop)-low])))
            for symbol in named.get(stop, []):
                binding = '.globl ' if symbol['binding'] == 1 else '.weak '
                source.extend([binding + symbol['name'], '.type ' + symbol['name'] + ',@object', symbol['name'] + ':',
                               f'.size {symbol["name"]},{symbol["size"]}'])
            at = stop
            if stop in words:
                value = int(re.search(r'\+0x([0-9a-f]+)', relocations[(pool['section'], stop)])[1], 16)
                source.append('.word ' + code_or_data_expression(value))
                at = stop + 4
    for span in sorted(bss, key=lambda s: s['offset']):
        section_origins.setdefault('.bss', span['offset'])
        named = ['.globl ' + span['name'], '.type ' + span['name'] + ',@object', span['name'] + ':'] if span['name'] else []
        source.extend(['.section .bss,"aw",@nobits', f'.org {span["offset"]-section_origins[".bss"]}',
                       *named, f'.space {span["size"]}',
                       *([f'.size {span["name"]},{span["size"]}'] if span['name'] else [])])
    asm = destination / 'target.s'
    asm.write_text('\n'.join(source) + '\n')
    obj = destination / 'target.o'
    run([tools['assembler'], *contract['assembler_flags'], '-o', obj, asm], root)
    owned_names = {f['name'] for f in funcs} | {s['name'] for s in bss if s['name']} | pool_names
    script = ['OUTPUT_FORMAT("elf32-littlemips")', 'OUTPUT_ARCH(mips:3000)', 'SECTIONS {']
    script += [f'{name} 0x{address:x} : {{ *({name}) }}'
               for name, address in section_origins.items()]
    script += ['}'] + [f'{name} = 0x{address:x};' for name, address in externals.items() if name not in owned_names]
    linker_script = destination / 'target.ld'
    linker_script.write_text('\n'.join(script) + '\n')
    linked = destination / 'target.elf'
    run([tools['linker'], *contract['linker_flags'], '--emit-relocs', '-T', linker_script, '-o', linked, obj], root)
    reference = iop_module.Elf(linked)
    for function in funcs:
        old = original.named_symbol(function['name'])
        new = reference.named_symbol(function['name'])
        verification.require(all(old[k] == new[k] for k in ('value', 'size', 'binding', 'type')),
                             'IOP reference function identity changed')
        def payload(elf, symbol):
            sec = elf.sections[symbol['section_index']]
            at = symbol['value'] - sec['address']
            return elf.payload(sec)[at:at+symbol['size']]
        verification.require(payload(original, old) == payload(reference, new),
                             'IOP original reference differs: ' + function['name'])
    for span in spans:
        old_section, new_section = original.by_name[span['section']], reference.by_name[span['section']]
        old_at, new_at = span['offset']-old_section['address'], span['offset']-new_section['address']
        verification.require(original.payload(old_section)[old_at:old_at+span['size']] ==
                             reference.payload(new_section)[new_at:new_at+span['size']],
                             'IOP original data reference differs')
    proof = dict(original_sha256=iop_module.sha(original.raw), target_sha256=verification.digest(obj),
                 reference_link_sha256=verification.digest(linked), original_function_bytes_equal=True,
                 functions=funcs, data_spans=spans, source_credit=False)
    (destination / 'reference-proof.json').write_text(json.dumps(proof, indent=2) + '\n')
    return obj, spans


def prepare(root, project, report_path, *, reporting=False):
    from tu import objdiff_inputs
    groups, contract, tools, report = load_evidence(root, report_path, reporting=reporting)
    from tools import iop_module
    prefix = Toolchain(root).dir('ps2dev-binutils') / 'mips64r5900el-ps2-elf-'
    units, expected, assigned = [], {}, {}
    originals = {unit: original for unit, _, _, original in groups}
    rows = {unit: instruction_rows(original, tools, root) for unit, original in originals.items()}
    owners = {unit: original_bss_owners(original) for unit, original in originals.items()}
    for unit, group, candidate, original in groups:
        name = unit + '/' + Path(group['source']).stem
        destination = project / name
        target, spans = assemble_reference(root, destination, group, original, rows[unit], contract, tools,
                                           owners[unit].get(group['fdr_index'], []), Path(candidate['object']))
        assigned.setdefault(unit, []).extend(spans)
        base = destination / 'base.o'
        objdiff_inputs.strip_debug(prefix, Path(candidate['object']), base, root)
        additions = [item for section in sorted(contents_symbol_sections(iop_module.Elf(base)))
                     for item in ('--add-symbol', f'{contents_symbol(section)}={section}:0,object,local')]
        if additions:
            run([str(prefix) + 'objcopy', *additions, base], root)
        scope = {f['name']: f['size'] for f in group['functions']}
        verification.require(objdiff_inputs.functions(target) == scope == objdiff_inputs.functions(base),
                             name + ': native object function scope differs')
        units.append(dict(name=name, target_path=str(target), base_path=str(base),
                          metadata=dict(complete=False, source_path=group['source'], progress_categories=[unit])))
        expected[name] = dict(functions=scope, allowed=scope, complete=False,
                              exact={name: row['exact'] for name, row in candidate['linked_functions'].items()})
    accounting = {}
    for unit, original in originals.items():
        destination = project / unit / 'unattributed-data'
        destination.mkdir(parents=True, exist_ok=True)
        source, residual = [], []
        totals = dict(original=0, attributed=0, residual=0)
        for section in original.sections:
            if not section['flags'] & 2 or section['flags'] & 4 or section['type'] not in (1, 8):
                continue
            totals['original'] += section['size']
            spans = sorted((s['offset'], s['offset'] + s['size']) for s in assigned.get(unit, [])
                           if s['section'] == section['name'])
            cursor = section['address']
            for start, end in spans + [(section['address']+section['size'], section['address']+section['size'])]:
                verification.require(cursor <= start <= end <= section['address']+section['size'],
                                     'IOP data ownership overlaps or exceeds the original')
                if cursor < start:
                    residual.append(dict(section=section['name'], offset=cursor, size=start-cursor))
                totals['attributed'] += end-start
                cursor = end
            chunks = [s for s in residual if s['section'] == section['name']]
            if chunks:
                kind = '@nobits' if section['type'] == 8 else '@progbits'
                source.append(f'.section {section["name"]}.iop_unattributed,"aw",{kind}')
            for span in chunks:
                totals['residual'] += span['size']
                if section['type'] == 8:
                    source.append(f'.space {span["size"]}')
                else:
                    start = span['offset']-section['address']
                    data = original.payload(section)[start:start+span['size']]
                    for at in range(0, len(data), 32):
                        source.append('.byte ' + ','.join(map(str, data[at:at+32])))
        verification.require(totals['original'] == totals['attributed'] + totals['residual'],
                             'IOP data accounting lost or duplicated original bytes')
        asm, obj = destination / 'target.s', destination / 'target.o'
        asm.write_text('\n'.join(source) + '\n')
        run([tools['assembler'], *contract['assembler_flags'], '-o', obj, asm], root)
        name = unit + '/unattributed-data'
        units.append(dict(name=name, target_path=str(obj), metadata=dict(complete=False, progress_categories=[unit])))
        expected[name] = dict(functions={}, allowed={}, complete=False)
        accounting[unit] = dict(totals=totals, residual_spans=residual)
        (destination / 'reference-proof.json').write_text(json.dumps(accounting[unit], indent=2)+'\n')
    return units, expected, dict(build_report=str(report_path), build_report_sha256=verification.digest(report_path),
                                 assigned_data=assigned, original_objects=ORIGINAL_OBJECTS, functions=495,
                                 data_accounting=accounting, data_ownership_complete=False)
