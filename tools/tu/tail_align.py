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
    """Declared tails from the private manifest or verified public build inputs."""
    path = Path(root) / 'config/tu-build.json'
    manifest = (json.loads(path.read_bytes()) if path.is_file()
                else public_manifest(root))
    return [t for t in manifest['tus'] if verified_tail_padding(root, t, manifest)]


def public_manifest(root):
    """Recover terminal padding from tracked TU ranges and original ELF symbols.

    Public clones do not carry config/tu-build.json. The published object
    ranges and the original function extents describe the same tails. Verify
    the original hash and every candidate's zero bytes before using them; the
    existing extended-tail check still requires a contiguous empty next TU.
    This supplies linker alignment only, never function recovery credit.
    """
    from elfinfo import Elf
    from toolchain import Toolchain

    root = Path(root)
    game = Toolchain(root).game_dir()
    originals = json.loads((root / 'config/originals.json').read_bytes())
    main = json.loads((root / 'config/objects/main.objects.json').read_bytes())
    overlays = json.loads((root / 'config/objects/overlays.compile.json').read_bytes())
    manifest = dict(units={}, tus=[])
    for unit, record in originals['units'].items():
        original = (game / record['file']).resolve()
        data = original.read_bytes()
        if len(data) != record['size'] or hashlib.sha256(data).hexdigest() != record['sha256']:
            raise ValueError(f'tail alignment: {original} differs from config/originals.json')
        elf = Elf(data)
        manifest['units'][unit] = dict(record, file=str(original))
        tus = ([t for t in main['tus'] if t['unit'] == unit] if unit == 'main'
               else overlays['units'][unit]['tus'])
        for tu in tus:
            span = tu['text']
            section = elf.section(span.get('section', '.text') if unit == 'main' else unit)
            functions = [s for s in elf.symbols if s.type == 2 and s.shndx == section.index]
            start, end = ([int(span[k], 16) for k in ('start', 'end')]
                          if isinstance(span, dict) else [int(v, 16) for v in span])
            if not section.addr <= start <= end <= section.addr + section.size:
                raise ValueError(f'tail alignment: {tu["id"]} is outside {section.name}')
            owned_functions = [s for s in functions if start <= s.value < end]
            code_end = max((s.value + s.size for s in owned_functions), default=end)
            offset_start = section.offset + start - section.addr
            offset_end = section.offset + end - section.addr
            owned = data[offset_start:offset_end]
            text = dict(start=hex(start), end=hex(end), code_end=hex(code_end),
                        size=end - start, empty=start == end,
                        sha256=hashlib.sha256(owned).hexdigest())
            # A symbol crossing the TU boundary or nonzero tail is not padding.
            if code_end > end or owned[code_end - start:] != bytes(max(0, end - code_end)):
                text['code_end'] = text['end']
            manifest['tus'].append(dict(
                id=tu['id'], unit=unit, name=tu['name'], ordinal=tu['ordinal'],
                text=text,
                functions=[dict(va=hex(s.value), size=s.size) for s in owned_functions],
                files=[dict(target=unit, file=str(original),
                            offset_start=offset_start, offset_end=offset_end)]))
    return manifest


def ld_statement(alignment=TAIL_ALIGN):
    """The linker-script statement (without its terminating `;`)."""
    if alignment not in (TAIL_ALIGN, EXTENDED_TAIL_ALIGN):
        raise ValueError(f'unsupported TU tail alignment: {alignment}')
    return f". = ALIGN({alignment})"


def ld_comment(tu_id):
    return f"/* declared tail alignment of {tu_id}: config/tu-build.json text.code_end..text.end */"
