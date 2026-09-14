/*
 * TU-local declarations of ov12/tu055 (src/ov12/rg_geom_poly.c).
 */

#ifndef SRC_OV12_RG_GEOM_POLY_H
#define SRC_OV12_RG_GEOM_POLY_H

#include "shared.h"

typedef struct RgGeomLocalMatrices {
    RgMatrix local;
    RgMatrix inverse_local;
} RgGeomLocalMatrices;

extern void XrgUnitMatrix(RgMatrix destination);

void RgGeomPolySetLocal(RgGeom *geom, const RgMatrix local);

void RgGeomPolyGetLocal(const RgGeom *geom, RgMatrix destination);

void RgGeomPolyGetInvLocal(const RgGeom *geom, RgMatrix destination);

#endif /* SRC_OV12_RG_GEOM_POLY_H */
