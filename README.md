# Xenosaga Episode I

> This project is developed with AI assistance.

A matching decompilation of **Xenosaga Episode I: Der Wille zur Macht**
for the PlayStation 2.

The goal is readable C source that reproduces the original machine code.

## Progress

| Version | Target | Bytes | Progress |
| --- | --- | ---: | ---: |
| NTSC-U | `SLUS_204.69` | 0 / 1,279,344 | 0.000% |
| NTSC-U | `OV01.OVL` | 0 / 238,124 | 0.000% |
| NTSC-U | `OV02.OVL` | 0 / 62,868 | 0.000% |
| NTSC-U | `OV10.OVL` | 0 / 269,564 | 0.000% |
| NTSC-U | `OV11.OVL` | 192 / 37,180 | 0.516% |
| NTSC-U | `OV12.OVL` | 0 / 321,380 | 0.000% |
| NTSC-U | `SSD.IRX` | 0 / 57,188 | 0.000% |
| NTSC-U | `RSSD.IRX` | 0 / 10,860 | 0.000% |

## Project Structure

```text
src/          Recovered C source
include/      Shared headers and types
config/       Targets, toolchains and matching packets
tools/        Analysis, build and matching utilities
tests/        Verification tests
annotations/  Prior reverse-engineering notes
docs/         Workflow and contribution guidelines
reports/      Analysis and matching evidence
game/         Local game files (not distributed)
build/        Generated artifacts (not distributed)
.work/        Local toolchains and agent workspaces (not distributed)
```

## Acknowledgments

Thanks to the developers and contributors of the projects on which this work relies:

- [splat](https://github.com/ethteck/splat),
  [spimdisasm](https://github.com/Decompollaborate/spimdisasm), and
  [Rabbitizer](https://github.com/Decompollaborate/rabbitizer) for splitting and disassembly.
- [ps2dev/binutils-gdb](https://github.com/ps2dev/binutils-gdb) and
  [SSXModding/ps2-ee-toolchain](https://github.com/SSXModding/ps2-ee-toolchain) for EE toolchains.
- [decomp.me's compiler collection](https://github.com/decompme/compilers) for historical compiler distributions.
- [PCSX2](https://github.com/PCSX2/pcsx2) for its documented EE implementation and behavioral reference.

## Disclaimer

A legally obtained copy of the game is required. No original game binaries or
copyrighted game assets are distributed in this repository. All game rights belong
to their respective owners; this project is not affiliated with or endorsed by them.
