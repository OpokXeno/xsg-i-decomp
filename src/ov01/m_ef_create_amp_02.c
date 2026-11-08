/*
 * OV01 original TU 27: 0x00a358a8..0x00a360e0 (5 functions)
 */
#include "common.h"
#include "ov01/m_ef_create.h"
#include "ov01/m_ef.h"
#include "shared.h"
#include "quadword.h"
#include "vu0_macro.h"
#include "main/xgl_packet.h"

typedef struct Amp02TargetPacket {
    float target_x;
    MEfGsPacket packet;
    unsigned long long parameter_first;
    unsigned long long parameter_second;
    unsigned short active[10];
    unsigned char unmodeled_3c[4];
    Vector4 points[10][1];
    float twist[10][1];
} Amp02TargetPacket;

typedef struct Amp02GsParameter {
    u64 primitive_tag;
    float scale_u;
    float scale_v;
} Amp02GsParameter;

typedef struct Amp02DrawView Amp02DrawView;
typedef struct Amp02Object Amp02Object;

struct Amp02InitState;

typedef void (*Amp02ProcessCallback)(void *self, struct Amp02InitState *work);
typedef void (*Amp02DrawCallback)(void *self, Amp02DrawView *work);
typedef void (*Amp02PacketCallback)(void *self, struct Amp02InitState *work);
typedef void (*Amp02LifetimeCallback)(void *self, struct Amp02InitState *work);

