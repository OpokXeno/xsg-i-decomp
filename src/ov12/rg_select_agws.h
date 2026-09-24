/*
 * TU-local declarations of ov12/tu069 (src/ov12/rg_select_agws.c).
 */

#ifndef SRC_OV12_RG_SELECT_AGWS_H
#define SRC_OV12_RG_SELECT_AGWS_H

/*
 * The texture archive handle owned and completed by ov12/tu073
 * (src/ov12/rg_bxx.c, struct body published in include/ov12/rg_bxx.h);
 * this TU only forwards the pointer to RgBxxGetPic, so the local typedef
 * (same convention as src/ov12/rg_piclist.h, src/ov12/rg_disp_wpn1p.h and
 * src/ov12/rg_effect_env.h) is enough here.
 */
typedef struct RgBxx RgBxx;

typedef struct FloatArrow FloatArrow;

/*
 * The selection screen keeps two picture handles followed by the two arrow
 * x/y coordinate pairs.  _DispFloatArrow reads the visibility mask at 0x18;
 * this function only updates the four coordinate fields.  Scalar members are
 * used here (changed hypothesis vs the predecessor array-member layout);
 * offsets are identical: pic0 0x00, pic1 0x04, x0 0x08, x1 0x0c, y0 0x10,
 * y1 0x14, visibility_mask 0x18.
 */
struct FloatArrow {
    void *pic0;
    void *pic1;
    int x0;
    int x1;
    int y0;
    int y1;
    unsigned int visibility_mask;
};

typedef struct WinInfo WinInfo;

/*
 * The determination (yes/no) window's picture set. _InitDetermWin's own
 * assert messages name every pointer field directly on its pWinInfo
 * parameter: "pWinInfo->m_pTop[0] != NIL", "pWinInfo->m_pTop[1] != NIL",
 * "pWinInfo->m_pBtm[0] != NIL", "pWinInfo->m_pBtm[1] != NIL",
 * "pWinInfo->m_pSide[0] != NIL", "pWinInfo->m_pSide[1] != NIL",
 * "pWinInfo->m_pInner != NIL", "pWinInfo->m_pLeft != NIL",
 * "pWinInfo->m_pRight != NIL", "pWinInfo->m_pMaru != NIL" and
 * "pWinInfo->m_pBatu != NIL". m_nValue0/m_nValue1 are _InitDetermWin's own
 * third and fourth parameters, stored unmodified; no function in this
 * allocation reads them back, so no stronger role is evidenced for them.
 */
struct WinInfo {
    void *m_pTop[2];
    void *m_pBtm[2];
    void *m_pSide[2];
    void *m_pInner;
    void *m_pLeft;
    void *m_pRight;
    void *m_pMaru;
    void *m_pBatu;
    int m_nValue0;
    int m_nValue1;
};

extern float s_fSinRad;

/*
 * Character-select cursor state. _InitSelectChar sets cursor to 6 and
 * selectableMask to 0x40 (bit 6 of the mask matches the initial cursor);
 * _IsExistItemUpper/_IsExistItemLower test the mask against the cursor
 * position. Only these two fields are evidenced within this TU's allocated
 * functions; the assert message "pSelChar != NIL" (D_00A56650) names the
 * pointer parameter pSelChar.
 */
typedef struct SelChar {
    int cursor;
    unsigned int selectableMask;
} SelChar;

/*
 * Weapon-selection list state, 0x208 bytes: cursor at 0x000, an inline list
 * of up to WEP_MAX handles at 0x004..0x203, and the populated count at
 * 0x204. The assert message "pSelWep->m_uSize > 0" (D_00A566D0) names the
 * count field m_uSize and the pointer parameter pSelWep; "uSize <= WEP_MAX"
 * (D_00A566B8) names the WEP_MAX bound. The same 0x208 stride is used by
 * _GetSelWepSelType's arg0 + index*0x208 + 0x10 elsewhere in this TU
 * (unallocated), independently confirming the size.
 */
#define WEP_MAX 0x80

typedef struct SelWep {
    int cursor;
    int list[WEP_MAX];
    unsigned int m_uSize;
} SelWep;

/*
 * The select-data record _InitSelect creates and RgSelectAGWS::pSelDat
 * points at; owned by _InitSelect/_Destruct (unallocated in this TU), which
 * never define it within this allocation. _SetBaseSelectData's own assert
 * names its first parameter "pSelDat" (ov12:0x00a56838, "pSelDat != NIL").
 */
typedef struct SelDat SelDat;

/*
 * RgDrawView, RgDraw and RgDrawStudio are defined in src/ov12/rg_draw.h
 * (ov12/tu045) and published in include/ov12/rg_draw.h; RgPicList is
 * defined in src/ov12/rg_piclist.h (ov12/tu075) and published in
 * include/ov12/rg_piclist.h. The .c prelude includes both owner headers
 * (docs/header-canon.md, "the definition wins", matching src/ov12/
 * rg_charmgr.c's own #include of the owner's header for a foreign
 * pointer-only type) instead of restating any of these tags here. This
 * replaces the previous per-name incomplete-type forward declarations
 * (review r14, ov12/tu069): header_harvest.py withholds a foreign owner's
 * own published typedef from its canonical header whenever this TU also
 * carries a separate, bare `typedef struct X X;` for that same tag, even
 * when the two spellings are textually identical, so every such name is
 * now read only from the owner's own public header instead.
 */

