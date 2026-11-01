#include "common.h"
#include "main/open_close_main.h"
#include "shared.h"
#include "main/menu_1.h"
#include "main/e_battle_win_open.h"
#include "main/menu_top_command_check.h"

#define MENU_ITEM_SPECIAL_ID 36
#define MENU_ITEM_USE_ALLOWED 0x8000
#define MENU_ITEM_USE_CATEGORY_BITS 0x1e43
#define MENU_ITEM_USE_SPECIAL_BIT 0x2000
#define MENU_ITEM_USE_STATE_BIT 0x10
#define MENU_ITEM_SPECIAL_ENABLED_BITS 0x20400000

typedef struct MenuItemPasWindow {
    short x;
    short y;
    int color;
    short width;
    short height;
    const char *tag_id;
    signed char state;
    u8 ribbon_initialized; /* +0x11 */
    u8 unmodeled_12[0x14 - 0x12];
    void (*callback)(WindowDX *window, void *work);
    void *callbackArg;
    u8 unmodeled_1c[0x194 - 0x1C];
} MenuItemPasWindow;

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
    MenuItemPasWindow window;
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

typedef struct MenuItemListRow {
    u8 unmodeled_00[8];
    u8 flag;
    u8 state;
    u8 unmodeled_0a[2];
} MenuItemListRow;

typedef struct MenuItemWindowSP {
    u8 state;
    u8 rowCount;
    u8 unmodeled_02[2];
    short x;
    short y;
    int color;
    short width;
    short height;
    const char *title;
    signed char columns;
    signed char rows;
    u8 unmodeled_16[6];
    struct MenuItemListRow *items;
} MenuItemWindowSP;

typedef struct MenuItemListWork {
    u8 state;
    u8 visible;
    u8 unmodeled_02;
    u8 motion;
    int activeTab;
    int color;
    MenuItemWindowSP window;
} MenuItemListWork;

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

typedef struct MenuItemPasMessage {
    u8 unmodeled_00;
    u8 mode;
    u8 unmodeled_02[2];
    short x;
    short y;
    int color;
    u8 unmodeled_0c[0x44 - 0x0C];
} MenuItemPasMessage;

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
    MenuItemPasWindow window;
    MenuItemPasMessage message[9];
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
    u8 unmodeled_71; /* +0x65 */
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

typedef struct MenuItemStatusCursor {
    u8 state;
    u8 unmodeled_01[3];
    short x;
    short y;
    int work;
    u8 unmodeled_0c[0x24 - 0x0C];
} MenuItemStatusCursor;

typedef struct MenuItemPartyStatusWork {
    u8 state;
    unsigned char visibleCount;
    signed char scrollStart;
    u8 scrollTick;
    u8 selectionRow;
    u8 unmodeled_05[3];
    int style;
    MenuItemPasWindow windows[8];
    MenuItemStatusContext contexts[8];
    MenuItemStatusCursor cursors[3];
    /* Two backgrounds, two outlines, then the clipping rectangle. */
    MenuItemPasBox boxes[5];
} MenuItemPartyStatusWork;

typedef struct MenuItemSpriteIds {
    short ids[10];
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
    signed char displacement[10];
    int style;
    MenuItemIconSprite sprites[10];
} MenuItemIconWork;

typedef struct MenuItemSelectContent {
    u8 unmodeled_00[2];
    unsigned short mode;
    int selection;
    const char *text;
    u8 unmodeled_0c[0xBF0 - 0x0C];
} MenuItemSelectContent;

typedef struct MenuItemSelectWindow MenuItemSelectWindow;

struct MenuItemSelectWindow {
    short x;
    short y;
    int color;
    short width;
    short height;
    const char *title;
    signed char state;
    u8 ribbonInitialized;
    u8 unmodeled_12[2];
    void (*callback)(MenuItemSelectWindow *, struct MenuItemSelectContent *);
    struct MenuItemSelectContent *callbackArg;
    u8 unmodeled_1c[0x194 - 0x1C];
};

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
    MenuItemSelectWindow window[1];
    u8 unmodeled_19c[0x330 - 0x19C];
    MenuItemSelectContent content;
    MenuItemSelectSprite sprite;
} MenuItemSelectWork;

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

typedef struct MenuItemSegmentMessage {
    u8 unmodeled_00;
    u8 mode;
    u8 unmodeled_02[2];
    short x;
    short y;
    int work;
    u8 unmodeled_0c[12];
    const char *text;
    u8 unmodeled_1c[68 - 28];
} MenuItemSegmentMessage;

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
    MenuItemSegmentPoint render[6][4];
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
    MenuItemPasWindow windows[2];
    MenuItemIconSprite labels[2];
    MenuItemSegmentMessage text[2];
    MenuItemSegmentRow rows[18];
    u8 unmodeled_1c88[4];
    unsigned short ownedParts;
    unsigned short selectedParts;
    MenuItemIconSprite mapSprites[19];
    MenuItemSegmentRibbon ribbon;
    MenuItemSegmentOutline outlines[2];
    int texture;
} MenuItemSegmentWork;

typedef struct MenuItemAgwsCursor {
    u8 mode;
    u8 unmodeled_01[3];
    short x;
    short y;
    int work;
} MenuItemAgwsCursor;

typedef struct MenuItemAgwsNumber {
    unsigned short x;
    unsigned short y;
    int work;
    u8 unmodeled_08[6];
    u8 format;
    u8 digitCount;
    u8 unmodeled_10[4];
    int value;
    u8 unmodeled_18[0x90 - 0x18];
} MenuItemAgwsNumber;

typedef struct MenuItemAgwsTag {
    u8 unmodeled_00[4];
    short x;
    short y;
    int work;
    signed char color[4];
    struct {
        short x;
        short y;
        int work;
        signed char color[4];
        const char *text;
    } render;
} MenuItemAgwsTag;

typedef struct MenuItemAgwsRow {
    MenuItemPasWindow window;
    MenuItemPasMessage name;
    MenuItemAgwsTag hpLabel;
    MenuItemAgwsNumber hpNumber;
} MenuItemAgwsRow;

typedef struct MenuItemAgwsStatusWork {
    u8 state;
    u8 unmodeled_01[3];
    int color;
    MenuItemAgwsRow rows[6];
    MenuItemAgwsCursor cursor;
} MenuItemAgwsStatusWork;

