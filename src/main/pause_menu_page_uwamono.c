#include "common.h"

#include "shared.h"

float D_004D7F88;

float D_004D7F8C;

extern float xglSin(float angle);

extern float xglCos(float angle);

float D_004D7F80;

float D_004D7F84;

typedef struct ItemBoxEntry {
    signed char category;
    unsigned char index;
    signed char count;
    signed char chance;
    int money;
} ItemBoxEntry;

/* The doubleword view gives the vector the 8-byte alignment the original
 * copies rely on (ld/sd instead of unaligned ldl/ldr pairs). */
typedef union UwamonoVector {
    Vector4 vector;
    u64 words[2];
} UwamonoVector;

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
    UwamonoVector position;
    unsigned char unmodeled_20[0x60];
    float (*matrix)[4];
    unsigned char unmodeled_84[0x20];
    short serial;
    unsigned char unmodeled_a6[0x0a];
    UwamonoVector size;
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

#define MAP_UNIT_COLLISION 0x7000

#define MAP_UNIT_SOUND 0x7012

#define UWAMONO_SELF_BREAK 0x1

#define UWAMONO_ENEMY_BREAK 0x2

extern signed char dispflg;

extern signed char lineflg;

extern signed char printflg;

extern signed char collflg;

/* PadData's +0x28/+0x2a/+0x2c fields match the local input view in input.c. */

extern UwamonoPadEntry PadData[2];

extern unsigned char EvtItemTbl[255 * 0x80];

extern ItemBoxEntry ItemBoxTbl[601];

extern UwamonoGameStatePrefix GameLoopState;

extern UwamonoDebugUnit MapUnit[64];

extern const char D_004CA680[];

char D_004DB798[];

const char D_004DB7A0[];

char D_004DB7D8[];

extern void UnlockMapUnit(void);

extern char *GetItemName(int category, int index);

extern int HexToStr(int value, char *buffer);

extern int xglFlagsGet1(int flag);

extern void xglFontDebugPrintf(int x, int y, const char *format, ...);

extern void *memset(void *s, int c, unsigned int n);

extern void xglRotTransPers(int *screen, float (*matrix)[4], Vector4 *point, int mode);

extern void DispPillar(Vector4 *base, Vector4 *step, const int *colors);

void DebDispEvtItem(void);

void DebDispItemBox(void);

static int debmode;

static int page_2;

const char D_004CA698[];

const char D_004CA6B0[];

const char D_004CA6C8[];

const char D_004CA6E0[];

const char D_004CA6F8[];

const char D_004CA710[];

const char D_004CA728[];

const char D_004CA740[];

const char D_004CA758[];

const char D_004CA770[];

const char D_004CA788[];

const char D_004CA7A0[];

const char D_004CA7E8[];

const char D_004CA7F8[];

void PauseMenuPageUwamono(void)
{
    static int cursol = 0;
    if (debmode == 1) {
        DebDispEvtItem();
        return;
    }
    if (debmode == 2) {
        DebDispItemBox();
        return;
    }
    xglFontDebugPrintf(16, 16, D_004CA680);
    xglFontDebugPrintf(48, cursol * 16 + 32, D_004DB798);
    if (lineflg != 0) xglFontDebugPrintf(64, 32, D_004CA698);
    else xglFontDebugPrintf(64, 32, D_004CA6B0);
    if (dispflg != 0) xglFontDebugPrintf(64, 48, D_004CA6C8);
    else xglFontDebugPrintf(64, 48, D_004CA6E0);
    if (printflg != 0) xglFontDebugPrintf(64, 64, D_004CA6F8);
    else xglFontDebugPrintf(64, 64, D_004CA710);
    if (collflg != 0) xglFontDebugPrintf(64, 80, D_004CA728);
    else xglFontDebugPrintf(64, 80, D_004CA740);
    if (GameLoopState.flags & 1) xglFontDebugPrintf(64, 96, D_004CA758);
    else xglFontDebugPrintf(64, 96, D_004CA770);
    xglFontDebugPrintf(64, 112, D_004CA788);
    xglFontDebugPrintf(64, 128, D_004CA7A0);
    if (PadData[0].pressed & 0x1000) {
        cursol--;
        if (cursol < 0) cursol = 6;
    }
    if (PadData[0].pressed & 0x4000) {
        cursol++;
        cursol %= 7;
    }
    if ((cursol == 5) && (PadData[0].pressed & 0x20)) debmode = 1;
    if ((cursol == 6) && (PadData[0].pressed & 0x20)) debmode = 2;
    if (PadData[0].pressed & 0x2000) {
        switch (cursol) {
        case 0: lineflg = 1; break;
        case 1: dispflg = 1; break;
        case 2: printflg = 1; break;
        case 3: collflg = 1; break;
        case 4: GameLoopState.flags |= 1; break;
        }
    }
    if (PadData[0].pressed & 0x8000) {
        switch (cursol) {
        case 0: lineflg = 0; return;
        case 1: dispflg = 0; return;
        case 2: printflg = 0; return;
        case 3: collflg = 0; return;
        case 4:
            UnlockMapUnit();
            GameLoopState.flags &= ~1u;
            break;
        }
    }
}

