#ifndef INCLUDE_OV12_RG_PARTICLE_EFFECT_H
#define INCLUDE_OV12_RG_PARTICLE_EFFECT_H

#include "shared.h"
#include "ov12/rg_char.h"
#include "ov12/xrg_particle.h"

typedef struct RgParticleEffect RgParticleEffect;
typedef struct RgParticleEffectEssence RgParticleEffectEssence;
typedef struct RgParticleShoot RgParticleShoot;
typedef struct RgParticleEffectControlData RgParticleEffectControlData;
typedef struct RgParticleChar RgParticleChar;
typedef struct RgParticleShot RgParticleShot;

/* RgParticleEffect.state bits. */
#define RG_PARTICLE_EFFECT_STATE_ACTIVE 1
#define RG_PARTICLE_EFFECT_STATE_LOCKED 2

/* A control timer that is not running (a timer only counts down while it is above zero). */
#define RG_PARTICLE_TIMER_OFF (-1.0f)

/* One effect runs at most this many shots, and drives at most this many particles. */
#define RG_PARTICLE_SHOT_MAX 4
#define RG_PARTICLE_TOTAL_MAX 4096

/*
 * Bit 0 of RgParticleShot.flags: when the shot's life countdown runs out,
 * RgParticleEffectPassTime reloads it from lifeMax, reinitialises the shoot
 * record and emits the opening burst again instead of letting the shot end.
 */
#define RG_PARTICLE_SHOT_REPEAT 1

/*
 * The shoot template CreateRgParticleEffect takes: _InitShoot copies its
 * appearance, spread and emission values into a runtime RgParticleShoot.
 */
struct RgParticleEffectEssence {
    char textureName[0x40];
    int particleCount;
    u8 unmodeled_44[0x50 - 0x44];
    RgVector appearance;        /* a new particle's size, intensity and angles */
    RgVector appearanceRate;    /* their change per second */
    float interval;
    float duration;
    u8 unmodeled_78[0x80 - 0x78];
    RgVector unitDirection;
    float speed;
    float life;
    float shotLife;
    int repeat;
    float resist;
    u8 unmodeled_a4[0xB0 - 0xA4];
};

/*
 * The runtime emission state of one shot, filled by _InitShoot from the
 * essence and stepped by _ShootParticles.
 */
struct RgParticleShoot {
    float interval;
    float duration;
    float speed;
    float life;
    float lifeMax;
    u8 unmodeled_14[0x20 - 0x14];
    RgVector appearance;
    RgVector appearanceRate;
    float emitRemainder;        /* fraction of a particle carried to the next frame */
    XrgParticle *cursor;        /* the next particle slot to overwrite */
    u8 unmodeled_48[0x50 - 0x48];
    RgMatrix localMatrix;       /* the emitter's matrix */
    RgMatrix previousMatrix;    /* the emitter's matrix at the last emission */
    int hasPrevious;            /* previousMatrix holds a valid matrix */
    u8 unmodeled_d4[0xE0 - 0xD4];
    RgVector inertia;
};

/*
 * One shot slot of RgParticleEffect.shots[], sizeof() 0x110: the slice of
 * the effect's particle array it drives, the picture they are drawn with
 * and its emission state.
 */
struct RgParticleShot {
    XrgParticle *particles;
    int particleCount;
    struct RgBxxPic *pic;
    float life;
    RgParticleShoot shoot;
    float lifeMax;              /* the life the countdown restarts from */
    int flags;                  /* RG_PARTICLE_SHOT_REPEAT */
    float resist;               /* passed to the driver as the velocity damping */
    int started;                /* the opening burst has been emitted */
};

/* Three countdown timers and the words around them. */
struct RgParticleEffectControlData {
    float timer[3];     /* start delay, time until shooting stops, time until the effect locks */
    int flags;
    int counter;
};

struct RgParticleEffect {
    int state;                  /* RG_PARTICLE_EFFECT_STATE_* */
    XrgParticle *particles;
    int particleTotal;
    int shootCount;
    RgParticleShot shots[RG_PARTICLE_SHOT_MAX];
    int shootActive;
    RgParticleEffectControlData controlData;
};

/* The character that owns an effect and draws it. */
struct RgParticleChar {
    RgChar base;
    RgParticleEffect *effect;
};

/* Functions and data used by other translation units. */
RgParticleEffect *CreateRgParticleEffect(RgParticleEffectEssence *essence,
                                         int essenceCount);
void DisposeRgParticleEffect(RgParticleEffect *effect);
void InitRgParticleEffectEssence(RgParticleEffectEssence *essence);
void RgParticleEffectEssenceSerialize(RgParticleEffectEssence *essence,
                                      RgParticleEffectEssence *destination);
void RgParticleEffectEssenceResume(RgParticleEffectEssence *essence,
                                   RgParticleEffectEssence *source);
void RgParticleEffectSetShootLocal(RgParticleEffect *effect, RgMatrix local);
void RgParticleEffectSetShootInertia(RgParticleEffect *effect,
                                     RgVector inertia);
void RgParticleEffectStopShoot(RgParticleEffect *effect);
void RgParticleEffectStopAlive(RgParticleEffect *effect, float time);
void RgParticleEffectSetActive(RgParticleEffect *effect, int active);
void InitRgParticleEffectControlData(RgParticleEffectControlData *pData);
void RgParticleEffectSetControlData(RgParticleEffect *effect,
                                    RgParticleEffectControlData *pSrc);
int RgParticleEffectIsAlive(RgParticleEffect *effect);
int RgParticleEffectIsActive(RgParticleEffect *effect);
void RgParticleEffectPassTime(RgParticleEffect *effect, float deltaTime);
void RgParticleEffectDisp(RgParticleEffect *effect);
void CreateRgParticleEffectChar(RgParticleEffect *effect);

#endif /* INCLUDE_OV12_RG_PARTICLE_EFFECT_H */
