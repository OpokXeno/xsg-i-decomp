#!/usr/bin/env python3
"""Write the objdiff project (objdiff.json + per-TU objects) from the build.

Run after `./configure.py && ninja gate` and `tools/iop_build.py`, then
`objdiff-cli report generate --project build/objdiff`.  No verification layer:
the gate already compared every image with the original; this only pairs, per
translation unit, the original object (target) with the compiled C (base).

  target  EE: the TU's original code and data as splat cut them (the build's
          all-asm `expected`/scaffold object plus the TU's splat data files);
          IOP: the original IRX bytes of each source group (tu/iop_objdiff.py).
  base    the object the build compiled from the published C source.  Functions
          still provided by INCLUDE_ASM or ACCEPTED_ASM are renamed so they never
          pair with the target: assembly earns no C credit.
"""
import argparse
import os
import json
import re
import struct
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

sys.dont_write_bytecode = True
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools'))
from tu.elfinfo import Elf  # noqa: E402
from tu.toolchain import Toolchain, tu_assembler_options  # noqa: E402

EE_UNITS = ('main', 'ov01', 'ov02', 'ov10', 'ov11', 'ov12')
IOP_UNITS = ('iop_ssd', 'iop_rssd')
ASM_MACRO = re.compile(r'^\s*(?:INCLUDE_ASM|ACCEPTED_ASM)\s*\(\s*"[^"]*"\s*,\s*(\w+)\s*\)', re.M)
SCAFFOLD_SUFFIX = '__scaffold_asm'


def run(argv, cwd):
    subprocess.run([str(a) for a in argv], cwd=cwd, check=True, timeout=120)


def read(path):
    return json.loads(Path(path).read_text())


class Tools:
    def __init__(self, root):
        self.root = root
        self.linked = {}
        self.ranges = image_ranges(root)
        prefix = Path(Toolchain(root).dir('ps2dev-binutils')) / 'mips64r5900el-ps2-elf-'
        self.as_ = str(prefix) + 'as'
        self.ld = str(prefix) + 'ld'
        self.objcopy = str(prefix) + 'objcopy'
        self.nm = str(prefix) + 'nm'
        self.objdump = str(prefix) + 'objdump'

    def addresses(self, unit):
        if unit not in self.linked:
            self.linked[unit] = linked_addresses(self.root, unit)
        return self.linked[unit]

    def assemble_data(self, source, output, cwd):
        run([self.as_, '-EL', '-march=r5900', '-mabi=eabi', '-mgp64', '-G0', '-no-pad-sections',
             '-mno-fix-r5900', '-I.', '-Iinclude', '-o', output, source], cwd)

    def symbols(self, obj):
        out = subprocess.run([self.nm, str(obj)], capture_output=True, text=True, check=True).stdout
        return {line.split()[-1] for line in out.splitlines() if line.strip()}

    def defined(self, obj):
        out = subprocess.run([self.nm, '--defined-only', str(obj)], capture_output=True,
                             text=True, check=True).stdout
        return {line.split()[-1] for line in out.splitlines() if len(line.split()) == 3}

    def empty_objects(self, obj):
        """Defined zero-size data symbols: objdiff cannot pair them."""
        out = subprocess.run([self.objdump, '-t', str(obj)], capture_output=True, text=True,
                             check=True).stdout
        names = set()
        for line in out.splitlines():
            fields = line.split()
            if len(fields) >= 6 and 'O' in fields[1:-3] and fields[-3] != '*UND*' and int(fields[-2], 16) == 0:
                names.add(fields[-1])
        return names

    def target(self, parts, output, cwd, names=None, strip=()):
        """One relocatable object of the original pieces, without debug records.

        `names` renames splat labels to the names the source gives the same
        address, so relocations name the same symbol on both sides."""
        joined = output.with_suffix('.joined.o')
        # _gp=0 keeps the inputs' GP0, so GPREL16 addends stay as assembled.
        run([self.ld, '-EL', '-m', 'elf32lr5900', '-r', '--defsym', '_gp=0', '-o', joined, *parts], cwd)
        options = ['--strip-symbol=_gp', *(f'--strip-symbol={name}' for name in strip)]
        present = self.symbols(joined)
        chosen = {}
        for old, new in sorted((names or {}).items()):
            # Storage aliases map several labels to one name; keep the first present.
            if old in present and new not in present and new not in chosen.values():
                chosen[old] = new
        if chosen:
            mapping = output.with_suffix('.names')
            mapping.write_text(''.join(f'{old} {new}\n' for old, new in chosen.items()))
            options.append(f'--redefine-syms={mapping}')
        run([self.objcopy, '--strip-debug', *options, joined, output], cwd)
        address_constants(output, self.ranges)

    def base(self, compiled, scaffold_functions, output, cwd, target=None, unit=None):
        """The compiled C object, debug-stripped, scaffold functions renamed.

        Zero-size objects (`int table[] = {}`) are renamed too: they hold no
        bytes, and objdiff refuses to pair a target symbol with one."""
        present = self.defined(compiled)
        scaffold = set(scaffold_functions) | assembly_functions(compiled)
        renames = [f'--redefine-sym={n}={n}{SCAFFOLD_SUFFIX}' for n in sorted(scaffold) if n in present]
        renames += [f'--redefine-sym={n}={n}.empty' for n in sorted(self.empty_objects(compiled))]
        if target is not None:
            renames += splat_function_names(target, compiled) + splat_static_names(target, compiled)
        run([self.objcopy, '--strip-debug', *renames, compiled, output], cwd)
        if target is None:
            return
        # Names reached from code first; a table named that way names its own
        # entries on the next pass; unnamed spans last.
        passes = [lambda: anonymous_labels(target, output)] * 3 + [lambda: (span_labels(target, output), {})]
        for labels in passes:
            added, local = labels()
            if not added and not local:
                continue
            named = output.with_suffix('.named.o')
            run([self.objcopy,
                 *[f'--add-symbol={name}={section}:{offset:#x},local' + (',object' if size else '')
                   for name, section, offset, size in added],
                 *[f'--redefine-sym={old}={new}' for old, new in local.items()], output, named], cwd)
            named.replace(output)
            symbol_sizes(output, {name: size for name, _, _, size in added if size})
        named_words(output)
        data_words(target, output, self.addresses(unit))
        gp_relocation_kinds(target, output)
        data_padding(target, output)
        interior = interior_labels(target, output)
        if interior:
            stripped = target.with_suffix('.stripped.o')
            run([self.objcopy, *[f'--strip-symbol={name}' for name in interior], target, stripped], cwd)
            stripped.replace(target)


