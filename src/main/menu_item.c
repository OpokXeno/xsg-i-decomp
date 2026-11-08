#include "common.h"
#include "main/open_close_main.h"
#include "shared.h"
#include "main/menu_1.h"
#include "main/e_battle_win_open.h"
#include "main/menu_top_command_check.h"
#include "main/e_number_main.h"
#include "main/end_print.h"
#include "main/party.h"
#include "main/xgl_2.h"
#include "main/xgl_font.h"
#include "main/xgl_sound.h"

#define MENU_ITEM_SPECIAL_ID 36
#define MENU_ITEM_USE_ALLOWED 0x8000
#define MENU_ITEM_USE_CATEGORY_BITS 0x1e43
#define MENU_ITEM_USE_SPECIAL_BIT 0x2000
#define MENU_ITEM_USE_STATE_BIT 0x10
#define MENU_ITEM_SPECIAL_ENABLED_BITS 0x20400000

/* Sound effect ids (xglSoundEffectNormalID); see also SE_* in shared.h. */
#define SE_SORT_SET 15

/* MenuWork.mode: the item menu's state machine. */
enum MenuItemMode {
    MENU_ITEM_MODE_INIT = 0,
    MENU_ITEM_MODE_LIST = 0x20,        /* browsing an item list */
    MENU_ITEM_MODE_TARGET = 0x22,      /* choosing who uses the item */
    MENU_ITEM_MODE_SEGMENT = 0x30,     /* the segment (robo parts) map */
    MENU_ITEM_MODE_PICK = 0x40,        /* returns the selected item to the caller */
    MENU_ITEM_MODE_SORT = 0x80,        /* choosing the sort method */
    MENU_ITEM_MODE_CLOSE = 0xF0,       /* closing animation */
    MENU_ITEM_MODE_DONE = 0xFF         /* back to the top menu */
};

/* MenuWork.flags bits. */
#define MENU_ITEM_FLAG_MODE_ENTERED 0x1
#define MENU_ITEM_FLAG_MODIFIED 0x4

/*
 * MenuItemUseCheck results (also MenuWork.selectionMode). SINGLE items are
 * used on one chosen party member, ALL ones on the whole party; AGWS ones
 * are used on an AGWS. UNUSABLE and BLOCKED_SPECIAL grey the list row.
 */
#define MENU_ITEM_USE_BLOCKED_SPECIAL (-1)
#define MENU_ITEM_USE_UNUSABLE 0
#define MENU_ITEM_USE_SINGLE 1
#define MENU_ITEM_USE_ALL 2
#define MENU_ITEM_USE_AGWS 10

/* MenuListEntry.status of an item row: the help text shown for it. */
#define MENU_ITEM_ROW_UNAVAILABLE 1
#define MENU_ITEM_ROW_RESTRICTED 2

/* Item attribute bits that decide what a consumable restores. */
#define MENU_ITEM_ATTR_HEAL_HP 0x1
#define MENU_ITEM_ATTR_HEAL_EP 0x2
#define MENU_ITEM_ATTR_SKILL_POINTS 0x200
#define MENU_ITEM_ATTR_ETHER_POINTS 0x400
#define MENU_ITEM_ATTR_POINTS 0x800
#define MENU_ITEM_POINTS_MAX 9999

/* Event items 11-15 are selectable on the event item tab; 11 opens the map. */
#define MENU_EVENT_ITEM_SEGMENT_MAP 11
#define MENU_EVENT_ITEM_SELECTABLE_COUNT 5
#define MENU_ITEM_GAME_FLAG_PICK 0x400000

/* Tabs: items, event items, then three equipment kinds for characters and for AGWS. */
#define MENU_ITEM_TAB_ITEMS 0
#define MENU_ITEM_TAB_EVENT 1
#define MENU_ITEM_TAB_EQUIPMENT_FIRST 2
#define MENU_ITEM_TAB_COUNT 8
/* The title bar has one message per tab plus one for the segment map. */
#define MENU_ITEM_TAB_SEGMENT 8
#define MENU_ITEM_PAS_MESSAGE_COUNT 9
#define MENU_ITEM_KEEP_SELECT_SIZE 5
/* Scenarios 0x90-0x98 (9 numbers) fix the AGWS list to three AGWS. */
#define MENU_SCENARIO_AGWS_FIXED_FIRST 0x90U
#define MENU_SCENARIO_AGWS_FIXED_COUNT 9U
/* MenuWork.busy: frames during which input is ignored after a change. */
#define MENU_ITEM_BUSY_NORMAL 12
#define MENU_ITEM_BUSY_SHORT 8
#define MENU_ITEM_BUSY_CLOSE 16
#define MENU_ITEM_SORT_CHOICE_COUNT 4
#define MENU_ITEM_SORT_CANCEL 3

/* Widget state: every *Main routine builds its windows in INIT, then runs. */
#define MENU_ITEM_STATE_INIT 0
#define MENU_ITEM_STATE_RUN 2

/* Item list window geometry. */
#define MENU_ITEM_LIST_ROWS 11
#define MENU_ITEM_LIST_ROW_HEIGHT 24
#define MENU_ITEM_LIST_PADDING 6
#define MENU_ITEM_LIST_HEIGHT 270
#define MENU_ITEM_LIST_WIDTH_ITEMS 224
#define MENU_ITEM_LIST_WIDTH_EQUIPMENT 468
#define MENU_ITEM_LIST_Y 64
#define MENU_ITEM_LIST_X_HIDDEN 528
#define MENU_ITEM_LIST_X_BESIDE_STATUS 272
#define MENU_ITEM_LIST_X_FULL 24

/* MenuItemListMain motion states. */
#define MENU_ITEM_LIST_MOTION_START 0
#define MENU_ITEM_LIST_MOTION_REBUILD 1
#define MENU_ITEM_LIST_MOTION_IDLE 2
#define MENU_ITEM_LIST_MOTION_TARGET 3
#define MENU_ITEM_LIST_MOTION_SORT 4
#define MENU_ITEM_LIST_MOTION_HIDDEN 99

/* MenuItemSegmentMain states. */
#define MENU_SEGMENT_STATE_SLIDE_IN 3
#define MENU_SEGMENT_STATE_BROWSE 4
#define MENU_SEGMENT_STATE_SLIDE_OUT 5

/* Info window (help text) heights and y positions per mode. */
#define MENU_ITEM_INFO_HEIGHT 54
#define MENU_ITEM_INFO_HEIGHT_SEGMENT 30
#define MENU_ITEM_INFO_Y_HIDDEN 480
#define MENU_ITEM_INFO_Y_LIST 384
#define MENU_ITEM_INFO_Y_SEGMENT 408
#define MENU_ITEM_INFO_Y_IDLE 464

/* Tab icons are laid out 22 pixels apart; the separator line follows them. */
#define MENU_ITEM_TAB_ICON_SPACING 22
#define MENU_ITEM_TAB_ICON_X 296
/* Header icons: two scroll arrows then one icon per tab. */
#define MENU_ITEM_ICON_COUNT 10
#define MENU_ITEM_ICON_ARROWS 2
#define MENU_ITEM_ICON_SLIDE_START 16
#define MENU_ITEM_ICON_SLIDE_MAX 15
#define MENU_ITEM_ICON_SLIDE_MAX_F 15.0f
#define MENU_ITEM_LINE_ALPHA_MAX 120
#define MENU_ITEM_LINE_ALPHA_STEP 8

/* The sort hint message ("Sort" header) slides in from the right edge. */
#define MENU_ITEM_EX_X_HIDDEN 544
#define MENU_ITEM_EX_X_SHOWN 400

/* Layer base of each widget's draw ordering (+1, +2 for its sub layers). */
#define MENU_ITEM_DRAW_KEY 0xFFFFF0
#define MENU_ITEM_DRAW_KEY_STATUS 0xF000F0
#define MENU_ITEM_DRAW_KEY_EX 0xFFFFF8
#define MENU_ITEM_DRAW_KEY_SEGMENT 0x0FFFF8

/* Common slide-in animation. */
#define MENU_ITEM_SLIDE_RATE 3.0f
#define MENU_ITEM_SLIDE_X_HIDDEN (-272)
#define MENU_ITEM_SLIDE_X_SHOWN 16

/* Party and AGWS status windows. */
#define MENU_ITEM_STATUS_WINDOW_WIDTH 240
#define MENU_ITEM_STATUS_WINDOW_HEIGHT 82
#define MENU_ITEM_STATUS_ROW_PITCH 104
#define MENU_ITEM_STATUS_TOP_Y 64
#define MENU_ITEM_STATUS_SCROLL_PARTY 4
#define MENU_ITEM_STATUS_VISIBLE_ROWS 3
#define MENU_ITEM_AGWS_ROW_HEIGHT 50

/* Segment map: 18 segments in three columns. */
#define MENU_SEGMENT_COUNT 18
#define MENU_SEGMENT_COLUMNS 3

/*
 * Event flag banks of the segments (one flag per segment, 18 each): the
 * location was found, and the rare item was obtained. MenuItemSegmentRow
 * .flags holds one bit per bank: LOCATION, RARE_ITEM and a bank at 3221.
 */
#define MENU_FLAG_SEGMENT_LOCATION_BASE 3201
#define MENU_FLAG_SEGMENT_RARE_ITEM_BASE 3241
#define MENU_SEGMENT_ROW_LOCATION 0x1
#define MENU_SEGMENT_ROW_RARE_ITEM 0x4
#define MENU_SEGMENT_ROW_COMPLETE 0x7

/* Segment tile size and the maximum alpha of the pulse animation. */
#define MENU_SEGMENT_TILE_WIDTH 72
#define MENU_SEGMENT_TILE_HEIGHT 58
#define MENU_SEGMENT_PULSE_MAX 120

/* Sprite ids of the segment tiles. */
#define MENU_SEGMENT_SPRITE_TILE 4864
#define MENU_SEGMENT_SPRITE_TILE_COMPLETE 4865
#define MENU_SEGMENT_SPRITE_HIGHLIGHT 4867
#define MENU_SEGMENT_SPRITE_COMPLETE_GLOW 4868
#define MENU_SEGMENT_SPRITE_MARK_LOCATION 4870


typedef struct MenuItemInfoText {
    unsigned short x;
    unsigned short y;
    unsigned char unmodeled_04[8];
    const char *message;
    unsigned char unmodeled_10[0x1B0 - 0x10];
} MenuItemInfoText;

typedef struct MenuItemInfoWork {
    unsigned char state;
    unsigned char unmodeled_01[2];
    unsigned char help;
    unsigned char unmodeled_04[4];
    int color;
    WindowDX window;
    MenuItemInfoText text;
    char buffer[128];
} MenuItemInfoWork;

typedef struct MenuItemInfoLabels {
    const char *sortMethods[3];
    const char *sortPrefix;
    const char *cancel;
} MenuItemInfoLabels;

typedef struct MenuItemInfoHelpLabels {
    const char *unavailable;
    const char *restricted;
} MenuItemInfoHelpLabels;

typedef struct MenuItemInfoPad {
    unsigned char unmodeled_00[0x2A];
    unsigned short pressed;
    unsigned char unmodeled_2C[8];
    unsigned short activity;
} MenuItemInfoPad;

typedef struct {
    u8 unmodeled_00[4];
    u16 use_flags; /* The high bit gates all item availability checks. */
    u8 unmodeled_06[2];
    u8 use_state;
    u8 unmodeled_09[3];
    u16 attributes;
} MenuItemUseData;

typedef struct {
    u8 unmodeled_00[0x10];
    u64 flags;
} MenuItemLoopState;

typedef struct MenuItemListWork {
    u8 state;
    u8 visible;
    u8 unmodeled_02;
    u8 motion;
    int activeTab;
    int color;
    WindowSPCursor window;
} MenuItemListWork;

/* MenuItem carves the ex work as 0x14 bytes (a full 0x44-byte EMessageParam
 * here turns its 1676-byte carve step into 1732), so only the first 12
 * bytes of the message record exist in this work. */
typedef struct MenuItemMessage {
    u8 active;
    u8 mode;
    u8 unmodeled_02[2];
    short x;
    short y;
    int color;
} MenuItemMessage;

typedef struct MenuItemExWork {
    u8 state;
    u8 unmodeled_01[3];
    int color;
    MenuItemMessage message;
} MenuItemExWork;

typedef struct MenuItemPasBox {
    short x;
    short y;
    int color;
    short width;
    short height;
} MenuItemPasBox;

typedef struct MenuItemPasWork {
    u8 state;
    u8 unmodeled_01[3];
    short x;
    short y;
    int color;
    WindowDX window;
    EMessageParam message[MENU_ITEM_PAS_MESSAGE_COUNT];
    MenuItemPasBox box;
    u8 callbackWork[1];
} MenuItemPasWork;

typedef struct MenuItemWorkData {
    u8 unmodeled_00[3];
    u8 mode;
    u8 unmodeled_04[0x20 - 0x04];
    u8 flags;
    u8 nextMode;
    u8 busy;
    u8 partyCount;
    signed char selectionMode;
    u8 unmodeled_25[0x2E - 0x25];
    short selectedItem;
    signed char listTab;
    signed char sortChoices[8]; /* +0x31 */
    u8 unmodeled_39[0x40 - 0x39];
    signed char tabChangeDirection;
    signed char scrollDir;
    u8 unmodeled_42[0x50 - 0x42];
    int selectDir;
    u8 unmodeled_54[0x62 - 0x54];
    signed char agwsMode;
    signed char selectedAgws;
    signed char agwsCount;
    u8 unmodeled_65[0x70 - 0x65];
    signed char selectedParty;
    u8 unmodeled_71; /* +0x71 */
    signed char sortSelection; /* +0x72 */
    u8 unmodeled_73;
    u8 segmentSelection; /* +0x74 */
} MenuItemWorkData;

typedef struct MenuItemSeparator {
    short x;
    short y;
    int color;
    char red;
    char green;
    char blue;
    char alpha;
} MenuItemSeparator;

typedef struct MenuItemLineWork {
    unsigned char state;
    unsigned char unmodeled_01[2];
    unsigned char alpha;
    int color;
    MenuItemSeparator separators[6];
} MenuItemLineWork;

typedef struct MenuItemLineStartX {
    int x[5];
} MenuItemLineStartX;

typedef struct MenuItemStatusContext {
    signed char characterId;
    u8 mode;
    short alternateId;
    short x;
    short y;
    short initialControls[2];
    u8 unmodeled_0c[12];
    int firstTextStyle;
    u8 unmodeled_1c[0x840 - 0x1C];
} MenuItemStatusContext;


