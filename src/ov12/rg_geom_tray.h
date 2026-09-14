/*
 * TU-local declarations of ov12/tu056 (src/ov12/rg_geom_tray.c).
 */

#ifndef SRC_OV12_RG_GEOM_TRAY_H
#define SRC_OV12_RG_GEOM_TRAY_H

#include "shared.h"

void InitRgGeomTray(RgGeom *geom);

extern void _InitRgGeomTray(RgGeom *geom);

void RgGeomTraySetLocal(void *tray, Matrix4 source);

void RgGeomTrayGetLocal(void *tray, Matrix4 destination);

#endif /* SRC_OV12_RG_GEOM_TRAY_H */
