/*
 * OV01 original TU 9: 0x00a26018..0x00a2c3a0 (91 functions)
 */
#include "common.h"
#include "ov01/actor.h"
#include "ov01/data_eth_name_get.h"
#include "shared.h"
#include "ov01/battle_init.h"
#include "ov01/calc.h"
#include "main/xgl_packet.h"
#include "ov01/gr_gp_init.h"
#include "ov01/menu.h"

/* The pad's direction bits: PAD_UP | PAD_RIGHT | PAD_DOWN | PAD_LEFT. */
#define PAD_DPAD_MASK 0xF000

typedef struct MenuEtherStatusEntry {
    unsigned char unmodeled_00[0xE];
    short etherStatus;
} MenuEtherStatusEntry;

/* dataEthGet's record as menuOpenEth reads it: the technique id at +6, passed
 * to dataTecGet. Same layout as CalcEtherStatus in src/ov01/calc.h. */
typedef struct MenuEtherRecord {
    unsigned char unmodeled_00[6];
    short tecId; /* +6 */
} MenuEtherRecord;

extern const signed char menu0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzText[];
extern const signed char menuJpText4[];
void menuStatSprite(GrPacket *packet, int x, int y, int width, int height, int srcX, int srcY);
extern int rnd(int max);

/*
 * statTbl: an array of pointers, indexed by charaId, each pointing at a
 * 0-terminated run of 0xC-byte entries. menuStatIconChk reads each entry's
 * flags word as a bitmask tested against the statFlags mask argument.
 */
typedef struct StatIconEntry {
    int flags; /* +0x00 */
    int srcX;  /* +0x04 */
    int srcY;  /* +0x08 */
} StatIconEntry;
static StatIconEntry *statTbl[8];

typedef struct MenuRGBA {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} MenuRGBA;

/* The sprite colour: menuStatSpriteRGB copies a caller's MenuRGBA into it
 * byte-wise, while menuStatSprite/menuStatSprite2 copy the whole word from an
 * aligned local, so the field is word-aligned. */
typedef union MenuColor {
    unsigned int word;
    MenuRGBA rgba;
} MenuColor;

/* grPutSpr's sprite record: two corner vertices, a colour and the texel rectangle. */
typedef struct GrSprite {
    float x0, y0, z0, w0; /* +0x00 */
    float x1, y1, z1, w1; /* +0x10 */
    MenuColor color;      /* +0x20 */
    short u0;             /* +0x24 */
    short u1;             /* +0x26 */
    short v0;             /* +0x28 */
    short v1;             /* +0x2A */
} GrSprite;

#define SPRITE_Z 16777212.0f

extern void grPutSpr(GrPacket *packet, GrSprite *sprite, int flags);
/* Defined by ov01/tu008 (src/ov01/gr_gp_init.c); the owner keeps them TU-local. */
extern void grOpenSpr(GrPacket *packet);
extern void grCloseSpr(void);
void menuStatSpriteRGB(GrPacket *packet, int x, int y, int width, int height, int srcX, int srcY, const MenuRGBA *rgba);
/* The shared sprite colour, the word 0x80808080. */
extern const MenuRGBA menuNeutralGray;

void menuATBEnvMake(void *packet);
void menuEventTimerDispSub(ObjectTask *task, GrPacket *packet);

/*
 * One entry of a battle window's command list (0xC bytes). The ether and item
 * result functions read the chosen entry's id.
 */
typedef struct MenuWinCommand {
    const char *name;                 /* +0x00: string-table entry (ether and item windows) */
    const char *help;                 /* +0x04: string-table entry */
    signed char disabled;             /* +0x08 */
    unsigned char amount;             /* +0x09: EP cost (ether window) or owned count (item window) */
    short id;                         /* +0x0A */
} MenuWinCommand;

/*
 * The ether/item window's parameter record: eBattleWinOpen's argument and
 * eBattleWinMain's (same layout as EndouBattleWindowParams). The result
 * functions load the command list, store the pad word and read the selection
 * (-1 for none).
 */
typedef struct MenuBattleWin {
    int x;                            /* +0x00 */
    int y;                            /* +0x04 */
    int color;                        /* +0x08 */
    int unk0C;                        /* +0x0C */
    int w;                            /* +0x10 */
    MenuWinCommand *commands;         /* +0x14 */
    const char *title;                /* +0x18 */
    int buttons;                      /* +0x1C */
    int selection;                    /* +0x20 */
    unsigned char unmodeled_24[4];
} MenuBattleWin;

/* Keep source-owned local BSS objects in their original address order;
 * intervening original-only storage remains C-owned between runs. */
static ObjectTask *pMenuBat;
static ObjectTask *pPreObj;
static ObjectTask *pAtbObj;
static ObjectTask *pKeyGuideObj;
static ObjectTask *pBoostObj;
static ObjectTask *pCritObj;
static ObjectTask *pStatObj[4];
static ObjectTask *pStockObj[5];
static float ETRotDist;
static int eventTimer;
static int eventTimerX;
static float RingRot;
static int mapHDist;
static int menuResBat;
static MenuBattleWin ethWin;
static MenuBattleWin itmWin;

void menuInitBat(void) {
    pMenuBat = 0;
}

/*
 * The battle menu's command tables, one 0x10-byte row per menu line, indexed
 * by menuSel. menuBatObjFree reads the command id at +0x04, which
 * menuCmdExecChk checks. chaTbl serves units without CALC_UNIT_FLAG_IS_AGWS,
 * agwsTbl units with it.
 */
typedef struct MenuBatCmdEntry {
    int iconRow;                      /* +0x00: menuBatObjDraw's icon row, times 0x14 */
    int command;                      /* +0x04 */
    int offsetX;                      /* +0x08: clamps the line's x offset (menuBatObjDraw) */
    int offsetY;                      /* +0x0C: clamps the line's y offset (menuBatObjDraw) */
} MenuBatCmdEntry;
static MenuBatCmdEntry chaTbl_00A3F038[5];
static MenuBatCmdEntry agwsTbl_00A3F088[5];

/* Work block of the battle menu task. */
typedef struct MenuBatWork {
    unsigned char unmodeled_00[0x80];
    BattleUnit *unit;                 /* +0x80: the unit the menu commands */
    unsigned char unmodeled_84[0x90 - 0x84];
    Vector4 pos;                      /* +0x90: the window's screen origin (menuOpenBat stores x = 8, z = 292, y = 0) */
    unsigned char unmodeled_A0[0xF0 - 0xA0];
    Vector4 splinePoints[2];          /* +0xF0: calcSpline's control points (menuBatObjDraw) */
} MenuBatWork;

/* The highlighted menu line (0..4, wrapping) and its animation counter. */
static int menuSel;
static int menuSelCnt;
void menuBatObj(ObjectTask *task);
void menuBatObjDraw(ObjectTask *task);
int sndSysSePlay(int soundId);
static Vector4 cPoint[4];
static int sSp[8];
void calcSplineInit(void *spline, Vector4 *points, int count, int duration);
extern ObjectTask *objEntry2(void *argument, void (*callback)(ObjectTask *task));
extern void objStdInit(ObjectTask *task);

/* Opens the battle command menu for unit. */
void menuOpenBat(BattleUnit *unit)
{
    ObjectTask *task;
    Vector4 *pos;
    MenuBatCmdEntry *table;
    float x;
    float y;

    menuResBat = 0;
    menuSel = 0;
    menuSelCnt = 0;
    task = objEntry2(menuBatObj, menuBatObjDraw);
    objStdInit(task);
    pos = &((MenuBatWork *)task->work)->pos;
    pos->x = 8.0f;
    pos->z = 292.0f;
    pos->y = 0.0f;
    if (calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS) {
        table = agwsTbl_00A3F088;
    } else {
        table = chaTbl_00A3F038;
    }
    table += 4;
    x = table->offsetX;
    y = table->offsetY;
    cPoint[0].x = 0.0f;
    cPoint[0].z = 0.0f;
    cPoint[1].x = 0.0f;
    cPoint[1].z = 0.0f;
    cPoint[2].x = x;
    cPoint[2].z = y;
    cPoint[3].x = x;
    cPoint[3].z = y;
    calcSplineInit(sSp, cPoint, 4, 4);
    pMenuBat = task;
    ((MenuBatWork *)task->work)->unit = unit;
    sndSysSePlay(7);
}


void objRemove(ObjectTask *task);
int sndSysSePlay(int soundId);

void menuCloseBat(void) {
    menuResBat = 0;
    objRemove(pMenuBat);
    pMenuBat = 0;
    sndSysSePlay(8);
}

int menuResultBat(void) {
    return menuResBat;
}

int menuOpenChkBat(void) {
    return pMenuBat != 0;
}

/* obj.c (ov01/tu001): returns the address of the command entry the task's
 * work block currently selects. */
void *objCmdPtrGet(ObjectTask *task);
void menuBatObjFree(ObjectTask *task);

/* Battle menu task callback: runs the pad handling in menuBatObjFree. */
void menuBatObj(ObjectTask *task) {
    void *work;

    objCmdPtrGet(task);
    menuBatObjFree(task);
    /* The original keeps the call and epilogue for this last call rather than a
     * tail jump: a post-call read blocks the compiler's sibcall lowering and
     * is then removed as dead code. */
    work = task->work;
}


extern int dataPadRead(int padNo);
int menuCmdExecChk(BattleUnit *unit, int command);

/*
 * Reads the pad for the battle menu: moves the highlighted line, plays the
 * cursor sound, and on a confirmed command sets menuResBat to its id.
 */
void menuBatObjFree(ObjectTask *task)
{
    BattleUnit *unit;
    MenuBatCmdEntry *table;

    unit = ((MenuBatWork *)task->work)->unit;
    if (calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS) {
        table = agwsTbl_00A3F088;
    } else {
        table = chaTbl_00A3F038;
    }
    if (dataPadRead(1) & PAD_DOWN) {
        /* A local, not a literal: the original keeps the 4 in a register for the comparison. */
        int lastLine = 4;

        menuSel++;
        if (menuSel > lastLine) {
            menuSel = 0;
        }
        menuSelCnt = 0;
        sndSysSePlay(SE_CURSOR);
    } else if (dataPadRead(1) & PAD_UP) {
        menuSel--;
        if (menuSel < 0) {
            menuSel = 4;
        }
        menuSelCnt = 0;
        sndSysSePlay(SE_CURSOR);
    }
    if (dataPadRead(1) & PAD_CIRCLE) {
        if (menuCmdExecChk(unit, table[menuSel].command) != 0) {
            menuResBat = table[menuSel].command;
            sndSysSePlay(SE_DECIDE);
        } else {
            sndSysSePlay(SE_BUZZER);
        }
        return;
    }
    if (dataPadRead(1) & PAD_CROSS) {
        sndSysSePlay(SE_CANCEL);
    }
    menuResBat = 0;
}

extern const MenuRGBA menuStartGray;
static int sSp[8];
int calcSpline(void *spline, Vector4 *out, int mode);
void menuTexTrans(void);
void menuEnvMake(void *packet);

/*
 * Draws the battle command window: five lines, with the highlighted one
 * stepping through its animation frames and then showing its command icon.
 */
void menuBatObjDraw(ObjectTask *task)
{
    GrPacket packet;
    MenuRGBA col;
    MenuBatCmdEntry *table;
    int *anchor; /* the current line's offsetY; its offsetX is anchor[-1] */
    float *pos;
    float *newVar;
    float fx;
    float fy;
    int i;
    int y;
    int x;
    BattleUnit *unit;
    int cx;
    int cy;
    int t;

    pos = &((MenuBatWork *)task->work)->pos.x;
    newVar = &pos[2];
    fx = pos[0];
    fy = *newVar;
    unit = ((MenuBatWork *)task->work)->unit;
    col = menuStartGray;
    x = (int)fx;
    y = (int)fy;
    if (calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS) {
        table = agwsTbl_00A3F088;
    } else {
        table = chaTbl_00A3F038;
    }
    grGpInit(&packet);
    menuTexTrans();
    menuEnvMake(&packet);
    grOpenSpr(&packet);
    /* Lines from the bottom (4) up to the top (0). */
    i = 4;
    anchor = &table[4].offsetY;
    do {
        cx = (int)pos[24];
        if (!(pos[24] < (float)anchor[-1])) {
            cx = anchor[-1];
        }
        cy = (int)pos[26];
        if (!(pos[26] < (float)anchor[0])) {
            cy = anchor[0];
        }
        if (menuSel == i) {
            menuStatSprite(&packet, x + cx, y + cy, 0x8E, 0x20, 0, menuSelCnt << 5);
            if (menuSelCnt == 3) {
                if (menuCmdExecChk(unit, table[menuSel].command) != 0) {
                    menuStatSprite(&packet, x + cx + 0x1B, y + cy + 7, 0x69, 0x14, 0x8F, table[menuSel].iconRow * 0x14);
                } else {
                    menuStatSpriteRGB(&packet, x + cx + 0x1B, y + cy + 7, 0x69, 0x14, 0x8F, table[menuSel].iconRow * 0x14, &col);
                }
            } else {
                t = menuSelCnt + 1;
                menuSelCnt = t;
                if (t >= 4) {
                    menuSelCnt = 3;
                }
            }
        } else {
            menuStatSprite(&packet, x + cx, y + cy, 0x8E, 0x20, 0, 0);
        }
        i -= 1;
        anchor -= 4;
    } while (i >= 0);
    grCloseSpr();
    calcSpline(sSp, (Vector4 *)(pos + 24), 2);
    grPacketSend(&packet);
}

extern int dataItmBoxChk(int id);
extern int curPosProcInit(BattleUnit *unit, int mode);
extern int cfEventGet(void);
extern int calcDblActChk(BattleUnit *unit);
int menuEtherNumChk(BattleUnit *unit);

/* Whether a battle menu command can run for this unit. */
int menuCmdExecChk(BattleUnit *unit, int command)
{
    int i;
    int n;
    int ret;
    short baseCharaId;

    switch (command) {
    case 1:
        if (calcStatGet(unit, 1, 0x1000) == 0) {
            if (menuEtherNumChk(unit) != 0) {
                ret = 1;
                break;
            }
        }
        ret = 0;
        break;
    case 2:
        n = 0;
        i = 1;
        do {
            n += dataItmBoxChk(i++);
        } while (i < 0x23);
        if (n != 0) {
            if (!(calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS)) {
                ret = 1;
                break;
            }
        }
        ret = 0;
        break;
    case 5:
        ret = curPosProcInit(unit, 6) != 0;
        break;
    case 4:
        if (cfEventGet() & 4) {
            goto zero;
        }
        if (!(calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS) && (calcUPGet(unit)->flags & 0x20)) {
            goto zero;
        }
        if ((calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS) && (calcUPGet(unit)->linkedUnit->flags & 0x20)) {
            goto zero;
        }
        if (calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS) {
            goto one;
        }
        baseCharaId = calcUPGet(unit)->baseCharaId;
        ret = 0;
        if (baseCharaId != 0) {
            goto one;
        }
        break;
    case 6:
        ret = calcDblActChk(unit) != 0;
        break;
    case 3:
    one:
        ret = 1;
        break;
    default:
    zero:
        ret = 0;
        break;
    }
    return ret;
}

/* Counts the unit's ether entries whose status is set. */
int menuEtherNumChk(BattleUnit *unit)
{
    int count;
    int slot;
    int offset;
    int countIfPresent;
    MenuEtherStatusEntry *etherEntry;

    count = 0;
    offset = 0x80;
    for (slot = 0xB; slot >= 0; slot--) {
        etherEntry = (MenuEtherStatusEntry *)((char *)calcUPGet(unit) + offset);
        countIfPresent = count + 1;
        offset += 2;
        count = (etherEntry->etherStatus != 0) ? countIfPresent : count;
    }
    return count;
}

/*
 * One line of the ether window: name and help are string-table words, the
 * window stores the disabled flag at +0x8, the EP cost at +0x9 and the id
 * (0 when the line is disabled) at +0xA.
 */
static MenuWinCommand ethBw[13];
extern const char menuEtherListText[];
MenuEtherRecord *dataEthGet(int id);
int calcTakeEpGet(BattleUnit *unit, int tecId);
int calcEthEnableChk(BattleUnit *unit, int id);
void eBattleWinOpen(void *window);

/* Fills the ether window's command list from the unit's ethers and opens the window. */
void menuOpenEth(BattleUnit *unit) {
    MenuWinCommand *e;
    short *p;
    int i;
    int n;
    int id;
    signed char dis;
    int ok;

    i = 0xB;
    n = 0;
    e = ethBw;
    p = calcUPGet(unit)->etherId;
    do {
        id = *p;
        p++;
        if (id != 0) {
            e->name = dataEthNameGet(id)[0];
            e->help = dataEthNameGet(id)[1];
            e->amount = calcTakeEpGet(unit, dataEthGet(id)->tecId);
            ok = calcEthEnableChk(unit, id);
            dis = 0;
            if (ok == 0) {
                dis = 1;
                id = 0;
            }
            e->id = id;
            n++;
            e->disabled = dis;
            e++;
        }
    } while (--i >= 0);
    ethBw[n].name = 0;
    ethWin.x = 0x10;
    ethWin.y = 0x6C;
    ethWin.color = 0xFFFFFD;
    ethWin.w = 0xF0;
    ethWin.title = menuEtherListText;
    ethWin.commands = ethBw;
    ethWin.unk0C = 0;
    eBattleWinOpen(&ethWin);
    sndSysSePlay(7);
}

void eBattleWinClose(void *window);

void menuCloseEth(void) {
    eBattleWinClose(&ethWin);
    sndSysSePlay(8);
}

void eBattleWinMain(void *window);

/*
 * The ether window's result: pad 2's direction bits (0xF000) replace pad 1's
 * before eBattleWinMain reads them. Stores the chosen ether's id (0 for an
 * unusable line, -1 for no selection) to *ethId and plays the decide, buzzer
 * or cancel sound. Returns the button word.
 */
