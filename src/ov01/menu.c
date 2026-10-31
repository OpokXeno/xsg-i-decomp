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
/*
 * grGpInit's argument, restated from src/ov01/gr_gp_init.c (the owner keeps
 * it TU-local; header proposal in the PR): a data pointer and a length. A
 * local packet must be the whole record, because grGpInit writes both words.
 */
typedef struct GrPacket {
    void *data;
    int count;
} GrPacket;

void menuStatSprite(GrPacket *packet, int x, int y, int width, int height, int srcX, int srcY);
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

extern void grPutSpr(GrPacket *packet, GrSprite *sprite, int flags);
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
void menuEventTimerDispSub(ObjectTask *task, GrPacket *packet);

/*
 * One entry of a battle window's command list (src/main/e_battle_win_open.h
 * BattleWinCommand, 0xC bytes, TU-local there): menuResultEth/menuResultItm
 * read the chosen entry's id as a halfword (lh $2,0xA($2) after
 * selection * 0xC).
 */
typedef struct MenuWinCommand {
    unsigned char unmodeled_00[0xA];
    short id;                         /* +0x0A */
} MenuWinCommand;

/*
 * The ether/item window's parameter record, eBattleWinOpen's argument and
 * eBattleWinMain's (src/main/endou_test.c EndouBattleWindowParams, the same
 * layout; e_battle_win_open.h BattleWinTask names +0x1C/+0x20). The result
 * functions load commands (lw 0x14), store the pad word (sw 0x1C) and read
 * the selection (lw 0x20, -1 for none).
 */
typedef struct MenuBattleWin {
    unsigned char unmodeled_00[0x14];
    MenuWinCommand *commands;         /* +0x14 */
    unsigned char unmodeled_18[4];
    int buttons;                      /* +0x1C */
    int selection;                    /* +0x20 */
    unsigned char unmodeled_24[4];
} MenuBattleWin;

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
static MenuBattleWin ethWin;
static MenuBattleWin itmWin;

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

/*
 * unitLoad and unitActupdate test this bit of the battle actor's flags word
 * to tell an enemy-controlled unit from a player-controlled one
 * (src/ov01/unit_cmd.h documents it as ACTOR_FLAG_ENEMY); ov01/tu013 snd.c's
 * sndConvSe already reuses the same evidenced mask off calcUPGet(unit)->flags.
 */
#define ACTOR_FLAG_ENEMY 0x40u

/*
 * The battle menu's command tables, one 0x10-byte row per menu line
 * (sll $2,$2,4 on menuSel); menuBatObjFree reads only the command id at +0x04
 * (lw $5,0x4($2)), which menuCmdExecChk checks and menuResultBat returns.
 * chaTbl serves units without CalcUnitParam flag 0x40, agwsTbl units with
 * it (lhu 0x1C; andi 0x40 selects %lo(agwsTbl_00A3F088)). Data still asm-owned.
 */
typedef struct MenuBatCmdEntry {
    unsigned char unmodeled_00[4];
    int command;                      /* +0x04 */
    unsigned char unmodeled_08[8];
} MenuBatCmdEntry;
extern MenuBatCmdEntry chaTbl_00A3F038[];
extern MenuBatCmdEntry agwsTbl_00A3F088[];

/* The battle menu task's work: the unit the menu commands (lw $16,0x80($2)). */
typedef struct MenuBatWork {
    unsigned char unmodeled_00[0x80];
    ObjectTask *unit;                 /* +0x80 */
} MenuBatWork;

/* The highlighted menu line (0..4, wrapping) and its blink counter (bss still
 * scaffold-owned between mapHDist and menuResBat; original ELF symbol names). */
extern int menuSel;
extern int menuSelCnt;

extern int dataPadRead(int padNo);
int menuCmdExecChk(ObjectTask *unit, int command);

void menuBatObjFree(ObjectTask *task)
{
    ObjectTask *unit;
    MenuBatCmdEntry *table;

    unit = ((MenuBatWork *)task->work)->unit;
    if (calcUPGet(unit)->flags & ACTOR_FLAG_ENEMY) {
        table = agwsTbl_00A3F088;
    } else {
        table = chaTbl_00A3F038;
    }
    if (dataPadRead(1) & 0x4000) {
        /* A local, not a literal: the original compares with the 4 held in
         * a register (addiu $3,$0,4; slt $3,$3,$2), where `menuSel > 4`
         * folds to slti $2,$3,5. */
        int lastLine = 4;

        menuSel++;
        if (menuSel > lastLine) {
            menuSel = 0;
        }
        menuSelCnt = 0;
        sndSysSePlay(3);
    } else if (dataPadRead(1) & 0x1000) {
        menuSel--;
        if (menuSel < 0) {
            menuSel = 4;
        }
        menuSelCnt = 0;
        sndSysSePlay(3);
    }
    if (dataPadRead(1) & 0x20) {
        if (menuCmdExecChk(unit, table[menuSel].command) != 0) {
            menuResBat = table[menuSel].command;
            sndSysSePlay(1);
        } else {
            sndSysSePlay(5);
        }
        return;
    }
    if (dataPadRead(1) & 0x40) {
        sndSysSePlay(2);
    }
    menuResBat = 0;
}

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
    eBattleWinClose(&ethWin);
    sndSysSePlay(8);
}