void DebDispEvtItem(void)
{
    static int page = 0;
    int row;
    int item_index;
    unsigned char *event_item;

    xglFontDebugPrintf(16, 16, D_004CA788);
    xglFontDebugPrintf(16, 32, D_004CA7E8);
    for (row = 0; row < 4; row++) {
        item_index = row + page * 4;
        if (item_index >= 255) {
            break;
        }
        event_item = &EvtItemTbl[item_index * 0x80];
        xglFontDebugPrintf(4, 60 + row * 34, D_004CA7F8,
                           item_index + 1, event_item, (signed char)event_item[0]);
        while (*event_item++ != 0) {
        }
        xglFontDebugPrintf(16, 72 + row * 34, D_004DB7A0, event_item);
    }
    if (PadData[0].repeat & 0x1000) {
        page--;
        if (page < 0) {
            page = 63;
        }
    }
    if (PadData[0].repeat & 0x4000) {
        page = (page + 1) % 64;
    }
    debmode = (PadData[0].pressed & 0x40) ? 0 : debmode;
}

void DebDispItemBox(void)
{
    int row;
    int y = 60;

    xglFontDebugPrintf(16, 16, D_004CA7A0);
    xglFontDebugPrintf(16, 32, D_004CA7E8);
    for (row = 0; row < 8; row++) {
        char amount_text[16];
        char chance_text[16];
        int id;
        int category;
        int money;
        int count;
        int chance;
        char *name;

        memset(amount_text, 0, sizeof(amount_text));
        memset(chance_text, 0, sizeof(chance_text));
        id = row + page_2 * 8;
        if (id > 600)
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
            xglFontDebugPrintf(128, y, "%s\270\304", amount_text);
        } else {
            HexToStr(money, amount_text);
            xglFontDebugPrintf(4, y, "%3d \244\252\266\342 %s\243\307", id, amount_text);
        }
        if (xglFlagsGet1(id + 0x79EC7) == 1)
            xglFontDebugPrintf(152, y, "\274\350\306\300\272\321");
        if (chance != 0) {
            HexToStr(chance, chance_text);
            xglFontDebugPrintf(186, y, "\263\316\316\250%s", chance_text);
        }
        y += 16;
    }
    if (PadData[0].repeat & 0x1000) {
        page_2--;
        if (page_2 < 0)
            page_2 = 75;
    }
    if (PadData[0].repeat & 0x4000)
        page_2 = (page_2 + 1) % 76;
    if (PadData[0].pressed & 0x40)
        debmode = 0;
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
    for (i = 0; i < 64; i++) {
        UwamonoDebugUnit *unit = &MapUnit[i];
        UwamonoInfo *info = &unit->info;
        int screen_col;
        int x;
        int y;
        int money;
        int count;
        int category;
        int index;

        if (!(unit->flags & 0x10000))
            continue;
        if (unit->matrix == 0)
            continue;
        if (unit->serial == -1)
            continue;
        xglRotTransPers(screen, unit->matrix, &offset, 0);
        if (screen[0] <= 0x7000 || screen[0] >= 0x9000)
            continue;
        if (screen[1] < 0x6E01)
            continue;
        if (screen[1] > 0x8DFF)
            continue;
        if (screen[2] <= 0)
            continue;
        screen_col = ((screen[0] >> 4) - 0x700) >> 1;
        y = ((screen[1] >> 4) - 0x720) >> 1;
        y += 32;
        if (unit->serial < 0x1000) {
            x = screen_col - 32;
            xglFontDebugPrintf(x, y, "\015\000\245\321\241\274\245\304%4d", unit->serial);
        } else {
            x = screen_col - 32;
            xglFontDebugPrintf(x, y, "\015\000\245\342\245\307\245\3530x%4x", unit->serial);
        }
        switch (info->kind) {
        case UWAMONO_DOOR:
            xglFontDebugPrintf(x, y + 12, "\015\000\245\311\245\242");
            break;
        case UWAMONO_BREAKABLE:
            xglFontDebugPrintf(x, y + 12, "\015\000\262\365\244\354\312\252");
            if (info->state_flags & UWAMONO_SELF_BREAK)
                xglFontDebugPrintf(x, y + 24, "\015\000\274\253\262\365 \241\373");
            else
                xglFontDebugPrintf(x, y + 24, "\015\000\274\253\262\365 \241\337");
            if (info->state_flags & UWAMONO_ENEMY_BREAK)
                xglFontDebugPrintf(x, y + 36, "\015\000\305\250\262\365 \241\373");
            else
                xglFontDebugPrintf(x, y + 36, "\015\000\305\250\262\365 \241\337");
            break;
        case UWAMONO_COLLISION:
            if (unit->serial == MAP_UNIT_COLLISION) {
                xglFontDebugPrintf(x, y + 12, "\015\000\245\263\245\352\245\270\245\347\245\363");
            } else if (unit->serial == MAP_UNIT_SOUND) {
                xglFontDebugPrintf(x, y + 12, "\015\000\262\273 0x%8x", info->sound);
                break;
            } else {
                xglFontDebugPrintf(x, y + 12, "\015\000\245\310\245\351\245\303\245\327");
            }
            if (info->state_flags & UWAMONO_SELF_BREAK)
                xglFontDebugPrintf(x, y + 24, "\015\000\274\253\262\365 \241\373");
            else
                xglFontDebugPrintf(x, y + 24, "\015\000\274\253\262\365 \241\337");
            if (info->state_flags & UWAMONO_ENEMY_BREAK)
                xglFontDebugPrintf(x, y + 36, "\015\000\305\250\262\365 \241\373");
            else
                xglFontDebugPrintf(x, y + 36, "\015\000\305\250\262\365 \241\337");
            break;
        case UWAMONO_TREASURE:
            xglFontDebugPrintf(x, y + 12, D_004DB7D8);
            {
                int item = info->item;

                money = ItemBoxTbl[item].money;
                count = ItemBoxTbl[item].count;
                index = ItemBoxTbl[item].index;
                category = ItemBoxTbl[item].category;
            }
            if (money != 0)
                xglFontDebugPrintf(x, y + 24, "\015\000\244\252\266\342 %6d G", money);
            else
                xglFontDebugPrintf(x, y + 24, "\015\000%s %3d\270\304",
                                   GetItemName(category, index), count);
            break;
        case UWAMONO_ITEM:
            xglFontDebugPrintf(x, y + 12, "\015\000\245\242\245\244\245\306\245\340");
            {
                int item = info->item;

                money = ItemBoxTbl[item].money;
                count = ItemBoxTbl[item].count;
                index = ItemBoxTbl[item].index;
                category = ItemBoxTbl[item].category;
            }
            if (money != 0)
                xglFontDebugPrintf(x, y + 24, "\015\000\244\252\266\342 %6dG", money);
            else
                xglFontDebugPrintf(x, y + 24, "\015\000%s %3d\270\304",
                                   GetItemName(category, index), count);
            break;
        case UWAMONO_SAVE:
            xglFontDebugPrintf(x, y + 12, "\015\000\245\273\241\274\245\326");
            break;
        case UWAMONO_SHOP:
            xglFontDebugPrintf(x, y + 12, "\015\000\245\267\245\347\245\303\245\327");
            break;
        }
        if (unit->serial != MAP_UNIT_SOUND && info->sound > 0)
            xglFontDebugPrintf(x, y + 48, "\015\000\262\273 0x%8x", info->sound);
    }
}

