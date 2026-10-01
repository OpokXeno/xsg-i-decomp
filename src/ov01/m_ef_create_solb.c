/*
 * OV01 original TU 29: 0x00a36be8..0x00a37ab8 (7 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/m_math.h"
#include "m_ef_create_solb.h"

extern void *memset(void *destination, int value, unsigned int count);
extern void MEfGetActorCoord(void *dst, u32 actor, u32 coord);
static void makePath(void *work);
static void makeHermiteParam(int mode, void *work);
extern void MGsGPInit(void *work, int flags, int mode);
static void fnSOLB_PR000(void *self, void *work);
static void fnSOLB_DP000(void *self, void *work);
static void fnSOLB_PO000(void *self, void *work);

int MEfCreate_SOLB(void *work)
{
    unsigned char *base = (unsigned char *)work;
    SolbState *state = (SolbState *)(base + 32);

    state->age = 0;
    memset(state->active, 0, sizeof(state->active));
    memset(state->birth_age, 0, sizeof(state->birth_age));
    memset(state->trail, 0, sizeof(state->trail));
    MEfGetActorCoord(state->trail, state->actor, state->coord);
    state->segment = 0;
    state->frame = 0;
    makePath(state);
    makeHermiteParam(0, state);
    /* The MGs packet record follows the SOLB state, at state + 0x350. */
    MGsGPInit(base + 880, 0, 0);
    ((SOLBProcessSlot *)(base + 4))->process_callback = fnSOLB_PR000;
    ((SOLBDrawPacketSlot *)(base + 12))->draw_packet_callback = fnSOLB_DP000;
    ((SOLBPostProcessSlot *)(base + 16))->post_process_callback = fnSOLB_PO000;
    return 1;
}

extern void MEfGetActorMatrix(Matrix4 *matrix, u32 actor, u32 coord);
extern Vector4 *MMathApplyMatrix(Vector4 *out, Matrix4 *matrix, Vector4 *vector);
extern Matrix4 *MMathRotateMatrixX(Matrix4 *out, Matrix4 *matrix, float angle);
extern Matrix4 *MMathRotateMatrixY(Matrix4 *out, Matrix4 *matrix, float angle);
extern Matrix4 *MMathRotateMatrixZ(Matrix4 *out, Matrix4 *matrix, float angle);
extern float srsAtan2(float deltaX, float deltaZ);
extern Vector4 offset_0[SOLB_CONTROL_POINTS];

static void makePath(void *work)
{
    SolbState *state = (SolbState *)work;
    Vector4 direction;
    Matrix4 matrix;
    Vector4 worldOrigin;
    Matrix4 actorMatrix;
    Matrix4 pitchMatrix;
    Matrix4 yawMatrix;
    Matrix4 headingMatrix;
    float sweepStep;
    float heading;
    int i;

    MEfGetActorMatrix(&actorMatrix, state->actor, state->coord);
    MMathApplyMatrix(&worldOrigin, &actorMatrix, &state->localOffset);
    __asm__ __volatile__("lqc2 $vf1, 0(%0)" : : "r"(&worldOrigin) : "memory");
    __asm__ __volatile__("vmove.w $vf1, $vf0" : : : "memory");
    __asm__ __volatile__("sqc2 $vf1, 48(%0)" : : "r"(&actorMatrix) : "memory");

    __asm__ __volatile__(
        "lq $8, 0(%1)\n\t"
        "lq $9, 16(%1)\n\t"
        "lq $10, 32(%1)\n\t"
        "lq $11, 48(%1)\n\t"
        "sq $8, 0(%0)\n\t"
        "sq $9, 16(%0)\n\t"
        "sq $10, 32(%0)\n\t"
        "sq $11, 48(%0)"
        : : "r"(&matrix), "r"(&actorMatrix)
        : "$8", "$9", "$10", "$11", "memory");
    __asm__ __volatile__("sqc2 $vf0, 0(%0)" : : "r"(&direction) : "memory");
    __asm__ __volatile__("lqc2 $vf1, 0(%0)" : : "r"(&direction) : "memory");
    __asm__ __volatile__("vmove.w $vf1, $vf0" : : : "memory");
    __asm__ __volatile__("sqc2 $vf1, 48(%0)" : : "r"(&matrix) : "memory");

    direction.x = 1.0f;
    MMathApplyMatrix(&direction, &matrix, &direction);
    MMathRotateMatrixY(&matrix, (Matrix4 *)0, -state->actorAngle->yaw);
    MMathApplyMatrix(&direction, &matrix, &direction);

    heading = srsAtan2(direction.y, direction.x);
    sweepStep = (float)state->sweepDegrees * 0.017453292f;
    sweepStep /= 3.0f;
    MMathRotateMatrixX(&pitchMatrix, (Matrix4 *)0, 0.13962634f);
    MMathRotateMatrixY(&yawMatrix, (Matrix4 *)0, state->actorAngle->yaw);
    MMathRotateMatrixZ(&headingMatrix, (Matrix4 *)0, heading);

    for (i = 0; i < SOLB_CONTROL_POINTS; i++) {
        MMathApplyMatrix(&direction, &headingMatrix, &offset_0[i]);
        MMathApplyMatrix(&direction, &pitchMatrix, &direction);
        MMathApplyMatrix(&direction, &yawMatrix, &direction);
        __asm__ __volatile__("lqc2 $vf1, 0(%0)" : : "r"(&direction) : "memory");
        __asm__ __volatile__("lqc2 $vf2, 0(%0)" : : "r"(&worldOrigin) : "memory");
        __asm__ __volatile__("vadd.xyz $vf1xyz, $vf1xyz, $vf2xyz" : : : "memory");
        __asm__ __volatile__("sqc2 $vf1, 0(%0)" : : "r"(&state->control[i]) : "memory");
        MMathRotateMatrixY(&yawMatrix, &yawMatrix, sweepStep);
    }
}

