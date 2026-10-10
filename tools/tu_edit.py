#!/usr/bin/env python3
"""Function-level editing of TU-mode C files (INCLUDE_ASM / ACCEPTED_ASM / C).

A TU file in the migration layout is a prelude (comments, preprocessor lines,
declarations) followed by one block per original function, in original
address order. Each block is one of

  asm           INCLUDE_ASM("asm/.../nonmatchings/<unit>/<tu>", name);  scaffold
  accepted_asm  ACCEPTED_ASM("src/<unit>/<tu>[/...]", name);            exact_asm
  c             a C function definition                                  claim

Free items between blocks (static data, inline helpers, #defines) belong to the
"pre" region of the block that follows them; whatever follows the last block is
the epilogue. A comment that starts its own line directly above a block (no
blank line in between) and comments on the block's last line belong to the
block. Parsing uses a small tokenizer that tracks comments, string/char
literals, preprocessor lines (with continuations), parentheses and brace depth;
it is not a C parser. Emission is the original text, so parse -> emit is
byte-identical by construction and every edit is a splice of the source text.

CLI (see docs/tu-tools.md):
  list TU [--skeleton S] [--unit-dir D] [--json]
  roundtrip TU [TU ...] [--json]
  extract TU NAME                         print one block (or --from-source FILE)
  to-c TU NAME (--body FILE | --from-source FILE) [--as NAME] [--replace-accepted-asm]
  to-asm TU NAME [--skeleton S | --asm-dir DIR] [--allow-accepted]
  subset TU --keep F[,F...] [--skeleton S | --asm-dir DIR]
  split-asm ASM.s --keep F[,F...] -o OUT.s [--manifest M.json]   re-cut an accepted group
  split-asm ASM.s --check M.json [--result OUT.s]                re-derive and compare
  merge --base B --ours O --theirs T [--skeleton S] [--allowed F,...] [--interstitial-allowed F,...]
        [--comments strict|ours]
  make-patch --base B --theirs T [--skeleton S]
  apply-patch --base B --target O --patch P.json [--skeleton S] [--allowed F,...]
Edits write to stdout, to -o OUT or back with --in-place (never on conflict).
Exit status: 0 ok, 1 conflicts/refusal, 2 usage or parse error.
"""
import argparse
import ast
import difflib
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile

sys.dont_write_bytecode = True

SCHEMA_PATCH = 'tu-patch/1'
SCHEMA_MERGE = 'tu-merge/1'
STATES = ('asm', 'accepted_asm', 'c')
MACRO_STATE = {'INCLUDE_ASM': 'asm', 'ACCEPTED_ASM': 'accepted_asm'}
_MACRO_ITEM = re.compile(r'(INCLUDE_ASM|ACCEPTED_ASM|INCLUDE_RODATA)\s*\(\s*"([^"\\\n]*)"\s*,'
                         r'\s*([A-Za-z_][A-Za-z_0-9]*)\s*\)\s*;')
_MACRO_WORD = re.compile(r'\b(?:INCLUDE_ASM|ACCEPTED_ASM|INCLUDE_RODATA)\b')
_IDENT = re.compile(r'[A-Za-z_][A-Za-z_0-9]*')
# Identifiers that can precede "(" in a declaration head without being the
# declarator name.
_NOT_NAMES = frozenset('''__attribute__ __attribute __asm__ __asm asm sizeof __typeof__ __typeof
typeof __extension__ __alignof__ if while for switch return'''.split())
_TYPE_WORDS = frozenset('''void char short int long float double signed unsigned __signed__
const volatile static extern inline __inline__ __inline register auto struct union enum typedef
__const __volatile__'''.split())


class ParseError(ValueError):
    """The text is not a TU file this tool can split safely."""


class EditError(ValueError):
    """A requested edit is refused (unknown function, wrong state, ...)."""


def sha256_text(text):
    return hashlib.sha256(text.encode('utf-8', 'surrogateescape')).hexdigest()


def line_of(text, offset):
    return text.count('\n', 0, offset) + 1


# ---------------------------------------------------------------------------
# Tokenizer
# ---------------------------------------------------------------------------

class Item:
    """One top-level construct: pp, comment, decl, func, asm, accepted_asm, rodata."""
    __slots__ = ('kind', 'start', 'end', 'name', 'asm_dir', 'macro', 'conditional')

    def __init__(self, kind, start, end, name=None, asm_dir=None, macro=None, conditional=False):
        self.kind, self.start, self.end = kind, start, end
        self.name, self.asm_dir, self.macro = name, asm_dir, macro
        self.conditional = conditional

    def text(self, source):
        return source[self.start:self.end]

    def __repr__(self):
        return f'Item({self.kind},{self.start},{self.end},{self.name})'


def _where(text, offset):
    return f'line {line_of(text, offset)}'


def _skip_literal(text, i, lenient=False):
    """Return the offset after the string/char literal opening at text[i]."""
    quote, j, n = text[i], i + 1, len(text)
    while j < n:
        c = text[j]
        if c == '\\':
            j += 2
            continue
        if c == quote:
            return j + 1
        if c == '\n':
            if lenient:
                return i + 1
            raise ParseError(f'unterminated literal at {_where(text, i)}')
        j += 1
    if lenient:
        return i + 1
    raise ParseError(f'unterminated literal at {_where(text, i)}')


def _line_comment_end(text, i):
    n = len(text)
    while True:
        e = text.find('\n', i)
        if e < 0:
            return n
        if text[e - 1] == '\\' or (text[e - 1] == '\r' and e >= 2 and text[e - 2] == '\\'):
            i = e + 1
            continue
        return e


_IF0 = re.compile(r'#[ \t]*if[ \t]*\(*[ \t]*0[ \t]*\)*[ \t]*(?=\r?\n|$|/[*/])')
_COND_OPEN = re.compile(r'[ \t]*#[ \t]*if(?:n?def)?\b')
_COND_MID = re.compile(r'[ \t]*#[ \t]*(?:else|elif)\b')
_COND_END = re.compile(r'[ \t]*#[ \t]*endif\b')


def _directive_end(text, i):
    """End offset (at the terminating newline) of the directive starting at i.

    `#if 0` also covers its dead lines up to the matching #else/#elif/#endif,
    which cpp skips without requiring balanced braces or literals.
    """
    e = _logical_line_end(text, i)
    if not _IF0.match(text, i):
        return e
    n, pos, depth, in_comment = len(text), e, 0, False
    while pos < n:
        line_start = pos + 1
        line_end = text.find('\n', line_start)
        line_end = n if line_end < 0 else line_end
        line = text[line_start:line_end]
        if not in_comment:
            if _COND_OPEN.match(line):
                depth += 1
            elif depth == 0 and (_COND_MID.match(line) or _COND_END.match(line)):
                return pos
            elif _COND_END.match(line):
                depth -= 1
        k = 0
        while k < len(line):
            if in_comment:
                close = line.find('*/', k)
                if close < 0:
                    break
                in_comment, k = False, close + 2
            else:
                opened = line.find('/*', k)
                if opened < 0 or '//' in line[k:opened]:
                    break
                in_comment, k = True, opened + 2
        pos = line_end
    raise ParseError(f'unterminated #if 0 at {_where(text, i)}')


def _logical_line_end(text, i):
    n, j = len(text), i + 1
    while j < n:
        c = text[j]
        if c == '\n':
            back = j - 1
            if back >= 0 and text[back] == '\r':
                back -= 1
            if back >= 0 and text[back] == '\\':
                j += 1
                continue
            return j
        if text.startswith('/*', j):
            e = text.find('*/', j + 2)
            if e < 0:
                raise ParseError(f'unterminated comment at {_where(text, j)}')
            j = e + 2
            continue
        if text.startswith('//', j):
            return _line_comment_end(text, j)
        if c in '"\'':
            j = _skip_literal(text, j, lenient=True)
            continue
        j += 1
    return n


def lexical(text):
    """Blank comments and literals (offsets and newlines preserved)."""
    out, i, n = list(text), 0, len(text)
    while i < n:
        c = text[i]
        if text.startswith('/*', i):
            e = text.find('*/', i + 2)
            e = n if e < 0 else e + 2
        elif text.startswith('//', i):
            e = _line_comment_end(text, i)
        elif c in '"\'':
            e = _skip_literal(text, i, lenient=True)
        else:
            i += 1
            continue
        for k in range(i, e):
            if out[k] != '\n':
                out[k] = ' '
        i = e
    return ''.join(out)


# ---------------------------------------------------------------------------
# Comment-neutral comparison
# ---------------------------------------------------------------------------
# User direction 2026-09-19: a change that only removes, rewords or adds C
# comments never blocks anything and never counts as an error. The published
# source is edited by hand (workflow citations were removed from comments of
# src/main/game_camera.c and src/main/ssd_1.c on 2026-09-16), so every check
# that compares source text has to be able to tell a comment edit from a code
# edit. Two texts are the same code when their C preprocessing-token streams,
# with every comment treated as whitespace (translation phase 3), are equal.
# Whitespace is not compared either, except that a newline ends a directive.

class LexError(ValueError):
    """The text cannot be tokenized (an unterminated comment or literal)."""


_PUNCTUATORS = tuple(sorted(('%:%:', '...', '<<=', '>>=', '->', '++', '--', '<<', '>>', '<=', '>=', '==',
                             '!=', '&&', '||', '*=', '/=', '%=', '+=', '-=', '&=', '^=', '|=', '##',
                             '<:', ':>', '<%', '%>', '%:'), key=len, reverse=True))
_TOKEN_IDENT = re.compile(r'[A-Za-z_$][A-Za-z_0-9$]*')
_TOKEN_NUMBER = re.compile(r'\.?[0-9](?:[eEpP][+-]|[0-9A-Za-z_.])*')
_LINE_SPLICE = re.compile(r'\\\r?\n')
DIRECTIVE_END = '\n'


def c_tokens(text):
    """The C preprocessing tokens of `text`, comments dropped.

    Backslash-newline splices are removed first (phase 2); a `/* */` or `//`
    comment is whitespace; string and character literals (with an `L` prefix)
    are single tokens, so comment markers inside them are text; identifiers,
    pp-numbers and punctuators follow maximal munch, so `a+ +b` and `a++b` stay
    different. Outside directives whitespace only separates tokens; a directive
    (a line whose first token is `#`) ends at the next newline outside a
    comment, which is emitted as DIRECTIVE_END. An unterminated comment, or an
    unterminated literal outside a directive, raises LexError; inside a
    directive (`#error don't`) a lone quote is one token, as cpp treats it.
    """
    text = _LINE_SPLICE.sub('', text)
    tokens, i, n = [], 0, len(text)
    directive, line_start = False, True
    while i < n:
        c = text[i]
        if c == '\n':
            if directive:
                tokens.append(DIRECTIVE_END)
                directive = False
            line_start = True
            i += 1
            continue
        if c in ' \t\r\f\v':
            i += 1
            continue
        if text.startswith('/*', i):
            e = text.find('*/', i + 2)
            if e < 0:
                raise LexError(f'unterminated comment at {_where(text, i)}')
            i = e + 2
            continue
        if text.startswith('//', i):
            e = text.find('\n', i)
            i = n if e < 0 else e
            continue
        if c in '"\'' or (c == 'L' and text[i + 1:i + 2] in ('"', "'")):
            q = i + 1 if c == 'L' else i
            quote, j = text[q], q + 1
            while j < n and text[j] != '\n':
                if text[j] == '\\':
                    j += 2
                    continue
                if text[j] == quote:
                    break
                j += 1
            if j < n and text[j] == quote:
                j += 1
            elif directive:
                j = q + 1
            else:
                raise LexError(f'unterminated literal at {_where(text, i)}')
            tok = text[i:j]
        else:
            m = _TOKEN_IDENT.match(text, i) or _TOKEN_NUMBER.match(text, i)
            tok = m.group() if m else next((p for p in _PUNCTUATORS if text.startswith(p, i)), c)
            j = i + len(tok)
        if line_start and tok in ('#', '%:'):
            directive = True
        line_start = False
        tokens.append(tok)
        i = j
    if directive:
        tokens.append(DIRECTIVE_END)
    return tokens


def code_key(text):
    """tuple(c_tokens(text)), or None when the text cannot be tokenized."""
    try:
        return tuple(c_tokens(text))
    except LexError:
        return None


def same_code(a, b):
    """True when a and b differ at most in comments and whitespace.

    Untokenizable text (an unterminated comment or literal) is the same code
    only as identical text, so such a difference keeps its strict treatment.
    """
    if a == b:
        return True
    key = code_key(a)
    return key is not None and key == code_key(b)


def is_comment_only(text):
    """True for non-blank text that holds nothing but comments and whitespace."""
    return bool(text.strip()) and code_key(text) == ()


def _strip_attributes(head):
    """Remove GNU __attribute__((...)) groups from a blanked declaration head."""
    while True:
        m = re.search(r'\b__attribute(?:__)?\s*\(', head)
        if not m:
            return head
        depth, k = 0, m.end() - 1
        while k < len(head):
            if head[k] == '(':
                depth += 1
            elif head[k] == ')':
                depth -= 1
                if depth == 0:
                    break
            k += 1
        head = head[:m.start()] + ' ' + head[k + 1:]


def _depth0_has(head, char):
    depth = 0
    for c in head:
        if c in '([{':
            depth += 1
        elif c in ')]}':
            depth -= 1
        elif c == char and depth == 0:
            return True
    return False


def _is_function_head(head):
    head = _strip_attributes(lexical(head)).strip()
    if not head.endswith(')') or '(' not in head:
        return False
    if re.match(r'typedef\b', head) or _depth0_has(head, '='):
        return False
    return True


def declarator_name(head):
    """Name of the declarator in a function head/prototype (blanked text)."""
    head = _strip_attributes(lexical(head))
    for m in _IDENT.finditer(head):
        word = m.group()
        if word in _NOT_NAMES or word in _TYPE_WORDS:
            continue
        k = m.end()
        while k < len(head) and head[k] in ' \t\r\n':
            k += 1
        if k >= len(head) or head[k] != '(':
            continue
        k += 1
        while k < len(head) and head[k] in ' \t\r\n':
            k += 1
        if k < len(head) and head[k] in '*^':
            continue  # grouping parenthesis of a declarator, not a parameter list
        return word
    return None


def scan(text):
    """Split text into top-level items; whitespace is the only text between items."""
    items, i, n = [], 0, len(text)
    cur, depth, paren, line_start, func_open = None, 0, 0, True, False
    cond = 0  # preprocessor conditional nesting of the text being scanned
    while i < n:
        c = text[i]
        if c == '\n':
            line_start = True
            i += 1
            continue
        if c in ' \t\r\f\v':
            i += 1
            continue
        if c == '#' and line_start:
            e = _directive_end(text, i)
            head = text[i:e].lstrip('#').strip()
            if cur is None and depth == 0:
                items.append(Item('pp', i, e, conditional=cond > 0))
            if _COND_OPEN.match(text[i:e]) and not _IF0.match(text, i):
                cond += 1
            elif _COND_END.match(text[i:e]):
                cond = max(0, cond - 1)
            del head
            i = e
            continue
        if text.startswith('/*', i):
            e = text.find('*/', i + 2)
            if e < 0:
                raise ParseError(f'unterminated comment at {_where(text, i)}')
            if cur is None:
                items.append(Item('comment', i, e + 2, conditional=cond > 0))
            i = e + 2
            continue
        if text.startswith('//', i):
            e = _line_comment_end(text, i)
            if cur is None:
                items.append(Item('comment', i, e, conditional=cond > 0))
            i = e
            continue
        line_start = False
        if cur is None:
            cur = i
        if c in '"\'':
            i = _skip_literal(text, i)
            continue
        if c in '([':
            paren += 1
        elif c in ')]':
            paren -= 1
            if paren < 0:
                raise ParseError(f'unbalanced ")" at {_where(text, i)}')
        elif c == '{':
            if depth == 0:
                func_open = paren == 0 and _is_function_head(text[cur:i])
            depth += 1
        elif c == '}':
            depth -= 1
            if depth < 0:
                raise ParseError(f'unbalanced "}}" at {_where(text, i)}')
            if depth == 0 and func_open:
                head = text[cur:text.index('{', cur)]
                name = declarator_name(head)
                if name is None:
                    raise ParseError(f'function definition without a name at {_where(text, cur)}')
                items.append(Item('func', cur, i + 1, name=name, conditional=cond > 0))
                cur, func_open = None, False
        elif c == ';' and depth == 0 and paren == 0:
            items.append(_classify_decl(text, Item('decl', cur, i + 1, conditional=cond > 0)))
            cur = None
        i += 1
    if cur is not None or depth or paren:
        raise ParseError(f'unterminated top-level item at {_where(text, cur or n)}')
    return items


def _classify_decl(text, item):
    raw = item.text(text)
    m = _MACRO_ITEM.fullmatch(raw)
    if m:
        macro, folder, name = m.groups()
        kind = {'INCLUDE_ASM': 'asm', 'ACCEPTED_ASM': 'accepted_asm', 'INCLUDE_RODATA': 'rodata'}[macro]
        if kind in ('asm', 'rodata') and (folder.startswith('src/') or not folder):
            raise ParseError(f'{macro} must name generated scaffold asm, not {folder!r} '
                             f'({_where(text, item.start)})')
        if kind == 'accepted_asm' and (not folder.startswith('src/') or 'nonmatchings' in folder.split('/')
                                       or folder.startswith('asm/')):
            raise ParseError(f'ACCEPTED_ASM must name tracked asm under src/<unit>/<tu>/, not {folder!r} '
                             f'({_where(text, item.start)})')
        if '..' in folder.split('/') or folder.startswith('/'):
            raise ParseError(f'unsafe asm folder {folder!r} ({_where(text, item.start)})')
        return Item(kind, item.start, item.end, name=name, asm_dir=folder, macro=macro,
                    conditional=item.conditional)
    if _MACRO_WORD.search(lexical(raw)):
        raise ParseError(f'unrecognised INCLUDE_ASM/ACCEPTED_ASM/INCLUDE_RODATA form at '
                         f'{_where(text, item.start)}: {" ".join(raw.split())[:80]}')
    return item


# ---------------------------------------------------------------------------
# Blocks and TU model
# ---------------------------------------------------------------------------

class Block:
    """A function block: attached comments + INCLUDE_ASM/ACCEPTED_ASM line or C definition.

    `key` is the original function name the block stands for (splat names a
    renamed local `name_<VA8>`; the C definition uses `name`). `covers` lists
    further original functions defined by the same included .s file.
    """
    __slots__ = ('name', 'key', 'state', 'start', 'end', 'core_start', 'core_end', 'asm_dir', 'static',
                 'role', 'covers', 'empty', 'conditional')

    def __init__(self, name, state, start, end, core_start, core_end, asm_dir=None, static=False):
        self.name, self.key, self.state, self.start, self.end = name, name, state, start, end
        self.core_start, self.core_end, self.asm_dir, self.static = core_start, core_end, asm_dir, static
        self.role, self.covers = 'function', []
        # True when the block sits inside a #if/#ifdef region: such text may or
        # may not be what the compiler saw, so tu_audit refuses to claim it.
        self.conditional = False
        # An empty C body (only whitespace/comments between the braces) is what
        # splat's auto_decompile_empty_functions writes; it is never a claim by itself.
        self.empty = False


_VA_SUFFIX = re.compile(r'^(.+)_([0-9A-Fa-f]{8})$')
_DOT_SUFFIX = re.compile(r'^(.+)\.([0-9]+)$')
_US_SUFFIX = re.compile(r'^(.+)_([0-9]+)$')

# A nested definition belongs to its enclosing TU-level C block. The editor
# binds unique original names; the audit checks the declared identity/parent.
GNU_NESTED_REGISTRY = 'config/gnu-nested-functions.json'
_GNU_NESTED_DEF = re.compile(r'([A-Za-z_]\w*)\s*\([^;{}]*\)\s*\{')
_GNU_NESTED_KEYWORDS = frozenset(('if', 'for', 'while', 'switch', 'catch'))


def gnu_nested_mask(text):
    """Mask comments and literals while preserving offsets and newlines."""
    return re.sub(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|/\*.*?\*/|//[^\n]*',
                  lambda m: ''.join('\n' if c == '\n' else ' ' for c in m[0]),
                  text, flags=re.S)


def gnu_nested_definitions(text):
    """Return structural function definitions with lexical parent metadata."""
    masked = gnu_nested_mask(text)
    definitions = []
    for match in _GNU_NESTED_DEF.finditer(masked):
        if match.group(1) in _GNU_NESTED_KEYWORDS:
            continue
        opening = masked.find('{', match.start(), match.end())
        if opening < 0:
            continue
        depth = sum(1 if c == '{' else -1 for c in masked[:match.start()]
                    if c in '{}')
        balance = 0
        closing = None
        for index in range(opening, len(masked)):
            if masked[index] == '{':
                balance += 1
            elif masked[index] == '}':
                balance -= 1
                if balance == 0:
                    closing = index
                    break
        definitions.append(dict(name=match.group(1), start=match.start(),
                                opening=opening, closing=closing, depth=depth,
                                parent=None))
    for definition in definitions:
        parents = [candidate for candidate in definitions
                   if candidate is not definition and candidate['opening'] < definition['start']
                   and candidate['closing'] is not None and definition['start'] < candidate['closing']]
        if parents:
            definition['parent'] = max(parents, key=lambda candidate: candidate['start'])['name']
    return definitions


def gnu_nested_parent(text, parent, child):
    """Whether *child* is a real nested definition directly in *parent*."""
    return any(item['name'] == child and item['parent'] == parent
               for item in gnu_nested_definitions(text))


def gnu_nested_specs(root, tu_id):
    """Merge discovered groups with durable/private per-TU source declarations."""
    result = {}
    root = Path(root)
    discovery = root / GNU_NESTED_REGISTRY
    if discovery.is_file():
        data = json.loads(discovery.read_text())
        if data.get('schema') != 'gnu-nested-functions/1':
            raise ParseError(f'{discovery}: unsupported nested-function registry schema')
        result.update((data.get('tus', {}).get(tu_id, {}).get('functions') or {}))
    classes = root / 'config/tu/source-classes.json'
    if classes.is_file():
        entry = json.loads(classes.read_text()).get('tus', {}).get(tu_id, {})
        for parent, spec in (entry.get('functions') or {}).items():
            if spec.get('nested_functions') is not None:
                result[parent] = dict(result.get(parent, {}), **spec)
    return result


def name_stem(name):
    m = _VA_SUFFIX.match(name)
    return m.group(1) if m else name


def same_function(a, b):
    """Equal names, or splat renames of the same original.

    splat spells a local `name` at VA as `name_<VA8>` and a dotted compiler
    static `name.N` as `name_N`; both bind to the original name.
    """
    if a == b:
        return True
    sa, sb = name_stem(a), name_stem(b)
    if sa == sb and ((sa != a) != (sb != b)):
        return True
    for x, y in ((a, b), (b, a)):
        dot, us = _DOT_SUFFIX.match(x), _US_SUFFIX.match(y)
        if dot and us and dot.group(1) == us.group(1) and dot.group(2) == us.group(2):
            return True
    return False


def _starts_line(text, offset):
    k = offset - 1
    while k >= 0 and text[k] in ' \t':
        k -= 1
    return k < 0 or text[k] == '\n'


def _attach(text, items, k):
    """Extent of block item k including attached leading/trailing comments."""
    start, end = items[k].start, items[k].end
    j = k - 1
    while j >= 0 and items[j].kind == 'comment' and _starts_line(text, items[j].start):
        gap = text[items[j].end:start]
        if gap.strip() or gap.count('\n') > 1:
            break
        start = items[j].start
        j -= 1
    m = k + 1
    while m < len(items) and items[m].kind == 'comment' and '\n' not in text[end:items[m].start]:
        end = items[m].end
        m += 1
    return start, end


class TU:
    """Parsed TU file. Immutable: edits build a new TU from a spliced text.

    functions: original function names of the TU in order (skeleton, TU map or
    config/tu-build.json). With it, C definitions outside the list are helpers,
    and list names must each have a block or be covered by an included .s
    (resolved against unit_dir). Without it, inline definitions are helpers.
    """

    def __init__(self, text, path=None, functions=None, unit_dir=None):
        self.text, self.path = text, path
        self.function_list = list(functions) if functions is not None else None
        self.unit_dir = unit_dir
        self.items = scan(text)
        self.blocks = []
        items = self.items
        claimed = set()
        for k, item in enumerate(items):
            if item.kind not in ('func', 'asm', 'accepted_asm'):
                continue
            start, end = _attach(text, items, k)
            # A comment already attached as trailing text of the previous block
            # cannot also lead this one.
            if self.blocks and start < self.blocks[-1].end:
                start = self.blocks[-1].end
                while start < item.start and text[start] in ' \t\r\n':
                    start += 1
            state = 'c' if item.kind == 'func' else item.kind
            static = inline = False
            if state == 'c':
                head = lexical(text[item.start:text.index('{', item.start)])
                static = re.search(r'\bstatic\b', head) is not None
                inline = re.search(r'\b(?:inline|__inline|__inline__)\b', head) is not None
            block = Block(item.name, state, start, end, item.start, item.end, item.asm_dir, static)
            block.conditional = item.conditional
            if state == 'c':
                body = text[text.index('{', item.start) + 1:item.end - 1]
                block.empty = not lexical(body).strip()
            if inline and functions is None:
                # Without an original function list an inline definition is
                # taken to be a helper (fully inlined, no symbol of its own).
                block.role = 'helper'
            if item.name in claimed:
                raise ParseError(f'duplicate function block {item.name!r} at {_where(text, item.start)}')
            claimed.add(item.name)
            self.blocks.append(block)
        self.covered = {}
        if functions is not None:
            self._bind(list(functions))
        self.by_name = {}
        for b in self.blocks:
            self.by_name.setdefault(b.name, b)
        for b in self.blocks:
            self.by_name.setdefault(b.key, b)
            for covered in b.covers:
                self.by_name.setdefault(covered, b)

    def _bind(self, functions):
        """Bind blocks to the original function list, in order.

        A block whose own name is not an original function is deferred: an
        included .s may define several original functions (one accepted record
        covering a group), and its file name is only the first of them.
        """
        pending, deferred = list(functions), []
        for b in self.blocks:
            match = next((n for n in pending if n == b.name), None)
            if match is None:
                stems = [n for n in pending if same_function(n, b.name)]
                match = stems[0] if len(stems) == 1 else None
            if match is None:
                if b.state == 'c':
                    b.role = 'helper'
                    continue
                deferred.append(b)
                continue
            b.key, b.role = match, 'function'
            pending.remove(match)
            # A GNU nested function is emitted as a local symbol but its C
            # definition is lexically inside its parent, so it has no separate
            # non-overlapping Block. Bind unique nested originals to the parent while
            # retaining the original child name for claims and audits.
            if b.state == 'c':
                body = self.text[b.core_start:b.core_end]
                for definition in gnu_nested_definitions(body):
                    if definition['depth'] == 0:
                        continue
                    hits = [name for name in pending if name == definition['name'] or
                            re.fullmatch(re.escape(definition['name']) + r'\.\d+', name)]
                    if len(hits) > 1:
                        raise ParseError(f'ambiguous original nested binding for {match}/{definition["name"]}: {hits}')
                    if hits:
                        nested = hits[0]
                        b.covers.append(nested)
                        self.covered[nested] = b.key
                        pending.remove(nested)
        for b in [x for x in self.blocks if x.state != 'c']:
            if not pending:
                break
            defined = (asm_file_functions(Path(self.unit_dir) / f'{b.asm_dir}/{b.name}.s')
                       if self.unit_dir is not None else None)
            hits = []
            for name in defined or []:
                hit = next((n for n in pending if same_function(n, name) and n not in hits), None)
                if hit is not None:
                    hits.append(hit)
            if not hits:
                continue
            if b in deferred:
                b.key, b.role = hits.pop(0), 'function'
                pending.remove(b.key)
                deferred.remove(b)
            for n in hits:
                if same_function(n, b.key):
                    continue
                b.covers.append(n)
                self.covered[n] = b.key
                pending.remove(n)
        for b in deferred:
            if b.state == 'accepted_asm':
                raise ParseError(f'{b.name}: ACCEPTED_ASM block is not an original function of this TU'
                                 + ('' if self.unit_dir else ' (pass unit_dir/--unit-dir so a shared accepted .s '
                                                            'can be resolved)'))
            # splat pseudo-function (e.g. func_<VA> over padding): scaffolding only
            b.role = 'scaffold'
        if pending:
            raise ParseError(f'original functions without a block in this TU: {pending}'
                             + ('' if self.unit_dir else ' (pass unit_dir/--unit-dir to resolve functions '
                                                         'defined by a shared included .s)'))
        order = [b.key for b in self.blocks if b.role == 'function']
        if [n for n in functions if n not in self.covered] != order:
            raise ParseError(f'function blocks are not in original order: {order}')

    # -- views ---------------------------------------------------------------
    def block(self, name):
        b = self.by_name.get(name)
        if b is None:
            raise EditError(f'no function block named {name!r}')
        return b

    def block_text(self, b):
        return self.text[b.start:b.end]

    def functions(self):
        return [b for b in self.blocks if b.role == 'function']

    def prelude_end(self):
        return self.blocks[0].start if self.blocks else len(self.text)

    def prelude(self):
        return self.text[:self.prelude_end()]

    def free_items(self):
        """Top-level items outside function blocks (prelude, interstitial, epilogue)."""
        spans = [(b.start, b.end) for b in self.blocks]
        out = []
        for item in self.items:
            if not any(s <= item.start and item.end <= e for s, e in spans):
                out.append(item)
        return out

    def asm_dirs(self):
        return sorted({b.asm_dir for b in self.blocks if b.state == 'asm'})

    def describe(self, unit_dir=None):
        unit_dir = unit_dir or self.unit_dir
        rows = []
        for i, b in enumerate(self.blocks):
            row = dict(index=i, name=b.name, state=b.state, role=b.role,
                       start_line=line_of(self.text, b.start), end_line=line_of(self.text, b.end),
                       sha256=sha256_text(self.block_text(b)))
            if b.key != b.name:
                row['original_name'] = b.key
            if b.conditional:
                row['conditional'] = True
            if b.state == 'c':
                row['static'] = b.static
                if b.empty:
                    row['empty_body'] = True
            if b.asm_dir is not None:
                row['asm_dir'] = b.asm_dir
                row['asm_path'] = f'{b.asm_dir}/{b.name}.s'
                if unit_dir is not None:
                    defined = asm_file_functions(Path(unit_dir) / row['asm_path'])
                    if defined is not None:
                        row['defines'] = defined
            if b.covers:
                row['covers'] = list(b.covers)
            rows.append(row)
        return rows

    # -- edits ----------------------------------------------------------------
    def splice(self, start, end, new):
        return TU(self.text[:start] + new + self.text[end:], self.path, self.function_list, self.unit_dir)


def asm_file_functions(path):
    """Function names defined by an included .s (glabel / .ent), or None if unreadable."""
    try:
        text = Path(path).read_text(encoding='utf-8', errors='surrogateescape')
    except OSError:
        return None
    names = []
    for m in re.finditer(r'^\s*(?:glabel\s+([A-Za-z_.$][\w.$]*)|\.ent\s+([A-Za-z_.$][\w.$]*))', text, re.M):
        name = m.group(1) or m.group(2)
        if name not in names:
            names.append(name)
    return names


# ---------------------------------------------------------------------------
# Accepted-assembly group split (user decision 2026-09-12)
# ---------------------------------------------------------------------------
#
# One tracked `.s` under `src/<unit>/<tu>/` may carry several original
# functions, and `tools/tu_audit.py` hashes it whole against the accepted
# `config/units` record that covers exactly those functions.  So while any one
# function of the group resists recovery, none of the others can be delivered
# as C.  The split re-cuts the group: the functions that stay assembly move
# into a new tracked `.s` holding exactly their bodies, **the same bytes**, and
# the record is narrowed to them with the provenance of what it no longer
# covers.  Nothing about the acceptance gate changes - the whole-file SHA-256
# of every file the TU links into must still be identical and every claimed
# function still passes the audit on its own merits.
#
# The re-cut is not a rewrite: `asm_slices` tiles the original text into a
# header, one slice per function and a tail, so that concatenating them
# reproduces the original byte for byte (asserted, not assumed).  A re-cut is
# then the concatenation of the header, the kept slices in their original order
# and the tail; every retained byte is a byte of the reviewed original, at a
# recorded offset, and `split-asm --check` re-derives the result from the
# original and compares.

SCHEMA_ASM_SPLIT = 'tu-asm-split/1'

_ASM_ENT = re.compile(r'^[ \t]*\.ent[ \t]+([A-Za-z_.$][\w.$]*)[ \t]*$', re.M)
_ASM_END = re.compile(r'^[ \t]*\.end[ \t]+([A-Za-z_.$][\w.$]*)[ \t]*$', re.M)
# Directives that belong to the function they precede rather than to the file:
# its placement and its symbol declaration.
_ASM_LEAD = re.compile(r'^[ \t]*(?:\.align|\.p2align|\.balign|\.globl|\.global|\.weak|\.local'
                       r'|\.type|\.size|\.hidden|/\*|\*|//|\#)')
# The same directives, matched one line at a time. `_ASM_LEAD` is anchored with
# `^` and compiled without re.MULTILINE, so `.match(text, pos, end)` only ever
# succeeds at pos 0: in practice every function but the first takes its lead-in
# from the tiling (a slice starts where the previous `.end` line ended), and the
# FIRST function's lead-in stays in the file header. That is what
# `_header_lead_in` finds and what `split_asm`'s boundary check guards. The
# tiling is not changed here: the published `tu-asm-split/1` manifests pin the
# byte ranges it produced.
_ASM_LEAD_LINE = re.compile(r'^[ \t]*(?:\.align|\.p2align|\.balign|\.globl|\.global|\.weak|\.local'
                            r'|\.type|\.size|\.hidden|/\*|\*|//|\#)')


def _header_lead_in(header):
    """Offset in `header` where the first function's own lead-in begins.

    The header of a tracked group is the file's own preamble (`.text`,
    `.set noreorder`, ...) followed, with no blank line between, by the
    placement and declaration directives of the first `.ent`. Those belong to
    the first function: if it is dropped, keeping them leaves a `.globl` for a
    symbol the file no longer defines and an `.align` before a different body.
    """
    lines = header.splitlines(keepends=True)
    i = len(lines)
    while i > 0 and _ASM_LEAD_LINE.match(lines[i - 1]):
        i -= 1
    return sum(len(line) for line in lines[:i])


class AsmSlice:
    """One function's verbatim extent in a tracked `.s`.

    `start`/`end` tile the file (the previous slice's end is this one's start),
    so the slice carries the blank line and the `.align`/`.globl` lead-in that
    belong to this function.  `body_start`/`body_end` are the `.ent` ... `.end`
    lines themselves, which is what the original bytes of the function are
    assembled from.
    """

    __slots__ = ('name', 'index', 'start', 'end', 'body_start', 'body_end')

    def __init__(self, name, index, start, end, body_start, body_end):
        self.name, self.index = name, index
        self.start, self.end = start, end
        self.body_start, self.body_end = body_start, body_end


def _line_start(text, i):
    return text.rfind('\n', 0, i) + 1


def _line_end(text, i):
    j = text.find('\n', i)
    return len(text) if j < 0 else j + 1


def asm_slices(text, path=None):
    """(header, [AsmSlice, ...], tail) tiling a tracked `.s` byte for byte.

    Refuses a file whose `.ent`/`.end` directives do not pair up in order with
    matching names, or that defines the same function twice: a group whose
    structure cannot be established is never re-cut.
    """
    where = f' in {path}' if path else ''
    ents = list(_ASM_ENT.finditer(text))
    ends = list(_ASM_END.finditer(text))
    if not ents:
        raise EditError(f'no .ent directive{where}: not a tracked accepted-assembly file')
    if len(ents) != len(ends):
        raise EditError(f'{len(ents)} .ent but {len(ends)} .end directive(s){where}')
    slices, seen, cursor = [], set(), None
    for index, (ent, end) in enumerate(zip(ents, ends)):
        name = ent.group(1)
        if name != end.group(1):
            raise EditError(f'.ent {name} is closed by .end {end.group(1)}{where}')
        if end.start() < ent.start():
            raise EditError(f'.end {name} precedes its .ent{where}')
        if cursor is not None and ent.start() < cursor:
            raise EditError(f'.ent {name} overlaps the previous function{where}')
        if name in seen:
            raise EditError(f'{name} is defined twice{where}')
        seen.add(name)
        body_start = _line_start(text, ent.start())
        body_end = _line_end(text, end.start())
        # The lead-in is the run of placement/declaration lines immediately
        # above `.ent`, blank lines excluded: those separate functions and stay
        # with the slice that follows them.
        lead = body_start
        while lead > 0:
            prev = _line_start(text, lead - 1)
            if not _ASM_LEAD.match(text, prev, lead):
                break
            lead = prev
        slices.append(AsmSlice(name, index, lead, body_end, body_start, body_end))
        cursor = body_end
    header = text[:slices[0].start]
    # Tile: every slice starts where the previous one ended.
    for i in range(1, len(slices)):
        if slices[i].start < slices[i - 1].end:
            raise EditError(f'the lead-in of {slices[i].name} reaches into '
                            f'{slices[i - 1].name}{where}')
        slices[i].start = slices[i - 1].end
    tail = text[slices[-1].end:]
    rebuilt = header + ''.join(text[s.start:s.end] for s in slices) + tail
    if rebuilt != text:
        raise EditError(f'the slice model does not reproduce{where}: refusing to re-cut')
    return header, slices, tail


