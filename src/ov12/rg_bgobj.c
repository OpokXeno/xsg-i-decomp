/*
 * OV12 original TU 17: 0x00a129c8..0x00a13200 (16 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_char.h"

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
    unsigned char unmodeled_34[4];  /* 0x34 */
    float hardness;                 /* 0x38 */
    int notUseGeomLocal;            /* 0x3C */
} RgBgObj;

/* Defined later in this TU. */
extern void _InitRgBgObj(RgBgObj *pObj, void *dispModel, RgGeom *geomTray);

/* Defined by other TUs. */
extern void DisposeRgDispModel(void *dispModel);
extern void RgGeomFree(RgGeom *geom);
extern void RgGeomTraySetLocal(void *tray, Matrix4 source);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_bgobj", _InitRgBgObj);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_bgobj", _Draw);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_bgobj", RgBgObjTryToBreak);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_bgobj", RgBgObjTryToBodyAttack);

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
