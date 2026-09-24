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

/*
 * The original routine keeps each literal-pool access as a distinct volatile
 * read across its quadrant branches; the pool addresses and load order are
 * evidenced by the function's original instructions.
 */
extern volatile const float D_004D82B4;
extern volatile const float D_004D82B8;
extern volatile const float D_004D82BC;
extern volatile const float D_004D82C0;
extern volatile const float D_004D82C4;
extern volatile const float D_004D82C8;
extern volatile const float D_004D82CC;
extern volatile const float D_004D82D0;
extern volatile const float D_004D82D4;
extern float *atanTbl_0;

float srsAtan2(float x, float y)
{
    int y_is_positive;
    int x_is_positive;
    float result = 0.0f;
    float angle;

    y_is_positive = y >= 0.0f;
    x_is_positive = x >= 0.0f;

    if (y == 0.0f) {
        if (x != 0.0f) {
            result = D_004D82B4;
            if (!x_is_positive) {
                return D_004D82B8;
            }
        }
        return result;
    }

    if (x == 0.0f) {
        if (!y_is_positive) {
            return D_004D82BC;
        }
        return result;
    }

    if (!y_is_positive) {
        y = -y;
    }
    if (!x_is_positive) {
        x = -x;
    }

    if (x <= y) {
        angle = atanTbl_0[(int)(x * 1024.0f / y)];

        if (y_is_positive) {
            if (!x_is_positive) {
                angle = -angle;
            }
        } else if (x_is_positive) {
            angle = D_004D82C0 - angle;
        } else {
            angle += D_004D82C4;
        }
    } else {
        angle = atanTbl_0[(int)(y * 1024.0f / x)];

        if (y_is_positive) {
            if (x_is_positive) {
                angle = D_004D82C8 - angle;
            } else {
                angle = -(D_004D82CC - angle);
            }
        } else if (x_is_positive) {
            angle += D_004D82D0;
        } else {
            angle = D_004D82D4 - angle;
        }
    }

    return angle;
}

INCLUDE_ASM("asm/main/nonmatchings/sef", sefRandf);

/*
 * sefIsBossID (main:0x002e11e8): true when character_id falls in the boss ID
 * range [0x97, 0xba] (sefCnvDeathEffectNo, sefSetupEnemy, this TU).
 */
static int sefIsBossID(int character_id)
{
    return (unsigned int)(character_id - 0x97) < 0x24;
}

/*
 * GNU EE native TI storage/copy type (docs/native-ti.md), used only for the
 * 16-byte aligned zero-fill blocks sefMemZero clears with por+sq; no wide
 * arithmetic is done on it.
 */
typedef unsigned int Quadword __attribute__((mode(TI)));

typedef union SefQuadBlock {
    Quadword quad;
} SefQuadBlock;

/*
 * sefMemZero (main:0x002e11f8): clears `size` bytes at `dest` in aligned
 * 16-byte blocks, then any remaining 4-byte word. sefInitLineData,
 * sefCreateParticle, sefInitScheduler, sefPushEffect, sefPopEffect,
 * sdvInitAlter and sdvDestroyAlter (all still assembly in this TU) are its
 * callers.
 */
void sefMemZero(void *dest, int size)
{
    SefQuadBlock *blocks = (SefQuadBlock *)dest;
    int block_count = size >> 4;
    int word_count = (size & 0xf) >> 2;
    int *words = (int *)(blocks + block_count);
    int i;

    for (i = 0; i < block_count; i++) {
        blocks[i].quad = 0;
    }
    for (i = 0; i < word_count; i++) {
        words[i] = 0;
    }
}

extern Matrix4 *MMathRotateMatrixXYZ(Matrix4 *out, Matrix4 *matrix, Vector4 *angles);
extern Matrix4 *MMathScaleMatrix(Matrix4 *out, Matrix4 *matrix, Vector4 *scale);

/*
 * sefCalcRotTransSMatrix (main:0x002e1270): builds `matrix` as a
 * translation-only matrix from `translation` (VU0 macro-mode broadcast of
 * vf0's own one-hot rows for the identity part, the same idiom
 * sefCalcInvView below uses, with vf1's own W lane forced to vf0's own W
 * and its XYZ lanes loaded from `translation`), then rotates it in place by
 * `angles` and scales it by `scale`. sefGetMotionNullMatrix,
 * sefGetParentMatrix and sefExecScheduler are its callers.
 */
void sefCalcRotTransSMatrix(Vector4 *angles, Vector4 *translation, Vector4 *scale, Matrix4 *matrix)
{
    __asm__ __volatile__("lqc2 vf1, 0(%0)" : : "r"(translation) : "memory");
    __asm__ __volatile__("vmove.w vf1, vf0" : : : "memory");
    __asm__ __volatile__("vmr32.xyzw vf2, vf0" : : : "memory");
    __asm__ __volatile__("vmr32.xyzw vf3, vf2" : : : "memory");
    __asm__ __volatile__("vmr32.xyzw vf4, vf3" : : : "memory");
    __asm__ __volatile__("sqc2 vf1, 48(%0)" : : "r"(matrix) : "memory");
    __asm__ __volatile__("sqc2 vf2, 32(%0)" : : "r"(matrix) : "memory");
    __asm__ __volatile__("sqc2 vf3, 16(%0)" : : "r"(matrix) : "memory");
    __asm__ __volatile__("sqc2 vf4, 0(%0)" : : "r"(matrix) : "memory");
    MMathRotateMatrixXYZ(matrix, matrix, angles);
    MMathScaleMatrix(matrix, matrix, scale);
}

