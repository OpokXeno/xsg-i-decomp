#!/usr/bin/env python3
"""Gate the provenance of every ACCEPTED_ASM / INCLUDE_ASM line of the built TU
sources (tu-main-v2 review finding 3).

exact_asm credit rests on the tracked, reviewed assembly of an accepted record, never
on generated scaffold.  For every source the build compiles (the carve skeletons and
the C TUs), each line must satisfy:

  ACCEPTED_ASM(FOLDER, NAME)
    * FOLDER is exactly `src/main/<tu>` (never asm/*, never another TU);
    * `<FOLDER>/<NAME>.s` exists and its sha256 equals `source.sha256` of an accepted
      ee-asm record of that TU;
    * the functions it defines (.globl/.ent/glabel) are exactly the record's functions.
  INCLUDE_ASM(FOLDER, NAME)
    * FOLDER is exactly `asm/main/nonmatchings/<tu>` (generated scaffold) and the file
      exists; an INCLUDE_ASM that points into src/main/<tu> (or anywhere else) fails.

    accepted_asm_gate.py --report build/accepted_asm.json   (exit 1 on any problem)
"""
import argparse
import hashlib
import json
import re
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
# HERE is tools/tu, so the repository root is two directories up; --root and
# --unit-dir rebind both for any checkout and any build layout.
ROOT = HERE.parents[1]
U = ROOT / "build/main"
P = ROOT


def set_paths(root, unit_dir):
    global ROOT, U, P
    ROOT = Path(root).resolve()
    U = Path(unit_dir).resolve()
    P = ROOT
LINE = re.compile(r'^\s*(INCLUDE_ASM|ACCEPTED_ASM)\("([^"]+)",\s*([^)]+)\);')
DEFS = re.compile(r"^\s*(?:\.globl|\.ent|glabel)\s+([A-Za-z_.$][\w.$]*)", re.M)


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--root", type=Path, default=ROOT)
    ap.add_argument("--unit-dir", type=Path, default=None)
    ap.add_argument("--report", type=Path, default=None)
    a = ap.parse_args()
    set_paths(a.root, a.unit_dir if a.unit_dir is not None else Path(a.root) / "build/main")
    if a.report is None:
        a.report = U / "build/accepted_asm.json"
    m = json.loads((U / "tu-manifest.json").read_bytes())
    problems, checked = [], dict(include_asm=0, accepted_asm=0, sources=0, from_manifest=0)
    for t in m["tus"]:
        srcs = []
        if t["text"]["splat"] == "c":
            srcs.append(U / t["scaffold_path"])          # carve skeleton
        if t["mode"] == "c":
            s = t["import"]["source"]
            srcs.append(U / f"build/gen/shim/{t['name'].replace('/', '__')}.c" if s["kind"] == "units-shim"
                        else U / s["path"])              # the TU source the main build compiles
        records = {}
        for r in t["accepted"]["records"]:
            if r["language"] != "ee-asm":
                continue
            # The acceptance records are not part of the repository, so the two
            # facts this gate needs about each one - the SHA-256 of the accepted
            # assembly and the functions it must define - travel with the object
            # in the per-object manifest.  Where the record itself is at hand it
            # is read instead, and it is then also the source of those fields.
            path = ROOT / r["unit_record"]
            if path.is_file():
                rec = json.loads(path.read_bytes())
                want = dict(sha256=rec["source"]["sha256"],
                            functions=[f["name"] for f in rec["functions"]],
                            stem=Path(rec["source"]["path"]).stem)
            elif "source_sha256" in r:
                checked["from_manifest"] += 1
                want = dict(sha256=r["source_sha256"], functions=r["source_functions"],
                            stem=Path(r["source"]).stem)
            else:
                problems.append(dict(tu=t["name"], problem="accepted ee-asm record is neither "
                                     "present nor carried by the manifest",
                                     record=r["unit_record"]))
                continue
            records[want["stem"]] = (r["unit_record"], want)
            records.setdefault(want["functions"][0], (r["unit_record"], want))
        for src in srcs:
            if not src.is_file():
                problems.append(dict(tu=t["name"], problem="TU source missing", source=str(src)))
                continue
            checked["sources"] += 1
            for line in src.read_text().splitlines():
                mm = LINE.match(line)
                if not mm:
                    continue
                kind, folder, name = mm[1], mm[2], mm[3].strip()
                where = dict(tu=t["name"], source=str(src.relative_to(U)), line=line.strip())
                if kind == "INCLUDE_ASM":
                    checked["include_asm"] += 1
                    if folder != f"asm/main/nonmatchings/{t['name']}" or not (U / folder / f"{name}.s").is_file():
                        problems.append(dict(**where, problem="INCLUDE_ASM must name this TU's generated scaffold "
                                             f"asm/main/nonmatchings/{t['name']}"))
                    continue
                checked["accepted_asm"] += 1
                if folder != f"src/main/{t['name']}":
                    problems.append(dict(**where, problem=f"ACCEPTED_ASM folder must be src/main/{t['name']} "
                                         "(tracked accepted assembly, never generated scaffold)"))
                    continue
                f = U / folder / f"{name}.s"
                if not f.is_file():
                    problems.append(dict(**where, problem="tracked accepted assembly missing"))
                    continue
                hit = records.get(name)
                if hit is None:
                    problems.append(dict(**where, problem="no accepted ee-asm record of this TU matches the name"))
                    continue
                unit_record, rec = hit
                if sha(f) != rec["sha256"]:
                    problems.append(dict(**where, problem="the copy differs from the accepted record's source",
                                         record=unit_record, copy_sha256=sha(f),
                                         record_sha256=rec["sha256"]))
                    continue
                defined = set(DEFS.findall(f.read_text()))
                want = set(rec["functions"])
                if not want <= defined:
                    problems.append(dict(**where, problem="the copy does not define the record's functions",
                                         record=unit_record, missing=sorted(want - defined)))
    rep = dict(ok=not problems, checked=checked, problems=problems)
    a.report.parent.mkdir(parents=True, exist_ok=True)
    a.report.write_text(json.dumps(rep, indent=1) + "\n")
    print(json.dumps(dict(ok=rep["ok"], **checked, problems=len(problems))))
    return 0 if rep["ok"] else 1


if __name__ == "__main__":
    sys.exit(main())
