/*
 * TU-local declarations of ov12/tu068 (src/ov12/rg_matrices_effector.c).
 */

#ifndef SRC_OV12_RG_MATRICES_EFFECTOR_H
#define SRC_OV12_RG_MATRICES_EFFECTOR_H

#include "shared.h"

typedef struct MatrixConstraint MatrixConstraint;

typedef struct MatrixEffector MatrixEffector;

typedef struct MatrixCombine MatrixCombine;

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

/*
 * The double effector CreateRgMatEffCombine allocates (0x140 bytes,
 * RgHeapAlloc at 0x00a37cb0) and _InitCombine (0x00a37c20) fills in.  It blends
 * each unit's matrix towards the pose that the `top` frame holds relative to
 * the `root` frame, by `weight`:
 *
 *   root, top          the two manipulators the combine owns.  _InitCombine
 *                      clears both (0x00a37c78/0x00a37c7c), RgMatEffCombineSetRoot
 *                      (0x00a37ce0) frees the previous ones through RgHeapFree
 *                      and installs its two arguments, and _CombineDestructFunc
 *                      (0x00a37b98) frees them.  _ManiGet/_ManiSet take a
 *                      manipulator as `void *` (the shape _ManiGet reads is its
 *                      own TU's), so that is the type used here too.
 *   root_matrix,       the two frames _CombineStartFunc reads out of the
 *   top_matrix         manipulators each pass (0x00a37a14/0x00a37a20).
 *   root_inverse,      their inverses, XrgInvMatrix at 0x00a37a2c/0x00a37a38.
 *   top_inverse
 *   weight             the blend weight _CombineStartFunc ramps by +0.4 while
 *                      `activity` is set and by -0.15 while it is clear, clamped
 *                      to [0.0, 1.0] (0x00a37a40..0x00a37a9c), and _CombineFunc
 *                      interpolates with.
 *
 * +0x28..+0x2f is the alignment the quadword-aligned matrix block at +0x30
 * forces after the two manipulator pointers: RgHeapAlloc returns 16-byte
 * aligned storage, every matrix here sits on a 16-byte boundary, and no
 * instruction in the six images reads or writes those eight bytes - the only
 * writers of a combine are _InitEffector, _InitDoubleEffector, _InitCombine,
 * RgMatEffCombineSetRoot, _EffectorSetActivity and _CombineStartFunc, and none
 * of them touches +0x28.  It is declared as that alignment, not as a field.
 * The type stops at +0x134; the remaining twelve bytes of the 0x140-byte
 * allocation have no witness.
 *
 * MatrixConstraint, the single-effector sibling _InitConstraint (0x00a377e8)
 * builds on the same head, stays opaque: its three manipulator slots at +0x20,
 * +0x24 and +0x28 (set by _ConstraintSetRoot/_ConstraintSetTop) have no
 * evidenced roles yet, so only its point-to vector at +0x30 is reached, through
 * the named offset below.
 */
struct MatrixCombine {
    MatrixEffector effector;
    void *root;
    void *top;
    unsigned char quadword_alignment_gap[8];
    RgMatrix root_matrix;
    RgMatrix root_inverse;
    RgMatrix top_matrix;
    RgMatrix top_inverse;
    float weight;
};

extern void _ManiGet(void *manipulator, RgMatrix matrix);

extern void XrgMulMatrix(RgMatrix destination, RgMatrix left, RgMatrix right);

extern void XrgLinearIntpVector(RgVector destination, RgVector first,
                                RgVector second, float weight);

extern void XrgCalcMatrixXtoY(RgMatrix destination, RgVector x_axis,
                              RgVector y_axis);

#endif /* SRC_OV12_RG_MATRICES_EFFECTOR_H */
