/*
 * OV01 original TU 9: 0x00a26018..0x00a2c3a0 (91 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov01/battle_init.h"
#include "ov01/calc.h"
extern CalcUnitParam *calcUPGet(ObjectTask *unit);

typedef struct MenuEtherStatusEntry {
    unsigned char unmodeled_00[0xE];
    short etherStatus;
} MenuEtherStatusEntry;

extern const signed char D_00A46AF8[];
extern const signed char D_00A46BE0[];
void menuStatSprite(int packet, int x, int y, int width, int height, int srcX, int srcY);
extern int rnd(int max);
extern float ETRotDist;

/*
 * statTbl: an array of pointers, indexed by charaId, each pointing at a
 * 0-terminated run of 0xC-byte entries (data still asm-owned). menuStatIconChk
 * (0x00A2A4C8..0x00A2A4E0) loads each entry's first word only, as a bitmask
 * tested against the statFlags mask argument.
 */
typedef struct StatIconEntry {
    int flags; /* +0x00 */
    int srcX;  /* +0x04: menuStatIconPut (0x00A2A56C..0x00A2A574) */
    int srcY;  /* +0x08 */
} StatIconEntry;
extern StatIconEntry *statTbl[];

typedef struct MenuRGBA {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} MenuRGBA;

/* The sprite colour: menuStatSpriteRGB copies a caller's MenuRGBA into it
 * byte-wise (lwl/lwr, swl/swr), while menuStatSprite/menuStatSprite2 copy the
 * whole word from an aligned local (lw/sw), so the field is word-aligned. */
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

extern void grPutSpr(int packet, GrSprite *sprite, int flags);
/* The shared sprite colour, .word 0x80808080 (build/ov01/asm/data/ov01/menu.rodata.s:587;
 * rodata still asm-owned). */
extern const MenuRGBA D_00A46C10;

/* The event timer object's work, as far as menuEventTimerDisp reads it:
 * its x/z position (lwc1 at +0x90/+0x98). */
typedef struct MenuEventTimerWork {
    unsigned char unmodeled_00[0x90];
    float x; /* +0x90 */
    unsigned char unmodeled_94[4];
    float z; /* +0x98 */
} MenuEventTimerWork;

void menuATBEnvMake(void *packet);
void menuEventTimerDispSub(ObjectTask *task, int packet);

/* Keep source-owned local BSS objects in their original address order;
 * intervening original-only storage remains scaffold-owned between runs. */
static ObjectTask *pMenuBat;
static ObjectTask *pPreObj;
static ObjectTask *pAtbObj;
static ObjectTask *pKeyGuideObj;
static ObjectTask *pBoostObj;
static ObjectTask *pCritObj;
static ObjectTask *pStatObj[4];
static ObjectTask *pStockObj[5];
static int eventTimer;
static int eventTimerX;
static float RingRot;
static int mapHDist;
static int menuResBat;
static unsigned char ethWin[0x28];
static unsigned char itmWin[0x28];

void menuInitBat(void) {
    pMenuBat = 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuOpenBat);

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

/* obj.c (ov01/tu001), still INCLUDE_ASM: returns the address of the
 * 20-byte command entry the task's work block currently selects. */
void *objCmdPtrGet(ObjectTask *task);
void menuBatObjFree(ObjectTask *task);

void menuBatObj(ObjectTask *task) {
    void *work;

    objCmdPtrGet(task);
    menuBatObjFree(task);
    /* The original keeps jal + epilogue for this last call rather than a
     * tail jump (CP-0132, config/compiler-patterns.json): a post-call read
     * blocks the compiler's sibcall lowering and is then removed as dead
     * code. */
    work = task->work;
}

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuBatObjFree);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuBatObjDraw);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuCmdExecChk);

int menuEtherNumChk(ObjectTask *unit)
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

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuOpenEth);

void eBattleWinClose(void *window);

void menuCloseEth(void) {
    eBattleWinClose(ethWin);
    sndSysSePlay(8);
}

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuResultEth);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuOpenItm);

void menuCloseItm(void) {
    eBattleWinClose(itmWin);
    sndSysSePlay(8);
}

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuResultItm);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuStatDispCreate);

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

/* 0x00a27278: `jr $31; nop`, an empty body in the original. */
void menuStatPreObj(void) {
}

void menuStatTexTrans(void);

void menuStatPreObjDraw(void) {
    menuStatTexTrans();
}

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuATBObj);

/* 0x00a27438: `jr $31; nop`, an empty body in the original. */
void menuKeyGuideObj(void) {
}