def split_asm(text, keep, path=None, drop_tail=False, drop_header_lead_in=False):
    """Re-cut a tracked `.s` to the `keep` functions, verbatim.

    Returns (new text, manifest).  Every kept byte comes from the original at
    the offsets the manifest records; nothing is rewritten, reordered or
    re-indented.

    Two boundary hazards are refused instead of being emitted (independent
    review of `tu-asm-group-split`, finding D5).  The file header carries the
    FIRST function's `.align`/`.globl` and the file tail - in all thirteen
    tracked groups that have one - is the terminal `.align` of the LAST
    function.  Dropping either of those functions while carrying its directives
    would leave a declaration for a symbol the file no longer defines, or
    alignment padding that belonged to a body now delivered as C.  The caller
    must re-attribute explicitly (`drop_header_lead_in` / `drop_tail`), and the
    decision is recorded in the manifest so `--check` re-derives the same bytes.
    """
    header, slices, tail = asm_slices(text, path)
    names = [s.name for s in slices]
    keep = list(keep)
    unknown = [n for n in keep if n not in names]
    if unknown:
        raise EditError(f'{", ".join(unknown)} is not defined by this .s ({", ".join(names)})')
    if len(set(keep)) != len(keep):
        raise EditError('a function is named twice in --keep')
    kept = set(keep)
    if not kept:
        raise EditError('a re-cut keeps at least one function; delete the file instead')
    if kept == set(names):
        raise EditError('--keep names every function: there is nothing to split off')

    where = f' in {path}' if path else ''
    lead_start = _header_lead_in(header)
    stranded_lead = header[lead_start:]
    orig_header_end, orig_tail = len(header), tail
    orig_tail_start = len(text) - len(tail)
    header_span = (0, len(header))
    tail_span = (orig_tail_start, len(text))
    if names[0] not in kept and stranded_lead.strip():
        if not drop_header_lead_in:
            raise EditError(
                f'the header{where} carries the lead-in of {names[0]}, which --keep drops '
                f'({stranded_lead.strip()!r}): the re-cut would declare a symbol the file no '
                'longer defines and align a body that is not the one the directive belongs to. '
                'Keep the first function, or re-attribute the lead-in explicitly with '
                '--drop-header-lead-in after confirming it belongs to it.')
        header = header[:lead_start]
        header_span = (0, lead_start)
    if names[-1] not in kept and tail.strip():
        if not drop_tail:
            raise EditError(
                f'the tail{where} follows the last function {names[-1]}, which --keep drops '
                f'({tail.strip()!r}): in every tracked group the tail is that function\'s own '
                'terminal alignment, and carrying it would pad after a different body while the '
                'compiler emits the converted function\'s own gap. Keep the last function, or '
                'drop the tail explicitly with --drop-tail after confirming it belongs to it.')
        tail = ''
        tail_span = (len(text), len(text))
    out, rows, cursor = [header], [], len(header)
    for s in slices:
        piece = text[s.start:s.end]
        row = dict(name=s.name, kept=s.name in kept,
                   origin=dict(start=s.start, end=s.end, sha256=sha256_text(piece)),
                   body=dict(start=s.body_start, end=s.body_end,
                             sha256=sha256_text(text[s.body_start:s.body_end])))
        if s.name in kept:
            row['result'] = dict(start=cursor, end=cursor + len(piece))
            row['body']['result'] = dict(start=cursor + (s.body_start - s.start),
                                         end=cursor + (s.body_end - s.start))
            out.append(piece)
            cursor += len(piece)
        rows.append(row)
    out.append(tail)
    result = ''.join(out)
    manifest = dict(
        schema=SCHEMA_ASM_SPLIT,
        origin=dict(path=path, sha256=sha256_text(text), bytes=len(text),
                    functions=names),
        keep=[n for n in names if n in kept],
        drop=[n for n in names if n not in kept],
        result=dict(path=None, sha256=sha256_text(result), bytes=len(result)),
        header=dict(start=header_span[0], end=header_span[1], sha256=sha256_text(header)),
        tail=dict(start=tail_span[0], end=tail_span[1], sha256=sha256_text(tail)),
        functions=rows,
    )
    if drop_header_lead_in or drop_tail:
        # Only recorded when a boundary was re-attributed, so the manifests of
        # the ordinary case keep the shape they were reviewed in.
        manifest['boundary'] = dict(
            header_lead_in_dropped=bool(drop_header_lead_in),
            header_lead_in=(dict(start=lead_start, end=orig_header_end, function=names[0],
                                 sha256=sha256_text(stranded_lead))
                            if drop_header_lead_in else None),
            tail_dropped=bool(drop_tail),
            tail=(dict(start=orig_tail_start, end=len(text), function=names[-1],
                       sha256=sha256_text(orig_tail)) if drop_tail else None))
    return result, manifest


def check_asm_split(manifest, origin_text, result_text):
    """Re-derive the re-cut from the original and compare; returns the report.

    This is the proof the split rests on: the tracked `.s` that carries the
    functions still delivered as assembly is byte for byte the corresponding
    bodies of the reviewed original, and the kept and dropped names partition
    the original's function list with nothing counted twice or dropped in
    silence.
    """
    problems = []
    if manifest.get('schema') != SCHEMA_ASM_SPLIT:
        raise EditError(f'not a {SCHEMA_ASM_SPLIT} manifest')
    origin_sha = sha256_text(origin_text)
    if origin_sha != manifest['origin']['sha256']:
        problems.append(f'the original is sha256 {origin_sha[:12]}..., the manifest pins '
                        f'{manifest["origin"]["sha256"][:12]}...')
    names = asm_slices(origin_text, manifest['origin'].get('path'))[1]
    names = [s.name for s in names]
    if names != list(manifest['origin']['functions']):
        problems.append(f'the original defines {names}, the manifest records '
                        f'{manifest["origin"]["functions"]}')
    keep, drop = list(manifest['keep']), list(manifest['drop'])
    if sorted(keep + drop) != sorted(names):
        problems.append(f'keep+drop is {sorted(keep + drop)}, the original is {sorted(names)}')
    if set(keep) & set(drop):
        problems.append(f'counted twice: {sorted(set(keep) & set(drop))}')
    # Every per-function `body` range the manifest records is verified against
    # the original text it claims to describe, not only the `origin` range
    # around it (independent review of `tu-asm-group-split`, finding D4): a
    # recorded-but-unchecked field invites drift.
    for b in manifest.get('functions', []):
        body = b.get('body')
        if not isinstance(body, dict) or 'start' not in body or 'end' not in body:
            problems.append(f'the manifest entry for {b.get("name")} records no body range')
            continue
        start, end = body['start'], body['end']
        if not (0 <= start <= end <= len(origin_text)):
            problems.append(f'the body range of {b.get("name")} ({start}..{end}) is not inside '
                            f'the original ({len(origin_text)} bytes)')
            continue
        digest = sha256_text(origin_text[start:end])
        if digest != body.get('sha256'):
            problems.append(f'the body of {b.get("name")} at {start}..{end} is sha256 '
                            f'{digest[:12]}..., the manifest pins {str(body.get("sha256"))[:12]}...')

    derived = derived_sha = None
    if not problems:
        boundary = manifest.get('boundary') or {}
        try:
            derived, fresh = split_asm(origin_text, keep, manifest['origin'].get('path'),
                                       drop_tail=bool(boundary.get('tail_dropped')),
                                       drop_header_lead_in=bool(boundary.get('header_lead_in_dropped')))
        except EditError as exc:
            derived, fresh = None, dict(functions=[])
            problems.append(f'the re-cut cannot be derived from the original: {exc}')
        if derived is not None:
            derived_sha = sha256_text(derived)
            if derived != result_text:
                problems.append('the re-cut does not reproduce from the original')
        if len(fresh['functions']) != len(manifest.get('functions', [])):
            problems.append(f'the original has {len(fresh["functions"])} function entries, '
                            f'the manifest records {len(manifest.get("functions", []))}')
        for a, b in zip(fresh['functions'], manifest['functions']):
            if a['name'] != b['name'] or a['origin'] != b['origin'] or a['kept'] != b['kept']:
                problems.append(f'the manifest entry for {b["name"]} does not match the original')
            elif a['body'] != b['body']:
                problems.append(f'the body record of {b["name"]} does not match the original '
                                f'({a["body"]} vs {b["body"]})')
    result_sha = sha256_text(result_text)
    if result_sha != manifest['result']['sha256']:
        problems.append(f'the re-cut is sha256 {result_sha[:12]}..., the manifest pins '
                        f'{manifest["result"]["sha256"][:12]}...')
    return dict(schema=SCHEMA_ASM_SPLIT, ok=not problems, problems=problems,
                origin=dict(sha256=origin_sha, functions=names),
                result=dict(sha256=result_sha, derived_sha256=derived_sha),
                keep=keep, drop=drop,
                bytes_kept=sum(f['origin']['end'] - f['origin']['start']
                               for f in manifest['functions'] if f['kept']))


def parse(text, path=None, functions=None, unit_dir=None):
    return TU(text, path, functions, unit_dir)


def load(path, functions=None, unit_dir=None):
    return TU(Path(path).read_text(encoding='utf-8', errors='surrogateescape'), str(path), functions, unit_dir)


def skeleton_functions(skeleton):
    return [b.name for b in skeleton.blocks if b.state != 'c'] if skeleton else None


# ---------------------------------------------------------------------------
# Function-level operations
# ---------------------------------------------------------------------------

def _clean_block_text(text):
    return text.strip('\n').rstrip()


def extract_function(source_text, name, declarations=False):
    """Text of the C definition `name` in another C file, attached comments included.

    With `declarations`, the file's top-level declarations before the definition
    (typedefs, macros, externs, prototypes) are returned in front of it, for
    `to_c` to place the missing ones beside the function.
    """
    tu = TU(source_text)
    b = tu.by_name.get(name)
    if b is None or b.state != 'c':
        raise EditError(f'no C definition of {name!r} in the source')
    if not declarations:
        return tu.block_text(b)
    lead = [item.text(source_text) for item in scan(source_text)
            if item.kind in ('pp', 'decl') and item.end <= b.start]
    return '\n'.join(lead + [tu.block_text(b)])


def check_c_body(text, name, alt=None):
    """The replacement must be exactly one C definition (plus attached comments) named name."""
    probe = TU(text)
    funcs = [b for b in probe.blocks if b.state == 'c']
    free = [i for i in probe.free_items() if i.kind != 'comment']
    if len(probe.blocks) != 1 or len(funcs) != 1 or free:
        raise EditError('a C body must contain exactly one function definition and no other '
                        'top-level declarations (put those in the prelude)')
    if funcs[0].name not in (name, alt):
        raise EditError(f'C body defines {funcs[0].name!r}, expected {name!r}')
    return funcs[0]


def split_leading_declarations(c_text):
    """(leading declaration items, definition text) of a body that bundles its prototypes.

    A bulk/m2c function file carries the typedefs, macros, externs and forward
    prototypes its definition needs, then the one definition. Anything after the
    definition, or a second definition, is still refused by check_c_body.
    """
    text = _clean_block_text(c_text)
    try:
        items = scan(text)
    except ParseError:
        return [], text
    funcs = [k for k, item in enumerate(items) if item.kind == 'func']
    if len(funcs) != 1 or any(item.kind not in ('comment',) for item in items[funcs[0] + 1:]):
        return [], text
    k = funcs[0]
    start = items[k].start
    # comments directly attached above the definition stay with it
    j = k - 1
    while j >= 0 and items[j].kind == 'comment' and _starts_line(text, items[j].start) \
            and text[items[j].end:start].count('\n') <= 1 and not text[items[j].end:start].strip():
        start = items[j].start
        j -= 1
    lead = [item.text(text) for item in items[:j + 1] if item.kind in ('pp', 'decl', 'comment')]
    return lead, text[start:]


def _declared_in(tu, item_text):
    """True when `item_text` (or a declaration of the same names) is already in the TU."""
    norm = _norm(item_text)
    keys = decl_keys(item_text)
    for item in scan(tu.text):
        if item.kind not in ('pp', 'decl'):
            continue
        other = item.text(tu.text)
        if _norm(other) == norm:
            return True
        if keys and keys & decl_keys(other):
            return True
    return False


def to_c(tu, name, c_text, as_name=None, replace_accepted_asm=False, report=None):
    """Replace block `name` with the C definition c_text (defining as_name or name).

    `c_text` may start with the declarations the definition needs (a bulk/m2c
    function file): those the TU does not declare yet are placed immediately
    before the function (interstitial text of an allocated function, which a
    function-level patch carries); those it already declares are skipped and
    listed in `report['skipped_declarations']`.
    """
    b = tu.block(name)
    if b.conditional:
        raise EditError(f'{name} sits inside a preprocessor conditional: edit the source by hand')
    if b.state == 'accepted_asm' and not replace_accepted_asm:
        raise EditError(f'{name} is accepted exact_asm; pass replace_accepted_asm for a re-treatment')
    lead, body = split_leading_declarations(c_text)
    defined = check_c_body(body, as_name or b.key, alt=None if as_name else name_stem(b.name)).name
    if not same_function(defined, b.key) and not same_function(defined, b.name) and defined != as_name:
        raise EditError(f'C body defines {defined!r}, not the function of block {name!r}')
    kept, skipped = [], []
    for item in lead:
        if item.lstrip().startswith(('/*', '//')):
            continue                            # the file's own banner/notes, not declarations
        if decl_keys(item) == {('ordinary', defined)}:
            skipped.append(item)                # the definition's own prototype
            continue
        (skipped if _declared_in(tu, item) or any(_norm(item) == _norm(k) for k in kept)
         else kept).append(item)
    if report is not None:
        report['added_declarations'] = kept
        report['skipped_declarations'] = skipped
    text = ('\n'.join(kept) + '\n\n' + body) if kept else body
    out = tu.splice(b.start, b.end, text)
    _check_same_layout(tu, out, {b.name: defined})
    return out


def include_line(name, folder, macro='INCLUDE_ASM'):
    return f'{macro}("{folder}", {name});'


def fallback_line(tu, name, skeleton=None, asm_dir=None):
    """The INCLUDE_ASM/ACCEPTED_ASM line a block reverts to.

    Preference: the skeleton's line for that function; else INCLUDE_ASM with
    the given or the TU's unique scaffold folder. With a unit_dir the .s must
    exist; a splat `name_<VA8>.s` is found for a renamed local.
    """
    b = tu.block(name)
    if skeleton is not None:
        sb = next((x for x in skeleton.blocks if x.name in (b.name, b.key)), None) or next(
            (x for x in skeleton.blocks if same_function(x.name, b.key) or same_function(x.name, b.name)), None)
        if sb is None:
            raise EditError(f'skeleton has no block {name!r}')
        if sb.state == 'c':
            raise EditError(f'skeleton block {name!r} is C, not INCLUDE_ASM/ACCEPTED_ASM')
        return skeleton.text[sb.core_start:sb.core_end]
    if asm_dir is None:
        dirs = tu.asm_dirs()
        if len(dirs) != 1:
            raise EditError(f'cannot infer the INCLUDE_ASM folder for {name} (found {dirs}); '
                            'pass a skeleton or asm_dir')
        asm_dir = dirs[0]
    target = b.name
    if tu.unit_dir is not None:
        folder = Path(tu.unit_dir) / asm_dir
        if not (folder / f'{target}.s').is_file():
            found = sorted(p.stem for p in folder.glob(f'{name_stem(b.key)}_*.s')
                           if same_function(p.stem, b.key))
            if len(found) != 1:
                raise EditError(f'no scaffold asm for {name} in {folder}')
            target = found[0]
    return include_line(target, asm_dir)


def to_asm(tu, name, line=None, skeleton=None, asm_dir=None, allow_accepted=False):
    """Revert block `name` to its INCLUDE_ASM (or skeleton ACCEPTED_ASM) line."""
    b = tu.block(name)
    if b.conditional:
        raise EditError(f'{name} sits inside a preprocessor conditional: edit the source by hand')
    if b.state == 'accepted_asm' and not allow_accepted:
        raise EditError(f'{name} is accepted exact_asm; refusing to replace it')
    if line is None:
        line = fallback_line(tu, name, skeleton, asm_dir)
    m = _MACRO_ITEM.fullmatch(line.strip())
    if not m or m.group(1) == 'INCLUDE_RODATA' or not (same_function(m.group(3), b.name)
                                                       or same_function(m.group(3), b.key)):
        raise EditError(f'not an INCLUDE_ASM/ACCEPTED_ASM line for {name}: {line!r}')
    out = tu.splice(b.start, b.end, line.strip())
    _check_same_layout(tu, out, {b.name: m.group(3)})
    return out


def _check_same_layout(before, after, renames):
    want = [renames.get(b.name, b.name) for b in before.blocks]
    got = [b.name for b in after.blocks]
    if len(want) != len(got) or not all(same_function(w, g) for w, g in zip(want, got)):
        raise EditError(f'edit changed the function block layout: {want} -> {got}')


def tu_header_path(tu):
    """The TU's own `<tu>.h`, when this TU was loaded from a path and it exists."""
    if not tu.path:
        return None
    header = Path(tu.path).with_suffix('.h')
    return header if header.is_file() else None


def declared_names(text):
    """Ordinary (non-macro, non-tag) names a piece of file-scope C text declares.

    The text is parsed with the TU tokenizer, which sees preprocessor lines,
    comments and literals; a header has no function blocks, so every item it
    holds is a free item.
    """
    names = set()
    for item in parse(text).free_items():
        if item.kind == 'decl':
            names |= {key for kind, key in decl_keys(item.text(text)) if kind == 'ordinary'}
    return names


def subset(tu, keep, skeleton=None, asm_dir=None, headers=None):
    """Keep only `keep` as C; every other original C function reverts. Returns (TU, report).

    `headers` are extra file-scope declaration sources to consider before warning
    that a reverted static has no prototype. It defaults to the TU's own
    `<tu>.h`, which is where a TU-local prototype normally lives
    (include/README.md): warning about a name that header already declares was a
    false positive (task tu-workflow-cutover). Pass an explicit (possibly empty)
    sequence to override.
    """
    keep = set(keep)
    unknown = sorted(n for n in keep if n not in tu.by_name)
    if unknown:
        raise EditError(f'unknown functions in keep set: {unknown}')
    kept_blocks = {tu.by_name[n] for n in keep}
    not_c = sorted(b.name for b in kept_blocks if b.state != 'c' or b.role != 'function')
    if not_c:
        raise EditError(f'functions to keep are not original C functions in this TU: {not_c}')
    out, reverted = tu, []
    for b in tu.blocks:
        if b.state == 'c' and b.role == 'function' and b not in kept_blocks:
            out = to_asm(out, b.name, skeleton=skeleton, asm_dir=asm_dir)
            reverted.append(b.name)
    report = dict(kept=sorted(b.name for b in kept_blocks), reverted=reverted, warnings=[])
    keep = {b.name for b in kept_blocks}
    # A reverted static function still referenced by kept C needs a prototype.
    # It can come from the TU's own free items or from the TU's own header, which
    # is where TU-local declarations live; only looking at the free items
    # reported a prototype the file next door already provides.
    if headers is None:
        header = tu_header_path(tu)
        headers = [header] if header else []
    declared = set()
    for item in out.free_items():
        if item.kind == 'decl':
            declared |= {k for kind, k in decl_keys(item.text(out.text)) if kind == 'ordinary'}
    header_sources = []
    for header in headers:
        try:
            text = Path(header).read_text(encoding='utf-8', errors='surrogateescape')
        except OSError:
            continue
        header_sources.append(str(header))
        declared |= declared_names(text)
    if header_sources:
        report['declaration_sources'] = header_sources
    kept_text = ''.join(lexical(out.block_text(out.by_name[n])) for n in keep)
    for name in reverted:
        if tu.by_name[name].static and re.search(r'\b' + re.escape(name) + r'\b', kept_text) \
                and name not in declared:
            report['warnings'].append(dict(kind='missing_prototype', function=name,
                                           detail='reverted static function is referenced by kept C '
                                                  'but has no file-scope declaration'))
    return out, report


# ---------------------------------------------------------------------------
# Declaration keys (prelude collision detection)
# ---------------------------------------------------------------------------

def _norm(text):
    return ' '.join(text.split())


def _norm_block(text):
    return '\n'.join(line.rstrip() for line in text.strip().splitlines())


def decl_keys(text):
    """Names an item declares: {('macro'|'include'|'tag'|'ordinary', name)}.

    Forward declarations (`struct T;`) declare nothing that can conflict.
    """
    keys = set()
    raw = text.strip()
    if raw.startswith('#'):
        m = re.match(r'#\s*define\s+([A-Za-z_]\w*)', raw)
        if m:
            keys.add(('macro', m.group(1)))
        m = re.match(r'#\s*include\s*([<"][^>"]+[>"])', raw)
        if m:
            keys.add(('include', m.group(1)[1:-1]))
        return keys
    lex = lexical(raw)
    if lex.lstrip().startswith(('/*', '//')) or not lex.strip():
        return keys
    lex = _strip_attributes(lex)
    if re.fullmatch(r'\s*(?:struct|union|enum)\s+[A-Za-z_]\w*\s*;\s*', lex):
        return keys
    for m in re.finditer(r'\b(struct|union|enum)\s+([A-Za-z_]\w*)\s*\{', lex):
        keys.add(('tag', m.group(2)))
    # Enumerators are ordinary identifiers.
    for m in re.finditer(r'\benum\b[^{;]*\{([^}]*)\}', lex):
        for part in m.group(1).split(','):
            ident = _IDENT.match(part.strip())
            if ident:
                keys.add(('ordinary', ident.group()))
    # A tag name after struct/union/enum names a tag, never an ordinary
    # identifier. Keep only the keyword, which still counts as a type specifier:
    # `struct S { ... };` then declares no ordinary name (only the tag above),
    # while `struct S *p;` or `struct S { ... } s;` still yield their declarator.
    # Done before brace removal so `struct { ... } obj;` keeps `obj`.
    lex = re.sub(r'\b(struct|union|enum)\s+[A-Za-z_]\w*', r'\1', lex)
    # Remove brace bodies, then read declarators separated by top-level commas.
    flat, depth = [], 0
    for c in lex:
        if c == '{':
            depth += 1
            continue
        if c == '}':
            depth -= 1
            continue
        if depth == 0:
            flat.append(c)
    flat = ''.join(flat).strip().rstrip(';')
    is_typedef = re.match(r'\s*typedef\b', flat) is not None
    parts, depth, begin = [], 0, 0
    for k, c in enumerate(flat):
        if c in '([':
            depth += 1
        elif c in ')]':
            depth -= 1
        elif c == ',' and depth == 0:
            parts.append(flat[begin:k])
            begin = k + 1
    parts.append(flat[begin:])
    for index, part in enumerate(parts):
        part = part.split('=', 1)[0]
        func = declarator_name(part)
        if func and not is_typedef and index == 0 and '(' in part:
            keys.add(('ordinary', func))
            continue
        # Declarator identifier: the last identifier outside brackets/params.
        simple = re.sub(r'\[[^\]]*\]', ' ', part)
        m = re.search(r'\(\s*\*\s*([A-Za-z_]\w*)\s*\)', simple)
        if m:
            keys.add(('ordinary', m.group(1)))
            continue
        simple = re.sub(r'\([^()]*\)', ' ', simple)
        words = [w for w in _IDENT.findall(simple) if w not in _TYPE_WORDS]
        if words and (index > 0 or len(_IDENT.findall(simple)) > 1):
            keys.add(('ordinary', words[-1]))
    return keys


# ---------------------------------------------------------------------------
# Regions for merging
# ---------------------------------------------------------------------------

class Regions:
    """prelude + [(pre, block)] * n + epilogue, aligned to an original name list."""

    def __init__(self, tu, names, label):
        self.tu, self.label = tu, label
        listed = tu.functions()
        got = [b.key for b in listed]
        if len(listed) != len(names) or not all(
                n == b.key or same_function(n, b.name) or same_function(n, b.key)
                for n, b in zip(names, listed)):
            missing = [n for n in names if not any(n == b.key or same_function(n, b.name) for b in listed)]
            raise _Structural(label, missing, got, names)
        text = tu.text
        self.prelude = text[:listed[0].start] if listed else text
        self.pre, self.block, self.state = {}, {}, {}
        prev = listed[0].start if listed else len(text)
        for n, b in zip(names, listed):
            self.pre[n] = text[prev:b.start]
            self.block[n] = text[b.start:b.end]
            self.state[n] = b.state
            prev = b.end
        self.epilogue = text[prev:] if listed else ''
        self.names = list(names)

    def emit(self, prelude=None, pre=None, block=None, epilogue=None):
        prelude = self.prelude if prelude is None else prelude
        pre, block = pre or self.pre, block or self.block
        return prelude + ''.join(pre[n] + block[n] for n in self.names) + (
            self.epilogue if epilogue is None else epilogue)


class _Structural(Exception):
    def __init__(self, label, missing, got, names):
        super().__init__(label)
        self.label, self.missing, self.got, self.names = label, missing, got, names


def prelude_items(text):
    """[(gap, item_text)] plus trailing gap for a prelude text."""
    items = scan(text)
    out, pos = [], 0
    for item in items:
        out.append((text[pos:item.start], text[item.start:item.end]))
        pos = item.end
    return out, text[pos:]


def _emit_prelude(items, tail):
    return ''.join(g + t for g, t in items) + tail


def _is_comment(text):
    return text.lstrip().startswith(('/*', '//'))


def _same_line_comment_gap(text, gap):
    """True when an added prelude comment sits on the line of the item before it.

    scan() makes a trailing comment such as `extern const char D_x[]; /* "s" */`
    its own item with a newline-free whitespace gap. Callers keep that gap
    (instead of forcing a newline) only when they insert the comment directly
    after the item that precedes it in theirs, so the comment stays attached to
    the declaration it annotates.
    """
    return _is_comment(text) and gap.strip() == '' and '\n' not in gap


def _single_item_kind(text):
    items = scan(text)
    if len(items) != 1 or items[0].start != 0 or items[0].end != len(text):
        return None, True
    return items[0].kind, items[0].conditional


def _pure_integer_macro(text):
    """Return the name only for an object-like integer-constant macro."""
    match = re.fullmatch(r'\s*#\s*define\s+([A-Za-z_]\w*)[ \t]+([^\\\n]+)\s*', text)
    if not match:
        return None
    name, expression = match.groups()
    expression = re.sub(r'(?i)(0[xX][0-9a-f]+|0[bB][01]+|[0-9]+)[uUlL]+\b', r'\1', expression)
    try:
        tree = ast.parse(expression, mode='eval')
    except SyntaxError:
        return None
    allowed = (ast.Expression, ast.Constant, ast.UnaryOp, ast.UAdd, ast.USub, ast.Invert,
               ast.BinOp, ast.Add, ast.Sub, ast.Mult, ast.FloorDiv, ast.Mod, ast.LShift,
               ast.RShift, ast.BitOr, ast.BitAnd, ast.BitXor)
    if any(not isinstance(node, allowed) for node in ast.walk(tree)):
        return None
    if any(isinstance(node, ast.Constant) and
           (not isinstance(node.value, int) or isinstance(node.value, bool))
           for node in ast.walk(tree)):
        return None
    return name


def select_dependency_prelude_additions(base_text, provider_text, allowlist, *,
                                        base_sha256=None, provider_sha256=None):
    """Apply only exact allowlisted declarations added by a pinned provider.

    The caller validates the provider's queue submission, TU identity and
    requested function list. Existing prelude items must remain byte-identical
    and in order. Every imported item must be an unconditional declaration
    whose exact text, SHA-256 and declared keys occur in ``allowlist``. Extra
    provider additions are ignored. An existing same-key declaration may be
    deduplicated only when its normalized text is identical; every other
    same-key collision is an error. This opt-in helper does not relax the
    default merge guards.

    Each allowlist row has exactly ``text``, ``sha256`` and ``declared_keys``.
    It returns ``{"text": ..., "manifest": ...}``.
    """
    if base_sha256 is not None and sha256_text(base_text) != base_sha256:
        raise EditError('dependency prelude base SHA-256 does not match its pin')
    if provider_sha256 is not None and sha256_text(provider_text) != provider_sha256:
        raise EditError('dependency provider source SHA-256 does not match its pin')
    try:
        base, provider = TU(base_text, None), TU(provider_text, None)
    except Exception as exc:
        raise EditError(f'cannot parse dependency prelude source: {exc}') from exc
    base_items, base_tail = prelude_items(base.prelude())
    provider_items, _ = prelude_items(provider.prelude())

    # A provider may add items but may not rewrite, remove or reorder existing
    # prelude items under this declaration/constant-macro path.
    cursor = 0
    for _, old_item in base_items:
        match = next((i for i in range(cursor, len(provider_items))
                      if provider_items[i][1] == old_item), None)
        if match is None:
            raise EditError('dependency provider changed, removed or reordered an existing prelude item')
        cursor = match + 1

    base_counts, provider_counts, additions = {}, {}, []
    for _, item in base_items:
        base_counts[item] = base_counts.get(item, 0) + 1
    for gap, item in provider_items:
        provider_counts[item] = provider_counts.get(item, 0) + 1
        if provider_counts[item] > base_counts.get(item, 0):
            kind, conditional = _single_item_kind(item)
            keys = sorted(decl_keys(item))
            if kind == 'decl' and not conditional and keys:
                additions.append((gap, item, keys, 'declaration'))
            elif kind == 'pp' and not conditional and _pure_integer_macro(item):
                additions.append((gap, item, [('macro', _pure_integer_macro(item))], 'constant_macro'))
            else:
                raise EditError('dependency provider additions must be unconditional declarations or pure integer macros')

    if not isinstance(allowlist, list):
        raise EditError('dependency prelude allowlist must be a list')
    allowed = {}
    for row in allowlist:
        if (not isinstance(row, dict) or
                set(row) not in ({'text', 'sha256', 'declared_keys'},
                                 {'text', 'sha256', 'declared_keys', 'kind'})):
            raise EditError('dependency prelude allowlist row has an invalid schema')
        item = row['text']
        keys = sorted(tuple(key) for key in row['declared_keys']
                      if isinstance(key, (list, tuple)) and len(key) == 2)
        if not isinstance(item, str) or sha256_text(item) != row['sha256']:
            raise EditError('dependency prelude allowlist SHA-256 does not match its exact text')
        kind, conditional = _single_item_kind(item)
        row_kind = row.get('kind', 'declaration')
        if row_kind == 'declaration':
            if kind != 'decl' or conditional or not keys or keys != sorted(decl_keys(item)):
                raise EditError('dependency prelude allowlist keys do not match one unconditional declaration')
        elif row_kind == 'constant_macro':
            macro_name = _pure_integer_macro(item)
            if kind != 'pp' or conditional or not macro_name or keys != [('macro', macro_name)]:
                raise EditError('dependency macro allowlist does not name one pure integer object-like macro')
        else:
            raise EditError('dependency prelude allowlist has an unknown item kind')
        if item in allowed:
            raise EditError('dependency prelude allowlist repeats an exact declaration')
        allowed[item] = dict(sha256=row['sha256'], declared_keys=keys)

    addition_counts = {}
    for _, item, _, _ in additions:
        addition_counts[item] = addition_counts.get(item, 0) + 1
    if any(count != 1 for count in addition_counts.values()):
        raise EditError('dependency provider repeats an added prelude declaration')
    if set(allowed) - set(addition_counts):
        raise EditError('an allowlisted declaration is absent from the provider prelude')

    existing = {}
    existing_macros = {}
    for _, item in base_items:
        if _single_item_kind(item)[0] == 'decl':
            for key in decl_keys(item):
                existing.setdefault(key, set()).add(_norm(item))
        macro_name = _pure_integer_macro(item)
        if macro_name:
            existing_macros.setdefault(macro_name, set()).add(_norm(item))
    selected = []
    added_keys = {}
    added_macros = {}
    for gap, item, keys, item_kind in additions:
        if item not in allowed:
            continue
        norm_item = _norm(item)
        if allowed[item]['declared_keys'] != keys:
            raise EditError('provider declaration keys differ from the allowlist')
        if item_kind == 'constant_macro':
            macro_name = keys[0][1]
            prior = existing_macros.get(macro_name, set()) | added_macros.get(macro_name, set())
            if prior and norm_item not in prior:
                raise EditError(f'dependency macro {macro_name} conflicts with an existing same-name macro')
            added_macros.setdefault(macro_name, set()).add(norm_item)
            present = norm_item in existing_macros.get(macro_name, set())
            selected.append(dict(text=item, sha256=allowed[item]['sha256'], declared_keys=keys,
                                 kind=item_kind,
                                 status='already_present' if present else 'added', gap=gap))
            continue
        for key in keys:
            prior = existing.get(tuple(key), set()) | added_keys.get(tuple(key), set())
            if prior and norm_item not in prior:
                raise EditError(f'dependency declaration for {key[1]} conflicts with an existing same-key item')
            added_keys.setdefault(tuple(key), set()).add(norm_item)
        present = all(norm_item in existing.get(tuple(key), set()) for key in keys)
        selected.append(dict(text=item, sha256=allowed[item]['sha256'], declared_keys=keys,
                             kind=item_kind,
                             status='already_present' if present else 'added', gap=gap))

    output_items = list(base_items)
    for row in selected:
        if row['status'] == 'added':
            output_items.append(('\n', row['text']))
            for key in row['declared_keys']:
                existing.setdefault(tuple(key), set()).add(_norm(row['text']))
    prelude = _emit_prelude(output_items, base_tail)
    text = prelude + base_text[base.prelude_end():]
    return dict(text=text,
                manifest=dict(schema='tu-dependency-prelude-selection/1',
                              base_sha256=sha256_text(base_text),
                              provider_sha256=sha256_text(provider_text),
                              additions=[{k: v for k, v in row.items() if k != 'gap'}
                                          for row in selected],
                              result_sha256=sha256_text(text)))


def _pinned_review_artifact(root, relative_path, expected_sha256, label):
    """Read one repository-relative proposal/review/evidence file by its pin."""
    if (not isinstance(relative_path, str) or not relative_path or
            Path(relative_path).is_absolute() or not isinstance(expected_sha256, str) or
            not re.fullmatch(r'[0-9a-f]{64}', expected_sha256)):
        raise EditError(f'reviewed prelude correction has an invalid {label} artifact pin')
    root = Path(root).resolve()
    path = (root / relative_path).resolve()
    try:
        path.relative_to(root)
    except ValueError as exc:
        raise EditError(f'reviewed prelude correction {label} artifact escapes the repository') from exc
    if not path.is_file():
        raise EditError(f'reviewed prelude correction {label} artifact is missing: {relative_path}')
    if hashlib.sha256(path.read_bytes()).hexdigest() != expected_sha256:
        raise EditError(f'reviewed prelude correction {label} artifact differs from its SHA-256 pin')
    return path


def apply_reviewed_prelude_corrections(base_text, proposal, *, expected_tu_id, expected_path,
                                       expected_original_tu_sha256, artifact_root):
    """Apply exact, independently reviewed, same-layout owner prelude corrections.

    This is a pure opt-in transformer. It requires exact owner identity, a
    pinned whole-base source, exact before/after declaration text and hashes,
    distinct proposer/reviewer identities, and existing proposal/review/layout
    artifacts whose bytes match their pins. It changes only the listed unique
    prelude items. Callers must
    still run the normal linked whole-file gate and TU audit; default merge
    behavior remains fail-closed.
    """
    required = {'schema', 'owner_tu', 'owner_path', 'original_tu_sha256', 'base_sha256', 'proposal_path',
                'proposal_sha256', 'proposed_by', 'reviewed_by', 'review_artifact_path',
                'review_artifact_sha256', 'corrections'}
    if not isinstance(proposal, dict) or set(proposal) != required or \
            proposal.get('schema') != 'tu-reviewed-prelude-corrections/1':
        raise EditError('reviewed prelude correction proposal has an invalid schema')
    if proposal['owner_tu'] != expected_tu_id or proposal['owner_path'] != expected_path:
        raise EditError('reviewed prelude correction owner identity does not match the target TU')
    if proposal['original_tu_sha256'] != expected_original_tu_sha256 or \
            not re.fullmatch(r'[0-9a-f]{64}', str(proposal['original_tu_sha256'])):
        raise EditError('reviewed prelude correction original TU byte identity does not match the target')
    if sha256_text(base_text) != proposal['base_sha256']:
        raise EditError('reviewed prelude correction base SHA-256 is stale')
    for field in ('proposal_path', 'review_artifact_path'):
        if not isinstance(proposal[field], str) or not proposal[field] or Path(proposal[field]).is_absolute():
            raise EditError(f'reviewed prelude correction has no repository-relative {field}')
    for field in ('proposal_sha256', 'review_artifact_sha256'):
        if not isinstance(proposal[field], str) or not re.fullmatch(r'[0-9a-f]{64}', proposal[field]):
            raise EditError(f'reviewed prelude correction has no valid {field} pin')
    if not proposal['proposed_by'] or not proposal['reviewed_by'] or \
            proposal['proposed_by'] == proposal['reviewed_by']:
        raise EditError('owner declaration correction requires an independent reviewer')
    proposal_file = _pinned_review_artifact(artifact_root, proposal['proposal_path'],
                                            proposal['proposal_sha256'], 'proposal')
    _pinned_review_artifact(artifact_root, proposal['review_artifact_path'],
                            proposal['review_artifact_sha256'], 'review')
    proposal_text = proposal_file.read_text(encoding='utf-8', errors='surrogateescape')
    if not isinstance(proposal['corrections'], list) or not proposal['corrections']:
        raise EditError('reviewed prelude correction has no declaration substitutions')
    try:
        base = TU(base_text, None)
    except Exception as exc:
        raise EditError(f'cannot parse reviewed prelude correction base: {exc}') from exc
    items, tail = prelude_items(base.prelude())
    replacements, seen_before, seen_keys = [], set(), set()
    for row in proposal['corrections']:
        fields = {'before_text', 'before_sha256', 'after_text', 'after_sha256',
                  'declared_keys', 'before_size', 'after_size', 'layout_evidence_path',
                  'layout_evidence_sha256'}
        if not isinstance(row, dict) or set(row) != fields:
            raise EditError('reviewed prelude correction row has an invalid schema')
        before, after = row['before_text'], row['after_text']
        if not isinstance(before, str) or not isinstance(after, str) or before == after:
            raise EditError('reviewed prelude correction needs distinct exact declaration texts')
        if sha256_text(before) != row['before_sha256'] or sha256_text(after) != row['after_sha256']:
            raise EditError('reviewed prelude correction declaration hash does not match its exact text')
        if not isinstance(row['before_size'], int) or not isinstance(row['after_size'], int) or \
                row['before_size'] <= 0 or row['before_size'] != row['after_size']:
            raise EditError('owner declaration correction is not proven same-size by its review record')
        evidence = row['layout_evidence_sha256']
        if not isinstance(row['layout_evidence_path'], str) or not row['layout_evidence_path'] or \
                Path(row['layout_evidence_path']).is_absolute() or not isinstance(evidence, str) or \
                not re.fullmatch(r'[0-9a-f]{64}', evidence):
            raise EditError('owner declaration correction lacks a pinned layout-evidence artifact')
        _pinned_review_artifact(artifact_root, row['layout_evidence_path'], evidence, 'layout-evidence')
        if proposal_text.count(after) != 1:
            raise EditError('proposed declaration is not present exactly once in its pinned proposal artifact')
        keys_before, keys_after = sorted(decl_keys(before)), sorted(decl_keys(after))
        keys = sorted(tuple(key) for key in row['declared_keys']
                      if isinstance(key, (list, tuple)) and len(key) == 2)
        if not keys_before or keys_before != keys_after or keys_before != keys:
            raise EditError('owner declaration correction must preserve its exact declaration keys')
        if seen_keys.intersection(keys):
            raise EditError('reviewed prelude correction rows overlap declaration keys')
        if before in seen_before:
            raise EditError('reviewed prelude correction repeats a before declaration')
        seen_keys.update(keys)
        seen_before.add(before)
        kind_before, conditional_before = _single_item_kind(before)
        kind_after, conditional_after = _single_item_kind(after)
        if kind_before != 'decl' or kind_after != 'decl' or conditional_before or conditional_after:
            raise EditError('reviewed owner correction must replace unconditional declaration items')
        matches = [i for i, (_, item) in enumerate(items) if item == before]
        if len(matches) != 1:
            raise EditError('reviewed owner correction before text must occur once in the current prelude')
        replacements.append((matches[0], after))
    for index, item in replacements:
        items[index] = (items[index][0], item)
    prelude = _emit_prelude(items, tail)
    text = prelude + base_text[base.prelude_end():]
    try:
        corrected = TU(text, None)
    except Exception as exc:
        raise EditError(f'reviewed owner prelude correction broke TU structure: {exc}') from exc
    corrected_items, _ = prelude_items(corrected.prelude())
    for row in proposal['corrections']:
        for key in row['declared_keys']:
            key = tuple(key)
            matches = [_norm(item) for _, item in corrected_items
                       if _single_item_kind(item)[0] == 'decl' and key in decl_keys(item)]
            if len(matches) != 1 or matches[0] != _norm(row['after_text']):
                raise EditError(f'reviewed owner correction leaves a duplicate or conflicting declaration for {key[1]}')
    return dict(text=text,
                manifest=dict(schema='tu-reviewed-prelude-corrections-applied/1',
                              owner_tu=expected_tu_id, owner_path=expected_path,
                              original_tu_sha256=proposal['original_tu_sha256'],
                              base_sha256=sha256_text(base_text),
                              proposal_path=proposal['proposal_path'],
                              proposal_sha256=proposal['proposal_sha256'],
                              review_artifact_path=proposal['review_artifact_path'],
                              review_artifact_sha256=proposal['review_artifact_sha256'],
                              artifacts_verified=True,
                              corrections=[dict(before_sha256=row['before_sha256'],
                                                after_sha256=row['after_sha256'],
                                                declared_keys=row['declared_keys'],
                                                layout_size=row['before_size'],
                                                layout_evidence_path=row['layout_evidence_path'],
                                                layout_evidence_sha256=row['layout_evidence_sha256'])
                                           for row in proposal['corrections']],
                              result_sha256=sha256_text(text)))


