#!/usr/bin/env python3
"""The build inputs the repository tracks, and how they reach a unit directory.

A clone holds the source, the split configuration, the symbol files and the
per-object compile contract.  Everything else a build needs - the linker
scripts, the disassembly, the INCLUDE_ASM skeletons and the retail blobs - is
generated from the originals by `./configure.py`, and none of it is tracked.

    config/split/<unit>.yaml             splat configuration of the unit
    config/split/main.data-splits.json   per-symbol data splits of main
    config/symbols/<unit>.txt            symbol addresses splat disassembles with
    config/symbols/<ovl>.main-symbols.ld MAIN addresses an overlay link resolves
    config/symbols/main.externals.ld     overlay addresses main's link resolves
    config/objects/<unit>.objects.json   one entry per original object
    config/objects/<unit>.compile.json   the compile contract of each object
    config/objects/<ovl>.layout.json     the overlay's section families

`stage` copies them into `build/<unit>/` under the names the build reads;
`freeze` writes them back from a configured unit directory (the maintenance
step after the TU map or the classification changes); `--check` says whether
the tracked copies still equal a freshly configured build.

    python3 -B tools/tu/tracked_inputs.py --stage  --unit-dir build/ov02 --unit ov02
    python3 -B tools/tu/tracked_inputs.py --freeze --unit-dir build/main --unit main
    python3 -B tools/tu/tracked_inputs.py --check

The per-object description keeps only what the build reads.  Three keys of the
generated manifest are dropped on the way in: `inputs` and `generator` name
private evidence under `.work/`, and `staged_headers` is a hash of the header
tree, which changes whenever a header moves and which nothing reads.
"""
import argparse
import json
import re
import shutil
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import toolchain as tcmod        # noqa: E402

sys.dont_write_bytecode = True

ROOT = Path(__file__).resolve().parents[2]
UNITS = ("main", "ov01", "ov02", "ov10", "ov11", "ov12")
OVERLAYS = UNITS[1:]

# Keys of the generated per-object manifest that describe how it was derived
# rather than how the unit builds.  They are not tracked.
UNTRACKED_MANIFEST_KEYS = ("inputs", "generator", "staged_headers")

# A compile contract names its compiler and its assembler.  Tracked, it names
# them by the identity of `config/toolchain-identity.json`; staged, it names the
# files on this machine.  Local paths are never tracked, so a clone is not asked
# to keep this project's private directory layout.
TOOL_TOKENS = {
    # Tool id unchanged; since 2026-09-23 it is the fsv3 release (reports/cc1-fsv3-alias.md).
    ".work/toolchains/ee-gcc2.96-realconv-lp7-fsv3-v1/bin": "tool:ee-gcc2.96-realconv-lp7",
    ".work/toolchains/ee-gcc2.96-realconv-lp7-fsv3-v1/bin/cc1": "tool:ee-gcc2.96-realconv-lp7/cc1",
    ".work/toolchains/compiler-variants/ee-gcc2.9-991111": "tool:ee-gcc2.9-991111",
    ".work/toolchains/compiler-variants/ee-gcc2.9-991111/cc1": "tool:ee-gcc2.9-991111/cc1",
    # Tool id unchanged; since 2026-09-24 it is the nolabel release (reports/as-la29-vsqrt-nolabel.md).
    ".work/toolchains/ee-legacy-binutils-la29-vsqrt-nolabel-v1/bin/ee-as": "tool:ee-as-la29-vsqrt",
    ".work/toolchains/ee-legacy-binutils/bin/ee-as": "tool:ee-as-2.9-plain",
    ".work/toolchains/ee-binutils/bin/mips64r5900el-ps2-elf-as":
        "tool:ps2dev-binutils/mips64r5900el-ps2-elf-as",
}
# The same tools named inside prose, where the sentence is the point.
TOOL_PROSE = {
    ".work/toolchains/ee-gcc2.96-realconv-lp7-fsv3-v1": "ee-gcc2.96-realconv-lp7",
}


