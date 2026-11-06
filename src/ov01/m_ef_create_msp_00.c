/*
 * OV01 original TU 23: 0x00a33f08..0x00a346c8 (5 functions)
 */
#include "common.h"
#include "ov01/m_ef_create.h"
#include "ov01/m_ef.h"
#include "shared.h"
#include "main/xgl_packet.h"

extern Matrix4 *MMathRotateMatrixYXZ(Matrix4 *destination,
                                     const Matrix4 *source,
                                     const Vector4 *angles);
extern Matrix4 *MMathScaleMatrix(Matrix4 *destination,
                                 Matrix4 *matrix,
                                 const Vector4 *scale);
extern Vector4 *MEfCalcCircumXZ(Vector4 *destination, const Vector4 *center, float radius);
extern Vector4 *MMathApplyMatrix(Vector4 *destination, Matrix4 *matrix, const Vector4 *vector);
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
    unsigned char unmodeled_02c[0x1c];
    u64 texture[2];                                             /* 0x048 */
    unsigned char unmodeled_058[0x18];
    int frame;                                                  /* 0x070 */
    unsigned char unmodeled_74[0xc];                            /* 0x074 */
    Vector4 spawnPoint;                                         /* 0x080 */
    Vector4 circumPoint;                                        /* 0x090 */
    Vector4 position;                                           /* 0x0a0 */
    Vector4 velocity;                                           /* 0x0b0 */
    Vector4 previousPosition;                                   /* 0x0c0 */
    Vector4 angles;                                             /* 0x0d0 */
    MEfGsPacket drawPacket;                                     /* 0x0e0 */
    short active[MSP00_TRAIL_SLOTS];                            /* 0x0f4 */
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
    void (*drawCallback)(void *self, MSP00Effect *effect);      /* 0x0c */
    void (*postCallback)(void *self, MSP00Effect *effect);      /* 0x10 */
    unsigned char unmodeled_14[0x0c];
    MSP00Effect work;                                           /* 0x20 */
} MSP00Object;