typedef struct MenuItemPartyStatusWork {
    u8 state;
    unsigned char visibleCount;
    signed char scrollStart;
    u8 scrollTick;
    u8 selectionRow;
    u8 unmodeled_05[3];
    int style;
    WindowDX windows[8];
    MenuItemStatusContext contexts[8];
    ECursolMainData cursors[3];
    /* Two backgrounds, two outlines, then the clipping rectangle. */
    MenuItemPasBox boxes[5];
} MenuItemPartyStatusWork;

typedef struct MenuItemSpriteIds {
    short ids[MENU_ITEM_ICON_COUNT];
} MenuItemSpriteIds;

typedef struct MenuItemIconSprite {
    u8 state;
    u8 unmodeled_01[3];
    short x;
    short y;
    int style;
    signed char red;
    signed char green;
    signed char blue;
    unsigned char alpha;
    u8 unmodeled_10[0x28 - 0x10];
} MenuItemIconSprite;

typedef struct MenuItemIconWork {
    u8 state;
    u8 unmodeled_01;
    u8 slide;
    u8 pulsePhase;
    u8 pulse;
    u8 pulseDirection;
    signed char displacement[MENU_ITEM_ICON_COUNT];
    int style;
    MenuItemIconSprite sprites[MENU_ITEM_ICON_COUNT];
} MenuItemIconWork;

typedef struct MenuItemSelectContent {
    u8 unmodeled_00[2];
    unsigned short mode;
    int selection;
    const char *text;
    u8 unmodeled_0c[0xBF0 - 0x0C];
} MenuItemSelectContent;


typedef struct MenuItemSelectSprite {
    u8 state;
    u8 unmodeled_01[3];
    short x;
    short y;
    int color;
} MenuItemSelectSprite;

typedef struct MenuItemSelectWork {
    u8 state;
    u8 unmodeled_01;
    u8 unmodeled_02;
    u8 unmodeled_03;
    int color;
    WindowDX window[1];
    u8 unmodeled_19c[0x330 - 0x19C];
    MenuItemSelectContent content;
    MenuItemSelectSprite sprite;
} MenuItemSelectWork;

/* The rows are 0x15c bytes apart with the flags at +4 (MenuItemSegmentMain
 * walks them from row + 0), so the number that ends each row has only 0x8c
 * bytes: the last four bytes of a full ENumberMainData (the second separator
 * sprite ID, unused by format 0) would overlap the next row. */
typedef struct MenuItemSegmentNumber {
    unsigned short x;
    unsigned short y;
    int work;
    u8 unmodeled_08[6];
    u8 format;
    u8 digitCount;
    u8 unmodeled_10[4];
    int value;
    u8 unmodeled_18[140 - 24];
} MenuItemSegmentNumber;

typedef struct MenuItemSegmentRow {
    u8 pulse;
    signed char pulseDirection;
    u8 unmodeled_02[2];
    u8 flags;
    u8 unmodeled_05;
    u8 emphasis;
    signed char emphasisDirection;
    MenuItemIconSprite sprites[5];
    MenuItemSegmentNumber number;
} MenuItemSegmentRow;

typedef struct MenuItemSegmentPoint {
    short x;
    short y;
    int work;
    signed char red;
    signed char green;
    signed char blue;
    signed char alpha;
} MenuItemSegmentPoint;

typedef struct MenuItemSegmentRibbon {
    short x;
    short y;
    int work;
    short width;
    short height;
    u8 mode;
    u8 progress;
    u8 visible;
    u8 style;
    MenuItemSegmentPoint points[6];
    u8 unmodeled_58[24];
    MenuItemSegmentPoint vertices[24];
} MenuItemSegmentRibbon;

typedef struct MenuItemSegmentOutline {
    u8 pulse;
    signed char pulseDirection;
    u8 unmodeled_02[2];
    MenuItemSegmentPoint points[12];
} MenuItemSegmentOutline;

typedef struct MenuItemSegmentWork {
    u8 state;
    u8 visible;
    short x;
    short y;
    u8 unmodeled_06[2];
    int work;
    int selected;
    WindowDX windows[2];
    MenuItemIconSprite labels[2];
    EMessageParam text[2];
    MenuItemSegmentRow rows[MENU_SEGMENT_COUNT];
    u8 unmodeled_1c88[4];
    unsigned short ownedParts;
    unsigned short selectedParts;
    MenuItemIconSprite mapSprites[19];
    MenuItemSegmentRibbon ribbon;
    MenuItemSegmentOutline outlines[2];
    int texture;
} MenuItemSegmentWork;

typedef struct MenuItemAgwsRow {
    WindowDX window;
    EMessageParam name;
    ETagFontMainData hpLabel;
    ENumberMainData hpNumber;
} MenuItemAgwsRow;

typedef struct MenuItemAgwsStatusWork {
    u8 state;
    u8 unmodeled_01[3];
    int color;
    MenuItemAgwsRow rows[6];
    ECursolMainData cursor;
} MenuItemAgwsStatusWork;

typedef struct MenuItemArena {
    MenuItemPasWork pas;
    u8 unmodeled_0414[0x480 - 0x414];
    MenuItemInfoWork info;
    MenuItemPartyStatusWork status;
    u8 unmodeled_57a4[0x57B0 - 0x57A4];
    MenuItemAgwsStatusWork agws;
    MenuItemListWork list;
    MenuItemIconWork icon;
    u8 unmodeled_7ff4[0x8044 - 0x7FF4];
    MenuItemSelectWork select;
    u8 unmodeled_8f70[0x8FB4 - 0x8F70];
    MenuItemExWork ex;
    u8 unmodeled_8fc8[0x9640 - 0x8FC8];
    MenuItemLineWork line;
    MenuItemSegmentWork segment;
} MenuItemArena;

typedef struct MenuItemCharacterPoints {
    u8 unmodeled_00[0x0C];
    int points;
    int etherPoints;
    int skillPoints;
} MenuItemCharacterPoints;

typedef struct MenuItemHpParameters {
    short maxHp;
    u8 unmodeled_02[0x34 - 0x02];
    short hp;
} MenuItemHpParameters;

typedef struct MenuItemSegmentTriangleOffsets {
    short x[3];
    short y[3];
} MenuItemSegmentTriangleOffsets;

typedef struct MenuItemSegmentRibbonHead {
    short x;
    short y;
    int work;
    short width;
    short height;
    u8 mode;
    u8 progress;
    u8 visible;
    u8 style;
    MenuItemSegmentPoint points[6];
} MenuItemSegmentRibbonHead;

typedef struct MenuItemSegmentAlignedWork {
    u8 state;
    u8 visible;
    short x;
    short y;
    u8 unmodeled_06[2];
    int work;
    int selected;
    WindowDX windows[2];
    MenuItemIconSprite labels[2];
    EMessageParam text[2];
    MenuItemSegmentRow rows[MENU_SEGMENT_COUNT];
    u8 unmodeled_1c88[4];
    unsigned short ownedParts;
    unsigned short selectedParts;
    MenuItemIconSprite mapSprites[19];
    MenuItemSegmentRibbonHead ribbon;
    MenuItemSegmentPoint unmodeled_1fe0[2];
    MenuItemSegmentPoint vertices[24];
    MenuItemSegmentOutline outlines[2];
    int texture;
} __attribute__((aligned(32))) MenuItemSegmentAlignedWork;

extern const MenuItemInfoLabels infoLabels;
extern MenuItemInfoHelpLabels infoHelpTable[];
extern const unsigned char emptyText[];
extern const char menuItemPeriodText[];
extern MenuItemInfoPad PadData;
extern MenuItemLoopState GameLoopState;
extern MenuItemPasWork *MenuItemPas;
extern void *MenuItemInfo;
extern void *MenuItemLine;
extern void *MenuItemStatus;
extern void *MenuItemStatusAgws;
extern MenuItemListWork *MenuItemList;
extern void *MenuItemSelect;
extern void *MenuItemIcon;
extern MenuItemExWork *MenuItemEx;
extern void *MenuItemSegment;
extern MenuItemWorkData MenuWork;
extern unsigned char MenuKeepSelect[];
extern const MenuItemSpriteIds iconSpriteTable;
extern const float menuItemPi;
extern const float iconAngleStep;
static char * msg00_4_0036D6B8[] ;
extern void *MainMenuWorkEnd;
extern int MenuScenarioNo;
extern const char debugItemLabel[];
extern const char listIndexFormat[];
static const char * msg10_3[] ;
static const char * msg00_2_0036D6A8[] ;
extern const char fhpText[];
static const char * tag_msg_5[] ;
extern const char temporaryText[];
extern const char sortModeChoicesText[40];
extern const char sortText[8];
extern const char rareItemTagText[16];
extern const char locationTagText[16];

extern const char sortHeaderText[24];

extern const char MenuItemInfoLabelsText[];
extern const char MenuItemCancelText[];

extern MenuItemUseData *func_A1A4E8(s16 itemId);
extern int *MenuSortAddrGet(int list);
extern MenuListEntry *MenuListGet(int list);
extern void MenuListMake(int list, int mode);
extern int MenuSortCheck(int list);
extern void MenuSortChange(int list, int index);
extern void MoveSlide(short *current, short *target, float rate);
extern int xglFlagsGet(int flag, int mode);
extern char *MenuTagTextGet(int index);
extern int PartyAttackerCheck(int characterId);
extern void WindowSPSet(WindowSPCursor *window);
extern void WindowSPMain(WindowSPCursor *window);
extern void eSpriteSet(void *sprite, short spriteId);
extern void eSpriteMain(void *sprite);
extern int MenuSortGet(int list, int index);
extern void MenuWorkEndCheck(void *end);
extern unsigned char *MenuSaveDataGet(void);
extern int MenuSelectMove(int selection, int count, int wrap);
extern int MenuBoxDec(int itemId);
extern int MenuCharHpCheck(int character);
extern int MenuCharEpCheck(int character);
extern MenuItemCharacterPoints *func_A19210(int character);
extern int func_00A114C0(int character, int itemId);
extern void ChangeTopLevel(int level);
extern char *strcpy(char *destination, const char *source);
extern char *strcat(char *destination, const char *source);
extern const char *MenuCharNameGet(int character);
extern MenuItemHpParameters *func_00A11108(int character, int *attack, int *defense);
extern const char *MenuSegmentMapNameGet(int segment);
extern const char *MenuSegmentItemNameGet(int segment);
extern const char *MenuSegmentInfoTextGet(int segment);
extern char *dataEvtItmNameGet(int item);
extern int WindowTexAddrGet(int texture);
/* Text: List{x 653}Num */
const char listNumText[16] = "List\x03\x8d" "Num";

/* Text: List{x 665}Num{x 665}3Num */
const char listNum3NumText[32] = "List\x03\x99Num\x03\x99\x03" "3Num";

static short MenuItemParty[8];

static short MenuItemAgws[8];

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
static char msg00_1_0036D698[10];

/* Returns the MENU_ITEM_USE_* result for item itemId. */
int MenuItemUseCheck(s16 itemId)
{
    MenuItemUseData *item;
    u16 attributes;
    int result;
    /* A special item is unavailable when neither game-loop bit is set. */
    int specialUnavailable = MENU_ITEM_USE_BLOCKED_SPECIAL;

    if (itemId == 0) {
        return MENU_ITEM_USE_UNUSABLE;
    }
    item = func_A1A4E8(itemId);
    if ((item->use_flags & MENU_ITEM_USE_ALLOWED) == 0) {
        return MENU_ITEM_USE_UNUSABLE;
    }
    if (itemId == MENU_ITEM_SPECIAL_ID) {
        result = (GameLoopState.flags & MENU_ITEM_SPECIAL_ENABLED_BITS) ? MENU_ITEM_USE_ALL : specialUnavailable;
    } else {
        attributes = item->attributes;
        if (attributes & MENU_ITEM_USE_CATEGORY_BITS) {
            result = (item->use_state & MENU_ITEM_USE_STATE_BIT) ? MENU_ITEM_USE_ALL : MENU_ITEM_USE_SINGLE;
        } else {
            result = (attributes & MENU_ITEM_USE_SPECIAL_BIT) ? MENU_ITEM_USE_AGWS : MENU_ITEM_USE_UNUSABLE;
        }
    }
    return result;
}

/*
 * Per-frame update of the tab strip window: builds it on the first run, slides it in
 * or out for the current menu mode, and slides each tab message to its target.
 */
void MenuItemPasMain(void)
{
    static const char *msg00[MENU_ITEM_PAS_MESSAGE_COUNT] = {
        (const char *)0x004DAAB8,
        (const char *)0x004C4E70,
        (const char *)0x004C4E58,
        (const char *)0x004C4E40,
        (const char *)0x004C4E30,
        (const char *)0x004C4E18,
        (const char *)0x004C4E00,
        (const char *)0x004C4DF0,
        (const char *)0x004C4DE0,
    };
    MenuItemPasWork *pas = MenuItemPas;
    short tabTarget[MENU_ITEM_PAS_MESSAGE_COUNT];
    short slideTarget;
    int i;

    switch (pas->state) {
    case MENU_ITEM_STATE_INIT:
        pas->x = MENU_ITEM_SLIDE_X_HIDDEN;
        pas->y = 8;
        pas->color = MENU_ITEM_DRAW_KEY;
        WindowDXSet(&pas->window);
        pas->window.x = pas->x;
        pas->window.y = pas->y;
        pas->window.style = pas->color;
        pas->window.width = 272;
        pas->window.height = 30;
        pas->window.close_callback = MenuPasWindow;
        pas->window.close_callback_arg = pas->callbackWork;
        pas->window.state = 1;
        WindowDXMain(&pas->window);
        pas->window.state = 3;
        for (i = 0; i < MENU_ITEM_PAS_MESSAGE_COUNT; i++) {
            eMessageSet(&pas->message[i], msg00[i]);
            pas->message[i].mode = 32;
            pas->message[i].x = 288;
            pas->message[i].draw_parameter = pas->color + 2;
            pas->message[i].y = pas->y + 3;
        }
        pas->state = MENU_ITEM_STATE_RUN;
        /* fallthrough */
    case MENU_ITEM_STATE_RUN:
        break;
    default:
        return;
    }

    slideTarget = -16;
    for (i = 0; i < MENU_ITEM_PAS_MESSAGE_COUNT; i++)
        tabTarget[i] = 288;

    /* The current list or segment tab slides in; the other tabs stay out of view. */
    switch (MenuWork.mode) {
    case MENU_ITEM_MODE_PICK:
    case MENU_ITEM_MODE_SORT:
    case MENU_ITEM_MODE_LIST:
    case MENU_ITEM_MODE_TARGET:
        tabTarget[MenuWork.listTab] = MENU_ITEM_SLIDE_X_SHOWN;
        break;
    case MENU_ITEM_MODE_SEGMENT:
        tabTarget[MENU_ITEM_TAB_SEGMENT] = MENU_ITEM_SLIDE_X_SHOWN;
        break;
    default:
        slideTarget = MENU_ITEM_SLIDE_X_HIDDEN;
        break;
    }

    MoveSlide(&pas->window.x, &slideTarget, MENU_ITEM_SLIDE_RATE);
    WindowDXMain(&pas->window);
    pas->box.x = pas->window.x + 3;
    pas->box.y = pas->window.y + 3;
    pas->box.color = pas->color;
    pas->box.width = pas->window.width - 6;
    pas->box.height = pas->window.height - 6;
    endPrintExtFunc(pas->color, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_SCISSOR_SET,&pas->box);
    for (i = 0; i < MENU_ITEM_PAS_MESSAGE_COUNT; i++) {
        MoveSlide(&pas->message[i].x, &tabTarget[i], MENU_ITEM_SLIDE_RATE);
        if (pas->message[i].x < 256)
            eMessageMain(&pas->message[i]);
    }
    endPrintExtFunc(pas->color, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_SCISSOR_RESET,0);
}

