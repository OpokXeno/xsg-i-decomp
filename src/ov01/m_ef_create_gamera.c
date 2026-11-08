/*
 * OV01 original TU 32: 0x00a387a8..0x00a39088 (7 functions)
 */
#include "common.h"
#include "ov01/m_ef_create.h"
#include "ov01/m_ef.h"
#include "shared.h"
#include "main/m_math.h"
#include "main/m_ef_obj.h"
#include "main/xgl_packet.h"

extern void MMathCalcHermitePrm(HermiteVector *tangent_start, HermiteVector *tangent_end,
                                const HermiteVector *point_prev,
                                const HermiteVector *point_start,
                                const HermiteVector *point_end,
                                const HermiteVector *point_next);

#define GAMERA_TRAIL_LENGTH 28
#define GAMERA_PATH_POINTS 4

/* The GAMERA record, at +0x20 of the pooled effect object. */
typedef struct GameraWork {
    unsigned char unmodeled_00[8];
    Actor *actor;        /* +0x08, the matrix source makePath reads */
    u32 coordinate;          /* +0x0c, its coordinate selector */
    unsigned char unmodeled_10[0x14];
    Actor *target_actor; /* +0x24, the coordinate source of target */
    u32 target_coordinate;   /* +0x28, its coordinate selector */
    unsigned char unmodeled_2c[0x44];
    short frame; /* +0x70 */
    unsigned char unmodeled_72[0x0e];
    Vector4 origin; /* +0x80 */
    Vector4 target; /* +0x90 */
    short trail_active[GAMERA_TRAIL_LENGTH]; /* +0xa0 */
    short trail_segment[GAMERA_TRAIL_LENGTH]; /* +0xd8 */
    Vector4 trail[GAMERA_TRAIL_LENGTH]; /* +0x110 */
    short segment; /* +0x2d0 */
    short path_frame; /* +0x2d2 */
    unsigned char unmodeled_2d4[12];
    HermiteVector control_points[GAMERA_PATH_POINTS]; /* +0x2e0 */
    float path_angle[GAMERA_PATH_POINTS]; /* +0x320 */
    HermiteVector tangent_start; /* +0x330 */
    HermiteVector tangent_end; /* +0x340 */
    MEfGsPacket packet; /* +0x350 */
} GameraWork;

typedef void (*GameraCallback)(void *self, GameraWork *work);

/* The pooled effect object: callbacks in its prefix, the record at +0x20. */
typedef struct GameraEffect {
    unsigned int : 32;
    GameraCallback process; /* +0x04 */
    unsigned int : 32;
    GameraCallback draw_packet; /* +0x0c */
    GameraCallback post_process; /* +0x10 */
    unsigned int : 32;
    unsigned int : 32;
    unsigned int : 32;
    GameraWork work; /* +0x20 */
} GameraEffect;

extern void MGsGPInit(MEfGsPacket *packet, void *address, int size);
static void makePath(GameraWork *state);
static void makeHermiteParams(int segment, GameraWork *state);
static void fnGAMERA_PR000(void *self, GameraWork *work);
static void fnGAMERA_DP000(void *self, GameraWork *work);
static void fnGAMERA_PO000(void *self, GameraWork *work);

/* Copies one sixteen-byte-aligned vector whole through the original's $8. */
#define GAMERA_COPY_VECTOR(destination, source)                          \
    __asm__ __volatile__("lq $8, 0(%1)\n\tsq $8, 0(%0)"                  \
                         : : "r"(destination), "r"(source) : "$8", "memory")

/* Initializes a GAMERA effect object: records the origin and the target, clears the trail, builds the path and the GS packet, and installs the callbacks. */
int MEfCreate_GAMERA(MEfObjRecord *self)
{
    GameraEffect *effect = (GameraEffect *)self;
    GameraWork *state = &effect->work;
    Vector4 *trail = state->trail;
    Vector4 *origin = &state->origin;

    state->frame = 0;
    MEfGetActorCoord(origin, state->actor, state->coordinate);
    MEfGetActorCoord(&state->target, state->target_actor, state->target_coordinate);
    memset(state->trail_active, 0, sizeof(state->trail_active));
    memset(state->trail_segment, 0, sizeof(state->trail_segment));
    memset(trail, 0, sizeof(state->trail));
    GAMERA_COPY_VECTOR(trail, origin);
    state->segment = 0;
    state->path_frame = 0;
    makePath(state);
    makeHermiteParams(0, state);
    MGsGPInit(&state->packet, 0, 0);
    effect->process = fnGAMERA_PR000;
    effect->draw_packet = fnGAMERA_DP000;
    effect->post_process = fnGAMERA_PO000;
    return 1;
}

