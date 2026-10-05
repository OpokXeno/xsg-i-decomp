#!/usr/bin/env python3
"""Verify public progress claims against the current compiled and linked build.

Run --begin before configure/ninja, then --finish after ninja gate. Only hashes,
symbol identities and counts go in verification.json; never original bytes.
This is public CI evidence, not a replacement for the recovery acceptance audit.
"""
import argparse
import hashlib
import json
import re
import subprocess
import sys
from pathlib import Path

sys.dont_write_bytecode = True
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools'))
import coverage_report as cov
from tu.elfinfo import Elf
from tu.mapcheck import parse as parse_link_map


class VerificationError(ValueError):
    pass


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def inputs(root):
    """Bind evidence to HEAD and every public build/source input, including edits."""
    def git(*args):
        return subprocess.check_output(['git', '-C', str(root), *args], timeout=30)
    paths = git('ls-files', '-z', '--cached', '--others', '--exclude-standard',
                '--', 'src', 'include', 'config', 'tools', '.github', '.gitignore',
                'configure.py', 'requirements.txt', 'README.md').split(b'\0')
    files = {p.decode(): digest(root / p.decode()) for p in sorted(set(paths)) if p}
    return dict(commit=git('rev-parse', 'HEAD').decode().strip(), files=files)


def require(ok, message):
    if not ok:
        raise VerificationError(message)


def read(path):
    try:
        return json.loads(path.read_bytes())
    except (OSError, ValueError) as error:
        raise VerificationError(f'{path}: {error}') from error


def same_inputs(root, snapshot):
    require(inputs(root) == snapshot,
            'source/build inputs changed since verification began; rebuild before reporting')


def compiler_functions(assembly):
    """Only cc1's .ent directives outside #APP count as C emission.

    INCLUDE_ASM and ACCEPTED_ASM are inserted inside #APP, so assembling the
    original scaffold cannot accidentally earn C matching credit.
    """
    emitted, app = set(), False
    for line in assembly.splitlines():
        if line.strip() == '#APP':
            app = True
        elif line.strip() == '#NO_APP':
            app = False
        elif not app:
            match = re.match(r'\s*\.ent\s+(\S+)', line)
            if match:
                emitted.add(match[1])
    return emitted


def symbol(elf, name, size, value=None):
    found = [s for s in elf.symbols if s.name == name and s.type == 2
             and s.shndx not in (0, 0xfff1) and s.size == size
             and (value is None or s.value == value)]
    require(len(found) == 1, f'{name}: missing/ambiguous FUNC symbol or wrong address/size')
    return found[0]


def original_symbol(elf, label, size, va):
    """Resolve annotation labels using the original ELF's function identity.

    Splat disambiguates repeated local names with address suffixes. Those
    labels do not rename the original symbol or the compiler's definition.
    """
    found = [s for s in elf.symbols if s.type == 2 and s.size == size
             and s.value == va and s.shndx not in (0, 0xfff1)]
    named = [s for s in found if s.name == label]
    if named:
        found = named
    require(len(found) == 1, f'{label}: missing/ambiguous original FUNC identity')
    return found[0]


def function_bytes(elf, sym):
    section = elf.sections[sym.shndx]
    offset = section.offset + sym.value - section.addr
    require(section.type != 8 and 0 <= offset <= len(elf.data) - sym.size,
            f'{sym.name}: invalid function extent')
    return elf.data[offset:offset + sym.size]


def build_object(root, build, obj):
    stem = Path(obj['source']).stem
    unit_dir = build / obj['unit']
    if obj['unit'] == 'main':
        path = unit_dir / 'build/c' / (stem + '.o')
        asm = Path(str(path) + '.s')
    else:
        path = unit_dir / 'build/src' / obj['unit'] / (stem + '.o')
        if not path.is_file():
            # Public links can retain the skeleton object's destination while
            # their cc edge compiles the published source into that destination.
            path = unit_dir / 'build/scaffold/src' / obj['unit'] / (stem + '.o')
        asm = path.with_suffix('.s')
    require(path.is_file() and asm.is_file(),
            f'{obj["id"]}: no compiled published object/cc1 assembly; run configure and ninja gate')
    return path, asm


