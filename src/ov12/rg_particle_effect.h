/*
 * TU-local declarations of ov12/tu035 (src/ov12/rg_particle_effect.c).
 */

#ifndef SRC_OV12_RG_PARTICLE_EFFECT_H
#define SRC_OV12_RG_PARTICLE_EFFECT_H

#include "shared.h"

typedef struct RgParticleEffect RgParticleEffect;
typedef struct RgParticleEffectEssence RgParticleEffectEssence;
typedef struct RgParticleShoot RgParticleShoot;
typedef struct RgParticleEffectControlData RgParticleEffectControlData;
typedef struct RgParticleChar RgParticleChar;
typedef struct XrgParticle XrgParticle;

#define RG_PARTICLE_EFFECT_STATE_ACTIVE 1
#define RG_PARTICLE_EFFECT_STATE_LOCKED 2

/*
 * The observed access view: a state word at 0x00 whose bit 0 marks the
 * effect active and bit 1 locks that state against RgParticleEffectSetActive
 * (RgParticleEffectIsActive reads (state & 3) == RG_PARTICLE_EFFECT_STATE_ACTIVE),
 * the heap-allocated particle array at 0x04 that _DestructEffect frees
 * through DisposeArrayOfXrgParticle when non-null, and the shoot-active flag
 * at 0x450 that RgParticleEffectStopShoot clears. CreateRgParticleEffect
 * allocates 0x470 bytes for it (RgHeapAlloc); no member or size beyond what
 * is listed here is claimed.
 */
struct RgParticleEffect {
    int state;
    XrgParticle *particles;
    u8 unmodeled_08[0x450 - 0x08];
    int shootActive;
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

/*
 * The runtime shoot state _InitShoot fills from the essence: the same
 * interval/duration/speed values, the essence's life value copied into both
 * a running total and its cap, the same position/direction vectors, two
 * counters _InitShoot resets to zero, two identity transforms XrgUnitMatrix
 * seeds, a flag word _InitShoot clears, and a force vector XrgClearVector
 * zeroes. No member outside those is claimed.
 */
struct RgParticleShoot {
    float interval;
    float duration;
    float speed;
    float life;
    float lifeMax;
    u8 unmodeled_14[0x20 - 0x14];
    RgVector position;
    RgVector direction;
    int shotCount;
    int elapsedFrames;
    u8 unmodeled_48[0x50 - 0x48];
    RgMatrix localMatrix;
    RgMatrix worldMatrix;
    int flags;
    u8 unmodeled_d4[0xE0 - 0xD4];
    RgVector force;
};

/*
 * The observed access view: three floats InitRgParticleEffectControlData
 * resets to -1.0f (RgParticleEffectSetControlData, this TU's own unrecovered
 * setter, presumably assigns the live values) and two words it clears. No
 * member beyond 0x14 is claimed.
 */
struct RgParticleEffectControlData {
    float value[3];
    int flags;
    int counter;
};

/*
 * The observed access view: the particle effect handle at 0x1C that
 * _DestructPtclChar disposes through DisposeRgParticleEffect and clears.
 * CreateRgParticleEffectChar (this TU's own unrecovered function) allocates
 * this object at RgCharAlloc(0x20, 4): 0x1C is exactly the size of RgChar
 * (src/ov12/rg_char.h, ov12/tu002), so this is plausibly an RgChar-derived
 * character with the effect handle appended, but that base is this TU's own
 * reading, not a shared layout. No member beyond 0x1C is claimed.
 */
struct RgParticleChar {
    u8 unmodeled_00[0x1C];
    RgParticleEffect *effect;
};

RgParticleEffect *CreateRgParticleEffect(RgParticleEffectEssence *essence,
                                         int context);
void RgParticleEffectStopShoot(RgParticleEffect *effect);
void RgParticleEffectSetActive(RgParticleEffect *effect, int active);
int RgParticleEffectIsActive(RgParticleEffect *effect);
void InitRgParticleEffectControlData(RgParticleEffectControlData *pData);

#endif /* SRC_OV12_RG_PARTICLE_EFFECT_H */
