#include "common.h"
#include "main/menu_1.h"
#include "main/menu_top_command_check.h"
#include "main/xgl_render.h"
#include "main/xgl_packet.h"
#include "main/xgl_thread.h"
#include "main/party.h"
#include "main/game_snap_shot.h"
#include "shared.h"
#include "main/xgl_task.h"
#include "menu_shop.h"

/* Referenced original data recovered from this translation unit. */
#include "main/xgl_studio.h"

#include "main/e_battle_win_open.h"
#include "main/open_close_main.h"
#include "main/e_number_main.h"
#include "main/xgl_sound.h"
#include "main/data.h"
#include "main/e_message.h"
#include "main/end_print.h"
#include "main/menu_model.h"
#include "main/xgl_camera.h"
#include "main/xgl_dma.h"
#include "main/xgl_font.h"

#define NULL ((void *)0)

/* Shop state machine (MenuShopWorkData.state / nextState). */
enum {
    SHOP_STATE_MODE_SELECT = 0x20,    /* Buy / Sell / Tune A.G.W.S. / Quit */
    SHOP_STATE_BUY_LIST = 0x30,       /* item list, buy mode */
    SHOP_STATE_SELL_LIST = 0x90,      /* item list, sell mode */
    SHOP_STATE_QUANTITY = 0xA0,       /* choose how many */
    SHOP_STATE_CONFIRM = 0xA2,        /* "Buy it?" / "Sell it?" */
    SHOP_STATE_RESULT = 0xA4,         /* "purchased." / "sold." */
    SHOP_STATE_AGWS_MENU = 0xC0,      /* Buy Frame / Buy Generator / Restore FHP / Quit */
    SHOP_STATE_RESTORE_UNIT = 0xC2,   /* pick the A.G.W.S. to restore */
    SHOP_STATE_RESTORE_CONFIRM = 0xC4,
    SHOP_STATE_RESTORE_DONE = 0xC6,
    SHOP_STATE_AGWS_BUY_LIST = 0xD0,  /* frame / generator price list */
    SHOP_STATE_AGWS_BUY_UNIT = 0xD2,  /* pick the A.G.W.S. to fit it to */
    SHOP_STATE_AGWS_BUY_CONFIRM = 0xD4,
    SHOP_STATE_AGWS_BUY_DONE = 0xD6,
    SHOP_STATE_EXIT = 0xF0,
    SHOP_STATE_END = 0xFF
};

/* High nibble of the A.G.W.S. tuning states. */
#define SHOP_STATE_GROUP_AGWS_MENU 0xC
#define SHOP_STATE_GROUP_AGWS_BUY 0xD

/* MenuShopWorkData.stateFlags */
#define SHOP_FLAG_STATE_ENTERED 0x01

/* MenuShopWorkData.mode: the command chosen in SHOP_STATE_MODE_SELECT. */
enum {
    SHOP_MODE_BUY = 0,
    SHOP_MODE_SELL = 1,
    SHOP_MODE_TUNE_AGWS = 2,
    SHOP_MODE_QUIT = 3
};

/* MenuShopWorkData.category: list category (also the bit index in categoryMask). */
enum {
    SHOP_CAT_ITEM = 0,
    SHOP_CAT_CHAR_WEAPON = 1,
    SHOP_CAT_CHAR_ACCESSORY = 2,
    SHOP_CAT_CHAR_AMMO = 3,
    SHOP_CAT_AGWS_WEAPON = 4,
    SHOP_CAT_AGWS_ACCESSORY = 5,
    SHOP_CAT_AGWS_AMMO = 6,
    SHOP_CAT_AGWS = 7,
    SHOP_CAT_CARD = 8,
    SHOP_CAT_COUNT = 9
};

/* High word of an item key (key >> 16): what kind of record the key names. */
enum {
    ITEM_KIND_ITEM = 1,
    ITEM_KIND_EVENT = 2,
    ITEM_KIND_WEAPON = 3,
    ITEM_KIND_BULLET = 4,
    ITEM_KIND_ACCESSORY = 5,
    ITEM_KIND_AGWS = 7,
    ITEM_KIND_FRAME = 11,
    ITEM_KIND_ENGINE = 12
};
#define ITEM_KEY_FRAME 0xB0000
#define ITEM_KEY_ENGINE 0xC0000

/* MenuSortSet category masks and filters (the sell lists). */
#define SHOP_SORT_ITEM 1
#define SHOP_SORT_EVENT 2
#define SHOP_SORT_WEAPON 4
#define SHOP_SORT_BULLET 8
#define SHOP_SORT_ACCESSORY 16
#define SHOP_SORT_FILTER_CHARACTER (-1)
#define SHOP_SORT_FILTER_AGWS (-2)

/* WindowDX.state: set to OPEN_REQUEST, WindowDXMain runs the open animation, then OPEN. */
#define WINDOW_STATE_OPEN_REQUEST 1
#define WINDOW_STATE_OPEN 3

/* MenuShopModelDisp: MenuModelUnit.extFuncFlag. */
enum {
    SHOP_MODEL_OPEN = 0,
    SHOP_MODEL_SHOWN = 10,
    SHOP_MODEL_SWAP = 0x1E,
    SHOP_MODEL_FADE_OUT = 0x1F
};

/* Draw-order keys of the shop's UI tasks. */
#define SHOP_DEPTH_TOP 0x00FFFFF0
#define SHOP_DEPTH_LIST 0x000FFFF0
#define SHOP_DEPTH_SELECT 0x00FFFFF4
#define SHOP_DEPTH_PARAMETER 0x00FFFF00

/* MoveSlide rate of every sliding panel. */
#define SHOP_SLIDE_RATE 3.0f

/* Help caption bar (MenuShopPas). */
#define SHOP_PAS_LABEL_COUNT 16
#define SHOP_PAS_WINDOW_WIDTH 272
#define SHOP_PAS_WINDOW_HIDDEN_X (-272)
#define SHOP_PAS_WINDOW_SHOWN_X (-16)
#define SHOP_PAS_LABEL_HIDDEN_X 288
#define SHOP_PAS_LABEL_SHOWN_X 16
/* Caption index (msg00_0_0036DAF0): 0 "Store", 3..11 the categories, then the A.G.W.S. commands. */
#define SHOP_PAS_LABEL_STORE 0
#define SHOP_PAS_LABEL_CATEGORY_FIRST 3
#define SHOP_PAS_LABEL_AGWS_TUNING 12
#define SHOP_PAS_LABEL_FHP_RESTORATION 13
#define SHOP_PAS_LABEL_BUY_FRAME 14
#define SHOP_PAS_LABEL_BUY_GENERATOR 15

/* Bit of MenuShopWorkData.categoryMask: category n is available. */
enum {
    SHOP_CAT_MASK_ITEM = 1 << SHOP_CAT_ITEM,
    SHOP_CAT_MASK_CHAR_WEAPON = 1 << SHOP_CAT_CHAR_WEAPON,
    SHOP_CAT_MASK_CHAR_ACCESSORY = 1 << SHOP_CAT_CHAR_ACCESSORY,
    SHOP_CAT_MASK_CHAR_AMMO = 1 << SHOP_CAT_CHAR_AMMO,
    SHOP_CAT_MASK_AGWS_WEAPON = 1 << SHOP_CAT_AGWS_WEAPON,
    SHOP_CAT_MASK_AGWS_ACCESSORY = 1 << SHOP_CAT_AGWS_ACCESSORY,
    SHOP_CAT_MASK_AGWS_AMMO = 1 << SHOP_CAT_AGWS_AMMO,
    SHOP_CAT_MASK_AGWS = 1 << SHOP_CAT_AGWS,
    SHOP_CAT_MASK_CARD = 1 << SHOP_CAT_CARD
};

/* Item key of an event (key) item, and the three card items sold in the Cards category. */
#define ITEM_KEY_EVENT 0x20000
#define SHOP_CARD_EVENT_1 0x35
#define SHOP_CARD_EVENT_2 0x36
#define SHOP_CARD_EVENT_3 0x37
/* MenuScenarioNo from which the first two cards, then the third, are for sale. */
#define SHOP_CARD_SCENARIO_FIRST 0x73
#define SHOP_CARD_SCENARIO_THIRD 0x12D

/* MenuShopSortSet types above the category range, and their menuShopRecordSortTable rows. */
#define SHOP_SORT_TYPE_FRAME 0x12
#define SHOP_SORT_TYPE_ENGINE 0x13
#define SHOP_TABLE_FRAME 9
#define SHOP_TABLE_ENGINE 10

/* MenuKeepSelect holds one entry per list; each entry is this many bytes. */
#define MENU_KEEP_ENTRY_SIZE 5

/* Item list window (MenuShopList, MenuShopListChange00/01). */
#define SHOP_LIST_WIDTH 272
#define SHOP_LIST_ROW_HEIGHT 24
#define SHOP_LIST_Y 0x50
#define SHOP_LIST_SHOWN_X 0x10
#define SHOP_LIST_ENTER_X (-0x120)
#define SHOP_LIST_HIDDEN_X (-0x110)
/* MenuShopList.state */
enum {
    SHOP_LIST_CLOSED = 0,
    SHOP_LIST_IDLE = 1,
    SHOP_LIST_ITEMS_OPEN = 10,
    SHOP_LIST_ITEMS_SHOWN = 11,
    SHOP_LIST_CLOSING = 12,
    SHOP_LIST_AGWS_OPEN = 30,   /* frame / generator price list */
    SHOP_LIST_AGWS_SHOWN = 31
};

/* Right-hand panels: x of the off-screen (hidden) slot, x when shown beside the list, width. */
#define SHOP_PANEL_HIDDEN_X 528
#define SHOP_PANEL_SHOWN_X 0x130
#define SHOP_PANEL_WIDTH 0x100

/* Sprite tint of a usable (lit) and an unusable (dim) party portrait. */
#define SHOP_SPRITE_LIT 0x80
#define SHOP_SPRITE_DIM 0x40

/* Price of restoring an A.G.W.S.'s FHP. */
#define SHOP_RESTORE_COST 100

/* Most copies of one item the player can hold. */
#define SHOP_STOCK_MAX 99

/* Flags of the item / weapon / bullet / accessory records. */
#define SHOP_ITEM_FLAG_NO_SALE 0x1000
#define SHOP_WEAPON_FLAG_BULLET 0x100

/* MenuShopPay (MenuShopSeisan02): which price is shown. */
enum {
    SHOP_PAY_KIND_RESTORE = 1,
    SHOP_PAY_KIND_BUY = 2
};
/* MenuShopSeisan02Work.state */
enum {
    SHOP_PAY_CLOSED = 0,
    SHOP_PAY_IDLE = 1,
    SHOP_PAY_RESTORE_OPEN = 10,
    SHOP_PAY_RESTORE_SHOWN = 11,
    SHOP_PAY_RESTORE_CLOSING = 12,
    SHOP_PAY_BUY_OPEN = 20,
    SHOP_PAY_BUY_SHOWN = 21,
    SHOP_PAY_BUY_CLOSING = 22
};

/* A.G.W.S. target panel (MenuShopEx02) state. */
enum {
    SHOP_UNITS_CLOSED = 0,
    SHOP_UNITS_IDLE = 1,
    SHOP_UNITS_RESTORE_OPEN = 10,
    SHOP_UNITS_RESTORE_SHOWN = 11,
    SHOP_UNITS_CLOSING = 12,
    SHOP_UNITS_BUY_OPEN = 20,
    SHOP_UNITS_BUY_SHOWN = 21
};

/* Category icon strip (MenuShopIcon): two frame sprites, then one icon per category. */
#define SHOP_ICON_CATEGORY_FIRST 2
#define SHOP_ICON_COUNT (SHOP_ICON_CATEGORY_FIRST + SHOP_CAT_COUNT)
#define SHOP_ICON_SPACING 21
#define SHOP_ICON_SLIDE_MAX 16
#define SHOP_ICON_LIFT_MAX 6

/* Category underline (MenuShopLine). */
#define SHOP_LINE_END_X 512
#define SHOP_LINE_ALPHA_MAX 120
#define SHOP_LINE_ALPHA_STEP 8

/* MenuShopParaSet modes: which stat comparison to build. */
enum {
    SHOP_PARA_FRAME = 0,         /* stats of the frame being bought */
    SHOP_PARA_ENGINE = 1,        /* stats of the generator being bought */
    SHOP_PARA_RESTORE = 2,       /* current FHP of the unit to restore */
    SHOP_PARA_FRAME_FIT = 3,     /* unit stats before / after fitting the frame */
    SHOP_PARA_ENGINE_FIT = 4,    /* unit stats before / after fitting the generator */
    SHOP_PARA_CHAR_WEAPON = 10,
    SHOP_PARA_CHAR_ACCESSORY = 11,
    SHOP_PARA_CHAR_AMMO = 12,
    SHOP_PARA_AGWS_WEAPON = 13,
    SHOP_PARA_AGWS_ACCESSORY = 14,
    SHOP_PARA_AGWS_AMMO = 15,
    SHOP_PARA_AGWS = 16
};
/* Rows of the stat panel: character gear shows 7, A.G.W.S. gear 6. */
#define SHOP_PARA_CHAR_COUNT 7
#define SHOP_PARA_AGWS_COUNT 6

/* Command menus and the confirmation popup (MenuShopSelect). */
#define SHOP_MENU_MAIN 0
#define SHOP_MENU_AGWS 1
#define SHOP_MENU_COMMAND_COUNT 4
/* MenuShopSelectWork.popupFlags */
#define SHOP_POPUP_SLIDES_LEFT 1

/* Description bar at the bottom (MenuShopInfo). */
#define SHOP_INFO_MESSAGE_SIZE 128
#define SHOP_INFO_HEIGHT 54
#define SHOP_INFO_SHOWN_Y 386
#define SHOP_INFO_HIDDEN_Y 480

/* A.G.W.S. unit ids of the VX series; lower ids are the Hyams AG series. */
#define SHOP_AGWS_VX_FIRST_ID 23

/* A.G.W.S. unit ids run from the first up to (not including) the end id. */
#define SHOP_AGWS_UNIT_FIRST 0x11
#define SHOP_AGWS_UNIT_END 0x21

/* MenuShopWorkData.waitFrames: input lockout after entering a state, after a choice, etc. */
#define SHOP_WAIT_SHORT 2
#define SHOP_WAIT_ENTER 8
#define SHOP_WAIT_TRANSITION 0xC
#define SHOP_WAIT_EXIT 0x10

/* Items bought at once move 1 per press, a whole page of this many per left/right press. */
#define SHOP_QUANTITY_PAGE_STEP 10

/* Commands of the A.G.W.S. menu (SHOP_STATE_AGWS_MENU), in sortOption order. */
#define SHOP_AGWS_CMD_FRAME 0
#define SHOP_AGWS_CMD_GENERATOR 1
#define SHOP_AGWS_CMD_RESTORE 2
#define SHOP_AGWS_CMD_QUIT 3
#define SHOP_AGWS_CMD_COUNT 4

/* MenuKeepSelect offsets of the frame and generator price lists. */
#define SHOP_KEEP_FRAME_LIST (SHOP_SORT_TYPE_FRAME * MENU_KEEP_ENTRY_SIZE)
#define SHOP_KEEP_ENGINE_LIST (SHOP_SORT_TYPE_ENGINE * MENU_KEEP_ENTRY_SIZE)

/* ShopData.records[] row holding the A.G.W.S. frames on sale. */
#define SHOP_RECORD_FRAME 7

/* MenuShopMain(shopType): the shop that only buys from the player. */
#define SHOP_TYPE_SELL_ONLY 20

/* MenuShopMain layout of the shop's work memory. */
#define SHOP_DATA_BUFFER_SIZE 8192
#define SHOP_WORK_SIZE 128
#define SHOP_TASK_COUNT 16
#define SHOP_MODEL_MEMORY_SIZE 0x180000
#define SHOP_PAS_SIZE 1516
#define SHOP_INFO_SIZE 976
#define SHOP_LIST_SIZE 5956
#define SHOP_SELECT_SIZE 7928
#define SHOP_EX_SIZE 1728
#define SHOP_EX2_SIZE 480
#define SHOP_EX02_SIZE 472
#define SHOP_SEISAN_SIZE 1668
#define SHOP_SEISAN02_SIZE 984
#define SHOP_ICON_SIZE 508
#define SHOP_PARAMETER_SIZE 1996
#define SHOP_LINE_SIZE 80

/* GameLoopState[4]: set while a menu screen is open. */
#define GAME_LOOP_FLAG_MENU_OPEN 0x04000000

/* Offsets of the equipped engine / frame ids in the unit record. */
#define SHOP_UNIT_ENGINE_ID_OFFSET 86
#define SHOP_UNIT_FRAME_ID_OFFSET 88

/* Each list selection updates the item and its signed model key together. */
#define MENU_SHOP_PREVIEW_ITEM(listIndex) \
    { \
        MenuShopWork->itemId = MenuSortGet(0, (listIndex)); \
        MenuShopWork->modelId = (s16) MenuShopWork->itemId; \
    }

typedef struct ShopDataCategory {
    unsigned short count;
    unsigned char unmodeled_02[30];
    unsigned short slots[4];
} ShopDataCategory;

typedef union ShopDataTail {
    struct {
        unsigned short count;
        unsigned char unmodeled_02[6];
    };
    unsigned short agwsSlots[4];
} ShopDataTail;

typedef struct MenuShopEx2Work {
    unsigned char unmodeled_00[4];
    int color;
    WindowDX window;
    EMessageParam message;
} MenuShopEx2Work;

typedef struct MenuShopWeapon {
    unsigned short shopParameter1;
    unsigned char unmodeled_02[4];
    unsigned short flags;
    unsigned char unmodeled_08[0x0A];
    unsigned short wagl;
    unsigned char unmodeled_14[6];
    short bullet;
    unsigned short shopParameter2;
} MenuShopWeapon;

/* Nine 0x28-byte categories and the final eight-byte slot/count view share
 * the original 0x170-byte storage extent. */
typedef struct {
    ShopDataCategory records[9];
    ShopDataTail tail;
} MenuShopData;

/* Referenced original data recovered from this translation unit. */
struct MenuShopPasMessage {
    unsigned char unmodeled_00;
    unsigned char mode;
    unsigned char unmodeled_02[2];
    short x;
    short y;
    int color;
    unsigned char unmodeled_0c[0x0c];
    const char *text;
    unsigned char unmodeled_1c[0x28];
};

struct MenuShopModelTransform {
    unsigned char unmodeled_00[0x10];
    Vector4 position;
    Vector4 rotation;
    unsigned char unmodeled_30[0x30];
    float fade;
};

struct MenuShopModelActor {
    unsigned char unmodeled_00[0xC0];
    struct MenuShopModelTransform transform;
};

struct MenuShopModelUnit {
    unsigned char unmodeled_00[0x10];
    signed char taskPhase;
    unsigned char active;
    unsigned char unmodeled_12;
    unsigned char state;
    unsigned char unmodeled_14[0x0C];
    struct MenuShopModelActor *actor;
};

struct MenuShopModelCallbackData {
    unsigned char unmodeled_00[0x10];
    unsigned char state;
    unsigned char unmodeled_11[0x0B];
    int modelId;
};

/* The map anchors nine 0x28-byte count records and a separate 8-byte tail.
 * The remaining bytes stay explicitly unmodeled. */
struct MenuShopAgwsListState {
    unsigned char unmodeled_00[0x160];
    unsigned short itemIds[4];
};

/* Partial unit record used by the shop's repair and equipment operations. */
typedef struct MenuShopUnitData {
    unsigned char unmodeled_00[0x34];
    unsigned short hitPoints;
    unsigned char unmodeled_36[0x20];
    short engineId;
    short frameId;
    unsigned char unmodeled_5a[4];
    short weaponIds[3];
    short accessoryIds[3];
    short attachmentIds[3];
    unsigned char unmodeled_70[0x110];
} MenuShopUnitData;

struct MenuShopSortOption {
    unsigned char type;
    unsigned char unmodeled_01[3];
    int order;
};

struct MenuShopSortOptionTable {
    struct MenuShopSortOption entries[9];
};

typedef struct MenuShopLineContext {
    unsigned char unmodeled_00[0x10];
    unsigned char mode;
} MenuShopLineContext;

typedef struct MenuShopLineRow {
    int text_id;
    unsigned char style[4];
    short position;
    short target;
} MenuShopLineRow;

typedef struct MenuShopLineDisplay {
    unsigned char unmodeled_00[3];
    unsigned char phase;
    int text_id;
    short top_position;
    MenuShopLineRow rows[5];
    int unmodeled_48;
} MenuShopLineDisplay;

struct MenuShopRowCountTable {
    unsigned char values[18];
};

typedef signed char s8;

typedef int s32;

typedef struct { unsigned char bytes[8]; } M2C_BLOCK8;

typedef struct MenuShopSortEntry {
    const short *itemIds;
    unsigned int highWord;
    int count;
} MenuShopSortEntry;

typedef struct MenuShopSortTable {
    MenuShopSortEntry entries[11];
} MenuShopSortTable;

struct MenuShopEquipRecord {
    unsigned char unmodeled_00[0x5e];
    short itemIds[3][3];
};

struct MenuShopEquipScratch {
    int partyIds[15];
    int slotIndex[9];
};

typedef struct { unsigned char bytes[4]; } M2C_BLOCK4;

typedef union {
    struct {
        M2C_BLOCK8 first;
        M2C_BLOCK8 second;
        M2C_BLOCK4 third;
        u16 last;
    } blocks;
    s16 ids[11];
} MenuShopIconIds;

typedef long long s64;

/* The calculator and unit snapshots each copy a complete 0x180-byte record.
 * Only the statistics used by the shop are modeled here. */
typedef struct ShopCalculatedPara {
    s16 maxHp;
    s16 maxEp;
    unsigned short attack;
    unsigned short physicalDefense;
    unsigned short displayStat2;
    unsigned short magicDefense;
    unsigned char byteStats[3];
    unsigned char unmodeled_0f[0x25];
    unsigned short currentHp;
    unsigned char unmodeled_36[0x14A];
} ShopCalculatedPara;

/* MenuShopNoSaleCheck reads the halfword at +0x4 (original VA 0x002a3c88). */
typedef struct MenuShopAccessoryData {
    unsigned char unmodeled_00[4];
    unsigned short flags;
    unsigned char unmodeled_06[6];
    unsigned char shopParameter2;
    unsigned char shopParameter3;
} MenuShopAccessoryData;

typedef struct MenuShopItemData {
    unsigned char unmodeled_00[4];
    unsigned short flags;
} MenuShopItemData;

typedef struct MenuShopAttackData {
    unsigned char unmodeled_00[10];
    unsigned short flags;
} MenuShopAttackData;

typedef struct MenuShopPasBounds {
    short x1;
    short y1;
    int color;
    short x2;
    short y2;
} MenuShopPasBounds;

typedef struct MenuShopPasWindow {
    WindowDX common;
    unsigned char unmodeled_30[0x194 - 0x30];
} MenuShopPasWindow;

typedef struct MenuShopPasData {
    unsigned char unmodeled_00[4];
    short x;
    short y;
    int color;
    MenuShopPasWindow window;
    EMessageParam messages[16];
    MenuShopPasBounds bounds;
} MenuShopPasData;

typedef struct MenuShopInfoPanel {
    short textOffset[2];
    unsigned char unmodeled_04[8];
    const char *text;
} MenuShopInfoPanel;

typedef struct MenuShopInfoWork {
    unsigned char unmodeled_00[2];
    unsigned char flag;
    unsigned char unmodeled_03;
    int unmodeled_04;
    int ticker;
    WindowDX window;
    MenuShopInfoPanel panel;
    unsigned char unmodeled_1b0[416];
    char message[SHOP_INFO_MESSAGE_SIZE];
} MenuShopInfoWork;

typedef struct MenuShopInfoT0 { const char *p[4]; } MenuShopInfoT0;

typedef struct MenuShopInfoT16 { const char *p[4]; } MenuShopInfoT16;

typedef struct MenuShopInfoT32 { const char *p[3]; } MenuShopInfoT32;

typedef struct MenuShopInfoT48 { const char *p[4]; } MenuShopInfoT48;

typedef struct MenuShopInfoT64 { const char *p[3]; } MenuShopInfoT64;

typedef struct MenuShopInfoT80 { const char *p[8]; } MenuShopInfoT80;

typedef struct MenuShopInfoT112 { const char *p[2]; } MenuShopInfoT112;

