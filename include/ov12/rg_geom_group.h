#ifndef INCLUDE_OV12_RG_GEOM_GROUP_H
#define INCLUDE_OV12_RG_GEOM_GROUP_H

#include "shared.h"
#include "ov12/rg_colidata.h"
#include "ov12/rg_geom.h"
#include "ov12/rg_vector.h"

/* A group owns a list of the geometries it created. */
typedef struct RgGeomGroup {
    RgPtrVector *m_pList;
} RgGeomGroup;

/* Functions and data used by other translation units. */
RgGeomGroup *CreateRgGeomGroup(void);
void DisposeRgGeomGroup(RgGeomGroup *pGroup);
RgGeom *RgGeomGroupCreatePoint(RgGeomGroup *pGroup, void *parent);
RgGeomBall *RgGeomGroupCreateBall(RgGeomGroup *pGroup, void *parent);
/* RgGeomGroupCreateRobot is defined void but its caller rg_player.c uses the
 * returned geometry, so rg_player.c declares the pointer return itself. */
RgGeom *RgGeomGroupCreatePoly(RgGeomGroup *pGroup, void *parent,
                              RgColiData *coliData);
RgGeom *RgGeomGroupCreateTray(RgGeomGroup *pGroup, void *parent);
RgGeom *RgGeomGroupCreatePiller(RgGeomGroup *pGroup, void *parent);
void RgGeomGroupDisposeElm(RgGeomGroup *pGroup, RgGeom *pElm);
void RgGeomGroupDebugDisp(RgGeomGroup *pGroup);

#endif /* INCLUDE_OV12_RG_GEOM_GROUP_H */
