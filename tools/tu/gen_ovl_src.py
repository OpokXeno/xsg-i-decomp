#!/usr/bin/env python3
"""Generate per-original-TU C files from accepted sources + INCLUDE_ASM.

    gen_src.py <unit_dir> <unit> --plan plan.json [--adapt adaptations.json]

For every TU the plan marks `c`, splat has written src/<unit>/<tu>.c (all
INCLUDE_ASM) and asm/nonmatchings/<unit>/<tu>/<func>.s.  This script rewrites the
C file: accepted C functions (verbatim function text from the accepted source,
in original address order), the accepted standalone EE assembly units through
ACCEPTED_ASM of an unmodified copy under src/<unit>/<tu>/, and INCLUDE_ASM for
every other function.
File-scope declarations of the accepted sources (their shared-header includes,
private.h bodies inlined, file-scope declarations) form the TU preamble,
de-duplicated.  Declarations that cannot coexist in one TU are handled only by
the explicit, recorded adaptations in adaptations.json (drop/replace one
preamble item), never silently.

TU-local functions (LOCAL in the original ELF) are named <name>_<VA> by the
splat symbol set so that asm TUs can define them globally; inside a C TU they
get their original name back (definitions and references in that TU's
INCLUDE_ASM bodies) and are defined `local`, matching the C `static`.
Writes <unit_dir>/tu-src-report.json.
"""
import argparse
import hashlib
import json
import re
import shutil
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import csplit  # noqa: E402
from elfinfo import Elf  # noqa: E402

# HERE is tools/tu, so the repository root is two directories up; --root rebinds it.
ROOT = HERE.parents[1]
SHARED_INC = ROOT / "include"
UNITS_DIR = ROOT / "config/units"


def set_root(root):
    global ROOT, SHARED_INC, UNITS_DIR
    ROOT = Path(root).resolve()
    SHARED_INC = ROOT / "include"
    UNITS_DIR = ROOT / "config/units"


def h(x):
    return int(x, 16)


def sha(b):
    return hashlib.sha256(b).hexdigest()


def norm(t):
    t = re.sub(r"/\*.*?\*/", " ", t, flags=re.S)
    t = re.sub(r"//[^\n]*", " ", t)
    return re.sub(r"\s+", " ", t).strip()


def header_items(path, seen_guards):
    """Items of a quoted private header, include guard removed."""
    text = path.read_text()
    items = csplit.split(text)
    if items and items[0][0] == "pp" and re.match(r"#\s*ifndef\s+\w+", items[0][1]):
        guard = items[0][1].split()[1]
        assert re.match(r"#\s*define\s+" + guard, items[1][1]) and re.match(r"#\s*endif", items[-1][1]), path
        items = items[2:-1]
    return items


def declared_name(item):
    """Identifier a file-scope declaration/#define declares (None for #include etc.)."""
    t = norm(item)
    m = re.match(r"#\s*define\s+(\w+)", t)
    if m:
        return m.group(1)
    if t.startswith("#"):
        return None
    m = re.search(r"\(\s*\*\s*(\w+)\s*\)", t)
    if t.startswith("typedef") and m:
        return m.group(1)
    if t.startswith("typedef"):
        return re.findall(r"\w+", t)[-1]
    depth, head = 0, ""
    for c in t:
        if c == "(" and depth == 0:
            ids = re.findall(r"\w+", head)
            return ids[-1] if ids else None
        head += c
        if c in "[=;":
            break
    ids = re.findall(r"[A-Za-z_]\w*", t.split("[")[0].split("=")[0].rstrip(";"))
    return ids[-1] if ids else None


ACCEPTED_ASM_DEF = """#ifndef ACCEPTED_ASM
/* Previously accepted standalone EE assembly (exact_asm): tracked, reviewed .s
 * under src/<unit>/<tu>/. Same expansion as INCLUDE_ASM; the separate name keeps
 * accepted assembly distinguishable from generated scaffolding. */
#define ACCEPTED_ASM(FOLDER, NAME) \\
    __asm__( \\
        ".section .text\\n" \\
        "    .set noat\\n" \\
        "    .set noreorder\\n" \\
        "    .include \\"" FOLDER "/" #NAME ".s\\"\\n" \\
        "    .set reorder\\n" \\
        "    .set at\\n" \\
    )
#endif
"""