extern StudioCamera *xglStudioGetActiveCamera(void);
extern Matrix4 *MMathRotateMatrixYXZ(Matrix4 *out, Matrix4 *matrix, Vector4 *angles);

/*
 * sefCalcInvView (main:0x002e12e0): writes the identity matrix into `matrix`
 * (VU0 macro-mode broadcast of vf0's own one-hot rows, the same idiom
 * sefCalcRotChange's own identity block below uses), then rotates it in
 * place by the active studio camera's own rotation (StudioCamera::rotation,
 * include/shared.h, at +0xA0) in YXZ order. sefDrawEffect is its only
 * caller.
 */
static void sefCalcInvView(Matrix4 *matrix) {
    StudioCamera *camera = xglStudioGetActiveCamera();

    __asm__ __volatile__("vmr32.xyzw vf1, vf0" : : : "memory");
    __asm__ __volatile__("vmr32.xyzw vf2, vf1" : : : "memory");
    __asm__ __volatile__("vmr32.xyzw vf3, vf2" : : : "memory");
    __asm__ __volatile__("sqc2 vf0, 48(%0)" : : "r"(matrix) : "memory");
    __asm__ __volatile__("sqc2 vf1, 32(%0)" : : "r"(matrix) : "memory");
    __asm__ __volatile__("sqc2 vf2, 16(%0)" : : "r"(matrix) : "memory");
    __asm__ __volatile__("sqc2 vf3, 0(%0)" : : "r"(matrix) : "memory");
    MMathRotateMatrixYXZ(matrix, matrix, &camera->rotation);
}

INCLUDE_ASM("asm/main/nonmatchings/sef", sefSearchMapperIndex2);

extern int sefSearchMapperIndex2(int effect_id, int number, int category);
extern int srsAnalyzeEftNo(int effect_id, int *number, int *category);

/*
 * sefSearchMapperIndex (main:0x002e1540): decomposes the effect id into a
 * number/category pair via srsAnalyzeEftNo, then feeds them to the mapper
 * lookup sefSearchMapperIndex2, discarding its result.
 */
void sefSearchMapperIndex(int effect_id) {
    int number;
    int category;

    srsAnalyzeEftNo(effect_id, &number, &category);
    sefSearchMapperIndex2(effect_id, number, category);
}

extern unsigned char _battleData[];
extern unsigned char _ptAlloc[];

/*
 * sevInitPtAllocator (main:0x002e1580): clears the whole _ptAlloc particle
 * allocator table and _battleData, then rebuilds _ptAlloc's own trailing
 * free-index queue (1024 entries, main VA 0x00794110's 0xa07fe-byte offset)
 * with the descending indices 0x3ff..0. sefInitEffect is its only caller.
 */
static void sevInitPtAllocator(void)
{
    short *slot;
    int index;

    memset(_ptAlloc, 0, 0xA0810);
    slot = (short *)(_ptAlloc + 0xA07FE);
    memset(_battleData, 0, 0x230);
    index = 0x3FF;
    do {
        *slot = (short)index;
        index--;
        slot--;
    } while (index >= 0);
}

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

/*
 * A 10-byte keyed-animation record (config/units, sefProgressKey5): the
 * end_frame this function tests against 1024 and the following record's own
 * jump index are the only fields any accepted function of this TU touches;
 * the six bytes between them are not yet evidenced.
 */
typedef struct SefKey5 {
    short end_frame;
    unsigned char unmodeled_2[6];
    short jump;
} SefKey5;

/*
 * sefProgressKey5 (main:0x002e1740): like sefProgressKey3 above, but on the
 * wider 10-byte SefKey5 keys sefLerpVector/sefLerpIVector/sefLerpIVector2
 * (all still assembly in this TU) share: advances `state`'s frame, and once
 * it reaches the current key's own end_frame either follows the next key's
 * jump index or moves to the next key in sequence, returning the active key.
 */
static SefKey5 *sefProgressKey5(SefKey5 *keys, SefProgressState *state)
{
    int key_index = state->key_index;
    int frame = state->frame + 1;
    SefKey5 *key = keys + key_index;
    short jump;

    state->frame = frame;
    if (frame < 1024) {
        if (key->end_frame != 1024 && frame >= key[1].end_frame) {
            key++;
            jump = key->jump;
            if (jump >= 0) {
                state->key_index = jump;
                key = keys + jump;
                state->frame = key->end_frame;
            } else {
                state->key_index = key_index + 1;
            }
        }
    }
    return key;
}

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

extern void sefGetPoint(Vector4 *position, unsigned char *source);
extern unsigned char _zeroPos_004CBF00[];

/*
 * sefGetPosition (main:0x002e2240): fills `position`'s XYZ via sefGetPoint
 * from `source` (or the shared zero-position table when source is NULL),
 * then forces the resulting vector's W lane to 1.0f.
 */
void sefGetPosition(Vector4 *position, unsigned char *source) {
    unsigned char *point = source;

    if (point == 0) {
        point = _zeroPos_004CBF00;
    }
    sefGetPoint(position, point);
    position->w = 1.0f;
}

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

