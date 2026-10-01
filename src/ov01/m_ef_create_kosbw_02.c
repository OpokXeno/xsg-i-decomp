/*
 * OV01 original TU 36: 0x00a3a090..0x00a3aac8 (5 functions)
 */
#include "common.h"
#include "m_ef_create.h"
#include "shared.h"

typedef struct Kosbw02DrawWork {
    unsigned char unmodeled_00[0x40];
    int model_entry;
    const char *texture;
    unsigned char unmodeled_48[0x28];
    int frame;
    unsigned char unmodeled_74[0x8c];
    Vector4 position;
    unsigned char unmodeled_110[0x10];
    Vector4 rotation;
} Kosbw02DrawWork;

typedef struct Kosbw02GsParameter {
    u64 primitive_tag;
    float scale_u;
    float scale_v;
} Kosbw02GsParameter;

typedef struct Kosbw02CreateState {
    unsigned char unmodeled_00[0x08];
    u32 source_actor;
    u32 source_part;
    Vector4 source_offset;
    s16 rotation_degrees;
    unsigned char unmodeled_22[0x02];
    u32 target_actor;
    u32 target_part;
    unsigned char unmodeled_2c[0x44];
    int frame;
    unsigned char unmodeled_74[0x0c];
    Vector4 start_point;
    Vector4 target_point;
    Vector4 midpoint;
    float half_length;
    float angle;
    float angle_step;
    unsigned char unmodeled_bc[0x04];
    Vector4 matrix[4];
    Vector4 position;
    Vector4 previous_position;
    Vector4 rotation;
    unsigned char draw_packet[0x18];
    u64 draw_parameter;
    u64 texture_parameter;
    u16 active[10];
    unsigned char unmodeled_16c[0x04];
    Vector4 points[30];
    float twist[30];
    float random_angle;
} Kosbw02CreateState;

struct Kosbw02Effect;

typedef struct Kosbw02Allocation {
    unsigned char unmodeled_00[0x04];
    void (*process_callback)(void *task, Kosbw02CreateState *state);
    void (*model_callback)(void *task, Kosbw02DrawWork *state);
    void (*packet_callback)(void *task, void *state);
    void (*post_callback)(void *task, struct Kosbw02Effect *state);
    unsigned char unmodeled_14[0x0c];
    Kosbw02CreateState state;
} Kosbw02Allocation;

static void fnKOSBW02_PR000(void *task, Kosbw02CreateState *state);
static void fnKOSBW02_DM000(void *task, Kosbw02DrawWork *state);
static void fnKOSBW02_DP000(void *task, void *state);
static void fnKOSBW02_PO000(void *task, struct Kosbw02Effect *state);

extern void MEfGetActorMatrix(Vector4 *destination, u32 actor, u32 part);
extern void MEfGetActorCoord(Vector4 *destination, u32 actor, u32 coord);
extern Vector4 *MMathApplyMatrix(Vector4 *destination, const Vector4 *matrix,
                                 const Vector4 *source);
extern Vector4 *MMathVectorInterpolation(Vector4 *destination,
                                         const Vector4 *first,
                                         const Vector4 *second,
                                         float parameter);
extern Vector4 *MMathRotateMatrixYX(Vector4 *destination,
                                    const Vector4 *source,
                                    const Vector4 *angles);
extern float MMathCalcDist(const Vector4 *first, const Vector4 *second);
extern float MMathMakeRandom(void);
extern void *MEfCalcAngle(Vector4 *destination, const Vector4 *from,
                          const Vector4 *to);
extern void MGsGPInit(void *packet, void *address, int size);
extern Kosbw02GsParameter *svGetPrmFromName(const char *name,
                                            Kosbw02GsParameter *output);
extern float MMathMakeRandom2PI(void);
extern const char D_00A51880[];
extern const char D_00A51890[];
extern Vector4 *MMathRotateMatrixYXZ(Vector4 *destination,
                                     const Vector4 *source,
                                     const Vector4 *angles);