int menuResultEth(int *ethId)
{
    int held;
    int buttons;

    held = dataPadRead(2) & PAD_DPAD_MASK;
    buttons = dataPadRead(1) & ~PAD_DPAD_MASK;
    buttons |= held;
    ethWin.buttons = buttons;
    eBattleWinMain(&ethWin);
    if (ethWin.selection == -1) {
        *ethId = ethWin.selection;
    } else {
        *ethId = ethWin.commands[ethWin.selection].id;
        if (buttons & PAD_CIRCLE) {
            if (*ethId != 0) {
                sndSysSePlay(SE_DECIDE);
            } else {
                sndSysSePlay(SE_BUZZER);
            }
        } else if (buttons & PAD_CROSS) {
            sndSysSePlay(SE_CANCEL);
        }
    }
    return buttons;
}

/*
 * One line of the item window: a MenuWinCommand, with the owned count (the
 * dataItmBoxChk result) at +0x9 in place of the EP cost.
 */
static MenuWinCommand itmBw[101];
extern const char menuItemsListText[];
const char **dataItmNameGet(int index);
int calcItmEnableChk(BattleUnit *unit, int id);

/* Fills the item window's command list from the owned items and opens the window. */
void menuOpenItm(BattleUnit *unit) {
    MenuWinCommand *e;
    int i;
    int n;
    int cnt;
    int id;
    signed char dis;
    int ok;

    i = 1;
    e = itmBw;
    n = 0;
    calcUPGet(unit);
    do {
        id = i;
        cnt = dataItmBoxChk(i);
        if (cnt > 0) {
            e->name = dataItmNameGet(i)[0];
            e->help = dataItmNameGet(i)[1];
            e->amount = cnt;
            ok = calcItmEnableChk(unit, i);
            dis = 0;
            if (ok == 0) {
                dis = 1;
                id = 0;
            }
            e->id = id;
            n++;
            e->disabled = dis;
            e++;
        }
        i++;
    } while (i < 0x24);
    itmBw[n].name = 0;
    itmWin.x = 0x10;
    itmWin.y = 0x6C;
    itmWin.color = 0xFFFFFD;
    itmWin.w = 0xF0;
    itmWin.title = menuItemsListText;
    itmWin.commands = itmBw;
    itmWin.unk0C = 0;
    eBattleWinOpen(&itmWin);
    sndSysSePlay(7);
}

void menuCloseItm(void) {
    eBattleWinClose(&itmWin);
    sndSysSePlay(8);
}

/* Item-window counterpart of menuResultEth: stores the chosen item id to *itmId. */
int menuResultItm(int *itmId)
{
    int held;
    int buttons;

    held = dataPadRead(2) & PAD_DPAD_MASK;
    buttons = dataPadRead(1) & ~PAD_DPAD_MASK;
    buttons |= held;
    itmWin.buttons = buttons;
    eBattleWinMain(&itmWin);
    if (itmWin.selection == -1) {
        *itmId = itmWin.selection;
    } else {
        *itmId = itmWin.commands[itmWin.selection].id;
        if (buttons & PAD_CIRCLE) {
            if (*itmId != 0) {
                sndSysSePlay(SE_DECIDE);
            } else {
                sndSysSePlay(SE_BUZZER);
            }
        } else if (buttons & PAD_CROSS) {
            sndSysSePlay(SE_CANCEL);
        }
    }
    return buttons;
}

/*
 * The +0x90 movement block of an object's work record: the current position
 * and the target objStdMove moves it toward. Shared by the stock faces
 * (MenuStockWork) and the critical indicator (MenuCritWork).
 */
typedef struct MenuObjTransform {
    Vector4 position;                  /* +0x00 */
    float ap;                          /* +0x10: the status window's displayed values */
    float hp;                          /* +0x14 */
    float ep;                          /* +0x18 */
    unsigned char unmodeled_1c[0x14];
    float bp;                          /* +0x30 */
    unsigned char unmodeled_34[0x2C];
    Vector4 target;                    /* +0x60: where the object moves to */
    unsigned char unmodeled_70[0x10];  /* +0x70: tail of the record, not accessed here */
} MenuObjTransform;

/*
 * The critical-indicator object's work, shared by menuCritObj and
 * menuCritObjDraw: menuStatDispCreate registers both on the same task and
 * menuCritDispOn writes the work. +0x80 is an int count, stored by
 * menuCritDispOn and tested by menuCritObjDraw and menuCritObj.
 */
typedef struct MenuCritWork {
    u32 flags;                        /* +0x00: bit 2 hides the indicator */
    unsigned char unmodeled_04[0x7C];
    int count;                        /* +0x80: 0 draws the banner without a digit */
    unsigned char unmodeled_84[0xC];
    MenuObjTransform transform;       /* +0x90: menuCritObjDraw reads
                                       * position.x/z; menuCritDispOn
                                       * takes work+0x90 once and seeds both points */
} MenuCritWork;

/* Bit of MenuCritWork.flags that hides the indicator: menuCritDispOff sets
 * it and menuCritObjDraw skips drawing while it is set. An enum constant
 * rather than a macro. */
enum MenuCritWorkFlag {
    MENU_CRIT_HIDDEN = 0x4
};

/* Reads the work slot as an int-sized word, like menuStockWorkGet below.
 * menuCritDispOn reloads the slot after each int store, as the original does. */
static __inline__ MenuCritWork *menuCritWorkGet(ObjectTask *task)
{
    /* Read as a u32 on purpose: a direct `(MenuCritWork *)task->work` cast
     * compiles the callers' loads differently. */
    return (MenuCritWork *)*(u32 *)&task->work;
}

typedef struct MenuStockWork {
    u32 flags;                         /* +0x00: bit 0x4 hides the face */
    unsigned char unmodeled_04[0x6C];
    int frameCount;                    /* +0x70: menuStockObj adds 1 every
                                        * update; command 3 moves the face
                                        * only once it is nonzero */
    unsigned char unmodeled_74[4];
    /* A code-shape device, not data: width is only ever read and written as
     * `value`. `alias` is never accessed; it makes the int store to width
     * conflict with the void * load of ObjectTask.work in gcc 2.96's alias
     * analysis, so menuStockObj reloads the slot after storing width, as the
     * original does. */
    union {
        int value;
        void *alias;
    } width;                           /* +0x78: menuStockStart seeds 4;
                                        * menuStockObj adds 4 while it is below
                                        * 0x14 and writes 0x14 otherwise;
                                        * menuStockObjDraw passes it as the
                                        * sprite width */
    int stockSlot;                     /* +0x7C: menuStockDead indexes
                                        * actStockGet() with it */
    int index;                         /* +0x80: the face's pStockObj slot;
                                        * menuStockDead decrements every later
                                        * face's index by one */
    unsigned char unmodeled_84[0xC];
    MenuObjTransform transform;        /* +0x90: menuStockDead writes its target */
} MenuStockWork;

/* Screen x/z of each stock face, read as int pairs. menuStockDead also indexes
 * this table. */
typedef struct MenuStockPos {
    int x;
    int z;
} MenuStockPos;
extern MenuStockPos stockPosTbl[5];

void *objCmdTailGet(ObjectTask *task);

/* Bit of MenuStockWork.flags that hides a face: menuStockStart sets it for
 * the fifth face and clears it for the other four; menuStockObjDraw tests it. */
enum MenuStockWorkFlag {
    MENU_STOCK_HIDDEN = 0x4
};

/*
 * Reads a stock face's MenuStockWork from its ObjectTask work slot as an
 * int-sized word. The original reloads the slot after each int store into the
 * record, but schedules the reload above the float stores: the slot's load
 * conflicts with int stores only. Read as the published `void *`, gcc keeps
 * the first load across those stores.
 */
static __inline__ MenuStockWork *menuStockWorkGet(ObjectTask *task)
{
    return (MenuStockWork *)*(u32 *)&task->work;
}

/*
 * The status window's +0x90 block (the same span as MenuObjTransform in the
 * other objects): the window position, then the displayed values that
 * menuPlStatPut truncates to ints.
 */
typedef struct MenuStatDisplay {
    Vector4 position;                  /* +0x00 */
    float ap;                          /* +0x10 */
    float hp;                          /* +0x14 */
    float ep;                          /* +0x18 */
    unsigned char unmodeled_1c[0x30 - 0x1C];
    float bp;                          /* +0x30 */
} MenuStatDisplay;

/*
 * The status-icon task's own work record, as far as menuStatObjDraw,
 * menuStatNamePut and menuPlStatPut read it. menuStatNamePut reads the
 * character id and the icon slot (+0x78, +0x7c) and passes them to
 * menuStatIconChk/dataStatNameGet as the character id and the icon slot to
 * check. menuStatObjDraw reads +0x80 and compares it against actUnitGet()'s
 * BattleUnit *, showing the name popup only for the currently active unit.
 */
typedef struct MenuStatObjWork MenuStatObjWork;
struct MenuStatObjWork {
    unsigned char unmodeled_00[0x78];
    int charaId;       /* +0x78 */
    int iconSlot;       /* +0x7C */
    BattleUnit *unit;   /* +0x80 */
    unsigned char unmodeled_84[0x90 - 0x84];
    MenuStatDisplay display; /* +0x90 */
};

/*
 * objCmdPtrGet's return record as this TU reads it: the selector word at
 * +0x00 and the argument at +0x04.
 */
typedef struct MenuCmdEntryView MenuCmdEntryView;
struct MenuCmdEntryView {
    int selector; /* +0x00 */
    int argument; /* +0x04: menuATBObj counts it down under selector 1 */
};

/*
 * The battle map's height value. menuStatDispCreate starts it at 0x40 along
 * with mapHDist, and menuATBObj steps it by 8 per update toward mapHDist.
 * menuMapHSet moves mapHDist to one of the two rest values. menuMapDisp
 * offsets the map by 0x40 - mapH; menuATBObjDraw draws the map item and
 * target line only while it is 0x40, and menuCritObjDraw draws the critical
 * indicator only while it is 0x14.
 */
static int mapH;

/* The two values mapH rests at: menuMapHSet(nonzero) selects MENU_MAP_H_SET,
 * and menuMapHSet(0) and menuStatDispCreate select MENU_MAP_H_RESET. */
enum MenuMapH {
    MENU_MAP_H_SET = 0x14,
    MENU_MAP_H_RESET = 0x40
};

static float ETRot;

void menuATBObj(ObjectTask *task);
void menuATBObjDraw(ObjectTask *task);
void menuKeyGuideObj(void);
void menuKeyGuideObjDraw(ObjectTask *guideTask);
void menuBoostObj(ObjectTask *task);
void menuBoostObjDraw(ObjectTask *task);
int menuCritObj(ObjectTask *task);
void menuCritObjDraw(ObjectTask *task);
void menuStockObj(ObjectTask *task);
void menuStockObjDraw(ObjectTask *task);
void menuStatObj(ObjectTask *task);
void menuStatObjDraw(ObjectTask *task);
void menuStatPreObj(void);
void menuStatPreObjDraw(ObjectTask *task);
extern void menuTimeSet(int time);
extern int unitTblGet(int side, BattleUnit ***table);
/* The z every status icon starts at. Its own enum type keeps that read out of
 * the int alias set of the task work stores. */
enum MenuStatStartZ { MENU_STAT_START_Z = -88 };
extern const enum MenuStatStartZ menuStatStartZ;

/* The common head of an on-screen object's work record: the flag word and
 * the +0x90 movement block. */
typedef struct MenuObjWork {
    u32 flags;                        /* +0x00: bit 2 hides the object */
    unsigned char unmodeled_04[0x8C];
    MenuObjTransform transform;       /* +0x90 */
} MenuObjWork;

void objStdInit(ObjectTask *task);
void *objCmdPush(ObjectTask *task);

static __inline__ MenuStatObjWork *menuStatWorkGet(ObjectTask *task)
{
    return (MenuStatObjWork *)*(u32 *)&task->work;
}

/*
 * Creates the battle status display: the ATB, key guide, boost and critical
 * objects, five stock faces, and one status object per unit in unit table 0.
 */
void menuStatDispCreate(void)
{
    MenuCmdEntryView *entry;
    MenuObjTransform *transform;
    ObjectTask *task;
    BattleUnit **units;
    float x;
    float stockX;
    float stockZ;
    int count;
    int i;
    int offset;
    const enum MenuStatStartZ *startZ = &menuStatStartZ;

    pPreObj = objEntry2(menuStatPreObj, menuStatPreObjDraw);
    objStdInit(pPreObj);

    /* Fixed-position objects. */
    pAtbObj = objEntry2(menuATBObj, menuATBObjDraw);
    x = 376.0f;
    objStdInit(pAtbObj);
    transform = &((MenuObjWork *)pAtbObj->work)->transform;
    transform->position.x = x;
    transform->position.y = 0.0f;
    transform->position.z = 608.0f;
    entry = objCmdPush(pAtbObj);
    entry->selector = 1;
    entry->argument = 10;
    entry = objCmdPush(pAtbObj);
    entry->selector = 2;
    transform->target.x = x;
    transform->target.y = 0.0f;
    transform->target.z = 300.0f;
    mapH = mapHDist = 64;
    RingRot = 0.0f;
    menuTimeSet(rnd(3));
    ETRot = ETRotDist;

    pKeyGuideObj = objEntry2(menuKeyGuideObj, menuKeyGuideObjDraw);
    objStdInit(pKeyGuideObj);
    transform = &((MenuObjWork *)pKeyGuideObj->work)->transform;
    transform->position.x = 8.0f;
    transform->position.y = 0.0f;
    transform->position.z = 292.0f;

    pBoostObj = objEntry2(menuBoostObj, menuBoostObjDraw);
    objStdInit(pBoostObj);
    transform = &((MenuObjWork *)pBoostObj->work)->transform;
    transform->position.x = 260.0f;
    transform->position.y = 0.0f;
    transform->position.z = 404.0f;
    ((MenuObjWork *)pBoostObj->work)->flags |= MENU_CRIT_HIDDEN;

    pCritObj = objEntry2(menuCritObj, menuCritObjDraw);
    objStdInit(pCritObj);
    menuCritWorkGet(pCritObj)->count = -1;
    transform = &menuCritWorkGet(pCritObj)->transform;
    transform->position.x = 576.0f;
    transform->position.y = 0.0f;
    transform->position.z = 310.0f;
    menuCritWorkGet(pCritObj)->flags |= MENU_CRIT_HIDDEN;

    /* Stock faces; the fifth one starts hidden. */
    for (i = 0; i < 5; i++) {
        task = objEntry2(menuStockObj, menuStockObjDraw);
        objStdInit(task);
        transform = &menuStockWorkGet(task)->transform;
        stockX = stockPosTbl[i].x;
        stockZ = stockPosTbl[i].z;
        menuStockWorkGet(task)->index = i;
        transform->position.y = 0.0f;
        transform->position.x = stockX;
        menuStockWorkGet(task)->stockSlot = i;
        transform->position.z = stockZ;
        if (i >= 4) {
            menuStockWorkGet(task)->flags |= MENU_STOCK_HIDDEN;
        }
        pStockObj[i] = task;
    }

    /* One status object per unit. */
    count = unitTblGet(0, &units);
    offset = 0;
    if (count != 0) {
        float w = *(float *)&units;

        *(float *)&units = w;
    }
    for (i = 0; i < count; i++) {
        void *object = menuStatObj;
        BattleUnit *unit = units[i];

        if (unit == 0) {
            pStatObj[i] = 0;
        } else {
            void (*draw)(ObjectTask *);
            float keep;

            draw = menuStatObjDraw;
            draw = 0;
            draw = menuStatObjDraw;
            keep = *(float *)unit;
            if (count != 0) {
                *(float *)unit = keep;
            }
            task = objEntry2(menuStatObj, draw);
            x = offset;
            if (offset != 0) {
                float word = *(float *)unit;

                *(float *)unit = word;
            }
            objStdInit(task);
            offset += 172;
            transform = (MenuObjTransform *)&menuStatWorkGet(task)->display;
            pStatObj[i] = task;
            transform->ap = 0;
            transform->hp = calcUPGet(unit)->currentHp;
            transform->ep = calcUPGet(unit)->currentEp;
            transform->bp = 0;
            menuStatWorkGet(task)->charaId = 7;
            transform->position.z = *startZ;
            menuStatWorkGet(task)->iconSlot = 0x8000;
            transform->position.x = x;
            transform->position.y = 0;
            menuStatWorkGet(task)->unit = unit;
            entry = objCmdPush(task);
            entry->selector = 1;
            entry->argument = 10;
            entry = objCmdPush(task);
            entry->selector = 2;
            transform->target.z = 16.0f;
            transform->target.x = x;
            transform->target.y = 0;
        }
    }
}


/* Removes the status display objects and closes the battle, ether and item windows. */
void menuStatDispRemove(void) {
    int i;

    if (pPreObj != 0) {
        objRemove(pPreObj);
        pPreObj = 0;
    }
    if (pAtbObj != 0) {
        objRemove(pAtbObj);
        pAtbObj = 0;
    }
    if (pKeyGuideObj != 0) {
        objRemove(pKeyGuideObj);
        pKeyGuideObj = 0;
    }
    if (pBoostObj != 0) {
        objRemove(pBoostObj);
        pBoostObj = 0;
    }
    if (pCritObj != 0) {
        objRemove(pCritObj);
        pCritObj = 0;
    }
    for (i = 0; i < 5; i++) {
        if (pStockObj[i] != 0) {
            objRemove(pStockObj[i]);
            pStockObj[i] = 0;
        }
    }
    for (i = 0; i < 3; i++) {
        if (pStatObj[i] != 0) {
            objRemove(pStatObj[i]);
            pStatObj[i] = 0;
        }
    }
    if (pMenuBat != 0) {
        menuCloseBat();
    }
    menuCloseEth();
    menuCloseItm();
}

/* Empty body, as in the original. */
void menuStatPreObj(void) {
}

