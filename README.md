# Xenosaga Episode I

> This project is developed with AI assistance.

A matching decompilation of **Xenosaga Episode I: Der Wille zur Macht**
for the PlayStation 2.

The goal is readable C source that reproduces the original machine code.
The Sony SDK and public libraries do not need to be recovered.

## Progress

Recovered 434 of 7,645 in-scope game function(s) (5.677%).

| Version | Target | Functions | Progress |
| --- | --- | ---: | ---: |
| NTSC-U | `SLUS_204.69` | 332 / 3,671 | 9.044% |
| NTSC-U | `OV01.OVL` | 32 / 1,081 | 2.960% |
| NTSC-U | `OV02.OVL` | 5 / 110 | 4.545% |
| NTSC-U | `OV10.OVL` | 0 / 361 | 0.000% |
| NTSC-U | `OV11.OVL` | 9 / 139 | 6.475% |
| NTSC-U | `OV12.OVL` | 56 / 1,788 | 3.132% |
| NTSC-U | `SSD.IRX` | 0 / 442 | 0.000% |
| NTSC-U | `RSSD.IRX` | 0 / 53 | 0.000% |

## Building

You need three things the repository does not contain: your own copy of the
game, the original toolchain, and a Python environment for the disassembler.

### 1. The originals

Extract `SLUS_204.69`, `OV01.OVL`, `OV02.OVL`, `OV10.OVL`, `OV11.OVL`,
`OV12.OVL` and the `IOP/` directory from your own disc image and put them in one
folder. `config/originals.json` names the six files with their sizes and
SHA-256; the build refuses anything else, so it tells you immediately if you
have a different release. This project reproduces the NTSC-U release.

### 2. The toolchain

`config/toolchain-identity.json`: for every compiler and
assembler it gives the version, the SHA-256 of each payload, and the provenance
needed to obtain or rebuild it. Two of the tools are patched, and both patches
are recorded there in full:

| Tool | Where it comes from |
| --- | --- |
| `ee-gcc2.96-realconv-lp7` | the published `ee-gcc2.96` archive, with eight documented single-byte patches to `cc1` (six that restore the original decimal-literal rounding, two that restore the R5900 short-loop padding) |
| `ee-gcc2.9-991111` | the published `ee-gcc2.9-991111` archive, unmodified |
| `ee-as-la29-vsqrt` | GNU as 2.9-ee-991111 built from the pinned `ps2-ee-toolchain` commit with the recorded two-hunk diff (an overlap-safe `memmove`, and the `vsqrt` opcode's bits 21-22) |
| `ee-as-2.9-plain` | the same commit, unpatched |
| `ps2dev-binutils` | ps2dev binutils 2.45.1, the linker, objcopy and modern GAS |

Lay them out under one directory as `config/toolchain-identity.json`'s
`tools.<id>.dir` / `.path` say. Then:

```bash
python3 -B tools/tu/toolchain.py --check
```

It prints where it looked and what it found, and refuses with the tool's name,
both hashes and the recipe when something is missing or different.

### 3. The disassembler

```bash
python3 -m venv .venv && .venv/bin/pip install -r requirements.txt
```

### 4. Tell the build where those three are

By environment variable:

```bash
export XENO_GAME_DIR=/path/to/extracted-iso
export XENO_TOOLCHAIN_DIR=/path/to/toolchains
export XENO_SPLAT_PYTHON=$PWD/.venv/bin/python
```

or once, in `config/toolchain-local.json` (untracked):

```json
{
  "game_dir": "/path/to/extracted-iso",
  "toolchain_root": "/path/to/toolchains",
  "splat_python": "/path/to/.venv/bin/python"
}
```

Three of the tools are built from source rather than downloaded, so their bytes
depend on the host compiler. A build of the same source that assembles the same
bytes still reproduces the images: accept it one tool at a time with
`XENO_TOOLCHAIN_ALLOW=ee-as-la29-vsqrt,ee-as-2.9-plain,ps2dev-binutils`. The
build then says which check it relaxed.

### 5. Build

```bash
./configure.py      # generate every unit build directory and build.ninja
ninja               # build and compare all six originals
ninja gate          # + per-unit status and the whole-file SHA-256 gate
```

`ninja gate` writes `build/gate.json`. `"result": "pass"` means every one of the
six files came out byte for byte identical to your original.

## Contributing

Pick an object from `config/objects/<unit>.objects.json` that has no source yet,
write the C, and build: `ninja` tells you whether the file still comes out
identical. A function counts when its translation unit's source defines it in C
and the whole-file gate passes.

## Acknowledgments

Thanks to the developers and contributors of the projects on which this work relies:

- [splat](https://github.com/ethteck/splat),
  [spimdisasm](https://github.com/Decompollaborate/spimdisasm), and
  [Rabbitizer](https://github.com/Decompollaborate/rabbitizer) for splitting and disassembly.
- [m2c](https://github.com/matt-kempster/m2c) for first-pass decompilation of MIPS
  assembly into C.
- [decomp-permuter](https://github.com/simonlindholm/decomp-permuter) for the bounded
  mutation search over a compiled non-exact base.
- [ps2dev/binutils-gdb](https://github.com/ps2dev/binutils-gdb) and
  [SSXModding/ps2-ee-toolchain](https://github.com/SSXModding/ps2-ee-toolchain) for EE toolchains.
- [decomp.me's compiler collection](https://github.com/decompme/compilers) for historical compiler distributions.
- [PCSX2](https://github.com/PCSX2/pcsx2) for its documented EE implementation and behavioral reference.

## Disclaimer

A legally obtained copy of the game is required. No original game binaries or
copyrighted game assets are distributed in this repository. All game rights belong
to their respective owners; this project is not affiliated with or endorsed by them.
