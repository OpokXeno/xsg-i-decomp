/*
 * OV12 original TU 69: 0x00a380d8..0x00a3c6a0 (103 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_bxx.h"
#include "rg_select_agws.h"

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _disp_pic);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _disp_pic_pl);

extern unsigned char s_aFontTbl_0[];

extern int CreateRgFont(void *table, int glyphCount, int pic);

static int _CreateSelectWepFont(int pic)
{
    return CreateRgFont(s_aFontTbl_0, 0x29, pic);
}

static void _SinInit(void)
{
    s_fSinRad = 0.0f;
}

static void _SinPassTime(void)
{
    float phase = s_fSinRad + 0.3926991f;
    while (phase > 3.1415928f)
        phase -= 6.2831855f;
    while (phase < -3.1415928f)
        phase += 6.2831855f;
    s_fSinRad = phase;
}

static int _SinGet(int amplitude)
{
    return (int)(sinf(s_fSinRad) * (float)amplitude);
}

extern int RgEquipCheckConfrict(int *equipData, int slot);

/*
 * The essence handle values evidenced by _GetNameOnCursorSelWep also carry
 * an equip sub-record at this offset; the only field _CheckEquipWeps
 * evidences on it (docs/style.md, "Struct fields, not offset casts" --
 * narrowly evidenced scalar access).
 */
#define WEP_ESSENCE_EQUIP_OFFSET 0x70

static int _CheckEquipWeps(int *essences, int slot)
{
    int equipData[3];
    unsigned int i;

    for (i = 0; i < 3; i++) {
        equipData[i] = essences[i] + WEP_ESSENCE_EQUIP_OFFSET;
    }
    return RgEquipCheckConfrict(equipData, slot);
}

extern int s_aeCharTbl[6];

static int _CharCursorToCharID(int charCursor)
{
    if ((unsigned int) charCursor >= 6) {
        return -1;
    }
    return s_aeCharTbl[charCursor];
}

static int _CharIDToCharCursor(int charID)
{
    unsigned int i;

    for (i = 0; i < 6; i++) {
        if (s_aeCharTbl[i] == charID) {
            return i;
        }
    }
    return 6;
}

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

extern const char D_00A56650[]; /* "pSelChar != NIL" */
extern const char D_00A56660[]; /* "../rg_select_agws.euc.c" */

static void _InitSelectChar(SelChar *pSelChar)
{
    if (pSelChar == 0) {
        assert_prog(D_00A56650, D_00A56660, 0xD0);
    }
    pSelChar->cursor = 6;
    pSelChar->selectableMask = 0x40;
}

/* Defined above (scaffold); returns s_aeCharTbl[charCursor], or -1 past the table. */
extern int _CharCursorToCharID(int charCursor);

static int _GetSelectChar(SelChar *pSelChar)
{
    if (pSelChar == 0) {
        assert_prog(D_00A56650, D_00A56660, 0xD8);
    }
    return _CharCursorToCharID(pSelChar->cursor);
}

static int _GetSelectCursor(SelChar *pSelChar)
{
    if (pSelChar == 0) {
        assert_prog(D_00A56650, D_00A56660, 0xDE);
    }
    return pSelChar->cursor;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _SetSelectableSelectChar);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _SetCharSelectChar);

static int _IsExistItemUpper(SelChar *pSelChar)
{
    if (pSelChar == 0) {
        assert_prog(D_00A56650, D_00A56660, 0x10F);
    }
    return (pSelChar->selectableMask & ((1 << pSelChar->cursor) - 1)) != 0;
}

static int _IsExistItemLower(SelChar *pSelChar)
{
    int cursorBit;

    if (pSelChar == 0) {
        assert_prog(D_00A56650, D_00A56660, 0x119);
    }
    cursorBit = 1 << pSelChar->cursor;
    return (pSelChar->selectableMask & ~((cursorBit - 1) | cursorBit)) != 0;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetSelectableSelectChar);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _PassTimeSelectChar);

extern const char D_00A56698[]; /* "pSelWep != NIL" */

static void _InitSelWep(SelWep *pSelWep)
{
    if (pSelWep == 0) {
        assert_prog(D_00A56698, D_00A56660, 0x157);
    }
    pSelWep->cursor = 0;
    pSelWep->m_uSize = 0;
}

