# Source Organization

Code is organized by responsibility, not by the ELF that contains it. Shared
project headers mirror these directories under `include/xeno/`. Binary identity,
overlay membership, original addresses and link order belong in build metadata.

The domains below are supported by original ELF symbols.
Empty directories contain only `.gitkeep`, not stub  implementations or recovered-code
credit. Add deeper directories when recovered code needs them rather than inventing every eventual module now.

## Domains

| Directory | Responsibility | Original Evidence |
| --- |  | --- |
| `core/` | Application lifecycle and game-mode coordination | `main`: `GameModePause`, `GameModeCfEvent` |
| `field/` | Exploration maps, field actors and interactions | `main`: `MAP_getHeight`, `MAP_updateUnitDoor`, `MAP_drawUnit` |
| `battle/` | Main battle system, combat units and commands | `ov01`: `battleInit`, `battleMain`, `MBattleCreate` |
| `menu/` | Inventory, equipment, shops and menu screens | `main`: `MenuShopModelMain`, `MenuShopSelect` |
| `umn/` | U.M.N. interface and services | `main`: `UmnMailBoxSet`; `ov02`: `UmnInit`, `UmnMailHeaderGet` |
| `casino/` | Network Casino minigame | `ov11`: `slot_main`, `NineGameBG_Draw`, `RES_GetBonusTime` |
| `cards/` | Xeno Card minigame | `ov10`: `CardPlayCommandExecute`, `CGPBattleAfterEffect` |
| `driller/` | Driller minigame | `main`: `InitDrill`, `DrillHitCheck`, `DrillClearContainer` |
| `robot_game/` | A.G.W.S Battle minigame | `ov12`: `RobotGameMain` |
| `graphics/` | Shared rendering and graphics submission | `main`: `xglRenderSyncInit`, `xglRenderDrawEnvInit` |
| `animation/` | Shared motion, joints and skeletal animation | `main`: `ACT_updateMotionCore`, `JNT_setFCurve` |
| `camera/` | Shared camera control and view calculations | `main`: `xglCameraControlInit`, `xglCameraTravelInit` |
| `effects/` | Shared visual-effect processing | `main`: `seffectDebugCf`, `sefInitClipViewVolume` |
| `audio/` | EE audio interface, music, effects and driver integration | `main`: `xglSoundSendSed`, `SsdSetSeqMasterVolume` |
| `script/` | Event execution, script threads and native bindings | `main`: `SCRIPT_getCfTime`, `Java_xeno_util_Runtime_evsSetRetPoint__` |
| `resources/` | Shared resource loading, lookup and lifetime | `main`: `RSRC_create`, `RSRC_loadFile`, `RSRC_dispose` |
| `io/` | Disc/archive access, memory-card storage and HDD operations | `main`: `xglCdGetFilePos`, `xglMcEUC2SJIS`, `xglHddCheckCore` |
| `input/` | Game-facing controller state and input processing | `main`: `xglPadInitial`, `xglPadRead`, `xglPadSetRepeat` |
| `movie/` | Movie playback and video integration | `main`: `xglMovieInit`, `xglMoviePlay`; `ov02`: `ipuplay` |
| `math/` | Shared vector, matrix and quaternion operations | `main`: `MATRIX_mul3x4`, `xglQuaternionInterpolateLinear` |
| `platform/` | Low-level PS2 services and hardware/SDK interfaces | `main`: `scePadInit2`, `scePadGetDmaStr` |
| `runtime/` | C runtime and compiler support, separate from gameplay | `main`: `malloc`, `memcpy`, `printf`, `__divdi3` |

Evidence refers to revision `slus-20469-412d448de315`; full binary hashes are in
`config/targets.json` and `config/iop-targets.json`. Evidence names are original
symbols, not proposed declarations.

## Ownership Boundaries

- A subsystem may span several binaries. Do not make each domain an alias for
  an overlay or merge equal symbol names/addresses across binaries.
- Domain-specific cameras, effects and interfaces stay with their domain unless
  analysis establishes a shared implementation.
- Prefixes are search hints, not assignment rules. `Game*`, `RES_*`, `unit*` and
  `DB_*` can have different roles. `xgl*` spans several engine services; it does
  not justify one undifferentiated directory. No full prefix expansion is assumed.
- `robot_game/` is not a catch-all for A.G.W.S. features in the main battle
  system. `cards/` is separate from casino poker.
- Keep implementation-private headers beside their source. Shared project
  interfaces belong in the matching `include/xeno/<domain>/` directory.

## Sound Modules

`audio/ssd/` is reserved for the required SSD sound-engine module;
`audio/rssd/` is reserved for its required remote/RPC and streaming adapter.
The original module names are `PS2_SuzukiSoundDriver` and `PS2_SSDRemote`.
EE-facing wrappers remain distinct from both IOP implementations.

These are separate binaries and ABI contexts within the audio domain. Preserve
`iop_ssd` versus `iop_rssd`, original aliases and module-relative offsets in build
metadata. These directories imply neither IOP matching readiness nor recovered C.