extern float MMathMakeRandom(void);
extern float MMathMakeRandom2PI(void);
extern unsigned short xglSRand(void);

/* Per-point forward distance and radius of the path. */
static const float disttbl_0_00A517D0[4];
static const float rtbl_1_00A517E0[4];

/* Start angles inside (210, 330) degrees are rejected. */
#define GAMERA_START_ANGLE_MIN 3.6651914f
#define GAMERA_START_ANGLE_MAX 5.7595863f
#define GAMERA_ANGLE_STEP 0.20943952f
#define GAMERA_ANGLE_STEP_GROWTH 1.3f
#define GAMERA_RADIUS_SCALE_MIN 0.05f

/*
 * Lays out the path control points on a spiral that starts at a random angle outside
 * 210 to 330 degrees and turns either way at random.
 */
static void makePath(GameraWork *state)
{
    Matrix4 matrix;
    Vector4 point;
    float angle;
    float step;
    float scale;
    float cosine;
    float sine;
    int i;

    MEfGetActorMatrix(&matrix, state->actor, state->coordinate);
    do {
        angle = MMathMakeRandom2PI();
    } while (!(angle < GAMERA_START_ANGLE_MIN || angle > GAMERA_START_ANGLE_MAX));

    step = GAMERA_ANGLE_STEP;
    if ((xglSRand() & 1) == 0) {
        step = -GAMERA_ANGLE_STEP;
    }
    scale = MMathMakeRandom() + GAMERA_RADIUS_SCALE_MIN;

    for (i = 0; i < GAMERA_PATH_POINTS; i++) {
        __asm__ __volatile__("sqc2 $vf0, 0(%0)" : : "r"(&point) : "memory");
        __asm__ __volatile__(
            "mfc1 $8,%1\n\t"
            "qmtc2.ni $8,$vf4\n\t"
            "vcallms 0xe8\n\t"
            "qmfc2.i $8,$vf1\n\t"
            "mtc1 $8,%0\n\t"
            : "=f"(cosine)
            : "f"(angle)
            : "$8", "memory");
        point.x = cosine * *(rtbl_1_00A517E0 + i) * scale;
        __asm__ __volatile__(
            "mfc1 $8,%1\n\t"
            "qmtc2.ni $8,$vf4\n\t"
            "vcallms 0x20\n\t"
            "qmfc2.i $8,$vf1\n\t"
            "mtc1 $8,%0\n\t"
            : "=f"(sine)
            : "f"(angle)
            : "$8", "memory");
        point.y = sine * *(rtbl_1_00A517E0 + i) * scale;
        point.z = *(disttbl_0_00A517D0 + i);
        MMathApplyMatrix((Vector4 *)&state->control_points[i], &matrix, &point);
        state->path_angle[i] = angle;
        angle += step;
        step *= GAMERA_ANGLE_STEP_GROWTH;
    }
}

/* Computes the tangents of the Hermite segment that starts at control point segment. */
static void makeHermiteParams(int segment, GameraWork *state)
{
    const HermiteVector *point_prev;
    const HermiteVector *point_end;
    const HermiteVector *point_next;

    if (segment == 0) {
        point_prev = &state->control_points[0];
    } else {
        point_prev = &state->control_points[segment - 1];
    }

    point_end = &state->control_points[segment + 1];
    point_next = &state->control_points[segment + 2];
    if (segment >= 2) {
        point_next = point_end;
    }

    MMathCalcHermitePrm(&state->tangent_start, &state->tangent_end,
                        point_prev, &state->control_points[segment],
                        point_end, point_next);
}

/* Evaluates the current Hermite position, at the path frame, into destination. */
static void makeHermiteCoord(float *destination, GameraWork *state)
{
    short segment = state->segment;
    short frame = state->path_frame;

    MMathCalcHermite(destination, (float)frame * 0.125f,
                     &state->tangent_start,
                     &state->tangent_end,
                     &state->control_points[segment],
                     &state->control_points[segment + 1]);
}

/* Frames per path segment (makeHermiteCoord steps by 1/8) and the frames
 * during which the head travels the path and records the trail. */
#define GAMERA_SEGMENT_FRAMES 8
#define GAMERA_TRAVEL_FRAMES 0x18

