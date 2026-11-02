/*
 * OV01 original TU 23: 0x00a33f08..0x00a346c8 (5 functions)
 */
#include "common.h"
#include "ov01/m_ef_create.h"
#include "ov01/m_ef.h"
#include "shared.h"

extern Matrix4 *MMathRotateMatrixYXZ(Matrix4 *destination,
                                     const Matrix4 *source,
                                     const Vector4 *angles);
extern Matrix4 *MMathScaleMatrix(Matrix4 *destination,
                                 Matrix4 *matrix,
                                 const Vector4 *scale);
extern void MEfDrawModel(const Vector4 *place, int entry, const char *texture);
extern Vector4 *MEfCalcCircumXZ(Vector4 *destination, const Vector4 *center, float radius);
extern Vector4 *MEfCalcAngle(Vector4 *destination, const Vector4 *from, const Vector4 *to);
extern Vector4 *MMathApplyMatrix(Vector4 *destination, const Matrix4 *matrix, const Vector4 *vector);
extern Vector4 *MMathSubVectorDivS(Vector4 *destination, const Vector4 *first,
                                   const Vector4 *second, float divisor);
extern void MMathVectorInterpolation(Vector4 *destination, const Vector4 *first,
                                     const Vector4 *second, float parameter);
extern void MMathRotateMatrixYX(Matrix4 matrix, int flags, const Vector4 *angles);
extern float MMathMakeRandom(void);
extern float MMathMakeRandom2PI(void);
extern void MGsGPInit(void *packet, void *address, int size);

/*
 * The MSP00 effect record: the work area the MEf effect object carries at
 * +0x20. MEfCreate_MSP00 reads the creation request (actor/selector pairs and the
 * creation offset), seeds the flight vectors and clears the ten-slot trail;
 * fnMSP00_PR000 moves the head and shifts the trail every frame.
 */
#define MSP00_TRAIL_SLOTS  10
#define MSP00_TRAIL_POINTS 3
#define MSP00_FLIGHT_FRAMES 18

typedef struct MSP00Effect {
    unsigned char unmodeled_000[0x08];
    Actor *actor;                                           /* 0x008 */
    u32 actorCoordinate;                                        /* 0x00c */
    Vector4 creationOffset;                                     /* 0x010 */
    unsigned char unmodeled_020[0x04];
    Actor *targetActor;                                     /* 0x024 */
    u32 targetCoordinate;                                       /* 0x028 */
    unsigned char unmodeled_02c[0x44];
    int frame;                                                  /* 0x070 */
    unsigned char unmodeled_74[0xc];                            /* 0x074 */
    Vector4 spawnPoint;                                         /* 0x080 */
    Vector4 circumPoint;                                        /* 0x090 */
    Vector4 position;                                           /* 0x0a0 */
    Vector4 velocity;                                           /* 0x0b0 */
    Vector4 previousPosition;                                   /* 0x0c0 */
    Vector4 angles;                                             /* 0x0d0 */
    unsigned char drawPacket[0x14];                             /* 0x0e0 */
    unsigned short active[MSP00_TRAIL_SLOTS];                   /* 0x0f4 */
    unsigned char unmodeled_108[0x8];                           /* 0x108 */
    Vector4 points[MSP00_TRAIL_SLOTS][MSP00_TRAIL_POINTS];      /* 0x110 */
    float twist[MSP00_TRAIL_SLOTS][MSP00_TRAIL_POINTS];         /* 0x2f0 */
} MSP00Effect;

struct MSP00DrawState;
typedef void MSP00Callback(void *self, void *work);

typedef struct MSP00Object {
    unsigned char unmodeled_00[4];
    void (*processCallback)(void *self, MSP00Effect *effect);   /* 0x04 */
    void (*drawModelCallback)(void *self, struct MSP00DrawState *work); /* 0x08 */
    MSP00Callback *drawCallback;                                /* 0x0c */
    MSP00Callback *postCallback;                                /* 0x10 */
    unsigned char unmodeled_14[0x0c];
    MSP00Effect work;                                           /* 0x20 */
} MSP00Object;