extern const char D_00A566A8[]; /* "uSize > 0" */
extern const char D_00A566B8[]; /* "uSize <= WEP_MAX" */

/*
 * Fills `list` with the weapons matching the filter and returns their count.
 * The first call passes no list and its count is checked against WEP_MAX
 * before the second fills pSelWep->list; both forward the same
 * characterID/weaponType (the first call leaves a1/a2 untouched).
 */
extern unsigned int RgSelectWeapons(int *list, int characterID, int weaponType);

/*
 * characterID/weaponType are the two forwarded filter arguments of
 * RgSelectWeapons, named from this function's own role ("CharType" weapon
 * selection): which character's equippable list this is and which weapon
 * type it is being filtered to. The callee itself is unresolved, so no
 * stronger evidence for these two roles exists within this allocation.
 */
static void _InitCharTypeSelWep(SelWep *pSelWep, int characterID,
                                int weaponType)
{
    unsigned int uSize;

    uSize = RgSelectWeapons(0, characterID, weaponType);
    if (uSize == 0) {
        assert_prog(D_00A566A8, D_00A56660, 0x160);
    }
    if (uSize > WEP_MAX) {
        assert_prog(D_00A566B8, D_00A56660, 0x161);
    }
    if (pSelWep == 0) {
        assert_prog(D_00A56698, D_00A56660, 0x162);
    }
    uSize = RgSelectWeapons(pSelWep->list, characterID, weaponType);
    pSelWep->cursor = 0;
    pSelWep->m_uSize = uSize;
}

extern const char D_00A566D0[]; /* "pSelWep->m_uSize > 0" */

static int _GetEssSelWep(SelWep *pSelWep)
{
    if (pSelWep == 0) {
        assert_prog(D_00A56698, D_00A56660, 0x16A);
    }
    if (pSelWep->m_uSize == 0) {
        assert_prog(D_00A566D0, D_00A56660, 0x16B);
    }
    return pSelWep->list[pSelWep->cursor];
}

static int *_GetEssListSelWep(SelWep *pSelWep)
{
    if (pSelWep == 0) {
        assert_prog(D_00A56698, D_00A56660, 0x172);
    }
    return pSelWep->list;
}

static unsigned int _GetEssListSizeSelWep(SelWep *pSelWep)
{
    if (pSelWep == 0) {
        assert_prog(D_00A56698, D_00A56660, 0x179);
    }
    return pSelWep->m_uSize;
}

static int _GetCursorSelWep(SelWep *pSelWep)
{
    if (pSelWep == 0) {
        assert_prog(D_00A56698, D_00A56660, 0x180);
    }
    return pSelWep->cursor;
}

extern const char D_00A566E8[]; /* "0 <= nCur && nCur < pSelWep->m_uSize" */

static void _SetCursorSelWep(SelWep *pSelWep, int nCur)
{
    if (pSelWep == 0) {
        assert_prog(D_00A56698, D_00A56660, 0x187);
    }
    if (nCur < 0 || (unsigned int) nCur >= pSelWep->m_uSize) {
        assert_prog(D_00A566E8, D_00A56660, 0x188);
    }
    pSelWep->cursor = nCur;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _SetCursorByNameSelWep);

/*
 * Name field offset within an unrecovered weapon-essence record; only this
 * field is evidenced by _GetNameOnCursorSelWep (docs/style.md, "Struct
 * fields, not offset casts" -- narrowly evidenced scalar access).
 */
#define WEP_ESSENCE_NAME_OFFSET 0x330

extern char *strcpy(char *destination, const char *source);

static void _GetNameOnCursorSelWep(SelWep *pSelWep, char *name)
{
    if (pSelWep == 0) {
        assert_prog(D_00A56698, D_00A56660, 0x1A1);
    }
    if (pSelWep->m_uSize != 0) {
        strcpy(name, (const char *) (pSelWep->list[pSelWep->cursor] +
                                     WEP_ESSENCE_NAME_OFFSET));
        return;
    }
    *name = '\0';
}

extern int XrgPadIsUp(void);
extern int XrgPadIsDown(void);
extern void XrgSoundSystemCursor(void);

