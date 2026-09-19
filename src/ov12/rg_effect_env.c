/*
 * OV12 original TU 33: 0x00a1fbd8..0x00a20040 (9 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_singleton_id.h"
#include "rg_effect_env.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(RgHeap *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file,
                       int line);

/*
 * RgBxx is defined by ov12/tu073 (src/ov12/rg_bxx.c); this TU only stores and
 * forwards the pointer LoadRgBxx_sub/DisposeRgBxx_sub return and take, so an
 * incomplete type is enough here.
 */
extern RgBxx *LoadRgBxx_sub(const char *pszName, const char *source_file,
                            int line);
extern void DisposeRgBxx_sub(RgBxx *pBxx, const char *source_file, int line);

/*
 * RgFileSys and RgFileSysData are defined by ov12/tu065 (src/ov12/rg_filesys.c);
 * this TU only stores and forwards the pointers InstanceOfRgFileSys/
 * RgFileSysRead return, so incomplete types are enough here.
 */
extern struct RgFileSys *InstanceOfRgFileSys(void);
extern struct RgFileSysData *RgFileSysRead(struct RgFileSys *pSys,
                                           const char *pszName,
                                           const char *pszRoot);
extern void DisposeRgFileSysData_sub(struct RgFileSysData *pFile,
                                     const char *source_file, int line);

/* File-backed OV12 witnesses, this TU's own .rodata (scaffold-owned, kept
 * under their splat names): 0x00a540c8 "pEnv != NIL", 0x00a540d8
 * "../rg_effect_env.euc.c", 0x00a540f0 "shot.rbg", 0x00a54100
 * "data\\nisimori\\", 0x00a54110 "particle_effect.rpl", 0x00a54128
 * "effect.bxx". */
extern const char D_00A540C8[];
extern const char D_00A540D8[];
extern const char D_00A540F0[];
extern const char D_00A54100[];
extern const char D_00A54110[];
extern const char D_00A54128[];

static void _InitEnv(RgEffectEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(D_00A540C8, D_00A540D8, 25);
    }
    pEnv->bxx = 0;
    pEnv->shotMdl = 0;
    pEnv->particleData = 0;
}

static void _DestructEnv(RgEffectEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(D_00A540C8, D_00A540D8, 33);
    }
    if (pEnv->bxx != 0) {
        DisposeRgBxx_sub(pEnv->bxx, D_00A540D8, 36);
    }
    if (pEnv->shotMdl != 0) {
        DisposeRgFileSysData_sub(pEnv->shotMdl, D_00A540D8, 39);
    }
    if (pEnv->particleData != 0) {
        DisposeRgFileSysData_sub(pEnv->particleData, D_00A540D8, 42);
    }
    _InitEnv(pEnv);
}

static void _WrapperDestruct(RgEffectEnv *pEnv)
{
    _DestructEnv(pEnv);
    RgHeapFree(InstanceOfRgHeap(), pEnv, D_00A540D8, 46);
}

RgEffectEnv *InstanceOfRgEffectEnv(void)
{
    RgEffectEnv *pEnv;

    pEnv = RgSingletonIDGet(8);
    if (pEnv == 0) {
        pEnv = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgEffectEnv), D_00A540D8, 56);
        _InitEnv(pEnv);
        RgSingletonIDEntry(8, (RgSimpleDB *) pEnv,
                           (void (*)(RgSimpleDB *)) _WrapperDestruct);
    }
    return pEnv;
}

void RgEffectEnvLoadData(RgEffectEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(D_00A540C8, D_00A540D8, 68);
    }
    if (pEnv->shotMdl == 0) {
        pEnv->shotMdl = RgFileSysRead(InstanceOfRgFileSys(), D_00A540F0, D_00A54100);
    }
    if (pEnv->particleData == 0) {
        pEnv->particleData = RgFileSysRead(InstanceOfRgFileSys(), D_00A54110, D_00A54100);
    }
    if (pEnv->bxx == 0) {
        pEnv->bxx = LoadRgBxx_sub(D_00A54128, D_00A540D8, 74);
    }
}

void RgEffectEnvDisposeData(RgEffectEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(D_00A540C8, D_00A540D8, 82);
    }
    if (pEnv->bxx != 0) {
        DisposeRgBxx_sub(pEnv->bxx, D_00A540D8, 84);
        pEnv->bxx = 0;
    }
    if (pEnv->shotMdl != 0) {
        DisposeRgFileSysData_sub(pEnv->shotMdl, D_00A540D8, 88);
        pEnv->shotMdl = 0;
    }
    if (pEnv->particleData != 0) {
        DisposeRgFileSysData_sub(pEnv->particleData, D_00A540D8, 92);
        pEnv->particleData = 0;
    }
}

RgBxx *RgEffectEnvGetBxx(RgEffectEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(D_00A540C8, D_00A540D8, 104);
    }
    return pEnv->bxx;
}

struct RgFileSysData *RgEffectEnvGetShotMdl(RgEffectEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(D_00A540C8, D_00A540D8, 112);
    }
    return pEnv->shotMdl;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_effect_env", RgEffectEnvGetParticleData);