typedef struct MenuItemArena {
    MenuItemPasWork pas;
    u8 unmodeled_0414[0x480 - 0x414];
    MenuItemInfoWork info;
    MenuItemPartyStatusWork status;
    u8 unmodeled_57a4[0x57B0 - 0x57A4];
    MenuItemAgwsStatusWork agws;
    u8 unmodeled_66f4[0x670C - 0x66F4];
    MenuItemListWork list;
    u8 unmodeled_6738[0x7E50 - 0x6738];
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
    MenuItemPasWindow windows[2];
    MenuItemIconSprite labels[2];
    MenuItemSegmentMessage text[2];
    MenuItemSegmentRow rows[18];
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

/* Original storage of this TU without a C reader; typed from its original bytes. */
extern const char sortHeaderText[24];

extern const char MenuItemInfoLabelsText[];
extern const char MenuItemCancelText[];

extern MenuItemUseData *func_A1A4E8(s16 itemId);
extern void endPrintExtFunc(int color, int id, void *data);
extern int *MenuSortAddrGet(int list);
extern MenuItemListRow *MenuListGet(int list);
extern void MenuListMake(int list, int mode);
extern int MenuSortCheck(int list);
extern void MenuSortSet(int list, int type, int order);
extern void MenuSortChange(int list, int index);
extern void MoveSlide(short *current, short *target, float rate);
extern void eMessageSet(void *message, const char *text);
extern void eMessageMain(void *message);
extern int xglFlagsGet(int flag, int mode);
extern char *MenuTagTextGet(int index);
extern int PartyAttackerCheck(int characterId);
extern void WindowSPSet(MenuItemWindowSP *window);
extern void WindowSPMain(MenuItemWindowSP *window);
extern void eSpriteSet(void *sprite, short spriteId);
extern void eSpriteMain(void *sprite);
extern float xglCos(float angle);
extern int MenuSortGet(int list, int index);
extern void xglSoundEffectNormalID(int sound, int channel);
extern void MenuWorkEndCheck(void *end);
extern unsigned char *MenuSaveDataGet(void);
extern int PartyAllPartyGet(int *characters, int mode);
extern int PartyAgwsGet(int *characters);
extern int MenuSelectMove(int selection, int count, int wrap);
extern int MenuBoxDec(int itemId);
extern int MenuBoxChk(int itemId);
extern int MenuCharHpCheck(int character);
extern int MenuCharEpCheck(int character);
extern MenuItemCharacterPoints *func_A19210(int character);
extern int func_00A114C0(int character, int itemId);
extern void ChangeTopLevel(int level);
extern void xglFontDebugPrintf(int x, int y, const char *format, ...);
extern void MenuPasWindow(void);
extern char *strcpy(char *destination, const char *source);
extern char *strcat(char *destination, const char *source);
extern const char *MenuCharNameGet(int character);
extern void eTagFontSet(void *tag, const char *text);
extern void eTagFontMain(void *tag);
extern void eNumberSet(void *number, int mode);
extern void eNumberMain(void *number);
extern MenuItemHpParameters *func_00A11108(int character, int *attack, int *defense);
extern const char *MenuSegmentMapNameGet(int segment);
extern const char *MenuSegmentItemNameGet(int segment);
extern const char *MenuSegmentInfoTextGet(int segment);
extern char *dataEvtItmNameGet(int item);
extern int WindowTexAddrGet(int texture);
extern void eRibbonSet(void *ribbon, int mode);
extern void eRibbonMain(void *ribbon);

/* Text: List{x 653}Num */
const char listNumText[16] = "List\x03\x8d" "Num";

/* Text: List{x 665}Num{x 665}3Num */
const char listNum3NumText[32] = "List\x03\x99Num\x03\x99\x03" "3Num";

static short MenuItemParty[8];

static short MenuItemAgws[8];

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
static char msg00_1_0036D698[10];

int MenuItemUseCheck(s16 itemId)
{
    MenuItemUseData *item;
    u16 attributes;
    int result;
    /* A special item is unavailable when both game-loop bits are clear. */
    int specialUnavailable = -1;

    if (itemId == 0) {
        return 0;
    }
    item = func_A1A4E8(itemId);
    if ((item->use_flags & MENU_ITEM_USE_ALLOWED) == 0) {
        return 0;
    }
    if (itemId == MENU_ITEM_SPECIAL_ID) {
        result = (GameLoopState.flags & MENU_ITEM_SPECIAL_ENABLED_BITS) ? 2 : specialUnavailable;
    } else {
        attributes = item->attributes;
        if (attributes & MENU_ITEM_USE_CATEGORY_BITS) {
            result = (item->use_state & MENU_ITEM_USE_STATE_BIT) ? 2 : 1;
        } else {
            result = (attributes & MENU_ITEM_USE_SPECIAL_BIT) ? 10 : 0;
        }
    }
    return result;
}

void MenuItemPasMain(void)
{
    static const char *msg00[9] = {
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
    short tabTarget[9];
    short slideTarget;
    int i;

    switch (pas->state) {
    case 0:
        pas->x = -272;
        pas->y = 8;
        pas->color = 0xFFFFF0;
        WindowDXSet((WindowDX *)&pas->window);
        pas->window.x = pas->x;
        pas->window.y = pas->y;
        pas->window.color = pas->color;
        pas->window.width = 272;
        pas->window.height = 30;
        pas->window.callback = (void (*)(WindowDX *, void *))MenuPasWindow;
        pas->window.callbackArg = pas->callbackWork;
        pas->window.state = 1;
        WindowDXMain((WindowDX *)&pas->window);
        pas->window.state = 3;
        for (i = 0; i < 9; i++) {
            eMessageSet(&pas->message[i], msg00[i]);
            pas->message[i].mode = 32;
            pas->message[i].x = 288;
            pas->message[i].color = pas->color + 2;
            pas->message[i].y = pas->y + 3;
        }
        pas->state = 2;
        /* fallthrough */
    case 2:
        break;
    default:
        return;
    }

    slideTarget = -16;
    for (i = 0; i < 9; i++)
        tabTarget[i] = 288;

    switch (MenuWork.mode) {
    case 0x40:
    case 0x80:
    case 0x20:
    case 0x22:
        tabTarget[MenuWork.listTab] = 16;
        break;
    case 0x30:
        tabTarget[8] = 16;
        break;
    default:
        slideTarget = -272;
        break;
    }

    MoveSlide(&pas->window.x, &slideTarget, 3.0f);
    WindowDXMain((WindowDX *)&pas->window);
    pas->box.x = pas->window.x + 3;
    pas->box.y = pas->window.y + 3;
    pas->box.color = pas->color;
    pas->box.width = pas->window.width - 6;
    pas->box.height = pas->window.height - 6;
    endPrintExtFunc(pas->color, 0x65, &pas->box);
    for (i = 0; i < 9; i++) {
        MoveSlide(&pas->message[i].x, &tabTarget[i], 3.0f);
        if (pas->message[i].x < 256)
            eMessageMain(&pas->message[i]);
    }
    endPrintExtFunc(pas->color, 0x66, 0);
}

void MenuItemInfoMain(void)
{
    MenuItemInfoWork *info = MenuItemInfo;
    MenuItemInfoLabels labels = infoLabels;
    MenuItemInfoHelpLabels helpLabels = infoHelpTable[0];
    short targetY;
    short targetHeight;

    switch (info->state) {
    case 0: {
        signed char windowState;
        unsigned short *textX;

        info->color = 0xFFFFF0;
        WindowDXSet((WindowDX *)&info->window);
        info->window.x = -16;
        info->window.y = 480;
        info->window.width = 544;
        info->window.height = 54;
        info->window.color = info->color;
        info->window.callback = (void (*)(WindowDX *, void *))MenuInfoWindow;
        windowState = 1;
        info->window.callbackArg = &info->text;
        textX = info->window.callbackArg;
        *textX = 0;
        info->window.state = windowState;
        info->text.y = 0;
        info->text.message = 0;
        WindowDXMain((WindowDX *)&info->window);
        info->window.state = 3;
        info->help = 0;
        memset(info->buffer, 0, sizeof(info->buffer));
        info->state = 2;
    }
        /* fall through */
    case 2:
        break;
    default:
        return;
    }

    targetY = 384;
    targetHeight = 54;
    if (MenuWork.flags & 1) {
        info->help = 0;
    }
    if (info->help == 0) {
        if (PadData.pressed & 32) {
            info->help = 1;
        }
    } else if (PadData.activity != 0) {
        info->help = 0;
    }

    switch (MenuWork.mode) {
    case 32:
    case 34:
    case 64:
        if (MenuWork.busy == 0) {
            if (MenuWork.selectDir >= 0) {
                strcpy(info->buffer, (const char *)MenuTextGet(MenuSortGet(0, MenuWork.selectDir))->auxiliary);
                if (MenuWork.listTab == 0 && info->help == 1) {
                    signed char use = MenuListGet(0)[MenuWork.selectDir].state;

                    if (use == 1) {
                        strcpy(info->buffer, helpLabels.unavailable);
                    } else if (use == 2) {
                        strcpy(info->buffer, helpLabels.restricted);
                    }
                }
            } else {
                info->buffer[0] = emptyText[0];
            }
        }
        break;
    case 128:
        if (MenuWork.sortSelection == 3) {
            strcpy(info->buffer, labels.cancel);
        } else {
            strcpy(info->buffer, labels.sortPrefix);
            strcat(info->buffer, labels.sortMethods[MenuWork.sortSelection]);
            strcat(info->buffer, menuItemPeriodText);
        }
        break;
    case 48:
        targetY = 408;
        targetHeight = 30;
        break;
    default:
        targetY = 464;
        break;
    }

    info->text.message = info->buffer;
    MoveSlide(&info->window.y, &targetY, 3.0f);
    MoveSlide(&info->window.height, &targetHeight, 3.0f);
    WindowDXMain((WindowDX *)&info->window);
}

void MenuItemLineMain(void)
{
    MenuItemLineWork *line = MenuItemLine;
    int i;

    switch (line->state) {
    case 0:
        line->color = 0xFFFFF0;
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
        line->state = 2;
        line->alpha = 0;
    }
        /* fall through */
    case 2: {
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
    tabY = menu->listTab * 22;
    firstTargetY = tabY + 302;
    slide.targetY[0] = firstTargetY;
    slide.targetY[1] = tabY + 306;
    slide.targetY[2] = tabY + 310;
    slide.centerLength = 48;
    slide.targetY[3] = 512;
    if (firstTargetY == line->separators[1].x) {
        slide.centerLength = 40;
    }

    switch (menu->mode) {
    case 0x20:
    case 0x22:
    case 0x30:
    case 0x40:
    case 0x80:
        if (line->alpha != 120) {
            line->alpha += 8;
        }
        break;
    default:
        if (line->alpha != 0) {
            line->alpha -= 8;
        }
        break;
    }

    for (i = 1; i < 5; i++) {
        MoveSlide(&line->separators[i].x, &slide.targetY[i - 1], 3.0f);
        line->separators[i].alpha = line->alpha;
    }
    line->separators[0].alpha = line->alpha;
    MoveSlide(&line->separators[2].y, &slide.centerLength, 8.0f);
    endPrintExtFunc(line->color, 0, line->separators);
        break;
    }
    }
}

void MenuItemStatusMain(void)
{
    MenuItemPartyStatusWork *status = MenuItemStatus;
    short targetX[8];
    short targetY[8];
    int i;

    switch (status->state) {
    case 0: {
        status->style = 0xF000F0;
        i = 0;
        if (MenuWork.partyCount != 0) {
            short *party = MenuItemParty;

            do {
                WindowDXSet((WindowDX *)&status->windows[i]);
                status->windows[i].callback = (void (*)(WindowDX *, void *))MenuStatusDisp;
                status->windows[i].x = -272;
                status->windows[i].width = 240;
                status->windows[i].height = 82;
                status->windows[i].y = 64 + i * 104;
                status->windows[i].callbackArg = &status->contexts[i];
                status->windows[i].color = status->style;
                if (PartyAttackerCheck(*party)) {
                    status->windows[i].tag_id = MenuTagTextGet(1);
                } else {
                    status->windows[i].tag_id = MenuTagTextGet(2);
                }
                status->windows[i].ribbon_initialized = 1;
                status->contexts[i].characterId = *party;
                status->contexts[i].x = 0;
                status->contexts[i].y = 0;
                status->contexts[i].mode = 0;
                status->contexts[i].initialControls[0] = 0;
                status->contexts[i].initialControls[1] = 31;
                status->windows[i].state = 1;
                WindowDXMain((WindowDX *)&status->windows[i]);
                status->windows[i].state = 3;
                party++;
            } while (++i < MenuWork.partyCount);
        }
        for (i = 0; i < 3; i++) {
            eCursolSet((ECursol *)(void *)(&status->cursors[i]), i);
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
        if (status->visibleCount >= 4) {
            status->visibleCount = 3;
        }
        status->state = 2;
    }
        /* fall through */
    case 2:
        break;
    default:
        return;
    }

    for (i = 0; i < MenuWork.partyCount; i++) {
        targetX[i] = -272;
        targetY[i] = 64 + (i - status->scrollStart) * 104;
    }
    switch (MenuWork.mode) {
    case 32:
        if (MenuWork.flags & 1) {
            status->cursors[0].state = 0;
        }
        if (MenuWork.listTab == 0 && MenuWork.agwsMode == 0) {
            for (i = 0; i < MenuWork.partyCount; i++) {
                targetX[i] = 16;
            }
        }
        break;
    case 34:
        if (MenuWork.flags & 1) {
            status->cursors[0].state = 32;
        }
        if (MenuWork.agwsMode == 0) {
            for (i = 0; i < MenuWork.partyCount; i++) {
                targetX[i] = 16;
            }
            if (MenuWork.selectionMode == 1) {
                if (MenuWork.partyCount >= 4) {
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
                            if (status->scrollStart != MenuWork.partyCount - 3) {
                                status->scrollStart++;
                            }
                        } else {
                            status->scrollTick++;
                        }
                        MenuWork.scrollDir = 2;
                    }
                    for (i = 0; i < MenuWork.partyCount; i++) {
                        targetY[i] = 64 + (i - status->scrollStart) * 104;
                    }
                }
            } else {
                int rowCount = MenuWork.partyCount;

                if (rowCount >= 4) {
                    rowCount = 3;
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
        endPrintExtFunc(status->style, 12, &status->boxes[i]);
    }
    status->boxes[4].x = 0;
    status->boxes[4].width = 256;
    status->boxes[4].y = 64;
    status->boxes[4].height = 272;
    endPrintExtFunc(status->style, 101, &status->boxes[4]);
    for (i = 0; i < MenuWork.partyCount; i++) {
        MoveSlide(&status->windows[i].x, &targetX[i], 3.0f);
        MoveSlide(&status->windows[i].y, &targetY[i], 3.0f);
        if (status->windows[i].y == targetY[i]) {
            MenuWork.scrollDir = 0;
        }
        WindowDXMain((WindowDX *)&status->windows[i]);
    }
    if (MenuWork.selectionMode == 1) {
        status->cursors[0].x = status->windows[MenuWork.selectedParty].x + 3;
        status->cursors[0].y = status->windows[MenuWork.selectedParty].y + status->windows[MenuWork.selectedParty].height / 2 - 4;
        eCursolMain((ECursolMainData *)(void *)(&status->cursors[0]));
        if (MenuWork.partyCount >= 4 && status->cursors[0].state != 0) {
            if (status->scrollStart != 0) {
                status->cursors[1].x = 240;
                status->cursors[1].y = 50;
                eCursolMain((ECursolMainData *)(void *)(&status->cursors[1]));
            }
            if (status->scrollStart != MenuWork.partyCount - 3) {
                status->cursors[2].x = 240;
                status->cursors[2].y = 338;
                eCursolMain((ECursolMainData *)(void *)(&status->cursors[2]));
            }
        }
    } else if (MenuWork.selectionMode == 2) {
        status->cursors[0].x = status->windows[MenuWork.selectedParty].x + 3;
        status->cursors[0].y = status->selectionRow * 104 + status->windows[MenuWork.selectedParty].height / 2 + 60;
        eCursolMain((ECursolMainData *)(void *)(&status->cursors[0]));
    }
    for (i = 0; i < 2; i++) {
        status->boxes[i + 2].x = status->boxes[i].x;
        status->boxes[i + 2].width = status->boxes[i].width;
        status->boxes[i + 2].y = status->boxes[i].y;
        status->boxes[i + 2].height = status->boxes[i].height;
        endPrintExtFunc(status->style, 12, &status->boxes[i + 2]);
    }
    endPrintExtFunc(status->style, 102, 0);
}

void MenuItemStatusAgwsMain(void)
{
    MenuItemAgwsStatusWork *status = MenuItemStatusAgws;
    short targetX[6];
    int attack[3];
    int defense[2];
    register int i;

    switch (status->state) {
    case 0: {
        status->color = 0xF000F0;
        for (i = 0; i < MenuWork.agwsCount; i++) {
            WindowDXSet((WindowDX *)&status->rows[i].window);
            status->rows[i].window.width = 240;
            status->rows[i].window.height = 50;
            status->rows[i].window.color = status->color;
            status->rows[i].window.x = -272;
            status->rows[i].window.state = 1;
            status->rows[i].window.y = i * status->rows[i].window.height + 64;
            WindowDXMain((WindowDX *)&status->rows[i].window);
            status->rows[i].window.state = 3;
            eMessageSet(&status->rows[i].name, MenuCharNameGet(MenuItemAgws[i]));
            status->rows[i].name.mode = 32;
            status->rows[i].name.color = status->rows[i].window.color + 2;
            eTagFontSet(&status->rows[i].hpLabel, fhpText);
            status->rows[i].hpLabel.work = status->color + 2;
            status->rows[i].hpLabel.color[0] = 112;
            eNumberSet(&status->rows[i].hpNumber, 0);
            status->rows[i].hpNumber.work = status->color + 2;
            status->rows[i].hpNumber.format = 12;
            status->rows[i].hpNumber.digitCount = 4;
        }
        eCursolSet((ECursol *)(void *)(&status->cursor), 0);
        status->cursor.mode = 32;
        status->state = 2;
        status->cursor.work = status->color + 4;
    }
        /* fall through */
    case 2:
        break;
    default:
        return;
    }

    for (i = 0; i < MenuWork.agwsCount; i++) {
        targetX[i] = -272;
    }
    if (MenuWork.mode == 32 || MenuWork.mode == 34) {
        if (MenuWork.flags & 1) {
            if (MenuWork.mode == 32) {
                status->cursor.mode = 0;
            } else {
                status->cursor.mode = 32;
            }
        }
        if (MenuWork.agwsMode == 1) {
            for (i = 0; i < MenuWork.agwsCount; i++) {
                targetX[i] = 16;
            }
        }
    }
    i = 0;
    while (i < MenuWork.agwsCount) {
        MenuItemHpParameters *parameters = func_00A11108(MenuItemAgws[i], attack, defense);

        MoveSlide(&status->rows[i].window.x, &targetX[i], 3.0f);
        WindowDXMain((WindowDX *)&status->rows[i].window);
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
        eCursolMain((ECursolMainData *)(void *)(&status->cursor));
    }
}

void MenuItemListMake00(void)
{
    MenuItemListRow *rows = MenuListGet(0);
    int *sortRow = MenuSortAddrGet(0);
    MenuItemListWork *work;
    int count;
    int i;

    MenuSortSet(0, 1, 0);
    MenuSortChange(0, 0);
    MenuSortChange(0, MenuWork.sortChoices[0]);
    MenuListMake(0, 0);
    count = MenuSortCheck(0);
    for (i = 0; i < count; i++) {
        int use = MenuItemUseCheck((short) sortRow[i]);

        switch (use) {
        case -1:
            rows[i].flag = 1;
            rows[i].state = 2;
            break;
        case 0:
            rows[i].flag = 1;
            rows[i].state = 1;
            break;
        case 1:
        case 2:
        default:
            rows[i].flag = 0;
            rows[i].state = 0;
            break;
        }
    }
    work = MenuItemList;
    work->window.rowCount = 7;
    work->window.title = listNumText;
    MenuItemList->window.columns = 1;
    MenuItemList->window.rows = 11;
    MenuItemList->window.width = 224;
    MenuItemList->window.height = MenuItemList->window.rows * 24 + 6;
    MenuItemList->window.items = MenuListGet(0);
    WindowSPItemChange((WindowSPCursor *)&MenuItemList->window);
    WindowSPSetSelect((WindowSPCursor *)&MenuItemList->window, MenuKeepSelect);
}

void MenuItemListMake00_1(void)
{
    MenuItemListRow *rows = MenuListGet(0);
    int *sortRow = MenuSortAddrGet(0);
    MenuItemListWork *work;
    int count;
    int i;

    MenuListMake(0, 0);
    count = MenuSortCheck(0);
    for (i = 0; i < count; i++) {
        int use = MenuItemUseCheck((short) sortRow[i]);

        switch (use) {
        case -1:
            rows[i].flag = 1;
            rows[i].state = 2;
            break;
        case 0:
            rows[i].flag = 1;
            rows[i].state = 1;
            break;
        case 1:
        case 2:
            rows[i].flag = 0;
            rows[i].state = 0;
            break;
        }
    }
    work = MenuItemList;
    work->window.rowCount = 7;
    work->window.title = listNumText;
    MenuItemList->window.columns = 1;
    MenuItemList->window.rows = 11;
    MenuItemList->window.width = 224;
    MenuItemList->window.height = MenuItemList->window.rows * 24 + 6;
    MenuItemList->window.items = MenuListGet(0);
    WindowSPItemChange((WindowSPCursor *)&MenuItemList->window);
    WindowSPSetSelect((WindowSPCursor *)&MenuItemList->window, MenuKeepSelect);
}

void MenuItemListMake01(int category)
{
    MenuItemListWork *work;

    switch (category) {
    case 0:
        MenuSortSet(0, 4, -1);
        break;
    case 1:
        MenuSortSet(0, 16, -1);
        break;
    case 2:
        MenuSortSet(0, 8, -1);
        break;
    case 3:
        MenuSortSet(0, 4, -2);
        break;
    case 4:
        MenuSortSet(0, 16, -2);
        break;
    case 5:
        MenuSortSet(0, 8, -2);
        break;
    }
    MenuSortChange(0, 0);
    MenuSortChange(0, MenuWork.sortChoices[category + 2]);
    MenuListMake(0, 0);
    work = MenuItemList;
    work->window.rowCount = 7;
    work->window.title = listNum3NumText;
    MenuItemList->window.columns = 2;
    MenuItemList->window.rows = 11;
    MenuItemList->window.width = 468;
    MenuItemList->window.height = MenuItemList->window.rows * 24 + 6;
    MenuItemList->window.items = MenuListGet(0);
    WindowSPItemChange((WindowSPCursor *)&MenuItemList->window);
    WindowSPSetSelect((WindowSPCursor *)&MenuItemList->window, &MenuKeepSelect[category * 5 + 10]);
}

void MenuItemListMake02(void)
{
    struct MenuItemListWork *list;

    MenuSortSet(0, 2, 0);
    MenuSortChange(0, 0);
    MenuSortChange(0, MenuWork.sortChoices[1]);
    MenuListMake(0, 0);

    list = MenuItemList;
    list->window.rowCount = 7;
    list->window.title = listNum3NumText;
    MenuItemList->window.columns = 2;
    MenuItemList->window.rows = 11;
    MenuItemList->window.width = 0x1D4;
    MenuItemList->window.height = MenuItemList->window.rows * 0x18 + 6;
    MenuItemList->window.items = MenuListGet(0);

    WindowSPItemChange((WindowSPCursor *)&MenuItemList->window);
    WindowSPSetSelect((WindowSPCursor *)&MenuItemList->window, &MenuKeepSelect[5]);
}

void MenuItemListMain(void)
{
    MenuItemListWork *list = MenuItemList;
    short targetX;
    int i;

    switch (list->state) {
    case 0:
        list->window.x = 528;
        list->window.color = list->color = 0xFFFFF0;
        list->window.title = msg00_1_0036D698;
        list->window.y = 64;
        list->window.rowCount = 7;
        list->window.rows = 11;
        list->window.height = 270;
        list->window.items = 0;
        WindowSPSet(&list->window);
        list->visible = 1;
        list->activeTab = -1;
        list->state = 2;
        list->motion = 0;
        /* fall through */
    case 2:
        break;
    default:
        return;
    }

    if (MenuWork.mode == 0xF0)
        list->motion = 99;

    switch (list->motion) {
    case 0:
        list->motion = 1;
        /* fall through */
    case 1:
        list->window.x = 528;
        switch (MenuWork.listTab) {
        case 0:
            MenuItemListMake00();
            break;
        case 1:
            MenuItemListMake02();
            break;
        default:
            MenuItemListMake01(MenuWork.listTab - 2);
            break;
        }
        list->window.state = 17;
        WindowSPMain(&list->window);
        list->motion = 2;
        /* fall through */
    case 2:
        if (list->activeTab != MenuWork.listTab) {
            list->activeTab = MenuWork.listTab;
            list->motion = 1;
        }
        if (MenuWork.listTab == 0)
            targetX = 272;
        else if (MenuWork.mode == 0x30)
            targetX = 528;
        else
            targetX = 24;
        switch (MenuWork.mode) {
        case 0x22:
            list->motion = 3;
            break;
        case 0x80:
            list->motion = 4;
            break;
        }
        break;
    case 3:
        if (MenuWork.mode != 0x22)
            list->motion = 2;
        targetX = 272;
        break;
    case 4:
        if (PadData.pressed & 0x20) {
            switch (MenuWork.listTab) {
            case 0:
                MenuItemListMake00();
                break;
            case 1:
                MenuItemListMake02();
                break;
            default:
                MenuItemListMake01(MenuWork.listTab - 2);
                break;
            }
        }
        if (MenuWork.mode != 0x80)
            list->motion = 2;
        targetX = 272;
        break;
    case 99:
        targetX = 528;
        break;
    default:
        break;
    }
    for (i = 0; i < 1; i++) {
        short *currentX = &list[i].window.x;
        short slideEnd = 528;

        MoveSlide(currentX, &targetX, 3.0f);
        if (*currentX != slideEnd)
            WindowSPMain(&list[i].window);
    }
}

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
    case 0:
        select->color = 0xFFFFF0;
        windowWidth = 176;
        windowHeight = 102;
        for (n = 0, m = 0; n < 1; n++, m++) {
        int *color = &select->color;
        const char **titleTable = msg10_3;
        const char **sortTextTable = msg00_2_0036D6A8;
        short *size = &select->window[n].width;
        int spriteColor;
        int windowState;

        WindowDXSet((WindowDX *)&select->window[n]);
        select->window[n].title = titleTable[0];
        windowState = 3;
        select->window[m].callbackArg = &select->content;
        select->window[n].color = *color;
        select->window[n].callback = (void (*)(MenuItemSelectWindow *, MenuItemSelectContent *))MenuSelectWindow;
        size[0] = windowWidth;
        select->content.text = sortTextTable[n];
        select->window[0].height = windowHeight;
        select->content.mode = 0;
        select->content.selection = 0;
        select->window[n].x = -208;
        select->window[n].y = 160;
        select->window[n].state = 1;
        WindowDXMain((WindowDX *)&select->window[n]);
        select->window[n].state = windowState;
        eSpriteSet(&select->sprite, 0x60A);
        spriteColor = *color;
        select->unmodeled_02 = 0;
        select->state = n + 2;
        select->sprite.color = spriteColor + 2;
    }
        /* fall through */
    case 2:
        break;
    default:
        return;
    }

    menu = &MenuWork;
    targetY = &slideTarget.y;
    slideTarget.x = -208;
    *targetY = 160;
    slideTarget.mode = 0;
    if (menu->mode == 0x80) {
        slideTarget.x = 80;
        slideTarget.mode = 1;
        select->content.selection = menu->sortSelection;
    }
    {
        short *currentX = &select->window[0].x;
        signed char tab;

        MoveSlide(currentX, &slideTarget.x, 3.0f);
        MoveSlide(&select->window[0].y, targetY, 3.0f);
        WindowDXMain((WindowDX *)&select->window[0]);
        select->sprite.x = (unsigned short)*currentX + 144;

        /* Eight tabs share the ability, normal and six category sort choices. */
        tab = menu->listTab;
        sortMode = menu->sortChoices[tab];
        select->sprite.y = (unsigned short)select->window[0].y + ((signed char)sortMode / 2) * 24 + 2;
        eSpriteMain(&select->sprite);
    }
}

void MenuItemIconMain(void)
{
    MenuItemIconWork *icon = MenuItemIcon;
    signed char *tabBounce;
    signed char *tabBounceIterator;
    int i;

    if (MenuWork.mode == 0) {
        return;
    }
    switch (icon->state) {
    case 0: {
        MenuItemSpriteIds spriteIds = iconSpriteTable;
        signed char *bounce;

        icon->style = 0xFFFFF0;
        for (i = 0; i < 10; i++) {
            eSpriteSet(&icon->sprites[i], spriteIds.ids[i]);
            icon->sprites[i].style = icon->style;
        }
        for (i = 0; i < 2; i++) {
            icon->sprites[i].red = icon->sprites[i].green = icon->sprites[i].blue = -128;
        }
        for (; i < 10; i++) {
            icon->sprites[i].red = icon->sprites[i].green = icon->sprites[i].blue = -128;
        }
        icon->pulseDirection = 1;
        tabBounce = &icon->displacement[2];
        icon->slide = 16;
        icon->pulse = 0;
        icon->pulsePhase = 0;
        icon->displacement[0] = 0;
        icon->displacement[1] = 0;
        bounce = tabBounce + 7;
        for (i = 7; i >= 0; i--) {
            *bounce-- = 0;
        }
        icon->state = 2;
        break;
    }
    case 2:
        tabBounce = &icon->displacement[2];
        break;
    default:
        return;
    }

    switch (MenuWork.mode) {
    case 240:
        icon->pulseDirection = 0;
        /* fall through */
    case 255:
        if (icon->slide != 15) {
            icon->slide++;
        }
        break;
    default:
        if (icon->slide != 0) {
            icon->slide--;
        }
        break;
    }
    if (MenuWork.mode == 32 && MenuWork.tabChangeDirection != 0) {
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

    for (i = 0; i < 2; i++) {
        float cosine;
        int slideOffset;

        icon->sprites[i].x = 264 + i * 208 + icon->displacement[i];
        cosine = xglCos(i * menuItemPi);
        slideOffset = -2 * icon->slide;
        icon->sprites[i].y = 13;
        icon->sprites[i].x += cosine * slideOffset;
        icon->sprites[i].alpha = 128.0f - (icon->slide / 15.0f) * 128.0f;
        if (MenuWork.mode == 48) {
            icon->sprites[i].red = icon->sprites[i].green = icon->sprites[i].blue = 64;
        } else {
            icon->sprites[i].red = icon->sprites[i].green = icon->sprites[i].blue = -128;
        }
    }
    tabBounceIterator = tabBounce;
    for (i = 2; i < 10; i++) {
        int tab = i - 2;
        float cosine;
        int slideOffset;

        icon->sprites[i].x = 296 + tab * 22;
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
        icon->sprites[i].alpha = 128.0f - (icon->slide / 15.0f) * 128.0f;
        tabBounceIterator++;
    }
    for (i = 0; i < 10; i++) {
        eSpriteMain(&icon->sprites[i]);
    }
}

void MenuItemExMain(void)
{
    MenuItemExWork *ex = MenuItemEx;
    MenuItemMessage *message = &ex->message;
    int *messageColor = &ex->message.color;
    short targetX;
    char *textColor;
    int i;

    switch (ex->state) {
    case 0: {
        int initialX = 544;
        int initialY = 345;
        int messageMode = 32;

        ex->color = 0xFFFFF8;
        eMessageSet(message, (const char *)msg00_4_0036D6B8[0]);
        ex->message.x = initialX;
        {
            int messageColorValue = ex->color;
            ex->message.y = initialY;
            *messageColor = messageColorValue;
            ex->message.mode = messageMode;
            ex->state = 2;
        }
    }
        /* fall through */
    case 2:
        targetX = 544;
        if (MenuWork.mode == 32) {
            targetX = 400;
            textColor = msg00_4_0036D6B8[0] + 1;
            if (MenuWork.selectDir >= 0) {
                textColor[0] = textColor[1] = textColor[2] = -128;
            } else {
                textColor[0] = textColor[1] = textColor[2] = 64;
            }
        }
        for (i = 0; i < 1; i++) {
            MoveSlide(&ex[i].message.x, &targetX, 3.0f);
            eMessageMain(&ex[i].message);
        }
    }
}

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

void MenuItemSegmentMain(void)
{
    MenuItemSegmentWork *segment = MenuItemSegment;
    int i;
    MenuItemSegmentAlignedWork *aligned = (MenuItemSegmentAlignedWork *)segment;

    switch (segment->state) {
    case 0: {
        segment->x = -512;
        segment->y = 52;
        segment->work = 0x0FFFF8;
        segment->visible = 0;
        segment->selected = 0;
        for (i = 0; i < 18; i++) {
            MenuItemSegmentRow *row = &segment->rows[i];

            row->flags = 0;
            if (xglFlagsGet(i + 3201, 1)) {
                row->flags |= 1;
            }
            if (xglFlagsGet(i + 3221, 1)) {
                row->flags |= 2;
            }
            if (xglFlagsGet(i + 3241, 1)) {
                row->flags |= 4;
            }
        }
        segment->ownedParts = 0;
        for (i = 0; i < 18; i++) {
            if (segment->rows[i].flags & 4) {
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
        for (i = 0; i < 18; i++) {
            int rowNumber = i + 1;

            eNumberSet(&aligned->rows[i].number, 0);
            aligned->rows[i].number.format = 0;
            aligned->rows[i].number.value = rowNumber;
            aligned->rows[i].number.digitCount = 2;
            if (segment->rows[i].flags == 7) {
                eSpriteSet(&segment->rows[i].sprites[0], 4865);
            } else {
                eSpriteSet(&segment->rows[i].sprites[0], 4864);
            }
            eSpriteSet(&segment->rows[i].sprites[1], 4870);
            eSpriteSet(&segment->rows[i].sprites[2], 4869);
            segment->rows[i].emphasis = 0;
            segment->rows[i].emphasisDirection = 0;
            eSpriteSet(&segment->rows[i].sprites[3], 4868);
            eSpriteSet(&segment->rows[i].sprites[4], 4867);
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
        eRibbonSet(&segment->ribbon, 3);
        {
            short labelSpriteIds[2] = {4886, 4887};

            for (i = 0; i < 2; i++) {
                WindowDXSet(&segment->windows[i]);
                segment->windows[i].color = 0;
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
        segment->state = 2;
    }
        /* fall through */
    case 2:
        if (MenuWork.mode == 48) {
            segment->state = 3;
            segment->visible = 1;
        }
        break;
    case 3: {
        short targetX = 16;

        MoveSlide(&segment->x, &targetX, 3.0f);
        if (segment->x == targetX) {
            segment->state = 4;
        }
        if (MenuWork.mode != 48) {
            segment->state = 5;
        }
        break;
    }
    case 4: {
        int movement = 0;

        switch (PadData.activity) {
        case 0x1000:
            movement = -3;
            break;
        case 0x8000:
            movement = -1;
            break;
        case 0x4000:
            movement = 3;
            break;
        case 0x2000:
            movement = 1;
            break;
        }
        if (movement != 0) {
            segment->selected += movement;
            if (segment->selected < 0) {
                segment->selected += 18;
            } else if (segment->selected >= 18) {
                segment->selected -= 18;
            }
            xglSoundEffectNormalID(3, 0);
        }
        segment->selectedParts = subRoboPartsCheck(segment->selected + 1);
        if (MenuWork.mode != 48) {
            segment->state = 5;
        }
        MenuWork.segmentSelection = segment->selected + 1;
        break;
    }
    case 5: {
        short targetX = -512;

        MoveSlide(&segment->x, &targetX, 3.0f);
        if (segment->x == targetX) {
            segment->visible = 0;
            segment->state = 2;
        }
        break;
    }
    }
    if (segment->visible == 0) {
        return;
    }
    for (i = 0; i < 2; i++) {
        segment->windows[i].x = segment->x + 238;
        segment->windows[i].color = segment->work;
        segment->windows[i].y = aligned->y + i * 80 + 16;
        WindowDXMain(&segment->windows[i]);
    }
    if (segment->rows[segment->selected].flags & 1) {
        segment->text[0].x = segment->windows[0].x + 7;
        segment->text[0].y = segment->windows[0].y + 3;
        segment->text[0].work = segment->windows[0].color + 2;
        segment->text[0].text = MenuSegmentMapNameGet(segment->selected + 1);
        eMessageMain(&segment->text[0]);
    }
    {
        const char *itemName;
        MenuItemInfoWork *info = MenuItemInfo;
        char *description = info->buffer;

        if (segment->rows[segment->selected].flags & 4) {
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
            segment->text[1].work = segment->windows[1].color + 2;
            eMessageMain(&segment->text[1]);
        } else {
            description[0] = emptyText[0];
        }
    }
    segment->texture = WindowTexAddrGet(2);
    endPrintExtFunc(0, 14, &segment->texture);
    segment->rows[0].pulse += segment->rows[0].pulseDirection;
    if (segment->rows[0].pulse == 120 || segment->rows[0].pulse == 0) {
        segment->rows[0].pulseDirection = -segment->rows[0].pulseDirection;
    }
    for (i = 0; i < 18; i++) {
        segment->rows[i].sprites[0].x = segment->x + (i % 3) * 72;
        segment->rows[i].sprites[0].y = segment->y + (i / 3) * 58;
        segment->rows[i].sprites[0].style = segment->work;
        eSpriteMain(&segment->rows[i].sprites[0]);
        if (segment->rows[i].flags & 1) {
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
        if (segment->rows[i].flags == 7) {
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
                       (segment->rows[segment->selected].flags & 4) &&
                       subRoboPartsCheck(i + 1) != 0) {
                if (segment->rows[i].flags & 4) {
                    highlighted = 1;
                } else {
                    highlighted = 0;
                }
            }
            if (highlighted == 1 && segment->rows[i].emphasisDirection == 0) {
                segment->rows[i].emphasisDirection = 4;
            }
            segment->rows[i].emphasis += segment->rows[i].emphasisDirection;
            if (segment->rows[i].emphasis >= 120) {
                segment->rows[i].emphasis = 120;
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
            if (segment->outlines[outline].pulse == 120 || segment->outlines[outline].pulse == 32) {
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
                    endPrintExtFunc(0, 0, &segment->outlines[outline].points[border * 6]);
                }
            }
        }
    }
    segment->ribbon.x = segment->x + 258;
    segment->ribbon.y = segment->y + 136;
    segment->ribbon.work = segment->work;
    segment->ribbon.width = 208;
    segment->ribbon.height = 208;
    eRibbonMain(&segment->ribbon);
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
                    endPrintExtFunc(0, 0, &aligned->vertices[i * 4]);
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
        segment->labels[i].style = segment->windows[i].color;
        segment->labels[i].y = segment->windows[i].y + 3;
        eSpriteMain(&segment->labels[i]);
    }
    endPrintExtFunc(0, 15, 0);
    for (i = 0; i < 18; i++) {

        aligned->rows[i].number.x = segment->rows[i].sprites[0].x + 15;
        aligned->rows[i].number.work = segment->rows[i].sprites[0].style + 1;
        aligned->rows[i].number.y = segment->rows[i].sprites[0].y + 6;
        eNumberMain(&aligned->rows[i].number);
    }
}

int MenuItem(void)
{
    MenuItemWorkData *menu;
    MenuItemUseData *item;
    int characters[8];
    int i;
    int memberIndex;

    if (MenuWork.mode == 0) {
        MenuItemArena *arena = MainMenuWorkEnd;
        unsigned char *cursor = MainMenuWorkEnd;
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
        pas->state = 0;
        allocation = cursor;
        info = allocation;
        MenuItemInfo = info;
        cursor += sizeof(arena->info);
        info->state = 0;
        allocation = cursor;
        status = allocation;
        MenuItemStatus = status;
        cursor += sizeof(arena->status) + sizeof(arena->unmodeled_57a4);
        status->state = 0;
        allocation = cursor;
        agws = allocation;
        MenuItemStatusAgws = agws;
        cursor += sizeof(arena->agws) + sizeof(arena->unmodeled_66f4);
        agws->state = 0;
        allocation = cursor;
        list = allocation;
        MenuItemList = list;
        cursor += sizeof(arena->list) + sizeof(arena->unmodeled_6738);
        list->state = 0;
        allocation = cursor;
        icon = allocation;
        MenuItemIcon = icon;
        cursor += sizeof(arena->icon) + sizeof(arena->unmodeled_7ff4);
        icon->state = 0;
        allocation = cursor;
        select = allocation;
        MenuItemSelect = select;
        cursor += sizeof(arena->select) + sizeof(arena->unmodeled_8f70);
        select->state = 0;
        allocation = cursor;
        ex = allocation;
        MenuItemEx = ex;
        cursor += sizeof(arena->ex) + sizeof(arena->unmodeled_8fc8);
        ex->state = 0;
        allocation = cursor;
        line = allocation;
        MenuItemLine = line;
        cursor += sizeof(arena->line);
        line->state = 0;
        MenuItemSegment = cursor;
        segment = (MenuItemSegmentWork *)cursor;
        cursor += sizeof(arena->segment);
        segment->state = 0;
        MenuWorkEndCheck(segment + 1);

        MenuWork.nextMode = 0x20;
        MenuWork.busy = 12;
        MenuWork.partyCount = 0;
        MenuWork.flags = 0;
        savedSelection = MenuKeepSelect;
        do {
            unsigned char *entry = savedSelection;

            savedSelection += 5;
            remaining--;
            memset(entry, 0, 5U);
        } while (remaining >= 0);
        for (i = 0; i < 8; i++) {
            unsigned char choice = MenuSaveDataGet()[i];

            MenuWork.sortChoices[i] = choice;
        }
        MenuWork.partyCount = PartyAllPartyGet(characters, 0);
        for (memberIndex = 0; memberIndex < MenuWork.partyCount; memberIndex++) {
            MenuItemParty[memberIndex] = (unsigned short)characters[memberIndex];
        }
        if ((unsigned int)MenuScenarioNo - 0x90U < 9U) {
            MenuWork.agwsCount = 3;
            MenuItemAgws[0] = 18;
            MenuItemAgws[1] = 19;
            MenuItemAgws[2] = 20;
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
        MenuWork.flags |= 1;
    } else {
        MenuWork.flags &= 0xFE;
    }
    menu = &MenuWork;
    if (MenuWork.busy != 0) {
        MenuWork.busy--;
    }

    switch (MenuWork.mode) {
    case 0x40:
        return menu->selectedItem;

    case 0x20:
        if ((menu->flags & 1) == 0) {
            if (menu->busy == 0) {
                menu->selectDir = WindowSPSelect((WindowSPCursor *)&MenuItemList->window, PadData.activity);
                WindowSPKeepSelect((WindowSPCursor *)&MenuItemList->window, &MenuKeepSelect[menu->listTab * 5]);
                menu->tabChangeDirection = 0;
                if (PadData.activity == 4) {
                    menu->tabChangeDirection = -1;
                }
                if (PadData.activity == 8) {
                    menu->tabChangeDirection = 1;
                }
                if (menu->tabChangeDirection != 0) {
                    WindowSPKeepSelectCheck(&MenuKeepSelect[menu->listTab * 5]);
                    menu->listTab += menu->tabChangeDirection;
                    if (menu->listTab < 0) {
                        menu->listTab = 7;
                    }
                    if (menu->listTab >= 8) {
                        menu->listTab = 0;
                    }
                    xglSoundEffectNormalID(1, 0);
                    menu->busy = 12;
                }
                if (PadData.activity == 0x10) {
                    if (MenuWork.selectDir >= 0) {
                        xglSoundEffectNormalID(1, 0);
                        MenuWork.nextMode = 0x80;
                    } else {
                        xglSoundEffectNormalID(5, 0);
                    }
                }
                if (MenuWork.selectDir >= 0) {
                    MenuWork.selectedItem = MenuSortGet(0, MenuWork.selectDir);
                    MenuWork.selectionMode = MenuItemUseCheck(MenuWork.selectedItem);
                    if (PadData.pressed == 0x20) {
                        switch (MenuWork.listTab) {
                        case 0:
                            switch (MenuWork.selectionMode) {
                            case 1:
                            case 2:
                                xglSoundEffectNormalID(1, 0);
                                MenuWork.nextMode = 0x22;
                                MenuWork.agwsMode = 0;
                                break;
                            case 10:
                                if (MenuWork.agwsCount != 0) {
                                    xglSoundEffectNormalID(1, 0);
                                    MenuWork.nextMode = 0x22;
                                    MenuWork.agwsMode = 1;
                                    MenuWork.busy = 8;
                                } else {
                                    xglSoundEffectNormalID(5, 0);
                                }
                                break;
                            default:
                                xglSoundEffectNormalID(5, 0);
                                break;
                            }
                            break;
                        case 1:
                            if ((unsigned int)((unsigned short)MenuWork.selectedItem - 11) < 5U) {
                                if (MenuWork.selectedItem == 11) {
                                    MenuWork.nextMode = 0x30;
                                    xglSoundEffectNormalID(1, 0);
                                } else if ((unsigned int)GameLoopState.flags & 0x400000) {
                                    MenuWork.nextMode = 0x40;
                                    xglSoundEffectNormalID(1, 0);
                                } else {
                                    xglSoundEffectNormalID(5, 0);
                                }
                            }
                            break;
                        }
                    }
                }
                if (PadData.pressed & 0x40) {
                    xglSoundEffectNormalID(2, 0);
                    MenuWork.nextMode = 0xF0;
                }
            } else {
                menu->selectDir = WindowSPSelect((WindowSPCursor *)&MenuItemList->window, 0);
            }
        }
        break;

    case 0x22:
        if (menu->busy == 0) {
            if (menu->agwsMode != 0) {
                int selection = MenuSelectMove(menu->selectedAgws, menu->agwsCount, 0);
                unsigned short pressed = PadData.pressed;

                menu->selectedAgws = selection;
                if (pressed == 0x20) {
                    int itemKey = MenuSortGet(0, menu->selectDir);
                    short itemId = itemKey;

                    menu->flags |= 4;
                    func_A1A4E8(itemId);
                    if (MenuCharHpCheck(MenuItemAgws[menu->selectedAgws]) != 0) {
                        xglSoundEffectNormalID(5, 0);
                        break;
                    }
                    MenuBoxDec(itemKey);
                    if (MenuBoxChk(itemKey) == 0) {
                        WindowSPKeepSelect((WindowSPCursor *)&MenuItemList->window, &MenuKeepSelect[menu->listTab * 5]);
                        MenuItemListMake00();
                        menu->nextMode = pressed;
                    }
                    func_00A114C0(MenuItemAgws[menu->selectedAgws], itemId);
                    xglSoundEffectNormalID(1, 0);
                    MenuItemListMake00_1();
                }
                if (PadData.pressed == 0x40) {
                    MenuItemListMake00();
                    xglSoundEffectNormalID(2, 0);
                    MenuWork.nextMode = 0x20;
                }
            } else if (menu->scrollDir == 0) {
                signed char selectionMode = menu->selectionMode;

                if (selectionMode == 1 && menu->partyCount >= 2) {
                    if (PadData.activity == 0x1000) {
                        menu->scrollDir = -1;
                    }
                    if (PadData.activity == 0x4000) {
                        menu->scrollDir = selectionMode;
                    }
                    if (menu->scrollDir != 0) {
                        if (menu->scrollDir == -1) {
                            if (menu->selectedParty != 0) {
                                menu->selectedParty--;
                                xglSoundEffectNormalID(3, 0);
                            }
                        } else {
                            if (menu->selectedParty != menu->partyCount - 1) {
                                menu->selectedParty++;
                                xglSoundEffectNormalID(3, 0);
                            }
                        }
                    }
                }
                if (PadData.pressed == 0x40) {
                    MenuItemListMake00();
                    xglSoundEffectNormalID(2, 0);
                    MenuWork.nextMode = 0x20;
                }
                if (PadData.pressed == 0x20) {
                    int itemKey = MenuSortGet(0, MenuWork.selectDir);
                    short itemId = itemKey;

                    MenuWork.flags |= 4;
                    item = func_A1A4E8(itemId);
                    if (itemId == MENU_ITEM_SPECIAL_ID) {
                        if (MenuCharHpCheck(-1) != 0 && MenuCharEpCheck(-1) != 0) {
                            xglSoundEffectNormalID(5, 0);
                            break;
                        }
                    } else {
                        unsigned short attributes;
                        MenuItemCharacterPoints *points = func_A19210(MenuItemParty[MenuWork.selectedParty]);

                        attributes = item->attributes;
                        if (attributes & 1) {
                            if (MenuWork.selectionMode == 1) {
                                if (MenuCharHpCheck(MenuItemParty[MenuWork.selectedParty]) != 0) {
                                    xglSoundEffectNormalID(5, 0);
                                    break;
                                }
                            } else {
                                if (MenuCharHpCheck(-1) != 0) {
                                    xglSoundEffectNormalID(5, 0);
                                    break;
                                }
                            }
                        } else if (attributes & 2) {
                            if (MenuWork.selectionMode == 1) {
                                if (MenuCharEpCheck(MenuItemParty[MenuWork.selectedParty]) != 0) {
                                    xglSoundEffectNormalID(5, 0);
                                    break;
                                }
                            } else {
                                if (MenuCharEpCheck(-1) != 0) {
                                    xglSoundEffectNormalID(5, 0);
                                    break;
                                }
                            }
                        } else if (attributes & 0x800) {
                            if (points->points >= 9999) {
                                xglSoundEffectNormalID(5, 0);
                                break;
                            }
                        } else if (attributes & 0x400) {
                            if (points->etherPoints >= 9999) {
                                xglSoundEffectNormalID(5, 0);
                                break;
                            }
                        } else if (attributes & 0x200) {
                            if (points->skillPoints >= 9999) {
                                xglSoundEffectNormalID(5, 0);
                                break;
                            }
                        }
                    }
                    MenuBoxDec(itemKey);
                    if (MenuBoxChk(itemKey) == 0) {
                        WindowSPKeepSelect((WindowSPCursor *)&MenuItemList->window, &MenuKeepSelect[MenuWork.listTab * 5]);
                        MenuItemListMake00();
                        MenuWork.nextMode = 0x20;
                    }
                    func_00A114C0(MenuItemParty[MenuWork.selectedParty], itemId);
                    xglSoundEffectNormalID(1, 0);
                    MenuItemListMake00_1();
                }
            }
        }
        break;

    case 0x30:
        if (menu->flags & 1) {
            menu->segmentSelection = 0;
            menu->busy = 12;
        }
        if (menu->busy == 0 && (PadData.pressed & 0x40)) {
            xglSoundEffectNormalID(2, 0);
            menu->nextMode = 0x20;
            menu->busy = 12;
        }
        break;

    case 0x80: {
        if (MenuWork.flags & 1) {
            signed char initialTab;
            unsigned char initialChoice;

            initialTab = MenuWork.listTab;
            initialChoice = menu->sortChoices[initialTab];
            MenuWork.busy = 8;
            MenuWork.sortSelection = (signed char)initialChoice / 2;
        }
        if (MenuWork.busy == 0) {
            int selection = MenuSelectMove(MenuWork.sortSelection, 4, 0);
            int pressed = PadData.pressed;

            MenuWork.sortSelection = selection;
            if (pressed == 0x20) {
                if (MenuWork.sortSelection != 3) {
                    unsigned char *saved;

                    *(menu->sortChoices + menu->listTab) = MenuWork.sortSelection * 2;
                    saved = MenuSaveDataGet();
                    saved[menu->listTab] = menu->sortChoices[menu->listTab];
                    MenuWork.flags |= 4;
                    xglSoundEffectNormalID(15, 0);
                } else {
                    MenuWork.nextMode = pressed;
                    xglSoundEffectNormalID(2, 0);
                }
            }
            if (PadData.pressed == 0x40) {
                MenuWork.nextMode = 0x20;
                MenuWork.busy = 12;
                xglSoundEffectNormalID(2, 0);
            }
        }
        break;
    }

    case 0xF0:
        if (menu->flags & 1) {
            menu->busy = 16;
        }
        if (menu->busy == 0) {
            menu->nextMode = 0xFF;
        }
        break;

    case 0xFF:
        ChangeTopLevel(0);
        break;
    }

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

/* JP: 仮
 * EN: Temporary */
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

static char * msg00_4_0036D6B8[] = { sortHeaderText };

const MenuItemInfoLabels infoLabels = { { MenuItemInfoLabelsText + 16, MenuItemInfoLabelsText + 40, MenuItemInfoLabelsText + 56 }, MenuItemInfoLabelsText + 80, MenuItemCancelText };

const char itemCannotBeUsedHereText[32] = "This item cannot be used here.";

const char itemSavePointOnlyText[48] = "This item can only be used at save points.";

const MenuItemSpriteIds iconSpriteTable = { { 0x0112, 0x0110, 0x0322, 0x0323, 0x0324, 0x0325, 0x0328, 0x0326, 0x0327, 0x0329 } };

/* JP: ：Sort
 * EN: : Sort */
const char sortHeaderText[24] = "\014\200\200\200\036\002\241\247Sort";

MenuItemInfoHelpLabels infoHelpTable[1] = { { itemCannotBeUsedHereText, itemSavePointOnlyText } };
