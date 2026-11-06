/*
 * OV01 original TU 30: 0x00a37ab8..0x00a38190 (4 functions)
 */
#include "common.h"
#include "ov01/m_ef_create.h"
#include "ov01/m_ef.h"
#include "shared.h"

typedef union ECM01QuadVector {
    unsigned int words[4];
    Quadword quad;
} ECM01QuadVector;

typedef struct ECM01Work {
    unsigned char unmodeled_00[8];
    Actor *actor;
    unsigned int actor_part;
    Vector4 creation_offset;
    unsigned char unmodeled_20[4];
    Actor *coordinate_actor;
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
    MEfGsPacket packet;
} ECM01Work;

typedef void ECM01Callback(void *self, ECM01Work *work);

typedef struct ECM01Object {
    unsigned char unmodeled_00[4];
    ECM01Callback *process_callback;
    unsigned char unmodeled_08[4];
    ECM01Callback *draw_callback;
    ECM01Callback *post_callback;
    unsigned char unmodeled_14[0x20 - 0x14];
    ECM01Work work;
} ECM01Object;

extern Vector4 *MMathSubVectorDivS(Vector4 *destination, const Vector4 *first,
                                   const Vector4 *second, float scale);
extern float MMathMakeRandom2PI(void);
extern void MGsGPInit(void *packet, int address, int size);

static void fnECM01_PR000(void *self, ECM01Work *work);
static void fnECM01_DP000(void *self, ECM01Work *work);
static void fnECM01_PO000(void *self, ECM01Work *work);

int MEfCreate_ECM01(MEfObjRecord *self)
{
    /* This constructor interprets its own variant of the pooled work. */
    ECM01Object *object = (ECM01Object *)self;
    ECM01Work *work = &object->work;
    Matrix4 actor_matrix;
    float angle_range;
    Quadword initial_dimensions;

    work->frame = 0;
    MEfGetActorMatrix(&actor_matrix, work->actor, work->actor_part);
    MMathApplyMatrix(&work->position, &actor_matrix, &work->creation_offset);
    MEfGetActorCoord(&work->actor_coordinates, work->coordinate_actor,
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

static void fnECM01_PR000(void *self, ECM01Work *work)
{
    ECM01Work *state = work;
    int i;

    for (i = ECM01_HISTORY_COUNT - 1; i > 0; i--) {
        state->visibility[i] = state->visibility[i - 1];
        state->frame_age[i] = state->frame_age[i - 1];
    }

    state->visibility[0] = 0;
    state->frame_age[0] = 0;
    if (state->frame < ECM01_ACTIVE_FRAMES) {
        if (state->frame == 0 || state->frame == ECM01_ACTIVE_FRAMES - 1) {
            state->visibility[0] = ECM01_ENDPOINT_MARKER;
        } else {
            state->visibility[0] = 1;
        }
        state->frame_age[0] = (short)state->frame;
    }
}

ACCEPTED_ASM("src/ov01/m_ef_create_ecm_01", fnECM01_DP000);

/* MEfObjDestroy is defined by src/main/m_ef_obj.c; sefHitEffect
 * is defined by src/main/sef.c. */
extern void sefHitEffect(void);

/* The post callback advances the per-frame lifetime counter (work's frame),
 * firing sefHitEffect at frame 0x3C and expiring through MEfObjDestroy once
 * it reaches 0x5A. */
static void fnECM01_PO000(void *self, ECM01Work *work)
{
    ECM01Work *state = work;

    state->frame++;
    if (state->frame == 0x3C) {
        sefHitEffect();
    }
    if (state->frame >= 0x5A) {
        MEfObjDestroy(self);
    }
}
