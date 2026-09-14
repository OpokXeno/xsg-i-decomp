#!/usr/bin/env python3
"""Configure the per-TU build of the six EE files, from the repository root.

    ./configure.py            # regenerate every unit build directory and build.ninja
    ./configure.py --unit ov02
    ninja                     # build and compare all six originals
    ninja gate                # mapcheck + accepted-asm provenance + whole-file elf_gate

What the repository tracks, and what this script generates from it.

Tracked inputs (`tools/tu/tracked_inputs.py` describes the whole set):

    config/originals.json                name, size and SHA-256 of the six originals
    config/split/<unit>.yaml             splat configuration of the unit
    config/split/main.data-splits.json   per-symbol data splits of main
    config/symbols/<unit>.txt            symbol addresses splat disassembles with
    config/symbols/<unit>.main-symbols.ld, config/symbols/main.externals.ld
    config/objects/<unit>.objects.json   one entry per original object
    config/objects/*.compile.json        the compile contract of each object
    config/objects/<ovl>.layout.json     the overlay's section families
    config/overlay-plan.json             which overlay objects build from C
    config/overlay-adaptations.json      declared differences in overlay TU sources
    config/toolchain-identity.json       the exact compilers and assemblers

Generated, per unit, under `build/<unit>/` - none of it is tracked:

    orig/<file>                          a copy of your original, hash-checked
    asm/<unit>/{nonmatchings,data}/...   splat disassembly (scaffold)
    assets/<unit>/...                    retail blobs of the ROM image
    scaffold/src/<unit>/<tu>.c           all-INCLUDE_ASM skeletons
    include/                             the published scaffold headers
    src -> ../../src                     the published sources
    *.ld, build.ninja, obj/, build/      link scripts and outputs

The recovered sources stay in the repository at the paths
`config/objects/<unit>.objects.json` records (`src/<unit>/<tu>.c`, with a TU's
tracked accepted assembly under `src/<unit>/<tu>/`), and a TU without one builds
from its generated skeleton.  Scaffolding is generated from the originals,
earns no credit and is never edited to make source match (AGENTS.md).

The originals and the tools are yours and are never distributed here; where
they live is a local setting - see `tools/tu/toolchain.py` and README.md.

Every compile/assemble/link/compare step runs under `timeout -k 5 30` with
process-group termination; the ninja parallelism comes from
`config/scaling-policy.json` (`limits.ninja_jobs`) when that file is present,
never from an unbounded -j.
"""
import argparse
import hashlib
import json
import os
import shutil
import subprocess
import pathlib
import sys
from pathlib import Path

sys.dont_write_bytecode = True

ROOT = Path(__file__).resolve().parents[1]
TOOLS = ROOT / "tools/tu"
sys.path.insert(0, str(TOOLS))
import toolchain as tcmod        # noqa: E402
import tracked_inputs            # noqa: E402

UNITS = ("main", "ov01", "ov02", "ov10", "ov11", "ov12")
OVERLAYS = ("ov01", "ov02", "ov10", "ov11", "ov12")
SCAFFOLD_HEADERS = ("include_asm.h", "common.h", "labels.inc", "macro.inc")
DEFAULT_JOBS = 4


def jobs(root):
    """Ninja parallelism from the scaling policy; never an unbounded -j."""
    policy = root / "config/scaling-policy.json"
    if policy.is_file():
        limits = json.loads(policy.read_bytes()).get("limits", {})
        value = limits.get("ninja_jobs")
        if isinstance(value, int) and value > 0:
            return value
    return DEFAULT_JOBS


def run(args, cwd, log, timeout, env=None):
    """One bounded subprocess with process-group termination (AGENTS.md)."""
    log.parent.mkdir(parents=True, exist_ok=True)
    with log.open("ab") as fh:
        proc = subprocess.Popen([str(x) for x in args], cwd=str(cwd), stdout=fh,
                                stderr=subprocess.STDOUT, start_new_session=True,
                                env=env)
        try:
            return proc.wait(timeout=timeout)
        except subprocess.TimeoutExpired:
            os.killpg(proc.pid, 9)
            proc.wait()
            return 124


def check(rc, what, log):
    if rc != 0:
        raise SystemExit(f"configure: {what} failed (rc={rc}); see {log}")


