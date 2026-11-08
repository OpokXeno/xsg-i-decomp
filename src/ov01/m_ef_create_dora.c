/*
 * OV01 original TU 35: 0x00a397c8..0x00a3a090 (7 functions)
 */
#include "common.h"
#include "ov01/m_ef_create.h"
#include "ov01/m_ef.h"
#include "shared.h"
#include "main/m_math.h"
#include "vu0_macro.h"
#include "main/m_ef_obj.h"
#include "main/xgl_packet.h"

/* Trail history length: the trail_active and trail_segment arrays hold 34 shorts each,
 * and the trail array holds 34 quadwords. */
#define DORA_TRAIL_SLOTS 34

/* The Hermite control-point storage shares the post-process state object.
 * The earlier quadword arrays preserve its observed sixteen-byte alignment. */
typedef struct DoraHermiteState {
    u32 unmodeled_00[2];
    Actor *actor;       /* +0x08, the matrix source makePath reads */
    u32 coordinate;         /* +0x0c, its coordinate selector */
    Vector4 offset;         /* +0x10, translation applied to that matrix */
    unsigned char unmodeled_20[4];
    Actor *target_actor; /* +0x24, the coordinate source MEfCreate_DORA reads */
    u32 target_coordinate;   /* +0x28, its coordinate selector */
    unsigned char unmodeled_2c[0x44];
    short frame;
    unsigned char unmodeled_72[0x0e];
    Quadword weapon_coord;
    Vector4 actor_coord;
    short trail_active[DORA_TRAIL_SLOTS];
    short trail_segment[DORA_TRAIL_SLOTS];
    unsigned char unmodeled_128[8];
    Vector4 trail[DORA_TRAIL_SLOTS];
    short segment;
    short segment_frame;
    unsigned char unmodeled_354[0x0c];
    HermiteVector control_points[4];
    float path_progress[4];
    HermiteVector tangent_start;
    HermiteVector tangent_end;
    MEfGsPacket mg_packet;
} DoraHermiteState;

struct DoraState;

typedef void (*DoraCallback)(void *self, DoraHermiteState *work);
typedef void (*DoraPostCallback)(void *self, struct DoraState *work);

/*
 * Pooled effect object. The process, draw_packet and post_process callbacks sit at
 * +0x04, +0x0c and +0x10, and the work area starts at +0x20. GameraEffect in
 * m_ef_create_gamera.c has the same prefix.
 */
typedef struct DoraObject {
    unsigned int : 32;
    DoraCallback process;      /* +0x04 */
    unsigned int : 32;
    DoraCallback draw_packet;  /* +0x0c */
    DoraPostCallback post_process; /* +0x10 */
    unsigned int : 32;
    unsigned int : 32;
    unsigned int : 32;
    DoraHermiteState work;     /* +0x20 */
} DoraObject;

extern Vector4 *MEfCalcWeaponCoord(void *destination, void *weapon);
extern void MGsGPInit(void *packet, void *address, int size);

static void makePath(DoraHermiteState *state);
static void makeHermiteParams(int segment, DoraHermiteState *state);
static void fnDORA_PR000(void *self, DoraHermiteState *work);
static void fnDORA_DP000(void *self, DoraHermiteState *work);
static void fnDORA_PO000(void *self, struct DoraState *work);

/* Initializes a DORA effect object: clears the trail, builds the control points and the GS packet, and installs the callbacks. */
int MEfCreate_DORA(MEfObjRecord *self)
{
    DoraObject *object = (DoraObject *)self;
    DoraHermiteState *state = &object->work;

    state->frame = 0;
    MEfCalcWeaponCoord(&state->weapon_coord, state);
    MEfGetActorCoord(&state->actor_coord, state->target_actor, state->target_coordinate);
    memset(state->trail_active, 0, sizeof(state->trail_active));
    memset(state->trail_segment, 0, sizeof(state->trail_segment));
    memset(state->trail, 0, sizeof(state->trail));
    __asm__ __volatile__(
        "lq $8, 0(%1)\n"
        "sq $8, 0(%0)\n"
        :
        : "r"(&state->trail[0]), "r"(&state->weapon_coord)
        : "$8", "memory");
    state->segment = 0;
    state->segment_frame = 0;
    makePath(state);
    makeHermiteParams(0, state);
    MGsGPInit(&state->mg_packet, 0, 0);
    object->process = fnDORA_PR000;
    object->draw_packet = fnDORA_DP000;
    object->post_process = fnDORA_PO000;
    return 1;
}

