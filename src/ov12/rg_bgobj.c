/*
 * OV12 original TU 17: 0x00a129c8..0x00a13200 (16 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_char.h"
#include "ov12/rg_debug_flags.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

/*
 * External file-backed witnesses, not candidate-emitted data: this window is
 * asm-owned scaffold data (splat names, no config/symbols/ov12.txt entry).
 *
 * ov12:0x00a529b0 contains the assertion expression "pObj != NIL".
 * ov12:0x00a529c0 contains the source filename "../rg_bgobj.euc.c".
 */
extern const char D_00A529B0[];
extern const char D_00A529C0[];

/* Scaffold-owned .data (ov12:0x00a4f638). */
extern float s_fBgObjBlight;

/* The RgChar type tag CreateRgBgObj passes to RgCharAlloc for a background object. */
#define RG_CHAR_TYPE_BGOBJ 2

/*
 * RgBgObj::flags bits. RgBgObjIgnoreHide sets or clears bit 0, which
 * RgBgObjSetHide tests; RgBgObjEnableBodyAttack sets or clears bit 1.
 */
#define RGBGOBJ_FLAG_IGNORE_HIDE (1 << 0)
#define RGBGOBJ_FLAG_BODY_ATTACK_ENABLED (1 << 1)

/*
 * A background object extends RgChar: CreateRgBgObj allocates it with
 * RgCharAlloc(sizeof(RgBgObj), RG_CHAR_TYPE_BGOBJ), so its own fields begin
 * right after RgChar's 0x1C bytes. The float at 0x34 is used only by
 * _InitRgBgObj, _PassTime and _Draw; its role is not established, so it
 * stays unmodeled.
 */
typedef struct RgBgObj {
    RgChar rgChar;                  /* 0x00 */
    unsigned int flags;             /* 0x1C */
    void *dispModel;                /* 0x20 */
    RgGeom *geomTray;               /* 0x24 */
    unsigned int hideState;         /* 0x28 */
    float hideDuration;             /* 0x2C */
    float hideElapsed;              /* 0x30 */
    float transparent;              /* 0x34 */
    float hardness;                 /* 0x38 */
    int notUseGeomLocal;            /* 0x3C */
} RgBgObj;

/*
 * _InitRgBgObj sets the field at 0x34 to -1.0f to disable it; _Draw reads it
 * and, when it is not negative, passes it to RgDispModelSetTransparent as
 * the dispModel's transparency override.
 */

/* Defined later in this TU. */
static void _InitRgBgObj(RgBgObj *pObj, void *dispModel, RgGeom *geomTray);
static void _Draw(RgBgObj *pObj);
void _PassTime(RgChar *pChar, float deltaTime);
static void _Destruct(RgBgObj *pObj);

/* Defined by other TUs. */
extern void DisposeRgDispModel(void *dispModel);
extern void RgGeomFree(RgGeom *geom);
extern void RgGeomSetParent(RgGeom *pGeom, void *parent);
extern void RgGeomTraySetLocal(void *tray, Matrix4 source);
extern void RgGeomTrayGetLocal(void *tray, Matrix4 destination);
extern void RgDispModelSetMode(void *dispModel, int mode);
extern void RgDispModelSetLocal(void *dispModel, Matrix4 local);
extern void RgDispModelSetTransparent(void *dispModel, float transparent);
extern void RgDispModelDisplay(void *dispModel);
extern RgDebugFlags *InstanceOfRgDebugFlags(void);
extern void XrgUnitMatrix(RgMatrix destination);
extern void CreateRgHitEffectPos(RgVector position, const char *effectFile,
                                 const char *defaultEffectFile);
extern unsigned char D_00A529D8[];

static void _InitRgBgObj(RgBgObj *pObj, void *dispModel, RgGeom *geomTray)
{
    if (pObj == 0) {
        assert_prog(D_00A529B0, D_00A529C0, 70);
    }
    RgCharDispMethod(&pObj->rgChar, (void (*)(RgChar *)) _Draw);
    RgCharPassTimeMethod(&pObj->rgChar, _PassTime);
    RgCharDestructMethod(&pObj->rgChar, (void (*)(RgChar *)) _Destruct);
    pObj->dispModel = dispModel;
    pObj->geomTray = geomTray;
    if (dispModel != 0) {
        RgDispModelSetMode(dispModel, 8);
    }
    pObj->flags = 0;
    pObj->hideState = 0;
    pObj->hideElapsed = 0.0f;
    pObj->hideDuration = 0.0f;
    pObj->transparent = -1.0f;
    pObj->notUseGeomLocal = 0;
    pObj->hardness = -1.0f;
    if (geomTray != 0) {
        RgGeomSetParent(geomTray, pObj);
    }
}

RgBgObj *CreateRgBgObj(void *dispModel, RgGeom *geomTray)
{
    RgBgObj *pObj;

    pObj = (RgBgObj *)RgCharAlloc(sizeof(RgBgObj), RG_CHAR_TYPE_BGOBJ);
    if (pObj == 0) {
        assert_prog(D_00A529B0, D_00A529C0, 110);
    }
    _InitRgBgObj(pObj, dispModel, geomTray);
    return pObj;
}

