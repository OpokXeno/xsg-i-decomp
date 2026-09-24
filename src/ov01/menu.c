/*
 * OV01 original TU 9: 0x00a26018..0x00a2c3a0 (91 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov01/battle_init.h"

extern ObjectTask *pMenuBat;

void menuInitBat(void) {
    pMenuBat = 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuOpenBat);

extern int menuResBat;
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

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuEtherNumChk);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuOpenEth);

extern unsigned char ethWin[];
void eBattleWinClose(void *window);

void menuCloseEth(void) {
    eBattleWinClose(ethWin);
    sndSysSePlay(8);
}

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuResultEth);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuOpenItm);

extern unsigned char itmWin[];

void menuCloseItm(void) {
    eBattleWinClose(itmWin);
    sndSysSePlay(8);
}

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuResultItm);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuStatDispCreate);

extern ObjectTask *pPreObj;
extern ObjectTask *pAtbObj;
extern ObjectTask *pKeyGuideObj;
extern ObjectTask *pBoostObj;
extern ObjectTask *pCritObj;
extern ObjectTask *pStatObj[4];
extern ObjectTask *pStockObj[5];

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

extern float RingRot;

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
extern int mapHDist;

void menuMapHSet(int enable)
{
    if (enable != 0) {
        mapHDist = 0x14;
        return;
    }
    mapHDist = 0x40;
}

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuMapDisp);

/*
 * ov01/tu008 src/ov01/gr_gp_init.c, still asm there; forward-declared here
 * until that TU publishes its header.
 */
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

INCLUDE_ASM("asm/nonmatchings/ov01/menu", faceUVGet);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuMapItemDispSub);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuSelLine);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuEventTimerDisp);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuEventTimerDispSub);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuTimeSet);

/* menuTimeSet (this TU, still asm) applies a new menu time setting. */
void menuTimeSet(int time);
extern int eventTimer;

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

extern int eventTimerX;

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

#include "ov01/calc.h"

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

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuApBarPut);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuBpBarPut);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuStatIconChk);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuStatIconPut);

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
    unsigned char *iconGraphic;    /* +0x14: D_00A468E8 */
};
int *dataStatNameGet(int charaId, int iconSlot);
void eBattleWinOpen2(struct StatNameOpenParams *params);
int menuStatIconChk(int charaId, int iconSlot);
extern unsigned char D_00A468E8[];
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

INCLUDE_ASM("asm/nonmatchings/ov01/menu", monsNameDisp);

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

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuFontIdxGet);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuDFontIdxGet);

void menuStatSprite(int packet, int x, int y, int width, int height, int srcX, int srcY, int srcRow);

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
                   col * FONT_GLYPH_WIDTH, row * FONT_GLYPH_HEIGHT, row);
}

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuFontPut);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuActCurPut);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuStatSprite);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuStatSprite2);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuStatSpriteRGB);

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

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuEnvMake);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuStatEnvMake);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuFaceEnvMake);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuATBEnvMake);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuFontEnvMake);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuStatIconEnvMake);

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuATBEnvPalSet);
