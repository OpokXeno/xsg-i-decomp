#include "common.h"
#include "shared.h"
#include "main/pause_menu_page_uwamono.h"
#include "main/init_uwamono_sys.h"
#include "main/karakama_test.h"
#include "main/xgl_2.h"
#include "main/xgl_flags.h"
#include "main/xgl_font.h"

#define PILLAR_SEGMENT_ANGLE (0.3926991f)
#define PILLAR_CORNER_ANGLE (0.3926991f)
#define COLL_POSITION_W (0.3f)
#define COLL_CORNER_W (0.1f)
#define MAP_UNIT_COLLISION 0x7000
#define MAP_UNIT_SOUND 0x7012
#define UWAMONO_SELF_BREAK 0x1
#define UWAMONO_ENEMY_BREAK 0x2

/* MapUnit serials below this are map parts; from it up they are models. */
#define UWAMONO_SERIAL_MODEL_BASE 0x1000
/* MapUnit flags: unit is loaded; unit is hidden by a hide trigger. */
#define UWAMONO_UNIT_LOADED 0x10000
#define UWAMONO_UNIT_HIDDEN 0x100000
/* GameLoopState.flags bit toggled by the "shooting flag" row. */
#define UWAMONO_GAME_SHOOTING_FLAG 0x1

/* Field-object page (PauseMenuPageUwamono): submode shown instead of the menu. */
enum {
    UWAMONO_DEBUG_MENU,
    UWAMONO_DEBUG_EVENT_ITEMS,
    UWAMONO_DEBUG_TREASURE
};

/* Rows of the field-object page, in display order. */
enum {
    UWAMONO_ROW_SHOT_TRAJECTORY,
    UWAMONO_ROW_OBJECT_INFO,
    UWAMONO_ROW_OBJECT_OUTPUT,
    UWAMONO_ROW_COLLISION,
    UWAMONO_ROW_SHOOTING_FLAG,
    UWAMONO_ROW_EVENT_ITEM_LIST,
    UWAMONO_ROW_TREASURE_LIST,
    UWAMONO_ROW_COUNT
};

/* DebDispEvtItem lists EvtItemTbl entries four to a page. */
#define EVENT_ITEM_COUNT 255
#define EVENT_ITEM_ENTRY_SIZE 0x80
#define EVENT_ITEM_PAGE_ROWS 4
#define EVENT_ITEM_PAGE_COUNT 64
/* DebDispItemBox lists ItemBoxTbl entries eight to a page. */
#define ITEM_BOX_PAGE_ROWS 8
#define ITEM_BOX_PAGE_COUNT 76
/* Event flag of treasure chest id: base + id (set once the chest was opened). */
#define ITEM_BOX_OBTAINED_FLAG_BASE 0x79EC7

/* Field-object markers are drawn only inside this GS-space window, relative to this origin. */
#define UWAMONO_SCREEN_X_MIN 0x7000
#define UWAMONO_SCREEN_X_MAX 0x9000
#define UWAMONO_SCREEN_Y_MIN 0x6E01
#define UWAMONO_SCREEN_Y_MAX 0x8DFF
#define UWAMONO_SCREEN_ORIGIN_X 0x700
#define UWAMONO_SCREEN_ORIGIN_Y 0x720


typedef struct ItemBoxEntry {
    signed char category;
    unsigned char index;
    signed char count;
    signed char chance;
    int money;
} ItemBoxEntry;

/* Corner arrays: the original copies their elements with aligned doublewords
 * (ld/sd). AlignedVector4 array elements are copied with ldl/ldr under this
 * compiler (gcc 2.96 drops the typedef's aligned(8) on array elements, also for
 * DoublewordAlignedVector4), so the arrays stay the union. */
typedef struct UwamonoPadEntry {
    unsigned char unmodeled_00[0x28];
    u16 held;
    u16 pressed;
    u16 repeat;
    unsigned char unmodeled_2e[0x68 - 0x2e];
} UwamonoPadEntry;

typedef struct UwamonoGameStatePrefix {
    unsigned char unmodeled_00[0x20];
    u32 flags;
} UwamonoGameStatePrefix;

typedef struct UwamonoInfo {
    short kind;
    short item;
    unsigned char unmodeled_04[6];
    signed char bounds_shown;
    unsigned char unmodeled_0b[0x22];
    signed char shape;
    unsigned char unmodeled_2e[0x16];
    u32 state_flags;
    int sound;
} UwamonoInfo;

