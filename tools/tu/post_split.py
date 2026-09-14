#!/usr/bin/env python3
"""Hook run after every splat run of the main tree (build_all.sh does it).

1. Legacy dialect (tools/legacy_dialect.py, unchanged copy of the tu-asmcompat
   tool, sha256 8fe8cec8...; its defaults): every generated function file
   asm/main/nonmatchings/**/*.s is rewritten in place into the GNU as
   2.9-ee-991111 dialect assembled by the LA29-vsqrt assembler inside the C TUs
   (symbolic gp `op r, sym+off` + `.extern sym,N`; `vsqrt Q, $vfNf`;
   `c1 0xIMM` -> `sqrt.s`; bare ACC/Q/I/R).  include/labels.inc becomes the
   legacy variant (alabel without .aent, jlabel global).  Modern GAS keeps
   assembling only the data/rodata/bss/bin scaffold (include/macro.inc).
   The hand-written asm objects (src/main/**.s, splat `hasm`, written once) get
   the same spelling rewrite in numeric gp mode (-G0 objects) and include
   labels.inc instead of macro.inc.  Every rewrite is idempotent.
2. ACCEPTED_ASM wiring: for every standalone ee-asm accepted game record
   (tu-manifest.json accepted.migration_hints state_hint exact_asm), a tracked
   copy of the accepted .s (byte-identical, sha256 checked against the record)
   is placed under src/main/<tu>/, and the TU's skeleton INCLUDE_ASM lines of
   exactly the record's functions are replaced by one
   ACCEPTED_ASM("src/main/<tu>", <stem>) line.  Only skeleton (all-INCLUDE_ASM)
   files are touched; a TU file that already contains C is reported, not edited.
(`.extern` hygiene for C TUs, review finding F3, is done at build time by
tools/cinc.py on a per-TU overlay, so the shared generated files never change.)

Writes main/post-split.json (statistics, flags, wiring, skipped cases).
"""
import argparse
import hashlib
import json
import re
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import legacy_dialect as ld  # noqa: E402

# Repository root and MAIN unit build directory; both are rebound by --root and
# --unit-dir so the promoted tool works from any checkout and any build layout.
ROOT = Path(__file__).resolve().parents[2]
U = ROOT / "build/main"


def set_paths(root, unit_dir):
    global ROOT, U
    ROOT = Path(root).resolve()
    U = Path(unit_dir).resolve()
LEGACY_DIALECT_SHA256 = "8fe8cec8eb7ba2048d796d6bf276a53dad017da2b9fcb84aafe9ef365af94464"
INC_RE = re.compile(r'^(INCLUDE_ASM|ACCEPTED_ASM)\("([^"]+)", ([^)]+)\);\s*$')


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def write_if_changed(path, text):
    if path.read_text() != text:
        path.write_text(text)
        return True
    return False


def dialect(report):
    assert sha(HERE / "legacy_dialect.py") == LEGACY_DIALECT_SHA256, "legacy_dialect.py changed"
    table, sizes = ld.load_symbols(U)
    stats = dict(files=0, changed=0, gp_rel=0, vsqrt=0, c1_sqrt_s=0)
    flags = {}
    for f in sorted([*(U / "asm/main/nonmatchings").rglob("*.s"), *(U / "asm/main/matchings").rglob("*.s")]):
        text, fl = ld.convert(f.read_text(), "symbolic", 8, table, sizes, stats, "mnemonic", [])
        stats["files"] += 1
        stats["changed"] += write_if_changed(f, text)
        if fl:
            flags[str(f.relative_to(U))] = sorted(fl)
    hstats = dict(files=0, changed=0, gp_rel=0, vsqrt=0, c1_sqrt_s=0)
    manifest = json.loads((U / "tu-manifest.json").read_bytes())
    for t in manifest["tus"]:
        if t["kind"] != "asm":
            continue
        f = U / t["scaffold_path"]
        text, fl = ld.convert(f.read_text(), "numeric", 0, table, sizes, hstats, "mnemonic", [])
        text = text.replace('.include "macro.inc"', '.include "labels.inc"')
        hstats["files"] += 1
        hstats["changed"] += write_if_changed(f, text)
        if fl:
            flags[str(f.relative_to(U))] = sorted(fl)
    splat_labels = (U / "include/labels.inc").read_text()
    if not splat_labels.startswith("# Legacy"):
        (U / "include/labels.inc").write_text(ld.legacy_labels(splat_labels))
    report["dialect"] = dict(tool="tools/legacy_dialect.py", sha256=LEGACY_DIALECT_SHA256,
                             options=dict(gp="symbolic", G=8, vsqrt="mnemonic"), nonmatchings=stats,
                             hand_asm=dict(hstats, gp="numeric", G=0), flags=flags,
                             labels_inc=sha(U / "include/labels.inc"))


