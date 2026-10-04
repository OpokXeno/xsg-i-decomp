/*
 * OV01 original TU 25: 0x00a34e90..0x00a358a0 (7 functions)
 */
#include "common.h"
#include "ov01/m_ef_create.h"
#include "shared.h"
#include "m_ef_create_smp_01.h"

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_smp_01", MEfCreate_SMP01);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_smp_01", updateSmoke);

extern void *MEfCalcAngle(Vector4 *destination, const Vector4 *from,
                          const Vector4 *to);
extern void *MMathApplyMatrix(Vector4 *destination, const Vector4 *matrix,
                              const Vector4 *point);
extern void MMathRotateMatrixYX(Vector4 *destination, const Vector4 *source,
                               const Vector4 *angles);
extern void updateSmoke(Smp01Work *work);

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
    {
        /* Keep the translation load's address in v1, as at 0x00a352ec. */
        register Vector4 *spawnPoint asm("$3") = &work->spawnPoint;

        __asm__ __volatile__("lqc2 vf1, 0(%0)" : : "r"(spawnPoint) : "memory");
    }
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

enum { SMP01_DRAW_FRAME_LIMIT = 15 };

extern Vector4 *MMathVectorInterpolation(Vector4 *destination, const Vector4 *first,
                                         const Vector4 *second, float parameter);

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
extern void MEfDrawModel(const Vector4 *place, int entry, const char *texture);
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

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_smp_01", fnSMP01_DP000);

extern void sefHitEffect(void);

static void fnSMP01_PO000(void *object, void *work)
{
    Smp01State *state = (Smp01State *)work;

    state->frame++;
    if (state->frame == 15) {
        sefHitEffect();
    }
    if (state->frame >= 25) {
        MEfObjDestroy(object);
    }
}
