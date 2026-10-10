#!/usr/bin/env python3
"""Verify compiler provenance and linked placement of a frozen data batch.

This produces acceptance evidence, never report scores. Whole-file comparison
and the existing TU source/function audit remain required alongside it.
"""
import argparse
import ast
import bisect
import hashlib
import importlib
import json
from pathlib import Path
import re
import struct
import subprocess
import os
import signal
import sys

CANONICAL=Path('/home/pc/xenosaga1-port/xenosaga-i-decomp')
sys.dont_write_bytecode=True
# A private integration checkout can carry the data-carve patch under test.
# Set DATA_GATE_TOOLS_ROOT to that checkout's root; default to the current
# canonical tree for normal use. The CLI selects --tools-root (default --root).
# This avoids mixing a private source build with canonical carve logic.
PRIVATE_TOOLS_ROOT = Path(__import__('os').environ['DATA_GATE_TOOLS_ROOT']) if __import__('os').environ.get('DATA_GATE_TOOLS_ROOT') else CANONICAL
sys.path.insert(0,str(CANONICAL/'tools'))
if PRIVATE_TOOLS_ROOT.resolve() != CANONICAL.resolve():
    sys.path.insert(0,str(PRIVATE_TOOLS_ROOT/'tools'))
from tu import data_carve
from tu.elfinfo import Elf
import tu_audit


def configure_tools_root(path):
    """Load the data-carve and assembler audit code from the requested checkout."""
    global data_carve, Elf, tu_audit
    tools = Path(path).resolve() / 'tools'
    require((tools / 'tu' / 'data_carve.py').is_file(),
            f'no tools/tu/data_carve.py beneath tools root {path}')
    for key in list(sys.modules):
        if key == 'tu' or key.startswith('tu.') or key == 'tu_audit':
            del sys.modules[key]
    sys.path[:] = [str(tools), str(CANONICAL/'tools')] + [
        item for item in sys.path if item not in (str(tools), str(CANONICAL/'tools'))]
    data_carve = importlib.import_module('tu.data_carve')
    Elf = importlib.import_module('tu.elfinfo').Elf
    tu_audit = importlib.import_module('tu_audit')


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def require(value,message):
    if not value:raise ValueError(message)


def native_main_carve_mapping(entry, record, owner_name, owner_va, size, raw_section,
                              carved_object_path, carved_section, carved_symbol,
                              source_path, raw_object_path, assembly_path):
    """Verify the standard MAIN splitter's NOBITS mapping from current inputs.

    MAIN's native build uses .carved.o directly, rather than the private overlay
    allocator's .allocated.carved.o. Recompute its complete section partition
    from the current raw object and registry, then prove the selected symbol's
    offset and extent in that partition. NOBITS bytes supply no evidence.
    """
    require(record.get('unit') == 'main' and entry['tu'].startswith('main/'),
            entry['tu'] + ': native carve storage route is MAIN-only')
    unit_dir = carved_object_path.parents[2]
    root = unit_dir.parents[1]
    facts = next((t for t in json.loads((root / 'config/tu-build.json').read_text())['tus']
                  if t['id'] == entry['tu']), None)
    require(facts and facts.get('in_scope') and facts['unit'] == 'main' and
            unit_dir.resolve() == (root / 'build/main').resolve() and
            carved_object_path.resolve() == (unit_dir / ('build/c/' + facts['name'] + '.carved.o')).resolve() and
            raw_object_path.resolve() == (unit_dir / ('build/c/' + facts['name'] + '.o')).resolve() and
            source_path.resolve() == (root / facts['path']).resolve(),
            entry['tu'] + ': native storage input paths differ from the TU build')
    source_pin = record['files'].get(facts['path'], {})
    require(record['object'].get('sha256') == sha(raw_object_path) and
            Path(record['object']['path']).resolve() == raw_object_path.resolve() and
            record['assembly'].get('sha256') == sha(assembly_path) and
            Path(record['assembly']['path']).resolve() == assembly_path.resolve() and
            source_pin.get('sha256') == sha(source_path),
            entry['tu'] + ': native storage source/object/assembly pins differ')
    map_path = Path(str(carved_object_path) + '.json')
    require(map_path.is_file(), entry['tu'] + ': missing native carve map')
    mapping = json.loads(map_path.read_text())
    require((unit_dir / mapping.get('out', '')).resolve() == carved_object_path.resolve() and
            (unit_dir / mapping.get('object', '')).resolve() == raw_object_path.resolve(),
            entry['tu'] + ': native carve map input/output paths differ')
    raw = Elf(raw_object_path.read_bytes())
    plans = data_carve.split_plans(root, 'main', unit_dir, entry['tu'], raw)
    pieces = []
    for index, part in enumerate(plans.get(raw_section, {}).get('pieces', [])):
        pieces.append(dict(section=data_carve.split_section_name(raw_section, index),
            offset=hex(part['offset']), length=hex(part['length']),
            va=data_carve.h8(part['lo']), run_end=data_carve.h8(part['hi'])))
    require(pieces and pieces == mapping.get('sections', {}).get(raw_section, {}).get('pieces'),
            entry['tu'] + ': native NOBITS partition differs from fresh raw input/registry derivation')
    raw_symbols = [s for s in raw.symbols if s.name == owner_name and
                   0 < s.shndx < len(raw.sections) and raw.sections[s.shndx].name == raw_section]
    require(len(raw_symbols) == 1, entry['tu'] + '/' + owner_name + ': raw owner is not unique')
    symbol = raw_symbols[0]
    raw_sec = raw.sections[symbol.shndx]
    emission = emitted_storage(assembly_path.read_text()).get(owner_name, {})
    common_proof = None
    if emission.get('section') == 'COMMON':
        compiler_object = unit_dir / ('build/c/' + facts['name'] + '.raw.o')
        compiler_assembly = Path(str(compiler_object) + '.s')
        compiler = Elf(compiler_object.read_bytes())
        common_symbols = [s for s in compiler.symbols if s.name == owner_name and s.shndx in (0xff03, 0xfff2)]
        require(len(common_symbols) == 1 and compiler_assembly.is_file() and
                sha(compiler_assembly) == sha(assembly_path),
                entry['tu'] + '/' + owner_name + ': native COMMON compiler input differs')
        common = common_symbols[0]
        require((common.bind, common.type, common.size) == (symbol.bind, symbol.type, size) and
                common.value == emission.get('alignment', common.value) and common.value > 0 and
                common.value & (common.value - 1) == 0 and raw_sec.align >= common.value and
                symbol.value % common.value == 0,
                entry['tu'] + '/' + owner_name + ': native COMMON allocation identity or alignment differs')
        common_proof = dict(compiler_object=str(compiler_object), compiler_object_sha256=sha(compiler_object),
                            compiler_assembly_sha256=sha(compiler_assembly), alignment=common.value)
    require(raw_sec.type == carved_section.type == 8 and
            raw_sec.flags & 3 == carved_section.flags & 3 == 3 and
            (symbol.bind, symbol.type, symbol.size) ==
            (carved_symbol.bind, carved_symbol.type, carved_symbol.size) and
            (emission.get('section') == raw_section or common_proof is not None) and emission.get('size') == size and
            (symbol.size == size or symbol.type == symbol.bind == symbol.size == 0 and
             emission.get('extent_kind') == 'label-space') and
            symbol.value + size <= raw_sec.size,
            entry['tu'] + '/' + owner_name + ': raw/carved compiler storage identities or extent differ')
    exact = [p for p in pieces if p['section'] == carved_section.name and
             int(p['offset'], 0) <= symbol.value and
             symbol.value + size <= int(p['offset'], 0) + int(p['length'], 0) and
             int(p['va'], 0) + symbol.value - int(p['offset'], 0) == owner_va and
             carved_symbol.value == symbol.value - int(p['offset'], 0) and
             owner_va + size <= int(p['run_end'], 0)]
    require(len(exact) == 1 and carved_section.size == int(exact[0]['length'], 0),
            entry['tu'] + '/' + owner_name + ': selected native section does not map the exact owner span')
    return dict(kind='native-main-nobits-carve', raw_section=raw_section,
                raw_object_sha256=sha(raw_object_path), carve_map=str(map_path),
                carve_map_sha256=sha(map_path), allocated_offset=hex(symbol.value),
                carved_section=carved_section.name, carved_offset=hex(carved_symbol.value),
                address=data_carve.h8(owner_va), size=size, piece=exact[0], common_allocation=common_proof)


def allocated_carve_mapping(entry, record, owner_name, owner_va, size, raw_section,
                            carved_object_path, carved_section, carved_symbol,
                            source_path, raw_object_path, assembly_path):
    """Prove one local NOBITS symbol's exact raw-to-carved allocation mapping.

    This is deliberately narrower than accepting a `.bss.carve.*` spelling:
    the allocator sidecars must name this exact output object, the allocated
    input object must be hash-pinned and identify the symbol at its raw section
    offset, and one exact allocation piece must translate that offset to the
    final mapped VA and carved-section-relative symbol offset.
    """
    if record.get('unit') == 'main' and carved_object_path.name.endswith('.carved.o'):
        return native_main_carve_mapping(entry, record, owner_name, owner_va, size, raw_section,
                                        carved_object_path, carved_section, carved_symbol,
                                        source_path, raw_object_path, assembly_path)
    require(carved_object_path.name.endswith('.allocated.carved.o'),
            entry['tu'] + '/' + owner_name + ': carved NOBITS section has no recognized allocator output')
    map_path = Path(str(carved_object_path) + '.json')
    require(map_path.is_file(), entry['tu'] + '/' + owner_name + ': missing exact carved allocation map')
    carve_map = json.loads(map_path.read_text())
    unit_dir = carved_object_path.parents[3]
    expected_out = (unit_dir / carve_map.get('out', '')).resolve()
    require(expected_out == carved_object_path.resolve(),
            entry['tu'] + '/' + owner_name + ': carve map output path differs from selected object')
    allocated_rel = carve_map.get('object')
    require(isinstance(allocated_rel, str) and allocated_rel,
            entry['tu'] + '/' + owner_name + ': carve map has no allocated input object')
    allocated_path = (unit_dir / allocated_rel).resolve()
    require(allocated_path.is_file() and allocated_path.name.endswith('.allocated.o'),
            entry['tu'] + '/' + owner_name + ': mapped allocated input object is missing')
    report_candidates = [allocated_path.with_name(allocated_path.name + '.report.json'),
                         allocated_path.with_name(allocated_path.name[:-len('.allocated.o')] + '.allocated.json')]
    allocated_report_path = next((path for path in report_candidates if path.is_file()), None)
    require(allocated_report_path is not None,
            entry['tu'] + '/' + owner_name + ': missing hash-pinned allocator report')
    allocated_report = json.loads(allocated_report_path.read_text())
    allocation_output = allocated_report.get('output') or allocated_report.get('link_allocation') or {}
    link_input_path = Path(allocation_output.get('path', ''))
    if not link_input_path.is_absolute():
        link_input_path = (unit_dir / link_input_path).resolve()
    require(link_input_path.resolve() == allocated_path and
            allocation_output.get('sha256') == sha(allocated_path),
            entry['tu'] + '/' + owner_name + ': allocator report does not hash-pin the exact carve input')
    source_pins = [value for value in record.get('files', {}).values()
                   if Path(value.get('path', '')).resolve() == source_path.resolve()]
    allocation_input = allocated_report.get('input') or {}
    require(Path(record['object']['path']).resolve() == raw_object_path.resolve() and
            record['object'].get('sha256') == sha(raw_object_path) and
            Path(allocation_input.get('path', '')).resolve() == raw_object_path.resolve() and
            allocation_input.get('sha256') == sha(raw_object_path) and
            record['assembly'].get('sha256') == sha(assembly_path) and
            Path(record['assembly']['path']).resolve() == assembly_path.resolve() and
            len(source_pins) == 1 and source_pins[0].get('sha256') == sha(source_path),
            entry['tu'] + '/' + owner_name + ': current source/raw-object/assembly pins do not bind this allocation')

    storage_evidence = [candidate for candidate in allocated_report.get('recovered_location_candidates', [])
                        if (candidate.get('storage_owner') or {}).get('name') == owner_name and
                        int((candidate.get('storage_owner') or {}).get('address', candidate.get('address', '0')), 16) == owner_va]
    require(storage_evidence, entry['tu'] + '/' + owner_name + ': allocator report has no exact owner candidate')
    evidence = storage_evidence[0].get('evidence') or {}
    require(evidence.get('source_sha256') == sha(source_path) and
            evidence.get('object_sha256') == sha(raw_object_path) and
            evidence.get('assembly_sha256') == sha(assembly_path),
            entry['tu'] + '/' + owner_name + ': allocator source/object/assembly provenance differs')

    raw_elf = Elf(raw_object_path.read_bytes())
    allocated_elf = Elf(allocated_path.read_bytes())
    raw_symbols = [s for s in raw_elf.symbols if s.name == owner_name and
                   0 < s.shndx < len(raw_elf.sections) and raw_elf.sections[s.shndx].name == raw_section]
    allocated_symbols = [s for s in allocated_elf.symbols if s.name == owner_name and
                         0 < s.shndx < len(allocated_elf.sections) and
                         allocated_elf.sections[s.shndx].name == raw_section]
    require(len(raw_symbols) == 1 and len(allocated_symbols) == 1,
            entry['tu'] + '/' + owner_name + ': raw/allocated NOBITS symbol identity is not unique')
    raw_symbol, allocated_symbol = raw_symbols[0], allocated_symbols[0]
    require(raw_elf.sections[raw_symbol.shndx].type == 8 and
            raw_elf.sections[raw_symbol.shndx].flags & 3 == 3 and
            allocated_elf.sections[allocated_symbol.shndx].type == 8 and
            allocated_elf.sections[allocated_symbol.shndx].flags & 3 == 3 and
            (raw_symbol.bind, raw_symbol.type, raw_symbol.size, raw_symbol.value) ==
            (allocated_symbol.bind, allocated_symbol.type, allocated_symbol.size, allocated_symbol.value),
            entry['tu'] + '/' + owner_name + ': raw and allocator NOBITS symbol facts differ')
    allocated_offset = raw_symbol.value
    require(allocated_offset == allocated_symbol.value,
            entry['tu'] + '/' + owner_name + ': allocated input offset differs from raw compiler symbol')
    verified_rows = [row for row in allocation_output.get('verified_symbols', [])
                     if row.get('name') == owner_name and row.get('section') == raw_section]
    require(len(verified_rows) == 1, entry['tu'] + '/' + owner_name + ': allocator has no unique symbol map row')
    verified_row = verified_rows[0]
    proof_size = verified_row.get('proven_size')
    proof_size = int(proof_size, 0) if isinstance(proof_size, str) else int(proof_size)
    row_offset = int(verified_row.get('allocated_offset', '-1'), 0)
    raw_section_obj = raw_elf.sections[raw_symbol.shndx]
    verified_alignment = int(verified_row.get('alignment', 1))
    require(row_offset == allocated_offset and proof_size == size and
            verified_row.get('bind') == raw_symbol.bind and verified_row.get('type') == raw_symbol.type,
            entry['tu'] + '/' + owner_name + ': allocator symbol row disagrees with raw owner extent/identity')
    require(raw_section_obj.align >= verified_alignment and allocated_offset % verified_alignment == 0,
            entry['tu'] + '/' + owner_name + ': raw compiler section/symbol alignment is weaker than allocator evidence')
    section_summary = (allocation_output.get('sections') or {}).get(raw_section) or {}
    require(section_summary.get('type') == 8 and section_summary.get('size', 0) >= allocated_offset + size and
            section_summary.get('align', 0) >= verified_row.get('alignment', 1),
            entry['tu'] + '/' + owner_name + ': allocator NOBITS section does not contain proven owner span')

    pieces = ((carve_map.get('sections') or {}).get(raw_section) or {}).get('pieces') or []
    exact_pieces = []
    for piece in pieces:
        piece_offset = int(piece.get('offset', '-1'), 0)
        piece_length = int(piece.get('length', '0'), 0)
        piece_va = int(piece.get('va', '0'), 0)
        piece_end = int(piece.get('run_end', '0'), 0)
        if (piece.get('section') == carved_section.name and piece_offset <= allocated_offset and
                allocated_offset + size <= piece_offset + piece_length and
                piece_va + (allocated_offset - piece_offset) == owner_va and
                piece_va <= owner_va and owner_va + size <= piece_end and
                carved_symbol.value == allocated_offset - piece_offset):
            exact_pieces.append(piece)
    require(len(exact_pieces) == 1,
            entry['tu'] + '/' + owner_name + ': no unique allocator piece maps the exact raw owner offset to carved section/VA')
    return dict(raw_section=raw_section, raw_object_sha256=sha(raw_object_path),
                allocated_object=str(allocated_path), allocated_object_sha256=sha(allocated_path),
                allocation_report=str(allocated_report_path), allocation_report_sha256=sha(allocated_report_path),
                carve_map=str(map_path), carve_map_sha256=sha(map_path),
                allocated_offset=f'0x{allocated_offset:X}', carved_section=carved_section.name,
                carved_offset=f'0x{carved_symbol.value:X}', address=f'0x{owner_va:08X}', size=size,
                piece=dict(exact_pieces[0]))


def is_nobits_derivation_problem(problem):
    """Recognize only section-local .bss/.sbss carve diagnostics.

    The linked storage proof checks those sections from compiler definitions,
    allocated object symbols, the original map, and the final link. A combined
    COMMON span can legitimately differ from the scaffold's individual item
    boundaries, so data_carve's BSS byte-run heuristic is not authoritative.
    Initialized-section diagnostics remain fatal.
    """
    if isinstance(problem, dict):
        section = problem.get('section')
        if section in ('.bss', '.sbss'):
            return True
        problem = problem.get('reason', problem.get('message', ''))
    return re.search(r'(?<![\w])\.(?:bss|sbss)(?=[:\s])', str(problem)) is not None


def emitted_data(text):
    """cc1 labels/common definitions outside inline scaffold/ASM blocks."""
    app=False
    section='.text'
    emitted=set()
    for line in text.splitlines():
        if line.strip()=='#APP':app=True;continue
        if line.strip()=='#NO_APP':app=False;continue
        if app:continue
        sec=re.match(r'\s*\.section\s+([^,\s]+)',line)
        simple=re.match(r'\s*(\.(?:rodata|rdata|data|sdata|lit4|lit8|bss|sbss|text))\s*(?:$|#)',line)
        if sec:section=sec[1]
        elif simple:section=simple[1]
        common=re.match(r'\s*\.(?:l?comm)\s+([^,\s]+)',line)
        if common:emitted.add(common[1])
        label=re.match(r'^\s*([A-Za-z_.$][\w.$]*):\s*(?:$|#)',line)
        if label and section not in ('.text','.init','.fini'):emitted.add(label[1])
    return emitted


def emitted_storage(text):
    """Compiler-authored storage directives outside inline ASM blocks.

    COMMON/SCOMMON entries are not NOBITS bytes. Their compiler declaration,
    linked address, size, binding, and references are checked separately.
    """
    app = False
    section = '.text'
    out = {}
    active_label = None
    for line in text.splitlines():
        if line.strip() == '#APP':
            app = True
            continue
        if line.strip() == '#NO_APP':
            app = False
            continue
        if app:
            continue
        sec = re.match(r'\s*\.section\s+([^,\s]+)', line)
        simple = re.match(r'\s*(\.(?:data|sdata|bss|sbss|scommon|rodata|lit4|lit8))\s*(?:$|#)', line)
        if sec:
            section = sec[1]
            active_label = None
        elif simple:
            section = simple[1]
            active_label = None
        common = re.match(r'\s*\.(comm|lcomm)\s+([^,\s]+)\s*,\s*(0x[\da-fA-F]+|\d+)(?:\s*,\s*(0x[\da-fA-F]+|\d+))?', line)
        if common:
            name, size = common[2], int(common[3], 0)
            align = int(common[4], 0) if common[4] else 1
            out[name] = dict(name=name, section='COMMON' if common[1] == 'comm' else '.bss',
                             size=size, alignment=align, directive=common[1], extent_kind='common-directive')
            active_label = None
            continue
        label = re.match(r'^\s*([A-Za-z_.$][\w.$]*):\s*(?:$|#)', line)
        if label and section in ('.bss', '.sbss', '.scommon'):
            active_label = label[1]
            out[active_label] = dict(name=active_label, section=section, size=0,
                                     alignment=1, extent_kind='section-label')
            continue
        align = re.match(r'\s*\.align\s+(0x[\da-fA-F]+|\d+)', line)
        if align and active_label in out:
            exponent = int(align[1], 0)
            if exponent < 32:
                out[active_label]['alignment'] = max(out[active_label].get('alignment', 1), 1 << exponent)
            continue
        balign = re.match(r'\s*\.balign\s+(0x[\da-fA-F]+|\d+)', line)
        if balign and active_label in out:
            out[active_label]['alignment'] = max(out[active_label].get('alignment', 1), int(balign[1], 0))
            continue
        space = re.match(r'\s*\.(?:space|skip|zero)\s+(0x[\da-fA-F]+|\d+)', line)
        if space and active_label in out:
            out[active_label]['size'] += int(space[1], 0)
            out[active_label]['extent_kind'] = 'label-space'
    return out


def storage_relocations(elf, symbol_index, emitted_functions=None):
    """Relocation targets to storage, optionally restricted to cc1 C functions.

    `emitted_functions=None` is used only for a preserved INCLUDE_ASM consumer
    of a separately compiler-defined data object. The object/link identity
    checks still establish the storage owner; this fallback merely records an
    actual relocation from that same TU's linked input object.
    """
    syms = elf.symbols
    functions = []
    for sym in syms:
        if (emitted_functions is None or sym.name in emitted_functions) and sym.type == 2 and sym.size and 0 < sym.shndx < len(elf.sections):
            functions.append((sym.shndx, sym.value, sym.value + sym.size))
    gp = data_carve.gp0(elf)
    found = []
    for relsec in elf.sections:
        if relsec.type not in (4, 9) or relsec.info >= len(elf.sections):
            continue
        target_sec = elf.sections[relsec.info]
        entries = data_carve._rel_entries(elf, relsec)
        text = elf.section_bytes(target_sec)
        for i, (off, kind, sym_index, addend) in enumerate(entries):
            if sym_index != symbol_index or not any(target_sec.index == idx and lo <= off < hi
                                                     for idx, lo, hi in functions):
                continue
            if addend is not None:
                value = addend
            elif kind == 2:  # R_MIPS_32
                value = struct.unpack_from('<i', text, off)[0]
            elif kind == 5:  # R_MIPS_HI16, paired with the next LO16
                partner = next((j for j in range(i + 1, len(entries))
                                if entries[j][1] == 6 and entries[j][2] == sym_index), None)
                require(partner is not None, 'unpaired R_MIPS_HI16 to storage symbol')
                high = struct.unpack_from('<I', text, off)[0] & 0xffff
                low = struct.unpack_from('<I', text, entries[partner][0])[0] & 0xffff
                value = (high << 16) + data_carve._sext16(low)
            elif kind in (6, 7, 8):  # LO16, GPREL16, LITERAL
                word = struct.unpack_from('<I', text, off)[0]
                value = data_carve._sext16(word)
                if kind in (7, 8) and syms[symbol_index].bind == 0:
                    value += gp
            else:
                raise ValueError(f'unsupported storage relocation type {kind}')
            source_function=next((sym.name for sym in syms if sym.type==2 and sym.size and
                                  sym.shndx==target_sec.index and sym.value<=off<sym.value+sym.size),None)
            found.append(dict(section=target_sec.name, offset=off, type=kind, addend=value,
                              source_function=source_function,
                              source_kind=('cc1' if source_function in (emitted_functions or set()) else
                                           'same_tu_object' if source_function else 'unresolved_function_extent')))
    return found


def cc1_functions(text):
    """Names of C function definitions, excluding included scaffold blocks."""
    app = False
    out = set()
    for line in text.splitlines():
        if line.strip() == '#APP':
            app = True
            continue
        if line.strip() == '#NO_APP':
            app = False
            continue
        if not app:
            match = re.match(r'\s*\.ent\s+(\S+)', line)
            if match:
                out.add(match[1])
    return out