typedef struct Amp02InitState {
    unsigned char unmodeled_00[8];
    Actor *source_actor;
    int source_part;
    Vector4 source_offset;
    unsigned char unmodeled_20[4];
    Actor *coordinate_actor;
    int coordinate_part;
    unsigned char unmodeled_2c[0x70 - 0x2C];
    int frame;
    unsigned char unmodeled_74[0x80 - 0x74];
    Vector4 spawn_position;
    Vector4 circumference_position;
    Vector4 position;
    Vector4 velocity;
    Vector4 previous_position;
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

extern Vector4 *MEfCalcCircumXZ(void *destination, const void *center, float radius);
extern Vector4 *MMathSubVectorDivS(void *destination, const void *first, const void *second, float divisor);
extern Vector4 *MEfCalcAngleMatrix(Vector4 *destination, Matrix4 *matrix);
extern float MMathCalcDir(const void *first, const void *second);
extern void MGsGPInit(void *packet, void *address, int size);
extern Amp02GsParameter *svGetPrmFromName(const char *name, Amp02GsParameter *output);
extern void *memset(void *destination, int value, unsigned int size);
const char efAmp02DeCld01Text[16] = "de_cld01";
const char efAmp02DeCld02Text[16] = "de_cld02";

static void fnAMP02_PR000(void *self, Amp02InitState *work);
static void fnAMP02_DP000(void *self, Amp02InitState *work);
static void fnAMP02_DM000(void *self, Amp02DrawView *work);
static void fnAMP02_PO000(void *self, Amp02InitState *work);

/* Starts the flight from the source actor's matrix, sets the facing and the trail packet, and installs the callbacks. */
int MEfCreate_AMP02(MEfObjRecord *self)
{
    /* The pooled record is viewed as this effect's own object type. */
    Amp02Object *object = (Amp02Object *)self;
    Amp02InitState *work = &object->work;
    Amp02GsParameter parameter;
    Vector4 actor_coordinate[1];
    Matrix4 actor_matrix;

    work->frame = 0;
    MEfGetActorMatrix(&actor_matrix, work->source_actor, work->source_part);
    MMathApplyMatrix(&work->spawn_position, &actor_matrix, &work->source_offset);
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

    MEfGetActorMatrix(&actor_matrix, work->source_actor, work->source_part);
    MEfCalcAngleMatrix(&work->facing, &actor_matrix);
    work->facing.x = 0.7853982f;
    work->facing.y = MMathCalcDir(&work->spawn_position, &work->circumference_position);
    work->target_packet.target_x = -work->facing.x * 0.06666667f;

    MGsGPInit(&work->target_packet.packet, 0, 0);
    svGetPrmFromName(efAmp02DeCld01Text, &parameter);
    work->target_packet.parameter_first = parameter.primitive_tag;
    svGetPrmFromName(efAmp02DeCld02Text, &parameter);
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


extern Vector4 *MEfCalcAngle(Vector4 *destination, const Vector4 *from, const Vector4 *to);
extern void MMathRotateMatrixYX(Vector4 *destination, const Vector4 *source, const Vector4 *angles);
extern void MMathScaleMatrix(Vector4 *out, const Vector4 *matrix, const Vector4 *scale);
extern void MEfDrawModel(const Vector4 *place, int entry, const char *texture);

#define AMP02_FLIGHT_FRAMES 30
#define AMP02_TRAIL_SLOTS   10
#define AMP02_TRAIL_POINTS  1
#define AMP02_POINT_BYTES   16
#define AMP02_POINTS_OFFSET 0x120

extern float MMathCalcLengthXZ(const Vector4 *vector);
extern float tanf(float angle);
extern float MMathMakeRandom(void);
extern float MMathMakeRandom2PI(void);
extern void MMathVectorInterpolation(Vector4 *destination, const Vector4 *first,
                                     const Vector4 *second, float parameter);

/*
 * Advances the flight: moves the head position, steps the facing angle x each frame,
 * and shifts the ten-slot trail of points and their twist values (target_packet).
 */
static void fnAMP02_PR000(void *self, Amp02InitState *work)
{
    Vector4 angles;
    Vector4 spray;
    Matrix4 matrix;
    int slot;
    int point;

    if (work->frame < AMP02_FLIGHT_FRAMES) {
        QuadwordCopyT0(&work->previous_position, &work->position);
        Vu0SefAddVectorXYZ(&work->position, &work->position, &work->velocity);
        work->position.y += MMathCalcLengthXZ(&work->velocity) * tanf(work->facing.x);
        work->facing.x += work->target_packet.target_x;
    }

    for (slot = AMP02_TRAIL_SLOTS - 2; slot >= 0; slot--) {
        work->target_packet.active[slot + 1] = work->target_packet.active[slot];
        for (point = 0; point < AMP02_TRAIL_POINTS; point++) {
            QuadwordCopyT0(
                (Vector4 *)((slot + 1) * AMP02_POINT_BYTES + (int)work + AMP02_POINTS_OFFSET + point * AMP02_POINT_BYTES),
                (Vector4 *)(slot * AMP02_POINT_BYTES + (int)work + AMP02_POINTS_OFFSET + point * AMP02_POINT_BYTES));
            work->target_packet.twist[slot + 1][point] = work->target_packet.twist[slot][point];
        }
    }

    work->target_packet.active[0] = 0;
    for (point = 0; point < AMP02_TRAIL_POINTS; point++)
        __asm__ __volatile__("sqc2 $vf0, 0(%0)" : : "r"(&work->target_packet.points[0][point]) : "memory");

    if (work->frame < AMP02_FLIGHT_FRAMES) {
        MEfCalcAngle(&angles, &work->previous_position, &work->position);
        work->target_packet.active[0] = 1;
        for (point = 0; point < AMP02_TRAIL_POINTS; point++) {
            MMathVectorInterpolation(&spray, &work->position, &work->previous_position,
                                     (float)point);
            MMathRotateMatrixYX((Vector4 *)matrix, 0, &work->facing);
            __asm__ __volatile__(
                "lqc2 $vf1, 0(%1)\n"
                "vmove.w $vf1, $vf0\n"
                "sqc2 $vf1, 48(%0)\n"
                :
                : "r"(matrix), "r"(&spray)
                : "memory");
            __asm__ __volatile__("sqc2 $vf0, 0(%0)" : : "r"(&spray) : "memory");
            spray.x = MMathMakeRandom() * 0.06f - 0.03f;
            spray.y = MMathMakeRandom() * 0.06f - 0.03f;
            MMathApplyMatrix(&work->target_packet.points[0][point], matrix, &spray);
            work->target_packet.twist[0][point] = MMathMakeRandom2PI();
        }
    }
}

/* Same hit-frame threshold fnAMP02_PO000 (below) checks; DM000 stops drawing
   once that frame is reached. */
#define AMP02_DRAW_FRAME 30

/*
 * DM000's own view of the AMP02 work area: the model `entry`/`texture` pair
 * MEfCreate_AMP02 installs, the same `frame`
 * counter fnAMP02_PO000 advances, this effect's own `position` and the
 * `target` it turns to face.
 */
/* Partial view of Amp02InitState. Offset +0xC0 is `target` for this draw role but
 * previous_position for fnAMP02_PR000, so the draw role keeps its own type. */
typedef struct Amp02DrawView {
    unsigned char unmodeled_00[0x40];
    int entry;             /* +0x40 */
    const char *texture;   /* +0x44 */
    unsigned char unmodeled_48[0x70 - 0x48];
    int frame;              /* +0x70 */
    unsigned char unmodeled_74[0xA0 - 0x74];
    Vector4 position;      /* +0xA0 */
    unsigned char unmodeled_B0[0xC0 - 0xB0];
    Vector4 target;         /* +0xC0 */
} Amp02DrawView;

/*
 * Draws AMP02 while it is still before the hit frame: aims a matrix at
 * `target` from `position`, scales it, forces the translation row's w lane
 * to the architectural VF0.w = 1 while loading `position` into it, then
 * draws the model.
 */
static void fnAMP02_DM000(void *self, Amp02DrawView *work) {
    static const Vector4 scale = { 0.08f, 0.08f, 0.25f, 1.0f };
    Vector4 angles;
    Vector4 matrix[4];

    if (work->frame < AMP02_DRAW_FRAME) {
        MEfCalcAngle(&angles, &work->position, &work->target);
        MMathRotateMatrixYX(matrix, (const Vector4 *)0, &angles);
        MMathScaleMatrix(matrix, matrix, &scale);
        __asm__ __volatile__("lqc2 vf1, 0(%0)" : : "r"(&work->position) : "memory");
        __asm__ __volatile__("vmove.w vf1, vf0" : : : "memory");
        __asm__ __volatile__("sqc2 vf1, 48(%0)" : : "r"(matrix) : "memory");
        MEfDrawModel(matrix, work->entry, work->texture);
    }
}

extern int MEfMake4Vertex2(Vector4 *dst, const Vector4 *source, float size, float angleBase);
extern void MGsGPTerm(void *packet);
extern Quadword McEfSTQtbl[];
static const Quadword coltbl_1_00A515B0[10];
static const float sztbl_2_00A51650[10];

/*
 * Builds the GS packet for the trail: a setup block, then one textured strip of four
 * vertices for each active slot.
 */
static void fnAMP02_DP000(void *self, Amp02InitState *work)
{
    int i;
    long regs;
    int nreg;
    const float *szp;
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
    unsigned long *tex;
    int j;
    int n;
    int off;
    int lo;
    int nloop;
    int texReg;
    char *p;

    i = 0;
    p = (char *)work + 8;
    pk = &work->target_packet.packet;
    MGsGPInit(pk, 0, 0);
    q = pk->current;
    q[0].gif.tag = MEF_GS_SETUP_TAG;
    q[0].gif.registers = MEF_GS_SETUP_REGS;
    pk->reg_count = 1;
    /* Write the GS state: test, texture filter, clamp, depth and additive alpha. */
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
    /* Emit the textured strip of each active trail slot. */
    for (; i < 10; i++) {
        if (((short *)work->target_packet.active)[i] != 0) {
            color = coltbl_1_00A515B0[i];
            szp = &sztbl_2_00A51650[i];
            lo = i < 6;
            off = 0xf8;
            if (lo) {
                off = 0xf0;
            }
            tex = (unsigned long *)(p + off);
            for (j = 0; j < 1; j++) {
                if (MEfMake4Vertex2(verts, &work->target_packet.points[i][j], *szp,
                                    work->target_packet.twist[i][j]) == 0) {
                    MEfGsQword *t;
                    MEfGsQword *g;
                    unsigned long texVal;
                    int n1;
                    int n2;

                    gp = &work->target_packet.packet;
                    texReg = SCE_GS_TEX0_1;
                    regs = MEF_GS_TEXTURED_STRIP_REGS;
                    nloop = 3;
                    t = gp->current;
                    t[0].gif.tag = MEF_GS_SETUP_TAG;
                    t[0].gif.registers = MEF_GS_SETUP_REGS;
                    gp->reg_count = 1;
                    a1 = t + 1;
                    texVal = *tex;
                    a1->ad.address = texReg;
                    a1->ad.data = texVal;
                    n1 = gp->reg_count + 1;
                    gp->current->nloop |= 1;
                    gp->current += n1;
                    gp->count += n1;
                    /* Strip tag, then four ST, colour and vertex quadwords. */
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
                }
            }
        }
    }
    MGsGPTerm(pk);
}

extern void sefHitEffect(void);

/*
 * Advances the frame counter (the work's frame at +0x70) once per call. It fires
 * sefHitEffect at frame 30 and calls MEfObjDestroy at frame 40.
 */
static void fnAMP02_PO000(void *self, Amp02InitState *work)
{
    work->frame++;
    if (work->frame == 30) {
        sefHitEffect();
    }
    if (work->frame >= 40) {
        MEfObjDestroy(self);
    }
}



static const Quadword coltbl_1_00A515B0[10] = {
    0x28000000800000008000000080,
    0x30000000800000008000000080,
    0x30000000800000008000000080,
    0x30000000800000008000000080,
    0x30000000800000008000000080,
    0x28000000800000008000000080,
    0x28000000800000008000000080,
    0x20000000800000008000000080,
    0x18000000800000008000000080,
    0x10000000800000008000000080
};

static const float sztbl_2_00A51650[10] = { 0.15000000596046448f, 0.20000000298023224f, 0.23999999463558197f, 0.27000001072883606f, 0.30000001192092896f, 0.31999999284744263f, 0.34000000357627869f, 0.36000001430511475f, 0.34999999403953552f, 0.31999999284744263f };