/* ManWork and manWorkGet come from their definer, ov01/tu003 battle_init.c,
 * through include/ov01/battle_init.h. menuKeyGuideDisp null-checks
 * unitTask, then reads a flag (bit 3) from unitTask->work, and compares
 * controlPhase against 0xb and 0x1a. */
void menuKeyGuideDisp(int packet, ObjectTask *task, int phase);

void menuKeyGuideObjDraw(int packet) {
    ObjectTask *task;
    int phase;

    task = manWorkGet()->unitTask;
    phase = manWorkGet()->controlPhase;
    menuKeyGuideDisp(packet, task, phase);
}

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuStatObj);

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

/*
 * actStockGet (ov01/tu003 battle_init.c, exact_c) returns &actStock; calcBoost's
 * own bounded view of the same record (src/ov01/calc.h CalcActStock) already
 * names the per-mode ObjectTask *slot[2] at +0x10. menuBoostObjDraw reads
 * the same two words only to test them for null, so it indexes actStockGet()
 * the same way rather than repeating a third bounded view of the record.
 */
extern int *actStockGet(void);
void menuBoostObjDrawCha(ObjectTask *task);
void menuBoostObjDrawSel(ObjectTask *task);

void menuBoostObjDraw(ObjectTask *task) {
    if (actStockGet()[4] == 0 && actStockGet()[5] == 0) {
        menuBoostObjDrawSel(task);
        return;
    }
    menuBoostObjDrawCha(task);
}

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuBoostObjDrawCha);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuBoostObjDrawSel);

/*
 * The boost-ring display object (data still asm-owned; original ELF symbol
 * name).
 * Its work record's first word is a flag set; bit 2 (0x4) hides the object.
 */
extern ObjectTask *pBoostObj;

void menuBoostDisp(int enabled)
{
    if (enabled != 0) {
        *(int *)pBoostObj->work &= ~4; /* clear hidden bit 2 */
        return;
    }
    *(int *)pBoostObj->work |= 4; /* set hidden bit 2 */
}

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuCritDispOn);

/*
 * The critical-indicator display object (data still asm-owned; original
 * ELF symbol name). Its work record's first word is a flag set; bit 2 (0x4) hides it.
 */
extern ObjectTask *pCritObj;

void menuCritDispOff(void)
{
    *(int *)pCritObj->work |= 4; /* set hidden bit 2 */
}

int objCmdNext(ObjectTask *task);
int objHatoVec(ObjectTask *task, int direction);
int objStdMove(ObjectTask *task);

/*
 * objCmdPtrGet's return, as far as menuCritObj reads it: `lw $3,0x0($2)`
 * loads the word at +0x00. obj.c's own view of the same 20-byte record
 * (src/ov01/obj.h ObjectCommandEntry.selector) already names that word;
 * this TU cannot redefine that tag, so it names its own bounded view of
 * the same record instead.
 */
typedef struct MenuCmdEntryView MenuCmdEntryView;
struct MenuCmdEntryView {
    int selector; /* +0x00 */
};

/*
 * The battle actor task->work points at, as far as menuCritObj reads it
 * (0x00a281f4..0x00a28200): `lw $2,0x10($16)` loads task->work, then
 * `lw $6,0x80($2)` reads one word at +0x80. ov01/tu002 unit_cmd.c's own
 * view of the same object (src/ov01/unit_cmd.h Actor) already evidences an
 * ObjectTask *targetUnit at the same offset, set by unitCmdDstSet; this TU
 * cannot redefine that tag, so it names its own bounded view of the same
 * object instead.
 */
typedef struct MenuCritActorView MenuCritActorView;
struct MenuCritActorView {
    unsigned char unmodeled_00[0x80];
    ObjectTask *targetUnit; /* +0x80 */
};

int menuCritObj(ObjectTask *task)
{
    MenuCmdEntryView *entry;
    MenuCritActorView *actor;
    int result;

    result = 1;
    entry = objCmdPtrGet(task);
    if (entry->selector == 1) {
        actor = task->work;
        objHatoVec(task, (actor->targetUnit == 0) ? 4 : 2);
        result = objStdMove(task);
        if (result != 0) {
            result = objCmdNext(task);
        }
    }
    return result;
}

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuCritObjDraw);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuATBObjDraw);

/*
 * The battle map's height-scan distance (data still asm-owned; original
 * ELF symbol name).
 */
void menuMapHSet(int enable)
{
    if (enable != 0) {
        mapHDist = 0x14;
        return;
    }
    mapHDist = 0x40;
}

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuMapDisp);

