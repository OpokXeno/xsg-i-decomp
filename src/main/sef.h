/*
 * TU-local declarations of main/tu211 (src/main/sef.c).
 */

#ifndef SRC_MAIN_SEF_H
#define SRC_MAIN_SEF_H

typedef struct SchedulerState SchedulerState;

int sefIsDeadSchduler(unsigned int scheduler_index);

/* The original scheduler table is an array of 0xab0-byte records. */
extern unsigned char _scheduler[];

SchedulerState *sefGetNowScheduler(void);

extern SchedulerState *_nowScheduler;

/* sefGetDirMatrix (main:0x002e2818) is still INCLUDE_ASM in this TU; declared
 * here so sefGetVecMatrix (main:0x002e2908) can call it. Caller evidence
 * (a0 unchanged destination matrix, a1 the temporary direction vector). */
void sefGetDirMatrix(Matrix4 *dest, Vector4 *dir);

/* sefRandf (main:0x002e11b0) is still INCLUDE_ASM in this TU; declared here
 * so sefGetCirclePos (main:0x002e2280) can call it. Returns a unit random
 * scalar in f0. */
float sefRandf(void);

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
 * config/units/math-main-w3-002e7f40.json (sefDeg2RadVector @ 0x002e7f40):
 * the degrees-to-radians factor is a scaffold-owned .lit4 word at 0x004d8354,
 * reachable image-wide as the linker symbol `lit4_004d8354`
 * (config/symbols/main.txt). Reading it as a real extern keeps the original
 * gp-relative relocation instead of a hand-written $gp offset
 */
extern const float lit4_004d8354;

/*
 * sefCalcRotChange (main:0x002e2f00): a second, distinct .lit4 occurrence of
 * the same degrees-to-radians factor as lit4_004d8354 above, at its own
 * address 0x004d8318. Unlike lit4_004d8354, no accepted record has renamed
 * this word yet, so it still keeps the scaffold's own splat label
 * (build/main/asm/main/data/sef.lit4.s: `dlabel D_004D8318`), and this extern
 * names it verbatim rather than inventing a new symbol the scaffold does not
 * define
 */
extern const float D_004D8318;

/* sefGetLineAdr (main:0x002e7ea0): the line-effect table, scaffold-owned like
 * _scheduler/_ptAlloc/_battleData above; only its pointer accessor is C. */
extern unsigned char _lineData[];

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
extern short D_0079411C[];

#endif /* SRC_MAIN_SEF_H */
