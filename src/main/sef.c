#include "common.h"
#include "shared.h"
#include "sef.h"

typedef struct SefKey3 {
    short frame;
    short value;
    short jump;
} SefKey3;

typedef struct SefProgressState {
    int key_index;
    int frame;
    int value;
} SefProgressState;

float MMathCalcLength(float *vec);

INCLUDE_ASM("asm/main/nonmatchings/sef", srsAtan2);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefRandf);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefIsBossID);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefMemZero);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefCalcRotTransSMatrix);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefCalcInvView);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefSearchMapperIndex2);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefSearchMapperIndex);

INCLUDE_ASM("asm/main/nonmatchings/sef", sevInitPtAllocator);

INCLUDE_ASM("asm/main/nonmatchings/sef", sevAllocPtAllocator);

INCLUDE_ASM("asm/main/nonmatchings/sef", sevFreePtAllocator);

static void sefProgressKey3(SefKey3 *keys, SefProgressState *state)
{
    int frame = state->frame + 1;

    state->frame = frame;
    if (frame < 1024) {
        SefKey3 *key = keys + state->key_index;

        if (key->frame != 1024 && frame >= key[1].frame) {
            if (key[1].jump >= 0) {
                state->key_index = key[1].jump;
                key = &keys[state->key_index];
                state->frame = key->frame;
            } else {
                state->key_index++;
            }
        }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/sef", sefProgressKey5);

/*
 * sefLerpVectorA (main:0x002e17c8), re-treated from the accepted assembly
 * group src/main/sef/sef_lerp_vector_a.s toward readable C with constrained
 * ee-vu-cop2 inline assembly (route ee-vu-cop2, docs/ps2-capabilities.md).
 *
 * Reads the packed signed halfword key at `keys` with the R5900 unaligned
 * doubleword ldl/ldr pair, discards the low halfword with dsrl, widens the
 * remaining three signed halfwords to words with pcgth/pextlh (the one
 * evidenced MMI packed-halfword widening idiom, user-authorized 2026-09-13:
 * config/compiler-patterns.json CP-0169 -- cc1 2.96 emits no MMI for any C
 * spelling, so this reproduces the original's own hand-written idiom, not
 * compiler output), converts the widened lanes to float with vitof0.xyz and
 * forces the resulting vector's W lane to the VU0 architectural constant
 * VF0.W=1.0 with vmove.w before storing the full quadword at `dest`. The
 * original has no .globl, so the emitted binding stays LOCAL.
 */
static void sefLerpVectorA(void *keys, Vector4 *dest)
{
    __asm__ __volatile__(
        "ldl $9, 7(%0)\n\t"
        "ldr $9, 0(%0)\n\t"
        "dsrl $9, $9, 0x10\n\t"
        "vmove.w $vf2w, $vf0w\n\t"
        "pcgth $10, $0, $9\n\t"
        "pextlh $10, $10, $9\n\t"
        "qmtc2 $10, $vf1\n\t"
        "vitof0.xyz $vf2xyz, $vf1xyz\n\t"
        "sqc2 $vf2, 0(%1)\n\t"
        "nop"
        :
        : "r"(keys), "r"(dest)
        : "$9", "$10", "memory"
    );
}

INCLUDE_ASM("asm/main/nonmatchings/sef", sefLerpVector);

/*
 * sefLerpVectorSC (main:0x002e1908), re-treated from the accepted assembly
 * group src/main/sef/sef_lerp_vector_sc.s toward readable C with constrained
 * ee-vu-cop2 inline assembly (route ee-vu-cop2, docs/ps2-capabilities.md).
 *
 * Forwards its own arguments to the still-unrecovered sefLerpVector, then
 * when its status is nonzero scales the XYZ lanes of the second four-slot
 * vector (the caller's own vectors pointer advanced by one slot) in place by
 * the shared 0.1 effect-scale literal (config/units/main-sef-lerp-vector-sc.json,
 * effect_scale_literal_0_1 @ 0x004d82dc), leaving its W lane untouched. The
 * original has no .globl, so the emitted binding stays LOCAL.
 *
 * The scale read is placed unconditionally, before the status test, matching
 * the original's own instruction order (lwc1 before beq). 2.96's scheduler
 * otherwise sinks that load past the branch into the taken arm since nothing
 * else forces it to stay put; an explicit empty compiler barrier right after
 * the read (before the branch) blocks that motion without changing any value
 * (the instruction-free scheduling barrier, user-authorized 2026-09-13:
 * config/compiler-patterns.json CP-0171 -- an empty `__asm__ __volatile__`
 * template with no output and no clobber emits no instruction, so it cannot
 * hide a hardware access; it only fixes the read's position for the
 * scheduler).
 */
static int sefLerpVectorSC(void *keys, Vector4 *vectors)
{
    int status = sefLerpVector(keys, vectors);
    float scale = effect_scale_literal_0_1;

    __asm__ __volatile__("" : : "f"(scale));

    if (status != 0) {
        vectors++;

        __asm__ __volatile__(
            "lqc2 $vf1, 0(%0)\n\t"
            "mfc1 $8, %1\n\t"
            "qmtc2 $8, $vf2\n\t"
            "vmulx.xyz $vf1xyz, $vf1xyz, $vf2x\n\t"
            "sqc2 $vf1, 0(%0)"
            :
            : "r"(vectors), "f"(scale)
            : "$8", "memory"
        );
    }

    return status;
}

INCLUDE_ASM("asm/main/nonmatchings/sef", sefLerpIVectorA);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefLerpIVector);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefLerpIVector2);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefLerpFloat);

