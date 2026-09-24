/*
 * TU-local declarations of ov12/tu056 (src/ov12/rg_geom_tray.c).
 */

#ifndef SRC_OV12_RG_GEOM_TRAY_H
#define SRC_OV12_RG_GEOM_TRAY_H

#include "shared.h"

void InitRgGeomTray(RgGeom *geom);

static void _InitRgGeomTray(RgGeom *geom);

void RgGeomTraySetLocal(void *tray, Matrix4 source);

void RgGeomTrayGetLocal(void *tray, Matrix4 destination);

/*
 * The original literals at ov12:0x00a55218 ("pTray != NIL") and 0x00a55228
 * ("../rg_geom_tray.euc.c") are scaffold-owned and have no entry in
 * config/symbols/ov12.txt, so they keep their splat names (docs/naming.md,
 * "Scaffold-owned data keeps its splat name").
 */
extern const char D_00A55218[];
extern const char D_00A55228[];

/*
 * Partial view of the tray's own 0xa0-byte allocation (CreateRgGeomTray).
 * RgGeomTraySetLocal and RgGeomTrayGetLocal already read/write the local and
 * inverse-local matrices at +0x20 and +0x60; RgGeomTraySetSize keeps the
 * tray's width and height immediately before them, at +0x18 and +0x1c.
 * _InitRgGeomTray also seeds width/height to 100.0f and both matrices to
 * identity (XrgUnitMatrix), which is the further evidence naming the two
 * matrix members below.
 */
typedef struct RgGeomTray {
    unsigned char unmodeled_00[0x18];
    float width;   /* +0x18: RgGeomTraySetSize */
    float height;  /* +0x1c: RgGeomTraySetSize */
    RgMatrix local;          /* +0x20: RgGeomTraySetLocal/GetLocal */
    RgMatrix inverse_local;  /* +0x60: RgGeomTraySetLocal */
} RgGeomTray;

RgGeom *CreateRgGeomTray(void);

void RgGeomTraySetSize(void *tray, float width, float height);

#endif /* SRC_OV12_RG_GEOM_TRAY_H */
