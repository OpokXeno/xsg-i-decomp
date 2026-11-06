/*
 * OV01 original TU 25: 0x00a34e90..0x00a358a0 (7 functions)
 */
#include "common.h"
#include "ov01/m_ef_create.h"
#include "ov01/m_ef.h"
#include "shared.h"
#include "main/xgl_packet.h"
#include "m_ef_create_smp_01.h"

extern Vector4 *MEfCalcCircumXZ(Vector4 *destination, const Vector4 *center,
                                float radius);
extern float MMathMakeRandom2PI(void);
extern void MGsGPInit(MEfGsPacket *packet, void *address, int size);
extern Smp01ImageParameter *svGetPrmFromName(const char *name,
                                             Smp01ImageParameter *output);
extern const char efSmp01DeCld01Text[];
extern const char efSmp01DeCld02Text[];

static void fnSMP01_PR000(Smp01Object *object, Smp01Work *work);
static void fnSMP01_DM000(Smp01Object *self, Smp01Work *work);
static void fnSMP01_DP000(Smp01Object *self, Smp01Work *work);
static void fnSMP01_PO000(Smp01Object *object, Smp01Work *work);

/* Place the smoke at the actor, aim it at a point on a circle around the
 * target and reset the smoke trails. */
int MEfCreate_SMP01(MEfObjRecord *self)
{
    /* This constructor interprets its own variant of the pooled work. */
    Smp01Object *object = (Smp01Object *)self;
    Smp01Work *work = &object->work;
    Smp01ImageParameter parameter;
    Vector4 targetPoint;
    Matrix4 actorMatrix;

    work->frame = 0;
    work->phase = 0;
    work->phaseCount = 0;
    MEfGetActorMatrix(&actorMatrix, work->sourceActor, work->sourceCoordinate);
    MMathApplyMatrix(&work->spawnPoint, actorMatrix, &work->sourceOffset);
    MEfGetActorCoord(&targetPoint, work->targetActor, MEF_POINT_ACTOR);
    MEfCalcCircumXZ(&work->targetPosition, &targetPoint, 1.0f);
    __asm__ __volatile__("lq $8,0(%1)\n\tsq $8,0(%0)"
                         : : "r"(&work->position), "r"(&work->spawnPoint)
                         : "$8", "memory");
    __asm__ __volatile__("lq $8,0(%1)\n\tsq $8,0(%0)"
                         : : "r"(&work->previousPosition), "r"(&work->spawnPoint)
                         : "$8", "memory");
    MEfCalcAngle(&work->angles, &work->targetPosition, &work->spawnPoint);
    work->phaseAngle = MMathMakeRandom2PI();

    MGsGPInit(&work->packet, 0, 0);
    svGetPrmFromName(efSmp01DeCld01Text, &parameter);
    work->smokeTexture0First = parameter.texture;
    svGetPrmFromName(efSmp01DeCld02Text, &parameter);
    work->smokeTexture0Second = parameter.texture;
    memset(work->smokeActive, 0, sizeof(work->smokeActive));
    memset(work->smokePoints, 0, sizeof(work->smokePoints));
    memset(work->smokeAngles, 0, sizeof(work->smokeAngles));

    object->updateCallback = fnSMP01_PR000;
    object->drawCallback = fnSMP01_DM000;
    object->packetCallback = fnSMP01_DP000;
    object->postCallback = fnSMP01_PO000;
    return 1;
}

extern float MMathMakeRandom(void);
extern Vector4 *MMathVectorInterpolation(Vector4 *destination, const Vector4 *first,
                                         const Vector4 *second, float parameter);
extern Vector4 *MMathRotateMatrixYX(Vector4 *destination, const Vector4 *source,
                                    const Vector4 *angles);

/* Smoke is emitted for as many frames as the model is drawn. */
enum { SMP01_DRAW_FRAME_LIMIT = 15 };

/* Age every smoke trail by one slot and, while the model is still drawn,
 * emit a fresh trail of jittered points along the last movement step. */
