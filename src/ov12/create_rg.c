/*
 * OV12 original TU 32: 0x00a1fa30..0x00a1fbd8 (3 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/create_rg.h"
#include "ov12/rg_charmgr.h"
#include "ov12/rg_effect_env.h"
#include "ov12/rg_particle_effect.h"
#include "ov12/xrg_rand_int.h"

/* Character-buffer slots that must be free before an effect is created. */
#define CREATE_RG_BUFFER_CHARS 0x14

/* Lookup buffer RgEffectEnvGetParticleData fills (0x2C0 bytes; its first 0xB0 are the essence). */
#define RG_PARTICLE_ESSENCE_WORDS 0xB0


void CreateRgHitEffect(RgMatrix local, const char *name1, const char *name2) {
    int essences[RG_PARTICLE_ESSENCE_WORDS];
    const char *name;
    RgParticleEffect *particle;
    int shotCount;

    name = name1;
    if (name == 0 || name[0] == 0) {
        name = name2;
    }
    if (name != 0 && name[0] != 0 &&
        RgCharMgrIsFullOfBuffer(InstanceOfRgCharMgr(), CREATE_RG_BUFFER_CHARS) == 0) {
        shotCount = RgEffectEnvGetParticleData(InstanceOfRgEffectEnv(), name, essences);
        if (shotCount != 0) {
            particle = CreateRgParticleEffect((RgParticleEffectEssence *) essences, shotCount);
            if (particle != 0) {
                CreateRgParticleEffectChar(particle);
                RgParticleEffectSetShootLocal(particle, local);
            }
        }
    }
}

void CreateRgHitEffectPos(RgVector position, const char *name1, const char *name2) {
    RgMatrix local;

    XrgUnitMatrix(local);
    XrgCopyVector(local + 12, position);
    local[15] = 1.0f;
    CreateRgHitEffect(local, name1, name2);
}

void CreateRgHitEffectPosDir(RgVector position, RgVector direction, const char *name1, const char *name2) {
    RgMatrix local;

    XrgCalcMatrixZtoY(local, direction, XrgVectorY());
    XrgCopyVector(local + 12, position);
    local[15] = 1.0f;
    CreateRgHitEffect(local, name1, name2);
}