struct MenuShopListDisplay {
    unsigned char state;
    unsigned char active;
    unsigned char initialFlags[2];
    unsigned int category;
    int color;
    WindowSPCursor window;
};

struct MenuShopPortrait {
    signed char state;
    unsigned char unmodeled_01[3];
    short x;
    short y;
    int style;
    signed char red;
    signed char green;
    signed char blue;
    signed char alpha;
    unsigned char unmodeled_10[0x14];
    short spriteId;
};

struct MenuShopUnitPanel {
    unsigned char state;
    unsigned char active;
    unsigned char selectionFlags;
    unsigned char flags;
    int depth;
    ERibbonMainData ribbon;
    ETagFontMainData label;
    struct MenuShopPortrait portraits[7];
    unsigned char unmodeled_1b0[4];
    ECursolMainData cursor;
};

struct MenuShopIconIds {
    short ids[SHOP_ICON_COUNT];
};

struct MenuShopIconSprite {
    signed char state;
    unsigned char unmodeled_01[3];
    short x;
    short y;
    int depth;
    signed char red;
    signed char green;
    signed char blue;
    unsigned char alpha;
    unsigned char unmodeled_10[0x14];
    short spriteId;
};

struct MenuShopIconDisplay {
    signed char state;
    unsigned char unmodeled_01;
    unsigned char slide;
    unsigned char phase;
    short animation;
    short active;
    signed char categoryLift[SHOP_CAT_COUNT];
    unsigned char unmodeled_11[3];
    int depth;
    float scale;
    struct MenuShopIconSprite sprites[SHOP_ICON_COUNT];
};

struct MenuShopParameterDisplay {
    unsigned char state;
    unsigned char active;
    unsigned char count;
    unsigned char twoColumns;
    int depth;
    WindowDX window;
    ETagFontMainData labels[9];
    ENumberMainData numbers[9];
};

struct MenuShopParameterDigits8 { unsigned char digits[8]; };

struct MenuShopParameterDigits7 { signed char digits[7]; };

struct MenuShopExBullet {
    unsigned char unmodeled_00[4];
    int depth;
    WindowDX window;
    EMessageParam message;
};

struct MenuShopExHeader {
    ERibbonMainData ribbon;
    ETagFontMainData label;
    ENumberMainData number;
};

struct MenuShopExPortrait {
    signed char state;
    unsigned char unmodeled_01[3];
    short x;
    short y;
    int depth;
    signed char red;
    signed char green;
    signed char blue;
    signed char alpha;
    unsigned char unmodeled_10[0x14];
    short spriteId;
};

struct MenuShopExParty {
    ERibbonMainData ribbon;
    ETagFontMainData label;
    struct MenuShopExPortrait sprites[7];
    unsigned char unmodeled_1a8[4];
};

struct MenuShopExDisplay {
    unsigned char state;
    unsigned char active;
    unsigned char flags;
    unsigned char slideFlags;
    int depth;
    struct MenuShopExHeader headers[2];
    struct MenuShopExParty parties[2];
    struct MenuShopExHeader equipment;
    struct MenuShopExBullet bullet;
};

struct MenuShopSelectConfirm {
    WindowDX window;
    MenuTopMenuPanel panel;
};

struct MenuShopSelectList {
    WindowDX window;
    MenuTopMenuPanel panel;
    MenuTopCommandEntry commands[8];
};

struct MenuShopSelectDisplay {
    unsigned char state;
    unsigned char unmodeled_01[2];
    unsigned char flags;
    int depth;
    struct MenuShopSelectList lists[2];
    struct MenuShopSelectConfirm confirmation[2];
};

typedef struct MenuShopSeisan02Work {
    unsigned char state;
    unsigned char flag;
    unsigned char kind;
    unsigned char unmodeled_03;
    int ticker;
    WindowDX window;
    ETagFontMainData tags[2];
    ENumberMainData numbers[3];
    ESprite sprite;
    ERibbonPoint points[2];
    unsigned char unmodeled_972[4];
    int unmodeled_976;
    unsigned char unmodeled_980[4];
} MenuShopSeisan02Work;

struct MenuShopSaleSprite {
    signed char state;
    unsigned char unmodeled_01[3];
    short x;
    short y;
    int style;
    unsigned char unmodeled_0c[0x18];
    short spriteId;
};

struct MenuShopSaleVertex {
    short x;
    short y;
    int depth;
    signed char red;
    signed char green;
    signed char blue;
    signed char alpha;
};

struct MenuShopSalePanel {
    unsigned char state;
    unsigned char active;
    unsigned char flags;
    unsigned char selectionFlags;
    int depth;
    WindowDX window;
    ETagFontMainData labels[4];
    ENumberMainData numbers[6];
    struct MenuShopSaleSprite sprites[2];
    struct MenuShopSaleVertex quantityDivider[2];
    unsigned char unmodeled_5e4[4];
    int quantityDividerState;
    unsigned char unmodeled_5ec[4];
    struct MenuShopSaleVertex moneyDivider[2];
    unsigned char unmodeled_608[4];
    int moneyDividerState;
    unsigned char unmodeled_610[4];
    ERibbonMainData ribbon;
};

typedef enum MenuShopListIndex {
    MENU_SHOP_LIST_NONE = -1
} MenuShopListIndex;

/* Both panes hold the MenuTopMenuPanel that MenuSelectWindow reads. */
typedef struct MenuShopCommandPane {
    WindowDX window;
    MenuTopMenuPanel panel;
    MenuTopCommandEntry commands[8];
} MenuShopCommandPane;

typedef struct MenuShopChoicePane {
    WindowDX window;
    MenuTopMenuPanel panel;
} MenuShopChoicePane;

typedef struct MenuShopSelectWork {
    unsigned char unmodeled_00[3];
    unsigned char popupFlags;
    int depth;
    MenuShopCommandPane menus[2];
    MenuShopChoicePane choices[1];
} MenuShopSelectWork;

typedef struct MenuShopModelTask {
    TskObject base;
    int modelId;
} MenuShopModelTask;

typedef struct MenuShopHelpWork {
    unsigned char unmodeled_00[4];
    short x;
    short y;
    int depth;
    WindowDX window;
    EMessageParam messages[16];
    MenuShopPasBounds bounds;
} MenuShopHelpWork;

/* The display is seven halfwords; foreign MenuAgwsPara uses the same layout. */
typedef struct AgwsParaDisplay {
    short value[7];
} AgwsParaDisplay;

/* Partial equipment records: the four contiguous values are displayed by
 * frame selection, and the engine selection reads the first value at +8. */
typedef struct MenuShopEquipmentValues {
    unsigned char unmodeled_00[8];
    unsigned short value[4];
} MenuShopEquipmentValues;

typedef struct MenuShopEngineValue {
    unsigned char unmodeled_00[8];
    unsigned short value;
} MenuShopEngineValue;

/* PrintLine consumes x/y, depth and RGBA records until a zero depth. */
typedef struct MenuShopDividerVertex {
    short x;
    short y;
    int depth;
    union {
        unsigned char color[4];
        struct {
            unsigned char r;
            unsigned char g;
            unsigned char b;
            unsigned char a;
        } rgba;
    };
} MenuShopDividerVertex;

typedef struct MenuShopDividerWork {
    unsigned char unmodeled_00[3];
    unsigned char alpha;
    int depth;
    MenuShopDividerVertex vertices[6];
} MenuShopDividerWork;

typedef struct MenuShopSelection {
    u8 mode;
    u8 category;
    s8 quantity;
    u8 quantityAction;
    u8 quantityLimit;
} MenuShopSelection;

typedef struct MenuShopItem {
    int listIndex;
    int itemId;
    int modelId;
} MenuShopItem;

typedef struct MenuShopAgws {
    short unitId;
    short equipmentId;
    signed char equipmentStatus;
    signed char unitCount;
    signed char allPartyCount;
    signed char unitSelection;
    signed char sort_mode_selector;
} MenuShopAgws;

typedef struct MenuShopParty {
    short unitIds[6];
} MenuShopParty;

extern const char menuShopBulletTitleText[];
extern int MenuScenarioNo;
extern unsigned char MenuKeepSelect[0x64];
extern const MenuShopSortTable menuShopRecordSortTable;
extern const char menuShopDebugModeFormat[];
extern const char menuShopDebugCategoryFormat[];
extern const char menuShopDebugListFormat[];
extern const char menuShopDebugUnitSelectionFormat[];
extern u8 menuShopBulletTailFirst[];
extern u8 menuShopBulletTailSecond[];
extern u8 menuShopBulletTailThird[];
extern u8 menuShopBulletTailFourth[];
extern const char * const menuShopMainPromptTexts[4];
extern const char * const menuShopQuantityPromptTexts[4];
extern const char * const menuShopUnitCountPromptTexts[3];
extern const char * const menuShopFramePromptTexts[4];
extern const char * const menuShopPurchasedPromptTexts[3];
extern const char * const menuShopExchangePromptTexts[8];
extern const char *menuShopMakerNames[];
extern const char menuShopBlankText[];
extern const char menuShopPeriodText[];
extern char menuShopPeriodEndText[];
extern const char menuShopTargetTagText[];
extern const struct MenuShopIconIds menuShopIconSpriteTable;
extern const float menuShopIconScaleGrowStep;
extern const float menuShopIconScaleShrinkStep;
extern const struct MenuShopParameterDigits8 menuShopCharacterDigitsTable[];
extern struct MenuShopParameterDigits7 menuShopAgwsDigitsTable[];
static const char * msg01_8_0036DBA0[] ;
extern const char menuShopPayText[];
extern const char menuShopMenuTagText[];
extern const char menuShopSelectTagText[];
extern const struct MenuShopSortOptionTable menuShopSortOptionTable;
extern const struct MenuShopRowCountTable menuShopRowCountTable;
static const char * msg00_4_0036DB60[] ;
extern const DoublewordAlignedVector4 menuShopModelCameraVector;
extern void *MenuModelOut[4];
extern AgwsParaDisplay MenuShopPara;
extern AgwsParaDisplay MenuShopPara2;
extern AgwsParaDisplay MenuAgwsPara;
extern const char menuShopHyamsMakerText[];
extern const char menuShopVectorMakerText[];
extern const char menuShopAgwsWeaponsText[24];
extern const char menuShopAgwsAmmoText[16];
extern const char MenuShopSellText[5];
extern const char MenuShopItemsText[6];
extern const char menuShopBuyFrameText[16];
extern const char menuShopCharacterAccessoriesText[24];
extern const char MenuShopCardsText[6];
extern const char menuShopFhpRestorationText[16];
extern const char menuShopAgwsText[16];
extern const char menuShopBuyGeneratorText[16];
extern const char menuShopCharacterAmmoText[16];
extern const char menuShopAgwsTuningText[16];
extern const char menuShopCharacterWeaponsText[24];
extern const char menuShopAgwsAccessoriesText[24];
extern const char MenuShopStoreText[5];
extern const char MenuShopBuyText[4];
extern const char menuShopQuitText[8];
extern const char menuShopBuyGeneratorCommandText[16];
extern const char menuShopRestoreFhpText[16];
extern const char menuShopSellItPromptText[16];
extern const char menuShopBuyItPromptText[24];
extern const char menuShopYesNoText[8];
extern const char menuShopRestoreItPromptText[16];
extern const char menuShopStockColumnText[16];
extern const char menuShopPriceColumnText[16];
extern const char menuShopMoneyText[8];
extern const char menuShopTargetText[8];
extern const char menuShopPriceText[8];
extern const char menuShopEquipText[8];
extern const char menuShopStockText[8];
extern const char menuShopResultText[8];
extern const char menuShopTotalText[8];
extern const char menuShopAglText[8];
extern const char menuShopEatkText[8];
extern const char menuShopVitText[8];
extern const char menuShopDexText[8];
extern const char menuShopStrText[8];
extern const char menuShopEvaText[8];
extern const char menuShopEdefText[8];
extern const char menuShopParmText[8];
extern const char menuShopFhpText[8];
extern const char menuShopDpowText[8];
extern const char menuShopWaglText[8];
extern const char menuShopEarmText[8];
extern const char menuShopTuneAgwsText[16];
extern const char menuShopBuyItemsWeaponsText[24];
extern const char menuShopSellItemsWeaponsText[24];
extern const char MenuShopQuitText[6];
extern const char menuShopQuantityPromptText[32];
extern const char MenuShopSelectText[8];
extern const char menuShopSellItemsText[16];
extern const char menuShopBuyItemsText[16];
extern const char menuShopRestoreFhpDescriptionText[24];
extern const char menuShopBuyGeneratorDescriptionText[32];
extern const char menuShopBuyFrameDescriptionText[24];
extern const char MenuShopIsText[];
extern const char MenuShopGoldUnitText[];
extern const char MenuShopSoldText[];
extern const char MenuShopPeriodText[];

extern void *MenuSortAddrGet(int list);
extern int MenuSortGet(int list, int index);
extern MenuListEntry *MenuListGet(int list);
extern int MenuSortCheck(int list);
int subMenuShopEquipCheck(int item, int category);
int MenuShopNoSaleCheck(int item);
extern void MoveSlide(short *current, short *target, float rate);
extern MenuShopWeapon *func_A1A3D8(int item);
extern const char **func_A2C738(int bullet);
extern MenuListEntry *MenuListMake(int list, int mode);
void MenuShopSortSet(int list, int type, int order, int option);
extern void MenuModelUnitOpen(MenuModelUnit *unit, int drawType);
void *func_A191C0(int);
extern int MenuShopListChange00(int list);
ShopCalculatedPara *func_00A11108(int, int *, int *);
void MenuAgwsParaSet(int, int);
s32 MenuEngineEquipCheck(int, int);
s32 MenuFrameEquipCheck(s32, s32);
extern MenuShopAccessoryData *func_A1A548(int item);
int MenuBoxDec(unsigned int);
int MenuBoxInc(unsigned int);
s32 MenuCharHpCheck(s16);
s32 MenuCursorKeepCheck(void);
int MenuSelectMove(int, int, int);
int MenuSelectMove2(int, int, int);
void xglFontDebugHex(int, int, unsigned int, int);
extern MenuShopWeapon *dataWpnGet(int item);
extern MenuShopAttackData *dataAttGet(int item);
extern MenuShopItemData *func_A1A4E8(int item);
void WindowSPSet(WindowSPCursor *cursor);
void WindowSPMain(WindowSPCursor *cursor);
void WindowSPSelectJump(WindowSPCursor *cursor, int selection);

/* Kept after the typedefs and prototypes above that spell these names. */
#define s8 signed char
#define s32 int
#define s64 long long

extern MenuTextEntry *func_A2C918(unsigned int itemId);
extern MenuTextEntry *func_A2C968(unsigned int itemId);
extern const char *MenuCharNameGet(int unitId);
extern const char *MenuNumberTextGet(long value, int digits, int flag);
extern void eSpriteSet(void *sprite, short spriteId);
extern void eSpriteMain(void *sprite);
static void MenuShopList(TskObject *task, void *data);
static void MenuShopSelect(TskObject *task, void *data);
static void MenuShopEx02(TskObject *task, void *data);
static void MenuShopIcon(TskObject *task, void *data);
static void MenuShopModelMain(TskObject *task, void *data);
extern void MenuModelCreate(void **slot, int partyId);
extern void MenuModelExtFuncSet(void *, void (*)(MenuModelUnit *), int);
extern MenuShopEquipmentValues *func_A1A598(int item);
extern MenuShopEngineValue *func_A1A5E8(int item);
extern unsigned short *func_A1A428(int item);
static void MenuShopParameter(TskObject *task, void *data);

/* Referenced original data recovered from this translation unit. */
const char menuShopPriceListTitleText[40] = "List                   Price";

static const char * msg00_9[7];

static const char * msg01_10[6];

static const char * msg00_6[3];

static const char * msg01_7[4];

static const char * msg00_1_0036DB30[8];

static const char * msg01_2_0036DB50[4];

static const char * msg00_5_0036DB68[5];

static const char * msg00_0_0036DAF0[16];

static const char * msg00_0_0036DAF0[16] = { MenuShopStoreText, MenuShopBuyText, MenuShopSellText, MenuShopItemsText, menuShopCharacterWeaponsText, menuShopCharacterAccessoriesText, menuShopCharacterAmmoText, menuShopAgwsWeaponsText, menuShopAgwsAccessoriesText, menuShopAgwsAmmoText, menuShopAgwsText, MenuShopCardsText, menuShopAgwsTuningText, menuShopFhpRestorationText, menuShopBuyFrameText, menuShopBuyGeneratorText };

static const char * msg00_1_0036DB30[8] = { MenuShopBuyText, MenuShopSellText, menuShopAgwsTuningText, menuShopQuitText, menuShopBuyFrameText, menuShopBuyGeneratorCommandText, menuShopRestoreFhpText, menuShopQuitText };

static const char * msg01_2_0036DB50[4] = { menuShopBuyItPromptText, menuShopSellItPromptText, menuShopRestoreItPromptText, menuShopYesNoText };

static const char * msg00_4_0036DB60[] = { menuShopPriceColumnText, menuShopStockColumnText };

static const char * msg00_5_0036DB68[5] = { menuShopStockText, menuShopMoneyText, menuShopTargetText, menuShopEquipText, menuShopPriceText };

static const char * msg00_6[3] = { MenuShopBuyText, MenuShopSellText, menuShopPayText };

static const char * msg01_7[4] = { menuShopStockText, menuShopTotalText, menuShopMoneyText, menuShopResultText };

static const char * msg01_8_0036DBA0[] = { menuShopMoneyText, menuShopResultText };

static const char * msg00_9[7] = { menuShopStrText, menuShopVitText, menuShopEatkText, menuShopEdefText, menuShopDexText, menuShopEvaText, menuShopAglText };

static const char * msg01_10[6] = { menuShopFhpText, menuShopDpowText, menuShopParmText, menuShopEarmText, menuShopAglText, menuShopWaglText };

AgwsParaDisplay MenuShopPara = { .value = { 0, 0, 0, 0, 0, 0, 0 } };

AgwsParaDisplay MenuShopPara2 = { .value = { 0, 0, 0, 0, 0, 0, 0 } };

static MenuShopWorkData *MenuShopWork;

static MenuShopData ShopData;

static WindowSPCursor *MenuShopWinSP;

static XglTaskScheduler * MenuShopTask;

static unsigned char * ShopDataBufTop;

static inline MenuShopWorkData *MenuShopWorkRead(void)
{
    return *(MenuShopWorkData *const *)&MenuShopWork;
}

static inline WindowSPCursor *MenuShopWinSPRead(void)
{
    return *&MenuShopWinSP;
}

static inline void *MenuShopAlignBuffer(unsigned char *buffer,
    unsigned int alignment)
{
    unsigned int address = (unsigned int)buffer;

    return (void *)((address + alignment - 1) & ~(alignment - 1));
}

static inline void MenuShopAppendPartyId(MenuShopWorkData *work, int *nextSlot,
    int **source)
{
    short *destination = &work->partyIds[*nextSlot];

    ++*nextSlot;
    *destination = (short)*(*source)++;
}

static inline void MenuShopAppendUnitId(MenuShopWorkData *work, int *nextSlot,
    int **source)
{
    short *destination = &work->unitIds[*nextSlot];

    ++*nextSlot;
    *destination = (short)*(*source)++;
}

void MenuShopAgwsListChange(void)
{
    int i;
    int next;

    for (i = 0; i < 4; i++) {
        if (ShopData.tail.agwsSlots[i] != 0 &&
            PartyTakeAgwsCheck(ShopData.tail.agwsSlots[i]) != 0)
            ShopData.tail.agwsSlots[i] = 0;
    }

    for (i = 0; i < 3; i++) {
        if (!ShopData.tail.agwsSlots[i]) {
            for (next = i + 1; next < 4; next++) {
                if (ShopData.tail.agwsSlots[next] != 0) {
                    ShopData.tail.agwsSlots[i] = ShopData.tail.agwsSlots[next];
                    ShopData.tail.agwsSlots[next] = 0;
                    break;
                }
            }
        }
    }
}

static void TskObjectSet3(TskObject *task, TskObjectWorker worker, void *data)
{
    task->data = data;
    task->worker = worker;
    task->state = 0;
}

static void tskTskMain3(TskObject *task)
{
    TskObjectWorker worker = task->worker;

    if (MenuShopWork->state == SHOP_STATE_END) {
        xglTaskWaitRemove(&task->base);
        return;
    }

    if (task->state != 0) {
        if (task->state != 2)
            return;
    } else {
        worker(task, task->data);
        task->state = 2;
    }

    worker(task, task->data);
}

void MenuShopModelDisp(MenuModelUnit *unit)
{
    Actor *actor = unit->actor;
    ActorMenuControl *transform;
    int state;

    if (actor == 0) {
        if (MenuShopWork->modelPending != 0) {
            MenuModelUnitBreak(unit);
            MenuShopWork->modelPending = 0;
        }
    }

    if (unit->active != 0) {
        transform = &actor->menu;
        if (MenuShopWork->modelPending != 0) {
            unit->extFuncFlag = SHOP_MODEL_SWAP;
            MenuShopWork->modelPending = 0;
        }

        state = unit->extFuncFlag;
        switch (state) {
        case SHOP_MODEL_SHOWN:
            break;
        case SHOP_MODEL_OPEN:
            transform->rotation.y = -0.2f;
            MenuModelUnitOpen(unit, 2);
            unit->extFuncFlag = SHOP_MODEL_SHOWN;
            return;
        case SHOP_MODEL_SWAP:
            MenuModelUnitOpen(unit, 1);
            unit->extFuncFlag = SHOP_MODEL_FADE_OUT;
            /* fall through */
        case SHOP_MODEL_FADE_OUT:
            if (transform->transparency == 0.0f)
                unit->taskPhase = -1;
            break;
        default:
            break;
        }
    }
}

static void MenuShopModelMain(TskObject *task, void *data)
{
    MenuShopModelTask *model = (MenuShopModelTask *)task;
    DoublewordAlignedVector4 position;
    DoublewordAlignedVector4 rotation;
    StudioCamera *camera;
    StudioCamera *camera2;
    MenuShopWorkData *work;
    int *modelId;
    int category;
    int enabled;

    modelId = &model->modelId;
    enabled = MenuShopWork->modelBreakPending;
    work = MenuShopWork;
    if (enabled == 0)
        return;

    switch (task->state) {
    case 0:
        *modelId = 0;
        position = menuShopModelCameraVector;
        MenuModelOut[0] = 0;
        memset(&rotation, 0, sizeof(rotation));
        rotation.w = 1.0f;
        camera = xglStudioGetCamera2(0);
        xglCameraInit(camera);
        camera->state = 1;
        camera->rotation.vector = rotation;
        camera->position.vector = position;
        camera2 = xglStudioGetCamera2(2);
        camera2->active = 1;
        camera2->state = 1;
        break;
    case 2:
        if (MenuShopWork->mode != 0)
            break;
        switch (MenuShopWork->state) {
        case SHOP_STATE_BUY_LIST:
        case SHOP_STATE_QUANTITY:
        case SHOP_STATE_CONFIRM:
        case SHOP_STATE_RESULT:
            category = MenuShopWork->category;
            if (category == SHOP_CAT_AGWS) {
                if ((MenuShopWork->itemId >> 16) != category)
                    break;
                if (*modelId != MenuShopWork->modelId) {
                    MenuShopWork->modelPending = 1;
                    *modelId = MenuShopWork->modelId;
                    work = MenuShopWorkRead();
                }
                if (work->modelId == 0)
                    break;
                if (MenuModelOut[0] != 0)
                    break;
                work->modelPending = 0;
                MenuModelCreate(MenuModelOut, work->modelId);
                MenuModelExtFuncSet(MenuModelOut[0], MenuShopModelDisp, 0);
                *modelId = MenuShopWork->modelId;
                break;
            }
            /* fall through */
        default:
            MenuShopWork->modelPending = 1;
            break;
        }
        break;
    }
}