static void _PassTimeSelWep(SelWep *pSelWep)
{
    int cursor;
    int previousCursor;

    if (pSelWep == 0) {
        assert_prog(D_00A56698, D_00A56660, 0x1AD);
    }
    if (pSelWep->m_uSize == 0) {
        assert_prog(D_00A566D0, D_00A56660, 0x1AE);
    }
    cursor = pSelWep->cursor;
    previousCursor = cursor;
    if (XrgPadIsUp() != 0 && cursor > 0) {
        cursor--;
    }
    if (XrgPadIsDown() != 0 && (unsigned int) cursor < pSelWep->m_uSize - 1) {
        cursor++;
    }
    pSelWep->cursor = cursor;
    if (cursor != previousCursor) {
        XrgSoundSystemCursor();
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _EquipCurIDToEquipType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _SetSelTypeCursor);

static void _ResumeSelTypeOldCursor(SelType *pSelType)
{
    pSelType->cursor = pSelType->oldCursor;
}

extern const char D_00A56758[]; /* "pSelType != NIL" */

static int _GetCursorSelType(SelType *pSelType)
{
    if (pSelType == 0) {
        assert_prog(D_00A56758, D_00A56660, 517);
    }
    return pSelType->cursor;
}

/* _EquipCurIDToEquipType (above) is still INCLUDE_ASM; declare it so its
 * caller does not see an implicit declaration. */
static int _EquipCurIDToEquipType(int curID);

static int _GetTypeSelType(SelType *pSelType)
{
    if (pSelType == 0) {
        assert_prog(D_00A56758, D_00A56660, 524);
    }
    return _EquipCurIDToEquipType(pSelType->cursor);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetCurSelType);

static SelWep *_GetSelWepSelType(SelType *pSelType, int equipType)
{
    if (pSelType == 0) {
        assert_prog(D_00A56758, D_00A56660, 544);
    }
    return &pSelType->wepList[equipType];
}

static int _GetConfrictSelType(SelType *pSelType, int charIndex)
{
    if (pSelType == 0) {
        assert_prog(D_00A56758, D_00A56660, 551);
    }
    return pSelType->confrict[charIndex];
}

/* _SetSelTypeCursor (above) is still INCLUDE_ASM; declare it so its
 * callers do not see an implicit declaration. */
static void _SetSelTypeCursor(SelType *pSelType, int mode);

static void _SetDetermModeSelType(SelType *pSelType)
{
    if (pSelType == 0) {
        assert_prog(D_00A56758, D_00A56660, 558);
    }
    _SetSelTypeCursor(pSelType, 3);
}

static void _SetSpecModeSelType(SelType *pSelType, int mode)
{
    if (pSelType == 0) {
        assert_prog(D_00A56758, D_00A56660, 566);
    }
    _SetSelTypeCursor(pSelType, mode);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetCurrentConfrictSelType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetCurrentWeaponSelType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetAllSelType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetAllNameOnCursorSelType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _CheckConfrictSelType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _SetAllCursorByNameSelType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _InitSelType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _InitByCharSelType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _PassTimeSelType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _DetachWeaponIfConfrictSelType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _InitWepListDisp);

extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size,
                         const char *source_file, int line);

/*
 * _InitWepListDisp (above) is still INCLUDE_ASM; declare it so its caller
 * does not see an implicit declaration. equipType is _CreateWepListDisp's
 * own argument, forwarded unchanged; the callee itself is unresolved, so
 * no stronger evidence for this role exists within this allocation.
 */
static void _InitWepListDisp(WepListDisp *pDisp, int equipType);

static WepListDisp *_CreateWepListDisp(int equipType)
{
    WepListDisp *pDisp;

    pDisp = RgHeapAlloc(InstanceOfRgHeap(), sizeof(WepListDisp), D_00A56660,
                        767);
    _InitWepListDisp(pDisp, equipType);
    return pDisp;
}

extern void DisposeRgFont(int font);
extern const char D_00A56798[]; /* "pDisp != NIL" */