static void fnGAMERA_PR000(void *self, GameraWork *work)
{
    GameraWork *state = work;
    Vector4 *head;
    int i;

    for (i = GAMERA_TRAIL_LENGTH - 2; i >= 0; i--) {
        state->trail_active[i + 1] = state->trail_active[i];
        state->trail_segment[i + 1] = state->trail_segment[i];
        GAMERA_COPY_VECTOR(&state->trail[i + 1], &state->trail[i]);
    }
    state->trail_active[0] = 0;
    head = &state->trail[0];
    __asm__ __volatile__("sqc2 $vf0, 0(%0)" : : "r"(head) : "memory");

    if (state->frame < GAMERA_TRAVEL_FRAMES) {
        state->trail_active[0] = 1;
        state->trail_segment[0] = state->segment;
        makeHermiteCoord(&head->x, state);
        if (++state->path_frame >= GAMERA_SEGMENT_FRAMES) {
            state->path_frame = 0;
            makeHermiteParams(++state->segment, state);
        }
    }
}

#define GAMERA_DRAWN_SEGMENTS 27
#define GAMERA_TRAIL_WIDTH_Y -0.005f
#define GAMERA_TRAIL_WIDTH_X 0.05f

extern void MGsGPTerm(MEfGsPacket *packet);
extern MEfObjCamParams mefCamParams;
static const MEfGsQword coltbl_2_00A517F0[3];

/* Writes the GIF tag that opens the packet's next primitive and returns
 * the first data quadword after it. */
static inline MEfGsQword *gameraOpenTag(MEfGsPacket *packet, int pre, int prim,
                                           int reg_count, u64 registers)
{
    MEfGsQword *qword = packet->current;

    qword->gif.tag = GIF_TAG_EOP | ((u64)pre << 46) | ((u64)prim << 47) | ((u64)reg_count << 60);
    qword->gif.registers = registers;
    packet->reg_count = reg_count;
    return qword + 1;
}

/* Writes one A+D register setting. */
static inline void gameraSetRegister(MEfGsQword *qword, unsigned char address, u64 data)
{
    qword->ad.data = data;
    qword->ad.address = address;
}

/* Stores NLOOP into the open GIF tag and moves past its data. */
static inline void gameraCloseTag(MEfGsPacket *packet, int nloop)
{
    int size;

    size = packet->reg_count * nloop + 1;
    packet->current->nloop |= nloop;
    packet->current += size;
    packet->count += size;
}

/* The two trail offsets of a strip side, overlaid on the offset vectors so a
 * line can index their components. */
typedef struct GameraOffsets {
    float inner[4];
    float outer[4];
} GameraOffsets;

/*
 * Draws the trail as strips with both sides per offset axis. Each vertex is offset across
 * the trail, transformed by its segment's angle, and clipped against the screen.
 */
