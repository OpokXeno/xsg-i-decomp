/*
 * OV12 original TU 24: 0x00a17c90..0x00a18668 (19 functions)
 */
#include "common.h"
#include "shared.h"

/* Opaque handles this TU only ever passes through by pointer. */
typedef struct RgDispModel RgDispModel;
typedef struct RgParticleEffect RgParticleEffect;
typedef struct RgEffectEnv RgEffectEnv;
typedef struct RgBxx RgBxx;

/*
 * CreateRgShotEffect is the object's only allocator, and its RgHeapAlloc call
 * sizes it at 0xA0 bytes. _InitEffect (0x00a17c90) evidences the layout past
 * texLineModelHeight: it stores 0x7F into each of four ints at 0x30/0x34/
 * 0x38/0x3C, calls XrgUnitMatrix(p+0x40), and clears vectors at p+0x80 and
 * p+0x90; RgShotEffectSetTexLineModelColor copies a 16-byte quadword into
 * +0x30 and RgShotEffectSetPos copies a matrix into +0x40 and two vectors
 * into +0x80/+0x90. No claimed function touches 0x24..0x2F.
 */
typedef struct RgShotEffect {
    RgDispModel *dispModel;          /* 0x00 */
    float scale;                     /* 0x04 */
    float yRotation;                 /* 0x08 */
    float transparent;               /* 0x0C */
    RgParticleEffect *particleEffect; /* 0x10 */
    int particleShootReverse;        /* 0x14 */
    int texLinePicId;                /* 0x18 */
    float texLineModelWidth;         /* 0x1C */
    float texLineModelHeight;        /* 0x20 */
    unsigned char unmodeled_24[0xC]; /* 0x24, untouched by every claimed function */
    int texLineModelColor[4];        /* 0x30 */
    RgMatrix matrix;                 /* 0x40 */
    RgVector startPos;               /* 0x80 */
    RgVector endPos;                 /* 0x90 */
} RgShotEffect;

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size, const char *source_file,
                         int line);
extern RgEffectEnv *InstanceOfRgEffectEnv(void);
extern const char **RgEffectEnvGetShotMdl(RgEffectEnv *effectEnv);
extern RgBxx *RgEffectEnvGetBxx(RgEffectEnv *effectEnv);
extern int RgBxxGetPic(RgBxx *bxx, const char *name);
extern RgDispModel *CreateXrgDispModelImpl(const char *name, const char *variant);
extern void DisposeRgDispModel(RgDispModel *dispModel);
extern void RgDispModelSetMode(RgDispModel *dispModel, int mode);
extern void DisposeRgParticleEffect(RgParticleEffect *particleEffect);
extern void RgParticleEffectStopAlive(RgParticleEffect *particleEffect, float time);
extern int RgParticleEffectIsAlive(RgParticleEffect *particleEffect);
extern void RgParticleEffectPassTime(RgParticleEffect *particleEffect, float time);
extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file,
                       int line);
extern int RgEffectEnvGetParticleData(RgEffectEnv *effectEnv, char *name,
                                      void *buffer);
typedef struct RgParticleEffectEssence RgParticleEffectEssence;
extern RgParticleEffect *CreateRgParticleEffect(RgParticleEffectEssence *essence,
                                                int context);

/*
 * Linker witnesses for the original literals at ov12:0x00a53418,
 * 0x00a53428 and 0x00a53440; this TU's .rodata stays scaffold-owned
 * (config/tu-build.json).
 */

/*
 * Linker witnesses for the original literals at ov12:0x00a53418 and
 * 0x00a53428; this TU's .rodata stays scaffold-owned (config/tu-build.json).
 */
extern const char D_00A53418[];
extern const char D_00A53428[];
extern const char D_00A53440[];

/* The one evidenced RgDispModelSetMode mode this TU passes. */
#define RG_DISP_MODEL_MODE_ADD_ALPHA 4

static void _InitEffect(RgShotEffect *pEff);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot_effect", _InitEffect_00A17C90);

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

    pEff = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgShotEffect), D_00A53428, 75);
    if (pEff == 0) {
        assert_prog(D_00A53418, D_00A53428, 76);
    }
    _InitEffect(pEff);
    return pEff;
}

void DisposeRgShotEffect(RgShotEffect *pShotEff)
{
    if (pShotEff == 0) {
        assert_prog(D_00A53440, D_00A53428, 83);
    }
    _DestructEffect(pShotEff);
    RgHeapFree(InstanceOfRgHeap(), pShotEff, D_00A53428, 85);
}