static void _DestructWepListDisp(WepListDisp *pDisp)
{
    if (pDisp == 0) {
        assert_prog(D_00A56798, D_00A56660, 775);
    }
    if (pDisp->font != 0) {
        DisposeRgFont(pDisp->font);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _DisposeWepListDisp);

static void _SetSelectorWepListDisp(WepListDisp *pDisp, SelType *pSelType)
{
    if (pDisp == 0) {
        assert_prog(D_00A56798, D_00A56660, 791);
    }
    pDisp->pSelType = pSelType;
    pDisp->pSelWep = 0;
    pDisp->topIndex = 0;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _DrawWepListDisp);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _InitSelectData);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _DumpSelect__);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _SaveCharSelectData);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _SaveWepSelectData);

extern const char D_00A56838[]; /* "pSelDat != NIL" */
extern const char D_00A568C0[]; /* "pData != NIL" */

/*
 * _SetCharSelectChar/_SetSelectableSelectChar (above) are still INCLUDE_ASM;
 * declare them so this caller does not see an implicit declaration.
 */
static void _SetCharSelectChar(SelChar *pSelChar, int charID);
static void _SetSelectableSelectChar(SelChar *pSelChar, unsigned int charIDMask);

static void _LoadCharSelectData(SelDat *pSelDat, int *pData)
{
    if (pSelDat == 0) {
        assert_prog(D_00A56838, D_00A56660, 1027);
    }
    if (pData == 0) {
        assert_prog(D_00A568C0, D_00A56660, 1028);
    }
    _SetCharSelectChar(&pSelDat->selChar, pData[0]);
    _SetSelectableSelectChar(&pSelDat->selChar, pData[1]);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _LoadWepSelectData);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetBaseSelectData);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _SetBaseSelectData);

static int _GetModeSelectData(SelDat *pSelDat)
{
    if (pSelDat == 0) {
        assert_prog(D_00A56838, D_00A56660, 1084);
    }
    return pSelDat->mode;
}

/*
 * _InitSelectData (above) is still INCLUDE_ASM; declare it so this caller
 * does not see an implicit declaration.
 */
static void _InitSelectData(SelDat *pSelDat);

static SelDat *_CreateSelectData(void)
{
    SelDat *pSelDat;

    pSelDat = RgHeapAlloc(InstanceOfRgHeap(), sizeof(SelDat), D_00A56660,
                          1091);
    _InitSelectData(pSelDat);
    return pSelDat;
}

extern void RgHeapFree(void *heap, void *ptr, const char *source_file,
                       int line);

static void _DisposeSelectData(SelDat *pSelDat)
{
    if (pSelDat == 0) {
        assert_prog(D_00A56838, D_00A56660, 1099);
    }
    RgHeapFree(InstanceOfRgHeap(), pSelDat, D_00A56660, 1100);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _TransitToWeaponSelect);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _TransitToCharSelect);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _SaveToBaseData);

extern const char D_00A568E0[]; /* "pSel != NIL" */

static void _TransitToOffSelect(SelDat *pSel)
{
    if (pSel == 0) {
        assert_prog(D_00A568E0, D_00A56660, 1143);
    }
    pSel->mode = 2;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _SetPlayerModeSelectData);

static int _GetPlayerModeSelectData(SelDat *pSel)
{
    if (pSel == 0) {
        assert_prog(D_00A568E0, D_00A56660, 1157);
    }
    return pSel->playerMode;
}

extern void XrgPadSetID(int pad_id);

/*
 * _PassTimeSelectChar/_PassTimeSelType (above) are still INCLUDE_ASM;
 * declare them so this caller does not see an implicit declaration.
 */
static void _PassTimeSelectChar(SelChar *pSelChar, float deltaTime);
static void _PassTimeSelType(SelType *pSelType, float deltaTime);

static void _PassTimeSelectData(SelDat *pSel, float deltaTime)
{
    if (pSel == 0) {
        assert_prog(D_00A568E0, D_00A56660, 1164);
    }
    if (pSel->playerMode == 0) {
        XrgPadSetID(0);
    } else {
        XrgPadSetID(1);
    }
    switch (pSel->mode) {
    case 1:
        _PassTimeSelType(&pSel->selType, deltaTime);
        return;
    do {
    case 0:
        _PassTimeSelectChar(&pSel->selChar, deltaTime);
        return;
    } while (0);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetSelectCharID);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetSelectWeps);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _GetWepSelection);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _DrawPlayerMode);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _SetCharSelDisp);

