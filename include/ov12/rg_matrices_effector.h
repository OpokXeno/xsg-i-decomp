#ifndef INCLUDE_OV12_RG_MATRICES_EFFECTOR_H
#define INCLUDE_OV12_RG_MATRICES_EFFECTOR_H

#include "shared.h"

typedef struct Actor Actor;
typedef struct MatrixEffector MatrixEffector;
typedef struct MatrixConstraint MatrixConstraint;
typedef struct MatrixCombine MatrixCombine;

/*
 * The skeleton-manipulator record _InitSkeMani (xrg_actor.c) fills: a small
 * get/set dispatch pair that the effectors call through, plus the actor and
 * joint element it is bound to.
 */
typedef struct SkeMani {
    void (*get)(struct SkeMani *mani, RgMatrix matrix); /* 0x00: joint matrix out */
    void (*set)(struct SkeMani *mani, RgMatrix matrix); /* 0x04: joint matrix in */
    Actor *pXenoAct;     /* 0x08: NULL when this manipulator has no joint data */
    int elementIndex;    /* 0x0C: only set together with pXenoAct */
} SkeMani;

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
 *                 (matricesEffectorUnknownTypeMessage) when it is neither.  The cells' addresses are
 *                 the identity; their contents are never read.  The member
 *                 keeps the originals' own word: OV12's ELF symbol table names
 *                 the two cells s_inSingleIdentifier (0x00a5abc0) and
 *                 s_inDoubleIdentifier (0x00a5abc4).
 *   units         the RgVector of unit records _InitEffector creates
 *                 (CreateRgVector at 0x00a36e90) and _DestructDoubleEffector
 *                 disposes (0x00a37098).  _EffectorAddUnit/_EffectorAddDoubleUnit
 *                 push one heap record per unit and the job walks them with
 *                 RgVectorSize/RgVectorIndex.  Its element type belongs to
 *                 ov12/tu050 (src/ov12/rg_vector.c).
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
 *                      (0x00a37b98) frees them.
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
 */
struct MatrixCombine {
    MatrixEffector effector;
    SkeMani *root;
    SkeMani *top;
    unsigned char quadword_alignment_gap[8];
    RgMatrix root_matrix;
    RgMatrix root_inverse;
    RgMatrix top_matrix;
    RgMatrix top_inverse;
    float weight;
};

/*
 * The constraint allocation contains this evidenced prefix through its ramp
 * weight.  The three manipulator slots are written by _ConstraintSetRoot and
 * _ConstraintSetTop; the point vector and two matrices are read or written by
 * the constraint helpers.  The allocator requests 0xD0 bytes, but the final
 * twelve bytes have no field witness and remain outside this partial type.
 */
struct MatrixConstraint {
    MatrixEffector effector;
    SkeMani *root_manipulator;
    SkeMani *joint_manipulator;
    SkeMani *top_manipulator;
    unsigned char unmodeled_2c[4];
    RgVector point_to;
    RgMatrix root_inverse;
    RgMatrix constrained_frame;
    float weight;
};

/* Functions and data used by other translation units. */
MatrixCombine *CreateRgMatEffCombine(void);
void RgMatEffCombineSetRoot(MatrixCombine *combine, SkeMani *root, SkeMani *top);
void RgMatricesEffectorSetActivity(MatrixEffector *effector, int activity);
void DisposeRgMatricesEffector(MatrixEffector *effector);
void RgMatricesEffectorAddManipulator(MatrixEffector *effector, SkeMani *unit);
void RgMatricesEffectorAddDoubleMani(MatrixEffector *effector, SkeMani *first,
                                     SkeMani *second);
void RgMatricesEffectorJob(MatrixEffector *effector);
MatrixConstraint *CreateRgMatEffConstraint(void);
void RgMatEffConstraintSetRoot(MatrixConstraint *constraint, SkeMani *root,
                               SkeMani *joint);
void RgMatEffConstraintSetTop(MatrixConstraint *constraint, SkeMani *top);
void RgMatEffConstraintSetTarget(MatrixConstraint *constraint, RgVector target);
void RgMatEffConstraintGetTarget(MatrixConstraint *constraint, RgVector target);

#endif /* INCLUDE_OV12_RG_MATRICES_EFFECTOR_H */