void menuStatTexTrans(void);

void menuStatPreObjDraw(ObjectTask *task) {
    menuStatTexTrans();
}

int objCmdNext(ObjectTask *task);
int objHatoVec(ObjectTask *task, int direction);
int objStdMove(ObjectTask *task);

/*
 * The ATB gauge object's update: runs its queued command (1 waits out the
 * argument count, 2 moves the gauge), steps mapH by 8 toward mapHDist inside
 * the two rest heights, and turns the event-timer ring (ETRot) by 8 degrees
 * per update toward ETRotDist, snapping once within one step.
 */
void menuATBObj(ObjectTask *task)
{
    MenuCmdEntryView *entry;
    float step;
    float twoPi;
    float rot;
    float distance;

    entry = objCmdPtrGet(task);
    /* Unsigned switch; the case 2 body is written ahead of case 1 to match the
     * original's layout. */
    switch ((unsigned int)entry->selector) {
    case 0:
        break;
    case 2:
        objHatoVec(task, 8);
        if (objStdMove(task) != 0) {
            objCmdNext(task);
        }
        break;
    case 1:
        entry->argument--;
        if (entry->argument < 0) {
            objCmdNext(task);
        }
        break;
    }
    if (mapH != mapHDist) {
        if (mapH > mapHDist) {
            mapH -= 8;
        } else {
            mapH += 8;
        }
        if (mapH > MENU_MAP_H_RESET) {
            mapH = MENU_MAP_H_RESET;
        }
        if (mapH < MENU_MAP_H_SET) {
            mapH = MENU_MAP_H_SET;
        }
    }
    step = 0.13962634f;
    if (ETRot != ETRotDist) {
        twoPi = 6.2831855f;
        rot = ETRot + step;
        ETRot = rot;
        if (rot > twoPi) {
            ETRot = rot - twoPi;
        }
        distance = ETRot - ETRotDist;
        /* The absolute value sits inside the condition, as in the original,
         * which tests against step on both arms. */
        if (((distance < 0.0f) ? -distance : distance) < step) {
            ETRot = ETRotDist;
        }
    }
}

/* Empty body, as in the original. */
void menuKeyGuideObj(void) {
}

void menuKeyGuideDisp(ObjectTask *guideTask, BattleUnit *unitTask, int phase);

void menuKeyGuideObjDraw(ObjectTask *guideTask) {
    BattleUnit *unit;
    int phase;

    unit = manWorkGet()->unitTask;
    phase = manWorkGet()->controlPhase;
    menuKeyGuideDisp(guideTask, unit, phase);
}

int menuStatIconChk(int charaId, int iconSlot);
extern int calcStatGetNorm(BattleUnit *unit, int category, int flag);

/*
 * Per-frame update of a unit's status object: runs its command, eases the
 * displayed ap/hp/ep/bp values toward the unit's values, and cycles the
 * status icon once its countdown runs out.
 */
void menuStatObj(ObjectTask *arg0) {
    u32 *cmd;
    BattleUnit *unit;
    CalcUnitParam *up;
    MenuStatDisplay *display;
    f32 *p0;
    f32 *p1;
    f32 *p2;
    f32 *p3;
    f32 d0;
    f32 n0;
    f32 d1;
    f32 n1;
    f32 d2;
    f32 n2;
    f32 d3;
    f32 n3;
    int i;
    int j;
    int idx;
    int ret;
    int cur;
    int bit;

    display = &((MenuStatObjWork *)*(u32 *)&arg0->work)->display;
    cmd = objCmdPtrGet(arg0);
    switch (cmd[0]) {
    case 0:
        break;
    case 2:
        objHatoVec(arg0, 8);
        if (objStdMove(arg0) != 0) {
            cmd[1] = 0;
            objCmdNext(arg0);
        }
        return;
    case 1:
        if ((int)--cmd[1] < 0) {
            cmd[1] = 0;
            objCmdNext(arg0);
        }
        return;
    }
    /* Each displayed value snaps when it is within 0.5 of the target, and
     * otherwise moves a fixed fraction of the way there. */
    p0 = &display->ap;
    p1 = &display->hp;
    p2 = &display->ep;
    p3 = &display->bp;
    unit = ((MenuStatObjWork *)*(u32 *)&arg0->work)->unit;
    up = calcUPGet(unit);

    d0 = up->dblActKind - *p0;
    if (d0 < 0.0f) {
        n0 = -d0;
        if (n0 < 0.5f) goto snap0;
        goto lerp0;
    }
    if (d0 < 0.5f) {
snap0:
        *p0 = up->dblActKind;
    } else {
lerp0:
        *p0 = *p0 + (up->dblActKind - *p0) * 0.25f;
    }
    d1 = up->currentHp - *p1;
    if (d1 < 0.0f) {
        n1 = -d1;
        if (n1 < 0.5f) goto snap1;
        goto lerp1;
    }
    if (d1 < 0.5f) {
snap1:
        *p1 = up->currentHp;
    } else {
lerp1:
        *p1 = *p1 + (up->currentHp - *p1) * 0.125f;
    }
    d2 = up->currentEp - *p2;
    if (d2 < 0.0f) {
        n2 = -d2;
        if (n2 < 0.5f) goto snap2;
        goto lerp2;
    }
    if (d2 < 0.5f) {
snap2:
        *p2 = up->currentEp;
    } else {
lerp2:
        *p2 = *p2 + (up->currentEp - *p2) * 0.125f;
    }
    d3 = up->bpGauge - *p3;
    if (d3 < 0.0f) {
        n3 = -d3;
        if (n3 < 0.5f) goto snap3;
        goto lerp3;
    }
    if (d3 < 0.5f) {
snap3:
        *p3 = up->bpGauge;
    } else {
lerp3:
        *p3 = *p3 + (up->bpGauge - *p3) * 0.125f;
    }
    /* Without any status bits, the object resets to character 7 and icon 0x8000. */
    j = 0;
    for (i = 0; i < 7; i++) {
        j |= calcStatGetNorm(unit, i, 0x7FFF);
    }
    if (j == 0) {
        ((MenuStatObjWork *)*(u32 *)&arg0->work)->charaId = 7;
        ((MenuStatObjWork *)*(u32 *)&arg0->work)->iconSlot = 0x8000;
        return;
    }
    cur = ((MenuStatObjWork *)*(u32 *)&arg0->work)->charaId;
    bit = *(u16 *)&((MenuStatObjWork *)*(u32 *)&arg0->work)->iconSlot;
    /* When the countdown is spent, pick the next icon: first the current
     * unit's next bit, then the other units in turn. */
    if ((int)cmd[1] <= 0 && bit != 0) {
        cmd[1] = 0x1E;
        ret = -1;
        if (menuStatIconChk(cur, calcStatGetNorm(unit, cur, bit)) == -1) {
            ((MenuStatObjWork *)*(u32 *)&arg0->work)->charaId = 7;
            ((MenuStatObjWork *)*(u32 *)&arg0->work)->iconSlot = 0x8000;
        }
        for (i = 0; i < 8; i++) {
            idx = (cur + i) % 8;
            for (j = bit >> 1; j != 0; j >>= 1) {
                ret = menuStatIconChk(idx, calcStatGetNorm(unit, idx, j));
                if (ret != -1) {
                    ((MenuStatObjWork *)*(u32 *)&arg0->work)->charaId = idx;
                    ((MenuStatObjWork *)*(u32 *)&arg0->work)->iconSlot = j;
                    break;
                }
            }
            if (ret != -1) break;
            bit = 0x8000;
        }
        if (ret == -1) {
            for (j = 0x4000; j != 0; j >>= 1) {
                if (menuStatIconChk(cur, calcStatGetNorm(unit, cur, j)) != -1) {
                    ((MenuStatObjWork *)*(u32 *)&arg0->work)->charaId = cur;
                    ((MenuStatObjWork *)*(u32 *)&arg0->work)->iconSlot = j;
                    break;
                }
            }
        }
    }
    cmd[1]--;
}

/* Advances the boost ring's rotation by one step. */
void menuBoostObj(ObjectTask *task) {
    float twoPi;
    float rot;

    objCmdPtrGet(task);
    twoPi = 6.2831855f;
    rot = RingRot - 0.034906585f;
    RingRot = rot;
    if (rot > twoPi) {
        RingRot = rot - twoPi;
    }
}

/* actStockGet returns the action-stock table; menuBoostObjDraw tests its
 * entries 4 and 5 for null. */
extern BattleUnit **actStockGet(void);
void menuBoostObjDrawCha(ObjectTask *task);
void menuBoostObjDrawSel(ObjectTask *task);

void menuBoostObjDraw(ObjectTask *task) {
    if (actStockGet()[4] == 0 && actStockGet()[5] == 0) {
        menuBoostObjDrawSel(task);
        return;
    }
    menuBoostObjDrawCha(task);
}

extern void xglMatrixUnit(float matrix[4][4]);
extern void xglVectorMulMat(float *dst, float mtx[4][4], void *src);
extern void grRotMatrix(float dst[4][4], float src[4][4], float *rot);
extern void grOpenFT4(GrPacket *packet);
extern void grCloseFT4(void);
extern void grPutFT4(GrPacket *packet, void *prim, int flag);
extern void grGsRegSet(GrPacket *packet, unsigned long long reg, unsigned long long value);
extern BattleUnit **actStockGet(void);
void menuATBEnvPalSet(GrPacket *packet, int palette);
void faceUVGet(BattleUnit *unit, int *srcX, int *srcY);
typedef union BoostColU {
    float f;
    unsigned char c[4];
} BoostColU;
typedef struct BoostFT4 {
    AlignedVector4 vertex[4];
    unsigned char col[4];
    float u[4];
    float v[4];
} BoostFT4;
typedef struct BoostFT4Pair {
    BoostFT4 e[2];
} BoostFT4Pair;
extern const BoostFT4Pair boostFT4Pair;

/* Draws the boost ring, rotated by RingRot, around the selected unit's face. */
void menuBoostObjDrawCha(ObjectTask *task) {
    GrPacket packet;
    float mtx[4][4];
    float rot[4];
    float vec[4];
    BoostFT4 vtx;
    BattleUnit *sel = 0;
    BoostFT4Pair tbl = boostFT4Pair;
    int fw;
    int fh;

    int mode;
    int x;
    int y;
    int j;
    int n;
    int k;

    /* Entry 5 selects the unit; entry 4 overrides it with mode 2. */
    mode = 0;
    if (actStockGet()[5] != 0) {
        sel = actStockGet()[5];
    }
    if (actStockGet()[4] != 0) {
        mode = 2;
        sel = actStockGet()[4];
    }
    if (sel != 0) {
        grGpInit(&packet);
        menuATBEnvMake(&packet);
        {
            Vector4 *pos = &((MenuObjWork *)task->work)->transform.position;
            x = (int)pos->x;
            y = (int)pos->z;
        }
        grGsRegSet(&packet, SCE_GS_ALPHA_1, 0x44);
        grGsRegSet(&packet, SCE_GS_TEX1_1, 0x60);
        menuATBEnvPalSet(&packet, mode);
        xglMatrixUnit(mtx);
        rot[0] = 0;
        rot[1] = 0;
        rot[2] = RingRot;
        rot[3] = 0;
        grRotMatrix(mtx, mtx, rot);
        /* Two ring quads, each vertex rotated and offset to the window origin. */
        for (j = 0, n = 0; j < 2; j++, n += 7) {
            Vector4 *src = (Vector4 *)((char *)(n * sizeof(Vector4)) + (int)tbl.e[0].vertex);
            for (k = 0; k < 4; k++) {
                xglVectorMulMat(vec, mtx, src);
                *(BoostColU *)vtx.col = *(BoostColU *)tbl.e[j].col;
                {
                    Vector4 *dst = &vtx.vertex[k];
                    dst->x = (float)(x + 0x32) + vec[0];
                    dst->y = (float)(y - 0x16) + vec[1];
                    dst->z = 16777212.0f;
                }
                vtx.u[k] = tbl.e[j].u[k];
                vtx.v[k] = tbl.e[j].v[k];
                src++;
            }
            grOpenFT4(&packet);
            grPutFT4(&packet, &vtx, 0);
            grCloseFT4();
        }
        grGsRegSet(&packet, SCE_GS_ALPHA_1, 0);
        grGsRegSet(&packet, SCE_GS_TEX1_1, 0);
        menuATBEnvPalSet(&packet, 1);
        grOpenSpr(&packet);
        faceUVGet(sel, &fw, &fh);
        menuStatSprite(&packet, x, y, 0x5E, 0x20, 0x9A, 0x60);
        menuStatSprite(&packet, x + 0x28, y - 0x22, 0x14, 0x18, fw, fh);
        menuStatSprite(&packet, x + 0x26, y - 0x24, 0x18, 0x1C, 0xC0, 0x20);
        grCloseSpr();
        grPacketSend(&packet);
    }
}

extern int unitTblGet(int side, BattleUnit ***table);
extern int calcBoostChk(BattleUnit *unit, int mode);
typedef struct BoostOfs3 {
    int v[3];
} BoostOfs3;
typedef struct BoostCol3 {
    MenuRGBA c[3];
} BoostCol3;
extern const BoostOfs3 boostOffsetsFirst;
extern const BoostOfs3 boostOffsetsSecond;
extern const BoostCol3 boostColors;

/* Draws the boost window frame and the face of each unit whose calcBoostChk passes. */
void menuBoostObjDrawSel(ObjectTask *task) {
    GrPacket packet;
    BoostOfs3 xo = boostOffsetsFirst;
    BoostOfs3 yo = boostOffsetsSecond;
    BoostCol3 col = boostColors;
    BattleUnit **units;
    int i;
    int n;
    int x;
    int y;
    int fw;
    int fh;

    if (!(((MenuObjWork *)*(u32 *)&task->work)->flags & 4)) {
        grGpInit(&packet);
        menuATBEnvMake(&packet);
        {
            Vector4 *pos = &((MenuObjWork *)task->work)->transform.position;
            x = (int)pos->x;
            y = (int)pos->z;
        }
        grOpenSpr(&packet);
        menuStatSprite(&packet, x, y, 0x5E, 0x20, 0x9A, 0x60);
        menuStatSprite(&packet, x + 0x24, y - 0x14, 0x18, 0xE, 0x80, 0x20);
        i = 0;
        n = unitTblGet(0, &units);
        if (n > 0) {
            do {
                if (units[i] != 0 && calcBoostChk(units[i], 1) != 0) {
                    faceUVGet(units[i], &fw, &fh);
                    menuStatSprite(&packet, x + xo.v[i], y + yo.v[i], 0x14, 0x18, fw, fh);
                    menuStatSpriteRGB(&packet, x + xo.v[i] - 2, y + yo.v[i] - 4, 0x18, 0x20, 0x80, 0, &col.c[i]);
                }
                i++;
            } while (i < n);
        }
        grCloseSpr();
        grPacketSend(&packet);
    }
}

/*
 * The boost-ring display object. Its work record's first word is a flag set;
 * bit 2 (0x4) hides the object.
 */
extern ObjectTask *pBoostObj;

void menuBoostDisp(int enabled)
{
    if (enabled != 0) {
        ((MenuObjWork *)pBoostObj->work)->flags &= ~4; /* clear hidden bit 2 */
        return;
    }
    ((MenuObjWork *)pBoostObj->work)->flags |= 4; /* set hidden bit 2 */
}

void *objCmdPush(ObjectTask *task);

/*
 * Shows the critical indicator with count: unless it is already showing that
 * count, queues command 1 (move in from x 576 to x 376 at z 310) and clears
 * the hidden bit. The three constants stay locals, in the original's load order.
 */
void menuCritDispOn(int count)
{
    MenuCmdEntryView *entry;
    MenuObjTransform *transform;
    float startX;
    float z;
    float endX;

    if ((menuCritWorkGet(pCritObj)->flags & MENU_CRIT_HIDDEN) == 0 &&
        menuCritWorkGet(pCritObj)->count == count) {
        return;
    }
    entry = objCmdPush(pCritObj);
    startX = 576.0f;
    z = 310.0f;
    endX = 376.0f;
    transform = &menuCritWorkGet(pCritObj)->transform;
    entry->selector = 1;
    menuCritWorkGet(pCritObj)->count = count;
    transform->position.x = startX;
    transform->position.y = 0.0f;
    transform->position.z = z;
    transform->target.x = endX;
    transform->target.y = 0.0f;
    transform->target.z = z;
    menuCritWorkGet(pCritObj)->flags &= ~MENU_CRIT_HIDDEN;
}

void menuCritDispOff(void)
{
    menuCritWorkGet(pCritObj)->flags |= MENU_CRIT_HIDDEN;
}
/* The work slot holds the address of the indicator's MenuCritWork.
 * menuCritObj reads the slot once and keeps the record in a local;
 * menuCritObjDraw loads the slot afresh before each read, so every access
 * there casts the slot itself instead of caching it. */

/* Critical indicator callback: runs its move command, then advances the queue. */
int menuCritObj(ObjectTask *task)
{
    MenuCmdEntryView *entry;
    MenuCritWork *work;
    int result;

    result = 1;
    entry = objCmdPtrGet(task);
    if (entry->selector == 1) {
        work = task->work;
        objHatoVec(task, (work->count == 0) ? 4 : 2);
        result = objStdMove(task);
        if (result != 0) {
            result = objCmdNext(task);
        }
    }
    return result;
}

void menuStatIconEnvMake(void *packet);


/*
 * Draws the critical banner and, for a nonzero count, its digit cell (0x18
 * wide). The digit's sheet offset is computed in its own statement ahead of
 * the first call: the original keeps it live across that call, and one
 * combined (count - 1) * 0x18 + 0xB1 expression would fold to a different form.
 */
