#!/usr/bin/env python3
"""TU-mode acceptance audit: compiler provenance, original symbols, eligibility.

With the whole-file SHA-256 gate this is the normative TU acceptance check
(AGENTS.md, "TU-mode acceptance gate"). It is fail-closed: the audit compiles
the TU itself and every check that cannot be performed is a failure.

What it does, for one TU source and the read-only original ELF of its unit:

  0. Contract: the compile contract comes from config/tu-build.json and
     config/toolchains-tu.json (id, cc1/cpp0/driver and assembler digests,
     flags including -G). Every pin is verified; a contract file or flag that
     differs from the manifest is a failure.
  1. Compile: cpp -> cc1 -> as under that contract, in a private output
     directory (each step 30 s, own process group, minimal environment). Any
     message from any step is a failure. The audit uses its own outputs, never
     supplied ones, so a stale or hand-edited .s/.o cannot be certified.
  2. Build binding: the build's object (--obj, required) must equal the audit's
     object in every allocated section, relocation and symbol; the build's .s
     (--build-s) must equal the audit's .s apart from the recorded .i path.
  3. Preprocessing: every file cpp opened (tu.i line markers) must be admitted
     source or a checked scaffold header, every include must resolve to the
     file cpp used, and each claimed definition must be the one cpp kept - the
     single definition of that name in tu.i, attributed to the TU source inside
     the audited block's lines, and not inside a preprocessor conditional.
  4. Per claimed C function: (a) exactly one cc1 region .ent/.end outside
     top-level asm, in .text, under the TU source's .file, at the block's
     position, without .include, inline asm (outside the hardware class),
     data in .text or any .word other than a cc1 jump table; (b) the original
     .symtab FUNC symbol and the object symbol agree on name, binding, type,
     size and offset from the TU start, with no lost alias; (c) the admission
     of the function's class accepts the prelude plus the function text.
  5. ACCEPTED_ASM functions (exact_asm): the tracked .s under src/<unit>/... is
     included once, hashes to the accepted ee-asm record of config/units, and
     passes match.ee_assembly; every function it defines passes (b).
  6. Scaffold: include/labels.inc must be the pinned include_asm.labels_inc of
     config/toolchains-tu.json and define only the scaffold macros (a macro may
     not shadow a mnemonic); the INCLUDE_ASM/ACCEPTED_ASM templates must stay in
     .text and INCLUDE_RODATA in a data section; INCLUDE_RODATA payloads carry
     data only; no other top-level asm; no scaffold `.extern` of a symbol the C
     defines; scaffold `.word` sites of config/tu-build.json block their
     function.
  7. Object-wide: no cc1 function or named data outside the original TU, no
     symbol that is neither original, scaffold-defined nor claimed; C-owned
     data sections must define every original data symbol of their range; cc1
     COMMON symbols must carry the original size and a compatible alignment.

Usage (docs/tu-tools.md):
  tu_audit.py --unit ov02 --unit-dir UNIT --tu-c UNIT/src/ov02/title.c \\
      --tu-id ov02/tu002 --obj UNIT/build/src/ov02/title.o --out-dir FRESH
"""
import argparse
import collections
import difflib
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import re
import shutil
import signal
import struct
import subprocess
import sys
import tempfile

sys.dont_write_bytecode = True
# This private copy lives outside both the shared tools package and the subject
# tree. Its helper imports are pinned to the canonical repository; `ROOT` below
# remains the tree under audit and is rebound only by `resolve_root()`.
_AUDIT_PATH = Path(__file__).resolve()
TOOL_ROOT = (_AUDIT_PATH.parents[1] if _AUDIT_PATH.parent.name == "tools"
             else _AUDIT_PATH.parents[3])
sys.path.insert(0, str(TOOL_ROOT))
from tools import tu_edit  # noqa: E402
from tools import asm_contract_lint  # noqa: E402

# The tree under audit. Every read of the subject's own content -- its `src`,
# its `config` (TU manifest, accepted records, source classes, asm contracts,
# adaptations), its archives and its originals -- goes through this, and it is
# resolved from the audit's subject by `resolve_root()` before anything is read.
# It is a module global rather than an argument because the audit is one process
# auditing one tree; `resolve_root()` is the only writer.
ROOT = TOOL_ROOT

SCHEMA = 'tu-audit/2'
TIMEOUT = 30
CLASSES = ('ordinary', 'hardware', 'runtime-verbatim', 'native-ti', 'gnu-nested-function', 'ee-mmi',
           'ee-asm-leaf', 'ee-fpu', 'ee-vu-clip', 'ee-assert-break', 'ee-retaddr-store', 'ee-asm-sort')
# Classes whose functions may carry admitted inline assembly and are reported
# `exact_c_with_asm` when they do: `hardware` (the ee-vu-cop2 route), `ee-mmi`
# (constrained inline EE MMI, user decision 2026-10-10, docs/ee-mmi.md), and
# `ee-asm-leaf` / `ee-fpu` (original hand-written assembly delivered as C with
# asm, user decision 2026-10-10 on main/tu100, docs/ee-hand-asm.md), and
# `ee-vu-clip` (the same decision extended on 2026-10-11 to the main/tu109 VU0
# lane-test blocks, docs/ee-hand-asm.md), and `ee-assert-break` (the assert
# macros' bare `break` trap through include/debug_break.h, user decision
# 2026-10-11 on main/tu121, docs/ee-hand-asm.md), and `ee-retaddr-store` (the
# one entry `sw $31,0($sp)` return-address store of UnduCheck, user decision
# 2026-10-27 on main/tu273, docs/ee-hand-asm.md; that identity only), and
# `ee-asm-sort` (the whole-body hand-written bubble sort of AlphaGroupSort, user
# decision 2026-10-28 on main/tu106, docs/ee-hand-asm.md; that identity only).
INLINE_ASM_CLASSES = ('hardware', 'ee-mmi', 'ee-asm-leaf', 'ee-fpu', 'ee-vu-clip', 'ee-assert-break',
                      'ee-retaddr-store', 'ee-asm-sort')
# The one function identity `ee-retaddr-store` admits (user decision 2026-10-27).
RETADDR_STORE_IDENTITY = ('main', 'main/tu273', 'UnduCheck')
# The one function identity `ee-asm-sort` admits (user decision 2026-10-28).
ASM_SORT_IDENTITY = ('main', 'main/tu106', 'AlphaGroupSort')
# The one `hardware` function whose COP2 block may open with the VU0 I-register
# immediate `lui $2,0x3e00` -> `ctc2 $2,$vi21` (user decision 2026-10-28,
# tools/match.py `vu0_i_immediate_c`, docs/ee-hand-asm.md).
VU0_I_IMMEDIATE_IDENTITY = ('main', 'main/tu097', 'StartOfScan')
# The one function identity whose TU may spell `__attribute__((packed))` on its
# TU-local byte enum typedefs (user decision 2026-10-28 on main/tu125): the gp
# byte stores GameMovieInit makes through pointer locals need a non-char,
# non-zero alias set, which a packed enum gives and `unsigned char` does not.
# Only the attribute of a whole `typedef enum [Tag] { ENUMERATORS }
# __attribute__((packed)) Name;` declaration in the TU source itself (never a
# header, a function body or any other declaration) is masked for admission,
# and only when the TU defines GameMovieInit as C. Never a precedent.
PACKED_ENUM_IDENTITY = ('main', 'main/tu125', 'GameMovieInit')
_PACKED_ENUM_TYPEDEF = re.compile(
    r'\s*typedef\s+enum\s+(?:[A-Za-z_]\w*\s*)?\{(?P<body>[^{}]*)\}\s*'
    r'(?P<attribute>__attribute__\s*\(\s*\(\s*packed\s*\)\s*\))\s*'
    r'(?P<name>[A-Za-z_]\w*)\s*;\s*')
_PACKED_ENUM_BODY = re.compile(r'[A-Za-z_0-9\s=,+\-|<>()~]*')
SOURCE_CLASS = {'game_c': 'ordinary', 'sdk_c': 'ordinary', 'runtime_c': 'ordinary', 'hand_asm': 'ordinary'}
TU_MAPS = ('.work/tu-migration/map-main/tu-map-main.json', '.work/tu-migration/map-ovl/tu-map-ovl.json')
BIND = {0: 'LOCAL', 1: 'GLOBAL', 2: 'WEAK'}
STT_NOTYPE, STT_OBJECT, STT_FUNC, STT_SECTION, STT_FILE = 0, 1, 2, 3, 4
SHN_COMMON, SHN_MIPS_SCOMMON = 0xfff2, 0xff03
MARKER_NAMES = ('gcc2_compiled.', '__gnu_compiled_c')
ASM_CONTRACT_FIELDS = {'purpose', 'inputs', 'outputs', 'clobbers', 'memory', 'state', 'privilege', 'control_flow'}
CINC_TOOL = 'tools/tu/cinc.py'
SOURCE_CLASSES_FILE = 'config/tu/source-classes.json'
SOURCE_CLASSES_SCHEMA = 'tu-source-classes/1'
DATA_DIRECTIVES = {'.word', '.half', '.hword', '.short', '.byte', '.dword', '.2byte', '.4byte', '.8byte', '.int',
                   '.long', '.quad', '.octa', '.ascii', '.asciz', '.string', '.float', '.single', '.double',
                   '.space', '.skip', '.fill', '.zero', '.incbin', '.inst', '.insn', '.gpword', '.gpdword',
                   '.org', '.dc', '.dc.b', '.dc.w', '.dc.l', '.dc.a'}
WORD_LIKE = {'.word', '.4byte', '.long', '.int', '.gpword'}
TEXT_SECTIONS = ('.text', '.vutext')
DATA_SECTIONS = ('.data', '.rodata', '.rdata', '.sdata', '.lit4', '.lit8', '.bss', '.sbss')
RODATA_LIKE = ('.rodata', '.rdata', '.lit4', '.lit8')
# Only these macros may exist in the scaffold macro include: a GAS macro is
# resolved before machine instructions, so any other name can shadow a mnemonic.
SCAFFOLD_MACROS = frozenset(('glabel', 'endlabel', 'alabel', 'ehlabel', 'jlabel', 'dlabel', 'enddlabel',
                             'nonmatching'))
# Sections that only record debug/compiler paths; excluded from the object
# comparison because the recorded name of the preprocessed input differs
# between the build's output directory and the audit's private one.
DEBUG_SECTIONS = ('.mdebug', '.mdebug.eabi64', '.comment', '.pdr', '.gptab.sdata', '.gptab.sbss')


class AuditError(ValueError):
    """An input that makes the audit impossible (reported as a failing audit)."""


# ---------------------------------------------------------------------------
# Which tree is being audited
# ---------------------------------------------------------------------------
# The audit must read the subject's own `src` and `config`, never a second tree
# that happens to hold the same paths. It used to take them from the script's
# location, which is wrong in exactly the case that matters: `tools/worker.py`
# builds a sandbox whose `src`, `include` and `build` are private and whose
# `tools/` and `config/` are links to the shared repository, and then runs the
# SHARED `tools/tu_audit.py` over it. `Path(__file__).parents[1]` then named the
# published tree, so
#
#   * `record_fidelity` hashed and read the PUBLISHED TU file instead of the
#     candidate. `rel(tu_c)` could not name the candidate relative to that tree
#     either, so the check never even recognised that the record points at the
#     file under audit: it silently compared the candidate's body against the
#     published body of the same function and reported `verbatim`. A candidate
#     that rewrote a *different* accepted C function of the same TU passed with
#     `grants_acceptance` (measured, tu-audit-root-resolution).
#   * `c_record_index`/`accepted_asm_records` globbed the PUBLISHED
#     `config/units`, so a re-cut accepted `.s` could never be presented with
#     its narrowed record: the audit only ever saw the wide published one.
#   * `published_source_classes`, the asm contracts it pins, `lookup_tu`, the
#     adaptation files, the retired-source archive and
#     `match.runtime_verbatim_provenance` (which re-reads the source it is given
#     a root for) all read published state while claiming to audit a candidate.
#
# So the root comes from the subject: `--root` when given, else the repository
# tree that encloses `--unit-dir` -- the unit build directory whose `src` link
# decides which sources are compiled in the first place. A `--unit-dir` outside
# any repository tree, or a TU source outside the resolved one, is a failure
# with a named remedy, not a quiet fallback to wherever the script lives.
ROOT_MARKERS = ('config/tu-build.json', 'config/toolchains-tu.json', 'src')


def enclosing_root(path, what):
    """The nearest enclosing repository tree of `path`."""
    here = Path(os.path.abspath(path))
    for candidate in (here, *here.parents):
        if all((candidate / marker).exists() for marker in ROOT_MARKERS):
            return candidate
    raise AuditError(f'{what} {path} is not inside a repository tree (no enclosing directory has '
                     f'{", ".join(ROOT_MARKERS)}); pass --root to name the tree to audit')


def resolve_root(args):
    """Point the audit at the tree it was given, and report which one that is."""
    global ROOT
    if args.root:
        root = Path(os.path.abspath(args.root))
        missing = [m for m in ROOT_MARKERS if not (root / m).exists()]
        if missing:
            raise AuditError(f'--root {root} is not a repository tree: {missing} missing')
        basis = '--root'
    else:
        root = enclosing_root(args.unit_dir, '--unit-dir')
        basis = '--unit-dir'
    ROOT = root
    # The record indexes are keyed by nothing but ROOT, so a process that
    # re-points the root re-reads them instead of serving another tree's.
    global _C_RECORD_INDEX, _ACCEPTED_ASM_INDEX
    _C_RECORD_INDEX = _ACCEPTED_ASM_INDEX = None
    tu_c = Path(os.path.abspath(args.tu_c))
    inside = (rel(tu_c) != str(tu_c))
    return dict(path=str(root), basis=basis, tool_root=str(TOOL_ROOT),
                tool_outside_root=not str(TOOL_ROOT).startswith(str(root) + os.sep) and TOOL_ROOT != root,
                tu_source_inside_root=inside,
                detail=None if inside else
                       (f'the TU source {tu_c} is not inside the audited tree {root}, so its accepted '
                        'records, adaptations and source classes cannot be resolved against it; audit the '
                        'candidate where it is built, or pass --root'))


def sha256_file(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def sha256_text(text):
    return hashlib.sha256(text.encode('utf-8', 'surrogateescape')).hexdigest()


def rel(path, base=None):
    # `base` defaults to the audited tree at CALL time: binding ROOT in the
    # signature would freeze the tool's own location into every path the audit
    # reports, and into `record_fidelity`'s "does this record point at the file
    # I am auditing?" test, which is how the check went blind in a sandbox.
    base = ROOT if base is None else base
    # Lexically first, so a file reached through one of the tree's own links
    # (`config/` and `.work/` are links in a worker sandbox) is still named
    # relative to the tree it was reached through; then by real path, which is
    # what a directory argument spelled through a link needs.
    for here, there in ((Path(os.path.abspath(path)), Path(os.path.abspath(base))),
                        (Path(path).resolve(), Path(base).resolve())):
        try:
            return str(here.relative_to(there))
        except ValueError:
            continue
    return str(path)


# ---------------------------------------------------------------------------
# Relocatable ELF32 LE reader (objects)
# ---------------------------------------------------------------------------

class RelObject:
    """Sections, symbols and relocations of an ELF32 little-endian MIPS object."""

    def __init__(self, data):
        self.data = data
        if data[:6] != b'\x7fELF\x01\x01':
            raise AuditError('object is not ELF32 little-endian')
        (etype, machine, _v, _entry, _phoff, shoff, _flags, _eh, _phes, _phn, shentsize, shnum,
         shstrndx) = struct.unpack_from('<HHIIIIIHHHHHH', data, 16)
        if etype != 1 or machine != 8:
            raise AuditError('object is not a MIPS relocatable (ET_REL)')
        if shentsize < 40 or shoff + shnum * shentsize > len(data):
            raise AuditError('object section table out of range')
        raw = [struct.unpack_from('<10I', data, shoff + i * shentsize) for i in range(shnum)]
        names = data[raw[shstrndx][4]:raw[shstrndx][4] + raw[shstrndx][5]]
        self.sections = []
        for i, (name, kind, flags, addr, offset, size, link, info, align, entsize) in enumerate(raw):
            end = names.find(b'\0', name)
            self.sections.append(dict(index=i, name=names[name:end].decode('ascii', 'replace'), type=kind,
                                      flags=flags, addr=addr, offset=offset, size=size, link=link, info=info,
                                      align=align, entsize=entsize))
        self.symbols = []
        for sec in self.sections:
            if sec['type'] != 2:
                continue
            strtab = self.sections[sec['link']]
            strings = data[strtab['offset']:strtab['offset'] + strtab['size']]
            for k in range(sec['size'] // 16):
                name, value, size, info, other, shndx = struct.unpack_from('<IIIBBH', data, sec['offset'] + k * 16)
                end = strings.find(b'\0', name)
                section = self.sections[shndx]['name'] if 0 < shndx < len(self.sections) else None
                self.symbols.append(dict(index=k, name=strings[name:end].decode('utf-8', 'surrogateescape'),
                                         value=value, size=size, type=info & 15, binding=info >> 4, other=other,
                                         shndx=shndx, section=section))

    def section(self, name):
        found = [s for s in self.sections if s['name'] == name]
        return found[0] if len(found) == 1 else None

    def bytes_of(self, sec):
        if sec['type'] == 8:  # NOBITS
            return b''
        return self.data[sec['offset']:sec['offset'] + sec['size']]

    def functions(self):
        return [s for s in self.symbols if s['type'] == STT_FUNC and s['shndx'] != 0]

    def relocations(self):
        """[(section, offset, type, symbol name)] over all REL sections."""
        out = []
        for sec in self.sections:
            if sec['type'] != 9 or sec['entsize'] not in (8, 0):
                continue
            target = self.sections[sec['info']]['name'] if sec['info'] < len(self.sections) else '?'
            symtab = self.sections[sec['link']] if sec['link'] < len(self.sections) else None
            syms = [s for s in self.symbols if symtab and s['index'] is not None] if symtab else []
            by_index = {s['index']: s for s in syms}
            for k in range(sec['size'] // 8):
                offset, info = struct.unpack_from('<II', self.data, sec['offset'] + k * 8)
                sym = by_index.get(info >> 8)
                out.append((target, offset, info & 0xff, sym['name'] if sym else str(info >> 8)))
        return out


def compare_objects(fresh, build):
    """Differences between two objects, ignoring debug/path-only sections."""
    diffs = []
    a, b = RelObject(Path(fresh).read_bytes()), RelObject(Path(build).read_bytes())
    names_a = [s['name'] for s in a.sections if s['name'] not in DEBUG_SECTIONS and s['name']]
    names_b = [s['name'] for s in b.sections if s['name'] not in DEBUG_SECTIONS and s['name']]
    if set(names_a) != set(names_b):
        diffs.append(f'section sets differ: {sorted(set(names_a) ^ set(names_b))}')
    for name in sorted(set(names_a) & set(names_b)):
        sa, sb = a.section(name), b.section(name)
        if sa is None or sb is None:
            diffs.append(f'{name}: duplicate section name')
            continue
        if (sa['type'], sa['flags'], sa['size'], sa['align']) != (sb['type'], sb['flags'], sb['size'], sb['align']):
            diffs.append(f'{name}: header differs (size {sa["size"]} vs {sb["size"]})')
        if a.bytes_of(sa) != b.bytes_of(sb):
            diffs.append(f'{name}: contents differ')
    def symkey(o):
        return sorted((s['name'], s['value'], s['size'], s['type'], s['binding'], s['other'],
                       (s['section'] or '') if s['section'] not in DEBUG_SECTIONS else '')
                      for s in o.symbols if s['type'] != STT_FILE)
    if symkey(a) != symkey(b):
        only_a = [s for s in symkey(a) if s not in symkey(b)][:6]
        only_b = [s for s in symkey(b) if s not in symkey(a)][:6]
        diffs.append(f'symbol tables differ: audit-only {only_a}, build-only {only_b}')
    if sorted(a.relocations()) != sorted(b.relocations()):
        diffs.append('relocations differ')
    return diffs


_FILE = re.compile(r'^\t?\.file\t?\s*(\d+)\s+"[^"]*"', re.M)


def compare_assembly(fresh_text, build_text):
    """Differences between two cc1 outputs, ignoring the recorded input paths.

    cc1 records `.file 1` for the preprocessed input and `.file 2` for the TU
    source itself, each spelled the way the caller named it. The audit compiles
    in its own output directory, so both spellings legitimately differ from the
    build edge's even when the emitted code is identical. `.file 1` was already
    normalised; `.file 2` was not, which is why `--build-s` could only be passed
    where the build happened to spell the source exactly as the audit does (main;
    the overlay edges spell it relative to the unit directory). Normalising every
    `.file N` path - keeping its number, so a different *set* of files still
    differs - lets `--build-s` run for every unit (docs/tu-build.md, "Layout
    requirement").

    What the paths could have caught is covered elsewhere and not by a debug
    string: the object comparison checks sections, relocations and symbols, and
    the preprocessing check verifies that every file cpp opened is admitted
    source and that each claimed definition comes from the TU source itself.
    """
    norm = lambda t: _FILE.sub(lambda m: f'\t.file\t{m.group(1)} "<input>"', t)
    if norm(fresh_text) == norm(build_text):
        return []
    a, b = norm(fresh_text).splitlines(), norm(build_text).splitlines()
    for i, (x, y) in enumerate(zip(a, b), 1):
        if x != y:
            return [f'line {i}: audit {x.strip()[:60]!r} vs build {y.strip()[:60]!r}']
    return [f'length differs: {len(a)} vs {len(b)} lines']


# ---------------------------------------------------------------------------
# cc1 assembly parser
# ---------------------------------------------------------------------------

def _strip_comment(line):
    """Remove a trailing '#' comment outside string literals."""
    quote, k = False, 0
    while k < len(line):
        c = line[k]
        if c == '\\' and quote:
            k += 2
            continue
        if c == '"':
            quote = not quote
        elif c == '#' and not quote:
            return line[:k]
        k += 1
    return line


_SECTION_ALIASES = {'.text': '.text', '.data': '.data', '.rdata': '.rodata', '.sdata': '.sdata', '.bss': '.bss',
                    '.sbss': '.sbss', '.lit4': '.lit4', '.lit8': '.lit8', '.rodata': '.rodata'}
_ALLOWED_APP = [re.compile(p) for p in (
    r'\.section\s+\.text', r'\.section\s+\.rodata', r'\.section\s+\.data', r'\.section\s+\.sdata',
    r'\.set\s+noat', r'\.set\s+noreorder', r'\.set\s+reorder', r'\.set\s+at',
    # state restoration after accepted .s bodies that switch it (tu-ovl-phase2 review)
    r'\.set\s+macro', r'\.set\s+nomacro',
    # plain alignment, which the approved ACCEPTED_ASM scaffold emits (`.align 3`:
    # accepted blocks start 8-byte aligned while the tracked .s starts at .align 2).
    # Only the padding form: `.align n, fill` carries a fill operand, does not match
    # here and stays a data directive.
    r'\.align\s+\d+')]
_SCAFFOLD_INC = re.compile(r'\.include\s+"include/(?:labels|macro)\.inc"')
_EXTERN_LINE = re.compile(r'\.extern\s+[A-Za-z_.$][\w.$]*\s*,\s*\d+')


def _app_line_allowed(line):
    return bool(_SCAFFOLD_INC.fullmatch(line) or any(p.fullmatch(line) for p in _ALLOWED_APP)
                or re.fullmatch(r'\.include\s+"[^"]+\.s"', line))


# cc1's anonymous constant pool (varasm): what GCC 2.96 writes for the string,
# float and address-template constants of the NEXT function before that
# function starts. It opens with a bare section switch into a data section (a
# pool never continues in .text), holds only `.align N`, `$LCn:` labels and data
# directives with plain operands (one statement per line: no `;`, no
# parentheses), and cc1 then leaves app mode (`#NO_APP`) and returns to `.text`
# for the function. The grammar is only half of the rule: `AsmFile` records the
# lines it admits as `pool_lines`, and `top_level_asm` still reports every one
# of them that a top-level asm statement of the preprocessed unit writes
# (`asm_statement_lines`), so hand-written asm cannot pass as a pool.
_POOL_OPEN = re.compile(r'\.(?:rdata|sdata|lit4|lit8)')
_POOL_OPERAND = r'(?:-?(?:0x[0-9a-fA-F]+|\d+)|\$LC\d+|[A-Za-z_][\w.]*)(?:\s*[-+]\s*\d+)?'
_POOL_LINE = re.compile(
    r'\.(?:rdata|sdata|lit4|lit8)|\.section\s+\.(?:rodata|sdata|lit4|lit8)|\.align\s+\d+|\$LC\d+:'
    r'|\.ascii\s+"(?:[^"\\]|\\.)*"'
    r'|\.(?:word|half|byte|dword|space|gpword)\s+' + _POOL_OPERAND + r'(?:\s*,\s*' + _POOL_OPERAND + r')*')
_POOL_LABEL = re.compile(r'\$LC\d+:')


def _norm_asm_line(line):
    return ' '.join(line.split())


def asm_statement_lines(i_text):
    """{normalized line} of every asm statement string in a preprocessed unit.

    GCC writes a top-level asm statement verbatim into the assembly, one output
    line per line of its string, so any line of the .s that equals one of these
    may come from hand-written asm. Inline asm templates and asm labels are
    included too: over-inclusion can only report a cc1 line as stray, never admit
    a hand-written one."""
    out = set()
    for m in re.finditer(r'\b(?:__asm__|__asm|asm)\b(?:\s*(?:__volatile__|__volatile|volatile))?\s*\(', i_text):
        pos, parts = m.end(), []
        while True:
            ws = re.match(r'(?:\s+|^#[^\n]*\n)*', i_text[pos:], re.M)
            pos += ws.end()
            lit = re.match(r'"((?:\\.|[^"\\\n])*)"', i_text[pos:])
            if not lit:
                break
            try:
                parts.append(lit.group(1).encode('latin-1', 'replace').decode('unicode_escape'))
            except UnicodeDecodeError:
                parts.append(lit.group(1))
            pos += lit.end()
        for line in ''.join(parts).split('\n'):
            line = _norm_asm_line(_strip_comment(line))
            if line:
                out.add(line)
    return out


class AsmFile:
    """Regions, top-level asm blocks and data of a cc1 .s file."""

    def __init__(self, text):
        self.regions, self.top_app, self.top_data, self.sequence = [], [], [], []
        self.files, self.errors, self.defined_data, self.comms, self.externs = {}, [], [], [], []
        self.section, self.previous, self.current_file = '.text', '.text', None
        self.region, self.app = None, None
        # cc1 constant pools that closed a top-level #APP block (_cc1_pool_end),
        # and the line of the deferred #NO_APP that then closes nothing
        self.pool_lines, self.deferred_no_app = [], set()
        lines = text.splitlines()
        index = 0
        while index < len(lines):
            number, raw = index + 1, lines[index]
            index += 1
            stripped = raw.strip()
            if stripped == '#APP':
                if self.app is not None:
                    self.errors.append(f'line {number}: nested #APP')
                self.app = dict(line=number, lines=[], function=self.region['name'] if self.region else None)
                continue
            if stripped == '#NO_APP':
                if self.app is None and number in self.deferred_no_app:
                    self.deferred_no_app.discard(number)
                    continue
                if self.app is None:
                    self.errors.append(f'line {number}: #NO_APP without #APP')
                    continue
                (self.region['app'] if self.region else self.top_app).append(self.app)
                self.app = None
                continue
            code = _strip_comment(raw).strip()
            if self.app is not None:
                if not code:
                    continue
                # cc1 writes no final #NO_APP: an unterminated top-level block
                # ends at the first line that is not part of a scaffold template.
                # cc1 leaves app mode only when the next function starts, so the
                # constant pool it writes for that function before it lands inside
                # the block the preceding INCLUDE_ASM expansions opened; the block
                # ends where the pool starts, and the deferred #NO_APP closes nothing.
                pool_end = (self._cc1_pool_end(lines, index)
                            if self.region is None and not _app_line_allowed(code) else None)
                if pool_end is not None:
                    self.top_app.append(self.app)
                    self.app = None
                    self.deferred_no_app.add(pool_end)
                    for k in range(index - 1, pool_end - 1):
                        pool_code = _strip_comment(lines[k]).strip()
                        if pool_code:
                            self.pool_lines.append((k + 1, pool_code))
                elif self.region is None and not _app_line_allowed(code) and not self._app_open_legitimately(lines, index):
                    self.top_app.append(self.app)
                    self.app = None
                else:
                    self.app['lines'].append((number, code))
                    self._app_state(code, number)
                    continue
            if code:
                self._line(number, code)
        if self.region is not None:
            self.errors.append(f'unterminated region {self.region["name"]}')
        if self.app is not None:
            if self.region is None:
                self.top_app.append(self.app)
            else:
                self.errors.append('unterminated #APP')

    @staticmethod
    def _cc1_pool_end(lines, index):
        """Line number of the #NO_APP ending a cc1 constant pool that starts at line `index`, or None.

        A pool opens with a bare switch into a data section, holds only pool
        lines (_POOL_LINE) and at least one `$LCn:` label up to the next
        #NO_APP, and that #NO_APP is directly followed by cc1's `.text` for the
        function whose start ended app mode. Anything else - another #APP, a
        function start, a line outside the grammar, no `.text` - is not a pool,
        and the block is judged as before."""
        first = _strip_comment(lines[index - 1]).strip()
        if not _POOL_OPEN.fullmatch(first):
            return None
        labels = 0
        for k in range(index - 1, len(lines)):
            s = lines[k].strip()
            if s == '#NO_APP':
                following = next((_strip_comment(x).strip() for x in lines[k + 1:]
                                  if _strip_comment(x).strip()), None)
                return k + 1 if labels and following == '.text' else None
            if s == '#APP':
                return None
            code = _strip_comment(lines[k]).strip()
            if not code:
                continue
            if not _POOL_LINE.fullmatch(code):
                return None
            labels += bool(_POOL_LABEL.fullmatch(code))
        return None

    def _app_open_legitimately(self, lines, index):
        """True while a later #NO_APP still closes this block (inline asm)."""
        for raw in lines[index - 1:]:
            s = raw.strip()
            if s == '#NO_APP':
                return True
            if s == '#APP':
                return False
        return False

    def _app_state(self, code, number):
        m = re.match(r'\.section\s+"?([^\s",]+)', code)
        if m:
            self.previous, self.section = self.section, _SECTION_ALIASES.get(m.group(1), m.group(1))
        elif code in _SECTION_ALIASES:
            self.previous, self.section = self.section, _SECTION_ALIASES[code]
        m = re.match(r'\.include\s+"([^"]+)"', code)
        if m and self.region is None:
            self.sequence.append(('include', m.group(1), number, self.section))

    def _line(self, number, code):
        m = re.match(r'\.file\s+(\d+)\s+"([^"]*)"', code)
        if m:
            self.files[int(m.group(1))] = m.group(2)
            self.current_file = m.group(2)
            return
        m = re.match(r'\.section\s+"?([^\s",]+)', code)
        if m:
            self.previous, self.section = self.section, _SECTION_ALIASES.get(m.group(1), m.group(1))
        elif code in ('.previous',):
            self.previous, self.section = self.section, self.previous
        elif code in _SECTION_ALIASES:
            self.previous, self.section = self.section, _SECTION_ALIASES[code]
        elif code.startswith(('.pushsection', '.popsection', '.subsection')):
            self.errors.append(f'line {number}: unsupported section directive {code[:40]}')
        m = re.match(r'\.ent\s+(\S+)', code)
        if m:
            if self.region is not None:
                self.errors.append(f'line {number}: .ent {m.group(1)} inside {self.region["name"]}')
            self.region = dict(name=m.group(1), start=number, end=None, section=self.section,
                               file=self.current_file, labels=[], app=[], data=[], includes=[])
            self.sequence.append(('c', m.group(1), number, self.section))
            return
        m = re.match(r'\.end\s+(\S+)', code)
        if m:
            if self.region is None or self.region['name'] != m.group(1):
                self.errors.append(f'line {number}: unmatched .end {m.group(1)}')
            else:
                self.region['end'] = number
                self.regions.append(self.region)
                self.region = None
            return
        label = re.match(r'"?([A-Za-z_.$][\w.$]*)"?:', code)
        if label:
            if self.region is not None:
                self.region['labels'].append((label.group(1), self.section))
            elif not label.group(1).startswith('$') and label.group(1) not in MARKER_NAMES:
                self.defined_data.append(label.group(1))
        directive = code.split(None, 1)[0]
        operands = code[len(directive):].strip()
        if directive == '.include':
            if self.region is not None:
                self.region['includes'].append((number, operands))
            else:
                self.errors.append(f'line {number}: .include outside #APP')
        m = re.match(r'\.(l?comm)\s+([A-Za-z_.$][\w.$]*)\s*,\s*(\d+)(?:\s*,\s*(\d+))?', code)
        if m:
            self.defined_data.append(m.group(2))
            self.comms.append(dict(kind=m.group(1), name=m.group(2), size=int(m.group(3)),
                                   align=int(m.group(4)) if m.group(4) else None, line=number))
        m = re.match(r'\.extern\s+([A-Za-z_.$][\w.$]*)', code)
        if m:
            self.externs.append(m.group(1))
        # `.align n, fill` emits fill bytes; plain alignment does not.
        fill = operands.split(',')[1].strip() if operands.count(',') >= 1 else ''
        emits = directive in DATA_DIRECTIVES or (directive in ('.align', '.p2align', '.balign') and fill)
        if emits:
            entry = dict(line=number, directive=directive, operands=operands, section=self.section)
            (self.region['data'] if self.region else self.top_data).append(entry)


# ---------------------------------------------------------------------------
# Preprocessed unit (tu.i)
# ---------------------------------------------------------------------------

class Preprocessed:
    """Line markers of a cpp output: which file and line every offset came from."""

    MARKER = re.compile(r'^#\s*(\d+)\s+"([^"]*)"(.*)$', re.M)

    def __init__(self, text):
        self.text = text
        self.markers = []  # (offset after the marker line, file, line)
        for m in self.MARKER.finditer(text):
            end = text.find('\n', m.end())
            end = len(text) if end < 0 else end + 1
            self.markers.append((end, m.group(2), int(m.group(1))))

    def files(self):
        out = []
        for _, name, _line in self.markers:
            if name.startswith('<') or name in out:
                continue
            out.append(name)
        return out

    def attribute(self, offset):
        """(file, line) of a text offset, following the markers."""
        best = None
        for start, name, line in self.markers:
            if start <= offset:
                best = (start, name, line)
            else:
                break
        if best is None:
            return None, None
        start, name, line = best
        return name, line + self.text.count('\n', start, offset)


# ---------------------------------------------------------------------------
# Scaffold headers and macro includes
# ---------------------------------------------------------------------------

def _blank_comments(text):
    return re.sub(r'"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'|/\*.*?\*/|//[^\n]*',
                  lambda m: m[0] if m[0][0] in '"\'' else ''.join('\n' if c == '\n' else ' ' for c in m[0]),
                  text, flags=re.S)


def _logical_lines(text):
    return re.sub(r'\\\r?\n', ' ', _blank_comments(text)).splitlines()


def _asm_template(body, params):
    """Decode __asm__(...) built from string literals and FOLDER/#NAME tokens."""
    m = re.fullmatch(r'\s*__asm__\s*\((.*)\)\s*;?\s*', body, re.S)
    if not m:
        return None
    out, rest = [], m.group(1).strip()
    while rest:
        lit = re.match(r'"((?:\\.|[^"\\])*)"\s*', rest)
        if lit:
            out.append(bytes(lit.group(1), 'ascii').decode('unicode_escape'))
            rest = rest[lit.end():]
            continue
        tok = re.match(r'(#?)\s*([A-Za-z_]\w*)\s*', rest)
        if tok and tok.group(2) in params:
            out.append('<' + params[tok.group(2)] + '>')
            rest = rest[tok.end():]
            continue
        return None
    return ''.join(out)


def _template_ok(macro, template):
    """The expansion must stay in .text (code) or a data section (rodata)."""
    want_text = macro in ('INCLUDE_ASM', 'ACCEPTED_ASM')
    section, included = None, False
    for raw in template.split('\n'):
        line = raw.strip()
        if not line:
            continue
        if line == '.include "<F>/<N>.s"':
            if section is None:
                return False, 'the payload is included before a section is selected'
            if want_text and section not in TEXT_SECTIONS:
                return False, f'{macro} includes code into {section}'
            if not want_text and section in TEXT_SECTIONS:
                return False, f'{macro} includes data into {section}'
            included = True
            continue
        m = re.fullmatch(r'\.section\s+([^\s,]+)', line)
        if m:
            if section is None and ((m.group(1) in TEXT_SECTIONS) != want_text):
                return False, f'{macro} starts in {m.group(1)}'
            section = m.group(1)
            continue
        if any(p.fullmatch(line) for p in _ALLOWED_APP):
            continue
        return False, f'unexpected line in the {macro} template: {line[:40]}'
    return (included, None if included else f'the {macro} template includes nothing')


def scaffold_header_ok(text):
    """(ok, reason): a header that only defines the INCLUDE_ASM family."""
    for line in _logical_lines(text):
        s = line.strip()
        if not s:
            continue
        if re.fullmatch(r'#\s*(?:if|ifdef|ifndef|elif)\b[^"\'{};]*', s) or re.fullmatch(r'#\s*(?:else|endif)\s*', s):
            continue
        if re.fullmatch(r'#\s*define\s+[A-Za-z_]\w*(?:\s+\d+)?\s*', s):
            continue
        if re.fullmatch(r'#\s*include\s+"(?:include_asm\.h|common\.h)"\s*', s):
            continue
        m = re.fullmatch(r'#\s*define\s+(INCLUDE_ASM|ACCEPTED_ASM|INCLUDE_RODATA)\s*\(\s*([A-Za-z_]\w*)\s*,'
                         r'\s*([A-Za-z_]\w*)\s*\)(.*)', s, re.S)
        if m:
            if not m.group(4).strip():
                continue
            if re.fullmatch(r'\s*(?:INCLUDE_ASM|ACCEPTED_ASM|INCLUDE_RODATA)\s*\(\s*' + re.escape(m.group(2))
                            + r'\s*,\s*' + re.escape(m.group(3)) + r'\s*\)\s*', m.group(4)):
                continue  # ACCEPTED_ASM delegating to INCLUDE_ASM with the same parameters
            template = _asm_template(m.group(4), {m.group(2): 'F', m.group(3): 'N'})
            if template is None:
                return False, f'{m.group(1)} is not a plain __asm__ template'
            ok, reason = _template_ok(m.group(1), template)
            if not ok:
                return False, reason
            continue
        template = _asm_template(s, {})
        if template is not None and all(not a.strip() or _SCAFFOLD_INC.fullmatch(a.strip())
                                        for a in template.split('\n')):
            continue
        return False, f'unexpected line in the scaffold header: {s[:60]}'
    return True, None


DEBUG_BREAK_HEADER_LINES = ['#ifndef DEBUG_BREAK_H', '#define DEBUG_BREAK_H',
                            '#define DEBUG_BREAK() __asm__ __volatile__("break")', '#endif']


def debug_break_header_ok(text):
    """(ok, reason): include/debug_break.h, the one header that spells the
    `ee-assert-break` trap (docs/ee-hand-asm.md): its guard and the single
    DEBUG_BREAK() definition, nothing else. A function only gains the trap by
    declaring `ee-assert-break` (or, for main/tu273 UnduCheck, `ee-retaddr-store`); `audit_c` refuses its #APP block otherwise."""
    code = re.sub(r'/\*.*?\*/|//[^\n]*', ' ', text, flags=re.S)
    lines = [re.sub(r'\s+', ' ', line).strip() for line in code.splitlines()]
    lines = [line for line in lines if line]
    if lines != DEBUG_BREAK_HEADER_LINES:
        return False, 'not the debug_break.h DEBUG_BREAK() definition'
    return True, None


# include/quadword.h (user decision 2026-11-02, .work/main-tidy-20261101/
# lq-sq-research.md): the original's quadword transfer macros. Each macro is one
# `__asm__ __volatile__` statement; the header holds its guard and the three
# definitions, nothing else.
QUADWORD_MACROS = {
    'QuadwordCopy': (('destination', 'source'),
                     ['lq $2, 0(%1)\\n', 'sq $2, 0(%0)\\n'], '"$2", "memory"'),
    'QuadwordZero': (('destination',), ['sq $0, 0(%0)\\n'], '"memory"'),
    'QuadwordCopyMatrix': (('destination', 'source'),
                           [f'{op} $2, {off}(%{n})\\n' for off in (0, 16, 32, 48)
                            for op, n in (('lq', 1), ('sq', 0))], '"$2", "memory"'),
}


def _quadword_definition(name):
    params, lines, clobbers = QUADWORD_MACROS[name]
    inputs = ', '.join(f'"r" ({p})' for p in params)
    body = ' '.join(f'"{line}"' for line in lines)
    return f'#define {name}({", ".join(params)}) __asm__ __volatile__( {body} : : {inputs} : {clobbers})'


QUADWORD_HEADER_LINES = ['#ifndef QUADWORD_H', '#define QUADWORD_H'] + \
    [_quadword_definition(n) for n in QUADWORD_MACROS] + ['#endif']


def quadword_header_ok(text):
    """(ok, reason): include/quadword.h, exactly its guard and the three
    quadword transfer macro definitions (line continuations folded)."""
    code = re.sub(r'/\*.*?\*/|//[^\n]*', ' ', text, flags=re.S).replace('\\\n', ' ')
    lines = [re.sub(r'\s+', ' ', line).strip() for line in code.splitlines()]
    lines = [re.sub(r'\(\s+', '(', re.sub(r'\s+\)', ')', line)) if line.startswith('#define Q') else line
             for line in lines if line]
    want = [re.sub(r'\(\s+', '(', re.sub(r'\s+\)', ')', line)) for line in QUADWORD_HEADER_LINES]
    if lines != want:
        return False, 'not the quadword.h macro definitions'
    return True, None


def _balanced_args(text, i):
    """Arguments of the call whose '(' is at text[i]: (args, index after ')')."""
    depth, args, cur, j = 0, [], '', i
    while j < len(text):
        c = text[j]
        if c == '(':
            depth += 1
            if depth > 1:
                cur += c
        elif c == ')':
            depth -= 1
            if depth == 0:
                args.append(cur.strip())
                return args, j + 1
            cur += c
        elif c == ',' and depth == 1:
            args.append(cur.strip())
            cur = ''
        else:
            cur += c
        j += 1
    return None, None


def quadword_admission_text(text):
    """Admission text of a function body: every include/quadword.h statement
    `QuadwordCopy(D, S);`, `QuadwordZero(D);`, `QuadwordCopyMatrix(D, S);` is
    replaced by the exact inline asm statement it expands to, so the function's
    own class admission (ee-mmi, hardware, ...) validates the same block as
    before the macros existed. Nothing else changes."""
    out, pos = [], 0
    for m in re.finditer(r'\b(QuadwordCopyMatrix|QuadwordCopy|QuadwordZero)\s*\(', text):
        if m.start() < pos:
            continue
        name = m.group(1)
        args, end = _balanced_args(text, m.end() - 1)
        params, lines, clobbers = QUADWORD_MACROS[name]
        if args is None or len(args) != len(params):
            continue
        inputs = ', '.join(f'"r" ({a})' for a in args)
        body = ' '.join(f'"{line}"' for line in lines)
        out.append(text[pos:m.start()])
        out.append(f'__asm__ __volatile__({body} : : {inputs} : {clobbers})')
        pos = end
    out.append(text[pos:])
    return ''.join(out)


def macro_include_ok(text):
    """(ok, reason): only the scaffold macros, guards and comments at top level."""
    depth, names = 0, []
    for raw in text.splitlines():
        s = _strip_comment(raw).strip()
        if not s:
            continue
        m = re.match(r'\.macro\s+([A-Za-z_.$][\w.$]*)', s)
        if m:
            depth += 1
            names.append(m.group(1))
            continue
        if s.startswith('.endm'):
            depth -= 1
            continue
        if depth:
            continue
        if re.fullmatch(r'\.(?:ifndef|ifdef)\s+\w+|\.endif|\.else|\.internal\s+\w+|\.set\s+\w+\s*,\s*\d+', s):
            continue
        return False, f'top-level content in the macro include: {s[:60]}'
    if depth:
        return False, 'unbalanced .macro/.endm'
    extra = sorted(set(names) - SCAFFOLD_MACROS)
    if extra:
        return False, f'macros outside the scaffold set (a macro can shadow a mnemonic): {extra}'
    return True, None


def asm_data_only(text):
    """(ok, reason): an INCLUDE_RODATA payload holds data, never code."""
    section = None
    for number, raw in enumerate(text.splitlines(), 1):
        line = _strip_comment(raw).strip()
        if not line or re.fullmatch(r'"?[A-Za-z_.$][\w.$]*"?:', line):
            continue
        if line.startswith('.'):
            directive = line.split(None, 1)[0]
            if directive in ('.section', '.text', '.data', '.rdata', '.sdata', '.rodata', '.bss', '.sbss',
                             '.lit4', '.lit8'):
                name = line.split()[1] if directive == '.section' else directive
                if _SECTION_ALIASES.get(name, name) in TEXT_SECTIONS:
                    return False, f'line {number}: payload switches to {name}'
                section = name
                continue
            if directive in DATA_DIRECTIVES or directive in ('.align', '.globl', '.global', '.type', '.size',
                                                             '.set', '.local', '.weakext', '.extern'):
                continue
            return False, f'line {number}: unsupported directive {directive}'
        return False, f'line {number}: instruction in a data payload: {line[:40]}'
    del section
    return True, None


# ---------------------------------------------------------------------------
# Admission (tools/match.py)
# ---------------------------------------------------------------------------

def admission(cls, tu_class=None, *, unit=None, tu_id=None, function=None):
    """The admission for a function of class `cls` inside a TU of class `tu_class`.

    `tu_class` matters for exactly one pair. A `hardware` function is admitted
    as the TU's prelude plus its own body (see `audit_c`), and in a TU declared
    `native-ti` that prelude carries the file-scope
    `typedef unsigned int N __attribute__((mode(TI)));`, which plain
    `match.hardware_c` refuses through `ordinary_c`. `match.hardware_native_ti_c`
    is the two already-authorized admissions composed -- the calibrated hardware
    table for the inline statements, the native-TI grammar (itself `ordinary_c`
    plus that one typedef and several extra refusals) for everything else -- and
    it admits no construct that one of the two routes does not admit on its own.
    The composition is reachable ONLY here: a `hardware` function in an
    `ordinary` TU still gets `hardware_c`, so the typedef stays refused there,
    and a `native-ti` function never gains inline assembly (`audit_c` refuses an
    `#APP` block in any non-`hardware` function, and `native_ti.source` refuses
    the `asm` token). See docs/native-ti.md.
    """
    from tools import match
    if cls == 'ordinary':
        return match.ordinary_c
    if cls == 'hardware':
        # User decision 2026-10-22: one COP1 load inside the original particle
        # scale block, for this identity only. All other COP1 spelling and FPR
        # clobbers keep the ordinary hardware refusal.
        if (unit, tu_id, function) == ('main', 'main/tu211', 'sefCreateParticle'):
            return (match.sef_particle_scale_native_ti_c if tu_class == 'native-ti'
                    else match.sef_particle_scale_c)
        # User decision 2026-10-28: StartOfScan's hand-written VU0 setup block may
        # open with `lui $2,0x3e00` feeding `ctc2 $2,$vi21` (0.125f into I), for this
        # identity only; every other scalar line keeps the ordinary hardware refusal.
        if (unit, tu_id, function) == VU0_I_IMMEDIATE_IDENTITY:
            return (match.vu0_i_immediate_native_ti_c if tu_class == 'native-ti'
                    else match.vu0_i_immediate_c)
        # User decision 2026-10-18: compose the existing MMI and COP2 guards
        # only for these two original NML identities, not for hardware generally.
        if (unit, tu_id, function) in {
                ('main', 'main/tu106', '_WeightToGlobalVec'),
                ('main', 'main/tu108', '_WeightToGlobalPlaceVec')}:
            return match.mmi_cop2_native_ti_c if tu_class == 'native-ti' else match.mmi_cop2_c
        return match.hardware_native_ti_c if tu_class == 'native-ti' else match.hardware_c
    if cls == 'runtime-verbatim':
        return match.runtime_verbatim_c
    if cls == 'native-ti':
        from tools import native_ti
        return native_ti.source
    if cls == 'gnu-nested-function':
        # This admits evidenced GNU nested definitions project-wide. It does not alter the TU contract or
        # the ordinary admission used for file-scope text and headers.
        return match.gnu_nested_c
    if cls == 'ee-mmi':
        # Constrained inline EE MMI (user decision 2026-10-10, docs/ee-mmi.md): the closed MMI grammar
        # for the inline blocks, ordinary (or native-TI) C for everything else. Not a precedent for
        # non-MMI assembly; the COP2/VU route stays `hardware`.
        return match.mmi_native_ti_c if tu_class == 'native-ti' else match.mmi_c
    if cls == 'ee-asm-leaf':
        # Original hand-written block-copy loop as C with asm (user decision 2026-10-10,
        # docs/ee-hand-asm.md): a leaf whose only statement is one admitted loop block.
        return match.asm_leaf_native_ti_c if tu_class == 'native-ti' else match.asm_leaf_c
    if cls == 'ee-fpu':
        # Inline COP1 transfer/convert blocks (user decision 2026-10-10, docs/ee-hand-asm.md).
        return match.fpu_native_ti_c if tu_class == 'native-ti' else match.fpu_c
    if cls == 'ee-vu-clip':
        # Original hand-written VU0 lane-test block with its set/clear join as the whole body
        # (user decision 2026-10-11 on main/tu109, docs/ee-hand-asm.md).
        if (unit, tu_id, function) == ('main', 'main/tu210', 'sefClipViewVolumeA'):
            return match.sef_vu_clip_native_ti_c if tu_class == 'native-ti' else match.sef_vu_clip_c
        return match.vu_clip_native_ti_c if tu_class == 'native-ti' else match.vu_clip_c
    if cls == 'ee-assert-break':
        # The assert macros' bare debugger trap, spelled only as DEBUG_BREAK() of
        # include/debug_break.h (user decision 2026-10-11 on main/tu121, docs/ee-hand-asm.md).
        return match.assert_break_native_ti_c if tu_class == 'native-ti' else match.assert_break_c
    if cls == 'ee-retaddr-store':
        # One `sw $31,0($sp)` return-address store statement at entry, for main/tu273
        # UnduCheck only (user decision 2026-10-27, docs/ee-hand-asm.md); never a precedent.
        if (unit, tu_id, function) != RETADDR_STORE_IDENTITY:
            def refused(*_args, **_kwargs):
                raise ValueError('ee-retaddr-store is admitted only for %s %s' % RETADDR_STORE_IDENTITY[1:])
            return refused
        return match.retaddr_store_native_ti_c if tu_class == 'native-ti' else match.retaddr_store_c
    if cls == 'ee-asm-sort':
        # The whole-body hand-written bubble sort (lq/sq swap) of main/tu106 AlphaGroupSort
        # only (user decision 2026-10-28, docs/ee-hand-asm.md); never a precedent.
        if (unit, tu_id, function) != ASM_SORT_IDENTITY:
            def refused(*_args, **_kwargs):
                raise ValueError('ee-asm-sort is admitted only for %s %s' % ASM_SORT_IDENTITY[1:])
            return refused
        return match.asm_sort_native_ti_c if tu_class == 'native-ti' else match.asm_sort_c
    raise AuditError(f'unknown source class {cls!r}')


def packed_enum_admission_items(unit, tu_id, tu, tu_text):
    """({item.start: admission text}, [typedef names]) for PACKED_ENUM_IDENTITY.

    Masks, for admission only (the compiled source is untouched), the single
    `__attribute__((packed))` of each whole TU-local `typedef enum` declaration
    of main/tu125 when that TU defines GameMovieInit as C (user decision
    2026-10-28). Every other attribute, declaration, TU and header stays under
    the ordinary refusal."""
    if (unit, tu_id) != PACKED_ENUM_IDENTITY[:2] or not any(
            b.key == PACKED_ENUM_IDENTITY[2] and b.state == 'c' for b in tu.blocks):
        return {}, []
    lexical_tokens = r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|/\*.*?\*/|//[^\n]*'
    items, names = {}, []
    for item in tu.free_items():
        if item.kind != 'decl' or item.conditional:
            continue
        text = item.text(tu_text)
        lexical = re.sub(lexical_tokens, lambda m: ''.join('\n' if c == '\n' else ' ' for c in m[0]),
                         text, flags=re.S)
        found = _PACKED_ENUM_TYPEDEF.fullmatch(lexical)
        if found is None or not _PACKED_ENUM_BODY.fullmatch(found['body']) or re.search(
                r'\b(?:asm|__asm|__asm__|__attribute|__attribute__|_Pragma)\b', found['body']):
            continue
        start, end = found.span('attribute')
        items[item.start] = text[:start] + ' ' * (end - start) + text[end:]
        names.append(found['name'])
    return items, names


# The one admitted type attribute (user decision 2026-11-01, AGENTS.md):
# include/shared.h's `typedef Vector4 AlignedVector4 __attribute__((aligned(16)));`,
# the quadword-aligned SDK vector (libvu0.h sceVu0FVECTOR is aligned(16)).
_ALIGNED_VECTOR4_TYPEDEF = 'typedef Vector4 AlignedVector4 __attribute__((aligned(16)));'
# and its doubleword sibling (user 2026-11-02): 8-aligned vectors copied ld/sd.
_DOUBLEWORD_ALIGNED_VECTOR4_TYPEDEF = 'typedef Vector4 DoublewordAlignedVector4 __attribute__((aligned(8)));'
_ADMITTED_VECTOR_TYPEDEFS = ((_ALIGNED_VECTOR4_TYPEDEF, '__attribute__((aligned(16)))'),
                             (_DOUBLEWORD_ALIGNED_VECTOR4_TYPEDEF, '__attribute__((aligned(8)))'))


def aligned_vector4_admission_text(full, text, unit_dir):
    """Admission text of a header: for include/shared.h (the repository file or
    the unit's staged copy) the attribute of each single exact admitted vector
    typedef line (AlignedVector4, DoublewordAlignedVector4) is blanked; every
    other attribute, header and spelling stays under the ordinary refusal. The
    compiled header is untouched."""
    full = Path(full).resolve()
    homes = {(ROOT / 'include' / 'shared.h').resolve(), (Path(unit_dir) / 'include' / 'shared.h').resolve()}
    if full not in homes:
        return text
    lines = text.split('\n')
    for typedef, attribute in _ADMITTED_VECTOR_TYPEDEFS:
        hits = [i for i, line in enumerate(lines) if line.strip() == typedef]
        if len(hits) == 1:
            lines[hits[0]] = lines[hits[0]].replace(attribute, ' ' * len(attribute))
    return '\n'.join(lines)


def included_names(text):
    return re.findall(r'^\s*#\s*include\s*["<]([^">]+)[">]', text, re.M)


def asm_contract_problems(path):
    """Read an asm contract and list what is wrong with it, never raising.

    The asm contract is OPTIONAL review documentation (user direction
    2026-09-27): the bytes are proven by the whole-file gate and this audit, and
    the automatic guard against illegitimate inline assembly is the source-class
    admission (`admission`, tools/match.py `hardware_c`), which stays enforced.
    Returns (data or None, [problem, ...]); `data` is None only when the file is
    missing or not a JSON object.  Callers record the problems as notes.
    """
    problems = []
    p = Path(path)
    if not p.is_file():
        return None, [f'asm contract {rel(p) if p.is_absolute() else p} is missing']
    try:
        data = json.loads(p.read_text())
    except (OSError, ValueError) as e:
        return None, [f'asm contract {rel(p.resolve())} is unreadable: {e}']
    if not isinstance(data, dict):
        return None, [f'asm contract {rel(p.resolve())} is not a JSON object']
    missing = [k for k in ASM_CONTRACT_FIELDS if not (isinstance(data.get(k), str) and data[k].strip())]
    if missing:
        problems.append(f'asm contract lacks {sorted(missing)}')
    evidence = data.get('evidence')
    if not (isinstance(evidence, dict) and set(evidence) == {'path', 'sha256'}):
        problems.append('asm contract evidence must be {path, sha256}')
    else:
        target = ROOT / str(evidence['path'])
        if not target.is_file() or sha256_file(target) != evidence['sha256']:
            problems.append('asm contract evidence changed or missing')
    lines = data.get('top_level_asm') or []
    if not isinstance(lines, list) or any(not isinstance(x, str) for x in lines):
        problems.append('asm contract top_level_asm must be a list of exact assembly lines (ignored)')
        data = dict(data, top_level_asm=[])
    return data, problems


def check_asm_contract(path):
    """Strict form of `asm_contract_problems` (raises on the first problem).

    Kept for callers that want a hard validation of a document they are about to
    write; the audit itself only uses the non-raising form.
    """
    data, problems = asm_contract_problems(path)
    if problems:
        raise AuditError(problems[0])
    return data


def load_asm_contract(path, notes, label=None):
    """Non-raising load for the audit: problems go to `notes`, never to a check."""
    data, problems = asm_contract_problems(path)
    for p in problems:
        notes.append(f'{label or rel(Path(path).resolve())}: {p}')
    return data


def owned_header_source_class(full, default, root, metadata):
    """Select only a pinned, defining-owner native-TI declaration header.

    This changes no body class or grammar. Unlisted headers retain their
    importing TU class; listed text still passes native_ti.source in full.
    """
    try:
        key = Path(full).resolve().relative_to(Path(root).resolve()).as_posix()
    except ValueError:
        return default, {}
    staged_copy = {}
    registered = metadata.get('owned_header_classes') or {}
    parts = Path(key).parts
    if parts[:3] == ('build', 'main', 'include'):
        canonical_key = (Path('include') / Path(*parts[3:])).as_posix()
        if canonical_key in registered:
            staged_copy = dict(path=key, canonical_header=canonical_key)
            key = canonical_key
    entry = registered.get(key)
    if entry is None:
        return default, {}
    if metadata.get('schema') != SOURCE_CLASSES_SCHEMA or entry.get('source_class') != 'native-ti':
        raise ValueError('owned header dialect must be the registered native-TI class')
    owner = entry.get('owner_tu')
    if ((metadata.get('tus') or {}).get(owner) or {}).get('source_class') != 'native-ti':
        raise ValueError('owned native-TI header lacks a native-TI defining TU')
    rows = json.loads((Path(root) / 'config/tu-build.json').read_text()).get('tus') or []
    row = next((r for r in rows if r.get('id') == owner), None)
    if row is None:
        raise ValueError('owned native-TI header has no original TU identity')
    source = Path(row['path'])
    expected = (Path('include') / source.parent.relative_to('src') / source.with_suffix('.h').name).as_posix()
    if key != expected:
        raise ValueError('owned native-TI header is not its normal defining-owner header')
    origin = entry.get('source_owner') or {}
    if origin.get('path') not in (source.as_posix(), source.with_suffix('.h').as_posix()):
        raise ValueError('owned native-TI declaration source is outside its defining TU')
    # The SHA-256 pins of the header, its source owner and its type export are
    # informative only (user decision 2026-11-01): a reorganized header keeps its
    # registered native-TI class and its text is still validated in full by the
    # strict native-TI grammar.
    source_path = Path(root) / origin['path']
    stale = []
    if (sha256_file(source_path) != origin.get('sha256')
            or sha256_file(Path(root) / key) != entry.get('header_sha256')
            or sha256_file(full) != entry.get('header_sha256')):
        stale.append('owned native-TI source/header pin changed')
    exports = json.loads((Path(root) / 'config/header-exports.json').read_text()).get('exports') or []
    names = set(entry.get('names') or [])
    if not names or not any(r.get('tu') == owner and names <= set(r.get('names') or [])
            and (r.get('source_owner') or {}).get('path') == origin['path']
            and (r.get('source_owner') or {}).get('sha256') == origin['sha256'] for r in exports):
        stale.append('owned native-TI header lacks its existing source-bound type export')
    return 'native-ti', dict(owner_tu=owner, source_owner=origin, header_sha256=entry['header_sha256'],
                             names=sorted(names), staged_copy=staged_copy, informative_stale_pins=stale)


def emitted_c_category(cls, source_statements, emitted_blocks):
    """Report admitted inlined helper ASM as ASM, never pure C coverage."""
    return ('exact_c_with_asm' if cls in INLINE_ASM_CLASSES
            and (source_statements or emitted_blocks) else 'exact_c')


# Source transport, not another ASM class. These two exact definitions implement
# already-admitted MMI operations in the three original JNT consumers. A name alone
# never qualifies: source, complete use, actual emission and absence of an
# out-of-line object must all be proven before free-text admission can omit it.
_JNT_MMI_HELPERS = {
    'jntCopyMatrix': ('71eed05bd729c11448457ede54d258ce16698949055dfe0e8ce4d3c7457e140e', (0, 16, 32, 48)),
    'jntCopyMatrixRow': ('87cafe1229e938bbb3ee1df1914ec5f318a62677a3447774f53c5f01669d61ad', (0,)),
}
_JNT_MMI_CONSUMERS = {
    'JNT_interpolate': (0x00313350, 1076, {'jntCopyMatrix': 4, 'jntCopyMatrixRow': 1}),
    'JNT_setMDLMatrix': (0x00313950, 320, {'jntCopyMatrix': 1}),
    'JNT_resetMatrix': (0x003148a8, 396, {'jntCopyMatrix': 2}),
}


def verified_jnt_mmi_helpers(unit, tu_id, tu_rel, tu, pre, asm, obj, classes,
                             tu_class, original_functions, original_sha256):
    """Return only fully proven JNT helper transports; never change compiled text.

    The closed token pins include signatures, static/inline, templates, operand
    expressions and clobbers. Both raw and cpp source must expose the same direct
    statement calls. All consumers, not merely this audit's claims, are checked.
    Unknown helpers stay in ordinary/native-TI free text as before.
    """
    names = set(_JNT_MMI_HELPERS)
    if ((unit, tu_id) != ('main', 'main/tu261') or
            not any(b.name in names for b in tu.blocks)):
        return set(), None

    def require_transport(condition, detail):
        if not condition:
            raise AuditError('JNT MMI helper transport: ' + detail)

    def token_pin(text):
        return hashlib.sha256('\0'.join(tu_edit.c_tokens(text)).encode()).hexdigest()

    require_transport(tu_class in ('ordinary', 'native-ti'), 'unsupported TU source class')
    require_transport(tu_rel == '../../src/main/jnt.c' and original_sha256 ==
                      '412d448de31505c03ad474cc63d500b3b4f8c48d49d32bd24c89fe5b21af0fb2',
                      'original revision or defining TU source path differs')
    cpp = tu_edit.parse(pre.text)
    sources = []
    helper_evidence = {}
    expected_calls = {n: collections.Counter(spec[2]) for n, spec in _JNT_MMI_CONSUMERS.items()}
    for label, source in (('raw', tu), ('cpp', cpp)):
        lexical = tu_edit.lexical(source.text)
        require_transport('\\' not in lexical and not any(
            escape in lexical for escape in ('??', '%:', '<%', '%>', '<:', ':>')),
            label + ' source has an executable escape/alternative punctuator')
        helper_blocks = {}
        for name, (digest, _offsets) in _JNT_MMI_HELPERS.items():
            found = [b for b in source.blocks if b.name == name]
            require_transport(len(found) == 1, label + ': helper definition is absent/ambiguous: ' + name)
            block = found[0]
            text = source.block_text(block)
            require_transport(block.state == 'c' and block.role == 'helper' and block.static
                              and not block.conditional and token_pin(text) == digest,
                              label + ': helper signature/body tokens differ: ' + name)
            require_transport('register' not in tu_edit.c_tokens(text),
                              label + ': register storage/binding is forbidden: ' + name)
            require_transport(admission('ee-mmi', tu_class)(text) == 1,
                              label + ': helper is not exactly one admitted MMI statement: ' + name)
            helper_blocks[name] = block
            if label == 'cpp':
                raw = tu.by_name[name]
                where, line = pre.attribute(block.core_start)
                require_transport(where == tu_rel and
                                  tu_edit.line_of(tu.text, raw.core_start) <= line <=
                                  tu_edit.line_of(tu.text, raw.end),
                                  'cpp helper definition is not the audited TU source: ' + name)
            else:
                helper_evidence[name] = dict(token_sha256=digest,
                    source_sha256=hashlib.sha256(text.encode('utf-8', 'surrogateescape')).hexdigest(),
                    lines=[tu_edit.line_of(tu.text, block.core_start), tu_edit.line_of(tu.text, block.end)])

        calls = {n: collections.Counter() for n in expected_calls}
        sites = []
        for match in re.finditer(r'\b(?:jntCopyMatrix|jntCopyMatrixRow)\b', lexical):
            name, offset = match.group(), match.start()
            if any(b.core_start <= offset < b.core_end for b in helper_blocks.values()):
                continue  # The complete pinned definitions were independently admitted above.
            owners = [b for b in source.blocks if b.core_start <= offset < b.core_end]
            require_transport(len(owners) == 1, label + ': helper reference outside a mapped consumer')
            caller = owners[0]
            require_transport(caller.state == 'c' and caller.name in expected_calls
                              and not caller.conditional and classes.get(caller.name, tu_class) == 'ee-mmi',
                              label + ': helper consumer is not a declared mapped ee-mmi function: ' + caller.name)
            raw = tu.by_name.get(caller.name)
            require_transport(raw is not None and raw.role == 'function' and raw.key == caller.name,
                              label + ': consumer is not an original mapped function: ' + caller.name)
            opening = lexical.find('{', caller.core_start, caller.core_end)
            before = lexical[opening + 1:offset].rstrip()
            after = lexical[match.end():caller.core_end].lstrip()
            require_transport(offset > opening and (not before or before[-1] in ';{}')
                              and after.startswith('('),
                              label + ': helper use is not a direct statement call: ' + name)
            depth, closing = 0, None
            for index, char in enumerate(after):
                depth += (char == '(') - (char == ')')
                if depth == 0:
                    closing = index
                    break
            require_transport(closing is not None and after[closing + 1:].lstrip().startswith(';'),
                              label + ': helper call has an unsupported expression/indirect use: ' + name)
            calls[caller.name][name] += 1
            sites.append(dict(helper=name, consumer=caller.name, line=tu_edit.line_of(source.text, offset)))
        require_transport(calls == expected_calls, label + ': complete direct-call multiplicity differs')
        sources.append(sites)

    for name, (va, size, _calls) in _JNT_MMI_CONSUMERS.items():
        raw, expanded = tu.by_name.get(name), cpp.by_name.get(name)
        require_transport(raw is not None and expanded is not None and
                          tu_edit.c_tokens(tu.block_text(raw)) == tu_edit.c_tokens(cpp.block_text(expanded)),
                          'raw/cpp consumer tokens differ: ' + name)
        require_transport(not {'register', '__attribute', '__attribute__'} &
                          set(tu_edit.c_tokens(tu.block_text(raw))),
                          'consumer register storage/binding or attribute is forbidden: ' + name)
        where, line = pre.attribute(expanded.core_start)
        require_transport(where == tu_rel and tu_edit.line_of(tu.text, raw.core_start) <= line <=
                          tu_edit.line_of(tu.text, raw.end), 'cpp consumer ownership differs: ' + name)
        original = [s for s in original_functions if s['name'] == name]
        require_transport(len(original) == 1 and int(original[0]['va'], 16) == va
                          and original[0]['size'] == size and original[0]['binding'] == 1
                          and original[0]['type'] == STT_FUNC,
                          'original consumer identity differs: ' + name)
        emitted = [s for s in obj.functions() if s['name'] == name]
        require_transport(len(emitted) == 1 and emitted[0]['size'] == size
                          and emitted[0]['binding'] == 1 and emitted[0]['section'] in TEXT_SECTIONS,
                          'emitted consumer identity differs: ' + name)
        require_transport(emitted[0]['value'] == va - 0x0030ec08,
                          'emitted consumer is not at its original TU offset: ' + name)

    # This is additional evidence, not an allow-list entry in any existing
    # emitted-helper/extra-function/object-symbol gate. Those gates stay intact.
    require_transport(not any(any(h in s['name'] for h in names) for s in obj.symbols),
                      'object retains a helper/clone/alias symbol')
    require_transport(not any(any(h in r['name'] for h in names) for r in asm.regions),
                      'compiler emits a standalone helper/clone region')
    require_transport(not any(any(h in row[3] for h in names) for row in obj.relocations()),
                      'object relocation refers to a helper/clone/alias')

    def emitted_shape(app):
        lines = [text.strip() for _line, text in app['lines'] if text.strip()]
        for name, (_digest, offsets) in _JNT_MMI_HELPERS.items():
            if len(lines) != len(offsets) * 2:
                continue
            bases = {}
            for index, (op, offset) in enumerate(
                    entry for offset in offsets for entry in (('lq', offset), ('sq', offset))):
                match = re.fullmatch(r'(lq|sq)\s+\$2,\s*(\d+)\(\$([0-9]+|sp)\)', lines[index])
                if (not match or match[1] != op or int(match[2]) != offset or match[3] == '2'
                        or (match[3] != 'sp' and not 0 <= int(match[3]) <= 31)
                        or (op in bases and bases[op] != match[3])):
                    break
                bases[op] = match[3]
            else:
                return name
        return None

    for region in asm.regions:
        if region['name'] not in expected_calls:
            require_transport(not any(emitted_shape(app) in names for app in region['app']),
                              'copy APP escapes the registered consumers: ' + region['name'])
    emission = {}
    for name, expected in expected_calls.items():
        regions = [r for r in asm.regions if r['name'] == name]
        require_transport(len(regions) == 1 and regions[0]['file'] == tu_rel
                          and regions[0]['section'] in TEXT_SECTIONS and not regions[0]['includes'],
                          'consumer compiler region ownership differs: ' + name)
        blocks = regions[0]['app']
        shapes = collections.Counter(emitted_shape(app) for app in blocks)
        require_transport(shapes == expected, 'emitted APP template/multiplicity differs: ' + name)
        emission[name] = dict(helpers=dict(shapes), app_lines=[app['line'] for app in blocks],
                             category=emitted_c_category('ee-mmi', 0, len(blocks)))
    return names, dict(schema='jnt-inline-mmi-transport/1', helpers=helper_evidence,
                       raw_calls=sources[0], preprocessed_calls=sources[1], consumers=emission,
                       source_sha256=hashlib.sha256(tu.text.encode('utf-8', 'surrogateescape')).hexdigest(),
                       preprocessed_sha256=hashlib.sha256(pre.text.encode()).hexdigest(),
                       object_sha256=hashlib.sha256(obj.data).hexdigest(),
                       original_sha256=original_sha256,
                       grants_helper_credit=False, grants_exactness=False)


def published_source_classes(tu_id):
    """The source class a published TU's functions were accepted under.

    `--function-class`/`--asm-contract` state a class at the moment a worker
    submits; nothing of that survives publication, so a fresh audit of the whole
    TU would read an `exact_c_with_asm` function as ordinary C and refuse its
    inline assembly.  `config/tu/source-classes.json` is where the class
    survives: per TU, and per function inside it, the class and the asm contract
    it was accepted with, pinned by SHA-256.  The audit reads it whenever it
    knows which TU it is auditing, so a whole-TU audit reproduces the classes
    without anyone having to restate them, and a later worker who did not
    allocate the function cannot lose them.

    Returns (tu_class, {name: class}, {name: contract path}, provenance) and
    raises when the file or the class is wrong: an unreadable declaration is
    never silently treated as "ordinary".  Asm contracts are optional review
    documentation (user direction 2026-09-27): an absent, lost or re-hashed one
    is reported in the provenance (`asm_contracts_missing`,
    `asm_contract_notes`) and never raises.
    """
    empty = (None, {}, {}, None)
    path = ROOT / SOURCE_CLASSES_FILE
    if not tu_id or not path.is_file():
        return empty
    data = json.loads(path.read_text())
    if data.get('schema') != SOURCE_CLASSES_SCHEMA:
        raise AuditError(f'{SOURCE_CLASSES_FILE} is not schema {SOURCE_CLASSES_SCHEMA}')
    entry = (data.get('tus') or {}).get(tu_id)
    prov = dict(path=SOURCE_CLASSES_FILE, sha256=sha256_file(path), declared=bool(entry))
    discovered = tu_edit.gnu_nested_specs(ROOT, tu_id)
    if not entry and not discovered:
        return None, {}, {}, prov
    entry = entry or {}

    # The asm contract is optional review documentation that never blocks (user
    # direction 2026-09-27): an absent, lost, unsafe or re-hashed contract is
    # recorded here as a note and the audit goes on.  The source class itself
    # (and hence the hardware admission) stays mandatory and validated.
    missing_contracts = {}
    contract_notes = []

    def one(where, spec):
        cls = spec.get('source_class')
        if cls not in CLASSES:
            raise AuditError(f'{SOURCE_CLASSES_FILE} {where}: invalid source_class {cls!r}')
        contract = spec.get('asm_contract')
        key = where[len(tu_id) + 1:] if where != tu_id else '*'
        if contract is None:
            if cls == 'hardware':
                contract_notes.append(f'{where}: hardware class declared without an asm_contract '
                                      '(optional review documentation)')
            return cls, None
        try:
            if not isinstance(contract, dict):
                raise AuditError('asm_contract is not {path, sha256}')
            rel_path = safe_relative(contract.get('path'), 'asm contract')
        except (AuditError, TypeError, ValueError) as e:
            contract_notes.append(f'{where}: unusable asm_contract declaration ignored: {e}')
            return cls, None
        target = ROOT / rel_path
        if not target.is_file():
            # A contract whose file was lost (e.g. the attempt directory it lived
            # in was cleaned up).  Its function's bytes are still verified: record
            # it and waive it visibly.
            missing_contracts[key] = rel_path
            return cls, None
        digest = sha256_file(target)
        if digest != contract.get('sha256'):
            contract_notes.append(f'{where}: {rel_path} is sha256 {digest[:12]}..., the declaration pins '
                                  f'{str(contract.get("sha256"))[:12]}... (recorded, not blocking)')
        return cls, rel_path

    tu_class, tu_contract = (one(tu_id, entry) if entry.get('source_class') else (None, None))
    if tu_class == 'gnu-nested-function':
        raise AuditError(f'{SOURCE_CLASSES_FILE} {tu_id}: gnu-nested-function is a function-level class '
                         'attached to an enclosing function, not a whole-TU class')
    classes, contracts, nested_functions = {}, {}, {}
    for name, spec in sorted((entry.get('functions') or {}).items()):
        cls, contract = one(f'{tu_id} {name}', spec)
        classes[name] = cls
        if contract:
            contracts[name] = contract
        nested = spec.get('nested_functions')
        if nested is not None:
            if cls != 'gnu-nested-function':
                raise AuditError(f'{SOURCE_CLASSES_FILE} {tu_id} {name}: nested_functions requires '
                                 'source_class gnu-nested-function')
            nested_functions[name] = nested
        elif cls == 'gnu-nested-function':
            if name not in discovered:
                raise AuditError(f'{SOURCE_CLASSES_FILE} {tu_id} {name}: gnu-nested-function needs '
                                  'an evidenced nested_functions mapping')
    for name, spec in discovered.items():
        nested_functions.setdefault(name, spec.get('nested_functions'))
    seen = set()
    for name, nested in nested_functions.items():
        if not isinstance(nested, list) or not nested:
            raise AuditError(f'{tu_id} {name}: nested_functions must be a nonempty list')
        lexical_names = {name} | {item.get('source_name') for item in nested if isinstance(item, dict)}
        for item in nested:
            required = ('source_name', 'original_name', 'parent', 'binding', 'type', 'va', 'size')
            if not isinstance(item, dict) or any(k not in item for k in required):
                raise AuditError(f'{tu_id} {name}: incomplete nested function identity')
            if (not isinstance(item['source_name'], str) or not re.fullmatch(r'[A-Za-z_]\w*', item['source_name'])
                    or not isinstance(item['original_name'], str) or not item['original_name']
                    or not isinstance(item['parent'], str)
                    or item['parent'] not in lexical_names or item['parent'] == item['source_name']
                    or item['binding'] != 'LOCAL' or item['type'] != 'FUNC'
                    or not isinstance(item['va'], str) or not re.fullmatch(r'0x[0-9a-f]{8}', item['va'])
                    or type(item['size']) is not int or item['size'] <= 0):
                raise AuditError(f'{tu_id} {name}: invalid nested function identity or lexical parent')
            if item['original_name'] in seen:
                raise AuditError(f'{tu_id}: duplicate nested identity {item["original_name"]}')
            seen.add(item['original_name'])
    discovery_path = ROOT / tu_edit.GNU_NESTED_REGISTRY
    if discovery_path.is_file() and discovered:
        prov['nested_discovery'] = dict(path=tu_edit.GNU_NESTED_REGISTRY, sha256=sha256_file(discovery_path))
    prov.update(tu_source_class=tu_class, functions={n: classes[n] for n in sorted(classes)},
                asm_contracts={n: dict(path=contracts[n], sha256=sha256_file(ROOT / contracts[n]))
                               for n in sorted(contracts)})
    if missing_contracts:
        prov['asm_contracts_missing'] = dict(sorted(missing_contracts.items()))
    if contract_notes:
        prov['asm_contract_notes'] = contract_notes
    if nested_functions:
        prov['nested_functions'] = nested_functions
    if tu_contract:
        contracts[None] = tu_contract
    return tu_class, classes, contracts, prov


# ---------------------------------------------------------------------------
# Toolchain steps
# ---------------------------------------------------------------------------

def run(argv, cwd):
    env = dict(PATH='/usr/bin:/bin', LC_ALL='C')
    proc = subprocess.Popen([str(a) for a in argv], cwd=str(cwd), env=env, stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT, start_new_session=True)
    try:
        out, _ = proc.communicate(timeout=TIMEOUT)
    except subprocess.TimeoutExpired:
        os.killpg(proc.pid, signal.SIGKILL)
        proc.communicate()
        raise AuditError(f'timeout after {TIMEOUT}s: {argv[0]}')
    return proc.returncode, out.decode('utf-8', 'replace')


def registry_contract(contract_id):
    path = ROOT / 'config/toolchains-tu.json'
    if not path.is_file():
        raise AuditError('config/toolchains-tu.json is missing')
    data = json.loads(path.read_bytes())
    entry = (data.get('contracts') or {}).get(contract_id)
    if not entry:
        raise AuditError(f'contract {contract_id!r} is not in config/toolchains-tu.json')
    return entry, data


# KOS-MOS 2026-09-27: opt-in, mirrors tools/tu/toolchain.py; Opok to approve.
# A source-built tool of config/toolchain-identity.json (the la29 ee-as, the plain
# 2.9 ee-as, ps2dev binutils, dvp-as) is not bit-reproducible across host compilers.
# tools/tu/toolchain.py accepts such a rebuilt payload when XENO_TOOLCHAIN_ALLOW
# names its tool id; the audit does the same for the contract ASSEMBLER only, and
# every report says so (`contract_relaxed`, top level and in `summary`): the
# whole-file gate still decides whether the rebuilt assembler emits the original bytes.
TOOLCHAIN_IDENTITY = 'config/toolchain-identity.json'


def toolchain_allow():
    """The tool ids XENO_TOOLCHAIN_ALLOW names (the same set tools/tu/toolchain.py reads)."""
    return set(filter(None, os.environ.get('XENO_TOOLCHAIN_ALLOW', '').split(',')))


def toolchain_tool_id(payload):
    """The config/toolchain-identity.json tool id whose payload the contract path
    `payload` (under .work/toolchains) names, or None when no tool owns it."""
    try:
        tools = json.loads((ROOT / TOOLCHAIN_IDENTITY).read_bytes())['tools']
    except (OSError, ValueError, KeyError, TypeError):
        return None
    payload = Path(payload).as_posix()
    for tool_id, entry in tools.items():
        names = ([entry['path']] if 'path' in entry
                 else [f'{entry.get("dir")}/{name}' for name in entry.get('files', {})])
        if any(payload.endswith('/' + name) for name in names):
            return tool_id
    return None


def resolve_contract(args, tu_record):
    """The TU's compile contract from the manifests, with every pin verified.

    Returns (contract, issues, relaxed). An override that differs from the manifest and
    any unverifiable pin is an issue, i.e. a failing audit; `relaxed` lists the
    assembler pin mismatch XENO_TOOLCHAIN_ALLOW admitted ([{tool, path, expected, actual}]).
    """
    issues, relaxed = [], []
    manifest = (tu_record or {}).get('contract') or {}
    if not manifest.get('id'):
        raise AuditError('config/tu-build.json has no contract for this TU (pass --tu-id)')
    entry, registry = registry_contract(manifest['id'])
    include_asm = registry.get('include_asm') or {}
    compiler_dir = ROOT / entry['compiler']['dir']
    contract = dict(id=manifest['id'], source='config/tu-build.json + config/toolchains-tu.json',
                    compiler=dict(dir=entry['compiler']['dir'], sha256=dict(entry['compiler']['sha256'])),
                    driver=entry.get('driver', 'ee-gcc'), driver_flags=list(entry.get('driver_flags', [])),
                    flags=list(entry['flags']),
                    assembler=dict(path=entry['assembler']['path'], sha256=entry['assembler']['sha256'],
                                   argv=entry['assembler'].get('argv', '-EL -m5900 -mabi=eabi <G> -I .'),
                                   flags=list(entry['assembler'].get('flags', []))),
                    include_asm=include_asm)
    if list(manifest.get('flags') or []) != contract['flags']:
        issues.append(f'config/tu-build.json flags {manifest.get("flags")} differ from contract '
                      f'{contract["id"]} flags {contract["flags"]}')
    if manifest.get('cc1_sha256') and manifest['cc1_sha256'] != contract['compiler']['sha256'].get('cc1'):
        issues.append('config/tu-build.json cc1 digest differs from the contract registry')
    if manifest.get('assembler_sha256') and manifest['assembler_sha256'] != contract['assembler']['sha256']:
        issues.append('config/tu-build.json assembler digest differs from the contract registry')
    for name, digest in contract['compiler']['sha256'].items():
        path = compiler_dir / name
        if not path.is_file() or sha256_file(path) != digest:
            issues.append(f'compiler payload pin mismatch: {rel(path)}')
    asm_path = ROOT / contract['assembler']['path']
    asm_found = sha256_file(asm_path) if asm_path.is_file() else None
    if asm_found != contract['assembler']['sha256']:
        # KOS-MOS 2026-09-27: opt-in, mirrors tools/tu/toolchain.py; Opok to approve.
        # Only the tool id XENO_TOOLCHAIN_ALLOW names is relaxed; a missing payload
        # or any other tool's mismatch fails the contract check as before.
        tool_id = toolchain_tool_id(contract['assembler']['path'])
        if asm_found and tool_id in toolchain_allow():
            relaxed.append(dict(tool=tool_id, path=rel(asm_path),
                                expected=contract['assembler']['sha256'], actual=asm_found))
        else:
            issues.append(f'assembler pin mismatch: {rel(asm_path)}')
    # Overrides are compared with the manifest contract, never trusted.
    override = {}
    if args.contract:
        override = json.loads(Path(args.contract).read_text())
    if args.cc_dir:
        override.setdefault('compiler', {})['dir'] = args.cc_dir
    if args.assembler:
        override.setdefault('assembler', {})['path'] = args.assembler
    if args.flags:
        override['flags'] = args.flags.split()
    if override:
        got_dir = (override.get('compiler') or {}).get('dir')
        if got_dir and Path(got_dir).resolve() != compiler_dir.resolve():
            issues.append(f'compiler directory override {got_dir} is not the contract compiler')
        if 'flags' in override and list(override['flags']) != contract['flags']:
            issues.append(f'flag override {override["flags"]} is not the contract {contract["flags"]}')
        got_as = (override.get('assembler') or {}).get('path')
        if got_as and Path(got_as).resolve() != asm_path.resolve():
            issues.append(f'assembler override {got_as} is not the contract assembler')
        argv = (override.get('assembler') or {}).get('argv')
        if argv and argv.split() != contract['assembler']['argv'].split():
            issues.append(f'assembler argv override {argv!r} is not the contract argv')
    contract['include_dirs'] = (args.include_dir or (override.get('include_dirs') if override else None)
                                or ['include'])
    contract['overrides'] = override or None
    return contract, issues, relaxed


def gflag(contract):
    g = [f for f in contract['flags'] if re.fullmatch(r'-G\d+', f)]
    if len(g) != 1:
        raise AuditError('the contract flags must carry exactly one -G flag')
    return g[0]


def assembler_argv(contract, out, src, extra_includes=()):
    spec = contract['assembler']['argv'].replace('<G>', gflag(contract)).split()
    argv = [ROOT / contract['assembler']['path'], *spec]
    for inc in extra_includes:
        argv += ['-I', str(inc)]
    return argv + ['-o', str(out), str(src)]


# ---------------------------------------------------------------------------
# The assembly step of the real build edge
# ---------------------------------------------------------------------------
#
# cpp and cc1 are spelled the same wherever a TU is built, but the **assembly**
# step is not.  A main C TU is assembled from a per-TU overlay directory that
# `tools/tu/cinc.py` fills first: it drops every `.extern SYM, N` line of the
# scaffold `.s` bodies the TU includes whose SYM the TU's own compiler output
# already declares or defines (tu-asmcompat review finding F3), and the overlay
# is searched before the unit files.  Assembling the same cc1 output from the
# unit directory instead -- the bare cpp/cc1/as sequence this audit used to run --
# produces a **different object**: a symbol whose scaffold `.extern` line the
# build dropped is entered in the symbol table where the C output declares it
# rather than where the scaffold line did, so `.symtab` and `.strtab` move and
# every symbol index in `.rel.text` moves with them.  No instruction changes and
# the link is unaffected, but the bytes are not identical, and
# `build_object_matches_audit` -- the check that binds the audited source to the
# object the whole-file gate actually links -- then failed on TUs whose source
# was exactly right.  Fourteen of main's sixty-three published C TUs were in
# that state (main/tu211 `sef` among them), and with them every
# `tools/worker.py` submission on those TUs, because `tu_citable_forms()`
# requires the audit's top-level `ok`.
#
# So the recipe is not assumed: the rule that produces `--obj` is read from the
# unit's own `build.ninja` and reproduced.  The table below mirrors
# `tools/tu/ninja_main.py` (rules `ccas`, `ccas_c`) and `tools/tu/cc_tu.sh`
# (rule `cc`); it is not a second source of truth, because the audit's object is
# still compared with the build's byte for byte, so a recipe that drifted from
# the generator would show up as a failure and can never mask a real mismatch.
# Nothing here normalises a difference away: the comparison is unchanged.
#
# The audit builds its **own** overlay from its **own** cc1 output instead of
# reusing the build's `build/cinc/<tu>`: the compile stays self-contained, so a
# stale or hand-edited overlay in the build tree can never certify anything, and
# the build's overlay is still validated -- transitively, because the two
# objects are compared.


def _ninja_document(unit_dir):
    """{'rules': {name: vars}, 'edges': {output: vars incl. 'rule'}} of build.ninja."""
    path = Path(unit_dir) / 'build.ninja'
    if not path.is_file():
        return None
    rules, edges, section = {}, {}, None
    for line in path.read_text(errors='replace').splitlines():
        if line[:1] in (' ', '\t'):
            if section is not None and '=' in line:
                key, _, value = line.strip().partition('=')
                section[key.strip()] = value.strip()
            continue
        match = re.match(r'rule (\S+)\s*$', line)
        if match:
            section = rules.setdefault(match.group(1), {})
            continue
        match = re.match(r'build ([^:]+):\s*(\S+)(.*)', line)
        if match:
            section = dict(rule=match.group(2), inputs=match.group(3).split('|', 1)[0].split())
            for out in match.group(1).split():
                edges[out] = section
            continue
        section = None
    return dict(rules=rules, edges=edges)


def build_assembly_recipe(unit_dir, obj):
    """How the unit's build assembles this TU, read from its build.ninja edge.

    Returns None when there is no build edge to read (no build.ninja, no --obj,
    or an output no edge produces); the caller then keeps the contract's bare
    `as -I .` in the unit directory and the report says so.
    """
    if obj is None:
        return None
    unit_dir = Path(unit_dir).resolve()
    document = _ninja_document(unit_dir)
    if document is None:
        return None
    try:
        target = os.path.relpath(Path(obj).resolve(), unit_dir)
    except ValueError:
        return None
    edge = document['edges'].get(target)
    if edge is None:
        return None
    rule = edge['rule']
    if rule == 'alloccommon':
        # Native MAIN first assembles the C TU, then uses the standard
        # relocatable link to allocate COMMON. Reproduce both steps rather
        # than comparing the final object with an unallocated compiler object.
        inputs = edge.get('inputs') or []
        command = (document['rules'].get(rule) or {}).get('command', '')
        expected = ('timeout -k 5 30 $ld -r -d -EL -m elf32lr5900 -o $obj $in && '
                    'timeout -k 5 30 cp $in.s $obj.s && timeout -k 5 30 cp $in.i $obj.i')
        if len(inputs) != 1 or command != expected or edge.get('obj') != target:
            raise AuditError('unsupported native MAIN COMMON-allocation recipe')
        raw = unit_dir / inputs[0]
        raw_edge = document['edges'].get(inputs[0]) or {}
        if raw_edge.get('rule') != 'ccas_c':
            raise AuditError('native MAIN COMMON allocation must follow the C compiler edge')
        linker_lines = re.findall(r'^ld\s*=\s*(.+)$', (unit_dir/'build.ninja').read_text(), re.M)
        if len(linker_lines) != 1:
            raise AuditError('native MAIN linker is not uniquely declared')
        linker_argv = linker_lines[0].split()
        if linker_argv[:4] == ['timeout', '-k', '5', '30']:
            linker_argv = linker_argv[4:]
        if len(linker_argv) != 1 or not Path(linker_argv[0]).is_absolute() or not Path(linker_argv[0]).is_file():
            raise AuditError('native MAIN linker declaration is not one absolute executable')
        recipe = build_assembly_recipe(unit_dir, raw)
        if recipe is None:
            raise AuditError('native MAIN raw compiler recipe is missing')
        recipe['common_allocation'] = dict(raw_object=str(raw), output_object=str(Path(obj).resolve()),
            linker=linker_argv[0], linker_sha256=sha256_file(linker_argv[0]))
        return recipe
    if rule == 'ccas_c':
        # tools/tu/ninja_main.py: cinc.py --root . --dst $overlay, then
        # `cd $overlay && ee-as ... -I . -I <unit_dir> -I <unit_dir>/include`
        overlay = edge.get('overlay')
        if not overlay:
            return None
        return dict(rule=rule, cinc=True, cwd=None,
                    build_overlay=unit_dir / overlay,
                    includes=[unit_dir, unit_dir / 'include'])
    if rule == 'ccas':
        # tools/tu/ninja_main.py: `ee-as ... -I . -I include` in the unit directory
        return dict(rule=rule, cinc=False, cwd=unit_dir, build_overlay=None,
                    includes=[unit_dir / 'include'])
    if rule == 'cc':
        # tools/tu/cc_tu.sh: `ee-as ... -I . -I <unit_dir> -I <root>` in the unit
        # directory; those two directories are the last arguments of the rule.
        command = (document['rules'].get('cc') or {}).get('command', '').split()
        if len(command) < 2:
            return None
        return dict(rule=rule, cinc=False, cwd=unit_dir, build_overlay=None,
                    includes=[unit_dir, Path(command[-1])])
    return None


def compile_tu(contract, tu_rel, unit_dir, out_dir, asm_cwd=None, asm_includes=(), recipe=None):
    """cpp -> cc1 -> [cinc] -> as under the contract and the build's recipe.

    Returns (paths, log, messages, asm_cwd, asm_includes): the last two are the
    directory the assembler ran in and the extra `-I` directories it was given,
    which the rest of the audit needs to resolve the same included `.s` files
    the assembler resolved.
    """
    cc = ROOT / contract['compiler']['dir']
    driver = cc / contract['driver']
    flags, incs = contract['flags'], []
    for d in contract['include_dirs']:
        incs += ['-I', d]
    stem = Path(out_dir) / Path(tu_rel).stem
    steps = [
        ('cpp', [driver, f'-B{cc}/', *contract['driver_flags'], '-E', *flags, *incs, str(tu_rel),
                 '-o', f'{stem}.i'], unit_dir),
        ('cc1', [driver, f'-B{cc}/', *contract['driver_flags'], '-S', *flags, f'{stem}.i', '-o', f'{stem}.s'],
         unit_dir),
    ]
    if recipe is not None:
        if recipe['cinc']:
            overlay = Path(out_dir) / 'cinc'
            # The audit's OWN cinc, not the audited tree's: this is tooling that
            # produces the audit's overlay, so it must not be supplied by the subject.
            steps.append(('cinc', [sys.executable, '-B', str(TOOL_ROOT / CINC_TOOL),
                                   '--i', f'{stem}.i', '--s', f'{stem}.s',
                                   '--root', str(unit_dir), '--dst', str(overlay),
                                   '--report', f'{stem}.cinc.json'], unit_dir))
            asm_cwd = overlay
        else:
            # `None` when the build assembles in the unit directory, which is the
            # audit's default too: the rest of the audit reads `asm_cwd` as "a
            # staged copy of the included .s files", and the unit files are not a
            # staged copy of themselves.
            asm_cwd = (Path(recipe['cwd']).resolve()
                       if Path(recipe['cwd']).resolve() != Path(unit_dir).resolve() else None)
        asm_includes = recipe['includes']
    allocation = (recipe or {}).get('common_allocation')
    assembled = f'{stem}.raw.o' if allocation else f'{stem}.o'
    steps.append(('as', assembler_argv(contract, assembled, f'{stem}.s', asm_includes),
                  asm_cwd or unit_dir))
    if allocation:
        if sha256_file(allocation['linker']) != allocation['linker_sha256']:
            raise AuditError('native MAIN linker changed during the audit')
        steps.append(('allocate_common', [allocation['linker'], '-r', '-d', '-EL', '-m', 'elf32lr5900',
                                         '-o', f'{stem}.o', assembled], unit_dir))
    log, messages = [], []
    for name, argv, cwd in steps:
        replay = None
        if name == 'cc1' and allocation:
            # This compiler can schedule identical preprocessed bytes differently
            # when their input/output path spellings change. Replay the native
            # build's actual relative paths, not an audit-specific filename.
            # The workspace is private: never overwrite the object being audited.
            try:
                raw_relative = Path(allocation['raw_object']).resolve().relative_to(
                    Path(unit_dir).resolve())
            except ValueError as exc:
                raise AuditError('native compiler output is outside its build directory') from exc
            workspace = Path(out_dir) / 'cc1-cwd'
            if workspace.exists():
                raise AuditError('native compiler replay workspace must be fresh')
            relative_i = Path(str(raw_relative) + '.i')
            relative_s = Path(str(raw_relative) + '.s')
            staged_i = workspace / relative_i
            staged_i.parent.mkdir(parents=True)
            input_sha256 = sha256_file(f'{stem}.i')
            shutil.copy2(f'{stem}.i', staged_i)
            if sha256_file(staged_i) != input_sha256:
                raise AuditError('native compiler replay changed its preprocessed input')
            argv = [driver, f'-B{cc}/', *contract['driver_flags'], '-S', *flags,
                    str(relative_i), '-o', str(relative_s)]
            cwd = workspace
            replay = dict(workspace=rel(workspace), input_relative=str(relative_i),
                          output_relative=str(relative_s), input_sha256=input_sha256)
        rc, out = run(argv, cwd)
        if replay is not None and rc == 0:
            generated_s = workspace / relative_s
            if not generated_s.is_file():
                raise AuditError('native compiler replay did not emit its assembly')
            output_sha256 = sha256_file(generated_s)
            shutil.copy2(generated_s, f'{stem}.s')
            if sha256_file(f'{stem}.s') != output_sha256:
                raise AuditError('native compiler replay changed its emitted assembly')
            replay['output_sha256'] = output_sha256
        entry = dict(step=name, argv=[rel(a) if str(a).startswith('/') else str(a) for a in argv],
                     cwd=rel(cwd), rc=rc, output=out[-2000:])
        if replay is not None:
            entry['compiler_replay'] = replay
        log.append(entry)
        if rc:
            raise AuditError(f'{name} failed ({rc}): {out[-400:]}')
        if out.strip():
            # Keep every message: a partially recovered TU can emit many legitimate
            # scaffold-exemption warnings, and truncating the capture cut the last
            # one mid-word so it failed the exemption match (2026-09-19).
            messages.append(dict(step=name, output=out.strip()[:65536]))
    return ((Path(f'{stem}.i'), Path(f'{stem}.s'), Path(f'{stem}.o')), log, messages,
            asm_cwd, list(asm_includes))


# ---------------------------------------------------------------------------
# Originals, manifests, accepted records
# ---------------------------------------------------------------------------

def original_elf(unit):
    from tools.inventory import ELF
    targets = json.loads((ROOT / 'config/targets.json').read_text())
    if unit not in targets['units']:
        raise AuditError(f'unknown unit {unit!r}')
    pin = targets['units'][unit]
    path = ROOT / 'game/extracted-iso' / pin['path']
    if not path.is_file():
        raise AuditError(f'original ELF missing: {rel(path)}')
    data = path.read_bytes()
    if hashlib.sha256(data).hexdigest() != pin['sha256']:
        raise AuditError(f'original ELF differs from config/targets.json: {rel(path)}')
    return ELF(data, unit), dict(unit=unit, path=rel(path), sha256=pin['sha256'])


def lookup_tu(tu_id, unit):
    """(start, end, function names, source, record) for a TU id."""
    build = ROOT / 'config/tu-build.json'
    tried = []
    if build.is_file():
        data = json.loads(build.read_text())
        objs = data.get('tus') or data.get('objects') or []
        for t in (objs if isinstance(objs, list) else objs.values()):
            if tu_id in (t.get('id'), t.get('map_id')):
                text = t.get('text') or {}
                funcs = [f['name'] if isinstance(f, dict) else f for f in t.get('functions', [])]
                return (int(text['start'], 16), int(text['end'], 16), funcs or None, 'config/tu-build.json', t)
        tried.append('config/tu-build.json')
        # MAIN carries the original OV02 image as an embedded section.  Its
        # link manifest names these aliases separately, while the canonical
        # recovery map counts each OV02 TU only once.  Resolve the alias only
        # when its complete original extent and compiler contract agree.
        embedded = re.fullmatch(r'main/ov02-embedded/tu(\d+)', tu_id)
        main_objects = ROOT / 'config/objects/main.objects.json'
        if unit == 'main' and embedded and main_objects.is_file():
            ordinal = int(embedded.group(1))
            canonical_id = f'ov02/tu{ordinal:03d}'
            records = objs if isinstance(objs, list) else objs.values()
            canonical = next((t for t in records if t.get('id') == canonical_id), None)
            aliases = json.loads(main_objects.read_text()).get('tus', [])
            alias = next((t for t in aliases if t.get('id') == tu_id), None)
            if canonical is None or alias is None:
                raise AuditError(f'embedded TU {tu_id} lacks its canonical OV02 mapping')
            if (alias.get('map_class') != 'ov02-embedded'
                    or not alias.get('alias_of', '').startswith(f'ov02/tu{ordinal:02d} ')):
                raise AuditError(f'embedded TU {tu_id} is not the declared OV02 alias')
            for bound in ('start', 'end'):
                if int(alias['text'][bound], 16) != int(canonical['text'][bound], 16):
                    raise AuditError(f'embedded TU {tu_id} differs at text {bound}')
            for field in ('flags', 'cc1_sha256', 'assembler_sha256'):
                if alias['contract'].get(field) != canonical['contract'].get(field):
                    raise AuditError(f'embedded TU {tu_id} has a different {field} contract')
            original_spans = []
            for target in ('main', 'ov02'):
                pin = data['units'][target]
                original = (ROOT / pin['file']).read_bytes()
                if hashlib.sha256(original).hexdigest() != pin['sha256']:
                    raise AuditError(f'embedded TU {tu_id}: {target} original pin differs')
                spans = [f for f in canonical['files'] if f['target'] == target]
                if len(spans) != 1:
                    raise AuditError(f'embedded TU {tu_id} has no unique {target} extent')
                span = spans[0]
                original_spans.append(original[span['offset_start']:span['offset_end']])
            extent = int(canonical['text']['end'], 16) - int(canonical['text']['start'], 16)
            if len(original_spans[0]) != extent or original_spans[0] != original_spans[1]:
                raise AuditError(f'embedded TU {tu_id} original images differ')
            record = dict(canonical, id=tu_id, unit=unit, path=alias['path'],
                          alias_of=canonical_id)
            names = [f['name'] for f in alias['functions']]
            return (int(alias['text']['start'], 16), int(alias['text']['end'], 16), names,
                    f'config/objects/main.objects.json (alias of {canonical_id})', record)
    for path in TU_MAPS:
        p = ROOT / path
        if not p.is_file():
            continue
        data = json.loads(p.read_text())
        tus = data.get('tus') or data.get('overlays', {}).get(unit, {}).get('tus', [])
        for t in tus:
            if t.get('id') == tu_id:
                start = t['text']['start'] if isinstance(t.get('text'), dict) else t['start']
                end = t['text']['end'] if isinstance(t.get('text'), dict) else t['end']
                return int(start, 16), int(end, 16), [f['name'] for f in t['functions']], path, t
        tried.append(path)
    raise AuditError(f'TU id {tu_id!r} not found in {tried}')


_ACCEPTED_ASM_INDEX = None


def accepted_asm_records():
    """{function name: (record path, record)} for accepted standalone ee-asm units."""
    global _ACCEPTED_ASM_INDEX
    if _ACCEPTED_ASM_INDEX is None:
        index = {}
        for path in sorted((ROOT / 'config/units').glob('*.json')):
            try:
                record = json.loads(path.read_bytes())
            except ValueError:
                continue
            if record.get('source_language') != 'ee-asm':
                continue
            for f in record.get('functions', []):
                index[f['name']] = (rel(path), record)
        _ACCEPTED_ASM_INDEX = index
    return _ACCEPTED_ASM_INDEX


# ---------------------------------------------------------------------------
# Audit
# ---------------------------------------------------------------------------

# Simplified audit (user decision 2026-11-01): these unit checks duplicate the
# whole-file gate or are bookkeeping, so they are reported but never fail the
# audit. `ok` stays True for every consumer; the real result is `finding_ok`.
# Function identity, emission, eligibility (forbidden-construct admission) and
# header/free-text admission still decide.
INFORMATIVE_UNIT_CHECKS = frozenset(('build_object_matches_audit', 'build_assembly_matches_audit',
                                     'toolchain_messages'))


class Checks:
    def __init__(self):
        self.rows = []

    def add(self, check, ok, **detail):
        if check in INFORMATIVE_UNIT_CHECKS:
            self.rows.append(dict(check=check, ok=True, informative=True, finding_ok=bool(ok), **detail))
            return ok
        self.rows.append(dict(check=check, ok=bool(ok), **detail))
        return ok


def function_classes(args):
    per = {}
    for spec in args.function_class or []:
        name, _, cls = spec.partition('=')
        if cls not in CLASSES:
            raise AuditError(f'invalid --function-class {spec!r}')
        per[name] = cls
    return per


def ledger_claims(path):
    """Claimed names from a TU ledger."""
    data = json.loads(Path(path).read_text())
    if isinstance(data, list):
        return [str(n) for n in data]
    if isinstance(data.get('claims'), list):
        return [str(n) for n in data['claims']]
    funcs = data.get('functions')
    if isinstance(funcs, dict):
        return [n for n, v in funcs.items() if not isinstance(v, dict) or v.get('state') in ('c', 'accepted_asm')]
    if isinstance(funcs, list):
        return [f['name'] for f in funcs if f.get('state') in ('c', 'accepted_asm')]
    raise AuditError(f'unrecognised ledger format: {path}')


def symbol_check(name, obj_funcs, pool, start):
    reasons, detail = [], {}
    orig = [s for s in pool if s['name'] == name]
    objs = obj_funcs.get(name, [])
    if len(orig) != 1:
        reasons.append(f'{len(orig)} original FUNC symbols named {name} in range')
    if len(objs) != 1:
        reasons.append(f'{len(objs)} FUNC symbols named {name} in the object')
    if len(orig) == 1 and len(objs) == 1:
        o, s = orig[0], objs[0]
        detail = dict(original=dict(va=o['va'], binding=BIND.get(o['binding'], o['binding']), type='FUNC',
                                    size=o['size']),
                      object=dict(offset=f'0x{s["value"]:x}', binding=BIND.get(s['binding'], s['binding']),
                                  type='FUNC', size=s['size'], section=s['section']))
        if s['section'] not in TEXT_SECTIONS:
            reasons.append(f'object symbol is in {s["section"]}, not .text')
        if o['binding'] != s['binding']:
            reasons.append(f'binding differs: original {BIND.get(o["binding"])} vs object {BIND.get(s["binding"])}')
        if o['size'] != s['size']:
            reasons.append(f'size differs: original {o["size"]} vs object {s["size"]}')
        if start is None:
            reasons.append('TU range unknown: offset not verified')
        elif int(o['va'], 16) - start != s['value']:
            reasons.append(f'offset differs: original +0x{int(o["va"], 16) - start:x} vs object +0x{s["value"]:x}')
        aliases = sorted({a['name'] for a in pool if a['va'] == o['va'] and a['name'] != name})
        lost = [a for a in aliases if not any(x['value'] == s['value'] for x in obj_funcs.get(a, []))]
        if aliases:
            detail['aliases'] = aliases
        if lost:
            reasons.append(f'original alias symbols missing from the object: {lost}')
    return reasons, detail


def nested_alias_names(specs, obj_funcs):
    """Candidate local names emitted for the authorized nested functions."""
    aliases = set()
    for entries in (specs or {}).values():
        for item in entries:
            prefix = re.escape(item['source_name']) + r'\.\d+'
            aliases.update(name for name in obj_funcs if re.fullmatch(prefix, name))
    return aliases


def audit_nested_c(item, parent, tu, prelude, tu_rel, header_set, regions, obj_funcs, pool, start,
                    tu_class=None):
    """Audit a compiler-emitted nested definition against its original identity."""
    original_name = item['original_name']
    reasons, checks = [], {}
    original = [s for s in pool if s['name'] == original_name]
    aliases = sorted(name for name in obj_funcs
                      if re.fullmatch(re.escape(item['source_name']) + r'\.\d+', name))
    if len(aliases) > 1 and len(original) == 1 and start is not None:
        aliases = [name for name in aliases if any(s['value'] == int(original[0]['va'], 16) - start
                                                  for s in obj_funcs[name])]
    if len(original) != 1:
        reasons.append(f'{len(original)} original FUNC symbols named {original_name} in range')
    if len(aliases) != 1:
        reasons.append(f'{len(aliases)} emitted local symbols match {item["source_name"]}.N')
    actual = aliases[0] if len(aliases) == 1 else None
    found = regions.get(actual, []) if actual else []
    prov = []
    if len(found) != 1:
        prov.append(f'{len(found)} cc1 regions (.ent/.end) for emitted nested symbol {actual or "<none>"}')
    elif found[0]['section'] not in TEXT_SECTIONS:
        prov.append(f'nested region starts in section {found[0]["section"]}')
    elif found[0]['file'] != tu_rel:
        prov.append(f'nested region is under .file {found[0]["file"]!r}, not {tu_rel}')
    elif not any(label == actual for label, _ in found[0]['labels']):
        prov.append(f'no {actual}: label inside nested region')
    checks['provenance'] = dict(ok=not prov, reasons=prov, emitted_name=actual)
    reasons += prov
    if len(original) == 1 and actual:
        expected = original[0]
        candidate = obj_funcs[actual][0]
        detail = dict(original=dict(name=original_name, va=expected['va'],
                                    binding=BIND.get(expected['binding'], expected['binding']),
                                    type='FUNC', size=expected['size']),
                      object=dict(name=actual, offset=f'0x{candidate["value"]:x}',
                                  binding=BIND.get(candidate['binding'], candidate['binding']),
                                  type='FUNC', size=candidate['size'], section=candidate['section']))
        symbol_reasons = []
        original_binding = BIND.get(expected['binding'], expected['binding'])
        # ELF symbols carry numeric STT_* values; nested-function mappings
        # use the corresponding readable label, just like bindings above.
        original_type = {STT_FUNC: 'FUNC'}.get(expected['type'], expected['type'])
        if original_binding != item['binding']:
            symbol_reasons.append(f'original binding differs from registered mapping: mapping {item["binding"]} '
                                  f'vs original {original_binding}')
        if original_type != item['type']:
            symbol_reasons.append(f'original type differs from registered mapping: mapping {item["type"]} '
                                  f'vs original {original_type}')
        if candidate['section'] not in TEXT_SECTIONS:
            symbol_reasons.append(f'object symbol is in {candidate["section"]}, not .text')
        if BIND.get(candidate['binding'], candidate['binding']) != item['binding']:
            symbol_reasons.append(f'binding differs: original {item["binding"]} vs '
                                  f'object {BIND.get(candidate["binding"], candidate["binding"])}')
        if candidate['size'] != item['size']:
            symbol_reasons.append(f'size differs: original {item["size"]} vs object {candidate["size"]}')
        if expected['size'] != item['size']:
            symbol_reasons.append(f'mapping size differs from original: {item["size"]} vs {expected["size"]}')
        if expected['va'] != item['va']:
            symbol_reasons.append(f'mapping VA differs from original: {item["va"]} vs {expected["va"]}')
        if start is None:
            symbol_reasons.append('TU range unknown: offset not verified')
        elif int(expected['va'], 16) - start != candidate['value']:
            symbol_reasons.append(f'offset differs: original +0x{int(expected["va"], 16) - start:x} '
                                  f'vs object +0x{candidate["value"]:x}')
        checks['symbol'] = dict(ok=not symbol_reasons, reasons=symbol_reasons, **detail)
        reasons += symbol_reasons
    else:
        checks['symbol'] = dict(ok=False, reasons=['nested symbol identity is incomplete'])
    text = prelude + tu.block_text(parent) + '\n'
    eligibility = []
    try:
        admission('gnu-nested-function', tu_class)(text, Path(tu_rel), header_set)
    except ValueError as error:
        eligibility.append(str(error))
    checks['eligibility'] = dict(ok=not eligibility, source_class='gnu-nested-function',
                                 reasons=eligibility)
    reasons += eligibility
    checks['record_fidelity'] = dict(ok=True, status='nested_parent', parent=parent.key,
                                     detail='the local symbol is covered by its parent C definition')
    return dict(name=original_name, block=parent.name, state='c', claimed=True, category='exact_c',
                ok=not reasons, reasons=reasons, checks=checks)


RODATA_ANON = RODATA_LIKE
# Sections whose anonymous cc1 constants (no original symbol names them) are
# covered only inside a declared carve run (config/objects/data-carves.json) and only
# when the whole compiled section, its relocations resolved at the runs'
# original addresses, equals the original bytes (`carved_section_bytes`). Under
# -G8 the small string literals and address templates go to .sdata, which
# otherwise holds named data, so .sdata is not blanket-anonymous like .rodata.
CARVED_ANON = ('.sdata',)


def carved_section_bytes(args, section, elf, obj):
    """dict(equal, ...): one carved C section, relocations resolved, vs the original bytes.

    Every byte of the compiled section is placed where the build places it: the
    declared runs, split by `tools/tu/data_carve.py` exactly as the build splits
    the object (`split_plans`), or the one run from its start. Each R_MIPS_32 is
    resolved at the original address of its target - a run of a carved section
    of this object, this TU's .text, or an original symbol of that name - and any
    other relocation, or a target that cannot be placed, makes the section
    unequal. Nothing is masked: an unresolved word is a failure, not a skip."""
    from tools.tu import data_carve as dc
    try:
        registry = dc.load_registry(ROOT)
        declared = (registry.get('tus') or {}).get(args.tu_id) or {}
        cobj = dc.Elf(bytes(obj.data))
        args_unit_dir = Path(args.unit_dir).resolve()
        orig = dc.Elf(bytes(elf.data))
        names = {'.rodata': ('.rodata', '.rdata')}
        plans = dc.split_plans(ROOT, args.unit, args_unit_dir, args.tu_id, cobj, registry)

        def pieces_of(sec_name):
            base = next((k for k, v in names.items() if sec_name in v), sec_name)
            runs = declared.get(base)
            if not runs:
                return None
            if base in plans:
                return plans[base]['pieces']
            csec_ = next(x for x in cobj.sections if x.name == sec_name)
            return [dict(offset=0, length=csec_.size, end=csec_.size, lo=dc.hx(runs[0]['range'][0]))]

        csec = next((x for x in cobj.sections if x.name in names.get(section, (section,)) and x.size), None)
        if csec is None:
            return dict(equal=False, reason=f'the C object emits no {section}')
        pieces = pieces_of(csec.name)
        data = bytearray(cobj.section_bytes(csec))
        text = next((x for x in cobj.sections if x.name == '.text'), None)
        text_start = dc.tu_context(ROOT, args.unit, args_unit_dir, args.tu_id)['text_start']
        by_name = {}
        for x in orig.symbols:
            if x.name and 0 < x.shndx < 0xff00:
                by_name.setdefault(x.name, set()).add(x.value)
        unresolved = []
        for rs in cobj.sections:
            if rs.type not in (4, 9) or rs.info != csec.index:
                continue
            for off, rtype, symi, add in dc._rel_entries(cobj, rs):
                sym = cobj.symbols[symi] if symi < len(cobj.symbols) else None
                where = f'{section}+0x{off:x}'
                if rtype != 2 or sym is None or off + 4 > len(data):
                    unresolved.append(f'{where}: relocation type {rtype}')
                    continue
                addend = add if add is not None else struct.unpack_from('<i', data, off)[0]
                target = (0 if sym.type == STT_SECTION else sym.value) + addend
                value = None
                if text is not None and sym.shndx == text.index:
                    value = text_start + target
                elif 0 < sym.shndx < len(cobj.sections) and sym.shndx != 0xfff1:
                    tp = pieces_of(cobj.sections[sym.shndx].name)
                    if tp:
                        try:
                            k = dc.piece_of(tp, target, where)
                            value = tp[k]['lo'] + target - tp[k]['offset']
                        except dc.CarveError:
                            value = None
                elif sym.shndx == 0 and len(by_name.get(sym.name, ())) == 1:
                    value = next(iter(by_name[sym.name])) + addend
                if value is None:
                    unresolved.append(f'{where}: {sym.name or cobj.sections[sym.shndx].name}+0x{target:x} '
                                      'has no original address')
                    continue
                struct.pack_into('<I', data, off, value & 0xffffffff)
        differing = []
        for k, p in enumerate(pieces):
            end = pieces[k + 1]['offset'] if k + 1 < len(pieces) else len(data)
            want = dc.va_bytes(orig, p['lo'], end - p['offset'])
            if want is None:
                return dict(equal=False, reason=f'run 0x{p["lo"]:08x} is not inside an allocated original section')
            got = bytes(data[p['offset']:end])
            differing += [f'0x{p["lo"] + i:08x}' for i in range(0, len(got), 4) if got[i:i + 4] != want[i:i + 4]]
        return dict(equal=not unresolved and not differing, pieces=len(pieces), size=len(data),
                    unresolved=unresolved[:8], differing_words=differing[:8])
    except (dc.CarveError, OSError, KeyError, ValueError, StopIteration) as exc:
        return dict(equal=False, reason=f'{type(exc).__name__}: {exc}')


def data_gate_row(args, tu_c):
    """Return current compiler-data evidence bound to this exact TU build.

    Carve declarations identify intended compiler-owned runs; they do not by
    themselves authorize object symbols. The linked C-data gate must have
    proved the current source/object/assembly and linked placement first.
    """
    path = getattr(args, 'data_gate_report', None)
    if not path:
        return None
    report = json.loads(Path(path).read_text())
    if report.get('result') != 'pass':
        raise AuditError('data gate report is not passing')
    rows = [row for row in report.get('objects', []) if row.get('tu') == args.tu_id]
    if len(rows) != 1:
        raise AuditError(f'data gate report has {len(rows)} rows for {args.tu_id}')
    row = rows[0]
    candidate_record = None
    manifest_path = getattr(args, 'candidate_manifest', None)
    if manifest_path:
        manifest_path = Path(manifest_path).resolve()
        manifest = json.loads(manifest_path.read_text())
        manifest_sha = sha256_file(manifest_path)
        if report.get('manifest_sha256') != manifest_sha:
            raise AuditError('candidate manifest hash differs from linked data-gate report')
        candidates = [item for item in manifest.get('candidates', []) if item.get('tu') == args.tu_id]
        if len(candidates) != 1:
            raise AuditError(f'candidate manifest has {len(candidates)} records for {args.tu_id}')
        candidate = candidates[0]
        candidate_path = Path(candidate['record']).resolve()
        candidate_sha = sha256_file(candidate_path)
        if candidate_sha != candidate.get('sha256'):
            raise AuditError('candidate record hash differs from candidate manifest')
        candidate_record = json.loads(candidate_path.read_text())
        if candidate_record.get('tu') != args.tu_id:
            raise AuditError('candidate record TU identity differs from candidate manifest')
        row = dict(row, _audit_candidate_record=dict(path=str(candidate_path), sha256=candidate_sha,
                                                     manifest_path=str(manifest_path),
                                                     manifest_sha256=manifest_sha),
                   _audit_location_dispositions=candidate_record.get('location_dispositions') or [])
    if row.get('source_sha256') != sha256_file(tu_c):
        raise AuditError('data gate report source hash differs from the audited TU')
    if not args.obj:
        raise AuditError('data gate report requires the raw build object')
    raw = Path(args.obj).resolve()
    selected = Path(getattr(args, 'data_gate_object', None) or raw).resolve()
    if row.get('object_sha256') != sha256_file(selected):
        raise AuditError('data gate report object hash differs from the selected linked-data object')
    if selected != raw:
        chain = validate_selected_data_object(args, raw, selected, tu_c, row)
        row = dict(row, _audit_selected_object_chain=chain)
    # Version 2 reports pin the compiler input and the linked data selection
    # separately. Function emission is still checked against the fresh raw
    # object; initialized spans must be read from the object the data gate used.
    if report.get('schema') == 'linked-c-data-audit/2':
        compiler_path = Path(row.get('compiler_raw_object_path', '/nonexistent')).resolve()
        data_path = Path(row.get('linked_data_object_path', '/nonexistent')).resolve()
        if compiler_path != raw or not data_path.is_relative_to(Path(args.unit_dir).resolve()):
            raise AuditError('linked data-gate compiler/selection paths differ from this unit build')
        if not data_path.is_file() or row.get('linked_data_object_sha256') != sha256_file(data_path):
            raise AuditError('linked data-gate selected object hash differs from the current build')
        code_input = raw
        allocation = row.get('compiler_allocation_proof')
        if allocation:
            if not candidate_record or args.unit != 'main':
                raise AuditError('native COMMON chain requires the frozen MAIN candidate manifest')
            code_input = Path(allocation.get('allocated_object_path', '/nonexistent')).resolve()
            verification = Path(allocation.get('verification_path', '/nonexistent')).resolve()
            linker = Path(allocation.get('linker_path', '/nonexistent')).resolve()
            if (not code_input.is_relative_to(Path(args.unit_dir).resolve()) or
                    Path(allocation.get('raw_object_path', '/nonexistent')).resolve() != raw or
                    allocation.get('raw_object_sha256') != sha256_file(raw) or
                    not code_input.is_file() or allocation.get('allocated_object_sha256') != sha256_file(code_input) or
                    not verification.is_file() or allocation.get('verification_sha256') != sha256_file(verification) or
                    sha256_file(verification) != sha256_file(code_input) or
                    not linker.is_file() or allocation.get('linker_sha256') != sha256_file(linker)):
                raise AuditError('native COMMON chain object/tool hashes differ from the current build')
            for key, path in (('compiler_object', raw), ('object', code_input)):
                pin = candidate_record.get(key) or {}
                if Path(pin.get('path', '/nonexistent')).resolve() != path or pin.get('sha256') != sha256_file(path):
                    raise AuditError('native COMMON chain differs from frozen candidate object pins')
            recipe = (build_assembly_recipe(args.unit_dir, code_input) or {}).get('common_allocation') or {}
            if (Path(recipe.get('raw_object', '/nonexistent')).resolve() != raw or
                    Path(recipe.get('output_object', '/nonexistent')).resolve() != code_input or
                    Path(recipe.get('linker', '/nonexistent')).resolve() != linker or
                    recipe.get('linker_sha256') != sha256_file(linker)):
                raise AuditError('native COMMON chain differs from the supported current build recipe')
            expected_argv = [str(linker), '-r', '-d', '-EL', '-m', 'elf32lr5900',
                             '-o', str(verification), str(raw)]
            if allocation.get('argv') != expected_argv:
                raise AuditError('native COMMON chain verification command differs from the exact recipe')
            with tempfile.TemporaryDirectory(prefix='audit-common-', dir=code_input.parent) as directory:
                independently_allocated = Path(directory) / 'allocated.o'
                argv = [str(linker), '-r', '-d', '-EL', '-m', 'elf32lr5900',
                        '-o', str(independently_allocated), str(raw)]
                status, log = run(argv, Path(args.unit_dir))
                if status or not independently_allocated.is_file() or sha256_file(independently_allocated) != sha256_file(code_input):
                    raise AuditError('independent native COMMON transformation differs: ' + log[:300])
            row = dict(row, _audit_compiler_allocation=dict(raw_object=str(raw),
                raw_sha256=sha256_file(raw), allocated_object=str(code_input),
                allocated_sha256=sha256_file(code_input), linker_sha256=sha256_file(linker),
                independent_native_reproduction=True))
        compare_code_identity(code_input, data_path)
        row = dict(row, _audit_linked_data_object=str(data_path))
    build_s = Path(args.build_s) if getattr(args, 'build_s', None) else Path(args.obj).with_suffix('.s')
    if build_s.is_file() and row.get('assembly_sha256') != sha256_file(build_s):
        raise AuditError('data gate report assembly hash differs from the build assembly')
    linked = unit_dir = Path(args.unit_dir)
    linked = unit_dir / 'build' / f'{args.unit}.rom.elf'
    if linked.is_file() and row.get('linked_elf_sha256') and row.get('linked_elf_sha256') != sha256_file(linked):
        raise AuditError('data gate report linked ELF hash differs from the current unit ELF')
    return row


def data_owner_control_role(args, source, role, pins):
    """Authenticate one ordinary DATA/control/ELF role without editing its rows."""
    from types import SimpleNamespace
    require = data_attribute_require
    require(isinstance(pins,dict) and set(pins)=={'data','elf','manifest','control','candidates'},
            'exact current-owner report role pins required')
    paths={key:data_attribute_pin(pins[key]) for key in ('data','elf','manifest','control')}
    if role=='primary':
        for key,actual in (('data',args.data_gate_report),('elf',args.data_elf_gate_report),
                           ('manifest',args.candidate_manifest)):
            require(actual and paths[key]==Path(actual).resolve(),'primary owner role differs from ordinary audit inputs')
    report=json.loads(paths['data'].read_text())
    manifest=json.loads(paths['manifest'].read_text())
    control=json.loads(paths['control'].read_text())
    gate=json.loads(paths['elf'].read_text())
    require(report.get('schema')=='linked-c-data-audit/2' and report.get('result')=='pass' and
        report.get('grants_data_recovery') is True and report.get('grants_function_acceptance') is False and
        not report.get('problems') and report.get('uncovered_storage_locations')==0 and
        report.get('manifest_sha256')==pins['manifest']['sha256'] and
        report.get('gate_sha256')==pins['elf']['sha256'] and
        Path(report.get('allocation_control_path','/nonexistent')).resolve()==paths['control'] and
        report.get('allocation_control_sha256')==pins['control']['sha256'],
        'current-owner role lacks its exact passing normal DATA chain')
    require(control.get('schema')=='data-recovery-allocation/1' and
        args.unit in manifest.get('linked_targets',[]) and gate.get('schema')=='elf-gate/1' and
        gate.get('result')=='pass','current-owner role has no passing unit ELF/control chain')
    target=gate.get('targets',{}).get(args.unit) or {}
    original,original_pin=original_elf(args.unit)
    rebuilt=(Path(args.unit_dir)/'build'/(args.unit+'.bin')).resolve()
    require(target.get('identical') is True and
        (target.get('original') or {}).get('sha256')==original_pin['sha256'] and
        (target.get('built') or {}).get('sha256')==original_pin['sha256'] and
        Path((target.get('built') or {}).get('path','/nonexistent')).resolve()==rebuilt and
        sha256_file(rebuilt)==original_pin['sha256'],
        'current-owner role ELF is not the actual current whole-file-identical unit')
    gate_manifest=gate.get('manifest') or {}
    gate_manifest_path=ROOT/gate_manifest.get('path','/nonexistent')
    require(gate_manifest_path.is_file() and sha256_file(gate_manifest_path)==gate_manifest.get('sha256'),
            'current-owner ELF gate configuration is stale')
    candidates=manifest.get('candidates')
    require(isinstance(candidates,list) and isinstance(pins['candidates'],dict) and
        len({c.get('tu') for c in candidates})==len(candidates) and
        set(pins['candidates'])=={c.get('tu') for c in candidates},
        'current-owner role requires the complete actual candidate collection')
    expected_role='whole_current_C_ownership_revalidation'
    if role=='secondary':
        require(all(item.get('role')==expected_role and item.get('additional_recovery_credit') is False
                    for item in (manifest,control)), 'secondary report is not zero-new-credit ownership revalidation')
    records={}
    for candidate in candidates:
        path=data_attribute_pin(pins['candidates'][candidate['tu']],candidate['record'])
        require(sha256_file(path)==candidate.get('sha256'),'owner candidate differs from normal manifest')
        record=json.loads(path.read_text());records[candidate['tu']]=record
        require(record.get('tu')==candidate['tu'] and record.get('unit')==args.unit and
            record.get('allocation_task')==control.get('task_id'),'owner candidate TU/unit/control mismatch')
        if role=='secondary':
            require(record.get('role')==expected_role and record.get('additional_recovery_credit') is False,
                    'secondary candidate has an unsupported role or recovery grant')
        def identity(item):
            return (item.get('unit'),item.get('tu',candidate['tu']),item.get('section'),
                    item.get('address'),tuple(item.get('names',[])))
        expected=[identity(item) for item in control.get('identities',[]) if item.get('tu')==candidate['tu']]
        actual=[identity(item) for item in record.get('recovered_location_candidates',[])]
        require(expected and len(expected)==len(set(expected)) and len(actual)==len(set(actual)) and
            set(expected)==set(actual) and all(item[0]==args.unit and item[2] in
                ('.rodata','.lit4','.lit8','.sdata','.data','.sbss','.bss') and item[4] for item in expected),
            'owner candidate identities differ from its normal unit/section allocation control')
    require(isinstance(control.get('tus'),list) and len(control['tus'])==len(records) and
        set(control['tus'])==set(records) and
        {item.get('tu') for item in control.get('identities',[])}==set(records),
        'owner allocation control and candidate TU collections differ')
    require(set(records)=={row.get('tu') for row in report.get('objects',[])} and
        len(report.get('objects',[]))==len(records),'owner report/candidate TU collections differ')
    if args.tu_id not in records:
        require(role=='secondary','primary report omits the actual audited TU')
        return None,None,dict(role=role,pins=pins,tu_present=False)
    adjusted=SimpleNamespace(**vars(args))
    adjusted.data_gate_report=str(paths['data']);adjusted.candidate_manifest=str(paths['manifest'])
    adjusted.data_elf_gate_report=str(paths['elf'])
    row=data_gate_row(adjusted,source)
    require(row.get('allocation_packet_sha256')==pins['control']['sha256'],
            'owner row is not bound to its normal allocation control')
    return row,records[args.tu_id],dict(role=role,pins=pins,tu_present=True)


def data_owner_native_common_storage(args,primary,secondary,item):
    """Re-prove one mandatory native COMMON owner outside mapped TU carves."""
    require=data_attribute_require
    require(args and args.unit=='main' and item in primary.get('verified_storage',[]) and
        item in secondary.get('verified_storage',[]) and item.get('section') in ('.bss','.sbss') and
        item.get('type')=='OBJECT' and item.get('bind')==1 and item.get('linked') is True,
        'outside-run storage is not an identical mandatory normal native COMMON owner')
    name=item['name'];address=int(item['address'],16);size=item['size'];alignment=item['alignment']
    require(type(size) is int and size>0 and type(alignment) is int and alignment>0 and
        alignment&(alignment-1)==0 and address%alignment==0 and
        item.get('original_identities')==[dict(name=name,binding=1,type=STT_OBJECT,size=size)],
        'native COMMON requires one exact positive original GLOBAL OBJECT identity')
    allocation=primary.get('compiler_allocation_proof') or {}
    require(all((row.get('_audit_compiler_allocation') or {}).get('independent_native_reproduction') is True
        for row in (primary,secondary)),'native COMMON roles were not independently reproduced')
    raw_path=Path(args.obj).resolve();allocated_path=Path(allocation.get('allocated_object_path','/nonexistent')).resolve()
    selected_path=Path(primary.get('_audit_linked_data_object','/nonexistent')).resolve()
    require(Path(allocation.get('raw_object_path','/nonexistent')).resolve()==raw_path and
        sha256_file(raw_path)==allocation.get('raw_object_sha256')==primary.get('object_sha256') and
        sha256_file(allocated_path)==allocation.get('allocated_object_sha256')==primary.get('derivation_input_sha256') and
        sha256_file(selected_path)==primary.get('linked_data_object_sha256')==item.get('object_sha256'),
        'native COMMON actual raw/allocated/selected bytes differ from normal proof')
    images=[RelObject(path.read_bytes()) for path in (raw_path,allocated_path,selected_path)]
    symbols=[]
    for image in images:
        found=[symbol for symbol in image.symbols if symbol.get('name')==name and symbol.get('shndx')!=0]
        require(len(found)==1 and found[0]['type']==STT_OBJECT and found[0]['binding']==1 and
            found[0]['size']==size,'native COMMON compiler identity/extent mismatch')
        symbols.append(found[0])
    raw,allocated,selected=symbols
    require(raw['shndx'] in (SHN_COMMON,SHN_MIPS_SCOMMON) and raw['value']==alignment,
            'outside-run storage is not actual compiler COMMON with its evidenced alignment')
    for image,symbol in zip(images[1:],symbols[1:]):
        section=image.section(symbol['section'])
        require(section and section['type']==8 and section['flags']&2 and
            symbol['section']==item['section'] and symbol['value']+size<=section['size'],
            'native COMMON allocation/selection is not complete NOBITS storage')
    assembly_path=Path(args.build_s).resolve()
    outside=re.sub(r'^\s*#APP\s*$.*?^\s*#NO_APP\s*$', '',assembly_path.read_text(),flags=re.M|re.S)
    emissions=[comm for comm in AsmFile(outside).comms if comm['name']==name]
    require(len(emissions)==1 and emissions[0]['kind']=='comm' and emissions[0]['size']==size and
        emissions[0]['align']==alignment and sha256_file(assembly_path)==primary.get('assembly_sha256'),
        'native COMMON lacks exact compiler .comm emission outside authored assembly')
    source=Path(args.tu_c).resolve();preprocessed=assembly_path.with_suffix('.i')
    require(sha256_file(source)==primary.get('source_sha256'),'native COMMON source pin drift')
    source_tu=tu_edit.parse(source.read_text());pre=Preprocessed(preprocessed.read_text());pre_tu=tu_edit.parse(pre.text)
    def definitions(parsed):
        result=[]
        for declaration in parsed.free_items():
            tokens=tu_edit.c_tokens(declaration.text(parsed.text))
            head=tokens[:tokens.index('=')] if '=' in tokens else tokens
            if declaration.kind=='decl' and name in head and not any(token in tokens for token in
                ('extern','typedef','asm','__asm','__asm__')) and '(' not in head[:head.index(name)]:
                result.append(declaration)
        return result
    source_defs,pre_defs=definitions(source_tu),definitions(pre_tu)
    require(len(source_defs)==len(pre_defs)==1,'native COMMON lacks unique actual C/CPP object definitions')
    where,line=pre.attribute(pre_defs[0].start)
    require(where and (Path(args.unit_dir)/where).resolve()==source and
        tu_edit.line_of(source_tu.text,source_defs[0].start)<=line<=tu_edit.line_of(source_tu.text,source_defs[0].end),
        'native COMMON CPP definition is not attributed to the current C source')
    original,_original_pin=original_elf(args.unit)
    from tools.inventory import ELF
    linked_path=(Path(args.unit_dir)/'build'/(args.unit+'.rom.elf')).resolve()
    require(sha256_file(linked_path)==primary.get('linked_elf_sha256'),'native COMMON linked ELF drift')
    linked=ELF(linked_path.read_bytes(),args.unit)
    for image in (original,linked):
        found=[symbol for symbol in image.symbols if symbol.get('name')==name and
            symbol.get('type')==STT_OBJECT and symbol.get('binding')==1 and symbol.get('size')==size and
            int(symbol.get('va','0'),16)==address and symbol.get('section')==item['section']]
        require(len(found)==1 and image.sections[found[0]['section_index']]['type']==8,
                'native COMMON does not match the exact original/linked GLOBAL OBJECT')
    mapping=item.get('allocated_carve_mapping') or {};common=mapping.get('common_allocation') or {}
    require(mapping.get('kind')=='native-main-nobits-carve' and mapping.get('raw_section')==allocated['section'] and
        mapping.get('raw_object_sha256')==allocation['allocated_object_sha256'] and
        int(mapping.get('allocated_offset','-1'),0)==allocated['value'] and
        mapping.get('carved_section')==selected['section'] and int(mapping.get('carved_offset','-1'),0)==selected['value'] and
        int(mapping.get('address','-1'),0)==address and mapping.get('size')==size and
        Path(common.get('compiler_object','/nonexistent')).resolve()==raw_path and
        common.get('compiler_object_sha256')==allocation['raw_object_sha256'] and
        common.get('compiler_assembly_sha256')==primary['assembly_sha256'] and common.get('alignment')==alignment,
        'native COMMON allocated/carved mapping or compiler provenance differs')
    carve_path=data_attribute_pin(dict(path=mapping.get('carve_map'),sha256=mapping.get('carve_map_sha256')))
    carve=json.loads(carve_path.read_text());piece=mapping.get('piece') or {}
    require(carve.get('tu')==args.tu_id and
        (Path(args.unit_dir)/carve.get('object','/nonexistent')).resolve()==allocated_path and
        (Path(args.unit_dir)/carve.get('out','/nonexistent')).resolve()==selected_path and
        piece in ((carve.get('sections',{}).get(allocated['section']) or {}).get('pieces',[])) and
        piece.get('section')==selected['section'] and int(piece.get('offset','-1'),0)==allocated['value'] and
        int(piece.get('length','-1'),0)==size and int(piece.get('va','-1'),0)==address and
        int(piece.get('run_end','-1'),0)==address+size,
        'native COMMON lacks its exact actual native carve-map piece')
    return dict(name=name,address=hex(address),size=size,alignment=alignment,
        original_identity=dict(name=name,binding=1,type=STT_OBJECT,size=size),
        actual_raw_COMMON=raw,actual_allocated=allocated,actual_selected=selected,
        carve_map=dict(path=str(carve_path),sha256=sha256_file(carve_path)),
        compiler_comm_emission=emissions[0],independent_native_reproduction_both_roles=True,
        additional_recovery_credit=False,grants_function_acceptance=False)


def data_owner_control_union(primary, secondary, registered_runs, args=None):
    """Combine proven ownership facts; different overlapping owners are refused."""
    require=data_attribute_require
    shared=('tu','source_sha256','object_sha256','compiler_raw_object_path','derivation_input_path',
        'derivation_input_sha256','linked_data_object_sha256','linked_data_object_path','assembly_sha256',
        'linked_elf_sha256','_audit_linked_data_object','_audit_compiler_allocation')
    require(all(primary.get(key)==secondary.get(key) for key in shared),
            'owner reports describe different source/compiler/allocation/selection/link contexts')
    allocations=[row.get('compiler_allocation_proof') for row in (primary,secondary)]
    require(bool(allocations[0])==bool(allocations[1]),'owner reports disagree on native COMMON allocation')
    if allocations[0]:
        require(all(allocations[0].get(key)==allocations[1].get(key) for key in
            ('raw_object_path','raw_object_sha256','allocated_object_path','allocated_object_sha256',
             'verification_sha256','linker_path','linker_sha256')),
            'normal COMMON role reproductions disagree on actual input/output/tool bytes')
    result=dict(primary)
    lists=('verified_locations','verified_location_spans','verified_storage','symbol_aliases','storage_owner_aliases',
        'anonymous_constants','uncredited_compiler_dependencies','uncredited_linker_dependencies',
        'uncredited_original_item_tails','uncredited_scaffold_suffixes','uncredited_original_alignment_padding',
        'natural_function_static_owner_aliases','original_object_identities','supplementary_allocations')
    for key in lists:
        values=[]
        for item in list(primary.get(key,[]))+list(secondary.get(key,[])):
            require(isinstance(item,dict),'unsupported ownership evidence row')
            if item not in values:values.append(item)
        result[key]=values
    for key in ('compiler_owned_ranges',):
        values={}
        for row in (primary,secondary):
            for section,ranges in row.get(key,{}).items():
                for interval in ranges:
                    require(isinstance(interval,list) and len(interval)==2 and
                        all(type(v) is int for v in interval) and 0<=interval[0]<interval[1],
                        'malformed compiler-owned interval')
                    if interval not in values.setdefault(section,[]):values[section].append(interval)
        result[key]=values
    intervals=[];native_common=[]
    for key in ('verified_location_spans','verified_storage'):
        for item in result[key]:
            if key=='verified_location_spans':
                interval=item.get('range');section=item.get('section')
            else:
                address=int(item['address'],16);interval=[address,address+item['size']];section=item.get('section')
            require(isinstance(interval,list) and len(interval)==2 and
                all(type(v) is int for v in interval) and 0<=interval[0]<interval[1],
                'malformed verified ownership extent')
            require(not any(section==other_section and max(interval[0],other[0])<min(interval[1],other[1])
                for other_section,other in intervals),'different or partially overlapping ownership evidence')
            contained=any(int(run['range'][0],16)<=interval[0]<interval[1]<=int(run['range'][1],16)
                          for run in registered_runs.get(section,[]))
            if not contained:
                require(key=='verified_storage','owner extent includes unregistered or unknown original scaffold storage')
                native_common.append(data_owner_native_common_storage(args,primary,secondary,item))
            intervals.append((section,interval))
    # A normal report must never turn an unknown scaffold suffix into C storage.
    tails=[]
    for key in ('uncredited_original_item_tails','uncredited_scaffold_suffixes','uncredited_original_alignment_padding'):
        for tail in result[key]:
            interval=tail.get('range')
            require(tail.get('credited') is False and isinstance(interval,list) and len(interval)==2,
                    'unsupported or credited scaffold tail in owner evidence')
            tails.append((tail.get('section'),interval))
    for section,extent in intervals:
        require(not any(section==tail_section and
            max(extent[0],interval[0])<min(extent[1],interval[1]) for tail_section,interval in tails),
            'ownership span swallows uncredited original scaffold storage')
    # Interior storage aliases must still be proved by a real complete candidate.
    # The secondary candidate is not synthesized or extended with primary names.
    if result['storage_owner_aliases']:
        require(not primary.get('storage_owner_aliases'),'unsupported split candidate provenance for storage aliases')
        result['_audit_candidate_record']=secondary['_audit_candidate_record']
    result['_audit_unclaimed_scaffold_runs_by_role']={
        'primary':primary.get('unclaimed_scaffold_runs',[]),
        'secondary':secondary.get('unclaimed_scaffold_runs',[])}
    result['_audit_compiler_allocation_proofs_by_role']={
        'primary':allocations[0],'secondary':allocations[1]}
    result['_audit_native_common_storage']=native_common
    result.pop('unclaimed_scaffold_runs',None)
    return result


def whole_ownership_data_row(args, source, primary):
    """Optional zero-new-credit union, used only by normal ownership/byte checks."""
    path=getattr(args,'data_owner_control_proof',None)
    if not path:return primary,None
    require=data_attribute_require
    document=json.loads(Path(path).read_text())
    require(isinstance(document,dict) and set(document)=={'schema','purpose','additional_recovery_credit',
        'grants_function_acceptance','primary','secondary','contexts'} and
        document.get('schema')=='data-owner-control-proof/1' and
        document.get('purpose')=='current-owner-revalidation' and
        document.get('additional_recovery_credit') is False and document.get('grants_function_acceptance') is False,
        'unsupported current-owner proof envelope or grant')
    require(primary is not None,'current-owner revalidation requires the primary normal DATA report')
    first,first_candidate,first_receipt=data_owner_control_role(args,source,'primary',document['primary'])
    second,second_candidate,second_receipt=data_owner_control_role(args,source,'secondary',document['secondary'])
    require(first==primary,'caller primary row differs from the actual normal DATA report')
    contexts=document['contexts']
    candidate_tus=set(document['primary']['candidates'])|set(document['secondary']['candidates'])
    require(isinstance(contexts,dict) and set(contexts)==candidate_tus and args.tu_id in contexts,
            'owner envelope context collection differs from its complete normal candidates')
    raw=Path(args.obj).resolve();assembly=Path(args.build_s).resolve()
    expected=dict(source=Path(source).resolve(),preprocessed=assembly.with_suffix('.i'),assembly=assembly,raw=raw,
        allocated=Path(first.get('derivation_input_path') or raw).resolve(),
        selected=Path(first['_audit_linked_data_object']).resolve(),
        linked=(Path(args.unit_dir)/'build'/(args.unit+'.rom.elf')).resolve(),
        original=(ROOT/original_elf(args.unit)[1]['path']).resolve(),registry=ROOT/'config/objects/data-carves.json',
        unit_manifest=Path(args.unit_dir)/'tu-manifest.json',config_tu_build=ROOT/'config/tu-build.json',
        data_gate_tool=ROOT/'tools/tu/data_gate.py',data_carve_tool=ROOT/'tools/tu/data_carve.py')
    pins=contexts[args.tu_id]
    require(isinstance(pins,dict) and set(pins)==set(expected),'exact current-owner compiler/context pins required')
    for key,actual in expected.items():data_attribute_pin(pins[key],actual)
    # Actual CPP and configuration provenance is bound by the normal secondary
    # candidate. Its source/raw/assembly/selected chain must equal the primary.
    if second is not None:
        candidate_files=second_candidate.get('files',{})
        for key in ('source','preprocessed','assembly','raw','allocated','selected','linked','registry','unit_manifest',
                    'config_tu_build','data_gate_tool','data_carve_tool'):
            matches=[pin for pin in candidate_files.values() if isinstance(pin,dict) and
                     Path(pin.get('path','/nonexistent')).resolve()==expected[key]]
            if key in ('source','raw','allocated','assembly'):
                matches += [pin for field in ('object','compiler_object','assembly')
                            if isinstance((pin:=second_candidate.get(field)),dict) and
                            Path(pin.get('path','/nonexistent')).resolve()==expected[key]]
            require(matches and all(pin.get('sha256')==pins[key]['sha256'] for pin in matches),
                    'owner context input lacks its actual normal candidate pin: '+key)
    registry=json.loads(expected['registry'].read_text())
    result=data_owner_control_union(first,second,(registry.get('tus') or {}).get(args.tu_id,{}),args) if second is not None else first
    receipt=dict(schema='whole-current-owner-coverage/1',proof=dict(path=str(Path(path).resolve()),
        sha256=sha256_file(path)),primary=first_receipt,secondary=second_receipt,context=pins,
        verified_locations=len(result.get('verified_locations',[])),additional_recovery_credit=False,
        primary_attribute_context_unchanged=True,grants_function_acceptance=False)
    return result,receipt


def compare_code_identity(raw_path, selected_path):
    """Require allocation/carving to preserve every compiler-emitted code identity."""
    raw = RelObject(Path(raw_path).read_bytes())
    selected = RelObject(Path(selected_path).read_bytes())
    raw_text = [(s['name'], s['size'], raw.bytes_of(s)) for s in raw.sections
                if s['name'] == '.text' or s['name'].startswith('.text.')]
    selected_text = [(s['name'], s['size'], selected.bytes_of(s)) for s in selected.sections
                     if s['name'] == '.text' or s['name'].startswith('.text.')]
    if raw_text != selected_text:
        raise AuditError('selected data object changes compiler-emitted .text sections')
    def funcs(obj):
        return sorted((s.get('name'), s.get('binding'), s.get('type'), s.get('size'), s.get('value'),
                       s.get('section')) for s in obj.symbols
                      if s.get('section') == '.text' and s.get('type') == STT_FUNC)
    if funcs(raw) != funcs(selected):
        raise AuditError('selected data object changes compiler-emitted function symbols')
    def text_relocs(obj):
        return sorted(row[:3] for row in obj.relocations() if 'text' in row[0])
    if text_relocs(raw) != text_relocs(selected):
        raise AuditError('selected data object changes compiler-emitted text relocations')


def allocation_output_path(args, raw):
    matches = []
    for report_path in getattr(args, 'allocation_report', None) or []:
        report = json.loads(Path(report_path).read_text())
        input_row, output_row = report.get('input') or {}, report.get('output') or {}
        input_path = Path(input_row.get('path', '/nonexistent')).resolve()
        # Some private allocators operate on a hash-pinned snapshot of the
        # Ninja raw object. Permit that exact copy only when both the snapshot
        # and current build input hash to the allocator's recorded input SHA.
        same_input = (input_path == raw or
                      (input_path.is_file() and raw.is_file() and
                       input_row.get('sha256') == sha256_file(raw) == sha256_file(input_path)))
        if same_input:
            matches.append((Path(output_row.get('path', '/nonexistent')).resolve(), report_path, report,
                            input_row, output_row))
    if len(matches) != 1:
        raise AuditError(f'{args.tu_id}: found {len(matches)} allocator outputs for the raw object')
    return matches[0]


def validate_selected_data_object(args, raw, selected, tu_c, row):
    """Bind linked-data evidence to raw C plus exact allocator/carve outputs.

    Function emission remains audited against `raw`. The linked-data report may
    instead pin a deterministic allocated/carved object; each edge is checked
    against its sidecar and current source/assembly before data is credited.
    """
    allocation_reports = getattr(args, 'allocation_report', None) or []
    carve_reports = getattr(args, 'data_carve_report', None) or []
    if not carve_reports:
        raise AuditError('--data-gate-object requires a current --data-carve-report')
    carved_arg = getattr(args, 'data_carve_object', None)
    if carved_arg and Path(carved_arg).resolve() != selected.resolve():
        raise AuditError('--data-carve-object differs from the linked-data report object')
    allocated = raw
    allocation_proof = None
    if allocation_reports:
        allocated, report_path, report, input_row, output_row = allocation_output_path(args, raw)
        if report.get('schema') != 'overlay-bss-allocation/1' or report.get('unit') != args.unit:
            raise AuditError('unsupported or mismatched overlay allocation report')
        if (input_row.get('sha256') != sha256_file(raw) or
                output_row.get('sha256') != sha256_file(allocated) or not allocated.is_file()):
            raise AuditError('overlay allocation report raw/output hash differs from current objects')
        source_hash = sha256_file(tu_c)
        file_pins = report.get('files') or {}
        build_s = Path(args.build_s).resolve() if args.build_s else raw.with_name(raw.name + '.s')
        if not build_s.is_file():
            raise AuditError('overlay allocation proof requires the current raw cc1 assembly')
        source_pinned = any(item.get('sha256') == source_hash and
                            Path(item.get('path', '')).resolve() == tu_c.resolve()
                            for item in file_pins.values())
        # Some overlay allocators record per-TU provenance on the recovered
        # rows rather than in the generic `files` map. Accept that shape only
        # when a row for this exact TU binds both current inputs by absolute
        # path and SHA. The raw/output object pins and code-identity checks
        # below remain mandatory.
        if not source_pinned:
            evidence_pins = [candidate.get('evidence') or {}
                             for candidate in report.get('recovered_location_candidates', [])
                             if (candidate.get('evidence') or {}).get('original_tu') == args.tu_id]
            source_pinned = any(
                evidence.get('source_sha256') == source_hash and
                Path(evidence.get('source_path', '/nonexistent')).resolve() == tu_c.resolve() and
                evidence.get('assembly_sha256') == sha256_file(build_s) and
                Path(evidence.get('assembly_path', '/nonexistent')).resolve() == build_s
                for evidence in evidence_pins)
        if not source_pinned:
            raise AuditError('overlay allocation report source/assembly pins differ from audited TU inputs')
        asm_pins = {(candidate.get('evidence') or {}).get('assembly_sha256')
                    for candidate in report.get('recovered_location_candidates', [])
                    if (candidate.get('evidence') or {}).get('original_tu') == args.tu_id}
        asm_pins.discard(None)
        if asm_pins and sha256_file(build_s) not in asm_pins:
            raise AuditError('overlay allocation report assembly pin differs from current cc1 output')
        compare_code_identity(raw, allocated)
        allocation_proof = dict(report=str(Path(report_path).resolve()),
                                 report_sha256=sha256_file(report_path),
                                 raw_object=str(raw), raw_sha256=sha256_file(raw),
                                 allocated_object=str(allocated), allocated_sha256=sha256_file(allocated),
                                 source_sha256=source_hash, assembly_sha256=sha256_file(build_s))
    carve_matches = []
    for report_path in carve_reports:
        report_path = Path(report_path).resolve()
        carve = json.loads(report_path.read_text())
        if carve.get('tu') != args.tu_id:
            continue
        input_path = Path(carve.get('object', ''))
        output_path = Path(carve.get('out', ''))
        if not input_path.is_absolute():
            input_path = Path(args.unit_dir).resolve() / input_path
        if not output_path.is_absolute():
            output_path = Path(args.unit_dir).resolve() / output_path
        if input_path.resolve() == allocated and output_path.resolve() == selected:
            carve_matches.append((report_path, carve))
    if len(carve_matches) != 1:
        raise AuditError(f'{args.tu_id}: found {len(carve_matches)} carve reports for the allocated/selected object pair')
    carve_path, carve_report = carve_matches[0]
    if not allocated.is_file() or not selected.is_file():
        raise AuditError('allocated or selected carved object is missing')
    helper_path = getattr(args, 'data_carve_tool', None)
    helper_sha = getattr(args, 'data_carve_tool_sha256', None)
    if not helper_path or not helper_sha:
        raise AuditError('selected-object proof requires pinned --data-carve-tool and --data-carve-tool-sha256')
    root = Path(args.root).resolve() if args.root else ROOT
    helper_path = Path(helper_path).resolve()
    try:
        helper_path.relative_to(root)
    except ValueError as exc:
        raise AuditError('pinned subject data-carve helper must resolve inside --root') from exc
    if sha256_file(helper_path) != helper_sha:
        raise AuditError('pinned subject data-carve helper SHA-256 differs')
    mod_name = '_tu_audit_subject_data_carve_' + helper_sha[:16]
    dc = sys.modules.get(mod_name)
    if dc is None:
        spec = importlib.util.spec_from_file_location(mod_name, helper_path)
        if spec is None or spec.loader is None:
            raise AuditError('cannot load pinned subject data-carve helper')
        dc = importlib.util.module_from_spec(spec)
        sys.modules[mod_name] = dc
        spec.loader.exec_module(dc)
    registry = dc.load_registry(root)
    derived = dc.derive(root, args.unit, Path(args.unit_dir).resolve(), args.tu_id, tu_c, allocated)
    if derived.get('problems'):
        raise AuditError('current data-carve derivation has problems: ' + '; '.join(derived['problems']))
    declared = (registry.get('tus') or {}).get(args.tu_id) or {}
    derived_runs = dc.normalize_runs(derived.get('runs', {}))
    declared_runs = dc.normalize_runs(declared)
    # Overlay allocation materializes native NOBITS owners after raw C
    # compilation. `derive` therefore covers initialized sections only; accept
    # a declared .bss/.sbss run only when the fresh linked-data row and the
    # current allocator output prove every named owner and its exact offset.
    if allocation_proof:
        allocation_report = allocation_proof['report']
        allocation = json.loads(Path(allocation_report).read_text())
        output = allocation.get('output') or {}
        allocated_elf = RelObject(allocated.read_bytes())
        allocated_symbols = [s for s in allocated_elf.symbols
                             if s.get('name') and s.get('type') in (STT_NOTYPE, STT_OBJECT)]
        allocator_rows = output.get('verified_symbols') or []
        for section in ('.bss', '.sbss'):
            runs = declared_runs.get(section) or []
            if not runs:
                continue
            storage_rows = [item for item in row.get('verified_storage', [])
                            if item.get('section') == section and item.get('linked') is True]
            for run in runs:
                run_lo, run_hi = int(run['range'][0], 16), int(run['range'][1], 16)
                spans = run.get('c_input_spans') or []
                for name in run.get('c_owned_symbols', []):
                    proofs = [item for item in storage_rows if item.get('name') == name]
                    alloc_rows = [item for item in allocator_rows if item.get('name') == name]
                    c_symbols = [item for item in allocated_symbols if item.get('name') == name]
                    if len(proofs) != 1 or len(alloc_rows) != 1 or len(c_symbols) != 1:
                        raise AuditError(f'{args.tu_id}: allocated {section} owner {name} lacks unique linked, '
                                         'allocator, or selected-object identity')
                    proof, alloc_row, c_symbol = proofs[0], alloc_rows[0], c_symbols[0]
                    try:
                        address, size = int(proof['address'], 16), proof['size']
                        alignment = proof['alignment']
                        owner_alignment = alloc_row['alignment']
                        map_lo, map_hi = (int(v, 16) for v in proof['tu_section_range'])
                        allocated_offset = int(alloc_row['allocated_offset'], 16)
                        span_matches = []
                        for span in spans:
                            if name not in span.get('symbols', []):
                                continue
                            span_lo, span_hi = (int(v, 16) for v in span['range'])
                            object_lo, object_hi = span['object_range']
                            mapped_offset = object_lo + address - span_lo
                            if (span.get('section') == c_symbol.get('section') and
                                    span_lo <= address and address + size <= span_hi and
                                    object_lo <= mapped_offset and mapped_offset + size <= object_hi):
                                span_matches.append(mapped_offset)
                    except (KeyError, TypeError, ValueError, IndexError):
                        raise AuditError(f'{args.tu_id}: malformed allocated {section} proof for {name}')
                    if (size <= 0 or alignment <= 0 or alignment & (alignment - 1) or
                            type(owner_alignment) is not int or owner_alignment <= 0 or
                            owner_alignment & (owner_alignment - 1) or
                            address % owner_alignment or allocated_offset % owner_alignment or
                            address < run_lo or address + size > run_hi or
                            map_lo > address or address + size > map_hi or
                            len(span_matches) != 1 or span_matches[0] != allocated_offset or
                            alloc_row.get('section') != c_symbol.get('section') or
                            alloc_row.get('proven_size') != size or
                            alloc_row.get('bind') != proof.get('bind') or
                            alloc_row.get('type') != {'NOTYPE': STT_NOTYPE, 'OBJECT': STT_OBJECT}.get(
                                proof.get('type')) or
                            c_symbol.get('value') != allocated_offset or
                            c_symbol.get('binding') != proof.get('bind') or
                            c_symbol.get('type') != {'NOTYPE': STT_NOTYPE, 'OBJECT': STT_OBJECT}.get(proof.get('type')) or
                            c_symbol.get('size') not in (0, size)):
                        raise AuditError(f'{args.tu_id}: allocated {section} owner {name} differs from its exact '
                                         'linked proof, input span, or current allocator output')
            declared_runs.pop(section, None)
            derived_runs.pop(section, None)
    def core_runs(run_map):
        return {sec: [{k: v for k, v in run.items() if k != 'c_storage_aliases'}
                      for run in runs] for sec, runs in run_map.items()}
    if core_runs(derived_runs) != core_runs(declared_runs):
        raise AuditError('current data-carve registry differs from fresh compiler derivation')
    plans = dc.split_plans(root, args.unit, Path(args.unit_dir).resolve(), args.tu_id,
                           dc.Elf(allocated.read_bytes()), registry)
    expected_sections = {sec: [dict(section=dc.split_section_name(sec, i), offset=hex(piece['offset']),
                                    length=hex(piece['length']), va=dc.h8(piece['lo']),
                                    run_end=dc.h8(piece['hi']))
                               for i, piece in enumerate(plan['pieces'])]
                         for sec, plan in plans.items()}
    recorded_sections = {sec: [{k: piece.get(k) for k in ('section', 'offset', 'length', 'va', 'run_end')}
                               for piece in value.get('pieces', [])]
                         for sec, value in (carve_report.get('sections') or {}).items()}
    if recorded_sections != expected_sections:
        raise AuditError('carve report sections differ from current fresh plans')
    expected = dc.split_object(allocated.read_bytes(), {sec: plan['pieces'] for sec, plan in plans.items()})
    if expected != selected.read_bytes():
        raise AuditError('selected carved object differs from deterministic data-carve output')
    compare_code_identity(raw, allocated)
    compare_code_identity(allocated, selected)
    return dict(raw_object=str(raw), raw_sha256=sha256_file(raw),
                allocation=allocation_proof, carve_report=str(carve_path),
                carve_report_sha256=sha256_file(carve_path),
                allocated_object=str(allocated), allocated_sha256=sha256_file(allocated),
                selected_object=str(selected), selected_sha256=sha256_file(selected),
                data_carve_tool=dict(path=str(helper_path), sha256=helper_sha))


def verified_storage_field_aliases(args, section, runs, data_row, storage_rows):
    """Validate interior address witnesses without adding C definitions."""
    aliases = [item for item in data_row.get('storage_owner_aliases',[]) if item.get('section') == section]
    if not aliases:
        return []
    from tools.tu import data_carve as dc, data_gate as dg
    unit_dir = Path(args.unit_dir).resolve()
    ctx = dc.tu_context(ROOT,args.unit,unit_dir,args.tu_id)
    source = Path(getattr(args,'tu_c',None) or ROOT/'src'/args.unit/(ctx['name']+'.c'))
    assembly = Path(args.build_s) if getattr(args,'build_s',None) else Path(str(args.obj)+'.s')
    selected = Path(data_row.get('_audit_linked_data_object','/nonexistent'))
    linked_path = unit_dir/'build'/f'{args.unit}.rom.elf'
    candidate_pin = data_row.get('_audit_candidate_record') or {}
    candidate_path = Path(candidate_pin.get('path','/nonexistent'))
    if (not source.is_file() or not assembly.is_file() or not selected.is_file() or not linked_path.is_file() or
            not candidate_path.is_file() or sha256_file(candidate_path) != candidate_pin.get('sha256') or
            sha256_file(source) != data_row.get('source_sha256') or
            sha256_file(assembly) != data_row.get('assembly_sha256') or
            sha256_file(selected) != data_row.get('linked_data_object_sha256') or
            sha256_file(linked_path) != data_row.get('linked_elf_sha256')):
        raise AuditError('interior storage aliases lack current source/compiler/selected/link/candidate pins')
    candidate = json.loads(candidate_path.read_text())
    raw = RelObject(Path(args.obj).read_bytes())
    input_path = Path((data_row.get('_audit_compiler_allocation') or {}).get('allocated_object',args.obj))
    allocation_input = RelObject(input_path.read_bytes())
    selected_obj = RelObject(selected.read_bytes())
    linked = dc.Elf(linked_path.read_bytes())
    original = dc.Elf(Path(ctx['orig']).read_bytes())
    items = dc.section_items(ctx,section)
    emission = dg.emitted_storage(assembly.read_text())
    checked, identities = [], set()
    for alias in aliases:
        try:
            address, owner_address = int(alias['address'],16),int(alias['owner_address'],16)
            extent, size, offset = alias['alias_extent'],alias['owner_size'],alias['owner_offset']
            name = alias['owner_name']
            names = set(alias['scaffold_names'])
        except (KeyError,TypeError,ValueError):
            raise AuditError('malformed interior storage field alias')
        if (not names or type(extent) is not int or extent <= 0 or type(size) is not int or size <= 0 or
                type(offset) is not int or offset <= 0 or not isinstance(name,str)):
            raise AuditError('malformed interior storage field alias extent or owner')
        owner_proofs = [proof for proof in storage_rows if proof.get('name') == name and
                        int(proof.get('address','0'),16) == owner_address and proof.get('size') == size]
        owner = owner_proofs[0] if len(owner_proofs) == 1 else {}
        raw_symbols = [symbol for symbol in raw.symbols if symbol.get('name') == name]
        input_symbols = [symbol for symbol in allocation_input.symbols if symbol.get('name') == name]
        selected_symbols = [symbol for symbol in selected_obj.symbols if symbol.get('name') == name]
        definition = emission.get(name,{})
        field = dg.source_storage_member_extent(source.read_text(),name,size,offset)
        current_candidates = [location for location in candidate.get('recovered_location_candidates',[])
                              if location.get('section') == section and int(location.get('address','0'),16) == address and
                              names == set(location.get('names',[])) and
                              (location.get('storage_owner') or {}).get('name') == name and
                              int((location.get('storage_owner') or {}).get('address','0'),16) == owner_address and
                              (location.get('storage_owner') or {}).get('size') == size and
                              location.get('alias_extent') == extent and location.get('alias_field') == field]
        mapped_items = [item for item in items if item['start'] == address and address+extent <= item['end'] and
                        names <= set(item.get('labels',[]))]
        mapped_inputs = [int(part['object_range'][0])+owner_address-int(part['range'][0],16)
                         for run in runs for part in run.get('c_input_spans',[])
                         if name in part.get('symbols',[]) and part.get('object_range') and
                         int(part['range'][0],16) <= owner_address and
                         owner_address+size <= int(part['range'][1],16)]
        expected_type = {'NOTYPE':STT_NOTYPE,'OBJECT':STT_OBJECT}.get(owner.get('type'))
        linked_owners = [symbol for symbol in linked.symbols if symbol.name == name and symbol.value == owner_address and
                         symbol.bind == owner.get('bind') and symbol.type == expected_type and symbol.size in (0,size) and
                         0 < symbol.shndx < len(linked.sections) and linked.sections[symbol.shndx].type == 8]
        original_at_alias = [symbol for symbol in original.symbols if symbol.value == address and
                             0 < symbol.shndx < len(original.sections) and original.sections[symbol.shndx].name == section]
        if (not names or len(owner_proofs) != 1 or type(extent) is not int or extent <= 0 or
                type(size) is not int or type(offset) is not int or offset <= 0 or
                address != owner_address+offset or offset+extent > size or
                alias.get('mapped_span') != [address,address+extent] or
                alias.get('linked') is not True or alias.get('source_field') != field or not field or field['size'] != extent or
                alias.get('owner_object_sha256') != data_row.get('linked_data_object_sha256') or
                alias.get('source_sha256') != data_row.get('source_sha256') or
                alias.get('assembly_sha256') != data_row.get('assembly_sha256') or
                alias.get('linked_elf_sha256') != data_row.get('linked_elf_sha256') or
                len(current_candidates) != 1 or len(mapped_items) != 1 or len(mapped_inputs) != 1 or
                len(raw_symbols) != 1 or len(input_symbols) != 1 or len(selected_symbols) != 1 or len(linked_owners) != 1 or
                definition.get('size') != size or definition.get('section') not in (section,'COMMON') or
                raw_symbols[0].get('binding') != owner.get('bind') or raw_symbols[0].get('type') != expected_type or
                raw_symbols[0].get('size') not in (0,size) or
                input_symbols[0].get('value') != mapped_inputs[0] or
                input_symbols[0].get('section') != section or
                selected_symbols[0].get('value') != alias.get('c_object_offset') or
                selected_symbols[0].get('section') != alias.get('c_object_section') or
                any(symbol.type == STT_OBJECT and symbol.size > 0 for symbol in original_at_alias)):
            raise AuditError(f'{args.tu_id}: unproven interior storage field alias at 0x{address:08x}')
        for scaffold_name in names:
            key = (address,scaffold_name)
            if key in identities or scaffold_name == name:
                raise AuditError('duplicate interior storage alias or alias counted as its owner')
            identities.add(key)
        checked.append(dict(alias))
    return checked


def c_owned_sections(args, contract, tu_record, data_row=None):
    """{section: {lo, hi, alo, ahi, exact, source}} for C-owned data sections."""
    owners, manifest_ranges, partial = {}, {}, {}
    if tu_record and isinstance(tu_record.get('data_ownership'), dict):
        for sec, own in tu_record['data_ownership'].items():
            if isinstance(own, dict):
                if own.get('owner') == 'c' and sec != '.text':
                    owners[sec] = own.get('range')
                    partial[sec] = bool(own.get('partial'))
                if own.get('window'):
                    manifest_ranges[sec] = own
            elif own == 'c' and sec != '.text':
                owners[sec] = None
    # A declared C-owned data run (config/objects/data-carves.json, tools/tu/data_carve.py):
    # the jump tables and literals the TU's C functions generate, carved out of the
    # scaffold piece. Declared under this root (a worker's or reviewer's private
    # copy, or the published one), it is newer than config/tu-build.json, which
    # carries it only after publication regenerates the manifest.
    # Several runs of one section (the C-generated items of the TU are separated by
    # scaffold items) own the span from the first run's start to the last run's end
    # for the size check, and only the runs themselves for the original symbols.
    carved, run_ranges = set(), {}
    verified_storage_by_section = {}
    storage_field_aliases_by_section = {}
    generated_aliases_by_section, symbol_aliases_by_section, anonymous_ranges_by_section = {}, {}, {}
    carve_path = ROOT / 'config/objects/data-carves.json'
    if getattr(args, 'tu_id', None) and carve_path.is_file():
        for sec, runs in ((json.loads(carve_path.read_text()).get('tus') or {}).get(args.tu_id) or {}).items():
            if runs:
                owners[sec] = [runs[0]['range'][0], runs[-1]['range'][1]]
                partial[sec] = True
                carved.add(sec)
                if len(runs) > 1:
                    run_ranges[sec] = [(int(r['range'][0], 16), int(r['range'][1], 16)) for r in runs]
                if data_row is not None:
                    declared = {name for run in runs for name in run.get('c_owned_symbols', [])}
                    verified = {name for item in data_row.get('verified_locations', [])
                                if item.get('section') == sec and
                                any(int(run['range'][0], 16) <= int(item['address'], 16) <
                                    int(run['range'][1], 16) for run in runs)
                                for name in item.get('names', [])}
                    # A disassembler label is not a retail ELF identity. The
                    # gate records the real C symbol separately when a literal
                    # has a semantic name but no original ELF symbol.
                    for span in data_row.get('verified_location_spans', []):
                        if (span.get('section') == sec and not span.get('original_identities') and
                                span.get('object_sha256') == data_row.get(
                                    'linked_data_object_sha256', data_row.get('object_sha256')) and
                                any(int(run['range'][0], 16) <= int(span['address'], 16) and
                                    int(span['address'], 16) + span['length'] <= int(run['range'][1], 16)
                                    for run in runs) and span.get('object_symbols')):
                            verified.difference_update(span.get('names', []))
                            verified.update(span['object_symbols'])
                    symbol_aliases = [dict(alias) for alias in data_row.get('symbol_aliases', [])
                                      if alias.get('section') == sec]
                    storage_rows = [dict(item) for item in data_row.get('verified_storage', [])
                                    if item.get('section') == sec and item.get('linked') is True]
                    if sec in ('.bss', '.sbss'):
                        valid_storage = []
                        for storage in storage_rows:
                            try:
                                address = int(storage['address'], 16)
                                size, alignment = storage['size'], storage['alignment']
                                map_lo, map_hi = (int(v, 16) for v in storage['tu_section_range'])
                                count, status = storage['relocation_count'], storage['relocation_status']
                            except (KeyError, TypeError, ValueError, IndexError):
                                continue
                            relocation_ok = ((type(count) is int and count > 0 and
                                              status == 'cc1_function_relocation') or
                                             (count == 0 and status ==
                                              'no_retained_relocation_exact_linked_identity'))
                            original_ids = storage.get('original_identities') or []
                            original_ok = all(
                                identity.get('binding') in (storage.get('bind'), BIND.get(storage.get('bind'))) and
                                ({STT_NOTYPE: 'NOTYPE', STT_OBJECT: 'OBJECT'}.get(identity.get('type'),
                                    identity.get('type')) == storage.get('type')) and
                                identity.get('size') in (0, size)
                                for identity in original_ids)
                            if (type(size) is int and size > 0 and type(alignment) is int and alignment > 0 and
                                    alignment & (alignment - 1) == 0 and storage.get('bind') in (0, 1, 2) and
                                    storage.get('type') in ('NOTYPE', 'OBJECT') and
                                    any(int(run['range'][0], 16) <= address and
                                        address + size <= int(run['range'][1], 16) for run in runs) and
                                    map_lo <= address and address + size <= map_hi and relocation_ok and original_ok):
                                valid_storage.append(storage)
                        valid_by_name = {item['name']: item for item in valid_storage}
                        declared_storage = {name for run in runs for name in run.get('c_owned_symbols', [])}
                        if set(valid_by_name) != declared_storage:
                            raise AuditError(f'{args.tu_id} {sec}: declared NOBITS owners differ from linked '
                                             f'storage proof (declared={sorted(declared_storage)}, '
                                             f'verified={sorted(valid_by_name)})')
                        verified.update(valid_by_name)
                        verified_storage_by_section[sec] = valid_storage
                        field_aliases = verified_storage_field_aliases(args,sec,runs,data_row,valid_storage)
                        verified.difference_update(name for alias in field_aliases for name in alias['scaffold_names'])
                        storage_field_aliases_by_section[sec] = field_aliases
                    alias_by_scaffold = {}
                    alias_objects = set()
                    for alias in symbol_aliases:
                        scaffold_name = alias.get('scaffold_name')
                        object_symbol = alias.get('object_symbol')
                        address = int(alias.get('address'), 16)
                        padded_owner_spans = set()
                        for span in data_row.get('verified_location_spans',[]):
                            owner = span.get('storage_owner') or {}
                            if (span.get('section') != sec or owner.get('name') != object_symbol or
                                    owner.get('size') != alias.get('size') or
                                    span.get('length',0) <= alias.get('size',0) or alias.get('size',0) <= 0 or
                                    owner.get('address','').lower() != alias.get('address','').lower() or
                                    owner.get('binding') != alias.get('binding') or owner.get('type') != alias.get('type') or
                                    owner.get('object_sha256') != data_row.get('object_sha256') or
                                    owner.get('source_sha256') != data_row.get('source_sha256') or
                                    owner.get('assembly_sha256') != data_row.get('assembly_sha256') or
                                    owner.get('linked_elf_sha256') != data_row.get('linked_elf_sha256')):
                                continue
                            from tools.tu import data_carve as dc
                            raw_owner = RelObject(Path(args.obj).read_bytes())
                            selected_owner = RelObject(Path(data_row['_audit_linked_data_object']).read_bytes())
                            raw_symbols = [symbol for symbol in raw_owner.symbols if symbol.get('name') == object_symbol and
                                           symbol.get('section') == owner.get('section') and
                                           symbol.get('value') == owner.get('c_object_offset') and
                                           symbol.get('size') == alias['size'] and symbol.get('binding') ==
                                           {'LOCAL':0,'GLOBAL':1,'WEAK':2}.get(alias.get('binding')) and
                                           symbol.get('type') == {'NOTYPE':STT_NOTYPE,'OBJECT':STT_OBJECT}.get(alias.get('type'))]
                            selected_symbols = [symbol for symbol in selected_owner.symbols if symbol.get('name') == object_symbol and
                                                symbol.get('section') == span.get('c_object_section') and
                                                symbol.get('value') == span.get('c_object_offset') and
                                                symbol.get('size') == alias['size'] and symbol.get('binding') ==
                                                {'LOCAL':0,'GLOBAL':1,'WEAK':2}.get(alias.get('binding')) and
                                                symbol.get('type') == {'NOTYPE':STT_NOTYPE,'OBJECT':STT_OBJECT}.get(alias.get('type'))]
                            if len(raw_symbols) != 1 or len(selected_symbols) != 1:
                                continue
                            raw_symbol, selected_symbol = raw_symbols[0],selected_symbols[0]
                            raw_payload = raw_owner.bytes_of(raw_owner.sections[raw_symbol['shndx']])
                            selected_payload = selected_owner.bytes_of(selected_owner.sections[selected_symbol['shndx']])
                            lo,size = raw_symbol['value'],raw_symbol['size']
                            selected_lo = selected_symbol['value']
                            ctx = dc.tu_context(ROOT,args.unit,Path(args.unit_dir),args.tu_id)
                            original = dc.Elf(Path(ctx['orig']).read_bytes())
                            raw_relocations = [row for row in raw_owner.relocations() if row[0] == raw_symbol['section'] and
                                               lo <= row[1] < lo+size]
                            if (not raw_relocations and raw_payload[lo:lo+size] == dc.va_bytes(original,address,size) and
                                    selected_payload[selected_lo:selected_lo+size] == raw_payload[lo:lo+size] and
                                    not any(selected_payload[selected_lo+size:selected_lo+span['length']])):
                                padded_owner_spans.add((span.get('section'),span.get('address'),span.get('c_object_offset')))
                        # R22 producers may represent a same-address alias in
                        # the TU-level symbol_aliases list while the matching
                        # span carries the original identity and selected C
                        # object identity. Accept that normalized form only
                        # when every identity field is independently pinned by
                        # the exact span and selected-object hash.
                        direct_span_alias = any(
                            span.get('address', '').lower() == alias.get('address', '').lower() and
                            object_symbol in span.get('object_symbols', []) and
                            object_symbol in span.get('names', []) and
                            span.get('object_sha256') == data_row.get(
                                'linked_data_object_sha256', data_row.get('object_sha256')) and
                            (span.get('length') == alias.get('size') or
                             (span.get('section'),span.get('address'),span.get('c_object_offset')) in padded_owner_spans) and
                            any(identity.get('name') == scaffold_name and
                                identity.get('address', '').lower() == alias.get('address', '').lower() and
                                identity.get('binding') == alias.get('binding') and
                                identity.get('type') == alias.get('type') and
                                identity.get('size') == alias.get('size')
                                for identity in span.get('original_identities', []))
                            for span in data_row.get('verified_location_spans', [])
                            if span.get('section') == sec)
                        direct_identity_alias = (
                            direct_span_alias and
                            alias.get('binding') in ('LOCAL', 'GLOBAL', 'WEAK') and
                            alias.get('type') in ('NOTYPE', 'OBJECT'))
                        if (not direct_identity_alias and sec in ('.bss', '.sbss') and
                                alias.get('binding') == 'LOCAL' and alias.get('type') == 'NOTYPE'):
                            # A zero-size retail local has one compiler .space
                            # owner, while Splat supplies a public marker name.
                            # Prove that exact alias without counting another
                            # compiler definition or inferring punctuation.
                            storage = verified_storage_by_section.get(sec, [])
                            proofs = [item for item in storage if item.get('name') == object_symbol and
                                      int(item.get('address', '0'),16) == address and
                                      item.get('size') == alias.get('size') and item.get('bind') == 0 and
                                      item.get('type') == 'NOTYPE' and item.get('linked') is True and
                                      item.get('object_sha256') == data_row.get(
                                          'linked_data_object_sha256',data_row.get('object_sha256')) and
                                      any(identity.get('name') == object_symbol and
                                          identity.get('binding') == 0 and identity.get('type') == 0 and
                                          identity.get('size') == 0 for identity in item.get('original_identities',[]))]
                            if len(proofs) == 1:
                                from tools.tu import data_carve as dc, data_gate as dg
                                raw_storage = RelObject(Path(args.obj).read_bytes())
                                symbols = [symbol for symbol in raw_storage.symbols
                                           if symbol.get('name') == object_symbol and symbol.get('section') == sec and
                                           symbol.get('binding') == 0 and symbol.get('type') == STT_NOTYPE and
                                           symbol.get('size') == 0]
                                build_s = Path(args.build_s) if getattr(args,'build_s',None) else Path(str(args.obj)+'.s')
                                emission = dg.emitted_storage(build_s.read_text()).get(object_symbol,{})
                                ctx = dc.tu_context(ROOT,args.unit,Path(args.unit_dir),args.tu_id)
                                items = [item for item in dc.section_items(ctx,sec) if item['start'] == address and
                                         item['end']-item['start'] == alias.get('size') and
                                         scaffold_name in item.get('labels',[])]
                                direct_identity_alias = (len(symbols) == 1 and len(items) == 1 and
                                    emission.get('extent_kind') == 'label-space' and emission.get('section') == sec and
                                    emission.get('size') == alias.get('size') and
                                    any(symbols[0]['value'] == int(part['object_range'][0]) + address-int(part['range'][0],16)
                                        for run in runs for part in run.get('c_input_spans',[])
                                        if object_symbol in part.get('symbols',[]) and part.get('object_range') and
                                        int(part['range'][0],16) <= address and
                                        address+alias['size'] <= int(part['range'][1],16)))
                        duplicate_object = object_symbol in alias_objects and not direct_identity_alias
                        if (not scaffold_name or not object_symbol or scaffold_name in alias_by_scaffold or
                                duplicate_object or scaffold_name == object_symbol or
                                (scaffold_name not in declared and not direct_identity_alias) or
                                object_symbol not in verified or
                                not any(int(run['range'][0], 16) <= address < int(run['range'][1], 16)
                                        for run in runs) or
                                not any(item.get('section') == sec and int(item['address'], 16) == address and
                                        object_symbol in item.get('names', [])
                                        for item in data_row.get('verified_locations', [])) and
                                not direct_identity_alias):
                            raise AuditError(f'{args.tu_id} {sec}: malformed or unproven scaffold/C symbol alias '
                                             f'in linked data-gate evidence: {alias}')
                        alias_by_scaffold[scaffold_name] = alias
                        alias_objects.add(object_symbol)
                    # GNU cc1 emits switch tables as anonymous section payload;
                    # the original linker/map may give those bytes a local
                    # `jtbl_<VA>` label which has no counterpart in the C object.
                    # Admit only the exact declared jtbl label whose complete
                    # address is covered by the fresh linked-data gate's
                    # compiler-owned range. This is hash-bound to the current
                    # source/object/assembly/linked ELF below; ordinary named
                    # data still has to appear in verified_locations.
                    generated_aliases = set()
                    gate_ranges = data_row.get('compiler_owned_ranges', {}).get(sec, [])
                    for name in declared - verified:
                        match = re.fullmatch(r'jtbl_([0-9A-Fa-f]{8})', name)
                        if not match:
                            continue
                        address = int(match.group(1), 16)
                        in_carve = any(int(run['range'][0], 16) <= address < int(run['range'][1], 16)
                                       for run in runs)
                        in_compiler_range = any(int(span[0]) <= address < int(span[1])
                                                for span in gate_ranges)
                        if in_carve and in_compiler_range:
                            generated_aliases.add(name)
                    alias_carve_names = ({name for name in alias_by_scaffold if name in declared} |
                                         {alias['object_symbol'] for alias in alias_by_scaffold.values()
                                          if alias['object_symbol'] in declared})
                    uncredited_scaffold_names = {
                        uncredited_dependency_label(dep) for dep in data_row.get('uncredited_compiler_dependencies', [])
                        if dep.get('section') == sec and dep.get('credited') is False and
                        dep.get('disposition') == 'uncredited_scaffold_dependency' and
                        dep.get('kind') in ('inline_assembly_data', 'compiler_region_switch_words') and
                        uncredited_dependency_label(dep) and
                        (dep.get('kind') != 'compiler_region_switch_words' or
                         valid_switch_word_dependency_shape(dep))}
                    expected_declared = ((verified - alias_objects - set(alias_by_scaffold)) |
                                         alias_carve_names | generated_aliases | uncredited_scaffold_names)
                    if declared != expected_declared:
                        raise AuditError(f'{args.tu_id} {sec}: declared C-owned symbols differ from '
                                          f'linked data-gate evidence (declared={sorted(declared)}, '
                                          f'verified={sorted(verified)}, aliases={sorted(alias_by_scaffold)})')
                    anonymous = [x for x in data_row.get('anonymous_constants', [])
                                 if x.get('section') == sec and x.get('range')]
                    anonymous_ranges = [(int(x['range'][0]), int(x['range'][1])) for x in anonymous]
                    generated_aliases_by_section[sec] = generated_aliases
                    symbol_aliases_by_section[sec] = list(alias_by_scaffold.values())
                    anonymous_ranges_by_section[sec] = anonymous_ranges
                else:
                    anonymous_ranges = []
    for spec in args.c_owned or []:
        sec, _, rng = spec.partition('=')
        if sec in carved:
            continue          # the declared run supersedes an import-era split of the section
        owners[sec] = rng.split(':') if rng else owners.get(sec)
    out = {}
    for sec, rng in owners.items():
        if rng:
            lo, hi = int(rng[0], 16), int(rng[1], 16)
            out[sec] = dict(lo=lo, hi=hi, alo=lo, ahi=hi, exact=True,
                            source='config/objects/data-carves.json' if sec in carved else 'explicit',
                            partial=partial.get(sec, False), runs=run_ranges.get(sec),
                            verified_names=sorted({name for item in (data_row or {}).get('verified_locations', [])
                                                   if item.get('section') == sec and lo <= int(item['address'], 16) < hi
                                                   for name in item.get('names', []) if name not in {
                                                       n for alias in storage_field_aliases_by_section.get(sec,[]) for n in alias['scaffold_names']}}),
                            verified_storage_field_aliases=storage_field_aliases_by_section.get(sec,[]),
                            verified_locations=[item for item in (data_row or {}).get('verified_locations', [])
                                                if item.get('section') == sec],
                            verified_storage=verified_storage_by_section.get(sec, []),
                            anonymous_ranges=anonymous_ranges_by_section.get(sec, []),
                            generated_aliases=sorted(generated_aliases_by_section.get(sec, set())),
                            symbol_aliases=symbol_aliases_by_section.get(sec, []),
                            uncredited_dependencies=[item for item in (data_row or {}).get(
                                'uncredited_compiler_dependencies', []) if item.get('section') == sec],
                            compiler_owned_ranges=[list(map(int, span)) for span in
                                                   (data_row or {}).get('compiler_owned_ranges', {}).get(sec, [])])
            continue
        entry = dict(lo=None, hi=None, alo=None, ahi=None, exact=False, source=None,
                     partial=partial.get(sec, False))
        if sec in manifest_ranges:
            own = manifest_ranges[sec]
            ev = own.get('evidence') or own['window']
            entry.update(lo=int(own['window'][0], 16), hi=int(own['window'][1], 16), alo=int(ev[0], 16),
                         ahi=int(ev[1], 16), source='config/tu-build.json window/evidence')
        else:
            data = (tu_record or {}).get('data') or {}
            if sec in data:
                d = data[sec]
                entry.update(lo=int(d['lower_bound_start'], 16), hi=int(d['upper_bound_end'], 16),
                             alo=int(d['anchored_start'], 16), ahi=int(d['anchored_end'], 16),
                             source='tu-map bounds')
            for d in (tu_record or {}).get('data_contributions') or []:
                if d.get('family') == sec:
                    entry.update(lo=int(d['start_window'][0], 16), hi=int(d['end_window'][1], 16),
                                 alo=int(d['start_window'][1], 16), ahi=int(d['end_window'][0], 16),
                                 source='tu-map windows')
        out[sec] = entry
    return out


def overlay_bss_owned_names(args, elf, obj):
    """Overlay BSS statics a TU owns, re-proved from the audited object.

    config/objects/data-carves.json `overlay_bss_records` lists the local BSS
    storage that tools/tu/overlay_link.py allocates for a recovered overlay TU.
    A recorded owner is admitted only when the audited object defines it once
    in .bss/.sbss/.scommon (or COMMON) with the recorded linkage and the
    original ELF carries the same identity, with the same binding, at the
    recorded owner address. A function-scope static is compared without
    cc1's `.N` counter (the original `cld0.8` is the object's `cld0.0`: the
    counter depends on how much of the TU is recovered), and a `D_<address>`
    placeholder is admitted only where the original has no symbol at all.
    Returns (names, reasons).
    """
    def static_base(symbol_name):
        return re.sub(r'\.\d+$', '', symbol_name)

    path = ROOT / 'config/objects/data-carves.json'
    tu_id = getattr(args, 'tu_id', None)
    if not tu_id or not path.is_file():
        return set(), []
    unit_records = (json.loads(path.read_bytes()).get('overlay_bss_records') or {}).get(args.unit) or {}
    accepted_tus = (tu_id, tu_id.split('_')[0])
    rows = [row for row in unit_records.get('recovered_location_candidates', [])
            if row.get('section') in ('.bss', '.sbss')
            and (row.get('evidence') or {}).get('original_tu') in accepted_tus]
    names, reasons = set(), []
    for row in rows:
        owner = row.get('storage_owner') or {}
        name = owner.get('name')
        if not name or not owner.get('address'):
            reasons.append(f'{row.get("address")}: record has no storage owner name/address')
            continue
        if name in names:
            continue
        defs = [s for s in obj.symbols if s['name'] == name and s['shndx'] != 0 and
                (s['section'] in ('.bss', '.sbss', '.scommon') or s['shndx'] in (SHN_COMMON, SHN_MIPS_SCOMMON))]
        if len(defs) != 1:
            reasons.append(f'{name}: {len(defs)} BSS definitions in the audited object')
            continue
        recorded_local = str(owner.get('linkage', '')).lower() in ('local', 'static', 'internal')
        if recorded_local != (defs[0]['binding'] == 0):
            reasons.append(f'{name}: object binding {defs[0]["binding"]} differs from recorded '
                           f'{owner.get("linkage")} linkage')
            continue
        address = int(owner['address'], 16)
        at_address = [s for s in elf.symbols if s['name'] and s['section_index'] != 0
                      and int(s['va'], 16) == address]
        if not at_address and name == f'D_{address:08X}':
            names.add(name)
            continue
        originals = [s for s in at_address if static_base(s['name']) == static_base(name)]
        if len(originals) != 1 or originals[0]['binding'] != defs[0]['binding']:
            original_names = sorted({s['name'] for s in at_address}) or ['no symbol']
            reasons.append(f'{name}: the original names 0x{address:08x} '
                           f'{", ".join(original_names)}')
            continue
        names.add(name)
    return names, reasons


def original_data_symbols(elf, lo, hi):
    return sorted((s for s in elf.symbols if s['type'] in (STT_NOTYPE, STT_OBJECT)
                   and 0 < s['section_index'] < 0xff00 and elf.sections[s['section_index']]['flags'] & 2
                   and lo <= int(s['va'], 16) < hi and s['name'] and s['name'] not in MARKER_NAMES
                   and not s['name'].startswith(('$', '.L'))), key=lambda s: int(s['va'], 16))


def valid_switch_word_dependency_shape(dep):
    """Validate the report's structural claim without granting data credit."""
    words = dep.get('word_spans') or []
    try:
        start = int(words[0]['scaffold_range'][0])
        end = int(words[-1]['scaffold_range'][1])
        first_object_offset = words[0]['object_offset']
        count = dep['compiler_word_count']
        label = words[0]['scaffold_name']
        match = re.fullmatch(r'jtbl_([0-9A-Fa-f]{8})', label)
        return (dep.get('credited') is False and
                dep.get('disposition') == 'uncredited_scaffold_dependency' and
                dep.get('kind') == 'compiler_region_switch_words' and
                dep.get('section') in ('.rodata', '.data') and
                type(count) is int and count > 0 and len(words) == count and
                match is not None and int(match.group(1), 16) == start and
                end - start == count * 4 and
                dep.get('c_object_section_size', 0) >= first_object_offset + count * 4 and
                all(word.get('scaffold_name') == label and
                    word.get('scaffold_range') == [start + i * 4, start + (i + 1) * 4] and
                    word.get('object_offset') == first_object_offset + i * 4 and
                    word.get('relocation_offset') == first_object_offset + i * 4 and
                    word.get('length') == 4 and
                    type(word.get('raw_rel_addend')) is int and
                    re.fullmatch(r'[0-9a-f]{64}', word.get('payload_sha256', '')) is not None
                    for i, word in enumerate(words)) and
                re.fullmatch(r'[0-9a-f]{64}', dep.get('selected_carve_sha256', '')) is not None and
                bool(dep.get('selected_carve_path')) and bool(dep.get('selected_carve_section')))
    except (KeyError, TypeError, ValueError, IndexError):
        return False


def uncredited_dependency_label(dep):
    if dep.get('kind') == 'compiler_region_switch_words':
        words = dep.get('word_spans') or []
        return words[0].get('scaffold_name') if words else None
    return dep.get('scaffold_name')


def initialized_zero_marker_review_task(connection, repository, task_id, expected_owner=None,
                                      expected_sha256=None, supplied_pin=None):
    """One actual sealed nongame mapcheck review under its assigned owner."""
    import stat
    require = data_attribute_require
    require(isinstance(task_id,str) and bool(task_id),'zero-marker review task identity is required')
    row = connection.execute('SELECT owner,status,artifact FROM tasks WHERE id=?',(task_id,)).fetchone()
    registered = Path(row[2]) if row and row[2] else Path('/nonexistent')
    if not registered.is_absolute(): registered = repository/registered
    registered = registered.resolve()
    require(row and row[0] and row[1] in ('submitted','done') and row[2] and
            (expected_owner is None or row[0] == expected_owner) and connection.execute(
                'SELECT 1 FROM resources WHERE task_id=? AND resource=?',
                (task_id,'file:'+str(registered))).fetchone() is not None,
            'zero-marker review queue owner/status/artifact/literal resource differs')
    if supplied_pin is not None: data_attribute_pin(supplied_pin,registered)
    require(registered.is_file() and not stat.S_IMODE(registered.stat().st_mode)&0o222 and
            (expected_sha256 is None or sha256_file(registered) == expected_sha256),
            'zero-marker independent review is not sealed approved bytes')
    report = json.loads(registered.read_text())
    require(report.get('schema') == 'independent-mapcheck-final-tool-review/1' and
            report.get('task') == task_id and report.get('review_task') == task_id and
            report.get('reviewer') == row[0] and report.get('verdict') == 'READY_TOOL' and
            report.get('decision') == 'accept' and report.get('final_tool_acceptance') is True and
            report.get('recovery_credit') == 0 and
            report.get('game_source_function_or_DATA_acceptance') is False,
            'zero-marker review lacks its actual assigned nongame tool verdict')
    return dict(task_id=task_id,owner=row[0],artifact=str(registered),sha256=sha256_file(registered)),report


def reviewed_initialized_zero_marker(args, section, original, spans, data_row, obj):
    """One zero-byte original marker, never a new C datum or credited span."""
    if (args is None or not getattr(args, 'data_zero_marker_proof', None) or
            args.unit != 'main' or args.tu_id != 'main/tu116' or section != '.data' or
            (original.get('name'), int(original.get('va', '0'), 16), original.get('binding'),
             original.get('type'), original.get('size')) != ('force_to_data', 0x338600, 0, STT_OBJECT, 0)):
        return None
    import ast
    import importlib.util
    import sqlite3
    import stat
    document_path = Path(args.data_zero_marker_proof).resolve()
    document = json.loads(document_path.read_text())
    require = data_attribute_require
    require(set(document) == {'schema','unit','tu','section','marker','positive_owner','positive','review',
                              'additional_recovery_credit','grants_function_acceptance'} and
            document['schema'] == 'data-zero-marker-proof/1' and
            (document['unit'],document['tu'],document['section']) == ('main','main/tu116','.data') and
            document['additional_recovery_credit'] is False and document['grants_function_acceptance'] is False,
            'unsupported zero-marker proof scope or credit')
    require(document['marker'] == dict(name='force_to_data',address='0x00338600',binding='LOCAL',type='OBJECT',size=0) and
            document['positive_owner'] == dict(name='str.0',address='0x00338600',binding='LOCAL',type='OBJECT',size=12),
            'zero-marker proof must bind the actual zero marker and positive owner')
    review = document['review']
    anchor_task = 'main-data100-identity-transport-final-review-20261020'
    approved_review = '88e782a98613ec552764af8e53ac4b073a09620577909a6c52b75d569954930a'
    require(set(review) == {'task_id','artifact'},'zero-marker review requires an actual task and artifact')
    repository = data_attribute_canonical_root()
    with sqlite3.connect('file:'+str(repository/'.work/queue.sqlite3')+'?mode=ro',uri=True) as connection:
        anchor, anchor_report = initialized_zero_marker_review_task(connection,repository,anchor_task,
            expected_owner='codex-main-data100-identity-transport-final-review',expected_sha256=approved_review)
        actual, report = initialized_zero_marker_review_task(connection,repository,review.get('task_id'),
            expected_owner=anchor['owner'],supplied_pin=review['artifact'])
    # The fixed independently approved assignment establishes owner/domain
    # authority only. The fresh actual report must attest its own current
    # positive and ordinary mapcheck; the anchor never attests a new digest.
    require(report.get('scope') == anchor_report.get('scope') and
            report.get('callable') == anchor_report.get('callable') and
            report.get('tool_snapshot',{}).get('sha256') == anchor_report.get('tool_snapshot',{}).get('sha256') and
            report.get('parser_snapshot',{}).get('sha256') == anchor_report.get('parser_snapshot',{}).get('sha256'),
            'fresh zero-marker review differs from the authenticated original marker domain/callable')
    positive_path = data_attribute_pin(document['positive'])
    require(not stat.S_IMODE(positive_path.stat().st_mode)&0o222 and
            report.get('current_positive',{}).get('path') == str(positive_path) and
            report.get('current_positive',{}).get('sha256') == sha256_file(positive_path),
            'independent review does not attest this current zero-marker proof')
    positive = json.loads(positive_path.read_text())
    require(positive.get('schema') == 'current-initialized-zero-marker-positive/1' and
            positive.get('API_equals_ordinary_proof') is True and positive.get('new_C_marker') is False and
            positive.get('new_storage_bytes') == 0 and positive.get('grants_source_or_DATA_recovery') is False,
            'invalid independent current zero-marker producer')
    inputs = positive.get('input_pins') or {}
    require(isinstance(inputs,dict) and 0 < len(inputs) <= 32, 'bounded actual marker input pins required')
    for path, digest in inputs.items(): data_attribute_pin(dict(path=path,sha256=digest))
    def current(path, digest=None):
        path = Path(path).resolve()
        require(str(path) in inputs and inputs[str(path)] == sha256_file(path) and
                (digest is None or digest == inputs[str(path)]), 'zero-marker input differs from actual current context')
        return path
    source = current(args.tu_c,data_row.get('source_sha256'))
    assembly_path = current(args.build_s,data_row.get('assembly_sha256'))
    preprocessed_path = current(assembly_path.with_suffix('.i'))
    raw_path = current(args.obj,data_row.get('object_sha256'))
    selected_path = current(data_row.get('linked_data_object_path','/nonexistent'),
                            data_row.get('linked_data_object_sha256'))
    linked_path = current(Path(args.unit_dir)/'build/main.rom.elf',data_row.get('linked_elf_sha256'))
    original_path = current(Path(args.unit_dir)/'orig/SLUS_204.69')
    registry_path = current(ROOT/'config/objects/data-carves.json')
    current(args.data_gate_report)
    current(args.candidate_manifest)
    current(Path(args.unit_dir)/'tu-manifest.carved.json')
    require(hashlib.sha256(obj.data).hexdigest() == sha256_file(selected_path),
            'zero-marker selected audit object differs')
    tool_pin, parser_pin = positive.get('tool_snapshot'), positive.get('parser_snapshot')
    tool_path, parser_path = data_attribute_pin(tool_pin), data_attribute_pin(parser_pin)
    require(tool_pin == report.get('tool_snapshot') and parser_pin == report.get('parser_snapshot') and
            tool_pin['sha256'] == 'e23c3bf674da31363a5f06f6e482f9d1ab2baf52f1fd166915820a446a46d616' and
            parser_pin['sha256'] == 'a6385e9748bb85181af274136e9c75596af3a21fa0d20cae1a82ed1ccbb2097f' and
            all(not stat.S_IMODE(path.stat().st_mode)&0o222 for path in (tool_path,parser_path)),
            'zero-marker callable/parser is not the exact independently reviewed snapshot')
    # Load only the exact approved parser and two unchanged pure function nodes;
    # no mutable import origin can select a different Elf or mapcheck helper.
    module_name = '_tu_zero_marker_elf_'+parser_pin['sha256']
    spec = importlib.util.spec_from_file_location(module_name,parser_path)
    parser = importlib.util.module_from_spec(spec);sys.modules[module_name] = parser;spec.loader.exec_module(parser)
    tree = ast.parse(tool_path.read_text())
    nodes = [node for node in tree.body if isinstance(node,ast.FunctionDef) and
             node.name in ('compiler_data_definitions','initialized_zero_marker_identity')]
    require(len(nodes) == 2,'zero-marker approved callable nodes missing')
    namespace = dict(Path=Path,hashlib=hashlib,struct=struct,re=re,Elf=parser.Elf)
    exec(compile(ast.Module(body=nodes,type_ignores=[]),str(tool_path),'exec'),namespace)
    original_image, selected_image, raw_image, linked_image = [parser.Elf(path.read_bytes())
        for path in (original_path,selected_path,raw_path,linked_path)]
    markers = [s for s in original_image.symbols if s.name == 'force_to_data' and s.value == 0x338600 and
               (s.bind,s.type,s.size,s.other) == (0,1,0,0) and 0 < s.shndx < len(original_image.sections) and
               original_image.sections[s.shndx].name == '.data']
    require(len(markers) == 1,'zero-marker actual original identity is not unique')
    aliases = [alias for run in json.loads(registry_path.read_text()).get('tus',{}).get(args.tu_id,{}).get(section,[])
               for alias in run.get('c_storage_aliases',[])]
    expected = positive['current_proof'];owner_proof = expected['owner_proof']
    owner_spans = [span for span in spans if int(span.get('address','0'),16) == 0x338600 and
        span.get('length') == 12 and span.get('object_symbols') == ['str_0'] and
        span.get('names') == ['str_0'] and span.get('c_object_section') == '.data' and
        span.get('c_object_offset') == 0 and span.get('c_object_section_size') == 12 and
        any(i.get('name') == 'str.0' and i.get('binding') == 'LOCAL' and i.get('type') == 'OBJECT' and
            i.get('size') == 12 for i in span.get('original_identities',[])) and
        any(i.get('name') == 'force_to_data' and i.get('binding') == 'LOCAL' and i.get('type') == 'OBJECT' and
            i.get('size') == 0 for i in span.get('original_identities',[]))]
    require(len(owner_spans) == 1 and expected.get('owner_alias_record') in aliases and
            owner_proof.get('c_object_path') == str(selected_path), 'zero-marker lacks its actual normal positive span/alias')
    ordinary_path = data_attribute_pin({k:report['ordinary_main_report'][k] for k in ('path','sha256')})
    current(ordinary_path)
    ordinary = json.loads(ordinary_path.read_text())
    require(ordinary.get('ok') is True and ordinary.get('problems') == [] and
            [p for p in ordinary.get('c_tu_audit',{}).get('game',{}).get('initialized_zero_marker_proofs',[])
             if p.get('original_symbol') == 'force_to_data'] == [expected],
            'zero-marker proof differs from the genuine passing ordinary mapcheck')
    assembly = assembly_path.read_text();pre = Preprocessed(preprocessed_path.read_text())
    tu = tu_edit.parse(source.read_text());pre_tu = tu_edit.parse(pre.text)
    items = [item for item in tu.free_items() if item.kind == 'decl' and not item.conditional and
             '=' in tu_edit.c_tokens(item.text(tu.text)) and ('ordinary','str_0') in tu_edit.decl_keys(item.text(tu.text))]
    require(len(items) == 1,'zero-marker positive owner must have one actual source definition')
    context = dict(pre_tu=pre_tu,pre=pre,unit_dir=Path(args.unit_dir),source=source)
    data_attribute_declaration(context,tu,items[0],'str_0')
    payload = original_image.section_bytes(original_image.sections[markers[0].shndx])[
        markers[0].value-original_image.sections[markers[0].shndx].addr:markers[0].value-original_image.sections[markers[0].shndx].addr+12]
    data_attribute_character_source(tu,items[0],'str_0',payload)
    require(namespace['compiler_data_definitions'](assembly).get('str_0') == owner_proof.get('compiler_storage') and
            not any(re.search(r'\b(?:str_0|force_to_data)\b',line) for text in (tu.text,pre.text)
                    for line in asm_statement_lines(text)), 'zero-marker owner is not actual typed cc1 DATA')
    actual = namespace['initialized_zero_marker_identity'](markers[0],section,original_image,selected_image,
                raw_image,linked_image,aliases,owner_proof,raw_path)
    require(actual == expected and actual.get('new_storage_bytes') == 0 and actual.get('grants_data_recovery') is False,
            'zero-marker unchanged API does not reproduce exact independently reviewed proof')
    return dict(name='force_to_data',address='0x00338600',size=0,positive_owner='str.0',c_owner='str_0',
                kind='reviewed_initialized_zero_marker_identity',proof=actual,independent_review=review,
                proof_document=dict(path=str(document_path),sha256=sha256_file(document_path)),
                credited=False,credited_bytes=0,new_storage_bytes=0,grants_data_recovery=False)


def linked_span_data_check(section, rng, elf, obj, data_row, args=None):
    """Check C data against the exact carved-object spans pinned by R22."""
    spans = [dict(span) for span in (data_row or {}).get('verified_location_spans', [])
             if span.get('section') == section]
    spans = [span for span in spans if rng['lo'] <= int(span.get('address', '0'), 16) < rng['hi']]
    if not spans:
        return False, dict(reasons=['linked-data report has no verified object spans in the declared run'],
                           range_source=rng.get('source')), set()
    selected_sha = (data_row or {}).get('linked_data_object_sha256', (data_row or {}).get('object_sha256'))
    if any(span.get('object_sha256') != selected_sha for span in spans):
        return False, dict(reasons=['verified object span hash differs from the selected linked-data object']), set()
    by_section = {item['name']: item for item in obj.sections}
    symbols = [item for item in obj.symbols if item['type'] in (STT_NOTYPE, STT_OBJECT)
               and item['name'] and not item['name'].startswith(('$', '.L'))
               and item['name'] not in MARKER_NAMES]
    aliases = [dict(alias) for alias in (data_row or {}).get('symbol_aliases', [])
               if alias.get('section') == section]
    aliases_by_original = {alias.get('scaffold_name'): alias for alias in aliases}
    originals = original_data_symbols(elf, rng['lo'], rng['hi'])
    runs = rng.get('runs')
    if runs:
        originals = [item for item in originals if any(lo <= int(item['va'], 16) < hi for lo, hi in runs)]
    reasons, accounted, matched = [], set(), set()
    dependency_names = set()
    dependency_proofs = []
    alias_address_witnesses = []
    raw_path = Path(args.obj).resolve() if args is not None else None
    raw_obj = RelObject(raw_path.read_bytes()) if raw_path is not None else None
    build_s = (Path(args.build_s).resolve() if args is not None and getattr(args, 'build_s', None)
               else raw_path.with_name(raw_path.name + '.s') if raw_path is not None else None)
    linked_path = (Path(args.unit_dir) / 'build' / f'{args.unit}.rom.elf').resolve() if args is not None else None
    linked_data = linked_path.read_bytes() if linked_path is not None and linked_path.is_file() else None
    empty_objects = {}
    if raw_obj is not None and build_s is not None and build_s.is_file() and linked_data is not None:
        from tools.inventory import ELF as InventoryELF
        linked_image = InventoryELF(linked_data, args.unit)
        outside = re.sub(r'^\s*#APP\s*$.*?^\s*#NO_APP\s*$', '',
                         build_s.read_text(errors='replace'), flags=re.M | re.S)
        for original in originals:
            if original['type'] != STT_OBJECT or original['size'] != 0:
                continue
            name, address = original['name'], int(original['va'], 16)
            owners = [s for s in originals if int(s['va'], 16) == address and
                      s['type'] == STT_OBJECT and s['size'] > 0]
            if len(owners) != 1:
                continue
            owner = owners[0]
            proofs = [s for s in spans if int(s.get('address', '0'), 16) == address and
                      name in s.get('object_symbols', []) and
                      owner['name'] in s.get('object_symbols', []) and owner['name'] in s.get('names', [])]
            selected = [s for s in symbols if s['name'] == name]
            selected_owner = [s for s in symbols if s['name'] == owner['name']]
            raw = [s for s in raw_obj.symbols if s['name'] == name]
            raw_owner = [s for s in raw_obj.symbols if s['name'] == owner['name']]
            if any(len(values) != 1 for values in (proofs, selected, selected_owner, raw, raw_owner)):
                continue
            span, selected, selected_owner, raw, raw_owner = (
                proofs[0], selected[0], selected_owner[0], raw[0], raw_owner[0])
            def identity(symbol, witness):
                return (symbol['type'], symbol['binding'], symbol['size']) == (
                    witness['type'], witness['binding'], witness['size'])
            linked_exact = all(len([s for s in linked_image.symbols
                                   if s['name'] == witness['name'] and int(s['va'], 16) == address and
                                   s.get('section') == section and identity(s, witness)]) == 1
                               for witness in (original, owner))
            compiler_span = span.get('compiler_input_span') or {}
            if (identity(selected, original) and identity(raw, original) and
                    identity(selected_owner, owner) and identity(raw_owner, owner) and linked_exact and
                    selected['section'] == selected_owner['section'] == span.get('c_object_section') and
                    selected['value'] == selected_owner['value'] == span.get('c_object_offset') and
                    raw['section'] == raw_owner['section'] == compiler_span.get('section') and
                    raw['value'] == raw_owner['value'] == compiler_span.get('offset') and
                    span.get('length') == owner['size'] and
                    data_row.get('object_sha256') == sha256_file(raw_path) and
                    data_row.get('assembly_sha256') == sha256_file(build_s) and
                    data_row.get('source_sha256') == sha256_file(Path(args.tu_c)) and
                    data_row.get('linked_elf_sha256') == sha256_file(linked_path) and
                    len(re.findall(r'^\s*\.type\s+' + re.escape(name) + r'\s*,\s*@object\s*$', outside, re.M)) == 1 and
                    len(re.findall(r'^\s*\.size\s+' + re.escape(name) + r'\s*,\s*0\s*$', outside, re.M)) == 1 and
                    len(re.findall(r'^' + re.escape(name) + r':\s*$', outside, re.M)) == 1):
                empty_objects[name] = dict(name=name, address=original['va'], size=0,
                                          credited_bytes=0, positive_owner=owner['name'])

    def elf_va_bytes(image, address, length):
        for ph in image.program_headers:
            lo, size = int(ph['va'], 16), ph['file_size']
            if ph['type'] == 1 and lo <= address and address + length <= lo + size:
                off = ph['file_offset'] + address - lo
                return image.data[off:off + length]
        return None

    # A report can also account an exact scaffold-backed compiler dependency
    # without crediting it as recovered C data. Validate the raw cc1 bytes,
    # selected carved-object bytes, original and linked bytes, plus every
    # current sidecar hash before admitting its original symbol to the
    # object-wide accounting set.
    for dep in (data_row or {}).get('uncredited_compiler_dependencies', []):
        if dep.get('section') != section or dep.get('kind') not in (
                'inline_assembly_data', 'compiler_region_switch_words'):
            continue
        if dep.get('kind') == 'compiler_region_switch_words':
            try:
                words = dep['word_spans']
                start = int(words[0]['scaffold_range'][0])
                length = len(words) * 4
                end = start + length
                raw_section = raw_obj.section(section)
                selected_path = Path(dep['selected_carve_path']).resolve()
                linker_path = Path(dep['linker_script_path']).resolve()
                selected_obj = RelObject(selected_path.read_bytes())
                selected_section = selected_obj.section(dep['selected_carve_section'])
                raw_bytes = raw_obj.bytes_of(raw_section)
                selected_bytes = selected_obj.bytes_of(selected_section)
                original_bytes = elf_va_bytes(elf, start, length)
                from tools.inventory import ELF as InventoryELF
                linked_image = InventoryELF(linked_data, args.unit) if linked_data is not None else None
                linked_bytes = elf_va_bytes(linked_image, start, length) if linked_image is not None else None
                relocations = {(sec, offset, kind) for sec, offset, kind, _name in raw_obj.relocations()}
                selected_relocations = {(sec, offset, kind) for sec, offset, kind, _name
                                        in selected_obj.relocations()}
                dep_label = uncredited_dependency_label(dep)
                first_offset = words[0]['object_offset']
                raw_table = raw_bytes[first_offset:first_offset + length]
                selected_offsets = [offset for offset in range(max(0, len(selected_bytes) - length + 1))
                                    if selected_bytes[offset:offset + length] == raw_table]
                selected_offset = selected_offsets[0] if len(selected_offsets) == 1 else None
                exact = (
                    valid_switch_word_dependency_shape(dep) and
                    raw_section is not None and
                    raw_section['size'] == dep.get('c_object_section_size') and
                    raw_section['align'] == dep.get('c_object_section_alignment') and
                    dep.get('object_sha256') == data_row.get('object_sha256') == sha256_file(raw_path) and
                    dep.get('source_sha256') == data_row.get('source_sha256') == sha256_file(Path(args.tu_c)) and
                    dep.get('assembly_sha256') == data_row.get('assembly_sha256') == sha256_file(build_s) and
                    dep.get('linked_elf_sha256') == data_row.get('linked_elf_sha256') ==
                    sha256_file(linked_path) and
                    dep.get('original_elf_sha256') == hashlib.sha256(elf.data).hexdigest() and
                    dep.get('selected_carve_sha256') == sha256_file(selected_path) and
                    dep.get('linker_script_sha256') == sha256_file(linker_path) and
                    selected_section is not None and selected_offset is not None and
                    original_bytes == linked_bytes and
                    all((section, word['relocation_offset'], 2) in relocations and
                        (selected_section['name'], selected_offset + i * 4, 2) in selected_relocations and
                        int.from_bytes(raw_bytes[word['object_offset']:word['object_offset'] + 4], 'little') ==
                        word['raw_rel_addend'] and
                        hashlib.sha256(raw_bytes[word['object_offset']:word['object_offset'] + 4]).hexdigest() ==
                        word['payload_sha256'] and
                        int.from_bytes(original_bytes[i * 4:(i + 1) * 4], 'little') ==
                        int(word['original_linked_word'], 16)
                        for i, word in enumerate(words)) and
                    not any(max(start, int(span.get('address', '0'), 16)) <
                            min(end, int(span.get('address', '0'), 16) + int(span.get('length', 0)))
                            for span in spans))
            except (KeyError, TypeError, ValueError, OSError, AttributeError, IndexError):
                exact = False
            if not exact:
                reasons.append(f'{uncredited_dependency_label(dep) or "switch-table dependency"}: uncredited compiler table '
                               'does not match exact current raw/selected/original/link evidence')
                continue
            dependency_names.add(dep_label)
            dependency_proofs.append(dict(name=dep_label, length=length, credited=False,
                                          raw_sha256=dep['object_sha256'],
                                          selected_sha256=dep['selected_carve_sha256']))
            continue
        try:
            lo, hi = map(int, dep['scaffold_range'])
            off, length = int(dep['c_object_offset']), int(dep['length'])
            csec = raw_obj.section(dep['c_object_section'])
            selected_sec = obj.section(dep['selected_section'])
            selected_off = int(dep['selected_section_offset'])
            alloc_path = Path(dep['allocation_report_path']).resolve()
            carve_path = Path(dep['carve_sidecar_path']).resolve()
            selected_path = Path(dep['selected_object_path']).resolve()
            allocation_reports = [Path(p).resolve() for p in (getattr(args, 'allocation_report', None) or [])]
            carve_reports = [Path(p).resolve() for p in (getattr(args, 'data_carve_report', None) or [])]
            payload = raw_obj.bytes_of(csec)[off:off + length]
            selected_bytes = obj.bytes_of(selected_sec)[selected_off:selected_off + length]
            original_bytes = elf_va_bytes(elf, lo, length)
            from tools.inventory import ELF as InventoryELF
            linked_image = InventoryELF(linked_data, args.unit) if linked_data is not None else None
            linked_bytes = elf_va_bytes(linked_image, lo, length) if linked_image is not None else None
            label = dep['compiler_assembly_label']
            asm_text = build_s.read_text(errors='replace') if build_s and build_s.is_file() else ''
            label_pos = asm_text.find(label + ':')
            app_pos = asm_text.rfind('#APP', 0, label_pos) if label_pos >= 0 else -1
            no_app_pos = asm_text.find('#NO_APP', label_pos) if label_pos >= 0 else -1
            alloc_report = json.loads(alloc_path.read_text())
            allocation_input = alloc_report.get('input') or {}
            allocation_output = alloc_report.get('output') or {}
            carve_report = json.loads(carve_path.read_text())
            allocated_path = Path(allocation_output.get('path', '/nonexistent')).resolve()
            original_names = [s for s in original_data_symbols(elf, lo, hi)
                              if s['name'] == dep['scaffold_name'] and int(s['va'], 16) == lo]
            exact = (
                dep.get('credited') is False and dep.get('disposition') == 'uncredited_scaffold_dependency' and
                dep.get('section') == section and hi - lo == length and lo < hi and
                dep.get('c_object_section') == section and csec is not None and
                off >= 0 and off + length <= csec['size'] and selected_sec is not None and
                selected_path == Path(getattr(args, 'data_carve_object', None) or
                                      getattr(args, 'data_gate_object', '')).resolve() and
                selected_path == Path(args.data_gate_object).resolve() and
                dep.get('selected_object_sha256') == sha256_file(selected_path) == data_row.get('object_sha256') and
                dep.get('raw_object_sha256') == sha256_file(raw_path) and
                dep.get('source_sha256') == data_row.get('source_sha256') == sha256_file(Path(args.tu_c)) and
                dep.get('assembly_sha256') == data_row.get('assembly_sha256') == sha256_file(build_s) and
                dep.get('linked_elf_sha256') == data_row.get('linked_elf_sha256') == sha256_file(linked_path) and
                alloc_path in allocation_reports and sha256_file(alloc_path) == dep.get('allocation_report_sha256') and
                allocation_input.get('sha256') == sha256_file(raw_path) and
                allocation_output.get('sha256') == dep.get('allocated_object_sha256') and
                Path(carve_report.get('object', '/nonexistent')).resolve() == allocated_path and
                Path(carve_report.get('out', '/nonexistent')).resolve() == selected_path and
                carve_path in carve_reports and sha256_file(carve_path) == dep.get('carve_sidecar_sha256') and
                dep.get('selected_section_offset') == selected_off and
                hashlib.sha256(payload).hexdigest() == dep.get('payload_sha256') and
                selected_bytes == payload and original_bytes == payload and linked_bytes == payload and
                len(original_names) == 1 and original_names[0]['size'] in (0, length) and
                label_pos >= 0 and app_pos >= 0 and no_app_pos >= label_pos and
                not any(max(lo, int(span.get('address', '0'), 16)) <
                        min(hi, int(span.get('address', '0'), 16) + int(span.get('length', 0)))
                        for span in spans))
        except (KeyError, TypeError, ValueError, OSError, AttributeError, IndexError):
            exact = False
        if not exact:
            reasons.append(f'{dep.get("scaffold_name", "inline assembly data")}: uncredited dependency does not match exact current raw/selected/original/link evidence')
            continue
        dependency_names.add(dep['scaffold_name'])
        accounted.add(dep['scaffold_name'])
        matched.add(dep['scaffold_name'])
        dependency_proofs.append(dict(name=dep['scaffold_name'], offset=off, length=length,
                                      credited=False, raw_sha256=dep['raw_object_sha256'],
                                      selected_sha256=dep['selected_object_sha256']))
    # A gate may prove a real named C object whose retail bytes have no named
    # original ELF symbol (for example a local literal array). Such a name is
    # creditable only through this exact selected-object span; the linked-data
    # report also binds its bytes and final linked address.
    for span in spans:
        try:
            offset = int(span['c_object_offset'])
            length = int(span['length'])
            section_size = int(span['c_object_section_size'])
            section_align = int(span['c_object_section_alignment'])
        except (KeyError, TypeError, ValueError):
            reasons.append('malformed selected-object span for a named C data object')
            continue
        csec = by_section.get(span.get('c_object_section'))
        address = int(span.get('address', '0'), 16)
        if (csec is None or csec['size'] != section_size or csec['align'] != section_align or
                offset < 0 or length <= 0 or offset + length > section_size or
                not any(lo <= address and address + length <= hi
                        for lo, hi in (runs or [(rng['lo'], rng['hi'])]))):
            reasons.append('selected-object span lies outside its exact section or declared run')
            continue
        for object_name in span.get('object_symbols', []):
            if object_name in empty_objects and int(empty_objects[object_name]['address'], 16) == address:
                continue
            candidates = [symbol for symbol in symbols if symbol['name'] == object_name]
            if (len(candidates) != 1 or candidates[0].get('section') != csec['name'] or
                    candidates[0].get('value') != offset or candidates[0].get('size', 0) <= 0 or
                    candidates[0].get('size', 0) > length):
                reasons.append(f'{object_name}: no unique selected-object symbol matches its verified span')
                continue
            matched.add(object_name)
            accounted.add(object_name)
        # Anonymous cc1 strings/tables can have a scaffold label without a
        # named ELF symbol. Their exact bytes and relocations are proved by
        # this current, hash-bound data gate; do not require a fabricated C
        # symbol merely to account for that label.
        if not any(int(original['va'], 16) == address for original in originals):
            matched.update(span.get('names', []))
    zero_markers = {}
    for original in originals:
        marker = reviewed_initialized_zero_marker(args,section,original,spans,data_row,obj)
        if marker:
            zero_markers[original['name']] = marker
    for original in originals:
        original_name = original['name']
        address = int(original['va'], 16)
        if original_name in zero_markers:
            accounted.add(original_name)
            matched.add(original_name)
            continue
        if original_name in empty_objects:
            accounted.add(original_name)
            matched.add(original_name)
            continue
        if original_name in dependency_names:
            # The linker scaffold remains the owner; the C switch words are
            # accepted only as a physical dependency of their compiler-emitted
            # function and receive no source-data credit.
            continue
        alias = aliases_by_original.get(original_name)
        object_name = alias.get('object_symbol') if alias else original_name
        candidates = [item for item in symbols if item['name'] == object_name]
        if len(candidates) != 1:
            reasons.append(f'original {original_name} @{original["va"]} has no unique selected C object {object_name}')
            continue
        symbol = candidates[0]
        proof_spans = [span for span in spans
                       if int(span.get('address', '0'), 16) == address and
                       object_name in (span.get('object_symbols') or []) and
                       object_name in (span.get('names') or [])]
        if len(proof_spans) != 1:
            reasons.append(f'{original_name} @{original["va"]}: no unique linked-data span for {object_name}')
            continue
        span = proof_spans[0]
        csec = by_section.get(span.get('c_object_section'))
        try:
            offset = int(span['c_object_offset'])
            length = int(span['length'])
            span_size = int(span['c_object_section_size'])
            span_align = int(span['c_object_section_alignment'])
        except (KeyError, TypeError, ValueError):
            reasons.append(f'{original_name}: malformed linked object span')
            continue
        if (csec is None or csec['size'] != span_size or csec['align'] != span_align or
                offset < 0 or length <= 0 or offset + length > csec['size'] or
                symbol['section'] != csec['name'] or symbol['value'] != offset or
                symbol['size'] != original['size'] or symbol['type'] != original['type'] or
                symbol['binding'] != original['binding']):
            reasons.append(f'{original_name} -> {object_name}: selected object identity/span differs from original')
            continue
        if alias:
            linked_matches = []
            linked_other = []
            if linked_data is not None:
                from tools.inventory import ELF as InventoryELF
                linked_image = InventoryELF(linked_data, args.unit)
                named_matches = [item for item in linked_image.symbols
                                 if item.get('name') == object_name and
                                 int(item.get('va', '0'), 16) == address]
                original_section = elf.sections[original['section_index']]
                for item in named_matches:
                    index = item.get('section_index', 0)
                    exact = (item.get('size') == original['size'] and
                             item.get('type') == original['type'] and
                             item.get('binding') == original['binding'] and
                             item.get('section') == original.get('section') and
                             0 < index < len(linked_image.sections) and
                             linked_image.sections[index]['type'] == original_section['type'] and
                             linked_image.sections[index]['flags'] & 2)
                    if exact:
                        linked_matches.append(item)
                    else:
                        linked_other.append(item)
            address_witnesses = [item for item in linked_other
                                 if item.get('section_index') == 0xfff1 and item.get('section') is None and
                                 item.get('binding') == 1 and item.get('type') == STT_NOTYPE and
                                 item.get('size') == 0]
            if (alias.get('section') != section or alias.get('scaffold_name') != original_name or
                    alias.get('object_symbol') != object_name or
                    int(alias.get('address', '0'), 16) != address or alias.get('size') != original['size'] or
                    alias.get('binding') != BIND.get(original['binding']) or
                    alias.get('type') != {STT_NOTYPE: 'NOTYPE', STT_OBJECT: 'OBJECT'}.get(original['type']) or
                    alias.get('binding') != BIND.get(symbol['binding']) or
                    alias.get('type') != {STT_NOTYPE: 'NOTYPE', STT_OBJECT: 'OBJECT'}.get(symbol['type']) or
                    alias.get('size') != symbol['size'] or len(linked_matches) != 1 or
                    linked_matches[0].get('size') != original['size'] or
                    linked_matches[0].get('type') != original['type'] or
                    linked_matches[0].get('binding') != original['binding'] or
                    len(address_witnesses) != len(linked_other) or
                    data_row.get('linked_elf_sha256') != sha256_file(linked_path)):
                reasons.append(f'{original_name} -> {object_name}: linked alias identity is not exact')
                continue
            alias_address_witnesses.extend(dict(name=item['name'], address=item['va'],
                                                binding='GLOBAL', type='NOTYPE', size=0,
                                                section_index=0xfff1, credited=False,
                                                original_name=original_name,
                                                disposition='absolute_linker_address_witness')
                                           for item in address_witnesses)
        accounted.add(original_name)
        # The exact selected-object span proves this original identity even
        # when the C object uses a different, verified alias name.  Keep both
        # names in the accounting set; alias identity, size, binding, type,
        # address and linked-symbol checks above remain mandatory.
        matched.add(original_name)
        accounted.add(object_name)
        matched.add(object_name)
    for empty in empty_objects.values():
        if empty['positive_owner'] not in accounted:
            reasons.append(f'{empty["name"]}: colocated positive owner did not pass its ordinary linked span checks')
    for marker in zero_markers.values():
        if not {marker['positive_owner'],marker['c_owner']} <= accounted:
            reasons.append(f'{marker["name"]}: positive owner did not pass its unchanged ordinary linked span checks')
    expected_c_names = {name for item in (data_row or {}).get('verified_locations', [])
                        if item.get('section') == section and rng['lo'] <= int(item['address'], 16) < rng['hi']
                        for name in item.get('names', [])}
    expected_c_names |= {alias.get('object_symbol') for alias in aliases
                         if rng['lo'] <= int(alias.get('address', '0'), 16) < rng['hi']}
    if expected_c_names - matched:
        reasons.append(f'linked data names lack matching carved-object spans: {sorted(expected_c_names - matched)}')
    return not reasons, dict(reasons=reasons, range_source=rng.get('source'),
                             original_symbols=len(originals), matched=len(accounted),
                             selected_spans=len(spans), selected_object_size=sum(by_section[s['c_object_section']]['size']
                                                                               for s in spans
                                                                               if s.get('c_object_section') in by_section),
                             uncredited_dependencies=dependency_proofs,
                             alias_address_witnesses=alias_address_witnesses,
                             verified_empty_objects=list(empty_objects.values()),
                             initialized_zero_marker_identities=list(zero_markers.values()),
                             detail='each claimed original identity is pinned to its exact selected-object span'), accounted


def data_section_check(section, rng, elf, obj, scaffold_names):
    """(ok, detail) for one C-owned data section, and the names it accounts for."""
    reasons, accounted = [], set()
    if rng.get('lo') is None:
        return False, dict(reasons=['C-owned section without a known original range (pass '
                                    '--c-owned SECTION=START:END)']), accounted
    storage_rows = rng.get('verified_storage') or []
    if section in ('.bss', '.sbss') and storage_rows:
        reasons, accounted = [], set()
        expected_type = {'NOTYPE': STT_NOTYPE, 'OBJECT': STT_OBJECT}
        for storage in storage_rows:
            candidates = [symbol for symbol in obj.symbols if symbol.get('name') == storage.get('name')]
            if len(candidates) != 1:
                reasons.append(f'{storage.get("name")}: verified storage owner is not defined once by the C object')
                continue
            symbol = candidates[0]
            common_symbol = symbol.get('shndx') in (SHN_COMMON, SHN_MIPS_SCOMMON)
            allowed_sections = (section, '.scommon') if section == '.sbss' else ('.bss', '.scommon')
            if (symbol.get('binding') != storage.get('bind') or
                    symbol.get('type') != expected_type.get(storage.get('type')) or
                    symbol.get('size') not in (0, storage.get('size')) or
                    (symbol.get('section') not in allowed_sections and not common_symbol)):
                reasons.append(f'{storage.get("name")}: emitted identity differs from hash-bound linked storage')
                continue
            accounted.add(storage['name'])
        storage_by_address = {int(item['address'], 16): item for item in storage_rows}
        originals = original_data_symbols(elf, rng['lo'], rng['hi'])
        if rng.get('runs'):
            originals = [item for item in originals
                         if any(lo <= int(item['va'], 16) < hi for lo, hi in rng['runs'])]
        for original in originals:
            address = int(original['va'], 16)
            storage = storage_by_address.get(address)
            if storage is None:
                reasons.append(f'original {original["name"]} @{original["va"]} is not covered by a '
                               'hash-bound C storage owner')
                continue
            if original['size'] > 0 and (original['binding'] != storage.get('bind') or
                                         original['size'] != storage.get('size') or
                                         original['type'] != expected_type.get(storage.get('type'))):
                reasons.append(f'{original["name"]}: meaningful original ELF identity differs from linked '
                               'C storage owner')
                continue
            accounted.add(original['name'])
        declared_storage = {item.get('name') for item in storage_rows}
        if not declared_storage.issubset(set(rng.get('verified_names') or [])):
            reasons.append('verified storage names are not present in the C-owned data evidence')
        return not reasons, dict(reasons=reasons, range_source=rng['source'],
                                 original_symbols=len(originals), matched=len(accounted),
                                 object_size=sum(int(item.get('size', 0)) for item in storage_rows),
                                 piece_start=f'0x{rng["lo"]:08x}', verified_storage=storage_rows,
                                 detail='C storage emission and final linked placement are hash-bound by the '
                                        'linked-data gate'), accounted
    names = {'.rodata': ('.rodata', '.rdata')}.get(section, (section,))
    secs = [x for x in obj.sections if x['name'] in names]
    if len(secs) != 1:
        detail = (f'the object has no {section} section, so the C-owned range '
                  f'0x{rng["lo"]:08x}..0x{rng["hi"]:08x} does not come from this TU\'s C'
                  if not secs else f'the object has {len(secs)} {section} sections')
        return False, dict(reasons=[detail], partial=rng.get('partial'),
                           range_source=rng.get('source')), accounted
    sec = secs[0]
    lo, hi = rng['lo'], rng['hi']
    originals = original_data_symbols(elf, lo, hi)
    runs = rng.get('runs')
    if runs:
        # several runs: the original symbols of the runs, not of the scaffold items
        # between them; the build places each run's slice at its own address
        # (tools/tu/data_carve.py split), so there is no single piece start to check
        originals = [o for o in originals if any(a <= int(o['va'], 16) < b for a, b in runs)]
    defined = [x for x in obj.symbols if x['shndx'] == sec['index'] and x['type'] in (STT_NOTYPE, STT_OBJECT)
               and x['name'] and not x['name'].startswith(('$', '.L')) and x['name'] not in scaffold_names
               and x['name'] not in MARKER_NAMES]
    by_name = {}
    for x in defined:
        by_name.setdefault(x['name'], []).append(x)
    deltas, matched = set(), []
    for alias in rng.get('symbol_aliases', []):
        name = alias['object_symbol']
        cands = by_name.get(name, [])
        if len(cands) != 1:
            reasons.append(f'scaffold alias {alias["scaffold_name"]} -> {name} is not defined once by the C '
                           f'object in {section}')
            continue
        x = cands[0]
        expected_binding = {'LOCAL': 0, 'GLOBAL': 1, 'WEAK': 2}.get(alias.get('binding'))
        expected_type = {'NOTYPE': STT_NOTYPE, 'OBJECT': STT_OBJECT, 'FUNC': STT_FUNC}.get(alias.get('type'))
        try:
            address = int(alias['address'], 16)
            size = int(alias['size'])
        except (KeyError, TypeError, ValueError):
            reasons.append(f'scaffold alias {alias["scaffold_name"]} -> {name} has malformed address/size')
            continue
        original_alias_ok = True
        if alias.get('evidence') == 'exact_same_address_original_and_linked_C_OBJECT':
            original_alias_ok = any(
                o['name'] == alias.get('scaffold_name') and int(o['va'], 16) == address and
                o['binding'] == expected_binding and o['type'] == expected_type and o['size'] == size
                for o in originals)
        if (alias.get('section') != section or expected_binding is None or expected_type is None or
                x['binding'] != expected_binding or x['type'] != expected_type or x['size'] != size or
                not original_alias_ok):
            reasons.append(f'{alias["scaffold_name"]} -> {name}: object {BIND.get(x["binding"])}/{x["size"]}/'
                           f'type {x["type"]} vs gate alias {alias.get("binding")}/{size}/type '
                           f'{alias.get("type")} in {section}' +
                           ('' if original_alias_ok else '; original ELF identity mismatch'))
            continue
        accounted.add(name)
        deltas.add(address - x['value'])
        matched.append((x['value'], x['value'] + x['size'], name))
    for o in originals:
        cands = by_name.get(o['name'], [])
        if len(cands) != 1:
            direct_alias = next((alias for alias in rng.get('symbol_aliases', [])
                                 if alias.get('evidence') == 'exact_same_address_original_and_linked_C_OBJECT' and
                                 alias.get('scaffold_name') == o['name'] and
                                 int(alias.get('address', '0'), 16) == int(o['va'], 16) and
                                 alias.get('binding') == BIND.get(o['binding']) and
                                 alias.get('type') == {STT_NOTYPE: 'NOTYPE', STT_OBJECT: 'OBJECT'}.get(o['type']) and
                                 alias.get('size') == o['size']), None)
            alias_candidates = by_name.get(direct_alias.get('object_symbol'), []) if direct_alias else []
            if len(alias_candidates) == 1:
                x = alias_candidates[0]
                if (x['binding'] == o['binding'] and x['type'] == o['type'] and x['size'] == o['size'] and
                        direct_alias.get('section') == section):
                    accounted.add(o['name'])
                    continue
            if o['name'] in rng.get('generated_aliases', []):
                address = int(o['va'], 16)
                if any(int(span[0]) <= address < int(span[1]) for span in
                       (rng.get('compiler_owned_ranges') or [])):
                    accounted.add(o['name'])
                    continue
            reasons.append(f'original {o["name"]} @{o["va"]} ({o["size"]} bytes) is not defined once by the C '
                           f'object in {section}')
            continue
        x = cands[0]
        accounted.add(o['name'])
        if x['binding'] != o['binding'] or x['size'] != o['size'] or x['type'] != o['type']:
            reasons.append(f'{o["name"]}: object {BIND.get(x["binding"])}/{x["size"]}/type {x["type"]} vs original '
                           f'{BIND.get(o["binding"])}/{o["size"]}/type {o["type"]}')
        deltas.add(int(o['va'], 16) - x['value'])
        matched.append((x['value'], x['value'] + x['size'], o['name']))
    expected_names = {o['name'] for o in originals} | set(rng.get('verified_names') or [])
    if rng.get('source') == 'config/objects/data-carves.json' and rng.get('verified_names'):
        by_name_defined = {name: values for name, values in by_name.items()}
        loc_address = {name: int(item['address'], 16)
                       for item in rng.get('verified_locations', [])
                       for name in item.get('names', [])}
        for name in rng['verified_names']:
            if len(by_name_defined.get(name, [])) != 1:
                address = loc_address.get(name)
                anonymous = any(a <= address < b for a, b in rng.get('anonymous_ranges', [])
                                ) if address is not None else False
                if not anonymous:
                    reasons.append(f'linked-data-gate symbol {name} is neither defined once nor covered by '
                                   f'compiler-proven anonymous constants in {section}')
            else:
                accounted.add(name)
    extra = sorted(x['name'] for x in defined if x['name'] not in expected_names)
    if extra:
        reasons.append(f'named data the original range lacks: {extra[:10]}')
    piece = None
    if runs:
        deltas = set()
    if len(deltas) > 1:
        reasons.append(f'symbols disagree on the piece start: {sorted(hex(d) for d in deltas)[:4]}')
    elif deltas:
        piece = deltas.pop()
    if piece is None and rng['exact'] and sec['size'] > hi - lo:
        reasons.append(f'the object\'s {section} ({sec["size"]} bytes) does not fit the C-owned range '
                       f'0x{lo:08x}..0x{hi:08x}')
    if piece is not None:
        if rng['exact'] and piece != lo:
            reasons.append(f'piece starts at 0x{piece:08x}, range starts at 0x{lo:08x}')
        if piece < lo or piece + sec['size'] > hi:
            reasons.append(f'piece 0x{piece:08x}+0x{sec["size"]:x} does not fit 0x{lo:08x}..0x{hi:08x}')
        if rng['alo'] is not None and (piece > rng['alo'] or piece + sec['size'] < rng['ahi']):
            reasons.append('piece does not cover the anchored span of the manifest')
    gaps, pos = [], 0
    for a, b, _ in sorted(matched):
        if a > pos:
            gaps.append((pos, a))
        pos = max(pos, b)
    if pos < sec['size']:
        gaps.append((pos, sec['size']))
    data = obj.bytes_of(sec)
    bad = []
    for a, b in gaps:
        zero = not data or not any(data[a:b])
        if (b - a) < 16 and zero:
            continue
        if section in RODATA_ANON:
            continue
        if section in CARVED_ANON and (rng.get('anon_bytes') or {}).get('equal'):
            continue
        bad.append(f'+0x{a:x}..+0x{b:x}' + ('' if zero else ' (nonzero)'))
    if bad:
        reasons.append(f'uncovered bytes that are not alignment padding: {bad[:6]}')
    detail = dict(reasons=reasons, range_source=rng['source'], original_symbols=len(originals),
                  matched=len(matched), object_size=sec['size'],
                  piece_start=f'0x{piece:08x}' if piece is not None else None)
    if rng.get('anon_bytes') is not None:
        detail['anon_bytes'] = rng['anon_bytes']
    return not reasons, detail, accounted


# ---------------------------------------------------------------------------
# Accepted C records: a claimed body must still be its record's body
# ---------------------------------------------------------------------------
# A record's source can be corrected after a TU was imported. The whole-file
# gate cannot see it when both spellings compile to the same bytes (main/tu092
# `xglCdArcCheck`), so every claimed C function whose accepted record resolves
# to a body is compared with that body verbatim. The only allowed difference is
# a per-TU adaptation named in the TU's adaptations file, and when the entry
# carries the exact old/new text the difference must be exactly that.
_C_RECORD_INDEX = None
_ADAPTATION_FILES = ('adaptations.json', 'merge-adaptations.json', 'tu-adaptations.json')
# Unpublished workflow evidence: config/adaptations/<unit>.json holds the declared
# differences between a TU body and its accepted record (the build never reads them).
_ADAPTATION_UNIT_FILE = 'config/adaptations/%s.json'
_ADAPTATION_PROSE = ('reason', 'detail', 'before', 'after', 'resolution', 'note', 'area')


def c_record_index():
    """{(unit, va): [(record path, record, function entry)]} over accepted C records."""
    global _C_RECORD_INDEX
    if _C_RECORD_INDEX is None:
        index = {}
        for path in sorted((ROOT / 'config/units').glob('*.json')):
            try:
                record = json.loads(path.read_bytes())
            except ValueError:
                continue
            if record.get('source_language') == 'ee-asm':
                continue
            source = (record.get('source') or {}).get('path', '')
            if not source.endswith('.c'):
                continue
            for f in record.get('functions', []):
                index.setdefault((record.get('unit'), str(f.get('va', '')).lower()), []).append(
                    (rel(path), record, f))
        _C_RECORD_INDEX = index
    return _C_RECORD_INDEX


def record_body(record, name, source_rel=None, expected_sha=None):
    """(text, error): the function's current definition text in the record's source."""
    source = ROOT / (source_rel or relative_source(record))
    want = expected_sha or record['source']['sha256']
    if not source.is_file():
        return None, f'record source missing: {rel(source)}'
    digest = sha256_file(source)
    if digest != want:
        return None, (f'record source {rel(source)} no longer matches the sha256 of its record '
                      f'({digest[:12]}... vs {want[:12]}...)')
    try:
        tu = tu_edit.TU(source.read_text(encoding='utf-8', errors='surrogateescape'))
    except tu_edit.ParseError as e:
        return None, f'record source {rel(source)} cannot be split: {e}'
    block = tu.by_name.get(name)
    if block is None or block.state != 'c':
        return None, f'record source {rel(source)} has no C definition of {name}'
    return tu.text[block.core_start:block.core_end], None


def safe_relative(path, what='record source'):
    if '..' in Path(path).parts or Path(path).is_absolute():
        raise AuditError(f'unsafe {what} path {path!r}')
    return path


def relative_source(record, tu_id=None):
    """The file this record's body is read from, for the TU being audited.

    A record that spans two original translation units (the accepted unit was one
    allocation, the published TUs are two objects) carries a `tu_sources` entry per
    TU; without `tu_id`, or without a matching entry, the record's own `source` is
    used.
    """
    source = record.get('source') or {}
    for entry in source.get('tu_sources') or []:
        if tu_id is not None and entry.get('tu') == tu_id and entry.get('path'):
            return safe_relative(entry['path'])
    return safe_relative(source['path'])


def relative_source_sha(record, tu_id=None):
    source = record.get('source') or {}
    for entry in source.get('tu_sources') or []:
        if tu_id is not None and entry.get('tu') == tu_id and entry.get('path'):
            return entry.get('sha256') or source['sha256']
    return source['sha256']


def retired_origin(record):
    """The archived pre-migration source of a retired record, when it is still here.

    Retiring the pre-migration layout (task tu-retire-legacy-src, 2026-09-12) moved
    every duplicated legacy source to `.work/archive/pre-tu-source-20260912/` and
    repointed its record at the published TU file. The archive is local and
    gitignored, so it is used when present and its absence is reported, never
    silently treated as agreement.
    """
    retired = ((record.get('source') or {}).get('retired_from') or {})
    path, digest = retired.get('archived_to'), retired.get('sha256')
    if not path or not digest:
        return None
    try:
        path = safe_relative(path, 'archived record source')
    except AuditError:
        return None
    return dict(path=path, sha256=digest, present=(ROOT / path).is_file())


def load_adaptations(paths):
    """Normalise the per-TU adaptation declarations of the importers/integrator."""
    entries = []
    for path in paths:
        p = Path(path)
        if not p.is_file():
            continue
        try:
            data = json.loads(p.read_text())
        except ValueError:
            continue
        raw = data.get('adaptations') if isinstance(data, dict) else data
        if isinstance(raw, list):
            for k, e in enumerate(raw):
                if not isinstance(e, dict):
                    continue
                symbols = {e[f] for f in ('symbol', 'function') if isinstance(e.get(f), str)}
                for f in ('symbols', 'functions'):
                    if isinstance(e.get(f), list):
                        symbols |= {x for x in e[f] if isinstance(x, str)}
                prose = ' '.join(str(e.get(f, '')) for f in _ADAPTATION_PROSE)
                entries.append(dict(source=rel(p), index=k, tu=str(e.get('tu') or e.get('id') or ''),
                                    symbols=symbols, prose=prose, old=e.get('old'), new=e.get('new'),
                                    kind=e.get('kind'), reason=(e.get('reason') or e.get('detail') or '')[:200]))
        elif isinstance(data, dict):
            # {unit: {tu: {replace: [{function, old, new, reason}], ...}}}
            for unit, tus in data.items():
                if not isinstance(tus, dict):
                    continue
                for tu_name, spec in tus.items():
                    if not isinstance(spec, dict):
                        continue
                    reason = str(spec.get('reason', ''))[:200]
                    for k, r in enumerate(spec.get('replace', []) or []):
                        entries.append(dict(source=rel(p), index=k, tu=f'{unit}/{tu_name}',
                                            symbols={r['function']} if r.get('function') else set(),
                                            prose=str(r.get('reason', '')) + ' ' + reason,
                                            old=r.get('old'), new=r.get('new'), kind='replace',
                                            reason=str(r.get('reason', reason))[:200]))
                    if not spec.get('replace'):
                        entries.append(dict(source=rel(p), index=-1, tu=f'{unit}/{tu_name}', symbols=set(),
                                            prose=reason, old=None, new=None, kind='tu', reason=reason))
    return entries


def discover_adaptations(paths, tu_c, unit_dir):
    """Explicit files, the ones beside the TU or its unit, and their declared inputs.

    A merged tree keeps its own decisions and names the deliverables it merged in
    `inputs`; each of those keeps the record of what that importer adapted, so the
    declared inputs are followed (bounded, without cycles).
    """
    found, seen = [], set()

    def add(path):
        resolved = str(Path(path).resolve())
        if resolved not in seen and Path(resolved).is_file():
            seen.add(resolved)
            found.append(resolved)
            return True
        return False

    for path in paths or []:
        add(path)
    # The declarations for a unit live outside the built tree (workflow evidence).
    unit_name = Path(tu_c).resolve().parent.name
    for base in (Path.cwd(), Path(unit_dir).resolve().parent):
        add(Path(base) / (_ADAPTATION_UNIT_FILE % unit_name))
    # A candidate carries its own exact accepted-body replacements beside its
    # TU. They follow inherited unit declarations and are transported/pinned
    # with the same candidate through review and serial publication.
    add(Path(tu_c).with_suffix('.adaptations.json'))
    for start in (Path(tu_c).parent, Path(unit_dir)):
        here = Path(start).resolve()
        for _ in range(4):
            for name in _ADAPTATION_FILES:
                add(here / name)
            if here.parent == here:
                break
            here = here.parent
    index = 0
    while index < len(found) and index < 64:
        current = Path(found[index])
        index += 1
        try:
            data = json.loads(current.read_text())
        except (ValueError, OSError):
            continue
        if not isinstance(data, dict):
            continue
        for entry in data.get('inputs') or []:
            if not isinstance(entry, str) or entry.startswith('/') or '..' in Path(entry).parts:
                continue
            base = ROOT / entry
            for name in _ADAPTATION_FILES:
                add(base / name if base.is_dir() else base.parent / name)
    return found


def _tu_matches(entry_tu, tu_id, tu_rel):
    if not entry_tu or entry_tu.lower() in ('none', 'range', 'all', '-'):
        return None  # a declaration that names no TU
    keys = {k.lower() for k in (tu_id or '', tu_rel, Path(tu_rel).stem) if k}
    value = entry_tu.lower()
    return any(k and (k in value or value in k or Path(k).stem == Path(value).stem) for k in keys)


def adaptations_for(entries, tu_id, tu_rel, function):
    """Declarations that name this function for this TU.

    An entry that carries exact `old`/`new` text is applied to a record body, so
    it must name the function it rewrites: a prose mention of a *different*
    function (a reason that explains why the TU's other users of the same object
    need the canonical declaration) would otherwise apply that entry's
    replacement to them as well and fail as `adaptation_text_not_found`. Only a
    prose-only declaration, which never rewrites anything, keeps the mention
    fallback.
    """
    hits = []
    for e in entries:
        match = _tu_matches(e['tu'], tu_id, tu_rel)
        named = function in e['symbols']
        has_text = bool(e.get('old') and e.get('new'))
        mentioned = (not has_text
                     and re.search(r'\b' + re.escape(function) + r'\b', e['prose']) is not None)
        if (named and match is not False) or (mentioned and match is True):
            hits.append(e)
    return hits


def record_digest(record_path):
    """The sha256 of the accepted record file the audit actually read.

    The audited tree supplies the records (see resolve_root), and a worker
    sandbox may legitimately stage a narrowed one for a re-cut accepted group.
    Reporting the digest of the file that was read is what keeps that from
    becoming credit nobody can check: the reviewer re-runs the audit against
    the published tree and sees at a glance whether the record it judged is the
    published record or a candidate for one.
    """
    try:
        return sha256_file(ROOT / record_path)
    except OSError:
        return None


def record_claims_tu(record, tu_id):
    """Does this accepted record say the TU under audit is what carries its body?

    Only such a record makes a statement about this TU's C set. A record whose
    source is still a pre-migration file of the legacy per-function pipeline
    describes an acceptance the TU has NOT imported yet -- main/tu107 delivers
    `nmlPacketAddTransMicrocode` as INCLUDE_ASM while
    config/units/math-main-nml-packet-trans-microcode.json holds its accepted C,
    and the per-TU ledger records exactly that (state `asm`, no records). Treating
    it as a function this TU lost would turn a recorded, legitimate backlog into a
    failure, so those are reported as `not_carried` instead.
    """
    source = record.get('source') or {}
    if tu_id and source.get('tu') == tu_id:
        return True
    return any(entry.get('tu') == tu_id for entry in source.get('tu_sources') or [])


def tu_accepted_c_functions(unit, tu_id, start, end):
    """([carried], [not carried]) accepted C records of the TU spanning [start, end).

    Keyed by the original address, so a record whose functions were split across
    two published TUs contributes only the ones this TU carries, and a name that
    occurs in more than one unit cannot be borrowed from the wrong one. Each entry
    is (record path, record, function entry, address).
    """
    carried, other = [], []
    if start is None:
        return carried, other
    for (record_unit, va), hits in c_record_index().items():
        if record_unit != unit or not va:
            continue
        try:
            address = int(va, 16)
        except ValueError:
            continue
        if not start <= address < end:
            continue
        for record_path, record, entry in hits:
            row = (record_path, record, entry, address)
            (carried if record_claims_tu(record, tu_id) else other).append(row)
    key = lambda row: (row[3], row[0])
    return sorted(carried, key=key), sorted(other, key=key)


# ---------------------------------------------------------------------------
# Comment-neutral record fidelity (user direction 2026-09-19)
# ---------------------------------------------------------------------------
# A change that only removes, rewords or adds C comments never blocks anything
# and never counts as an error. The published source is edited by hand (on
# 2026-09-16 workflow citations were removed from comments of
# src/main/game_camera.c, which moved `MoveHokan`'s body off its record's
# per-function body pin). A hash cannot tell a comment from code, so when a
# body no longer hashes to its pin the pinned text is recovered -- from the
# repository history of the record's source, the pre-migration archive or a
# C file the record itself names -- verified against the pin, and compared
# with the TU body as C token streams with comments treated as whitespace
# (tools/tu_edit.py `same_code`). Equal streams pass as
# `comment_only_drift_*`; a code difference, or a pin nothing in reach
# reproduces, is judged exactly as before.
_PRE_TU_ARCHIVE = '.work/archive/pre-tu-source-20260912'
_GIT_REVISION_LIMIT = 64
_GIT_TIMEOUT = 30
_WITNESS_CACHE = {}


def _git(repo, argv, stdin=None):
    """stdout bytes of a bounded, read-only git command, or None."""
    try:
        proc = subprocess.run(['git', '-C', str(repo), *argv], input=stdin, stdout=subprocess.PIPE,
                              stderr=subprocess.DEVNULL, timeout=_GIT_TIMEOUT, start_new_session=True)
    except (OSError, subprocess.SubprocessError):
        return None
    return proc.stdout if proc.returncode == 0 else None


def _git_versions(source_rel):
    """[(label, bytes)]: the index copy and up to _GIT_REVISION_LIMIT committed
    versions of `source_rel`, newest first, from the repository of the audited
    tree and the tool's own (a worker sandbox has no history of its own)."""
    found, paths = [], set()
    for tree in (ROOT, TOOL_ROOT):
        top = _git(tree, ['rev-parse', '--show-toplevel'])
        if not top:
            continue
        top = Path(top.decode('utf-8', 'surrogateescape').strip())
        try:
            git_path = Path(os.path.abspath(Path(tree) / source_rel)).relative_to(top).as_posix()
        except ValueError:
            continue
        # A private root may be nested in TOOL_ROOT's checkout without having
        # its own .git. Git then reports the same top for both roots, but their
        # source paths differ: the private path has no pinned history while the
        # canonical path does. Deduplicate paths, not repository roots.
        if (top, git_path) in paths:
            continue
        paths.add((top, git_path))
        revs = _git(top, ['log', '--format=%H', f'-n{_GIT_REVISION_LIMIT}', '--all', '--', git_path]) or b''
        specs = [f':{git_path}'] + [f'{r}:{git_path}' for r in revs.decode().split()]
        out = _git(top, ['cat-file', '--batch'], ''.join(s + '\n' for s in specs).encode())
        if out is None:
            continue
        pos, seen = 0, set()
        for spec in specs:
            eol = out.find(b'\n', pos)
            if eol < 0:
                break
            header = out[pos:eol].split()
            pos = eol + 1
            if len(header) != 3 or header[1] != b'blob':
                continue                      # `<spec> missing`, or not a file
            size = int(header[2])
            data, pos = out[pos:pos + size], pos + size + 1
            if header[0] in seen:
                continue
            seen.add(header[0])
            where = 'index' if spec.startswith(':') else spec.split(':', 1)[0][:12]
            found.append((f'git {where}:{git_path}', data))
    return found


def _archive_versions(record_path, record, source_rel):
    """[(label, bytes)] of the pre-migration archive files this record reaches."""
    paths = [f'{_PRE_TU_ARCHIVE}/{source_rel}']
    retired = ((record.get('source') or {}).get('retired_from') or {})
    if str(retired.get('archived_to', '')).endswith('.c'):
        paths.append(retired['archived_to'])
    found, seen = [], set()
    for base in (ROOT, TOOL_ROOT):
        try:
            manifest = json.loads((Path(base) / _PRE_TU_ARCHIVE / 'manifest.json').read_bytes())
        except (OSError, ValueError):
            manifest = {}
        for f in manifest.get('files') or []:
            if isinstance(f, dict) and f.get('pinned_by') == record_path and str(f.get('path', '')).endswith('.c'):
                paths.append(f'{_PRE_TU_ARCHIVE}/{f["path"]}')
        for path in paths:
            try:
                path = safe_relative(path, 'archived record source')
            except AuditError:
                continue
            full = Path(base) / path
            if full.is_file() and str(full.resolve()) not in seen:
                seen.add(str(full.resolve()))
                found.append((f'archive {path}', full.read_bytes()))
    return found


def _evidence_versions(record, source_rel):
    """[(label, bytes)] of the other C files the record itself names."""
    names, stack = set(), [record]
    while stack:
        node = stack.pop()
        if isinstance(node, dict):
            stack.extend(node.values())
        elif isinstance(node, list):
            stack.extend(node)
        elif isinstance(node, str) and node.endswith('.c') and '\n' not in node and node != source_rel:
            names.add(node)
    found = []
    for name in sorted(names):
        try:
            path = safe_relative(name, 'record evidence')
        except AuditError:
            continue
        if (ROOT / path).is_file():
            found.append((f'evidence {path}', (ROOT / path).read_bytes()))
    return found


def recover_pinned_body(record_path, record, name, source_rel, body_pin):
    """(text, witness): the body `body_pin` pins, recovered and verified, or (None, witness).

    Candidates are tried in order -- the repository versions of the record's
    source (a version whose sha256 is the record's own `source.sha256` first),
    then the pre-migration archive, then C files the record names -- and each
    is split with tu_edit.TU and sliced `core_start:core_end` exactly as
    record_body() does. Only a slice that hashes to the pin is returned.
    """
    key = (str(ROOT), record_path, name, source_rel, body_pin)
    if key in _WITNESS_CACHE:
        return _WITNESS_CACHE[key]
    source = record.get('source') or {}
    pins = {source.get('sha256')} | {e.get('sha256') for e in source.get('tu_sources') or []
                                     if isinstance(e, dict)}
    versions = _git_versions(source_rel)
    versions.sort(key=lambda v: hashlib.sha256(v[1]).hexdigest() not in pins)   # stable: pinned first
    versions += _archive_versions(record_path, record, source_rel)
    versions += _evidence_versions(record, source_rel)
    tried = []
    answer = None, dict(recovered=False, tried=tried)
    for label, data in versions:
        tried.append(label)
        try:
            tu = tu_edit.TU(data.decode('utf-8', 'surrogateescape'))
        except (tu_edit.ParseError, ValueError):
            continue
        block = tu.by_name.get(name)
        if block is None or block.state != 'c':
            continue
        text = tu.text[block.core_start:block.core_end]
        if sha256_text(text) == body_pin:
            answer = text, dict(recovered=True, witness=label, file_sha256=hashlib.sha256(data).hexdigest(),
                                file_matches_record_pin=hashlib.sha256(data).hexdigest() in pins)
            break
    _WITNESS_CACHE[key] = answer
    return answer


def recover_pinned_file(record_path, record, source_rel, want):
    """(text, label) of a version of the record's source whose sha256 is `want`, or (None, None)."""
    for label, data in _git_versions(source_rel) + _archive_versions(record_path, record, source_rel):
        if hashlib.sha256(data).hexdigest() == want:
            return data.decode('utf-8', 'surrogateescape'), label
    return None, None


def verified_record_storage_views(expected, body, tu, record, context):
    """Prove a byte-address view of a recovered, record-pinned data object."""
    if not context or not context.get('data_row'):
        return []
    args, row = context['args'], context['data_row']
    tokens = tu_edit.c_tokens(tu.text)
    directives, directive = [], None
    for token in tokens:
        if token == tu_edit.DIRECTIVE_END:
            if directive is not None:
                directives.append(directive)
            directive = None
        elif directive is not None:
            directive.append(token)
        elif token == '#':
            directive = []
    macros, depth = {}, 0
    for directive in directives:
        if not directive:
            continue
        if directive[0] in ('if', 'ifdef', 'ifndef'):
            depth += 1
        elif directive[0] == 'endif':
            depth -= 1
        elif directive[0] in ('define', 'undef') and len(directive) >= 2:
            name = directive[1]
            macros.setdefault(name, []).append((depth, directive))
    expected_tokens, body_tokens = tu_edit.c_tokens(expected), tu_edit.c_tokens(body)
    views = []
    for macro, definitions in macros.items():
        if len(definitions) != 1 or definitions[0][0] != 0 or macro not in body_tokens:
            continue
        definition = definitions[0][1]
        # No function-like macro, offset, arithmetic, or evaluation is admitted.
        for owner, reference in (record.get('references') or {}).items():
            if owner not in expected_tokens or definition != ['define', macro] + tu_edit.c_tokens(
                    '((unsigned char *)(void *)' + owner + ')'):
                continue
            if owner in macros or not isinstance(reference, dict) or reference.get('unit') != args.unit:
                continue
            # The cast is an address view only for an authored array.  A
            # pointer or scalar cast would introduce a different evaluation.
            declarations = []
            for item in tu.items:
                if item.kind != 'decl':
                    continue
                declaration = tu_edit.c_tokens(tu.text[item.start:item.end])
                if owner not in declaration:
                    continue
                index = declaration.index(owner)
                tail = declaration[index + 1:]
                if (declaration.count(owner) == 1 and 'extern' not in declaration[:index] and
                        '*' not in declaration[:index] and len(tail) >= 5 and
                        tail[0] == '[' and re.fullmatch(r'[1-9][0-9]*', tail[1]) and
                        tail[2:4] == [']', '=']):
                    declarations.append(declaration)
            if len(declarations) != 1:
                continue
            views.append((macro, owner, reference))
    if not views or len(views) > 16 or len({owner for _, owner, _ in views}) != len(views):
        return []
    substitutions = {owner: macro for macro, owner, _ in views}
    if [substitutions.get(token, token) for token in expected_tokens] != body_tokens:
        return []
    from tools.tu import data_carve as dc
    raw_path, source, assembly = Path(args.obj), Path(args.tu_c), Path(args.build_s)
    selected_path = Path(row.get('_audit_linked_data_object', '/nonexistent'))
    linked_path = Path(args.unit_dir) / 'build' / (args.unit + '.rom.elf')
    candidate_pin = row.get('_audit_candidate_record') or {}
    candidate_path = Path(candidate_pin.get('path', '/nonexistent'))
    current_hashes = dict(source_sha256=sha256_file(source), assembly_sha256=sha256_file(assembly),
                          object_sha256=sha256_file(raw_path),
                          linked_data_object_sha256=sha256_file(selected_path),
                          linked_elf_sha256=sha256_file(linked_path))
    if (any(row.get(key) != value for key, value in current_hashes.items()) or
            not candidate_path.is_file() or sha256_file(candidate_path) != candidate_pin.get('sha256') or
            source.read_text() != tu.text):
        return []
    candidate = json.loads(candidate_path.read_text())
    raw, selected = RelObject(raw_path.read_bytes()), RelObject(selected_path.read_bytes())
    ctx = dc.tu_context(ROOT, args.unit, Path(args.unit_dir), args.tu_id)
    original, linked = dc.Elf(Path(ctx['orig']).read_bytes()), dc.Elf(linked_path.read_bytes())
    authored_asm = asm_statement_lines(tu.text)
    asm_text = assembly.read_text()
    outside_app, app = [], False
    for line in asm_text.splitlines():
        if line.strip() == '#APP':
            app = True
        elif line.strip() == '#NO_APP':
            app = False
        elif not app:
            outside_app.append(line)
    outside_app = '\n'.join(outside_app)
    proofs = []
    for macro, owner, reference in views:
        address, size = int(reference['va'], 16), reference['size']
        spans = [span for span in row.get('verified_location_spans', [])
                 if span.get('address', '').lower() == reference['va'].lower() and
                 span.get('names') == [owner] and span.get('object_symbols') == [owner] and
                 span.get('length') == size and span.get('range') == [address, address + size] and
                 span.get('object_sha256') == current_hashes['linked_data_object_sha256']]
        raw_symbols = [symbol for symbol in raw.symbols if symbol['name'] == owner]
        selected_symbols = [symbol for symbol in selected.symbols if symbol['name'] == owner]
        if (type(size) is not int or size <= 0 or len(spans) != 1 or len(raw_symbols) != 1 or
                len(selected_symbols) != 1 or reference.get('symbols') != [owner] or
                any(re.search(r'\b' + re.escape(owner) + r'\b', line) for line in authored_asm)):
            return []
        span, raw_symbol, selected_symbol = spans[0], raw_symbols[0], selected_symbols[0]
        section = span['section']
        original_symbols = [symbol for symbol in original.symbols if symbol.name == owner and
                            symbol.value == address and symbol.size == size and symbol.type == STT_OBJECT and
                            symbol.bind == raw_symbol['binding'] and 0 < symbol.shndx < len(original.sections) and
                            original.sections[symbol.shndx].name == section]
        linked_symbols = [symbol for symbol in linked.symbols if symbol.name == owner and
                          symbol.value == address and symbol.size == size and symbol.type == STT_OBJECT and
                          symbol.bind == raw_symbol['binding'] and 0 < symbol.shndx < len(linked.sections) and
                          linked.sections[symbol.shndx].name == section]
        locations = [location for location in candidate.get('recovered_location_candidates', [])
                     if location.get('unit') == args.unit and location.get('section') == section and
                     location.get('address', '').lower() == reference['va'].lower() and location.get('names') == [owner]]
        raw_section, selected_section = raw.section(raw_symbol['section']), selected.section(selected_symbol['section'])
        input_span = span.get('compiler_input_span') or {}
        if (len(original_symbols) != 1 or len(linked_symbols) != 1 or len(locations) != 1 or
                raw_symbol['type'] != STT_OBJECT or raw_symbol['size'] != size or
                selected_symbol['type'] != STT_OBJECT or selected_symbol['size'] != size or
                selected_symbol['binding'] != raw_symbol['binding'] or
                raw_symbol['section'] != input_span.get('section') or raw_symbol['value'] != input_span.get('offset') or
                raw_section['size'] != input_span.get('section_size') or raw_section['type'] != 1 or
                selected_symbol['section'] != span['c_object_section'] or selected_symbol['value'] != span['c_object_offset'] or
                selected_section['type'] != 1 or selected_section['size'] != span['c_object_section_size'] or
                not re.search(r'^\s*' + re.escape(owner) + r':\s*$', outside_app, re.MULTILINE) or
                not re.search(r'^\s*\.size\s+' + re.escape(owner) + r'\s*,\s*' + str(size) + r'\s*$', outside_app, re.MULTILINE)):
            return []
        raw_bytes = raw.bytes_of(raw_section)[raw_symbol['value']:raw_symbol['value'] + size]
        selected_bytes = selected.bytes_of(selected_section)[selected_symbol['value']:selected_symbol['value'] + size]
        original_bytes, linked_bytes = dc.va_bytes(original, address, size), dc.va_bytes(linked, address, size)
        if (len(raw_bytes) != size or raw_bytes != selected_bytes or raw_bytes != original_bytes or
                raw_bytes != linked_bytes or hashlib.sha256(raw_bytes).hexdigest() != reference.get('sha256') or
                any(sec == raw_symbol['section'] and raw_symbol['value'] <= off < raw_symbol['value'] + size
                    for sec, off, _kind, _name in raw.relocations())):
            return []
        proofs.append(dict(macro=macro, owner=owner, address=reference['va'], size=size,
                           record_reference_sha256=reference['sha256'], candidate=candidate_pin,
                           kind='exact_recovered_object_byte_address_view', **current_hashes))
    return proofs


def record_fidelity(block, tu, unit, tu_id, tu_rel, original_va, entries, tu_repo=None,
                    storage_view_context=None):
    """(ok, detail) for one claimed C function against its accepted record.

    Since the pre-migration layout was retired (task tu-retire-legacy-src), a
    record's `source` is usually the published TU file itself. Comparing that file
    with itself proves nothing, so this check says so - `record_is_tu_source` - and
    falls back to the archived pre-migration body when the local archive still has
    it, which is the only place a real comparison is still available.

    What the check establishes is about ONE FUNCTION: that the TU's definition is
    still the body the record was accepted for. The record's whole-file
    `source.sha256` is not that; it is a property of the published baseline, it
    changes whenever any other function of the TU is edited, and in a candidate it
    can never hold. It is reported here (`source_pin`) and never decides.
    `accepted_c_functions` in `audit` is the other half: it runs this comparison
    for the accepted C functions of the TU that were NOT claimed, so an accepted
    neighbour cannot be dropped or reverted to assembly without the audit noticing.
    """
    body = tu.text[block.core_start:block.core_end]
    index = c_record_index()
    hits = list(index.get((unit, str(original_va).lower()), [])) if original_va else []
    if not hits and original_va is None:
        hits = [h for key, v in index.items() if key[0] == unit for h in v if h[2].get('name') == block.key]
    if not hits:
        return True, dict(status='no_accepted_record',
                          detail='no accepted config/units record resolves to this function; nothing to '
                                 'compare (a TU-mode recovery has no legacy record)')
    if len(hits) != 1:
        return False, dict(status='ambiguous_record', records=sorted(h[0] for h in hits),
                           detail='several accepted records claim this address')
    record_path, record, entry = hits[0]
    source_rel = relative_source(record, tu_id)
    self_reference = tu_repo is not None and source_rel == tu_repo
    origin = retired_origin(record) if self_reference else None
    if self_reference:
        # The record points at the file under audit, so `record_body` would read
        # the accepted body out of the very file it is judging. Two different
        # questions have to be kept apart here, and only one of them is this
        # check's:
        #
        #  * "is the TU's definition still the accepted function?" -- what the
        #    check is for. The witness is the archived pre-migration body, which
        #    the record names and pins by its own sha256 (`retired_from`). It is
        #    independent of the TU file, so it answers the question in a published
        #    tree and in a candidate tree alike, and it answers it per function.
        #  * "does the record still describe the file it points at?" -- the
        #    whole-file `source.sha256` pin. That is integrator bookkeeping about a
        #    published tree, and in a candidate it is unsatisfiable BY
        #    CONSTRUCTION: a candidate differs from the published baseline, which
        #    is what makes it a candidate. Deciding this check on that pin made an
        #    untouched neighbour refuse a byte-exact recovery of a different
        #    function of the same TU (ov01/tu028: `adjAngle`'s record pins
        #    src/ov01/m_ef_create_msp_02.c, so every re-treatment of
        #    `fnMSP02_PR000` failed on `adjAngle`), and it returned before the
        #    archived comparison below, which is the comparison that means
        #    something -- leaving that branch unreachable for every candidate.
        #
        # So the pin is measured and reported on every result (`source_pin`), the
        # archived body decides, and the TU-level `accepted_records` report lists
        # every record whose pin no longer matches the file it points at, which is
        # where an integrator reads the bookkeeping signal for a published tree.
        digest = sha256_file(ROOT / source_rel)
        want = relative_source_sha(record, tu_id)
        pin = dict(source=source_rel, source_pin='matches' if digest == want else 'differs',
                   source_sha256=digest, record_source_sha256=want)
        # A per-function body pin, if the record carries one, is a witness of the
        # accepted body that costs no second file and does not move when another
        # function of the TU is edited. A record born in TU mode has no archived
        # pre-migration source (its own note says so), so this is the witness it
        # can be given; the audit consumes it wherever it is present so adding one
        # needs no further tool change.
        body_pin = entry.get('body_sha256') if isinstance(entry, dict) else None
        if body_pin:
            pin['record_body_sha256'] = body_pin
            if sha256_text(body) == body_pin:
                return True, dict(status='verbatim_vs_record_body_pin', record=record_path,
                                  detail='the record points at this TU file; the body was compared with '
                                         'the per-function body pin the record carries and is that body '
                                         'verbatim', **pin)
            # The hash cannot tell a comment edit from a code edit: recover the
            # pinned text and compare code (see recover_pinned_body).
            declared = adaptations_for(entries, tu_id, tu_rel, block.key)
            pinned, witness = recover_pinned_body(record_path, record, block.key, source_rel, body_pin)
            if not declared and pinned is not None and tu_edit.same_code(pinned, body):
                return True, dict(status='comment_only_drift_vs_record_body_pin', record=record_path,
                                  body_sha256=sha256_text(body), pinned_body=witness,
                                  detail='the TU body does not hash to the per-function body pin of its '
                                         'record, but the pinned body (recovered from '
                                         f'{witness["witness"]} and verified against the pin) is the same '
                                         'C token stream once comments are treated as whitespace: the '
                                         'difference is comments only, which never counts as drift', **pin)
            drift = {}
            if pinned is not None:
                drift['diff'] = [line for line in difflib.unified_diff(
                    pinned.splitlines(), body.splitlines(), 'record body pin', 'TU', lineterm='', n=1)][:12]
            if not declared:
                return False, dict(status='undeclared_drift_vs_record_body_pin', record=record_path,
                                   body_sha256=sha256_text(body), pinned_body=witness, **drift,
                                   detail='the TU body does not hash to the per-function body pin of its '
                                          'accepted record and no adaptation in the TU\'s adaptations '
                                          'file names this function ('
                                          + (f'the pinned body, recovered from {witness["witness"]}, differs '
                                             'in code, not only in comments' if pinned is not None else
                                             'no repository version, archive or record evidence in reach '
                                             'reproduces the pin, so a comment-only difference cannot be '
                                             'told apart from a code change') + ')', **pin)
            # A hash cannot be rewritten by an `old`->`new` declaration, so the
            # difference is reported as declared and not as verified, exactly as a
            # prose-only declaration is against a body.
            return True, dict(status='declared_adaptation_vs_record_body_pin', record=record_path,
                              body_sha256=sha256_text(body), pinned_body=witness, **drift,
                              adaptations=[dict(source=x['source'], tu=x['tu'], kind=x['kind'],
                                                reason=x['reason']) for x in declared],
                              detail='the TU body does not hash to the per-function body pin of its '
                                     'record; the difference is covered by a declared per-TU adaptation, '
                                     'which a hash cannot verify', **pin)
        if origin is None or not origin['present']:
            # No independent witness of the accepted body exists in this tree: a
            # record accepted directly in TU mode never had a pre-migration source,
            # and the archive is local and gitignored. Say exactly what was and was
            # not established rather than passing quietly or refusing something no
            # tree can satisfy. What still holds this function down is the TU-level
            # `accepted_records` check (it must still be a C definition of this TU),
            # the original-symbol check (b) and the whole-file rebuild.
            archive = ((f"the pre-migration body this record was accepted from is in the archive "
                        f"({origin['path']}, sha256 {origin['sha256'][:12]}...), which is not in this "
                        f"tree; restore it to compare bodies again")
                       if origin else
                       'this record was accepted against the TU file itself (no pre-migration source '
                       'is recorded), so no tree holds a second copy of the accepted body; giving the '
                       'record entry a `body_sha256` (or archiving the accepted file under '
                       '`source.retired_from.archived_to`) restores the comparison, and this check '
                       'already consumes both')
            if digest == want:
                return True, dict(
                    status='record_is_tu_source', record=record_path, archived=(origin or {}).get('path'),
                    detail='the accepted record points at this TU file, so the body comparison is '
                           'trivially satisfied and establishes nothing beyond the sha256 pin, which was '
                           'checked. ' + archive + '.', **pin)
            return True, dict(
                status='record_is_tu_source_unwitnessed', record=record_path,
                archived=(origin or {}).get('path'),
                detail='the accepted record points at this TU file and the file differs from the '
                       'record\'s whole-file pin, which is what a candidate is; that pin is a property '
                       'of the published baseline, not of this function, so it is reported rather than '
                       'enforced here. No independent copy of the accepted body is available: '
                       + archive + '.', **pin)
        expected, error = record_body(record, block.key, origin['path'], origin['sha256'])
        if expected is None:
            return False, dict(status='unresolved_archived_record', record=record_path,
                               archived=origin['path'], detail=error, **pin)
        if expected == body:
            return True, dict(status='verbatim_vs_archived_record', record=record_path,
                              archived=origin['path'],
                              detail='the record points at this TU file; the body was compared with the '
                                     'archived pre-migration source instead, and is that body verbatim',
                              **pin)
        declared = adaptations_for(entries, tu_id, tu_rel, block.key)
        if not declared and tu_edit.same_code(expected, body):
            return True, dict(status='comment_only_drift_vs_archived_record', record=record_path,
                              archived=origin['path'],
                              detail='the record points at this TU file; the body was compared with the '
                                     'archived pre-migration source instead and differs from it only in '
                                     'comments (the same C token stream), which never counts as drift',
                              **pin)
        diff = [line for line in difflib.unified_diff(expected.splitlines(), body.splitlines(),
                                                      'archived record', 'TU', lineterm='', n=1)][:12]
        if not declared:
            return False, dict(status='undeclared_drift_vs_archived_record', record=record_path,
                               archived=origin['path'], diff=diff,
                               detail='the TU body differs from the archived pre-migration source of its '
                                      'record and no adaptation in the TU\'s adaptations file names this '
                                      'function', **pin)
        texts = [e for e in declared if e.get('old') and e.get('new')]
        rebuilt = expected
        for e in texts:
            if e['old'] not in rebuilt:
                return False, dict(status='adaptation_text_not_found', record=record_path,
                                   archived=origin['path'], diff=diff,
                                   adaptations=[dict(source=x['source'], reason=x['reason']) for x in declared],
                                   detail=f'the declared replacement {e["old"][:60]!r} is not in the '
                                          'archived record body', **pin)
            rebuilt = rebuilt.replace(e['old'], e['new'])
        storage_views = (verified_record_storage_views(rebuilt, body, tu, record, storage_view_context)
                         if not tu_edit.same_code(rebuilt, body) else [])
        if texts and not tu_edit.same_code(rebuilt, body) and not storage_views:
            return False, dict(status='adaptation_does_not_explain', record=record_path,
                               archived=origin['path'], diff=diff,
                               adaptations=[dict(source=x['source'], reason=x['reason']) for x in declared],
                               detail='applying the declared replacements to the archived record body does '
                                      'not produce the TU body', **pin)
        return True, dict(status=('declared_adaptation_and_storage_view_verified_vs_archived_record' if storage_views
                                  else 'declared_adaptation_verified_vs_archived_record' if texts
                                  else 'declared_adaptation_vs_archived_record'),
                          record=record_path, archived=origin['path'],
                          storage_views=storage_views,
                          adaptations=[dict(source=x['source'], reason=x['reason']) for x in declared],
                          detail='the record points at this TU file; the difference against its archived '
                                 'pre-migration source is covered by declared per-TU adaptations' +
                                 (' and independently verified byte-address storage views' if storage_views else ''),
                          **pin)
    want = relative_source_sha(record, tu_id)
    expected, error = record_body(record, block.key, source_rel, want)
    recovered = None
    if expected is None and (ROOT / source_rel).is_file():
        # The record's source no longer hashes to its pin. When the pinned
        # version is still in reach and the file has changed only in comments
        # since, the record still describes it: read the body from that version.
        pinned_text, label = recover_pinned_file(record_path, record, source_rel, want)
        if pinned_text is not None and tu_edit.same_code(
                pinned_text, (ROOT / source_rel).read_text(encoding='utf-8', errors='surrogateescape')):
            try:
                pinned_tu = tu_edit.TU(pinned_text)
                pinned_block = pinned_tu.by_name.get(block.key)
            except tu_edit.ParseError:
                pinned_block = None
            if pinned_block is not None and pinned_block.state == 'c':
                expected = pinned_tu.text[pinned_block.core_start:pinned_block.core_end]
                recovered = dict(witness=label, detail='the record source differs from its pin only in comments; '
                                                      'the body was read from the pinned version')
    if expected is None:
        return False, dict(status='unresolved_record', record=record_path, detail=error)
    extra = dict(record_source_comment_drift=recovered) if recovered else {}
    if expected == body:
        return True, dict(status='verbatim', record=record_path, **extra)
    declared = adaptations_for(entries, tu_id, tu_rel, block.key)
    if not declared and tu_edit.same_code(expected, body):
        return True, dict(status='comment_only_drift', record=record_path, **extra,
                          detail='the TU body differs from its accepted record only in comments (the same C '
                                 'token stream), which never counts as drift')
    diff = [line for line in difflib.unified_diff(expected.splitlines(), body.splitlines(),
                                                  'record', 'TU', lineterm='', n=1)][:12]
    if not declared:
        return False, dict(status='undeclared_drift', record=record_path, diff=diff,
                           detail='the TU body differs from its accepted record and no adaptation in the TU\'s '
                                  'adaptations file names this function')
    texts = [e for e in declared if e.get('old') and e.get('new')]
    if texts:
        rebuilt = expected
        for e in texts:
            if e['old'] not in rebuilt:
                return False, dict(status='adaptation_text_not_found', record=record_path, diff=diff,
                                   adaptations=[dict(source=e['source'], reason=e['reason']) for e in declared],
                                   detail=f'the declared replacement {e["old"][:60]!r} is not in the record body')
            rebuilt = rebuilt.replace(e['old'], e['new'])
        if not tu_edit.same_code(rebuilt, body):
            return False, dict(status='adaptation_does_not_explain', record=record_path, diff=diff,
                               adaptations=[dict(source=e['source'], reason=e['reason']) for e in declared],
                               detail='applying the declared replacements to the record body does not produce '
                                      'the TU body')
        return True, dict(status='declared_adaptation_verified', record=record_path,
                          adaptations=[dict(source=e['source'], reason=e['reason']) for e in texts])
    return True, dict(status='declared_adaptation', record=record_path, diff=diff,
                      adaptations=[dict(source=e['source'], tu=e['tu'], kind=e['kind'], reason=e['reason'])
                                   for e in declared],
                      detail='the difference is covered by a declared per-TU adaptation (prose declaration: the '
                             'entry names this function but carries no exact old/new text)')


def audit(args):
    checks = Checks()
    # Before anything is read: the tree this audit is about (see resolve_root).
    root = resolve_root(args)
    unit_dir = Path(args.unit_dir).resolve()
    tu_c = Path(args.tu_c).resolve()
    tu_rel = Path(os.path.relpath(tu_c, unit_dir)).as_posix()
    result = dict(schema=SCHEMA, tool=dict(path='tools/tu_audit.py', sha256=sha256_file(__file__),
                                           root=str(TOOL_ROOT)),
                  unit=args.unit, tu_id=args.tu_id, root=root, unit_dir=rel(unit_dir), tu_source=tu_rel,
                  ok=False)
    checks.add('root_resolution', root['tu_source_inside_root'], **root)
    if not tu_c.is_file():
        raise AuditError(f'TU source missing: {tu_c}')

    # -- manifest, range, function list ------------------------------------------
    start = end = names = tu_record = None
    if args.tu_id:
        if not args.tu_id.startswith(args.unit + '/'):
            raise AuditError(f'TU id {args.tu_id!r} does not belong to unit {args.unit!r}')
        start, end, names, source, tu_record = lookup_tu(args.tu_id, args.unit)
        result['range_source'] = source
    if args.range:
        a, b = args.range.split(':')
        start, end = int(a, 16), int(b, 16)
        result['range_source'] = 'argument'
    if args.functions:
        names = [n for part in args.functions for n in part.split(',') if n]
    elif args.skeleton:
        names = tu_edit.skeleton_functions(tu_edit.load(args.skeleton))
    if start is not None:
        result['range'] = [f'0x{start:08x}', f'0x{end:08x}']
    checks.add('range_known', start is not None,
               detail=None if start is not None else 'no --tu-id/--range: offsets and the symbol set of the TU '
                                                     'cannot be verified')

    # The TU source is split before anything is compiled: a structurally
    # refused TU is reported as such rather than through a later toolchain error.
    tu_text = tu_c.read_text(encoding='utf-8', errors='surrogateescape')
    tu = tu_edit.TU(tu_text, str(tu_c), names, unit_dir)

    # -- contract ------------------------------------------------------------------
    contract, contract_issues, contract_relaxed = resolve_contract(args, tu_record)
    result['contract'] = json.loads(json.dumps(contract).replace(str(ROOT) + '/', ''))
    # KOS-MOS 2026-09-27: `contract_relaxed` is [] when every pin held (see resolve_contract).
    result['contract_relaxed'] = contract_relaxed
    checks.add('contract', not contract_issues, issues=contract_issues, relaxed=contract_relaxed,
               detail=None if not contract_issues else 'the compile contract is not the TU contract of '
                                                       'config/tu-build.json / config/toolchains-tu.json')
    # The classes the published TU was accepted under (see
    # published_source_classes): the audit reads them from the repository, so a
    # whole-TU audit needs no --source-class/--function-class/--asm-contract to
    # reproduce what a worker declared at submission time.
    pub_tu_class, pub_classes, pub_contracts, pub_prov = published_source_classes(args.tu_id)
    if pub_prov is not None:
        result['source_classes'] = pub_prov
    manifest_class = SOURCE_CLASS.get(((tu_record or {}).get('contract') or {})
                                      .get('source_class'), 'ordinary')
    if args.source_class and pub_tu_class and args.source_class != pub_tu_class:
        raise AuditError(f'--source-class {args.source_class} contradicts {SOURCE_CLASSES_FILE}, '
                         f'which records {pub_tu_class} for {args.tu_id}')
    source_class = args.source_class or pub_tu_class or manifest_class
    result['source_class'] = dict(
        value=source_class,
        basis=('argument' if args.source_class else
               (SOURCE_CLASSES_FILE if pub_tu_class else 'config/tu-build.json')))

    # -- the audit compiles the TU itself -------------------------------------------
    # The assembly step is the build's, read from the unit's build.ninja edge for
    # --obj (build_assembly_recipe). `--asm-cwd`/`--asm-include` predate that and
    # may now only restate it, exactly as `--contract`/`--flags` may only restate
    # the compile contract: a caller naming a different staged directory is a
    # failure, not an override.
    asm_cwd = Path(args.asm_cwd).resolve() if args.asm_cwd else None
    asm_includes = [Path(p).resolve() for p in (args.asm_include or [])]
    recipe = build_assembly_recipe(unit_dir, args.obj)
    if recipe is not None:
        staged = recipe['build_overlay'] or recipe['cwd']
        if asm_cwd is not None and asm_cwd != staged:
            raise AuditError(f'--asm-cwd {rel(asm_cwd)} is not the directory the build assembles '
                             f'{rel(Path(args.obj).resolve())} from ({rel(staged)}, rule '
                             f'{recipe["rule"]} of {rel(unit_dir / "build.ninja")})')
        allowed = [Path(i).resolve() for i in recipe['includes']]
        unknown = [rel(p) for p in asm_includes if p not in allowed]
        if unknown:
            raise AuditError(f'--asm-include {unknown} is not in the include list the build '
                             f'assembles with ({[rel(p) for p in allowed]})')
    tmp = None
    if args.out_dir:
        out_dir = Path(args.out_dir).resolve()
        if out_dir.exists() and any(out_dir.iterdir()):
            raise AuditError('--out-dir must be empty or absent')
        out_dir.mkdir(parents=True, exist_ok=True)
    else:
        tmp = tempfile.TemporaryDirectory(prefix='tu_audit_')
        out_dir = Path(tmp.name)
    try:
        (tu_i, tu_s, tu_o), compile_log, messages, asm_cwd, asm_includes = compile_tu(
            contract, tu_rel, unit_dir, out_dir, asm_cwd, asm_includes, recipe)
        result['compile'] = compile_log
        # What the assembly step was, and where it came from. The audit's own
        # overlay (`cwd`) is built by CINC_TOOL from the audit's own cc1 output;
        # `build_overlay` is the directory the build edge used, recorded so the
        # two are visibly the same recipe.
        result['assembly'] = dict(
            rule=(recipe or {}).get('rule'), derived_from=rel(unit_dir / 'build.ninja'),
            cinc=bool((recipe or {}).get('cinc')),
            cinc_tool=(dict(path=CINC_TOOL, sha256=sha256_file(TOOL_ROOT / CINC_TOOL))
                       if (recipe or {}).get('cinc') else None),
            build_overlay=(rel((recipe or {})['build_overlay'])
                           if (recipe or {}).get('build_overlay') else None),
            cwd=rel(asm_cwd) if asm_cwd else rel(unit_dir),
            includes=[rel(Path(i)) for i in asm_includes])
        if (recipe or {}).get('common_allocation'):
            result['assembly']['common_allocation'] = recipe['common_allocation']
        checks.add('build_assembly_recipe', recipe is not None or not args.obj,
                   **result['assembly'],
                   detail=("the audit assembles this TU the way the build edge that produced "
                           "--obj does" if recipe is not None else
                           'no build.ninja edge produces --obj, so the assembly step could not be '
                           'reproduced from the build; the object comparison stays the binding and '
                           'a recipe difference would fail it'))
        result['toolchain_messages'] = messages
        result['inputs'] = {k: dict(path=rel(p), sha256=sha256_file(p)) for k, p in
                            (('tu_c', tu_c), ('audit_tu_i', tu_i), ('audit_tu_s', tu_s), ('audit_tu_o', tu_o))}
        return _audit_body(args, result, checks, unit_dir, tu_c, tu_rel, contract, source_class, tu_record,
                           start, end, names, tu_i, tu_s, tu_o, asm_cwd, asm_includes, tu, tu_text,
                           pub_classes, pub_contracts,
                           (pub_prov or {}).get('nested_functions') or {},
                           (pub_prov or {}).get('asm_contracts_missing') or {})
    finally:
        if tmp is not None:
            tmp.cleanup()


def _audit_body(args, result, checks, unit_dir, tu_c, tu_rel, contract, source_class, tu_record,
                start, end, names, tu_i, tu_s, tu_o, asm_cwd, asm_includes, tu, tu_text,
                pub_classes=None, pub_contracts=None, pub_nested=None, pub_missing=None):
    pub_missing = pub_missing or {}
    pub_classes = pub_classes or {}
    pub_contracts = pub_contracts or {}
    pub_nested = pub_nested or {}
    # -- binding to the build outputs ------------------------------------------------
    if args.obj:
        build_obj = Path(args.obj).resolve()
        diffs = compare_objects(tu_o, build_obj) if build_obj.is_file() else ['build object missing']
        checks.add('build_object_matches_audit', not diffs, build_object=rel(build_obj), differences=diffs[:8],
                   sha256=sha256_file(build_obj) if build_obj.is_file() else None,
                   detail='allocated sections, relocations and symbols of the build object equal the audit\'s own '
                          'compile (debug/path-only sections excluded)' if not diffs else
                          'the build object is not what this source compiles to')
    else:
        checks.add('build_object_matches_audit', False, waived=True,
                   detail='no --obj: the audited source is not bound to the object the whole-file gate linked')
    if args.build_s:
        build_s = Path(args.build_s).resolve()
        diffs = (compare_assembly(tu_s.read_text(errors='replace'), build_s.read_text(errors='replace'))
                 if build_s.is_file() else ['build assembly missing'])
        checks.add('build_assembly_matches_audit', not diffs, build_assembly=rel(build_s), differences=diffs[:4])

    # Every toolchain message is an error (config/toolchains-tu.json
    # include_asm.rules). The single exemption is cc1's note that a function
    # this TU declares static is not defined in C: its definition is the
    # INCLUDE_ASM/ACCEPTED_ASM body of the same TU. Every such line is listed.
    scaffold_block_names = {b.name for b in tu.blocks if b.state != 'c'} | {b.key for b in tu.blocks
                                                                            if b.state != 'c'}
    kept, exempt = [], []
    for message in result.get('toolchain_messages') or []:
        for line in message['output'].splitlines():
            if not line.strip():
                continue
            m = re.search(r"warning: `([A-Za-z_][\w.$]*)' (?:used but never defined|declared .?static.? but "
                          r"never defined)", line)
            if m and m.group(1) in scaffold_block_names:
                exempt.append(line.strip())
            elif re.fullmatch(r'\S+: (?:In function .*|At top level:)', line.strip()):
                continue
            else:
                kept.append(dict(step=message['step'], line=line.strip()))
    checks.add('toolchain_messages', not kept, messages=kept[:20], exempt=exempt[:20],
               detail=None if not kept else 'config/toolchains-tu.json include_asm.rules: any assembler or '
                                            'compiler message from a TU is an error (the only exemption is a '
                                            'static function whose definition is this TU\'s scaffold body)')
    elf, orig_info = original_elf(args.unit)
    result['inputs']['original'] = orig_info
    obj = RelObject(tu_o.read_bytes())
    asm = AsmFile(tu_s.read_text(encoding='utf-8', errors='surrogateescape'))
    pre = Preprocessed(tu_i.read_text(encoding='utf-8', errors='replace'))
    # Per-function classes: the repository's declaration first, the command line
    # for functions it does not classify. Where both name a function they must
    # agree - the command line may restate a published class, never change it.
    cli_classes = function_classes(args)
    disagree = [f'{n}: {SOURCE_CLASSES_FILE} records {pub_classes[n]}, --function-class says {c}'
                for n, c in sorted(cli_classes.items()) if n in pub_classes and pub_classes[n] != c]
    if disagree:
        raise AuditError('; '.join(disagree))
    classes = dict(pub_classes)
    classes.update(cli_classes)
    gnu_scope_errors = []
    if source_class == 'gnu-nested-function':
        gnu_scope_errors.append(f'{args.tu_id}: the GNU nested-function class must be attached to '
                                 'an enclosing function, not the whole TU')
    for name, cls in sorted(classes.items()):
        if cls != 'gnu-nested-function':
            continue
        if name not in pub_nested:
            gnu_scope_errors.append(f'{args.tu_id} {name}: the evidenced nested_functions mapping is missing')
            continue
        parents = [b for b in tu.blocks if name in (b.name, b.key) and b.state == 'c']
        if len(parents) == 1:
            actual = sorted((d['name'], d['parent']) for d in tu_edit.gnu_nested_definitions(tu.block_text(parents[0]))
                            if d['depth'] > 0)
            expected = sorted((item['source_name'], item['parent']) for item in pub_nested[name])
            if actual != expected:
                gnu_scope_errors.append(f'{args.tu_id} {name}: lexical nested definitions differ from the registered mapping')
    checks.add('gnu_nested_scope', not gnu_scope_errors, errors=gnu_scope_errors,
               detail=None if not gnu_scope_errors else
                'nested C requires original identities, lexical ownership and complete paired claims')
    # The asm contract is optional review documentation that never blocks (user
    # direction 2026-09-27): whatever is wrong with one is a note in the report.
    contract_notes = list((result.get('source_classes') or {}).get('asm_contract_notes') or [])
    asm_contract = (load_asm_contract(args.asm_contract, contract_notes, '--asm-contract')
                    if args.asm_contract else None)
    if asm_contract is not None:
        result['asm_contract'] = dict(path=rel(args.asm_contract),
                                      sha256=sha256_file(args.asm_contract),
                                      top_level_asm=asm_contract.get('top_level_asm') or [])
    # One parsed contract per function that has its own, plus the TU-wide one
    # (the declaration's, else --asm-contract) for every other function.
    contracts = {n: load_asm_contract(ROOT / rp, contract_notes, rp)
                 for n, rp in pub_contracts.items() if n is not None}
    contracts = {n: c for n, c in contracts.items() if c is not None}
    default_contract = asm_contract
    if pub_contracts.get(None) is not None:
        default_contract = (load_asm_contract(ROOT / pub_contracts[None], contract_notes, pub_contracts[None])
                            or asm_contract)
    missing_docs = (result.get('source_classes') or {}).get('asm_contracts_missing') or {}
    checks.add('asm_contract_documentation', True, informational=True,
               notes=contract_notes[:50], missing=missing_docs,
               detail=None if not (contract_notes or missing_docs) else
               'asm contracts are optional review documentation (user direction 2026-09-27): '
               'these notes never fail the audit')
    checks.add('assembly_parse', not asm.errors, errors=asm.errors)

    # -- claims ------------------------------------------------------------------------
    requested = []
    if args.claim:
        requested += [n for part in args.claim for n in part.split(',') if n]
    if args.ledger:
        requested += ledger_claims(args.ledger)
    listed_empty = {e.get('name') if isinstance(e, dict) else e
                    for e in ((tu_record or {}).get('scaffold') or {}).get('empty_functions') or []}
    for b in tu.blocks:
        if b.state == 'c' and (b.key in listed_empty or b.name in listed_empty):
            b.empty = True

    def auditable(b):
        return b.role == 'function' and b.state in ('c', 'accepted_asm')

    nested_by_original = {
        item['original_name']: dict(item, owner=parent)
        for parent, entries in pub_nested.items() for item in entries
    }
    nested_by_parent = {parent: entries for parent, entries in pub_nested.items()}
    claimed_blocks, unresolved, requested_nested = [], [], set()
    if requested:
        for n in requested:
            hit = [b for b in tu.blocks if auditable(b) and n in (b.name, b.key)]
            if len(hit) == 1:
                if hit[0] not in claimed_blocks:
                    claimed_blocks.append(hit[0])
            elif n in nested_by_original:
                parent_name = nested_by_original[n]['owner']
                hit = [b for b in tu.blocks if auditable(b) and parent_name in (b.name, b.key)]
                if len(hit) == 1:
                    requested_nested.add(n)
                    if hit[0] not in claimed_blocks:
                        claimed_blocks.append(hit[0])
                else:
                    unresolved.append(n)
            else:
                unresolved.append(n)
    else:
        claimed_blocks = [b for b in tu.blocks if auditable(b) and not (b.state == 'c' and b.empty)]
    pair_errors = []
    for block in claimed_blocks:
        entries = nested_by_parent.get(block.key) or nested_by_parent.get(block.name) or []
        if not entries or classes.get(block.key, source_class) != 'gnu-nested-function':
            continue
        if requested and (block.key not in requested and block.name not in requested):
            pair_errors.append(f'{block.key}: parent and nested function must be claimed together')
        for item in entries:
            if requested and item['original_name'] not in requested_nested and item['original_name'] not in requested:
                pair_errors.append(f'{block.key}: missing paired nested claim {item["original_name"]}')
    checks.add('gnu_nested_claim_pair', not pair_errors, errors=pair_errors,
               detail=None if not pair_errors else
               'the authorized GNU nested function and its parent are one recovery claim')
    checks.add('claims_resolved', not unresolved, requested=requested, unresolved=unresolved,
               audited=[b.key for b in claimed_blocks],
               detail=None if not unresolved else 'a claimed name is not an auditable C/ACCEPTED_ASM function '
                                                  'of this TU (INCLUDE_ASM scaffold, helper or unknown)')
    nested_claims = []
    for block in claimed_blocks:
        if classes.get(block.key, source_class) != 'gnu-nested-function':
            continue
        nested_claims.extend(nested_by_parent.get(block.key) or nested_by_parent.get(block.name) or [])
    result['claims'] = dict(source='--claim/--ledger' if requested else
                            'default: non-empty C and ACCEPTED_ASM blocks', requested=requested,
                            functions=[b.key for b in claimed_blocks] +
                                      [item['original_name'] for item in nested_claims])
    claimed_names = {b.key for b in claimed_blocks} | {item['original_name'] for item in nested_claims}

    # -- scaffold macro include and headers ------------------------------------------
    include_asm_pin = (contract.get('include_asm') or {}).get('labels_inc') or {}
    used_macro_incs = sorted({m.group(1) for blk in asm.top_app for _, line in blk['lines']
                              for m in [re.fullmatch(r'\.include\s+"([^"]+)"', line)]
                              if m and m.group(1).endswith('.inc')})
    for inc in used_macro_incs:
        path = (asm_cwd or unit_dir) / inc
        if not path.is_file():
            path = unit_dir / inc
        ok, reason = (False, 'missing') if not path.is_file() else macro_include_ok(path.read_text(errors='replace'))
        digest = sha256_file(path) if path.is_file() else None
        pinned = include_asm_pin.get('sha256')
        if ok and pinned and digest != pinned:
            ok, reason = False, (f'not the pinned config/toolchains-tu.json include_asm.labels_inc '
                                 f'({pinned[:12]}...): {digest[:12] if digest else None}...')
        elif ok and not pinned:
            ok, reason = False, 'config/toolchains-tu.json has no include_asm.labels_inc pin'
        checks.add(f'scaffold_macros:{inc}', ok, sha256=digest, pinned=include_asm_pin.get('sha256'),
                   reason=reason)

    include_dirs = contract['include_dirs']
    used_files = [p for p in pre.files() if p != tu_rel]
    header_results, admitted = [], {}

    def resolve_used(name, including_file):
        """The file cpp opened for `#include "name"` in including_file."""
        candidates = [Path(including_file).parent / name] + [Path(d) / name for d in include_dirs]
        for cand in candidates:
            full = cand if cand.is_absolute() else unit_dir / cand
            if full.is_file():
                return cand.as_posix() if not cand.is_absolute() else str(cand)
        return None

    def admit(path_rel, depth=0):
        key = path_rel
        if key in admitted:
            return admitted[key]
        admitted[key] = True
        full = Path(path_rel) if Path(path_rel).is_absolute() else unit_dir / path_rel
        if not full.is_file():
            header_results.append(dict(path=path_rel, ok=False, reason='missing'))
            admitted[key] = False
            return False
        text = full.read_text(errors='replace')
        resolved = {n: resolve_used(n, path_rel) for n in included_names(text)}
        ok, reason, kind = True, None, 'c'
        header_class, header_class_provenance = source_class, {}
        try:
            class_metadata = json.loads((ROOT / SOURCE_CLASSES_FILE).read_text())
            header_class, header_class_provenance = owned_header_source_class(
                full, source_class, ROOT, class_metadata)
            admission(header_class)(aligned_vector4_admission_text(full, text, unit_dir), Path(path_rel),
                                    {n for n, p in resolved.items() if p is not None})
        except ValueError as e:
            scaffold_ok, scaffold_reason = scaffold_header_ok(text)
            if scaffold_ok:
                kind = 'scaffold'
            elif Path(path_rel).name == 'debug_break.h' and debug_break_header_ok(text)[0]:
                kind = 'debug-break'
            elif Path(path_rel).name == 'quadword.h' and quadword_header_ok(text)[0]:
                kind = 'quadword'
            else:
                ok, reason = False, f'{e}' + (f'; not a scaffold header: {scaffold_reason}' if scaffold_reason else '')
        for n, p in resolved.items():
            if p is None:
                ok, reason = False, f'unresolved include {n!r}'
            elif p not in used_files and p != tu_rel:
                ok, reason = False, f'include {n!r} resolves to {p}, which cpp did not open'
            elif depth < 24 and not admit(p, depth + 1):
                ok, reason = False, f'included header {n!r} refused'
        header_results.append(dict(path=path_rel, sha256=sha256_file(full), kind=kind, ok=ok, reason=reason,
                                   source_class=header_class, source_class_provenance=header_class_provenance))
        admitted[key] = ok
        return ok

    tu_includes = {n: resolve_used(n, tu_rel) for n in included_names(tu_text)}
    for n, p in tu_includes.items():
        if p is not None:
            admit(p)
    unadmitted = [p for p in used_files if p not in admitted]
    checks.add('headers_admitted', all(p is not None for p in tu_includes.values())
               and all(h['ok'] for h in header_results) and not unadmitted,
               unresolved=[n for n, p in tu_includes.items() if p is None], not_admitted=unadmitted,
               files_used=len(used_files),
               detail=None if not unadmitted else 'cpp opened files that no #include of the audited text '
                                                  'resolves to')
    result['headers'] = header_results
    header_set = {n for n, p in tu_includes.items() if p is not None}

    # -- the audited text is what cpp kept ---------------------------------------------
    binding = []
    try:
        i_tu = tu_edit.TU(pre.text)
        i_defs = {}
        for b in i_tu.blocks:
            if b.state == 'c':
                i_defs.setdefault(b.name, []).append(b)
    except tu_edit.ParseError as e:
        i_defs = None
        binding.append(f'the preprocessed unit could not be split: {e}')
    if i_defs is not None:
        for b in claimed_blocks:
            if b.state != 'c':
                continue
            defs = i_defs.get(b.name, [])
            if len(defs) != 1:
                binding.append(f'{b.key}: {len(defs)} definitions in the preprocessed unit')
                continue
            where, line = pre.attribute(defs[0].core_start)
            lo = tu_edit.line_of(tu_text, b.core_start)
            hi = tu_edit.line_of(tu_text, b.end)
            if where != tu_rel:
                binding.append(f'{b.key}: the compiled definition comes from {where}, not {tu_rel}')
            elif not (lo <= line <= hi):
                binding.append(f'{b.key}: the compiled definition is at {tu_rel}:{line}, outside the audited '
                               f'block ({lo}-{hi})')
            if b.conditional:
                binding.append(f'{b.key}: the audited definition sits inside a preprocessor conditional')
    checks.add('source_is_what_cpp_compiled', not binding, reasons=binding,
               detail=None if not binding else 'the claimed definition is not the one the compiler used')

    # -- tu.s corresponds to the TU blocks ----------------------------------------------
    expected_seq = []
    for entry in sorted([*tu.blocks, *[i for i in tu.items if i.kind == 'rodata']], key=lambda x: x.start):
        if getattr(entry, 'state', None) == 'c':
            expected_seq.append(('c', entry.name))
        else:
            expected_seq.append(('include', f'{entry.asm_dir}/{entry.name}.s'))
    helpers = {b.name for b in tu.blocks if b.role == 'helper'}
    got_seq = [(k, v) for k, v, _n, _s in asm.sequence if not (k == 'include' and _SCAFFOLD_INC.fullmatch(
        f'.include "{v}"'))]
    emitted_helpers = [n for k, n in got_seq if k == 'c' and n in helpers]
    # GCC emits a nested definition as its own region before its lexical
    # parent. It is not a second top-level TU block. Only the durable admitted
    # mappings qualify; audit_nested_c still proves each original identity,
    # compiler region and lexical/static-chain admission independently.
    nested_emissions = nested_alias_names(pub_nested, {r['name']: [] for r in asm.regions})
    got_cmp = [e for e in got_seq if not (e[0] == 'c' and e[1] in helpers | nested_emissions)]
    exp_cmp = [e for e in expected_seq if not (e[0] == 'c' and e[1] in helpers)]
    checks.add('assembly_matches_source', got_cmp == exp_cmp,
               nested_emissions_checked_separately=sorted(nested_emissions),
               expected=[f'{k}:{v}' for k, v in exp_cmp][:200] if got_cmp != exp_cmp else None,
               found=[f'{k}:{v}' for k, v in got_cmp][:200] if got_cmp != exp_cmp else None,
               detail=None if got_cmp == exp_cmp else 'the compiler output does not follow the TU blocks')
    checks.add('no_emitted_helpers', not emitted_helpers, emitted=emitted_helpers,
               detail=None if not emitted_helpers else 'an emitted helper is code the original TU lacks')

    # -- top-level asm --------------------------------------------------------------------
    allowed_contract_lines = set()
    for _c in list(contracts.values()) + [default_contract]:
        allowed_contract_lines.update((_c or {}).get('top_level_asm') or [])
    stray, waived = [], []
    for block in asm.top_app:
        for number, line in block['lines']:
            if _app_line_allowed(line) or _EXTERN_LINE.fullmatch(line):
                continue
            (waived if line in allowed_contract_lines else stray).append(f'line {number}: {line}')
    # A cc1 constant pool that ended a top-level #APP block is compiler output only
    # when no asm statement of the unit writes any of its lines: a hand-written
    # `asm(".rdata\n$LC9: ...")` has the pool's shape but its lines are in tu.i.
    # Lines a scaffold template may write anyway (`.align N`, `.section .sdata`)
    # are allowed top-level asm on their own and are not matched.
    written = asm_statement_lines(pre.text) if asm.pool_lines else set()
    for number, line in asm.pool_lines:
        if not _app_line_allowed(line) and _norm_asm_line(line) in written:
            (waived if line in allowed_contract_lines else stray).append(f'line {number}: {line}')
    checks.add('top_level_asm', not stray, lines=stray[:50], waived_by_contract=waived[:20],
               **({'cc1_pool_lines': len(asm.pool_lines)} if asm.pool_lines else {}),
               detail=None if not stray else 'only the INCLUDE_ASM/ACCEPTED_ASM/INCLUDE_RODATA expansions, the '
                                             'scaffold macro include, cc1 .extern lines and a cc1 constant pool '
                                             'no asm statement writes may be top-level asm')
    top_text_data = [d for d in asm.top_data if d['section'] in TEXT_SECTIONS]
    checks.add('no_data_in_text', not top_text_data,
               entries=[f'line {d["line"]}: {d["directive"]} {d["operands"][:30]}' for d in top_text_data][:10],
               detail=None if not top_text_data else 'cc1 emitted data into .text outside every function')

    # -- included scaffold, accepted asm and rodata payloads ---------------------------------
    included, scaffold_names = {}, set()
    def effective_path(rel_path):
        if asm_cwd is not None and (asm_cwd / rel_path).is_file():
            return asm_cwd / rel_path, True
        for inc in asm_includes:
            if (inc / rel_path).is_file():
                return inc / rel_path, False
        return unit_dir / rel_path, False

    entries = [(b.name, b.asm_dir, b.state, b) for b in tu.blocks if b.state != 'c']
    entries += [(i.name, i.asm_dir, 'rodata', None) for i in tu.items if i.kind == 'rodata']
    for name, folder, kind, block in entries:
        rel_path = f'{folder}/{name}.s'
        path, staged = effective_path(rel_path)
        text = path.read_text(errors='replace') if path.is_file() else None
        base = unit_dir / rel_path
        included[name if kind != 'rodata' else f'rodata:{name}'] = dict(
            kind=kind, path=path, rel=rel_path, staged=staged, text=text, block=block,
            base_text=base.read_text(errors='replace') if base.is_file() else None,
            defines=tu_edit.asm_file_functions(path) if text is not None else None)
        for m in re.finditer(r'^\s*(?:glabel|alabel|dlabel|ehlabel|jlabel|\.globl|\.global)\s+"?([A-Za-z_.$][\w.$]*)',
                             text or '', re.M):
            scaffold_names.add(m.group(1))
        for m in re.finditer(r'^\s*"?([A-Za-z_.$][\w.$]*)"?:', text or '', re.M):
            scaffold_names.add(m.group(1))
        for m in re.finditer(r'^\s*\.(?:l?comm)\s+([A-Za-z_.$][\w.$]*)', text or '', re.M):
            scaffold_names.add(m.group(1))
        # the scaffold `nonmatching NAME` macro defines NAME.NON_MATCHING
        for m in re.finditer(r'^\s*nonmatching\s+"?([A-Za-z_.$][\w.$]*)', text or '', re.M):
            scaffold_names.add(m.group(1) + '.NON_MATCHING')
    checks.add('included_files_present', all(v['text'] is not None for v in included.values()),
               missing=[v['rel'] for v in included.values() if v['text'] is None])

    # staged (cinc) copies may only drop .extern lines the C declares or defines
    staged_issues = []
    c_declared = set(asm.externs) | set(asm.defined_data) | {r['name'] for r in asm.regions}
    for v in included.values():
        if not v['staged'] or v['text'] is None or v['base_text'] is None:
            continue
        base_lines, lines = v['base_text'].splitlines(), v['text'].splitlines()
        dropped = [l for l in base_lines if l not in lines]
        if [l for l in lines if l not in base_lines]:
            staged_issues.append(f'{v["rel"]}: the staged copy adds lines')
        for line in dropped:
            m = re.fullmatch(r'\s*\.extern\s+([A-Za-z_.$][\w.$]*)\s*,\s*\d+\s*', line)
            if not m:
                staged_issues.append(f'{v["rel"]}: the staged copy drops {line.strip()[:40]!r}')
            elif m.group(1) not in c_declared:
                staged_issues.append(f'{v["rel"]}: dropped .extern {m.group(1)} that the C does not declare')
    if asm_cwd is not None:
        checks.add('staged_asm_overlay', not staged_issues, issues=staged_issues[:10],
                   detail='the staged copies differ from the unit files only by .extern lines the C output '
                          'declares or defines' if not staged_issues else None)

    extern_hits = []
    c_defined = {r['name'] for r in asm.regions} | set(asm.defined_data)
    for v in included.values():
        for m in re.finditer(r'^\s*\.extern\s+"?([A-Za-z_.$][\w.$]*)', v['text'] or '', re.M):
            if m.group(1) in c_defined:
                extern_hits.append(dict(symbol=m.group(1), asm=v['rel']))
    checks.add('no_extern_of_c_defined_symbols', not extern_hits, hits=extern_hits[:20],
               detail=None if not extern_hits else 'a symbol cc1 defines is .extern-declared in the TU asm (GAS '
                                                   'makes it external)')
    rodata_bad = []
    for key, v in included.items():
        if v['kind'] != 'rodata' or v['text'] is None:
            continue
        ok, reason = asm_data_only(v['text'])
        if not ok:
            rodata_bad.append(f'{v["rel"]}: {reason}')
    checks.add('include_rodata_payloads', not rodata_bad, issues=rodata_bad[:10],
               detail=None if not rodata_bad else 'an INCLUDE_RODATA payload contains code or switches to .text')

    # -- scaffold .word sites block their function -------------------------------------------
    word_sites = {}
    for entry in ((tu_record or {}).get('scaffold') or {}).get('word_directives') or []:
        if isinstance(entry, dict) and entry.get('function'):
            word_sites.setdefault(entry['function'], []).append(entry.get('va'))
    scaffold_word_functions = sorted(word_sites)

    # -- object symbols ------------------------------------------------------------------------
    obj_funcs = {}
    for s in obj.functions():
        obj_funcs.setdefault(s['name'], []).append(s)
    orig_funcs = [s for s in elf.symbols if s['type'] == STT_FUNC and s['section_index'] != 0]
    in_range = [s for s in orig_funcs if start <= int(s['va'], 16) < end] if start is not None else []
    pool = in_range if start is not None else orig_funcs
    if start is not None:
        nested_aliases = nested_alias_names(pub_nested, obj_funcs)
        nested_originals = set(nested_by_original)
        claimed_syms = set()
        for b in claimed_blocks:
            claimed_syms.add(b.key)
            if b.state == 'accepted_asm':
                claimed_syms |= set((included.get(b.name) or {}).get('defines') or [])
        obj_named = {n: v for n, v in obj_funcs.items() if len(v) == 1 and n in claimed_syms}
        deltas = sorted({int(s['va'], 16) - start - obj_named[s['name']][0]['value']
                         for s in in_range if s['name'] in obj_named})
        checks.add('range_alignment', deltas in ([0], []), deltas=deltas[:8], checked=len(obj_named))
        orig_names = {s['name'] for s in in_range}
        c_region_names = {r['name'] for r in asm.regions}
        extra_c = sorted(n for n in obj_funcs
                         if n not in orig_names and n in c_region_names and n not in nested_aliases)
        checks.add('no_extra_compiled_functions', not extra_c, extra=extra_c,
                   nested_aliases=sorted(nested_aliases))
        result['scaffold'] = dict(
            extra_func_symbols=sorted(n for n in obj_funcs
                                     if n not in orig_names and n not in c_region_names and n not in nested_aliases),
            nested_aliases=sorted(nested_aliases),
            missing_original_functions=sorted(n for n in orig_names
                                              if n not in obj_funcs and n not in nested_originals),
            word_sites=scaffold_word_functions)

    # -- C-owned data and COMMON ------------------------------------------------------------------
    data_row = None
    validated_alignment_data_row = None
    data_checks_start = len(checks.rows)
    data_audit_error = None
    try:
        data_row = data_gate_row(args, tu_c)
        validated_alignment_data_row = data_row
        whole_ownership_row, owner_receipt = whole_ownership_data_row(args, tu_c, data_row)
        owned = c_owned_sections(args, contract, tu_record, whole_ownership_row)
        if owner_receipt:
            result['current_owner_revalidation'] = owner_receipt
        data_obj = obj
        selected_data_path = (getattr(args, 'data_carve_object', None) or
                              getattr(args, 'data_gate_object', None))
        if (data_row or {}).get('_audit_linked_data_object'):
            selected_data_path = data_row['_audit_linked_data_object']
        if selected_data_path:
            selected_data_path = Path(selected_data_path).resolve()
            data_obj = RelObject(selected_data_path.read_bytes())
        if args.data_gate_report:
            checks.add('linked_data_gate', data_row is not None,
                       report=rel(Path(args.data_gate_report).resolve()),
                       verified_locations=len((data_row or {}).get('verified_locations', [])))
            if (data_row or {}).get('_audit_candidate_record'):
                result['linked_data_candidate'] = data_row['_audit_candidate_record']
            if (data_row or {}).get('_audit_selected_object_chain'):
                result['selected_data_object_chain'] = data_row['_audit_selected_object_chain']
        result['data_ownership'] = {k: {kk: (f'0x{vv:08x}' if isinstance(vv, int) else vv) for kk, vv in v.items()}
                                    for k, v in owned.items()}
        owned_names = set()
        for section, rng in owned.items():
            if section in CARVED_ANON and rng.get('source') == 'config/objects/data-carves.json':
                rng['anon_bytes'] = carved_section_bytes(args, section, elf, data_obj)
            has_gate_spans = any(span.get('section') == section
                                 for span in (whole_ownership_row or {}).get('verified_location_spans', []))
            if (selected_data_path and section not in ('.bss', '.sbss') and data_row is not None and
                    (rng.get('source') == 'config/objects/data-carves.json' or has_gate_spans)):
                ok, detail, accounted = linked_span_data_check(section, rng, elf, data_obj, whole_ownership_row, args)
            else:
                ok, detail, accounted = data_section_check(section, rng, elf, obj, scaffold_names)
            owned_names |= accounted
            checks.add(f'c_owned_data:{section}', ok, **detail)
        comm_reasons, comm_names = [], set()
        orig_data_all = [s for s in elf.symbols if s['type'] in (STT_NOTYPE, STT_OBJECT)
                         and 0 < s['section_index'] < 0xff00 and elf.sections[s['section_index']]['flags'] & 2]
        for c in asm.comms:
            comm_names.add(c['name'])
            if c['kind'] != 'comm':
                continue
            sym = [x for x in obj.symbols if x['name'] == c['name'] and x['shndx'] in (SHN_COMMON, SHN_MIPS_SCOMMON)]
            cands = [o for o in orig_data_all if o['name'] == c['name'] and o['binding'] == 1]
            if len(cands) != 1:
                comm_reasons.append(f'{c["name"]}: {len(cands)} original global data symbols')
                continue
            o = cands[0]
            size = sym[0]['size'] if sym else c['size']
            align = sym[0]['value'] if sym else (c['align'] or 1)
            if size != o['size']:
                comm_reasons.append(f'{c["name"]}: COMMON size {size} vs original {o["size"]}')
            if align and int(o['va'], 16) % align:
                comm_reasons.append(f'{c["name"]}: COMMON alignment {align} does not divide {o["va"]}')
        checks.add('c_common_symbols', not comm_reasons, reasons=comm_reasons,
                   checked=sorted(c['name'] for c in asm.comms if c['kind'] == 'comm'))
        overlay_bss_names, overlay_bss_reasons = overlay_bss_owned_names(args, elf, obj)
        if overlay_bss_names or overlay_bss_reasons:
            checks.add('overlay_bss_storage', not overlay_bss_reasons, reasons=overlay_bss_reasons,
                       owned=sorted(overlay_bss_names))

        # -- no symbol beyond the original TU, the scaffold and the claims -------------------------
        if start is not None:
            allowed = ({s['name'] for s in in_range} | scaffold_names | owned_names | comm_names
                       | overlay_bss_names
                       | {r['name'] for r in asm.regions if r['name'] in {s['name'] for s in in_range}})
            allowed |= nested_alias_names(pub_nested, obj_funcs)
            for section, rng in owned.items():
                if rng.get('lo') is not None:
                    allowed |= {o['name'] for o in original_data_symbols(elf, rng['lo'], rng['hi'])}
            extra_syms = sorted({s['name'] for s in obj.symbols
                                 if s['shndx'] != 0 and s['type'] in (STT_NOTYPE, STT_OBJECT, STT_FUNC)
                                 and s['name'] and s['name'] not in MARKER_NAMES
                                 and not s['name'].startswith(('$', '.L')) and s['name'] not in allowed})
            checks.add('no_extra_object_symbols', not extra_syms, extra=extra_syms[:20],
                       detail=None if not extra_syms else 'the object defines names that are neither original, '
                                                          'scaffold-provided nor C-owned data')

    except (AuditError, OSError, ValueError, KeyError) as error:
        # Data scaffolding is permitted during function matching. A selected
        # object that changes emitted code remains a fatal provenance error.
        if (getattr(args, 'require_data_audit', False) or any(
                text in str(error) for text in ('changes compiler-emitted .text',
                                                'changes compiler-emitted function',
                                                'changes compiler-emitted text relocation'))):
            raise
        data_audit_error = f'{type(error).__name__}: {error}'
        data_row = None
    deferred_data_checks = []
    if not getattr(args, 'require_data_audit', False):
        for check in checks.rows[data_checks_start:]:
            if check['ok']:
                continue
            if check['check'] == 'no_extra_object_symbols':
                extra_functions = [s['name'] for s in obj.symbols
                                   if s['type'] == STT_FUNC and
                                   s['name'] in check.get('extra', [])]
                if extra_functions:
                    continue
            deferred_data_checks.append(dict(check))
            check.update(ok=True, informational=True, deferred=True)
    result['data_audit'] = dict(
        required=getattr(args, 'require_data_audit', False),
        pending=bool(data_audit_error or deferred_data_checks),
        error=data_audit_error, deferred_checks=deferred_data_checks,
        grants_data_recovery=False if data_audit_error or deferred_data_checks else True,
        note='Function acceptance permits original data scaffolding with zero data credit; '
             'explicit data recovery requires its own passing ownership and byte verification.')
    result['data_required'] = bool(getattr(args, 'require_data_audit', False))

    # -- eligibility text -------------------------------------------------------------------------
    admission_items, deferred_attributes, verified_placements = matching_data_section_attributes(
        tu, rel(tu_c), obj, elf, required=getattr(args, 'require_data_audit', False),
        data_row=data_row if result['data_audit']['grants_data_recovery'] else None,
        build_object=Path(args.obj) if args.obj else None, audit_args=args,
        verified_data_row=validated_alignment_data_row,
        assembly=tu_s.read_text(encoding='utf-8', errors='surrogateescape'),
        preprocessed=tu_i.read_text(encoding='utf-8', errors='replace'))
    if verified_placements:
        checks.add('verified_original_data_placement', True, evidence=verified_placements)
    hardware_items, hardware_alignment = verified_vu_data_alignment(
        args, tu, rel(tu_c), obj, elf, validated_alignment_data_row,
        tu_s.read_text(encoding='utf-8', errors='surrogateescape'),
        tu_i.read_text(encoding='utf-8', errors='replace'))
    admission_items.update(hardware_items)
    if hardware_alignment:
        checks.add('verified_vu_data_alignment', True, evidence=hardware_alignment)
    packet_items, packet_alignment = verified_original_packet_alignment(
        args, tu, rel(tu_c), obj, elf, validated_alignment_data_row,
        tu_s.read_text(encoding='utf-8', errors='surrogateescape'),
        tu_i.read_text(encoding='utf-8', errors='replace'))
    admission_items.update(packet_items)
    if packet_alignment:
        checks.add('verified_original_packet_alignment', True, evidence=packet_alignment)
    storage_items, storage_alignment = verified_current_storage_placement_alignment(
        args, tu, rel(tu_c), obj, elf, validated_alignment_data_row,
        tu_s.read_text(encoding='utf-8', errors='surrogateescape'),
        tu_i.read_text(encoding='utf-8', errors='replace'))
    admission_items.update(storage_items)
    if storage_alignment:
        checks.add('verified_current_storage_placement_alignment', True, evidence=storage_alignment)
    if deferred_attributes:
        result['data_audit'].update(pending=True, grants_data_recovery=False,
                                    deferred_source_attributes=deferred_attributes)
        checks.add('inherited_data_section_attributes', True, informational=True,
                   deferred=True, attributes=deferred_attributes)
    packed_items, packed_names = packed_enum_admission_items(args.unit, args.tu_id, tu, tu_text)
    if packed_names:
        admission_items.update(packed_items)
        checks.add('packed_enum_typedefs', True, informational=True,
                   identity='%s %s %s' % PACKED_ENUM_IDENTITY, typedefs=packed_names,
                   detail='user decision 2026-10-28: __attribute__((packed)) admitted on these TU-local '
                          'enum typedefs only (GameMovieInit gp byte-store alias set); never a precedent')
    prelude_end = tu.prelude_end()
    prelude = ''.join(admission_items.get(i.start, tu_text[i.start:i.end]) + '\n' for i in tu.items
                      if i.end <= prelude_end and i.kind != 'rodata')
    free = ''.join(admission_items.get(i.start, tu_text[i.start:i.end]) + '\n' for i in tu.free_items()
                   if i.start >= prelude_end and i.kind != 'rodata')
    transported_helpers = set()
    try:
        transported_helpers, transport = verified_jnt_mmi_helpers(
            args.unit, args.tu_id, tu_rel, tu, pre, asm, obj, classes, source_class, pool,
            orig_info['sha256'])
        if transport is not None:
            result['inline_mmi_helper_transport'] = transport
            checks.add('inline_mmi_helper_transport', True, helpers=sorted(transported_helpers))
    except (AuditError, ValueError) as e:
        checks.add('inline_mmi_helper_transport', False, reason=str(e))
    free += ''.join(tu.block_text(b) + '\n' for b in tu.blocks
                    if b.role == 'helper' and b.name not in transported_helpers)
    try:
        admission(source_class)(prelude + free, Path(tu_rel), header_set)
        checks.add('free_text_admitted', True)
    except ValueError as e:
        checks.add('free_text_admitted', False, reason=str(e))

    # -- accepted-record fidelity inputs -------------------------------------------------------------
    adaptation_files = discover_adaptations(args.adaptations, tu_c, unit_dir)
    adaptations = load_adaptations(adaptation_files)
    result['adaptations'] = dict(files=[rel(f) for f in adaptation_files], entries=len(adaptations))

    # -- the accepted C functions this TU carries ----------------------------------------------------
    # `record_fidelity` runs per CLAIMED function, so an accepted neighbour the
    # candidate silently drops was examined by nothing at all: a worker claims its
    # own slice, and reverting an accepted C function to INCLUDE_ASM re-emits the
    # ORIGINAL bytes, so the whole-file rebuild is byte-identical and cannot see it
    # either. Measured on ov01/tu028 (task tu-audit-record-fidelity-candidate): a
    # candidate that turned the accepted `adjAngle` back into INCLUDE_ASM and
    # claimed only `fnMSP02_PR000` audited ok=true with grants_acceptance=true.
    #
    # So every accepted C record whose function lies in this TU's range is resolved
    # here, claimed or not: it must still be a C definition of this TU, and if it
    # was not claimed its body is put through the same comparison the claimed ones
    # get. `stale_pins` is the other, non-fatal half: the records whose whole-file
    # `source.sha256` no longer matches the file they point at. That is integrator
    # bookkeeping over a published tree -- a candidate diverges from it by
    # construction -- so it is reported for whoever publishes, never charged to a
    # function that is untouched (see record_fidelity).
    accepted_here, accepted_elsewhere = tu_accepted_c_functions(args.unit, args.tu_id, start, end)
    accepted_rows, accepted_bad, stale_pins = [], [], []
    for record_path, record, entry, address in accepted_here:
        name, va = entry.get('name'), f'0x{address:08x}'
        row = dict(function=name, va=va, record=record_path, record_sha256=record_digest(record_path))
        block = tu.by_name.get(name)
        if block is None:
            row.update(state='absent', ok=False, status='accepted_function_absent',
                       detail=f'{name} has an accepted C record ({record_path}) but this TU defines no '
                              f'block for it; an accepted function cannot be dropped by a candidate')
        elif block.state != 'c':
            row.update(state=block.state, ok=False, status='accepted_function_not_c',
                       detail=f'{name} has an accepted C record ({record_path}) but this TU delivers it '
                              f'as {block.state}; the scaffolding re-emits the original bytes, so the '
                              'whole-file rebuild cannot see the loss')
        elif name in claimed_names:
            row.update(state='c', ok=True, claimed=True, status='claimed',
                       detail='claimed by this audit: the body comparison is reported with the function')
        else:
            ok, detail = record_fidelity(block, tu, args.unit, args.tu_id, tu_rel, entry.get('va'),
                                         adaptations, rel(tu_c), dict(args=args, data_row=data_row))
            row.update(state='c', ok=ok, claimed=False, **detail)
        accepted_rows.append(row)
        if not row['ok']:
            accepted_bad.append(f'{name}: {row.get("detail")}')
        try:
            source_rel = relative_source(record, args.tu_id)
            on_disk = sha256_file(ROOT / source_rel)
        except (AuditError, KeyError, OSError):
            source_rel, on_disk = None, None
        pinned = relative_source_sha(record, args.tu_id)
        if source_rel is not None and on_disk != pinned:
            stale_pins.append(dict(record=record_path, source=source_rel, on_disk=on_disk, pinned=pinned))
    result['accepted_records'] = dict(
        functions=accepted_rows, stale_pins=stale_pins,
        not_carried=[dict(function=e.get('name'), va=f'0x{address:08x}', record=record_path,
                          state=(tu.by_name[e['name']].state if e.get('name') in tu.by_name else 'absent'),
                          detail='an accepted C record for a function of this TU\'s range whose source is '
                                 'not this TU: an acceptance the TU has not imported, which the per-TU '
                                 'ledger records as well')
                     for record_path, _record, e, address in accepted_elsewhere])
    checks.add('accepted_c_functions', not accepted_bad, reasons=accepted_bad,
               functions=[r['function'] for r in accepted_rows],
               unclaimed_verified=[r['function'] for r in accepted_rows
                                   if r['ok'] and not r.get('claimed') and r['state'] == 'c'],
               detail=None if not accepted_bad else
               'an accepted C function of this TU is no longer the accepted C function')

    # -- asm contract truthfulness ------------------------------------------------------------------
    # `load_asm_contract` above notes whether the eight fields are non-empty
    # and whether the evidence pin still hashes.  Neither says whether the prose is
    # TRUE about the code it is attached to, and that -- not bytes -- is what
    # returned most of the 2026-09-12 ee-vu-cop2 re-treatment submissions: a
    # contract for a different function, the exact_asm-era document for the
    # standalone .s, a document left stale when the allocation was narrowed, a
    # claim false about the delivered code, an omitted alignment precondition.
    # tools/asm_contract_lint.py derives the facts from the very blocks this
    # audit compiled and reports a claim the source contradicts.  It is
    # contradiction-driven: silence is not a finding, and a clean result means
    # "no claim is contradicted", never "the contract is true".  Since the
    # contract became optional review documentation (user direction
    # 2026-09-27) the lint rows are informational: contradictions are shown to
    # the reviewer, never an audit failure.
    lint_groups = {}
    for b in claimed_blocks:
        if b.state != 'c' or classes.get(b.key, source_class) != 'hardware':
            continue
        if b.key in pub_missing:
            continue
        parsed = contracts.get(b.key, default_contract)
        if parsed is None:
            continue
        path = (pub_contracts.get(b.key) or pub_contracts.get(None)
                or (rel(args.asm_contract) if args.asm_contract else '<none>'))
        lint_groups.setdefault(path, (parsed, {}))[1][b.key] = tu.block_text(b)
    for path, (parsed, bodies) in sorted(lint_groups.items()):
        try:
            lint_report = asm_contract_lint.check_bodies(
                parsed, bodies, unit=args.unit, root=ROOT, source_label=rel(tu_c),
                file_scope_text=prelude + free)
        except Exception as e:  # noqa: BLE001 - documentation lint never blocks
            checks.add(f'asm_contract_lint:{path}', True, informational=True, error=str(e),
                       functions=sorted(bodies),
                       detail='the asm contract could not be checked against the assembly it '
                              'describes (informational: contracts never block)')
            continue
        contradictions = [f'{f["check"]}: {f["detail"]}' for f in lint_report.contradictions]
        # Informational (user direction 2026-09-27): a contradicted contract is a
        # review note about the document, not a failure of the exact source.
        checks.add(f'asm_contract_lint:{path}', True, informational=True,
                   contradicted=bool(contradictions), functions=sorted(bodies),
                   contradictions=contradictions[:20],
                   undecided=sorted({f['check'] for f in lint_report.findings
                                     if f['severity'] == asm_contract_lint.UNDECIDABLE}),
                   notes=sorted({f['check'] for f in lint_report.findings
                                 if f['severity'] == asm_contract_lint.NOTE}),
                   not_checked=asm_contract_lint.NOT_CHECKED,
                   detail=None if not contradictions else
                   'the asm contract is contradicted by the inline assembly it describes: '
                   + '; '.join(contradictions[:4]))

    # -- per function --------------------------------------------------------------------------------
    regions = {}
    for r in asm.regions:
        regions.setdefault(r['name'], []).append(r)
    functions = []
    for b in claimed_blocks:
        if b.state == 'c':
            parent_result = audit_c(b, tu, prelude, tu_rel, header_set, regions, scaffold_names, obj_funcs,
                                    pool, start, classes.get(b.key, source_class),
                                    (None if b.key in pub_missing else contracts.get(b.key, default_contract)),
                                    word_sites,
                                    args.unit, args.tu_id, adaptations, rel(tu_c),
                                    tu_class=source_class, contract_lost=pub_missing.get(b.key),
                                    storage_view_context=dict(args=args, data_row=data_row))
            functions.append(parent_result)
            # The nested helper has no independent C block, but it does have a
            # real LOCAL FUNC symbol and a distinct acceptance record. Audit it
            # beside the parent so the pair is visible in the immutable report.
            entries = nested_by_parent.get(b.key) or nested_by_parent.get(b.name) or []
            for item in entries:
                nested_result = audit_nested_c(item, b, tu, prelude, tu_rel, header_set, regions,
                                               obj_funcs, pool, start,
                                               tu_class=source_class)
                functions.append(nested_result)
        else:
            functions.append(audit_accepted_asm(b, included, asm, obj_funcs, pool, start, word_sites, args.unit))
    result['functions'] = functions
    result['unit_checks'] = checks.rows
    result['summary'] = dict(
        claimed=len(functions), passed=sum(f['ok'] for f in functions), failed=sum(not f['ok'] for f in functions),
        record_fidelity=dict(collections.Counter(
            (f.get('checks', {}).get('record_fidelity') or {}).get('status', 'n/a') for f in functions)),
        accepted_records=dict(
            functions=len(accepted_rows),
            unclaimed=sum(1 for r in accepted_rows if not r.get('claimed') and r['state'] == 'c'),
            failed=sum(1 for r in accepted_rows if not r['ok']),
            stale_pins=len(stale_pins), not_carried=len(accepted_elsewhere),
            status=dict(collections.Counter(r.get('status', 'n/a') for r in accepted_rows))),
        unit_failures=[c['check'] for c in checks.rows if not c['ok']],
        contract_relaxed=result['contract_relaxed'],
        categories={c: sum(1 for f in functions if f['ok'] and f['category'] == c)
                    for c in ('exact_c', 'exact_c_with_asm', 'exact_asm')})
    result['ok'] = all(c['ok'] for c in checks.rows) and all(f['ok'] for f in functions)
    # An audit that claims nothing grants nothing, and says so instead of failing.
    result['grants_acceptance'] = bool(result['ok'] and functions and
                                     (not result['data_required'] or
                                      not result['data_audit']['pending']))
    if not functions:
        result['summary']['note'] = ('no claimed function: the TU checks passed but this audit grants no '
                                     'acceptance')
    return result


def verified_vu_data_alignment(args, tu, tu_rel, obj, elf, data_row, assembly, preprocessed):
    """Admit one evidenced VU store's DATA alignment in admission text only.

    The linked DATA gate has already validated the current source, compiler,
    assembly, carving and linked ELF hashes. This is not an ordinary-C
    attribute exception: the exact original sqc2 access requires a 16-byte
    address, and the same typed storage and capacity must be emitted by cc1.
    Missing evidence leaves the original attribute for normal rejection.
    """
    if (args.unit != 'main' or args.tu_id != 'main/tu208' or
            tu_rel != 'src/main/set_path.c' or not data_row):
        return {}, []
    tokens = tu_edit.c_tokens
    declaration = 'static SetPathDummyActor dmyActor_12 __attribute__((aligned(16)));'
    declarations = [item for item in tu.free_items() if item.kind == 'decl' and
                    not item.conditional and tokens(item.text(tu.text)) == tokens(declaration)]
    if len(declarations) != 1:
        return {}, []
    actor_type = '''typedef struct SetPathDummyActor {
        unsigned char unmodeled_00[0x50]; Vector4 rotation; Vector4 scale;
        unsigned char unmodeled_70[0x86 - 0x70]; short enabled;
        unsigned char unmodeled_88[0xa70 - 0x88]; } SetPathDummyActor;'''
    vector_type = 'typedef struct Vector4 { float x; float y; float z; float w; } Vector4;'
    def contains(haystack, needle):
        return any(haystack[i:i + len(needle)] == needle for i in range(len(haystack) - len(needle) + 1))
    if not contains(tokens(preprocessed), tokens(actor_type)) or not contains(tokens(preprocessed), tokens(vector_type)):
        return {}, []
    block = tu.by_name.get('seffectDebugCf')
    consumer = '__asm__ __volatile__("sqc2 $vf0, 0(%0)" : : "r"(&dmyActor_12.scale) : "memory");'
    if not block or block.state != 'c' or not contains(tokens(tu.block_text(block)), tokens(consumer)):
        return {}, []
    from tools.tu.storage_directives import emitted_storage
    storage = emitted_storage(assembly).get('dmyActor_12') or {}
    if (storage.get('section') != '.bss' or storage.get('size') != 2672 or
            storage.get('alignment') != 16 or storage.get('extent_kind') != 'label-space'):
        return {}, []
    symbols = [s for s in obj.symbols if s['name'] == 'dmyActor_12' and s['shndx'] != 0]
    section = obj.section('.bss')
    if (len(symbols) != 1 or not section or section['type'] != 8 or section['align'] < 16 or
            any(symbols[0][key] != value for key, value in
                dict(value=240, size=0, binding=0, type=STT_NOTYPE, section='.bss').items())):
        return {}, []
    rows = [r for r in data_row.get('verified_storage', []) if r.get('name') == 'dmyActor_12']
    if len(rows) != 1:
        return {}, []
    row = rows[0]
    if (not row.get('linked') or any(row.get(key) != value for key, value in
            dict(address='0x00585400', size=2672, section='.bss', bind=0,
                 type='NOTYPE', alignment=16).items())):
        return {}, []
    mapping = row.get('allocated_carve_mapping') or {}
    piece = mapping.get('piece') or {}
    if (mapping.get('raw_object_sha256') != data_row.get('object_sha256') or
            mapping.get('allocated_offset') != '0xf0' or
            mapping.get('address') != '0x00585400' or mapping.get('size') != 2672 or
            piece.get('offset') != '0xf0' or piece.get('length') != '0xa70' or
            piece.get('va') != '0x00585400' or piece.get('run_end') != '0x00585e70'):
        return {}, []
    original = [s for s in elf.symbols if s['name'] == 'dmyActor.12']
    caller = [s for s in elf.symbols if s['name'] == 'seffectDebugCf' and s['type'] == STT_FUNC]
    if (len(original) != 1 or len(caller) != 1 or
            any(original[0].get(key) != value for key, value in
                dict(va='0x00585400', size=0, binding=0, type=STT_NOTYPE).items())):
        return {}, []
    witnesses = {0x2dd1b4: 0x3c100058, 0x2dd1f4: 0x26105400,
                 0x2dd1f8: 0x24060a70, 0x2dd208: 0x26030060, 0x2dd20c: 0xf8600000}
    caller_lo = int(caller[0]['va'], 16)
    for address, word in witnesses.items():
        if not caller_lo <= address < caller_lo + caller[0]['size']:
            return {}, []
        segments = [p for p in elf.program_headers if p['type'] == 1 and
                    int(p['va'], 16) <= address and address + 4 <= int(p['va'], 16) + p['file_size']]
        if len(segments) != 1:
            return {}, []
        offset = segments[0]['file_offset'] + address - int(segments[0]['va'], 16)
        if elf.data[offset:offset + 4] != struct.pack('<I', word):
            return {}, []
    item = declarations[0]
    evidence = dict(name='dmyActor_12', original_name='dmyActor.12',
                    original_address='0x00585400', original_symbol_size=0,
                    storage_size=2672, field='scale', field_offset=96, alignment=16,
                    source_sha256=data_row['source_sha256'],
                    compiler_sha256=data_row['object_sha256'],
                    assembly_sha256=data_row['assembly_sha256'],
                    linked_elf_sha256=data_row['linked_elf_sha256'],
                    original_words={f'0x{k:08x}': f'0x{v:08x}' for k, v in witnesses.items()},
                    scope='admission text only; original source and emitted storage unchanged')
    return {item.start: 'static SetPathDummyActor dmyActor_12;'}, [evidence]


def verified_original_packet_alignment(args, tu, tu_rel, obj, elf, data_row,
                                       assembly, preprocessed):
    """One original-placement DATA alignment hypothesis, not hardware16.

    The SDK copies six 16-byte records with two ld instructions per record,
    requiring eight-byte source alignment. The stronger literal alignment16
    is a Source storage hypothesis accepted only with this exact original
    OBJECT, complete initializer and fresh linked DATA evidence. Nothing in
    ordinary-C function or general attribute admission is widened.
    """
    if (args.unit != 'main' or args.tu_id != 'main/tu132' or
            tu_rel != 'src/main/yajima_test.c' or not data_row):
        return {}, []
    tokens = tu_edit.c_tokens
    words = ((32769, 1342390272, 348654, 0), (458752, 0, 71, 0),
             (68, 0, 66, 0), (0, 0, 0, 0), (0, 0, 0, 0), (0, 0, 0, 0))
    initializer = '{ ' + ', '.join('{ ' + ', '.join(
        f'.{field} = {value}' for field, value in zip(('x', 'y', 'z', 'w'), row)) + ' }'
        for row in words) + ' }'
    attribute = '__attribute__((aligned(16)))'
    declaration = 'static UmlDispVertex TestEnv_42[6] ' + attribute + ' = ' + initializer + ';'
    items = [item for item in tu.free_items() if item.kind == 'decl' and
             not item.conditional and tokens(item.text(tu.text)) == tokens(declaration)]
    if len(items) != 1:
        return {}, []
    def contains(text, fragment):
        haystack, needle = tokens(text), tokens(fragment)
        return any(haystack[i:i + len(needle)] == needle for i in range(len(haystack) - len(needle) + 1))
    vertex = 'typedef struct UmlDispVertex { int x; int y; int z; int w; } UmlDispVertex;'
    block = tu.by_name.get('UmlDispTestSub')
    if (not contains(preprocessed, vertex) or not block or block.state != 'c' or
            not contains(tu.block_text(block), 'sceVif1PkAddDirectDataN(*packet, TestEnv_42, 6);')):
        return {}, []
    emission = re.compile(r'\.align\s+4\s+\.type\s+TestEnv_42\s*,\s*@object\s+'
                          r'\.size\s+TestEnv_42\s*,\s*96\s+TestEnv_42:')
    # #APP cannot establish compiler-owned storage or alignment.
    outside = re.sub(r'^\s*#APP\s*$.*?^\s*#NO_APP\s*$', '', assembly, flags=re.M | re.S)
    if len(emission.findall(outside)) != 1:
        return {}, []
    symbols = [s for s in obj.symbols if s['name'] == 'TestEnv_42' and s['shndx'] != 0]
    section = obj.section('.data')
    if (len(symbols) != 1 or not section or section['align'] < 16 or
            any(symbols[0].get(k) != v for k, v in
                dict(value=1536, size=96, type=STT_OBJECT, binding=0, section='.data').items())):
        return {}, []
    original = [s for s in elf.symbols if s['name'] == 'TestEnv.42']
    if (len(original) != 1 or any(original[0].get(k) != v for k, v in
            dict(va='0x0036a630', size=96, type=STT_OBJECT, binding=0, section='.data').items())):
        return {}, []
    payload = struct.pack('<24i', *(v for row in words for v in row))
    original_bytes = elf.data[original[0]['file_offset']:original[0]['file_offset'] + 96]
    if obj.bytes_of(section)[1536:1632] != payload or original_bytes != payload:
        return {}, []
    locations = [r for r in data_row.get('verified_location_spans', [])
                 if 'TestEnv_42' in r.get('names', []) and r.get('section') == '.data']
    if len(locations) != 1:
        return {}, []
    location = locations[0]
    input_span = location.get('compiler_input_span') or {}
    if (location.get('range') != [0x36a630, 0x36a690] or location.get('length') != 96 or
            input_span.get('section') != '.data' or input_span.get('offset') != 1536 or
            location.get('c_object_section_alignment', 0) < 16 or
            location.get('original_identities') != [dict(name='TestEnv.42', address='0x0036A630',
                                                       binding='LOCAL', type='OBJECT', size=96)]):
        return {}, []
    witnesses = {0x2552dc: 0x3c020037, 0x2552e0: 0x2442a630,
                 0x2552ec: 0x0040282d, 0x25530c: 0x24060006,
                 0x25535c: 0x08082d3c, 0x20b510: 0xdce30000,
                 0x20b518: 0xdce40008, 0x20b524: 0x24e70010}
    for address, word in witnesses.items():
        segments = [p for p in elf.program_headers if p['type'] == 1 and
                    int(p['va'], 16) <= address and address + 4 <= int(p['va'], 16) + p['file_size']]
        if len(segments) != 1:
            return {}, []
        offset = segments[0]['file_offset'] + address - int(segments[0]['va'], 16)
        if elf.data[offset:offset + 4] != struct.pack('<I', word):
            return {}, []
    evidence = dict(name='TestEnv_42', original_name='TestEnv.42', address='0x0036a630',
                    size=96, elements=6, element_size=16, requested_alignment=16,
                    SDK_required_alignment=8, alignment_role='original-placement Source hypothesis',
                    original_payload_sha256=hashlib.sha256(payload).hexdigest(),
                    source_sha256=data_row['source_sha256'], compiler_sha256=data_row['object_sha256'],
                    assembly_sha256=data_row['assembly_sha256'], linked_elf_sha256=data_row['linked_elf_sha256'],
                    original_words={hex(k): hex(v) for k, v in witnesses.items()},
                    no_general_attribute_or_hardware_exception=True)
    item = items[0]
    return {item.start: item.text(tu.text).replace(attribute, ' ')}, [evidence]


def verified_current_storage_placement_alignment(args, tu, tu_rel, obj, elf, data_row,
                                                 assembly, preprocessed):
    """Two literal variable-placement attributes after normal full DATA proof.

    Partial provider TUs retain every ordinary C/ASM function check. This
    admits neither a function/header attribute nor a TU132 fallback, and does
    not assert original alignment, spelling or hardware requirements.
    """
    identities={('main/tu179','src/main/ether_tree.c'):('etherTreeLineColorTable',0x4c9ab0),
                ('main/tu204','src/main/wind_test.c'):('windRotationAndScaleVector',0x4cbb60)}
    identity=identities.get((getattr(args,'tu_id',None),tu_rel))
    if args.unit!='main' or identity is None or not data_row:
        return {},[]
    context=verified_data_attribute_context(args,tu,tu_rel,obj,elf,data_row,
                Path(args.obj) if getattr(args,'obj',None) else None,assembly,preprocessed)
    if context is None:
        return {},[]
    try:
        from tools.tu import data_gate
        name,address=identity
        original_path=context['unit_dir']/'orig'/json.loads(
            (ROOT/'config/originals.json').read_text())['units']['main']['file']
        proof=data_gate.current_storage_placement_alignment(ROOT,context['unit_dir'],args.tu_id,name,
            context['source'],context['preprocessed_path'],context['assembly_path'],context['raw_path'],
            context['selected_path'],context['linked_path'],original_path)
        if elf.data!=original_path.read_bytes():
            return {},[]
        spans=[s for s in context['row'].get('verified_location_spans',[])
               if name in s.get('names',[]) or name in s.get('object_symbols',[])]
        if len(spans)!=1:
            return {},[]
        span=spans[0];input_=span.get('compiler_input_span') or {}
        if (span.get('names')!=[name] or span.get('object_symbols')!=[name] or
                span.get('section')!='.rodata' or span.get('range')!=[address,address+16] or
                span.get('split_range')!=[address,address+16] or span.get('length')!=16 or
                int(span.get('address','0'),16)!=address or span.get('original_identities')!=[] or
                span.get('storage_owner') is not None or
                span.get('support_only') or span.get('storage_only') or span.get('credited') is False or
                span.get('object_sha256')!=context['hashes']['linked_data_object_sha256'] or
                input_.get('section')!='.rodata' or input_.get('offset')!=0 or
                input_.get('section_size')!=proof['raw']['section_size'] or
                span.get('c_object_section')!='.rodata' or span.get('c_object_offset')!=0 or
                span.get('c_object_section_size')!=16 or span.get('c_object_section_alignment')!=16):
            return {},[]
        for locations in (context['row'].get('verified_locations',[]),
                          context['candidate'].get('recovered_location_candidates',[]),
                          context['control'].get('identities',[])):
            matching=[x for x in locations if x.get('unit')=='main' and x.get('tu')==args.tu_id and
                      x.get('section')=='.rodata' and int(x.get('address','0'),16)==address and
                      x.get('names')==[name] and not x.get('support_only') and
                      not x.get('storage_only') and x.get('credited') is not False]
            if len(matching)!=1:
                return {},[]
        if [address,address+16] not in context['row'].get('compiler_owned_ranges',{}).get('.rodata',[]):
            return {},[]
        fresh=[s for s in obj.symbols if s['name']==name]
        raw_section=obj.section('.rodata')
        if (len(fresh)!=1 or not raw_section or raw_section['align']!=16 or
                any(fresh[0].get(k)!=v for k,v in dict(value=0,size=16,type=STT_OBJECT,
                    binding=1,section='.rodata').items()) or
                hashlib.sha256(obj.bytes_of(raw_section)[:16]).hexdigest()!=proof['payload_sha256']):
            return {},[]
        declarations=[i for i in tu.free_items() if i.kind=='decl' and not i.conditional and
                      tu_edit.c_tokens(i.text(tu.text))==tu_edit.c_tokens(proof['declaration'])]
        if len(declarations)!=1:
            return {},[]
        item=declarations[0];attribute='__attribute__((aligned(16)))'
        # The full declaration is exact before this literal occurrence is
        # removed. No typedef, nearby item, function or header is normalized.
        if item.text(tu.text).count(attribute)!=1:
            return {},[]
        evidence=dict(proof,normal_linked_DATA_hashes=context['hashes'],
            eligibility_scope='exact two current C DATA variable declarations only',
            no_inherited_or_TU132_fallback=True,grants_provider_or_function_completion=False)
        return {item.start:item.text(tu.text).replace(attribute,' ',1)},[evidence]
    except (AuditError,OSError,ValueError,KeyError,TypeError,tu_edit.ParseError):
        return {},[]


def verified_data_attribute_context(args, tu, tu_rel, obj, elf, row, build_object,
                                    assembly, preprocessed):
    """Bind per-object placement admission to the current complete DATA chain.

    This does not admit any function or header. The fresh audit's ordinary
    object/assembly/source checks still apply, including every unrelated C
    function. Only initialized positive OBJECT storage is considered here.
    """
    if not (args and row and build_object and assembly and preprocessed and
            getattr(args, 'data_gate_report', None) and getattr(args, 'candidate_manifest', None) and
            getattr(args, 'data_elf_gate_report', None) and
            getattr(args, 'build_s', None) and getattr(args, 'obj', None)):
        return None
    try:
        source, raw_path = Path(args.tu_c).resolve(), Path(args.obj).resolve()
        unit_dir = Path(args.unit_dir).resolve()
        if (source != (ROOT / tu_rel).resolve() or not source.is_relative_to(ROOT.resolve()) or
                Path(build_object).resolve() != raw_path or not raw_path.is_relative_to(unit_dir) or
                source.read_text(encoding='utf-8', errors='surrogateescape') != tu.text):
            return None
        report = json.loads(Path(args.data_gate_report).read_text())
        elf_gate_path = Path(args.data_elf_gate_report).resolve()
        elf_gate = json.loads(elf_gate_path.read_text())
        if (report.get('schema') != 'linked-c-data-audit/2' or report.get('result') != 'pass' or
                report.get('grants_data_recovery') is not True or report.get('problems') or
                report.get('uncovered_storage_locations') != 0 or
                report.get('gate_sha256') != sha256_file(elf_gate_path) or
                elf_gate.get('schema') != 'elf-gate/1' or elf_gate.get('result') != 'pass'):
            return None
        # Re-read the actual gate chain; caller-supplied or amended row dictionaries
        # cannot replace any fact in the passing report.
        current = data_gate_row(args, source)
        if not current or any(row.get(k) != v for k, v in current.items()):
            return None
        manifest_path = Path(args.candidate_manifest).resolve()
        manifest = json.loads(manifest_path.read_text())
        control_path = Path(report.get('allocation_control_path', '/nonexistent')).resolve()
        if (args.unit not in manifest.get('linked_targets', []) or not control_path.is_file() or
                sha256_file(control_path) != report.get('allocation_control_sha256') or
                row.get('allocation_packet_sha256') != report.get('allocation_control_sha256')):
            return None
        control = json.loads(control_path.read_text())
        candidate_pin = current.get('_audit_candidate_record') or {}
        candidate_path = Path(candidate_pin.get('path', '/nonexistent')).resolve()
        candidate = json.loads(candidate_path.read_text())
        if (control.get('schema') != 'data-recovery-allocation/1' or args.tu_id not in control.get('tus', []) or
                candidate.get('tu') != args.tu_id or candidate.get('unit') != args.unit or
                candidate.get('allocation_task') != control.get('task_id')):
            return None
        assembly_path = Path(args.build_s).resolve()
        preprocessed_path = assembly_path.with_suffix('.i')
        selected_path = Path(current.get('_audit_linked_data_object', '/nonexistent')).resolve()
        linked_path = unit_dir / 'build' / (args.unit + '.rom.elf')
        hashes = dict(source_sha256=sha256_file(source), object_sha256=sha256_file(raw_path),
                      assembly_sha256=sha256_file(assembly_path),
                      linked_data_object_sha256=sha256_file(selected_path),
                      linked_elf_sha256=sha256_file(linked_path))
        if (any(row.get(k) != v for k, v in hashes.items()) or
                Path(row.get('compiler_raw_object_path', '/nonexistent')).resolve() != raw_path or
                preprocessed_path.read_text(encoding='utf-8', errors='replace') != preprocessed or
                compare_assembly(assembly, assembly_path.read_text(encoding='utf-8', errors='surrogateescape'))):
            return None
        compiler_pin = candidate.get('compiler_object') if current.get('_audit_compiler_allocation') else candidate.get('object')
        pins = [(candidate.get('files', {}).get(tu_rel), source),
                (compiler_pin, raw_path), (candidate.get('assembly'), assembly_path)]
        if any(not pin or Path(pin.get('path', '/nonexistent')).resolve() != path or
               pin.get('sha256') != sha256_file(path) for pin, path in pins):
            return None
        original, original_pin = original_elf(args.unit)
        target = elf_gate.get('targets', {}).get(args.unit) or {}
        original_target, built_target = target.get('original') or {}, target.get('built') or {}
        gate_manifest = elf_gate.get('manifest') or {}
        if (elf.data != original.data or target.get('identical') is not True or
                original_target.get('sha256') != original_pin['sha256'] or
                built_target.get('sha256') != original_pin['sha256'] or
                sha256_file(built_target.get('path', '/nonexistent')) != original_pin['sha256'] or
                gate_manifest.get('sha256') != sha256_file(ROOT / gate_manifest.get('path', '/nonexistent'))):
            return None
        from tools.inventory import ELF
        return dict(row=current, candidate=candidate, control=control, source=source,
                    report=report, raw_path=raw_path, selected_path=selected_path,
                    linked_path=linked_path, assembly_path=assembly_path,
                    preprocessed_path=preprocessed_path,
                    raw=RelObject(raw_path.read_bytes()), selected=RelObject(selected_path.read_bytes()),
                    linked=ELF(linked_path.read_bytes(), args.unit), pre=Preprocessed(preprocessed),
                    pre_tu=tu_edit.parse(preprocessed), assembly=assembly,
                    hashes=dict(hashes, preprocessed_sha256=sha256_file(preprocessed_path),
                                original_sha256=original_pin['sha256'],
                                elf_gate_sha256=sha256_file(elf_gate_path),
                                report_sha256=sha256_file(args.data_gate_report),
                                manifest_sha256=sha256_file(manifest_path),
                                candidate_sha256=sha256_file(candidate_path),
                                allocation_control_sha256=sha256_file(control_path)), unit_dir=unit_dir)
    except (AuditError, OSError, ValueError, KeyError, TypeError, tu_edit.ParseError):
        return None


def verified_data_attribute_object(context, args, tu, item, name, section, obj, elf):
    """Prove one complete same-name positive original OBJECT, never a prefix."""
    if context is None:
        return None
    try:
        row, raw, selected = context['row'], context['raw'], context['selected']
        text = item.text(tu.text)
        token_text = tu_edit.c_tokens(text)
        declarations = [i for i in context['pre_tu'].free_items() if i.kind == 'decl' and
                        not i.conditional and tu_edit.c_tokens(i.text(context['pre'].text)) == token_text]
        if len(declarations) != 1 or '=' not in token_text:
            return None
        where, line = context['pre'].attribute(declarations[0].start)
        if (not where or (context['unit_dir'] / where).resolve() != context['source'] or
                not tu_edit.line_of(tu.text, item.start) <= line <= tu_edit.line_of(tu.text, item.end)):
            return None
        # An attribute on a function declarator is never a DATA attribute.
        stripped = re.sub(r'\b__attribute__?\s*\(\s*\(\s*section\s*\(\s*'
                          r'"\.[a-z0-9]+"\s*\)\s*\)\s*\)', '', text)
        before_initializer = tu_edit.c_tokens(stripped.split('=', 1)[0])
        if any(t in before_initializer for t in ('(', ')', '*', 'typedef', 'extern')):
            return None
        originals = [s for s in elf.symbols if s['name'] == name]
        symbols = [[s for s in image.symbols if s['name'] == name and s['shndx'] != 0]
                   for image in (obj, raw, selected)]
        if len(originals) != 1 or any(len(values) != 1 for values in symbols):
            return None
        original = originals[0]
        after, raw_symbol, selected_symbol = [values[0] for values in symbols]
        size, address = original['size'], int(original['va'], 16)
        identity = lambda s: (s['type'], s['binding'], s['size'])
        if (original['type'] != STT_OBJECT or not 0 < size <= 65536 or original.get('section') != section or
                any(identity(s) != identity(original) for s in (after, raw_symbol, selected_symbol)) or
                after['section'] != raw_symbol['section'] or after['value'] != raw_symbol['value'] or
                raw_symbol['section'] != section):
            return None
        spans = [s for s in row.get('verified_location_spans', [])
                 if name in s.get('names', []) or name in s.get('object_symbols', [])]
        if len(spans) != 1:
            return None
        span = spans[0]
        input_span = span.get('compiler_input_span') or {}
        expected_identity = dict(name=name, address=f'0x{address:08X}',
                                 binding={0:'LOCAL', 1:'GLOBAL'}.get(original['binding']),
                                 type='OBJECT', size=size)
        if (span.get('names') != [name] or span.get('object_symbols') != [name] or
                span.get('section') != section or span.get('range') != [address, address + size] or
                span.get('split_range') != [address, address + size] or
                span.get('length') != size or int(span.get('address', '0'), 16) != address or
                span.get('original_identities') != [expected_identity] or
                span.get('object_sha256') != context['hashes']['linked_data_object_sha256'] or
                span.get('support_only') or span.get('storage_only') or
                span.get('credited') is False or span.get('storage_owner') or
                input_span.get('section') != section or input_span.get('offset') != raw_symbol['value'] or
                selected_symbol['section'] != span.get('c_object_section') or
                selected_symbol['value'] != span.get('c_object_offset')):
            return None
        for locations in (row.get('verified_locations', []),
                          context['candidate'].get('recovered_location_candidates', []),
                          context['control'].get('identities', [])):
            matches = [loc for loc in locations if loc.get('unit') == args.unit and
                       loc.get('tu') == args.tu_id and loc.get('section') == section and
                       int(loc.get('address', '0'), 16) == address and loc.get('names') == [name] and
                       not loc.get('support_only') and not loc.get('storage_only') and
                       loc.get('credited') is not False]
            if len(matches) != 1:
                return None
        if not any(pair == [address, address + size]
                   for pair in row.get('compiler_owned_ranges', {}).get(section, [])):
            return None
        raw_section, selected_section = raw.section(section), selected.section(selected_symbol['section'])
        if (not raw_section or not selected_section or raw_section['type'] != 1 or selected_section['type'] != 1 or
                raw_section['size'] != input_span.get('section_size') or
                selected_section['size'] != span.get('c_object_section_size') or
                selected_section['align'] != span.get('c_object_section_alignment')):
            return None
        linked = [s for s in context['linked'].symbols if s['name'] == name and
                  int(s['va'], 16) == address and s.get('section') == section and identity(s) == identity(original)]
        if len(linked) != 1:
            return None
        for image, symbol in ((raw, raw_symbol), (selected, selected_symbol)):
            if any(sec == symbol['section'] and symbol['value'] <= off < symbol['value'] + size
                   for sec, off, _kind, _name in image.relocations()):
                return None
        from tools.tu.data_gate import compiler_only_section
        compiler_only_section(context['assembly'], context['pre'].text, context['unit_dir'], section)
        outside = re.sub(r'^\s*#APP\s*$.*?^\s*#NO_APP\s*$', '', context['assembly'], flags=re.M | re.S)
        if (len(re.findall(r'^\s*\.type\s+' + re.escape(name) + r'\s*,\s*@object\s*$', outside, re.M)) != 1 or
                len(re.findall(r'^\s*\.size\s+' + re.escape(name) + r'\s*,\s*' + str(size) + r'\s*$', outside, re.M)) != 1 or
                len(re.findall(r'^\s*' + re.escape(name) + r':\s*$', outside, re.M)) != 1 or
                any(re.search(r'\b' + re.escape(name) + r'\b', line)
                    for source in (tu.text, context['pre'].text) for line in asm_statement_lines(source))):
            return None
        original_bytes = elf.data[original['file_offset']:original['file_offset'] + size]
        linked_symbol = linked[0]
        linked_bytes = context['linked'].data[linked_symbol['file_offset']:linked_symbol['file_offset'] + size]
        payloads = [image.bytes_of(image.section(symbol['section']))[symbol['value']:symbol['value'] + size]
                    for image, symbol in ((obj, after), (raw, raw_symbol), (selected, selected_symbol))]
        if len(original_bytes) != size or any(p != original_bytes for p in [linked_bytes, *payloads]):
            return None
        return dict(name=name, section=section, size=size, address=original['va'],
                    binding=original['binding'], type=STT_OBJECT, full_object_span=span['range'],
                    compiler_input_span=input_span, payload_sha256=hashlib.sha256(original_bytes).hexdigest(),
                    evidence='complete_linked_per_object_original_positive_OBJECT',
                    grants_function_acceptance=False, **context['hashes'])
    except (AuditError, OSError, ValueError, KeyError, TypeError, tu_edit.ParseError):
        return None


def data_attribute_require(condition, message):
    if not condition:
        raise AuditError('DATA attribute: ' + message)


def data_attribute_canonical_root():
    """The shared preimage, never an installed tool's mutable subject stage."""
    parts=ROOT.resolve().parts
    if '.work' in parts:
        return Path(*parts[:parts.index('.work')])
    return TOOL_ROOT.resolve()


def data_attribute_digest(value):
    return hashlib.sha256(json.dumps(value,sort_keys=True,separators=(',',':'),
                                     ensure_ascii=True).encode()).hexdigest()


def data_attribute_pin(pin,expected=None):
    data_attribute_require(isinstance(pin,dict) and set(pin)=={'path','sha256'},'exact file pin required')
    path=Path(pin['path']).resolve()
    data_attribute_require(path.is_file() and pin['sha256']==sha256_file(path) and
                           (expected is None or path==Path(expected).resolve()),'missing/stale/wrong input pin')
    return path


def data_attribute_gate_bound_pin(context,path):
    path=Path(path).resolve()
    data_attribute_require(path.is_relative_to(ROOT.resolve()),'proof dependency outside actual subject')
    key=path.relative_to(ROOT.resolve()).as_posix()
    pin=context['candidate'].get('files',{}).get(key)
    data_attribute_pin(pin,path)
    context.setdefault('extension_pins',{})[str(path)]=pin['sha256']
    return path


def data_attribute_proof_document(context,args):
    path=getattr(args,'data_attribute_proof',None)
    data_attribute_require(path,'explicit DATA attribute proof is required')
    document=json.loads(Path(path).read_text())
    data_attribute_require(document.get('schema')=='data-section-attribute-proof/1' and
                           document.get('grants_function_acceptance') is False and
                           document.get('grants_data_recovery') is False,'unsupported proof document or grant')
    pins=document.get('gate_pins') or {}
    expected=dict(data=args.data_gate_report,elf=args.data_elf_gate_report,manifest=args.candidate_manifest,
                  control=context['report']['allocation_control_path'],
                  config_data_carves=ROOT/'config/objects/data-carves.json',
                  config_tu_build=ROOT/'config/tu-build.json',unit_manifest=context['unit_dir']/'tu-manifest.json')
    data_attribute_require(set(pins)==set(expected)|{'candidates'},'exact gate/config pin set required')
    for key,value in expected.items():
        data_attribute_pin(pins[key],value)
    candidate=context['row']['_audit_candidate_record']
    data_attribute_pin(pins['candidates'].get(args.tu_id),candidate['path'])
    manifest=json.loads(Path(args.candidate_manifest).read_text())
    candidates=manifest.get('candidates',[])
    data_attribute_require(isinstance(pins['candidates'],dict) and
        len({c.get('tu') for c in candidates})==len(candidates) and
        set(pins['candidates'])=={c.get('tu') for c in candidates},
        'proof candidate pins must equal the complete actual passing manifest')
    for candidate in candidates:
        actual=data_attribute_pin(pins['candidates'][candidate['tu']],candidate['record'])
        data_attribute_require(sha256_file(actual)==candidate.get('sha256'),'proof candidate differs from passing manifest')
    records=document.get('records')
    data_attribute_require(isinstance(records,list) and 0<len(records)<=64,'bounded explicit partial-view records required')
    seen=set();ranges=[]
    for record in records:
        identity=(record.get('unit'),record.get('tu'),record.get('name'))
        interval=record.get('semantic_range')
        data_attribute_require(record.get('kind')=='registered_partial_storage_view' and
            identity[0]==args.unit and identity[1] in pins['candidates'] and
            isinstance(identity[2],str) and re.fullmatch(r'[A-Za-z_]\w*',identity[2]) and identity not in seen and
            isinstance(interval,list) and len(interval)==2 and
            all(isinstance(v,int) for v in interval) and 0<=interval[0]<interval[1] and
            interval[1]-interval[0]<=65536,'unsupported/duplicate/malformed partial-view registration')
        data_attribute_require(not any(max(interval[0],lo)<min(interval[1],hi) for lo,hi in ranges),
                               'overlapping/doubly registered semantic ranges')
        seen.add(identity);ranges.append(interval)
    return document


def data_attribute_review_task(connection,repository,task_id,expected_owner=None,expected_sha256=None,supplied_pin=None):
    """One actual sealed queue submission and its literal artifact assignment."""
    import stat
    data_attribute_require(isinstance(task_id,str) and bool(task_id),'review task identity is required')
    row=connection.execute('SELECT owner,status,artifact FROM tasks WHERE id=?',(task_id,)).fetchone()
    data_attribute_require(row and row[0] and row[1] in ('submitted','done') and row[2] and
        (expected_owner is None or row[0]==expected_owner),
        'review task is not an actual submitted/done assignment under the trusted reviewer owner')
    artifact=Path(row[2])
    if not artifact.is_absolute():artifact=repository/artifact
    artifact=artifact.resolve()
    data_attribute_require(connection.execute('SELECT 1 FROM resources WHERE task_id=? AND resource=?',
        (task_id,'file:'+str(artifact))).fetchone() is not None,
        'review artifact lacks its exact literal queue file assignment')
    if supplied_pin is not None:data_attribute_pin(supplied_pin,artifact)
    data_attribute_require(artifact.is_file() and not stat.S_IMODE(artifact.stat().st_mode)&0o222 and
        (expected_sha256 is None or sha256_file(artifact)==expected_sha256),
        'review artifact is not the sealed approved submission')
    receipt=json.loads(artifact.read_text())
    data_attribute_require(receipt.get('task')==task_id and receipt.get('reviewer')==row[0] and
        receipt.get('decision')=='READY' and receipt.get('scope')=='DATA-placement-only' and
        receipt.get('grants_function_acceptance') is False,
        'actual review submission is not a READY DATA-placement-only receipt under its assigned owner')
    return dict(task_id=task_id,owner=row[0],artifact=str(artifact),sha256=sha256_file(artifact)),receipt


def data_attribute_review(record):
    """Authenticate a sealed distinct review through the actual shared queue.

    Record hashes cover canonical JSON excluding only independent_review.
    The live gate-pin digest is part of that record, so no gate identity is
    omitted from the reviewed payload.
    """
    import sqlite3
    import stat
    review=record.get('independent_review') or {}
    digest=data_attribute_digest({k:v for k,v in record.items() if k!='independent_review'})
    data_attribute_require(review.get('verdict')=='READY' and review.get('record_sha256')==digest and
                           review.get('author') and review.get('reviewer') and
                           review['author']!=review['reviewer'],'missing distinct semantic review')
    artifact=data_attribute_pin(dict(path=review.get('artifact'),sha256=review.get('sha256')))
    data_attribute_require(not stat.S_IMODE(artifact.stat().st_mode)&0o222,'review artifact is not sealed readonly')
    repository=data_attribute_canonical_root()
    db=repository/'.work/queue.sqlite3'
    # The original independently approved assignments are immutable trust
    # anchors for their TU domains. A fresh sealed task may attest a new core
    # only under that same actual queue owner, never under an authored name.
    review_tasks={**{f'main/tu{n}':('main-data100-partial-storage-b-review-20261020',
                       'c1a79f95f0d2c8fb3bb619398b7e24aa4351a999acf4c07a067f485291a4a64c')
                    for n in (120,155,163,169,170)},
                  **{f'main/tu{n}':('main-data100-partial-storage-c-review-20261020',
                       'a9b6bdd81ade11af7586e82d5877eff48089adf4e1f68363e1ef8aadec4f279e')
                    for n in (172,174,178,191,197)}}
    data_attribute_require(record.get('tu') in review_tasks,'review TU outside the original closed DATA domain')
    with sqlite3.connect('file:'+str(db)+'?mode=ro',uri=True) as connection:
        anchor_task,anchor_sha=review_tasks[record['tu']]
        anchor,_anchor_receipt=data_attribute_review_task(connection,repository,anchor_task,
                                                         expected_sha256=anchor_sha)
        actual,receipt=data_attribute_review_task(connection,repository,review.get('task_id'),
            expected_owner=anchor['owner'],supplied_pin=dict(path=str(artifact),sha256=review['sha256']))
    data_attribute_require(actual['owner']==review['reviewer'] and
                           digest in receipt.get('reviewed_data_attribute_proof_records',[]),
                           'fresh sealed review does not attest this complete canonical semantic record under the trusted owner')
    proposals=record.get('source_proposals')
    data_attribute_require(isinstance(proposals,list) and proposals,'actual sealed source proposal required')
    authors=set();current_source_attested=False
    for proposal in proposals:
        source_artifact=data_attribute_pin(proposal.get('artifact'))
        candidate=data_attribute_pin(proposal.get('candidate'))
        data_attribute_require(not stat.S_IMODE(source_artifact.stat().st_mode)&0o222 and
                               not stat.S_IMODE(candidate.stat().st_mode)&0o222,'source proposal files not sealed readonly')
        with sqlite3.connect('file:'+str(db)+'?mode=ro',uri=True) as connection:
            source_task=connection.execute('SELECT owner,status,artifact FROM tasks WHERE id=?',
                                            (proposal.get('task_id'),)).fetchone()
        data_attribute_require(source_task and source_task[0]==proposal.get('owner') and
                               source_task[1] in ('submitted','done') and source_task[2],
                               'source author is not bound to an actual sealed source task')
        registered_source=Path(source_task[2])
        if not registered_source.is_absolute():
            registered_source=repository/registered_source
        data_attribute_require(registered_source.resolve()==source_artifact,'source artifact differs from actual task')
        source_report=json.loads(source_artifact.read_text())
        attestations=source_report.get('source_candidate_attestations') or []
        data_attribute_require(any(a.get('tu')==record['tu'] and a.get('candidate')==proposal['candidate']
                                   for a in attestations),'source report lacks an explicit exact TU/candidate attestation')
        authors.add(source_task[0])
        if proposal['candidate']['sha256']==record.get('inputs',{}).get('source',{}).get('sha256'):
            current_source_attested=True
    data_attribute_require(current_source_attested and review['reviewer'] not in authors and
                           review['author'] in authors,'source author/reviewer independence or current source binding failed')
    return dict(task_id=review['task_id'],artifact=str(artifact),sha256=sha256_file(artifact),
                reviewer=review['reviewer'],authors=sorted(authors),record_sha256=digest,trusted_anchor=anchor)


def data_attribute_registry(context, args):
    """Actual current registry; callers must still prove every physical byte."""
    for relative in ('tools/tu/data_carve.py','tools/tu/data_gate.py'):
        data_attribute_gate_bound_pin(context,ROOT/relative)
    path = data_attribute_gate_bound_pin(context,ROOT / 'config/objects/data-carves.json')
    registry = json.loads(path.read_text())
    data_attribute_require(args.unit == 'main' and registry.get('schema'), 'unsupported registry/unit')
    return registry


def data_attribute_input_spans(context, args, transport_only=False):
    registry = data_attribute_registry(context, args)
    result = []
    for section, runs in registry.get('tus', {}).get(args.tu_id, {}).items():
        for run in runs:
            if not isinstance(run, dict):
                continue
            for part in run.get('c_input_spans', []):
                if not transport_only and (part.get('credit') == 'uncredited_dependency' or part.get('anonymous_emission')):
                    continue
                interval, original_range = part.get('object_range'), part.get('range')
                if not interval or not original_range:
                    continue
                lo, hi = [int(v, 0) if isinstance(v, str) else int(v) for v in original_range]
                data_attribute_require(len(interval) == 2 and 0 <= interval[0] < interval[1] and
                                       hi - lo == interval[1] - interval[0], 'invalid registered input span')
                result.append(dict(section=part.get('section', section), offset=interval[0],
                                   end=interval[1], address=lo, end_address=hi,
                                   symbols=part.get('symbols', []), run=run, part=part))
    for index, part in enumerate(result):
        data_attribute_require(not any(other['section']==part['section'] and
            max(part['offset'],other['offset']) < min(part['end'],other['end'])
            for other in result[:index]), 'overlapping/multiply-owned compiler input spans')
    return result


def data_attribute_declaration(context, tu, item, name, pointer_table=False):
    tokens = tu_edit.c_tokens(item.text(tu.text))
    declarations = [i for i in context['pre_tu'].free_items() if i.kind == 'decl' and
                    not i.conditional and tu_edit.c_tokens(i.text(context['pre'].text)) == tokens]
    data_attribute_require(len(declarations) == 1 and '=' in tokens, 'unique initialized CPP declaration')
    where, line = context['pre'].attribute(declarations[0].start)
    data_attribute_require(where and (context['unit_dir']/where).resolve() == context['source'] and
                           tu_edit.line_of(tu.text,item.start) <= line <= tu_edit.line_of(tu.text,item.end),
                           'declaration must originate in this actual source, not a header')
    clean = re.sub(r'\b__attribute__?\s*\(\s*\(\s*section\s*\(\s*"\.[a-z0-9]+"\s*\)\s*\)\s*\)',
                   ' ', item.text(tu.text))
    declarator = tu_edit.c_tokens(clean.split('=',1)[0])
    data_attribute_require(not any(t in declarator for t in ('(',')','typedef','extern','__attribute__','__attribute')),
                           'unsupported DATA declarator')
    if pointer_table:
        # This extension is closed to complete char-pointer arrays. Function
        # pointers, casts in declarators, arbitrary attributes and other types
        # remain under ordinary eligibility.
        spelling = ' '.join(declarator)
        pattern = (r'(?:static )?(?:const )?char \* (?:const )?' + re.escape(name) +
                   r' \[ ([0-9]+) \]')
        match = re.fullmatch(pattern,spelling)
        data_attribute_require(match and 1 <= int(match[1]) <= 16, 'not a bounded char-pointer table')
        return int(match[1])*4
    return None


def data_attribute_emission(context, tu, name, size, section):
    from tools.tu.data_gate import compiler_only_section
    compiler_only_section(context['assembly'],context['pre'].text,context['unit_dir'],section)
    outside = re.sub(r'^\s*#APP\s*$.*?^\s*#NO_APP\s*$', '',context['assembly'],flags=re.M|re.S)
    for expression in (r'^\s*\.type\s+'+re.escape(name)+r'\s*,\s*@object\s*$',
                       r'^\s*\.size\s+'+re.escape(name)+r'\s*,\s*'+str(size)+r'\s*$',
                       r'^\s*'+re.escape(name)+r':\s*$'):
        data_attribute_require(len(re.findall(expression,outside,re.M)) == 1,'unique compiler OBJECT emission')
    data_attribute_require(not any(re.search(r'\b'+re.escape(name)+r'\b',line)
        for source in (tu.text,context['pre'].text) for line in asm_statement_lines(source)),
        'ASM cannot establish DATA ownership')


def data_attribute_pointer_initializers(tu,item,count):
    text=re.sub(r'\b__attribute__?\s*\(\s*\(\s*section\s*\(\s*"\.[a-z0-9]+"\s*\)\s*\)\s*\)',
                ' ',item.text(tu.text))
    tokens=tu_edit.c_tokens(text)
    initializer=tokens[tokens.index('=')+1:]
    data_attribute_require(initializer[:1]==['{'] and initializer[-2:]==['}',';'],
                           'pointer initializer must be a complete symbolic array')
    entries=[];part=[]
    for token in initializer[1:-2]+[',']:
        if token==',':
            if part:
                entries.append(part);part=[]
        else:
            part.append(token)
    data_attribute_require(len(entries)==count,'pointer initializer must spell every element')
    names=[]
    for entry in entries:
        if entry[:4]==['(','char','*',')']:
            entry=entry[4:]
        elif entry[:5]==['(','const','char','*',')']:
            entry=entry[5:]
        data_attribute_require(len(entry)==1 and re.fullmatch(r'[A-Za-z_]\w*',entry[0]),
                               'only actual symbolic pointees, no numeric pointer placeholders')
        names.append(entry[0])
    return names


def data_attribute_character_pointee(context,name):
    declarations=[]
    for item in context['pre_tu'].free_items():
        if item.kind!='decl' or item.conditional:
            continue
        text=item.text(context['pre'].text)
        if ('ordinary',name) not in tu_edit.decl_keys(text):
            continue
        tokens=tu_edit.c_tokens(text)
        spelling=' '.join(tokens[:tokens.index('=')] if '=' in tokens else tokens[:-1])
        if re.fullmatch(r'(?:(?:extern|static) )?(?:const )?char '+re.escape(name)+r' \[ (?:[0-9]+ )?\]',spelling):
            declarations.append(text)
    data_attribute_require(declarations,'actual source/CPP pointee must be a character array')
    return declarations


def data_attribute_rel_entries(image, section, start, size):
    """Index-preserving REL records; empty section-symbol names are never guessed."""
    records = []
    symtabs = [s for s in image.sections if s['type'] == 2]
    data_attribute_require(len(symtabs) == 1,'one actual symbol table required')
    for rel in image.sections:
        if rel['type'] not in (4,9) or rel['info'] != section['index']:
            continue
        data_attribute_require(rel['type'] == 9 and rel['link'] == symtabs[0]['index'] and
                               rel['entsize'] in (0,8) and rel['size'] % 8 == 0,
                               'unsupported or malformed relocation section')
        for index in range(rel['size']//8):
            offset,info = struct.unpack_from('<II',image.data,rel['offset']+index*8)
            if offset < start+size and offset+4 > start:
                kind,symbol_index = info & 255,info >> 8
                data_attribute_require(kind == 2 and start <= offset and offset+4 <= start+size and
                                       offset % 4 == 0 and symbol_index < len(image.symbols),
                                       'only complete aligned R_MIPS_32 operands are supported')
                symbol = image.symbols[symbol_index]
                data_attribute_require(symbol['index'] == symbol_index,'relocation symbol index mismatch')
                addend = struct.unpack_from('<i',image.bytes_of(section),offset)[0]
                records.append(dict(offset=offset,relative=offset-start,type=kind,symbol=dict(symbol),addend=addend))
    offsets = [r['relative'] for r in records]
    data_attribute_require(len(offsets) == len(set(offsets)),'overlapping/duplicate R_MIPS_32 operands')
    return sorted(records,key=lambda r:r['relative'])


def data_attribute_named_target(context,args,name,elf):
    """Resolve an uncredited external label from the actual native TU map."""
    from tools.tu import data_carve
    manifest_path = data_attribute_gate_bound_pin(context,context['unit_dir']/'tu-manifest.json')
    manifest = json.loads(manifest_path.read_text())
    linked = [s for s in context['linked'].symbols if s['name'] == name and s['section_index'] != 0]
    data_attribute_require(len(linked) == 1,'external target must have one linked symbol')
    address = int(linked[0]['va'],16)
    owners = []
    for entry in manifest.get('tus',[]):
        for section,record in entry.get('sections',{}).items():
            if (section in DATA_SECTIONS and int(record.get('start','0'),0) <= address <
                    int(record.get('end','0'),0)):
                owners.append((entry['id'],section))
    data_attribute_require(len(owners) == 1,'external target native owner must be unique')
    owner,section = owners[0]
    native = data_carve.tu_context(ROOT,args.unit,context['unit_dir'],owner)
    items = [i for i in data_carve.section_items(native,section)
             if i['start'] == address and name in i['labels']]
    data_attribute_require(len(items) == 1 and items[0]['end'] > address,'exact registered target label required')
    pieces = [p for p in native['pieces'].get(section,[]) if p['start'] <= address < p['end']]
    data_attribute_require(len(pieces) == 1,'external target piece must be unique')
    piece_path = data_attribute_gate_bound_pin(context,Path(native['path_of'](pieces[0],section)))
    return dict(address=address,end=items[0]['end'],section=section,owner=owner,name=name,
                registration='actual_native_manifest_and_scaffold_label',pointee_credit=0)


def data_attribute_resolve_r32(context,args,image,relocation,elf,selected=False):
    symbol,addend = relocation['symbol'],relocation['addend']
    if symbol['shndx'] == 0:
        data_attribute_require(symbol['name'] and symbol['binding'] == 1,'unsupported undefined R32 symbol')
        target = data_attribute_named_target(context,args,symbol['name'],elf)
        data_attribute_require(0 <= addend < target['end']-target['address'],'external R32 addend outside registered item')
        return dict(target,addend=addend,value=target['address']+addend,symbol_index=symbol['index'])
    data_attribute_require(0 < symbol['shndx'] < len(image.sections),'unsupported absolute/COMMON R32 target')
    section = image.sections[symbol['shndx']]['name']
    offset = (symbol['value']+addend) & 0xffffffff
    mappings = []
    if selected:
        for span in context['row'].get('verified_location_spans',[]):
            if (span.get('c_object_section') == section and span.get('c_object_offset',-1) <= offset <
                    span.get('c_object_offset',-1)+span.get('length',0)):
                mappings.append(dict(address=span['range'][0]+offset-span['c_object_offset'],
                                     end=span['range'][1],section=span['section']))
        # Existing C-owned storage may intentionally carry no new DATA claim.
        for part in data_attribute_input_spans(context,args,transport_only=True):
            names=part['symbols']
            syms=[s for s in image.symbols if s['name'] in names and s['shndx'] != 0 and
                  s['section'] == section and s['value'] <= offset < s['value']+s['size']]
            for sym in syms:
                if len(names)==1 and sym['size']==part['end']-part['offset']:
                    mappings.append(dict(address=part['address']+offset-sym['value'],
                                         end=part['end_address'],section=part['section']))
    else:
        for part in data_attribute_input_spans(context,args,transport_only=True):
            if part['section'] == section and part['offset'] <= offset < part['end']:
                mappings.append(dict(address=part['address']+offset-part['offset'],
                                     end=part['end_address'],section=part['section']))
    # Exact duplicate coordinates from the existing and newly claimed routes
    # describe the same span; differing or multiply-owned coordinates refuse.
    coordinates={(m['address'],m['end'],m['section']) for m in mappings}
    data_attribute_require(len(coordinates)==1,'R32 target needs one actual complete registered input mapping')
    address,end,original_section=next(iter(coordinates))
    return dict(value=address,address=address,end=end,section=original_section,
                symbol_index=symbol['index'],symbol_name=symbol['name'],addend=addend,
                compiler_target_section=section,compiler_target_offset=offset,
                registration='actual_registered_compiler_input_span',pointee_credit=0)


def data_attribute_shared_suffix_hashes():
    """Exact root-authorized immutable mechanical implementation/dependencies."""
    return dict(data_gate='f830adcc692ed038048f4f30341af1082779ed0b1ed3d62eacb74e360a0619a2',
        data_carve='9db48818a0f664d3226e5131b4abd0f7d2e04686831d34cff196788c816325e6',
        predecessor_auditor='e680880ac28a97837890cf07568389c5b89268e50689467ea13c53c3e419a1a3',
        predecessor_review='fcdcdb757c854367fd6e59bbe4ddfe086a6e1ceec4996daade036ed461ff3da5',
        predecessor_origin='2858eb5876bf57ad90c86cec6b9cedd9b5e290364ddaf7317bdf919b2dca4f93',
        tu_edit='d774b4328056cfe31c4318fb5abe590ee1b628a36a8ce13392500075f4abe5f4',
        elfinfo='a6385e9748bb85181af274136e9c75596af3a21fa0d20cae1a82ed1ccbb2097f',
        mechanical_review='c33d5de3b611a3852e168e01986686b8339460981cda1f4479def385c36ed020')


def data_attribute_load_shared_suffix(record):
    """Load only the pinned mechanical code; never mutable installed P tools."""
    import stat
    expected=record.get('auxiliary_suffix_dependencies') or {}
    hashes=data_attribute_shared_suffix_hashes()
    data_attribute_require(set(expected)==set(hashes),'complete explicit shared dependency pins required')
    for key,pin in expected.items():
        path=data_attribute_pin(pin)
        data_attribute_require(pin['sha256']==hashes[key] and not stat.S_IMODE(path.stat().st_mode)&0o222,
                               'shared mechanical dependency is not the sealed approved content')
    origin=json.loads(Path(expected['predecessor_origin']['path']).read_text())
    data_attribute_require(sum(Path(entry.get('backup','/nonexistent')).resolve()==
        Path(expected['predecessor_auditor']['path']).resolve() and entry.get('sha256')==hashes['predecessor_auditor']
        for entry in origin.get('files',[]))==1,'predecessor auditor is not the actual approved snapshot entry')
    review=json.loads(Path(expected['mechanical_review']['path']).read_text())
    data_attribute_require(review.get('verdict')=='MECHANICAL_SOURCE_READY' and
        review.get('final_tool_acceptance') is False and review.get('source_approval') is False and
        review.get('DATA_credit')==0 and review.get('game_function_credit')==0 and
        all(review.get('postimages',{}).get(key,{}).get('sha256')==expected[key]['sha256']
            for key in ('data_carve','data_gate')),'shared mechanical review scope/postimage mismatch')
    prior=json.loads(Path(expected['predecessor_review']['path']).read_text())
    data_attribute_require(prior.get('decision')=='READY_TOOL' and
        prior.get('candidate',{}).get('sha256')==expected['predecessor_auditor']['sha256'] and
        prior.get('grants_data_recovery') is False and prior.get('grants_function_acceptance') is False,
        'shared CPP parser predecessor review mismatch')
    # These modules perform no builds. Explicit bindings prevent their bare
    # imports from selecting a mutable installed auditor or a different parser.
    modules={}
    for key in ('tu_edit','elfinfo','predecessor_auditor','data_carve','data_gate'):
        spec=importlib.util.spec_from_file_location('_data_attribute_shared_'+key,expected[key]['path'])
        module=importlib.util.module_from_spec(spec)
        sys.modules[spec.name]=module
        spec.loader.exec_module(module);modules[key]=module
        if key in ('tu_edit','elfinfo'):sys.modules[key]=module
        if key=='predecessor_auditor':sys.modules['tu_audit']=module
    modules['predecessor_auditor'].tu_edit=modules['tu_edit']
    modules['data_gate'].tu_audit=modules['predecessor_auditor']
    modules['data_gate'].Elf=modules['elfinfo'].Elf
    modules['data_carve'].Elf=modules['elfinfo'].Elf
    data_attribute_require(modules['data_gate'].Elf is modules['elfinfo'].Elf,
                           'shared ELF decoder dependency differs')
    return modules['data_gate'],modules['data_carve'],expected


def data_attribute_existing_C_owner(args,name,section,physical_slot,baseline_inputs):
    """Closed existing-owner provenance, never a new DATA claim or capacity."""
    import stat
    allowed={
        'main/tu169':{'registeredReserveText':(0x4c51a8,0x4c51c0), 'registeredCombatantText':(0x4c51c0,0x4c51d8),
            'combatantText':(0x4c51d8,0x4c51e8), 'dexterityText':(0x4c5250,0x4c5260),
            'etherDefenseText':(0x4c5260,0x4c5270), 'etherAttackText':(0x4c5270,0x4c5280),
            'vitalityText':(0x4c5280,0x4c5290), 'strengthText':(0x4c5290,0x4c52a0)},
        'main/tu170':{'msg_0_0036D8A8':(0x36d8a8,0x36d8d8), 'msg01_1':(0x36d8d8,0x36d8e4),
            'msg01_3':(0x36d8f8,0x36d900), 'msg_6':(0x36d930,0x36d934),
            'msg_7_0036D938':(0x36d938,0x36d940)}}
    data_attribute_require(args.unit=='main' and args.tu_id in allowed and name in allowed[args.tu_id] and
        tuple(physical_slot)==allowed[args.tu_id][name] and
        section==('.rodata' if args.tu_id=='main/tu169' else '.data'),
        'unclaimed auxiliary object outside the exact thirteen approved existing C owners')
    data_attribute_require(isinstance(baseline_inputs,dict) and set(baseline_inputs)=={'manifest','source','registry'},
                           'explicit immutable baseline source/registry/manifest pins required')
    manifest_path=data_attribute_pin(baseline_inputs['manifest'])
    data_attribute_require(baseline_inputs['manifest']['sha256']==
        'c3366d8b564ec449eafdb88bd86799d21c533a600f261266ef200c45f8eb22c7',
        'original ownership manifest differs from approved snapshot')
    manifest=json.loads(manifest_path.read_text())
    data_attribute_require(not stat.S_IMODE(manifest_path.stat().st_mode)&0o222 and
        manifest.get('schema')=='canonical-prepublication-inputs/1' and manifest.get('read_only') is True,
        'original C-ownership snapshot is not the sealed approved pre-campaign manifest')
    source_rel,source_sha={
        'main/tu169':('src/main/menu_1.c','d86ff343f4fd8240d04e49950d635d422d04f43445e918cef36352f5ad6ff630'),
        'main/tu170':('src/main/menu_para_pt_rate_get.c','656a52173a5fbf442f90e3ab60989fb0d900accea47301d2066b2e5aae89ff0e')
        }[args.tu_id]
    pins={}
    for key,relative,digest in (
        ('registry','config/tu/data-carves.json','36904149b40b331b00e097a2f79380e94a7363ce508b234f4d032e55fc8ad63b'),
        ('source',source_rel,source_sha)):
        entries=[e for e in manifest.get('entries',[]) if e.get('path')==relative and e.get('state')=='PRESENT']
        data_attribute_require(len(entries)==1 and entries[0].get('sha256')==digest,
                               'missing/wrong original canonical source or registry manifest entry')
        path=data_attribute_pin(baseline_inputs[key],entries[0]['backup_path'])
        data_attribute_require(baseline_inputs[key]['sha256']==digest,'historical source/registry input hash differs')
        data_attribute_require(not stat.S_IMODE(path.stat().st_mode)&0o222,'original C-ownership input is not sealed')
        pins[key]=dict(path=str(path),sha256=digest)
    registry=json.loads(Path(pins['registry']['path']).read_text())
    rows=[r for r in registry.get('tus',{}).get(args.tu_id,{}).get(section,[]) if
          r.get('c_owned_symbols')==[name] and
          [int(v,0) if isinstance(v,str) else int(v) for v in r.get('range',[])]==list(physical_slot) and
          [int(v,0) if isinstance(v,str) else int(v) for v in r.get('generated',[])]==list(physical_slot)]
    data_attribute_require(len(rows)==1,'missing/wrong historical canonical C storage owner')
    spans=[p for p in rows[0].get('c_input_spans',[]) if p.get('symbols')==[name] and
           p.get('credit')!='uncredited_dependency' and
           [int(v,0) if isinstance(v,str) else int(v) for v in p.get('range',[])]==list(physical_slot)]
    data_attribute_require(len(spans)==1 and len(spans[0].get('object_range',[]))==2 and
        spans[0]['object_range'][1]-spans[0]['object_range'][0]==physical_slot[1]-physical_slot[0],
        'historical canonical compiler ownership span is missing/partial')
    source=Path(pins['source']['path']).read_text(encoding='utf-8',errors='surrogateescape')
    declarations=[i.text(source) for i in tu_edit.parse(source).free_items() if i.kind=='decl' and
        not i.conditional and ('ordinary',name) in tu_edit.decl_keys(i.text(source)) and
        '=' in tu_edit.c_tokens(i.text(source)) and
        not any(t in tu_edit.c_tokens(i.text(source)) for t in ('extern','typedef','__asm__','asm'))]
    data_attribute_require(len(declarations)==1,'no actual original canonical C definition for existing owner')
    pins['manifest']=dict(path=str(manifest_path),sha256=sha256_file(manifest_path))
    return dict(evidence='auxiliary_existing_C_owner_without_new_claim',baseline_inputs=pins,
                baseline_C_storage_range=list(physical_slot),canonical_definition_tokens=tu_edit.c_tokens(declarations[0]),
                original_capacity_proved=False,grants_new_data_recovery=False,grants_function_acceptance=False)


def data_attribute_original_reference_exclusion(record,original,Elf,interval):
    """No indexed witness, explicitly not a global absence/use/capacity proof."""
    import ast
    import bisect
    import functools
    import stat
    pins=record.get('original_reference_index') or {}
    data_attribute_require(set(pins)=={'implementation','review'},'exact reviewed reference-index pins required')
    tool=data_attribute_pin(pins['implementation']);review=data_attribute_pin(pins['review'])
    data_attribute_require(pins['implementation']['sha256']==
        'd251f8c9cbcd644e1b88bb128c3fbaffcbc560eaae5044483ef0a7a4e4b05444' and
        pins['review']['sha256']=='f87d80d5929f404e7fee3b7a60670d5f762fe4e1c98d7a724e471820384861b2' and
        all(not stat.S_IMODE(p.stat().st_mode)&0o222 for p in (tool,review)),
        'original reference index differs from approved immutable implementation/review')
    tree=ast.parse(tool.read_text());names={'parsed_elf','original_reference_addresses','original_references'}
    nodes=[node for node in tree.body if isinstance(node,ast.FunctionDef) and node.name in names]
    data_attribute_require(len(nodes)==3 and {n.name for n in nodes}==names,'approved reference-index functions missing/duplicated')
    namespace=dict(Elf=Elf,lru_cache=functools.lru_cache,struct=struct,bisect_left=bisect.bisect_left)
    # Execute the exact reviewed function nodes, without importing unrelated
    # native-reference code or its mutable installed-tool imports.
    exec(compile(ast.Module(body=nodes,type_ignores=[]),str(tool),'exec'),namespace)
    native=Elf(original.data)
    addresses=namespace['original_reference_addresses'](original.data)
    hits=[v for v in addresses if interval[0]<=v<interval[1]]
    data_attribute_require(not hits and not namespace['original_references'](native,*interval),
                           'indexed original interior reference targets unknown scaffold suffix')
    return dict(range=interval,indexed_interior_hits=hits,index_size=len(addresses),pins=pins,
        scope='original relocations/relocation-referenced symbol VAs, aligned stored words, GP uses, LUI+low candidates within128 bytes',
        global_reference_absence_proved=False,unused_bytes_proved=False,
        original_capacity_proved=False,original_input_section_alignment=None)


def data_attribute_character_composition_scope(args,record):
    """The exact eighteen reviewed character prefixes; never scalar views."""
    allowed={
        ('main/tu155','EWDefaultResourcePath'):(0x4c23c0,0x4c23d7,0x4c23d8),
        ('main/tu169','registeredReserveText'):(0x4c51a8,0x4c51bb,0x4c51c0),
        ('main/tu169','registeredCombatantText'):(0x4c51c0,0x4c51d5,0x4c51d8),
        ('main/tu169','combatantText'):(0x4c51d8,0x4c51e2,0x4c51e8),
        ('main/tu169','dexterityText'):(0x4c5250,0x4c525a,0x4c5260),
        ('main/tu169','etherDefenseText'):(0x4c5260,0x4c526e,0x4c5270),
        ('main/tu169','etherAttackText'):(0x4c5270,0x4c527d,0x4c5280),
        ('main/tu169','vitalityText'):(0x4c5280,0x4c5289,0x4c5290),
        ('main/tu169','strengthText'):(0x4c5290,0x4c5299,0x4c52a0),
        ('main/tu169','vx20000Name'):(0x4c52a0,0x4c52a9,0x4c52b0),
        ('main/tu169','vx10000Name'):(0x4c52b0,0x4c52b9,0x4c52c0),
        ('main/tu169','catalinaText'):(0x4c52c0,0x4c52c9,0x4c52d0),
        ('main/tu169','cassiriaText'):(0x4c52d0,0x4c52db,0x4c52e0),
        ('main/tu169','blankNameText'):(0x4c52e0,0x4c52eb,0x4c52f0),
        ('main/tu169','ziggurat8Name'):(0x4c52f0,0x4c52fb,0x4c5300),
        ('main/tu169','gunoMotionName'):(0x4c54d8,0x4c54e1,0x4c54f0),
        ('main/tu174','etherSetSearchText'):(0x4c82d8,0x4c82ec,0x4c82f0),
        ('main/tu178','skillSetSearchText'):(0x4c9940,0x4c9953,0x4c9960)}
    key=(record.get('tu'),record.get('name'))
    data_attribute_require(args.unit=='main' and record.get('unit')=='main' and key in allowed and
        args.tu_id==key[0] and record.get('semantic_range')==list(allowed[key][:2]) and
        record.get('original_physical_slot')==[allowed[key][0],allowed[key][2]],
        'computed scaffold composition outside the exact eighteen reviewed character prefixes')


def data_attribute_composition_inputs(document,args,record):
    """Unsigned transport supplies mechanical pins, never semantic fields."""
    data_attribute_character_composition_scope(args,record)
    entries=document.get('scaffold_suffix_compositions')
    data_attribute_require(isinstance(entries,list) and len(entries)==18,
                           'complete closed eighteen-entry mechanical composition input required')
    seen=set();selected=[]
    keys={'unit','tu','name','core_record_sha256','scaffold_suffix_fragment','auxiliary_suffix_dependencies'}
    cores={(r['unit'],r['tu'],r['name']):r for r in document['records']}
    for entry in entries:
        data_attribute_require(isinstance(entry,dict) and set(entry)==keys,
                               'mechanical transport may not override semantic/type/source fields')
        identity=(entry['unit'],entry['tu'],entry['name'])
        data_attribute_require(identity not in seen and identity in cores,
                               'duplicate/unknown mechanical composition owner')
        core=cores[identity]
        local=type('CompositionScope',(),dict(unit=entry['unit'],tu_id=entry['tu']))()
        data_attribute_character_composition_scope(local,core)
        data_attribute_require(entry['core_record_sha256']==data_attribute_digest(
            {k:v for k,v in core.items() if k!='independent_review'}),
            'mechanical composition does not bind the authenticated semantic core')
        seen.add(identity)
        if identity==(record['unit'],record['tu'],record['name']):selected.append(entry)
    data_attribute_require(len(selected)==1,'one exact mechanical composition required for this semantic core')
    result=dict(record)
    for key in ('scaffold_suffix_fragment','auxiliary_suffix_dependencies'):
        data_attribute_require(key not in record or record[key]==selected[0][key],
                               'transport contradicts an already authenticated mechanical dependency')
        result[key]=selected[0][key]
    return result


def data_attribute_auxiliary_scaffold_fragment(context,args,record,section,semantic,slot,original):
    """Actual separate native fragment, outside the selected compiler prefix."""
    pins=record.get('scaffold_suffix_fragment') or {}
    data_attribute_character_composition_scope(args,record)
    data_attribute_require(set(pins)=={'source','object','carved_manifest','build_ninja'},
                           'actual scaffold source/object/manifest/build-edge pins required')
    manifest_path=data_attribute_pin(pins['carved_manifest'],context['unit_dir']/'tu-manifest.carved.json')
    data_attribute_gate_bound_pin(context,manifest_path)
    manifest=json.loads(manifest_path.read_text())
    entries=[t for t in manifest.get('tus',[]) if t.get('id')==args.tu_id and t.get('unit')==args.unit]
    data_attribute_require(len(entries)==1,'scaffold fragment TU missing/duplicated in actual carved manifest')
    pieces=[p for p in entries[0].get('sections',{}).get(section,{}).get('pieces',[]) if
        int(p.get('start','0'),0)==semantic[1] and int(p.get('end','0'),0)==slot[1] and p.get('c_split') is False]
    data_attribute_require(len(pieces)==1,'unknown suffix is not one actual non-C native fragment')
    piece=pieces[0];source=data_attribute_pin(pins['source'],context['unit_dir']/piece['file'])
    obj_path=data_attribute_pin(pins['object'],context['unit_dir']/'build/data'/(piece['name']+section+'.o'))
    for path in (source,obj_path):
        relative=path.relative_to(ROOT).as_posix()
        if relative in context['candidate'].get('files',{}):data_attribute_gate_bound_pin(context,path)
    data_attribute_pin(pins['build_ninja'],context['unit_dir']/'build.ninja')
    document=_ninja_document(context['unit_dir'])
    target=os.path.relpath(obj_path,context['unit_dir']);input_path=os.path.relpath(source,context['unit_dir'])
    edge=(document or {}).get('edges',{}).get(target) or {}
    data_attribute_require(edge.get('rule')=='as' and edge.get('inputs')==[input_path] and
        (document.get('rules',{}).get('as') or {}).get('command')=='$as $asflags -I . -I include -o $out $in',
        'scaffold fragment lacks its exact ordinary native assembler edge')
    ninja=(context['unit_dir']/'build.ninja').read_text()
    assemblers=re.findall(r'^as\s*=\s*timeout -k 5 30 (\S+)\s*$',ninja,re.M)
    flags=re.findall(r'^asflags\s*=\s*(.+)$',ninja,re.M)
    expected_flags=['-EL','-march=r5900','-mabi=eabi','-mgp64','-G0','-no-pad-sections','-mno-fix-r5900']
    data_attribute_require(len(assemblers)==len(flags)==1 and flags[0].split()==expected_flags,
                           'scaffold assembler command/flags differ from the ordinary current recipe')
    assembler=Path(assemblers[0]);identity_path=ROOT/'config/toolchain-identity.json'
    identity=json.loads(identity_path.read_text()).get('tools',{}).get('ps2dev-binutils',{})
    native_assembler_sha='6c4ee9ee347c647b2e034ba77009901bd04b6ba20c26cfd0d833a1c86e08d037'
    data_attribute_require(assembler.is_absolute() and assembler.is_file() and
        assembler.resolve()==(ROOT/'.work/toolchains'/identity.get('dir','')/'mips64r5900el-ps2-elf-as').resolve() and
        identity.get('files',{}).get('mips64r5900el-ps2-elf-as')==native_assembler_sha and
        sha256_file(assembler)==native_assembler_sha,
        'scaffold assembler differs from the actual approved native-data assembler identity')
    with tempfile.TemporaryDirectory(prefix='attribute-scaffold-') as temporary:
        fresh=Path(temporary)/'fragment.o'
        status,log=run([assembler,*expected_flags,'-I','.','-I','include','-o',fresh,input_path],context['unit_dir'])
        data_attribute_require(status==0 and fresh.is_file() and sha256_file(fresh)==sha256_file(obj_path),
                               'independent ordinary scaffold assembly differs from actual fragment object: '+log[:200])
    tails=[t for t in piece.get('uncredited_scaffold_suffixes',[]) if t.get('owner')==record['name'] and
        t.get('range')==[semantic[1],slot[1]] and t.get('compiler_range')==semantic and
        t.get('original_item_range')==slot and t.get('credited') is False and
        t.get('original_capacity_proved') is False and t.get('alignment_proved') is False]
    data_attribute_require(len(tails)==1,'actual scaffold fragment has no truthful zero-credit suffix disposition')
    payload,_metadata=data_attribute_image_bytes(original,semantic[1],slot[1]-semantic[1])
    data_attribute_require(payload==bytes(len(payload)),'unknown auxiliary suffix is not actual original zero storage')
    obj=RelObject(obj_path.read_bytes())
    metadata=[s for s in obj.sections if s['flags']&2 and s['size'] and s['name']=='.MIPS.abiflags']
    data_attribute_require(len(metadata)==1 and metadata[0]['type']==0x7000002a and
        metadata[0]['flags']==2 and metadata[0]['size']==24 and metadata[0]['align']==8 and
        metadata[0]['entsize']==24 and obj.bytes_of(metadata[0])==
            struct.pack('<HBBBBBBIIII',0,3,0,2,2,0,1,6,0,1,0) and
        all(s['type']==STT_SECTION and s['binding']==0 and s['value']==s['size']==0 and
            s['name']=='.MIPS.abiflags' for s in obj.symbols if s['shndx']==metadata[0]['index']) and
        not any(s['type'] in (4,9) and s['info']==metadata[0]['index'] for s in obj.sections) and
        not any(s['name']=='.MIPS.abiflags' and s['size']
            for s in context['linked'].sections),
        'scaffold assembler ABI metadata differs from the ordinary unlinked metadata section')
    sections=[s for s in obj.sections if s['flags']&2 and s['size'] and s is not metadata[0]]
    data_attribute_require(len(sections)==1 and sections[0]['name']==section and sections[0]['type']==1 and
        sections[0]['size']==len(payload) and obj.bytes_of(sections[0])==payload and
        not any(s['type'] in (4,9) and s['size'] for s in obj.sections),
        'actual assembled scaffold extent/bytes/relocations differ from original suffix')
    serialization=tails[0].get('serialization') or {}
    data_attribute_require(serialization.get('kind')=='original_zero_scaffold_serialization' and
        serialization.get('range')==[semantic[1],slot[1]] and serialization.get('credited') is False and
        serialization.get('source_capacity_proved') is False and
        serialization.get('original_elf_sha256')==context['hashes']['original_sha256'] and
        serialization.get('original_bytes_sha256')==hashlib.sha256(payload).hexdigest(),
        'current generator suffix receipt differs from actual original zero storage')
    shared_gate,shared_carve,_dependencies=data_attribute_load_shared_suffix(record)
    native=shared_carve.tu_context(ROOT,args.unit,context['unit_dir'],args.tu_id)
    original_pieces=[p for p in native['pieces'].get(section,[]) if p['start']<=slot[0] and slot[1]<=p['end']]
    data_attribute_require(len(original_pieces)==1,'one actual original source piece required for suffix serialization')
    original_source=Path(native['path_of'](original_pieces[0],section))
    data_attribute_gate_bound_pin(context,original_source)
    header,native_items=shared_carve.piece_items(original_source,original_pieces[0]['start'],original_pieces[0]['end'])
    native_items=[i for i in native_items if i['start']==slot[0] and i['end']==slot[1] and i['name']==record['name']]
    data_attribute_require(len(native_items)==1,'native registered item differs from the exact character layout slot')
    projection_context=dict(unit_dir=str(context['unit_dir']),section=section,inputs={
        'source':record['inputs']['source'],'preprocessed':record['inputs']['preprocessed'],
        'assembly':record['inputs']['assembly'],'raw':record['inputs']['raw'],
        'original':dict(path=str(native['orig']),sha256=context['hashes']['original_sha256'])})
    split=shared_carve.split_initialized_items(native_items,[semantic[1]],{semantic[1]},native['name'],
                                             character_context=projection_context)
    suffix=[i for i in split if i['start']==semantic[1] and i['end']==slot[1]]
    data_attribute_require(len(suffix)==1,'reviewed splitter did not reproduce the exact native suffix')
    regenerated=shared_carve.serialize_unknown_zero_suffix(suffix[0],projection_context)
    data_attribute_require(regenerated.get('uncredited_scaffold_suffix',{}).get('serialization')==serialization,
                           'actual original directive/byte serialization proof differs from pinned generator metadata')
    generated_lines=header+regenerated['block']['lines']
    while generated_lines and not generated_lines[-1].strip():generated_lines.pop()
    data_attribute_require(source.read_text()=='\n'.join(generated_lines)+'\n',
                           'actual suffix source is not the closed unchanged original zero-fragment generator output')
    return dict(pins=pins,range=[semantic[1],slot[1]],separate_scaffold_fragment=True,
        selected_C_prefix_contains_suffix=False,object_section=section,
        object_alignment=sections[0]['align'],size=len(payload),payload_sha256=hashlib.sha256(payload).hexdigest(),
        assembler_metadata=dict(section=metadata[0],payload_sha256=hashlib.sha256(obj.bytes_of(metadata[0])).hexdigest(),
                                present_in_linked_image=False,credited_bytes=0),
        ordinary_assembler_edge=edge,original_input_section_alignment=None,original_capacity_proved=False,
        assembler=dict(path=str(assembler),sha256=sha256_file(assembler),flags=expected_flags,
                       identity_registry=dict(path=str(identity_path),sha256=sha256_file(identity_path)),
                       independently_reassembled_object_sha256=sha256_file(obj_path)),
        regenerated_original_serialization=serialization,
        credited=False,credited_bytes=0)


def verified_relocated_data_attribute_object(context,args,tu,item,name,section,obj,elf):
    """Complete positive original char-pointer OBJECT with exact R32 transport.

    Raw, selected and linked symbols are facts of those objects. A registered
    dotted original identity remains distinct from its C coordinate name.
    Existing compiler-owned spans get no new recovery credit from this receipt.
    """
    if context is None:
        return None
    try:
        size=data_attribute_declaration(context,tu,item,name,pointer_table=True)
        initializer_names=data_attribute_pointer_initializers(tu,item,size//4)
        data_attribute_require(section=='.data','pointer-table route only proves original .data')
        symbols=[]
        for image in (obj,context['raw'],context['selected']):
            found=[s for s in image.symbols if s['name']==name and s['shndx']!=0]
            data_attribute_require(len(found)==1,'one compiler/selected named OBJECT required')
            symbol=found[0]
            data_attribute_require(symbol['type']==STT_OBJECT and symbol['binding']==0 and symbol['size']==size,
                                   'complete LOCAL compiler pointer-table identity')
            symbols.append(symbol)
        after,raw_symbol,selected_symbol=symbols
        data_attribute_require(after==raw_symbol and raw_symbol['section']==section,'fresh/raw compiler identity drift')
        parts=[p for p in data_attribute_input_spans(context,args) if p['symbols']==[name] and
               p['section']==section and p['offset']==raw_symbol['value'] and p['end']-p['offset']==size]
        data_attribute_require(len(parts)==1,'one exact complete registered owner span required')
        part=parts[0];address=part['address']
        data_attribute_require(part['run'].get('c_owned_symbols')==[name] and
            [int(v,0) if isinstance(v,str) else int(v) for v in part['run'].get('range',[])]==
            [address,address+size] and
            [int(v,0) if isinstance(v,str) else int(v) for v in part['run'].get('generated',[])]==[address,address+size],
            'full registered run/generated range must equal original OBJECT')
        originals=[s for s in elf.symbols if s['type']==STT_OBJECT and s['binding']==0 and
                   int(s['va'],16)==address and s['size']==size and s.get('section')==section]
        data_attribute_require(len(originals)==1,'one complete original LOCAL OBJECT identity required')
        original=originals[0]
        # The registry's textual hexadecimal case is not an identity distinction.
        aliases=[a for a in part['run'].get('c_storage_aliases',[]) if a.get('original_name')==original['name'] and
                 int(a.get('address','0'),0)==address and a.get('storage_owner',{}).get('name')==name and
                 int(a.get('storage_owner',{}).get('address','0'),0)==address and
                 a.get('storage_owner',{}).get('size')==size]
        data_attribute_require(name==original['name'] or len(aliases)==1,'exact original-to-C alias registration required')
        spans=[s for s in context['row'].get('verified_location_spans',[]) if name in s.get('names',[]) or
               name in s.get('object_symbols',[])]
        data_attribute_require(len(spans)<=1,'duplicate credited owner spans')
        baseline_owner=None
        if spans:
            span=spans[0];expected=dict(name=original['name'],address=f'0x{address:08X}',binding='LOCAL',type='OBJECT',size=size)
            data_attribute_require(span.get('names')==[name] and span.get('object_symbols')==[name] and
                span.get('range')==[address,address+size] and span.get('split_range')==[address,address+size] and
                span.get('length')==size and span.get('section')==section and
                span.get('original_identities')==[expected] and
                span.get('object_sha256')==context['hashes']['linked_data_object_sha256'] and
                span.get('c_object_section')==selected_symbol['section'] and
                span.get('c_object_offset')==selected_symbol['value'] and not any(span.get(k) for k in
                ('support_only','storage_only','storage_owner')) and span.get('credited') is not False,
                'new claimed complete span identity')
            data_attribute_require(span.get('compiler_input_span')==dict(section=section,offset=raw_symbol['value'],
                section_size=context['raw'].section(section)['size']) and
                span.get('c_object_section_size')==context['selected'].section(selected_symbol['section'])['size'] and
                span.get('c_object_section_alignment')==context['selected'].section(selected_symbol['section'])['align'],
                'actual raw/selected section sizes/offsets/alignment differ from gate receipt')
            for locations in (context['row'].get('verified_locations',[]),
                              context['candidate'].get('recovered_location_candidates',[]),
                              context['control'].get('identities',[])):
                matches=[r for r in locations if r.get('unit')==args.unit and r.get('tu')==args.tu_id and
                         r.get('section')==section and int(r.get('address','0'),0)==address and r.get('names')==[name]]
                data_attribute_require(len(matches)==1,'new claimed object absent/duplicated in actual ownership receipts')
        else:
            # This is computed from the actual existing registry and objects,
            # never inserted into the passing gate's new-claim span list.
            data_attribute_require(len(aliases)==1,'unclaimed object needs explicit existing C-owner alias')
            baseline_owner=data_attribute_existing_C_owner(args,name,section,[address,address+size],
                data_attribute_proof_document(context,args).get('existing_c_owner_baselines',{}).get(args.tu_id))
            data_attribute_require(not any(name in row.get('names',[]) for rows in
                (context['row'].get('verified_locations',[]),context['candidate'].get('recovered_location_candidates',[]),
                 context['control'].get('identities',[])) for row in rows if row.get('unit')==args.unit and
                 row.get('tu')==args.tu_id),'auxiliary existing pointer table cannot be a fabricated new claim')
        linked=[s for s in context['linked'].symbols if s['name']==name and int(s['va'],16)==address and
                s.get('section')==section and s['type']==STT_OBJECT and s['binding']==0 and s['size']==size]
        data_attribute_require(len(linked)==1,'linked compiler object identity/extent mismatch')
        data_attribute_emission(context,tu,name,size,section)
        payloads=[];relocation_sets=[]
        for image,symbol in ((context['raw'],raw_symbol),(context['selected'],selected_symbol)):
            csec=image.section(symbol['section'])
            data_attribute_require(csec and csec['type']==1 and symbol['value']+size<=csec['size'],
                                   'complete initialized compiler section extent')
            payloads.append(image.bytes_of(csec)[symbol['value']:symbol['value']+size])
            relocation_sets.append(data_attribute_rel_entries(image,csec,symbol['value'],size))
        data_attribute_require(payloads[0]==payloads[1] and
                               [r['relative'] for r in relocation_sets[0]]==list(range(0,size,4)) and
                               [r['relative'] for r in relocation_sets[0]]==[r['relative'] for r in relocation_sets[1]],
                               'every actual symbolic pointer slot needs one exact raw/selected R32')
        expected=bytearray(payloads[0]);evidence=[]
        for first,second in zip(*relocation_sets):
            initializer_name=initializer_names[first['relative']//4]
            pointee_declarations=data_attribute_character_pointee(context,initializer_name)
            initializer_symbols=[s for s in context['raw'].symbols if s['name']==initializer_name]
            data_attribute_require(len(initializer_symbols)==1,'source pointee must have one actual raw symbol')
            declared=initializer_symbols[0];actual=first['symbol']
            if declared['shndx']==0:
                data_attribute_require(declared['index']==actual['index'] and first['addend']==0,
                                       'external source pointee differs from actual R32 symbol/addend')
            else:
                data_attribute_require(declared['shndx']==actual['shndx'] and declared['value']==
                    ((actual['value']+first['addend'])&0xffffffff),
                    'source pointee coordinate differs from actual R32 symbol/addend')
            left=data_attribute_resolve_r32(context,args,context['raw'],first,elf)
            right=data_attribute_resolve_r32(context,args,context['selected'],second,elf,selected=True)
            data_attribute_require(first['addend']==second['addend'] and left['value']==right['value'],
                                   'raw/selected R32 actual symbol/addend resolution differs')
            struct.pack_into('<I',expected,first['relative'],left['value']&0xffffffff)
            evidence.append(dict(offset=first['relative'],source_pointee=initializer_name,
                                 source_character_array_declarations=pointee_declarations,raw=first,selected=second,target=left,
                                 selected_target=right,expected_retail_word=f'0x{left["value"]:08x}',pointee_credit=0))
        original_bytes=elf.data[original['file_offset']:original['file_offset']+size]
        linked_bytes=context['linked'].data[linked[0]['file_offset']:linked[0]['file_offset']+size]
        data_attribute_require(len(original_bytes)==size and bytes(expected)==original_bytes==linked_bytes,
                               'relocated words or literal nonrelocation bytes differ from retail')
        return dict(name=name,original_name=original['name'],section=section,address=original['va'],size=size,
                    original_identity=dict(type=STT_OBJECT,binding=0,size=size,name=original['name']),
                    compiler_identity=raw_symbol,selected_identity=selected_symbol,relocations=evidence,
                    payload_sha256=hashlib.sha256(original_bytes).hexdigest(),
                    evidence='complete_original_positive_OBJECT_R_MIPS_32',existing_C_owner_without_new_claim=not bool(spans),
                    existing_C_owner_provenance=baseline_owner,
                    grants_new_data_recovery=False,grants_function_acceptance=False,
                    compiler_section={k:context['raw'].section(section)[k] for k in ('name','flags','size','align')},
                    selected_section={k:context['selected'].section(selected_symbol['section'])[k]
                                      for k in ('name','flags','size','align')},
                    extension_input_pins=context.get('extension_pins',{}),**context['hashes'])
    except (AuditError,OSError,ValueError,KeyError,TypeError,tu_edit.ParseError,struct.error):
        return None


def data_attribute_image_bytes(image,address,size):
    sections=[s for s in image.sections if s['flags']&2 and s['type']==1 and
              int(s['va'],16)<=address and address+size<=int(s['va'],16)+s['size']]
    data_attribute_require(len(sections)==1,'one initialized retail/linked physical section required')
    section=sections[0];offset=section['file_offset']+address-int(section['va'],16)
    payload=image.data[offset:offset+size]
    data_attribute_require(len(payload)==size,'truncated original/linked payload')
    loads=[p for p in image.program_headers if p['type']==1 and int(p['va'],16)<=address and
           address+size<=int(p['va'],16)+p['file_size']]
    data_attribute_require(len(loads)==1,'one actual file-backed PT_LOAD required')
    return payload,dict(name=section['name'],flags=section['flags'],alignment=section['alignment'],
                        load_flags=loads[0]['flags'])


def data_attribute_constant_reads(elf,original):
    """Conservative straight-line R5900 address/load witness, not decompilation.

    Calls and control-flow joins discard inferred addresses. Unsupported
    writes also discard the destination, so a guessed register state cannot
    establish a copy bound. Only ordinary concrete loads count as reads.
    """
    payload=elf.data[original['file_offset']:original['file_offset']+original['size']]
    registers={0:0};reads=[]
    address=int(original['va'],16)
    leaders={0};delay_slots=set()
    words=[struct.unpack_from('<I',payload,i)[0] for i in range(0,len(payload)-3,4)]
    for index,word in enumerate(words):
        offset=index*4;op=word>>26;rs=(word>>21)&31;fun=word&63
        conditional=(op in (1,4,5,6,7,0x14,0x15,0x16,0x17) or (op in (0x10,0x11,0x12) and rs==8))
        control=conditional or op in (2,3) or (op==0 and fun in (8,9))
        if op==0 and fun==8 and rs!=31:
            # An unenumerated indirect local target could enter any inferred
            # block. This closed copy witness does not prove such a CFG.
            return []
        if control:
            delay_slots.add(offset+4);leaders.add(offset+8)
            if conditional:
                immediate=word&65535
                immediate=immediate-65536 if immediate&32768 else immediate
                target=offset+4+(immediate<<2)
                if 0<=target<len(payload):
                    leaders.add(target)
            elif op in (2,3):
                target=(((address+offset+4)&0xf0000000)|((word&0x3ffffff)<<2))-address
                if 0<=target<len(payload):
                    leaders.add(target)
    for offset in range(0,len(payload)-3,4):
        if offset in leaders:
            registers={0:0}
        if offset in delay_slots:
            # Branch-likely may annul this instruction. Discard its effects
            # and reads for this bounded proof, even for ordinary branches.
            registers={0:0}
            continue
        word=struct.unpack_from('<I',payload,offset)[0]
        op,rs,rt,rd,fun=word>>26,(word>>21)&31,(word>>16)&31,(word>>11)&31,word&63
        imm=word&65535;signed=imm-65536 if imm&32768 else imm
        old=dict(registers);is_control=False
        if op in (0x20,0x24,0x21,0x25,0x23,0x27,0x31,0x37,0x35,0x1e):
            width={0x20:1,0x24:1,0x21:2,0x25:2,0x23:4,0x27:4,0x31:4,0x37:8,0x35:8,0x1e:16}[op]
            if rs in old:
                lo=(old[rs]+signed)&0xffffffff
                reads.append(dict(instruction_address=hex(address+offset),word=f'0x{word:08x}',
                                  range=[lo,lo+width],width=width,base_register=rs,offset=signed))
            if op not in (0x31,0x35):
                registers.pop(rt,None)
        elif op==15:
            registers[rt]=(imm<<16)&0xffffffff
        elif op in (9,25,13):
            if rs in old:
                registers[rt]=((old[rs]|imm) if op==13 else old[rs]+signed)&0xffffffff
            else:
                registers.pop(rt,None)
        elif op==0 and fun in (0x21,0x2d,0x25) and rs in old and rt in old:
            registers[rd]=((old[rs]|old[rt]) if fun==0x25 else old[rs]+old[rt])&0xffffffff
        elif op==0 and word==0:
            pass
        elif op in (2,3,1,4,5,6,7,0x14,0x15,0x16,0x17) or (op==0 and fun in (8,9)):
            is_control=True
        elif op in (0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x3f,0x39,0x3d,0x1f):
            pass
        elif op==0:
            registers.pop(rd,None)
        elif op in (0x10,0x11,0x12):
            # COP1 can write a GPR only through mfc1/cfc1.
            if rs in (0,1,2,3):
                registers.pop(rt,None)
            elif rs==8:
                is_control=True
        elif op in (8,10,11,12,14,0x1a,0x1b,0x22,0x26):
            registers.pop(rt,None)
        else:
            # Unknown ISA families may write a destination in a different
            # field (e.g. EE MMI). Never retain an inferred address through it.
            registers={0:0}
        registers[0]=0
        if is_control:
            registers={0:0}
    return reads


def data_attribute_r26_call(raw,caller,offset,callee):
    """One actual named or same-section R26 call operand, before linking."""
    data_attribute_require(isinstance(offset,int) and
        caller['value']<=offset and offset+4<=caller['value']+caller['size'] and offset%4==0 and
        0<caller['shndx']<len(raw.sections) and raw.sections[caller['shndx']]['type']==1 and
        raw.sections[caller['shndx']]['flags']&4 and offset+4<=raw.sections[caller['shndx']]['size'],
        'call site outside actual emitted consumer')
    symtabs=[s for s in raw.sections if s['type']==2]
    data_attribute_require(len(symtabs)==1 and symtabs[0]['entsize'] in (0,16) and
        symtabs[0]['size']==len(raw.symbols)*16 and
        0<symtabs[0]['link']<len(raw.sections) and raw.sections[symtabs[0]['link']]['type']==3,
        'call relocation needs one valid actual symbol/string table')
    entries=[]
    for section in raw.sections:
        if section['type']==9 and section['info']==caller['shndx']:
            data_attribute_require(section['entsize'] in (0,8) and section['size']%8==0,
                                   'malformed call relocation section')
            data_attribute_require(section['link']==symtabs[0]['index'],
                                   'call relocation links a different symbol table')
            for index in range(section['size']//8):
                at,info=struct.unpack_from('<II',raw.data,section['offset']+index*8)
                if at==offset:
                    data_attribute_require(info>>8<len(raw.symbols),'call symbol index outside table')
                    entries.append((info&255,raw.symbols[info>>8]))
    data_attribute_require(len(entries)==1 and entries[0][0]==4,'call needs one actual R_MIPS_26')
    symbol=entries[0][1]
    word=struct.unpack_from('<I',raw.data,raw.sections[caller['shndx']]['offset']+offset)[0]
    addend=(word&0x3ffffff)<<2
    data_attribute_require(word>>26 in (2,3),'R26 operand is not an actual jump/call instruction')
    if symbol['type']==STT_SECTION:
        data_attribute_require(symbol['shndx']==caller['shndx'] and symbol['value']==0 and symbol['binding']==0,
                               'same-TU section call uses a different or nonlocal text section')
        targets=[s for s in raw.functions() if s['name']==callee and
                 s['shndx']==symbol['shndx'] and s['value']==symbol['value']+addend and s['size']>0]
        data_attribute_require(len(targets)==1,'section-symbol R26 does not resolve to this exact emitted callee')
        operand=dict(kind='actual_section_symbol_and_instruction_addend',addend=addend,
                     callee_section=targets[0]['section'],callee_offset=targets[0]['value'])
    else:
        data_attribute_require(symbol['name']==callee and addend==0 and
            ((symbol['shndx']==0 and symbol['type'] in (STT_NOTYPE,STT_FUNC) and
              symbol['value']==0 and symbol['size']==0 and symbol['binding']==1) or
             (symbol['shndx']!=0 and symbol['type']==STT_FUNC and symbol['size']>0)),
            'named R26 operand does not target the exact callee without an offset')
        operand=dict(kind='actual_named_callee',addend=addend,
                     callee_section=symbol['section'],callee_offset=symbol['value'])
    return dict(relocation=dict(type=4,symbol_index=symbol['index'],name=symbol['name']),raw_operand=operand)


def data_attribute_consumer_binding(context,args,record,consumer,elf):
    """Re-read a reviewed consumer's current compiler and original-byte facts."""
    from tools.tu.elfinfo import Elf
    from tools.tu.data_gate import storage_relocations
    function=consumer.get('function');tu_id=consumer.get('tu')
    facts=json.loads((ROOT/'config/tu-build.json').read_text())
    units=[t for t in facts.get('tus',[]) if t['id']==tu_id and t.get('unit')==args.unit and t.get('in_scope')]
    data_attribute_require(len(units)==1,'consumer TU is not one actual in-scope source owner')
    source_path=ROOT/units[0]['path'];raw_path=context['unit_dir']/'build/c'/(source_path.stem+'.o')
    expected=dict(source=source_path,preprocessed=Path(str(raw_path)+'.i'),
                  assembly=Path(str(raw_path)+'.s'),raw=raw_path)
    pins=consumer.get('inputs') or {}
    data_attribute_require(set(pins) in (set(expected)|{'audit','audit_tool'},
                                        set(expected)|{'audit','audit_tool','audit_tool_review','audit_tool_origin'}),
                           'complete actual consumer compiler/audit pins required')
    for key,path in expected.items():
        data_attribute_pin(pins[key],path)
    audit_path=data_attribute_pin(pins['audit'])
    receipt=json.loads(audit_path.read_text())
    data_attribute_require(receipt.get('schema')==SCHEMA and receipt.get('tu_id')==tu_id and
        receipt.get('unit')==args.unit and receipt.get('inputs',{}).get('tu_c',{}).get('sha256')==sha256_file(source_path) and
        receipt.get('inputs',{}).get('original',{}).get('sha256')==context['hashes']['original_sha256'] and
        receipt.get('contract_relaxed')==[],'stale/relaxed/foreign consumer compiler provenance receipt')
    audit_tool=data_attribute_pin(pins['audit_tool'])
    allowed_auditors={'e680880ac28a97837890cf07568389c5b89268e50689467ea13c53c3e419a1a3',
                      sha256_file(_AUDIT_PATH)}
    data_attribute_require(receipt.get('tool',{}).get('sha256')==sha256_file(audit_tool) and
                           sha256_file(audit_tool) in allowed_auditors,
                           'consumer receipt did not use an actual independently reviewed compiler-provenance auditor')
    if sha256_file(audit_tool)=='e680880ac28a97837890cf07568389c5b89268e50689467ea13c53c3e419a1a3':
        import stat
        data_attribute_require(not stat.S_IMODE(audit_tool.stat().st_mode)&0o222,
            'predecessor provenance auditor is not the exact approved readonly snapshot')
        origin_path=data_attribute_pin(pins.get('audit_tool_origin'))
        data_attribute_require(sha256_file(origin_path)=='2858eb5876bf57ad90c86cec6b9cedd9b5e290364ddaf7317bdf919b2dca4f93' and
            not stat.S_IMODE(origin_path.stat().st_mode)&0o222,'predecessor snapshot origin is not the sealed approved manifest')
        origin=json.loads(origin_path.read_text())
        data_attribute_require(sum(Path(entry.get('backup','/nonexistent')).resolve()==audit_tool and
            entry.get('sha256')==sha256_file(audit_tool) for entry in origin.get('files',[]))==1,
            'predecessor auditor path differs from actual snapshot origin')
        predecessor=data_attribute_pin(pins.get('audit_tool_review'))
        data_attribute_require(sha256_file(predecessor)==
            'fcdcdb757c854367fd6e59bbe4ddfe086a6e1ceec4996daade036ed461ff3da5' and
            not stat.S_IMODE(predecessor.stat().st_mode)&0o222,
            'predecessor compiler-provenance review is not the exact sealed approved receipt')
        predecessor_review=json.loads(predecessor.read_text())
        data_attribute_require(predecessor_review.get('decision')=='READY_TOOL' and
            predecessor_review.get('candidate',{}).get('sha256')==sha256_file(audit_tool) and
            predecessor_review.get('grants_data_recovery') is False and
            predecessor_review.get('grants_function_acceptance') is False,
            'predecessor tool review does not attest this bounded provenance implementation')
    for check_name in ('build_object_matches_audit','build_assembly_matches_audit',
                       'source_is_what_cpp_compiled','assembly_matches_source','toolchain_messages'):
        checks=[c for c in receipt.get('unit_checks',[]) if c.get('check')==check_name]
        data_attribute_require(len(checks)==1 and checks[0].get('ok') is True and
                               not checks[0].get('deferred') and not checks[0].get('informational'),
                               'actual consumer compiler/cc1/object provenance is not proved')
        if check_name=='build_object_matches_audit':
            data_attribute_require(checks[0].get('sha256')==sha256_file(raw_path),
                                   'consumer raw object differs from audited compilation')
    emitted=[f for f in receipt.get('functions',[]) if f.get('name')==function and f.get('state')=='c']
    data_attribute_require(len(emitted)==1,'one actual C consumer emission required')
    for check_name in ('provenance','symbol','record_fidelity'):
        data_attribute_require(emitted[0].get('checks',{}).get(check_name,{}).get('ok') is True,
                               'consumer emission/original identity is not proved')
    source=source_path.read_text(encoding='utf-8',errors='surrogateescape');parsed=tu_edit.parse(source)
    blocks=[b for b in parsed.functions() if b.name==function and b.state=='c' and not b.conditional]
    data_attribute_require(len(blocks)==1,'consumer must have one current C definition')
    block=blocks[0];body=source[block.core_start:block.core_end]
    data_attribute_require(consumer.get('definition_sha256')==hashlib.sha256(body.encode('utf-8',errors='surrogateescape')).hexdigest(),
                           'reviewed consumer definition changed')
    originals=[s for s in elf.symbols if s['name']==function and s['type']==STT_FUNC and s['size']>0]
    data_attribute_require(len(originals)==1,'consumer needs one actual original FUNCTION')
    original=originals[0];identity=consumer.get('original_identity') or {}
    data_attribute_require(identity.get('name')==function and identity.get('type')==STT_FUNC and
        identity.get('binding')==original['binding'] and identity.get('size')==original['size'] and
        int(identity.get('address','0'),0)==int(original['va'],16),'consumer original FUNCTION identity drift')
    raw=RelObject(raw_path.read_bytes());raw_functions=[s for s in raw.functions() if s['name']==function]
    linked=[s for s in context['linked'].symbols if s['name']==function and s['type']==STT_FUNC and
            s['binding']==original['binding'] and s['size']==original['size'] and s['va']==original['va']]
    data_attribute_require(len(raw_functions)==len(linked)==1 and raw_functions[0]['size']==original['size'] and
                           raw_functions[0]['binding']==original['binding'],'consumer raw/linked FUNCTION extent/binding drift')
    original_code=elf.data[original['file_offset']:original['file_offset']+original['size']]
    linked_code=context['linked'].data[linked[0]['file_offset']:linked[0]['file_offset']+linked[0]['size']]
    data_attribute_require(original_code==linked_code,'consumer linked code differs from retail')
    pp_text=expected['preprocessed'].read_text(encoding='utf-8',errors='replace')
    pp=tu_edit.parse(pp_text);pp_blocks=[b for b in pp.functions() if b.name==function and b.state=='c']
    data_attribute_require(len(pp_blocks)==1,'consumer CPP definition missing/duplicated')
    pp_body=pp_text[pp_blocks[0].core_start:pp_blocks[0].core_end]
    roles=[];statements=consumer.get('statements')
    data_attribute_require(isinstance(statements,list) and statements,'concrete reviewed consumer statements required')
    source_tokens,pp_tokens=tu_edit.c_tokens(body),tu_edit.c_tokens(pp_body)
    def contains(tokens,needle):
        return (isinstance(needle,list) and len(needle)>=3 and
                sum(tokens[i:i+len(needle)]==needle for i in range(len(tokens)-len(needle)+1))==1)
    for statement in statements:
        data_attribute_require(statement.get('role') in ('view_address','pointer_table','terminated_reader',
            'fixed_width_copy','mutable_color_write','message_decoder','decoder_table','pointer_field_flow') and
            contains(source_tokens,statement.get('source_tokens')) and
            contains(pp_tokens,statement.get('preprocessed_tokens')),'consumer statement/operand/role changed')
        roles.append(statement['role'])
    helpers=[]
    for helper in consumer.get('source_helpers',[]):
        data_attribute_require(helper.get('name')=='eMessageIsFontCommand',
                               'unsupported source-inline helper witness')
        owned=[i for i in parsed.items if i.kind=='func' and i.name==helper['name'] and not i.conditional]
        expanded=[i for i in pp.items if i.kind=='func' and i.name==helper['name'] and not i.conditional]
        data_attribute_require(len(owned)==len(expanded)==1,'actual inline source helper missing/duplicated')
        text=owned[0].text(source)
        expanded_text=expanded[0].text(pp_text)
        data_attribute_require(helper.get('definition_sha256')==hashlib.sha256(text.encode()).hexdigest() and
            helper.get('source_tokens')==tu_edit.c_tokens(text) and
            helper.get('preprocessed_tokens')==tu_edit.c_tokens(expanded_text) and
            'static' in tu_edit.c_tokens(text)[:8] and
            any(t in tu_edit.c_tokens(text)[:8] for t in ('inline','__inline','__inline__')) and
            not any(s['name']==helper['name'] for s in raw.functions()),
            'source helper must be the reviewed actual inlined definition, never an invented original FUNCTION')
        helpers.append(dict(name=helper['name'],definition_sha256=helper['definition_sha256'],
                            original_FUNCTION_identity=None,no_function_credit=True))
    calls=[]
    raw_function=raw_functions[0]
    for call in consumer.get('calls',[]):
        callee=call.get('callee');offset=call.get('raw_offset')
        operand=data_attribute_r26_call(raw,raw_function,offset,callee)
        data_attribute_require(call.get('relocation')==operand['relocation'] and
            call.get('raw_operand')==operand['raw_operand'],'actual callee relocation/operand changed')
        original_callees=[s for s in elf.symbols if s['name']==callee and s['type']==STT_FUNC and s['size']>0]
        data_attribute_require(len(original_callees)==1,'call needs one actual original FUNCTION target')
        relative=offset-raw_function['value'];word=struct.unpack_from('<I',original_code,relative)[0]
        call_address=int(original['va'],16)+relative
        target=((call_address+4)&0xf0000000)|((word&0x3ffffff)<<2)
        data_attribute_require(word>>26 in (2,3) and target==int(original_callees[0]['va'],16) and
            any(source_tokens[i:i+2]==[callee,'('] for i in range(len(source_tokens)-1)),
            'retail call target/source operand path differs')
        calls.append(dict(call,retail_call_address=hex(call_address),retail_target=hex(target),word=f'0x{word:08x}'))
    native=Elf(raw_path.read_bytes());address_links=[]
    for link in consumer.get('address_links',[]):
        data_attribute_require(link.get('kind') in ('direct_object','pointer_table'),'unsupported address-use path')
        target_name=link.get('target_name')
        target=[s for s in native.symbols if s.name==target_name]
        data_attribute_require(len(target)==1,'address-use target must be an actual unique raw symbol')
        target=target[0];matches=[]
        for symbol in native.symbols:
            if ((target.shndx!=0 and symbol.shndx==target.shndx and symbol.type in (STT_OBJECT,STT_SECTION)) or
                    (target.shndx==0 and symbol.index==target.index)):
                for ref in storage_relocations(native,symbol.index,{function}):
                    coordinate=symbol.value+ref['addend']
                    if ref['source_function']==function and ((target.shndx!=0 and target.value<=coordinate<target.value+target.size) or
                                                              (target.shndx==0 and coordinate==0)):
                        matches.append(dict(ref,symbol_index=symbol.index,target_offset=coordinate))
        data_attribute_require(link.get('reference') in matches,'actual relocation/GP-resolved consumer address use changed')
        if link['kind']=='direct_object':
            data_attribute_require(target_name==record['name'],'direct consumer does not address this exact view')
        else:
            data_attribute_require(tu_id==args.tu_id and isinstance(link.get('pointer_index'),int),
                                   'pointer table path must be registered in this exact source owner')
            table=[s for s in raw.symbols if s['name']==target_name and s['type']==STT_OBJECT and s['shndx']!=0]
            data_attribute_require(len(table)==1,'pointer-table producer storage missing')
            table=table[0];index=link['pointer_index'];off=table['value']+index*4
            data_attribute_require(0<=index<table['size']//4,'pointer-table index outside actual bounds')
            rels=data_attribute_rel_entries(raw,raw.section(table['section']),off,4)
            data_attribute_require(len(rels)==1,'pointer-table element needs one actual R32')
            resolved=data_attribute_resolve_r32(context,args,raw,rels[0],elf)
            data_attribute_require(resolved['value']==record['semantic_range'][0],
                                   'actual pointer-table element does not target this view')
        address_links.append(dict(link,actual_reference=link['reference'],view_range=record['semantic_range']))
    return dict(tu=tu_id,function=function,definition_sha256=consumer['definition_sha256'],
                original_identity=identity,original_code_sha256=hashlib.sha256(original_code).hexdigest(),
                roles=roles,address_links=address_links,original_constant_reads=data_attribute_constant_reads(elf,original),
                source_helpers=helpers,calls=calls,input_pins=pins,
                compiler_preservation_only=True,grants_function_acceptance=False)


def data_attribute_decode_literal(token):
    """Only the byte-string grammar used by actual C/assembler literals."""
    data_attribute_require(isinstance(token,str) and re.fullmatch(
        r'"(?:[^"\\\n]|\\(?:["\\?abfnrtv]|[0-7]{1,3}|x[0-9a-fA-F]+))*"',token),
        'not one complete byte string literal')
    result=bytearray();index=1
    escapes={'a':7,'b':8,'f':12,'n':10,'r':13,'t':9,'v':11,'"':34,'\\':92,'?':63}
    while index<len(token)-1:
        character=token[index];index+=1
        if character!='\\':
            value=ord(character)
        else:
            character=token[index];index+=1
            if character in escapes:
                value=escapes[character]
            elif character=='x':
                digits=re.match('[0-9a-fA-F]+',token[index:])[0];index+=len(digits);value=int(digits,16)
            else:
                digits=character
                while len(digits)<3 and index<len(token)-1 and token[index] in '01234567':
                    digits+=token[index];index+=1
                value=int(digits,8)
        data_attribute_require(0<=value<=255,'literal escape cannot invent byte truncation')
        result.append(value)
    return bytes(result)


def data_attribute_character_source(tu,item,name,payload):
    """Actual character declaration and complete byte-string initializer."""
    clean=re.sub(r'\b__attribute__?\s*\(\s*\(\s*section\s*\(\s*"\.[a-z0-9]+"\s*\)\s*\)\s*\)',
                 ' ',item.text(tu.text))
    tokens=tu_edit.c_tokens(clean);equal=tokens.index('=')
    declarator=' '.join(tokens[:equal]);literals=tokens[equal+1:-1]
    data_attribute_require(tokens[-1]==';' and re.fullmatch(
        r'(?:static )?(?:const )?char '+re.escape(name)+r' \[ (?:'+str(len(payload))+r' )?\]',declarator),
        'partial character object must be exactly its literal prefix, not an envelope/blob')
    data_attribute_require(literals and b''.join(data_attribute_decode_literal(t) for t in literals)+b'\0'==payload,
                           'complete source literal and required final NUL must equal the minimal compiler object')
    return tokens,equal,literals


def data_attribute_original_character_literal(context,args,tu,item,record,payload,elf,native_item):
    """A minimal partial character representation; no original C identity."""
    from tools.tu import data_carve
    proof=record['consumer_proof'];name=record['name']
    for key in ('original_purpose','original_type','original_capacity','original_binding'):
        data_attribute_require(key in record and record[key] is None,'unknown original character facts must be explicit null')
    tokens,equal,literals=data_attribute_character_source(tu,item,name,payload)
    data_attribute_require(len(payload)>1 and payload[-1]==0 and 0 not in payload[:-1],
                           'no-reader printable text needs nonempty text and its sole final NUL')
    directives=data_carve._data_directives(native_item)
    data_attribute_require(directives and directives[0][0]==record['semantic_range'][0],
                           'actual original mapped item must start at the character literal')
    line=native_item['block']['lines'][directives[0][2]].strip()
    match=re.fullmatch(r'(?:/\*[^\n]*?\*/\s*)?\.(asciz|string)\s+("(?:[^"\\\n]|\\.)*")\s*',line)
    data_attribute_require(match and data_attribute_decode_literal(match[2])+b'\0'==payload,
                           'registered native .asciz/.string must independently decode to this exact retail prefix')
    index=0;non_ascii=False
    while index<len(payload)-1:
        byte=payload[index]
        if 32<=byte<=126:
            index+=1
        else:
            data_attribute_require(index+1<len(payload)-1 and 0xa1<=byte<=0xfe and
                                   0xa1<=payload[index+1]<=0xfe,'numeric/control/unknown byte blob is not a character literal')
            index+=2;non_ascii=True
    if non_ascii:
        data_attribute_require(proof.get('text_encoding')=='evidenced_game_two_byte_text',
                               'non-ASCII character interpretation needs explicit encoded-text evidence')
        witness=proof.get('encoded_reader_witness') or {}
        original=[s for s in elf.symbols if s['name']==witness.get('function') and s['name']=='eMessageCat' and
                  s['type']==STT_FUNC and s['size']>0]
        data_attribute_require(len(original)==1,'encoded-text view needs the actual borrowed byte-pair reader identity')
        original=original[0];code=elf.data[original['file_offset']:original['file_offset']+original['size']]
        data_attribute_require(witness.get('address')==original['va'] and witness.get('size')==original['size'] and
                               witness.get('sha256')==hashlib.sha256(code).hexdigest(),
                               'borrowed encoded reader evidence is stale or unrelated')
    else:
        data_attribute_require(proof.get('text_encoding')=='ASCII','printable literal must disclose its byte encoding')
    constraints=proof.get('known_consumer_constraints')
    data_attribute_require(isinstance(constraints,list),'known consumer constraints must be explicitly disclosed')
    for constraint in constraints:
        data_attribute_require(constraint.get('kind') in ('terminated_read','bounded_read','bounded_write') and
            isinstance(constraint.get('required_extent'),int) and 0<constraint['required_extent']<=len(payload),
            'known consumer requires storage outside the proved character prefix')
        data_attribute_require(not (constraint['kind']=='bounded_write' and 'const' in tokens[:equal]),
                               'known write contradicts a const character view')
        source=data_attribute_pin(constraint.get('source'))
        data_attribute_require(source.is_relative_to(ROOT.resolve()),'known source witness outside actual subject')
        source_text=source.read_text(encoding='utf-8',errors='surrogateescape')
        functions=[b for b in tu_edit.parse(source_text).functions() if b.name==constraint.get('function') and b.state=='c']
        data_attribute_require(len(functions)==1 and constraint.get('definition_sha256')==hashlib.sha256(
            source_text[functions[0].core_start:functions[0].core_end].encode('utf-8',errors='surrogateescape')).hexdigest(),
            'known current consumer witness changed')
    data_attribute_require(not any(t in tu_edit.c_tokens(tu.text) for t in ('__builtin_object_size',)),
                           'unsupported object-capacity expression needs an explicit bounded source proof')
    return dict(kind='original_character_literal_view',length=len(payload),terminator_offset=len(payload)-1,
                original_directive=match[1],source_literal_tokens=literals,text_encoding=proof['text_encoding'],
                original_identity=None,original_purpose=None,original_type=None,original_capacity=None,
                original_binding=None,known_consumer_constraints=constraints,
                surviving_C_reader_required=False,borrowed_reader_credit=0)


def data_attribute_partial_consumer(context,args,tu,item,record,payload,elf,native_item):
    """Closed consumer proofs are checked separately from physical envelopes."""
    proof=record.get('consumer_proof') or {}
    kind=proof.get('kind')
    if kind=='original_character_literal_view':
        return data_attribute_original_character_literal(context,args,tu,item,record,payload,elf,native_item)
    data_attribute_require(kind in ('terminated_character_sequence','fixed_width_copy','encoded_message_sequence'),
                           'unsupported semantic consumer proof kind')
    chain=proof.get('consumer_chain')
    data_attribute_require(isinstance(chain,list) and 1<=len(chain)<=12,'bounded actual consumer chain required')
    checked=[];address_links=[]
    for consumer in chain:
        checked_consumer=data_attribute_consumer_binding(context,args,record,consumer,elf)
        checked.append(checked_consumer)
        address_links.extend(checked_consumer['address_links'])
    data_attribute_require(address_links,'no actual compiler address-use path to this semantic view')
    type_evidence=proof.get('type_evidence') or {}
    data_attribute_require(type_evidence.get('declaration_tokens')==tu_edit.c_tokens(item.text(tu.text)),
                           'semantic type proof does not bind this exact current declaration')
    names={c['function'] for c in checked}
    source_helpers={h['name'] for c in checked for h in c['source_helpers']}
    if kind=='terminated_character_sequence':
        data_attribute_character_source(tu,item,record['name'],payload)
        data_attribute_require(type_evidence.get('kind')=='character_sequence' and payload and
                               payload[-1]==0 and 0 not in payload[:-1] and
                               proof.get('element_size')==1 and proof.get('terminator')==0,
                               'not one complete character sequence including its actual terminator')
        data_attribute_require(any('terminated_reader' in c['roles'] for c in checked),
                               'actual reviewed terminated byte reader missing')
        data_attribute_require({'MenuCharNameGet','CharInfoMain','eMessageCpy','eMessageCat'}<=names and
            {'MenuCharNameGet','eMessageCpy','eMessageCat'}<=
            {call['callee'] for member in checked for call in member['calls']},
            'closed returned-character-pointer consumer/call chain is incomplete')
        semantic=dict(kind=kind,length=len(payload),terminator_offset=len(payload)-1)
    elif kind=='encoded_message_sequence':
        data_attribute_character_source(tu,item,record['name'],payload)
        character_declaration=re.sub(r'\b__attribute__?\s*\(\s*\(\s*section\s*\(\s*"\.[a-z0-9]+"\s*\)\s*\)\s*\)',
                                     ' ',item.text(tu.text)).split('=',1)[0]
        data_attribute_require(re.fullmatch(r'(?:static )?char '+re.escape(record['name'])+r' \[ '+
            str(len(payload))+r' \]', ' '.join(tu_edit.c_tokens(character_declaration))),
            'mutable message must be an actual character prefix, not a const/type/capacity substitution')
        _bytes,metadata=data_attribute_image_bytes(elf,record['semantic_range'][0],len(payload))
        data_attribute_require(metadata['load_flags']&2,'actual written message must reside in a writable retail load segment')
        data_attribute_require(type_evidence.get('kind')=='mutable_encoded_message' and
                               proof.get('element_size')==1 and
                               'xglFontGetSPcodeSize' in names and 'eMessageIsFontCommand' in source_helpers and
                               bool(names & {'eMessageNextGyou','eMessageNextWaitKeySearch','eMessageDrawType01'}),
                               'actual bounded message decoder chain/type is incomplete')
        data_attribute_require(any(c['callee']=='xglFontGetSPcodeSize' for member in checked for c in member['calls']) and
                               any(c['callee']=='eMessageSet' for member in checked for c in member['calls']),
                               'actual producer/decoder call operand path is incomplete')
        table=proof.get('decoder_table') or {}
        symbols=[s for s in elf.symbols if s['name']=='spcode.0' and s['type']==STT_OBJECT and
                 s['binding']==0 and s['size']==32 and s.get('section')=='.data' and s['va']=='0x00490990']
        data_attribute_require(len(symbols)==1,'actual original 32-byte decoder table required')
        symbol=symbols[0]
        table_bytes=elf.data[symbol['file_offset']:symbol['file_offset']+32]
        data_attribute_require(table==dict(name='spcode.0',address=symbol['va'],size=32,
            element_size=1,index='code',sha256=hashlib.sha256(table_bytes).hexdigest()) and table_bytes[12]==3,
            'actual decoder table/index/width changed')
        # This capability is closed to the reviewed two message grammar forms.
        # Internal NUL parameters are consumed by their command; a generic
        # strlen rule or additional unreviewed controls cannot enter here.
        data_attribute_require(len(payload)>=7 and payload[0]==12 and payload[4:6]==b'\x1e\x00',
                               'unsupported encoded-message command prefix')
        cursor=6;steps=[dict(offset=0,kind='font_command_12',length=1+table_bytes[12]),
                        dict(offset=4,kind='message_command_30',length=2)]
        if payload[cursor:cursor+2]==b'\xa1\xa7':
            steps.append(dict(offset=cursor,kind='evidenced_EUC_pair',length=2));cursor+=2
        while cursor<len(payload) and 32<=payload[cursor]<=126:
            cursor+=1
        data_attribute_require(cursor==len(payload)-1 and payload[cursor]==0,
                               'message prefix must end at its decoded NUL without an emitted unknown suffix')
        data_attribute_require(any('mutable_color_write' in c['roles'] for c in checked),
                               'actual mutable color-byte consumer evidence missing')
        semantic=dict(kind=kind,length=len(payload),decoded_terminator_offset=cursor,steps=steps,
                      decoder_table=table,pointee_table_credit=0)
    else:
        data_attribute_require(type_evidence.get('kind')=='float_vector_view' and
                               proof.get('element_size')==4 and len(payload) in (16,32) and
                               type_evidence.get('scalar_count')==len(payload)//4,
                               'fixed-copy scope is a typed 4/8-float vector view')
        view_types={'D_004C6580':('MenuCameraSetPair',32),
            'D_004C65A0':('MenuCameraSetRotationPair',32),'D_004C65C0':('MenuCameraSetPair',32),
            'D_004C65E0':('MenuCameraSetVector',16),'D_004C6670':('MenuCharacterVector',16)}
        data_attribute_require(args.tu_id=='main/tu170' and record['name'] in view_types,
                               'float-view admission is closed to the five evidenced vector objects')
        type_name,type_size=view_types[record['name']]
        clean=re.sub(r'\b__attribute__?\s*\(\s*\(\s*section\s*\(\s*"\.[a-z0-9]+"\s*\)\s*\)\s*\)',
                     ' ',item.text(tu.text))
        data_attribute_require(tu_edit.c_tokens(clean.split('=',1)[0])==[type_name,record['name']] and
                               len(payload)==type_size,'actual float-view declarator/type/size differs')
        definitions=[
            'typedef struct Vector4 { float x; float y; float z; float w; } Vector4;',
            'typedef union MenuCharacterVector { Vector4 vector; unsigned long long words[2]; } MenuCharacterVector;',
            'typedef union MenuCameraPair { Vector4 vector[2]; float component[2][4]; unsigned long long words[4]; } MenuCameraPair;',
            'typedef union MenuCameraRotationPair { Vector4 vector[2]; unsigned long long words[4]; } MenuCameraRotationPair;',
            'typedef MenuCharacterVector MenuCameraVector;',
            'typedef struct MenuCameraSetPositionVectors { Vector4 vector[2]; } MenuCameraSetPositionVectors;',
            'typedef struct MenuCameraSetRotationVectors { Vector4 vector[2]; } MenuCameraSetRotationVectors;',
            'typedef struct MenuCameraSetPair { union { MenuCameraPair representation; MenuCameraSetPositionVectors values; } storage; } MenuCameraSetPair;',
            'typedef struct MenuCameraSetRotationPair { union { MenuCameraRotationPair representation; MenuCameraSetRotationVectors values; } storage; } MenuCameraSetRotationPair;',
            'typedef struct MenuCameraSetVector { MenuCameraVector storage; } MenuCameraSetVector;']
        expected_types=[tu_edit.c_tokens(text) for text in definitions]
        actual_types=[tu_edit.c_tokens(i.text(context['pre'].text)) for i in context['pre_tu'].free_items()
                      if i.kind=='decl' and not i.conditional]
        data_attribute_require(type_evidence.get('type_definition_tokens')==expected_types and
            all(actual_types.count(tokens)==1 for tokens in expected_types),
            'actual CPP float fields/array widths/ABI storage views differ from evidenced types')
        coverage=set();read_evidence=[]
        for consumer in checked:
            for access in consumer['original_constant_reads']:
                lo,hi=access['range']
                if record['semantic_range'][0]<=lo<record['semantic_range'][1]:
                    data_attribute_require(hi<=record['semantic_range'][1],
                                           'original fixed copy reads beyond compiler semantic prefix')
                    coverage.update(range(lo,hi));read_evidence.append(access)
        data_attribute_require(coverage==set(range(*record['semantic_range'])) and
                               any('fixed_width_copy' in c['roles'] for c in checked),
                               'actual original loads do not prove the entire exact fixed copy bound')
        semantic=dict(kind=kind,length=len(payload),element_size=4,original_reads=read_evidence)
    return dict(semantic=semantic,consumer_chain=checked,reviewed_range=record['semantic_range'],
                independent_semantic_review_required=True,no_function_or_pointee_credit=True)


def verified_partial_data_attribute_view(context,args,tu,item,name,section,obj,elf):
    """Compiler-defined semantic prefix, explicitly unknown original identity.

    This receipt grants only DATA section placement. It cannot turn a native
    scaffold slot or its unexplained suffix into an original C array extent.
    """
    if context is None:
        return None
    try:
        document=data_attribute_proof_document(context,args)
        records=[r for r in document.get('records',[]) if r.get('kind')=='registered_partial_storage_view' and
                 r.get('unit')==args.unit and r.get('tu')==args.tu_id and r.get('name')==name]
        data_attribute_require(len(records)==1,'one registered partial storage view required')
        record=records[0]
        data_attribute_require(record.get('original_identity','absent') is None and
                               record.get('gate_pins_sha256')==data_attribute_digest(document['gate_pins']),
                               'original identity must be explicitly unknown; gate pins must be reviewed')
        data_attribute_require(all(key in record and record[key] is None for key in
            ('original_purpose','original_type','original_capacity','original_binding')),
            'anonymous view must disclose unknown original purpose/type/capacity/binding')
        review=data_attribute_review(record)
        data_attribute_declaration(context,tu,item,name)
        inputs=record.get('inputs') or {}
        original_path=Path(original_elf(args.unit)[1]['path'])
        if not original_path.is_absolute():
            original_path=ROOT/original_path
        expected=dict(source=context['source'],preprocessed=context['preprocessed_path'],
                      assembly=context['assembly_path'],raw=context['raw_path'],selected=context['selected_path'],
                      linked=context['linked_path'],original=original_path)
        data_attribute_require(set(inputs)==set(expected),'complete actual partial-view input pins required')
        for key,value in expected.items():
            data_attribute_pin(inputs[key],value)
        slot,semantic=record.get('original_physical_slot'),record.get('semantic_range')
        data_attribute_require(isinstance(slot,list) and isinstance(semantic,list) and
            len(slot)==len(semantic)==2 and all(isinstance(v,int) for v in slot+semantic) and
            slot[0]==semantic[0] and slot[0]<semantic[1]<=slot[1] and slot[1]-slot[0]<=65536,
            'semantic range must be one bounded prefix of an explicit layout slot')
        address,end=semantic;size=end-address
        expected_unknown=[] if end==slot[1] else [[end,slot[1]]]
        data_attribute_require(record.get('unknown_ranges')==expected_unknown and
                               record.get('unknown_ranges_credited') is False,
                               'unknown suffix must exactly partition the slot and have zero credit')
        data_attribute_require(not any(s['type']==STT_OBJECT and s['size']>0 and
            int(s['va'],16)<slot[1] and slot[0]<int(s['va'],16)+s['size'] for s in elf.symbols
            if 0<s['section_index']<0xff00),'partial view cannot overlap any actual original OBJECT')
        data_attribute_require(not any(s['name'] and int(s['va'],16)==address for s in elf.symbols
            if s['type'] not in (3,4) and 0<s['section_index']<0xff00),
            'an actual original name/type/binding cannot be hidden as unknown')
        symbols=[]
        for image in (obj,context['raw'],context['selected']):
            found=[s for s in image.symbols if s['name']==name and s['shndx']!=0]
            data_attribute_require(len(found)==1,'unique actual compiler view symbol required')
            symbol=found[0]
            data_attribute_require(symbol['type']==STT_OBJECT and symbol['size']==size and
                                   symbol['binding'] in (0,1),'compiler OBJECT must equal semantic prefix extent')
            symbols.append(symbol)
        after,raw_symbol,selected_symbol=symbols
        data_attribute_require(after==raw_symbol and raw_symbol['section']==section and
            record.get('compiler_identity')=={key:raw_symbol[key] for key in
                ('name','type','binding','size','section','value')},'actual compiler identity differs from registration')
        parts=[p for p in data_attribute_input_spans(context,args) if p['symbols']==[name] and
               p['section']==section and p['offset']==raw_symbol['value'] and p['end']-p['offset']==size and
               p['address']==address and p['end_address']==end]
        data_attribute_require(len(parts)==1,'registered compiler span must equal semantic prefix, not slot')
        owner=parts[0]
        data_attribute_require(owner['run'].get('c_owned_symbols')==[name] and
            [int(v,0) if isinstance(v,str) else int(v) for v in owner['run'].get('range',[])]==semantic and
            [int(v,0) if isinstance(v,str) else int(v) for v in owner['run'].get('generated',[])]==semantic and
            owner['part'].get('credit')=='verified' and not owner['run'].get('c_storage_aliases'),
            'current partial-view owner/generated span must equal only the semantic prefix')
        linked_symbols=[s for s in context['linked'].symbols if s['name']==name and
            s['type']==STT_OBJECT and s['binding']==raw_symbol['binding'] and s['size']==size and
            s.get('section')==section and int(s['va'],16)==address]
        data_attribute_require(len(linked_symbols)==1,'linked compiler view symbol identity/address/extent mismatch')
        spans=[s for s in context['row'].get('verified_location_spans',[]) if name in s.get('names',[]) or
               name in s.get('object_symbols',[])]
        data_attribute_require(len(spans)<=1,'duplicate strict DATA prefix receipts')
        span=spans[0] if spans else None
        baseline_owner=None
        if span:
            raw_section=context['raw'].section(raw_symbol['section'])
            selected_section=context['selected'].section(selected_symbol['section'])
            data_attribute_require(span.get('names')==[name] and span.get('object_symbols')==[name] and
                span.get('original_identities')==[] and span.get('range')==semantic and span.get('length')==size and
                span.get('split_range')==semantic and span.get('section')==section and
                span.get('c_object_section')==selected_symbol['section'] and
                span.get('c_object_offset')==selected_symbol['value'] and
                span.get('c_object_section_size')==selected_section['size'] and
                span.get('c_object_section_alignment')==selected_section['align'] and
                span.get('compiler_input_span')==dict(section=raw_symbol['section'],offset=raw_symbol['value'],
                                                     section_size=raw_section['size']) and
                span.get('object_sha256')==context['hashes']['linked_data_object_sha256'] and
                span.get('credited') is not False and
                not any(span.get(k) for k in ('support_only','storage_only','storage_owner')),
                'actual strict DATA prefix receipt differs from partial view')
        else:
            data_attribute_require(args.tu_id=='main/tu169' and
                record.get('consumer_proof',{}).get('kind')=='original_character_literal_view',
                'no claimed prefix receipt and not an approved existing anonymous character owner')
            baseline_owner=data_attribute_existing_C_owner(args,name,section,slot,
                record.get('existing_c_owner_baseline'))
            data_attribute_require(record.get('existing_c_owner_baseline')==baseline_owner['baseline_inputs'],
                                   'reviewed auxiliary view lacks exact historical C-definition/ownership pins')
        other=[s for s in context['row'].get('verified_location_spans',[]) if s is not span and
               max(s.get('range',[0,0])[0],slot[0])<min(s.get('range',[0,0])[1],slot[1])]
        data_attribute_require(not other,'another credited span intersects partial view slot')
        for locations in (context['row'].get('verified_locations',[]),
                          context['candidate'].get('recovered_location_candidates',[]),
                          context['control'].get('identities',[])):
            matches=[r for r in locations if r.get('unit')==args.unit and r.get('tu')==args.tu_id and
                     r.get('section')==section and int(r.get('address','0'),0)==address and r.get('names')==[name]]
            data_attribute_require(len(matches)==(1 if span else 0),
                                   'partial prefix claim missing/duplicated or auxiliary owner falsely claimed as new')
        from tools.tu import data_carve
        native=data_carve.tu_context(ROOT,args.unit,context['unit_dir'],args.tu_id)
        data_attribute_gate_bound_pin(context,context['unit_dir']/'tu-manifest.json')
        items=[i for i in data_carve.section_items(native,section) if i['start']==slot[0] and i['end']==slot[1]]
        data_attribute_require(len(items)==1,'physical layout slot must be an actual native mapped item')
        data_attribute_require(items[0].get('name')==name and name in items[0].get('labels',[]),
                               'anonymous view must retain its actual registered scaffold label')
        pieces=[p for p in native['pieces'].get(section,[]) if p['start']<=slot[0] and slot[1]<=p['end']]
        data_attribute_require(len(pieces)==1,'one actual native physical-slot piece required')
        data_attribute_gate_bound_pin(context,Path(native['path_of'](pieces[0],section)))
        validated_tails=[]
        for unknown in expected_unknown:
            auxiliary_detail=None
            tails=[t for t in (context['row'].get('uncredited_original_item_tails',[])+
                              context['row'].get('uncredited_scaffold_suffixes',[])) if
                   t.get('credited') is False and t.get('section')==section and t.get('range')==unknown and
                   t.get('compiler_range')==semantic and t.get('original_item_range')==slot and
                   t.get('kind') in ('original_string_item_tail','uncredited_scaffold_suffix')]
            if baseline_owner is not None:
                data_attribute_require(not tails,'auxiliary existing owner must not fabricate a new-claim gate tail receipt')
            if not tails:
                import inspect
                from tools.tu import data_gate
                from tools.tu.elfinfo import Elf
                method=record.get('auxiliary_tail_proof_kind') if baseline_owner is not None else 'shared_unknown_scaffold_suffix'
                data_attribute_require(method in ('computed_original_string_item_tail','shared_unknown_scaffold_suffix'),
                                       'reviewed existing view must explicitly name its auxiliary suffix computation')
                auxiliary_detail=None
                if method=='computed_original_string_item_tail':
                    predicate=data_gate.original_string_item_tail
                    data_attribute_require(hashlib.sha256(inspect.getsource(predicate).encode()).hexdigest()==
                        '7581dea8729032139f8f2be23e23b06b7030991aba72b4abdb51039c29be8989',
                        'approved closed original-string-tail predicate changed')
                    raw_native=Elf(context['raw_path'].read_bytes())
                    original_native=Elf(Path(native['orig']).read_bytes())
                    data_attribute_require(sha256_file(native['orig'])==context['hashes']['original_sha256'],
                                           'auxiliary tail native original differs from configured retail authority')
                    raw_sections=[s for s in raw_native.sections if s.name==raw_symbol['section']]
                    data_attribute_require(len(raw_sections)==1,'one actual current raw suffix section required')
                    computed=predicate(native,data_carve,section,slot[0],semantic[1],slot[1],raw_native,
                                       raw_sections[0],raw_symbol['value'],original_native)
                    data_attribute_require(computed is not None,'closed existing tail predicate refused this auxiliary suffix')
                else:
                    mechanical_record=data_attribute_composition_inputs(document,args,record)
                    shared_gate,shared_carve,dependencies=data_attribute_load_shared_suffix(mechanical_record)
                    data_attribute_require(not any(s['type'] in (4,9) for s in elf.sections),
                                           'auxiliary retail-reference exclusion does not support original REL/RELA sections')
                    data_attribute_require(not any(s['type'] not in (3,4) and 0<s['section_index']<0xff00 and
                        semantic[1]<=int(s['va'],16)<slot[1] for s in elf.symbols),
                        'unknown suffix contains an actual original interior symbol, including unnamed symbols')
                    exclusion=(data_attribute_original_reference_exclusion(record,elf,shared_gate.Elf,unknown)
                               if baseline_owner is not None else None)
                    fragment=data_attribute_auxiliary_scaffold_fragment(context,args,mechanical_record,section,semantic,slot,elf)
                    computed=shared_gate.uncredited_character_scaffold_suffix(native,shared_carve,section,
                        slot[0],semantic[1],slot[1],context['raw_path'],context['selected_path'],context['linked_path'],
                        context['source'],context['assembly_path'],context['preprocessed_path'],
                        ROOT/'config/objects/data-carves.json',tu_id=args.tu_id)
                    data_attribute_require(computed is not None,'shared typed-prefix mechanics refused auxiliary unknown suffix')
                    auxiliary_detail=dict(kind=('auxiliary_existing_C_owner_unknown_scaffold_suffix' if baseline_owner is not None
                                               else 'computed_uncredited_character_scaffold_suffix'),
                        reference_exclusion=exclusion,scaffold_composition=fragment,dependencies=dependencies,
                        original_input_section_alignment=None,original_capacity_proved=False,
                        credited=False,credited_bytes=0)
                tails=[computed]
            data_attribute_require(len(tails)==1,'unknown suffix needs actual separate uncredited tail receipt')
            tail=tails[0]
            data_attribute_require(tail.get('credited') is False and tail.get('section')==section and
                tail.get('range')==unknown and tail.get('compiler_range')==semantic and
                tail.get('original_item_range')==slot,'actual suffix disposition differs from semantic/physical partition')
            if tail['kind']=='uncredited_scaffold_suffix':
                data_attribute_require(tail.get('disposition')=='uncredited_scaffold_suffix' and
                    tail.get('credited_bytes')==0 and tail.get('owner')==name and
                    tail.get('original_capacity_proved') is False and tail.get('alignment_proved') is False and
                    tail.get('grants_source_eligibility') is False and
                    tail.get('input_section')==raw_symbol['section'] and
                    tail.get('input_offset')==raw_symbol['value'],
                    'unknown suffix is not a zero-credit mechanical scaffold disposition')
                data_attribute_require(set(tail.get('inputs',{}))==set(inputs)|{'registry','unit_manifest'},
                                       'scaffold suffix must pin every actual compiler/image/registry input')
                for key,path in expected.items():
                    # The native splitter reads its actual unit original copy;
                    # the auditor independently reads the configured retail
                    # authority. Both paths must be exact and hash-identical.
                    actual_path=Path(native['orig']) if key=='original' else path
                    data_attribute_pin(tail['inputs'][key],actual_path)
                    if key=='original':
                        data_attribute_require(tail['inputs'][key]['sha256']==context['hashes']['original_sha256'],
                                               'native suffix original copy differs from configured retail authority')
                data_attribute_pin(tail['inputs']['registry'],ROOT/'config/objects/data-carves.json')
                data_attribute_pin(tail['inputs']['unit_manifest'],context['unit_dir']/'tu-manifest.json')
            validated_tails.append(dict(mechanical_receipt=tail,auxiliary_computation=not any(tail is t for t in
                context['row'].get('uncredited_original_item_tails',[])+context['row'].get('uncredited_scaffold_suffixes',[])),
                current_compiler_raw_input_alignment=context['raw'].section(raw_symbol['section'])['align'],
                original_input_section_alignment=None,original_capacity_proved=False,credited=False,credited_bytes=0))
            if auxiliary_detail is not None:
                validated_tails[-1]['computed_unknown_suffix']=auxiliary_detail
        original_bytes,original_metadata=data_attribute_image_bytes(elf,slot[0],slot[1]-slot[0])
        linked_bytes,linked_metadata=data_attribute_image_bytes(context['linked'],slot[0],slot[1]-slot[0])
        data_attribute_require(original_bytes==linked_bytes,'full physical slot differs from retail')
        payload=original_bytes[:size]
        for unknown in expected_unknown:
            tail=next(t['mechanical_receipt'] for t in validated_tails if
                      t['mechanical_receipt'].get('range')==unknown)
            if tail.get('kind')=='uncredited_scaffold_suffix':
                data_attribute_require(tail.get('prefix_sha256')==hashlib.sha256(payload).hexdigest() and
                    tail.get('suffix_sha256')==hashlib.sha256(original_bytes[size:]).hexdigest(),
                    'mechanical suffix payload hashes do not match actual retail bytes')
        compiler_metadata=[]
        for image,symbol in ((context['raw'],raw_symbol),(context['selected'],selected_symbol)):
            csec=image.section(symbol['section'])
            data_attribute_require(csec and csec['type']==1 and symbol['value']+size<=csec['size'] and
                image.bytes_of(csec)[symbol['value']:symbol['value']+size]==payload and
                not data_attribute_rel_entries(image,csec,symbol['value'],size),
                'literal compiler prefix must be complete, relocation-free and retail-equal')
            compiler_metadata.append(dict(name=csec['name'],flags=csec['flags'],alignment=csec['align'],
                                          object_offset=symbol['value'],object_size=symbol['size']))
        data_attribute_emission(context,tu,name,size,section)
        consumer=data_attribute_partial_consumer(context,args,tu,item,record,payload,elf,items[0])
        return dict(name=name,section=section,address=hex(address),size=size,
                    evidence='registered_partial_storage_view',original_identity=None,
                    existing_C_owner_provenance=baseline_owner,
                    original_physical_slot=slot,semantic_range=semantic,unknown_ranges=expected_unknown,
                    unknown_ranges_credited=False,compiler_identity=record['compiler_identity'],
                    uncredited_suffix_evidence=validated_tails,original_input_section_alignment=None,
                    original_capacity_proved=False,
                    compiler_section=compiler_metadata[0],selected_section=compiler_metadata[1],
                    original_section=original_metadata,linked_section=linked_metadata,
                    semantic_payload_sha256=hashlib.sha256(payload).hexdigest(),
                    physical_slot_sha256=hashlib.sha256(original_bytes).hexdigest(),consumer_proof=consumer,
                    extension_input_pins=context.get('extension_pins',{}),
                    independent_review=review,grants_new_data_recovery=False,grants_function_acceptance=False,
                    **context['hashes'])
    except (AuditError,OSError,ValueError,KeyError,TypeError,tu_edit.ParseError,struct.error):
        return None


def matching_data_section_attributes(tu, tu_rel, obj, elf, required=False, data_row=None, build_object=None,
                                     audit_args=None, assembly=None, preprocessed=None, verified_data_row=None):
    """Separate unchanged published data placement from function admission.

    This affects admission text only, never the compiled source or object.
    A complete DATA-only gate can admit one literal section attribute on an
    exact positive original OBJECT; its original size, binding, section and
    full linked compiler span remain mandatory. An explicitly supplied full
    ELF/DATA gate chain can establish the same per-object placement in a TU
    that also contains C functions; their normal admission is unchanged.
    Matching may otherwise defer
    one literal data-section attribute on a published, unchanged top-level
    object declaration emitted in the requested data section. Original data
    identity remains a deferred data check, not a function-admission condition.
    Function attributes, new declarations, other attributes and headers remain
    subject to their ordinary source-class checks.
    """
    verified_only = required
    complete_data_only = bool(data_row and not tu.functions() and build_object and build_object.is_file() and
                              tu.text == (ROOT / tu_rel).read_text(encoding='utf-8', errors='surrogateescape') and
                              data_row.get('source_sha256') == sha256_file(ROOT / tu_rel) and
                              data_row.get('object_sha256') == sha256_file(build_object))
    # A separately validated linked receipt can prove one complete object even
    # when unrelated inherited DATA ownership is still pending. It never feeds
    # the legacy full-DATA-only branch or changes the whole-TU recovery grant.
    placement_row = verified_data_row if verified_data_row is not None else data_row
    context = verified_data_attribute_context(audit_args, tu, tu_rel, obj, elf, placement_row,
                                             build_object, assembly, preprocessed)
    if required and not complete_data_only and context is None:
        return {}, [], []
    canonical_root=data_attribute_canonical_root()
    published = canonical_root / tu_rel
    if not published.resolve().is_relative_to(canonical_root) or not published.is_file():
        return {}, [], []
    published_text = published.read_text(errors='surrogateescape')
    published_declarations = {
        tuple(tu_edit.c_tokens(item.text(published_text)))
        for item in tu_edit.parse(published_text).free_items()
        if item.kind == 'decl' and not item.conditional
    }
    attribute = re.compile(
        r'\b__attribute__?\s*\(\s*\(\s*section\s*\(\s*'
        r'"(?P<section>\.(?:data|rodata|rdata|sdata|bss|sbss|lit4|lit8))"'
        r'\s*\)\s*\)\s*\)')
    replacements, deferred, verified = {}, [], []
    for item in tu.free_items():
        if item.kind != 'decl' or item.conditional:
            continue
        text = item.text(tu.text)
        found = list(attribute.finditer(text))
        if len(found) != 1:
            continue
        tokens = tu_edit.c_tokens(text)
        if ((tuple(tokens) not in published_declarations and not complete_data_only and context is None) or
                'typedef' in tokens or 'extern' in tokens):
            continue
        attrs = [i for i, token in enumerate(tokens) if token in ('__attribute', '__attribute__')]
        if len(attrs) != 1:
            continue
        pos = attrs[0]
        section = found[0]['section']
        expected = [tokens[pos], '(', '(', 'section', '(', '"' + section + '"', ')', ')', ')']
        if tokens[pos:pos + len(expected)] != expected or ('=' in tokens and pos > tokens.index('=')):
            continue
        names = {name for kind, name in tu_edit.decl_keys(text) if kind == 'ordinary'}
        if len(names) != 1:
            continue
        name = next(iter(names))
        if any(token == name and index + 1 < len(tokens) and tokens[index + 1] == '('
               for index, token in enumerate(tokens)):
            continue
        original = [s for s in elf.symbols if s['name'] == name and s['type'] == STT_OBJECT]
        emitted = [s for s in obj.symbols if s['name'] == name and s['shndx'] != 0]
        if len(emitted) != 1:
            continue
        after = emitted[0]
        if (after['type'] != STT_OBJECT or after['size'] <= 0 or
                after.get('section') not in DATA_SECTIONS or
                _SECTION_ALIASES.get(after['section']) != _SECTION_ALIASES.get(section)):
            continue
        before = original[0] if len(original) == 1 else None
        original_identity_equal = bool(
            before and before['size'] == after['size'] and
            before['binding'] == after['binding'] and
            before.get('section') in DATA_SECTIONS and
            _SECTION_ALIASES.get(before['section']) == _SECTION_ALIASES.get(section))
        match = found[0]
        proof = verified_data_attribute_object(context, audit_args, tu, item, name, section, obj, elf)
        if proof is None:
            proof = verified_relocated_data_attribute_object(context,audit_args,tu,item,name,section,obj,elf)
        if proof is None:
            proof = verified_partial_data_attribute_view(context,audit_args,tu,item,name,section,obj,elf)
        if proof is not None:
            replacements[item.start] = text[:match.start()] + ' ' + text[match.end():]
            verified.append(proof)
            continue
        spans = [span for span in (data_row or {}).get('verified_location_spans', [])
                 if name in span.get('names', []) and name in span.get('object_symbols', [])]
        if complete_data_only and original_identity_equal and before['size'] > 0 and len(spans) == 1:
            span = spans[0]
            compiler_span = span.get('compiler_input_span') or {}
            address = int(before['va'], 16)
            if (span.get('section') == section and span.get('range') == [address, address + before['size']] and
                    span.get('length') == before['size'] and
                    compiler_span.get('section') == after['section'] and
                    compiler_span.get('offset') == after['value']):
                replacements[item.start] = text[:match.start()] + ' ' + text[match.end():]
                verified.append(dict(name=name, section=section, size=before['size'],
                                     address=before['va'], source_sha256=data_row['source_sha256'],
                                     object_sha256=data_row['object_sha256'],
                                     linked_elf_sha256=data_row['linked_elf_sha256'],
                                     evidence='complete_linked_data_only_original_positive_OBJECT'))
                continue
        if verified_only or tuple(tokens) not in published_declarations:
            continue
        replacements[item.start] = text[:match.start()] + ' ' + text[match.end():]
        deferred.append(dict(name=name, section=section, size=after['size'],
                             original_identity_equal=original_identity_equal,
                             published_source=rel(published),
                             published_sha256=sha256_file(published),
                             declaration_sha256=hashlib.sha256(text.encode()).hexdigest(),
                             grants_source_or_data_recovery=False,
                             reason='Unchanged inherited data placement; pending strict data/source audit.'))
    return replacements, deferred, verified


def _asm_sort_lines(lines):
    """The non-empty #APP lines of a block, whitespace-normalised (`ee-asm-sort`)."""
    out = []
    for _, line in lines:
        line = re.sub(r'\s*,\s*', ',', re.sub(r'\s+', ' ', line.strip()))
        if line:
            out.append(line)
    return out


# What the one admitted `ee-asm-sort` block assembles from: the template of
# tools/match.py ASM_SORT_TEMPLATE with its two operands printed by cc1.
def _asm_sort_emitted():
    from tools import match
    return [line.replace('%%', '%').replace('%0', 's_nAlphaGroup').replace('%1', 's_aAlphaGroup')
            for line in match.ASM_SORT_TEMPLATE]


ASM_SORT_EMITTED = _asm_sort_emitted()


def audit_c(b, tu, prelude, tu_rel, header_set, regions, scaffold_names, obj_funcs, pool, start, cls,
            asm_contract, word_sites, unit=None, tu_id=None, adaptations=(), tu_repo=None,
            tu_class=None, contract_lost=None, storage_view_context=None):
    name = b.name
    reasons, checks = [], {}
    prov = []
    found = regions.get(name, [])
    if len(found) != 1:
        prov.append(f'{len(found)} cc1 regions (.ent/.end) for {name}')
    else:
        r = found[0]
        if r['section'] not in TEXT_SECTIONS:
            prov.append(f'region starts in section {r["section"]}')
        if r['file'] != tu_rel:
            prov.append(f'region is under .file {r["file"]!r}, not {tu_rel}')
        if not any(lbl == name for lbl, _ in r['labels']):
            prov.append(f'no {name}: label inside its region')
        if r['includes']:
            prov.append('.include inside the function region')
        for app in r['app']:
            bad = [line for _, line in app['lines']
                   if line.split(None, 1)[0] in DATA_DIRECTIVES or line.startswith(('.include', '.section'))]
            if cls not in INLINE_ASM_CLASSES:
                prov.append(f'inline asm (#APP at line {app["line"]}) in a {cls} function')
            elif cls == 'ee-assert-break' and [line.strip() for _, line in app['lines']
                                               if line.strip()] != ['break']:
                prov.append(f'an ee-assert-break #APP block (line {app["line"]}) must be exactly `break`')
            elif cls == 'ee-retaddr-store' and [line.strip() for _, line in app['lines']
                                                if line.strip()] not in (['sw $31,0($sp)'], ['break']):
                prov.append(f'an ee-retaddr-store #APP block (line {app["line"]}) must be exactly '
                            f'`sw $31,0($sp)` (or the DEBUG_BREAK() `break`)')
            elif cls == 'ee-asm-sort' and _asm_sort_lines(app['lines']) != ASM_SORT_EMITTED:
                prov.append(f'an ee-asm-sort #APP block (line {app["line"]}) must be exactly the '
                            f'AlphaGroupSort bubble sort of docs/ee-hand-asm.md')
            elif bad:
                prov.append(f'inline asm emits data/sections: {bad[:3]}')
        if cls == 'ee-asm-sort' and len(r['app']) != 1:
            prov.append(f'an ee-asm-sort function emits exactly one #APP block, not {len(r["app"])}')
        if cls == 'ee-retaddr-store':
            stores = sum(1 for app in r['app'] if [line.strip() for _, line in app['lines']
                                                   if line.strip()] == ['sw $31,0($sp)'])
            if stores != 1:
                prov.append(f'an ee-retaddr-store function emits exactly one `sw $31,0($sp)` #APP block, '
                            f'not {stores}')
        for d in r['data']:
            if d['section'] in TEXT_SECTIONS:
                prov.append(f'{d["directive"]} {d["operands"][:40]} in .text at line {d["line"]}')
            elif d['directive'] in WORD_LIKE and not all(
                    re.fullmatch(r'\$L\d+', op.strip()) for op in d['operands'].split(',')):
                prov.append(f'{d["directive"]} {d["operands"][:40]} at line {d["line"]} is not a cc1 jump table')
        checks['jump_table_words'] = sum(1 for d in r['data'] if d['directive'] in WORD_LIKE
                                         and d['section'] not in TEXT_SECTIONS)
    if name in scaffold_names or b.key in scaffold_names:
        prov.append(f'{name} is also defined by an included scaffold .s')
    if b.key in word_sites:
        prov.append(f'config/tu-build.json records scaffold .word sites for {b.key}: acceptance is blocked')
    checks['provenance'] = dict(ok=not prov, reasons=prov)
    reasons += prov
    sym_reasons, sym_detail = symbol_check(b.key, obj_funcs, pool, start)
    checks['symbol'] = dict(ok=not sym_reasons, reasons=sym_reasons, **sym_detail)
    reasons += sym_reasons
    elig, count = [], None
    text = prelude + tu.block_text(b) + '\n'
    if cls in INLINE_ASM_CLASSES:
        text = quadword_admission_text(text)
    composed = cls in INLINE_ASM_CLASSES and tu_class == 'native-ti'
    try:
        from tools import unaligned_scalar
        trial_types = unaligned_scalar.declarations(tu.block_text(b))
        if trial_types and (unit, tu_id, b.key) not in unaligned_scalar.TRIAL_FUNCTIONS:
            raise ValueError('aligned(1) scalar trial is not authorized for this function identity')
        count = admission(cls, tu_class, unit=unit, tu_id=tu_id, function=b.key)(
            text, Path(tu_rel), header_set)
    except ValueError as e:
        elig.append(f'{cls + "+native-ti" if composed else cls} admission refused: {e}')
    if cls == 'runtime-verbatim' and not elig:
        try:
            from tools import match
            match.runtime_verbatim_provenance(ROOT, dict(source=dict(path=tu_rel), headers=[]))
        except (ValueError, OSError) as e:
            elig.append(f'runtime-verbatim provenance: {e}')
    elig_notes = []
    if unaligned_scalar.declarations(tu.block_text(b)):
        elig_notes.append('GNU aligned(1) scalar trial: original-byte, extent and effective-type review required')
    if cls == 'hardware' and count and asm_contract is None:
        # Optional review documentation (user direction 2026-09-27): noted, never
        # a reason.  The hardware admission above stays the enforced guard.
        elig_notes.append('hardware inline asm without an asm contract' +
                          (f' (declared contract {contract_lost} is missing)' if contract_lost else ''))
    checks['eligibility'] = dict(ok=not elig, source_class=cls, tu_source_class=tu_class,
                                 asm_contract_lost=contract_lost, notes=elig_notes,
                                 admission=(cls + '+native-ti' if composed else cls), reasons=elig,
                                 hardware_statements=count if cls in INLINE_ASM_CLASSES else None,
                                 emitted_inline_blocks=len(found[0]['app']) if len(found) == 1 else None)
    reasons += elig
    original = [x for x in pool if x['name'] == b.key]
    fid_ok, fid = record_fidelity(b, tu, unit, tu_id, tu_rel,
                                  original[0]['va'] if len(original) == 1 else None, adaptations,
                                  tu_repo, storage_view_context)
    if fid.get('record'):
        fid['record_sha256'] = record_digest(fid['record'])
    # Informative only (user decision 2026-11-01: published = accepted; neutral
    # edits are proven by tools/codegen_neutral.py and the whole-file gate).
    checks['record_fidelity'] = dict(ok=True, informative=True, finding_ok=fid_ok, **fid)
    return dict(name=b.key, block=name, state='c', claimed=True,
                category=emitted_c_category(cls, count, len(found[0]['app']) if len(found) == 1 else 0),
                ok=not reasons, reasons=reasons, checks=checks)


def audit_accepted_asm(b, included, asm, obj_funcs, pool, start, word_sites, unit):
    from tools import match
    reasons, checks = [], {}
    info = included.get(b.name)
    rel_path = f'{b.asm_dir}/{b.name}.s'
    parts = Path(b.asm_dir).parts
    if not (len(parts) >= 2 and parts[0] == 'src' and parts[1] == unit) or 'nonmatchings' in parts:
        reasons.append(f'ACCEPTED_ASM folder {b.asm_dir} is not tracked source under src/{unit}/')
    if not info or info['text'] is None:
        reasons.append(f'{rel_path} missing')
        return dict(name=b.key, state='accepted_asm', claimed=True, category='exact_asm', ok=False,
                    reasons=reasons, checks=checks)
    includes = [v for k, v, _n, _s in asm.sequence if k == 'include']
    if includes.count(rel_path) != 1:
        reasons.append(f'{rel_path} is included {includes.count(rel_path)} times by the compiler output')
    defined = list(info['defines'] or [])
    digest = hashlib.sha256((info['text'] or '').encode()).hexdigest()
    index = accepted_asm_records()
    records = {index[n][0] for n in defined if n in index}
    checks['accepted_record'] = dict(functions=defined, records=sorted(records), sha256=digest,
                                     record_sha256={r: record_digest(r) for r in sorted(records)})
    if len(records) != 1:
        reasons.append(f'{rel_path} does not resolve to exactly one accepted ee-asm record: {sorted(records)}')
    else:
        record_path = sorted(records)[0]
        record = index[defined[0]][1]
        expected = record['source']['sha256']
        checks['accepted_record'].update(record=record_path, expected_sha256=expected)
        if digest != expected:
            reasons.append(f'{rel_path} is not the accepted source of {record_path} (sha256 {digest[:12]}... '
                           f'vs {expected[:12]}...)')
        missing = [f['name'] for f in record['functions'] if f['name'] not in defined]
        extra = [n for n in defined if n not in {f['name'] for f in record['functions']}]
        if missing or extra:
            reasons.append(f'{rel_path} defines {defined}, the record has '
                           f'{[f["name"] for f in record["functions"]]}')
        if relative_source(record) == rel_path:
            # The record was repointed at this tracked .s when the pre-migration
            # layout was retired, so the hash comparison above is the record's own
            # pin rather than a second witness. Say so instead of reading it as
            # agreement between two sources.
            origin = retired_origin(record)
            checks['accepted_record'].update(
                record_is_tu_source=True, archived=(origin or {}).get('path'),
                note='the accepted record points at this tracked .s, so the sha256 comparison checks the '
                     'record pin and nothing else; the pre-migration copy it was accepted from is in the '
                     'archive' + (f" ({origin['path']})" if origin else ' (no provenance recorded)'))
            if origin and origin['present']:
                archived = hashlib.sha256((ROOT / origin['path']).read_bytes()).hexdigest()
                checks['accepted_record']['archived_sha256'] = archived
                if archived != digest:
                    reasons.append(f'{rel_path} differs from the archived pre-migration copy '
                                   f'{origin["path"]} (sha256 {digest[:12]}... vs {archived[:12]}...)')
    try:
        match.ee_assembly(info['text'], [dict(name=n) for n in defined])
        checks['ee_assembly'] = dict(ok=True, functions=defined)
    except ValueError as e:
        reasons.append(f'ee_assembly refused {rel_path}: {e}')
        checks['ee_assembly'] = dict(ok=False, reason=str(e))
    sym = {}
    for n in defined or [b.key]:
        if n in word_sites:
            reasons.append(f'config/tu-build.json records scaffold .word sites for {n}')
        r, d = symbol_check(n, obj_funcs, pool, start)
        sym[n] = dict(ok=not r, reasons=r, **d)
        reasons += [f'{n}: {x}' for x in r]
    checks['symbols'] = sym
    return dict(name=b.key, state='accepted_asm', covers=defined, claimed=True, category='exact_asm',
                ok=not reasons, reasons=reasons, checks=checks)


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    ap.add_argument('--unit', required=True, help='main, ov01, ov02, ov10, ov11 or ov12')
    ap.add_argument('--tu-c', required=True, help='TU C source')
    ap.add_argument('--unit-dir', required=True, help='directory the TU is compiled from (cwd, -I .)')
    ap.add_argument('--root', help='the repository tree to audit: its src, config, records, adaptations and '
                                   'originals (default: the tree enclosing --unit-dir, never the tree this '
                                   'script happens to live in)')
    ap.add_argument('--tu-id', help='TU id of config/tu-build.json (or the TU map id)')
    ap.add_argument('--obj', help='the build object this TU links into; compared with the audit\'s own compile')
    ap.add_argument('--build-s', '--tu-s', dest='build_s', help='the build cc1 output, compared with the audit\'s')
    ap.add_argument('--tu-i', dest='ignored_tu_i', help=argparse.SUPPRESS)
    ap.add_argument('--out-dir', help='empty directory for the audit\'s own compile (default: a temporary one)')
    ap.add_argument('--compile', action='store_true', help=argparse.SUPPRESS)
    ap.add_argument('--no-reassemble', action='store_true', help=argparse.SUPPRESS)
    ap.add_argument('--asm-cwd', help='directory the build assembles from (staged .extern overlay)')
    ap.add_argument('--asm-include', action='append', help='extra -I directory for the assembler')
    ap.add_argument('--range', help='START:END text range (hex VAs)')
    ap.add_argument('--functions', action='append', help='original function names in order')
    ap.add_argument('--skeleton', help='all-INCLUDE_ASM skeleton giving the function list')
    ap.add_argument('--claim', action='append', help='audit only these functions')
    ap.add_argument('--ledger', help='TU ledger JSON naming the claimed functions')
    ap.add_argument('--source-class', choices=CLASSES, help='default: config/tu-build.json contract.source_class')
    ap.add_argument('--function-class', action='append', help='NAME=CLASS override')
    ap.add_argument('--asm-contract', help='asm contract JSON (hardware inline asm, enumerated top-level asm)')
    ap.add_argument('--contract', help='compile contract JSON; must equal the TU contract of the manifests')
    ap.add_argument('--cc-dir')
    ap.add_argument('--assembler')
    ap.add_argument('--flags')
    ap.add_argument('--include-dir', action='append')
    ap.add_argument('--c-owned', action='append', help='SECTION[=START:END] owned by the C TU')
    ap.add_argument('--data-gate-report', help='passing linked C-data report, hash-bound to this TU build')
    ap.add_argument('--data-elf-gate-report', help='explicit passing ELF gate pinned by the DATA report; '
                    'required for strict per-object DATA section attributes in functional TUs')
    ap.add_argument('--data-attribute-proof',help='explicit hash-bound independently reviewed partial DATA '
                    'view registration; grants only eligible DATA section placement')
    ap.add_argument('--data-owner-control-proof',help='explicit primary/secondary normal DATA context envelope; '
                    'zero-new-credit revalidation of complete current C-owned storage')
    ap.add_argument('--data-zero-marker-proof',help='explicit independently reviewed zero-byte original marker '
                    'transport through its already proven positive C owner; zero new storage or credit')
    ap.add_argument('--require-data-audit', action='store_true',
                    help='require complete data recovery checks in addition to function emission; '
                         'default: data_required=false; original scaffolding is permitted for '
                         'function acceptance with zero data credit')
    ap.add_argument('--candidate-manifest', help='manifest whose hash is recorded by the linked C-data report')
    ap.add_argument('--data-gate-object', help='object hash-pinned by linked C-data evidence; defaults to --obj')
    ap.add_argument('--allocation-report', action='append',
                    help='hash-bound raw-to-allocated overlay object report (repeatable)')
    ap.add_argument('--data-carve-report', action='append',
                    help='deterministic allocated-to-carved object report (repeatable)')
    ap.add_argument('--data-carve-object', help='selected carved object; defaults to --data-gate-object')
    ap.add_argument('--data-carve-tool', help='subject-root data_carve.py implementation used by the build')
    ap.add_argument('--data-carve-tool-sha256', help='SHA-256 pin for --data-carve-tool')
    ap.add_argument('--adaptations', action='append',
                    help='per-TU adaptation declarations (default: adaptations.json / merge-adaptations.json '
                         'beside the TU or its unit directory)')
    ap.add_argument('--json-out', help='write the audit JSON here (default stdout)')
    args = ap.parse_args(argv)
    try:
        result = audit(args)
    except (AuditError, tu_edit.ParseError, tu_edit.EditError, OSError, KeyError, ValueError) as e:
        result = dict(schema=SCHEMA, ok=False, unit=args.unit, tu_id=args.tu_id,
                      grants_acceptance=False, data_required=args.require_data_audit,
                      tool=dict(path='tools/tu_audit.py', sha256=sha256_file(__file__)),
                      error=f'{type(e).__name__}: {e}')
    except Exception as e:  # fail closed: an audit that cannot finish is a failing audit
        import traceback
        result = dict(schema=SCHEMA, ok=False, unit=args.unit, tu_id=args.tu_id,
                      grants_acceptance=False, data_required=args.require_data_audit,
                      tool=dict(path='tools/tu_audit.py', sha256=sha256_file(__file__)),
                      error=f'internal error {type(e).__name__}: {e}', traceback=traceback.format_exc()[-2000:])
    text = json.dumps(result, indent=1) + '\n'
    if args.json_out:
        Path(args.json_out).parent.mkdir(parents=True, exist_ok=True)
        Path(args.json_out).write_text(text)
    else:
        sys.stdout.write(text)
    if not result['ok']:
        summary = result.get('summary') or {}
        print(f'tu_audit: FAIL {result.get("error", "")} {summary.get("unit_failures", "")} '
              f'{[f["name"] for f in result.get("functions", []) if not f["ok"]]}', file=sys.stderr)
    return 0 if result['ok'] else 1


if __name__ == '__main__':
    sys.exit(main())
