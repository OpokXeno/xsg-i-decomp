#ifndef INCLUDE_OV12_RG_GAME_COLLISION_H
#define INCLUDE_OV12_RG_GAME_COLLISION_H

#include "shared.h"
#include "ov12/rg_handler.h"

/* The queue of group pairs checked each frame (private layout). */
typedef struct RgGameCollision RgGameCollision;

/* A collision callback; the miss callbacks receive a null collision record. */
typedef void (*RgGameColiCallback)(RgGeom *geom, RgBgCollision *collision);

/*
 * A queued collision-check argument: the two geometry groups to test against
 * each other, with the callbacks run when a member of one is hit by the other
 * and the callbacks run for the members of a group that nothing hit.
 * _CreateColiArg copies all six words into the queued entry; the group
 * members name their group by tag, so RgGeomGroup keeps its single
 * definition in ov12/rg_geom_group.h.
 */
typedef struct RgGameColiArg {
    struct RgGeomGroup *m_pGroup1;     /* +0x00 */
    RgGameColiCallback hitCallback1;   /* +0x04: a group 1 member was hit */
    RgGameColiCallback missCallback1;  /* +0x08: group 1 members, nothing hit */
    struct RgGeomGroup *m_pGroup2;     /* +0x0C */
    RgGameColiCallback hitCallback2;   /* +0x10: a group 2 member was hit */
    RgGameColiCallback missCallback2;  /* +0x14: group 2 members, nothing hit */
} RgGameColiArg;

/* Functions and data used by other translation units. */
RgGameCollision *CreateRgGameCollision(void);
void DisposeRgGameCollision(RgGameCollision *pGameColi);
void RgGameCollisionAdd(RgGameCollision *pGameColi, RgGameColiArg *pArg);
void RgGameCollisionClear(RgGameCollision *pGameColi);
void RgGameCollisionJob(RgGameCollision *pGameColi);
void InitRgGeomGroupColiArg(RgGameColiArg *pArg);
void RgGeomGroupCollision(RgGameColiArg *pArg);

#endif /* INCLUDE_OV12_RG_GAME_COLLISION_H */
