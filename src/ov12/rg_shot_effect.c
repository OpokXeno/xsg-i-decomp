#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_bxx.h"
#include "ov12/rg_effect_env.h"
#include "ov12/rg_shot_effect.h"
#include "ov12/xrg_rand_int.h"
#include "ov12/rg_dispmodel.h"
#include "ov12/xrg_paint2d.h"


const char shotEffectAssertEffNotNil[] = "pEff != NIL";

const char shotEffectSourcePath[] = "../rg_shot_effect.euc.c";

const char shotEffectAssertShotEffNotNil[] = "pShotEff != NIL";


/* The one evidenced RgDispModelSetMode mode this TU passes. */
#define RG_DISP_MODEL_MODE_ADD_ALPHA 4

static void _InitEffect(RgShotEffect *pEff)
{
    if (pEff == 0) {
        assert_prog(shotEffectAssertEffNotNil, shotEffectSourcePath, 47);
    }
    pEff->scale = 0.1f;
    pEff->transparent = 1.0f;
    pEff->dispModel = 0;
    pEff->particleEffect = 0;
    pEff->texLinePic = 0;
    pEff->yRotation = 0.0f;
    XrgUnitMatrix(pEff->matrix);
    XrgClearVector(pEff->startPos);
    XrgClearVector(pEff->endPos);
    pEff->texLineModelWidth = 1.0f;
    pEff->particleShootReverse = 1;
    pEff->texLineModelHeight = 0.2f;
    pEff->texLineModelColor.a = 0x7F;
    pEff->texLineModelColor.b = 0x7F;
    pEff->texLineModelColor.g = 0x7F;
    pEff->texLineModelColor.r = 0x7F;
}

static void _DestructEffect(RgShotEffect *pEff)
{
    if (pEff->dispModel != 0) {
        DisposeRgDispModel(pEff->dispModel);
    }
    if (pEff->particleEffect != 0) {
        DisposeRgParticleEffect(pEff->particleEffect);
    }
}

RgShotEffect *CreateRgShotEffect(void)
{
    RgShotEffect *pEff;

    pEff = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgShotEffect), shotEffectSourcePath, 75);
    if (pEff == 0) {
        assert_prog(shotEffectAssertEffNotNil, shotEffectSourcePath, 76);
    }
    _InitEffect(pEff);
    return pEff;
}

void DisposeRgShotEffect(RgShotEffect *pShotEff)
{
    if (pShotEff == 0) {
        assert_prog(shotEffectAssertShotEffNotNil, shotEffectSourcePath, 83);
    }
    _DestructEffect(pShotEff);
    RgHeapFree(InstanceOfRgHeap(), pShotEff, shotEffectSourcePath, 85);
}

void RgShotEffectStopAlive(RgShotEffect *pEff, float time)
{
    if (pEff == 0) {
        assert_prog(shotEffectAssertEffNotNil, shotEffectSourcePath, 96);
    }
    if (pEff->particleEffect != 0) {
        RgParticleEffectStopAlive(pEff->particleEffect, time);
    }
}

void RgShotEffectSetModel(RgShotEffect *pEff, const char *variant)
{
    RgFileSysData *shotMdl;

    if (pEff == 0) {
        assert_prog(shotEffectAssertEffNotNil, shotEffectSourcePath, 108);
    }
    if (pEff->dispModel != 0) {
        DisposeRgDispModel(pEff->dispModel);
        pEff->dispModel = 0;
    }
    shotMdl = RgEffectEnvGetShotMdl(InstanceOfRgEffectEnv());
    if (shotMdl != 0) {
        pEff->dispModel = CreateXrgDispModelImpl(shotMdl->data, variant);
    }
}