R_MIPS_32, R_MIPS_HI16, R_MIPS_LO16, R_MIPS_GPREL16, R_MIPS_LITERAL = 2, 5, 6, 7, 8
ADDRESS_LABEL = re.compile(r'D_([0-9A-F]+)$')
LITERAL_SECTIONS = ('.lit4', '.lit8')


def text_relocations(elf):
    """{offset: (rel entry file offset, type, symbol)} of .rel.text."""
    text = elf.section('.text').index
    found = {}
    for section in elf.sections:
        if section.type == 9 and section.info == text:  # SHT_REL
            for at in range(section.offset, section.offset + section.size, 8):
                offset, info = struct.unpack_from('<II', elf.data, at)
                found[offset] = (at, info & 0xff, elf.symbols[info >> 8])
    return found


def functions(elf):
    text = elf.section('.text').index
    return {s.name: s for s in elf.symbols if s.type == 2 and s.shndx == text}


def image_ranges(root):
    """Address ranges of every allocated section of the six linked EE images."""
    ranges = []
    for unit in EE_UNITS:
        elf = Elf((root / 'build' / unit / 'build' / f'{unit}.elf').read_bytes())
        ranges += [(s.addr, s.addr + s.size) for s in elf.sections if s.flags & 2 and s.size]  # SHF_ALLOC
    return ranges


def address_constants(path, ranges):
    """Resolve the target's references to splat labels of no image address.

    splat names every %hi/%lo pair and every `.word` that looks like an address
    `D_<address>`, also when the value is a constant or a hardware address (the
    0x70000000 scratchpad, 0x0000FFFF, 0x8000): the original holds it as a
    constant, and so does the compiled C.  A label whose address lies in no
    section of the six images is such a constant: apply its relocation as a
    link would, leaving the value in place and dropping the relocation."""
    elf = Elf(path.read_bytes())
    data = bytearray(elf.data)
    for table in [s for s in elf.sections if s.type == 9]:  # SHT_REL
        section = elf.sections[table.info]
        if section.type == 8:
            continue
        resolved, pending = set(), []
        for at in range(table.offset, table.offset + table.size, 8):
            offset, info = struct.unpack_from('<II', data, at)
            kind, symbol = info & 0xff, elf.symbols[info >> 8]
            label = ADDRESS_LABEL.match(symbol.name)
            if kind not in (R_MIPS_32, R_MIPS_HI16, R_MIPS_LO16) or symbol.shndx != 0 or not label:
                continue
            address = int(label.group(1), 16)
            if any(start <= address < end for start, end in ranges):
                continue
            where = section.offset + offset
            word = struct.unpack_from('<I', data, where)[0]
            if kind == R_MIPS_32:
                struct.pack_into('<I', data, where, (word + address) & 0xffffffff)
            elif kind == R_MIPS_HI16:
                pending.append((where, at, address))
                continue
            else:
                value = address + struct.unpack_from('<h', data, where)[0]
                for hi_where, hi_at, hi_address in pending:
                    if hi_address == address:
                        hi = struct.unpack_from('<I', data, hi_where)[0]
                        struct.pack_into('<I', data, hi_where,
                                         (hi & 0xffff0000) | (((value + 0x8000) >> 16) & 0xffff))
                        resolved.add(hi_at)
                pending = [p for p in pending if p[2] != address]
                struct.pack_into('<I', data, where, (word & 0xffff0000) | (value & 0xffff))
            resolved.add(at)
        if resolved:
            kept = [bytes(data[at:at + 8]) for at in range(table.offset, table.offset + table.size, 8)
                    if at not in resolved]
            data[table.offset:table.offset + 8 * len(kept)] = b''.join(kept)
            struct.pack_into('<I', data, elf.shoff + table.index * elf.shentsize + 20, 8 * len(kept))
    path.write_bytes(bytes(data))


