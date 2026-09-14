#!/usr/bin/env python3
"""Resolve the build's tools from the tracked identity and the local settings.

`config/toolchain-identity.json` says *which* tools the build needs: version
string, SHA-256 and the provenance needed to obtain or rebuild each one.  It
holds no local path, because where the tools live differs per machine.  Where
they live is said once, locally:

    XENO_TOOLCHAIN_DIR=/path/to/toolchains       # environment, highest priority
    config/toolchain-local.json  {"root": "..."} # untracked local settings
    .work/toolchains                             # fallback, this project's own

The originals and the splat interpreter follow the same rule:

    XENO_GAME_DIR=/path/to/extracted-iso         # default: <repo>/game/extracted-iso
    XENO_SPLAT_PYTHON=/path/to/venv/bin/python   # default: .work/envs/split-tools,
                                                 # then the interpreter running us

Every payload is hashed before the build uses it, and a tool that is missing or
whose bytes differ stops the build with the tool's name, both hashes and the
line of `config/toolchain-identity.json` that says how to obtain it.  Three
tools are built from source rather than downloaded (the two `ee-as` builds and
ps2dev binutils) and their bytes depend on the host compiler; a build of the
same source that assembles the same bytes still reproduces the images, so those
hash checks can be relaxed one id at a time:

    XENO_TOOLCHAIN_ALLOW=ee-as-la29-vsqrt,ps2dev-binutils

A relaxed check is reported, never silent.

    python3 -B tools/tu/toolchain.py --check      # resolve and verify everything
    python3 -B tools/tu/toolchain.py --json       # the resolved paths, for a script
"""
import argparse
import hashlib
import json
import os
import sys
from pathlib import Path

sys.dont_write_bytecode = True

ROOT = Path(__file__).resolve().parents[2]
IDENTITY = "config/toolchain-identity.json"
LOCAL = "config/toolchain-local.json"
DEFAULT_TOOLCHAIN_ROOT = ".work/toolchains"
DEFAULT_GAME_DIR = "game/extracted-iso"
DEFAULT_SPLAT_PYTHON = ".work/envs/split-tools/bin/python"


class ToolchainError(SystemExit):
    """A missing or wrong tool: the build stops and says which one."""

    def __init__(self, message):
        super().__init__("toolchain: " + message)


def sha256(path):
    h = hashlib.sha256()
    with open(path, "rb") as fh:
        for block in iter(lambda: fh.read(1 << 20), b""):
            h.update(block)
    return h.hexdigest()


def _how_to_get(tool_id, entry):
    prov = entry.get("provenance", {})
    lines = [f"  {tool_id}: {entry.get('version', '?')} - {entry.get('role', '')}"]
    base = prov.get("base_archive", {})
    if base.get("url"):
        lines.append(f"  download {base['url']}")
        lines.append(f"  sha256   {base.get('sha256', '?')}")
    up = prov.get("upstream", {})
    if up.get("repository"):
        lines.append(f"  build    {up['repository']} @ {up.get('commit', up.get('branch', '?'))}")
    for step in prov.get("rebuild", []):
        lines.append(f"    - {step}")
    lines.append(f"  The full recipe is in {IDENTITY}, tools.{tool_id}.")
    return "\n".join(lines)