void RgShotEffectSetParticle(RgShotEffect *pEff, char *name)
{
    RgParticleEffectEssence essences[RG_PARTICLE_SHOT_MAX];
    int shotCount;

    if (pEff == 0) {
        assert_prog(shotEffectAssertEffNotNil, shotEffectSourcePath, 127);
    }
    shotCount = RgEffectEnvGetParticleData(InstanceOfRgEffectEnv(), name,
                                           essences);
    if (shotCount != 0) {
        pEff->particleEffect = CreateRgParticleEffect(essences, shotCount);
    }
}

void RgShotEffectSetParticleShootReverse(RgShotEffect *pEff)
{
    if (pEff == 0) {
        assert_prog(shotEffectAssertEffNotNil, shotEffectSourcePath, 139);
    }
    pEff->particleShootReverse = 0;
}

RgParticleEffect *RgShotEffectGetPtclEffect(RgShotEffect *pEff)
{
    if (pEff == 0) {
        assert_prog(shotEffectAssertEffNotNil, shotEffectSourcePath, 147);
    }
    return pEff->particleEffect;
}

void RgShotEffectSetTexLine(RgShotEffect *pEff, const char *name)
{
    RgBxx *bxx;

    bxx = RgEffectEnvGetBxx(InstanceOfRgEffectEnv());
    if (pEff == 0) {
        assert_prog(shotEffectAssertEffNotNil, shotEffectSourcePath, 157);
    }
    if (name != 0 && *name != 0) {
        pEff->texLinePic = RgBxxGetPic(bxx, name);
    }
}

void RgShotEffectSetTexLineModelSize(RgShotEffect *pEff, float width, float height)
{
    if (pEff == 0) {
        assert_prog(shotEffectAssertEffNotNil, shotEffectSourcePath, 168);
    }
    pEff->texLineModelWidth = width;
    pEff->texLineModelHeight = height;
}

void RgShotEffectSetTexLineModelColor(RgShotEffect *pEff, const XrgPaint2DRGBA *color)
{
    if (pEff == 0) {
        assert_prog(shotEffectAssertEffNotNil, shotEffectSourcePath, 177);
    }
    __asm__ __volatile__("lqc2 vf31, 0(%0)\n\tsqc2 vf31, 0(%1)"
                         : : "r"(color), "r"(&pEff->texLineModelColor) : "memory");
}

void RgShotEffectSetPos(RgShotEffect *pEff, RgVector startPos, RgVector endPos)
{
    RgVector direction;
    RgMatrix localMatrix;

    if (pEff == 0) {
        assert_prog(shotEffectAssertEffNotNil, shotEffectSourcePath, 189);
    }
    __asm__ __volatile__("lqc2 vf31, 0(%0)\n\t"
                         "sqc2 vf31, 0(%1)"
                         :
                         : "r"(startPos), "r"(pEff->startPos)
                         : "memory");
    __asm__ __volatile__("lqc2 vf31, 0(%0)\n\t"
                         "sqc2 vf31, 0(%1)"
                         :
                         : "r"(endPos), "r"(pEff->endPos)
                         : "memory");
    __asm__ __volatile__("lqc2 vf30, 0(%1)\n\t"
                         "lqc2 vf31, 0(%2)\n\t"
                         "vsub.xyzw vf30, vf30, vf31\n\t"
                         "sqc2 vf30, 0(%0)"
                         :
                         : "r"(direction), "r"(endPos), "r"(startPos)
                         : "memory");
    if (direction[0] * direction[0] + direction[1] * direction[1] +
            direction[2] * direction[2] < 0.005f) {
        XrgSetVectorXYZ(direction, 1.0f, 0.0f, 0.0f);
    }
    XrgCalcMatrixZtoY(localMatrix, direction, XrgVectorY());
    XrgCopyVectorXYZ(&localMatrix[12], endPos);
    XrgCopyMatrix(pEff->matrix, localMatrix);
    if (pEff->particleEffect != 0) {
        if (pEff->particleShootReverse != 0) {
            XrgNegateVector(&localMatrix[4], &localMatrix[4]);
            XrgNegateVector(&localMatrix[8], &localMatrix[8]);
            localMatrix[7] = 0.0f;
            localMatrix[11] = 0.0f;
        } else {
            XrgCopyVectorXYZ(&localMatrix[12], startPos);
        }
        RgParticleEffectSetShootLocal(pEff->particleEffect, localMatrix);
    }
}

