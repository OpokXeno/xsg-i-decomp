"""Declared terminal alignment of a translation unit's `.text`.

`config/tu-build.json` records, for every TU, where its code ends
(`text.code_end`) and where the original object's `.text` ends (`text.end`).
Where the two differ by less than 8 bytes and `text.end` is 8-byte aligned, the
original object's `.text` was padded out to an 8-byte boundary after its last
function (zero words: `nop`). The TU map records the same fact as
`text_tail_padding` / "N byte(s) of zero padding after the last function
before the next object".

That padding belongs to the object, not to any function: the `.size` of the
last function stops before it. While the last function is `INCLUDE_ASM` the
generated `.s` file carries it as trailing bytes; once that function is C, cc1
emits no trailing alignment and the TU's `.text` ends 4 bytes early. Whether the
next object then starts at the original address depended on that object's own
section alignment. The build reproduces the declared property instead: the
linker scripts place `. = ALIGN(8);` after the TU's `.text`
(`tools/tu/ninja_main.py`, `tools/tu/ninja_ovl.py`, `tools/tu/link_elf.py`),
which is a no-op while the TU's object already ends at `text.end` and restores
exactly the declared zero padding when it does not. `tools/worker.py`'s `.text`
size precheck accepts a TU exactly that many bytes short.

Nothing here admits anything into C source: the TU's C stays ordinary, the
per-function sizes are still compared by `tools/tu_audit.py`, and the whole-file
gate still compares every byte, the padding included.
"""
import json
from pathlib import Path

TAIL_ALIGN = 8


def declared_tail_padding(text):
    """Bytes of declared tail padding of one `config/tu-build.json` `text` record
    (0 when the TU declares none)."""
    if not text or text.get('empty'):
        return 0
    end = int(text['end'], 16)
    code_end = int(text.get('code_end') or text['end'], 16)
    pad = end - code_end
    if 0 < pad < TAIL_ALIGN and end % TAIL_ALIGN == 0:
        return pad
    return 0


def declared_tail_align(text):
    """`TAIL_ALIGN` when the TU declares tail padding, else None."""
    return TAIL_ALIGN if declared_tail_padding(text) else None


def declaring_tus(root):
    """The `config/tu-build.json` records of every TU that declares tail padding."""
    manifest = json.loads((Path(root) / 'config/tu-build.json').read_text())
    return [t for t in manifest['tus'] if declared_tail_align(t.get('text'))]


def ld_statement():
    """The linker-script statement (without its terminating `;`)."""
    return f". = ALIGN({TAIL_ALIGN})"


def ld_comment(tu_id):
    return f"/* declared tail alignment of {tu_id}: config/tu-build.json text.code_end..text.end */"