/*
 * Updates the help-text window. Circle toggles the help text on list rows; the text
 * and the window's target position follow the menu mode.
 */
void MenuItemInfoMain(void)
{
    MenuItemInfoWork *info = MenuItemInfo;
    MenuItemInfoLabels labels = infoLabels;
    MenuItemInfoHelpLabels helpLabels = infoHelpTable[0];
    short targetY;
    short targetHeight;

    switch (info->state) {
    case MENU_ITEM_STATE_INIT: {
        signed char windowState;
        unsigned short *textX;

        info->color = MENU_ITEM_DRAW_KEY;
        WindowDXSet(&info->window);
        info->window.x = -16;
        info->window.y = MENU_ITEM_INFO_Y_HIDDEN;
        info->window.width = 544;
        info->window.height = MENU_ITEM_INFO_HEIGHT;
        info->window.style = info->color;
        info->window.close_callback = MenuInfoWindow;
        windowState = 1;
        info->window.close_callback_arg = &info->text;
        textX = info->window.close_callback_arg;
        *textX = 0;
        info->window.state = windowState;
        info->text.y = 0;
        info->text.message = 0;
        WindowDXMain(&info->window);
        info->window.state = 3;
        info->help = 0;
        memset(info->buffer, 0, sizeof(info->buffer));
        info->state = MENU_ITEM_STATE_RUN;
    }
        /* fall through */
    case MENU_ITEM_STATE_RUN:
        break;
    default:
        return;
    }

    targetY = MENU_ITEM_INFO_Y_LIST;
    targetHeight = MENU_ITEM_INFO_HEIGHT;
    if (MenuWork.flags & MENU_ITEM_FLAG_MODE_ENTERED) {
        info->help = 0;
    }
    if (info->help == 0) {
        if (PadData.pressed & PAD_CIRCLE) {
            info->help = 1;
        }
    } else if (PadData.activity != 0) {
        info->help = 0;
    }

    /* Pick the text for the current mode; segment mode only moves the window. */
    switch (MenuWork.mode) {
    case MENU_ITEM_MODE_LIST:
    case MENU_ITEM_MODE_TARGET:
    case MENU_ITEM_MODE_PICK:
        if (MenuWork.busy == 0) {
            if (MenuWork.selectDir >= 0) {
                strcpy(info->buffer, MenuTextGet(MenuSortGet(0, MenuWork.selectDir))->auxiliary);
                if (MenuWork.listTab == MENU_ITEM_TAB_ITEMS && info->help == 1) {
                    signed char use = MenuListGet(0)[MenuWork.selectDir].status;

                    if (use == MENU_ITEM_ROW_UNAVAILABLE) {
                        strcpy(info->buffer, helpLabels.unavailable);
                    } else if (use == MENU_ITEM_ROW_RESTRICTED) {
                        strcpy(info->buffer, helpLabels.restricted);
                    }
                }
            } else {
                info->buffer[0] = emptyText[0];
            }
        }
        break;
    case MENU_ITEM_MODE_SORT:
        if (MenuWork.sortSelection == MENU_ITEM_SORT_CANCEL) {
            strcpy(info->buffer, labels.cancel);
        } else {
            strcpy(info->buffer, labels.sortPrefix);
            strcat(info->buffer, labels.sortMethods[MenuWork.sortSelection]);
            strcat(info->buffer, menuItemPeriodText);
        }
        break;
    case MENU_ITEM_MODE_SEGMENT:
        targetY = MENU_ITEM_INFO_Y_SEGMENT;
        targetHeight = MENU_ITEM_INFO_HEIGHT_SEGMENT;
        break;
    default:
        targetY = MENU_ITEM_INFO_Y_IDLE;
        break;
    }

    info->text.message = info->buffer;
    MoveSlide(&info->window.y, &targetY, MENU_ITEM_SLIDE_RATE);
    MoveSlide(&info->window.height, &targetHeight, MENU_ITEM_SLIDE_RATE);
    WindowDXMain(&info->window);
}

/*
 * Updates the separator lines around the tab strip: initializes them on the first run,
 * then slides them toward the current tab and fades them in or out.
 */
void MenuItemLineMain(void)
{
    MenuItemLineWork *line = MenuItemLine;
    int i;

    switch (line->state) {
    case MENU_ITEM_STATE_INIT:
        line->color = MENU_ITEM_DRAW_KEY;
    {
        MenuItemLineStartX initialX = { { 0, 0x134, 0x138, 0x13c, 0x200 } };

        for (i = 0; i < 5; i++) {
            line->separators[i].blue = 96;
            line->separators[i].green = 96;
            line->separators[i].red = 96;
            line->separators[i].alpha = -128;
            line->separators[i].y = 48;
            line->separators[i].x = initialX.x[i];
            line->separators[i].color = line->color;
        }
        line->separators[5].color = 0;
        line->state = MENU_ITEM_STATE_RUN;
        line->alpha = 0;
    }
        /* fall through */
    case MENU_ITEM_STATE_RUN: {
    struct {
        short targetY[8];
        short centerLength;
    } slide;
    MenuItemWorkData *menu;
    int tabY;
    int firstTargetY;

    i = 3;
    do {
        slide.targetY[i] = 0;
    } while (--i >= 0);
    menu = &MenuWork;
    tabY = menu->listTab * MENU_ITEM_TAB_ICON_SPACING;
    firstTargetY = tabY + 302;
    slide.targetY[0] = firstTargetY;
    slide.targetY[1] = tabY + 306;
    slide.targetY[2] = tabY + 310;
    slide.centerLength = 48;
    slide.targetY[3] = 512;
    if (firstTargetY == line->separators[1].x) {
        slide.centerLength = 40;
    }

    /* The lines fade in for the list-style modes and fade out for the others. */
    switch (menu->mode) {
    case MENU_ITEM_MODE_LIST:
    case MENU_ITEM_MODE_TARGET:
    case MENU_ITEM_MODE_SEGMENT:
    case MENU_ITEM_MODE_PICK:
    case MENU_ITEM_MODE_SORT:
        if (line->alpha != MENU_ITEM_LINE_ALPHA_MAX) {
            line->alpha += MENU_ITEM_LINE_ALPHA_STEP;
        }
        break;
    default:
        if (line->alpha != 0) {
            line->alpha -= MENU_ITEM_LINE_ALPHA_STEP;
        }
        break;
    }

    for (i = 1; i < 5; i++) {
        MoveSlide(&line->separators[i].x, &slide.targetY[i - 1], MENU_ITEM_SLIDE_RATE);
        line->separators[i].alpha = line->alpha;
    }
    line->separators[0].alpha = line->alpha;
    MoveSlide(&line->separators[2].y, &slide.centerLength, 8.0f);
    endPrintExtFunc(line->color, END_PRINT_CMD_LINE, line->separators);
        break;
    }
    }
}

/*
 * Updates the party status windows: builds them on the first run, then moves them to
 * their target positions for the current mode and places the selection cursor.
 */
void MenuItemStatusMain(void)
{
    MenuItemPartyStatusWork *status = MenuItemStatus;
    short targetX[8];
    short targetY[8];
    int i;

    switch (status->state) {
    case MENU_ITEM_STATE_INIT: {
        status->style = MENU_ITEM_DRAW_KEY_STATUS;
        i = 0;
        if (MenuWork.partyCount != 0) {
            short *party = MenuItemParty;

            do {
                WindowDXSet(&status->windows[i]);
                status->windows[i].close_callback = MenuStatusDisp;
                status->windows[i].x = MENU_ITEM_SLIDE_X_HIDDEN;
                status->windows[i].width = MENU_ITEM_STATUS_WINDOW_WIDTH;
                status->windows[i].height = MENU_ITEM_STATUS_WINDOW_HEIGHT;
                status->windows[i].y = MENU_ITEM_STATUS_TOP_Y + i * MENU_ITEM_STATUS_ROW_PITCH;
                status->windows[i].close_callback_arg = &status->contexts[i];
                status->windows[i].style = status->style;
                if (PartyAttackerCheck(*party)) {
                    status->windows[i].tag_id = MenuTagTextGet(MENU_TAG_COMBATANT);
                } else {
                    status->windows[i].tag_id = MenuTagTextGet(MENU_TAG_RESERVE);
                }
                status->windows[i].ribbon_initialized = 1;
                status->contexts[i].characterId = *party;
                status->contexts[i].x = 0;
                status->contexts[i].y = 0;
                status->contexts[i].mode = 0;
                status->contexts[i].initialControls[0] = 0;
                status->contexts[i].initialControls[1] = 31;
                status->windows[i].state = 1;
                WindowDXMain(&status->windows[i]);
                status->windows[i].state = 3;
                party++;
            } while (++i < MenuWork.partyCount);
        }
        for (i = 0; i < 3; i++) {
            eCursolSet(&status->cursors[i], i);
            status->cursors[i].state = 32;
            status->cursors[i].work = status->style + 16;
        }
        status->boxes[0].x = 0;
        status->boxes[0].width = 256;
        status->boxes[0].y = 0;
        status->boxes[0].height = 48;
        status->boxes[1].x = 0;
        status->boxes[1].width = 256;
        status->boxes[1].y = 354;
        status->boxes[1].height = 158;
        for (i = 0; i < 2; i++) {
            status->boxes[i].color = 0xFFFFFF;
            status->boxes[i + 2].color = 0;
        }
        status->scrollTick = 0;
        status->scrollStart = 0;
        status->selectionRow = 0;
        status->visibleCount = MenuWork.partyCount;
        if (status->visibleCount >= MENU_ITEM_STATUS_SCROLL_PARTY) {
            status->visibleCount = MENU_ITEM_STATUS_VISIBLE_ROWS;
        }
        status->state = MENU_ITEM_STATE_RUN;
    }
        /* fall through */
    case MENU_ITEM_STATE_RUN:
        break;
    default:
        return;
    }

    for (i = 0; i < MenuWork.partyCount; i++) {
        targetX[i] = MENU_ITEM_SLIDE_X_HIDDEN;
        targetY[i] = MENU_ITEM_STATUS_TOP_Y + (i - status->scrollStart) * MENU_ITEM_STATUS_ROW_PITCH;
    }
    /* Rows slide in for the item list and the target choice; a single target also scrolls them. */
    switch (MenuWork.mode) {
    case MENU_ITEM_MODE_LIST:
        if (MenuWork.flags & MENU_ITEM_FLAG_MODE_ENTERED) {
            status->cursors[0].state = 0;
        }
        if (MenuWork.listTab == MENU_ITEM_TAB_ITEMS && MenuWork.agwsMode == 0) {
            for (i = 0; i < MenuWork.partyCount; i++) {
                targetX[i] = MENU_ITEM_SLIDE_X_SHOWN;
            }
        }
        break;
    case MENU_ITEM_MODE_TARGET:
        if (MenuWork.flags & MENU_ITEM_FLAG_MODE_ENTERED) {
            status->cursors[0].state = 32;
        }
        if (MenuWork.agwsMode == 0) {
            for (i = 0; i < MenuWork.partyCount; i++) {
                targetX[i] = MENU_ITEM_SLIDE_X_SHOWN;
            }
            if (MenuWork.selectionMode == MENU_ITEM_USE_SINGLE) {
                if (MenuWork.partyCount >= MENU_ITEM_STATUS_SCROLL_PARTY) {
                    if (MenuWork.scrollDir == -1) {
                        if (status->scrollTick == 0) {
                            if (status->scrollStart != 0) {
                                status->scrollStart--;
                            }
                        } else {
                            status->scrollTick--;
                        }
                        MenuWork.scrollDir = 2;
                    } else if (MenuWork.scrollDir == 1) {
                        if (status->scrollTick == 2) {
                            if (status->scrollStart != MenuWork.partyCount - MENU_ITEM_STATUS_VISIBLE_ROWS) {
                                status->scrollStart++;
                            }
                        } else {
                            status->scrollTick++;
                        }
                        MenuWork.scrollDir = 2;
                    }
                    for (i = 0; i < MenuWork.partyCount; i++) {
                        targetY[i] = MENU_ITEM_STATUS_TOP_Y + (i - status->scrollStart) * MENU_ITEM_STATUS_ROW_PITCH;
                    }
                }
            } else {
                int rowCount = MenuWork.partyCount;

                if (rowCount >= MENU_ITEM_STATUS_SCROLL_PARTY) {
                    rowCount = MENU_ITEM_STATUS_VISIBLE_ROWS;
                }
                if (status->selectionRow == rowCount - 1) {
                    status->selectionRow = 0;
                } else {
                    status->selectionRow++;
                }
            }
        }
        break;
    }

    for (i = 0; i < 2; i++) {
        endPrintExtFunc(status->style, END_PRINT_CMD_Z_SCISSOR,&status->boxes[i]);
    }
    status->boxes[4].x = 0;
    status->boxes[4].width = 256;
    status->boxes[4].y = 64;
    status->boxes[4].height = 272;
    endPrintExtFunc(status->style, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_SCISSOR_SET,&status->boxes[4]);
    for (i = 0; i < MenuWork.partyCount; i++) {
        MoveSlide(&status->windows[i].x, &targetX[i], MENU_ITEM_SLIDE_RATE);
        MoveSlide(&status->windows[i].y, &targetY[i], MENU_ITEM_SLIDE_RATE);
        if (status->windows[i].y == targetY[i]) {
            MenuWork.scrollDir = 0;
        }
        WindowDXMain(&status->windows[i]);
    }
    /* One cursor on the selected member; scroll arrows show while the list scrolls. */
    if (MenuWork.selectionMode == MENU_ITEM_USE_SINGLE) {
        status->cursors[0].x = status->windows[MenuWork.selectedParty].x + 3;
        status->cursors[0].y = status->windows[MenuWork.selectedParty].y + status->windows[MenuWork.selectedParty].height / 2 - 4;
        eCursolMain(&status->cursors[0]);
        if (MenuWork.partyCount >= MENU_ITEM_STATUS_SCROLL_PARTY && status->cursors[0].state != 0) {
            if (status->scrollStart != 0) {
                status->cursors[1].x = 240;
                status->cursors[1].y = 50;
                eCursolMain(&status->cursors[1]);
            }
            if (status->scrollStart != MenuWork.partyCount - MENU_ITEM_STATUS_VISIBLE_ROWS) {
                status->cursors[2].x = 240;
                status->cursors[2].y = 338;
                eCursolMain(&status->cursors[2]);
            }
        }
    } else if (MenuWork.selectionMode == MENU_ITEM_USE_ALL) {
        status->cursors[0].x = status->windows[MenuWork.selectedParty].x + 3;
        status->cursors[0].y = status->selectionRow * MENU_ITEM_STATUS_ROW_PITCH + status->windows[MenuWork.selectedParty].height / 2 + 60;
        eCursolMain(&status->cursors[0]);
    }
    for (i = 0; i < 2; i++) {
        status->boxes[i + 2].x = status->boxes[i].x;
        status->boxes[i + 2].width = status->boxes[i].width;
        status->boxes[i + 2].y = status->boxes[i].y;
        status->boxes[i + 2].height = status->boxes[i].height;
        endPrintExtFunc(status->style, END_PRINT_CMD_Z_SCISSOR,&status->boxes[i + 2]);
    }
    endPrintExtFunc(status->style, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_SCISSOR_RESET, 0);
}

