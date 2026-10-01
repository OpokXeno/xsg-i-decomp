/*
 * OV01 original TU 33: 0x00a39088..0x00a397c0 (4 functions)
 */
#include "common.h"
#include "m_ef_create.h"
#include "shared.h"

typedef struct EAD00ProcessWork {
    unsigned char unmodeled_00[8];
    int actor_id;
    int part_id;
    unsigned char unmodeled_10[0x60];
    int frame;
    unsigned char unmodeled_74[0x0c];
    Vector4 start[3];
    Vector4 end[3];
    Vector4 displacement[3];
    Vector4 angle[3];
} EAD00ProcessWork;

extern void MEfGetActorMatrix(Matrix4 *matrix, int actor_id, int part_id);
extern Vector4 *MMathApplyMatrix(Vector4 *destination, const Matrix4 *matrix,
                                 const Vector4 *point);
extern Vector4 *MMathSubVectorDivS(Vector4 *destination, const Vector4 *first,
                                    const Vector4 *second, float divisor);
extern void *MEfCalcAngle(Vector4 *destination, const Vector4 *from,
                           const Vector4 *to);

/* MGsGPInit (src/ov01/m_gs.c, ov01 TU 18): still TU-local there, declared
 * here for this TU until its header is published. MEfCreate_EAD00 calls it
 * with a null address and zero size, so it targets MGsGPInit's own default
 * scratchpad buffer and capacity. */
extern void MGsGPInit(void *packet, void *address, int size);

/*
 * self is the effect object MEfObjExec1st/MEfObjExec2nd (src/main/m_ef_obj.c)
 * call back with (self, self's work area); MEfCreate_EAD00 installs its own
 * process, draw and lifetime callbacks in that object's callback slots.
 */
typedef void (*EAD00Callback)(void *self, void *work);

typedef struct EAD00Object {
    unsigned char unmodeled_00[4];
    EAD00Callback process; /* +0x04 */
    unsigned char unmodeled_08[4];
    EAD00Callback draw;     /* +0x0C */
    EAD00Callback lifetime; /* +0x10 */
} EAD00Object;

/*
 * MEfCreate_EAD00's own view of self's work area (self+0x20): the frame
 * counter fnEAD00_PO000 advances (see EAD00State below) and the MGs direct-
 * transfer packet MGsGPInit initializes at work+0x140; the packet's own
 * record belongs to MGsPacket (src/ov01/m_gs.c).
 */
typedef struct EAD00Init {
    unsigned char unmodeled_00[0x70];
    int frame; /* +0x70 */
    unsigned char unmodeled_74[0x140 - 0x74];
    unsigned char packet[0x14]; /* +0x140 */
} EAD00Init;

static void fnEAD00_PR000(void *self, void *work);
static void fnEAD00_DP000(void *self, void *work);
static void fnEAD00_PO000(void *self, void *work);

int MEfCreate_EAD00(MEfObjRecord *self)
{
    EAD00Object *object = (EAD00Object *)self;
    EAD00Init *init = (EAD00Init *)((unsigned char *)self + 0x20);

    init->frame = 0;
    MGsGPInit(init->packet, 0, 0);
    object->process = fnEAD00_PR000;
    object->draw = fnEAD00_DP000;
    object->lifetime = fnEAD00_PO000;
    return 1;
}

static void fnEAD00_PR000(void *self, void *work)
{
    EAD00ProcessWork *state = (EAD00ProcessWork *)work;
    Vector4 first_point;
    Vector4 second_point;
    Matrix4 actor_matrix;
    Vector4 transformed_point;
    float frame_scale;
    Vector4 *matrix_input;
    int remaining;
    unsigned int point_index;
    int actor_id;
    int part_id;

    /* VU0's constant vf0 supplies zero xyz and unit w for both local points. */
    __asm__ __volatile__("sqc2 $vf0, 0(%0)" : : "r"(&first_point) : "memory");
    first_point.x = -0.1f;
    first_point.z = -0.2f;
    __asm__ __volatile__("sqc2 $vf0, 0(%0)" : : "r"(&second_point) : "memory");
    actor_id = state->actor_id;
    second_point.x = -9.0f;
    part_id = state->part_id;
    second_point.z = 28.0f;

    MEfGetActorMatrix(&actor_matrix, actor_id, part_id);
    frame_scale = 1.0f;
    if (state->frame < 8) {
        frame_scale = (float)state->frame * 0.125f;
    }
    if (state->frame >= 62) {
        frame_scale = (float)(69 - state->frame) * 0.125f;
    }

    matrix_input = &transformed_point;
    for (point_index = 0, remaining = 2; remaining >= 0; ) {
        /* Copy the complete aligned vector through the original scratch GPR. */
        __asm__ __volatile__("lq $8, 0(%1)\n\tsq $8, 0(%0)"
                             : : "r"(matrix_input), "r"(&first_point)
                             : "$8", "memory");
        transformed_point.x *= frame_scale;
        MMathApplyMatrix(&state->start[point_index], (const Matrix4 *)&actor_matrix,
                         matrix_input);
        first_point.x += 0.1f;

        __asm__ __volatile__("lq $8, 0(%1)\n\tsq $8, 0(%0)"
                             : : "r"(matrix_input), "r"(&second_point)
                             : "$8", "memory");
        remaining--;
        transformed_point.x *= frame_scale;
        MMathApplyMatrix(&state->end[point_index], (const Matrix4 *)&actor_matrix,
                         matrix_input);
        second_point.x += 9.0f;

        MMathSubVectorDivS(&state->displacement[point_index],
                           &state->end[point_index], &state->start[point_index], 50.0f);
        MEfCalcAngle(&state->angle[point_index], &state->start[point_index],
                      &state->end[point_index]);
        point_index++;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_ead_00", fnEAD00_DP000);

/* MEfObjDestroy (src/main/m_ef_obj.c) and sefHitEffect (src/main/sef.c) are
 * still asm in their defining TU; declared locally until published there. */
extern void sefHitEffect(void);

/* work's layout is unresolved beyond +0x70: the per-frame lifetime counter
 * this function advances, firing sefHitEffect and expiring through
 * MEfObjDestroy on the same frame, 0x46. */
#define EAD00_LIFETIME 0x46

typedef struct EAD00State {
    unsigned char unmodeled_00[0x70];
    int frame; /* +0x70 */
} EAD00State;

static void fnEAD00_PO000(void *self, void *work)
{
    EAD00State *state = (EAD00State *)work;

    state->frame++;
    if (state->frame == EAD00_LIFETIME) {
        sefHitEffect();
    }
    if (state->frame >= EAD00_LIFETIME) {
        MEfObjDestroy(self);
    }
}
