"""Prepare ELF object pairs for objdiff, without calculating report scores.

Targets contain original assembly scaffolds and original TU data. Bases are fresh C
compilations with SKIP_ASM: neither fallback nor standalone assembly earns C
credit. IOP originals have no base. All binary inputs remain local.
"""
from concurrent.futures import ThreadPoolExecutor
import json
from pathlib import Path
import subprocess

import coverage_report as cov
from tu import verify_report as verification
from tu.elfinfo import Elf
from tu.toolchain import Toolchain


def run(argv, cwd):
    subprocess.run(['timeout', '-k', '5', '30', *map(str, argv)], cwd=cwd,
                   check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)


def compile_source(root, unit_dir, source, output, ccdir, assembler, flags, skip_asm):
    common = [ccdir / 'ee-gcc', f'-B{ccdir}/', '-nostdinc', '-fno-builtin']
    preprocessed, assembly = Path(str(output) + '.i'), Path(str(output) + '.s')
    run([*common, '-E', *flags, *(['-DSKIP_ASM'] if skip_asm else []), '-Iinclude', '-I.',
         f'-I{root / "include"}', f'-I{source.parent}', source, '-o', preprocessed], unit_dir)
    run([*common, '-S', *flags, preprocessed, '-o', assembly], unit_dir)
    gflag = next(flag for flag in flags if flag.startswith('-G'))
    run([assembler, '-EL', '-m5900', '-mabi=eabi', gflag, '-I.',
         '-Iinclude', '-o', output, assembly], unit_dir)
    return assembly


def build_target(unit_dir, stem, unit):
    candidates = ([f'build/expected/{stem}.o', f'build/hasm/{stem}.o']
                  if unit == 'main' else [f'build/src/{unit}/{stem}.o',
                                         f'build/scaffold/src/{unit}/{stem}.o'])
    path = next((unit_dir / p for p in candidates if (unit_dir / p).is_file()), None)
    verification.require(path is not None, f'{unit}/{stem}: missing verified build object')
    return path


def functions(path):
    return {s.name: s.size for s in Elf(path.read_bytes()).symbols
            if s.type == 2 and s.shndx not in (0, 0xfff1) and s.size}


def strip_debug(prefix, source, destination, cwd, iop_reference=False):
    # Old GCC emits ECOFF debug records that objdiff's MIPS reader cannot parse
    # reliably. Remove debug records only; verify every allocated section and
    # function identity survives unchanged. No instruction/data normalization.
    before = Elf(source.read_bytes())
    # IRX stores module-relative data addresses/relocations. Make its data
    # sections section-relative for objdiff, keeping text addresses and every
    # FUNC identity. Target-only IOP references have no base and earn zero.
    options = (['--remove-section=.rel.text', '--remove-section=.rel.data', '--strip-all',
                *[f'--change-section-address={s.name}=0' for s in before.sections
                  if s.flags & 2 and not s.flags & 4],
                *[f'--keep-symbol={name}' for name in functions(source)]] if iop_reference else [])
    run([str(prefix) + 'objcopy', '--strip-debug', *options, source, destination], cwd)
    after = Elf(destination.read_bytes())
    allocated = lambda elf: {s.name: (s.type, s.flags,
                             0 if iop_reference and not s.flags & 4 else s.addr,
                             s.size, elf.section_bytes(s))
                             for s in elf.sections if s.flags & 2}
    verification.require(allocated(before) == allocated(after) and
                         functions(source) == functions(destination),
                         f'{source}: debug stripping changed report inputs')
    identity = lambda elf: {(s.name, s.value, s.size, s.bind, s.type)
                           for s in elf.symbols if s.type == 2 and s.size}
    verification.require(identity(before) == identity(after), f'{source}: FUNC identity changed')


