#ifndef INCLUDE_OV12_RG_GEOM_TRAY_H
#define INCLUDE_OV12_RG_GEOM_TRAY_H

#include "shared.h"
#include "ov12/rg_geom.h"

/*
 * The contact record RgGeomTrayCheckBall fills: the last contact point and the
 * clipped ball position (both world space), up to three contact normals (world
 * space), and their count at +0x50.
 */
typedef struct RgGeomTrayContact {
    RgVector last_contact;      /* +0x00 */
    RgVector clipped_position;  /* +0x10 */
    RgVector normals[3];        /* +0x20 */
    unsigned int hit_count;     /* +0x50 */
} RgGeomTrayContact;

/* Functions and data used by other translation units. */
void InitRgGeomTray(RgGeom *geom);
RgGeom *CreateRgGeomTray(void);
void RgGeomTraySetSize(void *tray, float width, float height);
void RgGeomTraySetLocal(void *tray, RgMatrix source);
void RgGeomTrayGetLocal(void *tray, RgMatrix destination);
int RgGeomTrayCheckBall(RgGeomTray *tray, RgGeomBall *ball,
                        RgGeomTrayContact *contact);

#endif /* INCLUDE_OV12_RG_GEOM_TRAY_H */