/*
 * The ether window's result: pad 2's direction bits (0xF000) replace pad
 * 1's before eBattleWinMain reads them; the chosen ether's id (0 for an
 * unusable line, -1 for no selection) goes to *ethId, with the confirm
 * (0x20) and cancel (0x40) sounds. buttons is or-ed in its own statement
 * (and $17 then or $17,$17,$16: held keeps $16) and the function has one
 * return (each path ends at the shared daddu $2,$17,$0).
 */
void eBattleWinMain(void *window);

int menuResultEth(int *ethId)
{
    int held;
    int buttons;

    held = dataPadRead(2) & 0xF000;
    buttons = dataPadRead(1) & ~0xF000;
    buttons |= held;
    ethWin.buttons = buttons;
    eBattleWinMain(&ethWin);
    if (ethWin.selection == -1) {
        *ethId = ethWin.selection;
    } else {
        *ethId = ethWin.commands[ethWin.selection].id;
        if (buttons & 0x20) {
            if (*ethId != 0) {
                sndSysSePlay(1);
            } else {
                sndSysSePlay(5);
            }
        } else if (buttons & 0x40) {
            sndSysSePlay(2);
        }
    }
    return buttons;
}

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuOpenItm);

void menuCloseItm(void) {
    eBattleWinClose(&itmWin);
    sndSysSePlay(8);
}

/* menuResultEth for the item window. */
int menuResultItm(int *itmId)
{
    int held;
    int buttons;

    held = dataPadRead(2) & 0xF000;
    buttons = dataPadRead(1) & ~0xF000;
    buttons |= held;
    itmWin.buttons = buttons;
    eBattleWinMain(&itmWin);
    if (itmWin.selection == -1) {
        *itmId = itmWin.selection;
    } else {
        *itmId = itmWin.commands[itmWin.selection].id;
        if (buttons & 0x20) {
            if (*itmId != 0) {
                sndSysSePlay(1);
            } else {
                sndSysSePlay(5);
            }
        } else if (buttons & 0x40) {
            sndSysSePlay(2);
        }
    }
    return buttons;
}

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
    int argument; /* +0x04: menuATBObj counts it down under selector 1
                   * (lw $2,0x4($4); addiu $2,$2,-1; sw $2,0x4($4)) */
};

/*
 * The battle map's height value (bss still scaffold-owned, like ETRotDist;
 * original ELF symbol name). menuStatDispCreate starts it and mapHDist at
 * 0x40 (`sw $5,%lo(mapH)` / `sw $5,%lo(mapHDist)` at 0x00a26df0..0x00a26df4),
 * menuMapHSet below moves mapHDist to 0x14 or 0x40, and menuATBObj
 * steps mapH by 8 per update toward mapHDist (0x00a27320..0x00a2733c).
 * menuMapDisp offsets the map by 0x40 - mapH (0x00a2848c); menuATBObjDraw
 * draws the map item and target line only while it is 0x40 (0x00a283d4),
 * menuCritObjDraw the critical indicator only while it is 0x14
 * (`bnel` against 0x14). The two rest values are MENU_MAP_H_SET/RESET.
 */
extern int mapH;

/* The two values mapH rests at: menuMapHSet(nonzero) sends it to 0x14
 * (`addiu $2,$0,0x14` at 0x00a28428), menuMapHSet(0) and menuStatDispCreate
 * to 0x40 (0x00a28438, 0x00a26ddc). */
enum MenuMapH {
    MENU_MAP_H_SET = 0x14,
    MENU_MAP_H_RESET = 0x40
};

extern float ETRot;

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
    /* Unsigned equality chain (beql 1, beqz 0, bnel 2); the case 2 body is
     * laid out ahead of the case 1 body, as written here. */
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
        /* The absolute value inside the condition: the original tests
         * against step on both arms (two c.lt.s $f0,$f2) and reloads
         * ETRotDist at the join. */
        if (((distance < 0.0f) ? -distance : distance) < step) {
            ETRot = ETRotDist;
        }
    }
}

/* 0x00a27438: `jr $31; nop`, an empty body in the original. */
void menuKeyGuideObj(void) {
}

/* ManWork and manWorkGet come from their definer, ov01/tu003 battle_init.c,
 * through include/ov01/battle_init.h. menuKeyGuideDisp null-checks
 * unitTask, then reads a flag (bit 3) from unitTask->work, and compares
 * controlPhase against 0xb and 0x1a. */
/* The first parameter was spelled GrPacket *packet: menuKeyGuideDisp loads
 * its work (lw $2,0x10($17)) and the position floats there (lwc1 0x98/0x90
 * at 0x00A2A6E0), so it is the key guide object's own task. */
void menuKeyGuideDisp(ObjectTask *guideTask, ObjectTask *unitTask, int phase);