void menuCritObjDraw(ObjectTask *task)
{
    GrPacket packet;
    int x;
    int z;
    int count;
    int digit;
    int digitX;

    if ((((MenuCritWork *)task->work)->flags & MENU_CRIT_HIDDEN) == 0 && mapH == MENU_MAP_H_SET) {
        grGpInit(&packet);
        menuStatIconEnvMake(&packet);
        x = (int)((MenuCritWork *)task->work)->transform.position.x;
        z = (int)((MenuCritWork *)task->work)->transform.position.z;
        grOpenSpr(&packet);
        menuStatSprite(&packet, x, z, 0x84, 0x18, 0x7C, 0);
        count = ((MenuCritWork *)task->work)->count;
        if (count == 0) {
            menuStatSprite(&packet, x + 0x12, z + 4, 0x61, 0x10, 0x9B, 0x19);
        } else {
            digit = count - 1;
            digitX = digit * 0x18;
            menuStatSprite(&packet, x + 0x12, z + 4, 0x41, 0x10, 0xB1, 0x2A);
            menuStatSprite(&packet, x + 0x53, z + 4, 0x18, 0x10,
                           digitX + 0xB1, 0x3B);
        }
        grCloseSpr();
        grPacketSend(&packet);
    }
}

static int selX;
static int selY;
void menuEventTimerDisp(ObjectTask *task, GrPacket *packet);
void menuMapDisp(ObjectTask *task, GrPacket *packet);
void menuMapItemDisp(ObjectTask *task, GrPacket *packet);
void menuTgtLineDisp(ObjectTask *task, GrPacket *packet);

/*
 * Draws the ATB window: the event timer ring and the map, plus the map item
 * and target line while the map is at its reset height.
 */
void menuATBObjDraw(ObjectTask *task)
{
    GrPacket packet;
    int h;

    grGpInit(&packet);
    selX = selY = 0;
    menuEventTimerDisp(task, &packet);
    menuMapDisp(task, &packet);
    h = mapH;
    if (h == MENU_MAP_H_RESET) {
        menuMapItemDisp(task, &packet);
        if (mapH == h) {
            menuTgtLineDisp(task, &packet);
        }
    }
    grPacketSend(&packet);
}

/* Sets the rest height mapH moves toward: nonzero selects MENU_MAP_H_SET. */
void menuMapHSet(int enable)
{
    if (enable != 0) {
        mapHDist = MENU_MAP_H_SET;
        return;
    }
    mapHDist = MENU_MAP_H_RESET;
}

int curSelGrpGet(void);
void menuStatSprite2(GrPacket *packet, int x, int y, int width, int height, int srcLeft, int srcTop, int srcRight, int srcBottom);
void menuStatSprite2RGB(GrPacket *packet, int x, int y, int width, int height, int srcLeft, int srcTop, int srcRight, int srcBottom, const MenuRGBA *rgba);
extern const MenuRGBA menuBlueRedColors[2];

/* Draws the battle map panel at the ATB window's position, sized by mapH. */
void menuMapDisp(ObjectTask *task, GrPacket *packet)
{
    int sp20;
    int h;
    int d;
    int x;
    int y;
    int a;
    int b;
    int y23;
    int y22;
    int x17;
    int x19;
    int x20;
    int yb;
    MenuRGBA col[2];

    h = mapH;
    d = 0x40 - h;
    y22 = (int)((MenuObjWork *)task->work)->transform.position.z;
    y = y22;
    x = (int)((MenuObjWork *)task->work)->transform.position.x;
    sp20 = 0x35;
    __builtin_memcpy(col, menuBlueRedColors, 8);
    y23 = y + 0x4C;
    y22 = y + d;
    x17 = x + 63;
    sp20 = 0x35 - d;
    menuATBEnvMake(packet);
    grGsRegSet(packet, SCE_GS_TEX1_1, 0x60U);
    grGsRegSet(packet, SCE_GS_ALPHA_1, 0x44U);
    menuATBEnvPalSet(packet, 0);
    grOpenSpr(packet);
    menuStatSprite2(packet, x, y22, 0x40, h, 1, 1, 0x40, h);
    menuStatSprite2(packet, x17, y22, 0x40, h, 0x40, 1, 0, h);
    menuStatSprite2(packet, x, y23, 0x40, h, 1, h, 0x40, 0);
    menuStatSprite2(packet, x17, y23, 0x40, h, 0x40, h, 0, 0);
    if (curSelGrpGet() == 0) {
        a = 0;
        b = 1;
    } else {
        a = 1;
        b = 0;
    }
    x19 = x + 0x34;
    x20 = x + 0x40;
    yb = y23 + sp20;
    menuStatSprite2RGB(packet, x19, y22, 0xC, 0xA, 0, 0x40, 0xC, 0x4A, &col[a]);
    menuStatSprite2RGB(packet, x20, y22, 0xC, 0xA, 0xC, 0x40, 0, 0x4A, &col[a]);
    menuStatSprite2RGB(packet, x19, yb, 0xC, 0xA, 0, 0x4B, 0xC, 0x55, &col[b]);
    menuStatSprite2RGB(packet, x20, yb, 0xC, 0xA, 0xC, 0x4B, 0, 0x55, &col[b]);
    grCloseSpr();
}

/* Defined by ov01/tu008 (src/ov01/gr_gp_init.c). */
extern void grCloseSpr(void);
extern void grGsRegSet(GrPacket *packet, unsigned long long reg, unsigned long long value);

void menuATBEnvPalSet(GrPacket *packet, int palette);
void menuMapItemDispSub(ObjectTask *task, GrPacket *packet, int index, int x, int z);

/*
 * task->work's +0x90 block is the map item's world position (MenuObjTransform);
 * only x and z feed this draw call.
 */
void menuMapItemDisp(ObjectTask *task, GrPacket *packet)
{
    MenuObjWork *work;
    int x;
    int z;
    int zFar;

    work = task->work;
    z = (int)work->transform.position.z;
    x = (int)work->transform.position.x;
    grGsRegSet(packet, SCE_GS_ALPHA_1, 0);
    zFar = z + 0x4C;
    grGsRegSet(packet, SCE_GS_TEX1_1, 0);
    menuATBEnvPalSet(packet, 1);
    grOpenSpr(packet);
    menuMapItemDispSub(task, packet, 0, x, zFar);
    menuMapItemDispSub(task, packet, 1, x, z);
    grCloseSpr();
}

/* The target line's screen end point; 0 means unset. */
static int selX;
static int selY;

BattleUnit *tgtUnitGet(void);
extern int calcLineChk(BattleUnit *unit);
extern int calcSkillEquChkAll(int skillId);
void menuSelLine(GrPacket *packet, int x0, int y0, int x1, int y1, int blocked);
void menuNumPut(GrPacket *packet, int x, int y, int value, int digits, int style);
void menuStatEnvMake(void *packet);

/*
 * Draws the line from the ATB gauge to the selected target, an enemy mark
 * for an enemy target, and the target's HP: style 1 while it is below
 * 10000 and skill 0x41 is equipped, style 3 otherwise.
 */
void menuTgtLineDisp(ObjectTask *task, GrPacket *packet)
{
    MenuObjWork *work;
    BattleUnit *target;
    Actor *actor;
    CalcUnitParam *up;
    int x;
    int y;
    int srcX;
    int srcY;
    int hp;
    int style;

    work = task->work;
    x = (int)work->transform.position.x;
    y = (int)work->transform.position.z;
    if (selX != 0 && selY != 0) {
        target = tgtUnitGet();
        menuSelLine(packet, x + 0x80, y - 4, selX + 8, selY + 8, calcLineChk(target) == 0);
        actor = target->work;
        if (actor->flags & ACTOR_FLAG_ENEMY) {
            grOpenSpr(packet);
            up = calcUPGet(target);
            if (up->flags & 0x4000) {
                srcX = 0xF0;
                srcY = 0x20;
            } else if (up->flags & 0x2000) {
                srcX = 0xE0;
                srcY = 0x30;
            } else {
                srcX = 0xF0;
                srcY = 0x30;
            }
            menuStatSprite(packet, x + 0x70, y - 0x12, 0x10, 0x10, srcX, srcY);
            grCloseSpr();
        }
        hp = calcUPGet(target)->currentHp;
        if (hp < 10000 && calcSkillEquChkAll(0x41) != 0) {
            style = 1;
        } else {
            style = 3;
        }
        menuStatEnvMake(packet);
        grOpenSpr(packet);
        menuNumPut(packet, x + 0x48, y - 0x12, hp, 4, style);
        grCloseSpr();
    }
}

extern BattleUnit **actStockGet(void);

/*
 * Seeds the five stock faces: slot index, command 2, screen position from
 * stockPosTbl, width 4, and only the fifth face hidden. x and z are read
 * before the width store: the original loads both table words first, and the
 * union width (see MenuStockWork) is stored with alias set 0.
 */
void menuStockStart(void)
{
    ObjectTask *task;
    Vector4 *position;
    MenuCmdEntryView *entry;
    float x;
    float z;
    int i;

    for (i = 0; i < 5; i++) {
        task = pStockObj[i];
        position = &menuStockWorkGet(task)->transform.position;
        entry = objCmdTailGet(task);
        menuStockWorkGet(task)->index = i;
        entry->selector = 2;
        position->y = 0.0f;
        x = stockPosTbl[i].x;
        z = stockPosTbl[i].z;
        menuStockWorkGet(task)->width.value = 4;
        position->x = x;
        position->z = z;
        if (i >= 4) {
            menuStockWorkGet(task)->flags |= MENU_STOCK_HIDDEN;
        } else {
            menuStockWorkGet(task)->flags &= ~MENU_STOCK_HIDDEN;
        }
    }
}

/*
 * Moves the first four stock faces one position left (command 1 toward
 * stockPosTbl[index - 1]); a face already at position 0 stays there and is
 * hidden. Every record access goes through menuStockWorkGet, because the
 * original reloads the work slot after the int store to index.
 */
void menuStockNext(void)
{
    ObjectTask *task;
    MenuObjTransform *transform;
    MenuCmdEntryView *entry;
    int index;
    int i;

    for (i = 0; i < 4; i++) {
        task = pStockObj[i];
        transform = &menuStockWorkGet(task)->transform;
        entry = objCmdTailGet(task);
        index = menuStockWorkGet(task)->index;
        if (index <= 0) {
            menuStockWorkGet(task)->index = 0;
            menuStockWorkGet(task)->flags |= MENU_STOCK_HIDDEN;
        } else {
            index--;
            transform->target.y = 0.0f;
            menuStockWorkGet(task)->index = index;
            entry->selector = 1;
            transform->target.x = stockPosTbl[index].x;
            transform->target.z = stockPosTbl[index].z;
        }
    }
}

/*
 * Brings in the boost face (pStockObj[4]): command 3, shown from its
 * stockPosTbl slot and moving to the first one. The target is written before
 * the position, and the work slot is re-read through menuStockWorkGet after
 * the command store.
 */
void menuStockBoost(void)
{
    ObjectTask *task;
    MenuObjTransform *transform;
    MenuCmdEntryView *entry;

    task = pStockObj[4];
    transform = &menuStockWorkGet(task)->transform;
    entry = objCmdTailGet(task);
    entry->selector = 3;
    transform->target.y = 0.0f;
    transform->position.y = 0.0f;
    transform->target.x = stockPosTbl[0].x;
    transform->target.z = stockPosTbl[0].z;
    transform->position.x = stockPosTbl[4].x;
    transform->position.z = stockPosTbl[4].z;
    menuStockWorkGet(task)->flags &= ~MENU_STOCK_HIDDEN;
}

/*
 * menuStockDead's typed view of the action-stock record actStockGet returns.
 * Indexing this member array, rather than the published int *, keeps the
 * original's address-computation order.
 */
typedef struct MenuActStockView {
    BattleUnit *stock[4]; /* +0x00: the queued unit per stock slot */
} MenuActStockView;

extern const char menuStockDeadText[]; /* "** StockDead %d %d\n" */
extern int printf(const char *format, ...);

/*
 * A dead unit's face leaves the gauge: unless its action-stock slot is
 * still occupied, every later face is sent one position left. printf takes
 * only dead and i: passing the decremented index as well adds a copy.
 */
void menuStockDead(int dead)
{
    ObjectTask *task;
    MenuCmdEntryView *entry;
    MenuObjTransform *transform;
    MenuStockWork *work;
    int index;
    int i;

    if (((MenuActStockView *)actStockGet())->stock[((MenuStockWork *)pStockObj[dead]->work)->stockSlot] == 0) {
        for (i = dead + 1; i < 4; i++) {
            task = pStockObj[i];
            entry = objCmdTailGet(task);
            work = task->work;
            index = work->index;
            transform = &work->transform;
            if (index > 0) {
                index--;
                transform->target.y = 0.0f;
                work->index = index;
                entry->selector = 1;
                transform->target.x = stockPosTbl[index].x;
                transform->target.z = stockPosTbl[index].z;
                printf(menuStockDeadText, dead, i);
            }
        }
    }
}

/*
 * Per-frame update of a stock face: runs its command (1 slides left, 3 slides
 * in once frameCount is set) and grows its width toward 0x14.
 */
void menuStockObj(ObjectTask *task)
{
    MenuCmdEntryView *entry;
    MenuStockWork *stock;

    entry = objCmdPtrGet(task);
    /* Unsigned switch, which reproduces the original's equality chain; a
     * signed selector would split it differently. */
    switch ((unsigned int)entry->selector) {
    case 0:
        break;
    case 1:
        objHatoVec(task, 2);
        if (objStdMove(task) != 0) {
            objCmdNext(task);
        }
        break;
    case 2:
        break;
    case 3:
        if (((MenuStockWork *)task->work)->frameCount != 0) {
            objHatoVec(task, 2);
            if (objStdMove(task) != 0) {
                objCmdNext(task);
            }
        }
        break;
    }
    /* Not a clamp: a width below 0x14 grows by 4 (so 0x12 becomes 0x16),
     * any other value is replaced by 0x14. */
    stock = ((MenuStockWork *)task->work);
    stock->width.value = (stock->width.value < 0x14) ? stock->width.value + 4 : 0x14;
    ((MenuStockWork *)task->work)->frameCount++;
}

/*
 * menuStockObjDraw's bounded view of the record actStockGet returns (the
 * MenuActStockView stock array, then the words this function reads).
 */
typedef struct MenuActStockDrawView {
    BattleUnit *stock[4];   /* +0x00 */
    BattleUnit *boost[2];   /* +0x10: battle_init.c ActStock.boost */
    int count;              /* +0x18: 0 draws no face */
    int picked;             /* +0x1C: subtracted from count */
    BattleUnit *boostUnit;  /* +0x20: face 4's unit */
    int boostState;         /* +0x24: compared with 2 */
} MenuActStockDrawView;

/* A stock unit's actor flags, read as one doubleword. */
typedef struct MenuStockActor {
    u64 flags;
} MenuStockActor;

/* The face's unit must be present (bit 0x1) and not down (bit 0x80). */
#define MENU_STOCK_ACTOR_MASK 0x81
#define MENU_STOCK_ACTOR_PRESENT 0x1

/* The boost face: pStockObj[4], hidden by menuStockStart. */
#define MENU_STOCK_BOOST_FACE 4
#define MENU_STOCK_BOOST_STATE 2

extern int frameCntGet(void);
void faceUVGet(BattleUnit *unit, int *srcX, int *srcY);
void menuStatSprite2(GrPacket *packet, int x, int y, int width, int height, int srcLeft, int srcTop, int srcRight, int srcBottom);
void menuStatSprite2RGB(GrPacket *packet, int x, int y, int width, int height, int srcLeft, int srcTop, int srcRight, int srcBottom, const MenuRGBA *rgba);

/*
 * Draws one stock face beside the ATB gauge: the face of the unit queued in
 * its action-stock slot (the boost unit for face 4), raised by 4 for the
 * current target, and its frame; faces 0 and 4 get a pulsing frame.
 * Ordering notes: faces is computed before the count test, and the pulse is
 * stored as b, g, r to match the original's store order.
 */
void menuStockObjDraw(ObjectTask *task)
{
    GrPacket packet;
    MenuRGBA color = { 0, 0, 0, 0x80 };
    int srcX;
    int srcY;
    MenuActStockDrawView *stock;
    MenuStockWork *work;
    BattleUnit *unit;
    int index;
    int slot;
    int faces;
    int y;
    int width;
    int x;
    int raise;
    int faceX;
    int faceY;
    int inset;
    int pulse;

    if (((MenuStockWork *)task->work)->flags & MENU_STOCK_HIDDEN) {
        return;
    }
    /* Skip the face when there is no count, no slot for it, or the boost
     * state hides it. */
    stock = (MenuActStockDrawView *)actStockGet();
    work = task->work;
    faces = stock->count - stock->picked + 1;
    index = work->index;
    slot = work->stockSlot;
    if (stock->count == 0) {
        return;
    }
    if (index >= faces && index != MENU_STOCK_BOOST_FACE) {
        return;
    }
    if (index == 0 && stock->boostState == MENU_STOCK_BOOST_STATE) {
        return;
    }
    if (index == MENU_STOCK_BOOST_FACE) {
        if (stock->boostState != MENU_STOCK_BOOST_STATE) {
            return;
        }
        unit = stock->boostUnit;
    } else {
        unit = stock->stock[slot];
    }
    if (unit == 0) {
        return;
    }
    if ((((MenuStockActor *)unit->work)->flags & MENU_STOCK_ACTOR_MASK) != MENU_STOCK_ACTOR_PRESENT) {
        return;
    }
    /* The face position is relative to the ATB window origin. */
    x = (int)work->transform.position.x;
    y = (int)work->transform.position.z;
    x += ((MenuObjWork *)pAtbObj->work)->transform.position.x;
    y += ((MenuObjWork *)pAtbObj->work)->transform.position.z;
    grGpInit(&packet);
    menuATBEnvMake(&packet);
    faceUVGet(unit, &srcX, &srcY);
    raise = -4;
    if (unit != tgtUnitGet()) {
        raise = 0;
    }
    faceY = y + raise;
    width = ((MenuStockWork *)task->work)->width.value;
    inset = 10 - width / 2;
    grOpenSpr(&packet);
    faceX = x + inset;
    menuStatSprite2(&packet, faceX, faceY, width, 0x18, srcX, srcY, srcX + 0x14, srcY + 0x18);
    if (index == 0 || index == MENU_STOCK_BOOST_FACE) {
        pulse = (frameCntGet() * 2) & 0x7F;
        if (pulse >= 0x40) {
            pulse = 0x7F - pulse;
        }
        color.b = pulse + 0x80;
        color.g = pulse + 0x80;
        color.r = pulse + 0x80;
        menuStatSprite2RGB(&packet, faceX - 4, faceY - 4, width + 8, 0x20, 0xD8, 0x40, 0xF4, 0x61, &color);
    } else {
        menuStatSprite2(&packet, faceX - 2, faceY - 2, width + 4, 0x1C, 0xC0, 0x20, 0xD9, 0x3D);
    }
    grCloseSpr();
    grPacketSend(&packet);
}

