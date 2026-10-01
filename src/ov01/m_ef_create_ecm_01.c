/*
 * OV01 original TU 30: 0x00a37ab8..0x00a38190 (4 functions)
 */
#include "common.h"
#include "shared.h"

typedef unsigned int Quadword __attribute__((mode(TI)));

typedef union ECM01QuadVector {
    unsigned int words[4];
    Quadword quad;
} ECM01QuadVector;

typedef struct ECM01Packet {
    void *data;
    int capacity;
    void *current;
    int count;
    int aux_count;
} ECM01Packet;

typedef struct ECM01Work {
    unsigned char unmodeled_00[8];
    unsigned int actor_id;
    unsigned int actor_part;
    Vector4 creation_offset;
    unsigned char unmodeled_20[4];
    unsigned int coordinate_actor_id;
    unsigned int coordinate_part;
    unsigned char unmodeled_2C[0x70 - 0x2C];
    int frame;
    unsigned char unmodeled_74[0x80 - 0x74];
    Vector4 position;
    Vector4 actor_coordinates;
    Vector4 position_delta;
    Vector4 angles;
    float size;
    float size_step;
    float initial_angle;
    float angle_step;
    ECM01QuadVector dimensions;
    ECM01QuadVector vector_origin;
    short visibility[31];
    short frame_age[31];
    ECM01Packet packet;
} ECM01Work;

typedef void ECM01Callback(void *self, void *work);

typedef struct ECM01Object {
    unsigned char unmodeled_00[4];
    ECM01Callback *process_callback;
    unsigned char unmodeled_08[4];
    ECM01Callback *draw_callback;
    ECM01Callback *post_callback;
    unsigned char unmodeled_14[0x20 - 0x14];
    ECM01Work work;
} ECM01Object;

extern void MEfGetActorMatrix(Matrix4 destination, unsigned int actor_id,
                              unsigned int part);
extern void MEfGetActorCoord(void *destination, unsigned int actor_id,
                             unsigned int part);
extern void MMathApplyMatrix(Vector4 *destination, Matrix4 matrix,
                             Vector4 *point);
extern Vector4 *MMathSubVectorDivS(Vector4 *destination, const Vector4 *first,
                                   const Vector4 *second, float scale);
extern void *MEfCalcAngle(Vector4 *destination, const Vector4 *from,
                          const Vector4 *to);
extern float MMathMakeRandom2PI(void);
extern void MGsGPInit(void *packet, int address, int size);

static void fnECM01_PR000(void *self, void *work);
static void fnECM01_DP000(void *self, void *work);
static void fnECM01_PO000(void *self, void *work);

int MEfCreate_ECM01(ECM01Object *object)
{
    ECM01Work *work = &object->work;
    Matrix4 actor_matrix;
    float angle_range;
    Quadword initial_dimensions;

    work->frame = 0;
    MEfGetActorMatrix(actor_matrix, work->actor_id, work->actor_part);
    MMathApplyMatrix(&work->position, actor_matrix, &work->creation_offset);
    MEfGetActorCoord(&work->actor_coordinates, work->coordinate_actor_id,
                     work->coordinate_part);
    MMathSubVectorDivS(&work->position_delta, &work->actor_coordinates,
                       &work->position, 30.0f);
    MEfCalcAngle(&work->angles, &work->position, &work->actor_coordinates);

    work->size = 0.1f;
    work->size_step = 0.06f;
    work->initial_angle = (float)(xglSRand() & 1) * 3.1415927f;
    angle_range = MMathMakeRandom2PI() + 6.283185f;
    /* The packed dimension words, from low to high: 24, 48, 24, 255. */
    initial_dimensions = (Quadword)0xFF000000180000003000000018;
    work->vector_origin.quad = 0;
    work->dimensions.quad = initial_dimensions;
    work->angle_step = angle_range / 30.0f;
    work->dimensions.words[0] = (xglSRand() & 0xF) + 16;
    work->dimensions.words[2] = (xglSRand() & 0xF) + 16;

    memset(work->visibility, 0, sizeof(work->visibility));
    memset(work->frame_age, 0, sizeof(work->frame_age));
    MGsGPInit(&work->packet, 0, 0);

    object->process_callback = fnECM01_PR000;
    object->draw_callback = fnECM01_DP000;
    object->post_callback = fnECM01_PO000;
    return 1;
}

/* This callback advances the per-frame state and its 31-entry history. */
#define ECM01_ACTIVE_FRAMES 60
#define ECM01_HISTORY_COUNT 31
#define ECM01_ENDPOINT_MARKER 2

typedef struct ECM01Trail {
    unsigned char unmodeled_00[0x70];
    int frame;                         /* +0x70 */
    unsigned char unmodeled_74[0xF0 - 0x74];
    short visibility[ECM01_HISTORY_COUNT]; /* +0xF0 */
    short frame_age[ECM01_HISTORY_COUNT];   /* +0x12E */
} ECM01Trail;

static void fnECM01_PR000(void *self, void *work)
{
    ECM01Trail *trail = (ECM01Trail *)work;
    int i;

    for (i = ECM01_HISTORY_COUNT - 1; i > 0; i--) {
        trail->visibility[i] = trail->visibility[i - 1];
        trail->frame_age[i] = trail->frame_age[i - 1];
    }

    trail->visibility[0] = 0;
    trail->frame_age[0] = 0;
    if (trail->frame < ECM01_ACTIVE_FRAMES) {
        if (trail->frame == 0 || trail->frame == ECM01_ACTIVE_FRAMES - 1) {
            trail->visibility[0] = ECM01_ENDPOINT_MARKER;
        } else {
            trail->visibility[0] = 1;
        }
        trail->frame_age[0] = (short)trail->frame;
    }
}

ACCEPTED_ASM("src/ov01/m_ef_create_ecm_01", fnECM01_DP000);

/* MEfObjDestroy (src/main/m_ef_obj.c) and sefHitEffect (src/main/sef.c) are
 * still asm in their defining TU; declared locally until published there. */
extern void MEfObjDestroy(void *self);
extern void sefHitEffect(void);

/* work's layout is unresolved beyond +0x70: the per-frame lifetime counter
 * this function advances, firing sefHitEffect at frame 0x3C and expiring
 * through MEfObjDestroy once it reaches 0x5A. */
typedef struct ECM01State {
    unsigned char unmodeled_00[0x70];
    int frame; /* +0x70 */
} ECM01State;

static void fnECM01_PO000(void *self, void *work)
{
    ECM01State *state = (ECM01State *)work;

    state->frame++;
    if (state->frame == 0x3C) {
        sefHitEffect();
    }
    if (state->frame >= 0x5A) {
        MEfObjDestroy(self);
    }
}