void RgShotEffectStopAlive(RgShotEffect *pEff, float time)
{
    if (pEff == 0) {
        assert_prog(D_00A53418, D_00A53428, 96);
    }
    if (pEff->particleEffect != 0) {
        RgParticleEffectStopAlive(pEff->particleEffect, time);
    }
}

void RgShotEffectSetModel(RgShotEffect *pEff, const char *variant)
{
    const char **shotMdl;

    if (pEff == 0) {
        assert_prog(D_00A53418, D_00A53428, 108);
    }
    if (pEff->dispModel != 0) {
        DisposeRgDispModel(pEff->dispModel);
        pEff->dispModel = 0;
    }
    shotMdl = RgEffectEnvGetShotMdl(InstanceOfRgEffectEnv());
    if (shotMdl != 0) {
        pEff->dispModel = CreateXrgDispModelImpl(*shotMdl, variant);
    }
}

void RgShotEffectSetParticle(RgShotEffect *pEff, char *name)
{
    /* Raw particle-essence lookup buffer, opaque to this TU; see
     * RgRobotEffectStartJet (ov12/rg_robot_effect.c). */
    int shotData[0xB0];
    int shotCount;

    if (pEff == 0) {
        assert_prog(D_00A53418, D_00A53428, 127);
    }
    shotCount = RgEffectEnvGetParticleData(InstanceOfRgEffectEnv(), name, shotData);
    if (shotCount != 0) {
        pEff->particleEffect = CreateRgParticleEffect((RgParticleEffectEssence *) shotData, shotCount);
    }
}

void RgShotEffectSetParticleShootReverse(RgShotEffect *pEff)
{
    if (pEff == 0) {
        assert_prog(D_00A53418, D_00A53428, 139);
    }
    pEff->particleShootReverse = 0;
}

RgParticleEffect *RgShotEffectGetPtclEffect(RgShotEffect *pEff)
{
    if (pEff == 0) {
        assert_prog(D_00A53418, D_00A53428, 147);
    }
    return pEff->particleEffect;
}

void RgShotEffectSetTexLine(RgShotEffect *pEff, const char *name)
{
    RgBxx *bxx;

    bxx = RgEffectEnvGetBxx(InstanceOfRgEffectEnv());
    if (pEff == 0) {
        assert_prog(D_00A53418, D_00A53428, 157);
    }
    if (name != 0 && *name != 0) {
        pEff->texLinePicId = RgBxxGetPic(bxx, name);
    }
}

void RgShotEffectSetTexLineModelSize(RgShotEffect *pEff, float width, float height)
{
    if (pEff == 0) {
        assert_prog(D_00A53418, D_00A53428, 168);
    }
    pEff->texLineModelWidth = width;
    pEff->texLineModelHeight = height;
}

void RgShotEffectSetTexLineModelColor(RgShotEffect *pEff, const int *color)
{
    if (pEff == 0) {
        assert_prog(D_00A53418, D_00A53428, 177);
    }
    __asm__ __volatile__("lqc2 vf31, 0(%0)\n\tsqc2 vf31, 0(%1)"
                         : : "r"(color), "r"(pEff->texLineModelColor) : "memory");
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot_effect", RgShotEffectSetPos);

void RgShotEffectSetScale(RgShotEffect *pEff, float scale)
{
    if (pEff == 0) {
        assert_prog(D_00A53418, D_00A53428, 229);
    }
    pEff->scale = scale;
}

void RgShotEffectSetYRot(RgShotEffect *pEff, float yRotation)
{
    if (pEff == 0) {
        assert_prog(D_00A53418, D_00A53428, 237);
    }
    pEff->yRotation = yRotation;
}

void RgShotEffectSetTransparent(RgShotEffect *pEff, float transparent)
{
    if (pEff == 0) {
        assert_prog(D_00A53418, D_00A53428, 245);
    }
    pEff->transparent = transparent;
}

void RgShotEffectSetAddAlpha(RgShotEffect *pEff)
{
    if (pEff == 0) {
        assert_prog(D_00A53418, D_00A53428, 253);
    }
    if (pEff->dispModel != 0) {
        RgDispModelSetMode(pEff->dispModel, RG_DISP_MODEL_MODE_ADD_ALPHA);
    }
}

void RgShotEffectPassTime(RgShotEffect *pEff, float time)
{
    RgParticleEffect *ptclEffect;

    if (pEff == 0) {
        assert_prog(D_00A53418, D_00A53428, 267);
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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot_effect", RgShotEffectDisp);