def gp_relocation_kinds(target_path, base_path):
    """Give the target's gp-relative loads the relocation type of the base.

    The original executable has no relocations: whether its assembler wrote
    R_MIPS_GPREL16 (`%gp_rel(sym)`, a named small-data object) or
    R_MIPS_LITERAL (a compiler pool entry) at an instruction is not in its
    bytes.  splat always writes the first.  Where the base has the other at the
    same instruction of the same function, the target takes the base's type."""
    target, base = Elf(target_path.read_bytes()), Elf(base_path.read_bytes())
    target_relocations, base_relocations = text_relocations(target), text_relocations(base)
    target_functions = functions(target)
    data = bytearray(target.data)
    for name, function in functions(base).items():
        twin = target_functions.get(name)
        if twin is None or twin.size != function.size:
            continue
        for offset in range(function.value, function.value + function.size, 4):
            mine = base_relocations.get(offset)
            theirs = target_relocations.get(twin.value + offset - function.value)
            kinds = {R_MIPS_GPREL16, R_MIPS_LITERAL}
            if mine and theirs and mine[1] in kinds and theirs[1] in kinds and mine[1] != theirs[1]:
                at, _, symbol = theirs
                struct.pack_into('<I', data, at + 4, (symbol.index << 8) | mine[1])
    target_path.write_bytes(bytes(data))


def relocations(elf):
    """{section index: {offset: (rel entry file offset, type, symbol)}}."""
    found = {}
    for table in elf.sections:
        if table.type == 9:  # SHT_REL
            entries = found.setdefault(table.info, {})
            for at in range(table.offset, table.offset + table.size, 8):
                offset, info = struct.unpack_from('<II', elf.data, at)
                entries[offset] = (at, info & 0xff, elf.symbols[info >> 8])
    return found


def symbol_sizes(path, sizes):
    elf = Elf(path.read_bytes())
    symtab = next(s for s in elf.sections if s.type == 2)
    data = bytearray(elf.data)
    for symbol in elf.symbols:
        if symbol.name in sizes:
            struct.pack_into('<I', data, symtab.offset + 16 * symbol.index + 8, sizes[symbol.name])
    path.write_bytes(bytes(data))


def masked(elf, section, start, size, relocated):
    """Bytes of an object with its 32-bit relocation fields zeroed."""
    body = bytearray(elf.section_bytes(section)[start:start + size])
    for offset, (_, kind, _) in relocated.items():
        if kind == R_MIPS_32 and start <= offset < start + size:
            body[offset - start:offset - start + 4] = bytes(4)
    return bytes(body)


def named_words(path):
    """Point the base's section-relative data words at the symbol they reach.

    A `.word` of the compiled C to a static object or a jump-table case is
    `R_MIPS_32 .text+0x6b8`; objdiff compares relocation symbols by name and
    does not resolve that address.  Where exactly one named symbol starts at
    the address, the word is relocated against it with addend zero: the same
    address, now under the name the target also uses."""
    elf = Elf(path.read_bytes())
    data = bytearray(elf.data)
    at_address = {}
    for symbol in elf.symbols:
        if (symbol.shndx and symbol.shndx < len(elf.sections) and symbol.type in (0, 1, 2) and symbol.name
                and not symbol.name.endswith('.NON_MATCHING') and not symbol.name.startswith('$')
                and symbol.name not in ('gcc2_compiled.', '__gnu_compiled_c')):
            at_address.setdefault((symbol.shndx, symbol.value), []).append(symbol)
    changed = False
    for table in [s for s in elf.sections if s.type == 9]:
        section = elf.sections[table.info]
        if section.name == '.text' or section.type == 8:
            continue
        for at in range(table.offset, table.offset + table.size, 8):
            offset, info = struct.unpack_from('<II', data, at)
            symbol = elf.symbols[info >> 8]
            if info & 0xff != R_MIPS_32 or symbol.type != 3:
                continue
            where = section.offset + offset
            address = struct.unpack_from('<I', data, where)[0]
            names = at_address.get((symbol.shndx, address), [])
            if len(names) != 1:
                continue
            struct.pack_into('<I', data, where, 0)
            struct.pack_into('<I', data, at + 4, (names[0].index << 8) | R_MIPS_32)
            changed = True
    if changed:
        path.write_bytes(bytes(data))


def same_extent(target, section, base, twin):
    """Whether two data sections hold the same layout: equal sizes, or the
    target longer only by the zero fill splat keeps up to the next file."""
    extra = section.size - twin.size
    if not 0 <= extra < 16:
        return False
    return extra == 0 or section.type == 8 or not any(target.section_bytes(section)[twin.size:])