void DispCollUwamono(void)
{
    UwamonoVector bottom[4];
    UwamonoVector top[4];
    UwamonoVector position;
    UwamonoVector size;
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
    position_w = D_004D7F80;
    corner_w = D_004D7F84;
    for (i = 0; i < 64; i++) {
        UwamonoInfo *info = &MapUnit[i].info;
        UwamonoDebugUnit *unit;
        float (*rotation)[4];

        if (MapUnit[i].serial == -1)
            continue;
        if (MapUnit[i].flags & 0x100000)
            continue;
        if (MapUnit[i].serial == MAP_UNIT_SOUND) {
            unit = &MapUnit[i];
            position = unit->position;
            if (info->bounds_shown == 0) {
                position.vector.w = position_w;
            } else {
                rotation = unit->matrix;
                bottom[0].vector.x = position.vector.x - rotation[0][0];
                bottom[0].vector.y = position.vector.y - rotation[0][1];
                bottom[0].vector.z = position.vector.z - rotation[0][2];
                bottom[0].vector.w = 0.0f;
                bottom[1].vector.x = position.vector.x + rotation[0][0];
                bottom[1].vector.y = position.vector.y + rotation[0][1];
                bottom[1].vector.z = position.vector.z + rotation[0][2];
                bottom[1].vector.w = 0.0f;
            }
        } else if (info->shape != 0 && (MapUnit[i].flags & 0x10000)) {
            unit = &MapUnit[i];
            rotation = unit->matrix;
            position = unit->position;
            size = unit->size;
            if (info->shape == 2) {
                bottom[0].vector.x = position.vector.x + (size.vector.x * rotation[0][0] + size.vector.z * rotation[2][0]);
                bottom[0].vector.y = position.vector.y;
                bottom[0].vector.z = position.vector.z + (size.vector.x * rotation[0][2] + size.vector.z * rotation[2][2]);
                bottom[1].vector.x = position.vector.x - (size.vector.z * rotation[0][2] + size.vector.x * rotation[2][2]);
                bottom[1].vector.y = position.vector.y;
                bottom[1].vector.z = position.vector.z + (size.vector.z * rotation[0][0] + size.vector.x * rotation[2][0]);
                bottom[2].vector.x = position.vector.x - (size.vector.x * rotation[0][0] + size.vector.z * rotation[2][0]);
                bottom[2].vector.y = position.vector.y;
                bottom[2].vector.z = position.vector.z - (size.vector.x * rotation[0][2] + size.vector.z * rotation[2][2]);
                bottom[3].vector.x = position.vector.x + (size.vector.z * rotation[0][2] + size.vector.x * rotation[2][2]);
                bottom[3].vector.y = position.vector.y;
                bottom[3].vector.z = position.vector.z - (size.vector.z * rotation[0][0] + size.vector.x * rotation[2][0]);
                bottom[0].vector.w = corner_w;
                bottom[1].vector.w = corner_w;
                bottom[2].vector.w = corner_w;
                bottom[3].vector.w = corner_w;
                top[0] = bottom[0];
                top[1] = bottom[1];
                top[2] = bottom[2];
                top[3] = bottom[3];
                top[0].vector.y += size.vector.y;
                top[1].vector.y += size.vector.y;
                top[2].vector.y += size.vector.y;
                top[3].vector.y += size.vector.y;
            } else if (info->shape == 1) {
                DispPillar(&unit->position.vector, &unit->size.vector, colors);
            }
            if (info->sound > 0) {
                unit = &MapUnit[i];
                position = unit->position;
                position.vector.w = position_w;
            }
        }
    }
}