def wire_accepted_asm(report):
    manifest = json.loads((U / "tu-manifest.json").read_bytes())
    wired, skipped = [], []
    for t in manifest["tus"]:
        hints = [h for h in t["accepted"]["migration_hints"] if h["state_hint"] == "exact_asm"]
        if not hints:
            continue
        cfile = U / t["scaffold_path"]
        lines = cfile.read_text().splitlines()
        is_skeleton = all(not l.strip() or l.startswith(("#include", "INCLUDE_ASM(", "ACCEPTED_ASM(", "/*", " *"))
                          for l in lines)
        if not is_skeleton or t["mode"] == "c":
            skipped.append(dict(tu=t["name"], reason="TU file is not an all-INCLUDE_ASM skeleton"))
            continue
        by_va = {int(f["va"], 16): f["splat_name"] for f in t["functions"]}
        order = sorted(by_va)
        # The acceptance records are not part of the repository, so the three
        # facts needed here - the accepted assembly, its SHA-256 and the
        # functions it carries - travel with the object in the per-object
        # manifest.  Where a record is at hand it is read instead.
        carried = {r["unit_record"]: r for r in t["accepted"]["records"]}
        for hnt in hints:
            path = ROOT / hnt["unit_record"]
            if path.is_file():
                rec = json.loads(path.read_bytes())
                source, digest = rec["source"]["path"], rec["source"]["sha256"]
                functions = [f["name"] for f in rec["functions"]]
            else:
                entry = carried.get(hnt["unit_record"], {})
                if "source_sha256" not in entry:
                    skipped.append(dict(tu=t["name"], record=hnt["unit_record"],
                                        reason="accepted ee-asm record is neither present "
                                               "nor carried by the manifest"))
                    continue
                source, digest = entry["source"], entry["source_sha256"]
                functions = entry["source_functions"]
            src = ROOT / source
            assert sha(src) == digest, src
            stem = src.stem if src.stem != "candidate" else functions[0]
            want = set(functions)
            vas = [int(f["va"], 16) for f in t["functions"] if f["name"] in want]
            names = [by_va[v] for v in vas]
            i0 = order.index(vas[0])
            if order[i0:i0 + len(vas)] != vas:
                skipped.append(dict(tu=t["name"], record=hnt["unit_record"], reason="functions not consecutive"))
                continue
            dst = U / "src/main" / t["name"] / f"{stem}.s"
            dst.parent.mkdir(parents=True, exist_ok=True)
            if not dst.exists() or dst.read_bytes() != src.read_bytes():
                dst.write_bytes(src.read_bytes())
            folder = f"src/main/{t['name']}"
            new, done = [], False
            for l in lines:
                m = INC_RE.match(l)
                if m and m[1] == "INCLUDE_ASM" and m[3] in names:
                    if not done:
                        new.append(f'ACCEPTED_ASM("{folder}", {stem});')
                        done = True
                    continue
                if m and m[1] == "ACCEPTED_ASM" and m[3] == stem:
                    done = True
                new.append(l)
            # drop blank-line runs left by removed INCLUDE_ASM lines
            text = re.sub(r"\n{3,}", "\n\n", "\n".join(new) + "\n")
            write_if_changed(cfile, text)
            lines = text.splitlines()
            wired.append(dict(tu=t["name"], record=hnt["unit_record"], source=source,
                              source_sha256=digest, copy=str(dst.relative_to(U)),
                              line=f'ACCEPTED_ASM("{folder}", {stem});', functions=functions,
                              tu_flags=t["contract"]["flags"]))
    report["accepted_asm"] = dict(wired=wired, skipped=skipped)