/*
 * Updates the AGWS status rows: builds them on the first run, slides them in for the
 * item list and target modes, and fills each row's name and HP display.
 */
void MenuItemStatusAgwsMain(void)
{
    MenuItemAgwsStatusWork *status = MenuItemStatusAgws;
    short targetX[6];
    int attack[3];
    int defense[2];
    register int i;

    switch (status->state) {
    case MENU_ITEM_STATE_INIT: {
        status->color = MENU_ITEM_DRAW_KEY_STATUS;
        for (i = 0; i < MenuWork.agwsCount; i++) {
            WindowDXSet(&status->rows[i].window);
            status->rows[i].window.width = MENU_ITEM_STATUS_WINDOW_WIDTH;
            status->rows[i].window.height = MENU_ITEM_AGWS_ROW_HEIGHT;
            status->rows[i].window.style = status->color;
            status->rows[i].window.x = MENU_ITEM_SLIDE_X_HIDDEN;
            status->rows[i].window.state = 1;
            status->rows[i].window.y = i * status->rows[i].window.height + MENU_ITEM_STATUS_TOP_Y;
            WindowDXMain(&status->rows[i].window);
            status->rows[i].window.state = 3;
            eMessageSet(&status->rows[i].name, MenuCharNameGet(MenuItemAgws[i]));
            status->rows[i].name.mode = 32;
            status->rows[i].name.draw_parameter = status->rows[i].window.style + 2;
            eTagFontSet(&status->rows[i].hpLabel, fhpText);
            status->rows[i].hpLabel.work = status->color + 2;
            status->rows[i].hpLabel.color[0] = 112;
            eNumberSet(&status->rows[i].hpNumber, 0);
            status->rows[i].hpNumber.work = status->color + 2;
            status->rows[i].hpNumber.format = 12;
            status->rows[i].hpNumber.digits = 4;
        }
        eCursolSet(&status->cursor, 0);
        status->cursor.state = 32;
        status->state = MENU_ITEM_STATE_RUN;
        status->cursor.work = status->color + 4;
    }
        /* fall through */
    case MENU_ITEM_STATE_RUN:
        break;
    default:
        return;
    }

    for (i = 0; i < MenuWork.agwsCount; i++) {
        targetX[i] = MENU_ITEM_SLIDE_X_HIDDEN;
    }
    if (MenuWork.mode == MENU_ITEM_MODE_LIST || MenuWork.mode == MENU_ITEM_MODE_TARGET) {
        if (MenuWork.flags & MENU_ITEM_FLAG_MODE_ENTERED) {
            if (MenuWork.mode == MENU_ITEM_MODE_LIST) {
                status->cursor.state = 0;
            } else {
                status->cursor.state = 32;
            }
        }
        if (MenuWork.agwsMode == 1) {
            for (i = 0; i < MenuWork.agwsCount; i++) {
                targetX[i] = MENU_ITEM_SLIDE_X_SHOWN;
            }
        }
    }
    /* Each row's name, HP label and HP number follow its window. */
    i = 0;
    while (i < MenuWork.agwsCount) {
        MenuItemHpParameters *parameters = func_00A11108(MenuItemAgws[i], attack, defense);

        MoveSlide(&status->rows[i].window.x, &targetX[i], MENU_ITEM_SLIDE_RATE);
        WindowDXMain(&status->rows[i].window);
        status->rows[i].name.x = status->rows[i].window.x + 19;
        status->rows[i].name.y = status->rows[i].window.y + 3;
        eMessageMain(&status->rows[i].name);
        status->rows[i].hpLabel.x = status->rows[i].window.x + 123;
        status->rows[i].hpLabel.y = status->rows[i].window.y + 29;
        eTagFontMain(&status->rows[i].hpLabel);
        {
            unsigned int packedHp = parameters->hp + ((unsigned int)parameters->maxHp << 16);

            status->rows[i].hpNumber.value = packedHp;
            status->rows[i].hpNumber.x = status->rows[i].hpLabel.x + 52;
            status->rows[i].hpNumber.y = status->rows[i].hpLabel.y;
        }
        eNumberMain(&status->rows[i].hpNumber);
        i++;
    }
    if (MenuWork.agwsCount != 0) {
        status->cursor.x = status->rows[MenuWork.selectedAgws].window.x + 3;
        status->cursor.y = status->rows[MenuWork.selectedAgws].window.y + 19;
        eCursolMain(&status->cursor);
    }
}

/*
 * Rebuilds the item list rows (tab 0) from a fresh item sort. Unusable and restricted
 * items are flagged, and the list window is set up for one column.
 */
void MenuItemListMake00(void)
{
    MenuListEntry *rows = MenuListGet(0);
    int *sortRow = MenuSortAddrGet(0);
    MenuItemListWork *work;
    int count;
    int i;

    MenuSortSet(0, MENU_SORT_ITEM, 0);
    MenuSortChange(0, 0);
    MenuSortChange(0, MenuWork.sortChoices[0]);
    MenuListMake(0, 0);
    count = MenuSortCheck(0);
    for (i = 0; i < count; i++) {
        int use = MenuItemUseCheck((short) sortRow[i]);

        switch (use) {
        case MENU_ITEM_USE_BLOCKED_SPECIAL:
            rows[i].flag = 1;
            rows[i].status = MENU_ITEM_ROW_RESTRICTED;
            break;
        case MENU_ITEM_USE_UNUSABLE:
            rows[i].flag = 1;
            rows[i].status = MENU_ITEM_ROW_UNAVAILABLE;
            break;
        case MENU_ITEM_USE_SINGLE:
        case MENU_ITEM_USE_ALL:
        default:
            rows[i].flag = 0;
            rows[i].status = 0;
            break;
        }
    }
    work = MenuItemList;
    work->window.draw_flags = 7;
    work->window.title = listNumText;
    MenuItemList->window.columns = 1;
    MenuItemList->window.rows = MENU_ITEM_LIST_ROWS;
    MenuItemList->window.width = MENU_ITEM_LIST_WIDTH_ITEMS;
    MenuItemList->window.height = MenuItemList->window.rows * MENU_ITEM_LIST_ROW_HEIGHT + MENU_ITEM_LIST_PADDING;
    MenuItemList->window.items = MenuListGet(0);
    WindowSPItemChange(&MenuItemList->window);
    WindowSPSetSelect(&MenuItemList->window, MenuKeepSelect);
}

/* Refreshes the item list rows and window without changing the sort; flags as MenuItemListMake00. */
void MenuItemListMake00_1(void)
{
    MenuListEntry *rows = MenuListGet(0);
    int *sortRow = MenuSortAddrGet(0);
    MenuItemListWork *work;
    int count;
    int i;

    MenuListMake(0, 0);
    count = MenuSortCheck(0);
    for (i = 0; i < count; i++) {
        int use = MenuItemUseCheck((short) sortRow[i]);

        switch (use) {
        case MENU_ITEM_USE_BLOCKED_SPECIAL:
            rows[i].flag = 1;
            rows[i].status = MENU_ITEM_ROW_RESTRICTED;
            break;
        case MENU_ITEM_USE_UNUSABLE:
            rows[i].flag = 1;
            rows[i].status = MENU_ITEM_ROW_UNAVAILABLE;
            break;
        case MENU_ITEM_USE_SINGLE:
        case MENU_ITEM_USE_ALL:
            rows[i].flag = 0;
            rows[i].status = 0;
            break;
        }
    }
    work = MenuItemList;
    work->window.draw_flags = 7;
    work->window.title = listNumText;
    MenuItemList->window.columns = 1;
    MenuItemList->window.rows = MENU_ITEM_LIST_ROWS;
    MenuItemList->window.width = MENU_ITEM_LIST_WIDTH_ITEMS;
    MenuItemList->window.height = MenuItemList->window.rows * MENU_ITEM_LIST_ROW_HEIGHT + MENU_ITEM_LIST_PADDING;
    MenuItemList->window.items = MenuListGet(0);
    WindowSPItemChange(&MenuItemList->window);
    WindowSPSetSelect(&MenuItemList->window, MenuKeepSelect);
}

/*
 * Builds the equipment list for category 0 to 5 (weapon, accessory or bullet, for
 * characters or for AGWS) in two columns.
 */
void MenuItemListMake01(int category)
{
    MenuItemListWork *work;

    switch (category) {
    case 0:
        MenuSortSet(0, MENU_SORT_WEAPON, MENU_SORT_FILTER_CHARACTER);
        break;
    case 1:
        MenuSortSet(0, MENU_SORT_ACCESSORY, MENU_SORT_FILTER_CHARACTER);
        break;
    case 2:
        MenuSortSet(0, MENU_SORT_BULLET, MENU_SORT_FILTER_CHARACTER);
        break;
    case 3:
        MenuSortSet(0, MENU_SORT_WEAPON, MENU_SORT_FILTER_AGWS);
        break;
    case 4:
        MenuSortSet(0, MENU_SORT_ACCESSORY, MENU_SORT_FILTER_AGWS);
        break;
    case 5:
        MenuSortSet(0, MENU_SORT_BULLET, MENU_SORT_FILTER_AGWS);
        break;
    }
    MenuSortChange(0, 0);
    MenuSortChange(0, MenuWork.sortChoices[category + MENU_ITEM_TAB_EQUIPMENT_FIRST]);
    MenuListMake(0, 0);
    work = MenuItemList;
    work->window.draw_flags = 7;
    work->window.title = listNum3NumText;
    MenuItemList->window.columns = 2;
    MenuItemList->window.rows = MENU_ITEM_LIST_ROWS;
    MenuItemList->window.width = MENU_ITEM_LIST_WIDTH_EQUIPMENT;
    MenuItemList->window.height = MenuItemList->window.rows * MENU_ITEM_LIST_ROW_HEIGHT + MENU_ITEM_LIST_PADDING;
    MenuItemList->window.items = MenuListGet(0);
    WindowSPItemChange(&MenuItemList->window);
    WindowSPSetSelect(&MenuItemList->window, &MenuKeepSelect[category * MENU_ITEM_KEEP_SELECT_SIZE + 10]);
}

/* Builds the event item list in two columns. */
void MenuItemListMake02(void)
{
    struct MenuItemListWork *list;

    MenuSortSet(0, MENU_SORT_EVENT_ITEM, 0);
    MenuSortChange(0, 0);
    MenuSortChange(0, MenuWork.sortChoices[MENU_ITEM_TAB_EVENT]);
    MenuListMake(0, 0);

    list = MenuItemList;
    list->window.draw_flags = 7;
    list->window.title = listNum3NumText;
    MenuItemList->window.columns = 2;
    MenuItemList->window.rows = MENU_ITEM_LIST_ROWS;
    MenuItemList->window.width = MENU_ITEM_LIST_WIDTH_EQUIPMENT;
    MenuItemList->window.height = MenuItemList->window.rows * MENU_ITEM_LIST_ROW_HEIGHT + MENU_ITEM_LIST_PADDING;
    MenuItemList->window.items = MenuListGet(0);

    WindowSPItemChange(&MenuItemList->window);
    WindowSPSetSelect(&MenuItemList->window, &MenuKeepSelect[MENU_ITEM_KEEP_SELECT_SIZE]);
}

/*
 * Updates the item list window: rebuilds its rows when the tab changes or a sort is
 * requested, and slides it to the position for the current mode.
 */
