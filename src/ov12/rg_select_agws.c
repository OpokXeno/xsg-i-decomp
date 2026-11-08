/*
 * OV12 original TU 69: 0x00a380d8..0x00a3c6a0 (103 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_bxx.h"
#include "ov12/rg_draw.h"
#include "ov12/rg_equip_type.h"
#include "ov12/rg_error.h"
#include "ov12/rg_filesys.h"
#include "ov12/rg_font.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_piclist.h"
#include "ov12/xrg_main_font.h"
#include "ov12/xrg_pad.h"
#include "ov12/xrg_paint2d.h"
#include "ov12/xrg_rand_int.h"
#include "ov12/xrg_sound.h"
#include "ov12/xrg_v_log.h"
#include "rg_select_agws.h"

/* Values of SelDat.mode: which selector is active. */
#define SEL_MODE_CHAR 0
#define SEL_MODE_WEAPON 1
#define SEL_MODE_OFF 2

/* Values of RgSelectAGWS.state. */
enum {
    AGWS_STATE_CHAR_SELECT,
    AGWS_STATE_WEAPON_SELECT,
    AGWS_STATE_SLIDE_TO_CHAR,
    AGWS_STATE_SLIDE_TO_WEAPON,
    AGWS_STATE_START_CHAR,
    AGWS_STATE_START_WEAPON,
    AGWS_STATE_DONE,
    AGWS_STATE_CANCELLED
};

/* The picture lists slide by this many pixels, SEL_SLIDE_STEP per frame. */
#define SEL_SLIDE_WIDTH 256
#define SEL_SLIDE_STEP 32

/* Phase step and range of the bobbing of the arrows. */
#define SEL_SIN_STEP 0.3926991f
#define SEL_PI 3.1415928f
#define SEL_TWO_PI 6.2831855f
#define SEL_ARROW_BOB 4

/* Weapon list layout: visible rows, row height, origin of the list. */
#define WEP_LIST_ROWS 7
#define WEP_LIST_ROW_HEIGHT 23
#define WEP_LIST_ORIGIN_X 568
#define WEP_LIST_ORIGIN_Y 168

/* Determination window text: x of the equip type label and of the weapon
 * name, and the row height. */
#define DETERM_LABEL_X 184
#define DETERM_NAME_X 232
#define DETERM_ROW_HEIGHT 26

/* Rows of the determination window between its top and bottom edges. */
#define DETERM_WINDOW_ROWS 8

/* Bits of FloatArrow.visibility_mask: the first arrow, both arrows. */
#define FLOAT_ARROW_FIRST 1
#define FLOAT_ARROW_BOTH 3

/* Glyphs of the weapon list font; the two arrows are the characters that
 * mark more rows above and below. */
#define WEP_FONT_MORE_ABOVE '@'
#define WEP_FONT_MORE_BELOW '!'
#define WEP_FONT_GLYPH_COUNT 0x29

/*
 * Character-select cursor layout: RG_ACTOR_CHAR_NUM character positions
 * (s_aeCharTbl) plus one more, SEL_CHAR_CURSOR_NOP. That is the position
 * _CharIDToCharCursor returns for an unknown character and
 * _CharCursorToCharID maps back to RG_ACTOR_CHAR_NOP; _InitSelectChar starts
 * the cursor there and _SetSelectableSelectChar keeps it selectable
 * (SEL_CHAR_CURSOR_NOP_BIT).
 */
#define SEL_CHAR_CURSOR_NOP RG_ACTOR_CHAR_NUM
#define SEL_CHAR_CURSOR_NUM (RG_ACTOR_CHAR_NUM + 1)
#define SEL_CHAR_CURSOR_NOP_BIT (1 << SEL_CHAR_CURSOR_NOP)

/* The "no character" id (assert "eCharID != RG_ACTOR_CHAR_NOP"). */
#define RG_ACTOR_CHAR_NOP (-1)

/*
 * SelType cursor positions: 0..2 are the equip types (_EquipCurIDToEquipType),
 * SEL_TYPE_CURSOR_DETERM is the position _SetDetermModeSelType moves to and
 * _PassTimeSelType never steps onto, SEL_TYPE_CURSOR_LAST the rightmost.
 */
#define SEL_TYPE_CURSOR_DETERM 3
#define SEL_TYPE_CURSOR_LAST 4

/* _EquipCurIDToEquipType's result for a cursor that is no equip type
 * (assert "eType != RG_EQUIP_TYPE_INVALID"). */
#define RG_EQUIP_TYPE_INVALID (-1)

/* The picture indices of one character on the character-select picture list. */
typedef struct RgSelCharPicEntry {
    int m_idx[6];
} RgSelCharPicEntry;

/* The picture index of each equip type's frame on the weapon picture list. */
typedef struct SelWepPicIdx {
    int m_idx[3];
} SelWepPicIdx;

/* A 4-component color copied as two doublewords (ld/sd); its components are
 * only read by RgPicSetColor. */
typedef union SelWepColor {
    long long halves[2];
    RgPicColor color;
} SelWepColor;

/*
 * Partial view of RgSelectAGWSData as doublewords (measured: a struct copy of
 * the 4-aligned record compiles to word copies or a call, the 8-aligned view
 * moves the block with doubleword loads and stores as the original does).
 */
typedef struct RgSelectAGWSDataWords {
    u64 word[sizeof(struct RgSelectAGWSData) / 8];
} RgSelectAGWSDataWords;

/* Messages, picture names and tables used by this TU. */
static RgFontColor s_aRed_1[2];
static RgFontColor s_aWhite_2[2];
static RgSelCharPicEntry s_ainPics_3[6];
extern const SelWepPicIdx agwsSelectWepPicIndexTable;
extern const SelWepColor agwsSelectConflictColor;
extern const SelWepColor agwsSelectNormalColor;
extern const char agwsSelectAssertCharIdNotNop[];
extern const char agwsSelectAssertNameNotNil[];
extern const char agwsSelectAssertNameArrayNotNil[];
extern const char agwsSelectWepListTextureName[];
extern const char agwsSelectAssertTextureNotNil[];
extern const char agwsSelectWepListFrameTextureName[];
extern const char agwsSelectUpArrowTextureName[];
extern const char agwsSelectDownArrowTextureName[];
extern const char agwsSelectCannotEquipForBattleText[];
extern const char agwsSelectCannotEquipTogetherText[];
extern const char agwsSelectDumpTitleFormat[];
extern const char agwsSelectDumpCharacterFormat[];
extern const char agwsSelectDumpWeaponNameFormat[];
extern const char agwsSelectAssertResultNotNil[];
extern const char agwsSelectPlayer1FrameTextureName[];
extern const char agwsSelectPlayer2FrameTextureName[];
extern const char agwsSelectAssertListNotNil[];
extern const char agwsSelectDetermLabelFullWidthL[];
extern const char agwsSelectDetermLabelFullWidthR[];
extern const char agwsSelectDetermLabelFullWidthLR[];
extern const char agwsSelectDetermConfirmText[];
extern const char agwsSelectDetermCancelText[];
extern const char agwsSelectAssertPaintNotNil[];
extern const char agwsSelectUnknownSelectModeFormat[];

static void _InitSelect(RgSelectAGWS *pSel);
static void _Destruct(RgSelectAGWS *pSel);
static void _PassTime(RgSelectAGWS *pSel, float deltaTime);
static void _Disp(RgSelectAGWS *pSel);

/* Defined here: the strings of this TU; the later ones stay in place among
 * the functions and are declared after them. */
const char agwsSelectAssertSelCharNotNil[16] = "pSelChar != NIL";

const char agwsSelectSourcePath[24] = "../rg_select_agws.euc.c";

const char agwsSelectAssertSelWepNotNil[16] = "pSelWep != NIL";

const char agwsSelectAssertSizePositive[16] = "uSize > 0";

const char agwsSelectAssertSizeWithinWepMax[24] = "uSize <= WEP_MAX";

const char agwsSelectAssertSelWepSizePositive[24] = "pSelWep->m_uSize > 0";

const char agwsSelectAssertCursorWithinSelWepSize[40] = "0 <= nCur && nCur < pSelWep->m_uSize";

const char agwsSelectUnknownTypeSelectIdFormat[32] = "unknown type select ID %d";

/* Declared here, defined further down: */
extern const char agwsSelectAssertSelTypeNotNil[];
extern const char agwsSelectAssertEquipTypeNotInvalid[];
extern const char agwsSelectAssertWepListDispNotNil[];
extern const char agwsSelectAssertSelDataNotNil[];
extern const char agwsSelectAssertDataNotNil[];
extern const char agwsSelectAssertSelNotNil[];
extern const char agwsSelectAssertInCharNotNop[];
extern const char agwsSelectAssertBxxNotNil[];
extern const char agwsSelectDetermFrameTopLeftTextureName[];
extern const char agwsSelectDetermFrameTopRightTextureName[];
extern const char agwsSelectDetermFrameBottomLeftTextureName[];
extern const char agwsSelectDetermFrameBottomRightTextureName[];
extern const char agwsSelectDetermFrameSideLeftTextureName[];
extern const char agwsSelectDetermFrameSideRightTextureName[];
extern const char agwsSelectDetermFrameInnerTextureName[];
extern const char agwsSelectLeftArrowTextureName[];
extern const char agwsSelectRightArrowTextureName[];
extern const char agwsSelectYesTextureName[];
extern const char agwsSelectNoTextureName[];
extern const char agwsSelectAssertWinTopLeftNotNil[];
extern const char agwsSelectAssertWinTopRightNotNil[];
extern const char agwsSelectAssertWinBottomLeftNotNil[];
extern const char agwsSelectAssertWinBottomRightNotNil[];
extern const char agwsSelectAssertWinSideLeftNotNil[];
extern const char agwsSelectAssertWinSideRightNotNil[];
extern const char agwsSelectAssertWinInnerNotNil[];
extern const char agwsSelectAssertWinLeftArrowNotNil[];
extern const char agwsSelectAssertWinRightArrowNotNil[];
extern const char agwsSelectAssertWinYesNotNil[];
extern const char agwsSelectAssertWinNoNotNil[];
extern const char agwsSelectAssertArrowPicLeftNotNil[];
extern const char agwsSelectAssertArrowPicRightNotNil[];
extern const char agwsSelectAssertViewNotNil[];
extern const char agwsSelectAssertFileNameNotNil[];
extern const char agwsSelectDataDirectory[];
extern const char agwsSelectBxxFileName[];
extern const char agwsSelectCannotLoadBxxError[];
extern const char agwsSelectAgwsTopPicListFileName[];
extern const char agwsSelectAgwsPicListFileName[];
extern const char agwsSelectWepTopPicListFileName[];
extern const char agwsSelectWepPicListFileName[];
extern const char agwsSelectAssertCollectsNotNil[];

static void _disp_pic(XrgPaint2D *paint, struct RgBxxPic *pic, int x, int y)
{
    XrgPaint2DUseTexture(paint, pic);
    XrgPaint2DAlpha(paint, XRG_PAINT2D_BLEND_LINEAR);
    XrgPaint2DDrawXYWH(paint, XRG_PAINT2D_MODE_PIC_SIZE, x, y, 0, 0);
}

/* The paint tint of each player. */
static XrgPaint2DRGBA s_aPlayerCol[2] = {{51, 124, 62, 127}, {123, 90, 64, 127}};

static void _disp_pic_pl(XrgPaint2D *paint, struct RgBxxPic *pic, int x, int y,
                         int playerIndex)
{
    XrgPaint2DUseTexture(paint, pic);
    XrgPaint2DAlpha(paint, XRG_PAINT2D_BLEND_LINEAR);
    XrgPaint2DColor(paint, &s_aPlayerCol[playerIndex]);
    XrgPaint2DDrawXYWH(paint, XRG_PAINT2D_MODE_PIC_SIZE, x, y, 0, 0);
}

