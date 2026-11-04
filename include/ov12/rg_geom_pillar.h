#ifndef INCLUDE_OV12_RG_GEOM_PILLAR_H
#define INCLUDE_OV12_RG_GEOM_PILLAR_H

#include "shared.h"
#include "ov12/rg_geom.h"

/* Functions and data used by other translation units. */
void InitRgGeomPillar(RgGeom *pPillar);
RgGeom *CreateRgGeomPillar(void);
int RgGeomPillarCheckPoint(RgGeomTray *pillar, RgGeomBall *ball,
                           RgVector contact);
int RgGeomPillarCheckBall(RgGeomTray *pillar, RgGeomBall *ball,
                          RgVector worldResults[3]);

#endif /* INCLUDE_OV12_RG_GEOM_PILLAR_H */