def data_sizes(root, objects):
    """Original TU data remains in the denominator even when it is scaffolding.
    """
    sizes = {}
    main = read(root / cov.MAIN_OBJECTS)
    for tu in main['tus']:
        sizes[tu['id']] = sum(s.get('size', 0) for s in tu.get('sections', {}).values())
    for unit in cov.OVERLAY_UNITS:
        layout = read(root / 'config/objects' / f'{unit}.layout.json')
        for runs in layout['families'].values():
            for name, start, end in runs:
                sizes[name] = sizes.get(name, 0) + int(end, 16) - int(start, 16)
    return {o['id']: sizes.get(o['id'], sizes.get(
        f'{o["unit"]}/{Path(o["source"]).stem}' if o['source'] else '', 0))
        for o in objects}


def linked_object_complete(root, build, obj, placed, derived, fills, original):
    """Objdiff's complete flag means the entire original object is recompiled.

    Check actual linker inputs, including initialized data and BSS. Partial
    data carves do not complete a TU while any original storage comes from an
    assembly input. Linker alignment fill is not an unrecovered data object.
    """
    unit, stem = obj['unit'], Path(obj['source']).stem
    compiled, _ = build_object(root, build, obj)
    owned = {str(compiled.relative_to(build / unit))}
    while True:
        additions = {output for output, input_path in derived.items() if input_path in owned}
        if additions <= owned:
            break
        owned.update(additions)
    if unit == 'main':
        manifest = read(build / unit / 'tu-manifest.carved.json')
        tu = next(t for t in manifest['tus'] if t['name'] == stem)
        spans = [(int(p['start'], 16), int(p['end'], 16))
                 for section in tu['sections'].values() for p in section.get('pieces', [])]
    else:
        layout = read(root / 'config/objects' / f'{unit}.layout.json')
        spans = [(int(lo, 16), int(hi, 16)) for rows in layout['families'].values()
                 for name, lo, hi in rows if name == f'{unit}/{stem}']
    for lo, hi in spans:
        covered = []
        for (input_path, section), (start, size) in placed.items():
            if start < hi and lo < start + size:
                if input_path not in owned:
                    return False
                covered.append((max(lo, start), min(hi, start + size)))
        for start, end in fills:
            a, b = max(lo, start), min(hi, end)
            if a < b and not any(s.type == 1 and s.size and s.value < b
                                 and a < s.value + s.size for s in original.symbols):
                covered.append((a, b))
        cursor = lo
        for start, end in sorted(covered):
            if start > cursor:
                return False
            cursor = max(cursor, end)
        if cursor < hi:
            return False
    return True