def span_labels(target_path, base_path):
    """Label the base's unnamed spans after the target, offset for offset.

    splat labels every address the original references or pads, so a
    target data section also names alignment fill and anonymous spans the
    compiled C leaves unnamed.  Where a data section has the same size on
    both sides, a target label whose span holds the same bytes in the base
    and overlaps no base object is given to the base at the same offset."""
    target, base = Elf(Path(target_path).read_bytes()), Elf(Path(base_path).read_bytes())
    target_relocations, base_relocations = relocations(target), relocations(base)
    by_name = {s.name: s for s in base.sections}
    used = {s.name for s in base.symbols}
    added = []
    for section in target.sections:
        twin = by_name.get(section.name)
        if (twin is None or section.name == '.text' or not section.name.startswith('.')
                or section.flags & 2 == 0 or not same_extent(target, section, base, twin)):
            continue
        spans = sorted((s.value, s.value + max(s.size, 1)) for s in base.symbols
                       if s.shndx == twin.index and s.type in (1, 2))
        for label in target.symbols:
            if (label.shndx != section.index or label.type != 1 or not label.size or label.name in used
                    or label.name.endswith('.NON_MATCHING')):
                continue
            start, end = label.value, label.value + label.size
            if end > twin.size or any(a < end and start < b for a, b in spans):
                continue
            if section.type != 8 and (masked(target, section, start, label.size, target_relocations.get(section.index, {}))
                                      != masked(base, twin, start, label.size, base_relocations.get(twin.index, {}))):
                continue
            added.append((label.name, twin.name, start, label.size))
            used.add(label.name)
            spans.append((start, end))
    return added


def linked_addresses(root, unit):
    """{name: address} of the symbols with one address in a linked image."""
    elf = Elf((root / 'build' / unit / 'build' / f'{unit}.elf').read_bytes())
    seen = {}
    for symbol in elf.symbols:
        if symbol.shndx and symbol.name:
            seen.setdefault(symbol.name, set()).add(symbol.value)
    return {name: values.pop() for name, values in seen.items() if len(values) == 1}


def data_words(target_path, base_path, addresses):
    """Drop the pointers splat guessed in data the compiled C holds as values.

    splat turns every data word that looks like an address into `.word sym`.
    Where the data object of the same name in the base has no relocation at
    that word and holds exactly the address the linked image gives `sym`, the
    original word was that value, not a reference: the target keeps the value
    and loses the relocation."""
    target, base = Elf(target_path.read_bytes()), Elf(Path(base_path).read_bytes())
    base_objects = {(base.sections[s.shndx].name, s.name): s for s in base.symbols
                    if s.type == 1 and 0 < s.shndx < len(base.sections) and s.size}
    base_relocations = relocations(base)
    data = bytearray(target.data)
    drop = set()
    for symbol in target.symbols:
        if symbol.type != 1 or not 0 < symbol.shndx < len(target.sections) or not symbol.size:
            continue
        section = target.sections[symbol.shndx]
        twin = base_objects.get((section.name, symbol.name))
        if twin is None or twin.size != symbol.size or section.name == '.text':
            continue
        theirs = base_relocations.get(twin.shndx, {})
        table = next((t for t in target.sections if t.type == 9 and t.info == section.index), None)
        if table is None:
            continue
        for at in range(table.offset, table.offset + table.size, 8):
            offset, info = struct.unpack_from('<II', data, at)
            if info & 0xff != R_MIPS_32 or not symbol.value <= offset < symbol.value + symbol.size:
                continue
            position = twin.value + offset - symbol.value
            if position in theirs:
                continue
            name = target.symbols[info >> 8].name
            label = ADDRESS_LABEL.match(name)
            address = addresses.get(name, int(label.group(1), 16) if label else None)
            if address is None:
                continue
            where = section.offset + offset
            value = (struct.unpack_from('<I', data, where)[0] + address) & 0xffffffff
            if struct.unpack_from('<I', base.section_bytes(base.sections[twin.shndx]), position)[0] != value:
                continue
            struct.pack_into('<I', data, where, value)
            drop.add(at)
    if drop:
        for table in [t for t in target.sections if t.type == 9]:
            kept = [bytes(data[at:at + 8]) for at in range(table.offset, table.offset + table.size, 8)
                    if at not in drop]
            data[table.offset:table.offset + 8 * len(kept)] = b''.join(kept)
            struct.pack_into('<I', data, target.shoff + table.index * target.shentsize + 20, 8 * len(kept))
        target_path.write_bytes(bytes(data))