def _map_strings(value, fn):
    if isinstance(value, dict):
        return {k: _map_strings(v, fn) for k, v in value.items()}
    if isinstance(value, list):
        return [_map_strings(v, fn) for v in value]
    if isinstance(value, str):
        return fn(value)
    return value


def _suffixes(table):
    """The same table keyed on the part below the toolchain root, so a path is
    recognised wherever that root happens to be on a given machine."""
    out = {}
    for path, value in table.items():
        out[path] = value
        out[path.split("toolchains/", 1)[1]] = value
    return out


TOOL_SUFFIXES = _suffixes(TOOL_TOKENS)
PROSE_SUFFIXES = _suffixes(TOOL_PROSE)


def to_tokens(data):
    """Replace this machine's tool paths by the identity they stand for."""
    def one(text):
        for suffix, token in TOOL_SUFFIXES.items():
            if text == suffix or text.endswith("/" + suffix):
                return token
        for suffix, name in PROSE_SUFFIXES.items():
            text = re.sub(r"[\w./+-]*" + re.escape(suffix), name, text)
        return text
    return _map_strings(data, one)


def to_paths(data, tc):
    """Replace each tool identity by its verified path on this machine."""
    cache = {}

    def resolve(token):
        if token not in cache:
            name = token[len("tool:"):]
            tool_id, _, member = name.partition("/")
            entry = tc.entry(tool_id)
            if member:
                cache[token] = str(tc.file(tool_id, member))
            elif "path" in entry:
                cache[token] = str(tc.file(tool_id))
            else:
                cache[token] = str(tc.dir(tool_id))
        return cache[token]

    return _map_strings(data, lambda t: resolve(t) if t.startswith("tool:") else t)


def plan(unit):
    """[(tracked path, unit-directory path, kind)] for one unit."""
    items = [
        (f"config/split/{unit}.yaml", "splat.yaml", "text"),
        (f"config/symbols/{unit}.txt", "symbol_addrs.txt", "text"),
    ]
    if unit == "main":
        items += [
            ("config/objects/main.compile.json", "compile-manifest.json", "contract"),
            ("config/split/main.data-splits.json", "auto-splits.json", "json"),
            ("config/objects/main.objects.json", "tu-manifest.json", "manifest"),
            ("config/symbols/main.externals.ld", "externals.ld", "text"),
        ]
    else:
        # One compile contract covers all five overlays; every overlay build
        # directory reads the same file.
        items += [
            ("config/objects/overlays.compile.json", "compile-manifest.json", "contract"),
            (f"config/symbols/{unit}.main-symbols.ld", "main_symbols.ld", "text"),
            (f"config/objects/{unit}.layout.json", "layout.json", "json"),
        ]
    return items


def carry_accepted_asm(root, manifest):
    """Keep the checkable part of an accepted assembly record with the object.

    The acceptance records themselves are not tracked, but a TU that includes
    accepted assembly ships that `.s` in `src/<unit>/<tu>/`, and two facts about
    it can be checked anywhere: its SHA-256, and the function names it must
    define.  Carrying those two here lets the ACCEPTED_ASM provenance gate run
    without the records.  Where a record is at hand the values come from it;
    otherwise the ones already tracked are kept.
    """
    for tu in manifest.get("tus", []):
        for record in tu.get("accepted", {}).get("records", []):
            if record.get("language") != "ee-asm":
                continue
            path = Path(root) / record["unit_record"]
            if not path.is_file():
                continue
            data = json.loads(path.read_bytes())
            record["source_sha256"] = data["source"]["sha256"]
            record["source_functions"] = [f["name"] for f in data["functions"]]
    return manifest


def tracked_bytes(unit_dir, name, kind, root=ROOT):
    """What the tracked copy of one generated file should contain."""
    data = (Path(unit_dir) / name).read_bytes()
    if kind not in ("manifest", "contract"):
        return data
    parsed = json.loads(data)
    if kind == "manifest":
        for key in UNTRACKED_MANIFEST_KEYS:
            parsed.pop(key, None)
        parsed = carry_accepted_asm(root, parsed)
    return (json.dumps(to_tokens(parsed), indent=1) + "\n").encode()