/*
 * sefGetCubePosBtm (main:0x002e23c8) is still INCLUDE_ASM in this TU;
 * declared here so sefGetCubePosTop (main:0x002e24c0) can call it. Both are
 * LOCAL in the original, so both stay static. `extent` is the cube's
 * integer extent vector (four ints, loaded with lqc2 and converted with
 * vitof0); the Top variant forwards it untouched.
 */
static void sefGetCubePosBtm(Vector4 *pos, const int *extent);

/*
 * sefGetCubePosTop (main:0x002e24c0): fills `pos` with the bottom corner via
 * sefGetCubePosBtm, then negates its Y component to mirror it to the top.
 */
static void sefGetCubePosTop(Vector4 *pos, const int *extent)
{
    sefGetCubePosBtm(pos, extent);
    pos->y = -pos->y;
}

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

/*
 * sefGetDirVector (main:0x002e2938): a single VU0 macro-mode sequence with
 * no general-purpose instruction of its own (all 7 hardware operations,
 * leaf, no calls; sefGetSpeed is the only caller). Negates `source`'s Z
 * lane against the VU0 architectural constant vf0 (0,0,0,1), accumulates it
 * against the caller's own vf7/vf8/vf3/vf4 basis (ACC = vf7*x + vf8*y +
 * vf3*(-z), then vf1 = ACC + vf4*w) and stores the four-lane result to
 * `dest`. vf7/vf8/vf3/vf4 are whatever sefGetSpeed already loaded into VU0
 * before this call; this function neither reads nor writes them as C state.
 */
static void sefGetDirVector(void *dest, void *source) {
    __asm__ __volatile__("lqc2 vf1, 0(%0)\n\tvsub.z vf1, vf0, vf1\n\tvmulax.xyzw ACC, vf7, vf1x\n\tvmadday.xyzw ACC, vf8, vf1y\n\tvmaddaz.xyzw ACC, vf3, vf1z\n\tvmaddw.xyzw vf1, vf4, vf1w\n\tsqc2 vf1, 0(%1)\n\tnop" :  : "r"(source), "r"(dest) : "memory");
}

extern void sefGetOfsRange(void *position, unsigned char *offset_table, short index);

/*
 * SefParticleOffset (sefGetStartPos/sefGetTargetPos, main:0x002e2960 and
 * main:0x002e29b0): the only fields sefCreateParticle's effect record these
 * two functions evidence -- a start and a target position vector, and a
 * start and a target offset-table/index pair consumed opaquely by
 * sefGetOfsRange (still assembly). The bytes between and around them are
 * not evidenced by any accepted function in this TU.
 */
typedef struct SefParticleOffset {
    unsigned char unmodeled_00[0x30];
    Vector4 start_position;                  /* +0x030 */
    unsigned char unmodeled_40[0x10];
    Vector4 target_position;                 /* +0x050 */
    unsigned char unmodeled_60[0x110];
    unsigned char start_offset_table[0x20];  /* +0x170 */
    unsigned char target_offset_table[0x20]; /* +0x190 */
    unsigned char unmodeled_1b0[0x24];
    short *start_index;                      /* +0x1D4 */
    unsigned char unmodeled_1d8[4];
    short *target_index;                     /* +0x1DC */
} SefParticleOffset;

/*
 * sefGetStartPos (main:0x002e2960): looks up the configured start offset
 * through sefGetOfsRange using the record's own start offset table and the
 * index its start_index points to, then adds the record's own
 * start_position vector into the caller's own position accumulator.
 * sefCreateParticle is its only caller.
 */
static void sefGetStartPos(Vector4 *position, SefParticleOffset *record) {
    sefGetOfsRange(position, record->start_offset_table, *record->start_index);
    __asm__ __volatile__("lqc2 vf1, 0(%0)" : : "r"(&record->start_position) : "memory");
    __asm__ __volatile__("lqc2 vf2, 0(%0)" : : "r"(position) : "memory");
    __asm__ __volatile__("vadd.xyz vf1, vf1, vf2" : : : "memory");
    __asm__ __volatile__("sqc2 vf1, 0(%0)" : : "r"(position) : "memory");
}

/*
 * sefGetTargetPos (main:0x002e29b0): identical in shape to sefGetStartPos
 * above, at the record's target fields instead of its start fields.
 * sefCreateParticle is also its only caller.
 */
static void sefGetTargetPos(Vector4 *position, SefParticleOffset *record) {
    sefGetOfsRange(position, record->target_offset_table, *record->target_index);
    __asm__ __volatile__("lqc2 vf1, 0(%0)" : : "r"(&record->target_position) : "memory");
    __asm__ __volatile__("lqc2 vf2, 0(%0)" : : "r"(position) : "memory");
    __asm__ __volatile__("vadd.xyz vf1, vf1, vf2" : : : "memory");
    __asm__ __volatile__("sqc2 vf1, 0(%0)" : : "r"(position) : "memory");
}

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

extern void sevFreePtAllocator(short handle);

#define SEF_LOCAL_DATA_PT_TABLE 0x600

/*
 * sefFreeLocalData (main:0x002e34e0): releases the particle allocator slot
 * this line owns at index `index` (SEF_LOCAL_DATA_PT_TABLE, an array of
 * allocator handles) and clears it to -1. The slot is only touched when
 * `index` is valid (< 256); an out-of-range index still returns the byte
 * offset that would have been used, which neither caller
 * (sefExecLineLocalData, sefDestroyLocalData) treats as anything but a
 * non-(-1) sentinel.
 */