def interior_labels(target_path, base_path):
    """Target labels splat placed inside an object of the compiled C.

    splat labels every address something points at, also inside an array
    (`D_0041118C` within `_srsChrEfTbl2`).  The original object has no such
    symbol and objdiff counts it unmatched; so is a label on the zero fill
    after the end of the C section.  A label is dropped when no target
    relocation names it, its section has the same layout on both sides
    (`same_extent`) and its span lies inside one base object at the same
    offset, where those bytes are still compared, or in that zero fill."""
    target, base = Elf(target_path.read_bytes()), Elf(Path(base_path).read_bytes())
    referenced = {entry[2].name for table in relocations(target).values() for entry in table.values()}
    base_names = {s.name for s in base.symbols}
    by_name = {s.name: s for s in base.sections}
    drop = []
    for label in target.symbols:
        if (label.type != 1 or not 0 < label.shndx < len(target.sections) or label.name in referenced
                or label.name in base_names or label.name.endswith('.NON_MATCHING')):
            continue
        section = target.sections[label.shndx]
        twin = by_name.get(section.name)
        if twin is None or section.name == '.text' or not same_extent(target, section, base, twin):
            continue
        start, end = label.value, label.value + max(label.size, 1)
        if start >= twin.size or any(s.shndx == twin.index and s.type == 1 and s.value <= start
                                     and end <= s.value + s.size and (s.value, s.size) != (label.value, label.size)
                                     for s in base.symbols):
            drop.append(label.name)
            drop.append(label.name + '.NON_MATCHING')
    return drop


def same_place(base, section, position, target, label):
    """Whether a base .text address and a target code label lie at the same
    offset of functions of the same name and size."""
    def containing(elf, index, address):
        return next((s for s in elf.symbols if s.type == 2 and s.shndx == index
                     and s.value <= address < s.value + s.size), None)
    mine, theirs = containing(base, section, position), containing(target, label.shndx, label.value)
    return (mine is not None and theirs is not None and mine.name == theirs.name and mine.size == theirs.size
            and position - mine.value == label.value - theirs.value)


def anonymous_labels(target_path, base_path):
    """Name what the compiled C references without a name after the target.

    The compiler references its own pools, strings and jump tables through a
    section (`.rodata+0x40`, `.lit4+0x74`) or a local `$L` label; splat named
    the same objects (`D_...`, `jtbl_...`).  Wherever a function or data
    object exists on both sides, each base relocation is set against the
    target's relocation at the same position.  When the base one names no
    object, the address it reaches is labeled with the target's name and size
    (or the `$L` label is renamed), provided the object there holds the same
    bytes, relocation fields aside.  objdiff still compares everything; this
    only lets the same object carry the same name on both sides."""
    target, base = Elf(Path(target_path).read_bytes()), Elf(Path(base_path).read_bytes())
    reginfo = next((s for s in base.sections if s.name == '.reginfo'), None)
    gp0 = struct.unpack_from('<i', base.data, reginfo.offset + 20)[0] if reginfo else 0
    target_relocations, base_relocations = relocations(target), relocations(base)
    regions = []
    target_functions = functions(target)
    for name, function in functions(base).items():
        twin = target_functions.get(name)
        if twin is not None and twin.size == function.size:
            regions.append((function, twin))
    target_objects = {(target.sections[s.shndx].name, s.name): s for s in target.symbols
                      if s.type == 1 and 0 < s.shndx < len(target.sections)}
    for symbol in base.symbols:
        if symbol.type == 1 and 0 < symbol.shndx < len(base.sections) and symbol.size:
            twin = target_objects.get((base.sections[symbol.shndx].name, symbol.name))
            # A shorter base object stands for the same one: splat counted the fill.
            if twin is not None and 0 <= twin.size - symbol.size <= 15:
                regions.append((symbol, twin))
    occupied = {(s.shndx, s.value) for s in base.symbols if s.type in (1, 2) and s.shndx}
    used = {s.name for s in base.symbols}
    added, local, taken = [], {}, set()
    for mine, theirs in regions:
        base_section = base.sections[mine.shndx]
        target_section = target.sections[theirs.shndx]
        base_entries = base_relocations.get(mine.shndx, {})
        target_entries = target_relocations.get(theirs.shndx, {})
        high = {}
        for offset in sorted(o for o in base_entries if mine.value <= o < mine.value + mine.size):
            _, kind, symbol = base_entries[offset]
            word = struct.unpack_from('<I', base.section_bytes(base_section), offset)[0]
            if kind == R_MIPS_HI16:
                high[symbol.index] = (word & 0xffff) << 16
                continue
            match = target_entries.get(theirs.value + offset - mine.value)
            if match is None:
                continue
            label = match[2]
            if not 0 < label.shndx < len(target.sections) or label.name in used:
                continue
            target_word = struct.unpack_from('<I', target.section_bytes(target_section),
                                             theirs.value + offset - mine.value)[0]
            if symbol.type == 3:  # STT_SECTION: compute the address the compiler reaches
                if kind == R_MIPS_32 and match[1] == R_MIPS_32 and target_word == 0:
                    position = word
                elif kind == R_MIPS_LO16 and match[1] == R_MIPS_LO16 and target_word & 0xffff == 0:
                    if symbol.index not in high:
                        continue
                    position = high[symbol.index] + struct.unpack('<h', struct.pack('<H', word & 0xffff))[0]
                elif kind in (R_MIPS_GPREL16, R_MIPS_LITERAL) and match[1] in (R_MIPS_GPREL16, R_MIPS_LITERAL) \
                        and target_word & 0xffff == 0:
                    position = struct.unpack('<h', struct.pack('<H', word & 0xffff))[0] + gp0
                else:
                    continue
                destination = symbol.shndx
            elif symbol.name.startswith('$L') and symbol.bind == 0 and 0 < symbol.shndx < len(base.sections):
                position, destination = symbol.value, symbol.shndx
            else:
                continue
            section = base.sections[destination]
            if section.name != target.sections[label.shndx].name:
                continue
            size = label.size
            if section.name == '.text':
                # A code label (a jump-table case): the same offset in the same function.
                if size or not same_place(base, destination, position, target, label):
                    continue
            elif size == 0:
                continue
            elif section.type != 8:  # SHT_NOBITS holds no bytes to compare
                original = masked(target, target.sections[label.shndx], label.value, size,
                                  target_relocations.get(label.shndx, {}))
                compiled = base_relocations.get(destination, {})
                foreign = any(position <= o < position + size
                              and label.value + o - position not in target_relocations.get(label.shndx, {})
                              for o in compiled)
                if foreign or masked(base, section, position, size, compiled) != original:
                    # splat sizes a label up to the next one, alignment fill included:
                    # a string keeps its terminator, a table its last relocated
                    # word, at most 15 zero bytes are fill.
                    body = original.rstrip(b'\0')
                    words = [o + 4 - label.value for o in target_relocations.get(label.shndx, {})
                             if label.value <= o < label.value + size]
                    size = max(len(body) + 1 if body else 0, *words, 0)
                    if (not size or len(original) - size > 15
                            or masked(base, section, position, size, compiled) != original[:size]):
                        continue
            if symbol.type == 3:
                if (destination, position) in occupied or (destination, position) in taken:
                    continue
                taken.add((destination, position))
                added.append((label.name, section.name, position, size))
            elif local.setdefault(symbol.name, label.name) != label.name:
                continue
            used.add(label.name)
    return added, local


