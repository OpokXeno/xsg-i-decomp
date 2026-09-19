#ifndef INCLUDE_OV12_RG_MATRICES_EFFECTOR_H
#define INCLUDE_OV12_RG_MATRICES_EFFECTOR_H

typedef struct MatrixEffector MatrixEffector;

/*
 * The 0x20-byte head every matrices effector carries, written by _InitEffector
 * (0x00a36e70) and refined by _InitSingleEffector (0x00a374a8) and
 * _InitDoubleEffector (0x00a374e8).  Each member is named from the one place
 * that uses it:
 *
 *   identifier    _InitSingleEffector and _InitDoubleEffector store the address
 *                 of one of two distinct four-byte .bss cells here
 *                 (0x00a374c8/0x00a37510); _DestructEffector (0x00a370a0)
 *                 compares this word against both addresses to pick the
 *                 destructor and reaches RgError "unknown effector type"
 *                 (D_00A565F8) when it is neither.  The cells' addresses are
 *                 the identity; their contents are never read.  The member
 *                 keeps the originals' own word: OV12's ELF symbol table names
 *                 the two cells s_inSingleIdentifier (0x00a5abc0) and
 *                 s_inDoubleIdentifier (0x00a5abc4).
 *   units         the RgVector of unit records _InitEffector creates
 *                 (CreateRgVector at 0x00a36e90) and _DestructDoubleEffector
 *                 disposes (0x00a37098).  _EffectorAddUnit/_EffectorAddDoubleUnit
 *                 push one heap record per unit and the job walks them with
 *                 RgVectorSize/RgVectorIndex.  Its element type belongs to
 *                 ov12/tu050 (src/ov12/rg_vector.c) and is not declared here.
 *   start         called with the effector as its only argument at the top of
 *                 a job pass (_DoubleEffectorJob 0x00a37298).
 *   job           called with the effector as its only argument by _EffectorJob
 *                 (0x00a37490); _InitSingleEffector/_InitDoubleEffector install
 *                 _SingleEffectorJob/_DoubleEffectorJob here.
 *   destruct      called with the effector as its only argument by
 *                 _DestructEffector (0x00a37118) after the per-kind teardown.
 *   set_activity  called as (effector, activity) by _EffectorSetActivity
 *                 (0x00a373a4), one instruction after it stores `activity`.
 *   apply         the per-unit callback the job runs between _ManiGet and
 *                 _ManiSet.  Its argument list is the effector kind's, so it is
 *                 declared without one here: a single effector's is
 *                 (effector, RgMatrix) and a double effector's is
 *                 (effector, RgMatrix, RgMatrix) (_DoubleEffectorJob 0x00a37308).
 *   activity      the on/off flag _EffectorSetActivity stores (0x00a3739c) and
 *                 _CombineStartFunc reads to pick the weight's ramp direction.
 */
struct MatrixEffector {
    void *identifier;
    void *units;
    void (*start)(MatrixEffector *effector);
    void (*job)(MatrixEffector *effector);
    void (*destruct)(MatrixEffector *effector);
    void (*set_activity)(MatrixEffector *effector, int activity);
    void (*apply)();
    int activity;
};

#endif /* INCLUDE_OV12_RG_MATRICES_EFFECTOR_H */