static void updateSmoke(Smp01Work *work)
{
    Vector4 matrix[4];
    Vector4 offset;
    int trail;
    int point;

    for (trail = SMP01_SMOKE_TRAILS - 2; trail >= 0; trail--) {
        work->smokeActive[trail + 1] = work->smokeActive[trail];
        for (point = 0; point < SMP01_SMOKE_POINTS; point++) {
            __asm__ __volatile__("lq $8,0(%1)\n\tsq $8,0(%0)"
                                 :
                                 : "r"(&work->smokePoints[trail + 1][point]),
                                   "r"(&work->smokePoints[trail][point])
                                 : "$8", "memory");
            work->smokeAngles[trail + 1][point] = work->smokeAngles[trail][point];
        }
    }

    work->smokeActive[0] = 0;
    for (point = 0; point < SMP01_SMOKE_POINTS; point++) {
        __asm__ __volatile__("sqc2 vf0, 0(%0)"
                             :
                             : "r"(&work->smokePoints[0][point])
                             : "memory");
    }

    if (work->frame < SMP01_DRAW_FRAME_LIMIT) {
        work->smokeActive[0] = 1;
        for (point = 0; point < SMP01_SMOKE_POINTS; point++) {
            MMathVectorInterpolation(&offset, &work->position,
                                     &work->previousPosition,
                                     (float)point / 3.0f);
            MMathRotateMatrixYX(matrix, 0, &work->angles);
            __asm__ __volatile__("lqc2 vf1, 0(%0)" : : "r"(&offset) : "memory");
            __asm__ __volatile__("vmove.w vf1, vf0" : : : "memory");
            __asm__ __volatile__("sqc2 vf1, 48(%0)" : : "r"(matrix) : "memory");
            __asm__ __volatile__("sqc2 vf0, 0(%0)" : : "r"(&offset) : "memory");
            offset.x = MMathMakeRandom() * 0.06f - 0.03f;
            offset.y = MMathMakeRandom() * 0.06f - 0.03f;
            MMathApplyMatrix(&work->smokePoints[0][point], matrix, &offset);
            work->smokeAngles[0][point] = MMathMakeRandom2PI();
        }
    }
}

static void fnSMP01_PR010(Smp01Object *object, Smp01Work *work);

static void fnSMP01_PR000(Smp01Object *object, Smp01Work *work)
{
    Vector4 offset;
    Vector4 matrix[4];
    int phase;
    float sine;
    float cosine;

    phase = work->phase;
    work->phase = phase + 1;
    work->phaseCount = work->phaseCount - phase + 5;

    __asm__ __volatile__("lq $8, 0(%1)\n\t"
                         "sq $8, 0(%0)"
                         :
                         : "r"(&work->previousPosition), "r"(&work->position)
                         : "$8", "memory");
    __asm__ __volatile__("sqc2 vf0, 0(%0)" : : "r"(&offset) : "memory");

    __asm__ __volatile__("mfc1 $8, %1\n\t"
                         "qmtc2.ni $8, vf4\n\t"
                         "vcallms 0x20\n\t"
                         "qmfc2.i $8, vf1\n\t"
                         "mtc1 $8, %0"
                         : "=f"(sine)
                         : "f"(work->phaseAngle)
                         : "$8", "memory");
    offset.x = (float)work->phase * 0.06f * sine;

    __asm__ __volatile__("mfc1 $8, %1\n\t"
                         "qmtc2.ni $8, vf4\n\t"
                         "vcallms 0xe8\n\t"
                         "qmfc2.i $8, vf1\n\t"
                         "mtc1 $8, %0"
                         : "=f"(cosine)
                         : "f"(work->phaseAngle)
                         : "$8", "memory");
    offset.y = (float)work->phase * 0.06f * cosine;
    offset.z = (float)work->phaseCount * -0.02f;

    MMathRotateMatrixYX(matrix, 0, &work->angles);
    __asm__ __volatile__("lqc2 vf1, 0(%0)" : : "r"(&work->spawnPoint) : "memory");
    __asm__ __volatile__("vmove.w vf1, vf0" : : : "memory");
    __asm__ __volatile__("sqc2 vf1, 48(%0)" : : "r"(matrix) : "memory");
    MMathApplyMatrix(&work->position, matrix, &offset);

    if (work->phase >= 4) {
        updateSmoke(work);
    }

    if (work->phase >= 5) {
        work->phase = 0;
        __asm__ __volatile__("lq $8, 0(%1)\n\t"
                             "sq $8, 0(%0)"
                             :
                             : "r"(&work->phaseStart), "r"(&work->position)
                             : "$8", "memory");
        MEfCalcAngle(&work->angles, &work->targetPosition, &work->phaseStart);
        object->updateCallback = fnSMP01_PR010;
    }
}

static void fnSMP01_PR010(Smp01Object *object, Smp01Work *work)
{
    (void)object;
    if (work->frame < SMP01_DRAW_FRAME_LIMIT) {
        float phase;

        __asm__ __volatile__("lq $8, 0(%1)\n\t"
                             "sq $8, 0(%0)"
                             :
                             : "r"(&work->previousPosition), "r"(&work->position)
                             : "$8", "memory");
        phase = ((float)(work->phase + 1) / 10.0f) * 1.5707964f + 4.712389f;
        __asm__ __volatile__("mfc1 $8, %0\n\t"
                             "qmtc2.ni $8, vf4\n\t"
                             "vcallms 0x20\n\t"
                             "qmfc2.i $8, vf1\n\t"
                             "mtc1 $8, %0"
                             : "+f"(phase)
                             :
                             : "$8", "memory");
        MMathVectorInterpolation(&work->position, &work->phaseStart,
                                 &work->targetPosition, phase + 1.0f);
        work->angles.z += 0.5235988f;
    }
    updateSmoke(work);
    work->phase++;
}