def stage_original(root, unit, unit_dir, tc):
    """Copy your original into the unit directory, refusing a different file."""
    originals = json.loads((root / "config/originals.json").read_bytes())["units"]
    want = originals[unit]
    src = tc.game_dir() / want["file"]
    if not src.is_file():
        raise SystemExit(
            f"configure: {src} is missing.\n"
            f"  {want['file']} comes from your own copy of the game; it is never "
            "distributed here.\n"
            "  Point XENO_GAME_DIR (or config/toolchain-local.json \"game_dir\") at the "
            "folder holding\n"
            "  SLUS_204.69 and the five .OVL files.")
    data = src.read_bytes()
    found = hashlib.sha256(data).hexdigest()
    if found != want["sha256"]:
        raise SystemExit(
            f"configure: {src} is not the release this source reproduces.\n"
            f"  expected sha256 {want['sha256']} ({want['size']} bytes)\n"
            f"  found    sha256 {found} ({len(data)} bytes)\n"
            "  config/originals.json names the NTSC-U release; another release has "
            "other bytes\n  and this source does not reproduce it.")
    (unit_dir / "orig").mkdir(parents=True, exist_ok=True)
    (unit_dir / "orig" / want["file"]).write_bytes(data)


def stage_headers(root, unit_dir):
    """The published scaffold headers, copied where splat and the assembler find them.

    This runs AFTER the split and after the INCLUDE_ASM dialect pass, because
    both write their own copies into the unit's include directory: splat
    regenerates `include_asm.h`/`macro.inc` on every run (its version knows
    nothing of ACCEPTED_ASM) and the dialect pass writes `labels.inc`. The
    published headers are the contract, so they are staged last and win.
    """
    (unit_dir / "include").mkdir(parents=True, exist_ok=True)
    for name in SCAFFOLD_HEADERS:
        src = root / "include" / name
        if src.is_file():
            shutil.copyfile(src, unit_dir / "include" / name)


def stage_shared_headers(root, unit_dir):
    """`include/shared.h` and the per-TU public headers `include/<unit>/<tu>.h`.

    Only main needs them here: its compile line is `-Iinclude -I.` relative to
    the unit directory, so the repository's `include/` is not on its search path
    (the overlays get `-I $ROOT/include` from tools/tu/cc_tu.sh and read the
    published headers directly). Refreshed on every configure, so a header
    edited in the repository reaches the next build.
    """
    dst_root = unit_dir / "include"
    dst_root.mkdir(parents=True, exist_ok=True)
    shared = root / "include/shared.h"
    if shared.is_file():
        shutil.copyfile(shared, dst_root / "shared.h")
    for entry in sorted((root / "include").iterdir()):
        if not entry.is_dir():
            continue
        dst = dst_root / entry.name
        shutil.rmtree(dst, ignore_errors=True)
        shutil.copytree(entry, dst)


def link_sources(root, unit_dir):
    """`src` inside the unit directory is the published source tree, so the
    INCLUDE_ASM/ACCEPTED_ASM folder strings of a TU file resolve unchanged."""
    link = unit_dir / "src"
    if link.is_symlink():
        link.unlink()
    elif link.exists():
        shutil.rmtree(link)
    link.symlink_to(os.path.relpath(root / "src", unit_dir))


def splat(root, unit_dir, log, tc):
    return run([tc.splat_python(), "-B", TOOLS / "run_splat.py", "splat.yaml"],
               unit_dir, log, 600)


def configure_main(root, build, logs, tc, verbose):
    """main: tracked inputs -> splat -> legacy dialect and per-symbol data splits
    -> build.ninja."""
    unit_dir = build / "main"
    log = logs / "main.log"
    unit_dir.mkdir(parents=True, exist_ok=True)
    # The published sources must be reachable from inside the unit directory
    # before anything reads them: the split, the per-symbol data splits and the
    # ACCEPTED_ASM provenance gate all resolve a TU's source and its tracked .s
    # through this link.
    link_sources(root, unit_dir)
    tracked_inputs.stage(root, "main", unit_dir, tc)
    stage_original(root, "main", unit_dir, tc)
    for name in ("asm", "assets", "scaffold"):
        shutil.rmtree(unit_dir / name, ignore_errors=True)
    check(splat(root, unit_dir, log, tc), "splat (main)", log)
    check(run([sys.executable, "-B", TOOLS / "post_split.py", "--root", root,
               "--unit-dir", unit_dir], root, log, 300), "post_split.py", log)
    stage_headers(root, unit_dir)
    check(run([sys.executable, "-B", TOOLS / "ninja_main.py", "--root", root,
               "--unit-dir", unit_dir], root, log, 120, env=tool_env(tc)),
          "ninja_main.py", log)
    stage_headers(root, unit_dir)          # ninja_main writes its own copies too
    stage_shared_headers(root, unit_dir)   # include/shared.h, include/<unit>/<tu>.h
    if verbose:
        print("main: unit build directory ready")


