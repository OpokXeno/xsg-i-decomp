/*
 * OV12 original TU 32: 0x00a1fa30..0x00a1fbd8 (3 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/xrg_rand_int.h"

/*
 * Opaque: the other TU (rg_effect_env.c) that owns RgEffectEnv still keeps
 * RgEffectEnvGetParticleData as scaffold asm, so it has no published
 * prototype yet (see rg_robot_effect.c for the same reading).
 */
typedef struct RgEffectEnv RgEffectEnv;
extern RgCharMgr *InstanceOfRgCharMgr(void);
RgEffectEnv *InstanceOfRgEffectEnv(void);
int RgCharMgrIsFullOfBuffer(RgCharMgr *manager, int count);
int RgEffectEnvGetParticleData(RgEffectEnv *env, char *name, void *buffer);

/*
 * Opaque: this TU only forwards a pointer to it, from a stack buffer this
 * function fills; see RgRobotEffectStartJet/RgRobotEffectStartDash
 * (rg_robot_effect.c).
 */
typedef struct RgParticleEffect RgParticleEffect;
typedef struct RgParticleEffectEssence RgParticleEffectEssence;
extern RgParticleEffect *CreateRgParticleEffect(RgParticleEffectEssence *essence,
                                                int context);
extern void CreateRgParticleEffectChar(RgParticleEffect *effect);
extern void RgParticleEffectSetShootLocal(RgParticleEffect *effect, RgMatrix local);

void CreateRgHitEffect(RgMatrix local, char *name1, char *name2) {
    /*
     * Raw particle-essence lookup buffer, opaque to this TU; see
     * RgRobotEffectStartJet (rg_robot_effect.c).
     */
    int buffer[0xB0];
    char *name;
    RgParticleEffect *particle;
    int shotCount;

    name = name1;
    if (name == 0 || name[0] == 0) {
        name = name2;
    }
    if (name != 0 && name[0] != 0 &&
        RgCharMgrIsFullOfBuffer(InstanceOfRgCharMgr(), 0x14) == 0) {
        shotCount = RgEffectEnvGetParticleData(InstanceOfRgEffectEnv(), name, buffer);
        if (shotCount != 0) {
            particle = CreateRgParticleEffect((RgParticleEffectEssence *) buffer, shotCount);
            if (particle != 0) {
                CreateRgParticleEffectChar(particle);
                RgParticleEffectSetShootLocal(particle, local);
            }
        }
    }
}

extern void XrgUnitMatrix(RgMatrix destination);

void CreateRgHitEffectPos(RgVector position, char *name1, char *name2) {
    RgMatrix local;

    XrgUnitMatrix(local);
    XrgCopyVector(local + 12, position);
    local[15] = 1.0f;
    CreateRgHitEffect(local, name1, name2);
}

extern void XrgCalcMatrixZtoY(RgMatrix matrix, RgVector zAxis, RgVector yAxis);
extern float *XrgVectorY(void);

void CreateRgHitEffectPosDir(RgVector position, RgVector direction, char *name1, char *name2) {
    RgMatrix local;

    XrgCalcMatrixZtoY(local, direction, XrgVectorY());
    XrgCopyVector(local + 12, position);
    local[15] = 1.0f;
    CreateRgHitEffect(local, name1, name2);
}
