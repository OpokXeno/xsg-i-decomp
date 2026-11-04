#ifndef INCLUDE_OV12_RG_EFFECT_ENV_H
#define INCLUDE_OV12_RG_EFFECT_ENV_H

#include "shared.h"

/*
 * RgBxx is defined by ov12/tu073 (src/ov12/rg_bxx.c); this allocation only
 * stores and forwards the pointer LoadRgBxx_sub returns and DisposeRgBxx_sub
 * takes, so an incomplete type is enough here.
 */
typedef struct RgBxx RgBxx;

typedef struct RgEffectEnv RgEffectEnv;

/*
 * The per-battle shot-effect resources: the BXX texture archive
 * ("effect.bxx") and the two file-system reads ("shot.rbg" and
 * "particle_effect.rpl", both under "data\nisimori\"). RgEffectEnvLoadData
 * stores each one only once (checked against 0 first); RgEffectEnvDisposeData
 * and _DestructEnv release all three and clear bxx and shotMdl back to 0.
 * RgEffectEnvGetBxx and RgEffectEnvGetShotMdl read bxx and shotMdl;
 * particleData is read by RgEffectEnvGetParticleData.
 */
struct RgEffectEnv {
    RgBxx *bxx;                    /* +0x00 */
    RgFileSysData *shotMdl;        /* +0x04 */
    RgFileSysData *particleData;   /* +0x08 */
};

/* Functions and data used by other translation units. */
RgEffectEnv *InstanceOfRgEffectEnv(void);
void RgEffectEnvLoadData(RgEffectEnv *pEnv);
void RgEffectEnvDisposeData(RgEffectEnv *pEnv);
RgBxx *RgEffectEnvGetBxx(RgEffectEnv *pEnv);
RgFileSysData *RgEffectEnvGetShotMdl(RgEffectEnv *pEnv);
int RgEffectEnvGetParticleData(RgEffectEnv *pEnv, const char *pszName, void *aResult);

#endif /* INCLUDE_OV12_RG_EFFECT_ENV_H */
