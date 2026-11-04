#!/usr/bin/env python3
"""Build explicit C, audited hardware C/ASM or EE ASM; verify full linked extents.

CLI: --root PROJECT --packet JSON --output-dir FRESH [--profile JSON].
Packet v1 is documented in reports/B05-loop.md. All evidence remains local.
"""
import argparse
import difflib
import json
import os
from pathlib import Path
import re
import signal
import struct
import subprocess
import sys

sys.dont_write_bytecode = True
BASE = next(p for p in Path(__file__).resolve().parents if (p / 'tools/inventory.py').is_file())
sys.path.insert(0, str(BASE))
from tools import compiler_calibrate as cal
from tools import external_reference

PROFILE = Path(__file__).resolve().parents[1] / 'config/match-profile.json'
# `ee-gcc2_96-nosibcall` (config/nosibcall-match-profile.json, docs/nosibcall.md)
# is the unchanged pinned GCC 2.96 payload plus exactly one reviewed code
# generation flag. Its lock is the default 2.96 lock with only the profile ID and
# variant flags changed; this runner accepts it with exactly this flag matrix.
NOSIBCALL = 'ee-gcc2_96-nosibcall'
NOSIBCALL_FLAGS = [['-O2', '-G0', '-fno-optimize-sibling-calls'],
                   ['-O2', '-G8', '-fno-optimize-sibling-calls']]
# config/toolchains.json names it, and the profiles below are retired.
GCC296_PAYLOAD = '.work/toolchains/ee-gcc2.96-realconv-lp7-fsv12-v1/bin'
GCC296_PAYLOAD_SHA256 = {
    'cc1': '58a63b84ed4385101b1135a71919a2516f2d6c61b80258b5917ccd47bc56b1f4',
    'cpp0': 'f6f6b598f39649edfe171d6da8c7f91d07f2b87c2aeeffffbc5f566bfb1a39a9',
    'ee-gcc': 'b8c284d16c9c0a0e8788e9522c46881e224ad01fa1c732aed43f821cc32f168b',
}
# Profiles retired by the user decision of 2026-10-27 (single cc1): each selected
# another cc1 (GCC 2.9-ee-990721) or a cc1-only variant route; match() refuses them.
RETIRED_PROFILES = ('ee-gcc2_9-990721', NOSIBCALL)
RETIRED_REASON = ""


def require(ok, message):
    if not ok:
        raise ValueError(message)


def relative(value):
    require(isinstance(value, str) and re.fullmatch(r'[A-Za-z0-9_./-]+', value)
            and not value.startswith(('/', '-')) and all(p not in ('', '.', '..') for p in value.split('/')),
            'unsafe relative path')
    return Path(value)


def address(value):
    require(isinstance(value, str) and re.fullmatch(r'0x[0-9a-f]{8}', value), 'invalid VA')
    return int(value, 16)


def ident(value):
    require(isinstance(value, str) and re.fullmatch(r'[A-Za-z_][A-Za-z_0-9]*', value), 'unsafe symbol')
    return value


def extent(elf, span):
    start, size = address(span['va']), span['size']
    require(type(size) is int and 0 < size <= 1048576 and start + size <= 0x100000000, 'invalid extent size')
    sections = [s for s in elf.sections if s['flags'] & 2 and address(s['va']) <= start
                and start + size <= address(s['va']) + s['size']]
    require(len(sections) == 1, 'extent lacks unique original allocated section')
    section = sections[0]
    data = None if section['file_offset'] is None else elf.span(
        section['file_offset'] + start - address(section['va']), size, 'packet extent')
    return section, data


def ordinary_c(text, staged=None, headers=(), allow_nested=False):
    require('\x00' not in text and '??' not in text and '%:' not in text,
            'non-ordinary C encoding or alternative preprocessor tokens')
    text = re.sub(r'\\\r?\n', '', text)
    # Keep strings intact while removing comments; never reinterpret comment text as directives.
    text = re.sub(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|/\*.*?\*/|//[^\n]*',
                  lambda m: ' ' if m[0].startswith(('/*', '//')) else m[0], text, flags=re.S)
    # Comments emit no C tokens. Historical accepted comments may describe game
    # text in its original language; executable tokens and literals stay ASCII.
    require(text.isascii(), 'non-ordinary C encoding or alternative preprocessor tokens')
    # Explicit user decision 2026-10-19: a closed function-local scalar trial,
    # not packed records, arbitrary attributes, or a different compiler.
    from tools import unaligned_scalar
    text = unaligned_scalar.admission_text(text)
    require(not re.search(r'\b(?:asm|__asm|__asm__|__attribute|__attribute__|_Pragma)\b|\.incbin', text),
            'ASM, incbin, attributes or pragmas are not ordinary candidate C')
    if not allow_nested:
        nested = [name for name, depth in _gnu_nested_definitions(text) if depth > 0]
        require(not nested, 'GNU nested function definitions are not ordinary candidate C')
    for line in text.splitlines():
        if not line.lstrip().startswith('#'):
            continue
        if staged is None:  # Compiler-generated line markers only after preprocessing.
            require(re.fullmatch(r'\s*#\s*\d+\s+"[^"\n]+"(?:\s+\d+)*\s*', line), 'unexpected preprocessed directive')
            continue
        match = re.fullmatch(r'\s*#\s*include\s*["<]([A-Za-z0-9_./-]+)[">]\s*', line)
        if match:
            name = relative(match[1])
            require(any(p.as_posix() in headers for p in (staged.parent / name, Path('include') / name, name)),
                    'include is not a declared staged header')
        else:
            require(re.match(r'\s*#\s*(?:define|undef|if|ifdef|ifndef|elif|else|endif)\b', line)
                    and '##' not in line, 'unsupported preprocessor directive')


# GNU nested functions are a source-language class, not a second compiler
# contract. Original identities and lexical ownership are checked by the TU
# audit. This syntax admission does not grant unrelated GNU extensions.
_GNU_NESTED_DEF = re.compile(r'([A-Za-z_]\w*)\s*\([^;{}]*\)\s*\{')
_GNU_NESTED_KEYWORDS = frozenset(('if', 'for', 'while', 'switch', 'catch'))


def _gnu_nested_mask(text):
    from tools import tu_edit
    return tu_edit.gnu_nested_mask(text)


def _gnu_nested_definitions(text):
    """Return (name, brace-depth) for C-like function definitions in *text*."""
    from tools import tu_edit
    return [(item['name'], item['depth']) for item in tu_edit.gnu_nested_definitions(text)]


def gnu_nested_c(text, staged=None, headers=()):
    """Admit evidenced GNU nested C project-wide; no other GNU extension."""
    ordinary_c(text, staged, headers, allow_nested=True)
    from tools import tu_edit
    definitions = tu_edit.gnu_nested_definitions(text)
    nested = [item for item in definitions if item['depth'] > 0]
    require(nested, 'gnu nested-function source must contain a nested definition')
    require(all(item['parent'] is not None and item['closing'] is not None for item in nested),
            'every GNU nested definition must have a lexical parent and complete body')
    masked = _gnu_nested_mask(text)
    require(not re.search(r'\b(?:typeof|__typeof__|__extension__|__label__|__builtin_\w+)\b|\(\s*\{|\bgoto\s*\*', masked),
            'unrelated GNU constructs are not admitted by the nested-function class')
    labels = re.findall(r'(?:^|[;{}])\s*([A-Za-z_]\w*)\s*:', masked, re.M)
    require(not any(re.search(r'&&\s*' + re.escape(label) + r'\b', masked) for label in labels),
            'label addresses are not admitted by the nested-function class')
    return len(nested)


# ---------------------------------------------------------------------------
# runtime-verbatim source scope (src/runtime upstream text)
# ---------------------------------------------------------------------------
# A packet may set "source_scope": "runtime-verbatim" when its source is public
# upstream runtime text copied into src/runtime unchanged. The scope exists
# because the upstream C library and compiler runtime really are written in GNU
# C: their integer and float widths, their bitfield layouts and their noreturn
# declarations are carried by GNU attributes, and removing one would produce a
# paraphrase rather than the upstream file. Nothing else is relaxed - assembly,
# incbin, _Pragma, non-ASCII text and every attribute outside the closed list
# below stay rejected - and the scope is only reachable when
# config/runtime-provenance.json carries an entry for the source whose recorded
# upstream sha256 equals the file on disk with this project's provenance comment
# and recorded selector #defines removed. The category stays exact_c; the scope
# is reported separately so coverage can distinguish upstream text from
# project-written C. docs/ps2-capabilities.md owns the eligibility statement.
RUNTIME_VERBATIM = 'runtime-verbatim'
# Machine modes: the integer set the coordinator authorized, the two IEEE float
# modes the evidenced soft-float source declares beside them (SFtype and DFtype
# in config/fp-bit.c), and the four spellings the evidenced libgcc2.c typedef
# block uses - XF, TF, __word__ and pointer (user-authorized 2026-03-11 for the
# runtime-verbatim scope only). _runtime_plain() strips the __wrapped__ form, so
# `word` stands for `__word__` here. The list is closed: a mode not written down
# here is refused, in this scope and in every other.
_RUNTIME_MODES = frozenset(('QI', 'HI', 'SI', 'DI', 'TI', 'SF', 'DF',
                            'XF', 'TF', 'word', 'pointer'))