def label_fidelity(report):
    """INCLUDE_ASM bodies define every function under its ORIGINAL name and binding
    (tu-main-v3 item b): `glabel X, local` for originally LOCAL functions, the original
    spelling for names splat suffixed with _<VA> (duplicates).  References inside the
    TU's own function files follow the rename; references from other scaffold objects
    keep the splat name and resolve through build/scaffold_aliases.ld (PROVIDE relative
    to the TU text start, generated by configure.py from main/scaffold-labels.json)."""
    manifest = json.loads((U / "tu-manifest.json").read_bytes())
    labels, changed = [], 0
    for t in manifest["tus"]:
        if t["text"]["splat"] != "c" or t.get("alias_of"):
            continue
        todo = [f for f in t["functions"] if f["binding"] == "LOCAL" or f["splat_name"] != f["name"]]
        if not todo:
            continue
        files = [*(U / "asm/main/nonmatchings" / t["name"]).glob("*.s"), *(U / "asm/main/matchings" / t["name"]).glob("*.s")]
        texts = {f: f.read_text() for f in files}
        for fn in todo:
            sp, nm, local = fn["splat_name"], fn["name"], fn["binding"] == "LOCAL"
            vis = ", local" if local else ""
            for f, text in texts.items():
                new = re.sub(rf"^glabel {re.escape(sp)}\s*$", f"glabel {nm}{vis}", text, flags=re.M)
                if sp != nm:
                    word = re.compile(rf"(?<![\w.$]){re.escape(sp)}(?![\w.$])")
                    new = "\n".join(l if l.lstrip().startswith("nonmatching ") else word.sub(nm, l)
                                    for l in new.split("\n"))
                texts[f] = new
            labels.append(dict(tu=t["name"], name=nm, splat_name=sp, va=fn["va"], binding=fn["binding"],
                               text_start=t["text"]["splat_range"][0]))
        for f, text in texts.items():
            if f.read_text() != text:
                f.write_text(text)
                changed += 1
    (U / "scaffold-labels.json").write_text(json.dumps(labels, indent=1) + "\n")
    report["label_fidelity"] = dict(functions=len(labels), local=sum(l["binding"] == "LOCAL" for l in labels),
                                    renamed=sum(l["name"] != l["splat_name"] for l in labels), files_changed=changed)


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--root", type=Path, default=ROOT)
    ap.add_argument("--unit-dir", type=Path, default=None)
    a = ap.parse_args(argv)
    set_paths(a.root, a.unit_dir if a.unit_dir is not None else Path(a.root) / "build/main")
    report = {}
    dialect(report)
    label_fidelity(report)
    wire_accepted_asm(report)
    (U / "post-split.json").write_text(json.dumps(report, indent=1) + "\n")
    print(json.dumps(dict(nonmatchings=report["dialect"]["nonmatchings"], hand_asm=report["dialect"]["hand_asm"],
                          label_fidelity=report["label_fidelity"],
                          flagged=len(report["dialect"]["flags"]), accepted_asm_wired=len(report["accepted_asm"]["wired"]),
                          accepted_asm_skipped=len(report["accepted_asm"]["skipped"]))))


if __name__ == "__main__":
    main()