static RgFontGlyph s_aFontTbl_0[WEP_FONT_GLYPH_COUNT] = {
    {' ', 0, 0, 0, {0, 0}, 0, 4},
    {'A', 0, 1, 1, {13, 14}, 0, -3},
    {'B', 0, 15, 1, {12, 14}, 0, -3},
    {'C', 0, 28, 1, {12, 14}, 0, -3},
    {'D', 0, 41, 1, {13, 14}, 0, -3},
    {'E', 0, 55, 1, {12, 14}, 0, -3},
    {'F', 0, 68, 1, {11, 14}, 0, -3},
    {'G', 0, 80, 1, {13, 14}, 0, -3},
    {'H', 0, 94, 1, {13, 14}, 0, -3},
    {'I', 0, 108, 1, {10, 14}, 0, -3},
    {'J', 0, 119, 1, {10, 14}, 0, -3},
    {'K', 0, 130, 1, {13, 14}, 0, -3},
    {'L', 0, 144, 1, {11, 14}, 0, -3},
    {'M', 0, 156, 1, {15, 14}, 0, -3},
    {'N', 0, 172, 1, {13, 14}, 0, -3},
    {'O', 0, 186, 1, {14, 14}, 0, -3},
    {'P', 0, 201, 1, {12, 14}, 0, -3},
    {'Q', 0, 214, 1, {14, 14}, 0, -3},
    {'R', 0, 229, 1, {13, 14}, 0, -3},
    {'S', 0, 243, 1, {12, 14}, 0, -3},
    {'T', 0, 256, 1, {12, 14}, 0, -3},
    {'U', 0, 269, 1, {13, 14}, 0, -3},
    {'V', 0, 283, 1, {12, 14}, 0, -3},
    {'W', 0, 296, 1, {17, 14}, 0, -3},
    {'X', 0, 314, 1, {12, 14}, 0, -3},
    {'Y', 0, 327, 1, {12, 14}, 0, -3},
    {'Z', 0, 340, 1, {12, 14}, 0, -3},
    {'0', 0, 1, 17, {12, 14}, 0, -3},
    {'1', 0, 14, 17, {12, 14}, 0, -3},
    {'2', 0, 27, 17, {12, 14}, 0, -3},
    {'3', 0, 40, 17, {12, 14}, 0, -3},
    {'4', 0, 53, 17, {12, 14}, 0, -3},
    {'5', 0, 66, 17, {12, 14}, 0, -3},
    {'6', 0, 79, 17, {12, 14}, 0, -3},
    {'7', 0, 92, 17, {12, 14}, 0, -3},
    {'8', 0, 105, 17, {12, 14}, 0, -3},
    {'9', 0, 118, 17, {12, 14}, 0, -3},
    {'-', 0, 131, 17, {9, 14}, 0, -3},
    {'?', 0, 141, 17, {11, 14}, 0, -3},
    {'!', 0, 153, 17, {16, 14}, 0, -3},
    {'@', 0, 170, 17, {16, 14}, 0, -3},
};


static RgFont *_CreateSelectWepFont(struct RgBxxPic *pic)
{
    return CreateRgFont(s_aFontTbl_0, WEP_FONT_GLYPH_COUNT, pic);
}

static float s_fSinRad = 0.0f;

static void _SinInit(void)
{
    s_fSinRad = 0.0f;
}

/* Advances the arrow bobbing phase by SEL_SIN_STEP, wrapped into -SEL_PI..SEL_PI. */
static void _SinPassTime(void)
{
    float phase = s_fSinRad + SEL_SIN_STEP;
    while (phase > SEL_PI)
        phase -= SEL_TWO_PI;
    while (phase < -SEL_PI)
        phase += SEL_TWO_PI;
    s_fSinRad = phase;
}

static int _SinGet(int amplitude)
{
    return (int)(sinf(s_fSinRad) * (float)amplitude);
}

/* Returns the equip conflict result of the weapon essences, one per equip type, at slot. */
static int _CheckEquipWeps(RgWeaponEssence **essences,
                           int slot)
{
    RgEquip *equipData[RG_EQUIP_TYPE_NUM];
    unsigned int i;

    for (i = 0; i < RG_EQUIP_TYPE_NUM; i++) {
        equipData[i] = &essences[i]->equip;
    }
    return RgEquipCheckConfrict(equipData, slot);
}

/* Character id of each character-select cursor position. */
static int s_aeCharTbl[RG_ACTOR_CHAR_NUM] = {0, 4, 2, 5, 3, 1};

static int _CharCursorToCharID(int charCursor)
{
    if ((unsigned int) charCursor >= RG_ACTOR_CHAR_NUM) {
        return RG_ACTOR_CHAR_NOP;
    }
    return s_aeCharTbl[charCursor];
}

static int _CharIDToCharCursor(int charID)
{
    unsigned int i;

    for (i = 0; i < RG_ACTOR_CHAR_NUM; i++) {
        if (s_aeCharTbl[i] == charID) {
            return i;
        }
    }
    return SEL_CHAR_CURSOR_NOP;
}


static void _InitSelectChar(SelChar *pSelChar)
{
    if (pSelChar == 0) {
        assert_prog(agwsSelectAssertSelCharNotNil, agwsSelectSourcePath, 0xD0);
    }
    pSelChar->cursor = SEL_CHAR_CURSOR_NOP;
    pSelChar->selectableMask = SEL_CHAR_CURSOR_NOP_BIT;
}

static int _GetSelectChar(SelChar *pSelChar)
{
    if (pSelChar == 0) {
        assert_prog(agwsSelectAssertSelCharNotNil, agwsSelectSourcePath, 0xD8);
    }
    return _CharCursorToCharID(pSelChar->cursor);
}

static int _GetSelectCursor(SelChar *pSelChar)
{
    if (pSelChar == 0) {
        assert_prog(agwsSelectAssertSelCharNotNil, agwsSelectSourcePath, 0xDE);
    }
    return pSelChar->cursor;
}

static void _SetSelectableSelectChar(SelChar *pSelChar, unsigned int charIDMask)
{
    unsigned int i;

    if (pSelChar == 0) {
        assert_prog(agwsSelectAssertSelCharNotNil, agwsSelectSourcePath, 0xE7);
    }
    pSelChar->selectableMask = 0;
    for (i = 0; i < RG_ACTOR_CHAR_NUM; i++) {
        if (charIDMask & (1 << i)) {
            pSelChar->selectableMask |= 1 << _CharIDToCharCursor(i);
        }
    }
    pSelChar->selectableMask |= SEL_CHAR_CURSOR_NOP_BIT;
    if ((pSelChar->selectableMask & (1 << pSelChar->cursor)) == 0) {
        for (i = 0; i < SEL_CHAR_CURSOR_NUM; i++) {
            if (pSelChar->selectableMask & (1 << i)) {
                pSelChar->cursor = i;
                break;
            }
        }
    }
}


/*
 * Moves the cursor to charID's position when that position is selectable;
 * otherwise it falls back to the highest selectable position.
 */
static void _SetCharSelectChar(SelChar *pSelChar, int charID)
{
    int cursor;
    unsigned int i;

    if (pSelChar == 0) {
        assert_prog(agwsSelectAssertSelCharNotNil, agwsSelectSourcePath, 0xFE);
    }
    if (charID == RG_ACTOR_CHAR_NOP) {
        assert_prog(agwsSelectAssertCharIdNotNop, agwsSelectSourcePath, 0xFF);
    }
    cursor = _CharIDToCharCursor(charID);
    if (pSelChar->selectableMask & (1 << cursor)) {
        pSelChar->cursor = cursor;
        return;
    }
    for (i = 0; i < SEL_CHAR_CURSOR_NUM; i++) {
        if (pSelChar->selectableMask & (1 << i)) {
            pSelChar->cursor = i;
        }
    }
}

static int _IsExistItemUpper(SelChar *pSelChar)
{
    if (pSelChar == 0) {
        assert_prog(agwsSelectAssertSelCharNotNil, agwsSelectSourcePath, 0x10F);
    }
    return (pSelChar->selectableMask & ((1 << pSelChar->cursor) - 1)) != 0;
}

static int _IsExistItemLower(SelChar *pSelChar)
{
    int cursorBit;

    if (pSelChar == 0) {
        assert_prog(agwsSelectAssertSelCharNotNil, agwsSelectSourcePath, 0x119);
    }
    cursorBit = 1 << pSelChar->cursor;
    return (pSelChar->selectableMask & ~((cursorBit - 1) | cursorBit)) != 0;
}

static int _GetSelectableSelectChar(SelChar *pSelChar)
{
    int charMask;
    unsigned int charID;

    if (pSelChar == 0) {
        assert_prog(agwsSelectAssertSelCharNotNil, agwsSelectSourcePath, 0x126);
    }
    charMask = 0;
    for (charID = 0; charID < RG_ACTOR_CHAR_NUM; charID++) {
        if ((pSelChar->selectableMask & (1 << _CharIDToCharCursor(charID))) != 0) {
            charMask |= 1 << charID;
        }
    }
    return charMask;
}


/*
 * Moves the cursor up or down to the next selectable position and plays the
 * cursor sound when it moves.
 */
static void _PassTimeSelectChar(SelChar *pSelChar, float deltaTime)
{
    int cursor;
    int oldCursor;
    int step;

    if (pSelChar == 0) {
        assert_prog(agwsSelectAssertSelCharNotNil, agwsSelectSourcePath, 0x133);
    }
    cursor = pSelChar->cursor;
    oldCursor = cursor;
    if (XrgPadIsUp() != 0) {
        step = -1;
    } else {
        step = XrgPadIsDown() != 0;
    }
    if (step != 0) {
        do {
            cursor += step;
            if ((unsigned int) cursor >= SEL_CHAR_CURSOR_NUM) {
                cursor = pSelChar->cursor;
                break;
            }
        } while ((pSelChar->selectableMask & (1 << cursor)) == 0);
    }
    pSelChar->cursor = cursor;
    if (cursor != oldCursor) {
        XrgSoundSystemCursor();
    }
}


static void _InitSelWep(SelWep *pSelWep)
{
    if (pSelWep == 0) {
        assert_prog(agwsSelectAssertSelWepNotNil, agwsSelectSourcePath, 0x157);
    }
    pSelWep->cursor = 0;
    pSelWep->m_uSize = 0;
}

/*
 * Fills the list with the weapons characterID can use for weaponType. The
 * first call passes no list and its count is checked against WEP_MAX before
 * the second fills pSelWep->list.
 */
static void _InitCharTypeSelWep(SelWep *pSelWep, int characterID,
                                int weaponType)
{
    unsigned int uSize;

    uSize = RgSelectWeapons(0, characterID, weaponType);
    if (uSize == 0) {
        assert_prog(agwsSelectAssertSizePositive, agwsSelectSourcePath, 0x160);
    }
    if (uSize > WEP_MAX) {
        assert_prog(agwsSelectAssertSizeWithinWepMax, agwsSelectSourcePath, 0x161);
    }
    if (pSelWep == 0) {
        assert_prog(agwsSelectAssertSelWepNotNil, agwsSelectSourcePath, 0x162);
    }
    uSize = RgSelectWeapons(pSelWep->list, characterID, weaponType);
    pSelWep->cursor = 0;
    pSelWep->m_uSize = uSize;
}


static RgWeaponEssence *_GetEssSelWep(SelWep *pSelWep)
{
    if (pSelWep == 0) {
        assert_prog(agwsSelectAssertSelWepNotNil, agwsSelectSourcePath, 0x16A);
    }
    if (pSelWep->m_uSize == 0) {
        assert_prog(agwsSelectAssertSelWepSizePositive, agwsSelectSourcePath, 0x16B);
    }
    return pSelWep->list[pSelWep->cursor];
}

static RgWeaponEssence **_GetEssListSelWep(SelWep *pSelWep)
{
    if (pSelWep == 0) {
        assert_prog(agwsSelectAssertSelWepNotNil, agwsSelectSourcePath, 0x172);
    }
    return pSelWep->list;
}

static unsigned int _GetEssListSizeSelWep(SelWep *pSelWep)
{
    if (pSelWep == 0) {
        assert_prog(agwsSelectAssertSelWepNotNil, agwsSelectSourcePath, 0x179);
    }
    return pSelWep->m_uSize;
}

static int _GetCursorSelWep(SelWep *pSelWep)
{
    if (pSelWep == 0) {
        assert_prog(agwsSelectAssertSelWepNotNil, agwsSelectSourcePath, 0x180);
    }
    return pSelWep->cursor;
}


static void _SetCursorSelWep(SelWep *pSelWep, int nCur)
{
    if (pSelWep == 0) {
        assert_prog(agwsSelectAssertSelWepNotNil, agwsSelectSourcePath, 0x187);
    }
    if (nCur < 0 || (unsigned int) nCur >= pSelWep->m_uSize) {
        assert_prog(agwsSelectAssertCursorWithinSelWepSize, agwsSelectSourcePath, 0x188);
    }
    pSelWep->cursor = nCur;
}


/*
 * Selects the first list entry whose name equals pszName and returns 1; the
 * cursor is 0 while nothing matches, and the result is 0.
 */
static int _SetCursorByNameSelWep(SelWep *pSelWep, char *pszName)
{
    unsigned int i;

    if (pSelWep == 0) {
        assert_prog(agwsSelectAssertSelWepNotNil, agwsSelectSourcePath, 401);
    }
    if (pszName == 0) {
        assert_prog(agwsSelectAssertNameNotNil, agwsSelectSourcePath, 402);
    }
    pSelWep->cursor = 0;
    for (i = 0; i < pSelWep->m_uSize; i++) {
        if (strcmp(pSelWep->list[i]->databaseName, pszName) == 0) {
            pSelWep->cursor = i;
            return 1;
        }
    }
    return 0;
}


static void _GetNameOnCursorSelWep(SelWep *pSelWep, char *name)
{
    if (pSelWep == 0) {
        assert_prog(agwsSelectAssertSelWepNotNil, agwsSelectSourcePath, 0x1A1);
    }
    if (pSelWep->m_uSize != 0) {
        strcpy(name, pSelWep->list[pSelWep->cursor]->databaseName);
        return;
    }
    *name = '\0';
}