def stage(root, unit, unit_dir, tc=None, missing_ok=False):
    """Copy the tracked inputs of one unit into its build directory.

    A file that names tools names them by identity; staging resolves each one
    to its verified path on this machine, so the generated build.ninja runs the
    tools that are actually here.
    """
    root = Path(root)
    unit_dir = Path(unit_dir)
    unit_dir.mkdir(parents=True, exist_ok=True)
    tc = tc or tcmod.Toolchain(root)
    staged = []
    for tracked, name, kind in plan(unit):
        src = root / tracked
        if not src.is_file():
            if missing_ok:
                continue
            raise SystemExit(
                f"tracked-inputs: {tracked} is missing.\n"
                "  It is a tracked build input; a clone has it.  If you are "
                "regenerating the split, run this tool with --freeze first.")
        if kind in ("manifest", "contract"):
            resolved = to_paths(json.loads(src.read_bytes()), tc)
            (unit_dir / name).write_text(json.dumps(resolved, indent=1) + "\n")
        else:
            shutil.copyfile(src, unit_dir / name)
        staged.append(name)
    return staged


def freeze(root, unit, unit_dir):
    """Write the tracked inputs of one unit back from its build directory."""
    written = []
    for tracked, name, kind in plan(unit):
        src = Path(unit_dir) / name
        if not src.is_file():
            raise SystemExit(f"tracked-inputs: {src} does not exist; configure {unit} first")
        dst = Path(root) / tracked
        dst.parent.mkdir(parents=True, exist_ok=True)
        data = tracked_bytes(unit_dir, name, kind, root)
        if not dst.is_file() or dst.read_bytes() != data:
            dst.write_bytes(data)
            written.append(tracked)
    return written


def check(root, units=UNITS, build=None):
    """Report tracked inputs that are missing or differ from the built unit."""
    build = Path(build or Path(root) / "build")
    problems = []
    for unit in units:
        unit_dir = build / unit
        for tracked, name, kind in plan(unit):
            dst = Path(root) / tracked
            if not dst.is_file():
                problems.append(dict(unit=unit, path=tracked, problem="tracked input missing"))
                continue
            src = unit_dir / name
            if not src.is_file():
                problems.append(dict(unit=unit, path=tracked, problem="not configured",
                                     detail=f"{src} does not exist; run ./configure.py"))
                continue
            if dst.read_bytes() != tracked_bytes(unit_dir, name, kind, root):
                problems.append(dict(unit=unit, path=tracked, problem="differs from the built unit",
                                     detail=f"{src} disagrees; --freeze to adopt it"))
    return problems


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--root", type=Path, default=ROOT)
    ap.add_argument("--unit", choices=UNITS)
    ap.add_argument("--unit-dir", type=Path)
    ap.add_argument("--stage", action="store_true")
    ap.add_argument("--freeze", action="store_true")
    ap.add_argument("--check", action="store_true")
    a = ap.parse_args(argv)
    root = a.root.resolve()
    if a.check:
        problems = check(root, [a.unit] if a.unit else UNITS)
        for p in problems:
            print(f"CHECK {p['unit']} {p['path']}: {p['problem']}"
                  + (f" ({p['detail']})" if p.get("detail") else ""), file=sys.stderr)
        print(json.dumps(dict(checked=len(UNITS) if not a.unit else 1, problems=problems), indent=1))
        return 1 if problems else 0
    if not a.unit:
        ap.error("--stage and --freeze need --unit")
    unit_dir = a.unit_dir or root / "build" / a.unit
    if a.stage:
        print(json.dumps(dict(unit=a.unit, staged=stage(root, a.unit, unit_dir))))
    elif a.freeze:
        print(json.dumps(dict(unit=a.unit, written=freeze(root, a.unit, unit_dir))))
    else:
        ap.error("choose --stage, --freeze or --check")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