def apply_reviewed_owner_proposals(repo_root, manifest_path, project_root, target_tu):
    """Stage only independently reviewed owner-type proposals into a private root.

    The manifest is an explicit opt-in launch artifact.  It pins every proposal,
    reviewer report, evidence manifest, and any structured original-byte
    witnesses.  A proposal may change only its named complete typedef; the old
    and proposed files must be byte-identical everywhere else.  The ordinary
    worker baseline gate and TU audit remain responsible for code/data exactness.
    """
    repo_root = Path(repo_root).resolve()
    project_root = Path(project_root).resolve()
    manifest_path = Path(manifest_path)
    if manifest_path.is_absolute():
        manifest_path = manifest_path.resolve()
        try:
            manifest_rel = manifest_path.relative_to(repo_root).as_posix()
        except ValueError as exc:
            raise EditError('owner proposal manifest escapes the repository') from exc
    else:
        manifest_rel = manifest_path.as_posix()
        if '..' in manifest_path.parts or not manifest_rel:
            raise EditError('owner proposal manifest is not repository-relative')
        manifest_path = (repo_root / manifest_path).resolve()
        try:
            manifest_path.relative_to(repo_root)
        except ValueError as exc:
            raise EditError('owner proposal manifest escapes the repository') from exc
        if not manifest_path.is_file():
            raise EditError('owner proposal manifest is missing')
    manifest_sha = hashlib.sha256(manifest_path.read_bytes()).hexdigest()
    try:
        manifest = json.loads(manifest_path.read_text(encoding='utf-8'))
    except (OSError, ValueError) as exc:
        raise EditError(f'cannot read owner proposal manifest: {exc}') from exc
    if (not isinstance(manifest, dict) or set(manifest) - {'schema', 'proposals', 'header_exports'} or
            not {'schema', 'proposals'} <= set(manifest) or
            manifest.get('schema') != 'tu-reviewed-owner-proposals/1' or
            not isinstance(manifest.get('proposals'), list) or not manifest['proposals']):
        raise EditError('owner proposal manifest has an invalid schema')

    applied, seen, reviewed_rows = [], set(), {}
    row_keys = {'id', 'owner_tu', 'applies_to', 'owner_path', 'owner_type', 'field', 'offset',
                'before_member', 'after_member', 'layout_bytes', 'base_sha256',
                'proposal_path', 'proposal_sha256', 'review_path', 'review_sha256',
                'review_owner', 'evidence_manifest_path', 'evidence_manifest_sha256',
                'evidence'}
    for row in manifest['proposals']:
        if not isinstance(row, dict) or set(row) != row_keys:
            raise EditError('owner proposal manifest row has an invalid schema')
        # Entries for other TUs are allowed in a grouped manifest and are still
        # artifact-checked below; duplicate ids are always refused.
        if (not isinstance(row['id'], str) or not row['id'] or row['id'] in seen or
                not isinstance(row['applies_to'], list) or
                not row['applies_to'] or any(not isinstance(x, str) for x in row['applies_to'])):
            raise EditError('owner proposal manifest has a duplicate or invalid identity')
        seen.add(row['id'])
        review_file = _pinned_review_artifact(repo_root, row['review_path'],
                                              row['review_sha256'], 'owner-layout-review')
        proposal_file = _pinned_review_artifact(repo_root, row['proposal_path'],
                                                row['proposal_sha256'], 'owner-proposal')
        evidence_manifest = _pinned_review_artifact(
            repo_root, row['evidence_manifest_path'], row['evidence_manifest_sha256'],
            'owner-layout-evidence-manifest')
        evidence_rows = row['evidence']
        if not isinstance(evidence_rows, list):
            raise EditError('owner proposal evidence list is malformed')
        for evidence in evidence_rows:
            if not isinstance(evidence, dict) or set(evidence) != {'path', 'sha256'}:
                raise EditError('owner proposal evidence row has an invalid schema')
            _pinned_review_artifact(repo_root, evidence['path'], evidence['sha256'],
                                    'owner-layout-evidence')

        try:
            review = json.loads(review_file.read_text(encoding='utf-8'))
        except (OSError, ValueError) as exc:
            raise EditError(f'cannot parse owner-layout review artifact: {exc}') from exc
        reviewed_rows[row['id']] = dict(review=review, path=row['review_path'],
                                        sha256=row['review_sha256'])
        review_schema = review.get('schema')
        if review_schema not in ('owner-proposal-review/1', 'private-tu-owner-proposal-review/1'):
            raise EditError('owner-layout review artifact has an unexpected schema')
        if review.get('reviewer') == row['review_owner']:
            raise EditError('owner proposal reviewer must be independent of its author')
        if review_schema == 'owner-proposal-review/1':
            decisions = [d for d in review.get('decisions') or []
                         if d.get('owner') == row['owner_tu'] + ' ' + row['owner_type'] and
                         d.get('field') == row['field'] and d.get('offset') == row['offset'] and
                         d.get('verdict') == 'evidence_supported']
            if len(decisions) != 1:
                raise EditError(f'{row["id"]}: independent review does not approve this exact owner field')
            artifact_pins = review.get('proposal_artifacts') or {}
            pinned_pairs = {(item.get('path'), item.get('sha256'))
                            for item in artifact_pins.values() if isinstance(item, dict)}
            if (row['proposal_path'], row['proposal_sha256']) not in pinned_pairs or \
                    (row['evidence_manifest_path'], row['evidence_manifest_sha256']) not in pinned_pairs:
                raise EditError(f'{row["id"]}: reviewer report does not pin the proposal and evidence manifest')
        else:
            # Current owner-proposal review packets predate the generic review
            # schema. Accept them only when the immutable report names the
            # exact owner base, proposed file, evidence manifest, proposal id,
            # and field/offset under review. This is still layout evidence;
            # it does not replace the normal TU gate or audit.
            identities = review.get('identities') or {}
            expected_identities = {
                'owner_base': (row['owner_path'], row['base_sha256']),
                'proposal': (row['proposal_path'], row['proposal_sha256']),
                'evidence': (row['evidence_manifest_path'], row['evidence_manifest_sha256']),
            }
            for label, (path, digest) in expected_identities.items():
                actual = identities.get(label) or {}
                if (actual.get('path'), actual.get('sha256')) != (path, digest):
                    raise EditError(f'{row["id"]}: review does not pin the exact {label} artifact')
            offset_review = review.get('base_and_offset_review') or {}
            assembly_review = review.get('assembly_evidence') or {}
            review_text = json.dumps(review, sort_keys=True)
            if (review.get('proposal_id') != row['id'] or
                    offset_review.get('current_base_sha256') != row['base_sha256'] or
                    offset_review.get('proposal_sha256') != row['proposal_sha256'] or
                    row['field'] not in review_text or row['offset'] not in review_text or
                    not assembly_review or not review.get('neighbor_assertions')):
                raise EditError(f'{row["id"]}: review packet does not approve the exact field and offset')
        for witness in review.get('additional_pinned_evidence') or []:
            if not isinstance(witness, dict) or set(witness) < {'path', 'sha256'}:
                raise EditError(f'{row["id"]}: malformed reviewer-pinned layout witness')
            _pinned_review_artifact(repo_root, witness['path'], witness['sha256'],
                                    'reviewer-layout-witness')

        owner_rel = Path(row['owner_path'])
        if owner_rel.is_absolute() or '..' in owner_rel.parts or not owner_rel.parts:
            raise EditError('owner proposal path is not a safe repository-relative path')
        base_file = _pinned_review_artifact(repo_root, owner_rel.as_posix(),
                                            row['base_sha256'], 'owner-base-source')
        base_text = base_file.read_text(encoding='utf-8', errors='surrogateescape')
        proposed_text = proposal_file.read_text(encoding='utf-8', errors='surrogateescape')
        name = re.escape(row['owner_type'])
        block_re = re.compile(r'(?ms)^typedef\s+struct\s+' + name +
                              r'\s*\{.*?^\}\s*' + name + r'\s*;')
        before_blocks, after_blocks = list(block_re.finditer(base_text)), list(block_re.finditer(proposed_text))
        if len(before_blocks) != 1 or len(after_blocks) != 1:
            raise EditError(f'{row["id"]}: owner typedef is not unique in both pinned files')
        before_block, after_block = before_blocks[0], after_blocks[0]
        outside_type_changed = (
            base_text[:before_block.start()] != proposed_text[:after_block.start()] or
            base_text[before_block.end():] != proposed_text[after_block.end():])
        if outside_type_changed:
            raise EditError(f'{row["id"]}: proposal changes bytes outside its one reviewed owner typedef')
        before_type, after_type = before_block.group(0), after_block.group(0)
        if not isinstance(row['layout_bytes'], int) or row['layout_bytes'] <= 0:
            raise EditError(f'{row["id"]}: owner layout byte width is invalid')
        before_lines = [line.strip() for line in before_type.splitlines()
                        if row['before_member'] in line]
        after_lines = [line.strip() for line in after_type.splitlines()
                       if row['after_member'] in line]
        if len(before_lines) != 1 or len(after_lines) != 1:
            raise EditError(f'{row["id"]}: exact before/after owner members are not unique')
        if (row['before_member'] not in before_type or row['after_member'] not in after_type):
            raise EditError(f'{row["id"]}: proposed member text does not match reviewed offset evidence')
        if target_tu not in row['applies_to']:
            continue
        target = (project_root / owner_rel).resolve()
        try:
            target.relative_to(project_root)
        except ValueError as exc:
            raise EditError('owner proposal destination escapes the private project') from exc
        if not target.is_file():
            raise EditError(f'{row["id"]}: private owner file is missing: {owner_rel.as_posix()}')
        current_sha = hashlib.sha256(target.read_bytes()).hexdigest()
        proposal_sha = hashlib.sha256(proposal_file.read_bytes()).hexdigest()
        current_text = target.read_text(encoding='utf-8', errors='surrogateescape')
        if current_sha == row['base_sha256']:
            # A consumer TU's private mirror carries the defining TU source as
            # a read-only copy. The reviewed owner edit is still confined to
            # this attempt's src tree, so unlock only that copy before writing.
            _guard_canonical_write(target)
            target.chmod(0o644)
            target.write_text(proposed_text, encoding='utf-8', errors='surrogateescape')
            current_text = proposed_text
        elif current_sha != proposal_sha:
            # An owner TU may already contain worker function edits after init.
            # Keep those edits only when its complete owner typedef still equals
            # the independently reviewed proposal byte-for-byte.
            current_blocks = list(block_re.finditer(current_text))
            if len(current_blocks) != 1 or current_blocks[0].group(0) != after_type:
                raise EditError(f'{row["id"]}: private owner file is neither the pinned base/proposal nor '
                                'a candidate retaining the exact reviewed owner typedef')
        staged_hash = hashlib.sha256(current_text.encode('utf-8', errors='surrogateescape')).hexdigest()
        applied.append(dict(id=row['id'], owner_tu=row['owner_tu'], owner_path=owner_rel.as_posix(),
                            proposal_path=row['proposal_path'], proposal_sha256=proposal_sha,
                            review_path=row['review_path'], review_sha256=row['review_sha256'],
                            evidence_manifest_path=row['evidence_manifest_path'],
                            evidence_manifest_sha256=row['evidence_manifest_sha256'],
                            owner_type=row['owner_type'], field=row['field'], offset=row['offset'],
                            source_sha256=staged_hash))
    exported = []
    private_reconciliations = []
    for export in manifest.get('header_exports', []):
        if (not isinstance(export, dict) or set(export) !=
                {'owner_proposal_id', 'applies_to', 'header_path', 'types', 'declarations'} or
                not isinstance(export['applies_to'], list) or
                not isinstance(export['types'], list) or not export['types'] or
                not isinstance(export['declarations'], list)):
            raise EditError('owner header-export entry has an invalid schema')
        if target_tu not in export['applies_to']:
            continue
        row = next((item for item in applied if item['id'] == export['owner_proposal_id']), None)
        if row is None:
            raise EditError('owner header export does not name an applied reviewed proposal')
        header_rel = Path(export['header_path'])
        if (header_rel.is_absolute() or '..' in header_rel.parts or
                not header_rel.parts or not str(header_rel).startswith('include/')):
            raise EditError('owner header-export destination is not a private include path')
        owner_source = project_root / row['owner_path']
        header = project_root / header_rel
        if not owner_source.is_file() or not header.is_file():
            raise EditError('owner header-export source or generated destination is missing')
        owner_text = owner_source.read_text(encoding='utf-8', errors='surrogateescape')
        header_text = header.read_text(encoding='utf-8', errors='surrogateescape')
        snippets = []
        for name in export['types']:
            if not isinstance(name, str) or not re.fullmatch(r'[A-Za-z_]\w*', name):
                raise EditError('owner header-export type name is invalid')
            pattern = re.compile(r'(?ms)^typedef\s+(?:struct|union)\s+' + re.escape(name) +
                                r'\s*\{.*?^\}\s*' + re.escape(name) + r'\s*;')
            matches = list(pattern.finditer(owner_text))
            if len(matches) != 1:
                raise EditError(f'{name}: owner source does not contain one complete tagged typedef')
            snippet = matches[0].group(0)
            if name in header_text:
                if snippet not in header_text:
                    raise EditError(f'{name}: generated header already has a different declaration')
                continue
            snippets.append(snippet)
        declarations = []
        for declaration in export['declarations']:
            if (not isinstance(declaration, str) or not declaration.endswith(';') or
                    '\n' in declaration or owner_text.count(declaration) != 1):
                raise EditError('owner header-export declaration is not one exact source-owned declaration')
            if declaration in header_text:
                continue
            declarations.append(declaration)
        if snippets:
            endif = re.search(r'(?m)^#endif\b[^\n]*$', header_text)
            if not endif:
                raise EditError('generated owner header has no closing include guard')
        if snippets or declarations:
            endif = re.search(r'(?m)^#endif\b[^\n]*$', header_text)
            if not endif:
                raise EditError('generated owner header has no closing include guard')
            exported_text = snippets + declarations
            block = ('\n/* Reviewed private owner declarations staged by worker init. */\n' +
                     '\n\n'.join(exported_text) + '\n\n')
            header_text = header_text[:endif.start()] + block + header_text[endif.start():]
            # `worker.py init` deliberately stages published headers read-only.
            # This is a private generated-header overlay, so make only this
            # sandbox copy writable before applying the reviewed export.
            _guard_canonical_write(header)
            header.chmod(0o644)
            header.write_text(header_text, encoding='utf-8', errors='surrogateescape')
        # MAIN's compile edges include the copied build/main/include tree.
        # Staging only project/include is insufficient: init generates the
        # build graph immediately afterwards and compilers include from that
        # private build tree. Keep this exact reviewed overlay in sync there.
        build_header = project_root / 'build' / 'main' / 'include' / Path(*header_rel.parts[1:])
        build_header_sha = None
        if build_header.is_file():
            _guard_canonical_write(build_header)
            build_header.chmod(0o644)
            build_header.write_bytes(header.read_bytes())
            build_header_sha = hashlib.sha256(build_header.read_bytes()).hexdigest()
        exported.append(dict(owner_proposal_id=row['id'], header_path=header_rel.as_posix(),
                             types=list(export['types']), declarations=list(export['declarations']),
                             header_sha256=hashlib.sha256(header.read_bytes()).hexdigest(),
                             build_header_sha256=build_header_sha))
        # TU114's private compatibility header still carries a one-word
        # RssdWorkArea declaration from before tu110's reviewed full owner
        # view was harvested. Once that exact owner type and symbol are
        # exported, remove only this conflicting private duplicate so the
        # complete MAIN baseline compiles against one declaration of RssdWork.
        if ('RssdWorkFlags' in export['types'] and
                'extern RssdWorkFlags RssdWork;' in export['declarations']):
            legacy_header = project_root / 'src/main/ssd_3.h'
            owner_header = project_root / header_rel
            reviewed_owner = reviewed_rows.get(row['id']) or {}
            review = reviewed_owner.get('review') or {}
            partial_identity = (review.get('identities') or {}).get('current_tu114_partial_header') or {}
            if (review.get('schema') != 'private-tu-owner-proposal-review/1' or
                    partial_identity.get('path') != 'src/main/ssd_3.h' or
                    not legacy_header.is_file()):
                raise EditError('TU114 private RssdWork reconciliation lacks an exact reviewed source-header pin')
            # The owner-layout proposal is staged at init and revalidated again
            # at result/review time. Accept exactly the pinned preimage or the
            # deterministic postimage of this one reviewed declaration change;
            # a third byte state is never treated as an already-applied edit.
            # Derive the postimage from the pinned canonical preimage so the
            # receipt remains verifiable after the private init side effect.
            original_header = repo_root / partial_identity['path']
            if (not original_header.is_file() or
                    hashlib.sha256(original_header.read_bytes()).hexdigest() !=
                    partial_identity.get('sha256')):
                raise EditError('TU114 reviewed source-header preimage is stale')
            original_text = original_header.read_text(encoding='utf-8', errors='surrogateescape')
            legacy_block = re.compile(
                r'(?ms)^/\*\n \* RssdWork is the RSSD RPC/background-wave work area, main:0x004aa080\n'
                r'.*?^extern RssdWorkArea RssdWork;\s*')
            original_matches = list(legacy_block.finditer(original_text))
            if len(original_matches) != 1:
                raise EditError('TU114 pinned source header has no unique reviewed partial-view declaration')
            original_match = original_matches[0]
            replacement = '/* RssdWork uses the reviewed shared owner layout from main/tu110. */\n'
            expected_after_text = (original_text[:original_match.start()] + replacement +
                                   original_text[original_match.end():])
            before_sha256 = hashlib.sha256(original_text.encode(
                'utf-8', errors='surrogateescape')).hexdigest()
            after_sha256 = hashlib.sha256(expected_after_text.encode(
                'utf-8', errors='surrogateescape')).hexdigest()
            tu114_basis = (review.get('neighbor_assertions') or {}).get('tu114')
            if (not isinstance(tu114_basis, str) or 'RssdWorkArea' not in tu114_basis or
                    'flags' not in tu114_basis or '+0' not in tu114_basis):
                raise EditError('TU114 private reconciliation lacks reviewed flags-at-zero neighbor evidence')
            owner_text = owner_header.read_text(encoding='utf-8', errors='surrogateescape')
            owner_type = re.search(r'(?ms)^typedef\s+struct\s+RssdWorkFlags\s*\{.*?^\}\s*RssdWorkFlags\s*;',
                                   owner_text)
            if not owner_type or 'extern RssdWorkFlags RssdWork;' not in owner_text:
                raise EditError('RssdWork owner export is missing its exact reviewed type or symbol declaration')
            legacy_bytes = legacy_header.read_bytes()
            current_sha256 = hashlib.sha256(legacy_bytes).hexdigest()
            if current_sha256 == before_sha256:
                _guard_canonical_write(legacy_header)
                legacy_header.chmod(0o644)
                legacy_header.write_text(expected_after_text, encoding='utf-8', errors='surrogateescape')
                current_sha256 = after_sha256
            elif current_sha256 != after_sha256 or legacy_bytes != expected_after_text.encode(
                    'utf-8', errors='surrogateescape'):
                raise EditError('private main/ssd_3.h matches neither the pinned before nor reviewed after bytes')
            private_reconciliations.append(dict(
                schema='tu-reviewed-private-header-reconciliation/1',
                id='main-tu114-rssdwork-partial-view-v1',
                path='src/main/ssd_3.h',
                operation='replace-one-reviewed-RssdWorkArea-partial-view-with-owner-reference-v1',
                status='applied-or-verified',
                review_path=reviewed_owner.get('path'),
                review_sha256=reviewed_owner.get('sha256'),
                evidence_before_sha256=partial_identity.get('sha256'),
                before_sha256=before_sha256,
                after_sha256=after_sha256,
                operation_match_count=1,
                owner_type='RssdWorkFlags',
                owner_symbol='extern RssdWorkFlags RssdWork;',
                preserved_neighbor='RssdWorkArea.flags remains at +0',
                current_sha256=current_sha256))
    return dict(schema='tu-owner-proposal-stage/1', manifest_path=manifest_rel,
                manifest_sha256=manifest_sha, target_tu=target_tu,
                applied=applied, header_exports=exported,
                private_reconciliations=private_reconciliations)


# The banner tools/tu/gen_ovl_src.py writes at the top of an overlay skeleton
# (build/<unit>/scaffold/src/<unit>/<tu>.c) for a TU with no published source.
# It is generated text, not authored source, and its presence is not
# deterministic: gen_ovl_src.py `--scaffold-out` writes it only when
# src/<unit>/<tu>.c does not exist, and otherwise leaves splat's banner-less
# skeleton in place. So a private configure run after a candidate was staged at
# src/<unit>/<tu>.c regenerates the skeleton without it, and a worker may drop
# or rewrite it. Only this exact generated shape is matched: the one-line form
# gen_ovl_src.py writes since 2026-09-19 (user direction), which published TU
# files also carry, and the older long form with name/accepted/provenance lines.
_GENERATED_BANNER = re.compile(
    r'/\*\n \* (?:MAIN|OV\d\d) original TU \d+: 0x[0-9a-f]+\.\.0x[0-9a-f]+ \(\d+ functions\)\n'
    r'(?: \* Name: [^\n]*\n \* Accepted C: [^\n]*\n \* Accepted standalone EE assembly: [^\n]*\n'
    r' \* Generated by tools/tu/gen_ovl_src\.py from the accepted\n'
    r' \* unit sources \(function text verbatim\) and splat INCLUDE_ASM bodies\.\n'
    r'(?: \* Header divergence [^\n]*\n)*)? \*/')


def is_generated_banner(text):
    """True for exactly the gen_ovl_src.py skeleton banner comment."""
    return _GENERATED_BANNER.fullmatch(text.strip()) is not None


def strip_generated_banner(text):
    """`text` without a leading gen_ovl_src.py banner (and the newline after it).

    Any other text, including any other leading comment, is returned unchanged,
    so comparing two stripped files still fails on every other difference.
    """
    m = _GENERATED_BANNER.match(text)
    if not m:
        return text
    end = m.end()
    return text[end + 1:] if text[end:end + 1] == '\n' else text[end:]


def _comment_variants(b_items, t_items):
    """Positional pairing of the prelude items theirs changed only in comments.

    Returns (base indices, theirs indices, reworded theirs indices). A base item
    and a theirs item pair when they sit in the same replaced run of the item
    sequences and are the same code (comments and whitespace aside), so a
    duplicated line elsewhere (a second `#endif`) is never mistaken for one. A
    comment-only theirs item in a replaced run that also drops a base comment
    is that comment reworded (one theirs comment per dropped base comment, in
    order, so a comment written for a new declaration in the same run stays an
    addition).
    """
    b_norm, t_norm = [_norm(t) for _, t in b_items], [_norm(t) for _, t in t_items]
    # A generated banner is not authored text: its replacement is merged as an
    # ordinary addition (merge_prelude), never read as a reworded comment.
    b_code = [None if is_generated_banner(t) else code_key(t) for _, t in b_items]
    t_code = [code_key(t) for _, t in t_items]
    base_ix, theirs_ix, reworded = set(), set(), set()
    matcher = difflib.SequenceMatcher(None, b_norm, t_norm, autojunk=False)
    for tag, i1, i2, j1, j2 in matcher.get_opcodes():
        if tag != 'replace':
            continue
        free = [i for i in range(i1, i2) if b_code[i]]
        for j in range(j1, j2):
            if not t_code[j]:
                continue
            hit = next((i for i in free if b_code[i] == t_code[j]), None)
            if hit is not None:
                free.remove(hit)
                base_ix.add(hit)
                theirs_ix.add(j)
        # each dropped base comment accounts for at most one theirs comment of the run
        dropped = sum(1 for i in range(i1, i2) if b_code[i] == ())
        reworded.update([j for j in range(j1, j2) if t_code[j] == ()][:dropped])
    return base_ix, theirs_ix, reworded


# ---------------------------------------------------------------------------
# Struct member sizes (shared with tools/review_stage.py and header_types.py)
# ---------------------------------------------------------------------------
# AGENTS.md lets a worker replace an `unmodeled_XX[N]` span by named members of
# the same total size. These helpers compute that size from the text alone:
# plain scalar/pointer/array members only. A member whose type is not listed
# (a typedef'd struct, a bitfield) has no provable size; callers treat that as
# "unknown", never as a pass.

MEMBER_SIZES = {'char': 1, 'signed char': 1, 'unsigned char': 1, 'u8': 1, 's8': 1,
                'short': 2, 'signed short': 2, 'unsigned short': 2, 'u16': 2, 's16': 2,
                'short int': 2, 'unsigned short int': 2,
                'int': 4, 'signed int': 4, 'unsigned int': 4, 'unsigned': 4, 'signed': 4,
                'long': 4, 'unsigned long': 4, 'u32': 4, 's32': 4, 'float': 4,
                'long long': 8, 'unsigned long long': 8, 'u64': 8, 's64': 8, 'double': 8}
# `_unmodeled_0a0` (leading underscore) is the same span spelling as `unmodeled_0a0`
UNMODELED_MEMBER = re.compile(r'(?<![A-Za-z0-9])_?unmodell?ed_\w*')


def member_dim(expr):
    """Value of an array bound written as integer arithmetic (`0x9A0 - 0x6F8`), or None."""
    expr = expr.strip()
    if not expr or not re.fullmatch(r'[\s0-9a-fA-FxX+\-*()]+', expr):
        return None
    try:
        value = eval(re.sub(r'\b0[xX][0-9a-fA-F]+\b|\b\d+\b', lambda m: str(int(m.group(0), 0)), expr),
                     {'__builtins__': {}}, {})
    except Exception:
        return None
    return value if isinstance(value, int) and value >= 0 else None


def member_layout(decl):
    """(size, alignment) of one simple member declaration `TYPE name[N][M]` (no `;`), or None.

    Alignment is the natural one of the element type (the EE ABI aligns every
    listed scalar to its size and a pointer to 4).
    """
    decl = ' '.join(decl.split())
    dims = []
    m = re.fullmatch(r'[A-Za-z_][\w\s]*\(\s*\*\s*[A-Za-z_]\w*\s*((?:\[[^\]]+\]\s*)*)\)\s*\(.*\)', decl)
    if m:                                  # function pointer member
        size, dims = 4, re.findall(r'\[([^\]]+)\]', m.group(1))
    else:
        m = re.fullmatch(r'((?:(?:const|volatile)\s+)*[A-Za-z_][\w\s]*?)\s*(\*+)?\s*'
                         r'([A-Za-z_]\w*)\s*((?:\[[^\]]+\]\s*)*)', decl)
        if not m:
            return None
        base, stars, _, dim_text = m.groups()
        base = ' '.join(w for w in base.split() if w not in ('const', 'volatile', 'struct'))
        size = 4 if stars else MEMBER_SIZES.get(base)
        dims = re.findall(r'\[([^\]]+)\]', dim_text or '')
    if size is None:
        return None
    align = size
    for dim in dims:
        value = member_dim(dim)
        if value is None:
            return None
        size *= value
    return size, align


def member_size(decl):
    """Byte size of one simple member declaration, or None (see member_layout)."""
    layout = member_layout(decl)
    return None if layout is None else layout[0]


def members_size(members):
    """Total byte size of member declarations (no padding), or None when one is unknown."""
    total = 0
    for member in members:
        size = member_size(member)
        if size is None:
            return None
        total += size
    return total


def strip_member_comments(text):
    """`text` with its comments replaced by spaces, tolerating a cut comment.

    The text may be a run of lines cut out of a larger body (a diff hunk): a
    block comment may then open on its last line and close on a line outside
    the run, or close on its first line after opening outside it. Both halves
    are comment text: an unterminated trailing `/* ...` and a leading
    `... */` with no opening before it are removed as well.
    """
    first_close, first_open = text.find('*/'), text.find('/*')
    if first_close >= 0 and (first_open < 0 or first_close < first_open):
        text = ' ' * (first_close + 2) + text[first_close + 2:]
    text = re.sub(r'/\*.*?\*/|//[^\n]*', ' ', text, flags=re.S)
    cut = text.find('/*')
    return text if cut < 0 else text[:cut]


def body_members(text):
    """Member declarations of a run of struct-body lines, or None when it holds anything else."""
    code = strip_member_comments(text)
    if re.search(r'[{}#]', code):
        return None
    pieces = [p.strip() for p in code.split(';')]
    if pieces and pieces[-1]:
        return None                        # text after the last `;`
    return [p for p in pieces if p]


def _struct_parts(text):
    """(head, [members], tail) of a single-body struct/union item, comments ignored, or None."""
    code = lexical(text)
    if '#' in code or code.count('{') != 1 or code.count('}') != 1:
        return None
    start, stop = code.index('{'), code.index('}')
    if stop < start or not re.search(r'\bstruct\b', code[:start]):
        return None
    members = body_members(code[start + 1:stop])
    if not members:
        return None
    return _norm(code[:start]), [_norm(m) for m in members], _norm(code[stop + 1:])


def _unmodeled_name(member):
    words = _IDENT.findall(re.sub(r'\[[^\]]*\]', ' ', member))
    return bool(words) and UNMODELED_MEMBER.fullmatch(words[-1]) is not None