static void MenuShopPas(TskObject *task, void *data)
{
    MenuShopHelpWork *work = data;
    short messageX[16];
    short windowX;
    int i;
    WindowDX *window;
    short *target;
    const char **text;

    switch (task->state) {
    case 0:
        work->x = SHOP_PAS_WINDOW_HIDDEN_X;
        work->y = 8;
        work->depth = SHOP_DEPTH_TOP;
        window = &work->window;
        WindowDXSet(window);
        window->x = work->x;
        work->window.style = work->depth;
        work->window.width = SHOP_PAS_WINDOW_WIDTH;
        work->window.y = work->y;
        work->window.height = 32;
        work->window.state = WINDOW_STATE_OPEN_REQUEST;
        WindowDXMain(window);
        work->window.state = WINDOW_STATE_OPEN;
        text = msg00_0_0036DAF0;
        for (i = 0; i < SHOP_PAS_LABEL_COUNT; i++, text++) {
            eMessageSet(&work->messages[i], *text);
            work->messages[i].mode = 32;
            work->messages[i].x = SHOP_PAS_LABEL_HIDDEN_X;
            work->messages[i].draw_parameter = work->depth + 2;
            work->messages[i].y = work->y + 3;
        }
        break;
    case 2:
        windowX = SHOP_PAS_WINDOW_SHOWN_X;
        for (i = 0; i < SHOP_PAS_LABEL_COUNT; i++)
            messageX[i] = SHOP_PAS_LABEL_HIDDEN_X;
        switch (MenuShopWork->state) {
        case SHOP_STATE_MODE_SELECT:
            messageX[SHOP_PAS_LABEL_STORE] = SHOP_PAS_LABEL_SHOWN_X;
            break;
        case SHOP_STATE_BUY_LIST:
        case SHOP_STATE_SELL_LIST:
        case SHOP_STATE_QUANTITY:
        case SHOP_STATE_CONFIRM:
        case SHOP_STATE_RESULT:
            messageX[MenuShopWork->category + SHOP_PAS_LABEL_CATEGORY_FIRST] = SHOP_PAS_LABEL_SHOWN_X;
            break;
        case SHOP_STATE_AGWS_MENU:
            messageX[SHOP_PAS_LABEL_AGWS_TUNING] = SHOP_PAS_LABEL_SHOWN_X;
            break;
        case SHOP_STATE_RESTORE_UNIT:
        case SHOP_STATE_RESTORE_CONFIRM:
        case SHOP_STATE_RESTORE_DONE:
            messageX[SHOP_PAS_LABEL_FHP_RESTORATION] = SHOP_PAS_LABEL_SHOWN_X;
            break;
        case SHOP_STATE_AGWS_BUY_LIST:
        case SHOP_STATE_AGWS_BUY_UNIT:
        case SHOP_STATE_AGWS_BUY_CONFIRM:
        case SHOP_STATE_AGWS_BUY_DONE:
            if (MenuShopWork->sort_mode_selector != 0)
                messageX[SHOP_PAS_LABEL_BUY_FRAME] = SHOP_PAS_LABEL_SHOWN_X;
            else
                messageX[SHOP_PAS_LABEL_BUY_GENERATOR] = SHOP_PAS_LABEL_SHOWN_X;
            break;
        default:
            windowX = SHOP_PAS_WINDOW_HIDDEN_X;
            break;
        }
        window = &work->window;
        MoveSlide(&window->x, &windowX, SHOP_SLIDE_RATE);
        WindowDXMain(window);
        work->bounds.x1 = window->x + 3;
        work->bounds.y1 = work->window.y + 3;
        work->bounds.x2 = work->window.width - 6;
        work->bounds.y2 = work->window.height - 6;
        work->bounds.color = work->depth;
        endPrintExtFunc(work->depth, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_SCISSOR_SET,&work->bounds);
        target = messageX;
        for (i = 0; i < 16; i++, target++) {
            MoveSlide(&work->messages[i].x, target, SHOP_SLIDE_RATE);
            if (work->messages[i].x < 256)
                eMessageMain(&work->messages[i]);
        }
        endPrintExtFunc(work->depth, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_SCISSOR_RESET, 0);
        break;
    }
}

static void MenuShopInfo(TskObject *task, void *data)
{
    MenuShopInfoWork *work = data;
    MenuShopInfoT0 tab0;
    MenuShopInfoT16 tab16;
    MenuShopInfoT32 tab32;
    MenuShopInfoT48 tab48;
    MenuShopInfoT64 tab64;
    MenuShopInfoT80 tab80;
    MenuShopInfoT112 tab112;
    short targetY;
    short targetH;
    MenuShopInfoPanel *panel;
    unsigned char *unit;
    short value;

    tab0 = *(MenuShopInfoT0 *)menuShopMainPromptTexts;
    tab16 = *(MenuShopInfoT16 *)menuShopQuantityPromptTexts;
    tab32 = *(MenuShopInfoT32 *)menuShopUnitCountPromptTexts;
    tab48 = *(MenuShopInfoT48 *)menuShopFramePromptTexts;
    tab64 = *(MenuShopInfoT64 *)menuShopPurchasedPromptTexts;
    tab80 = *(MenuShopInfoT80 *)menuShopExchangePromptTexts;
    tab112 = *(MenuShopInfoT112 *)menuShopMakerNames;

    switch (task->state) {
    case 0:
        work->ticker = SHOP_DEPTH_TOP;
        WindowDXSet(&work->window);
        work->window.x = -16;
        work->window.y = SHOP_INFO_HIDDEN_Y;
        work->window.width = 544;
        work->window.height = SHOP_INFO_HEIGHT;
        work->window.style = work->ticker;
        work->window.close_callback = MenuInfoWindow;
        work->window.state = WINDOW_STATE_OPEN_REQUEST;
        panel = &work->panel;
        work->window.close_callback_arg = panel;
        WindowDXMain(&work->window);
        work->window.state = WINDOW_STATE_OPEN;
        panel->textOffset[0] = 0;
        work->panel.textOffset[1] = 0;
        work->panel.text = 0;
        work->flag = 0;
        work->unmodeled_04 = 0;
        memset(work->message, 0, SHOP_INFO_MESSAGE_SIZE);
        return;
    case 2:
        break;
    default:
        return;
    }

    targetY = SHOP_INFO_SHOWN_Y;
    targetH = SHOP_INFO_HEIGHT;
    switch (MenuShopWork->state) {
    case SHOP_STATE_MODE_SELECT:
        eMessageCpy(work->message, tab0.p[MenuShopWork->mode]);
        break;
    case SHOP_STATE_BUY_LIST:
    case SHOP_STATE_SELL_LIST:
        if (MenuShopWork->itemId == 0) {
            eMessageCpy(work->message, menuShopBlankText);
        } else if ((MenuShopWork->itemId >> 16) == ITEM_KIND_AGWS) {
            if (MenuShopWork->modelId >= SHOP_AGWS_VX_FIRST_ID)
                eMessageCpy(work->message, tab112.p[0]);
            else
                eMessageCpy(work->message, tab112.p[1]);
        } else {
            eMessageCpy(work->message, MenuTextGet(MenuShopWork->itemId)->auxiliary);
        }
        break;
    case SHOP_STATE_QUANTITY:
        if (MenuShopWork->itemId != 0) {
            eMessageCpy(work->message, tab16.p[2]);
            eMessageCat(tab16.p[3]);
        }
        break;
    case SHOP_STATE_CONFIRM:
        if (MenuShopWork->itemId != 0) {
            long money;

            eMessageCpy(work->message, MenuNumberTextGet(MenuShopWork->quantity, 2, 0));
            eMessageCat(tab32.p[2]);
            eMessageCat(MenuTextGet(MenuShopWork->itemId)->name);
            eMessageCat(tab32.p[0]);
            switch (MenuShopWork->mode) {
            case SHOP_MODE_BUY:
                money = MenuBoxMoneyGet(MenuShopWork->itemId, 0) * MenuShopWork->quantity;
                break;
            default:
                money = MenuBoxMoneyGet(MenuShopWork->itemId, 1) * MenuShopWork->quantity;
                break;
            }
            eMessageCat(MenuNumberTextGet(money, 10, 0));
            eMessageCat(tab32.p[1]);
        }
        break;
    case SHOP_STATE_RESULT:
        if (MenuShopWork->itemId != 0) {
            eMessageCpy(work->message, MenuNumberTextGet(MenuShopWork->quantity, 2, 0));
            eMessageCat(tab64.p[0]);
            eMessageCat(MenuTextGet(MenuShopWork->itemId)->name);
            eMessageCat(menuShopBlankText);
            eMessageCat(tab64.p[MenuShopWork->mode + 1]);
        }
        break;
    case SHOP_STATE_AGWS_MENU:
        eMessageCpy(work->message, tab48.p[MenuShopWork->sortOption]);
        break;
    case SHOP_STATE_RESTORE_UNIT:
    case SHOP_STATE_RESTORE_CONFIRM:
        eMessageCpy(work->message, MenuCharNameGet(MenuShopWork->unitId));
        eMessageCat(tab80.p[0]);
        break;
    case SHOP_STATE_RESTORE_DONE:
        eMessageCpy(work->message, MenuCharNameGet(MenuShopWork->unitId));
        eMessageCat(tab80.p[7]);
        break;
    case SHOP_STATE_AGWS_BUY_LIST:
        if (MenuShopWork->listIndex < 0 || MenuShopWork->equipmentId == 0) {
            eMessageCpy(work->message, menuShopBlankText);
        } else {
            eMessageCpy(work->message, func_A2C968(MenuShopWork->equipmentId)->auxiliary);
        }
        break;
    case SHOP_STATE_AGWS_BUY_UNIT:
    case SHOP_STATE_AGWS_BUY_CONFIRM:
        if (MenuShopWork->listIndex >= 0) {
            unit = func_A191C0(MenuShopWork->unitId);
            eMessageCpy(work->message, MenuCharNameGet(MenuShopWork->unitId));
            eMessageCat(tab80.p[1]);
            if (MenuShopWork->sort_mode_selector != 0) {
                value = *(short *)(unit + SHOP_UNIT_FRAME_ID_OFFSET);
                if (value != 0)
                    eMessageCat(func_A2C968(value)->name);
                eMessageCat(tab80.p[2]);
                eMessageCat(tab80.p[3]);
                if (MenuShopWork->equipmentId != 0)
                    eMessageCat(func_A2C968(MenuShopWork->equipmentId)->name);
            } else {
                value = *(short *)(unit + SHOP_UNIT_ENGINE_ID_OFFSET);
                if (value != 0)
                    eMessageCat(func_A2C918(value)->name);
                eMessageCat(tab80.p[2]);
                eMessageCat(tab80.p[3]);
                if (MenuShopWork->equipmentId != 0)
                    eMessageCat(func_A2C918(MenuShopWork->equipmentId)->name);
            }
            eMessageCat(menuShopPeriodText);
        } else {
            eMessageCpy(work->message, menuShopBlankText);
        }
        break;
    case SHOP_STATE_AGWS_BUY_DONE:
        eMessageCpy(work->message, MenuCharNameGet(MenuShopWork->unitId));
        if (MenuShopWork->sort_mode_selector != 0) {
            eMessageCat(tab80.p[4]);
            eMessageCat(tab80.p[6]);
            if (MenuShopWork->equipmentId != 0)
                eMessageCat(func_A2C968(MenuShopWork->equipmentId)->name);
        } else {
            eMessageCat(tab80.p[5]);
            eMessageCat(tab80.p[6]);
            if (MenuShopWork->equipmentId != 0)
                eMessageCat(func_A2C918(MenuShopWork->equipmentId)->name);
        }
        eMessageCat(menuShopPeriodEndText);
        break;
    default:
        targetY = SHOP_INFO_HIDDEN_Y;
        break;
    }
    MoveSlide(&work->window.y, &targetY, SHOP_SLIDE_RATE);
    MoveSlide(&work->window.height, &targetH, SHOP_SLIDE_RATE);
    work->panel.text = work->message;
    WindowDXMain(&work->window);
}

static void MenuShopSelect(TskObject *task, void *data)
{
    MenuShopSelectWork *work = data;

    MenuTopCommandEntry *ListMake(int menuIndex)
    {
        MenuShopCommandPane *pane = (MenuShopCommandPane *)(menuIndex * sizeof(MenuShopCommandPane) + (int)work + 8);

        memset(pane->commands, 0, sizeof(pane->commands));
        {
            int commandIndex;

            for (commandIndex = 0; commandIndex < SHOP_MENU_COMMAND_COUNT; commandIndex++) {
                work->menus[menuIndex].commands[commandIndex].name =
                    msg00_1_0036DB30[menuIndex * SHOP_MENU_COMMAND_COUNT + commandIndex];
                work->menus[menuIndex].commands[commandIndex].locked = 0;
            }
            work->menus[menuIndex].commands[commandIndex].name = 0;
        }
        if (menuIndex == SHOP_MENU_MAIN) {
            if (MenuShopWork->noCategories != 0)
                work->menus[menuIndex].commands[SHOP_MODE_BUY].locked = 1;
            if (MenuShopWork->equipmentStatus == 0)
                work->menus[menuIndex].commands[SHOP_MODE_TUNE_AGWS].locked = 1;
        }
        return work->menus[menuIndex].commands;
    }

    int i;

    switch (task->state) {
    case 0:
        work->depth = SHOP_DEPTH_SELECT;
        for (i = 0; i < 2; i++) {
            WindowDXSet(&work->menus[i].window);
            work->menus[i].window.x = SHOP_PANEL_HIDDEN_X;
            work->menus[i].window.style = work->depth;
            if (i == SHOP_MENU_MAIN)
                work->menus[0].window.width = 205;
            else
                work->menus[i].window.width = 205;
            work->menus[i].window.height = 102;
            work->menus[i].window.close_callback_arg = &work->menus[i].panel;
            work->menus[i].window.close_callback = MenuSelectWindow;
            work->menus[i].window.tag_id = menuShopMenuTagText;
            work->menus[i].panel.cursor = 0;
            work->menus[i].panel.commands = ListMake(i);
            work->menus[i].panel.rows = 2;
            work->menus[i].window.state = WINDOW_STATE_OPEN_REQUEST;
            WindowDXMain(&work->menus[i].window);
            work->menus[i].window.state = WINDOW_STATE_OPEN;
        }
        for (i = 0; i < 1; i++) {
            WindowDXSet(&work->choices[i].window);
            work->choices[i].window.x = SHOP_PANEL_HIDDEN_X;
            work->choices[i].window.width = 169;
            work->choices[i].window.height = 102;
            work->choices[i].window.style = work->depth;
            work->choices[i].window.tag_id = menuShopSelectTagText;
            work->choices[i].panel.text = msg01_2_0036DB50[3];
            work->choices[i].panel.heading = msg01_2_0036DB50[0];
            work->choices[i].window.close_callback_arg = &work->choices[i].panel;
            work->choices[i].window.close_callback = MenuSelectWindow;
            work->choices[i].panel.cursor = 0;
            work->choices[i].panel.rows = 1;
            work->choices[i].window.state = WINDOW_STATE_OPEN_REQUEST;
            WindowDXMain(&work->choices[i].window);
            work->choices[i].window.state = WINDOW_STATE_OPEN;
        }
        work->popupFlags = 0;
        break;
    case 2: {
        short menuX[2];
        short choiceX[2];
        MenuShopWorkData *shop;

        for (i = 0; i < 2; i++) {
            unsigned char popupVisible;

            menuX[i] = SHOP_PANEL_HIDDEN_X;
            popupVisible = work->popupFlags & SHOP_POPUP_SLIDES_LEFT;
            if (popupVisible)
                choiceX[i] = -176;
            else
                choiceX[i] = SHOP_PANEL_HIDDEN_X;
        }
        shop = MenuShopWork;
        switch (shop->state) {
        case SHOP_STATE_MODE_SELECT:
            if (shop->stateFlags & SHOP_FLAG_STATE_ENTERED) {
                work->menus[SHOP_MENU_MAIN].window.x = SHOP_PANEL_HIDDEN_X;
                work->menus[SHOP_MENU_MAIN].window.y = 192;
            }
            menuX[SHOP_MENU_MAIN] = 152;
            work->menus[SHOP_MENU_MAIN].panel.cursor = shop->mode;
            break;
        case SHOP_STATE_AGWS_MENU:
            if (shop->stateFlags & SHOP_FLAG_STATE_ENTERED) {
                work->menus[SHOP_MENU_AGWS].window.x = SHOP_PANEL_HIDDEN_X;
                work->menus[SHOP_MENU_AGWS].window.y = 192;
            }
            menuX[SHOP_MENU_AGWS] = 160;
            work->menus[SHOP_MENU_AGWS].panel.cursor = shop->sortOption;
            break;
        case SHOP_STATE_CONFIRM: {
            unsigned char *mode = &shop->mode;
            unsigned char updatePopup;

            updatePopup = shop->stateFlags & SHOP_FLAG_STATE_ENTERED;

            if (updatePopup) {
                switch (*mode) {
                case SHOP_MODE_BUY:
                    work->choices[0].panel.heading = msg01_2_0036DB50[0];
                    switch (mode[1]) {
                    case SHOP_CAT_ITEM:
                        work->choices[0].window.x = SHOP_PANEL_HIDDEN_X;
                        work->choices[0].window.y = 256;
                        work->popupFlags &= ~SHOP_POPUP_SLIDES_LEFT;
                        break;
                    case SHOP_CAT_AGWS:
                        work->choices[0].window.x = -136;
                        work->choices[0].window.y = 208;
                        work->popupFlags |= SHOP_POPUP_SLIDES_LEFT;
                        break;
                    case SHOP_CAT_CARD:
                        work->choices[0].window.x = SHOP_PANEL_HIDDEN_X;
                        work->choices[0].window.y = 256;
                        work->popupFlags &= ~SHOP_POPUP_SLIDES_LEFT;
                        break;
                    default:
                        work->choices[0].window.x = -136;
                        work->choices[0].window.y = 80;
                        work->popupFlags |= SHOP_POPUP_SLIDES_LEFT;
                        break;
                    }
                    break;
                default:
                    work->choices[0].panel.heading = msg01_2_0036DB50[1];
                    switch (mode[1]) {
                    case SHOP_CAT_ITEM:
                    case SHOP_CAT_CARD:
                        work->choices[0].window.x = SHOP_PANEL_HIDDEN_X;
                        work->choices[0].window.y = 256;
                        work->popupFlags &= ~SHOP_POPUP_SLIDES_LEFT;
                        break;
                    case SHOP_CAT_CHAR_WEAPON:
                        work->choices[0].window.x = -136;
                        work->choices[0].window.y = 272;
                        work->popupFlags |= SHOP_POPUP_SLIDES_LEFT;
                        break;
                    case SHOP_CAT_AGWS_WEAPON:
                        work->choices[0].window.x = -136;
                        work->choices[0].window.y = 272;
                        work->popupFlags |= SHOP_POPUP_SLIDES_LEFT;
                        break;
                    case SHOP_CAT_AGWS:
                        work->choices[0].window.x = -136;
                        work->choices[0].window.y = 192;
                        work->popupFlags |= SHOP_POPUP_SLIDES_LEFT;
                        break;
                    default:
                        work->choices[0].window.x = -136;
                        work->choices[0].window.y = 80;
                        work->popupFlags |= SHOP_POPUP_SLIDES_LEFT;
                        break;
                    }
                    break;
                }
            }
            switch (MenuShopWork->category) {
            case SHOP_CAT_ITEM:
            case SHOP_CAT_CARD:
                choiceX[0] = 304;
                break;
            case SHOP_CAT_AGWS:
                choiceX[0] = 48;
                break;
            default:
                choiceX[0] = 32;
                break;
            }
            work->choices[0].panel.cursor = MenuShopWork->quantityAction;
            break;
        }
        case SHOP_STATE_RESTORE_CONFIRM:
            if (shop->stateFlags & SHOP_FLAG_STATE_ENTERED) {
                work->choices[0].panel.heading = msg01_2_0036DB50[2];
                work->choices[0].window.x = -136;
                work->choices[0].window.y = 192;
                work->popupFlags |= SHOP_POPUP_SLIDES_LEFT;
            }
            choiceX[0] = 88;
            work->choices[0].panel.cursor = MenuShopWork->quantityAction;
            break;
        case SHOP_STATE_AGWS_BUY_CONFIRM:
            if (shop->stateFlags & SHOP_FLAG_STATE_ENTERED) {
                work->choices[0].panel.heading = msg01_2_0036DB50[0];
                work->choices[0].window.x = SHOP_PANEL_HIDDEN_X;
                work->choices[0].window.y = 176;
                work->popupFlags &= ~SHOP_POPUP_SLIDES_LEFT;
            }
            choiceX[0] = 304;
            work->choices[0].panel.cursor = MenuShopWork->quantityAction;
            break;
        }
        for (i = 0; i < 2; i++) {
            MoveSlide(&work->menus[i].window.x, &menuX[i], SHOP_SLIDE_RATE);
            WindowDXMain(&work->menus[i].window);
        }
        MoveSlide(&work->choices[0].window.x, &choiceX[0], SHOP_SLIDE_RATE);
        WindowDXMain(&work->choices[0].window);
        break;
    }
    }
}

void MenuShopSortSet(int list, int type, int order, int option)
{
    MenuShopSortTable table;
    int *output = MenuSortAddrGet(list);

    table = menuShopRecordSortTable;

    if (type == SHOP_SORT_TYPE_FRAME)
        type = SHOP_TABLE_FRAME;
    if (type == SHOP_SORT_TYPE_ENGINE)
        type = SHOP_TABLE_ENGINE;

    if (type == SHOP_CAT_CARD) {
        if (MenuScenarioNo >= SHOP_CARD_SCENARIO_FIRST) {
            *output++ = ITEM_KEY_EVENT + SHOP_CARD_EVENT_1;
            *output++ = ITEM_KEY_EVENT + SHOP_CARD_EVENT_2;
        }
        if (MenuScenarioNo >= SHOP_CARD_SCENARIO_THIRD)
            *output++ = ITEM_KEY_EVENT + SHOP_CARD_EVENT_3;
    } else {
        int i = 0;

        if (table.entries[type].count > 0) {
            const short *itemIds = table.entries[type].itemIds;

            do {
                int itemId = *itemIds++;
                unsigned int itemValue = (unsigned short)itemId;

                if (itemId == 0)
                    break;
                *output++ = itemValue + (table.entries[type].highWord << 16);
                i++;
            } while (i < table.entries[type].count);
        }
    }

    *output = 0;
}

int subMenuShopEquipCheck(int item, int category)
{
    int partyIds[16];
    int selectedSlot[8];
    short selectedItem = (short)item;
    int partyCount = 0;
    int matches = 0;
    int party;
    int remaining;
    short *equipment;
    struct MenuShopEquipRecord *unit;

    memset(selectedSlot, 0, sizeof(selectedSlot));
    selectedSlot[1] = 1;
    selectedSlot[4] = 1;
    switch (MenuShopWork->category) {
    case SHOP_CAT_CHAR_WEAPON:
    case SHOP_CAT_CHAR_ACCESSORY:
    case SHOP_CAT_CHAR_AMMO:
        partyCount = PartyAllPartyGet(partyIds, 0);
        break;
    case SHOP_CAT_AGWS_WEAPON:
    case SHOP_CAT_AGWS_ACCESSORY:
    case SHOP_CAT_AGWS_AMMO:
        partyCount = PartyAgwsGet(partyIds);
        break;
    }
    if (partyCount == 0)
        return 0;
    switch (MenuShopWork->category) {
    case SHOP_CAT_CHAR_WEAPON:
        for (party = 0; party < partyCount; party++) {
            unit = func_A191C0(partyIds[party]);
            if (unit == NULL)
                return 0;
            if (unit->itemIds[0][selectedSlot[partyIds[party] - 1]] == selectedItem)
                matches++;
        }
        break;
    case SHOP_CAT_CHAR_AMMO:
        for (party = 0; party < partyCount; party++) {
            unit = func_A191C0(partyIds[party]);
            if (unit == NULL)
                return 0;
            if (unit->itemIds[2][selectedSlot[partyIds[party] - 1]] == selectedItem)
                matches++;
        }
        break;
    case SHOP_CAT_CHAR_ACCESSORY:
        for (party = 0; party < partyCount; party++) {
            unit = func_A191C0(partyIds[party]);
            if (unit == NULL)
                return 0;
            for (equipment = unit->itemIds[1], remaining = 2;
                 remaining >= 0; equipment++, remaining--) {
                if (*equipment == selectedItem)
                    matches++;
            }
        }
        break;
    case SHOP_CAT_AGWS_WEAPON:
        for (party = 0; party < partyCount; party++) {
            unit = func_A191C0(partyIds[party]);
            if (unit == NULL)
                return 0;
            for (equipment = unit->itemIds[0], remaining = 2;
                 remaining >= 0; equipment++, remaining--) {
                if (*equipment == selectedItem)
                    matches++;
            }
        }
        break;
    case SHOP_CAT_AGWS_AMMO:
        for (party = 0; party < partyCount; party++) {
            unit = func_A191C0(partyIds[party]);
            if (unit == NULL)
                return 0;
            for (equipment = unit->itemIds[2], remaining = 2;
                 remaining >= 0; equipment++, remaining--) {
                if (*equipment == selectedItem)
                    matches++;
            }
        }
        break;
    case SHOP_CAT_AGWS_ACCESSORY:
        for (party = 0; party < partyCount; party++) {
            unit = func_A191C0(partyIds[party]);
            if (unit == NULL)
                return 0;
            for (equipment = unit->itemIds[1], remaining = 2;
                 remaining >= 0; equipment++, remaining--) {
                if (*equipment == selectedItem)
                    matches++;
            }
        }
        break;
    }
    return matches;
}