typedef struct UwamonoDebugUnit {
    u32 flags;
    unsigned char unmodeled_04[0x0c];
    AlignedVector4 position;
    unsigned char unmodeled_20[0x60];
    float (*matrix)[4];
    unsigned char unmodeled_84[0x20];
    short serial;
    unsigned char unmodeled_a6[0x0a];
    AlignedVector4 size;
    unsigned char unmodeled_c0[0xe0];
    UwamonoInfo info;
    unsigned char unmodeled_1ec[0x300 - 0x1ec];
} UwamonoDebugUnit;

enum {
    UWAMONO_DOOR = 1,
    UWAMONO_BREAKABLE,
    UWAMONO_COLLISION,
    UWAMONO_TREASURE,
    UWAMONO_ITEM,
    UWAMONO_SAVE,
    UWAMONO_SHOP
};

/* PadData's +0x28/+0x2a/+0x2c fields match the local input view in input.c. */
extern UwamonoPadEntry PadData[2];
extern char EvtItemTbl[EVENT_ITEM_COUNT * EVENT_ITEM_ENTRY_SIZE];
extern ItemBoxEntry ItemBoxTbl[ITEM_BOX_COUNT];
extern UwamonoGameStatePrefix GameLoopState;
extern UwamonoDebugUnit MapUnit[MAP_UNIT_COUNT];
extern const char PauseMenuFieldObjectLabel[];
char pauseMenuPageUwamonoCursorMarkerText[];
const char eventItemNameFormat[];
char treasureChestText[];
const char shotTrajectoryOnText[];
const char shotTrajectoryOffText[];
const char fieldObjectInfoOnText[];
const char fieldObjectInfoOffText[];
const char fieldObjectOutputOnText[];
const char fieldObjectOutputOffText[];
const char collisionDisplayOnText[];
const char collisionDisplayOffText[];
const char shootingFlagOnText[];
const char shootingFlagOffText[];
const char eventItemListText[];
const char treasureChestListText[];
const char pressCrossToReturnText[];
const char eventItemRowFormat[];

extern const char *GetItemName(int category, int index);
extern int HexToStr(int value, char *buffer);
extern void *memset(void *s, int c, unsigned int n);
extern void xglRotTransPers(int *screen, float (*matrix)[4], Vector4 *point, int mode);
void DebDispEvtItem(void);
void DebDispItemBox(void);

static int debmode;

static int debmode = 0;

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
static int page_2;

/* JP: 射撃軌跡表示　　オン
 * EN: Shot trajectory display: On */
const char shotTrajectoryOnText[24] = "\xBC\xCD\xB7\xE2\xB5\xB0\xC0\xD7\xC9\xBD\xBC\xA8\xA1\xA1\xA1\xA1\xA5\xAA\xA5\xF3";

/* JP: 射撃軌跡表示　　オフ
 * EN: Shot trajectory display: Off */
const char shotTrajectoryOffText[24] = "\xBC\xCD\xB7\xE2\xB5\xB0\xC0\xD7\xC9\xBD\xBC\xA8\xA1\xA1\xA1\xA1\xA5\xAA\xA5\xD5";

/* JP: 上物情報表示　　オン
 * EN: Field object information display: On */
const char fieldObjectInfoOnText[24] = "\xBE\xE5\xCA\xAA\xBE\xF0\xCA\xF3\xC9\xBD\xBC\xA8\xA1\xA1\xA1\xA1\xA5\xAA\xA5\xF3";

/* JP: 上物情報表示　　オフ
 * EN: Field object information display: Off */
const char fieldObjectInfoOffText[24] = "\xBE\xE5\xCA\xAA\xBE\xF0\xCA\xF3\xC9\xBD\xBC\xA8\xA1\xA1\xA1\xA1\xA5\xAA\xA5\xD5";

/* JP: 上物情報出力　　オン
 * EN: Field object information output: On */
const char fieldObjectOutputOnText[24] = "\xBE\xE5\xCA\xAA\xBE\xF0\xCA\xF3\xBD\xD0\xCE\xCF\xA1\xA1\xA1\xA1\xA5\xAA\xA5\xF3";

/* JP: 上物情報出力　　オフ
 * EN: Field object information output: Off */
const char fieldObjectOutputOffText[24] = "\xBE\xE5\xCA\xAA\xBE\xF0\xCA\xF3\xBD\xD0\xCE\xCF\xA1\xA1\xA1\xA1\xA5\xAA\xA5\xD5";