void sefLerpInt(SefKey3 *keys, SefProgressState *state)
{
    SefKey3 *key;
    int frame_delta;
    int frame_offset;
    int key_index;
    float fraction;

    key = (SefKey3 *)((char *)keys + state->key_index * 6);
    if (key->frame == 1024) {
        state->value = key->value;
    } else if (((SefKey3 *)((char *)key + 6))->frame == 1024) {
        state->value = key->value;
    } else {
        sefProgressKey3(keys, state);
        key_index = state->key_index;
        key = (SefKey3 *)((char *)keys + key_index * 6);
        if (key->frame != ((SefKey3 *)((char *)key + 6))->frame) {
            frame_delta = ((SefKey3 *)((char *)key + 6))->frame - key->frame;
            frame_offset = state->frame - key->frame;
            fraction = (float)frame_offset / (float)frame_delta;
            state->value = (short)((float)(((SefKey3 *)((char *)key + 6))->value - key->value) * fraction + (float)key->value);
        } else {
            state->value = key->value;
        }
    }
}

static void sefProgressInt(SefKey3 *keys, SefProgressState *state)
{
    sefProgressKey3(keys, state);
    state->value = ((SefKey3 *)((char *)keys + state->key_index * 6))->value;
}

INCLUDE_ASM("asm/main/nonmatchings/sef", sefGetMotionNullMatrix);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefGetNullPosition);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefGetWeaponPosition);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefGetPoint);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefGetPosition);

/*
 * sefGetCirclePos (main:0x002e2280): draws one random unit scalar, scales it
 * into a full-circle angle in radians (randf() * 360deg * (pi/180)), and asks
 * the VU0 macro-mode circle microprogram for that angle's two position
 * components (vcallms 0xe8, then vcallms 0x20 on the same angle), each
 * scaled by the caller's radius. Y is exact zero and W is exact 1.0f; there
 * is only one random draw, reused for both VU calls. Each block moves the
 * angle into the VU0 broadcast register and the result back out through
 * COP1 (mfc1/mtc1) around the qmtc2/vcallms/qmfc2.i transfer, exactly the
 * hardware handoff the original performs; the VU microprogram body itself is
 * outside this EE-side caller.
 */
