#ifndef INCLUDE_OV12_RG_PARTICLE_EFFECT_H
#define INCLUDE_OV12_RG_PARTICLE_EFFECT_H

#include "shared.h"

typedef struct XrgParticle XrgParticle;

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

#endif /* INCLUDE_OV12_RG_PARTICLE_EFFECT_H */
