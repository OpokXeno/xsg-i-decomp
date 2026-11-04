#ifndef INCLUDE_OV12_XRG_ACTOR_H
#define INCLUDE_OV12_XRG_ACTOR_H

#include "shared.h"
#include "ov12/rg_light.h"
#include "ov12/rg_matrices_effector.h"

/* The engine actor record (main act_1.h), owned by the ACT_* module. */
typedef struct Actor Actor;

/* Bounds of the joint IDs XrgActorAttachChild accepts. */
#define RG_ACTOR_JNT_LEFT_HAND 0
#define RG_ACTOR_JNT_MAX 21

/* Effector banks per actor, and entries per bank. */
#define XRG_ACTOR_EFFECT_BANKS 2
#define XRG_ACTOR_EFFECT_MAX 8

/*
 * The game-object wrapper around one engine actor. Only the offsets its own
 * functions read or write are named; the rest are unmodeled spans.
 * The full object is XRG_ACTOR_SIZE bytes, the RgHeapAlloc request in
 * CreateXrgActor and DuplicateXrgActor; the members past pakaBaseJnt
 * are not modeled.
 */
#define XRG_ACTOR_SIZE 0x170

typedef struct XrgActor {
    Actor *pXenoAct;                  /* 0x000 */
    unsigned char unmodeled_004[0x010 - 0x004];
    RgMatrix local;                     /* 0x010 */
    int smoothPlay;                      /* 0x050 */
    int loopPlay;                         /* 0x054 */
    int motionID;                          /* 0x058: -1 when no motion is set */
    int motionComaStep;                     /* 0x05C */
    float motionComaStepAccum;               /* 0x060: float, as _PassTime reads and
                                                writes it with lwc1/swc1 */
    void *children;                            /* 0x064: the RgVector of attached child XrgActor
                                                   records XrgActorAttachChild pushes to and
                                                   XrgActorDettachChild removes from
                                                   (RgVectorPush/RgVectorRemove, ov12/tu050); its
                                                   element type belongs to that TU */
    struct XrgActor *parent;                  /* 0x068 */
    int attachID;                              /* 0x06C: -1 when not attached to a parent */
    int charID;                                /* 0x070: set by _LoadXenoActor; a parent's
                                                   charID is what XrgActorCheckPakaPaka passes
                                                   to _GetPakaPakaBaseJnt and
                                                   RgActorCharIDToName */
    MatrixEffector *effector[XRG_ACTOR_EFFECT_BANKS][XRG_ACTOR_EFFECT_MAX]; /* 0x074: two effector banks
                                                   (32 bytes each); _DisposeUseXenoActor
                                                   disposes every live entry of both banks */
    unsigned char unmodeled_0b4[0x0F4 - 0x0B4];
    int effectorCount[XRG_ACTOR_EFFECT_BANKS];   /* 0x0F4: live entries per bank */
    unsigned char unmodeled_0fc[0x104 - 0x0FC];
    RgFileSysData *resourceFiles[4];            /* 0x104: per-index roles beyond the load/cache
                                                   bookkeeping _DupLoadXenoActor performs are not
                                                   evidenced; index 3 is the joint/skeleton file
                                                   _InitSkeMani reads back through pXenoAct */
    int weaponMotionActive;                     /* 0x114 */
    int pakaPaka;                               /* 0x118: set to 1 by XrgActorCheckPakaPaka
                                                   once a paka base joint is found; cleared by
                                                   _InitXenoActor and _DupXenoActor */
    int pakaBaseJnt;                            /* 0x11C: _GetPakaPakaBaseJnt's result for
                                                   the parent's charID and this attachID */
    RgMatrix pakaBaseMatrix;                    /* 0x120 */
    int pakaBaseMatrixSet;                      /* 0x160: _LoadXenoActor clears it; _PassTime sets it
                                                   once pakaBaseMatrix is computed */
} XrgActor;

/*
 * The actor-selection record InitXrgActorEssence fills and
 * XrgActorEssenceGetFileName reads back (the assert string "pEss != NIL"
 * names the parameter).
 */
typedef struct XrgActorEssence {
    int actorID;                /* 0x000 */
    char fileName[0x84 - 0x004]; /* 0x004: InitXrgActorEssence empties it by clearing byte 0 */
    RgLight *light;               /* 0x084: the light _LoadXenoActor hands XrgActorSetLight;
                                     InitXrgActorEssence clears it to 0 (the default light) */
} XrgActorEssence;

/* Functions and data used by other translation units. */
int _ActJntToAccID_sub(int jntID, const char *sourceFile, int line);
int XrgActorJntIDtoAccID(int jntID);
XrgActor *CreateXrgActor(XrgActorEssence *pEss);
XrgActor *DuplicateXrgActor(XrgActor *pActor, XrgActor *pNewParent);
void DisposeXrgActor(XrgActor *pActor);
void XrgActorSetLightCost(XrgActor *pActor);
void XrgActorSetDropWeapon(XrgActor *pActor);
void XrgActorSetPlayMotionComaStep(XrgActor *pActor, int step);
void XrgActorSetLocal(XrgActor *pActor, const RgMatrix matrix);
void XrgActorSetLight(XrgActor *pActor, RgLight *light);
void XrgActorSetMotion(XrgActor *pActor, int motion);
void XrgActorSetSmoothPlay(XrgActor *pActor, int smoothPlay);
void XrgActorSetLoopPlay(XrgActor *pActor, int loopPlay);
void XrgActorDeriveMotion(XrgActor *pActor, XrgActor *pSrcActor);
void XrgActorSetMotionFrame(XrgActor *pActor, float frame);
void XrgActorSetDraw(XrgActor *pActor, int draw);
void XrgActorSetTransparent(XrgActor *pActor, float transparency);
int XrgActorIsEndOfMotion(XrgActor *pActor);
float XrgActorGetMotionLength(XrgActor *pActor);
float XrgActorGetPlayingMotTime(XrgActor *pActor);
void XrgActorGetLocal(XrgActor *pActor, RgMatrix matrix);
int XrgActorGetJointLocal(XrgActor *pActor, int jntID, RgMatrix matrix);
int XrgActorGetAttachedJoint(XrgActor *pActor);
void XrgActorAttachChild(XrgActor *pParent, XrgActor *pChild, int eJntID);
void XrgActorCheckPakaPaka(XrgActor *pActor);
void XrgActorDettachChild(XrgActor *pParent, XrgActor *pChild);
void XrgActorDraw(XrgActor *pActor);
void XrgActorPassTime(XrgActor *pActor, float dt);
void InitXrgActorEssence(XrgActorEssence *pEss, int actorID);
int XrgActorEssenceGetFileName(XrgActorEssence *pEss, char *pFileName);
MatrixEffector *XrgActorCreateEffConstraint(XrgActor *pActor,
                                            unsigned int eEffID,
                                            XrgActor *pWep);
MatrixEffector *XrgActorCreateEffArmCombine(XrgActor *pActor,
                                            XrgActor *pSrcActor,
                                            XrgActor *pWpn, XrgActor *pSubWpn,
                                            unsigned int eEffID);
void XrgActorDisposeEffector(XrgActor *pActor, MatrixEffector *pEffector);
void XrgActorDebugDisp(XrgActor *pActor);

#endif /* INCLUDE_OV12_XRG_ACTOR_H */
