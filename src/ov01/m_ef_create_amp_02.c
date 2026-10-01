/*
 * OV01 original TU 27: 0x00a358a8..0x00a360e0 (5 functions)
 */
#include "common.h"
#include "shared.h"

typedef unsigned int Amp02Quadword __attribute__((mode(TI)));

typedef struct Amp02TargetPacket {
    float target_x;
    unsigned char packet[0x14];
    unsigned long long parameter_first;
    unsigned long long parameter_second;
    unsigned short active[10];
    unsigned char unmodeled_3c[4];
    Vector4 points[10];
    float twist[10];
} Amp02TargetPacket;

typedef struct Amp02GsParameter {
    u64 primitive_tag;
    float scale_u;
    float scale_v;
} Amp02GsParameter;

typedef struct Amp02DrawState Amp02DrawState;
typedef struct Amp02Effect Amp02Effect;
typedef struct Amp02Object Amp02Object;

typedef void (*Amp02ProcessCallback)(void *self, void *work);
typedef void (*Amp02DrawCallback)(void *self, Amp02DrawState *work);
typedef void (*Amp02PacketCallback)(void *self, void *work);
typedef void (*Amp02LifetimeCallback)(void *self, Amp02Effect *work);

typedef struct Amp02InitState {
    unsigned char unmodeled_00[8];
    int source_actor;
    int source_part;
    Vector4 source_offset;
    unsigned char unmodeled_20[4];
    int coordinate_actor;
    int coordinate_part;
    unsigned char unmodeled_2c[0x70 - 0x2C];
    int frame;
    unsigned char unmodeled_74[0x80 - 0x74];
    Amp02Quadword spawn_position;
    Amp02Quadword circumference_position;
    Amp02Quadword position;
    Amp02Quadword velocity;
    Amp02Quadword previous_position;
    Vector4 facing;
    Amp02TargetPacket target_packet;
} Amp02InitState;

struct Amp02Object {
    unsigned char unmodeled_00[4];
    Amp02ProcessCallback process;
    Amp02DrawCallback draw;
    Amp02PacketCallback draw_packet;
    Amp02LifetimeCallback lifetime;
    unsigned char unmodeled_14[0x0C];
    Amp02InitState work;
};

extern void MEfGetActorMatrix(void *destination, u32 actor, u32 part);
extern void MEfGetActorCoord(void *destination, u32 actor, u32 part);
extern Vector4 *MEfCalcCircumXZ(void *destination, const void *center, float radius);
extern void *MMathApplyMatrix(void *destination, const void *matrix, const void *point);
extern Vector4 *MMathSubVectorDivS(void *destination, const void *first, const void *second, float divisor);
extern Vector4 *MEfCalcAngleMatrix(Vector4 *destination, const Vector4 *matrix);
extern float MMathCalcDir(const void *first, const void *second);
extern void MGsGPInit(void *packet, void *address, int size);
extern Amp02GsParameter *svGetPrmFromName(const char *name, Amp02GsParameter *output);
extern void *memset(void *destination, int value, unsigned int size);
extern const char D_00A51580[];
extern const char D_00A51590[];

static void fnAMP02_PR000(void *self, void *work);
static void fnAMP02_DP000(void *self, void *work);
static void fnAMP02_DM000(void *self, Amp02DrawState *work);
static void fnAMP02_PO000(void *self, Amp02Effect *work);