int MenuShopNoSaleCheck(int item)
{
    int itemId;
    int category;
    int flags;

    itemId = item & 0xFFFF;
    category = (unsigned int)item >> 16;
    flags = 0;

    if (itemId == 0)
        return 0;

    switch (category) {
    case ITEM_KIND_ITEM:
        flags = func_A1A4E8(itemId)->flags;
        break;
    case ITEM_KIND_WEAPON:
        flags = dataWpnGet(itemId)->flags;
        break;
    case ITEM_KIND_BULLET:
        flags = dataAttGet(itemId)->flags;
        break;
    case ITEM_KIND_ACCESSORY:
        flags = func_A1A548(itemId)->flags;
        break;
    }

    if ((flags & SHOP_ITEM_FLAG_NO_SALE) != 0)
        return 1;
    return 0;
}

void MenuShopListColorChange(int list, int kind)
{
    int *sorted = MenuSortAddrGet(list);
    int count = MenuSortCheck(list);
    MenuListEntry *entry = MenuListGet(list);
    int i;

    if (MenuShopWork->mode == SHOP_MODE_BUY || kind == 2) {
        for (i = 0; i < count; i++) {
            if (MenuBoxMoneyGet(MenuSortGet(list, i), 0) > dataMoneyBoxChk())
                entry[i].flag = 1;
        }
    } else {
        for (i = 0; i < count; i++) {
            int noSale;

            MenuBoxChk(MenuSortGet(list, i));
            subMenuShopEquipCheck(MenuSortGet(list, i), 0);
            noSale = MenuShopNoSaleCheck(sorted[i]);
            if (noSale == 1)
                entry[i].flag = noSale;
        }
    }
}

int MenuShopListChange00(int list)
{
    struct MenuShopSortOptionTable sortOptions;
    struct MenuShopRowCountTable rowCounts;
    MenuShopWorkData *shop;
    WindowSPCursor *window;
    short width;
    unsigned int columns;
    unsigned int drawFlags;

    if (MenuShopWork->mode == SHOP_MODE_BUY) {
        MenuShopSortSet(0, MenuShopWork->category, 0, 0);
        MenuListMake(0, -10);
        MenuShopListColorChange(0, 0);
    } else {
        sortOptions = menuShopSortOptionTable;
        MenuSortSet(0, sortOptions.entries[MenuShopWork->category].type,
                    sortOptions.entries[MenuShopWork->category].order);
        MenuListMake(0, 0);
        MenuShopListColorChange(0, 0);
    }
    rowCounts = menuShopRowCountTable;
    shop = MenuShopWorkRead();
    columns = 1;
    drawFlags = 7;
    width = SHOP_LIST_WIDTH - shop->mode * 32;
    window = MenuShopWinSPRead();
    window->width = width;
    window->height =
        rowCounts.values[shop->category + shop->mode * SHOP_CAT_COUNT] * SHOP_LIST_ROW_HEIGHT + 6;
    window->rows =
        rowCounts.values[shop->category + shop->mode * SHOP_CAT_COUNT];
    MenuShopWinSP->columns = columns;
    MenuShopWinSP->title = msg00_4_0036DB60[MenuShopWork->mode];
    MenuShopWinSP->draw_flags = drawFlags;
    MenuShopWinSP->items = MenuListGet(0);
    WindowSPItemChange(MenuShopWinSP);
    WindowSPSetSelect(MenuShopWinSP,
        &MenuKeepSelect[(MenuShopWork->category + MenuShopWork->mode * SHOP_CAT_COUNT) * MENU_KEEP_ENTRY_SIZE]);
    if (MenuShopWork->mode == SHOP_MODE_BUY)
        MenuShopWinSP->mode = 6;
    return WindowSPSelect(MenuShopWinSP, 0);
}

int MenuShopListChange01(int list)
{
    int type = SHOP_SORT_TYPE_FRAME;
    WindowSPCursor *window;
    short window_width;

    if (MenuShopWork->sort_mode_selector == 0)
        type = SHOP_SORT_TYPE_ENGINE;

    MenuShopSortSet(0, type, 0, 0);
    MenuListMake(0, -10);
    MenuShopListColorChange(0, 2);

    window = MenuShopWinSP;
    MenuShopWinSP->rows = 9;
    window_width = SHOP_LIST_WIDTH;
    window->height = 222;
    window->width = window_width;
    MenuShopWinSP->columns = 1;
    MenuShopWinSP->title = menuShopPriceListTitleText;
    MenuShopWinSP->draw_flags = 7;
    MenuShopWinSP->items = MenuListGet(0);
    WindowSPItemChange(MenuShopWinSP);
    WindowSPSetSelect(MenuShopWinSP, &MenuKeepSelect[type * MENU_KEEP_ENTRY_SIZE]);

    MenuShopWinSP->mode = 6;
    return WindowSPSelect(MenuShopWinSP, 0);
}

static void MenuShopList(TskObject *task, void *data)
{
    struct MenuShopListDisplay *work = data;
    MenuShopWorkData *selection;
    short closeX;
    short secondTargetX;

    switch (task->state) {
    case 0:
        work->window.x = SHOP_LIST_HIDDEN_X;
        work->window.y = SHOP_LIST_Y;
        work->color = SHOP_DEPTH_LIST;
        work->window.style = SHOP_DEPTH_LIST;
        work->window.draw_flags = 7;
        work->window.title = menuShopBlankText;
        work->window.rows = 0xb;
        work->window.height = 0x10e;
        work->window.state = 0;
        work->window.items = 0;
        WindowSPSet(&work->window);
        work->initialFlags[0] = 0;
        work->initialFlags[1] = 0;
        work->state = SHOP_LIST_CLOSED;
        work->active = 0;
        return;
    case 2:
        switch (work->state) {
        case SHOP_LIST_CLOSED:
            work->active = 0;
            work->state = SHOP_LIST_IDLE;
            /* fall through */
        case SHOP_LIST_IDLE:
            switch (MenuShopWork->state) {
            case SHOP_STATE_BUY_LIST:
            case SHOP_STATE_SELL_LIST:
                work->state = SHOP_LIST_ITEMS_OPEN;
                break;
            case SHOP_STATE_AGWS_BUY_LIST:
                work->state = SHOP_LIST_AGWS_OPEN;
                break;
            }
            break;
        case SHOP_LIST_ITEMS_OPEN:
            work->window.x = SHOP_LIST_ENTER_X;
            work->window.y = SHOP_LIST_Y;
            MenuShopListChange00(0);
            work->window.state = 0x11;
            work->state = SHOP_LIST_ITEMS_SHOWN;
            work->active = 1;
            selection = MenuShopWork;
            work->category = selection->category;
            goto update_open_window;
        case SHOP_LIST_ITEMS_SHOWN:
            selection = MenuShopWork;
        update_open_window:
        {
            short targetX = SHOP_LIST_SHOWN_X;
            if (selection->state == SHOP_STATE_CONFIRM) {
                unsigned char category = selection->category;

                if (selection->mode == SHOP_MODE_BUY) {
                    if (category != SHOP_CAT_ITEM && category != SHOP_CAT_CARD && category != SHOP_CAT_AGWS)
                        targetX = SHOP_LIST_ENTER_X;
                } else if (category != SHOP_CAT_ITEM && category != SHOP_CAT_CARD && category != SHOP_CAT_CHAR_WEAPON && category != SHOP_CAT_AGWS_WEAPON) {
                    targetX = SHOP_LIST_ENTER_X;
                }
            }
            MoveSlide(&work->window.x, &targetX, SHOP_SLIDE_RATE);
            selection = MenuShopWork;
            if (selection->state == SHOP_STATE_MODE_SELECT) {
                work->state = SHOP_LIST_CLOSING;
                selection = MenuShopWork;
            }
            if (work->category != selection->category)
                work->state = SHOP_LIST_ITEMS_OPEN;
            break;
        }
        case SHOP_LIST_CLOSING:
            closeX = -((unsigned short)work->window.width + 0x10);
            MoveSlide(&work->window.x, &closeX, SHOP_SLIDE_RATE);
            if (work->window.x == closeX)
                work->state = SHOP_LIST_CLOSED;
            break;
        case SHOP_LIST_AGWS_OPEN:
            work->window.x = SHOP_LIST_ENTER_X;
            work->window.y = SHOP_LIST_Y;
            MenuShopListChange01(0);
            work->window.state = 0x11;
            work->state = SHOP_LIST_AGWS_SHOWN;
            work->active = 1;
            /* fall through */
        case SHOP_LIST_AGWS_SHOWN:
            secondTargetX = SHOP_LIST_SHOWN_X;
            MoveSlide(&work->window.x, &secondTargetX, SHOP_SLIDE_RATE);
            if (MenuShopWork->state == SHOP_STATE_AGWS_MENU)
                work->state = SHOP_LIST_CLOSING;
            break;
        }
        if (work->active != 0)
            WindowSPMain(&work->window);
        break;
    }
}

static void MenuShopEx(TskObject *task, void *data)
{
    struct MenuShopExDisplay *work = data;
    MenuShopWorkData **shop = &MenuShopWork;
    short ribbonX[2];
    short rosterX[2];
    short ribbonY[2];
    short rosterY[2];
    short totalX[1];
    short totalY[1];
    int i;
    int j;
    extern unsigned short *func_A1A428(int item);
    extern int MenuWeaponEquipCheck(int unit, int item, int mode);
    extern int MenuAccessoryEquipCheck(int unit, int item, int mode);

    switch (task->state) {
    case 0:
        work->depth = SHOP_DEPTH_TOP;
        for (i = 0; i < 2; i++) {
            eRibbonSet(&work->headers[i].ribbon, 3);
            work->headers[i].ribbon.x = SHOP_PANEL_HIDDEN_X;
            work->headers[i].ribbon.y = 0;
            work->headers[i].ribbon.width = SHOP_PANEL_WIDTH;
            work->headers[i].ribbon.height = 0x18;
            work->headers[i].ribbon.x = 0x220;
            work->headers[i].ribbon.y = 0x53 + 0x20 * i;
            work->headers[i].ribbon.work = work->depth;
            eTagFontSet(&work->headers[i].label, msg00_5_0036DB68[i]);
            work->headers[i].label.work = work->depth + 2;
            eNumberSet(&work->headers[i].number, 0);
            work->headers[i].number.work = work->depth + 2;
        }
        for (i = 0; i < 2; i++) {
            eRibbonSet(&work->parties[i].ribbon, 3);
            work->parties[i].ribbon.width = SHOP_PANEL_WIDTH;
            work->parties[i].ribbon.work = work->depth;
            work->parties[i].ribbon.x = 0x220;
            work->parties[i].ribbon.y = 0x93;
            eTagFontSet(&work->parties[i].label, msg00_5_0036DB68[2]);
            work->parties[i].label.work = work->depth + 2;
        }
        work->parties[0].ribbon.height = 0x30;
        for (j = 0; j < (*shop)->partyCount; j++) {
            eSpriteSet(&work->parties[0].sprites[j], MenuFaceEpidGet(*(short *)((char *)(*shop)->partyIds + (j << 1)), 1));
            work->parties[0].sprites[j].depth = work->depth + 2;
        }
        work->parties[1].ribbon.height = 0x60;
        for (j = 0; j < (*shop)->unitCount; j++) {
            eSpriteSet(&work->parties[1].sprites[j], MenuFaceEpidGet(*(short *)((char *)(*shop)->unitIds + (j << 1)), 1));
            work->parties[1].sprites[j].depth = work->depth + 2;
        }
        eRibbonSet(&work->equipment.ribbon, 3);
        {
            const char **labels;
            short width, height, left, top;
            int style;

            left = SHOP_PANEL_HIDDEN_X;
            {
                const char *head = *(const char **)(void *)&work->equipment.ribbon;

                *(const char **)(void *)&work->equipment.ribbon = head;
            }
            style = ((union { int value; short half[2]; } *)(void *)&work->depth)->value;
            labels = msg00_5_0036DB68;
            top = 0x93;
            height = 0x18;
            width = SHOP_PANEL_WIDTH;
            work->equipment.ribbon.x = left;
            work->equipment.ribbon.width = width;
            work->equipment.ribbon.work = style;
            work->equipment.ribbon.height = height;
            work->equipment.ribbon.y = top;
            eTagFontSet(&work->equipment.label, labels[3]);
        }
        work->equipment.label.work = work->depth + 2;
        eNumberSet(&work->equipment.number, 0);
        work->equipment.number.work = work->depth + 2;
        work->slideFlags = 0;
        work->equipment.number.format = 0;
        work->flags = 0;
        work->equipment.number.digits = 1;
        break;
    case 2:

        for (i = 0; i < 2; i++) {
            ribbonX[i] = SHOP_PANEL_HIDDEN_X;
            ribbonY[i] = 0x53 + 0x20 * i;
        }
        rosterY[0] = 0x93;
        rosterY[1] = 0x43;
        rosterX[0] = SHOP_PANEL_HIDDEN_X;
        rosterX[1] = SHOP_PANEL_HIDDEN_X;
        totalX[0] = SHOP_PANEL_HIDDEN_X;
        totalY[0] = 0xcb;

        switch ((*shop)->state) {
        case SHOP_STATE_BUY_LIST:
        case SHOP_STATE_SELL_LIST:
        case SHOP_STATE_RESULT:
            if ((unsigned char)((*shop)->stateFlags & SHOP_FLAG_STATE_ENTERED)) {
                if ((*shop)->mode == SHOP_MODE_BUY)
                    eTagFontSet(&work->headers[0].label, msg00_5_0036DB68[0]);
                else
                    eTagFontSet(&work->headers[0].label, msg00_5_0036DB68[4]);
                work->slideFlags |= 2;
            }
            for (i = 0; i < 2; i++) {
                ribbonX[i] = SHOP_PANEL_SHOWN_X;
                ribbonY[i] = 0x53 + 0x20 * i;
                if (work->slideFlags & 2) {
                    work->headers[i].ribbon.y = 0x53 + 0x20 * i;
                    work->slideFlags &= ~2;
                }
            }
            /* category - 1: 0..2 character gear, 3..5 A.G.W.S. gear, 6 A.G.W.S. */
            switch ((*shop)->category - 1) {
            case 0:
            case 1:
            case 2:
                work->equipment.ribbon.y = totalY[0] = 0xcb;
                totalX[0] = SHOP_PANEL_SHOWN_X;
                eTagFontSet(&work->parties[0].label, msg00_5_0036DB68[2]);
                rosterX[0] = SHOP_PANEL_SHOWN_X;
                rosterY[0] = 0x93;
                work->parties[0].ribbon.y = 0x93;
                work->slideFlags |= 2;
                break;
            case 3:
            case 4:
            case 5:
                work->equipment.ribbon.y = totalY[0] = 0xfb;
                totalX[0] = SHOP_PANEL_SHOWN_X;
                eTagFontSet(&work->parties[1].label, msg00_5_0036DB68[2]);
                rosterX[1] = SHOP_PANEL_SHOWN_X;
                rosterY[1] = 0x93;
                work->parties[1].ribbon.y = 0x93;
                work->slideFlags |= 2;
                break;
            case 6:
                ribbonX[0] = SHOP_PANEL_HIDDEN_X;
                ribbonX[1] = SHOP_PANEL_SHOWN_X;
                ribbonY[1] = 0x118;
                break;
            }
            break;
        case SHOP_STATE_QUANTITY:
        case SHOP_STATE_CONFIRM:
            switch ((*shop)->category - 1) {
            case 0:
            case 1:
            case 2:
                rosterX[0] = SHOP_PANEL_SHOWN_X;
                rosterY[0] = 0xf0;
                break;
            case 3:
            case 4:
            case 5:
                rosterX[1] = SHOP_PANEL_SHOWN_X;
                rosterY[1] = 0xe8;
                break;
            case 6:
                ribbonX[1] = SHOP_PANEL_SHOWN_X;
                ribbonY[1] = 0x118;
                break;
            }
            break;
        }

        for (i = 0; i < 2; i++) {
            MoveSlide(&work->headers[i].ribbon.x, &ribbonX[i], SHOP_SLIDE_RATE);
            if (work->headers[i].ribbon.x == ribbonX[i])
                MoveSlide(&work->headers[i].ribbon.y, &ribbonY[i], SHOP_SLIDE_RATE);
            eRibbonMain(&work->headers[i].ribbon);
            work->headers[i].number.x = work->headers[i].ribbon.x + 0x40;
            work->headers[i].number.y = work->headers[i].ribbon.y + 4;
            work->headers[i].number.format = 0;
            if (i == 0) {
                if ((*shop)->mode == SHOP_MODE_BUY) {
                    work->headers[i].number.digits = 2;
                    if ((*shop)->listIndex >= 0)
                        work->headers[0].number.value = MenuBoxChk(MenuSortGet(0, (*shop)->listIndex));
                    else
                        work->headers[0].number.value = 0;
                } else {
                    work->headers[i].number.digits = 6;
                    if ((*shop)->listIndex >= 0)
                        work->headers[0].number.value = MenuBoxMoneyGet(MenuSortGet(0, (*shop)->listIndex), 1);
                    else
                        work->headers[0].number.value = 0;
                }
            } else {
                work->headers[i].number.digits = 9;
                if ((*shop)->listIndex >= 0)
                    work->headers[i].number.value = dataMoneyBoxChk();
                else
                    work->headers[i].number.value = 0;
            }
            eNumberMain(&work->headers[i].number);
            work->headers[i].label.x = work->headers[i].number.x - 8;
            work->headers[i].label.y = work->headers[i].ribbon.y + 4;
            eTagFontMain(&work->headers[i].label);
        }

        for (i = 0; i < 2; i++) {
            MoveSlide(&work->parties[i].ribbon.x, &rosterX[i], SHOP_SLIDE_RATE);
            if (work->parties[i].ribbon.x == rosterX[i])
                MoveSlide(&work->parties[i].ribbon.y, &rosterY[i], SHOP_SLIDE_RATE);
            eRibbonMain(&work->parties[i].ribbon);
            if (i == 0) {
                for (j = 0; j < (*shop)->partyCount; j++) {
                    int usable;

                    usable = 0;
                    if ((*shop)->modelId != 0) {
                        switch (((short *)&(*shop)->itemId)[1]) {
                        case ITEM_KIND_WEAPON:
                            if (MenuWeaponEquipCheck(*(short *)((char *)(*shop)->partyIds + (j << 1)), (*shop)->modelId, 0))
                                usable = 1;
                            break;
                        case ITEM_KIND_ACCESSORY:
                            if (MenuAccessoryEquipCheck(*(short *)((char *)(*shop)->partyIds + (j << 1)), (*shop)->modelId, -1))
                                usable = 1;
                            break;
                        case ITEM_KIND_BULLET:
                            usable = MenuWeaponEquipCheck(*(short *)((char *)(*shop)->partyIds + (j + j)), ((short *)func_A1A428((*shop)->modelId))[6], 0) != 0;
                            break;
                        }
                    }
                    if (usable == 1) {
                        work->parties[i].sprites[j].blue = SHOP_SPRITE_LIT;
                        work->parties[i].sprites[j].green = SHOP_SPRITE_LIT;
                        work->parties[i].sprites[j].red = SHOP_SPRITE_LIT;
                    } else {
                        work->parties[i].sprites[j].blue = SHOP_SPRITE_DIM;
                        work->parties[i].sprites[j].green = SHOP_SPRITE_DIM;
                        work->parties[i].sprites[j].red = SHOP_SPRITE_DIM;
                    }
                    work->parties[i].sprites[j].x = work->parties[i].ribbon.x + (j % 4) * 20 + 0x40;
                    work->parties[i].sprites[j].y = work->parties[i].ribbon.y + (j / 4) * 24;
                    eSpriteMain(&work->parties[i].sprites[j]);
                }
            } else {
                for (j = 0; j < (*shop)->unitCount; j++) {
                    int usable;

                    usable = 0;
                    eSpriteSet(&work->parties[i].sprites[j], MenuFaceEpidGet(*(short *)((char *)(*shop)->unitIds + (j << 1)), 1));
                    if ((*shop)->modelId != 0) {
                        switch (((short *)&(*shop)->itemId)[1]) {
                        case ITEM_KIND_WEAPON:
                            if (MenuWeaponEquipCheck(*(short *)((char *)(*shop)->unitIds + (j << 1)), (*shop)->modelId, 0))
                                usable = 1;
                            break;
                        case ITEM_KIND_ACCESSORY:
                            if (MenuAccessoryEquipCheck(*(short *)((char *)(*shop)->unitIds + (j << 1)), (*shop)->modelId, -1))
                                usable = 1;
                            break;
                        case ITEM_KIND_BULLET:
                            usable = MenuWeaponEquipCheck(*(short *)((char *)(*shop)->unitIds + (j << 1)), ((short *)func_A1A428((*shop)->modelId))[6], 0) != 0;
                            break;
                        }
                    }
                    if (usable == 1) {
                        work->parties[i].sprites[j].blue = SHOP_SPRITE_LIT;
                        work->parties[i].sprites[j].green = SHOP_SPRITE_LIT;
                        work->parties[i].sprites[j].red = SHOP_SPRITE_LIT;
                    } else {
                        work->parties[i].sprites[j].blue = SHOP_SPRITE_DIM;
                        work->parties[i].sprites[j].green = SHOP_SPRITE_DIM;
                        work->parties[i].sprites[j].red = SHOP_SPRITE_DIM;
                    }
                    work->parties[i].sprites[j].x = work->parties[i].ribbon.x + (j & 1) * 64 + 0x40;
                    work->parties[i].sprites[j].depth = work->parties[i].ribbon.work + 2;
                    work->parties[i].sprites[j].y = work->parties[i].ribbon.y + (j / 2) * 32;
                    eSpriteMain(&work->parties[i].sprites[j]);
                }
            }
            work->parties[i].label.x = work->parties[i].sprites[0].x - 8;
            work->parties[i].label.y = work->parties[i].ribbon.y + i * 28 + 16;
            eTagFontMain(&work->parties[i].label);
        }
        MoveSlide(&work->equipment.ribbon.x, &totalX[0], SHOP_SLIDE_RATE);
        if ((&work->equipment)[i].ribbon.x == totalX[i])
            MoveSlide(&work->equipment.ribbon.y, &totalY[0], SHOP_SLIDE_RATE);
        eRibbonMain(&work->equipment.ribbon);
        work->equipment.number.x = work->equipment.ribbon.x + 0x40;
        work->equipment.number.y = work->equipment.ribbon.y + 4;
        work->equipment.number.value = (*shop)->canEquip;
        eNumberMain(&work->equipment.number);
        work->equipment.label.x = work->equipment.number.x - 8;
        work->equipment.label.y = work->equipment.ribbon.y + 4;
        eTagFontMain(&work->equipment.label);
        break;
    }
}

