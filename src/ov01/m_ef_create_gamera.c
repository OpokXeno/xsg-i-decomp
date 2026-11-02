/*
 * OV01 original TU 32: 0x00a387a8..0x00a39088 (7 functions)
 */
#include "common.h"
#include "ov01/m_ef_create.h"
#include "ov01/m_ef.h"
#include "shared.h"
#include "main/m_math.h"

extern void MMathCalcHermitePrm(HermiteVector *tangent_start, HermiteVector *tangent_end,
                                const HermiteVector *point_prev,
                                const HermiteVector *point_start,
                                const HermiteVector *point_end,
                                const HermiteVector *point_next);

#define GAMERA_TRAIL_LENGTH 28
#define GAMERA_PATH_POINTS 4

typedef unsigned int GameraQuadword __attribute__((mode(TI)));

/* One quadword of a GIF packet: a GIF tag, an A+D register write, or a
 * packed RGBAQ / XYZ2 register value. */
typedef union GameraGsQword {
    GameraQuadword whole;
    unsigned short nloop; /* low halfword of a GIF tag: NLOOP and EOP */
    struct {
        u64 tag;
        u64 registers;
    } gif;
    struct {
        u64 data;
        unsigned char address;
    } ad;
    struct {
        int r, g, b, a;
    } rgba;
    struct {
        int x, y, z, adc;
    } xyz;
} GameraGsQword;

/* The GS packet MGsGPInit prepares and MGsGPTerm flushes (ov01 m_gs.c). */
typedef struct GameraGsPacket {
    void *data;
    int capacity;
    GameraGsQword *current; /* the open GIF tag */
    int count;              /* queued quadwords */
    int regCount;           /* NREG of the open GIF tag */
} GameraGsPacket;

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
    HermiteVector trail[GAMERA_TRAIL_LENGTH]; /* +0x110 */
    short segment; /* +0x2d0 */
    short path_frame; /* +0x2d2 */
    unsigned char unmodeled_2d4[12];
    HermiteVector control_points[GAMERA_PATH_POINTS]; /* +0x2e0 */
    float path_angle[GAMERA_PATH_POINTS]; /* +0x320 */
    HermiteVector tangent_start; /* +0x330 */
    HermiteVector tangent_end; /* +0x340 */
    GameraGsPacket packet; /* +0x350 */
} GameraWork;

typedef void (*GameraCallback)(void *self, void *work);

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

extern void MGsGPInit(GameraGsPacket *packet, void *address, int size);
static void makePath(GameraWork *state);
static void makeHermiteParams(int segment, GameraWork *state);
static void fnGAMERA_PR000(void *self, void *work);
static void fnGAMERA_DP000(void *self, void *work);
static void fnGAMERA_PO000(void *self, void *work);

/* Copies one sixteen-byte-aligned vector whole through the original's $8. */
#define GAMERA_COPY_VECTOR(destination, source)                          \
    __asm__ __volatile__("lq $8, 0(%1)\n\tsq $8, 0(%0)"                  \
                         : : "r"(destination), "r"(source) : "$8", "memory")

int MEfCreate_GAMERA(MEfObjRecord *self)
{
    GameraEffect *effect = (GameraEffect *)self;
    GameraWork *state = &effect->work;
    HermiteVector *trail = state->trail;
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

extern Vector4 *MMathApplyMatrix(Vector4 *out, Matrix4 *matrix, const Vector4 *vector);
extern float MMathMakeRandom(void);
extern float MMathMakeRandom2PI(void);
extern unsigned short xglSRand(void);

/* Per-point forward distance and radius of the path (splat-named rodata). */
extern float disttbl_0_00A517D0[GAMERA_PATH_POINTS];
extern float rtbl_1_00A517E0[GAMERA_PATH_POINTS];

/* Start angles inside (210, 330) degrees are rejected. */
#define GAMERA_START_ANGLE_MIN 3.6651914f
#define GAMERA_START_ANGLE_MAX 5.7595863f
#define GAMERA_ANGLE_STEP 0.20943952f
#define GAMERA_ANGLE_STEP_GROWTH 1.3f
#define GAMERA_RADIUS_SCALE_MIN 0.05f

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
        point.x = cosine * rtbl_1_00A517E0[i] * scale;
        __asm__ __volatile__(
            "mfc1 $8,%1\n\t"
            "qmtc2.ni $8,$vf4\n\t"
            "vcallms 0x20\n\t"
            "qmfc2.i $8,$vf1\n\t"
            "mtc1 $8,%0\n\t"
            : "=f"(sine)
            : "f"(angle)
            : "$8", "memory");
        point.y = sine * rtbl_1_00A517E0[i] * scale;
        point.z = disttbl_0_00A517D0[i];
        MMathApplyMatrix((Vector4 *)&state->control_points[i], &matrix, &point);
        state->path_angle[i] = angle;
        angle += step;
        step *= GAMERA_ANGLE_STEP_GROWTH;
    }
}

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

static void makeHermiteCoord(float *destination, void *effect)
{
    unsigned char *base = (unsigned char *)effect;
    short segment = *(short *)(base + 720);
    short frame = *(short *)(base + 722);

    MMathCalcHermite(destination, (float)frame * 0.125f,
                     (HermiteVector *)(base + 816),
                     (HermiteVector *)(base + 832),
                     (HermiteVector *)(base + 736 + (segment << 4)),
                     (HermiteVector *)(base + 752 + (segment << 4)));
}

/* Frames per path segment (makeHermiteCoord steps by 1/8) and the frames
 * during which the head travels the path and records the trail. */
#define GAMERA_SEGMENT_FRAMES 8
#define GAMERA_TRAVEL_FRAMES 0x18

static void fnGAMERA_PR000(void *self, void *work)
{
    GameraWork *state = (GameraWork *)work;
    HermiteVector *head;
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

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_gamera", fnGAMERA_DP000);

/* MEfObjDestroy is defined by src/main/m_ef_obj.c; sefHitEffect
 * is defined by src/main/sef.c. */
extern void sefHitEffect(void);

/* work's layout is unresolved beyond +0x70: the per-frame lifetime counter
 * this function advances, firing sefHitEffect on frame 0x18 and expiring
 * through MEfObjDestroy from frame 0x33 on. */
#define GAMERA_HIT_FRAME 0x18
#define GAMERA_LIFETIME 0x33

typedef struct GameraState {
    unsigned char unmodeled_00[0x70];
    short frame; /* +0x70 */
} GameraState;

static void fnGAMERA_PO000(void *self, void *work)
{
    GameraState *state = (GameraState *)work;

    state->frame++;
    if (state->frame == GAMERA_HIT_FRAME) {
        sefHitEffect();
    }
    if (state->frame >= GAMERA_LIFETIME) {
        MEfObjDestroy(self);
    }
}