/*
 * The 2D paint context CreateXrgPaint2D_sub allocates and
 * DisposeXrgPaint2D_sub releases; owned by ov12/tu086 (src/ov12/
 * xrg_paint2d.c, full body there), matching this file's own prelude
 * typedef of the same handle. No published include/ov12/xrg_paint2d.h
 * exists yet, so this allocation only forwards the pointer through an
 * incomplete type, as the prelude's own typedef already does.
 */
typedef struct XrgPaint2D XrgPaint2D;

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
 * The observed access view: CreateRgSelectAGWS allocates 0x34 bytes for one
 * (RG_SELECT_AGWS_SIZE below). RgSelectAGWSSetSelectData, -GetCollectData,
 * -GetCharacter, -GetWeapon and -SetMode all read only the select-data
 * pointer at +0x04; no member beyond it is claimed by this allocation.
 */
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

/* The allocation size CreateRgSelectAGWS requests for one RgSelectAGWS. */
#define RG_SELECT_AGWS_SIZE 0x34

/*
 * Equip weapon-type slot count: the same value is already evidenced
 * elsewhere in this unit by an assert bound ("RG_EQUIP_TYPE_MIN <=
 * (eArmType) && (eArmType) < RG_EQUIP_TYPE_NUM", src/ov12/rg_robot.c,
 * src/ov12/rg_shotmot_control.c); this TU's own allocation has no
 * independent bound check for it.
 */
#define RG_EQUIP_TYPE_NUM 3

/*
 * Equip-type selection cursor the *SelType functions in this allocation
 * share (assert message "pSelType != NIL", ov12:0x00a56758):
 *   - cursor at 0x000: the currently selected equip ID; _GetCursorSelType
 *     reads it and _EquipCurIDToEquipType (unallocated) turns it into an
 *     equip type; _ResumeSelTypeOldCursor restores it from oldCursor.
 *   - oldCursor at 0x004: the saved value _ResumeSelTypeOldCursor
 *     restores cursor from.
 *   - wepList at 0x010: one embedded SelWep per equip type
 *     (RG_EQUIP_TYPE_NUM above); _GetSelWepSelType's own 0x208*index+0x10
 *     address computation is this allocation's evidence for both the
 *     offset and the stride (matching sizeof(SelWep)).
 *   - confrict at 0x634: one per-character conflict flag (six characters,
 *     s_aeCharTbl above); _GetConfrictSelType's own index*4+0x634 address
 *     computation is this allocation's evidence.
 * Only these fields are evidenced within this allocation; the two spans
 * between them stay unmodeled.
 */
/*
 * charID at 0x630 (within the second span above): _InitSelType (this
 * allocation) resets it to RG_ACTOR_CHAR_NOP, and _InitByCharSelType
 * (ov12:0x00a39458, unallocated) stores its own charID argument there once
 * it has initialized every wepList entry for that character; no function in
 * this allocation reads it back.
 */
typedef struct SelType {
    int cursor;
    int oldCursor;
    unsigned char unmodeled_008[8];
    SelWep wepList[RG_EQUIP_TYPE_NUM];
    unsigned char unmodeled_628[8];
    int charID;
    int confrict[6];
} SelType;

/*
 * The select-data record _CreateSelectData allocates and _DisposeSelectData
 * releases; RgHeapAlloc's own literal 0x8B0 is this allocation's evidence
 * for the total size (used below via sizeof). Four members are evidenced:
 * mode at 0x000 (_GetModeSelectData reads it, _TransitToOffSelect sets it
 * to 2, and _PassTimeSelectData switches on it: mode 0 forwards to selChar,
 * mode 1 to selType), the embedded selChar at 0x010 (_LoadCharSelectData
 * and _PassTimeSelectData's own mode-0 case both take its address), the
 * embedded selType at 0x020 (_PassTimeSelectData's own mode-1 case takes
 * its address; the same offset is used by _LoadWepSelectData and
 * _SetBaseSelectData elsewhere in this unit, unallocated), and playerMode
 * at 0x8A8, four bytes before the end of the allocation
 * (_GetPlayerModeSelectData reads it, and _PassTimeSelectData branches on
 * it to pick the pad ID). The spans between them stay unmodeled.
 */
struct SelDat {
    int mode;
    unsigned char unmodeled_004[12];
    SelChar selChar;
    unsigned char unmodeled_018[8];
    SelType selType;
    unsigned char unmodeled_66c[0x23c];
    int playerMode;
    unsigned char unmodeled_8ac[4];
};

/*
 * Weapon-list display object _CreateWepListDisp allocates and
 * _DestructWepListDisp releases (assert message "pDisp != NIL",
 * ov12:0x00a56798); RgHeapAlloc's own literal 0x24 is this allocation's
 * only evidence for the total size. _DestructWepListDisp's own offset 8
 * is this allocation's only evidence for the font field (an int handle,
 * matching _CreateSelectWepFont/CreateRgFont's own int return, above).
 */
/*
 * _SetSelectorWepListDisp (this allocation) stores its second argument as
 * pSelType and clears pSelWep and topIndex to 0. _DrawWepListDisp's own
 * reads (unallocated in this TU) pass pSelType to _GetCursorSelType/
 * _GetTypeSelType/_GetCurSelType, and compare pSelWep against
 * _GetCurSelType's own SelWep * result to detect a list change, reseeding
 * topIndex from _GetCursorSelWep before clamping it against
 * _GetEssListSizeSelWep -- the first visible row of the scrolling weapon
 * list.
 */
typedef struct WepListDisp {
    SelType *pSelType;
    SelWep *pSelWep;
    int font;
    unsigned char unmodeled_00c[20];
    int topIndex;
} WepListDisp;

#endif /* SRC_OV12_RG_SELECT_AGWS_H */