/*
 * deltaTime is unused here, but the caller _PassTimeSelType still passes its
 * deltaTime in $f12 to this call.
 */
static void _PassTimeSelWep(SelWep *pSelWep, float deltaTime)
{
    int cursor;
    int previousCursor;

    if (pSelWep == 0) {
        assert_prog(agwsSelectAssertSelWepNotNil, agwsSelectSourcePath, 0x1AD);
    }
    if (pSelWep->m_uSize == 0) {
        assert_prog(agwsSelectAssertSelWepSizePositive, agwsSelectSourcePath, 0x1AE);
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


/*
 * Maps a cursor position to its equip type, or RG_EQUIP_TYPE_INVALID for a
 * position that is no type.
 */
static int _EquipCurIDToEquipType(int curID)
{
    int eType;

    eType = RG_EQUIP_TYPE_INVALID;
    switch (curID) {
    case 0:
        eType = 0;
        break;
    case 1:
        eType = 1;
        break;
    case 2:
        eType = 2;
        break;
    case 3:
    case 4:
        break;
    default:
        RgError(agwsSelectUnknownTypeSelectIdFormat, agwsSelectSourcePath, 0x1EC, curID);
        break;
    }
    return eType;
}

const char agwsSelectAssertSelTypeNotNil[16] = "pSelType != NIL";

const char agwsSelectAssertEquipTypeNotInvalid[32] = "eType != RG_EQUIP_TYPE_INVALID";

const char agwsSelectAssertWepListDispNotNil[16] = "pDisp != NIL";

const char agwsSelectAssertSelDataNotNil[16] = "pSelDat != NIL";

const char agwsSelectAssertDataNotNil[16] = "pData != NIL";

const char agwsSelectAssertSelNotNil[16] = "pSel != NIL";

const char agwsSelectAssertInCharNotNop[56] = "_GetSelectChar(&pSel->m_inChar) != RG_ACTOR_CHAR_NOP";

const char agwsSelectAssertBxxNotNil[16] = "pBxx != NIL";

const char agwsSelectDetermFrameTopLeftTextureName[16] = "frm11on_ul.bmp";

const char agwsSelectDetermFrameTopRightTextureName[16] = "frm11on_ur.bmp";

const char agwsSelectDetermFrameBottomLeftTextureName[16] = "frm11on_bl.bmp";

const char agwsSelectDetermFrameBottomRightTextureName[16] = "frm11on_br.bmp";

const char agwsSelectDetermFrameSideLeftTextureName[16] = "frm11on_cl.bmp";

const char agwsSelectDetermFrameSideRightTextureName[16] = "frm11on_cr.bmp";

const char agwsSelectDetermFrameInnerTextureName[16] = "frm11on_cc.bmp";

const char agwsSelectLeftArrowTextureName[24] = "icon_leftarrow.bmp";

const char agwsSelectRightArrowTextureName[24] = "icon_rightarrow.bmp";

const char agwsSelectYesTextureName[16] = "say_yes.bmp";

const char agwsSelectNoTextureName[16] = "say_no.bmp";

const char agwsSelectAssertWinTopLeftNotNil[32] = "pWinInfo->m_pTop[0] != NIL";

const char agwsSelectAssertWinTopRightNotNil[32] = "pWinInfo->m_pTop[1] != NIL";

const char agwsSelectAssertWinBottomLeftNotNil[32] = "pWinInfo->m_pBtm[0] != NIL";

const char agwsSelectAssertWinBottomRightNotNil[32] = "pWinInfo->m_pBtm[1] != NIL";

const char agwsSelectAssertWinSideLeftNotNil[32] = "pWinInfo->m_pSide[0] != NIL";

const char agwsSelectAssertWinSideRightNotNil[32] = "pWinInfo->m_pSide[1] != NIL";

const char agwsSelectAssertWinInnerNotNil[32] = "pWinInfo->m_pInner != NIL";

const char agwsSelectAssertWinLeftArrowNotNil[32] = "pWinInfo->m_pLeft != NIL";

const char agwsSelectAssertWinRightArrowNotNil[32] = "pWinInfo->m_pRight != NIL";

const char agwsSelectAssertWinYesNotNil[32] = "pWinInfo->m_pMaru != NIL";

const char agwsSelectAssertWinNoNotNil[32] = "pWinInfo->m_pBatu != NIL";

const char agwsSelectAssertArrowPicLeftNotNil[24] = "pDat->m_apPic[0] != NIL";

const char agwsSelectAssertArrowPicRightNotNil[24] = "pDat->m_apPic[1] != NIL";


static void _SetSelTypeCursor(SelType *pSelType, int mode)
{
    int cursor;

    if (pSelType == 0) {
        assert_prog(agwsSelectAssertSelTypeNotNil, agwsSelectSourcePath, 0x1F5);
    }
    cursor = pSelType->cursor;
    if (cursor != mode) {
        pSelType->oldCursor = cursor;
        pSelType->cursor = mode;
    }
}

static void _ResumeSelTypeOldCursor(SelType *pSelType)
{
    pSelType->cursor = pSelType->oldCursor;
}

static int _GetCursorSelType(SelType *pSelType)
{
    if (pSelType == 0) {
        assert_prog(agwsSelectAssertSelTypeNotNil, agwsSelectSourcePath, 517);
    }
    return pSelType->cursor;
}

static int _GetTypeSelType(SelType *pSelType)
{
    if (pSelType == 0) {
        assert_prog(agwsSelectAssertSelTypeNotNil, agwsSelectSourcePath, 524);
    }
    return _EquipCurIDToEquipType(pSelType->cursor);
}

/* Null when the cursor is not on an equip type. */
static SelWep *_GetCurSelType(SelType *pSelType)
{
    int eType;

    if (pSelType == 0) {
        assert_prog(agwsSelectAssertSelTypeNotNil, agwsSelectSourcePath, 533);
    }
    eType = _EquipCurIDToEquipType(pSelType->cursor);
    if (eType == RG_EQUIP_TYPE_INVALID) {
        return 0;
    }
    return &pSelType->wepList[eType];
}

static SelWep *_GetSelWepSelType(SelType *pSelType, int equipType)
{
    if (pSelType == 0) {
        assert_prog(agwsSelectAssertSelTypeNotNil, agwsSelectSourcePath, 544);
    }
    return &pSelType->wepList[equipType];
}

static int _GetConfrictSelType(SelType *pSelType, int charIndex)
{
    if (pSelType == 0) {
        assert_prog(agwsSelectAssertSelTypeNotNil, agwsSelectSourcePath, 551);
    }
    return pSelType->confrict[charIndex];
}

static void _SetDetermModeSelType(SelType *pSelType)
{
    if (pSelType == 0) {
        assert_prog(agwsSelectAssertSelTypeNotNil, agwsSelectSourcePath, 558);
    }
    _SetSelTypeCursor(pSelType, SEL_TYPE_CURSOR_DETERM);
}

static void _SetSpecModeSelType(SelType *pSelType, int mode)
{
    if (pSelType == 0) {
        assert_prog(agwsSelectAssertSelTypeNotNil, agwsSelectSourcePath, 566);
    }
    _SetSelTypeCursor(pSelType, mode);
}

static int _GetCurrentConfrictSelType(SelType *pSelType)
{
    int eType;

    if (pSelType == 0) {
        assert_prog(agwsSelectAssertSelTypeNotNil, agwsSelectSourcePath, 0x23F);
    }
    eType = _EquipCurIDToEquipType(pSelType->cursor);
    if (eType == RG_EQUIP_TYPE_INVALID) {
        assert_prog(agwsSelectAssertEquipTypeNotInvalid, agwsSelectSourcePath, 0x241);
    }
    return pSelType->confrict[eType];
}

static RgWeaponEssence *_GetCurrentWeaponSelType(SelType *pSelType)
{
    int eType;

    if (pSelType == 0) {
        assert_prog(agwsSelectAssertSelTypeNotNil, agwsSelectSourcePath, 0x24A);
    }
    eType = _EquipCurIDToEquipType(pSelType->cursor);
    if (eType == RG_EQUIP_TYPE_INVALID) {
        assert_prog(agwsSelectAssertEquipTypeNotInvalid, agwsSelectSourcePath, 0x24C);
    }
    return _GetEssSelWep(&pSelType->wepList[eType]);
}

static void _GetAllSelType(SelType *pSelType,
                           RgWeaponEssence **out)
{
    unsigned int i;

    for (i = 0; i < RG_EQUIP_TYPE_NUM; i++) {
        out[i] = _GetEssSelWep(&pSelType->wepList[i]);
    }
}


/* Writes the name under each equip type's cursor into aszName, WEP_NAME_SIZE bytes apiece. */
static void _GetAllNameOnCursorSelType(SelType *pSelType, char *aszName)
{
    unsigned int i;

    if (pSelType == 0) {
        assert_prog(agwsSelectAssertSelTypeNotNil, agwsSelectSourcePath, 0x25C);
    }
    if (aszName == 0) {
        assert_prog(agwsSelectAssertNameArrayNotNil, agwsSelectSourcePath, 0x25D);
    }
    for (i = 0; i < RG_EQUIP_TYPE_NUM; i++) {
        _GetNameOnCursorSelWep(&pSelType->wepList[i], aszName + i * WEP_NAME_SIZE);
    }
}

/* Recomputes the conflict result of each equip type from the weapons under its cursor. */
static void _CheckConfrictSelType(SelType *pSelType)
{
    RgWeaponEssence *essences[RG_EQUIP_TYPE_NUM];
    unsigned int i;

    for (i = 0; i < RG_EQUIP_TYPE_NUM; i++) {
        essences[i] = _GetEssSelWep(&pSelType->wepList[i]);
    }
    for (i = 0; i < RG_EQUIP_TYPE_NUM; i++) {
        pSelType->confrict[i] = _CheckEquipWeps(essences, i);
    }
}


/* Moves each equip type's cursor to the weapon named in aszName, then rechecks conflicts. */
static void _SetAllCursorByNameSelType(SelType *pSelType, char *aszName)
{
    unsigned int i;

    if (pSelType == 0) {
        assert_prog(agwsSelectAssertSelTypeNotNil, agwsSelectSourcePath, 0x272);
    }
    if (aszName == 0) {
        assert_prog(agwsSelectAssertNameArrayNotNil, agwsSelectSourcePath, 0x273);
    }
    for (i = 0; i < RG_EQUIP_TYPE_NUM; i++) {
        _SetCursorByNameSelWep(&pSelType->wepList[i], aszName + i * WEP_NAME_SIZE);
    }
    _CheckConfrictSelType(pSelType);
}

/* Clears the cursors, conflict results and weapon lists, and sets the character to none. */
static void _InitSelType(SelType *pSelType)
{
    unsigned int i;

    if (pSelType == 0) {
        assert_prog(agwsSelectAssertSelTypeNotNil, agwsSelectSourcePath, 0x283);
    }
    pSelType->cursor = 0;
    pSelType->oldCursor = 0;
    for (i = 0; i < RG_EQUIP_TYPE_NUM; i++) {
        _InitSelWep(&pSelType->wepList[i]);
        pSelType->confrict[i] = 0;
    }
    pSelType->charID = RG_ACTOR_CHAR_NOP;
}

/* Resets the selector and loads the weapon list of each equip type for charID. */
static void _InitByCharSelType(SelType *pSelType, int charID)
{
    unsigned int i;

    if (pSelType == 0) {
        assert_prog(agwsSelectAssertSelTypeNotNil, agwsSelectSourcePath, 0x293);
    }
    _InitSelType(pSelType);
    for (i = 0; i < RG_EQUIP_TYPE_NUM; i++) {
        _InitCharTypeSelWep(&pSelType->wepList[i], charID, i);
    }
    pSelType->charID = charID;
    _CheckConfrictSelType(pSelType);
}


/*
 * Moves the equip type cursor left or right on pad input, skipping the
 * determination position.
 */
static void _PassTimeSelType(SelType *pSelType, float deltaTime)
{
    int cursor;
    int newCursor;
    int eType;

    if (pSelType == 0) {
        assert_prog(agwsSelectAssertSelTypeNotNil, agwsSelectSourcePath, 0x2A3);
    }
    cursor = pSelType->cursor;
    newCursor = cursor;
    eType = _EquipCurIDToEquipType(cursor);
    if (eType != RG_EQUIP_TYPE_INVALID) {
        _PassTimeSelWep(&pSelType->wepList[eType], deltaTime);
    }
    if (cursor != SEL_TYPE_CURSOR_DETERM) {
        if (XrgPadIsLeft() != 0 && cursor != 0) {
            newCursor = cursor - 1;
            if (newCursor == SEL_TYPE_CURSOR_DETERM) {
                newCursor = SEL_TYPE_CURSOR_DETERM - 1;
            }
        }
        if (XrgPadIsRight() != 0 && (unsigned int) newCursor < SEL_TYPE_CURSOR_LAST) {
            newCursor++;
            if (newCursor == SEL_TYPE_CURSOR_DETERM) {
                newCursor = SEL_TYPE_CURSOR_DETERM + 1;
            }
        }
    }
    _SetSelTypeCursor(pSelType, newCursor);
    if (pSelType->cursor != cursor) {
        XrgSoundSystemCursor();
    }
    _CheckConfrictSelType(pSelType);
}

/*
 * If the current equip type conflicts, resets the cursor of each conflicting
 * list and returns 1.
 */
static int _DetachWeaponIfConfrictSelType(SelType *pSelType)
{
    int eType;
    unsigned int confrict;
    int i;

    if (pSelType == 0) {
        assert_prog(agwsSelectAssertSelTypeNotNil, agwsSelectSourcePath, 0x2CA);
    }
    eType = _EquipCurIDToEquipType(pSelType->cursor);
    if (eType != RG_EQUIP_TYPE_INVALID) {
        confrict = pSelType->confrict[eType];
        if (confrict != 0) {
            for (i = 0; i < RG_EQUIP_TYPE_NUM; i++) {
                if (confrict & (1 << i)) {
                    _SetCursorSelWep(&pSelType->wepList[i], 0);
                }
            }
            _CheckConfrictSelType(pSelType);
            return 1;
        }
    }
    return 0;
}


/*
 * Sets up a weapon list display: its font, frame and arrow pictures from pBxx,
 * and its default origin.
 */
static void _InitWepListDisp(WepListDisp *pDisp, RgBxx *pBxx)
{
    struct RgBxxPic *pTex;

    if (pDisp == 0) {
        assert_prog(agwsSelectAssertWepListDispNotNil, agwsSelectSourcePath, 748);
    }
    pTex = RgBxxGetPic(pBxx, agwsSelectWepListTextureName);
    if (pTex == 0) {
        assert_prog(agwsSelectAssertTextureNotNil, agwsSelectSourcePath, 751);
    }
    pDisp->font = _CreateSelectWepFont(pTex);
    pDisp->pFrame = RgBxxGetPic(pBxx, agwsSelectWepListFrameTextureName);
    pDisp->pUpArrow = RgBxxGetPic(pBxx, agwsSelectUpArrowTextureName);
    pDisp->pDownArrow = RgBxxGetPic(pBxx, agwsSelectDownArrowTextureName);
    pDisp->originX = WEP_LIST_ORIGIN_X;
    pDisp->originY = WEP_LIST_ORIGIN_Y;
    pDisp->topIndex = 0;
    pDisp->pSelType = 0;
    pDisp->pSelWep = 0;
}


/* Allocates a weapon list display and initializes it from pBxx. */
static WepListDisp *_CreateWepListDisp(RgBxx *pBxx)
{
    WepListDisp *pDisp;

    pDisp = RgHeapAlloc(InstanceOfRgHeap(), sizeof(WepListDisp), agwsSelectSourcePath,
                        767);
    _InitWepListDisp(pDisp, pBxx);
    return pDisp;
}


static void _DestructWepListDisp(WepListDisp *pDisp)
{
    if (pDisp == 0) {
        assert_prog(agwsSelectAssertWepListDispNotNil, agwsSelectSourcePath, 775);
    }
    if (pDisp->font != 0) {
        DisposeRgFont(pDisp->font);
    }
}


static void _DisposeWepListDisp(WepListDisp *pDisp)
{
    if (pDisp == 0) {
        assert_prog(agwsSelectAssertWepListDispNotNil, agwsSelectSourcePath, 0x30F);
    }
    _DestructWepListDisp(pDisp);
    RgHeapFree(InstanceOfRgHeap(), pDisp, agwsSelectSourcePath, 0x311);
}

static void _SetSelectorWepListDisp(WepListDisp *pDisp, SelType *pSelType)
{
    if (pDisp == 0) {
        assert_prog(agwsSelectAssertWepListDispNotNil, agwsSelectSourcePath, 791);
    }
    pDisp->pSelType = pSelType;
    pDisp->pSelWep = 0;
    pDisp->topIndex = 0;
}


/*
 * Draws the visible rows of the selected equip type's weapon list with the
 * cursor frame, the scroll arrows and the name or conflict message of the
 * current weapon.
 */
static void _DrawWepListDisp(WepListDisp *pDisp, XrgPaint2D *paint, int ofsX,
                             int ofsY)
{
    int equipType;
    int xBase;
    int yBase;
    int cursor;
    int essSize;
    RgWeaponEssence **pSlot;
    int endIdx;
    int maxTop;
    int x;
    int y;
    int i;
    int selected;
    int confrict;
    int topIndex;
    RgWeaponEssence **essList;
    unsigned int curConfrict;
    RgWeaponEssence *ess;
    SelWep *pCur;
    RgWeaponEssence *essences[RG_EQUIP_TYPE_NUM + 1];

    if (pDisp == 0) {
        assert_prog(agwsSelectAssertWepListDispNotNil, agwsSelectSourcePath, 813);
    }
    if (pDisp->pSelType == 0) {
        return;
    }
    _GetCursorSelType(pDisp->pSelType);
    equipType = _GetTypeSelType(pDisp->pSelType);
    pCur = _GetCurSelType(pDisp->pSelType);
    if (pCur == 0) {
        return;
    }
    _GetAllSelType(pDisp->pSelType, essences);
    if (pCur != pDisp->pSelWep) {
        pDisp->pSelWep = pCur;
        pDisp->topIndex = _GetCursorSelWep(pCur) - 3;
    }
    topIndex = pDisp->topIndex;
    cursor = _GetCursorSelWep(pCur);
    essSize = _GetEssListSizeSelWep(pCur);
    if (topIndex > cursor - 2) {
        topIndex = cursor - 2;
    }
    if (topIndex < cursor - 4) {
        topIndex = cursor - 4;
    }
    maxTop = essSize - WEP_LIST_ROWS;
    if (maxTop < 0) {
        maxTop = 0;
    }
    if (topIndex < 0) {
        topIndex = 0;
    } else if (topIndex > maxTop) {
        topIndex = maxTop;
    }
    pDisp->topIndex = topIndex;
    x = pDisp->originX + ofsX;
    y = pDisp->originY + ofsY;
    xBase = x;
    yBase = y;
    essList = _GetEssListSelWep(pCur);
    endIdx = topIndex + WEP_LIST_ROWS;

    for (i = topIndex; i < endIdx && i < essSize; i++) {
        selected = (i == cursor);
        ess = essList[i];
        pSlot = &essences[equipType];
        *pSlot = ess;
        confrict = _CheckEquipWeps(essences, equipType);
        if (selected != 0) {
            XrgPaint2DUseTexture(paint, pDisp->pFrame);
            XrgPaint2DAlpha(paint, XRG_PAINT2D_BLEND_LINEAR);
            XrgPaint2DDrawXYWH(paint, XRG_PAINT2D_MODE_PIC_SIZE, x - 9, y - 8, 0, 0);
        }
        if (!confrict) {
            RgFontSetColor(pDisp->font, &s_aWhite_2[selected]);
        } else {
            RgFontSetColor(pDisp->font, &s_aRed_1[selected]);
        }
        RgFontStr(paint, pDisp->font, ess->databaseName, x, y, 0);
        y += WEP_LIST_ROW_HEIGHT;
    }

    /*
     * Up arrow when the cursor is below the top row, down arrow when it is not
     * on the last weapon.
     */
    if (topIndex < cursor) {
        x += 108;
        y = yBase - _SinGet(SEL_ARROW_BOB) + 58;
        XrgPaint2DUseTexture(paint, pDisp->pUpArrow);
        XrgPaint2DAlpha(paint, XRG_PAINT2D_BLEND_LINEAR);
        XrgPaint2DDrawXYWH(paint, XRG_PAINT2D_MODE_PIC_SIZE, x, y, 0, 0);
    }
    if (cursor < essSize - 1) {
        x = xBase + 108;
        y = yBase + _SinGet(SEL_ARROW_BOB) + 102;
        XrgPaint2DUseTexture(paint, pDisp->pDownArrow);
        XrgPaint2DAlpha(paint, XRG_PAINT2D_BLEND_LINEAR);
        XrgPaint2DDrawXYWH(paint, XRG_PAINT2D_MODE_PIC_SIZE, x, y, 0, 0);
    }

    /* Glyphs marking more rows above and below the visible window. */
    RgFontSetDefaultColor(pDisp->font);
    if (topIndex > 0) {
        RgFontPut(paint, pDisp->font, WEP_FONT_MORE_ABOVE, xBase + 32, yBase - 20, 1);
    }
    if (topIndex < essSize - WEP_LIST_ROWS) {
        RgFontPut(paint, pDisp->font, WEP_FONT_MORE_BELOW, xBase + 32, yBase + 156, 1);
    }

    /* Name of the current weapon, or the conflict message in its place. */
    x = xBase - 48;
    y = yBase + 192;
    ess = _GetCurrentWeaponSelType(pDisp->pSelType);
    curConfrict = _GetCurrentConfrictSelType(pDisp->pSelType);
    if (!curConfrict) {
        XrgDispMainFont(x, y, ess->displayName, 0);
    } else if (curConfrict & (1 << equipType)) {
        XrgDispMainFont(x, y, agwsSelectCannotEquipForBattleText, 0);
    } else {
        XrgDispMainFont(x, y, agwsSelectCannotEquipTogetherText, 0);
    }
}

/*
 * Initializes a selector to character selection, with no character and the
 * weapon lists cleared.
 */
static void _InitSelectData(SelDat *pSelDat)
{
    SelChar *pSelChar;
    SelType *pSelType;

    pSelChar = &pSelDat->selChar;
    pSelType = &pSelDat->selType;
    if (pSelDat == 0) {
        assert_prog(agwsSelectAssertSelDataNotNil, agwsSelectSourcePath, 0x3CD);
    }
    pSelDat->mode = SEL_MODE_CHAR;
    pSelDat->playerMode = 0;
    _InitSelectChar(pSelChar);
    _InitSelType(pSelType);
    _InitByCharSelType(pSelType, _GetSelectChar(pSelChar));
    InitRgSelectAGWSData(&pSelDat->base);
}

/* Logs the selected and available characters and the weapon name table of pData. */
static void _DumpSelect__(struct RgSelectAGWSData *pData, const char *pszTitle, int nValue)
{
    unsigned int i;
    unsigned int j;

    XrgLog(agwsSelectDumpTitleFormat, agwsSelectSourcePath, 0x3DD, pszTitle, nValue);
    XrgLog(agwsSelectDumpCharacterFormat, agwsSelectSourcePath, 0x3DE, pData->selectedCharacter, pData->availableCharacters);
    for (i = 0; i < RG_ACTOR_CHAR_NUM; i++) {
        for (j = 0; j < RG_EQUIP_TYPE_NUM; j++) {
            XrgLog(agwsSelectDumpWeaponNameFormat, agwsSelectSourcePath, 0x3E1, i, j, pData->weaponName[i][j]);
        }
    }
}

/* Saves the selected and the selectable characters of pSelDat into pData. */
static void _SaveCharSelectData(SelDat *pSelDat,
                                struct RgSelectAGWSData *pData)
{
    if (pSelDat == 0) {
        assert_prog(agwsSelectAssertSelDataNotNil, agwsSelectSourcePath, 0x3E9);
    }
    if (pData == 0) {
        assert_prog(agwsSelectAssertDataNotNil, agwsSelectSourcePath, 0x3EA);
    }
    pData->selectedCharacter = _GetSelectChar(&pSelDat->selChar);
    pData->availableCharacters = _GetSelectableSelectChar(&pSelDat->selChar);
}

/*
 * The result is declared int although no caller (_TransitToCharSelect and
 * _SaveToBaseData below) reads it: the original enters
 * _GetAllNameOnCursorSelType with jal and returns through this function's
 * own epilogue, instead of releasing the frame and jumping to it as a void
 * result would allow.
 */
static int _SaveWepSelectData(SelDat *pSelDat,
                              struct RgSelectAGWSData *pData)
{
    SelType *pSelType;
    int charID;
    int nameIndex;

    if (pSelDat == 0) {
        assert_prog(agwsSelectAssertSelDataNotNil, agwsSelectSourcePath, 1012);
    }
    if (pData == 0) {
        assert_prog(agwsSelectAssertDataNotNil, agwsSelectSourcePath, 1013);
    }
    charID = _GetSelectChar(&pSelDat->selChar);
    pSelType = &pSelDat->selType;
    nameIndex = charID * RG_EQUIP_TYPE_NUM;
    if ((unsigned int) charID < RG_ACTOR_CHAR_NUM) {
        /* nameIndex counts name rows: character * equip types. */
        _GetAllNameOnCursorSelType(pSelType, pData->weaponName[0][nameIndex]);
    }
}

/* Restores the selected and the selectable characters of pSelDat from pData. */
static void _LoadCharSelectData(SelDat *pSelDat,
                                struct RgSelectAGWSData *pData)
{
    if (pSelDat == 0) {
        assert_prog(agwsSelectAssertSelDataNotNil, agwsSelectSourcePath, 1027);
    }
    if (pData == 0) {
        assert_prog(agwsSelectAssertDataNotNil, agwsSelectSourcePath, 1028);
    }
    _SetCharSelectChar(&pSelDat->selChar, pData->selectedCharacter);
    _SetSelectableSelectChar(&pSelDat->selChar, pData->availableCharacters);
}

/*
 * Declared int for the same reason as _SaveWepSelectData above: the original
 * enters _SetAllCursorByNameSelType with jal and returns through its own
 * epilogue instead of tail-jumping to it.
 */
static int _LoadWepSelectData(SelDat *pSelDat,
                              struct RgSelectAGWSData *pData)
{
    SelType *pSelType;
    int charID;

    pSelType = &pSelDat->selType;
    if (pSelDat == 0) {
        assert_prog(agwsSelectAssertSelDataNotNil, agwsSelectSourcePath, 0x40E);
    }
    if (pData == 0) {
        assert_prog(agwsSelectAssertDataNotNil, agwsSelectSourcePath, 0x40F);
    }
    charID = _GetSelectChar(&pSelDat->selChar);
    _InitByCharSelType(pSelType, charID);
    if ((unsigned int) charID < RG_ACTOR_CHAR_NUM) {
        _SetAllCursorByNameSelType(pSelType, pData->weaponName[charID][0]);
    }
}


static void _GetBaseSelectData(SelDat *pSelDat,
                               struct RgSelectAGWSData *pResult)
{
    if (pSelDat == 0) {
        assert_prog(agwsSelectAssertSelDataNotNil, agwsSelectSourcePath, 1058);
    }
    if (pResult == 0) {
        assert_prog(agwsSelectAssertResultNotNil, agwsSelectSourcePath, 1059);
    }
    *(RgSelectAGWSDataWords *) pResult = *(RgSelectAGWSDataWords *) &pSelDat->base;
}

/*
 * Loads pData into pSelDat: copies the base record, then restores the
 * character and weapon selections.
 */
static void _SetBaseSelectData(SelDat *pSelDat,
                               struct RgSelectAGWSData *pData)
{
    if (pSelDat == 0) {
        assert_prog(agwsSelectAssertSelDataNotNil, agwsSelectSourcePath, 1068);
    }
    if (pData == 0) {
        assert_prog(agwsSelectAssertDataNotNil, agwsSelectSourcePath, 1069);
    }
    *(RgSelectAGWSDataWords *) &pSelDat->base = *(RgSelectAGWSDataWords *) pData;
    _LoadCharSelectData(pSelDat, pData);
    _LoadWepSelectData(pSelDat, pData);
}

static int _GetModeSelectData(SelDat *pSelDat)
{
    if (pSelDat == 0) {
        assert_prog(agwsSelectAssertSelDataNotNil, agwsSelectSourcePath, 1084);
    }
    return pSelDat->mode;
}

static SelDat *_CreateSelectData(void)
{
    SelDat *pSelDat;

    pSelDat = RgHeapAlloc(InstanceOfRgHeap(), sizeof(SelDat), agwsSelectSourcePath,
                          1091);
    _InitSelectData(pSelDat);
    return pSelDat;
}

static void _DisposeSelectData(SelDat *pSelDat)
{
    if (pSelDat == 0) {
        assert_prog(agwsSelectAssertSelDataNotNil, agwsSelectSourcePath, 1099);
    }
    RgHeapFree(InstanceOfRgHeap(), pSelDat, agwsSelectSourcePath, 1100);
}

/* Switches from character selection to weapon selection, saving the character choice first. */
static void _TransitToWeaponSelect(SelDat *pSel)
{
    struct RgSelectAGWSData *pData;

    pData = &pSel->base;
    if (pSel == 0) {
        assert_prog(agwsSelectAssertSelNotNil, agwsSelectSourcePath, 0x452);
    }
    if (_GetSelectChar(&pSel->selChar) == RG_ACTOR_CHAR_NOP) {
        assert_prog(agwsSelectAssertInCharNotNop, agwsSelectSourcePath, 0x453);
    }
    pSel->mode = SEL_MODE_WEAPON;
    _SaveCharSelectData(pSel, pData);
    _LoadWepSelectData(pSel, pData);
}

/*
 * Switches from weapon selection back to character selection, saving the
 * weapon choices first.
 */
static void _TransitToCharSelect(SelDat *pSel)
{
    struct RgSelectAGWSData *pData;

    pData = &pSel->base;
    if (pSel == 0) {
        assert_prog(agwsSelectAssertSelNotNil, agwsSelectSourcePath, 0x462);
    }
    _SaveWepSelectData(pSel, pData);
    _LoadCharSelectData(pSel, pData);
    pSel->mode = SEL_MODE_CHAR;
}

/* Writes the current weapon and character selections into the base record. */
static void _SaveToBaseData(SelDat *pSel)
{
    struct RgSelectAGWSData *pData;

    pData = &pSel->base;
    _SaveWepSelectData(pSel, pData);
    _SaveCharSelectData(pSel, pData);
}

static void _TransitToOffSelect(SelDat *pSel)
{
    if (pSel == 0) {
        assert_prog(agwsSelectAssertSelNotNil, agwsSelectSourcePath, 1143);
    }
    pSel->mode = SEL_MODE_OFF;
}

static void _SetPlayerModeSelectData(SelDat *pSelDat, int mode)
{
    if (pSelDat == 0) {
        assert_prog(agwsSelectAssertSelNotNil, agwsSelectSourcePath, 1150);
    }
    pSelDat->playerMode = mode;
}

static int _GetPlayerModeSelectData(SelDat *pSel)
{
    if (pSel == 0) {
        assert_prog(agwsSelectAssertSelNotNil, agwsSelectSourcePath, 1157);
    }
    return pSel->playerMode;
}


/* Passes the time to the active selector and sets the pad to the player of playerMode. */
static void _PassTimeSelectData(SelDat *pSel, float deltaTime)
{
    if (pSel == 0) {
        assert_prog(agwsSelectAssertSelNotNil, agwsSelectSourcePath, 1164);
    }
    if (pSel->playerMode == 0) {
        XrgPadSetID(0);
    } else {
        XrgPadSetID(1);
    }
    switch (pSel->mode) {
    case SEL_MODE_WEAPON:
        _PassTimeSelType(&pSel->selType, deltaTime);
        return;
    do {
    case SEL_MODE_CHAR:
        _PassTimeSelectChar(&pSel->selChar, deltaTime);
        return;
    } while (0);
    }
}

static int _GetSelectCharID(SelDat *pSelDat)
{
    if (pSelDat == 0) {
        assert_prog(agwsSelectAssertSelNotNil, agwsSelectSourcePath, 1187);
    }
    return _GetSelectChar(&pSelDat->selChar);
}

/*
 * Fills essences with the current weapon of each equip type and confricts with
 * its conflict result.
 */
static void _GetSelectWeps(SelDat *pSel,
                           RgWeaponEssence **essences,
                           int *confricts)
{
    SelType *pSelType;
    unsigned int i;

    if (pSel == 0) {
        assert_prog(agwsSelectAssertSelNotNil, agwsSelectSourcePath, 0x4AC);
    }
    pSelType = &pSel->selType;
    _GetAllSelType(pSelType, essences);
    for (i = 0; i < RG_EQUIP_TYPE_NUM; i++) {
        confricts[i] = _GetConfrictSelType(pSelType, i);
    }
}

/*
 * Stores the weapon of each equip type in apCollects, taking the first entry of
 * a list that conflicts. Returns the mask of the equip types that conflict.
 */
static int _GetWepSelection(SelDat *pSelDat,
                            RgWeaponEssence **apCollects)
{
    SelWep *pSelWep;
    RgWeaponEssence **essList;
    unsigned int i;
    int confrictMask;

    confrictMask = 0;
    for (i = 0; i < RG_EQUIP_TYPE_NUM; i++) {
        pSelWep = _GetSelWepSelType(&pSelDat->selType, i);
        if (pSelWep == 0) {
            assert_prog(agwsSelectAssertSelWepNotNil, agwsSelectSourcePath, 0x4C0);
        }
        essList = _GetEssListSelWep(pSelWep);
        if (_GetConfrictSelType(&pSelDat->selType, i) != 0) {
            confrictMask |= 1 << i;
            *apCollects = essList[0];
        } else {
            *apCollects = _GetEssSelWep(pSelWep);
        }
        apCollects++;
    }
    return confrictMask;
}


/* Draws the frame of the player selected by playerMode (0 or 1). */
static void _DrawPlayerMode(XrgPaint2D *paint, RgBxx *pBxx, int playerMode)
{
    struct RgBxxPic *pic;

    pic = 0;
    if (pBxx == 0) {
        assert_prog(agwsSelectAssertBxxNotNil, agwsSelectSourcePath, 0x4D4);
    }
    switch (playerMode) {
    case 0:
        pic = RgBxxGetPic(pBxx, agwsSelectPlayer1FrameTextureName);
        break;
    case 1:
        pic = RgBxxGetPic(pBxx, agwsSelectPlayer2FrameTextureName);
        break;
    }
    if (pic != 0) {
        _disp_pic_pl(paint, pic, 4, 0x2C, playerMode);
    }
}


/*
 * Updates the character-select picture list: the state of each character cell
 * from the selectable mask and the cursor, and the scroll arrows.
 */
static void _SetCharSelDisp(RgPicList *pList, SelChar *pSelChar, int playerMode)
{
    int i;
    unsigned int cursor;
    int flag;
    unsigned int selectable;
    unsigned int selChar;
    int off;
    void *pA;
    void *pB;
    RgPic *pUp;
    const char *table = (const char *) s_ainPics_3;
    RgPic *pDown;

    if (pList == 0) {
        assert_prog(agwsSelectAssertListNotNil, agwsSelectSourcePath, 1292);
    }
    if (pSelChar == 0) {
        assert_prog(agwsSelectAssertSelCharNotNil, agwsSelectSourcePath, 1293);
    }
    i = 0;
    off = 0;
    selChar = _GetSelectChar(pSelChar);
    cursor = _GetSelectCursor(pSelChar);
    selectable = _GetSelectableSelectChar(pSelChar);

    for (; i < RG_ACTOR_CHAR_NUM; i++, off += 24) {
        if ((unsigned int) i < RG_ACTOR_CHAR_NUM) {
            RgPic *pC;
            RgPic *pD;
            RgPic *pE;
            pA = RgPicListGetPic(pList, ((const RgSelCharPicEntry *) (table + off))->m_idx[2]);
            table = pA;
            if (pA != 0) {
                table = (const char *) s_ainPics_3;
            } else if (selChar != 0) {
                table = (const char *) s_ainPics_3;
            } else {
                table = (const char *) s_ainPics_3;
            }
            pB = RgPicListGetPic(pList, ((const RgSelCharPicEntry *) (off + table))->m_idx[3]);
            table = pB;
            pC = RgPicListGetPic(pList, s_ainPics_3[i].m_idx[4]);
            pD = RgPicListGetPic(pList, s_ainPics_3[i].m_idx[5]);
            pE = RgPicListGetPic(pList, s_ainPics_3[i].m_idx[1]);
            table = (const char *) s_ainPics_3;
            if ((selectable & (1 << i)) != 0) {
                if (pA) {
                    RgPicSetAlpha(pA, 3);
                    if (selChar == i) {
                        RgPicNotDraw(pA, 0);
                    } else {
                        RgPicNotDraw(pA, 1);
                    }
                }
                if (0 != pB) {
                    RgPicSetAlpha(pB, 3);
                    RgPicSetDark(pB, 1);
                    if (selChar == i) {
                        RgPicNotDraw(pB, 1);
                    } else {
                        RgPicNotDraw(pB, 0);
                    }
                }
                if (0 != pC) {
                    RgPicNotDraw(pC, 1);
                }
                if (0 != pD) {
                    RgPicNotDraw(pD, 1);
                }
                if (0 != pE) {
                    RgPicNotDraw(pE, 0);
                }
            } else {
                if (pA != 0) {
                    RgPicNotDraw(pA, 1);
                }
                if (pB != 0) {
                    RgPicNotDraw(pB, 1);
                }
                if (pE != 0) {
                    RgPicNotDraw(pE, 1);
                }
                if (pC != 0) {
                    RgPicNotDraw(pC, 0);
                }
                if (pD != 0) {
                    RgPicNotDraw(pD, 0);
                    RgPicSetDark(pD, 1);
                }
            }
        }
    }

    /* Pictures 44 and 57 are darkened while the cursor is on a character. */
    flag = (cursor != SEL_CHAR_CURSOR_NOP);
    RgPicSetDark(RgPicListGetPic(pList, 44), flag);
    RgPicSetDark(RgPicListGetPic(pList, 57), flag);

    for (i = 0; RG_ACTOR_CHAR_NUM > i; i++) {
        if ((unsigned int) i < RG_ACTOR_CHAR_NUM) {
            pA = RgPicListGetPic(pList, s_ainPics_3[i].m_idx[0]);
            pB = RgPicListGetPic(pList, s_ainPics_3[i].m_idx[1]);
            if (pA != 0) {
                if (selChar == i) {
                    RgPicSetDark(pA, 0);
                } else {
                    RgPicSetDark(pA, 1);
                }
            }
            if (pB != 0) {
                if (selChar == i) {
                    RgPicSetDark(pB, 0);
                } else {
                    RgPicSetDark(pB, 1);
                }
            }
        }
    }

    /* Picture 58 is drawn while an item lies above the cursor, picture 59 while one lies below; both bob vertically. */
    {
        pUp = RgPicListGetPic(pList, 58);

        if (pUp != 0) {
            if (_IsExistItemUpper(pSelChar) != 0) {
                RgPicNotDraw(pUp, 0);
            } else {
                RgPicNotDraw(pUp, 1);
            }
            RgPicSetOfsXY(pUp, 0, -_SinGet(SEL_ARROW_BOB));
        }
    }
    {
        pDown = RgPicListGetPic(pList, 59);

        if (pDown != 0) {
            if (_IsExistItemLower(pSelChar) != 0) {
                RgPicNotDraw(pDown, 0);
            } else {
                RgPicNotDraw(pDown, 1);
            }
            RgPicSetOfsXY(pDown, 0, _SinGet(SEL_ARROW_BOB));
        }
    }
}


/* Loads the pictures of the determination window into pWinInfo and checks each one. */
static void _InitDetermWin(WinInfo *pWinInfo, RgBxx *pBxx, int value0, int value1)
{
    struct RgBxxPic *pBatu;

    pWinInfo->m_pTop[0] = RgBxxGetPic(pBxx, agwsSelectDetermFrameTopLeftTextureName);
    pWinInfo->m_pTop[1] = RgBxxGetPic(pBxx, agwsSelectDetermFrameTopRightTextureName);
    pWinInfo->m_pBtm[0] = RgBxxGetPic(pBxx, agwsSelectDetermFrameBottomLeftTextureName);
    pWinInfo->m_pBtm[1] = RgBxxGetPic(pBxx, agwsSelectDetermFrameBottomRightTextureName);
    pWinInfo->m_pSide[0] = RgBxxGetPic(pBxx, agwsSelectDetermFrameSideLeftTextureName);
    pWinInfo->m_pSide[1] = RgBxxGetPic(pBxx, agwsSelectDetermFrameSideRightTextureName);
    pWinInfo->m_pInner = RgBxxGetPic(pBxx, agwsSelectDetermFrameInnerTextureName);
    pWinInfo->m_pLeft = RgBxxGetPic(pBxx, agwsSelectLeftArrowTextureName);
    pWinInfo->m_pRight = RgBxxGetPic(pBxx, agwsSelectRightArrowTextureName);
    pWinInfo->m_pMaru = RgBxxGetPic(pBxx, agwsSelectYesTextureName);
    pBatu = RgBxxGetPic(pBxx, agwsSelectNoTextureName);
    pWinInfo->m_nValue0 = value0;
    pWinInfo->m_pBatu = pBatu;
    pWinInfo->m_nValue1 = value1;
    if (pWinInfo->m_pTop[0] == 0) {
        assert_prog(agwsSelectAssertWinTopLeftNotNil, agwsSelectSourcePath, 1434);
    }
    if (pWinInfo->m_pTop[1] == 0) {
        assert_prog(agwsSelectAssertWinTopRightNotNil, agwsSelectSourcePath, 1435);
    }
    if (pWinInfo->m_pBtm[0] == 0) {
        assert_prog(agwsSelectAssertWinBottomLeftNotNil, agwsSelectSourcePath, 1436);
    }
    if (pWinInfo->m_pBtm[1] == 0) {
        assert_prog(agwsSelectAssertWinBottomRightNotNil, agwsSelectSourcePath, 1437);
    }
    if (pWinInfo->m_pSide[0] == 0) {
        assert_prog(agwsSelectAssertWinSideLeftNotNil, agwsSelectSourcePath, 1438);
    }
    if (pWinInfo->m_pSide[1] == 0) {
        assert_prog(agwsSelectAssertWinSideRightNotNil, agwsSelectSourcePath, 1439);
    }
    if (pWinInfo->m_pInner == 0) {
        assert_prog(agwsSelectAssertWinInnerNotNil, agwsSelectSourcePath, 1440);
    }
    if (pWinInfo->m_pLeft == 0) {
        assert_prog(agwsSelectAssertWinLeftArrowNotNil, agwsSelectSourcePath, 1441);
    }
    if (pWinInfo->m_pRight == 0) {
        assert_prog(agwsSelectAssertWinRightArrowNotNil, agwsSelectSourcePath, 1442);
    }
    if (pWinInfo->m_pMaru == 0) {
        assert_prog(agwsSelectAssertWinYesNotNil, agwsSelectSourcePath, 1443);
    }
    if (pWinInfo->m_pBatu == 0) {
        assert_prog(agwsSelectAssertWinNoNotNil, agwsSelectSourcePath, 1444);
    }
}



/*
 * Draws the determination window: its frame, the weapon name of each equip
 * type and the confirm and cancel texts.
 */
static void _DispDetermWindow(XrgPaint2D *paint, WinInfo *pWinInfo,
                              SelDat *pSelDat)
{
    unsigned int charID;
    int innerX;
    unsigned int i;
    unsigned int j;
    RgWeaponEssence *apCollects[RG_EQUIP_TYPE_NUM];
    struct RgBxxPic *inner;
    struct RgBxxPic *side1;
    struct RgBxxPic *side0;
    int buttonX;
    int y;
    int x;
    int baseY;
    SelDat *selData;
    int originX;
    int coord;

    coord = pWinInfo->m_nValue0;
    y = pWinInfo->m_nValue1;
    pWinInfo->m_nValue0 = coord;
    x = coord;
    _disp_pic(paint, pWinInfo->m_pTop[0], x, y);
    originX = x;
    baseY = y;
    _disp_pic(paint, pWinInfo->m_pTop[1],
              x + pWinInfo->m_pTop[0]->width, y);
    coord = y + pWinInfo->m_pTop[1]->height;
    y = coord;
    for (i = 0; i < DETERM_WINDOW_ROWS; i++) {
        side0 = pWinInfo->m_pSide[0];
        side1 = pWinInfo->m_pSide[1];
        inner = pWinInfo->m_pInner;
        _disp_pic(paint, side0, x, y);
        _disp_pic(paint, inner, x + side0->width, y);
        _disp_pic(paint, side1, x + side0->width + inner->width, y);
        y = y + inner->height;
    }
    _disp_pic(paint, pWinInfo->m_pBtm[0], x, y);
    x += pWinInfo->m_pBtm[0]->width;
    buttonX = originX + 281;
    _disp_pic(paint, pWinInfo->m_pBtm[1], x, y);
    y = 183;
    _disp_pic(paint, pWinInfo->m_pMaru, buttonX, baseY + 130);
    _disp_pic(paint, pWinInfo->m_pBatu, buttonX, baseY + 154);

    selData = pSelDat;
    charID = _GetWepSelection(selData, apCollects);
    _GetSelectCharID(selData);
    for (j = 0; j < RG_EQUIP_TYPE_NUM; j++) {
        switch (j) {
        case 0:
            XrgDispMainFont(DETERM_LABEL_X, y, agwsSelectDetermLabelFullWidthL, 0);
            break;
        case 1:
            XrgDispMainFont(DETERM_LABEL_X, y, agwsSelectDetermLabelFullWidthR, 0);
            break;
        case 2:
            XrgDispMainFont(DETERM_LABEL_X, y, agwsSelectDetermLabelFullWidthLR, 0);
            break;
        }
        XrgDispMainFont(DETERM_NAME_X, y,
                        apCollects[j]->displayName,
                        charID & (1u << j));
        y += DETERM_ROW_HEIGHT;
    }
    XrgDispMainFont(372, y + 12, agwsSelectDetermConfirmText, 0);
    XrgDispMainFont(372, y + 36, agwsSelectDetermCancelText, 0);
}

/* Loads the left and right arrow pictures and clears the arrow positions and visibility. */
static void _InitFloatArrowHori(FloatArrow *pDat, RgBxx *pBxx)
{
    pDat->m_apPic[0] = RgBxxGetPic(pBxx, agwsSelectLeftArrowTextureName);
    pDat->m_apPic[1] = RgBxxGetPic(pBxx, agwsSelectRightArrowTextureName);
    if (pDat->m_apPic[0] == 0) {
        assert_prog(agwsSelectAssertArrowPicLeftNotNil, agwsSelectSourcePath, 1534);
    }
    if (pDat->m_apPic[1] == 0) {
        assert_prog(agwsSelectAssertArrowPicRightNotNil, agwsSelectSourcePath, 1535);
    }
    pDat->visibility_mask = 0;
    pDat->m_aiX[0] = 0;
    pDat->m_aiX[1] = 0;
    /*
     * gcc 2.96 -O2 reorders an unbroken run of independent stores by moving
     * the last one to the front; grouping this trailing pair in its own
     * scope keeps them in the original in-order sequence.
     */
    do {
        pDat->m_aiY[0] = 0;
        pDat->m_aiY[1] = 0;
    } while (0);
}

/* Sets the positions of both arrows from two points, each shifted by the offset. */
static void _SetFloatArrowPos(FloatArrow *arrow, int first_x, int first_y,
                              int second_x, int second_y, int offset_x,
                              int offset_y)
{
    arrow->m_aiX[0] = first_x + offset_x;
    arrow->m_aiY[0] = first_y + offset_y;
    arrow->m_aiX[1] = second_x + offset_x;
    arrow->m_aiY[1] = second_y + offset_y;
}


/* Draws each visible arrow, bobbing it horizontally by SEL_ARROW_BOB. */
static void _DispFloatArrow(XrgPaint2D *paint, FloatArrow *pDat)
{
    int aOfs[2] = {-1, 1};
    unsigned int i;
    struct RgBxxPic *pic;

    for (i = 0; i < 2; i++) {
        if (pDat->visibility_mask & (1 << i)) {
            pic = pDat->m_apPic[i];
            if (pic != 0) {
                _disp_pic(paint, pic,
                          pDat->m_aiX[i] + aOfs[i] * _SinGet(SEL_ARROW_BOB),
                          pDat->m_aiY[i]);
            }
        }
    }
}


/*
 * Updates the weapon-select picture list while the weapon selector is active:
 * the conflict color of each equip type and the state of the cursor pictures.
 */
static void _SetWepSelPicList(RgPicList *pList, SelDat *pSelDat)
{
    unsigned int slot;
    unsigned int cursor;
    SelWepPicIdx picIdx;

    picIdx = agwsSelectWepPicIndexTable;
    if (_GetModeSelectData(pSelDat) == SEL_MODE_WEAPON) {
        RgPic *pic;
        int equipType;
        SelType *pSelType = &pSelDat->selType;
        const SelWepColor *pConfrictColor;
        const SelWepColor *pNormalColor;

        cursor = _GetCursorSelType(pSelType);
        equipType = _GetTypeSelType(pSelType);
        RgPicNotDraw(RgPicListGetPic(pList, 17), 1);
        RgPicNotDraw(RgPicListGetPic(pList, 18), 1);
        RgPicNotDraw(RgPicListGetPic(pList, 4), 1);
        RgPicNotDraw(RgPicListGetPic(pList, 3), 1);
        pConfrictColor = &agwsSelectConflictColor;
        pNormalColor = &agwsSelectNormalColor;

        slot = 0;
        while (RG_EQUIP_TYPE_NUM > slot) {
            RgPic *loopPic;
            SelWepColor colorA;
            SelWepColor colorB;

            colorA = *pConfrictColor;
            colorB = *pNormalColor;
            loopPic = RgPicListGetPic(pList, picIdx.m_idx[slot]);
            RgPicSetDark(loopPic, 1);
            if (_GetConfrictSelType(pSelType, slot) != 0) {
                RgPicSetColor(loopPic, &colorA.color);
            } else {
                RgPicSetColor(loopPic, &colorB.color);
            }
            slot++;
        }
        RgPicSetDark(RgPicListGetPic(pList, 16), 1);
        RgPicSetDark(RgPicListGetPic(pList, 15), 1);
        /* Picture 14 is darkened unless the cursor is before SEL_TYPE_CURSOR_DETERM. */
        pic = RgPicListGetPic(pList, 14);
        RgPicSetAlpha(pic, 2);
        if (cursor < SEL_TYPE_CURSOR_DETERM) {
            RgPicSetDark(pic, 0);
        } else {
            RgPicSetDark(pic, 1);
        }
        switch (cursor) {
        case RG_EQUIP_TYPE_LEFT:
        case RG_EQUIP_TYPE_RIGHT:
        case RG_EQUIP_TYPE_BACK:
            RgPicSetDark(RgPicListGetPic(pList, picIdx.m_idx[equipType]), 0);
            break;
        case SEL_TYPE_CURSOR_LAST:
            RgPicSetDark(RgPicListGetPic(pList, 16), 0);
            RgPicSetDark(RgPicListGetPic(pList, 15), 0);
            break;
        default:
            break;
        }
    }
}


/*
 * Draws the overlays of the weapon selector: the arrows beside the equip types,
 * or the determination window at its position.
 */
static void _DispWepSelOverlaps(XrgPaint2D *paint, RgBxx *pBxx, SelDat *pSelDat,
                                int ofsX, int ofsY)
{
    WinInfo win;
    FloatArrow arrow;
    int cursor;

    if (paint == 0) {
        assert_prog(agwsSelectAssertPaintNotNil, agwsSelectSourcePath, 1651);
    }
    if (pBxx == 0) {
        assert_prog(agwsSelectAssertBxxNotNil, agwsSelectSourcePath, 1652);
    }
    if (pSelDat == 0) {
        assert_prog(agwsSelectAssertSelDataNotNil, agwsSelectSourcePath, 1653);
    }
    if (_GetModeSelectData(pSelDat) == SEL_MODE_WEAPON) {
        _InitFloatArrowHori(&arrow, pBxx);
        cursor = _GetCursorSelType(&pSelDat->selType);
        _SetFloatArrowPos(&arrow, 256, 103, 463, 104, 256, 0);
        switch (cursor) {
        case RG_EQUIP_TYPE_LEFT:
            arrow.visibility_mask = FLOAT_ARROW_BOTH;
            break;
        case RG_EQUIP_TYPE_RIGHT:
            arrow.visibility_mask = FLOAT_ARROW_BOTH;
            break;
        case RG_EQUIP_TYPE_BACK:
            arrow.visibility_mask = FLOAT_ARROW_BOTH;
            break;
        case SEL_TYPE_CURSOR_DETERM:
            _InitDetermWin(&win, pBxx, 167 - ofsX, 144 - ofsY);
            _DispDetermWindow(paint, &win, pSelDat);
            arrow.visibility_mask = 0;
            break;
        case SEL_TYPE_CURSOR_LAST:
            _SetFloatArrowPos(&arrow, 353, 411, 0, 0, -ofsX, -ofsY);
            arrow.visibility_mask = FLOAT_ARROW_FIRST;
            break;
        }
        _DispFloatArrow(paint, &arrow);
    }
}

const char agwsSelectAssertViewNotNil[16] = "pView != NIL";


static void _init_camera(RgDrawView *pView)
{
    if (pView == 0) {
        assert_prog(agwsSelectAssertViewNotNil, agwsSelectSourcePath, 1751);
    }
    RgDrawViewInit(pView);
}


/* Places the view at (0, 1, 4) with no rotation. */
static void _set_camera(RgDrawView *pView)
{
    RgVector position;

    XrgClearVector(position);
    position[1] = 1.0f;
    position[2] = 4.0f;
    RgDrawViewSetPosition(pView, position);
    RgDrawViewSetRotateX(pView, 0.0f);
    RgDrawViewSetRotateY(pView, 0.0f);
    RgDrawViewSetRotateZ(pView, 0.0f);
}


/*
 * Releases the paint, the picture lists, the selector data and the weapon list
 * display of pSel.
 */
void _Destruct(RgSelectAGWS *pSel) {
    if (pSel == 0) {
        assert_prog(agwsSelectAssertSelNotNil, agwsSelectSourcePath, 0x6F0);
    }
    DisposeXrgPaint2D_sub(pSel->paint, agwsSelectSourcePath, 0x6F2);
    DisposeRgPicList(pSel->pWepSel);
    DisposeRgPicList(pSel->pWepSelTop);
    DisposeRgPicList(pSel->pAgwsSel);
    DisposeRgPicList(pSel->pAgwsSelTop);
    _DisposeSelectData(pSel->pSelDat);
    _DisposeWepListDisp(pSel->pWepListDisp);
    DisposeRgSelectRobot(pSel->pRobot);
    DisposeRgBxx_sub(pSel->pBxx, agwsSelectSourcePath, 0x6FA);
}


/*
 * Advances the select screen state machine by one frame and updates the robot
 * and the selection record.
 */
static void _PassTime(RgSelectAGWS *pSel, float deltaTime)
{
    SelDat *pSelDat;
    SelType *pSelType;
    struct RgSelectAGWSData *pData;
    struct RgSelectRobotChoice aSelection;
    unsigned int cursor;
    int type;
    int charID;

    pSelDat = pSel->pSelDat;
    if (pSel == 0) {
        assert_prog(agwsSelectAssertSelNotNil, agwsSelectSourcePath, 1798);
    }
    switch (pSel->state) {
    case AGWS_STATE_CHAR_SELECT:
        RgSelectRobotScreenPos(pSel->pRobot, 0.0f);
        RgSelectRobotSetMode(pSel->pRobot, -1);
        pData = &pSelDat->base;
        _SaveCharSelectData(pSelDat, pData);
        _LoadWepSelectData(pSelDat, pData);
        if (XrgPadIsRight() != 0) {
            charID = _GetSelectCharID(pSelDat);
            if (charID != RG_ACTOR_CHAR_NOP) {
                pSel->state = AGWS_STATE_SLIDE_TO_WEAPON;
                _TransitToOffSelect(pSelDat);
                XrgSoundSystemCursor();
                break;
            }
        }
        if (XrgPadIsMaru() != 0) {
            if (_GetSelectCharID(pSelDat) == RG_ACTOR_CHAR_NOP) {
                pSel->state = AGWS_STATE_CANCELLED;
                XrgSoundSystemCancel();
            } else {
                pSel->state = AGWS_STATE_SLIDE_TO_WEAPON;
                _TransitToOffSelect(pSelDat);
                XrgSoundSystemOk();
            }
        }
        break;
    case AGWS_STATE_WEAPON_SELECT:
        pSelType = &pSelDat->selType;
        cursor = _GetCursorSelType(pSelType);
        type = _GetTypeSelType(pSelType);
        RgSelectRobotSetMode(pSel->pRobot, type);
        RgSelectRobotScreenPos(pSel->pRobot, 1.0f);
        pData = &pSelDat->base;
        _SaveWepSelectData(pSelDat, pData);
        if (XrgPadIsLR() != 0) {
            if (cursor != SEL_TYPE_CURSOR_DETERM) {
                _SetSpecModeSelType(pSelType, RG_EQUIP_TYPE_BACK);
                XrgSoundSystemCursor();
            }
        } else if (XrgPadIsL12() != 0) {
            if (cursor != SEL_TYPE_CURSOR_DETERM) {
                _SetSpecModeSelType(pSelType, RG_EQUIP_TYPE_LEFT);
                XrgSoundSystemCursor();
            }
        } else if (XrgPadIsR12() != 0) {
            if (cursor != SEL_TYPE_CURSOR_DETERM) {
                _SetSpecModeSelType(pSelType, RG_EQUIP_TYPE_RIGHT);
                XrgSoundSystemCursor();
            }
        } else if (XrgPadIsLeft() != 0) {
            if (cursor == RG_EQUIP_TYPE_LEFT) {
                pSel->state = AGWS_STATE_SLIDE_TO_CHAR;
                _TransitToOffSelect(pSelDat);
                XrgSoundSystemCursor();
            }
        } else if (XrgPadIsMaru() != 0) {
            switch (cursor) {
            case RG_EQUIP_TYPE_LEFT:
            case RG_EQUIP_TYPE_RIGHT:
            case RG_EQUIP_TYPE_BACK:
                if (_DetachWeaponIfConfrictSelType(pSelType) == 0) {
                    _SetDetermModeSelType(pSelType);
                    XrgSoundSystemOk();
                } else {
                    XrgSoundSystemNG();
                }
                break;
            case SEL_TYPE_CURSOR_DETERM:
                pSel->state = AGWS_STATE_DONE;
                XrgSoundSystemOk();
                break;
            case SEL_TYPE_CURSOR_LAST:
                pSel->state = AGWS_STATE_SLIDE_TO_CHAR;
                _TransitToOffSelect(pSelDat);
                XrgSoundSystemCancel();
                break;
            }
        } else if (XrgPadIsBatu() != 0) {
            if (cursor != SEL_TYPE_CURSOR_DETERM) {
                pSel->state = AGWS_STATE_SLIDE_TO_CHAR;
                _TransitToOffSelect(pSelDat);
                XrgSoundSystemCancel();
            } else {
                _ResumeSelTypeOldCursor(pSelType);
                XrgSoundSystemCancel();
            }
        }
        break;
    /* Slides the picture lists back to the character side, then starts character selection. */
    case AGWS_STATE_SLIDE_TO_CHAR:
        if (pSel->ofsX < 0) {
            pSel->ofsX += SEL_SLIDE_STEP;
            if (pSel->ofsX > 0) {
                pSel->ofsX = 0;
            }
            if (pSel->ofsX >= 0) {
                pSel->state = AGWS_STATE_START_CHAR;
            }
        } else {
            pSel->state = AGWS_STATE_START_CHAR;
        }
        RgSelectRobotScreenPos(pSel->pRobot,
                               (float) -pSel->ofsX * (1.0f / SEL_SLIDE_WIDTH));
        break;
    /* Slides the picture lists to the weapon side, then starts weapon selection. */
    case AGWS_STATE_SLIDE_TO_WEAPON:
        if (pSel->ofsX < -(SEL_SLIDE_WIDTH - 1)) {
            pSel->state = AGWS_STATE_START_WEAPON;
        } else {
            pSel->ofsX -= SEL_SLIDE_STEP;
            if (pSel->ofsX < -SEL_SLIDE_WIDTH) {
                pSel->ofsX = -SEL_SLIDE_WIDTH;
            }
            if (pSel->ofsX < -(SEL_SLIDE_WIDTH - 1)) {
                pSel->state = AGWS_STATE_START_WEAPON;
            }
        }
        _TransitToWeaponSelect(pSelDat);
        RgSelectRobotScreenPos(pSel->pRobot,
                               (float) -pSel->ofsX * (1.0f / SEL_SLIDE_WIDTH));
        break;
    case AGWS_STATE_START_CHAR:
        pSel->state = AGWS_STATE_CHAR_SELECT;
        RgPicListSetDrawEnable(pSel->pAgwsSelTop, 1);
        RgPicListSetDrawEnable(pSel->pWepSelTop, 0);
        _TransitToCharSelect(pSelDat);
        break;
    case AGWS_STATE_START_WEAPON:
        pSel->state = AGWS_STATE_WEAPON_SELECT;
        RgPicListSetDrawEnable(pSel->pAgwsSelTop, 0);
        RgPicListSetDrawEnable(pSel->pWepSelTop, 1);
        _SetSelectorWepListDisp(pSel->pWepListDisp, &pSelDat->selType);
        break;
    case AGWS_STATE_DONE:
    case AGWS_STATE_CANCELLED:
        break;
    default:
        RgError(agwsSelectUnknownSelectModeFormat, agwsSelectSourcePath, 1987, pSel->state);
        break;
    }
    /*
     * Applies this frame's pad input to the selectors, then sends the
     * selection to the robot.
     */
    _PassTimeSelectData(pSelDat, deltaTime);
    RgSelectRobotPassTime(pSel->pRobot, deltaTime);
    aSelection.actorID = _GetSelectCharID(pSelDat);
    _GetSelectWeps(pSelDat, aSelection.weapons, aSelection.weaponActive);
    RgSelectRobotSet(pSel->pRobot, &aSelection);
}

const char agwsSelectAssertFileNameNotNil[24] = "pszFileName != NIL";

const char agwsSelectDataDirectory[16] = "data\\nisimori\\";

const char agwsSelectBxxFileName[16] = "select.bxx";

const char agwsSelectCannotLoadBxxError[24] = "cannot load select.bxx";

const char agwsSelectAgwsTopPicListFileName[16] = "agwsseltop.r2d";

const char agwsSelectAgwsPicListFileName[16] = "agwssel.r2d";

const char agwsSelectWepTopPicListFileName[16] = "wepseltop.r2d";

const char agwsSelectWepPicListFileName[16] = "wepsel.r2d";

const char agwsSelectAssertCollectsNotNil[24] = "apCollects != NIL";


/*
 * Draws the select screen: the robot, the character and weapon picture lists,
 * the weapon list and the overlays.
 */
void _Disp(RgSelectAGWS *pSel) {
    SelDat *pSelDat;

    if (pSel == 0) {
        assert_prog(agwsSelectAssertSelNotNil, agwsSelectSourcePath, 0x7DA);
    }
    pSelDat = pSel->pSelDat;
    _SinPassTime();
    RgSelectRobotDisp(pSel->pRobot);
    _DrawPlayerMode(pSel->paint, pSel->pBxx, _GetPlayerModeSelectData(pSelDat));
    _SetCharSelDisp(pSel->pAgwsSel, &pSelDat->selChar, _GetPlayerModeSelectData(pSelDat));
    _SetWepSelPicList(pSel->pWepSel, pSelDat);
    _DrawWepListDisp(pSel->pWepListDisp, pSel->paint, pSel->ofsX, pSel->ofsY);
    RgPicSetAlpha(RgPicListGetPic(pSel->pAgwsSelTop, 0), 3);
    RgPicSetAlpha(RgPicListGetPic(pSel->pWepSelTop, 0x19), 3);
    RgPicListDraw(pSel->pAgwsSelTop, pSel->paint);
    RgPicListDraw(pSel->pWepSelTop, pSel->paint);
    RgPicListSetOffset(pSel->pAgwsSel, pSel->ofsX, pSel->ofsY);
    RgPicListSetOffset(pSel->pWepSel, pSel->ofsX, pSel->ofsY);
    RgPicListDraw(pSel->pAgwsSel, pSel->paint);
    RgPicListDraw(pSel->pWepSel, pSel->paint);
    _DispWepSelOverlaps(pSel->paint, pSel->pBxx, pSelDat, pSel->ofsX, pSel->ofsY);
    XrgPaint2DFlush(pSel->paint);
}

/* Loads the picture list stored in the data file pszFileName, with the textures of pBxx. */
static struct RgPicList *_LoadPicList(const char *pszFileName, struct RgBxx *pBxx)
{
    RgFileSysData *pFile;
    struct RgPicList *pList;

    if (pBxx == 0) {
        assert_prog(agwsSelectAssertBxxNotNil, agwsSelectSourcePath, 2052);
    }
    if (pszFileName == 0) {
        assert_prog(agwsSelectAssertFileNameNotNil, agwsSelectSourcePath, 2053);
    }
    pFile = RgFileSysRead(InstanceOfRgFileSys(), pszFileName, agwsSelectDataDirectory);
    if (pFile == 0) {
        assert_prog(agwsSelectAssertDataNotNil, agwsSelectSourcePath, 2056);
    }
    pList = CreateRgPicListFromBinary(pBxx, pFile->data);
    DisposeRgFileSysData_sub(pFile, agwsSelectSourcePath, 2058);
    return pList;
}


/*
 * Creates the select screen: its data, camera, BXX resource, picture lists,
 * weapon list, paint object and robot.
 */
void _InitSelect(RgSelectAGWS *pSel) {
    RgBxx *pBxx;
    RgDrawStudio *pStudio;

    if (pSel == 0) {
        assert_prog(agwsSelectAssertSelNotNil, agwsSelectSourcePath, 0x810);
    }
    pSel->state = AGWS_STATE_START_CHAR;
    pSel->pSelDat = _CreateSelectData();
    pStudio = RgDrawGetStudio(InstanceOfRgDraw(), 0);
    pSel->pStudio = pStudio;
    _init_camera(RgDrawStudioGetView(pStudio));
    _set_camera(RgDrawStudioGetView(pSel->pStudio));
    pBxx = LoadRgBxx_sub(agwsSelectBxxFileName, agwsSelectSourcePath, 0x827);
    pSel->pBxx = pBxx;
    if (pBxx == 0) {
        RgError(agwsSelectCannotLoadBxxError, agwsSelectSourcePath, 0x829);
    }
    pSel->pAgwsSelTop = _LoadPicList(agwsSelectAgwsTopPicListFileName, pSel->pBxx);
    pSel->pAgwsSel = _LoadPicList(agwsSelectAgwsPicListFileName, pSel->pBxx);
    pSel->pWepSelTop = _LoadPicList(agwsSelectWepTopPicListFileName, pSel->pBxx);
    pSel->pWepSel = _LoadPicList(agwsSelectWepPicListFileName, pSel->pBxx);
    RgPicListSetOffset(pSel->pAgwsSelTop, 0, 0);
    RgPicListSetOffset(pSel->pAgwsSel, 0, 0);
    RgPicListSetOffset(pSel->pWepSelTop, 0, 0);
    RgPicListSetOffset(pSel->pWepSel, 0, 0);
    RgPicListSetDrawEnable(pSel->pAgwsSelTop, 1);
    RgPicListSetDrawEnable(pSel->pAgwsSel, 1);
    RgPicListSetDrawEnable(pSel->pWepSelTop, 0);
    RgPicListSetDrawEnable(pSel->pWepSel, 1);
    pSel->ofsY = 0;
    pSel->ofsX = 0;
    pSel->pWepListDisp = _CreateWepListDisp(pSel->pBxx);
    pSel->paint = CreateXrgPaint2D_sub(agwsSelectSourcePath, 0x841);
    pSel->pRobot = CreateRgSelectRobot(pSel->pStudio);
    _SinInit();
}

/*
 * Sets pData to the default record: character 0 selected, only character 0
 * available, no weapon names.
 */
void InitRgSelectAGWSData(struct RgSelectAGWSData *pData)
{
    unsigned int charID;
    unsigned int i;

    if (pData == 0) {
        assert_prog(agwsSelectAssertDataNotNil, agwsSelectSourcePath, 2129);
    }
    pData->selectedCharacter = 0;
    pData->availableCharacters = 1;
    for (charID = 0; charID < RG_ACTOR_CHAR_NUM; charID++) {
        for (i = 0; i < RG_EQUIP_TYPE_NUM; i++) {
            pData->weaponName[charID][i][0] = 0;
        }
    }
}

void RgSelectAGWSSetSelectData(RgSelectAGWS *pSel,
                               struct RgSelectAGWSData *pData)
{
    if (pSel == 0) {
        assert_prog(agwsSelectAssertSelNotNil, agwsSelectSourcePath, 2143);
    }
    if (pData == 0) {
        assert_prog(agwsSelectAssertDataNotNil, agwsSelectSourcePath, 2144);
    }
    _SetBaseSelectData(pSel->pSelDat, pData);
}

void RgSelectAGWSGetSelectData(RgSelectAGWS *pSel,
                               struct RgSelectAGWSData *pData)
{
    if (pSel == 0) {
        assert_prog(agwsSelectAssertSelNotNil, agwsSelectSourcePath, 2152);
    }
    if (pData == 0) {
        assert_prog(agwsSelectAssertDataNotNil, agwsSelectSourcePath, 2153);
    }
    _SaveToBaseData(pSel->pSelDat);
    _GetBaseSelectData(pSel->pSelDat, pData);
}

/* Stores the weapon of each equip type in apCollects and returns the selected character id. */
int RgSelectAGWSGetCollectData(RgSelectAGWS *pSel,
                               RgWeaponEssence **apCollects)
{
    SelDat *pSelDat;

    if (pSel == 0) {
        assert_prog(agwsSelectAssertSelNotNil, agwsSelectSourcePath, 2169);
    }
    if (apCollects == 0) {
        assert_prog(agwsSelectAssertCollectsNotNil, agwsSelectSourcePath, 2170);
    }
    pSelDat = pSel->pSelDat;
    _GetWepSelection(pSelDat, apCollects);
    return _GetSelectCharID(pSelDat);
}

void RgSelectAGWSSetMode(RgSelectAGWS *pSel, int mode)
{
    _SetPlayerModeSelectData(pSel->pSelDat, mode);
}

/*
 * Returns RG_SELECT_AGWS_END_DONE or RG_SELECT_AGWS_END_CANCELLED once the
 * screen has finished, else RG_SELECT_AGWS_END_NONE.
 */
int RgSelectAGWSIsEnd(RgSelectAGWS *pSel)
{
    int result;

    if (pSel == 0) {
        assert_prog(agwsSelectAssertSelNotNil, agwsSelectSourcePath, 2194);
    }
    switch (pSel->state) {
    case AGWS_STATE_DONE:
        result = RG_SELECT_AGWS_END_DONE;
        break;
    case AGWS_STATE_CANCELLED:
        result = RG_SELECT_AGWS_END_CANCELLED;
        break;
    default:
        result = RG_SELECT_AGWS_END_NONE;
        break;
    }
    return result;
}

int RgSelectAGWSGetCharacter(RgSelectAGWS *pSel)
{
    if (pSel == 0) {
        assert_prog(agwsSelectAssertSelNotNil, agwsSelectSourcePath, 2213);
    }
    return _GetSelectCharID(pSel->pSelDat);
}

void RgSelectAGWSGetWeapon(RgSelectAGWS *pSel,
                           RgWeaponEssence **apCollects)
{
    if (pSel == 0) {
        assert_prog(agwsSelectAssertSelNotNil, agwsSelectSourcePath, 2221);
    }
    _GetWepSelection(pSel->pSelDat, apCollects);
}

RgSelectAGWS *CreateRgSelectAGWS(void)
{
    RgSelectAGWS *pSel;

    pSel = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgSelectAGWS), agwsSelectSourcePath,
                       2231);
    _InitSelect(pSel);
    return pSel;
}

