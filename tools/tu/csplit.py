#!/usr/bin/env python3
"""Tiny top-level C splitter (comments/strings/preprocessor aware).

split(text) -> list of items (kind, text, name):
  kind 'pp'       preprocessor directive (with continuation lines)
  kind 'comment'  a free-standing comment block (not followed on its line by code)
  kind 'decl'     a top-level declaration ending with ';'
  kind 'func'     a function definition (name = function name)
Leading comments directly attached (no blank line) to a func are kept with it.
"""
import re

IDENT = re.compile(r"[A-Za-z_]\w*")


def _scan(text):
    """Yield (start, end, kind) chunks at brace depth 0."""
    i, n = 0, len(text)
    items = []
    cur_start = None
    depth = 0
    paren = 0
    line_start = True
    while i < n:
        c = text[i]
        if line_start and c == "#" and depth == 0 and cur_start is None:
            j = i
            while True:
                e = text.find("\n", j)
                if e == -1:
                    e = n
                    break
                if text[e - 1] == "\\":
                    j = e + 1
                    continue
                break
            items.append((i, e, "pp"))
            i = e
            continue
        if c == "\n":
            line_start = True
            i += 1
            continue
        if c in " \t\r\f\v":
            i += 1
            continue
        line_start = False
        if text.startswith("/*", i):
            e = text.index("*/", i + 2) + 2
            if cur_start is None and depth == 0:
                items.append((i, e, "comment"))
            i = e
            continue
        if text.startswith("//", i):
            e = text.find("\n", i)
            e = n if e == -1 else e
            if cur_start is None and depth == 0:
                items.append((i, e, "comment"))
            i = e
            continue
        if c in "\"'":
            j = i + 1
            while text[j] != c:
                j += 2 if text[j] == "\\" else 1
            if cur_start is None:
                cur_start = i
            i = j + 1
            continue
        if cur_start is None:
            cur_start = i
        if c == "(":
            paren += 1
        elif c == ")":
            paren -= 1
        elif c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
            if depth == 0:
                head = text[cur_start:text.index("{", cur_start)]
                if "(" in head and "=" not in head and not re.search(r"\b(struct|union|enum)\b[^(]*$", head):
                    items.append((cur_start, i + 1, "func"))
                    cur_start = None
        elif c == ";" and depth == 0 and paren == 0:
            items.append((cur_start, i + 1, "decl"))
            cur_start = None
        i += 1
    if cur_start is not None:
        raise ValueError("unterminated top-level item")
    return items


def func_name(src):
    head = src[:src.index("{")]
    # identifier directly before the parameter list's '(' at paren depth 0
    depth = 0
    for m in re.finditer(r"[()]|[A-Za-z_]\w*", head):
        t = m.group()
        if t == "(":
            if depth == 0:
                before = IDENT.findall(head[:m.start()])
                return before[-1]
            depth += 1
        elif t == ")":
            depth -= 1
    raise ValueError("no function name in " + head)


def split(text):
    raw = _scan(text)
    out = []
    for k, (s, e, kind) in enumerate(raw):
        body = text[s:e]
        name = func_name(body) if kind == "func" else None
        out.append([kind, body, name, s, e])
    # attach a comment immediately preceding a func/decl (only whitespace, at most
    # one newline between) to that item
    merged = []
    for item in out:
        if merged and merged[-1][0] == "comment" and item[0] in ("func", "decl"):
            gap = text[merged[-1][4]:item[3]]
            if gap.count("\n") <= 1 and not gap.strip():
                c = merged.pop()
                item = [item[0], c[1] + gap + item[1], item[2], c[3], item[4]]
        merged.append(item)
    return [(k, b, n) for k, b, n, _, _ in merged]