void menuKeyGuideObjDraw(ObjectTask *guideTask) {
    ObjectTask *task;
    int phase;

    task = manWorkGet()->unitTask;
    phase = manWorkGet()->controlPhase;
    menuKeyGuideDisp(guideTask, task, phase);
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

/* The +0x90 movement block of an object's work record (src/ov01/obj.h's
 * ObjectWorkTransform restates the same span; TU-local there): the current
 * position and the target objStdMove moves it toward. Shared by the stock
 * faces (MenuStockWork) and the critical indicator (MenuCritWork). */
typedef struct MenuObjTransform {
    Vector4 position;                  /* +0x00: addiu $16,$16,0x90, then
                                        * swc1 0x0/0x8 and sw $0,0x4 */
    unsigned char unmodeled_10[0x50];
    Vector4 target;                    /* +0x60: where the object moves to
                                        * (menuStockDead/menuStockNext
                                        * sw $0,0x64; menuCritDispOn
                                        * swc1 0x60/0x68 from work+0x90) */
    unsigned char unmodeled_70[0x10];  /* +0x70: ObjectWorkTransform's tail,
                                        * not accessed here */
} MenuObjTransform;

/*
 * The critical-indicator object's work, one record for menuCritObj and
 * menuCritObjDraw: menuStatDispCreate registers both on the same task
 * (the addresses of menuCritObj and menuCritObjDraw are formed at
 * 0x00A26E94..0x00A26EB8 and passed to objEntry2; the result is stored to
 * pCritObj) and menuCritDispOn writes pCritObj's work.
 * +0x80 is an int count: menuCritDispOn compares it with its int argument
 * (0x00A28128 lw $2,0x80($3); beql $2,$16) and stores it (0x00A28174
 * sw $16,0x80($2)); menuCritObjDraw tests it and draws digit count - 1
 * (0x00A282E8 lw $2,0x80($3); 0x00A282FC addiu $3,$2,-1); menuCritObj only
 * tests it against zero (0x00A28200 lw $6,0x80($2); movn $5,$3,$6).
 * This extends, and does not preserve the size of, the local view master
 * carried as MenuCritActorView (0x84 bytes: 0x80 unmodeled bytes and a task
 * pointer at +0x80 that menuCritObj only tested for null): the record is now
 * 0xA0 bytes, flags at +0x00 and position at +0x90 are new members, and the
 * +0x80 word is the int count rather than a pointer.
 */
typedef struct MenuCritWork {
    u32 flags;                        /* +0x00: bit 2 hides the indicator */
    unsigned char unmodeled_04[0x7C];
    int count;                        /* +0x80: 0 draws the plain banner */
    unsigned char unmodeled_84[0xC];
    MenuObjTransform transform;       /* +0x90: menuCritObjDraw reads
                                       * position.x/z; menuCritDispOn
                                       * takes work+0x90 once (addiu
                                       * $3,$3,0x90) and seeds both points */
} MenuCritWork;

/* Bit of MenuCritWork.flags that hides the indicator: menuCritDispOff sets
 * it (|= 4), menuCritObjDraw skips drawing while it is set (andi $2,$2,0x4).
 * An enum constant, not a macro (a macro can never be published by
 * header_harvest.py); it stays TU-local until a declaration needs it. */
enum MenuCritWorkFlag {
    MENU_CRIT_HIDDEN = 0x4
};

/* A record view read from the work slot as an int-sized word, like
 * menuStockWorkGet below: menuCritDispOn reloads the slot after each int
 * store (lw $2,0x10($4) after sw $5,0x0($2); lw $5,0x10($4) after
 * sw $16,0x80($2)). */
static __inline__ MenuCritWork *menuCritWorkGet(ObjectTask *task)
{
    /* Read as a u32 on purpose: `(MenuCritWork *)task->work` compiles the
     * callers' loads differently and OV01.OVL stops matching. */
    return (MenuCritWork *)*(u32 *)&task->work;
}

void *objCmdPush(ObjectTask *task);

/*
 * Shows the critical indicator with count: unless it is already showing that
 * count, queues command 1 (move in from x 576 to x 376 at z 310) and clears
 * the hidden bit. The three constants are locals assigned in the original's
 * load order (576.0f -> $f2, 310.0f -> $f1, 376.0f -> $f0); written inline
 * in the stores, 310.0f takes $f2 instead.
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

/*
 * The critical-indicator display object (data still asm-owned; original
 * ELF symbol name). Its work record's first word is a flag set; bit 2 (0x4) hides it.
 */
extern ObjectTask *pCritObj;

void menuCritDispOff(void)
{
    menuCritWorkGet(pCritObj)->flags |= MENU_CRIT_HIDDEN;
}




/* The work slot holds the address of the indicator's MenuCritWork.
 * menuCritObj reads the slot once (lw $2,0x10($16) at 0x00A281F4) and so
 * keeps the record in a local; menuCritObjDraw loads it afresh before each
 * of its reads (lw 0x10($18) at 0x00A28254, 0x00A2828C and 0x00A282D8), so
 * every access there casts the slot itself instead of caching it. */

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


/* Defined by ov01/tu008 (src/ov01/gr_gp_init.c). Declared here because
 * menuCritObjDraw calls them ahead of the declarations further down; without
 * them gcc 2.96 reports "type mismatch with previous implicit declaration". */
extern void grGpInit(GrPacket *packet);
extern void grOpenSpr(GrPacket *packet);
extern void grCloseSpr(void);
extern void grPacketSend(GrPacket *packet);

/*
 * Draws the critical banner and, for a nonzero count, its digit cell
 * (0x18 wide, starting at sheet x 0xB1). The digit's sheet offset is its
 * own statement ahead of the first call: the original keeps (count - 1) * 3
 * in $16 across that call and finishes *8 and +0xB1 after it; one
 * (count - 1) * 0x18 + 0xB1 expression folds to count * 0x18 + 0x99.
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

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuATBObjDraw);

/*
 * The battle map's height-scan distance (data still asm-owned; original
 * ELF symbol name).
 */
void menuMapHSet(int enable)
{
    if (enable != 0) {
        mapHDist = MENU_MAP_H_SET;
        return;
    }
    mapHDist = MENU_MAP_H_RESET;
}

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuMapDisp);