/* Radial and forward distance of each Hermite control point from the actor-relative origin. */
static const float disttbl_0_00A51820[4];
static const float rtbl_1_00A51830[4];

extern void MMathTranslateMatrix(Matrix4 *destination, Matrix4 *matrix, Vector4 *translation);
extern void MMathRotateMatrixX(Matrix4 *destination, Matrix4 *matrix, float angle);
extern float MMathMakeRandom2PI(void);
extern float MMathMakeRandom(void);
extern unsigned short xglSRand(void);

/* Lay out the four control points on a widening spiral in front of the
 * source actor, turning either way at random. */
static void makePath(DoraHermiteState *state)
{
    Matrix4 matrix;
    Vector4 offset;
    float angle;
    float angle_step;
    float radius;
    float cosine;
    float sine;
    int point;

    angle_step = 0.13962634f;
    MEfGetActorMatrix(&matrix, state->actor, state->coordinate);
    MMathTranslateMatrix(&matrix, &matrix, &state->offset);
    MMathRotateMatrixX(&matrix, &matrix, 0.27925268f);
    angle = MMathMakeRandom2PI();
    if ((xglSRand() & 1) == 0) {
        angle_step = -0.13962634f;
    }
    radius = MMathMakeRandom() + 0.05f;
    for (point = 0; point < 4; point++) {
        __asm__ __volatile__("sqc2 $vf0, 0(%0)" : : "r"(&offset) : "memory");
        __asm__ __volatile__(
            "mfc1 $8, %1\n"
            "qmtc2 $8, $vf4\n"
            "vcallms 0xe8\n" /* VU0 micro cosine routine. */
            "qmfc2.i $8, $vf1\n"
            "mtc1 $8, %0\n"
            : "=f"(cosine)
            : "f"(angle)
            : "$8", "memory");
        offset.x = cosine * *(rtbl_1_00A51830 + point) * radius;
        __asm__ __volatile__(
            "mfc1 $8, %1\n"
            "qmtc2 $8, $vf4\n"
            "vcallms 0x20\n" /* VU0 micro sine routine. */
            "qmfc2.i $8, $vf1\n"
            "mtc1 $8, %0\n"
            : "=f"(sine)
            : "f"(angle)
            : "$8", "memory");
        offset.y = sine * *(rtbl_1_00A51830 + point) * radius;
        offset.z = *(disttbl_0_00A51820 + point);
        MMathApplyMatrix(&state->control_points[point], &matrix, &offset);
        state->path_progress[point] = angle;
        angle += angle_step;
        angle_step *= 1.2f;
    }
}

extern void MMathCalcHermitePrm(HermiteVector *tangent_start,
                                HermiteVector *tangent_end,
                                const HermiteVector *point_previous,
                                const HermiteVector *point_start,
                                const HermiteVector *point_end,
                                const HermiteVector *point_next);

/* The trail grows for the first fifteen frames; each Hermite segment
 * spans five frames (makeHermiteCoord scales the frame by 0.2). */
#define DORA_TRAIL_FRAMES 15
#define DORA_SEGMENT_FRAMES 5

/* Computes the tangents of the Hermite segment that starts at control point segment. */
static void makeHermiteParams(int segment, DoraHermiteState *state)
{
    HermiteVector *point_previous;
    HermiteVector *point_start;
    HermiteVector *point_end;
    HermiteVector *point_next;

    if (segment == 0) {
        point_previous = &state->control_points[0];
    } else {
        point_previous = &state->control_points[segment - 1];
    }
    point_start = &state->control_points[segment];
    point_end = &state->control_points[segment + 1];
    if (segment < 2) {
        point_next = &state->control_points[segment + 2];
    } else {
        point_next = point_end;
    }

    MMathCalcHermitePrm(&state->tangent_start, &state->tangent_end,
                        point_previous, point_start, point_end, point_next);
}

/* Evaluates the current Hermite position, at the segment frame, into destination. */
static void makeHermiteCoord(float *destination, DoraHermiteState *state)
{
    short segment = state->segment;
    short frame = state->segment_frame;

    MMathCalcHermite(destination, (float)frame * 0.200000003f,
                     &state->tangent_start,
                     &state->tangent_end,
                     &state->control_points[segment],
                     &state->control_points[segment + 1]);
}

