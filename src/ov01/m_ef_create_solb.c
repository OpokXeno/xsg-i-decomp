/*
 * OV01 original TU 29: 0x00a36be8..0x00a37ab8 (7 functions)
 */
#include "common.h"
#include "ov01/m_ef_create.h"
#include "shared.h"
#include "main/xgl_packet.h"
#include "main/m_math.h"
#include "main/m_ef_obj.h"
#include "vu0_macro.h"
#include "m_ef_create_solb.h"

extern void *memset(void *destination, int value, unsigned int count);
static void makePath(SolbState *state);
static void makeHermiteParam(int mode, SolbState *state);
extern void MGsGPInit(void *work, int flags, int mode);
static void fnSOLB_PR000(void *self, SolbState *state);
static void fnSOLB_DP000(void *self, SolbState *state);
static void fnSOLB_PO000(void *self, SolbState *state);

/*
 * Initializes an SOLB effect object: clears the trail, builds the path and the
 * first segment's tangents, sets up the GS packet and installs the process,
 * draw and post-process callbacks. Returns 1.
 */
int MEfCreate_SOLB(MEfObjRecord *work)
{
    SolbObject *object = (SolbObject *)work;
    SolbState *state = &object->state;

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
    MGsGPInit(&state->packet, 0, 0);
    object->process_callback = fnSOLB_PR000;
    object->draw_packet_callback = fnSOLB_DP000;
    object->post_process_callback = fnSOLB_PO000;
    return 1;
}

extern Vector4 *MMathApplyMatrix(Vector4 *out, Matrix4 *matrix, const Vector4 *vector);
extern Matrix4 *MMathRotateMatrixX(Matrix4 *out, Matrix4 *matrix, float angle);
extern Matrix4 *MMathRotateMatrixY(Matrix4 *out, Matrix4 *matrix, float angle);
extern Matrix4 *MMathRotateMatrixZ(Matrix4 *out, Matrix4 *matrix, float angle);
extern float srsAtan2(float deltaX, float deltaZ);
static const int timetbl[4] = { 5, 6, 12, 23 };
/*
 * Fills state->control with the four path points. Each offset is rotated by the
 * heading and a fixed pitch, turned by the actor yaw plus a step of sweepDegrees / 3
 * per point, then moved to the actor's world origin.
 */
