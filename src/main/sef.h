/*
 * TU-local declarations of main/tu211 (src/main/sef.c).
 */

#ifndef SRC_MAIN_SEF_H
#define SRC_MAIN_SEF_H
#include "main/sef.h"

#define offsetof(type, member) ((__SIZE_TYPE__) &(((type *)0)->member))

/* A named .lit4 literal (config/symbols/main.txt), read by sefGetCirclePos;
 * referencing it by symbol keeps the compiler's own literal pool from
 * allocating a new, differently placed constant for the same original
 * GP-relative slot. */
extern const float circle_angle_scale_literal; /* 0x004d82e8, pi/180 */

/*
 * config/units/main-sef-lerp-vector-sc.json (sefLerpVectorSC @ 0x002e1908):
 * the effect scale factor is a scaffold-owned .lit4 word at 0x004d82dc,
 * reachable image-wide as the linker symbol `effect_scale_literal_0_1`
 * (config/symbols/main.txt). Reading it as a real extern keeps the original
 * gp-relative relocation instead of a hand-written $gp offset
 */
extern const float effect_scale_literal_0_1; /* 0x004d82dc, 0.1f */

/*
 * sefGetDmgNull (main:0x002e7e40): 0x0079411C is _battleData + SEF_BATTLE_PHASE
 * (0x00794110 + 0x00c), the same battle-phase field sefCalcLocalMatrix reads
 * as *(short *)(battle + SEF_BATTLE_PHASE) (this file, main:0x002e2d48).
 * Compiled in isolation (no other _battleData field is touched in
 * sefGetDmgNull), the original computes this address directly against its
 * own scaffold-owned .bss symbol instead of folding +0x00c onto
 * _battleData's own relocation, so this declaration keeps the scaffold's own
 * splat name rather than reusing _battleData (docs/naming.md,
 * "Scaffold-owned data keeps its splat name"); measured with
 * tools/asm_diff.py, `_battleData + SEF_BATTLE_PHASE` here relocates against
 * _battleData with the offset folded into the load's own immediate
 * (`lh v1,12(v0)`), while the original relocates against this narrower
 * symbol with a zero immediate (`lh v1,0(v0)`) -- byte-identical once linked,
 * but relocations_equal only holds for the latter.
 */
extern short sefDmgNullCount[];

/* sefGetDirMatrix (main:0x002e2818) is defined later in this TU; declared
 * here so sefGetVecMatrix (main:0x002e2908) can call it. Caller evidence
 * (a0 unchanged destination matrix, a1 the temporary direction vector). */
static void sefGetDirMatrix(Matrix4 *dest, Vector4 *dir);

/* sefRandf (main:0x002e11b0) is defined later in this TU; declared here
 * so sefGetCirclePos (main:0x002e2280) can call it. Returns a unit random
 * scalar in f0. */
static float sefRandf(void);

/* Original TU-local current-scheduler pointer (main:0x004DC670). */
static SchedulerState *_nowScheduler;

/* sefGetLineAdr (main:0x002e7ea0): the line-effect table, scaffold-owned like
 * _scheduler/_ptAlloc/_battleData above; only its pointer accessor is C. */
/* 128 records of the evidenced 0x820-byte line-data stride. */
static unsigned char _lineData[0x41000];

#endif /* SRC_MAIN_SEF_H */