static void sefGetCirclePos(Vector4 *pos, float radius)
{
    float angle = sefRandf() * 360.0f * circle_angle_scale_literal;
    float x_unit;
    float z_unit;

    __asm__ __volatile__(
        "mfc1 $8, %1\n\t"
        "qmtc2 $8, $vf4\n\t"
        "vcallms 0xe8\n\t"
        "qmfc2.i $8, $vf1\n\t"
        "mtc1 $8, %0\n\t"
        : "=f"(x_unit)
        : "f"(angle)
        : "$8", "memory"
    );
    pos->x = x_unit * radius;

    __asm__ __volatile__(
        "mfc1 $8, %1\n\t"
        "qmtc2 $8, $vf4\n\t"
        "vcallms 0x20\n\t"
        "qmfc2.i $8, $vf1\n\t"
        "mtc1 $8, %0\n\t"
        : "=f"(z_unit)
        : "f"(angle)
        : "$8", "memory"
    );

    pos->y = 0.0f;
    pos->w = 1.0f;
    pos->z = z_unit * radius;
}

INCLUDE_ASM("asm/main/nonmatchings/sef", sefGetSpherePos);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefGetCubePosBtm);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefGetCubePosTop);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefGetCubePos);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefGetOfsRange);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefGetDirMatrix);

/*
 * sefGetVecMatrix (main:0x002e2908): subtracts source vector B from source
 * vector A into a temporary direction vector (VU0 macro-mode vsub.xyz writes
 * only the XYZ lanes, so the temporary keeps source A's own W lane), then
 * builds the destination orientation matrix from that temporary via the
 * still-unrecovered sefGetDirMatrix helper. Only the subtraction is genuine
 * COP2/VU0 hardware; the call is ordinary C.
 */
void sefGetVecMatrix(Matrix4 *dest, Vector4 *source_a, Vector4 *source_b)
{
    Vector4 dir;

    __asm__ __volatile__(
        "lqc2 $vf1, 0(%0)\n\t"
        "lqc2 $vf2, 0(%1)\n\t"
        "vsub.xyz $vf1xyz, $vf1xyz, $vf2xyz\n\t"
        "sqc2 $vf1, 0(%2)\n\t"
        :
        : "r"(source_a), "r"(source_b), "r"(&dir)
        : "memory"
    );
    sefGetDirMatrix(dest, &dir);
}

INCLUDE_ASM("asm/main/nonmatchings/sef", sefGetDirVector);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefGetStartPos);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefGetTargetPos);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefGetSpeed);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefMoveParticle);

/*
 * sefCalcLocalMatrix (main:0x002e2d48), re-treated from the accepted assembly
 * group src/main/sef/sefCalcLocalMatrix.s toward readable C with constrained
 * ee-vu-cop2 inline assembly (route ee-vu-cop2, docs/ps2-capabilities.md).
 * The second function of that group, sefCalcRotChange, converted the same
 * way further down this file; the group now holds no tracked assembly.
 *
 * Names recovered from config/symbols/main.txt: _battleData (0x00794110),
 * _parentLine, _nowParentLocal, _ptAlloc. The battle state block and the
 * scheduler local record are read through narrowly evidenced scalar accesses
 * rather than through structs with invented members: only the fields below
 * are evidenced, the bytes between them are not, and docs/naming.md forbids
 * inventing padding members to close the gaps.
 *   _battleData + 0x00c  short  battle phase compared against 2
 *   _battleData + 0x22c  int    "battle is running" flag
 *   _battleData + 0x200         the matrix source both callees take
 *   record     + 0x000         the local matrix this call produces
 *   record     + 0x080         the base matrix it is multiplied by
 *   record     + 0x0c0         the rotation vector
 *   record     + 0x0f0/0x0f8   the offset the multiply is gated on
 *
 * record is kept as unsigned char* (its full layout is not recovered, only
 * these offsets are evidenced), and sefGetDirMatrix/sefGetVecMatrix are
 * called through Matrix4 and Vector4 pointer casts of record's own address to match
 * their published prototypes (src/main/sef.h): both take the destination
 * matrix at offset 0, so the cast is exactly what an already-Matrix4-typed
 * `record` would give the callee, with no different bytes read or written.
 */
#define SEF_BATTLE_PHASE         0x00c
#define SEF_BATTLE_MATRIX_SOURCE 0x200
#define SEF_BATTLE_RUNNING       0x22c
#define SEF_LOCAL_MATRIX         0x000
#define SEF_LOCAL_BASE_MATRIX    0x080
#define SEF_LOCAL_ROTATION       0x0c0
#define SEF_LOCAL_OFFSET         0x0f0
#define SEF_LOCAL_OFFSET_Z       0x0f8
#define SEF_PARENT_DIR_MATRIX    0x120