static void makePath(SolbState *state)
{
    static const Vector4 offset[SOLB_CONTROL_POINTS] = {
        { 0.0f, 0.0f, 0.0f, 1.0f },
        { 1.7f, 0.0f, 0.5f, 1.0f },
        { 2.0f, 0.0f, 3.6f, 1.0f },
        { 2.0f, 0.0f, 20.0f, 1.0f }
    };
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
    MMathRotateMatrixY(&matrix, 0, -state->actorAngle->yaw);
    MMathApplyMatrix(&direction, &matrix, &direction);

    heading = srsAtan2(direction.y, direction.x);
    sweepStep = (float)state->sweepDegrees * 0.017453292f;
    sweepStep /= 3.0f;
    MMathRotateMatrixX(&pitchMatrix, 0, 0.13962634f);
    MMathRotateMatrixY(&yawMatrix, 0, state->actorAngle->yaw);
    MMathRotateMatrixZ(&headingMatrix, 0, heading);

    for (i = 0; i < SOLB_CONTROL_POINTS; i++) {
        MMathApplyMatrix(&direction, &headingMatrix, &offset[i]);
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

/* Computes the start and end tangents of a path segment from its neighbouring control points. */
static void makeHermiteParam(int segment, SolbState *state)
{
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

/*
 * Writes the curve point for the current frame of the current segment to
 * destination. The tangents are the segment tangents; the control points are
 * the segment's start and end points.
 */
static void makeHermiteCoord(float *destination, SolbState *state)
{
    short segment_index = state->segment;
    short frame_index = state->frame;

    MMathCalcHermite(destination, (float)frame_index / (float)timetbl[segment_index],
                     &state->tangent_at_segment_start,
                     &state->tangent_at_segment_end,
                     &state->control[segment_index],
                     &state->control[segment_index + 1]);
}

/* New trail entries are only emitted while the effect is still travelling. */
#define SOLB_TRAVEL_FRAMES 0x17

/* Frame of the object's hit effect and the frame its lifetime expires on
 * (fnSOLB_PO000 below). */
#define SOLB_HIT_FRAME 0x17
#define SOLB_LIFETIME  0x35

/*
 * Process callback: shifts the trail back by one entry, then, while the effect
 * is travelling, writes a new head point and advances the frame and segment.
 */
static void fnSOLB_PR000(void *self, SolbState *state)
{
    Vector4 *head;
    int i;

    for (i = SOLB_TRAIL_LENGTH - 2; i >= 0; i--) {
        state->active[i + 1] = state->active[i];
        state->birth_age[i + 1] = state->birth_age[i];
        __asm__ __volatile__("lq $8, 0(%1)\n\tsq $8, 0(%0)"
                             : : "r"(&state->trail[i + 1]), "r"(&state->trail[i]) : "$8", "memory");
    }
    state->active[0] = 0;
    state->birth_age[0] = 0;
    head = &state->trail[0];
    __asm__ __volatile__("sqc2 $vf0, 0(%0)" : : "r"(head) : "memory");

    if (state->age < SOLB_TRAVEL_FRAMES) {
        state->active[0] = 1;
        state->birth_age[0] = state->age;
        makeHermiteCoord(&head->x, state);
        if (++state->frame >= timetbl[state->segment]) {
            state->frame = 0;
            makeHermiteParam(++state->segment, state);
        }
    }
}

static Quadword coltbl_1_00A43780[3];
extern Vector4 *MEfCalcAngle(Vector4 *destination, const Vector4 *from, const Vector4 *to);
extern float MMathMakeRandom2PI(void);
extern int MMathRotTransPersClip(Vector4 *destination, const StudioCamera *camera,
                                 Matrix4 *matrix, const Vector4 *point);
extern void MGsGPTerm(MEfGsPacket *packet);
extern MEfObjCamParams mefCamParams;

#define SOLB_RIBBON_PRIM 0x4C
#define SOLB_LINE_PRIM 0x4A
#define SOLB_RIBBON_REGS 0x551
#define SOLB_LINE_REGS 0x51

#define SOLB_FIBER_COUNT 6
#define SOLB_TRAIL_DRAWN 30

static inline MEfGsQword *solbOpenTag(MEfGsPacket *packet, int pre, int prim, int reg_count,
                                      u64 registers)
{
    MEfGsQword *qword = packet->current;

    qword->gif.tag = GIF_TAG_EOP | ((u64)pre << 46) | ((u64)prim << 47) | ((u64)reg_count << 60);
    qword->gif.registers = registers;
    packet->reg_count = reg_count;
    return qword + 1;
}

static inline void solbSetRegister(MEfGsQword *qword, unsigned char address, u64 data)
{
    qword->ad.data = data;
    qword->ad.address = address;
}

static inline void solbCloseTag(MEfGsPacket *packet, int nloop)
{
    int size;

    size = packet->reg_count * nloop + 1;
    packet->current->nloop |= nloop;
    packet->current += size;
    packet->count += size;
}

/*
 * Draw-packet callback: builds the GS packet for the trail, with two ribbon
 * passes, six fiber passes and two more fiber passes with random angles.
 */
static void fnSOLB_DP000(void *self, SolbState *state)
{
    MEfGsQword color;
    Matrix4 matrix;
    Vector4 aim;
    Vector4 lead;
    MEfGsQword vertex[2];
    MEfGsPacket *packet;
    Vector4 *point;
    StudioCamera *screen;
    Matrix4 *view;
    MEfGsQword *qword;
    float width;
    int *adc;
    int alpha;
    int clip;
    int drawn;
    int pass;
    int i;
    int j;

    screen = mefCamParams.screen;
    view = mefCamParams.matrix;
    point = (Vector4 *)&state->packet;
    MGsGPInit(point, 0, 0);
    /* Render state for the ribbons: alpha test, no depth write, additive blend. */
    {
    MEfGsQword *setting = ((MEfGsPacket *)point)->current;
    setting[0].gif.tag = MEF_GS_SETUP_TAG;
    setting[0].gif.registers = MEF_GS_SETUP_REGS;
    ((MEfGsPacket *)point)->reg_count = 1;
    qword = &setting[1];
    solbSetRegister(qword, SCE_GS_TEST_1, MEF_GS_TEST_1_ALPHA_GREATER_ZGEQUAL);
    solbSetRegister(&setting[2], SCE_GS_ZBUF_1, MEF_GS_ZBUF_1_PSMZ24_NO_DEPTH_WRITE);
    solbSetRegister(&setting[3], SCE_GS_ALPHA_1, MEF_GS_ALPHA_1_ADDITIVE);
    solbCloseTag((MEfGsPacket *)point, 3);
    }
    point = *(Vector4 **)point;
    adc = &vertex[1].xyz.adc;

    /* Two ribbon passes: the lead point is offset by +width, then by -width. */
    color.whole = coltbl_1_00A43780[0];
    for (pass = 0; pass < 2; pass++) {
        clip = 0;
        drawn = 0;
        qword = solbOpenTag(&state->packet, 1, SOLB_RIBBON_PRIM, 3, SOLB_RIBBON_REGS);
        {
            float x = point->x;

            point->x = x;
        }
        Vu0SetOrigin(&lead);
        for (i = 0; i < SOLB_TRAIL_DRAWN; i++) {
            if (state->active[i] == 0) {
                continue;
            }
            if (i == SOLB_TRAIL_DRAWN - 1) {
                color.rgba.a = 0;
            } else {
                alpha = 256 - state->birth_age[i] * 11;
                if (alpha < 0) {
                    alpha = 0;
                }
                color.rgba.a = alpha;
            }
            if (i != 0 && i != SOLB_TRAIL_DRAWN - 1) {
                width = 0.02f;
            } else {
                width = 0.0f;
            }
            point = &state->trail[i];
            MEfCalcAngle(&aim, point, &state->trail[i + 1]);
            MMathRotateMatrixYXZ(&matrix, 0, &aim);
            __asm__ __volatile__(
                "lqc2 $vf1, 0(%1)\n"
                "vmove.w $vf1w, $vf0w\n"
                "sqc2 $vf1, 48(%0)\n"
                : : "r"(&matrix), "r"(point) : "memory");

            (&lead.x)[pass] = width;
            MMathApplyMatrix(&aim, &matrix, &lead);
            if (MMathRotTransPersClip(&vertex[0].vector, screen, view, &aim)) {
                clip = 2;
            } else if (clip != 0) {
                clip--;
                vertex[0].xyz.adc = MEF_GS_XYZ_ADC;
            }
            (&lead.x)[pass] = -width;
            MMathApplyMatrix(&aim, &matrix, &lead);
            if (MMathRotTransPersClip(&vertex[1].vector, screen, view, &aim)) {
                clip = 2;
            } else if (clip != 0) {
                clip--;
                *adc = MEF_GS_XYZ_ADC;
            }
            drawn++;
            qword[0].whole = color.whole;
            qword[1].whole = vertex[0].whole;
            qword[2].whole = vertex[1].whole;
            qword += 3;
        }
        packet = &state->packet;
        if (drawn > 0) {
            solbCloseTag(packet, drawn);
        }
    }

    {
    MEfGsQword *setting = packet->current;
    setting[0].gif.tag = MEF_GS_SETUP_TAG;
    setting[0].gif.registers = MEF_GS_SETUP_REGS;
    packet->reg_count = 1;
    qword = &setting[1];
    solbSetRegister(qword, SCE_GS_TEST_1, MEF_GS_TEST_1_ALPHA_GREATER_ZGEQUAL);
    solbSetRegister(&setting[2], SCE_GS_ZBUF_1, MEF_GS_ZBUF_1_PSMZ24_NO_DEPTH_WRITE);
    solbSetRegister(&setting[3], SCE_GS_ALPHA_1, MEF_GS_ALPHA_1_ADDITIVE);
    solbCloseTag(packet, 3);
    }
    /* Six fiber passes: each point is offset around a circle, with the angle set by the point index and the pass. */
    color.whole = coltbl_1_00A43780[1];
    for (pass = 0; pass < SOLB_FIBER_COUNT; pass++) {
        clip = 0;
        drawn = 0;
        qword = solbOpenTag(&state->packet, 1, SOLB_LINE_PRIM, 2, SOLB_LINE_REGS);
        for (j = 0; j < SOLB_TRAIL_DRAWN; j++) {
            if (state->active[j] == 0) {
                continue;
            }
            if (j == SOLB_TRAIL_DRAWN - 1) {
                color.rgba.a = 0;
            } else {
                color.rgba.a = 256 - state->birth_age[j] * 11;
                if (color.rgba.a < 48) {
                    color.rgba.a = 48;
                }
            }
            point = &state->trail[j];
            MEfCalcAngle(&aim, point, &state->trail[j + 1]);
            MMathRotateMatrixYXZ(&matrix, 0, &aim);
            __asm__ __volatile__(
                "lqc2 $vf1, 0(%1)\n"
                "vmove.w $vf1w, $vf0w\n"
                "sqc2 $vf1, 48(%0)\n"
                : : "r"(&matrix), "r"(point) : "memory");
            point = &state->trail[j + 2];
            {
                float x = point->x;

                point->x = x;
            }
            Vu0SetOrigin(&aim);
            if (j != 0 && j < SOLB_TRAIL_DRAWN - 1) {
                width = (float)j * 0.2094395f;
                width = width + (float)pass * 1.0471975f;
                Vu0Cos(aim.x, width);
                aim.x = aim.x * 0.1f;
                Vu0Sin(aim.y, width);
                aim.y = aim.y * 0.1f;
            }
            MMathApplyMatrix(&aim, &matrix, &aim);
            if (MMathRotTransPersClip(&vertex[0].vector, screen, view, &aim)) {
                clip = 1;
            } else if (clip != 0) {
                clip--;
                vertex[0].xyz.adc = MEF_GS_XYZ_ADC;
            }
            drawn++;
            qword[0].whole = color.whole;
            qword[1].whole = vertex[0].whole;
            qword += 2;
        }
        if (drawn > 0) {
            solbCloseTag(packet, drawn);
        }
    }

    {
    MEfGsQword *setting = packet->current;
    setting[0].gif.tag = MEF_GS_SETUP_TAG;
    setting[0].gif.registers = MEF_GS_SETUP_REGS;
    packet->reg_count = 1;
    qword = &setting[1];
    solbSetRegister(qword, SCE_GS_TEST_1, MEF_GS_TEST_1_ALPHA_GREATER_ZGEQUAL);
    solbSetRegister(&setting[2], SCE_GS_ZBUF_1, MEF_GS_ZBUF_1_PSMZ24_NO_DEPTH_WRITE);
    solbSetRegister(&setting[3], SCE_GS_ALPHA_1, MEF_GS_ALPHA_1_ADDITIVE);
    solbCloseTag(packet, 3);
    }
    /* Two more fiber passes, each point offset by a random angle on a circle. */
    color.whole = coltbl_1_00A43780[1];
    for (pass = 0; pass < 2; pass++) {
        drawn = 0;
        clip = 0;
        qword = solbOpenTag(&state->packet, 1, SOLB_LINE_PRIM, 2, SOLB_LINE_REGS);
        for (j = 0; j < SOLB_TRAIL_DRAWN; j++) {
            if (state->active[j] == 0) {
                continue;
            }
            if (j == SOLB_TRAIL_DRAWN - 1) {
                color.rgba.a = 0;
            } else {
                color.rgba.a = 256 - state->birth_age[j] * 11;
                if (color.rgba.a < 48) {
                    color.rgba.a = 48;
                }
            }
            point = &state->trail[j];
            MEfCalcAngle(&aim, point, &state->trail[j + 1]);
            MMathRotateMatrixYXZ(&matrix, 0, &aim);
            __asm__ __volatile__(
                "lqc2 $vf1, 0(%1)\n"
                "vmove.w $vf1w, $vf0w\n"
                "sqc2 $vf1, 48(%0)\n"
                : : "r"(&matrix), "r"(point) : "memory");
            point = &state->trail[j + 2];
            {
                float x = point->x;

                point->x = x;
            }
            {
                float y = point->y;

                point->y = y;
            }
            {
                float z = point->z;

                point->z = z;
            }
            Vu0SetOrigin(&aim);
            if (j != 0 && j < SOLB_TRAIL_DRAWN - 1) {
                width = MMathMakeRandom2PI();
                Vu0Cos(aim.x, width);
                aim.x = aim.x * 0.25f;
                Vu0Sin(aim.y, width);
                aim.y = aim.y * 0.25f;
            }
            MMathApplyMatrix(&aim, &matrix, &aim);
            if (MMathRotTransPersClip(&vertex[0].vector, screen, view, &aim)) {
                clip = 1;
            } else if (clip != 0) {
                clip--;
                vertex[0].xyz.adc = MEF_GS_XYZ_ADC;
            }
            drawn++;
            qword[0].whole = color.whole;
            qword[1].whole = vertex[0].whole;
            qword += 2;
        }
        if (drawn > 0) {
            solbCloseTag(packet, drawn);
        }
    }
    MGsGPTerm(packet);
    drawn = *(int *)point;
    drawn++;
}

/* MEfObjDestroy is defined by src/main/m_ef_obj.c; sefHitEffect
 * is defined by src/main/sef.c. */
extern void sefHitEffect(void);

static void fnSOLB_PO000(void *self, SolbState *state)
{
    state->age++;
    if (state->age == SOLB_HIT_FRAME) {
        sefHitEffect();
    }
    if (state->age >= SOLB_LIFETIME) {
        MEfObjDestroy(self);
    }
}



static Quadword coltbl_1_00A43780[3] = {
    0xff000000a0000000a0000000a0,
    0xff000000a00000002000000000,
    0xff0000002000000080000000c0
};