static int sefFreeLocalData(unsigned char *line_data, unsigned int index) {
    int valid = index < 0x100;
    int offset = (int)(index * 2) + SEF_LOCAL_DATA_PT_TABLE;

    if (valid) {
        short *slot = (short *)(line_data + offset);

        sevFreePtAllocator(*slot);
        offset = -1;
        *slot = -1;
    }
    return offset;
}

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

/*
 * PARTIAL ACCESSED VIEW of an effect-table record (sefInitEffectData): the
 * first 0x20 bytes are filled with 0xff by memset and are not otherwise
 * touched by this function; owner, effect_no and category are the fields it
 * sets from its own parameters, flags and frame are reset to 0. The 2-byte
 * gap after flags is not yet evidenced by any accepted function of this TU.
 */
typedef struct EffectData {
    unsigned char unmodeled_00[0x20];
    int owner;
    short category;
    short effect_no;
    short flags;
    unsigned char unmodeled_2a[2];
    short frame;
} EffectData;

/*
 * sefInitEffectData (main:0x002e49e8): fills `effect`'s first 0x20 bytes
 * with 0xff, then sets owner/effect_no/category from its own parameters and
 * resets flags/frame to 0. sefInitScheduler and sefInitEffectTbl are its
 * callers.
 */
static void sefInitEffectData(EffectData *effect, int owner, int effect_no, int category)
{
    memset(effect, -1, 0x20);
    effect->owner = owner;
    effect->effect_no = (short)effect_no;
    effect->category = (short)category;
    effect->frame = 0;
    effect->flags = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/sef", sefDestroyEffectData);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefAllocEffectData);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefFreeEffectData);

/*
 * sefCheckFinish (main:0x002e4b80): when `condition` carries the 0x4000
 * "loop ended" bit, marks the current scheduler's own record finished
 * (SchedulerState::flags |= 0x40, sef.h). The first parameter is not read
 * by this function.
 */
static void sefCheckFinish(int effect_no, int condition) {
    if (condition & 0x4000) {
        _nowScheduler->flags |= 0x40;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/sef", sefExecEffectData);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefInitScheduler);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefAllocScheduler);

extern void xglSoundEffectStopID(int soundId, int channel);
static void sefDestroyEffectData(unsigned char *effect);

/*
 * sefFreeSchedulerCf (main:0x002e5118): releases an active scheduler
 * record -- stops its currently playing sound effect if any, destroys each
 * of its 32 effect slots, and clears the record's script binding. A NULL
 * scheduler or one that is already free (inUse == 0) does nothing.
 * sefDeleteEffect2 and sefDeleteEffectCf (this file) tail-call this with
 * their own scheduler argument unchanged.
 */
void sefFreeSchedulerCf(SchedulerState *scheduler)
{
  int soundId;
  int i;

  if (scheduler != 0 && scheduler->inUse != 0)
  {
    soundId = scheduler->soundId;
    if (soundId > 0)
    {
      xglSoundEffectStopID(soundId, 0);
      scheduler->soundId = 0;
    }
    for (i = 0; i < 32; i++)
    {
      sefDestroyEffectData(scheduler->effects[i]);
    }
    scheduler->scriptBinding.state = 0;
    scheduler->inUse = 0;
    scheduler->scriptBinding.script_id = -1;
    scheduler->scriptBinding.task_id = -1;
  }
}

INCLUDE_ASM("asm/main/nonmatchings/sef", sefFreeScheduler);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefDestroyScriptScheduler);

extern void sefFreeScheduler(unsigned int scheduler_index);

/*
 * sefDestroyScriptScheduler2 (main:0x002e5320): scDestroyScript2 (main:
 * 0x002e9e48, still assembly in a different TU) forwards its own two
 * parameters here unchanged and also to scDeleteTask, whose own accepted
 * parameters are named script_index and task_index (src/main/sc_get.c).
 * This loop walks the scheduler table's own SchedulerState records (sef.h)
 * at their established 0xab0 stride and frees the first slot whose
 * scriptBinding matches.
 */
void sefDestroyScriptScheduler2(int script_index, int task_index)
{
    SchedulerState *scheduler;
    ScriptBinding *record;
    int index;

    index = 0;
    scheduler = (SchedulerState *)_scheduler;
    record = &scheduler->scriptBinding;
    do {
        if (record->script_id == script_index && record->task_id == task_index) {
            sefFreeScheduler(index);
        }
        index++;
        record = (ScriptBinding *)((unsigned char *)record + 0xab0);
    } while (index < 0x80);
}

INCLUDE_ASM("asm/main/nonmatchings/sef", sefInitEffectTbl);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefCreateScheduler2);

extern int sefCreateScheduler2(int effect_no, int value2, int value3, int value4, int flags, int value5);

/*
 * sefCreateScheduler (main:0x002e5900): forwards to the scheduler allocator
 * sefCreateScheduler2 with a plain 0 for its own flags slot, and returns its
 * handle unchanged: scEFFECTScript/scEFFECT2Script/scEFFECT3Script store
 * that handle as `effect_scheduler` (src/main/sc_get.h). value2/value3/
 * value4/value5 are forwarded unchanged; this allocation has no further
 * evidence for them.
 */
int sefCreateScheduler(int effect_no, int value2, int value3, int value4, int value5) {
    return sefCreateScheduler2(effect_no, value2, value3, value4, 0, value5);
}

extern int offset_2[];

/*
 * sefGetSizeOffset (main:0x002e5920): returns the size-offset table entry at
 * `offset_2[index]` (config/symbols/main.txt, offset_2 @ 0x004CBF48, 5
 * ints); sefCreateReactionEffect and sefCnvDeathEffectNo are its callers.
 */
static int sefGetSizeOffset(int index) {
    return offset_2[index];
}

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

extern void smInitilize(void *pool, unsigned int poolSize);
extern void sresInitMemoryRes(void);
extern void srsInitCdRead(void);
extern void svInitImageMapper(void);
extern void sefInitScheduler(void);
extern void scInitScript(void);
extern void sdvInitSpecialWork(void);
extern void sdvInitAmbient(void);
extern void sresLoadCommonMemory(void);
extern unsigned char _eftBuffer[];
extern unsigned char _battlePrm[];
extern int _sefLoadEftQue;
extern short _initialize;

/*
 * sefInitEffect (main:0x002e6b60): the effect subsystem's one-time
 * bring-up. Initializes the effect memory pool (_eftBuffer, 0xd4800 bytes),
 * the resource/CD-read/particle-allocator/image-mapper/scheduler/script
 * subsystems, clears the battle parameter and battle-data tables, brings
 * up the special-work and ambient renderers and the common resource set,
 * clears the pending load queue and marks the subsystem initialized.
 */
void sefInitEffect(void)
{
    smInitilize(_eftBuffer, 0xD4800);
    sresInitMemoryRes();
    srsInitCdRead();
    sevInitPtAllocator();
    svInitImageMapper();
    sefInitScheduler();
    scInitScript();
    memset(_battlePrm, 0, 0x34);
    memset(_battleData, 0, 0x230);
    sdvInitSpecialWork();
    sdvInitAmbient();
    sresLoadCommonMemory();
    _sefLoadEftQue = 0;
    _initialize = 1;
}

INCLUDE_ASM("asm/main/nonmatchings/sef", sefInitEffectBattle);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefInitEffectCf);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefSetupPlayer);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefSetupEnemy);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefReleaseID);