void MenuItemListMain(void)
{
    MenuItemListWork *list = MenuItemList;
    short targetX;
    int i;

    switch (list->state) {
    case MENU_ITEM_STATE_INIT:
        list->window.x = MENU_ITEM_LIST_X_HIDDEN;
        list->window.style = list->color = MENU_ITEM_DRAW_KEY;
        list->window.title = msg00_1_0036D698;
        list->window.y = MENU_ITEM_LIST_Y;
        list->window.draw_flags = 7;
        list->window.rows = MENU_ITEM_LIST_ROWS;
        list->window.height = MENU_ITEM_LIST_HEIGHT;
        list->window.items = 0;
        WindowSPSet(&list->window);
        list->visible = 1;
        list->activeTab = -1;
        list->state = MENU_ITEM_STATE_RUN;
        list->motion = MENU_ITEM_LIST_MOTION_START;
        /* fall through */
    case MENU_ITEM_STATE_RUN:
        break;
    default:
        return;
    }

    if (MenuWork.mode == MENU_ITEM_MODE_CLOSE)
        list->motion = MENU_ITEM_LIST_MOTION_HIDDEN;

    /* Rebuild the rows on a tab change, idle at the position for the tab and mode, then follow target or sort. */
    switch (list->motion) {
    case MENU_ITEM_LIST_MOTION_START:
        list->motion = MENU_ITEM_LIST_MOTION_REBUILD;
        /* fall through */
    case MENU_ITEM_LIST_MOTION_REBUILD:
        list->window.x = MENU_ITEM_LIST_X_HIDDEN;
        switch (MenuWork.listTab) {
        case MENU_ITEM_TAB_ITEMS:
            MenuItemListMake00();
            break;
        case MENU_ITEM_TAB_EVENT:
            MenuItemListMake02();
            break;
        default:
            MenuItemListMake01(MenuWork.listTab - MENU_ITEM_TAB_EQUIPMENT_FIRST);
            break;
        }
        list->window.state = 17;
        WindowSPMain(&list->window);
        list->motion = MENU_ITEM_LIST_MOTION_IDLE;
        /* fall through */
    case MENU_ITEM_LIST_MOTION_IDLE:
        if (list->activeTab != MenuWork.listTab) {
            list->activeTab = MenuWork.listTab;
            list->motion = MENU_ITEM_LIST_MOTION_REBUILD;
        }
        if (MenuWork.listTab == MENU_ITEM_TAB_ITEMS)
            targetX = MENU_ITEM_LIST_X_BESIDE_STATUS;
        else if (MenuWork.mode == MENU_ITEM_MODE_SEGMENT)
            targetX = MENU_ITEM_LIST_X_HIDDEN;
        else
            targetX = MENU_ITEM_LIST_X_FULL;
        switch (MenuWork.mode) {
        case MENU_ITEM_MODE_TARGET:
            list->motion = MENU_ITEM_LIST_MOTION_TARGET;
            break;
        case MENU_ITEM_MODE_SORT:
            list->motion = MENU_ITEM_LIST_MOTION_SORT;
            break;
        }
        break;
    case MENU_ITEM_LIST_MOTION_TARGET:
        if (MenuWork.mode != MENU_ITEM_MODE_TARGET)
            list->motion = MENU_ITEM_LIST_MOTION_IDLE;
        targetX = MENU_ITEM_LIST_X_BESIDE_STATUS;
        break;
    case MENU_ITEM_LIST_MOTION_SORT:
        if (PadData.pressed & PAD_CIRCLE) {
            switch (MenuWork.listTab) {
            case MENU_ITEM_TAB_ITEMS:
                MenuItemListMake00();
                break;
            case MENU_ITEM_TAB_EVENT:
                MenuItemListMake02();
                break;
            default:
                MenuItemListMake01(MenuWork.listTab - MENU_ITEM_TAB_EQUIPMENT_FIRST);
                break;
            }
        }
        if (MenuWork.mode != MENU_ITEM_MODE_SORT)
            list->motion = MENU_ITEM_LIST_MOTION_IDLE;
        targetX = MENU_ITEM_LIST_X_BESIDE_STATUS;
        break;
    case MENU_ITEM_LIST_MOTION_HIDDEN:
        targetX = MENU_ITEM_LIST_X_HIDDEN;
        break;
    default:
        break;
    }
    for (i = 0; i < 1; i++) {
        short *currentX = &list[i].window.x;
        short slideEnd = MENU_ITEM_LIST_X_HIDDEN;

        MoveSlide(currentX, &targetX, MENU_ITEM_SLIDE_RATE);
        if (*currentX != slideEnd)
            WindowSPMain(&list[i].window);
    }
}

/*
 * Updates the sort-selection window: builds it on the first run, slides it in while the
 * sort mode is active, and places its sprite on the current tab's sort choice.
 */
void MenuItemSelectMain(void)
{
    MenuItemSelectWork *select = MenuItemSelect;
    struct {
        short x;
        short y;
        short mode;
    } slideTarget;
    short *targetY;
    unsigned char sortMode;
    MenuItemWorkData *menu;
    int n;
    int m;
    short windowWidth;
    short windowHeight;

    switch (select->state) {
    case MENU_ITEM_STATE_INIT:
        select->color = MENU_ITEM_DRAW_KEY;
        windowWidth = 176;
        windowHeight = 102;
        for (n = 0, m = 0; n < 1; n++, m++) {
        int *color = &select->color;
        const char **titleTable = msg10_3;
        const char **sortTextTable = msg00_2_0036D6A8;
        short *size = &select->window[n].width;
        int spriteColor;
        int windowState;

        WindowDXSet(&select->window[n]);
        select->window[n].tag_id = titleTable[0];
        windowState = 3;
        select->window[m].close_callback_arg = &select->content;
        select->window[n].style = *color;
        select->window[n].close_callback = MenuSelectWindow;
        size[0] = windowWidth;
        select->content.text = sortTextTable[n];
        select->window[0].height = windowHeight;
        select->content.mode = 0;
        select->content.selection = 0;
        select->window[n].x = -208;
        select->window[n].y = 160;
        select->window[n].state = 1;
        WindowDXMain(&select->window[n]);
        select->window[n].state = windowState;
        eSpriteSet(&select->sprite, 0x60A);
        spriteColor = *color;
        select->unmodeled_02 = 0;
        select->state = n + 2;
        select->sprite.color = spriteColor + 2;
    }
        /* fall through */
    case MENU_ITEM_STATE_RUN:
        break;
    default:
        return;
    }

    menu = &MenuWork;
    targetY = &slideTarget.y;
    slideTarget.x = -208;
    *targetY = 160;
    slideTarget.mode = 0;
    if (menu->mode == MENU_ITEM_MODE_SORT) {
        slideTarget.x = 80;
        slideTarget.mode = 1;
        select->content.selection = menu->sortSelection;
    }
    {
        short *currentX = &select->window[0].x;
        signed char tab;

        MoveSlide(currentX, &slideTarget.x, MENU_ITEM_SLIDE_RATE);
        MoveSlide(&select->window[0].y, targetY, MENU_ITEM_SLIDE_RATE);
        WindowDXMain(&select->window[0]);
        select->sprite.x = (unsigned short)*currentX + 144;

        /* One sort choice per tab: items, event items, then six equipment categories. */
        tab = menu->listTab;
        sortMode = menu->sortChoices[tab];
        select->sprite.y = (unsigned short)select->window[0].y + ((signed char)sortMode / 2) * 24 + 2;
        eSpriteMain(&select->sprite);
    }
}

/*
 * Updates the header icons: two scroll arrows and one icon per tab, each with a slide
 * animation; the tab icons also bounce.
 */
void MenuItemIconMain(void)
{
    MenuItemIconWork *icon = MenuItemIcon;
    signed char *tabBounce;
    signed char *tabBounceIterator;
    int i;

    if (MenuWork.mode == MENU_ITEM_MODE_INIT) {
        return;
    }
    switch (icon->state) {
    case MENU_ITEM_STATE_INIT: {
        MenuItemSpriteIds spriteIds = iconSpriteTable;
        signed char *bounce;

        icon->style = MENU_ITEM_DRAW_KEY;
        for (i = 0; i < MENU_ITEM_ICON_COUNT; i++) {
            eSpriteSet(&icon->sprites[i], spriteIds.ids[i]);
            icon->sprites[i].style = icon->style;
        }
        for (i = 0; i < MENU_ITEM_ICON_ARROWS; i++) {
            icon->sprites[i].red = icon->sprites[i].green = icon->sprites[i].blue = -128;
        }
        for (; i < MENU_ITEM_ICON_COUNT; i++) {
            icon->sprites[i].red = icon->sprites[i].green = icon->sprites[i].blue = -128;
        }
        icon->pulseDirection = 1;
        tabBounce = &icon->displacement[2];
        icon->slide = MENU_ITEM_ICON_SLIDE_START;
        icon->pulse = 0;
        icon->pulsePhase = 0;
        icon->displacement[0] = 0;
        icon->displacement[1] = 0;
        bounce = tabBounce + 7;
        for (i = 7; i >= 0; i--) {
            *bounce-- = 0;
        }
        icon->state = MENU_ITEM_STATE_RUN;
        break;
    }
    case MENU_ITEM_STATE_RUN:
        tabBounce = &icon->displacement[2];
        break;
    default:
        return;
    }

    /* Slide the icons away for the close and done modes, and back for the others. */
    switch (MenuWork.mode) {
    case MENU_ITEM_MODE_CLOSE:
        icon->pulseDirection = 0;
        /* fall through */
    case MENU_ITEM_MODE_DONE:
        if (icon->slide != MENU_ITEM_ICON_SLIDE_MAX) {
            icon->slide++;
        }
        break;
    default:
        if (icon->slide != 0) {
            icon->slide--;
        }
        break;
    }
    if (MenuWork.mode == MENU_ITEM_MODE_LIST && MenuWork.tabChangeDirection != 0) {
        if (MenuWork.tabChangeDirection < 0) {
            icon->displacement[0] = -6;
        } else {
            icon->displacement[1] = 6;
        }
        MenuWork.tabChangeDirection = 0;
    }
    if (icon->displacement[0] != 0) {
        icon->displacement[0]++;
    }
    if (icon->displacement[1] != 0) {
        icon->displacement[1]--;
    }
    if (icon->pulsePhase & 1) {
        if (icon->pulse >= 11) {
            icon->pulseDirection = -icon->pulseDirection;
        }
        icon->pulse += icon->pulseDirection;
    }
    icon->pulsePhase++;

    for (i = 0; i < MENU_ITEM_ICON_ARROWS; i++) {
        float cosine;
        int slideOffset;

        icon->sprites[i].x = 264 + i * 208 + icon->displacement[i];
        cosine = xglCos(i * menuItemPi);
        slideOffset = -2 * icon->slide;
        icon->sprites[i].y = 13;
        icon->sprites[i].x += cosine * slideOffset;
        icon->sprites[i].alpha = 128.0f - (icon->slide / MENU_ITEM_ICON_SLIDE_MAX_F) * 128.0f;
        if (MenuWork.mode == MENU_ITEM_MODE_SEGMENT) {
            icon->sprites[i].red = icon->sprites[i].green = icon->sprites[i].blue = 64;
        } else {
            icon->sprites[i].red = icon->sprites[i].green = icon->sprites[i].blue = -128;
        }
    }
    /* The selected tab's icon bounces up while the icons are shown; the others bounce back down. */
    tabBounceIterator = tabBounce;
    for (i = MENU_ITEM_ICON_ARROWS; i < MENU_ITEM_ICON_COUNT; i++) {
        int tab = i - MENU_ITEM_ICON_ARROWS;
        float cosine;
        int slideOffset;

        icon->sprites[i].x = MENU_ITEM_TAB_ICON_X + tab * MENU_ITEM_TAB_ICON_SPACING;
        cosine = xglCos(tab * iconAngleStep);
        slideOffset = -2 * icon->slide;
        icon->sprites[i].y = 12;
        icon->sprites[i].x += cosine * slideOffset;
        icon->sprites[i].y += *tabBounceIterator;
        if (icon->slide == 0 && MenuWork.listTab == tab) {
            if (*tabBounceIterator != 6) {
                (*tabBounceIterator)++;
            }
        } else if (*tabBounceIterator != 0) {
            (*tabBounceIterator)--;
        }
        icon->sprites[i].alpha = 128.0f - (icon->slide / MENU_ITEM_ICON_SLIDE_MAX_F) * 128.0f;
        tabBounceIterator++;
    }
    for (i = 0; i < MENU_ITEM_ICON_COUNT; i++) {
        eSpriteMain(&icon->sprites[i]);
    }
}

/*
 * Updates the sort hint message. It slides in while the list is shown, and its text
 * color follows the selection direction.
 */
void MenuItemExMain(void)
{
    MenuItemExWork *ex = MenuItemEx;
    MenuItemMessage *message = &ex->message;
    int *messageColor = &ex->message.color;
    short targetX;
    char *textColor;
    int i;

    switch (ex->state) {
    case MENU_ITEM_STATE_INIT: {
        int initialX = MENU_ITEM_EX_X_HIDDEN;
        int initialY = 345;
        int messageMode = 32;

        ex->color = MENU_ITEM_DRAW_KEY_EX;
        eMessageSet((EMessageParam *)message, msg00_4_0036D6B8[0]);
        ex->message.x = initialX;
        {
            int messageColorValue = ex->color;
            ex->message.y = initialY;
            *messageColor = messageColorValue;
            ex->message.mode = messageMode;
            ex->state = MENU_ITEM_STATE_RUN;
        }
    }
        /* fall through */
    case MENU_ITEM_STATE_RUN:
        targetX = MENU_ITEM_EX_X_HIDDEN;
        if (MenuWork.mode == MENU_ITEM_MODE_LIST) {
            targetX = MENU_ITEM_EX_X_SHOWN;
            textColor = msg00_4_0036D6B8[0] + 1;
            if (MenuWork.selectDir >= 0) {
                textColor[0] = textColor[1] = textColor[2] = -128;
            } else {
                textColor[0] = textColor[1] = textColor[2] = 64;
            }
        }
        for (i = 0; i < 1; i++) {
            MoveSlide(&ex[i].message.x, &targetX, MENU_ITEM_SLIDE_RATE);
            eMessageMain((EMessageParam *)&ex[i].message);
        }
    }
}

/*
 * Returns the robo-part bits for part (1 to 9). The value is larger when the part's
 * event flag is set.
 */
int subRoboPartsCheck(int part)
{
    /* Reading the result back through its address keeps it in memory until
     * the address is purged, so case 3 becomes a conditional move only in
     * if-conversion, which keeps the original xori v0,v0,0. */
    int *resultAddress;
    int result = 0;

    switch (part) {
    case 1:
        result = 1;
        if (xglFlagsGet(3301, 1)) {
            result = 65;
        }
        break;
    case 2:
        result = 2;
        if (xglFlagsGet(3303, 1)) {
            result = 130;
        }
        break;
    case 9:
        result = 4;
        if (xglFlagsGet(3302, 1)) {
            result = 268;
        }
        break;
    case 7:
        result = 8;
        if (xglFlagsGet(3302, 1)) {
            result = 268;
        }
        break;
    case 8:
        result = 16;
        if (xglFlagsGet(3304, 1)) {
            result = 560;
        }
        break;
    case 3:
        result = (xglFlagsGet(3304, 1) == 0) ? (32) : (560);
        break;
    }
    resultAddress = &result;
    return *resultAddress;
}