def unmodeled_split(base_text, theirs_text):
    """Why `theirs_text` is not a same-size `unmodeled_` split of `base_text` (a string), or its spans.

    The two prelude items must be one struct definition (a single brace body,
    no nested struct/union body, no directive) with the same text before and
    after the body and the same members, except runs of base members that are
    all `unmodeled_*` and are replaced by members of exactly the same total
    byte size (member_size). Comments are ignored. When the offset of a run is
    computable from the members before it, both the run and its replacement are
    laid out from there with natural alignment and must end at the same offset,
    so no later member moves; the struct's alignment must not grow. Where an
    offset or the struct alignment is not computable (a member of unlisted
    type), `alignment_checked` is False and the whole-file gate is the proof.
    Returns a list of span dicts on success.
    """
    b, t = _struct_parts(base_text), _struct_parts(theirs_text)
    if b is None or t is None:
        return 'not a single-body struct definition with plain member declarations'
    if b[0] != t[0] or b[2] != t[2]:
        return 'the text around the struct body changed'
    b_members, t_members = b[1], t[1]
    b_layout = [member_layout(m) for m in b_members]
    t_layout = [member_layout(m) for m in t_members]
    spans = []
    matcher = difflib.SequenceMatcher(None, b_members, t_members, autojunk=False)
    for tag, i1, i2, j1, j2 in matcher.get_opcodes():
        if tag == 'equal':
            continue
        if tag != 'replace':
            return f'members are {"added" if tag == "insert" else "removed"} outside an unmodeled_ span'
        named = [m for m in b_members[i1:i2] if not _unmodeled_name(m)]
        if named:
            return f'a member that is not an unmodeled_ span changes ({named[0]})'
        want, got = members_size(b_members[i1:i2]), members_size(t_members[j1:j2])
        if want is None or got is None:
            return 'the size of the unmodeled_ span or of its replacement is not provable from the text'
        if want != got:
            return f'the unmodeled_ span is {want:#x} bytes, its replacement {got:#x}'
        offset = 0
        for layout in b_layout[:i1]:
            if layout is None:
                offset = None
                break
            offset = -(-offset // layout[1]) * layout[1] + layout[0]
        if offset is not None:
            ends = []
            for layouts in (b_layout[i1:i2], t_layout[j1:j2]):
                end = offset
                for size, align in layouts:
                    end = -(-end // align) * align + size
                ends.append(end)
            if ends[0] != ends[1]:
                return (f'with natural alignment the replacement of the unmodeled_ span after {offset:#x} '
                        f'ends at {ends[1]:#x}, the span at {ends[0]:#x}')
        spans.append(dict(offset=offset, bytes=want, removed=b_members[i1:i2], added=t_members[j1:j2]))
    if not spans:
        return 'no member changed'
    # the struct's own alignment (and so its size and tail padding) must not grow
    known = [layout[1] for layout in b_layout if layout is not None]
    added_align = max(layout[1] for s in spans for layout in map(member_layout, s['added']))
    alignment_checked = all(s['offset'] is not None for s in spans)
    if not known or added_align > max(known):
        if None not in b_layout:
            return f'the replacement raises the struct alignment from {max(known)} to {added_align}'
        alignment_checked = False
    for s in spans:
        s['alignment_checked'] = alignment_checked
    return spans


def _prelude_unmodeled_split(item, keys, added_ix, t_items, o_items, taken):
    """The unmodeled_ split that replaces base prelude `item`, or why it is none.

    Returns a dict (theirs_index, ours_index, replacement, spans) when exactly
    one added theirs item declares the same names as `item`, the target (ours)
    still holds `item` itself exactly once (a change by the patch only), and
    unmodeled_split() accepts the pair. Otherwise a reason string, or None when
    the patch adds no item of the same name (a plain removal).
    """
    candidates = sorted({k for key in keys for k in added_ix.get(key, [])} - taken)
    if not candidates:
        return None
    if len(candidates) != 1:
        return 'more than one added item declares its names'
    k = candidates[0]
    if decl_keys(t_items[k][1]) != keys:
        return 'the replacement declares different names'
    at = [j for j, (_, t) in enumerate(o_items) if _norm(t) == _norm(item)]
    if len(at) != 1:
        return 'the target no longer holds the base item unchanged (changed on both sides)'
    spans = unmodeled_split(item, t_items[k][1])
    if isinstance(spans, str):
        return spans
    return dict(theirs_index=k, ours_index=at[0], replacement=t_items[k][1], spans=spans)


def merge_prelude(base, ours, theirs, comments='strict'):
    """Three-way additive prelude merge. Returns (text, conflicts, notes).

    Comment-only differences never conflict (user direction 2026-09-19): a
    comment theirs removes or rewords, or a prelude item theirs changes only in
    its comments, keeps the target's text and is reported as a note
    (`prelude_comment_removed_ignored`, `prelude_comment_changed_ignored`). A
    new comment is added as before, and code changes are treated as before.

    One non-additive change is merged (2026-09-19): a base struct item that the
    patch replaces by the same struct with `unmodeled_` span member(s) split
    into named members of the same total size (unmodeled_split), while the
    target still holds the base item unchanged. The patch's item takes its
    place and a `prelude_unmodeled_split` note records it for the reviewer. A
    split that changes a size, touches another member, adds or removes members
    outside a span, or meets a target that changed the item stays
    `prelude_modified`, whose detail then says why.
    """
    b_items, _ = prelude_items(base)
    o_items, o_tail = prelude_items(ours)
    t_items, _ = prelude_items(theirs)
    conflicts, notes = [], []
    b_set = {_norm(t) for _, t in b_items}
    t_set = {_norm(t) for _, t in t_items}
    variant_base, variant_theirs, reworded = _comment_variants(b_items, t_items)
    removed = [(i, t) for i, (_, t) in enumerate(b_items) if _norm(t) not in t_set]
    added = [(k, g, t) for k, (g, t) in enumerate(t_items) if _norm(t) not in b_set]
    added_keys, added_ix = {}, {}
    for k, _, t in added:
        if k in variant_theirs:
            continue
        for key in decl_keys(t):
            added_keys.setdefault(key, []).append(t)
            added_ix.setdefault(key, []).append(k)
    split_theirs, split_ours = set(), {}
    dropped_banner = []
    for i, t in removed:
        if is_generated_banner(t):
            # Generated scaffold text (see _GENERATED_BANNER): removing or
            # replacing it is not a prelude change of authored source. It is
            # removed from the result too, so the replacement (if any) is
            # merged as an ordinary addition.
            dropped_banner.append(_norm(t))
            notes.append(dict(kind='generated_banner_removed', item=t))
            continue
        if _is_comment(t) and comments == 'ours':
            notes.append(dict(kind='prelude_comment_removed_ignored', item=t))
            continue
        if is_comment_only(t):
            notes.append(dict(kind='prelude_comment_removed_ignored', item=t,
                              detail='the patch removes or rewords a prelude comment; comment-only changes '
                                     'never conflict, and the target keeps its text'))
            continue
        if i in variant_base:
            notes.append(dict(kind='prelude_comment_changed_ignored', item=t,
                              detail='the patch changes only the comments of this prelude item; the target '
                                     'keeps its text'))
            continue
        shared = [a for key in decl_keys(t) for a in added_keys.get(key, [])]
        split = _prelude_unmodeled_split(t, decl_keys(t), added_ix, t_items, o_items, split_theirs)
        if isinstance(split, dict):
            split_theirs.add(split['theirs_index'])
            split_ours[split['ours_index']] = t_items[split['theirs_index']][1]
            notes.append(dict(kind='prelude_unmodeled_split', item=t, replacement=split['replacement'],
                              spans=split['spans'],
                              detail='the patch replaces unmodeled_ span member(s) of this prelude struct by '
                                     'named members of the same total size (the target still has the base '
                                     'item); merged in place for the reviewer to confirm against the member '
                                     'offsets in the evidence'))
            continue
        conflicts.append(dict(kind='prelude_modified' if shared else 'prelude_removed', item=t,
                              replacement=shared[0] if shared else None,
                              detail='the patch changes or removes an existing prelude item; '
                                     'prelude changes must be additive' +
                                     (f' (not a same-size unmodeled_ split: {split})' if split else '')))
    merged = [(g, split_ours.get(k, t)) for k, (g, t) in enumerate(o_items) if _norm(t) not in dropped_banner]
    merged_norm = [_norm(t) for _, t in merged]
    merged_code = [code_key(t) for _, t in merged]
    if merged and len(merged) < len(o_items) and not o_items[0][0].strip() and merged[0] != o_items[0]:
        merged[0] = (o_items[0][0], merged[0][1])   # the file keeps its leading gap, not the banner's
    for k, gap, t in added:
        nt = _norm(t)
        if k in split_theirs:
            continue                  # merged in place of the base item (prelude_unmodeled_split)
        if _is_comment(t) and comments == 'ours':
            notes.append(dict(kind='prelude_comment_added_ignored', item=t))
            continue
        if k in variant_theirs:
            continue                  # reported with the base item it changes (prelude_comment_changed_ignored)
        if k in reworded:
            notes.append(dict(kind='prelude_comment_changed_ignored', item=t,
                              detail='the patch rewords a prelude comment; the target keeps its text'))
            continue
        if nt in merged_norm:
            notes.append(dict(kind='prelude_already_present', item=t))
            continue
        t_decl, t_code = decl_keys(t), code_key(t)
        if t_decl and t_code and t_code in merged_code:
            notes.append(dict(kind='prelude_already_present', item=t,
                              detail='the target already has this item, differing only in comments'))
            continue
        clash = []
        for key in t_decl:
            for (_, ot), on in zip(merged, merged_norm):
                if key in decl_keys(ot) and on != nt:
                    clash.append(ot)
        if clash:
            if not any(c['kind'] == 'prelude_modified' and c['replacement'] == t for c in conflicts):
                conflicts.append(dict(kind='prelude_key_conflict', item=t, existing=clash[0],
                                      detail='the patch adds a declaration whose name is already '
                                             'declared differently in the target'))
            continue
        # Anchor by item identity: the nearest neighbour in theirs that occurs
        # exactly once in the merged prelude. A duplicated anchor text (two
        # `#else` lines, say) is ambiguous and must not be guessed.
        pos, ambiguous, right_after = None, None, False
        for direction in (-1, 1):
            j = k + direction
            while 0 <= j < len(t_items):
                n = _norm(t_items[j][1])
                count = merged_norm.count(n)
                if count == 1:
                    pos = merged_norm.index(n) + (1 if direction < 0 else 0)
                    # anchored on the item immediately before it in theirs
                    right_after = direction < 0 and j == k - 1
                    break
                if count > 1:
                    ambiguous = t_items[j][1]
                    break
                j += direction
            if pos is not None:
                break
        if pos is None and ambiguous is not None:
            conflicts.append(dict(kind='prelude_anchor_ambiguous', item=t, anchor=ambiguous,
                                  detail='the neighbouring prelude item of the addition occurs more than once '
                                         'in the target, so the insertion point is not determined'))
            continue
        if pos is None:
            pos = len(merged)
        # A trailing same-line comment keeps its gap when it follows the very
        # item it annotated in theirs. A `//` comment also needs a line break
        # after it in the target, or it would swallow the next item.
        follows = merged[pos][0] if pos < len(merged) else o_tail
        keep_inline = (right_after and _same_line_comment_gap(t, gap)
                       and (not t.lstrip().startswith('//') or '\n' in follows))
        if '\n' not in gap and not keep_inline:
            gap = '\n'
        if pos == 0 and not merged:
            gap = ''
        elif pos == 0 and not merged[0][0].strip():
            # A new first item takes the file's leading gap; the old first item
            # is then separated from it the way theirs separates them.
            follow = t_items[k + 1][0] if k + 1 < len(t_items) else '\n'
            gap, merged[0] = merged[0][0], (follow if '\n' in follow else '\n', merged[0][1])
        merged.insert(pos, (gap, t))
        merged_norm.insert(pos, nt)
        merged_code.insert(pos, t_code)
        notes.append(dict(kind='prelude_added', item=t, position=pos))
    common_b = [_norm(t) for _, t in b_items if _norm(t) in t_set]
    common_t = [_norm(t) for _, t in t_items if _norm(t) in b_set]
    if common_b != common_t:
        notes.append(dict(kind='prelude_reordered',
                          detail='the patch reorders existing prelude items; the target order is kept'))
    return _emit_prelude(merged, o_tail), conflicts, notes


def _three(base, ours, theirs, norm=_norm_block):
    """(result, status) for one region: status in same|ours|theirs|both_same|conflict."""
    b, o, t = norm(base), norm(ours), norm(theirs)
    if t == b:
        return ours, 'ours' if o != b else 'same'
    if o == b:
        return theirs, 'theirs'
    if o == t:
        return ours, 'both_same'
    return None, 'conflict'


_MERGE_FILE_TIMEOUT = 30


def _merge_comment_edits(base, ours, theirs):
    """theirs' code change with the target's comment-only edits kept, or None.

    `git merge-file` merges the region line by line (bounded, in a private
    temporary directory). The result is used only when the merge is clean and
    is exactly theirs as code, so the target's edits it carries are comments.
    """
    git = shutil.which('git')
    if git is None:
        return None
    try:
        with tempfile.TemporaryDirectory(prefix='tu-edit-merge-') as tmp:
            paths = []
            for name, text in (('ours', ours), ('base', base), ('theirs', theirs)):
                path = Path(tmp) / name
                _guard_canonical_write(path)
                path.write_bytes(text.encode('utf-8', 'surrogateescape'))
                paths.append(str(path))
            proc = subprocess.run([git, 'merge-file', '-p', '-q', *paths], stdout=subprocess.PIPE,
                                  stderr=subprocess.DEVNULL, timeout=_MERGE_FILE_TIMEOUT,
                                  start_new_session=True)
    except (OSError, subprocess.SubprocessError):
        return None
    if proc.returncode != 0:
        return None
    merged = proc.stdout.decode('utf-8', 'surrogateescape')
    return merged if same_code(merged, theirs) else None


def _three_code(base, ours, theirs, in_scope=True, merge_comments=True):
    """_three with comment-only differences made neutral (user direction 2026-09-19).

    Returns (text, status, note). A region theirs changes only in comments
    never conflicts: outside the allocation (`in_scope` False) or where the
    target changed the region too, the target's text is kept and `note` says
    so. Where the target changed only comments and theirs changes code, theirs
    is taken, with the target's comment edits merged in when that merges
    cleanly. Every code change keeps exactly the status `_three` gives it.
    """
    text, status = _three(base, ours, theirs)
    if not (status == 'conflict' or (status == 'theirs' and not in_scope)):
        return text, status, None
    if same_code(base, theirs):
        kept = 'ours' if _norm_block(ours) != _norm_block(base) else 'same'
        if status == 'theirs':
            return ours, kept, dict(kind='comment_only_change_ignored',
                                    detail='the patch changes only comments here, outside its allocation; '
                                           'comment-only changes never conflict, and the target keeps its text')
        return ours, kept, dict(kind='comment_only_change_superseded',
                                detail='the patch changes only comments here and the target changed this '
                                       'region as well; the target keeps its text')
    if status == 'conflict' and same_code(base, ours):
        merged = _merge_comment_edits(base, ours, theirs) if merge_comments else None
        if merged is not None:
            return merged, 'theirs', dict(kind='target_comment_edits_merged',
                                          detail='the target changed only comments here and the patch changes '
                                                 'the code; the patch is applied and the target\'s comment '
                                                 'edits are kept (git merge-file, clean)')
        return theirs, 'theirs', dict(kind='target_comment_edits_superseded',
                                      detail='the target changed only comments here and the patch changes the '
                                             'code; the patch text is taken, so the target\'s comment edits in '
                                             'this region are not kept')
    return text, status, None


def merge3(base, ours, theirs, names=None, allowed=None, comments='strict', interstitial_allowed=None):
    """Function-level three-way merge of TU objects. Returns a result dict.

    result['text'] is None when there are conflicts. Conflicts: structural
    (function missing/reordered), prelude (non-additive change, name clash),
    function changed on both sides, removal of accepted C or accepted asm,
    interstitial/epilogue text changed on both sides, change outside `allowed`.

    `interstitial_allowed` names functions outside `allowed` whose interstitial
    text (the file-scope text just before the function) the patch may change
    anyway: the worker's explicit, reasoned waiver
    `merge:interstitial_outside_allocation:<function>` (tools/worker.py result
    --allow-preflight). It is applied only when the function's own block is
    unchanged by the patch and the edit removes no function definition and no
    INCLUDE_ASM/ACCEPTED_ASM item of the base's interstitial text; otherwise it
    stays the `interstitial_outside_allocation` conflict. Each applied edit is
    reported in result['notes'] as `interstitial_outside_allocation_waived`, for
    the reviewer to judge; the whole-file gate and the TU audit stay the proof.

    A difference that is only in comments is never one of them (user direction
    2026-09-19): see merge_prelude and _three_code. It is reported in
    result['notes'] (`prelude_comment_*`, `comment_only_change_*`,
    `target_comment_edits_*`), and every code change is treated as before.
    """
    names = [b.key for b in base.functions()] if names is None else [n for n in names if n not in base.covered]
    result = dict(schema=SCHEMA_MERGE, conflicts=[], notes=[], applied=[], text=None,
                  base_sha256=sha256_text(base.text), ours_sha256=sha256_text(ours.text),
                  theirs_sha256=sha256_text(theirs.text))
    try:
        rb, ro, rt = Regions(base, names, 'base'), Regions(ours, names, 'ours'), Regions(theirs, names, 'theirs')
    except _Structural as e:
        result['conflicts'].append(dict(kind='structure', side=e.label, missing=e.missing,
                                        detail='the original function sequence differs', found=e.got))
        return result
    head = dict(result)
    result = _merge_regions(head, base, rb, ro, rt, names, allowed, comments, True, interstitial_allowed)
    if result.pop('_merged_comment_edits', False) and result['text'] is None and any(
            c['kind'] == 'structure' for c in result['conflicts']):
        # A region merged with the target's comment edits broke the layout:
        # take the patch text for those regions instead (never a new conflict).
        result = _merge_regions(head, base, rb, ro, rt, names, allowed, comments, False, interstitial_allowed)
        result.pop('_merged_comment_edits', None)
    return result


_DEFINITION_HEAD = re.compile(r'^[^;{}]*\)\s*\{')
_COMMENTS = re.compile(r'/\*.*?\*/|//[^\n]*', re.S)


def _interstitial_code_removed(base_text, theirs_text):
    """Function definitions and INCLUDE_ASM/ACCEPTED_ASM items of `base_text`
    (an interstitial region) that `theirs_text` no longer contains."""
    base_items, _ = prelude_items(base_text)
    theirs_items = {_norm(t) for _, t in prelude_items(theirs_text)[0]}
    removed = []
    for _, item in base_items:
        code = _COMMENTS.sub(' ', item).strip()
        if not code:
            continue
        if (_MACRO_ITEM.search(code) or _DEFINITION_HEAD.match(code)) and _norm(item) not in theirs_items:
            removed.append(code.splitlines()[0][:80])
    return removed


def _merge_regions(head, base, rb, ro, rt, names, allowed, comments, merge_comments, interstitial_allowed=None):
    result = dict(head, conflicts=list(head['conflicts']), notes=list(head['notes']), applied=list(head['applied']))
    prelude, conflicts, notes = merge_prelude(rb.prelude, ro.prelude, rt.prelude, comments)
    result['conflicts'] += conflicts
    result['notes'] += notes
    pre, block = {}, {}
    for n in names:
        bs, os_, ts = rb.state[n], ro.state[n], rt.state[n]
        in_scope = allowed is None or any(same_function(a, n) for a in allowed)
        text, status, note = _three_code(rb.block[n], ro.block[n], rt.block[n], in_scope, merge_comments)
        if note:
            result['notes'].append(dict(note, function=n))
            if note['kind'] == 'target_comment_edits_merged':
                result['_merged_comment_edits'] = True
        theirs_changed = status in ('theirs', 'both_same') or status == 'conflict'
        if theirs_changed and not in_scope:
            result['conflicts'].append(dict(kind='outside_allocation', function=n,
                                            detail='the patch changes a function it was not allocated'))
        if theirs_changed and bs == 'c' and ts != 'c':
            result['conflicts'].append(dict(kind='removes_accepted_c', function=n,
                                            detail=f'base has accepted C; the patch makes it {ts}'))
        elif theirs_changed and bs == 'accepted_asm' and ts == 'asm':
            result['conflicts'].append(dict(kind='removes_accepted_asm', function=n,
                                            detail='base has ACCEPTED_ASM; the patch makes it INCLUDE_ASM'))
        elif status == 'conflict':
            result['conflicts'].append(dict(kind='function_both_changed', function=n,
                                            base_state=bs, ours_state=os_, theirs_state=ts))
        else:
            if status in ('theirs', 'both_same'):
                result['applied'].append(dict(function=n, state=ts, from_state=bs))
                if bs == 'c' and ts == 'c':
                    result['notes'].append(dict(kind='changes_accepted_c', function=n))
                if bs == 'accepted_asm' and ts == 'c':
                    result['notes'].append(dict(kind='replaces_accepted_asm', function=n))
        block[n] = text if text is not None else ro.block[n]
        ptext, pstatus, note = _three_code(rb.pre[n], ro.pre[n], rt.pre[n], in_scope, merge_comments)
        if note:
            result['notes'].append(dict(note, interstitial_before=n))
            if note['kind'] == 'target_comment_edits_merged':
                result['_merged_comment_edits'] = True
        if pstatus == 'conflict':
            result['conflicts'].append(dict(kind='interstitial_both_changed', before_function=n))
            ptext = ro.pre[n]
        elif pstatus in ('theirs', 'both_same'):
            waived = not in_scope and bool(interstitial_allowed) and any(
                same_function(a, n) for a in interstitial_allowed)
            removed = _interstitial_code_removed(rb.pre[n], rt.pre[n]) if waived else []
            block_changed = status in ('theirs', 'both_same', 'conflict')
            if not in_scope and not (waived and not removed and not block_changed):
                detail = ('the patch changes file-scope text before a function it was not allocated (it can '
                          'redefine macros or data other functions use)')
                if waived and block_changed:
                    detail += '; the waiver does not apply: the function\'s own block changed too'
                if waived and removed:
                    detail += f'; the waiver does not apply: the edit removes {removed}'
                result['conflicts'].append(dict(kind='interstitial_outside_allocation', before_function=n,
                                                detail=detail))
            elif not in_scope:
                result['applied'].append(dict(interstitial_before=n, waived=True))
                result['notes'].append(dict(
                    kind='interstitial_outside_allocation_waived', before_function=n,
                    detail='the patch changes file-scope text before a function it was not allocated; applied '
                           'under the worker\'s recorded waiver (the function\'s own block is unchanged and no '
                           'definition or asm include is removed): the reviewer must judge the edit'))
            else:
                result['applied'].append(dict(interstitial_before=n))
        pre[n] = ptext
    etext, estatus, note = _three_code(rb.epilogue, ro.epilogue, rt.epilogue, True, merge_comments)
    if note:
        result['notes'].append(dict(note, epilogue=True))
        if note['kind'] == 'target_comment_edits_merged':
            result['_merged_comment_edits'] = True
    if estatus == 'conflict':
        result['conflicts'].append(dict(kind='epilogue_both_changed'))
        etext = ro.epilogue
    elif estatus in ('theirs', 'both_same'):
        result['applied'].append(dict(epilogue=True))
    if result['conflicts']:
        return result
    text = rb.emit(prelude=prelude, pre=pre, block=block, epilogue=etext)
    merged = TU(text, None, base.function_list, base.unit_dir)
    try:
        Regions(merged, names, 'merged')
    except _Structural:
        result['conflicts'].append(dict(kind='structure', side='merged', detail='merge broke the layout'))
        return result
    result['text'] = text
    result['sha256'] = sha256_text(text)
    result['functions'] = {b.key: b.state for b in merged.functions()}
    return result


# ---------------------------------------------------------------------------
# Patches
# ---------------------------------------------------------------------------

def merge_candidate(base, ours, theirs, names=None, allowed=None):
    """Carry exact authored file-scope text, with mapped function ownership.

    Declarations, storage and nonemitted static inline support are judged by
    the ordinary compiler, gate, data audit and independent source review.
    Their position before an unallocated function does not give that function
    ownership of the preceding text. Concurrent edits still fail closed.
    """
    originals = list(names if names is not None else
                     (base.function_list or [b.key for b in base.functions()]))
    result = dict(schema=SCHEMA_MERGE, conflicts=[], notes=[], applied=[], text=None,
                  base_sha256=sha256_text(base.text), ours_sha256=sha256_text(ours.text),
                  theirs_sha256=sha256_text(theirs.text), transport='tu-authored-source/1')
    if len(originals) != len(set(originals)) or not originals:
        raise EditError('candidate transport needs a unique original function list')
    allocated = originals if allowed is None else list(allowed)
    if len(allocated) != len(set(allocated)) or any(
            not any(same_function(n, f) for f in originals) for n in allocated):
        raise EditError('candidate allocation names a duplicate or non-original function')
    if any(not any(same_function(b.key, n) for n in originals)
           for t in (base, ours) for b in t.functions()):
        raise EditError('candidate original list omits a baseline or current mapped function')
    try:
        bound = [TU(t.text, t.path, originals, t.unit_dir) for t in (base, ours, theirs)]
        introduced = {child: parent for child, parent in bound[2].covered.items()
                      if child not in bound[0].covered}
        if introduced:
            # Recovery moves an original standalone scaffold helper into its
            # evidenced lexical parent. Merge the pair as one source region,
            # while retaining both original identities in claims and audits.
            # Removing its obsolete INCLUDE_ASM is permitted only with both
            # identities allocated; accepted standalone source is never erased.
            for child, parent in introduced.items():
                if not all(any(same_function(name, selected) for selected in allocated)
                           for name in (child, parent)):
                    raise ParseError(f'nested group needs both allocated identities: {parent}/{child}')
                old = bound[0].block(child)
                if old.state != 'asm' or child in bound[0].covered:
                    raise ParseError(f'nested transition cannot remove accepted standalone source: {child}')
                if child in bound[1].covered:
                    if bound[1].covered[child] != parent:
                        raise ParseError(f'concurrent nested parent differs for {child}')
                elif bound[1].block_text(bound[1].block(child)) != bound[0].block_text(old):
                    raise ParseError(f'concurrent standalone helper changed: {child}')
            for index in (0, 1):
                parsed = bound[index]
                cuts = [parsed.block(child) for child in introduced if child not in parsed.covered]
                text = parsed.text
                for block in sorted(cuts, key=lambda b: b.core_start, reverse=True):
                    text = text[:block.core_start] + text[block.core_end:]
                reduced = [name for name in originals if name not in introduced]
                bound[index] = TU(text, parsed.path, reduced, parsed.unit_dir)
            result['notes'].append(dict(kind='nested_group_transport', groups=introduced,
                                        detail='obsolete standalone scaffold regions are grouped with allocated lexical parents; original identities and ordinary audits remain required'))
        sequence = [n for n in originals if n not in bound[0].covered]
        sequence = [n for n in sequence if n not in introduced]
        rb, ro, rt = [Regions(t, sequence, label) for t, label in
                      zip(bound, ('base', 'ours', 'theirs'))]
    except (ParseError, _Structural) as exc:
        result['conflicts'].append(dict(kind='structure', detail=str(exc)))
        return result
    baseline_helpers = {b.name: b for b in bound[0].blocks if b.role == 'helper'}
    for block in bound[2].blocks:
        if block.role != 'helper':
            continue
        old = baseline_helpers.get(block.name)
        if old is not None and bound[0].block_text(old) == bound[2].block_text(block):
            continue
        head = lexical(theirs.text[block.core_start:theirs.text.index('{', block.core_start)])
        if old is not None or not (block.static and re.search(r'\b(?:inline|__inline|__inline__)\b', head)):
            result['conflicts'].append(dict(kind='unallocated_support_function', function=block.name,
                detail='only new unmapped static inline C support is carried; ordinary audits must '
                       'confirm it emits no extra function, code or data'))
        else:
            result['notes'].append(dict(kind='inline_support', function=block.name, credit=False,
                detail='ordinary compiler emission, source eligibility, gate and data audit remain '
                       'mandatory; this helper is not a mapped recovery claim'))
    for name in baseline_helpers:
        if not any(b.name == name and b.role == 'helper' for b in bound[2].blocks):
            result['conflicts'].append(dict(kind='removed_support_function', function=name))

    def merge_region(before, current, authored, **where):
        text, status = _three(before, current, authored, norm=lambda value: value)
        if status == 'conflict':
            result['conflicts'].append(dict(kind='candidate_both_changed', **where))
            return current
        if status in ('theirs', 'both_same') and authored != before:
            result['applied'].append(where)
        return text

    prelude = merge_region(rb.prelude, ro.prelude, rt.prelude, prelude=True)
    pre, blocks = {}, {}
    for n in sequence:
        selected = any(same_function(n, name) for name in allocated)
        bb, tb = bound[0].block(n), bound[2].block(n)
        bcore = bound[0].text[bb.core_start:bb.core_end]
        tcore = bound[2].text[tb.core_start:tb.core_end]
        if not selected and (bb.state != tb.state or bcore != tcore):
            result['conflicts'].append(dict(kind='outside_allocation', function=n,
                detail='authored source changes an unallocated mapped function identity or body'))
        if selected and bb.state == 'c' and tb.state != 'c':
            result['conflicts'].append(dict(kind='removes_accepted_c', function=n))
        if selected and bb.state == 'accepted_asm' and tb.state == 'asm':
            result['conflicts'].append(dict(kind='removes_accepted_asm', function=n))
        blocks[n] = merge_region(rb.block[n], ro.block[n], rt.block[n], function=n)
        pre[n] = merge_region(rb.pre[n], ro.pre[n], rt.pre[n], interstitial_before=n)
    epilogue = merge_region(rb.epilogue, ro.epilogue, rt.epilogue, epilogue=True)
    if result['conflicts']:
        return result
    text = rb.emit(prelude=prelude, pre=pre, block=blocks, epilogue=epilogue)
    merged = TU(text, None, originals, base.unit_dir)
    Regions(merged, sequence, 'merged')
    result.update(text=text, sha256=sha256_text(text),
                  functions={b.key: b.state for b in merged.functions()})
    return result


def make_candidate_patch(base, theirs, names=None, allowed=None, tu_path=None):
    """Pin exact candidate source for the serial merge used at publication."""
    names = list(names if names is not None else
                 (base.function_list or [b.key for b in base.functions()]))
    allowed = names if allowed is None else list(allowed)
    replay = merge_candidate(base, base, theirs, names, allowed)
    if replay.get('text') != theirs.text:
        raise EditError('candidate source is not an exact owned merge: ' +
                        json.dumps(replay.get('conflicts') or []))
    return dict(schema=SCHEMA_PATCH, tu=tu_path, base_sha256=sha256_text(base.text),
                prelude=dict(add=[], remove=[]), functions={}, interstitial={},
                candidate_source=dict(schema='tu-authored-source/1', functions=names,
                                      allocated=allowed, text=theirs.text,
                                      sha256=sha256_text(theirs.text)))


def _candidate_patch_source(base, patch):
    carrier = patch.get('candidate_source')
    required = {'schema', 'functions', 'allocated', 'text', 'sha256'}
    if (not isinstance(carrier, dict) or set(carrier) != required or
            carrier.get('schema') != 'tu-authored-source/1' or
            not isinstance(carrier.get('text'), str) or
            not isinstance(carrier.get('functions'), list) or
            not isinstance(carrier.get('allocated'), list) or
            any(not isinstance(n, str) for n in carrier['functions'] + carrier['allocated']) or
            sha256_text(carrier['text']) != carrier.get('sha256') or
            patch.get('base_sha256') != sha256_text(base.text) or
            patch.get('prelude') != {'add': [], 'remove': []} or
            patch.get('functions') != {} or patch.get('interstitial') != {} or
            set(patch) - {'schema', 'tu', 'base_sha256', 'prelude', 'functions', 'interstitial',
                          'candidate_source'}):
        raise EditError('candidate source carrier has invalid exact source, base or allocation pins')
    authored = TU(carrier['text'], None, carrier['functions'], base.unit_dir)
    replay = merge_candidate(base, base, authored, carrier['functions'], carrier['allocated'])
    if replay.get('text') != authored.text:
        raise EditError('candidate source carrier changes an unallocated mapped function: ' +
                        json.dumps(replay.get('conflicts') or []))
    return authored


def make_patch(base, theirs, names=None, tu_path=None):
    """Function-level patch that turns base into theirs."""
    names = [b.key for b in base.functions()] if names is None else [n for n in names if n not in base.covered]
    rb, rt = Regions(base, names, 'base'), Regions(theirs, names, 'theirs')
    b_items, _ = prelude_items(rb.prelude)
    t_items, _ = prelude_items(rt.prelude)
    # Prelude declarations are patch data too.  Normalizing whitespace here
    # loses deliberate layout edits (and can make replay differ from the
    # staged candidate).  Track exact text with multiplicity so duplicate
    # declarations are neither silently collapsed nor removed together.
    b_remaining = [t for _, t in b_items]
    additions = {}
    for _, text in t_items:
        if text in b_remaining:
            b_remaining.remove(text)
        else:
            additions[text] = additions.get(text, 0) + 1
    add = []
    for k, (gap, t) in enumerate(t_items):
        if additions.get(t, 0) == 0:
            continue
        # Consume one occurrence: identical prelude items can appear more
        # than once in real TUs and patch replay preserves their count.
        additions[t] -= 1
        after = None
        for j in range(k - 1, -1, -1):
            after = _norm(t_items[j][1])
            break
        add.append(dict(text=t, after=after, gap=gap))
    remove = b_remaining
    patch = dict(schema=SCHEMA_PATCH, tu=tu_path, base_sha256=sha256_text(base.text),
                 prelude=dict(add=add, remove=remove), functions={}, interstitial={})
    for n in names:
        if _norm_block(rb.block[n]) != _norm_block(rt.block[n]):
            patch['functions'][n] = dict(state=rt.state[n], text=_clean_block_text(rt.block[n]))
        if _norm_block(rb.pre[n]) != _norm_block(rt.pre[n]):
            patch['interstitial'][n] = rt.pre[n]
    if _norm_block(rb.epilogue) != _norm_block(rt.epilogue):
        patch['epilogue'] = rt.epilogue
    return patch


def patch_to_theirs(base, patch, names=None):
    """Apply a patch to base alone (no merge); returns the 'theirs' TU."""
    if patch.get('schema') != SCHEMA_PATCH:
        raise EditError(f'unsupported patch schema {patch.get("schema")!r}')
    if patch.get('base_sha256') not in (None, sha256_text(base.text)):
        raise EditError('patch base_sha256 does not match the base file')
    if 'candidate_source' in patch:
        return _candidate_patch_source(base, patch)
    names = [b.key for b in base.functions()] if names is None else [n for n in names if n not in base.covered]
    rb = Regions(base, names, 'base')
    items, tail = prelude_items(rb.prelude)
    prelude_patch = patch.get('prelude', {})
    removes = list(prelude_patch.get('remove', []))
    kept = []
    for gap, text in items:
        try:
            removes.remove(text)
        except ValueError:
            kept.append((gap, text))
    if removes:
        raise EditError('prelude removal item not found in base')
    items = kept
    for entry in prelude_patch.get('add', []):
        text, after = entry['text'], entry.get('after')
        norms = [_norm(t) for _, t in items]
        if after is None:
            pos = 0
        elif after in norms:
            pos = norms.index(after) + 1
        else:
            raise EditError(f'prelude anchor not found in base: {after[:60]!r}')
        gap = entry.get('gap', '\n')
        # `after` is always the item immediately before this one in theirs
        # (make_patch), so a trailing same-line comment keeps its gap there.
        keep = gap.strip() == '' and ('\n' in gap or pos == 0 or
                                      (after is not None and _same_line_comment_gap(text, gap)))
        items.insert(pos, (gap if keep else '\n', text))
    prelude = _emit_prelude(items, tail)
    pre, block = dict(rb.pre), dict(rb.block)
    for n, spec in patch.get('functions', {}).items():
        n = next((k for k in block if same_function(k, n)), n)
        if n not in block:
            raise EditError(f'patch names unknown function {n!r}')
        if spec.get('state') not in STATES:
            raise EditError(f'invalid state for {n}: {spec.get("state")!r}')
        text = _clean_block_text(spec['text'])
        if spec['state'] == 'c':
            check_c_body(text, name_stem(n), alt=n)
        else:
            m = _MACRO_ITEM.fullmatch(text.strip())
            if not m or MACRO_STATE.get(m.group(1)) != spec['state'] or not same_function(m.group(3), n):
                raise EditError(f'patch text for {n} is not its {spec["state"]} line')
        block[n] = text
    for n, text in patch.get('interstitial', {}).items():
        if n not in pre:
            raise EditError(f'patch interstitial names unknown function {n!r}')
        pre[n] = text
    epilogue = patch.get('epilogue', rb.epilogue)
    out = TU(prelude + ''.join(pre[n] + block[n] for n in names) + epilogue, None, base.function_list,
             base.unit_dir)
    Regions(out, names, 'patched')
    return out


def apply_patch(base, target, patch, names=None, allowed=None, comments='strict', interstitial_allowed=None):
    theirs = patch_to_theirs(base, patch, names)
    if 'candidate_source' in patch:
        carrier = patch['candidate_source']
        allocated = carrier['allocated']
        if allowed is not None and any(not any(same_function(n, a) for a in allowed)
                                       for n in allocated):
            raise EditError('candidate source carrier allocation exceeds the caller allocation')
        if names is not None and list(names) != carrier['functions']:
            raise EditError('candidate source carrier original function list differs from the caller')
        return merge_candidate(base, target, theirs, carrier['functions'], allocated)
    return merge3(base, target, theirs, names, allowed, comments, interstitial_allowed)


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def _guard_canonical_write(path):
    """TU editing writes private copies; the publisher owns the production tree."""
    target = Path(path).absolute()
    published = None
    candidates = (Path.cwd(), *Path.cwd().parents, Path(__file__).resolve().parent,
                  *Path(__file__).resolve().parents)
    for candidate in dict.fromkeys(candidates):
        queue = candidate / '.work/queue.sqlite3'
        if queue.is_file():
            production = queue.resolve().parent.parent
            if (production / 'config/tu-build.json').is_file():
                published = production
                break
    if published is None:
        raise EditError('cannot resolve the production queue; refusing a TU file write')
    resolved = target.resolve()
    for tree in ('src', 'include'):
        protected = published / tree
        if target.is_relative_to(protected) or resolved.is_relative_to(protected.resolve()):
            raise EditError(f'refusing tu_edit write to published {tree}: {target}; '
                            'edit a private attempt copy; normal publication uses tu_publish')
    if target.is_file() and target.stat().st_nlink != 1:
        raise EditError(f'refusing tu_edit write to a hardlinked destination: {target}')
    return target


def _write(path, text, in_place_of=None):
    if path in (None, '-'):
        sys.stdout.write(text)
        return
    path = _guard_canonical_write(path)
    fd, tmp = tempfile.mkstemp(dir=path.parent, prefix='.' + path.name + '.')
    with os.fdopen(fd, 'w', encoding='utf-8', errors='surrogateescape', newline='') as fh:
        fh.write(text)
    os.replace(tmp, path)


def _read(path):
    return Path(path).read_text(encoding='utf-8', errors='surrogateescape')


def _names(value):
    if value is None:
        return None
    return [n for part in value for n in part.split(',') if n]


def _same_file(a, b):
    """Whether two paths name the same file, without requiring both to exist."""
    pa, pb = Path(a), Path(b)
    try:
        if pa.exists() and pb.exists():
            return pa.samefile(pb)
    except OSError:
        pass
    return os.path.normpath(os.path.abspath(pa)) == os.path.normpath(os.path.abspath(pb))


def _emit_json(obj):
    json.dump(obj, sys.stdout, indent=1, sort_keys=False)
    sys.stdout.write('\n')


def _out_target(args):
    if getattr(args, 'in_place', False):
        return args.tu
    return getattr(args, 'output', None)


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split('\n\n')[0])
    sub = ap.add_subparsers(dest='cmd', required=True)

    def common(p, skeleton=True, out=False):
        p.add_argument('--unit-dir', help='unit build directory; resolves included .s files')
        if skeleton:
            p.add_argument('--skeleton', help='all-INCLUDE_ASM skeleton of this TU (original function list '
                                              'and revert lines)')
            p.add_argument('--functions', action='append',
                           help='original function names of this TU in order (comma-separated, repeatable)')
        if out:
            p.add_argument('-o', '--output', help='output file (default stdout)')
            p.add_argument('--in-place', action='store_true', help='rewrite the TU file')
        p.add_argument('--json', action='store_true', help='JSON report')

    p = sub.add_parser('list', help='functions and their state in original order')
    p.add_argument('tu')
    common(p)
    p = sub.add_parser('roundtrip', help='parse and re-emit, check byte identity')
    p.add_argument('tu', nargs='+')
    p.add_argument('--json', action='store_true')
    p = sub.add_parser('extract', help='print one function block')
    p.add_argument('tu')
    p.add_argument('name')
    p.add_argument('--from-source', action='store_true', help='TU is any C file; print the definition')
    p = sub.add_parser('to-c', help='replace a block with a C definition')
    p.add_argument('tu')
    p.add_argument('name')
    g = p.add_mutually_exclusive_group(required=True)
    g.add_argument('--body', help='file holding exactly the C definition')
    g.add_argument('--from-source', help='C file to take the definition of NAME (or --as) from')
    p.add_argument('--with-declarations', action='store_true',
                   help='with --from-source: also take the declarations the file has before the '
                        'definition (a bulk/m2c function file); the ones the TU lacks are placed '
                        'just before the function. --body accepts such leading declarations always')
    p.add_argument('--as', dest='as_name', help='C function name when it differs from the block name')
    p.add_argument('--replace-accepted-asm', action='store_true')
    common(p, skeleton=False, out=True)
    p = sub.add_parser('to-asm', help='revert a block to INCLUDE_ASM')
    p.add_argument('tu')
    p.add_argument('name')
    p.add_argument('--asm-dir')
    p.add_argument('--allow-accepted', action='store_true')
    common(p, out=True)
    p = sub.add_parser('subset', help='keep only the named functions as C')
    p.add_argument('tu')
    p.add_argument('--keep', action='append', default=[], help='comma-separated names (repeatable)')
    p.add_argument('--asm-dir')
    p.add_argument('--header', action='append', default=[],
                   help='extra header to read file-scope declarations from before warning '
                        'about a reverted static without a prototype (repeatable; '
                        "default: the TU's own <tu>.h)")
    common(p, out=True)
    p = sub.add_parser('split-asm', help='re-cut a tracked accepted .s to the functions '
                                         'still delivered as assembly')
    p.add_argument('asm', help='the tracked .s of the accepted group')
    p.add_argument('--keep', action='append', default=[],
                   help='comma-separated names to keep as assembly (repeatable)')
    p.add_argument('-o', '--output', help='the re-cut .s (default stdout)')
    p.add_argument('--manifest', help='write the tu-asm-split/1 manifest here')
    p.add_argument('--check', help='verify an existing manifest: re-derive the re-cut from the '
                                   'original and compare byte for byte')
    p.add_argument('--result', help='with --check, the re-cut to verify (default: the manifest '
                                    "entry's result.path)")
    p.add_argument('--drop-tail', action='store_true',
                   help="drop the file tail: re-attribute it to the last function, which --keep "
                        "drops (refused by default)")
    p.add_argument('--drop-header-lead-in', action='store_true',
                   help="drop the first function's .align/.globl lead-in from the header, when "
                        '--keep drops that function (refused by default)')
    p.add_argument('--json', action='store_true', help='JSON report')
    p = sub.add_parser('merge', help='function-level three-way merge')
    p.add_argument('--base', required=True)
    p.add_argument('--ours', required=True)
    p.add_argument('--theirs', required=True)
    p.add_argument('--allowed', action='append', help='functions the patch may change')
    p.add_argument('--interstitial-allowed', action='append',
                   help='functions outside --allowed whose preceding file-scope text the patch may change '
                        '(a recorded merge:interstitial_outside_allocation waiver)')
    p.add_argument('--comments', choices=('strict', 'ours'), default='strict')
    common(p, out=True)
    p = sub.add_parser('make-patch', help='function-level patch base -> theirs')
    p.add_argument('--base', required=True)
    p.add_argument('--theirs', required=True)
    p.add_argument('-o', '--output')
    p.add_argument('--skeleton')
    p.add_argument('--functions', action='append')
    p.add_argument('--unit-dir')
    p = sub.add_parser('apply-patch', help='three-way apply a patch onto a target')
    p.add_argument('--base', required=True)
    p.add_argument('--target', required=True)
    p.add_argument('--patch', required=True)
    p.add_argument('--allowed', action='append')
    p.add_argument('--interstitial-allowed', action='append')
    p.add_argument('--comments', choices=('strict', 'ours'), default='strict')
    common(p, out=True)
    p = sub.add_parser('owner-correction-manifest',
                       help='bind existing independently submitted owner receipts without rewriting them')
    p.add_argument('--repo', required=True)
    p.add_argument('--proposal', required=True)
    p.add_argument('--author-task', required=True)
    p.add_argument('--author-submission', required=True)
    p.add_argument('--review', required=True)
    p.add_argument('--review-task', required=True)
    p.add_argument('--review-submission', required=True)
    p.add_argument('--base-copy', help='immutable original owner header for a source-postimage proposal')
    p.add_argument('--consumer', action='append', required=True)
    p.add_argument('-o', '--output', required=True)
    args = ap.parse_args(argv)

    try:
        if args.cmd == 'owner-correction-manifest':
            manifest = owner_correction_manifest(Path(args.repo), args.proposal,
                args.author_task, args.author_submission, args.review, args.review_task,
                args.review_submission, args.consumer, args.base_copy)
            _write(args.output, json.dumps(manifest, indent=2) + '\n')
            return 0
        skeleton = load(args.skeleton) if getattr(args, 'skeleton', None) else None
        names = _names(getattr(args, 'functions', None)) or skeleton_functions(skeleton)
        unit_dir = getattr(args, 'unit_dir', None)

        def load_tu(path, with_names=True):
            return load(path, names if with_names else None, unit_dir)
        if args.cmd == 'list':
            tu = load_tu(args.tu)
            rows = tu.describe()
            if args.json:
                _emit_json(dict(tu=args.tu, sha256=sha256_text(tu.text),
                                prelude_lines=[1, line_of(tu.text, tu.prelude_end())], functions=rows))
            else:
                for r in rows:
                    extra = f" {r['asm_dir']}" if 'asm_dir' in r else (' static' if r.get('static') else '')
                    extra += ' (empty body)' if r.get('empty_body') else ''
                    role = '' if r['role'] == 'function' else f" ({r['role']})"
                    print(f"{r['index']:4d} {r['state']:12s} {r['name']}{role}{extra}  "
                          f"[{r['start_line']}-{r['end_line']}]")
            return 0
        if args.cmd == 'roundtrip':
            rows, bad = [], 0
            for path in args.tu:
                raw = Path(path).read_bytes()
                try:
                    tu = TU(raw.decode('utf-8', 'surrogateescape'), path)
                    out = tu.text.encode('utf-8', 'surrogateescape')
                    # Rebuild from regions to prove the block model covers every byte.
                    names_rt = [b.key for b in tu.functions()]
                    emitted = Regions(tu, names_rt, 'rt').emit().encode('utf-8', 'surrogateescape') \
                        if names_rt else out
                    ok = out == raw and emitted == raw
                    rows.append(dict(tu=path, identical=ok, functions=len(tu.blocks),
                                     states={s: sum(b.state == s for b in tu.blocks) for s in STATES}))
                except (ParseError, EditError, _Structural) as e:
                    ok = False
                    rows.append(dict(tu=path, identical=False, error=str(e)))
                bad += not ok
            if args.json:
                _emit_json(dict(checked=len(rows), failed=bad, results=rows))
            else:
                for r in rows:
                    print(('OK  ' if r['identical'] else 'FAIL'), r['tu'], r.get('error', ''))
            return 1 if bad else 0
        if args.cmd == 'extract':
            if args.from_source:
                sys.stdout.write(extract_function(_read(args.tu), args.name) + '\n')
                return 0
            tu = load(args.tu)
            sys.stdout.write(tu.block_text(tu.block(args.name)) + '\n')
            return 0
        if args.cmd == 'to-c':
            tu = load_tu(args.tu)
            if args.body:
                body = _read(args.body)
            else:
                body = extract_function(_read(args.from_source), args.as_name or args.name,
                                        declarations=args.with_declarations)
            report = {}
            out = to_c(tu, args.name, body, args.as_name, args.replace_accepted_asm, report=report)
            _write(_out_target(args), out.text)
            if report.get('added_declarations') or report.get('skipped_declarations'):
                print(f'to-c {args.name}: {len(report["added_declarations"])} declaration(s) placed '
                      f'before the function, {len(report["skipped_declarations"])} already declared '
                      f'in the TU (skipped)', file=sys.stderr)
            if args.json:
                print(json.dumps(dict(function=args.name, state='c', sha256=sha256_text(out.text),
                                      added_declarations=report.get('added_declarations') or [],
                                      skipped_declarations=report.get('skipped_declarations') or [])),
                      file=sys.stderr)
            return 0
        if args.cmd == 'to-asm':
            tu = load_tu(args.tu)
            out = to_asm(tu, args.name, skeleton=skeleton, asm_dir=args.asm_dir,
                         allow_accepted=args.allow_accepted)
            _write(_out_target(args), out.text)
            return 0
        if args.cmd == 'subset':
            tu = load_tu(args.tu)
            out, report = subset(tu, _names(args.keep) or [], skeleton, args.asm_dir,
                                 headers=args.header or None)
            _write(_out_target(args), out.text)
            report['sha256'] = sha256_text(out.text)
            if args.json or report['warnings']:
                print(json.dumps(report, indent=1), file=sys.stderr)
            return 0
        if args.cmd == 'split-asm':
            if args.check:
                manifest = json.loads(_read(args.check))
                if manifest.get('schema') != SCHEMA_ASM_SPLIT:
                    # An accepted `config/units` record carries the manifest of
                    # its own re-cut, so the record itself is checkable.
                    manifest = (manifest.get('source') or {}).get('split') or manifest
                # The ORIGIN named on the command line is what is read and
                # hashed, and it must be the one the manifest pins: `--check`
                # used to ignore the positional argument and silently read
                # `manifest['origin']['path']` instead, so a reviewer who named
                # a file had verified nothing about it (independent review of
                # `tu-asm-group-split`, finding D3).
                pinned_path = manifest['origin'].get('path')
                origin_path = args.asm
                if pinned_path and not _same_file(origin_path, pinned_path):
                    raise EditError(
                        f'the origin named on the command line ({origin_path}) is not the one '
                        f'this manifest pins ({pinned_path}); --check verifies the pinned '
                        'original, so name that file or check the manifest that describes '
                        'the one you named')
                origin = _read(origin_path)
                result_path = args.result or (manifest.get('result') or {}).get('path')
                if not result_path:
                    raise EditError('--check needs --result or a result.path in the manifest')
                report = check_asm_split(manifest, origin, _read(result_path))
                report['manifest'] = args.check
                report['origin']['path'] = origin_path
                report['origin']['pinned_path'] = pinned_path
                report['result']['path'] = result_path
                if args.json:
                    _emit_json(report)
                else:
                    print(('OK  ' if report['ok'] else 'FAIL'), result_path,
                          f"{len(report['keep'])} kept, {len(report['drop'])} split off, "
                          f"{report['bytes_kept']} byte(s) verbatim")
                    for problem in report['problems']:
                        print('   ', problem, file=sys.stderr)
                return 0 if report['ok'] else 1
            text = _read(args.asm)
            result, manifest = split_asm(text, _names(args.keep) or [], args.asm,
                                         drop_tail=args.drop_tail,
                                         drop_header_lead_in=args.drop_header_lead_in)
            if args.output:
                manifest['result']['path'] = args.output
            _write(args.output, result)
            if args.manifest:
                _write(args.manifest, json.dumps(manifest, indent=1) + '\n')
            if args.json:
                print(json.dumps(manifest, indent=1), file=sys.stderr)
            return 0
        if args.cmd in ('merge', 'apply-patch'):
            base = load_tu(args.base)
            if args.cmd == 'merge':
                ours, theirs = load_tu(args.ours), load_tu(args.theirs)
                result = merge3(base, ours, theirs, names, _names(args.allowed), args.comments,
                                _names(args.interstitial_allowed))
            else:
                target = load_tu(args.target)
                patch = json.loads(_read(args.patch))
                result = apply_patch(base, target, patch, names, _names(args.allowed), args.comments,
                                     _names(args.interstitial_allowed))
            text = result.pop('text')
            if text is not None:
                dest = args.ours if (args.cmd == 'merge' and args.in_place) else (
                    args.target if args.in_place else args.output)
                _write(dest, text)
            if args.json or text is None:
                print(json.dumps(result, indent=1), file=sys.stderr)
            return 0 if text is not None else 1
        if args.cmd == 'make-patch':
            base, theirs = load_tu(args.base), load_tu(args.theirs)
            patch = make_patch(base, theirs, names, args.theirs)
            _write(args.output, json.dumps(patch, indent=1) + '\n')
            return 0
    except (ParseError, EditError, _Structural) as e:
        print(f'tu_edit: {e}', file=sys.stderr)
        return 2 if isinstance(e, ParseError) else 1
    return 2


# Hash-bound, zero-credit reviewed source corrections.
import sqlite3
import time

class CorrectionError(ValueError):
    pass


def digest(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def _artifact(repo: Path, ref: dict, label: str) -> tuple[Path, bytes]:
    if not isinstance(ref, dict) or set(ref) != {'path', 'sha256'}:
        raise CorrectionError(f'{label}: expected path and sha256')
    raw = Path(ref['path'])
    if raw.is_absolute() or '..' in raw.parts or not raw.parts:
        raise CorrectionError(f'{label}: path must be repository-relative')
    path = (repo / raw).resolve()
    try:
        path.relative_to(repo.resolve())
    except ValueError as exc:
        raise CorrectionError(f'{label}: path escapes repository') from exc
    data = path.read_bytes()
    if digest(data) != ref['sha256']:
        raise CorrectionError(f'{label}: pinned artifact digest changed')
    return path, data


def _copy_bytes(repo: Path, value: str, expected_sha: str, label: str) -> bytes:
    raw = Path(value)
    path = raw.resolve() if raw.is_absolute() else (repo / raw).resolve()
    try:
        path.relative_to(repo.resolve())
    except ValueError as exc:
        raise CorrectionError(f'{label}: copy escapes repository') from exc
    data = path.read_bytes()
    if digest(data) != expected_sha:
        raise CorrectionError(f'{label}: copy digest changed')
    return data


def _safe_destination(value: str) -> Path:
    path = Path(value)
    if path.is_absolute() or '..' in path.parts or not path.parts or path.parts[0] not in ('src', 'include'):
        raise CorrectionError('correction destination must be a safe src/ or include/ path')
    return path


def _verify_reviewer_task(db_path: Path, reviewer: str, report_path: Path) -> None:
    uri = f'file:{db_path.resolve()}?mode=ro'
    with sqlite3.connect(uri, uri=True) as db:
        row = db.execute('SELECT owner, status, lease_until FROM tasks WHERE id=?', (reviewer,)).fetchone()
        events = db.execute('SELECT at, action, actor, detail FROM audit WHERE task_id=? ORDER BY id',
                            (reviewer,)).fetchall()
    if not row:
        raise CorrectionError('review identity is not the owner of a live/completed queue task')
    report_mtime = report_path.stat().st_mtime
    claim_at = None
    claim_identity_ok = False
    for at, action, actor, detail in events:
        if action == 'claim' and actor == reviewer:
            claim_at = at
        elif action in ('release', 'expire') and claim_at is not None:
            if claim_at <= report_mtime <= at:
                claim_identity_ok = True
            claim_at = None
    if (row[0] == reviewer and row[1] in ('claimed', 'submitted', 'done') and
            (row[1] != 'claimed' or (row[2] is not None and row[2] >= time.time()))):
        claim_identity_ok = True
    if not claim_identity_ok:
        raise CorrectionError('review report was not produced while its independent reviewer task was claimed')


def _verify_submission(db_path: Path, task: str, submission_id: str, artifact: str, author: str) -> None:
    uri = f'file:{db_path.resolve()}?mode=ro'
    with sqlite3.connect(uri, uri=True) as db:
        task_row = db.execute('SELECT owner, status FROM tasks WHERE id=?', (task,)).fetchone()
        audit = db.execute("SELECT detail FROM audit WHERE task_id=? AND action='submit' ORDER BY id DESC LIMIT 1",
                           (task,)).fetchone()
    if not task_row or task_row[0] != author or task_row[1] not in ('submitted', 'done') or not audit:
        raise CorrectionError('proposal submission is absent from the production queue')
    try:
        detail = json.loads(audit[0])
    except ValueError as exc:
        raise CorrectionError('proposal submit event has invalid JSON') from exc
    if detail.get('submission_id') != submission_id:
        raise CorrectionError('proposal task does not have the exact cited latest submission')
    repo = db_path.resolve().parent.parent
    paths = []
    for value in (detail.get('artifact'), artifact):
        if not isinstance(value, str) or not value:
            raise CorrectionError('proposal submission has no exact artifact path')
        path = Path(value)
        path = (path if path.is_absolute() else repo / path).resolve()
        try:
            path.relative_to(repo)
        except ValueError as exc:
            raise CorrectionError('proposal submission artifact escapes repository') from exc
        paths.append(path)
    if paths[0] != paths[1]:
        raise CorrectionError('proposal task does not have the exact cited latest submission')


def _correction_row(row: dict) -> tuple[dict, dict]:
    """Return the file row and its pinned operation metadata."""
    correction = row.get('correction') if isinstance(row, dict) else None
    if correction is None and isinstance(row, dict) and 'id' in row:
        return row, {}
    if not isinstance(correction, dict) or correction.get('schema') != 'reviewed-source-correction/1':
        raise CorrectionError('file row lacks reviewed-source-correction/1 metadata')
    flattened = dict(correction)
    flattened.pop('schema', None)
    flattened.update({key: row[key] for key in ('role', 'path', 'action', 'base', 'result', 'patch')
                      if key in row})
    flattened['id'] = correction.get('id')
    return flattened, correction


def _receipt_ref(repo, value):
    """Normalize an artifact spelling, never its bytes or hash."""
    path = Path(value['path'] if 'path' in value else value['copy'])
    if path.is_absolute():
        path = path.resolve().relative_to(repo.resolve())
    ref = dict(path=path.as_posix(), sha256=value['sha256'])
    _artifact(repo, ref, 'existing receipt artifact')
    return ref


def _queue_submission(repo, task, sid, artifact=None, actor=None):
    with sqlite3.connect(f'file:{repo / ".work/queue.sqlite3"}?mode=ro', uri=True) as db:
        current = db.execute('SELECT owner,artifact FROM tasks WHERE id=?', (task,)).fetchone()
    if not current or not current[0] or not current[1]:
        raise CorrectionError('receipt task has no submitted actor/artifact')
    if actor is not None and actor != current[0]:
        raise CorrectionError('receipt actor differs from its exact queue submission')
    path = Path(current[1])
    path = (path if path.is_absolute() else repo / path).resolve()
    _verify_submission(repo / '.work/queue.sqlite3', task, sid,
                       str(artifact or path), current[0])
    return current[0], path


def _review_submission(repo, row, review, review_path):
    """Bind a report's actor or official task alias to its exact submitted actor."""
    actor, _ = _queue_submission(repo, row['review_task'], row['review_submission_id'], review_path)
    if review.get('reviewer') != row['reviewer'] or row['reviewer'] not in (actor, row['review_task']):
        raise CorrectionError('reviewer is neither the submitted actor nor its official task alias')
    if actor == row['proposal_author'] or row['reviewer'] == row['proposal_author']:
        raise CorrectionError('proposal author and submitted reviewer must be distinct')
    return actor


def _owner_harvest_module():
    """Resolve the peer tool for normal CLI and file-loaded worker entry points."""
    import importlib.util
    peer = Path(__file__).resolve().with_name('header_harvest.py')
    if not peer.is_file():
        raise CorrectionError('normal owner ingress is missing its peer header_harvest tool')
    name = '_tu_edit_owner_harvest_' + digest(peer.read_bytes())[:16]
    if name not in sys.modules:
        # Peer routines use the ordinary tu_edit import for shared tokenizers.
        # File-loaded callers need the same actual module registered by name.
        sys.modules.setdefault('tu_edit', sys.modules[__name__])
        spec = importlib.util.spec_from_file_location(name, peer)
        module = importlib.util.module_from_spec(spec)
        sys.modules[name] = module
        try:
            spec.loader.exec_module(module)
        except BaseException:
            sys.modules.pop(name, None)
            raise
    return sys.modules[name]


def _header_owner_receipt(repo, row, base, post):
    """Use the submitted full header review; admit no function or storage definition."""
    harvest = _owner_harvest_module()
    operation = row['operation']
    graph = json.loads((repo / 'config/tu-build.json').read_text())
    owner = next((t for t in graph['tus'] if t['id'] == operation.get('owner_tu')), None)
    if (not owner or owner.get('category') != 'game' or not owner.get('in_scope') or
            row['path'] != str(Path(owner['path']).with_suffix('.h')) or
            operation.get('revision') != graph['revision'] or
            operation.get('original_tu_sha256') != owner['text']['sha256']):
        raise CorrectionError('reviewed header is not its exact original defining owner')
    before, after = (data.decode('utf-8', 'surrogateescape') for data in (base, post))
    if TU(before).blocks or TU(after).blocks or \
            harvest.owner_migration_context(before) != harvest.owner_migration_context(after):
        raise CorrectionError('reviewed owner header contains function bodies')
    for text in (before, after):
        for item in scan(text):
            if item.kind not in ('decl', 'comment', 'pp'):
                raise CorrectionError('reviewed header contains unsupported source')
            if item.kind == 'decl':
                raw = item.text(text)
                retained_static_proto = (re.fullmatch(
                    r'\s*static\s+[^(){}=;]+\s+[A-Za-z_]\w*\s*\([^{};]*\)\s*;\s*', raw) and
                    before.count(raw) == after.count(raw) == 1)
                if (not list(harvest.scan_declarations(raw)) and not retained_static_proto) or \
                        harvest.ordinary_candidate_c(raw) is not None:
                    raise CorrectionError('reviewed header contains a storage or function definition')
    old = {(d.namespace, d.name): d for d in harvest.scan_declarations(before)}
    new = list(harvest.scan_declarations(after))
    if len({(d.namespace, d.name) for d in new}) != len(new):
        raise CorrectionError('reviewed header has ambiguous entities')
    keys = [(d.namespace, d.name) for d in new if (d.namespace, d.name) not in old or
            not same_code(d.text, old[(d.namespace, d.name)].text)]
    exports = []
    for key in keys:
        entity = harvest.owner_migration_entity(after, key)
        if entity.kind in harvest.TYPE_KINDS and '{' in entity.canon:
            exports.append(dict(namespace=key[0], name=key[1]))
        elif entity.kind == harvest.KIND_EXTERN_DATA:
            harvest.original_asm_data_owner(str(repo), owner['id'], key[1])
            exports.append(dict(namespace=key[0], name=key[1]))
    return exports


def _asm_extern_receipt(repo, row, proposal, report, review, base, post, patch_bytes, evidence_bytes):
    """Adapt the retained original ASM-owner schemas, retaining every original pin."""
    if (proposal.get('id') != row['id'] or proposal.get('path') != row['path'] or
            proposal.get('proposed_by') != row['proposal_author'] or
            proposal.get('operation') != row['operation'] or proposal.get('zero_credit') is not True or
            proposal.get('source_credit') != 0 or proposal.get('storage_status') != 'assembly-scaffolding' or
            proposal.get('function_bodies_changed') != [] or proposal.get('data_definitions_added') != [] or
            proposal.get('base') != row['base'] or proposal.get('result') != row['result'] or
            _receipt_ref(repo, proposal['patch']) != row['patch']):
        raise CorrectionError('original ASM extern proposal differs from its exact carrier')
    report_path, report_bytes = _artifact(repo, row['proposal'], 'submitted original author report')
    if (report.get('schema') != 'owner-asm-data-extern-author-report/1' or
            report.get('task') != row['proposal_task'] or report.get('author') != row['proposal_author'] or
            report.get('source_credit') != 0 or
            _receipt_ref(repo, report['evidence']) != row['evidence'] or
            _receipt_ref(repo, report['patch']) != row['patch'] or
            _receipt_ref(repo, report['header_postimage']) !=
            dict(path=Path(row['result']['copy']).as_posix(), sha256=digest(post))):
        raise CorrectionError('submitted original author report does not pin this proposal/postimage')
    _queue_submission(repo, row['proposal_task'], row['proposal_submission_id'], report_path,
                      row['proposal_author'])
    pin = review.get('proposal') or {}
    export = review.get('export') or {}
    evidence = json.loads(evidence_bytes)
    if (review.get('schema') != 'independent-owner-asm-data-extern-review/1' or
            review.get('verdict') != 'supported_for_private_owner_header_staging' or
            pin.get('task') != row['proposal_task'] or
            pin.get('submission_id') != row['proposal_submission_id'] or
            _receipt_ref(repo, pin) != _receipt_ref(repo, report['proposal']) or
            _receipt_ref(repo, review['evidence']) != row['evidence'] or
            export.get('path') != row['path'] or export.get('base_sha256') != digest(base) or
            export.get('postimage_sha256') != digest(post) or export.get('zero_credit') is not True or
            row['affected_tus'] != [export.get('consumer_tu')] or
            review.get('owner_tu') != proposal.get('owner_tu') or
            export.get('declaration') != evidence.get('declaration')):
        raise CorrectionError('original independent ASM review does not bind this exact owner/consumer')
    harvest = _owner_harvest_module()
    identity = evidence.get('identity') or {}
    harvest.verify_asm_data_owner(str(repo), proposal['owner_tu'], identity.get('name'), identity)
    proof = review['evidence'].get('symbol') or {}
    expected = dict(name=identity['name'], address=identity['va'], size=identity['size'],
                    binding=identity['binding'], type=identity['type'], section=identity['section'],
                    raw_bytes_sha256=identity['raw_bytes_sha256'])
    if proof != expected or review['evidence'].get('original_elf_sha256') != identity['original_elf_sha256']:
        raise CorrectionError('independent ASM review original bytes/identity differ')
    declaration_delta(base.decode(), post.decode(), row['operation']['changes'])


def validate_correction(repo: Path, row: dict, *, declaration_config=False) -> dict:
    """Validate row provenance and exact patch before any destination write."""
    row, metadata = _correction_row(row)
    row = dict(row)
    correction_kind = row.pop('kind', metadata.get('kind'))
    verification = row.pop('verification_tus', None)
    required = {
        'role', 'id', 'path', 'action', 'base', 'result', 'patch', 'proposal',
        'review', 'evidence', 'proposal_author', 'proposal_task', 'proposal_submission_id',
        'reviewer', 'review_task', 'review_submission_id', 'affected_tus',
        'zero_credit', 'operation',
    }
    role = 'owner_config_correction' if declaration_config else 'owner_source_correction'
    if set(row) != required or row.get('role') != role or \
            row.get('action') != 'patch' or row.get('zero_credit') is not True:
        raise CorrectionError('correction row has unexpected fields or is not zero-credit')
    if declaration_config:
        if row['path'] != 'config/header-owner-migrations.json':
            raise CorrectionError('reviewed declaration config path is outside its one-path scope')
    else:
        _safe_destination(row['path'])
    if row['proposal_author'] == row['reviewer']:
        raise CorrectionError('proposal author and reviewer must be distinct')
    if not isinstance(row['affected_tus'], list) or not row['affected_tus'] or \
            len(set(row['affected_tus'])) != len(row['affected_tus']):
        raise CorrectionError('affected_tus must be a nonempty unique list')
    for label in ('base', 'result'):
        spec = row[label]
        if not isinstance(spec, dict) or set(spec) != {'sha256', 'copy'}:
            raise CorrectionError(f'{label}: expected sha256 and copy')
        data = _copy_bytes(repo, spec['copy'], spec['sha256'], f'{label} copy')
        if label == 'base':
            base = data
        else:
            result = data
    patch_path, patch_bytes = _artifact(repo, row['patch'], 'correction patch')
    proposal_path, proposal_bytes = _artifact(repo, row['proposal'], 'proposal')
    review_path, review_bytes = _artifact(repo, row['review'], 'review')
    _, evidence_bytes = _artifact(repo, row['evidence'], 'evidence')
    try:
        proposal = json.loads(proposal_bytes)
        review = json.loads(review_bytes)
    except (UnicodeError, ValueError) as exc:
        raise CorrectionError(f'proposal/review JSON is invalid: {exc}') from exc
    asm_receipt = proposal.get('schema') == 'owner-asm-data-extern-author-report/1'
    author_report = proposal if asm_receipt else None
    if asm_receipt:
        original_proposal_ref = _receipt_ref(repo, author_report['proposal'])
        original_proposal_bytes = _artifact(repo, original_proposal_ref, 'original ASM extern proposal')[1]
        proposal = json.loads(original_proposal_bytes)
        if declaration_config or proposal.get('schema') != 'owner-asm-data-extern-proposal/1':
            raise CorrectionError('author report adapter is only for the original ASM extern schema')
        _asm_extern_receipt(repo, row, proposal, author_report, review, base, result, patch_bytes, evidence_bytes)
    elif proposal.get('schema') != 'private-owner-source-patch/1' or \
            proposal.get('id') != row['id'] or proposal.get('owner_path') != row['path']:
        raise CorrectionError('proposal identity/path differs from correction row')
    if not asm_receipt and row['operation'] != proposal.get('field'):
        raise CorrectionError('correction operation differs from the pinned proposal field')
    identity = proposal.get('source_identity') or {}
    if not asm_receipt:
        _, proposal_source = _artifact(repo, {'path': proposal.get('proposal_artifact'),
                                         'sha256': proposal.get('proposal_sha256')},
                                  'proposed owner source')
    if not asm_receipt and (identity.get('before_sha256') != digest(base) or \
            identity.get('after_sha256') != digest(result) or proposal_source != result):
        raise CorrectionError('proposal does not bind exact before/after bytes')
    if not asm_receipt and (proposal.get('patch_artifact') != row['patch']['path'] or \
            proposal.get('patch_sha256') != digest(patch_bytes)):
        raise CorrectionError('proposal does not bind exact patch')
    if not asm_receipt and (review.get('schema') != 'owner-source-patch-review/1' or \
            review.get('decision') != 'evidence_supported' or \
            review.get('reviewer') != row['reviewer']):
        raise CorrectionError('review identity or verdict is not supported')
    submission = review.get('submission') or {}
    pins = review.get('pins') or {}
    if not asm_receipt and (submission.get('author') != row['proposal_author'] or
            submission.get('task') != row['proposal_task'] or
            submission.get('submission_id') != row['proposal_submission_id'] or
            submission.get('artifact') != row['proposal']['path'] or
            submission.get('artifact_sha256') != digest(proposal_bytes) or
            pins.get('patch') != row['patch'] or
            pins.get('evidence') != row['evidence'] or
            (pins.get('current_owner_source') != {'path': row['path'], 'sha256': digest(base)} and
             pins.get('base_source') != {'path': row['path'], 'sha256': digest(base)}) or
            (pins.get('proposed_owner_source_sha256') != digest(result) and
             (pins.get('proposed_source') or {}).get('sha256') != digest(result))):
        raise CorrectionError('independent review does not pin this exact author/proposal/patch/source')
    if not asm_receipt:
        _verify_submission(repo / '.work/queue.sqlite3', row['proposal_task'], row['proposal_submission_id'],
                           str(proposal_path), row['proposal_author'])
    review_actor = _review_submission(repo, row, review, review_path)
    scope = proposal.get('scope') or {}
    if not asm_receipt and (scope.get('function_bodies_changed') != [] or \
            scope.get('accepted_neighbor_bodies_changed') != []):
        raise CorrectionError('owner correction changes or fails to preserve function bodies')
    # The exact patch must transform the exact before-image into the exact
    # postimage. Use git's patch parser rather than trusting a claimed diff.
    with tempfile.TemporaryDirectory(prefix='owner-correction-') as tmp:
        work = Path(tmp)
        target = work / row['path']
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_bytes(base)
        check = subprocess.run(['git', 'apply', '--check', str(patch_path)],
                               cwd=work, stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=False)
        if check.returncode:
            raise CorrectionError(f'git apply --check failed: {check.stderr.decode(errors="replace")}')
        apply = subprocess.run(['git', 'apply', str(patch_path)], cwd=work,
                               stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=False)
        if apply.returncode or target.read_bytes() != result:
            raise CorrectionError('exact patch does not produce the pinned postimage')
    receipt = dict(path=row['path'], before_sha256=digest(base), after_sha256=digest(result),
                proposal_author=row['proposal_author'], reviewer=row['reviewer'],
                proposal_sha256=digest(proposal_bytes), review_sha256=digest(review_bytes),
                evidence_sha256=digest(evidence_bytes), patch_sha256=digest(patch_bytes),
                affected_tus=list(row['affected_tus']), zero_credit=True,
                operation=row['operation'])
    if asm_receipt or row['operation'].get('schema') in (
            'tu-reviewed-owner-source-postimage/1', 'tu-reviewed-declaration-delta/1') and \
            row['path'].endswith('.h') and correction_kind == 'owner_source':
        receipt['export_declarations'] = _header_owner_receipt(repo, row, base, result)
        receipt['review_actor'] = review_actor
        if not isinstance(verification, list) or not verification or \
                row['operation']['owner_tu'] not in verification or \
                not set(row['affected_tus']).issubset(verification):
            raise CorrectionError('owner receipt verification set omits its owner or consumer')
        receipt['verification_tus'] = verification
        if asm_receipt:
            receipt['author_report_sha256'] = row['proposal']['sha256']
            receipt['original_proposal'] = original_proposal_ref
    return receipt


def validate_owner_export(repo, report_row):
    """Carry an existing independent export review without changing owner source."""
    row, _ = _correction_row(report_row)
    row = dict(row)
    if row.pop('kind', None) != 'owner_export':
        raise CorrectionError('unchanged owner export lacks its normal carrier kind')
    required = {'role', 'id', 'path', 'action', 'base', 'result', 'patch', 'proposal',
                'review', 'evidence', 'proposal_author', 'proposal_task', 'proposal_submission_id',
                'reviewer', 'review_task', 'review_submission_id', 'affected_tus',
                'verification_tus', 'zero_credit', 'operation'}
    if set(row) != required or row['role'] != 'owner_source_correction' or \
            row['action'] != 'patch' or row['zero_credit'] is not True or row['base'] != row['result']:
        raise CorrectionError('unchanged export carrier changes source or has unexpected fields')
    _safe_destination(row['path'])
    author_path, author_bytes = _artifact(repo, row['proposal'], 'original export author report')
    author_report = json.loads(author_bytes)
    if author_report.get('schema') != 'existing-owner-type-export-author-report/1' or \
            author_report.get('task') != row['proposal_task'] or author_report.get('zero_source_credit') is not True:
        raise CorrectionError('unchanged export author report identity differs')
    _queue_submission(repo, row['proposal_task'], row['proposal_submission_id'], author_path,
                      row['proposal_author'])
    proposal_ref = _receipt_ref(repo, author_report['proposal'])
    proposal = json.loads(_artifact(repo, proposal_ref, 'original export proposal')[1])
    export = proposal.get('export') or {}
    spec = export.get('source_owner') or {}
    if (proposal.get('schema') != 'existing-owner-type-export-proposal/1' or
            proposal.get('path') != 'config/header-exports.json' or proposal.get('id') != row['id'] or
            proposal.get('source_edits') != [] or proposal.get('zero_credit') is not True or
            export.get('tu') != proposal.get('owner_tu') or spec.get('path') != row['path'] or
            row['operation'] != dict(schema='tu-reviewed-existing-owner-export/1',
                owner_tu=proposal['owner_tu'], export=export) or
            not row['affected_tus'] or len(row['affected_tus']) != len(set(row['affected_tus'])) or
            row['affected_tus'] != proposal.get('consumers') or
            export.get('consumers') != row['affected_tus'] or
            row['verification_tus'] != sorted(set(row['affected_tus']) | {proposal['owner_tu']})):
        raise CorrectionError('unchanged export scope differs from the original reviewed proposal')
    if (row['evidence'] != _receipt_ref(repo, proposal['evidence']) or
            row['patch'] != _receipt_ref(repo, proposal['patch']) or
            author_report.get('evidence') != proposal.get('evidence') or
            author_report.get('checks') != proposal.get('checks') or
            author_report.get('patch') != proposal.get('patch')):
        raise CorrectionError('unchanged export evidence/patch differs from the original report')
    _artifact(repo, proposal['checks'], 'original export generation checks')
    witness = _copy_bytes(repo, row['base']['copy'], row['base']['sha256'], 'unchanged owner source witness')
    if digest(witness) != spec.get('sha256'):
        raise CorrectionError('unchanged owner source differs from its original review witness')
    before_path, before = _artifact(repo, proposal['base'], 'original export config before')
    after_path, after = _artifact(repo, proposal['result'], 'original export config after')
    before_doc, after_doc = json.loads(before), json.loads(after)
    expected = dict(before_doc)
    expected['exports'] = list(before_doc['exports']) + [export]
    if after_doc != expected:
        raise CorrectionError('original export config adds more than the one reviewed owner row')
    patch_path, _ = _artifact(repo, proposal['patch'], 'original export config patch')
    with tempfile.TemporaryDirectory(prefix='owner-export-receipt-') as temporary:
        target = Path(temporary) / proposal['path']
        target.parent.mkdir(parents=True)
        target.write_bytes(before)
        applied = subprocess.run(['git', 'apply', str(patch_path)], cwd=temporary,
            stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=False, timeout=30)
        if applied.returncode or target.read_bytes() != after:
            raise CorrectionError('original export patch does not reproduce its config postimage')
    review_path, review_bytes = _artifact(repo, row['review'], 'original independent export review')
    review = json.loads(review_bytes)
    actor = _review_submission(repo, row, review, review_path)
    if actor == row['proposal_author'] or review.get('schema') != 'existing-owner-type-export-review/1' or \
            review.get('decision') != 'supported' or review.get('author') != dict(
                task=row['proposal_task'], owner=row['proposal_author'],
                submission_id=row['proposal_submission_id'], report=row['proposal']):
        raise CorrectionError('unchanged export has no distinct supported original review')
    pinned = review.get('proposal') or {}
    scope = review.get('scope') or {}
    if (any(pinned.get(k) != proposal_ref[k] for k in ('path', 'sha256')) or
            any(pinned.get(k) != proposal.get(k) for k in ('evidence', 'checks', 'patch')) or
            scope.get('owner_tu') != proposal['owner_tu'] or
            scope.get('consumer_tus') != row['affected_tus'] or
            scope.get('source_owner') != dict(path=spec['path'], sha256=spec['sha256']) or
            (scope.get('name') != (export.get('names') or [None])[0] and
             scope.get('names') != export.get('names')) or
            scope.get('zero_source_credit') is not True):
        raise CorrectionError('independent export review pins or owner scope differ')
    graph = json.loads((repo / 'config/tu-build.json').read_text())
    owner = next((t for t in graph['tus'] if t['id'] == proposal['owner_tu']), None)
    consumers = {t['id']: t for t in graph['tus']}
    if (not owner or owner.get('category') != 'game' or not owner.get('in_scope') or
            row['path'] not in (owner['path'], str(Path(owner['path']).with_suffix('.h'))) or
            any(t not in consumers or consumers[t]['unit'] != owner['unit'] or
                not consumers[t].get('in_scope') or t == owner['id'] for t in row['affected_tus'])):
        raise CorrectionError('unchanged export is outside its original owner/consumer tuples')
    harvest = _owner_harvest_module()
    harvest.source_bound_exports(str(repo), {}, doc=dict(exports=[export]))
    return dict(kind='owner_export', path=row['path'], before_sha256=digest(witness),
                after_sha256=digest(witness), export=export, original_proposal=proposal_ref,
                proposal_sha256=digest(author_bytes), review_sha256=digest(review_bytes),
                review_actor=actor, affected_tus=row['affected_tus'],
                verification_tus=row['verification_tus'], operation=row['operation'], zero_credit=True)


def retain_export_owner_source(repo, project, report_row):
    """Keep newer owner declarations; prove only the reviewed entity and context."""
    receipt = validate_owner_export(repo, report_row)
    target = Path(project) / report_row['path']
    _guard_canonical_write(target)
    if target.is_symlink() or not target.resolve().is_relative_to(Path(project).resolve()):
        raise CorrectionError('unchanged export owner source escapes the private project')
    live = target.read_text(encoding='utf-8')
    witness = _copy_bytes(repo, report_row['base']['copy'], report_row['base']['sha256'],
                          'unchanged export source').decode('utf-8')
    harvest = _owner_harvest_module()
    if harvest.owner_migration_context(witness) != harvest.owner_migration_context(live):
        raise CorrectionError('unchanged export owner preprocessing context changed')
    for declaration in receipt['export']['source_owner']['declarations']:
        key = declaration['namespace'], declaration['name']
        before, after = (harvest.owner_migration_entity(text, key) for text in (witness, live))
        if (before.kind != after.kind or before.defined != after.defined or
                not same_code(before.text, after.text) or harvest.ordinary_candidate_c(after.text) is not None):
            raise CorrectionError('unchanged export owner entity changed')
    pin = digest(target.read_bytes())
    receipt.update(source_retained=True, current_before_sha256=pin, result_sha256=pin,
                   outcome='retained_current_owner_source')
    return receipt


def owner_correction_manifest(repo, proposal_path, author_task, author_sid,
                              review_path, review_task, review_sid, consumers, base_copy=None):
    """Produce only pointers to actual immutable author/reviewer artifacts."""
    repo = Path(repo).resolve()
    def ref(path):
        target = Path(path)
        target = (target if target.is_absolute() else repo / target).resolve()
        relative = target.relative_to(repo).as_posix()
        return dict(path=relative, sha256=digest(target.read_bytes()))
    proposal_ref, review_ref = ref(proposal_path), ref(review_path)
    proposal = json.loads(_artifact(repo, proposal_ref, 'existing proposal')[1])
    review = json.loads(_artifact(repo, review_ref, 'existing review')[1])
    author, submitted = _queue_submission(repo, author_task, author_sid)
    if proposal.get('schema') == 'existing-owner-type-export-proposal/1':
        if not base_copy:
            raise CorrectionError('unchanged export requires its already immutable owner source witness')
        source_ref = ref(base_copy)
        source = dict(copy=source_ref['path'], sha256=source_ref['sha256'])
        row = dict(role='owner_source_correction', path=proposal['export']['source_owner']['path'],
            action='patch', base=source, result=dict(source), patch=_receipt_ref(repo, proposal['patch']),
            correction=dict(schema='reviewed-source-correction/1', kind='owner_export',
                id=proposal['id'], proposal=ref(submitted), review=review_ref,
                evidence=_receipt_ref(repo, proposal['evidence']), proposal_author=author,
                proposal_task=author_task, proposal_submission_id=author_sid,
                reviewer=review['reviewer'], review_task=review_task, review_submission_id=review_sid,
                affected_tus=sorted(consumers),
                verification_tus=sorted(set(consumers) | {proposal['owner_tu']}), zero_credit=True,
                operation=dict(schema='tu-reviewed-existing-owner-export/1',
                               owner_tu=proposal['owner_tu'], export=proposal['export'])))
        validate_owner_export(repo, row)
        return dict(schema='reviewed-source-corrections/1', corrections=[row])
    if proposal.get('schema') == 'owner-asm-data-extern-proposal/1':
        operation, path = proposal['operation'], proposal['path']
        base, post = proposal['base'], proposal['result']
        patch = _receipt_ref(repo, proposal['patch'])
        evidence = _receipt_ref(repo, json.loads(submitted.read_text())['evidence'])
        proposal_ref = ref(submitted)
    elif proposal.get('schema') == 'private-owner-source-patch/1' and \
            proposal.get('field', {}).get('schema') in (
                'tu-reviewed-owner-source-postimage/1', 'tu-reviewed-declaration-delta/1') and \
            proposal.get('owner_path', '').endswith('.h'):
        if not base_copy:
            raise CorrectionError('source-postimage manifest requires its immutable original header copy')
        operation, path = proposal['field'], proposal['owner_path']
        base_ref = ref(base_copy)
        base = dict(copy=base_ref['path'], sha256=base_ref['sha256'])
        post = dict(copy=proposal['proposal_artifact'], sha256=proposal['proposal_sha256'])
        patch = dict(path=proposal['patch_artifact'], sha256=proposal['patch_sha256'])
        evidence = proposal['evidence']
    else:
        raise CorrectionError('no normal adapter for this immutable proposal schema')
    graph = json.loads((repo / 'config/tu-build.json').read_text())
    owner = next((t for t in graph['tus'] if t['id'] == operation['owner_tu']), None)
    tus = {t['id']: t for t in graph['tus']}
    if (not owner or len(consumers) != len(set(consumers)) or not consumers or
            any(t not in tus or tus[t]['unit'] != owner['unit'] or
                not tus[t].get('in_scope') or t == owner['id'] for t in consumers)):
        raise CorrectionError('owner manifest has an invalid original consumer set')
    row = dict(role='owner_source_correction', path=path, action='patch',
               base=base, result=post, patch=patch,
               correction=dict(schema='reviewed-source-correction/1', kind='owner_source',
                   id=proposal['id'], proposal=proposal_ref, review=review_ref, evidence=evidence,
                   proposal_author=author, proposal_task=author_task, proposal_submission_id=author_sid,
                   reviewer=review['reviewer'], review_task=review_task, review_submission_id=review_sid,
                   affected_tus=sorted(consumers), verification_tus=sorted(set(consumers) | {owner['id']}),
                   zero_credit=True, operation=operation))
    validate_reviewed_source_correction(repo, row)
    return dict(schema='reviewed-source-corrections/1', corrections=[row])


def declaration_keys(text):
    """Keys of every top-level item in an exact declaration snippet."""
    return set().union(*(decl_keys(item.text(text)) for item in scan(text)))


def declaration_delta(base_text, result_text, changes):
    """Prove that exact listed prelude declarations are the entire source delta."""
    base, result = TU(base_text), TU(result_text)
    items, _ = prelude_items(base.prelude())
    expected = base_text
    seen = set()
    if not isinstance(changes, list) or not changes:
        raise CorrectionError('declaration correction has no exact declaration changes')
    for change in changes:
        fields = {'before_text', 'after_text', 'before_sha256', 'after_sha256',
                  'before_keys', 'after_keys', 'witnesses'}
        if not isinstance(change, dict) or set(change) not in (fields, fields | {'after_anchor'}):
            raise CorrectionError('declaration change has unexpected fields')
        before, after = change['before_text'], change['after_text']
        if not isinstance(before, str) or not isinstance(after, str) or before == after:
            raise CorrectionError('declaration change lacks distinct exact before/after text')
        if sha256_text(before) != change['before_sha256'] or sha256_text(after) != change['after_sha256']:
            raise CorrectionError('declaration text differs from its hash')
        anchor = change.get('after_anchor')
        if not before:
            if (not anchor or sum(text == anchor for _, text in items) != 1 or
                    len(declaration_keys(after)) != 1 or not re.fullmatch(
                        r'\s*extern\s+[^;{}=()]+;\s*', lexical(after))):
                raise CorrectionError('new owner declaration must be an exact anchored plain data extern')
        elif anchor is not None or before in seen or sum(text == before for _, text in items) != 1:
            raise CorrectionError('before declaration is repeated or not a unique whole prelude item')
        seen.add(before)
        for text, keys in ((before, change['before_keys']), (after, change['after_keys'])):
            if sorted(declaration_keys(text)) != sorted(tuple(key) for key in keys):
                raise CorrectionError('declaration keys differ from exact declaration text')
            for item in scan(text):
                raw = item.text(text)
                if item.conditional or item.kind not in ('decl', 'comment', 'pp'):
                    raise CorrectionError('declaration correction contains non-declaration source')
                if item.kind == 'pp' and not re.fullmatch(
                        r'\s*#\s*define\s+([A-Za-z_]\w*)\s+\1\.[A-Za-z_]\w*\s*', raw):
                    raise CorrectionError('declaration correction contains an unsupported directive')
        if (before and not declaration_keys(before)) or not change['witnesses']:
            raise CorrectionError('declaration correction lacks declaration keys or reviewed witnesses')
        # Replace only the exact prelude item; never a matching use inside a function.
        head = expected[:TU(expected).prelude_end()]
        target = before or anchor
        if head.count(target) != 1:
            raise CorrectionError('before declaration is not unique in the current prelude')
        expected = head.replace(target, after if before else anchor + '\n' + after, 1) + expected[len(head):]
    if expected != result_text:
        raise CorrectionError('unlisted declaration, function or interstitial bytes changed')
    before_blocks = {b.key: base.block_text(b) for b in base.blocks}
    after_blocks = {b.key: result.block_text(b) for b in result.blocks}
    if before_blocks != after_blocks:
        raise CorrectionError('declaration correction changes a function block')
    return dict(function_bodies_changed=[], exact_listed_delta=True)


FILE_SCOPE_OPERATION = 'tu-reviewed-file-scope-delta/1'


def _file_scope_regions(tu):
    """Account for every byte outside all top-level function blocks, including helpers."""
    blocks = tu.blocks
    regions = {'prelude': tu.text[:blocks[0].start] if blocks else tu.text}
    previous = blocks[0].start if blocks else len(tu.text)
    for block in blocks:
        regions['before:' + block.key] = tu.text[previous:block.start]
        previous = block.end
    regions['epilogue'] = tu.text[previous:] if blocks else ''
    return regions


def file_scope_inventory(text):
    """Exact scanner inventory for one explicit nonfunction region."""
    items, _ = prelude_items(text)
    return [dict(gap=gap, text=raw, sha256=sha256_text(raw),
                 kind=scan(raw)[0].kind, keys=[list(k) for k in sorted(declaration_keys(raw))])
            for gap, raw in items]


def file_scope_delta(base_text, result_text, regions):
    """Prove a complete listed file-scope delta while preserving every function byte."""
    base, result = TU(base_text), TU(result_text)
    signature = lambda tu: [(b.key, b.name, b.state, b.static, b.conditional,
                            tu.block_text(b), tu.text[b.core_start:b.core_end]) for b in tu.blocks]
    if signature(base) != signature(result):
        raise CorrectionError('file-scope correction changes a function block or core')
    before, after = _file_scope_regions(base), _file_scope_regions(result)
    fields = {'name', 'before_text', 'after_text', 'before_sha256', 'after_sha256',
              'before_items', 'after_items'}
    if (not isinstance(regions, list) or [r.get('name') for r in regions if isinstance(r, dict)] !=
            list(before) or list(before) != list(after)):
        raise CorrectionError('file-scope inventory does not list every region in exact order')
    old_items, new_items = [], []
    for row in regions:
        if set(row) != fields:
            raise CorrectionError('file-scope region has unexpected fields')
        for label, actual in (('before', before[row['name']]), ('after', after[row['name']])):
            if row[label + '_text'] != actual or row[label + '_sha256'] != sha256_text(actual):
                raise CorrectionError('file-scope region differs from exact source bytes')
            expected = file_scope_inventory(actual)
            given = row[label + '_items']
            if not isinstance(given, list) or len(given) != len(expected):
                raise CorrectionError('file-scope region item inventory is incomplete')
            for item, facts in zip(given, expected):
                if (not isinstance(item, dict) or set(item) != set(facts) | {'witnesses'} or
                        {k: v for k, v in item.items() if k != 'witnesses'} != facts or
                        not isinstance(item['witnesses'], list) or
                        (label == 'before' and item['witnesses'])):
                    raise CorrectionError('file-scope item differs from its exact scanner inventory')
                scanned = scan(item['text'])[0]
                if scanned.conditional or scanned.kind not in ('decl', 'comment', 'pp'):
                    raise CorrectionError('file-scope correction contains conditional or executable source')
                if scanned.kind == 'pp' and not re.fullmatch(
                        r'\s*#\s*include\s+"[A-Za-z0-9_./-]+"\s*', item['text']):
                    raise CorrectionError('file-scope correction contains an unsupported directive')
                if scanned.kind == 'decl' and re.search(
                        r'\b(?:asm|__asm|__asm__|__attribute__|INCLUDE_ASM|ACCEPTED_ASM|incbin)\b',
                        lexical(item['text'])):
                    raise CorrectionError('file-scope declaration contains an emission extension')
                (old_items if label == 'before' else new_items).append(item)
    if base_text == result_text:
        raise CorrectionError('file-scope correction has no source delta')
    return dict(function_bodies_changed=[], exact_listed_delta=True,
                baseline_function_blocks=[dict(name=b.key, sha256=sha256_text(base.block_text(b)),
                                               core_sha256=sha256_text(base.text[b.core_start:b.core_end]))
                                          for b in base.blocks],
                old_items=old_items, new_items=new_items)


def _file_scope_roundtrip(post, author, allocated, owner):
    """The reviewed correction accounts for all declarations; only allocated bodies remain."""
    corrected, authored = TU(post), TU(author)
    names = [b.key for b in corrected.blocks]
    if (not isinstance(allocated, list) or not allocated or len(allocated) != len(set(allocated)) or
            any(not isinstance(n, str) or n not in names or not any(
                same_function(n, f['name']) for f in owner['functions']) for n in allocated) or
            [b.key for b in authored.blocks] != names or
            _file_scope_regions(corrected) != _file_scope_regions(authored)):
        raise CorrectionError('authored source has unreviewed file-scope bytes or allocation identity')
    for block in corrected.blocks:
        if block.key not in allocated and corrected.block_text(block) != authored.block_text(
                authored.block(block.key)):
            raise CorrectionError('authored source changes an unallocated function block')
    merged = merge3(corrected, corrected, authored, allowed=allocated)
    if merged.get('conflicts') or merged.get('text') != author:
        raise CorrectionError('ordinary allocated-body merge does not reproduce exact authored bytes')
    return dict(author_sha256=sha256_text(author), allocated_functions=list(allocated),
                exact_ordinary_roundtrip=True)


def _file_scope_author(repo, flat, owner):
    """A slice claim cannot authorize relocation/recovery of whole-TU declarations or data."""
    uri = f'file:{(repo / ".work/queue.sqlite3").resolve()}?mode=ro'
    with sqlite3.connect(uri, uri=True) as db:
        resources = {r[0] for r in db.execute('SELECT resource FROM resources WHERE task_id=?',
                                             (flat['proposal_task'],))}
        events = db.execute('SELECT at, action, actor FROM audit WHERE task_id=? ORDER BY id',
                            (flat['proposal_task'],)).fetchall()
    if 'otu:' + owner['id'] not in resources:
        raise CorrectionError('file-scope author lacks the real whole-TU declaration/data assignment')
    claim = None
    submitted = None
    for at, action, actor in events:
        if action == 'claim':
            claim = at if actor == flat['proposal_author'] else None
        elif action in ('release', 'expire'):
            claim = None
        elif action == 'submit' and actor == flat['proposal_author']:
            submitted = (claim, at)
    if not submitted or submitted[0] is None:
        raise CorrectionError('file-scope proposal was not submitted under its author claim')
    for label, path in (('proposal', repo / flat['proposal']['path']),
                        ('correction postimage', Path(flat['result']['copy']))):
        if not path.is_absolute():
            path = repo / path
        if not submitted[0] <= path.stat().st_mtime <= submitted[1]:
            raise CorrectionError(f'file-scope {label} was not produced under the whole-TU claim')


def _file_scope_compilation(repo, refs, owner, post_sha, original):
    """Pin fresh compiler emission and normal data/gate/audit evidence to the correction source."""
    labels = {'source', 'object', 'assembly', 'candidate', 'carves', 'audit', 'gate'}
    if not isinstance(refs, dict) or set(refs) != labels:
        raise CorrectionError('file-scope compilation lacks exact source/object/assembly/data/gate/audit pins')
    raw = {key: _artifact(repo, value, 'file-scope ' + key)[1] for key, value in refs.items()}
    candidate, carves, audit, gate = (json.loads(raw[k]) for k in ('candidate', 'carves', 'audit', 'gate'))
    if (digest(raw['source']) != post_sha or
            candidate.get('schema') != 'data-recovery-candidate/1' or candidate.get('tu') != owner['id'] or
            candidate.get('unit') != owner['unit'] or
            (candidate.get('files', {}).get(owner['path']) or {}).get('sha256') != post_sha or
            (candidate.get('compiler_object') or {}).get('sha256') != digest(raw['object']) or
            (candidate.get('assembly') or {}).get('sha256') != digest(raw['assembly']) or
            audit.get('schema') != 'tu-audit/2' or audit.get('ok') is not True or
            audit.get('grants_acceptance') is not True or audit.get('contract_relaxed') or
            audit.get('tu_id') != owner['id'] or
            audit.get('contract', {}).get('id') != owner['contract']['id'] or
            audit.get('contract', {}).get('flags') != owner['contract']['flags'] or
            (audit.get('contract', {}).get('compiler', {}).get('sha256') or {}).get('cc1') !=
            owner['contract']['cc1_sha256'] or
            audit.get('contract', {}).get('assembler', {}).get('sha256') != owner['contract']['assembler_sha256'] or
            (audit.get('inputs', {}).get('tu_c') or {}).get('sha256') != post_sha or
            (audit.get('inputs', {}).get('original') or {}).get('sha256') != original['sha256'] or
            (audit.get('linked_data_candidate') or {}).get('sha256') != digest(raw['candidate']) or
            gate.get('schema') != 'elf-gate/1' or gate.get('result') != 'pass' or
            owner['id'] not in (gate.get('tus') or []) or
            not set(owner['linked_into']).issubset(gate.get('required_targets') or [])):
        raise CorrectionError('file-scope compiler/source/object/assembly/data/gate/audit provenance differs')
    for unit in owner['linked_into']:
        target = gate.get('targets', {}).get(unit) or {}
        if (target.get('identical') is not True or
                unit != owner['unit'] or (target.get('original') or {}).get('sha256') != original['sha256'] or
                (target.get('built') or {}).get('sha256') != (target.get('original') or {}).get('sha256')):
            raise CorrectionError('file-scope correction lacks a complete identical linked target')
    return raw['object'], carves, raw['assembly']


def _file_scope_witness(repo, ref, owner, tu_build, verification):
    """Explicit current accepted declarations or original witnesses, never a candidate-as-authority."""
    _, raw = _artifact(repo, ref, 'file-scope declaration witness')
    witness = json.loads(raw)
    if witness.get('schema') in ('tu-declaration-evidence/1', 'tu-declaration-witness/1'):
        return _declaration_witness(repo, ref, owner, tu_build, verification)
    if witness.get('schema') != 'tu-file-scope-witness/1':
        raise CorrectionError('file-scope declaration witness has an unsupported schema')
    kind = witness.get('kind')
    defining = next((t for t in tu_build['tus'] if t['id'] == witness.get('declaration_tu')), None)
    if not defining or defining['unit'] != owner['unit']:
        raise CorrectionError('file-scope declaration witness lacks a genuine mapped owner')
    if kind == 'original_symbol':
        if set(witness) != {'schema', 'kind', 'declaration_tu', 'witness'}:
            raise CorrectionError('original declaration witness has unexpected fields')
        return _declaration_witness(repo, witness['witness'], defining, tu_build, verification)
    if kind == 'header_include':
        if set(witness) != {'schema', 'kind', 'declaration_tu', 'header'}:
            raise CorrectionError('header include witness has unexpected fields')
        expected = f'include/{defining["unit"]}/{Path(defining["path"]).stem}.h'
        _artifact(repo, witness['header'], 'published owner header')
        if witness['header']['path'] != expected:
            raise CorrectionError('included header is outside its mapped defining owner')
        return {('include', expected.removeprefix('include/'))}
    if kind != 'published_declaration' or set(witness) != {
            'schema', 'kind', 'declaration_tu', 'source', 'text', 'sha256'}:
        raise CorrectionError('published declaration witness has unexpected fields')
    if witness['source']['path'] not in (defining['path'], str(Path(defining['path']).with_suffix('.h'))):
        raise CorrectionError('declaration witness is outside current accepted defining source')
    _, source = _artifact(repo, witness['source'], 'published defining source')
    text = source.decode('utf-8', 'surrogateescape')
    matches = [item for item in scan(text) if item.kind == 'decl' and not item.conditional and
               item.text(text) == witness['text']]
    if len(matches) != 1 or sha256_text(witness['text']) != witness['sha256']:
        raise CorrectionError('published declaration witness is not an exact unique accepted item')
    return declaration_keys(witness['text'])


def _file_scope_original_item(repo, ref, owner, original, section, name, address, size, binding):
    """Exact original scaffold label span and dlabel binding, paired with original ELF bytes."""
    if owner['unit'] != 'main' or not isinstance(ref, dict) or set(ref) != {'manifest', 'piece', 'macros'}:
        raise CorrectionError('original storage item lacks supported MAIN scaffold identity pins')
    manifest_path, raw = _artifact(repo, ref['manifest'], 'original MAIN scaffold manifest')
    manifest = json.loads(raw)
    if (ref['manifest']['path'] != 'build/main/tu-manifest.json' or manifest.get('unit') != 'main' or
            manifest.get('identity') != {'binary': original['path'], 'sha256': original['sha256']}):
        raise CorrectionError('original scaffold manifest has a different binary identity')
    tus = [t for t in manifest.get('tus', []) if t.get('id') == owner['id'] and t.get('path') == owner['path']]
    if len(tus) != 1:
        raise CorrectionError('original scaffold does not have its exact defining TU')
    sec = tus[0].get('sections', {}).get(section) or {}
    pieces = sec.get('pieces') or [dict(name=tus[0]['name'], start=sec.get('start'), end=sec.get('end'))]
    matching = [p for p in pieces if int(p['start'], 16) <= address < address + size <= int(p['end'], 16)]
    if len(matching) != 1:
        raise CorrectionError('original storage span is outside one defining scaffold piece')
    piece = matching[0]
    relative = piece.get('file') or f'asm/main/data/{piece["name"]}.{section[1:]}.s'
    expected = (manifest_path.parent / relative).resolve().relative_to(repo.resolve()).as_posix()
    if ref['piece']['path'] != expected or ref['macros']['path'] != 'build/main/include/macro.inc':
        raise CorrectionError('original item or binding macro is outside the exact scaffold mapping')
    _, data = _artifact(repo, ref['piece'], 'original storage scaffold piece')
    _, macros = _artifact(repo, ref['macros'], 'original storage binding macros')
    macro = re.search(r'(?ms)^\.macro dlabel label, visibility=global\s*\n(.*?)^\.endm', macros.decode())
    if (not macro or '.\\visibility "\\label"' not in macro[1] or
            '.type "\\label", @object' not in macro[1]):
        raise CorrectionError('original dlabel OBJECT/GLOBAL binding macro differs')
    lines = data.decode('utf-8', 'surrogateescape').splitlines()
    starts = [i for i, line in enumerate(lines) if re.match(r'^nonmatching\s+\S+', line)]
    items = []
    for index, start in enumerate(starts):
        block = lines[start:starts[index + 1] if index + 1 < len(starts) else len(lines)]
        labels = [re.fullmatch(r'\s*dlabel\s+([^\s,]+)(?:,\s*visibility=(global|local))?\s*', line)
                  for line in block]
        labels = [label for label in labels if label]
        addresses = [re.match(r'^\s*/\* (?:(?:[0-9A-F]{4,6}) )?([0-9A-F]{8})', line) for line in block]
        addresses = [int(a[1], 16) for a in addresses if a]
        if not labels or not addresses:
            raise CorrectionError('original scaffold contains an unbounded storage item')
        items.append((labels, addresses[0]))
    found = [(labels, lo, items[i + 1][1] if i + 1 < len(items) else int(piece['end'], 16))
             for i, (labels, lo) in enumerate(items) if any(label[1] == name for label in labels)]
    if (len(found) != 1 or found[0][1:] != (address, address + size) or
            len([label for label in found[0][0] if label[1] == name and
                 (0 if label[2] == 'local' else 1) == binding]) != 1):
        raise CorrectionError('original scaffold name/binding/complete item extent differs')


def _file_scope_storage(repo, records, object_raw, carves, owner, original, assembly_raw):
    """Verify original storage identities/raw bytes and their compiler/carve offsets and order."""
    _, original_raw = _artifact(repo, original, 'file-scope original ELF')
    import importlib.util
    module_name = 'xeno_file_scope_elfinfo'
    spec = importlib.util.spec_from_file_location(module_name, repo / 'tools/tu/elfinfo.py')
    module = importlib.util.module_from_spec(spec)
    sys.modules[module_name] = module
    spec.loader.exec_module(module)
    elf, obj = module.Elf(original_raw), module.Elf(object_raw)
    compiler_labels, app = set(), False
    for line in assembly_raw.decode('utf-8', 'surrogateescape').splitlines():
        if line.strip() == '#APP':
            app = True
        elif line.strip() == '#NO_APP':
            app = False
        elif not app:
            label = re.match(r'^([A-Za-z_.$][\w.$]*):(?:\s|$)', line)
            if label:
                compiler_labels.add(label[1])
    fields = {'name', 'section', 'address', 'size', 'binding', 'type', 'object_offset',
              'raw_sha256', 'original_symbol', 'extent_evidence', 'scaffold'}
    if not isinstance(records, list):
        raise CorrectionError('file-scope storage inventory is absent')
    seen, spans, object_spans = set(), {}, {}
    for record in records:
        if (not isinstance(record, dict) or set(record) != fields or record['name'] in seen or
                any(type(record[k]) is not int for k in ('address', 'size', 'binding', 'type', 'object_offset')) or
                record['size'] <= 0 or record['object_offset'] < 0 or record['type'] != 1):
            raise CorrectionError('file-scope storage identity/extent inventory is invalid')
        seen.add(record['name'])
        if record['name'] not in compiler_labels:
            raise CorrectionError('storage label is not emitted by the compiler outside assembly scaffolding')
        section, address, size = record['section'], record['address'], record['size']
        window = (owner.get('data_ownership', {}).get(section) or {}).get('window') or []
        sections = [s for s in elf.sections if s.name == section and s.addr <= address and
                    address + size <= s.addr + s.size]
        if (len(window) != 2 or not int(window[0], 16) <= address < address + size <= int(window[1], 16) or
                len(sections) != 1 or not record['extent_evidence']):
            raise CorrectionError('file-scope storage lacks original owner/range/extent proof')
        for pin in record['extent_evidence']:
            _artifact(repo, pin, 'original storage identity/extent evidence')
        _file_scope_original_item(repo, record['scaffold'], owner, original, section,
                                  record['name'], address, size, record['binding'])
        originals = [s for s in elf.symbols if s.value == address and s.type == 1 and s.name]
        identity = record['original_symbol']
        if identity is None:
            if originals or record['name'] != f'D_{address:08X}' or record['binding'] != 1:
                raise CorrectionError('fallback storage identity conflicts with original named storage')
        elif (not isinstance(identity, dict) or set(identity) != {'name', 'binding', 'type', 'size'} or
              identity['name'] != record['name'] or identity['binding'] != record['binding'] or
              identity['type'] != record['type'] or
              len([s for s in originals if s.name == identity['name'] and s.bind == identity['binding'] and
                   s.type == identity['type'] and s.size == identity['size'] and
                   elf.sections[s.shndx].name == section]) != 1 or
              (identity['size'] and identity['size'] != size)):
            raise CorrectionError('original named storage identity/binding/type/extent differs')
        symbols = [s for s in obj.symbols if s.name == record['name'] and s.shndx < len(obj.sections)]
        if len(symbols) != 1:
            raise CorrectionError('compiled storage is missing or repeated')
        symbol = symbols[0]
        emitted = obj.sections[symbol.shndx]
        if (symbol.type != record['type'] or symbol.bind != record['binding'] or symbol.size != size or
                symbol.value != record['object_offset'] or emitted.name != section or
                not 0 <= symbol.value < symbol.value + size <= emitted.size):
            raise CorrectionError('compiled storage binding/type/extent/section/order differs')
        original_bytes = (b'\0' * size if sections[0].type == 8 else
                          elf.section_bytes(sections[0])[address - sections[0].addr:address - sections[0].addr + size])
        object_bytes = (b'\0' * size if emitted.type == 8 else
                        obj.section_bytes(emitted)[symbol.value:symbol.value + size])
        if original_bytes != object_bytes or digest(original_bytes) != record['raw_sha256']:
            raise CorrectionError('compiled storage bytes differ from exact original bytes')
        matches = [span for run in carves.get('tus', {}).get(owner['id'], {}).get(section, [])
                   for span in run.get('c_input_spans', []) if record['name'] in span.get('symbols', [])]
        if (len(matches) != 1 or matches[0].get('section') != section or
                [int(v, 16) for v in matches[0].get('range', [])] != [address, address + size] or
                matches[0].get('object_range') != [symbol.value, symbol.value + size]):
            raise CorrectionError('normal storage carve does not bind exact original and compiler extents')
        for table, lo in ((spans, address), (object_spans, symbol.value)):
            if any(lo < hi and start < lo + size for start, hi in table.setdefault(section, [])):
                raise CorrectionError('storage inventory overlaps original or compiler extents')
            table[section].append((lo, lo + size))
    return seen


def _file_scope_is_storage(item):
    if item['kind'] != 'decl':
        return False
    text = lexical(item['text']).strip()
    if re.match(r'(?:typedef|extern)\b', text) or not any(k[0] == 'ordinary' for k in item['keys']):
        return False
    return '=' in text or not re.search(r'\b[A-Za-z_]\w*\s*\([^;{}]*\)\s*;\s*$', text)


def _validate_file_scope_correction(repo, flat, receipt, verification, owner, tu_build, base, post):
    operation = flat['operation']
    if owner['unit'] != 'main':
        raise CorrectionError('normal file-scope operation currently supports MAIN TU declaration/data ownership')
    if set(operation) != {'schema', 'owner_tu', 'revision', 'original_tu_sha256',
                          'author_source', 'allocated_functions', 'regions', 'storage', 'compilation'}:
        raise CorrectionError('normal file-scope operation has unexpected fields')
    _file_scope_author(repo, flat, owner)
    proof = file_scope_delta(base, post, operation['regions'])
    prior_storage = [item for item in proof['old_items'] if _file_scope_is_storage(item)]
    current_storage = [item for item in proof['new_items'] if _file_scope_is_storage(item)]
    prior_texts = [item['text'] for item in prior_storage]
    if ([item['text'] for item in current_storage if item['text'] in prior_texts] != prior_texts or
            any(sum(item['text'] == raw for item in current_storage) != 1 for raw in prior_texts)):
        raise CorrectionError('file-scope correction changes accepted storage definitions or their source order')
    _, raw = _artifact(repo, operation['author_source'], 'immutable authored source')
    roundtrip = _file_scope_roundtrip(post, raw.decode('utf-8', 'surrogateescape'),
                                     operation['allocated_functions'], owner)
    _, review_raw = _artifact(repo, flat['review'], 'file-scope independent review')
    if json.loads(review_raw).get('pins', {}).get('authored_source') != operation['author_source']:
        raise CorrectionError('independent review does not pin exact authored body source')
    original = tu_build['units'][owner['unit']]
    original_ref = dict(path=original['file'], sha256=original['sha256'])
    object_raw, carves, assembly = _file_scope_compilation(repo, operation['compilation'], owner,
                                                        digest(post.encode('utf-8', 'surrogateescape')), original_ref)
    storage = _file_scope_storage(repo, operation['storage'], object_raw, carves, owner, original_ref, assembly)
    old_texts = {i['text'] for i in proof['old_items']}
    old_keys = set().union(*(set(tuple(k) for k in i['keys']) for i in proof['old_items']))
    after_keys, witnessed = set(), set()
    defined_storage = set()
    for item in proof['new_items']:
        keys = set(tuple(k) for k in item['keys'])
        after_keys.update(keys)
        if _file_scope_is_storage(item):
            defined_storage.update(k[1] for k in keys if k[0] == 'ordinary')
        if item['text'] in old_texts or item['kind'] == 'comment':
            continue
        covered = set().union(*[_file_scope_witness(repo, ref, owner, tu_build, verification)
                               for ref in item['witnesses']])
        # Storage has its own stronger original-byte, emitted-symbol and carve proof.
        covered.update(('ordinary', name) for name in storage)
        if not keys or not keys.issubset(covered):
            raise CorrectionError('new/changed file-scope item lacks exact independently reviewed witnesses')
        witnessed.update(covered)
    if not old_keys.issubset(after_keys | witnessed):
        raise CorrectionError('file-scope correction removes an unaccounted declaration identity')
    if storage != defined_storage:
        raise CorrectionError('storage proof does not exhaustively match every file-scope definition')
    receipt.update(kind='declaration_only', verification_tus=verification,
                   function_bodies_changed=[], exact_listed_delta=True,
                   baseline_function_blocks=proof['baseline_function_blocks'],
                   storage_verified=sorted(storage), **roundtrip)
    return receipt


def _declaration_witness(repo, ref, owner, tu_build, verification=()):
    """Check original identities behind a reviewer-pinned declaration decision."""
    _, raw = _artifact(repo, ref, 'declaration witness')
    witness = json.loads(raw)
    if witness.get('schema') == 'tu-declaration-evidence/1':
        if not isinstance(witness.get('witnesses'), list) or not witness['witnesses']:
            raise CorrectionError('declaration evidence index has no original witnesses')
        return set().union(*[_declaration_witness_record(repo, row, owner, tu_build, verification)
                             for row in witness['witnesses']])
    return _declaration_witness_record(repo, witness, owner, tu_build, verification)


def _declaration_witness_record(repo, witness, owner, tu_build, verification):
    """One exact original witness, standalone or inside a hash-pinned index."""
    if not isinstance(witness, dict):
        raise CorrectionError('declaration witness is not an object')
    original = tu_build['units'][owner['unit']]
    if (witness.get('schema') != 'tu-declaration-witness/1' or
            witness.get('owner_tu') != owner['id'] or
            witness.get('original_elf') != {'path': original['file'], 'sha256': original['sha256']}):
        raise CorrectionError('declaration witness has a different owner/original identity')
    _, original_bytes = _artifact(repo, witness['original_elf'], 'declaration original ELF')
    import importlib.util
    module_name = 'xeno_declaration_elfinfo'
    spec = importlib.util.spec_from_file_location(module_name, repo / 'tools/tu/elfinfo.py')
    module = importlib.util.module_from_spec(spec)
    sys.modules[module_name] = module
    spec.loader.exec_module(module)
    elf = module.Elf(original_bytes)
    kind = witness.get('kind')
    if kind == 'symbol':
        identity = witness.get('symbol') or {}
        matches = [s for s in elf.symbols if s.name == identity.get('name') and
                   s.value == identity.get('address') and s.bind == identity.get('binding') and
                   s.type == identity.get('type') and s.size == identity.get('size') and
                   s.shndx < len(elf.sections) and elf.sections[s.shndx].name == identity.get('section')]
        if len(matches) != 1:
            raise CorrectionError('declaration witness symbol identity differs from original ELF')
        symbol = matches[0]
        if symbol.type == 2:
            if not any(f['name'] == symbol.name and int(f['va'], 16) == symbol.value
                       for f in owner['functions']):
                raise CorrectionError('declaration function witness is outside its mapped owner TU')
        else:
            span = witness.get('extent') or []
            section = elf.sections[symbol.shndx]
            window = (owner.get('data_ownership', {}).get(section.name) or {}).get('window') or []
            boundary = min([s.value for s in elf.symbols if s.shndx == symbol.shndx and
                            s.value > symbol.value] + [section.addr + section.size])
            if (len(span) != 2 or span[0] != symbol.value or len(window) != 2 or
                    not int(window[0], 16) <= span[0] < span[1] <= int(window[1], 16) or
                    not span[0] < span[1] <= boundary or
                    (symbol.size and span[1] - span[0] != symbol.size) or
                    (not symbol.size and (span[1] != boundary or not witness.get('extent_evidence')))):
                raise CorrectionError('declaration witness lacks an exact original storage extent')
            for pin in witness.get('extent_evidence') or []:
                _artifact(repo, pin, 'original storage extent evidence')
        return {('ordinary', symbol.name), ('macro', symbol.name)}
    if kind != 'layout' or not witness.get('type_name') or not witness.get('fields'):
        raise CorrectionError('declaration witness kind is unsupported or lacks original field offsets')
    extent = witness.get('extent')
    if not isinstance(extent, int) or extent <= 0 or not witness.get('original_instructions'):
        raise CorrectionError('layout correction lacks original extent/offset instruction witnesses')
    for field in witness['fields']:
        if (set(field) != {'name', 'offset', 'size'} or not field['name'] or
                not isinstance(field['offset'], int) or not isinstance(field['size'], int) or
                not 0 <= field['offset'] < field['offset'] + field['size'] <= extent):
            raise CorrectionError('layout witness field is outside its pinned extent')
    for instruction in witness['original_instructions']:
        address, expected = instruction['address'], instruction['bytes']
        witness_tu = instruction.get('tu', owner['id'])
        instruction_owner = next((t for t in tu_build['tus'] if t['id'] == witness_tu), None)
        if (not instruction_owner or instruction_owner['unit'] != owner['unit'] or
                (witness_tu != owner['id'] and witness_tu not in verification) or
                not int(instruction_owner['text']['start'], 16) <= address < address + 4 <=
                    int(instruction_owner['text']['end'], 16)):
            raise CorrectionError('layout instruction witness is outside its original owner TU')
        sections = [s for s in elf.sections if s.type != 8 and s.addr <= address and
                    address + 4 <= s.addr + s.size]
        if len(sections) != 1 or elf.section_bytes(sections[0])[
                address - sections[0].addr:address - sections[0].addr + 4].hex() != expected:
            raise CorrectionError('layout witness instruction differs from original bytes')
    return {('ordinary', witness['type_name']), ('tag', witness['type_name'])}


def validate_declaration_correction(repo, row):
    """Use existing independent-review pins, then constrain the exact declaration delta."""
    flat, _ = _correction_row(row)
    flat = dict(flat)
    flat.pop('kind', None)
    verification = flat.pop('verification_tus', None)
    receipt = validate_correction(repo, flat)
    operation = flat['operation']
    fields = {'schema', 'owner_tu', 'revision', 'original_tu_sha256', 'changes'}
    file_scope = isinstance(operation, dict) and operation.get('schema') == FILE_SCOPE_OPERATION
    if not isinstance(operation, dict) or (not file_scope and (
            set(operation) != fields or operation.get('schema') != 'tu-reviewed-declaration-delta/1')):
        raise CorrectionError('declaration correction operation has an invalid schema')
    tu_build = json.loads((repo / 'config/tu-build.json').read_text())
    owner = next((t for t in tu_build['tus'] if t['id'] == operation['owner_tu']), None)
    if (not owner or not owner.get('in_scope') or owner.get('category') != 'game' or
            operation['revision'] != tu_build['revision'] or
            operation['original_tu_sha256'] != owner['text']['sha256'] or
            flat['path'] not in (owner['path'], str(Path(owner['path']).with_suffix('.h'))) or
            not isinstance(verification, list) or not verification or
            len(verification) != len(set(verification)) or owner['id'] not in verification or
            not set(flat['affected_tus']).issubset(verification)):
        raise CorrectionError('declaration correction owner, byte identity, path or verification scope differs')
    base = _copy_bytes(repo, flat['base']['copy'], flat['base']['sha256'], 'declaration base').decode('utf-8', 'surrogateescape')
    post = _copy_bytes(repo, flat['result']['copy'], flat['result']['sha256'], 'declaration postimage').decode('utf-8', 'surrogateescape')
    if file_scope:
        if flat['path'] != owner['path']:
            raise CorrectionError('normal file-scope operation requires its defining TU source')
        return _validate_file_scope_correction(repo, flat, receipt, verification, owner, tu_build, base, post)
    declaration_delta(base, post, operation['changes'])
    for change in operation['changes']:
        witnessed = set().union(*[_declaration_witness(repo, ref, owner, tu_build, verification)
                                  for ref in change['witnesses']])
        if not set(tuple(key) for key in change['before_keys'] + change['after_keys']).issubset(witnessed):
            raise CorrectionError('declaration witnesses do not cover every changed declaration key')
    receipt.update(kind='declaration_only', verification_tus=verification, function_bodies_changed=[])
    return receipt


def validate_declaration_config(repo, report_row):
    """Pin only reviewed RSSD owner rows to their paired declaration postimages."""
    flat, _ = _correction_row(report_row)
    flat = dict(flat)
    flat.pop('kind', None)
    verification = flat.pop('verification_tus', None)
    phase = flat.pop('application_phase', None)
    receipt = validate_correction(repo, flat, declaration_config=True)
    operation = flat['operation']
    if (not isinstance(operation, dict) or set(operation) != {'schema', 'entities'} or
            operation.get('schema') != 'tu-reviewed-owner-config-delta/1' or
            not isinstance(operation.get('entities'), list) or not operation['entities'] or
            phase != 'post_source' or not isinstance(verification, list) or not verification or
            len(verification) != len(set(verification)) or
            not set(flat['affected_tus']).issubset(verification)):
        raise CorrectionError('owner config correction lacks atomic post-source scope or verification')
    before = json.loads(_copy_bytes(repo, flat['base']['copy'], flat['base']['sha256'], 'owner config base'))
    post_bytes = _copy_bytes(repo, flat['result']['copy'], flat['result']['sha256'], 'owner config postimage')
    after = json.loads(post_bytes)
    expected = json.loads(json.dumps(before))
    tu_build = json.loads((repo / 'config/tu-build.json').read_text())
    import importlib.util
    name = 'xeno_declaration_header_harvest'
    spec = importlib.util.spec_from_file_location(name, repo / 'tools/header_harvest.py')
    harvest = importlib.util.module_from_spec(spec)
    sys.modules[name] = harvest
    spec.loader.exec_module(harvest)
    seen, pairs, sources = set(), [], []
    for entity in operation['entities']:
        if (set(entity) != {'namespace', 'name', 'owner_tu', 'header_row', 'source',
                           'before_row_sha256', 'after_row_sha256'} or
                entity['name'] not in {'RssdRpcResponse', 'RssdWorkFlags', 'RssdWork'} or
                (entity['namespace'], entity['name']) in seen):
            raise CorrectionError('owner config correction contains an unscoped or repeated entity')
        seen.add((entity['namespace'], entity['name']))
        matches = [r for r in expected['migrations'] if
                   (r.get('namespace'), r.get('name')) == (entity['namespace'], entity['name'])]
        if len(matches) != 1:
            raise CorrectionError('owner migration entity is not unique')
        migration = matches[0]
        canonical = lambda value: digest(json.dumps(value, sort_keys=True, separators=(',', ':'),
                                                    ensure_ascii=False).encode())
        if canonical(migration) != entity['before_row_sha256']:
            raise CorrectionError('prior owner migration row differs from its exact pin')
        header = json.loads(_artifact(repo, entity['header_row'], 'paired declaration row')[1])
        proof = validate_reviewed_source_correction(repo, header)
        owner = next((t for t in tu_build['tus'] if t['id'] == entity['owner_tu']), None)
        if (not owner or (header.get('correction') or {}).get('kind') != 'declaration_only' or
                proof['operation']['owner_tu'] != owner['id'] or
                header['path'] != str(Path(owner['path']).with_suffix('.h')) or
                not set(proof['verification_tus']).issubset(verification)):
            raise CorrectionError('config entity lacks its exact reviewed defining-header correction')
        text = _copy_bytes(repo, header['result']['copy'], header['result']['sha256'], 'paired header').decode()
        declarations = [d for d in harvest.scan_declarations(text) if
                        (d.namespace, d.name) == (entity['namespace'], entity['name'])]
        source = entity['source']
        if (len(declarations) != 1 or set(source) != {'path', 'copy', 'sha256'} or
                source['path'] != owner['path']):
            raise CorrectionError('config entity is absent from its postimage or has a different source owner')
        source_text = _copy_bytes(repo, source['copy'], source['sha256'], 'paired owner source').decode('utf-8', 'surrogateescape')
        if entity['name'] == 'RssdWork':
            definitions = [i for i in scan(source_text) if i.kind == 'decl' and
                           ('ordinary', 'RssdWork') in decl_keys(i.text(source_text)) and
                           '=' in lexical(i.text(source_text))]
            if len(definitions) != 1 or owner['id'] != 'main/tu107':
                raise CorrectionError('RssdWork owner is not its actual mapped C data definition')
        elif owner['id'] != 'main/tu110':
            raise CorrectionError('RSSD type entity is outside its reviewed defining TU')
        migration['canon'] = harvest.normalize_ws(declarations[0].canon)
        migration['owner'] = dict(migration['owner'], tu=owner['id'], path=header['path'],
            source_sha256=header['result']['sha256'], tu_source_sha256=source['sha256'],
            tu_record_sha256=canonical(owner))
        if canonical(migration) != entity['after_row_sha256']:
            raise CorrectionError('owner migration postimage differs from its paired exact entity')
        pairs.append(header)
        sources.append(source)
    paired_paths = {header['path'] for header in pairs}
    required_entities = {(row.get('namespace'), row.get('name')) for row in before['migrations']
                         if (row.get('owner') or {}).get('path') in paired_paths}
    if not required_entities.issubset(seen):
        raise CorrectionError('atomic config omits another entity pinned to the changed owner header')
    if after != expected:
        raise CorrectionError('owner config correction changes unlisted rows or historical provenance')
    receipt.update(kind='declaration_owner_config', verification_tus=verification,
                   application_phase=phase, prior_config_sha256=flat['base']['sha256'],
                   paired_headers=pairs, owner_sources=sources)
    return receipt


def apply_declaration_config(repo_root, project_root, row):
    repo, project = Path(repo_root).resolve(), Path(project_root).resolve()
    receipt = validate_declaration_config(repo, row)
    for header in receipt['paired_headers']:
        if digest((project / header['path']).read_bytes()) != header['result']['sha256']:
            raise CorrectionError('atomic config application lacks its exact header postimage')
    for source in receipt['owner_sources']:
        if digest((project / source['path']).read_bytes()) != source['sha256']:
            raise CorrectionError('atomic config application lacks its exact defining source postimage')
    target = project / row['path']
    current = digest(target.read_bytes())
    if current not in (row['base']['sha256'], row['result']['sha256']):
        raise CorrectionError('atomic owner config has a third, unreviewed hash')
    if current != row['result']['sha256']:
        out = _copy_bytes(repo, row['result']['copy'], row['result']['sha256'], 'atomic owner config')
        with tempfile.NamedTemporaryFile(prefix=f'.{target.name}.', dir=target.parent, delete=False) as temp:
            temp.write(out)
            temp.flush()
            os.fsync(temp.fileno())
            temporary = Path(temp.name)
        os.chmod(temporary, target.stat().st_mode & 0o777)
        os.replace(temporary, target)
    return receipt


def reviewed_declaration_view(repo_root, manifest_path, tu_id, project=None):
    """Validate an opt-in correction manifest and return read-only baseline postimages."""
    repo = Path(repo_root).resolve()
    relative = Path(manifest_path)
    if relative.is_absolute():
        relative = relative.resolve().relative_to(repo)
    if '..' in relative.parts:
        raise CorrectionError('declaration manifest escapes the repository')
    path = repo / relative
    manifest = json.loads(path.read_text())
    if manifest.get('schema') != 'reviewed-source-corrections/1' or not manifest.get('corrections'):
        raise CorrectionError('declaration manifest has an invalid schema')
    views, receipts, rows = {}, [], []
    for row in manifest['corrections']:
        correction = row.get('correction') or {}
        if correction.get('kind') not in ('declaration_only', 'declaration_owner_config', 'owner_source', 'owner_export') or tu_id not in (correction.get('affected_tus') or []):
            raise CorrectionError('worker declaration manifest contains an unrelated correction')
        config = correction['kind'] == 'declaration_owner_config'
        receipt = validate_declaration_config(repo, row) if config else validate_reviewed_source_correction(repo, row)
        if correction.get('kind') == 'owner_source' and 'export_declarations' not in receipt:
            raise CorrectionError('worker owner source input lacks a validated defining header receipt')
        if row['path'] in views or (correction.get('kind') != 'owner_export' and
                digest((repo / row['path']).read_bytes()) != row['base']['sha256']):
            raise CorrectionError('declaration correction path repeats or its current baseline moved')
        post = _copy_bytes(repo, row['result']['copy'], row['result']['sha256'], 'declaration view').decode('utf-8', 'surrogateescape')
        if correction.get('kind') == 'owner_export':
            post = (repo / row['path']).read_text(encoding='utf-8', errors='surrogateescape')
        if project is not None and not config:
            if correction.get('kind') == 'owner_export':
                retain_export_owner_source(repo, project, row)
                views[row['path']] = (Path(project) / row['path']).read_text(
                    encoding='utf-8', errors='surrogateescape')
                receipts.append(receipt)
                rows.append(row)
                continue
            authored = (Path(project) / row['path']).read_text(encoding='utf-8', errors='surrogateescape')
            operation = correction['operation']
            if correction.get('kind') == 'owner_source':
                if sha256_text(authored) != row['result']['sha256']:
                    raise CorrectionError('authored owner header differs from the exact reviewed postimage')
            elif operation.get('schema') == FILE_SCOPE_OPERATION:
                if sha256_text(authored) != operation['author_source']['sha256']:
                    raise CorrectionError('authored source differs from its independently reviewed body pin')
                tu_build = json.loads((repo / 'config/tu-build.json').read_text())
                owner = next(t for t in tu_build['tus'] if t['id'] == operation['owner_tu'])
                _file_scope_roundtrip(post, authored, operation['allocated_functions'], owner)
            else:
                authored_items, _ = prelude_items(TU(authored).prelude())
                for change in operation['changes']:
                    if change['after_text'] and TU(authored).prelude().count(change['after_text']) != 1:
                        raise CorrectionError('authored source lacks the exact reviewed declaration postimage')
                    if change['before_text'] and any(item == change['before_text'] for _, item in authored_items):
                        raise CorrectionError('authored source retains the superseded declaration')
        views[row['path']] = post
        receipts.append(receipt)
        rows.append(row)
    for row in rows:
        if (row.get('correction') or {}).get('kind') != 'declaration_owner_config':
            continue
        receipt = validate_declaration_config(repo, row)
        for pair in receipt['paired_headers']:
            if pair not in rows:
                raise CorrectionError('atomic config manifest omits its exact paired declaration row')
    return dict(views=views, rows=rows, receipt=dict(schema='tu-reviewed-declaration-stage/1',
                target_tu=tu_id, manifest_path=relative.as_posix(), manifest_sha256=digest(path.read_bytes()),
                corrections=receipts))


def apply_correction(repo: Path, root: Path, row: dict) -> dict:
    """Apply exact before->after correction; exact after is idempotent; else fail."""
    _guard_canonical_write(root / row['path'])
    kind = ((row.get('correction') or {}).get('kind') or row.get('kind')) if isinstance(row, dict) else None
    validator = (validate_correction if kind == 'owner_source' else
                 validate_declaration_correction if kind == 'declaration_only' or
                 (isinstance(row.get('operation'), dict) and
                  row['operation'].get('schema') in ('tu-reviewed-declaration-delta/1', FILE_SCOPE_OPERATION)) else
                 validate_pair_correction if kind == 'owner_pair' or 'pair_manifest' in row else
                 validate_layout_correction if 'owner_manifest' in row else validate_correction)
    receipt = validator(repo, row)
    target = (root / _safe_destination(row['path'])).resolve()
    try:
        target.relative_to(root.resolve())
    except ValueError as exc:
        raise CorrectionError('destination escapes publication root') from exc
    current = target.read_bytes()
    current_sha = digest(current)
    if current_sha == receipt['before_sha256']:
        out = _copy_bytes(repo, row['result']['copy'], receipt['after_sha256'], 'postimage')
        if digest(out) != receipt['after_sha256']:
            raise CorrectionError('postimage changed after validation')
        target.parent.mkdir(parents=True, exist_ok=True)
        with tempfile.NamedTemporaryFile(prefix=f'.{target.name}.', dir=target.parent, delete=False) as temp:
            temp.write(out)
            temp.flush()
            os.fsync(temp.fileno())
            temp_path = Path(temp.name)
        os.chmod(temp_path, target.stat().st_mode & 0o777)
        os.replace(temp_path, target)
        outcome = 'applied'
    elif current_sha == receipt['after_sha256']:
        outcome = 'already_applied'
    else:
        raise CorrectionError(f'{row["path"]}: third hash {current_sha} is neither pinned before nor after')
    receipt.update(current_before_sha256=current_sha, outcome=outcome,
                   result_sha256=digest(target.read_bytes()))
    return receipt


def validate_layout_correction(repo: Path, row: dict) -> dict:
    """Validate a publication row against a reviewed owner-layout manifest.

    This is separate from the TU156 source-patch receipt. The layout reviewer
    approves one exact tagged typedef edit; the generated diff is only a
    transport for those already-pinned source bytes.
    """
    row, metadata = _correction_row(row)
    required = {
        'role', 'id', 'path', 'action', 'base', 'result', 'patch', 'owner_manifest',
        'owner_manifest_row', 'owner_manifest_task', 'owner_manifest_submission_id',
        'review', 'proposal', 'evidence', 'proposal_author', 'reviewer', 'affected_tus',
        'zero_credit', 'operation',
    }
    if set(row) != required or row.get('role') != 'owner_source_correction' or \
            row.get('action') != 'patch' or row.get('zero_credit') is not True:
        raise CorrectionError('owner-layout correction row has unexpected fields')
    _safe_destination(row['path'])
    if row['proposal_author'] == row['reviewer']:
        raise CorrectionError('owner-layout author and reviewer must be distinct')
    _, manifest_bytes = _artifact(repo, row['owner_manifest'], 'owner manifest')
    review_path, review_bytes = _artifact(repo, row['review'], 'owner-layout review')
    review = json.loads(review_bytes)
    manifest = json.loads(manifest_bytes)
    if manifest.get('schema') != 'tu-reviewed-owner-proposals/1' or \
            not isinstance(manifest.get('proposals'), list):
        raise CorrectionError('owner manifest schema is unsupported')
    matches = [p for p in manifest['proposals'] if p.get('id') == row['owner_manifest_row']]
    if len(matches) != 1:
        raise CorrectionError('owner manifest row id is absent or duplicated')
    owner = matches[0]
    if owner.get('owner_path') != row['path'] or owner.get('review_owner') != row['proposal_author']:
        raise CorrectionError('owner manifest destination or author differs from correction row')
    if row['operation'] != {
            'owner_tu': owner.get('owner_tu'), 'owner_type': owner.get('owner_type'),
            'field': owner.get('field'), 'offset': owner.get('offset'),
            'before_member': owner.get('before_member'), 'after_member': owner.get('after_member'),
            'layout_bytes': owner.get('layout_bytes')}:
        raise CorrectionError('operation differs from exact reviewed owner-layout row')
    if row['review']['path'] != owner.get('review_path') or \
            row['review']['sha256'] != owner.get('review_sha256'):
        raise CorrectionError('correction review reference differs from the owner manifest')
    if row['proposal'] != {'path': owner.get('proposal_path'), 'sha256': owner.get('proposal_sha256')} or \
            row['evidence'] != {'path': owner.get('evidence_manifest_path'),
                                'sha256': owner.get('evidence_manifest_sha256')}:
        raise CorrectionError('proposal/evidence refs differ from the exact owner manifest row')
    _verify_submission(repo / '.work/queue.sqlite3', row['owner_manifest_task'],
                       row['owner_manifest_submission_id'], row['owner_manifest']['path'],
                       row['proposal_author'])
    if review.get('schema') != 'private-tu-owner-proposal-review/1' or \
            review.get('reviewer') != row['reviewer']:
        raise CorrectionError('owner-layout reviewer identity or schema differs')
    if review.get('status') != 'supported-as-narrow-layout-evidence-not-a-published-header-or-function-acceptance':
        raise CorrectionError('owner-layout review does not support this narrow proposal')
    _verify_reviewer_task(repo / '.work/queue.sqlite3', row['reviewer'], review_path)
    identities = review.get('identities') or {}
    expected = {
        'owner_base': (row['path'], row['base']['sha256']),
        'proposal': (owner.get('proposal_path'), owner.get('proposal_sha256')),
        'evidence': (owner.get('evidence_manifest_path'), owner.get('evidence_manifest_sha256')),
    }
    for label, (path, digest_value) in expected.items():
        actual = identities.get(label) or {}
        if (actual.get('path'), actual.get('sha256')) != (path, digest_value):
            raise CorrectionError(f'owner-layout review does not pin exact {label}')
    if owner.get('base_sha256') != row['base']['sha256'] or \
            owner.get('proposal_sha256') != row['result']['sha256']:
        raise CorrectionError('owner manifest before/after hashes differ from correction row')
    if owner.get('owner_type') not in json.dumps(review) or owner.get('field') not in json.dumps(review) or \
            owner.get('offset') not in json.dumps(review):
        raise CorrectionError('review does not discuss the exact field and offset')
    if not review.get('neighbor_assertions') or not review.get('assembly_evidence'):
        raise CorrectionError('owner-layout review lacks neighbor or assembly evidence')
    base = _copy_bytes(repo, row['base']['copy'], row['base']['sha256'], 'owner correction base')
    result = _copy_bytes(repo, row['result']['copy'], row['result']['sha256'], 'owner correction result')
    _, proposal = _artifact(repo, {'path': owner['proposal_path'],
                                   'sha256': owner['proposal_sha256']}, 'owner proposal')
    if result != proposal:
        raise CorrectionError('published correction result is not the exact reviewed proposal bytes')
    # Only the reviewed complete tagged type may differ. This prevents owner
    # layout rows from smuggling unrelated source or neighboring function edits.
    import re
    tag = re.escape(owner['owner_type'])
    block = re.compile(r'(?ms)^typedef\s+struct\s+' + tag + r'\s*\{.*?^\}\s*' + tag + r'\s*;')
    before_text = base.decode('utf-8', 'surrogateescape')
    after_text = result.decode('utf-8', 'surrogateescape')
    before_blocks, after_blocks = list(block.finditer(before_text)), list(block.finditer(after_text))
    if len(before_blocks) != 1 or len(after_blocks) != 1:
        raise CorrectionError('reviewed owner typedef is not unique in exact before/after files')
    b, a = before_blocks[0], after_blocks[0]
    if before_text[:b.start()] != after_text[:a.start()] or \
            before_text[b.end():] != after_text[a.end():]:
        raise CorrectionError('owner correction changes bytes outside the reviewed typedef')
    _, patch = _artifact(repo, row['patch'], 'owner correction patch')
    # Verify generated transport diff against the exact before/postimage.
    with tempfile.TemporaryDirectory(prefix='owner-layout-correction-') as tmp:
        work = Path(tmp)
        target = work / row['path']
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_bytes(base)
        patch_path, _ = _artifact(repo, row['patch'], 'patch')
        check = subprocess.run(['git', 'apply', '--check', str(patch_path)],
                               cwd=work, stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=False)
        apply = subprocess.run(['git', 'apply', str(patch_path)],
                               cwd=work, stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=False)
        if check.returncode or apply.returncode or target.read_bytes() != result:
            raise CorrectionError('owner patch transport does not reproduce exact reviewed source bytes')
    return dict(path=row['path'], before_sha256=digest(base), after_sha256=digest(result),
                proposal_author=row['proposal_author'], reviewer=row['reviewer'],
                owner_manifest_sha256=digest(manifest_bytes), review_sha256=digest(review_bytes),
                patch_sha256=digest(patch), affected_tus=list(row['affected_tus']),
                zero_credit=True, operation=row['operation'])


def validate_pair_correction(repo: Path, row: dict) -> dict:
    """Validate one member of an independently reviewed owner-source pair."""
    row, _ = _correction_row(row)
    required = {
        'role', 'id', 'path', 'action', 'base', 'result', 'patch', 'proposal', 'evidence',
        'proposal_task', 'proposal_submission_id',
        'pair_manifest', 'pair_row_owner_tu', 'pair_task', 'pair_submission_id',
        'proposal_author', 'reviewer', 'review', 'review_task', 'review_submission_id',
        'affected_tus', 'verification_tus',
        'zero_credit', 'operation',
    }
    if set(row) != required or row.get('role') != 'owner_source_correction' or \
            row.get('action') != 'patch' or row.get('zero_credit') is not True:
        raise CorrectionError('owner-pair correction has unexpected fields or is not zero-credit')
    _safe_destination(row['path'])
    if row['proposal_author'] == row['reviewer']:
        raise CorrectionError('owner-pair author and reviewer must be distinct')
    for key in ('affected_tus', 'verification_tus'):
        values = row[key]
        if not isinstance(values, list) or not values or len(set(values)) != len(values):
            raise CorrectionError(f'{key} must be a nonempty unique list')
    if not set(row['affected_tus']).issubset(row['verification_tus']):
        raise CorrectionError('every candidate consumer must also be rebuilt and verified')

    _, manifest_bytes = _artifact(repo, row['pair_manifest'], 'owner-pair manifest')
    manifest = json.loads(manifest_bytes)
    if manifest.get('schema') != 'private-owner-composability-patch-pair/1' or \
            manifest.get('credit') != 'zero; owner and header corrections only':
        raise CorrectionError('owner-pair manifest schema or credit declaration is unsupported')
    if manifest.get('author') != row['proposal_author']:
        raise CorrectionError('owner-pair manifest author differs from the correction row')
    members = [p for p in manifest.get('patches') or []
               if p.get('owner_tu') == row['pair_row_owner_tu'] and p.get('path') == row['path']]
    if len(members) != 1:
        raise CorrectionError('owner-pair entry is absent or duplicated')
    member = members[0]
    base = _copy_bytes(repo, row['base']['copy'], row['base']['sha256'], 'pair base')
    result = _copy_bytes(repo, row['result']['copy'], row['result']['sha256'], 'pair postimage')
    patch_path, patch_bytes = _artifact(repo, row['patch'], 'pair patch')
    if (row['base']['sha256'] != member.get('before_sha256') or
            row['result']['sha256'] != member.get('after_sha256') or
            row['patch']['path'] != member.get('patch_artifact') or
            row['patch']['sha256'] != member.get('patch_sha256') or
            digest(patch_bytes) != member.get('patch_sha256')):
        raise CorrectionError('correction row does not match its exact manifest member')
    if (row['proposal'] != row['pair_manifest'] or row['proposal_task'] != row['pair_task'] or
            row['proposal_submission_id'] != row['pair_submission_id']):
        raise CorrectionError('generic proposal identity does not alias the exact pair submission')
    old_paths = [line[4:] for line in patch_bytes.decode('utf-8', 'replace').splitlines()
                 if line.startswith('--- ')]
    new_paths = [line[4:] for line in patch_bytes.decode('utf-8', 'replace').splitlines()
                 if line.startswith('+++ ')]
    if old_paths != ['a/' + row['path']] or new_paths != ['b/' + row['path']]:
        raise CorrectionError('pair patch must contain exactly one exact-path file delta')
    with tempfile.TemporaryDirectory(prefix='owner-pair-correction-') as tmp:
        work = Path(tmp)
        target = work / row['path']
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_bytes(base)
        check = subprocess.run(['git', 'apply', '--check', str(patch_path)], cwd=work,
                               stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=False)
        apply = subprocess.run(['git', 'apply', str(patch_path)], cwd=work,
                               stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=False)
        if check.returncode or apply.returncode or target.read_bytes() != result:
            raise CorrectionError('pair patch does not reproduce its exact reviewed postimage')

    review_path, review_bytes = _artifact(repo, row['review'], 'independent pair review')
    review = json.loads(review_bytes)
    if (review.get('schema') != 'owner-source-pair-review/1' or
            review.get('decision') != 'evidence_supported' or
            review.get('reviewer') != row['reviewer']):
        raise CorrectionError('independent pair review identity or decision is unsupported')
    submission = review.get('submission') or {}
    if (submission.get('task') != row['pair_task'] or
            submission.get('submission_id') != row['pair_submission_id'] or
            submission.get('author') != row['proposal_author'] or
            submission.get('artifact') != row['pair_manifest']['path'] or
            submission.get('artifact_sha256') != digest(manifest_bytes)):
        raise CorrectionError('independent review does not pin the exact pair submission')
    if row['evidence'] != {'path': submission.get('evidence'),
                           'sha256': submission.get('evidence_sha256')}:
        raise CorrectionError('generic evidence reference differs from pair review receipt')
    _verify_submission(repo / '.work/queue.sqlite3', row['pair_task'], row['pair_submission_id'],
                       row['pair_manifest']['path'], row['proposal_author'])
    if row['review_task'] != review.get('review_task'):
        raise CorrectionError('review task identity differs from the pinned review report')
    _verify_submission(repo / '.work/queue.sqlite3', review.get('review_task'),
                       row['review_submission_id'], row['review']['path'], row['reviewer'])
    reviewed_rows = review.get('corrections') or []
    reviewed = [r for r in reviewed_rows if r.get('owner_tu') == member.get('owner_tu')
                and r.get('path') == member.get('path')]
    if len(reviewed) != 1:
        raise CorrectionError('independent review omits or duplicates this pair member')
    accepted = reviewed[0]
    if (accepted.get('patch') != member.get('patch_artifact') or
            accepted.get('patch_sha256') != member.get('patch_sha256') or
            accepted.get('before_sha256') != member.get('before_sha256') or
            accepted.get('after_sha256') != member.get('after_sha256')):
        raise CorrectionError('independent review does not bind the exact patch transformation')
    if row['pair_row_owner_tu'] == 'main/tu110':
        semantic = member.get('independent_owner_review') or {}
        if semantic.get('path') not in json.dumps(review) or semantic.get('sha256') not in json.dumps(review):
            raise CorrectionError('pair review omits the exact semantic owner review')
        _, semantic_bytes = _artifact(repo, semantic, 'semantic owner review')
        semantic_report = json.loads(semantic_bytes)
        if (semantic_report.get('schema') != 'private-tu-owner-proposal-review/1' or
                semantic_report.get('status') !=
                'supported-as-narrow-layout-evidence-not-a-published-header-or-function-acceptance'):
            raise CorrectionError('semantic owner review does not support the layout proposal')
    elif row['pair_row_owner_tu'] == 'main/tu114':
        materialization = member.get('independent_materialization_receipt') or {}
        if (materialization.get('path') not in json.dumps(review) or
                materialization.get('sha256') not in json.dumps(review) or
                materialization.get('before_sha256') != member.get('before_sha256') or
                materialization.get('after_sha256') != member.get('after_sha256')):
            raise CorrectionError('pair review omits the exact TU114 materialization receipt')
        _, material_bytes = _artifact(repo, {'path': materialization.get('path'),
                                             'sha256': materialization.get('sha256')},
                                      'TU114 materialization receipt')
        material_report = json.loads(material_bytes)
        if (material_report.get('schema') != 'private-tu-owner-route-check/1' or
                not (material_report.get('result') or {}).get('passed')):
            raise CorrectionError('TU114 materialization receipt is not supported')
    else:
        raise CorrectionError('owner-pair route is closed to unreviewed owner TU identities')
    verification = list((manifest.get('required_rebuild_and_audit_set') or {}).get('direct_header_consumers') or [])
    if (sorted(row['verification_tus']) != sorted(verification) or
            (manifest.get('required_rebuild_and_audit_set') or {}).get('whole_linked_target_gate') != ['main']):
        raise CorrectionError('verification set differs from the independently reviewed pair')
    expected_operation = ({'owner_tu': 'main/tu110', 'field': member.get('field'),
                           'offset': member.get('offset')}
                          if row['pair_row_owner_tu'] == 'main/tu110' else
                          {'owner_tu': 'main/tu114', 'change': member.get('change')})
    if row['operation'] != expected_operation:
        raise CorrectionError('operation differs from the exact pair member')
    safety = manifest.get('safety_and_scope') or {}
    if safety.get('function_bodies_changed') != [] or safety.get('new_c_credit') != 0:
        raise CorrectionError('owner-pair manifest does not preserve zero-credit function scope')
    return dict(path=row['path'], before_sha256=digest(base), after_sha256=digest(result),
                pair_manifest_sha256=digest(manifest_bytes), review_sha256=digest(review_bytes),
                patch_sha256=digest(patch_bytes), proposal_author=row['proposal_author'],
                reviewer=row['reviewer'], affected_tus=list(row['affected_tus']),
                verification_tus=list(row['verification_tus']), zero_credit=True,
                operation=row['operation'])


def apply_reviewed_source_correction(repo_root, project_root, report_row):
    _guard_canonical_write(Path(project_root) / report_row['path'])
    row = dict(report_row)
    correction = row.pop('correction', None)
    if isinstance(correction, dict) and correction.get('schema') == 'user-authorized-source-correction/1':
        return apply_user_authorized_source_correction(Path(repo_root).resolve(),
                                                       Path(project_root).resolve(), row, correction)
    if not isinstance(correction, dict) or correction.get('schema') != 'reviewed-source-correction/1':
        raise CorrectionError('report row has no reviewed-source-correction/1 metadata')
    kind = correction.get('kind')
    if kind == 'owner_export':
        return retain_export_owner_source(Path(repo_root).resolve(), Path(project_root).resolve(), report_row)
    if kind not in ('owner_source', 'owner_layout', 'owner_pair', 'declaration_only'):
        raise CorrectionError('correction kind is not yet supported by this bounded route')
    row.update({key: value for key, value in correction.items() if key not in ('schema', 'kind')})
    if kind == 'owner_source':
        row['kind'] = kind
    return apply_correction(Path(repo_root).resolve(), Path(project_root).resolve(), row)


def validate_reviewed_source_correction(repo_root, report_row):
    row = dict(report_row)
    correction = row.pop('correction', None)
    if isinstance(correction, dict) and correction.get('schema') == 'user-authorized-source-correction/1':
        return validate_user_authorized_source_correction(Path(repo_root).resolve(), row, correction)
    if not isinstance(correction, dict) or correction.get('schema') != 'reviewed-source-correction/1':
        raise CorrectionError('report row has no reviewed-source-correction/1 metadata')
    kind = correction.get('kind')
    if kind == 'owner_export':
        return validate_owner_export(Path(repo_root).resolve(), report_row)
    if kind not in ('owner_source', 'owner_layout', 'owner_pair', 'declaration_only'):
        raise CorrectionError('correction kind is not yet supported by this bounded route')
    row.update({key: value for key, value in correction.items() if key not in ('schema', 'kind')})
    if kind == 'owner_source':
        row['kind'] = kind
    validator = (validate_declaration_correction if kind == 'declaration_only' else
                 validate_pair_correction if kind == 'owner_pair' else
                 validate_layout_correction if kind == 'owner_layout' else validate_correction)
    return validator(Path(repo_root).resolve(), row)


def _user_correction_inputs(repo: Path, row: dict, correction: dict):
    if correction.get('kind') == 'block_scope_declaration_only':
        return _validate_user_block_scope_declaration_correction(repo, row, correction)
    if correction.get('kind') == 'declaration_only':
        return _validate_user_declaration_correction(repo, row, correction)
    if correction.get('kind') == 'qualifier_cast_only':
        return _validate_user_qualifier_cast_correction(repo, row, correction)
    required = {'role', 'id', 'path', 'action', 'base', 'result', 'patch'}
    if set(row) != required or row.get('role') != 'owner_source_correction' or row.get('action') != 'patch' or \
            row.get('path') != 'src/ov10/cgp.c' or correction.get('kind') != 'ov10_cgpcursorposition_owner_prelude':
        raise CorrectionError('user-authorized source correction row has unexpected scope or fields')
    if correction.get('zero_credit') is not True or correction.get('owner_tu') != 'ov10/tu008':
        raise CorrectionError('user-authorized correction is not the bounded zero-credit OV10/tu008 operation')
    for name in ('authority', 'proposal', 'evidence', 'application_manifest', 'composer_result'):
        _artifact(repo, correction.get(name), f'user correction {name}')
    for label in ('base', 'result'):
        spec = row.get(label) or {}
        if set(spec) != {'sha256', 'copy'}:
            raise CorrectionError(f'user correction {label} copy pin has unexpected fields')
        _copy_bytes(repo, spec['copy'], spec['sha256'], f'user correction {label}')
    patch_path, patch_bytes = _artifact(repo, row['patch'], 'user correction patch')
    patch = json.loads(patch_bytes)
    result_ref = correction['composer_result']
    _, result_bytes = _artifact(repo, result_ref, 'compose11 result')
    result = json.loads(result_bytes)
    proposal = json.loads(_artifact(repo, correction['proposal'], 'owner proposal')[1])
    evidence = json.loads(_artifact(repo, correction['evidence'], 'owner evidence')[1])
    app = json.loads(_artifact(repo, correction['application_manifest'], 'application manifest')[1])
    authority = json.loads(_artifact(repo, correction['authority'], 'user authority')[1])
    if (result.get('schema') != 'ov10-tu008-compose11-private-result/1' or
            result.get('status') != 'composed_pending_user_authority_stamp_and_live_gate' or
            result.get('artifact_hashes', {}).get('source.c') != correction.get('composed_source_sha256')):
        raise CorrectionError('compose11 result or source hash does not match the user correction row')
    if (proposal.get('schema') != 'ov10-owner-declaration-move-proposal/1' or
            proposal.get('owner_tu') != correction['owner_tu'] or
            proposal.get('owner_path') != row['path'] or
            proposal.get('published_base_sha256') != row['base']['sha256'] or
            proposal.get('authority', {}).get('status') != 'user_directed_zero_credit_refinement' or
            proposal.get('review', {}).get('status') != 'not_requested_by_user' or
            proposal.get('scope', {}).get('function_bodies_changed') != [] or
            proposal.get('scope', {}).get('new_c_credit') != 0):
        raise CorrectionError('owner proposal is not the bounded user-directed zero-credit declaration operation')
    if (evidence.get('schema') != 'ov10-cgpcursorposition-layout-evidence/1' or
            evidence.get('owner_tu') != correction['owner_tu'] or
            evidence.get('owner_path') != row['path'] or
            evidence.get('published_source_sha256') != row['base']['sha256']):
        raise CorrectionError('layout evidence identity differs from the pinned owner source')
    expected = app.get('expected_replay') or {}
    if (app.get('schema') != 'ov10-user-directed-source-correction-application/1' or
            app.get('owner_tu') != correction['owner_tu'] or app.get('owner_path') != row['path'] or
            app.get('status') != 'composed_replay_verified_waiting_for_user_authority_metadata_and_live_gate' or
            expected.get('source_sha256') != correction['composed_source_sha256'] or
            sorted(expected.get('function_patch_sids') or []) != sorted(correction.get('function_patch_sids') or []) or
            app.get('authority_artifact') is not None):
        raise CorrectionError('application manifest does not pin this exact no-review composition')
    frozen = authority.get('scope') or {}
    policy = authority.get('integration_policy') or {}
    if (authority.get('schema') != 'user-direct-acceptance-authority/1' or
            authority.get('authority') != 'explicit human instruction in the current root conversation' or
            not authority.get('user_instruction_verbatim') or
            frozen.get('frozen_index_sha256') != 'ec6c5675d4cdf7821a2daa94c672177c3799155e122a8aff51d9f0730320817c' or
            frozen.get('function_count') != 938 or frozen.get('new_recovery_outside_frozen_scope') is not False or
            authority.get('independent_review') != 'waived by the user; no review has been performed by this record' or
            policy.get('private_integrator_preflight') is not False or
            policy.get('post_integration_review') is not False or
            policy.get('live_publisher_byte_comparison_and_tu_audit') != 'required' or
            correction['authority'].get('sha256') != '7b4e8d276062c4c93cddb1ed259de2115df5b1c8e42c33e03a52c1161792e83c' or
            correction.get('owner_tu') != 'ov10/tu008' or
            correction['composer_result'].get('sha256') != 'afcf23c40768530a003a50fe49802f5cae6a657d4f4df5f960c7bb2cb47d4e41'):
        raise CorrectionError('pinned authority stamp does not authorize this exact correction under the user waiver')
    if result.get('owner_correction', {}).get('patch') != patch:
        raise CorrectionError('correction patch differs from immutable compose11 result')
    base = _copy_bytes(repo, row['base']['copy'], row['base']['sha256'], 'user correction base')
    post = _copy_bytes(repo, row['result']['copy'], row['result']['sha256'], 'user correction postimage')
    if digest(base) != proposal['published_base_sha256'] or digest(post) != result['owner_correction']['corrected_sha256']:
        raise CorrectionError('exact before/after source bytes differ from proposal or composer result pins')
    if not patch.get('schema') == 'tu-patch/1' or patch.get('functions') or \
            set(patch.get('interstitial') or {}) != {'CGPCMSub'}:
        raise CorrectionError('owner correction must change only the prelude and CGPCMSub declarations')
    base_text, post_text = base.decode('utf-8', 'surrogateescape'), post.decode('utf-8', 'surrogateescape')
    base_tu, post_tu = TU(base_text, row['path']), TU(post_text, row['path'])
    applied = apply_patch(base_tu, base_tu, patch).get('text')
    if applied is None or applied.encode('utf-8', 'surrogateescape') != post:
        raise CorrectionError('owner correction patch does not exactly reproduce its pinned postimage')
    before = {b.name: base_tu.block_text(b) for b in base_tu.functions()}
    after = {b.name: post_tu.block_text(b) for b in post_tu.functions()}
    if before != after:
        raise CorrectionError('user-directed zero-credit owner correction changes a function body')
    protected = proposal['scope'].get('function_block_sha256') or {}
    if protected.get('CGPCMSub') != digest(post_tu.block_text(post_tu.by_name['CGPCMSub']).encode('utf-8', 'surrogateescape')):
        raise CorrectionError('the protected CGPCMSub function block changed during the declaration correction')
    return dict(id=row['id'], kind=correction['kind'], owner_tu=correction['owner_tu'],
                path=row['path'], before_sha256=digest(base), after_sha256=digest(post),
                zero_credit=True, authority_sha256=correction['authority']['sha256'],
                composer_result_sha256=correction['composer_result']['sha256'],
                proposal_sha256=correction['proposal']['sha256'], evidence_sha256=correction['evidence']['sha256'],
                application_manifest_sha256=correction['application_manifest']['sha256'],
                function_patch_sids=list(correction['function_patch_sids']),
                function_bodies_changed=[], protected_function='CGPCMSub', protected_function_unchanged=True,
                operation='apply declared owner refinement before the exact composed function patch')


def _validate_user_declaration_correction(repo: Path, row: dict, correction: dict):
    required = {'role', 'id', 'path', 'action', 'base', 'result', 'patch'}
    if (set(row) != required or row.get('role') != 'owner_source_correction' or
            row.get('action') != 'patch' or correction.get('zero_credit') is not True or
            correction.get('function_bodies_changed') != [] or
            not isinstance(correction.get('changed_declarations'), list) or
            not correction['changed_declarations'] or not correction.get('operation') or
            not re.fullmatch(r'(main|ov01|ov02|ov10|ov11|ov12)/tu[0-9]{3}',
                             str(correction.get('owner_tu') or ''))):
        raise CorrectionError('user declaration correction has unexpected scope or is not zero-credit')
    tu_build = json.loads((repo / 'config/tu-build.json').read_text(encoding='utf-8'))
    tu_record = next((x for x in tu_build.get('tus', []) if x.get('id') == correction['owner_tu']), None)
    unit, tu_name = correction['owner_tu'].split('/')
    if not tu_record or not tu_record.get('in_scope') or tu_record.get('category') != 'game':
        raise CorrectionError('declaration correction owner TU is outside scoped game TUs')
    tu_stem = Path(tu_record['path']).stem
    owner_headers = {f'include/{unit}/{tu_stem}.h', f'src/{unit}/{tu_stem}.h'}
    if row.get('path') != tu_record['path'] and row.get('path') not in owner_headers:
        raise CorrectionError('declaration correction path is not the TU source or its generated public header')
    frozen_bytes = (repo / '.work/reworks/orq-rework940-20260929/luna-46/index.json').read_bytes()
    if digest(frozen_bytes) != 'ec6c5675d4cdf7821a2daa94c672177c3799155e122a8aff51d9f0730320817c':
        raise CorrectionError('frozen938 index hash changed')
    frozen_index = json.loads(frozen_bytes)
    if not any((item.get('identity') or {}).get('tu') == correction['owner_tu']
               for item in frozen_index.get('identities', [])):
        raise CorrectionError('declaration correction owner TU is absent from pinned frozen938 scope')
    refs = {}
    for name in ('authority', 'proposal', 'evidence', 'application_manifest'):
        ref = correction.get(name)
        _, data = _artifact(repo, ref, f'declaration correction {name}')
        refs[name] = json.loads(data) if name != 'authority' else json.loads(data)
    authority = refs['authority']
    frozen, policy = authority.get('scope') or {}, authority.get('integration_policy') or {}
    if (correction['authority'].get('sha256') !=
            '7b4e8d276062c4c93cddb1ed259de2115df5b1c8e42c33e03a52c1161792e83c' or
            authority.get('schema') != 'user-direct-acceptance-authority/1' or
            authority.get('recorded_by') != 'orq-review940-20260929' or
            frozen.get('frozen_index_sha256') !=
            'ec6c5675d4cdf7821a2daa94c672177c3799155e122a8aff51d9f0730320817c' or
            frozen.get('function_count') != 938 or frozen.get('new_recovery_outside_frozen_scope') is not False or
            policy.get('private_integrator_preflight') is not False or
            policy.get('post_integration_review') is not False or
            policy.get('live_publisher_byte_comparison_and_tu_audit') != 'required'):
        raise CorrectionError('declaration correction is not bound to the pinned user waiver authority')
    for label in ('base', 'result'):
        spec = row.get(label) or {}
        if set(spec) != {'sha256', 'copy'}:
            raise CorrectionError(f'declaration correction {label} source copy pin is malformed')
    base = _copy_bytes(repo, row['base']['copy'], row['base']['sha256'], 'declaration correction base')
    post = _copy_bytes(repo, row['result']['copy'], row['result']['sha256'], 'declaration correction postimage')
    if (correction.get('base_sha256') != digest(base) or correction.get('result_sha256') != digest(post) or
            correction.get('owner_tu') is None):
        raise CorrectionError('declaration correction source bytes differ from their immutable pins')
    patch_path, patch_bytes = _artifact(repo, row['patch'], 'declaration correction patch')
    patch = json.loads(patch_bytes)
    if (patch.get('schema') != 'tu-patch/1' or patch.get('tu') != row['path'] or
            patch.get('base_sha256') != digest(base) or patch.get('functions') or
            set(patch) - {'schema', 'tu', 'base_sha256', 'prelude', 'functions', 'interstitial'}):
        raise CorrectionError('declaration correction patch is not prelude/interstitial-only')
    before_tu = TU(base.decode('utf-8', 'surrogateescape'), row['path'])
    after_tu = TU(post.decode('utf-8', 'surrogateescape'), row['path'])
    # This exact user-authorized correction may remove old declarations;
    # ordinary candidate merges keep their additive-prelude restrictions.
    applied = patch_to_theirs(before_tu, patch).text
    if applied is None or applied.encode('utf-8', 'surrogateescape') != post:
        raise CorrectionError('declaration correction patch does not replay to its pinned postimage')
    if row['path'] == tu_record['path']:
        owner_source = base
        if correction.get('owner_source_sha256') != digest(owner_source):
            raise CorrectionError('owner source pin differs from the TU source correction base')
        owner_source_after = post
    else:
        owner_source_ref = correction.get('owner_source') or {}
        owner_source_path, owner_source = _artifact(repo, owner_source_ref, 'owner TU source')
        if (owner_source_path.relative_to(repo.resolve()).as_posix() != tu_record['path'] or
                owner_source_ref.get('sha256') != correction.get('owner_source_sha256')):
            raise CorrectionError('owner-header correction does not pin the mapped TU source')
        owner_source_after = owner_source
    source_tu = TU(owner_source.decode('utf-8', 'surrogateescape'), tu_record['path'])
    before_blocks = {fn.name: digest(source_tu.block_text(fn).encode('utf-8', 'surrogateescape'))
                     for fn in source_tu.functions()}
    source_tu_after = TU(owner_source_after.decode('utf-8', 'surrogateescape'), tu_record['path'])
    after_blocks = {fn.name: digest(source_tu_after.block_text(fn).encode('utf-8', 'surrogateescape'))
                    for fn in source_tu_after.functions()}
    if before_blocks != after_blocks or correction.get('function_block_sha256') != before_blocks:
        raise CorrectionError('declaration correction changed or failed to pin every mapped function block')
    return dict(id=row['id'], kind='declaration_only', owner_tu=correction['owner_tu'], path=row['path'],
                before_sha256=digest(base), after_sha256=digest(post), patch_sha256=digest(patch_bytes),
                zero_credit=True, authority_sha256=correction['authority']['sha256'],
                proposal_sha256=correction['proposal']['sha256'], evidence_sha256=correction['evidence']['sha256'],
                application_manifest_sha256=correction['application_manifest']['sha256'],
                function_bodies_changed=[], changed_declarations=list(correction['changed_declarations']),
                function_block_sha256=before_blocks, operation=correction['operation'])


def _qualifier_cast_edits():
    return [
        dict(function='CardPlayCommandPlay', callee='xglMatrixTrans', argument_index=2,
             before='xglMatrixTrans(matrix, matrix, translation);',
             after='xglMatrixTrans(matrix, (const float (*)[4])matrix, translation);'),
        dict(function='CardPlayCommandPlay', callee='xglMatrixScale', argument_index=2,
             before='xglMatrixScale(matrix, matrix, scale);',
             after='xglMatrixScale(matrix, (const float (*)[4])matrix, scale);'),
    ]


def _replace_qualifier_casts(text: str, side: str, path: str) -> str:
    """Apply or normalize the one pinned pair of old-GCC array-qualifier casts."""
    if side not in ('before', 'after'):
        raise CorrectionError('qualifier cast operation side must be before or after')
    tu = TU(text, path)
    block = tu.by_name.get('CardPlayCommandPlay')
    if block is None or block.state != 'c' or block.role != 'function':
        raise CorrectionError('qualifier-cast owner is not the mapped C CardPlayCommandPlay function')
    old_block = tu.block_text(block)
    replacements = []
    for edit in _qualifier_cast_edits():
        old, new = edit['before'], edit['after']
        source, target = (old, new) if side == 'before' else (new, old)
        if old_block.count(source) != 1:
            raise CorrectionError(f"CardPlayCommandPlay must contain one exact {edit['callee']} {side} call")
        replacements.append((source, target))
    new_block = old_block
    for source, target in replacements:
        new_block = new_block.replace(source, target, 1)
    return text[:block.start] + new_block + text[block.end:]


def _validate_user_qualifier_cast_correction(repo: Path, row: dict, correction: dict):
    """Validate the narrowly authorized OV10 matrix qualifier-only adjustment.

    It deliberately reports CardPlayCommandPlay as a raw source-body change.
    Zero credit is established only after removing these two exact casts and
    proving the complete normalized function block unchanged.
    """
    required_row = {'role', 'id', 'path', 'action', 'base', 'result', 'patch'}
    required_correction = {
        'schema', 'kind', 'owner_tu', 'application_phase', 'authority', 'proposal',
        'evidence', 'application_manifest', 'base_sha256', 'result_sha256',
        'owner_source_sha256', 'function_block_sha256', 'result_function_block_sha256',
        'normalized_function_body_sha256_before', 'normalized_function_body_sha256_after',
        'function_bodies_changed', 'changed_declarations', 'operation', 'zero_credit',
        'candidate_source', 'composed_candidate', 'preserved_candidate_functions',
        'source_task',
    }
    if (set(row) != required_row or row.get('role') != 'owner_source_correction' or
            row.get('action') != 'patch' or set(correction) != required_correction or
            correction.get('schema') != 'user-authorized-source-correction/1' or
            correction.get('kind') != 'qualifier_cast_only' or
            correction.get('owner_tu') != 'ov10/tu003' or
            correction.get('application_phase') != 'pre_source' or
            correction.get('zero_credit') is not True or
            correction.get('function_bodies_changed') != ['CardPlayCommandPlay'] or
            correction.get('changed_declarations') != [] or
            correction.get('operation') != _qualifier_cast_edits()):
        raise CorrectionError('qualifier-cast correction has unexpected scope or operation')

    tu_build = json.loads((repo / 'config/tu-build.json').read_text(encoding='utf-8'))
    tu_record = next((entry for entry in tu_build.get('tus', []) if entry.get('id') == 'ov10/tu003'), None)
    if (not tu_record or not tu_record.get('in_scope') or tu_record.get('category') != 'game' or
            row.get('path') != tu_record.get('path') or
            row.get('path') != 'src/ov10/ccu_038_exec_sub.c'):
        raise CorrectionError('qualifier-cast correction is not the mapped in-scope OV10/tu003 owner')

    frozen_bytes = (repo / '.work/reworks/orq-rework940-20260929/luna-46/index.json').read_bytes()
    if digest(frozen_bytes) != 'ec6c5675d4cdf7821a2daa94c672177c3799155e122a8aff51d9f0730320817c':
        raise CorrectionError('frozen938 index hash changed')
    frozen_index = json.loads(frozen_bytes)
    if not any((entry.get('identity') or {}).get('tu') == 'ov10/tu003'
               for entry in frozen_index.get('identities', [])):
        raise CorrectionError('qualifier-cast owner TU is absent from frozen938 scope')

    authority_ref = correction.get('authority')
    authority_path, authority_bytes = _artifact(repo, authority_ref, 'qualifier-cast user authority')
    authority = json.loads(authority_bytes)
    frozen_scope, policy = authority.get('scope') or {}, authority.get('integration_policy') or {}
    if (authority_ref.get('sha256') != '7b4e8d276062c4c93cddb1ed259de2115df5b1c8e42c33e03a52c1161792e83c' or
            authority.get('schema') != 'user-direct-acceptance-authority/1' or
            authority.get('recorded_by') != 'orq-review940-20260929' or
            frozen_scope.get('frozen_index_sha256') != 'ec6c5675d4cdf7821a2daa94c672177c3799155e122a8aff51d9f0730320817c' or
            frozen_scope.get('function_count') != 938 or
            frozen_scope.get('new_recovery_outside_frozen_scope') is not False or
            policy.get('private_integrator_preflight') is not False or
            policy.get('post_integration_review') is not False or
            policy.get('live_publisher_byte_comparison_and_tu_audit') != 'required'):
        raise CorrectionError('qualifier-cast correction is not bound to the pinned user authority')

    refs = {}
    for name in ('proposal', 'evidence', 'application_manifest'):
        _path, data = _artifact(repo, correction[name], f'qualifier-cast {name}')
        refs[name] = json.loads(data)
    proposal, evidence, application = refs['proposal'], refs['evidence'], refs['application_manifest']
    if (proposal.get('schema') != 'user-authorized-qualifier-cast-proposal/1' or
            proposal.get('owner_tu') != 'ov10/tu003' or proposal.get('owner_path') != row['path'] or
            proposal.get('base_sha256') != row['base'].get('sha256') or
            proposal.get('result_sha256') != row['result'].get('sha256') or
            proposal.get('operation') != correction['operation'] or
            proposal.get('scope', {}).get('new_c_credit') != 0 or
            proposal.get('scope', {}).get('normalized_function_bodies_changed') != []):
        raise CorrectionError('qualifier-cast proposal does not pin the exact zero-credit scope')
    if (evidence.get('schema') != 'ov10-cardplay-matrix-qualifier-cast-evidence/1' or
            evidence.get('owner_tu') != 'ov10/tu003' or evidence.get('owner_path') != row['path']):
        raise CorrectionError('qualifier-cast evidence has the wrong owner identity')
    expected = application.get('expected_replay') or {}
    if (application.get('schema') != 'ov10-user-authorized-qualifier-cast-application/1' or
            application.get('owner_tu') != 'ov10/tu003' or application.get('owner_path') != row['path'] or
            application.get('application_phase') != 'pre_source' or
            expected.get('owner_postimage_sha256') != correction.get('result_sha256') or
            expected.get('composed_candidate_sha256') != correction.get('composed_candidate', {}).get('sha256') or
            expected.get('normalized_candidate_sha256') != correction.get('candidate_source', {}).get('sha256') or
            expected.get('body_hashes_unchanged') is not True or
            expected.get('whole_file_gate_and_tu_audit') != 'required on actual live publisher build'):
        raise CorrectionError('qualifier-cast application manifest does not pin the exact replay')
    if (evidence.get('audit_report') is None or evidence.get('candidate_source') != correction.get('candidate_source') or
            evidence.get('composed_candidate') != correction.get('composed_candidate')):
        raise CorrectionError('qualifier-cast evidence does not pin the cited audit and candidate sources')

    for label in ('base', 'result'):
        spec = row.get(label) or {}
        if set(spec) != {'sha256', 'copy'}:
            raise CorrectionError(f'qualifier-cast {label} copy pin is malformed')
        _copy_bytes(repo, spec['copy'], spec['sha256'], f'qualifier-cast {label}')
    base = _copy_bytes(repo, row['base']['copy'], row['base']['sha256'], 'qualifier-cast base')
    post = _copy_bytes(repo, row['result']['copy'], row['result']['sha256'], 'qualifier-cast postimage')
    if (correction.get('base_sha256') != digest(base) or correction.get('result_sha256') != digest(post) or
            correction.get('owner_source_sha256') != digest(base)):
        raise CorrectionError('qualifier-cast correction bytes do not match their owner source pins')
    base_text = base.decode('utf-8', 'surrogateescape')
    post_text = post.decode('utf-8', 'surrogateescape')
    expected_post = _replace_qualifier_casts(base_text, 'before', row['path'])
    if expected_post.encode('utf-8', 'surrogateescape') != post:
        raise CorrectionError('qualifier-cast postimage contains bytes beyond the exact two call casts')
    if _replace_qualifier_casts(post_text, 'after', row['path']).encode('utf-8', 'surrogateescape') != base:
        raise CorrectionError('qualifier-cast normalization does not reproduce the complete base source')

    patch_path, patch_bytes = _artifact(repo, row['patch'], 'qualifier-cast patch')
    patch = json.loads(patch_bytes)
    if (patch.get('schema') != 'tu-patch/1' or patch.get('tu') != row['path'] or
            patch.get('base_sha256') != digest(base) or
            set(patch.get('functions') or {}) != {'CardPlayCommandPlay'} or
            patch.get('prelude') != {'add': [], 'remove': []} or patch.get('interstitial') != {} or
            set(patch) - {'schema', 'tu', 'base_sha256', 'prelude', 'functions', 'interstitial'}):
        raise CorrectionError('qualifier-cast patch is not the exact single-function patch')
    before_tu = TU(base_text, row['path'])
    after_tu = TU(post_text, row['path'])
    replay = patch_to_theirs(before_tu, patch).text
    if replay is None or replay.encode('utf-8', 'surrogateescape') != post:
        raise CorrectionError('qualifier-cast function patch does not replay to the pinned postimage')
    before_blocks = {block.name: digest(before_tu.block_text(block).encode('utf-8', 'surrogateescape'))
                     for block in before_tu.functions()}
    after_blocks = {block.name: digest(after_tu.block_text(block).encode('utf-8', 'surrogateescape'))
                    for block in after_tu.functions()}
    if (correction.get('function_block_sha256') != before_blocks or
            correction.get('result_function_block_sha256') != after_blocks or
            set(before_blocks) != set(after_blocks) or
            [name for name in before_blocks if before_blocks[name] != after_blocks[name]] != ['CardPlayCommandPlay']):
        raise CorrectionError('qualifier-cast source changed function blocks outside the named caller')
    normalized_before = before_tu.block_text(before_tu.by_name['CardPlayCommandPlay'])
    normalized_after = after_tu.block_text(after_tu.by_name['CardPlayCommandPlay'])
    normalized_after_tu_text = _replace_qualifier_casts(post_text, 'after', row['path'])
    normalized_after_tu = TU(normalized_after_tu_text, row['path'])
    normalized_after = normalized_after_tu.block_text(normalized_after_tu.by_name['CardPlayCommandPlay'])
    normalized_before_hash = digest(normalized_before.encode('utf-8', 'surrogateescape'))
    normalized_after_hash = digest(normalized_after.encode('utf-8', 'surrogateescape'))
    normalized = {'CardPlayCommandPlay': normalized_before_hash}
    normalized_result = {'CardPlayCommandPlay': normalized_after_hash}
    if (normalized_before_hash != normalized_after_hash or
            correction.get('normalized_function_body_sha256_before') != normalized or
            correction.get('normalized_function_body_sha256_after') != normalized_result):
        raise CorrectionError('qualifier-cast normalization changed the complete caller function block')

    candidate_path, candidate_bytes = _artifact(repo, correction['candidate_source'], 'submitted candidate source')
    composed_path, composed_bytes = _artifact(repo, correction['composed_candidate'], 'cast-corrected candidate source')
    candidate_text = candidate_bytes.decode('utf-8', 'surrogateescape')
    composed_text = composed_bytes.decode('utf-8', 'surrogateescape')
    expected_composed = _replace_qualifier_casts(candidate_text, 'before', row['path'])
    if expected_composed.encode('utf-8', 'surrogateescape') != composed_bytes:
        raise CorrectionError('composed candidate differs beyond the two explicit casts')
    if _replace_qualifier_casts(composed_text, 'after', row['path']).encode('utf-8', 'surrogateescape') != candidate_bytes:
        raise CorrectionError('composed candidate normalization does not reproduce its submitted source')
    candidate_tu = TU(candidate_text, row['path'])
    composed_tu = TU(composed_text, row['path'])
    candidate_blocks = {block.name: digest(candidate_tu.block_text(block).encode('utf-8', 'surrogateescape'))
                        for block in candidate_tu.functions()}
    composed_blocks = {block.name: digest(composed_tu.block_text(block).encode('utf-8', 'surrogateescape'))
                       for block in composed_tu.functions()}
    if [name for name in candidate_blocks if candidate_blocks[name] != composed_blocks.get(name)] != ['CardPlayCommandPlay']:
        raise CorrectionError('composed candidate changes bodies beyond CardPlayCommandPlay')
    preserved = correction.get('preserved_candidate_functions')
    if (not isinstance(preserved, dict) or set(preserved) != {
            'CCC06ExecSub', 'CCC07ExecSub', 'CCC10ExecSub', 'CCC30ExecSub', 'CCC31ExecSub',
            'CCU062ExecSub', 'CardPlayCommandBattleExecute', 'CardPlayCommandCheck'} or
            any(candidate_blocks.get(name) != digest_value or composed_blocks.get(name) != digest_value
                for name, digest_value in preserved.items())):
        raise CorrectionError('the eight original author function blocks are not byte-identical in the composition')

    source_task = correction.get('source_task') or {}
    task_id, owner, submission_id = (source_task.get('task_id'), source_task.get('owner'),
                                    source_task.get('submission_id'))
    source_result_ref = source_task.get('result') or {}
    if (set(source_task) != {'task_id', 'owner', 'submission_id', 'result', 'allocated_functions',
                             'direct_candidate_id', 'author_source_sha256'} or
            source_task.get('allocated_functions') != list(preserved) or
            not task_id or not owner or not submission_id or set(source_result_ref) != {'path', 'sha256'}):
        raise CorrectionError('original source task provenance is malformed')
    result_path, result_bytes = _artifact(repo, source_result_ref, 'original source result')
    result = json.loads(result_bytes)
    if result.get('task_id') != task_id:
        raise CorrectionError('original source result identity differs from the pinned queue task')
    _verify_submission(repo / '.work/queue.sqlite3', task_id, submission_id,
                       str(result_path), owner)
    result_function_names = {name for item in result.get('functions', [])
                             for name in (item.get('original_names') or [])}
    if not set(preserved).issubset(result_function_names):
        raise CorrectionError('original source result does not contain all eight allocated functions')
    author_source_maps = []
    for item in result.get('functions', []):
        names = item.get('original_names') or []
        if set(names) & set(preserved):
            artifacts = item.get('candidate_artifacts') or []
            if not artifacts:
                raise CorrectionError('original source result omits an allocated function candidate artifact')
            found = False
            for artifact in artifacts:
                source_path = Path(artifact.get('path') or '').resolve()
                try:
                    source_path.relative_to(repo.resolve())
                except ValueError as exc:
                    raise CorrectionError('original source candidate path escapes the repository') from exc
                source_bytes = source_path.read_bytes()
                if digest(source_bytes) != artifact.get('sha256'):
                    raise CorrectionError('original source candidate artifact digest changed')
                if artifact.get('sha256') == source_task.get('author_source_sha256'):
                    author_tu = TU(source_bytes.decode('utf-8', 'surrogateescape'), row['path'])
                    author_source_maps.append({
                        name: digest(author_tu.block_text(author_tu.by_name[name]).encode('utf-8', 'surrogateescape'))
                        for name in names if name in preserved
                    })
                    found = True
            if not found:
                raise CorrectionError('original source result candidate artifact does not match its pinned author source')
    author_blocks = {}
    for block_map in author_source_maps:
        author_blocks.update(block_map)
    if any(author_blocks.get(name) != preserved[name] for name in preserved):
        raise CorrectionError('allocated candidate function bodies differ from the immutable author candidate')
    audit_ref = evidence.get('audit_report') or {}
    _audit_path, audit_bytes = _artifact(repo, audit_ref, 'live OV10 TU audit report')
    audit = json.loads(audit_bytes)
    messages = audit.get('toolchain_messages') or []
    if not isinstance(messages, list) or not all(
            any(callee in str(message) and 'arg 2' in str(message) for message in messages)
            for callee in ('xglMatrixTrans', 'xglMatrixScale')):
        raise CorrectionError('live audit does not contain both exact matrix qualifier warnings')
    direct_ref = evidence.get('direct_acceptance_report') or {}
    _direct_path, direct_bytes = _artifact(repo, direct_ref, 'direct acceptance provenance')
    direct = json.loads(direct_bytes)
    direct_candidate = next((item for item in direct.get('candidates', [])
                             if item.get('id') == source_task['direct_candidate_id']), None)
    direct_source = (direct_candidate.get('sources') or [{}])[0] if direct_candidate else {}
    if (not direct_candidate or direct_candidate.get('tu') != 'ov10/tu003' or
            direct_candidate.get('allocated') != list(preserved) or
            direct_source.get('task') != task_id or direct_source.get('submission_id') != submission_id or
            not any((file_row.get('result') or {}).get('sha256') == correction['candidate_source']['sha256']
                    for file_row in direct_candidate.get('files', []))):
        raise CorrectionError('direct acceptance report does not pin this exact owner submission and eight-function candidate')

    return dict(id=row['id'], kind='qualifier_cast_only', owner_tu='ov10/tu003', path=row['path'],
                application_phase='pre_source', before_sha256=digest(base), after_sha256=digest(post),
                patch_sha256=digest(patch_bytes), zero_credit=True,
                authority_sha256=authority_ref['sha256'], proposal_sha256=correction['proposal']['sha256'],
                evidence_sha256=correction['evidence']['sha256'],
                application_manifest_sha256=correction['application_manifest']['sha256'],
                function_bodies_changed=['CardPlayCommandPlay'],
                normalized_function_body_sha256_before=normalized,
                normalized_function_body_sha256_after=normalized_result,
                function_block_sha256=before_blocks, result_function_block_sha256=after_blocks,
                allocated_functions_preserved=preserved,
                submission_id=submission_id, operation=correction['operation'])


def _is_plain_extern_prototype(statement: str, symbol: str) -> bool:
    """Accept only a simple standalone extern function declaration."""
    if not isinstance(statement, str) or not statement or not isinstance(symbol, str) or not symbol:
        return False
    if any(token in statement for token in ('__attribute__', '__declspec', '__asm__', ' asm ', '=')):
        return False
    if lexical(statement) != statement:
        return False
    pattern = r'\s*extern\s+[^{};=]*\b' + re.escape(symbol) + r'\s*\([^{};=]*\)\s*;\s*'
    return re.fullmatch(pattern, statement, re.S) is not None


def _block_declaration_rows(correction: dict):
    rows = correction.get('changed_declarations')
    if not isinstance(rows, list) or not rows:
        raise CorrectionError('block-scope correction must pin at least one exact declaration')
    seen = set()
    for item in rows:
        if not isinstance(item, dict) or set(item) != {'function', 'symbol', 'before', 'after'}:
            raise CorrectionError('block-scope declaration row has unexpected fields')
        fn, symbol = item.get('function'), item.get('symbol')
        before, after = item.get('before'), item.get('after')
        if (not isinstance(fn, str) or not fn or not isinstance(symbol, str) or not symbol or
                not _is_plain_extern_prototype(before, symbol) or
                (after != '' and not _is_plain_extern_prototype(after, symbol))):
            raise CorrectionError('block-scope correction is not an exact plain extern prototype removal/replacement')
        key = (fn, symbol)
        if key in seen:
            raise CorrectionError('block-scope correction repeats a function/symbol pair')
        seen.add(key)
    return rows


def apply_block_scope_declaration_corrections(source_text: str, path: str, correction: dict) -> str:
    """Apply only exact, pinned extern prototype edits inside named C functions."""
    tu = TU(source_text, path)
    rows = _block_declaration_rows(correction)
    seen_functions = set()
    result = source_text
    for item in rows:
        function = item['function']
        if function in seen_functions:
            raise CorrectionError(f'block-scope correction has multiple edits in {function}; use one exact row')
        seen_functions.add(function)
        block = tu.by_name.get(function)
        if block is None:
            raise CorrectionError(f'block-scope correction target {function} is not a mapped C function')
        old_block = tu.block_text(block)
        before = item['before']
        if old_block.count(before) != 1:
            raise CorrectionError(f'{function}: exact old extern prototype must occur once in the mapped block')
        new_block = old_block.replace(before, item['after'], 1)
        if item['after'] and new_block.count(item['after']) != 1:
            raise CorrectionError(f'{function}: replacement extern prototype is not unique')
        result = result[:block.start] + new_block + result[block.end:]
        tu = TU(result, path)
    return result


def _normalized_block_body(text: str, rows: list, function: str, side: str) -> str:
    normalized = text
    for item in rows:
        if item['function'] != function:
            continue
        statement = item['before' if side == 'before' else 'after']
        if statement:
            if normalized.count(statement) != 1:
                raise CorrectionError(f'{function}: declaration normalization expected one {side} occurrence')
            normalized = normalized.replace(statement, '', 1)
    return normalized


def _validate_user_block_scope_declaration_correction(repo: Path, row: dict, correction: dict):
    required = {'role', 'id', 'path', 'action', 'base', 'result', 'patch'}
    owner_tu = correction.get('owner_tu')
    if (set(row) != required or row.get('role') != 'owner_source_correction' or row.get('action') != 'patch' or
            correction.get('zero_credit') is not True or correction.get('function_bodies_changed') != [] or
            correction.get('application_phase') != 'post_source' or correction.get('affected_tus') != [owner_tu] or
            not owner_tu or not correction.get('operation') or
            not re.fullmatch(r'(main|ov01|ov02|ov10|ov11|ov12)/tu[0-9]{3}', str(owner_tu))):
        raise CorrectionError('block-scope declaration correction has unexpected scope or is not zero-credit')
    tu_build = json.loads((repo / 'config/tu-build.json').read_text(encoding='utf-8'))
    tu_record = next((entry for entry in tu_build.get('tus', []) if entry.get('id') == owner_tu), None)
    if not tu_record or not tu_record.get('in_scope') or tu_record.get('category') != 'game' or row.get('path') != tu_record.get('path'):
        raise CorrectionError('block-scope declaration correction path is not its in-scope owner TU source')
    frozen_bytes = (repo / '.work/reworks/orq-rework940-20260929/luna-46/index.json').read_bytes()
    if digest(frozen_bytes) != 'ec6c5675d4cdf7821a2daa94c672177c3799155e122a8aff51d9f0730320817c':
        raise CorrectionError('frozen938 index hash changed')
    frozen_index = json.loads(frozen_bytes)
    if not any((entry.get('identity') or {}).get('tu') == owner_tu for entry in frozen_index.get('identities', [])):
        raise CorrectionError('block-scope correction owner TU is absent from frozen938 scope')
    for label in ('base', 'result'):
        spec = row.get(label) or {}
        if set(spec) != {'sha256', 'copy'}:
            raise CorrectionError(f'block-scope correction {label} source copy pin is malformed')
    base = _copy_bytes(repo, row['base']['copy'], row['base']['sha256'], 'block-scope correction base')
    post = _copy_bytes(repo, row['result']['copy'], row['result']['sha256'], 'block-scope correction postimage')
    if correction.get('base_sha256') != digest(base) or correction.get('result_sha256') != digest(post):
        raise CorrectionError('block-scope correction bytes differ from their immutable pins')
    refs = {}
    for name in ('authority', 'proposal', 'evidence', 'application_manifest'):
        ref = correction.get(name)
        _path, data = _artifact(repo, ref, f'block-scope correction {name}')
        refs[name] = json.loads(data)
    authority = refs['authority']
    frozen, policy = authority.get('scope') or {}, authority.get('integration_policy') or {}
    if (correction['authority'].get('sha256') != '7b4e8d276062c4c93cddb1ed259de2115df5b1c8e42c33e03a52c1161792e83c' or
            authority.get('schema') != 'user-direct-acceptance-authority/1' or
            authority.get('recorded_by') != 'orq-review940-20260929' or
            frozen.get('frozen_index_sha256') != 'ec6c5675d4cdf7821a2daa94c672177c3799155e122a8aff51d9f0730320817c' or
            frozen.get('function_count') != 938 or frozen.get('new_recovery_outside_frozen_scope') is not False or
            policy.get('private_integrator_preflight') is not False or policy.get('post_integration_review') is not False or
            policy.get('live_publisher_byte_comparison_and_tu_audit') != 'required'):
        raise CorrectionError('block-scope declaration correction is not bound to the pinned user waiver')
    changed = _block_declaration_rows(correction)
    proposal, evidence, application = refs['proposal'], refs['evidence'], refs['application_manifest']
    provider = evidence.get('provider_prototype') or {}
    provider_ref = evidence.get('provider_source') or {}
    provider_path, provider_bytes = _artifact(repo, provider_ref, 'provider definition source')
    provider_owner = provider.get('owner_tu')
    provider_record = next((entry for entry in tu_build.get('tus', []) if entry.get('id') == provider_owner), None)
    provider_source = provider_bytes.decode('utf-8', 'surrogateescape')
    expected_provider = ('void RgGeomRobotSetDir(RgGeom *geom, RgVector direction)')
    if (not provider_record or provider_record.get('path') != provider_path.relative_to(repo.resolve()).as_posix() or
            provider_ref.get('sha256') != digest(provider_bytes) or provider.get('declaration') != expected_provider or
            re.search(r'(?m)^\s*void\s+RgGeomRobotSetDir\s*\(\s*RgGeom\s*\*\s*geom\s*,\s*'
                      r'RgVector\s+direction\s*\)\s*\{', provider_source) is None):
        raise CorrectionError('provider prototype evidence does not match the mapped defining TU source')
    if (proposal.get('schema') != 'user-authorized-block-scope-declaration-proposal/1' or
            proposal.get('owner_tu') != owner_tu or proposal.get('owner_path') != row['path'] or
            proposal.get('base_sha256') != digest(base) or proposal.get('result_sha256') != digest(post) or
            proposal.get('changed_declarations') != changed or proposal.get('function_bodies_changed') != [] or
            proposal.get('new_c_credit') != 0 or
            evidence.get('schema') != 'block-scope-declaration-evidence/1' or
            evidence.get('owner_tu') != owner_tu or evidence.get('base_sha256') != digest(base) or
            provider.get('symbol') != 'RgGeomRobotSetDir' or provider_owner != 'ov12/tu054' or
            application.get('schema') != 'user-authorized-block-scope-application/1' or
            application.get('owner_tu') != owner_tu or application.get('base_sha256') != digest(base) or
            application.get('result_sha256') != digest(post) or application.get('patch_sha256') != row['patch'].get('sha256')):
        raise CorrectionError('proposal/evidence/application artifacts do not pin this exact correction')
    patch_path, patch_bytes = _artifact(repo, row['patch'], 'block-scope declaration patch')
    patch = json.loads(patch_bytes)
    if (patch.get('schema') != 'tu-patch/1' or patch.get('tu') != row['path'] or
            patch.get('base_sha256') != digest(base) or patch.get('functions') or
            set(patch) - {'schema', 'tu', 'base_sha256', 'prelude', 'functions', 'interstitial',
                          'block_scope_declaration_corrections'} or
            patch.get('prelude') not in (None, {}) or patch.get('interstitial') not in (None, {}) or
            patch.get('block_scope_declaration_corrections') != changed):
        raise CorrectionError('block-scope patch has changes outside exact declaration correction rows')
    before_tu = TU(base.decode('utf-8', 'surrogateescape'), row['path'])
    after_tu = TU(post.decode('utf-8', 'surrogateescape'), row['path'])
    replay = apply_block_scope_declaration_corrections(before_tu.text, row['path'], correction)
    if replay.encode('utf-8', 'surrogateescape') != post:
        raise CorrectionError('block-scope declaration patch does not exactly reproduce its pinned postimage')
    before_blocks = {fn.name: before_tu.block_text(fn) for fn in before_tu.functions()}
    after_blocks = {fn.name: after_tu.block_text(fn) for fn in after_tu.functions()}
    if set(before_blocks) != set(after_blocks):
        raise CorrectionError('block-scope correction changed the mapped function set')
    normalized_before = {name: _normalized_block_body(text, changed, name, 'before')
                         for name, text in before_blocks.items()}
    normalized_after = {name: _normalized_block_body(text, changed, name, 'after')
                        for name, text in after_blocks.items()}
    if normalized_before != normalized_after:
        raise CorrectionError('block-scope correction changed executable function-body text beyond pinned prototype rows')
    raw_before = {name: digest(text.encode('utf-8', 'surrogateescape')) for name, text in before_blocks.items()}
    raw_after = {name: digest(text.encode('utf-8', 'surrogateescape')) for name, text in after_blocks.items()}
    if correction.get('function_block_sha256') != raw_before or correction.get('result_function_block_sha256') != raw_after:
        raise CorrectionError('block-scope correction does not retain exact raw before/after function-block hashes')
    return dict(id=row['id'], kind='block_scope_declaration_only', owner_tu=owner_tu, path=row['path'],
                before_sha256=digest(base), after_sha256=digest(post), patch_sha256=digest(patch_bytes),
                zero_credit=True, authority_sha256=correction['authority']['sha256'],
                proposal_sha256=correction['proposal']['sha256'], evidence_sha256=correction['evidence']['sha256'],
                application_manifest_sha256=correction['application_manifest']['sha256'],
                function_bodies_changed=[], changed_declarations=changed,
                raw_function_block_sha256_before=raw_before, raw_function_block_sha256_after=raw_after,
                normalized_function_body_sha256_before={name: digest(text.encode('utf-8', 'surrogateescape'))
                                                       for name, text in normalized_before.items()},
                normalized_function_body_sha256_after={name: digest(text.encode('utf-8', 'surrogateescape'))
                                                      for name, text in normalized_after.items()},
                operation=correction['operation'])


def validate_user_authorized_source_correction(repo: Path, row: dict, correction: dict):
    return _user_correction_inputs(repo, row, correction)


def apply_user_authorized_source_correction(repo: Path, project: Path, row: dict, correction: dict):
    _guard_canonical_write(project / row['path'])
    receipt = validate_user_authorized_source_correction(repo, row, correction)
    target = (project / row['path']).resolve()
    target.relative_to(project.resolve())
    current = target.read_bytes() if target.is_file() else b''
    current_sha = digest(current)
    if current_sha == row['result']['sha256']:
        receipt.update(outcome='already_applied', current_before_sha256=current_sha)
        return receipt
    if current_sha != row['base']['sha256']:
        raise CorrectionError('current owner source is neither the pinned original nor corrected declaration')
    post = _copy_bytes(repo, row['result']['copy'], row['result']['sha256'], 'user correction postimage')
    target.parent.mkdir(parents=True, exist_ok=True)
    target.write_bytes(post)
    receipt.update(outcome='applied', current_before_sha256=current_sha,
                   result_sha256=digest(target.read_bytes()))
    return receipt


if __name__ == '__main__':
    sys.exit(main())