extern void sresLoadBattleData(unsigned char *battle_prm);
extern unsigned char _battlePrm[];

/*
 * sefSetupEffect (main:0x002e6fe0): loads battle effect resources into
 * _battlePrm (main VA 0x00794340). sefSetupPlayer and sefSetupEnemy are its
 * only callers besides the seffectDebugBattle debug entry point.
 */
void sefSetupEffect(void) {
    sresLoadBattleData(_battlePrm);
}

/*
 * sefIsEntryBoss (main:0x002e7000): 0x00794370 is _battlePrm + 0x30
 * (_battlePrm at main VA 0x00794340, cleared to 0x34 bytes by sefInitEffect
 * and loaded by sresLoadBattleData in sefSetupEffect above). Compiled in
 * isolation, the original relocates directly against this narrower address
 * rather than folding +0x30 onto _battlePrm's own relocation, so this
 * declaration keeps the scaffold's own splat name.
 */
extern int D_00794370[];

int sefIsEntryBoss(void) {
    return D_00794370[0];
}

extern void scDestroyScriptAll(void);
extern void sdvDestroyAlters(void);
extern void sdvInitAmbient(void);
extern void sresFreeReloaderMemory(int reload_bgm);
extern unsigned char _battleActor[];
extern int _sefLoadEftQue;
void sefKillEffect(int effect_no);

/*
 * sefDestroyEffect (main:0x002e7010): tears down the whole battle effect
 * state -- clears the load queue, kills every running scheduler, resets the
 * ambient/script/reload-memory subsystems and the battle parameter/state/
 * actor tables, then destroys every alter. sresFreeReloaderMemory's own
 * argument selects the disk-side reloader (1). SimajiriTest and
 * sefInitEffectBattle are its callers.
 */
void sefDestroyEffect(void)
{
    _sefLoadEftQue = 0;
    sefKillEffect(-1);
    sdvInitAmbient();
    scDestroyScriptAll();
    sresFreeReloaderMemory(1);
    memset(_battlePrm, 0, 0x34);
    memset(_battleData, 0, 0x230);
    memset(_battleActor, 0, 0xDA0);
    sdvDestroyAlters();
}

/*
 * sefDestroyEffectCf (main:0x002e7088): the character-file variant of
 * sefDestroyEffect above, identical except sresFreeReloaderMemory's own
 * argument selects the memory-card-side reloader (0). seffectDebugCf,
 * seffectDebugBattle and sefInitEffectCf are its callers.
 */
void sefDestroyEffectCf(void)
{
    _sefLoadEftQue = 0;
    sefKillEffect(-1);
    sdvInitAmbient();
    scDestroyScriptAll();
    sresFreeReloaderMemory(0);
    memset(_battlePrm, 0, 0x34);
    memset(_battleData, 0, 0x230);
    memset(_battleActor, 0, 0xDA0);
    sdvDestroyAlters();
}

/*
 * sefCnvEtEffectNo (main:0x002e74e0) is still assembly in this TU and LOCAL
 * in the original, so it is declared static. It takes two parameters:
 * besides the effect number in a0 it tests a1 against 2 (effects 2024..2029
 * shift by 3) and against 1 (effects 2038..2041 shift by 2), so the same
 * encounter effect resolves to a per-character variant. Every call site
 * loads a1 first: sefIsFinishEffect2 (below) passes the scheduler record's
 * character_id; seffectDebugDb (main:0x002dd038) passes 1,
 * seffectDebugBattle (main:0x002dd568) its debug character and ov01
 * scenarioBatExecPhase10 (0x00a0bf34) calcUPGet(...)->charaId, those three
 * through sefLoadEffect.
 */