void RgShotEffectSetScale(RgShotEffect *pEff, float scale)
{
    if (pEff == 0) {
        assert_prog(shotEffectAssertEffNotNil, shotEffectSourcePath, 229);
    }
    pEff->scale = scale;
}

void RgShotEffectSetYRot(RgShotEffect *pEff, float yRotation)
{
    if (pEff == 0) {
        assert_prog(shotEffectAssertEffNotNil, shotEffectSourcePath, 237);
    }
    pEff->yRotation = yRotation;
}

void RgShotEffectSetTransparent(RgShotEffect *pEff, float transparent)
{
    if (pEff == 0) {
        assert_prog(shotEffectAssertEffNotNil, shotEffectSourcePath, 245);
    }
    pEff->transparent = transparent;
}

void RgShotEffectSetAddAlpha(RgShotEffect *pEff)
{
    if (pEff == 0) {
        assert_prog(shotEffectAssertEffNotNil, shotEffectSourcePath, 253);
    }
    if (pEff->dispModel != 0) {
        RgDispModelSetMode(pEff->dispModel, RG_DISP_MODEL_MODE_ADD_ALPHA);
    }
}

void RgShotEffectPassTime(RgShotEffect *pEff, float time)
{
    RgParticleEffect *ptclEffect;

    if (pEff == 0) {
        assert_prog(shotEffectAssertEffNotNil, shotEffectSourcePath, 267);
    }
    ptclEffect = pEff->particleEffect;
    if (ptclEffect != 0) {
        RgParticleEffectPassTime(ptclEffect, time);
        if (RgParticleEffectIsAlive(ptclEffect) == 0) {
            DisposeRgParticleEffect(ptclEffect);
            pEff->particleEffect = 0;
        }
    }
}

void RgShotEffectDisp(RgShotEffect *pEff)
{
    RgMatrix matrix;
    XrgPaint2DLine3D line;
    RgVector diff;
    XrgPaint2D *paint2D;

    if (pEff == 0) {
        assert_prog(shotEffectAssertEffNotNil, shotEffectSourcePath, 284);
    }
    if (pEff->dispModel != 0) {
        XrgUnitMatrix(matrix);
        matrix[10] = matrix[5] = matrix[0] = pEff->scale;
        XrgRotMatrixY(matrix, matrix, pEff->yRotation);
        XrgMulMatrix(matrix, pEff->matrix, matrix);
        RgDispModelSetLocal(pEff->dispModel, matrix);
        if (pEff->transparent < 1.0f) {
            RgDispModelSetTransparent(pEff->dispModel, pEff->transparent);
        }
        RgDispModelDisplay(pEff->dispModel);
    }
    if (pEff->particleEffect != 0) {
        RgParticleEffectDisp(pEff->particleEffect);
    }
    if (pEff->texLinePic != 0) {
        paint2D = InstanceOfXrgPaint2D();
        XrgPaint2DAlpha(paint2D, 1);
        XrgPaint2DUseTexture(paint2D, pEff->texLinePic);
        InitXrgPaint2DLine3D(&line, 1, pEff->texLineModelHeight);
        __asm__ __volatile__("lqc2 vf31, 0(%0)\n\t"
                             "sqc2 vf31, 0(%1)"
                             :
                             : "r"(pEff->endPos), "r"(line.start)
                             : "memory");
        XrgSubVector(diff, pEff->startPos, pEff->endPos);
        XrgScaleVectorXYZ(diff, diff, pEff->texLineModelWidth);
        XrgAddVector(line.end, line.start, diff);
        XrgPaint2DColor(paint2D, &pEff->texLineModelColor);
        XrgPaint2DDrawLine(paint2D, &line);
    }
}
