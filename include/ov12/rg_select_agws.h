#ifndef INCLUDE_OV12_RG_SELECT_AGWS_H
#define INCLUDE_OV12_RG_SELECT_AGWS_H

/*
 * RgBxx is defined by ov12/tu073 (src/ov12/rg_bxx.c); this allocation only
 * stores and forwards the pointer LoadRgBxx_sub returns and DisposeRgBxx_sub
 * takes, so an incomplete type is enough here.
 */
typedef struct RgBxx RgBxx;

typedef struct RgDrawStudio RgDrawStudio;

typedef struct RgPicList RgPicList;

/*
 * The select-data record _InitSelect creates and RgSelectAGWS::pSelDat
 * points at; owned by _InitSelect/_Destruct (unallocated in this TU), which
 * never define it within this allocation. _SetBaseSelectData's own assert
 * names its first parameter "pSelDat" (ov12:0x00a56838, "pSelDat != NIL").
 */
typedef struct SelDat SelDat;

/*
 * The robot-preview object CreateRgSelectRobot allocates and
 * DisposeRgSelectRobot releases; owned by ov12/tu071 (src/ov12/
 * rg_select_robot.c, full body there). No published include/ov12/
 * rg_select_robot.h exists yet, so this allocation only forwards the
 * pointer, never dereferencing one itself.
 */
typedef struct RgSelectRobot RgSelectRobot;

typedef struct WepListDisp WepListDisp;

typedef struct RgSelectAGWS RgSelectAGWS;

/*
 * _InitSelect, _Destruct and _Disp (this allocation) claim every remaining
 * member up to RG_SELECT_AGWS_SIZE. _InitSelect stores a fixed value (4) at
 * +0x00 (state; no function in this allocation reads it back), the archive
 * LoadRgBxx_sub("select.bxx") returns at +0x08 (pBxx), four picture lists
 * _LoadPicList loads from "agwsseltop.r2d", "agwssel.r2d", "wepseltop.r2d"
 * and "wepsel.r2d" at +0x0C/+0x10/+0x14/+0x18, a scroll offset pair at
 * +0x1C/+0x20 that _Disp forwards to RgPicListSetOffset (named ofsX/ofsY
 * after src/ov12/rg_piclist.h's own fields for the same two values), the
 * weapon-list display object _CreateWepListDisp returns at +0x24
 * (pWepListDisp), the paint context CreateXrgPaint2D_sub returns at +0x28
 * (paint), the draw studio RgDrawGetStudio returns at +0x2C (pStudio) and
 * the robot preview CreateRgSelectRobot returns at +0x30 (pRobot).
 */
struct RgSelectAGWS {
    int state;
    SelDat *pSelDat;
    RgBxx *pBxx;
    RgPicList *pAgwsSelTop;
    RgPicList *pAgwsSel;
    RgPicList *pWepSelTop;
    RgPicList *pWepSel;
    int ofsX;
    int ofsY;
    WepListDisp *pWepListDisp;
    XrgPaint2D *paint;
    RgDrawStudio *pStudio;
    RgSelectRobot *pRobot;
};

#endif /* INCLUDE_OV12_RG_SELECT_AGWS_H */