extern unsigned char _battleData[];
extern int _parentLine;
extern int _nowParentLocal;
extern unsigned char _ptAlloc[];
/* _battleData + 0x200, still owned by the scaffold, so it keeps the original
 * object's own name for the address (docs/naming.md). The two callees below
 * materialise the same address differently, which is why both spellings are
 * here: the flag path reaches it from the block base it already holds. */
extern unsigned char D_00794310[];

extern void MMathMulMatrix(void *destination, void *left, void *right);

static void sefCalcLocalMatrix(unsigned char *record, int target, int previous_target)
{
    unsigned char *battle = _battleData;
    int in_battle = *(int *)(battle + SEF_BATTLE_RUNNING);

    target &= ~0xe000;
    if (in_battle != 0 && (unsigned int)(target - 257) < 16) {
        target += 1792;
    }
    if (target == 529) {
        target = (*(short *)(battle + SEF_BATTLE_PHASE) >= 2) ? 527 : 528;
    }

    if (target == 4) {
        unsigned char *parent;

        if (_parentLine >= 0 && _nowParentLocal >= 0) {
            parent = &_ptAlloc[_nowParentLocal * 640];
        } else {
            parent = 0;
        }
        if (parent != 0 && target != previous_target) {
            sefGetDirMatrix((Matrix4 *)record, (Vector4 *)(parent + SEF_PARENT_DIR_MATRIX));
        }
    } else if (target == 1) {
        /* The base matrix becomes the local one: the four quadword moves the
         * original performs inline through the EE's own scratch GPRs. */
        __asm__ __volatile__(
            "lq $8, 0(%0)\n\t"
            "lq $9, 16(%0)\n\t"
            "lq $10, 32(%0)\n\t"
            "lq $11, 48(%0)\n\t"
            "sq $8, 0(%1)\n\t"
            "sq $9, 16(%1)\n\t"
            "sq $10, 32(%1)\n\t"
            "sq $11, 48(%1)"
            :
            : "r"(record + SEF_LOCAL_BASE_MATRIX), "r"(record)
            : "$8", "$9", "$10", "$11", "memory");
        return;
    } else if (in_battle != 0 && (target & 0x200) != 0) {
        Vector4 direction;

        __asm__ __volatile__(
            "lqc2 $vf1, 0(%0)\n\t"
            "lqc2 $vf2, 0(%1)\n\t"
            "vadd.xyz $vf1xyz, $vf1xyz, $vf2xyz\n\t"
            "sqc2 $vf1, 0(%2)"
            :
            : "r"(record + SEF_LOCAL_ROTATION), "r"((unsigned char *)_nowScheduler + 0x80),
              "r"(&direction)
            : "memory");
        sefGetVecMatrix((Matrix4 *)record, (Vector4 *)(battle + SEF_BATTLE_MATRIX_SOURCE),
                        &direction);
    } else if (target >= 5) {
        sefGetVecMatrix((Matrix4 *)record, (Vector4 *)D_00794310,
                        (Vector4 *)(record + SEF_LOCAL_ROTATION));
    } else {
        /* The identity matrix, built by rotating $vf0 = (0,0,0,1) one lane at
         * a time and storing the rows back to front. */
        __asm__ __volatile__(
            "vmr32.xyzw $vf1xyzw, $vf0xyzw\n\t"
            "vmr32.xyzw $vf2xyzw, $vf1xyzw\n\t"
            "vmr32.xyzw $vf3xyzw, $vf2xyzw\n\t"
            "sqc2 $vf0, 48(%0)\n\t"
            "sqc2 $vf1, 32(%0)\n\t"
            "sqc2 $vf2, 16(%0)\n\t"
            "sqc2 $vf3, 0(%0)"
            :
            : "r"(record)
            : "memory");
    }

    if (*(long long *)(record + SEF_LOCAL_OFFSET) != 0 ||
            *(float *)(record + SEF_LOCAL_OFFSET_Z) != 0.0f) {
        MMathMulMatrix(record, record, record + SEF_LOCAL_BASE_MATRIX);
    }
}