/*
 * Updates the segment (robo parts) map: initializes its rows and sprites on the first
 * run, slides it in and out, moves the selection in browse state, and draws it.
 */
void MenuItemSegmentMain(void)
{
    MenuItemSegmentWork *segment = MenuItemSegment;
    int i;
    MenuItemSegmentAlignedWork *aligned = (MenuItemSegmentAlignedWork *)segment;

    /* State machine: init, slide in, browse the segments, slide out. */
    switch (segment->state) {
    case MENU_ITEM_STATE_INIT: {
        segment->x = -512;
        segment->y = 52;
        segment->work = MENU_ITEM_DRAW_KEY_SEGMENT;
        segment->visible = 0;
        segment->selected = 0;
        for (i = 0; i < MENU_SEGMENT_COUNT; i++) {
            MenuItemSegmentRow *row = &segment->rows[i];

            row->flags = 0;
            if (xglFlagsGet(i + MENU_FLAG_SEGMENT_LOCATION_BASE, 1)) {
                row->flags |= MENU_SEGMENT_ROW_LOCATION;
            }
            if (xglFlagsGet(i + 3221, 1)) {
                row->flags |= 2;
            }
            if (xglFlagsGet(i + MENU_FLAG_SEGMENT_RARE_ITEM_BASE, 1)) {
                row->flags |= MENU_SEGMENT_ROW_RARE_ITEM;
            }
        }
        segment->ownedParts = 0;
        for (i = 0; i < MENU_SEGMENT_COUNT; i++) {
            if (segment->rows[i].flags & MENU_SEGMENT_ROW_RARE_ITEM) {
                segment->ownedParts |= subRoboPartsCheck(i + 1);
            }
        }
        segment->outlines[0].pulse = 32;
        segment->selectedParts = 0;
        segment->outlines[0].pulseDirection = 4;
        segment->rows[0].pulseDirection = 4;
        segment->outlines[1].pulse = 0;
        segment->outlines[1].pulseDirection = 0;
        segment->rows[0].pulse = 0;
        for (i = 0; i < MENU_SEGMENT_COUNT; i++) {
            int rowNumber = i + 1;

            eNumberSet((ENumberMainData *)&aligned->rows[i].number, 0);
            aligned->rows[i].number.format = 0;
            aligned->rows[i].number.value = rowNumber;
            aligned->rows[i].number.digitCount = 2;
            if (segment->rows[i].flags == MENU_SEGMENT_ROW_COMPLETE) {
                eSpriteSet(&segment->rows[i].sprites[0], MENU_SEGMENT_SPRITE_TILE_COMPLETE);
            } else {
                eSpriteSet(&segment->rows[i].sprites[0], MENU_SEGMENT_SPRITE_TILE);
            }
            eSpriteSet(&segment->rows[i].sprites[1], MENU_SEGMENT_SPRITE_MARK_LOCATION);
            eSpriteSet(&segment->rows[i].sprites[2], 4869);
            segment->rows[i].emphasis = 0;
            segment->rows[i].emphasisDirection = 0;
            eSpriteSet(&segment->rows[i].sprites[3], MENU_SEGMENT_SPRITE_COMPLETE_GLOW);
            eSpriteSet(&segment->rows[i].sprites[4], MENU_SEGMENT_SPRITE_HIGHLIGHT);
        }
        {
            short mapSpriteIds[19] = {
                4885, 4878, 4872, 4874, 4874, 4876, 4876, 4881, 4884, 4883,
                4880, 4882, 4879, 4877, 4871, 4873, 4873, 4875, 4875
            };

            for (i = 0; i < 19; i++) {
                eSpriteSet(&segment->mapSprites[i], mapSpriteIds[i]);
            }
        }
        eRibbonSet((ERibbonMainData *)&segment->ribbon, 3);
        {
            short labelSpriteIds[2] = {4886, 4887};

            for (i = 0; i < 2; i++) {
                WindowDXSet(&segment->windows[i]);
                segment->windows[i].style = 0;
                segment->windows[i].tag_id = tag_msg_5[i];
                segment->windows[i].x = 544;
                segment->windows[i].width = 247;
                segment->windows[i].height = (2 - i) * 24 + 7;
                segment->windows[i].state = 1;
                segment->windows[i].y = 0;
                WindowDXMain(&segment->windows[i]);
                segment->windows[i].state = 3;
                eSpriteSet(&segment->labels[i], labelSpriteIds[i]);
                eMessageSet(&segment->text[i], temporaryText);
                segment->text[i].mode = 32;
            }
        }
        segment->state = MENU_ITEM_STATE_RUN;
    }
        /* fall through */
    case MENU_ITEM_STATE_RUN:
        if (MenuWork.mode == MENU_ITEM_MODE_SEGMENT) {
            segment->state = MENU_SEGMENT_STATE_SLIDE_IN;
            segment->visible = 1;
        }
        break;
    case MENU_SEGMENT_STATE_SLIDE_IN: {
        short targetX = MENU_ITEM_SLIDE_X_SHOWN;

        MoveSlide(&segment->x, &targetX, MENU_ITEM_SLIDE_RATE);
        if (segment->x == targetX) {
            segment->state = MENU_SEGMENT_STATE_BROWSE;
        }
        if (MenuWork.mode != MENU_ITEM_MODE_SEGMENT) {
            segment->state = MENU_SEGMENT_STATE_SLIDE_OUT;
        }
        break;
    }
    case MENU_SEGMENT_STATE_BROWSE: {
        int movement = 0;

        switch (PadData.activity) {
        case PAD_UP:
            movement = -MENU_SEGMENT_COLUMNS;
            break;
        case PAD_LEFT:
            movement = -1;
            break;
        case PAD_DOWN:
            movement = MENU_SEGMENT_COLUMNS;
            break;
        case PAD_RIGHT:
            movement = 1;
            break;
        }
        if (movement != 0) {
            segment->selected += movement;
            if (segment->selected < 0) {
                segment->selected += MENU_SEGMENT_COUNT;
            } else if (segment->selected >= MENU_SEGMENT_COUNT) {
                segment->selected -= MENU_SEGMENT_COUNT;
            }
            xglSoundEffectNormalID(SE_CURSOR, 0);
        }
        segment->selectedParts = subRoboPartsCheck(segment->selected + 1);
        if (MenuWork.mode != MENU_ITEM_MODE_SEGMENT) {
            segment->state = MENU_SEGMENT_STATE_SLIDE_OUT;
        }
        MenuWork.segmentSelection = segment->selected + 1;
        break;
    }
    case MENU_SEGMENT_STATE_SLIDE_OUT: {
        short targetX = -512;

        MoveSlide(&segment->x, &targetX, MENU_ITEM_SLIDE_RATE);
        if (segment->x == targetX) {
            segment->visible = 0;
            segment->state = MENU_ITEM_STATE_RUN;
        }
        break;
    }
    }
    if (segment->visible == 0) {
        return;
    }
    for (i = 0; i < 2; i++) {
        segment->windows[i].x = segment->x + 238;
        segment->windows[i].style = segment->work;
        segment->windows[i].y = aligned->y + i * 80 + 16;
        WindowDXMain(&segment->windows[i]);
    }
    if (segment->rows[segment->selected].flags & MENU_SEGMENT_ROW_LOCATION) {
        segment->text[0].x = segment->windows[0].x + 7;
        segment->text[0].y = segment->windows[0].y + 3;
        segment->text[0].draw_parameter = segment->windows[0].style + 2;
        segment->text[0].text = MenuSegmentMapNameGet(segment->selected + 1);
        eMessageMain(&segment->text[0]);
    }
    {
        const char *itemName;
        MenuItemInfoWork *info = MenuItemInfo;
        char *description = info->buffer;

        if (segment->rows[segment->selected].flags & MENU_SEGMENT_ROW_RARE_ITEM) {
            switch (segment->selected) {
            case 0:
                if (segment->ownedParts & 0x40) {
                    itemName = MenuSegmentItemNameGet(19);
                    strcpy(description, MenuSegmentInfoTextGet(19));
                } else {
                    itemName = dataEvtItmNameGet(56);
                    strcpy(description, MenuSegmentInfoTextGet(segment->selected + 1));
                }
                break;
            case 1:
                if (segment->ownedParts & 0x80) {
                    itemName = MenuSegmentItemNameGet(21);
                    strcpy(description, MenuSegmentInfoTextGet(21));
                } else {
                    itemName = dataEvtItmNameGet(59);
                    strcpy(description, MenuSegmentInfoTextGet(segment->selected + 1));
                }
                break;
            case 8:
                if (segment->ownedParts & 0x100) {
                    itemName = MenuSegmentItemNameGet(20);
                    strcpy(description, MenuSegmentInfoTextGet(20));
                } else {
                    itemName = dataEvtItmNameGet(58);
                    strcpy(description, MenuSegmentInfoTextGet(segment->selected + 1));
                }
                break;
            case 6:
                if (segment->ownedParts & 0x100) {
                    itemName = MenuSegmentItemNameGet(20);
                    strcpy(description, MenuSegmentInfoTextGet(20));
                } else {
                    itemName = dataEvtItmNameGet(57);
                    strcpy(description, MenuSegmentInfoTextGet(segment->selected + 1));
                }
                break;
            case 7:
                if (segment->ownedParts & 0x200) {
                    itemName = MenuSegmentItemNameGet(22);
                    strcpy(description, MenuSegmentInfoTextGet(22));
                } else {
                    itemName = dataEvtItmNameGet(61);
                    strcpy(description, MenuSegmentInfoTextGet(segment->selected + 1));
                }
                break;
            case 2:
                if (segment->ownedParts & 0x200) {
                    itemName = MenuSegmentItemNameGet(22);
                    strcpy(description, MenuSegmentInfoTextGet(22));
                } else {
                    itemName = dataEvtItmNameGet(60);
                    strcpy(description, MenuSegmentInfoTextGet(segment->selected + 1));
                }
                break;
            default:
                itemName = MenuSegmentItemNameGet(segment->selected + 1);
                strcpy(description, MenuSegmentInfoTextGet(segment->selected + 1));
                break;
            }
            segment->text[1].text = itemName;
            segment->text[1].x = segment->windows[1].x + 7;
            segment->text[1].y = segment->windows[1].y + 3;
            segment->text[1].draw_parameter = segment->windows[1].style + 2;
            eMessageMain(&segment->text[1]);
        } else {
            description[0] = emptyText[0];
        }
    }
    segment->texture = WindowTexAddrGet(2);
    endPrintExtFunc(0, END_PRINT_CMD_FONT_TEX_CHANGE,&segment->texture);
    segment->rows[0].pulse += segment->rows[0].pulseDirection;
    if (segment->rows[0].pulse == MENU_SEGMENT_PULSE_MAX || segment->rows[0].pulse == 0) {
        segment->rows[0].pulseDirection = -segment->rows[0].pulseDirection;
    }
    /* Draw each segment tile with its marks, complete glow and emphasis highlight. */
    for (i = 0; i < MENU_SEGMENT_COUNT; i++) {
        segment->rows[i].sprites[0].x = segment->x + (i % MENU_SEGMENT_COLUMNS) * MENU_SEGMENT_TILE_WIDTH;
        segment->rows[i].sprites[0].y = segment->y + (i / MENU_SEGMENT_COLUMNS) * MENU_SEGMENT_TILE_HEIGHT;
        segment->rows[i].sprites[0].style = segment->work;
        eSpriteMain(&segment->rows[i].sprites[0]);
        if (segment->rows[i].flags & MENU_SEGMENT_ROW_LOCATION) {
            segment->rows[i].sprites[1].x = segment->rows[i].sprites[0].x + 10;
            segment->rows[i].sprites[1].style = segment->rows[i].sprites[0].style + 1;
            segment->rows[i].sprites[1].y = segment->rows[i].sprites[0].y + 29;
            eSpriteMain(&segment->rows[i].sprites[1]);
        }
        if (segment->rows[i].flags & 2) {
            segment->rows[i].sprites[2].x = segment->rows[i].sprites[0].x + 42;
            segment->rows[i].sprites[2].style = segment->rows[i].sprites[0].style + 1;
            segment->rows[i].sprites[2].y = segment->rows[i].sprites[0].y + 29;
            eSpriteMain(&segment->rows[i].sprites[2]);
        }
        if (segment->rows[i].flags == MENU_SEGMENT_ROW_COMPLETE) {
            segment->rows[i].sprites[3].x = segment->rows[i].sprites[0].x + 39;
            segment->rows[i].sprites[3].style = segment->rows[i].sprites[0].style + 1;
            segment->rows[i].sprites[3].y = segment->rows[i].sprites[0].y;
            segment->rows[i].sprites[3].alpha = segment->rows[0].pulse;
            eSpriteMain(&segment->rows[i].sprites[3]);
        }
        {
            int highlighted = 0;

            if (segment->selected == i) {
                highlighted = 1;
            } else if (segment->selectedParts != 0 &&
                       (segment->rows[segment->selected].flags & MENU_SEGMENT_ROW_RARE_ITEM) &&
                       subRoboPartsCheck(i + 1) != 0) {
                if (segment->rows[i].flags & MENU_SEGMENT_ROW_RARE_ITEM) {
                    highlighted = 1;
                } else {
                    highlighted = 0;
                }
            }
            if (highlighted == 1 && segment->rows[i].emphasisDirection == 0) {
                segment->rows[i].emphasisDirection = 4;
            }
            segment->rows[i].emphasis += segment->rows[i].emphasisDirection;
            if (segment->rows[i].emphasis >= MENU_SEGMENT_PULSE_MAX) {
                segment->rows[i].emphasis = MENU_SEGMENT_PULSE_MAX;
                segment->rows[i].emphasisDirection = -segment->rows[i].emphasisDirection;
            }
            if (segment->rows[i].emphasis == 0) {
                segment->rows[i].emphasis = 0;
                segment->rows[i].emphasisDirection = -segment->rows[i].emphasisDirection;
            }
            if (highlighted == 0) {
                if (segment->rows[i].emphasisDirection >= 0) {
                    segment->rows[i].emphasisDirection = -segment->rows[i].emphasisDirection;
                }
                if (segment->rows[i].emphasis == 0) {
                    segment->rows[i].emphasisDirection = 0;
                }
            }
        }
        segment->rows[i].sprites[4].x = segment->rows[i].sprites[0].x + 2;
        segment->rows[i].sprites[4].style = segment->rows[i].sprites[0].style + 1;
        segment->rows[i].sprites[4].y = segment->rows[i].sprites[0].y + 2;
        segment->rows[i].sprites[4].alpha = segment->rows[i].emphasis;
        eSpriteMain(&segment->rows[i].sprites[4]);
    }
    {
        int outline;

        for (outline = 0; outline < 1; outline++) {
            int border;

            segment->outlines[outline].pulse += segment->outlines[outline].pulseDirection;
            if (segment->outlines[outline].pulse == MENU_SEGMENT_PULSE_MAX || segment->outlines[outline].pulse == 32) {
                segment->outlines[outline].pulseDirection = -segment->outlines[outline].pulseDirection;
            }
            for (border = 0; border < 2; border++) {
                int point;

                for (point = 0; point < 5; point++) {
                    segment->outlines[outline].points[border * 6 + point].red = -2 - border * 64;
                    segment->outlines[outline].points[border * 6 + point].green = -88 - border * 64;
                    segment->outlines[outline].points[border * 6 + point].blue = 1;
                    segment->outlines[outline].points[border * 6 + point].alpha = segment->outlines[outline].pulse;
                }
                {
                    segment->outlines[outline].points[border * 6].x =
                        segment->outlines[outline].points[border * 6 + 3].x =
                        segment->outlines[outline].points[border * 6 + 4].x = segment->rows[segment->selected].sprites[0].x - border;
                    segment->outlines[outline].points[border * 6 + 1].x =
                        segment->outlines[outline].points[border * 6 + 2].x = segment->rows[segment->selected].sprites[0].x + border + 71;
                    segment->outlines[outline].points[border * 6].y =
                        segment->outlines[outline].points[border * 6 + 1].y =
                        segment->outlines[outline].points[border * 6 + 4].y = segment->rows[segment->selected].sprites[0].y - border;
                    segment->outlines[outline].points[border * 6 + 2].y =
                        segment->outlines[outline].points[border * 6 + 3].y = segment->rows[segment->selected].sprites[0].y + border + 57;
                    segment->outlines[outline].points[border * 6].work =
                        segment->outlines[outline].points[border * 6 + 1].work =
                        segment->outlines[outline].points[border * 6 + 2].work =
                        segment->outlines[outline].points[border * 6 + 3].work =
                        segment->outlines[outline].points[border * 6 + 4].work = segment->rows[segment->selected].sprites[0].style + 2;
                    segment->outlines[outline].points[border * 6 + 5].work = 0;
                    endPrintExtFunc(0, END_PRINT_CMD_LINE, &segment->outlines[outline].points[border * 6]);
                }
            }
        }
    }
    segment->ribbon.x = segment->x + 258;
    segment->ribbon.y = segment->y + 136;
    segment->ribbon.work = segment->work;
    segment->ribbon.width = 208;
    segment->ribbon.height = 208;
    eRibbonMain((ERibbonMainData *)&segment->ribbon);
    segment->mapSprites[0].x = segment->ribbon.x + segment->ribbon.width / 2 - 44;
    segment->mapSprites[0].y = segment->ribbon.y + segment->ribbon.height / 2 - 84;
    segment->mapSprites[0].style = segment->work;
    eSpriteMain(&segment->mapSprites[0]);
    {
        int partOffset;
        short partX[6] = {30, 24, 72, 4, 49, 23};
        short partY[6] = {4, 39, 39, 39, 99, 99};
        short labelX[6] = {64, -49, 96, -53, 72, -33};
        short labelY[6] = {-10, 4, 39, 70, 107, 138};
        MenuItemSegmentTriangleOffsets triangleOffsets[6] = {
            {{45, 61, 109}, {19, 3, 3}},
            {{43, -5, -53}, {64, 18, 18}},
            {{78, 96, 144}, {68, 52, 52}},
            {{10, -6, -54}, {68, 84, 84}},
            {{55, 71, 119}, {136, 120, 120}},
            {{29, 13, -35}, {136, 152, 152}}
        };

        /* Owned robo parts: each draws its sprite, selection mark, triangle and label. */
        for (i = 0; i < 6; i++) {
            if ((aligned->ownedParts >> i) & 1) {
                aligned->mapSprites[i + 1].x = aligned->mapSprites[0].x + partX[i];
                aligned->mapSprites[i + 1].style = aligned->mapSprites[0].style + 1;
                aligned->mapSprites[i + 1].y = aligned->mapSprites[0].y + partY[i];
                eSpriteMain(&aligned->mapSprites[i + 1]);
                if ((aligned->selectedParts >> i) & 1) {
                    if (i == 0) {
                        aligned->mapSprites[i + 13].x = aligned->mapSprites[i + 1].x - 3;
                        aligned->mapSprites[i + 13].y = aligned->mapSprites[i + 1].y - 3;
                    } else {
                        aligned->mapSprites[i + 13].x = aligned->mapSprites[i + 1].x - 2;
                        aligned->mapSprites[i + 13].y = aligned->mapSprites[i + 1].y - 2;
                    }
                    aligned->mapSprites[i + 13].style = aligned->mapSprites[i + 1].style + 1;
                    eSpriteMain(&aligned->mapSprites[i + 13]);
                }
                {
                    int last;
                    int point;

                    for (point = 0; point < 3; point++) {
                        if (point == 2) {
                            aligned->vertices[i * 4 + point].red = -36;
                            aligned->vertices[i * 4 + point].green = -66;
                            aligned->vertices[i * 4 + point].blue = -1;
                        } else {
                            aligned->vertices[i * 4 + point].red = 75;
                            aligned->vertices[i * 4 + point].green = -46;
                            aligned->vertices[i * 4 + point].blue = -1;
                        }
                        aligned->vertices[i * 4 + point].alpha = -128;
                    }
                    for (point = 0; point < 3; point++) {
                        aligned->vertices[i * 4 + point].x = aligned->mapSprites[0].x + triangleOffsets[i].x[point];
                        aligned->vertices[i * 4 + point].y = aligned->mapSprites[0].y + triangleOffsets[i].y[point];
                        aligned->vertices[i * 4 + point].work = aligned->mapSprites[0].style + 2;
                    }
                    last = i * 4 + point;
                    aligned->vertices[last].work = 0;
                    endPrintExtFunc(0, END_PRINT_CMD_LINE, &aligned->vertices[i * 4]);
                }
                aligned->mapSprites[i + 7].x = aligned->mapSprites[0].x + labelX[i];
                aligned->mapSprites[i + 7].style = aligned->mapSprites[0].style + 2;
                aligned->mapSprites[i + 7].y = aligned->mapSprites[0].y + labelY[i];
                eSpriteMain(&aligned->mapSprites[i + 7]);
            }
            /* Dead, but its read after the loop is what gives the original's partX/partY index copy. */
            partOffset = i * 2;
        }
        i = partOffset;
    }
    for (i = 0; i < 2; i++) {
        segment->labels[i].x = segment->windows[i].x + 3;
        segment->labels[i].style = segment->windows[i].style;
        segment->labels[i].y = segment->windows[i].y + 3;
        eSpriteMain(&segment->labels[i]);
    }
    endPrintExtFunc(0, END_PRINT_CMD_FONT_TEX_RELOAD, 0);
    for (i = 0; i < MENU_SEGMENT_COUNT; i++) {

        aligned->rows[i].number.x = segment->rows[i].sprites[0].x + 15;
        aligned->rows[i].number.work = segment->rows[i].sprites[0].style + 1;
        aligned->rows[i].number.y = segment->rows[i].sprites[0].y + 6;
        eNumberMain((ENumberMainData *)&aligned->rows[i].number);
    }
}