/* Defined by ov01/tu008 (src/ov01/gr_gp_init.c). */
extern void grCloseSpr(void);
extern void grGsRegSet(GrPacket *packet, unsigned long long reg, unsigned long long value);

void menuATBEnvPalSet(GrPacket *packet, int palette);
void menuMapItemDispSub(ObjectTask *task, GrPacket *packet, int index, int x, int z);

/*
 * task->work's own float pair at these offsets is the map item's world
 * position; only x and z feed this draw call.
 */
#define MAP_ITEM_POS_X_OFF 0x90
#define MAP_ITEM_POS_Z_OFF 0x98

void menuMapItemDisp(ObjectTask *task, GrPacket *packet)
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

/* The ATB gauge object's work: its screen position (lwc1 0x98/0x90). */
typedef struct MenuATBWork {
    unsigned char unmodeled_00[0x90];
    Vector4 position;                 /* +0x90 */
} MenuATBWork;

/* The target line's screen end point (bss still scaffold-owned between
 * pStockObj and eventTimer; original ELF symbol names); 0 means unset. */
extern int selX;
extern int selY;

BattleUnit *tgtUnitGet(void);
extern int calcLineChk(ObjectTask *unit);
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
    MenuATBWork *work;
    ObjectTask *target;
    BattleActor *actor;
    CalcUnitParam *up;
    int x;
    int y;
    int srcX;
    int srcY;
    int hp;
    int style;

    work = task->work;
    x = (int)work->position.x;
    y = (int)work->position.z;
    if (selX != 0 && selY != 0) {
        target = (ObjectTask *)tgtUnitGet();
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
        hp = (short)calcUPGet(target)->currentHp;
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

extern int *actStockGet(void);

typedef struct MenuStockWork {
    u32 flags;                         /* +0x00: bit 0x4 hides the face
                                        * (menuStockObjDraw, andi 0x4) */
    unsigned char unmodeled_04[0x6C];
    int frameCount;                    /* +0x70: menuStockObj adds 1 every
                                        * update; command 3 moves the face
                                        * only once it is nonzero */
    unsigned char unmodeled_74[4];
    /* A code-shape device, not data: width is only ever read and written as
     * `value`. `alias` is never accessed; it makes the int store to width
     * conflict with the void * load of ObjectTask.work in gcc 2.96's alias
     * analysis, so menuStockObj reloads the slot after storing width
     * (lw $4,0x10($16) after sw $3,0x78($5)) as the original does. */
    union {
        int value;
        void *alias;
    } width;                           /* +0x78: menuStockStart seeds 4
                                        * (sw $22,0x78); menuStockObj adds 4
                                        * while it is below 0x14 and writes
                                        * 0x14 otherwise; menuStockObjDraw
                                        * passes it as the sprite width */
    int stockSlot;                     /* +0x7C: menuStockDead indexes
                                        * actStockGet() with it
                                        * (lw $3,0x7C($5) at 0x00A28C40) */
    int index;                         /* +0x80: the face's pStockObj slot
                                        * (sw $18,0x80); menuStockDead
                                        * decrements every later face's
                                        * index by one */
    unsigned char unmodeled_84[0xC];
    /*
     * The +0x90..+0x110 tail of this work record (src/ov01/obj.h's
     * ObjectWorkTransform restates the same span for objStdMove/objStdInit;
     * that type is TU-local to obj.c and not published, so this is a
     * second local view of the same storage -- flagged for Opok).
     * menuStockDead takes this address once (addiu $8,$9,0x90) and writes
     * target through it (sw $0,0x64($8) / swc1 0x60/0x68($8)).
     */
    MenuObjTransform transform;      /* +0x90 */
} MenuStockWork;

/* gr_gp_init.c data (still asm-owned): each face's x/z on screen, read as
 * int pairs (lwc1 0x0/0x4, cvt.s.w). menuStockDead also indexes this. */
typedef struct MenuStockPos {
    int x;
    int z;
} MenuStockPos;
extern MenuStockPos stockPosTbl[5];

void *objCmdTailGet(ObjectTask *task);

/* Bit of MenuStockWork.flags that hides a face: menuStockStart sets it for
 * the fifth face (ori $2,$2,0x4) and clears it for the other four
 * (and $2,$2,$21 with $21 = -5); menuStockObjDraw tests it (andi 0x4). */
enum MenuStockWorkFlag {
    MENU_STOCK_HIDDEN = 0x4
};

/*
 * A stock face's MenuStockWork, read from its ObjectTask work slot as an
 * int-sized word. The original reloads the slot after each int store into
 * the record (menuStockStart: lw $3,0x10($17) after objCmdTailGet,
 * lw $2,0x10($17) after sw $18,0x80($3), lw $3,0x10($17) after
 * sw $22,0x78($2)) but schedules the reload above the float stores
 * (lw $2,0x10($17) above swc1 $f1,0x0($16)): the slot's load conflicts
 * with int stores only. Read as the published `void *`, gcc keeps the first
 * load across those stores (lw $2 at 0x00A289FC disappears). Flagged for
 * Opok: the original slot type looks int-sized (ObjectTask.work, shared.h).
 */
static __inline__ MenuStockWork *menuStockWorkGet(ObjectTask *task)
{
    return (MenuStockWork *)*(u32 *)&task->work;
}

/*
 * Seeds the five stock faces: slot index, command 2, screen position from
 * stockPosTbl, width 4, and only the fifth face hidden. x and z are read
 * before the width store: the union width (see MenuStockWork) is stored
 * with alias set 0, and the original loads both table words
 * (lwc1 0x0/0x4($19)) above sw $22,0x78($2).
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
 * hidden. Every record access goes through menuStockWorkGet: the original
 * reloads the work slot after the int store to index
 * (lw $3,0x10($16) after sw $0,0x80($5)).
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
 * stockPosTbl slot and moving to the first one. The target is written
 * before the position (lwc1 0x24 first, swc1 0x0 last), and the work slot
 * is re-read through menuStockWorkGet after the command store
 * (lw $4,0x10($17) after sw $4,0x0($2)).
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
 * menuStockDead's bounded view of the action-stock record actStockGet
 * returns (battle_init.c ActStock.stock, the calc.c CalcActStock idiom):
 * the lookup is addu $2,$2,$3 (record + slot*4), the member-array address
 * order; the published `int *` indexed directly gives slot*4 + record
 * (addu $3,$3,$2) instead. Restated locally because actStockGet's
 * published return type is plain int * -- flagged for Opok.
 */
typedef struct MenuActStockView {
    ObjectTask *stock[4]; /* +0x00: the queued actor per stock slot */
} MenuActStockView;

extern const char D_00A46898[]; /* "** StockDead %d %d\n" */
extern int printf(const char *format, ...);

/*
 * A dead unit's face leaves the gauge: unless its action-stock slot is
 * still occupied, every later face is sent one position left. printf takes
 * only dead and i (the decremented index lands in a different register by
 * allocation, not as an argument: passing it adds a copy).
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
                printf(D_00A46898, dead, i);
            }
        }
    }
}

void menuStockObj(ObjectTask *task)
{
    MenuCmdEntryView *entry;
    MenuStockWork *stock;

    entry = objCmdPtrGet(task);
    /* Compared as an unsigned equality chain (beq 1, beql 0, beq 2, beq 3);
     * a signed selector splits it with slti $2,$3,2 / bnel instead. */
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
     * any other value is replaced by 0x14 (slti $2,$2,0x14; movn $3,$4,$2). */
    stock = ((MenuStockWork *)task->work);
    stock->width.value = (stock->width.value < 0x14) ? stock->width.value + 4 : 0x14;
    ((MenuStockWork *)task->work)->frameCount++;
}