/*
 * sefCalcRotChange (main:0x002e2f00), re-treated from the accepted assembly
 * group src/main/sef/sefCalcRotChange.s (itself the re-cut remainder of
 * src/main/sef/sefCalcLocalMatrix.s once its sibling sefCalcLocalMatrix
 * converted, config/units/math-correction13-5.json) toward readable C with
 * constrained ee-vu-cop2 inline assembly (route ee-vu-cop2,
 * docs/ps2-capabilities.md). The group now holds no assembly of its own.
 *
 * record keeps the offsets sefCalcLocalMatrix's own comment already
 * evidences, plus what this function itself reads and writes:
 *   record + 0x040   the source matrix MMathRotateMatrixXYZ multiplies by
 *                    when the masked mode selects it
 *   record + 0x080   SEF_LOCAL_BASE_MATRIX, the matrix every path here
 *                    rotates in place and sefCalcLocalMatrix later reads
 *   record + 0x0e0   the accumulated offset before this frame's change
 *   record + 0x0f0/0x0f4/0x0f8
 *                    SEF_LOCAL_OFFSET, the combined offset this call
 *                    produces and sefCalcLocalMatrix gates its multiply on.
 *                    The three components are the x/y/z of one Vector4, so
 *                    the single-axis rotations below read them as fields of
 *                    `(Vector4 *)(record + SEF_LOCAL_OFFSET)` rather than as
 *                    three separate offsets; SEF_LOCAL_OFFSET_Z stays for
 *                    sefCalcLocalMatrix, which reads +0x0f8 on its own
 *   record + 0x1c0   the per-frame rotation delta: degrees on entry,
 *                    converted to radians in place by the first block
 *
 * flags selects the rotation order: masking off bits 0x6000 and comparing to
 * 1 picks the XYZ path (Y/Z/X one axis at a time when bit 0x4000 is also set,
 * or one MMathRotateMatrixXYZ call over the source matrix otherwise); with
 * that not selected, bit 0x6000 clear takes MMathRotateMatrixYXZ, and
 * otherwise bit 0x4000 takes the same Y/Z/X order and bit 0x2000 takes
 * MMathRotateMatrixXYZ again, this time from the identity default and the
 * combined offset. update disables all of it: only the vector scale and
 * offset accumulation happen. MMathRotateMatrixX/Y/Z/XYZ/YXZ are main/tu219
 * (src/main/m_math.c); a null matrix argument here reaches their own
 * null-default idiom (docs/tu-worker.md), not a call site concern.
 *
 * The degrees-to-radians factor is D_004D8318, a second occurrence of the
 * same .lit4 constant sefDeg2RadVector reads as lit4_004d8354 above: EE GCC
 * 2.96 does not merge identical .lit4 words across call sites, so each user
 * gets its own address and its own extern (docs/naming.md, "Scaffold-owned
 * data keeps its splat name").
 *
 * MMathRotateMatrixX/Y/Z/XYZ/YXZ are declared with a non-const Matrix4 *
 * `matrix` parameter, matching MMathMulMatrix's own cross-TU declaration
 * above rather than m_math.c's `const Matrix4 *`: EE GCC 2.96 warns passing
 * a plain Matrix4 pointer (or a literal 0) into a `const Matrix4 *` array
 * parameter, and dropping the const here does not change what is read.
 */
#define SEF_ROTCHANGE_SRC_MATRIX  0x040
#define SEF_ROTCHANGE_OFFSET_BASE 0x0e0
#define SEF_ROTCHANGE_DELTA       0x1c0

extern Matrix4 *MMathRotateMatrixX(Matrix4 *out, Matrix4 *matrix, float angle);
extern Matrix4 *MMathRotateMatrixY(Matrix4 *out, Matrix4 *matrix, float angle);
extern Matrix4 *MMathRotateMatrixZ(Matrix4 *out, Matrix4 *matrix, float angle);
extern Matrix4 *MMathRotateMatrixXYZ(Matrix4 *out, Matrix4 *matrix, Vector4 *angles);
extern Matrix4 *MMathRotateMatrixYXZ(Matrix4 *out, Matrix4 *matrix, Vector4 *angles);

