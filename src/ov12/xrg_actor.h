/*
 * TU-local declarations of ov12/tu080 (src/ov12/xrg_actor.c).
 */

#ifndef SRC_OV12_XRG_ACTOR_H
#define SRC_OV12_XRG_ACTOR_H

#include "shared.h"
#include "main/xgl_studio.h"

/*
 * The runtime actor instance every XrgActor wraps through its pXenoAct
 * field.  Its owner is the ACT_* animation module (ACT_initMotion at
 * 0x00307C48, ACT_initExMotion at 0x00308330, both in config/symbols/main.txt),
 * not this translation unit, so only the offsets this TU's own setters read
 * or write are named; the rest are unmodeled spans.
 */
typedef struct XenoAct {
    unsigned int flags;             /* 0x000: |= 0x408 drops the weapon, |= 0x800 marks it transparent */
    unsigned char unmodeled_004[0x008 - 0x004];
    void (*drawFunc)(struct XenoAct *pXenoAct); /* 0x008: _InitAfterLoadXenoActor installs
                                        _ActorDrawFunction here; the ACT_* module calls it
                                        back with this same XenoAct as its argument */
    unsigned char unmodeled_00c[0x090 - 0x00C];
    signed char lightCost[2];        /* 0x090: byte pair enabled by renderFlags 0x40;
                                        _InitAfterLoadXenoActor stores {7, 0x30},
                                        XrgActorSetLightCost {9, 0x70}; the engine
                                        meaning of each byte is not evidenced */
    unsigned char unmodeled_092[0x510 - 0x092];
    StudioLight light;                 /* 0x510: _InitAfterLoadXenoActor's
                                           xglLightSetDefault target */
    unsigned char unmodeled_600[0x6F4 - 0x600];
    float motionFrame;                /* 0x6F4 */
    unsigned char unmodeled_6f8[0x8D0 - 0x6F8];
    void *dupFileData;                 /* 0x8D0: resourceFiles[2]->data, cached by
                                           _DupLoadXenoActor after RgFileSysDup */
    void *auxFileData;                  /* 0x8D4: resourceFiles[1]->data, cached by
                                           _DupLoadXenoActor after RgFileSysRead */
    void *jointData;                     /* 0x8D8: resourceFiles[3]->data, cached by
                                           _DupLoadXenoActor after RgFileSysRead;
                                           _InitSkeMani reads a joint/element count
                                           from byte offset 6 of this data */
    unsigned char unmodeled_8dc[0x8FC - 0x8DC];
    int renderLevelFlag;               /* 0x8FC: _ActorDrawFunction raises the render level
                                           to 0x10 when this is nonzero; no writer in this
                                           allocation evidences its meaning further */
    unsigned char unmodeled_900[0x9A0 - 0x900];
    unsigned int renderFlags;          /* 0x9A0: |= 0x40 light cost, |= 0x3 transparent */
    unsigned int lightCostLevel;        /* 0x9A4: 0x12 from XrgActorSetLightCost; engine
                                           meaning not evidenced */
    unsigned char unmodeled_9a8[0x9C0 - 0x9A8];
    float transparency;                  /* 0x9C0 */
    unsigned char unmodeled_9c4[0x9D8 - 0x9C4];
    long long blendMode;                  /* 0x9D8: XrgActorSetTransparent stores 0x44, the
                                             GS ALPHA value for (Cs - Cd) * As + Cd */
} XenoAct;

/*
 * The game-object wrapper this TU defines.  Only the offsets its own
 * functions read or write are named here; the rest are unmodeled spans.
 * The full object is XRG_ACTOR_SIZE bytes, the RgHeapAlloc request in
 * CreateXrgActor and DuplicateXrgActor; the members past weaponMotionActive
 * are not modeled.
 */
#define XRG_ACTOR_SIZE 0x170

typedef struct XrgActor {
    XenoAct *pXenoAct;                /* 0x000 */
    unsigned char unmodeled_004[0x010 - 0x004];
    RgMatrix local;                     /* 0x010 */
    int smoothPlay;                      /* 0x050 */
    int loopPlay;                         /* 0x054 */
    int motionID;                          /* 0x058: -1 when no motion is set */
    int motionComaStep;                     /* 0x05C */
    float motionComaStepAccum;               /* 0x060: float, as _PassTime_00A45D30 reads and
                                                writes it with lwc1/swc1 */
    void *children;                            /* 0x064: the RgVector of attached child XrgActor
                                                   records XrgActorAttachChild pushes to and
                                                   XrgActorDettachChild removes from
                                                   (RgVectorPush/RgVectorRemove, ov12/tu050); its
                                                   element type belongs to that TU and is not
                                                   declared here */
    struct XrgActor *parent;                  /* 0x068 */
    int attachID;                              /* 0x06C: -1 when not attached to a parent */
    unsigned char unmodeled_070[0x104 - 0x070];
    RgFileSysData *resourceFiles[4];            /* 0x104: per-index roles beyond the load/cache
                                                   bookkeeping _DupLoadXenoActor performs are not
                                                   evidenced; index 3 is the joint/skeleton file
                                                   _InitSkeMani reads back through pXenoAct */
    int weaponMotionActive;                     /* 0x114 */
} XrgActor;

/*
 * The lightweight actor-selection record InitXrgActorEssence fills and
 * XrgActorEssenceGetFileName (0x00a47a18, still assembly) reads back.  Only
 * the offsets InitXrgActorEssence writes are named; the assert string
 * "pEss != NIL" (0x00a58b00) gives the parameter name this TU uses for it.
 */
typedef struct XrgActorEssence {
    int actorID;                /* 0x000 */
    char fileName[0x84 - 0x004]; /* 0x004: InitXrgActorEssence empties it by clearing byte 0,
                                    the same empty-string idiom _InitRgShotEssence uses on
                                    RgShotEssence.hitEffectFile (src/ov12/rg_shot.h) */
    int state;                    /* 0x084: InitXrgActorEssence clears it; no reader in this
                                     allocation evidences its meaning beyond that */
} XrgActorEssence;

#endif /* SRC_OV12_XRG_ACTOR_H */