/* Initialize actor geometry, the previous position and the drawing packet. */
int MEfCreate_AMP02(Amp02Object *object)
{
    Amp02InitState *work = &object->work;
    Amp02GsParameter parameter;
    Vector4 actor_coordinate[1];
    Vector4 actor_matrix[4];

    work->frame = 0;
    MEfGetActorMatrix(actor_matrix, work->source_actor, work->source_part);
    MMathApplyMatrix(&work->spawn_position, actor_matrix, &work->source_offset);
    MEfGetActorCoord(actor_coordinate, work->coordinate_actor, work->coordinate_part);
    MEfCalcCircumXZ(&work->circumference_position, actor_coordinate, 1.0f);
    __asm__ __volatile__("lq $8,0(%1)\n\tsq $8,0(%0)"
                         : : "r"(&work->position), "r"(&work->spawn_position)
                         : "$8", "memory");
    __asm__ __volatile__("lq $8,0(%1)\n\tsq $8,0(%0)"
                         : : "r"(&work->previous_position), "r"(&work->spawn_position)
                         : "$8", "memory");
    MMathSubVectorDivS(&work->velocity, &work->circumference_position,
                       &work->spawn_position, 30.0f);

    MEfGetActorMatrix(actor_matrix, work->source_actor, work->source_part);
    MEfCalcAngleMatrix(&work->facing, actor_matrix);
    work->facing.x = 0.7853982f;
    work->facing.y = MMathCalcDir(&work->spawn_position, &work->circumference_position);
    work->target_packet.target_x = -work->facing.x * 0.06666667f;

    MGsGPInit(work->target_packet.packet, 0, 0);
    svGetPrmFromName(D_00A51580, &parameter);
    work->target_packet.parameter_first = parameter.primitive_tag;
    svGetPrmFromName(D_00A51590, &parameter);
    work->target_packet.parameter_second = parameter.primitive_tag;
    memset(work->target_packet.active, 0, sizeof(work->target_packet.active));
    memset(work->target_packet.points, 0, sizeof(work->target_packet.points));
    memset(work->target_packet.twist, 0, sizeof(work->target_packet.twist));

    object->process = fnAMP02_PR000;
    object->draw = fnAMP02_DM000;
    object->draw_packet = fnAMP02_DP000;
    object->lifetime = fnAMP02_PO000;
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_amp_02", fnAMP02_PR000);

extern void *MEfCalcAngle(Vector4 *destination, const Vector4 *from, const Vector4 *to);
extern void MMathRotateMatrixYX(Vector4 *destination, const Vector4 *source, const Vector4 *angles);
extern void MMathScaleMatrix(Vector4 *out, const Vector4 *matrix, const Vector4 *scale);
extern void MEfDrawModel(const Vector4 *place, int entry, const char *texture);
extern Vector4 scale_0_00A515A0;

/* Same hit-frame threshold fnAMP02_PO000 (below) checks; DM000 stops drawing
   once that frame is reached. */
#define AMP02_DRAW_FRAME 30

/*
 * DM000's own view of the AMP02 work area: the model `entry`/`texture` pair
 * MEfCreate_AMP02 (outside this allocation) installs, the same `frame`
 * counter fnAMP02_PO000 advances, this effect's own `position` and the
 * `target` it turns to face.
 */
typedef struct Amp02DrawState {
    unsigned char unmodeled_00[0x40];
    int entry;             /* +0x40 */
    const char *texture;   /* +0x44 */
    unsigned char unmodeled_48[0x70 - 0x48];
    int frame;              /* +0x70 */
    unsigned char unmodeled_74[0xA0 - 0x74];
    Vector4 position;      /* +0xA0 */
    unsigned char unmodeled_B0[0xC0 - 0xB0];
    Vector4 target;         /* +0xC0 */
} Amp02DrawState;

/*
 * Draws AMP02 while it is still before the hit frame: aims a matrix at
 * `target` from `position`, scales it, forces the translation row's w lane
 * to the architectural VF0.w = 1 while loading `position` into it, then
 * draws the model.
 */
static void fnAMP02_DM000(void *self, Amp02DrawState *work) {
    Vector4 angles;
    Vector4 matrix[4];

    if (work->frame < AMP02_DRAW_FRAME) {
        MEfCalcAngle(&angles, &work->position, &work->target);
        MMathRotateMatrixYX(matrix, (const Vector4 *)0, &angles);
        MMathScaleMatrix(matrix, matrix, &scale_0_00A515A0);
        __asm__ __volatile__("lqc2 vf1, 0(%0)" : : "r"(&work->position) : "memory");
        __asm__ __volatile__("vmove.w vf1, vf0" : : : "memory");
        __asm__ __volatile__("sqc2 vf1, 48(%0)" : : "r"(matrix) : "memory");
        MEfDrawModel(matrix, work->entry, work->texture);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_amp_02", fnAMP02_DP000);

extern void MEfObjDestroy(void *self);
extern void sefHitEffect(void);

/*
 * The AMP02 effect record: the work area MEfCreate_AMP02 (outside this
 * allocation) installs fnAMP02_PO000 into. `frame` is the same counter role
 * the sibling MSP02 effect keeps at this same offset (MspEffect,
 * src/ov01/m_ef_create_msp_02.c): incremented once per call, it fires
 * sefHitEffect at 30 and MEfObjDestroy at 40.
 */
typedef struct Amp02Effect {
    unsigned char unmodeled_00[0x70];
    int frame;
} Amp02Effect;

static void fnAMP02_PO000(void *self, Amp02Effect *work)
{
    work->frame++;
    if (work->frame == 30) {
        sefHitEffect();
    }
    if (work->frame >= 40) {
        MEfObjDestroy(self);
    }
}