static void sefCalcRotChange(unsigned char *record, int flags, int update)
{
    __asm__ __volatile__("lqc2 $vf2, 0(%0)" : : "r"(record + SEF_ROTCHANGE_DELTA) : "memory");

    {
        register float degToRad asm("$f8") = D_004D8318;

        __asm__ __volatile__(
            "mfc1 $8, %1\n\t"
            "qmtc2.ni $8, $vf1\n\t"
            "vmulx.xyz $vf2, $vf2, $vf1x\n\t"
            "sqc2 $vf2, 0(%0)"
            :
            : "r"(record + SEF_ROTCHANGE_DELTA), "f"(degToRad)
            : "$8", "memory");
    }

    __asm__ __volatile__(
        "lqc2 $vf1, 0(%1)\n\t"
        "lqc2 $vf2, 0(%2)\n\t"
        "vadd.xyz $vf1xyz, $vf1xyz, $vf2xyz\n\t"
        "sqc2 $vf1, 0(%0)"
        :
        : "r"(record + SEF_LOCAL_OFFSET), "r"(record + SEF_ROTCHANGE_OFFSET_BASE),
          "r"(record + SEF_ROTCHANGE_DELTA)
        : "memory");

    if (update == 0) {
        return;
    }

    if ((flags & 0xffff1fff) == 1) {
        if (flags & 0x4000) {
            MMathRotateMatrixY((Matrix4 *)(record + SEF_LOCAL_BASE_MATRIX), (Matrix4 *)0,
                               ((Vector4 *)(record + SEF_LOCAL_OFFSET))->y);
            MMathRotateMatrixZ((Matrix4 *)(record + SEF_LOCAL_BASE_MATRIX),
                               (Matrix4 *)(record + SEF_LOCAL_BASE_MATRIX),
                               ((Vector4 *)(record + SEF_LOCAL_OFFSET))->z);
            MMathRotateMatrixX((Matrix4 *)(record + SEF_LOCAL_BASE_MATRIX),
                               (Matrix4 *)(record + SEF_LOCAL_BASE_MATRIX),
                               ((Vector4 *)(record + SEF_LOCAL_OFFSET))->x);
        } else {
            MMathRotateMatrixXYZ((Matrix4 *)(record + SEF_LOCAL_BASE_MATRIX),
                                 (Matrix4 *)(record + SEF_ROTCHANGE_SRC_MATRIX),
                                 (Vector4 *)(record + SEF_ROTCHANGE_DELTA));
        }
        return;
    }

    if ((flags & 0x6000) == 0) {
        MMathRotateMatrixYXZ((Matrix4 *)(record + SEF_LOCAL_BASE_MATRIX), (Matrix4 *)0,
                             (Vector4 *)(record + SEF_LOCAL_OFFSET));
        return;
    }

    if (flags & 0x4000) {
        MMathRotateMatrixY((Matrix4 *)(record + SEF_LOCAL_BASE_MATRIX), (Matrix4 *)0,
                           ((Vector4 *)(record + SEF_LOCAL_OFFSET))->y);
        MMathRotateMatrixZ((Matrix4 *)(record + SEF_LOCAL_BASE_MATRIX),
                           (Matrix4 *)(record + SEF_LOCAL_BASE_MATRIX),
                           ((Vector4 *)(record + SEF_LOCAL_OFFSET))->z);
        MMathRotateMatrixX((Matrix4 *)(record + SEF_LOCAL_BASE_MATRIX),
                           (Matrix4 *)(record + SEF_LOCAL_BASE_MATRIX),
                           ((Vector4 *)(record + SEF_LOCAL_OFFSET))->x);
        return;
    }

    if (flags & 0x2000) {
        MMathRotateMatrixXYZ((Matrix4 *)(record + SEF_LOCAL_BASE_MATRIX), (Matrix4 *)0,
                             (Vector4 *)(record + SEF_LOCAL_OFFSET));
        return;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/sef", sefExecLineLocalData);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefFreeLocalData);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefAllocLocalData);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefDestroyLocalData);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefInitLineData);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefAllocLineData);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefFreeLineData);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefAnalyzeAnim);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefAnimate);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefCreateParticle);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefExecLineGlobalData);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefExecLineData);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefInitEffectData);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefDestroyEffectData);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefAllocEffectData);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefFreeEffectData);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefCheckFinish);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefExecEffectData);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefInitScheduler);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefAllocScheduler);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefFreeSchedulerCf);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefFreeScheduler);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefDestroyScriptScheduler);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefDestroyScriptScheduler2);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefInitEffectTbl);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefCreateScheduler2);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefCreateScheduler);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefGetSizeOffset);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefCreateReactionEffect);

/* sefGetMatrixScale (main:0x002e5af8): scale helper for matrix records.
 * Callers pass matrix/vector storage in a0 (sefDrawSchedulerEffect passes a
 * stack record; sefGetParentMatrix passes s0) and consume the scalar result
 * in f0. The body tail-calls the resident MMathCalcLength helper and returns
 * its value unchanged; the frame only saves/restores ra per the ABI. */
float sefGetMatrixScale(float *matrix)
{
    return MMathCalcLength(matrix);
}

INCLUDE_ASM("asm/main/nonmatchings/sef", sefGetParentMatrix);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefExecScheduler);

int sefIsDeadSchduler(unsigned int scheduler_index)
{
    if (scheduler_index >= 0x80) {
        return 1;
    }
    return *(int *)(_scheduler + 0x6b0 + scheduler_index * 0xab0) == 0;
}

INCLUDE_ASM("asm/main/nonmatchings/sef", sefSetReverseDir);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefCreateBattleActorTbl);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefInitBattlePrm);

