#ifndef INCLUDE_OV12_RG_PARTICLE_EFFECT_H
#define INCLUDE_OV12_RG_PARTICLE_EFFECT_H

#include "shared.h"

typedef struct RgParticleEffectControlData RgParticleEffectControlData;

typedef struct XrgParticle XrgParticle;

typedef struct RgParticleShot RgParticleShot;

/*
 * Further members this allocation's own functions evidence: a shot count at
 * 0x0C bounding a shots[] array of four RgParticleShot slots from 0x10 to
 * 0x450 (RgParticleEffectSetShootLocal, RgParticleEffectSetShootInertia,
 * RgParticleEffectStopAlive and RgParticleEffectDisp all step the same
 * shootCount-bounded array by sizeof(RgParticleShot)), and, right after
 * shootActive, an inline block RgParticleEffectSetControlData copies its
 * argument into member by member (the same value[3]/flags/counter layout as
 * struct RgParticleEffectControlData above). No member or size beyond those
 * is claimed.
 */
struct RgParticleEffect {
    int state;
    XrgParticle *particles;
    u8 unmodeled_08[0x0C - 0x08];
    int shootCount;
    RgParticleShot shots[4];
    int shootActive;
    RgParticleEffectControlData controlData;
};

/*
 * The shoot template CreateRgParticleEffect's essence argument provides:
 * _InitShoot copies its position/direction vectors and its
 * interval/duration/speed/life scalars into a runtime RgParticleShoot. No
 * member outside those six is claimed.
 */
struct RgParticleEffectEssence {
    u8 unmodeled_00[0x50];
    RgVector position;
    RgVector direction;
    float interval;
    float duration;
    u8 unmodeled_78[0x90 - 0x78];
    float speed;
    float life;
};

#endif /* INCLUDE_OV12_RG_PARTICLE_EFFECT_H */