/*
 * The 3x11 face-icon sheet's per-character source cell: chrId<32 indexes a
 * grid of 21x25 cells (column = chrId%3, row = chrId/3); chrId>=32 (no face
 * tile) leaves the row at 0 and picks one of three fixed columns from
 * CalcUnitParam.flags: bit 0x4000 selects sheet x 0xC0, else bit 0x2000
 * selects 0xD5, else 0xEA. Neither bit has an established name, so they stay
 * numeric.
 */
void faceUVGet(BattleUnit *unit, int *srcX, int *srcY)
{
    int chrId;
    CalcUnitParam *up;
    u16 flags;

    chrId = menuStatChrIdGet(unit);
    if (chrId < 0x20) {
        *srcX = (chrId % 3) * 21 + 0x40;
        *srcY = (chrId / 3) * 25;
        return;
    }
    up = calcUPGet(unit);
    *srcY = 0;
    flags = up->flags;
    if (flags & 0x4000) {
        *srcX = 0xC0;
    } else if (flags & 0x2000) {
        *srcX = 0xD5;
    } else {
        *srcX = 0xEA;
    }
}

extern const MenuRGBA menuGreenYellowRedColors[3];
extern float *dataPosTblGet(int index);

/*
 * Draws the map markers of one unit list at (x, z). Index 0 uses one marker
 * sprite and the other lists another. The target's marker position is stored
 * to selX/selY.
 */
void menuMapItemDispSub(ObjectTask *task, GrPacket *packet, int index, int x, int z)
{
    MenuRGBA col[3];
    BattleUnit **tbl;
    BattleUnit *unit;
    int n;
    int i;
    int tmp;
    int id;
    int c;
    int pos;
    int sprite;
    int px;
    int py;
    int dy;

    __builtin_memcpy(col, menuGreenYellowRedColors, 12);
    n = unitTblGet(index, &tbl);
    for (i = 0; i < n; i++) {
        unit = tbl[i];
        if (unit == 0) {
            continue;
        }
        if (*(int *)unit->work & 0x80) {
            continue;
        }
        id = calcUPGet(unit)->posId;
        if (calcStatGetNorm(tbl[i], 2, 0x100) != 0 || calcStatGetNorm(tbl[i], 2, 0x80) != 0
            || calcStatGetNorm(tbl[i], 6, 0x4000) != 0 || calcStatGetNorm(tbl[i], 6, 0x2000) != 0) {
            c = 1;
        } else if (calcStatGetNorm(tbl[i], 0, 0x1000) != 0 || calcStatGetNorm(tbl[i], 4, 0x800) != 0) {
            c = 2;
        } else {
            c = 0;
        }
        if (index == 0) {
            tmp = (int)(-dataPosTblGet(id)[2] * 128.0f / 12.0f + 64.0f - 8.0f);
            dy = (int)(dataPosTblGet(id)[0] * 64.0f * 0.125f - 8.0f);
            py = z + dy;
            px = x + tmp;
            menuStatSpriteRGB(packet, px, py, 0x10, 0x10, 0xC0, 0x3E, &col[c]);
        } else {
            if (id < 0) {
                id = -id;
            }
            pos = 0x1C;
            if (id != 0x23) {
                pos = id;
            }
            sprite = pos;
            if (pos == 0x20 || pos == 0x22) {
                sprite = 0x1E;
            } else if (pos == 0x1F || pos == 0x21) {
                sprite = 0x1D;
            }
            tmp = (int)(-dataPosTblGet(sprite)[2] * 128.0f / 14.0f + 64.0f - 8.0f);
            dy = (int)(64.0f - dataPosTblGet(pos)[0] * 64.0f * 0.0625f - 8.0f);
            py = z + dy;
            px = x + tmp;
            menuStatSprite2RGB(packet, px, py, 0x10, 0x10, 0xC0, 0x4E, 0xD0, 0x3E, &col[c]);
        }
        if (tbl[i] == tgtUnitGet()) {
            int *xs = &selX;
            int *ys = &selY;

            *ys = py;
            *xs = px;
        }
    }
}

/*
 * The target line: a 0x3A-pixel stub left of (x0, y0), then on to (x1, y1);
 * green, or magenta when the line is blocked. The depth is written as an
 * integer constant, which the original converts at run time.
 */
void menuSelLine(GrPacket *packet, int x0, int y0, int x1, int y1, int blocked)
{
    GrF2Vertices line;
    int depth;

    depth = 0xFFFFFC;
    line.color[3] = 0x80;
    if (blocked == 0) {
        line.color[0] = 0;
        line.color[1] = 0xFF;
        line.color[2] = 0x80;
    } else {
        line.color[0] = 0xF0;
        line.color[1] = 0;
        line.color[2] = 0x80;
    }
    grOpenF2(packet);
    line.points[0].x = x0;
    line.points[0].y = y0;
    line.points[0].z = depth;
    line.points[1].x = x0 - 0x3A;
    line.points[1].y = y0;
    line.points[1].z = depth;
    grPutF2(packet, &line, 0);
    line.points[0].x = line.points[1].x;
    line.points[0].y = line.points[1].y;
    line.points[0].z = depth;
    line.points[1].x = x1;
    line.points[1].y = y1;
    line.points[1].z = depth;
    grPutF2(packet, &line, 0);
    grCloseF2();
}

void menuEventTimerDisp(ObjectTask *task, GrPacket *packet)
{
    MenuObjWork *work;
    int x;
    int y;

    work = task->work;
    x = work->transform.position.x;
    y = work->transform.position.z;
    menuATBEnvMake(packet);
    menuEventTimerDispSub(task, packet);
    x -= 0x18;
    y += 0x38;
    grOpenSpr(packet);
    menuStatSprite(packet, x, y, 0x18, 0x4E, 0x80, 0x32);
    grCloseSpr();
}

extern float xglSin(float angle);
extern float xglCos(float angle);

/*
 * Draws the event timer ring: twelve quads on a circle of radius 28, rotated
 * by ETRot. A quad is drawn only when its first vertex lies above its third.
 */
void menuEventTimerDispSub(ObjectTask *task, GrPacket *packet)
{
    float ring[12][4];
    float *point;
    Vector4 *dst;
    Vector4 corner;
    BoostFT4 vtx;
    float mtx[4][4];
    float rot[4];
    float pos[4];
    MenuObjWork *work;
    float radius = 28.0f;
    float halfWidth = 9.0f;
    float angleStep = 0.5235988f;
    int cellSize = 15;
    int originX;
    int originY;
    int i;
    int j;
    int row;
    int col;
    int idx;
    int cellRow;
    int uBase[4] = {154, 174, 174, 154};
    int vBase[4] = {48, 48, 1, 1};

    /* Read the work slot as a word, like menuCritWorkGet: a pointer-typed load
     * of task->work is hoisted above the table copies above. */
    work = (MenuObjWork *)*(u32 *)&task->work;
    originX = (int)work->transform.position.x;
    originY = (int)work->transform.position.z;
    originY += 101;
    originX -= 13;
    for (i = 0, point = ring[0]; i < 12; i++, point += 4) {
        point[0] = 0;
        point[1] = radius * xglSin(angleStep * (float)i);
        point[2] = -(radius * xglCos(angleStep * (float)i));
        point[3] = 1.0f;
    }
    xglMatrixUnit(mtx);
    rot[0] = ETRot - 0.7853982f;
    rot[1] = 0;
    rot[2] = 0;
    rot[3] = 1.0f;
    grRotMatrix(mtx, mtx, rot);
    /* Store-back of an unchanged element: reload_cse deletes it, but until then
     * it holds the memory slot that decides where the -9 and 16777212 constants
     * are born. */
    {
        float v = mtx[0][0];
        mtx[0][0] = v;
    }
    vtx.col[0] = 0x80;
    vtx.col[1] = 0x80;
    vtx.col[2] = 0x80;
    vtx.col[3] = 0x80;
    /* Each cell's four vertices; its UVs come from its row and column in the sheet. */
    for (i = 0; i < 12; i++) {
        for (j = 0; j < 4; j++) {
            row = j >> 1;
            col = j & 1;
            idx = (i + row) % 12;
            cellRow = i / 3;
            corner.x = halfWidth;
            if (col == 0) {
                corner.x = -halfWidth;
            }
            corner.y = ring[idx][1];
            corner.z = ring[idx][2];
            corner.w = ring[idx][3];
            xglVectorMulMat(pos, mtx, &corner);
            dst = &vtx.vertex[j];
            dst->x = (float)originX + pos[0];
            dst->y = (float)originY + pos[1];
            dst->z = 16777212.0f;
            vtx.u[j] = (float)(uBase[cellRow] + col * 18);
            vtx.v[j] = (float)(vBase[cellRow] + i % 3 * cellSize) + (float)(row * cellSize);
        }
        if (vtx.vertex[0].y < vtx.vertex[2].y) {
            grOpenFT4(packet);
            grPutFT4(packet, &vtx, 0);
            grCloseFT4();
        }
    }
}

/*
 * Sets the event timer, picks its X tier from a random roll, and sets the
 * ring's target rotation (a quarter turn per timer step).
 */
void menuTimeSet(int time) {
    int timerRoll;

    eventTimer = time;
    timerRoll = rnd(100);
    if (timerRoll >= 0x60) {
        eventTimerX = 10;
    } else if (timerRoll >= 0x3D) {
        eventTimerX = 4;
    } else {
        eventTimerX = 2;
    }

    ETRotDist = (float)eventTimer * 1.5707964f;
    if (ETRotDist > 6.2831855f) {
        ETRotDist -= 6.2831855f;
    }
}

/* Advances the event timer through 0..3, updates the ring target, and returns the timer. */
int menuTimeNext(void) {
    eventTimer++;
    if (eventTimer >= 4) {
        eventTimer = 0;
    }
    menuTimeSet(eventTimer);
    return eventTimer;
}

int menuTimeGet(void) {
    return eventTimer;
}

int menuTimeXGet(void) {
    return eventTimerX;
}

extern BattleUnit *actUnitGet(void);
void grGpInit(GrPacket *packet);
void menuPlStatPut(ObjectTask *task, GrPacket *packet, BattleUnit *unit);
void menuStatNamePut(ObjectTask *task);

void menuStatObjDraw(ObjectTask *task) {
    GrPacket packet;
    MenuStatObjWork *work;
    BattleUnit *unit;

    grGpInit(&packet);
    work = task->work;
    unit = work->unit;
    menuPlStatPut(task, &packet, unit);
    grPacketSend(&packet);
    if (unit == actUnitGet()) {
        menuStatNamePut(task);
    }
}


/*
 * Returns the unit's charaId minus 1. An AGWS unit, or a non-AGWS unit with a
 * charaId in 0xBB..0xC2, reports its linked unit's charaId instead.
 */
int menuStatChrIdGet(BattleUnit *unit)
{
    int charaId;
    CalcUnitParam *up;

    up = calcUPGet(unit);
    charaId = up->charaId;
    if (!(calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS)) {
        if (calcUPGet(unit)->charaId >= 0xBB) {
            if (calcUPGet(unit)->charaId < 0xC3) {
                charaId = up->linkedUnit->charaId;
            }
        }
    } else {
        charaId = up->linkedUnit->charaId;
    }
    return charaId - 1;
}

/* calc.c: nonzero while the unit's HP sits in the low range. */
extern int calcIdol2Chk(BattleUnit *unit);
void menuFaceEnvMake(void *packet);
void menuApBarPut(GrPacket *packet, int x, int y, int amount, BattleUnit *unit);
void menuBpBarPut(GrPacket *packet, int x, int y, int length);
void menuStatIconPut(GrPacket *packet, int x, int y, int charaId, int statFlags);

/*
 * Draws one party member's status window at the work position: the frame,
 * the face, the value labels, HP/EP, the AP and BP bars, and the status icon
 * when an icon slot is selected.
 */
void menuPlStatPut(ObjectTask *task, GrPacket *packet, BattleUnit *unit)
{
    /* The spilled locals keep the original's slots in declaration order
     * (0x10 up, 0x14 charaId, 0x18 iconSlot, 0x1C ap, 0x20 ep, 0x24 bp). */
    CalcUnitParam *up;
    MenuStatObjWork *work;
    int charaId;
    int iconSlot;
    float *ap;
    float *hp;
    float *ep;
    float *bp;
    int x;
    int z;
    int chrId;
    int faceU;
    int faceV;
    int pulse;

    up = calcUPGet(unit);
    work = task->work;
    z = work->display.position.z;
    x = work->display.position.x;
    hp = &work->display.hp;
    ep = &work->display.ep;
    bp = &work->display.bp;
    charaId = work->charaId;
    iconSlot = work->iconSlot;
    /* Last read of work: the scheduler hoists the instruction that frees it. */
    ap = &work->display.ap;
    /* x is reassigned rather than written as (int)x - 8 inline: the original
     * keeps x - 8 live and adds every window offset to it, while the
     * one-expression form folds the -8 into each offset. */
    x -= 8;
    chrId = menuStatChrIdGet(unit);
    faceU = (chrId % 6) * 40;
    faceV = (chrId / 6) * 80;
    menuFaceEnvMake(packet);
    pulse = (frameCntGet() * 2) & 0xFF;
    if (pulse >= 0x80) {
        pulse = 0xFF - pulse;
    }
    grGsRegSet(packet, SCE_GS_ALPHA_1, ((long long)(pulse + 10) << 32) | 0x68);
    grOpenSpr(packet);
    if (actUnitGet() == unit) {
        menuStatSprite2(packet, x + 0xC, z - 0xC, 0xA0, 0x58, 0xC8, 0xA0, 0xFF, 0xF0);
    }
    grCloseSpr();
    grGsRegSet(packet, SCE_GS_ALPHA_1, 0);
    grOpenSpr(packet);
    menuStatSprite(packet, x + 0x10, z - 8, 0x28, 0x50, faceU, faceV);
    grCloseSpr();
    menuStatEnvMake(packet);
    grOpenSpr(packet);
    menuStatSprite(packet, x + 0x30, z, 0x12, 0xC, 0, 0xAE);
    menuStatSprite(packet, x + 0x30, z + 0x10, 0x12, 0xC, 0x12, 0xAE);
    menuStatSprite(packet, x + 0x30, z + 0x22, 0x12, 0xC, 0x24, 0xAE);
    menuStatSprite(packet, x + 0x30, z + 0x32, 0x12, 0xC, 0x36, 0xAE);
    menuNumPut(packet, x + 0x42, z, (int)*hp, 4, (calcIdol2Chk(unit) != 0) ? 2 : 0);
    menuStatSprite(packet, x + 0x76, z + 2, 8, 0x10, 0x7A, 0x90);
    menuNumPut(packet, x + 0x7E, z + 2, up->maxHp, 4, 1);
    menuNumPut(packet, x + 0x5A, z + 0x11, (int)*ep, 2, 0);
    menuStatSprite(packet, x + 0x76, z + 0x13, 8, 0x10, 0x7A, 0x90);
    menuNumPut(packet, x + 0x7E, z + 0x13, up->maxEp, 2, 1);
    menuApBarPut(packet, x + 0x42, z + 0x22, (int)*ap, unit);
    menuStatSprite(packet, x + 0x44, z + 0x30, 0x3C, 0x10, 0x30, 0xF0);
    menuNumPut(packet, x + 0x82, z + 0x30, up->bcValue, 1, 1);
    grCloseSpr();
    menuBpBarPut(packet, x, z, (int)*bp);
    if (iconSlot != 0) {
        menuStatIconEnvMake(packet);
        grOpenSpr(packet);
        menuStatIconPut(packet, x, z, charaId, iconSlot);
        grCloseSpr();
    }
}

typedef struct NumPutCol {
    MenuRGBA c[2];
} NumPutCol;
extern const NumPutCol numPutCol;

void menuNumPut(GrPacket *packet, int x, int y, int value, int digits, int style)
{
    NumPutCol col = numPutCol;
    int colorRow;
    int srcY;
    int width;
    int advance;
    int height;
    int scale;
    int digit;
    int i;

    colorRow = 0;
    if (style == 1) {
        srcY = 160;
        width = 10;
        advance = 10;
        height = 13;
    } else if (style == 2) {
        srcY = 144;
        width = 12;
        advance = 12;
        height = 16;
        colorRow = 1;
    } else if (style == 3) {
        advance = 10;
        for (i = 0; i < 4; i++) {
            menuStatSpriteRGB(packet, x + i * advance, y, 10, 13, advance * 10, 160, &col.c[colorRow]);
        }
        return;
    } else {
        srcY = 144;
        width = 12;
        advance = 12;
        height = 16;
    }
    scale = 1;
    for (i = 0; i < digits - 1; i++) {
        scale *= 10;
    }
    for (i = 0; i < digits; i++) {
        digit = value / scale;
        value %= scale;
        menuStatSpriteRGB(packet, x + i * advance, y, width, height, width * digit, srcY, &col.c[colorRow]);
        scale /= 10;
    }
}

