/*
 * OV12 original TU 56: 0x00a2dd58..0x00a2e698 (8 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_geom_tray.h"

#define RgGeomLocalMatricesAt(geom) \
    ((RgGeomLocalMatrices *)((unsigned char *)(geom) + 0x20))
#define RgGeomLocalMatricesConstAt(geom) \
    ((const RgGeomLocalMatrices *)((const unsigned char *)(geom) + 0x20))

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_tray", _InitRgGeomTray);

void InitRgGeomTray(RgGeom *geom)
{
    _InitRgGeomTray(geom);
}

extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size,
                         const char *source_file, int line);

RgGeom *CreateRgGeomTray(void)
{
    RgGeomTray *tray;

    tray = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgGeomTray), D_00A55228, 37);
    _InitRgGeomTray((RgGeom *)tray);
    return (RgGeom *)tray;
}

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

void RgGeomTraySetSize(void *tray, float width, float height)
{
    RgGeomTray *pTray = tray;

    if (pTray == 0) {
        assert_prog(D_00A55218, D_00A55228, 48);
    }
    if (width <= 0.0f) {
        width = 0.0f;
    }
    if (height <= 0.0f) {
        height = 0.0f;
    }
    pTray->width = width;
    pTray->height = height;
}

/*
 * The tray stores its local and inverse-local four-by-four matrices at byte
 * offsets 0x20 and 0x60.  The preceding geometry header is intentionally not
 * modelled here: these two routines only address the two evidenced matrix
 * objects and pass them to the established matrix helpers.
 */

void RgGeomTraySetLocal(void *tray, Matrix4 source)
{
    XrgCopyMatrix((float *)((char *)tray + 0x20), (const float *)source);
    XrgInvMatrix((float *)((char *)tray + 0x60), (const float *)source);
}

void RgGeomTrayGetLocal(void *tray, Matrix4 destination)
{
    XrgCopyMatrix((float *)destination, (const float *)((char *)tray + 0x20));
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_tray", _CalcIntersect);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_geom_tray", RgGeomTrayCheckBall);