void DisposeRgSelectAGWS(RgSelectAGWS *pSel)
{
    if (pSel == 0) {
        assert_prog(agwsSelectAssertSelNotNil, agwsSelectSourcePath, 2239);
    }
    _Destruct(pSel);
    RgHeapFree(InstanceOfRgHeap(), pSel, agwsSelectSourcePath, 2241);
}

void RgSelectAGWSPassTime(RgSelectAGWS *pSel, float deltaTime)
{
    if (pSel == 0) {
        assert_prog(agwsSelectAssertSelNotNil, agwsSelectSourcePath, 2249);
    }
    _PassTime(pSel, deltaTime);
}

void RgSelectAGWSDisp(RgSelectAGWS *pSel)
{
    if (pSel == 0) {
        assert_prog(agwsSelectAssertSelNotNil, agwsSelectSourcePath, 2256);
    }
    _Disp(pSel);
}



/*
 * Weapon list text colors, indexed by the selected flag: red for a conflicting
 * weapon, white otherwise.
 */
static RgFontColor s_aRed_1[2] = {
    { 0x50, 0x10, 0x10, 0x7F },
    { 0x80, 0x20, 0x20, 0x7F }
};

static RgFontColor s_aWhite_2[2] = {
    { 0x50, 0x50, 0x50, 0x7F },
    { 0x80, 0x80, 0x80, 0x7F }
};