extern void *RgBxxGetPic(RgBxx *pBxx, const char *pszName);

extern const char D_00A56968[]; /* "frm11on_ul.bmp" */
extern const char D_00A56978[]; /* "frm11on_ur.bmp" */
extern const char D_00A56988[]; /* "frm11on_bl.bmp" */
extern const char D_00A56998[]; /* "frm11on_br.bmp" */
extern const char D_00A569A8[]; /* "frm11on_cl.bmp" */
extern const char D_00A569B8[]; /* "frm11on_cr.bmp" */
extern const char D_00A569C8[]; /* "frm11on_cc.bmp" */
extern const char D_00A569D8[]; /* "icon_leftarrow.bmp" */
extern const char D_00A569F0[]; /* "icon_rightarrow.bmp" */
extern const char D_00A56A08[]; /* "say_yes.bmp" */
extern const char D_00A56A18[]; /* "say_no.bmp" */
extern const char D_00A56A28[]; /* "pWinInfo->m_pTop[0] != NIL" */
extern const char D_00A56A48[]; /* "pWinInfo->m_pTop[1] != NIL" */
extern const char D_00A56A68[]; /* "pWinInfo->m_pBtm[0] != NIL" */
extern const char D_00A56A88[]; /* "pWinInfo->m_pBtm[1] != NIL" */
extern const char D_00A56AA8[]; /* "pWinInfo->m_pSide[0] != NIL" */
extern const char D_00A56AC8[]; /* "pWinInfo->m_pSide[1] != NIL" */
extern const char D_00A56AE8[]; /* "pWinInfo->m_pInner != NIL" */
extern const char D_00A56B08[]; /* "pWinInfo->m_pLeft != NIL" */
extern const char D_00A56B28[]; /* "pWinInfo->m_pRight != NIL" */
extern const char D_00A56B48[]; /* "pWinInfo->m_pMaru != NIL" */
extern const char D_00A56B68[]; /* "pWinInfo->m_pBatu != NIL" */