/*
 * menuStockObjDraw's bounded view of the record actStockGet returns (the
 * MenuActStockView stock array, then the words this function reads).
 */
typedef struct MenuActStockDrawView {
    ObjectTask *stock[4];   /* +0x00: lw $16,0x0(record + slot*4) */
    ObjectTask *boost[2];   /* +0x10: battle_init.c ActStock.boost */
    int count;              /* +0x18: lw $3,0x18($4); 0 draws no face */
    int picked;             /* +0x1C: lw $2,0x1C($4), subtracted from count */
    ObjectTask *boostUnit;  /* +0x20: lw $16,0x20($4), face 4's unit */
    int boostState;         /* +0x24: lw $3,0x24($4), compared with 2 */
} MenuActStockDrawView;

/* A stock unit's actor flags, read as one doubleword (ld $3,0x0($2)). */
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
void faceUVGet(ObjectTask *task, int *srcX, int *srcY);
void menuStatSprite2(GrPacket *packet, int x, int y, int width, int height, int srcLeft, int srcTop, int srcRight, int srcBottom);
void menuStatSprite2RGB(GrPacket *packet, int x, int y, int width, int height, int srcLeft, int srcTop, int srcRight, int srcBottom, const MenuRGBA *rgba);

/*
 * Draws one stock face beside the ATB gauge: the face of the unit queued in
 * its action-stock slot (the boost unit for face 4), raised by 4 for the
 * current target, and its frame; faces 0 and 4 get a pulsing frame.
 * The colour's initializer is what calls memset and stores a = 0x80 (sb
 * 0x23), sharing the constant 4 with the face compares ($16). faces is
 * formed before the count test (subu ahead of beqz $3), inset and the face
 * corner are their own values (li 10; subu $16 before grOpenSpr, addu
 * $16,$20,$16 after it), and the pulse is stored b, g, r to get the
 * original's sb 0x20/0x22/0x21 order.
 */
