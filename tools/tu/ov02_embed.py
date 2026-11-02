#!/usr/bin/env python3
"""MAIN's embedded OV02 section, linked from the OV02 unit's own objects.

SLUS_204.69 carries a byte copy of OV02.OVL (section `ov02`, VA 0x00A00000).
The OV02 unit (build/ov02) builds that image from the published `src/ov02`
sources, its data carves and its scaffold; MAIN links the very same objects at
the very same addresses instead of a second copy of the TUs (user 2026-11-02:
one source per OV02 TU, published C reaches both files).

Run by tools/configure.py after both units are configured.  Writes, in the MAIN
unit directory:

  ov02_embedded.ld        the `ov02` output-section body: the `.ov02` body of
                          build/ov02/ov02.ld (text, data, rodata with their
                          carve pins) followed by its `.ov02_bss` body rebased
                          to the bss offset (the bss is zero bytes of the
                          embedded image), object paths rebased to ../ov02/;
  ov02_embedded.ninja     `ov02_embedded_objs` (the link inputs, for the
                          undefined filter's object list) and the phony
                          `ov02_embedded_inputs` the link edges depend on;
  ov02_embedded.syms.ld   PROVIDEs for names the links still need that no
                          linked object defines any more: MAIN's splat names
                          inside the embedded range (its old scaffold defined
                          them; MAIN code cites some, e.g. func_00A11108) and
                          the OV02 unit's undefined_*_auto.txt names of MAIN
                          addresses (e.g. D_4A1A0C, renamed in MAIN).  A
                          PROVIDE is used only when a linked object references
                          the name and none defines it, so it never stands in
                          for a definition (undef_filter.py's rule F8);
  ov02_embedded.json      report.

    ov02_embed.py --main-dir build/main --ov02-dir build/ov02
"""
import argparse
import json
import re
import sys
from pathlib import Path

ASSIGN = re.compile(r"^\s*([^\s=]+)\s*=\s*(0x[0-9A-Fa-f]+|\d+)\s*;")
LABEL = re.compile(r"^\s*(?:glabel|dlabel)\s+([A-Za-z_][\w.$]*)")
ADDR_COMMENT = re.compile(r"/\*\s*[0-9A-Fa-f]+\s+([0-9A-Fa-f]{8})\s")
OBJECT = re.compile(r"(\.\./ov02/build/[\w/.\-]+\.o)\(")


def section_body(lines, name):
    """The statements inside `.<name> ... { ... }` of a splat linker script."""
    start = next(k for k, line in enumerate(lines) if re.match(rf"\s*\.{name}\s", line))
    head = lines[start]
    open_at = next(k for k in range(start, len(lines)) if lines[k].strip() == "{")
    depth, body = 0, []
    for k in range(open_at, len(lines)):
        depth += lines[k].count("{") - lines[k].count("}")
        if k > open_at and depth == 0:
            return head, body
        if k > open_at:
            body.append(lines[k])
    raise SystemExit(f"ov02_embed: unterminated .{name} in ov02.ld")


def rebase(body, base):
    """Statements of an OV02 section as statements of MAIN's `ov02` section."""
    out = []
    for line in body:
        s = line.strip()
        if not s or s.startswith("/*") or s.startswith("FILL(") or re.match(r"ov02_\w+ = ", s):
            continue
        s = re.sub(r"(?<![\w/.])build/", "../ov02/build/", s)
        pin = re.match(r"\. = (0x[0-9A-Fa-f]+);(.*)", s)
        if pin and base:
            s = f". = 0x{int(pin[1], 16) + base:X};{pin[2]}"
        out.append(s)
    return out


def assignments(path):
    found = {}
    for line in path.read_text().splitlines():
        m = ASSIGN.match(line)
        if m:
            found.setdefault(m[1], int(m[2], 0))
    return found


