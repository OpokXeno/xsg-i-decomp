#ifndef INCLUDE_OV12_RG_BGOBJ_H
#define INCLUDE_OV12_RG_BGOBJ_H

#include "shared.h"
#include "ov12/rg_char.h"
#include "ov12/rg_dispmodel.h"
#include "ov12/rg_geom.h"

/* The RgChar type tag CreateRgBgObj passes to RgCharAlloc for a background
 * object. */
#define RG_CHAR_TYPE_BGOBJ 2

/*
 * RgBgObj.flags bits. RgBgObjIgnoreHide sets or clears the first, which
 * RgBgObjSetHide tests; RgBgObjEnableBodyAttack sets or clears the second.
 */
#define RG_BGOBJ_FLAG_IGNORE_HIDE (1 << 0)
#define RG_BGOBJ_FLAG_BODY_ATTACK_ENABLED (1 << 1)

/* RgBgObj.hideState: a camera-blocking object fades out, stays hidden for
 * hideDuration seconds, then fades back in. */
enum {
    RG_BGOBJ_HIDE_NONE = 0,
    RG_BGOBJ_HIDE_FADE_OUT = 1,
    RG_BGOBJ_HIDE_HIDDEN = 2,
    RG_BGOBJ_HIDE_FADE_IN = 3
};

/*
 * A background object extends RgChar: CreateRgBgObj allocates it with
 * RgCharAlloc(sizeof(RgBgObj), RG_CHAR_TYPE_BGOBJ), so its own fields begin
 * right after RgChar's 0x1C bytes. transparent is -1 when the object draws
 * with its model's own transparency; otherwise _Draw passes it to
 * RgDispModelSetTransparent.
 */
typedef struct RgBgObj {
    RgChar rgChar;                  /* 0x00 */
    unsigned int flags;             /* 0x1C */
    RgDispModel *dispModel;         /* 0x20 */
    RgGeom *geomTray;               /* 0x24 */
    unsigned int hideState;         /* 0x28 */
    float hideDuration;             /* 0x2C */
    float hideElapsed;              /* 0x30 */
    float transparent;              /* 0x34 */
    float hardness;                 /* 0x38: -1 for an unbreakable object */
    int notUseGeomLocal;            /* 0x3C */
} RgBgObj;

/* Functions and data used by other translation units. */
RgBgObj *CreateRgBgObj(RgDispModel *dispModel, RgGeom *geomTray);
void RgBgObjSetLocal(RgBgObj *pObj, RgMatrix local);
void RgBgObjSetHardness(RgBgObj *pObj, float hardness);
void RgBgObjSetHide(RgBgObj *pObj, float duration);
void RgBgObjIgnoreHide(RgBgObj *pObj, int ignoreHide);
void RgBgObjEnableBodyAttack(RgBgObj *pObj, int enableBodyAttack);
void RgBgObjNotUseGeomLocal(RgBgObj *pObj);
RgDispModel *RgBgObjGetDispModel(RgBgObj *pObj);
int RgBgObjTryToBreak(RgBgObj *pObj, float damage);
void RgBgObjTryToBodyAttack(RgBgObj *pObj, float energy);
float StaticRgBgObjGetBlight(void);
void StaticRgBgObjSetBlight(float blight);

#endif /* INCLUDE_OV12_RG_BGOBJ_H */
