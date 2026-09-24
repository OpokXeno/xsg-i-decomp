#ifndef INCLUDE_OV12_RG_ROBOT_EFFECT_H
#define INCLUDE_OV12_RG_ROBOT_EFFECT_H

#include "shared.h"

/*
 * Opaque: this TU only forwards a pointer to it, from a stack buffer this
 * function fills; see RgRobotEffectStartJet/RgRobotEffectStartDash
 * (rg_robot_effect.c).
 */
typedef struct RgParticleEffect RgParticleEffect;

/* Opaque here: rg_robot_effect.c (ov12/tu010) owns the RgRobotEffect definition. */
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

#endif /* INCLUDE_OV12_RG_ROBOT_EFFECT_H */