/* JP: コリジョン表示　オン
 * EN: Collision display: On */
const char collisionDisplayOnText[24] = "\xA5\xB3\xA5\xEA\xA5\xB8\xA5\xE7\xA5\xF3\xC9\xBD\xBC\xA8\xA1\xA1\xA5\xAA\xA5\xF3";

/* JP: コリジョン表示　オフ
 * EN: Collision display: Off */
const char collisionDisplayOffText[24] = "\xA5\xB3\xA5\xEA\xA5\xB8\xA5\xE7\xA5\xF3\xC9\xBD\xBC\xA8\xA1\xA1\xA5\xAA\xA5\xD5";

/* JP: 射撃フラグ　　　オン
 * EN: Shooting flag: On */
const char shootingFlagOnText[24] = "\xBC\xCD\xB7\xE2\xA5\xD5\xA5\xE9\xA5\xB0\xA1\xA1\xA1\xA1\xA1\xA1\xA5\xAA\xA5\xF3";

/* JP: 射撃フラグ　　　オフ
 * EN: Shooting flag: Off */
const char shootingFlagOffText[24] = "\xBC\xCD\xB7\xE2\xA5\xD5\xA5\xE9\xA5\xB0\xA1\xA1\xA1\xA1\xA1\xA1\xA5\xAA\xA5\xD5";

/* JP: イベントアイテム一覧
 * EN: Event item list */
const char eventItemListText[24] = "\xA5\xA4\xA5\xD9\xA5\xF3\xA5\xC8\xA5\xA2\xA5\xA4\xA5\xC6\xA5\xE0\xB0\xEC\xCD\xF7";

/* JP: 宝箱中身一覧
 * EN: Treasure chest contents list */
const char treasureChestListText[16] = "\xCA\xF5\xC8\xA2\xC3\xE6\xBF\xC8\xB0\xEC\xCD\xF7";