ACCEPTED_ASM("src/main/sef", sefCaclAllTarget);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefSetLightFlag);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefSetHitSignal);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefIsHitActor);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefSetSeSignal);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefIsSeSignal);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefAddLightActor);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefInitEffect);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefInitEffectBattle);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefInitEffectCf);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefSetupPlayer);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefSetupEnemy);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefReleaseID);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefSetupEffect);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefIsEntryBoss);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefDestroyEffect);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefDestroyEffectCf);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefLoadEffect);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefCheckLoad);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefExecEffect);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefProgressEffect);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefDrawEffect);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefDrawEffect3D);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefDrawEffect2D);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefCnvDeathEffectNo);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefCnvWaitEffectNo);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefDeleteEffectWait);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefCnvEtEffectNo);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefCreateEffect);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefCreateEffect2);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefDeleteEffect2);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefLoadEffectCf);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefLoadEffectCfName);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefLoadMemoryEffectCf);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefLoadMemoryEffectCfName);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefCreateEffectCf2);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefCreateEffectCf);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefDeleteEffectCf);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefRewindEffectCf);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefClearEffectCf);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefIsFinishEffect);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefIsFinishEffect2);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefSetLoadQue);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefGetLoadQue);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefExecLoadQue);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefKillEffect);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefHitEffect);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefPushEffect);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefPopEffect);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefGetBattleData);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefGetDmgNull);

INCLUDE_ASM("asm/main/nonmatchings/sef", sevGetPtAllocator);

INCLUDE_ASM("asm/main/nonmatchings/sef", sevGetPtAllocator2);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefGetLineAdr);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefGetScheduler);

/* Spark S1: static sbss scheduler-pointer getter.
 * Changed hypothesis: the original TU holds the current scheduler as a
 * file-static sbss pointer `_nowScheduler` (VA 0x004dc670, matching the ELF local
 * symbol) and `sefGetNowScheduler` returns it. Callers in sdvTransOffset
 * (0x002ed178) and sdvExecSpecial (0x002ed414) keep the result in s4/s3 and
 * dereference +2700, so the return type is a data pointer, not int. The
 * gp-relative `lw v0,-13568(gp)` (delay slot after `jr ra`) is EE GCC 2.96
 * small-data codegen with _gp == 0x004dfb70; hence flags -O2 -G8 with an
 * explicit _gp witness. Reviewed against the pinned m2c scaffold
 * (first_pass.c): m2c models the body as a gp-0x3500 load, untrusted only.
 * No ASM, no attributes.
 */