/* The parameter routine writes both tangents for the current path segment. */
extern void MMathCalcHermitePrm(HermiteVector *tangent_start, HermiteVector *tangent_end,
                                const HermiteVector *point_previous, const HermiteVector *point_start,
                                const HermiteVector *point_end, const HermiteVector *point_next);

static void makeHermiteParam(int segment, void *work)
{
    SolbState *state = (SolbState *)work;
    HermiteVector *point_previous;
    HermiteVector *point_start;
    HermiteVector *point_end;
    HermiteVector *point_next;

    if (segment == 0) {
        point_previous = &state->control[0];
    } else {
        point_previous = &state->control[segment - 1];
    }
    point_start = &state->control[segment];
    point_end = &state->control[segment + 1];
    if (segment < 2) {
        point_next = &state->control[segment + 2];
    } else {
        point_next = point_end;
    }
    MMathCalcHermitePrm(&state->tangent_at_segment_start, &state->tangent_at_segment_end,
                        point_previous, point_start, point_end, point_next);
}

/* makeHermiteCoord: corrected tangent/endpoint attribution.
 *
 * Proof of the corrected names from the actual helper bodies:
 * - MMathCalcHermitePrm (main 0x002ef1d0) takes its two outputs in a0/a1 and
 *   stores (vf3-vf1)*k to (a0) and (vf4-vf2)*k to (a1). Its caller
 *   makeHermiteParams passes effect+0x330 in a0 and effect+0x340 in a1, so
 *   +0x330/+0x340 hold the computed segment TANGENTS, not endpoints.
 * - MMathCalcHermite (main 0x002ef160) loads its middle operand vectors from
 *   (a1)/(a2) and weights them with the parameter-derived tangent
 *   coefficients, while the (a3)/(t0) vectors take the endpoint weights; the
 *   caller passes +0x330/+0x340 in a1/a2 and the segment-indexed +0x2f0/+0x300
 *   pair in a3/t0. Hence a1/a2 are tangent_at_segment_start/end and a3/t0
 *   are control_at_segment_start/end. Start/end ordering within each pair is
 *   unchanged from the prior exact form; only the category names and this
 *   comment are corrected. Codegen is unchanged (LOCAL binding preserved).
 *
 * Those four addresses are now the SolbState members tangent_at_segment_start,
 * tangent_at_segment_end and control[segment]/control[segment + 1]; the effect
 * this is called with is the same record MEfCreate_SOLB sets up, which
 * fnSOLB_PR000 (0x00a37010) proves by passing it in a1 to both.
 */

static void makeHermiteCoord(float *destination, void *effect)
{
    SolbState *state = (SolbState *)effect;
    short segment_index = state->segment;
    short frame_index = state->frame;

    MMathCalcHermite(destination, (float)frame_index / (float)timetbl[segment_index],
                     &state->tangent_at_segment_start,
                     &state->tangent_at_segment_end,
                     &state->control[segment_index],
                     &state->control[segment_index + 1]);
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_solb", fnSOLB_PR000);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_solb", fnSOLB_DP000);

/* MEfObjDestroy (src/main/m_ef_obj.c) and sefHitEffect (src/main/sef.c) are
 * still asm in their defining TU; declared locally until published there. */
extern void MEfObjDestroy(void *self);
extern void sefHitEffect(void);

static void fnSOLB_PO000(void *self, void *work)
{
    SolbState *state = (SolbState *)work;

    state->age++;
    if (state->age == 0x17) {
        sefHitEffect();
    }
    if (state->age >= 0x35) {
        MEfObjDestroy(self);
    }
}