/* DispCollUwamono supplies its eight color words in a2; this helper does
 * not consume them in the original implementation. */
void DispPillar(Vector4 *base, Vector4 *step, const int *colors)
{
    union PillarVertex {
        Vector4 vector;
        u64 words[2];
    } corners[4];
    int segment = 0;
    float angle;
    float angle_increment;

    base->w = 1.0f;
    do {
        /* Compute four local corners for each angular segment. */
        corners[0].vector = *base;
        corners[1].vector = *base;
        angle_increment = D_004D7F88;
        angle = (float)segment * angle_increment;
        corners[0].vector.x += xglSin(angle) * step->x;
        angle_increment = D_004D7F8C;
        corners[0].vector.z += xglCos(angle) * step->x;
        angle = (float)(segment + 1) * angle_increment;
        corners[1].vector.x += xglSin(angle) * step->x;
        corners[1].vector.z += xglCos(angle) * step->x;
        corners[2] = corners[0];
        corners[3] = corners[1];
        corners[2].vector.y += step->y;
        corners[3].vector.y += step->y;
        segment++;
    } while (segment < 17);
}

float D_004D7F88 = 0.3926991f;
float D_004D7F8C = 0.3926991f;
float D_004D7F80 = 0.3f;
float D_004D7F84 = 0.1f;
char D_004DB798[4] = "\xA1\xE4";
const char D_004DB7A0[4] = "%s";
char D_004DB7D8[8] = "\015\000\312\365\310\242";
static int debmode = 0;
static int page_2 = 0;
const char D_004CA698[24] = "\xBC\xCD\xB7\xE2\xB5\xB0\xC0\xD7\xC9\xBD\xBC\xA8\xA1\xA1\xA1\xA1\xA5\xAA\xA5\xF3";
const char D_004CA6B0[24] = "\xBC\xCD\xB7\xE2\xB5\xB0\xC0\xD7\xC9\xBD\xBC\xA8\xA1\xA1\xA1\xA1\xA5\xAA\xA5\xD5";
const char D_004CA6C8[24] = "\xBE\xE5\xCA\xAA\xBE\xF0\xCA\xF3\xC9\xBD\xBC\xA8\xA1\xA1\xA1\xA1\xA5\xAA\xA5\xF3";
const char D_004CA6E0[24] = "\xBE\xE5\xCA\xAA\xBE\xF0\xCA\xF3\xC9\xBD\xBC\xA8\xA1\xA1\xA1\xA1\xA5\xAA\xA5\xD5";
const char D_004CA6F8[24] = "\xBE\xE5\xCA\xAA\xBE\xF0\xCA\xF3\xBD\xD0\xCE\xCF\xA1\xA1\xA1\xA1\xA5\xAA\xA5\xF3";
const char D_004CA710[24] = "\xBE\xE5\xCA\xAA\xBE\xF0\xCA\xF3\xBD\xD0\xCE\xCF\xA1\xA1\xA1\xA1\xA5\xAA\xA5\xD5";
const char D_004CA728[24] = "\xA5\xB3\xA5\xEA\xA5\xB8\xA5\xE7\xA5\xF3\xC9\xBD\xBC\xA8\xA1\xA1\xA5\xAA\xA5\xF3";
const char D_004CA740[24] = "\xA5\xB3\xA5\xEA\xA5\xB8\xA5\xE7\xA5\xF3\xC9\xBD\xBC\xA8\xA1\xA1\xA5\xAA\xA5\xD5";
const char D_004CA758[24] = "\xBC\xCD\xB7\xE2\xA5\xD5\xA5\xE9\xA5\xB0\xA1\xA1\xA1\xA1\xA1\xA1\xA5\xAA\xA5\xF3";
const char D_004CA770[24] = "\xBC\xCD\xB7\xE2\xA5\xD5\xA5\xE9\xA5\xB0\xA1\xA1\xA1\xA1\xA1\xA1\xA5\xAA\xA5\xD5";
const char D_004CA788[24] = "\xA5\xA4\xA5\xD9\xA5\xF3\xA5\xC8\xA5\xA2\xA5\xA4\xA5\xC6\xA5\xE0\xB0\xEC\xCD\xF7";
const char D_004CA7A0[16] = "\xCA\xF5\xC8\xA2\xC3\xE6\xBF\xC8\xB0\xEC\xCD\xF7";
const char D_004CA7E8[16] = "\xA1\xDF\xA5\xDC\xA5\xBF\xA5\xF3\xA4\xC7\xCC\xE1\xA4\xEB";
const char D_004CA7F8[16] = "%3d %s %c";