/* Shift the trail history back one slot, then while the effect is still
 * emitting append the current Hermite position and advance the segment. */
static void fnDORA_PR000(void *self, DoraHermiteState *work)
{
    DoraHermiteState *state = work;
    int slot;

    /* Shift slots 0..32 into 1..33 (the last of DORA_TRAIL_SLOTS); written
     * source-indexed on purpose; a destination-indexed loop generates different code. */
    for (slot = DORA_TRAIL_SLOTS - 2; slot >= 0; slot--) {
        state->trail_active[slot + 1] = state->trail_active[slot];
        state->trail_segment[slot + 1] = state->trail_segment[slot];
        __asm__ __volatile__(
            "lq $8, 0(%1)\n"
            "sq $8, 0(%0)\n"
            :
            : "r"(&state->trail[slot + 1]), "r"(&state->trail[slot])
            : "$8", "memory");
    }

    state->trail_active[0] = 0;
    __asm__ __volatile__("sqc2 $vf0, 0(%0)" : : "r"(&state->trail[0]) : "memory");

    if (state->frame < DORA_TRAIL_FRAMES) {
        state->trail_active[0] = 1;
        state->trail_segment[0] = state->segment;
        makeHermiteCoord(&state->trail[0].x, state);
        state->segment_frame++;
        if (state->segment_frame >= DORA_SEGMENT_FRAMES) {
            state->segment_frame = 0;
            state->segment++;
            makeHermiteParams(state->segment, state);
        }
    }
}

extern void MGsGPTerm(MEfGsPacket *packet);
static const MEfGsQword coltbl_2_00A51840[4];
extern MEfObjCamParams mefCamParams;
extern int MMathRotTransPersClip(Vector4 *destination, const StudioCamera *camera,
                                 Matrix4 *matrix, const Vector4 *point);

#define DORA_STRIP_SEGMENTS 33

/* Writes one A+D register setting. */
static inline void doraSetRegister(MEfGsQword *qword, unsigned char address, u64 data)
{
    qword->ad.data = data;
    qword->ad.address = address;
}

/* Writes the GIF tag that opens the packet's next primitive and returns
 * the first data quadword after it. */
static inline MEfGsQword *doraOpenTag(MEfGsPacket *packet, int pre, int prim, int reg_count,
                                       u64 registers)
{
    MEfGsQword *qword = packet->current;

    qword->gif.tag = GIF_TAG_EOP | ((u64)pre << 46) | ((u64)prim << 47) | ((u64)reg_count << 60);
    qword->gif.registers = registers;
    packet->reg_count = reg_count;
    return qword + 1;
}

/* Stores NLOOP into the open GIF tag and moves past its data. */
static inline void doraCloseTag(MEfGsPacket *packet, int nloop)
{
    int size;

    size = packet->reg_count * nloop + 1;
    packet->current->nloop |= nloop;
    packet->current += size;
    packet->count += size;
}

/*
 * Draws the trail as strips, two sides per axis. Each vertex is offset across the trail,
 * transformed by its segment's rotation, and clipped against the screen.
 */