# `stdcall` is in this set for one reason: upstream libgcc2.c declares
# VirtualProtect with it inside `#ifdef L_trampoline` / `#if defined (WINNT) &&
# !defined (__CYGWIN__)` / `#ifdef i386`, three guards this target never takes.
# Admission reads the staged file rather than the preprocessed unit, so the
# spelling has to be admitted for the file to stay byte-identical to upstream.
# It changes no type and reaches no emitted code; docs/ps2-capabilities.md marks
# it as this implementation's choice, for a reviewer to confirm.
_RUNTIME_BARE = frozenset(('packed', 'noreturn', 'unused', 'const', 'stdcall'))
# Comments and literals are blanked before any attribute is read, so a commented
# out or quoted attribute is never mistaken for a declaration.
_RUNTIME_TOKENS = re.compile(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|/\*.*?\*/|//[^\n]*', re.S)
_RUNTIME_TRIGRAPH = re.compile(r"\?\?[=/'()!<>-]")


def _runtime_plain(token):
    """GNU spells every attribute name plainly and wrapped in double underscores."""
    return token[2:-2] if len(token) > 4 and token.startswith('__') and token.endswith('__') else token


def _runtime_attribute_item(item):
    match = re.fullmatch(r'\s*([A-Za-z_][A-Za-z_0-9]*)\s*(?:\(\s*([A-Za-z_0-9]+)\s*\))?\s*', item)
    require(match is not None, 'unsupported runtime-verbatim attribute spelling: ' + item.strip())
    name, argument = _runtime_plain(match[1]), match[2]
    if name == 'mode':
        require(argument is not None and _runtime_plain(argument) in _RUNTIME_MODES,
                'machine mode outside the runtime-verbatim allowlist: ' + item.strip())
    elif name == 'aligned':
        require(argument is not None and re.fullmatch(r'[0-9]+', argument),
                'aligned needs an explicit decimal byte count: ' + item.strip())
    else:
        require(name in _RUNTIME_BARE and argument is None,
                'attribute outside the runtime-verbatim allowlist: ' + item.strip())


def _runtime_attribute_items(inner):
    items, depth, start = [], 0, 0
    for index, character in enumerate(inner):
        if character in '([':
            depth += 1
        elif character in ')]':
            depth -= 1
        elif character == ',' and depth == 0:
            items.append(inner[start:index])
            start = index + 1
    items.append(inner[start:])
    require(all(item.strip() for item in items), 'empty runtime-verbatim attribute item')
    return items


def runtime_verbatim_attributes(text):
    """Blank every allowlisted __attribute__((...)); leave the rest for ordinary_c."""
    lexical = _RUNTIME_TOKENS.sub(
        lambda m: ''.join('\n' if c == '\n' else ' ' for c in m[0]), text)
    chars, index = list(text), 0
    while True:
        index = lexical.find('__attribute__', index)
        if index < 0:
            return ''.join(chars)
        cursor = index + len('__attribute__')
        while cursor < len(lexical) and lexical[cursor] in ' \t\n\r':
            cursor += 1
        require(lexical.startswith('((', cursor), 'attribute is not the GNU ((...)) spelling')
        depth, end = 0, cursor
        while end < len(lexical):
            if lexical[end] == '(':
                depth += 1
            elif lexical[end] == ')':
                depth -= 1
                if depth == 0:
                    break
            end += 1
        require(depth == 0 and end + 1 <= len(lexical) and lexical[end - 1] == ')',
                'unbalanced or unwrapped attribute parentheses')
        span = lexical[index:end + 1]
        require('\\' not in span and '__attribute__' not in span[len('__attribute__'):],
                'line splice or nested attribute inside a runtime-verbatim attribute')
        for item in _runtime_attribute_items(lexical[cursor + 2:end - 1]):
            _runtime_attribute_item(item)
        for position in range(index, end + 1):
            if chars[position] != '\n':
                chars[position] = ' '
        index = end + 1


def runtime_verbatim_c(text, staged=None, headers=()):
    """ordinary_c plus the closed GNU attribute allowlist of the runtime-verbatim scope."""
    require(text.isascii() and '\x00' not in text and '%:' not in text,
            'non-ordinary C encoding or alternative preprocessor tokens')
    # ordinary_c refuses every "??" on sight. Upstream runtime text writes "???"
    # editorial markers inside comments, which no trigraph spelling can be, so
    # the scope rejects the nine real trigraphs by name instead and leaves the
    # file byte-identical to upstream rather than editing its comments.
    require(not _RUNTIME_TRIGRAPH.search(text), 'ISO C trigraph spelling in runtime-verbatim source')
    ordinary_c(runtime_verbatim_attributes(text).replace('??', '? '), staged, headers)


# ---------------------------------------------------------------------------
# runtime-verbatim inline assembly (source_language "c-asm" inside the scope)
# ---------------------------------------------------------------------------
# User-authorized 2026-09-11, for verbatim upstream runtime text only: an asm
# statement that the upstream file itself contains is admitted, the packet
# carries an asm_contract citing the upstream file and lines, and the unit is
# reported as exact_c_with_asm rather than exact_c. Nothing about this reaches
# game code: the scope already requires every staged path to be under
# src/runtime and every source body to hash to its recorded upstream bytes.
#
# What is checked, in order:
#   1. every asm statement in a staged file is located lexically (comments and
#      string literals blanked first, so a commented-out or quoted `asm` is not
#      one) and must be the GNU `asm (...)`/`__asm__ (...)` statement form;
#   2. the exact source text of each statement must appear verbatim in the
#      upstream file config/runtime-provenance.json records for that staged
#      file, whose own bytes are pinned by sha256. An edited operand, a changed
#      template or a statement the upstream file does not contain fails here;
#   3. the packet's asm_contract.upstream_asm must be exactly the derived
#      {file, line, text_sha256} citation list - it cannot under- or over-claim;
#   4. the preprocessed translation unit is checked as well, and every asm
#      template in it must be one of the templates the staged files declared.
#      Macro expansion rewrites operands but never the assembly text, so this
#      is what stops an asm arriving through an undeclared macro.
# Everything ordinary_c refuses is still refused in the remainder of the file.
_RUNTIME_ASM_WORD = re.compile(r'\b(?:asm|__asm|__asm__)\b')
_RUNTIME_ASM_QUALIFIER = re.compile(r'[ \t\r\n]*(?:__volatile__|__volatile|volatile)?[ \t\r\n]*')


def _runtime_lexical(text):
    """Blank comments and literals in place, preserving offsets and newlines."""
    return _RUNTIME_TOKENS.sub(
        lambda m: ''.join('\n' if c == '\n' else ' ' for c in m[0]), text)


def _runtime_asm_spans(lexical):
    """Offsets of every GNU asm statement; raise with an exact reason otherwise."""
    spans, index = [], 0
    while True:
        found = _RUNTIME_ASM_WORD.search(lexical, index)
        if found is None:
            return spans
        cursor = _RUNTIME_ASM_QUALIFIER.match(lexical, found.end()).end()
        require(cursor < len(lexical) and lexical[cursor] == '(',
                'runtime-verbatim assembly must be the GNU asm (...) statement form')
        depth, end = 0, cursor
        while end < len(lexical):
            if lexical[end] == '(':
                depth += 1
            elif lexical[end] == ')':
                depth -= 1
                if depth == 0:
                    break
            end += 1
        require(depth == 0 and end < len(lexical),
                'unbalanced parentheses in runtime-verbatim assembly')
        spans.append((found.start(), end + 1))
        index = end + 1


def runtime_verbatim_asm(text):
    """Split a runtime-verbatim source into (C remainder, exact asm statements)."""
    chars, statements = list(text), []
    for start, end in _runtime_asm_spans(_runtime_lexical(text)):
        statements.append(text[start:end])
        for position in range(start, end):
            if chars[position] != '\n':
                chars[position] = ' '
    return ''.join(chars), statements


def runtime_verbatim_asm_template(statement):
    """The decoded assembly template of an asm statement, or None.

    None means the template section is not a plain string literal - upstream
    writes `asm (INIT_SECTION_ASM_OP)` and friends, where a macro supplies the
    text. Those cannot be compared as templates, so a staged statement like that
    contributes nothing to the declared set; the preprocessed unit, where every
    macro has already been expanded, is required to have a literal template.
    """
    statement = re.sub(r'\\\r?\n', '', statement)
    inner = statement[statement.index('(') + 1:-1]
    return _template_bodies(_split_top(inner, ':')[0])


def runtime_verbatim_asm_c(text, staged=None, headers=()):
    """runtime_verbatim_c with the GNU asm statements lifted out first.

    Returns the statements so the caller can check them against upstream; the
    remainder of the file still has to pass the ordinary runtime-verbatim rules.
    """
    require(text.isascii() and '\x00' not in text and '%:' not in text,
            'non-ordinary C encoding or alternative preprocessor tokens')
    require(not _RUNTIME_TRIGRAPH.search(text), 'ISO C trigraph spelling in runtime-verbatim source')
    require(not re.search(r'^\s*#.*\b(?:asm|__asm|__asm__)\b', text, re.M),
            'inline assembly cannot be supplied by a preprocessor directive')
    remainder, statements = runtime_verbatim_asm(text)
    ordinary_c(runtime_verbatim_attributes(remainder).replace('??', '? '), staged, headers)
    return statements


def runtime_verbatim_upstream(root, unit):
    """Resolve the unit's per-file `upstream` records: staged path -> pinned text."""
    upstream = unit.get('upstream', {})
    require(isinstance(upstream, dict), 'invalid runtime provenance upstream record')
    resolved = {}
    for path, entry in sorted(upstream.items()):
        require(isinstance(entry, dict) and set(entry) == {'local_path', 'sha256'}
                and re.fullmatch(r'[0-9a-f]{64}', str(entry.get('sha256', ''))),
                'runtime provenance upstream record must be {local_path, sha256}: ' + path)
        require(relative(path).parts[:2] == ('src', 'runtime'),
                'runtime provenance upstream record is not a staged src/runtime path: ' + path)
        local = root / relative(entry['local_path'])
        # The upstream text must come from an acquired public tree, never from the
        # published runtime source itself, or the verbatim rule would be vacuous.
        require(relative(entry['local_path']).parts[:2] != ('src', 'runtime'),
                'runtime provenance upstream record points back into src/runtime: ' + path)
        require(local.is_file() and local.resolve().is_relative_to(root)
                and cal.digest(local) == entry['sha256'],
                'recorded upstream file changed or is missing: ' + entry['local_path'])
        resolved[relative(path).as_posix()] = dict(
            local_path=relative(entry['local_path']).as_posix(), sha256=entry['sha256'],
            text=local.read_text(encoding='ascii'))
    return resolved


def runtime_verbatim_asm_evidence(path, statements, upstream):
    """Cite each staged asm statement in the upstream file, or refuse it."""
    if not statements:
        return []
    record = upstream.get(path)
    require(record is not None,
            'runtime provenance records no upstream file for the assembly in ' + path)
    text, citations = record['text'], []
    for statement in statements:
        index = text.find(statement)
        require(index >= 0, 'assembly statement is not in the recorded upstream text of '
                + record['local_path'] + ': ' + ' '.join(statement.split())[:80])
        citations.append(dict(file=record['local_path'], line=text.count('\n', 0, index) + 1,
                              text_sha256=cal.sha256(statement.encode('ascii'))))
    return citations


def runtime_verbatim_provenance(root, packet):
    """Resolve and check the config/runtime-provenance.json record for this source.

    Every staged path must be under src/runtime, exactly one provenance unit must
    claim this source file, and the file on disk must be the recorded upstream
    bytes with only this project's provenance comment block and the selector
    #defines that provenance itself records in front of them.
    """
    for entry in [packet['source'], *packet['headers']]:
        require(relative(entry['path']).parts[:2] == ('src', 'runtime'),
                'runtime-verbatim sources and headers must live under src/runtime: ' + entry['path'])
    path = root / 'config/runtime-provenance.json'
    require(path.is_file(), 'runtime-verbatim scope requires config/runtime-provenance.json')
    record = json.loads(path.read_bytes())
    require(record.get('schema_version') == 1 and isinstance(record.get('units'), dict),
            'unsupported runtime provenance schema')
    source = packet['source']['path']
    matches = sorted(name for name, unit in record['units'].items()
                     if isinstance(unit, dict) and unit.get('source') == source)
    require(len(matches) == 1, 'source has no unique config/runtime-provenance.json unit: ' + source)
    unit = record['units'][matches[0]]
    for key in ('project', 'revision', 'upstream_path', 'upstream_sha256', 'license'):
        require(isinstance(unit.get(key), str) and unit[key].strip(),
                'runtime provenance is missing ' + key + ' for ' + matches[0])
    require(re.fullmatch(r'[0-9a-f]{64}', unit['upstream_sha256']), 'invalid upstream sha256')
    selectors = unit.get('selectors', [])
    require(isinstance(selectors, list) and all(
        isinstance(s, str) and re.fullmatch(r'[A-Za-z_][A-Za-z_0-9]*(?:=[A-Za-z_0-9]+)?', s)
        for s in selectors), 'invalid runtime provenance selector list')
    text = (root / relative(source)).read_text(encoding='ascii')
    comment = re.match(r'/\*.*?\*/\n', text, re.S)
    require(comment is not None, 'runtime-verbatim source must open with the provenance comment block')
    body = text[comment.end():]
    for selector in selectors:
        name, _, value = selector.partition('=')
        line = '#define ' + name + (' ' + value if value else '') + '\n'
        require(body.startswith(line), 'declared selector is not the next line of the source: ' + selector)
        body = body[len(line):]
    require(cal.sha256(body.encode('ascii')) == unit['upstream_sha256'],
            'source body is not the recorded upstream bytes: ' + source)
    return matches[0], unit, path


# Calibrated COP2/VU0-macro inline allowlist (ee-vu-cop2 EE caller side).
# Only evidenced spellings; every line is validated with exact operand/state
# checks. vsqrt is deliberately absent: it was excluded because the legacy and
# LA29 assemblers mis-encode every accepted vsqrt spelling (0x4a0303bd emitted
# for a 0x4a2303bd original). The GCC 2.96-family assembler since 2026-09-11
# (ee-legacy-binutils-la29-vsqrt-v1) encodes it correctly, but admitting it
# inline still needs its own calibrated operand/state validation.
# (Helpers part 1 of 2.)
_WS = r'[ \t]*'
_SEP = r'[ \t]*,[ \t]*'
_MLANES = r'[xyzw]{1,4}'


def _cop2_canonical(letters):
    return letters != '' and re.fullmatch(r'x?y?z?w?', letters) is not None


def _cop2_vf(num, field, single=False):
    try:
        n = int(num, 10)
    except ValueError:
        return 'VF register number is not numeric'
    if n > 31:
        return 'VF register out of range (vf00-vf31)'
    if field and not _cop2_canonical(field):
        return 'VF field letters must be canonical (xyzw order, no repeats)'
    if single and len(field) != 1:
        return 'single broadcast/component field required here'
    return None


def _cop2_acc(field):
    if field and not _cop2_canonical(field):
        return 'ACC field letters must be canonical (xyzw order, no repeats)'
    return None


def _cop2_mask(mask):
    if mask is not None and not _cop2_canonical(mask):
        return 'lane mask must be canonical (xyzw order, no repeats)'
    return None


def _cop2_off(token):
    # Both spellings occur in the accepted sources (`0(a0)` and `0x30($17)`).
    if token.startswith('%'):
        return None
    try:
        v = int(token, 0)
    except ValueError:
        return 'lqc2/sqc2 offset is not numeric'
    if v < 0:
        return 'lqc2/sqc2 offset must not be negative'
    if v % 16:
        return 'lqc2/sqc2 offset must be 16-byte aligned'
    if v > 32752:
        return 'lqc2/sqc2 offset out of range'
    return None


def _gpr_quad_off(token):
    """Displacement of an `lq`/`sq`: a literal, 16-byte-aligned, in range.

    `lq`/`sq` fault on a misaligned effective address, so the displacement is
    part of the operation's contract and not a free constant. Both spellings the
    accepted, reviewed `.s` sources use occur (`0($2)` and `0x30($19)`), and
    every one of the 66 quadword displacements in that corpus is a non-negative
    multiple of 16; the 16-bit signed field is the hardware bound.
    """
    try:
        v = int(token, 0)
    except ValueError:
        return 'lq/sq displacement is not numeric'
    if v < 0:
        return 'lq/sq displacement must not be negative'
    if v % 16:
        return 'lq/sq displacement must be 16-byte aligned'
    if v > 32752:
        return 'lq/sq displacement out of range'
    return None


def _cop2_imm15(token):
    try:
        v = int(token, 0)
    except ValueError:
        return 'vcallms target is not a numeric address'
    if v > 0x7fff:
        return 'vcallms target out of range (15-bit CMSAR address)'
    return None


# Broadcast component of every mnemonic whose last letter selects one lane of
# the third operand. The set is closed and derived from the accepted, reviewed
# `.s` sources under `src/<unit>/<tu>/`: every combination below occurs there, so
# every one is already assembled to the original bytes by the pinned assembler
# under the whole-file gate. `vmsubw` and `vmaddax` are the two members of these
# families that no accepted source uses, and they stay refused for that reason
# alone -- an attempt that evidences one adds it here with the same rigour.
_BC_ALU = ('vaddw', 'vaddx', 'vaddy', 'vaddz',
           'vsubw', 'vsubx', 'vsuby', 'vsubz',
           'vmulw', 'vmulx', 'vmuly', 'vmulz',
           'vmaddw', 'vmaddx', 'vmaddy', 'vmaddz',
           'vmsubx', 'vmsuby', 'vmsubz')
_BC_ACC = ('vmulaw', 'vmulax', 'vmulay', 'vmulaz',
           'vmaddaw', 'vmadday', 'vmaddaz')
# Broadcast min/max members calibrated on 2026-09-23 against the ORIGINAL
# binary rather than an accepted source (see `_ORIGINAL_CALIBRATION` below):
# each spelling was assembled by the contract assembler and compared with the
# original instruction word at every site the scaffold shows. Only the three
# lane combinations the original actually uses are admitted, for the same
# reason `vmsubw`/`vmaddax` stay out of the tuples above.
_BC_MINMAX = ('vmaxw', 'vmaxx', 'vminiy')
_BC_OF = {op: op[-1] for op in _BC_ALU + _BC_ACC + _BC_MINMAX}


def _cop2_field(mask, field, what):
    """A register's lane letters must be absent or spell the destination mask.

    The encoding carries only the destination mask and the broadcast lane, so
    the letters written after a source register are decoration -- which is
    exactly why they have to agree with the mask. Both spellings occur in the
    accepted sources (`vadd.xyz $vf1, $vf1, $vf2` and
    `vadd.xyz vf1xyz,vf1xyz,vf2xyz`); a third one that says `xyz` in the mask and
    `xy` on an operand says two different things about the same instruction.
    """
    if not field:
        return None
    if not _cop2_canonical(field):
        return '%s field letters must be canonical (xyzw order, no repeats)' % what
    if field != (mask or ''):
        return ('%s field letters %s do not match the destination mask %s'
                % (what, field, mask or '(none)'))
    return None


def _check_masked_3op(m):
    """vadd/vsub/vmul: three VF registers under one destination mask."""
    err = _cop2_mask(m.group('mask'))
    if err:
        return err
    for key, what in (('d', 'destination'), ('s', 'first source'), ('t', 'second source')):
        err = _cop2_vf(m.group(key), '') or _cop2_field(m.group('mask'), m.group(key + 'f'), what)
        if err:
            return err
    return None


def _check_bc_3op(m):
    """vaddbc/vsubbc/vmulbc/vmaddbc/vmsubbc: the third operand is one lane."""
    err = _cop2_mask(m.group('mask'))
    if err:
        return err
    for key, what in (('d', 'destination'), ('s', 'first source')):
        err = _cop2_vf(m.group(key), '') or _cop2_field(m.group('mask'), m.group(key + 'f'), what)
        if err:
            return err
    err = _cop2_vf(m.group('t'), m.group('tf'), single=True)
    if err:
        return err
    if m.group('tf') != _BC_OF[m.group('op')]:
        return 'broadcast component must match the mnemonic (%s)' % _BC_OF[m.group('op')]
    return None


def _check_acc_bc_3op(m):
    """vmulabc/vmaddabc: the same, writing the accumulator instead of a VF."""
    err = _cop2_mask(m.group('mask'))
    if err:
        return err
    err = _cop2_field(m.group('mask'), m.group('df'), 'ACC')
    if err:
        return err
    err = _cop2_vf(m.group('s'), '') or _cop2_field(m.group('mask'), m.group('sf'), 'first source')
    if err:
        return err
    err = _cop2_vf(m.group('t'), m.group('tf'), single=True)
    if err:
        return err
    if m.group('tf') != _BC_OF[m.group('op')]:
        return 'broadcast component must match the mnemonic (%s)' % _BC_OF[m.group('op')]
    return None


def _check_move_2op(m):
    """vmove/vmr32/vitof0/vitof4/vftoi0: one masked VF-to-VF operation."""
    err = _cop2_mask(m.group('mask'))
    if err:
        return err
    for key, what in (('d', 'destination'), ('s', 'source')):
        err = _cop2_vf(m.group(key), '') or _cop2_field(m.group('mask'), m.group(key + 'f'), what)
        if err:
            return err
    return None


def _check_q_2op(m):
    """vaddq/vmulq: a masked VF operation whose third operand is the Q register."""
    err = _cop2_mask(m.group('mask'))
    if err:
        return err
    for key, what in (('d', 'destination'), ('s', 'source')):
        err = _cop2_vf(m.group(key), '') or _cop2_field(m.group('mask'), m.group(key + 'f'), what)
        if err:
            return err
    return None


# ---------------------------------------------------------------------------
# VU0 macro families calibrated against the ORIGINAL binary (2026-09-23).
#
# The families above were derived from accepted, reviewed sources. The ones
# below were missing from that corpus, so census candidates that are
# byte-identical under the whole-file gate were refused at audit with
# "uncalibrated COP2 inline spelling". Each family was calibrated directly
# against SLUS_204.69 / its overlays: every site where the scaffold asm
# (`build/*/asm/**/nonmatchings/**/*.s`, comment `/* offset VA BYTES */`,
# bytes little-endian) shows the family was assembled with the contract
# assembler (ee-legacy-binutils-la29-vsqrt-v1 `ee-as -EL -m5900 -mabi=eabi`)
# in three spellings -- the splat one (`$vf1x`), the bare one (`vf1x`) and the
# bare one without spaces after commas -- and compared word for word with the
# original. Evidence: .work/cop2-calibration-20260923/calibration.json.
#
#   family    sites  spellings  equal  mismatched   admitted as
#   vdiv        31      93        93       0        `vdiv`       (new)
#   vclipw      15      60        60       0        `clipw`      (new)
#   vftoi4      18      54        54       0        `move2`      (extended)
#   vmadd       15      45        45       0        `plain3`     (extended)
#   vmuli        8      24        24       0        `imove`      (new)
#   vopmula      5      15        15       0        `accplain3`  (new)
#   vopmsub      5      15        15       0        `opmsub`     (new)
#   vmula        4      12        12       0        `accplain3`  (new)
#   vrinit       2       6         6       0        `rinit`      (new)
#   vmadda       1       3         3       0        `accplain3`  (new)
#   vmaddq       1       3         3       0        `qmove`      (extended)
#   vmax         1       3         3       0        `plain3`     (extended)
#   vmini        1       3         3       0        `plain3`     (extended)
#   vmaxw        1       3         3       0        `bc3`        (extended)
#   vmaxx        1       3         3       0        `bc3`        (extended)
#   vminiy       1       3         3       0        `bc3`        (extended)
#   vrnext       1       3         3       0        `rnext`      (new)
#
# (vclipw has a fourth spelling, the first source without its `xyz` letters.)
# The assembler refuses (errors, emits nothing) every mis-shaped probe tried:
# a multi-lane or lane-less vdiv/vrinit operand, vopmula/vopmsub with a mask
# other than `xyz`, vclipw with a non-`w` second operand, a broadcast lane that
# disagrees with the mnemonic, and Q/I/R swapped between families. It ACCEPTS
# `vclipw.xyzw` (0x4be021ff), which the original never uses and the hardware
# defines only for `xyz`, so the table enforces the `xyz` mask itself.
#
# Refused although the original uses no other spelling of them: nothing -- all
# seventeen families the scaffold shows and the table lacked calibrate exactly.
# VU0 macro families the original does not use (vrget, vrxor, vabs, vmulaq,
# vaddai, viadd, vlqi/vsqi, vilwr/viswr, vftoi12/15, vitof12/15, ...) stay
# refused: there is no original word to calibrate them against. The whole-file
# gate stays the authority on bytes; this table only decides eligibility.
_ORIGINAL_CALIBRATION = '.work/cop2-calibration-20260923/calibration.json'


def _check_acc_3op(m):
    """vmula/vmadda/vopmula: ACC = vfs (op) vft under one destination mask.

    `vopmula` is the first half of the outer product and the hardware defines
    it only for `xyz`; every original site uses exactly that mask.
    """
    err = _cop2_mask(m.group('mask'))
    if err:
        return err
    if m.group('op') == 'vopmula' and m.group('mask') != 'xyz':
        return 'vopmula takes exactly the .xyz destination mask'
    err = _cop2_field(m.group('mask'), m.group('df'), 'ACC')
    if err:
        return err
    for key, what in (('s', 'first source'), ('t', 'second source')):
        err = _cop2_vf(m.group(key), '') or _cop2_field(m.group('mask'), m.group(key + 'f'), what)
        if err:
            return err
    return None


def _check_opmsub(m):
    """vopmsub: the second half of the outer product, `.xyz` only."""
    if m.group('mask') != 'xyz':
        return 'vopmsub takes exactly the .xyz destination mask'
    return _check_masked_3op(m)


def _check_i_2op(m):
    """vmuli: a masked VF operation whose third operand is the I register."""
    return _check_q_2op(m)


def _check_div(m):
    """vdiv Q, vfs<f>, vft<f>: each source names exactly one lane."""
    return (_cop2_vf(m.group('s'), m.group('sf'), single=True) or
            _cop2_vf(m.group('t'), m.group('tf'), single=True))


def _check_clipw(m):
    """vclipw.xyz vfs[xyz], vftw: the clip judgement is defined only for xyz
    against one w lane. The assembler would accept `.xyzw`; the original never
    writes it and the hardware does not define it, so it is refused here."""
    if m.group('mask') != 'xyz':
        return 'vclipw takes exactly the .xyz destination mask'
    err = _cop2_vf(m.group('s'), '') or _cop2_field(m.group('mask'), m.group('sf'), 'first source')
    if err:
        return err
    err = _cop2_vf(m.group('t'), m.group('tf'), single=True)
    if err:
        return err
    if m.group('tf') != 'w':
        return 'vclipw compares against the w lane of its second operand'
    return None


def _check_rnext(m):
    """vrnext.<mask> vft, R: the destination letters must spell the mask."""
    err = _cop2_mask(m.group('mask'))
    if err:
        return err
    return _cop2_vf(m.group('d'), '') or _cop2_field(m.group('mask'), m.group('df'), 'destination')


def _vf3(op):
    return (r'(?P<op>' + op + r')(?:\.(?P<mask>' + _MLANES + r'))?' + _WS +
            r'\$?vf(?P<d>[0-9]+)(?P<df>[xyzw]*)' + _SEP +
            r'\$?vf(?P<s>[0-9]+)(?P<sf>[xyzw]*)' + _SEP +
            r'\$?vf(?P<t>[0-9]+)(?P<tf>[xyzw]*)')


def _cop2_vi(num):
    try:
        n = int(num, 10)
    except ValueError:
        return 'VI register number is not numeric'
    if n > 31:
        return 'VI register out of range (vi00-vi31)'
    return None


def _cop2_imm5(token):
    try:
        v = int(token, 0)
    except ValueError:
        return 'viaddi immediate is not numeric'
    if not -16 <= v <= 15:
        return 'viaddi immediate out of range (5-bit signed)'
    return None


# The R5900 GPR quadword load/store as the original wrote it inline: a
# hard-coded scratch GPR carries the 128 bits, and the address comes from a C
# operand. `_cop2_parse_construct` additionally requires the scratch register to
# be declared in the clobber list. See `_line_patterns` for why the spelling is
# this narrow.
_GPR_QUAD = re.compile(r'(?P<op>lq|sq)' + _WS + r'\$(?P<gpr>[0-9]|[12][0-9]|3[01])' + _SEP +
                       r'(?P<off>0[xX][0-9a-fA-F]+|[0-9]+)' + _WS + r'\(' + _WS +
                       r'%(?P<base>[0-9])' + _WS + r'\)')


# R5900 unaligned doubleword load pair and the MMI packed-halfword widening
# (user-authorized 2026-09-13). These are not COP2 instructions. They are
# admitted for one evidenced idiom and nothing else: the `sefLerp*Vector*`
# family reads three packed signed halfwords with an `ldl`/`ldr` pair, widens
# them to words with `pcgth` against zero followed by `pextlh`, and hands the
# quadword to COP2 with `qmtc2`.
#
# The census is the argument for how narrow this is. Across the 329,113-line
# disassembly of SLUS_204.69's .text, `pcgth` occurs 16 times and `pextlh` 10,
# and all 26 sites are in just two families: the six sefLerp*Vector* helpers,
# which all reuse this one hand-written idiom, and six SDK routines at
# 0x00211de0..0x00212320 that are out of recovery scope. cc1 2.96 emits no MMI
# for scalar or aggregate C at all (config/compiler-patterns.json CP-0169), so
# a candidate containing these is reproducing the original's own inline
# assembly, not hand-scheduling compiler work.
#
# The spellings follow the `gprquad` reasoning exactly:
#
#   * data registers are hard-coded `$N` and never `%` operands, because the
#     idiom pins its scratch registers; an allocator-chosen register is what
#     compiler output looks like;
#   * an `ldl`/`ldr` base is a `%N` operand and never a hard-coded `$N`,
#     because the address is a C expression the compiler computes;
#   * the displacements carry the little-endian unaligned-doubleword
#     convention (`ldl` at +7 of the doubleword, `ldr` at +0), which is what
#     both accepted sources use and what makes the pair a doubleword load
#     rather than an arbitrary unaligned access;
#   * `pcgth`'s second source must be `$0`: the evidenced operation is a sign
#     mask, not a general packed comparison;
#   * `pextlh` and `dsrl` write back to their own first source, which is what
#     all four accepted occurrences do.
#
# `sdl`/`sdr` and every other MMI instruction stay refused: none appears in the
# accepted corpus, so none has evidence yet.
_MMI_LOAD = re.compile(r'(?P<op>ldl|ldr)' + _WS + r'\$(?P<gpr>[0-9]|[12][0-9]|3[01])' + _SEP +
                       r'(?P<off>0[xX][0-9a-fA-F]+|[0-9]+)' + _WS + r'\(' + _WS +
                       r'%(?P<base>[0-9])' + _WS + r'\)')
_MMI_CMP = re.compile(r'(?P<op>pcgth)' + _WS + r'\$(?P<d>[0-9]|[12][0-9]|3[01])' + _SEP +
                      r'\$(?P<s>0)' + _SEP + r'\$(?P<t>[0-9]|[12][0-9]|3[01])')
_MMI_EXT = re.compile(r'(?P<op>pextlh)' + _WS + r'\$(?P<d>[0-9]|[12][0-9]|3[01])' + _SEP +
                      r'\$(?P<s>[0-9]|[12][0-9]|3[01])' + _SEP +
                      r'\$(?P<t>[0-9]|[12][0-9]|3[01])')
_MMI_SHIFT = re.compile(r'(?P<op>dsrl)' + _WS + r'\$(?P<d>[0-9]|[12][0-9]|3[01])' + _SEP +
                        r'\$(?P<s>[0-9]|[12][0-9]|3[01])' + _SEP +
                        r'(?P<imm>0[xX][0-9a-fA-F]+|[0-9]+)')

# The VU0 colour-clamp tail of main/tu097 ConvertYUV2RGB: after the VU0
# conversion, `qmfc2` moves the result and the clamp bounds to GPRs and the
# block ends
#
#     pmaxw $3,$2,$8     ; clamp each word lane below (parallel max word)
#     pminw $2,$3,$9     ; clamp each word lane above (parallel min word)
#     ppach $3,$0,$2     ; pack the four words to halfwords
#     ppacb $2,$0,$3     ; pack the halfwords to bytes
#     sw    $2,0(%1)     ; store the RGBA pixel through the C pointer operand
#
# Admitted in this calibrated table by user decisions of 2026-10-28: `pmaxw`
# first ("Actualiza el pin y acepta esa instrucción pmaxw"), then the other four
# lines ("Vale, acepta esas otras instrucciones"). They are admitted for every
# `hardware` function, but each entry is only as wide as this evidence.
#
# Calibration against the original (SLUS_204.69 0x002252cc..0x002252dc, the
# only `pmaxw`/`pminw`/`ppach` sites of the main ELF; no `pmaxw` in the
# overlays): the contract assembler (ee-legacy-binutils-la29-vsqrt-nolabel-
# laimm-v1 and its -swap twin, `-EL -m5900 -mabi=eabi`) encodes the five lines
# to 0x704818c8, 0x706910e8, 0x70021dc8, 0x700316c8 and, with `%1` allocated to
# $10 as the published object does, 0xad420000 -- word for word the original.
# Evidence: .work/tool-edit-pmaxw-20261028/calib/ and
# .work/tool-edit-pmmi2-20261028/calib/.
#
# The spellings follow the other MMI entries of this table:
#
#   * `mmiminmax` (`pmaxw`/`pminw`): three hard-coded `$N` GPRs, never `%N`;
#   * `mmipack` (`ppach`/`ppacb`): hard-coded `$N`, and the first source is
#     `$0`, which is the evidenced use (the upper half of the packed result is
#     zero); any other first source stays refused;
#   * `gprstore` (`sw`): the data register is a hard-coded `$N` other than
#     `$0`, the base is a `%N` C pointer operand (never a hard-coded base:
#     a scalar store through a register the compiler did not compute is
#     compiler output, not hardware), and the displacement is a literal,
#     non-negative, word-aligned 16-bit offset. COP2 blocks already must
#     declare `"memory"`.
#
# For all three, a written destination is a hard-coded scratch that
# `_cop2_quadword_scratch` requires in the clobber list (never `$0`), and
# `_cop2_block_scratch_reads` requires every hard-coded register they READ to
# have been written earlier in the same block, so they only ever consume the
# block's own values (e.g. what `qmfc2` brought out of VU0), never live
# compiler state. None of them is one of `_MMI_KINDS` (the packed-halfword
# widening), and a block made only of them is not COP2 content. The rest of the
# MMI P* family and every other scalar line stay outside this table (MMI is
# admitted generally only under the `ee-mmi` class, docs/ee-mmi.md); the gate
# stays the authority on bytes.
_GPR_NUM = r'([0-9]|[12][0-9]|3[01])'
_MMI_MINMAX = re.compile(r'(?P<op>pmaxw|pminw)' + _WS + r'\$(?P<d>' + _GPR_NUM + r')' + _SEP +
                         r'\$(?P<s>' + _GPR_NUM + r')' + _SEP + r'\$(?P<t>' + _GPR_NUM + r')')
_MMI_PACK = re.compile(r'(?P<op>ppach|ppacb)' + _WS + r'\$(?P<d>' + _GPR_NUM + r')' + _SEP +
                       r'\$(?P<s>0)' + _SEP + r'\$(?P<t>' + _GPR_NUM + r')')
_GPR_STORE = re.compile(r'(?P<op>sw)' + _WS + r'\$(?P<t>' + _GPR_NUM + r')' + _SEP +
                        r'(?P<off>0[xX][0-9a-fA-F]+|[0-9]+)' + _WS + r'\(' + _WS +
                        r'(?P<base>%[0-9])' + _WS + r'\)')
# Calibrated entries that are not COP2/VU instructions and not part of the
# widening idiom; like `gprquad`, they never make a block COP2 content on
# their own. The hard-coded registers they read must be block-local scratch.
_MMI_ARITH_KINDS = ('mmiminmax', 'mmipack', 'gprstore')


def _gpr_store_check(m):
    """`sw $N,OFF(%N)`: a real data register and a word-aligned literal offset."""
    if m.group('t') == '0':
        return 'the admitted sw stores a block-computed register, never $0'
    try:
        v = int(m.group('off'), 0)
    except ValueError:
        return 'sw displacement is not numeric'
    if v > 0x7fff:
        return 'sw displacement out of 16-bit range'
    if v % 4:
        return 'sw displacement must be word aligned'
    return None


def _mmi_load_off(op, token):
    """Displacement of an `ldl`/`ldr`: the little-endian doubleword convention.

    On the EE, the pair that loads one unaligned doubleword is `ldl` at the
    high byte of the doubleword and `ldr` at its base, seven apart. Both
    accepted sources spell it `ldl $N,7(base)` / `ldr $N,0(base)`. Requiring
    `ldl` at 7 modulo 8 and `ldr` at 0 modulo 8 keeps the admission to that
    operation instead of an arbitrary unaligned byte access, and the 16-bit
    signed field is the hardware bound.
    """
    try:
        v = int(token, 0)
    except ValueError:
        return 'ldl/ldr displacement is not numeric'
    if v < 0:
        return 'ldl/ldr displacement must not be negative'
    if v > 0x7fff:
        return 'ldl/ldr displacement out of 16-bit range'
    want = 7 if op == 'ldl' else 0
    if v % 8 != want:
        return ('%s displacement must be %d modulo 8: the evidenced pair loads one '
                'unaligned doubleword (ldl at +7, ldr at +0)' % (op, want))
    return None


def _mmi_dest(reg):
    return 'MMI destination register must not be $0' if reg == '0' else None


def _mmi_same(d, s, op):
    return (None if d == s else
            '%s must write back to its own first source ($%s vs $%s): that is the '
            'evidenced idiom' % (op, d, s))


def _mmi_shift_amount(token):
    try:
        v = int(token, 0)
    except ValueError:
        return 'dsrl shift amount is not numeric'
    return None if 0 <= v <= 31 else 'dsrl shift amount out of range (0-31)'


# The operand each admitted entry WRITES, when that operand can be a GPR. `None`
# means the entry writes no GPR at all (it writes a VF/VI/FPR, or only memory).
# `_line_patterns` asserts every entry appears here, so a new spelling cannot be
# added without stating its direction.
_GPR_DEST_GROUP = {
    'transfer': None,    # lqc2/sqc2: the GPR, when hard-coded, is the address base
    'move': 'gpr',       # qmfc2 writes the GPR; qmtc2 reads it (_GPR_READ_ONLY)
    'fpmove': 'gpr',     # mfc1 writes the GPR; mtc1 reads it
    'control': 'gpr',    # cfc2 writes the GPR; ctc2 reads it
    'plain3': None, 'bc3': None, 'accbc3': None, 'move2': None, 'qmove': None,
    'rqsrt': None, 'sqrt': None, 'vi3': None, 'vimove': None, 'barrier': None,
    'callms': None, 'callmsr': None,
    # VU0 macro families calibrated against the original on 2026-09-23: they
    # write VF registers, ACC, Q, R or the clip flag, never a GPR.
    'accplain3': None, 'opmsub': None, 'imove': None, 'vdiv': None,
    'clipw': None, 'rinit': None, 'rnext': None,
    'gprquad': 'gpr',    # lq writes the GPR; sq reads it
    'mmiload': 'gpr', 'mmicmp': 'd', 'mmiext': 'd', 'mmishift': 'd',
    'mmiminmax': 'd', 'mmipack': 'd',   # pmaxw/pminw, ppach/ppacb (user decisions 2026-10-28)
    'gprstore': None,    # sw writes only memory; its data GPR is read (_GPR_READ_ONLY)
}


def _line_patterns():
    """The calibrated hardware line table.

    Every COP2/VU0-macro family below is derived from the instructions the
    accepted, reviewed standalone sources under `src/<unit>/<tu>/` actually
    contain, or -- for the families added on 2026-09-23 (`accplain3`,
    `opmsub`, `imove`, `vdiv`, `clipw`, `rinit`, `rnext`, and `vmadd`/`vmini`/
    `vmax`/`vftoi4`/`vmaddq`/`vmaxw`/`vmaxx`/`vminiy` in existing entries) --
    calibrated word for word against the original binary with the contract
    assembler (`_ORIGINAL_CALIBRATION`). Each line is validated for register
    range, lane masks, field agreement, broadcast component and immediate range.

    Three entries are not COP2 instructions, and each carries its own measured
    reason: the `mfc1`/`mtc1` pair, which is the only way a C float reaches VU0
    macro mode; the R5900 GPR quadword load/store, which the original performed
    through inline assembly (see the `gprquad` entry below); and, outside this
    table because it is a multi-line group rather than a line, the null-default
    idiom in `_cop2_null_default`, which is the ONE admitted non-hardware
    idiom.

    What the same sources contain and this table still refuses is everything
    else that is not a hardware operation: the rest of the scalar code (`sw`,
    `lw`, `addu`, `sll`, `daddu`, a bare `lui`/`addiu`/`movz`/`movn`), all the
    other MMI (the widening idiom's lines and, since 2026-10-28, ConvertYUV2RGB's
    `pmaxw`/`pminw`/`ppach`/`ppacb` and its `sw $N,OFF(%N)` store -- `mmiminmax`,
    `mmipack`, `gprstore` -- are the only MMI/scalar entries) and `ldl`/`ldr`/`sdl`/`sdr`, the standalone spelling of `lq`/`sq`
    with a hard-coded base (that is the compiler-emitted 128-bit copy, and it
    stays on the native-TI source route), the branches and labels, the
    assembler mode directives (`.set noreorder`, `.set nomacro`, `.set noat`)
    and the delay-slot `nop`. Those are the compiler's and the assembler's
    output around the hardware operations, and in this route they stay in C.
    """
    table = []
    gpr = r'(?P<gpr>%[0-9]|\$(?:[0-9]|[12][0-9]|3[01]))'
    table.append(('transfer',
                  re.compile(r'(?P<op>lqc2|sqc2)' + _WS + r'\$?vf(?P<d>[0-9]+)' + _SEP +
                             r'(?P<off>0[xX][0-9a-fA-F]+|[0-9]+|%[0-9])' + _WS + r'\(' + _WS +
                             r'(?P<base>%[0-9]|\$(?:[0-9]|[12][0-9]|3[01]))' + _WS + r'\)'),
                  lambda m: _cop2_vf(m.group('d'), '') or _cop2_off(m.group('off'))))
    # GPR <-> VF quadword move. `.i` waits for VU0 (interlocked), `.ni` does not;
    # both spellings occur in the accepted sources, and which one an original
    # used is part of the state contract, not a detail the table may normalise.
    table.append(('move',
                  re.compile(r'(?P<op>qmtc2|qmfc2)(?P<mode>\.n?i)?' + _WS + gpr + _SEP +
                             r'\$?vf(?P<src>[0-9]+)'),
                  lambda m: _cop2_vf(m.group('src'), '')))
    # COP1 <-> GPR move. This is the ONLY COP1 pair admitted, and it is admitted
    # because it is the interface of a VU0 macro-mode call rather than general
    # floating-point code: the EABI keeps a C `float` in an FPR, `qmtc2`/`qmfc2`
    # read and write a GPR, and there is no other way to hand a C float to VU0 or
    # take one back. Every other COP1 instruction the accepted sources contain
    # (`add.s`, `mul.s`, `sub.s`, `div.s`, `neg.s`, `mov.s`, `cvt.s.w`, `c.lt.s`,
    # `c.eq.s`, `sqrt.s`, `lwc1`, `swc1`) is arithmetic the C says, and stays
    # refused.
    table.append(('fpmove',
                  re.compile(r'(?P<op>mfc1|mtc1)' + _WS + gpr + _SEP +
                             r'(?P<fpr>%[0-9]|\$f(?P<fnum>[0-9]+))'),
                  lambda m: (None if m.group('fnum') is None
                             else ('FPU register out of range ($f0-$f31)'
                                   if int(m.group('fnum'), 10) > 31 else None))))
    # GPR <-> VI/control move, same interlock spellings.
    table.append(('control',
                  re.compile(r'(?P<op>ctc2|cfc2)(?P<mode>\.n?i)?' + _WS + gpr + _SEP +
                             r'\$?vi(?P<vi>[0-9]+)'),
                  lambda m: _cop2_vi(m.group('vi'))))
    # `vmadd`, `vmini` and `vmax` joined 2026-09-23 (`_ORIGINAL_CALIBRATION`).
    table.append(('plain3', re.compile(_vf3('vadd|vsub|vmul|vmadd|vmini|vmax')),
                  _check_masked_3op))
    table.append(('bc3', re.compile(_vf3('|'.join(_BC_ALU + _BC_MINMAX))), _check_bc_3op))
    table.append(('accbc3',
                  re.compile(r'(?P<op>' + '|'.join(_BC_ACC) + r')(?:\.(?P<mask>' + _MLANES +
                             r'))?' + _WS + r'ACC(?P<df>[xyzw]*)' + _SEP +
                             r'\$?vf(?P<s>[0-9]+)(?P<sf>[xyzw]*)' + _SEP +
                             r'\$?vf(?P<t>[0-9]+)(?P<tf>[xyzw]*)'),
                  _check_acc_bc_3op))
    table.append(('move2',
                  re.compile(r'(?P<op>vmove|vmr32|vitof0|vitof4|vftoi0|vftoi4)(?:\.(?P<mask>' +
                             _MLANES + r'))?' + _WS +
                             r'\$?vf(?P<d>[0-9]+)(?P<df>[xyzw]*)' + _SEP +
                             r'\$?vf(?P<s>[0-9]+)(?P<sf>[xyzw]*)'),
                  _check_move_2op))
    table.append(('qmove',
                  re.compile(r'(?P<op>vaddq|vmulq|vmaddq)(?:\.(?P<mask>' + _MLANES + r'))?' + _WS +
                             r'\$?vf(?P<d>[0-9]+)(?P<df>[xyzw]*)' + _SEP +
                             r'\$?vf(?P<s>[0-9]+)(?P<sf>[xyzw]*)' + _SEP + r'Q'),
                  _check_q_2op))
    table.append(('rqsrt',
                  re.compile(r'vrsqrt' + _WS + r'Q' + _SEP +
                             r'\$?vf(?P<s>[0-9]+)(?P<sf>[xyzw]*)' + _SEP +
                             r'\$?vf(?P<t>[0-9]+)(?P<tf>[xyzw]*)'),
                  lambda m: (_cop2_vf(m.group('s'), m.group('sf'), single=True) or
                             _cop2_vf(m.group('t'), m.group('tf'), single=True))))
    # vsqrt was excluded while the only assemblers available mis-encoded it
    # (0x4a0303bd emitted for a 0x4a2303bd original). The contract assembler
    # since 2026-09-11 (ee-legacy-binutils-la29-vsqrt-v1) encodes it correctly,
    # and nine vsqrt lines in the accepted sources are assembled by it under the
    # whole-file gate, so the remaining condition -- its own calibrated operand
    # validation -- is what this entry adds. The gate stays the authority: a
    # mis-encoding assembler produces different bytes and fails it.
    table.append(('sqrt',
                  re.compile(r'vsqrt' + _WS + r'Q' + _SEP +
                             r'\$?vf(?P<s>[0-9]+)(?P<sf>[xyzw]*)'),
                  lambda m: _cop2_vf(m.group('s'), m.group('sf'), single=True)))
    table.append(('vi3',
                  re.compile(r'(?P<op>viaddi)' + _WS + r'\$?vi(?P<d>[0-9]+)' + _SEP +
                             r'\$?vi(?P<s>[0-9]+)' + _SEP +
                             r'(?P<imm>-?(?:0[xX][0-9a-fA-F]+|[0-9]+))'),
                  lambda m: (_cop2_vi(m.group('d')) or _cop2_vi(m.group('s')) or
                             _cop2_imm5(m.group('imm')))))
    table.append(('vimove',
                  re.compile(r'(?P<op>vmfir)(?:\.(?P<mask>' + _MLANES + r'))?' + _WS +
                             r'\$?vf(?P<d>[0-9]+)(?P<df>[xyzw]*)' + _SEP +
                             r'\$?vi(?P<s>[0-9]+)'),
                  lambda m: (_cop2_mask(m.group('mask')) or _cop2_vf(m.group('d'), '') or
                             _cop2_field(m.group('mask'), m.group('df'), 'destination') or
                             _cop2_vi(m.group('s')))))
    # vwaitq and vnop are VU0-macro instructions: one waits for the Q pipeline,
    # the other occupies a VU slot. A scalar `nop` is neither, and is refused --
    # see hardware_c.
    table.append(('barrier', re.compile(r'(?P<op>vwaitq|vnop)'), lambda m: None))
    table.append(('callms',
                  re.compile(r'vcallms' + _WS + r'(?P<imm>0[xX][0-9a-fA-F]+|[0-9]+)'),
                  lambda m: _cop2_imm15(m.group('imm'))))
    # R5900 GPR quadword load/store (`lq`/`sq`). This is not a COP2
    # instruction: it is the 128-bit GPR move the SDK's `sceVu0CopyVector`
    # performs, and it is admitted because the ORIGINAL performed it through
    # inline assembly, not because a 128-bit copy needs a machine instruction.
    #
    # Measured on ov01/tu028 `fnMSP02_PR000`, which is byte-identical with three
    # such copies: the original forces both addresses into registers with
    # `addiu` and moves the quadword through a hard-coded `$8`
    # (`addiu $2,$19,176; addiu $17,$19,160; lq $8,0($17); sq $8,0($2)`). A
    # native-TI (`typedef ... mode(TI)`) copy of the same two fields is a
    # different instruction stream -- `lq $2,160($19)` / `sq $2,176($19)`, the
    # offsets folded into the base and the register chosen by the allocator --
    # so the two forms are distinguishable in the bytes, and ov01/tu030 is the
    # sibling whose `lq`/`sq` really are compiler-emitted 16-byte copies and
    # which therefore needs `--source-class native-ti` instead.
    #
    # That distinction is what the spelling enforces, and it is why this entry
    # is narrower than the COP2 ones above:
    #
    #   * the data register is a hard-coded `$N` and never an operand, because
    #     the evidenced form pins the SDK's scratch register; an allocator-
    #     chosen register is what compiler output looks like;
    #   * the base register is a `%N` operand and never a hard-coded `$N`,
    #     because the address is a C expression the compiler computes -- a
    #     hand-written `lq $2,160($19)` is exactly the compiler-emitted
    #     aggregate copy that stays on the native-TI route (docs/native-ti.md);
    #   * the displacement is a literal, non-negative, 16-byte-aligned constant
    #     in range (`_gpr_quad_off`), because `lq`/`sq` fault otherwise.
    #
    # The whole `lq`/`sq` family beyond this shape stays refused. The packed
    # integer question this comment used to defer -- "a different eligibility
    # question with its own evidence" -- was answered on 2026-09-13: the
    # evidence was gathered (CP-0169) and the user authorized one idiom, the
    # `ldl`/`ldr` + `pcgth`/`pextlh` packed-halfword widening below. Every
    # other MMI instruction, and `sdl`/`sdr`, stay refused: none appears in the
    # accepted corpus, so none has evidence yet.
    table.append(('gprquad', _GPR_QUAD, lambda m: _gpr_quad_off(m.group('off'))))
    table.append(('mmiload', _MMI_LOAD,
                  lambda m: (_mmi_dest(m.group('gpr')) or
                             _mmi_load_off(m.group('op'), m.group('off')))))
    table.append(('mmicmp', _MMI_CMP, lambda m: _mmi_dest(m.group('d'))))
    table.append(('mmiext', _MMI_EXT,
                  lambda m: (_mmi_dest(m.group('d')) or
                             _mmi_same(m.group('d'), m.group('s'), 'pextlh'))))
    table.append(('mmishift', _MMI_SHIFT,
                  lambda m: (_mmi_dest(m.group('d')) or
                             _mmi_same(m.group('d'), m.group('s'), 'dsrl') or
                             _mmi_shift_amount(m.group('imm')))))
    # ConvertYUV2RGB's clamp/pack/store tail, calibrated against the original
    # (user decisions 2026-10-28). See `_MMI_MINMAX`.
    table.append(('mmiminmax', _MMI_MINMAX, lambda m: _mmi_dest(m.group('d'))))
    table.append(('mmipack', _MMI_PACK, lambda m: _mmi_dest(m.group('d'))))
    table.append(('gprstore', _GPR_STORE, _gpr_store_check))
    table.append(('callmsr',
                  re.compile(r'vcallmsr' + _WS + r'\$?vi(?P<vi>[0-9]+)'),
                  lambda m: _cop2_vi(m.group('vi'))))
    # VU0 macro families calibrated against the ORIGINAL binary on 2026-09-23;
    # the evidence table is next to `_ORIGINAL_CALIBRATION`. Q, I, R and ACC
    # are spelled exactly as the contract assembler accepts them, and each
    # entry is as narrow as the original's own use of the family.
    table.append(('accplain3',
                  re.compile(r'(?P<op>vmula|vmadda|vopmula)(?:\.(?P<mask>' + _MLANES +
                             r'))?' + _WS + r'ACC(?P<df>[xyzw]*)' + _SEP +
                             r'\$?vf(?P<s>[0-9]+)(?P<sf>[xyzw]*)' + _SEP +
                             r'\$?vf(?P<t>[0-9]+)(?P<tf>[xyzw]*)'),
                  _check_acc_3op))
    table.append(('opmsub', re.compile(_vf3('vopmsub')), _check_opmsub))
    table.append(('imove',
                  re.compile(r'(?P<op>vmuli)(?:\.(?P<mask>' + _MLANES + r'))?' + _WS +
                             r'\$?vf(?P<d>[0-9]+)(?P<df>[xyzw]*)' + _SEP +
                             r'\$?vf(?P<s>[0-9]+)(?P<sf>[xyzw]*)' + _SEP + r'I'),
                  _check_i_2op))
    table.append(('vdiv',
                  re.compile(r'vdiv' + _WS + r'Q' + _SEP +
                             r'\$?vf(?P<s>[0-9]+)(?P<sf>[xyzw]*)' + _SEP +
                             r'\$?vf(?P<t>[0-9]+)(?P<tf>[xyzw]*)'),
                  _check_div))
    table.append(('clipw',
                  re.compile(r'vclipw(?:\.(?P<mask>' + _MLANES + r'))?' + _WS +
                             r'\$?vf(?P<s>[0-9]+)(?P<sf>[xyzw]*)' + _SEP +
                             r'\$?vf(?P<t>[0-9]+)(?P<tf>[xyzw]*)'),
                  _check_clipw))
    table.append(('rinit',
                  re.compile(r'vrinit' + _WS + r'R' + _SEP +
                             r'\$?vf(?P<s>[0-9]+)(?P<sf>[xyzw]*)'),
                  lambda m: _cop2_vf(m.group('s'), m.group('sf'), single=True)))
    table.append(('rnext',
                  re.compile(r'vrnext(?:\.(?P<mask>' + _MLANES + r'))?' + _WS +
                             r'\$?vf(?P<d>[0-9]+)(?P<df>[xyzw]*)' + _SEP + r'R'),
                  _check_rnext))
    missing = sorted({name for name, _, _ in table} - set(_GPR_DEST_GROUP))
    assert not missing, ('every calibrated entry must state which operand it writes so '
                         '_cop2_quadword_scratch can require its clobber: ' + ', '.join(missing))
    return table


_LINE_PATTERNS = _line_patterns()

# `f` is the FPU-register class. It is admitted because `mfc1`/`mtc1` above
# consume it, and because a measured case shows the GPR-only spelling cannot
# express what the original does: ov01/tu017 `MCamMoveProc_CalcRatio` reaches
# 99.474% with `"r"` operands, where the allocator picks v0 and the original uses
# t0, and reaches the original bytes with `"f"` operands on the same C.
_COP2_CONSTRAINTS = {'r', 'm', 'i', 'f', '=r', '=m', '=f', '+r', '+f', '=&r', '=&f'}
_COP2_CLOBBER_GPR = re.compile(r'\$(?:[0-9]|[12][0-9]|3[01])')
_COP2_ASM_WORD = re.compile(r'\b(?:asm|__asm|__asm__|__attribute|__attribute__|_Pragma)\b')
_COP2_BUILTIN = re.compile(r'\b__builtin_[A-Za-z_0-9]+')
# `asm` is accepted as an alias of `__asm__` because the permuter's vendored
# pycparser REPRINTS `__asm__ __volatile__(...)` as `asm __volatile__(...)`,
# dropping the underscores. Recognising only the underscored spelling meant
# every `c-hardware` search failed on its very first evaluation -- the
# unmodified base itself -- with "ASM, incbin, attributes or pragmas are not
# ordinary candidate C", because the reprinted span was never seen as COP2 and
# fell through to `ordinary_c`. tools/permuter_search.py's own detector already
# accepts both spellings; this gives `_COP2_FIND` the same accommodation. The
# two spellings are the same GNU C keyword, so this admits no new instruction:
# the lines inside the span are still checked one by one against the calibrated
# table, and a span still needs `__volatile__` (_COP2_HEAD) to be one at all.
_COP2_FIND = re.compile(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|/\*.*?\*/|//[^\n]*'
                        r'|\b(?:__asm__|asm)\b')
_COP2_KEYWORD = re.compile(r'(?:__asm__|__asm|asm)\b')
_COP2_HEAD = re.compile(r'[ \t]*__volatile__[ \t]*\(')
_COP2_CLOBBER_LIT = re.compile(r'"(?:\\.|[^"\\])*"')


# (Helpers part 2 of 2.)


def _split_top(text, sep):
    parts, depth, cur, instr, inch, esc = [], 0, '', False, False, False
    for ch in text:
        if instr or inch:
            cur += ch
            if esc:
                esc = False
            elif ch == '\\':
                esc = True
            elif instr and ch == '"':
                instr = False
            elif inch and ch == "'":
                inch = False
            continue
        if ch == '"':
            instr, cur = True, cur + ch
        elif ch == "'":
            inch, cur = True, cur + ch
        elif ch == '(':
            depth, cur = depth + 1, cur + ch
        elif ch == ')':
            depth, cur = depth - 1, cur + ch
        elif ch == sep and depth == 0:
            parts.append(cur)
            cur = ''
        else:
            cur += ch
    parts.append(cur)
    return parts


def _literal_prefix(item):
    bodies, i, n = [], 0, len(item)
    while True:
        while i < n and item[i] in ' \t\r\n':
            i += 1
        if i >= n or item[i] != '"':
            break
        j, body, ok = i + 1, '', False
        while j < n:
            ch = item[j]
            if ch == '\\':
                if j + 1 >= n:
                    return None
                nxt = item[j + 1]
                if nxt == 'n':
                    body += '\n'
                elif nxt == 't':
                    body += '\t'
                elif nxt in ('\\', '"'):
                    body += nxt
                else:
                    return None
                j += 2
                continue
            if ch == '"':
                ok = True
                break
            body += ch
            j += 1
        if not ok:
            return None
        bodies.append(body)
        i = j + 1
    if not bodies:
        return None
    return ''.join(bodies), item[i:]


def _template_bodies(section):
    parsed = _literal_prefix(section)
    if parsed is None:
        return None
    decoded, rest = parsed
    if rest.strip(' \t\r\n'):
        return None
    return decoded


def _cop2_template_lines(template):
    lines = [raw.strip(' \t') for raw in template.split('\n')]
    lines = [line for line in lines if line]
    require(lines, 'empty hardware template in COP2 inline')
    return lines


def _cop2_delay_slot_filler(lines):
    """Strip a trailing `nop`, the one non-COP2 line a hardware block may carry.

    A `nop` is not a COP2 instruction and is admitted nowhere else, because the
    allowlist exists so that inline assembly stays hardware operations rather
    than hand-scheduled code. It is admitted here, as the LAST line of a block
    that also contains at least one calibrated COP2 line, for one measured
    reason.

    The pinned assembler fills a compiler-emitted `jr $31` delay slot by moving
    the preceding instruction into it, and among the instruction kinds this
    route emits it does that for `lqc2` and `sqc2` and for nothing else
    (measured: `sw`, `lw`, `sd`, `addu` and `mult` before a bare `j $31` all
    leave `nop` in the slot, while `lqc2`/`sqc2` are moved). cc1 leaves that slot
    to the assembler whenever its epilogue is a bare `j $31` -- a leaf function
    with no frame to restore -- so a recovered function whose last hardware
    operation is a quadword transfer loses four bytes and the original's
    `sqc2; jr; nop` order. The original binary shows the original toolchain did
    not do this: of the 79 sites in SLUS_204.69 where a COP2 transfer sits
    immediately before a return, 74 keep `jr $31; nop` and 5 are swapped.

    A trailing `nop` is what restores the original order, because the assembler
    then has a `nop` to move instead of the transfer. It is a filler, never an
    opcode substitute: no admitted COP2 spelling encodes to `0x00000000`, so it
    stands for nothing that could otherwise be written, and the whole-file gate
    remains the only thing that decides whether the bytes are right. Reproducing
    the original assembler's scheduling exactly stays the real fix and belongs to
    the toolchain, not to this table.
    """
    if lines[-1] != 'nop':
        return lines
    require(len(lines) > 1,
            'a lone `nop` is not a hardware inline statement: the delay-slot filler is '
            'admitted only as the last line of a block that performs a COP2/VU operation')
    return lines[:-1]


def _cop2_match_line(line):
    """The table entry a line belongs to, or None. Never raises."""
    for name, pattern, check in _LINE_PATTERNS:
        m = pattern.fullmatch(line)
        if m:
            return name, m, check
    return None


def _cop2_check_line(line):
    found = _cop2_match_line(line)
    if found is None:
        require(False, 'uncalibrated COP2 inline spelling: ' + line[:120])
    name, m, check = found
    err = check(m)
    if err:
        require(False, err + ' in COP2 inline: ' + line[:120])


def _cop2_check_operands(section, kind, allowed=None):
    """Validate one operand section and return its constraints, in order.

    The constraints are returned because two block-level rules need them: `%N`
    numbering is outputs first and then inputs, and the null-default group below
    has to know that the operand it defaults is a read-write output. `allowed`
    is the closed constraint set (default `_COP2_CONSTRAINTS`); the `ee-mmi`
    route passes its own, narrower one (`_MMI_CONSTRAINTS`).
    """
    allowed = _COP2_CONSTRAINTS if allowed is None else allowed
    items = _split_top(section, ',')
    constraints = []
    for item in items:
        if not item.strip(' \t\r\n'):
            continue
        parsed = _literal_prefix(item)
        require(parsed, 'hardware %s must be constraint (expression) pairs' % kind)
        total, rest = parsed
        expr = rest.strip(' \t\r\n')
        require(expr.startswith('(') and expr.endswith(')') and len(expr) >= 2,
                'hardware %s must be constraint (expression) pairs' % kind)
        require(total in allowed,
                'uncalibrated hardware %s constraint: %s' % (kind, total[:40]))
        inner = expr[1:-1]
        require(inner.strip(' \t\r\n') and '#' not in inner
                and not _COP2_ASM_WORD.search(inner)
                and not _COP2_BUILTIN.search(inner),
                'unsupported hardware %s expression' % kind)
        constraints.append(total)
    return constraints


def _cop2_check_clobbers(section, fprs=frozenset()):
    """`fprs`: the FPRs this block loads with an admitted `li.s` (see
    `_cop2_float_constant`); no other FPR may be clobbered."""
    seen = set()
    for item in _split_top(section, ','):
        stripped = item.strip(' \t\r\n')
        require(_COP2_CLOBBER_LIT.fullmatch(stripped),
                'hardware clobbers must be single string literals')
        name = _literal_prefix(stripped)[0]
        require(name == 'memory' or _COP2_CLOBBER_GPR.fullmatch(name) or name in fprs,
                'uncalibrated hardware clobber: ' + name[:40])
        require(name not in seen, 'duplicate hardware clobber: ' + name[:40])
        seen.add(name)
    require('memory' in seen, 'COP2 inline must declare a memory clobber')
    return seen


# The null-default idiom: `%%hi`/`%%lo` materialisation of a file-scope constant
# feeding a conditional move that defaults a null pointer argument. The three
# lines are recognised as ONE group, never as three admissible instructions; see
# `_cop2_null_default`.
_NULL_DEFAULT_HI = re.compile(r'lui' + _WS + r'\$(?P<reg>[0-9]|[12][0-9]|3[01])' + _SEP +
                              r'%%hi\((?P<sym>[A-Za-z_][A-Za-z_0-9]*)\)')
_NULL_DEFAULT_LO = re.compile(r'addiu' + _WS + r'\$(?P<reg>[0-9]|[12][0-9]|3[01])' + _SEP +
                              r'\$(?P<src>[0-9]|[12][0-9]|3[01])' + _SEP +
                              r'%%lo\((?P<sym>[A-Za-z_][A-Za-z_0-9]*)\)')
_NULL_DEFAULT_MOVZ = re.compile(r'movz' + _WS + r'%(?P<d>[0-9])' + _SEP +
                                r'\$(?P<reg>[0-9]|[12][0-9]|3[01])' + _SEP + r'%(?P<t>[0-9])')


def _cop2_null_default(body, outputs, clobbers):
    """Consume the one admitted NON-hardware idiom: the null-pointer default.

    Everything else in this table is a hardware operation -- a COP2/VU0-macro
    instruction, the `mfc1`/`mtc1` pair that is VU0's float interface, or the
    GPR quadword move. This group is three ordinary scalar R5900 instructions,
    and admitting it is a deliberate, user-authorized exception (2026-09-12)
    with a stated reason, not a general opening for scalar code.

    The evidence is main/tu219, whose eight matrix/vector helpers
    (`MMathRotateMatrixX/Y/Z/XY/YX/XYZ/YXZ` and `MMathSpecialVectorSub`) are
    byte-identical as C except for exactly this sequence:

        lui   $8,%hi(McMathUnitMatrix)
        addiu $8,$8,%lo(McMathUnitMatrix)
        movz  $5,$8,$5

    placed AFTER the `daddu $2,$4,$0` that materialises `return out;`. Two
    independent facts make it a hand-written idiom rather than unfound C:

    * The order is unreachable from C with the pinned compiler. cc1 hoists the
      `lui` above the parameter copy unless BOTH `-fschedule-insns` and
      `-fschedule-insns2` are disabled, and the TU's compile contract has
      neither disabled. A per-function flag is not available and would be a
      contract change, which a worker never makes (AGENTS.md).
    * The original itself used a fixed scratch register here: across the 53
      functions of that TU the original uses `$8` 99 times and `$3` not once,
      which is a library writing its own assembly, not an allocator.

    So the admission is written to be recognisable as THAT shape and nothing
    else. All of these must hold, or the `lui` is refused like any other
    uncalibrated spelling:

    * the three lines are the FIRST three lines of the block, contiguous and in
      this order -- a pointer is defaulted before it is used;
    * one hard-coded scratch GPR carries the address through all three lines,
      and it is declared in the block's clobber list;
    * `%%hi` and `%%lo` name the same plain C identifier, so the group can only
      materialise the address of a symbol the TU already references;
    * `movz` writes and reads the SAME `%N`, and that operand is a read-write
      (`+r`) OUTPUT -- the group defaults an argument in place and can do
      nothing else;
    * the rest of the block still performs a real COP2/VU0 operation, so this
      can never be a scalar-only inline block.

    What stays refused is everything the shape does not cover: `lui`, `addiu`
    or `movz` on their own or in any other combination, `movn`, a `movz`
    between two hard-coded registers, a group that defaults an input rather
    than a read-write output, a second group in the same block, and every other
    scalar instruction (`sw`, `lw`, `addu`, `sll`, `daddu`, branches, labels and
    the assembler mode directives). Further `movz` lines after the first, each
    defaulting another read-write output from the same scratch, are admitted
    since 2026-11-02 (MMathCalcAngle/Cam). The other scalar library sequences
    with their own evidence are `_cop2_float_constant` and `_cop2_clip_flags`
    (user decision 2026-11-02); any further one needs its own evidence that the
    original hand-wrote it and that no C reaches it.
    """
    head = _NULL_DEFAULT_HI.fullmatch(body[0]) if body else None
    if head is None:
        for line in body:
            require(not _NULL_DEFAULT_HI.fullmatch(line),
                    'the null-default idiom is admitted only as the first three lines of a '
                    'hardware block: ' + line[:120])
        return body
    require(len(body) >= 4,
            'the null-default idiom needs its addiu/movz lines and a hardware operation after '
            'them: ' + body[0][:120])
    low = _NULL_DEFAULT_LO.fullmatch(body[1])
    require(low is not None,
            'the null-default idiom needs `addiu $N,$N,%%lo(symbol)` after its lui: ' + body[1][:120])
    move = _NULL_DEFAULT_MOVZ.fullmatch(body[2])
    require(move is not None,
            'the null-default idiom needs `movz %N,$N,%N` after its addiu: ' + body[2][:120])
    reg = head.group('reg')
    require(low.group('reg') == reg and low.group('src') == reg and move.group('reg') == reg,
            'the null-default idiom must carry the address in one scratch register, not $%s/$%s/$%s'
            % (reg, low.group('reg'), move.group('reg')))
    require(head.group('sym') == low.group('sym'),
            'the null-default idiom must materialise one symbol, not %s and %s'
            % (head.group('sym')[:40], low.group('sym')[:40]))
    require('$' + reg in clobbers,
            'the null-default idiom must declare its scratch register $%s as a clobber' % reg)
    require(move.group('d') == move.group('t'),
            'the null-default idiom must default the operand it tests, not %%%s from %%%s'
            % (move.group('d'), move.group('t')))
    # User decision 2026-11-02: one lui/addiu may default several pointers, each
    # with its own `movz` (MMathCalcAngle / MMathCalcAngleCam default both
    # `source` and `target` to McMathUnitVector through one $8). Every movz is
    # checked like the first and must default a distinct read-write output.
    defaulted = set()
    count = 2
    while True:
        index = int(move.group('d'), 10)
        require(index < len(outputs) and outputs[index] == '+r',
                'the null-default idiom must default a read-write output operand; %%%s is %s'
                % (move.group('d'), outputs[index] if index < len(outputs) else 'an input'))
        require(move.group('d') == move.group('t') and move.group('reg') == reg,
                'every null-default movz tests and defaults one operand from the same scratch: '
                + body[count][:120])
        require(index not in defaulted,
                'the null-default idiom defaults each operand once: ' + body[count][:120])
        defaulted.add(index)
        count += 1
        move = _NULL_DEFAULT_MOVZ.fullmatch(body[count]) if count < len(body) else None
        if move is None:
            break
    rest = body[count:]
    require(any((_cop2_match_line(line) or ('', ))[0] not in ('', 'gprquad') + _MMI_KINDS
               for line in rest),
            'the null-default idiom is admitted only inside a block that performs a COP2/VU '
            'operation')
    for line in rest:
        require(not _NULL_DEFAULT_HI.fullmatch(line),
                'one null-default idiom per hardware block: ' + line[:120])
    return rest


# Library scalar glue (user decision 2026-11-02, "calibra tu_audit"). The VU0
# libraries of MAIN (MMath, nml, sef, sv) wrote these scalar steps inside their
# own asm blocks with their fixed scratch registers; plain C never reaches those
# registers (probed: the allocator picks v0/v1/at, never $8/$9/$2/$f8), and the
# register-asm bindings and empty barriers that used to imitate them are
# forbidden (user 2026-10-16 and 2026-11-02). Each shape below is admitted only
# as written; any other scalar line stays refused.

_FLOAT_CONSTANT = re.compile(r'li\.s' + _WS + r'\$f(?P<f>[0-9]|[12][0-9]|3[01])' + _SEP +
                             r'(?P<value>-?[0-9]+\.[0-9]+(?:e-?[0-9]+)?)')
_FLOAT_CONSTANT_MFC1 = re.compile(r'mfc1' + _WS + r'\$(?P<g>[0-9]|[12][0-9]|3[01])' + _SEP +
                                  r'\$f(?P<f>[0-9]|[12][0-9]|3[01])')


def _cop2_float_constant_registers(lines):
    """The FPRs an admitted `li.s` writes, so the clobber check can accept them."""
    return frozenset('$f' + m.group('f') for m in map(_FLOAT_CONSTANT.fullmatch, lines) if m)


def _cop2_float_constant(body, clobbers):
    """Consume one `li.s $fN, constant` feeding the next line's `mfc1 $G, $fN`.

    Evidence: the original loads the VU0 scale constants of MMathDeg2Rad/Rad2Deg
    (and their I forms), sefDeg2RadVector, sefCalcRotChange and svDrawModel
    with an `lwc1 $fN, %gp_rel(.lit4)` that gas emits for `li.s`, immediately
    followed by the block's `mfc1`; in svDrawModel (`li.s $f0, 0.0078125`) the
    gas COP1 hazard `nop` between `mtc1 at,$f0` and `mfc1` appears only when the
    two lines share one template. Admitted once per block, only into a declared
    FPR clobber, and only consumed by the very next line's `mfc1` (VU0's float
    interface); the `mfc1` itself is checked by the ordinary table.
    """
    out, seen = [], False
    for index, line in enumerate(body):
        m = _FLOAT_CONSTANT.fullmatch(line)
        if m is None:
            out.append(line)
            continue
        require(not seen, 'one li.s float constant per hardware block: ' + line[:120])
        seen = True
        nxt = _FLOAT_CONSTANT_MFC1.fullmatch(body[index + 1]) if index + 1 < len(body) else None
        require(nxt is not None and nxt.group('f') == m.group('f'),
                'a li.s float constant must be consumed by the next line\'s `mfc1 $G,$f%s`: %s'
                % (m.group('f'), line[:120]))
        require('$f' + m.group('f') in clobbers,
                'a li.s float constant must declare its FPR $f%s as a clobber' % m.group('f'))
        require('$' + nxt.group('g') in clobbers,
                'the li.s float constant\'s mfc1 scratch $%s must be declared as a clobber'
                % nxt.group('g'))
    return out


_CLIP_CFC2_HARD = re.compile(r'cfc2' + _WS + r'\$(?P<r>[1-9]|[12][0-9]|3[01])' + _SEP + r'\$vi18')
_CLIP_CFC2_OPERAND = re.compile(r'cfc2' + _WS + r'%(?P<k>[0-9])' + _SEP + r'\$vi18')
_CLIP_ANDI = re.compile(r'andi' + _WS + r'\$(?P<d>[0-9]+)' + _SEP + r'\$(?P<s>[0-9]+)' + _SEP +
                        r'(?P<imm>0x[0-9a-fA-F]+|[0-9]+)')
_CLIP_MOVE = re.compile(r'move' + _WS + r'%(?P<k>[0-9])' + _SEP + r'\$(?P<s>[0-9]+)')
_CLIP_LI = re.compile(r'li' + _WS + r'\$(?P<s>[1-9]|[12][0-9]|3[01])' + _SEP +
                      r'(?P<imm>0x[0-9a-fA-F]+|[0-9]+)')
_CLIP_MOVN = re.compile(r'movn' + _WS + r'%(?P<d>[0-9])' + _SEP + r'\$(?P<s>[0-9]+)' + _SEP +
                        r'%(?P<t>[0-9])')


def _cop2_clip_flags(body, outputs, clobbers):
    """Consume the clip-flag read-out after `cfc2 ..., $vi18` (the VU0 clip flags).

    Two library shapes, both measured against the original (2026-11-02):

      * nml `_CurRotTransPersClip` (three TUs): the block ends
        `cfc2 $R,$vi18 / nop / andi $R,$R,mask / move %K,$R` -- the flags are
        masked in the scratch and handed to an `=r` output, which is why the
        original returns through `move a0,v0; jr ra; move v0,a0`;
      * MMathRotTransPersClip: `li $S,imm` earlier in the block, then
        `cfc2 %K,$vi18 / nop / movn %K,$S,%K` (flags != 0 -> the clipped mark),
        VU lines may follow.

    The `nop` is the COP2 control-transfer hazard slot the original keeps right
    after `cfc2`. Admitted once per block; the scratch must be clobbered and %K
    must be an `=r` output. Every other scalar line stays refused.
    """
    def output(k, line):
        index = int(k, 10)
        require(index < len(outputs) and outputs[index] in ('=r', '=&r'),
                'the clip-flag read-out must deliver into an =r output: ' + line[:120])
    for index, line in enumerate(body):
        hard = _CLIP_CFC2_HARD.fullmatch(line)
        if hard and body[index + 1:index + 2] == ['nop'] and len(body) == index + 4:
            andi = _CLIP_ANDI.fullmatch(body[index + 2])
            move = _CLIP_MOVE.fullmatch(body[index + 3])
            reg = hard.group('r')
            require(andi is not None and move is not None and andi.group('d') == reg
                    and andi.group('s') == reg and move.group('s') == reg,
                    'the clip-flag read-out is `cfc2 $R,$vi18 / nop / andi $R,$R,mask / '
                    'move %K,$R`: ' + line[:120])
            require('$' + reg in clobbers,
                    'the clip-flag scratch $%s must be declared as a clobber' % reg)
            output(move.group('k'), body[index + 3])
            return body[:index + 1]
        oper = _CLIP_CFC2_OPERAND.fullmatch(line)
        if oper and body[index + 1:index + 2] == ['nop'] and index + 2 < len(body) \
                and _CLIP_MOVN.fullmatch(body[index + 2]):
            movn = _CLIP_MOVN.fullmatch(body[index + 2])
            k = oper.group('k')
            require(movn.group('d') == k and movn.group('t') == k,
                    'the clip-flag movn must test and update the cfc2 operand: ' + body[index + 2][:120])
            loads = [(i, _CLIP_LI.fullmatch(l)) for i, l in enumerate(body[:index])
                     if _CLIP_LI.fullmatch(l)]
            require(len(loads) == 1 and loads[0][1].group('s') == movn.group('s'),
                    'the clip-flag movn takes its mark from one earlier `li $S,imm` of the block: '
                    + body[index + 2][:120])
            require('$' + movn.group('s') in clobbers,
                    'the clip-flag mark scratch $%s must be declared as a clobber' % movn.group('s'))
            output(k, line)
            return ([l for i, l in enumerate(body[:index]) if i != loads[0][0]] + [line] +
                    body[index + 3:])
    return body


# VU register-only lines (VF/ACC/Q/I arithmetic, moves and waits): no GPR, no
# memory, no VI, no microprogram call.
_VU_REGISTER_KINDS = frozenset(('accbc3', 'accplain3', 'bc3', 'plain3', 'opmsub', 'imove',
                                'qmove', 'move2', 'barrier', 'sqrt', 'vdiv', 'clipw'))


def _cop2_register_only_block(sections, body, operands):
    """A VU0 block with no operands and no clobbers that touches only VU registers.

    User decision 2026-11-02, evidence sefDrawParticle (main/tu210): the original
    splits its particle transform so that the pure VU matrix multiply (vf9..vf12
    times vf21..vf24 into vf27..vf30) is its own `__asm__ __volatile__(... : :)`.
    Such a statement is a bare volatile ASM_OPERANDS for cc1, which flushes cse's
    table there -- that is what the forbidden empty barrier used to imitate --
    while a "memory" clobber would make it a PARALLEL and change the code. It is
    admitted only when there is no operand and no clobber at all and EVERY line
    is a VU register operation (no lqc2/sqc2, no GPR or COP1 transfer, no VI or
    control register, no vcallms): such a block cannot read or write anything
    the compiler owns, so it has nothing to declare.
    """
    if operands or len(sections) < 2 or any(part.strip(' \t\r\n') for part in sections[1:]):
        return False
    for line in body:
        found = _cop2_match_line(line)
        if found is None or found[0] not in _VU_REGISTER_KINDS:
            return False
    for line in body:
        _cop2_check_line(line)
    return True


def _cop2_block_scratch_reads(body):
    """The 2026-10-28 MMI/store entries consume only the block's own scratch.

    Every hard-coded register read by an `_MMI_ARITH_KINDS` line (other than
    `$0`) must have been written by an earlier admitted line of the same block
    -- `qmfc2`, `cfc2`, `mfc1`, `lq`, or another MMI line. A hard-coded register
    the block did not write would be reading compiler-allocated state, which is
    what the `%N` operands are for.
    """
    written = set()
    for line in body:
        found = _cop2_match_line(line)
        if found is None:
            continue
        kind, m = found[0], found[1]
        if kind in _MMI_ARITH_KINDS:
            names = m.groupdict()
            reads = [names.get(k) for k in ('s', 't') if names.get(k) is not None]
            for reg in reads:
                require(reg == '0' or reg in written,
                        'hard-coded $%s is read before this block writes it: the admitted '
                        'MMI/store lines consume only block-local scratch: %s' % (reg, line[:120]))
        group = _GPR_DEST_GROUP.get(kind)
        if group is None or (kind, m.groupdict().get('op')) in _GPR_READ_ONLY:
            continue
        value = m.group(group)
        if value is not None and not value.startswith('%'):
            written.add(value.lstrip('$'))


def _cop2_quadword_scratch(body, clobbers):
    """Every hard-coded scratch GPR an admitted line writes must be clobbered.

    The register is hard-coded precisely because the original pinned it, so the
    compiler has to be told it is written; an undeclared one is a real defect
    the whole-file gate would only catch by luck, in another function. This
    covers the `lq`/`sq` quadword move and, since 2026-09-13, the destination of
    every admitted MMI line: `ldl`/`ldr`, `pcgth`, `pextlh` and `dsrl` all write
    a hard-coded GPR for the same reason.
    """
    for line in body:
        found = _cop2_match_line(line)
        if found is None:
            continue
        kind, m = found[0], found[1]
        group = _GPR_DEST_GROUP.get(kind)
        if group is None or (kind, m.groupdict().get('op')) in _GPR_READ_ONLY:
            continue
        value = m.group(group)
        if value is None or value.startswith('%'):
            # A `%N` operand is the compiler's own register: it already knows.
            continue
        if value.lstrip('$') == '0':
            # `$0` is hardwired to zero on MIPS: a write to it discards, so it
            # cannot carry state the compiler needs to be told about. This is
            # the accepted `cfc2.i $0,$vi0` VU0 interlock wait, which reads the
            # status word only for its side effect of waiting.
            continue
        reg = '$' + value.lstrip('$')
        require(reg in clobbers,
                'a hard-coded scratch register must be declared as a clobber: %s is written '
                'by this line and is not in the clobber list: %s' % (reg, line[:120]))


def _cop2_scheduling_barrier(sections, decoded):
    """The instruction-free scheduling barrier (user-authorized 2026-09-13).

    `__asm__ __volatile__("" : : "f"(x));` -- an EMPTY template, no output
    operand and no clobber list. It is admitted because it is not a hardware
    access and cannot be one: an empty template emits nothing, so no opcode,
    no `.word` and no hand-scheduled instruction can hide in it. What it does
    is fix a value's position for the scheduler.

    Measured on `sefLerpVectorSC` (main/tu211, 0x002e1908), 16 forms: the
    original reads a `.lit4` scale with `lwc1` BEFORE the `beq` that guards its
    only use, and cc1 2.96 sinks that load into the taken arm for every
    ordinary C spelling, because the value is dead on the other path. A
    `register float x asm("$f8")`/`$f0` pin does not stop it; a `volatile`
    local stops it but adds a stack round-trip the original has not; making the
    read and the pointer advance unconditional still sinks it. With this
    barrier, forms 11 and 12 pass the whole-file gate with the main image
    byte-identical (config/compiler-patterns.json CP-0171).

    The shape is exactly as narrow as that evidence:

      * the template must decode to whitespace only -- no comment, no text;
      * there must be no output operand, because an output makes the statement
        a value producer rather than a fence;
      * there must be no clobber section, because a clobber is a claim about
        machine state that an empty template cannot make (the `"memory"` full
        barrier is a different construct and is not admitted here);
      * there must be at least one input operand, because a barrier that names
        no value constrains nothing and would be dead text in the source.

    Anything else falls through to the ordinary calibrated path and is refused
    there, including a non-empty template with no clobber.
    """
    if decoded.strip(' \t\r\n'):
        return False
    # "Empty" here means "assembles to nothing": the template may hold only
    # whitespace, which the assembler discards. A comment is NOT whitespace to
    # this check -- `/* ... */` survives `strip` and falls through to the
    # calibrated path, where it is refused as an uncalibrated spelling.
    require(len(sections) <= 3,
            'an empty hardware template must not declare a clobber: it emits no '
            'instruction, so it makes no claim about machine state')
    outputs = (_cop2_check_operands(sections[1], 'output')
               if len(sections) >= 2 and sections[1].strip(' \t\r\n') else [])
    require(not outputs,
            'an empty hardware template must not declare an output operand: it is '
            'a scheduling barrier, not a value producer')
    inputs = (_cop2_check_operands(sections[2], 'input')
              if len(sections) >= 3 and sections[2].strip(' \t\r\n') else [])
    require(inputs,
            'an empty hardware template must name at least one input operand: a '
            'barrier that names no value constrains nothing')
    return True


_MMI_KINDS = ('mmiload', 'mmicmp', 'mmiext', 'mmishift')

# The one instruction that may consume the packed widening. It goes GPR -> VF,
# which is the direction the idiom needs, and both accepted sources end the
# widening exactly there. `ctc2` (GPR -> VI control) and `mtc1` (GPR -> FPR) also
# read a GPR, but neither appears in the corpus at the end of this idiom, so
# neither is evidence that the widening reached VU0 as a vector -- admitting
# them was wider than the authorization (independent review, round three).
# `qmfc2`, `cfc2` and `mfc1` go the other way: a register appearing in one of
# those is destroyed by it, not consumed.
_GPR_READING = {('move', 'qmtc2')}

# Admitted spellings that only READ the GPR they name, so they leave a packed
# register's provenance intact. Everything else that names a GPR is treated as
# writing it and invalidates that register's provenance -- deliberately
# fail-safe, so a table entry added later cannot silently reintroduce the stale-
# provenance bug the independent review found in round five: a register loaded
# by a pair, overwritten by `mfc1`/`qmfc2`/`cfc2`/`lq`, and then consumed by
# `pcgth` as if it still held the packed halfwords. Traceable-but-stale is not
# traceable.
_GPR_READ_ONLY = {('move', 'qmtc2'), ('control', 'ctc2'), ('fpmove', 'mtc1'), ('gprstore', 'sw'),
                  ('gprquad', 'sq'), ('transfer', 'lqc2'), ('transfer', 'sqc2')}
_GPR_TOKEN = re.compile(r'\$(3[01]|[12][0-9]|[0-9])\b')
def _mmi_block_shape(body):
    """The MMI admission is one idiom, so the BLOCK has to be that idiom.

    The per-line rules say each instruction is well formed; they cannot say the
    instructions are the evidenced operation. Two rounds of independent review
    of this admission (2026-09-13, before any function used it) found that
    per-line and set-membership checks let through: an `ldl`/`ldr` "pair" whose
    halves read different operands or write different registers; a lone `ldl`;
    a `pcgth` consuming a register nothing loaded; a packed chain with no COP2
    consumer; a "consumer" that merely mentions the register while WRITING it
    (`mfc1 $10,%1`); a consumer placed BEFORE the chain that produces the value;
    and two independent load pairs whose halves are cross-consumed by one
    `pextlh`, which is two idioms interleaved rather than one.

    So the block is checked as a dataflow, in order:

      * every `ldl` is matched by an `ldr` with the SAME destination register
        and the SAME base operand, seven apart -- one unaligned doubleword;
      * every packed operand is traceable to a load pair earlier in the block,
        and still holds what that pair loaded: any admitted line that WRITES a
        GPR (`mfc1`, `qmfc2`, `cfc2`, `lq`, and anything whose direction the
        table does not state) invalidates that register's provenance, so a
        value overwritten between the load and its use is no longer consumable.
        `pextlh` may only combine registers from the SAME pair, so two
        unrelated reads cannot be mixed into one packed result;
      * the result reaches VU0: a later `qmtc2` -- the one evidenced consumer,
        and the instruction both accepted sources end the widening with --
        reads a register the MMI lines wrote, and does so after the write. A
        packed computation that never reaches VU0 is scalar MMI, which has
        neither evidence nor authorization.
    """
    rows = []
    for line in body:
        found = _cop2_match_line(line)
        if found is not None:
            rows.append((found[0], found[1], line))
    if not any(kind in _MMI_KINDS for kind, _, _ in rows):
        return

    loads = {}
    for kind, m, line in rows:
        if kind == 'mmiload':
            loads.setdefault((m.group('gpr'), m.group('base')), {})[m.group('op')] = \
                int(m.group('off'), 0)
    for kind, m, line in rows:
        if kind != 'mmiload':
            continue
        pair = loads[(m.group('gpr'), m.group('base'))]
        other = 'ldr' if m.group('op') == 'ldl' else 'ldl'
        require(other in pair,
                'an %s needs its matching %s on the same register and base in the same '
                'block: %s' % (m.group('op'), other, line[:120]))
        require(pair['ldl'] - pair['ldr'] == 7,
                'the ldl/ldr pair must be seven apart on one doubleword (got %d and %d): %s'
                % (pair['ldl'], pair['ldr'], line[:120]))

    # Provenance: which load pair each live register descends from. A register
    # with no entry has not been produced by this block's MMI lines.
    root, written_at = {}, {}
    for index, (kind, m, line) in enumerate(rows):
        if kind == 'mmiload':
            key = (m.group('gpr'), m.group('base'))
            root[m.group('gpr')] = key
            written_at[m.group('gpr')] = index
            continue
        if kind not in _MMI_KINDS:
            # A non-MMI line that writes a GPR destroys whatever the widening
            # had put there, so that register is no longer traceable to its
            # load pair. Read-only spellings leave it alone; anything else,
            # known or not, invalidates.
            if (kind, m.groupdict().get('op')) not in _GPR_READ_ONLY:
                for reg in _GPR_TOKEN.findall(line):
                    root.pop(reg, None)
                    written_at.pop(reg, None)
            continue
        if kind == 'mmicmp':
            sources, dest = [m.group('t')], m.group('d')
        elif kind == 'mmiext':
            sources, dest = [m.group('s'), m.group('t')], m.group('d')
        else:
            sources, dest = [m.group('s')], m.group('d')
        for reg in sources:
            require(reg in root,
                    'packed operand $%s is not produced by a load pair in this block: the '
                    'widening must consume what its own pair loaded: %s' % (reg, line[:120]))
        origins = {root[reg] for reg in sources}
        require(len(origins) == 1,
                'this line combines registers from %d different load pairs: the admitted '
                'idiom widens ONE unaligned doubleword, not several mixed together: %s'
                % (len(origins), line[:120]))
        root[dest] = origins.pop()
        written_at[dest] = index

    for index, (kind, m, line) in enumerate(rows):
        if kind in _MMI_KINDS or (kind, m.groupdict().get('op')) not in _GPR_READING:
            continue
        reg = (m.group('gpr') or '').lstrip('$')
        if reg in written_at and written_at[reg] < index:
            return
    require(False,
            'the packed widening must reach VU0: no later qmtc2 in this block reads a '
            'register the MMI lines wrote')


def _sef_particle_scale_block(lines, outputs, inputs, input_section, clobber_section):
    """The one user-authorized COP1 load in main/tu211:sefCreateParticle.

    Keep `lwc1` and FPR clobbers outside the general COP2 grammar. The exact
    producer/consumer chain below loads one float, transfers it through $8 to
    VF1.x, scales VF2.xyz and stores the same vector. No other scalar operation
    or hardware state effect is admitted by this exception (2026-10-22).
    """
    expected = (
        r'lqc2' + _WS + r'vf2' + _SEP + r'0' + _WS + r'\(' + _WS + r'%0' + _WS + r'\)',
        r'lwc1' + _WS + r'\$f8' + _SEP + r'%1',
        r'mfc1' + _WS + r'\$8' + _SEP + r'\$f8',
        r'qmtc2' + _WS + r'\$8' + _SEP + r'vf1',
        r'vmulx\.xyz' + _WS + r'vf2' + _SEP + r'vf2' + _SEP + r'vf1x',
        r'sqc2' + _WS + r'vf2' + _SEP + r'0' + _WS + r'\(' + _WS + r'%0' + _WS + r'\)',
    )
    require(len(lines) == len(expected) and all(re.fullmatch(pattern, line)
            for pattern, line in zip(expected, lines)),
            'sefCreateParticle scale exception requires its exact six-line producer/consumer block')
    require(outputs == [] and inputs == ['r', 'm'],
            'sefCreateParticle scale exception has no outputs and exactly r(pointer), m(scale) inputs')
    items = [item for item in _split_top(input_section, ',') if item.strip()]
    require(len(items) == 2 and re.fullmatch(r'\(\s*randomRotationDegToRad\s*\)',
            _literal_prefix(items[1])[1].strip()),
            'sefCreateParticle scale exception loads only randomRotationDegToRad')
    clobbers = []
    for item in _split_top(clobber_section, ','):
        stripped = item.strip(' \t\r\n')
        require(_COP2_CLOBBER_LIT.fullmatch(stripped),
                'sefCreateParticle scale clobbers must be single string literals')
        clobbers.append(_literal_prefix(stripped)[0])
    require(len(clobbers) == 3 and set(clobbers) == {'$8', '$f8', 'memory'},
            'sefCreateParticle scale exception clobbers exactly $8, $f8 and memory')
    for index, line in enumerate(lines):
        if index != 1:
            _cop2_check_line(line)


# StartOfScan VU0 I-register immediate (user decision 2026-10-28 on main/tu097,
# "Aceptalo como c con asm, ya que podemos suponer que es codigo en asm que
# escribio un humano"). The original StartOfScan (main VA 0x00225310) opens its
# hand-written VU0 setup block with `lui v0,0x3e00` (0x0022534c) immediately
# followed by `ctc2 v0,$vi21` (0x00225350) and the block's `lqc2 $vf1..`: it
# loads 0.125f (0x3e000000) into the VU0 I register through $2, the usual
# hand-written VU0 idiom. Only this pair is admitted, only as the first two
# lines of a block that goes on to perform calibrated COP2 operations, only
# with the block declaring its "$2" clobber, at most once per function, and only
# for the identity tools/tu_audit.py selects (`vu0_i_immediate_c`). The load is
# never admitted on its own, with another register, value or VI target, or
# anywhere else in the hardware route. Not a precedent.
_VU0_I_IMMEDIATE_LOAD = re.compile(r'(?:lui' + _WS + r'\$2' + _SEP + r'0x3e00'
                                   r'|li' + _WS + r'\$2' + _SEP + r'0x3e000000)')
_VU0_I_IMMEDIATE_CTC2 = re.compile(r'ctc2' + _WS + r'\$2' + _SEP + r'\$vi21')


def _vu0_i_immediate(body, clobbers):
    """Consume StartOfScan's leading `lui $2,0x3e00` feeding `ctc2 $2,$vi21`.

    Returns (rest, used). `rest` keeps the `ctc2`, which the ordinary table
    still checks; only the immediate load itself is consumed here.
    """
    if not body or not _VU0_I_IMMEDIATE_LOAD.fullmatch(body[0]):
        return body, False
    require(len(body) >= 3 and _VU0_I_IMMEDIATE_CTC2.fullmatch(body[1]),
            'the StartOfScan I-register immediate must be consumed by the very next '
            '`ctc2 $2,$vi21` of a block that performs COP2/VU operations: ' + body[0][:120])
    require('$2' in clobbers,
            'the StartOfScan I-register immediate writes $2: the block must declare "$2"')
    for line in body[2:]:
        require(not _VU0_I_IMMEDIATE_LOAD.fullmatch(line),
                'one StartOfScan I-register immediate per hardware block: ' + line[:120])
    return body[1:], True


# XrgParticle _Disp clip-sequence nops (user decision 2026-11-04 on ov12/tu084). The
# original _Disp (ov12 VA 0x00a49318) keeps three EE `sll $0,$0,0` between
# `vmaddw.xyzw vf31,vf30,vf0w` and `vclipw.xyz vf31,vf31w`; `vnop` encodes
# 0x4a0002ff and cannot produce those bytes. Only that exact run is admitted, once
# per function and only for the identities tools/tu_audit.py selects
# (`xrg_clip_nops_c`: also XrgPaint2D _CalcPosOnScreen and _CalcPosOnScreenFloat, same
# sequence, user decision 2026-11-04). Every other `nop` keeps the ordinary refusal.
# Not a precedent.
_XRG_CLIP_NOPS_BEFORE = re.compile(r'vmaddw\.xyzw' + _WS + r'vf31' + _SEP + r'vf30' + _SEP + r'vf0w')
_XRG_CLIP_NOPS_AFTER = re.compile(r'vclipw\.xyz' + _WS + r'vf31' + _SEP + r'vf31w')


def _xrg_clip_nops(body):
    """Consume the three `nop` lines of the XRG clip sequence. Returns (rest, used)."""
    for index in range(1, len(body) - 3):
        if (body[index:index + 3] == ['nop', 'nop', 'nop']
                and _XRG_CLIP_NOPS_BEFORE.fullmatch(body[index - 1])
                and _XRG_CLIP_NOPS_AFTER.fullmatch(body[index + 3])):
            return body[:index] + body[index + 3:], True
    return body, False


def _cop2_parse_construct(text, start, *, mmi_transpose=False, sef_particle_scale=False,
                          vu0_i_immediate=False, xrg_clip_nops=False):
    # Advance by the keyword actually written, not by a fixed len('__asm__').
    # The permuter's vendored pycparser reprints `__asm__ __volatile__(...)` as
    # `asm __volatile__(...)`, and skipping seven characters past a three-letter
    # `asm` overshot into the middle of `__volatile__`, so the construct never
    # parsed, the span was never found, and `ordinary_c`'s blanket asm refusal
    # failed every `c-hardware` permuter search on its own unmodified base.
    keyword = _COP2_KEYWORD.match(text, start)
    if not keyword:
        return None
    head = _COP2_HEAD.match(text, keyword.end())
    if not head:
        return None
    depth, i, n = 0, head.end() - 1, len(text)
    instr, inch, esc = False, False, False
    while i < n:
        ch = text[i]
        if instr or inch:
            if esc:
                esc = False
            elif ch == '\\':
                esc = True
            elif instr and ch == '"':
                instr = False
            elif inch and ch == "'":
                inch = False
        elif ch == '"':
            instr = True
        elif ch == "'":
            inch = True
        elif ch == '(':
            depth += 1
        elif ch == ')':
            depth -= 1
            if not depth:
                break
        i += 1
    require(i < n, 'unbalanced parentheses in hardware inline')
    sections = _split_top(text[head.end():i], ':')
    require(len(sections) <= 4, 'too many sections in hardware inline')
    decoded = _template_bodies(sections[0])
    require(decoded is not None, 'hardware template must be string literal(s)')
    if _cop2_scheduling_barrier(sections, decoded):
        _COP2_SPANS.append(dict(kind='sched_barrier', start=start))
        return i + 1
    lines = _cop2_template_lines(decoded)
    body = _cop2_delay_slot_filler(lines)
    outputs = _cop2_check_operands(sections[1], 'output') if len(sections) >= 2 else []
    inputs = _cop2_check_operands(sections[2], 'input') if len(sections) >= 3 else []
    operands = len(outputs) + len(inputs)
    if _cop2_register_only_block(sections, body, operands):
        _COP2_SPANS.append(dict(kind='cop2', start=start))
        return i + 1
    require(len(sections) >= 4, 'COP2 inline must declare a memory clobber')
    if sef_particle_scale and any(re.match(r'lwc1(?:\s|$)', line) for line in lines):
        _sef_particle_scale_block(lines, outputs, inputs, sections[2], sections[3])
        _COP2_SPANS.append(dict(kind='cop2', sef_particle_scale=True, start=start))
        return i + 1
    clobbers = _cop2_check_clobbers(sections[3], _cop2_float_constant_registers(body))
    if mmi_transpose:
        _mmi_cop2_transpose_block(body, outputs, inputs, clobbers)
        _COP2_SPANS.append(dict(kind='cop2', start=start))
        return i + 1
    body = _cop2_null_default(body, outputs, clobbers)
    body = _cop2_float_constant(body, clobbers)
    body = _cop2_clip_flags(body, outputs, clobbers)
    i_immediate = False
    if vu0_i_immediate:
        body, i_immediate = _vu0_i_immediate(body, clobbers)
    clip_nops = False
    if xrg_clip_nops:
        body, clip_nops = _xrg_clip_nops(body)
    for line in body:
        _cop2_check_line(line)
    _cop2_quadword_scratch(body, clobbers)
    _cop2_block_scratch_reads(body)
    _mmi_block_shape(body)
    for ref in re.findall(r'%([0-9])', '\n'.join(lines)):
        require(int(ref) < operands, 'hardware operand reference out of range: %' + ref)
    _COP2_SPANS.append(dict(
        kind=('cop2' if any((_cop2_match_line(line) or ('',))[0]
                            not in ('', 'gprquad') + _MMI_KINDS + _MMI_ARITH_KINDS
                            for line in body)
              else 'other'),
        vu0_i_immediate=i_immediate,
        xrg_clip_nops=clip_nops,
        start=start))
    return i + 1


# What each parsed span turned out to be and where it sits, for the
# function-level checks below. Reset by `_cop2_inline_spans`, the only entry
# point that walks a whole candidate. `kind` is 'sched_barrier' for the
# instruction-free barrier, 'cop2' for a block containing a genuine COP2/VU
# line, and 'other' for one whose only content is the quadword move or the MMI
# widening.
_COP2_SPANS = []


def _cop2_inline_spans(text, *, mmi_transpose=False, sef_particle_scale=False,
                       vu0_i_immediate=False, xrg_clip_nops=False):
    """Locate calibrated COP2 inline statements; raise with exact errors."""
    del _COP2_SPANS[:]
    found = []
    for m in _COP2_FIND.finditer(text):
        # The three spellings are one GNU C keyword. `asm` has to be here, not
        # only in _COP2_FIND, because the permuter's vendored pycparser reprints
        # `__asm__ __volatile__(...)` as `asm __volatile__(...)`: recognising
        # only the underscored token made every `c-hardware` search fail on its
        # own unmodified base. A string, comment or `register T v asm("$8")`
        # declaration is not a span either way -- the first three are consumed
        # by the alternation above, and the last has no `__volatile__`, so
        # _cop2_parse_construct returns None for it.
        if m.group(0) not in ('__asm__', '__asm', 'asm'):
            continue
        end = _cop2_parse_construct(text, m.start(), mmi_transpose=mmi_transpose,
                                    sef_particle_scale=sef_particle_scale,
                                    vu0_i_immediate=vu0_i_immediate,
                                    xrg_clip_nops=xrg_clip_nops)
        if end is not None:
            found.append((m.start(), end))
    return found


# `register T v asm("$8");` -- a GNU C storage declaration, not an inline-asm
# statement. `ordinary_c` refuses the token `asm` outright, which made a plain
# declaration a hard wall in a hardware-classified function, so the binding is
# recognised here and blanked before the remainder is checked. Only the numeric
# EE register names are admitted, and only inside a `register` declaration.
_COP2_REG_BIND = re.compile(
    r'\bregister\b[^;{}()]{1,200}?\b(?P<kw>asm|__asm|__asm__)\s*\(\s*'
    r'"(?P<reg>\$(?:f?(?:[0-9]|[12][0-9]|3[01])))"\s*\)')


def _cop2_register_bindings(text):
    """Blank every `register ... asm("$N")` binding; refuse any other spelling.

    A fixed-register local is admitted so that it stops being an absolute wall,
    not because it is good evidence. It says the compiler's own allocator did not
    land where the original did, and the likelier reading is that the original C
    shape has not been found yet: the original authors wrote plain C and the
    compiler chose the registers. A candidate that leans on pinned locals is
    weaker than one where the allocator reaches the original by itself, and a
    reviewer should ask for the shape before accepting the pin.
    """
    out, end = [], 0
    for m in _COP2_REG_BIND.finditer(text):
        out.append(text[end:m.start('kw')])
        # Blank the binding itself and keep the declaration, preserving newlines
        # so nothing downstream sees two lines joined into one.
        out.append(''.join('\n' if c == '\n' else ' ' for c in text[m.start('kw'):m.end()]))
        end = m.end()
    out.append(text[end:])
    # Whatever `asm` spelling is left is a statement, and this route admits only
    # the calibrated COP2 constructs above.
    return ''.join(out)


def _brace_counting_text(text):
    """The same text with everything that is not really a brace neutralised.

    A depth walk may only count a `{` that actually opens a block. Three kinds
    of text can hold one that does not, and each was found the hard way by the
    independent review of this admission:

      * comments -- already collapsed to a space before `hardware_c` runs;
      * string and character literals, whose content `hardware_c` deliberately
        preserves because the inline templates ARE string literals. One stray
        `{` in an ordinary message (`"unexpected token {"`) kept the depth from
        ever returning to zero, folding every later function in the file into
        one block (round three);
      * preprocessor directive lines. `ordinary_c` permits `#define`/`#undef`/
        `#if`/`#ifdef`/`#ifndef`/`#elif`/`#else`/`#endif` by keyword, without
        reading their bodies, so a single unbalanced one-line macro -- present,
        not even invoked -- reopened exactly the same hole (round four).

    Blanking the literal vector alone was fixing the instance instead of the
    class, which is why the same bug arrived twice. This function is the class:
    braces are counted only where they are really braces. Length is preserved
    so callers can index the result by the original offsets.
    """
    out, i, n = list(text), 0, len(text)
    while i < n:
        ch = text[i]
        if ch in '"\'':
            quote, i = ch, i + 1
            while i < n:
                if text[i] == '\\':
                    out[i] = ' '
                    if i + 1 < n:
                        out[i + 1] = ' '
                    i += 2
                    continue
                if text[i] == quote:
                    i += 1
                    break
                out[i] = ' '
                i += 1
            continue
        i += 1
    # Directive lines: blank their braces wherever the line's first non-blank
    # character is `#`. Done after the literal pass so a `#` inside a literal
    # cannot start a phantom directive.
    start = 0
    blanked = ''.join(out)
    for line in blanked.split('\n'):
        if line.lstrip(' \t').startswith('#'):
            for offset, ch in enumerate(line):
                if ch in '{}':
                    out[start + offset] = ' '
        start += len(line) + 1
    return ''.join(out)


def _cop2_barrier_companions(masked, spans):
    """A scheduling barrier needs COP2/VU content in its OWN brace block.

    Emitting nothing, the barrier cannot be the hardware content that makes a
    source eligible (docs/ps2-capabilities.md: actual COP2/VU instructions in
    the pinned original establish it), so it is admitted only beside real ones.
    Two rounds of independent review narrowed what "beside" means:

      * not `sync`/`ei`/`di`. Those are the interrupt primitives, counted
        separately and admitted for their own reason; a function whose only
        hardware is an unrelated interrupt barrier would otherwise carry a
        scheduling barrier anywhere in it;
      * not the quadword move or the MMI widening on their own, which is why
        `_cop2_parse_construct` records a span as `cop2` only when the block
        contains a genuine COP2/VU line;
      * not another function's. `hardware_c` is handed whole-file text on some
        paths, so two concatenated definitions -- one with only a barrier, one
        with real COP2 -- satisfied each other. Brace depth gives each span its
        enclosing top-level block and the companion has to be in the same one.
        The depth walk runs over `_brace_counting_text(masked)`, which
        neutralises every brace that is not really a brace -- inside a literal
        or on a preprocessor directive line -- because either one otherwise
        merges every later function in the file into one block.
    """
    if not any(s['kind'] == 'sched_barrier' for s in spans):
        return
    block, depth, current, counter = {}, 0, None, 0
    for index, ch in enumerate(_brace_counting_text(masked)):
        if ch == '{':
            if depth == 0:
                counter += 1
                current = counter
            depth += 1
        block[index] = current
        if ch == '}':
            depth -= 1
            if depth <= 0:
                depth, current = 0, None
    have = {block.get(s['start']) for s in spans if s['kind'] == 'cop2'}
    for s in spans:
        if s['kind'] != 'sched_barrier':
            continue
        where = block.get(s['start'])
        require(where is not None and where in have,
                'a scheduling barrier is admitted only inside a function that itself '
                'performs a calibrated COP2/VU operation: it emits no instruction, so it '
                'cannot be the hardware content that makes this source eligible, and a '
                'sync/ei/di primitive or another function\'s hardware does not count')


def _hardware_line_continuations_ok(text):
    """Backslash-newline is admitted only as the continuation of a `#define`
    that names no inline assembly (user direction 2026-09-27: a published
    multi-line macro in a TU prelude must not refuse every hardware function
    of that TU).  A continuation anywhere else, or any continued directive that
    mentions asm, is still refused as a preprocessing escape."""
    in_define = False
    for line in text.split('\n'):
        line = line.rstrip('\r')
        is_define = in_define or re.match(r'\s*#\s*define\b', line) is not None
        if is_define and re.search(r'\b(?:asm|__asm|__asm__)\b', line):
            return False
        continued = line.endswith('\\')
        if continued and not is_define:
            return False
        in_define = is_define and continued
    return True


def hardware_c(text, staged=None, headers=(), inner=None, *, mmi_transpose=False,
               sef_particle_scale=False, vu0_i_immediate=False, xrg_clip_nops=False):
    """Only calibrated barriers/interrupt operations and an allowlisted
    COP2/VU0-macro spelling set are admitted inline (EE caller side).

    Memory clobbers constrain compiler motion; volatile preserves the operation.
    Privilege, interrupt state and hardware ordering still require semantic review.
    COP2 admission is syntactic allowlisting with exact operand/state validation;
    complete exact linked evidence is still required downstream. VU microprogram
    bodies remain vu-microcode; only the EE-side vcallms dispatch call is admitted.
    The admitted spelling set is derived from the COP2/VU0-macro instructions
    the accepted, reviewed standalone sources under `src/<unit>/<tu>/` actually
    contain, and each line is validated for register range, lane mask, field
    agreement, broadcast component and immediate range (`_line_patterns`). What
    those same sources also contain and this function still refuses is
    everything that is not a hardware operation: the scalar code, the branches
    and the labels, the assembler mode directives (`.set noreorder`,
    `.set nomacro`, `.set noat`), all MMI beyond the one quadword-move shape
    below, and every COP1 instruction but the `mfc1`/`mtc1` pair that is the
    interface of a VU0 macro-mode call. A whole original function wrapped in one
    `__asm__` block with register-pinned locals is refused by exactly those
    rules, and it is not recovery: this route is for a few hardware operations
    inside real C.

    Three admissions are not COP2 instructions and each is bounded by its own
    measured reason (since 2026-11-02 the library scalar glue of
    `_cop2_float_constant` / `_cop2_clip_flags`, the multi-`movz` null default
    and the operand-free `_cop2_register_only_block` join them, each documented
    where it is checked):

    * a trailing `nop`, and only as the last line of a block that performs a
      COP2/VU operation (`_cop2_delay_slot_filler`);
    * the R5900 GPR quadword load/store, and only in the shape the original
      wrote inline -- a hard-coded scratch register, a `%N` operand base and an
      aligned literal displacement, with the scratch register declared
      (`_line_patterns` `gprquad`, `_cop2_quadword_scratch`). A compiler-emitted
      128-bit aggregate copy is NOT admitted here and stays on the native-TI
      source route (docs/native-ti.md);
    * the null-default idiom, the one admitted NON-hardware idiom: a
      `%%hi`/`%%lo` symbol-address materialisation feeding a `movz` that
      defaults a null pointer argument, recognised as one three-line group at
      the head of a block that also performs a COP2/VU operation
      (`_cop2_null_default`). No other scalar instruction or sequence is
      admitted anywhere in this route.

    In the C around the inline blocks, the only builtin admitted is a direct
    call of a calibrated builtin (`_HARDWARE_C_BUILTINS`: `__builtin_fabsf`,
    one inline `abs.s`, no call; `__builtin_sqrtf`, an inline `sqrt.s` guarded
    by `c.eq.s`/`bc1t` with a `jal sqrtf` fallback for a NaN result, the
    original's own shape). Every other `__builtin_*` is refused there,
    and every builtin is refused inside an inline operand expression.
    """
    require(text.isascii() and '\x00' not in text and '??' not in text and '%:' not in text
            and _hardware_line_continuations_ok(text), 'hardware C encoding or preprocessing escape')
    require(not re.search(r'^\s*#.*\b(?:asm|__asm|__asm__)\b', text, re.M),
            'inline assembly cannot be supplied by a preprocessor directive')
    statement = (r'(?P<hardware>\b__asm__\s+__volatile__\s*\(\s*"(?:sync(?:\.l)?|ei|di)"'
                 r'\s*:\s*:\s*:\s*"memory"\s*\)\s*;)')
    tokens = statement + r'|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|/\*.*?\*/|//[^\n]*'
    count = 0

    def replace(match):
        nonlocal count
        if match.group('hardware'):
            count += 1
            return ';'
        return ' ' if match[0].startswith(('/*', '//')) else match[0]

    masked = re.sub(tokens, replace, text, flags=re.S)
    spans = _cop2_inline_spans(masked, mmi_transpose=mmi_transpose,
                               sef_particle_scale=sef_particle_scale,
                               vu0_i_immediate=vu0_i_immediate,
                               xrg_clip_nops=xrg_clip_nops)
    if xrg_clip_nops:
        require(sum(span.get('xrg_clip_nops', False) for span in _COP2_SPANS) <= 1,
                'the XRG clip sequence admits its three nops once per function')
    if mmi_transpose:
        require(count == 0 and len(spans) == 1, 'the bounded MMI/COP2 transpose is one inline block')
    if sef_particle_scale:
        require(sum(span.get('sef_particle_scale', False) for span in _COP2_SPANS) <= 1,
                'sefCreateParticle admits at most one bounded COP1 scale block per function')
    if vu0_i_immediate:
        require(sum(span.get('vu0_i_immediate', False) for span in _COP2_SPANS) <= 1,
                'StartOfScan admits at most one I-register immediate per function')
    # The scheduling barrier emits nothing, so on its own it is not a hardware
    # operation and cannot make a candidate hardware-eligible. docs/ps2-
    # capabilities.md makes actual COP2/VU instructions in the pinned original
    # what establishes eligibility, and the measurement that earned this
    # admission was a function that genuinely performs them (sefLerpVectorSC:
    # lqc2, mfc1, qmtc2, vmulx.xyz, sqc2). Admitting a barrier in a candidate
    # with no hardware content at all would turn it into a general-purpose
    # "pin this scalar's schedule" escape hatch, gated only by a human reading
    # the contract prose -- exactly the unearned barrier
    # docs/ee-reference/inline-cop2.md warns about. Found by the independent
    # review of the admission on 2026-09-13, before any function used it.
    _cop2_barrier_companions(masked, _COP2_SPANS)
    chars = list(masked)
    for start, end in spans:
        for i in range(start, end):
            # Newlines survive the blanking. `ordinary_c` reads the remainder
            # line by line for preprocessing directives and `native_ti.source`
            # (see hardware_native_ti_c) matches its typedef and counts block
            # scope per line, so joining the line before an inline block to the
            # line after it would change what the remainder check sees.
            if chars[i] != '\n':
                chars[i] = ' '
    remainder = _cop2_register_bindings(''.join(chars))
    # What is left once the calibrated hardware statements are masked is
    # ordinary candidate C -- unless the TU is declared native-TI, in which case
    # it is native-TI C, which is `ordinary_c` plus one file-scope typedef
    # grammar and several extra refusals (hardware_native_ti_c).
    (inner or ordinary_c)(remainder, staged, headers)
    # Only a calibrated builtin, written as a direct call, may remain in the C
    # around the inline blocks (_HARDWARE_C_BUILTINS). The operand expressions
    # of an inline block are not part of `remainder` and keep refusing every
    # builtin (`_cop2_check_operands`, `_COP2_BUILTIN`).
    _hardware_c_builtin_calls(remainder, 'hardware C')
    return count + len(spans)


# Builtins admitted in the C that remains once a hardware function's inline
# blocks are masked. The set is closed; each entry needs its own measurement
# under the pinned contract. `ordinary_c` does not restrict builtins at all (the
# whole-file gate decides their bytes), so this route had refused every builtin
# outright, which made a hardware function that also needs a float absolute
# value unwritable.
#
# __builtin_fabsf (user decision 2026-10-10, "Sí a las tres: asigna el cambio de
# match.py"): measured with ee-gcc2.96-realconv-lp7-fsv4-v1 cc1 at `-O2 -G8
# -fno-builtin`, the TU contract of main/tu257. Each call expands inline to one
# `abs.s` and emits no call (`return __builtin_fabsf(x);` -> `j $31; abs.s
# $f0,$f12`; three calls -> three `abs.s`), where plain `fabsf(x)` under
# -fno-builtin is a call to fabsf and `x < 0 ? -x : x` is a `c.lt.s`/`bc1tl`
# branch with `neg.s`. Two identical calls may be merged by CSE into one
# `abs.s`, as in main/tu257 ACT_updatePlayer (0x0030afb8), whose
# hardware-class lq/sq copy is the reason this admission was needed.
#
# __builtin_sqrtf (user decision 2026-10-10, "Admite ese builtin si es algo que
# podía usar el programador original del juego"): unlike fabsf it is NOT a
# single instruction with no libcall. Measured with the same contract cc1 (and
# the stock ee-gcc 2.96-ee-001003-1 cc1, identically) at `-O2 -G8 -fno-builtin`,
# `return __builtin_sqrtf(x);` expands to an inline `sqrt.s $f0,$f12`, then
# `c.eq.s $f0,$f0` and `bc1t` over a `jal sqrtf` fallback taken only when the
# result is NaN (the errno path); the scheduler may turn the `bc1t` into
# `bc1tl` and move independent work between the pieces. Plain `sqrtf(x)` under
# -fno-builtin is an ordinary call (`j sqrtf`), and an undeclared non-builtin
# `__builtin_*` name draws an implicit-declaration warning, which this one
# does not. That guarded shape is exactly what the original main ELF carries:
# all 100 `sqrt.s` of SLUS_204.69 .text (47 functions, e.g. LOOK_head_cont at
# 0x00317618) are followed by `c.eq.s` of the result with itself, a
# `bc1t`/`bc1tl` (66/34) and a `jal sqrtf` (0x003240f8), and those are all 100
# `jal sqrtf` in the binary. Under the contract only this builtin reaches that
# shape, and GCC 2.96 provided it to the original programmer.
#
# Every other `__builtin_*` stays refused here, and a builtin inside an inline
# asm operand stays refused by `_cop2_check_operands`.
_HARDWARE_C_BUILTINS = frozenset(('__builtin_fabsf', '__builtin_sqrtf'))
_HARDWARE_C_BUILTIN_USE = re.compile(r'\b(?P<name>__builtin_[A-Za-z_0-9]+)(?P<call>\s*\()?')


def _hardware_c_builtin_calls(text, label):
    """The closed builtin admission reused by composed native-TI C."""
    for found in _HARDWARE_C_BUILTIN_USE.finditer(text):
        require(found['name'] in _HARDWARE_C_BUILTINS,
                'uncalibrated builtin helper in %s' % label)
        require(found['call'],
                'calibrated builtin %s is admitted in %s only as a direct call'
                % (found['name'], label))


def hardware_native_ti_c(text, staged=None, headers=()):
    """One function that does BOTH kinds of already-authorized work.

    A function classified `hardware` inside a TU classified `native-ti`
    (`--source-class native-ti --function-class NAME=hardware --asm-contract P`;
    tools/tu_audit.py `admission`). It exists because ov01/tu030
    `fnECM01_DP000` measurably contains both at once: 18 COP2/VU0-macro
    instructions with no C expression at all, one hand-written SDK-shape
    quadword copy (`lq $8,0($2)` / `sq $8,0($4)` off `addiu`-computed bases --
    the `gprquad` entry), and eight compiler-emitted 128-bit aggregate copies in
    the allocator-folded shape (`lq $2,224($18)` / `sq $2,16($29)`) that only
    the native-TI typedef produces. docs/native-ti.md names that TU as the
    native-TI side of the `lq`/`sq` boundary; docs/ee-reference/vu.md covers the
    COP2 side. Under either class alone the other third of the function cannot
    be written, so the function was unreachable for reasons of tool structure
    rather than evidence.

    This composes the two admissions; it does not widen either:

    * the hardware side is `hardware_c` unchanged -- the same calibrated line
      table, the same operand/state validation, the same `--asm-contract`
      requirement in the audit, the same `exact_c_with_asm` category. In
      particular the compiler-emitted `lq $2,160($19)` spelling stays refused
      inline, so the native-TI declaration cannot be used to write native-TI
      output as assembly instead;
    * the non-hardware side is `native_ti.source`, which is `ordinary_c` plus
      exactly one whole-line file-scope `typedef unsigned int N
      __attribute__((mode(TI)));` and MORE refusals than `ordinary_c` has
      (digraphs, trigraphs, line escapes, uncalibrated builtins, `__extension__`,
      `__int128`, `typeof`, preprocessing that supplies braces or type syntax).
      Every other attribute, pragma, asm alias and alternate native-type
      spelling stays refused.

    The already-calibrated direct-call builtins are checked by the shared
    hardware builtin gate in the native-TI remainder; native-TI alone, TU free
    text and headers still refuse every builtin. The ordering matters and is the whole implementation: the
    hardware statements are masked FIRST, so the native-TI grammar never sees an
    assembly template (its `\\` refusal would reject `"...\\n\\t"`), and the
    native-TI grammar with that closed builtin gate governs the remaining text, including the
    prelude the audit prepends to every function body.

    The bytes stay the arbiter. A native-TI copy and an inline quadword copy
    assemble to different instruction streams, so the whole-file gate still
    decides which one the original had; this admission only stops the two from
    being unwritable in the same function.
    """
    from tools import native_ti          # imports ordinary_c from this module
    return hardware_c(text, staged, headers, inner=native_ti.hardware_source)


# ---------------------------------------------------------------------------
# `ee-mmi`: constrained inline EE MMI assembly (user decision 2026-10-10)
# ---------------------------------------------------------------------------
# The user's decision of 2026-10-10 ("Acepta MMI inline asm, hazlo una regla del
# proyecto, que el MMI se acepte como inline asm") admits EE MMI (128-bit
# multimedia) instructions written as constrained GCC inline assembly inside
# otherwise readable C, as the admitted source class `ee-mmi` (docs/ee-mmi.md).
# cc1 2.96 emits no MMI for scalar or aggregate C (config/compiler-patterns.json
# CP-0169; a 128-bit C operation fails with "unsupported wide integer
# operation"), so an original whose bytes contain MMI wrote it inline; the
# measurement that earned the class is main/tu219 MMathIsVectorEqual /
# MMathIsVectorEqual4 (`lq`, `pxor`, `pexew`/`pcpyud`, 64-bit `movn`).
#
# The class is a closed grammar, not "any assembly in a function that also has
# MMI". A block is admitted only when every line is one of:
#
#   * an MMI instruction (`_MMI_SIGNATURES`: the P* family, `qfsrv`, and the
#     pipeline-1 `mfhi1`/`mflo1`/`mthi1`/`mtlo1`/`mult1`/`multu1`/`div1`/
#     `divu1`/`madd1`/`maddu1`), with the operand count and kinds of its
#     encoding;
#   * the GPR quadword `lq`/`sq` with a `%N` base and a literal, 16-byte
#     aligned displacement (`_gpr_quad_off`);
#   * the 64/128-bit `movn`/`movz` conditional move, only when its condition
#     register was produced by an MMI line or an `lq` earlier in the block --
#     the select "used together with them";
#   * a result default `addi`/`addiu`/`daddiu`/`ori rd,$0,imm`, only when the
#     same register is the destination of a LATER `movn`/`movz` in the block
#     (the default value that select overrides; MMathIsVectorEqual's
#     `addi v0,$0,1` is the measured case: cc1 spells `li` as `addiu`, so an
#     `addi` in the original is the original's own inline text);
#   * a trailing `nop`, as the COP2 route's delay-slot filler.
#
# Every block must contain at least one MMI instruction or `lq`/`sq`; since the
# user's follow-up decision of 2026-10-10 a block of only `lq`/`sq` (a 128-bit
# zero-store `sq $0,0(%N)` or a 16-byte copy through a forced base) is admitted,
# while a scalar select or result default alone is not. Registers are `%N` operands
# wherever a C value is involved -- addresses, inputs, results; a hard-coded
# `$N` is admitted only as block-local 128-bit scratch, because no C type of the
# pinned compiler can carry a 128-bit value through an operand without the
# native-TI typedef: it must be written in the block before it is read, declared
# in the clobber list, and never be `$1`, `$26`-`$29` or `$31`. A block with an
# `sq` must declare `"memory"`.
#
# Everything else -- branches, labels, other scalar arithmetic, loads/stores
# other than `lq`/`sq`, COP0/COP1/COP2, `.word` and every directive -- stays
# refused: this route is MMI operations inside real C, never a precedent for
# non-MMI assembly. Whole-file exactness and tools/tu_audit.py identity/emission
# still decide acceptance, and the category is `exact_c_with_asm`.
_MMI_CONSTRAINTS = frozenset(('r', '=r', '+r', '=&r'))
_MMI_REG = r'(?:%[0-9]|\$(?:3[01]|[12][0-9]|[0-9]))'
_MMI_LINE = re.compile(r'(?P<op>[a-z][a-z0-9]*(?:\.[a-z]+)?)(?:[ \t]+(?P<args>\S.*?))?[ \t]*')
_MMI_MEM = re.compile(r'(?P<off>0[xX][0-9a-fA-F]+|[0-9]+)' + _WS + r'\(' + _WS + r'%(?P<base>[0-9])' + _WS + r'\)')
_MMI_IMM = re.compile(r'-?(?:0[xX][0-9a-fA-F]+|[0-9]+)')
_MMI_RESERVED = frozenset(('1', '26', '27', '28', '29', '31'))

# Operand signatures: `w` a written GPR, `r` a read GPR, `i` a literal shift
# amount. A mnemonic may have more than one accepted signature.
_MMI_SIGNATURES = {}


def _mmi_signature(names, *signatures):
    for name in names.split():
        _MMI_SIGNATURES[name] = signatures


_mmi_signature('paddb paddh paddw paddsb paddsh paddsw paddub padduh padduw padsbh '
               'psubb psubh psubw psubsb psubsh psubsw psubub psubuh psubuw '
               'pand por pxor pnor pceqb pceqh pceqw pcgtb pcgth pcgtw '
               'pmaxh pmaxw pminh pminw pextlb pextlh pextlw pextub pextuh pextuw '
               'ppacb ppach ppacw pcpyld pcpyud pinth pinteh qfsrv '
               'psllvw psrlvw psravw', 'wrr')
_mmi_signature('pexew pexcw pexch prevh prot3w pabsh pabsw pcpyh plzcw ppac5 pext5', 'wr')
_mmi_signature('psllh psrlh psrah psllw psrlw psraw', 'wri')
_mmi_signature('pmfhi pmflo mfhi1 mflo1 pmfhl.lw pmfhl.uw pmfhl.slw pmfhl.lh pmfhl.sh', 'w')
_mmi_signature('pmthi pmtlo mthi1 mtlo1 pmthl.lw', 'r')
# Multiply/accumulate: the destination may be omitted, or be `$0` (HI/LO only).
_MMI_MULTIPLY = frozenset(('pmultw', 'pmultuw', 'pmaddw', 'pmadduw', 'pmsubw', 'pmulth', 'pmaddh',
                           'pmsubh', 'phmadh', 'phmsbh', 'mult1', 'multu1', 'madd1', 'maddu1'))
_mmi_signature('pmultw pmultuw pmaddw pmadduw pmsubw pmulth pmaddh pmsubh phmadh phmsbh', 'wrr')
_mmi_signature('mult1 multu1 madd1 maddu1', 'rr', 'wrr')
_mmi_signature('div1 divu1 pdivw pdivuw pdivbw', 'rr')
_MMI_SELECT = frozenset(('movn', 'movz'))
_MMI_DEFAULT = frozenset(('addi', 'addiu', 'daddiu', 'ori'))
_MMI_CLOBBER_NAMES = frozenset(('memory', 'hi', 'lo'))


def _mmi_parse_line(line):
    """(op, [(role, token)], mem) for one admitted line; raise on anything else."""
    m = _MMI_LINE.fullmatch(line)
    require(m is not None, 'unadmitted ee-mmi inline line: ' + line[:120])
    op = m.group('op')
    args = [a.strip(' \t') for a in (m.group('args') or '').split(',')] if m.group('args') else []
    mem = None
    if op in ('lq', 'sq'):
        require(len(args) == 2 and re.fullmatch(_MMI_REG, args[0]),
                'ee-mmi %s must be `%s $N|%%N,OFF(%%N)`: %s' % (op, op, line[:120]))
        mem = _MMI_MEM.fullmatch(args[1])
        require(mem is not None,
                'ee-mmi %s address must be a literal displacement off a %%N operand base: %s'
                % (op, line[:120]))
        err = _gpr_quad_off(mem.group('off'))
        require(err is None, '%s in ee-mmi inline: %s' % (err, line[:120]))
        return op, [('w' if op == 'lq' else 'r', args[0])], mem
    if op in _MMI_SELECT:
        signature = ('wrr',)
    elif op in _MMI_DEFAULT:
        require(len(args) == 3 and re.fullmatch(_MMI_REG, args[0]) and args[1] == '$0'
                and _MMI_IMM.fullmatch(args[2]),
                'ee-mmi result default must be `%s rd,$0,IMM`: %s' % (op, line[:120]))
        value = int(args[2], 0)
        low, high = (0, 0xffff) if op == 'ori' else (-0x8000, 0x7fff)
        require(low <= value <= high, 'ee-mmi %s immediate out of range: %s' % (op, line[:120]))
        return op, [('w', args[0])], None
    else:
        signature = _MMI_SIGNATURES.get(op)
        require(signature is not None, 'unadmitted ee-mmi inline instruction: ' + line[:120])
    for sig in signature:
        if len(sig) != len(args):
            continue
        roles = []
        for role, token in zip(sig, args):
            if role == 'i':
                if not re.fullmatch(r'0[xX][0-9a-fA-F]+|[0-9]+', token):
                    break
                limit = 15 if op.endswith('h') else 31
                require(0 <= int(token, 0) <= limit,
                        'ee-mmi shift amount out of range (0-%d): %s' % (limit, line[:120]))
                continue
            if not re.fullmatch(_MMI_REG, token):
                break
            roles.append((role, token))
        else:
            return op, roles, None
    require(False, 'ee-mmi %s operands do not match its encoding (%s): %s'
            % (op, ' or '.join(signature), line[:120]))


def _mmi_check_clobbers(section):
    seen = set()
    for item in _split_top(section, ','):
        stripped = item.strip(' \t\r\n')
        if not stripped:
            continue
        require(_COP2_CLOBBER_LIT.fullmatch(stripped), 'ee-mmi clobbers must be single string literals')
        name = _literal_prefix(stripped)[0]
        require(name in _MMI_CLOBBER_NAMES or _COP2_CLOBBER_GPR.fullmatch(name),
                'unadmitted ee-mmi clobber: ' + name[:40])
        require(name not in seen, 'duplicate ee-mmi clobber: ' + name[:40])
        seen.add(name)
    return seen


def _mmi_block(body, outputs, inputs, clobbers, *, rows=None):
    """Check one ee-mmi block as a dataflow, in order (see the section comment)."""
    if rows is None:
        rows = [_mmi_parse_line(line) + (line,) for line in body]
    # User decision 2026-10-10 (follow-up): a block of only `lq`/`sq` 128-bit
    # loads/stores is admitted too (main/tu246 `sq $0,0(%0)` zero-stores,
    # main/tu205 and main/tu263 16-byte copies through a forced base). A select
    # or a result default alone is still not MMI.
    require(any(op in _MMI_SIGNATURES or op in ('lq', 'sq') for op, _, _, _ in rows),
            'an ee-mmi inline block must perform at least one MMI instruction or lq/sq; '
            'movn/movz or a result default alone is not MMI')
    require(not any(op == 'sq' for op, _, _, _ in rows) or 'memory' in clobbers,
            'an ee-mmi block that stores with sq must declare a "memory" clobber')
    require(all(c[0] in '=+' for c in outputs), 'ee-mmi output constraints must be =r, =&r or +r')
    require(all(c[0] not in '=+' for c in inputs), 'ee-mmi input constraints must be r')
    operands = len(outputs) + len(inputs)
    mmi_written = set()     # registers whose current value an MMI line or lq produced
    written = set()         # registers (hard-coded or %N) written so far in this block
    for index, (op, roles, mem, line) in enumerate(rows):
        if mem is not None:
            base = int(mem.group('base'), 10)
            require(base < operands, 'ee-mmi operand reference out of range: %' + mem.group('base'))
        # Reads first: a line that reads and writes one register reads the old value.
        for role, token in roles:
            if token.startswith('%'):
                n = int(token[1:], 10)
                require(n < operands, 'ee-mmi operand reference out of range: ' + token)
                continue
            if role == 'r':
                require(token[1:] == '0' or token in written,
                        'ee-mmi reads hard-coded %s before this block writes it; a value from C '
                        'must arrive through a %%N operand: %s' % (token, line[:120]))
        if op in _MMI_SELECT:
            # movn/movz keep the destination's old value when they do not move,
            # so the destination is read too: it must hold the block's own
            # default or be a read-write operand.
            target = roles[0][1]
            require(target in written or (target.startswith('%')
                                          and outputs[int(target[1:], 10)].startswith('+')),
                    'ee-mmi %s destination %s must first receive a default in this block or be '
                    'a +r operand: %s' % (op, target, line[:120]))
        for role, token in roles:
            if role != 'w':
                continue
            if token.startswith('%'):
                n = int(token[1:], 10)
                require(n < len(outputs),
                        'ee-mmi writes %s, which is not an output operand: %s' % (token, line[:120]))
                written.add(token)
                continue
            reg = token[1:]
            if reg == '0':
                require(op in _MMI_MULTIPLY,
                        'ee-mmi destination must not be $0: %s' % line[:120])
                continue
            require(reg not in _MMI_RESERVED,
                    'ee-mmi must not write reserved register %s: %s' % (token, line[:120]))
            require(token in clobbers,
                    'a hard-coded ee-mmi scratch register must be declared as a clobber: %s is '
                    'written by this line and is not in the clobber list: %s' % (token, line[:120]))
            written.add(token)
        if op in _MMI_SELECT:
            condition = roles[2][1]
            require(condition in mmi_written,
                    'ee-mmi %s must select on a register an MMI line or lq produced earlier in '
                    'this block: %s' % (op, line[:120]))
        if op in _MMI_DEFAULT:
            target = roles[0][1]
            require(any(later_op in _MMI_SELECT and later_roles[0][1] == target
                        for later_op, later_roles, _, _ in rows[index + 1:]),
                    'an ee-mmi result default is admitted only as the value a later movn/movz '
                    'into the same register overrides: %s' % line[:120])
        for role, token in roles:
            if role != 'w':
                continue
            if op in _MMI_SIGNATURES or op == 'lq':
                mmi_written.add(token)
            else:
                mmi_written.discard(token)


def _mmi_parse_construct(text, start):
    keyword = _COP2_KEYWORD.match(text, start)
    if not keyword:
        return None
    head = _COP2_HEAD.match(text, keyword.end())
    if not head:
        return None
    depth, i, n = 0, head.end() - 1, len(text)
    instr, inch, esc = False, False, False
    while i < n:
        ch = text[i]
        if instr or inch:
            if esc:
                esc = False
            elif ch == '\\':
                esc = True
            elif instr and ch == '"':
                instr = False
            elif inch and ch == "'":
                inch = False
        elif ch == '"':
            instr = True
        elif ch == "'":
            inch = True
        elif ch == '(':
            depth += 1
        elif ch == ')':
            depth -= 1
            if not depth:
                break
        i += 1
    require(i < n, 'unbalanced parentheses in ee-mmi inline')
    sections = _split_top(text[head.end():i], ':')
    require(len(sections) <= 4, 'too many sections in ee-mmi inline')
    decoded = _template_bodies(sections[0])
    require(decoded is not None, 'ee-mmi template must be string literal(s)')
    lines = [raw.strip(' \t') for raw in decoded.split('\n')]
    lines = [line for line in lines if line]
    require(lines, 'empty ee-mmi inline template')
    body = lines[:-1] if lines[-1] == 'nop' and len(lines) > 1 else lines
    outputs = _cop2_check_operands(sections[1], 'output', _MMI_CONSTRAINTS) if len(sections) >= 2 else []
    inputs = _cop2_check_operands(sections[2], 'input', _MMI_CONSTRAINTS) if len(sections) >= 3 else []
    clobbers = _mmi_check_clobbers(sections[3]) if len(sections) >= 4 else set()
    _mmi_block(body, outputs, inputs, clobbers)
    return i + 1


def _mmi_inline_spans(text):
    found = []
    for m in _COP2_FIND.finditer(text):
        if m.group(0) not in ('__asm__', '__asm', 'asm'):
            continue
        end = _mmi_parse_construct(text, m.start())
        if end is not None:
            found.append((m.start(), end))
    return found


def mmi_c(text, staged=None, headers=(), inner=None):
    """Readable C with constrained inline EE MMI assembly (`ee-mmi`, 2026-10-10).

    Each `__asm__ __volatile__(...)` block must pass `_mmi_block`; once the
    admitted blocks and `register T v asm("$N")` bindings are masked, what is
    left must be ordinary candidate C (or native-TI C through `inner`). Returns
    the number of admitted blocks; the audit reports a function with at least
    one as `exact_c_with_asm`.
    """
    require(text.isascii() and '\x00' not in text and '??' not in text and '%:' not in text
            and _hardware_line_continuations_ok(text), 'ee-mmi C encoding or preprocessing escape')
    require(not re.search(r'^\s*#.*\b(?:asm|__asm|__asm__)\b', text, re.M),
            'inline assembly cannot be supplied by a preprocessor directive')
    tokens = r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|/\*.*?\*/|//[^\n]*'
    masked = re.sub(tokens, lambda m: ' ' if m[0].startswith(('/*', '//')) else m[0], text, flags=re.S)
    spans = _mmi_inline_spans(masked)
    chars = list(masked)
    for start, end in spans:
        for i in range(start, end):
            if chars[i] != '\n':
                chars[i] = ' '
    remainder = _cop2_register_bindings(''.join(chars))
    (inner or ordinary_c)(remainder, staged, headers)
    _hardware_c_builtin_calls(remainder, 'ee-mmi C')
    return len(spans)


def mmi_native_ti_c(text, staged=None, headers=()):
    """An `ee-mmi` function inside a `native-ti` TU: the MMI blocks are admitted
    by `mmi_c`, everything else (including the TU prelude's file-scope TI
    typedef) by `native_ti.source`. Neither route is widened."""
    from tools import native_ti          # imports ordinary_c from this module
    return mmi_c(text, staged, headers, inner=native_ti.hardware_source)


def _mmi_cop2_transpose_block(body, outputs, inputs, clobbers):
    """The two original NML weight transposes (user decision 2026-10-18).

    Reuse the calibrated COP2 lines and the MMI operand/dataflow checker. Only
    the evidenced seven word-shuffle instructions join them; scalar assembly,
    arbitrary MMI/COP2 composition and hard-coded pointer inputs stay refused.
    tools/tu_audit.py selects this composition only for the two original IDs.
    """
    require(not outputs and inputs == ['r', 'r', 'r'],
            'the NML MMI/COP2 transpose has three r pointer inputs and no outputs')
    require(clobbers == {'$' + str(n) for n in range(2, 10)} | {'memory'},
            'the NML MMI/COP2 transpose clobbers exactly $2-$9 and memory')
    expected = ['pextlw $6,$3,$2', 'pextuw $7,$3,$2',
                'pextlw $8,$5,$4', 'pextuw $9,$5,$4',
                'pcpyld $2,$8,$6', 'pcpyud $3,$6,$8', 'pcpyld $4,$9,$7']
    shuffles, rows, loads, from_vu, to_vu, events = [], [], [], [], [], []
    for line in body:
        op = line.split(None, 1)[0]
        if op in _MMI_SIGNATURES or op in ('lq', 'sq'):
            row = _mmi_parse_line(line)
            require(op in ('lq', 'pextlw', 'pextuw', 'pcpyld', 'pcpyud'),
                    'unadmitted instruction in bounded NML transpose: ' + line[:120])
            if op == 'lq':
                loads.append(re.sub(_SEP, ',', re.sub(r'[ \t]+', ' ', line)))
            else:
                shuffles.append(re.sub(_SEP, ',', re.sub(r'[ \t]+', ' ', line)))
            events.append(re.sub(_SEP, ',', re.sub(r'[ \t]+', ' ', line)))
            rows.append(row + (line,))
            continue
        found = _cop2_match_line(line)
        require(found is not None and found[0] in ('transfer', 'move', 'plain3', 'bc3', 'accbc3', 'move2'),
                'uncalibrated line in bounded NML MMI/COP2 transpose: ' + line[:120])
        kind, parsed, check = found
        error = check(parsed)
        require(not error, str(error) + ' in NML MMI/COP2 transpose: ' + line[:120])
        roles = []
        if kind == 'transfer':
            base = parsed.group('base')
            require(base in ('%0', '%1', '%2') and not parsed.group('off').startswith('%'),
                    'NML COP2 memory bases must be C pointer operands with literal offsets')
        elif kind == 'move':
            reg = parsed.group('gpr')
            roles = [('w' if op.startswith('qmfc2') else 'r', reg)]
            (from_vu if op.startswith('qmfc2') else to_vu).append((reg, parsed.group('src')))
            events.append('%s %s,vf%s' % (parsed.group('op'), reg, parsed.group('src')))
        rows.append((op, roles, None, line))
    require(shuffles == expected, 'the bounded NML MMI transpose must retain its seven original shuffles')
    require(loads == ['lq $2,0(%2)', 'lq $3,16(%2)', 'lq $4,32(%2)'],
            'the bounded NML transpose loads three quadwords through its third C input')
    require(from_vu == [('$5', '0')] and to_vu == [('$2', '5'), ('$3', '6'), ('$4', '7')],
            'the bounded NML transpose bridges vf0 and vf5-vf7 through its declared GPR scratch')
    require(events == loads + ['qmfc2 $5,vf0'] + expected +
            ['qmtc2 $2,vf5', 'qmtc2 $3,vf6', 'qmtc2 $4,vf7'],
            'NML COP2 bridges must consume the completed transpose, in original producer order')
    _mmi_block(body, outputs, inputs, clobbers, rows=rows)


def mmi_cop2_c(text, staged=None, headers=(), inner=None):
    """Identity-scoped NML MMI/COP2 composition; not a general hardware widening."""
    return hardware_c(text, staged, headers, inner=inner, mmi_transpose=True)


def mmi_cop2_native_ti_c(text, staged=None, headers=()):
    from tools import native_ti
    return mmi_cop2_c(text, staged, headers, inner=native_ti.hardware_source)


def sef_particle_scale_c(text, staged=None, headers=(), inner=None):
    """Identity-scoped COP1 load exception, selected only by TU audit admission."""
    tokens = re.sub(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|/\*.*?\*/|//[^\n]*',
                    ' ', text, flags=re.S)
    require(not re.search(r'\bregister\b', tokens),
            'sefCreateParticle scale exception does not admit register declarations or bindings')
    return hardware_c(text, staged, headers, inner=inner, sef_particle_scale=True)


def sef_particle_scale_native_ti_c(text, staged=None, headers=()):
    from tools import native_ti
    return sef_particle_scale_c(text, staged, headers, inner=native_ti.hardware_source)


def vu0_i_immediate_c(text, staged=None, headers=(), inner=None):
    """Identity-scoped StartOfScan `lui $2,0x3e00` -> `ctc2 $2,$vi21` admission,
    selected only by TU audit admission (main/tu097 StartOfScan, 2026-10-28)."""
    return hardware_c(text, staged, headers, inner=inner, vu0_i_immediate=True)


def vu0_i_immediate_native_ti_c(text, staged=None, headers=()):
    from tools import native_ti
    return vu0_i_immediate_c(text, staged, headers, inner=native_ti.hardware_source)


def xrg_clip_nops_c(text, staged=None, headers=(), inner=None):
    """Identity-scoped admission of _Disp's three clip-sequence `nop` lines,
    selected only by TU audit admission (ov12/tu084 _Disp, 2026-11-04)."""
    return hardware_c(text, staged, headers, inner=inner, xrg_clip_nops=True)


def xrg_clip_nops_native_ti_c(text, staged=None, headers=()):
    from tools import native_ti
    return xrg_clip_nops_c(text, staged, headers, inner=native_ti.hardware_source)


# ---------------------------------------------------------------------------
# `ee-asm-leaf` and `ee-fpu`: original hand-written assembly delivered as C with
# inline asm (user decision 2026-10-10)
# ---------------------------------------------------------------------------
# The user's decision of 2026-10-10 on main/tu100 ("Sobre tu100, investiga si
# realmente eran asm originalmente: Si realmente hay indicios de ser asm
# original, debería dejarse como C with asm") admits, as `exact_c_with_asm`,
# functions whose original bytes carry hand-written-assembly fingerprints that
# no C spelling of the pinned compiler reproduces (docs/ee-hand-asm.md). Two
# closed grammars, each as narrow as the evidence:
#
# `ee-asm-leaf` -- the xglMemCopy64/64b/16/16b/8/8b/4/4b/2/2b block-copy loops
# (main 0x228e08..0x228fc4). Fingerprints: the trapping `addi` (cc1 2.96 always
# spells pointer and counter steps `addiu`), the `bne $0,reg` operand order (cc1
# writes `bne reg,$0`), an unfilled loop delay slot, and a fixed `$2` scratch. A
# block is admitted only when it is the ONLY statement of its function (a leaf
# whose whole body is the loop; nothing else in C), every operand is a `+r`
# function parameter, and its lines are exactly, in order:
#
#   * one local numeric label `N:` (the loop head; alone or prefixing the first
#     transfer);
#   * one or more transfer pairs `lX $T,OFF(%S)` / `sX $T,OFF(%D)` of one width
#     (`lq`/`sq` 16, `ld`/`sd` 8, `lw`/`sw` 4, `lh`/`sh` 2) through one hard-coded,
#     clobbered scratch `$T` (never `$0`, `$1`, `$26`-`$29`, `$31`), loads off one
#     operand and stores off another, pair k at displacement k*width in the
#     direction of the step;
#   * three steps `addi|addiu %X,%X,IMM`, one per operand: the count by -1, the
#     destination and source by the same stride, +-(pairs*width);
#   * `bne $0,%C,Nb` (or `bne %C,$0,Nb`) on the count, back to the head label;
#   * optionally a trailing `nop` (the delay slot).
#
# `ee-fpu` -- F2I (main 0x22ab98, `mfc1 v1,$f12; mtc1 v1,$f0; cvt.w.s
# $f0,$f0; mfc1 v0,$f0`): a GPR round trip no C conversion produces (cc1 folds
# every spelling to `cvt.w.s $f0,$f12`), the same "r"-operand idiom as the
# published xglFSrand. A block is admitted when its lines are only `mtc1
# %N,$fK` (N an `r` input), `cvt.w.s`/`cvt.s.w $fA,$fB`, `mfc1 %N,$fK` (N an
# `=r`/`=&r` output) and `nop`, with at least one conversion, every FPR it
# writes hard-coded and clobbered, every conversion reading an FPR the block
# loaded, every `mfc1` reading a converted FPR, and every output written.
#
# Everything else stays refused in both: calls, jumps, branches to C labels or
# out of the block, other labels, directives (`.set`, `.word`, ...), raw
# opcodes, `register ... asm` bindings, and any other instruction. Neither class
# is a precedent for other assembly; the whole-file gate and tools/tu_audit.py
# still decide acceptance.
#
# `ee-vu-clip` (user decision 2026-10-11 on main/tu109: "Si es parecido a tu100
# y seguro que es asm originalmente, adelante") -- _ModelCalcClip,
# _ModelCalcClipMat1 and _ModelCalcClipMat2 (main 0x23ef78..0x23f358). The
# original is itself a C function around one inline asm block: VU0 macro code,
# a chain of `qmfc2 $T / mtc1 $T,$fK / c.lt.s / bc1f` lane tests, and the
# hand-written set/clear join `sub $T,$T,$T; j exit` / `set: sub $T,$T,$T;
# addi $T,$T,1` / `exit: move %0,$T`. Fingerprints: the eight `sub` of the whole
# executable are these joins (and a sister copy in sefClipViewVolumeA); cc1
# 2.96 never emits `sub` or a trapping `addi` (none in the 63 cc1 TUs of main);
# the delay slots are unfilled; the scratch is fixed; the join labels survive
# as `_$mcc*_set_end`/`_clr_end`/`_exit` symbols, the `_$` convention of the
# xglMemCopy `_$mc16_loop` labels; splat marks the three "Handwritten". A block
# is admitted only as the WHOLE body `{ T out; __asm__ __volatile__(...);
# return out; }`, one `=r` output, `r` inputs that are the function's
# parameters, and these lines, in order:
#
#   * a body of calibrated VU0 macro lines of four families only -- `lqc2`
#     loads off an input operand (`transfer`), the broadcast and ACC-broadcast
#     arithmetic (`bc3`, `accbc3`) and `vmove`/`vmr32` (`move2`) -- interleaved
#     with lane tests: `qmfc2 $T,vfN`, then `mtc1 $T,$fK`, and `c.lt.s $fA,$fB`
#     (both FPRs loaded by this block) immediately followed by `bc1f Nf`;
#   * the join, exactly: `sub $T,$T,$T`, `j Mf`, `N:`, `sub $T,$T,$T`,
#     `addi $T,$T,1`, `M:`, `move %0,$T` (N, M distinct digits 1-9; every
#     `bc1f` goes to N).
#
# `$T` is one hard-coded scratch GPR (never `$0`, `$1`, `$26`-`$29`, `$31`);
# the clobbers are exactly `$T`, the FPRs the block loads, and "memory". No
# `nop` (the assembler inserts the hazard and delay-slot fillers), no other
# label, branch, jump, store or instruction, no `register ... asm` binding.
_LEAF_PAIRS = {'lq': ('sq', 16), 'ld': ('sd', 8), 'lw': ('sw', 4), 'lh': ('sh', 2)}
_LEAF_STORES = {store for store, _ in _LEAF_PAIRS.values()}
_ASM_IMM = r'-?(?:0[xX][0-9a-fA-F]+|[0-9]+)'
_LEAF_LABEL = re.compile(r'(?P<n>[1-9]):(?:' + _WS + r'(?P<rest>\S.*))?')
_LEAF_MOVE = re.compile(r'(?P<op>lq|sq|ld|sd|lw|sw|lh|sh)' + _WS + r'\$(?P<t>[0-9]|[12][0-9]|3[01])' + _SEP +
                        r'(?P<off>' + _ASM_IMM + r')' + _WS + r'\(' + _WS + r'%(?P<base>[0-9])' + _WS + r'\)')
_LEAF_STEP = re.compile(r'(?P<op>addi|addiu)' + _WS + r'%(?P<d>[0-9])' + _SEP + r'%(?P<s>[0-9])' + _SEP +
                        r'(?P<imm>' + _ASM_IMM + r')')
_LEAF_BRANCH = re.compile(r'bne' + _WS + r'(?:\$0' + _SEP + r'%(?P<a>[0-9])|%(?P<b>[0-9])' + _SEP + r'\$0)' +
                          _SEP + r'(?P<n>[1-9])b')
_LEAF_RESERVED = frozenset(('0', '1', '26', '27', '28', '29', '31'))
_FPR = r'\$f(?:[0-9]|[12][0-9]|3[01])'
_FPU_MOVE = re.compile(r'(?P<op>mtc1|mfc1)' + _WS + r'%(?P<gpr>[0-9])' + _SEP + r'(?P<f>' + _FPR + r')')
_FPU_CVT = re.compile(r'(?P<op>cvt\.w\.s|cvt\.s\.w)' + _WS + r'(?P<d>' + _FPR + r')' + _SEP + r'(?P<s>' + _FPR + r')')
_IDENT_TAIL = re.compile(r'([A-Za-z_][A-Za-z_0-9]*)\s*$')
_CLIP_GPR = r'\$(?P<t>[0-9]|[12][0-9]|3[01])'
_CLIP_LQC2 = re.compile(r'lqc2' + _WS + r'\$?vf(?P<d>[0-9]+)' + _SEP + r'(?P<off>0[xX][0-9a-fA-F]+|[0-9]+)' +
                        _WS + r'\(' + _WS + r'%(?P<base>[0-9])' + _WS + r'\)')
_CLIP_QMFC2 = re.compile(r'qmfc2' + _WS + _CLIP_GPR + _SEP + r'\$?vf(?P<src>[0-9]+)')
_CLIP_MTC1 = re.compile(r'mtc1' + _WS + _CLIP_GPR + _SEP + r'(?P<f>' + _FPR + r')')
_CLIP_CMP = re.compile(r'c\.lt\.s' + _WS + r'(?P<a>' + _FPR + r')' + _SEP + r'(?P<b>' + _FPR + r')')
_CLIP_BC1F = re.compile(r'bc1f' + _WS + r'(?P<n>[1-9])f')
_CLIP_VU_KINDS = frozenset(('transfer', 'bc3', 'accbc3', 'move2'))


def _inline_construct(text, start, label):
    """(end, template lines, [outputs, inputs, clobbers sections]) of one
    `__asm__ __volatile__(...)` construct at `start`, or None if not one."""
    keyword = _COP2_KEYWORD.match(text, start)
    if not keyword:
        return None
    head = _COP2_HEAD.match(text, keyword.end())
    if not head:
        return None
    depth, i, n = 0, head.end() - 1, len(text)
    instr, inch, esc = False, False, False
    while i < n:
        ch = text[i]
        if instr or inch:
            if esc:
                esc = False
            elif ch == '\\':
                esc = True
            elif instr and ch == '"':
                instr = False
            elif inch and ch == "'":
                inch = False
        elif ch == '"':
            instr = True
        elif ch == "'":
            inch = True
        elif ch == '(':
            depth += 1
        elif ch == ')':
            depth -= 1
            if not depth:
                break
        i += 1
    require(i < n, 'unbalanced parentheses in %s inline' % label)
    sections = _split_top(text[head.end():i], ':')
    require(len(sections) <= 4, 'too many sections in %s inline' % label)
    decoded = _template_bodies(sections[0])
    require(decoded is not None, '%s template must be string literal(s)' % label)
    lines = [line for line in (raw.strip(' \t') for raw in decoded.split('\n')) if line]
    require(lines, 'empty %s inline template' % label)
    return i + 1, lines, sections[1:] + [''] * (4 - len(sections))


def _inline_operand_pairs(section, kind, allowed, label):
    """[(constraint, expression)] of one operand section, in order."""
    pairs = []
    for item in _split_top(section, ','):
        if not item.strip(' \t\r\n'):
            continue
        parsed = _literal_prefix(item)
        require(parsed, '%s %s must be constraint (expression) pairs' % (label, kind))
        constraint, rest = parsed
        expr = rest.strip(' \t\r\n')
        require(expr.startswith('(') and expr.endswith(')') and len(expr) >= 2,
                '%s %s must be constraint (expression) pairs' % (label, kind))
        require(constraint in allowed, 'unadmitted %s %s constraint: %s' % (label, kind, constraint[:40]))
        inner = expr[1:-1].strip(' \t\r\n')
        require(inner and '#' not in inner and not _COP2_ASM_WORD.search(inner)
                and not _COP2_BUILTIN.search(inner), 'unsupported %s %s expression' % (label, kind))
        pairs.append((constraint, inner))
    return pairs


def _inline_clobber_names(section, label, admitted):
    seen = set()
    for item in _split_top(section, ','):
        stripped = item.strip(' \t\r\n')
        if not stripped:
            continue
        require(_COP2_CLOBBER_LIT.fullmatch(stripped), '%s clobbers must be single string literals' % label)
        name = _literal_prefix(stripped)[0]
        require(admitted(name), 'unadmitted %s clobber: %s' % (label, name[:40]))
        require(name not in seen, 'duplicate %s clobber: %s' % (label, name[:40]))
        seen.add(name)
    return seen


def _asm_imm(token):
    try:
        return int(token, 0)
    except ValueError:
        raise ValueError('inline immediate is not a plain decimal or hex literal: ' + token[:40])


def _leaf_block(lines, outputs, inputs, clobbers):
    """Check one ee-asm-leaf block (see the section comment); return the operands
    it binds, so the caller can require them to be the function's parameters."""
    label = 'ee-asm-leaf'
    require(not inputs, 'an %s block takes no input operands: every value is a +r parameter' % label)
    require(len(outputs) == 3 and all(c == '+r' for c, _ in outputs),
            'an %s block binds exactly three +r operands: destination, source and count' % label)
    head = _LEAF_LABEL.fullmatch(lines[0])
    require(head is not None, 'an %s block must open with its loop label `N:`: %s' % (label, lines[0][:120]))
    body = ([head.group('rest')] if head.group('rest') else []) + lines[1:]
    if body and body[-1] == 'nop':
        body = body[:-1]
    require(len(body) >= 6, 'an %s block is transfer pairs, three steps and the loop branch' % label)
    transfers, steps, branch = body[:-4], body[-4:-1], body[-1]
    require(len(transfers) % 2 == 0, 'an %s block moves data in load/store pairs' % label)
    scratch, width, source, destination, offsets = None, None, None, None, []
    for index in range(0, len(transfers), 2):
        load = _LEAF_MOVE.fullmatch(transfers[index])
        store = _LEAF_MOVE.fullmatch(transfers[index + 1])
        require(load is not None and load.group('op') in _LEAF_PAIRS,
                'unadmitted %s line (expected a load): %s' % (label, transfers[index][:120]))
        require(store is not None and store.group('op') in _LEAF_STORES,
                'unadmitted %s line (expected a store): %s' % (label, transfers[index + 1][:120]))
        op_store, op_width = _LEAF_PAIRS[load.group('op')]
        require(store.group('op') == op_store and (width is None or width == op_width),
                '%s pairs must all be one width (%s/%s): %s' % (label, load.group('op'), op_store,
                                                                transfers[index + 1][:120]))
        width = op_width
        require(load.group('t') == store.group('t') and (scratch is None or scratch == load.group('t')),
                'an %s block moves every value through one scratch register: %s' % (label, transfers[index][:120]))
        scratch = load.group('t')
        require(_asm_imm(load.group('off')) == _asm_imm(store.group('off')),
                'an %s store must write the displacement its load read: %s' % (label, transfers[index + 1][:120]))
        require((source is None or source == load.group('base'))
                and (destination is None or destination == store.group('base')),
                'an %s block loads off one operand and stores off another' % label)
        source, destination = load.group('base'), store.group('base')
        offsets.append(_asm_imm(load.group('off')))
    require(source != destination, 'an %s block must load and store through different operands' % label)
    require(scratch not in _LEAF_RESERVED, '%s scratch register $%s is reserved' % (label, scratch))
    require(clobbers == {'$' + scratch, 'memory'},
            'an %s block clobbers exactly its scratch register and "memory" ($%s, memory)' % (label, scratch))
    stride = {}
    for line in steps:
        step = _LEAF_STEP.fullmatch(line)
        require(step is not None and step.group('d') == step.group('s'),
                'unadmitted %s line (expected `addi %%N,%%N,IMM`): %s' % (label, line[:120]))
        require(step.group('d') not in stride, 'an %s block steps each operand once: %s' % (label, line[:120]))
        stride[step.group('d')] = _asm_imm(step.group('imm'))
    count = ({'0', '1', '2'} - {source, destination})
    require(set(stride) == {'0', '1', '2'} and len(count) == 1,
            'an %s block steps its destination, source and count operands once each' % label)
    count = count.pop()
    pairs = len(offsets)
    require(stride[count] == -1, 'an %s count steps by -1' % label)
    forward = stride[destination]
    require(forward == stride[source] and abs(forward) == pairs * width,
            'an %s block steps destination and source by the bytes one iteration copies (%d)'
            % (label, pairs * width))
    direction = 1 if forward > 0 else -1
    require(offsets == [direction * k * width for k in range(pairs)],
            'an %s block copies consecutive %d-byte units from its pointers in the step direction'
            % (label, width))
    loop = _LEAF_BRANCH.fullmatch(branch)
    require(loop is not None, 'unadmitted %s line (expected `bne $0,%%N,Nb`): %s' % (label, branch[:120]))
    require((loop.group('a') or loop.group('b')) == count and loop.group('n') == head.group('n'),
            'an %s loop branches on its count operand back to its own head label' % label)
    return [expr for _, expr in outputs]


def _leaf_function_parameters(masked, start, end):
    """The parameter names of the function whose WHOLE body is the construct at
    [start, end); raise unless the construct is that body's only statement."""
    label = 'ee-asm-leaf'
    counting = _brace_counting_text(masked)
    before = counting[:start].rstrip()
    require(before.endswith('{'), 'an %s block must be the first and only statement of its function' % label)
    brace = len(before) - 1
    require(counting[:brace].count('{') == counting[:brace].count('}'),
            'an %s block must be a function body, not a nested block' % label)
    require(re.match(r'\s*;\s*\}', counting[end:]),
            'an %s block must be the only statement of its function: no C may follow it' % label)
    header = counting[:brace].rstrip()
    require(header.endswith(')'), 'an %s block must be a function body' % label)
    depth, i = 0, len(header) - 1
    while i >= 0:
        if header[i] == ')':
            depth += 1
        elif header[i] == '(':
            depth -= 1
            if not depth:
                break
        i -= 1
    require(i >= 0, 'unbalanced %s function header' % label)
    name = _IDENT_TAIL.search(header[:i])
    require(name is not None and name.group(1) not in ('if', 'while', 'for', 'switch', 'return', 'sizeof'),
            'an %s block must be a function body' % label)
    names = set()
    for parameter in _split_top(masked[i + 1:len(header) - 1], ','):
        found = _IDENT_TAIL.search(parameter.strip())
        if found:
            names.add(found.group(1))
    return names


def _fpu_block(lines, outputs, inputs, clobbers):
    """Check one ee-fpu block as a dataflow, in order (see the section comment)."""
    label = 'ee-fpu'
    loaded, converted, written_outputs, conversions = set(), set(), set(), 0
    require(any(line != 'nop' for line in lines), 'an %s block must convert something' % label)
    for line in lines:
        if line == 'nop':
            continue
        move = _FPU_MOVE.fullmatch(line)
        if move is not None:
            n = int(move.group('gpr'), 10)
            fpr = move.group('f')
            if move.group('op') == 'mtc1':
                require(len(outputs) <= n < len(outputs) + len(inputs),
                        '%s mtc1 must read an `r` input operand: %s' % (label, line[:120]))
                require(fpr in clobbers, 'an %s FPR the block writes must be clobbered: %s' % (label, line[:120]))
                loaded.add(fpr)
                converted.discard(fpr)
            else:
                require(n < len(outputs), '%s mfc1 must write an output operand: %s' % (label, line[:120]))
                require(fpr in converted, '%s mfc1 must read an FPR this block converted: %s' % (label, line[:120]))
                written_outputs.add(n)
            continue
        cvt = _FPU_CVT.fullmatch(line)
        require(cvt is not None, 'unadmitted %s inline line: %s' % (label, line[:120]))
        require(cvt.group('s') in loaded or cvt.group('s') in converted,
                '%s conversion must read an FPR this block loaded: %s' % (label, line[:120]))
        require(cvt.group('d') in clobbers, 'an %s FPR the block writes must be clobbered: %s' % (label, line[:120]))
        loaded.discard(cvt.group('d'))
        converted.add(cvt.group('d'))
        conversions += 1
    require(conversions, 'an %s block must perform cvt.w.s or cvt.s.w; a bare transfer is C' % label)
    require(written_outputs == set(range(len(outputs))), 'every %s output must receive an mfc1' % label)


def _vu_clip_block(lines, outputs, inputs, clobbers):
    """Check one ee-vu-clip block (see the section comment), in order."""
    label = 'ee-vu-clip'
    require(len(outputs) == 1 and outputs[0][0] == '=r',
            'an %s block has exactly one `=r` output: the 0/1 test result' % label)
    require(inputs, 'an %s block reads its vectors through `r` input operands' % label)
    require(len(lines) >= 9, 'an %s block is VU0 lines, lane tests and the set/clear join' % label)
    join, body = lines[-7:], lines[:-7]
    clear = re.fullmatch(r'sub' + _WS + _CLIP_GPR + _SEP + r'\$(?P<s>[0-9]+)' + _SEP + r'\$(?P<u>[0-9]+)', join[0])
    require(clear is not None and clear.group('t') == clear.group('s') == clear.group('u'),
            'an %s join opens with `sub $T,$T,$T`: %s' % (label, join[0][:120]))
    scratch = clear.group('t')
    require(scratch not in _LEAF_RESERVED, '%s scratch register $%s is reserved' % (label, scratch))
    exit_jump = re.fullmatch(r'j' + _WS + r'(?P<n>[1-9])f', join[1])
    set_label = re.fullmatch(r'(?P<n>[1-9]):', join[2])
    exit_label = re.fullmatch(r'(?P<n>[1-9]):', join[5])
    expected = ['sub $%s,$%s,$%s' % ((scratch,) * 3), 'addi $%s,$%s,1' % (scratch, scratch),
                'move %%0,$%s' % scratch]
    require(exit_jump is not None and set_label is not None and exit_label is not None
            and exit_jump.group('n') == exit_label.group('n') != set_label.group('n')
            and [re.sub(_SEP, ',', re.sub(r'[ \t]+', ' ', join[k])) for k in (3, 4, 6)] == expected,
            'an %s block ends with exactly `sub $T,$T,$T; j Mf; N: sub $T,$T,$T; addi $T,$T,1; M: move %%0,$T`'
            % label)
    target = set_label.group('n')
    gpr_holds, fpr_loaded, used_inputs, tests, pending_test = False, set(), set(), 0, False
    for line in body:
        if pending_test:
            branch = _CLIP_BC1F.fullmatch(line)
            require(branch is not None and branch.group('n') == target,
                    'an %s `c.lt.s` is followed by `bc1f %sf`: %s' % (label, target, line[:120]))
            pending_test, gpr_holds = False, False
            tests += 1
            continue
        read = _CLIP_QMFC2.fullmatch(line)
        if read is not None:
            require(read.group('t') == scratch, '%s qmfc2 must write the scratch $%s: %s' % (label, scratch, line[:120]))
            require(not _cop2_vf(read.group('src'), ''), 'bad VU register in %s: %s' % (label, line[:120]))
            gpr_holds = True
            continue
        move = _CLIP_MTC1.fullmatch(line)
        if move is not None:
            require(move.group('t') == scratch and gpr_holds,
                    '%s mtc1 must move the lane the preceding qmfc2 read: %s' % (label, line[:120]))
            require(move.group('f') in clobbers, 'an %s FPR the block writes must be clobbered: %s' % (label, line[:120]))
            fpr_loaded.add(move.group('f'))
            gpr_holds = False
            continue
        compare = _CLIP_CMP.fullmatch(line)
        if compare is not None:
            require(compare.group('a') in fpr_loaded and compare.group('b') in fpr_loaded,
                    'an %s compare must read FPRs this block loaded: %s' % (label, line[:120]))
            pending_test = True
            continue
        found = _cop2_match_line(line)
        require(found is not None and found[0] in _CLIP_VU_KINDS,
                'unadmitted %s inline line: %s' % (label, line[:120]))
        if found[0] == 'transfer':
            load = _CLIP_LQC2.fullmatch(line)
            require(load is not None, 'an %s block only loads (lqc2 off an input operand): %s' % (label, line[:120]))
            base = int(load.group('base'), 10)
            require(1 <= base < 1 + len(inputs), '%s lqc2 must address an input operand: %s' % (label, line[:120]))
            used_inputs.add(base)
        err = found[2](found[1])
        require(not err, '%s in %s inline: %s' % (err, label, line[:120]))
    require(not pending_test, 'an %s compare must be followed by its branch' % label)
    require(tests, 'an %s block tests at least one lane (c.lt.s + bc1f)' % label)
    require(used_inputs == set(range(1, 1 + len(inputs))), 'every %s input operand is an lqc2 base' % label)
    require(clobbers == {'$' + scratch, 'memory'} | fpr_loaded,
            'an %s block clobbers exactly its scratch, the FPRs it loads and "memory"' % label)


def _vu_clip_function_shape(masked, start, end, output, inputs):
    """Require the construct at [start, end) to be the whole body
    `{ T output; <construct>; return output; }` of a function whose parameters
    include every input expression."""
    label = 'ee-vu-clip'
    counting = _brace_counting_text(masked)
    before = counting[:start]
    brace = before.rfind('{')
    require(brace >= 0 and counting[:brace].count('{') == counting[:brace].count('}'),
            'an %s block must be a function body, not a nested block' % label)
    require(re.fullmatch(r'\s*(?:[A-Za-z_][A-Za-z_0-9]*\s+)+' + re.escape(output) + r'\s*;\s*', before[brace + 1:]),
            'an %s block is preceded only by the declaration of its output `%s`' % (label, output[:40]))
    require(re.match(r'\s*;\s*return\s+' + re.escape(output) + r'\s*;\s*\}', counting[end:]),
            'an %s block is followed only by `return %s;`' % (label, output[:40]))
    header = counting[:brace].rstrip()
    require(header.endswith(')'), 'an %s block must be a function body' % label)
    depth, i = 0, len(header) - 1
    while i >= 0:
        if header[i] == ')':
            depth += 1
        elif header[i] == '(':
            depth -= 1
            if not depth:
                break
        i -= 1
    require(i >= 0, 'unbalanced %s function header' % label)
    name = _IDENT_TAIL.search(header[:i])
    require(name is not None and name.group(1) not in ('if', 'while', 'for', 'switch', 'return', 'sizeof'),
            'an %s block must be a function body' % label)
    names = set()
    for parameter in _split_top(masked[i + 1:len(header) - 1], ','):
        found = _IDENT_TAIL.search(parameter.strip())
        if found:
            names.add(found.group(1))
    require(all(expr in names for expr in inputs), 'every %s input must be a parameter of its function' % label)


def _inline_class_c(text, staged, headers, inner, label, check, leaf):
    require(text.isascii() and '\x00' not in text and '??' not in text and '%:' not in text
            and _hardware_line_continuations_ok(text), '%s C encoding or preprocessing escape' % label)
    require(not re.search(r'^\s*#.*\b(?:asm|__asm|__asm__)\b', text, re.M),
            'inline assembly cannot be supplied by a preprocessor directive')
    tokens = r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|/\*.*?\*/|//[^\n]*'
    masked = re.sub(tokens, lambda m: ' ' if m[0].startswith(('/*', '//')) else m[0], text, flags=re.S)
    spans = []
    for m in _COP2_FIND.finditer(masked):
        if m.group(0) not in ('__asm__', '__asm', 'asm'):
            continue
        found = _inline_construct(masked, m.start(), label)
        if found is None:
            continue
        end, lines, (out_section, in_section, clobber_section) = found[0], found[1], found[2][:3]
        outputs = _inline_operand_pairs(out_section, 'output', ('+r',) if leaf is True else ('=r', '=&r'), label)
        inputs = _inline_operand_pairs(in_section, 'input', ('r',), label)
        if leaf == 'vu-clip':
            clobbers = _inline_clobber_names(
                clobber_section, label,
                lambda name: name == 'memory' or _COP2_CLOBBER_GPR.fullmatch(name) or re.fullmatch(_FPR, name))
            check(lines, outputs, inputs, clobbers)
            _vu_clip_function_shape(masked, m.start(), end, outputs[0][1], [expr for _, expr in inputs])
        elif leaf:
            clobbers = _inline_clobber_names(
                clobber_section, label, lambda name: name == 'memory' or _COP2_CLOBBER_GPR.fullmatch(name))
            bound = check(lines, outputs, inputs, clobbers)
            parameters = _leaf_function_parameters(masked, m.start(), end)
            require(all(expr in parameters for expr in bound),
                    'every %s operand must be a parameter of its function' % label)
        else:
            clobbers = _inline_clobber_names(clobber_section, label, lambda name: re.fullmatch(_FPR, name))
            check(lines, outputs, inputs, clobbers)
        spans.append((m.start(), end))
    chars = list(masked)
    for start, end in spans:
        for i in range(start, end):
            if chars[i] != '\n':
                chars[i] = ' '
    remainder = ''.join(chars)
    (inner or ordinary_c)(remainder, staged, headers)
    _hardware_c_builtin_calls(remainder, label + ' C')
    return len(spans)


def asm_leaf_c(text, staged=None, headers=(), inner=None):
    """`ee-asm-leaf` (user decision 2026-10-10, docs/ee-hand-asm.md): a leaf
    function whose only statement is one admitted block-copy loop. Returns the
    number of admitted blocks; the audit reports the function `exact_c_with_asm`."""
    return _inline_class_c(text, staged, headers, inner, 'ee-asm-leaf', _leaf_block, True)


def asm_leaf_native_ti_c(text, staged=None, headers=()):
    """An `ee-asm-leaf` function inside a `native-ti` TU; neither route widened."""
    from tools import native_ti          # imports ordinary_c from this module
    return asm_leaf_c(text, staged, headers, inner=native_ti.hardware_source)


def fpu_c(text, staged=None, headers=(), inner=None):
    """`ee-fpu` (user decision 2026-10-10, docs/ee-hand-asm.md): readable C with
    inline COP1 transfer/convert blocks. Returns the number of admitted blocks;
    the audit reports a function with at least one `exact_c_with_asm`."""
    return _inline_class_c(text, staged, headers, inner, 'ee-fpu', _fpu_block, False)


def fpu_native_ti_c(text, staged=None, headers=()):
    """An `ee-fpu` function inside a `native-ti` TU; neither route widened."""
    from tools import native_ti          # imports ordinary_c from this module
    return fpu_c(text, staged, headers, inner=native_ti.hardware_source)


def vu_clip_c(text, staged=None, headers=(), inner=None):
    """`ee-vu-clip` (user decision 2026-10-11, docs/ee-hand-asm.md): a function
    whose whole body is one original hand-written VU0 lane-test block with its
    set/clear join. Returns the number of admitted blocks; the audit reports
    the function `exact_c_with_asm`."""
    return _inline_class_c(text, staged, headers, inner, 'ee-vu-clip', _vu_clip_block, 'vu-clip')


def vu_clip_native_ti_c(text, staged=None, headers=()):
    """An `ee-vu-clip` function inside a `native-ti` TU; neither route widened."""
    from tools import native_ti          # imports ordinary_c from this module
    return vu_clip_c(text, staged, headers, inner=native_ti.hardware_source)


def _sef_vu_clip_block(lines, outputs, inputs, clobbers):
    """Only sefClipViewVolumeA's six branch nops and one join nop (2026-10-18)."""
    filtered, branch_nops, join_nops = [], 0, 0
    for index, line in enumerate(lines):
        if line != 'nop':
            filtered.append(line)
            continue
        require(index > 0, 'a SEF clip nop must follow its branch or join jump')
        if _CLIP_BC1F.fullmatch(lines[index - 1]):
            branch_nops += 1
        elif re.fullmatch(r'j' + _WS + r'[1-9]f', lines[index - 1]):
            join_nops += 1
        else:
            require(False, 'a SEF clip nop is allowed only immediately after bc1f or the join jump')
    require(branch_nops == 6 and join_nops == 1,
            'sefClipViewVolumeA retains exactly six branch nops and one join nop')
    require(sum(bool(_CLIP_BC1F.fullmatch(line)) for line in lines) == 6,
            'sefClipViewVolumeA tests exactly six lanes')
    _vu_clip_block(filtered, outputs, inputs, clobbers)


def sef_vu_clip_c(text, staged=None, headers=(), inner=None):
    """Identity-scoped original SEF clip scheduling; all other clip checks unchanged."""
    return _inline_class_c(text, staged, headers, inner, 'ee-vu-clip', _sef_vu_clip_block, 'vu-clip')


def sef_vu_clip_native_ti_c(text, staged=None, headers=()):
    from tools import native_ti
    return sef_vu_clip_c(text, staged, headers, inner=native_ti.hardware_source)


# `ee-assert-break` (user decision 2026-10-11 on main/tu121: "Si break 0 es
# inline asm de una macro, crea esa macro y usala") -- the original assert
# macros end in a bare debugger trap. Evidence: UnduCheck prints the
# "Assert(%s)\n    file = %s  line = %d" banner with "../Undulate.euc.c" and
# line 1269, then `break 0`; GameCfPlayerLoadResource and
# Java_xeno_Chr_setShadow__aB carry the bare `break 0` after a failed check. cc1
# 2.96 refuses `__builtin_trap` ("not supported by this target") and its only
# own trap is the divide check `break 7`; the original code after each
# `break 0` falls through into the next block (reorg even duplicates the
# delay-slot instruction after it), which is what a volatile asm statement
# gives and a noreturn trap would not. The trap is spelled once, as the
# DEBUG_BREAK() macro of include/debug_break.h (admitted there by
# tools/tu_audit.py `debug_break_header_ok`); the function itself stays
# ordinary C that invokes `DEBUG_BREAK();` as a statement. tools/tu_audit.py
# additionally requires every `#APP` block of such a function to be exactly
# `break`.
DEBUG_BREAK_HEADER = 'debug_break.h'
_DEBUG_BREAK_USE = re.compile(r'\bDEBUG_BREAK\b')
_DEBUG_BREAK_STATEMENT = re.compile(r'\bDEBUG_BREAK\s*\(\s*\)\s*;')
_DEBUG_BREAK_EXPANSION = re.compile(r'\b__asm__\s+__volatile__\s*\(\s*"break"\s*\)')


def assert_break_c(text, staged=None, headers=(), inner=None):
    """`ee-assert-break` (see the comment above): ordinary (or native-TI) C
    whose only assembly is the DEBUG_BREAK() trap of include/debug_break.h.
    Returns the number of DEBUG_BREAK() statements; the audit reports the
    function `exact_c_with_asm`."""
    label = 'ee-assert-break'
    if staged is None:
        # Preprocessed text (a bounded search's candidate.i): the macro is already
        # expanded; fold its exact expansion back so the same grammar applies.
        text = _DEBUG_BREAK_EXPANSION.sub('DEBUG_BREAK()', text)
    (inner or ordinary_c)(text, staged, headers)
    masked = re.sub(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|/\*.*?\*/|//[^\n]*', ' ', text, flags=re.S)
    uses = len(_DEBUG_BREAK_USE.findall(masked))
    statements = len(_DEBUG_BREAK_STATEMENT.findall(masked))
    require(uses, 'an %s function invokes DEBUG_BREAK();' % label)
    require(statements == uses, 'DEBUG_BREAK is admitted in %s C only as the statement `DEBUG_BREAK();`' % label)
    require(not re.search(r'^\s*#\s*(?:define|undef)\s+DEBUG_BREAK\b', masked, re.M),
            'DEBUG_BREAK is defined only by include/%s' % DEBUG_BREAK_HEADER)
    return uses


def assert_break_native_ti_c(text, staged=None, headers=()):
    """An `ee-assert-break` function inside a `native-ti` TU; neither route widened."""
    from tools import native_ti          # imports ordinary_c from this module
    return assert_break_c(text, staged, headers, inner=native_ti.source)


# `ee-retaddr-store` (user decision 2026-10-27 on main/tu273, for UnduCheck at
# 0x003236f8 only; reports/cc1-study-20261027.md family B) -- the original
# stores its return address into the caller-visible frame slot with one inline
# asm statement at entry, `sw $31,0($sp)` at +0x28, and its out-of-range assert
# later prints that word. It occurs once in all of MAIN, and
# `__builtin_return_address` does not reproduce it. The class admits exactly one
# volatile asm statement performing that store, spelled
#     __asm__ __volatile__("sw $31,%0" : "=m"(LOCAL));   or
#     __asm__ __volatile__("sw $31,0($sp)");
# each with an optional "memory" clobber, inside otherwise ordinary (or
# native-TI) C. The same function's assert trap stays admitted only as the
# `ee-assert-break` statement `DEBUG_BREAK();` under that class's own rules.
# tools/tu_audit.py additionally requires exactly one #APP block of the function
# to be `sw $31,0($sp)` and every other one to be `break`, and admits the class
# for that one identity only. Not a precedent for any other function.
_RETADDR_STORE = re.compile(
    r'\b__asm__\s+__volatile__\s*\(\s*'
    r'(?:"sw \$31,%0"\s*:\s*"=m"\s*\(\s*[A-Za-z_]\w*\s*\)(?:\s*:\s*:\s*"memory")?'
    r'|"sw \$31,0\(\$sp\)"(?:\s*:\s*:\s*:\s*"memory")?)'
    r'\s*\)\s*;')


def retaddr_store_c(text, staged=None, headers=(), inner=None):
    """`ee-retaddr-store` (see the comment above): ordinary (or native-TI) C
    with exactly one return-address store statement, plus any `ee-assert-break`
    DEBUG_BREAK(); statements. Returns 1 (one admitted asm statement); the audit
    reports the function `exact_c_with_asm`."""
    label = 'ee-retaddr-store'
    if staged is None:
        # Preprocessed text: fold the DEBUG_BREAK() expansion back, as assert_break_c does.
        text = _DEBUG_BREAK_EXPANSION.sub('DEBUG_BREAK()', text)
    require(text.isascii() and '\x00' not in text and '??' not in text and '%:' not in text
            and _hardware_line_continuations_ok(text), '%s C encoding or preprocessing escape' % label)
    require(not re.search(r'^\s*#.*\b(?:asm|__asm|__asm__)\b', text, re.M),
            'inline assembly cannot be supplied by a preprocessor directive')
    tokens = r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|/\*.*?\*/|//[^\n]*'
    masked = re.sub(tokens, lambda m: ' ' if m[0].startswith(('/*', '//')) else m[0], text, flags=re.S)
    stores = list(_RETADDR_STORE.finditer(masked))
    require(len(stores) == 1, 'an %s function holds exactly one statement '
            '`__asm__ __volatile__("sw $31,%%0" : "=m"(LOCAL));` or '
            '`__asm__ __volatile__("sw $31,0($sp)");` (optional "memory" clobber)' % label)
    store = stores[0]
    remainder = (masked[:store.start()] + re.sub(r'[^\n]', ' ', store.group(0)) +
                 masked[store.end():])
    # Every other asm token, attribute or pragma is refused by the ordinary grammar below.
    bare = re.sub(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'', ' ', remainder)
    uses = len(_DEBUG_BREAK_USE.findall(bare))
    require(len(_DEBUG_BREAK_STATEMENT.findall(bare)) == uses,
            'DEBUG_BREAK is admitted in %s C only as the statement `DEBUG_BREAK();`' % label)
    require(not re.search(r'^\s*#\s*(?:define|undef)\s+DEBUG_BREAK\b', bare, re.M),
            'DEBUG_BREAK is defined only by include/%s' % DEBUG_BREAK_HEADER)
    (inner or ordinary_c)(remainder, staged, headers)
    return 1


def retaddr_store_native_ti_c(text, staged=None, headers=()):
    """An `ee-retaddr-store` function inside a `native-ti` TU; neither route widened."""
    from tools import native_ti          # imports ordinary_c from this module
    return retaddr_store_c(text, staged, headers, inner=native_ti.source)


# `ee-asm-sort` (user decision 2026-10-28 on main/tu106, for AlphaGroupSort at
# 0x00230d68, 0x70 bytes, only) -- the original is hand-written assembly: the
# trapping `addi` for its counters and pointer, `bne $0,$4` operand order, the
# `lui` scheduled into the `bnez` delay slot with the hazard and delay-slot
# `nop`s written out, and splat marks it "Handwritten function". It bubble-sorts
# the s_nAlphaGroup 16-byte NmlAlphaGroup records of s_aAlphaGroup by descending
# sortDistance (+4), swapping neighbours with one lq/sq pair, until a pass makes
# no swap. The class admits a function whose WHOLE body is one
# `__asm__ __volatile__` statement with exactly the template below (whitespace
# around operands aside), no outputs, the inputs `"m"(s_nAlphaGroup)` and
# `"i"(s_aAlphaGroup)`, and exactly the clobbers below, inside an otherwise
# ordinary (or native-TI) C function shell without parameters.
# tools/tu_audit.py additionally requires the function to emit exactly that one
# #APP block, and admits the class for that one identity only. Not a precedent.
ASM_SORT_TEMPLATE = (
    '.set noreorder',
    'lw $8,%0',
    'slti $2,$8,2',
    'bne $2,$0,3f',
    'lui $7,%%hi(%1)',
    'addiu $7,$7,%%lo(%1)',
    '1:',
    'move $5,$0',
    'move $4,$8',
    'addi $4,$4,-1',
    'move $6,$7',
    '2:',
    'lwc1 $f1,4($6)',
    'lwc1 $f0,20($6)',
    'c.le.s $f0,$f1',
    'nop',
    'bc1t 4f',
    'nop',
    'lq $2,0($6)',
    'lq $3,16($6)',
    'sq $3,0($6)',
    'sq $2,16($6)',
    'addi $5,$5,1',
    '4:',
    'addi $6,$6,16',
    'addi $4,$4,-1',
    'bne $0,$4,2b',
    'nop',
    'bne $0,$5,1b',
    'nop',
    '3:',
    '.set reorder',
)
ASM_SORT_INPUTS = [('m', 's_nAlphaGroup'), ('i', 's_aAlphaGroup')]
ASM_SORT_CLOBBERS = {'$2', '$3', '$4', '$5', '$6', '$7', '$8', '$f0', '$f1', 'memory'}


def asm_sort_c(text, staged=None, headers=(), inner=None):
    """`ee-asm-sort` (see the comment above): a parameterless C function whose
    whole body is the one admitted bubble-sort block. Returns 1 (one admitted
    asm statement); the audit reports the function `exact_c_with_asm`."""
    label = 'ee-asm-sort'
    require(text.isascii() and '\x00' not in text and '??' not in text and '%:' not in text
            and _hardware_line_continuations_ok(text), '%s C encoding or preprocessing escape' % label)
    require(not re.search(r'^\s*#.*\b(?:asm|__asm|__asm__)\b', text, re.M),
            'inline assembly cannot be supplied by a preprocessor directive')
    tokens = r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|/\*.*?\*/|//[^\n]*'
    masked = re.sub(tokens, lambda m: ' ' if m[0].startswith(('/*', '//')) else m[0], text, flags=re.S)
    spans = []
    for m in _COP2_FIND.finditer(masked):
        if m.group(0) not in ('__asm__', '__asm', 'asm'):
            continue
        found = _inline_construct(masked, m.start(), label)
        require(found is not None, 'an %s asm statement must be `__asm__ __volatile__(...)`' % label)
        spans.append((m.start(), found))
    require(len(spans) == 1, 'an %s function holds exactly one asm statement' % label)
    start, (end, lines, sections) = spans[0]
    out_section, in_section, clobber_section = sections[:3]
    require(not out_section.strip(), 'an %s block has no output operands' % label)
    inputs = _inline_operand_pairs(in_section, 'input', ('m', 'i'), label)
    require(inputs == ASM_SORT_INPUTS,
            'an %s block takes exactly the inputs "m"(s_nAlphaGroup), "i"(s_aAlphaGroup)' % label)
    clobbers = _inline_clobber_names(clobber_section, label, lambda name: name in ASM_SORT_CLOBBERS)
    require(clobbers == ASM_SORT_CLOBBERS,
            'an %s block clobbers exactly %s' % (label, ', '.join(sorted(ASM_SORT_CLOBBERS))))
    normalised = [re.sub(r'\s*,\s*', ',', re.sub(r'[ \t]+', ' ', line.strip())) for line in lines]
    require(normalised == list(ASM_SORT_TEMPLATE),
            'an %s block is exactly the AlphaGroupSort template of docs/ee-hand-asm.md' % label)
    parameters = _leaf_function_parameters(masked, start, end)
    require(parameters <= {'void'}, 'an %s function takes no parameters' % label)
    remainder = masked[:start] + re.sub(r'[^\n]', ' ', masked[start:end]) + masked[end:]
    # Every other asm token, attribute or pragma is refused by the ordinary grammar below.
    (inner or ordinary_c)(remainder, staged, headers)
    return 1


def asm_sort_native_ti_c(text, staged=None, headers=()):
    """An `ee-asm-sort` function inside a `native-ti` TU; neither route widened."""
    from tools import native_ti          # imports ordinary_c from this module
    return asm_sort_c(text, staged, headers, inner=native_ti.source)


def ee_assembly(text, functions):
    """Audit directives and code/data boundaries, not an EE mnemonic allowlist.

    GAS alone selects instruction encodings. No includes, macros, expressions
    emitting code, custom sections, source preprocessing or raw text data.
    """
    require(text.isascii() and all(ord(c) >= 32 or c in '\n\t\r' for c in text)
            and not any(token in text for token in ('\\', '/*', '*/')),
            'unsupported assembly encoding, escape or statement separator')
    names = {f['name'] for f in functions}
    seen, active, section, count = set(), None, None, 0
    symbol = r'[A-Za-z_][A-Za-z_0-9]*'
    data_sections = {'.rodata', '.data', '.sdata', '.lit4', '.lit8'}
    for number, raw in enumerate(text.splitlines(), 1):
        require(not re.match(r'\s*#\s*(?:include|define|if|pragma|line)\b', raw),
                'assembly preprocessing is not supported')
        line = raw.split('#', 1)[0].strip()
        require(';' not in line, 'assembly statement separators are not supported')
        if not line:
            continue
        error = 'unsupported assembly source at line ' + str(number) + ': ' + line
        if re.fullmatch(symbol + ':', line):
            require(section is not None, error)
            name = line[:-1]
            require(name not in names or active == name, 'function label outside its .ent')
            continue
        if line.startswith('.'):
            # Tabs are also valid GAS whitespace.
            parts = line.split(None, 1)
            directive, value = parts[0], parts[1].strip() if len(parts) == 2 else ''
            if directive in ('.text', '.data', '.sdata', '.rdata') or directive == '.section':
                new = ('.rodata' if directive == '.rdata' else directive) if directive != '.section' else value
                require(active is None and new in {'.text', *data_sections}
                        and (directive == '.section' or not value), error)
                section = new
            elif directive == '.set':
                require(value in ('noreorder', 'reorder', 'noat', 'at', 'nomacro', 'macro'), error)
            elif directive == '.align':
                require(section is not None and re.fullmatch(r'[0-4]', value), error)
            elif directive in ('.globl', '.global'):
                require(re.fullmatch(symbol, value), error)
            elif directive == '.ent':
                require(section == '.text' and active is None and value in names and value not in seen, error)
                active, count = value, 0
                seen.add(value)
            elif directive == '.end':
                require(active is not None and value == active and count > 0, error)
                active = None
            elif directive in ('.frame', '.mask', '.fmask'):
                require(active is not None and re.fullmatch(r'[A-Za-z0-9_$, \t-]+', value), error)
            elif directive == '.type':
                declaration = re.fullmatch('(' + symbol + r'),\s*@(function|object)', value)
                require(declaration and (
                    declaration[2] == 'function' and section == '.text' and declaration[1] in names
                    or declaration[2] == 'object' and section in data_sections and declaration[1] not in names), error)
            elif directive == '.size':
                # The legacy assembler derives FUNC extents from .ent/.end and
                # can ignore a conflicting .size. Do not accept a false extent.
                require(section in data_sections and value.split(',', 1)[0] not in names
                        and re.fullmatch(symbol + r',\s*(?:[0-9]+|\.\s*-\s*' + symbol + ')', value), error)
            elif directive in ('.word', '.half', '.byte', '.float', '.double'):
                require(section in data_sections and active is None
                        and re.fullmatch(r'[A-Za-z0-9_.,()+ \t-]+', value), error)
            else:
                raise ValueError(error)
        else:
            # GAS's COPZ operand is a raw 25-bit function encoding, not a
            # semantic instruction. Both spellings bypass the raw-data ban.
            require(not re.fullmatch(r'(?:c|cop)[0-3]', line.split()[0]),
                    'generic coprocessor opcode encoders are not assembly source')
            require(section == '.text' and active is not None
                    and re.fullmatch(r'[a-z][a-z0-9]*(?:\.[a-z0-9]+)*(?:\s+[A-Za-z0-9_$.,()+% \t-]+)?', line), error)
            count += 1
    require(active is None and seen == names, 'assembly must define every packet function with .ent/.end')


def difference(expected, actual, va):
    offset = next((i for i, pair in enumerate(zip(expected, actual)) if pair[0] != pair[1]),
                  min(len(expected), len(actual)) if expected != actual else None)
    return dict(equal=expected == actual, expected_size=len(expected), actual_size=len(actual),
                expected_sha256=cal.sha256(expected), actual_sha256=cal.sha256(actual), first_difference=None if offset is None else dict(
                    offset=offset, va=f'0x{address(va) + offset:08x}', instruction_offset=offset // 4 * 4,
                    expected=expected[offset:offset + 4].hex(), actual=actual[offset:offset + 4].hex()))


def compare(elf, packet, expected, original):
    """Pure byte/layout gate shared by real builds and synthetic ELF regressions."""
    bad = [s['name'] for s in elf.sections if s['size'] and s['type'] in (4, 9)]
    unresolved = [s['name'] for s in elf.symbols if s['name'] and s['section_index'] == 0]
    spans = [dict(packet['text'], section='.text'), *packet['generated_data']]
    require(len(spans) == len(expected), 'missing comparison reference spans')
    checks = []
    for span, data in zip(spans, expected):
        sections = [s for s in elf.sections if s['name'] == span['section']]
        section = sections[0] if len(sections) == 1 else None
        actual = (elf.section_bytes(section) or b'') if section else b''
        check = dict(section=span['section'], va=span['va'], **difference(data, actual, span['va']))
        check['layout_equal'] = bool(section and section['va'] == span['va'] and section['size'] == span['size']
                                     and section['flags'] & 2 and section['type'] == 1
                                     and bool(section['flags'] & 4) == (span['section'] == '.text'))
        check['verified'] = check['equal'] and check['layout_equal']
        if span.get('entry_width') == 4:
            check['expected_words'] = [f'0x{int.from_bytes(data[i:i+4], "little"):08x}' for i in range(0, len(data), 4)]
            check['actual_words'] = [f'0x{int.from_bytes(actual[i:i+4], "little"):08x}' for i in range(0, len(actual), 4)]
        checks.append(check)
    functions = []
    for function in packet['functions']:
        originals = [s for s in original.function_symbols if s['name'] == function['name']
                     and s['va'] == function['va'] and s['size'] == function['size']]
        require(len(originals) == 1, 'function lacks unique original binding witness')
        reference = originals[0]
        symbols = [s for s in elf.symbols if s['name'] == function['name']]
        symbol = symbols[0] if len(symbols) == 1 and symbols[0] in elf.function_symbols else None
        offset = address(function['va']) - address(packet['text']['va'])
        data = expected[0][offset:offset + function['size']]
        actual = elf.span(symbol['file_offset'], symbol['size'], 'linked function') if symbol else b''
        check = dict(function, **difference(data, actual, function['va']))
        check.update(expected_binding=reference['binding'], actual_binding=symbol['binding'] if symbol else None,
                     binding_equal=bool(symbol and symbol['binding'] == reference['binding']
                                        and symbol['other'] == reference['other']))
        check['verified'] = bool(symbol and symbol['va'] == function['va'] and symbol['size'] == function['size']
                                 and symbol['section'] == '.text' and check['equal'] and check['binding_equal'])
        functions.append(check)
    allowed = {s['section'] for s in spans}
    bindings = []
    for name, ref in packet['references'].items():
        symbols = [s for s in elf.symbols if s['name'] == name]
        bindings.append(dict(name=name, verified=len(symbols) == 1 and symbols[0]['va'] == ref['va']
                             and symbols[0]['section_index'] != 0))
        if 'witness' in ref:
            bindings[-1]['verified'] &= (len(symbols) == 1 and symbols[0]['section_index'] == 0xfff1
                                         and symbols[0]['binding'] == 1 and symbols[0]['other'] == 0)
        if ref.get('binding_section'):
            section_name = '.external_' + name
            allowed.add(section_name)
            sections = [s for s in elf.sections if s['name'] == section_name]
            bindings[-1]['verified'] &= (len(sections) == 1 and sections[0]['type'] == 8
                                          and sections[0]['va'] == ref['va'] and sections[0]['size'] == ref['size']
                                          and not sections[0]['flags'] & 4)
    # REGINFO is tool metadata, not candidate data. It is retained and reported, never discarded.
    extras = [s['name'] for s in elf.sections if s['size'] and s['flags'] & 6 and s['name'] not in allowed
              and not (s['name'] == '.reginfo' and s['type'] == 0x70000006 and s['size'] == 24 and not s['flags'] & 4)]
    extra_functions = [s['name'] for s in elf.function_symbols if s['name'] not in {f['name'] for f in packet['functions']}]
    entry_equal = elf.header['entry'] == packet['functions'][0]['va']
    structural = elf.header['type'] == 2 and entry_equal and not (bad or unresolved or extras or extra_functions) and all(b['verified'] for b in bindings)
    if '_gp' in packet['references']:
        structural &= elf.header['gp'] == packet['references']['_gp']['va']
    generated = all(c['verified'] for c in checks[1:]) and not extras
    code = structural and checks[0]['verified'] and all(f['verified'] for f in functions)
    return dict(code_bytes_verified=code, data_generated_checked=generated, unit_verified=False,
                 gate_passed=code and generated, spans=checks, functions=functions, bindings=bindings, entry_equal=entry_equal,
                pending_relocations=bad, unresolved=unresolved, extra_sections=extras, extra_functions=extra_functions,
                metadata=dict(header=elf.header, sections=elf.sections, program_headers=elf.program_headers))


def terminal_text_alignment(packet, profile, original, assembled=None):
    """Authorize only a witnessed original terminal gap, not a requested byte count.

    The contract covers every route whose candidate object comes out of the pinned
    assembler as one `.text` input: ordinary C, audited hardware C/ASM (`c-asm`)
    and standalone EE assembly (`ee-asm`, user authorization 2026-09-11). The
    checks below are the same for all three: the original gap is witnessed zero
    bytes, the input `.text` ends exactly at the last FUNC end (an `.align` or
    `.space` that pads the input is refused, never counted as the gap) and only
    the linker's `ALIGN(8)` supplies it.
    """
    if 'terminal_text_alignment' not in packet:
        return ''
    require(packet.get('source_language', 'c') in ('c', 'c-asm', 'ee-asm')
            and packet.get('source_language', 'c') in profile.get('source_languages', ['c'])
            and profile.get('terminal_text_alignment') == 'original-zero-gap-align8-v1',
            'terminal text alignment is not enabled for this profile and source language')
    contract = packet['terminal_text_alignment']
    require(isinstance(contract, dict) and set(contract) == {'alignment', 'gap', 'section', 'boundary'}
            and type(contract['alignment']) is int and contract['alignment'] == 8,
            'invalid terminal text alignment contract')
    text = packet['text']
    section, _ = extent(original, text)
    require(contract['section'] == {k: section[k] for k in ('index', 'name', 'va', 'size')}
            and section['type'] == 1 and section['flags'] & 4 and section['alignment'] >= 8,
            'terminal alignment section witness mismatch')
    last = packet['functions'][-1]
    begin = address(last['va']) + last['size']
    end = address(text['va']) + text['size']
    gap = contract['gap']
    require(isinstance(gap, dict) and set(gap) == {'va', 'size', 'sha256'}
            and gap['va'] == f'0x{begin:08x}' and type(gap['size']) is int
            and gap['size'] == end - begin and 0 < end - begin <= 7
            and end == (begin + 7) // 8 * 8,
            'terminal gap is not the exact nonempty ALIGN(8) interval')
    gap_section, data = extent(original, gap)
    require(gap_section == section and data is not None and not any(data)
            and cal.sha256(data) == gap['sha256'], 'terminal gap is not witnessed original zero bytes')
    symbols = [s for s in original.symbols if s['section_index'] == section['index']
               and s['name'] and s['type'] not in (3, 4)]
    require(not any(begin <= address(s['va']) < end
                    or address(s['va']) < begin < address(s['va']) + s['size'] for s in symbols),
            'terminal gap contains or overlaps an original symbol')
    boundary = contract['boundary']
    require(isinstance(boundary, dict), 'invalid terminal boundary witness')
    section_end = address(section['va']) + section['size']
    if end == section_end:
        require(boundary == dict(kind='section_end', va=f'0x{end:08x}'),
                'terminal section-end witness mismatch')
    else:
        following = sorted({s['name'] for s in symbols if address(s['va']) == end})
        require(following and boundary == dict(kind='next_symbol', va=f'0x{end:08x}', symbols=following),
                'terminal next-symbol witness mismatch')
    if assembled is not None:
        # Inventory deliberately accepts only linked ELFs. Read just the bounded
        # ELF32/LE MIPS ET_REL section table here; never rewrite its header.
        require(len(assembled) >= 52, 'truncated terminal alignment input ELF')
        header = struct.unpack_from('<16sHHIIIIIHHHHHH', assembled)
        require(header[0][:7] == b'\x7fELF\x01\x01\x01' and header[1:4] == (1, 8, 1)
                and header[11] == 40 and 0 < header[13] < header[12]
                and 52 <= header[6] <= len(assembled) - 40 * header[12],
                'unsupported terminal alignment input ELF section table')
        table = [struct.unpack_from('<10I', assembled, header[6] + i * 40) for i in range(header[12])]
        strings = table[header[13]]
        require(strings[1] == 3 and strings[4] + strings[5] <= len(assembled),
                'invalid terminal alignment input section names')
        names = assembled[strings[4]:strings[4] + strings[5]]
        inputs = [s for s in table if s[5] and s[2] & 4]
        require(len(inputs) == 1 and inputs[0][0] < len(names)
                and names[inputs[0][0]:].split(b'\x00', 1)[0] == b'.text'
                and inputs[0][1] == 1 and inputs[0][4] + inputs[0][5] <= len(assembled)
                and inputs[0][5] == begin - address(text['va']),
                'terminal alignment requires one exact unpadded input .text extent')
    # The assertion measures linker input BEFORE alignment; ALIGN cannot conceal
    # a shortened body or extra input padding. No fill value or bytes are supplied.
    return f' ASSERT(. == 0x{begin:08x}, "terminal pre-alignment extent"); . = ALIGN(8);'


def match(root, packet_path, output, profile_path=PROFILE):
    root, packet_path, profile_path = root.resolve(), packet_path.resolve(), profile_path.resolve()
    profile, packet = [json.loads(p.read_bytes()) for p in (profile_path, packet_path)]
    require(profile['schema_version'] == packet['schema_version'] == 1, 'unsupported schema')
    require(set(packet) - {'flags', 'source_language', 'source_scope', 'asm_contract', 'terminal_text_alignment'} == {'schema_version', 'revision', 'unit', 'binaries', 'source', 'headers',
                                        'text', 'functions', 'references', 'generated_data'}, 'packet fields differ from schema v1')
    language = packet.get('source_language', 'c')
    scope = packet.get('source_scope')
    require(scope is None or scope == RUNTIME_VERBATIM, 'unknown source scope')
    require(scope is None or language in ('c', 'c-asm'),
            'the runtime-verbatim scope carries only C and upstream-verbatim C/asm')
    require(language != 'c-asm' or scope == RUNTIME_VERBATIM
            or 'upstream_asm' not in packet.get('asm_contract', {}),
            'upstream assembly citations exist only in the runtime-verbatim scope')
    kinds = {'c': 'c', 'c-asm': 'mixed', 'ee-asm': 'asm'}
    require(language in kinds and language in profile.get('source_languages', ['c']),
            'source language is not enabled by this profile')
    require(profile.get('source_languages', ['c']) in (['c'], ['c', 'c-asm', 'ee-asm']),
            'unsupported profile source language contract')
    require(language != 'c' or 'asm_contract' not in packet, 'ordinary C cannot carry an assembly contract')
    lock_path = root / relative(profile['compiler_lock'])
    require(cal.digest(lock_path) == profile['compiler_lock_sha256'], 'compiler lock changed')
    lock = json.loads(lock_path.read_bytes())
    # Public tool specifications use project-relative installation paths.
    for item in lock['variants']:
        command = item['command']
        command[0] = str(root / command[0])
        command[1:] = ['-B' + str(root / arg[2:]) + '/' if arg.startswith('-B') else arg
                       for arg in command[1:]]
    lock['tools'] = {name: str(root / path) for name, path in lock['tools'].items()}
    for pin in lock['pins']:
        pin['path'] = str(root / pin['path'])
    # Host libraries are relative to the host filesystem, not the project.
    for pin in lock.get('host_libraries', []):
        path = relative(pin['path'])
        require(path.parts[0] in ('lib', 'lib64'), 'unsupported host library directory')
        lock['pins'].append(dict(path=str(Path('/') / path), sha256=pin['sha256']))
    require(profile['revision'] == packet['revision'] == lock['revision'] == cal.REVISION, 'revision mismatch')
    require(lock['profile'] not in RETIRED_PROFILES and profile['selected'] not in RETIRED_PROFILES,
            f"retired compiler profile {lock['profile']}: {RETIRED_REASON}")
    require(profile['selected'] == lock['profile'] and lock['profile'] in ('ee-gcc2_96', 'ee-gcc2_9-990721', NOSIBCALL),
            'unapproved compiler')
    legacy = lock['profile'] == 'ee-gcc2_9-990721'
    nosibcall = lock['profile'] == NOSIBCALL
    variant = next(v for v in lock['variants'] if v['id'] == profile['selected'])
    flags = packet.get('flags', profile['default_flags'])
    permitted = ([['-O2', '-G0']] if legacy else NOSIBCALL_FLAGS if nosibcall
                 else [['-O2', '-G0'], ['-O2', '-G8']])
    require(profile['default_flags'] == variant['flags'] == permitted[0]
            and profile['permitted_flags'] == permitted
            and flags in profile['permitted_flags'] and profile['timeout_seconds'] == 30, 'unapproved flags/budget')
    compiler_dir = Path(variant['command'][0]).parent
    if legacy:
        prefix = root / '.work/toolchains/compiler-variants/ee-gcc2.9-990721'
        require(profile.get('source_languages') == ['c'] and variant['command'] == [
            str(prefix / 'ee-gcc'), '-B' + str(prefix) + '/', '-nostdinc', '-fno-builtin', '-S'],
            'unapproved GCC 2.9 source/command contract')
    if nosibcall:
        # Same compiler binary as ee-gcc2_96: the unchanged payload directory and
        # command, C only, with the three payload members pinned to 2.96 bytes.
        prefix = root / GCC296_PAYLOAD
        require(profile.get('source_languages') == ['c'] and variant['command'] == [
            str(prefix / 'ee-gcc'), '-B' + str(prefix) + '/', '-nostdinc', '-fno-builtin', '-S']
            and all(p['sha256'] == GCC296_PAYLOAD_SHA256[Path(p['path']).name] for p in lock['pins']
                    if Path(p['path']).parent == prefix)
            and {Path(p['path']).name for p in lock['pins'] if Path(p['path']).parent == prefix}
            == set(GCC296_PAYLOAD_SHA256),
            'unapproved nosibcall source/command/payload contract')
    require({p.name for p in compiler_dir.iterdir()} == {'cc1', 'cpp' if legacy else 'cpp0', 'ee-gcc'},
            'unexpected compiler support payload')
    pins = {p['path']: p['sha256'] for p in lock['pins']}
    required_tools = [*compiler_dir.iterdir(), *map(Path, lock['tools'].values())]
    required_tools += [Path(p) for p in pins if p.startswith(('/lib/', '/lib64/'))]
    inputs = {str(p): pins[str(p)] for p in required_tools}
    inputs.update({str(p): cal.digest(p) for p in (packet_path, profile_path, lock_path, Path(__file__), Path(cal.__file__))})
    inputs[str(Path(cal.__file__))] = pins[str(Path(cal.__file__))]
    target_path = root / 'config/targets.json'
    require(cal.digest(target_path) == cal.TARGET_MANIFEST_SHA256, 'target manifest changed')
    inputs[str(target_path)] = cal.TARGET_MANIFEST_SHA256
    inputs[str(BASE / 'tools/inventory.py')] = pins[str(BASE / 'tools/inventory.py')]
    qualified = any('witness' in ref for ref in packet['references'].values())
    contract = profile.get('external_reference_contract')
    require(not qualified or contract is not None, 'qualified external references require a pinned profile contract')
    if contract is not None:
        helper = Path(external_reference.__file__).resolve()
        require(contract == dict(schema_version=1, path='tools/external_reference.py', sha256=cal.digest(helper))
                and helper == (root / 'tools/external_reference.py').resolve(), 'external reference helper pin mismatch')
        inputs[str(helper)] = contract['sha256']
    if language != 'c':
        contract = packet.get('asm_contract', {})
        fields = {'purpose', 'inputs', 'outputs', 'clobbers', 'memory', 'state', 'privilege', 'control_flow'}
        # Inside the runtime-verbatim scope the upstream citation list is part of
        # the contract, not an extra: an asm packet without it is refused here.
        cited = {'upstream_asm'} if scope == RUNTIME_VERBATIM else set()
        require(set(contract) == fields | {'evidence'} | cited and all(
            isinstance(contract[k], str) and contract[k].strip() for k in fields), 'missing assembly semantic/state contract')
        evidence = contract['evidence']
        require(set(evidence) == {'path', 'sha256'}, 'invalid assembly semantic evidence pin')
        declared = contract.get('upstream_asm')
        require(scope != RUNTIME_VERBATIM or (isinstance(declared, list) and declared and all(
            isinstance(c, dict) and set(c) == {'file', 'line', 'text_sha256'}
            and type(c['line']) is int and c['line'] > 0
            and re.fullmatch(r'[0-9a-f]{64}', str(c['text_sha256']))
            for c in declared)), 'invalid upstream assembly citation list')
        path = root / relative(evidence['path'])
        require(path.is_file() and path.resolve().is_relative_to(root) and cal.digest(path) == evidence['sha256'],
                'assembly semantic evidence changed or missing')
        inputs[str(path)] = evidence['sha256']
    provenance, upstream_texts = None, {}
    if scope == RUNTIME_VERBATIM:
        name, record, provenance_path = runtime_verbatim_provenance(root, packet)
        upstream_texts = runtime_verbatim_upstream(root, record)
        require(language == 'c' or upstream_texts,
                'the runtime-verbatim assembly route needs a recorded upstream file')
        require(language == 'c-asm' or not upstream_texts,
                'upstream assembly records belong to a c-asm runtime-verbatim packet')
        provenance = dict(unit=name, **{k: record[k] for k in
                                        ('project', 'revision', 'upstream_path', 'upstream_sha256', 'license')},
                          selectors=list(record.get('selectors', [])),
                          # Only the assembly route records upstream files, and a
                          # scope unit without them keeps exactly its old result.json.
                          **(dict(upstream={k: {f: v[f] for f in ('local_path', 'sha256')}
                                            for k, v in upstream_texts.items()})
                             if upstream_texts else {}),
                          provenance=dict(path='config/runtime-provenance.json',
                                          sha256=cal.digest(provenance_path)))
        inputs[str(provenance_path)] = provenance['provenance']['sha256']
        for entry in upstream_texts.values():
            inputs[str(root / relative(entry['local_path']))] = entry['sha256']
    target = json.loads(target_path.read_bytes())
    embedded = {p['section_index'] for a in target['section_aliases'] for p in a['placements'] if p['unit'] == 'main'}
    unit = packet['unit']
    require(unit in target['units'], 'unknown unit')
    elfs = {}
    for name, binary in packet['binaries'].items():
        require(name in target['units'] and (name in (unit, 'main') or any(
            ref['unit'] == name and 'witness' in ref for ref in packet['references'].values())), 'overlay rebind')
        pin = target['units'][name]
        require(binary == dict(path='game/extracted-iso/' + pin['path'], sha256=pin['sha256']), 'binary/unit identity mismatch')
        path = root / relative(binary['path'])
        require(cal.digest(path) == binary['sha256'], 'binary changed')
        inputs[str(path)] = binary['sha256']
        elfs[name] = cal.ELF(path.read_bytes(), name)
    original = elfs[unit]
    start, end = address(packet['text']['va']), address(packet['text']['va']) + packet['text']['size']
    expected, witnesses = [], {}
    names, cursor = set(), start
    for f in packet['functions']:
        ident(f['name'])
        require(f['name'] not in names and address(f['va']) >= cursor and type(f['size']) is int and f['size'] > 0,
                'functions must be unique, ordered and nonoverlapping within full text')
        names.add(f['name'])
        symbols = [s for s in original.function_symbols if s['name'] == f['name'] and s['va'] == f['va']]
        require(len(symbols) == 1 and symbols[0]['size'] == f['size'], 'function extent/name differs from ELF')
        cursor = address(f['va']) + f['size']
    require(names and cursor <= end, 'full text does not contain complete function extents')
    in_span = {s['name'] for s in original.function_symbols if start <= address(s['va']) < end
               or address(s['va']) < start < address(s['va']) + s['size']}
    require(in_span == names, 'original functions/aliases omitted from packet')
    sections = set()
    ranges = [(start, end)]
    for span in [packet['text'], *packet['generated_data']]:
        if span is not packet['text']:
            require(span['section'] in ('.rodata', '.data', '.sdata', '.lit4', '.lit8') and span['section'] not in sections,
                    'unsupported or duplicate generated section')
            sections.add(span['section'])
            lo, hi = address(span['va']), address(span['va']) + span['size']
            require(not any(lo < b and a < hi for a, b in ranges), 'overlapping code/generated data')
            ranges.append((lo, hi))
            require(span.get('entry_width') in (None, 4) and (span.get('entry_width') != 4 or span['size'] % 4 == 0), 'invalid table width')
        section, data = extent(original, span)
        require(span is not packet['text'] or section['flags'] & 4, 'original text span is not executable')
        require(unit != 'main' or section['index'] not in embedded, 'embedded overlay requires canonical unit identity')
        require(data is not None and cal.sha256(data) == span['sha256'], 'original span hash mismatch')
        expected.append(data)
    text_alignment = terminal_text_alignment(packet, profile, original)
    for name, ref in packet['references'].items():
        ident(name)
        require(name not in names and (ref['unit'] in (unit, 'main') or 'witness' in ref),
                'overlay rebind or function/reference collision')
        elf = elfs[ref['unit']]
        section, data = extent(elf, ref)
        require(ref['unit'] != 'main' or section['index'] not in embedded, 'embedded overlay reference requires canonical unit identity')
        symbols = sorted({s['name'] for s in elf.symbols if s['name'] and s['va'] == ref['va'] and s['section_index'] != 0 and s['type'] != 3})
        if 'witness' in ref:
            external_reference.validate(name, ref, elf, section, data, packet['binaries'][ref['unit']])
        require(ref['symbols'] == symbols and (data is not None or symbols or 'witness' in ref), 'external symbol witness mismatch')
        require(ref['sha256'] == (cal.sha256(data) if data is not None else None), 'external byte witness mismatch')
        require(ref.get('binding_section') in (None, '.sdata'), 'unsupported external binding section')
        require(not any(address(ref['va']) < b and a < address(ref['va']) + ref['size'] for a, b in ranges), 'external reference overlaps generated scope')
        if name == '_gp':
            require(elf.header['gp'] == ref['va'], 'GP lacks original symbol/REGINFO evidence')
        witnesses[name] = dict(ref, binary_sha256=cal.sha256(elf.data), section=section['name'],
                               file_backed=data is not None, hex=data.hex() if data is not None else None)
    require(flags[1] != '-G8' or '_gp' in packet['references'], 'G8 requires explicit original GP witness')
    runtime_asm = scope == RUNTIME_VERBATIM and language == 'c-asm'
    candidate_c = (runtime_verbatim_asm_c if runtime_asm
                   else hardware_c if language == 'c-asm'
                   else runtime_verbatim_c if scope == RUNTIME_VERBATIM else ordinary_c)
    files = [packet['source'], *packet['headers']]
    staged = [relative(f['staged_path']) for f in files]
    suffix = '.s' if language == 'ee-asm' else '.c'
    # `.def` is a header for admission purposes in this scope only: GCC's
    # machmode.h includes machmode.def, and staging it is what keeps both files
    # byte-identical to upstream. It is pinned by sha256 like every other header.
    header_suffixes = ('.h', '.def') if scope == RUNTIME_VERBATIM else ('.h',)
    require(staged[0].suffix == suffix and all(p.suffix in header_suffixes for p in staged[1:])
            and len(set(staged)) == len(staged), 'invalid staged source/header paths')
    require(language != 'ee-asm' or not packet['headers'], 'standalone assembly has no include interface')
    sources, citations, templates = [], [], set()
    for f, stage in zip(files, staged):
        path = root / relative(f['path'])
        require(path.is_file() and path.resolve().is_relative_to(root) and cal.digest(path) == f['sha256'], 'source/header hash or path mismatch')
        text = path.read_text(encoding='ascii')
        if language == 'ee-asm':
            require(path.suffix == '.s', 'standalone assembly requires an unpreprocessed .s source')
            ee_assembly(text, packet['functions'])
        elif runtime_asm:
            statements = candidate_c(text, stage, {p.as_posix() for p in staged[1:]})
            citations += runtime_verbatim_asm_evidence(relative(f['path']).as_posix(),
                                                       statements, upstream_texts)
            templates |= {t for t in map(runtime_verbatim_asm_template, statements)
                          if t is not None}
        else:
            candidate_c(text, stage, {p.as_posix() for p in staged[1:]})
        sources.append(path)
        inputs[str(path)] = f['sha256']
    if runtime_asm:
        # The contract cites the upstream file and lines; it must be exactly what
        # the staged files carry, so it can neither omit nor invent a statement.
        require(citations, 'a runtime-verbatim c-asm packet must contain assembly')
        derived = sorted({(c['file'], c['line'], c['text_sha256']) for c in citations})
        require(sorted({(c['file'], c['line'], c['text_sha256'])
                        for c in packet['asm_contract']['upstream_asm']}) == derived,
                'asm_contract.upstream_asm is not the derived upstream citation list')
        provenance['upstream_asm'] = [dict(file=f, line=l, text_sha256=s) for f, l, s in derived]
    require(not os.path.lexists(output) and output.absolute() == output.resolve(), 'output collision or symlink')
    for path, digest in inputs.items():
        require(cal.digest(path) == digest, 'input/tool payload changed: ' + path)
    output = cal.prepare_output(root, output, [root / '.work/toolchains', *map(Path, inputs)])
    env = dict(PATH='/usr/bin:/bin', LC_ALL='C', HOME=str(output), TMPDIR=str(output))
    result = dict(schema_version=1, revision=packet['revision'], unit=unit, binary=packet['binaries'][unit],
                  source_language=language, recovery_kind=kinds[language], semantic_acceptance=False,
                  **(dict(source_scope=scope, runtime_verbatim=provenance) if scope else {}),
                  packet_sha256=cal.digest(packet_path), profile=profile, input_hashes=inputs.copy(),
                  flags=flags, environment=env, commands=[], witnesses=witnesses, code_bytes_verified=False,
                  data_generated_checked=False, external_data_witnessed=True, unit_verified=False, gate_passed=False)
    if language != 'c':
        result['asm_contract'] = packet['asm_contract']
    if text_alignment:
        result['terminal_text_alignment'] = packet['terminal_text_alignment']
    result.update(generated_data_bytes=sum(s['size'] for s in packet['generated_data']),
                  external_witness_bytes=sum(r['size'] for r in witnesses.values() if r['file_backed']),
                  external_data_compiled=False, original_compiler_distribution_proven=False,
                  original_function_symbols=[s for s in original.function_symbols if s['name'] in names])
    for path, stage in zip(sources, staged):
        dest = output / 'staged' / stage
        dest.parent.mkdir(parents=True, exist_ok=True)
        dest.write_bytes(path.read_bytes())
        require(cal.digest(dest) == inputs[str(path)], 'source changed during staging')
        inputs[str(dest)] = cal.digest(dest)
    (output / 'packet.json').write_bytes(packet_path.read_bytes())
    # A numeric linker entry does not require exporting a translation-unit-local function.
    script = 'SECTIONS {\n.text ' + packet['text']['va'] + ' : { *(.text .text.*)' + text_alignment + ' }\n'
    for span in packet['generated_data']:
        script += f"{span['section']} {span['va']} : {{ *({span['section']} {span['section']}.*) }}\n"
    for name, ref in packet['references'].items():
        script += (f".external_{name} {ref['va']} (NOLOAD) : {{ {name} = .; . += {ref['size']}; }}\n" if ref.get('binding_section') else f"{name} = {ref['va']};\n")
    script += '}\n'
    for f in packet['functions']:
        symbol = next(s for s in original.function_symbols if s['name'] == f['name'] and s['va'] == f['va'])
        # ld script expressions cannot resolve STB_LOCAL; compare() checks every VA and binding.
        if symbol['binding'] != 0:
            script += f"ASSERT({f['name']} == {f['va']}, \"function VA: {f['name']}\")\n"
    for name, ref in packet['references'].items():
        script += f'ASSERT({name} == {ref["va"]}, "reference VA: {name}")\n'
    for span in [dict(packet['text'], section='.text'), *packet['generated_data']]:
        script += f'ASSERT(SIZEOF({span["section"]}) == {span["size"]}, "full section size: {span["section"]}")\n'
    (output / 'layout.ld').write_text(script, encoding='ascii')
    inputs[str(output / 'layout.ld')] = cal.digest(output / 'layout.ld')

    def run(argv, label):
        for path, digest in inputs.items():
            require(cal.digest(path) == digest, 'input changed before subprocess: ' + path)
        log = output / (label + '.log')
        with log.open('xb') as stream:
            proc = subprocess.Popen(argv, cwd=output, env=env, stdout=stream, stderr=subprocess.STDOUT, start_new_session=True)
            try:
                rc = proc.wait(timeout=profile['timeout_seconds'])
            except subprocess.TimeoutExpired:
                os.killpg(proc.pid, signal.SIGKILL)
                proc.wait()
                rc = 124
        result['commands'].append(dict(argv=argv, cwd=str(output), returncode=rc, log=log.name, log_sha256=cal.digest(log)))
        return rc

    try:
        if language == 'ee-asm':
            (output / 'candidate.s').write_bytes((output / 'staged' / staged[0]).read_bytes())
            ee_assembly((output / 'candidate.s').read_text(encoding='ascii'), packet['functions'])
        else:
            cmd = variant['command']
            includes = ['-Istaged/include', '-Istaged']
            require(run([*cmd[:-1], '-E', *flags, *includes, 'staged/' + staged[0].as_posix(), '-o', 'candidate.i'], 'preprocess') == 0, 'preprocess failed')
            preprocessed = (output / 'candidate.i').read_text(encoding='ascii')
            if runtime_asm:
                # Macro expansion rewrites operands but never the assembly text,
                # so every template here must be one the staged files declared.
                for statement in candidate_c(preprocessed):
                    template = runtime_verbatim_asm_template(statement)
                    require(template is not None and template in templates,
                            'preprocessed assembly template is not one of the staged upstream templates')
            elif language == 'c-asm':
                require(hardware_c(preprocessed) > 0, 'mixed source must contain a calibrated hardware statement')
            else:
                candidate_c(preprocessed)
            inputs[str(output / 'candidate.i')] = cal.digest(output / 'candidate.i')
            require(run([*cmd, *flags, 'candidate.i', '-o', 'candidate.s'], 'compile') == 0, 'compile failed')
        inputs[str(output / 'candidate.s')] = cal.digest(output / 'candidate.s')
        tools = lock['tools']
        # Single EE assembler for game code (user 2026-10-27): the lock's swap-mask GNU as
        # with -g, as every in-scope game C TU contract assembles (no delay-slot swapping).
        require(run([tools['assembler'], '-EL', '-m5900', '-mabi=eabi', flags[1], '-g', '-o', 'candidate.o', 'candidate.s'], 'assemble') == 0, 'assemble failed')
        inputs[str(output / 'candidate.o')] = cal.digest(output / 'candidate.o')
        if text_alignment:
            # --noinhibit-exec may return zero for a failed ld assertion. Check
            # the actual input independently; retain the assertion as a witness.
            terminal_text_alignment(packet, profile, original,
                                    (output / 'candidate.o').read_bytes())
        # Retain a diagnostic ELF after assertion failure, but a nonzero link can never pass.
        link_rc = run([tools['linker'], '-m', 'elf32lr5900', '--no-undefined', '--noinhibit-exec',
                       '-e', packet['functions'][0]['va'], '-T', 'layout.ld', '-o', 'candidate.elf', 'candidate.o'], 'link')
        elf = cal.ELF((output / 'candidate.elf').read_bytes(), unit)
        inputs[str(output / 'candidate.elf')] = cal.digest(output / 'candidate.elf')
        result.update(compare(elf, packet, expected, original))
        result['gate_passed'] &= link_rc == 0
        result['code_bytes_verified'] &= link_rc == 0
        original_section, _ = extent(original, packet['text'])
        for label, binary, section in [('original', str(root / packet['binaries'][unit]['path']), original_section['name']), ('candidate', 'candidate.elf', '.text')]:
            stop = end if label == 'original' else start + next((s['size'] for s in elf.sections if s['name'] == '.text'), 0)
            require(run([tools['objdump'], '-Dz', '-M', 'no-aliases', '-j', section, '--start-address=' + packet['text']['va'],
                         '--stop-address=' + hex(stop), binary], label + '.asm') == 0, 'disassembly failed')
        diff = difflib.unified_diff((output / 'original.asm.log').read_text().splitlines(True),
                                   (output / 'candidate.asm.log').read_text().splitlines(True), fromfile='original', tofile='candidate')
        (output / 'assembly.diff').write_text(''.join(diff))
        (output / 'bytes.diff').write_text(json.dumps(result['spans'], indent=2) + '\n')
        result['original_metadata'] = dict(header=original.header, scope_section=original_section)
        candidate_section = next(s for s in elf.sections if s['name'] == '.text')
        result['metadata_differences'] = {
            category: {key: dict(original=before[key], candidate=after[key]) for key in keys if before[key] != after[key]}
            for category, before, after, keys in [
                ('header', original.header, elf.header, ('type', 'machine', 'flags', 'entry', 'gp', 'section_count', 'program_header_count')),
                ('containing_code_section', original_section, candidate_section, ('name', 'flags', 'va', 'file_offset', 'size', 'alignment'))]}
    except (OSError, ValueError) as exc:
        result.update(error=str(exc), gate_passed=False, code_bytes_verified=False)
    result['artifacts'] = {str(p.relative_to(output)): cal.digest(p) for p in sorted(output.rglob('*')) if p.is_file()}
    (output / 'result.json').write_text(json.dumps(result, indent=2, sort_keys=True) + '\n')
    return result


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root', type=Path, required=True)
    parser.add_argument('--packet', type=Path, required=True)
    parser.add_argument('--output-dir', type=Path, required=True)
    parser.add_argument('--profile', type=Path, default=PROFILE)
    args = parser.parse_args(argv)
    try:
        result = match(args.root, args.packet, args.output_dir, args.profile)
        print(json.dumps({k: result[k] for k in ('gate_passed', 'code_bytes_verified', 'data_generated_checked', 'external_data_witnessed', 'unit_verified', 'source_language', 'recovery_kind')}))
        return 0 if result['gate_passed'] else 1
    except (OSError, ValueError, KeyError, TypeError, StopIteration) as exc:
        print(json.dumps(dict(gate_passed=False, error=str(exc))))
        return 1


if __name__ == '__main__':
    sys.exit(main())
