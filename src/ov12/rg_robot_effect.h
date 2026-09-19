/*
 * TU-local declarations of ov12/tu010 (src/ov12/rg_robot_effect.c).
 */

#ifndef SRC_OV12_RG_ROBOT_EFFECT_H
#define SRC_OV12_RG_ROBOT_EFFECT_H

#include "shared.h"

/* Only the pointer identity is evidenced (_AddEffect 0x00a0e268 stores
 * CreateRgParticleEffect's return, _ClearEffect 0x00a0e164 hands the same
 * pointer to DisposeRgParticleEffect); its layout is outside this
 * allocation. */
typedef struct RgParticleEffect RgParticleEffect;

typedef struct RgRobotEffect RgRobotEffect;

/*
 * Signature inferred from the one indirect call site, _PassTimeEffect
 * (0x00a0e374..0x00a0e3b4): generator, item index, and two caller buffers the
 * function fills, the first then passed to RgParticleEffectSetShootLocal and
 * the second to RgParticleEffectSetShootInertia; a nonzero return applies
 * them.  That caller and the two implementations
 * (_GetJetShootLocal 0x00a0e600, _GetDashShootLocal 0x00a0e688) are outside
 * this allocation, so this typedef documents the storage site
 * (_SetShootLocalGenEffect), not a verified call.
 */
typedef int (*RgRobotEffectShootLocalFunc)(RgRobotEffect *generator,
                                            int index, void *local,
                                            RgVector inertia);

/*
 * One robot effect "slot" (jet or dash).  RgRobotEffect embeds two of these,
 * 0x40 bytes apart (0x00a0e108.._SetInertiaEffect and the surrounding
 * accessors read/write only the fields below; _AddEffect 0x00a0e220,
 * _ClearEffect 0x00a0e130, _StopEffect 0x00a0e2a0, _PassTimeEffect
 * 0x00a0e320 and _DispEffect 0x00a0e480 are outside this allocation but
 * evidence particles/particle_count).
 */
typedef struct RgRobotEffectItem {
    RgParticleEffect *particles[2]; /* +0x00 */
    int particle_count;             /* +0x08 */
    float clear_time;               /* +0x0c: counts down; the item fully
                                      * clears at zero (_PassTimeEffect
                                      * 0x00a0e42c). */
    float stop_time;                /* +0x10: counts down; spawning stops at
                                      * zero (_PassTimeEffect 0x00a0e450). */
    int inertia;                    /* +0x14 */
    RgRobotEffect *shoot_generator; /* +0x18 */
    RgRobotEffectShootLocalFunc shoot_func; /* +0x1c */
    char ptcl_name[32];             /* +0x20: particle template name
                                      * (RgRobotEffectSetJetPtclName/
                                      * RgRobotEffectSetDashPtclName strcpy
                                      * into it; _InitRobEff clears its first
                                      * byte). The span past that first byte
                                      * is set by the fixed 0x40 stride
                                      * between the jet and dash items, not
                                      * independently observed. */
} RgRobotEffectItem;

struct RgRobotEffect {
    void *actor;              /* +0x00 */
    RgGeomPoint *geom;         /* +0x04 */
    unsigned char unmodeled_08[8]; /* +0x08 */
    RgRobotEffectItem jet;     /* +0x10 */
    RgRobotEffectItem dash;    /* +0x50 */
};

/*
 * _StopEffect (0x00a0e2a0) has local binding in the original symbol table.
 * RgRobotEffectTermJet calls it on the jet item (&effect->jet).
 */
static void _StopEffect(RgRobotEffectItem *item);

RgRobotEffect *CreateRgRobotEffect(void);
void DisposeRgRobotEffect(RgRobotEffect *effect);
void RgRobotEffectSetActor(RgRobotEffect *effect, void *actor);
void RgRobotEffectSetGeom(RgRobotEffect *effect, RgGeomPoint *geom);
void RgRobotEffectSetJetPtclName(RgRobotEffect *effect, const char *name);
void RgRobotEffectSetDashPtclName(RgRobotEffect *effect, const char *name);
void RgRobotEffectTermAll(RgRobotEffect *effect);

#endif /* SRC_OV12_RG_ROBOT_EFFECT_H */