static void fnDORA_DP000(void *self, DoraHermiteState *work)
{
    DoraHermiteState *state = work;
    MEfGsQword colour[2];
    MEfGsQword matrix[4];
    Vector4 angle;
    MEfGsQword offset[2];
    MEfGsQword vertex[2];
    StudioCamera *screen;
    Matrix4 *view;
    MEfGsPacket *packet;
    MEfGsPacket *strip_packet;
    MEfGsQword *setting;
    MEfGsQword *qword;
    float inner;
    float outer;
    int clip;
    int drawn;
    int axis;
    int side;
    int slot;

    screen = mefCamParams.screen;
    view = mefCamParams.matrix;
    packet = &state->mg_packet;
    MGsGPInit(packet, 0, 0);
    /* Write the GS state for the strips: test, depth and additive alpha. */
    setting = packet->current;
    setting[0].gif.tag = MEF_GS_SETUP_TAG;
    setting[0].gif.registers = MEF_GS_SETUP_REGS;
    packet->reg_count = 1;
    doraSetRegister(&setting[1], SCE_GS_TEST_1, MEF_GS_TEST_1_ALPHA_GREATER_ZGEQUAL);
    doraSetRegister(&setting[2], SCE_GS_ZBUF_1, MEF_GS_ZBUF_1_PSMZ24_NO_DEPTH_WRITE);
    doraSetRegister(&setting[3], SCE_GS_ALPHA_1, MEF_GS_ALPHA_1_ADDITIVE);
    doraCloseTag(packet, 3);

    packet = &state->mg_packet;
    strip_packet = packet;
    /* One strip per axis and side; the side sets the offset across the trail. */
    for (axis = 0; axis < 2; axis++) {
        Vu0SetOrigin(&offset[0]);
        Vu0SetOrigin(&offset[1]);
        for (side = -1; side < 2; side += 2) {
            inner = (float)side * -0.025f;
            outer = (float)side * 0.3f;
            drawn = 0;
            clip = 0;
            qword = doraOpenTag(&state->mg_packet, 1, MEF_GS_STRIP_PRIM, 4, MEF_GS_STRIP_REGS);
            for (slot = 0; slot < DORA_STRIP_SEGMENTS; slot++) {
                if (state->trail_active[slot] == 0) {
                    continue;
                }
                colour[0].whole = coltbl_2_00A51840[0].whole;
                colour[1].whole = coltbl_2_00A51840[1].whole;
                if (slot == 0 || slot == DORA_STRIP_SEGMENTS - 1) {
                    colour[0].channel[3] = colour[1].channel[3] = 0;
                    offset[0].element[axis] = offset[1].element[axis] = 0.0f;
                } else {
                    offset[0].element[axis] = inner;
                    offset[1].element[axis] = outer;
                }
                MEfCalcAngle(&angle, &state->trail[slot], &state->trail[slot + 1]);
                angle.z = state->path_progress[state->trail_segment[slot]];
                MMathRotateMatrixYXZ((Matrix4 *)matrix, 0, &angle);
                /* Use the trail point as the translation of the strip matrix. */
                __asm__ __volatile__(
                    "lqc2 vf1, 0(%1)\n"
                    "vmove.w vf1, vf0\n"
                    "sqc2 vf1, 48(%0)\n"
                    :
                    : "r"(matrix), "r"(&state->trail[slot])
                    : "memory");
                MMathApplyMatrix(&angle, matrix, &offset[0].vector);
                if (MMathRotTransPersClip(&vertex[0].vector, screen, view, &angle)) {
                    clip = 2;
                } else if (clip != 0) {
                    clip--;
                    vertex[0].xyz.adc = MEF_GS_XYZ_ADC;
                }
                MMathApplyMatrix(&angle, matrix, &offset[1].vector);
                if (MMathRotTransPersClip(&vertex[1].vector, screen, view, &angle)) {
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
                doraCloseTag(strip_packet, drawn);
            }
        }
    }
    MGsGPTerm(strip_packet);
}

/* MEfObjDestroy is defined by src/main/m_ef_obj.c; sefHitEffect
 * is defined by src/main/sef.c. */
extern void sefHitEffect(void);

/* work's layout is unresolved beyond +0x70: the per-frame lifetime counter
 * this function advances, firing sefHitEffect on frame 0xf and expiring
 * through MEfObjDestroy from frame 0x30 on. */
#define DORA_HIT_FRAME 0xf
#define DORA_LIFETIME 0x30

typedef struct DoraState {
    unsigned char unmodeled_00[0x70];
    short frame; /* +0x70 */
} DoraState;

static void fnDORA_PO000(void *self, DoraState *work)
{
    DoraState *state = work;

    state->frame++;
    if (state->frame == DORA_HIT_FRAME) {
        sefHitEffect();
    }
    if (state->frame >= DORA_LIFETIME) {
        MEfObjDestroy(self);
    }
}



static const float disttbl_0_00A51820[4] = { 0.0f, 3.0f, 9.0f, 20.0f };

static const float rtbl_1_00A51830[4] = { 0.0f, 3.0f, 6.0f, 9.0f };

static const MEfGsQword coltbl_2_00A51840[4] = {
    { .channel = { 0x60, 0x18, 0x18, 0xff } },
    { .channel = { 0x0, 0x0, 0x0, 0xff } },
    { .channel = { 0x40, 0x40, 0x40, 0xff } },
    { .channel = { 0xff, 0xff, 0x60, 0xff } }
};