/* Picture indices of the six pictures of each character cell on the character-select list. */
static RgSelCharPicEntry s_ainPics_3[6] = { { .m_idx = { 38, 45, 51, 60, 66, 72 } }, { .m_idx = { 43, 50, 56, 65, 71, 77 } }, { .m_idx = { 39, 47, 53, 62, 68, 74 } }, { .m_idx = { 41, 49, 55, 64, 70, 76 } }, { .m_idx = { 40, 46, 52, 61, 67, 73 } }, { .m_idx = { 42, 48, 54, 63, 69, 75 } } };

const char agwsSelectAssertCharIdNotNop[32] = "eCharID != RG_ACTOR_CHAR_NOP";

const char agwsSelectAssertNameNotNil[16] = "pszName != NIL";

const char agwsSelectAssertNameArrayNotNil[16] = "aszName != NIL";

const char agwsSelectWepListTextureName[16] = "fnt02on.bmp";

const char agwsSelectAssertTextureNotNil[16] = "pTex != NIL";

const char agwsSelectWepListFrameTextureName[16] = "frm08on.bmp";

const char agwsSelectUpArrowTextureName[24] = "icon_uparrow.bmp";

const char agwsSelectDownArrowTextureName[24] = "icon_downarrow.bmp";

const char agwsSelectCannotEquipForBattleText[24] = "Cannot equip for battle";

