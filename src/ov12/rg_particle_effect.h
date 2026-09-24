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
typedef struct RgParticleShot RgParticleShot;

#define RG_PARTICLE_EFFECT_STATE_ACTIVE 1
#define RG_PARTICLE_EFFECT_STATE_LOCKED 2

/*
 * Bit 0 of RgParticleShot.flags: when the shot's life countdown runs out,
 * RgParticleEffectPassTime reloads it from lifeMax, reinitialises the shoot
 * record and emits the opening burst again instead of letting the shot end.
 * _InitEffect sets exactly this bit, from a flag of the essence at 0x9C.
 */
#define RG_PARTICLE_SHOT_REPEAT 1

/*
 * Further members of one shot slot, evidenced by _InitEffect (which fills a
 * fresh slot) and by RgParticleEffectPassTime (which steps it every frame):
 *
 *   0x10   the RgParticleShoot record _InitShoot fills and _ReInitShoot
 *          resets; _InitEffect calls _InitShoot with shots[i] + 0x10 and
 *          _ShootParticles reads the same record there. Its own localMatrix
 *          and force are the two vectors this header already names at 0x60
 *          and 0xF0, so the record cannot be named as one member here
 *          without restating them; only its first bytes carry the name.
 *   0x100  the life the countdown at 0x0C starts from: _InitEffect stores
 *          the essence value at 0x98 into both, and RgParticleEffectPassTime
 *          reloads 0x0C from it when the shot repeats.
 *   0x104  the flag word _InitEffect clears and sets to
 *          RG_PARTICLE_SHOT_REPEAT, and RgParticleEffectPassTime tests.
 *   0x108  the coefficient RgParticleEffectPassTime hands to
 *          XrgParticleDriverPassTimeReq, which the driver's particle step
 *          turns into the per-frame factor 1.0f - resist * deltaTime;
 *          _InitEffect copies it from the essence at 0xA0.
 *   0x10C  cleared by _InitEffect and set to 1 by RgParticleEffectPassTime
 *          once it has emitted the slot's opening burst.
 */

/*
 * One shot slot of RgParticleEffect's shots[] array (stride sizeof(this),
 * evidenced by RgParticleEffectSetShootLocal, RgParticleEffectSetShootInertia,
 * RgParticleEffectStopAlive and RgParticleEffectDisp all stepping the same
 * effect->shootCount-bounded array by 0x110 bytes): three ints
 * RgParticleEffectDisp forwards verbatim to XrgParticleDriverDispReq, a life
 * countdown RgParticleEffectStopAlive sets and RgParticleEffectDisp checks
 * (>= 0.0f) before issuing that request, a local matrix
 * RgParticleEffectSetShootLocal copies into, and an inertia vector
 * RgParticleEffectSetShootInertia copies into (src/ov12/rg_robot_effect.h
 * documents "local"/"inertia" as the names of these same two setters'
 * caller-side arguments). No member beyond those is claimed.
 */
struct RgParticleShot {
    int dispArg0;
    int dispArg1;
    int dispArg2;
    float life;
    u8 shoot[0x60 - 0x10];
    RgMatrix localMatrix;
    u8 unmodeled_a0[0xF0 - 0xA0];
    RgVector inertia;
    float lifeMax;
    int flags;
    float resist;
    int started;
};

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
/*
 * The observed access view: three floats InitRgParticleEffectControlData
 * resets to -1.0f (RgParticleEffectSetControlData, this TU's own unrecovered
 * setter, presumably assigns the live values) and two words it clears. No
 * member beyond 0x14 is claimed. Defined here, ahead of struct
 * RgParticleEffect, because RgParticleEffectSetControlData's inline copy
 * (member by member, see below) evidences this exact layout embedded in
 * RgParticleEffect's own controlData member too.
 */
struct RgParticleEffectControlData {
    float value[3];
    int flags;
    int counter;
};

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