def splat_labels(files):
    """Every glabel/dlabel of MAIN's embedded OV02 scaffold, at its VA."""
    found = {}
    for f in files:
        pending = []
        for line in f.read_text().splitlines():
            m = LABEL.match(line)
            if m:
                pending.append(m[1])
                continue
            a = ADDR_COMMENT.search(line)
            if a and pending:
                for name in pending:
                    found.setdefault(name, int(a[1], 16))
                pending = []
    return found


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--main-dir", type=Path, required=True)
    ap.add_argument("--ov02-dir", type=Path, required=True)
    a = ap.parse_args()
    main_dir, ov_dir = a.main_dir.resolve(), a.ov02_dir.resolve()
    script = ov_dir / "ov02.ld"
    if not script.is_file():
        raise SystemExit("ov02_embed: build/ov02/ov02.ld is missing; configure the ov02 unit first")
    lines = script.read_text().splitlines()
    head, text_data = section_body(lines, "ov02")
    if not re.match(r"\s*\.ov02 0xA00000 ", head):
        raise SystemExit(f"ov02_embed: unexpected .ov02 header {head.strip()!r}")
    bss_head, bss = section_body(lines, "ov02_bss")
    bss_va = int(re.match(r"\s*\.ov02_bss (0x[0-9A-Fa-f]+)", bss_head)[1], 16)
    bss_base = bss_va - 0xA00000
    body = (["/* Generated by tools/tu/ov02_embed.py from build/ov02/ov02.ld: MAIN's embedded OV02",
             "   image links the OV02 unit's own objects at their OV02 addresses. */"]
            + rebase(text_data, 0)
            + [f". = 0x{bss_base:X}; /* OV02 .ov02_bss: zero bytes of the embedded image */"]
            + rebase(bss, bss_base))
    objs = sorted(set(OBJECT.findall("\n".join(body))))
    missing_rel = [o for o in objs if not o.startswith("../ov02/build/")]
    if missing_rel or not objs:
        raise SystemExit(f"ov02_embed: unexpected object paths {missing_rel[:4]}")

    # Names that MAIN's embedded scaffold used to define.
    manifest = json.loads((main_dir / "tu-manifest.json").read_text())
    names = manifest.get("ov02_embedded", [])
    label_files = sorted(p for n in names for p in (main_dir / "asm/main/nonmatchings" / n).glob("*.s"))
    label_files += [main_dir / "asm/main/data/ov02/data.data.s"]
    old = splat_labels([f for f in label_files if f.is_file()])
    main_auto = {}
    for f in ("undefined_syms_auto.txt", "undefined_funcs_auto.txt"):
        for k, v in assignments(main_dir / f).items():
            main_auto.setdefault(k, v)
    ov_auto = {}
    for f in ("undefined_syms_auto.txt", "undefined_funcs_auto.txt"):
        for k, v in assignments(ov_dir / f).items():
            ov_auto.setdefault(k, v)
    conflicts, out = [], {}
    for source, table in (("main embedded scaffold", old), ("ov02 undefined_*_auto.txt", ov_auto)):
        for k, v in sorted(table.items()):
            known = main_auto.get(k, out.get(k, (None, None))[0] if k in out else None)
            if known is not None and known != v:
                conflicts.append(dict(name=k, value=f"0x{v:X}", other=f"0x{known:X}", source=source))
                continue
            if k in main_auto or k in out:
                continue
            out[k] = (v, source)
    if conflicts:
        raise SystemExit("ov02_embed: symbol value conflicts: " + json.dumps(conflicts[:8]))

    def write(path, text):
        if not path.is_file() or path.read_text() != text:
            path.write_text(text)

    write(main_dir / "ov02_embedded.ld", "\n".join(body) + "\n")
    write(main_dir / "ov02_embedded.ninja",
          "# Generated by tools/tu/ov02_embed.py: the OV02 unit's objects MAIN's ov02 section links\n"
          f"ov02_embedded_objs = {' '.join(objs)}\n"
          f"build ov02_embedded_inputs: phony {' '.join(objs)}\n")
    write(main_dir / "ov02_embedded.syms.ld",
          "/* MAIN embedded OV02: names no linked object may define any more (tools/tu/ov02_embed.py) */\n"
          + "".join(f"PROVIDE({k} = 0x{v:X});\n" for k, (v, _) in sorted(out.items())))
    write(main_dir / "ov02_embedded.json", json.dumps(dict(
        source=str(script), bss_base=f"0x{bss_base:X}", objects=len(objs), body_lines=len(body),
        names=dict(main_embedded_scaffold=sum(1 for v in out.values() if v[1] == "main embedded scaffold"),
                   ov02_auto=sum(1 for v in out.values() if v[1] != "main embedded scaffold"))), indent=1) + "\n")
    return 0


if __name__ == "__main__":
    sys.exit(main())