const char agwsSelectCannotEquipTogetherText[24] = "Cannot equip together";

const char agwsSelectDumpTitleFormat[48] = "---------------- %s %d ----------------------\n";

const char agwsSelectDumpCharacterFormat[32] = "char %d (selectable %x)\n";

const char agwsSelectDumpWeaponNameFormat[40] = "   **** char %d type %d : '%s' ***\n";

const char agwsSelectAssertResultNotNil[16] = "pResult != NIL";

const char agwsSelectPlayer1FrameTextureName[16] = "frm12_1p.bmp";

const char agwsSelectPlayer2FrameTextureName[16] = "frm12_2p.bmp";

const char agwsSelectAssertListNotNil[16] = "pList != NIL";

const char agwsSelectDetermLabelFullWidthL[8] = "\243\314";

const char agwsSelectDetermLabelFullWidthR[8] = "\243\322";

const char agwsSelectDetermLabelFullWidthLR[8] = "\243\314\243\322";

const char agwsSelectDetermConfirmText[8] = "Confirm";

const char agwsSelectDetermCancelText[8] = "Cancel";

const SelWepPicIdx agwsSelectWepPicIndexTable = { .m_idx = { 0, 1, 2 } };

const SelWepColor agwsSelectConflictColor = { .halves = { 137438953600LL, 545460846624LL } };

const SelWepColor agwsSelectNormalColor = { .halves = { 549755814016LL, 545460846720LL } };

const char agwsSelectAssertPaintNotNil[16] = "pPaint != NIL";

const char agwsSelectUnknownSelectModeFormat[24] = "unknown select mode %d";
