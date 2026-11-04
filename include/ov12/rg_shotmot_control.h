#ifndef INCLUDE_OV12_RG_SHOTMOT_CONTROL_H
#define INCLUDE_OV12_RG_SHOTMOT_CONTROL_H

#include "ov12/rg_matrices_effector.h"

struct XrgActor;

/*
 * The arm-combine controller one RgShotThread owns while a shot motion plays:
 * a shadow copy of the parent actor and a copy of the attached weapon actor
 * that play the shot motion, blended into the parent by an arm-combine
 * effector.
 */
typedef struct RgShotMotCont {
    int active;                       /* 0x00 */
    struct XrgActor *m_pParent;       /* 0x04 */
    MatrixEffector *effector;         /* 0x08 */
    struct XrgActor *shadowActor;     /* 0x0c: its end of motion ends the shot motion */
    struct XrgActor *attachedActor;   /* 0x10: kept at the shadow actor's frame */
    int motion;                       /* 0x14: the motion id last played, -1 for none */
} RgShotMotCont;

/* Functions and data used by other translation units. */
RgShotMotCont *CreateRgShotMotCont(struct XrgActor *pParentActor,
                                   struct XrgActor *attachActor,
                                   unsigned int eArmType);
void DisposeRgShotMotCont(RgShotMotCont *pCont);
void RgShotMotContPlay(RgShotMotCont *pCont, int motion);
void RgShotMotContSetFrame(RgShotMotCont *pCont, float frame);
void RgShotMotContStop(RgShotMotCont *pCont);
int RgShotMotContIsActive(RgShotMotCont *pCont);
int RgShotMotContIsEndOfShadowMotion(RgShotMotCont *pCont);
void RgShotMotContPassTime(RgShotMotCont *pCont, float deltaTime);

#endif /* INCLUDE_OV12_RG_SHOTMOT_CONTROL_H */