static void MenuShopSeisan(TskObject *task, void *data)
{
    struct MenuShopSalePanel *work = data;
    short targetX[1];
    int i;

    switch (task->state) {
    case 0:
        work->depth = SHOP_DEPTH_TOP;
        WindowDXSet(&work->window);
        work->window.x = SHOP_PANEL_HIDDEN_X;
        work->window.style = work->depth;
        work->window.y = 0x50;
        work->window.width = 0xc0;
        work->window.height = 0x98;
        work->window.tag_id = msg00_6[0];
        work->window.state = WINDOW_STATE_OPEN_REQUEST;
        WindowDXMain(&work->window);
        work->window.state = WINDOW_STATE_OPEN;
        for (i = 0; i < 4; i++) {
            eTagFontSet(&work->labels[i], msg01_7[i]);
            work->labels[i].work = work->depth + 2;
        }
        for (i = 0; i < 6; i++) {
            eNumberSet(&work->numbers[i], 0);
            work->numbers[i].work = work->depth + 2;
        }
        for (i = 0; i < 2; i++) {
            eSpriteSet(&work->sprites[i], i + 0x705);
            work->sprites[i].style = work->depth + 2;
        }
        for (i = 0; i < 2; i++) {
            work->quantityDivider[i].red =
                work->quantityDivider[i].green =
                work->quantityDivider[i].blue = -0x60;
            work->quantityDivider[i].alpha = -0x80;
            work->quantityDivider[i].depth = work->depth + 2;
        }
        work->quantityDividerState = 0;
        for (i = 0; i < 2; i++) {
            work->moneyDivider[i].red =
                work->moneyDivider[i].green =
                work->moneyDivider[i].blue = -0x60;
            work->moneyDivider[i].alpha = -0x80;
            work->moneyDivider[i].depth = work->depth + 2;
        }
        work->moneyDividerState = 0;
        eRibbonSet(&work->ribbon, 0);
        {
        int cellIndex;
        for (cellIndex = 0; cellIndex < 6; cellIndex++) {
            if (cellIndex < 4) {
                work->ribbon.points[cellIndex].red = 0x20;
                work->ribbon.points[cellIndex].green = 0x34;
                work->ribbon.points[cellIndex].blue = 0x34;
            } else {
                work->ribbon.points[cellIndex].blue = 0;
                work->ribbon.points[cellIndex].green = 0;
                work->ribbon.points[cellIndex].red = 0;
            }
            if ((unsigned int)(cellIndex - 2) < 2)
                work->ribbon.points[cellIndex].alpha = 0x20;
            else
                work->ribbon.points[cellIndex].alpha = 0;
        }
        }
        work->flags = 0;
        work->selectionFlags = 0;
        break;
    case 2:
        targetX[0] = SHOP_PANEL_HIDDEN_X;
        if (MenuShopWork->state == SHOP_STATE_QUANTITY || MenuShopWork->state == SHOP_STATE_CONFIRM)
            targetX[0] = SHOP_PANEL_SHOWN_X;
        MoveSlide(&work->window.x, targetX, SHOP_SLIDE_RATE);
        work->window.title_font.render.text = msg00_6[MenuShopWork->mode & 1];
        WindowDXMain(&work->window);
        for (i = 0; i < 4; i++) {
            work->labels[i].x = (unsigned short)work->window.x + 0x43;
            work->labels[i].y = (unsigned short)work->window.y +
                ((i % 2) * 0x30 + (i / 2) * 0x4c) + 4;
            eTagFontMain(&work->labels[i]);
        }
        for (i = 0; i < 2; i++) {
            if (MenuShopWork->mode == SHOP_MODE_BUY)
                work->sprites[i].spriteId = i + 0x705;
            else
                work->sprites[i].spriteId = 0x706 - i;
            work->sprites[i].x = (unsigned short)work->window.x + 0x39;
            work->sprites[i].y = (unsigned short)work->window.y + i * 0x4c + 0x1c;
            eSpriteMain(&work->sprites[i]);
        }
        work->ribbon.x = (unsigned short)work->window.x + 0x37;
        work->ribbon.y = (unsigned short)work->window.y + 0x1c;
        work->ribbon.width = 0x2c;
        work->ribbon.height = 0x10;
        work->ribbon.work = work->depth + 1;
        for (i = 0; i < 6; i++)
            eRibbonMain(&work->ribbon);
        {
        MenuShopWorkData *selection = MenuShopWork;
        if (selection->listIndex >= 0) {
            unsigned char quantityLimit;
            signed char quantity;
            int unitPrice;
            work->numbers[0].digits = work->numbers[1].digits = work->numbers[2].digits = 2;
            work->numbers[3].digits = work->numbers[4].digits = work->numbers[5].digits = 9;
            quantityLimit = MenuShopWork->quantityLimit;
            work->numbers[0].value = quantityLimit;
            quantity = MenuShopWork->quantity;
            work->numbers[1].value = quantity;
            if (MenuShopWork->mode == SHOP_MODE_BUY) {
                work->numbers[2].value = quantityLimit + quantity;
                work->numbers[3].value = MenuShopWork->availableMoney;
                unitPrice = MenuBoxMoneyGet(MenuSortGet(0, MenuShopWork->listIndex), 0);
                selection = MenuShopWork;
                work->numbers[4].value = unitPrice * selection->quantity;
                work->numbers[5].value = (int)selection->availableMoney - work->numbers[4].value;
            } else {
                work->numbers[2].value = quantityLimit - quantity;
                work->numbers[3].value = MenuShopWork->availableMoney;
                unitPrice = MenuBoxMoneyGet(MenuSortGet(0, MenuShopWork->listIndex), 1);
                selection = MenuShopWork;
                work->numbers[4].value = unitPrice * selection->quantity;
                work->numbers[5].value = (int)selection->availableMoney + work->numbers[4].value;
            }
        }
        i = 0;
        if (*(MenuShopListIndex *)&selection->listIndex >= 0) {
            do {
                work->numbers[i].x = (unsigned short)work->window.x + 0x47;
                work->numbers[i].y = (unsigned short)work->window.y + ((i % 3) * 0x18 + (i / 3) * 0x4c) + 4;
                work->numbers[i].format = 0;
                eNumberMain(&work->numbers[i]);
                if (++i >= 6)
                    break;
            } while (MenuShopWork->listIndex >= 0);
        }
        }
        work->quantityDivider[0].x = (unsigned short)work->window.x + 0x1a;
        work->quantityDivider[1].x = (unsigned short)work->window.x + 0x5d;
        work->quantityDivider[1].y = (unsigned short)work->window.y + 0x30;
        work->quantityDivider[0].y = (unsigned short)work->window.y + 0x30;
        work->moneyDivider[0].x = (unsigned short)work->window.x + 0x14;
        work->moneyDivider[1].x = (unsigned short)work->window.x + 0xaa;
        work->moneyDivider[1].y = (unsigned short)work->window.y + 0x7c;
        work->moneyDivider[0].y = (unsigned short)work->window.y + 0x7c;
        endPrintExtFunc(work->depth, END_PRINT_CMD_LINE, work->quantityDivider);
        endPrintExtFunc(work->depth, END_PRINT_CMD_LINE, work->moneyDivider);
        break;
    }
}

static void MenuShopSeisan02(TskObject *task, void *data)
{
    MenuShopSeisan02Work *work = data;
    int i;
    unsigned char *spriteBase;
    short slideTarget48;
    short slideTargetBack;
    short slideTarget288;
    short slideTarget528;
    const char **tagSlot;
    int *spriteWork;

    switch (task->state) {
    case 0:
        work->ticker = SHOP_DEPTH_TOP;
        WindowDXSet(&work->window);
        work->window.x = SHOP_PANEL_HIDDEN_X;
        work->window.y = 80;
        work->window.style = work->ticker;
        work->window.width = 208;
        work->window.height = 80;
        work->window.tag_id = menuShopPayText;
        tagSlot = &work->window.tag_id;
        work->window.state = WINDOW_STATE_OPEN_REQUEST;
        WindowDXMain(&work->window);
        work->window.state = WINDOW_STATE_OPEN;
        for (i = 0; i < 2; i++) {
            eTagFontSet(&work->tags[i], msg01_8_0036DBA0[i]);
            work->tags[i].work = work->ticker + 2;
        }
        for (i = 0; i < 3; i++) {
            eNumberSet(&work->numbers[i], 0);
            work->numbers[i].work = work->ticker + 2;
        }
        spriteWork = (int *)((unsigned char *)tagSlot + 896);
        eSpriteSet(&work->sprite, 1798);
        *spriteWork = work->ticker + 2;
        for (i = 0; i < 2; i++) {
            int off = 928 + 12 * i;
            unsigned char *ent = (unsigned char *)work + off;
            signed char *stateBase;

            ent[30] = -96;
            ent[29] = -96;
            ent[28] = -96;
            ent[31] = -128;
            stateBase = &work->window.state;
            *(int *)(stateBase + off) = work->ticker + 2;
        }
        work->unmodeled_976 = 0;
        work->unmodeled_03 = 0;
        work->state = 0;
        break;
    case 2:
        switch (work->state) {
        case SHOP_PAY_CLOSED:
            work->flag = 0;
            work->state = SHOP_PAY_IDLE;
            /* fall through */
        case SHOP_PAY_IDLE:
            switch (MenuShopWork->state) {
            case SHOP_STATE_RESTORE_UNIT:
                work->state = SHOP_PAY_RESTORE_OPEN;
                break;
            case SHOP_STATE_AGWS_BUY_CONFIRM:
                work->state = SHOP_PAY_BUY_OPEN;
                break;
            }
            break;
        case SHOP_PAY_RESTORE_OPEN:
            work->window.x = -224;
            work->window.y = 80;
            work->state = SHOP_PAY_RESTORE_SHOWN;
            work->kind = SHOP_PAY_KIND_RESTORE;
            work->flag = 1;
            /* fall through */
        case SHOP_PAY_RESTORE_SHOWN:
            slideTarget48 = 48;
            MoveSlide(&work->window.x, &slideTarget48, SHOP_SLIDE_RATE);
            if (MenuShopWork->state == SHOP_STATE_AGWS_MENU)
                work->state = SHOP_PAY_RESTORE_CLOSING;
            break;
        case SHOP_PAY_RESTORE_CLOSING:
            slideTargetBack = -224;
            MoveSlide(&work->window.x, &slideTargetBack, SHOP_SLIDE_RATE);
            if (work->window.x == slideTargetBack)
                work->state = SHOP_PAY_CLOSED;
            break;
        case SHOP_PAY_BUY_OPEN:
            work->window.x = SHOP_PANEL_HIDDEN_X;
            work->window.y = 64;
            work->kind = SHOP_PAY_KIND_BUY;
            work->state = SHOP_PAY_BUY_SHOWN;
            work->flag = 1;
            /* fall through */
        case SHOP_PAY_BUY_SHOWN:
            slideTarget288 = 288;
            MoveSlide(&work->window.x, &slideTarget288, SHOP_SLIDE_RATE);
            switch (MenuShopWork->state) {
            case SHOP_STATE_AGWS_MENU:
            case SHOP_STATE_AGWS_BUY_LIST:
            case SHOP_STATE_AGWS_BUY_UNIT:
                work->state = SHOP_PAY_BUY_CLOSING;
                break;
            }
            break;
        case SHOP_PAY_BUY_CLOSING:
            slideTarget528 = SHOP_PANEL_HIDDEN_X;
            MoveSlide(&work->window.x, &slideTarget528, SHOP_SLIDE_RATE);
            if (work->window.x == slideTarget528)
                work->state = SHOP_PAY_CLOSED;
            break;
        }

        if (!work->flag)
            return;
        WindowDXMain(&work->window);
        for (i = 0; i < 2; i++) {
            work->tags[i].x = work->window.x + 75;
            work->tags[i].y = work->window.y + (48 * (i % 2) + 76 * (i / 2)) + 7;
            eTagFontMain(&work->tags[i]);
        }
        i = 1;
        spriteBase = (unsigned char *)work + 896;
        *(short *)(spriteBase + 16) = work->window.x + 65;
        *(short *)(spriteBase + 18) = work->window.y + 31;
        eSpriteMain(&work->sprite);
        work->numbers[0].value = dataMoneyBoxChk();
        if (work->kind == i) {
            work->numbers[1].value = SHOP_RESTORE_COST;
        } else {
            int item;

            if (MenuShopWork->sort_mode_selector != 0)
                item = ITEM_KEY_FRAME + (unsigned short)MenuShopWork->equipmentId;
            else
                item = ITEM_KEY_ENGINE + (unsigned short)MenuShopWork->equipmentId;
            work->numbers[1].value = MenuBoxMoneyGet(item, 0);
        }
        work->numbers[2].value = work->numbers[0].value - work->numbers[1].value;
        for (i = 0; i < 3; i++) {
            work->numbers[i].digits = 9;
            work->numbers[i].x = work->window.x + 83;
            work->numbers[i].y = work->window.y + 24 * i + 7;
            work->numbers[i].format = 0;
            eNumberMain(&work->numbers[i]);
        }
        work->points[0].x = work->window.x + 28;
        work->points[1].x = work->window.x + 181;
        work->points[1].y = work->window.y + 51;
        work->points[0].y = work->window.y + 51;
        endPrintExtFunc(work->ticker, END_PRINT_CMD_LINE,&work->points[0]);
        break;
    }
}

static void MenuShopEx02(TskObject *task, void *data)
{
    struct MenuShopUnitPanel *work = data;
    short openX;
    short closeX;
    int i;

    switch (task->state) {
    case 0:
    {
        short *unitIds;
        work->depth = SHOP_DEPTH_LIST;
        eRibbonSet(&work->ribbon, 3);
        work->ribbon.x = SHOP_PANEL_HIDDEN_X;
        work->ribbon.y = 0x40;
        work->ribbon.work = work->depth;
        work->ribbon.width = SHOP_PANEL_WIDTH;
        work->ribbon.height = 0x90;
        eTagFontSet(&work->label, menuShopTargetTagText);
        work->label.work = work->depth + 2;
        for (i = 0; i < MenuShopWork->unitCount; i++) {
            unitIds = MenuShopWork->unitIds;
            eSpriteSet(&work->portraits[i], MenuFaceEpidGet(unitIds[i], 0));
            work->portraits[i].style = work->depth + 2;
        }
        eCursolSet(&work->cursor, 0);
        work->cursor.work = work->depth + 4;
        work->cursor.state = 0;
        work->active = work->flags = work->selectionFlags = 0;
        work->state = 0;
        return;
    }
    case 2:
        switch (work->state) {
        case SHOP_UNITS_CLOSED:
            work->active = 0;
            work->state = SHOP_UNITS_IDLE;
            /* fall through */
        case SHOP_UNITS_IDLE:
            switch (MenuShopWork->state) {
            case SHOP_STATE_AGWS_BUY_LIST:
            case SHOP_STATE_AGWS_BUY_UNIT:
                work->state = SHOP_UNITS_BUY_OPEN;
                break;
            case SHOP_STATE_RESTORE_UNIT:
                work->state = SHOP_UNITS_RESTORE_OPEN;
                break;
            }
            break;
        case SHOP_UNITS_RESTORE_OPEN:
        case SHOP_UNITS_BUY_OPEN:
            work->ribbon.x = SHOP_PANEL_HIDDEN_X;
            work->active = 1;
            if (work->state == SHOP_UNITS_RESTORE_OPEN) {
                work->state = SHOP_UNITS_RESTORE_SHOWN;
                work->cursor.state = 0x20;
            } else {
                work->state = SHOP_UNITS_BUY_SHOWN;
            }
            /* fall through */
        case SHOP_UNITS_RESTORE_SHOWN:
        case SHOP_UNITS_BUY_SHOWN:
        {
            unsigned char shopState;
            openX = SHOP_PANEL_SHOWN_X;
            MoveSlide(&work->ribbon.x, &openX, SHOP_SLIDE_RATE);
            shopState = MenuShopWork->state;
            if (shopState == SHOP_STATE_AGWS_MENU) {
                work->state = SHOP_UNITS_CLOSING;
            }
            if (MenuShopWork->state == SHOP_STATE_AGWS_BUY_CONFIRM) {
                work->state = SHOP_UNITS_CLOSING;
            }
            if (work->state == SHOP_UNITS_BUY_SHOWN) {
                for (i = 0; i < MenuShopWork->unitCount; i++) {
                    int canEquip = 0;
                    short *unitIds = MenuShopWork->unitIds;
                    eSpriteSet(&work->portraits[i], MenuFaceEpidGet(unitIds[i], 0));
                    if (MenuShopWork->sort_mode_selector != 0) {
                        short *currentIds = MenuShopWork->unitIds;
                        if (MenuFrameEquipCheck(currentIds[i], MenuShopWork->equipmentId) != 0)
                            canEquip = 1;
                    } else {
                        short *currentIds = MenuShopWork->unitIds;
                        canEquip = MenuEngineEquipCheck(currentIds[i], MenuShopWork->equipmentId) != 0;
                    }
                    if (canEquip == 0) {
                        work->portraits[i].blue = SHOP_SPRITE_DIM;
                        work->portraits[i].green = SHOP_SPRITE_DIM;
                        work->portraits[i].red = SHOP_SPRITE_DIM;
                    } else {
                        work->portraits[i].blue = -0x80;
                        work->portraits[i].green = -0x80;
                        work->portraits[i].red = -0x80;
                    }
                }
                if (MenuShopWork->state == SHOP_STATE_AGWS_BUY_UNIT && work->cursor.state == 0)
                    work->cursor.state = 0x20;
                if (MenuShopWork->state == SHOP_STATE_AGWS_BUY_LIST)
                    work->cursor.state = 0;
            } else {
                for (i = 0; i < MenuShopWork->unitCount; i++) {
                    short *unitIds = MenuShopWork->unitIds;
                    eSpriteSet(&work->portraits[i], MenuFaceEpidGet(unitIds[i], 0));
                    unitIds = MenuShopWork->unitIds;
                    if (MenuCharHpCheck(unitIds[i]) != 0) {
                        work->portraits[i].blue = SHOP_SPRITE_DIM;
                        work->portraits[i].green = SHOP_SPRITE_DIM;
                        work->portraits[i].red = SHOP_SPRITE_DIM;
                    } else {
                        work->portraits[i].blue = -0x80;
                        work->portraits[i].green = -0x80;
                        work->portraits[i].red = -0x80;
                    }
                }
            }
            break;
        }
        case SHOP_UNITS_CLOSING:
            closeX = SHOP_PANEL_HIDDEN_X;
            MoveSlide(&work->ribbon.x, &closeX, SHOP_SLIDE_RATE);
            if (work->ribbon.x == closeX)
                work->state = SHOP_UNITS_CLOSED;
            work->cursor.state = 0;
            break;
        }
        if (work->active != 0) {
            eRibbonMain(&work->ribbon);
            work->label.x = work->ribbon.x + 8;
            work->label.y = work->ribbon.y + 0x44;
            eTagFontMain(&work->label);
            for (i = 0; i < MenuShopWork->unitCount; i++) {
                work->portraits[i].x = work->ribbon.x + (i & 1) * 0x40 + 0x40;
                work->portraits[i].y = work->ribbon.y + (i / 2) * 0x30;
                work->portraits[i].style = work->depth + 2;
                eSpriteMain(&work->portraits[i]);
            }
            work->cursor.x = work->ribbon.x + ((unsigned char)MenuShopWork->unitSelection & 1) * 0x40 + 0x40;
            work->cursor.y = work->ribbon.y + (MenuShopWork->unitSelection / 2) * 0x30 + 0x10;
            work->cursor.work = work->depth + 4;
            eCursolMain(&work->cursor);
        }
        break;
    }
}

static void MenuShopIcon(TskObject *task, void *data)
{
    struct MenuShopIconDisplay *work = data;
    int i;

    switch (task->state) {
    case 0:
        {
            struct MenuShopIconIds spriteIds = menuShopIconSpriteTable;
            work->depth = SHOP_DEPTH_TOP;
            for (i = 0; i < SHOP_ICON_COUNT; i++) {
                eSpriteSet(&work->sprites[i], spriteIds.ids[i]);
                work->sprites[i].depth = work->depth;
            }
        }
        {
            short topY = 12;
            for (i = 1; i >= 0; i--)
                work->sprites[i].y = topY;
        }
        for (i = SHOP_ICON_CATEGORY_FIRST; i < SHOP_ICON_COUNT; i++) {
            work->sprites[i].y = 12;
            if (((MenuShopWork->categoryMask >> (i - SHOP_ICON_CATEGORY_FIRST)) & 1) == 0) {
                work->sprites[i].blue = SHOP_SPRITE_DIM;
                work->sprites[i].green = SHOP_SPRITE_DIM;
                work->sprites[i].red = SHOP_SPRITE_DIM;
            }
        }
        work->active = 1;
        work->slide = SHOP_ICON_SLIDE_MAX;
        work->scale = 1.0f;
        work->animation = 0;
        work->phase = 0;
        for (i = 0; i < SHOP_CAT_COUNT; i++)
            work->categoryLift[i] = 0;
        work->state = 2;
        return;
    case 2:
        switch (MenuShopWork->state) {
        case SHOP_STATE_MODE_SELECT:
            if (MenuShopWork->stateFlags & SHOP_FLAG_STATE_ENTERED) {
                for (i = SHOP_ICON_CATEGORY_FIRST; i < SHOP_ICON_COUNT; i++) {
                    work->sprites[i].blue = -0x80;
                    work->sprites[i].green = -0x80;
                    work->sprites[i].red = -0x80;
                }
            }
            if (work->slide != 0)
                work->slide--;
            if (work->scale != 1.0f)
                work->scale += 0.2;
            break;
        case SHOP_STATE_QUANTITY:
        case SHOP_STATE_RESULT:
        case SHOP_STATE_CONFIRM:
        case SHOP_STATE_BUY_LIST:
        case SHOP_STATE_SELL_LIST:
            if (MenuShopWork->stateFlags & SHOP_FLAG_STATE_ENTERED) {
                for (i = SHOP_ICON_CATEGORY_FIRST; i < SHOP_ICON_COUNT; i++) {
                    if (((MenuShopWork->categoryMask >> (i - SHOP_ICON_CATEGORY_FIRST)) & 1) == 0) {
                        work->sprites[i].blue = SHOP_SPRITE_DIM;
                        work->sprites[i].green = SHOP_SPRITE_DIM;
                        work->sprites[i].red = SHOP_SPRITE_DIM;
                    } else {
                        work->sprites[i].blue = -0x80;
                        work->sprites[i].green = -0x80;
                        work->sprites[i].red = -0x80;
                    }
                }
            }
            if (work->slide != 0)
                work->slide--;
            work->scale += menuShopIconScaleGrowStep;
            if (work->scale > 1.0f)
                work->scale = 1.0f;
            break;
        case SHOP_STATE_EXIT:
            work->active = 0;
            /* fall through */
        case SHOP_STATE_END:
            if (work->slide != SHOP_ICON_SLIDE_MAX)
                work->slide++;
            break;
        default:
            if (work->slide != 0)
                work->slide--;
            work->scale -= menuShopIconScaleShrinkStep;
            if (work->scale < 0.0f)
                work->scale = 0.0f;
            break;
        }
        for (i = 0; i < SHOP_CAT_COUNT; i++) {
            if (i == MenuShopWork->category) {
                unsigned char state = MenuShopWork->state;
                if (state != SHOP_STATE_MODE_SELECT && state != SHOP_STATE_EXIT &&
                    (state >> 4) != SHOP_STATE_GROUP_AGWS_MENU && (state >> 4) != SHOP_STATE_GROUP_AGWS_BUY) {
                    if (work->categoryLift[i] != SHOP_ICON_LIFT_MAX)
                        work->categoryLift[i]++;
                    continue;
                }
            }
            if (work->categoryLift[i] != 0)
                work->categoryLift[i]--;
        }
        work->phase++;
        {
        for (i = 0; i < 2; i++) {
            float cosine;
            float alpha;
            work->sprites[i].x = 0x102 + i * 0xdd;
            cosine = xglCos((float)i * 3.1415927f);
            work->sprites[i].x = (int)((float)work->sprites[i].x + cosine * (float)-(work->slide * 2));
            work->sprites[i].y = 14;
            alpha = (128.0f - (float)work->slide * 0.0625f * 128.0f) * work->scale;
            work->sprites[i].alpha = alpha;
        }
        }
        {
        for (i = SHOP_ICON_CATEGORY_FIRST; i < SHOP_ICON_COUNT; i++) {
            float alpha;
            work->sprites[i].x = 0x121 + (i - SHOP_ICON_CATEGORY_FIRST) * SHOP_ICON_SPACING;
            work->sprites[i].x = (int)((float)work->sprites[i].x +
                xglCos((float)(i - SHOP_ICON_CATEGORY_FIRST) * 0.448798954f) * (float)-(work->slide * 2));
            work->sprites[i].y = work->categoryLift[i - SHOP_ICON_CATEGORY_FIRST] + 12;
            alpha = (128.0f - (float)work->slide * 0.0625f * 128.0f) * work->scale;
            work->sprites[i].alpha = alpha;
        }
        }
        for (i = 0; i < SHOP_ICON_COUNT; i++)
            eSpriteMain(&work->sprites[i]);
        break;
    }
}