static void fnGAMERA_DP000(void *self, GameraWork *work)
{
    GameraWork *state = work;
    MEfGsQword colour[2];
    Matrix4 matrix;
    Vector4 angles;
    Vector4 offset[2];
    MEfGsQword vertex[2];
    const void *screen;
    const void *view;
    MEfGsPacket *packet;
    MEfGsPacket *tail;
    MEfGsQword *setting;
    MEfGsQword *qword;
    int clip;
    int drawn;
    int line;
    int side;
    MEfGsPacket *opener;
    int segment;
    GameraOffsets *slots;
    Vector4 *outer_offset;
    float inner;
    float outer;

    screen = mefCamParams.screen;
    view = mefCamParams.matrix;
    packet = &state->packet;
    MGsGPInit(packet, 0, 0);
    /* Write the GS state for the strips: test, depth and additive alpha. */
    setting = packet->current;
    setting[0].gif.tag = MEF_GS_SETUP_TAG;
    setting[0].gif.registers = MEF_GS_SETUP_REGS;
    packet->reg_count = 1;
    gameraSetRegister(&setting[1], SCE_GS_TEST_1, MEF_GS_TEST_1_ALPHA_GREATER_ZGEQUAL);
    gameraSetRegister(&setting[2], SCE_GS_ZBUF_1, MEF_GS_ZBUF_1_PSMZ24_NO_DEPTH_WRITE);
    gameraSetRegister(&setting[3], SCE_GS_ALPHA_1, MEF_GS_ALPHA_1_ADDITIVE);
    gameraCloseTag(packet, 3);
    line = 0;
    outer_offset = &offset[1];
    opener = packet;
    tail = packet;

    slots = (GameraOffsets *)offset;
    /* Two passes, one per offset axis; each pass draws both sides of the strip. */
    for (; line < 2; line++) {
        __asm__ __volatile__("sqc2 $vf0, 0(%0)" : : "r"(&offset[0]) : "memory");
        __asm__ __volatile__("sqc2 $vf0, 0(%0)" : : "r"(outer_offset) : "memory");
        for (side = -1; side < 2; side += 2) {
            drawn = 0;
            inner = (float)side * GAMERA_TRAIL_WIDTH_Y;
            outer = (float)side * GAMERA_TRAIL_WIDTH_X;
            qword = gameraOpenTag(opener, 1, MEF_GS_STRIP_PRIM, 4, MEF_GS_STRIP_REGS);
            packet = &state->packet;
            clip = 0;
            for (segment = 0; segment < GAMERA_DRAWN_SEGMENTS; segment++) {
                if (state->trail_active[segment] == 0) {
                    continue;
                }
                colour[0].whole = coltbl_2_00A517F0[0].whole;
                colour[1].whole = coltbl_2_00A517F0[1].whole;
                if (segment == 0 || segment == GAMERA_DRAWN_SEGMENTS - 1) {
                    colour[0].rgba.a = colour[1].rgba.a = 0;
                    slots->inner[line] = slots->outer[line] = 0.0f;
                } else {
                    slots->inner[line] = inner;
                    slots->outer[line] = outer;
                }
                MEfCalcAngle(&angles, &state->trail[segment],
                             &state->trail[segment + 1]);
                angles.z = state->path_angle[state->trail_segment[segment]];
                MMathRotateMatrixYXZ(&matrix, 0, &angles);
                /* Use the trail point as the translation of the strip matrix. */
                __asm__ __volatile__(
                    "lqc2 $vf1,0(%1)\n\t"
                    "vmove.w $vf1w,$vf0w\n\t"
                    "sqc2 $vf1,48(%0)\n\t"
                    :
                    : "r"(matrix), "r"(&state->trail[segment])
                    : "memory");
                MMathApplyMatrix(&angles, &matrix, &offset[0]);
                if (MMathRotTransPersClip(&vertex[0].vector, screen, view, &angles)) {
                    clip = 2;
                } else if (clip != 0) {
                    clip--;
                    vertex[0].xyz.adc = MEF_GS_XYZ_ADC;
                }
                MMathApplyMatrix(&angles, &matrix, outer_offset);
                if (MMathRotTransPersClip(&vertex[1].vector, screen, view, &angles)) {
                    clip = 2;
                } else if (clip != 0) {
                    clip--;
                    vertex[1].xyz.adc = MEF_GS_XYZ_ADC;
                }
                drawn++;
                qword[0].whole = colour[0].whole;
                qword[1].whole = vertex[0].whole;
                qword[2].whole = colour[1].whole;
                qword[3].whole = vertex[1].whole;
                qword += 4;
            }
            if (drawn > 0) {
                gameraCloseTag(tail, drawn);
            }
        }
    }
    MGsGPTerm(tail);
}

/* MEfObjDestroy is defined by src/main/m_ef_obj.c; sefHitEffect
 * is defined by src/main/sef.c. */
extern void sefHitEffect(void);

/* work's layout is unresolved beyond +0x70: the per-frame lifetime counter
 * this function advances, firing sefHitEffect on frame 0x18 and expiring
 * through MEfObjDestroy from frame 0x33 on. */
#define GAMERA_HIT_FRAME 0x18
#define GAMERA_LIFETIME 0x33

static void fnGAMERA_PO000(void *self, GameraWork *work)
{
    GameraWork *state = work;

    state->frame++;
    if (state->frame == GAMERA_HIT_FRAME) {
        sefHitEffect();
    }
    if (state->frame >= GAMERA_LIFETIME) {
        MEfObjDestroy(self);
    }
}



static const float disttbl_0_00A517D0[4] = { 0.0f, 2.0f, 9.0f, 20.0f };

static const float rtbl_1_00A517E0[4] = { 0.0f, 5.0f, 7.0f, 8.0f };

static const MEfGsQword coltbl_2_00A517F0[3] = {
    { .rgba = { 0x10, 0x30, 0xa0, 0xff } },
    { .rgba = { 0x0, 0x10, 0x80, 0xff } },
    { .rgba = { 0x20, 0x20, 0x0, 0xff } }
};