static void _Destruct(RgBgObj *pObj)
{
    if (pObj == 0) {
        assert_prog(D_00A529B0, D_00A529C0, 122);
    }
    if (pObj->dispModel != 0) {
        DisposeRgDispModel(pObj->dispModel);
    }
    if (pObj->geomTray != 0) {
        RgGeomFree(pObj->geomTray);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_bgobj", _PassTime_00A12B90);

/*
 * InstanceOfRgDebugFlags()->flags[1] is one of the individual debug toggles
 * _InitRgDebugFlags enables by default (include/ov12/rg_debug_flags.h), so
 * background objects draw normally unless a developer disables it.
 */
static void _Draw(RgBgObj *pObj)
{
    void *dispModel;
    Matrix4 local;
    float transparent;

    if (InstanceOfRgDebugFlags()->flags[1] != 0) {
        if (pObj == 0) {
            assert_prog(D_00A529B0, D_00A529C0, 179);
        }
        if (pObj->hideState != 2) {
            dispModel = pObj->dispModel;
            if (dispModel != 0) {
                if (pObj->notUseGeomLocal != 0) {
                    XrgUnitMatrix((float *)local);
                } else {
                    RgGeomTrayGetLocal(pObj->geomTray, local);
                }
                RgDispModelSetLocal(dispModel, local);
                transparent = pObj->transparent;
                if (transparent >= 0.0f) {
                    RgDispModelSetTransparent(dispModel, transparent);
                }
                RgDispModelDisplay(dispModel);
            }
        }
    }
}

void RgBgObjSetLocal(RgBgObj *pObj, Matrix4 local)
{
    if (pObj == 0) {
        assert_prog(D_00A529B0, D_00A529C0, 205);
    }
    RgGeomTraySetLocal(pObj->geomTray, local);
}

void RgBgObjSetHardness(RgBgObj *pObj, float hardness)
{
    if (pObj == 0) {
        assert_prog(D_00A529B0, D_00A529C0, 215);
    }
    pObj->hardness = hardness;
}

void RgBgObjSetHide(RgBgObj *pObj, float duration)
{
    if (pObj == 0) {
        assert_prog(D_00A529B0, D_00A529C0, 226);
    }

    if (!(pObj->flags & RGBGOBJ_FLAG_IGNORE_HIDE)) {
        unsigned int hideState;

        hideState = pObj->hideState;
        pObj->hideDuration = duration;
        switch (hideState) {
        case 2:
            break;

        case 0:
            pObj->hideElapsed = 0.0f;
            /* fallthrough */
        case 1:
        case 3:
            hideState = 1;
            break;
        }
        pObj->hideState = hideState;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_bgobj", RgBgObjIgnoreHide);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_bgobj", RgBgObjEnableBodyAttack);

void RgBgObjNotUseGeomLocal(RgBgObj *pObj)
{
    if (pObj == 0) {
        assert_prog(D_00A529B0, D_00A529C0, 272);
    }
    pObj->notUseGeomLocal = 1;
}

void *RgBgObjGetDispModel(RgBgObj *pObj)
{
    if (pObj == 0) {
        assert_prog(D_00A529B0, D_00A529C0, 283);
    }
    return pObj->dispModel;
}

int RgBgObjTryToBreak(int bgObject, float damage)
{
    RgBgObj *pObj = (RgBgObj *)bgObject;
    Matrix4 local;

    if (pObj == 0) {
        assert_prog(D_00A529B0, D_00A529C0, 294);
    }
    if (pObj->hardness < 0.0f) {
        return 0;
    }
    if (pObj->hardness < damage) {
        if (pObj->geomTray != 0) {
            RgGeomTrayGetLocal(pObj->geomTray, local);
            local[3][1] += 2.0f;
            CreateRgHitEffectPos(local[3], D_00A529D8, 0);
        }
        RgCharFree(&pObj->rgChar);
    } else {
        pObj->hardness -= damage;
    }
    return 1;
}

void RgBgObjTryToBodyAttack(void *bgObject, float energy)
{
    RgBgObj *pObj = (RgBgObj *)bgObject;
    Matrix4 local;

    if (pObj == 0) {
        assert_prog(D_00A529B0, D_00A529C0, 324);
    }
    if (energy < 288000.0f) {
        return;
    }
    if (!(pObj->flags & RGBGOBJ_FLAG_BODY_ATTACK_ENABLED)) {
        return;
    }
    if (pObj->geomTray != 0) {
        RgGeomTrayGetLocal(pObj->geomTray, local);
        local[3][1] += 2.0f;
        CreateRgHitEffectPos(local[3], D_00A529D8, 0);
    }
    RgCharFree(&pObj->rgChar);
}

float StaticRgBgObjGetBlight(void)
{
    return s_fBgObjBlight;
}

void StaticRgBgObjSetBlight(float blight)
{
    if (blight < 0.0f) {
        blight = 0.0f;
    }
    if (blight > 1.0f) {
        blight = 1.0f;
    }
    s_fBgObjBlight = blight;
}