extern void MEfDrawModel(const Vector4 *place, int entry, const char *texture);

int MEfCreate_KOSBW02(MEfObjRecord *storage)
{
    Kosbw02Allocation *allocation = (Kosbw02Allocation *)storage;
    Kosbw02CreateState *state = &allocation->state;
    Kosbw02GsParameter parameter;
    Vector4 angle_vector;
    Vector4 actor_matrix[4];

    state->frame = 0;
    MEfGetActorMatrix(actor_matrix, state->source_actor, state->source_part);
    MMathApplyMatrix(&state->start_point, actor_matrix, &state->source_offset);
    MEfGetActorCoord(&state->target_point, state->target_actor, state->target_part);
    MMathVectorInterpolation(&state->midpoint, &state->start_point,
                             &state->target_point, 0.5f);
    state->half_length = MMathCalcDist(&state->start_point, &state->target_point) * 0.5f;
    MEfCalcAngle(&angle_vector, &state->target_point, &state->start_point);
    angle_vector.z = (float)state->rotation_degrees * 0.017453292f;
    MMathRotateMatrixYXZ(state->matrix, 0, &angle_vector);
    __asm__ __volatile__(
        "lqc2 $vf1, 0(%1)\n"
        "vmove.w $vf1, $vf0\n"
        "sqc2 $vf1, 48(%0)\n"
        :
        : "r"(state->matrix), "r"(&state->midpoint)
        : "memory");
    state->angle = 0.0f;
    state->angle_step = 0.12566371f;
    __asm__ __volatile__(
        "lq $8, 0(%1)\n"
        "sq $8, 0(%0)\n"
        :
        : "r"(&state->position), "r"(&state->start_point)
        : "$8", "memory");
    __asm__ __volatile__(
        "lq $8, 0(%1)\n"
        "sq $8, 0(%0)\n"
        :
        : "r"(&state->previous_position), "r"(&state->start_point)
        : "$8", "memory");
    MEfCalcAngle(&state->rotation, &state->target_point, &state->start_point);
    MGsGPInit(state->draw_packet, 0, 0);
    svGetPrmFromName(D_00A51880, &parameter);
    state->draw_parameter = parameter.primitive_tag;
    svGetPrmFromName(D_00A51890, &parameter);
    state->texture_parameter = parameter.primitive_tag;
    memset(state->active, 0, sizeof(state->active));
    memset(state->points, 0, sizeof(state->points));
    memset(state->twist, 0, sizeof(state->twist));
    state->random_angle = MMathMakeRandom2PI();
    allocation->process_callback = fnKOSBW02_PR000;
    allocation->model_callback = fnKOSBW02_DM000;
    allocation->packet_callback = fnKOSBW02_DP000;
    allocation->post_callback = fnKOSBW02_PO000;
    return 1;
}

