#ifndef INCLUDE_OV12_RG_GEOM_POLY_H
#define INCLUDE_OV12_RG_GEOM_POLY_H

#include "shared.h"
#include "ov12/rg_colidata.h"
#include "ov12/rg_geom.h"

/* Functions and data used by other translation units. */
RgGeom *CreateRgGeomPoly(void);
void InitRgGeomPoly(RgGeom *geom);
void RgGeomPolySetColiData(RgGeom *geom, RgColiData *coliData);
RgColiData *RgGeomPolyGetData(const RgGeom *geom);
void RgGeomPolySetLocal(RgGeom *geom, const RgMatrix local);
void RgGeomPolyGetLocal(const RgGeom *geom, RgMatrix destination);
void RgGeomPolyGetInvLocal(const RgGeom *geom, RgMatrix destination);
int RgGeomPolyCheckBall(RgGeom *pPoly, RgColiBall *pOrgArg,
                        RgColiBallResult *pResult);
int RgGeomPolyCheckRay(RgGeom *pPoly, RgColiSegment *pOrgArg,
                       RgColiSegmentHit *pResult);

#endif /* INCLUDE_OV12_RG_GEOM_POLY_H */
