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
  merge --base B --ours O --theirs T [--skeleton S] [--allowed F,...] [--comments strict|ours]
  make-patch --base B --theirs T [--skeleton S]
  apply-patch --base B --target O --patch P.json [--skeleton S] [--allowed F,...]
Edits write to stdout, to -o OUT or back with --in-place (never on conflict).
Exit status: 0 ok, 1 conflicts/refusal, 2 usage or parse error.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
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


def extract_function(source_text, name):
    """Text of the C definition `name` in another C file, attached comments included."""
    tu = TU(source_text)
    b = tu.by_name.get(name)
    if b is None or b.state != 'c':
        raise EditError(f'no C definition of {name!r} in the source')
    return tu.block_text(b)


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


def to_c(tu, name, c_text, as_name=None, replace_accepted_asm=False):
    """Replace block `name` with the C definition c_text (defining as_name or name)."""
    b = tu.block(name)
    if b.conditional:
        raise EditError(f'{name} sits inside a preprocessor conditional: edit the source by hand')
    if b.state == 'accepted_asm' and not replace_accepted_asm:
        raise EditError(f'{name} is accepted exact_asm; pass replace_accepted_asm for a re-treatment')
    body = _clean_block_text(c_text)
    defined = check_c_body(body, as_name or b.key, alt=None if as_name else name_stem(b.name)).name
    if not same_function(defined, b.key) and not same_function(defined, b.name) and defined != as_name:
        raise EditError(f'C body defines {defined!r}, not the function of block {name!r}')
    out = tu.splice(b.start, b.end, body)
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


def merge_prelude(base, ours, theirs, comments='strict'):
    """Three-way additive prelude merge. Returns (text, conflicts, notes)."""
    b_items, _ = prelude_items(base)
    o_items, o_tail = prelude_items(ours)
    t_items, _ = prelude_items(theirs)
    conflicts, notes = [], []
    b_set = {_norm(t) for _, t in b_items}
    o_norm = [_norm(t) for _, t in o_items]
    t_set = {_norm(t) for _, t in t_items}
    removed = [t for _, t in b_items if _norm(t) not in t_set]
    added = [(k, g, t) for k, (g, t) in enumerate(t_items) if _norm(t) not in b_set]
    added_keys = {}
    for _, _, t in added:
        for key in decl_keys(t):
            added_keys.setdefault(key, []).append(t)
    for t in removed:
        if _is_comment(t) and comments == 'ours':
            notes.append(dict(kind='prelude_comment_removed_ignored', item=t))
            continue
        shared = [a for key in decl_keys(t) for a in added_keys.get(key, [])]
        conflicts.append(dict(kind='prelude_modified' if shared else 'prelude_removed', item=t,
                              replacement=shared[0] if shared else None,
                              detail='the patch changes or removes an existing prelude item; '
                                     'prelude changes must be additive'))
    merged = list(o_items)
    merged_norm = list(o_norm)
    for k, gap, t in added:
        nt = _norm(t)
        if _is_comment(t) and comments == 'ours':
            notes.append(dict(kind='prelude_comment_added_ignored', item=t))
            continue
        if nt in merged_norm:
            notes.append(dict(kind='prelude_already_present', item=t))
            continue
        clash = []
        for key in decl_keys(t):
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
        pos, ambiguous = None, None
        for direction in (-1, 1):
            j = k + direction
            while 0 <= j < len(t_items):
                n = _norm(t_items[j][1])
                count = merged_norm.count(n)
                if count == 1:
                    pos = merged_norm.index(n) + (1 if direction < 0 else 0)
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
        gap = gap if '\n' in gap else '\n'
        if pos == 0 and not merged:
            gap = ''
        merged.insert(pos, (gap, t))
        merged_norm.insert(pos, nt)
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