def finish(root, build, snapshot):
    same_inputs(root, snapshot)
    started = (build / 'verification-inputs.json').stat().st_mtime_ns
    def fresh(path):
        require(path.stat().st_mtime_ns >= started,
                f'{path}: predates this verification; use a fresh build checkout')
        return digest(path)
    gate = read(build / 'gate.json')
    fresh(build / 'gate.json')
    originals = read(root / cov.ORIGINALS)['units']
    require(gate.get('result') == 'pass', 'whole-file gate did not pass')
    require(set(gate.get('targets', {})) == set(cov.EE_UNITS), 'gate must cover all six EE files')
    objects, _, _, _, _ = cov.tracked_scope(root)
    parser = cov.import_tu_edit(root)
    results, artifacts, problems = {}, {}, []
    for unit in cov.EE_UNITS:
        unit_dir = build / unit
        original_path = unit_dir / 'orig' / originals[unit]['file']
        image_path = unit_dir / 'build' / f'{unit}.bin'
        linked_path = unit_dir / 'build' / f'{unit}.rom.elf'
        expected = originals[unit]['sha256']
        require(digest(original_path) == digest(image_path) == expected,
                f'{unit}: actual current original/rebuilt whole-file hashes differ')
        status = read(unit_dir / 'status.json')
        require(status.get('result') == 'pass', f'{unit}: unit checks failed')
        original = Elf(original_path.read_bytes())
        linked = Elf(linked_path.read_bytes())
        map_path = unit_dir / 'build' / f'{unit}.rom.elf.map'
        ninja_path = unit_dir / 'build.ninja'
        map_text = map_path.read_text()
        placed, _ = parse_link_map(map_text)
        fills = [(int(a, 16), int(a, 16) + int(n, 16)) for a, n in
                 re.findall(r'^ \*fill\*\s+0x([0-9a-f]+)\s+0x([0-9a-f]+)\b', map_text, re.M)]
        # These existing build transformations only place/split compiler data;
        # they never import original storage into a compiled object.
        # An edge may list implicit outputs (`build X | X.report.json: ...`).
        derived = dict(re.findall(r'^build (\S+)(?: \| [^:]+)?: (?:carvesplit|allocate_bss) (\S+)(?:\s|$)',
                                  ninja_path.read_text(), re.M))
        artifacts[str(map_path.relative_to(root))] = fresh(map_path)
        artifacts[str(ninja_path.relative_to(root))] = digest(ninja_path)
        if unit == 'main':
            path = unit_dir / 'tu-manifest.carved.json'
            artifacts[str(path.relative_to(root))] = digest(path)
        artifacts[str(original_path.relative_to(root))] = digest(original_path)
        for path in (image_path, linked_path, unit_dir / 'status.json'):
            artifacts[str(path.relative_to(root))] = fresh(path)
        for obj in (o for o in objects if o['unit'] == unit):
            claimed = cov.count_object(root, obj, parser, problems)
            verified = []
            if claimed:
                object_path, assembly_path = build_object(root, build, obj)
                compiled = Elf(object_path.read_bytes())
                emitted = compiler_functions(assembly_path.read_text())
                for path in (object_path, assembly_path):
                    artifacts[str(path.relative_to(root))] = fresh(path)
                for name, va, size in obj['functions']:
                    if va not in claimed:
                        continue
                    category = claimed[va][1]
                    # Handwritten accepted ASM is reported separately as recovery,
                    # but never earns decompiled C matching/complete credit.
                    if category == 'exact_asm':
                        continue
                    target = original_symbol(original, name, size, va)
                    require(target.name in emitted,
                            f'{obj["id"]}/{name}: no compiler-emitted C function')
                    current = symbol(linked, target.name, size, va)
                    compiled_sym = symbol(compiled, target.name, size)
                    require(target.bind == current.bind == compiled_sym.bind,
                            f'{obj["id"]}/{name}: symbol binding differs')
                    a, b = function_bytes(original, target), function_bytes(linked, current)
                    require(a == b, f'{obj["id"]}/{name}: linked function bytes differ')
                    verified.append(dict(name=name, original_symbol=target.name,
                                         va=va, size=size, category=category,
                                         sha256=hashlib.sha256(b).hexdigest()))
            complete = (bool(obj['functions']) and len(verified) == len(obj['functions'])
                        and linked_object_complete(root, build, obj, placed, derived, fills, original))
            results[obj['id']] = dict(functions=verified, complete=complete)
    require(not problems, f'source/object scope inconsistencies: {problems[:3]}')
    sizes = data_sizes(root, objects)
    for obj in objects:
        entry = results[obj['id']]
        entry['total_data'] = sizes[obj['id']]
    same_inputs(root, snapshot)
    artifacts[str((build / 'gate.json').relative_to(root))] = digest(build / 'gate.json')
    return dict(schema='verified-progress/1', inputs=snapshot, artifacts=artifacts,
                objects=results, note='IOP has no build and earns zero credit. Complete means '
                'all original TU code and storage are recompiled and linked from C; '
                'assembly scaffolding never completes a TU.')


def load_verified(root, build):
    evidence = read(build / 'verification.json')
    require(evidence.get('schema') == 'verified-progress/1', 'wrong verification schema')
    same_inputs(root, evidence['inputs'])
    for name, want in evidence['artifacts'].items():
        require(digest(root / name) == want, f'{name}: build evidence changed; re-verify')
    return evidence


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--root', type=Path, default=ROOT)
    mode = ap.add_mutually_exclusive_group(required=True)
    mode.add_argument('--begin', action='store_true')
    mode.add_argument('--finish', action='store_true')
    a = ap.parse_args()
    root, build = a.root.resolve(), a.root.resolve() / 'build'
    try:
        build.mkdir(exist_ok=True)
        if a.begin:
            # Invalidate old outputs before attempting a fresh verification.
            for name in ('verification.json', 'report.json'):
                (build / name).unlink(missing_ok=True)
            result = inputs(root)
            output = build / 'verification-inputs.json'
        else:
            result = finish(root, build, read(build / 'verification-inputs.json'))
            output = build / 'verification.json'
        output.write_text(json.dumps(result, indent=1) + '\n')
        print(f'verify_report: {output}')
        return 0
    except (VerificationError, OSError, subprocess.SubprocessError) as error:
        print(f'verify_report: {error}', file=sys.stderr)
        return 1


if __name__ == '__main__':
    sys.exit(main())