static void fnMSP00_PR000(void *self, MSP00Effect *effect);
static void fnMSP00_DM000(void *self, struct MSP00DrawState *work);
static void fnMSP00_DP000(void *self, void *work);
static void fnMSP00_PO000(void *self, void *work);

int MEfCreate_MSP00(MSP00Object *object)
{
    MSP00Effect *effect = &object->work;
    Vector4 targetPoint;
    Matrix4 actorMatrix;

    effect->frame = 0;
    MEfGetActorMatrix(&actorMatrix, effect->actor, effect->actorCoordinate);
    MMathApplyMatrix(&effect->spawnPoint, &actorMatrix, &effect->creationOffset);
    MEfGetActorCoord(&targetPoint, effect->targetActor, effect->targetCoordinate);
    MEfCalcCircumXZ(&effect->circumPoint, &targetPoint, 1.0f);
    /* Quadword copies: position = previousPosition = spawnPoint. */
    __asm__ __volatile__(
        "lq $8, 0(%1)\n"
        "sq $8, 0(%0)\n"
        :
        : "r"(&effect->position), "r"(&effect->spawnPoint)
        : "$8", "memory");
    __asm__ __volatile__(
        "lq $8, 0(%1)\n"
        "sq $8, 0(%0)\n"
        :
        : "r"(&effect->previousPosition), "r"(&effect->spawnPoint)
        : "$8", "memory");
    MMathSubVectorDivS(&effect->velocity, &effect->circumPoint, &effect->spawnPoint,
                       (float)MSP00_FLIGHT_FRAMES);
    MEfCalcAngle(&effect->angles, &effect->circumPoint, &effect->spawnPoint);
    MGsGPInit(effect->drawPacket, 0, 0);
    memset(effect->active, 0, sizeof(effect->active));
    memset(effect->points, 0, sizeof(effect->points));
    memset(effect->twist, 0, sizeof(effect->twist));

    object->processCallback = fnMSP00_PR000;
    object->drawModelCallback = fnMSP00_DM000;
    object->drawCallback = fnMSP00_DP000;
    object->postCallback = fnMSP00_PO000;
    return 1;
}

/*
 * fnMSP00_PR000: per-frame update. While the effect is in flight the head
 * advances by its velocity and spins about z; every frame the ten-slot trail
 * history shifts down one slot, and while in flight slot 0 is refilled with
 * three points sprayed along the step just taken.
 */