/*
 * Draws the six-cell AP bar; a cell shows its filled source while amount is
 * still positive. The unit argument is passed by menuPlStatPut but never read.
 */
void menuApBarPut(GrPacket *packet, int x, int y, int amount, BattleUnit *unit)
{
    int i;
    int xPosition;

    xPosition = x;
    for (i = 0; i < 6; i++) {
        int sourceX;

        sourceX = 24;
        if (amount > 0) {
            sourceX = 0;
        }
        menuStatSprite(packet, xPosition, y, 24, 16, sourceX, 240);
        amount--;
        xPosition += 16;
    }
}

/* grPutF4's quad: four corners and one colour at +0x40. */
typedef struct GrF4Vertices {
    Vector4 points[4]; /* +0x00 */
    MenuRGBA color;    /* +0x40 */
} GrF4Vertices;

extern void grOpenF4(GrPacket *packet);
extern void grPutF4(GrPacket *packet, GrF4Vertices *quad, const Matrix4 matrix);
extern void grCloseF4(void);

/*
 * The BP bar: a quad 4 pixels high and length pixels long, at
 * (x + 0x49, y + 0x36), in the fixed colour (0xFF, 0x20, 0x80).
 */
void menuBpBarPut(GrPacket *packet, int x, int y, int length)
{
    GrF4Vertices quad;

    grOpenF4(packet);
    quad.color.r = 0xFF;
    quad.color.g = 0x20;
    quad.color.b = 0x80;
    quad.color.a = 0x80;
    quad.points[0].x = x + 0x49;
    quad.points[0].y = y + 0x36;
    quad.points[0].z = SPRITE_Z;
    quad.points[1].x = x + length + 0x49;
    quad.points[1].y = y + 0x36;
    quad.points[1].z = SPRITE_Z;
    quad.points[2].x = x + 0x49;
    quad.points[2].y = y + 0x3A;
    quad.points[2].z = SPRITE_Z;
    quad.points[3].x = x + length + 0x49;
    quad.points[3].y = y + 0x3A;
    quad.points[3].z = SPRITE_Z;
    grPutF4(packet, &quad, 0);
    grCloseF4();
}

/*
 * Index of the first icon entry of the character's table whose flags share a
 * bit with statFlags (a status flag mask), or -1 when none does.
 */
int menuStatIconChk(int charaId, int statFlags)
{
    StatIconEntry *icons;
    int index;

    icons = statTbl[charaId];
    if (icons == 0 || statFlags == 0) {
        return -1;
    }
    for (index = 0; icons[index].flags != 0; index++) {
        if (icons[index].flags & statFlags) {
            return index;
        }
    }
    return -1;
}

int menuStatIconChk(int charaId, int statFlags);

void menuStatIconPut(GrPacket *packet, int x, int y, int charaId, int statFlags)
{
    StatIconEntry *icons;
    int index;

    icons = statTbl[charaId];
    index = menuStatIconChk(charaId, statFlags);
    if (index != -1) {
        menuStatSprite(packet, x + 0x90, y + 0x32, 0x16, 0x18, icons[index].srcX,
                       icons[index].srcY);
    }
}

/*
 * Argument record of eBattleWinOpen2. flags and target are confirmed for this
 * callee; the other field names follow the sibling window's record
 * (BattleMsgOpenParams in src/ov01/cmd.c). iconGraphic is an extra word that
 * this call adds.
 */
struct StatNameOpenParams {
    int textId;                    /* +0x00: 8 at this call site */
    int speaker;                   /* +0x04: 0x68 at this call site */
    int flags;                     /* +0x08: 0x00FFFFFD */
    int priority;                  /* +0x0C: 0x800000B0 at this call site */
    const char *target;            /* +0x10: dataStatNameGet's resolved name */
    const char *iconGraphic;       /* +0x14: menuStatusText */
};
void eBattleWinOpen2(struct StatNameOpenParams *params);
int menuStatIconChk(int charaId, int iconSlot);
const char menuStatusText[8] = "Status";
extern void eBattleWinMain2(void);

/* Opens the status-name window for the unit's icon while the battle menu is open. */
void menuStatNamePut(ObjectTask *task)
{
    struct StatNameOpenParams params;
    MenuStatObjWork *work;
    int charaId;
    int icon;

    work = task->work;
    charaId = work->charaId;
    icon = menuStatIconChk(charaId, work->iconSlot);
    if (icon != -1 && menuOpenChkBat() != 0) {
        params.textId = 8;
        params.speaker = 0x68;
        params.flags = 0xFFFFFD;
        params.priority = 0x800000B0;
        params.target = *dataStatNameGet(charaId, icon);
        params.iconGraphic = menuStatusText;
        eBattleWinOpen2(&params);
        eBattleWinMain2();
    }
}

/*
 * One line of a key guide list: the pad button icon (uvTbl index, + 4 for its
 * dimmed variant) and the key command it triggers; a 0 button ends the list.
 */
typedef struct KeyGuideLine {
    int button; /* +0x00 */
    int key;    /* +0x04 */
} KeyGuideLine;

/*
 * One row of keyGuideTblChr/keyGuideTblAgws (8 bytes): the guide lines and the
 * 0-terminated button icon list of one guide mode.
 */
typedef struct KeyGuideMode {
    KeyGuideLine *lines; /* +0x00 */
    int *buttons;        /* +0x04 */
} KeyGuideMode;

static KeyGuideMode keyGuideTblChr[13];
static KeyGuideMode keyGuideTblAgws[13];

/* The battle actor flag the key guide requires. */
#define ACTOR_FLAG_KEY_GUIDE 0x8u

/* Control phase with no guide. */
#define KEY_GUIDE_PHASE_NONE 0x1A
/* First control phase that needs calcTuigekiChk. */
#define KEY_GUIDE_PHASE_TUIGEKI 0xB
/* Key command without a check. */
#define KEY_CMD_NO_CHECK 0x1C

extern int menuChk(void);
extern int calcTuigekiChk(BattleUnit *unit);
extern int keyCmdChk(BattleUnit *unit, BattleUnit *target, int key);
extern int battleCtrlHelpGet(void);
void keyGideDisp(GrPacket *packet, int x, int y, int button);
void menuKeyGuideHelp(GrPacket *packet, BattleUnit *unit, int key, int x, int y);
void menuKeyGuideVal(GrPacket *packet, BattleUnit *unit, int key, int x, int y);

/*
 * Draws the key guide of phase for unitTask: the button icons of the phase in
 * a row, then each guide line with its button (dimmed while keyCmdChk gives
 * 1, skipped for 2 or more or a negative result) and, unless the help display
 * is in state 2, its help text and values; then the guide footer. guideTask is
 * the key guide object's own task, which holds the draw position. Each table
 * is read in its own branch, lines before buttons, and both lists are walked
 * by index, as the original's code shape requires.
 */
void menuKeyGuideDisp(ObjectTask *guideTask, BattleUnit *unitTask, int phase)
{
    GrPacket packet;
    MenuObjWork *work;
    BattleUnit *target;
    KeyGuideLine *lines;
    int *buttons;
    int x;
    int y;
    int i;
    int check;
    int icon;

    if (unitTask == 0) {
        return;
    }
    if ((unitTask->work->flags & ACTOR_FLAG_KEY_GUIDE) == 0) {
        return;
    }
    if (menuChk() != 0) {
        return;
    }
    if (phase == KEY_GUIDE_PHASE_NONE) {
        return;
    }
    if (phase >= KEY_GUIDE_PHASE_TUIGEKI && calcTuigekiChk(unitTask) == 0) {
        return;
    }
    work = guideTask->work;
    x = (int)work->transform.position.x;
    y = (int)work->transform.position.z;
    grGpInit(&packet);
    menuStatEnvMake(&packet);
    target = tgtUnitGet();
    if (calcUPGet(unitTask)->flags & CALC_UNIT_FLAG_IS_AGWS) {
        lines = keyGuideTblAgws[phase].lines;
        buttons = keyGuideTblAgws[phase].buttons;
    } else {
        lines = keyGuideTblChr[phase].lines;
        buttons = keyGuideTblChr[phase].buttons;
    }
    if (buttons != 0) {
        for (i = 0; buttons[i] != 0; i++) {
            keyGideDisp(&packet, x, y, buttons[i]);
            x += 0x18;
        }
    }
    if (lines != 0) {
        for (i = 0; lines[i].button != 0; i++) {
            if (lines[i].key == 0 || lines[i].key == KEY_CMD_NO_CHECK) {
                check = 0;
            } else {
                check = keyCmdChk(unitTask, target, lines[i].key);
            }
            if (check < 2) {
                if (check >= 0) {
                    if (check == 0) {
                        icon = lines[i].button;
                    } else {
                        icon = lines[i].button + 4;
                    }
                    keyGideDisp(&packet, x, y, icon);
                    if (battleCtrlHelpGet() != 2) {
                        menuKeyGuideHelp(&packet, unitTask, lines[i].key, x, y);
                        menuKeyGuideVal(&packet, unitTask, lines[i].key, x, y);
                    }
                    x += 4;
                    y += 0x1C;
                }
            }
        }
    }
    grOpenSpr(&packet);
    menuStatSprite(&packet, 8, 0x1A8, 0xB2, 0xB, 0x4C, 0xAF);
    grCloseSpr();
    grPacketSend(&packet);
}

/*
 * One pad button icon's cell in the menu sheet: keyGideDisp passes its
 * srcX/srcY and width/height to menuStatSprite.
 */
typedef struct UvTblEntry {
    int srcX;   /* +0x00 */
    int srcY;   /* +0x04 */
    int width;  /* +0x08 */
    int height; /* +0x0C */
} UvTblEntry;

static UvTblEntry uvTbl[8];

/*
 * Draws pad button icon button (1-based: uvTbl[0] is button 1) at (x, y).
 * The index is decremented in its own statement, as the original does before
 * the table index.
 */
void keyGideDisp(GrPacket *packet, int x, int y, int button)
{
    grOpenSpr(packet);
    button--;
    menuStatSprite(packet, x, y, uvTbl[button].width, uvTbl[button].height, uvTbl[button].srcX,
                   uvTbl[button].srcY);
    grCloseSpr();
}

/* The guide frame's colour table, three RGBA entries. */
typedef struct GuideFrameCol {
    MenuRGBA c[3];
} GuideFrameCol;
extern const GuideFrameCol guideFrameCol;

void menuGuideFrameDisp(GrPacket *packet, int x, int y, int width, int sel)
{
    GuideFrameCol col;

    col = guideFrameCol;
    grGsRegSet(packet, SCE_GS_ALPHA_1, 0x2000000029ULL);
    grOpenSpr(packet);
    menuStatSprite2RGB(packet, x, y, width, 0xC, 0x84, 0xC4, 0xBC, 0xCF, &col.c[sel]);
    menuStatSprite2RGB(packet, x + width, y, width + 1, 0xC, 0xBC, 0xC4, 0x84, 0xCF, &col.c[sel]);
    menuStatSprite2RGB(packet, x, y + 0xC, width, 0xC, 0x84, 0xCF, 0xBC, 0xC4, &col.c[sel]);
    menuStatSprite2RGB(packet, x + width, y + 0xC, width + 1, 0xC, 0xBC, 0xCF, 0x84, 0xC4, &col.c[sel]);
    grCloseSpr();
}

extern const char **dataEnemyNameGet(int index);
extern const char **dataPlayerNameGet(int id);
extern unsigned int strlen(const char *s);
void menuGuideFrameDisp(GrPacket *packet, int x, int y, int width, int sel);
void menuFontEnvMake(void *packet);
void menuFontPut(GrPacket *packet, int x, int y, const char *text);

/*
 * Draws a unit's name: a frame, then the name right-aligned by its
 * length. isEnemy selects the enemy or the player name table.
 */
void monsNameDisp(void *packet, BattleUnit *unitTask, int x, int y, int isEnemy)
{
    const char *name;
    int len;

    menuStatEnvMake(packet);
    menuGuideFrameDisp(packet, x, y, 0x50, 2);
    x = x + 2;
    y = y + 3;
    menuFontEnvMake(packet);
    if (isEnemy == 1) {
        name = *dataEnemyNameGet(calcUPGet(unitTask)->charaId);
    } else {
        name = *dataPlayerNameGet(calcUPGet(unitTask)->charaId);
    }
    len = strlen(name);
    menuFontPut(packet, x + (0x10 - len) * 5, y, name);
}

/*
 * dataTecGet's record, as far as menuKeyGuideHelp and menuKeyGuideVal read it.
 */
typedef struct MenuTecData {
    unsigned char attr;     /* +0x00: menuKeyGuideHelp's `& 3` picks atr1Name */
    unsigned char unmodeled_01[0x05 - 0x01];
    signed char extraRow;   /* +0x05: nonzero gives the key guide frame its second row */
    unsigned char unmodeled_06[0x11 - 0x06];
    signed char kind;       /* +0x11: compared with TEC_KIND_FIXED_AP */
    unsigned short attr2;   /* +0x12: masked by 0xE0 (atr2Name) and 0x1F in menuKeyGuideHelp; name not established */
} MenuTecData;

/*
 * dataWpnGet's record, as far as menuKeyGuideVal reads it.
 */
typedef struct MenuWpnData {
    unsigned char unmodeled_00[0x0F];
    signed char apCost;     /* +0x0F */
    unsigned char equipped; /* +0x10 */
    unsigned char unmodeled_11[0x14 - 0x11];
    short tecId;            /* +0x14: calcTakeEpGet's argument */
} MenuWpnData;

extern MenuTecData *dataTecGet(int tecId);

extern const char **dataNrmNameGet(int tecId);
extern const char **dataSpcNameGet(int tecId);
extern const char **dataWepNameGet(int weaponId);
static const char *atr1Name[3];
static const char *atr2Name[4];
static const char *atr3Name[6];
extern const char menuJpText2[];
extern const char menuJpText3[];
extern const char menuEndText[];
extern const char menuText[];
extern int keyCmd2wno(BattleUnit *unit, int key);
extern int keyCmd2tid(BattleUnit *unit, int key);

/*
 * Draws the help text of a key guide command: the technique or weapon name,
 * or the attribute string when help mode is on.
 */
void menuKeyGuideHelp(GrPacket *packet, BattleUnit *task, int cmd, int x, int y) {
    char sp[32];
    const char *text;
    CalcUnitParam *up;
    int tx;
    int ty;
    int tid;
    int sel;
    int h;
    int a;
    int i;
    int b;

    tx = x + 0x18;
    if (cmd != 0 && cmd != 0x1C) {
        sel = dataTecGet(keyCmd2tid(task, cmd))->extraRow != 0;
    } else {
        sel = 2;
    }
    ty = y + 3;
    menuGuideFrameDisp(packet, tx, y, 0x56, sel);
    menuFontEnvMake(packet);
    tx += 2;
    text = menuEndText;
    if (cmd != 0 && cmd != 0x1C) {
        tid = keyCmd2tid(task, cmd);
        text = menuJpText2;
        if (battleCtrlHelpGet() == 0) {
            if ((unsigned)(tid - 1) < 0x2C) {
                text = *dataNrmNameGet(tid);
            } else if ((unsigned)(tid - 0x2D) < 0x36) {
                text = *dataSpcNameGet(tid);
            } else if ((unsigned)(tid - 0x2DC) < 0x8C) {
                up = calcUPGet(task);
                text = *dataWepNameGet(up->weaponId[keyCmd2wno(task, cmd)]);
            }
        } else {
            /* Help mode: builds the attribute string from the technique's fields. */
            sp[0] = 0;
            text = sp;
            b = dataTecGet(tid)->attr & 3;
            if (b == 1) {
                i = 1;
                goto atr1_have;
            }
            if (b < 2) {
                if (b == 0) {
                    i = 0;
                    goto atr1_have;
                }
            }
            i = 2;
atr1_have:;
            strcat(sp, atr1Name[i]);
            strcat(sp, menuJpText3);
            a = dataTecGet(tid)->attr2 & 0xE0;
            switch (a) {
            case 0x40:
                i = 1;
                break;
            case 0x20:
                i = 2;
                break;
            case 0x80:
                i = 0;
                break;
            default:
                i = 3;
                break;
            }
            strcat(sp, atr2Name[i]);
            strcat(sp, menuJpText3);
            h = dataTecGet(tid)->attr2 & 0x1F;
            switch (h) {
            case 16:
                i = 0;
                break;
            case 8:
                i = 1;
                break;
            case 4:
                i = 2;
                break;
            case 2:
                i = 3;
                break;
            case 1:
                i = 4;
                break;
            default:
                i = 5;
                break;
            }
            strcat(sp, atr3Name[i]);
        }
    } else if (cmd == 0x1C) {
        text = menuText;
    }
    menuFontPut(packet, tx + ((0x11 - strlen(text)) * 5), ty, text);
    menuStatEnvMake(packet);
}

/* keyCmd2wno and keyCmd2tid (ov01 battle_init.c) give the weapon slot and the
 * technique id a key command selects for a unit; -1 means no weapon slot. */

extern MenuWpnData *dataWpnGet(int weaponId);

/* Technique kind whose AP cost is fixed at 6 instead of the weapon's. */
#define TEC_KIND_FIXED_AP 6

/*
 * The value panel of one key guide entry: the weapon slot's count, the EP
 * cost and the AP cost of the command bound to key. For a unit without
 * CALC_UNIT_FLAG_IS_AGWS, a technique command shows only for TEC_KIND_FIXED_AP
 * techniques. The frame position is a variable stepped to the value row
 * (valueX += 8, valueY += 6), as the original does. The AP cell sits in a
 * do { } while (0) block: the original calls grCloseSpr without a tail call
 * and sets the sprite's arguments in order; a plain statement list gives a
 * tail call and another argument order.
 */