/* Defined by ov01/tu008 (src/ov01/gr_gp_init.c). */
extern void grCloseSpr(void);
extern void grGsRegSet(int packet, int reg, int value);
extern void grOpenSpr(int packet);

void menuATBEnvPalSet(int packet, int palette);
void menuMapItemDispSub(ObjectTask *task, int packet, int index, int x, int z);

/*
 * task->work's own float pair at these offsets is the map item's world
 * position; only x and z feed this draw call.
 */
#define MAP_ITEM_POS_X_OFF 0x90
#define MAP_ITEM_POS_Z_OFF 0x98

void menuMapItemDisp(ObjectTask *task, int packet)
{
    void *work;
    int x;
    int z;
    int zFar;

    work = task->work;
    z = (int)(*(float *)((char *)work + MAP_ITEM_POS_Z_OFF));
    x = (int)(*(float *)((char *)work + MAP_ITEM_POS_X_OFF));
    grGsRegSet(packet, 0x42, 0);
    zFar = z + 0x4C;
    grGsRegSet(packet, 0x14, 0);
    menuATBEnvPalSet(packet, 1);
    grOpenSpr(packet);
    menuMapItemDispSub(task, packet, 0, x, zFar);
    menuMapItemDispSub(task, packet, 1, x, z);
    grCloseSpr();
}

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuTgtLineDisp);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuStockStart);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuStockNext);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuStockBoost);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuStockDead);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuStockObj);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuStockObjDraw);

/*
 * The 3x11 face-icon sheet's per-character source cell: chrId<32 indexes a
 * grid of 21x25 cells (column = chrId%3, row = chrId/3); chrId>=32 (no face
 * tile) picks one of three fixed columns from calcUPGet(task)->flags's
 * 0x4000/0x2000 bits and leaves the row at 0.
 */
void faceUVGet(ObjectTask *task, int *srcX, int *srcY)
{
    int chrId;
    CalcUnitParam *up;
    u16 flags;

    chrId = menuStatChrIdGet(task);
    if (chrId < 0x20) {
        *srcX = (chrId % 3) * 21 + 0x40;
        *srcY = (chrId / 3) * 25;
        return;
    }
    up = calcUPGet(task);
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

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuMapItemDispSub);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuSelLine);

void menuEventTimerDisp(ObjectTask *task, int packet)
{
    MenuEventTimerWork *work;
    int x;
    int y;

    work = task->work;
    x = work->x;
    y = work->z;
    menuATBEnvMake(packet);
    menuEventTimerDispSub(task, packet);
    x -= 0x18;
    y += 0x38;
    grOpenSpr(packet);
    menuStatSprite(packet, x, y, 0x18, 0x4E, 0x80, 0x32);
    grCloseSpr();
}

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuEventTimerDispSub);

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
void grGpInit(int packet);
void grPacketSend(int packet);
void menuPlStatPut(ObjectTask *task, int packet, BattleUnit *unit);
void menuStatNamePut(ObjectTask *task);

/*
 * The status-icon task's own work record, as far as menuStatObjDraw and
 * menuStatNamePut read it. menuStatNamePut (0x00a2a5c0..0x00a2a5c8) loads
 * `lw $17,0x78($2)` / `lw $5,0x7c($2)` from task->work and passes them to
 * menuStatIconChk/dataStatNameGet as the character id and the icon slot to
 * check; menuStatObjDraw (0x00a29bf4..0x00a29c00) loads `lw $17,0x80($2)`
 * from the same block and compares it against actUnitGet()'s BattleUnit *,
 * showing the name popup only for the currently active unit.
 */
typedef struct MenuStatObjWork MenuStatObjWork;
struct MenuStatObjWork {
    unsigned char unmodeled_00[0x78];
    int charaId;       /* +0x78 */
    int iconSlot;       /* +0x7C */
    BattleUnit *unit;   /* +0x80 */
};

void menuStatObjDraw(ObjectTask *task) {
    int packet;
    MenuStatObjWork *work;
    BattleUnit *unit;

    grGpInit((int)&packet);
    work = task->work;
    unit = work->unit;
    menuPlStatPut(task, (int)&packet, unit);
    grPacketSend((int)&packet);
    if (unit == actUnitGet()) {
        menuStatNamePut(task);
    }
}


/* calcUPGet's return type comes from its definer, ov01/tu004 calc.c
 * (published include/ov01/calc.h); calcUPGet itself is still asm there, so
 * this TU declares it the same way ov01/tu002 unit_cmd.c and ov01/tu013
 * snd.c do. */
extern CalcUnitParam *calcUPGet(ObjectTask *unit);

