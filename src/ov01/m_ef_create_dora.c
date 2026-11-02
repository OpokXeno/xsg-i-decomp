/*
 * OV01 original TU 35: 0x00a397c8..0x00a3a090 (7 functions)
 */
#include "common.h"
#include "ov01/m_ef_create.h"
#include "ov01/m_ef.h"
#include "shared.h"
#include "main/m_math.h"

typedef unsigned int DoraQuadword __attribute__((mode(TI)));

/* Trail history length: trail_active +0xa0..+0xe4, trail_segment
 * +0xe4..+0x128 (0x44 bytes = 34 shorts each, the memset sizes in
 * MEfCreate_DORA), trail +0x130..+0x350 (0x220 bytes = 34 quadwords). */
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
    DoraQuadword weapon_coord;
    Vector4 actor_coord;
    short trail_active[DORA_TRAIL_SLOTS];
    short trail_segment[DORA_TRAIL_SLOTS];
    unsigned char unmodeled_128[8];
    DoraQuadword trail[DORA_TRAIL_SLOTS];
    short segment;
    short segment_frame;
    unsigned char unmodeled_354[0x0c];
    HermiteVector control_points[4];
    float path_progress[4];
    HermiteVector tangent_start;
    HermiteVector tangent_end;
    unsigned char mg_packet;
} DoraHermiteState;

typedef void (*DoraCallback)(void *self, void *work);

/* The pooled effect object, offsets as MEfCreate_DORA proves them: the
 * callback stores `sw $3,0x4($17)` (process), `sw $4,0xC($17)` (draw_packet)
 * and `sw $2,0x10($17)` (post_process) on the object, and the record at +0x20
 * from `addiu $16,$17,0x20` whose +0x24/+0x28/+0x70 fields are then reached
 * with `lw`/`sh`. Same shape as GameraEffect in the landed
 * m_ef_create_gamera.c; no published header declares this prefix. */
typedef struct DoraObject {
    unsigned int : 32;
    DoraCallback process;      /* +0x04 */
    unsigned int : 32;
    DoraCallback draw_packet;  /* +0x0c */
    DoraCallback post_process; /* +0x10 */
    unsigned int : 32;
    unsigned int : 32;
    unsigned int : 32;
    DoraHermiteState work;     /* +0x20 */
} DoraObject;

extern Vector4 *MEfCalcWeaponCoord(void *destination, void *weapon);
extern void MGsGPInit(void *packet, void *address, int size);

static void makePath(DoraHermiteState *state);
static void makeHermiteParams(int segment, DoraHermiteState *state);
static void fnDORA_PR000(void *self, void *work);
static void fnDORA_DP000(void *self, void *work);
static void fnDORA_PO000(void *self, void *work);

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

/* Radial and forward distance of each Hermite control point from the
 * actor-relative origin (rodata owned by this TU's data file). */
extern float disttbl_0_00A51820[4];
extern float rtbl_1_00A51830[4];

extern void MMathTranslateMatrix(Matrix4 *destination, Matrix4 *matrix, Vector4 *translation);
extern void MMathRotateMatrixX(Matrix4 *destination, Matrix4 *matrix, float angle);
extern float MMathMakeRandom2PI(void);
extern float MMathMakeRandom(void);
extern unsigned short xglSRand(void);
extern void *MMathApplyMatrix(void *destination, const void *matrix, const void *point);

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
            "vcallms 0xe8\n" /* VU0 micro cos, as m_ef.c MEfScatterXY and gamera makePath */
            "qmfc2.i $8, $vf1\n"
            "mtc1 $8, %0\n"
            : "=f"(cosine)
            : "f"(angle)
            : "$8", "memory");
        offset.x = cosine * rtbl_1_00A51830[point] * radius;
        __asm__ __volatile__(
            "mfc1 $8, %1\n"
            "qmtc2 $8, $vf4\n"
            "vcallms 0x20\n" /* VU0 micro sin, as m_ef.c MEfScatterXY and gamera makePath */
            "qmfc2.i $8, $vf1\n"
            "mtc1 $8, %0\n"
            : "=f"(sine)
            : "f"(angle)
            : "$8", "memory");
        offset.y = sine * rtbl_1_00A51830[point] * radius;
        offset.z = disttbl_0_00A51820[point];
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

static void makeHermiteCoord(float *destination, void *effect)
{
    unsigned char *base = (unsigned char *)effect;
    short segment = *(short *)(base + 848);
    short frame = *(short *)(base + 850);

    MMathCalcHermite(destination, (float)frame * 0.200000003f,
                     (HermiteVector *)(base + 944),
                     (HermiteVector *)(base + 960),
                     (HermiteVector *)(base + 864 + (segment << 4)),
                     (HermiteVector *)(base + 880 + (segment << 4)));
}

/* Shift the trail history back one slot, then while the effect is still
 * emitting append the current Hermite position and advance the segment. */
static void fnDORA_PR000(void *self, void *work)
{
    DoraHermiteState *state = (DoraHermiteState *)work;
    int slot;

    /* Shift slots 0..32 into 1..33 (the last of DORA_TRAIL_SLOTS); written
     * source-indexed because the destination-indexed loop moved the bytes. */
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
        makeHermiteCoord((float *)&state->trail[0], state);
        state->segment_frame++;
        if (state->segment_frame >= DORA_SEGMENT_FRAMES) {
            state->segment_frame = 0;
            state->segment++;
            makeHermiteParams(state->segment, state);
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_dora", fnDORA_DP000);

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

static void fnDORA_PO000(void *self, void *work)
{
    DoraState *state = (DoraState *)work;

    state->frame++;
    if (state->frame == DORA_HIT_FRAME) {
        sefHitEffect();
    }
    if (state->frame >= DORA_LIFETIME) {
        MEfObjDestroy(self);
    }
}