class Toolchain:
    """The tools of one build, resolved and verified."""

    def __init__(self, root=ROOT, identity=None, toolchain_root=None, allow=None):
        self.root = Path(root).resolve()
        self.identity = identity or json.loads((self.root / IDENTITY).read_bytes())
        self.allow = set(allow if allow is not None else
                         filter(None, os.environ.get("XENO_TOOLCHAIN_ALLOW", "").split(",")))
        self.toolchain_root = Path(toolchain_root) if toolchain_root else self._toolchain_root()
        self.relaxed = []
        self._checked = {}

    # ------------------------------------------------------------ locations
    def _toolchain_root(self):
        env = os.environ.get("XENO_TOOLCHAIN_DIR")
        if env:
            return Path(env).expanduser()
        local = self.root / LOCAL
        if local.is_file():
            value = json.loads(local.read_bytes()).get("toolchain_root")
            if value:
                return Path(value).expanduser()
        return self.root / DEFAULT_TOOLCHAIN_ROOT

    def game_dir(self):
        env = os.environ.get("XENO_GAME_DIR")
        if env:
            return Path(env).expanduser()
        local = self.root / LOCAL
        if local.is_file():
            value = json.loads(local.read_bytes()).get("game_dir")
            if value:
                return Path(value).expanduser()
        return self.root / DEFAULT_GAME_DIR

    def splat_python(self):
        env = os.environ.get("XENO_SPLAT_PYTHON")
        if env:
            return Path(env).expanduser()
        local = self.root / LOCAL
        if local.is_file():
            value = json.loads(local.read_bytes()).get("splat_python")
            if value:
                return Path(value).expanduser()
        pinned = self.root / DEFAULT_SPLAT_PYTHON
        return pinned if pinned.is_file() else Path(sys.executable)

    # ------------------------------------------------------------ the tools
    def entry(self, tool_id):
        try:
            return self.identity["tools"][tool_id]
        except KeyError:
            raise ToolchainError(f"{IDENTITY} has no tool {tool_id!r}")

    def dir(self, tool_id):
        """The directory of a multi-file tool, with every file verified."""
        entry = self.entry(tool_id)
        if "dir" not in entry:
            raise ToolchainError(f"{tool_id} is a single file, not a directory")
        base = self.toolchain_root / entry["dir"]
        for name, digest in entry["files"].items():
            self._verify(tool_id, base / name, digest)
        return base

    def file(self, tool_id, name=None):
        """One tool payload, verified."""
        entry = self.entry(tool_id)
        if "path" in entry:
            path = self.toolchain_root / entry["path"]
            self._verify(tool_id, path, entry["sha256"])
            return path
        base = self.toolchain_root / entry["dir"]
        if name is None:
            raise ToolchainError(f"{tool_id} holds several files; name one of "
                                 f"{sorted(entry['files'])}")
        self._verify(tool_id, base / name, entry["files"][name])
        return base / name

    def _verify(self, tool_id, path, digest):
        key = str(path)
        if self._checked.get(key) == digest:
            return
        if not path.is_file():
            raise ToolchainError(
                f"{path} is missing.\n"
                f"  Looked under {self.toolchain_root} (set XENO_TOOLCHAIN_DIR or "
                f"{LOCAL} to point elsewhere).\n" + _how_to_get(tool_id, self.entry(tool_id)))
        found = sha256(path)
        if found != digest:
            if tool_id in self.allow:
                note = f"{tool_id}: {path.name} sha256 {found[:16]}... != {digest[:16]}... " \
                       f"(accepted: XENO_TOOLCHAIN_ALLOW names {tool_id})"
                if note not in self.relaxed:
                    self.relaxed.append(note)
                self._checked[key] = digest
                return
            raise ToolchainError(
                f"{path} is not the pinned payload.\n"
                f"  expected sha256 {digest}\n"
                f"  found    sha256 {found}\n" + _how_to_get(tool_id, self.entry(tool_id)) +
                (f"\n  If this is your own build of the same source, accept it with "
                 f"XENO_TOOLCHAIN_ALLOW={tool_id}."
                 if self.entry(tool_id).get("provenance", {}).get("kind", "").startswith("built_from_source")
                 else ""))
        self._checked[key] = digest

    # ------------------------------------------------------------ contracts
    def contract(self, contract_id):
        try:
            return self.identity["contracts"][contract_id]
        except KeyError:
            raise ToolchainError(f"{IDENTITY} has no contract {contract_id!r}")

    def resolved(self):
        """Every path the build needs, verified, as plain strings."""
        out = {
            "toolchain_root": str(self.toolchain_root),
            "game_dir": str(self.game_dir()),
            "splat_python": str(self.splat_python()),
            "tools": {},
        }
        for tool_id, entry in self.identity["tools"].items():
            if "path" in entry:
                out["tools"][tool_id] = str(self.file(tool_id))
            else:
                out["tools"][tool_id] = str(self.dir(tool_id))
        out["relaxed"] = list(self.relaxed)
        return out


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--root", type=Path, default=ROOT)
    ap.add_argument("--check", action="store_true", help="resolve and verify every tool")
    ap.add_argument("--json", action="store_true", help="print the resolved paths")
    a = ap.parse_args(argv)
    tc = Toolchain(a.root)
    resolved = tc.resolved()
    game = Path(resolved["game_dir"])
    if not game.is_dir():
        raise ToolchainError(f"{game} is not a directory; set XENO_GAME_DIR to the "
                             "folder holding SLUS_204.69 and the five .OVL files "
                             "(they come from your own copy of the game and are "
                             "never distributed here).")
    if a.json:
        print(json.dumps(resolved, indent=1))
    else:
        print(f"toolchain root : {resolved['toolchain_root']}")
        print(f"game originals : {resolved['game_dir']}")
        print(f"splat python   : {resolved['splat_python']}")
        for tool_id, path in resolved["tools"].items():
            print(f"  {tool_id:28s} {path}")
    for note in resolved["relaxed"]:
        print("relaxed: " + note, file=sys.stderr)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