void menuStockObjDraw(ObjectTask *task)
{
    GrPacket packet;
    MenuRGBA color = { 0, 0, 0, 0x80 };
    int srcX;
    int srcY;
    MenuActStockDrawView *stock;
    MenuStockWork *work;
    ObjectTask *unit;
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
    x = (int)work->transform.position.x;
    y = (int)work->transform.position.z;
    x += ((MenuATBWork *)pAtbObj->work)->position.x;
    y += ((MenuATBWork *)pAtbObj->work)->position.z;
    grGpInit(&packet);
    menuATBEnvMake(&packet);
    faceUVGet(unit, &srcX, &srcY);
    raise = -4;
    if (unit != (ObjectTask *)tgtUnitGet()) {
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
 * CalcUnitParam.flags (lhu $3,0x1C($2) at 0x00A29144): bit 0x4000 set
 * (andi $3,$3,0x4000 at 0x00A2914C) selects sheet x 0xC0 (0x00A2915C),
 * otherwise bit 0x2000 set (andi $2,$3,0x2000 at 0x00A29148) selects 0xD5
 * (0x00A29164), otherwise 0xEA (0x00A29168). Neither bit has an established
 * name (include/ov01/calc.h and src/ov01/calc.h name other bits of this
 * halfword only), so they stay numeric; which characters carry them is not
 * evidenced here.
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

/* grPutF2's line (src/ov01/gr_gp_init.c GrF2Vertices, TU-local there): two
 * end points and one colour at +0x20 (sb 0x20..0x23 in menuSelLine). */
typedef struct GrF2Vertices {
    Vector4 points[2]; /* +0x00 */
    MenuRGBA color;    /* +0x20 */
} GrF2Vertices;

extern void grOpenF2(GrPacket *packet);
extern void grPutF2(GrPacket *packet, GrF2Vertices *line, const Matrix4 matrix);
extern void grCloseF2(void);

/*
 * The target line: a 0x3A-pixel stub left of (x0, y0), then on to (x1, y1);
 * green, or magenta when the line is blocked. The depth is converted at run
 * time (lui/ori 0x00FFFFFC; cvt.s.w into $f20), so it is written as an integer.
 */
void menuSelLine(GrPacket *packet, int x0, int y0, int x1, int y1, int blocked)
{
    GrF2Vertices line;
    int depth;

    depth = 0xFFFFFC;
    line.color.a = 0x80;
    if (blocked == 0) {
        line.color.r = 0;
        line.color.g = 0xFF;
        line.color.b = 0x80;
    } else {
        line.color.r = 0xF0;
        line.color.g = 0;
        line.color.b = 0x80;
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
void grGpInit(GrPacket *packet);
void grPacketSend(GrPacket *packet);
void menuPlStatPut(ObjectTask *task, GrPacket *packet, BattleUnit *unit);
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


/* calcUPGet's return type comes from its definer, ov01/tu004 calc.c
 * (published include/ov01/calc.h); calcUPGet itself is still asm there, so
 * this TU declares it the same way ov01/tu002 unit_cmd.c and ov01/tu013
 * snd.c do. */
extern CalcUnitParam *calcUPGet(ObjectTask *unit);


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

void menuApBarPut(GrPacket *packet, int x, int y, int amount)
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

/* grPutF4's quad (src/ov01/gr_gp_init.c GrF4Vertices, TU-local there): four
 * corners and one colour at +0x40 (sb 0x40..0x43 in menuBpBarPut). */
typedef struct GrF4Vertices {
    Vector4 points[4]; /* +0x00 */
    MenuRGBA color;    /* +0x40 */
} GrF4Vertices;

extern void grOpenF4(GrPacket *packet);
extern void grPutF4(GrPacket *packet, GrF4Vertices *quad, const Matrix4 matrix);
extern void grCloseF4(void);

/*
 * The BP bar: an orange quad 4 pixels high, length pixels long, at
 * (x + 0x49, y + 0x36).
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

/*
 * One line of a key guide list (menu.data keyGuideL, keyGuideNull, ...;
 * still asm-owned): the pad button icon (uvTbl index, + 4 for its dimmed
 * variant) and the key command it triggers; a 0 button ends the list.
 */
typedef struct KeyGuideLine {
    int button; /* +0x00: lw $7,0x0($16) */
    int key;    /* +0x04: lw $6,0x4($16) */
} KeyGuideLine;

/*
 * One row of keyGuideTblChr/keyGuideTblAgws (8 bytes, sll $3,$16,3): the
 * guide lines and the 0-terminated button icon list of one guide mode.
 */
typedef struct KeyGuideMode {
    KeyGuideLine *lines; /* +0x00 */
    int *buttons;        /* +0x04 */
} KeyGuideMode;

extern KeyGuideMode keyGuideTblChr[];
extern KeyGuideMode keyGuideTblAgws[];

/* The key guide object's work: its screen position (lwc1 0x98/0x90). */
typedef struct MenuKeyGuideWork {
    unsigned char unmodeled_00[0x90];
    Vector4 position; /* +0x90 */
} MenuKeyGuideWork;

/* The battle actor flag the key guide requires (lw $2,0x0($3); andi 0x8). */
#define ACTOR_FLAG_KEY_GUIDE 0x8u

/* Control phase with no guide (beq $16,0x1A). */
#define KEY_GUIDE_PHASE_NONE 0x1A
/* First control phase that needs calcTuigekiChk (slti $2,$16,0xB). */
#define KEY_GUIDE_PHASE_TUIGEKI 0xB
/* Key command without a check (bne $6,0x1C). */
#define KEY_CMD_NO_CHECK 0x1C

extern int menuChk(void);
extern int calcTuigekiChk(ObjectTask *unit);
extern int keyCmdChk(ObjectTask *unit, BattleUnit *target, int key);
extern int battleCtrlHelpGet(void);
extern void grPacketSend(GrPacket *packet);
void keyGideDisp(GrPacket *packet, int x, int y, int button);
void menuKeyGuideHelp(GrPacket *packet, ObjectTask *unit, int key, int x, int y);
void menuKeyGuideVal(GrPacket *packet, ObjectTask *unit, int key, int x, int y);

/*
 * Draws the key guide of phase for unitTask: the button icons of the phase in a
 * row, then each guide line with its button (dimmed while keyCmdChk gives
 * 1, skipped for 2 or more or a negative result) and, unless the help
 * display is in state 2, its help text and values; then the guide footer.
 * Each table is read in its own branch, lines before buttons (the original
 * keeps sll $3,$16,3 in both branches and forms two addresses, addu $4,$2,$3
 * and addu $3,$3,$2); both lists are walked by index (daddu $16,$7,$0 copies
 * the base, the element is reloaded at the loop head); the range check is
 * two nested ifs (slti $2,$3,0x2 then bltzl $3).
 */
void menuKeyGuideDisp(ObjectTask *guideTask, ObjectTask *unitTask, int phase)
{
    GrPacket packet;
    MenuKeyGuideWork *work;
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
    if ((((BattleActor *)unitTask->work)->flags & ACTOR_FLAG_KEY_GUIDE) == 0) {
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
    x = (int)work->position.x;
    y = (int)work->position.z;
    grGpInit(&packet);
    menuStatEnvMake(&packet);
    target = tgtUnitGet();
    if (calcUPGet(unitTask)->flags & 0x40) {
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
 * One pad button icon's cell in the menu sheet (menu.data uvTbl, 8 entries
 * of 0x10 bytes; still asm-owned): keyGideDisp passes srcX/srcY (lw 0x0/0x4)
 * and width/height (lw 0x8/0xC) to menuStatSprite.
 */
typedef struct UvTblEntry {
    int srcX;   /* +0x00 */
    int srcY;   /* +0x04 */
    int width;  /* +0x08 */
    int height; /* +0x0C */
} UvTblEntry;

extern UvTblEntry uvTbl[];

/*
 * Draws pad button icon button (1-based: uvTbl[0] is button 1) at (x, y).
 * The index is decremented as its own statement: the original subtracts 1
 * before the shift (addiu $16,$16,-1; sll $16,$16,4) instead of folding the
 * -0x10 into the table address.
 */
void keyGideDisp(GrPacket *packet, int x, int y, int button)
{
    grOpenSpr(packet);
    button--;
    menuStatSprite(packet, x, y, uvTbl[button].width, uvTbl[button].height, uvTbl[button].srcX,
                   uvTbl[button].srcY);
    grCloseSpr();
}

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuGuideFrameDisp);

extern const char **dataEnemyNameGet(int index);
extern const char **dataPlayerNameGet(int id);
extern unsigned int strlen(const char *s);
void menuGuideFrameDisp(void *packet, int x, int y, int width, int height);
void menuFontEnvMake(void *packet);
void menuStatEnvMake(void *packet);
void menuFontPut(GrPacket *packet, int x, int y, const char *text);

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

/* battle_init.c (ov01), still INCLUDE_ASM there: the weapon slot and the
 * technique id a key command selects for a unit (-1: no weapon slot). */
extern int keyCmd2wno(ObjectTask *unit, int key);
extern int keyCmd2tid(ObjectTask *unit, int key);
/* calc.c (ov01): a unit's EP cost for a technique. */
extern int calcTakeEpGet(ObjectTask *unit, int tecId);

/*
 * dataTecGet's record (src/ov01/data_unit_org_get.c, TecRecord), as far as
 * menuKeyGuideVal reads it.
 */
typedef struct MenuTecData {
    unsigned char unmodeled_00[0x05];
    signed char extraRow;   /* +0x05: `lb $8,0x5($2)`, nonzero gives the key
                             * guide frame its second row */
    unsigned char unmodeled_06[0x11 - 0x06];
    signed char kind;       /* +0x11: `lb $3,0x11($2)`, compared with 6 */
} MenuTecData;

/*
 * dataWpnGet's record (src/ov01/data_unit_org_get.c, WpnRecord), as far as
 * menuKeyGuideVal reads it; names follow calc.c's WpnData view.
 */
typedef struct MenuWpnData {
    unsigned char unmodeled_00[0x0F];
    signed char apCost;     /* +0x0F: `lb $23,0xF($20)` */
    unsigned char equipped; /* +0x10: `lbu $2,0x10($20)` */
    unsigned char unmodeled_11[0x14 - 0x11];
    short tecId;            /* +0x14: `lh $5,0x14($20)`, calcTakeEpGet's argument */
} MenuWpnData;

extern MenuTecData *dataTecGet(int tecId);
extern MenuWpnData *dataWpnGet(int weaponId);

/* Technique kind whose AP cost is fixed at 6 instead of the weapon's. */
#define TEC_KIND_FIXED_AP 6

/*
 * The value panel of one key guide entry: the weapon slot's count, the EP
 * cost and the AP cost of the command bound to key. A technique command of
 * a unit without flag 0x40 shows only for TEC_KIND_FIXED_AP techniques.
 * The frame position is a variable stepped to the value row (valueX += 8,
 * valueY += 6): the original keeps y in $16 and adds 6 in place, and draws
 * the AP cell from that row ($18 + 0x42, + 0x54). The AP cell sits in a
 * do { } while (0) block: the original calls grCloseSpr with jal and
 * returns (no `j grCloseSpr` tail call) and sets the sprite's arguments
 * $5..$10 in order with `daddu $4,$17,$0` in the delay slot; a plain
 * statement list gives the tail call and another argument order.
 */
void menuKeyGuideVal(GrPacket *packet, ObjectTask *unit, int key, int x, int y)
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
    if ((calcUPGet(unit)->flags & 0x40) == 0) {
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
    grGsRegSet(packet, 0x42, 0);
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

void menuFontPutSub(GrPacket *packet, int x, int y, int charCode) {
    int row;
    int col;

    row = charCode / FONT_GLYPH_COLUMNS;
    col = charCode % FONT_GLYPH_COLUMNS;
    menuStatSprite(packet, x, y, FONT_GLYPH_WIDTH, FONT_GLYPH_HEIGHT,
                   col * FONT_GLYPH_WIDTH, row * FONT_GLYPH_HEIGHT);
}

extern int sscanf(const char *string, const char *format, ...);
extern const char D_00A46C08[]; /* "%X": menuFontPut parses a '#'-escaped hex glyph code */

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
                * their index, -1 for none, into the same slot (sw $2,0x10) */
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
            sscanf(hex, D_00A46C08, &glyph);
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
 * The cursor object's transform, the work's +0x90..+0x110 tail that
 * grCalcMatrix reads through grBBCalc (a second local view of
 * src/ov01/gr_gp_init.c's TU-local TransformState: position at +0x90,
 * offset at +0xa0; flagged for Opok). menuActCurPut saves and restores all
 * 0x80 bytes with aligned ld/sd loops (0x00A2B930, 0x00A2BA10), so the
 * storage is eight-byte aligned: the union gives it that alignment the
 * include/ov01/m_ef_create.h way. It writes position through
 * sefGetNullPosition (addiu $17,$2,0x90) and offset.y (swc1 $f0,0x14($17)).
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
 * (lw $5,0x14($6) at 0x00A2B964): a second local view of src/ov01/cmd.h's
 * TU-local CmdUnitRecord, flagged for Opok. */
typedef struct MenuUnitTask {
    ObjectTask task;   /* +0x00 */
    void *motionActor; /* +0x14 */
} MenuUnitTask;

/* src/main/sef.c, spelled as src/ov01/cmd.h and src/ov01/m_cam.c do. */
extern void sefGetNullPosition(Vector4 *position, void *actor, int part, int flag);
/* src/ov01/gr_gp_init.c takes a MatrixCommand *, whose only recovered member
 * (state at +0x10) is ObjectTask.work's slot; the cursor task is passed
 * as is (daddu $4,$19,$0). Flagged for Opok. */
extern int grBBCalc(ObjectTask *task, int *screenX, int *screenY);

/*
 * Draws the action cursor over a player unit: the cursor's transform is
 * moved to the unit's null position, raised by 1.3, projected to the screen,
 * and restored afterwards. Enemy units (BattleActor flag 0x40) get none.
 */
void menuActCurPut(GrPacket *packet, ObjectTask *cursor, ObjectTask *unit, int frame)
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
        menuStatEnvMake(packet);
        grOpenSpr(packet);
        menuStatSprite(packet, screenX - 8, screenY - 8, 16, 16, (frame % 8) * 16, 0x80);
        grCloseSpr();
        grPacketSend(packet);
    }
    *transform = saved;
}

void menuStatSprite(GrPacket *packet, int x, int y, int width, int height, int srcX, int srcY) {
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

void menuStatSprite2(GrPacket *packet, int x, int y, int width, int height, int srcLeft, int srcTop, int srcRight, int srcBottom) {
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

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuStatSprite2RGB);

/* dataXtxFileGet's record: only the texture id at +0x4 is read here. */
extern int *dataXtxFileGet(int index);
extern void xglPacketTextureTrans(int textureId);

void menuTexTrans(void) {
    GrPacket packet;

    xglPacketTextureTrans(dataXtxFileGet(1)[1]);
    grGpInit(&packet);
    grGsRegSet(&packet, 0x3F, 1);
    grPacketSend(&packet);
}

void menuStatTexTrans(void) {
    GrPacket packet;

    xglPacketTextureTrans(dataXtxFileGet(0)[1]);
    grGpInit(&packet);
    grGsRegSet(&packet, 0x3F, 1);
    grPacketSend(&packet);
}

/* Defined by ov01/tu008 (src/ov01/gr_gp_init.c). The six *EnvMake callers keep
 * their packet parameter as void *, which converts here. */
extern void GS_REG_SET(GrPacket *packet, long long register_id, long long control);
extern unsigned short dataXtxAdrGet(int index);

/*
 * The six *EnvMake functions below write one GIF packet each. No neighbouring
 * code names the GS registers or the packed words (menuStatTexTrans above and
 * encEnvMake pass raw numbers to grGsRegSet/GS_REG_SET), so they stay numeric:
 * - 0xE is the GIF A+D descriptor; its word is the GIFtag (NLOOP 5 for the five
 *   register writes that follow, EOP set, NREG 1), the same word
 *   gr_gp_init.c's grGifAppendRegister(packet, 14, ...) opens its packets with;
 * - 0x06 TEX0_1: tbp0 is the texture base, tbp1 << 37 the CLUT base (CBP); the
 *   constant carries the buffer width, pixel formats, size and function bits;
 * - 0x42 ALPHA_1 (blend equation; menuFontEnvMake's 0x44 differs from the 0 of
 *   the others), 0x47 TEST_1 (0x3000D: alpha test on, depth test on),
 *   0x08 CLAMP_1 and 0x14 TEX1_1 both 0.
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

INCLUDE_ASM("asm/nonmatchings/ov01/menu", menuATBEnvPalSet);



const signed char D_00A46AF8[232] = " 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz                                                                                    .    '                      /                   -                                 ";



const signed char D_00A46BE0[40] = "\241\277\241\241\241\246\241\241\241\260\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241";