def prepare(root, evidence):
    build, project = root / 'build', root / 'build/objdiff'
    project.mkdir(exist_ok=True)
    objects, targets, _, _, _ = cov.tracked_scope(root)
    tc = Toolchain(root)
    prefix = Path(tc.dir('ps2dev-binutils')) / 'mips64r5900el-ps2-elf-'
    main = verification.read(build / 'main/tu-manifest.json')
    main_tus = {t['name']: t for t in main['tus']}
    overlays = {unit: verification.read(build / unit / 'compile-manifest.json')
                for unit in cov.OVERLAY_UNITS}
    expected = {}

    def prepare_object(obj):
        unit, stem = obj['unit'], Path(obj['source']).stem
        unit_dir, name = build / unit, f'{unit}/{stem}'
        destination = project / name
        destination.mkdir(parents=True, exist_ok=True)
        checked = evidence['objects'][obj['id']]
        target_text = destination / 'text.o'
        if unit == 'main':
            tu = main_tus[stem]
            contract = tu.get('contract', {})
            ccdir = Path(contract.get('cc_dir', '.'))
            assembler = Path(contract.get('assembler_path', '.'))
            flags = contract.get('flags', [])
            data_sources = [unit_dir / (piece.get('file') or
                            f'asm/main/data/{piece["piece"]}.{section[1:]}.s')
                            for section, entry in main['sections'].items()
                            for piece in entry['order'] if piece.get('tu') == stem]
            if obj['functions']:
                target_text = build_target(unit_dir, stem, unit)
            else:
                run([str(prefix) + 'as', '-EL', '-march=r5900', '-mabi=eabi', '-mgp64',
                     '-o', target_text, '/dev/null'], unit_dir)
        else:
            tus = overlays[unit]['units'][unit]['tus']
            tu = next(t for t in tus if t['name'] == stem)
            ccdir = Path(tu['compiler']['dir'])
            assembler = Path(tu['assembler']['path'])
            flags = tu['flags']
            data_sources = sorted((unit_dir / f'asm/data/{unit}').glob(stem + '.*.s'))
            if tu['kind'] == 'c':
                compile_source(root, unit_dir, unit_dir / f'scaffold/src/{unit}/{stem}.c',
                               target_text, ccdir, assembler, flags, skip_asm=False)
            else:
                target_text = build_target(unit_dir, stem, unit)
        data = []
        for source in data_sources:
            output = destination / (source.stem + '.o')
            run([str(prefix) + 'as', '-EL', '-march=r5900', '-mabi=eabi', '-mgp64',
                 '-G0', '-no-pad-sections', '-mno-fix-r5900', '-I.', '-Iinclude',
                 '-o', output, source], unit_dir)
            data.append(output)
        target = destination / 'target.o'
        stripped = destination / 'text.nodebug.o'
        strip_debug(prefix, target_text, stripped, unit_dir)
        run([str(prefix) + 'ld', '-EL', '-m', 'elf32lr5900', '-r',
             '-o', target, stripped, *data], unit_dir)
        unit_config = dict(name=name, target_path=str(target), metadata=dict(
            source_path=obj['source'], progress_categories=[unit], complete=checked['complete']))
        allowed = {f['original_symbol']: f['size'] for f in checked['functions']}
        if allowed:
            base = destination / 'base.o'
            assembly = compile_source(root, unit_dir, root / obj['source'], base,
                                      ccdir, assembler, flags, skip_asm=True)
            emitted = verification.compiler_functions(assembly.read_text())
            actual = functions(base)
            # Removing inline scaffold blocks can change scheduling/alignment.
            # Native objdiff measures that difference; the unmodified published
            # build remains the authority for exact linked sizes and bytes.
            verification.require(set(actual) == set(allowed) and set(actual) <= emitted,
                                 f'{name}: SKIP_ASM emission differs from verified C scope')
            stripped_base = destination / 'base.nodebug.o'
            strip_debug(prefix, base, stripped_base, unit_dir)
            unit_config['base_path'] = str(stripped_base)
        target_functions = functions(target)
        verification.require(len(target_functions) == len(obj['functions']) and
                             sum(target_functions.values()) == sum(f[2] for f in obj['functions']),
                             f'{name}: target function scope differs from tracked TU')
        return unit_config, dict(functions=target_functions, allowed=allowed,
                                 complete=checked['complete'])

    with ThreadPoolExecutor(max_workers=4) as pool:
        prepared = list(pool.map(prepare_object, objects))
    units = []
    for unit, checked in prepared:
        verification.require(unit['name'] not in expected, 'duplicate TU name')
        units.append(unit)
        expected[unit['name']] = checked
    iop = verification.read(root / cov.IOP_OBJECTS)
    for unit, entry in iop['units'].items():
        target = tc.game_dir() / entry['file']
        verification.require(verification.digest(target) == entry['sha256'], f'{unit}: wrong original')
        original_functions = functions(target)
        verification.require(len(original_functions) == entry['functions'], f'{unit}: original scope differs')
        destination = project / unit
        destination.mkdir(exist_ok=True)
        stripped = destination / 'target.o'
        strip_debug(prefix, target, stripped, root, iop_reference=True)
        units.append(dict(name=unit, target_path=str(stripped),
                          metadata=dict(complete=False, progress_categories=[unit])))
        expected[unit] = dict(functions=original_functions, allowed={}, complete=False)
    config = dict(build_target=False, build_base=False, units=units,
                  progress_categories=[dict(id=u, name=targets.get(u, u)) for u in cov.UNITS])
    (project / 'objdiff.json').write_text(json.dumps(config, indent=1) + '\n')
    return project, expected


def validate(report, expected):
    """Reject scope drift and scaffold credit; leave native JSON unchanged."""
    units = {u['name']: u for u in report['units']}
    verification.require(set(units) == set(expected), 'native report dropped or added units')
    for name, checked in expected.items():
        unit = units[name]
        actual = {f['name']: int(f['size']) for f in unit.get('functions', [])}
        verification.require(actual == checked['functions'], f'{name}: native function scope drift')
        for function in unit.get('functions', []):
            verification.require(not function.get('fuzzy_match_percent', 0) or
                                 function['name'] in checked['allowed'],
                                 f'{name}/{function["name"]}: scaffolding received matching credit')
        # Keep objdiff's native BSS measure. It describes uninitialized storage
        # matching, never recovered C declarations or source/link completeness.
        if not checked['allowed']:
            verification.require(not any(int(s.get('size', 0)) and s['name'] not in ('.bss', '.sbss')
                                         and s.get('fuzzy_match_percent') == 100
                                         for s in unit.get('sections', [])),
                                 f'{name}: initialized scaffold data received matching credit')
        verification.require(bool(unit.get('metadata', {}).get('complete', False)) == checked['complete'],
                             f'{name}: unexpected complete metadata')
