/*
 * TU-local declarations of ov12/tu033 (src/ov12/rg_effect_env.c).
 */

#ifndef SRC_OV12_RG_EFFECT_ENV_H
#define SRC_OV12_RG_EFFECT_ENV_H

#include "shared.h"

/* Opaque handle this TU only ever passes through by pointer. */
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
    struct RgFileSysData *shotMdl; /* +0x04 */
    struct RgFileSysData *particleData; /* +0x08 */
};

RgEffectEnv *InstanceOfRgEffectEnv(void);
void RgEffectEnvLoadData(RgEffectEnv *pEnv);
void RgEffectEnvDisposeData(RgEffectEnv *pEnv);
RgBxx *RgEffectEnvGetBxx(RgEffectEnv *pEnv);
struct RgFileSysData *RgEffectEnvGetShotMdl(RgEffectEnv *pEnv);

#endif /* SRC_OV12_RG_EFFECT_ENV_H */