static int sefCnvEtEffectNo(int effect_no, int character_id);
extern void svFileLoadScript(int mode, int effect_no);

/*
 * sefLoadEffect (main:0x002e7100): both parameters stay live in a0/a1 across
 * the call (neither register is rewritten before the jal), so they are
 * forwarded unchanged to sefCnvEtEffectNo; all three callers load a1 before
 * calling sefLoadEffect. The tail call (j, not jal+jr) makes
 * svFileLoadScript's own return value sefLoadEffect's return value on that
 * path; the effect_no<=0 path falls straight through to the epilogue without
 * setting v0, so the original never uses this function's result and the
 * return type is void.
 */
void sefLoadEffect(int effect_no, int character_id)
{
    if (effect_no > 0) {
        svFileLoadScript(0, sefCnvEtEffectNo(effect_no, character_id));
    }
}

extern int srsLeaveCdRead(void);
extern int _sefLoadEftQue;

int sefCheckLoad(void)
{
    if (_sefLoadEftQue != 0) {
        return 1;
    }
    return srsLeaveCdRead() > 0;
}

extern void scExecEffect(void);
extern void sdvExecAlters(void);
extern short _hitFlag;
extern short _hitSignal;
extern short _seSignal;

void sefExecEffect(void)
{
    scExecEffect();
    sdvExecAlters();
    _hitFlag = 0;
    _hitSignal = 0;
    _seSignal = 0;
}

/*
 * sefProgressEffect (main:0x002e7198): the entry test reads the parameter
 * register directly (blez a0) before it is copied into the loop counter, and
 * the counter is decremented after each call (the decrement is scheduled
 * into the call's own delay slot), matching the original's own instruction
 * order.
 */