def merge3(base, ours, theirs, names=None, allowed=None, comments='strict'):
    """Function-level three-way merge of TU objects. Returns a result dict.

    result['text'] is None when there are conflicts. Conflicts: structural
    (function missing/reordered), prelude (non-additive change, name clash),
    function changed on both sides, removal of accepted C or accepted asm,
    interstitial/epilogue text changed on both sides, change outside `allowed`.
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
    prelude, conflicts, notes = merge_prelude(rb.prelude, ro.prelude, rt.prelude, comments)
    result['conflicts'] += conflicts
    result['notes'] += notes
    pre, block = {}, {}
    for n in names:
        bs, os_, ts = rb.state[n], ro.state[n], rt.state[n]
        text, status = _three(rb.block[n], ro.block[n], rt.block[n])
        theirs_changed = status in ('theirs', 'both_same') or status == 'conflict'
        if theirs_changed and allowed is not None and not any(same_function(a, n) for a in allowed):
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
        ptext, pstatus = _three(rb.pre[n], ro.pre[n], rt.pre[n])
        if pstatus == 'conflict':
            result['conflicts'].append(dict(kind='interstitial_both_changed', before_function=n))
            ptext = ro.pre[n]
        elif pstatus in ('theirs', 'both_same'):
            if allowed is not None and not any(same_function(a, n) for a in allowed):
                result['conflicts'].append(dict(
                    kind='interstitial_outside_allocation', before_function=n,
                    detail='the patch changes file-scope text before a function it was not allocated (it can '
                           'redefine macros or data other functions use)'))
            else:
                result['applied'].append(dict(interstitial_before=n))
        pre[n] = ptext
    etext, estatus = _three(rb.epilogue, ro.epilogue, rt.epilogue)
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

def make_patch(base, theirs, names=None, tu_path=None):
    """Function-level patch that turns base into theirs."""
    names = [b.key for b in base.functions()] if names is None else [n for n in names if n not in base.covered]
    rb, rt = Regions(base, names, 'base'), Regions(theirs, names, 'theirs')
    b_items, _ = prelude_items(rb.prelude)
    t_items, _ = prelude_items(rt.prelude)
    b_set = {_norm(t) for _, t in b_items}
    t_set = {_norm(t) for _, t in t_items}
    add = []
    for k, (gap, t) in enumerate(t_items):
        if _norm(t) in b_set:
            continue
        after = None
        for j in range(k - 1, -1, -1):
            after = _norm(t_items[j][1])
            break
        add.append(dict(text=t, after=after, gap=gap))
    remove = [t for _, t in b_items if _norm(t) not in t_set]
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
    names = [b.key for b in base.functions()] if names is None else [n for n in names if n not in base.covered]
    rb = Regions(base, names, 'base')
    items, tail = prelude_items(rb.prelude)
    prelude_patch = patch.get('prelude', {})
    removes = {_norm(t) for t in prelude_patch.get('remove', [])}
    items = [(g, t) for g, t in items if _norm(t) not in removes]
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
        items.insert(pos, (gap if gap.strip() == '' and ('\n' in gap or pos == 0) else '\n', text))
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


def apply_patch(base, target, patch, names=None, allowed=None, comments='strict'):
    theirs = patch_to_theirs(base, patch, names)
    return merge3(base, target, theirs, names, allowed, comments)


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def _write(path, text, in_place_of=None):
    if path in (None, '-'):
        sys.stdout.write(text)
        return
    path = Path(path)
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
    p.add_argument('--comments', choices=('strict', 'ours'), default='strict')
    common(p, out=True)
    args = ap.parse_args(argv)

    try:
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
                body = extract_function(_read(args.from_source), args.as_name or args.name)
            out = to_c(tu, args.name, body, args.as_name, args.replace_accepted_asm)
            _write(_out_target(args), out.text)
            if args.json:
                print(json.dumps(dict(function=args.name, state='c', sha256=sha256_text(out.text))),
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
                result = merge3(base, ours, theirs, names, _names(args.allowed), args.comments)
            else:
                target = load_tu(args.target)
                patch = json.loads(_read(args.patch))
                result = apply_patch(base, target, patch, names, _names(args.allowed), args.comments)
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


if __name__ == '__main__':
    sys.exit(main())