def configure_overlay(root, unit, build, logs, tc, verbose):
    unit_dir = build / unit
    log = logs / f"{unit}.log"
    shutil.rmtree(unit_dir, ignore_errors=True)
    unit_dir.mkdir(parents=True, exist_ok=True)
    tracked_inputs.stage(root, unit, unit_dir, tc)
    stage_original(root, unit, unit_dir, tc)
    check(splat(root, unit_dir, log, tc), f"splat ({unit})", log)
    link_sources(root, unit_dir)
    # The per-TU source pass: it rewrites this unit's INCLUDE_ASM bodies to the
    # TU-local names a C TU uses (`glabel X, local` for an original LOCAL
    # function), which is what the published sources were generated against, and
    # writes its regenerated TU file to scaffold/src/ instead of over the
    # published one. It runs before the dialect pass, as in the accepted
    # overlay pipeline.
    check(run([sys.executable, "-B", TOOLS / "gen_ovl_src.py", unit_dir, unit,
               "--root", root, "--scaffold-out",
               "--plan", root / "config/overlay-plan.json",
               "--adapt", root / "config/overlay-adaptations.json"],
              root, log, 300, env=tool_env(tc)), f"gen_ovl_src.py {unit}", log)
    # The published headers are staged only after the dialect pass, in
    # finish_overlays: `legacy_dialect.py` converts splat's own modern
    # `labels.inc` and refuses an input that is already in the legacy dialect.
    if verbose:
        print(f"{unit}: split")


def finish_overlays(root, build, overlays, logs, tc, verbose):
    """The INCLUDE_ASM dialect pass and the per-unit build.ninja of the overlays."""
    log = logs / "overlays.log"
    # Only units that actually have INCLUDE_ASM bodies: a unit whose TUs are all
    # hand-written assembler objects (ov10 today) has no nonmatchings tree, and
    # the dialect pass has nothing to convert there.
    with_include_asm = [u for u in overlays if (build / u / "asm/nonmatchings").is_dir()]
    if with_include_asm:
        check(run([sys.executable, "-B", TOOLS / "apply_dialect.py", build, *with_include_asm],
                  root, log, 600), "apply_dialect.py", log)
    for unit in overlays:
        unit_dir = build / unit
        stage_headers(root, unit_dir)      # after the dialect pass rewrote labels.inc
        check(run([sys.executable, "-B", TOOLS / "ninja_ovl.py", unit_dir, unit,
                   "--root", root, "--manifest", unit_dir / "compile-manifest.json"],
                  root, log, 120, env=tool_env(tc)), f"ninja_ovl.py {unit}", log)
        if verbose:
            print(f"{unit}: unit build directory ready")


def tool_env(tc):
    """The resolved toolchain, passed to the generators that write build.ninja."""
    env = dict(os.environ)
    env["XENO_TOOLCHAIN_DIR"] = str(tc.toolchain_root)
    env["XENO_GAME_DIR"] = str(tc.game_dir())
    env["XENO_SPLAT_PYTHON"] = str(tc.splat_python())
    return env


