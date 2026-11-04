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

/*
 * Partial view of a ball's point body as RgGeomTrayCheckBall reads it: the
 * current and previous positions at +0x20 and +0x30 and the radius at +0x70,
 * where rg_geom_ball.c keeps it.  The bytes no function here reads stay
 * unmodeled.
 */
typedef struct RgGeomBallBody {
    unsigned char unmodeled_00[0x20];
    RgVector position;      /* +0x20 */
    RgVector old_position;  /* +0x30 */
    unsigned char unmodeled_40[0x30];
    float radius;           /* +0x70 */
} RgGeomBallBody;

/*
 * The contact record RgGeomTrayCheckBall fills: the last contact point and the
 * clipped ball position (both world space), up to three contact normals (world
 * space), and their count at +0x50.
 */
typedef struct RgGeomTrayContact {
    RgVector last_contact;      /* +0x00 */
    RgVector clipped_position;  /* +0x10 */
    RgVector normals[3];        /* +0x20 */
    int hit_count;              /* +0x50 */
} RgGeomTrayContact;

RgGeom *CreateRgGeomTray(void);

void RgGeomTraySetSize(void *tray, float width, float height);

int RgGeomTrayCheckBall(RgGeomTray *tray, RgGeomBallBody *ball,
                        RgGeomTrayContact *contact);

#endif /* SRC_OV12_RG_GEOM_TRAY_H */
