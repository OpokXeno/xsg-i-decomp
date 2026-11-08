/*
 * OV12 original TU 17: 0x00a129c8..0x00a13200 (16 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_bgobj.h"
#include "ov12/rg_debug_flags.h"
#include "ov12/rg_geom_tray.h"
#include "ov12/xrg_rand_int.h"
#include "ov12/create_rg.h"

/* Seconds that one fade-out or fade-in of a hidden background object takes. */
#define RG_BGOBJ_FADE_TIME 0.3f

/* A body attack breaks the object at this energy or more. */
#define RG_BGOBJ_BODY_ATTACK_MIN_ENERGY 288000.0f

/* Height above the object where the break effect appears. */
#define RG_BGOBJ_BREAK_EFFECT_HEIGHT 2.0f

/* Assertion strings and the source filename passed to the assertion helper. */
const char bgObjAssertObjNotNil[16] = "pObj != NIL";

const char bgObjSourcePath[24] = "../rg_bgobj.euc.c";

static float s_fBgObjBlight = 1.0f;

/* Defined later in this TU. */
static void _InitRgBgObj(RgBgObj *pObj, RgDispModel *dispModel, RgGeom *geomTray);
static void _Draw(RgChar *pChar);
static void _PassTime(RgChar *pChar, float deltaTime);
static void _Destruct(RgChar *pChar);

const char bgObjBreakHitEffectName[16] = "eff12.ptcl";