void PauseMenuPageUwamono(void)
{
    static int cursol = 0;
    if (debmode == UWAMONO_DEBUG_EVENT_ITEMS) {
        DebDispEvtItem();
        return;
    }
    if (debmode == UWAMONO_DEBUG_TREASURE) {
        DebDispItemBox();
        return;
    }
    xglFontDebugPrintf(16, 16, PauseMenuFieldObjectLabel);
    xglFontDebugPrintf(48, cursol * 16 + 32, pauseMenuPageUwamonoCursorMarkerText);
    if (lineflg != 0) xglFontDebugPrintf(64, 32, shotTrajectoryOnText);
    else xglFontDebugPrintf(64, 32, shotTrajectoryOffText);
    if (dispflg != 0) xglFontDebugPrintf(64, 48, fieldObjectInfoOnText);
    else xglFontDebugPrintf(64, 48, fieldObjectInfoOffText);
    if (printflg != 0) xglFontDebugPrintf(64, 64, fieldObjectOutputOnText);
    else xglFontDebugPrintf(64, 64, fieldObjectOutputOffText);
    if (collflg != 0) xglFontDebugPrintf(64, 80, collisionDisplayOnText);
    else xglFontDebugPrintf(64, 80, collisionDisplayOffText);
    if (GameLoopState.flags & UWAMONO_GAME_SHOOTING_FLAG) xglFontDebugPrintf(64, 96, shootingFlagOnText);
    else xglFontDebugPrintf(64, 96, shootingFlagOffText);
    xglFontDebugPrintf(64, 112, eventItemListText);
    xglFontDebugPrintf(64, 128, treasureChestListText);
    if (PadData[0].pressed & PAD_UP) {
        cursol--;
        if (cursol < 0) cursol = UWAMONO_ROW_COUNT - 1;
    }
    if (PadData[0].pressed & PAD_DOWN) {
        cursol++;
        cursol %= UWAMONO_ROW_COUNT;
    }
    if ((cursol == UWAMONO_ROW_EVENT_ITEM_LIST) && (PadData[0].pressed & PAD_CIRCLE)) debmode = UWAMONO_DEBUG_EVENT_ITEMS;
    if ((cursol == UWAMONO_ROW_TREASURE_LIST) && (PadData[0].pressed & PAD_CIRCLE)) debmode = UWAMONO_DEBUG_TREASURE;
    if (PadData[0].pressed & PAD_RIGHT) {
        switch (cursol) {
        case UWAMONO_ROW_SHOT_TRAJECTORY: lineflg = 1; break;
        case UWAMONO_ROW_OBJECT_INFO: dispflg = 1; break;
        case UWAMONO_ROW_OBJECT_OUTPUT: printflg = 1; break;
        case UWAMONO_ROW_COLLISION: collflg = 1; break;
        case UWAMONO_ROW_SHOOTING_FLAG: GameLoopState.flags |= UWAMONO_GAME_SHOOTING_FLAG; break;
        }
    }
    if (PadData[0].pressed & PAD_LEFT) {
        switch (cursol) {
        case UWAMONO_ROW_SHOT_TRAJECTORY: lineflg = 0; return;
        case UWAMONO_ROW_OBJECT_INFO: dispflg = 0; return;
        case UWAMONO_ROW_OBJECT_OUTPUT: printflg = 0; return;
        case UWAMONO_ROW_COLLISION: collflg = 0; return;
        case UWAMONO_ROW_SHOOTING_FLAG:
            UnlockMapUnit();
            GameLoopState.flags &= ~UWAMONO_GAME_SHOOTING_FLAG;
            break;
        }
    }
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
/* JP: ＞
 * EN: > */
char pauseMenuPageUwamonoCursorMarkerText[4] = "\xA1\xE4";

/* JP: ×ボタンで戻る
 * EN: Press the cross button to return */
const char pressCrossToReturnText[16] = "\xA1\xDF\xA5\xDC\xA5\xBF\xA5\xF3\xA4\xC7\xCC\xE1\xA4\xEB";

const char eventItemRowFormat[16] = "%3d %s %c";

void DebDispEvtItem(void)
{
    static int page = 0;
    int row;
    int item_index;
    char *event_item;

    xglFontDebugPrintf(16, 16, eventItemListText);
    xglFontDebugPrintf(16, 32, pressCrossToReturnText);
    for (row = 0; row < EVENT_ITEM_PAGE_ROWS; row++) {
        item_index = row + page * EVENT_ITEM_PAGE_ROWS;
        if (item_index >= EVENT_ITEM_COUNT) {
            break;
        }
        event_item = &EvtItemTbl[item_index * EVENT_ITEM_ENTRY_SIZE];
        xglFontDebugPrintf(4, 60 + row * 34, eventItemRowFormat,
                           item_index + 1, event_item, event_item[0]);
        while (*event_item++ != 0) {
        }
        xglFontDebugPrintf(16, 72 + row * 34, eventItemNameFormat, event_item);
    }
    if (PadData[0].repeat & PAD_UP) {
        page--;
        if (page < 0) {
            page = EVENT_ITEM_PAGE_COUNT - 1;
        }
    }
    if (PadData[0].repeat & PAD_DOWN) {
        page = (page + 1) % EVENT_ITEM_PAGE_COUNT;
    }
    debmode = (PadData[0].pressed & PAD_CROSS) ? UWAMONO_DEBUG_MENU : debmode;
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const char eventItemNameFormat[4] = "%s";

static int page_2 = 0;

void DebDispItemBox(void)
{
    int row;
    int y = 60;

    xglFontDebugPrintf(16, 16, treasureChestListText);
    xglFontDebugPrintf(16, 32, pressCrossToReturnText);
    for (row = 0; row < ITEM_BOX_PAGE_ROWS; row++) {
        char amount_text[16];
        char chance_text[16];
        int id;
        int category;
        int money;
        int count;
        int chance;
        const char *name;

        memset(amount_text, 0, sizeof(amount_text));
        memset(chance_text, 0, sizeof(chance_text));
        id = row + page_2 * ITEM_BOX_PAGE_ROWS;
        if (id > ITEM_BOX_COUNT - 1)
            break;
        category = ItemBoxTbl[id].category;
        money = ItemBoxTbl[id].money;
        count = ItemBoxTbl[id].count;
        chance = ItemBoxTbl[id].chance;
        name = GetItemName(category, ItemBoxTbl[id].index);
        if (name == 0)
            name = "NULL";
        if (money == 0) {
            HexToStr(count, amount_text);
            xglFontDebugPrintf(4, y, "%3d %s", id, name);
            /* JP: %s個
             * EN: %s items */
            xglFontDebugPrintf(128, y, "%s\270\304", amount_text);
        } else {
            HexToStr(money, amount_text);
            /* JP: %3d お金 %sＧ
             * EN: %3d Money %sG */
            xglFontDebugPrintf(4, y, "%3d \244\252\266\342 %s\243\307", id, amount_text);
        }
        if (xglFlagsGet1(id + ITEM_BOX_OBTAINED_FLAG_BASE) == 1)
            /* JP: 取得済
             * EN: Already obtained */
            xglFontDebugPrintf(152, y, "\274\350\306\300\272\321");
        if (chance != 0) {
            HexToStr(chance, chance_text);
            /* JP: 確率%s
             * EN: Probability %s */
            xglFontDebugPrintf(186, y, "\263\316\316\250%s", chance_text);
        }
        y += 16;
    }
    if (PadData[0].repeat & PAD_UP) {
        page_2--;
        if (page_2 < 0)
            page_2 = ITEM_BOX_PAGE_COUNT - 1;
    }
    if (PadData[0].repeat & PAD_DOWN)
        page_2 = (page_2 + 1) % ITEM_BOX_PAGE_COUNT;
    if (PadData[0].pressed & PAD_CROSS)
        debmode = UWAMONO_DEBUG_MENU;
}

void DispDebugUwamono(void)
{
    int screen[3];
    Vector4 offset;
    int i;

    offset.x = 0.0f;
    offset.y = 2.0f;
    offset.w = 1.0f;
    offset.z = 0.0f;
    for (i = 0; i < MAP_UNIT_COUNT; i++) {
        UwamonoDebugUnit *unit = &MapUnit[i];
        UwamonoInfo *info = &unit->info;
        int screen_col;
        int x;
        int y;
        int money;
        int count;
        int category;
        int index;

        if (!(unit->flags & UWAMONO_UNIT_LOADED))
            continue;
        if (unit->matrix == 0)
            continue;
        if (unit->serial == -1)
            continue;
        xglRotTransPers(screen, unit->matrix, &offset, 0);
        if (screen[0] <= UWAMONO_SCREEN_X_MIN || screen[0] >= UWAMONO_SCREEN_X_MAX)
            continue;
        if (screen[1] < UWAMONO_SCREEN_Y_MIN)
            continue;
        if (screen[1] > UWAMONO_SCREEN_Y_MAX)
            continue;
        if (screen[2] <= 0)
            continue;
        screen_col = ((screen[0] >> 4) - UWAMONO_SCREEN_ORIGIN_X) >> 1;
        y = ((screen[1] >> 4) - UWAMONO_SCREEN_ORIGIN_Y) >> 1;
        y += 32;
        if (unit->serial < UWAMONO_SERIAL_MODEL_BASE) {
            x = screen_col - 32;
            /* JP: パーツ%4d
             * EN: Part %4d */
            xglFontDebugPrintf(x, y, "\015\000\245\321\241\274\245\304%4d", unit->serial);
        } else {
            x = screen_col - 32;
            /* JP: モデル0x%4x
             * EN: Model 0x%4x */
            xglFontDebugPrintf(x, y, "\015\000\245\342\245\307\245\3530x%4x", unit->serial);
        }
        switch (info->kind) {
        case UWAMONO_DOOR:
            /* JP: ドア
             * EN: Door */
            xglFontDebugPrintf(x, y + 12, "\015\000\245\311\245\242");
            break;
        case UWAMONO_BREAKABLE:
            /* JP: 壊れ物
             * EN: Breakable object */
            xglFontDebugPrintf(x, y + 12, "\015\000\262\365\244\354\312\252");
            if (info->state_flags & UWAMONO_SELF_BREAK)
                /* JP: 自壊 ○
                 * EN: Self-destruction: Yes */
                xglFontDebugPrintf(x, y + 24, "\015\000\274\253\262\365 \241\373");
            else
                /* JP: 自壊 ×
                 * EN: Self-destruction: No */
                xglFontDebugPrintf(x, y + 24, "\015\000\274\253\262\365 \241\337");
            if (info->state_flags & UWAMONO_ENEMY_BREAK)
                /* JP: 敵壊 ○
                 * EN: Enemy destruction: Yes */
                xglFontDebugPrintf(x, y + 36, "\015\000\305\250\262\365 \241\373");
            else
                /* JP: 敵壊 ×
                 * EN: Enemy destruction: No */
                xglFontDebugPrintf(x, y + 36, "\015\000\305\250\262\365 \241\337");
            break;
        case UWAMONO_COLLISION:
            if (unit->serial == MAP_UNIT_COLLISION) {
                /* JP: コリジョン
                 * EN: Collision */
                xglFontDebugPrintf(x, y + 12, "\015\000\245\263\245\352\245\270\245\347\245\363");
            } else if (unit->serial == MAP_UNIT_SOUND) {
                /* JP: 音 0x%8x
                 * EN: Sound 0x%8x */
                xglFontDebugPrintf(x, y + 12, "\015\000\262\273 0x%8x", info->sound);
                break;
            } else {
                /* JP: トラップ
                 * EN: Trap */
                xglFontDebugPrintf(x, y + 12, "\015\000\245\310\245\351\245\303\245\327");
            }
            if (info->state_flags & UWAMONO_SELF_BREAK)
                /* JP: 自壊 ○
                 * EN: Self-destruction: Yes */
                xglFontDebugPrintf(x, y + 24, "\015\000\274\253\262\365 \241\373");
            else
                /* JP: 自壊 ×
                 * EN: Self-destruction: No */
                xglFontDebugPrintf(x, y + 24, "\015\000\274\253\262\365 \241\337");
            if (info->state_flags & UWAMONO_ENEMY_BREAK)
                /* JP: 敵壊 ○
                 * EN: Enemy destruction: Yes */
                xglFontDebugPrintf(x, y + 36, "\015\000\305\250\262\365 \241\373");
            else
                /* JP: 敵壊 ×
                 * EN: Enemy destruction: No */
                xglFontDebugPrintf(x, y + 36, "\015\000\305\250\262\365 \241\337");
            break;
        case UWAMONO_TREASURE:
            xglFontDebugPrintf(x, y + 12, treasureChestText);
            {
                int item = info->item;

                money = ItemBoxTbl[item].money;
                count = ItemBoxTbl[item].count;
                index = ItemBoxTbl[item].index;
                category = ItemBoxTbl[item].category;
            }
            if (money != 0)
                /* JP: お金 %6d G
                 * EN: Money %6d G */
                xglFontDebugPrintf(x, y + 24, "\015\000\244\252\266\342 %6d G", money);
            else
                /* JP: %s %3d個
                 * EN: %s %3d items */
                xglFontDebugPrintf(x, y + 24, "\015\000%s %3d\270\304",
                                   GetItemName(category, index), count);
            break;
        case UWAMONO_ITEM:
            /* JP: アイテム
             * EN: Item */
            xglFontDebugPrintf(x, y + 12, "\015\000\245\242\245\244\245\306\245\340");
            {
                int item = info->item;

                money = ItemBoxTbl[item].money;
                count = ItemBoxTbl[item].count;
                index = ItemBoxTbl[item].index;
                category = ItemBoxTbl[item].category;
            }
            if (money != 0)
                /* JP: お金 %6dG
                 * EN: Money %6dG */
                xglFontDebugPrintf(x, y + 24, "\015\000\244\252\266\342 %6dG", money);
            else
                /* JP: %s %3d個
                 * EN: %s %3d items */
                xglFontDebugPrintf(x, y + 24, "\015\000%s %3d\270\304",
                                   GetItemName(category, index), count);
            break;
        case UWAMONO_SAVE:
            /* JP: セーブ
             * EN: Save */
            xglFontDebugPrintf(x, y + 12, "\015\000\245\273\241\274\245\326");
            break;
        case UWAMONO_SHOP:
            /* JP: ショップ
             * EN: Shop */
            xglFontDebugPrintf(x, y + 12, "\015\000\245\267\245\347\245\303\245\327");
            break;
        }
        if (unit->serial != MAP_UNIT_SOUND && info->sound > 0)
            /* JP: 音 0x%8x
             * EN: Sound 0x%8x */
            xglFontDebugPrintf(x, y + 48, "\015\000\262\273 0x%8x", info->sound);
    }
}

void DispCollUwamono(void)
{
    DoublewordVector4 bottom[4];
    DoublewordVector4 top[4];
    AlignedVector4 position;
    AlignedVector4 size;
    int colors[8];
    float position_w;
    float corner_w;
    int i;

    colors[0] = 0;
    colors[1] = 0;
    colors[2] = 255;
    colors[3] = 128;
    colors[4] = 255;
    colors[5] = 0;
    colors[6] = 0;
    colors[7] = 128;
    position_w = COLL_POSITION_W;
    corner_w = COLL_CORNER_W;
    for (i = 0; i < MAP_UNIT_COUNT; i++) {
        UwamonoInfo *info = &MapUnit[i].info;
        UwamonoDebugUnit *unit;
        float (*rotation)[4];

        if (MapUnit[i].serial == -1)
            continue;
        if (MapUnit[i].flags & UWAMONO_UNIT_HIDDEN)
            continue;
        if (MapUnit[i].serial == MAP_UNIT_SOUND) {
            unit = &MapUnit[i];
            position = unit->position;
            if (info->bounds_shown == 0) {
                position.w = position_w;
            } else {
                rotation = unit->matrix;
                bottom[0].vector.x = position.x - rotation[0][0];
                bottom[0].vector.y = position.y - rotation[0][1];
                bottom[0].vector.z = position.z - rotation[0][2];
                bottom[0].vector.w = 0.0f;
                bottom[1].vector.x = position.x + rotation[0][0];
                bottom[1].vector.y = position.y + rotation[0][1];
                bottom[1].vector.z = position.z + rotation[0][2];
                bottom[1].vector.w = 0.0f;
            }
        } else if (info->shape != 0 && (MapUnit[i].flags & UWAMONO_UNIT_LOADED)) {
            unit = &MapUnit[i];
            rotation = unit->matrix;
            position = unit->position;
            size = unit->size;
            if (info->shape == UWAMONO_SHAPE_BOX) {
                bottom[0].vector.x = position.x + (size.x * rotation[0][0] + size.z * rotation[2][0]);
                bottom[0].vector.y = position.y;
                bottom[0].vector.z = position.z + (size.x * rotation[0][2] + size.z * rotation[2][2]);
                bottom[1].vector.x = position.x - (size.z * rotation[0][2] + size.x * rotation[2][2]);
                bottom[1].vector.y = position.y;
                bottom[1].vector.z = position.z + (size.z * rotation[0][0] + size.x * rotation[2][0]);
                bottom[2].vector.x = position.x - (size.x * rotation[0][0] + size.z * rotation[2][0]);
                bottom[2].vector.y = position.y;
                bottom[2].vector.z = position.z - (size.x * rotation[0][2] + size.z * rotation[2][2]);
                bottom[3].vector.x = position.x + (size.z * rotation[0][2] + size.x * rotation[2][2]);
                bottom[3].vector.y = position.y;
                bottom[3].vector.z = position.z - (size.z * rotation[0][0] + size.x * rotation[2][0]);
                bottom[0].vector.w = corner_w;
                bottom[1].vector.w = corner_w;
                bottom[2].vector.w = corner_w;
                bottom[3].vector.w = corner_w;
                top[0] = bottom[0];
                top[1] = bottom[1];
                top[2] = bottom[2];
                top[3] = bottom[3];
                top[0].vector.y += size.y;
                top[1].vector.y += size.y;
                top[2].vector.y += size.y;
                top[3].vector.y += size.y;
            } else if (info->shape == UWAMONO_SHAPE_CIRCLE) {
                DispPillar(&unit->position, &unit->size, colors);
            }
            if (info->sound > 0) {
                unit = &MapUnit[i];
                position = unit->position;
                position.w = position_w;
            }
        }
    }
}

/* DispCollUwamono supplies its eight color words in a2; this helper does
 * not consume them in the original implementation. */
void DispPillar(Vector4 *base, Vector4 *step, const int *colors)
{
    DoublewordVector4 corners[4];
    int segment = 0;
    float angle;

    base->w = 1.0f;
    do {
        /* Compute four local corners for each angular segment. */
        corners[0].vector = *base;
        corners[1].vector = *base;
        angle = PILLAR_SEGMENT_ANGLE;
        angle *= (float)segment;
        corners[0].vector.x += xglSin(angle) * step->x;
        corners[0].vector.z += xglCos(angle) * step->x;
        angle = PILLAR_CORNER_ANGLE;
        angle *= (float)(segment + 1);
        corners[1].vector.x += xglSin(angle) * step->x;
        corners[1].vector.z += xglCos(angle) * step->x;
        corners[2] = corners[0];
        corners[3] = corners[1];
        corners[2].vector.y += step->y;
        corners[3].vector.y += step->y;
        segment++;
    } while (segment < 17);
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
/* JP: 宝箱
 * EN: Treasure chest */
char treasureChestText[8] = "\015\000\312\365\310\242";