void menuKeyGuideVal(GrPacket *packet, BattleUnit *unit, int key, int x, int y)
{
    MenuWpnData *weapon;
    int slot;
    int tecId;
    int apCost;
    int valueX;
    int valueY;

    slot = keyCmd2wno(unit, key);
    if (slot == -1) {
        return;
    }
    tecId = keyCmd2tid(unit, key);
    if ((calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS) == 0) {
        if (tecId == 0) {
            return;
        }
        if (dataTecGet(tecId)->kind != TEC_KIND_FIXED_AP) {
            return;
        }
    }
    weapon = dataWpnGet(calcUPGet(unit)->weaponId[slot]);
    valueX = x + 0xC6;
    valueY = y;
    menuGuideFrameDisp(packet, valueX, valueY, 0x38, dataTecGet(tecId)->extraRow != 0);
    valueX += 8;
    valueY += 6;
    grGsRegSet(packet, SCE_GS_ALPHA_1, 0);
    grOpenSpr(packet);
    if (weapon->equipped != 0) {
        menuNumPut(packet, valueX, valueY, calcUPGet(unit)->wpnSlotValue[slot], 2, 1);
    }
    menuStatSprite(packet, x + 0xE6, valueY, 0x12, 0xC, 0x12, 0xAE);
    menuNumPut(packet, x + 0xF8, valueY, calcTakeEpGet(unit, weapon->tecId), 2, 1);
    if (dataTecGet(tecId)->kind == TEC_KIND_FIXED_AP) {
        apCost = 6;
    } else {
        apCost = weapon->apCost;
    }
    do {
        menuStatSprite(packet, valueX + 0x42, valueY, 0x12, 0xC, 0x24, 0xAE);
        menuNumPut(packet, valueX + 0x54, valueY, apCost, 1, 1);
        grCloseSpr();
    } while (0);
}

/* obj.c (ov01/tu001): allocates a task from the free list and stores the
 * caller's callback into its ->exec slot. */
extern ObjectTask *objEntry2(void *argument, void (*callback)(ObjectTask *task));
extern void objStdInit(ObjectTask *task);

void objDmgNum(ObjectTask *task);
void objDmgNumDraw(ObjectTask *task);

/* The work block of an objDmgNum/objDmgNumDraw task. Only the fields that
 * objDmgNumCreate writes are known; the span before them is not used here. */
typedef struct {
    unsigned char unmodeled_00[0x70];
    int countdown;         /* +0x70: objDmgNumCreate always sets 30 */
    int value;              /* +0x74 */
    int flags;               /* +0x78 */
    int dy;                 /* +0x7C: objDmgNum stores the vertical offset here */
    void *target;           /* +0x80 */
    unsigned char unmodeled_84[0x90 - 0x84];
    Vector4 position;       /* +0x90: objDmgNum passes it to sefGetNullPosition */
} DmgNumWork;

/* Creates a damage number task for target that shows value; the low byte of flags selects the display type, the next byte the message icon. */
ObjectTask *objDmgNumCreate(void *target, int value, int flags)
{
    ObjectTask *task;
    DmgNumWork *work;

    task = objEntry2(objDmgNum, objDmgNumDraw);
    if (task != 0) {
        work = task->work;
        objStdInit(task);
        work->target = target;
        work->value = value;
        work->flags = flags;
        work->countdown = 30;
        return task;
    }
}

extern void sefGetNullPosition(Vector4 *position, void *actor, int part, int flag);
extern int grBBCalc(ObjectTask *task, int *screenX, int *screenY);

/*
 * Moves the damage number to the target's null position with the vertical
 * offset for the current countdown step, and removes the task when it ends.
 */
void objDmgNum(ObjectTask *task)
{
    int ofs[30] = {
        0, 8, 14, 8, 0, -8, -14, -3, 0, 3, 6, 5, 0, -3, -5, -3,
        0, 3, 4, 2, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0,
    };
    DmgNumWork *work;

    /* Codegen-only statement: it keeps the work load after the table copy, as the original orders it. */
    { float _sb = task != 0; if (_sb) {} }
    work = task->work;
    sefGetNullPosition(&work->position, *(void **)((char *)work->target + 0x14), 15, 0);
    work->dy = ofs[30 - work->countdown];
    if (work->countdown-- <= 0) {
        objRemove(task);
    }
}

void dmgMsgPut(GrPacket *packet, int x, int y, int id);
void dmgNumPut(GrPacket *packet, int x, int y, int num, int type);

/* Draws the damage number and its message icon while grBBCalc returns nonzero for the task. */
void objDmgNumDraw(ObjectTask *task)
{
    GrPacket packet;
    DmgNumWork *work;
    int x;
    int y;
    int flags;
    int type;
    int bits;
    int ofs;
    int id;
    int dy;

    work = task->work;
    if (grBBCalc(task, &x, &y) != 0) {
        grGpInit(&packet);
        menuStatTexTrans();
        menuStatEnvMake(&packet);
        grOpenSpr(&packet);
        flags = work->flags;
        type = flags & 0xFF;
        ofs = work->countdown - 8;
        bits = (flags >> 8) & 0xFF;
        ofs = (ofs >= 0) ? -ofs : 0;
        dy = work->dy;
        switch (type) {
        case 0:
            dmgMsgPut(&packet, x, y + dy, 2);
            break;
        case 1: case 2: case 3: case 4: case 7: case 8: case 9: case 10:
            dmgNumPut(&packet, x, y + dy, work->value, type);
            break;
        }
        id = -1;
        if (bits & 1) {
            id = 0;
        } else if (bits & 2) {
            id = 3;
        } else if (bits & 4) {
            id = 1;
        } else if (bits & 8) {
            id = 4;
        }
        if (id != -1) {
            dmgMsgPut(&packet, x + ofs, y, id);
        }
        grCloseSpr();
        grPacketSend(&packet);
    }
}

/* Five message cells, copied whole before indexing. */
typedef struct DmgMsgUVTbl {
    UvTblEntry e[5];
} DmgMsgUVTbl;
extern const DmgMsgUVTbl dmgMsgUVTbl;

void dmgMsgPut(GrPacket *packet, int x, int y, int id)
{
    DmgMsgUVTbl tbl = dmgMsgUVTbl;

    menuStatSprite(packet, x - 0x12, y - 0xE, tbl.e[id].width, tbl.e[id].height, tbl.e[id].srcX, tbl.e[id].srcY);
}

void menuStatSpriteRGB(GrPacket *packet, int x, int y, int width, int height, int srcX, int srcY, const MenuRGBA *rgba);

/* Draws num as up to four decimal digits, centred on x. */
void dmgNumPut(GrPacket *packet, int x, int y, int num, int type)
{
    MenuRGBA col[4] = {{0xFF, 0xFF, 0xFF, 0x80}, {0x00, 0xFF, 0x00, 0x80}, {0x00, 0xFF, 0xFF, 0x80}, {0x00, 0xFF, 0x00, 0x80}};
    int place;
    int divisor;
    int digits;
    int colorRow;
    int penX;
    int digitU;
    int slotX;
    int digitWidth;

    digitWidth = 0xF;
    place = 1;
    divisor = 1000;
    digits = 0;
    do {
        if (num >= place) {
            digits += 1;
        }
        place *= 10;
    } while (place <= divisor);
    if (digits == 0) {
        digits = 1;
    }
    switch (type) {
    case 1:
        colorRow = 0;
        break;
    case 2:
        colorRow = 1;
        break;
    case 3:
    case 9:
    case 10:
        colorRow = 2;
        break;
    case 4:
        colorRow = 3;
        break;
    default:
        colorRow = 0;
        break;
    }
    place = 4;
    penX = x - ((digits * digitWidth) / 2);
    do {
        digitU = (num / divisor) * digitWidth;
        num = num % divisor;
        if (digits >= place) {
            slotX = penX;
            penX += digitWidth;
            menuStatSpriteRGB(packet, slotX, y, digitWidth, 0x16, digitU, 0x6A, &col[colorRow]);
        }
        place -= 1;
        divisor = divisor / 10;
    } while (place > 0);
}

/* Returns the index of text[0] in the single-byte font table, or -1 when it is absent. */
int menuFontIdxGet(const signed char *text)
{
    static const signed char *font = menu0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzText;
    int index;

    for (index = 0; index < 230; index++) {
        if (text[0] == font[index]) {
            return index;
        }
    }
    return -1;
}

/* Returns 175 plus the index of the two-byte glyph at text in the table menuJpText4, or -1 when absent. */
int menuDFontIdxGet(const signed char *text)
{
    static const signed char *font = menuJpText4;
    int index;

    for (index = 0; index < 230; index++) {
        if (text[0] == font[index * 2] && text[1] == font[index * 2 + 1]) {
            return index + 175;
        }
    }
    return -1;
}


/* The font sheet is a 25-column grid of 0xA x 0x12 (10x18) glyph cells. */
#define FONT_GLYPH_COLUMNS 25
#define FONT_GLYPH_WIDTH 0xA
#define FONT_GLYPH_HEIGHT 0x12

void menuFontPutSub(GrPacket *packet, int x, int y, int charCode) {
    int row;
    int col;

    row = charCode / FONT_GLYPH_COLUMNS;
    col = charCode % FONT_GLYPH_COLUMNS;
    menuStatSprite(packet, x, y, FONT_GLYPH_WIDTH, FONT_GLYPH_HEIGHT,
                   col * FONT_GLYPH_WIDTH, row * FONT_GLYPH_HEIGHT);
}

extern int sscanf(const char *string, const char *format, ...);
extern const char menuJpText5[]; /* "%X": menuFontPut parses a '#'-escaped hex glyph code */

/*
 * Draws text one glyph per FONT_GLYPH_WIDTH: "#XX" is a hex glyph code, a byte
 * with bit 7 set starts a two-byte glyph, anything else is a single-byte one.
 * The loop indexes text[consumed] and takes the glyph cursor at the top of
 * the body: the original tests text[0] through the text register and reloads
 * the byte through the cursor (a cursor-driven loop shares that load).
 */
void menuFontPut(GrPacket *packet, int x, int y, const char *text)
{
    char hex[3];
    u32 glyph; /* sscanf "%X" target; menuFontIdxGet/menuDFontIdxGet store
                * their index, -1 for none, into the same slot */
    int consumed;
    const char *cursor;

    grOpenSpr(packet);
    for (consumed = 0; text[consumed] != '\0'; consumed++, x += FONT_GLYPH_WIDTH) {
        cursor = &text[consumed];
        if (*cursor == '#') {
            hex[0] = cursor[1];
            consumed += 2;
            hex[1] = cursor[2];
            hex[2] = '\0';
            sscanf(hex, menuJpText5, &glyph);
        } else if (*cursor & 0x80) {
            consumed++;
            glyph = menuDFontIdxGet((const signed char *)cursor);
        } else {
            glyph = menuFontIdxGet((const signed char *)cursor);
        }
        if ((int)glyph != -1) {
            menuFontPutSub(packet, x, y, (int)glyph);
        }
    }
    grCloseSpr();
}

/*
 * The cursor object's transform: the work's +0x90..+0x110 tail that grBBCalc
 * reads through the cursor task, with the same position and offset layout as
 * TransformState in src/ov01/gr_gp_init.c (position at +0x90, offset at
 * +0xa0). menuActCurPut saves and restores all 0x80 bytes with aligned copies,
 * so the storage is eight-byte aligned: the union gives it that alignment, as
 * in include/ov01/m_ef_create.h.
 */
typedef union MenuCursorTransform {
    struct {
        Vector4 position; /* +0x00 */
        Vector4 offset;   /* +0x10 */
        unsigned char unmodeled_20[0x60];
    } fields;
    u64 storage[0x80 / sizeof(u64)];
} MenuCursorTransform;

typedef struct MenuCursorWork {
    unsigned char unmodeled_00[0x90];
    MenuCursorTransform transform; /* +0x90 */
} MenuCursorWork;

/* Same prototype as in src/ov01/cmd.h and src/ov01/m_cam.c. */
extern void sefGetNullPosition(Vector4 *position, void *actor, int part, int flag);
/* Declared in src/ov01/gr_gp_init.c with a MatrixCommand * parameter; the cursor task is passed as is. */
extern int grBBCalc(ObjectTask *task, int *screenX, int *screenY);

/*
 * Draws the action cursor over a player unit: the cursor's transform is
 * moved to the unit's null position, raised by 1.3, projected to the screen,
 * and restored afterwards. Enemy units (ACTOR_FLAG_ENEMY) get none.
 */
void menuActCurPut(GrPacket *packet, ObjectTask *cursor, BattleUnit *unit, int frame)
{
    MenuCursorTransform saved;
    int screenX;
    int screenY;
    MenuCursorTransform *transform;

    if (unit == 0 || (unit->work->flags & ACTOR_FLAG_ENEMY)) {
        return;
    }
    transform = &((MenuCursorWork *)cursor->work)->transform;
    saved = *transform;
    sefGetNullPosition(&transform->fields.position, unit->actor, 0, 0);
    transform->fields.offset.y = 1.3f;
    if (grBBCalc(cursor, &screenX, &screenY) != 0) {
        menuStatEnvMake(packet);
        grOpenSpr(packet);
        menuStatSprite(packet, screenX - 8, screenY - 8, 16, 16, (frame % 8) * 16, 0x80);
        grCloseSpr();
        grPacketSend(packet);
    }
    *transform = saved;
}

/* Adds a width x height sprite at (x, y) sampled from (srcX, srcY), in the neutral gray colour. */
void menuStatSprite(GrPacket *packet, int x, int y, int width, int height, int srcX, int srcY) {
    GrSprite sprite;
    MenuColor color;

    color.rgba = menuNeutralGray;
    sprite.x0 = x;
    sprite.y0 = y;
    sprite.z0 = SPRITE_Z;
    sprite.x1 = x + width;
    sprite.y1 = y + height;
    sprite.z1 = SPRITE_Z;
    sprite.u0 = srcX;
    sprite.u1 = srcX + width + 1;
    sprite.v0 = srcY;
    sprite.v1 = srcY + height + 1;
    sprite.color = color;
    grPutSpr(packet, &sprite, 0);
}

/* Adds a sprite at (x, y) sampled from the texture rectangle srcLeft..srcRight, srcTop..srcBottom, in the neutral gray colour. */
void menuStatSprite2(GrPacket *packet, int x, int y, int width, int height, int srcLeft, int srcTop, int srcRight, int srcBottom) {
    GrSprite sprite;
    MenuColor color;

    color.rgba = menuNeutralGray;
    sprite.u0 = srcLeft;
    sprite.x0 = x;
    sprite.y0 = y;
    sprite.v0 = srcTop;
    sprite.z0 = SPRITE_Z;
    sprite.x1 = x + width;
    sprite.y1 = y + height;
    sprite.z1 = SPRITE_Z;
    sprite.u1 = srcRight;
    sprite.v1 = srcBottom;
    sprite.color = color;
    grPutSpr(packet, &sprite, 0);
}

/* Same as menuStatSprite, with the colour taken from rgba. */
void menuStatSpriteRGB(GrPacket *packet, int x, int y, int width, int height, int srcX, int srcY, const MenuRGBA *rgba) {
    GrSprite sprite;

    sprite.x0 = x;
    sprite.y0 = y;
    sprite.z0 = SPRITE_Z;
    sprite.x1 = x + width;
    sprite.y1 = y + height;
    sprite.z1 = SPRITE_Z;
    sprite.u0 = srcX;
    sprite.u1 = srcX + width + 1;
    sprite.v0 = srcY;
    sprite.v1 = srcY + height + 1;
    sprite.color.rgba = *rgba;
    grPutSpr(packet, &sprite, 0);
}

/* Same as menuStatSprite2, with the colour taken from rgba. */
void menuStatSprite2RGB(GrPacket *packet, int x, int y, int width, int height, int srcLeft, int srcTop, int srcRight, int srcBottom, const MenuRGBA *rgba) {
    GrSprite sprite;

    sprite.u0 = srcLeft;
    sprite.v0 = srcTop;
    sprite.x0 = x;
    sprite.y0 = y;
    sprite.z0 = SPRITE_Z;
    sprite.x1 = x + width;
    sprite.y1 = y + height;
    sprite.z1 = SPRITE_Z;
    sprite.u1 = srcRight;
    sprite.v1 = srcBottom;
    /* The colour is copied as four bytes, since the caller's record may be unaligned. */
    __builtin_memcpy(&sprite.color, rgba, 4);
    grPutSpr(packet, &sprite, 0);
}

/* dataXtxFileGet's record: only the texture id at +0x4 is read here. */
extern int *dataXtxFileGet(int index);
extern void xglPacketTextureTrans(int textureId);

/* Transfers the texture from xtx entry 1 and flushes the texture cache. */
void menuTexTrans(void) {
    GrPacket packet;

    xglPacketTextureTrans(dataXtxFileGet(1)[1]);
    grGpInit(&packet);
    grGsRegSet(&packet, SCE_GS_TEXFLUSH, 1);
    grPacketSend(&packet);
}

/* Transfers the texture from xtx entry 0 and flushes the texture cache. */
void menuStatTexTrans(void) {
    GrPacket packet;

    xglPacketTextureTrans(dataXtxFileGet(0)[1]);
    grGpInit(&packet);
    grGsRegSet(&packet, SCE_GS_TEXFLUSH, 1);
    grPacketSend(&packet);
}

/* Defined in src/ov01/gr_gp_init.c. The six *EnvMake callers pass their packet
 * parameter as void *, which converts here. */
extern void GS_REG_SET(GrPacket *packet, long long register_id, long long control);
extern unsigned short dataXtxAdrGet(int index);

/*
 * The six *EnvMake functions below write one GIF packet each. The GS registers
 * and packed words are written as numbers:
 * - 0xE is the GIF A+D descriptor; its word is the GIFtag (NLOOP 5 for the five
 *   register writes that follow, EOP set, NREG 1);
 * - 0x06 TEX0_1: tbp0 is the texture base, tbp1 << 37 the CLUT base (CBP); the
 *   constant carries the buffer width, pixel formats, size and function bits;
 * - 0x42 ALPHA_1 (blend equation; menuFontEnvMake uses 0x44, the others 0),
 *   0x47 TEST_1 (0x3000D: alpha test on, depth test on), 0x08 CLAMP_1 and
 *   0x14 TEX1_1 both 0.
 * The dataXtxAdrGet indices select which texture/CLUT pair each environment
 * binds.
 */