/*
 * Runs one frame of the item menu. The first call carves the widget work areas from
 * the shared buffer. Returns the picked item in pick mode, otherwise 0.
 */
int MenuItem(void)
{
    MenuItemWorkData *menu;
    MenuItemUseData *item;
    int characters[8];
    int i;
    int memberIndex;

    if (MenuWork.mode == MENU_ITEM_MODE_INIT) {
        MenuItemArena *arena = MainMenuWorkEnd;
        void *cursor = MainMenuWorkEnd;
        /* Each widget is carved in turn, with the padding the original layout has after it. */
        void *allocation;
        MenuItemPasWork *pas;
        MenuItemInfoWork *info;
        MenuItemPartyStatusWork *status;
        MenuItemAgwsStatusWork *agws;
        MenuItemListWork *list;
        MenuItemIconWork *icon;
        MenuItemSelectWork *select;
        MenuItemExWork *ex;
        MenuItemLineWork *line;
        MenuItemSegmentWork *segment;
        int remaining = 7;
        unsigned char *savedSelection;

        allocation = cursor;
        pas = allocation;
        MenuItemPas = pas;
        cursor += sizeof(arena->pas) + sizeof(arena->unmodeled_0414);
        pas->state = MENU_ITEM_STATE_INIT;
        allocation = cursor;
        info = allocation;
        MenuItemInfo = info;
        cursor += sizeof(arena->info);
        info->state = MENU_ITEM_STATE_INIT;
        allocation = cursor;
        status = allocation;
        MenuItemStatus = status;
        cursor += sizeof(arena->status) + sizeof(arena->unmodeled_57a4);
        status->state = MENU_ITEM_STATE_INIT;
        allocation = cursor;
        agws = allocation;
        MenuItemStatusAgws = agws;
        cursor += sizeof(arena->agws);
        agws->state = MENU_ITEM_STATE_INIT;
        allocation = cursor;
        list = allocation;
        MenuItemList = list;
        cursor += sizeof(arena->list);
        list->state = MENU_ITEM_STATE_INIT;
        allocation = cursor;
        icon = allocation;
        MenuItemIcon = icon;
        cursor += sizeof(arena->icon) + sizeof(arena->unmodeled_7ff4);
        icon->state = MENU_ITEM_STATE_INIT;
        allocation = cursor;
        select = allocation;
        MenuItemSelect = select;
        cursor += sizeof(arena->select) + sizeof(arena->unmodeled_8f70);
        select->state = MENU_ITEM_STATE_INIT;
        allocation = cursor;
        ex = allocation;
        MenuItemEx = ex;
        cursor += sizeof(arena->ex) + sizeof(arena->unmodeled_8fc8);
        ex->state = MENU_ITEM_STATE_INIT;
        allocation = cursor;
        line = allocation;
        MenuItemLine = line;
        cursor += sizeof(arena->line);
        line->state = MENU_ITEM_STATE_INIT;
        MenuItemSegment = cursor;
        segment = cursor;
        cursor += sizeof(arena->segment);
        segment->state = MENU_ITEM_STATE_INIT;
        MenuWorkEndCheck(segment + 1);

        MenuWork.nextMode = MENU_ITEM_MODE_LIST;
        MenuWork.busy = MENU_ITEM_BUSY_NORMAL;
        MenuWork.partyCount = 0;
        MenuWork.flags = 0;
        savedSelection = MenuKeepSelect;
        do {
            unsigned char *entry = savedSelection;

            savedSelection += MENU_ITEM_KEEP_SELECT_SIZE;
            remaining--;
            memset(entry, 0, MENU_ITEM_KEEP_SELECT_SIZE);
        } while (remaining >= 0);
        for (i = 0; i < MENU_ITEM_TAB_COUNT; i++) {
            unsigned char choice = MenuSaveDataGet()[i];

            MenuWork.sortChoices[i] = choice;
        }
        MenuWork.partyCount = PartyAllPartyGet(characters, 0);
        for (memberIndex = 0; memberIndex < MenuWork.partyCount; memberIndex++) {
            MenuItemParty[memberIndex] = (unsigned short)characters[memberIndex];
        }
        if (MenuScenarioNo - MENU_SCENARIO_AGWS_FIXED_FIRST < MENU_SCENARIO_AGWS_FIXED_COUNT) {
            MenuWork.agwsCount = 3;
            MenuItemAgws[0] = MENU_CHAR_VX_7000;
            MenuItemAgws[1] = MENU_CHAR_VX_9000;
            MenuItemAgws[2] = MENU_CHAR_VX_20000;
        } else {
            int agwsCount = PartyAgwsGet(characters);

            MenuWork.agwsCount = agwsCount;
            for (memberIndex = 0; memberIndex < (signed char)agwsCount; memberIndex++) {
                MenuItemAgws[memberIndex] = (unsigned short)characters[memberIndex];
            }
        }
    }

    if (MenuWork.mode != MenuWork.nextMode) {
        MenuWork.mode = MenuWork.nextMode;
        MenuWork.flags |= MENU_ITEM_FLAG_MODE_ENTERED;
    } else {
        MenuWork.flags &= 0xFE;
    }
    menu = &MenuWork;
    if (MenuWork.busy != 0) {
        MenuWork.busy--;
    }

    /* Input for the current mode. */
    switch (MenuWork.mode) {
    case MENU_ITEM_MODE_PICK:
        return menu->selectedItem;

    case MENU_ITEM_MODE_LIST:
        if ((menu->flags & MENU_ITEM_FLAG_MODE_ENTERED) == 0) {
            if (menu->busy == 0) {
                menu->selectDir = WindowSPSelect(&MenuItemList->window, PadData.activity);
                WindowSPKeepSelect(&MenuItemList->window, &MenuKeepSelect[menu->listTab * MENU_ITEM_KEEP_SELECT_SIZE]);
                menu->tabChangeDirection = 0;
                if (PadData.activity == PAD_L1) {
                    menu->tabChangeDirection = -1;
                }
                if (PadData.activity == PAD_R1) {
                    menu->tabChangeDirection = 1;
                }
                if (menu->tabChangeDirection != 0) {
                    WindowSPKeepSelectCheck(&MenuKeepSelect[menu->listTab * MENU_ITEM_KEEP_SELECT_SIZE]);
                    menu->listTab += menu->tabChangeDirection;
                    if (menu->listTab < 0) {
                        menu->listTab = MENU_ITEM_TAB_COUNT - 1;
                    }
                    if (menu->listTab >= MENU_ITEM_TAB_COUNT) {
                        menu->listTab = 0;
                    }
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                    menu->busy = MENU_ITEM_BUSY_NORMAL;
                }
                if (PadData.activity == PAD_TRIANGLE) {
                    if (MenuWork.selectDir >= 0) {
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                        MenuWork.nextMode = MENU_ITEM_MODE_SORT;
                    } else {
                        xglSoundEffectNormalID(SE_BUZZER, 0);
                    }
                }
                if (MenuWork.selectDir >= 0) {
                    MenuWork.selectedItem = MenuSortGet(0, MenuWork.selectDir);
                    MenuWork.selectionMode = MenuItemUseCheck(MenuWork.selectedItem);
                    if (PadData.pressed == PAD_CIRCLE) {
                        switch (MenuWork.listTab) {
                        case MENU_ITEM_TAB_ITEMS:
                            switch (MenuWork.selectionMode) {
                            case MENU_ITEM_USE_SINGLE:
                            case MENU_ITEM_USE_ALL:
                                xglSoundEffectNormalID(SE_DECIDE, 0);
                                MenuWork.nextMode = MENU_ITEM_MODE_TARGET;
                                MenuWork.agwsMode = 0;
                                break;
                            case MENU_ITEM_USE_AGWS:
                                if (MenuWork.agwsCount != 0) {
                                    xglSoundEffectNormalID(SE_DECIDE, 0);
                                    MenuWork.nextMode = MENU_ITEM_MODE_TARGET;
                                    MenuWork.agwsMode = 1;
                                    MenuWork.busy = MENU_ITEM_BUSY_SHORT;
                                } else {
                                    xglSoundEffectNormalID(SE_BUZZER, 0);
                                }
                                break;
                            default:
                                xglSoundEffectNormalID(SE_BUZZER, 0);
                                break;
                            }
                            break;
                        case MENU_ITEM_TAB_EVENT:
                            if ((unsigned int)((unsigned short)MenuWork.selectedItem - MENU_EVENT_ITEM_SEGMENT_MAP) < MENU_EVENT_ITEM_SELECTABLE_COUNT) {
                                if (MenuWork.selectedItem == MENU_EVENT_ITEM_SEGMENT_MAP) {
                                    MenuWork.nextMode = MENU_ITEM_MODE_SEGMENT;
                                    xglSoundEffectNormalID(SE_DECIDE, 0);
                                } else if ((unsigned int)GameLoopState.flags & MENU_ITEM_GAME_FLAG_PICK) {
                                    MenuWork.nextMode = MENU_ITEM_MODE_PICK;
                                    xglSoundEffectNormalID(SE_DECIDE, 0);
                                } else {
                                    xglSoundEffectNormalID(SE_BUZZER, 0);
                                }
                            }
                            break;
                        }
                    }
                }
                if (PadData.pressed & PAD_CROSS) {
                    xglSoundEffectNormalID(SE_CANCEL, 0);
                    MenuWork.nextMode = MENU_ITEM_MODE_CLOSE;
                }
            } else {
                menu->selectDir = WindowSPSelect(&MenuItemList->window, 0);
            }
        }
        break;

    case MENU_ITEM_MODE_TARGET:
        if (menu->busy == 0) {
            if (menu->agwsMode != 0) {
                int selection = MenuSelectMove(menu->selectedAgws, menu->agwsCount, 0);
                unsigned short pressed = PadData.pressed;

                menu->selectedAgws = selection;
                if (pressed == PAD_CIRCLE) {
                    int itemKey = MenuSortGet(0, menu->selectDir);
                    short itemId = itemKey;

                    menu->flags |= MENU_ITEM_FLAG_MODIFIED;
                    func_A1A4E8(itemId);
                    if (MenuCharHpCheck(MenuItemAgws[menu->selectedAgws]) != 0) {
                        xglSoundEffectNormalID(SE_BUZZER, 0);
                        break;
                    }
                    MenuBoxDec(itemKey);
                    if (MenuBoxChk(itemKey) == 0) {
                        WindowSPKeepSelect(&MenuItemList->window, &MenuKeepSelect[menu->listTab * MENU_ITEM_KEEP_SELECT_SIZE]);
                        MenuItemListMake00();
                        menu->nextMode = pressed;
                    }
                    func_00A114C0(MenuItemAgws[menu->selectedAgws], itemId);
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                    MenuItemListMake00_1();
                }
                if (PadData.pressed == PAD_CROSS) {
                    MenuItemListMake00();
                    xglSoundEffectNormalID(SE_CANCEL, 0);
                    MenuWork.nextMode = MENU_ITEM_MODE_LIST;
                }
            } else if (menu->scrollDir == 0) {
                signed char selectionMode = menu->selectionMode;

                if (selectionMode == MENU_ITEM_USE_SINGLE && menu->partyCount >= 2) {
                    if (PadData.activity == PAD_UP) {
                        menu->scrollDir = -1;
                    }
                    if (PadData.activity == PAD_DOWN) {
                        menu->scrollDir = selectionMode;
                    }
                    if (menu->scrollDir != 0) {
                        if (menu->scrollDir == -1) {
                            if (menu->selectedParty != 0) {
                                menu->selectedParty--;
                                xglSoundEffectNormalID(SE_CURSOR, 0);
                            }
                        } else {
                            if (menu->selectedParty != menu->partyCount - 1) {
                                menu->selectedParty++;
                                xglSoundEffectNormalID(SE_CURSOR, 0);
                            }
                        }
                    }
                }
                if (PadData.pressed == PAD_CROSS) {
                    MenuItemListMake00();
                    xglSoundEffectNormalID(SE_CANCEL, 0);
                    MenuWork.nextMode = MENU_ITEM_MODE_LIST;
                }
                if (PadData.pressed == PAD_CIRCLE) {
                    int itemKey = MenuSortGet(0, MenuWork.selectDir);
                    short itemId = itemKey;

                    MenuWork.flags |= MENU_ITEM_FLAG_MODIFIED;
                    item = func_A1A4E8(itemId);
                    if (itemId == MENU_ITEM_SPECIAL_ID) {
                        if (MenuCharHpCheck(MENU_CHAR_WHOLE_PARTY) != 0 && MenuCharEpCheck(MENU_CHAR_WHOLE_PARTY) != 0) {
                            xglSoundEffectNormalID(SE_BUZZER, 0);
                            break;
                        }
                    } else {
                        unsigned short attributes;
                        MenuItemCharacterPoints *points = func_A19210(MenuItemParty[MenuWork.selectedParty]);

                        attributes = item->attributes;
                        if (attributes & MENU_ITEM_ATTR_HEAL_HP) {
                            if (MenuWork.selectionMode == MENU_ITEM_USE_SINGLE) {
                                if (MenuCharHpCheck(MenuItemParty[MenuWork.selectedParty]) != 0) {
                                    xglSoundEffectNormalID(SE_BUZZER, 0);
                                    break;
                                }
                            } else {
                                if (MenuCharHpCheck(MENU_CHAR_WHOLE_PARTY) != 0) {
                                    xglSoundEffectNormalID(SE_BUZZER, 0);
                                    break;
                                }
                            }
                        } else if (attributes & MENU_ITEM_ATTR_HEAL_EP) {
                            if (MenuWork.selectionMode == MENU_ITEM_USE_SINGLE) {
                                if (MenuCharEpCheck(MenuItemParty[MenuWork.selectedParty]) != 0) {
                                    xglSoundEffectNormalID(SE_BUZZER, 0);
                                    break;
                                }
                            } else {
                                if (MenuCharEpCheck(MENU_CHAR_WHOLE_PARTY) != 0) {
                                    xglSoundEffectNormalID(SE_BUZZER, 0);
                                    break;
                                }
                            }
                        } else if (attributes & MENU_ITEM_ATTR_POINTS) {
                            if (points->points >= MENU_ITEM_POINTS_MAX) {
                                xglSoundEffectNormalID(SE_BUZZER, 0);
                                break;
                            }
                        } else if (attributes & MENU_ITEM_ATTR_ETHER_POINTS) {
                            if (points->etherPoints >= MENU_ITEM_POINTS_MAX) {
                                xglSoundEffectNormalID(SE_BUZZER, 0);
                                break;
                            }
                        } else if (attributes & MENU_ITEM_ATTR_SKILL_POINTS) {
                            if (points->skillPoints >= MENU_ITEM_POINTS_MAX) {
                                xglSoundEffectNormalID(SE_BUZZER, 0);
                                break;
                            }
                        }
                    }
                    MenuBoxDec(itemKey);
                    if (MenuBoxChk(itemKey) == 0) {
                        WindowSPKeepSelect(&MenuItemList->window, &MenuKeepSelect[MenuWork.listTab * MENU_ITEM_KEEP_SELECT_SIZE]);
                        MenuItemListMake00();
                        MenuWork.nextMode = MENU_ITEM_MODE_LIST;
                    }
                    func_00A114C0(MenuItemParty[MenuWork.selectedParty], itemId);
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                    MenuItemListMake00_1();
                }
            }
        }
        break;

    case MENU_ITEM_MODE_SEGMENT:
        if (menu->flags & MENU_ITEM_FLAG_MODE_ENTERED) {
            menu->segmentSelection = 0;
            menu->busy = MENU_ITEM_BUSY_NORMAL;
        }
        if (menu->busy == 0 && (PadData.pressed & PAD_CROSS)) {
            xglSoundEffectNormalID(SE_CANCEL, 0);
            menu->nextMode = MENU_ITEM_MODE_LIST;
            menu->busy = MENU_ITEM_BUSY_NORMAL;
        }
        break;

    case MENU_ITEM_MODE_SORT: {
        if (MenuWork.flags & MENU_ITEM_FLAG_MODE_ENTERED) {
            signed char initialTab;
            unsigned char initialChoice;

            initialTab = MenuWork.listTab;
            initialChoice = menu->sortChoices[initialTab];
            MenuWork.busy = MENU_ITEM_BUSY_SHORT;
            MenuWork.sortSelection = (signed char)initialChoice / 2;
        }
        if (MenuWork.busy == 0) {
            int selection = MenuSelectMove(MenuWork.sortSelection, MENU_ITEM_SORT_CHOICE_COUNT, 0);
            int pressed = PadData.pressed;

            MenuWork.sortSelection = selection;
            if (pressed == PAD_CIRCLE) {
                if (MenuWork.sortSelection != MENU_ITEM_SORT_CANCEL) {
                    unsigned char *saved;

                    *(menu->sortChoices + menu->listTab) = MenuWork.sortSelection * 2;
                    saved = MenuSaveDataGet();
                    saved[menu->listTab] = menu->sortChoices[menu->listTab];
                    MenuWork.flags |= MENU_ITEM_FLAG_MODIFIED;
                    xglSoundEffectNormalID(SE_SORT_SET, 0);
                } else {
                    MenuWork.nextMode = pressed;
                    xglSoundEffectNormalID(SE_CANCEL, 0);
                }
            }
            if (PadData.pressed == PAD_CROSS) {
                MenuWork.nextMode = MENU_ITEM_MODE_LIST;
                MenuWork.busy = MENU_ITEM_BUSY_NORMAL;
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
        }
        break;
    }

    case MENU_ITEM_MODE_CLOSE:
        if (menu->flags & MENU_ITEM_FLAG_MODE_ENTERED) {
            menu->busy = MENU_ITEM_BUSY_CLOSE;
        }
        if (menu->busy == 0) {
            menu->nextMode = MENU_ITEM_MODE_DONE;
        }
        break;

    case MENU_ITEM_MODE_DONE:
        ChangeTopLevel(0);
        break;
    }

    /* Update every widget for this frame. */
    MenuItemPasMain();
    MenuItemInfoMain();
    MenuItemStatusMain();
    MenuItemStatusAgwsMain();
    MenuItemListMain();
    MenuItemIconMain();
    MenuItemSelectMain();
    MenuItemExMain();
    MenuItemLineMain();
    MenuItemSegmentMain();
    xglFontDebugPrintf(0, 0x40, debugItemLabel);
    xglFontDebugPrintf(0, 0x48, listIndexFormat, MenuWork.selectDir);
    xglFontDebugPrintf(0, 0x50, listIndexFormat, MenuWork.selectionMode);
    return 0;
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
static char msg00_1_0036D698[10] = "List\003\215Num";

static const char * msg00_2_0036D6A8[] = { sortModeChoicesText };

static const char * msg10_3[] = { sortText };

static const char * tag_msg_5[] = { locationTagText, rareItemTagText };

const char sortModeChoicesText[40] = "Categorical\nNumerical\nAlphabetical\nBack";

const char rareItemTagText[16] = "Rare item";

const char locationTagText[16] = "Location";

const float menuItemPi = 3.1415927410125732f;

const float iconAngleStep = 0.44879895448684692f;

const unsigned char emptyText[8] = "";

const char menuItemPeriodText[8] = ".";

/* Text: \x01FHP */
const char fhpText[8] = "\001FHP";

const char sortText[8] = "Sort";

/* Text: Temporary */
const char temporaryText[8] = "\262\276";

const char debugItemLabel[8] = "item";

const char listIndexFormat[8] = "lst%2d";

MenuItemPasWork * MenuItemPas = 0;

void * MenuItemInfo = 0;

void * MenuItemLine = 0;

void * MenuItemStatus = 0;

void * MenuItemStatusAgws = 0;

MenuItemListWork * MenuItemList = 0;

void * MenuItemSelect = 0;

void * MenuItemIcon = 0;

MenuItemExWork * MenuItemEx = 0;

void * MenuItemSegment = 0;

const char MenuDigit0Text[8] = "0";

const char MenuDigit1Text[8] = "1";

const char MenuDigit2Text[8] = "2";

const char MenuDigit3Text[8] = "3";

const char MenuDigit4Text[8] = "4";

const char MenuDigit5Text[8] = "5";

const char MenuDigit6Text[8] = "6";

const char MenuDigit7Text[8] = "7";

const char MenuDigit8Text[8] = "8";

const char MenuDigit9Text[8] = "9";

/* MenuItemExMain writes the color bytes of this text at run time. */
static char * msg00_4_0036D6B8[] = { (char *)sortHeaderText };

const MenuItemInfoLabels infoLabels = { { MenuItemInfoLabelsText + 16, MenuItemInfoLabelsText + 40, MenuItemInfoLabelsText + 56 }, MenuItemInfoLabelsText + 80, MenuItemCancelText };

const char itemCannotBeUsedHereText[32] = "This item cannot be used here.";

const char itemSavePointOnlyText[48] = "This item can only be used at save points.";

const MenuItemSpriteIds iconSpriteTable = { { 0x0112, 0x0110, 0x0322, 0x0323, 0x0324, 0x0325, 0x0328, 0x0326, 0x0327, 0x0329 } };

/* Text: a control-code prefix, then Sort */
const char sortHeaderText[24] = "\014\200\200\200\036\002\241\247Sort";

MenuItemInfoHelpLabels infoHelpTable[1] = { { itemCannotBeUsedHereText, itemSavePointOnlyText } };