void sefProgressEffect(int frame_count)
{
    int remaining;

    if (frame_count > 0) {
        remaining = frame_count;
        do {
            sefExecEffect();
            remaining--;
        } while (remaining != 0);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/sef", sefDrawEffect);

extern void svDrawScheduler3D(int flags);
extern short _initialize;

/*
 * GameLoopState (main VA 0x00338680, src/main/game_camera.c) is a
 * 0x2a030-byte global; only its +0x10 flags word is evidenced here (the
 * same field main/tu148 models as GameLoopFlagsPrefix, src/main/script.h).
 */
typedef struct {
    unsigned char unmodeled_00[0x10];
    int flags;
} GameLoopFlagsView;

extern GameLoopFlagsView GameLoopState;

#define SEF_DRAW_FLAG_PENDING 0x04000000

/*
 * sefDrawEffect3D (main:0x002e7220): while `_initialize` is set and
 * GameLoopState's flags word does not already carry SEF_DRAW_FLAG_PENDING,
 * calls svDrawScheduler3D with that same bit as its argument. xglRenderMove
 * is its only caller.
 */
void sefDrawEffect3D(void) {
    if (_initialize != 0) {
        if (!(GameLoopState.flags & SEF_DRAW_FLAG_PENDING)) {
            svDrawScheduler3D(SEF_DRAW_FLAG_PENDING);
        }
    }
}

extern void svDrawScheduler2D(int flags);

/*
 * sefDrawEffect2D (main:0x002e7260): the same gate as sefDrawEffect3D above,
 * calling svDrawScheduler2D instead.
 */
void sefDrawEffect2D(void) {
    if (_initialize != 0) {
        if (!(GameLoopState.flags & SEF_DRAW_FLAG_PENDING)) {
            svDrawScheduler2D(SEF_DRAW_FLAG_PENDING);
        }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/sef", sefCnvDeathEffectNo);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefCnvWaitEffectNo);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefDeleteEffectWait);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefCnvEtEffectNo);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefCreateEffect);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefCreateEffect2);

/*
 * sefDeleteEffect2 (main:0x002e7748) tail-calls sefFreeSchedulerCf (main:
 * 0x002e5118, still assembly in this file) with its own argument unchanged:
 * no register is written before the j. sefDeleteEffectWait (main:0x002e7430)
 * passes the address of a _scheduler record in a0 and never reads a result
 * back from the call.
 */
extern void sefFreeSchedulerCf(SchedulerState *scheduler);

void sefDeleteEffect2(SchedulerState *scheduler)
{
    sefFreeSchedulerCf(scheduler);
}

/*
 * sefLoadEffectCf (main:0x002e7760) also sets up no registers before its
 * call: seffectDebugCf and getPeer_Effect (its only callers) load a cf id
 * into a0 and an effect number into a1 first, so both are real parameters
 * forwarded unchanged to srsFileLoadCf. The result is discarded; v0 is
 * cleared to 0 after the call rather than read from it.
 */
extern void srsFileLoadCf(int cf_id, int effect_no);

int sefLoadEffectCf(int cf_id, int effect_no)
{
    srsFileLoadCf(cf_id, effect_no);
    return 0;
}

/*
 * sefLoadEffectCfName (main:0x002e7780) converts effect_name with
 * srsEffectNameToID before forwarding to the same two-parameter
 * srsFileLoadCf established above, keeping the cf id live in a saved
 * register across the conversion call.
 */
extern int srsEffectNameToID(int effect_name);

int sefLoadEffectCfName(int cf_id, int effect_name)
{
    int effect_no = srsEffectNameToID(effect_name);

    if (effect_no > 0) {
        srsFileLoadCf(cf_id, effect_no);
    }
    return 0;
}

/*
 * sefLoadMemoryEffectCf (main:0x002e77c0) sets up no registers before
 * calling srsMemoryLoadCf, so it forwards whatever its own caller passed;
 * modeled here with the same three parameters srsMemoryLoadCf takes in
 * sefLoadMemoryEffectCfName below, the only call in this file with evidenced
 * arguments.
 */
extern void srsMemoryLoadCf(int cf_id, int effect_no, int buffer);

int sefLoadMemoryEffectCf(int cf_id, int effect_no, int buffer)
{
    srsMemoryLoadCf(cf_id, effect_no, buffer);
    return 0;
}

/*
 * sefLoadMemoryEffectCfName (main:0x002e77e0) converts effect_name with
 * srsEffectNameToID before forwarding to srsMemoryLoadCf, keeping the cf id
 * and the buffer argument live in saved registers across the conversion
 * call.
 */
int sefLoadMemoryEffectCfName(int cf_id, int effect_name, int buffer)
{
    int effect_no = srsEffectNameToID(effect_name);

    if (effect_no > 0) {
        srsMemoryLoadCf(cf_id, effect_no, buffer);
    }
    return 0;
}

INCLUDE_ASM("asm/main/nonmatchings/sef", sefCreateEffectCf2);

extern void sefCreateEffectCf2(int cf_id, int effect_no, int value3, int value4);

/*
 * sefCreateEffectCf (main:0x002e7918): forwards to the CF effect allocator
 * sefCreateEffectCf2 with a fixed -1 for its own 4th parameter; cf_id and
 * effect_no follow the naming srsFileLoadCf's callers already evidence for
 * this file's other Cf functions (sefLoadEffectCf above). value3 is
 * forwarded unchanged; this allocation has no further evidence for it.
 */
void sefCreateEffectCf(int cf_id, int effect_no, int value3) {
    sefCreateEffectCf2(cf_id, effect_no, value3, -1);
}

/*
 * sefDeleteEffectCf (main:0x002e7930) is another tail call to
 * sefFreeSchedulerCf, identical in shape to sefDeleteEffect2 above;
 * CheckGameSymbol (one of its callers) loads a saved scheduler handle into
 * a0 and never reads a result back.
 */
void sefDeleteEffectCf(SchedulerState *scheduler)
{
    sefFreeSchedulerCf(scheduler);
}

/* The flag sefRewindEffectCf sets in SchedulerState's flags word (sef.h);
 * sefFreeScheduler (main:0x002e51a8) tests the same word with a wider mask
 * (0x202). */
#define SEF_SCHEDULER_REWIND_FLAG 1

void sefRewindEffectCf(SchedulerState *scheduler)
{
    if (scheduler != 0) {
        scheduler->flags |= SEF_SCHEDULER_REWIND_FLAG;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/sef", sefClearEffectCf);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefIsFinishEffect);

/*
 * The fields of a 0xab0-byte _scheduler record sefIsFinishEffect2 reads,
 * as displacements from the record + 0xa70 anchor its loop keeps in a
 * register (the same anchor sefKillEffect below and sefIsFinishEffect, still
 * assembly, use): the liveness word sefIsDeadSchduler tests at +0x6b0, the
 * owning character at +0xa74 (sefCnvEtEffectNo's character_id) and the
 * effect number at +0xa78 (the field sefKillEffect matches).
 */
#define SEF_RECORD_ANCHOR 0xa70
#define SEF_RECORD_LIVE (0x6b0 - SEF_RECORD_ANCHOR)
#define SEF_RECORD_CHARACTER_ID (0xa74 - SEF_RECORD_ANCHOR)
#define SEF_RECORD_EFFECT_NO (0xa78 - SEF_RECORD_ANCHOR)
#define SEF_SCHEDULER_RECORD_SIZE 0xab0
#define SEF_SCHEDULER_COUNT 0x80

/*
 * sefIsFinishEffect2 (main:0x002e7aa8): returns 1 when no live scheduler
 * record is still playing effect_no, comparing against each record's effect
 * number converted for its own character, so a per-character variant counts
 * as the effect it was started from. A non-positive effect_no is always
 * finished. As in sefKillEffect, record_offset stays a separate assignment:
 * folded into record's initializer, cc1 2.96 folds the 0xa70 into
 * _scheduler's own %lo relocation (one addiu instead of the original's two)
 * and no longer reproduces the original bytes. The unused index is counted
 * down (0x7f..0, bgez) by the compiler's own loop reversal.
 */
int sefIsFinishEffect2(int effect_no)
{
    int record_offset;
    unsigned char *record;
    int index;
    int running;

    running = 0;
    if (effect_no <= 0) {
        return 1;
    }
    record_offset = SEF_RECORD_ANCHOR;
    record = _scheduler + record_offset;
    for (index = 0; index < SEF_SCHEDULER_COUNT; index++) {
        if (*(int *)(record + SEF_RECORD_LIVE) != 0
            && sefCnvEtEffectNo(*(short *)(record + SEF_RECORD_EFFECT_NO),
                                *(short *)(record + SEF_RECORD_CHARACTER_ID))
                   == effect_no) {
            running++;
        }
        record += SEF_SCHEDULER_RECORD_SIZE;
    }
    return running == 0;
}

void sefSetLoadQue(int effect_no)
{
    _sefLoadEftQue = effect_no;
}

int *sefGetLoadQue(void)
{
    return &_sefLoadEftQue;
}

void sefExecLoadQue(void)
{
    if (_sefLoadEftQue > 0) {
        svFileLoadScript(0, _sefLoadEftQue);
    }
}

extern void sefFreeScheduler(unsigned int scheduler_index);

/*
 * sefKillEffect (main:0x002e7b80): the loop's running pointer is preset to
 * _scheduler + record_offset, the same anchor sefIsDeadSchduler's own 0x6b0
 * offset (this TU) and sefIsFinishEffect2's own +8 offset (still assembly in
 * this TU) resolve against, so both fields this loop dereferences -- the
 * 0x6b0 liveness word, reached at record-0x3c0 from here, and the 0xa78
 * effect number, at record+8 -- use small displacements from it.
 * record_offset stays a separate assignment (not folded into record's own
 * initializer): with it inlined, cc1 2.96 swaps which of record/index lands
 * in s0 versus s1 and no longer reproduces the original bytes.
 */
void sefKillEffect(int effect_no)
{
    int record_offset;
    unsigned char *record;
    int index;

    record_offset = 0xa70;
    record = _scheduler + record_offset;
    index = 0;
    do {
        if (*(int *)(record - 0x3c0) != 0
            && (effect_no < 0 || *(short *)(record + 8) == effect_no)) {
            sefFreeScheduler(index);
        }
        index++;
        record += 0xab0;
    } while (index < 0x80);
}

void sefHitEffect(void) {
    _hitFlag = 1;
}

INCLUDE_ASM("asm/main/nonmatchings/sef", sefPushEffect);

INCLUDE_ASM("asm/main/nonmatchings/sef", sefPopEffect);

/* The effect tables are scaffold-owned, so these helpers expose their original
 * addresses without defining or renaming the backing data. */
unsigned char *sefGetBattleData(void)
{
    return _battleData;
}

/*
 * sefGetDmgNull returns the same two target ids sefCalcLocalMatrix selects
 * for its target == 529 case (this file, main:0x002e2d48, sef.c:351-352).
 * D_0079411C is _battleData + SEF_BATTLE_PHASE (declared in sef.h with the
 * measured relocation evidence for why it keeps the scaffold's own name).
 */
#define SEF_TARGET_DAMAGE_NULL_A 527
#define SEF_TARGET_DAMAGE_NULL_B 528

int sefGetDmgNull(void)
{
    return (D_0079411C[0] >= 2) ? SEF_TARGET_DAMAGE_NULL_A : SEF_TARGET_DAMAGE_NULL_B;
}

/* _ptAlloc's per-record byte size, the same 640 sefCalcLocalMatrix already
 * multiplies by at sef.c:359. */
#define SEF_PT_ALLOC_RECORD_SIZE 640

unsigned char *sevGetPtAllocator(int allocator_index)
{
    return &_ptAlloc[allocator_index * SEF_PT_ALLOC_RECORD_SIZE];
}

unsigned char *sevGetPtAllocator2(int allocator_index)
{
    return &_ptAlloc[allocator_index * SEF_PT_ALLOC_RECORD_SIZE];
}

/* _lineData's per-record byte size; evidenced only here. */
#define SEF_LINE_DATA_RECORD_SIZE 0x820

unsigned char *sefGetLineAdr(int line_index)
{
    if (line_index < 0) {
        return 0;
    }
    return &_lineData[line_index * SEF_LINE_DATA_RECORD_SIZE];
}

unsigned char *sefGetScheduler(void)
{
    return _scheduler;
}

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

/* Same gate and record address as the target == 4 case in sefCalcLocalMatrix
 * (this file, sef.c:358-359), but on parameters rather than on _parentLine
 * and _nowParentLocal directly. */
unsigned char *sefGetParentLine(int line, int parent_local)
{
    if (line >= 0 && parent_local >= 0) {
        return &_ptAlloc[parent_local * SEF_PT_ALLOC_RECORD_SIZE];
    }
    return 0;
}

/*
 * sefScaleIVectorAdd (main:0x002e7f18): converts the integer vector at
 * `scale_source` to float (VU0 macro-mode vitof0), scales its XYZ lanes by
 * `scale` (moved into vf2.x through the GPR the compiler's own mfc1
 * materializes for the "r" operand, matching the original's own transfer),
 * adds `addend`'s XYZ lanes and stores the sum to `dest`. Leaf, no calls,
 * no evidenced caller in this allocation.
 */
void sefScaleIVectorAdd(void *dest, void *scale_source, void *addend, float scale) {
    __asm__ __volatile__("lqc2 vf1, 0(%0)\n\tlqc2 vf3, 0(%1)\n\tvitof0.xyzw vf1, vf1\n\tqmtc2.ni %2, vf2\n\tvmulx.xyz vf1, vf1, vf2x\n\tvadd.xyz vf3, vf1, vf3\n\tsqc2 vf3, 0(%3)\n\tnop" :  : "r"(scale_source), "r"(addend), "r"(scale), "r"(dest) : "memory");
}

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