def source_storage_array_extent(text, owner_name, owner_size):
    """Corroborate an exact compiler extent with one local, fixed-size array.

    This does not establish an original extent. Callers must independently
    prove the complete original scaffold span and compiler storage definition.
    """
    if len(text) > 1024 * 1024 or type(owner_size) is not int or owner_size <= 0:
        return None
    clean = re.sub(r'/\*.*?\*/|//[^\n]*', '', text, flags=re.S)
    c_type = r'((?:unsigned|signed)\s+(?:char|short(?:\s+int)?|int)|[A-Za-z_]\w*)'
    declarations = re.findall(r'\bstatic\s+' + c_type + r'\s+' +
                              re.escape(owner_name) + r'\s*\[([^\]]+)\]\s*;', clean)
    if len(declarations) != 1:
        return None
    typename, expression = declarations[0]
    try:
        node = ast.parse(expression.strip(), mode='eval').body
    except (SyntaxError, ValueError):
        return None
    if not isinstance(node, ast.Constant) or type(node.value) is not int:
        return None
    count = node.value
    if not 1 < count <= 1024 * 1024 or owner_size % count:
        return None
    return dict(kind='local_fixed_array_compiler_extent', owner_name=owner_name,
                owner_type=typename, owner_size=owner_size, element_count=count,
                element_stride=owner_size // count)


def source_storage_layout_text(root, source_path):
    """Collect bounded quoted-header text needed to resolve local C layouts.

    This is only a declaration lookup for the closed layout parser below; it
    does not preprocess macros or establish compiler/original storage. The
    source, compiler object, original map and linked image remain the callers'
    independent authorities. Missing or oversized headers are simply omitted,
    which leaves unknown types unsupported.
    """
    root = Path(root).resolve()
    source_path = Path(source_path).resolve()
    try:
        source_path.relative_to(root)
    except ValueError:
        return source_path.read_text(errors='surrogateescape')
    queue = [source_path]
    seen = set()
    queued = {source_path}
    chunks = []
    total_size = 0
    include_re = re.compile(r'^\s*#\s*include\s*"([^"]+)"', re.M)
    while queue and len(seen) < 64 and total_size <= 4 * 1024 * 1024:
        path = queue.pop(0).resolve()
        try:
            path.relative_to(root)
        except ValueError:
            continue
        if path in seen or not path.is_file():
            continue
        seen.add(path)
        if path.stat().st_size > 1024 * 1024:
            continue
        text = path.read_text(errors='surrogateescape')
        total_size += len(text)
        if total_size > 4 * 1024 * 1024:
            break
        lines = text.splitlines(keepends=True)
        directives = [(index, re.match(r'\s*#\s*(ifndef|define|endif)\b\s*([A-Za-z_]\w*)?', line))
                      for index, line in enumerate(lines)]
        directives = [(index, match) for index, match in directives if match]
        if len(directives) >= 3:
            first_index, first = directives[0]
            second_index, second = directives[1]
            last_index, last = directives[-1]
            if (first.group(1) == 'ifndef' and second.group(1) == 'define' and
                    first.group(2) == second.group(2) and last.group(1) == 'endif' and
                    not ''.join(lines[last_index + 1:]).strip()):
                lines[first_index] = '\n' if lines[first_index].endswith('\n') else ''
                lines[second_index] = '\n' if lines[second_index].endswith('\n') else ''
                lines[last_index] = '\n' if lines[last_index].endswith('\n') else ''
                text = ''.join(lines)
        chunks.append(text)
        for include in include_re.findall(text):
            candidates = (path.parent / include, root / 'include' / include,
                          root / 'src' / include, root / 'src' / path.parent.name / include,
                          root / include)
            for candidate in candidates:
                if not candidate.is_file():
                    continue
                resolved = candidate.resolve()
                try:
                    resolved.relative_to(root)
                except ValueError:
                    continue
                if resolved not in seen and resolved not in queued and len(queue) < 128:
                    queue.append(resolved)
                    queued.add(resolved)
                break
    return '\n'.join(chunks)


def source_storage_member_extent(text, owner_name, owner_size, offset):
    """Closed 32-bit C layout proof for an exact member of a plain struct.

    Unsupported declarations return no proof. This never infers an extent
    from the next alias or from the remainder of an enclosing storage object.
    Primitive member arrays are supported at their start and aligned element
    starts. Nested plain structs are laid out recursively, but aliases inside
    nested aggregates are not inferred. Fixed multidimensional owner arrays
    use their bounded dimension product and the same exact element stride.
    The caller independently binds the
    complete owner object to cc1 and the link.
    """
    if (len(text) > 4 * 1024 * 1024 or type(owner_size) is not int or owner_size <= 0 or
            type(offset) is not int or offset < 0):
        return None
    clean = re.sub(r'/\*.*?\*/|//[^\n]*', '', text, flags=re.S)
    declarations = re.findall(r'\b(?:(static|extern)\s+)?((?!return\b)[A-Za-z_]\w*)\s+' +
                              re.escape(owner_name) + r'\s*((?:\[[^\]]+\]\s*)*)\s*;', clean)
    definitions = [row for row in declarations if row[0] != 'extern']
    if len(definitions) != 1:
        return None
    _, typename, array_dimensions = definitions[0]
    conditional_depth = []
    depth = 0
    for line in clean.splitlines(keepends=True):
        conditional_depth.append(depth)
        directive = re.match(r'\s*#\s*(if|ifdef|ifndef|endif)\b', line)
        if directive:
            if directive.group(1) == 'endif':
                depth = max(0, depth - 1)
            else:
                depth += 1
    line_starts = [0]
    line_starts.extend(m.end() for m in re.finditer('\n', clean))

    def is_unconditional(position):
        line_index = max(0, bisect.bisect_right(line_starts, position) - 1)
        return line_index < len(conditional_depth) and conditional_depth[line_index] == 0

    definitions = {}
    structure_re = re.compile(
        r'\btypedef\s+struct(?:\s+([A-Za-z_]\w*))?\s*\{([^{}]*)\}\s*([A-Za-z_]\w*)\s*;', re.S)
    for match in structure_re.finditer(clean):
        if not is_unconditional(match.start()):
            continue
        tag, body, alias = match.groups()
        aliases = [alias] + ([tag] if tag else [])
        for name in aliases:
            definitions.setdefault(name, set()).add(body)
    if any(len(bodies) != 1 for bodies in definitions.values()):
        return None
    definitions = {name: next(iter(bodies)) for name, bodies in definitions.items()}
    array_typedefs = {}
    array_typedef_re = re.compile(
        r'\btypedef\s+([^;{}]+?)\s+([A-Za-z_]\w*)\s*((?:\s*\[[^\]]+\])+)[ \t]*;', re.S)
    for match in array_typedef_re.finditer(clean):
        if not is_unconditional(match.start()):
            continue
        base_type, alias, dimensions = match.groups()
        dimensions = re.findall(r'\[([^\]]+)\]', dimensions)
        if dimensions:
            array_typedefs.setdefault(alias, set()).add((base_type.strip(), tuple(dimensions)))
    if any(len(rows) != 1 for rows in array_typedefs.values()):
        return None
    array_typedefs = {name: next(iter(rows)) for name, rows in array_typedefs.items()}

    def constant(expression):
        try:
            node = ast.parse(expression.strip(), mode='eval').body
        except (SyntaxError, ValueError):
            return None
        def value(node):
            if isinstance(node, ast.Constant) and type(node.value) is int:
                return node.value
            if isinstance(node, ast.BinOp) and isinstance(node.op, (ast.Add, ast.Sub)):
                a, b = value(node.left), value(node.right)
                if a is not None and b is not None:
                    return a + b if isinstance(node.op, ast.Add) else a - b
            return None
        result = value(node)
        return result if result is not None and 0 < result <= 1024 * 1024 else None

    scalar_sizes = {'char': 1, 'signed char': 1, 'unsigned char': 1, 'u8': 1, 's8': 1,
                    'short': 2, 'short int': 2, 'signed short': 2,
                    'signed short int': 2, 'unsigned short': 2,
                    'unsigned short int': 2, 'u16': 2, 's16': 2,
                    'int': 4, 'signed': 4, 'unsigned': 4,
                    'unsigned int': 4, 'signed int': 4, 'float': 4,
                    'u32': 4, 's32': 4}
    active = set()

    def structure_layout(type_name):
        type_name = ' '.join(type_name.split())
        type_name = re.sub(r'^(?:(?:const|volatile)\s+)+', '', type_name)
        if type_name in scalar_sizes:
            return scalar_sizes[type_name], scalar_sizes[type_name], None
        if re.fullmatch(r'(?:const\s+|volatile\s+)*(?:struct\s+)?[A-Za-z_]\w*\s*\*', type_name):
            return 4, 4, None
        alias_name = type_name[7:].strip() if type_name.startswith('struct ') else type_name
        if alias_name in definitions and alias_name in array_typedefs:
            return None
        array_alias = array_typedefs.get(alias_name)
        if array_alias is not None:
            if alias_name in active:
                return None
            active.add(alias_name)
            base_type, dimensions = array_alias
            layout = structure_layout(base_type)
            count = 1
            for dimension in dimensions:
                value = constant(dimension)
                if value is None or count * value > 1024 * 1024:
                    active.remove(alias_name)
                    return None
                count *= value
            active.remove(alias_name)
            if layout is None:
                return None
            item_size, item_alignment, _ = layout
            return item_size * count, item_alignment, None
        name = type_name[7:].strip() if type_name.startswith('struct ') else type_name
        body = definitions.get(name)
        if body is None or name in active:
            return None
        active.add(name)
        fields = []
        cursor = 0
        alignment = 1
        declarations = body.strip()
        if not declarations.endswith(';'):
            active.remove(name)
            return None
        for declaration in declarations[:-1].split(';'):
            match = re.fullmatch(r'\s*(.*?)\s*\b([A-Za-z_]\w*)\s*(?:\[([^\]]+)\])?\s*',
                                 declaration, re.S)
            if not match:
                active.remove(name)
                return None
            field_type, member_name, count_expr = match.groups()
            field_type = ' '.join(field_type.split())
            layout = structure_layout(field_type)
            if layout is None:
                active.remove(name)
                return None
            item_size, item_alignment, nested_fields = layout
            count = constant(count_expr) if count_expr else 1
            if count is None or (count_expr and field_type.endswith('*')):
                active.remove(name)
                return None
            plain_type = re.sub(r'^(?:(?:const|volatile)\s+)+', '', field_type)
            cursor = (cursor + item_alignment - 1) // item_alignment * item_alignment
            fields.append(dict(member=member_name, offset=cursor,
                               size=item_size * count, type=field_type,
                               scalar=count_expr is None, array=count_expr is not None,
                               element_size=item_size, element_count=count,
                               primitive=plain_type in scalar_sizes,
                               pointer=bool(re.fullmatch(
                                   r'(?:const\s+|volatile\s+)*(?:struct\s+)?[A-Za-z_]\w*\s*\*',
                                   field_type)),
                               nested=nested_fields is not None))
            cursor += item_size * count
            alignment = max(alignment, item_alignment)
        active.remove(name)
        stride = (cursor + alignment - 1) // alignment * alignment
        return (stride, alignment, fields)

    layout = structure_layout(typename)
    if layout is None:
        return None
    stride, _, fields = layout
    count = 1
    for expression in re.findall(r'\[([^\]]+)\]', array_dimensions):
        dimension = constant(expression)
        if dimension is None or count * dimension > 1024 * 1024:
            return None
        count *= dimension
    if not stride or stride * count != owner_size or not 0 <= offset < owner_size:
        return None
    member_offset = offset % stride
    nested_layout_proof = ({'layout_text_sha256': hashlib.sha256(
        text.encode('utf-8', 'surrogateescape')).hexdigest()}
        if any(field['nested'] for field in fields) else {})
    matches = []
    for field in fields:
        relative = member_offset - field['offset']
        if relative < 0 or relative >= field['size']:
            continue
        if relative == 0 and (not field['array'] or field['primitive']):
            matches.append(dict(field, member_element_index=None))
        elif field['array'] and field['primitive'] and relative % field['element_size'] == 0:
            matches.append(dict(field, size=field['element_size'],
                                member_element_index=relative // field['element_size']))
    if len(matches) != 1:
        return None
    field = matches[0]
    if field['scalar'] and (field['primitive'] or field['pointer']):
        return dict(kind='plain_32bit_struct_scalar_field', owner_name=owner_name,
                    owner_type=typename, owner_size=owner_size, element_stride=stride,
                    element_count=count, element_index=offset // stride,
                    owner_offset=offset, member=field['member'], offset=field['offset'],
                    size=field['size'], type=field['type'], scalar=True,
                    **nested_layout_proof)
    if field['array']:
        return dict(kind='plain_32bit_struct_array_member', owner_name=owner_name,
                    owner_type=typename, owner_size=owner_size, element_stride=stride,
                    element_count=count, element_index=offset // stride,
                    owner_offset=offset, member=field['member'], offset=field['offset'],
                    size=field['size'], type=field['type'], scalar=False,
                    member_element_count=field['element_count'],
                    member_element_index=field['member_element_index'],
                    **nested_layout_proof)
    return dict(kind='plain_32bit_struct_nested_member', owner_name=owner_name,
                owner_type=typename, owner_size=owner_size, element_stride=stride,
                element_count=count, element_index=offset // stride,
                owner_offset=offset, member=field['member'], offset=field['offset'],
                size=field['size'], type=field['type'], scalar=False,
                **nested_layout_proof)


def registered_common_storage_locations(root, tu_id, original_elf, compiler_elf, assembly_text):
    """Current compiler COMMON owners already mapped in the native MAIN tail."""
    emitted = emitted_storage(assembly_text)
    rows = []
    for section, entries in data_carve.load_registry(root).get('common_tail', {}).items():
        for entry in entries:
            if entry.get('tu') != tu_id or not entry.get('c_input_span'):
                continue
            owner = entry.get('storage_owner') or {}
            name = owner.get('name')
            symbols = [s for s in compiler_elf.symbols if s.name == name and s.shndx != 0]
            require(symbols, tu_id + '/' + str(name) + ': registered COMMON owner is absent from compiler definitions')
            lo, hi = map(data_carve.hx, entry['range'])
            alignment = owner.get('alignment')
            original = [s for s in original_elf.symbols if s.name == name and s.value == lo and
                        0 < s.shndx < len(original_elf.sections) and
                        original_elf.sections[s.shndx].name == section]
            definition = emitted.get(name) or {}
            require(tu_id.startswith('main/') and section in ('.bss', '.sbss') and
                    len(symbols) == len(original) == 1 and
                    (original[0].bind, original[0].type, original[0].size) == (1, 1, hi-lo) and
                    (symbols[0].bind, symbols[0].type, symbols[0].size) == (1, 1, hi-lo) and
                    symbols[0].shndx in (0xfff2, 0xff03) and
                    type(alignment) is int and alignment > 0 and not alignment & (alignment-1) and
                    symbols[0].value == alignment and lo % alignment == 0 and
                    definition.get('section') == 'COMMON' and definition.get('size') == hi-lo and
                    definition.get('alignment') == alignment and
                    owner.get('linkage') == 'global' and owner.get('size') == hi-lo and
                    data_carve.hx(owner.get('address', '-1')) == lo and
                    entry.get('symbols') == [name] and entry.get('c_input_span'),
                    tu_id + '/' + str(name) + ': registered COMMON identity, extent or alignment differs')
            rows.append(dict(unit='main', section=section, address=f'0x{lo:08X}', names=[name],
                             storage_owner=dict(name=name, address=f'0x{lo:08X}', size=hi-lo),
                             original_symbol_size=hi-lo))
    return rows


def require_registered_common_claims(tu_id, expected, record):
    """Every active registered owner must remain in the current proof request."""
    locations = record.get('recovered_location_candidates') or []
    for owner in expected:
        matches = [row for row in locations if all(row.get(key) == owner[key]
                   for key in ('unit', 'section', 'address', 'names', 'storage_owner'))]
        require(len(matches) == 1,
                tu_id + '/' + owner['names'][0] + ': registered COMMON owner is omitted or changed in candidate locations')


def linked_storage_proof(root, entry, record, packet, packet_tu, object_elf, assembly_text,
                         linked_elf, original_elf, original_section_ranges,
                         object_path, linked_path, packet_path, source_path, assembly_path):
    """Verify explicitly claimed BSS/SBSS storage against source, map, and link.

    Candidate metadata names an enclosing C object only for an interior alias:
    `storage_owner: {name,address,size}`. The map packet supplies the original
    location identity; NOBITS section bytes are never read or compared. Passing
    `linked_elf=None` performs only the source/object/map/relocation half and
    marks every returned object `linked=False`.
    """
    original_items = packet_tu.get('data', [])
    storage_locations = [x for x in record.get('recovered_location_candidates', [])
                         if x.get('section') in ('.bss', '.sbss')]
    emitted = emitted_storage(assembly_text)
    emitted_functions = cc1_functions(assembly_text)
    packet_by_key = {}
    for item in original_items:
        if item.get('section') in ('.bss', '.sbss'):
            for name in item.get('names', []):
                packet_by_key[(item['section'], item['address'], name)] = item
    verified, seen, aliases, storage_owner_aliases = [], set(), [], []
    for loc in storage_locations:
        section, address = loc['section'], loc['address']
        names = loc.get('names') or []
        require(names, entry['tu'] + ': storage candidate has no original map name')
        matches = [packet_by_key[(section, address, name)] for name in names
                   if (section, address, name) in packet_by_key]
        require(matches, entry['tu'] + ': storage candidate is absent from its claimed allocation packet')
        original = matches[0]
        owner = loc.get('storage_owner') or dict(name=names[0], address=address)
        owner_name = owner['name']
        owner_address = owner.get('address', address)
        owner_map_name = owner.get('map_name', owner_name)
        owner_va = int(owner_address, 16) if isinstance(owner_address, str) else int(owner_address)
        loc_va = int(address, 16)
        if loc_va != owner_va and loc.get('storage_owner') and not any(
                s.value == loc_va and s.type not in (3, 4) and
                0 < s.shndx < len(original_elf.sections) and
                original_elf.sections[s.shndx].name == section for s in original_elf.symbols):
            # A map-only child is not another recovered storage identity.
            # Its exact enclosing owner is proved by the owner's own location;
            # the declared alias is checked separately as an uncredited linker
            # dependency, including its source/member and consumer offset.
            require(any(x.get('section') == section and int(x.get('address', '0'), 16) == owner_va
                        and owner['name'] in (x.get('names') or []) for x in storage_locations),
                    entry['tu'] + '/' + names[0] + ': map-only alias has no claimed parent storage location')
            continue
        owner_map = [item for item in original_items if item.get('section') == section
                     and int(item.get('address', '0'), 16) == owner_va
                     and owner_map_name in item.get('names', [])]
        if not owner_map and owner_va == loc_va:
            # A C name may replace an address-labelled original symbol. The
            # claimed location itself must still be present in the packet.
            owner_map = [item for item in matches if int(item.get('address', '0'), 16) == owner_va]
        require(owner_map, entry['tu'] + '/' + owner_name + ': storage owner is absent from the frozen original map')
        asm = emitted.get(owner_name)
        require(asm is not None, entry['tu'] + '/' + owner_name + ': no cc1 storage definition outside #APP')
        owner_symbols = [s for s in object_elf.symbols if s.name == owner_name and
                         (s.type == 1 or (s.type == 0 and s.bind == 0 and
                                          asm.get('extent_kind') == 'label-space'))
                         and (s.shndx in (0xff03, 0xfff2) or
                              (0 < s.shndx < len(object_elf.sections) and
                               object_elf.sections[s.shndx].type == 8 and
                               object_elf.sections[s.shndx].flags & 3 == 3))]
        require(len(owner_symbols) == 1, entry['tu'] + '/' + owner_name + ': compiler object has no unique BSS/COMMON object')
        symbol = owner_symbols[0]
        carve_allocation = None
        if symbol.shndx in (0xff03, 0xfff2):
            require(asm['section'] in ('COMMON', '.bss'),
                    entry['tu'] + '/' + owner_name + ': COMMON directive disagrees with ELF symbol class')
            alignment = symbol.value
            require(alignment > 0 and alignment & (alignment - 1) == 0,
                    entry['tu'] + '/' + owner_name + ': invalid COMMON alignment')
            if 'alignment' in asm:
                require(alignment == asm['alignment'],
                        entry['tu'] + '/' + owner_name + ': assembler/object COMMON alignment differs')
        else:
            section_obj = object_elf.sections[symbol.shndx]
            if asm['section'] == 'COMMON':
                # The private allocation link uses ld -r -d: compiler COMMON
                # symbols become allocated .bss/.sbss/.scommon objects here.
                if (record.get('unit') == 'main' and object_path.name.endswith('.carved.o') and
                        (section_obj.name == section or section_obj.name.startswith(section + '.carve.'))):
                    proof_size = owner.get('size', symbol.size or asm.get('size', 0))
                    proof_size = int(proof_size, 0) if isinstance(proof_size, str) else int(proof_size)
                    carve_allocation = allocated_carve_mapping(
                        entry, record, owner_name, owner_va, proof_size, section, object_path, section_obj, symbol,
                        source_path, Path(record['object']['path']), assembly_path)
                require((section_obj.name in ('.bss', '.sbss', '.scommon') or carve_allocation is not None) and
                        section_obj.type == 8 and section_obj.flags & 3 == 3,
                        entry['tu'] + '/' + owner_name + ': COMMON was not allocated as writable NOBITS')
                require(section_obj.align >= asm.get('alignment', 1),
                        entry['tu'] + '/' + owner_name + ': allocated COMMON alignment is weaker than cc1 request')
            else:
                if asm['section'] != section_obj.name:
                    proof_size = owner.get('size', symbol.size or asm.get('size', 0))
                    proof_size = int(proof_size, 0) if isinstance(proof_size, str) else int(proof_size)
                    carve_allocation = allocated_carve_mapping(
                        entry, record, owner_name, owner_va, proof_size, asm['section'],
                        object_path, section_obj, symbol, source_path,
                        Path(record['object']['path']), assembly_path)
                if asm.get('extent_kind') == 'label-space':
                    # A carved output section may have weaker section alignment
                    # than the raw input section because its piece starts at an
                    # aligned nonzero input offset. The exact allocator/carve
                    # mapping proves that input offset; additionally require
                    # the final mapped VA itself to satisfy cc1's alignment.
                    mapped_alignment = (carve_allocation is not None or
                                        section_obj.align >= asm.get('alignment', 1))
                    symbol_alignment = (owner_va % asm.get('alignment', 1) == 0 and
                                        (carve_allocation is not None or
                                         symbol.value % asm.get('alignment', 1) == 0))
                    require(asm.get('size', 0) > 0 and mapped_alignment and symbol_alignment,
                            entry['tu'] + '/' + owner_name + ': cc1 label/space extent or alignment differs from allocated object')
            alignment = section_obj.align
        # Old cc1 records a TU-local tentative object as a LOCAL NOTYPE label
        # with st_size zero even though its ordinary BSS section contains the
        # exact `.space` extent.  Use that independent compiler directive
        # extent as the default owner bound; an explicit frozen owner size is
        # still checked against it below.
        size = owner.get('size', symbol.size or asm.get('size', 0))
        size = int(size, 0) if isinstance(size, str) else int(size)
        exact_symbol_extent = symbol.size == size
        exact_local_space_extent = (symbol.size == 0 and symbol.type == 0 and symbol.bind == 0
                                    and asm.get('extent_kind') == 'label-space'
                                    and asm.get('size') == size and asm.get('section') == section)
        require((exact_symbol_extent or exact_local_space_extent) and size > 0,
                entry['tu'] + '/' + owner_name + ': compiler object storage extent differs from frozen owner')
        require(owner_va <= loc_va < owner_va + size,
                entry['tu'] + '/' + owner_name + ': original label lies outside the C storage owner')
        owner_end = owner_va + size
        mapped_range = original_section_ranges.get(section)
        if not (mapped_range and mapped_range[0] <= owner_va < owner_end <= mapped_range[1]):
            compiler_path = Path((record.get('compiler_object') or {}).get('path', '/nonexistent'))
            require(compiler_path.is_file() and
                    sha(compiler_path) == (record.get('compiler_object') or {}).get('sha256'),
                    entry['tu'] + '/' + owner_name + ': common-tail compiler pin differs')
            common_locations = registered_common_storage_locations(
                root, entry['tu'], original_elf, Elf(compiler_path.read_bytes()), assembly_text)
            exact = [row for row in common_locations if row['section'] == section and
                     int(row['address'], 16) == owner_va and row['names'] == [owner_name] and
                     row['storage_owner']['size'] == size]
            require(len(exact) == 1 and carve_allocation is not None,
                    entry['tu'] + '/' + owner_name + ': out-of-TU storage lacks a registered native COMMON carve')
            mapped_range = (owner_va, owner_end)
        require(mapped_range and mapped_range[0] <= owner_va < owner_end <= mapped_range[1],
                entry['tu'] + '/' + owner_name + ': C storage extent is outside the original TU section map')
        original_identities = [s for s in original_elf.symbols
                               if s.shndx not in (0, 0xfff1) and s.value == owner_va
                               and mapped_range[0] <= s.value < mapped_range[1]]
        for original_symbol in original_identities:
            if original_symbol.type == 1 and original_symbol.size > 0:
                require((symbol.bind, symbol.type, size) ==
                        (original_symbol.bind, original_symbol.type, original_symbol.size),
                        entry['tu'] + '/' + owner_name + ': C storage binding/type/size differs from meaningful original OBJECT at the same address (' + original_symbol.name + ')')
            else:
                require(symbol.bind == original_symbol.bind,
                        entry['tu'] + '/' + owner_name + ': C storage binding differs from original zero-size symbol at the same address (' + original_symbol.name + ')')
        item_size = original.get('original_symbol_size')
        if item_size:
            require(loc_va + int(item_size) <= owner_end,
                    entry['tu'] + '/' + names[0] + ': C storage owner truncates the original mapped item span')
        link_verified = False
        if linked_elf is not None:
            linked = [s for s in linked_elf.symbols if s.name == owner_name and s.type == symbol.type
                      and s.value == owner_va and (s.size == size or exact_local_space_extent and s.size == 0)
                      and s.bind == symbol.bind
                      and s.shndx not in (0, 0xfff1)]
            require(len(linked) == 1, entry['tu'] + '/' + owner_name + ': linked symbol name/address/size/binding differs')
            linked_sec = linked_elf.sections[linked[0].shndx]
            if entry.get('tu', '').startswith('main/'):
                require(linked_sec.name == section and linked_sec.type == 8 and linked_sec.flags & 3 == 3,
                        entry['tu'] + '/' + owner_name + ': linked storage section is not mapped writable NOBITS')
            else:
                # Overlay link images place BSS/SBSS addresses in the single
                # writable PROGBITS overlay section in some flows, while other
                # flows keep a unit-specific writable NOBITS section. The input
                # object's NOBITS provenance and exact final symbol placement
                # remain the proof; image zeros do not participate.
                unit = record['unit']
                image_section = linked_sec.name == unit and linked_sec.type == 1
                bss_section = linked_sec.name == f'.{unit}_bss' and linked_sec.type == 8
                require((image_section or bss_section) and linked_sec.flags & 3 == 3,
                        entry['tu'] + '/' + owner_name + ': overlay storage is not in its writable allocated image/BSS section')
            link_verified = True
        cc1_refs = storage_relocations(object_elf, symbol.index, emitted_functions)
        refs = cc1_refs
        offset = loc_va - owner_va
        if not any(ref['addend'] == offset for ref in refs):
            # Storage may be intentionally consumed only from preserved
            # INCLUDE_ASM in this same original TU. Preserve that code while
            # still requiring an object-code relocation to the exact alias
            # offset; the source/cc1 storage and final linked identity proofs
            # remain mandatory.
            refs = storage_relocations(object_elf, symbol.index, None)
        has_offset_relocation=any(ref['addend'] == offset for ref in refs)
        # An INCLUDE_ASM consumer can address C-defined storage without a
        # retained relocation (the assembler or guarded partial link resolves
        # same-object references). The exact source/cc1 definition, allocated
        # section/map span, final linked symbol identity and full-file pass
        # still prove ownership; never inspect NOBITS contents. Before the
        # final link this exception remains explicitly unverified/partial.
        if not has_offset_relocation and linked_elf is None:
            relocation_status='no_retained_relocation_link_check_pending'
        elif not has_offset_relocation:
            relocation_status='no_retained_relocation_exact_linked_identity'
        else:
            relocation_status=('cc1_function_relocation' if any(
                ref['addend']==offset and ref.get('source_kind')=='cc1' for ref in refs)
                               else 'same_tu_object_relocation')
        if loc_va != owner_va and loc.get('storage_owner'):
            alias_extent = loc.get('alias_extent', item_size)
            alias_extent = int(alias_extent, 0) if isinstance(alias_extent, str) else alias_extent
            field_proof = None
            if not alias_extent or loc.get('alias_field'):
                field_proof = source_storage_member_extent(
                    source_storage_layout_text(root, source_path), owner_name, size, offset)
                require(field_proof is not None,
                        entry['tu'] + '/' + names[0] + ': interior storage alias has no exact source field extent')
                require(not alias_extent or alias_extent == field_proof['size'],
                        entry['tu'] + '/' + names[0] + ': frozen alias extent differs from current source field')
                require(not loc.get('alias_field') or loc['alias_field'] == field_proof,
                        entry['tu'] + '/' + names[0] + ': frozen alias field differs from current source layout')
                alias_extent = field_proof['size']
            require(alias_extent and alias_extent > 0 and loc_va + alias_extent <= owner_end,
                    entry['tu'] + '/' + names[0] + ': interior storage alias lacks a bounded exact extent')
            if original.get('mapped_extent') is not None:
                require(alias_extent <= int(original['mapped_extent']),
                        entry['tu'] + '/' + names[0] + ': source field exceeds its exact original mapped item')
            if item_size:
                require(alias_extent <= int(item_size),
                        entry['tu'] + '/' + names[0] + ': interior alias exceeds its original packet item extent')
            alias_originals = [s for s in original_elf.symbols
                               if s.shndx not in (0, 0xfff1) and s.value == loc_va
                               and mapped_range[0] <= s.value < mapped_range[1]]
            meaningful_aliases = [s for s in alias_originals if s.type == 1 and s.size > 0]
            for original_alias in meaningful_aliases:
                exact_alias = [s for s in object_elf.symbols if s.value == symbol.value + offset
                               and s.type == original_alias.type and s.bind == original_alias.bind
                               and s.size == original_alias.size]
                require(len(exact_alias) == 1,
                        entry['tu'] + '/' + names[0] + ': meaningful original interior OBJECT lacks its exact C identity')
                linked_alias = ([s for s in linked_elf.symbols
                                 if s.name == exact_alias[0].name and s.value == loc_va
                                 and s.type == original_alias.type and s.bind == original_alias.bind
                                 and s.size == original_alias.size and s.shndx not in (0, 0xfff1)]
                                if linked_elf is not None else [])
                require(linked_elf is not None and len(linked_alias) == 1,
                        entry['tu'] + '/' + names[0] + ': meaningful original interior OBJECT lacks its exact linked C identity')
            storage_owner_aliases.append(dict(section=section, scaffold_names=names,
                                              address=f'0x{loc_va:08X}',
                                              mapped_span=[loc_va, loc_va + alias_extent],
                                              original_identities=[dict(name=s.name, binding=s.bind,
                                                                        type=s.type, size=s.size)
                                                                   for s in alias_originals],
                                              alias_extent=alias_extent, owner_name=owner_name,
                                              owner_address=f'0x{owner_va:08X}', owner_size=size,
                                              owner_offset=offset, c_object_section=section,
                                              c_object_offset=symbol.value,
                                              owner_binding=('LOCAL' if symbol.bind == 0 else
                                                             'GLOBAL' if symbol.bind == 1 else str(symbol.bind)),
                                              owner_type=('OBJECT' if symbol.type == 1 else 'NOTYPE'),
                                              owner_object_sha256=sha(object_path),
                                              linked_elf_sha256=sha(linked_path) if linked_elf is not None else None,
                                              packet_sha256=sha(packet_path),
                                              source_sha256=sha(source_path),
                                              assembly_sha256=sha(assembly_path),
                                              **({'source_field':field_proof} if field_proof else {}),
                                              linked=link_verified, relocation_count=len(refs),
                                              relocation_status=relocation_status))
        key = (owner_name, owner_va, size)
        # Preserve an exact same-address packet-label-to-C-owner identity for
        # the TU audit consumer. Interior aliases are already represented by
        # their storage_owner span and are not misreported as ELF symbols.
        if loc_va == owner_va:
            for scaffold_name in names:
                if scaffold_name == owner_name:
                    continue
                aliases.append(dict(section=section,scaffold_name=scaffold_name,
                                    object_symbol=owner_name,address=f'0x{loc_va:08X}',
                                    size=size,binding=('LOCAL' if symbol.bind==0 else
                                                       'GLOBAL' if symbol.bind==1 else str(symbol.bind)),
                                    type=('OBJECT' if symbol.type==1 else 'NOTYPE')))
        if key not in seen:
            seen.add(key)
            verified_row = dict(name=owner_name, address=f'0x{owner_va:08X}', size=size,
                                section=section, bind=symbol.bind,
                                type=('OBJECT' if symbol.type == 1 else 'NOTYPE'),alignment=alignment,
                                object_sha256=sha(object_path),
                                tu_section_range=[f'0x{mapped_range[0]:08X}',f'0x{mapped_range[1]:08X}'],
                                original_identities=[dict(name=s.name,binding=s.bind,type=s.type,size=s.size)
                                                     for s in original_identities],
                                relocation_count=len(refs), relocs=refs,
                                relocation_status=relocation_status,linked=link_verified)
            if carve_allocation is not None:
                verified_row['allocated_carve_mapping'] = carve_allocation
            verified.append(verified_row)
    claimed = {(x['section'], x['address'], name)
               for x in storage_locations for name in x.get('names', [])}
    owner_spans = [(x['section'], int(x['address'], 16), int(x['address'], 16) + x['size'])
                   for x in verified]
    uncovered = [dict(section=x.get('section'), address=x.get('address'), names=x.get('names', []))
                 for x in original_items if x.get('section') in ('.bss', '.sbss')
                 and any((x.get('section'), x.get('address'), n) not in claimed and
                         not any(sec == x.get('section') and
                                 int(x.get('address', '0'), 16) <= va < end
                                 for sec, va, end in owner_spans)
                         for n in x.get('names', []))]
    return verified, uncovered, aliases, storage_owner_aliases


def allocation_packet(root, task, allocation_root=None):
    """Resolve the immutable block packet named by a candidate's allocation task."""
    match = re.fullmatch(r'data-recovery-(\d{8})-(\d+)', task or '')
    require(match is not None, f'unknown allocation task identity: {task!r}')
    campaign, block = match[1], int(match[2])
    packet_root = Path(allocation_root) if allocation_root else CANONICAL
    path = packet_root / '.work' / f'data-recovery-{campaign}' / f'block-{block:02d}' / 'packet.json'
    require(path.is_file(), f'missing allocation packet: {path}')
    packet = json.loads(path.read_text())
    return packet, path


def owner_corrected_items(root, unit, ctx, tu_id, section, address):
    """Original map items of a verified owner-corrected run that holds `address`.

    A verified owner correction (config/tu/data-carves.json basis.owner_correction)
    moves an original item from its provisional map TU to this source TU. Its
    mapped identity then lives in the map TU's pieces; only items wholly inside
    the corrected run are returned (main/tu091 menutbl, 2026-10-06)."""
    out = []
    for run in (data_carve.load_registry(root)['tus'].get(tu_id) or {}).get(section, []):
        correction = (run.get('basis') or {}).get('owner_correction') or {}
        lo, hi = (data_carve.hx(v) for v in run['range'])
        if not correction.get('map_tu') or not lo <= address < hi:
            continue
        map_ctx = data_carve.tu_context(root, unit, Path(ctx['orig']).parents[1], correction['map_tu'])
        out += [item for item in data_carve.section_items(map_ctx, section)
                if lo <= item['start'] and item['end'] <= hi]
    return out


def current_allocation_scope(control_path, task, entry, record, ctx, original_elf, root=None):
    """Validate a current takeover allocation without inventing a worker packet.

    The coordinator's data-recovery-allocation/1 control file is the authority
    for this takeover.  Its exact (TU, address, names) identities are checked
    against both the frozen candidate record and the original TU's mapped
    section items.  The returned in-memory view contains only those scoped
    identities; it is not serialized or represented as a historical packet.
    """
    control_path = Path(control_path).resolve()
    require(control_path.is_file(), 'missing current allocation control file')
    control = json.loads(control_path.read_text())
    require(control.get('schema') == 'data-recovery-allocation/1' and
            control.get('task_id') == task,
            'current allocation control schema/task identity differs')
    require(entry['tu'] in control.get('tus', []),
            entry['tu'] + ': absent from current allocation control TU set')

    def identity_key(row):
        return (row.get('unit'), row.get('tu', entry['tu']), row.get('address'),
                tuple(row.get('names', [])))

    scoped = [row for row in control.get('identities', [])
              if row.get('tu') == entry['tu']]
    require(scoped, entry['tu'] + ': no identities in current allocation control')
    unit = record.get('unit')
    require(unit in ('main', 'ov01', 'ov02', 'ov10', 'ov11', 'ov12'),
            entry['tu'] + ': candidate has no supported unit identity')
    record_rows = [row for row in record.get('recovered_location_candidates', [])
                   if row.get('unit') == unit]
    control_keys = [identity_key(row) for row in scoped]
    record_keys = [identity_key(row) for row in record_rows]
    require(len(control_keys) == len(set(control_keys)) and
            set(control_keys) == set(record_keys),
            entry['tu'] + ': candidate record identities differ from current allocation control')

    data = []
    for identity in scoped:
        key = identity_key(identity)
        candidate = next(row for row in record_rows if identity_key(row) == key)
        section = candidate.get('section')
        require(section in data_carve.SECTIONS,
                entry['tu'] + ': current allocation identity has an unsupported section')
        address = int(identity['address'], 0)
        names = list(identity['names'])
        original_size = None
        matching_symbols = [symbol for symbol in original_elf.symbols
                            if symbol.name in names and symbol.value == address and
                            0 < symbol.shndx < len(original_elf.sections) and
                            original_elf.sections[symbol.shndx].name == section]
        require(len(matching_symbols) <= 1,
                entry['tu'] + ': current allocation identity has ambiguous original ELF symbols')
        if matching_symbols:
            original_size = matching_symbols[0].size
        map_items = [item for item in data_carve.section_items(ctx, section)
                     if item['start'] == address and
                     set(names).intersection(item.get('labels', [item.get('name')]))]
        if (not map_items and root is not None and len(matching_symbols) == 1 and
                matching_symbols[0].bind == 0 and matching_symbols[0].type == 1 and matching_symbols[0].size > 0):
            registered = data_carve.registered_object_items(
                root, unit, original_elf, matching_symbols[0],
                {section: data_carve.section_items(ctx, section)})
            registered = [row for row in registered if row[1]['start'] == address]
            if (len(registered) == 1 and candidate.get('original_map_aliases') == registered[0][2]):
                map_items = [registered[0][1]]
        if not map_items and root is not None and section not in ('.bss', '.sbss'):
            # A verified owner correction (config/tu/data-carves.json) moves an
            # original item from its provisional map TU to this source TU; the
            # mapped identity is then found in the map TU's pieces, and only
            # inside the corrected run (main/tu091 menutbl, 2026-10-06).
            for run in (data_carve.load_registry(root)['tus'].get(entry['tu']) or {}).get(section, []):
                correction = (run.get('basis') or {}).get('owner_correction') or {}
                lo, hi = (data_carve.hx(v) for v in run['range'])
                if not correction.get('map_tu') or not lo <= address < hi:
                    continue
                map_ctx = data_carve.tu_context(root, unit, Path(ctx['orig']).parents[1], correction['map_tu'])
                corrected = [item for item in data_carve.section_items(map_ctx, section)
                             if item['start'] == address and item['end'] <= hi and
                             set(names).intersection(item.get('labels', [item.get('name')]))]
                if len(corrected) == 1:
                    map_items = corrected
        if section not in ('.bss', '.sbss'):
            require(len(map_items) == 1,
                    entry['tu'] + ': current allocation identity is not one exact original mapped item')
        else:
            require(len(map_items) <= 1 and (matching_symbols or map_items),
                    entry['tu'] + ': storage identity has no unique original symbol or mapped item')
            original_names = {symbol.name for symbol in original_elf.symbols
                              if symbol.value == address and 0 < symbol.shndx < len(original_elf.sections)
                              and original_elf.sections[symbol.shndx].name == section}
            mapped_names = set(map_items[0].get('labels', [])) if map_items else set()
            require(set(names) <= original_names | mapped_names,
                    entry['tu'] + ': storage names contain an unproven original alias')
        data.append(dict(unit=identity['unit'], address=identity['address'],
                         section=section, names=names,
                         owner_tu=entry['tu'], original_symbol_size=original_size,
                         **({'mapped_extent':map_items[0]['end']-map_items[0]['start']} if map_items else {}),
                         allocation_classification=identity.get('classification')))
    return dict(id=entry['tu'], unit=unit, data=data), control_path, control


def validate_main_common_tail_storage(root, storage_manifest, candidates, gate, gate_path):
    """Validate and return exact MAIN common-tail storage rows.

    The frozen worker packets predate the linker common-tail allocation.  This
    narrow sidecar binds those map items to the independently pinned r6
    raw->allocated->selected objects, actual retail ELF, current full link map,
    linked ELF, and exact whole-file gate.  It only supplies packet-shaped map
    items in memory; it never edits an immutable packet or grants credit to
    residual scaffold storage.
    """
    proof = storage_manifest.get('main_common_tail')
    if proof is None:
        return {}
    require(proof.get('schema') == 'main-common-tail-linked-storage/1',
            'unsupported MAIN common-tail storage proof schema')

    def pinned(path_key, hash_key, parse_json=True):
        path = Path(proof[path_key])
        require(path.is_file() and sha(path) == proof[hash_key],
                'MAIN common-tail input pin differs: ' + path_key)
        return path, json.loads(path.read_text()) if parse_json else None

    schema_path, schema = pinned('schema_path', 'schema_sha256')
    allocation_path, allocation = pinned('allocation_path', 'allocation_sha256')
    placement_path, placement = pinned('placement_path', 'placement_sha256')
    stage_path, stage = pinned('stage_manifest_path', 'stage_manifest_sha256')
    registry_path, registry = pinned('registry_path', 'registry_sha256')
    map_path, _ = pinned('map_path', 'map_sha256', False)
    linked_path, _ = pinned('linked_elf_path', 'linked_elf_sha256', False)
    compare_path, compare = pinned('compare_path', 'compare_sha256')
    require(gate is not None and gate.get('whole_file_identical') is True and
            gate.get('diff_range_count') == 0 and
            gate.get('original_sha256') == gate.get('rebuilt_sha256'),
            'MAIN common-tail proof lacks the current exact whole-file gate')
    require(Path(compare_path).resolve() == Path(gate_path).resolve() and
            sha(linked_path) == proof['linked_elf_sha256'] and
            sha(map_path) == proof['map_sha256'],
            'MAIN common-tail gate/map/ELF paths are not the same current stage')
    require(schema.get('schema') == 'main-common-tail-carves/1' and
            allocation.get('status') == 'pass' and placement.get('status') == 'pass',
            'MAIN common-tail schema or allocation/placement report is not passing')
    require(allocation.get('stage_manifest', {}).get('sha256') == sha(stage_path) and
            placement.get('stage_manifest', {}).get('sha256') == sha(stage_path),
            'MAIN common-tail reports are not pinned to the same stage manifest')
    require(allocation.get('registry_input', {}).get('sha256') == sha(registry_path) and
            placement.get('registry', {}).get('sha256') == sha(registry_path),
            'MAIN common-tail reports are not pinned to the active stage registry')
    require(stage.get('native_build', {}).get('linked_elf', {}).get('sha256') == sha(linked_path) and
            stage.get('native_build', {}).get('link_map', {}).get('sha256') == sha(map_path) and
            stage.get('native_build', {}).get('comparison', {}).get('sha256') == sha(compare_path),
            'MAIN common-tail files do not match the current stage manifest')
    require(placement.get('linked_elf', {}).get('sha256') == sha(linked_path) and
            placement.get('map', {}).get('sha256') == sha(map_path) and
            placement.get('whole_file_compare', {}).get('sha256') == sha(compare_path),
            'MAIN common-tail placement report is stale')

    allocation_rows = allocation.get('rows', [])
    placements = schema.get('placements', [])
    placed_owners = placement.get('owners', [])
    require(len(allocation_rows) == 56 and len(placed_owners) == 56 and
            len(storage_manifest['recovered_location_candidates']) == 56,
            'MAIN common-tail candidate set is not the exact 56-row group')
    by_key = {}
    for row in allocation_rows:
        owner = row['storage_owner']
        by_key[(row['tu'], row['section'], row['target_range'][0], owner['name'])] = row
    placement_by_key = {(row['tu'], row['section'], row['address'], row['name']): row
                        for row in placed_owners}
    schema_by_key = {}
    for row in placements:
        mi = row['evidence']['map_item']
        schema_by_key[(row['tu'], row['section'], mi['range'][0], mi['name'])] = row
    stage_inputs = stage['native_build']['candidate_tu_inputs']
    required_tus = {entry['tu'] for entry in candidates}
    validated = {}
    retail = Elf(Path(placement['retail_original_elf']['path']).read_bytes())
    linked = Elf(linked_path.read_bytes())
    for side_row in storage_manifest['recovered_location_candidates']:
        ev = side_row.get('evidence', {})
        tu = ev.get('original_tu')
        owner = side_row.get('storage_owner', {})
        address = side_row.get('address')
        key = (tu, side_row.get('section'), address, owner.get('name'))
        ar = by_key.get(key)
        pr = placement_by_key.get(key)
        sr = schema_by_key.get(key)
        require(ar is not None and pr is not None and sr is not None,
                str(key) + ': no exact r6 allocation/schema/placement row')
        require(tu in required_tus and side_row['names'] == sr['evidence']['map_item']['labels'] and
                owner.get('address') == ar['storage_owner']['address'] and
                owner.get('size') == ar['storage_owner']['size'] and
                int(address, 16) == int(ar['target_range'][0], 16) and
                int(ar['target_range'][1], 16) - int(ar['target_range'][0], 16) == owner.get('size'),
                tu + '/' + owner.get('name', '?') + ': exact map item or owner extent differs')
        inp = stage_inputs[tu]
        checks = [
            (inp['source_path'], inp['source_sha256'], ar['source']['path'], ar['source']['sha256']),
            (inp['compiler_raw_object_path'], inp['compiler_raw_object_sha256'], ar['raw_object']['path'], ar['raw_object']['sha256']),
            (inp['cc1_assembly_path'], inp['cc1_assembly_sha256'], ar['cc1']['path'], ar['cc1']['sha256']),
            (inp['allocated_object_path'], inp['allocated_object_sha256'], ar['allocated_object']['path'], ar['allocated_object']['sha256']),
            (inp['c_link_object_path'], inp['c_link_object_sha256'], ar['selected_object']['path'], ar['selected_object']['sha256'])]
        for p1, h1, p2, h2 in checks:
            require(Path(p1).resolve() == Path(p2).resolve() and h1 == h2 and sha(Path(p1)) == h1,
                    tu + '/' + owner['name'] + ': current compiler object chain pin differs')
        require(ev.get('source_sha256') == inp['source_sha256'] and
                ev.get('object_sha256') == inp['compiler_raw_object_sha256'] and
                ev.get('assembly_sha256') == inp['cc1_assembly_sha256'],
                tu + '/' + owner['name'] + ': sidecar raw source/object/cc1 pin differs')
        require(ar['retail_original_elf']['sha256'] == sha(Path(placement['retail_original_elf']['path'])) and
                ar['linked_placement']['linked_elf_sha256'] == sha(linked_path) and
                ar['linked_placement']['map_sha256'] == sha(map_path) and
                ar['linked_placement']['map_object'] == pr['map_object'] and
                ar['linked_placement']['map_start'] == ar['target_range'][0] and
                ar['linked_placement']['map_size'] == owner['size'] and
                pr['map_start'] == ar['target_range'][0] and pr['map_size'] == owner['size'],
                tu + '/' + owner['name'] + ': original or linked placement hash differs')
        size = owner['size']; va = int(owner['address'], 16)
        raw = Elf(Path(inp['compiler_raw_object_path']).read_bytes())
        allocated = Elf(Path(inp['allocated_object_path']).read_bytes())
        raw_sym = [s for s in raw.symbols if s.name == owner['name']]
        alloc_sym = [s for s in allocated.symbols if s.name == owner['name']]
        original_sym = [s for s in retail.symbols if s.name == owner['name'] and s.value == va]
        linked_sym = [s for s in linked.symbols if s.name == owner['name'] and s.value == va]
        require(len(raw_sym) == len(alloc_sym) == len(original_sym) == len(linked_sym) == 1,
                tu + '/' + owner['name'] + ': raw/allocated/retail/linked identity is not unique')
        rs, ass, os, ls = raw_sym[0], alloc_sym[0], original_sym[0], linked_sym[0]
        require(rs.shndx in (0xfff2, 0xff03) and rs.size == size and rs.type == 1 and
                rs.bind == 1 and ass.type == 1 and ass.bind == 1 and ass.size == size and
                ass.shndx < len(allocated.sections) and allocated.sections[ass.shndx].type == 8 and
                os.type == os.bind == 1 and os.size == size and
                ls.type == ls.bind == 1 and ls.size == size and ls.shndx < len(linked.sections),
                tu + '/' + owner['name'] + ': raw COMMON, allocated NOBITS, retail or linked identity differs')
        require(rs.value == ar['raw_symbol']['value'] and ass.size == ar['allocated_symbol']['size'] and
                ar['allocated_symbol']['section_type'] == 8 and
                (os.value, os.size, os.bind, os.type) ==
                (va, size, ar['retail_original_symbol']['bind'], ar['retail_original_symbol']['type']) and
                (ls.value, ls.size, ls.bind, ls.type) ==
                (va, size, pr['linked_symbol']['bind'], pr['linked_symbol']['type']),
                tu + '/' + owner['name'] + ': symbol attributes disagree with r6 evidence')
        validated.setdefault(tu, []).append(dict(section=side_row['section'], address=address,
            names=side_row['names'], original_symbol_size=size, owner=owner['name'],
            owner_size=size, target_range=ar['target_range']))
    require(sum(map(len, validated.values())) == 56 and required_tus == set(validated),
            'MAIN common-tail rows do not cover exactly the eight selected TUs')
    return validated


def mapped_section_ranges(root, unit, tu_id):
    """Exact original TU section spans from the checked-in object map."""
    if unit == 'main':
        objects = json.loads((Path(root) / 'config/objects/main.objects.json').read_text())
        tu = next((x for x in objects['tus'] if x['id'] == tu_id), None)
        require(tu is not None, f'{tu_id}: missing original TU map')
        return {name: (int(section['start'], 16), int(section['end'], 16))
                for name, section in tu.get('sections', {}).items()}
    stem = data_carve.ovl_tu_names(root, unit).get(tu_id)
    require(stem is not None, f'{tu_id}: missing overlay TU name mapping')
    layout = json.loads((Path(root) / 'config/objects' / f'{unit}.layout.json').read_text())
    out = {}
    for section, runs in layout['families'].items():
        for tu, lo, hi in runs:
            if tu == f'{unit}/{stem}':
                out[section] = (int(lo, 16), int(hi, 16))
    return out


def validate_auxiliary_allocation(root, record, entry, allocation_path, packet_tu,
                                 source, obj_path, asm_path, original_elf,
                                 original_section_ranges):
    """Load coordinator-authorized auxiliary locations without editing the packet.

    These rows must be in a separately pinned sidecar referenced by the frozen
    candidate record. They are checked against the original TU map, scaffold
    declarations, reference ELF, caller evidence, and current compiler inputs.
    They never alter the original packet or its denominator.
    """
    ref = record.get('supplementary_allocation')
    if not ref:
        return packet_tu, []
    path = Path(ref['path'])
    if not path.is_absolute():
        path = Path(root) / path
    require(path.is_file() and sha(path) == ref['sha256'],
            entry['tu'] + ': supplementary allocation sidecar hash differs')
    side = json.loads(path.read_text())
    require(side.get('schema') == 'supplementary-data-allocation/1',
            entry['tu'] + ': unsupported supplementary allocation schema')
    require(side.get('owner_tu') == entry['tu'],
            entry['tu'] + ': supplementary allocation belongs to another TU')
    require(side.get('coordinator_authorization') and
            side.get('packet_context', {}).get('supplemental_addresses_are_excluded_from_original_denominator') is True,
            entry['tu'] + ': supplementary allocation lacks authorization or denominator boundary')
    packet_ctx = side['packet_context']
    require(Path(packet_ctx['immutable_original_packet']).resolve() == allocation_path.resolve() and
            packet_ctx['sha256'] == sha(allocation_path),
            entry['tu'] + ': supplementary allocation is not pinned to this immutable packet')
    section_map = side['original_section_map']
    section = section_map['section']
    mapped = original_section_ranges.get(section)
    start, end = int(section_map['source_range']['start'], 16), int(section_map['source_range']['end'], 16)
    require(mapped and mapped[0] <= start < end <= mapped[1],
            entry['tu'] + ': supplementary section range escapes original TU map')
    for key in ('scaffold_path', 'reference_elf'):
        evidence_path = Path(section_map[key])
        require(evidence_path.is_file() and sha(evidence_path) == section_map['scaffold_sha256' if key == 'scaffold_path' else 'reference_elf_sha256'],
                entry['tu'] + ': supplementary ' + key + ' hash differs')
    caller = side['caller_proof']
    caller_path = Path(caller['path'])
    require(caller_path.is_file() and sha(caller_path) == caller['sha256'],
            entry['tu'] + ': supplementary caller proof hash differs')
    candidate = side['compiler_candidate']
    require(candidate['source_sha256'] == sha(source) and
            candidate['object_sha256'] == sha(obj_path) and
            candidate['raw_cc1_assembly_sha256'] == sha(asm_path),
            entry['tu'] + ': supplementary compiler input hashes differ')
    reference = Elf(Path(section_map['reference_elf']).read_bytes())
    for item in side.get('reference_elf_symbol_evidence', []):
        addr = int(item['address'], 16)
        bind = {'LOCAL': 0, 'GLOBAL': 1, 'WEAK': 2}.get(item['binding'])
        stype = {'NOTYPE': 0, 'OBJECT': 1, 'FUNC': 2}.get(item['type'])
        found = [s for s in reference.symbols if s.name == item['name'] and s.value == addr
                 and s.size == item['size'] and (bind is None or s.bind == bind)
                 and (stype is None or s.type == stype) and s.shndx == item['section_index']]
        require(len(found) == 1, entry['tu'] + '/' + item['name'] +
                ': supplementary reference ELF symbol pin differs')
    scaffold = Path(section_map['scaffold_path']).read_text(errors='surrogateescape')
    declarations = {name: int(size, 16) for name, size in
                    re.findall(r'^\s*nonmatching\s+(\S+)\s*,\s*(0x[\da-fA-F]+)', scaffold, re.M)}
    scaffold_lines=scaffold.splitlines()
    # Some data scaffold items spell their extent as `nonmatching NAME`, a
    # `dlabel`/`enddlabel` pair, and GNU `.size NAME, . - NAME`. Derive the
    # extent from only the enclosed size-bearing directives; do not accept the
    # symbol name or sidecar's claimed size by itself.
    for declaration in re.finditer(r'^\s*nonmatching\s+([A-Za-z_.$][\w.$]*)\s*(?:#.*)?$',
                                   scaffold,re.M):
        name=declaration.group(1)
        if name in declarations:
            continue
        dlabel=next((i for i,line in enumerate(scaffold_lines)
                     if re.fullmatch(r'\s*dlabel\s+'+re.escape(name)+r'\s*(?:#.*)?',line)),None)
        enddlabel=next((i for i,line in enumerate(scaffold_lines)
                        if i>(dlabel if dlabel is not None else -1) and
                        re.fullmatch(r'\s*enddlabel(?:\s+'+re.escape(name)+r')?\s*(?:#.*)?',line)),None)
        size_matches=[m for m in re.finditer(
            r'^\s*\.size\s+'+re.escape(name)+r'\s*,\s*\.\s*-\s*'+re.escape(name)+r'\s*(?:#.*)?$',
            scaffold,re.M)]
        if dlabel is None or enddlabel is None or len(size_matches)!=1:
            continue
        extent=0
        supported=True
        for line in scaffold_lines[dlabel+1:enddlabel]:
            raw=line.split('#',1)[0].strip()
            raw=re.sub(r'^/\*.*?\*/\s*','',raw)
            if not raw or raw.endswith(':') or raw.startswith(('.type ','.globl ','.global ','.local ')):
                continue
            match=re.match(r'\.(word|4byte|long|byte|2byte|half|short|space|skip|align|p2align|ascii|asciz|string)\s+(.+)$',raw)
            if not match:
                continue
            directive,operands=match.groups()
            values=[value.strip() for value in operands.split(',')]
            try:
                if directive in ('word','4byte','long'):
                    extent+=4*len(values)
                elif directive in ('byte',):
                    extent+=len(values)
                elif directive in ('2byte','half','short'):
                    extent+=2*len(values)
                elif directive in ('space','skip'):
                    extent+=int(values[0],0)
                elif directive in ('align','p2align'):
                    alignment=1<<int(values[0],0)
                    extent=(extent+alignment-1)&-alignment
                elif directive in ('ascii','asciz','string'):
                    strings=re.findall(r'"(?:\\.|[^"\\])*"',operands)
                    require(strings,'unsupported scaffold string extent expression')
                    extent+=sum(len(ast.literal_eval(value).encode('latin1'))+
                                (1 if directive in ('asciz','string') else 0) for value in strings)
            except (ValueError,SyntaxError,OverflowError):
                supported=False
                break
        if supported:
            declarations[name]=extent
    extra = []
    for item in side.get('supplementary_locations', []):
        require(item.get('unit') == record['unit'] and item.get('owner_tu') == entry['tu'] and
                item.get('section') == section,
                entry['tu'] + ': supplementary location unit/owner/section differs')
        name, address, size = item['name'], int(item['address'], 16), int(item['size'])
        require(name in declarations and declarations[name] == size,
                entry['tu'] + '/' + name + ': supplementary extent differs from original scaffold declaration')
        if name.startswith('D_'):
            require(int(name[2:], 16) == address,
                    entry['tu'] + '/' + name + ': supplementary address differs from original label')
        require(start <= address < address + size <= end,
                entry['tu'] + '/' + name + ': supplementary location escapes its authorized source range')
        record_locations = [loc for loc in record.get('recovered_location_candidates', [])
                            if loc.get('section') == section and
                            int(loc.get('address', '0'), 16) == address and
                            name in (loc.get('names') or [])]
        require(len(record_locations) == 1,
                entry['tu'] + '/' + name + ': supplementary location is not in recovered candidate rows')
        original_names = [s for s in original_elf.symbols if s.name == name and s.value == address
                          and s.shndx not in (0, 0xfff1)]
        meaningful = [s.size for s in original_names if s.type == 1 and s.size > 0]
        extra.append(dict(unit=record['unit'], address=f'0x{address:08X}',section=section,
                          names=[name],original_symbol_size=meaningful[0] if len(set(meaningful)) == 1 else None,
                          supplementary_allocation_id=side['record_id'],size=size))
    packet_copy = dict(packet_tu)
    packet_copy['data'] = list(packet_tu.get('data', [])) + extra
    evidence = dict(path=str(path),sha256=sha(path),record_id=side['record_id'],
                    authorization=side['coordinator_authorization'],
                    original_denominator_unchanged=True,locations=extra)
    return packet_copy, [evidence]


def direct_initialized_symbol_span(entry, location, record, packet_tu, ctx,
                                   object_elf, linked_elf, original_elf,
                                   original_section_ranges, emitted, obj_path,
                                   unit_dir, data_carve):
    """Verify one named C object when the private linker carved its input section.

    This narrow path covers exact scalar/table objects separated into
    `.data.carve.N` by the standard link layout. It proves current cc1 output,
    object bytes, exact original item and retail identity, exact link-script
    section selection, final symbol placement, and final bytes. Any relocation
    inside the object is rejected here; pointer/relocatable payloads remain on
    the normal data-carve path.
    """
    section, address=location['section'],location['address']
    va=int(address,16)
    names=set(location.get('names') or [])
    require(names,entry['tu']+'/'+address+': initialized candidate has no names')
    candidates=[s for s in object_elf.symbols if s.name in names and s.type==1 and
                0<s.shndx<len(object_elf.sections) and s.shndx not in (0,0xfff1)]
    require(len(candidates)==1,entry['tu']+'/'+address+': no unique named C OBJECT in carved input')
    sym=candidates[0]
    input_section=object_elf.sections[sym.shndx]
    require(input_section.type==1 and input_section.flags & 2 and
            (input_section.name==section or input_section.name.startswith(section+'.carve.')),
            entry['tu']+'/'+sym.name+': C object is not in the mapped input section/carve')
    require(sym.size>0 and sym.name in emitted,
            entry['tu']+'/'+sym.name+': cc1 does not emit this C object outside #APP')
    mapped=original_section_ranges.get(section)
    require(mapped and mapped[0]<=va<va+sym.size<=mapped[1],
            entry['tu']+'/'+sym.name+': carved C object escapes exact original TU section map')
    items=data_carve.section_items(ctx,section)
    named_original=[s for s in original_elf.symbols if s.name in names and s.value==va and
                    s.shndx not in (0,0xfff1) and mapped[0]<=s.value<mapped[1]]
    owners=[item for item in items if item['start']<=va and va+sym.size<=item['end'] and
            (names.intersection(item.get('labels') or []) or item['start']==va and named_original)]
    require(len(owners)==1,entry['tu']+'/'+sym.name+': no unique original scaffold item covers exact C object span')
    packet_rows=[x for x in packet_tu.get('data',[]) if x.get('section')==section and
                 int(x.get('address','0'),16)==va and
                 (names.intersection(x.get('names') or []) or named_original)]
    require(packet_rows,entry['tu']+'/'+sym.name+': exact C object address is absent from original packet')
    for original_symbol in original_elf.symbols:
        if original_symbol.shndx in (0,0xfff1) or original_symbol.value!=va or not mapped[0]<=va<mapped[1]:
            continue
        if original_symbol.type==1 and original_symbol.size>0:
            require((sym.bind,sym.type,sym.size)==
                    (original_symbol.bind,original_symbol.type,original_symbol.size),
                    entry['tu']+'/'+sym.name+': carved C OBJECT differs from meaningful original identity '+original_symbol.name)
        else:
            require(sym.bind==original_symbol.bind,
                    entry['tu']+'/'+sym.name+': carved C binding differs from same-address original symbol '+original_symbol.name)
    data=object_elf.section_bytes(input_section)
    require(sym.value+sym.size<=len(data),entry['tu']+'/'+sym.name+': C object exceeds its input section')
    for relsec in object_elf.sections:
        if relsec.type in (4,9) and relsec.info==input_section.index:
            for off,kind,target,addend in data_carve._rel_entries(object_elf,relsec):
                require(not sym.value<=off<sym.value+sym.size,
                        entry['tu']+'/'+sym.name+': carved direct-object path cannot prove a relocated payload')
    original_bytes=data_carve.va_bytes(original_elf,va,sym.size)
    require(data[sym.value:sym.value+sym.size]==original_bytes,
            entry['tu']+'/'+sym.name+': carved C object bytes differ from exact original item')
    # MAIN's scaffold script is main.ld; the candidate ROM link selects its
    # compiler/carved inputs through main.rom.ld. Overlays use <unit>.ld.
    ld_path=unit_dir/('main.rom.ld' if record['unit']=='main' else record['unit']+'.ld')
    require(ld_path.is_file(),entry['tu']+': no current candidate linker script for carved object')
    # Unlike scaffold-layout derivation, this proof checks the current private
    # link script's explicit data-carve selections.  `ovl_restore` rewrites
    # those entries back to the original scaffold inputs, which is useful for
    # deriving original boundaries but would erase the very C input selection
    # this direct-object proof must attest.
    ld_text=ld_path.read_text()
    object_rel=obj_path.relative_to(unit_dir).as_posix()
    require(re.search(re.escape(object_rel)+r'\('+re.escape(input_section.name)+r'\);',ld_text),
            entry['tu']+'/'+sym.name+': current linker script does not select this exact carved input section')
    linked=[s for s in linked_elf.symbols if s.name==sym.name and s.value==va and
            s.size==sym.size and s.type==sym.type and s.bind==sym.bind and s.shndx not in (0,0xfff1)]
    require(len(linked)==1,entry['tu']+'/'+sym.name+': carved C object lacks exact linked identity')
    linked_bytes=data_carve.va_bytes(linked_elf,va,sym.size)
    require(linked_bytes==original_bytes,
            entry['tu']+'/'+sym.name+': final linked carved C bytes differ from original')
    # Preserve meaningful retail names when the C owner is an exact same-address
    # alias.  This direct-object path is used when the overlay linker selects
    # carved object sections instead of producing ordinary data-carve runs;
    # previously it checked the identities but did not emit the alias row that
    # the linker-storage-alias consumer requires.
    alias_rows = []
    for original_symbol in original_elf.symbols:
        if (original_symbol.name != sym.name and original_symbol.value == va and
                original_symbol.type == sym.type and original_symbol.size == sym.size and
                original_symbol.shndx not in (0, 0xfff1) and
                mapped[0] <= original_symbol.value < mapped[1]):
            require((original_symbol.bind, original_symbol.type, original_symbol.size) ==
                    (sym.bind, sym.type, sym.size),
                    entry['tu']+'/'+sym.name+': direct C alias differs from meaningful retail identity '+original_symbol.name)
            alias_rows.append(dict(section=section, scaffold_name=original_symbol.name,
                                   object_symbol=sym.name, address=f'0x{va:08X}',
                                   size=sym.size,
                                   binding=('LOCAL' if sym.bind == 0 else
                                            'GLOBAL' if sym.bind == 1 else str(sym.bind)),
                                   type='OBJECT',
                                   evidence='exact_same_address_original_and_linked_C_OBJECT'))
    return dict(section=section,address=f'0x{va:08X}',names=sorted(names),range=[va,va+sym.size],
                c_object_section=input_section.name,c_object_offset=sym.value,length=sym.size,
                c_object_section_size=input_section.size,c_object_section_alignment=input_section.align,
                object_symbols=[sym.name],object_sha256=sha(obj_path),
                original_identities=[dict(name=s.name,address=f'0x{s.value:08X}',
                                          binding=('LOCAL' if s.bind==0 else 'GLOBAL' if s.bind==1 else str(s.bind)),
                                          type=('OBJECT' if s.type==1 else str(s.type)),size=s.size)
                                     for s in named_original],
                split_range=[va,va+sym.size],scaffold_item=owners[0]['name'],
                symbol_aliases=alias_rows,
                evidence='direct_named_object_exact_bytes_and_link_identity')


def compiler_only_section(assembly, preprocessed, unit_dir, section):
    """Prove anonymous constants using the existing cc1 provenance parser.

    Assemblers discard temporary $LC labels. Such storage is still C output,
    provided its payload is cc1 data and no included scaffold supplies payload
    to the same input section. This does not assign an invented object symbol.
    """
    parsed=tu_audit.AsmFile(assembly)
    require(not parsed.errors,'cc1 provenance parser errors: '+str(parsed.errors))
    # GNU MIPS cc1 spells compiler-generated read-only data as `.rdata`;
    # the selected EE assembler folds that input section into ELF `.rodata`.
    # Keep this one standard assembler alias narrow and verify the final
    # object/linked `.rodata` bytes and split placement at the caller.
    compiler_sections={section}
    if section=='.rodata':
        compiler_sections.add('.rdata')
    directives=[row for row in parsed.top_data if row['section'] in compiler_sections]
    region_switch_tables=[]
    # cc1 places some computed-switch tables after their function body but
    # before `.end`. AsmFile correctly retains these rows in that function's
    # `region['data']`, rather than `top_data`; admit only the narrow local-
    # label `.word` table shape emitted for a C switch. This does not admit
    # arbitrary region data or inline-assembly payload.
    if section=='.rodata':
        source_lines=assembly.splitlines()
        app_lines=set()
        in_app=False
        for lineno,line in enumerate(source_lines,1):
            if line.strip()=='#APP':
                in_app=True
                app_lines.add(lineno)
                continue
            if line.strip()=='#NO_APP':
                app_lines.add(lineno)
                in_app=False
                continue
            if in_app:
                app_lines.add(lineno)
        c_functions=cc1_functions(assembly)
        for region in parsed.regions:
            if region.get('name') not in c_functions:
                continue
            rows=[row for row in region.get('data',[]) if row.get('section') in compiler_sections]
            if not rows:
                continue
            labels={name for name,sec in region.get('labels',[]) if sec in compiler_sections}
            table_labels={name for name in labels if re.fullmatch(r'\$L\d+',name)}
            local_text_labels={name for name,sec in region.get('labels',[]) if sec=='.text'}
            if not table_labels or not local_text_labels:
                continue
            if not all(row.get('directive')=='.word' and
                       re.fullmatch(r'\$L\d+',row.get('operands','').strip()) and
                       row.get('line') not in app_lines for row in rows):
                continue
            if not all(row['operands'].strip() in local_text_labels for row in rows):
                continue
            first_data_line=min(row['line'] for row in rows)
            body_start=max(0,int(region.get('start',1))-1)
            before_data=source_lines[body_start:max(0,first_data_line-1)]
            referenced={label for label in table_labels
                        if any(re.search(r'(?<![\w.$])'+re.escape(label)+r'(?![\w.$])',line)
                               for line in before_data)}
            if not referenced:
                continue
            directives.extend(rows)
            region_switch_tables.append(dict(function=region['name'],
                                             table_labels=sorted(referenced),
                                             word_count=len(rows)))
    if not directives and section == '.lit4':
        return None
    require(directives,'Anonymous section has no compiler data directives')
    authored=tu_audit.asm_statement_lines(preprocessed)
    for row in directives:
        normalized=tu_audit._norm_asm_line(row['directive']+' '+row['operands'])
        require(normalized not in authored,'Anonymous payload is present in an authored asm statement')
    seen=set()
    def inspect_include(path,depth=0):
        require(depth<=16,'Nested scaffold include depth exceeds the provenance bound')
        path=path.resolve()
        if path in seen or path.name=='labels.inc':return
        seen.add(path)
        contents=path.read_text(errors='surrogateescape')
        included=tu_audit.AsmFile(contents)
        require(not any(row['section'] in compiler_sections for row in included.top_data),
                'Included scaffold emits payload into the anonymous compiler section: '+str(path))
        for raw in contents.splitlines():
            match=re.match(r'\s*\.include\s+"([^"]+)"',raw)
            if match:
                target=unit_dir/match[1]
                if not target.is_file():target=path.parent/match[1]
                inspect_include(target,depth+1)
    for block in parsed.top_app:
        for _,code in block['lines']:
            match=re.fullmatch(r'\.include\s+"([^"]+)"',code)
            if match:
                path=unit_dir/match[1]
                if not path.is_file():path=unit_dir/'include'/match[1]
                inspect_include(path)
    return dict(kind='anonymous_cc1_constants',section=section,
                compiler_payload_directives=len(directives),
                compiler_region_switch_tables=region_switch_tables,
                scaffold_includes_checked=len(seen))


def compiler_li_s_pool(assembly, preprocessed, unit_dir, object_elf, csec, piece, section):
    """Prove a `.lit4` item emitted by standard cc1 `li.s` lowering.

    The pseudo is outside #APP in a named C function; R_MIPS_LITERAL from
    that function must reach each exact pool word. The object bytes and normal
    linker placement are independently checked by plan_split and the caller.
    """
    require(section == '.lit4', 'li.s pool proof is restricted to .lit4')
    parsed = tu_audit.AsmFile(assembly)
    require(not parsed.errors, 'cc1 provenance parser errors: ' + str(parsed.errors))
    authored = tu_audit.asm_statement_lines(preprocessed)
    li_by_function = {}
    app = False
    current = None
    li_re = re.compile(r'^\s*li\.s\s+\$f\d+\s*,\s*([-+]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][-+]?\d+)?)\s*(?:[#;].*)?$')
    for line in assembly.splitlines():
        stripped = line.strip()
        if stripped == '#APP':
            app = True
            continue
        if stripped == '#NO_APP':
            app = False
            continue
        if app:
            continue
        ent = re.match(r'\s*\.ent\s+(\S+)', line)
        if ent:
            current = ent[1]
            continue
        end = re.match(r'\s*\.end\s+(\S+)', line)
        if end and current == end[1]:
            current = None
            continue
        match = li_re.match(line)
        if match and current:
            normalized = tu_audit._norm_asm_line(line)
            require(normalized not in authored, 'li.s pool operand is present in authored asm')
            bits = struct.unpack('<I', struct.pack('<f', float(match[1])))[0]
            li_by_function.setdefault(current, set()).add(bits)
    require(li_by_function, 'no outside-#APP C li.s pseudo is available for this .lit4 pool')
    c_functions = {s.name: s for s in object_elf.symbols
                   if s.name in li_by_function and s.type == 2 and s.size
                   and 0 < s.shndx < len(object_elf.sections)}
    require(c_functions, 'li.s source has no current compiler function symbols')
    refs = data_carve.section_references(object_elf, csec)
    witnesses = []
    for ref in refs:
        if ref['type'] != 8 or not piece['offset'] <= ref['target'] < piece['offset'] + piece['length']:
            continue
        relsec = object_elf.sections[ref['rel']]
        if relsec.info >= len(object_elf.sections):
            continue
        source_sec = object_elf.sections[relsec.info]
        functions = [s for s in c_functions.values()
                     if s.shndx == source_sec.index and s.value <= ref['offset'] < s.value + s.size]
        for function in functions:
            target_word = struct.unpack('<I', object_elf.section_bytes(csec)[ref['target']:ref['target'] + 4])[0]
            if target_word in li_by_function.get(function.name, set()):
                witnesses.append(dict(function=function.name, target_offset=ref['target'],
                                       literal_bits=f'0x{target_word:08X}', relocation_offset=ref['offset']))
    start, end = piece['offset'], piece['offset'] + piece['length']
    require(piece['length'] > 0 and piece['length'] % 4 == 0,
            'anonymous .lit4 piece is not a sequence of complete float words')
    required_offsets = set(range(start, end, 4))
    witnessed_offsets = {x['target_offset'] for x in witnesses}
    require(witnessed_offsets == required_offsets,
            'not every .lit4 word has an exact C li.s/R_MIPS_LITERAL witness')
    return dict(kind='anonymous_cc1_li_s_pool', section=section,
                range=[piece['lo'], piece['lo'] + piece['length']],
                words=len(required_offsets), witnesses=sorted(witnesses, key=lambda x:x['target_offset']))


def ov02_tu011_uncredited_literal_dependency(entry, record, unit_dir, source,
                                             obj_path, asm_path, linked_path,
                                             object_elf, linked_elf, original_elf,
                                             allocation_path, ctx, derived):
    """Prove the TU011 C literal's scaffold mapping without awarding data credit."""
    if entry.get('tu') != 'ov02/tu011':
        return None
    section = '.rodata'
    lo, hi = 0x00A13200, 0x00A13218
    item_matches = [item for item in data_carve.section_items(ctx, section)
                    if item.get('name') == 'D_00A13200' and
                    item.get('start') == lo and item.get('end') == hi]
    require(len(item_matches) == 1,
            'ov02/tu011 literal dependency: exact original scaffold item is absent')
    require(not any(loc.get('section') == section and
                    int(loc.get('address', '0'), 16) == lo
                    for loc in record.get('recovered_location_candidates', [])),
            'ov02/tu011 literal dependency must remain unclaimed')
    require(section in derived.get('runs', {}),
            'ov02/tu011 literal dependency: current derivation lacks .rodata')
    items = data_carve.section_items(ctx, section)
    spans = data_carve.declared_runs(entry['tu'], section, derived['runs'][section])
    csec = next(s for s in object_elf.sections if s.name == section and s.size)
    plan = data_carve.plan_split(object_elf, csec,
                                 data_carve.run_specs(items, spans),
                                 lambda va, n: data_carve.va_bytes(original_elf, va, n))
    pieces = [p for p in plan['pieces'] if p['lo'] == lo and p['hi'] == hi]
    require(len(pieces) == 1 and pieces[0]['offset'] == 0 and pieces[0]['length'] == 24,
            'ov02/tu011 literal dependency: raw C object offset does not map exactly to the scaffold span')
    piece = pieces[0]
    def section_payload(elf, sec):
        return b'' if sec.type == 8 else elf.data[sec.offset:sec.offset+sec.size]
    raw = section_payload(object_elf, csec)[piece['offset']:piece['offset']+piece['length']]
    original = data_carve.va_bytes(original_elf, lo, hi-lo)
    linked_bytes = data_carve.va_bytes(linked_elf, lo, hi-lo)
    expected = b'Environmental Simulator\0'
    require(raw == expected and raw == original and linked_bytes == original,
            'ov02/tu011 literal dependency: compiler/object/original/linked bytes are not all identical')
    assembly = asm_path.read_text(errors='surrogateescape')
    parsed = tu_audit.AsmFile(assembly)
    require(not parsed.errors, 'ov02/tu011 literal dependency: cc1 parser errors')
    rows = []
    for row in parsed.top_data:
        if row.get('section') != section or row.get('directive') != '.ascii':
            continue
        try:
            payload = ast.literal_eval(row.get('operands', ''))
        except (SyntaxError, ValueError):
            continue
        if isinstance(payload, str) and payload.encode('latin-1') == raw:
            rows.append(row)
    require(len(rows) == 1,
            'ov02/tu011 literal dependency: no unique top-level cc1 .ascii payload matches the object span')
    preprocessed_path = asm_path.with_suffix('.i')
    require(preprocessed_path.is_file(), 'ov02/tu011 literal dependency: preprocessed input is absent')
    preprocessed = preprocessed_path.read_text(errors='surrogateescape')
    require('"Environmental Simulator"' in preprocessed,
            'ov02/tu011 literal dependency: source literal is absent from current preprocessed input')
    normalized = tu_audit._norm_asm_line(rows[0]['directive']+' '+rows[0]['operands'])
    require(normalized not in tu_audit.asm_statement_lines(preprocessed),
            'ov02/tu011 literal dependency: matching .ascii is present in authored inline assembly')
    carved = obj_path.with_name(obj_path.stem + '.carved.o')
    require(carved.is_file(), 'ov02/tu011 literal dependency: selected C carve object is absent')
    carved_elf = Elf(carved.read_bytes())
    carved_section = next((s for s in carved_elf.sections if s.name == section), None)
    require(carved_section is not None and carved_section.size == hi-lo and
            section_payload(carved_elf, carved_section) == raw,
            'ov02/tu011 literal dependency: selected carve input differs from the exact object slice')
    ld_path = unit_dir / 'ov02.ld'
    ld_text = ld_path.read_text()
    carved_rel = carved.relative_to(unit_dir).as_posix()
    require(re.search(re.escape(carved_rel)+r'\(\.rodata\);', ld_text),
            'ov02/tu011 literal dependency: linker script does not select this exact C carve input')
    return dict(disposition='uncredited_scaffold_dependency', credited=False,
                reason='compiler-authored C string matches this original scaffold span; it remains outside recovered-location credit',
                section=section, scaffold_name='D_00A13200', scaffold_range=[lo, hi],
                c_object_offset=piece['offset'], length=piece['length'],
                c_object_section=section, c_object_section_size=csec.size,
                c_object_section_alignment=csec.align, selected_carve_path=str(carved),
                selected_carve_sha256=sha(carved), selected_carve_section_offset=0,
                linker_script_path=str(ld_path), linker_script_sha256=sha(ld_path),
                cc1_ascii_line=rows[0]['line'], cc1_operand=rows[0]['operands'],
                payload_sha256=hashlib.sha256(raw).hexdigest(),
                source_sha256=sha(source), object_sha256=sha(obj_path),
                assembly_sha256=sha(asm_path), linked_elf_sha256=sha(linked_path),
                original_elf_sha256=sha(ctx['orig']), packet_sha256=sha(allocation_path))


def compiler_switch_table_dependencies(entry, record, unit_dir, source,
                                       obj_path, asm_path, linked_path,
                                       object_elf, linked_elf, original_elf,
                                       allocation_path, ctx, derived, data_carve):
    """Prove C-emitted switch payloads that remain outside recovered credit.

    A carve run can contain packet labels for a compiler switch table even
    when that table is intentionally not among the recovered locations.  This
    record ties those exact labels to outside-#APP cc1 `.word $Lnnn` rows,
    R_MIPS_32 relocations, the selected carved input, and the byte-identical
    final image.  It is a dependency only: it never adds a location, changes a
    run, or contributes to recovered-location counts.
    """
    if record.get('unit') is None:
        return []
    section = '.rodata'
    run_records = derived.get('runs', {}).get(section, [])
    if not run_records:
        return []
    # Main uses a distinct whole-object linker route. Without an explicit
    # exact object-to-VA witness, retain its switch labels as scaffold data.
    if entry['tu'].split('/', 1)[0] == 'main':
        return []
    items = data_carve.section_items(ctx, section)
    spans = data_carve.declared_runs(entry['tu'], section, run_records)
    csec = next((s for s in object_elf.sections if s.name == section and s.size), None)
    require(csec is not None, entry['tu'] + ': switch dependency lacks current .rodata input')
    plan = data_carve.plan_split(object_elf, csec,
                                     data_carve.run_specs(items, spans),
                                 lambda va, n: data_carve.va_bytes(original_elf, va, n))
    claimed = {(loc.get('section'), int(loc.get('address', '0'), 16), name)
               for loc in record.get('recovered_location_candidates', [])
               for name in (loc.get('names') or [])}
    raw_asm = asm_path.read_text(errors='surrogateescape')
    preprocessed_path = asm_path.with_suffix('.i')
    if not preprocessed_path.is_file() and obj_path.name.endswith(('.allocated.carved.o', '.allocated.o')):
        preprocessed_path = unit_dir / 'build' / 'scaffold' / 'src' / 'ov10' / (source.stem + '.i')
    require(preprocessed_path.is_file(), entry['tu'] + ': switch dependency lacks current preprocessed C')
    compiler_proof = compiler_only_section(raw_asm,
                    preprocessed_path.read_text(errors='surrogateescape'), unit_dir, section)
    require(compiler_proof is not None, entry['tu'] + ': no compiler section proof for switch data')
    switch_by_function = {}
    for row in compiler_proof.get('compiler_region_switch_tables', []):
        switch_by_function[row['function']] = row
    rel_sections = [s for s in object_elf.sections
                    if s.type in (4, 9) and s.info == csec.index]
    linked_script = unit_dir / (record['unit'] + '.ld')
    require(linked_script.is_file(), entry['tu'] + ': no current unit linker script')
    ld_text = linked_script.read_text()
    carved = obj_path.with_name(obj_path.stem + '.carved.o')
    if not carved.is_file():
        return []
    carved_elf = Elf(carved.read_bytes())
    selected_rel = carved.relative_to(unit_dir).as_posix()
    payload = object_elf.section_bytes(csec)
    out = []
    for index, piece in enumerate(plan['pieces']):
        run = next((r for r in run_records
                    if int(r['range'][0], 16) == piece['lo'] and
                       int(r['range'][1], 16) == piece['hi']), None)
        if run is None:
            continue
        table_items = [item for item in items
                       if item['name'] in (run.get('c_owned_symbols') or []) and
                          item['name'].startswith('jtbl_') and
                          piece['lo'] <= item['start'] < item['end'] <= piece['hi'] and
                          (section, item['start'], item['name']) not in claimed]
        if not table_items:
            continue
        functions = set(run.get('generated_by') or [])
        regions = [switch_by_function[name] for name in sorted(functions)
                   if name in switch_by_function]
        require(regions, entry['tu'] + ': unclaimed switch labels have no outside-#APP C region table proof')
        word_count = sum(int(row['word_count']) for row in regions)
        lo_off, hi_off = piece['offset'], piece['offset'] + piece['length']
        relocations = []
        for relsec in rel_sections:
            for offset, rtype, symidx, addend in data_carve._rel_entries(object_elf, relsec):
                if not lo_off <= offset < hi_off:
                    continue
                require(rtype == 2, entry['tu'] + ': switch dependency contains non-R_MIPS_32 relocation')
                require(symidx < len(object_elf.symbols), entry['tu'] + ': malformed switch relocation symbol')
                target = object_elf.symbols[symidx]
                textsec = next((s for s in object_elf.sections if s.name == '.text'), None)
                require(textsec is not None and target.shndx == textsec.index,
                        entry['tu'] + ': switch relocation does not target this C object .text')
                relocations.append(dict(offset=offset,type='R_MIPS_32',symbol=target.name,
                                        symbol_section=target.shndx,addend=addend))
        require(len(relocations) == word_count,
                entry['tu'] + ': compiler-region switch word count differs from exact .rodata relocations')
        item_rows = []
        all_table_reloc_offsets = set()
        for item in table_items:
            start = piece['offset'] + item['start'] - piece['lo']
            length = item['end'] - item['start']
            table_relocs = [r for r in relocations if start <= r['offset'] < start + length]
            require(table_relocs and all((r['offset'] - start) % 4 == 0 for r in table_relocs),
                    entry['tu'] + '/' + item['name'] + ': no aligned C switch relocations map to this scaffold table')
            all_table_reloc_offsets.update(r['offset'] for r in table_relocs)
            item_rows.append(dict(scaffold_name=item['name'],scaffold_range=[item['start'],item['end']],
                                  object_offset=start,length=length,
                                  relocation_offsets=sorted(r['offset'] for r in table_relocs)))
        require(all(r['offset'] in all_table_reloc_offsets for r in relocations),
                entry['tu'] + ': switch relocation escapes exact unclaimed table items')
        original_bytes = data_carve.va_bytes(original_elf, piece['lo'], piece['length'])
        require(len(original_bytes) == piece['length'] and
                data_carve.va_bytes(linked_elf, piece['lo'], piece['length']) == original_bytes,
                entry['tu'] + ': final linked switch dependency bytes differ from original scaffold span')
        carved_sec_name = section if index == 0 else section + '.carve.' + str(index)
        carved_sec = next((s for s in carved_elf.sections if s.name == carved_sec_name), None)
        require(carved_sec is not None and carved_sec.size == piece['length'] and
                carved_elf.section_bytes(carved_sec) == payload[piece['offset']:piece['offset']+piece['length']],
                entry['tu'] + ': selected carved switch input is not the exact compiler section slice')
        require(re.search(re.escape(selected_rel) + r'\(' + re.escape(carved_sec_name) + r'\);', ld_text),
                entry['tu'] + ': current linker script does not select the exact switch carve input')
        out.append(dict(disposition='uncredited_scaffold_dependency',credited=False,
                        kind='compiler_region_switch_tables',section=section,
                        scaffold_range=[piece['lo'],piece['hi']],
                        scaffold_items=item_rows,compiler_regions=regions,
                        compiler_word_count=word_count,relocations=relocations,
                        c_object_offset=piece['offset'],length=piece['length'],
                        c_object_section=section,c_object_section_size=csec.size,
                        c_object_section_alignment=csec.align,
                        selected_carve_path=str(carved),selected_carve_sha256=sha(carved),
                        selected_carve_section=carved_sec_name,
                        linker_script_path=str(linked_script),linker_script_sha256=sha(linked_script),
                        source_sha256=sha(source),object_sha256=sha(obj_path),
                        assembly_sha256=sha(asm_path),linked_elf_sha256=sha(linked_path),
                        original_elf_sha256=sha(ctx['orig']),packet_sha256=sha(allocation_path)))
    return out


def compiler_switch_word_dependencies(entry, record, unit_dir, source,
                                     obj_path, asm_path, linked_path,
                                     object_elf, linked_elf, original_elf,
                                     allocation_path, ctx, derived, data_carve):
    """Map each unclaimed C switch-table word to its exact scaffold VA.

    The table's enclosing Splat item can include neighboring strings or
    alignment bytes. This records only the 4-byte outside-#APP `.word` entries
    that have corresponding R_MIPS_32 relocations to local C text labels.
    """
    if record.get('unit') is None:
        return []
    section = '.rodata'
    run_records = derived.get('runs', {}).get(section, [])
    if not run_records:
        return []
    csec = next((s for s in object_elf.sections if s.name == section and s.size), None)
    if csec is None:
        return []
    raw_asm = asm_path.read_text(errors='surrogateescape')
    pp = asm_path.with_suffix('.i')
    if not pp.is_file():
        pp = unit_dir / 'build' / 'scaffold' / 'src' / record['unit'] / (source.stem + '.i')
    require(pp.is_file(), entry['tu'] + ': switch proof lacks current preprocessed source')
    compiler = compiler_only_section(raw_asm, pp.read_text(errors='surrogateescape'), unit_dir, section)
    if not compiler:
        return []
    regions = compiler['compiler_region_switch_tables']
    if not regions:
        return []
    region_functions={r['function'] for r in regions}
    switch_runs=[run for run in run_records
                 if region_functions.intersection(run.get('generated_by') or [])]
    if not switch_runs:
        return []
    items = data_carve.section_items(ctx, section)
    claimed = {(loc.get('section'), int(loc.get('address', '0'), 16), name)
               for loc in record.get('recovered_location_candidates', [])
               for name in (loc.get('names') or [])}
    # Recovered tables already passed the current compiler/carve/link proof.
    # This helper has no dependency to establish for those tables; unrelated
    # scaffold .text relocations must not create a second claim obligation.
    if not any(item['name'].startswith('jtbl_') and
               (section,item['start'],item['name']) not in claimed and
               any(int(run['range'][0],16) <= item['start'] < item['end'] <= int(run['range'][1],16)
                   for run in switch_runs) for item in items):
        return []
    spans = data_carve.declared_runs(entry['tu'], section, switch_runs)
    plan = data_carve.plan_split(object_elf, csec, data_carve.run_specs(items, spans),
                                 lambda va, n: data_carve.va_bytes(original_elf, va, n))
    linked_script = (unit_dir / 'main.elf.ld' if record['unit'] == 'main'
                     else unit_dir / (record['unit'] + '.ld'))
    if not linked_script.is_file():
        return []
    ld_text = linked_script.read_text()
    carved = obj_path.with_name(obj_path.stem + '.carved.o')
    raw_selected = False
    if carved.is_file():
        selected_path = carved
        carved_elf = Elf(carved.read_bytes())
        selected_rel = carved.relative_to(unit_dir).as_posix()
    else:
        # Some overlays link the compiler object's complete section directly
        # after a pinned scaffold run. Admit only that exact script selection.
        selected_path = obj_path
        selected_rel = obj_path.relative_to(unit_dir).as_posix()
        if not re.search(re.escape(selected_rel) + r'\(' + re.escape(section) + r'\);', ld_text):
            return []
        carved_elf = object_elf
        raw_selected = True
    rel_entries = []
    for relsec in object_elf.sections:
        if relsec.type not in (4, 9) or relsec.info != csec.index:
            continue
        for off, kind, symidx, addend in data_carve._rel_entries(object_elf, relsec):
            if kind != 2 or symidx >= len(object_elf.symbols):
                continue
            target = object_elf.symbols[symidx]
            if target.shndx == next((s.index for s in object_elf.sections if s.name == '.text'), -1):
                rel_entries.append(off)
    rel_entries = sorted(set(rel_entries))
    expected_total = sum(int(r['word_count']) for r in regions)
    require(len(rel_entries) == expected_total,
            entry['tu'] + ': C switch `.word` count does not equal exact .text R_MIPS_32 offsets')
    rel_cursor = 0
    output = []
    payload = object_elf.section_bytes(csec)
    for piece_index, piece in enumerate(plan['pieces']):
        run = next((r for r in run_records
                    if int(r['range'][0], 16) == piece['lo'] and
                       int(r['range'][1], 16) == piece['hi']), None)
        if not run:
            continue
        functions = set(run.get('generated_by') or [])
        piece_regions = [r for r in regions if r['function'] in functions]
        if not piece_regions:
            continue
        piece_relocs = [off for off in rel_entries
                        if piece['offset'] <= off < piece['offset'] + piece['length']]
        expected = sum(int(r['word_count']) for r in piece_regions)
        if len(piece_relocs) != expected:
            continue  # keep this run scaffold-owned until exact partition is available
        secname = section if raw_selected else (section if piece_index == 0 else section + '.carve.' + str(piece_index))
        carved_sec = next((s for s in carved_elf.sections if s.name == secname), None)
        if not carved_sec or carved_sec.size != piece['length'] or \
           carved_elf.section_bytes(carved_sec) != payload[piece['offset']:piece['offset']+piece['length']]:
            continue
        if not re.search(re.escape(selected_rel)+r'\('+re.escape(secname)+r'\);', ld_text):
            continue
        rel_iter = iter(piece_relocs)
        word_rows = []
        for region in piece_regions:
            for _ in range(int(region['word_count'])):
                off = next(rel_iter)
                va = piece['lo'] + off - piece['offset']
                scaff = next((item for item in items if item['start'] <= va and va+4 <= item['end']), None)
                require(scaff is not None,
                        entry['tu'] + ': switch word offset does not map inside one exact scaffold item')
                if (section, scaff['start'], scaff['name']) in claimed:
                    # A partially claimed Splat item cannot be used as an
                    # uncredited dependency without an exact item-level split.
                    continue
                orig = data_carve.va_bytes(original_elf, va, 4)
                linked = data_carve.va_bytes(linked_elf, va, 4)
                raw = payload[off:off+4]
                raw_value = struct.unpack('<I', raw)[0] if len(raw) == 4 else None
                final_value = struct.unpack('<I', linked)[0] if len(linked) == 4 else None
                original_value = struct.unpack('<I', orig)[0] if len(orig) == 4 else None
                require(len(raw) == len(orig) == len(linked) == 4 and
                        original_value == final_value,
                        entry['tu'] + ': relocated switch word differs in original and linked images')
                word_rows.append(dict(compiler_function=region['function'],
                                      compiler_table_labels=region['table_labels'],
                                      scaffold_name=scaff['name'], scaffold_range=[va, va+4],
                                      object_offset=off, length=4, relocation_offset=off,
                                      raw_rel_addend=raw_value,
                                      original_linked_word=f'0x{original_value:08X}',
                                      payload_sha256=hashlib.sha256(raw).hexdigest()))
        if word_rows:
            output.append(dict(disposition='uncredited_scaffold_dependency', credited=False,
                               kind='compiler_region_switch_words', section=section,
                               word_spans=word_rows,
                               compiler_word_count=len(word_rows),
                               c_object_section_size=csec.size,
                               c_object_section_alignment=csec.align,
                               selected_carve_path=str(selected_path), selected_carve_sha256=sha(selected_path),
                               selected_carve_section=secname,
                               linker_script_path=str(linked_script), linker_script_sha256=sha(linked_script),
                               source_sha256=sha(source), object_sha256=sha(obj_path),
                               assembly_sha256=sha(asm_path), linked_elf_sha256=sha(linked_path),
                               original_elf_sha256=sha(ctx['orig']), packet_sha256=sha(allocation_path)))
    return output


def uncredited_li_s_dependencies(entry, record, unit_dir, source,
                                 obj_path, asm_path, linked_path,
                                 object_elf, linked_elf, original_elf,
                                 allocation_path, ctx, derived, data_carve):
    """Prove explicitly recorded, out-of-scope C literal dependencies.

    Each requested row is checked against the immutable packet item and the
    compiler's exact section split. It is deliberately reported separately
    from verified locations so auxiliary bytes cannot inflate recovery scope.
    """
    requests = [r for r in record.get('uncredited_data_dependencies', [])
                if r.get('kind') in (None, 'li_s_pool') and r.get('section') == '.lit4']
    out = []
    for request in requests:
        section = request['section']
        address = request['address']
        lo = int(address, 0) if isinstance(address, str) else int(address)
        length = int(request['size'])
        hi = lo + length
        name = request['name']
        require(section == '.lit4' and length > 0 and length % 4 == 0,
                entry['tu'] + ': uncredited literal dependency is not a complete .lit4 word range')
        require(not any(loc.get('section') == section and
                        int(loc.get('address', '0'), 16) == lo and
                        name in (loc.get('names') or [])
                        for loc in record.get('recovered_location_candidates', [])),
                entry['tu'] + '/' + name + ': uncredited dependency is also claimed for credit')
        items = [item for item in data_carve.section_items(ctx, section)
                 if item.get('name') == name and item.get('start') == lo and
                 item.get('end') == hi]
        require(len(items) == 1,
                entry['tu'] + '/' + name + ': exact original scaffold item is absent')
        spans = data_carve.declared_runs(entry['tu'], section,
                                         derived.get('runs', {}).get(section, []))
        require(any(a <= lo and hi <= b for a, b in spans),
                entry['tu'] + '/' + name + ': dependency is outside fresh derived section runs')
        csec = next((s for s in object_elf.sections if s.name == section and s.size), None)
        require(csec is not None,
                entry['tu'] + '/' + name + ': current compiler section is absent')
        plan = data_carve.plan_split(object_elf, csec,
                                     data_carve.run_specs(data_carve.section_items(ctx, section),
                                                          spans),
                                     lambda va, n: data_carve.va_bytes(original_elf, va, n))
        pieces = [piece for piece in plan['pieces']
                  if piece['lo'] == lo and piece['hi'] == hi]
        require(len(pieces) == 1 and pieces[0]['length'] == length,
                entry['tu'] + '/' + name + ': object offset does not map exactly to the auxiliary scaffold item')
        piece = pieces[0]
        preprocessed_path = asm_path.with_suffix('.i') if record['unit'] != 'main' else Path(str(obj_path) + '.i')
        require(preprocessed_path.is_file(), entry['tu'] + ': uncredited literal lacks current preprocessed C')
        proof = compiler_li_s_pool(asm_path.read_text(errors='surrogateescape'),
                                   preprocessed_path.read_text(errors='surrogateescape'),
                                   unit_dir, object_elf, csec, piece, section)
        raw = object_elf.section_bytes(csec)[piece['offset']:piece['offset'] + length]
        original = data_carve.va_bytes(original_elf, lo, length)
        linked = data_carve.va_bytes(linked_elf, lo, length)
        require(raw == original == linked,
                entry['tu'] + '/' + name + ': compiler, original and linked literal bytes differ')
        out.append(dict(disposition='uncredited_auxiliary_dependency', credited=False,
                        reason=request.get('reason', 'auxiliary compiler literal outside the frozen location scope'),
                        section=section, scaffold_name=name, scaffold_range=[lo, hi],
                        c_object_offset=piece['offset'], length=length,
                        c_object_section=section, c_object_section_size=csec.size,
                        c_object_section_alignment=csec.align,
                        payload_sha256=hashlib.sha256(raw).hexdigest(),
                        anonymous_proof=proof,
                        source_sha256=sha(source), object_sha256=sha(obj_path),
                        assembly_sha256=sha(asm_path), linked_elf_sha256=sha(linked_path),
                        original_elf_sha256=sha(ctx['orig']),
                        allocation_packet_sha256=sha(allocation_path)))
    return out


def uncredited_named_object_dependencies(entry, record, unit_dir, source, obj_path,
                                         asm_path, linked_path, object_elf,
                                         linked_elf, original_elf,
                                         allocation_path, ctx, derived,
                                         data_carve):
    """Prove exact compiler-emitted named objects retained as scaffolding.

    These are auxiliary dependencies outside the frozen packet denominator.
    The record must give an exact original scaffold item; the current compiler
    object and final linked ELF must both carry the same named OBJECT at the
    exact mapped address, binding, type, and size. This path never awards data
    credit and never widens a neighboring claimed run.
    """
    requests = [r for r in record.get('uncredited_data_dependencies', [])
                if r.get('kind') == 'named_object']
    out = []
    for request in requests:
        section = request['section']
        name = request['name']
        symbol_name = request.get('object_symbol', name)
        address = request['address']
        lo = int(address, 0) if isinstance(address, str) else int(address)
        length = int(request['size'])
        hi = lo + length
        require(length > 0, entry['tu']+'/'+name+': named dependency has empty extent')
        require(not any(loc.get('section') == section and
                        int(loc.get('address', '0'), 16) == lo and
                        name in (loc.get('names') or [])
                        for loc in record.get('recovered_location_candidates', [])),
                entry['tu']+'/'+name+': uncredited dependency is also claimed for credit')
        items = [item for item in data_carve.section_items(ctx, section)
                 if item.get('name') == name and item.get('start') == lo and
                 item.get('end') == hi]
        require(len(items) == 1,
                entry['tu']+'/'+name+': exact original scaffold item is absent')
        declared = data_carve.declared_runs(entry['tu'], section,
                                             derived.get('runs', {}).get(section, []))
        require(any(a <= lo and hi <= b for a, b in declared),
                entry['tu']+'/'+name+': dependency is outside fresh derived section runs')
        csec = next((s for s in object_elf.sections if s.name == section and s.size), None)
        require(csec is not None,
                entry['tu']+'/'+name+': current compiler section is absent')
        plan = data_carve.plan_split(object_elf, csec,
                                     data_carve.run_specs(data_carve.section_items(ctx, section), declared),
                                     lambda va, n: data_carve.va_bytes(original_elf, va, n))
        pieces = [p for p in plan['pieces'] if p['lo'] <= lo and hi <= p['hi']]
        require(len(pieces) == 1,
                entry['tu']+'/'+name+': scaffold item is not inside one exact compiler run')
        piece = pieces[0]
        object_offset = piece['offset'] + lo - piece['lo']
        c_symbol = next((s for s in object_elf.symbols
                         if s.name == symbol_name and s.shndx == csec.index and
                         s.value == object_offset and s.size == length and s.type == 1), None)
        require(c_symbol is not None,
                entry['tu']+'/'+name+': compiler object lacks exact named OBJECT identity')
        expected_binding = request.get('binding')
        if expected_binding is not None:
            binding = {0:'LOCAL', 1:'GLOBAL', 2:'WEAK'}.get(c_symbol.bind, str(c_symbol.bind))
            require(binding == expected_binding,
                    entry['tu']+'/'+name+': compiler OBJECT binding differs from recorded identity')
        asm = asm_path.read_text(errors='surrogateescape')
        preprocessed_path = asm_path.with_suffix('.i') if record['unit'] != 'main' else Path(str(obj_path) + '.i')
        require(preprocessed_path.is_file(),
                entry['tu']+'/'+name+': named dependency lacks current preprocessed C')
        authored = tu_audit.asm_statement_lines(
            preprocessed_path.read_text(errors='surrogateescape'))
        app = False
        current_section = None
        emitted_label = False
        for line in asm.splitlines():
            stripped = line.strip()
            if stripped == '#APP':
                app = True
                continue
            if stripped == '#NO_APP':
                app = False
                continue
            if app:
                continue
            section_match = re.match(r'\s*\.section\s+([^,\s]+)', line)
            if section_match:
                current_section = section_match.group(1)
            elif re.match(r'\s*\.(?:rdata|rodata)\b', line):
                current_section = '.rdata' if stripped.startswith('.rdata') else '.rodata'
            if current_section in (section, '.rdata') and re.match(
                    r'^\s*'+re.escape(symbol_name)+r':\s*(?:[#;].*)?$', line):
                emitted_label = True
                break
        require(emitted_label,
                entry['tu']+'/'+name+': current cc1 output lacks an outside-#APP data label')
        require(not any(re.search(r'\b'+re.escape(symbol_name)+r'\b', line)
                        for line in authored),
                entry['tu']+'/'+name+': dependency label is in authored assembly')
        raw = object_elf.section_bytes(csec)[object_offset:object_offset+length]
        original = data_carve.va_bytes(original_elf, lo, length)
        linked = data_carve.va_bytes(linked_elf, lo, length)
        require(len(raw) == len(original) == len(linked) == length and
                raw == original == linked,
                entry['tu']+'/'+name+': raw, original and linked bytes differ')
        final_symbol = next((s for s in linked_elf.symbols
                             if s.name == symbol_name and s.value == lo), None)
        require(final_symbol is not None and final_symbol.size == length and
                final_symbol.type == c_symbol.type and final_symbol.bind == c_symbol.bind,
                entry['tu']+'/'+name+': final linked OBJECT identity differs from compiler object')
        original_section_index = next((s.index for s in original_elf.sections
                                        if s.name == section), None)
        original_meaningful = [s for s in original_elf.symbols
                               if s.shndx == original_section_index and s.value == lo and
                               s.type == c_symbol.type and s.size > 0 and
                               s.shndx not in (0,0xfff1)]
        original_named = [s for s in original_elf.symbols if s.name == name and
                          s.shndx == original_section_index]
        require(not original_named or any(
                    s.value == lo and s.size == length and s.bind == c_symbol.bind and
                    s.type == c_symbol.type for s in original_named),
                entry['tu']+'/'+name+': same-name retail symbol conflicts with compiler OBJECT')
        require(not original_meaningful or any(
                    s.size == length and s.bind == c_symbol.bind and s.type == c_symbol.type
                    for s in original_meaningful),
                entry['tu']+'/'+name+': compiler OBJECT conflicts with meaningful retail OBJECT at the same address')
        out.append(dict(disposition='uncredited_auxiliary_dependency', credited=False,
                        kind='compiler_named_object', section=section,
                        scaffold_name=name, scaffold_range=[lo, hi],
                        c_object_symbol=symbol_name, c_object_offset=object_offset,
                        c_object_section=section, c_object_section_size=csec.size,
                        c_object_section_alignment=csec.align, length=length,
                        binding={0:'LOCAL', 1:'GLOBAL', 2:'WEAK'}.get(c_symbol.bind, str(c_symbol.bind)),
                        type='OBJECT', payload_sha256=hashlib.sha256(raw).hexdigest(),
                        source_sha256=sha(source), object_sha256=sha(obj_path),
                        assembly_sha256=sha(asm_path), linked_elf_sha256=sha(linked_path),
                        original_elf_sha256=sha(ctx['orig']),
                        allocation_packet_sha256=sha(allocation_path)))
    return out


def uncredited_inline_asm_data_dependency(entry, record, section, span, items,
                                         unit_dir, raw_object_path, selected_object_path,
                                         linked_path, raw_elf, selected_elf,
                                         linked_elf, original_elf, ctx,
                                         allocation_path, source, asm_path,
                                         data_carve):
    """Pin one exact #APP data item through the raw/allocated/carved/link chain.

    This is a zero-credit disposition for assembler-authored bytes included in
    a C TU. It cannot establish C ownership or satisfy a recovered location.
    The exact map item, raw section offset, allocator output, carve sidecar,
    linked bytes, and the cc1 #APP directives must all agree.
    """
    lo, hi = span
    length = hi - lo
    requests=[r for r in record.get('uncredited_data_dependencies', [])
              if r.get('name') and r.get('section')==section and
              int(r.get('address','0'),16)==lo and int(r.get('size',0))==length and
              r.get('credited') is False and r.get('source',{}).get('inside_app') is True]
    require(len(requests)==1,
            entry['tu']+': unclaimed #APP span lacks one exact immutable dependency disposition')
    request=requests[0]
    require(length > 0, entry['tu'] + ': empty uncredited inline-assembly span')
    require(not any(x.get('section') == section and
                    lo <= int(x.get('address', '0'), 16) < hi
                    for x in record.get('recovered_location_candidates', [])),
            entry['tu'] + ': inline-assembly dependency overlaps a claimed C location')
    map_items = [x for x in items if x['start'] == lo and x['end'] == hi]
    require(len(map_items) == 1,
            entry['tu'] + ': inline-assembly dependency lacks one exact scaffold item')

    carved_sidecar = Path(str(selected_object_path) + '.json')
    require(carved_sidecar.is_file(),
            entry['tu'] + ': inline-assembly dependency lacks current carve sidecar')
    require(request['source']['path']==str(source) and request['source']['sha256']==sha(source) and
            request['source']['assembly_path']==str(asm_path) and
            request['source']['assembly_sha256']==sha(asm_path),
            entry['tu']+': dependency source/cc1 assembly pins are stale')
    require(request['raw_object']['path']==str(raw_object_path) and
            request['raw_object']['sha256']==sha(raw_object_path) and
            request['raw_object']['section']==section and
            int(request['raw_object']['offset'],0)>=0 and
            int(request['raw_object']['size'])==length,
            entry['tu']+': dependency raw object pin or section extent differs')
    carved_report = json.loads(carved_sidecar.read_text())
    require(carved_report.get('out') == selected_object_path.relative_to(unit_dir).as_posix(),
            entry['tu'] + ': carve sidecar selects a different object')
    pieces = carved_report.get('sections', {}).get(section, {}).get('pieces', [])
    matching = []
    for piece in pieces:
        va = int(piece['va'], 0)
        off = int(piece['offset'], 0)
        size = int(piece['length'], 0)
        if va == lo and size == length:
            matching.append((piece, off))
    require(len(matching) == 1,
            entry['tu'] + ': carve sidecar has no unique exact raw-offset mapping')
    piece, object_offset = matching[0]
    allocated_path = selected_object_path.with_name(
        selected_object_path.name.replace('.allocated.carved.o', '.allocated.o'))
    require(allocated_path.is_file(),
            entry['tu'] + ': inline-assembly dependency lacks allocated input')
    allocation_report_path = Path(str(allocated_path) + '.report.json')
    require(allocation_report_path.is_file(),
            entry['tu'] + ': inline-assembly dependency lacks allocation report')
    allocation_report = json.loads(allocation_report_path.read_text())
    require(allocation_report.get('input', {}).get('path') == str(raw_object_path) and
            allocation_report.get('input', {}).get('sha256') == sha(raw_object_path) and
            allocation_report.get('output', {}).get('path') == str(allocated_path) and
            allocation_report.get('output', {}).get('sha256') == sha(allocated_path),
            entry['tu'] + ': allocator report does not bind current raw/allocated objects')
    # Older immutable records sometimes preserve the pre-grouping build-root
    # spelling. Bind that provenance by exact artifact basenames and hashes;
    # the actual paths used below are independently derived from the current
    # TU linker script and selected carve sidecar.
    require(Path(request['allocation']['report_path']).name==allocation_report_path.name and
            request['allocation']['report_sha256']==sha(allocation_report_path) and
            request['allocation']['input_sha256']==sha(raw_object_path) and
            Path(request['allocation']['output_path']).name==allocated_path.name and
            request['allocation']['output_sha256']==sha(allocated_path),
            entry['tu']+': dependency allocation report pins differ')
    require(carved_report.get('object') == allocated_path.relative_to(unit_dir).as_posix(),
            entry['tu'] + ': carve sidecar does not consume exact allocated object')
    require(Path(request['carve']['report_path']).name==carved_sidecar.name and
            request['carve']['report_sha256']==sha(carved_sidecar) and
            Path(request['carve']['input_path']).name==allocated_path.name and
            request['carve']['input_sha256']==sha(allocated_path) and
            Path(request['carve']['output_path']).name==selected_object_path.name and
            request['carve']['output_sha256']==sha(selected_object_path) and
            request['carve']['input_section']==section and
            int(request['carve']['input_offset'],0)==object_offset and
            int(request['carve']['length'],0)==length and
            request['carve']['output_section']==piece['section'] and
            int(request['carve']['output_offset'],0)==0 and
            int(request['carve']['address'],0)==lo and int(request['carve']['end'],0)==hi,
            entry['tu']+': dependency raw-to-carved mapping pins differ')

    raw_section = next((s for s in raw_elf.sections if s.name == section and s.size), None)
    alloc_elf = Elf(allocated_path.read_bytes())
    alloc_section = next((s for s in alloc_elf.sections if s.name == section), None)
    selected_section = next((s for s in selected_elf.sections
                             if s.name == piece['section']), None)
    require(raw_section is not None and alloc_section is not None and selected_section is not None,
            entry['tu'] + ': raw, allocated, or carved section is absent')
    raw_bytes = raw_elf.section_bytes(raw_section)
    alloc_bytes = alloc_elf.section_bytes(alloc_section)
    selected_bytes = selected_elf.section_bytes(selected_section)
    require(object_offset + length <= len(raw_bytes) and
            object_offset + length <= len(alloc_bytes) and
            selected_section.size == length and
            selected_bytes == alloc_bytes[object_offset:object_offset+length] and
            raw_bytes[object_offset:object_offset+length] == selected_bytes,
            entry['tu'] + ': raw-to-allocated-to-carved section bytes differ')
    original = data_carve.va_bytes(original_elf, lo, length)
    final = data_carve.va_bytes(linked_elf, lo, length)
    require(len(original) == length and original == final == selected_bytes,
            entry['tu'] + ': inline-assembly dependency bytes differ from original/linked image')
    retail=Path(request['retail']['path'])
    require(retail.is_file() and request['retail']['sha256']==sha(retail),
            entry['tu']+': dependency retail image pin is stale')
    retail_offset=int(request['retail']['file_offset'],0)
    retail_bytes=retail.read_bytes()[retail_offset:retail_offset+length]
    require(len(retail_bytes)==length and retail_bytes==original and
            request['retail']['bytes_hex']==retail_bytes.hex() and
            request['retail']['bytes_sha256']==hashlib.sha256(retail_bytes).hexdigest(),
            entry['tu']+': dependency retail payload pin differs')
    require(request['linked_image']['elf_path']==str(linked_path) and
            request['linked_image']['elf_sha256']==sha(linked_path) and
            request['linked_image']['linked_gate_sha256']==sha(Path(request['linked_image']['linked_gate_path'])) and
            request['linked_image']['whole_file_compare_sha256']==sha(Path(request['linked_image']['whole_file_compare_path'])) and
            request['linked_image']['credited'] is False,
            entry['tu']+': dependency linked image/gate pins are stale or credited')

    # Require an exact local label and emitting directive within a #APP block.
    # Parsing is intentionally limited to ASCII literals and does not confer C
    # data ownership; it only identifies the source of this retained dependency.
    asm = asm_path.read_text(errors='surrogateescape')
    label = None
    app = False
    active_section = None
    literal_bytes = bytearray()
    label_name = None
    for line in asm.splitlines():
        text = line.strip()
        if text == '#APP':
            app = True
            active_section = None
            label_name = None
            literal_bytes = bytearray()
            continue
        if text == '#NO_APP':
            if app and label_name and bytes(literal_bytes) == raw_bytes[object_offset:object_offset+length]:
                label = label_name
                break
            app = False
            active_section = None
            continue
        if not app:
            continue
        section_match = re.match(r'^\.(?:section\s+([^,\s]+)|(rdata|rodata))\b', text)
        if section_match:
            active_section = (section_match.group(1) or
                              ('.rdata' if section_match.group(2) == 'rdata' else '.rodata'))
            continue
        if active_section not in (section, '.rdata'):
            continue
        label_match = re.match(r'^([$A-Za-z_.$][\w.$]*):\s*$', text)
        if label_match:
            if label_name and bytes(literal_bytes) == raw_bytes[object_offset:object_offset+length]:
                label = label_name
                break
            label_name = label_match.group(1)
            literal_bytes = bytearray()
            continue
        if label_name:
            ascii_match = re.match(r'^\.ascii\s+(.+?)\s*(?:#.*)?$', text)
            if ascii_match:
                try:
                    import ast
                    decoded = ast.literal_eval(ascii_match.group(1))
                except (ValueError, SyntaxError):
                    continue
                if isinstance(decoded, str):
                    literal_bytes.extend(decoded.encode('latin1'))
                elif isinstance(decoded, bytes):
                    literal_bytes.extend(decoded)
                continue
            space_match = re.match(r'^\.space\s+(\d+)', text)
            if space_match:
                literal_bytes.extend(bytes(int(space_match.group(1))))
                continue
    require(label is not None,
            entry['tu'] + ': exact mapped bytes are not emitted by a #APP data item')
    require(label==request['name'] and request['source']['assembly_line_start'] <= len(asm.splitlines()) and
            request['source']['assembly_line_end'] <= len(asm.splitlines()) and
            request['source']['assembly_line_start'] < request['source']['assembly_line_end'] and
            request['source']['inside_app'] is True,
            entry['tu']+': dependency #APP source span does not match cc1 evidence')

    return dict(disposition='uncredited_scaffold_dependency', credited=False,
                kind='inline_assembly_data', section=section,
                scaffold_name=map_items[0]['name'], scaffold_range=[lo, hi],
                compiler_assembly_label=label, c_object_offset=object_offset,
                c_object_section=section, length=length,
                payload_sha256=hashlib.sha256(raw_bytes[object_offset:object_offset+length]).hexdigest(),
                source_sha256=sha(source), raw_object_sha256=sha(raw_object_path),
                allocation_report_path=str(allocation_report_path),
                allocation_report_sha256=sha(allocation_report_path),
                allocated_object_sha256=sha(allocated_path),
                carve_sidecar_path=str(carved_sidecar), carve_sidecar_sha256=sha(carved_sidecar),
                selected_object_path=str(selected_object_path), selected_object_sha256=sha(selected_object_path),
                selected_section=piece['section'], selected_section_offset=0,
                assembly_sha256=sha(asm_path), linked_elf_sha256=sha(linked_path),
                original_elf_sha256=sha(ctx['orig']))


_REPRODUCED_CARVES = {}


def original_string_item_tail(ctx, data_carve, section, lo, cut, hi,
                              raw_elf, raw_section, offset, original_elf):
    """Prove an uncredited zero tail inside one original string item.

    This closed route accepts an exact terminated string followed only by
    unlabelled empty-string/alignment directives. Arbitrary zero data is not
    padding evidence and cannot enter this route.
    """
    if not (lo < cut < hi and hi-cut < raw_section.align and hi % raw_section.align == 0):
        return None
    items = [item for item in data_carve.section_items(ctx, section)
             if item['start'] == lo and item['end'] == hi]
    if len(items) != 1 or len(items[0].get('labels') or []) != 1:
        return None
    item = items[0]
    directives = data_carve._data_directives(item)
    if len(directives) < 2 or directives[0][0] != lo or directives[1][0] != cut:
        return None
    lines = item['block']['lines']
    literals = []
    for address, _, index in directives:
        match = re.search(r'\*/\s*\.asciz\s+("(?:[^"\\]|\\.)*")\s*$', lines[index])
        if not match:
            return None
        try:
            literal = ast.literal_eval(match[1]).encode('latin1') + b'\0'
        except (SyntaxError, ValueError, UnicodeEncodeError, AttributeError):
            return None
        if address == lo:
            if address+len(literal) != cut:
                return None
        elif address < cut or literal != b'\0':
            return None
        literals.append(literal)
    prefix = raw_elf.section_bytes(raw_section)[offset:offset+cut-lo]
    if (prefix != literals[0] or data_carve.va_bytes(original_elf, lo, cut-lo) != prefix or
            data_carve.va_bytes(original_elf, cut, hi-cut) != bytes(hi-cut)):
        return None
    if any(cut <= s.value < hi and s.type not in (3, 4) and
           0 < s.shndx < len(original_elf.sections) and
           original_elf.sections[s.shndx].name == section for s in original_elf.symbols):
        return None
    raw_lo, raw_hi = offset+cut-lo, offset+hi-lo
    if any(s.shndx == raw_section.index and s.type not in (3, 4) and
           raw_lo <= s.value < raw_hi for s in raw_elf.symbols):
        return None
    if any(raw_lo < at+4 and at < raw_hi
           for at, _, _ in data_carve.read_relocations(raw_elf).get(raw_section.index, [])):
        return None
    for target in raw_elf.symbols:
        if target.shndx != raw_section.index or target.type != 3:
            continue
        if any(raw_lo <= target.value+ref['addend'] < raw_hi
               for ref in storage_relocations(raw_elf, target.index, None)):
            return None
    return dict(disposition='uncredited_original_item_tail', credited=False,
                kind='original_string_item_tail', section=section,
                scaffold_name=item['name'], original_item_range=[lo, hi],
                range=[cut, hi], compiler_range=[lo, cut],
                input_section=raw_section.name, input_offset=offset,
                input_alignment=raw_section.align,
                original_directive_boundaries=[address for address, _, _ in directives]+[hi],
                original_elf_sha256=sha(ctx['orig']),
                compiler_prefix_sha256=hashlib.sha256(prefix).hexdigest())


def exact_native_section_plan(entry, section, spans, unit_dir, raw_path,
                              selected_path, raw_elf, linked_elf, original_elf,
                              ctx, data_carve, source_section, preserved_carve_input=None):
    """Prove a complete named initialized section selected without splitting."""
    unit = entry['tu'].split('/')[0]
    require(unit == 'main' and source_section == section and len(spans) == 1 and
            (raw_path.resolve() == selected_path.resolve() or preserved_carve_input is not None) and
            linked_elf is not None,
            entry['tu'] + ': native unsplit plan is not the exact MAIN raw selected section')
    record_path = Path(entry['record'])
    require(sha(record_path) == entry['sha256'], entry['tu'] + ': native source record changed')
    record = json.loads(record_path.read_text())
    raw = record.get('compiler_object') or record.get('object') or {}
    require(Path(raw.get('path', '')).resolve() == raw_path.resolve() and
            raw.get('sha256') == sha(raw_path), entry['tu'] + ': native compiler object pin differs')
    assembly = Path(record['assembly']['path'])
    require(assembly.is_file() and sha(assembly) == record['assembly']['sha256'],
            entry['tu'] + ': native cc1 assembly pin differs')
    root = unit_dir.parents[1]
    derive_path = raw_path
    preserved = None
    if preserved_carve_input is not None:
        derive_path = preserved_carve_input
        native = record.get('object') or {}
        sidecar = Path(str(selected_path) + '.json')
        cache_key = (str(selected_path.resolve()), sha(raw_path), sha(derive_path), sha(selected_path))
        plans = _REPRODUCED_CARVES.get(cache_key)
        facts = next((t for t in json.loads((root / 'config/tu-build.json').read_text())['tus']
                      if t['id'] == entry['tu']), None)
        mapping = json.loads(sidecar.read_text()) if sidecar.is_file() else {}
        require(facts and facts.get('in_scope') and facts['unit'] == unit and
                facts['name'] == ctx['name'] and facts['path'] == f'src/{unit}/{ctx["name"]}.c' and
                Path(native.get('path', '')).resolve() == derive_path.resolve() and
                native.get('sha256') == sha(derive_path) and
                derive_path.relative_to(unit_dir).as_posix() == f'build/c/{ctx["name"]}.o' and
                selected_path.relative_to(unit_dir).as_posix() == f'build/c/{ctx["name"]}.carved.o' and
                mapping.get('object') == derive_path.relative_to(unit_dir).as_posix() and
                mapping.get('out') == selected_path.relative_to(unit_dir).as_posix() and
                mapping.get('tu') == entry['tu'] and plans and section not in plans and
                set(mapping.get('sections', {})) == set(plans) and section not in mapping['sections'],
                entry['tu'] + ': preserved initialized section lacks the complete native allocation/carve chain')
        def identity(elf, sec):
            symbols = sorted((s.name, s.value, s.size, s.bind, s.type, s.other)
                             for s in elf.symbols if s.shndx == sec.index and s.type not in (3, 4))
            relocations = []
            for rel in elf.sections:
                if rel.type not in (4, 9) or rel.info != sec.index:
                    continue
                stride = 12 if rel.type == 4 else 8
                require(rel.entsize == stride and rel.size % stride == 0,
                        entry['tu'] + ': preserved initialized relocation layout differs')
                for off in range(0, rel.size, stride):
                    position, info = struct.unpack_from('<II', elf.data, rel.offset + off)
                    require((info >> 8) < len(elf.symbols),
                            entry['tu'] + ': preserved initialized relocation has no symbol identity')
                    symbol = elf.symbols[info >> 8]
                    target = elf.sections[symbol.shndx].name if 0 < symbol.shndx < len(elf.sections) else symbol.shndx
                    addend = struct.unpack_from('<i', elf.data, rel.offset + off + 8)[0] if stride == 12 else None
                    relocations.append((rel.type, position, info & 255, addend, symbol.name,
                                        symbol.value, symbol.size, symbol.bind, symbol.type, symbol.other, target))
            return ((sec.name, sec.type, sec.flags, sec.addr, sec.size, sec.link, sec.info, sec.align, sec.entsize),
                    elf.section_bytes(sec), symbols, sorted(relocations))
        identities = []
        for path in (raw_path, derive_path, selected_path):
            elf = Elf(path.read_bytes())
            matches = [s for s in elf.sections if s.name == section]
            require(len(matches) == 1 and matches[0].type == 1 and matches[0].size > 0,
                    entry['tu'] + ': preserved sibling is not a unique initialized section')
            identities.append(identity(elf, matches[0]))
        require(identities[0] == identities[1] == identities[2],
                entry['tu'] + ': preserved initialized bytes, metadata, symbols or relocations changed')
        preserved = dict(schema='preserved-initialized-carve-section/1',
                         raw_object_sha256=sha(raw_path), native_object_sha256=sha(derive_path),
                         selected_object_sha256=sha(selected_path), sidecar_sha256=sha(sidecar),
                         complete_deterministic_carve=True)
    sources = [p for p in record['files'] if p.endswith('.c')]
    require(len(sources) == 1 and sources[0] == f'src/{unit}/{ctx["name"]}.c' and
            (preserved is not None or
             selected_path.relative_to(unit_dir).as_posix() == f'build/c/{ctx["name"]}.o') and
            sha(root / sources[0]) == record['files'][sources[0]]['sha256'],
            entry['tu'] + ': native source pin differs')
    raw_elf = Elf(raw_path.read_bytes())
    rawsec = next((s for s in raw_elf.sections if s.name == section), None)
    lo, hi = spans[0]
    require(rawsec is not None and rawsec.type == 1 and 0 < rawsec.size <= hi-lo and
            rawsec.size > 0 and any(p['start'] <= lo < hi <= p['end']
                                  for p in ctx['pieces'].get(section, [])),
            entry['tu'] + ': native plan does not cover exactly the original/raw initialized section')
    owners = sorted((s for s in raw_elf.symbols if s.shndx == rawsec.index and s.type not in (3, 4)),
                    key=lambda s: s.value)
    emitted = emitted_data(assembly.read_text(errors='surrogateescape'))
    cursor = 0
    for owner in owners:
        require(owner.type == 1 and owner.size > 0 and owner.value == cursor and owner.name in emitted,
                entry['tu'] + ': native initialized owners do not tile cc1 storage')
        if preserved is not None:
            originals = [s for s in original_elf.symbols if
                         (s.name, s.value, s.size, s.bind, s.type, s.other) ==
                         (owner.name, lo+owner.value, owner.size, owner.bind, owner.type, owner.other) and
                         0 < s.shndx < len(original_elf.sections) and
                         original_elf.sections[s.shndx].name == section]
            require(len(originals) == 1,
                    entry['tu'] + ': preserved initialized owner lacks exact original named storage')
        cursor += owner.size
    require(cursor == rawsec.size and owners,
            entry['tu'] + ': native initialized section has anonymous, padding or unowned bytes')
    # This is the same raw-emission classification used by the caller; actual
    # linker placement must not hide the compiler's declarations from derive.
    previous = data_carve.tu_context
    def emission_context(*args, **kwargs):
        context = previous(*args, **kwargs)
        context['placed'] = set()
        return context
    data_carve.tu_context = emission_context
    try:
        derived = data_carve.derive(root, unit, unit_dir, entry['tu'], root / sources[0], derive_path)
    finally:
        data_carve.tu_context = previous
    declared = data_carve.load_registry(root)['tus'].get(entry['tu'], {}).get(section, [])
    padding = []
    physical = data_carve.absorb_padding(entry['tu'], section, declared,
                                         data_carve.section_items(ctx, section))
    require(len(declared) == 1 and tuple(map(data_carve.hx, declared[0]['range'])) ==
            (lo, lo+rawsec.size) and tuple(map(data_carve.hx, physical[0]['range'])) == (lo, hi),
            entry['tu'] + ': native logical/physical spans differ from the original guarded partition')
    if rawsec.size < hi-lo:
        padding = physical[0].get('absorbed_padding') or []
        require(padding == [[data_carve.h8(lo+rawsec.size), data_carve.h8(hi)]] and
                data_carve.va_bytes(original_elf, lo+rawsec.size, hi-lo-rawsec.size) ==
                bytes(hi-lo-rawsec.size) and not any(
                    lo+rawsec.size <= s.value < hi and s.type not in (3, 4) and
                    0 < s.shndx < len(original_elf.sections) and
                    original_elf.sections[s.shndx].name == section for s in original_elf.symbols),
                entry['tu'] + ': native tail lacks exact uncredited original alignment-padding proof')
    require(not [p for p in derived['problems'] if not is_nobits_derivation_problem(p)] and
            data_carve.normalize_runs({section: derived['runs'].get(section)}) ==
            data_carve.normalize_runs({section: declared}) and
            (derived['sections'].get(section, {}).get('bytes') or {}).get('checked') is True and
            (derived['sections'].get(section, {}).get('bytes') or {}).get('equal') is True and
            section not in data_carve.split_plans(root, unit, unit_dir, entry['tu'], Elf(derive_path.read_bytes())),
            entry['tu'] + ': native data differs from the complete guarded raw run')
    ld_path = unit_dir / 'main.rom.ld'
    selected_rel = selected_path.relative_to(unit_dir).as_posix()
    selector = re.escape(selected_rel) + r'\(' + re.escape(section) + r'\);'
    ldtext = ld_path.read_text()
    placements = re.findall(r'\.\s*=\s*(0x[0-9a-fA-F]+);\s*' + selector, ldtext)
    marker = '__c_' + ctx['name'] + '_' + section.lstrip('.').replace('.', '_')
    native_marker = re.findall(re.escape(marker) + r'\s*=\s*\.;\s*' + selector, ldtext)
    linked_markers = [s for s in linked_elf.symbols if s.name == marker and s.value == lo and s.type == 0]
    require((len(placements) == 1 and int(placements[0], 16) == lo) or
            (len(native_marker) == 1 and len(linked_markers) == 1),
            entry['tu'] + ': native linker selection/address is not exact')
    for owner in owners:
        matches = [s for s in linked_elf.symbols if
                   (s.name, s.value, s.size, s.bind, s.type) ==
                   (owner.name, lo+owner.value, owner.size, owner.bind, owner.type) and
                   0 < s.shndx < len(linked_elf.sections) and
                   linked_elf.sections[s.shndx].name == section]
        require(len(matches) == 1, entry['tu'] + ': native named compiler object was not selected exactly')
    require(data_carve.va_bytes(original_elf, lo, hi-lo) ==
            data_carve.va_bytes(linked_elf, lo, hi-lo),
            entry['tu'] + ': native initialized final bytes differ from original')
    return dict(pieces=[dict(lo=lo, hi=hi, offset=0, length=rawsec.size,
                            input_section=section, selected_section=section, selected_offset=0)],
                native_unsplit=dict(schema='native-unsplit-initialized-section/1',
                    input_section=section, logical_range=[lo, lo+rawsec.size], physical_range=[lo, hi],
                    credited_bytes=rawsec.size, uncredited_padding=padding,
                    object_sha256=sha(raw_path), assembly_sha256=sha(assembly),
                    source_sha256=sha(root / sources[0]), linker_sha256=sha(ld_path),
                    original_elf_sha256=hashlib.sha256(original_elf.data).hexdigest(),
                    linked_elf_sha256=hashlib.sha256(linked_elf.data).hexdigest(),
                    **({'preserved_carve': preserved} if preserved is not None else {})))


def exact_carved_section_plan(entry, section, spans, unit_dir, raw_path,
                              selected_path, raw_elf, selected_elf,
                              linked_elf, original_elf, linked_path,
                              ctx, data_carve, source_section=None,
                              compiler_allocation_proof=None):
    """Translate raw section runs through the exact current allocator/carve map."""
    sidecar=Path(str(selected_path)+'.json')
    if not sidecar.is_file() and raw_path.resolve() == selected_path.resolve():
        return exact_native_section_plan(entry, section, spans, unit_dir, raw_path,
                                         selected_path, raw_elf, linked_elf, original_elf,
                                         ctx, data_carve, source_section or section)
    require(sidecar.is_file(),entry['tu']+': raw/carved fallback lacks carve sidecar for '+str(selected_path))
    carve=json.loads(sidecar.read_text())
    input_section=source_section or section
    require(carve.get('out')==selected_path.relative_to(unit_dir).as_posix(),
            entry['tu']+': carve sidecar does not select current object')
    direct_raw_carve=(selected_path.name.endswith('.carved.o') and
                      not selected_path.name.endswith('.allocated.carved.o'))
    allocated=None
    if direct_raw_carve:
        raw_rel=carve.get('object')
        current_raw_path=unit_dir/raw_rel if isinstance(raw_rel,str) else Path('/__missing_raw_carve_object__')
        require(current_raw_path.is_file(),entry['tu']+': direct carve input is absent')
        if current_raw_path.resolve()!=raw_path.resolve():
            proof=compiler_allocation_proof or {}
            verification=Path(proof.get('verification_path','/__missing_allocation_verification__'))
            require(entry['tu'].startswith('main/') and
                    Path(proof.get('raw_object_path','/__missing_raw__')).resolve()==raw_path.resolve() and
                    proof.get('raw_object_sha256')==sha(raw_path) and
                    Path(proof.get('allocated_object_path','/__missing_allocated__')).resolve()==current_raw_path.resolve() and
                    proof.get('allocated_object_sha256')==sha(current_raw_path) and
                    verification.is_file() and proof.get('verification_sha256')==sha(verification)==sha(current_raw_path),
                    entry['tu']+': direct carve input lacks exact current compiler-to-COMMON allocation proof')
        else:
            require(sha(current_raw_path)==sha(raw_path),
                    entry['tu']+': direct carve sidecar does not bind the packet-pinned raw compiler object')
        carve_input_elf=Elf(current_raw_path.read_bytes())
        unit=entry['tu'].split('/')[0]
        private_root=unit_dir.parents[1]
        cache_key=(str(selected_path.resolve()),sha(raw_path),sha(current_raw_path),sha(selected_path))
        if cache_key not in _REPRODUCED_CARVES:
            plans=data_carve.split_plans(private_root,unit,unit_dir,entry['tu'],carve_input_elf)
            expected=data_carve.split_object(current_raw_path.read_bytes(),
                                             {sec:plan['pieces'] for sec,plan in plans.items()})
            require(selected_path.read_bytes()==expected,
                    entry['tu']+': direct carve object differs from deterministic raw-section split (including relocations)')
            _REPRODUCED_CARVES[cache_key]=plans
        plans=_REPRODUCED_CARVES[cache_key]
        if input_section not in plans:
            return exact_native_section_plan(entry, section, spans, unit_dir, raw_path,
                                             selected_path, raw_elf, linked_elf, original_elf,
                                             ctx, data_carve, input_section,
                                             preserved_carve_input=current_raw_path)
        require(input_section in plans,entry['tu']+': direct carve has no guarded split plan for '+input_section)
        expected_pieces=[dict(section=data_carve.split_section_name(input_section,index),
                              offset=hex(piece['offset']),length=hex(piece['length']),
                              va=data_carve.h8(piece['lo']),run_end=data_carve.h8(piece['hi']))
                         for index,piece in enumerate(plans[input_section]['pieces'])]
        require(carve.get('sections',{}).get(input_section,{}).get('pieces')==expected_pieces,
                entry['tu']+': direct carve sidecar section plan differs from the guarded registry')
    else:
        allocated=selected_path.with_name(selected_path.name.replace('.allocated.carved.o','.allocated.o'))
        alloc_report=Path(str(allocated)+'.report.json')
        require(allocated.is_file() and alloc_report.is_file(),
                entry['tu']+': raw/carved fallback lacks allocated object proof')
        ar=json.loads(alloc_report.read_text())
        require(ar.get('input',{}).get('path')==str(raw_path) and
                ar.get('input',{}).get('sha256')==sha(raw_path) and
                ar.get('output',{}).get('path')==str(allocated) and
                ar.get('output',{}).get('sha256')==sha(allocated) and
                carve.get('object')==allocated.relative_to(unit_dir).as_posix(),
                entry['tu']+': allocator/carve chain does not bind current raw object')
    rawsec=next((s for s in raw_elf.sections if s.name==input_section and s.size),None)
    alloc_elf=carve_input_elf if direct_raw_carve else Elf(allocated.read_bytes())
    allocsec=next((s for s in alloc_elf.sections if s.name==input_section),None)
    require(rawsec is not None and allocsec is not None,
            entry['tu']+': raw or allocated initialized section is absent')
    rawbytes=raw_elf.section_bytes(rawsec)
    allocbytes=alloc_elf.section_bytes(allocsec)
    require(rawbytes==allocbytes and rawsec.size==allocsec.size and
            rawsec.type==allocsec.type and rawsec.flags==allocsec.flags and rawsec.align==allocsec.align,
            entry['tu']+': allocator changed initialized section bytes or identity')
    if direct_raw_carve and current_raw_path.resolve()!=raw_path.resolve():
        def section_symbols(elf, sec):
            return sorted((s.name,s.value,s.size,s.bind,s.type) for s in elf.symbols
                          if s.shndx==sec.index and s.name and s.type not in (3,4))
        require(section_symbols(raw_elf,rawsec)==section_symbols(alloc_elf,allocsec),
                entry['tu']+': COMMON allocation changed initialized symbol identities/offsets')
    if not direct_raw_carve:
        cache_key=(str(selected_path.resolve()),sha(allocated),sha(selected_path))
        if cache_key not in _REPRODUCED_CARVES:
            plans=data_carve.split_plans(unit_dir.parents[1],entry['tu'].split('/')[0],unit_dir,entry['tu'],alloc_elf)
            expected=data_carve.split_object(allocated.read_bytes(),{s:p['pieces'] for s,p in plans.items()})
            require(selected_path.read_bytes()==expected,
                    entry['tu']+': allocated carve does not reproduce the complete guarded split including relocations')
            _REPRODUCED_CARVES[cache_key]=plans
    pieces=carve.get('sections',{}).get(input_section,{}).get('pieces',[])
    result=[]
    unit=entry['tu'].split('/')[0]
    ld_path=unit_dir/('main.rom.ld' if unit=='main' else unit+'.ld')
    require(ld_path.is_file(),entry['tu']+': carved fallback lacks current linker script')
    ldtext=ld_path.read_text()
    selected_rel=selected_path.relative_to(unit_dir).as_posix()
    declared_runs=data_carve.load_registry(unit_dir.parents[1]).get('tus',{}).get(entry['tu'],{}).get(section,[])
    if direct_raw_carve:
        # The split uses physical runs extended over original alignment pads.
        # Reproduce that extension; registry/generated extents remain C-only.
        declared_runs=data_carve.absorb_padding(entry['tu'],section,declared_runs,
                                                data_carve.section_items(ctx,section))
    for lo,hi in spans:
        item_tail = None
        matches=[p for p in pieces if int(p['va'],0)==lo and int(p['run_end'],0)==hi]
        require(len(matches)==1,entry['tu']+': carve sidecar has no unique run-to-VA mapping')
        p=matches[0]
        off=int(p['offset'],0); size=int(p['length'],0)
        require(0<size<=hi-lo and off+size<=len(rawbytes),
                entry['tu']+': carve mapping extent differs from raw section run')
        if direct_raw_carve:
            run=next((r for r in declared_runs
                      if data_carve.declared_runs(entry['tu'],section,[r])==[(lo,hi)]),None)
            generated=run.get('generated') if run else None
            if generated:
                require(data_carve.hx(generated[0])==lo and data_carve.hx(generated[1])==lo+size,
                        entry['tu']+': direct carve extent differs from the pinned compiler-generated byte range')
                if size<hi-lo:
                    padding=[tuple(map(data_carve.hx,span)) for span in run.get('absorbed_padding') or []]
                    if padding != [(lo+size,hi)]:
                        item_tail = original_string_item_tail(
                            ctx,data_carve,section,lo,lo+size,hi,raw_elf,rawsec,off,original_elf)
                    require((padding==[(lo+size,hi)] or item_tail is not None) and
                            data_carve.va_bytes(original_elf,lo+size,hi-lo-size)==bytes(hi-lo-size),
                            entry['tu']+': direct carve gap lacks exact original alignment-padding proof')
            else:
                require(size==hi-lo,
                        entry['tu']+': direct carve leaves bytes outside a pinned generated extent')
        else:
            require(size==hi-lo,
                    entry['tu']+': allocated carve extent differs from raw section run')
        selectedsec=next((s for s in selected_elf.sections if s.name==p['section']),None)
        require(selectedsec is not None and selectedsec.size==size,
                entry['tu']+': selected carve section extent differs from the exact guarded split')
        require(re.search(re.escape(selected_rel)+r'\('+re.escape(p['section'])+r'\);',ldtext),
                entry['tu']+': linker script does not select exact carved section')
        original=data_carve.va_bytes(original_elf,lo,hi-lo)
        final=data_carve.va_bytes(linked_elf,lo,hi-lo)
        require(len(original)==hi-lo and original==final,
                entry['tu']+': carved fallback section differs from original/final linked bytes')
        result.append(dict(lo=lo,hi=hi,offset=off,length=size,
                           input_section=input_section,
                           selected_section=p['section'],selected_offset=0,
                           **({'uncredited_original_item_tail':item_tail} if item_tail else {})))
    return dict(pieces=result)


def uncredited_linker_map_alias(root, entry, alias_section, alias, owner,
                                packet_tu, storage_rows, original_elf,
                                linked_elf, original_section_ranges, ctx,
                                data_carve, source, asm_path, linked_path,
                                allocation_path, object_elf=None):
    """Validate a map-only linker name as a zero-credit dependency of C storage."""
    name=alias['original_name']
    va=int(alias['address'],16)
    owner_va=int(owner['address'],16)
    owner_size=int(owner['size'])
    owners=[row for row in storage_rows if row['name']==owner['name'] and
            int(row['address'],16)==owner_va and row['size']==owner_size and
            row['section']==alias_section and row.get('linked') is True]
    require(len(owners)==1,
            entry['tu']+'/'+name+': map-only alias lacks one exact linked C owner')
    mapped=original_section_ranges.get(alias_section)
    require(mapped and mapped[0]<=owner_va<=va<owner_va+owner_size<=mapped[1],
            entry['tu']+'/'+name+': map-only alias is outside exact C owner/map span')
    packet_items=[x for x in packet_tu.get('data',[]) if x.get('section')==alias_section and
                  int(x.get('address','0'),16)==va and name in (x.get('names') or [])]
    packet_owner_items=[x for x in packet_tu.get('data',[]) if x.get('section')==alias_section and
                        int(x.get('address','0'),16)==owner_va and owner['name'] in (x.get('names') or [])]
    # Interior linker names need not have their own frozen allocation row: the
    # packet may claim the enclosing C owner at its base, while the original
    # scaffold supplies an interior label. Keep this dependency zero-credit.
    require(packet_items or (va > owner_va and packet_owner_items),
            entry['tu']+'/'+name+': map-only alias and its owner are absent from frozen original packet')
    items=[x for x in data_carve.section_items(ctx,alias_section)
           if x['start']==va and name in (x.get('labels') or [])]
    scaffold_path = None
    scaffold_sha256 = None
    original_alias = [s for s in original_elf.symbols if s.name == name and s.value == va
                      and 0 < s.shndx < len(original_elf.sections)
                      and original_elf.sections[s.shndx].name == alias_section]
    alias_identity_source = 'scaffold_label'
    if alias_section in ('.bss', '.sbss') and not items and original_alias:
        # MAIN's disassembler can spell a local retail label with its C-safe
        # name. Preserve the retail identity separately; the actual scaffold
        # item must still prove the same base and span as the verified owner.
        require(len(original_alias) == 1 and original_alias[0].type == 0 and
                original_alias[0].size == 0 and original_alias[0].bind == owners[0]['bind']
                and va == owner_va,
                entry['tu']+'/'+name+': renamed original storage alias identity differs')
        items = [x for x in data_carve.section_items(ctx, alias_section)
                 if x['start'] == va and owner['name'] in (x.get('labels') or [])]
        require(len(items) == 1,
                entry['tu']+'/'+name+': renamed original label has no exact owner scaffold item')
        pieces = [p for p in ctx['pieces'].get(alias_section, []) if p['start'] <= va < p['end']]
        require(len(pieces) == 1, entry['tu']+'/'+name+': ambiguous original storage piece')
        scaffold_path = ctx['path_of'](pieces[0], alias_section)
        scaffold_sha256 = sha(scaffold_path)
        alias_identity_source = 'original_elf_zero_size_label_and_c_named_scaffold_item'
    if alias_section in ('.bss', '.sbss') and not items:
        unit = ctx['orig'].stem.lower()
        scaffold_path = ctx['path_of'](
            {'file': f'asm/data/{unit}/{ctx["name"]}{alias_section}.s'}, alias_section)
        require(scaffold_path.is_file(),
                entry['tu']+'/'+name+': original BSS scaffold data file is absent')
        _, blocks = data_carve.parse_data_file(scaffold_path)
        for index, block in enumerate(blocks):
            if block['start'] != va or name not in block.get('labels', []):
                continue
            end = blocks[index + 1]['start'] if index + 1 < len(blocks) else mapped[1]
            items.append(dict(start=va, end=end, labels=block['labels'], block=block))
        scaffold_sha256 = sha(scaffold_path)
    require(len(items)==1,
            entry['tu']+'/'+name+': map-only alias lacks one exact scaffold item')
    item=items[0]
    extent=item['end']-va
    packet_size=next((int(x['original_symbol_size']) for x in packet_items
                      if x.get('original_symbol_size') is not None and
                      int(x['original_symbol_size'])>0),None)
    if packet_size is not None:
        extent=min(extent,packet_size)
    # A disassembler label's span may include alignment space after its C
    # owner. This dependency earns no credit: keep only the alias bytes inside
    # that proved owner; the remainder stays scaffold-owned. Meaningful ELF
    # OBJECT identities remain prohibited below rather than being truncated.
    scaffold_extent = extent
    extent=min(extent,owner_va+owner_size-va)
    require(extent>0 and va+extent<=owner_va+owner_size,
            entry['tu']+'/'+name+': exact map alias extent exceeds compiler owner')
    original_at=[s for s in original_elf.symbols if s.shndx not in (0,0xfff1) and
                 s.value==va and mapped[0]<=va<mapped[1]]
    meaningful=[s for s in original_at if s.type==1 and s.size>0]
    require(not meaningful,
            entry['tu']+'/'+name+': map-only dependency overlaps meaningful original OBJECT identity')
    source_member = None
    consumer_relocations = []
    if alias_section in ('.bss', '.sbss') and va > owner_va:
        text = source.read_text(errors='surrogateescape')
        array = source_storage_array_extent(text, owner['name'], owner_size)
        field = source_storage_member_extent(
            source_storage_layout_text(root, source), owner['name'], owner_size, va-owner_va)
        if array and (va-owner_va) % array['element_stride'] == 0:
            source_member = dict(array, kind='local_fixed_array_element',
                                 element_index=(va-owner_va)//array['element_stride'],
                                 owner_offset=va-owner_va, size=array['element_stride'])
        elif field:
            source_member = field
        require(source_member and source_member['size'] <= extent,
                entry['tu']+'/'+name+': map-only interior alias lacks a bounded source array/member witness')
        require(object_elf is not None,
                entry['tu']+'/'+name+': map-only interior alias lacks a current compiler object')
        original_references = data_carve.identifiers(
            path for folder in ctx['fdirs'] if folder.is_dir() for path in sorted(folder.glob('*.s')))
        owner_symbols = [s for s in object_elf.symbols if s.name == owner['name'] and
                         0 < s.shndx < len(object_elf.sections)]
        require(len(owner_symbols) == 1,
                entry['tu']+'/'+name+': map-only interior alias lacks a unique compiler owner')
        owner_symbol = owner_symbols[0]
        targets = [owner_symbol] + [s for s in object_elf.symbols
                                   if s.shndx == owner_symbol.shndx and s.type == 3]
        for target in targets:
            consumer_relocations.extend(dict(ref, owner_offset=ref['addend']+target.value-owner_symbol.value)
                                        for ref in storage_relocations(object_elf, target.index, None)
                                        if ref['addend']+target.value-owner_symbol.value == va-owner_va)
        for target in object_elf.symbols:
            if target.name == name and target.shndx == 0:
                consumer_relocations.extend(dict(ref, owner_offset=va-owner_va+ref['addend'], map_alias=name)
                                            for ref in storage_relocations(object_elf, target.index, None)
                                            if ref['addend'] == 0)
        require(name not in original_references or consumer_relocations,
                entry['tu']+'/'+name+': original alias consumer has no current relocation at the exact owner offset')
    linked_names=[s for s in linked_elf.symbols if s.name==name]
    if linked_names:
        require(len(linked_names)==1 and linked_names[0].value==va,
                entry['tu']+'/'+name+': current linked image has conflicting map alias identity')
        final=linked_names[0]
        require(final.shndx not in (0,),
                entry['tu']+'/'+name+': linked map alias is unresolved')
        linked_identity=dict(name=final.name,binding=final.bind,type=final.type,
                             size=final.size,section_index=final.shndx)
        alias_link_status='linked_symbol_at_exact_owner_offset'
    else:
        # A map label can remain absent from the linked symtab when the
        # selected C owner replaces the scaffold BSS object. The exact owner
        # symbol/extent and carve-to-VA proof above still establish this
        # uncredited alias as an address within C-owned storage; the complete
        # exact linked image proves all retained scaffold consumers. Never use
        # this path for a meaningful original OBJECT (rejected above).
        owner_type = owners[0]['type']
        if isinstance(owner_type, str):
            owner_type = {'NOTYPE': 0, 'OBJECT': 1, 'FUNC': 2, 'SECTION': 3,
                          'FILE': 4, 'COMMON': 5, 'TLS': 6}.get(owner_type, owner_type)
        linked_owner=[s for s in linked_elf.symbols if s.name==owner['name'] and
                      s.value==owner_va and s.bind==owners[0]['bind'] and
                      s.type==owner_type and s.shndx not in (0,0xfff1)]
        require(len(linked_owner)==1,
                entry['tu']+'/'+name+': absent alias has no unique linked C owner spanning its address')
        linked_identity=None
        alias_link_status='symbol_not_materialized_exact_linked_owner_span'
    same_name_original=[s for s in original_elf.symbols if s.name==name and s.value==va and
                        s.shndx not in (0,0xfff1)]
    return dict(disposition='uncredited_linker_dependency',credited=False,
                kind='map_only_storage_alias',section=alias_section,
                scaffold_name=name,address=f'0x{va:08X}',range=[va,va+extent],
                original_scaffold_range=[va,va+scaffold_extent],
                scaffold_only_tail=([va+extent,va+scaffold_extent] if extent<scaffold_extent else None),
                owner_name=owner['name'],owner_address=f'0x{owner_va:08X}',
                owner_size=owner_size,owner_offset=va-owner_va,
                owner_binding=owners[0]['bind'],owner_type=owners[0]['type'],
                owner_relocation_count=owners[0]['relocation_count'],
                owner_object_section=owners[0]['section'],
                owner_tu_section_range=owners[0]['tu_section_range'],
                original_identities=[dict(name=s.name,binding=s.bind,type=s.type,size=s.size)
                                     for s in original_at],
                original_same_name_identities=[dict(name=s.name,binding=s.bind,type=s.type,size=s.size)
                                               for s in same_name_original],
                scaffold_path=str(scaffold_path) if scaffold_path else None,
                scaffold_sha256=scaffold_sha256,
                alias_identity_source=alias_identity_source,
                **({'source_member': source_member} if source_member else {}),
                **({'consumer_relocations': consumer_relocations} if consumer_relocations else {}),
                original_elf_sha256=sha(ctx['orig']),
                linked_identity=linked_identity,linked_alias_status=alias_link_status,
                source_sha256=sha(source),assembly_sha256=sha(asm_path),
                linked_elf_sha256=sha(linked_path),allocation_packet_sha256=sha(allocation_path),
                owner_object_sha256=owners[0].get('object_sha256'))


def validate_initialized_storage_owner(entry, location, section, piece, own,
                                       object_elf, linked_elf, original_elf,
                                       original_section_ranges, emitted, object_path,
                                       linked_path, packet_sha, source_sha,
                                       assembly_sha):
    """Bind an interior map label to an exact, meaningful C/original owner.

    This permits an original zero-size/address alias inside a real object. It
    does not enlarge, merge or rename meaningful original OBJECT records: the
    owner at its base must retain the original address, binding, type and full
    size exactly.
    """
    owner = location.get('storage_owner')
    if not owner:
        return None
    name = owner.get('name')
    address = owner.get('address')
    va = int(address, 16) if isinstance(address, str) else int(address)
    size = owner.get('size')
    size = int(size, 0) if isinstance(size, str) else int(size)
    require(name and size > 0, entry['tu'] + ': storage_owner has no exact name/size')
    offset = piece['offset'] + va - piece['lo']
    candidates = [sym for sym in own if sym.name == name and sym.value == offset and
                  sym.size == size and sym.type == 1]
    require(len(candidates) == 1,
            entry['tu'] + '/' + name + ': storage_owner lacks one exact compiler OBJECT at its base')
    owner_sym = candidates[0]
    require(name in emitted,
            entry['tu'] + '/' + name + ': storage_owner was not emitted by cc1 C')
    require(va <= int(location['address'], 16) < va + size,
            entry['tu'] + '/' + name + ': interior map label is outside its declared C owner')
    mapped = original_section_ranges.get(section)
    require(mapped and mapped[0] <= va < va + size <= mapped[1],
            entry['tu'] + '/' + name + ': storage_owner escapes original TU section map')
    retail_owners = [sym for sym in original_elf.symbols if sym.shndx not in (0, 0xfff1) and
                     sym.value == va and sym.type == 1 and sym.size > 0 and
                     mapped[0] <= sym.value < mapped[1]]
    require(retail_owners and all((sym.bind, sym.type, sym.size) ==
                                  (owner_sym.bind, owner_sym.type, owner_sym.size)
                                  for sym in retail_owners),
            entry['tu'] + '/' + name + ': storage_owner does not exactly preserve a meaningful original owner OBJECT')
    linked_owners = [sym for sym in linked_elf.symbols if sym.shndx not in (0, 0xfff1) and
                     sym.name == name and sym.value == va and sym.size == size and
                     sym.type == owner_sym.type and sym.bind == owner_sym.bind]
    require(len(linked_owners) == 1,
            entry['tu'] + '/' + name + ': linked storage_owner identity differs')
    compiler_bytes = object_elf.section_bytes(object_elf.sections[owner_sym.shndx])[offset:offset + size]
    require(len(compiler_bytes) == size,
            entry['tu'] + '/' + name + ': compiler owner extent exceeds its input section')
    require(data_carve.va_bytes(linked_elf, va, size) == data_carve.va_bytes(original_elf, va, size),
            entry['tu'] + '/' + name + ': linked owner bytes differ from original initialized object')
    return dict(name=name, address=f'0x{va:08X}', size=size, section=section,
                c_object_offset=offset, binding=('LOCAL' if owner_sym.bind == 0 else
                                                 'GLOBAL' if owner_sym.bind == 1 else str(owner_sym.bind)),
                type='OBJECT', object_sha256=sha(object_path),
                linked_elf_sha256=sha(linked_path), packet_sha256=packet_sha,
                source_sha256=source_sha, assembly_sha256=assembly_sha,
                bytes_evidence='compiler-owned run comparison plus exact final link and whole-file gate')


def initialized_storage_owner_aliases(entry, record, location, packet_tu, items,
                                      section, piece, owner_symbol, object_elf,
                                      linked_elf, original_elf, original_section_ranges,
                                      emitted, object_path, linked_path, packet_sha,
                                      source_sha, assembly_sha):
    """Prove one initialized C aggregate contains adjacent retail objects.

    This path is deliberately narrower than ordinary same-address aliases:
    each packet location must name one exact meaningful retail OBJECT, the
    source record must name the single enclosing C object, and those original
    OBJECT spans must tile the C object's complete extent. The C symbol remains
    at its actual base; interior aliases are reported separately.
    """
    owner = location.get('storage_owner')
    require(isinstance(owner, dict) and owner.get('name') == owner_symbol.name,
            entry['tu'] + ': aggregate alias lacks an explicit storage_owner naming the C object')
    owner_address = owner.get('address')
    owner_va = int(owner_address, 16) if isinstance(owner_address, str) else int(owner_address)
    owner_size = owner.get('size')
    owner_size = int(owner_size, 0) if isinstance(owner_size, str) else int(owner_size)
    start_va = piece['lo'] + owner_symbol.value - piece['offset']
    require(owner_va == start_va and owner_size == owner_symbol.size and owner_size > 0,
            entry['tu'] + '/' + owner_symbol.name + ': storage_owner base or extent differs from compiler OBJECT')
    require(owner_symbol.type == 1 and owner_symbol.name in emitted,
            entry['tu'] + '/' + owner_symbol.name + ': aggregate owner is not a compiler-emitted OBJECT')
    require(start_va >= piece['lo'] and start_va + owner_size <= piece['lo'] + piece['length'],
            entry['tu'] + '/' + owner_symbol.name + ': aggregate owner escapes its verified compiler split piece')
    original_map = original_section_ranges.get(section)
    require(original_map and original_map[0] <= start_va < start_va + owner_size <= original_map[1],
            entry['tu'] + '/' + owner_symbol.name + ': aggregate owner escapes original TU section map')
    owner_rows = []
    for candidate in record.get('recovered_location_candidates', []):
        row_owner = candidate.get('storage_owner') or {}
        row_va = int(candidate.get('address', '0'), 16)
        row_owner_va = row_owner.get('address')
        row_owner_va = int(row_owner_va, 16) if isinstance(row_owner_va, str) else row_owner_va
        row_owner_size = row_owner.get('size')
        row_owner_size = int(row_owner_size, 0) if isinstance(row_owner_size, str) else row_owner_size
        if (candidate.get('section') == section and row_owner.get('name') == owner_symbol.name
                and row_owner_va == owner_va and row_owner_size == owner_size):
            owner_rows.append(candidate)
    require(owner_rows, entry['tu'] + '/' + owner_symbol.name + ': no candidate locations declare this aggregate owner')
    intervals, aliases = [], []
    for candidate in owner_rows:
        va = int(candidate['address'], 16)
        names = set(candidate.get('names') or [])
        packet_matches = [item for item in packet_tu.get('data', [])
                          if item.get('section') == section and int(item.get('address', '0'), 16) == va
                          and names.intersection(item.get('names') or [])]
        require(len(packet_matches) == 1,
                entry['tu'] + '/' + candidate['address'] + ': aggregate alias is not one exact packet item')
        original_objects = [sym for sym in original_elf.symbols
                            if sym.shndx not in (0, 0xfff1) and sym.value == va
                            and sym.type == 1 and sym.size > 0
                            and original_map[0] <= sym.value < original_map[1]]
        require(len(original_objects) == 1,
                entry['tu'] + '/' + candidate['address'] + ': aggregate alias lacks one meaningful retail OBJECT')
        original = original_objects[0]
        item = packet_matches[0]
        if item.get('original_symbol_size') is not None:
            require(int(item['original_symbol_size']) == original.size,
                    entry['tu'] + '/' + candidate['address'] + ': packet object extent differs from retail OBJECT')
        require((owner_symbol.bind, owner_symbol.type) == (original.bind, original.type),
                entry['tu'] + '/' + candidate['address'] + ': aggregate owner binding/type differs from retail alias')
        require(owner_va <= va and va + original.size <= owner_va + owner_size,
                entry['tu'] + '/' + candidate['address'] + ': retail alias escapes explicit C aggregate owner')
        explicit_offset = candidate.get('owner_offset', owner.get('offset'))
        if explicit_offset is not None:
            explicit_offset = int(explicit_offset, 0) if isinstance(explicit_offset, str) else int(explicit_offset)
            require(explicit_offset == va - owner_va,
                    entry['tu'] + '/' + candidate['address'] + ': recorded owner_offset disagrees with exact addresses')
        intervals.append((va, va + original.size))
        aliases.append(dict(section=section, original_name=original.name,
                            address=f'0x{va:08X}', size=original.size,
                            binding=('LOCAL' if original.bind == 0 else 'GLOBAL' if original.bind == 1 else str(original.bind)),
                            type='OBJECT', owner_name=owner_symbol.name,
                            owner_address=f'0x{owner_va:08X}', owner_size=owner_size,
                            owner_offset=va-owner_va, c_object_section=section,
                            c_object_offset=owner_symbol.value, object_sha256=sha(object_path),
                            linked_elf_sha256=sha(linked_path), packet_sha256=packet_sha,
                            source_sha256=source_sha, assembly_sha256=assembly_sha))
    intervals.sort()
    cursor = owner_va
    for lo, hi in intervals:
        require(lo == cursor and hi > lo,
                entry['tu'] + '/' + owner_symbol.name + ': retail alias spans do not tile aggregate extent without gaps/overlap')
        cursor = hi
    require(cursor == owner_va + owner_size,
            entry['tu'] + '/' + owner_symbol.name + ': retail alias spans do not cover complete aggregate extent')
    linked = [sym for sym in linked_elf.symbols if sym.name == owner_symbol.name
              and sym.value == owner_va and sym.size == owner_size and sym.type == owner_symbol.type
              and sym.bind == owner_symbol.bind and sym.shndx not in (0, 0xfff1)]
    require(len(linked) == 1, entry['tu'] + '/' + owner_symbol.name + ': linked aggregate owner identity differs')
    object_bytes = object_elf.section_bytes(object_elf.sections[owner_symbol.shndx])
    current_bytes = object_bytes[owner_symbol.value:owner_symbol.value + owner_size]
    require(len(current_bytes) == owner_size and current_bytes == data_carve.va_bytes(original_elf, owner_va, owner_size),
            entry['tu'] + '/' + owner_symbol.name + ': aggregate compiler bytes differ from original initialized data')
    require(data_carve.va_bytes(linked_elf, owner_va, owner_size) == current_bytes,
            entry['tu'] + '/' + owner_symbol.name + ': linked aggregate bytes differ from current compiler object')
    return aliases


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root',type=Path,required=True)
    parser.add_argument('--manifest',type=Path,required=True)
    parser.add_argument('--gate',type=Path,default=None)
    parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--storage-manifest',type=Path,default=None,
                        help='optional frozen sidecar of per-TU BSS/SBSS candidate rows')
    parser.add_argument('--allocation-root',type=Path,default=CANONICAL,
                        help='repository containing immutable allocation packets (defaults to canonical root)')
    parser.add_argument('--allocation-control',type=Path,default=None,
                        help='current coordinator data-recovery-allocation/1 control file for a takeover cohort')
    parser.add_argument('--tools-root',type=Path,default=None,
                        help='checkout whose private tools/tu/data_carve.py is used (defaults to --root)')
    parser.add_argument('--storage-only',action='store_true',
                        help='verify only frozen .bss/.sbss candidates; initialized locations remain unverified')
    parser.add_argument('--input-only',action='store_true',
                        help='emit a compiler/object/map/relocation storage report before final link; implies --storage-only')
    args=parser.parse_args()
    root=args.root.resolve()
    configure_tools_root(args.tools_root or root)
    manifest=json.loads(args.manifest.read_text())
    storage_manifest=(json.loads(args.storage_manifest.read_text())
                      if args.storage_manifest else None)
    gate=json.loads(args.gate.read_text()) if args.gate else None
    if args.input_only:
        require(gate is None,'--input-only does not accept a linked whole-file gate')
    else:
        direct_pass = gate is not None and gate.get('result') == 'pass'
        compare_pass = (gate is not None and gate.get('whole_file_identical') is True and
                        gate.get('original_sha256') == gate.get('rebuilt_sha256') and
                        gate.get('diff_range_count') == 0)
        require(direct_pass or compare_pass,
                'Linked whole-file gate or exact whole-file compare did not pass')
    originals=json.loads((root/'config/originals.json').read_text())['units']
    registry=data_carve.load_registry(root)
    input_only = args.input_only
    storage_only = args.storage_only or input_only
    output=dict(schema='compiler-c-data-input-audit/1' if input_only else 'linked-c-data-audit/2',
                data_required=False, grants_function_acceptance=False,
                manifest_sha256=sha(args.manifest),
                scope='compiler_storage_input_only' if input_only else
                      'storage_only' if storage_only else 'initialized_and_storage',
                gate_sha256=sha(args.gate) if args.gate else None,objects=[],problems=[])
    output['storage_manifest_path']=str(args.storage_manifest) if args.storage_manifest else None
    output['storage_manifest_sha256']=sha(args.storage_manifest) if args.storage_manifest else None
    output['allocation_control_path']=str(args.allocation_control.resolve()) if args.allocation_control else None
    output['allocation_control_sha256']=sha(args.allocation_control) if args.allocation_control else None
    if storage_manifest is not None:
        require(storage_manifest.get('schema') in ('overlay-bss-allocation/1',
                                                    'main-common-tail-storage/1') and
                isinstance(storage_manifest.get('recovered_location_candidates'),list),
                'invalid supplementary linked-storage manifest')
    main_common_tail_rows = (validate_main_common_tail_storage(
        root, storage_manifest, manifest['candidates'], gate, args.gate)
        if storage_manifest is not None and storage_manifest.get('main_common_tail') else {})
    if not input_only:
        for unit in manifest['linked_targets']:
            unit_dir=root/'build'/unit
            original=unit_dir/'orig'/originals[unit]['file']
            rebuilt=unit_dir/'build'/(unit+'.bin')
            require(sha(original)==sha(rebuilt)==originals[unit]['sha256'],
                    unit+': current actual whole-file hashes differ')
    for entry in manifest['candidates']:
        try:
            record_path=Path(entry['record'])
            require(sha(record_path)==entry['sha256'],entry['tu']+': frozen record changed')
            record=json.loads(record_path.read_text())
            for relative, frozen in record['files'].items():
                require(sha(root/relative)==frozen['sha256'],
                        entry['tu']+': substituted input changed '+relative)
            source_rel=next(p for p in record['files'] if p.endswith('.c'))
            source=root/source_rel
            require(sha(source)==record['files'][source_rel]['sha256'],entry['tu']+': substituted source changed')
            unit=record['unit']
            bss_rows=[]
            if storage_manifest is not None:
                bss_rows=[row for row in storage_manifest['recovered_location_candidates']
                          if row.get('evidence',{}).get('original_tu')==entry['tu']]
                for row in bss_rows:
                    evidence=row.get('evidence',{})
                    require(evidence.get('source_sha256')==sha(source) and
                            evidence.get('object_sha256')==record['object']['sha256'] and
                            evidence.get('assembly_sha256')==record['assembly']['sha256'],
                            entry['tu']+': supplementary BSS row source/object/assembly pins differ')
                seen={(x.get('section'),x.get('address'),tuple(x.get('names',[])))
                      for x in record.get('recovered_location_candidates',[])}
                for row in bss_rows:
                    key=(row.get('section'),row.get('address'),tuple(row.get('names',[])))
                    if key not in seen:
                        record.setdefault('recovered_location_candidates',[]).append(row)
                        seen.add(key)
            if storage_only:
                # Keep the frozen record/hash intact but restrict the proof
                # request to its storage candidates. This supports isolated
                # linked NOBITS provenance when unrelated initialized-data
                # identities in the same TU are still scaffold-owned.
                record=dict(record)
                record['recovered_location_candidates']=[loc for loc in record.get('recovered_location_candidates',[])
                                                          if loc.get('section') in ('.bss','.sbss')]
            unit_dir=root/'build'/unit
            name=source.stem
            obj_path=None
            if unit == 'main':
                ld_path=unit_dir/'main.rom.ld'
                carved=unit_dir/'build'/'c'/(name+'.carved.o')
                if ld_path.is_file() and carved.is_file():
                    carved_rel=carved.relative_to(unit_dir).as_posix()
                    if re.search(re.escape(carved_rel)+r'\(\.',ld_path.read_text()):
                        obj_path=carved
            # Some grouped carve flows place initialized constants into a
            # second `*.allocated.carved.o`; the linker script can select it
            # while an older data_carve selector still returns the uncarved
            # allocated object. Prefer this object only when the current unit
            # linker script names that exact per-TU path.
            if unit != 'main':
                ld_path=unit_dir/f'{unit}.ld'
                carved=unit_dir/'build'/'c'/unit/(name+'.allocated.carved.o')
                if ld_path.is_file() and carved.is_file():
                    ld_text=ld_path.read_text()
                    carved_rel=f'build/c/{unit}/{name}.allocated.carved.o'
                    if re.search(re.escape(carved_rel)+r'\(\.text\);',ld_text):
                        obj_path=carved
            # Prefer a direct raw-to-carved object when the linker selects it
            # for any section. The ordinary selector returns the uncarved raw
            # object and would miss the exact split actually linked.
            if obj_path is None and unit != 'main':
                ld_path=unit_dir/f'{unit}.ld'
                carved=unit_dir/'build'/'scaffold'/'src'/unit/(name+'.carved.o')
                if ld_path.is_file() and carved.is_file():
                    ld_text=ld_path.read_text()
                    carved_rel=carved.relative_to(unit_dir).as_posix()
                    if re.search(re.escape(carved_rel)+r'\(\.',ld_text):
                        obj_path=carved
            if obj_path is None:
                obj_path=data_carve.tu_object(unit_dir,unit,name)
            if unit!='main' and (obj_path is None or obj_path.suffix=='.o'):
                ld_path=unit_dir/f'{unit}.ld'
                carved=unit_dir/'build'/'scaffold'/'src'/unit/(name+'.carved.o')
                if ld_path.is_file() and carved.is_file():
                    ld_text=data_carve.ovl_restore(ld_path.read_text())
                    carved_rel=carved.relative_to(unit_dir).as_posix()
                    if re.search(re.escape(carved_rel)+r'\(\.',ld_text):
                        obj_path=carved
            # Private grouped allocators may link a second, carved allocated
            # object whose suffix is not recognized by the checkout's current
            # data_carve.tu_object(). Probe only the two exact per-TU paths
            # emitted by that allocator; never search the build tree.
            if obj_path is None and unit != 'main':
                for suffix in ('.allocated.carved.o', '.allocated.o'):
                    candidate_object=unit_dir/'build'/'c'/unit/(name+suffix)
                    if candidate_object.is_file():
                        obj_path=candidate_object
                        break
            require(obj_path is not None,entry['tu']+': no freshly compiled published object')
            asm_path=(Path(str(obj_path)+'.s') if unit=='main' else obj_path.with_suffix('.s'))
            if obj_path.name.endswith('.carved.o') and not obj_path.name.endswith('.allocated.carved.o'):
                sidecar=Path(str(obj_path)+'.json')
                require(sidecar.is_file(),entry['tu']+': raw carve lacks its normal sidecar')
                carve=json.loads(sidecar.read_text())
                raw_meta=record.get('object') or {}
                raw_obj=unit_dir/carve.get('object','')
                require(carve.get('out')==obj_path.relative_to(unit_dir).as_posix() and
                        raw_obj.is_file() and sha(raw_obj)==raw_meta.get('sha256'),
                        entry['tu']+': raw carve sidecar does not bind the packet-pinned compiler object bytes')
                asm_meta=record.get('assembly') or {}
                asm_path=Path(asm_meta.get('path',''))
                require(asm_path.is_file() and sha(asm_path)==asm_meta.get('sha256'),
                        entry['tu']+': raw carve lacks packet-pinned compiler assembly')
            if not asm_path.is_file() and unit!='main' and obj_path.name.endswith(('.allocated.carved.o','.allocated.o')):
                asm_path=unit_dir/'build'/'scaffold'/'src'/unit/(name+'.s')
            require(asm_path.is_file(),entry['tu']+': no compiler assembly')
            obj=Elf(obj_path.read_bytes())
            emitted=emitted_data(asm_path.read_text(errors='surrogateescape'))
            parsed_assembly=tu_audit.AsmFile(asm_path.read_text(errors='surrogateescape'))
            emitted.update(parsed_assembly.defined_data)
            linked_path=unit_dir/'build'/(unit+'.rom.elf')
            linked= None if input_only else Elf(linked_path.read_bytes())
            ctx=data_carve.tu_context(root,unit,unit_dir,entry['tu'])
            original_elf=Elf(Path(ctx['orig']).read_bytes())
            if args.allocation_control:
                require(record.get('allocation_task') is not None,
                        entry['tu'] + ': candidate lacks its coordinator task identity')
                packet_tu, allocation_path, allocation = current_allocation_scope(
                    args.allocation_control, record['allocation_task'], entry, record,
                    ctx, original_elf, root)
            else:
                allocation, allocation_path = allocation_packet(
                    root, record.get('allocation_task'),args.allocation_root)
                packet_tu=next((x for x in allocation['allocation'] if x['id']==entry['tu']),None)
                require(packet_tu is not None,entry['tu']+': not present in its frozen allocation packet')
            original_section_ranges=mapped_section_ranges(root,unit,entry['tu'])
            # MAIN common-tail objects are outside the original per-TU map and
            # immutable worker packets.  Inject only rows that passed the
            # strict current r6 raw/allocated/selected/retail/map/link proof
            # above; every bound remains limited to its exact owner interval.
            common_rows=main_common_tail_rows.get(entry['tu'],[])
            if common_rows:
                packet_tu=dict(packet_tu)
                packet_tu['data']=list(packet_tu.get('data',[]))
                by_section={}
                for common in common_rows:
                    lo=int(common['target_range'][0],16); hi=int(common['target_range'][1],16)
                    packet_tu['data'].append(dict(unit='main',address=common['address'],
                        section=common['section'],owner_tu='linker/common_tail',
                        original_symbol_size=common['owner_size'],names=common['names'],
                        common_tail_proof=True))
                    previous=by_section.get(common['section'])
                    by_section[common['section']]=(lo,hi) if previous is None else \
                        (min(previous[0],lo),max(previous[1],hi))
                for section, span in by_section.items():
                    existing=original_section_ranges.get(section)
                    if existing is not None:
                        original_section_ranges[section]=(min(existing[0],span[0]),
                                                          max(existing[1],span[1]))
                    else:
                        original_section_ranges[section]=span
            # A record may carry multiple section-scoped supplementary rows.
            # Each sidecar independently pins its section map and source/link
            # evidence; apply them serially to a private packet copy. Keep the
            # legacy singular field readable for already frozen records.
            sidecar_refs=record.get('supplementary_allocations', [])
            require(isinstance(sidecar_refs,list),entry['tu']+
                    ': supplementary_allocations must be an array of pinned sidecars')
            sidecar_refs=list(sidecar_refs)
            if record.get('supplementary_allocation'):
                sidecar_refs.insert(0,record['supplementary_allocation'])
            sidecar_keys=[(x.get('path'),x.get('sha256')) for x in sidecar_refs]
            require(len(sidecar_keys)==len(set(sidecar_keys)),entry['tu']+
                    ': duplicate supplementary allocation reference')
            auxiliary_allocations=[]
            for sidecar_ref in sidecar_refs:
                if storage_only:
                    sidecar_path=Path(sidecar_ref['path'])
                    if not sidecar_path.is_absolute():
                        sidecar_path=Path(root)/sidecar_path
                    if not sidecar_path.is_file():
                        continue
                    try:
                        sidecar_section=json.loads(sidecar_path.read_text()).get(
                            'original_section_map',{}).get('section')
                    except (ValueError,OSError):
                        sidecar_section=None
                    if sidecar_section not in ('.bss','.sbss'):
                        continue
                sidecar_record=dict(record)
                sidecar_record['supplementary_allocation']=sidecar_ref
                packet_tu, auxiliary=validate_auxiliary_allocation(
                    root,sidecar_record,entry,allocation_path,packet_tu,source,obj_path,
                    asm_path,original_elf,original_section_ranges)
                auxiliary_allocations.extend(auxiliary)
            # Derivation is a compiler-emission check and must inspect the
            # record-pinned raw object. Overlay linkers may select an
            # allocated/carved derivative whose sections are split or marked
            # already placed; feeding that output back into derive() loses the
            # raw section-to-map relation and can erase otherwise exact run
            # facts. The selected linked object remains the authority for
            # allocation and final placement checks above.
            raw_object_info=record.get('compiler_object') or record.get('object') or {}
            derive_obj=Path(raw_object_info.get('path',''))
            require(derive_obj.is_file() and raw_object_info.get('sha256')==sha(derive_obj),
                    entry['tu']+': missing or stale record-pinned raw compiler object for derivation')
            require_registered_common_claims(entry['tu'], registered_common_storage_locations(
                root, entry['tu'], original_elf, Elf(derive_obj.read_bytes()),
                asm_path.read_text(errors='surrogateescape')), record)
            compiler_allocation_proof=None
            if record.get('compiler_object'):
                compiler_assembly=Path(str(derive_obj)+'.s')
                require(compiler_assembly.is_file() and sha(compiler_assembly)==sha(asm_path) and
                        record['assembly'].get('sha256')==sha(compiler_assembly),
                        entry['tu']+': compiler cc1 assembly pin differs')
                allocation_input=Path(record['object']['path'])
                require(allocation_input.is_file() and sha(allocation_input)==record['object'].get('sha256'),
                        entry['tu']+': allocated carve input pin differs')
                if allocation_input.resolve()!=derive_obj.resolve():
                    require(unit=='main',entry['tu']+': separate compiler input is MAIN-only')
                    recipe=tu_audit.build_assembly_recipe(unit_dir,allocation_input)
                    common=(recipe or {}).get('common_allocation') or {}
                    require(Path(common.get('raw_object','/nonexistent')).resolve()==derive_obj.resolve() and
                            Path(common.get('output_object','/nonexistent')).resolve()==allocation_input.resolve(),
                            entry['tu']+': allocation recipe does not connect exact compiler/carve inputs')
                    linker=Path(common['linker']).resolve()
                    require(sha(linker)==common['linker_sha256'],entry['tu']+': allocation linker pin differs')
                    check_path=args.output.parent/(entry['tu'].replace('/','-')+'.verified-alloc.o')
                    require(not check_path.exists(),entry['tu']+': allocation verification output already exists')
                    argv=[str(linker),'-r','-d','-EL','-m','elf32lr5900','-o',str(check_path),str(derive_obj)]
                    process=subprocess.Popen(argv,stdout=subprocess.PIPE,stderr=subprocess.PIPE,start_new_session=True)
                    try:
                        stdout,stderr=process.communicate(timeout=30)
                    except subprocess.TimeoutExpired:
                        os.killpg(process.pid,signal.SIGKILL)
                        process.communicate()
                        raise ValueError(entry['tu']+': exact COMMON-allocation verification timed out')
                    require(process.returncode==0 and check_path.is_file() and
                            sha(check_path)==sha(allocation_input),
                            entry['tu']+': exact current COMMON-allocation bytes differ: '+stderr.decode(errors='replace')[:300])
                    compiler_allocation_proof=dict(raw_object_path=str(derive_obj),raw_object_sha256=sha(derive_obj),
                        allocated_object_path=str(allocation_input),allocated_object_sha256=sha(allocation_input),
                        verification_path=str(check_path),verification_sha256=sha(check_path),
                        linker_path=str(linker),linker_sha256=sha(linker),argv=argv)
            raw_object_elf=Elf(derive_obj.read_bytes())
            derivation_obj=Path(compiler_allocation_proof['allocated_object_path']) if compiler_allocation_proof else derive_obj
            if compiler_allocation_proof:
                allocated_elf=Elf(derivation_obj.read_bytes())
                # The verified native ld step supplies COMMON storage for
                # NOBITS planning. Every initialized section still has the
                # compiler's exact bytes, metadata and named-symbol offsets.
                for raw_section in raw_object_elf.sections:
                    if raw_section.name not in data_carve.SECTIONS or raw_section.type==8 or not raw_section.size:
                        continue
                    allocated_sections=[s for s in allocated_elf.sections if s.name==raw_section.name]
                    require(len(allocated_sections)==1,entry['tu']+': COMMON allocation lost an initialized section')
                    allocated_section=allocated_sections[0]
                    require((raw_section.type,raw_section.flags,raw_section.align,raw_section.size)==
                            (allocated_section.type,allocated_section.flags,allocated_section.align,allocated_section.size) and
                            raw_object_elf.section_bytes(raw_section)==allocated_elf.section_bytes(allocated_section),
                            entry['tu']+': COMMON allocation changed initialized compiler bytes/metadata')
                    raw_symbols=sorted((s.name,s.value,s.size,s.bind,s.type) for s in raw_object_elf.symbols
                                       if s.shndx==raw_section.index and s.name and s.type not in (3,4))
                    allocated_symbols=sorted((s.name,s.value,s.size,s.bind,s.type) for s in allocated_elf.symbols
                                             if s.shndx==allocated_section.index and s.name and s.type not in (3,4))
                    require(raw_symbols==allocated_symbols,
                            entry['tu']+': COMMON allocation changed initialized compiler symbol identities')
            # `tu_context()` marks sections as `placed` when the current
            # linker script selects the TU's entire compiler section. That is
            # a link-layout fact, not an emission fact: for this raw-object
            # derivation pass, retain the map pieces but let derive() classify
            # their compiler-generated items. Restore the context function
            # immediately afterwards; downstream allocation/placement checks
            # still use the actual selected object and current linker script.
            original_tu_context=data_carve.tu_context
            def raw_emission_context(*context_args, **context_kwargs):
                context=original_tu_context(*context_args, **context_kwargs)
                if context_args and context_args[-1] == entry['tu']:
                    context['placed']=set()
                return context
            data_carve.tu_context=raw_emission_context
            try:
                derived=data_carve.derive(root,unit,unit_dir,entry['tu'],source,derivation_obj)
            finally:
                data_carve.tu_context=original_tu_context
            initialized_problems=[p for p in derived['problems'] if not is_nobits_derivation_problem(p)]
            carved_data_problem=bool(initialized_problems) and all(
                'no offset of the C object starts the run' in str(p) for p in initialized_problems)
            if not storage_only:
                require(not initialized_problems or carved_data_problem,
                        entry['tu']+': current initialized-data derivation problems '+str(initialized_problems))
            declared=registry['tus'].get(entry['tu'],{})
            # The existing overlay derivation deliberately covers initialized
            # data only. BSS/SBSS ownership is proven from emitted storage,
            # original map and final link below, not by demanding a byte carve.
            derived_initialized={s:r for s,r in derived['runs'].items() if s not in ('.bss','.sbss')}
            declared_initialized={s:r for s,r in declared.items() if s not in ('.bss','.sbss')}
            direct_object_spans={}
            direct_unclaimed=[]
            if not storage_only:
                derived_normalized=data_carve.normalize_runs(derived_initialized)
                declared_normalized=data_carve.normalize_runs(declared_initialized)
                # c_storage_aliases are bounded linker/audit metadata, not
                # compiler-derived run facts.  The run range and C-owned
                # symbol set still compare exactly here; each alias itself is
                # checked below against the current object, retail identity,
                # mapped span, and linked image before it earns credit.
                for runs in (derived_normalized, declared_normalized):
                    for section_runs in runs.values():
                        for run in section_runs:
                            run.pop('c_storage_aliases', None)
                # Some private overlay link layouts select exact C objects
                # directly from the allocated TU object and carry the split
                # pins in the linker script, so derive() legitimately returns
                # no run for that section.  Preserve the frozen run facts, but
                # allow only a complete one-to-one set of packet-backed named
                # objects to be proved through the direct-object validator.
                # Anonymous compiler output and partial run coverage cannot
                # use this path.
                direct_sections=set()
                for section, section_runs in declared_initialized.items():
                    if derived_initialized.get(section):
                        continue
                    if not section_runs:
                        continue
                    locations=[loc for loc in record.get('recovered_location_candidates',[])
                               if loc.get('section')==section]
                    covered=[]
                    eligible=True
                    for lo,hi in data_carve.declared_runs(entry['tu'],section,section_runs):
                        raw_run=next((run for run in section_runs
                                      if tuple(data_carve.hx(x) for x in run.get('range',[]))==(lo,hi)),None)
                        symbols=(raw_run or {}).get('c_owned_symbols',[])
                        if not symbols:
                            eligible=False
                            break
                        for symbol_name in symbols:
                            matches=[loc for loc in locations
                                     if symbol_name in (loc.get('names') or []) and
                                     lo<=int(loc.get('address','0'),16)<hi]
                            if len(matches)!=1:
                                eligible=False
                                break
                            location=matches[0]
                            proof=direct_initialized_symbol_span(
                                entry,location,record,packet_tu,ctx,obj,linked,original_elf,
                                original_section_ranges,emitted,obj_path,unit_dir,data_carve)
                            covered.append((location,proof))
                        if not eligible:
                            break
                    if eligible and covered:
                        direct_sections.add(section)
                        for location, proof in covered:
                            direct_object_spans[(section,location['address'])]=proof
                        for lo,hi in data_carve.declared_runs(entry['tu'],section,section_runs):
                            intervals=sorted((max(lo,p['range'][0]),min(hi,p['range'][1]))
                                             for _,p in covered
                                             if p['range'][0]<hi and lo<p['range'][1])
                            cursor=lo
                            for start,end in intervals:
                                if cursor<start:
                                    direct_unclaimed.append(dict(section=section,range=[cursor,start],
                                        reason='direct_named_object_proof_leaves_scaffold_bytes_uncredited'))
                                cursor=max(cursor,end)
                            if cursor<hi:
                                direct_unclaimed.append(dict(section=section,range=[cursor,hi],
                                    reason='direct_named_object_proof_leaves_scaffold_bytes_uncredited'))
                        declared_normalized.pop(section,None)
                require(derived_normalized==declared_normalized
                        or carved_data_problem,
                        entry['tu']+': declared data runs do not equal current compiler derivation')
            verified=[]
            verified_location_spans=[]
            proven_ranges={}
            anonymous=[]
            dependencies=[]
            linker_dependencies=[]
            unclaimed_scaffold_runs=list(direct_unclaimed)
            derived_nobits={}
            symbol_aliases=[]
            original_object_identities=[]
            storage_owner_aliases=[]
            original_item_tails=[]
            native_unsplit_sections=[]
            for section,runs in derived['runs'].items():
                if storage_only and section not in ('.bss','.sbss'):
                    unclaimed_scaffold_runs.extend(dict(section=section,range=[lo,hi])
                                                   for lo,hi in data_carve.declared_runs(entry['tu'],section,runs))
                    continue
                items=data_carve.section_items(ctx,section)
                spans=data_carve.declared_runs(entry['tu'],section,runs)
                if section in ('.bss','.sbss'):
                    detail=derived['sections'][section]
                    owners=detail.get('nobits_ownership',[])
                    require(not detail.get('bytes',{}).get('checked'),
                            entry['tu']+'/'+section+': NOBITS bytes must not be used as ownership proof')
                    derived_nobits[section]=dict(ownership=owners,
                                                 covered_scaffold_items=detail.get('covered_scaffold_items',[]),
                                                 derive_problems=[p for p in derived['problems']
                                                                  if is_nobits_derivation_problem(p)],
                                                 ranges=[[lo,hi] for lo,hi in spans])
                    continue
                claimed_spans=[(lo,hi) for lo,hi in spans
                               if any(loc.get('section')==section and lo<=int(loc.get('address','0'),16)<hi
                                      for loc in record.get('recovered_location_candidates',[]))]
                unclaimed_spans=[(lo,hi) for lo,hi in spans if (lo,hi) not in claimed_spans]
                unclaimed_scaffold_runs.extend(dict(section=section,range=[lo,hi]) for lo,hi in unclaimed_spans)
                if not spans:
                    continue
                # Claimed runs are planned from the raw compiler section. An
                # unclaimed adjacent #APP item can have no C anchor (for
                # example a literal label emitted by preserved inline asm), so
                # keep it as an exact placement anchor but prove it separately
                # through the allocator/carve chain without crediting it.
                claimed_piece_spans=set(claimed_spans)
                if claimed_spans:
                    explicit_runs=[run for run in runs if
                                   tuple(map(data_carve.hx,run['range'])) in claimed_spans]
                    if explicit_runs and all(run.get('c_input_spans') for run in explicit_runs):
                        plan={'pieces':[]}
                        for run in explicit_runs:
                            for part in run['c_input_spans']:
                                part_span=tuple(map(data_carve.hx,part['range']))
                                proof=exact_carved_section_plan(
                                    entry,section,[part_span],unit_dir,derive_obj,obj_path,
                                    raw_object_elf,obj,linked,original_elf,linked_path,ctx,data_carve,
                                    source_section=part['section'],
                                    compiler_allocation_proof=compiler_allocation_proof)
                                plan['pieces'].extend(proof['pieces'])
                                # A logical original run may be assembled from
                                # several noncontiguous compiler input slices.
                                # Each slice has just passed the exact current
                                # raw/allocation/carve/link proof; process its
                                # own original span rather than silently
                                # dropping it because it is smaller than the
                                # enclosing logical run.
                                if part.get('credit','verified')=='verified':
                                    claimed_piece_spans.add(part_span)
                    else:
                        csec=next(s for s in raw_object_elf.sections if s.name==section and s.size)
                        physical_runs=data_carve.absorb_padding(entry['tu'],section,runs,items)
                        physical_spans=[tuple(map(data_carve.hx,physical['range']))
                                        for logical,physical in zip(runs,physical_runs)
                                        if tuple(map(data_carve.hx,logical['range'])) in claimed_spans]
                        if physical_spans != claimed_spans:
                            # Alignment pads are physical carve bytes, never
                            # additional named definitions. Prove the complete
                            # current guarded split and exact selected input.
                            plan=exact_carved_section_plan(
                                entry,section,physical_spans,unit_dir,derive_obj,obj_path,
                                raw_object_elf,obj,linked,original_elf,linked_path,ctx,data_carve,
                                compiler_allocation_proof=compiler_allocation_proof)
                            claimed_piece_spans.update(physical_spans)
                        else:
                            try:
                                plan=data_carve.plan_split(raw_object_elf,csec,
                                    data_carve.run_specs(items,claimed_spans),
                                    lambda va,n:data_carve.va_bytes(original_elf,va,n))
                            except data_carve.CarveError:
                                plan=exact_carved_section_plan(
                                    entry,section,claimed_spans,unit_dir,derive_obj,obj_path,
                                    raw_object_elf,obj,linked,original_elf,linked_path,ctx,data_carve,
                                    compiler_allocation_proof=compiler_allocation_proof)
                else:
                    plan={'pieces': []}
                if plan.get('native_unsplit'):
                    native_unsplit_sections.append(plan['native_unsplit'])
                for lo,hi in unclaimed_spans:
                    try:
                        dependency=uncredited_inline_asm_data_dependency(
                            entry,record,section,(lo,hi),items,unit_dir,derive_obj,obj_path,
                            linked_path,raw_object_elf,obj,linked,original_elf,ctx,
                            allocation_path,source,asm_path,data_carve)
                    except (ValueError,OSError,KeyError,StopIteration):
                        dependency=None
                    if dependency is not None:
                        dependencies.append(dependency)
                for piece in plan['pieces']:
                    if (piece['lo'],piece['hi']) not in claimed_piece_spans:
                        continue
                    if piece.get('uncredited_original_item_tail'):
                        original_item_tails.append(piece['uncredited_original_item_tail'])
                    csec=next(s for s in raw_object_elf.sections
                              if s.name==piece.get('input_section',section) and s.size)
                    own=[s for s in raw_object_elf.symbols if s.shndx==csec.index and s.type not in (3,4) and s.name]
                    symbols=[s for s in own if piece['offset']<=s.value<piece['offset']+piece['length']]
                    if symbols:
                        require(all(s.name in emitted for s in symbols),
                                entry['tu']+'/'+section+': run contains data not emitted by cc1 C definitions')
                    else:
                        preprocessed_path=asm_path.with_suffix('.i') if unit!='main' else Path(str(derive_obj)+'.i')
                        require(preprocessed_path.is_file(),entry['tu']+': no current preprocessed compiler input')
                        proof=compiler_only_section(asm_path.read_text(errors='surrogateescape'),
                                                   preprocessed_path.read_text(errors='surrogateescape'),unit_dir,section)
                        if proof is None:
                            proof=compiler_li_s_pool(asm_path.read_text(errors='surrogateescape'),
                                                     preprocessed_path.read_text(errors='surrogateescape'),
                                                     unit_dir,raw_object_elf,csec,piece,section)
                        else:
                            proof['range']=[piece['lo'],piece['lo']+piece['length']]
                        anonymous.append(proof)
                    for sym in symbols:
                        expected=piece['lo']+sym.value-piece['offset']
                        matches=[s for s in linked.symbols if s.name==sym.name and s.value==expected
                                 and s.shndx not in (0,0xfff1) and s.size==sym.size
                                 and s.type==sym.type and s.bind==sym.bind]
                        require(matches,entry['tu']+'/'+sym.name+': linked data identity differs or resolves to an absolute scaffold witness')
                        original_range=original_section_ranges.get(section)
                        if original_range and sym.name and sym.type==1:
                            original_named=[s for s in original_elf.symbols
                                            if s.name==sym.name and s.shndx not in (0,0xfff1)
                                            and original_range[0]<=s.value<original_range[1]]
                            # Renaming a C object does not erase its original
                            # identity: meaningful OBJECT records at this exact
                            # mapped VA still constrain the source binding,
                            # type, and extent. This catches static tables made
                            # GLOBAL merely because the source used a different
                            # spelling from the retail local symbol.
                            original_at_address=[s for s in original_elf.symbols
                                                 if s.shndx not in (0,0xfff1)
                                                 and original_range[0]<=s.value<original_range[1]
                                                 and s.value==expected and s.type==1 and s.size>0]
                            for orig in original_at_address:
                                require((sym.bind,sym.type,sym.size)==(orig.bind,orig.type,orig.size),
                                        entry['tu']+'/'+sym.name+': C OBJECT binding/type/size differs from meaningful original object at the same address ('+orig.name+')')
                                if not any(x.get('name')==orig.name and x.get('address')==f'0x{expected:08X}'
                                           for x in original_object_identities):
                                    original_object_identities.append(dict(name=orig.name,candidate_name=sym.name,
                                                                           address=f'0x{expected:08X}',
                                                                           binding=('LOCAL' if orig.bind==0 else 'GLOBAL' if orig.bind==1 else str(orig.bind)),
                                                                           type='OBJECT',size=orig.size,identity='same_address_original'))
                            meaningful=[s for s in original_named if s.type==1 and s.size>0]
                            if meaningful:
                                exact_original=[s for s in meaningful if s.value==expected]
                                require(len(exact_original)==1,
                                        entry['tu']+'/'+sym.name+': meaningful original OBJECT is not at the C-owned address')
                                orig=exact_original[0]
                                require((sym.bind,sym.type,sym.size)==(orig.bind,orig.type,orig.size),
                                        entry['tu']+'/'+sym.name+': C OBJECT binding/type/size differs from meaningful original ELF identity')
                                original_object_identities.append(dict(name=sym.name,address=f'0x{expected:08X}',
                                                                       binding=('LOCAL' if orig.bind==0 else 'GLOBAL' if orig.bind==1 else str(orig.bind)),
                                                                       type='OBJECT',size=orig.size,identity='meaningful'))
                            elif original_named:
                                exact_original=[s for s in original_named if s.value==expected]
                                require(len(exact_original)==1,
                                        entry['tu']+'/'+sym.name+': zero-size original symbol is not at the C-owned address')
                                orig=exact_original[0]
                                require(sym.bind==orig.bind,
                                        entry['tu']+'/'+sym.name+': C OBJECT binding differs from original zero-size symbol')
                                original_object_identities.append(dict(name=sym.name,address=f'0x{expected:08X}',
                                                                       binding=('LOCAL' if orig.bind==0 else 'GLOBAL' if orig.bind==1 else str(orig.bind)),
                                                                       original_type=orig.type,original_size=orig.size,
                                                                       type='OBJECT',size=sym.size,identity='zero_size_original'))
                        if sym.bind == 0 and sym.type == 1:
                            candidate_alias = any(loc.get('section') == section and
                                                  int(loc.get('address', '0'), 16) == expected and
                                                  sym.name in (loc.get('names') or [])
                                                  for loc in record.get('recovered_location_candidates', []))
                            if candidate_alias:
                                packet_labels = [label for item in packet_tu.get('data', [])
                                                 if item.get('section') == section and
                                                 int(item.get('address', '0'), 16) == expected and
                                                 item.get('original_symbol_size') is not None and
                                                 int(item['original_symbol_size']) == sym.size
                                                 for label in item.get('names', []) if label != sym.name]
                                for scaffold_name in packet_labels:
                                    symbol_aliases.append(dict(section=section,scaffold_name=scaffold_name,
                                                              object_symbol=sym.name,address=f'0x{expected:08X}',
                                                              size=sym.size,binding='LOCAL',type='OBJECT'))
                    proven_ranges.setdefault(section,[]).append((piece['lo'],piece['lo']+piece['length']))
                    for location in record.get('recovered_location_candidates',[]):
                        va=int(location.get('address','0'),16)
                        if location.get('section')!=section or not piece['lo']<=va<piece['lo']+piece['length']:
                            continue
                        names=set(location.get('names') or [])
                        # A candidate can preserve the meaningful retail ELF
                        # name while the Splat packet names the same item with
                        # an underscore alias (or only an address label). Bind
                        # that case by exact address and original ELF identity;
                        # do not require the candidate spelling to be copied
                        # into the immutable packet.
                        named_original_at_va=[s for s in original_elf.symbols
                                              if s.name in names and s.value==va and
                                              s.shndx not in (0,0xfff1) and
                                              original_section_ranges.get(section,(0,0))[0]<=s.value<
                                                  original_section_ranges.get(section,(0,0))[1]]
                        owners=[item for item in items if item['start']<=va<item['end']
                                and (names.intersection(item.get('labels') or []) or
                                     (item['start']==va and named_original_at_va))]
                        if not owners:
                            # owner-corrected run: the item is mapped in its map TU
                            owners=[item for item in owner_corrected_items(root,unit,ctx,entry['tu'],section,va)
                                    if item['start']<=va<item['end'] and
                                    names.intersection(item.get('labels') or [])]
                        require(len(owners)==1,entry['tu']+'/'+location['address']+
                                ': no unique original scaffold item bounds the verified location')
                        item=owners[0]
                        end=min(item['end'],piece['lo']+piece['length'])
                        packet_matches=[row for row in packet_tu.get('data',[])
                                        if row.get('section')==section and
                                        int(row.get('address','0'),16)==va and
                                        (names.intersection(row.get('names') or []) or
                                         bool(named_original_at_va))]
                        if packet_matches:
                            declared_sizes=[int(row['original_symbol_size']) for row in packet_matches
                                            if row.get('original_symbol_size') is not None and
                                            int(row['original_symbol_size'])>0]
                            if declared_sizes:
                                end=min(end,va+min(declared_sizes))
                        length=end-va
                        require(length>0 and va+length<=piece['lo']+piece['length'],
                                entry['tu']+'/'+location['address']+
                                ': scaffold item does not fit inside its compiler-owned split piece')
                        object_offset=piece['offset']+va-piece['lo']
                        exact_object_symbols=[s.name for s in own if s.value==object_offset]
                        owner_proof=validate_initialized_storage_owner(
                            entry,location,section,piece,own,raw_object_elf,linked,original_elf,
                            original_section_ranges,emitted,derive_obj,linked_path,
                            sha(allocation_path),sha(source),sha(asm_path))
                        owner_va=(int(owner_proof['address'],16) if owner_proof else None)
                        if owner_proof:
                            if owner_proof['name'] not in exact_object_symbols:
                                exact_object_symbols.append(owner_proof['name'])
                        original_at_location=[s for s in original_elf.symbols
                                              if s.shndx not in (0,0xfff1) and s.value==va
                                              and original_section_ranges.get(section,(0,0))[0]<=s.value<
                                                  original_section_ranges.get(section,(0,0))[1]]
                        meaningful_original=[s for s in original_at_location if s.type==1 and s.size>0]
                        if owner_proof and va != owner_va:
                            declared_extent=location.get('alias_extent')
                            if declared_extent is not None:
                                declared_extent=(int(declared_extent,0) if isinstance(declared_extent,str)
                                                 else int(declared_extent))
                                require(declared_extent>0 and declared_extent<=length and
                                        va+declared_extent<=owner_va+owner_proof['size'],
                                        entry['tu']+'/'+location['address']+
                                        ': storage alias extent exceeds its original item or exact owner')
                                length=declared_extent
                            if names:
                                storage_owner_aliases.append(dict(section=section,
                                                                  scaffold_names=sorted(names),
                                                                  address=f'0x{va:08X}',
                                                                  mapped_span=[va,va+length],
                                                                  original_identities=[dict(name=s.name,
                                                                                            binding=('LOCAL' if s.bind==0 else 'GLOBAL' if s.bind==1 else str(s.bind)),
                                                                                            type=('OBJECT' if s.type==1 else str(s.type)),size=s.size)
                                                                                       for s in original_at_location],
                                                                  alias_extent=declared_extent,
                                                                  owner_name=owner_proof['name'],
                                                                  owner_address=owner_proof['address'],
                                                                  owner_size=owner_proof['size'],
                                                                  owner_offset=va-owner_va,
                                                                  c_object_section=owner_proof['section'],
                                                                  c_object_offset=owner_proof['c_object_offset'],
                                                                  owner_binding=owner_proof['binding'],
                                                                  owner_type=owner_proof['type'],
                                                                  owner_object_sha256=owner_proof['object_sha256'],
                                                                  linked_elf_sha256=owner_proof['linked_elf_sha256'],
                                                                  packet_sha256=owner_proof['packet_sha256'],
                                                                  source_sha256=owner_proof['source_sha256'],
                                                                  assembly_sha256=owner_proof['assembly_sha256']))
                        # A frozen allocation can use a fresh C alias as its
                        # packet name while retail ELF preserves a different
                        # local OBJECT spelling (for example TestEnv.0 versus
                        # TestEnv_0_<VA>). Admit only the exact same-address,
                        # same-binding/type/size pair, and require the C name
                        # to be present in this candidate's verified location.
                        for original_symbol in meaningful_original:
                            if any(x['scaffold_name']==original_symbol.name and
                                   x['address']==f'0x{va:08X}' for x in symbol_aliases):
                                continue
                            exact_aliases=[s for s in own if s.value==object_offset and
                                           s.bind==original_symbol.bind and
                                           s.type==original_symbol.type and
                                           s.size==original_symbol.size and
                                           s.name in names]
                            if (len(exact_aliases)==1 and
                                    exact_aliases[0].name != original_symbol.name):
                                exact_alias=exact_aliases[0]
                                symbol_aliases.append(dict(section=section,
                                                           scaffold_name=original_symbol.name,
                                                           object_symbol=exact_alias.name,
                                                           address=f'0x{va:08X}',
                                                           size=original_symbol.size,
                                                           binding=('LOCAL' if original_symbol.bind==0 else
                                                                    'GLOBAL' if original_symbol.bind==1 else
                                                                    str(original_symbol.bind)),
                                                           type='OBJECT'))
                        for original_symbol in meaningful_original:
                            exact_c=[s for s in own if s.name==original_symbol.name and
                                     s.value==object_offset and s.type==original_symbol.type and
                                     s.bind==original_symbol.bind and s.size==original_symbol.size]
                            alias_c=[a for a in symbol_aliases if a['scaffold_name']==original_symbol.name and
                                     int(a['address'],16)==va and a['size']==original_symbol.size and
                                     a['binding']==('LOCAL' if original_symbol.bind==0 else 'GLOBAL' if original_symbol.bind==1 else str(original_symbol.bind)) and
                                     a['type']==('OBJECT' if original_symbol.type==1 else str(original_symbol.type))]
                            require(len(exact_c)==1 or len(alias_c)==1,
                                    entry['tu']+'/'+original_symbol.name+
                                    ': meaningful original OBJECT has no exact compiler object identity at the verified span')
                        # The plan is expressed in the compiler input. The
                        # report hash, however, pins the selected linked object;
                        # describe its split section and rebased offset exactly.
                        selected_section = csec
                        selected_offset = object_offset
                        carve_path = Path(str(obj_path) + '.json')
                        carve = json.loads(carve_path.read_text()) if carve_path.is_file() else {}
                        if csec.name in carve.get('sections', {}):
                            mappings = [mapping for mapping in
                                        carve.get('sections', {}).get(csec.name, {}).get('pieces', [])
                                        if int(mapping['va'], 0) == piece['lo'] and
                                        int(mapping['offset'], 0) == piece['offset'] and
                                        int(mapping['length'], 0) == piece['length']]
                            require(len(mappings) == 1,
                                    entry['tu'] + ': selected span has no unique native carve mapping')
                            mapped = mappings[0]
                            selected_section = next(s for s in obj.sections if s.name == mapped['section'])
                            selected_offset -= piece['offset']
                        else:
                            selected_section = next(s for s in obj.sections if s.name == csec.name)
                            require(selected_section.size == csec.size and
                                    obj.section_bytes(selected_section) == raw_object_elf.section_bytes(csec),
                                    entry['tu'] + ': unsplit selected section differs from the compiler input')
                        require(0 <= selected_offset and selected_offset + length <= selected_section.size,
                                entry['tu'] + ': selected span exceeds the linked object section')
                        verified_location_spans.append(dict(section=section,address=f'0x{va:08X}',
                                                            names=sorted(names),range=[va,va+length],
                                                            c_object_section=selected_section.name,
                                                            c_object_offset=selected_offset,length=length,
                                                            c_object_section_size=selected_section.size,
                                                            c_object_section_alignment=selected_section.align,
                                                            compiler_input_span=dict(section=csec.name,
                                                                                     offset=object_offset,
                                                                                     section_size=csec.size),
                                                            object_symbols=exact_object_symbols,
                                                            original_identities=[dict(name=s.name,address=f'0x{s.value:08X}',
                                                                                      binding=('LOCAL' if s.bind==0 else 'GLOBAL' if s.bind==1 else str(s.bind)),
                                                                                      type=('OBJECT' if s.type==1 else str(s.type)),size=s.size)
                                                                                 for s in original_at_location],
                                                            object_sha256=sha(obj_path),
                                                            split_range=[piece['lo'],piece['lo']+piece['length']],
                                                            scaffold_item=item['name'],
                                                            storage_owner=owner_proof))
                    detail=derived['sections'][section].get('bytes',{})
                if claimed_spans:
                    detail=derived['sections'][section].get('bytes',{})
                    require(detail.get('checked') and detail.get('equal'),
                            entry['tu']+'/'+section+': data diagnostics disagree')
            for location in record['recovered_location_candidates']:
                va=int(location['address'],16)
                if location.get('section') in ('.bss','.sbss'):
                    continue
                if not any(lo<=va<hi for lo,hi in proven_ranges.get(location['section'],[])):
                    direct_span=direct_object_spans.get((location.get('section'),location.get('address')))
                    if direct_span is not None and not storage_only:
                        verified_location_spans.append(direct_span)
                        symbol_aliases.extend(direct_span.get('symbol_aliases', []))
                        proven_ranges.setdefault(location['section'],[]).append(tuple(direct_span['range']))
                    elif carved_data_problem and not storage_only:
                        span=direct_initialized_symbol_span(
                            entry,location,record,packet_tu,ctx,obj,linked,original_elf,
                            original_section_ranges,emitted,obj_path,unit_dir,data_carve)
                        verified_location_spans.append(span)
                        symbol_aliases.extend(span.get('symbol_aliases', []))
                        proven_ranges.setdefault(location['section'],[]).append(tuple(span['range']))
                    else:
                        raise ValueError(entry['tu']+': candidate location lacks current compiler-owned placement '+location['address'])
                verified.append(location)
            if carved_data_problem and not storage_only:
                for problem in initialized_problems:
                    match=re.search(r'run\s+(0x[\da-fA-F]+)-(0x[\da-fA-F]+)',str(problem))
                    require(match is not None,entry['tu']+': unsupported compiler carve diagnostic '+str(problem))
                    problem_range=(int(match[1],16),int(match[2],16))
                    require(any(tuple(span.get('range',[]))==problem_range and
                                span.get('evidence') in ('direct_named_carved_object_exact_bytes_and_link_identity',
                                                         'direct_named_object_exact_bytes_and_link_identity')
                                for span in verified_location_spans),
                            entry['tu']+': unclosed compiler carve diagnostic range '+str(problem_range))
            # Explicit #APP data dependencies may not appear in data_carve's
            # compiler-owned run set because cc1 emitted them while inline ASM
            # was active. Validate each frozen dependency directly through
            # its exact raw/allocated/carved/scaffold/linked byte mapping, but
            # keep it separate from recovered locations and C credit.
            if not storage_only and not input_only:
                for request in record.get('uncredited_data_dependencies', []):
                    if request.get('source', {}).get('inside_app') is not True:
                        continue
                    dep_section=request.get('section')
                    dep_lo=int(request.get('address', '0'), 0)
                    dep_hi=dep_lo+int(request.get('size', 0))
                    if any(d.get('kind')=='inline_assembly_data' and d.get('section')==dep_section and
                           d.get('range')==[dep_lo,dep_hi] for d in dependencies):
                        continue
                    dep_items=data_carve.section_items(ctx,dep_section)
                    dependency=uncredited_inline_asm_data_dependency(
                        entry,record,dep_section,(dep_lo,dep_hi),dep_items,unit_dir,
                        derive_obj,obj_path,linked_path,raw_object_elf,obj,linked,
                        original_elf,ctx,allocation_path,source,asm_path,data_carve)
                    dependencies.append(dependency)
            storage, uncovered_storage, storage_aliases, bss_owner_aliases=linked_storage_proof(
                root,entry,record,allocation,packet_tu,obj,asm_path.read_text(errors='surrogateescape'),
                linked,original_elf,original_section_ranges,obj_path,linked_path,
                allocation_path,source,asm_path)
            symbol_aliases.extend(storage_aliases)
            storage_owner_aliases.extend(bss_owner_aliases)
            for location in record['recovered_location_candidates']:
                if location.get('section') in ('.bss','.sbss'):
                    owner = location.get('storage_owner') or {}
                    va = int(location.get('address', '0'), 16)
                    if owner and va != int(owner.get('address', '0'), 16) and not any(
                            s.value == va and s.type not in (3, 4) and
                            0 < s.shndx < len(original_elf.sections) and
                            original_elf.sections[s.shndx].name == location['section']
                            for s in original_elf.symbols):
                        continue
                    aliases_at_location=[alias['object_symbol'] for alias in storage_aliases
                                         if alias['section']==location.get('section') and
                                         alias['address']==f"0x{int(location.get('address','0'),16):08X}" and
                                         alias['scaffold_name'] in (location.get('names') or [])]
                    if aliases_at_location:
                        materialized=dict(location)
                        materialized['names']=sorted(set((location.get('names') or [])+aliases_at_location))
                        verified.append(materialized)
                    else:
                        verified.append(location)
            symbol_aliases = sorted({(x['section'],x['scaffold_name'],x['object_symbol'],x['address'],x['size']):x
                                     for x in symbol_aliases}.values(),
                                    key=lambda x:(x['section'],x['address'],x['object_symbol']))
            # Link-layout alias rows are not compiler-derived run facts, but
            # they are consumed by the linker/map checker.  Bind every one to
            # an independently verified alias row here; ignoring this
            # metadata during run equality must not turn it into a waiver.
            if not input_only:
                for alias_section, alias_runs in declared.items():
                    for alias_run in alias_runs:
                        for alias in alias_run.get('c_storage_aliases', []):
                            alias_name = alias['original_name']
                            alias_address = f"0x{int(alias['address'],16):08X}"
                            owner = alias['storage_owner']
                            owner_address = f"0x{int(owner['address'],16):08X}"
                            owner_size = int(owner['size'])
                            owner_va = int(owner_address,16)
                            if not any(loc.get('section')==alias_section and
                                       owner_va<=int(loc.get('address','0'),16)<owner_va+owner_size
                                       for loc in record.get('recovered_location_candidates',[])):
                                # An incremental DATA batch claims only its
                                # frozen locations. Keep previously published
                                # aliases outside that allocation unclaimed;
                                # they must not gain new credit or require an
                                # unrelated storage-owner recovery packet.
                                unclaimed_scaffold_runs.append(dict(
                                    section=alias_section,range=[owner_va,owner_va+owner_size],
                                    scaffold_alias=alias_name,credited=False,
                                    reason='previously_published_alias_outside_current_allocation'))
                                continue
                            direct = any(x['section'] == alias_section and
                                         x['scaffold_name'] == alias_name and
                                         x['address'] == alias_address and
                                         x['object_symbol'] == owner['name'] and
                                         x['address'] == owner_address and
                                         x['size'] == owner_size
                                         for x in symbol_aliases)
                            interior = any(x['section'] == alias_section and
                                           alias_name in x['scaffold_names'] and
                                           x['address'] == alias_address and
                                           x['owner_name'] == owner['name'] and
                                           x['owner_address'] == owner_address and
                                           x['owner_size'] == owner_size and x.get('linked_elf_sha256')
                                           for x in storage_owner_aliases)
                            # An initialized owner can be recorded as an alias
                            # of itself by the linker carrier. Bind that row to
                            # its existing complete OBJECT proof, not to the
                            # NOBITS-only storage collection below.
                            initialized_direct = (alias_name == owner['name'] and
                                alias_address == owner_address and any(
                                    proof and span['section'] == alias_section and
                                    alias_name in span['names'] and
                                    span['range'] == [owner_va, owner_va + owner_size] and
                                    proof['name'] == owner['name'] and
                                    proof['address'] == owner_address and proof['size'] == owner_size and
                                    proof['section'] == alias_section and proof['type'] == 'OBJECT' and
                                    proof['object_sha256'] == sha(derive_obj) and
                                    proof['linked_elf_sha256'] == sha(linked_path) and
                                    proof['packet_sha256'] == sha(allocation_path) and
                                    proof['source_sha256'] == sha(source) and
                                    proof['assembly_sha256'] == sha(asm_path) and any(
                                        identity['name'] == alias_name and
                                        identity['address'] == alias_address and
                                        identity['binding'] == proof['binding'] and
                                        identity['type'] == 'OBJECT' and identity['size'] == owner_size
                                        for identity in span['original_identities'])
                                    for span in verified_location_spans
                                    for proof in [span.get('storage_owner')]))
                            if not (direct or interior or initialized_direct):
                                linker_dependencies.append(uncredited_linker_map_alias(
                                    root,entry,alias_section,alias,owner,packet_tu,storage,
                                    original_elf,linked,original_section_ranges,ctx,
                                    data_carve,source,asm_path,linked_path,allocation_path,obj))
                for alias in symbol_aliases:
                    require(any(loc.get('section') == alias['section'] and
                                int(loc.get('address', '0'), 16) == int(alias['address'], 16) and
                                alias['object_symbol'] in (loc.get('names') or []) for loc in verified),
                            entry['tu']+'/'+alias['object_symbol']+': symbol alias lacks a verified location')
            # A frozen location can name both a native C owner and its retail
            # zero-size label. Keep the separately proved linker label in the
            # dependency ledger; it must not become a recovered C identity.
            for dependency in linker_dependencies:
                if (dependency.get('credited') is not False or
                        dependency.get('kind') != 'map_only_storage_alias'):
                    continue
                for location in verified:
                    if (location.get('section') == dependency['section'] and
                            int(location.get('address', '0'), 16) == int(dependency['address'], 16) and
                            dependency['owner_name'] in location.get('names', [])):
                        location['names'] = [name for name in location['names']
                                             if name != dependency['scaffold_name']]
            output['objects'].append(dict(tu=entry['tu'],source_sha256=sha(source),
                                          # The TU audit's compiler-emission
                                          # identity is the raw cc1 object.
                                          # Keep allocation/carve provenance
                                          # separate instead of relabeling a
                                          # derivative as the compiler output.
                                          object_sha256=sha(derive_obj),
                                          compiler_raw_object_path=str(derive_obj),
                                          derivation_input_path=str(derivation_obj),
                                          derivation_input_sha256=sha(derivation_obj),
                                          compiler_allocation_proof=compiler_allocation_proof,
                                          linked_data_object_sha256=sha(obj_path),
                                          linked_data_object_path=str(obj_path),
                                          assembly_sha256=sha(asm_path),
                                          linked_elf_sha256=None if input_only else sha(linked_path),
                                          verified_locations=verified,compiler_owned_ranges=proven_ranges,
                                          verified_location_spans=verified_location_spans,
                                          storage_owner_aliases=storage_owner_aliases,
                                          uncredited_original_item_tails=original_item_tails,
                                          native_unsplit_sections=native_unsplit_sections,
                                          original_object_identities=original_object_identities,
                                          anonymous_constants=anonymous,unclaimed_scaffold_runs=unclaimed_scaffold_runs,
                                          derived_nobits=derived_nobits,
                                          verified_storage=storage,symbol_aliases=symbol_aliases,
                                          supplementary_allocations=auxiliary_allocations,
                                          uncovered_storage_locations=uncovered_storage,
                                          uncredited_linker_dependencies=linker_dependencies,
                                          allocation_packet_sha256=sha(allocation_path)))
            if not input_only:
                dependency = ov02_tu011_uncredited_literal_dependency(
                    entry, record, unit_dir, source, obj_path, asm_path, linked_path,
                    obj, linked, original_elf, allocation_path, ctx, derived)
                if dependency is not None:
                    dependencies.append(dependency)
                dependencies.extend(uncredited_li_s_dependencies(
                    entry, record, unit_dir, source, obj_path, asm_path, linked_path,
                    obj, linked, original_elf, allocation_path, ctx, derived, data_carve))
                dependencies.extend(uncredited_named_object_dependencies(
                    entry, record, unit_dir, source, obj_path, asm_path, linked_path,
                    obj, linked, original_elf, allocation_path, ctx, derived,
                    data_carve))
                dependencies.extend(compiler_switch_word_dependencies(
                    entry, record, unit_dir, source, obj_path, asm_path, linked_path,
                    obj, linked, original_elf, allocation_path, ctx, derived, data_carve))
                if dependencies:
                    output['objects'][-1]['uncredited_compiler_dependencies'] = dependencies
        except (ValueError,OSError,KeyError,StopIteration) as error:
            output['problems'].append(dict(tu=entry['tu'],reason=str(error)))
    output['result']='fail' if output['problems'] else 'pass'
    output['grants_data_recovery'] = not input_only and output['result'] == 'pass'
    output['verified_tus']=len(output['objects'])
    output['verified_locations']=sum(len(o['verified_locations']) for o in output['objects'])
    output['verified_storage_objects']=sum(len(o.get('verified_storage',[])) for o in output['objects'])
    output['uncovered_storage_locations']=sum(len(o.get('uncovered_storage_locations',[])) for o in output['objects'])
    args.output.write_text(json.dumps(output,indent=2)+'\n')
    print(json.dumps(dict(result=output['result'],tus=output['verified_tus'],
                          locations=output['verified_locations'],problems=output['problems'])))
    return bool(output['problems'])


if __name__=='__main__':
    try:raise SystemExit(main())
    except (ValueError,OSError) as error:
        print(str(error),file=sys.stderr);raise SystemExit(1)
