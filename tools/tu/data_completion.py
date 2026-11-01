"""Replace evidenced DATA alignment fragments with linker fill.

No storage definition or recovered-byte counter is manufactured here. A gap
must follow compiler-owned storage, end at the next compiler piece or TU end,
contain no original storage identity and be exactly one bounded ALIGN step.
References refuse except for the separately authenticated direct-array folded
base constructor; that receipt retains every generating site and all owners.
The ordinary map checker and complete original-file gate remain mandatory.
"""
from pathlib import Path
import hashlib
import inspect
import math
import json
import os
import re
import signal
import struct
import subprocess
import sys


def mark_alignment(root, unit_dir, manifest):
    # Configure must work in a clean checkout, before any C object exists.
    # All decisions below depend on originals, source and registered ownership.
    sys.path.insert(0, str(Path(root) / 'tools'))
    from tu.objdiff_inputs import parsed_elf, original_references
    from tu.verify_report import read

    filename = read(Path(root) / 'config/originals.json')['units']['main']['file']
    original = parsed_elf((Path(unit_dir) / 'orig' / filename).read_bytes())
    proofs = []
    for tu in manifest['tus']:
        if tu.get('mode') != 'c' or not tu.get('in_scope'):
            continue
        source = Path(root) / (tu.get('source', {}).get('path') or tu['path'])
        if not source.is_file():
            continue
        text = source.read_text()
        text = re.sub(r'/\*.*?\*/|//[^\n]*', '', text, flags=re.S)
        # Reviewed standalone assembly does not change a C DATA gap's
        # alignment proof. Unrecovered function scaffolding is still excluded.
        if re.search(r'\bINCLUDE_ASM\s*\(', text):
            continue
        for section_name, section in tu['sections'].items():
            pieces = section.get('pieces', [])
            original_section = original.section(section_name)
            for index, piece in enumerate(pieces):
                if piece.get('c_split') or index == 0:
                    continue
                lo, hi = (int(piece[k], 16) for k in ('start', 'end'))
                first = index
                while first and not pieces[first - 1].get('c_split'):
                    first -= 1
                last = index
                while last + 1 < len(pieces) and not pieces[last + 1].get('c_split'):
                    last += 1
                if not first:
                    continue
                group_lo = int(pieces[first]['start'], 16)
                group_hi = int(pieces[last]['end'], 16)
                if int(pieces[first - 1]['end'], 16) != group_lo:
                    continue
                if any(pieces[j]['end'] != pieces[j + 1]['start']
                       for j in range(first, last)):
                    continue
                if last + 1 < len(pieces):
                    following = pieces[last + 1]
                    if int(following['start'], 16) != group_hi:
                        continue
                elif group_hi != int(section['end'], 16):
                    continue
                # Consider only bounded DATA alignments, up to 128 bytes.
                # A larger zero allocation is storage, not inferred alignment.
                if not any((group_lo + n - 1) // n * n == group_hi
                           for n in (2, 4, 8, 16, 32, 64, 128)):
                    continue
                alignment = next((n for n in (2, 4, 8, 16, 32, 64, 128)
                                  if lo < hi and (lo + n - 1) // n * n == hi), None)
                if alignment is None:
                    continue
                if not original_section.addr <= lo < hi <= original_section.addr + original_section.size:
                    continue
                if original_section.type != 8:
                    a = original_section.offset + lo - original_section.addr
                    if any(original.data[a:a + hi - lo]):
                        continue
                # Even unspecified original labels are real identities; never
                # discard one as padding merely because its storage is zero.
                if any(s.shndx == original_section.index and s.type != 3 and s.name
                       and (lo <= s.value < hi or
                            s.size and s.value < hi and lo < s.value + s.size)
                       for s in original.symbols):
                    continue
                if original_references(original, lo, hi):
                    continue
                proof = dict(kind='unreferenced_original_alignment',
                             range=[piece['start'], piece['end']], alignment=alignment,
                             original_symbols_absent=True, original_references_absent=True,
                             zero_or_nobits=True)
                piece['linker_padding'] = proof
                for row in manifest['sections'][section_name]['order']:
                    if row['tu'] == tu['name'] and row['piece'] == piece['name']:
                        row['linker_padding'] = proof
                proofs.append(dict(tu=tu['id'], section=section_name, **proof))
    return proofs


class AlignmentUnproved(ValueError):
    """Leave an unsupported suffix as its existing, uncredited scaffold."""


COMPLETION_FILL_KINDS = ('compiler_alignment_fill', 'external_input_alignment_fill')
EXTERNAL_AUTHORITY = 'config/objects/data-input-provenance.json'


def _require(condition, message):
    if not condition:
        raise AlignmentUnproved(message)


def _hash(data):
    return hashlib.sha256(data).hexdigest()


def _digest(value):
    return _hash(json.dumps(value, sort_keys=True, separators=(',', ':')).encode())


def _apis(root):
    sys.path.insert(0, str(Path(root) / 'tools'))
    import tu_audit
    import tu_edit
    from tu import data_carve, data_gate, objdiff_inputs
    from tu.elfinfo import Elf
    from tu.toolchain import Toolchain
    return data_carve, data_gate, objdiff_inputs, tu_audit, tu_edit, Elf, Toolchain


def _pins(paths):
    out = []
    for path in sorted({str(Path(p).resolve()) for p in paths}):
        p = Path(path)
        _require(p.is_file(), 'missing compiler input: ' + path)
        out.append(dict(path=path, sha256=_hash(p.read_bytes())))
    return out


def _check_pins(pins):
    _require(isinstance(pins, list) and pins, 'missing input pins')
    _require(_pins([p['path'] for p in pins]) == pins, 'compiler inputs changed')


_PP_FACTS_CACHE = {}


def _pp_facts(text, unit_dir, audit, edit):
    # one parse per preprocessed text and unit (each context used to parse it twice)
    key = (hashlib.sha256(text.encode('utf-8', 'surrogateescape')).digest(), str(unit_dir))
    if key not in _PP_FACTS_CACHE:
        _PP_FACTS_CACHE[key] = _pp_facts_uncached(text, unit_dir, audit, edit)
    pp, files, tokens = _PP_FACTS_CACHE[key]
    return pp, _pins([Path(row['path']) for row in files]), tokens


def _pp_facts_uncached(text, unit_dir, audit, edit):
    pp = audit.Preprocessed(text)
    files = []
    for name in pp.files():
        path = Path(name)
        files.append(path if path.is_absolute() else Path(unit_dir) / path)
    # File/line spelling can differ between stdin and the ordinary .i path.
    # Every actual C token and every included file remains independently bound.
    tokens = edit.c_tokens(audit.Preprocessed.MARKER.sub('', text))
    return pp, _pins(files), _digest(tokens)


def _contract_tools(root, contract, Toolchain):
    tc = Toolchain(root)
    recipe = tc.contract(contract['id'])
    compiler_id, assembler_id = recipe['compiler'], recipe['assembler']
    compiler_entry, assembler_entry = tc.entry(compiler_id), tc.entry(assembler_id)
    _require(compiler_entry.get('files', {}).get('cc1') == contract.get('cc1_sha256')
             and recipe['flags'] == contract['flags'], 'compiler recipe differs from TU contract')
    directory = tc.dir(compiler_id)
    if 'path' in assembler_entry:
        _require(assembler_entry.get('sha256') == contract.get('assembler_sha256'),
                 'assembler recipe differs from TU contract')
        assembler = tc.file(assembler_id)
    else:
        names = [name for name, sha in assembler_entry.get('files', {}).items()
                 if sha == contract.get('assembler_sha256')]
        _require(len(names) == 1, 'missing/ambiguous assembler payload in recipe')
        assembler = tc.file(assembler_id, names[0])
    _require(not tc.relaxed, 'relaxed toolchain cannot authenticate compiler alignment')
    _require(_hash((directory/'cc1').read_bytes()) == contract['cc1_sha256'] and
             _hash(assembler.read_bytes()) == contract['assembler_sha256'],
             'actual compiler/assembler differs from TU contract')
    return directory, [directory/name for name in compiler_entry['files']] + [assembler]


def _compiler_object(context, apis):
    """Assemble the authenticated cc1 stream into a seekable memory file."""
    if '_compiler_object' in context:
        return context['_compiler_object']
    carve, gate, native, audit, edit, Elf, Toolchain = apis
    root, unit_dir = Path(context['root']), Path(context['unit_dir'])
    contract = carve._character_tu_record(root, context['tu'])['contract']
    _contract_tools(root, contract, Toolchain)
    tc = Toolchain(root)
    recipe = tc.contract(contract['id'])
    entry = tc.entry(recipe['assembler'])
    names = [n for n, sha in entry.get('files', {}).items() if sha == contract['assembler_sha256']]
    assembler = tc.file(recipe['assembler']) if 'path' in entry else tc.file(recipe['assembler'], names[0])
    _require(not tc.relaxed, 'relaxed assembler cannot witness current constructors')
    fd = os.memfd_create('current-compiler-constructors', 0)
    try:
        argv = [str(assembler), *recipe['assembler_argv'], '-I', str(unit_dir), '-I', str(root),
                '-o', '/proc/self/fd/'+str(fd)]
        process = subprocess.Popen(argv, cwd=unit_dir, env={'PATH':'/usr/bin:/bin', 'LC_ALL':'C'},
                    stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                    pass_fds=(fd,), start_new_session=True)
        try:
            output, error = process.communicate(context['assembly'].encode('utf-8','surrogateescape'), timeout=30)
        except subprocess.TimeoutExpired:
            os.killpg(process.pid, signal.SIGKILL)
            process.communicate()
            raise AlignmentUnproved('memory constructor assembly timed out')
        _require(process.returncode == 0 and not output and not error,
                 'ordinary memory constructor assembly failed')
        size = os.lseek(fd, 0, os.SEEK_END)
        _require(0 < size <= 16*1024*1024, 'memory compiler object exceeds bound')
        os.lseek(fd, 0, os.SEEK_SET)
        data = os.read(fd, size)
    finally:
        os.close(fd)
    _check_pins(context['inputs'])
    context['_compiler_object'] = Elf(data)
    return context['_compiler_object']


def _diagnostic_provenance(match, assembly, source, allowed, edit, provenance):
    name, line = match[3], int(match[2])
    path = Path(match[1])
    if not path.is_absolute(): path = Path(provenance['unit_dir'])/path
    path = path.resolve()
    _require(str(path) in {p['path'] for p in provenance['headers']},
             'diagnostic declaration is not an actual pinned CPP input')
    text = path.read_text(errors='surrogateescape')
    found = [item for item in edit.TU(text).free_items() if item.kind == 'decl'
             and (not item.conditional or path != source.resolve())
             and 'static' in edit.c_tokens(item.text(text)) and name in edit.c_tokens(item.text(text))
             and '(' in edit.c_tokens(item.text(text)) and
             edit.line_of(text,item.start) <= line <= edit.line_of(text,item.end)]
    _require(len(found) == 1, 'diagnostic lacks one real source/header static declaration')
    tokens = edit.c_tokens(found[0].text(text))
    pp_text, pp = provenance['preprocessed'], provenance['pp']
    effective = []
    for item in edit.TU(pp_text).free_items():
        if item.kind != 'decl' or item.conditional or edit.c_tokens(item.text(pp_text)) != tokens: continue
        origin, at = pp.attribute(item.start)
        origin = Path(origin or '')
        if not origin.is_absolute(): origin = Path(provenance['unit_dir'])/origin
        if origin.resolve() == path and edit.line_of(text,found[0].start) <= at <= edit.line_of(text,found[0].end):
            effective.append(item)
    _require(len(effective) == 1, 'diagnostic header declaration is not unique effective CPP syntax')
    alias = None
    if name not in allowed:
        native, audit = provenance['native'], provenance['audit']
        authored = native.actual_source_asm_lines(pp_text)
        aliases = []
        for raw in assembly.splitlines():
            include = re.fullmatch(r'\s*\.include\s+"([^"\n]+\.s)"\s*',raw)
            if not include or Path(include[1]).stem not in allowed or audit._norm_asm_line(raw.strip()) not in authored:
                continue
            file = Path(provenance['unit_dir'])/include[1]
            _require(file.is_file() and file.stat().st_size <= 1024*1024, 'missing/big diagnostic ASM alias input')
            body_bytes = file.read_bytes()
            body = body_bytes.decode('utf-8','surrogateescape')
            owned = re.findall(r'^\s*nonmatching\s+([A-Za-z_]\w*)\s*,\s*(0x[\da-fA-F]+|[0-9]+)\s*$',body,re.M)
            labels = re.findall(r'^\s*glabel\s+([A-Za-z_]\w*)\s*,\s*local\s*$',body,re.M)
            if len(owned) != 1 or owned[0][0] != Path(include[1]).stem or labels != [name]: continue
            originals = [s for s in provenance['original'].symbols if s.name == name and
                         s.type == 2 and s.bind == 0 and s.size == int(owned[0][1],0) and
                         any(f['name'] in (name,owned[0][0]) and _number(f['va']) == s.value and
                             f['size'] == s.size for f in provenance['tu']['functions'])]
            if len(originals) != 1: continue
            symbol = originals[0]
            if not any(f['name'] in (name,owned[0][0]) and _number(f['va']) == symbol.value and
                       f['size'] == symbol.size for f in provenance['tu']['functions']): continue
            pins = _pins([file])
            _require(pins[0]['sha256'] == _hash(body_bytes), 'diagnostic alias changed while being read')
            provenance['extra_inputs'].extend(pins)
            aliases.append(dict(scaffold=owned[0][0],original_name=name,va=symbol.value,size=symbol.size,
                                input=pins[0]))
        _require(len(aliases) == 1, 'diagnostic name lacks one explicit original/scaffold local alias')
        alias = aliases[0]
    return dict(name=name,line=line,kind='undefined_static_INCLUDE_ASM',source=str(path),
                source_declaration_sha256=_digest(tokens),**({'scaffold_alias':alias} if alias else {}))


def _context_process(argv, unit_dir, data, source, allowed, edit, provenance=None):
    """Only an exact undefined-static scaffold diagnostic can be incidental."""
    process = subprocess.Popen(argv, cwd=unit_dir, env={'PATH':'/usr/bin:/bin','LC_ALL':'C'},
                    stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                    start_new_session=True)
    try:
        output, error = process.communicate(data, timeout=30)
    except subprocess.TimeoutExpired:
        os.killpg(process.pid, signal.SIGKILL)
        process.communicate()
        raise AlignmentUnproved('compiler context step timed out')
    _require(process.returncode == 0 and len(output) <= 8*1024*1024,
             'compiler context step failed')
    diagnostics = []
    for line in error.decode('utf-8','surrogateescape').splitlines():
        match = re.fullmatch(r"(.+):([0-9]+)(?::[0-9]+)?: warning: [`']([A-Za-z_]\w*)' used but never defined", line)
        if match is not None and provenance is not None:
            diagnostics.append(_diagnostic_provenance(match,output.decode('utf-8','surrogateescape'),
                                                     source,allowed,edit,provenance))
            continue
        _require(match is not None and match[3] in allowed and
                 Path(match[1]).resolve() == source.resolve(),
                 'unsupported compiler context diagnostic: '+line[:200])
        text = source.read_text(errors='surrogateescape')
        declarations = [item for item in edit.TU(text).free_items() if item.kind == 'decl'
                        and not item.conditional and 'static' in edit.c_tokens(item.text(text))
                        and match[3] in edit.c_tokens(item.text(text))
                        and '(' in edit.c_tokens(item.text(text))
                        and edit.line_of(text,item.start) <= int(match[2]) <= edit.line_of(text,item.end)]
        _require(len(declarations) == 1, 'diagnostic lacks exact source static scaffold declaration')
        diagnostics.append(dict(name=match[3],line=int(match[2]),kind='undefined_static_INCLUDE_ASM',
                                source_declaration_sha256=_digest(edit.c_tokens(declarations[0].text(text)))))
    return output, diagnostics


def _source_context(root, unit_dir, tu, apis, require_complete=True):
    carve, gate, native, audit, edit, Elf, Toolchain = apis
    record = carve._character_tu_record(root, tu['id'])
    source = Path(root) / record['path']
    _require(source.resolve() == (Path(root) / tu['path']).resolve(), 'TU source route differs')
    _require(source.is_file() and source.stat().st_size <= 1024 * 1024, 'missing/big C source')
    _require(not require_complete or not carve.source_scaffold_names(source)[0],
             'complete-C alignment owner retains function scaffolding')
    contract = record['contract']
    _require(contract.get('flags') in (['-O2', '-G0'], ['-O2', '-G8']), 'unsupported TU flags')
    directory, tool_paths = _contract_tools(root, contract, Toolchain)
    base = [str(directory / 'ee-gcc'), '-B' + str(directory) + '/', '-nostdinc', '-fno-builtin']
    original = Path(unit_dir) / 'orig' / json.loads(
        (Path(root) / 'config/originals.json').read_bytes())['units']['main']['file']
    dependencies = [source, original, Path(root) / 'config/originals.json',
                    Path(root) / 'config/toolchain-identity.json', carve.registry_input_path(root),
                    Path(root) / ('config/tu-build.json' if (Path(root) / 'config/tu-build.json').is_file()
                                  else 'config/objects/main.compile.json'),
                    Path(__file__), Path(carve.__file__), Path(gate.__file__),
                    Path(native.__file__), Path(audit.__file__), Path(edit.__file__),
                    Path(sys.modules[Toolchain.__module__].__file__), *tool_paths]
    before = _pins(dependencies)
    cpp_argv = base + ['-E'] + contract['flags'] + [
        '-I', 'include', '-I', str(Path(root) / 'include'), str(source), '-o', '/dev/stdout']
    preprocessed = carve._character_process(cpp_argv, unit_dir)
    pp_text = preprocessed.decode(errors='surrogateescape')
    pp, headers, tokens = _pp_facts(pp_text, unit_dir, audit, edit)
    _check_pins(before)
    before = _pins([p['path'] for p in before+headers])
    cc1_argv = base + ['-S'] + contract['flags'] + [
        '-x', 'cpp-output', '/dev/stdin', '-o', '/dev/stdout']
    scaffold = set(carve.source_scaffold_names(source)[0]) if not require_complete else set()
    diagnostic_context = dict(unit_dir=str(unit_dir),preprocessed=pp_text,pp=pp,headers=headers,
                              native=native,audit=audit,original=Elf(original.read_bytes()),
                              tu=tu,extra_inputs=[])
    assembly, diagnostics = _context_process(cc1_argv, unit_dir, preprocessed, source, scaffold, edit,
                                              diagnostic_context if not require_complete else None)
    _check_pins(before)
    before = _pins([p['path'] for p in before+diagnostic_context['extra_inputs']])
    _check_pins(before)
    pp_text, asm_text = preprocessed.decode(errors='surrogateescape'), assembly.decode(errors='surrogateescape')
    pp, headers, tokens = _pp_facts(pp_text, unit_dir, audit, edit)
    required = {f['name'] for f in tu.get('functions', [])}
    emitted = gate.cc1_functions(asm_text)
    _require(required and (required <= emitted if require_complete else bool(required & emitted)),
             'TU lacks the required compiler-emitted C functions')
    return dict(source=source, source_text=source.read_text(errors='surrogateescape'),
                preprocessed=pp_text, pp=pp, assembly=asm_text, inputs=before,
                headers=headers, preprocessed_tokens_sha256=tokens,
                contract_sha256=_digest(contract), original=Elf(original.read_bytes()),
                generated_preprocessed_sha256=_hash(preprocessed),
                generated_assembly_sha256=_hash(assembly), argv=[cpp_argv, cc1_argv],
                tu=tu['id'], root=str(root), unit_dir=str(unit_dir), require_complete=require_complete,
                compiler_diagnostics=diagnostics,
                function_scope=[dict(original_symbol=f['name'],name=f['name'],
                                     va=_number(f['va']),size=f['size']) for f in tu['functions']])


def _compiler_pool_lines(assembly, preprocessed, unit_dir, native, audit):
    try:
        return native.compiler_pool_line_numbers(assembly, preprocessed, unit_dir)
    except RuntimeError as error:
        _require(str(error) == 'Unsupported deferred compiler pool APP origin',
                 'unsupported compiler pool provenance error: '+str(error))
        # cc1 may defer NO_APP across a literal pool after an actual TEXT-only
        # INCLUDE_ASM wrapper. Authenticate that exact authored prefix rather
        # than treating an unrelated include as compiler DATA provenance.
        parsed = native.compiler_assembly(assembly)
        _require(not parsed.errors and parsed.pool_lines, 'unsupported deferred compiler pool')
        lines = assembly.splitlines()
        authored = native.actual_source_asm_lines(preprocessed)
        numbers = {number for number, code in parsed.pool_lines}
        remaining = set(numbers)
        while remaining:
            first = min(remaining)
            markers = [i for i, line in enumerate(lines[:first-1],1)
                       if line.strip() in ('#APP','#NO_APP')]
            _require(markers and lines[markers[-1]-1].strip() == '#APP',
                     'deferred pool lacks exact APP boundary')
            begin = markers[-1]
            finish = next((i for i in range(first,len(lines)+1) if lines[i-1].strip()=='#NO_APP'),None)
            _require(finish is not None, 'deferred pool lacks NO_APP')
            prefix = [audit._strip_comment(line).strip() for line in lines[begin:first-1]
                      if audit._strip_comment(line).strip()]
            includes = [line for line in prefix if re.fullmatch(r'\.include\s+"[^"\n]+\.s"',line)]
            _require(len(includes) == 1 and prefix and prefix[0] in ('.section .text','.text')
                     and all(re.fullmatch(r'\.(?:section\s+\.text|text|set\s+(?:noat|at|noreorder|reorder)|align\s+[0-7])',line)
                             or line in includes for line in prefix)
                     and all(audit._norm_asm_line(line) in authored for line in prefix),
                     'deferred pool origin is not exact authored TEXT-only wrapper')
            group = {i for i in remaining if first <= i < finish}
            _require(group, 'empty deferred pool region')
            for number, code in parsed.pool_lines:
                if number in group:
                    _require(audit._app_line_allowed(code) or audit._norm_asm_line(code) not in authored,
                             'deferred pool payload is authored ASM')
            remaining -= group
        return frozenset(numbers)


def _ordinary_lines(assembly, preprocessed, unit_dir, native, audit):
    parsed = native.compiler_assembly(assembly)
    _require(not parsed.errors, 'cc1 parser errors')
    pool = _compiler_pool_lines(assembly, preprocessed, unit_dir, native, audit)
    app, out, app_text = False, [], False
    for number, line in enumerate(assembly.splitlines(), 1):
        if line.strip() == '#APP':
            app = True
            app_text = False
            continue
        if line.strip() == '#NO_APP':
            app = False
            continue
        code = audit._strip_comment(line).strip()
        if not code:
            continue
        if app and number not in pool:
            # Instruction-only C inline asm is allowed. No authored storage,
            # alignment or section-stack operation can seed a DATA proof.
            text_switch = re.fullmatch(r'\.(?:section\s+\.text|text)', code)
            if text_switch:
                app_text = True
                out.append('.text')
            _require(not code.startswith('.') or
                     re.fullmatch(r'\.(?:set|option)\s+[^;]+', code) or
                     text_switch or (app_text and re.fullmatch(r'\.align\s+[0-7]', code)) or
                     re.fullmatch(r'\.extern\s+[A-Za-z_]\w*\s*,\s*[0-9]+', code) or
                     re.fullmatch(r'\.include\s+"[^"\n]+"', code),
                     'authored ASM may supply DATA/alignment: ' + code[:80])
            continue
        out.append(code)
    return out


def _assembly_dependencies(assembly, unit_dir, section, apis):
    """Pin every include and refuse storage hidden in an included function/macro."""
    audit = apis[3]
    seen = set()
    payload = re.compile(r'^\s*\.(?:byte|half|short|2byte|word|long|4byte|dword|quad|8byte|'
                         r'float|double|ascii|asciz|string|space|skip|zero|fill|org|incbin|'
                         r'comm|lcomm|pushsection|popsection|subsection|previous|macro|'
                         r'rdata|rodata|sdata|data|lit4|lit8|bss|sbss)\b')
    def inspect(text, parent, depth):
        _require(depth <= 16 and len(seen) <= 256, 'ASM include bounds exceeded')
        for name in re.findall(r'^\s*\.include\s+"([^"\n]+)"\s*$', text, re.M):
            choices = [Path(unit_dir)/name, parent/name]
            path = next((p.resolve() for p in choices if p.is_file()), None)
            _require(path is not None, 'missing ordinary ASM include: ' + name)
            if path in seen:
                continue
            seen.add(path)
            data = path.read_bytes()
            if path.name == 'labels.inc':
                _require(_hash(data) == '2cff19a41f1917f2acf5516b3e3f06b69195340442afe65fad70fb0fc694de19',
                         'macro-only labels include differs')
                continue
            body = data.decode(errors='surrogateescape')
            parsed = audit.AsmFile(body)
            _require(not parsed.errors and not any(payload.match(line) or
                     re.match(r'^\s*\.section\s+["\']?\.(?!text\b)', line)
                     for line in body.splitlines()),
                     'ASM include contains unsupported DATA/section-stack directive')
            inspect(body, path.parent, depth+1)
    inspect(assembly, Path(unit_dir), 0)
    return _pins(seen) if seen else []


def _compiler_listing(context, apis):
    if '_compiler_listing' in context:
        return context['_compiler_listing']
    carve, gate, native, audit, edit, Elf, Toolchain = apis
    compiler = _compiler_object(context,apis)
    root, unit_dir = Path(context['root']),Path(context['unit_dir'])
    contract = carve._character_tu_record(root,context['tu'])['contract']
    _contract_tools(root,contract,Toolchain)
    tc = Toolchain(root);recipe = tc.contract(contract['id']);entry = tc.entry(recipe['assembler'])
    names = [n for n,sha in entry.get('files',{}).items() if sha == contract['assembler_sha256']]
    assembler = tc.file(recipe['assembler']) if 'path' in entry else tc.file(recipe['assembler'],names[0])
    _require(not tc.relaxed,'relaxed assembler listing cannot prove a constructor')
    fd = os.memfd_create('compiler-constructor-listing',0)
    try:
        argv = [str(assembler),*recipe['assembler_argv'],'-aln','-I',str(unit_dir),'-I',str(root),
                '-o','/proc/self/fd/'+str(fd)]
        process = subprocess.Popen(argv,cwd=unit_dir,env={'PATH':'/usr/bin:/bin','LC_ALL':'C'},
             stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=subprocess.PIPE,pass_fds=(fd,),start_new_session=True)
        try: listing,error = process.communicate(context['assembly'].encode('utf-8','surrogateescape'),timeout=30)
        except subprocess.TimeoutExpired:
            os.killpg(process.pid,signal.SIGKILL);process.communicate()
            raise AlignmentUnproved('compiler listing replay timed out')
        _require(process.returncode == 0 and not error and 0 < len(listing) <= 16*1024*1024,
                 'ordinary compiler listing replay failed')
        size = os.lseek(fd,0,os.SEEK_END);os.lseek(fd,0,os.SEEK_SET)
        _require(size == len(compiler.data) and os.read(fd,size) == compiler.data,
                 'listing replay differs from whole ordinary compiler object')
    finally: os.close(fd)
    _check_pins(context['inputs'])
    context['_compiler_listing'] = listing.decode('utf-8','surrogateescape')
    return context['_compiler_listing']


def _register_double_immediates(context, apis):
    """Closed, nonstorage ORI/DSLL32 expansions of actual cc1 li.d lines."""
    carve,gate,native,audit,edit,Elf,Toolchain = apis
    compiler = _compiler_object(context,apis);listing = _compiler_listing(context,apis)
    authored = native.actual_source_asm_lines(context['preprocessed'])
    function, app, result, occurrences = None,False,{},{}
    number = r'[-+]?(?:[0-9]+(?:\.[0-9]*)?|\.[0-9]+)(?:[eE][-+]?[0-9]+)?'
    for line_number,line in enumerate(context['assembly'].splitlines(),1):
        if line.strip() == '#APP': app=True;continue
        if line.strip() == '#NO_APP': app=False;continue
        if app: continue
        ent = re.fullmatch(r'\s*\.ent\s+(\S+)\s*',line)
        end = re.fullmatch(r'\s*\.end\s+(\S+)\s*',line)
        if ent: function=ent[1];continue
        if end: function=None;continue
        m = re.fullmatch(r'\s*li\.d\s+\$([0-9]+),\s*('+number+r')\s*',line)
        if not m or not function: continue
        occurrence=occurrences.get(function,0);occurrences[function]=occurrence+1
        _require(audit._norm_asm_line(line.strip()) not in authored,'li.d constructor is authored ASM')
        register = int(m[1]);value=float(m[2])
        _require(0 < register < 31 and math.isfinite(value),'unsupported li.d immediate value/register')
        payload = struct.pack('<d',value)
        rows = [r for r in listing.splitlines() if re.match(r'^\s*'+str(line_number)+r'\s',r)]
        first = re.fullmatch(r'\s*'+str(line_number)+r'\s+([0-9A-Fa-f]{4,8})\s+([0-9A-Fa-f]{8})\s+(.*?)\s*',rows[0]) if rows else None
        second = re.fullmatch(r'\s*'+str(line_number)+r'\s+([0-9A-Fa-f]{8})\s*',rows[1]) if len(rows)==2 else None
        # GAS prints source tabs as listing whitespace. Bind every source
        # token; no operand, expression or emitted byte is normalized.
        if not first or not second or first[3].split() != line.split(): continue
        code = bytes.fromhex(first[2]+second[1]);upper,shift = struct.unpack('<2I',code)
        if upper >> 26 != 13 or upper >> 21 & 31 or upper >> 16 & 31 != register: continue
        amount = ((shift >> 6)&31)+32
        if shift != ((register<<16)|(register<<11)|((amount-32)<<6)|0x3c): continue
        if (((upper & 0xffff)<<amount)&0xffffffffffffffff).to_bytes(8,'little') != payload: continue
        functions = [s for s in compiler.symbols if s.name == function and s.type == 2]
        scope = [s for s in context['function_scope'] if s['name'] == function]
        _require(len(functions) == len(scope) == 1,'immediate li.d lacks unique function identity')
        f,scope = functions[0],scope[0];section=compiler.sections[f.shndx];offset=int(first[1],16)
        _require(section.name == '.text' and f.value <= offset <= f.value+f.size-8 and
                 compiler.section_bytes(section)[offset:offset+8] == code and f.size == scope['size'],
                 'listing immediate does not match actual complete compiler function')
        _require(not any(rel.type in (4,9) and rel.info == section.index and
                         any(offset <= p < offset+8 for p,k,n,a in carve._rel_entries(compiler,rel))
                         for rel in compiler.sections),'register-immediate li.d has a relocation')
        original_functions = [s for s in context['original'].symbols if s.name == function and
                              (s.type,s.bind,s.size,s.value)==(2,f.bind,f.size,scope['va'])]
        _require(len(original_functions)==1 and carve.va_bytes(context['original'],scope['va']+offset-f.value,8)==code,
                 'register-immediate li.d differs from original instructions')
        source_witnesses=[];literal_count=None
        for text in (context['source_text'],context['preprocessed']):
            fs=[x for x in edit.scan(text) if x.kind=='func' and x.name==function]
            _require(len(fs)==1,'register-immediate li.d lacks a unique C function')
            decls=[x for x in native.source_matches(r'\bdouble\s+[A-Za-z_]\w*\s*=\s*('+number+r')\s*;',fs[0].text(text))
                   if struct.pack('<d',float(x[1]))==payload]
            if len(decls)==1:
                source_witnesses.append(edit.c_tokens(decls[0][0]))
            else:
                # An unsuffixed floating expression operand is also double.
                # Retain the complete identical C/CPP function token sequence;
                # suffixed float/long-double tokens cannot enter this route.
                tokens=edit.c_tokens(fs[0].text(text))
                floating=r'(?:[0-9]+\.[0-9]*|\.[0-9]+)(?:[eE][-+]?[0-9]+)?|[0-9]+[eE][-+]?[0-9]+'
                indices=[i for i,token in enumerate(tokens) if re.fullmatch(floating,token) and
                         struct.pack('<d',float(token))==payload]
                _require(indices,'register-immediate li.d lacks an exact C double expression literal')
                source_witnesses.append(dict(kind='direct-unsuffixed-double-expression',
                                             function=function,tokens=tokens,literal_indices=indices))
                literal_count=len(indices)
        _require(source_witnesses[0]==source_witnesses[1],'li.d double initializer differs in source/CPP')
        if literal_count is not None:
            pp_function=next(x for x in edit.scan(context['preprocessed']) if x.kind=='func' and x.name==function)
            origin,at=context['pp'].attribute(pp_function.start)
            origin=Path(origin or '')
            if not origin.is_absolute():origin=Path(context['unit_dir'])/origin
            _require(origin.resolve()==context['source'].resolve(),
                     'double expression function has a foreign CPP origin')
        key=(function,register,payload.hex(),occurrence)
        _require(key not in result,'ambiguous repeated register-immediate li.d')
        result[key]=dict(function=function,destination=register,binary64=payload.hex(),line=line_number,
                         occurrence=occurrence,**({'source_expression_literal_count':literal_count} if literal_count is not None else {}),
                         offset=offset,words=[upper,shift],original_va=scope['va']+offset-f.value,
                         initializer_sha256=_digest(source_witnesses[0]),listing_sha256=_hash(listing.encode('utf-8','surrogateescape')),
                         constructor='ORI-zero-u16;DSLL32-same-register',data_bytes=0)
    for item in result.values():
        if 'source_expression_literal_count' in item:
            _require(sum(x['function']==item['function'] and x['binary64']==item['binary64'] for x in result.values())
                     ==item['source_expression_literal_count'],
                     'double expression literals and actual nonstorage constructors differ')
    context['_register_double_listing_diagnostics'] = list(result.values())
    return result


def _layout(assembly, preprocessed, unit_dir, section, apis, context=None):
    carve, gate, native, audit, edit, Elf, Toolchain = apis
    _require(section in ('.rodata', '.sdata', '.data'), 'unsupported alignment section')
    included = _assembly_dependencies(assembly, unit_dir, section, apis)
    # The established positive source/cc1 guard remains mandatory. Its native
    # wrapper has an older labels.inc-only deferred-pool restriction; the
    # alternate path authenticates each exact TEXT wrapper above, then reuses
    # the same positive guard and include view on the unchanged full stream.
    try:
        provenance = native.compiler_only_section(assembly, preprocessed, Path(unit_dir), section)
    except RuntimeError as error:
        _require(str(error) == 'Unsupported deferred compiler pool APP origin',
                 'unsupported compiler section provenance error: '+str(error))
        _require(_compiler_pool_lines(assembly, preprocessed, unit_dir, native, audit),
                 'alternate section provenance lacks authenticated deferred pool')
        view = native._compiler_include_views.get(assembly, Path(unit_dir))
        provenance = gate.compiler_only_section(assembly, preprocessed, view, section)
    _require(provenance, 'missing positive compiler DATA provenance')
    lines = _ordinary_lines(assembly, preprocessed, unit_dir, native, audit)
    pools, pool_index, immediates, used_immediates = [], 0, {}, set()
    if section == '.rodata' and re.search(r'^\s*li\.d\s+', assembly, re.M):
        _require(context is not None, 'assembler constructor needs authenticated source context')
        compiler = _compiler_object(context, apis)
        immediates = _register_double_immediates(context,apis)
        registry = carve.load_registry(context['root'])['tus'][context['tu']]
        pools = native.registered_assembler_double_pools(context['original'],compiler,assembly,
                    preprocessed,registry,context['function_scope'],Path(unit_dir))
        pools = sorted(pools, key=lambda p:p[2])
        _require((pools or immediates) and len({p[2] for p in pools}) == len(pools),
                 'li.d constructors lack unique registered native source witnesses')
    types, sizes, globals_, weak, commons = {}, {}, set(), set(), []
    for code in lines:
        common = re.fullmatch(r'\.comm\s+([A-Za-z_.$][\w.$]*)\s*,\s*([0-9]+)\s*,\s*([0-9]+)', code)
        if common:
            size, alignment = int(common[2]), int(common[3])
            _require(0 < size <= 8*1024*1024 and 0 < alignment <= 128
                     and alignment & (alignment-1) == 0
                     and not any(c['name'] == common[1] for c in commons),
                     'unsupported/duplicate ordinary COMMON declaration')
            commons.append(dict(name=common[1], size=size, alignment=alignment, binding=1))
            continue
        m = re.fullmatch(r'\.type\s+([\w.$]+)\s*,\s*@object', code)
        if m:
            _require(m[1] not in types, 'duplicate OBJECT type')
            types[m[1]] = 1
        m = re.fullmatch(r'\.size\s+([\w.$]+)\s*,\s*(0x[0-9a-fA-F]+|[0-9]+)', code)
        if m:
            _require(m[1] not in sizes, 'duplicate OBJECT size')
            sizes[m[1]] = int(m[2], 0)
        m = re.fullmatch(r'\.(globl|global|weak)\s+([\w.$]+)', code)
        if m:
            (weak if m[1] == 'weak' else globals_).add(m[2])
    current, previous, cursor, events, labels, requirement = '.text', '.text', 0, [], {}, 1
    function = None
    literal_occurrences = {}
    metadata = re.compile(r'\.(?:type|size|globl|global|local|weak|hidden|extern|file|loc|'
                          r'stabs|stabn|ident|set|option|ent|end|frame|mask|fmask)\b')
    for code in lines:
        ent = re.fullmatch(r'\.ent\s+(\S+)',code)
        end_function = re.fullmatch(r'\.end\s+(\S+)',code)
        if ent:
            function = ent[1]
        if end_function:
            function = None
        literal = re.fullmatch(r'li\.d\s+\$(\d+),\s*([-+]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][-+]?\d+)?)',code)
        if literal and section == '.rodata':
            occurrence=literal_occurrences.get(function,0);literal_occurrences[function]=occurrence+1
            key=(function,int(literal[1]),struct.pack('<d',float(literal[2])).hex(),occurrence)
            if key in immediates:
                _require(current=='.text' and key not in used_immediates,'repeated/nonTEXT immediate li.d')
                used_immediates.add(key)
                continue
            _require(current == '.text' and function and pool_index < len(pools),
                     'unwitnessed li.d source constructor')
            template, va, offset, span, consumers = pools[pool_index]
            source = template['source']
            payload = struct.pack('<d',float(literal[2]))
            _require(source['function'] == function and payload == template['payload']
                     and any(p['destination'] == int(literal[1]) and
                             struct.pack('<d',float(p['spelling'])) == payload
                             for p in source['actual_cc1_li_d'])
                     and offset == (cursor+7)&-8,
                     'assembler pool sequence differs from actual C literal constructor')
            if cursor != offset:
                events.append(dict(kind='align',offset=cursor,end=offset,alignment=8,
                                   text=code,assembler_constructor_alignment=True))
            cursor = offset+8
            actual_alignment = compiler.section('.rodata').align
            _require(actual_alignment == 16, 'unsupported actual li.d ELF input alignment')
            requirement = max(requirement,actual_alignment)
            events.append(dict(kind='payload',offset=offset,end=cursor,text=code,
                               natural_alignment=8,assembler_double_pool=True,
                               input_alignment=actual_alignment,payload_hex=payload.hex(),
                               native_witness_sha256=_digest(dict(source=source,consumers=consumers,
                                                                 original_address=va,span=span))))
            pool_index += 1
            continue
        m = re.match(r'\.section\s+"?([^,"\s]+)', code)
        simple = re.fullmatch(r'\.(text|rdata|rodata|sdata|data|lit4|lit8|bss|sbss|scommon)', code)
        if m or simple:
            previous, current = current, m[1] if m else '.' + simple[1]
            if current == '.rdata':
                current = '.rodata'
            continue
        if code == '.previous':
            current, previous = previous, current
            continue
        _require(not re.match(r'\.(?:pushsection|popsection|subsection)\b', code), 'unsupported section stack')
        if re.fullmatch(r'\.comm\s+([A-Za-z_.$][\w.$]*)\s*,\s*([0-9]+)\s*,\s*([0-9]+)', code):
            # COMMON allocates in its own linker namespace, irrespective of
            # the active section. Runtime checks the actual C declaration and
            # raw/allocated identity; no active DATA bytes are added here.
            continue
        if current != section:
            continue
        label = re.fullmatch(r'([A-Za-z_.$][\w.$]*):', code)
        if label:
            _require(label[1] not in labels, 'duplicate data label')
            labels[label[1]] = cursor
            events.append(dict(kind='label', offset=cursor, end=cursor, text=code))
            continue
        if metadata.match(code):
            continue
        align = re.fullmatch(r'\.align\s+([0-7])', code)
        if align:
            exponent = int(align[1])
            _require(exponent != 0, 'automatic alignment disabled')
            requirement = max(requirement, 1 << exponent)
            end = (cursor + (1 << exponent) - 1) & -(1 << exponent)
            events.append(dict(kind='align', offset=cursor, end=end,
                               alignment=1 << exponent, text=code))
            cursor = end
            continue
        string = re.fullmatch(r'\.(ascii|asciz)\s+("(?:[^"\\]|\\.)*")', code)
        space = re.fullmatch(r'\.(?:space|zero|skip)\s+(0x[0-9a-fA-F]+|[0-9]+)', code)
        scalar = re.fullmatch(r'\.(byte|half|short|2byte|word|long|4byte|float|dword|8byte|double)\s+(.+)', code)
        if string:
            width, size = 1, len(native.c_literal_bytes(string[2])) + (string[1] == 'asciz')
        elif space:
            width, size = 1, int(space[1], 0)
        elif scalar:
            width = {'byte': 1, 'half': 2, 'short': 2, '2byte': 2, 'word': 4,
                     'long': 4, '4byte': 4, 'float': 4, 'dword': 8, '8byte': 8, 'double': 8}[scalar[1]]
            values = scalar[2].split(',')
            _require(all(re.fullmatch(r'[A-Za-z0-9_.$+*/()%\s-]+', value) and value.strip()
                         for value in values), 'unsupported scalar expression')
            size = width * len(values)
        else:
            raise AlignmentUnproved('unsupported cc1 DATA directive: ' + code[:80])
        requirement = max(requirement, width)
        start = (cursor + width - 1) & -width
        if start != cursor:
            _require(not any(cursor <= off < start for off in labels.values()),
                     'implicit alignment has a preceding label')
            events.append(dict(kind='align', offset=cursor, end=start, alignment=width,
                               text=code, automatic_scalar_alignment=True))
            cursor = start
        cursor += size
        _require(0 <= cursor <= 8 * 1024 * 1024, 'compiler section exceeds bound')
        events.append(dict(kind='payload', offset=start, end=cursor, text=code,
                           natural_alignment=width))
    _require(pool_index == len(pools), 'registered assembler constructor was not emitted')
    _require(used_immediates == set(immediates),'register-immediate li.d was not emitted')
    objects = {name: dict(name=name, offset=value, size=sizes[name], type=1,
                         binding=2 if name in weak else int(name in globals_))
               for name, value in labels.items() if name in types and name in sizes and sizes[name] > 0}
    return dict(section=section, size=cursor, alignment=requirement,
                events=events, objects=objects, labels=labels,
                assembly_inputs=included, commons=commons,
                source_provenance_sha256=_digest(provenance),
                **({'register_double_immediates':[
                    {k:v for k,v in item.items() if k not in ('line','listing_sha256')}
                    for item in immediates.values()]} if immediates else {}))


_DECL_CACHE = {}


def _initialized_decls(text, edit):
    """[(item, tokens, declarator head tokens)] of a source text's initialized
    file-scope declarations, parsed once per text (2026-10-31: the alignment
    proofs looked up every name by re-tokenizing the whole TU, ~100 s/configure)."""
    key = hashlib.sha256(text.encode('utf-8', 'surrogateescape')).digest()
    rows = _DECL_CACHE.get(key)
    if rows is None:
        rows = []
        for item in edit.TU(text).free_items():
            if item.kind != 'decl' or item.conditional:
                continue
            tokens = edit.c_tokens(item.text(text))
            if '=' in tokens:
                rows.append((item, tokens, frozenset(tokens[:tokens.index('=')])))
        _DECL_CACHE[key] = rows
    return rows


def _definition(context, name, apis):
    carve, gate, native, audit, edit, Elf, Toolchain = apis
    def find(text):
        found = [(item, tokens) for item, tokens, head in _initialized_decls(text, edit)
                 if name in head]
        _require(len(found) == 1, name + ': missing/ambiguous initialized source declaration')
        return found[0]
    try:
        original, tokens = find(context['source_text'])
        actual, pp_tokens = find(context['preprocessed'])
    except AlignmentUnproved:
        compiled = _compiler_object(context, apis)
        bindings = native.function_static_source_bindings(context['preprocessed'], compiled, context['assembly'])
        _require(name in bindings, name + ': no unique actual lexical static witness')
        binding = bindings[name]
        symbols = [s for s in compiled.symbols if s.name == name and s.bind == 0 and s.type == 1]
        _require(len(symbols) == 1 and symbols[0].size > 0, 'lexical static lacks actual LOCAL OBJECT')
        def lexical(text):
            functions = [f for f in edit.scan(text) if f.kind == 'func' and f.name == binding['source_function']]
            _require(len(functions) == 1, 'lexical static function is ambiguous')
            function = functions[0]
            # A declaration prefix is not an initializer witness. Capture the
            # entire bounded initialized declaration in its unique C owner.
            stem = re.escape(binding['source_lexical_name'])
            pattern = re.compile(r'\bstatic\s+[^;{}]*\b'+stem+r'\s*(?:\[[^;{}]*?\]\s*)*=[^;]*;', re.S)
            matches = list(native.source_matches(pattern.pattern, function.text(text)))
            _require(len(matches) == 1, 'missing/ambiguous full lexical initializer')
            match = matches[0]
            return function.start+match.start(), function.start+match.end(), edit.c_tokens(match[0])
        start, end, tokens = lexical(context['source_text'])
        pp_start, pp_end, pp_tokens = lexical(context['preprocessed'])
        origin, line = context['pp'].attribute(pp_start)
        p = Path(origin or '')
        if not p.is_absolute():
            p = Path(context['unit_dir'])/p
        _require(p.resolve() == context['source'].resolve() and tokens == pp_tokens and
                 edit.line_of(context['source_text'], start) <= line <= edit.line_of(context['source_text'], end),
                 'lexical initializer differs in source/CPP owner')
        # A raw character offset in CPP is diagnostic transport, not lexical
        # identity: official ordinary CPP may spell equal whitespace differently.
        # Full tokens, actual origin/line, owner, consumer and OBJECT stay checked.
        context.setdefault('_lexical_binding_diagnostics', {})[name] = dict(binding)
        semantic_binding = {k:v for k,v in binding.items() if k != 'source_declaration_offset'}
        return _digest(dict(tokens=tokens, binding=semantic_binding,
                            actual_object=dict(name=name, size=symbols[0].size, binding=0, type=1)))
    origin, line = context['pp'].attribute(actual.start)
    p = Path(origin or '')
    if not p.is_absolute():
        p = Path(context['unit_dir']) / p
    _require(p.resolve() == context['source'].resolve() and tokens == pp_tokens and
             edit.line_of(context['source_text'], original.start) <= line <=
             edit.line_of(context['source_text'], original.end), name + ': CPP source declaration differs')
    return _digest(tokens)


def _number(value):
    _require(type(value) in (str, int), 'invalid coordinate type')
    return int(value, 16) if isinstance(value, str) else value


def _span_signature(part, physical=False):
    names = part.get('symbols')
    raw, va = part.get('object_range'), part.get('range')
    _require(isinstance(names, list) and all(isinstance(n, str) and n for n in names)
             and len(names) == len(set(names)), 'invalid span names')
    _require(isinstance(raw, list) and len(raw) == 2 and all(type(v) is int for v in raw)
             and isinstance(va, list) and len(va) == 2, 'missing exact span coordinates')
    va = list(map(_number, va))
    _require(0 <= raw[0] < raw[1] and 0 <= va[0] < va[1]
             and raw[1] - raw[0] == va[1] - va[0], 'bad raw/original span')
    section = part.get('section', '').split('.carve.', 1)[0]
    _require(section in ('.data', '.sdata', '.rodata'), 'unsupported raw DATA section')
    tail = part.get('zero_padding_tail', 0)
    _require(type(tail) is int and 0 <= tail < raw[1]-raw[0] and tail <= 127,
             'invalid current physical alignment tail')
    alignment_only = (physical and part.get('support_only') is True and not names
                      and part.get('credit') == 'uncredited_dependency'
                      and not part.get('anonymous_emission') and not tail
                      and raw[1]-raw[0] <= 127)
    _require((not part.get('support_only') or alignment_only) and not part.get('typed_character_prefix_proof')
             and (physical or not tail) and (part.get('credit', 'verified') == 'verified' or alignment_only),
             'partial/support-only/uncredited span cannot authenticate alignment')
    anonymous = part.get('anonymous_emission', False)
    _require(type(anonymous) is bool and ((bool(names) != anonymous) or alignment_only),
             'ambiguous named/anonymous span')
    return dict(section=section, symbols=names, object_range=raw, original_range=va,
                credit=part.get('credit', 'verified'), anonymous_emission=anonymous,
                **({'physical_zero_padding_tail': tail} if tail else {}),
                **({'physical_alignment_only': True, 'support_only': True} if alignment_only else {}))


def _piece_spans(piece, section, registry, tu_id):
    _require(piece.get('c_split') is True and piece.get('c_input_spans'),
             'neighbor lacks explicit registered C input spans')
    out, cursor = [], _number(piece['start'])
    for part in piece['c_input_spans']:
        signature = _span_signature(part, physical=True)
        def core(value):
            return {k: v for k, v in value.items() if k != 'physical_zero_padding_tail'}
        matches = [(run, p) for run in registry['tus'].get(tu_id, {}).get(section, [])
                   for p in run.get('c_input_spans', [])
                   if p.get('object_range') == signature['object_range']
                   and list(map(_number, p.get('range', []))) == signature['original_range']
                   and p.get('section', '').split('.carve.', 1)[0] == signature['section']
                   and core(_span_signature(p, physical=True)) == core(signature)]
        _require(len(matches) == 1, 'span differs from exact current registry mapping')
        run, registered = matches[0]
        actual_signature = _span_signature(registered, physical=True)
        _require('zero_padding_tail' not in part or
                 part['zero_padding_tail'] == registered.get('zero_padding_tail', 0),
                 'physical tail flag differs from exact current registry')
        signature = actual_signature
        _require(not run.get('support_only')
                 and not run.get('uncredited_scaffold_ranges'), 'partial registry run stays uncredited')
        lo, hi = signature['original_range']
        _require(lo == cursor and hi <= _number(piece['end'])
                 and _number(run['range'][0]) <= lo < hi <= _number(run['range'][1]),
                 'registered spans do not tile the entire selected piece')
        _require(set(signature['symbols']) <= set(run.get('c_owned_symbols', [])),
                 'named span exceeds its registered owners')
        if run.get('typed_character_prefix_proof') is not None:
            _require(signature['symbols'] and len(signature['symbols']) == 1
                     and not signature['anonymous_emission'], 'invalid character core registration')
            signature = dict(signature, prefix_registration_sha256=_digest(run['typed_character_prefix_proof']))
        out.append(signature)
        cursor = hi
    _require(cursor == _number(piece['end']), 'selected piece has an unregistered tail')
    raw = out[0]['object_range'][0]
    for span in out:
        _require(span['section'] == out[0]['section'] and span['object_range'][0] == raw,
                 'selected input pieces are reordered or span raw sections')
        raw = span['object_range'][1]
    return out


def _alignment_fragment(layout, lo, hi):
    """A slice of one real cc1 ALIGN is current fill, never source capacity."""
    candidates = [e for e in layout['events'] if e['kind'] == 'align'
                  and not e.get('automatic_scalar_alignment')
                  and e['offset'] <= lo < hi <= e['end']]
    _require(len(candidates) == 1 and 0 < hi-lo <= 127,
             'physical tail lacks one genuine covering cc1 ALIGN')
    event = candidates[0]
    _require((event['offset']+event['alignment']-1) & -event['alignment'] == event['end']
             and event['end']-event['offset'] <= 127,
             'covering cc1 ALIGN equation differs')
    _empty_gap(layout, event['offset'], event['end'])
    return dict(range=[event['offset'], event['end']], selected_range=[lo, hi],
                alignment=event['alignment'], event_sha256=_digest(event))


def _original_alignment_fragment(tail, span, context, apis):
    base = span['original_range'][0] - span['object_range'][0]
    lo, hi = (base+offset for offset in tail['range'])
    _original_gap(context, span['section'], lo, hi, apis)
    return dict(tail, original_range=[lo, hi], credited_bytes=0,
                original_capacity_proved=False)


def _geometry(spans, layout, context, apis):
    owners = []
    for span in spans:
        a, b = span['object_range']
        _require(0 <= a < b <= layout['size'], 'span exceeds fresh cc1 section')
        if span.get('physical_alignment_only'):
            tail = _original_alignment_fragment(_alignment_fragment(layout,a,b),span,context,apis)
            owners.append(dict(kind='alignment',offset=a,size=b-a,compiler_alignment_tail=tail,
                               support_only=True,credit='uncredited_dependency',credited_bytes=0))
        elif span['anonymous_emission']:
            labels = [n for n, off in layout['labels'].items() if a <= off < b]
            payload = [e for e in layout['events'] if e['kind'] == 'payload'
                       and e['offset'] < b and a < e['end']]
            pool = (len(payload) == 1 and payload[0].get('assembler_double_pool') and
                    payload[0]['offset'] == a and payload[0]['end'] == b and not labels)
            _require((pool or labels and any(layout['labels'][n] == a for n in labels)) and payload
                     and all(re.fullmatch(r'\$L(?:C)?[0-9]+', n) for n in labels)
                     and all(a <= e['offset'] < e['end'] <= b for e in payload)
                     and not any(o['offset'] < b and a < o['offset'] + o['size']
                                 for o in layout['objects'].values()),
                     'anonymous span lacks complete cc1 label/payload geometry')
            tail = None
            if payload[-1]['end'] != b:
                end = payload[-1]['end']
                tail = _original_alignment_fragment(
                    _alignment_fragment(layout, end, b), span, context, apis)
            _require(not span.get('physical_zero_padding_tail') or tail and
                     b-payload[-1]['end'] == span['physical_zero_padding_tail'],
                     'registered physical tail is not actual cc1 alignment')
            owners.append(dict(kind='anonymous', offset=a, size=b-a, labels=labels,
                               **({'compiler_alignment_tail': tail} if tail else {}),
                               fragment_sha256=_digest([e for e in layout['events']
                                                        if a <= e['offset'] < b])))
        else:
            cursor = a
            for name in span['symbols']:
                owner = layout['objects'].get(name)
                _require(owner and owner['binding'] in (0, 1) and owner['offset'] == cursor
                         and cursor + owner['size'] <= b, 'named cc1 owners do not tile registered span')
                extra = {}
                if span.get('prefix_registration_sha256'):
                    extra['character_core'] = _character_core(span, context, apis)
                owners.append(dict(owner, kind='named', **extra,
                                   source_declaration_sha256=_definition(context, name, apis)))
                cursor += owner['size']
            if cursor < b:
                owners[-1]['compiler_alignment_tail'] = _original_alignment_fragment(
                    _alignment_fragment(layout, cursor, b), span, context, apis)
            _require(not span.get('physical_zero_padding_tail') or
                     b-cursor == span['physical_zero_padding_tail'],
                     'declared OBJECT capacity cannot become a physical alignment tail')
    return owners


def _character_core(span, context, apis):
    """Use the existing current semantic-prefix API; its JSON marker is no waiver."""
    carve = apis[0]
    root, unit_dir = Path(context['root']), Path(context['unit_dir'])
    registry = carve.load_registry(root)
    runs = [run for run in registry['tus'][context['tu']][span['section']]
            if run.get('typed_character_prefix_proof') is not None and
            _digest(run['typed_character_prefix_proof']) == span['prefix_registration_sha256']]
    _require(len(runs) == 1, 'missing/duplicate current character core receipt')
    ctx = carve.tu_context(root, 'main', unit_dir, context['tu'])
    lo, hi = span['original_range']
    registration = runs[0]['typed_character_prefix_proof']
    items = [item for item in carve.section_items(ctx, span['section'])
             if item['name'] == span['symbols'][0] and item['start'] == lo and
             [item['start'], item['end']] == registration.get('original_item_range')]
    _require(len(items) == 1, 'character core has no unique original item')
    proof_context = carve._current_character_context(root, unit_dir, context['tu'],
                                                    span['section'], runs, ctx['orig'])
    if context.get('ordinary_paths'):
        files = dict(context['ordinary_paths'], source=context['source'], original=Path(ctx['orig']))
        proof_context['inputs'] = {k: dict(path=str(Path(p).resolve()), sha256=_hash(Path(p).read_bytes()))
                                   for k, p in files.items()}
    receipt = carve.typed_character_prefix_projection(items[0], hi, proof_context)
    _require(receipt.get('range') == [lo, hi] and receipt.get('input_offset') == span['object_range'][0]
             and receipt.get('size') == hi-lo and receipt.get('grants_source_eligibility') is False
             and receipt.get('grants_recovery') is False and receipt.get('consumers'),
             'fresh character core projection differs')
    return {k: receipt[k] for k in ('name', 'range', 'original_item_range', 'input_section',
             'input_offset', 'size', 'consumers', 'payload_sha256', 'grants_source_eligibility', 'grants_recovery')}


def _piece_projection(piece):
    _require(type(piece.get('c_split', False)) is bool, 'invalid global/local C input flag')
    return dict(name=piece.get('name', piece.get('piece')),
                start=_number(piece['start']), end=_number(piece['end']),
                c_split=piece.get('c_split', False), c_section=piece.get('c_section'),
                spans=[_span_signature(p, physical=True) for p in piece.get('c_input_spans', [])],
                suffixes=piece.get('uncredited_scaffold_suffixes', []))


def _global_order(manifest, tu, section, index):
    pieces = tu['sections'][section]['pieces']
    rows = manifest['sections'][section]['order']
    own = [row for row in rows if row.get('tu') == tu['name']]
    _require([_piece_projection(p) for p in pieces] == [_piece_projection(r) for r in own],
             'TU-local and global piece order differ')
    key = (tu['name'], pieces[index]['name'])
    positions = [i for i, row in enumerate(rows) if (row.get('tu'), row.get('piece')) == key]
    _require(len(positions) == 1, 'missing/duplicate global piece')
    position = positions[0]
    _require(position > 0 and position + 1 < len(rows), 'missing immediate global neighbor')
    _require(rows[position-1]['tu'] == tu['name'] and
             _piece_projection(rows[position-1]) == _piece_projection(pieces[index-1]),
             'left neighbor differs in global order')
    following = rows[position+1]
    targets = [t for t in manifest['tus'] if t['name'] == following['tu']]
    _require(len(targets) == 1, 'missing/ambiguous next TU')
    target = targets[0]
    matches = [(i, p) for i, p in enumerate(target['sections'][section]['pieces'])
               if p['name'] == following['piece']]
    _require(len(matches) == 1 and _piece_projection(matches[0][1]) == _piece_projection(following),
             'next global input differs from local TU piece')
    return target, matches[0][0], _digest([dict(tu=r['tu'], **_piece_projection(r))
                                          for r in rows[position-1:position+2]])


def _marker_bijection(manifest):
    local, global_ = {}, {}
    for tu in manifest['tus']:
        for section, entry in tu.get('sections', {}).items():
            for p in entry.get('pieces', []):
                proof = p.get('linker_padding', {})
                if proof.get('kind') in COMPLETION_FILL_KINDS:
                    key = (tu['name'], section, p['name'])
                    _require(key not in local and proof.get('tu') == tu['id']
                             and proof.get('section') == section and proof.get('piece') == p['name']
                             and list(map(_number, proof.get('range', []))) ==
                             [_number(p['start']), _number(p['end'])], 'copied/duplicate local proof marker')
                    local[key] = (proof, _piece_projection(p))
    for section, entry in manifest['sections'].items():
        for p in entry.get('order', []):
            proof = p.get('linker_padding', {})
            if proof.get('kind') in COMPLETION_FILL_KINDS:
                key = (p['tu'], section, p['piece'])
                _require(key not in global_, 'duplicate global proof marker')
                global_[key] = (proof, _piece_projection(p))
    _require(local == global_, 'TU-local/global compiler proof markers are not one-to-one')
    return local


def _context_receipt(context):
    return {k: context[k] for k in ('tu', 'require_complete', 'inputs', 'headers',
            'contract_sha256', 'preprocessed_tokens_sha256', 'generated_preprocessed_sha256',
            'generated_assembly_sha256')} | dict(
                source=str(context['source']),
                source_sha256=_hash(context['source_text'].encode('utf-8', 'surrogateescape')),
                compiler_diagnostics=context.get('compiler_diagnostics', []),
                function_scope=context['function_scope'])


def _selected_object_route(input_):
    # The first physical slice legitimately keeps the raw section's name.
    # Its ordinary carved-object route, complete extent and full split replay
    # distinguish that slice from an unsplit raw object.
    return re.fullmatch(r'build/c/[A-Za-z_]\w*\.carved\.o', input_.get('object','')) is not None


def _input(tu, piece, spans, layout, boundary='first'):
    _require(boundary in ('first', 'last'), 'unknown selected input boundary')
    index = 0 if boundary == 'first' else len(spans)-1
    span = spans[index]
    parts = piece['c_input_spans']
    _require(len(parts) == len(spans), 'selected input group differs from registered spans')
    a, b = span['object_range']
    raw_section = span['section']
    selected = (piece.get('c_section') or parts[0].get('section') or raw_section) if len(spans) == 1 \
               else parts[index].get('section')
    _require(selected and (len(spans) == 1 or
             len({part.get('section') for part in parts}) == len(parts)),
             'grouped pieces do not identify distinct actual selected inputs')
    _require(selected == raw_section or re.fullmatch(re.escape(raw_section)+r'\.carve\.[1-9][0-9]*', selected),
             'unsupported selected input section route')
    alignment = layout['alignment']
    def lowbit(v):
        return v & -v if v else 1 << 32
    object_path = tu.get('c_link_object') or 'build/c/' + tu['name'] + '.o'
    alignment = min(alignment, lowbit(span['original_range'][0]),
                    lowbit(a) if selected != raw_section or _selected_object_route(dict(object=object_path)) else alignment)
    return dict(tu=tu['id'], piece=piece['name'], object=object_path, section=selected, raw_section=raw_section,
                object_range=[a, b], original_range=span['original_range'],
                alignment=alignment)


def _structure(tu, section, index):
    pieces = tu['sections'][section]['pieces']
    def part(p):
        return {k: p[k] for k in ('name', 'start', 'end', 'file', 'c_split', 'c_section',
                                  'c_input_spans', 'uncredited_scaffold_suffixes') if k in p}
    return _digest(dict(tu=tu['id'], name=tu['name'], source=tu['path'], section=section,
                        boundary=[tu['sections'][section]['start'], tu['sections'][section]['end']],
                        pieces=[part(p) for p in pieces[index - 1:index + 2]]))


def _original_index_sites(original, lo, hi, native):
    """Retain multiplicity for exactly the existing conservative index.

    The supported index implementation is pinned as well as its returned set:
    equality of sets alone cannot detect a second producer of the same value.
    These witnesses describe that index, not exhaustive CFG/alias analysis.
    """
    _require(_hash(inspect.getsource(native.original_reference_addresses.__wrapped__).encode()) ==
             'bef0f448ed692e5de9d232261f4ce21ff51f6d7b4f439277e1226496e9703703',
             'unsupported original-reference index implementation')
    addresses, sites = set(), []
    gp = original.gp
    def add(value, kind, section, offset, **facts):
        addresses.add(value)
        if lo <= value < hi:
            sites.append(dict(value=value, kind=kind, section=section.name,
                              offset=offset, **facts))
    symbols = {}
    for symbol in original.symbols:
        symbols.setdefault(symbol.index, symbol)
    for section in original.sections:
        if section.type == 9:
            for offset in range(section.offset, section.offset+section.size, section.entsize):
                address, info = struct.unpack_from('<II', original.data, offset)
                add(address, 'relocation_place', section, offset)
                symbol = symbols.get(info >> 8)
                if symbol:
                    add(symbol.value, 'relocation_symbol', section, offset, symbol=symbol.index)
        if not section.flags & 2 or section.type == 8:
            continue
        data = original.section_bytes(section)
        if not section.flags & 4:
            for offset in range(0, len(data)-3, 4):
                add(struct.unpack_from('<I', data, offset)[0], 'stored_word', section, offset)
            continue
        upper = {}
        for offset in range(0, len(data)-3, 4):
            word = struct.unpack_from('<I', data, offset)[0]
            opcode, rs, rt = word >> 26, word >> 21 & 31, word >> 16 & 31
            immediate = word & 0xffff
            signed = immediate-(0x10000 if immediate & 0x8000 else 0)
            if opcode == 15:
                upper[rt] = (immediate << 16, offset)
            elif opcode in (8, 9, 13) or 0x20 <= opcode <= 0x3f:
                if rs in upper and offset-upper[rs][1] <= 128:
                    value = upper[rs][0] | immediate if opcode == 13 else upper[rs][0]+signed
                    add(value, 'upper_low', section, offset, high_offset=upper[rs][1], word=word)
                if rs == 28 and gp:
                    add(gp+signed, 'GP', section, offset, word=word)
    _require(tuple(sorted(addresses)) == native.original_reference_addresses(original.data),
             'original reference witness enumeration differs')
    return sites


def _folded_array_reference(context, section, lo, hi, layout, registry, apis):
    """One direct array[index-1] whose emitted static base is array-4.

    A positive original array and its pointees remain the only storage owners.
    The C expression has its ordinary implicit valid-index domain; this does
    not assert that every runtime call obeys it or construct array-1 in C.
    """
    carve, gate, native, audit, edit, Elf, Toolchain = apis
    original, compiled = context['original'], _compiler_object(context, apis)
    _require(hi-lo == 4 and section == '.data', 'folded array base is not one pointer-sized ALIGN')
    original_section, raw_section = original.section(section), compiled.section(section)
    tables = [s for s in original.symbols if s.shndx == original_section.index and
              s.value == hi and s.bind == 0 and s.type == 1 and 0 < s.size <= 256 and s.size % 4 == 0]
    _require(len(tables) == 1, 'folded base lacks one positive LOCAL pointer array')
    table = tables[0]
    raw_tables = [s for s in compiled.symbols if s.name == table.name and s.shndx == raw_section.index
                  and (s.size, s.type, s.bind) == (table.size, 1, 0)]
    _require(len(raw_tables) == 1, 'folded array current LOCAL OBJECT differs')
    raw_table = raw_tables[0]
    def registered(symbol, raw_symbol):
        matches = [p for run in registry['tus'][context['tu']].get(section, [])
                   for p in run.get('c_input_spans', []) if p.get('symbols') == [symbol.name]
                   and list(map(_number, p.get('range', []))) == [symbol.value, symbol.value+symbol.size]
                   and p.get('object_range') == [raw_symbol.value, raw_symbol.value+raw_symbol.size]]
        _require(len(matches) == 1, 'folded array/pointee lacks exact verified registration')
        _span_signature(matches[0])
    registered(table, raw_table)
    declaration = _definition(context, table.name, apis)
    def array_initializer(text):
        pattern = r'\bstatic\s+u8\s*\*\s*'+re.escape(table.name)+r'\s*\[\s*(\d+)\s*\]\s*=\s*\{([^{};]+)\}\s*;'
        found = [m for item in edit.TU(text).free_items() if item.kind == 'decl' and not item.conditional
                 for m in native.source_matches(pattern, item.text(text))]
        _require(len(found) == 1 and int(found[0][1]) * 4 == table.size,
                 'folded array lacks exact source pointer-array initializer')
        names = [n.strip() for n in found[0][2].split(',') if n.strip()]
        _require(len(names) == table.size//4 and len(set(names)) == len(names) and
                 all(re.fullmatch(r'[A-Za-z_]\w*', n) for n in names),
                 'folded array initializer is not distinct named pointees')
        return names
    pointee_names = array_initializer(context['source_text'])
    _require(array_initializer(context['preprocessed']) == pointee_names,
             'folded array source/CPP initializer differs')
    pointees, expected_relocations = [], []
    for i, name in enumerate(pointee_names):
        old = [s for s in original.symbols if s.name == name and s.shndx == original_section.index
               and s.type == 1 and s.bind == 0 and s.size == 4]
        new = [s for s in compiled.symbols if s.name == name and s.shndx == raw_section.index
               and s.type == 1 and s.bind == 0 and s.size == 4]
        _require(len(old) == len(new) == 1, 'folded array pointee identity differs')
        old, new = old[0], new[0]
        registered(old, new)
        _require(carve.va_bytes(original, old.value, 4) == compiled.section_bytes(raw_section)[new.value:new.value+4]
                 and struct.unpack_from('<I', carve.va_bytes(original, hi+i*4, 4))[0] == old.value,
                 'folded array pointee/pointer bytes differ')
        pointees.append(dict(name=name, original=old.value, raw=new.value,
                             declaration_sha256=_definition(context, name, apis)))
        expected_relocations.append((raw_table.value+i*4, new.value))
    actual_relocations = []
    for rel in compiled.sections:
        if rel.type not in (4, 9) or rel.info != raw_section.index:
            continue
        for offset, kind, index, addend in carve._rel_entries(compiled, rel):
            if raw_table.value <= offset < raw_table.value+raw_table.size:
                target = compiled.symbols[index]
                _require(rel.type == 9 and kind == 2 and addend is None and target.shndx == raw_section.index,
                         'folded pointer array relocation is unsupported')
                actual_relocations.append((offset, target.value+struct.unpack_from('<i', compiled.section_bytes(raw_section), offset)[0]))
    _require(actual_relocations == expected_relocations, 'folded pointer array R32 meanings differ')
    pattern = (r'\b'+re.escape(table.name)+r'\s*\[\s*\(\(\s*([A-Za-z_]\w*)\s*\*\s*\)\s*'
               r'([A-Za-z_]\w*)\s*\)\s*->\s*([A-Za-z_]\w*)\s*-\s*1\s*\]')
    def expression(text):
        found = [(f, m) for f in edit.scan(text) if f.kind == 'func'
                 for m in native.source_matches(pattern, f.text(text))]
        _require(len(found) == 1, 'folded base lacks one direct source array[index-1] expression')
        f, m = found[0]
        return f, m, edit.c_tokens(m[0])
    source_function, use, tokens = expression(context['source_text'])
    pp_function, pp_use, pp_tokens = expression(context['preprocessed'])
    origin, line = context['pp'].attribute(pp_function.start+pp_use.start())
    origin_path = Path(origin or '')
    if not origin_path.is_absolute(): origin_path = Path(context['unit_dir'])/origin_path
    _require(tokens == pp_tokens and source_function.name == pp_function.name and
             origin_path.resolve() == context['source'].resolve() and
             edit.line_of(context['source_text'], source_function.start+use.start()) <= line <=
             edit.line_of(context['source_text'], source_function.start+use.end()),
             'folded source expression/CPP owner differs')
    record_pattern = (r'\btypedef\s+struct\s*\{\s*u8\s+[A-Za-z_]\w*\s*\[\s*(0[xX][0-9a-fA-F]+|[0-9]+)\s*\]\s*;\s*'
                      r'u16\s+'+re.escape(use[3])+r'\s*;\s*\}\s*'+re.escape(use[1])+r'\s*;')
    records = [[m for item in edit.TU(text).free_items() if not item.conditional
                for m in native.source_matches(record_pattern, item.text(text))]
               for text in (context['source_text'], context['preprocessed'])]
    _require(all(len(r) == 1 for r in records) and
             edit.c_tokens(records[0][0][0]) == edit.c_tokens(records[1][0][0]),
             'folded selector lacks exact source/CPP unsigned-halfword field layout')
    field_offset = int(records[0][0][1], 0)
    _require(field_offset % 2 == 0 and field_offset <= 0x100000,
             'folded selector field offset is unsupported')
    functions = [s for s in compiled.symbols if s.name == source_function.name and s.type == 2 and s.bind == 0]
    scopes = [f for f in context['function_scope'] if f['name'] == source_function.name]
    _require(len(functions) == len(scopes) == 1, 'folded reference lacks one emitted original function')
    function, scope = functions[0], scopes[0]
    old_functions = [s for s in original.symbols if s.name == function.name and
                     (s.value, s.size, s.type, s.bind) == (scope['va'], function.size, 2, 0)]
    _require(len(old_functions) == 1 and scope['size'] == function.size,
             'folded original function identity differs')
    text = compiled.sections[function.shndx]
    raw_bytes = compiled.section_bytes(text)
    rels = [(o, k, n, a) for r in compiled.sections if r.type in (4, 9) and r.info == text.index
            for o, k, n, a in carve._rel_entries(compiled, r)
            if function.value <= o < function.value+function.size]
    pairs = []
    for high, kind, target_index, addend in rels:
        if kind != 5 or addend is not None: continue
        target = compiled.symbols[target_index]
        if target.shndx != raw_section.index: continue
        for low, lk, li, la in rels:
            if lk != 6 or li != target_index or la is not None or low != high+8: continue
            words = list(struct.unpack_from('<4I', raw_bytes, high-4))
            shift, upper, add, load = words
            idx, base = shift >> 11 & 31, upper >> 16 & 31
            _require(idx != 0 and base != 0, 'folded constructor uses zero register')
            if (shift != ((idx << 16)|(idx << 11)|(2 << 6)) or upper & 0xffe00000 != 0x3c000000 or
                add != ((base << 21)|(idx << 16)|(base << 11)|0x21) or
                load >> 26 != 35 or load >> 21 & 31 != base or load >> 16 & 31 != base): continue
            value = target.value+((upper & 0xffff) << 16)+carve._sext16(load)
            if value != raw_table.value-4: continue
            pairs.append((high, low, target_index, words, idx, base))
    _require(len(pairs) == 1, 'folded reference lacks one exact ordinary scaled HI/LO constructor')
    high, low, target_index, words, idx, base = pairs[0]
    # Bind the unsigned selector load and its actual global+field relocation.
    selector_pairs = []
    for sl, lk, si, la in rels:
        target = compiled.symbols[si]
        if lk != 6 or la is not None or target.name != use[2] or not function.value <= sl < high-4: continue
        lw = struct.unpack_from('<I', raw_bytes, sl)[0]
        if lw >> 26 != 37 or lw >> 16 & 31 != idx: continue
        for sh, hk, hi_index, ha in rels:
            if hk != 5 or hi_index != si or ha is not None or not function.value <= sh < sl: continue
            hw = struct.unpack_from('<I', raw_bytes, sh)[0]
            if hw & 0xffe00000 != 0x3c000000 or hw >> 16 & 31 != lw >> 21 & 31: continue
            if ((hw & 0xffff) << 16)+carve._sext16(lw) != field_offset: continue
            if not 0 < sl-sh <= 128: continue
            before = list(struct.unpack_from('<'+'I'*((sl-sh)//4-1), raw_bytes, sh+4))
            selector_base = hw >> 16 & 31
            if any(w and w >> 26 not in (49, 53) and
                   not (w >> 26 in (8,9,12,13,14) and w >> 16 & 31 not in (selector_base,idx))
                   for w in before): continue
            between = list(struct.unpack_from('<'+'I'*((high-4-sl)//4-1), raw_bytes, sl+4))
            # This closed constructor has only NOP, an unrelated register move
            # and COP1 arithmetic before its scale; no reaching-value guess.
            if any(w and not (w >> 26 == 17 and w >> 21 & 31 >= 16) and
                   not (w >> 26 == 0 and w & 63 in (0x21, 0x25, 0x2d) and
                        w >> 11 & 31 not in (idx, base)) for w in between): continue
            selector_pairs.append((sh, sl, hw, lw, before, between, target.name))
    _require(len(selector_pairs) == 1, 'folded index lacks its exact unsigned selector-field constructor')
    sh, sl, hw, lw, before, between, global_name = selector_pairs[0]
    original_data = carve.va_bytes(original, scope['va'], function.size)
    native_words = list(struct.unpack_from('<4I', original_data, high-function.value-4))
    _require(all((x & 0xffff0000) == (y & 0xffff0000) if i in (1, 3) else x == y
                 for i, (x, y) in enumerate(zip(words, native_words))) and
             ((native_words[1] & 0xffff) << 16)+carve._sext16(native_words[3]) == lo,
             'original folded constructor/register/stride differs')
    old_selector = [s for s in original.symbols if s.name == global_name and s.shndx != 0]
    _require(len(old_selector) == 1, 'folded selector global original identity is ambiguous')
    old_hi, old_lo = (struct.unpack_from('<I', original_data, off-function.value)[0] for off in (sh, sl))
    _require(old_hi & 0xffff0000 == hw & 0xffff0000 and old_lo & 0xffff0000 == lw & 0xffff0000
             and ((old_hi & 0xffff) << 16)+carve._sext16(old_lo) == old_selector[0].value+field_offset
             and [w & (0xffff0000 if w >> 26 in (49,53) else 0xffffffff) for w in
                  struct.unpack_from('<'+'I'*len(before), original_data, sh-function.value+4)] ==
                 [w & (0xffff0000 if w >> 26 in (49,53) else 0xffffffff) for w in before]
             and original_data[sl-function.value+4:high-function.value-4] == raw_bytes[sl+4:high-4],
             'original selector-field constructor differs')
    sites = _original_index_sites(original, lo, hi, native)
    expected = dict(value=lo, kind='upper_low', section=original.sections[old_functions[0].shndx].name,
                    offset=scope['va']+low-function.value-original.sections[old_functions[0].shndx].addr,
                    high_offset=scope['va']+high-function.value-original.sections[old_functions[0].shndx].addr,
                    word=native_words[3])
    _require(sites == [expected], 'additional/unclassified original alignment reference producer')
    bodies = list(re.finditer(r'^\s*\.ent\s+'+re.escape(function.name)+r'\s*$(.*?)^\s*\.end\s+'
                             +re.escape(function.name)+r'\s*$', context['assembly'], re.M|re.S))
    _require(len(bodies) == 1 and '#APP' not in bodies[0][1] and
             len(re.findall(r'^\s*lw\s+\$'+str(base)+r'\s*,\s*'+re.escape(table.name)
                      +r'-4\(\$'+str(idx)+r'\)\s*$', bodies[0][1], re.M)) == 1,
             'folded cc1 source operand differs')
    return dict(schema='direct-array-index-minus-one-base/1', table=table.name, table_size=table.size,
                original_range=[lo, hi], raw_range=[raw_table.value-4, raw_table.value],
                declaration_sha256=declaration, pointees=pointees, source_expression_sha256=_digest(tokens),
                function=function.name, function_size=function.size, function_binding=0,
                function_original=scope['va'], function_raw=function.value, source_section=text.name,
                high_offset=high-function.value, low_offset=low-function.value,
                words=words, table_raw_offset=raw_table.value,
                raw_target=dict(name=compiled.symbols[target_index].name,
                    value=compiled.symbols[target_index].value, type=compiled.symbols[target_index].type,
                    binding=compiled.symbols[target_index].bind, section=raw_section.name),
                selector=dict(global_name=global_name, field_offset=field_offset,
                    high_offset=sh-function.value, low_offset=sl-function.value,
                    words=[hw,lw], before=[w & (0xffff0000 if w >> 26 in (49,53) else 0xffffffff)
                                            for w in before], between=between), original_index_sites=sites,
                implicit_valid_selector_domain=[1, table.size//4], global_runtime_bound_proved=False,
                reference_class='compiler-folded static array base; not separate storage', credited_bytes=0)


def _original_gap(context, section, lo, hi, apis, folded=()):
    carve, gate, native, audit, edit, Elf, Toolchain = apis
    original = context['original']
    osec = original.section(section)
    _require(0 < hi-lo <= 127 and osec.type == 1 and
             osec.addr <= lo < hi <= osec.addr+osec.size and
             carve.va_bytes(original, lo, hi-lo) == bytes(hi-lo), 'nonzero/unbounded original gap')
    _require(not any(s.shndx == osec.index and s.type not in (3, 4) and
                     (lo <= s.value < hi or s.size and s.value < hi and lo < s.value+s.size)
                     for s in original.symbols), 'gap overlaps original identity')
    if native.original_references(original, lo, hi):
        _require(len(folded) == 1 and folded[0]['original_range'] == [lo, hi] and
                 _original_index_sites(original, lo, hi, native) == folded[0]['original_index_sites'],
                 'original reference index hits gap')


def _empty_gap(layout, a, b):
    _require(not any(e['kind'] == 'payload' and e['offset'] < b and a < e['end']
                     for e in layout['events']), 'gap contains explicit compiler payload')
    _require(not any(a <= off < b for off in layout['labels'].values()), 'gap contains compiler label')


def _governing_alignment(layout, offset):
    events = layout['events']
    labels = [i for i, e in enumerate(events) if e['kind'] == 'label' and e['offset'] == offset]
    _require(labels, 'right payload has no exact cc1 boundary label')
    first = min(labels)
    preceding = max((i for i, e in enumerate(events[:first]) if e['kind'] == 'payload'), default=-1)
    aligns = [e for e in events[preceding+1:first] if e['kind'] == 'align' and e['end'] == offset]
    _require(aligns, 'right payload has no governing initial cc1 ALIGN')
    alignment = max(e['alignment'] for e in aligns)
    _require(alignment > 1 and not any(e['kind'] == 'payload' for e in events[preceding+1:first]),
             'right alignment is not a cc1 boundary requirement')
    return alignment, aligns


def _required_input_alignment(layout, spans, selected):
    """A whole input keeps every constructor; a crop keeps only its own requirement."""
    start, end = selected['object_range']
    _require(selected['object_range'] == spans[0]['object_range'],
             'right input is not the complete first registered selected span')
    if start != 0 or end != layout['size'] or selected['section'] != layout['section']:
        return _governing_alignment(layout, start)
    events, witnesses = layout['events'], []
    for i, event in enumerate(events):
        if event['kind'] == 'payload' and event.get('assembler_double_pool'):
            witnesses.append(dict(event,alignment=event['input_alignment']))
        elif event['kind'] == 'payload' and event.get('natural_alignment', 1) > 1:
            witnesses.append(dict(event, alignment=event['natural_alignment']))
        elif event['kind'] == 'align':
            following = next((e for e in events[i+1:] if e['kind'] == 'payload'), None)
            if following and following['offset'] == event['end']:
                witnesses.append(event)
    required = max((e['alignment'] for e in witnesses), default=1)
    _require(required > 1 and required == layout['alignment'],
             'whole input alignment lacks a contained compiler constructor requirement')
    return required, [e for e in witnesses if e['alignment'] == required]


def _retained_input_alignment(layout, selected):
    """Prove the current splitter's actual input requirement, not its C type's."""
    whole = dict(object_range=[0, layout['size']], section=layout['section'])
    raw_required, witnesses = _required_input_alignment(layout, [whole], whole)
    def lowbit(value):
        return value & -value if value else 1 << 32
    expected = min(raw_required, lowbit(selected['original_range'][0]),
                   lowbit(selected['object_range'][0])
                   if selected['section'] != selected['raw_section'] else raw_required)
    _require(expected > 1 and selected['alignment'] == expected,
             'selected alignment is not exact ordinary retained raw alignment')
    return expected, dict(mode='retained_raw_section_alignment',
                         raw_requirement=raw_required, raw_size=layout['size'],
                         selected_alignment=expected, witnesses=witnesses,
                         splitter_rule='min(raw.align,lowbit(original_start),lowbit(raw_start))',
                         original_alignment_proved=False, credited_bytes=0)


def _legacy_literal_bytes(layout, native):
    """Closed whole-input route for genuine anonymous cc1 string literals."""
    _require(layout['section'] == '.rodata' and not layout['objects'],
             'legacy whole literal input has typed/non-RODATA storage')
    payload, count = bytearray(layout['size']), 0
    for event in layout['events']:
        if event['kind'] != 'payload':
            continue
        match = re.fullmatch(r'\.(ascii|asciz)\s+("(?:[^"\\]|\\.)*")', event['text'])
        _require(match is not None, 'legacy whole input is not literal-only compiler storage')
        data = native.c_literal_bytes(match[2]) + (b'\0' if match[1] == 'asciz' else b'')
        _require(len(data) == event['end']-event['offset'], 'legacy literal constructor size differs')
        payload[event['offset']:event['end']] = data
        count += 1
    _require(count > 0, 'legacy whole input has no literal payload')
    return bytes(payload), count


def _legacy_literal_input(tu, piece, section, layout, context, registry, apis):
    """Describe an existing unsplit C input, never a fabricated registry span."""
    carve, gate, native, audit, edit, Elf, Toolchain = apis
    imported = tu.get('import') or {}
    source = imported.get('source') or {}
    _require(section == '.rodata' and tu.get('mode') == 'c' and tu.get('in_scope')
             and imported.get('mode') == 'c' and imported.get('owners', {}).get(section) == 'c'
             and tu.get('owners', {}).get(section) == 'c'
             and source.get('kind') == 'file' and source.get('path') == tu['path']
             and context['require_complete'] is True and not tu.get('c_link_object')
             and piece.get('c_split') is False
             and not any(piece.get(k) for k in ('c_input_spans', 'c_section', 'support_only',
                         'uncredited_scaffold_suffixes', 'padding_tail', 'typed_character_prefix_proof'))
             and not registry['tus'].get(tu['id'], {}).get(section),
             'legacy successor is not an existing whole ordinary C literal input')
    pieces = tu['sections'][section]['pieces']
    _require(len(pieces) == 1 and _piece_projection(pieces[0]) == _piece_projection(piece),
             'legacy whole input has multiple/partial original pieces')
    lo, physical_hi = map(_number, (piece['start'], piece['end']))
    authority_path = Path(context['root'])/'config/objects/main.objects.json'
    authority_bytes = authority_path.read_bytes()
    declarations = [row for row in json.loads(authority_bytes)['tus'] if row['id'] == tu['id']]
    _require(len(declarations) == 1, 'legacy input has no unique tracked owner declaration')
    declared = declarations[0]
    owner_section = declared.get('sections', {}).get(section, {})
    _require(declared.get('import') == imported and declared.get('path') == tu['path']
             and declared.get('owners', {}).get(section) == 'c'
             and list(map(_number, (owner_section.get('start'), owner_section.get('end')))) ==
                 [lo, physical_hi]
             and len(owner_section.get('pieces', [])) == 1
             and _piece_projection(owner_section['pieces'][0]) == _piece_projection(piece),
             'legacy input differs from tracked whole-section C ownership')
    authority_pin = dict(path=str(authority_path.resolve()), sha256=_hash(authority_bytes))
    end = lo + layout['size']
    _require(0 < layout['size'] <= physical_hi-lo, 'legacy current input exceeds original physical piece')
    geometry = dict(section=section, object_range=[0, layout['size']],
                    original_range=[lo, end], symbols=[], anonymous_emission=True)
    owners = _geometry([geometry], layout, context, apis)
    payload, count = _legacy_literal_bytes(layout, native)
    _require(carve.va_bytes(context['original'], lo, len(payload)) == payload,
             'legacy complete literal bytes differ from original input')
    input_ = dict(tu=tu['id'], piece=piece['name'], object='build/c/'+tu['name']+'.o',
                  section=section, raw_section=section, object_range=[0, len(payload)],
                  original_range=[lo, end], alignment=layout['alignment'])
    receipt = dict(schema='legacy-whole-c-literal-input/1', physical_range=[lo, physical_hi],
                   current_range=[lo, end], literal_count=count, payload_sha256=_hash(payload),
                   import_sha256=_digest(imported), current_compiler_input_only=True,
                   tracked_owner_sha256=_digest(declared), ownership_input=authority_pin,
                   credited_bytes=0, original_capacity_proved=False, grants_source_eligibility=False)
    _check_pins([authority_pin])
    return [geometry], owners, input_, receipt


def _external_input(tu, piece, section, context, manifest, apis):
    """Reproduce a reviewed, unchanged original-byte assembler input.

    The public record is project reconstruction authority, never evidence of
    original compiler alignment or provider C ownership. No private baseline
    file is read. This closed route supports complete relocation-free RODATA.
    """
    carve, gate, native, audit, edit, Elf, Toolchain = apis
    root, unit_dir = Path(context['root']), Path(context['unit_dir'])
    authority = root/EXTERNAL_AUTHORITY
    if not authority.is_file():
        return None
    authority_bytes = authority.read_bytes()
    records = json.loads(authority_bytes)
    _require(records.get('schema') == 'unchanged-data-input-provenance/1',
             'unknown external input provenance schema')
    matches = [r for r in records.get('inputs', []) if
               (r.get('tu'), r.get('section'), r.get('piece')) ==
               (tu['id'], section, piece['name'])]
    if not matches:
        return None
    _require(len(matches) == 1, 'duplicate external input authority')
    record = matches[0]
    lo, hi = map(_number, (piece['start'], piece['end']))
    _require(section == '.rodata' and piece.get('c_split') is False and
             not any(piece.get(k) for k in ('c_input_spans', 'c_section', 'support_only',
                          'uncredited_scaffold_suffixes', 'typed_character_prefix_proof')) and
             record.get('schema') == 'unchanged-external-input/1' and
             record.get('range') == [lo, hi] and 0 < hi-lo <= 1024*1024 and
             type(record.get('credited_bytes')) is int and record['credited_bytes'] == 0 and
             record.get('original_alignment_proved') is False and
             record.get('original_capacity_proved') is False and
             record.get('grants_source_eligibility') is False,
             'external successor is not a complete uncredited native input')
    owned = json.loads((root/'config/objects/main.objects.json').read_bytes())
    declarations = [t for t in owned['tus'] if t['id'] == tu['id']]
    _require(len(declarations) == 1 and _digest(declarations[0]) == record['owner_sha256'] and
             all(tu.get(k) == declarations[0].get(k) for k in
                 ('id', 'name', 'path', 'mode', 'in_scope', 'import', 'functions')) and
             tu.get('owners', {}).get(section) == declarations[0].get('owners', {}).get(section) and
             _digest({k:tu.get(k) for k in ('id', 'name', 'path', 'mode', 'in_scope',
                      'import', 'owners', 'functions', 'c_link_object')}) == record['current_owner_sha256'] and
             _digest([_piece_projection(p) for p in tu['sections'][section]['pieces']]) ==
                 record['local_order_sha256'] and
             [_piece_projection(p) for p in tu['sections'][section]['pieces']] ==
             [_piece_projection(p) for p in manifest['sections'][section]['order']
              if p.get('tu') == tu['name']], 'external original owner/input order differs')
    source = unit_dir/piece['file']
    _require(source.resolve().is_relative_to(unit_dir.resolve()) and
             piece['file'] == record['source'] and _hash(source.read_bytes()) == record['source_sha256'],
             'external generated source changed')
    original_manifest = json.loads((unit_dir/'tu-manifest.json').read_bytes())
    origins = [t for t in original_manifest['tus'] if t['id'] == tu['id']]
    _require(len(origins) == 1, 'external input lacks original generated owner')
    origin_pieces = origins[0]['sections'][section]['pieces']
    origins = [p for p in origin_pieces if _number(p['start']) <= lo < hi <= _number(p['end'])]
    _require(len(origins) == 1, 'external input is not inside one original generated piece')
    origin = origins[0]
    origin_path = carve.main_piece_file(unit_dir, origin, section)
    _require(origin_path.resolve().is_relative_to(unit_dir.resolve()) and
             _hash(origin_path.read_bytes()) == record['origin_source_sha256'],
             'external original generated item source changed')
    header, items = carve.piece_items(origin_path, _number(origin['start']), _number(origin['end']))
    items = [it for it in items if lo <= it['start'] < hi]
    _require(items and items[0]['start'] == lo and items[-1]['end'] == hi and
             all(a['end'] == b['start'] for a, b in zip(items, items[1:])),
             'external input truncates an original generated item')
    lines = header + [line for item in items for line in item['block']['lines']]
    while lines and not lines[-1].strip():
        lines.pop()
    text = '\n'.join(lines)+'\n'
    _require(source.read_bytes() == text.encode() and _digest(items) == record['items_sha256'],
             'external input is not exact ordinary item regeneration')
    # Restrict the replay to the ordinary generated DATA grammar. Includes,
    # arbitrary section switches, reserve directives and authored ASM refuse.
    clean = re.sub(r'/\*.*?\*/', '', text, flags=re.S)
    for line in clean.splitlines():
        line = line.strip()
        if not line:
            continue
        _require(bool(re.fullmatch(r'(?:\.include "macro\.inc"|\.section \.rodata, "a"|'
                    r'(?:nonmatching|dlabel|enddlabel) [A-Za-z_$][\w.$]*|'
                    r'\.size ([A-Za-z_$][\w.$]*), \. - \1|\.align [0-7]|'
                    r'\.(?:word|byte) (?:0x[0-9a-fA-F]+|[0-9]+)(?:,\s*(?:0x[0-9a-fA-F]+|[0-9]+))*|'
                    r'\.(?:ascii|asciz) "(?:[^"\\]|\\.)*")', line)),
                 'unsupported external generated DATA directive')
    macro = unit_dir/'include/macro.inc'
    _require(_hash(macro.read_bytes()) == record['macro_sha256'], 'external macro changed')
    # The original/config/generated files and relevant complete generators are
    # current inputs. Baseline provenance is independently reviewed metadata.
    required = {'root:config/objects/main.objects.json', 'root:config/originals.json',
                'root:config/toolchain-identity.json', 'root:config/objects/main.compile.json',
                'root:config/objects/data-carves.json', 'root:config/split/main.yaml',
                'root:config/split/main.data-splits.json', 'root:config/symbols/main.txt',
                'root:requirements.txt', 'root:configure.py', 'root:tools/configure.py',
                'root:tools/tu/tracked_inputs.py', 'root:tools/tu/run_splat.py',
                'root:tools/tu/post_split.py', 'root:tools/tu/legacy_dialect.py',
                'root:tools/tu/data_carve.py', 'root:tools/tu/ninja_main.py',
                'root:tools/tu/tail_align.py', 'root:tools/tu/toolchain.py', 'root:tools/tu/elfinfo.py',
                'root:include/macro.inc', 'unit:tu-manifest.json', 'unit:splat.yaml',
                'unit:symbol_addrs.txt', 'unit:auto-splits.json'}
    dependencies = record['generator_inputs']
    _require(set(dependencies) >= required and len(dependencies) <= 64,
             'incomplete external generator dependency closure')
    # Staging resolves public tool identities to this host's verified paths.
    # Authenticate that exact official transformation; only the existing
    # non-build provenance keys may differ. Raw current file bytes are still
    # pinned below and must remain identical through ordinary validation.
    from tu import tracked_inputs
    tc = Toolchain(root)
    expected_manifest = tracked_inputs.to_paths(owned, tc)
    staged_manifest, portable_manifest = dict(original_manifest), dict(owned)
    for key in tracked_inputs.UNTRACKED_MANIFEST_KEYS:
        staged_manifest.pop(key, None)
        expected_manifest.pop(key, None)
        portable_manifest.pop(key, None)
    _require(staged_manifest == expected_manifest and
             record.get('manifest_identity') == 'official-staged-tool-paths/1',
             'external generated manifest differs from exact public tool staging')
    paths = [authority, source, origin_path, macro]
    for name, digest in dependencies.items():
        kind, relative = name.split(':', 1)
        base = root if kind == 'root' else unit_dir
        path = base/relative
        identity = (_digest(portable_manifest) if name == 'unit:tu-manifest.json'
                    else _hash(path.read_bytes()))
        _require(kind in ('root', 'unit') and path.resolve().is_relative_to(base.resolve()) and
                 identity == digest, 'external generator/config input changed')
        paths.append(path)
    assembler = tc.dir('ps2dev-binutils')/'mips64r5900el-ps2-elf-as'
    flags = ['-EL', '-march=r5900', '-mabi=eabi', '-mgp64', '-G0',
             '-no-pad-sections', '-mno-fix-r5900']
    _require(not tc.relaxed and record['assembler_sha256'] == _hash(assembler.read_bytes()) and
             record['assembler_argv'] == flags, 'external strict assembler recipe differs')
    pins = _pins(paths+[assembler])
    _require(_hash(context['original'].data) == record['original_sha256'],
             'external original identity differs')
    fd = os.memfd_create('unchanged-external-input', 0)
    try:
        process = subprocess.Popen([str(assembler), *flags, '-I', '.', '-I', 'include',
                    '-o', '/proc/self/fd/'+str(fd), piece['file']], cwd=unit_dir,
                    env={'PATH':'/usr/bin:/bin', 'LC_ALL':'C'}, stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE, pass_fds=(fd,), start_new_session=True)
        try:
            out, error = process.communicate(timeout=30)
        except subprocess.TimeoutExpired:
            os.killpg(process.pid, signal.SIGKILL)
            process.communicate()
            raise AlignmentUnproved('external input memory assembly timed out')
        _require(process.returncode == 0 and not out and not error, 'external input assembly failed')
        size = os.lseek(fd, 0, os.SEEK_END)
        _require(0 < size <= 4*1024*1024, 'external assembler object exceeds bound')
        os.lseek(fd, 0, os.SEEK_SET)
        data = os.read(fd, size)
    finally:
        os.close(fd)
    image = Elf(data)
    sec = image.section(section)
    payload = image.section_bytes(sec)
    _require(all(type(n) is int for n in record['section_header']) and
             _hash(data) == record['object_sha256'] and
             [sec.type, sec.flags, sec.size, sec.align] == record['section_header'] and
             sec.type == 1 and sec.flags == 2 and sec.size == hi-lo and
             sec.align in (2, 4, 8, 16, 32, 64, 128) and
             not any(r.type in (4, 9) and r.size for r in image.sections) and
             payload == carve.va_bytes(context['original'], lo, hi-lo) and
             _hash(payload) == record['payload_sha256'], 'external complete object/retail bytes differ')
    first = [s for s in image.symbols if s.shndx == sec.index and s.type == 1 and
             s.value == 0 and s.size > 1 and s.name == items[0]['name']]
    _require(len(first) == 1 and first[0].size <= items[0]['end']-lo and
             any(payload[:first[0].size]), 'external first payload is absent or padding-only')
    for item in items:
        symbols = [s for s in image.symbols if s.name == item['name'] and s.shndx == sec.index
                   and s.type == 1 and s.value == item['start']-lo]
        _require(len(symbols) == 1 and 0 < symbols[0].size <= item['end']-item['start'],
                 'external item geometry/symbol identity differs')
    _check_pins(pins)
    input_ = dict(tu=tu['id'], piece=piece['name'], object='build/data/'+piece['name']+section+'.o',
                  section=section, raw_section=section, object_range=[0, hi-lo],
                  original_range=[lo, hi], alignment=sec.align)
    receipt = dict(schema='unchanged-external-input-receipt/1', authority=str(authority.resolve()),
                   authority_sha256=_hash(authority_bytes), record_sha256=_digest(record), inputs=pins,
                   object_sha256=_hash(data), payload_sha256=_hash(payload),
                   credited_bytes=0, grants_source_eligibility=False)
    return input_, receipt, image


def _case_vector_input(tu,piece,section,context,manifest,apis):
    """A corrected native case-vector recipe, distinct from unchanged input05."""
    carve,gate,native,audit,edit,Elf,Toolchain=apis
    if section != '.rodata' or piece.get('c_split') is not False or not hasattr(carve,'native_case_vector_provenance'):
        return None
    root,unit_dir=Path(context['root']),Path(context['unit_dir'])
    choices=[p for p in carve.native_case_vector_provenance(root,unit_dir,tu['id'],context['original'])
             if p['input']['original_range']==list(map(_number,(piece['start'],piece['end']))) and
             p['input']['source']==piece.get('file')]
    if not choices:return None
    _require(len(choices)==1,'ambiguous reconstructed native input')
    classification=choices[0];expected=classification['input']
    _require(classification['schema']=='native-case-vector-reconstruction/1' and
             classification['credited_bytes']==0 and classification['grants_source_eligibility'] is False and
             classification['original_input_alignment_proved'] is False and
             classification['original_source_type_proved'] is False and classification['alignment']==16 and
             not any(piece.get(k) for k in ('c_input_spans','c_section','support_only','typed_character_prefix_proof')),
             'wrong reconstructed native provider schema')
    tracked=json.loads((root/'config/objects/main.objects.json').read_bytes())
    owners=[o for o in tracked['tus'] if o['id']==tu['id']]
    current_pieces=tu['sections'][section]['pieces']
    registered=carve.load_registry(root)['tus'].get(tu['id'],{}).get(section,[])
    registered=registered.get('runs',[]) if isinstance(registered,dict) else registered
    c_pieces=[p for p in current_pieces if p.get('c_split') is True]
    split_owner=(len(owners)==1 and owners[0].get('owners',{}).get(section)=='asm' and
                 tu.get('owners',{}).get(section)=='split' and bool(c_pieces) and
                 all(any(list(map(_number,r['range']))==list(map(_number,(p['start'],p['end'])))
                         for r in registered) for p in c_pieces))
    _require(len(owners)==1 and all(tu.get(k)==owners[0].get(k) for k in
             ('id','name','path','mode','in_scope','import','functions')) and
             (tu.get('owners',{}).get(section)==owners[0].get('owners',{}).get(section) or split_owner) and
             [_piece_projection(p) for p in tu['sections'][section]['pieces']]==
             [_piece_projection(p) for p in manifest['sections'][section]['order'] if p['tu']==tu['name']],
             'reconstructed provider owner/local/global input order differs')
    selected_route='build/c/'+tu['name']+'.carved.o' if any(
        p.get('c_split') for e in tu['sections'].values() for p in e.get('pieces',[])) else None
    _require(tu.get('c_link_object')==selected_route,'reconstructed provider C route substituted')
    source=unit_dir/expected['source'];object_path=unit_dir/expected['object']
    _require(source.resolve().is_relative_to(unit_dir.resolve()) and
             object_path.resolve().is_relative_to(unit_dir.resolve()) and
             expected['object']=='build/data/'+piece['name']+'.rodata.o' and
             source.read_bytes()==expected['assembly'].encode() and
             _hash(source.read_bytes())==expected['assembly_sha256'],
             'ordinary generated case-vector source differs from semantic recipe')
    tc=Toolchain(root);assembler=tc.dir('ps2dev-binutils')/'mips64r5900el-ps2-elf-as'
    flags=['-EL','-march=r5900','-mabi=eabi','-mgp64','-G0','-no-pad-sections','-mno-fix-r5900']
    driver=root/'tools/tu/ninja_main.py'
    declared=re.findall(r'^ASFLAGS = "([^"\n]+)"$',driver.read_text(),re.M)
    _require(not tc.relaxed and declared==[' '.join(flags)],'ordinary case-vector assembler recipe differs')
    paths=[Path(p['path']) for p in classification['inputs']]+[source,assembler,driver,Path(carve.__file__)]
    pins=_pins(paths)
    fd=os.memfd_create('reconstructed-case-vector-input',0)
    try:
        process=subprocess.Popen([str(assembler),*flags,'-I','.','-I','include','-o','/proc/self/fd/'+str(fd),expected['source']],
            cwd=unit_dir,env={'PATH':'/usr/bin:/bin','LC_ALL':'C'},stdout=subprocess.PIPE,stderr=subprocess.PIPE,
            pass_fds=(fd,),start_new_session=True)
        try:out,error=process.communicate(timeout=30)
        except subprocess.TimeoutExpired:
            os.killpg(process.pid,signal.SIGKILL);process.communicate()
            raise AlignmentUnproved('case-vector input assembly timed out')
        _require(process.returncode==0 and not out and not error,'case-vector ordinary assembly failed')
        size=os.lseek(fd,0,os.SEEK_END);os.lseek(fd,0,os.SEEK_SET)
        _require(0<size<=4*1024*1024,'case-vector object exceeds bound')
        data=os.read(fd,size)
    finally:os.close(fd)
    byte_proof=carve.validate_native_case_vector_object(root,unit_dir,tu['id'],classification,data,context['original'])
    _check_pins(pins)
    input_=dict(tu=tu['id'],piece=piece['name'],object=expected['object'],section=section,raw_section=section,
                object_range=[0,expected['size']],original_range=expected['original_range'],alignment=16)
    receipt=dict(schema='reconstructed-native-case-vector-input/1',classification=classification,
                 byte_proof=byte_proof,inputs=pins,object_sha256=_hash(data),
                 assembler_argv=flags,assembler_sha256=_hash(assembler.read_bytes()),
                 credited_bytes=0,grants_source_eligibility=False,
                 original_alignment_proved=False,original_capacity_proved=False)
    return input_,receipt,Elf(data)


def _external_alignment(tu, section, index, layout, context, registry, apis, manifest,
                        next_tu, next_index, order_digest, external):
    pieces = tu['sections'][section]['pieces']
    piece, before = pieces[index], pieces[index-1]
    lo, hi = map(_number, (piece['start'], piece['end']))
    _require(index == len(pieces)-1 and next_tu['id'] != tu['id'] and next_index == 0,
             'external successor is not the immediate NEXT first physical input')
    left = _piece_spans(before, section, registry, tu['id'])
    _require(layout['section'] == left[0]['section'], 'external left raw section differs')
    owners = _geometry(left, layout, context, apis)
    left_input = _input(tu, before, left, layout, 'last')
    a = left[-1]['object_range'][1]
    _require(a == layout['size'] or (left_input['section'] != left[0]['section'] or _selected_object_route(left_input)) and
             left_input['object_range'] == left[-1]['object_range'],
             'external alignment needs a complete current LEFT input')
    right_input, receipt, image = external
    reconstructed=receipt.get('schema')=='reconstructed-native-case-vector-input/1'
    alignment = right_input['alignment']
    _require(0 < hi-lo <= 127 and (lo+alignment-1)&-alignment == hi,
             'unchanged external input alignment does not force the full gap')
    _original_gap(context, section, lo, hi, apis)
    return dict(schema=('native-case-vector-input-suffix-alignment/1' if reconstructed else
                        'external-input-suffix-alignment/1'), kind='external_input_alignment_fill',
                mode=('reconstructed_native_case_vector_input_alignment' if reconstructed else
                      'unchanged_external_input_linker_alignment'), tu=tu['id'], section=section,
                piece=piece['name'], range=[piece['start'], piece['end']], alignment=alignment,
                input_range=[a, a], owners=owners,
                neighbors=[dict(input=left_input, spans=left, owners=owners),
                           dict(input=right_input, spans=[], owners=[],
                                **({'native_case_vector_input':receipt} if reconstructed else {'external_input':receipt}))],
                contexts=[_context_receipt(context)], structure_sha256=_structure(tu, section, index),
                global_order_sha256=order_digest, compiler_fragment_sha256=_digest(layout),
                current_compiler_alignment_proved=False, current_linker_alignment_proved=True,
                original_alignment_proved=False, original_capacity_proved=False,
                credited_bytes=0, grants_source_eligibility=False, ordinary_object_validation_required=True)


def _alignment(tu, section, index, layout, context, registry, apis, manifest,
               contexts, layouts):
    pieces = tu['sections'][section]['pieces']
    piece = pieces[index]
    _require(index > 0, 'gap has no preceding compiler piece')
    before = pieces[index-1]
    lo, hi = map(_number, (piece['start'], piece['end']))
    suffixes = piece.get('uncredited_scaffold_suffixes', [])
    original_item = not suffixes and _original_item_candidate(context['original'], section, piece, apis[2])
    _require((original_item or len(suffixes) == 1 and suffixes[0].get('range') == [lo, hi] and
             suffixes[0].get('credited') is False and
             suffixes[0].get('original_capacity_proved') is False) and
             _number(before['end']) == lo, 'gap is not one adjacent unknown suffix')
    next_tu, next_index, order_digest = _global_order(manifest, tu, section, index)
    after = next_tu['sections'][section]['pieces'][next_index]
    _require(_number(after['start']) == hi, 'next actual input does not begin at gap end')
    if after.get('c_split') is False:
        reconstructed=_case_vector_input(next_tu,after,section,context,manifest,apis)
        if reconstructed is not None:
            return _external_alignment(tu,section,index,layout,context,registry,apis,manifest,
                                       next_tu,next_index,order_digest,reconstructed)
        external = _external_input(next_tu, after, section, context, manifest, apis)
        if external is not None:
            return _external_alignment(tu, section, index, layout, context, registry, apis, manifest,
                                       next_tu, next_index, order_digest, external)
    left = _piece_spans(before, section, registry, tu['id'])
    legacy = after.get('c_split') is False and not after.get('c_input_spans')
    right = None if legacy else _piece_spans(after, section, registry, next_tu['id'])
    left_section, right_section = left[0]['section'], section if legacy else right[0]['section']
    _require(layout['section'] == left_section, 'left raw section differs')
    next_context = contexts(next_tu, next_tu['id'] == tu['id'] or legacy)
    right_layout = layouts(next_tu, right_section, next_context)
    left_owners = _geometry(left, layout, context, apis)
    legacy_receipt = None
    if legacy:
        _require(next_tu['id'] != tu['id'] and next_index == 0,
                 'legacy successor is not NEXT first whole C input')
        _require([_piece_projection(p) for p in next_tu['sections'][section]['pieces']] ==
                 [_piece_projection(p) for p in manifest['sections'][section]['order']
                  if p.get('tu') == next_tu['name']], 'legacy NEXT local/global input order differs')
        right, right_owners, right_input, legacy_receipt = _legacy_literal_input(
            next_tu, after, section, right_layout, next_context, registry, apis)
    else:
        right_owners = _geometry(right, right_layout, next_context, apis)
        right_input = _input(next_tu, after, right, right_layout)
    a, b = left[-1]['object_range'][1], right[0]['object_range'][0]
    left_input = _input(tu, before, left, layout, 'last')
    alignment_receipt = None
    try:
        required, governing = _required_input_alignment(right_layout, right, right_input)
        _require((lo+required-1) & -required == hi,
                 'own constructor ALIGN does not require entire original gap')
    except AlignmentUnproved:
        required, alignment_receipt = _retained_input_alignment(right_layout, right_input)
        governing = alignment_receipt['witnesses']
    _require((lo+required-1) & -required == hi and right_input['alignment'] >= required and
             (lo+right_input['alignment']-1) & -right_input['alignment'] == hi,
             'actual right input alignment does not force the exact original gap')
    if next_tu['id'] == tu['id'] and left_section == right_section and lo-a == hi-b and 0 < b-a == hi-lo:
        mode, gap_end = 'affine_interobject', b
        aligns = [e for e in layout['events'] if e['kind'] == 'align' and
                  e['offset'] <= a < b <= e['end']]
        _require(len(aligns) == 1, 'gap is not one governing cc1 ALIGN step')
        _alignment_fragment(layout, a, b)
    elif (next_tu['id'] == tu['id'] and index+1 < len(pieces)
          and next_index == index+1):
        # Reordering may put a later raw constructor immediately after this
        # selected slice. A real left ALIGN corroborates the gap, while the
        # independently governing right input ALIGN actually requires it.
        # Complete current constructors can be linked in original order even
        # when their raw emission order is different. Their exact physical
        # input extents and alignment, rather than an invented affine raw gap,
        # are rechecked by whole ordinary split equality and the actual map.
        mode, gap_end, aligns = 'selected_input_linker_alignment', a, []
        _require((left_input['section'] != left_section or _selected_object_route(left_input)) and
                 left_input['object_range'] == left[-1]['object_range'],
                 'reordered LEFT is not a complete actual selected input')
    else:
        mode, gap_end, aligns = 'next_compiler_input', a, []
        adjacent = ((next_tu['id'] != tu['id'] and index == len(pieces)-1 and next_index == 0)
                    or (next_tu['id'] == tu['id'] and next_index == index+1))
        _require(adjacent and (a == layout['size'] or
                 (left_input['section'] != left_section or _selected_object_route(left_input)) and
                 left_input['object_range'] == left[-1]['object_range']),
                 'terminal proof needs complete actual left input and immediate next input')
        if b == 0:
            _require(right_input['section'] == right_section,
                     'successor is not the actual first selected input')
        else:
            # Original input order can start with a complete later raw
            # constructor after ordinary carvesplit reorders its C pieces.
            # Its own leading ALIGN (above), not a discarded later maximum,
            # must require the entire gap. Runtime binds the exact split,
            # selected offset0/size/alignment, aliases and real map placement.
            _require(next_tu['id'] != tu['id'] and next_index == 0
                     and (right_input['section'] != right_section or _selected_object_route(right_input)),
                     'cropped successor is not NEXT first actual selected input')
            _require([_piece_projection(p) for p in next_tu['sections'][section]['pieces']] ==
                     [_piece_projection(p) for p in manifest['sections'][section]['order']
                      if p.get('tu') == next_tu['name']],
                     'NEXT selected input order differs between local and global views')
    folded = []
    if apis[2].original_references(context['original'], lo, hi):
        _require(mode == 'affine_interobject' and next_tu['id'] == tu['id'],
                 'referenced fill is not one same-TU compiler ALIGN')
        folded = [_folded_array_reference(context, section, lo, hi, layout, registry, apis)]
    _original_gap(context, section, lo, hi, apis, folded)
    fragment = dict(left=layout, right=right_layout, governing=governing, alignments=aligns)
    return dict(schema='compiler-suffix-alignment/2', kind='compiler_alignment_fill',
                mode=mode, range=[piece['start'], piece['end']], alignment=required,
                tu=tu['id'], section=section, piece=piece['name'],
                input_range=[a, gap_end], owners=left_owners,
                **({'raw_gap_alignment': aligns[0]['alignment']} if mode == 'affine_interobject' else {}),
                neighbors=[dict(input=left_input, spans=left, owners=left_owners),
                           dict(input=right_input, spans=[] if legacy else right, owners=right_owners,
                                **({'legacy_whole_c_input': legacy_receipt} if legacy else {}))],
                compiler_fragment_sha256=_digest(fragment),
                structure_sha256=_structure(tu, section, index), global_order_sha256=order_digest,
                contexts=[_context_receipt(context), _context_receipt(next_context)],
                **({'current_input_alignment': alignment_receipt} if alignment_receipt else {}),
                current_compiler_alignment_proved=True, original_alignment_proved=False,
                original_capacity_proved=False, credited_bytes=0, grants_source_eligibility=False,
                **({'folded_array_references': folded} if folded else {}),
                **({'referenced_original_item_boundary': True} if original_item and folded else {}),
                **({'unreferenced_original_item_boundary': True} if original_item and not folded else {}),
                original_reference_evidence=('existing conservative index; every generating hit is the authenticated '
                    'direct-array compiler-folded base pair' if folded else
                    'existing conservative original_reference_addresses index; no hits'),
                ordinary_object_validation_required=True)


def _folded_item_candidate(original, section, piece, native):
    """Intake only; the positive source/native witness remains mandatory."""
    if section != '.data' or piece.get('c_split') is not False:
        return False
    lo, hi = map(_number, (piece['start'], piece['end']))
    sec = original.section(section)
    return hi-lo == 4 and any(s.shndx == sec.index and s.value == hi and s.type == 1 and
        s.bind == 0 and 0 < s.size <= 256 and s.size % 4 == 0 for s in original.symbols) and \
        native.original_references(original, lo, hi)


def _original_item_candidate(original, section, piece, native):
    """A separate original item may be current fill without being a cut tail.

    This is intake, not zero-byte capacity inference. Admission still requires
    both complete source constructors, exact compiler/input alignment, full
    ordinary split/reference checks and the real map's zero-input fill.
    """
    if _folded_item_candidate(original, section, piece, native):
        return True
    if section not in ('.data','.sdata','.rodata') or piece.get('c_split') is not False:
        return False
    lo, hi = map(_number, (piece['start'],piece['end']))
    sec = original.section(section)
    return (sec.type == 1 and sec.flags & 2 and 0 < hi-lo <= 127 and
        sec.addr <= lo < hi <= sec.addr+sec.size and
        original.section_bytes(sec)[lo-sec.addr:hi-sec.addr] == bytes(hi-lo) and
        not any(s.shndx == sec.index and s.type not in (3,4) and
                (lo <= s.value < hi or s.size and s.value < hi and lo < s.value+s.size)
                for s in original.symbols) and not native.original_references(original,lo,hi))


def compiler_suffix_alignments(root, unit_dir, manifest):
    """Prove current compiler/linker fill; never infer original source capacity."""
    apis = _apis(root)
    registry = apis[0].load_registry(root)
    original = apis[5]((Path(unit_dir)/'orig'/json.loads(
        (Path(root)/'config/originals.json').read_bytes())['units']['main']['file']).read_bytes())
    proofs, refusals, context_cache, layout_cache = [], [], {}, {}
    compiler_suffix_alignments.last_refusals = refusals

    def contexts(tu, complete):
        key = (tu['id'], complete)
        if key not in context_cache:
            _require(tu['id'].startswith('main/') and tu.get('mode') == 'c' and tu.get('in_scope'),
                     'not a MAIN C input TU')
            context_cache[key] = _source_context(root, unit_dir, tu, apis, complete)
        value = context_cache[key]
        if isinstance(value, BaseException):
            raise value
        return value

    # 2026-11-01: the per-TU cpp/cc1 context compiles are independent; run the
    # ones the loop below needs in parallel (results and refusals unchanged).
    wanted = [tu for tu in manifest['tus']
              if tu['id'].startswith('main/') and tu.get('mode') == 'c' and tu.get('in_scope')
              and any((piece.get('uncredited_scaffold_suffixes') or
                       _original_item_candidate(original, sec, piece, apis[2]))
                      for sec, entry in tu.get('sections', {}).items()
                      for piece in entry.get('pieces', []))]
    def prefetch(tu):
        try:
            return _source_context(root, unit_dir, tu, apis, True)
        except AlignmentUnproved as error:
            return error
    import concurrent.futures
    with concurrent.futures.ThreadPoolExecutor(max_workers=8) as pool:
        for tu, value in zip(wanted, pool.map(prefetch, wanted)):
            context_cache[(tu['id'], True)] = value

    def layouts(tu, section, context):
        key = (tu['id'], section)
        if key not in layout_cache:
                layout_cache[key] = _layout(context['assembly'], context['preprocessed'], unit_dir, section, apis, context)
        return layout_cache[key]

    for tu in manifest['tus']:
        for sec, entry in tu.get('sections', {}).items():
            for index, piece in enumerate(entry.get('pieces', [])):
                if not piece.get('uncredited_scaffold_suffixes') and not \
                   _original_item_candidate(original, sec, piece, apis[2]):
                    continue
                try:
                    context = contexts(tu, True)
                    left = _piece_spans(entry['pieces'][index-1], sec, registry, tu['id']) if index else []
                    _require(left, 'suffix has no registered left input')
                    layout = layouts(tu, left[0]['section'], context)
                    proof = _alignment(tu, sec, index, layout, context, registry, apis,
                                       manifest, contexts, layouts)
                    rows = [r for r in manifest['sections'][sec]['order']
                            if r['tu'] == tu['name'] and r['piece'] == piece['name']]
                    _require(len(rows) == 1, 'missing/duplicate global suffix')
                    piece['linker_padding'] = proof
                    rows[0]['linker_padding'] = proof
                    proofs.append(proof)
                except (ValueError, RuntimeError, OSError, KeyError, IndexError, TypeError,
                        apis[0].CarveError) as error:
                    refusals.append(dict(tu=tu['id'], section=sec,
                                         range=[piece['start'], piece['end']], reason=str(error)))
    _marker_bijection(manifest)
    return proofs


def _object_gap(elf, section, proof, carve):
    secs = [s for s in elf.sections if s.name == section]
    _require(elf.e_type == 1 and len(secs) == 1, 'missing/ambiguous raw compiler section')
    sec = secs[0]
    _require(type(sec.align) is int and sec.align > 0 and sec.align & (sec.align-1) == 0,
             'invalid raw input alignment')
    a, b = proof['input_range']
    terminal = proof.get('mode') in ('next_compiler_input',
                                    'unchanged_external_input_linker_alignment',
                                    'reconstructed_native_case_vector_input_alignment')
    selected_boundary = proof.get('mode') == 'selected_input_linker_alignment' or (terminal and a != sec.size)
    if selected_boundary:
        left = proof['neighbors'][0]['input']
        _require(a == b and 0 <= a <= sec.size and left['raw_section'] == section
                 and (left['section'] != section or _selected_object_route(left)) and left['object_range'][1] == a
                 and left['original_range'][1] == _number(proof['range'][0]),
                 'selected LEFT is not a complete actual input boundary')
        _require(not any(s.shndx == sec.index and s.type not in (3, 4) and s.size
                         and s.value < a < s.value+s.size for s in elf.symbols),
                 'selected LEFT cut crosses a current source OBJECT')
    elif terminal:
        _require(a == b == sec.size, 'terminal left is not the full raw section end')
        b = a + _number(proof['range'][1]) - _number(proof['range'][0])
    _require(sec.type == 1 and sec.flags & 2 and
             (selected_boundary or (terminal and 0 <= a < b) or
              (0 <= a < b <= sec.size and sec.align >= proof.get('raw_gap_alignment',proof['alignment'])
               and elf.section_bytes(sec)[a:b] == bytes(b-a))),
             'ordinary raw compiler alignment bytes differ')
    for owner in proof['owners']:
        if owner.get('kind') in ('anonymous','alignment'):
            continue
        matches = [s for s in elf.symbols if s.name == owner['name'] and s.shndx == sec.index and
                   (s.value, s.size, s.type, s.bind) ==
                   (owner['offset'], owner['size'], owner['type'], owner['binding'])]
        _require(len(matches) == 1, 'ordinary raw OBJECT differs from cc1 owner')
    _require(not any(s.shndx == sec.index and s.type not in (3, 4) and
                     (a <= s.value < b or s.size and s.value < b and a < s.value + s.size)
                     for s in elf.symbols), 'ordinary raw symbol overlaps alignment')
    if not selected_boundary:
        _reference_gap(elf, sec, a, b, carve, folded=proof.get('folded_array_references', ()))
    # There is no raw affine gap in selected-boundary mode. Both actual
    # selected objects are still checked in ORIGINAL coordinates by the
    # mandatory _reference_gap calls in the runtime validator.


def _relative_address(value):
    # Selected carve section aliases use the ordinary ELF32 two's-complement
    # negative base. Resolve their actual S+A rather than trusting a signless
    # symbol field; this also catches a negative addend before the next input.
    return ((value + (1 << 31)) & 0xffffffff) - (1 << 31)


def _unpaired_lo_excludes_gap(base, word, interval, carve):
    """Exclude every 64-KiB lift by its fixed low bits; infer no high/address."""
    lo, hi = interval
    # Ordinary I-type address constructors (including EE lq/sq) and ORI.
    # Jumps, branches, LUI, COP formats and raw words supply no such witness.
    _require(word >> 26 in (8, 9, 13, 24, 25, 26, 27, 30, 31,
                            32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43,
                            44, 45, 46, 47, 48, 49, 50, 51, 53, 54, 55,
                            56, 57, 58, 59, 61, 62, 63),
             'unpaired LO16 lacks an ordinary immediate address constructor')
    _require(type(base) is int and 0 < hi-lo <= 127,
             'unpaired LO16 exclusion needs a bounded original gap')
    low = (base + carve._sext16(word)) & 0xffff
    _require(not any((address & 0xffff) == low for address in range(lo, hi)),
             'unpaired/reused LO16 may address an original-gap 64-KiB lift')


def _folded_pair_allowed(elf, source, target, high_offset, low_offset, address,
                         interval, witnesses, entries, resolved, original_gap):
    """Classify only the exact rederived pair; all other references still fail."""
    if len(witnesses) != 1:
        return False
    witness = witnesses[0]
    if witness.get('schema') != 'direct-array-index-minus-one-base/1':
        return False
    if original_gap is not None and list(original_gap) != witness['original_range']:
        return False
    if address != interval[0] or interval[1]-interval[0] != 4:
        return False
    functions = [s for s in elf.symbols if s.name == witness['function'] and
                 (s.type, s.bind, s.size, s.shndx) ==
                 (2, witness['function_binding'], witness['function_size'], source.index)]
    if len(functions) != 1 or source.name != witness['source_section']:
        return False
    function = functions[0]
    if (high_offset-function.value, low_offset-function.value) != \
       (witness['high_offset'], witness['low_offset']):
        return False
    data = elf.section_bytes(source)
    if list(struct.unpack_from('<4I', data, high_offset-4)) != witness['words']:
        return False
    if resolved is None:
        expected = witness['raw_target']
        if (target.name, target.value, target.type, target.bind,
            elf.sections[target.shndx].name) != \
           (expected['name'], expected['value'], expected['type'], expected['binding'], expected['section']):
            return False
        if list(interval) != witness['raw_range']:
            return False
    else:
        tables = [(i,s) for i,s in enumerate(elf.symbols) if s.name == witness['table'] and
                  (s.type,s.bind,s.size) == (1,0,witness['table_size']) and
                  0 < s.shndx < len(elf.sections)]
        if len(tables) != 1:
            return False
        table_index, table = tables[0]
        if resolved.get(table_index,{}).get('value') != witness['original_range'][1]:
            return False
        target_index = next((i for i,s in enumerate(elf.symbols) if s is target),None)
        mapped = resolved.get(target_index,{})
        target_section = elf.sections[target.shndx] if 0 < target.shndx < len(elf.sections) else None
        if target_section is None or target.type != 0 or target.bind != 0 or \
           not re.fullmatch(r'\.Lcarve'+re.escape(witness['raw_target']['section'])+r'\.[0-9]+',target.name) or \
           mapped.get('value') != witness['original_range'][1]+witness['raw_target']['value']-witness['table_raw_offset'] or \
           resolved.get('_sections',{}).get(target.shndx) is None or \
           resolved['_sections'][target.shndx]+_relative_address(target.value) != mapped['value']:
            return False
    selector = witness['selector']
    sh, sl = (function.value+selector[k] for k in ('high_offset','low_offset'))
    actual = [e for e in entries if e[0] in (sh, sl)]
    if len(actual) != 2 or [e[0:2] for e in actual] != [(sh, 5), (sl, 6)] or \
       actual[0][2] != actual[1][2] or any(e[3] is not None for e in actual):
        return False
    symbol = elf.symbols[actual[0][2]]
    if symbol.name != selector['global_name'] or \
       [struct.unpack_from('<I', data, at)[0] for at in (sh, sl)] != selector['words'] or \
       [w & (0xffff0000 if w >> 26 in (49,53) else 0xffffffff) for w in
        struct.unpack_from('<'+'I'*len(selector['before']), data, sh+4)] != selector['before'] or \
       list(struct.unpack_from('<'+'I'*len(selector['between']), data, sl+4)) != selector['between']:
        return False
    return True


def _reference_gap(elf, sec, a, b, carve, resolved=None, original_gap=None, folded=()):
    for rel in elf.sections:
        if rel.type not in (4, 9):
            continue
        width = 12 if rel.type == 4 else 8
        _require(rel.entsize == width and rel.size % width == 0 and
                 type(rel.info) is int and 0 <= rel.info < len(elf.sections) and
                 type(rel.link) is int and 0 <= rel.link < len(elf.sections) and
                 elf.sections[rel.link].type == 2 and
                 sum(s.type == 2 for s in elf.sections) == 1 and
                 0 <= rel.offset <= len(elf.data) - rel.size,
                 'malformed ordinary REL/RELA section')
        entries = carve._rel_entries(elf, rel)
        source = elf.sections[rel.info]
        data = elf.section_bytes(source)
        pending = {}
        for i, (offset, kind, target_index, addend) in enumerate(entries):
            _require(type(offset) is int and 0 <= offset <= len(data) - 4 and offset % 4 == 0
                     and type(target_index) is int and 0 <= target_index < len(elf.symbols),
                     'bad ordinary relocation')
            supported = kind in (2, 4, 5, 6, 7, 8)
            target = elf.symbols[target_index]
            mapped = (resolved or {}).get(target_index)
            if mapped is not None:
                _require('value' in mapped, 'undefined ordinary alias has no unique linked identity')
                _require(not original_gap[0] <= mapped['value'] < original_gap[1] or
                         mapped.get('generated_carve_basis') is True,
                         'ordinary resolved alias identifies original alignment')
            _require(supported or (source.index != sec.index and target.shndx != sec.index),
                     'unsupported relocation kind/width affects alignment section')
            _require(not (source.index == sec.index and offset < b and a < offset + 4), 'relocation inside alignment')
            local = target.shndx == sec.index
            if not local and mapped is None:
                continue
            if not supported:
                raise AlignmentUnproved('unsupported relocation against resolved alias')
            base, interval = ((target.value, (a, b)) if local else
                              (mapped['value'], original_gap))
            if kind == 4:
                # R_MIPS_26 supplies the low 28 bits of a real J/JAL target.
                # Require actual placement for P and S; a symbol's FUNC/SECTION
                # classification is neither an address proof nor an exemption.
                source_base = (resolved or {}).get('_sections', {}).get(source.index)
                _require(not local and source_base is not None,
                         'R_MIPS_26 needs actual resolved source/target placements')
                word = struct.unpack_from('<I', data, offset)[0]
                _require(word >> 26 in (2, 3), 'R_MIPS_26 is not a J/JAL constructor')
                displacement = (word & 0x03ffffff) << 2 if addend is None else addend
                address = base + displacement
                _require(type(displacement) is int and displacement % 4 == 0
                         and 0 <= address <= 0xffffffff and
                         address & 0xf0000000 == (source_base+offset+4) & 0xf0000000,
                         'R_MIPS_26 address/region is unsupported')
                _require(not original_gap[0] <= address < original_gap[1],
                         'ordinary J/JAL references original alignment')
                continue
            if addend is None:
                word = struct.unpack_from('<I', data, offset)[0]
                if kind == 2:
                    addend = struct.unpack_from('<i', data, offset)[0]
                elif kind == 5:
                    # The linker resolves every outstanding HI for this
                    # symbol at the following LO. Do not let a prior pair
                    # authenticate a later, independently addressed LO.
                    pending.setdefault(target_index, []).append((word & 0xffff, offset))
                    continue
                elif kind == 6:
                    highs = pending.pop(target_index, [])
                    if not highs:
                        _require(local or mapped is not None and original_gap is not None,
                                 'unpaired/reused storage LO16 needs a separate address proof')
                        lo_base, lo_interval = ((mapped['value'], original_gap) if mapped is not None
                                                else (target.value, (a, b)))
                        _unpaired_lo_excludes_gap(lo_base, word, lo_interval, carve)
                        continue
                    for high, high_offset in highs:
                        address = _relative_address(base + (high << 16) + carve._sext16(word))
                        _require(not interval[0] <= address < interval[1] or len(highs) == 1 and
                                 _folded_pair_allowed(elf, source, target, high_offset, offset, address,
                                      interval, folded, entries, resolved, original_gap),
                                 'compiler HI16/LO16 references alignment')
                    continue
                elif kind in (7, 8):
                    addend = carve._sext16(word) + (carve.gp0(elf) if target.bind == 0 else 0)
                else:
                    raise AlignmentUnproved('unsupported relocation against alignment section')
            _require(not interval[0] <= _relative_address(base + addend) < interval[1],
                     'compiler relocation references alignment')
        _require(not pending, 'unpaired storage HI16')


def _resolved_targets(elf, input_, placed, linked):
    """Resolve current ordinary aliases from actual input placements/linked identities."""
    _require(linked is not None, 'runtime references require the actual linked ELF')
    names = {}
    for symbol in linked.symbols:
        if symbol.name and symbol.shndx not in (0, 0xfff2):
            names.setdefault(symbol.name, set()).add((symbol.value, symbol.type))
    out = {'_sections': {section.index: placed[(input_['object'], section.name)][0]
                        for section in elf.sections if (input_['object'], section.name) in placed}}
    for i, symbol in enumerate(elf.symbols):
        if 0 < symbol.shndx < len(elf.sections):
            section = elf.sections[symbol.shndx]
            location = placed.get((input_['object'], section.name))
            if location is not None:
                out[i] = dict(value=location[0]+_relative_address(symbol.value), type=symbol.type)
                basis = getattr(elf, '_completion_carve_bases', {}).get(i)
                if basis:
                    _require(symbol.type == 0 and symbol.bind == 0 and symbol.size == 0 and
                             location == (basis['original_start'],basis['size']) and
                             section.size == basis['size'] and
                             symbol.value == (-basis['raw_offset']) & 0xffffffff and
                             out[i]['value'] == basis['original_start']-basis['raw_offset'],
                             'generated selected-section basis differs from actual input')
                    # This is an ordinary relocation coordinate, not a storage
                    # identity or an exemption for an effective S+A in the gap.
                    out[i]['generated_carve_basis'] = True
        elif symbol.shndx == 0xfff1:
            out[i] = dict(value=symbol.value, type=symbol.type)
        elif symbol.shndx == 0 and symbol.name:
            matches = names.get(symbol.name, set())
            if len(matches) == 1:
                value, kind = next(iter(matches))
                out[i] = dict(value=value, type=kind)
            else:
                out[i] = dict(unresolved=True)
    return out


def _object_layout(elf, section, layout, owners):
    _require(elf.e_type == 1 and len([s for s in elf.sections if s.name == section]) == 1,
             'ordinary raw object has ambiguous section')
    sec = elf.section(section)
    _require(sec.type == 1 and sec.flags & 2 and sec.size == layout['size']
             and sec.align == layout['alignment'] and sec.offset >= 0
             and sec.offset+sec.size <= len(elf.data), 'ordinary raw cc1 layout/alignment differs')
    for owner in owners:
        a, b = owner['offset'], owner['offset']+owner['size']
        _require(0 <= a < b <= sec.size, 'ordinary owner exceeds raw section')
        if owner['kind'] == 'named':
            found = [s for s in elf.symbols if s.name == owner['name'] and s.shndx == sec.index
                     and (s.value, s.size, s.type, s.bind) ==
                     (a, b-a, owner['type'], owner['binding'])]
            _require(len(found) == 1, 'ordinary raw named OBJECT differs')
        else:
            _require(not any(s.shndx == sec.index and s.type not in (0, 3, 4)
                             and (a <= s.value < b or s.size and s.value < b and a < s.value+s.size)
                     for s in elf.symbols), 'ordinary anonymous span has a named OBJECT')


def _common_geometry(raw, layout, context, apis):
    commons = layout.get('commons', [])
    if not commons:
        return
    carve, gate, native, audit, edit, Elf, Toolchain = apis
    compiler = _compiler_object(context, apis)
    for common in commons:
        name = common['name']
        def declaration(text):
            items = []
            for item in edit.TU(text).free_items():
                tokens = edit.c_tokens(item.text(text))
                if item.kind == 'decl' and not item.conditional and name in tokens and '=' not in tokens \
                   and 'extern' not in tokens and 'static' not in tokens:
                    items.append((item, tokens))
            _require(len(items) == 1, 'COMMON lacks one actual uninitialized C declaration')
            return items[0]
        source, tokens = declaration(context['source_text'])
        pp, pp_tokens = declaration(context['preprocessed'])
        origin, line = context['pp'].attribute(pp.start)
        p = Path(origin or '')
        if not p.is_absolute():
            p = Path(context['unit_dir'])/p
        _require(p.resolve() == context['source'].resolve() and tokens == pp_tokens and
                 edit.line_of(context['source_text'], source.start) <= line <=
                 edit.line_of(context['source_text'], source.end), 'COMMON C/CPP declaration differs')
        emitted = [s for s in compiler.symbols if s.name == name]
        _require(len(emitted) == 1 and emitted[0].shndx in (0xfff2, 0xff03) and emitted[0].type == 1
                 and emitted[0].bind == common['binding'] and emitted[0].size == common['size']
                 and emitted[0].value == common['alignment'], 'actual C COMMON identity differs')
        found = [s for s in raw.symbols if s.name == name and s.type == 1
                 and s.bind == common['binding'] and s.size == common['size']]
        _require(len(found) == 1, 'ordinary COMMON/allocated identity differs')
        symbol = found[0]
        if symbol.shndx in (0xfff2, 0xff03):
            _require(symbol.shndx == emitted[0].shndx and symbol.value == common['alignment'],
                     'ordinary COMMON namespace/alignment differs')
        else:
            _require(0 < symbol.shndx < len(raw.sections), 'COMMON has no ordinary allocator section')
            section = raw.sections[symbol.shndx]
            expected_sections = ('.scommon',) if emitted[0].shndx == 0xff03 else ('.bss', '.sbss')
            _require(section.name in expected_sections and section.type == 8 and section.flags == 3
                     and section.align >= common['alignment'] and symbol.value % common['alignment'] == 0
                     and 0 <= symbol.value <= section.size-common['size'],
                     'COMMON allocation has wrong kind/extent/alignment')


def _ordinary_context(root, unit_dir, tu, receipt, apis):
    carve, gate, native, audit, edit, Elf, Toolchain = apis
    _require(receipt['tu'] == tu['id'] and type(receipt['require_complete']) is bool,
             'ordinary source context identity differs')
    _check_pins(receipt['inputs'])
    record = carve._character_tu_record(root, tu['id'])
    _contract_tools(root, record['contract'], Toolchain)
    source = Path(root)/record['path']
    _require(str(source) == receipt['source'] and _hash(source.read_bytes()) == receipt['source_sha256']
             and _digest(record['contract']) == receipt['contract_sha256'], 'source/contract changed')
    raw = Path(unit_dir)/('build/c/'+tu['name']+'.o')
    assembly_path, pp_path = Path(str(raw)+'.s'), Path(str(raw)+'.i')
    assembly = assembly_path.read_text(errors='surrogateescape')
    preprocessed = pp_path.read_text(errors='surrogateescape')
    pp, headers, tokens = _pp_facts(preprocessed, unit_dir, audit, edit)
    _require(headers == receipt['headers'] and tokens == receipt['preprocessed_tokens_sha256'],
             'ordinary CPP tokens/headers differ')
    required, emitted = {f['name'] for f in tu.get('functions', [])}, gate.cc1_functions(assembly)
    _require(required and (required <= emitted if receipt['require_complete'] else bool(required & emitted)),
             'ordinary TU lacks required C function emission')
    original_path = Path(unit_dir)/'orig'/json.loads(
        (Path(root)/'config/originals.json').read_bytes())['units']['main']['file']
    context = dict(receipt, source=source, source_text=source.read_text(errors='surrogateescape'),
                   preprocessed=preprocessed, pp=pp, assembly=assembly,
                   root=str(root), unit_dir=str(unit_dir), original=Elf(original_path.read_bytes()),
                   ordinary_paths=dict(raw=raw, assembly=assembly_path, preprocessed=pp_path),
                   function_scope=[dict(original_symbol=f['name'],name=f['name'],
                                        va=_number(f['va']),size=f['size']) for f in tu['functions']])
    _require(_context_receipt(context) == receipt, 'ordinary context receipt differs')
    return context


def _selected_input(root, unit_dir, tu, input_, context, layout, owners, registry, apis):
    carve, gate, native, audit, edit, Elf, Toolchain = apis
    raw_path = Path(context['ordinary_paths']['raw'])
    raw = Elf(raw_path.read_bytes())
    _object_layout(raw, input_['raw_section'], layout, owners)
    _common_geometry(raw, layout, context, apis)
    for owner in owners:
        tail = owner.get('compiler_alignment_tail')
        if tail:
            _object_gap(raw, input_['raw_section'],
                        dict(input_range=tail['range'], alignment=tail['alignment'], owners=[]), carve)
    # Reuse the ordinary split planner's complete registered-byte and relocation
    # comparison. This is not a new eligibility or normalization route.
    plans = carve.split_plans(root, 'main', unit_dir, tu['id'], raw, registry)
    plans = {section: (plan if isinstance(plan, dict) else dict(pieces=plan, notes=[]))
             for section, plan in plans.items()}
    selected_path = Path(unit_dir)/input_['object']
    _require(selected_path.resolve().is_relative_to(Path(unit_dir).resolve()) and
             selected_path.is_file(), 'missing/outside ordinary selected object')
    if selected_path.resolve() == raw_path.resolve():
        selected, extra = raw, []
        _require(input_['section'] == input_['raw_section'] and
                 input_['object_range'] == [0, layout['size']],
                 'unsplit object is only a partial input view')
    else:
        sidecar = Path(str(selected_path)+'.json')
        detail = json.loads(sidecar.read_bytes())
        _require(detail.get('object') == raw_path.relative_to(unit_dir).as_posix() and
                 detail.get('out') == selected_path.relative_to(unit_dir).as_posix(),
                 'selected carve sidecar route differs')
        # Comparing the existing ordinary splitter's whole output also binds
        # relocation retargeting, slice order, symbol aliases and real alignment.
        _require(carve.split_object(raw.data, {section: plan['pieces'] for section, plan in plans.items()})
                 == selected_path.read_bytes(),
                 'actual selected object differs from ordinary validated split')
        selected, extra = Elf(selected_path.read_bytes()), [sidecar]
        bases = {}
        for base, plan in plans.items():
            if base not in ('.data','.sdata','.rodata','.lit4','.lit8'):
                continue
            for k, part in enumerate(plan['pieces']):
                name = '.Lcarve'+base+'.'+str(k)
                selected_name = carve.split_section_name(base,k)
                matches = [(j,s) for j,s in enumerate(selected.symbols) if s.name == name]
                if not matches:
                    continue
                _require(len(matches) == 1 and part['offset'] > 0,
                         'ambiguous/unsupported generated carve basis')
                j,symbol = matches[0]
                _require(symbol.type == 0 and symbol.bind == 0 and symbol.size == 0 and
                         0 < symbol.shndx < len(selected.sections) and
                         selected.sections[symbol.shndx].name == selected_name and
                         symbol.value == (-part['offset']) & 0xffffffff,
                         'selected alias differs from ordinary raw-section basis')
                bases[j] = dict(raw_offset=part['offset'],size=part['length'],original_start=part['lo'])
        selected._completion_carve_bases = bases
    secs = [s for s in selected.sections if s.name == input_['section']]
    _require(len(secs) == 1, 'missing/ambiguous actual selected section')
    sec = secs[0]
    _require(sec.type == 1 and sec.flags & 2 and sec.size == input_['original_range'][1]-input_['original_range'][0]
             and sec.align == input_['alignment'], 'actual selected input size/alignment differs')
    return raw, selected, dict((str(p.resolve()), _hash(p.read_bytes()))
                     for p in [raw_path, selected_path, *context['ordinary_paths'].values(), *extra])


def _actual_fill(proof, placed, fills):
    lo, hi = map(_number, proof['range'])
    _require(not any(type(size) is not int or size < 0 or
                     size > 0 and start < hi and lo < start+size
                     for (path, section), (start, size) in placed.items()),
             'positive/malformed actual input overlaps alignment')
    overlaps = [(start, end) for start, end in fills if start < hi and lo < end]
    _require(overlaps == [(lo, hi)], 'map does not contain exactly the proved real fill')
    right = proof['neighbors'][1]['input']
    _require(placed.get((right['object'], right['section'])) ==
             (hi, right['original_range'][1]-hi), 'actual next input placement differs')
    left = proof['neighbors'][0]['input']
    _require(placed.get((left['object'], left['section'])) ==
             (left['original_range'][0], lo-left['original_range'][0]),
             'actual left selected input placement differs')


def _legacy_literal_runtime(raw, selected, neighbor, context, layout, placed, fills, linked, apis):
    receipt, input_ = neighbor['legacy_whole_c_input'], neighbor['input']
    _check_pins([receipt['ownership_input']])
    carve, gate, native, audit, edit, Elf, Toolchain = apis
    lo, end = receipt['current_range']
    physical_lo, physical_hi = receipt['physical_range']
    payload, count = _legacy_literal_bytes(layout, native)
    _require(selected is raw and context['require_complete'] is True and linked is not None
             and input_['object_range'] == [0, len(payload)] and lo == physical_lo
             and end-lo == len(payload) and receipt['payload_sha256'] == _hash(payload)
             and receipt['literal_count'] == count,
             'legacy input is cropped/substituted or lacks complete current literal extent')
    for image in (raw, context['original'], linked):
        section = image.section(input_['section'])
        actual = image.section_bytes(section) if image is raw else carve.va_bytes(image, lo, len(payload))
        _require(actual == payload, 'ordinary/original/linked legacy literal input bytes differ')
        _require(not any(rel.type in (4, 9) and rel.info == section.index and rel.size
                         for rel in image.sections), 'legacy literal section has outgoing relocations')
    _require(placed.get((input_['object'], input_['section'])) == (lo, len(payload)),
             'legacy whole ordinary input placement/size differs')
    if end != physical_hi:
        _require(end < physical_hi and
                 not any(size > 0 and start < physical_hi and end < start+size
                         for start, size in placed.values()) and
                 [(a, b) for a, b in fills if a < physical_hi and end < b] == [(end, physical_hi)],
                 'legacy physical suffix is not its separate existing real map fill')


def validate_compiler_suffix_alignments(root, unit_dir, tu, compiled_path, assembly_path,
                                        manifest=None, placed=None, fills=None, linked=None):
    """Re-prove both contexts, raw/selected objects, global order and exact map fill."""
    validate_compiler_suffix_alignments.last_artifacts = {}
    refusal_errors = (ValueError, RuntimeError, OSError, KeyError, IndexError, TypeError,
                      StopIteration, struct.error)
    try:
        _require(manifest is not None, 'runtime proof requires the actual whole manifest')
        markers = _marker_bijection(manifest)
        selected = [(sec, i, p['linker_padding']) for sec, entry in tu.get('sections', {}).items()
                    for i, p in enumerate(entry.get('pieces', []))
                    if p.get('linker_padding', {}).get('kind') in COMPLETION_FILL_KINDS]
        if not selected:
            return True
        _require(placed is not None and fills is not None, 'runtime proof requires actual map inputs/fill')
        apis = _apis(root)
        carve, gate, native, audit, edit, Elf, Toolchain = apis
        refusal_errors += (carve.CarveError,)
        expected = Path(unit_dir)/('build/c/'+tu['name']+'.o')
        _require(Path(compiled_path).resolve() == expected.resolve() and
                 Path(assembly_path).resolve() == Path(str(expected)+'.s').resolve(),
                 'ordinary compiler sidecar route differs')
        registry, artifacts = carve.load_registry(root), {}
        for sec, index, proof in selected:
            external = proof.get('kind') == 'external_input_alignment_fill'
            reconstructed=external and proof.get('mode')=='reconstructed_native_case_vector_input_alignment'
            _require(proof.get('schema') == ('native-case-vector-input-suffix-alignment/1' if reconstructed else
                                            'external-input-suffix-alignment/1' if external
                                            else 'compiler-suffix-alignment/2') and
                     proof.get('credited_bytes') == 0 and proof.get('grants_source_eligibility') is False and
                     proof.get('current_compiler_alignment_proved') is (not external) and
                     (not external or proof.get('current_linker_alignment_proved') is True and
                      proof.get('mode') in ('unchanged_external_input_linker_alignment',
                                            'reconstructed_native_case_vector_input_alignment')) and
                     proof.get('original_capacity_proved') is False and
                     proof.get('original_alignment_proved') is False, 'wrong completion-only schema')
            receipts = {}
            _require(len(proof['contexts']) == (1 if external else 2), 'missing neighbor context receipts')
            for receipt in proof['contexts']:
                key = (receipt['tu'], receipt['require_complete'])
                _require(key not in receipts or receipt == receipts[key], 'conflicting context receipts')
                receipts[key] = receipt
            cache, layout_cache = {}, {}
            def contexts(owner, complete):
                key = (owner['id'], complete)
                _require(key in receipts, 'missing authenticated next source context')
                if key not in cache:
                    cache[key] = _ordinary_context(root, unit_dir, owner, receipts[key], apis)
                return cache[key]
            def layouts(owner, section, context):
                key = (owner['id'], section)
                if key not in layout_cache:
                    layout_cache[key] = _layout(context['assembly'], context['preprocessed'], unit_dir, section, apis, context)
                return layout_cache[key]
            context = contexts(tu, True)
            left_section = proof['neighbors'][0]['input']['raw_section']
            layout = layouts(tu, left_section, context)
            actual = _alignment(tu, sec, index, layout, context, registry, apis,
                                manifest, contexts, layouts)
            _require(actual == proof, 'ordinary compiler/registry/global-order proof differs')
            for neighbor in proof['neighbors']:
                owner = next(t for t in manifest['tus'] if t['id'] == neighbor['input']['tu'])
                if neighbor.get('native_case_vector_input'):
                    _require(reconstructed and neighbor is proof['neighbors'][1],
                             'reconstructed native input is not the exact right neighbor')
                    ext_piece=next(p for p in owner['sections'][sec]['pieces'] if p['name']==neighbor['input']['piece'])
                    ext=_case_vector_input(owner,ext_piece,sec,context,manifest,apis)
                    _require(ext is not None and ext[:2]==(neighbor['input'],neighbor['native_case_vector_input']),
                             'reconstructed native input proof differs')
                    input_,receipt,reproduced=ext
                    object_path=Path(unit_dir)/input_['object'];data=object_path.read_bytes()
                    _require(data==reproduced.data,'actual corrected object differs from whole ordinary assembler replay')
                    classification=receipt['classification']
                    actual_native=carve.validate_native_case_vector_input(root,unit_dir,owner['id'],classification,
                                                                         object_path,context['original'])
                    _require({k:v for k,v in actual_native.items() if k!='object'}==receipt['byte_proof'],
                             'actual corrected object relocation proof differs')
                    size=classification['input']['size'];start=input_['original_range'][0]
                    _require(linked is not None and carve.va_bytes(linked,start,size)==
                             carve.va_bytes(context['original'],start,size),
                             'linked reconstructed full provider payload differs')
                    resolved=_resolved_targets(reproduced,input_,placed,linked)
                    # Exact R32 meanings are independently bound to the source
                    # recipe, original FUNCTION/internal target and actual linked
                    # pointer slot. This also covers internal .L labels omitted
                    # from the final symbol table; no name-only address guess.
                    for meaning in receipt['byte_proof']['relocation_meaning']:
                        target=meaning['target']-meaning['addend']
                        candidates=[(j,s) for j,s in enumerate(reproduced.symbols) if s.name==meaning['symbol']]
                        _require(len(candidates)==1,'ambiguous corrected R32 symbol identity')
                        j,symbol=candidates[0]
                        _require(struct.unpack('<I',carve.va_bytes(linked,start+meaning['offset'],4))[0]==meaning['target'],
                                 'actual linked corrected R32 pointer differs')
                        known=resolved.get(j,{})
                        _require('value' not in known or known['value']==target,
                                 'corrected R32 linked symbol/address proof differs')
                        resolved[j]=dict(value=target,type=symbol.type)
                    _reference_gap(reproduced,reproduced.section(sec),
                                   _number(proof['range'][0])-_number(proof['range'][1]),0,
                                   carve,resolved,tuple(map(_number,proof['range'])))
                    _check_pins(receipt['inputs'])
                    source_path=Path(unit_dir)/classification['input']['source']
                    artifacts[str(object_path.resolve())]=_hash(data)
                    artifacts[str(source_path.resolve())]=classification['input']['assembly_sha256']
                    continue
                if neighbor.get('external_input'):
                    _require(external and neighbor is proof['neighbors'][1],
                             'external receipt is not the exact right neighbor')
                    ext_piece = next(p for p in owner['sections'][sec]['pieces']
                                     if p['name'] == neighbor['input']['piece'])
                    ext = _external_input(owner, ext_piece, sec, context, manifest, apis)
                    _require(ext is not None and ext[:2] == (neighbor['input'], neighbor['external_input']),
                             'external input reconstruction differs')
                    input_, receipt, reproduced = ext
                    object_path = Path(unit_dir)/input_['object']
                    _require(object_path.resolve().is_relative_to(Path(unit_dir).resolve()) and
                             object_path.read_bytes() == reproduced.data,
                             'actual ordinary external object differs from exact replay')
                    _require(linked is not None and carve.va_bytes(linked,
                             input_['original_range'][0], reproduced.section(sec).size) ==
                             reproduced.section_bytes(reproduced.section(sec)),
                             'linked external payload differs')
                    resolved = _resolved_targets(reproduced, input_, placed, linked)
                    _reference_gap(reproduced, reproduced.section(sec),
                                   _number(proof['range'][0])-_number(proof['range'][1]), 0,
                                   carve, resolved, tuple(map(_number, proof['range'])))
                    _check_pins(receipt['inputs'])
                    # The broad input fence is already bound by this manifest's
                    # receipt. Explicitly report the public authority as a
                    # frozen SOURCE input, plus the fresh ordinary object.
                    artifacts[receipt['authority']] = receipt['authority_sha256']
                    artifacts[str(object_path.resolve())] = _hash(reproduced.data)
                    continue
                ctx = contexts(owner, owner['id'] == tu['id'] or bool(neighbor.get('legacy_whole_c_input')))
                lyt = layouts(owner, neighbor['input']['raw_section'], ctx)
                raw, selected_object, pins = _selected_input(root, unit_dir, owner, neighbor['input'], ctx,
                                           lyt, neighbor['owners'], registry, apis)
                artifacts.update(pins)
                if neighbor.get('legacy_whole_c_input'):
                    _legacy_literal_runtime(raw, selected_object, neighbor, ctx, lyt, placed, fills,
                                            linked, apis)
                selected_section = selected_object.section(neighbor['input']['section'])
                gap_size = _number(proof['range'][1]) - _number(proof['range'][0])
                resolved = _resolved_targets(selected_object, neighbor['input'], placed, linked)
                original_gap = tuple(map(_number, proof['range']))
                if neighbor is proof['neighbors'][0]:
                    _reference_gap(selected_object, selected_section, selected_section.size,
                                   selected_section.size+gap_size, carve, resolved, original_gap,
                                   proof.get('folded_array_references', ()))
                else:
                    _reference_gap(selected_object, selected_section, -gap_size, 0, carve,
                                   resolved, original_gap, proof.get('folded_array_references', ()))
                if owner['id'] == tu['id'] and neighbor is proof['neighbors'][0]:
                    _object_gap(raw, left_section, proof, carve)
            _actual_fill(proof, placed, fills)
            if external:
                key='native_case_vector_input' if reconstructed else 'external_input'
                _check_pins(proof['neighbors'][1][key]['inputs'])
        validate_compiler_suffix_alignments.last_artifacts = artifacts
        return True
    except refusal_errors:
        return False