void MenuShopParaSet2(int item, int mode)
{
    int attack[4];
    int defense[4];
    MenuShopUnitData savedUnit;
    ShopCalculatedPara previous;
    MenuShopUnitData *unit;
    ShopCalculatedPara *calculated;
    int i;

    unit = func_A191C0(3);
    savedUnit = *unit;
    for (i = 0; i < 3; i++) {
        unit->weaponIds[i] = unit->accessoryIds[i] = unit->attachmentIds[i] = 0;
    }
    previous = *func_00A11108(3, attack, defense);
    previous.attack += (unsigned short)attack[0];
    previous.physicalDefense += (unsigned short)defense[0];
    previous.magicDefense += (unsigned short)defense[1];

    switch (mode) {
    case 0:
        unit->accessoryIds[0] = item;
        break;
    case 1:
        unit->accessoryIds[0] = item;
        break;
    case 2:
        unit->attachmentIds[0] = item;
        break;
    }

    calculated = func_00A11108(3, attack, defense);
    calculated->attack += (unsigned short)attack[0];
    calculated->physicalDefense += (unsigned short)defense[0];
    calculated->magicDefense += (unsigned short)defense[1];
    MenuShopPara.value[0] = calculated->attack - previous.attack;
    MenuShopPara.value[1] = calculated->physicalDefense - previous.physicalDefense;
    MenuShopPara.value[2] = calculated->displayStat2 - previous.displayStat2;
    MenuShopPara.value[3] = calculated->magicDefense - previous.magicDefense;
    MenuShopPara.value[4] = (signed char)calculated->byteStats[0] - (signed char)previous.byteStats[0];
    MenuShopPara.value[5] = (signed char)calculated->byteStats[1] - (signed char)previous.byteStats[1];
    MenuShopPara.value[6] = (signed char)calculated->byteStats[2] - (signed char)previous.byteStats[2];
    *unit = savedUnit;
}

void MenuShopParaSet(unsigned int mode)
{
    int attack[4];
    int defense[4];
    MenuShopEquipmentValues *frame;
    MenuShopEngineValue *engine;
    ShopCalculatedPara *calculated;
    MenuShopUnitData *unit;
    MenuShopWorkData *selection;
    MenuShopWeapon *weapon;
    MenuShopAccessoryData *accessory;
    short oldEquipment;
    int i;

    memset(&MenuShopPara, 0, sizeof(MenuShopPara));
    memset(&MenuShopPara2, 0, sizeof(MenuShopPara2));
    switch (mode) {
    case SHOP_PARA_FRAME:
        if (MenuShopWork->equipmentId != 0) {
            frame = func_A1A598(MenuShopWork->equipmentId);
            MenuShopPara.value[0] = frame->value[0];
            MenuShopPara.value[2] = frame->value[1];
            MenuShopPara.value[3] = frame->value[2];
            MenuShopPara.value[4] = frame->value[3];
        }
        break;
    case SHOP_PARA_ENGINE:
        if (MenuShopWork->equipmentId != 0) {
            engine = func_A1A5E8(MenuShopWork->equipmentId);
            MenuShopPara.value[1] = engine->value;
        }
        break;
    case SHOP_PARA_RESTORE:
        if (MenuShopWork->unitId != 0) {
            calculated = func_00A11108(MenuShopWork->unitId, attack, defense);
            MenuAgwsParaSet(MenuShopWork->unitId, 0);
            for (i = 0; i < 6; i++) {
                MenuShopPara.value[i] = MenuAgwsPara.value[i];
                MenuShopPara2.value[i] = MenuAgwsPara.value[i];
            }
            MenuShopPara.value[0] = calculated->currentHp;
        }
        break;
    case SHOP_PARA_FRAME_FIT:
    case SHOP_PARA_ENGINE_FIT:
        unit = func_A191C0(MenuShopWork->unitId);
        MenuAgwsParaSet(MenuShopWork->unitId, 10);
        for (i = 0; i < 6; i++)
            MenuShopPara.value[i] = MenuAgwsPara.value[i];
        if (mode == SHOP_PARA_FRAME_FIT) {
            if (MenuFrameEquipCheck(MenuShopWork->unitId,
                                   MenuShopWork->equipmentId)) {
                selection = MenuShopWork;
                oldEquipment = unit->frameId;
                unit->frameId = selection->equipmentId;
            } else {
                for (i = 0; i < 6; i++)
                    MenuShopPara2.value[i] = MenuShopPara.value[i];
                break;
            }
        } else {
            if (MenuEngineEquipCheck(MenuShopWork->unitId,
                                    MenuShopWork->equipmentId)) {
                selection = MenuShopWork;
                oldEquipment = unit->engineId;
                unit->engineId = selection->equipmentId;
            } else {
                for (i = 0; i < 6; i++)
                    MenuShopPara2.value[i] = MenuShopPara.value[i];
                break;
            }
        }
        MenuAgwsParaSet(selection->unitId, 10);
        for (i = 0; i < 6; i++)
            MenuShopPara2.value[i] = MenuAgwsPara.value[i];
        if (mode == SHOP_PARA_FRAME_FIT)
            unit->frameId = oldEquipment;
        else
            unit->engineId = oldEquipment;
        break;
    case SHOP_PARA_CHAR_WEAPON:
        if ((short)(MenuShopWork->itemId >> 16) != ITEM_KIND_WEAPON)
            return;
        MenuShopParaSet2(MenuShopWork->modelId, 0);
        return;
    case SHOP_PARA_CHAR_ACCESSORY:
        if ((short)(MenuShopWork->itemId >> 16) != ITEM_KIND_ACCESSORY)
            return;
        MenuShopParaSet2(MenuShopWork->modelId, 1);
        return;
    case SHOP_PARA_CHAR_AMMO:
        if ((short)(MenuShopWork->itemId >> 16) == ITEM_KIND_BULLET)
            MenuShopPara.value[0] = *func_A1A428(MenuShopWork->modelId);
        break;
    case SHOP_PARA_AGWS_WEAPON:
        if ((short)(MenuShopWork->itemId >> 16) == ITEM_KIND_WEAPON) {
            weapon = func_A1A3D8(MenuShopWork->modelId);
            MenuShopPara.value[1] = weapon->shopParameter1;
            MenuShopPara.value[2] = weapon->shopParameter2;
            MenuShopPara.value[5] = weapon->wagl;
        }
        break;
    case SHOP_PARA_AGWS_ACCESSORY:
        if ((short)(MenuShopWork->itemId >> 16) == ITEM_KIND_ACCESSORY) {
            accessory = func_A1A548(MenuShopWork->modelId);
            MenuShopPara.value[2] = accessory->shopParameter2;
            MenuShopPara.value[3] = accessory->shopParameter3;
        }
        break;
    case SHOP_PARA_AGWS_AMMO:
        if ((short)(MenuShopWork->itemId >> 16) == ITEM_KIND_BULLET)
            MenuShopPara.value[1] = *func_A1A428(MenuShopWork->modelId);
        break;
    case SHOP_PARA_AGWS:
        if ((short)(MenuShopWork->itemId >> 16) == ITEM_KIND_AGWS) {
            MenuAgwsParaSet(MenuShopWork->modelId, 0);
            for (i = 0; i < 6; i++)
                MenuShopPara.value[i] = MenuAgwsPara.value[i];
        }
        break;
    }
}

static void MenuShopParameter(TskObject *task, void *data)
{
    struct MenuShopParameterDisplay *work = data;
    struct MenuShopParameterDigits8 characterDigits = menuShopCharacterDigitsTable[0];
    struct MenuShopParameterDigits7 agwsDigits = menuShopAgwsDigitsTable[0];
    short targetX;
    int i;

    switch (task->state) {
    case 0:
        work->depth = SHOP_DEPTH_PARAMETER;
        WindowDXSet(&work->window);
        work->window.x = -0x1f0;
        work->window.style = work->depth;
        work->window.y = 0x149;
        work->window.width = 0x1e0;
        work->window.height = 0x36;
        work->window.state = WINDOW_STATE_OPEN_REQUEST;
        WindowDXMain(&work->window);
        work->window.state = WINDOW_STATE_OPEN;
        for (i = 0; i < 9; i++) {
            eTagFontSet(&work->labels[i], 0);
            work->labels[i].work = work->depth + 2;
            work->labels[i].color[0] = 0x70;
        }
        for (i = 0; i < 9; i++) {
            eNumberSet(&work->numbers[i], 0);
            work->numbers[i].format = 5;
            work->numbers[i].work = work->depth + 2;
        }
        work->count = 0;
        work->twoColumns = 0;
        break;
    case 2:
        targetX = -0x1f0;
        switch (MenuShopWork->state) {
        case SHOP_STATE_BUY_LIST:
        {
            int category = MenuShopWork->category;
            switch (category) {
            case SHOP_CAT_CHAR_WEAPON:
            case SHOP_CAT_CHAR_ACCESSORY:
            case SHOP_CAT_CHAR_AMMO:
                    switch (category) {
                    case SHOP_CAT_CHAR_WEAPON: MenuShopParaSet(SHOP_PARA_CHAR_WEAPON); break;
                    case SHOP_CAT_CHAR_ACCESSORY: MenuShopParaSet(SHOP_PARA_CHAR_ACCESSORY); break;
                    case SHOP_CAT_CHAR_AMMO: MenuShopParaSet(SHOP_PARA_CHAR_AMMO); break;
                    }
                    work->twoColumns = 0;
                    work->count = SHOP_PARA_CHAR_COUNT;
                    for (i = 0; i < work->count; i++)
                        work->labels[i].render.text = msg00_9[i];
                    for (i = 0; i < work->count; i++) {
                        work->numbers[i].format = 5;
                        work->numbers[i].digits = characterDigits.digits[i];
                        work->numbers[i].colorModeSecond = 0;
                        work->numbers[i].firstOnly = 1;
                        work->numbers[i].value = MenuShopPara.value[i];
                    }
                break;
            case SHOP_CAT_AGWS_WEAPON:
            case SHOP_CAT_AGWS_ACCESSORY:
            case SHOP_CAT_AGWS_AMMO:
            case SHOP_CAT_AGWS:
                    switch (category) {
                    case SHOP_CAT_AGWS_WEAPON: MenuShopParaSet(SHOP_PARA_AGWS_WEAPON); break;
                    case SHOP_CAT_AGWS_ACCESSORY: MenuShopParaSet(SHOP_PARA_AGWS_ACCESSORY); break;
                    case SHOP_CAT_AGWS_AMMO: MenuShopParaSet(SHOP_PARA_AGWS_AMMO); break;
                    case SHOP_CAT_AGWS: MenuShopParaSet(SHOP_PARA_AGWS); break;
                    }
                    work->count = SHOP_PARA_AGWS_COUNT;
                    work->twoColumns = 1;
                    for (i = 0; i < work->count; i++)
                        work->labels[i].render.text = msg01_10[i];
                    for (i = 0; i < work->count; i++) {
                        work->numbers[i].format = 5;
                        work->numbers[i].digits = agwsDigits.digits[i];
                        work->numbers[i].colorModeSecond = 0;
                        work->numbers[i].firstOnly = 1;
                        work->numbers[i].value = MenuShopPara.value[i];
                    }
                break;
            }
        }
            /* fall through */
        case SHOP_STATE_QUANTITY:
        case SHOP_STATE_CONFIRM:
        case SHOP_STATE_RESULT:
            if (MenuShopWork->mode == SHOP_MODE_BUY && MenuShopWork->category != SHOP_CAT_ITEM && MenuShopWork->category != SHOP_CAT_CARD)
                targetX = 0x10;
            break;
        case SHOP_STATE_AGWS_BUY_LIST:
        case SHOP_STATE_AGWS_BUY_UNIT:
        case SHOP_STATE_AGWS_BUY_CONFIRM:
        case SHOP_STATE_AGWS_BUY_DONE:
            if (MenuShopWork->sort_mode_selector != 0)
                MenuShopParaSet(SHOP_PARA_FRAME_FIT);
            else
                MenuShopParaSet(SHOP_PARA_ENGINE_FIT);
            work->count = SHOP_PARA_AGWS_COUNT;
            work->twoColumns = 1;
            for (i = 0; i < work->count; i++)
                work->labels[i].render.text = msg01_10[i];
            for (i = 0; i < work->count; i++) {
                work->numbers[i].format = 5;
                work->numbers[i].digits = agwsDigits.digits[i];
                work->numbers[i].value = (unsigned int)MenuShopPara.value[i] +
                    ((unsigned int)MenuShopPara2.value[i] << 16);
                work->numbers[i].colorModeSecond = 0;
                work->numbers[i].firstOnly = 1;
                if (MenuShopPara.value[i] < MenuShopPara2.value[i]) {
                    work->numbers[i].colorModeSecond = 2;
                    work->numbers[i].firstOnly = 0;
                } else if (MenuShopPara2.value[i] < MenuShopPara.value[i]) {
                    work->numbers[i].colorModeSecond = 1;
                    work->numbers[i].firstOnly = 0;
                }
            }
            targetX = 0x10;
            break;
        case SHOP_STATE_RESTORE_UNIT:
        case SHOP_STATE_RESTORE_CONFIRM:
        case SHOP_STATE_RESTORE_DONE:
            MenuShopParaSet(SHOP_PARA_RESTORE);
            work->count = SHOP_PARA_AGWS_COUNT;
            work->twoColumns = 1;
            for (i = 0; i < work->count; i++)
                work->labels[i].render.text = msg01_10[i];
            for (i = 0; i < work->count; i++) {
                work->numbers[i].format = 5;
                work->numbers[i].colorModeSecond = 0;
                work->numbers[i].digits = agwsDigits.digits[i];
                work->numbers[i].firstOnly = 1;
                work->numbers[i].value = (unsigned int)MenuShopPara.value[i] +
                    ((unsigned int)MenuShopPara2.value[i] << 16);
            }
            work->numbers[0].format = 3;
            work->numbers[0].colorModeSecond = 0;
            work->numbers[0].firstOnly = 0;
            targetX = 0x10;
            break;
        }
        MoveSlide(&work->window.x, &targetX, SHOP_SLIDE_RATE);
        if (work->twoColumns == 0) {
            work->window.y = 0x139;
            work->window.height = 0x36;
        } else {
            work->window.y = 0x149;
            work->window.height = 0x26;
        }
        WindowDXMain(&work->window);
        for (i = 0; i < work->count; i++) {
            if (work->twoColumns == 0) {
                work->labels[i].x = (unsigned short)work->window.x + (i / 3) * 0xa0 + 0x30;
                work->labels[i].y = (unsigned short)work->window.y + (i % 3) * 0x10 + 3;
            } else {
                work->labels[i].x = (unsigned short)work->window.x + (i / 2) * 0xa0 + 0x30;
                work->labels[i].y = (unsigned short)work->window.y + (i % 2) * 0x10 + 3;
            }
            eTagFontMain(&work->labels[i]);
        }
        for (i = 0; i < work->count; i++) {
            if (work->twoColumns == 0) {
                work->numbers[i].x = (unsigned short)work->window.x + (i / 3) * 0xa0 + 0x60;
                work->numbers[i].y = (unsigned short)work->window.y + (i % 3) * 0x10 + 3;
            } else {
                work->numbers[i].x = (unsigned short)work->window.x + (i / 2) * 0xa0 + 0x60;
                work->numbers[i].y = (unsigned short)work->window.y + (i % 2) * 0x10 + 3;
            }
            eNumberMain(&work->numbers[i]);
        }
        return;
    }
}

static void MenuShopEx2(TskObject *task, void *data)
{
    MenuShopEx2Work *work = data;
    short x;
    MenuShopWeapon *weapon;

    switch (task->state) {
    case 0:
        work->color = SHOP_DEPTH_PARAMETER;
        WindowDXSet(&work->window);
        work->window.x = -0xB6;
        work->window.style = work->color;
        work->window.y = 0x100;
        work->window.width = 0xA6;
        work->window.height = 0x1E;
        work->window.tag_id = menuShopBulletTitleText;
        work->window.state = WINDOW_STATE_OPEN_REQUEST;
        WindowDXMain(&work->window);
        work->window.state = WINDOW_STATE_OPEN;
        eMessageSet(&work->message, 0);
        work->message.mode = 0x20;
        work->message.draw_parameter = work->color + 2;
        break;

    case 2:
        x = -0xB6;
        if (MenuShopWork->state == SHOP_STATE_BUY_LIST && MenuShopWork->category == SHOP_CAT_AGWS_WEAPON && MenuShopWork->listIndex >= 0) {
            weapon = func_A1A3D8((short)MenuSortGet(0, MenuShopWork->listIndex));
            if (weapon != 0 && (weapon->flags & SHOP_WEAPON_FLAG_BULLET) && weapon->bullet != 0) {
                work->message.text = *func_A2C738(weapon->bullet);
                x = 0x10;
            }
        }
        MoveSlide(&work->window.x, &x, SHOP_SLIDE_RATE);
        WindowDXMain(&work->window);
        work->message.x = work->window.x + 3;
        work->message.y = work->window.y + 3;
        eMessageMain(&work->message);
        break;
    }
}

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
/* MenuShopEx2 takes the window title by address; the string follows it in .sdata. */
const char menuShopBulletTitleText[8] = "Bullet";

static void MenuShopLine(TskObject *task, void *data)
{
    MenuShopDividerWork *work = data;
    short targets[4];
    short verticalTarget[1];
    int i;
    short *target;

    switch (task->state) {
    case 0:
        work->depth = SHOP_DEPTH_TOP;
        for (i = 0; i < 5; i++) {
            work->vertices[i].rgba.r = work->vertices[i].rgba.g =
                work->vertices[i].rgba.b = 96;
            work->vertices[i].rgba.a = 128;
            work->vertices[i].x = 0;
            work->vertices[i].y = 48;
            work->vertices[i].depth = work->depth;
        }
        work->vertices[5].depth = 0;
        break;
    case 2:
        verticalTarget[0] = 48;
        targets[0] = MenuShopWork->category * SHOP_ICON_SPACING + 294;
        targets[1] = MenuShopWork->category * SHOP_ICON_SPACING + 298;
        targets[2] = MenuShopWork->category * SHOP_ICON_SPACING + 302;
        targets[3] = SHOP_LINE_END_X;
        switch (MenuShopWork->state) {
        case SHOP_STATE_BUY_LIST:
        case SHOP_STATE_SELL_LIST:
        case SHOP_STATE_QUANTITY:
        case SHOP_STATE_CONFIRM:
        case SHOP_STATE_RESULT:
            if (targets[0] == work->vertices[1].x)
                verticalTarget[0] = 40;
            if (work->alpha != SHOP_LINE_ALPHA_MAX)
                work->alpha += SHOP_LINE_ALPHA_STEP;
            break;
        default:
            if (work->alpha != 0)
                work->alpha -= SHOP_LINE_ALPHA_STEP;
            break;
        }
        work->vertices[0].x = 0;
        work->vertices[5].x = SHOP_LINE_END_X;
        target = targets;
        for (i = 1; i < 5; i++, target++) {
            MoveSlide(&work->vertices[i].x, target, SHOP_SLIDE_RATE);
            work->vertices[i].rgba.a = work->alpha;
        }
        work->vertices[0].rgba.a = work->alpha;
        MoveSlide(&work->vertices[2].y, verticalTarget, 8.0f);
        endPrintExtFunc(work->depth, END_PRINT_CMD_LINE, work->vertices);
        break;
    }
}

void MenuShopInfoSet(void)
{
    MenuShopWork->categoryMask = 0;

    if (MenuShopWork->mode == SHOP_MODE_BUY) {
        if (MenuShopWork->noCategories != 0)
            return;
        if (ShopData.records[SHOP_CAT_ITEM].count != 0)
            MenuShopWork->categoryMask = SHOP_CAT_MASK_ITEM;
        if (ShopData.records[SHOP_CAT_CHAR_WEAPON].count != 0)
            MenuShopWork->categoryMask |= SHOP_CAT_MASK_CHAR_WEAPON;
        if (ShopData.records[SHOP_CAT_CHAR_ACCESSORY].count != 0)
            MenuShopWork->categoryMask |= SHOP_CAT_MASK_CHAR_ACCESSORY;
        if (ShopData.records[SHOP_CAT_CHAR_AMMO].count != 0)
            MenuShopWork->categoryMask |= SHOP_CAT_MASK_CHAR_AMMO;
        if (ShopData.records[SHOP_CAT_AGWS_WEAPON].count != 0)
            MenuShopWork->categoryMask |= SHOP_CAT_MASK_AGWS_WEAPON;
        if (ShopData.records[SHOP_CAT_AGWS_ACCESSORY].count != 0)
            MenuShopWork->categoryMask |= SHOP_CAT_MASK_AGWS_ACCESSORY;
        if (ShopData.records[SHOP_CAT_AGWS_AMMO].count != 0)
            MenuShopWork->categoryMask |= SHOP_CAT_MASK_AGWS_AMMO;
        if (ShopData.tail.count != 0)
            MenuShopWork->categoryMask |= SHOP_CAT_MASK_AGWS;
        if (MenuScenarioNo >= SHOP_CARD_SCENARIO_FIRST)
            MenuShopWork->categoryMask |= SHOP_CAT_MASK_CARD;
    } else {
        MenuSortSet(0, SHOP_SORT_ITEM, 0);
        if (MenuSortCheck(0) != 0)
            MenuShopWork->categoryMask |= SHOP_CAT_MASK_ITEM;
        MenuSortSet(0, SHOP_SORT_WEAPON, SHOP_SORT_FILTER_CHARACTER);
        if (MenuSortCheck(0) != 0)
            MenuShopWork->categoryMask |= SHOP_CAT_MASK_CHAR_WEAPON;
        MenuSortSet(0, SHOP_SORT_ACCESSORY, SHOP_SORT_FILTER_CHARACTER);
        if (MenuSortCheck(0) != 0)
            MenuShopWork->categoryMask |= SHOP_CAT_MASK_CHAR_ACCESSORY;
        MenuSortSet(0, SHOP_SORT_BULLET, SHOP_SORT_FILTER_CHARACTER);
        if (MenuSortCheck(0) != 0)
            MenuShopWork->categoryMask |= SHOP_CAT_MASK_CHAR_AMMO;
        MenuSortSet(0, SHOP_SORT_WEAPON, SHOP_SORT_FILTER_AGWS);
        if (MenuSortCheck(0) != 0)
            MenuShopWork->categoryMask |= SHOP_CAT_MASK_AGWS_WEAPON;
        MenuSortSet(0, SHOP_SORT_ACCESSORY, SHOP_SORT_FILTER_AGWS);
        if (MenuSortCheck(0) != 0)
            MenuShopWork->categoryMask |= SHOP_CAT_MASK_AGWS_ACCESSORY;
        MenuSortSet(0, SHOP_SORT_BULLET, SHOP_SORT_FILTER_AGWS);
        if (MenuSortCheck(0) != 0)
            MenuShopWork->categoryMask |= SHOP_CAT_MASK_AGWS_AMMO;
        if (dataEvtBoxChk(SHOP_CARD_EVENT_1) != 0 || dataEvtBoxChk(SHOP_CARD_EVENT_2) != 0 || dataEvtBoxChk(SHOP_CARD_EVENT_3) != 0)
            MenuShopWork->categoryMask |= SHOP_CAT_MASK_CARD;
    }
}

static inline MenuShopSelection *MenuShopSelectionGet(void)
{
    return (MenuShopSelection *)&MenuShopWork->mode;
}

static inline MenuShopItem *MenuShopItemGet(void)
{
    return (MenuShopItem *)&MenuShopWork->listIndex;
}

static inline MenuShopAgws *MenuShopAgwsGet(void)
{
    return (MenuShopAgws *)&MenuShopWork->unitId;
}

static inline MenuShopParty *MenuShopPartyGet(void)
{
    return (MenuShopParty *)MenuShopWork->unitIds;
}

