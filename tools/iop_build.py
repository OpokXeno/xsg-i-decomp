#!/usr/bin/env python3
"""Rebuild complete IOP SSD/RSSD modules from the published C sources.

This is the source-driven publication build for the legacy MIPS-I IOP lane. It
compiles each manifest source once under its original ECOFF object's selected
soft/hard-float contract, verifies compiler emission and original symbol
identity, then assembles and links each complete IRX through iop_module.py.
The original IRX remains the byte authority; this tool never edits it.
--reporting retains same-layout code differences after a valid ordinary build;
gate_passed still records exact whole-module equality and never becomes a waiver.
"""

import argparse
from concurrent.futures import ThreadPoolExecutor, as_completed
import hashlib
import json
import os
from pathlib import Path
import shutil
import signal
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
from tools import iop_inventory, iop_module
from tools.iop_backend import ELF, emission_audit, ordinary_c, require
from tools.match_iop import contract


def digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def run(command, cwd, log):
    command = [str(x) for x in command]
    with Path(log).open("x", encoding="utf-8") as stream:
        stream.write(json.dumps(command) + "\n")
        stream.flush()
        process = subprocess.Popen(command, cwd=cwd, stdout=stream,
                                   stderr=subprocess.STDOUT, start_new_session=True,
                                   env=dict(os.environ, LC_ALL="C", LANG="C"))
        try:
            code = process.wait(timeout=30)
        except subprocess.TimeoutExpired:
            os.killpg(process.pid, signal.SIGTERM)
            try:
                process.wait(timeout=2)
            except subprocess.TimeoutExpired:
                os.killpg(process.pid, signal.SIGKILL)
                process.wait()
            raise ValueError(f"30-second IOP process timeout; see {log}") from None
    require(code == 0, f"command exited {code}; see {log}")
    return dict(command=command, log=str(log), returncode=code)


def load_manifest(path):
    def unique(pairs):
        result = {}
        for key, value in pairs:
            require(key not in result, "duplicate manifest key: " + key)
            result[key] = value
        return result
    return json.loads(Path(path).read_text(encoding="utf-8"), object_pairs_hook=unique)


def validate_manifest(root, manifest, profile_path):
    require(manifest.get("schema_version") == 1 and
            manifest.get("kind") == "iop_source_build" and
            manifest.get("revision") == "slus-20469-412d448de315" and
            manifest.get("profile") == "iop-gcc2_8_1", "unsupported IOP build manifest")
    require(digest(profile_path) == manifest["compiler_profile_sha256"],
            "IOP compiler profile changed since manifest creation")
    units = manifest.get("units")
    require(set(units) == {"iop_ssd", "iop_rssd"}, "manifest must name both IOP modules")
    identities = set()
    source_count = function_count = 0
    for unit, groups in units.items():
        require(isinstance(groups, list), "unit source list is invalid")
        for group in groups:
            source_count += 1
            source = root / group["source"]
            require(source.is_file() and source.resolve().is_relative_to(root / "src"),
                    "IOP source must be a published src/ file")
            require(digest(source) == group["source_sha256"],
                    "published IOP source hash differs: " + group["source"])
            require(group["float_mode"] in ("soft", "hard"), "unsupported IOP float mode")
            # A data-only original FDR (no procedures, e.g. ssd_data.c) is a source
            # group too, provided it maps the data its compiler output defines.
            require(group["functions"] or group.get("generated_data"),
                    "source group has neither original functions nor generated data")
            for function in group["functions"]:
                function_count += 1
                identity = (unit, function["offset"], function["name"])
                require(identity not in identities, "duplicate IOP function identity")
                identities.add(identity)
                require(type(function["offset"]) is int and type(function["size"]) is int and
                        function["offset"] >= 0 and function["size"] > 0,
                        "invalid IOP function range")
            sections = [data["section"] for data in group.get("generated_data", [])]
            require(len(sections) == len(set(sections)),
                    "one contiguous generated-data span per section is required")
    require(source_count == sum(len(x) for x in units.values()), "invalid source count")
    counts = {unit: sum(len(g["functions"]) for g in groups) for unit, groups in units.items()}
    expected_counts = manifest.get("scope_counts", {})
    require(all(counts.get(unit) == expected_counts.get(unit) for unit in units) and
            expected_counts.get("total") == function_count == 495 and
            counts == {"iop_ssd": 442, "iop_rssd": 53},
            "IOP build manifest no longer covers the frozen 442+53 scope")
    return source_count, function_count