def splat_function_names(target_path, base_path):
    """Give a C function the splat name the target uses for it.

    splat names a function it found no symbol for `<name>_<address>`; where
    splat also left a reference to that name, the target cannot take the
    source name (`_DumpHead_00A46508` for `_DumpHead`).  The base function at
    the same .text offset then takes the splat name."""
    target, base = Elf(Path(target_path).read_bytes()), Elf(Path(base_path).read_bytes())
    target_functions = functions(target)
    by_offset = {f.value: name for name, f in target_functions.items()}
    renames = []
    for name, function in functions(base).items():
        twin = by_offset.get(function.value)
        if (name not in target_functions and twin and re.fullmatch(re.escape(name) + r'_[0-9A-F]{8}', twin)
                and target_functions[twin].size == function.size):
            renames.append(f'--redefine-sym={name}={twin}')
    return renames


def splat_static_names(target_path, base_path):
    """Give a function-scope static the name splat gave it (`TestEnv.0` ->
    `TestEnv_0_004A7B60`): same section, splat's spelling plus an address."""
    target, base = Elf(Path(target_path).read_bytes()), Elf(Path(base_path).read_bytes())
    names = {}
    for symbol in target.symbols:
        if symbol.type == 1 and 0 < symbol.shndx < len(target.sections):
            names.setdefault((target.sections[symbol.shndx].name, symbol.name), symbol)
    renames = []
    for symbol in base.symbols:
        if symbol.type != 1 or '.' not in symbol.name or not 0 < symbol.shndx < len(base.sections):
            continue
        section = base.sections[symbol.shndx].name
        pattern = re.compile(re.escape(symbol.name.replace('.', '_')) + r'(_[0-9A-F]{8})?')
        twins = [name for (where, name) in names if where == section and pattern.fullmatch(name)]
        if len(twins) == 1:
            renames.append(f'--redefine-sym={symbol.name}={twins[0]}')
    return renames


def data_padding(target_path, base_path):
    """Trim the alignment fill splat counted into a target data label.

    splat sizes a data label up to the next label, so a 26-byte string before
    an 8-byte-aligned object is 32 bytes in the target.  Where the source
    object of the same name is shorter and every byte past it is zero (or the
    section is .bss), the target label takes the source size."""
    target, base = Elf(target_path.read_bytes()), Elf(base_path.read_bytes())
    sizes = {s.name: s.size for s in base.symbols if s.type == 1 and s.shndx}
    symtab = next(s for s in target.sections if s.type == 2)
    data = bytearray(target.data)
    for symbol in target.symbols:
        size = sizes.get(symbol.name)
        if symbol.type != 1 or not 0 < symbol.shndx < len(target.sections) or not size or size >= symbol.size:
            continue
        section = target.sections[symbol.shndx]
        tail = target.section_bytes(section)[symbol.value + size:symbol.value + symbol.size]
        if section.type == 8 or not any(tail):  # SHT_NOBITS or zero fill
            struct.pack_into('<I', data, symtab.offset + 16 * symbol.index + 8, size)
    target_path.write_bytes(bytes(data))