extern Vector4 *MMathRotateMatrixYXZ(Vector4 *out, const Vector4 *matrix, const Vector4 *angles);
extern Vector4 *MMathScaleMatrix(Vector4 *out, const Vector4 *matrix, const Vector4 *scale);
static void fnSMP01_DM000(Smp01Object *self, Smp01Work *work)
{
    static const Vector4 scale = { 0.038f, 0.038f, 0.15f, 1.0f };
    Vector4 matrix[4];

    (void)self;
    if (work->frame < SMP01_DRAW_FRAME_LIMIT) {
        MMathRotateMatrixYXZ(matrix, 0, &work->angles);
        MMathScaleMatrix(matrix, matrix, &scale);
        __asm__ __volatile__("lqc2 vf1, 0(%0)" : : "r"(&work->position) : "memory");
        __asm__ __volatile__("vmove.w vf1, vf0" : : : "memory");
        __asm__ __volatile__("sqc2 vf1, 48(%0)" : : "r"(matrix) : "memory");
        MEfDrawModel(matrix, work->modelEntry, work->textureName);
    }
}

extern void MGsGPTerm(void *packet);
extern Quadword McEfSTQtbl[];
static const Quadword coltbl_1_00A514B0[10];
static const float sztbl_2_00A51550[10];

static void fnSMP01_DP000(Smp01Object *self, Smp01Work *work)
{
    int i;
    long regs;
    int nreg;
    float *szp;
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
    int lo;
    int nloop;
    int texReg;
    int off;
    unsigned char *base;

    pk = &work->packet;
    MGsGPInit(&work->packet, 0, 0);
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
    for (i = 0; i < SMP01_SMOKE_TRAILS; i++) {
        base = (unsigned char *)&work->sourceActor;
        if (((short *)(base + 256))[i] != 0) {
            color = coltbl_1_00A514B0[i];
            lo = i < 5;
            off = 248;
            if (lo) {
                off = 240;
            }
            tex = (unsigned long *)(base + off);
            for (j = 0; j < SMP01_SMOKE_POINTS; j++) {
                if (MEfMake4Vertex2(verts, &work->smokePoints[i][j], sztbl_2_00A51550[i], work->smokeAngles[i][j]) == 0) {
                    MEfGsQword *t;
                    MEfGsQword *g;
                    unsigned long texVal;
                    int n1;
                    int n2;

                    gp = &work->packet;
                    texReg = SCE_GS_TEX0_1;
                    regs = MEF_GS_TEXTURED_STRIP_REGS;
                    nloop = 3;
                    t = gp->current;
                    t[0].gif.tag = MEF_GS_SETUP_TAG;
                    t[0].gif.registers = MEF_GS_SETUP_REGS;
                    gp->reg_count = 1;
                    a1 = ++t;
                    texVal = *tex;
                    a1->ad.address = texReg;
                    a1->ad.data = texVal;
                    n1 = gp->reg_count + 1;
                    gp->current->nloop |= 1;
                    gp->current += n1;
                    gp->count += n1;
                    g = gp->current;
                    g[0].gif.tag = MEF_GS_TEXTURED_STRIP_TAG;
                    g[0].gif.registers = regs;
                    gp->reg_count = nloop;
                    (++g)->whole =McEfSTQtbl[0];
                    (++g)->whole =color;
                    (++g)->whole =*(Quadword *)&verts[0];
                    (++g)->whole =McEfSTQtbl[1];
                    (++g)->whole =color;
                    (++g)->whole =*(Quadword *)&verts[1];
                    (++g)->whole =McEfSTQtbl[2];
                    (++g)->whole =color;
                    (++g)->whole =*(Quadword *)&verts[2];
                    (++g)->whole =McEfSTQtbl[3];
                    (++g)->whole =color;
                    (++g)->whole =*(Quadword *)&verts[3];
                    n2 = gp->reg_count * 4 + 1;
                    gp->current->nloop |= 4;
                    gp->current += n2;
                    gp->count += n2;
                }
            }
        }
    }
    MGsGPTerm(&work->packet);
}


extern void sefHitEffect(void);

/* The frame sefHitEffect fires on and the frame the object's lifetime
 * expires on (fnSMP01_PO000). */
#define SMP01_HIT_FRAME 15
#define SMP01_LIFETIME  25

static void fnSMP01_PO000(Smp01Object *object, Smp01Work *work)
{
    work->frame++;
    if (work->frame == SMP01_HIT_FRAME) {
        sefHitEffect();
    }
    if (work->frame >= SMP01_LIFETIME) {
        MEfObjDestroy(object);
    }
}



const char efSmp01DeCld01Text[16] = "de_cld01";

const char efSmp01DeCld02Text[16] = "de_cld02";

static const Quadword coltbl_1_00A514B0[10] = {
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

static const float sztbl_2_00A51550[10] = { 0.10000000149011612f, 0.12999999523162842f, 0.15999999642372131f, 0.18000000715255737f, 0.20000000298023224f, 0.2199999988079071f, 0.23999999463558197f, 0.25999999046325684f, 0.2800000011920929f, 0.25f };