static void fnMSP00_PR000(void *self, MSP00Effect *effect);
static void fnMSP00_DM000(void *self, struct MSP00DrawState *work);
static void fnMSP00_DP000(void *self, MSP00Effect *effect);
static void fnMSP00_PO000(void *self, MSP00Effect *effect);

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
    MGsGPInit(&effect->drawPacket, 0, 0);
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
            MMathApplyMatrix(&effect->points[0][point], &matrix, &spray);
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

    if (work->frame < MSP00_FLIGHT_FRAMES) {
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

extern void MGsGPTerm(void *packet);
extern Quadword McEfSTQtbl[];
static const Quadword coltbl_1_00A51390[10];
static const float sztbl_2_00A51430[10];

static void fnMSP00_DP000(void *self, MSP00Effect *effect)
{
    int i;
    long regs;
    int nreg;
    Vector4 verts[4];
    MEfGsPacket *pk;
    MEfGsPacket *gp;
    MEfGsQword *q;
    volatile MEfGsQword *a1;
    volatile MEfGsQword *a2;
    volatile MEfGsQword *a3;
    volatile MEfGsQword *a4;
    volatile MEfGsQword *a5;
    Quadword color;
    int j;
    int n;
    int nloop;
    int texReg;

    pk = &effect->drawPacket;
    MGsGPInit(pk, 0, 0);
    q = pk->current;
    q[0].gif.tag = MEF_GS_SETUP_TAG;
    q[0].gif.registers = MEF_GS_SETUP_REGS;
    pk->reg_count = 1;
    a1 = q + 1;
    a2 = q + 2;
    a3 = q + 3;
    a4 = q + 4;
    a5 = q + 5;
    a1->ad.address = SCE_GS_TEST_1;
    a1->ad.data = MEF_GS_TEST_1_ALPHA_GREATER_ZGEQUAL;
    a2->ad.address = SCE_GS_TEX1_1;
    a2->ad.data = MEF_GS_TEX1_1_LINEAR_FILTER;
    a3->ad.address = SCE_GS_CLAMP_1;
    a3->ad.data = 5;
    a4->ad.address = SCE_GS_ZBUF_1;
    a4->ad.data = MEF_GS_ZBUF_1_PSMZ24_NO_DEPTH_WRITE;
    a5->ad.address = SCE_GS_ALPHA_1;
    a5->ad.data = MEF_GS_ALPHA_1_ADDITIVE;
    nreg = 5;
    n = pk->reg_count * nreg + 1;
    pk->current->nloop |= nreg;
    pk->current += n;
    pk->count += n;
    i = 0;
    for (; i < MSP00_TRAIL_SLOTS; i++) {
        if (effect->active[i] != 0) {
            color = coltbl_1_00A51390[i];
            for (j = 0; j < 3; j++) {
                if (MEfMake4Vertex2(verts, &effect->points[i][j], *(sztbl_2_00A51430 + i), effect->twist[i][j]) == 0) {
                    MEfGsQword *t;
                    MEfGsQword *g;
                    int tagCount;
                    long texture;
                    int n1;
                    int n2;

                    gp = &effect->drawPacket;
                    texReg = SCE_GS_TEX0_1;
                    regs = MEF_GS_TEXTURED_STRIP_REGS;
                    nloop = 3;
                    t = gp->current;
                    t[0].gif.tag = MEF_GS_SETUP_TAG;
                    t[0].gif.registers = MEF_GS_SETUP_REGS;
                    tagCount = 1;
                    gp->reg_count = tagCount;
                    a1 = t + 1;
                    texture = effect->texture[(j ^ i) & 1];
                    a1->ad.address = texReg;
                    a1->ad.data = texture;
                    n1 = gp->reg_count + 1;
                    gp->current->nloop |= 1;
                    gp->current += n1;
                    gp->count += n1;
                    g = gp->current;
                    g[0].gif.tag = MEF_GS_TEXTURED_STRIP_TAG;
                    g[0].gif.registers = regs;
                    gp->reg_count = nloop;
                    g[1].whole = McEfSTQtbl[0];
                    g[2].whole = color;
                    g[3].whole = *(Quadword *)&verts[0];
                    g[4].whole = McEfSTQtbl[1];
                    g[5].whole = color;
                    g[6].whole = *(Quadword *)&verts[1];
                    g[7].whole = McEfSTQtbl[2];
                    g[8].whole = color;
                    g[9].whole = *(Quadword *)&verts[2];
                    g[10].whole = McEfSTQtbl[3];
                    g[11].whole = color;
                    g[12].whole = *(Quadword *)&verts[3];
                    n2 = gp->reg_count * 4 + 1;
                    gp->current->nloop |= 4;
                    gp->current += n2;
                    gp->count += n2;
                    n1 = j;
                    n2 = j;
                    n = j;
                    nreg = j;
                    t = g;
                    tagCount = j;
                }
            }
        }
    }
    MGsGPTerm(pk);
}

/* MEfObjDestroy is defined by src/main/m_ef_obj.c; sefHitEffect
 * is defined by src/main/sef.c. */
extern void sefHitEffect(void);

/* The post callback advances the frame counter (+0x70) once per call, fires
 * sefHitEffect at MSP00_HIT_FRAME and expires through MEfObjDestroy at
 * MSP00_LIFETIME. */
#define MSP00_HIT_FRAME 0x12
#define MSP00_LIFETIME  0x1C

static void fnMSP00_PO000(void *self, MSP00Effect *effect)
{
    effect->frame++;
    if (effect->frame == MSP00_HIT_FRAME) {
        sefHitEffect();
    }
    if (effect->frame >= MSP00_LIFETIME) {
        MEfObjDestroy(self);
    }
}



static const Quadword coltbl_1_00A51390[10] = {
    0x20000000800000008000000080,
    0x28000000800000008000000080,
    0x28000000800000008000000080,
    0x28000000800000008000000080,
    0x28000000800000008000000080,
    0x24000000800000008000000080,
    0x20000000800000008000000080,
    0x18000000800000008000000080,
    0x10000000800000008000000080,
    0x8000000800000008000000080
};

static const float sztbl_2_00A51430[10] = { 0.10000000149011612f, 0.12999999523162842f, 0.15999999642372131f, 0.18000000715255737f, 0.20000000298023224f, 0.2199999988079071f, 0.23999999463558197f, 0.25999999046325684f, 0.2800000011920929f, 0.25f };