static void fnMSP00_PR000(void *self, MSP00Effect *effect)
{
    Vector4 spray;
    Matrix4 matrix;
    int slot;
    int point;

    if (effect->frame < MSP00_FLIGHT_FRAMES) {
        /* Quadword copy: previousPosition = position. */
        __asm__ __volatile__(
            "lq $8, 0(%1)\n"
            "sq $8, 0(%0)\n"
            :
            : "r"(&effect->previousPosition), "r"(&effect->position)
            : "$8", "memory");
        /* position.xyz += velocity.xyz, leaving position.w alone. */
        __asm__ __volatile__(
            "lqc2 $vf1, 0(%1)\n"
            "lqc2 $vf2, 0(%2)\n"
            "vadd.xyz $vf1, $vf1, $vf2\n"
            "sqc2 $vf1, 0(%0)\n"
            :
            : "r"(&effect->position), "r"(&effect->position), "r"(&effect->velocity)
            : "memory");
        effect->angles.z += 0.5235988f;
    }

    for (slot = MSP00_TRAIL_SLOTS - 2; slot >= 0; slot--) {
        effect->active[slot + 1] = effect->active[slot];
        for (point = 0; point < MSP00_TRAIL_POINTS; point++) {
            __asm__ __volatile__(
                "lq $8, 0(%1)\n"
                "sq $8, 0(%0)\n"
                :
                : "r"(&effect->points[slot + 1][point]), "r"(&effect->points[slot][point])
                : "$8", "memory");
            effect->twist[slot + 1][point] = effect->twist[slot][point];
        }
    }

    effect->active[0] = 0;
    for (point = 0; point < MSP00_TRAIL_POINTS; point++)
        __asm__ __volatile__("sqc2 $vf0, 0(%0)" : : "r"(&effect->points[0][point]) : "memory");

    if (effect->frame < MSP00_FLIGHT_FRAMES) {
        effect->active[0] = 1;
        for (point = 0; point < MSP00_TRAIL_POINTS; point++) {
            MMathVectorInterpolation(&spray, &effect->position, &effect->previousPosition,
                                     (float)point / 3.0f);
            MMathRotateMatrixYX(matrix, 0, &effect->angles);
            /* matrix[3] = the interpolated point, keeping the W lane of vf0. */
            __asm__ __volatile__(
                "lqc2 $vf1, 0(%1)\n"
                "vmove.w $vf1, $vf0\n"
                "sqc2 $vf1, 48(%0)\n"
                :
                : "r"(matrix), "r"(&spray)
                : "memory");

            /* A small random spray around the interpolated trail point. */
            __asm__ __volatile__("sqc2 $vf0, 0(%0)" : : "r"(&spray) : "memory");
            spray.x = MMathMakeRandom() * 0.06f - 0.03f;
            spray.y = MMathMakeRandom() * 0.06f - 0.03f;
            MMathApplyMatrix(&effect->points[0][point], (const Matrix4 *)&matrix, &spray);
            effect->twist[0][point] = MMathMakeRandom2PI();
        }
    }
}

typedef struct MSP00DrawState {
    unsigned char unmodeled_00[0x40];
    int entry;                 /* +0x40 */
    const char *texture;       /* +0x44 */
    unsigned char unmodeled_48[0x28];
    int frame;                 /* +0x70 */
    unsigned char unmodeled_74[0x2c];
    Vector4 position;          /* +0xa0 */
    unsigned char unmodeled_b0[0x20];
    Vector4 angles;             /* +0xd0 */
} MSP00DrawState;

static void fnMSP00_DM000(void *self, MSP00DrawState *work)
{
    static const Vector4 scale = { 0.04f, 0.04f, 0.2f, 1.0f };
    Matrix4 matrix;

    if (work->frame < 18) {
        MMathRotateMatrixYXZ(&matrix, 0, &work->angles);
        MMathScaleMatrix(&matrix, &matrix, &scale);
        __asm__ __volatile__(
            "lqc2 vf1, 0(%1)\n\t"
            "vmove.w vf1, vf0\n\t"
            "sqc2 vf1, 48(%0)\n\t"
            :
            : "r"(&matrix), "r"(&work->position)
            : "memory"
        );
        MEfDrawModel((const Vector4 *)&matrix, work->entry, work->texture);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_msp_00", fnMSP00_DP000);

/* MEfObjDestroy is defined by src/main/m_ef_obj.c; sefHitEffect
 * is defined by src/main/sef.c. */
extern void sefHitEffect(void);

/* work's layout is unresolved beyond +0x70: the per-frame lifetime counter
 * this function advances, firing sefHitEffect at frame 0x12 and expiring
 * through MEfObjDestroy once it reaches 0x1C. */
#define MSP00_HIT_FRAME 0x12
#define MSP00_LIFETIME  0x1C

typedef struct MSP00State {
    unsigned char unmodeled_00[0x70];
    int frame; /* +0x70 */
} MSP00State;

static void fnMSP00_PO000(void *self, void *work)
{
    MSP00State *state = (MSP00State *)work;

    state->frame++;
    if (state->frame == MSP00_HIT_FRAME) {
        sefHitEffect();
    }
    if (state->frame >= MSP00_LIFETIME) {
        MEfObjDestroy(self);
    }
}