def add_accepted_asm_macro(path):
    """Add ACCEPTED_ASM next to splat's INCLUDE_ASM in the generated include_asm.h
    (and its empty M2CTX/PERMUTER variant). Idempotent."""
    text = path.read_text()
    if "define ACCEPTED_ASM" in text:
        return
    anchor = "#ifndef INCLUDE_RODATA\n#define INCLUDE_RODATA(FOLDER, NAME) \\\n"
    assert text.count(anchor) == 1, path
    text = text.replace(anchor, ACCEPTED_ASM_DEF + anchor, 1)
    anchor2 = "#ifndef INCLUDE_RODATA\n#define INCLUDE_RODATA(FOLDER, NAME)\n"
    assert text.count(anchor2) == 1, path
    text = text.replace(anchor2, "#ifndef ACCEPTED_ASM\n#define ACCEPTED_ASM(FOLDER, NAME)\n#endif\n" + anchor2, 1)
    path.write_text(text)


def load_symbols(unit_dir):
    by_va = {}
    for l in (unit_dir / "symbol_addrs.txt").read_text().splitlines():
        if "=" not in l:
            continue
        n, v = l.split("//")[0].split("=")
        by_va.setdefault(h(v.strip().rstrip(";")), n.strip())
    return by_va


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("unit_dir", type=Path)
    ap.add_argument("unit")
    ap.add_argument("--root", type=Path, default=None)
    ap.add_argument("--scaffold-out", action="store_true",
                    help="write the regenerated TU file under scaffold/src/ and never over "
                         "the published source at src/<unit>/<tu>.c; the accepted .s copies "
                         "are likewise left alone. The INCLUDE_ASM body rewrite (TU-local "
                         "names, `glabel X, local`) still happens, because the published "
                         "source depends on it.")
    ap.add_argument("--plan", type=Path, default=None)
    ap.add_argument("--adapt", type=Path)
    a = ap.parse_args()
    # `--root` first: everything below is resolved against it.
    if a.root is not None:
        set_root(a.root)
    if a.plan is None:
        a.plan = ROOT / "config/overlay-plan.json"
    plan = json.loads(a.plan.read_text())["units"][a.unit]
    adapt = json.loads(a.adapt.read_text()).get(a.unit, {}) if a.adapt and a.adapt.exists() else {}
    # The unit's object partition and its accepted sets are a tracked build
    # input (config/objects/overlays.compile.json, staged as compile-manifest.json).
    manifest = json.loads((a.unit_dir / "compile-manifest.json").read_text())["units"][a.unit]
    # The original is the authority for function names, addresses and binding.
    # configure.py staged it here and checked its SHA-256 before this pass ran.
    original = json.loads((ROOT / "config/originals.json").read_text())["units"][a.unit]["file"]
    elf = Elf((a.unit_dir / "orig" / original).read_bytes())
    text_syms = sorted((s.value, s.name, s.bind)
                       for s in elf.symbols if s.shndx == 1 and s.type == 2)
    splat_name = load_symbols(a.unit_dir)
    # The acceptance records are not part of the repository.  Where they are at
    # hand the accepted sources are checked against them as before; where they
    # are not, the names, the addresses, the binding and the hashes of the
    # accepted assembly still come from the original and the tracked manifest,
    # and the tool says which check it could not perform.
    records_available = UNITS_DIR.is_dir()
    unchecked_records = set()
    report = {}
    for t in manifest["tus"]:
        if t["ordinal"] not in set(plan.get("c_tus", [])):
            continue
        tu = t["name"]
        cpath = (a.unit_dir / "scaffold/src" / a.unit / f"{tu}.c" if a.scaffold_out
                 else a.unit_dir / "src" / a.unit / f"{tu}.c")
        # With the TU source published, the C text this generator would rebuild is
        # already in the tree and is what the build compiles; only the INCLUDE_ASM
        # body renames, the ACCEPTED_ASM provenance check and the macro header are
        # still this pass's work. Rebuilding it would also need the accepted
        # records' pre-migration sources, which were archived on 2026-09-12
        # (task tu-retire-legacy-src) - the records point at the published TU file
        # itself now, and that file cannot be re-split into the per-record
        # fragments it was assembled from.
        reconstruct = not (a.scaffold_out and (ROOT / "src" / a.unit / f"{tu}.c").is_file())
        cpath.parent.mkdir(parents=True, exist_ok=True)
        asm_dir = f"asm/nonmatchings/{a.unit}/{tu}"
        # The functions of this object, from the original's symbol table.
        lo, hi = h(t["text"][0]), h(t["text"][1])
        funcs = [dict(va="0x%08x" % va, name=name,
                      binding="local" if bind == 0 else "global")
                 for va, name, bind in text_syms if lo <= va < hi]
        if len(funcs) != t["functions"]:
            raise SystemExit(f"gen_ovl_src: {a.unit}/{tu} has {len(funcs)} functions in "
                             f"{original} but the compile contract says {t['functions']}")
        # A TU just promoted into `c_tus` has nothing accepted yet, so the compile
        # manifest carries none of these keys.  That is the legitimate initial
        # state of every skeleton -- the same state main's 400 skeletons are in --
        # and demanding the keys made promotion look like a tool crash
        # (KeyError: 'c_functions') rather than the one-line config change it is.
        accepted_names = (set(t.get("c_functions") or ())
                          | set(t.get("accepted_asm_functions") or ()))
        acc = {h(f["va"]): f for f in funcs if f["name"] in accepted_names}
        # name -> the accepted standalone-assembly unit that carries it
        asm_unit = {n: e for e in (t.get("accepted_asm_files") or ())
                    for n in e["functions"]}
        recs = {rp: [] for rp in (t.get("accepted_records") or ())}
        rec_items, rec_lang, rec_json, rec_helpers = {}, {}, {}, {}
        preamble, seen, split_dropped = [], set(), {}
        adaptations_used = []
        drop = {norm(x) for x in adapt.get(tu, {}).get("drop_preamble", [])}
        if not records_available and reconstruct:
            raise SystemExit(
                f"gen_ovl_src: {a.unit}/{tu} has no published source at "
                f"src/{a.unit}/{tu}.c, and the acceptance records it would be "
                "rebuilt from are not part of the repository (config/units)")
        if not records_available:
            unchecked_records.update(recs)
        tu_rel = f"src/{a.unit}/{tu}.c"
        published_defs, record_pins = None, []
        for rp in (sorted(recs) if records_available else []):
            rec = json.loads((ROOT / rp).read_text())
            rec_json[rp] = rec
            src = ROOT / rec["source"]["path"]
            lang = "ee-asm" if src.suffix == ".s" else "c"
            rec_lang[rp] = lang
            on_tu = [s for s in (rec["source"].get("tu_sources") or ()) if s.get("path") == tu_rel]
            if lang == "c" and not reconstruct and on_tu:
                # The record was repointed at the published TU file (task
                # tu-retire-legacy-src), and that file, not the record, is what the
                # build compiles here. Its whole-file `sha256` is then a property of
                # the published baseline: it moves whenever anything else in the
                # file is edited (another function, a header move, a comment), so
                # it neither proves nor disproves anything about this record's
                # functions and must not stop the build. It is reported
                # (`record_pins`), exactly as tools/tu_audit.py reports it
                # (`accepted_records.stale_pins`); the body is judged by the audit
                # against the archived pre-migration source. What this pass still
                # requires is that every function of the record is a C definition
                # of the published TU file.
                if published_defs is None:
                    published_defs = {n for k, _, n in csplit.split((ROOT / tu_rel).read_text())
                                      if k == "func"}
                on_disk = sha((ROOT / tu_rel).read_bytes())
                for s in on_tu:
                    missing = [n for n in s.get("functions", ()) if n not in published_defs]
                    if missing:
                        raise SystemExit(f"gen_ovl_src: {rp}: {', '.join(missing)} is not a C "
                                         f"definition of the published {tu_rel}")
                    pinned = sorted({s["sha256"]} | ({rec["source"]["sha256"]}
                                                     if rec["source"]["path"] == tu_rel else set()))
                    record_pins.append(dict(record=rp, source=tu_rel,
                                            pin="matches" if pinned == [on_disk] else "stale",
                                            on_disk=on_disk, pinned=pinned))
                continue
            # The record's own source is read (a TU rebuilt from it) or it is not the
            # published TU file (an accepted .s, a legacy per-function source): its
            # pin is the integrity of exactly what is consumed, and it is enforced.
            if sha(src.read_bytes()) != rec["source"]["sha256"]:
                raise SystemExit(f"gen_ovl_src: {rp}: {rec['source']['path']} does not hash to "
                                 f"the record's source.sha256")
            if lang == "ee-asm" or not reconstruct:
                continue
            items = csplit.split(src.read_text())
            names = {f["name"] for f in rec["functions"]}
            rec_items[rp] = {n: b for k, b, n in items if k == "func" and n in names}
            rec_helpers[rp] = [b for k, b, n in items if k == "func" and n not in names]
            lead_comments, rec_pre = [], []
            for k, b, n in items:
                if k == "func":
                    continue
                if k == "comment":
                    lead_comments.append(b)
                    continue
                m = re.match(r'#\s*include\s+"([^"]+)"', b)
                sub = [(k, b, n)]
                # a shared header is left as an `#include`; only a header that
                # really sits beside the record source is inlined
                if m and (src.parent / m.group(1)).is_file():
                    hp = src.parent / m.group(1)
                    sub = header_items(hp, seen)
                for k2, b2, n2 in sub:
                    if k2 == "comment":
                        continue
                    if norm(b2) in drop:
                        adaptations_used.append(dict(kind="drop_preamble", item=b2, record=rp,
                                                     reason=adapt[tu].get("reason")))
                        continue
                    rec_pre.append(b2)
            rec_helpers[rp] = lead_comments + rec_helpers[rp]
            here = [n for n in names if any(f["name"] == n for f in funcs)]
            if len(here) < len(names):
                # an accepted record that spans two original TUs: this TU's file keeps
                # only the file-scope declarations its own functions of that record use
                need = " ".join(norm(rec_items[rp][n]) for n in here) + " ".join(map(norm, rec_helpers[rp]))
                kept, changed = [], True
                while changed:
                    changed = False
                    for b2 in rec_pre:
                        if b2 in kept:
                            continue
                        dn = declared_name(b2)
                        if dn is None or re.search(r"\b" + re.escape(dn) + r"\b", need):
                            kept.append(b2)
                            need += " " + norm(b2)
                            changed = True
                split_dropped[rp] = [b2 for b2 in rec_pre if b2 not in kept]
                rec_pre = [b2 for b2 in rec_pre if b2 in kept]
            for b2 in rec_pre:
                if norm(b2) not in seen:
                    seen.add(norm(b2))
                    preamble.append(b2)
        # local-function renames inside this TU's INCLUDE_ASM bodies
        renames = {}
        for f in funcs:
            va = h(f["va"])
            if f["binding"] == "local" and splat_name.get(va) != f["name"]:
                renames[splat_name[va]] = f["name"]
        body, emitted_helpers, included = [], set(), []
        done_asm_units, accepted_asm_files = set(), []
        c_funcs, asm_accepted = [], []
        for f in funcs:
            va = h(f["va"])
            if va in acc:
                e = asm_unit.get(f["name"])
                if e is not None:
                    first = e["functions"][0]
                    if first in done_asm_units:
                        asm_accepted.append(f["name"])
                        continue
                    done_asm_units.add(first)
                    # tracked, reviewed .s beside the TU (AGENTS.md TU mode: ACCEPTED_ASM
                    # from src/<unit>/<tu>/, never generated asm/)
                    dst = a.unit_dir / e["file"]
                    if not (a.scaffold_out and dst.is_file()):
                        if not records_available:
                            raise SystemExit(f"gen_ovl_src: {e['file']} is missing; it is "
                                             "the tracked accepted assembly of this TU")
                        dst.parent.mkdir(parents=True, exist_ok=True)
                        shutil.copyfile(ROOT / rec_json[e["record"]]["source"]["path"], dst)
                    # The tracked .s must hash to what was accepted; the hash travels
                    # with the object in the compile contract, so this holds whether or
                    # not the record itself is here.  The published file is never
                    # rewritten (the unit's `src` is a link to the repository tree).
                    if sha(dst.read_bytes()) != e["sha256"]:
                        raise SystemExit(f"gen_ovl_src: {e['file']} differs from the "
                                         f"accepted assembly of {e['record']}")
                    accepted_asm_files.append(dict(e))
                    body.append(f"/* exact_asm: {e['record']} ({e['source']}) */\n"
                                f'ACCEPTED_ASM("src/{a.unit}/{tu}", {first});')
                    asm_accepted.append(f["name"])
                    continue
                if not reconstruct:
                    c_funcs.append(f["name"])
                    continue
                rp = acc[va]["unit_record"]
                if rp not in emitted_helpers:
                    emitted_helpers.add(rp)
                    body.extend(rec_helpers[rp])
                ftext = rec_items[rp][f["name"]]
                for r in adapt.get(tu, {}).get("replace", []):
                    if r["function"] == f["name"]:
                        assert ftext.count(r["old"]) == 1, (tu, r)
                        ftext = ftext.replace(r["old"], r["new"])
                        adaptations_used.append(dict(kind="replace", function=f["name"], old=r["old"],
                                                     new=r["new"], reason=r.get("reason")))
                body.append(ftext)
                c_funcs.append(f["name"])
            else:
                sn = splat_name[va]
                p = a.unit_dir / asm_dir / f"{sn}.s"
                assert p.exists(), p
                text = p.read_text()
                out = []
                for line in text.splitlines(keepends=True):
                    if not line.lstrip().startswith("nonmatching "):
                        for old, new in renames.items():
                            line = re.sub(r"(?<![\w.$])" + re.escape(old) + r"(?![\w$])", new, line)
                    out.append(line)
                text = "".join(out)
                if f["binding"] == "local":
                    # a C-TU static: defined local, like the compiler's own statics
                    text = re.sub(r"^glabel (\S+)$", r"glabel \1, local", text, flags=re.M)
                p.write_text(text)
                body.append(f'INCLUDE_ASM("{asm_dir}", {sn});')
                included.append(sn)
        # INCLUDE_ASM bodies whose name changed: the file name keeps the splat name
        # One-line banner (user direction 2026-09-19): the original TU and its
        # extent only; name, accepted lists and provenance are not repeated here.
        head = [f"/*\n * {a.unit.upper()} original TU {t['ordinal']}: {t['text'][0]}..{t['text'][1]}"
                f" ({t['functions']} functions)\n */",
                '#include "common.h"']
        pp_inc = [p for p in preamble if p.lstrip().startswith("#include")]
        rest = [p for p in preamble if not p.lstrip().startswith("#include")]
        for extra in adapt.get(tu, {}).get("add_preamble", []):
            (pp_inc if extra.lstrip().startswith("#include") else rest).append(extra)
            adaptations_used.append(dict(kind="add_preamble", item=extra))
        text = "\n".join(head + pp_inc) + "\n\n" + ("\n".join(rest) + "\n\n" if rest else "") + "\n\n".join(body) + "\n"
        if reconstruct:
            cpath.write_text(text)
        else:
            cpath = ROOT / "src" / a.unit / f"{tu}.c"
            text = cpath.read_text()
        report[tu] = dict(ordinal=t["ordinal"], path=str(cpath), records=sorted(recs),
                          reconstructed=reconstruct,
                          c_functions=c_funcs, accepted_asm_functions=asm_accepted,
                          include_asm_functions=included, local_renames=renames,
                          adaptations=adaptations_used, split_unused_declarations=split_dropped,
                          accepted_asm_files=accepted_asm_files,
                          header_divergence=adapt.get(tu, {}).get("header_divergence", []),
                          record_pins=record_pins,
                          sha256=sha(text.encode()))
    if report:
        add_accepted_asm_macro(a.unit_dir / "include/include_asm.h")
    (a.unit_dir / "tu-src-report.json").write_text(json.dumps(report, indent=1) + "\n")
    renamed = sum(len(r["local_renames"]) for r in report.values())
    asm_units = sum(len(r["accepted_asm_files"]) for r in report.values())
    print(f"{a.unit}: {len(report)} C TUs, {renamed} INCLUDE_ASM local renames, "
          f"{asm_units} accepted-assembly unit(s) checked against the compile contract")
    stale = sum(1 for r in report.values() for p in r["record_pins"] if p["pin"] == "stale")
    if stale:
        print(f"{a.unit}: {stale} accepted record(s) carry a whole-file source.sha256 that no "
              f"longer matches the published TU file they point at; their functions are still "
              f"C definitions of it (bookkeeping, not a build input: see record_pins in "
              f"tu-src-report.json and tools/tu_audit.py accepted_records.stale_pins)")
    if unchecked_records:
        print(f"{a.unit}: the acceptance records are not part of this repository, so the "
              f"source-integrity check of {len(unchecked_records)} record(s) was not "
              f"performed; names, addresses, binding and the accepted-assembly hashes "
              f"were checked against {original} and compile-manifest.json")


if __name__ == "__main__":
    main()