static void _InitDetermWin(WinInfo *pWinInfo, RgBxx *pBxx, int value0, int value1)
{
    void *pBatu;

    pWinInfo->m_pTop[0] = RgBxxGetPic(pBxx, D_00A56968);
    pWinInfo->m_pTop[1] = RgBxxGetPic(pBxx, D_00A56978);
    pWinInfo->m_pBtm[0] = RgBxxGetPic(pBxx, D_00A56988);
    pWinInfo->m_pBtm[1] = RgBxxGetPic(pBxx, D_00A56998);
    pWinInfo->m_pSide[0] = RgBxxGetPic(pBxx, D_00A569A8);
    pWinInfo->m_pSide[1] = RgBxxGetPic(pBxx, D_00A569B8);
    pWinInfo->m_pInner = RgBxxGetPic(pBxx, D_00A569C8);
    pWinInfo->m_pLeft = RgBxxGetPic(pBxx, D_00A569D8);
    pWinInfo->m_pRight = RgBxxGetPic(pBxx, D_00A569F0);
    pWinInfo->m_pMaru = RgBxxGetPic(pBxx, D_00A56A08);
    pBatu = RgBxxGetPic(pBxx, D_00A56A18);
    pWinInfo->m_nValue0 = value0;
    pWinInfo->m_pBatu = pBatu;
    pWinInfo->m_nValue1 = value1;
    if (pWinInfo->m_pTop[0] == 0) {
        assert_prog(D_00A56A28, D_00A56660, 1434);
    }
    if (pWinInfo->m_pTop[1] == 0) {
        assert_prog(D_00A56A48, D_00A56660, 1435);
    }
    if (pWinInfo->m_pBtm[0] == 0) {
        assert_prog(D_00A56A68, D_00A56660, 1436);
    }
    if (pWinInfo->m_pBtm[1] == 0) {
        assert_prog(D_00A56A88, D_00A56660, 1437);
    }
    if (pWinInfo->m_pSide[0] == 0) {
        assert_prog(D_00A56AA8, D_00A56660, 1438);
    }
    if (pWinInfo->m_pSide[1] == 0) {
        assert_prog(D_00A56AC8, D_00A56660, 1439);
    }
    if (pWinInfo->m_pInner == 0) {
        assert_prog(D_00A56AE8, D_00A56660, 1440);
    }
    if (pWinInfo->m_pLeft == 0) {
        assert_prog(D_00A56B08, D_00A56660, 1441);
    }
    if (pWinInfo->m_pRight == 0) {
        assert_prog(D_00A56B28, D_00A56660, 1442);
    }
    if (pWinInfo->m_pMaru == 0) {
        assert_prog(D_00A56B48, D_00A56660, 1443);
    }
    if (pWinInfo->m_pBatu == 0) {
        assert_prog(D_00A56B68, D_00A56660, 1444);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _DispDetermWindow);

extern const char D_00A56BB0[]; /* "pDat->m_apPic[0] != NIL" */
extern const char D_00A56BC8[]; /* "pDat->m_apPic[1] != NIL" */

static void _InitFloatArrowHori(FloatArrow *pDat, RgBxx *pBxx)
{
    pDat->pic0 = RgBxxGetPic(pBxx, D_00A569D8);
    pDat->pic1 = RgBxxGetPic(pBxx, D_00A569F0);
    if (pDat->pic0 == 0) {
        assert_prog(D_00A56BB0, D_00A56660, 1534);
    }
    if (pDat->pic1 == 0) {
        assert_prog(D_00A56BC8, D_00A56660, 1535);
    }
    pDat->visibility_mask = 0;
    pDat->x0 = 0;
    pDat->x1 = 0;
    /*
     * gcc 2.96 -O2 reorders an unbroken run of independent stores by moving
     * the last one to the front; grouping this trailing pair in its own
     * scope keeps them in the original in-order sequence.
     */
    do {
        pDat->y0 = 0;
        pDat->y1 = 0;
    } while (0);
}

static void _SetFloatArrowPos(FloatArrow *arrow, int first_x, int first_y,
                              int second_x, int second_y, int offset_x,
                              int offset_y)
{
    arrow->x0 = first_x + offset_x;
    arrow->y0 = first_y + offset_y;
    arrow->x1 = second_x + offset_x;
    arrow->y1 = second_y + offset_y;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _DispFloatArrow);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _SetWepSelPicList);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _DispWepSelOverlaps);

/*
 * Local helpers still unallocated in this TU. Their own definitions stay
 * INCLUDE_ASM; these forward declarations only let the wrapper accessors
 * below call them (docs/tu-worker.md, "static for LOCAL symbols").
 */
static void _InitSelect(RgSelectAGWS *pSel);
static void _Destruct(RgSelectAGWS *pSel);
static void _PassTime(RgSelectAGWS *pSel, float deltaTime);
static void _Disp(RgSelectAGWS *pSel);
static void _SetBaseSelectData(SelDat *pSelDat, void *pData);
static int _GetWepSelection(SelDat *pSelDat, int *apCollects);
static int _GetSelectCharID(SelDat *pSelDat);
static void _SetPlayerModeSelectData(SelDat *pSelDat, int mode);

extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgHeapFree(void *heap, void *ptr, const char *source_file,
                       int line);

extern void RgDrawViewInit(RgDrawView *pView);

extern const char D_00A56C48[]; /* "pView != NIL" */

static void _init_camera(RgDrawView *pView)
{
    if (pView == 0) {
        assert_prog(D_00A56C48, D_00A56660, 1751);
    }
    RgDrawViewInit(pView);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _set_camera);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _Destruct_00A3B940);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _PassTime_00A3B9E8);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _Disp_00A3BE70);

extern RgFileSys *InstanceOfRgFileSys(void);
extern RgFileSysData *RgFileSysRead(RgFileSys *pSys, const char *pszName,
                                    const char *pszRoot);
extern void DisposeRgFileSysData_sub(RgFileSysData *pFile, const char *pszFile,
                                     int iLine);

/*
 * Opaque handle: CreateRgPicListFromBinary is defined by ov12/tu070
 * (src/ov12/rg_piclist.c), still INCLUDE_ASM there, and this call only
 * forwards its result, so an incomplete type is enough here.
 */
