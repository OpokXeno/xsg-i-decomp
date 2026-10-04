#!/usr/bin/env python3
"""Guarded overlay COMMON allocation and native compiler storage proof capture.

The published build uses the same guarded allocator as private builds. It turns a
compiler COMMON object into an ordinary relocatable .bss object only when every
COMMON/SCOMMON storage object in that input is explicitly named by the ownership
record.  Unclaimed COMMON storage stays in the raw object and is never consumed
by the allocation step.
"""
import argparse
import hashlib
import json
import subprocess
import sys
from pathlib import Path

# Resolve imports from this repository-layout copy, and use the explicit
# --root argument for project data inside the selected private snapshot.
ROOT = Path(__file__).resolve().parents[2]
TOOLS = ROOT / "tools/tu"
sys.path.insert(0, str(TOOLS))
from elfinfo import Elf  # noqa: E402
from toolchain import Toolchain  # noqa: E402

SHN_COMMON = 0xFFF2
SHN_SCOMMON = 0xFF03


def sha(path):
    h = hashlib.sha256()
    with Path(path).open("rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def alloc_symbols(elf):
    return [s for s in elf.symbols if s.name and s.shndx in (SHN_COMMON, SHN_SCOMMON)]


def bss_assembly_extent(assembly, name, root=ROOT):
    """Read a local cc1 BSS label's size/alignment from its emitted directives.

    GCC 2.96 emits local static objects as a label followed by .align and
    .space, but gives their ELF symbols STT_NOTYPE/size 0. Accept only the
    exact non-APP label plus a single explicit space directive; the full gate
    separately pins C source, relocation use, map address and linked symbol.
    """
    assembly = Path(assembly)
    if not assembly.is_absolute():
        assembly = Path(root) / assembly
    lines = assembly.read_text(errors="replace").splitlines()
    for index, line in enumerate(lines):
        if line.strip() != f"{name}:":
            continue
        app = False
        for prior in lines[:index]:
            marker = prior.strip()
            if marker == "#APP":
                app = True
            elif marker == "#NO_APP":
                app = False
        if app:
            raise SystemExit(f"allocation refused: {name} cc1 storage label is inside #APP")
        align = None
        size = None
        for following in lines[index + 1:index + 8]:
            text = following.strip()
            if text.startswith(".align"):
                try:
                    align = 1 << int(text.split()[1].split(",")[0], 0)
                except (ValueError, IndexError):
                    raise SystemExit(f"allocation refused: cannot parse {name} cc1 alignment")
            elif text.startswith(".space"):
                try:
                    size = int(text.split()[1].split(",")[0], 0)
                except (ValueError, IndexError):
                    raise SystemExit(f"allocation refused: cannot parse {name} cc1 object extent")
                break
            elif text.startswith((".previous", ".section", ".globl", ".ent", ".type")):
                break
        if size is None or align is None:
            raise SystemExit(f"allocation refused: {name} lacks explicit local cc1 .align/.space storage")
        return size, align
    raise SystemExit(f"allocation refused: {name} has no cc1 local storage label")


def allocate(root, record, tu, raw, out, report):
    root = Path(root).resolve()
    raw, out, report = Path(raw).resolve(), Path(out).resolve(), Path(report).resolve()
    rec = json.loads(Path(record).read_text())
    if 'overlay_bss_records' in rec:
        rec = rec['overlay_bss_records'].get(tu.split('/')[0], {})
        path_keys = {'source_path', 'object_path', 'assembly_path',
                     'raw_input_object', 'object', 'cc1_assembly'}

        def resolve_paths(value):
            if isinstance(value, list):
                return [resolve_paths(item) for item in value]
            if isinstance(value, dict):
                return {key: str(root / item) if key in path_keys and
                        isinstance(item, str) and not Path(item).is_absolute()
                        else resolve_paths(item) for key, item in value.items()}
            return value

        rec = resolve_paths(rec)
    short_tu = tu.split("_")[0]
    # Accept both the one-TU candidate records and the block-level compiler
    # report schema. Normalize the latter without weakening its per-row owner
    # and compiler-storage evidence.
    selected_report = next((item for item in rec.get("candidates", [])
                            if item.get("tu") == tu), None)
    if selected_report:
        candidates = []
        for location in selected_report.get("original_data", []):
            if location.get("section") not in (".bss", ".sbss"):
                continue
            owner = location.get("storage_owner")
            if not owner:
                continue
            storage = location.get("compiler_storage", {})
            candidates.append(dict(unit=selected_report.get("unit"),
                                   address=location.get("address"),
                                   section=location.get("section"),
                                   names=location.get("names", []),
                                   storage_owner=owner,
                                   compiler_storage=storage,
                                   evidence=dict(original_tu=tu,
                                                 assembly_path=selected_report.get("assembly"))))
    else:
        record_tu = rec.get("tu")
        candidates = [item for item in rec.get("recovered_location_candidates", [])
                      if item.get("section") in (".bss", ".sbss")
                      and (item.get("evidence", {}).get("original_tu") in (tu, short_tu)
                           or record_tu == tu)]
        if record_tu == tu:
            emission_rows = rec.get("storage_emission_evidence", [])
            normalized = []
            for item in candidates:
                if item.get("storage_owner"):
                    normalized.append(item)
                    continue
                address = str(item.get("address", "")).upper().replace("0X", "0x")
                name_set = set(item.get("names", []))
                proof = next((row for row in emission_rows
                              if str(row.get("address", "")).upper().replace("0X", "0x") == address
                              and (not name_set or row.get("symbol") in name_set
                                   or row.get("location_name") in name_set)), None)
                if proof is None:
                    normalized.append(item)
                    continue
                symbol = proof.get("symbol") or proof.get("location_name") or next(iter(name_set), None)
                obj_sym = proof.get("object_symbol", {})
                extent = proof.get("owner_extent", {})
                size = extent.get("size", proof.get("original_extent", obj_sym.get("size", 0)))
                alignment = extent.get("alignment", obj_sym.get("common_alignment"))
                owner_addr = extent.get("address", proof.get("address", item.get("address")))
                if symbol and size and alignment:
                    item = dict(item, storage_owner=dict(name=symbol, address=owner_addr,
                                size=size, alignment=alignment,
                                type=extent.get("type", proof.get("source_type"))))
                    item["evidence"] = dict(original_tu=tu,
                                             assembly_path=proof.get("cc1_assembly"),
                                             source_path=proof.get("source_definition", "").split(":",1)[0],
                                             source_sha256=proof.get("source_sha256"),
                                             object_path=proof.get("object"),
                                             object_sha256=proof.get("object_sha256"),
                                             assembly_sha256=proof.get("assembly_sha256"))
                    item["compiler_storage"] = dict(label=symbol,
                                             source_sha256=proof.get("source_sha256"),
                                             object_sha256=proof.get("object_sha256"),
                                             assembly_sha256=proof.get("assembly_sha256"),
                                             space=size, alignment=alignment)
                normalized.append(item)
            candidates = normalized
    if not candidates:
        raise SystemExit(f"allocation refused: no BSS ownership record for {tu}")
    wanted = {}
    for item in candidates:
        source_owner = item.get("storage_owner", {})
        owner = {key: source_owner[key] for key in
                 ("name", "address", "size", "alignment", "type", "linkage")
                 if key in source_owner}
        name = owner.get("name") or next(iter(item.get("names", [])), None)
        if not name:
            raise SystemExit(f"allocation refused: {tu} has a BSS location without an owner name")
        entry = wanted.setdefault(name, dict(section=item.get("section"), owner=owner, locations=[]))
        stable_owner_fields = ("name", "address", "size", "alignment", "type", "linkage")
        if (entry["section"] != item.get("section")
                or any(entry["owner"].get(key) != owner.get(key)
                       for key in stable_owner_fields
                       if key in entry["owner"] or key in owner)):
            raise SystemExit(f"allocation refused: {name} has conflicting owner or section records")
        entry["locations"].append(item)

    # Some immutable block records keep the original-TU/compiler proof in a
    # parallel emission table rather than duplicating it on every location.
    # Index it by owner name; the table itself is evidence only and does not
    # replace the object/assembly checks below.
    emissions = {row.get("owner_extent", {}).get("name"): row
                 for row in rec.get("storage_emission_evidence", [])
                 if row.get("owner_extent", {}).get("name")}
    for item in candidates:
        storage = item.get("compiler_storage", {})
        if storage.get("label"):
            emissions.setdefault(storage["label"], dict(
                cc1_assembly=selected_report.get("assembly") if selected_report else None,
                source_sha256=storage.get("source_sha256"),
                object_sha256=storage.get("object_sha256"),
                assembly_sha256=storage.get("assembly_sha256"),
                owner_extent=item.get("storage_owner")))
    if selected_report:
        expected_raw = selected_report.get("object_sha256")
        expected_asm = selected_report.get("assembly_sha256")
        expected_src = selected_report.get("source_sha256")
        if expected_raw and sha(raw) != expected_raw:
            raise SystemExit(f"allocation refused: raw object SHA-256 differs from selected TU record for {tu}")
        for label, path, expected in (("assembly", selected_report.get("assembly"), expected_asm),
                                      ("source", selected_report.get("source"), expected_src)):
            if expected and (not path or sha(path) != expected):
                raise SystemExit(f"allocation refused: {label} SHA-256 differs from selected TU record for {tu}")
    else:
        # Bind the actual linker input and its cc1 provenance to the frozen
        # record. Standard one-TU records may pin either a raw `object` field
        # or the raw path/hash on each location; previously allocated object
        # hashes are deliberately not mistaken for the linker input.
        raw_pins = set()
        provenance = list(rec.get("storage_emission_evidence", []))
        provenance.extend(item.get("compiler_storage", {}) for item in candidates)
        provenance.extend(item.get("evidence", {}) for item in candidates)
        for row in provenance:
            if not isinstance(row, dict):
                continue
            obj_path = row.get("raw_input_object") or row.get("object")
            obj_hash = row.get("raw_input_object_sha256") or row.get("object_sha256")
            if obj_path and obj_hash:
                raw_pins.add((str(Path(obj_path).resolve()), obj_hash))
            for path_key, hash_key, label in (("source_path", "source_sha256", "source"),
                                              ("cc1_assembly", "assembly_sha256", "assembly"),
                                              ("assembly_path", "assembly_sha256", "assembly")):
                path, expected = row.get(path_key), row.get(hash_key)
                if path and expected and sha(path) != expected:
                    raise SystemExit(f"allocation refused: {label} SHA-256 differs from owner evidence for {tu}")
        raw_identity = str(raw.resolve())
        matching = [expected for path, expected in raw_pins if path == raw_identity]
        if not matching:
            object_row = rec.get("object", {})
            object_path = object_row.get("path") if isinstance(object_row, dict) else None
            object_hash = object_row.get("sha256") if isinstance(object_row, dict) else None
            if object_path and object_hash and str(Path(object_path).resolve()) == raw_identity:
                matching = [object_hash]
        if not matching or any(expected != sha(raw) for expected in matching):
            raise SystemExit(f"allocation refused: raw input object SHA-256 is not pinned for {tu}")
    source = Elf(raw.read_bytes())
    common = alloc_symbols(source)
    found = {s.name: s for s in common}
    extras = sorted(set(found) - set(wanted))
    bss_sections = {s.index: s for s in source.sections if s.name in (".bss", ".scommon", ".sbss")}
    preallocated = [s for s in source.symbols if s.name and s.shndx in bss_sections]
    preallocated_extras = sorted(s.name for s in preallocated if s.name not in wanted)
    source_defs = {s.name: s for s in preallocated}
    missing = sorted(set(wanted) - set(found) - set(source_defs))
    if missing:
        raise SystemExit(f"allocation refused: expected COMMON symbols absent from {raw}: {missing}")
    if extras:
        raise SystemExit(f"allocation refused: raw COMMON is not fully claimed; preserve raw object: {extras}")
    if preallocated_extras:
        raise SystemExit(f"allocation refused: raw .bss/.scommon also has unclaimed storage; preserve raw object: {preallocated_extras}")
    for name, entry in wanted.items():
        sec, owner, locations = entry["section"], entry["owner"], entry["locations"]
        sym = found.get(name) or source_defs.get(name)
        expected_sec = sec
        want_size = int(owner.get("size", 0))
        want_align = int(owner.get("alignment", 0))
        want_addr = int(str(owner.get("address", "0")), 16)
        if not want_addr or not want_size:
            raise SystemExit(f"allocation refused: {name} owner needs an exact address and extent")
        for item in locations:
            location_addr = int(str(item.get("address", "0")), 16)
            if not want_addr <= location_addr < want_addr + want_size:
                raise SystemExit(f"allocation refused: {name} does not enclose claimed location {location_addr:#x}")
        sym_sec = (".sbss" if sym.shndx == SHN_SCOMMON else ".bss" if sym.shndx == SHN_COMMON
                   else bss_sections[sym.shndx].name)
        is_common = sym.shndx in (SHN_COMMON, SHN_SCOMMON)
        if is_common:
            common_align = int(sym.value)
        else:
            emission = emissions.get(name, {})
            asm = (owner.get("assembly_path") or item.get("evidence", {}).get("assembly_path")
                   or emission.get("cc1_assembly"))
            if not asm:
                raise SystemExit(f"allocation refused: {name} local storage has no cc1 assembly evidence")
            emitted_size, common_align = bss_assembly_extent(asm, name, root)
            if emitted_size != want_size or common_align != want_align:
                raise SystemExit(f"allocation refused: {name} cc1 directives prove size/alignment "
                                 f"{emitted_size:#x}/{common_align:#x}, record claims {want_size:#x}/{want_align:#x}")
            if sym.bind != 0:
                raise SystemExit(f"allocation refused: {name} static cc1 storage is not LOCAL")
        if (sec == ".bss" and sym_sec != ".bss") or (sec == ".sbss" and sym_sec not in (".sbss", ".scommon")):
            raise SystemExit(f"allocation refused: {name} is {sym_sec} but record says {sec}")
        if not want_size or (is_common and sym.size != want_size) or (not is_common and sym.size != 0):
            raise SystemExit(f"allocation refused: {name} size {sym.size:#x} != proven {want_size:#x}")
        if not want_align or common_align != want_align:
            raise SystemExit(f"allocation refused: {name} storage alignment {common_align:#x} != proven {want_align:#x}")
        if not is_common and sym.value % want_align:
            raise SystemExit(f"allocation refused: {name} section offset violates its proven alignment")
        if int(str(owner.get("address", "0")), 16) != want_addr:
            raise SystemExit(f"allocation refused: {name} owner address is not pinned")

    tc = Toolchain(root)
    ld = tc.dir("ps2dev-binutils") / "mips64r5900el-ps2-elf-ld"
    out.parent.mkdir(parents=True, exist_ok=True)
    proc = subprocess.run(["timeout", "-k", "5", "30", str(ld), "-r", "-d", "-EL",
                           "-m", "elf32lr5900", "-o", str(out), str(raw)],
                          cwd=root, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                          text=True, check=False)
    if proc.returncode:
        raise SystemExit(f"ld -r -d failed ({proc.returncode}): {proc.stderr.strip()}")
    allocated = Elf(out.read_bytes())
    allocated_sections = {s.name: s for s in allocated.sections}
    if not all((".scommon" in allocated_sections if s.shndx == SHN_SCOMMON
                else (".bss" in allocated_sections or ".scommon" in allocated_sections))
               for s in common):
        raise SystemExit("allocated output lost an expected NOBITS .bss/.scommon section")
    defined = {s.name: s for s in allocated.symbols if s.name}
    verified = []
    for name, entry in wanted.items():
        sec, owner, locations = entry["section"], entry["owner"], entry["locations"]
        sym = defined.get(name)
        section_name = allocated.sections[sym.shndx].name if sym is not None and sym.shndx < len(allocated.sections) else ""
        output_sec = section_name if section_name in (".bss", ".scommon") else (
            ".scommon" if sec == ".sbss" else ".bss")
        section = allocated_sections.get(output_sec)
        input_symbol = found.get(name) or source_defs.get(name)
        local_bss = input_symbol is not None and input_symbol.shndx not in (SHN_COMMON, SHN_SCOMMON)
        if (sym is None or section is None or section.type != 8 or sym.shndx != section.index
                or (not local_bss and sym.size != int(owner["size"]))
                or (local_bss and sym.size != 0)):
            raise SystemExit(f"allocated output does not define {name} with expected {output_sec} size")
        align = int(owner["alignment"])
        if section.align < align or sym.value % align:
            raise SystemExit(f"allocated output {output_sec} alignment/offset is insufficient for {name}")
        verified.append(dict(name=name,
                             aliases=sorted({alias for item in locations for alias in item.get("names", [])}),
                             alias_locations=[dict(address=item.get("address"), names=item.get("names", []))
                                              for item in locations], section=output_sec,
                             elf_size=sym.size, proven_size=int(owner["size"]), alignment=align,
                             allocated_offset=f"0x{sym.value:X}", bind=sym.bind,
                             type=sym.type,
                             output_section_align=section.align))

    allocation_evidence = dict(path=str(out), sha256=sha(out),
                               sections={name: dict(type=sec.type, size=sec.size, align=sec.align)
                                         for name, sec in allocated_sections.items()
                                         if name in (".bss", ".scommon")},
                               verified_symbols=verified)
    for item in candidates:
        item.setdefault("evidence", {})["link_allocation"] = allocation_evidence

    result = dict(schema="overlay-bss-allocation/1",
                  unit=rec.get("unit", selected_report.get("unit") if selected_report else None),
                  allocation_task=rec.get("allocation_task"), files=rec.get("files", []),
                  recovered_location_candidates=candidates,
                  input=dict(path=str(raw), sha256=sha(raw), common_symbols=[
                      dict(name=s.name, section=".scommon" if s.shndx == SHN_SCOMMON else ".bss",
                           size=s.size, alignment=s.value, bind=s.bind, type=s.type)
                      for s in common]),
                  output=allocation_evidence,
                  command=[str(ld), "-r", "-d", "-EL", "-m", "elf32lr5900"],
                  stderr=proc.stderr,
                  final_link=rec.get("final_link", dict(status="pending")))
    report.parent.mkdir(parents=True, exist_ok=True)
    report.write_text(json.dumps(result, indent=2) + "\n")
    print(json.dumps(result, indent=2))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("allocate", choices=("allocate",))
    ap.add_argument("--root", required=True)
    ap.add_argument("--record", required=True)
    ap.add_argument("--tu", required=True)
    ap.add_argument("--raw", required=True)
    ap.add_argument("--out", required=True)
    ap.add_argument("--report", required=True)
    a = ap.parse_args()
    allocate(a.root, a.record, a.tu, a.raw, a.out, a.report)


if __name__ == "__main__":
    main()