def root_ninja(root, build, njobs):
    """One build.ninja at the repository root.

    Each unit is built by its own generated build.ninja inside `build/<unit>`,
    where every compile/assemble/link edge carries the 30-second bound.  `ninja`
    with no target builds and compares all six originals; `ninja gate` adds the
    per-unit map check, the ACCEPTED_ASM/INCLUDE_ASM provenance gate and the
    whole-file `tools/elf_gate.py` over the six rebuilt images.
    """
    images = {u: f"build/{u}/build/{u}.bin" for u in UNITS}
    lines = [
        "# Generated by ./configure.py: the per-TU build of the six EE files.",
        "# Every compile/assemble/link/compare step is bounded inside the per-unit",
        "# build.ninja; this file only sequences the units and the gate.",
        f"jobs = {njobs}",
        "",
        # The per-unit edge always runs: its declared output is never created,
        # so ninja re-enters each unit directory on every invocation and lets
        # that unit's own (incremental) build.ninja decide what to do. Without
        # this the root file would never notice an edited TU source, because it
        # does not know the units' inputs.
        "rule unit",
        "  command = ninja -C $dir -j $jobs $targets",
        "  description = UNIT $name",
        "  pool = console",
        "",
        "rule status",
        "  command = timeout -k 5 30 python3 -B tools/tu/unit_status.py"
        " --unit $name --unit-dir $dir --out $out",
        "  description = STATUS $name",
        "",
        "rule elf_gate",
        "  command = timeout -k 5 30 python3 -B tools/elf_gate.py $images --report $out",
        "  description = ELF-GATE",
        "",
    ]
    stamps, statuses = [], []
    for unit in UNITS:
        stamp = f"build/{unit}/.always-built"
        status = f"build/{unit}/status.json"
        stamps.append(stamp)
        statuses.append(status)
        lines += [f"build {stamp}: unit",
                  f"  dir = build/{unit}",
                  f"  name = {unit}",
                  "  targets = ",
                  "",
                  f"build {status}: status {stamp} | tools/tu/unit_status.py",
                  f"  dir = build/{unit}",
                  f"  name = {unit}",
                  ""]
    gate_images = " ".join(f"--image {u}={p}" for u, p in images.items())
    lines += [f"build build/gate.json: elf_gate | {' '.join(statuses)} "
              "tools/elf_gate.py config/originals.json",
              f"  images = {gate_images}",
              "",
              "build all: phony " + " ".join(stamps),
              # `gate` is the whole acceptance-facing gate: every unit's own
              # compare/carve/mapcheck/accepted-asm summary, then the whole-file
              # SHA-256 of all six images against the originals.
              "build gate: phony build/gate.json " + " ".join(statuses),
              "default all",
              ""]
    (root / "build.ninja").write_text("\n".join(lines))



def refuse_symlinked_root(root, explicit, tool):
    """Refuse to act on the SHARED tree when invoked from a private one.

    `ROOT` defaults to the resolved parent of this file.  A private review or
    worker tree usually symlinks `tools/` at the shared repository, and resolving
    through that symlink lands on the SHARED root -- so a tool invoked from the
    private tree without `--root` silently reads and writes the wrong tree.  On
    2026-09-13 that made a careful reviewer regenerate the shared `build/main`
    and, worse, read a clean `repin --check` that was measuring the shared tree
    rather than the candidate it was reviewing.  A check that measures the wrong
    thing and reports success is the failure this project spends the most effort
    eliminating, so this one refuses instead of guessing.
    """
    here = pathlib.Path(__file__).absolute().parents[1]
    if explicit or here == root:
        return
    sys.exit(
        f'{tool}: refusing to run.\n'
        f'  This file was reached through a symlinked tools/ directory, so the default\n'
        f'  root resolved to  {root}\n'
        f'  while you invoked it from  {here}\n'
        f'  Those are different trees. Without --root this would act on the SHARED one:\n'
        f'  writing its build, or reporting a check that measured it instead of yours.\n'
        f'  Pass --root explicitly to say which tree you mean.')

def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--root", type=Path, default=ROOT)
    ap.add_argument("--unit", action="append", choices=UNITS,
                    help="configure only these units (default: all six)")
    ap.add_argument("--quiet", action="store_true")
    a = ap.parse_args(argv)
    root = a.root.resolve()
    refuse_symlinked_root(root, '--root' in sys.argv, 'configure.py')
    tc = tcmod.Toolchain(root)
    tc.resolved()                      # verify every tool before anything runs
    build = root / "build"
    logs = build / "configure-logs"
    build.mkdir(parents=True, exist_ok=True)
    logs.mkdir(parents=True, exist_ok=True)
    wanted = a.unit or list(UNITS)
    njobs = jobs(root)
    for unit in wanted:
        if unit == "main":
            configure_main(root, build, logs, tc, not a.quiet)
        else:
            configure_overlay(root, unit, build, logs, tc, not a.quiet)
    overlays = [u for u in wanted if u != "main"]
    if overlays:
        finish_overlays(root, build, overlays, logs, tc, not a.quiet)
    root_ninja(root, build, njobs)
    for note in tc.relaxed:
        print("configure: relaxed tool check: " + note, file=sys.stderr)
    if not a.quiet:
        print(json.dumps(dict(units=wanted, jobs=njobs,
                              toolchain=str(tc.toolchain_root),
                              build_ninja=str(root / "build.ninja"))))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