SchedulerState *sefGetNowScheduler(void)
{
    return _nowScheduler;
}

INCLUDE_ASM("asm/main/nonmatchings/sef", sefGetParentLine);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefScaleIVectorAdd);

/*
 * config/units/math-main-w3-002e7f40.json: scale the XYZ lanes of an aligned
 * four-lane vector by the degrees-to-radians factor, leaving W untouched.
 * ee-vu-cop2 (docs/ps2-capabilities.md): the lane-selective VU0 macro-mode
 * multiply is what the C says, not an approximation of it; the compiler
 * materializes the .lit4 factor into an FPU register on its own (the `f`
 * operand), which is why the block has no lwc1 of its own.
 */
void sefDeg2RadVector(Vector4 *dst, Vector4 *src)
{
    __asm__ __volatile__("lqc2 $vf2, 0(%0)" : : "r"(src) : "memory");

    {
        register float degToRad asm("$f8") = lit4_004d8354;

        __asm__ __volatile__(
            "mfc1 $8, %1\n\t"
            "qmtc2.ni $8, $vf1\n\t"
            "vmulx.xyz $vf2, $vf2, $vf1x\n\t"
            "sqc2 $vf2, 0(%0)\n\t"
            "nop"
            :
            : "r"(dst), "f"(degToRad)
            : "$8", "memory"
        );
    }
}

INCLUDE_ASM("asm/main/nonmatchings/sef", sefMergeMatrixPos);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefLerpVectorB);

/*
 * sefLerpIVectorB (main:0x002e7fe8), re-treated from the accepted assembly
 * group src/main/sef/sefLerpIVectorB.s toward readable C with constrained
 * ee-vu-cop2 inline assembly (route ee-vu-cop2, docs/ps2-capabilities.md).
 *
 * Reads two packed signed-halfword key records -- `first` and, 10 bytes
 * later, `second` (the layout the original's own `addiu $2,$4,10` computes;
 * that address is ordinary pointer arithmetic and stays in C) -- each with
 * the R5900 unaligned doubleword ldl/ldr pair and widens the packed lanes to
 * words with pcgth/pextlh (the one evidenced MMI packed-halfword widening
 * idiom, user-authorized 2026-09-13: config/compiler-patterns.json CP-0169
 * -- cc1 2.96 emits no MMI for any C spelling, so this reproduces the
 * original's own hand-written idiom, not compiler output). Both widened
 * records are converted to float (vitof0.xyzw) and linearly interpolated by
 * `factor` (VF2 = VF1 + (VF2 - VF1) * factor, XYZ lanes only), then converted
 * back to packed integer lanes (vftoi0.xyzw) and stored as the complete
 * four-slot result at `dest`. The original has a .globl directive, so the
 * emitted binding stays GLOBAL.
 */
void sefLerpIVectorB(void *first, Vector4 *dest, float factor)
{
    void *second = (char *)first + 10;

    __asm__ __volatile__(
        "ldl $8, 7(%0)\n\t"
        "ldr $8, 0(%0)\n\t"
        "ldl $9, 7(%1)\n\t"
        "ldr $9, 0(%1)\n\t"
        "mfc1 $10, %3\n\t"
        "qmtc2.ni $10, $vf3\n\t"
        "pcgth $10, $0, $8\n\t"
        "pcgth $11, $0, $9\n\t"
        "pextlh $10, $10, $8\n\t"
        "pextlh $11, $11, $9\n\t"
        "qmtc2.ni $10, $vf1\n\t"
        "qmtc2.ni $11, $vf2\n\t"
        "vitof0.xyzw $vf1, $vf1\n\t"
        "vitof0.xyzw $vf2, $vf2\n\t"
        "vsub.xyz $vf2, $vf2, $vf1\n\t"
        "vmulx.xyz $vf2, $vf2, $vf3x\n\t"
        "vadd.xyz $vf2, $vf2, $vf1\n\t"
        "vftoi0.xyzw $vf2, $vf2\n\t"
        "sqc2 $vf2, 0(%2)\n\t"
        "nop"
        :
        : "r"(first), "r"(second), "r"(dest), "f"(factor)
        : "$8", "$9", "$10", "$11", "memory"
    );
}