static void fnKOSBW02_PR000(void *task, Kosbw02CreateState *work)
{
    Vector4 work_vector;
    Vector4 matrix[4];
    float sine;
    float cosine;
    int trail;
    int point;
    int point_index;
    Vector4 *shifted_point;
    Vector4 *source_point;
    Vector4 *trail_point;

    if (work->frame < 25) {
        __asm__ __volatile__(
            "lq $8, 0(%1)\n"
            "sq $8, 0(%0)\n"
            :
            : "r"(&work->previous_position), "r"(&work->position)
            : "$8", "memory");
        __asm__ __volatile__("sqc2 $vf0, 0(%0)"
                             :
                             : "r"(&work_vector)
                             : "memory");
        __asm__ __volatile__(
            "mfc1 $8, %1\n"
            "qmtc2 $8, $vf4\n"
            "vcallms 0x20\n"
            "qmfc2.i $8, $vf1\n"
            "mtc1 $8, %0\n"
            : "=f"(sine)
            : "f"(work->angle)
            : "$8", "memory");
        work_vector.x = work->half_length * sine;
        __asm__ __volatile__(
            "mfc1 $8, %1\n"
            "qmtc2 $8, $vf4\n"
            "vcallms 0xe8\n"
            "qmfc2.i $8, $vf1\n"
            "mtc1 $8, %0\n"
            : "=f"(cosine)
            : "f"(work->angle)
            : "$8", "memory");
        work_vector.z = work->half_length * cosine;
        MMathApplyMatrix(&work->position, work->matrix, &work_vector);
        work->angle = work->angle + work->angle_step;
        MEfCalcAngle(&work->rotation, &work->position,
                     &work->previous_position);
    }

    for (trail = 8; trail >= 0; trail--) {
        work->active[trail + 1] = work->active[trail];
        for (point = 0; point < 3; point++) {
            point_index = trail * 3 + point;
            shifted_point = &work->points[point_index + 3];
            source_point = &work->points[point_index];
            __asm__ __volatile__(
                "lq $8, 0(%1)\n"
                "sq $8, 0(%0)\n"
                :
                : "r"(shifted_point), "r"(source_point)
                : "$8", "memory");
            work->twist[point_index + 3] = work->twist[point_index];
        }
    }

    work->active[0] = 0;
    for (point = 0; point < 3; point++) {
        trail_point = &work->points[point];
        __asm__ __volatile__("sqc2 $vf0, 0(%0)"
                             :
                             : "r"(trail_point)
                             : "memory");
    }

    if (work->frame < 25) {
        work->active[0] = 1;
        for (point = 0; point < 3; point++) {
            MMathVectorInterpolation(&work_vector, &work->position,
                                     &work->previous_position,
                                     (float)point / 3.0f);
            MMathRotateMatrixYX(matrix, 0, &work->rotation);
            __asm__ __volatile__(
                "lqc2 $vf1, 0(%1)\n"
                "vmove.w $vf1, $vf0\n"
                "sqc2 $vf1, 48(%0)\n"
                :
                : "r"(matrix), "r"(&work_vector)
                : "memory");
            __asm__ __volatile__("sqc2 $vf0, 0(%0)"
                                 :
                                 : "r"(&work_vector)
                                 : "memory");
            work_vector.x = MMathMakeRandom() * 0.06f - 0.03f;
            work_vector.y = MMathMakeRandom() * 0.06f - 0.03f;
            MMathApplyMatrix(&work->points[point], matrix, &work_vector);
            work->twist[point] = MMathMakeRandom2PI();
        }
    }
}

static void fnKOSBW02_DM000(void *task, Kosbw02DrawWork *work)
{
    Vector4 matrix[4];

    if (work->frame < 25) {
        MMathRotateMatrixYXZ(matrix, 0, &work->rotation);
        __asm__ __volatile__(
            "lqc2 $vf1, 0(%1)\n"
            "vmove.w $vf1, $vf0\n"
            "sqc2 $vf1, 48(%0)\n"
            :
            : "r"(matrix), "r"(&work->position)
            : "memory");
        MEfDrawModel(matrix, work->model_entry, work->texture);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_kosbw_02", fnKOSBW02_DP000);

extern void sefHitEffect(void);

/*
 * The KOSBW02 effect record: the work area MEfCreate_KOSBW02 (outside this
 * allocation) installs fnKOSBW02_PO000 into. `frame` is the same counter role
 * the sibling MSP02 effect keeps at this same offset (MspEffect,
 * src/ov01/m_ef_create_msp_02.c): incremented once per call, it fires
 * sefHitEffect at 25 and MEfObjDestroy at 35.
 */
typedef struct Kosbw02Effect {
    unsigned char unmodeled_00[0x70];
    int frame;
} Kosbw02Effect;

static void fnKOSBW02_PO000(void *self, Kosbw02Effect *work)
{
    work->frame++;
    if (work->frame == 25) {
        sefHitEffect();
    }
    if (work->frame >= 35) {
        MEfObjDestroy(self);
    }
}