/* Installs the char methods on a background object, sets its defaults and attaches its geom tray. */
static void _InitRgBgObj(RgBgObj *pObj, RgDispModel *dispModel, RgGeom *geomTray)
{
    if (pObj == 0) {
        assert_prog(bgObjAssertObjNotNil, bgObjSourcePath, 70);
    }
    RgCharDispMethod(&pObj->rgChar, _Draw);
    RgCharPassTimeMethod(&pObj->rgChar, _PassTime);
    RgCharDestructMethod(&pObj->rgChar, _Destruct);
    pObj->dispModel = dispModel;
    pObj->geomTray = geomTray;
    if (dispModel != 0) {
        RgDispModelSetMode(dispModel, RG_DISP_MODEL_MODE_MAPCLIP);
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

RgBgObj *CreateRgBgObj(RgDispModel *dispModel, RgGeom *geomTray)
{
    RgBgObj *pObj;

    pObj = (RgBgObj *)RgCharAlloc(sizeof(RgBgObj), RG_CHAR_TYPE_BGOBJ);
    if (pObj == 0) {
        assert_prog(bgObjAssertObjNotNil, bgObjSourcePath, 110);
    }
    _InitRgBgObj(pObj, dispModel, geomTray);
    return pObj;
}

static void _Destruct(RgChar *pChar)
{
    RgBgObj *pObj = (RgBgObj *)pChar;

    if (pObj == 0) {
        assert_prog(bgObjAssertObjNotNil, bgObjSourcePath, 122);
    }
    if (pObj->dispModel != 0) {
        DisposeRgDispModel(pObj->dispModel);
    }
    if (pObj->geomTray != 0) {
        RgGeomFree(pObj->geomTray);
    }
}

/* Advances the hide state machine by deltaTime and updates the transparency to match. */
static void _PassTime(RgChar *pChar, float deltaTime)
{
    RgBgObj *pObj = (RgBgObj *)pChar;
    unsigned int hideState;

    if (pObj == 0) {
        assert_prog(bgObjAssertObjNotNil, bgObjSourcePath, 136);
    }
    pObj->hideElapsed += deltaTime;
    hideState = pObj->hideState;
    switch (hideState) {
    case RG_BGOBJ_HIDE_NONE:
        break;

    case RG_BGOBJ_HIDE_FADE_OUT:
        if (pObj->hideElapsed > RG_BGOBJ_FADE_TIME) {
            pObj->transparent = 0.0f;
            hideState = RG_BGOBJ_HIDE_HIDDEN;
        } else {
            pObj->transparent = (RG_BGOBJ_FADE_TIME - pObj->hideElapsed) / RG_BGOBJ_FADE_TIME;
        }
        break;

    case RG_BGOBJ_HIDE_HIDDEN:
        pObj->hideDuration -= deltaTime;
        if (pObj->hideDuration < 0.0f) {
            pObj->hideElapsed = 0.0f;
            hideState = RG_BGOBJ_HIDE_FADE_IN;
        }
        break;

    case RG_BGOBJ_HIDE_FADE_IN:
        if (pObj->hideElapsed > RG_BGOBJ_FADE_TIME) {
            pObj->transparent = -1.0f;
            hideState = RG_BGOBJ_HIDE_NONE;
        } else {
            pObj->transparent = pObj->hideElapsed / RG_BGOBJ_FADE_TIME;
        }
        break;
    }
    pObj->hideState = hideState;
}

/*
 * Draws the object's display model unless it is hidden. The draw is gated by
 * InstanceOfRgDebugFlags()->flags[1], which _InitRgDebugFlags enables by default
 * (include/ov12/rg_debug_flags.h), so background objects draw normally unless a
 * developer disables it.
 */
static void _Draw(RgChar *pChar)
{
    RgBgObj *pObj = (RgBgObj *)pChar;
    RgDispModel *dispModel;
    RgMatrix local;
    float transparent;

    if (InstanceOfRgDebugFlags()->flags[1] != 0) {
        if (pObj == 0) {
            assert_prog(bgObjAssertObjNotNil, bgObjSourcePath, 179);
        }
        if (pObj->hideState != RG_BGOBJ_HIDE_HIDDEN) {
            dispModel = pObj->dispModel;
            if (dispModel != 0) {
                if (pObj->notUseGeomLocal != 0) {
                    XrgUnitMatrix(local);
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

void RgBgObjSetLocal(RgBgObj *pObj, RgMatrix local)
{
    if (pObj == 0) {
        assert_prog(bgObjAssertObjNotNil, bgObjSourcePath, 205);
    }
    RgGeomTraySetLocal(pObj->geomTray, local);
}

void RgBgObjSetHardness(RgBgObj *pObj, float hardness)
{
    if (pObj == 0) {
        assert_prog(bgObjAssertObjNotNil, bgObjSourcePath, 215);
    }
    pObj->hardness = hardness;
}

/*
 * Hides the object for duration seconds: it fades out, unless it is already hidden
 * or ignores hiding.
 */
void RgBgObjSetHide(RgBgObj *pObj, float duration)
{
    if (pObj == 0) {
        assert_prog(bgObjAssertObjNotNil, bgObjSourcePath, 226);
    }

    if (!(pObj->flags & RG_BGOBJ_FLAG_IGNORE_HIDE)) {
        unsigned int hideState;

        hideState = pObj->hideState;
        pObj->hideDuration = duration;
        switch (hideState) {
        case RG_BGOBJ_HIDE_HIDDEN:
            break;

        case RG_BGOBJ_HIDE_NONE:
            pObj->hideElapsed = 0.0f;
            /* fallthrough */
        case RG_BGOBJ_HIDE_FADE_OUT:
        case RG_BGOBJ_HIDE_FADE_IN:
            hideState = RG_BGOBJ_HIDE_FADE_OUT;
            break;
        }
        pObj->hideState = hideState;
    }
}

void RgBgObjIgnoreHide(RgBgObj *pObj, int ignoreHide)
{
    if (pObj == 0) {
        assert_prog(bgObjAssertObjNotNil, bgObjSourcePath, 254);
    }

    if (ignoreHide != 0) {
        pObj->flags |= RG_BGOBJ_FLAG_IGNORE_HIDE;
    } else {
        pObj->flags &= ~RG_BGOBJ_FLAG_IGNORE_HIDE;
    }
}

void RgBgObjEnableBodyAttack(RgBgObj *pObj, int enableBodyAttack)
{
    if (pObj == 0) {
        assert_prog(bgObjAssertObjNotNil, bgObjSourcePath, 263);
    }

    if (enableBodyAttack != 0) {
        pObj->flags |= RG_BGOBJ_FLAG_BODY_ATTACK_ENABLED;
    } else {
        pObj->flags &= ~RG_BGOBJ_FLAG_BODY_ATTACK_ENABLED;
    }
}

void RgBgObjNotUseGeomLocal(RgBgObj *pObj)
{
    if (pObj == 0) {
        assert_prog(bgObjAssertObjNotNil, bgObjSourcePath, 272);
    }
    pObj->notUseGeomLocal = 1;
}

RgDispModel *RgBgObjGetDispModel(RgBgObj *pObj)
{
    if (pObj == 0) {
        assert_prog(bgObjAssertObjNotNil, bgObjSourcePath, 283);
    }
    return pObj->dispModel;
}

/*
 * Applies damage: the object breaks, with a break effect, when the damage exceeds its
 * hardness, and otherwise loses that much hardness. Returns 0 when hardness is unset.
 */
int RgBgObjTryToBreak(RgBgObj *pObj, float damage)
{
    RgMatrix local;

    if (pObj == 0) {
        assert_prog(bgObjAssertObjNotNil, bgObjSourcePath, 294);
    }
    if (pObj->hardness < 0.0f) {
        return 0;
    }
    if (pObj->hardness < damage) {
        if (pObj->geomTray != 0) {
            RgGeomTrayGetLocal(pObj->geomTray, local);
            local[13] += RG_BGOBJ_BREAK_EFFECT_HEIGHT;
            CreateRgHitEffectPos(&local[12], bgObjBreakHitEffectName, 0);
        }
        RgCharFree(&pObj->rgChar);
    } else {
        pObj->hardness -= damage;
    }
    return 1;
}

/* Breaks the object on a body attack of enough energy, if body attacks are enabled for it. */
void RgBgObjTryToBodyAttack(RgBgObj *pObj, float energy)
{
    RgMatrix local;

    if (pObj == 0) {
        assert_prog(bgObjAssertObjNotNil, bgObjSourcePath, 324);
    }
    if (energy < RG_BGOBJ_BODY_ATTACK_MIN_ENERGY) {
        return;
    }
    if (!(pObj->flags & RG_BGOBJ_FLAG_BODY_ATTACK_ENABLED)) {
        return;
    }
    if (pObj->geomTray != 0) {
        RgGeomTrayGetLocal(pObj->geomTray, local);
        local[13] += RG_BGOBJ_BREAK_EFFECT_HEIGHT;
        CreateRgHitEffectPos(&local[12], bgObjBreakHitEffectName, 0);
    }
    RgCharFree(&pObj->rgChar);
}

float StaticRgBgObjGetBlight(void)
{
    return s_fBgObjBlight;
}

/* Sets the blight level, clamped to the range 0 to 1. */
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