def compile_group(root, profile_path, unit, group, module, build_root, expected_mode,
                  reuse_work=None, expected_compile=None):
    rel = Path(group["source"])
    # Private build paths follow the original ECOFF FDR identity, even when
    # the published source filename carries its stable index as a prefix.
    tag = Path(group.get("fdr_filename", rel.stem)).stem
    work = (reuse_work / "compile" / unit / f"fdr-{group['fdr_index']:03d}" / tag
            if reuse_work else build_root / "compile" / unit / f"fdr-{group['fdr_index']:03d}" / tag)
    if reuse_work:
        require(work.is_dir(), "reused IOP compile directory is missing: " + str(work))
    else:
        require(not work.exists(), "IOP group build must be fresh: " + str(work))
        (work / "staged").mkdir(parents=True)
    stage = work / "staged"
    source = root / rel
    staged_source = stage / "candidate.c"
    if not reuse_work:
        shutil.copyfile(source, staged_source)
    else:
        require(staged_source.is_file() and digest(staged_source) == digest(source),
                "reused staged source differs from published src/: " + group["source"])
    # The module's shared declarations: include/ssd or include/rssd, one per
    # owner source as in the EE units. Headers are ordinary C too: the
    # preprocessed stream below is checked as a whole.
    module_dirs = {"iop_ssd": ("ssd",), "iop_rssd": ("rssd", "ssd")}[unit]
    module_headers = sorted(header for name in module_dirs for header in (root / "include").glob(f"{name}/*.h"))
    if not reuse_work:
        for header in module_headers:
            target = stage / "include" / header.relative_to(root / "include")
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(header, target)
    authored = staged_source.read_text(encoding="ascii")
    ordinary_c(authored, staged=staged_source,
               headers={header.relative_to(root).as_posix() for header in module_headers})

    functions = [dict(name=f["name"], va=f"0x{f['offset']:08x}", size=f["size"])
                 for f in group["functions"]]
    packet = dict(unit=unit, binaries={unit: dict(path="game/extracted-iso/" + module["path"],
                                                  sha256=module["sha256"])},
                  functions=functions)
    if not functions:
        packet["data_fdr"] = dict(index=group["fdr_index"], filename=group["fdr_filename"])
    profile, lock, lock_path, compiler, tools, assembler_flags, pins = contract(
        root, profile_path, packet)
    original = module["original_elf"]
    original_object = original
    # The contract resolver is authoritative for hard/soft selection. The
    # manifest carries the same FDR-derived value as a separate consistency pin.
    hard_fdrs = {item["index"] for item in profile["original_object_modes"]["units"][unit]["hard_fdrs"]}
    resolved_mode = "hard" if group["fdr_index"] in hard_fdrs else "soft"
    require(group["float_mode"] == resolved_mode ==
            ("hard" if compiler[5:6] == ["-mhard-float"] else "soft"),
            "manifest FDR float mode differs from current original-object compiler contract")

    flags = profile["default_flags"]
    includes = ["-Istaged", "-Istaged/include"]
    commands = []
    preprocess = [*compiler[:-1], "-E", *flags, *includes,
                  "staged/candidate.c", "-o", "candidate.i"]
    if not reuse_work:
        commands.append(run(preprocess, work, work / "preprocess.log"))
    else:
        require((work / "candidate.i").is_file(), "reused preprocessed source is missing")
    preprocessed = (work / "candidate.i").read_text(encoding="ascii")
    ordinary_c(preprocessed)
    compile_command = [*compiler, *flags, "candidate.i", "-o", "candidate.s"]
    if not reuse_work:
        commands.append(run(compile_command, work, work / "compile.log"))
    assembly_path = work / "candidate.s"
    object_path = work / "candidate.o"
    assemble = [tools["assembler"], *assembler_flags, "-o", "candidate.o", "candidate.s"]
    if not reuse_work:
        commands.append(run(assemble, work, work / "assemble.log"))
    else:
        require(assembly_path.is_file() and object_path.is_file(),
                "reused compiler assembly/object is missing")

    generated = [dict(section=d["section"], va=f"0x{d['offset']:08x}", size=d["size"])
                 for d in group.get("generated_data", [])]
    emitted = ELF(object_path.read_bytes())
    audit = emission_audit(emitted, assembly_path.read_text(encoding="ascii"),
                           functions, module["symbols"], generated)
    context_identity = (len(audit["functions"]) == 1 and not generated and
                        not audit["functions"][0]["size_equal"])
    require(all(item["compiler_emitted"] and item["binding_equal"] and
                (item["size_equal"] or context_identity) for item in audit["functions"])
            and all(item["verified"] for item in audit["data_objects"])
            and not audit["extra_functions"] and not audit["extra_sections"],
            "IOP compiler emission or original symbol/data identity failed: " + group["source"])
    object_hash, assembly_hash = digest(object_path), digest(assembly_path)
    if expected_compile is not None:
        require(expected_compile.get("object_sha256") == object_hash and
                expected_compile.get("assembly_sha256") == assembly_hash and
                expected_compile.get("source_sha256") == digest(source),
                "reused compile objects differ from their immutable first-run receipt")
    descriptor = dict(object=str(object_path), assembly=str(assembly_path),
                      functions=[dict(name=f["name"], offset=f["offset"], size=f["size"])
                                 for f in group["functions"]],
                      generated_data=[dict(section=d["section"], offset=d["offset"], size=d["size"])
                                      for d in group.get("generated_data", [])],
                      module_context_identity=context_identity)
    return dict(unit=unit, group=group, work=str(work), source_sha256=digest(source),
                preprocessed_sha256=digest(work / "candidate.i"), assembly_sha256=digest(assembly_path),
                object_sha256=object_hash, float_mode=resolved_mode,
                compiler_contract=dict(profile=profile["selected"], lock_path=str(lock_path),
                                       lock_sha256=digest(lock_path), flags=flags,
                                       assembler_flags=assembler_flags, tools=tools,
                                       pins_verified=len(pins)),
                commands=commands, emission=audit, descriptor=descriptor)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=ROOT)
    parser.add_argument("--manifest", type=Path, default=ROOT / "config/iop-build.json")
    parser.add_argument("--build-dir", type=Path, required=True)
    parser.add_argument("--report", type=Path, required=True)
    parser.add_argument("--reuse-compiled", type=Path,
                        help="reuse freshly compiled sources from a prior failed module-link run")
    parser.add_argument("--reuse-report", type=Path,
                        help="prior build-report.json pinning the compile objects to reuse")
    parser.add_argument("--jobs", type=int, default=4)
    parser.add_argument("--reporting", action="store_true",
                        help="retain valid same-layout code mismatches for native progress; not acceptance")
    parser.add_argument("--unit", choices=("iop_ssd", "iop_rssd", "both"), default="both")
    args = parser.parse_args(argv)
    root, build_root, report_path = args.root.resolve(), args.build_dir.resolve(), args.report.resolve()
    require(build_root.is_relative_to(root / ".work") and not build_root.exists(),
            "build root must be a fresh private directory under .work")
    require(report_path.is_relative_to(root / ".work") and not report_path.exists(),
            "build report must be a fresh private file under .work")
    require(1 <= args.jobs <= 4, "IOP build concurrency is bounded to four compiler processes")
    manifest = load_manifest(args.manifest)
    profile_path = root / manifest["compiler_profile"]
    source_count, function_count = validate_manifest(root, manifest, profile_path)
    inventory = iop_inventory.verify_iop(root)
    units = inventory["units"]
    for unit in units:
        original_path = root / "game/extracted-iso" / units[unit]["path"]
        units[unit]["original_elf"] = ELF(original_path.read_bytes())
    selected_units = ["iop_ssd", "iop_rssd"] if args.unit == "both" else [args.unit]
    prior_compile = None
    reuse_root = args.reuse_compiled.resolve() if args.reuse_compiled else None
    if reuse_root:
        require(args.reuse_report is not None and args.reuse_report.is_file(),
                "reusing compile artifacts requires the immutable prior report")
        prior = load_manifest(args.reuse_report)
        require(prior.get("kind") == "iop_published_source_build" and
                len(prior.get("source_emission", [])) == source_count,
                "prior report does not pin the complete source compile set")
        prior_compile = {item["source"]: item for item in prior["source_emission"]}
        require(len(prior_compile) == source_count, "prior source compile report has duplicate paths")
    build_root.mkdir(parents=True)
    report_path.parent.mkdir(parents=True, exist_ok=True)
    receipt = dict(schema_version=1, kind="iop_published_source_build", ok=False,
                   gate_passed=False, runtime_execution_proven=False,
                   mode='reporting' if args.reporting else 'exact',
                   source_acceptance=False, recovery_credit=False,
                   selected_units=selected_units, functions_expected=function_count,
                   unique_source_groups=source_count, compiler_process_jobs=args.jobs,
                   manifest=str(args.manifest.resolve()), manifest_sha256=digest(args.manifest),
                   compiler_profile_sha256=digest(profile_path), source_emission=[],
                   module_reports={}, whole_irx={})
    try:
        receipts = []
        futures = {}
        with ThreadPoolExecutor(max_workers=args.jobs) as pool:
            for unit in selected_units:
                for group in manifest["units"][unit]:
                    old = prior_compile.get(group["source"]) if prior_compile else None
                    if prior_compile:
                        require(old is not None, "prior compile report omits " + group["source"])
                    futures[pool.submit(compile_group, root, profile_path, unit,
                                        group, units[unit], build_root, group["float_mode"],
                                        reuse_root if prior_compile else None, old)] = (unit, group)
            for future in as_completed(futures):
                receipts.append(future.result())
        receipts.sort(key=lambda x: (x["unit"], x["group"]["fdr_index"], x["group"]["source"]))
        receipt["source_emission"] = [dict(unit=item["unit"], source=item["group"]["source"],
                                            source_sha256=item["source_sha256"],
                                            float_mode=item["float_mode"],
                                            functions=item["emission"]["functions"],
                                            data_objects=item["emission"]["data_objects"],
                                            compiler_contract=item["compiler_contract"],
                                            object_sha256=item["object_sha256"],
                                            assembly_sha256=item["assembly_sha256"])
                                       for item in receipts]
        for unit in selected_units:
            candidates = [item["descriptor"] for item in receipts if item["unit"] == unit]
            module_dir = build_root / "module" / unit
            output = build_root / "output" / f"{unit}.IRX"
            module_report = iop_module.build_module(root, unit, candidates, module_dir, output,
                                                    reporting=args.reporting)
            receipt["module_reports"][unit] = module_report
            require(module_report.get("build_passed" if args.reporting else "gate_passed") is True,
                    f"complete {unit} IRX gate failed: {module_report.get('error') or module_report.get('comparison')}")
            actual = output.read_bytes()
            original = (root / "game/extracted-iso" / units[unit]["path"]).read_bytes()
            if not args.reporting:
                require(actual == original, f"complete {unit} raw IRX differs after module gate")
            receipt["whole_irx"][unit] = dict(path=str(output), sha256=digest(output),
                                                original_sha256=hashlib.sha256(original).hexdigest(),
                                                byte_identical=actual == original,
                                                functions_verified=len(module_report.get("functions", [])))
        receipt["ok"] = True
        receipt["gate_passed"] = all(v['byte_identical'] for v in receipt['whole_irx'].values())
    except Exception as exc:
        receipt["error"] = f"{type(exc).__name__}: {exc}"
        raise
    finally:
        report_path.write_text(json.dumps(receipt, sort_keys=True, indent=2) + "\n",
                               encoding="utf-8")
    print(json.dumps({"ok": receipt["ok"], "gate_passed": receipt["gate_passed"],
                      "functions": function_count, "sources": source_count,
                      "whole_irx": {u: v["sha256"] for u, v in receipt["whole_irx"].items()},
                      "report": str(report_path)}))
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, ValueError, KeyError, json.JSONDecodeError, subprocess.SubprocessError) as exc:
        print(json.dumps({"ok": False, "gate_passed": False, "error": str(exc)}))
        raise SystemExit(1)