/*
 * unitLoad and unitActupdate test this bit of the battle actor's flags word
 * to tell an enemy-controlled unit from a player-controlled one
 * (src/ov01/unit_cmd.h documents it as ACTOR_FLAG_ENEMY); ov01/tu013 snd.c's
 * sndConvSe already reuses the same evidenced mask off calcUPGet(unit)->flags.
 */
#define ACTOR_FLAG_ENEMY 0x40u

/* An enemy unit, or one whose charaId falls in the boss range ov01/tu013
 * snd.c's sndConvSe documents (0xBB..0xC2), reports the linked unit's
 * charaId instead of its own. */
int menuStatChrIdGet(ObjectTask *unit)
{
    int charaId;
    CalcUnitParam *up;

    up = calcUPGet(unit);
    charaId = up->charaId;
    if (!(calcUPGet(unit)->flags & ACTOR_FLAG_ENEMY)) {
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

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuPlStatPut);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuNumPut);

void menuApBarPut(int packet, int x, int y, int amount)
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

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuBpBarPut);

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

void menuStatIconPut(int packet, int x, int y, int charaId, int statFlags)
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
 * eBattleWinOpen2's own argument words (src/main/e_battle_win_open.c, still
 * asm there, no published parameter type). The header shape (textId/
 * speaker/flags/priority/target) and the flags constant 0x00FFFFFD match
 * src/ov01/cmd.c's BattleMsgOpenParams, the same window family's argument
 * record for the sibling eBattleWinOpen4; only flags and target are
 * confirmed for this callee, the textId/speaker/priority names are carried
 * over from that sibling record's field order. This call adds one more
 * word, an icon graphic table pointer, that eBattleWinOpen4 does not take.
 */
struct StatNameOpenParams {
    int textId;                /* +0x00: 8 at this call site */
    int speaker;                /* +0x04: 0x68 at this call site */
    int flags;                   /* +0x08: 0x00FFFFFD */
    int priority;                 /* +0x0C: 0x800000B0 at this call site */
    int target;                    /* +0x10: dataStatNameGet's resolved name id */
    const char *iconGraphic;       /* +0x14: D_00A468E8 */
};
int *dataStatNameGet(int charaId, int iconSlot);
void eBattleWinOpen2(struct StatNameOpenParams *params);
int menuStatIconChk(int charaId, int iconSlot);
const char D_00A468E8[8] = "Status";
extern void eBattleWinMain2(void);

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
        params.iconGraphic = D_00A468E8;
        eBattleWinOpen2(&params);
        eBattleWinMain2();
    }
}

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuKeyGuideDisp);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", keyGideDisp);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuGuideFrameDisp);

extern const char **dataEnemyNameGet(int index);
extern const char **dataPlayerNameGet(int id);
extern unsigned int strlen(const char *s);
void menuGuideFrameDisp(void *packet, int x, int y, int width, int height);
void menuFontEnvMake(void *packet);
void menuStatEnvMake(void *packet);
void menuFontPut(void *packet, int x, int y, const char *text);

void monsNameDisp(void *packet, ObjectTask *unitTask, int x, int y, int isEnemy)
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

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuKeyGuideHelp);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuKeyGuideVal);

/* obj.c (ov01/tu001), exact_c: allocates a task from the free list and
 * stores the caller's callback into its ->exec slot. */
extern ObjectTask *objEntry2(void *argument, void (*callback)(ObjectTask *task));
/* obj.c (ov01/tu001), still INCLUDE_ASM there. */
extern void objStdInit(ObjectTask *task);

void objDmgNum(ObjectTask *task);
void objDmgNumDraw(ObjectTask *task);

/* The work block objWorkGet allocates for an objDmgNum/objDmgNumDraw task.
 * Only the fields objDmgNumCreate itself writes are evidenced; the span
 * before them is read by neither this function nor any other claimed one. */
typedef struct {
    unsigned char unmodeled_00[0x70];
    int countdown;         /* +0x70: objDmgNumCreate always sets 30 */
    int value;              /* +0x74 */
    int flags;               /* +0x78 */
    unsigned char unmodeled_7C[4];
    void *target;           /* +0x80 */
} DmgNumWork;

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

INCLUDE_ASM("asm/nonmatchings/ov01/menu", objDmgNum);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", objDmgNumDraw);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", dmgMsgPut);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", dmgNumPut);

int menuFontIdxGet(const signed char *text)
{
    static const signed char *font = D_00A46AF8;
    int index;

    for (index = 0; index < 230; index++) {
        if (text[0] == font[index]) {
            return index;
        }
    }
    return -1;
}