void menuEnvMake(void *packet)
{
    unsigned short tbp0 = dataXtxAdrGet(0xD);
    unsigned short tbp1 = dataXtxAdrGet(0xE);

    GS_REG_SET(packet, 0xE, 0x1000000000008005LL);
    GS_REG_SET(packet, 0x6,
               tbp0 | ((long long)tbp1 << 37) | 0x2000000629340000LL);
    GS_REG_SET(packet, 0x42, 0);
    GS_REG_SET(packet, 0x47, 0x3000D);
    GS_REG_SET(packet, 0x8, 0);
    GS_REG_SET(packet, 0x14, 0);
}

void menuStatEnvMake(void *packet)
{
    unsigned short tbp0 = dataXtxAdrGet(0);
    unsigned short tbp1 = dataXtxAdrGet(3);

    GS_REG_SET(packet, 0xE, 0x1000000000008005LL);
    GS_REG_SET(packet, 0x6,
               tbp0 | ((long long)tbp1 << 37) | 0x2000000629340000LL);
    GS_REG_SET(packet, 0x42, 0);
    GS_REG_SET(packet, 0x47, 0x3000D);
    GS_REG_SET(packet, 0x8, 0);
    GS_REG_SET(packet, 0x14, 0);
}

void menuFaceEnvMake(void *packet)
{
    unsigned short tbp0 = dataXtxAdrGet(0x8);
    unsigned short tbp1 = dataXtxAdrGet(0x9);

    GS_REG_SET(packet, 0xE, 0x1000000000008005LL);
    GS_REG_SET(packet, 0x6,
               tbp0 | ((long long)tbp1 << 37) | 0x2000000629340000LL);
    GS_REG_SET(packet, 0x42, 0);
    GS_REG_SET(packet, 0x47, 0x3000D);
    GS_REG_SET(packet, 0x8, 0);
    GS_REG_SET(packet, 0x14, 0);
}

void menuATBEnvMake(void *packet)
{
    unsigned short tbp0 = dataXtxAdrGet(4);
    unsigned short tbp1 = dataXtxAdrGet(7);

    GS_REG_SET(packet, 0xE, 0x1000000000008005LL);
    GS_REG_SET(packet, 0x6,
               tbp0 | ((long long)tbp1 << 37) | 0x2000000629340000LL);
    GS_REG_SET(packet, 0x42, 0);
    GS_REG_SET(packet, 0x47, 0x3000D);
    GS_REG_SET(packet, 0x8, 0);
    GS_REG_SET(packet, 0x14, 0);
}

void menuFontEnvMake(void *packet)
{
    unsigned short tbp0 = dataXtxAdrGet(0xB);
    unsigned short tbp1 = dataXtxAdrGet(0xC);

    GS_REG_SET(packet, 0xE, 0x1000000000008005LL);
    GS_REG_SET(packet, 0x6,
               tbp0 | ((long long)tbp1 << 37) | 0x2000000629340000LL);
    GS_REG_SET(packet, 0x42, 0x44);
    GS_REG_SET(packet, 0x47, 0x3000D);
    GS_REG_SET(packet, 0x8, 0);
    GS_REG_SET(packet, 0x14, 0);
}

void menuStatIconEnvMake(void *packet)
{
    unsigned short tbp0 = dataXtxAdrGet(0);
    unsigned short tbp1 = dataXtxAdrGet(1);

    GS_REG_SET(packet, 0xE, 0x1000000000008005LL);
    GS_REG_SET(packet, 0x6,
               tbp0 | ((long long)tbp1 << 37) | 0x2000000629340000LL);
    GS_REG_SET(packet, 0x42, 0);
    GS_REG_SET(packet, 0x47, 0x3000D);
    GS_REG_SET(packet, 0x8, 0);
    GS_REG_SET(packet, 0x14, 0);
}

/* Sets TEX0_1 for the ATB texture with the CLUT of palette 0, 1 or, for any other value, 2. */
void menuATBEnvPalSet(GrPacket *packet, int palette)
{
    unsigned short tbp;
    unsigned short cbp;

    tbp = dataXtxAdrGet(4);
    if (palette == 0) {
        cbp = dataXtxAdrGet(5);
    } else if (palette == 1) {
        cbp = dataXtxAdrGet(7);
    } else {
        cbp = dataXtxAdrGet(10);
    }
    grGsRegSet(packet, SCE_GS_TEX0_1,(unsigned long long)tbp | ((unsigned long long)cbp << 37) | 0x2000000629340000ULL);
}



const signed char menu0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzText[232] = " 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz                                                                                    .    '                      /                   -                                 ";



const signed char menuJpText4[40] = "\241\277\241\241\241\246\241\241\241\260\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241";

extern const char menuNearText[8];
extern const char menuLineText[8];
extern const char menuAirText[8];
extern const char menuJpText[8];
extern const char menuHitText[8];
extern const char menuSlshText[8];
extern const char menuPrceText[8];
extern const char menuSprtText[8];
extern const char menuBeamText[8];
extern const char menuLtngText[8];
extern const char menuFireText[8];
extern const char menuIceText[8];

static MenuBatCmdEntry chaTbl_00A3F038[5] = { { .iconRow = 0, .command = 1, .offsetX = 0, .offsetY = 0 }, { .iconRow = 1, .command = 2, .offsetX = 4, .offsetY = 28 }, { .iconRow = 2, .command = 5, .offsetX = 8, .offsetY = 56 }, { .iconRow = 3, .command = 4, .offsetX = 12, .offsetY = 84 }, { .iconRow = 5, .command = 3, .offsetX = 16, .offsetY = 112 } };

static MenuBatCmdEntry agwsTbl_00A3F088[5] = { { .iconRow = 6, .command = 6, .offsetX = 0, .offsetY = 0 }, { .iconRow = 1, .command = 2, .offsetX = 4, .offsetY = 28 }, { .iconRow = 2, .command = 5, .offsetX = 8, .offsetY = 56 }, { .iconRow = 4, .command = 4, .offsetX = 12, .offsetY = 84 }, { .iconRow = 5, .command = 3, .offsetX = 16, .offsetY = 112 } };

static StatIconEntry statBA[15] = {
    { 0x2000, 0, 0 },
    { 0x1000, 22, 0 },
    { 0x0800, 44, 0 },
    { 0x0400, 66, 0 },
    { 0x0200, 66, 0 },
    { 0x0100, 66, 0 },
    { 0x0080, 66, 0 },
    { 0x0040, 66, 0 },
    { 0x0020, 88, 0 },
    { 0x0010, 88, 0 },
    { 0x0008, 88, 0 },
    { 0x0004, 0, 24 },
    { 0x0002, 0, 24 },
    { 0x0001, 0, 24 },
    { 0x0000, 0, 0 }
};

static StatIconEntry statMA[12] = {
    { 0x4000, 22, 24 },
    { 0x2000, 44, 24 },
    { 0x1000, 0, 24 },
    { 0x0800, 66, 0 },
    { 0x0400, 66, 24 },
    { 0x0080, 88, 0 },
    { 0x0040, 88, 0 },
    { 0x0020, 44, 0 },
    { 0x0010, 88, 0 },
    { 0x0008, 88, 0 },
    { 0x0004, 0, 24 },
    { 0x0000, 0, 0 }
};

static StatIconEntry statBB[13] = {
    { 0x4000, 88, 48 },
    { 0x2000, 0, 48 },
    { 0x1000, 22, 48 },
    { 0x0800, 22, 48 },
    { 0x0400, 22, 48 },
    { 0x0200, 22, 48 },
    { 0x0100, 44, 48 },
    { 0x0080, 44, 48 },
    { 0x0040, 22, 48 },
    { 0x0020, 22, 48 },
    { 0x0010, 66, 48 },
    { 0x0001, 88, 48 },
    { 0x0000, 0, 0 }
};

static StatIconEntry statMB[9] = {
    { 0x0800, 110, 48 },
    { 0x0400, 0, 72 },
    { 0x0080, 22, 72 },
    { 0x0040, 44, 72 },
    { 0x0020, 22, 48 },
    { 0x0010, 22, 48 },
    { 0x0008, 66, 72 },
    { 0x0004, 88, 72 },
    { 0x0000, 0, 0 }
};

static StatIconEntry statAA1[14] = {
    { 0x2000, 0, 0 },
    { 0x1000, 66, 0 },
    { 0x0800, 22, 0 },
    { 0x0400, 66, 0 },
    { 0x0200, 66, 0 },
    { 0x0100, 66, 0 },
    { 0x0080, 88, 0 },
    { 0x0040, 88, 24 },
    { 0x0020, 88, 0 },
    { 0x0010, 88, 0 },
    { 0x0004, 0, 24 },
    { 0x0002, 0, 24 },
    { 0x0001, 0, 24 },
    { 0x0000, 0, 0 }
};

static StatIconEntry statAA2[4] = {
    { 0x4000, 110, 24 },
    { 0x2000, 44, 24 },
    { 0x1000, 22, 24 },
    { 0x0000, 0, 0 }
};

static StatIconEntry statAB[5] = {
    { 0x4000, 44, 48 },
    { 0x2000, 44, 48 },
    { 0x1000, 66, 48 },
    { 0x0800, 110, 72 },
    { 0x0000, 0, 0 }
};

static StatIconEntry *statTbl[8] = {
    statBA, statMA, statBB, statMB,
    statAA1, statAA2, statAB, 0
};

static int keyDispL[2] = { 5, 0 };

static int keyDispU[2] = { 6, 0 };

static int keyDispLL[3] = { 5, 5, 0 };

static int keyDispLU[3] = { 5, 6, 0 };

static int keyDispUU[3] = { 6, 6, 0 };

static int keyDispUL[3] = { 6, 5, 0 };

static int keyDispLR[3] = { 5, 8, 0 };

static int keyDispUR[3] = { 6, 8, 0 };

static int keyDispR[2] = { 8, 0 };

static int keyDispRL[3] = { 8, 5, 0 };

static int keyDispRU[3] = { 8, 6, 0 };

static int keyDispRR[3] = { 8, 8, 0 };

static KeyGuideLine keyGuideNull[5] = { { 1, 1 }, { 2, 2 }, { 4, 7 }, { 3, 28 }, { 0, 0 } };

static KeyGuideLine keyGuideL[5] = { { 1, 3 }, { 2, 4 }, { 4, 11 }, { 3, 0 }, { 0, 0 } };

static KeyGuideLine keyGuideU[5] = { { 1, 6 }, { 2, 5 }, { 4, 12 }, { 3, 0 }, { 0, 0 } };

static KeyGuideLine keyGuideLL[3] = { { 4, 13 }, { 3, 0 }, { 0, 0 } };

static KeyGuideLine keyGuideLU[3] = { { 4, 14 }, { 3, 0 }, { 0, 0 } };

static KeyGuideLine keyGuideUU[3] = { { 4, 15 }, { 3, 0 }, { 0, 0 } };

static KeyGuideLine keyGuideUL[3] = { { 4, 16 }, { 3, 0 }, { 0, 0 } };

static KeyGuideLine keyGuideLR[3] = { { 4, 17 }, { 3, 0 }, { 0, 0 } };

static KeyGuideLine keyGuideUR[3] = { { 4, 18 }, { 3, 0 }, { 0, 0 } };

static KeyGuideLine keyGuideR[5] = { { 1, 8 }, { 2, 9 }, { 4, 10 }, { 3, 0 }, { 0, 0 } };

static KeyGuideLine keyGuideD[2] = { { 3, 0 }, { 0, 0 } };

static KeyGuideMode keyGuideTblChr[13] = {
    { keyGuideNull, 0 },
    { keyGuideL, keyDispL },
    { keyGuideU, keyDispU },
    { keyGuideLL, keyDispLL },
    { keyGuideLU, keyDispLU },
    { keyGuideUU, keyDispUU },
    { keyGuideUL, keyDispUL },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { keyGuideLR, keyDispLR },
    { keyGuideUR, keyDispUR }
};

static KeyGuideMode keyGuideTblAgws[13] = {
    { keyGuideNull, 0 },
    { keyGuideL, keyDispL },
    { keyGuideU, keyDispU },
    { keyGuideD, keyDispLL },
    { keyGuideD, keyDispLU },
    { keyGuideD, keyDispUU },
    { keyGuideD, keyDispUL },
    { keyGuideR, keyDispR },
    { keyGuideD, keyDispRL },
    { keyGuideD, keyDispRU },
    { keyGuideD, keyDispRR },
    { keyGuideD, keyDispLR },
    { keyGuideD, keyDispUR }
};

static UvTblEntry uvTbl[8] = { { .srcX = 24, .srcY = 192, .width = 24, .height = 24 }, { .srcX = 48, .srcY = 192, .width = 24, .height = 24 }, { .srcX = 72, .srcY = 192, .width = 24, .height = 24 }, { .srcX = 0, .srcY = 192, .width = 24, .height = 24 }, { .srcX = 24, .srcY = 216, .width = 24, .height = 24 }, { .srcX = 48, .srcY = 216, .width = 24, .height = 24 }, { .srcX = 72, .srcY = 216, .width = 24, .height = 24 }, { .srcX = 0, .srcY = 216, .width = 24, .height = 24 } };

static const char * atr1Name[3] = { menuNearText, menuLineText, menuAirText };

static const char * atr2Name[4] = { menuHitText, menuSlshText, menuPrceText, menuJpText };

static const char * atr3Name[6] = { menuIceText, menuFireText, menuLtngText, menuSprtText, menuBeamText, menuJpText };

const MenuRGBA menuStartGray = { 0x40, 0x40, 0x40, 0x80 };

const char menuEtherListText[16] = "Ether List";

const char menuItemsListText[12] = "Items List";

const enum MenuStatStartZ menuStatStartZ = MENU_STAT_START_Z;

const BoostFT4Pair boostFT4Pair = {
    {
        {
            {
                { 0.0f, -30.f, 0.0f, 1.0f },
                { 30.f, -30.f, 0.0f, 1.0f },
                { 0.0f, 30.f, 0.0f, 1.0f },
                { 30.f, 30.f, 0.0f, 1.0f }
            },
            { 0x80, 0x80, 0x80, 0x80 },
            { 16.0f, 45.0f, 16.0f, 45.0f },
            { 65.0f, 65.0f, 125.0f, 125.0f }
        },
        {
            {
                { -30.f, -30.f, 0.0f, 1.0f },
                { 0.0f, -30.f, 0.0f, 1.0f },
                { -30.f, 30.f, 0.0f, 1.0f },
                { 0.0f, 30.f, 0.0f, 1.0f }
            },
            { 0x80, 0x80, 0x80, 0x80 },
            { 45.0f, 16.0f, 45.0f, 16.0f },
            { 125.0f, 125.0f, 65.0f, 65.0f }
        }
    }
};

const BoostOfs3 boostOffsetsFirst = { { 12, 38, 64 } };

const BoostOfs3 boostOffsetsSecond = { { -28, -56, -28 } };

const BoostCol3 boostColors = { { { 0x60, 0x0, 0x60, 0x80 }, { 0x0, 0x60, 0x0, 0x80 }, { 0x60, 0x0, 0x0, 0x80 } } };

const MenuRGBA menuBlueRedColors[2] = { { 0x80, 0x80, 0xff, 0x80 }, { 0xff, 0x80, 0x80, 0x80 } };

const char menuStockDeadText[24] = "** StockDead %d %d\n";

const MenuRGBA menuGreenYellowRedColors[3] = { { 0x0, 0x80, 0x0, 0x80 }, { 0x80, 0x80, 0x0, 0x80 }, { 0x80, 0x0, 0x0, 0x80 } };

const NumPutCol numPutCol = { .c = { { .r = 255, .g = 255, .b = 255, .a = 128 }, { .r = 255, .g = 0, .b = 0, .a = 128 } } };

const GuideFrameCol guideFrameCol = { .c = { { .r = 24, .g = 24, .b = 80, .a = 128 }, { .r = 80, .g = 24, .b = 64, .a = 128 }, { .r = 24, .g = 80, .b = 24, .a = 128 } } };

const char menuAirText[8] = "Air ";

const char menuLineText[8] = "Line";

const char menuNearText[8] = "Near";

const char menuJpText[8] = "----";

const char menuPrceText[8] = "Prce";

const char menuSlshText[8] = "Slsh";

const char menuHitText[8] = "Hit ";

const char menuBeamText[8] = "Beam";

const char menuSprtText[8] = "Sprt";

const char menuLtngText[8] = "Ltng";

const char menuFireText[8] = "Fire";

const char menuIceText[8] = "Ice ";

const char menuJpText2[8] = "";

const char menuJpText3[8] = "/";

const char menuEndText[8] = "End";

const char menuText[8] = "Menu";

const DmgMsgUVTbl dmgMsgUVTbl = { .e = { { .srcX = 132, .srcY = 209, .width = 43, .height = 14 }, { .srcX = 132, .srcY = 223, .width = 43, .height = 14 }, { .srcX = 132, .srcY = 237, .width = 43, .height = 14 }, { .srcX = 175, .srcY = 223, .width = 31, .height = 14 }, { .srcX = 175, .srcY = 209, .width = 31, .height = 14 } } };

const char menuJpText5[8] = "%X";

const MenuRGBA menuNeutralGray = { 0x80, 0x80, 0x80, 0x80 };

static int selX;

static int selY;

static float ETRot;

static float ETRotDist;

static int mapH;

static int menuSel;

static int menuSelCnt;

static int sSp[8];

static Vector4 cPoint[4];

static MenuWinCommand ethBw[13];

static MenuWinCommand itmBw[101];