def scaffold_functions(source):
    """Functions the source still takes from assembly (INCLUDE_ASM, ACCEPTED_ASM)."""
    return set(ASM_MACRO.findall(source.read_text(errors='replace'))) if source.is_file() else set()


def assembly_functions(path):
    """Functions of an object that INCLUDE_ASM took from splat's assembly,
    whatever label the file defines: splat marks each one with a
    `<name>.NON_MATCHING` symbol at the same address."""
    elf = Elf(Path(path).read_bytes())
    marked = {(s.shndx, s.value) for s in elf.symbols if s.name.endswith('.NON_MATCHING')}
    return {s.name for s in elf.symbols
            if s.type == 2 and (s.shndx, s.value) in marked and not s.name.endswith('.NON_MATCHING')}


def main_units(root, tools, project, renames):
    unit_dir = root / 'build/main'
    names = dict(renames)
    names.update({label['splat_name']: label['name'] for label in read(unit_dir / 'scaffold-labels.json')
                  if label['splat_name'] != label['name']})
    manifest = read(unit_dir / 'tu-manifest.json')
    by_name = {t['name']: t for t in manifest['tus']}
    jobs = []
    for tu in read(root / 'config/objects/main.objects.json')['tus']:
        if not tu.get('in_scope'):
            continue
        stem = tu['name']
        source = root / tu['path'] if tu.get('path') else None
        jobs.append((stem, tu, by_name[stem], source))

    def one(job):
        stem, tracked, built, source = job
        destination = project / 'main' / stem
        destination.mkdir(parents=True, exist_ok=True)
        parts = []
        if tracked.get('functions'):
            parts.append(unit_dir / 'build/expected' / f'{stem}.o')
        for section, entry in manifest['sections'].items():
            for piece in entry['order']:
                if piece.get('tu') != stem:
                    continue
                data = unit_dir / (piece.get('file') or f'asm/main/data/{piece["piece"]}.{section[1:]}.s')
                output = destination / f'{piece["piece"]}.{section[1:]}.o'
                tools.assemble_data(data, output, unit_dir)
                parts.append(output)
        unit = dict(name=f'main/{stem}', metadata=dict(progress_categories=['main'], complete=False))
        if not parts:
            return None
        target = destination / 'target.o'
        tools.target(parts, target, unit_dir, names)
        unit['target_path'] = str(target)
        compiled = unit_dir / 'build/c' / f'{stem}.o'
        if source is not None and source.is_file() and built.get('mode') == 'c' and compiled.is_file():
            scaffold = scaffold_functions(source)
            base = destination / 'base.o'
            tools.base(compiled, scaffold, base, unit_dir, target, 'main')
            unit['base_path'] = str(base)
            unit['metadata'].update(source_path=tracked['path'], complete=not scaffold)
        return unit

    with ThreadPoolExecutor(max_workers=4) as pool:
        return [u for u in pool.map(one, jobs) if u]


