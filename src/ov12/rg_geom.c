/*
 * OV12 original TU 51: 0x00a2be48..0x00a2c288 (13 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_geom.h"
#include "ov12/rg_heap.h"

/* Shared by every accessor below: the assertion text "pGeom != NIL" and
   the original source file name of this unit, both in .rodata. */
const char geomAssertGeomNotNil[] = "pGeom != NIL";
const char geomSourcePath[] = "../rg_geom.euc.c";

/* Resets a geometry object to its defaults: no type, no parent, active,
   no event flag and no pass-time or destruct method. */
void RgGeomInit(RgGeom *pGeom)
{
    if (pGeom == 0) {
        assert_prog(geomAssertGeomNotNil, geomSourcePath, 0x12);
    }
    pGeom->type = -1;
    pGeom->parent = 0;
    pGeom->status = RG_GEOM_STATUS_ACTIVE;
    pGeom->eventFlag = 0;
    pGeom->passTimeMethod = 0;
    pGeom->destructMethod = 0;
}

/* Calls the destruct method when one is set, then frees the object to the heap. */
void RgGeomFree(RgGeom *pGeom)
{
    RgGeomDestructMethod destruct;

    if (pGeom == 0) {
        assert_prog(geomAssertGeomNotNil, geomSourcePath, 0x1D);
    }
    destruct = pGeom->destructMethod;
    if (destruct != 0) {
        destruct(pGeom);
    }
    RgHeapFree(InstanceOfRgHeap(), pGeom, geomSourcePath, 0x20);
}

void RgGeomSetParent(RgGeom *pGeom, void *parent)
{
    if (pGeom == 0) {
        assert_prog(geomAssertGeomNotNil, geomSourcePath, 0x27);
    }
    pGeom->parent = parent;
}

void RgGeomSetType(RgGeom *pGeom, int type)
{
    if (pGeom == 0) {
        assert_prog(geomAssertGeomNotNil, geomSourcePath, 0x2D);
    }
    pGeom->type = type;
}

void RgGeomSetPassTimeMeshod(RgGeom *pGeom, RgGeomPassTimeMethod method)
{
    if (pGeom == 0) {
        assert_prog(geomAssertGeomNotNil, geomSourcePath, 0x33);
    }
    pGeom->passTimeMethod = method;
}

void RgGeomSetDestructMethod(RgGeom *pGeom, RgGeomDestructMethod method)
{
    if (pGeom == 0) {
        assert_prog(geomAssertGeomNotNil, geomSourcePath, 0x39);
    }
    pGeom->destructMethod = method;
}

void RgGeomSetStatus(RgGeom *pGeom, unsigned int flags)
{
    if (pGeom == 0) {
        assert_prog(geomAssertGeomNotNil, geomSourcePath, 0x40);
    }
    pGeom->status |= flags;
}

void RgGeomResetStatus(RgGeom *pGeom, unsigned int flags)
{
    if (pGeom == 0) {
        assert_prog(geomAssertGeomNotNil, geomSourcePath, 0x46);
    }
    pGeom->status &= ~flags;
}

void *RgGeomGetParent(RgGeom *pGeom)
{
    if (pGeom == 0) {
        assert_prog(geomAssertGeomNotNil, geomSourcePath, 0x4E);
    }
    return pGeom->parent;
}

int RgGeomGetType(RgGeom *pGeom)
{
    if (pGeom == 0) {
        assert_prog(geomAssertGeomNotNil, geomSourcePath, 0x54);
    }
    return pGeom->type;
}

unsigned int RgGeomGetStatus(RgGeom *pGeom)
{
    if (pGeom == 0) {
        assert_prog(geomAssertGeomNotNil, geomSourcePath, 0x5B);
    }
    return pGeom->status;
}

int RgGeomGetEventFlag(RgGeom *pGeom)
{
    if (pGeom == 0) {
        assert_prog(geomAssertGeomNotNil, geomSourcePath, 0x62);
    }
    return pGeom->eventFlag;
}

/* Clears the event flag, then calls the pass-time method when one is set
   and the object is active. */
void RgGeomPassTime(RgGeom *pGeom, float deltaTime)
{
    RgGeomPassTimeMethod method;

    if (pGeom == 0) {
        assert_prog(geomAssertGeomNotNil, geomSourcePath, 0x6A);
    }
    method = pGeom->passTimeMethod;
    pGeom->eventFlag = 0;
    if (method != 0 && (pGeom->status & RG_GEOM_STATUS_ACTIVE) != 0) {
        method(pGeom, deltaTime);
    }
}
