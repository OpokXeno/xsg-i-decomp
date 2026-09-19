/*
 * OV12 original TU 51: 0x00a2be48..0x00a2c288 (13 functions)
 */
#include "common.h"
#include "shared.h"

/*
 * RgGeom's complete layout is outside this allocation (the other OV12
 * translation units that read it, e.g. rg_geom_pillar.c, keep their own
 * partial view for the members they observe further in).  These are the
 * six words every RgGeom carries at its head, the ones this TU's own
 * accessors initialise, read and write: type, parent, a status bit mask,
 * an event flag and the two per-subclass method pointers RgGeomFree and
 * RgGeomPassTime call through.
 */
typedef void (*RgGeomPassTimeMethod)(void *pGeom, float deltaTime);
typedef void (*RgGeomDestructMethod)(void *pGeom);

struct RgGeom {
    int type;
    void *parent;
    unsigned int status;
    int eventFlag;
    RgGeomPassTimeMethod passTimeMethod;
    RgGeomDestructMethod destructMethod;
};

/* The status bit RgGeomInit sets and RgGeomPassTime tests before running the
   per-subclass pass-time method. */
#define RG_GEOM_STATUS_ACTIVE 1

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern void RgHeapFree(void *heap, void *pointer, const char *source_file,
                       int line);
extern RgHeap *InstanceOfRgHeap(void);

/* Referenced by every accessor below: the assertion text "pGeom != NIL" and
   this TU's own original file name "../rg_geom.euc.c", both scaffold-owned
   .rodata (not registered under a friendlier name in config/symbols/ov12.txt). */
extern const char D_00A550C8[];
extern const char D_00A550D8[];

void RgGeomInit(RgGeom *pGeom)
{
    if (pGeom == 0) {
        assert_prog(D_00A550C8, D_00A550D8, 0x12);
    }
    pGeom->type = -1;
    pGeom->parent = 0;
    pGeom->status = RG_GEOM_STATUS_ACTIVE;
    pGeom->eventFlag = 0;
    pGeom->passTimeMethod = 0;
    pGeom->destructMethod = 0;
}

void RgGeomFree(RgGeom *pGeom)
{
    RgGeomDestructMethod destruct;

    if (pGeom == 0) {
        assert_prog(D_00A550C8, D_00A550D8, 0x1D);
    }
    destruct = pGeom->destructMethod;
    if (destruct != 0) {
        destruct(pGeom);
    }
    RgHeapFree(InstanceOfRgHeap(), pGeom, D_00A550D8, 0x20);
}

void RgGeomSetParent(RgGeom *pGeom, void *parent)
{
    if (pGeom == 0) {
        assert_prog(D_00A550C8, D_00A550D8, 0x27);
    }
    pGeom->parent = parent;
}

void RgGeomSetType(RgGeom *pGeom, int type)
{
    if (pGeom == 0) {
        assert_prog(D_00A550C8, D_00A550D8, 0x2D);
    }
    pGeom->type = type;
}

void RgGeomSetPassTimeMeshod(RgGeom *pGeom, RgGeomPassTimeMethod method)
{
    if (pGeom == 0) {
        assert_prog(D_00A550C8, D_00A550D8, 0x33);
    }
    pGeom->passTimeMethod = method;
}

void RgGeomSetDestructMethod(RgGeom *pGeom, RgGeomDestructMethod method)
{
    if (pGeom == 0) {
        assert_prog(D_00A550C8, D_00A550D8, 0x39);
    }
    pGeom->destructMethod = method;
}

void RgGeomSetStatus(RgGeom *pGeom, unsigned int flags)
{
    if (pGeom == 0) {
        assert_prog(D_00A550C8, D_00A550D8, 0x40);
    }
    pGeom->status |= flags;
}

void RgGeomResetStatus(RgGeom *pGeom, unsigned int flags)
{
    if (pGeom == 0) {
        assert_prog(D_00A550C8, D_00A550D8, 0x46);
    }
    pGeom->status &= ~flags;
}

void *RgGeomGetParent(RgGeom *pGeom)
{
    if (pGeom == 0) {
        assert_prog(D_00A550C8, D_00A550D8, 0x4E);
    }
    return pGeom->parent;
}

int RgGeomGetType(RgGeom *pGeom)
{
    if (pGeom == 0) {
        assert_prog(D_00A550C8, D_00A550D8, 0x54);
    }
    return pGeom->type;
}

unsigned int RgGeomGetStatus(RgGeom *pGeom)
{
    if (pGeom == 0) {
        assert_prog(D_00A550C8, D_00A550D8, 0x5B);
    }
    return pGeom->status;
}

int RgGeomGetEventFlag(RgGeom *pGeom)
{
    if (pGeom == 0) {
        assert_prog(D_00A550C8, D_00A550D8, 0x62);
    }
    return pGeom->eventFlag;
}

void RgGeomPassTime(RgGeom *pGeom, float deltaTime)
{
    RgGeomPassTimeMethod method;

    if (pGeom == 0) {
        assert_prog(D_00A550C8, D_00A550D8, 0x6A);
    }
    method = pGeom->passTimeMethod;
    pGeom->eventFlag = 0;
    if (method != 0 && (pGeom->status & RG_GEOM_STATUS_ACTIVE) != 0) {
        method(pGeom, deltaTime);
    }
}
