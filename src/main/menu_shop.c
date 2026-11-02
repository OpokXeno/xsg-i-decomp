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
    char message[128];
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
    short ids[11];
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
    signed char categoryLift[9];
    unsigned char unmodeled_11[3];
    int depth;
    float scale;
    struct MenuShopIconSprite sprites[11];
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

    if (MenuShopWork->state == 0xff) {
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
            unit->extFuncFlag = 0x1e;
            MenuShopWork->modelPending = 0;
        }

        state = unit->extFuncFlag;
        switch (state) {
        case 10:
            break;
        case 0:
            transform->rotation.y = -0.2f;
            MenuModelUnitOpen(unit, 2);
            unit->extFuncFlag = 10;
            return;
        case 0x1e:
            MenuModelUnitOpen(unit, 1);
            unit->extFuncFlag = 0x1f;
            /* fall through */
        case 0x1f:
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
        case 0x30:
        case 0xA0:
        case 0xA2:
        case 0xA4:
            break;
        default:
            goto mark_pending;
        }
        category = MenuShopWork->category;
        if (category != 7)
            goto mark_pending;
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
mark_pending:
        MenuShopWork->modelPending = 1;
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
        work->x = -272;
        work->y = 8;
        work->depth = 0xfffff0;
        window = &work->window;
        WindowDXSet(window);
        window->x = work->x;
        work->window.style = work->depth;
        work->window.width = 272;
        work->window.y = work->y;
        work->window.height = 32;
        work->window.state = 1;
        WindowDXMain(window);
        work->window.state = 3;
        text = msg00_0_0036DAF0;
        for (i = 0; i < 16; i++, text++) {
            eMessageSet(&work->messages[i], *text);
            work->messages[i].mode = 32;
            work->messages[i].x = 288;
            work->messages[i].draw_parameter = work->depth + 2;
            work->messages[i].y = work->y + 3;
        }
        break;
    case 2:
        windowX = -16;
        for (i = 0; i < 16; i++)
            messageX[i] = 288;
        switch (MenuShopWork->state) {
        case 0x20:
            messageX[0] = 16;
            break;
        case 0x30:
        case 0x90:
        case 0xa0:
        case 0xa2:
        case 0xa4:
            messageX[MenuShopWork->category + 3] = 16;
            break;
        case 0xc0:
            messageX[12] = 16;
            break;
        case 0xc2:
        case 0xc4:
        case 0xc6:
            messageX[13] = 16;
            break;
        case 0xd0:
        case 0xd2:
        case 0xd4:
        case 0xd6:
            if (MenuShopWork->sort_mode_selector != 0)
                messageX[14] = 16;
            else
                messageX[15] = 16;
            break;
        default:
            windowX = -272;
            break;
        }
        window = &work->window;
        MoveSlide(&window->x, &windowX, 3.0f);
        WindowDXMain(window);
        work->bounds.x1 = window->x + 3;
        work->bounds.y1 = work->window.y + 3;
        work->bounds.x2 = work->window.width - 6;
        work->bounds.y2 = work->window.height - 6;
        work->bounds.color = work->depth;
        endPrintExtFunc(work->depth, 101, &work->bounds);
        target = messageX;
        for (i = 0; i < 16; i++, target++) {
            MoveSlide(&work->messages[i].x, target, 3.0f);
            if (work->messages[i].x < 256)
                eMessageMain(&work->messages[i]);
        }
        endPrintExtFunc(work->depth, 102, 0);
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
        work->ticker = 0x00fffff0;
        WindowDXSet(&work->window);
        work->window.x = -16;
        work->window.y = 480;
        work->window.width = 544;
        work->window.height = 54;
        work->window.style = work->ticker;
        work->window.close_callback = MenuInfoWindow;
        work->window.state = 1;
        panel = &work->panel;
        work->window.close_callback_arg = panel;
        WindowDXMain(&work->window);
        work->window.state = 3;
        panel->textOffset[0] = 0;
        work->panel.textOffset[1] = 0;
        work->panel.text = 0;
        work->flag = 0;
        work->unmodeled_04 = 0;
        memset(work->message, 0, 128);
        return;
    case 2:
        break;
    default:
        return;
    }

    targetY = 386;
    targetH = 54;
    switch (MenuShopWork->state) {
    case 32:
        eMessageCpy(work->message, tab0.p[MenuShopWork->mode]);
        break;
    case 48:
    case 144:
        if (MenuShopWork->itemId == 0) {
            eMessageCpy(work->message, menuShopBlankText);
        } else if ((MenuShopWork->itemId >> 16) == 7) {
            if (MenuShopWork->modelId >= 23)
                eMessageCpy(work->message, tab112.p[0]);
            else
                eMessageCpy(work->message, tab112.p[1]);
        } else {
            eMessageCpy(work->message, MenuTextGet(MenuShopWork->itemId)->auxiliary);
        }
        break;
    case 160:
        if (MenuShopWork->itemId != 0) {
            eMessageCpy(work->message, tab16.p[2]);
            eMessageCat(tab16.p[3]);
        }
        break;
    case 162:
        if (MenuShopWork->itemId != 0) {
            long money;

            eMessageCpy(work->message, MenuNumberTextGet(MenuShopWork->quantity, 2, 0));
            eMessageCat(tab32.p[2]);
            eMessageCat(MenuTextGet(MenuShopWork->itemId)->name);
            eMessageCat(tab32.p[0]);
            switch (MenuShopWork->mode) {
            case 0:
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
    case 164:
        if (MenuShopWork->itemId != 0) {
            eMessageCpy(work->message, MenuNumberTextGet(MenuShopWork->quantity, 2, 0));
            eMessageCat(tab64.p[0]);
            eMessageCat(MenuTextGet(MenuShopWork->itemId)->name);
            eMessageCat(menuShopBlankText);
            eMessageCat(tab64.p[MenuShopWork->mode + 1]);
        }
        break;
    case 192:
        eMessageCpy(work->message, tab48.p[MenuShopWork->sortOption]);
        break;
    case 194:
    case 196:
        eMessageCpy(work->message, MenuCharNameGet(MenuShopWork->unitId));
        eMessageCat(tab80.p[0]);
        break;
    case 198:
        eMessageCpy(work->message, MenuCharNameGet(MenuShopWork->unitId));
        eMessageCat(tab80.p[7]);
        break;
    case 208:
        if (MenuShopWork->listIndex < 0 || MenuShopWork->equipmentId == 0) {
            eMessageCpy(work->message, menuShopBlankText);
        } else {
            eMessageCpy(work->message, func_A2C968(MenuShopWork->equipmentId)->auxiliary);
        }
        break;
    case 210:
    case 212:
        if (MenuShopWork->listIndex >= 0) {
            unit = func_A191C0(MenuShopWork->unitId);
            eMessageCpy(work->message, MenuCharNameGet(MenuShopWork->unitId));
            eMessageCat(tab80.p[1]);
            if (MenuShopWork->sort_mode_selector != 0) {
                value = *(short *)(unit + 88);
                if (value != 0)
                    eMessageCat(func_A2C968(value)->name);
                eMessageCat(tab80.p[2]);
                eMessageCat(tab80.p[3]);
                if (MenuShopWork->equipmentId != 0)
                    eMessageCat(func_A2C968(MenuShopWork->equipmentId)->name);
            } else {
                value = *(short *)(unit + 86);
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
    case 214:
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
        targetY = 480;
        break;
    }
    MoveSlide(&work->window.y, &targetY, 3.0f);
    MoveSlide(&work->window.height, &targetH, 3.0f);
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

            for (commandIndex = 0; commandIndex < 4; commandIndex++) {
                work->menus[menuIndex].commands[commandIndex].name =
                    msg00_1_0036DB30[menuIndex * 4 + commandIndex];
                work->menus[menuIndex].commands[commandIndex].locked = 0;
            }
            work->menus[menuIndex].commands[commandIndex].name = 0;
        }
        if (menuIndex == 0) {
            if (MenuShopWork->noCategories != 0)
                work->menus[menuIndex].commands[0].locked = 1;
            if (MenuShopWork->equipmentStatus == 0)
                work->menus[menuIndex].commands[2].locked = 1;
        }
        return work->menus[menuIndex].commands;
    }

    int i;

    switch (task->state) {
    case 0:
        work->depth = 0xfffff4;
        for (i = 0; i < 2; i++) {
            WindowDXSet(&work->menus[i].window);
            work->menus[i].window.x = 528;
            work->menus[i].window.style = work->depth;
            if (i == 0)
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
            work->menus[i].window.state = 1;
            WindowDXMain(&work->menus[i].window);
            work->menus[i].window.state = 3;
        }
        for (i = 0; i < 1; i++) {
            WindowDXSet(&work->choices[i].window);
            work->choices[i].window.x = 528;
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
            work->choices[i].window.state = 1;
            WindowDXMain(&work->choices[i].window);
            work->choices[i].window.state = 3;
        }
        work->popupFlags = 0;
        break;
    case 2: {
        short menuX[2];
        short choiceX[2];
        MenuShopWorkData *shop;

        for (i = 0; i < 2; i++) {
            unsigned char popupVisible;

            menuX[i] = 528;
            popupVisible = work->popupFlags & 1;
            if (popupVisible)
                choiceX[i] = -176;
            else
                choiceX[i] = 528;
        }
        shop = MenuShopWork;
        switch (shop->state) {
        case 0x20:
            if (shop->stateFlags & 1) {
                work->menus[0].window.x = 528;
                work->menus[0].window.y = 192;
            }
            menuX[0] = 152;
            work->menus[0].panel.cursor = shop->mode;
            break;
        case 0xc0:
            if (shop->stateFlags & 1) {
                work->menus[1].window.x = 528;
                work->menus[1].window.y = 192;
            }
            menuX[1] = 160;
            work->menus[1].panel.cursor = shop->sortOption;
            break;
        case 0xa2: {
            unsigned char *mode = &shop->mode;
            unsigned char updatePopup;

            updatePopup = shop->stateFlags & 1;

            if (updatePopup) {
                switch (*mode) {
                case 0:
                    work->choices[0].panel.heading = msg01_2_0036DB50[0];
                    switch (mode[1]) {
                    case 0:
                        work->choices[0].window.x = 528;
                        work->choices[0].window.y = 256;
                        work->popupFlags &= ~1;
                        break;
                    case 7:
                        work->choices[0].window.x = -136;
                        work->choices[0].window.y = 208;
                        work->popupFlags |= 1;
                        break;
                    case 8:
                        work->choices[0].window.x = 528;
                        work->choices[0].window.y = 256;
                        work->popupFlags &= ~1;
                        break;
                    default:
                        work->choices[0].window.x = -136;
                        work->choices[0].window.y = 80;
                        work->popupFlags |= 1;
                        break;
                    }
                    break;
                default:
                    work->choices[0].panel.heading = msg01_2_0036DB50[1];
                    switch (mode[1]) {
                    case 0:
                    case 8:
                        work->choices[0].window.x = 528;
                        work->choices[0].window.y = 256;
                        work->popupFlags &= ~1;
                        break;
                    case 1:
                        work->choices[0].window.x = -136;
                        work->choices[0].window.y = 272;
                        work->popupFlags |= 1;
                        break;
                    case 4:
                        work->choices[0].window.x = -136;
                        work->choices[0].window.y = 272;
                        work->popupFlags |= 1;
                        break;
                    case 7:
                        work->choices[0].window.x = -136;
                        work->choices[0].window.y = 192;
                        work->popupFlags |= 1;
                        break;
                    default:
                        work->choices[0].window.x = -136;
                        work->choices[0].window.y = 80;
                        work->popupFlags |= 1;
                        break;
                    }
                    break;
                }
            }
            switch (MenuShopWork->category) {
            case 0:
            case 8:
                choiceX[0] = 304;
                break;
            case 7:
                choiceX[0] = 48;
                break;
            default:
                choiceX[0] = 32;
                break;
            }
            work->choices[0].panel.cursor = MenuShopWork->quantityAction;
            break;
        }
        case 0xc4:
            if (shop->stateFlags & 1) {
                work->choices[0].panel.heading = msg01_2_0036DB50[2];
                work->choices[0].window.x = -136;
                work->choices[0].window.y = 192;
                work->popupFlags |= 1;
            }
            choiceX[0] = 88;
            work->choices[0].panel.cursor = MenuShopWork->quantityAction;
            break;
        case 0xd4:
            if (shop->stateFlags & 1) {
                work->choices[0].panel.heading = msg01_2_0036DB50[0];
                work->choices[0].window.x = 528;
                work->choices[0].window.y = 176;
                work->popupFlags &= ~1;
            }
            choiceX[0] = 304;
            work->choices[0].panel.cursor = MenuShopWork->quantityAction;
            break;
        }
        for (i = 0; i < 2; i++) {
            MoveSlide(&work->menus[i].window.x, &menuX[i], 3.0f);
            WindowDXMain(&work->menus[i].window);
        }
        MoveSlide(&work->choices[0].window.x, &choiceX[0], 3.0f);
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

    if (type == 0x12)
        type = 9;
    if (type == 0x13)
        type = 10;

    if (type == 8) {
        if (MenuScenarioNo >= 0x73) {
            *output++ = 0x20035;
            *output++ = 0x20036;
        }
        if (MenuScenarioNo >= 0x12D)
            *output++ = 0x20037;
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
    case 1:
    case 2:
    case 3:
        partyCount = PartyAllPartyGet(partyIds, 0);
        break;
    case 4:
    case 5:
    case 6:
        partyCount = PartyAgwsGet(partyIds);
        break;
    }
    if (partyCount == 0)
        return 0;
    switch (MenuShopWork->category) {
    case 1:
        for (party = 0; party < partyCount; party++) {
            unit = func_A191C0(partyIds[party]);
            if (unit == NULL)
                return 0;
            if (unit->itemIds[0][selectedSlot[partyIds[party] - 1]] == selectedItem)
                matches++;
        }
        break;
    case 3:
        for (party = 0; party < partyCount; party++) {
            unit = func_A191C0(partyIds[party]);
            if (unit == NULL)
                return 0;
            if (unit->itemIds[2][selectedSlot[partyIds[party] - 1]] == selectedItem)
                matches++;
        }
        break;
    case 2:
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
    case 4:
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
    case 6:
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
    case 5:
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
    case 1:
        flags = func_A1A4E8(itemId)->flags;
        break;
    case 3:
        flags = dataWpnGet(itemId)->flags;
        break;
    case 4:
        flags = dataAttGet(itemId)->flags;
        break;
    case 5:
        flags = func_A1A548(itemId)->flags;
        break;
    }

    if ((flags & 0x1000) != 0)
        return 1;
    return 0;
}

void MenuShopListColorChange(int list, int kind)
{
    int *sorted = MenuSortAddrGet(list);
    int count = MenuSortCheck(list);
    MenuListEntry *entry = MenuListGet(list);
    int i;

    if (MenuShopWork->mode == 0 || kind == 2) {
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

    if (MenuShopWork->mode == 0) {
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
    width = 272 - shop->mode * 32;
    window = MenuShopWinSPRead();
    window->width = width;
    window->height =
        rowCounts.values[shop->category + shop->mode * 9] * 24 + 6;
    window->rows =
        rowCounts.values[shop->category + shop->mode * 9];
    MenuShopWinSP->columns = columns;
    MenuShopWinSP->title = msg00_4_0036DB60[MenuShopWork->mode];
    MenuShopWinSP->draw_flags = drawFlags;
    MenuShopWinSP->items = MenuListGet(0);
    WindowSPItemChange(MenuShopWinSP);
    WindowSPSetSelect(MenuShopWinSP,
        &MenuKeepSelect[(MenuShopWork->category + MenuShopWork->mode * 9) * 5]);
    if (MenuShopWork->mode == 0)
        MenuShopWinSP->mode = 6;
    return WindowSPSelect(MenuShopWinSP, 0);
}

int MenuShopListChange01(int list)
{
    int type = 18;
    WindowSPCursor *window;
    short window_width;

    if (MenuShopWork->sort_mode_selector == 0)
        type = 19;

    MenuShopSortSet(0, type, 0, 0);
    MenuListMake(0, -10);
    MenuShopListColorChange(0, 2);

    window = MenuShopWinSP;
    MenuShopWinSP->rows = 9;
    window_width = 272;
    window->height = 222;
    window->width = window_width;
    MenuShopWinSP->columns = 1;
    MenuShopWinSP->title = menuShopPriceListTitleText;
    MenuShopWinSP->draw_flags = 7;
    MenuShopWinSP->items = MenuListGet(0);
    WindowSPItemChange(MenuShopWinSP);
    WindowSPSetSelect(MenuShopWinSP, &MenuKeepSelect[type * 5]);

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
        work->window.x = -0x110;
        work->window.y = 0x50;
        work->color = 0xffff0;
        work->window.style = 0xffff0;
        work->window.draw_flags = 7;
        work->window.title = menuShopBlankText;
        work->window.rows = 0xb;
        work->window.height = 0x10e;
        work->window.state = 0;
        work->window.items = 0;
        WindowSPSet(&work->window);
        work->initialFlags[0] = 0;
        work->initialFlags[1] = 0;
        work->state = 0;
        work->active = 0;
        return;
    case 2:
        switch (work->state) {
        case 0:
            work->active = 0;
            work->state = 1;
            /* fall through */
        case 1:
            switch (MenuShopWork->state) {
            case 0x30:
            case 0x90:
                work->state = 0xa;
                break;
            case 0xd0:
                work->state = 0x1e;
                break;
            }
            break;
        case 10:
            work->window.x = -0x120;
            work->window.y = 0x50;
            MenuShopListChange00(0);
            work->window.state = 0x11;
            work->state = 0xb;
            work->active = 1;
            selection = MenuShopWork;
            work->category = selection->category;
            goto update_open_window;
        case 11:
            selection = MenuShopWork;
        update_open_window:
        {
            short targetX = 0x10;
            if (selection->state == 0xa2) {
                unsigned char category = selection->category;

                if (selection->mode == 0) {
                    if (category != 0 && category != 8 && category != 7)
                        targetX = -0x120;
                } else if (category != 0 && category != 8 && category != 1 && category != 4) {
                    targetX = -0x120;
                }
            }
            MoveSlide(&work->window.x, &targetX, 3.0f);
            selection = MenuShopWork;
            if (selection->state == 0x20) {
                work->state = 0xc;
                selection = MenuShopWork;
            }
            if (work->category != selection->category)
                work->state = 0xa;
            break;
        }
        case 12:
            closeX = -((unsigned short)work->window.width + 0x10);
            MoveSlide(&work->window.x, &closeX, 3.0f);
            if (work->window.x == closeX)
                work->state = 0;
            break;
        case 30:
            work->window.x = -0x120;
            work->window.y = 0x50;
            MenuShopListChange01(0);
            work->window.state = 0x11;
            work->state = 0x1f;
            work->active = 1;
            /* fall through */
        case 31:
            secondTargetX = 0x10;
            MoveSlide(&work->window.x, &secondTargetX, 3.0f);
            if (MenuShopWork->state == 0xc0)
                work->state = 0xc;
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
        work->depth = 0x00fffff0;
        for (i = 0; i < 2; i++) {
            eRibbonSet(&work->headers[i].ribbon, 3);
            work->headers[i].ribbon.x = 0x210;
            work->headers[i].ribbon.y = 0;
            work->headers[i].ribbon.width = 0x100;
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
            work->parties[i].ribbon.width = 0x100;
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

            left = 0x210;
            {
                const char *head = *(const char **)(void *)&work->equipment.ribbon;

                *(const char **)(void *)&work->equipment.ribbon = head;
            }
            style = ((union { int value; short half[2]; } *)(void *)&work->depth)->value;
            labels = msg00_5_0036DB68;
            top = 0x93;
            height = 0x18;
            width = 0x100;
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
            ribbonX[i] = 0x210;
            ribbonY[i] = 0x53 + 0x20 * i;
        }
        rosterY[0] = 0x93;
        rosterY[1] = 0x43;
        rosterX[0] = 0x210;
        rosterX[1] = 0x210;
        totalX[0] = 0x210;
        totalY[0] = 0xcb;

        switch ((*shop)->state) {
        case 0x30:
        case 0x90:
        case 0xa4:
            if ((unsigned char)((*shop)->stateFlags & 1)) {
                if ((*shop)->mode == 0)
                    eTagFontSet(&work->headers[0].label, msg00_5_0036DB68[0]);
                else
                    eTagFontSet(&work->headers[0].label, msg00_5_0036DB68[4]);
                work->slideFlags |= 2;
            }
            for (i = 0; i < 2; i++) {
                ribbonX[i] = 0x130;
                ribbonY[i] = 0x53 + 0x20 * i;
                if (work->slideFlags & 2) {
                    work->headers[i].ribbon.y = 0x53 + 0x20 * i;
                    work->slideFlags &= ~2;
                }
            }
            switch ((*shop)->category - 1) {
            case 0:
            case 1:
            case 2:
                work->equipment.ribbon.y = totalY[0] = 0xcb;
                totalX[0] = 0x130;
                eTagFontSet(&work->parties[0].label, msg00_5_0036DB68[2]);
                rosterX[0] = 0x130;
                rosterY[0] = 0x93;
                work->parties[0].ribbon.y = 0x93;
                work->slideFlags |= 2;
                break;
            case 3:
            case 4:
            case 5:
                work->equipment.ribbon.y = totalY[0] = 0xfb;
                totalX[0] = 0x130;
                eTagFontSet(&work->parties[1].label, msg00_5_0036DB68[2]);
                rosterX[1] = 0x130;
                rosterY[1] = 0x93;
                work->parties[1].ribbon.y = 0x93;
                work->slideFlags |= 2;
                break;
            case 6:
                ribbonX[0] = 0x210;
                ribbonX[1] = 0x130;
                ribbonY[1] = 0x118;
                break;
            }
            break;
        case 0xa0:
        case 0xa2:
            switch ((*shop)->category - 1) {
            case 0:
            case 1:
            case 2:
                rosterX[0] = 0x130;
                rosterY[0] = 0xf0;
                break;
            case 3:
            case 4:
            case 5:
                rosterX[1] = 0x130;
                rosterY[1] = 0xe8;
                break;
            case 6:
                ribbonX[1] = 0x130;
                ribbonY[1] = 0x118;
                break;
            }
            break;
        }

        for (i = 0; i < 2; i++) {
            MoveSlide(&work->headers[i].ribbon.x, &ribbonX[i], 3.0f);
            if (work->headers[i].ribbon.x == ribbonX[i])
                MoveSlide(&work->headers[i].ribbon.y, &ribbonY[i], 3.0f);
            eRibbonMain(&work->headers[i].ribbon);
            work->headers[i].number.x = work->headers[i].ribbon.x + 0x40;
            work->headers[i].number.y = work->headers[i].ribbon.y + 4;
            work->headers[i].number.format = 0;
            if (i == 0) {
                if ((*shop)->mode == 0) {
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
            MoveSlide(&work->parties[i].ribbon.x, &rosterX[i], 3.0f);
            if (work->parties[i].ribbon.x == rosterX[i])
                MoveSlide(&work->parties[i].ribbon.y, &rosterY[i], 3.0f);
            eRibbonMain(&work->parties[i].ribbon);
            if (i == 0) {
                for (j = 0; j < (*shop)->partyCount; j++) {
                    int usable;

                    usable = 0;
                    if ((*shop)->modelId != 0) {
                        switch (((short *)&(*shop)->itemId)[1]) {
                        case 3:
                            if (MenuWeaponEquipCheck(*(short *)((char *)(*shop)->partyIds + (j << 1)), (*shop)->modelId, 0))
                                usable = 1;
                            break;
                        case 5:
                            if (MenuAccessoryEquipCheck(*(short *)((char *)(*shop)->partyIds + (j << 1)), (*shop)->modelId, -1))
                                usable = 1;
                            break;
                        case 4:
                            usable = MenuWeaponEquipCheck(*(short *)((char *)(*shop)->partyIds + (j + j)), ((short *)func_A1A428((*shop)->modelId))[6], 0) != 0;
                            break;
                        }
                    }
                    if (usable == 1) {
                        work->parties[i].sprites[j].blue = 0x80;
                        work->parties[i].sprites[j].green = 0x80;
                        work->parties[i].sprites[j].red = 0x80;
                    } else {
                        work->parties[i].sprites[j].blue = 0x40;
                        work->parties[i].sprites[j].green = 0x40;
                        work->parties[i].sprites[j].red = 0x40;
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
                        case 3:
                            if (MenuWeaponEquipCheck(*(short *)((char *)(*shop)->unitIds + (j << 1)), (*shop)->modelId, 0))
                                usable = 1;
                            break;
                        case 5:
                            if (MenuAccessoryEquipCheck(*(short *)((char *)(*shop)->unitIds + (j << 1)), (*shop)->modelId, -1))
                                usable = 1;
                            break;
                        case 4:
                            usable = MenuWeaponEquipCheck(*(short *)((char *)(*shop)->unitIds + (j << 1)), ((short *)func_A1A428((*shop)->modelId))[6], 0) != 0;
                            break;
                        }
                    }
                    if (usable == 1) {
                        work->parties[i].sprites[j].blue = 0x80;
                        work->parties[i].sprites[j].green = 0x80;
                        work->parties[i].sprites[j].red = 0x80;
                    } else {
                        work->parties[i].sprites[j].blue = 0x40;
                        work->parties[i].sprites[j].green = 0x40;
                        work->parties[i].sprites[j].red = 0x40;
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
        MoveSlide(&work->equipment.ribbon.x, &totalX[0], 3.0f);
        if ((&work->equipment)[i].ribbon.x == totalX[i])
            MoveSlide(&work->equipment.ribbon.y, &totalY[0], 3.0f);
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
        work->depth = 0xfffff0;
        WindowDXSet(&work->window);
        work->window.x = 0x210;
        work->window.style = work->depth;
        work->window.y = 0x50;
        work->window.width = 0xc0;
        work->window.height = 0x98;
        work->window.tag_id = msg00_6[0];
        work->window.state = 1;
        WindowDXMain(&work->window);
        work->window.state = 3;
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
        targetX[0] = 0x210;
        if (MenuShopWork->state == 0xa0 || MenuShopWork->state == 0xa2)
            targetX[0] = 0x130;
        MoveSlide(&work->window.x, targetX, 3.0f);
        work->window.title_font.render.text = msg00_6[MenuShopWork->mode & 1];
        WindowDXMain(&work->window);
        for (i = 0; i < 4; i++) {
            work->labels[i].x = (unsigned short)work->window.x + 0x43;
            work->labels[i].y = (unsigned short)work->window.y +
                ((i % 2) * 0x30 + (i / 2) * 0x4c) + 4;
            eTagFontMain(&work->labels[i]);
        }
        for (i = 0; i < 2; i++) {
            if (MenuShopWork->mode == 0)
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
            if (MenuShopWork->mode == 0) {
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
        endPrintExtFunc(work->depth, 0, work->quantityDivider);
        endPrintExtFunc(work->depth, 0, work->moneyDivider);
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
        work->ticker = 0x00fffff0;
        WindowDXSet(&work->window);
        work->window.x = 528;
        work->window.y = 80;
        work->window.style = work->ticker;
        work->window.width = 208;
        work->window.height = 80;
        work->window.tag_id = menuShopPayText;
        tagSlot = &work->window.tag_id;
        work->window.state = 1;
        WindowDXMain(&work->window);
        work->window.state = 3;
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
        case 0:
            work->flag = 0;
            work->state = 1;
            /* fall through */
        case 1:
            switch (MenuShopWork->state) {
            case 0xC2:
                work->state = 10;
                break;
            case 0xD4:
                work->state = 20;
                break;
            }
            break;
        case 10:
            work->window.x = -224;
            work->window.y = 80;
            work->state = 11;
            work->kind = 1;
            work->flag = 1;
            /* fall through */
        case 11:
            slideTarget48 = 48;
            MoveSlide(&work->window.x, &slideTarget48, 3.0f);
            if (MenuShopWork->state == 0xC0)
                work->state = 12;
            break;
        case 12:
            slideTargetBack = -224;
            MoveSlide(&work->window.x, &slideTargetBack, 3.0f);
            if (work->window.x == slideTargetBack)
                work->state = 0;
            break;
        case 20:
            work->window.x = 528;
            work->window.y = 64;
            work->kind = 2;
            work->state = 21;
            work->flag = 1;
            /* fall through */
        case 21:
            slideTarget288 = 288;
            MoveSlide(&work->window.x, &slideTarget288, 3.0f);
            switch (MenuShopWork->state) {
            case 0xC0:
            case 0xD0:
            case 0xD2:
                work->state = 22;
                break;
            }
            break;
        case 22:
            slideTarget528 = 528;
            MoveSlide(&work->window.x, &slideTarget528, 3.0f);
            if (work->window.x == slideTarget528)
                work->state = 0;
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
            work->numbers[1].value = 100;
        } else {
            int item;

            if (MenuShopWork->sort_mode_selector != 0)
                item = 0xB0000 + (unsigned short)MenuShopWork->equipmentId;
            else
                item = 0xC0000 + (unsigned short)MenuShopWork->equipmentId;
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
        endPrintExtFunc(work->ticker, 0, &work->points[0]);
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
        work->depth = 0xffff0;
        eRibbonSet(&work->ribbon, 3);
        work->ribbon.x = 0x210;
        work->ribbon.y = 0x40;
        work->ribbon.work = work->depth;
        work->ribbon.width = 0x100;
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
        case 0:
            work->active = 0;
            work->state = 1;
            /* fall through */
        case 1:
            switch (MenuShopWork->state) {
            case 0xd0:
            case 0xd2:
                work->state = 20;
                break;
            case 0xc2:
                work->state = 10;
                break;
            }
            break;
        case 10:
        case 20:
            work->ribbon.x = 0x210;
            work->active = 1;
            if (work->state == 10) {
                work->state = 11;
                work->cursor.state = 0x20;
            } else {
                work->state = 21;
            }
            /* fall through */
        case 11:
        case 21:
        {
            unsigned char shopState;
            openX = 0x130;
            MoveSlide(&work->ribbon.x, &openX, 3.0f);
            shopState = MenuShopWork->state;
            if (shopState == 0xc0) {
                work->state = 12;
            }
            if (MenuShopWork->state == 0xd4) {
                work->state = 12;
            }
            if (work->state == 21) {
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
                        work->portraits[i].blue = 0x40;
                        work->portraits[i].green = 0x40;
                        work->portraits[i].red = 0x40;
                    } else {
                        work->portraits[i].blue = -0x80;
                        work->portraits[i].green = -0x80;
                        work->portraits[i].red = -0x80;
                    }
                }
                if (MenuShopWork->state == 0xd2 && work->cursor.state == 0)
                    work->cursor.state = 0x20;
                if (MenuShopWork->state == 0xd0)
                    work->cursor.state = 0;
            } else {
                for (i = 0; i < MenuShopWork->unitCount; i++) {
                    short *unitIds = MenuShopWork->unitIds;
                    eSpriteSet(&work->portraits[i], MenuFaceEpidGet(unitIds[i], 0));
                    unitIds = MenuShopWork->unitIds;
                    if (MenuCharHpCheck(unitIds[i]) != 0) {
                        work->portraits[i].blue = 0x40;
                        work->portraits[i].green = 0x40;
                        work->portraits[i].red = 0x40;
                    } else {
                        work->portraits[i].blue = -0x80;
                        work->portraits[i].green = -0x80;
                        work->portraits[i].red = -0x80;
                    }
                }
            }
            break;
        }
        case 12:
            closeX = 0x210;
            MoveSlide(&work->ribbon.x, &closeX, 3.0f);
            if (work->ribbon.x == closeX)
                work->state = 0;
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
            work->depth = 0xfffff0;
            for (i = 0; i < 11; i++) {
                eSpriteSet(&work->sprites[i], spriteIds.ids[i]);
                work->sprites[i].depth = work->depth;
            }
        }
        {
            short topY = 12;
            for (i = 1; i >= 0; i--)
                work->sprites[i].y = topY;
        }
        for (i = 2; i < 11; i++) {
            work->sprites[i].y = 12;
            if (((MenuShopWork->categoryMask >> (i - 2)) & 1) == 0) {
                work->sprites[i].blue = 0x40;
                work->sprites[i].green = 0x40;
                work->sprites[i].red = 0x40;
            }
        }
        work->active = 1;
        work->slide = 16;
        work->scale = 1.0f;
        work->animation = 0;
        work->phase = 0;
        for (i = 0; i < 9; i++)
            work->categoryLift[i] = 0;
        work->state = 2;
        return;
    case 2:
        switch (MenuShopWork->state) {
        case 0x20:
            if (MenuShopWork->stateFlags & 1) {
                for (i = 2; i < 11; i++) {
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
        case 0xa0:
        case 0xa4:
        case 0xa2:
        case 0x30:
        case 0x90:
            if (MenuShopWork->stateFlags & 1) {
                for (i = 2; i < 11; i++) {
                    if (((MenuShopWork->categoryMask >> (i - 2)) & 1) == 0) {
                        work->sprites[i].blue = 0x40;
                        work->sprites[i].green = 0x40;
                        work->sprites[i].red = 0x40;
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
        case 0xf0:
            work->active = 0;
            /* fall through */
        case 0xff:
            if (work->slide != 16)
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
        for (i = 0; i < 9; i++) {
            if (i == MenuShopWork->category) {
                unsigned char state = MenuShopWork->state;
                if (state != 0x20 && state != 0xf0 &&
                    (state >> 4) != 0xc && (state >> 4) != 0xd) {
                    if (work->categoryLift[i] != 6)
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
        for (i = 2; i < 11; i++) {
            float alpha;
            work->sprites[i].x = 0x121 + (i - 2) * 0x15;
            work->sprites[i].x = (int)((float)work->sprites[i].x +
                xglCos((float)(i - 2) * 0.448798954f) * (float)-(work->slide * 2));
            work->sprites[i].y = work->categoryLift[i - 2] + 12;
            alpha = (128.0f - (float)work->slide * 0.0625f * 128.0f) * work->scale;
            work->sprites[i].alpha = alpha;
        }
        }
        for (i = 0; i < 11; i++)
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
    case 0:
        if (MenuShopWork->equipmentId != 0) {
            frame = func_A1A598(MenuShopWork->equipmentId);
            MenuShopPara.value[0] = frame->value[0];
            MenuShopPara.value[2] = frame->value[1];
            MenuShopPara.value[3] = frame->value[2];
            MenuShopPara.value[4] = frame->value[3];
        }
        break;
    case 1:
        if (MenuShopWork->equipmentId != 0) {
            engine = func_A1A5E8(MenuShopWork->equipmentId);
            MenuShopPara.value[1] = engine->value;
        }
        break;
    case 2:
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
    case 3:
    case 4:
        unit = func_A191C0(MenuShopWork->unitId);
        MenuAgwsParaSet(MenuShopWork->unitId, 10);
        for (i = 0; i < 6; i++)
            MenuShopPara.value[i] = MenuAgwsPara.value[i];
        if (mode == 3) {
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
        if (mode == 3)
            unit->frameId = oldEquipment;
        else
            unit->engineId = oldEquipment;
        break;
    case 10:
        if ((short)(MenuShopWork->itemId >> 16) != 3)
            return;
        MenuShopParaSet2(MenuShopWork->modelId, 0);
        return;
    case 11:
        if ((short)(MenuShopWork->itemId >> 16) != 5)
            return;
        MenuShopParaSet2(MenuShopWork->modelId, 1);
        return;
    case 12:
        if ((short)(MenuShopWork->itemId >> 16) == 4)
            MenuShopPara.value[0] = *func_A1A428(MenuShopWork->modelId);
        break;
    case 13:
        if ((short)(MenuShopWork->itemId >> 16) == 3) {
            weapon = func_A1A3D8(MenuShopWork->modelId);
            MenuShopPara.value[1] = weapon->shopParameter1;
            MenuShopPara.value[2] = weapon->shopParameter2;
            MenuShopPara.value[5] = weapon->wagl;
        }
        break;
    case 14:
        if ((short)(MenuShopWork->itemId >> 16) == 5) {
            accessory = func_A1A548(MenuShopWork->modelId);
            MenuShopPara.value[2] = accessory->shopParameter2;
            MenuShopPara.value[3] = accessory->shopParameter3;
        }
        break;
    case 15:
        if ((short)(MenuShopWork->itemId >> 16) == 4)
            MenuShopPara.value[1] = *func_A1A428(MenuShopWork->modelId);
        break;
    case 16:
        if ((short)(MenuShopWork->itemId >> 16) == 7) {
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
        work->depth = 0xffff00;
        WindowDXSet(&work->window);
        work->window.x = -0x1f0;
        work->window.style = work->depth;
        work->window.y = 0x149;
        work->window.width = 0x1e0;
        work->window.height = 0x36;
        work->window.state = 1;
        WindowDXMain(&work->window);
        work->window.state = 3;
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
        case 0x30:
        {
            int category = MenuShopWork->category;
            switch (category) {
            case 1:
            case 2:
            case 3:
                    switch (category) {
                    case 1: MenuShopParaSet(10); break;
                    case 2: MenuShopParaSet(11); break;
                    case 3: MenuShopParaSet(12); break;
                    }
                    work->twoColumns = 0;
                    work->count = 7;
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
            case 4:
            case 5:
            case 6:
            case 7:
                    switch (category) {
                    case 4: MenuShopParaSet(13); break;
                    case 5: MenuShopParaSet(14); break;
                    case 6: MenuShopParaSet(15); break;
                    case 7: MenuShopParaSet(16); break;
                    }
                    work->count = 6;
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
        case 0xa0:
        case 0xa2:
        case 0xa4:
            if (MenuShopWork->mode == 0 && MenuShopWork->category != 0 && MenuShopWork->category != 8)
                targetX = 0x10;
            break;
        case 0xd0:
        case 0xd2:
        case 0xd4:
        case 0xd6:
            if (MenuShopWork->sort_mode_selector != 0)
                MenuShopParaSet(3);
            else
                MenuShopParaSet(4);
            work->count = 6;
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
        case 0xc2:
        case 0xc4:
        case 0xc6:
            MenuShopParaSet(2);
            work->count = 6;
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
        MoveSlide(&work->window.x, &targetX, 3.0f);
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
        work->color = 0xFFFF00;
        WindowDXSet(&work->window);
        work->window.x = -0xB6;
        work->window.style = work->color;
        work->window.y = 0x100;
        work->window.width = 0xA6;
        work->window.height = 0x1E;
        work->window.tag_id = menuShopBulletTitleText;
        work->window.state = 1;
        WindowDXMain(&work->window);
        work->window.state = 3;
        eMessageSet(&work->message, 0);
        work->message.mode = 0x20;
        work->message.draw_parameter = work->color + 2;
        break;

    case 2:
        x = -0xB6;
        if (MenuShopWork->state == 0x30 && MenuShopWork->category == 4 && MenuShopWork->listIndex >= 0) {
            weapon = func_A1A3D8((short)MenuSortGet(0, MenuShopWork->listIndex));
            if (weapon != 0 && (weapon->flags & 0x100) && weapon->bullet != 0) {
                work->message.text = *func_A2C738(weapon->bullet);
                x = 0x10;
            }
        }
        MoveSlide(&work->window.x, &x, 3.0f);
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
        work->depth = 0xfffff0;
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
        targets[0] = MenuShopWork->category * 21 + 294;
        targets[1] = MenuShopWork->category * 21 + 298;
        targets[2] = MenuShopWork->category * 21 + 302;
        targets[3] = 512;
        switch (MenuShopWork->state) {
        case 0x30:
        case 0x90:
        case 0xa0:
        case 0xa2:
        case 0xa4:
            if (targets[0] == work->vertices[1].x)
                verticalTarget[0] = 40;
            if (work->alpha != 120)
                work->alpha += 8;
            break;
        default:
            if (work->alpha != 0)
                work->alpha -= 8;
            break;
        }
        work->vertices[0].x = 0;
        work->vertices[5].x = 512;
        target = targets;
        for (i = 1; i < 5; i++, target++) {
            MoveSlide(&work->vertices[i].x, target, 3.0f);
            work->vertices[i].rgba.a = work->alpha;
        }
        work->vertices[0].rgba.a = work->alpha;
        MoveSlide(&work->vertices[2].y, verticalTarget, 8.0f);
        endPrintExtFunc(work->depth, 0, work->vertices);
        break;
    }
}

void MenuShopInfoSet(void)
{
    MenuShopWork->categoryMask = 0;

    if (MenuShopWork->mode == 0) {
        if (MenuShopWork->noCategories != 0)
            return;
        if (ShopData.records[0].count != 0)
            MenuShopWork->categoryMask = 1;
        if (ShopData.records[1].count != 0)
            MenuShopWork->categoryMask |= 0x2;
        if (ShopData.records[2].count != 0)
            MenuShopWork->categoryMask |= 0x4;
        if (ShopData.records[3].count != 0)
            MenuShopWork->categoryMask |= 0x8;
        if (ShopData.records[4].count != 0)
            MenuShopWork->categoryMask |= 0x10;
        if (ShopData.records[5].count != 0)
            MenuShopWork->categoryMask |= 0x20;
        if (ShopData.records[6].count != 0)
            MenuShopWork->categoryMask |= 0x40;
        if (ShopData.tail.count != 0)
            MenuShopWork->categoryMask |= 0x80;
        if (MenuScenarioNo >= 115)
            MenuShopWork->categoryMask |= 0x100;
    } else {
        MenuSortSet(0, 1, 0);
        if (MenuSortCheck(0) != 0)
            MenuShopWork->categoryMask |= 0x1;
        MenuSortSet(0, 4, -1);
        if (MenuSortCheck(0) != 0)
            MenuShopWork->categoryMask |= 0x2;
        MenuSortSet(0, 16, -1);
        if (MenuSortCheck(0) != 0)
            MenuShopWork->categoryMask |= 0x4;
        MenuSortSet(0, 8, -1);
        if (MenuSortCheck(0) != 0)
            MenuShopWork->categoryMask |= 0x8;
        MenuSortSet(0, 4, -2);
        if (MenuSortCheck(0) != 0)
            MenuShopWork->categoryMask |= 0x10;
        MenuSortSet(0, 16, -2);
        if (MenuSortCheck(0) != 0)
            MenuShopWork->categoryMask |= 0x20;
        MenuSortSet(0, 8, -2);
        if (MenuSortCheck(0) != 0)
            MenuShopWork->categoryMask |= 0x40;
        if (dataEvtBoxChk(53) != 0 || dataEvtBoxChk(54) != 0 || dataEvtBoxChk(55) != 0)
            MenuShopWork->categoryMask |= 0x100;
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
        MenuShopWork->stateFlags |= 1;
    } else {
        MenuShopWork->stateFlags &= 0xFE;
    }
    if (MenuShopWork->waitFrames != 0) {
        MenuShopWork->waitFrames--;
    }

    switch (MenuShopWork->state) {
    case 0x20:
        if (MenuShopWork->stateFlags & 1) {
            MenuShopWork->stateFlags |= 0x10;
            MenuShopWork->waitFrames = 8;
            break;
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        MenuShopWork->mode = MenuSelectMove(MenuShopWork->mode, 4, 0);
        if (PadData.half_2a == 0x20)
            switch (MenuShopWork->mode) {
            case 0:
            case 1: {
                u8 listStates[2] = { 0x30, 0x90 };
                u8 *nextStates = listStates;
                int categoryIndex;

                MenuShopInfoSet();
                if (MenuShopWork->categoryMask != 0) {
                    for (categoryIndex = 0; categoryIndex < 8; categoryIndex++) {
                        if ((MenuShopWork->categoryMask >> categoryIndex) & 1) {
                            MenuShopWork->category = categoryIndex;
                            break;
                        }
                    }
                    MenuShopWork->nextState = nextStates[MenuShopWork->mode];
                    MenuShopWork->stateFlags |= 0xC;
                    xglSoundEffectNormalID(1, 0);
                    MenuShopWork->waitFrames = 0xC;
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
                break;
            }
            case 2:
                if (MenuShopWork->equipmentStatus != 0) {
                    xglSoundEffectNormalID(1, 0);
                    MenuShopWork->nextState = 0xC0;
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
                break;
            case 3:
                xglSoundEffectNormalID(2, 0);
                MenuShopWork->nextState = 0xF0;
                break;
            }
        if (PadData.half_2a & 0x40) {
            MenuShopWork->nextState = 0xF0;
            xglSoundEffectNormalID(2, 0);
        }
        break;

    case 0x30:
    case 0x90:
        if (MenuShopWork->stateFlags & 1) {
            MenuShopWork->listIndex = MenuShopListChange00(1);
            MenuShopWork->itemId = MenuShopWork->modelId = 0;
            MenuShopWork->availableMoney = dataMoneyBoxChk();
        }
        if (MenuShopWork->waitFrames == 0) {
            MenuShopWork->listIndex = WindowSPSelect(MenuShopWinSP,PadData.mappedRepeat);
            WindowSPKeepSelect(MenuShopWinSP,&MenuKeepSelect[(MenuShopWork->category + MenuShopWork->mode * 9) * 5]);
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
            if (PadData.mappedRepeat & 0xA000) {
                u8 previousCategory;
                int categoryStep;
                int category;
                int count;

                previousCategory = MenuShopWork->category;
                WindowSPKeepSelectCheck(&MenuKeepSelect[(previousCategory + MenuShopWork->mode * 9) * 5]);
                categoryStep = (PadData.mappedRepeat & 0x2000) ? 1 : -1;
                category = MenuShopWork->category;
                for (count = 0; count < 9; count += categoryStep) {
                    category += categoryStep;
                    if (category < 0) {
                        category = 8;
                    }
                    if (category >= 9) {
                        category = 0;
                    }
                    if ((MenuShopWork->categoryMask >> category) & 1) {
                        MenuShopWork->category = category;
                        break;
                    }
                }
                if (previousCategory != MenuShopWork->category) {
                    xglSoundEffectNormalID(1, 0);
                    MenuShopWork->listIndex = MenuShopListChange00(0);
                    if (MenuShopWork->listIndex >= 0) {
                        MenuShopWork->canEquip = subMenuShopEquipCheck(MenuSortGet(0, MenuShopWork->listIndex), MenuShopWork->category);
                        MENU_SHOP_PREVIEW_ITEM(MenuShopWork->listIndex);
                    }
                    MenuShopWork->waitFrames = 8;
                }
            } else if (PadData.half_2a & 0x20) {
                if (MenuSortCheck(0) != 0) {
                    if (MenuShopWork->mode == 0) {
                        if (MenuBoxMoneyGet(MenuShopWork->itemId, 0) > MenuShopWork->availableMoney
                            || MenuBoxChk(MenuShopWork->itemId) >= 99) {
                            xglSoundEffectNormalID(5, 0);
                        } else {
                            MenuShopWork->nextState = 0xA0;
                            MenuShopWork->quantity = 1;
                            MenuShopWork->quantityLimit = MenuBoxChk(MenuShopWork->itemId);
                            xglSoundEffectNormalID(1, 0);
                        }
                    } else if (MenuShopNoSaleCheck(MenuShopWork->itemId) != 0) {
                        xglSoundEffectNormalID(5, 0);
                    } else {
                        MenuShopWork->nextState = 0xA0;
                        MenuShopWork->quantity = 1;
                        MenuShopWork->quantityLimit = MenuBoxChk(MenuShopWork->itemId);
                        xglSoundEffectNormalID(1, 0);
                    }
                }
            } else if (PadData.half_2a & 0x40) {
                if (MenuCursorKeepCheck() == 0) {
                    MenuShopWork->mode = 0;
                }
                MenuShopWork->nextState = 0x20;
                xglSoundEffectNormalID(2, 0);
            }
        } else {
            MenuShopWork->listIndex = WindowSPSelect(MenuShopWinSP,0);
        }
        break;

    case 0xA0: {
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
                        xglSoundEffectNormalID(3, 0);
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
                    xglSoundEffectNormalID(3, 0);
                    quantityState.playChangeSound = 0;
                }
            }
        }

        if (MenuShopWork->stateFlags & 1) {
            if (MenuShopWork->category == 7) {
                MenuShopWork->nextState = 0xA2;
                MenuShopWork->waitFrames = 1;
            } else {
                MenuShopWork->waitFrames = 8;
            }
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        if (MenuShopWork->mode == 0) {
            quantityState.maximumQuantity = 99 - MenuShopWork->quantityLimit;
            quantityState.minimumQuantity = 1;
            quantityState.playChangeSound = 1;
            quantityState.calculatedTotal = 0;
            if (PadData.mappedRepeat & 0x4000) {
                subMenuShopNumerDec();
            }
            if (PadData.mappedRepeat & 0x1000) {
                subMenuShopNumerInc();
            }
            if (PadData.mappedRepeat & 0x2000) {
                for (i = 0; i < 10; i++) {
                    subMenuShopNumerInc();
                }
            }
            if (PadData.mappedRepeat & 0x8000) {
                for (i = 0; i < 10; i++) {
                    subMenuShopNumerDec();
                }
            }
        } else {
            int minimumQuantity = 1;
            u8 quantityLimit;

            quantityLimit = MenuShopSelectionGet()->quantityLimit;
            if ((PadData.mappedRepeat & 0x4000) && MenuShopSelectionGet()->quantity != minimumQuantity) {
                MenuShopSelectionGet()->quantity--;
                xglSoundEffectNormalID(3, 0);
            }
            if ((PadData.mappedRepeat & 0x1000) && MenuShopSelectionGet()->quantity != quantityLimit) {
                MenuShopSelectionGet()->quantity++;
                xglSoundEffectNormalID(3, 0);
            }
            if ((PadData.mappedRepeat & 0x2000) && MenuShopSelectionGet()->quantity != quantityLimit) {
                MenuShopSelectionGet()->quantity += 10;
                if (MenuShopSelectionGet()->quantity > quantityLimit) {
                    MenuShopSelectionGet()->quantity = quantityLimit;
                }
                xglSoundEffectNormalID(3, 0);
            }
            if ((PadData.mappedRepeat & 0x8000) && MenuShopSelectionGet()->quantity != minimumQuantity) {
                MenuShopSelectionGet()->quantity -= 10;
                if (MenuShopSelectionGet()->quantity < minimumQuantity) {
                    MenuShopSelectionGet()->quantity = minimumQuantity;
                }
                xglSoundEffectNormalID(3, 0);
            }
        }
        if (PadData.half_2a & 0x20) {
            MenuShopWork->nextState = 0xA2;
            xglSoundEffectNormalID(1, 0);
        }
        if (PadData.half_2a & 0x40) {
            if (MenuShopWork->mode == 0) {
                MenuShopWork->nextState = 0x30;
            } else {
                MenuShopWork->nextState = 0x90;
            }
            xglSoundEffectNormalID(2, 0);
        }
        break;
    }

    case 0xA2:
        if (MenuShopWork->stateFlags & 1) {
            MenuShopWork->quantityAction = 0;
            MenuShopWork->waitFrames = 8;
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        MenuShopWork->quantityAction = MenuSelectMove(MenuShopWork->quantityAction, 2, 0);
        if (PadData.half_2a & 0x20) {
            if (MenuShopSelectionGet()->quantityAction == 0) {
                int i;

                if (MenuShopSelectionGet()->mode == 0) {
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
                MenuShopWork->nextState = 0xA4;
            } else if (MenuShopSelectionGet()->category == 7) {
                u8 listStates[2] = { 0x30, 0x90 };

                MenuShopWork->nextState = *(listStates + MenuShopSelectionGet()->mode);
            } else {
                MenuShopWork->nextState = 0xA0;
            }
            xglSoundEffectNormalID(1, 0);
        }
        if (PadData.half_2a & 0x40) {
            if (MenuShopSelectionGet()->category == 7) {
                u8 listStates[2] = { 0x30, 0x90 };

                MenuShopWork->nextState = *(listStates + MenuShopSelectionGet()->mode);
            } else {
                MenuShopWork->nextState = 0xA0;
            }
            xglSoundEffectNormalID(2, 0);
        }
        break;

    case 0xA4:
        if (MenuShopWork->stateFlags & 1) {
            if (MenuShopWork->mode == 0 && MenuShopWork->category == 7) {
                int partyUnitIds[16];
                int i;

                MenuShopWork->unitCount = PartyAgwsGet(partyUnitIds);
                for (i = 0; i < MenuShopWork->unitCount; i++) {
                    MenuShopPartyGet()->unitIds[i] = partyUnitIds[i];
                }
                MenuShopAgwsListChange();
            }
            MenuShopWork->listIndex = MenuShopListChange00(1);
            MenuShopWork->waitFrames = 8;
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        if (PadData.half_2a & 0x20) {
            u8 listStates[2] = { 0x30, 0x90 };

            MenuShopWork->nextState = *(listStates + MenuShopWork->mode);
            if (MenuShopWork->mode == 0) {
                MenuShopListColorChange(0, 0);
            }
            xglSoundEffectNormalID(1, 0);
        }
        break;

    case 0xC0:
        if (MenuShopWork->stateFlags & 1) {
            MenuShopWork->waitFrames = 8;
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        MenuShopWork->sortOption = MenuSelectMove(MenuShopWork->sortOption, 4, 0);
        if (PadData.mappedPressed == 0x20) {
            switch (MenuShopWork->sortOption) {
            case 0:
            case 1:
                if (MenuShopWork->sortOption == 0) {
                    MenuShopWork->sort_mode_selector = 1;
                } else {
                    MenuShopWork->sort_mode_selector = 0;
                }
                xglSoundEffectNormalID(1, 0);
                if (MenuShopWork->sort_mode_selector != 0) {
                    WindowSPKeepSelectCheck(&MenuKeepSelect[0x5A]);
                } else {
                    WindowSPKeepSelectCheck(&MenuKeepSelect[0x5F]);
                }
                MenuShopWork->listIndex = MenuShopListChange01(0);
                MenuShopWork->nextState = 0xD0;
                MenuShopWork->waitFrames = 0xC;
                break;
            case 2:
                if (dataMoneyBoxChk() >= 100) {
                    xglSoundEffectNormalID(1, 0);
                    MenuShopWork->nextState = 0xC2;
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
                MenuShopWork->waitFrames = 0xC;
                break;
            case 3:
                xglSoundEffectNormalID(2, 0);
                MenuShopWork->nextState = 0x20;
                break;
            }
        }
        if (PadData.mappedPressed == 0x40) {
            if (MenuCursorKeepCheck() == 0) {
                MenuShopWork->mode = 0;
            }
            MenuShopWork->nextState = 0x20;
            xglSoundEffectNormalID(2, 0);
        }
        break;

    case 0xC2:
        if (MenuShopWork->stateFlags & 1) {
            MenuShopWork->waitFrames = 0xC;
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        MenuShopWork->unitSelection = MenuSelectMove2(MenuShopAgwsGet()->unitSelection, MenuShopAgwsGet()->unitCount, 0);
        MenuShopAgwsGet()->unitId = MenuShopPartyGet()->unitIds[MenuShopAgwsGet()->unitSelection];
        if (PadData.mappedPressed == 0x20) {
            if (dataMoneyBoxChk() >= 100 && MenuCharHpCheck(MenuShopWork->unitId) == 0) {
                MenuShopWork->nextState = 0xC4;
                xglSoundEffectNormalID(1, 0);
            } else {
                xglSoundEffectNormalID(5, 0);
            }
        }
        if (PadData.mappedPressed == 0x40) {
            if (MenuCursorKeepCheck() == 0) {
                MenuShopWork->sortOption = 0;
            }
            MenuShopWork->nextState = 0xC0;
            xglSoundEffectNormalID(2, 0);
        }
        break;

    case 0xC4:
        if (MenuShopWork->stateFlags & 1) {
            MenuShopWork->quantityAction = 0;
            MenuShopWork->waitFrames = 8;
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        MenuShopWork->quantityAction = MenuSelectMove(MenuShopWork->quantityAction, 2, 0);
        if (PadData.mappedPressed == 0x20) {
            if (MenuShopWork->quantityAction == 0) {
                MenuShopWork->nextState = 0xC6;
                xglSoundEffectNormalID(1, 0);
            } else {
                MenuShopWork->nextState = 0xC2;
                xglSoundEffectNormalID(2, 0);
            }
        }
        if (PadData.mappedPressed == 0x40) {
            MenuShopWork->nextState = 0xC2;
            xglSoundEffectNormalID(2, 0);
        }
        break;

    case 0xC6:
        if (MenuShopWork->stateFlags & 1) {
            int repairAttack[4];
            int repairDefense[4];
            MenuShopUnitData *unit;
            ShopCalculatedPara *para;

            unit = func_A191C0(MenuShopPartyGet()->unitIds[MenuShopWork->unitSelection]);
            para = func_00A11108(MenuShopPartyGet()->unitIds[MenuShopWork->unitSelection], repairAttack, repairDefense);
            unit->hitPoints = para->maxHp;
            dataMoneyBoxDec(100);
            MenuShopWork->waitFrames = 2;
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        if (PadData.mappedPressed == 0x20) {
            if (dataMoneyBoxChk() >= 100) {
                MenuShopWork->nextState = 0xC2;
            } else {
                MenuShopWork->nextState = 0xC0;
            }
            xglSoundEffectNormalID(1, 0);
        }
        break;

    case 0xD0:
        if (MenuShopWork->waitFrames == 0) {
            MenuShopWork->listIndex = WindowSPSelect(MenuShopWinSP,PadData.mappedRepeat);
            if (MenuShopWork->sort_mode_selector != 0) {
                WindowSPKeepSelect(MenuShopWinSP,&MenuKeepSelect[0x5A]);
            } else {
                WindowSPKeepSelect(MenuShopWinSP,&MenuKeepSelect[0x5F]);
            }
            if (MenuShopWork->listIndex >= 0) {
                int itemId;
                int unitId;

                itemId = MenuSortGet(0, MenuShopWork->listIndex);
                MenuShopItemGet()->itemId = itemId;
                MenuShopItemGet()->modelId = (s16)MenuShopItemGet()->itemId;
                if (MenuShopItemGet()->listIndex >= 0) {
                    MenuShopAgwsGet()->equipmentId = MenuShopItemGet()->modelId;
                    MenuShopAgwsGet()->unitId = 0x11;
                    for (unitId = 0x11; unitId < 0x21; unitId++) {
                        if (MenuFrameEquipCheck(unitId, MenuShopWork->equipmentId) != 0) {
                            MenuShopWork->unitId = unitId;
                            break;
                        }
                    }
                    if (PadData.mappedPressed == 0x20) {
                        int money;

                        money = dataMoneyBoxChk();
                        if (money >= MenuBoxMoneyGet(MenuShopWork->itemId, 0)) {
                            MenuShopWork->nextState = 0xD4;
                            xglSoundEffectNormalID(1, 0);
                            MenuShopWork->waitFrames = 2;
                            if (MenuCursorKeepCheck() == 0) {
                                MenuShopWork->unitSelection = 0;
                            }
                        } else {
                            xglSoundEffectNormalID(5, 0);
                            MenuShopWork->waitFrames = 2;
                        }
                    }
                }
            }
            if (PadData.mappedPressed == 0x40) {
                if (MenuCursorKeepCheck() == 0) {
                    MenuShopWork->sortOption = 0;
                }
                MenuShopWork->nextState = 0xC0;
                MenuShopWork->waitFrames = 2;
                xglSoundEffectNormalID(2, 0);
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

    case 0xD2: {
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
        if (PadData.mappedPressed == 0x20) {
            if (MenuShopAgwsGet()->sort_mode_selector != 0) {
                canEquip = MenuFrameEquipCheck(MenuShopAgwsGet()->unitId, MenuShopAgwsGet()->equipmentId);
            } else {
                canEquip = MenuEngineEquipCheck(MenuShopAgwsGet()->unitId, MenuShopAgwsGet()->equipmentId);
            }
            if (canEquip != 0) {
                MenuShopWork->nextState = 0xD4;
                xglSoundEffectNormalID(1, 0);
            } else {
                xglSoundEffectNormalID(5, 0);
            }
        }
        if (PadData.mappedPressed == 0x40) {
            MenuShopWork->nextState = 0xD0;
            xglSoundEffectNormalID(2, 0);
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

    case 0xD4:
        if (MenuShopWork->stateFlags & 1) {
            MenuShopWork->quantityAction = 0;
            MenuShopWork->waitFrames = 8;
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        MenuShopWork->quantityAction = MenuSelectMove(MenuShopWork->quantityAction, 2, 0);
        if (PadData.mappedPressed == 0x20) {
            if (MenuShopWork->quantityAction == 0) {
                MenuShopWork->nextState = 0xD6;
                xglSoundEffectNormalID(1, 0);
            } else {
                MenuShopWork->nextState = 0xD0;
                xglSoundEffectNormalID(2, 0);
            }
        }
        if (PadData.mappedPressed == 0x40) {
            MenuShopWork->nextState = 0xD0;
            xglSoundEffectNormalID(2, 0);
        }
        break;

    case 0xD6:
        if (MenuShopWork->stateFlags & 1) {
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
                equipmentKey = (u16)MenuShopAgwsGet()->equipmentId + 0xB0000;
            } else {
                equipmentKey = (u16)MenuShopAgwsGet()->equipmentId + 0xC0000;
            }
            dataMoneyBoxDec(MenuBoxMoneyGet(equipmentKey, 0));
            MenuShopListColorChange(0, 2);
            MenuShopWork->waitFrames = 8;
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        if (PadData.mappedPressed == 0x20) {
            MenuShopWork->nextState = 0xD0;
            xglSoundEffectNormalID(1, 0);
        }
        break;

    case 0xF0:
        if (MenuShopWork->stateFlags & 1) {
            MenuShopWork->waitFrames = 0x10;
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        MenuShopWork->nextState = 0xFF;
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
    GameLoopState[4] |= 0x04000000;
    xglSleep();
    endPrintInit();
    GameLoopState[4] |= 0x04000000;
    MenuCfTaikiPush();
    MenuGameDataPush();
    xglStudioChange(0);
    endPrintDirectFrameCopy(xglPacketGetCurrent(), sRender.height << 5,
        sRender.flip_base << 5);

    workspace = (unsigned char *)(((unsigned int)MenuWorkEndGet() + 127) & ~127U);
    ShopDataBufTop = workspace;
    {
        MenuShopWorkData *initialWork =
            MenuShopAlignBuffer(workspace + 8192, 2048);
        MenuShopWork = initialWork;
        workspace = (unsigned char *)initialWork + 128;
        memset(initialWork, 0, 128);
    }
    workspace = MenuBgTaskInit(workspace, 3);
    MenuShopTask = (XglTaskScheduler *)workspace;
    workspace = xglTaskInitial(workspace, 16, 0);

    TskObjectSet3((TskObject *)xglTaskEntryNext(MenuShopTask,
        (int (*)(XglTaskPrefix *))tskTskMain3,
        MenuShopTask != 0 ? MenuShopTask->active_tail : 0),
        MenuShopPas, workspace);
    workspace += 1516;
    TskObjectSet3((TskObject *)xglTaskEntryNext(MenuShopTask,
        (int (*)(XglTaskPrefix *))tskTskMain3,
        MenuShopTask != 0 ? MenuShopTask->active_tail : 0),
        MenuShopInfo, workspace);
    workspace += 976;
    TskObjectSet3((TskObject *)xglTaskEntryNext(MenuShopTask,
        (int (*)(XglTaskPrefix *))tskTskMain3,
        MenuShopTask != 0 ? MenuShopTask->active_tail : 0),
        MenuShopList, workspace);
    MenuShopWinSP = (WindowSPCursor *)(workspace + 12);
    workspace += 5956;
    TskObjectSet3((TskObject *)xglTaskEntryNext(MenuShopTask,
        (int (*)(XglTaskPrefix *))tskTskMain3,
        MenuShopTask != 0 ? MenuShopTask->active_tail : 0),
        MenuShopSelect, workspace);
    workspace += 7928;
    TskObjectSet3((TskObject *)xglTaskEntryNext(MenuShopTask,
        (int (*)(XglTaskPrefix *))tskTskMain3,
        MenuShopTask != 0 ? MenuShopTask->active_tail : 0),
        MenuShopEx, workspace);
    workspace += 1728;
    TskObjectSet3((TskObject *)xglTaskEntryNext(MenuShopTask,
        (int (*)(XglTaskPrefix *))tskTskMain3,
        MenuShopTask != 0 ? MenuShopTask->active_tail : 0),
        MenuShopEx2, workspace);
    workspace += 480;
    TskObjectSet3((TskObject *)xglTaskEntryNext(MenuShopTask,
        (int (*)(XglTaskPrefix *))tskTskMain3,
        MenuShopTask != 0 ? MenuShopTask->active_tail : 0),
        MenuShopEx02, workspace);
    workspace += 472;
    TskObjectSet3((TskObject *)xglTaskEntryNext(MenuShopTask,
        (int (*)(XglTaskPrefix *))tskTskMain3,
        MenuShopTask != 0 ? MenuShopTask->active_tail : 0),
        MenuShopSeisan, workspace);
    workspace += 1668;
    TskObjectSet3((TskObject *)xglTaskEntryNext(MenuShopTask,
        (int (*)(XglTaskPrefix *))tskTskMain3,
        MenuShopTask != 0 ? MenuShopTask->active_tail : 0),
        MenuShopSeisan02, workspace);
    workspace += 984;
    TskObjectSet3((TskObject *)xglTaskEntryNext(MenuShopTask,
        (int (*)(XglTaskPrefix *))tskTskMain3,
        MenuShopTask != 0 ? MenuShopTask->active_tail : 0),
        MenuShopIcon, workspace);
    workspace += 508;
    TskObjectSet3((TskObject *)xglTaskEntryNext(MenuShopTask,
        (int (*)(XglTaskPrefix *))tskTskMain3,
        MenuShopTask != 0 ? MenuShopTask->active_tail : 0),
        MenuShopParameter, workspace);
    workspace += 1996;
    TskObjectSet3((TskObject *)xglTaskEntryNext(MenuShopTask,
        (int (*)(XglTaskPrefix *))tskTskMain3,
        MenuShopTask != 0 ? MenuShopTask->active_tail : 0),
        MenuShopLine, workspace);
    workspace += 80;
    TskObjectSet3((TskObject *)xglTaskEntryNext(MenuShopTask,
        (int (*)(XglTaskPrefix *))tskTskMain3,
        MenuShopTask != 0 ? MenuShopTask->active_tail : 0),
        MenuShopModelMain, 0);
    workspace = MenuModelInit(workspace);

    modelMemory = GameResourceWorkAlloc(0x180000);
    xglStudioChange(0);
    MenuModelMemoryInit(modelMemory, 0x180000);
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
    xglSoundEffectNormalID(1, 0);

    while (MenuLoadSync() != 0) {
        MenuBgTaskMain();
        xglSleep();
    }

    ShopData = ((const MenuShopData *)ShopDataBufTop)[shopType - 1];
    if (shopType == 20)
        MenuShopWork->noCategories = 1;
    if (shopType == 6 && MenuEquipStealMaskCheck() == 1)
        MenuShopWork->noCategories = 1;
    MenuShopAgwsListChange();
    MenuShopWork->nextState = 32;
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

    if (unitCount != 0 && ShopData.records[7].count != 0)
        MenuShopWork->equipmentStatus = 1;
    else
        MenuShopWork->equipmentStatus = 0;
    if (ShopData.tail.count != 0)
        MenuShopWork->modelBreakPending = 1;
    else
        MenuShopWork->modelBreakPending = 0;
    MenuScenarioNo = MenuScenarioNoGet();

    while ((GameSnapShotCheck(), MenuShopWork->state != 0xff) &&
        (MenuShopCore(), 1)) {
        endPrintExtFunc(0, 100, 0);
        MenuBgTaskMain();
        nmlModelFlush();
        xglTaskExecute(MenuShopTask);
        MenuModelMain();
        xglSleep();
    }

    MenuLoadCancel();
    MenuBgTaskBreak();
    MenuShopWork->state = 0xff;
    xglTaskExecute(MenuShopTask);
    if (MenuShopWork->modelBreakPending != 0)
        MenuModelAllBreak();
    MenuCfTaikiPop();
    MenuFontLoad(0, 0);
    WindowTexLoad(0, 0);
    GameResourceWorkReload();
    GameCfPlayerLoadResource(1);
    MenuGameDataPop();
    GameLoopState[4] &= ~0x04000000;
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