def overlay_units(root, tools, project, unit_name):
    unit_dir = root / 'build' / unit_name
    manifest = read(unit_dir / 'compile-manifest.json')['units'][unit_name]
    cc = root / 'tools/tu/cc_tu.sh'
    renames_path = root / 'config/symbols' / f'{unit_name}.renames.json'
    renames = read(renames_path).get('renames', {}) if renames_path.is_file() else {}
    registry = read(root / 'config/objects/data-carves.json')

    def alignment_labels(tu, parts):
        """Splat labels on the zero alignment between the C objects of a fully C-owned TU.

        They name no object, so no symbol of the source can pair with them.
        Their bytes stay in the target; the original-file gate proves them."""
        tu_id = tu['id'].split('_')[0]
        if tu_id not in registry.get('alignment_linker_fill_tus', ()):
            return []
        runs = [[int(v, 16) for v in run['range']]
                for section, rows in registry['tus'][tu_id].items() for run in rows]
        labels = []
        for name in sorted({n for part in parts for n in tools.defined(part)}):
            found = re.fullmatch(r'D_([0-9A-Fa-f]{8})(\.NON_MATCHING)?', name)
            if found and not any(lo <= int(found[1], 16) < hi for lo, hi in runs):
                labels.append(name)
        return labels

    def external_pointers(tu, data, destination):
        """The scaffold data with raw words naming another image's symbol made relocations.

        splat keeps `.word 0x004A6940` when the address lies outside the overlay.
        In a fully C-owned TU the compiled table relocates against that symbol, so
        the word becomes `.word SaveData+addend` when symbol plus addend is the same value."""
        tu_id = tu['id'].split('_')[0]
        if tu_id not in registry.get('alignment_linker_fill_tus', ()):
            return data
        compiled = Elf((unit_dir / 'build/scaffold/src' / unit_name / f'{tu["name"]}.o').read_bytes())
        addresses = {m[1]: int(m[2], 16) for m in re.finditer(
            r'^(\w+) = (0x[0-9A-Fa-f]+);', (unit_dir / 'main_symbols.ld').read_text(), re.M)}
        symbolic = {}
        for table in [t for t in compiled.sections if t.type == 9]:
            section = compiled.sections[table.info]
            for at in range(table.offset, table.offset + table.size, 8):
                offset, info = struct.unpack_from('<II', compiled.data, at)
                symbol = compiled.symbols[info >> 8]
                if info & 0xff != R_MIPS_32 or symbol.shndx != 0 or symbol.name not in addresses:
                    continue
                addend = struct.unpack_from('<i', compiled.data, section.offset + offset)[0]
                for run in registry['tus'][tu_id].get(section.name, []):
                    for span in run['c_input_spans']:
                        low, high = span['object_range']
                        if low <= offset < high:
                            symbolic[int(span['range'][0], 16) + offset - low] = symbol.name, addend
        text = data.read_text()
        def replace(match):
            name, addend = symbolic.get(int(match[1], 16), (None, 0))
            return (match[0] if name is None or addresses[name] + addend != int(match[3], 16)
                    else f'{match[2]}.word {name}+{addend}')
        changed = re.sub(
            r'(?m)^(?=\s*/\*\s*\w+\s+([0-9A-Fa-f]{8})\s)(\s*/\*[^*]*\*/\s*)\.word (0x[0-9A-Fa-f]+)\s*$', replace, text)
        if changed == text:
            return data
        copy = destination / data.name
        copy.write_text(changed)
        return copy

    def one(tu):
        stem = tu['name']
        destination = project / unit_name / stem
        destination.mkdir(parents=True, exist_ok=True)
        parts = []
        skeleton = unit_dir / 'scaffold/src' / unit_name / f'{stem}.c'
        flags = tu.get('flags') or []
        gflag = next((f for f in flags if f.startswith('-G')), '-G0')
        if tu.get('functions') and skeleton.is_file():
            text = destination / 'text.o'
            subprocess.run([str(cc), str(skeleton), str(text), tu['compiler']['dir'], tu['assembler']['path'],
                            gflag, str(unit_dir), str(root)], cwd=unit_dir, check=True, timeout=120,
                           env={**os.environ,
                                'XENO_EE_AS_OPTIONS': ' '.join(tu_assembler_options(tu)),
                                'XENO_TU_DEPFILE': str(text) + '.d'})
            parts.append(text)
        for data in sorted((unit_dir / 'asm/data' / unit_name).glob(stem + '.*.s')):
            output = destination / (data.stem + '.o')
            tools.assemble_data(external_pointers(tu, data, destination), output, unit_dir)
            parts.append(output)
        if not parts:
            return None
        target = destination / 'target.o'
        tools.target(parts, target, unit_dir, names=renames, strip=alignment_labels(tu, parts))
        unit = dict(name=f'{unit_name}/{stem}', target_path=str(target),
                    metadata=dict(progress_categories=[unit_name], complete=False))
        source = root / tu['source'] if tu.get('source') else None
        compiled = unit_dir / 'build/scaffold/src' / unit_name / f'{stem}.o'
        if tu.get('kind') == 'c' and source is not None and source.is_file() and compiled.is_file():
            scaffold = scaffold_functions(source)
            base = destination / 'base.o'
            tools.base(compiled, scaffold, base, unit_dir, target, unit_name)
            unit['base_path'] = str(base)
            unit['metadata'].update(source_path=tu['source'], complete=not scaffold)
        return unit

    tus = [t for t in manifest['tus'] if t.get('in_scope')]
    with ThreadPoolExecutor(max_workers=4) as pool:
        return [u for u in pool.map(one, tus) if u]


def iop_units(root, project, iop_report):
    from tu import iop_objdiff
    report = read(iop_report)
    units, _, _ = iop_objdiff.prepare(root, project, Path(iop_report).resolve(),
                                      reporting=report.get('mode') == 'reporting')
    return units


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--project', type=Path, default=ROOT / 'build/objdiff')
    ap.add_argument('--iop-report', type=Path, required=True,
                    help='report.json written by tools/iop_build.py')
    args = ap.parse_args()
    root = ROOT
    project = args.project.resolve()
    project.mkdir(parents=True, exist_ok=True)
    tools = Tools(root)
    renames = read(root / 'config/symbols/main.renames.json').get('renames', {})
    units = main_units(root, tools, project, renames)
    for unit in EE_UNITS[1:]:
        units += overlay_units(root, tools, project, unit)
    units += iop_units(root, project, args.iop_report)
    originals = read(root / 'config/originals.json')['units']
    names = {unit: Path(record['file']).name for unit, record in originals.items()}
    names.update({unit: Path(record['file']).name
                  for unit, record in read(root / 'config/objects/iop.json')['units'].items()})
    config = dict(build_target=False, build_base=False, units=units,
                  progress_categories=[dict(id=u, name=names.get(u, u)) for u in EE_UNITS + IOP_UNITS])
    (project / 'objdiff.json').write_text(json.dumps(config, indent=1) + '\n')
    print(json.dumps(dict(project=str(project), units=len(units),
                          with_base=sum('base_path' in u for u in units))))


if __name__ == '__main__':
    main()
