"""Declared terminal alignment of a translation unit's `.text`.

`config/tu-build.json` records, for every TU, where its code ends
(`text.code_end`) and where the original object's `.text` ends (`text.end`).
Where the two differ by less than 8 bytes and `text.end` is 8-byte aligned, the
original object's `.text` was padded out to an 8-byte boundary after its last
function (zero words: `nop`). One separately verified 8-byte gap aligns
main/tu273 to 16 bytes. The TU map records these facts as
`text_tail_padding` / "N byte(s) of zero padding after the last function
before the next object".

That padding belongs to the object, not to any function: the `.size` of the
last function stops before it. While the last function is `INCLUDE_ASM` the
generated `.s` file carries it as trailing bytes; once that function is C, cc1
emits no trailing alignment and the TU's `.text` ends 4 bytes early. Whether the
next object then starts at the original address depended on that object's own
section alignment. The build reproduces the declared property instead: the
linker scripts place the TU's declared alignment after its `.text`
(`tools/tu/ninja_main.py`, `tools/tu/ninja_ovl.py`, `tools/tu/link_elf.py`),
which is a no-op while the TU's object already ends at `text.end` and restores
exactly the declared zero padding when it does not. `tools/worker.py`'s `.text`
size precheck accepts a TU exactly that many bytes short.

Nothing here admits anything into C source: the TU's C stays ordinary, the
per-function sizes are still compared by `tools/tu_audit.py`, and the whole-file
gate still compares every byte, the padding included.
"""
import hashlib
import json
from pathlib import Path

TAIL_ALIGN = 8
EXTENDED_TAIL_ALIGN = 16


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
    if pad == TAIL_ALIGN and code_end % EXTENDED_TAIL_ALIGN == TAIL_ALIGN \
            and end % EXTENDED_TAIL_ALIGN == 0:
        return pad
    return 0


def declared_tail_align(text):
    """Structural alignment candidate; an 8-byte gap also needs original proof."""
    padding = declared_tail_padding(text)
    return EXTENDED_TAIL_ALIGN if padding == TAIL_ALIGN else TAIL_ALIGN if padding else None


def verified_extended_tail(root, tu, manifest=None):
    """Prove a full eight-byte zero tail outside functions in the pinned original.

    The old 4-byte declarations are unchanged. An 8-byte alignment permission
    needs more than arithmetic: a contiguous empty next TU, the original file
    and TU hashes, and zero bytes after every original function's ELF extent.
    """
    if declared_tail_padding(tu['text']) != TAIL_ALIGN:
        return False
    root = Path(root)
    manifest = manifest or json.loads((root / 'config/tu-build.json').read_text())
    start = int(tu['text']['start'], 16)
    code_end = int(tu['text']['code_end'], 16)
    end = int(tu['text']['end'], 16)
    following = [item for item in manifest['tus']
                 if item['unit'] == tu['unit'] and item['ordinal'] == tu['ordinal'] + 1
                 and int(item['text']['start'], 16) == end]
    if (len(following) != 1 or not following[0]['text'].get('empty') or
            following[0]['text']['size'] != 0 or
            any(int(func['va'], 16) + func['size'] > code_end
                for func in tu['functions'])):
        return False
    ranges = [item for item in tu['files'] if item['target'] == tu['unit']]
    if len(ranges) != 1:
        return False
    span = ranges[0]
    if span['offset_end'] - span['offset_start'] != end - start:
        return False
    original = root / span['file']
    if not original.is_file():
        return False
    data = original.read_bytes()
    if hashlib.sha256(data).hexdigest() != manifest['units'][tu['unit']]['sha256']:
        return False
    owned = data[span['offset_start']:span['offset_end']]
    return (len(owned) == tu['text']['size'] and
            hashlib.sha256(owned).hexdigest() == tu['text']['sha256'] and
            owned[code_end - start:] == bytes(TAIL_ALIGN))


def verified_tail_padding(root, tu, manifest=None):
    """Padding eligible for the precheck and link script, not recovery credit."""
    padding = declared_tail_padding(tu['text'])
    return padding if padding != TAIL_ALIGN or verified_extended_tail(root, tu, manifest) else 0


def declaring_tus(root):
    """The `config/tu-build.json` records of every TU that declares tail padding."""
    manifest = json.loads((Path(root) / 'config/tu-build.json').read_text())
    return [t for t in manifest['tus'] if verified_tail_padding(root, t, manifest)]


def ld_statement(alignment=TAIL_ALIGN):
    """The linker-script statement (without its terminating `;`)."""
    if alignment not in (TAIL_ALIGN, EXTENDED_TAIL_ALIGN):
        raise ValueError(f'unsupported TU tail alignment: {alignment}')
    return f". = ALIGN({alignment})"


def ld_comment(tu_id):
    return f"/* declared tail alignment of {tu_id}: config/tu-build.json text.code_end..text.end */"