void MenuShopCore(void) {
    if (MenuShopWork->state != MenuShopWork->nextState) {
        MenuShopWork->state = MenuShopWork->nextState;
        MenuShopWork->stateFlags |= SHOP_FLAG_STATE_ENTERED;
    } else {
        MenuShopWork->stateFlags &= 0xFE;
    }
    if (MenuShopWork->waitFrames != 0) {
        MenuShopWork->waitFrames--;
    }

    switch (MenuShopWork->state) {
    case SHOP_STATE_MODE_SELECT:
        if (MenuShopWork->stateFlags & SHOP_FLAG_STATE_ENTERED) {
            MenuShopWork->stateFlags |= 0x10;
            MenuShopWork->waitFrames = SHOP_WAIT_ENTER;
            break;
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        MenuShopWork->mode = MenuSelectMove(MenuShopWork->mode, 4, 0);
        if (PadData.half_2a == PAD_CIRCLE)
            switch (MenuShopWork->mode) {
            case SHOP_MODE_BUY:
            case SHOP_MODE_SELL: {
                u8 listStates[2] = { SHOP_STATE_BUY_LIST, SHOP_STATE_SELL_LIST };
                u8 *nextStates = listStates;
                int categoryIndex;

                MenuShopInfoSet();
                if (MenuShopWork->categoryMask != 0) {
                    for (categoryIndex = 0; categoryIndex < SHOP_CAT_CARD; categoryIndex++) {
                        if ((MenuShopWork->categoryMask >> categoryIndex) & 1) {
                            MenuShopWork->category = categoryIndex;
                            break;
                        }
                    }
                    MenuShopWork->nextState = nextStates[MenuShopWork->mode];
                    MenuShopWork->stateFlags |= 0xC;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                    MenuShopWork->waitFrames = SHOP_WAIT_TRANSITION;
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
                break;
            }
            case SHOP_MODE_TUNE_AGWS:
                if (MenuShopWork->equipmentStatus != 0) {
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                    MenuShopWork->nextState = SHOP_STATE_AGWS_MENU;
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
                break;
            case SHOP_MODE_QUIT:
                xglSoundEffectNormalID(SE_CANCEL, 0);
                MenuShopWork->nextState = SHOP_STATE_EXIT;
                break;
            }
        if (PadData.half_2a & PAD_CROSS) {
            MenuShopWork->nextState = SHOP_STATE_EXIT;
            xglSoundEffectNormalID(SE_CANCEL, 0);
        }
        break;

    case SHOP_STATE_BUY_LIST:
    case SHOP_STATE_SELL_LIST:
        if (MenuShopWork->stateFlags & SHOP_FLAG_STATE_ENTERED) {
            MenuShopWork->listIndex = MenuShopListChange00(1);
            MenuShopWork->itemId = MenuShopWork->modelId = 0;
            MenuShopWork->availableMoney = dataMoneyBoxChk();
        }
        if (MenuShopWork->waitFrames == 0) {
            MenuShopWork->listIndex = WindowSPSelect(MenuShopWinSP,PadData.mappedRepeat);
            WindowSPKeepSelect(MenuShopWinSP,&MenuKeepSelect[(MenuShopWork->category + MenuShopWork->mode * SHOP_CAT_COUNT) * MENU_KEEP_ENTRY_SIZE]);
            if (MenuShopWork->listIndex >= 0) {
                MenuShopWork->canEquip = subMenuShopEquipCheck(MenuSortGet(0, MenuShopWork->listIndex), MenuShopWork->category);
            } else {
                MenuShopWork->canEquip = 0;
            }
            if (MenuShopWork->listIndex >= 0) {
                MENU_SHOP_PREVIEW_ITEM(MenuShopWork->listIndex);
            } else {
                MenuShopWork->itemId = MenuShopWork->modelId = 0;
            }
            if (PadData.mappedRepeat & (PAD_LEFT | PAD_RIGHT)) {
                u8 previousCategory;
                int categoryStep;
                int category;
                int count;

                previousCategory = MenuShopWork->category;
                WindowSPKeepSelectCheck(&MenuKeepSelect[(previousCategory + MenuShopWork->mode * SHOP_CAT_COUNT) * MENU_KEEP_ENTRY_SIZE]);
                categoryStep = (PadData.mappedRepeat & PAD_RIGHT) ? 1 : -1;
                category = MenuShopWork->category;
                for (count = 0; count < SHOP_CAT_COUNT; count += categoryStep) {
                    category += categoryStep;
                    if (category < 0) {
                        category = SHOP_CAT_CARD;
                    }
                    if (category >= SHOP_CAT_COUNT) {
                        category = SHOP_CAT_ITEM;
                    }
                    if ((MenuShopWork->categoryMask >> category) & 1) {
                        MenuShopWork->category = category;
                        break;
                    }
                }
                if (previousCategory != MenuShopWork->category) {
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                    MenuShopWork->listIndex = MenuShopListChange00(0);
                    if (MenuShopWork->listIndex >= 0) {
                        MenuShopWork->canEquip = subMenuShopEquipCheck(MenuSortGet(0, MenuShopWork->listIndex), MenuShopWork->category);
                        MENU_SHOP_PREVIEW_ITEM(MenuShopWork->listIndex);
                    }
                    MenuShopWork->waitFrames = SHOP_WAIT_ENTER;
                }
            } else if (PadData.half_2a & PAD_CIRCLE) {
                if (MenuSortCheck(0) != 0) {
                    if (MenuShopWork->mode == SHOP_MODE_BUY) {
                        if (MenuBoxMoneyGet(MenuShopWork->itemId, 0) > MenuShopWork->availableMoney
                            || MenuBoxChk(MenuShopWork->itemId) >= SHOP_STOCK_MAX) {
                            xglSoundEffectNormalID(SE_BUZZER, 0);
                        } else {
                            MenuShopWork->nextState = SHOP_STATE_QUANTITY;
                            MenuShopWork->quantity = 1;
                            MenuShopWork->quantityLimit = MenuBoxChk(MenuShopWork->itemId);
                            xglSoundEffectNormalID(SE_DECIDE, 0);
                        }
                    } else if (MenuShopNoSaleCheck(MenuShopWork->itemId) != 0) {
                        xglSoundEffectNormalID(SE_BUZZER, 0);
                    } else {
                        MenuShopWork->nextState = SHOP_STATE_QUANTITY;
                        MenuShopWork->quantity = 1;
                        MenuShopWork->quantityLimit = MenuBoxChk(MenuShopWork->itemId);
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                    }
                }
            } else if (PadData.half_2a & PAD_CROSS) {
                if (MenuCursorKeepCheck() == 0) {
                    MenuShopWork->mode = SHOP_MODE_BUY;
                }
                MenuShopWork->nextState = SHOP_STATE_MODE_SELECT;
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
        } else {
            MenuShopWork->listIndex = WindowSPSelect(MenuShopWinSP,0);
        }
        break;

    case SHOP_STATE_QUANTITY: {
        struct MenuShopQuantityChangeState {
            int maximumQuantity;
            int calculatedTotal;
            int playChangeSound;
            int minimumQuantity;
        } quantityState;
        int i;

        void subMenuShopNumerInc(void)
        {
            int unitCost;

            if (MenuShopWork->quantity < quantityState.maximumQuantity) {
                unitCost = MenuBoxMoneyGet(MenuShopWork->itemId, 0);
                quantityState.calculatedTotal = unitCost * (MenuShopWork->quantity + 1);
                if (MenuShopWork->availableMoney >= quantityState.calculatedTotal) {
                    MenuShopWork->quantity++;
                    if (quantityState.playChangeSound != 0) {
                        xglSoundEffectNormalID(SE_CURSOR, 0);
                        quantityState.playChangeSound = 0;
                    }
                }
            }
        }

        void subMenuShopNumerDec(void)
        {
            if (quantityState.minimumQuantity < MenuShopWork->quantity) {
                MenuShopWork->quantity = MenuShopWork->quantity - 1;
                if (quantityState.playChangeSound != 0) {
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                    quantityState.playChangeSound = 0;
                }
            }
        }

        if (MenuShopWork->stateFlags & SHOP_FLAG_STATE_ENTERED) {
            if (MenuShopWork->category == SHOP_CAT_AGWS) {
                MenuShopWork->nextState = SHOP_STATE_CONFIRM;
                MenuShopWork->waitFrames = 1;
            } else {
                MenuShopWork->waitFrames = SHOP_WAIT_ENTER;
            }
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        if (MenuShopWork->mode == SHOP_MODE_BUY) {
            quantityState.maximumQuantity = SHOP_STOCK_MAX - MenuShopWork->quantityLimit;
            quantityState.minimumQuantity = 1;
            quantityState.playChangeSound = 1;
            quantityState.calculatedTotal = 0;
            if (PadData.mappedRepeat & PAD_DOWN) {
                subMenuShopNumerDec();
            }
            if (PadData.mappedRepeat & PAD_UP) {
                subMenuShopNumerInc();
            }
            if (PadData.mappedRepeat & PAD_RIGHT) {
                for (i = 0; i < SHOP_QUANTITY_PAGE_STEP; i++) {
                    subMenuShopNumerInc();
                }
            }
            if (PadData.mappedRepeat & PAD_LEFT) {
                for (i = 0; i < SHOP_QUANTITY_PAGE_STEP; i++) {
                    subMenuShopNumerDec();
                }
            }
        } else {
            int minimumQuantity = 1;
            u8 quantityLimit;

            quantityLimit = MenuShopSelectionGet()->quantityLimit;
            if ((PadData.mappedRepeat & PAD_DOWN) && MenuShopSelectionGet()->quantity != minimumQuantity) {
                MenuShopSelectionGet()->quantity--;
                xglSoundEffectNormalID(SE_CURSOR, 0);
            }
            if ((PadData.mappedRepeat & PAD_UP) && MenuShopSelectionGet()->quantity != quantityLimit) {
                MenuShopSelectionGet()->quantity++;
                xglSoundEffectNormalID(SE_CURSOR, 0);
            }
            if ((PadData.mappedRepeat & PAD_RIGHT) && MenuShopSelectionGet()->quantity != quantityLimit) {
                MenuShopSelectionGet()->quantity += SHOP_QUANTITY_PAGE_STEP;
                if (MenuShopSelectionGet()->quantity > quantityLimit) {
                    MenuShopSelectionGet()->quantity = quantityLimit;
                }
                xglSoundEffectNormalID(SE_CURSOR, 0);
            }
            if ((PadData.mappedRepeat & PAD_LEFT) && MenuShopSelectionGet()->quantity != minimumQuantity) {
                MenuShopSelectionGet()->quantity -= SHOP_QUANTITY_PAGE_STEP;
                if (MenuShopSelectionGet()->quantity < minimumQuantity) {
                    MenuShopSelectionGet()->quantity = minimumQuantity;
                }
                xglSoundEffectNormalID(SE_CURSOR, 0);
            }
        }
        if (PadData.half_2a & PAD_CIRCLE) {
            MenuShopWork->nextState = SHOP_STATE_CONFIRM;
            xglSoundEffectNormalID(SE_DECIDE, 0);
        }
        if (PadData.half_2a & PAD_CROSS) {
            if (MenuShopWork->mode == SHOP_MODE_BUY) {
                MenuShopWork->nextState = SHOP_STATE_BUY_LIST;
            } else {
                MenuShopWork->nextState = SHOP_STATE_SELL_LIST;
            }
            xglSoundEffectNormalID(SE_CANCEL, 0);
        }
        break;
    }

    case SHOP_STATE_CONFIRM:
        if (MenuShopWork->stateFlags & SHOP_FLAG_STATE_ENTERED) {
            MenuShopWork->quantityAction = 0;
            MenuShopWork->waitFrames = SHOP_WAIT_ENTER;
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        MenuShopWork->quantityAction = MenuSelectMove(MenuShopWork->quantityAction, 2, 0);
        if (PadData.half_2a & PAD_CIRCLE) {
            if (MenuShopSelectionGet()->quantityAction == 0) {
                int i;

                if (MenuShopSelectionGet()->mode == SHOP_MODE_BUY) {
                    for (i = 0; i < MenuShopSelectionGet()->quantity; i++) {
                        MenuBoxInc(MenuSortGet(0, MenuShopWork->listIndex));
                    }
                    dataMoneyBoxDec(MenuBoxMoneyGet(MenuShopWork->itemId, 0) * MenuShopWork->quantity);
                } else {
                    for (i = 0; i < MenuShopSelectionGet()->quantity; i++) {
                        MenuBoxDec(MenuShopWork->itemId);
                    }
                    dataMoneyBoxInc(MenuBoxMoneyGet(MenuShopWork->itemId, 1) * MenuShopWork->quantity);
                    MenuShopWork->stateFlags |= 0x10;
                }
                MenuShopWork->nextState = SHOP_STATE_RESULT;
            } else if (MenuShopSelectionGet()->category == SHOP_CAT_AGWS) {
                u8 listStates[2] = { SHOP_STATE_BUY_LIST, SHOP_STATE_SELL_LIST };

                MenuShopWork->nextState = *(listStates + MenuShopSelectionGet()->mode);
            } else {
                MenuShopWork->nextState = SHOP_STATE_QUANTITY;
            }
            xglSoundEffectNormalID(SE_DECIDE, 0);
        }
        if (PadData.half_2a & PAD_CROSS) {
            if (MenuShopSelectionGet()->category == SHOP_CAT_AGWS) {
                u8 listStates[2] = { SHOP_STATE_BUY_LIST, SHOP_STATE_SELL_LIST };

                MenuShopWork->nextState = *(listStates + MenuShopSelectionGet()->mode);
            } else {
                MenuShopWork->nextState = SHOP_STATE_QUANTITY;
            }
            xglSoundEffectNormalID(SE_CANCEL, 0);
        }
        break;

    case SHOP_STATE_RESULT:
        if (MenuShopWork->stateFlags & SHOP_FLAG_STATE_ENTERED) {
            if (MenuShopWork->mode == SHOP_MODE_BUY && MenuShopWork->category == SHOP_CAT_AGWS) {
                int partyUnitIds[16];
                int i;

                MenuShopWork->unitCount = PartyAgwsGet(partyUnitIds);
                for (i = 0; i < MenuShopWork->unitCount; i++) {
                    MenuShopPartyGet()->unitIds[i] = partyUnitIds[i];
                }
                MenuShopAgwsListChange();
            }
            MenuShopWork->listIndex = MenuShopListChange00(1);
            MenuShopWork->waitFrames = SHOP_WAIT_ENTER;
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        if (PadData.half_2a & PAD_CIRCLE) {
            u8 listStates[2] = { SHOP_STATE_BUY_LIST, SHOP_STATE_SELL_LIST };

            MenuShopWork->nextState = *(listStates + MenuShopWork->mode);
            if (MenuShopWork->mode == SHOP_MODE_BUY) {
                MenuShopListColorChange(0, 0);
            }
            xglSoundEffectNormalID(SE_DECIDE, 0);
        }
        break;

    case SHOP_STATE_AGWS_MENU:
        if (MenuShopWork->stateFlags & SHOP_FLAG_STATE_ENTERED) {
            MenuShopWork->waitFrames = SHOP_WAIT_ENTER;
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        MenuShopWork->sortOption = MenuSelectMove(MenuShopWork->sortOption, SHOP_AGWS_CMD_COUNT, 0);
        if (PadData.mappedPressed == PAD_CIRCLE) {
            switch (MenuShopWork->sortOption) {
            case SHOP_AGWS_CMD_FRAME:
            case SHOP_AGWS_CMD_GENERATOR:
                if (MenuShopWork->sortOption == SHOP_AGWS_CMD_FRAME) {
                    MenuShopWork->sort_mode_selector = 1;
                } else {
                    MenuShopWork->sort_mode_selector = 0;
                }
                xglSoundEffectNormalID(SE_DECIDE, 0);
                if (MenuShopWork->sort_mode_selector != 0) {
                    WindowSPKeepSelectCheck(&MenuKeepSelect[SHOP_KEEP_FRAME_LIST]);
                } else {
                    WindowSPKeepSelectCheck(&MenuKeepSelect[SHOP_KEEP_ENGINE_LIST]);
                }
                MenuShopWork->listIndex = MenuShopListChange01(0);
                MenuShopWork->nextState = SHOP_STATE_AGWS_BUY_LIST;
                MenuShopWork->waitFrames = SHOP_WAIT_TRANSITION;
                break;
            case SHOP_AGWS_CMD_RESTORE:
                if (dataMoneyBoxChk() >= SHOP_RESTORE_COST) {
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                    MenuShopWork->nextState = SHOP_STATE_RESTORE_UNIT;
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
                MenuShopWork->waitFrames = SHOP_WAIT_TRANSITION;
                break;
            case SHOP_AGWS_CMD_QUIT:
                xglSoundEffectNormalID(SE_CANCEL, 0);
                MenuShopWork->nextState = SHOP_STATE_MODE_SELECT;
                break;
            }
        }
        if (PadData.mappedPressed == PAD_CROSS) {
            if (MenuCursorKeepCheck() == 0) {
                MenuShopWork->mode = SHOP_MODE_BUY;
            }
            MenuShopWork->nextState = SHOP_STATE_MODE_SELECT;
            xglSoundEffectNormalID(SE_CANCEL, 0);
        }
        break;

    case SHOP_STATE_RESTORE_UNIT:
        if (MenuShopWork->stateFlags & SHOP_FLAG_STATE_ENTERED) {
            MenuShopWork->waitFrames = SHOP_WAIT_TRANSITION;
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        MenuShopWork->unitSelection = MenuSelectMove2(MenuShopAgwsGet()->unitSelection, MenuShopAgwsGet()->unitCount, 0);
        MenuShopAgwsGet()->unitId = MenuShopPartyGet()->unitIds[MenuShopAgwsGet()->unitSelection];
        if (PadData.mappedPressed == PAD_CIRCLE) {
            if (dataMoneyBoxChk() >= SHOP_RESTORE_COST && MenuCharHpCheck(MenuShopWork->unitId) == 0) {
                MenuShopWork->nextState = SHOP_STATE_RESTORE_CONFIRM;
                xglSoundEffectNormalID(SE_DECIDE, 0);
            } else {
                xglSoundEffectNormalID(SE_BUZZER, 0);
            }
        }
        if (PadData.mappedPressed == PAD_CROSS) {
            if (MenuCursorKeepCheck() == 0) {
                MenuShopWork->sortOption = 0;
            }
            MenuShopWork->nextState = SHOP_STATE_AGWS_MENU;
            xglSoundEffectNormalID(SE_CANCEL, 0);
        }
        break;

    case SHOP_STATE_RESTORE_CONFIRM:
        if (MenuShopWork->stateFlags & SHOP_FLAG_STATE_ENTERED) {
            MenuShopWork->quantityAction = 0;
            MenuShopWork->waitFrames = SHOP_WAIT_ENTER;
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        MenuShopWork->quantityAction = MenuSelectMove(MenuShopWork->quantityAction, 2, 0);
        if (PadData.mappedPressed == PAD_CIRCLE) {
            if (MenuShopWork->quantityAction == 0) {
                MenuShopWork->nextState = SHOP_STATE_RESTORE_DONE;
                xglSoundEffectNormalID(SE_DECIDE, 0);
            } else {
                MenuShopWork->nextState = SHOP_STATE_RESTORE_UNIT;
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
        }
        if (PadData.mappedPressed == PAD_CROSS) {
            MenuShopWork->nextState = SHOP_STATE_RESTORE_UNIT;
            xglSoundEffectNormalID(SE_CANCEL, 0);
        }
        break;

    case SHOP_STATE_RESTORE_DONE:
        if (MenuShopWork->stateFlags & SHOP_FLAG_STATE_ENTERED) {
            int repairAttack[4];
            int repairDefense[4];
            MenuShopUnitData *unit;
            ShopCalculatedPara *para;

            unit = func_A191C0(MenuShopPartyGet()->unitIds[MenuShopWork->unitSelection]);
            para = func_00A11108(MenuShopPartyGet()->unitIds[MenuShopWork->unitSelection], repairAttack, repairDefense);
            unit->hitPoints = para->maxHp;
            dataMoneyBoxDec(SHOP_RESTORE_COST);
            MenuShopWork->waitFrames = SHOP_WAIT_SHORT;
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        if (PadData.mappedPressed == PAD_CIRCLE) {
            if (dataMoneyBoxChk() >= SHOP_RESTORE_COST) {
                MenuShopWork->nextState = SHOP_STATE_RESTORE_UNIT;
            } else {
                MenuShopWork->nextState = SHOP_STATE_AGWS_MENU;
            }
            xglSoundEffectNormalID(SE_DECIDE, 0);
        }
        break;

    case SHOP_STATE_AGWS_BUY_LIST:
        if (MenuShopWork->waitFrames == 0) {
            MenuShopWork->listIndex = WindowSPSelect(MenuShopWinSP,PadData.mappedRepeat);
            if (MenuShopWork->sort_mode_selector != 0) {
                WindowSPKeepSelect(MenuShopWinSP,&MenuKeepSelect[SHOP_KEEP_FRAME_LIST]);
            } else {
                WindowSPKeepSelect(MenuShopWinSP,&MenuKeepSelect[SHOP_KEEP_ENGINE_LIST]);
            }
            if (MenuShopWork->listIndex >= 0) {
                int itemId;
                int unitId;

                itemId = MenuSortGet(0, MenuShopWork->listIndex);
                MenuShopItemGet()->itemId = itemId;
                MenuShopItemGet()->modelId = (s16)MenuShopItemGet()->itemId;
                if (MenuShopItemGet()->listIndex >= 0) {
                    MenuShopAgwsGet()->equipmentId = MenuShopItemGet()->modelId;
                    MenuShopAgwsGet()->unitId = SHOP_AGWS_UNIT_FIRST;
                    for (unitId = SHOP_AGWS_UNIT_FIRST; unitId < SHOP_AGWS_UNIT_END; unitId++) {
                        if (MenuFrameEquipCheck(unitId, MenuShopWork->equipmentId) != 0) {
                            MenuShopWork->unitId = unitId;
                            break;
                        }
                    }
                    if (PadData.mappedPressed == PAD_CIRCLE) {
                        int money;

                        money = dataMoneyBoxChk();
                        if (money >= MenuBoxMoneyGet(MenuShopWork->itemId, 0)) {
                            MenuShopWork->nextState = SHOP_STATE_AGWS_BUY_CONFIRM;
                            xglSoundEffectNormalID(SE_DECIDE, 0);
                            MenuShopWork->waitFrames = SHOP_WAIT_SHORT;
                            if (MenuCursorKeepCheck() == 0) {
                                MenuShopWork->unitSelection = 0;
                            }
                        } else {
                            xglSoundEffectNormalID(SE_BUZZER, 0);
                            MenuShopWork->waitFrames = SHOP_WAIT_SHORT;
                        }
                    }
                }
            }
            if (PadData.mappedPressed == PAD_CROSS) {
                if (MenuCursorKeepCheck() == 0) {
                    MenuShopWork->sortOption = 0;
                }
                MenuShopWork->nextState = SHOP_STATE_AGWS_MENU;
                MenuShopWork->waitFrames = SHOP_WAIT_SHORT;
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
        } else {
            MenuShopWork->listIndex = WindowSPSelect(MenuShopWinSP,0);
            if (MenuShopWork->listIndex >= 0) {
                int itemId;

                itemId = MenuSortGet(0, MenuShopWork->listIndex);
                MenuShopItemGet()->itemId = itemId;
                MenuShopItemGet()->modelId = (s16)MenuShopItemGet()->itemId;
                MenuShopWork->equipmentId = MenuShopItemGet()->modelId;
            }
        }
        break;

    case SHOP_STATE_AGWS_BUY_UNIT: {
        MenuShopUnitData *unit;
        int unitSelection;
        int canEquip;
        s16 previousEquipmentId;

        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        unitSelection = MenuSelectMove2(MenuShopAgwsGet()->unitSelection, MenuShopAgwsGet()->unitCount, 0);
        if (MenuShopAgwsGet()->unitSelection != unitSelection) {
            MenuAgwsParaSet(MenuShopAgwsGet()->unitId, 10);
        }
        MenuShopWork->unitSelection = unitSelection;
        MenuShopAgwsGet()->unitId = MenuShopPartyGet()->unitIds[MenuShopAgwsGet()->unitSelection];
        if (PadData.mappedPressed == PAD_CIRCLE) {
            if (MenuShopAgwsGet()->sort_mode_selector != 0) {
                canEquip = MenuFrameEquipCheck(MenuShopAgwsGet()->unitId, MenuShopAgwsGet()->equipmentId);
            } else {
                canEquip = MenuEngineEquipCheck(MenuShopAgwsGet()->unitId, MenuShopAgwsGet()->equipmentId);
            }
            if (canEquip != 0) {
                MenuShopWork->nextState = SHOP_STATE_AGWS_BUY_CONFIRM;
                xglSoundEffectNormalID(SE_DECIDE, 0);
            } else {
                xglSoundEffectNormalID(SE_BUZZER, 0);
            }
        }
        if (PadData.mappedPressed == PAD_CROSS) {
            MenuShopWork->nextState = SHOP_STATE_AGWS_BUY_LIST;
            xglSoundEffectNormalID(SE_CANCEL, 0);
        }
        unit = func_A191C0(MenuShopWork->unitId);
        if (MenuShopAgwsGet()->sort_mode_selector != 0) {
            previousEquipmentId = unit->frameId;
            unit->frameId = MenuShopAgwsGet()->equipmentId;
        } else {
            previousEquipmentId = unit->engineId;
            unit->engineId = MenuShopAgwsGet()->equipmentId;
        }
        MenuAgwsParaSet(MenuShopWork->unitId, 1);
        if (MenuShopWork->sort_mode_selector != 0) {
            unit->frameId = previousEquipmentId;
        } else {
            unit->engineId = previousEquipmentId;
        }
        break;
    }

    case SHOP_STATE_AGWS_BUY_CONFIRM:
        if (MenuShopWork->stateFlags & SHOP_FLAG_STATE_ENTERED) {
            MenuShopWork->quantityAction = 0;
            MenuShopWork->waitFrames = SHOP_WAIT_ENTER;
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        MenuShopWork->quantityAction = MenuSelectMove(MenuShopWork->quantityAction, 2, 0);
        if (PadData.mappedPressed == PAD_CIRCLE) {
            if (MenuShopWork->quantityAction == 0) {
                MenuShopWork->nextState = SHOP_STATE_AGWS_BUY_DONE;
                xglSoundEffectNormalID(SE_DECIDE, 0);
            } else {
                MenuShopWork->nextState = SHOP_STATE_AGWS_BUY_LIST;
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
        }
        if (PadData.mappedPressed == PAD_CROSS) {
            MenuShopWork->nextState = SHOP_STATE_AGWS_BUY_LIST;
            xglSoundEffectNormalID(SE_CANCEL, 0);
        }
        break;

    case SHOP_STATE_AGWS_BUY_DONE:
        if (MenuShopWork->stateFlags & SHOP_FLAG_STATE_ENTERED) {
            MenuShopUnitData *unit;
            int equipmentKey;

            unit = func_A191C0(MenuShopWork->unitId);
            if (MenuShopAgwsGet()->sort_mode_selector != 0) {
                unit->frameId = MenuShopAgwsGet()->equipmentId;
            } else {
                unit->engineId = MenuShopAgwsGet()->equipmentId;
            }
            MenuAgwsParaSet(MenuShopWork->unitId, 0);
            if (MenuShopAgwsGet()->sort_mode_selector != 0) {
                equipmentKey = (u16)MenuShopAgwsGet()->equipmentId + ITEM_KEY_FRAME;
            } else {
                equipmentKey = (u16)MenuShopAgwsGet()->equipmentId + ITEM_KEY_ENGINE;
            }
            dataMoneyBoxDec(MenuBoxMoneyGet(equipmentKey, 0));
            MenuShopListColorChange(0, 2);
            MenuShopWork->waitFrames = SHOP_WAIT_ENTER;
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        if (PadData.mappedPressed == PAD_CIRCLE) {
            MenuShopWork->nextState = SHOP_STATE_AGWS_BUY_LIST;
            xglSoundEffectNormalID(SE_DECIDE, 0);
        }
        break;

    case SHOP_STATE_EXIT:
        if (MenuShopWork->stateFlags & SHOP_FLAG_STATE_ENTERED) {
            MenuShopWork->waitFrames = SHOP_WAIT_EXIT;
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        MenuShopWork->nextState = SHOP_STATE_END;
        break;
    }
    MenuShopWork->stateFlags &= 0xFD;
    xglFontDebugHex(0, 0x48, MenuShopWork->state, 2);
    xglFontDebugHex(0, 0x50, MenuShopWork->nextState, 2);
    xglFontDebugPrintf(0, 0x60, menuShopDebugModeFormat, MenuShopWork->mode);
    xglFontDebugPrintf(0, 0x68, menuShopDebugCategoryFormat, MenuShopWork->category);
    xglFontDebugPrintf(0, 0x70, menuShopDebugListFormat, MenuShopWork->listIndex);
    xglFontDebugPrintf(0, 0x80, menuShopDebugUnitSelectionFormat, MenuShopWork->unitSelection);
}

void MenuShopMain(int shopType)
{
    int partyIds[16];
    unsigned char *workspace;
    unsigned char *modelMemory;
    int memberIndex;
    int unitCount;

    extern unsigned int GameLoopState[];
    extern XglTaskScheduler *MenuShopTask;
    extern unsigned char *ShopDataBufTop;
    extern const char menuShopDataPath[];
    extern void *MenuWorkEndGet(void);
    extern unsigned char *MenuBgTaskInit(unsigned char *work, int cameraId);
    extern void *xglTaskInitial(void *pool, int capacity, int flags);
    extern void *MenuModelInit(void *workStart);
    extern unsigned char *GameResourceWorkAlloc(int size);
    extern void MenuModelMemoryInit(void *base, int size);
    extern void MenuModelMemorySet(int count);
    extern void MenuModelMenuMotionLoad(void);
    extern void MenuWorkEndCheck(void *workEnd);
    extern void MenuLoadInit(void);
    extern void MenuLoadCancel(void);
    extern int MenuLoadSync(void);
    extern void MenuLoadFile(const char *name, void *buffer);
    extern void MenuBgTaskMain(void);
    extern void MenuBgTaskBreak(void);
    extern void MenuCfTaikiPush(void);
    extern void MenuCfTaikiPop(void);
    extern void MenuGameDataPush(void);
    extern void MenuGameDataPop(void);
    extern void endPrintInit(void);
    extern void endBackTexDraw(void *color);
    extern void MenuFontLoad(int font, int option);
    extern int WindowTexLoad(unsigned char *buffer, unsigned int request);
    extern int MenuEquipStealMaskCheck(void);
    extern void MenuModelMain(void);
    extern void MenuModelAllBreak(void);
    extern void nmlModelFlush(void);
    extern void GameResourceWorkReload(void);
    extern void GameCfPlayerLoadResource(int mode);
    extern void MenuShopCore(void);
    extern void endPrintExtFunc(int depth, int mode, void *vertices);
    extern void endPrintDirectFrameCopy(XglPacket *packet, int x, int y);
    extern void xglStudioChange(int mode);

    endPrintDirectFrameCopy(xglPacketGetCurrent(), sRender.width << 5,
        sRender.height << 5);
    GameLoopState[4] |= GAME_LOOP_FLAG_MENU_OPEN;
    xglSleep();
    endPrintInit();
    GameLoopState[4] |= GAME_LOOP_FLAG_MENU_OPEN;
    MenuCfTaikiPush();
    MenuGameDataPush();
    xglStudioChange(0);
    endPrintDirectFrameCopy(xglPacketGetCurrent(), sRender.height << 5,
        sRender.flip_base << 5);

    workspace = (unsigned char *)(((unsigned int)MenuWorkEndGet() + 127) & ~127U);
    ShopDataBufTop = workspace;
    {
        MenuShopWorkData *initialWork =
            MenuShopAlignBuffer(workspace + SHOP_DATA_BUFFER_SIZE, 2048);
        MenuShopWork = initialWork;
        workspace = (unsigned char *)initialWork + SHOP_WORK_SIZE;
        memset(initialWork, 0, SHOP_WORK_SIZE);
    }
    workspace = MenuBgTaskInit(workspace, 3);
    MenuShopTask = (XglTaskScheduler *)workspace;
    workspace = xglTaskInitial(workspace, SHOP_TASK_COUNT, 0);

    TskObjectSet3((TskObject *)xglTaskEntryNext(MenuShopTask,
        (int (*)(XglTaskPrefix *))tskTskMain3,
        MenuShopTask != 0 ? MenuShopTask->active_tail : 0),
        MenuShopPas, workspace);
    workspace += SHOP_PAS_SIZE;
    TskObjectSet3((TskObject *)xglTaskEntryNext(MenuShopTask,
        (int (*)(XglTaskPrefix *))tskTskMain3,
        MenuShopTask != 0 ? MenuShopTask->active_tail : 0),
        MenuShopInfo, workspace);
    workspace += SHOP_INFO_SIZE;
    TskObjectSet3((TskObject *)xglTaskEntryNext(MenuShopTask,
        (int (*)(XglTaskPrefix *))tskTskMain3,
        MenuShopTask != 0 ? MenuShopTask->active_tail : 0),
        MenuShopList, workspace);
    MenuShopWinSP = (WindowSPCursor *)(workspace + 12);
    workspace += SHOP_LIST_SIZE;
    TskObjectSet3((TskObject *)xglTaskEntryNext(MenuShopTask,
        (int (*)(XglTaskPrefix *))tskTskMain3,
        MenuShopTask != 0 ? MenuShopTask->active_tail : 0),
        MenuShopSelect, workspace);
    workspace += SHOP_SELECT_SIZE;
    TskObjectSet3((TskObject *)xglTaskEntryNext(MenuShopTask,
        (int (*)(XglTaskPrefix *))tskTskMain3,
        MenuShopTask != 0 ? MenuShopTask->active_tail : 0),
        MenuShopEx, workspace);
    workspace += SHOP_EX_SIZE;
    TskObjectSet3((TskObject *)xglTaskEntryNext(MenuShopTask,
        (int (*)(XglTaskPrefix *))tskTskMain3,
        MenuShopTask != 0 ? MenuShopTask->active_tail : 0),
        MenuShopEx2, workspace);
    workspace += SHOP_EX2_SIZE;
    TskObjectSet3((TskObject *)xglTaskEntryNext(MenuShopTask,
        (int (*)(XglTaskPrefix *))tskTskMain3,
        MenuShopTask != 0 ? MenuShopTask->active_tail : 0),
        MenuShopEx02, workspace);
    workspace += SHOP_EX02_SIZE;
    TskObjectSet3((TskObject *)xglTaskEntryNext(MenuShopTask,
        (int (*)(XglTaskPrefix *))tskTskMain3,
        MenuShopTask != 0 ? MenuShopTask->active_tail : 0),
        MenuShopSeisan, workspace);
    workspace += SHOP_SEISAN_SIZE;
    TskObjectSet3((TskObject *)xglTaskEntryNext(MenuShopTask,
        (int (*)(XglTaskPrefix *))tskTskMain3,
        MenuShopTask != 0 ? MenuShopTask->active_tail : 0),
        MenuShopSeisan02, workspace);
    workspace += SHOP_SEISAN02_SIZE;
    TskObjectSet3((TskObject *)xglTaskEntryNext(MenuShopTask,
        (int (*)(XglTaskPrefix *))tskTskMain3,
        MenuShopTask != 0 ? MenuShopTask->active_tail : 0),
        MenuShopIcon, workspace);
    workspace += SHOP_ICON_SIZE;
    TskObjectSet3((TskObject *)xglTaskEntryNext(MenuShopTask,
        (int (*)(XglTaskPrefix *))tskTskMain3,
        MenuShopTask != 0 ? MenuShopTask->active_tail : 0),
        MenuShopParameter, workspace);
    workspace += SHOP_PARAMETER_SIZE;
    TskObjectSet3((TskObject *)xglTaskEntryNext(MenuShopTask,
        (int (*)(XglTaskPrefix *))tskTskMain3,
        MenuShopTask != 0 ? MenuShopTask->active_tail : 0),
        MenuShopLine, workspace);
    workspace += SHOP_LINE_SIZE;
    TskObjectSet3((TskObject *)xglTaskEntryNext(MenuShopTask,
        (int (*)(XglTaskPrefix *))tskTskMain3,
        MenuShopTask != 0 ? MenuShopTask->active_tail : 0),
        MenuShopModelMain, 0);
    workspace = MenuModelInit(workspace);

    modelMemory = GameResourceWorkAlloc(SHOP_MODEL_MEMORY_SIZE);
    xglStudioChange(0);
    MenuModelMemoryInit(modelMemory, SHOP_MODEL_MEMORY_SIZE);
    MenuModelMemorySet(20);
    MenuModelMenuMotionLoad();
    MenuWorkEndCheck(workspace);
    MenuLoadInit();
    endBackTexDraw((void *)-1);
    xglSleep();
    MenuFontLoad(1, 1);
    WindowTexLoad(0, 0x10001);
    WindowTexLoad(0, 0x10002);
    MenuLoadFile(menuShopDataPath, ShopDataBufTop);
    xglSoundEffectNormalID(SE_DECIDE, 0);

    while (MenuLoadSync() != 0) {
        MenuBgTaskMain();
        xglSleep();
    }

    ShopData = ((const MenuShopData *)ShopDataBufTop)[shopType - 1];
    if (shopType == SHOP_TYPE_SELL_ONLY)
        MenuShopWork->noCategories = 1;
    if (shopType == 6 && MenuEquipStealMaskCheck() == 1)
        MenuShopWork->noCategories = 1;
    MenuShopAgwsListChange();
    MenuShopWork->nextState = SHOP_STATE_MODE_SELECT;
    MenuKeepSelectReset();
    MenuShopWork->partyCount = PartyAllPartyGet(partyIds, 1);
    {
        MenuShopWorkData *partyWork = MenuShopWork;
        memberIndex = 0;
        if (partyWork->partyCount > 0) {
            int *partySource = partyIds;
            do {
                MenuShopAppendPartyId(partyWork, &memberIndex, &partySource);
            } while (memberIndex < partyWork->partyCount);
        }
    }
    MenuShopWork->unitCount = PartyAgwsGet(partyIds);
    memberIndex = 0;
    unitCount = MenuShopWork->unitCount;
    if (unitCount > 0) {
        MenuShopWorkData *unitWork = MenuShopWork;
        int *unitSource = partyIds;
        do {
            MenuShopAppendUnitId(unitWork, &memberIndex, &unitSource);
        } while (memberIndex < unitWork->unitCount);
        unitCount = MenuShopWork->unitCount;
    }

    if (unitCount != 0 && ShopData.records[SHOP_RECORD_FRAME].count != 0)
        MenuShopWork->equipmentStatus = 1;
    else
        MenuShopWork->equipmentStatus = 0;
    if (ShopData.tail.count != 0)
        MenuShopWork->modelBreakPending = 1;
    else
        MenuShopWork->modelBreakPending = 0;
    MenuScenarioNo = MenuScenarioNoGet();

    while ((GameSnapShotCheck(), MenuShopWork->state != SHOP_STATE_END) &&
        (MenuShopCore(), 1)) {
        endPrintExtFunc(0, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_FLUSH, 0);
        MenuBgTaskMain();
        nmlModelFlush();
        xglTaskExecute(MenuShopTask);
        MenuModelMain();
        xglSleep();
    }

    MenuLoadCancel();
    MenuBgTaskBreak();
    MenuShopWork->state = SHOP_STATE_END;
    xglTaskExecute(MenuShopTask);
    if (MenuShopWork->modelBreakPending != 0)
        MenuModelAllBreak();
    MenuCfTaikiPop();
    MenuFontLoad(0, 0);
    WindowTexLoad(0, 0);
    GameResourceWorkReload();
    GameCfPlayerLoadResource(1);
    MenuGameDataPop();
    GameLoopState[4] &= ~GAME_LOOP_FLAG_MENU_OPEN;
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
/* MenuShopModelMain copies all four coordinates to the camera at +0xD0. */
const DoublewordAlignedVector4 menuShopModelCameraVector = { -2.0f, 1.8f, 8.5f, 1.0f };

/* MenuShopInfo copies this pair and chooses the AG or VX maker prompt. */
const char *menuShopMakerNames[2] = { menuShopHyamsMakerText, menuShopVectorMakerText };

const char menuShopBuyGeneratorText[16] = "Buy Generator ";

const char menuShopBuyFrameText[16] = "Buy Frame";

const char menuShopFhpRestorationText[16] = "FHP Restoration";

const char menuShopAgwsTuningText[16] = "A.G.W.S. Tuning";

const char menuShopAgwsText[16] = "A.G.W.S.";

const char menuShopAgwsAmmoText[16] = "A.G.W.S. Ammo";

const char menuShopAgwsAccessoriesText[24] = "A.G.W.S. Accessories";

const char menuShopAgwsWeaponsText[24] = "A.G.W.S. Weapons";

const char menuShopCharacterAmmoText[16] = "Character Ammo";

const char menuShopCharacterAccessoriesText[24] = "Character Accessories";

const char menuShopCharacterWeaponsText[24] = "Character Weapons";

const char menuShopBuyItemsWeaponsText[24] = "Buy items and weapons.";

const char menuShopSellItemsWeaponsText[24] = "Sell items and weapons.";

const char menuShopTuneAgwsText[16] = "Tune A.G.W.S.";

const char * const menuShopMainPromptTexts[4] = { menuShopBuyItemsWeaponsText, menuShopSellItemsWeaponsText, menuShopTuneAgwsText, MenuShopQuitText };

const char menuShopBuyItemsText[16] = "Buy items.";

const char menuShopSellItemsText[16] = "Sell items.";

/* Text: quantity.\nPress \x1E\x01 to accept. */
const char menuShopQuantityPromptText[32] = "quantity.\nPress \036\001 to accept.";

const char * const menuShopQuantityPromptTexts[4] = { menuShopBuyItemsText, menuShopSellItemsText, MenuShopSelectText, menuShopQuantityPromptText };

const char menuShopUnitOfText[16] = " unit(s) of ";

const char menuShopBuyFrameDescriptionText[24] = "Buy an A.G.W.S. Frame.";

const char menuShopBuyGeneratorDescriptionText[32] = "Buy an A.G.W.S. Generator.";

const char menuShopRestoreFhpDescriptionText[24] = "Restore A.G.W.S. FHP.";

const char * const menuShopFramePromptTexts[4] = { menuShopBuyFrameDescriptionText, menuShopBuyGeneratorDescriptionText, menuShopRestoreFhpDescriptionText, MenuShopQuitText };

/* Text:  purchased.\x1F */
const char menuShopPurchasedText[16] = " purchased.\037";

const char menuShopFhpFullyRestoredText[32] = " FHP will be fully restored.";

const char menuShopEquippedWithText[16] = " equipped with ";

const char menuShopExchangeWithText[16] = "Exchange with ";

const char menuShopPossessiveFrameText[16] = "'s frame ";

const char menuShopPossessiveGeneratorText[16] = "'s generator ";

const char menuShopExchangedWithText[32] = "has been exchanged with ";

/* Text:  FHP has been fully restored. \x1F */
const char menuShopFhpHasBeenRestoredText[32] = " FHP has been fully restored. \037";

const char menuShopRestoreFhpText[16] = "Restore FHP ";

const char menuShopBuyGeneratorCommandText[16] = "Buy Generator";

const char menuShopRestoreItPromptText[16] = " Restore it\?";

const char menuShopSellItPromptText[16] = " Sell it\?";

const char menuShopBuyItPromptText[24] = " Buy it\?";

const struct MenuShopSortOptionTable menuShopSortOptionTable = { .entries = { { .type = 1, .unmodeled_01 = { 0, 0, 0 }, .order = 0 }, { .type = 4, .unmodeled_01 = { 0, 0, 0 }, .order = -1 }, { .type = 16, .unmodeled_01 = { 0, 0, 0 }, .order = -1 }, { .type = 8, .unmodeled_01 = { 0, 0, 0 }, .order = -1 }, { .type = 4, .unmodeled_01 = { 0, 0, 0 }, .order = -2 }, { .type = 16, .unmodeled_01 = { 0, 0, 0 }, .order = -2 }, { .type = 8, .unmodeled_01 = { 0, 0, 0 }, .order = -2 }, { .type = 2, .unmodeled_01 = { 0, 0, 0 }, .order = -1 }, { .type = 2, .unmodeled_01 = { 0, 0, 0 }, .order = -1 } } };

const struct MenuShopRowCountTable menuShopRowCountTable = { .values = { 11, 9, 9, 9, 6, 9, 9, 4, 3, 11, 7, 9, 9, 6, 9, 9, 4, 3 } };

/* Text: List{x 653}{x 524}Stock */
const char menuShopStockColumnText[16] = "List\003\215\003\014Stock";

/* Text: List{x 653}{x 536}Price */
const char menuShopPriceColumnText[16] = "List\003\215\003\030Price";

const char menuShopDebugModeFormat[16] = "tra  %3d";

const char menuShopDebugCategoryFormat[16] = "cate %3d";

const char menuShopDebugListFormat[16] = "list %3d";

const char menuShopDebugUnitSelectionFormat[16] = "ag_sl %3d";

const char menuShopDataPath[40] = "data\\endou\\shop\\shopdata.bin";

const float menuShopIconScaleGrowStep = 0.20000000298023224f;

const float menuShopIconScaleShrinkStep = 0.20000000298023224f;

const char menuShopBlankText[8] = "";

const char menuShopPeriodText[8] = ".";

const char menuShopQuitText[8] = "Quit";

const char menuShopYesNoText[8] = "Yes\nNo";

const char menuShopMenuTagText[8] = "Menu";

const char menuShopSelectTagText[8] = "Select";

/* Text: \x01Price */
const char menuShopPriceText[8] = "\001Price";

/* Text: \x01Equip */
const char menuShopEquipText[8] = "\001Equip";

/* Text: \x01Target */
const char menuShopTargetText[8] = "\001Target";

/* Text: \x01Money */
const char menuShopMoneyText[8] = "\001Money";

/* Text: \x01Stock */
const char menuShopStockText[8] = "\001Stock";

const char menuShopPayText[8] = "Pay";

/* Text: \x01Result */
const char menuShopResultText[8] = "\001Result";

/* Text: \x01Total */
const char menuShopTotalText[8] = "\001Total";

const char menuShopTargetTagText[8] = "Target";

/* Text: \x01AGL */
const char menuShopAglText[8] = "\001AGL";

/* Text: \x01EVA */
const char menuShopEvaText[8] = "\001EVA";

/* Text: \x01DEX */
const char menuShopDexText[8] = "\001DEX";

/* Text: \x01EDEF */
const char menuShopEdefText[8] = "\001EDEF";

/* Text: \x01EATK */
const char menuShopEatkText[8] = "\001EATK";

/* Text: \x01VIT */
const char menuShopVitText[8] = "\001VIT";

/* Text: \x01STR */
const char menuShopStrText[8] = "\001STR";

/* Text: \x01WAGL */
const char menuShopWaglText[8] = "\001WAGL";

/* Text: \x01EARM */
const char menuShopEarmText[8] = "\001EARM";

/* Text: \x01PARM */
const char menuShopParmText[8] = "\001PARM";

/* Text: \x01DPOW */
const char menuShopDpowText[8] = "\001DPOW";

/* Text: \x01FHP */
const char menuShopFhpText[8] = "\001FHP";

const struct MenuShopParameterDigits8 menuShopCharacterDigitsTable[1] = { { .digits = { 3, 3, 3, 3, 2, 2, 2, 3 } } };

const char * const menuShopUnitCountPromptTexts[3] = { MenuShopIsText, MenuShopGoldUnitText, menuShopUnitOfText };

const char * const menuShopPurchasedPromptTexts[3] = { menuShopUnitOfText, menuShopPurchasedText, MenuShopSoldText };

const char * const menuShopExchangePromptTexts[8] = { menuShopFhpFullyRestoredText, menuShopEquippedWithText, MenuShopPeriodText, menuShopExchangeWithText, menuShopPossessiveFrameText, menuShopPossessiveGeneratorText, menuShopExchangedWithText, menuShopFhpHasBeenRestoredText };

const MenuShopSortTable menuShopRecordSortTable = { { { (const short *)&ShopData.records[0], 1, 20 }, { (const short *)&ShopData.records[1], 3, 20 }, { (const short *)&ShopData.records[2], 5, 20 }, { (const short *)&ShopData.records[3], 4, 20 }, { (const short *)&ShopData.records[4], 3, 20 }, { (const short *)&ShopData.records[5], 5, 20 }, { (const short *)&ShopData.records[6], 4, 20 }, { (const short *)&ShopData.tail, 7, 4 }, { 0, 0, 0 }, { (const short *)&ShopData.records[7], 11, 20 }, { (const short *)&ShopData.records[8], 12, 20 } } };

const struct MenuShopIconIds menuShopIconSpriteTable = { { 0x0109, 0x010a, 0x0322, 0x0324, 0x0325, 0x0328, 0x0326, 0x0327, 0x0329, 0x032a, 0x0323 } };

/* Text: . \x1F */
char menuShopPeriodEndText[8] = ". \037";

struct MenuShopParameterDigits7 menuShopAgwsDigitsTable[1] = { { { 4, 3, 3, 3, 2, 2, 3 } } };

const char menuShopHyamsMakerText[40] = "Hyams Heavy Industries - AG Series";

const char menuShopVectorMakerText[32] = "Vector Industries - VX Series";