int menuDFontIdxGet(const signed char *text)
{
    static const signed char *font = D_00A46BE0;
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

void menuFontPutSub(int packet, int x, int y, int charCode) {
    int row;
    int col;

    row = charCode / FONT_GLYPH_COLUMNS;
    col = charCode % FONT_GLYPH_COLUMNS;
    menuStatSprite(packet, x, y, FONT_GLYPH_WIDTH, FONT_GLYPH_HEIGHT,
                   col * FONT_GLYPH_WIDTH, row * FONT_GLYPH_HEIGHT);
}

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuFontPut);

/*
 * The cursor object's transform, the work's +0x90..+0x110 tail that
 * grCalcMatrix reads through grBBCalc (src/ov01/gr_gp_init.c TransformState:
 * position at +0x90, offset at +0xa0, added to position). menuActCurPut
 * saves and restores all 0x80 bytes with aligned ld/sd, so the storage is
 * eight-byte aligned (the include/ov01/m_ef_create.h spelling); it writes
 * position through sefGetNullPosition and offset.y (swc1 +0x14) itself.
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

/* A unit task's motion actor beside its ObjectTask prefix
 * (`lw $5,0x14($6)`; src/ov01/cmd.h CmdUnitRecord names the same word). */
typedef struct MenuUnitTask {
    ObjectTask task;   /* +0x00 */
    void *motionActor; /* +0x14 */
} MenuUnitTask;

/* src/main/sef.c and src/ov01/gr_gp_init.c (ov01/tu008). */
extern void sefGetNullPosition(Vector4 *position, void *actor, int part, int flag);
extern int grBBCalc(ObjectTask *task, int *screenX, int *screenY);

void menuActCurPut(int packet, ObjectTask *cursor, ObjectTask *unit, int frame)
{
    MenuCursorTransform saved;
    int screenX;
    int screenY;
    MenuCursorTransform *transform;

    if (unit == 0 || (((BattleActor *)unit->work)->flags & ACTOR_FLAG_ENEMY)) {
        return;
    }
    transform = &((MenuCursorWork *)cursor->work)->transform;
    saved = *transform;
    sefGetNullPosition(&transform->fields.position, ((MenuUnitTask *)unit)->motionActor, 0, 0);
    transform->fields.offset.y = 1.3f;
    if (grBBCalc(cursor, &screenX, &screenY) != 0) {
        menuStatEnvMake((void *)packet);
        grOpenSpr(packet);
        menuStatSprite(packet, screenX - 8, screenY - 8, 16, 16, (frame % 8) * 16, 0x80);
        grCloseSpr();
        grPacketSend(packet);
    }
    *transform = saved;
}

void menuStatSprite(int packet, int x, int y, int width, int height, int srcX, int srcY) {
    GrSprite sprite;
    MenuColor color;

    color.rgba = D_00A46C10;
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

void menuStatSprite2(int packet, int x, int y, int width, int height, int srcLeft, int srcTop, int srcRight, int srcBottom) {
    GrSprite sprite;
    MenuColor color;

    color.rgba = D_00A46C10;
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

void menuStatSpriteRGB(int packet, int x, int y, int width, int height, int srcX, int srcY, const MenuRGBA *rgba) {
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

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuStatSprite2RGB);

/* dataXtxFileGet's record: only the texture id at +0x4 is read here. */
extern int *dataXtxFileGet(int index);
extern void xglPacketTextureTrans(int textureId);

void menuTexTrans(void) {
    int packet;

    xglPacketTextureTrans(dataXtxFileGet(1)[1]);
    grGpInit((int)&packet);
    grGsRegSet((int)&packet, 0x3F, 1);
    grPacketSend((int)&packet);
}

void menuStatTexTrans(void) {
    int packet;

    xglPacketTextureTrans(dataXtxFileGet(0)[1]);
    grGpInit((int)&packet);
    grGsRegSet((int)&packet, 0x3F, 1);
    grPacketSend((int)&packet);
}

/* GS_REG_SET's real (void *, long long, long long) prototype, as
 * src/ov01/entry_first_init.c's encEnvMake already declares it for the same
 * idiom. */
extern void GS_REG_SET(void *packet, long long register_id, long long control);
extern unsigned short dataXtxAdrGet(int index);

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

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuATBEnvPalSet);



const signed char D_00A46AF8[232] = " 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz                                                                                    .    '                      /                   -                                 ";



const signed char D_00A46BE0[40] = "\241\277\241\241\241\246\241\241\241\260\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241";
