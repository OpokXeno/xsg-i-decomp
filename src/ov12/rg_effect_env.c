/*
 * OV12 original TU 33: 0x00a1fbd8..0x00a20040 (9 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_effect_env.h"
#include "ov12/rg_singleton_id.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_bxx.h"
#include "ov12/rg_filesys.h"
#include "ov12/rg_linkdata.h"
#include "ov12/rg_particle_effect.h"

/* Particle effect records returned per link-data entry, and their strides. */
#define RG_EFFECT_ENV_PARTICLE_SLOTS 4

/* One archive entry: the essence, then data nothing here reads, up to the 0x100-byte entry stride. */
typedef struct RgEffectEnvLinkEntry {
    RgParticleEffectEssence essence;
    u8 unmodeled_b0[0x100 - 0xB0];
} RgEffectEnvLinkEntry;

/* The link-data record and the essence slots are only forwarded, never read. */

const char effectEnvAssertEnvNotNil[] = "pEnv != NIL";
const char effectEnvSourcePath[] = "../rg_effect_env.euc.c";
const char effectEnvShotModelFileName[] = "shot.rbg";
const char effectEnvDataDirectory[] = "data\\nisimori\\";
const char effectEnvParticleDataFileName[] = "particle_effect.rpl";
const char effectEnvBxxFileName[] = "effect.bxx";

const char effectEnvAssertNameNotNil[] = "pszName != NIL";
const char effectEnvAssertResultNotNil[] = "aResult != NIL";

static void _InitEnv(RgEffectEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(effectEnvAssertEnvNotNil, effectEnvSourcePath, 25);
    }
    pEnv->bxx = 0;
    pEnv->shotMdl = 0;
    pEnv->particleData = 0;
}

static void _DestructEnv(RgEffectEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(effectEnvAssertEnvNotNil, effectEnvSourcePath, 33);
    }
    if (pEnv->bxx != 0) {
        DisposeRgBxx_sub(pEnv->bxx, effectEnvSourcePath, 36);
    }
    if (pEnv->shotMdl != 0) {
        DisposeRgFileSysData_sub(pEnv->shotMdl, effectEnvSourcePath, 39);
    }
    if (pEnv->particleData != 0) {
        DisposeRgFileSysData_sub(pEnv->particleData, effectEnvSourcePath, 42);
    }
    _InitEnv(pEnv);
}

static void _WrapperDestruct(void *pEnv)
{
    _DestructEnv(pEnv);
    RgHeapFree(InstanceOfRgHeap(), pEnv, effectEnvSourcePath, 46);
}

RgEffectEnv *InstanceOfRgEffectEnv(void)
{
    RgEffectEnv *pEnv;

    pEnv = RgSingletonIDGet(RG_SINGLETON_EFFECT_ENV);
    if (pEnv == 0) {
        pEnv = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgEffectEnv), effectEnvSourcePath, 56);
        _InitEnv(pEnv);
        RgSingletonIDEntry(RG_SINGLETON_EFFECT_ENV, pEnv, _WrapperDestruct);
    }
    return pEnv;
}

void RgEffectEnvLoadData(RgEffectEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(effectEnvAssertEnvNotNil, effectEnvSourcePath, 68);
    }
    if (pEnv->shotMdl == 0) {
        pEnv->shotMdl = RgFileSysRead(InstanceOfRgFileSys(), effectEnvShotModelFileName, effectEnvDataDirectory);
    }
    if (pEnv->particleData == 0) {
        pEnv->particleData = RgFileSysRead(InstanceOfRgFileSys(), effectEnvParticleDataFileName, effectEnvDataDirectory);
    }
    if (pEnv->bxx == 0) {
        pEnv->bxx = LoadRgBxx_sub(effectEnvBxxFileName, effectEnvSourcePath, 74);
    }
}

void RgEffectEnvDisposeData(RgEffectEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(effectEnvAssertEnvNotNil, effectEnvSourcePath, 82);
    }
    if (pEnv->bxx != 0) {
        DisposeRgBxx_sub(pEnv->bxx, effectEnvSourcePath, 84);
        pEnv->bxx = 0;
    }
    if (pEnv->shotMdl != 0) {
        DisposeRgFileSysData_sub(pEnv->shotMdl, effectEnvSourcePath, 88);
        pEnv->shotMdl = 0;
    }
    if (pEnv->particleData != 0) {
        DisposeRgFileSysData_sub(pEnv->particleData, effectEnvSourcePath, 92);
        pEnv->particleData = 0;
    }
}

RgBxx *RgEffectEnvGetBxx(RgEffectEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(effectEnvAssertEnvNotNil, effectEnvSourcePath, 104);
    }
    return pEnv->bxx;
}

RgFileSysData *RgEffectEnvGetShotMdl(RgEffectEnv *pEnv)
{
    if (pEnv == 0) {
        assert_prog(effectEnvAssertEnvNotNil, effectEnvSourcePath, 112);
    }
    return pEnv->shotMdl;
}

int RgEffectEnvGetParticleData(RgEffectEnv *pEnv, const char *pszName, void *aResult)
{
    RgFileSysData *particleData;
    RgLinkData linkData;
    RgEffectEnvLinkEntry *linkEntry;
    RgParticleEffectEssence *slot;
    int count;
    unsigned int index;

    if (pEnv == 0) {
        assert_prog(effectEnvAssertEnvNotNil, effectEnvSourcePath, 124);
    }
    if (pszName == 0) {
        assert_prog(effectEnvAssertNameNotNil, effectEnvSourcePath, 125);
    }
    if (aResult == 0) {
        assert_prog(effectEnvAssertResultNotNil, effectEnvSourcePath, 126);
    }
    particleData = pEnv->particleData;
    if (particleData == 0) {
        return 0;
    }
    InitRgLinkData(&linkData, particleData->data);
    linkEntry = RgLinkDataGet(&linkData, pszName);
    if (linkEntry == 0) {
        return 0;
    }
    slot = aResult;
    count = 0;
    index = 0;
    do {
        RgParticleEffectEssenceResume(slot, &linkEntry->essence);
        index++;
        if (slot->textureName[0] != 0) {
            count++;
        }
        linkEntry++;
        slot++;
    } while (index < RG_EFFECT_ENV_PARTICLE_SLOTS);
    return count;
}