struct RgPicList;

extern struct RgPicList *CreateRgPicListFromBinary(struct RgBxx *pBxx, void *pData);

extern const char D_00A56928[]; /* "pBxx != NIL" */
extern const char D_00A56C90[]; /* "pszFileName != NIL" */
extern const char D_00A56CA8[]; /* "data\\nisimori\\" */

static struct RgPicList *_LoadPicList(const char *pszFileName, struct RgBxx *pBxx)
{
    RgFileSysData *pFile;
    struct RgPicList *pList;

    if (pBxx == 0) {
        assert_prog(D_00A56928, D_00A56660, 2052);
    }
    if (pszFileName == 0) {
        assert_prog(D_00A56C90, D_00A56660, 2053);
    }
    pFile = RgFileSysRead(InstanceOfRgFileSys(), pszFileName, D_00A56CA8);
    if (pFile == 0) {
        assert_prog(D_00A568C0, D_00A56660, 2056);
    }
    pList = CreateRgPicListFromBinary(pBxx, pFile->data);
    DisposeRgFileSysData_sub(pFile, D_00A56660, 2058);
    return pList;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", _InitSelect);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", InitRgSelectAGWSData);

extern const char D_00A568E0[]; /* "pSel != NIL" */
extern const char D_00A568C0[]; /* "pData != NIL" */

void RgSelectAGWSSetSelectData(RgSelectAGWS *pSel, void *pData)
{
    if (pSel == 0) {
        assert_prog(D_00A568E0, D_00A56660, 2143);
    }
    if (pData == 0) {
        assert_prog(D_00A568C0, D_00A56660, 2144);
    }
    _SetBaseSelectData(pSel->pSelDat, pData);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", RgSelectAGWSGetSelectData);

extern const char D_00A56D20[]; /* "apCollects != NIL" */

int RgSelectAGWSGetCollectData(RgSelectAGWS *pSel, int *apCollects)
{
    SelDat *pSelDat;

    if (pSel == 0) {
        assert_prog(D_00A568E0, D_00A56660, 2169);
    }
    if (apCollects == 0) {
        assert_prog(D_00A56D20, D_00A56660, 2170);
    }
    pSelDat = pSel->pSelDat;
    _GetWepSelection(pSelDat, apCollects);
    return _GetSelectCharID(pSelDat);
}

void RgSelectAGWSSetMode(RgSelectAGWS *pSel, int mode)
{
    _SetPlayerModeSelectData(pSel->pSelDat, mode);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_agws", RgSelectAGWSIsEnd);

int RgSelectAGWSGetCharacter(RgSelectAGWS *pSel)
{
    if (pSel == 0) {
        assert_prog(D_00A568E0, D_00A56660, 2213);
    }
    return _GetSelectCharID(pSel->pSelDat);
}

void RgSelectAGWSGetWeapon(RgSelectAGWS *pSel, int *apCollects)
{
    if (pSel == 0) {
        assert_prog(D_00A568E0, D_00A56660, 2221);
    }
    _GetWepSelection(pSel->pSelDat, apCollects);
}

RgSelectAGWS *CreateRgSelectAGWS(void)
{
    RgSelectAGWS *pSel;

    pSel = RgHeapAlloc(InstanceOfRgHeap(), RG_SELECT_AGWS_SIZE, D_00A56660,
                       2231);
    _InitSelect(pSel);
    return pSel;
}

void DisposeRgSelectAGWS(RgSelectAGWS *pSel)
{
    if (pSel == 0) {
        assert_prog(D_00A568E0, D_00A56660, 2239);
    }
    _Destruct(pSel);
    RgHeapFree(InstanceOfRgHeap(), pSel, D_00A56660, 2241);
}

void RgSelectAGWSPassTime(RgSelectAGWS *pSel, float deltaTime)
{
    if (pSel == 0) {
        assert_prog(D_00A568E0, D_00A56660, 2249);
    }
    _PassTime(pSel, deltaTime);
}

void RgSelectAGWSDisp(RgSelectAGWS *pSel)
{
    if (pSel == 0) {
        assert_prog(D_00A568E0, D_00A56660, 2256);
    }
    _Disp(pSel);
}
