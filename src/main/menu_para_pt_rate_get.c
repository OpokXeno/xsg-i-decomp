#include "common.h"
#include "shared.h"
#include "main/xgl_task.h"

/*
 * The menu work block (window_tex_load's MenuWork data, 0x80 bytes). Only the
 * members tskCharSpotLight reads are modelled.
 * - state (+0x03): the MenuCharactor state (its jump table covers 0x10..0xFF);
 *   tskCharSpotLight and tskCharLineField remove themselves when it is 0xFF.
 * - partyMode (+0x31): MenuCharactor stores its argument here (sb at
 *   0x00297ccc); MainMenu calls MenuCharactor(0) and MenuCharactor(1).
 * - formationSlot (+0x46): the 1-based party position the spotlight marks,
 *   indexing the six-entry position table.
 */
/*
 * MenuCharactor keeps
 * every cursor and selection here and the sub-screens only read them:
 * - weaponId/bulletId/accessoryId (+0x12/+0x14/+0x16): the item under the
 *   open list's cursor; bulletFits (+0x18) records that bulletId belongs to
 *   the equipped weapon.
 * - leaderLocked (+0x19): why the shown character cannot become the
 *   on-screen one (0 free, 1 already on screen, 2 a reserve, 3 a guest), the
 *   MenuCharLeaderMask result; guestDenied (+0x1A) marks a refused guest.
 * - swapOutId/swapInId (+0x20/+0x21): the attacker and the reserve friend of
 *   the pending formation swap; modelSlot (+0x22) is the menu model slot and
 *   weaponChanged (+0x23) that the weapon line has to be rebuilt.
 * - partyCount/menuCount/reserveCount (+0x30/+0x32/+0x50): the number of
 *   rows of the party, command and reserve lists.
 * - equipSlot (+0x35): the equipment slot under the cursor, indexing the
 *   three accessories of CharParaData and selecting the bullet over the
 *   weapon; equipKind (+0x4A) is 0 for a weapon or bullet and 1 for an
 *   accessory, and accessoryList (+0x4F) the accessory sort order 0..2.
 * - partyCursor/commandCursor/menuCursor/confirmCursor/reserveCursor
 *   (+0x40/+0x41/+0x49/+0x7C/+0x51): the row of each list, and listSelect
 *   (+0x43) the row of the open item list, negative while nothing is picked.
 * - formationColumn/formationRow (+0x44/+0x45) build formationSlot (+0x46).
 * - wait (+0x48): frames before the pad is read again.
 * - chrNo (+0x60): the character the screens show (1-based).
 * - flags (+0x79), paraIndex (+0x7A), paraUpCount (+0x7B) and paraCost
 *   (+0x7E): the parameter being improved, how many levels are pending and
 *   what they cost.
 * The unknown_* members are only printed by MenuCharactor's debug dump.
 */
typedef struct MenuWorkData {
    unsigned char unmodeled_00[0x03];
    unsigned char state;            /* +0x03 */
    unsigned char unmodeled_04[0x12 - 0x04];
    unsigned short weaponId;        /* +0x12 */
    unsigned short bulletId;        /* +0x14 */
    unsigned short accessoryId;     /* +0x16 */
    unsigned char bulletFits;       /* +0x18 */
    signed char leaderLocked;       /* +0x19 */
    unsigned char guestDenied;      /* +0x1A */
    unsigned char unmodeled_1b[0x20 - 0x1B];
    unsigned char swapOutId;        /* +0x20 */
    signed char swapInId;           /* +0x21 */
    signed char modelSlot;          /* +0x22 */
    signed char weaponChanged;      /* +0x23 */
    unsigned char unmodeled_24[0x30 - 0x24];
    signed char partyCount;         /* +0x30 */
    signed char partyMode;          /* +0x31 */
    signed char menuCount;          /* +0x32 */
    unsigned char unmodeled_33[0x35 - 0x33];
    signed char equipSlot;          /* +0x35 */
    unsigned char unmodeled_36[0x40 - 0x36];
    signed char partyCursor;        /* +0x40 */
    signed char commandCursor;      /* +0x41 */
    unsigned char unmodeled_42;
    signed char listSelect;         /* +0x43 */
    signed char formationColumn;    /* +0x44 */
    signed char formationRow;       /* +0x45 */
    signed char formationSlot;      /* +0x46 */
    signed char unknown_47;
    unsigned char wait;             /* +0x48 */
    signed char menuCursor;         /* +0x49 */
    signed char equipKind;          /* +0x4A */
    signed char unknown_4b[2];
    unsigned char unmodeled_4d[0x4F - 0x4D];
    signed char accessoryList;      /* +0x4F */
    signed char reserveCount;       /* +0x50 */
    signed char reserveCursor;      /* +0x51 */
    signed char unknown_52[4];
    unsigned char unmodeled_56[0x60 - 0x56];
    /* +0x60: the character every screen of the block shows (1-based).
     * The read is a union member because 2.96 gives a reference through a
     * union alias set 0, and that is the dependence which keeps the lh of
     * this halfword at 0x0029301c behind the store of the command list
     * cursor at 0x00293014: declared as a plain short the load takes the
     * alias set of short, no longer conflicts with that int store, and the
     * scheduler moves it up, which reallocates ten words of
     * 0x00292f68..0x00293024 in CharMenuMain. Every other function of the
     * block is unchanged by the declaration. */
    union {
        short chrNo;
    } view;
    unsigned char unmodeled_62[0x79 - 0x62];
    unsigned char flags;            /* +0x79 */
    signed char paraIndex;          /* +0x7A */
    signed char paraUpCount;        /* +0x7B */
    signed char confirmCursor;      /* +0x7C */
    unsigned char unmodeled_7d;
    unsigned short paraCost;        /* +0x7E */
} MenuWorkData;

/*
 * CharPasMain reads two more bytes of the same trailing span: menuCursor
 * (+0x49), the category the command cursor stands on, and equipKind (+0x4A),
 * the equip sub-tab. It slides the tab of the selected category, and of the
 * sub-tab it opens, out of the strip (states 0x40-0x41, 0x52-0x53, 0x54-0x55).
 */

extern MenuWorkData MenuWork;

#define MENU_WORK_END 0xFF

/* MenuWork.state >> 4 for the 0xB0..0xBF states (jump-table cases 0xB2..0xB6
 * and 0xBF), the only ones in which the spotlight fades in. */
#define MENU_STATE_GROUP_FORMATION 11

/*
 * The spotlight task: the scheduler prefix, then the fields tskCharSpotLight
 * owns. MenuCharactor creates it with xglTaskEntryNext and clears mode
 * (sb zero,0x10 at 0x00297ca8).
 */
typedef struct CharSpotLightTask {
    XglTaskPrefix task;             /* +0x00 */
    unsigned char mode;             /* +0x10 */
    unsigned char visible;          /* +0x11 */
    unsigned char counter;          /* +0x12: cleared with visible on start */
    unsigned char unmodeled_13[0x20 - 0x13];
    Vector4 position;               /* +0x20: eases toward the table entry */
    float size;                     /* +0x30: 0..1 fade, radius weight 48 */
    float pulse;                    /* +0x34: 0..1 swing, radius weight 16 */
    float pulseStep;                /* +0x38: added to pulse every frame */
} CharSpotLightTask;

#define SPOTLIGHT_START    0
#define SPOTLIGHT_HIDDEN   2
#define SPOTLIGHT_SHOWN    10
#define SPOTLIGHT_FADE_OUT 20

/*
 * The circle endPrintDirectCircle draws. PrintCircleCore reads x/y (lh 0/2,
 * then adds 0x700/0x720 back at 0x0027dd94/0x0027ddf8), z (lw 4), the centre
 * RGBA (lbu 8..0xB), the radius (lw 0xC), the segment count (lw 0x10, the
 * loop bound) and the rim RGBA (lbu 0x14..0x17).
 */
typedef struct DirectCircle {
    short x;                        /* +0x00 */
    short y;                        /* +0x02 */
    int z;                          /* +0x04 */
    unsigned char innerColor[4];    /* +0x08 */
    int radius;                     /* +0x0C */
    int segments;                   /* +0x10 */
    unsigned char outerColor[4];    /* +0x14 */
} DirectCircle;

/* Screen-centre origin of the 12.4 fixed-point coordinates
 * xglRotTransPersN returns. */
#define CIRCLE_ORIGIN_X 0x700
#define CIRCLE_ORIGIN_Y 0x720

/*
 * The six spotlight positions (x = 1, 0, -1 on the z = 0.6 and z = -0.6 rows),
 * scaffold .rodata at 0x004C6770. tskCharSpotLight copies the whole table to
 * its frame with ld/sd and no alignment test (0x00297440..0x00297470), which
 * 2.96 emits only for a source type aligned to 8 bytes; the doubleword member
 * carries that alignment.
 */
typedef union SpotLightTable {
    Vector4 position[6];
    u64 doublewords[12];
} SpotLightTable;

extern const SpotLightTable D_004C6770;

/* Scaffold .lit4 constants: the pulse step (1/24) and the easing factor
 * (0.4) tskCharSpotLight loads gp-relative. */
extern const float D_004D7E50;
extern const float D_004D7E54;

/* xglRotTransPersN (VA 0x00229ab0): projects count points through the
 * optional matrix and the camera, writing four words per point. */
void xglRotTransPersN(int *screen, Matrix4 *matrix, Vector4 *points, int count, int cameraId);

/* endPrintDirectCircle (main/end_print.c) passes its argument on to
 * PrintCircleCore, which reads it as the DirectCircle above; this call site
 * passes the address of one. */
void endPrintDirectCircle(DirectCircle *circle);

/*
 * One character's 0x180-byte parameter record. dataUnitOrgGet (ov01 VA
 * 0x00a191c0) returns orgData + (chrNo - 1) * 0x180, and calcTotalParaMenu
 * (ov01 VA 0x00a11108) returns a recalculated copy of the same record.
 * calcTotalParaMenuSub clamps the stored hp (+0x34) and ep (+0x36) to the
 * recalculated maxHp (+0x00) and maxEp (+0x02) (lh/slt/sh at
 * 0x00a11180..0x00a111ac), and it fills the two
 * caller buffers MenuCharParaSet adds into attack (+0x04), phyDefense (+0x06)
 * and magDefense (+0x0A): three weapon attack words from calcWpnAtkGet and the
 * calcPhyDefGet/calcMagDefGet pair. MenuCharParaSet compares the three
 * halfwords at +0x64 with the item ids 95 and 96.
 */
/*
 * The parameter window shows two more values past the statIndex table
 * MenuParaPtNowGet covers (+0x0E and +0x1E), and the equipment screens read
 * the three equipment slots of the same record: the weapon (+0x5E) and the
 * bullet (+0x6A) of the slot setSlotTbl gives the character, and the three
 * accessories (+0x64), a zero id meaning an empty slot.
 */
typedef struct CharParaData {
    short maxHp;                /* +0x00 */
    short maxEp;                /* +0x02 */
    unsigned short attack;      /* +0x04 */
    unsigned short phyDefense;  /* +0x06 */
    unsigned short stat4;       /* +0x08: statIndex 4 of MenuParaPtNowGet */
    unsigned short magDefense;  /* +0x0A */
    signed char stat6;          /* +0x0C: statIndex 6 */
    signed char stat7;          /* +0x0D: statIndex 7 */
    signed char stat8;          /* +0x0E */
    unsigned char unmodeled_0f[0x1E - 0x0F];
    short stat9;                /* +0x1E */
    unsigned char unmodeled_20[0x34 - 0x20];
    short hp;                   /* +0x34 */
    short ep;                   /* +0x36 */
    unsigned char unmodeled_38[0x5E - 0x38];
    short weapon[3];            /* +0x5E: indexed by setSlotTbl */
    short accessory[3];         /* +0x64 */
    unsigned short bullet[3];   /* +0x6A: indexed by setSlotTbl */
    unsigned char unmodeled_70[0x180 - 0x70];
} CharParaData;

/* Accessory ids after which MenuCharParaSet restores an hp/ep that was above
 * the recalculated maximum. */
#define ACCESSORY_KEEP_HP 95
#define ACCESSORY_KEEP_EP 96

/* The menu's two parameter records: MenuCharParaSet always writes the
 * recalculated record to [1] and, unless previewOnly, copies it to [0] too. */
extern CharParaData paraUnit[2];

/* Weapon slot per party character (chrNo 1..7), indexing calcTotalParaMenu's
 * attack words. */
extern unsigned char setSlotTbl[];

/* dataUnitOrgGet (ov01 VA 0x00a191c0), linked by the scaffold's
 * undefined_funcs_auto.txt placeholder name. */
CharParaData *func_A191C0(int chrNo);

/* calcTotalParaMenu (ov01 VA 0x00a11108), linked by the scaffold's label for
 * that address. */
CharParaData *func_00A11108(int chrNo, int *attack, int *defense);

/*
 * ParaDataBuf is a pointer to a block of parallel per-character halfword
 * tables (one such table per stat display, e.g. base); each character's row
 * is 8 halfwords wide. ParaDataChangeTbl (8 bytes) remaps a stat index
 * (statIndex) to its halfword slot within that row. MenuParaPtBaseGet reads
 * the computed slot from a table 56 halfwords after ParaDataBuf.
 */
extern unsigned short *ParaDataBuf;
extern unsigned char ParaDataChangeTbl[];

/* The rate table starts one character row (8 halfwords) before the row slot
 * ParaDataBuf addresses, so character chrNo (1-based) reads row chrNo - 1. */
unsigned short MenuParaPtRateGet(int chrNo, int statIndex)
{
    unsigned short *entry = ParaDataBuf + chrNo * 8 + ParaDataChangeTbl[statIndex] - 8;
    return *entry;
}

unsigned short MenuParaPtBaseGet(int chrNo, int statIndex)
{
    unsigned short *table = ParaDataBuf + 56;
    int slot = chrNo * 8 + ParaDataChangeTbl[statIndex];
    return table[slot];
}

/*
 * The character's current value of one displayed parameter (statIndex 0..7:
 * maxHp, maxEp, attack, phyDefense, stat4, magDefense, stat6, stat7); 0 for
 * any other index.
 */
unsigned short MenuParaPtNowGet(int chrNo, int statIndex)
{
    CharParaData *org = func_A191C0(chrNo);

    switch (statIndex) {
    case 2:
        return org->attack;
    case 3:
        return org->phyDefense;
    case 4:
        return org->stat4;
    case 5:
        return org->magDefense;
    case 6:
        return org->stat6;
    case 7:
        return org->stat7;
    case 0:
        return org->maxHp;
    case 1:
        return org->maxEp;
    }
    return 0;
}

int PartyFriendCheck(int id);

/*
 * The highest current value of one parameter among the characters in the
 * party (ids 1..7), rounded down to a multiple of 10 for maxHp and of 2 for
 * maxEp.
 */
unsigned short MenuParaUpMaxGet(int statIndex)
{
    unsigned short max = 0;
    int chrNo;

    for (chrNo = 1; chrNo < 8; chrNo++) {
        if (PartyFriendCheck(chrNo) != 0) {
            unsigned short now = MenuParaPtNowGet(chrNo, statIndex);

            if (max < now) {
                max = now;
            }
        }
    }
    switch (statIndex) {
    case 0:
        max = max / 10 * 10;
        break;
    case 1:
        max = max / 2 * 2;
        break;
    }
    return max;
}

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", MenuParaNextPointGet);

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", MenuParaUp);

/* The character's running record: the unspent parameter points at +0x0C. */
typedef struct CharPointData {
    unsigned char unmodeled_00[0x0C];
    int points;                     /* +0x0C */
    /* The two other pools the same record carries, each displayed by the "Ex"
     * bar of the screen that spends it: MenuEtherExMain shows the word at
     * +0x10 (lw at 0x002ab340) and MenuSkillExMain the word at +0x14 (lw at
     * 0x002b8018), while MenuTecExMain shows points itself (lw at
     * 0x002b08bc). CharExMain shows all three. */
    int etherPoints;                /* +0x10 */
    int skillPoints;                /* +0x14 */
} CharPointData;

/* dataPlChaGet (ov01 VA 0x00a19210; see src/main/menu_skill.h and
 * src/main/menu_tec.c, which model this same per-character record under the
 * same scaffold label). Kept under the scaffold's undefined_funcs_auto.txt
 * placeholder name func_A19210 until ov01 recovers dataPlChaGet itself. */
CharPointData *func_A19210(int chrNo);

int MenuParaNextPointGet(int chrNo, int statIndex, int count);

/*
 * Whether the parameter can be raised once more: -2 when it has reached the
 * party maximum, -1 when the character lacks the points, 0 otherwise.
 */
int MenuParaUpCheck(int chrNo, int statIndex)
{
    CharPointData *unit;

    if (MenuParaPtNowGet(chrNo, statIndex) >= MenuParaUpMaxGet(statIndex)) {
        return -2;
    }
    unit = func_A19210(chrNo);
    if (unit->points < MenuParaNextPointGet(chrNo, statIndex, 0)) {
        return -1;
    }
    return 0;
}

int MenuCharWeaponAttCheck(void) {
    return 0;
}

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", MenuCharLeaderMask);

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", MenuPasLengthGet);

/*
 * CharPasWindow is the sub-window control block WindowDXSet/WindowDXMain
 * share with every other "*Main" screen task (see e.g. MoveSlide's
 * work->window.x use elsewhere); only the members CharPasMain itself sets
 * are modeled. callback/callbackArg are the members WindowDXMain calls once
 * non-null; CharPasMain points them at the empty MenuPasWindow stub and at
 * the trailing callbackWork span of its own work block.
 */
typedef struct CharPasWindow {
    short x;                           /* +0x00 */
    short y;                           /* +0x02 */
    int color;                         /* +0x04 */
    short width;                       /* +0x08 */
    short height;                      /* +0x0A */
    unsigned char unmodeled_0c[0x10 - 0x0C];
    unsigned char state;               /* +0x10 */
    unsigned char unmodeled_11[0x14 - 0x11];
    void (*callback)(void);            /* +0x14 */
    void *callbackArg;                 /* +0x18 */
} CharPasWindow;

/*
 * One category-tab caption. eMessageSet/eMessageMain read/write the rest of
 * this record; only the members CharPasMain itself sets are modeled.
 */
typedef struct CharPasMessage {
    unsigned char unmodeled_00;
    unsigned char mode;                /* +0x01 */
    unsigned char unmodeled_02[0x04 - 0x02];
    short x;                           /* +0x04 */
    short y;                           /* +0x06 */
    int color;                         /* +0x08 */
    unsigned char unmodeled_0c[0x44 - 0x0C];
} CharPasMessage;

/* The inset frame endPrintExtFunc draws behind the tab strip. */
typedef struct CharPasBox {
    short x;                           /* +0x00 */
    short y;                           /* +0x02 */
    int kind;                          /* +0x04 */
    short width;                       /* +0x08 */
    short height;                      /* +0x0A */
} CharPasBox;

/*
 * The character-pass screen's own work block: MenuCharactor carves it as a
 * 0x550-byte block of the menu heap (CHAR_PAS_SIZE) and clears only its
 * first byte (state) before handing it to CharPasMain. Only the members
 * CharPasMain itself reads or writes are modeled.
 */
typedef struct CharPasWork {
    unsigned char state;               /* +0x00 */
    unsigned char unmodeled_01[0x03 - 0x01];
    unsigned char tabCount;            /* +0x03 */
    int kind;                          /* +0x04 */
    CharPasWindow window;              /* +0x08 */
    unsigned char unmodeled_24[0x19C - 0x24];
    CharPasMessage messages[12];       /* +0x19C */
    CharPasBox box;                    /* +0x4CC */
    unsigned char unmodeled_4d8[0x4E0 - 0x4D8];
    unsigned char callbackWork[0x550 - 0x4E0];  /* +0x4E0 */
} CharPasWork;

extern CharPasWork *CharPas;

/* The twelve normal-menu category captions ("Characters", "/Equip", ...). */
extern const char *const msg_0_0036D8A8[12];
/* The three battle-formation category captions. */
extern const char *const msg01_1[3];

/* MenuPasLengthGet (this TU, still assembler): the caption's pixel width. */
int MenuPasLengthGet(const char *text);

/* MenuPasWindow (main/menu_top_command_check.c): an empty callback stub. */
void MenuPasWindow(void);

/* MoveSlide (main/window_tex_load.c): eases a short position toward its
 * target at 1/rate of the remaining distance. */
void MoveSlide(short *current, short *target, float rate);

/*
 * The display elements every "*Main" screen of this menu builds: the
 * sub-window WindowDXSet clears and WindowDXMain draws, the text line, the
 * icon, the selection cursor, the number, the label and the ribbon. Each
 * screen models only the members of the element record it writes itself, so
 * the element is passed untyped and every screen keeps its own view below
 * (main/e_battle_win_open.c, main/e_message.c and main/e_number_main.c own
 * the records).
 */
void WindowDXSet(void *window);
void WindowDXMain(void *window);
void eMessageSet(void *message, const char *text);
void eMessageMain(void *message);
void eSpriteSet(void *sprite, short spriteId);
void eSpriteMain(void *sprite);
void eCursolSet(void *cursor, signed char width);
void eCursolMain(void *cursor);
/* eNumberSet, eTagFontSet and eRibbonSet take a second argument every call
 * site of this menu passes and the initialisers themselves leave unused
 * (move a1,zero at 0x002931e8, daddu $5,$0,$0 at 0x00295aa8); eTagFontSet
 * takes the tag word of the label it draws. */
void eNumberSet(void *number, int mode);
void eNumberMain(void *number);
void eTagFontSet(void *tag, int tag_word);
void eTagFontMain(void *tag);
void eRibbonSet(void *ribbon, int mode);
void eRibbonMain(void *ribbon);
void endPrintExtFunc(int kind, int id, void *data);

/*
 * The category-tab strip at the top of the character menu. MenuWork.state
 * selects which sub-screen currently owns the display; CharPasMain hides
 * its own strip (slides it fully off-screen) unless MenuWork.state falls in
 * its own range, and highlights the incoming tab during the handful of
 * transition states that carry a category index.
 */
void CharPasMain(void)
{
    CharPasWork *pas = CharPas;
    int tabLength[12];
    int formTabLength[3];
    short tabTarget[16];
    short slideTarget;
    int i;

    for (i = 0; i < 12; i++)
        tabLength[i] = MenuPasLengthGet(msg_0_0036D8A8[i]);
    for (i = 0; i < 3; i++)
        formTabLength[i] = MenuPasLengthGet(msg01_1[i]);

    switch (pas->state) {
    case 0:
        pas->kind = 0x00FFFFF0;
        WindowDXSet(&pas->window);
        pas->window.x = -0x120;
        pas->window.y = 8;
        pas->window.color = pas->kind;
        if (MenuWork.partyMode == 1) {
            pas->window.width = 0x130;
        } else {
            pas->window.width = 0x110;
        }
        pas->window.height = 0x1E;
        pas->window.callback = MenuPasWindow;
        pas->window.callbackArg = pas->callbackWork;
        pas->window.state = 1;
        WindowDXMain(&pas->window);
        pas->window.state = 3;
        if (MenuWork.partyMode == 0) {
            for (i = 0; i < 12; i++) {
                eMessageSet(&pas->messages[i], msg_0_0036D8A8[i]);
                pas->messages[i].mode = 0x20;
            }
            for (i = 0; i < 12; i++) {
                pas->messages[i].x = 0x120;
                pas->messages[i].y = 0xB;
                pas->messages[i].color = pas->kind + 2;
            }
            pas->tabCount = 12;
        } else {
            for (i = 0; i < 3; i++) {
                eMessageSet(&pas->messages[i], msg01_1[i]);
                pas->messages[i].mode = 0x20;
            }
            for (i = 0; i < 3; i++) {
                pas->messages[i].x = 0x120;
                pas->messages[i].y = 0xB;
                pas->messages[i].color = pas->kind + 2;
            }
            pas->tabCount = 3;
        }
        pas->state = 2;
        /* fallthrough */
    case 2:
        break;
    default:
        return;
    }

    slideTarget = -0x10;
    for (i = 0; i < pas->tabCount; i++)
        tabTarget[i] = 0x120;

    switch (MenuWork.state) {
    case 0x40: case 0x41: {
        int curTab = MenuWork.menuCursor;

        tabTarget[0] = 0x10;
        tabTarget[curTab + 1] = tabLength[0] + 0x10;
        break;
    }
    case 0x52: case 0x53: {
        int curTab = MenuWork.menuCursor;

        tabTarget[0] = -(tabLength[0] + 0x10);
        tabTarget[curTab + 1] = 0x10;
        tabTarget[MenuWork.equipKind + 5] = tabLength[curTab + 1] + 0x10;
        break;
    }
    case 0x54: case 0x55: {
        int curTab = MenuWork.menuCursor;

        tabTarget[0] = -(tabLength[0] + 0x10);
        tabTarget[curTab + 1] = 0x10;
        tabTarget[7] = tabLength[curTab + 1] + 0x10;
        break;
    }
    case 0x10: case 0x11: case 0x20: case 0x21:
    case 0x80: case 0x81: case 0x82: case 0x83:
    case 0x84: case 0x85: case 0x86: case 0x87:
    case 0xA0: case 0xA1: case 0xA2: case 0xA3:
        tabTarget[0] = 0x10;
        break;
    case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB6: case 0xBF:
        tabTarget[0] = 0x10;
        tabTarget[1] = formTabLength[0] + 0x10;
        break;
    case 0xC2: case 0xC3: case 0xC4: case 0xC5:
        tabTarget[0] = 0x10;
        tabTarget[2] = formTabLength[0] + 0x10;
        break;
    default:
        slideTarget = -0x120;
        break;
    }

    MoveSlide(&pas->window.x, &slideTarget, 3.0f);
    WindowDXMain(&pas->window);
    pas->box.x = pas->window.x + 3;
    pas->box.y = pas->window.y + 3;
    pas->box.kind = pas->kind;
    pas->box.width = pas->window.width - 6;
    pas->box.height = pas->window.height - 6;
    endPrintExtFunc(pas->kind, 0x65, &pas->box);
    for (i = 0; i < pas->tabCount; i++) {
        MoveSlide(&pas->messages[i].x, &tabTarget[i], 5.0f);
        if (pas->messages[i].x < 0x100)
            eMessageMain(&pas->messages[i]);
    }
    endPrintExtFunc(pas->kind, 0x66, 0);
}

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", MenuCharEquipCalcPointGet);

/*
 * PARTIAL ACCESSED PREFIX of PadData (the 0xD0-byte pad block at 0x00490d90):
 * the halfwords this menu reads. held/pressed are the two the shared
 * include/shared.h PadPrefix view names (half_28/half_2a); repeat carries the
 * direction auto-repeat and repeatFirst equals it only on the frame a
 * direction is first pressed, so a list wraps only on a fresh press.
 *
 * header_divergence (recorded, allowed): a local extension of PadData past
 * PadPrefix, the same situation documented in src/main/game.h's
 * PadDataDebugLayout and src/main/set_path.c's PadDataEffectLayout.
 */
typedef struct MenuPadData {
    unsigned char unmodeled_00[0x28];
    unsigned short held;            /* +0x28 */
    unsigned short pressed;         /* +0x2A */
    unsigned char unmodeled_2c[0x32 - 0x2C];
    unsigned short repeatFirst;     /* +0x32 */
    unsigned short repeat;          /* +0x34 */
    unsigned char unmodeled_36[0xD0 - 0x36];
} MenuPadData;

extern MenuPadData PadData;

#define PAD_L2       0x0001
#define PAD_R2       0x0002
#define PAD_L1       0x0004
#define PAD_R1       0x0008
#define PAD_CIRCLE   0x0020
#define PAD_CROSS    0x0040
#define PAD_SQUARE   0x0080
#define PAD_UP       0x1000
#define PAD_RIGHT    0x2000
#define PAD_DOWN     0x4000
#define PAD_LEFT     0x8000
/*
 * The sub-window control block WindowDXSet/WindowDXMain/WindowDXFlagChange
 * (main/e_battle_win_open.c) manage, embedded at +0x08 of the help window's
 * work block; src/main/menu_shop.c names x/y/color/width/height of the same
 * block in its own view, and state (+0x10) is the member WindowDXFlagChange
 * writes. WindowDXMain calls callback(window, callbackArg) every frame: here
 * MenuInfoWindow, which switches on window->state (lb 0x10 at 0x0027a12c) and
 * hands the text of callbackArg (lw 0xC at 0x0027a158) to eMessageSet.
 */
typedef struct CharInfoWindow CharInfoWindow;

/*
 * The record MenuInfoWindow draws one line from. Only the two coordinates the
 * setup clears and the text pointer are modelled; from +0x10 on it is the
 * eMessage block MenuInfoWindow fills itself.
 */
typedef struct CharInfoText {
    short x;                        /* +0x00 */
    short y;                        /* +0x02 */
    unsigned char unmodeled_04[0x0C - 0x04];
    char *text;                     /* +0x0C */
    unsigned char unmodeled_10[0x1B0 - 0x10];
} CharInfoText;

struct CharInfoWindow {
    short x;                        /* +0x00 */
    short y;                        /* +0x02 */
    int color;                      /* +0x04 */
    short width;                    /* +0x08 */
    short height;                   /* +0x0A */
    unsigned char unmodeled_0c[0x10 - 0x0C];
    unsigned char state;            /* +0x10 */
    unsigned char unmodeled_11[0x14 - 0x11];
    void (*callback)(CharInfoWindow *window, CharInfoText *text); /* +0x14 */
    CharInfoText *callbackArg;      /* +0x18 */
    unsigned char unmodeled_1c[0x198 - 0x1C];
};

/*
 * The help window's work block, the 0x450 bytes MenuCharactor carves out of
 * the menu heap. state is 0 for the frame it was carved in and 2 from then on.
 * opened and selected are written once by the setup and no recovered function
 * reads them back. reasonShown remembers that the line in the buffer is the
 * explanation of a locked character, which stays until the next pad input.
 */
typedef struct CharInfoWork {
    unsigned char state;            /* +0x00 */
    unsigned char opened;           /* +0x01 */
    signed char selected;           /* +0x02 */
    unsigned char reasonShown;      /* +0x03 */
    int color;                      /* +0x04 */
    CharInfoWindow window;          /* +0x08 */
    CharInfoText info;              /* +0x1A0 */
    char message[0x100];            /* +0x350 */
} CharInfoWork;

#define CHAR_INFO_SETUP 0
#define CHAR_INFO_MAIN  2

extern CharInfoWork *CharInfo;

/*
 * One entry of the text table MenuTextGet indexes: the display name at +0x00
 * (main/menu_1.c models the same record with its sort name at +0x08) and, at
 * +0x04, the description line this help window prints. The index carries the
 * category in its high halfword and the item or character id in its low one.
 */
typedef struct MenuTextRecord {
    char *name;                     /* +0x00 */
    char *description;              /* +0x04 */
} MenuTextRecord;

#define MENU_TEXT_WEAPON    0x30000
#define MENU_TEXT_BULLET    0x40000
#define MENU_TEXT_ACCESSORY 0x50000
#define MENU_TEXT_CHARACTER 0x60000

/* The step one improvement adds to each parameter, and the character ids of
 * the party positions and of the reserve list. */
extern unsigned short UpPara[8];
extern unsigned short MenuCharParty[8];
extern unsigned short MenuCharPartyReser[4];

int MenuCharEquipCalcPointGet(int chrNo);
void MenuInfoWindow(CharInfoWindow *window, CharInfoText *text);
void WindowDXFlagChange(void *window, int flag);
void eMessageCpy(char *destination, char *source);
void eMessageCat(char *source);
char *MenuCharNameGet(int chrNo);
char *MenuParaNameGet(int statIndex);
char *MenuNumberTextGet(int value, int width, int mode);
MenuTextRecord *MenuTextGet(int index);
int MenuSortGet(int listIndex, int entryIndex);
int PartyAttackPosCheck(int attackPosition);
int strcmp(const char *first, const char *second);

/*
 * The help line at the bottom of the character menu: the window is built on
 * the frame MenuCharactor carves its work block, and from then on every frame
 * writes the line MenuCharactor's current state asks for into the block's own
 * message buffer and slides the window to that state's height.
 */
void CharInfoMain(void)
{
    char *menuHelp[3] = {
        "Equip weapons and accessories.",
        "Unequip accessories.",
        "Use T.Pts to improve character stats."
    };
    char *equipHelp[4] = {
        "/",
        "Empty slot. A weapon can be equipped.",
        "Empty slot. An accessory can be equipped.",
        "Accessory"
    };
    char *charHelp[2] = {
        "Please select a character.",
        " can't be chosen."
    };
    char *partyHelp[3] = {
        "Change the battle formation of the combatant.",
        "Replace the combatant with reserves.",
        "Cancel."
    };
    char *formationHelp[6] = {
        "Select a new location for your character and press \x1E\x01.",
        "Change places with ",
        "Select a character to replace and press \x1E\x01.",
        "Move ",
        "?\nPress \x1E\x01 to accept.",
        " here?\nPress \x1E\x01 to accept."
    };
    /*
     * The confirmation of a reserve swap names two characters, and the table
     * keeps the pieces that go around those names twice: entry 0 what precedes
     * a name and entry 1 what follows it. The accept line comes in both rows
     * because a name that already ends in a period needs none of its own.
     */
    struct ReserveHelp {
        char *firstName;            /* the piece printed with the first name */
        char *secondName;           /* the piece printed with the second name */
        char *accept;               /* the line that asks for the button */
    } reserveHelp[2] = {
        { "Remove ", "and replace with ", ".\nPress \x1E\x01 to accept." },
        { " ",       ".",                 "\nPress \x1E\x01 to accept." }
    };
    char *improveHelp[10] = {
        "Improve ",
        "Please specify points. \n",
        "The maximum for ",
        " is ",
        " will be increased from ",
        " to ",
        "",
        " has been increased. \x1F",
        "You do not have enough T.Pts for further improvements.",
        "This is the maximum for "
    };
    char *leaderHelp[5] = {
        "Select a character.",
        "A new on-screen character has been chosen.",
        "Reserves cannot be chosen as on-screen characters.",
        "This is the current on-screen character.",
        " is not available as an on-screen character."
    };
    CharInfoWork *work = CharInfo;
    int totals[3];
    short slideTarget;
    CharParaData *para;

    switch (work->state) {
    case CHAR_INFO_SETUP:
        work->color = 0x00FFFFF0;
        WindowDXSet(&work->window);
        work->window.x = -16;
        work->window.color = work->color;
        work->window.y = 480;
        work->window.width = 544;
        work->window.height = 54;
        work->window.callback = MenuInfoWindow;
        work->window.callbackArg = &work->info;
        work->info.x = 0;
        work->info.y = 0;
        work->info.text = 0;
        work->window.state = 1;
        WindowDXMain(&work->window);
        work->window.state = 3;
        work->selected = -1;
        work->opened = 1;
        work->reasonShown = 0;
        memset(work->message, 0, sizeof(work->message));
        work->state = CHAR_INFO_MAIN;
        /* fallthrough */
    case CHAR_INFO_MAIN:
        para = func_00A11108(MenuWork.view.chrNo, totals, totals);
        slideTarget = 386;
        switch (MenuWork.state) {
        case 0x10:
        case 0x11:
            if (work->reasonShown == 0) {
                eMessageCpy(work->message, leaderHelp[0]);
                if (PadData.repeatFirst & PAD_SQUARE) {
                    switch (MenuWork.leaderLocked) {
                    case 0:
                        eMessageCpy(work->message, leaderHelp[1]);
                        break;
                    case 1:
                        eMessageCpy(work->message, MenuCharNameGet(MenuWork.view.chrNo));
                        eMessageCat(leaderHelp[4]);
                        break;
                    case 2:
                        eMessageCpy(work->message, leaderHelp[2]);
                        break;
                    case 3:
                        eMessageCpy(work->message, leaderHelp[3]);
                        break;
                    }
                    work->reasonShown = 1;
                }
            } else if (PadData.repeatFirst != 0) {
                work->reasonShown = 0;
            }
            break;
        case 0x20:
        case 0x21:
            eMessageCpy(work->message, menuHelp[MenuWork.menuCursor]);
            break;
        case 0x40:
        case 0x41:
            if (MenuWork.equipKind == 0) {
                int slot = setSlotTbl[MenuWork.view.chrNo - 1];
                short weapon = para->weapon[slot];

                if (weapon != 0) {
                    if (MenuWork.equipSlot == 0) {
                        eMessageCpy(work->message,
                                    MenuTextGet(MENU_TEXT_WEAPON + (unsigned short)weapon)->description);
                    } else {
                        eMessageCpy(work->message,
                                    MenuTextGet(MENU_TEXT_BULLET
                                                + (unsigned short)para->bullet[slot])->description);
                    }
                } else {
                    eMessageCpy(work->message, equipHelp[1]);
                }
            } else {
                short accessory = para->accessory[MenuWork.equipSlot];

                if (accessory != 0) {
                    eMessageCpy(work->message,
                                MenuTextGet(MENU_TEXT_ACCESSORY + (unsigned short)accessory)->description);
                } else {
                    eMessageCpy(work->message, equipHelp[2]);
                }
            }
            break;
        case 0x52:
        case 0x53:
        case 0x54:
        case 0x55:
            if (MenuWork.listSelect >= 0) {
                eMessageCpy(work->message,
                            MenuTextGet(MenuSortGet(0, MenuWork.listSelect))->description);
            } else {
                eMessageCpy(work->message, "");
            }
            break;
        case 0x80:
        case 0x81:
            if (MenuWork.listSelect >= 0) {
                eMessageCpy(work->message, improveHelp[0]);
                eMessageCat(MenuParaNameGet(MenuWork.listSelect));
                eMessageCat(".");
            } else {
                eMessageCpy(work->message, "");
            }
            break;
        case 0x82:
        case 0x83:
            {
                int max = MenuCharEquipCalcPointGet(MenuWork.view.chrNo)
                          + MenuParaUpMaxGet(MenuWork.paraIndex);

                eMessageCpy(work->message, improveHelp[1]);
                eMessageCat(improveHelp[2]);
                eMessageCat(MenuParaNameGet(MenuWork.listSelect));
                eMessageCat(improveHelp[3]);
                eMessageCat(MenuNumberTextGet(max, -1, 0));
                eMessageCat(".");
            }
            break;
        case 0x84:
        case 0x85:
            {
                int now = MenuParaPtNowGet(MenuWork.view.chrNo, MenuWork.paraIndex)
                          + MenuCharEquipCalcPointGet(MenuWork.view.chrNo);
                int next = now + UpPara[MenuWork.paraIndex];

                eMessageCpy(work->message, MenuParaNameGet(MenuWork.paraIndex));
                eMessageCat(improveHelp[4]);
                eMessageCat(MenuNumberTextGet(now, -1, 0));
                eMessageCat(improveHelp[5]);
                eMessageCat(MenuNumberTextGet(next, -1, 0));
                eMessageCat(".");
            }
            break;
        case 0x86:
            eMessageCpy(work->message, MenuParaNameGet(MenuWork.listSelect));
            eMessageCat(improveHelp[7]);
            break;
        case 0x87:
            if (MenuParaUpCheck(MenuWork.view.chrNo, MenuWork.paraIndex) == -1) {
                eMessageCpy(work->message, improveHelp[8]);
            } else {
                eMessageCpy(work->message, improveHelp[9]);
                eMessageCat(MenuParaNameGet(MenuWork.paraIndex));
                eMessageCat(".");
            }
            break;
        case 0xA0:
            eMessageCpy(work->message, charHelp[0]);
            WindowDXFlagChange(&work->window, 1);
            break;
        case 0xA1:
            break;
        case 0xA2:
        case 0xA3:
            eMessageCpy(work->message, partyHelp[MenuWork.commandCursor]);
            break;
        case 0xB2:
            eMessageCpy(work->message, formationHelp[0]);
            break;
        case 0xB3:
            break;
        case 0xB4:
        case 0xB5:
            eMessageCpy(work->message, formationHelp[1]);
            eMessageCat(MenuCharNameGet(PartyAttackPosCheck(MenuWork.formationSlot)));
            eMessageCat(formationHelp[4]);
            break;
        case 0xB6:
            eMessageCpy(work->message, formationHelp[3]);
            eMessageCat(MenuCharNameGet(MenuWork.view.chrNo));
            eMessageCat(formationHelp[5]);
            break;
        case 0xBF:
            break;
        case 0xC0:
            eMessageCpy(work->message, formationHelp[2]);
            break;
        case 0xC1:
            break;
        case 0xC2:
        case 0xC3:
            {
                int partyText = MENU_TEXT_CHARACTER + MenuCharParty[MenuWork.partyCursor];
                int reserveText = MENU_TEXT_CHARACTER + MenuCharPartyReser[MenuWork.reserveCursor];

                eMessageCpy(work->message, reserveHelp[0].firstName);
                eMessageCat(MenuTextGet(partyText)->name);
                eMessageCat(reserveHelp[1].firstName);
                eMessageCat(reserveHelp[0].secondName);
                eMessageCat(MenuTextGet(reserveText)->name);
                if (strcmp(MenuTextGet(reserveText)->name, "Jr.") != 0) {
                    eMessageCat(reserveHelp[0].accept);
                } else {
                    eMessageCat(reserveHelp[1].accept);
                }
            }
            break;
        case 0xC4:
        case 0xC5:
            {
                int partyText = MENU_TEXT_CHARACTER + MenuCharParty[MenuWork.partyCursor];
                int reserveText = MENU_TEXT_CHARACTER + MenuCharPartyReser[MenuWork.reserveCursor];

                eMessageCpy(work->message, reserveHelp[0].firstName);
                eMessageCat(MenuTextGet(reserveText)->name);
                eMessageCat(reserveHelp[1].firstName);
                eMessageCat(reserveHelp[0].secondName);
                eMessageCat(MenuTextGet(partyText)->name);
                if (strcmp(MenuTextGet(partyText)->name, "Jr.") != 0) {
                    eMessageCat(reserveHelp[1].secondName);
                }
            }
            break;
        default:
            slideTarget = 512;
            eMessageCpy(work->message, "");
            break;
        }
        work->info.text = work->message;
        MoveSlide(&work->window.y, &slideTarget, 3.0f);
        WindowDXMain(&work->window);
        break;
    }
}
/*
 * One of CharCategory's five equip-category icons (weapon/bullet/accessory/
 * status/point). eSpriteSet and eSpriteMain (main/e_number_main.c, not yet
 * published) own the fields CharCategoryMain does not touch; the sprite
 * object they operate on starts at +0x00 of this record (no header bytes
 * precede it), so the array stride is the record size.
 */
typedef struct CharCategoryIcon {
    unsigned char unmodeled_00[0x04]; /* eSpriteSet's own fields */
    short x;                          /* +0x04 */
    short y;                          /* +0x06 */
    int work;                         /* +0x08 */
    signed char color[3];             /* +0x0C: r,g,b tint */
    unsigned char alpha;              /* +0x0F */
    unsigned char unmodeled_10[0x28 - 0x10]; /* eSpriteMain's own fields */
} CharCategoryIcon;

/*
 * The 0x244-byte block MenuCharactor carves for the category screen.
 * edgeDelta[0]/[1] nudge icons[0]/icons[1] left/right for a few frames after
 * the pad repeats PAD_LEFT/PAD_RIGHT; icons[2..4] dim to 0x40 or brighten to
 * -0x80 to mark the one matching MenuWork.accessoryList.
 */
typedef struct CharCategoryWork {
    unsigned char state;          /* +0x00: 0 needs init, 2 active */
    unsigned char phase;          /* +0x01: cleared with state on init */
    unsigned char pulseCounter;   /* +0x02: 0..8, drives the icon nudge and fade */
    unsigned char tickCounter;    /* +0x03: increments every call */
    int pendingReset;             /* +0x04: nonzero selects the wide icon layout */
    int iconResetWork;            /* +0x08: seeds every icon's work field */
    unsigned short flashLevel;    /* +0x0C: 0..10 bounce, stepped every other tick */
    unsigned short flashStep;     /* +0x0E: 1 while rising, 0xFFFF while falling */
    signed char edgeDelta[2];     /* +0x10: icons[0]/icons[1] pad nudge */
    unsigned char unmodeled_12[0x14 - 0x12];
    CharCategoryIcon icons[5];    /* +0x14 */
    unsigned char unmodeled_b4[0x244 - 0xB4];
} CharCategoryWork;

extern CharCategoryWork *CharCategory;

/* Scaffold .rodata: the five icon sprite ids CharCategoryMain assigns at init. */
typedef struct CharCategoryIconIdTable {
    short id[5];
} CharCategoryIconIdTable;
extern const CharCategoryIconIdTable D_004C6048;

float xglCos(float radians);

void CharCategoryMain(void)
{
    CharCategoryWork *work = CharCategory;
    int i;

    switch (work->state) {
    case 0: {
        CharCategoryIconIdTable ids = D_004C6048;

        work->iconResetWork = 0x00FFFFFE;
        for (i = 0; i < 5; i++) {
            eSpriteSet(&work->icons[i], ids.id[i]);
            work->icons[i].work = work->iconResetWork;
        }
        work->pulseCounter = 8;
        work->flashStep = 1;
        work->state = 2;
        work->phase = 0;
        work->edgeDelta[1] = 0;
        work->edgeDelta[0] = 0;
        work->tickCounter = 0;
        work->flashLevel = 0;
        work->pendingReset = 0;
    }
    /* fallthrough */
    case 2: {
        int arcSpan;
        int rowBase;

        if (work->pendingReset == 0) {
            arcSpan = 58;
            rowBase = 45;
        } else {
            arcSpan = 288;
            rowBase = 32;
        }

        switch (MenuWork.state) {
        case 0x52:
            work->pendingReset = 0;
            /* fallthrough */
        case 0x53: {
            int equipKind = MenuWork.equipKind;

            if (equipKind == 1) {
                if (work->pulseCounter != 0) {
                    work->pulseCounter--;
                }
                work->flashStep = equipKind;
            } else {
                if (work->pulseCounter != 8) {
                    work->pulseCounter++;
                }
                work->flashStep = 0;
            }
            if (MenuWork.wait == 0) {
                if (PadData.repeat == PAD_LEFT) {
                    work->edgeDelta[0] = -6;
                }
                if (PadData.repeat == PAD_RIGHT) {
                    work->edgeDelta[1] = 6;
                }
            }
            break;
        }
        default:
            if (work->pulseCounter != 8) {
                work->pulseCounter++;
            }
            work->flashStep = 0;
            break;
        }

        if (work->edgeDelta[0] != 0) {
            work->edgeDelta[0]++;
        }
        if (work->edgeDelta[1] != 0) {
            work->edgeDelta[1]--;
        }

        if (work->tickCounter & 1) {
            work->flashLevel += work->flashStep;
            if (work->flashLevel >= 11) {
                work->flashStep = -work->flashStep;
            }
        }
        work->tickCounter++;

        {
            short rowY = rowBase + 83;

            for (i = 0; i < 2; i++) {
                float cosVal;
                int negTwicePulse;
                unsigned char edge; /* the pad nudge byte, applied signed */

                work->icons[i].x = arcSpan + i * 110;
                cosVal = xglCos((float)i * 3.1415927f);
                negTwicePulse = -(work->pulseCounter * 2);
                work->icons[i].x = (short)((float)work->icons[i].x + cosVal * (float)negTwicePulse);
                edge = work->edgeDelta[i];
                work->icons[i].y = rowY;
                work->icons[i].x += (signed char)edge;
                work->icons[i].alpha = (unsigned char)(128.0f - (float)work->pulseCounter * 0.125f * 128.0f);
            }

            for (i = 2; i < 5; i++) {
                float cosVal;
                int negTwicePulse;
                short x;

                work->icons[i].x = arcSpan + 36 + (i - 2) * 24;
                cosVal = xglCos((float)(i - 2) * 1.0471976f);
                negTwicePulse = -(work->pulseCounter * 2);
                work->icons[i].y = rowY;
                x = (short)((float)work->icons[i].x + cosVal * (float)negTwicePulse);
                work->icons[i].x = x;
                work->icons[i].alpha = (unsigned char)(128.0f - (float)work->pulseCounter * 0.125f * 128.0f);
                if (i - 2 == MenuWork.accessoryList) {
                    work->icons[i].color[2] = -0x80;
                    work->icons[i].color[1] = -0x80;
                    work->icons[i].color[0] = -0x80;
                } else {
                    work->icons[i].color[2] = 0x40;
                    work->icons[i].color[1] = 0x40;
                    work->icons[i].color[0] = 0x40;
                }
            }
        }

        for (i = 0; i < 5; i++) {
            eSpriteMain(&work->icons[i]);
        }
        break;
    }
    default:
        return;
    }
}
/*
 * The character screen's command windows. CharMenuMain builds one WindowDX
 * (main/tu166, 0x194 bytes) per command list and hands MenuSelectWindow the
 * matching list block as the window's close callback argument. Only the
 * members this screen writes are modelled: the slide-in x and the y of the
 * top-left corner, the frame colour, the frame size, the title string, the
 * state WindowDXMain switches on (1 while the frame opens, 3 once it is up)
 * and the untyped handler/argument pair WindowDXMain calls with the window:
 * both slots hold plain addresses, so a store to one orders against a store
 * to the other.
 */
typedef struct CharMenuWindowDX CharMenuWindowDX;
typedef struct CharMenuList CharMenuList;

struct CharMenuWindowDX {
    short x;                            /* +0x00 */
    short y;                            /* +0x02 */
    int color;                          /* +0x04 */
    short width;                        /* +0x08 */
    short height;                       /* +0x0A */
    const char *title;                  /* +0x0C */
    unsigned char state;                /* +0x10 */
    unsigned char unmodeled_11[0x14 - 0x11];
    void *select;                       /* +0x14 */
    void *selectArg;                    /* +0x18 */
    unsigned char unmodeled_1c[0x194 - 0x1C];
};

/*
 * One selection list MenuSelectWindow (main/menu_top_command_check.c) draws
 * inside such a window: it reads hasPrompt (lhu 2), the highlighted row
 * (lw 4, multiplied by the 24-pixel row height), the newline-separated item
 * text (lw 8) and the prompt line above the items (lw 0xC), and keeps its
 * cursor and twelve message objects in the rest of the 0x5F8 bytes.
 */
struct CharMenuList {
    unsigned char unmodeled_00[0x02];
    unsigned short hasPrompt;           /* +0x02 */
    int cursor;                         /* +0x04 */
    const char *items;                  /* +0x08 */
    const char *prompt;                 /* +0x0C */
    unsigned char unmodeled_10[0x5F8 - 0x10];
};

/*
 * The character screen's work block. state 0 builds the windows and turns
 * into state 2, the frame CharMenuMain runs every frame afterwards; subState
 * is cleared when the windows are built and set to 16 while MenuWork.state
 * sits in the 0xB2..0xB5 and 0xBF range.
 */
typedef struct CharMenuWork {
    unsigned char state;                /* +0x000 */
    unsigned char unmodeled_01[0x02 - 0x01];
    unsigned char windowCount;          /* +0x002 */
    unsigned char subState;             /* +0x003 */
    int color;                          /* +0x004 */
    CharMenuWindowDX window[3];         /* +0x008 */
    unsigned char unmodeled_4c4[0x658 - 0x4C4];
    CharMenuList list[3];               /* +0x658 */
} CharMenuWork;

extern CharMenuWork *CharMenu;

/* The window titles, the small .sdata string constants "Menu" (0x004DADC8)
 * and "Select" (0x004DADD0) the scaffold still owns. */
extern const char D_004DADC8[];
extern const char D_004DADD0[];

/*
 * The command lists, one entry per party mode and window: "Equip\nUnequip\nUse
 * T.Pts" behind a colour escape, "Formation\nReplace\nCancel" and
 * "Draw\nSet\nCancel". The three bytes after the escape of the first entry are
 * the grey level the Equip row is drawn in.
 */
extern char *msg00_2_0036D8E8[3];

/* The confirmation window's list ("Yes\nNo") and the question above it
 * (" Is this okay?"). */
extern const char *msg01_3[];

void MenuSelectWindow(CharMenuWindowDX *window, CharMenuList *list);

/* Whether the shown character may use the Equip command. */
int MenuMainCharCheck(int chrNo);

/* The frame colour of every command window: opaque white with the lowest
 * intensity bits cleared. */
#define CHAR_MENU_COLOR 0x00FFFFF0

/* The grey level of a row the player may pick and of one that is locked. */
#define CHAR_MENU_ROW_ON  0x80
#define CHAR_MENU_ROW_OFF 0x40

static void CharMenuMain(void)
{
    CharMenuWork *work = CharMenu;
    short target[3];
    int i;

    switch (work->state) {
    case 0:
        work->color = CHAR_MENU_COLOR;
        if (MenuWork.partyMode == 0) {
            work->windowCount = 3;
        } else {
            work->windowCount = 1;
        }
        for (i = 0; i < work->windowCount; i++) {
            WindowDXSet(&work->window[i]);
            work->window[i].x = 528;
            work->window[i].width = 126;
            work->window[i].height = 78;
            work->window[i].color = work->color;
            work->window[i].title = D_004DADC8;
            work->window[i].select = (void *)MenuSelectWindow;
            work->window[i].selectArg = &work->list[i];
            work->list[i].cursor = 0;
            if (i != 2) {
                work->list[i].hasPrompt = 0;
                work->list[i].items = msg00_2_0036D8E8[MenuWork.partyMode + i * 2];
            } else {
                work->window[i].x = 528;
                work->window[i].width = 166;
                work->window[i].height = 102;
                work->window[i].title = D_004DADD0;
                work->list[i].hasPrompt = 1;
                work->list[i].items = msg01_3[0];
                work->list[i].prompt = msg01_3[1];
            }
            work->window[i].state = 1;
            WindowDXMain(&work->window[i]);
            work->window[i].state = 3;
        }
        if (MenuWork.partyMode == 0) {
            work->window[0].y = 226;
            work->window[1].y = 224;
            work->window[2].y = 160;
        } else {
            work->window[0].y = 208;
        }
        work->subState = 0;
        work->state = 2;
        /* fallthrough */
    case 2:
        target[0] = target[1] = target[2] = 528;
        switch (MenuWork.state) {
        case 32:
        case 33:
            {
                char *row = msg00_2_0036D8E8[0] + 1;

                target[0] = 264;
                work->list[0].cursor = MenuWork.menuCursor;
                if (MenuMainCharCheck(MenuWork.view.chrNo) != 0) {
                    row[0] = row[1] = row[2] = CHAR_MENU_ROW_ON;
                } else {
                    row[0] = row[1] = row[2] = CHAR_MENU_ROW_OFF;
                }
            }
            break;
        case 132:
        case 133:
            target[2] = 330;
            work->list[2].cursor = MenuWork.confirmCursor;
            break;
        case 162:
        case 163:
            target[0] = 320;
            work->list[0].cursor = MenuWork.commandCursor;
            break;
        case 178:
        case 179:
        case 180:
        case 181:
        case 191:
            work->subState = 16;
            break;
        }
        for (i = 0; i < work->windowCount; i++) {
            MoveSlide(&work->window[i].x, &target[i], 3.0f);
            WindowDXMain(&work->window[i]);
        }
        break;
    }
}
/*
 * The three display elements of the point bar, as this screen writes them.
 * src/main/e_number_main.c owns the records and models the members its
 * setters write (color, mode, flag, state, subState, value); each definition
 * below keeps those members and their names, names the members this screen
 * writes in the spans that file left as padding, and carries the element's
 * whole stride, which the work block below needs: 0x70 for a ribbon (ex + 0x18 + i * 0x70), 0x20 for
 * a label and 0x90 for a number. The word that follows x and y in all three
 * is the draw order the element's own Main copies verbatim into its render
 * record (eNumberMain: lw 0x4, sw 0x6C at 0x0027648c); this screen gives
 * every ribbon the bar's base order and puts the label and the value two in
 * front of it.
 */
typedef struct CharExRibbon {
    short x;                        /* +0x00 */
    short y;                        /* +0x02 */
    unsigned int drawOrder;         /* +0x04 */
    short width;                    /* +0x08 */
    short height;                   /* +0x0A */
    signed char mode;               /* +0x0C */
    u8 unmodeled_00d[0x70 - 0x0D];
} CharExRibbon;

typedef struct CharExLabel {
    signed char state;              /* +0x00 */
    signed char subState;           /* +0x01 */
    u8 unmodeled_002[2];            /* +0x02..0x03 */
    short x;                        /* +0x04 */
    short y;                        /* +0x06 */
    unsigned int drawOrder;         /* +0x08 */
    signed char color[4];           /* +0x0c..0x0f */
    u8 unmodeled_010[0xc];          /* +0x10..0x1b */
    int value;                      /* +0x1c */
} CharExLabel;

/*
 * digits (+0x0F) is the number of digit cells eNumberMain lays out, twelve
 * pixels apart (lbu then *3*4 at 0x0027647c), and format (+0x0E) is the
 * layout it selects with, rejecting anything above 14 (sltiu 0xF at
 * 0x002763a0).
 */
typedef struct CharExNumber {
    short x;                        /* +0x00 */
    short y;                        /* +0x02 */
    unsigned int drawOrder;         /* +0x04 */
    signed char color[4];           /* +0x08..0x0b */
    signed char mode;               /* +0x0c */
    u8 unmodeled_00d;               /* +0x0d */
    unsigned char format;           /* +0x0E */
    unsigned char digits;           /* +0x0F */
    signed char flag[4];            /* +0x10..0x13 */
    int value;                      /* +0x14 */
    u8 unmodeled_018[0x90 - 0x18];
} CharExNumber;

/*
 * The character screen's point bar: three rows, each one ribbon with its
 * label and its value.
 */
typedef struct CharExWork {
    unsigned char unmodeled_00[0x10];
    unsigned char state;            /* +0x10 */
    unsigned char unmodeled_11[0x14 - 0x11];
    unsigned int drawOrder;         /* +0x14 */
    CharExRibbon ribbon[3];         /* +0x18 */
    CharExLabel label[3];              /* +0x168 */
    CharExNumber number[3];              /* +0x1C8 */
} CharExWork;

extern CharExWork *CharEx;

#define CHAR_EX_START 0
#define CHAR_EX_SLIDE 2

/* The bar's draw order, the same form as the packed print-colour word
 * xglFontPrintf takes. */
#define CHAR_EX_DRAW_ORDER 0x00FFFFF0

/* The three row labels, scaffold .data at 0x0036D900 (three tag words, as
 * eTagFontSet takes them). */
extern int msg00_4_0036D900[3];

/* The ribbon slides in from the right edge to this column while the screen is
 * open, and back out again when it is not. */
#define CHAR_EX_X_OUT 512
#define CHAR_EX_X_IN  264

void CharExMain(void)
{
    CharExWork *ex = CharEx;
    short slide[3];
    int i;

    switch (ex->state) {
    case CHAR_EX_START:
        ex->drawOrder = CHAR_EX_DRAW_ORDER;
        for (i = 0; i < 3; i++) {
            eRibbonSet(&ex->ribbon[i], 3);
            ex->ribbon[i].x = CHAR_EX_X_OUT;
            ex->ribbon[i].y = 128 + i * 28;
            ex->ribbon[i].width = 248;
            ex->ribbon[i].height = 23;
            ex->ribbon[i].drawOrder = ex->drawOrder;
            eTagFontSet(&ex->label[i], msg00_4_0036D900[i]);
            ex->label[i].drawOrder = ex->drawOrder + 2;
            eNumberSet(&ex->number[i], 0);
            ex->number[i].drawOrder = ex->drawOrder + 2;
        }
        ex->state = CHAR_EX_SLIDE;
        /* fallthrough */
    case CHAR_EX_SLIDE:
        for (i = 0; i < 3; i++) {
            slide[i] = CHAR_EX_X_OUT;
        }
        switch (MenuWork.state) {
        case 0x21:
        case 0x80:
        case 0x81:
        case 0x86:
        case 0x87:
            slide[0] = slide[1] = slide[2] = CHAR_EX_X_IN;
            break;
        }
        for (i = 0; i < 3; i++) {
            CharPointData *unit;

            MoveSlide(&ex->ribbon[i].x, &slide[i], 3.0f);
            eRibbonMain(&ex->ribbon[i]);
            ex->label[i].x = ex->ribbon[i].x + 48;
            ex->label[i].y = ex->ribbon[i].y + 4;
            eTagFontMain(&ex->label[i]);
            unit = func_A19210(MenuWork.view.chrNo);
            ex->number[i].x = ex->label[i].x + 8;
            ex->number[i].y = ex->ribbon[i].y + 4;
            ex->number[i].digits = 4;
            ex->number[i].format = 0;
            switch (i) {
            case 0:
                ex->number[0].value = unit->points;
                break;
            case 1:
                ex->number[1].value = unit->etherPoints;
                break;
            case 2:
                ex->number[2].value = unit->skillPoints;
                break;
            }
            eNumberMain(&ex->number[i]);
        }
        break;
    }
}
/*
 * Partial view of the object eSpriteSet (src/main/e_number_main.c, a
 * different TU) initialises: only the fields CharL1R1Main itself reads or
 * writes after the call.
 */
typedef struct L1R1Sprite {
    unsigned char unmodeled_00[0x04];
    short x;                 /* +0x04 */
    short y;                 /* +0x06 */
    int work;                /* +0x08 */
    unsigned char unmodeled_0c[0x28 - 0x0c];
} L1R1Sprite;

/*
 * CharL1R1's 0x58-byte work block: MenuCharactor carves it from the menu
 * heap with state left at 0. CharL1R1Main runs the one-time setup while
 * state is 0, leaves it at 2 and keeps re-running the per-frame update on
 * every later call while it stays 2; any other value makes it return
 * immediately.
 */
typedef struct CharL1R1Work {
    unsigned char state;         /* +0x00 */
    unsigned char unmodeled_01;
    signed char slideOffset[2];  /* +0x02: added into sprite[i].x while non-zero */
    int spriteWork;              /* +0x04: shared initial value for both sprites' work */
    L1R1Sprite sprite[2];        /* +0x08: L1, then R1 */
} CharL1R1Work;

extern CharL1R1Work *CharL1R1;

/*
 * Scaffold .sdata tables read by CharL1R1Main. 0x004DADF0 holds the L1 and
 * R1 sprite ids (0x0112, 0x0110), 0x004DADF8 the per-side step (+1, -1) that
 * walks a pressed side's slide offset back to zero. Both are copied as a
 * pair; their extent past that pair belongs to the scaffold.
 */
typedef struct L1R1SpriteIdPair {
    short id[2];
} L1R1SpriteIdPair;
extern const L1R1SpriteIdPair D_004DADF0[];

typedef struct L1R1SlideStep {
    signed char side[2];
} L1R1SlideStep;
extern const L1R1SlideStep D_004DADF8[];

void CharL1R1Main(void)
{
    CharL1R1Work *self = CharL1R1;
    signed char *slideOffset;
    int i;

    switch (self->state) {
    case 0: {
        L1R1SpriteIdPair spriteIds = D_004DADF0[0];

        self->spriteWork = 0x00FFFFFF;
        for (i = 0; i < 2; i++) {
            eSpriteSet(&self->sprite[i], spriteIds.id[i]);
            self->sprite[i].work = self->spriteWork;
            self->sprite[i].y = 214;
        }
        self->sprite[0].x = -45;
        self->sprite[1].x = 528;
        slideOffset = self->slideOffset;
        self->slideOffset[1] = 0;
        self->slideOffset[0] = 0;
        i = 2;
        self->state = i;
        break;
    }
    case 2:
        slideOffset = self->slideOffset;
        break;
    default:
        return;
    }

    {
        short target[2];
        int menuState = MenuWork.state;

        /* Both sprites sit just off screen unless the character menu is on
         * one of the two pages that show the L1/R1 page hints. */
        target[0] = -45;
        target[1] = 528;
        if (menuState < 34) {
            if (menuState >= 32) {
                target[0] = 8;
                target[1] = 475;
                if (PadData.pressed & PAD_L1) {
                    self->slideOffset[0] = -6;
                } else if (PadData.pressed & PAD_R1) {
                    self->slideOffset[1] = 6;
                }
            }
        }

        {
            L1R1SlideStep step = D_004DADF8[0];

            for (i = 0; i < 2; i++) {
                if (self->slideOffset[i] != 0) {
                    self->slideOffset[i] = self->slideOffset[i] + step.side[i];
                }
            }
        }

        for (i = 0; i < 2; i++) {
            MoveSlide(&self->sprite[i].x, &target[i], 3.0f);
            self->sprite[i].x = self->sprite[i].x + slideOffset[i];
            eSpriteMain(&self->sprite[i]);
        }
    }
}
/*
 * One entry of the CharStatus block: a twelve-byte entry header followed by
 * the record the drawing function reads. The menu writes the build-time
 * fields of a status entry through the entry (header+0x0C, +0x0D, +0x10,
 * +0x12 = record chrNo, fullStatus, offsetX, offsetY) and hands the record
 * address itself to MenuStatusDisp, and the swap cursor's state is written the
 * same way (entry+0x0C at CharStatus+0x4EC4). The block's own control fields
 * are the header of its first entry.
 */
typedef struct CharStatusHeader {
    unsigned char state;                /* +0x00 */
    unsigned char cursorOn;             /* +0x01 */
    unsigned char unmodeled_02[0x04 - 0x02];
    unsigned char moveVert;             /* +0x04: slide y before x */
    unsigned char flags;                /* +0x05: bit 0 = the slide finished */
    unsigned char subState;             /* +0x06 */
    signed char lastCursor;             /* +0x07 */
    int color;                          /* +0x08 */
} CharStatusHeader;

/*
 * The 0x834-byte record MenuStatusDisp (main/menu_status.c, VA 0x00279098)
 * draws one character's status panel from. Only the fields this menu writes
 * are modelled.
 */
typedef struct MenuStatusWork {
    unsigned char chrNo;                /* +0x00 */
    unsigned char fullStatus;           /* +0x01: 1 outside the formation menu */
    unsigned char unmodeled_02[0x04 - 0x02];
    short offsetX;                      /* +0x04 */
    short offsetY;                      /* +0x06 */
    unsigned char unmodeled_08[0x0C - 0x08];
    short maxHp;                        /* +0x0C: with the pending point spend */
    signed char hpMark;                 /* +0x0E */
    short maxEp;                        /* +0x10 */
    signed char epMark;                 /* +0x12 */
    unsigned char unmodeled_13[0x834 - 0x13];
} MenuStatusWork;

void MenuStatusDisp(MenuStatusWork *work);

/*
 * The window record WindowDXSet initialises and WindowDXMain draws, as this
 * menu uses it (the shared control block is defined by the translation unit
 * that owns WindowDXSet; this is the partial view of the members the status
 * windows write). The caption is MenuTagTextGet's result, which WindowDXMain
 * passes on as a tag id, and the drawing callback receives the status record.
 */
typedef struct MenuStatusWindow {
    short x;                            /* +0x00 */
    short y;                            /* +0x02 */
    int color;                          /* +0x04 */
    short width;                        /* +0x08 */
    short height;                       /* +0x0A */
    char *title;                        /* +0x0C */
    signed char state;                  /* +0x10: 1 while opening, 3 when open */
    unsigned char visible;              /* +0x11 */
    unsigned char unmodeled_12[0x14 - 0x12];
    void (*disp)(MenuStatusWork *work);  /* +0x14 */
    MenuStatusWork *dispWork;           /* +0x18 */
    unsigned char unmodeled_1c[0x188 - 0x1C];
} MenuStatusWindow;

/* The window's content colours: the party slot under the cursor is brighter. */
#define CHAR_STATUS_COLOR_SELECTED 0x00F07F00
#define CHAR_STATUS_COLOR_NORMAL   0x00F07E00

/*
 * The cursor record eCursolSet initialises and eCursolMain draws, as this
 * menu uses it (the shared record is defined by the translation unit that
 * owns eCursolSet, which clears the record and takes its width as the second
 * argument).
 */
typedef struct MenuStatusCursor {
    signed char state;                  /* +0x00 */
    unsigned char unmodeled_01[0x04 - 0x01];
    short x;                            /* +0x04 */
    short y;                            /* +0x06 */
    int work;                           /* +0x08 */
    unsigned char unmodeled_0c[0x10 - 0x0C];
} MenuStatusCursor;

/* The state the swap menu's reserve cursor runs with, and the work word both
 * status cursors are given after eCursolSet has cleared the record. */
#define CHAR_STATUS_CURSOR_STATE 0x20
#define CHAR_STATUS_CURSOR_WORK  0x00FFFFFF

typedef struct CharStatusWindowEntry {
    CharStatusHeader header;            /* +0x0000 */
    MenuStatusWindow window;           /* +0x000C */
} CharStatusWindowEntry;

typedef struct CharStatusDispEntry {
    CharStatusHeader header;            /* +0x0000 */
    MenuStatusWork work;               /* +0x000C */
} CharStatusDispEntry;

typedef struct CharStatusCursorEntry {
    CharStatusHeader header;            /* +0x0000 */
    MenuStatusCursor cursor;           /* +0x000C */
    unsigned char unmodeled_1c[0x24 - 0x1C];
} CharStatusCursorEntry;

/*
 * The character-status work block MenuCharactor carves from the menu heap
 * (0x4EF4 bytes, entry-header state 0 means it still has to be built): eight
 * window entries, the three party slots followed by the five reserve slots,
 * then their eight status records, then the two swap cursors (cursor[0] marks
 * the party slot and cursor[1] the reserve slot), and the header that closes
 * the list.
 */
typedef struct CharStatusWork {
    CharStatusWindowEntry window[8];   /* +0x0000 */
    CharStatusDispEntry disp[8];       /* +0x0CA0 */
    CharStatusCursorEntry cursor[2];   /* +0x4EA0 */
    CharStatusHeader endHeader;        /* +0x4EE8 */
} CharStatusWork;

#define CHAR_STATUS_BUILD 0
#define CHAR_STATUS_RUN   2

/* The block MenuCharactor allocated for the character status menu. */
extern CharStatusWork *CharStatus;

/* main/menu_tag.c: the caption string of one window kind. */
char *MenuTagTextGet(int kind);

/* main/party.c: non-zero while the character fights in the active party. */
int PartyAttackerCheck(int chrNo);

/* The on-screen and off-screen window geometry of the party slots. */
#define STATUS_WINDOW_WIDTH   240
#define STATUS_WINDOW_HEIGHT  82
#define STATUS_WINDOW_OPEN_H  99
#define STATUS_ROW_HEIGHT     104
#define STATUS_ROW_TOP        64
#define STATUS_COLUMN_WIDTH   240
#define STATUS_COLUMN_LEFT    16
#define STATUS_OFF_LEFT       (-272)
#define STATUS_OFF_RIGHT      816

void CharStatusMain(void)
{
    CharStatusWork *status = CharStatus;
    short targetX[8];
    short targetY[8];
    short targetHeight[8];
    short targetOffset[8];
    CharStatusCursorEntry *swapEntry;
    int para[4];
    int i;

    switch (status->window[0].header.state) {
    case CHAR_STATUS_BUILD:
        status->window[0].header.color = 0x00FFF000;
        if (MenuWork.partyMode == 0) {
            for (i = 0; i < MenuWork.partyCount; i++) {
                short chrNo = MenuCharParty[i];

                WindowDXSet(&status->window[i].window);
                status->window[i].window.x = (i & 1) * STATUS_OFF_RIGHT + STATUS_OFF_LEFT;
                status->window[i].window.y = (i / 2) * STATUS_ROW_HEIGHT + STATUS_ROW_TOP;
                status->window[i].window.color = status->window[0].header.color;
                status->window[i].window.width = STATUS_WINDOW_WIDTH;
                status->window[i].window.height = STATUS_WINDOW_HEIGHT;
                status->window[i].window.visible = 1;
                if (PartyAttackerCheck(chrNo) != 0) {
                    status->window[i].window.title = MenuTagTextGet(1);
                } else {
                    status->window[i].window.title = MenuTagTextGet(2);
                }
                status->window[i].window.disp = MenuStatusDisp;
                status->window[i].window.dispWork = &status->disp[i].work;
                status->disp[i].work.offsetX = 0;
                status->disp[i].work.fullStatus = 1;
                status->disp[i].work.chrNo = chrNo;
                status->disp[i].work.offsetY = 0;
                status->window[i].window.state = 1;
                WindowDXMain(&status->window[i].window);
                status->window[i].window.state = 3;
            }
        } else {
            for (i = 0; i < 3; i++) {
                short chrNo = MenuCharParty[i];

                WindowDXSet(&status->window[i].window);
                status->window[i].window.x = STATUS_OFF_LEFT;
                status->window[i].window.y = i * STATUS_ROW_HEIGHT + STATUS_ROW_TOP;
                status->window[i].window.color = status->window[0].header.color;
                status->window[i].window.width = STATUS_WINDOW_WIDTH;
                status->window[i].window.height = STATUS_WINDOW_HEIGHT;
                status->window[i].window.visible = 1;
                status->window[i].window.title = MenuTagTextGet(1);
                status->window[i].window.disp = MenuStatusDisp;
                status->window[i].window.dispWork = &status->disp[i].work;
                status->disp[i].work.chrNo = chrNo;
                status->disp[i].work.fullStatus = 0;
                status->disp[i].work.offsetX = 0;
                status->disp[i].work.offsetY = 0;
                status->window[i].window.state = 1;
                WindowDXMain(&status->window[i].window);
                status->window[i].window.state = 3;
            }
            for (i = 0; i < MenuWork.reserveCount; i++) {
                short chrNo = MenuCharPartyReser[i];

                WindowDXSet(&status->window[3 + i].window);
                status->window[3 + i].window.x = 544;
                status->window[3 + i].window.width = STATUS_WINDOW_WIDTH;
                status->window[3 + i].window.height = STATUS_WINDOW_HEIGHT;
                status->window[3 + i].window.color = status->window[0].header.color;
                status->window[3 + i].window.y = i * STATUS_ROW_HEIGHT + STATUS_ROW_TOP;
                status->window[3 + i].window.visible = 1;
                status->window[3 + i].window.title = MenuTagTextGet(2);
                status->window[3 + i].window.disp = MenuStatusDisp;
                status->window[3 + i].window.dispWork = &status->disp[3 + i].work;
                status->disp[3 + i].work.chrNo = chrNo;
                status->disp[3 + i].work.fullStatus = 0;
                status->disp[3 + i].work.offsetX = 0;
                status->disp[3 + i].work.offsetY = 0;
                status->window[3 + i].window.state = 1;
                WindowDXMain(&status->window[3 + i].window);
                status->window[3 + i].window.state = 3;
            }
        }
        for (i = 0; i < 2; i++) {
            eCursolSet(&status->cursor[i].cursor, 0);
            if (i == 1) {
                swapEntry = &status->cursor[1];
                swapEntry->cursor.state = CHAR_STATUS_CURSOR_STATE;
            } else {
                status->cursor[i].cursor.state = 0;
            }
            status->cursor[i].cursor.work = CHAR_STATUS_CURSOR_WORK;
        }
        status->window[0].header.cursorOn = 0;
        status->window[0].header.flags = 0;
        status->window[0].header.subState = 0;
        status->window[0].header.moveVert = 0;
        status->window[0].header.lastCursor = MenuWork.partyCursor;
        status->window[0].header.state = CHAR_STATUS_RUN;
        /* fall through */
    case CHAR_STATUS_RUN:
        if (MenuWork.partyMode == 0) {
            for (i = 0; i < MenuWork.partyCount; i++) {
                targetX[i] = (i & 1) * STATUS_OFF_RIGHT + STATUS_OFF_LEFT;
                targetY[i] = (i / 2) * STATUS_ROW_HEIGHT + STATUS_ROW_TOP;
                targetHeight[i] = STATUS_WINDOW_HEIGHT;
                targetOffset[i] = 0;
            }
        } else {
            for (i = 0; i < 3; i++) {
                targetX[i] = STATUS_OFF_LEFT;
                targetY[i] = i * STATUS_ROW_HEIGHT + STATUS_ROW_TOP;
                targetHeight[i] = STATUS_WINDOW_HEIGHT;
                targetOffset[i] = 0;
            }
            for (i = 0; i < MenuWork.reserveCount; i++) {
                targetX[3 + i] = 544;
                targetY[3 + i] = i * STATUS_ROW_HEIGHT + STATUS_ROW_TOP;
                targetHeight[3 + i] = STATUS_WINDOW_HEIGHT;
                targetOffset[3 + i] = 0;
            }
        }
        status->disp[MenuWork.partyCursor].work.hpMark =
            status->disp[MenuWork.partyCursor].work.epMark = 0;
        switch (MenuWork.state) {
        case 0x10:
            status->window[0].header.cursorOn = 1;
            status->cursor[0].cursor.state = CHAR_STATUS_CURSOR_STATE;
            status->window[0].header.moveVert = 1;
            if ((status->window[0].header.flags & 1) != 0) {
                if (i != MenuWork.partyCursor) {
                    status->window[i].window.x = (i & 1) * STATUS_OFF_RIGHT + STATUS_OFF_LEFT;
                    status->window[i].window.y = (i / 2) * STATUS_ROW_HEIGHT + STATUS_ROW_TOP;
                }
                status->window[0].header.flags &= ~1;
            }
            /* fall through */
        case 0x11:
            for (i = 0; i < MenuWork.partyCount; i++) {
                targetX[i] = (i & 1) * STATUS_COLUMN_WIDTH + STATUS_COLUMN_LEFT;
            }
            break;
        case 0x20:
            status->window[0].header.lastCursor = MenuWork.partyCursor;
            if (MenuWork.state != 0x20) {
                for (i = 0; i < MenuWork.partyCount; i++) {
                    targetHeight[i] = STATUS_WINDOW_HEIGHT;
                }
            }
            /* fall through */
        case 0x80:
        case 0x81:
        case 0x82:
        case 0x83:
        case 0x84:
        case 0x85:
        case 0x86:
        case 0x87: {
            CharParaData *total;
            int menuState;
            int mark;

            total = func_00A11108(MenuWork.view.chrNo, para, para);
            mark = 1;
            status->disp[MenuWork.partyCursor].work.maxHp = total->maxHp + UpPara[0];
            status->disp[MenuWork.partyCursor].work.maxEp = total->maxEp + UpPara[1];
            menuState = MenuWork.state;
            if (menuState < 0x84) {
                if (menuState >= 0x82) {
                    mark = 2;
                }
            }
            switch (MenuWork.paraIndex) {
            case 0:
                status->disp[MenuWork.partyCursor].work.hpMark = mark;
                break;
            case 1:
                status->disp[MenuWork.partyCursor].work.epMark = mark;
                break;
            }
        }
            /* fall through */
        case 0x21:
            if (status->window[0].header.lastCursor != MenuWork.partyCursor) {
                status->window[MenuWork.partyCursor].window.x = 264;
                status->window[MenuWork.partyCursor].window.y = 24;
                status->window[MenuWork.partyCursor].window.height = STATUS_WINDOW_OPEN_H;
                status->window[0].header.lastCursor = MenuWork.partyCursor;
            }
            if ((status->window[0].header.flags & 1) != 0) {
                for (i = 0; i < MenuWork.partyCount; i++) {
                    if (i == MenuWork.partyCursor) {
                        targetX[i] = 264;
                        targetY[i] = 24;
                        targetHeight[i] = STATUS_WINDOW_OPEN_H;
                    } else {
                        targetX[i] = (i & 1) * STATUS_OFF_RIGHT + STATUS_OFF_LEFT;
                        status->window[i].window.x = targetX[i];
                    }
                }
            } else {
                for (i = 0; i < MenuWork.partyCount; i++) {
                    if (i == MenuWork.partyCursor) {
                        targetX[i] = 264;
                        targetY[i] = 24;
                        targetHeight[i] = STATUS_WINDOW_OPEN_H;
                    }
                }
                if (targetY[MenuWork.partyCursor] == status->window[MenuWork.partyCursor].window.y) {
                    status->window[0].header.flags |= 1;
                }
            }
            status->window[0].header.cursorOn = 0;
            status->window[0].header.moveVert = 0;
            break;
        case 0x40:
        case 0x41:
            for (i = 0; i < MenuWork.partyCount; i++) {
                if (i == MenuWork.partyCursor) {
                    targetX[i] = 544;
                    targetY[i] = 24;
                    targetHeight[i] = STATUS_WINDOW_OPEN_H;
                    targetOffset[i] = -80;
                } else {
                    targetX[i] = (i & 1) * STATUS_OFF_RIGHT + STATUS_OFF_LEFT;
                    status->window[i].window.x = targetX[i];
                }
            }
            break;
        case 0x52:
        case 0x53:
        case 0x54:
        case 0x55:
            status->window[0].header.moveVert = 0;
            status->window[0].header.flags &= ~1;
            for (i = 0; i < MenuWork.partyCount; i++) {
                targetX[i] = 544;
                if (i == MenuWork.partyCursor) {
                    targetY[i] = 192;
                    targetHeight[i] = STATUS_WINDOW_OPEN_H;
                } else {
                    status->window[i].window.x = 544;
                }
            }
            targetOffset[MenuWork.partyCursor] = -80;
            break;
        case 0xA0:
        case 0xA1:
        case 0xA2:
        case 0xA3:
        case 0xB2:
        case 0xB3:
        case 0xB4:
        case 0xB5:
        case 0xB6:
        case 0xBF:
            status->cursor[0].cursor.state = CHAR_STATUS_CURSOR_STATE;
            for (i = 0; i < 3; i++) {
                targetX[i] = STATUS_COLUMN_LEFT;
                targetY[i] = i * STATUS_ROW_HEIGHT + STATUS_ROW_TOP;
            }
            break;
        case 0xC2:
        case 0xC5:
            if (MenuWork.state == 0xC2) {
                for (i = 0; i < MenuWork.reserveCount; i++) {
                    status->disp[3 + i].work.chrNo = MenuCharPartyReser[i];
                    status->window[3 + i].window.state = 1;
                    WindowDXMain(&status->window[3 + i].window);
                    status->window[3 + i].window.state = 3;
                }
            } else {
                status->disp[MenuWork.partyCursor].work.chrNo = MenuWork.swapInId;
                status->window[MenuWork.partyCursor].window.state = 1;
                WindowDXMain(&status->window[MenuWork.partyCursor].window);
                status->window[MenuWork.partyCursor].window.state = 3;
            }
            /* fall through */
        case 0xC3:
            for (i = 0; i < 3; i++) {
                targetX[i] = STATUS_COLUMN_LEFT;
                targetY[i] = i * STATUS_ROW_HEIGHT + STATUS_ROW_TOP;
            }
            if (MenuWork.state == 0xC3) {
                for (i = 0; i < MenuWork.reserveCount; i++) {
                    targetX[3 + i] = 258;
                    targetY[3 + i] = i * STATUS_ROW_HEIGHT + STATUS_ROW_TOP;
                }
            }
            break;
        case 0xC4:
            for (i = 0; i < 3; i++) {
                if (i != MenuWork.partyCursor) {
                    targetX[i] = STATUS_COLUMN_LEFT;
                    targetY[i] = i * STATUS_ROW_HEIGHT + STATUS_ROW_TOP;
                }
            }
            break;
        }
        if (MenuWork.partyMode == 0) {
            for (i = 0; i < MenuWork.partyCount; i++) {
                if (status->window[0].header.moveVert == 0) {
                    MoveSlide(&status->window[i].window.height, &targetHeight[i], 3.0f);
                    MoveSlide(&status->window[i].window.x, &targetX[i], 3.0f);
                    if (status->window[i].window.x == targetX[i]) {
                        MoveSlide(&status->window[i].window.y, &targetY[i], 3.0f);
                    }
                } else {
                    MoveSlide(&status->window[i].window.y, &targetY[i], 3.0f);
                    if (status->window[i].window.y == targetY[i]) {
                        MoveSlide(&status->window[i].window.height, &targetHeight[i], 3.0f);
                        MoveSlide(&status->window[i].window.x, &targetX[i], 3.0f);
                    }
                }
                if (i == MenuWork.partyCursor) {
                    status->window[i].window.color = CHAR_STATUS_COLOR_SELECTED;
                } else {
                    status->window[i].window.color = CHAR_STATUS_COLOR_NORMAL;
                }
                WindowDXMain(&status->window[i].window);
            }
            if (status->window[0].header.cursorOn != 0) {
                int open = 0;

                for (i = 0; i < MenuWork.partyCount; i++) {
                    if (status->window[i].window.state == 3) {
                        open = 1;
                        break;
                    }
                }
                if (open != 0) {
                    status->cursor[0].cursor.x = status->window[MenuWork.partyCursor].window.x + 3;
                    status->cursor[0].cursor.y = status->window[MenuWork.partyCursor].window.y
                                        + status->window[MenuWork.partyCursor].window.height / 2 - 4;
                    status->cursor[0].cursor.work = CHAR_STATUS_CURSOR_WORK;
                    eCursolMain(&status->cursor[0].cursor);
                }
            }
        } else {
            for (i = 0; i < MenuWork.partyCount; i++) {
                MoveSlide(&status->window[i].window.x, &targetX[i], 3.0f);
                WindowDXMain(&status->window[i].window);
            }
            for (i = 0; i < MenuWork.reserveCount; i++) {
                MoveSlide(&status->window[3 + i].window.x, &targetX[3 + i], 3.0f);
                WindowDXMain(&status->window[3 + i].window);
            }
            status->cursor[0].cursor.x = status->window[MenuWork.partyCursor].window.x + 3;
            status->cursor[0].cursor.y = status->window[MenuWork.partyCursor].window.y
                                + status->window[MenuWork.partyCursor].window.height / 2 - 4;
            eCursolMain(&status->cursor[0].cursor);
            if (MenuWork.partyMode == 1 && MenuWork.reserveCount != 0) {
                status->cursor[1].cursor.x = status->window[3 + MenuWork.reserveCursor].window.x + 3;
                status->cursor[1].cursor.y = status->window[3 + MenuWork.reserveCursor].window.y
                                    + status->window[3 + MenuWork.reserveCursor].window.height / 2 - 4;
                eCursolMain(&status->cursor[1].cursor);
            }
        }
        break;
    }
}
/*
 * The portrait icon slots of the equip-list screen (CharListWork below holds
 * three of them). Only the colour word CharListMain writes right after
 * eSpriteSet is modeled; the rest is the sprite record's own layout
 * (main/e_number_main.c).
 */
typedef struct CharListIcon {
    unsigned char unmodeled_00[0x08];
    int color;                      /* +0x08 */
    unsigned char unmodeled_0c[0x28 - 0x0C];
} CharListIcon;

/*
 * The per-character stat rows of the equip-list screen (CharListWork below
 * holds eight of them). Only the colour word CharListMain writes right after
 * eNumberSet is modeled; the rest is the number record's own layout
 * (main/e_number_main.c).
 */
typedef struct CharListStatRow {
    unsigned char unmodeled_00[0x04];
    int color;                      /* +0x04 */
    unsigned char unmodeled_08[0x90 - 0x08];
} CharListStatRow;

/*
 * The list rows MenuListMake builds and MenuListGet returns, twelve bytes
 * each: CharListMake_Gun only sets the per-row flag at +0x08 that
 * MenuListMake and its subListMake00/01 fills clear. main/menu_1.c owns the
 * record and declares the same members in its own TU-local header, so this
 * is this TU's view of the rows it touches.
 */
typedef struct CharListRow {
    unsigned char unmodeled_00[0x08];
    unsigned char flag;             /* +0x08 */
    unsigned char unmodeled_09[0x0C - 0x09];
} CharListRow;

CharListRow *MenuListMake(int listIndex, int mode);
CharListRow *MenuListGet(int listIndex);

/* MenuSortSet, WindowSPItemChange and WindowSPSetSelect are not yet
 * recovered (main/menu_1.c and main/e_battle_win_open.c, still
 * assembler). */
void MenuSortSet(int listIndex, int type, int order);

/*
 * CharListSPWindow is the scrollable list window CharListMake_Wpn builds at
 * CharList+0x504 and hands to WindowSPItemChange/WindowSPSetSelect. Its
 * +0x14/+0x15 row/column grid and +0x1C item list match the fields those
 * still-assembler functions read at the same offsets for every WindowSP*
 * list they manage (main/e_battle_win_open.c). CharListMake_Gun, and
 * CharListMake_Acc/_Para (still assembler), fill this same window for their
 * own category: all four load CharList+0x504.
 * - state (+0x00): 0 while the list is still being built, 0x11 once a
 *   category has been populated (CharListMain).
 * - position (+0x04): the short MoveSlide eases toward the target the list
 *   state selects (CharListMain).
 * - contentHeight (+0x06): the scrolled content height, set per category
 *   (CharListMain); distinct from height (+0x0E), the fixed frame height.
 * - color (+0x08): the packed 0xRRGGBB tint CharListMain sets once.
 */
typedef struct CharListSPWindow {
    unsigned char state;        /* +0x00 */
    unsigned char rowCount;    /* +0x01 */
    unsigned char unmodeled_02[0x04 - 0x02];
    short position;             /* +0x04 */
    short contentHeight;        /* +0x06 */
    int color;                  /* +0x08 */
    short width;                /* +0x0C */
    short height;                /* +0x0E */
    int flags;                  /* +0x10 */
    unsigned char columns;      /* +0x14 */
    unsigned char rows;         /* +0x15 */
    unsigned char unmodeled_16[0x1C - 0x16];
    CharListRow *items;         /* +0x1C */
} CharListSPWindow;

void WindowSPItemChange(CharListSPWindow *window);
void WindowSPSetSelect(CharListSPWindow *window, unsigned char *savedSelection);

/*
 * CharListWork is the equip-list screen's work block.
 * - state (+0x00): the CharListMain state its jump table dispatches on.
 * - active (+0x01): CharListMain runs the window only while it is set.
 * - cursorIndex/scrollIndex (+0x02/+0x03): cleared when the list opens.
 * - color (+0x08): the packed 0xRRGGBB tint CharListMain sets once, matching
 *   the list window's own tint.
 */
typedef struct CharListWork {
    unsigned char state;        /* +0x00 */
    unsigned char active;       /* +0x01 */
    unsigned char cursorIndex;  /* +0x02 */
    unsigned char scrollIndex;  /* +0x03 */
    unsigned char unmodeled_04[0x08 - 0x04];
    int color;                  /* +0x08 */
    CharListIcon icons[3];      /* +0x0C */
    CharListStatRow rows[8];    /* +0x84 */
    CharListSPWindow weapon;   /* +0x504 */
    unsigned char unmodeled_524[0x3374 - 0x524];
    /* +0x3374: the twelve twelve-byte list rows CharListMake_Para builds in
     * place (addiu $16,$16,0xC at 0x00295900) and hands to the window as its
     * item list; CharListMain clears them when the screen opens. */
    unsigned char paraItems[12 * 12];
} CharListWork;

extern CharListWork *CharList;

/* Saved WindowSP selections, one 5-byte snapshot per list; index 15 = the
 * 4th slot, this screen's weapon list, and index 20 the 5th, its bullet
 * list. */
extern unsigned char MenuKeepSelect[0x64];

/*
 * The bullet record dataAttGet (ov01 VA 0x00a1a428, still assembler) returns
 * for one ammunition id; only the id of the weapon it belongs to (+0xC) is
 * read here, the same member main/menu_1.c's MenuBulletCheck compares. Kept
 * under the scaffold's undefined_funcs_auto.txt placeholder name func_A1A428
 * until ov01 recovers dataAttGet itself.
 */
typedef struct MenuBulletData {
    unsigned char unmodeled_00[0x0C];
    short weapon;                   /* +0x0C */
} MenuBulletData;

MenuBulletData *func_A1A428(int ammoId);

/*
 * MenuSortAddrGet and MenuSortCheck (main/menu_1.c) return the sort row of
 * one list and the number of entries in it. A row entry packs the item id in
 * its low halfword (MenuIdChange adds the high part).
 */
void *MenuSortAddrGet(int listIndex);
int MenuSortCheck(int listIndex);

/* The two list windows of CharList the selection calls work on: the
 * weapon/accessory list (+0x504) and the parameter-up list (+0x1C3C). */
extern CharListSPWindow *CharWinSP[2];
/*
 * CharParameter is the parameter window's work block. MenuCharactor carves it
 * out of the menu heap and CharParameterMain drives it: state 0 builds the
 * window and its widgets, state 2 runs one frame. The sub-window sits at +0x08
 * and is the same 0x194-byte control block WindowDXSet/WindowDXMain share, so
 * the eight numbers start at +0x19C.
 */
typedef struct ParaWindow {
    short x;                    /* +0x00 */
    short y;                    /* +0x02 */
    int color;                  /* +0x04 */
    short width;                /* +0x08 */
    short height;               /* +0x0A */
    unsigned char unmodeled_0c[0x10 - 0x0C];
    signed char state;          /* +0x10 */
    unsigned char unmodeled_11[0x194 - 0x11];
} ParaWindow;

/*
 * One of the eight numbers. eNumberSet clears the render colour, the mode at
 * +0x0C and the four bytes at +0x10..+0x13; eNumberMain formats the word at
 * +0x14 in base 10 and, when the byte at +0x13 is zero, draws one more sprite
 * chosen from the byte at +0x11 (lbu 0x13/0x11 at 0x002767f8/0x00276798).
 * This screen packs the current value in the low halfword of +0x14 and the
 * recalculated one in its high halfword, and reads both halves back to pick
 * the arrow (2 above, 1 below, 0 equal, and then no arrow at all).
 */
typedef struct ParaNumber {
    short x;                    /* +0x00 */
    short y;                    /* +0x02 */
    int color;                  /* +0x04 */
    unsigned char unmodeled_08[0x0E - 0x08];
    signed char size;           /* +0x0E: 5 for every number of this window */
    unsigned char digits;       /* +0x0F */
    unsigned char unmodeled_10[1];
    signed char arrow;          /* +0x11 */
    unsigned char unmodeled_12[1];
    signed char arrowHidden;    /* +0x13 */
    int value;                  /* +0x14 */
    unsigned char unmodeled_18[0x90 - 0x18];
} ParaNumber;

/*
 * One of the eight stat labels. eTagFontSet clears the state pair at +0x00 and
 * the render colour at +0x0C and stores the text pointer; this screen only
 * moves the label and dims its first colour channel.
 */
typedef struct ParaTagFont {
    unsigned char unmodeled_00[4];
    short x;                    /* +0x04 */
    short y;                    /* +0x06 */
    int color;                  /* +0x08 */
    signed char rgba[4];        /* +0x0C */
    unsigned char unmodeled_10[0x20 - 0x10];
} ParaTagFont;

/*
 * The ribbon that marks the selected parameter row: the same x/y/colour/size
 * head the sub-window uses, then the eight corner records eRibbonMain walks,
 * of which this screen colours six.
 */
typedef struct ParaRibbonPoint {
    unsigned char unmodeled_00[8];
    unsigned char rgba[4];      /* +0x08 */
} ParaRibbonPoint;

typedef struct ParaRibbon {
    short x;                    /* +0x00 */
    short y;                    /* +0x02 */
    int color;                  /* +0x04 */
    short width;                /* +0x08 */
    short height;               /* +0x0A */
    unsigned char unmodeled_0c[4];
    ParaRibbonPoint point[8];   /* +0x10 */
} ParaRibbon;

typedef struct CharParameterWork {
    unsigned char state;        /* +0x00 */
    /* +0x01: 1 while the window is built, then 7, or 15 once the equip list
     * reports state 3; +0x02 is reset to -1 with it. No recovered function
     * reads either back yet. */
    signed char mask;           /* +0x01 */
    signed char selectNo;       /* +0x02 */
    unsigned char unmodeled_03[1];
    int color;                  /* +0x04 */
    ParaWindow window;          /* +0x08 */
    ParaNumber number[8];       /* +0x19C */
    unsigned char unmodeled_61c[0x75C - 0x61C];
    ParaTagFont tag[8];         /* +0x75C */
    unsigned char ribbonShown;  /* +0x85C */
    ParaRibbon ribbon;          /* +0x860 */
} CharParameterWork;

extern CharParameterWork *CharParameter;

/* The tag word of each of the eight stat labels, in the order the window
 * lays them out. */
extern int msg_5[8];

/* The digit count of each of the eight numbers (three digits for the halfword
 * stats, two for the byte ones), copied into the frame as one record. */
typedef struct ParaDigitCounts {
    unsigned char count[8];
} ParaDigitCounts;

extern ParaDigitCounts D_004DAE40[];

/* The window slides in from x = -512 and rests at x = 16. */
#define PARA_WINDOW_HIDDEN (-512)
#define PARA_WINDOW_SHOWN  16

void CharParameterMain(void)
{
    CharParameterWork *work = CharParameter;
    int i;

    switch (work->state) {
    case 0:
        {
            ParaDigitCounts digits;
            int corner;

            work->color = 0x00FFFFF0;
            WindowDXSet(&work->window);
            work->window.x = PARA_WINDOW_HIDDEN;
            work->window.color = work->color;
            work->window.y = 313;
            work->window.width = 480;
            work->window.height = 64;
            work->window.state = 1;
            WindowDXMain(&work->window);
            work->window.state = 3;
            digits = D_004DAE40[0];
            for (i = 0; i < 8; i++) {
                eNumberSet(&work->number[i], 0);
                work->number[i].color = work->color + 2;
                work->number[i].size = 5;
                work->number[i].digits = digits.count[i];
            }
            for (i = 0; i < 8; i++) {
                eTagFontSet(&work->tag[i], msg_5[i]);
                work->tag[i].color = work->color + 2;
                work->tag[i].rgba[0] = 112;
            }
            work->ribbonShown = 0;
            eRibbonSet(&work->ribbon, 0);
            for (corner = 0; corner < 6; corner++) {
                ParaRibbonPoint *point = &work->ribbon.point[corner];

                switch (corner) {
                case 0:
                case 1:
                case 2:
                case 3:
                    point->rgba[0] = 32;
                    point->rgba[1] = 52;
                    point->rgba[2] = 52;
                    break;
                case 4:
                case 5:
                    point->rgba[2] = 0;
                    point->rgba[1] = 0;
                    point->rgba[0] = 0;
                    break;
                }
                if (corner == 2 || corner == 3) {
                    point->rgba[3] = 32;
                } else {
                    point->rgba[3] = 0;
                }
            }
            work->mask = 1;
            work->selectNo = -1;
            work->state = 2;
        }
        /* fallthrough */

    case 2:
        {
            short targetX;

            targetX = PARA_WINDOW_HIDDEN;
            switch (MenuWork.state) {
            case 82:
                work->mask = 7;
                /* fallthrough */
            case 83:
                if (CharWinSP[0]->state == 3) {
                    work->mask = 15;
                } else {
                    work->mask = 7;
                }
                targetX = PARA_WINDOW_SHOWN;
                break;
            case 32:
                work->mask = 7;
                /* fallthrough */
            case 33:
                targetX = PARA_WINDOW_SHOWN;
                break;
            case 64:
            case 65:
            case 84:
            case 85:
            case 128:
            case 129:
            case 130:
            case 131:
            case 132:
            case 133:
            case 134:
            case 135:
                targetX = PARA_WINDOW_SHOWN;
                break;
            }
            switch (MenuWork.state) {
            case 130:
            case 131:
                if (MenuWork.paraIndex >= 2) {
                    work->ribbonShown = 1;
                } else {
                    work->ribbonShown = 0;
                }
                break;
            default:
                work->ribbonShown = 0;
                break;
            }
            MoveSlide(&work->window.x, &targetX, 3.0f);
            WindowDXMain(&work->window);
            if (work->ribbonShown == 1) {
                const int rows = 3;
                ParaRibbon *ribbon;

                work->ribbon.x = (MenuWork.paraIndex - 2) / rows * 160 -
                                 (rows - (MenuWork.paraIndex - 2) / 4) * 11 + 104;
                work->ribbon.y = (MenuWork.paraIndex - 2) % rows * 20 + 316;
                work->ribbon.width = (rows - (MenuWork.paraIndex - 2) / 4) * 22 + 23;
                work->ribbon.height = 16;
                work->ribbon.color = work->color;
                ribbon = &work->ribbon;
                for (i = 5; i >= 0; i--) {
                    eRibbonMain(ribbon);
                }
            }
            if (work->window.state == 3) {
                const int rows = 3;

                for (i = 0; i < 7; i++) {
                    work->tag[i].x = work->window.x + i / rows * 160 + 48;
                    work->tag[i].y = work->window.y + i % rows * 20 + 3;
                    eTagFontMain(&work->tag[i]);
                }
                /* The four battle parameters are stored unsigned (lhu in
                 * MenuParaPtNowGet and MenuCharParaSet) but the pair of
                 * halfwords eNumberMain shows is read signed here (the eight
                 * lh at 0x00294cb0..0x00294cf8), so a value below its base
                 * shows as a negative difference. */
                work->number[0].value = (short)paraUnit[0].attack +
                                        (((short)paraUnit[1].attack + UpPara[2]) << 16);
                work->number[1].value = (short)paraUnit[0].phyDefense +
                                        (((short)paraUnit[1].phyDefense + UpPara[3]) << 16);
                work->number[2].value = (short)paraUnit[0].stat4 +
                                        (((short)paraUnit[1].stat4 + UpPara[4]) << 16);
                work->number[3].value = (short)paraUnit[0].magDefense +
                                        (((short)paraUnit[1].magDefense + UpPara[5]) << 16);
                work->number[4].value = paraUnit[0].stat6 +
                                        ((paraUnit[1].stat6 + UpPara[6]) << 16);
                work->number[5].value = paraUnit[0].stat7 +
                                        ((paraUnit[1].stat7 + UpPara[7]) << 16);
                work->number[6].value = paraUnit[0].stat8 + (paraUnit[1].stat8 << 16);
                work->number[7].value = paraUnit[0].stat9 + (paraUnit[1].stat9 << 16);
                for (i = 0; i < 7; i++) {
                    short shown[2];

                    shown[0] = work->number[i].value;
                    shown[1] = work->number[i].value >> 16;
                    if (shown[0] < shown[1]) {
                        work->number[i].arrowHidden = 0;
                        work->number[i].arrow = 2;
                    } else if (shown[1] < shown[0]) {
                        work->number[i].arrow = 1;
                        work->number[i].arrowHidden = 0;
                    } else {
                        work->number[i].arrow = 0;
                        if (MenuWork.paraIndex - 2 == i) {
                            switch (MenuWork.state) {
                            case 128:
                            case 129:
                            case 130:
                            case 131:
                            case 132:
                            case 133:
                            case 135:
                                work->number[i].arrowHidden = 0;
                                break;
                            default:
                                work->number[i].arrowHidden = 1;
                                break;
                            }
                        } else {
                            work->number[i].arrowHidden = 1;
                        }
                    }
                    work->number[i].x = work->window.x + i / rows * 160 + 89;
                    work->number[i].y = work->window.y + i % rows * 20 + 3;
                    eNumberMain(&work->number[i]);
                }
            }
        }
        break;
    }
}
/*
 * The widget control blocks the menu screens embed. WindowDXSet/WindowDXMain
 * take the sub-window (stride 404 bytes: CharWeaponMain reaches its second
 * one with `mult $3,$2,404` on MenuWork.equipKind), eMessageSet/eMessageMain
 * the text line, eSpriteSet/eSpriteMain the icon and eCursolSet/eCursolMain
 * the selection cursor. All four carry x/y at +0x04/+0x06 and the display
 * word this screen fills from CharWeaponWork.color at +0x08; the window
 * instead starts with its own x/y at +0x00/+0x02 and keeps that word at
 * +0x04, which is what MoveSlide slides and WindowDXMain draws.
 */
typedef struct CharWeaponWindow {
    short x;                            /* +0x000 */
    short y;                            /* +0x002 */
    int color;                          /* +0x004 */
    short width;                        /* +0x008 */
    short height;                       /* +0x00A */
    unsigned char unmodeled_00c[0x10 - 0x0C];
    signed char state;                  /* +0x010 */
    unsigned char unmodeled_011[0x194 - 0x11];
} CharWeaponWindow;

typedef struct CharWeaponMessage {
    unsigned char unmodeled_000[0x01];
    unsigned char mode;                 /* +0x001 */
    unsigned char unmodeled_002[0x04 - 0x02];
    short x;                            /* +0x004 */
    short y;                            /* +0x006 */
    int color;                          /* +0x008 */
    unsigned char unmodeled_00c[0x18 - 0x0C];
    const char *text;                   /* +0x018 */
    unsigned char unmodeled_01c[0x44 - 0x1C];
} CharWeaponMessage;

typedef struct CharWeaponSprite {
    unsigned char unmodeled_000[0x04];
    short x;                            /* +0x004 */
    short y;                            /* +0x006 */
    int color;                          /* +0x008 */
    unsigned char unmodeled_00c[0x28 - 0x0C];
} CharWeaponSprite;

typedef struct CharWeaponCursor {
    signed char state;                  /* +0x000 */
    unsigned char unmodeled_001[0x04 - 0x01];
    short x;                            /* +0x004 */
    short y;                            /* +0x006 */
    int color;                          /* +0x008 */
    unsigned char unmodeled_00c[0x24 - 0x0C];
} CharWeaponCursor;

/* Whether the weapon takes the bullet row of the weapon window as well. */
int MenuBulletCheck2(int weapon, int chrNo);

/*
 * The weapon icon of every party character (chrNo 1..7), as the scaffold's
 * .rodata piece at 0x004C6398 holds it; the fourth character carries no
 * weapon of its own. CharWeaponMain reads it through a copy on its frame.
 */
typedef struct WeaponIconTable {
    short icon[7];
} WeaponIconTable;

extern const WeaponIconTable D_004C6398;

/*
 * The accessory icon of every icon group, as the scaffold's small-data piece
 * at 0x004DAE48 holds it (the third group shares the empty-row sprite).
 * CharWeaponMain reads it through a copy on its frame.
 */
typedef struct AccessoryIconTable {
    short icon[3];
} AccessoryIconTable;

extern const AccessoryIconTable D_004DAE48[];

/*
 * The accessory record, as far as the equipment display needs it: the icon
 * group its sprite comes from. src/ov01/calc.h owns the AccessoryData type
 * and models its +0x0D term, so this screen keeps its own bounded view of
 * the same record until the two are harvested into one header.
 */
typedef struct MenuAccessoryData {
    unsigned char unmodeled_00[0x0A];
    unsigned short iconKind;            /* +0x0A */
} MenuAccessoryData;

/* dataAccGet (ov01 VA 0x00a1a548), linked by the scaffold's
 * undefined_funcs_auto.txt placeholder name. */
MenuAccessoryData *func_A1A548(int accessory);

/*
 * The weapon/accessory display of the character menu (0x770 bytes carved
 * from the menu heap). window[0] is the weapon window with the weapon line
 * (message/sprite 0) and the bullet line (message/sprite 1); window[1] is
 * the accessory window with its three rows (message/sprite 2..4); the cursor
 * marks the row MenuWork.equipSlot in the window MenuWork.equipKind selects.
 * color is the display word every widget copies, the foreground ones with 2
 * added.
 */
typedef struct CharWeaponWork {
    unsigned char state;                /* +0x000 */
    unsigned char subState;             /* +0x001: cleared with the move to state 2 */
    unsigned char unmodeled_002[0x04 - 0x02];
    int color;                          /* +0x004 */
    CharWeaponWindow window[3];             /* +0x008 */
    CharWeaponCursor cursor;                  /* +0x4C4 */
    CharWeaponMessage message[6];             /* +0x4E8 */
    CharWeaponSprite sprite[6];               /* +0x680 */
} CharWeaponWork;

extern CharWeaponWork *CharWeapon;

/* The display word of the equipment windows: opaque, farthest back. */
#define MENU_WEAPON_COLOR 0x00FFFFF0

/* The row the cursor and the accessory lines advance by, and the offset the
 * weapon line drops to when the weapon has no bullet row. */
#define MENU_WEAPON_ROW_HEIGHT 24
#define MENU_WEAPON_LINE_DROP  12

/* The sprite drawn for the bullet line and for an empty accessory row. */
#define SPRITE_BULLET        780
#define SPRITE_NO_ACCESSORY  781

void CharWeaponMain(void)
{
    CharWeaponWork *work = CharWeapon;
    CharParaData *total;
    int attack[3];
    short targetX[2];
    short targetY[2];
    int bulletFits;
    short weaponId;
    short spriteId;
    int i;

    switch (work->state) {
    case 0:
        work->color = MENU_WEAPON_COLOR;
        WindowDXSet(&work->window[0]);
        work->window[0].x = -238;
        work->window[0].y = 48;
        work->window[0].color = work->color;
        work->window[0].width = 228;
        work->window[0].height = 54;
        for (i = 0; i < 2; i++) {
            eSpriteSet(&work->sprite[i], 0);
            eMessageSet(&work->message[i], 0);
            work->message[i].mode = 32;
        }
        work->window[0].state = 1;
        WindowDXMain(&work->window[0]);
        work->window[0].state = 3;
        WindowDXSet(&work->window[1]);
        work->window[1].x = -238;
        work->window[1].y = 102;
        work->window[1].color = work->color;
        work->window[1].width = 228;
        work->window[1].height = 78;
        for (i = 0; i < 3; i++) {
            eSpriteSet(&work->sprite[i + 2], 0);
            eMessageSet(&work->message[i + 2], 0);
            work->message[i + 2].mode = 32;
        }
        work->window[1].state = 1;
        WindowDXMain(&work->window[1]);
        work->window[1].state = 3;
        eCursolSet(&work->cursor, 0);
        work->cursor.color = work->color + 2;
        work->cursor.state = 32;
        work->state = 2;
        work->subState = 0;
        /* fallthrough */
    case 2:
        total = func_00A11108(MenuWork.view.chrNo, attack, attack);
        for (i = 0; i < 2; i++) {
            targetX[i] = -238;
            targetY[i] = i * 54 + 48;
        }
        switch (MenuWork.state) {
        case 0x10:
        case 0x11:
        case 0x20:
        case 0x21:
            work->cursor.state = 0;
            break;
        case 0x40:
            work->cursor.state = 32;
            /* fallthrough */
        case 0x41:
            for (i = 0; i < 2; i++) {
                targetX[i] = 16;
            }
            break;
        case 0x52:
        case 0x53:
        case 0x54:
        case 0x55:
            targetX[MenuWork.equipKind] = 16;
            targetY[MenuWork.equipKind] = 48;
            break;
        }
        weaponId = total->weapon[setSlotTbl[MenuWork.view.chrNo - 1]];
        bulletFits = MenuBulletCheck2(weaponId, MenuWork.view.chrNo);
        MoveSlide(&work->window[0].x, &targetX[0], 3.0f);
        MoveSlide(&work->window[0].y, &targetY[0], 3.0f);
        if (MenuMainCharCheck(MenuWork.view.chrNo) != 0) {
            WindowDXMain(&work->window[0]);
            work->message[0].text = MenuTextGet(MENU_TEXT_WEAPON + (unsigned short)weaponId)->name;
            work->message[0].x = work->window[0].x + 39;
            work->message[0].color = work->color + 2;
            work->message[0].y = work->window[0].y + (bulletFits ^ 1) * MENU_WEAPON_LINE_DROP + 3;
            eMessageMain(&work->message[0]);
            {
                WeaponIconTable weaponIcon = D_004C6398;

                eSpriteSet(&work->sprite[0], weaponIcon.icon[MenuWork.view.chrNo - 1]);
            }
            work->sprite[0].x = work->window[0].x + 19;
            work->sprite[0].color = work->color + 2;
            work->sprite[0].y = work->window[0].y + (bulletFits ^ 1) * MENU_WEAPON_LINE_DROP + 3;
            eSpriteMain(&work->sprite[0]);
            if (bulletFits != 0) {
                work->message[1].text =
                    MenuTextGet(MENU_TEXT_BULLET + total->bullet[setSlotTbl[MenuWork.view.chrNo - 1]])->name;
                work->message[1].x = work->window[0].x + 39;
                work->message[1].color = work->color + 2;
                work->message[1].y = work->window[0].y + 27;
                eMessageMain(&work->message[1]);
                eSpriteSet(&work->sprite[1], SPRITE_BULLET);
                work->sprite[1].x = work->window[0].x + 19;
                work->sprite[1].color = work->color + 2;
                work->sprite[1].y = work->window[0].y + 27;
                eSpriteMain(&work->sprite[1]);
            }
        }
        MoveSlide(&work->window[1].x, &targetX[1], 3.0f);
        MoveSlide(&work->window[1].y, &targetY[1], 3.0f);
        WindowDXMain(&work->window[1]);
        if (work->window[1].state == 3) {
            for (i = 0; i < 3; i++) {
                work->message[i + 2].text =
                    MenuTextGet(MENU_TEXT_ACCESSORY + (unsigned short)total->accessory[i])->name;
                work->message[i + 2].x = work->window[1].x + 39;
                work->message[i + 2].y = work->window[1].y + i * MENU_WEAPON_ROW_HEIGHT + 3;
                work->message[i + 2].color = work->color + 2;
                eMessageMain(&work->message[i + 2]);
                spriteId = SPRITE_NO_ACCESSORY;
                if (total->accessory[i] != 0) {
                    AccessoryIconTable accessoryIcon = D_004DAE48[0];

                    spriteId = accessoryIcon.icon[func_A1A548(total->accessory[i])->iconKind];
                }
                eSpriteSet(&work->sprite[i + 2], spriteId);
                work->sprite[i + 2].x = work->window[1].x + 19;
                work->sprite[i + 2].color = work->color + 2;
                work->sprite[i + 2].y = work->window[1].y + i * MENU_WEAPON_ROW_HEIGHT + 3;
                eSpriteMain(&work->sprite[i + 2]);
            }
        }
        work->cursor.x = work->window[MenuWork.equipKind].x + 3;
        work->cursor.y = work->window[MenuWork.equipKind].y +
                         MenuWork.equipSlot * MENU_WEAPON_ROW_HEIGHT + 7;
        if (MenuWork.equipKind == 0) {
            work->cursor.y += (bulletFits ^ 1) * MENU_WEAPON_LINE_DROP;
        }
        eCursolMain(&work->cursor);
        break;
    }
}
void CharListMake_Wpn(void)
{
    CharListSPWindow *window = &CharList->weapon;

    /* MenuSortSet's order argument is MenuWork.view.chrNo (+0x60), the character
     * whose equipment the screen lists. */
    MenuSortSet(0, 4, *(short *)((char *)&MenuWork + 0x60));
    MenuListMake(0, 0);
    window->width = 228;
    window->rows = 7;
    window->height = 174;
    window->columns = 1;
    window->rowCount = 7;
    window->flags = 0;
    window->items = MenuListGet(0);
    WindowSPItemChange(window);
    WindowSPSetSelect(window, &MenuKeepSelect[15]);
}

/* The bullet list: the same window and geometry as the weapon list, with each
 * row flagged when its bullet does not fit the weapon under the cursor. */
void CharListMake_Gun(void)
{
    int *sortRow = MenuSortAddrGet(0);
    CharListRow *entries = MenuListGet(0);
    CharListSPWindow *window = &CharList->weapon;
    int count;
    int i;

    MenuSortSet(0, 8, MenuWork.view.chrNo);
    MenuListMake(0, 0);
    count = MenuSortCheck(0);
    for (i = 0; i < count; i++) {
        if (func_A1A428((unsigned short)sortRow[i])->weapon != MenuWork.weaponId) {
            entries[i].flag = 1;
        } else {
            entries[i].flag = 0;
        }
    }
    window->width = 228;
    window->rows = 7;
    window->height = 174;
    window->columns = 1;
    window->rowCount = 7;
    window->flags = 0;
    window->items = MenuListGet(0);
    WindowSPItemChange(window);
    WindowSPSetSelect(window, &MenuKeepSelect[20]);
}
INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", CharListMake_Acc);

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", CharListMake_Para);

/* D_004DADB0: scaffold data CharListMain and CharListMake_Para store as the
 * list window's flags; no further evidence of its own layout. */
extern unsigned char D_004DADB0[];

/* WindowSPSet and WindowSPMain are not yet recovered
 * (main/e_battle_win_open.c, still assembler); both read only the window
 * pointer in $4 (WindowSPSet overwrites $5 and $6 with its own values at
 * 0x00283b88 before reading them), so the window geometry this state stores
 * stays in the argument registers across the call without being passed. */
void WindowSPSet(CharListSPWindow *window);
void WindowSPMain(CharListSPWindow *window);

/* CharListMake_Acc and CharListMake_Para are still assembler in this TU;
 * like CharListMake_Wpn they take no arguments. */
void CharListMake_Acc(void);
void CharListMake_Para(void);

/*
 * The equip-list screen's frame step: it opens the list window, fills it for
 * the category MenuWork.state asks for, slides it in and out, and runs the
 * window while the list is up.
 */
void CharListMain(void)
{
    CharListWork *list = CharList;
    int i;

    switch (list->state) {
    case 0:
        list->weapon.position = 528;
        list->weapon.contentHeight = 64;
        list->color = 0x00FFFFF8;
        list->weapon.color = 0x00FFFFF8;
        list->weapon.rowCount = 7;
        list->weapon.flags = (int)D_004DADB0;
        list->weapon.rows = 11;
        list->weapon.height = 270;
        list->weapon.state = 0;
        list->weapon.items = 0;
        WindowSPSet(&list->weapon);
        for (i = 0; i < 3; i++) {
            eSpriteSet(&list->icons[i], 1546);
            list->icons[i].color = 0x00FFFFFF;
        }
        for (i = 0; i < 8; i++) {
            eNumberSet(&list->rows[i], 0);
            list->rows[i].color = 0x00FFFFFA;
        }
        list->cursorIndex = 0;
        list->state = 2;
        list->active = 0;
        list->scrollIndex = 0;
        memset(list->paraItems, 0, sizeof(list->paraItems));
        /* fallthrough */
    case 2:
        list->active = 0;
        switch (MenuWork.state) {
        case 85:
            list->state = 20;
            break;
        case 83:
            list->state = 10;
            break;
        case 129:
            list->state = 30;
            break;
        }
        break;

    case 10:
    case 20:
    case 30:
        list->weapon.position = -244;
        if (list->state == 10) {
            if (MenuWork.equipKind == 0) {
                list->weapon.contentHeight = 102;
                CharListMake_Wpn();
            } else {
                list->weapon.contentHeight = 153;
                CharListMake_Acc();
            }
            list->state = 11;
        } else if (list->state == 20) {
            list->weapon.contentHeight = 102;
            CharListMake_Gun();
            list->state = 21;
        } else {
            list->weapon.contentHeight = 80;
            CharListMake_Para();
            list->state = 11;
        }
        list->active = 1;
        list->weapon.state = 0x11;
        /* fallthrough */
    case 11:
    case 13:
    case 21:
    {
        short target = 16;

        if (list->state == 13) {
            target = -244;
        }
        MoveSlide(&list->weapon.position, &target, 3.0f);
        if (list->weapon.position == target) {
            switch (list->state) {
            case 13:
                list->state = 2;
                break;
            case 11:
                list->state = 12;
                break;
            case 21:
                list->state = 22;
                break;
            }
        }
        break;
    }

    case 12:
        switch (MenuWork.state) {
        case 65:
        case 33:
            list->state = 13;
            break;
        case 85:
            list->state = 20;
            break;
        }
        break;

    case 22:
        if (MenuWork.state == 65) {
            list->state = 13;
        }
        break;
    }

    if (list->active) {
        WindowSPMain(&list->weapon);
    }
}
/*
 * Partial view of the sprite object eSpriteSet/eSpriteMain (main/e_number_main.c,
 * still assembler at this TU's boundary) receive: the x/y/work fields this
 * call overwrites right after eSpriteSet, at the same offsets eSpriteSet's
 * own ESprite type uses for them.
 */
typedef struct CharSwitchArrow {
    unsigned char unmodeled_00[0x04];
    short x;                        /* +0x04 */
    short y;                        /* +0x06 */
    int work;                       /* +0x08 */
} CharSwitchArrow;

/*
 * Partial view of the message object eMessageSet/eMessageMain (main/e_message.c,
 * still assembler at this TU's boundary) receive, at the same x/y/color
 * offsets menu_shop.c's MenuShopMessage evidences for the same two
 * functions; mode is eMessageModeChange's own field (main/e_message.c).
 */
typedef struct CharSwitchMessage {
    unsigned char unmodeled_00;
    unsigned char mode;             /* +0x01 */
    unsigned char unmodeled_02[0x04 - 0x02];
    short x;                        /* +0x04 */
    short y;                        /* +0x06 */
    int color;                      /* +0x08 */
} CharSwitchMessage;

/*
 * The switch-character arrow indicator MenuCharactor carves from the menu
 * heap (still assembler there); only the members CharSwitchMain touches are
 * modelled.
 */
typedef struct CharSwitchWork {
    unsigned char state;            /* +0x00 */
    unsigned char unmodeled_01[0x04 - 0x01];
    int color;                      /* +0x04 */
    CharSwitchArrow arrow;          /* +0x08 */
    unsigned char unmodeled_14[0x30 - 0x14];
    CharSwitchMessage message;      /* +0x30 */
} CharSwitchWork;

extern CharSwitchWork *CharSwitch;

/* The character-switch prompt's text pointer (config/symbols/main.txt: msg_6,
 * size 4), whose stored address is the string "Set lead character"; the
 * pointer itself is still part of this TU's assembler data. */
extern char *msg_6[];

/*
 * The switch-character arrow and its "press to confirm" prompt: state 0
 * initialises both and starts sliding the arrow toward its resting x (or a
 * nearer one while MenuWork is on states 0x10/0x11); state 2 keeps
 * sliding it and tracks the prompt's position and colour off the arrow.
 */
void CharSwitchMain(void)
{
    CharSwitchWork *csw = CharSwitch;

    switch (csw->state) {
    case 0:
        csw->color = 0x00FFFFF0;
        eSpriteSet(&csw->arrow, 0x201);
        csw->arrow.x = 0x210;
        csw->arrow.y = 0x0C;
        csw->arrow.work = csw->color;
        eMessageSet(&csw->message, *msg_6);
        csw->message.color = csw->color;
        csw->message.mode = 0x20;
        csw->state = 2;
        /* fallthrough */
    case 2:
    {
        short target = 0x210;

        switch (MenuWork.state) {
        case 0x10:
        case 0x11:
            target = 0x110;
            break;
        }
        MoveSlide(&csw->arrow.x, &target, 3.0f);
        eSpriteMain(&csw->arrow);
        csw->message.x = csw->arrow.x + 20;
        csw->message.y = csw->arrow.y - 2;
        csw->message.color = csw->arrow.work;
        eMessageMain(&csw->message);
        break;
    }
    }
}
/* The "T.Pts" panel title window_tex_load's string table holds. */
extern const char D_004DAE58[];

/* The two eTagFontSet values CharPointMain's title-row loop passes. */
extern const int msg_7_0036D938[];

/*
 * One title-row slot (eTagFontSet/eTagFontMain object): x/y (+0x04/+0x06)
 * position the row and id (+0x08) is a per-row handle CharPointMain derives
 * from tagBase.
 */
typedef struct CharPointTagSlot {
    unsigned char unmodeled_00[0x04];
    short x;                        /* +0x04 */
    short y;                        /* +0x06 */
    int id;                         /* +0x08 */
    unsigned char unmodeled_0c[0x20 - 0x0C];
} CharPointTagSlot;

/*
 * One point-value slot (eNumberSet/eNumberMain object): x/y position it,
 * id is the per-row handle, flagA (+0x0E) and flagB (+0x0F) are the two
 * bytes this panel rewrites as 0 and 4 every frame before eNumberMain, and
 * value (+0x14) is the number eNumberMain formats in base 10.
 */
typedef struct CharPointNumberSlot {
    short x;                        /* +0x00 */
    short y;                        /* +0x02 */
    int id;                         /* +0x04 */
    unsigned char unmodeled_08[0x0E - 0x08];
    signed char flagA;              /* +0x0E */
    signed char flagB;              /* +0x0F */
    unsigned char unmodeled_10[0x14 - 0x10];
    /* The original stores the first slot's value before it reads
     * MenuWork.paraCost, so the two accesses conflict. */
    union {
        int value;
    } number;                       /* +0x14 */
    unsigned char unmodeled_18[0x90 - 0x18];
} CharPointNumberSlot;

/*
 * The panel's background quad (eSpriteSet/eSpriteMain object): x/y position
 * it, id is the same kind of per-row handle, and printX/printY are the
 * first of the two points endPrintExtFunc's caption line reads.
 */
typedef struct CharPointSprite {
    unsigned char unmodeled_00[0x04];
    short x;                        /* +0x04 */
    short y;                        /* +0x06 */
    int id;                         /* +0x08 */
    unsigned char unmodeled_0c[0x28 - 0x0C];
    short printX;                   /* +0x28 */
    short printY;                   /* +0x2A */
} CharPointSprite;

/*
 * A tint record: id is the same per-row handle, r/g/b/a are the colour
 * eSpriteMain/eTagFontMain blend into their render colour (the byte order
 * MenuBgColor uses), and extraX/extraY (only ever set on the first record)
 * are the second of the two points endPrintExtFunc's caption line reads.
 */
typedef struct CharPointColorSlot {
    int id;                         /* +0x00 */
    signed char r;                  /* +0x04 */
    signed char g;                  /* +0x05 */
    signed char b;                  /* +0x06 */
    signed char a;                  /* +0x07 */
    short extraX;                   /* +0x08 */
    short extraY;                   /* +0x0A */
} CharPointColorSlot;

/*
 * CharPointMain's own work block: window_tex_load carves it from the menu
 * heap (0x3D8 bytes) and CharPoint points at it. state (+0x000) is 0 before
 * the panel has been built and 1 once CharPointMain has built it once.
 * width/height (+0x008/+0x00A) are the WindowDXSet/WindowDXMain window's own
 * size, and every child slot below is positioned relative to them.
 * windowTagId/windowField3/windowField4/title/windowState (+0x00C..+0x018)
 * are the remaining window fields CharPointMain itself writes.
 */
typedef struct CharPointPanel {
    short width;                                    /* +0x000 */
    unsigned short height;                          /* +0x002 */
    int windowTagId;                                /* +0x004 */
    short windowField3;                             /* +0x008 */
    short windowField4;                             /* +0x00A */
    const char *title;                              /* +0x00C */
    unsigned char windowState;                      /* +0x010 */
    unsigned char unmodeled_011[0x194 - 0x011];
    CharPointTagSlot tagSlots[2];                   /* +0x194 */
    CharPointNumberSlot numberSlots[3];             /* +0x1D4 */
    CharPointSprite sprite;                         /* +0x384 */
    CharPointColorSlot colors[2];                   /* +0x3B0 */
    int frameStart;                                 /* +0x3C8 */
    unsigned char unmodeled_3cc[0x3D0 - 0x3CC];
} CharPointPanel;

typedef struct CharPointWork {
    unsigned char state;                            /* +0x000 */
    unsigned char unmodeled_001[3];
    int tagBase;                                    /* +0x004 */
    CharPointPanel panel;                           /* +0x008 */
} CharPointWork;

extern CharPointWork *CharPoint;

void CharPointMain(void)
{
    CharPointWork *work = CharPoint;

    switch (work->state) {
    case 0:
    {
        int i;

        work->tagBase = 0xF00000;
        WindowDXSet(&work->panel.width);
        work->panel.width = 0x210;
        work->panel.height = 0xA0;
        work->panel.windowTagId = work->tagBase;
        work->panel.title = D_004DAE58;
        work->panel.windowField3 = 0x80;
        work->panel.windowField4 = 0x50;
        work->panel.windowState = 1;
        WindowDXMain(&work->panel.width);
        work->panel.windowState = 3;

        for (i = 0; i < 2; i++) {
            eTagFontSet(&work->panel.tagSlots[i], msg_7_0036D938[i]);
            work->panel.tagSlots[i].id = work->tagBase + 2;
        }

        for (i = 0; i < 3; i++) {
            eNumberSet(&work->panel.numberSlots[i], 0);
            work->panel.numberSlots[i].id = work->tagBase + 2;
        }

        eSpriteSet(&work->panel.sprite, 1798);
        work->panel.sprite.id = work->tagBase + 2;

        for (i = 0; i < 2; i++) {
            work->panel.colors[i].b = -96;
            work->panel.colors[i].g = -96;
            work->panel.colors[i].r = -96;
            work->panel.colors[i].a = -128;
            work->panel.colors[i].id = work->tagBase + 2;
        }

        work->panel.frameStart = 0;
        work->state = 1;
        break;
    }
    case 1:
        break;
    default:
        return;
    }

    {
        /* The menu state drives the panel's target width; it is compared as
         * a signed int (slti), so it is read into one. */
        int menuState = MenuWork.state;
        short targetWidth = 0x210;
        CharPointData *record;
        int points;
        int cost;
        int i;

        if (menuState >= 130) {
            if (menuState >= 132) {
                if (menuState < 134) {
                    targetWidth = 0xC0;
                }
            } else {
                targetWidth = 0x110;
            }
        }

        MoveSlide(&work->panel.width, &targetWidth, 3.0f);
        WindowDXMain(&work->panel.width);

        for (i = 0; i < 2; i++) {
            work->panel.tagSlots[i].x = work->panel.width + 68;
            work->panel.tagSlots[i].y = work->panel.height + ((i % 2) * 3 + (i / 2) * 5) * 16 + 4;
            eTagFontMain(&work->panel.tagSlots[i]);
        }

        work->panel.sprite.x = work->panel.width + 57;
        work->panel.sprite.y = work->panel.height + 28;
        eSpriteMain(&work->panel.sprite);

        record = func_A19210(MenuWork.view.chrNo);
        points = record->points;
        work->panel.numberSlots[0].number.value = points;
        cost = MenuWork.paraCost;
        work->panel.numberSlots[1].number.value = cost;
        work->panel.numberSlots[2].number.value = points - cost;

        for (i = 0; i < 3; i++) {
            work->panel.numberSlots[i].x = work->panel.width + 72;
            work->panel.numberSlots[i].y = work->panel.height + ((i % 3) * 24 + (i / 3) * 80) + 4;
            work->panel.numberSlots[i].flagA = 0;
            work->panel.numberSlots[i].flagB = 4;
            eNumberMain(&work->panel.numberSlots[i]);
        }

        work->panel.sprite.printX = work->panel.width + 11;
        work->panel.colors[0].extraX = work->panel.width + 115;
        work->panel.colors[0].extraY = work->panel.height + 48;
        work->panel.sprite.printY = work->panel.height + 48;
        endPrintExtFunc(work->tagBase, 0, &work->panel.sprite.printX);
    }
}
INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", MenuCharCameraSet);

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", MenuCharCameraMove);

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", MenuCharModelMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", MenuBattleModelMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", tskCharLineField);

static void tskCharSpotLight(CharSpotLightTask *light)
{
    SpotLightTable positions = D_004C6770;
    DirectCircle circle;
    int screen[4];

    if (MenuWork.state == MENU_WORK_END || MenuWork.partyMode == 0) {
        xglTaskWaitRemove(&light->task);
        return;
    }
    switch (light->mode) {
    case SPOTLIGHT_START:
        light->mode = SPOTLIGHT_HIDDEN;
        light->visible = 0;
        light->counter = 0;
        /* fallthrough */
    case SPOTLIGHT_HIDDEN:
        if ((MenuWork.state >> 4) == MENU_STATE_GROUP_FORMATION) {
            light->mode = SPOTLIGHT_SHOWN;
            light->pulse = 1.0f;
            light->pulseStep = D_004D7E50;
            light->size = 0.0f;
            light->position = positions.position[MenuWork.formationSlot - 1];
            light->visible = 1;
        }
        break;
    case SPOTLIGHT_SHOWN:
        if (light->size < 1.0f) {
            light->size += 0.125f;
        } else {
            light->size = 1.0f;
        }
        if ((MenuWork.state >> 4) != MENU_STATE_GROUP_FORMATION) {
            light->mode = SPOTLIGHT_FADE_OUT;
        }
        break;
    case SPOTLIGHT_FADE_OUT:
        if (light->size != 0.0f) {
            light->size -= 0.125f;
        } else {
            light->mode = SPOTLIGHT_HIDDEN;
            light->visible = 0;
        }
        break;
    }
    light->pulse += light->pulseStep;
    if (light->pulseStep > 0.0f) {
        if (light->pulse >= 1.0f) {
            light->pulseStep = -light->pulseStep;
        }
    } else if (light->pulse <= 0.0f) {
        light->pulseStep = -light->pulseStep;
    }
    if (light->visible) {
        light->position.x += (positions.position[MenuWork.formationSlot - 1].x - light->position.x) * D_004D7E54;
        light->position.y += (positions.position[MenuWork.formationSlot - 1].y - light->position.y) * D_004D7E54;
        light->position.z += (positions.position[MenuWork.formationSlot - 1].z - light->position.z) * D_004D7E54;
        xglRotTransPersN(screen, 0, &light->position, 1, 0);
        screen[0] = screen[0] / 16 - CIRCLE_ORIGIN_X;
        screen[1] = screen[1] / 16 - CIRCLE_ORIGIN_Y;
        circle.x = screen[0];
        circle.y = screen[1];
        circle.z = screen[2];
        circle.innerColor[0] = 0xA0;
        circle.innerColor[1] = 0xA0;
        circle.innerColor[2] = 0x60;
        circle.innerColor[3] = 0x80;
        circle.radius = light->size * 48.0f + light->pulse * 16.0f;
        circle.segments = 32;
        circle.outerColor[0] = 0xA0;
        circle.outerColor[1] = 0xA0;
        circle.outerColor[2] = 0x60;
        circle.outerColor[3] = 0;
        endPrintDirectCircle(&circle);
    }
}

void MenuCharParaSet(int chrNo, int previewOnly)
{
    CharParaData *org;
    CharParaData *total;
    int attack[3];
    int defense[2];
    short keptHp;
    short keptEp;
    int i;

    if (chrNo > 0) {
        org = func_A191C0(chrNo);
        keptHp = 0;
        keptEp = 0;
        if (chrNo >= 1 && chrNo <= 7) {
            if (org->maxHp < org->hp) {
                keptHp = org->hp;
            }
            if (org->maxEp < org->ep) {
                keptEp = org->ep;
            }
        }
        total = func_00A11108(chrNo, attack, defense);
        if (total != 0) {
            total->attack += attack[setSlotTbl[chrNo - 1]];
            total->phyDefense += defense[0];
            total->magDefense += defense[1];
        }
        if (previewOnly == 0) {
            paraUnit[0] = paraUnit[1] = *total;
            for (i = 0; i < 3; i++) {
                if (org->accessory[i] == ACCESSORY_KEEP_HP && keptHp != 0) {
                    org->hp = keptHp;
                }
                if (org->accessory[i] == ACCESSORY_KEEP_EP && keptEp != 0) {
                    org->ep = keptEp;
                }
            }
        } else {
            paraUnit[1] = *total;
            if (keptHp != 0) {
                org->hp = keptHp;
            }
            if (keptEp != 0) {
                org->ep = keptEp;
            }
        }
    }
}

/*
 * The sizes of the work blocks MenuCharactor carves out of the menu heap
 * after MainMenuWorkEnd, one per sub-screen. Each sub-screen starts idle when
 * the first byte of its block is 0; CharEx keeps that byte at +0x10.
 */
#define CHAR_PAS_SIZE       0x550
#define CHAR_CATEGORY_SIZE  0x244
#define CHAR_MENU_SIZE      0x1E38
#define CHAR_INFO_SIZE      0x450
#define CHAR_STATUS_SIZE    0x4EF4
#define CHAR_PARAMETER_SIZE 0x8D0
#define CHAR_EX_SIZE        0x378
#define CHAR_WEAPON_SIZE    0x770
#define CHAR_LIST_SIZE      0x3404
#define CHAR_SWITCH_SIZE    0x74
#define CHAR_L1R1_SIZE      0x58
#define CHAR_POINT_SIZE     0x3D8

/* First free byte of the menu heap after the main menu's own blocks. */
extern unsigned char *MainMenuWorkEnd;
extern XglTaskScheduler *MenuTask_XMX;

/* The parameter table file read into ParaDataBuf (2 KB aligned). */
extern const char D_004C67D0[];
/* The debug caption printed above the state dump. */
extern const char D_004C67F0[];

/* Sound effect ids. */
#define SE_DECIDE 1
#define SE_CANCEL 2
#define SE_CURSOR 3
#define SE_BUZZER 5
#define SE_SLIDE  10
#define SE_SWAP   23
#define SE_PLACE  24

/* The three menu model handles, one per attacker slot. */
extern int MenuModelOut[];

/* The points one level-up of a parameter adds. */
extern signed char ParaPointTbl[];

/* The id of the character with two weapon slots (both are equipped). */
#define CHR_DUAL_WEAPON 7
/* The menu id that cannot be switched to or equipped (the guest). */
#define CHR_GUEST 12
/* The character whose weapon cannot be changed (only the bullet). */
#define CHR_FIXED_WEAPON 3

/* MenuKeepSelect holds one 5-byte cursor snapshot per list: the three
 * accessory sort orders first (0..2), then these fixed lists. */
#define KEEP_WEAPON     3
#define KEEP_BULLET     4
#define KEEP_PARAMETER  5

/* The weapon record, as far as this menu needs it: the bullet it is loaded
 * with by default (ov01's calc code owns the whole record). */
typedef struct MenuWeaponData {
    unsigned char unmodeled_00[0x1A];
    short bullet;                   /* +0x1A */
} MenuWeaponData;

/* dataWpnGet (ov01 VA 0x00a1a3d8), linked by the scaffold's
 * undefined_funcs_auto.txt placeholder name. */
MenuWeaponData *func_A1A3D8(int weapon);

typedef struct PartyAttackPosition {
    unsigned short party_id;
    signed char attack_position;
} PartyAttackPosition;

typedef struct PartyDataPrefix {
    u8 _unmodeled_00[0x24];
    u16 lock_party_mask;
    u16 out_friend_mask;
    u16 friend_mask;
    u16 take_agws_mask;
    u16 leader_battle_id;
    u8 radar_disp;
    PartyAttackPosition attack_slots[3];
} PartyDataPrefix;

int PartyLeaderSet(int leader);
int PartyLeaderCheck(int chara_id);
int PartyFriendLockCheck(int id, int flag);
int MenuMaryIdChange(int id);
int PartyAttackerGet(int *out);
void PartyAttackPosSet(int party_id, int attack_position);
void PartyAttackPosChange(int first_party_id, int second_party_id);

void xglSoundEffectNormalID(int sound_id, int variant);
void xglFontDebugPrintf(int x, int y, const char *format, ...);
void xglFontDebugHex(int x, int y, int value, int digits);
XglTaskPrefix *xglTaskEntryNext(XglTaskScheduler *scheduler,
                                int (*callback)(XglTaskPrefix *task),
                                XglTaskPrefix *entry);

void MenuWorkEndCheck(unsigned char *end);
void MenuKeepSelectReset(void);
int MenuCursorKeepCheck(void);
signed char MenuSelectMove(int cursor, int count, int wrap);
int MenuCharLeaderMask(unsigned short chrNo);
int MenuBulletCheck(int weapon, int bullet);
int MenuAccessoryEquipCheck(int chrNo, int accessory, int slot);
void MenuModelMemorySet(int count);
void MenuModelMenuMotionLoad(void);
void MenuModelCreate(int *model, int chrNo);
void MenuModelExtFuncSet(int model, void (*func)(void), int argument);
void MenuModelAllBreak(void);
void ChangeTopLevel(int level);

int WindowSPSelect(CharListSPWindow *window, int pad);
void WindowSPKeepSelect(CharListSPWindow *window, unsigned char *keep);
void WindowSPKeepSelectCheck(unsigned char *keep);

void dataWpnBoxInc(int weapon);
void dataWpnBoxDec(int weapon);
void dataAccBoxInc(int accessory);
void dataAccBoxDec(int accessory);
void dataBltBoxInc(int bullet);
void dataBltBoxDec(int bullet);
int dataBltBoxChk(int bullet);

/* MenuParaUp (this TU, still assembler): adds one level of the parameter. */
void MenuParaUp(int chrNo, int statIndex);

void MenuCharCameraSet(void);
void MenuCharCameraMove(void);
void MenuCharModelMain(void);
void MenuBattleModelMain(void);
int tskCharLineField(XglTaskPrefix *task);

/*
 * The character menu (status, equipment, parameter points and formation).
 * The first call lays out the sub-screen work blocks and builds the party
 * list; every later call advances the MenuWork.state machine and runs the
 * sub-screens. partyMode is 0 from the main menu and 1 from battle.
 */
void MenuCharactor(int partyMode)
{
    if (MenuWork.state == 0) {
        unsigned char *heap = MainMenuWorkEnd;
        PartyAttackPosition *slot;
        int i;
        int id;

        if (partyMode == 0) {
            ParaDataBuf = (unsigned short *)(((int)heap + 0x7FF) & ~0x7FF);
            heap = (unsigned char *)ParaDataBuf + 0x800;
            xglCdReadFile(D_004C67D0, ParaDataBuf, 0, 1);
        }
        CharPas = (CharPasWork *)heap;
        heap += CHAR_PAS_SIZE;
        CharPas->state = 0;
        CharCategory = (CharCategoryWork *)heap;
        heap += CHAR_CATEGORY_SIZE;
        CharCategory->state = 0;
        CharMenu = (CharMenuWork *)heap;
        heap += CHAR_MENU_SIZE;
        CharMenu->state = 0;
        CharInfo = (CharInfoWork *)heap;
        heap += CHAR_INFO_SIZE;
        CharInfo->state = 0;
        CharStatus = (CharStatusWork *)heap;
        heap += CHAR_STATUS_SIZE;
        CharStatus->window[0].header.state = 0;
        CharParameter = (CharParameterWork *)heap;
        heap += CHAR_PARAMETER_SIZE;
        CharParameter->state = 0;
        CharEx = (CharExWork *)heap;
        heap += CHAR_EX_SIZE;
        CharEx->state = 0;
        CharWeapon = (CharWeaponWork *)heap;
        heap += CHAR_WEAPON_SIZE;
        CharWeapon->state = 0;
        CharList = (CharListWork *)heap;
        heap += CHAR_LIST_SIZE;
        CharList->state = 0;
        CharSwitch = (CharSwitchWork *)heap;
        heap += CHAR_SWITCH_SIZE;
        CharWinSP[0] = &CharList->weapon;
        CharWinSP[1] = (CharListSPWindow *)((unsigned char *)CharList + 0x1C3C);
        CharSwitch->state = 0;
        CharL1R1 = (CharL1R1Work *)heap;
        heap += CHAR_L1R1_SIZE;
        CharL1R1->state = 0;
        CharPoint = (CharPointWork *)heap;
        heap += CHAR_POINT_SIZE;
        CharPoint->state = 0;
        {
            CharSpotLightTask *task;

            task = (CharSpotLightTask *)xglTaskEntryNext(
                MenuTask_XMX, tskCharLineField,
                MenuTask_XMX != 0 ? MenuTask_XMX->active_tail : 0);
            if (task != 0) {
                task->mode = 0;
            }
            task = (CharSpotLightTask *)xglTaskEntryNext(
                MenuTask_XMX, (int (*)(XglTaskPrefix *))tskCharSpotLight,
                MenuTask_XMX != 0 ? MenuTask_XMX->active_tail : 0);
            if (task != 0) {
                task->mode = 0;
            }
        }
        memset(UpPara, 0, sizeof(UpPara));
        MenuWorkEndCheck(heap);
        MenuWork.partyMode = partyMode;
        slot = ((PartyDataPrefix *)PartyDataGet())->attack_slots;
        for (i = 0; i < 3; i++, slot++) {
            id = slot->party_id;
            if (id != 0) {
                MenuCharParty[MenuWork.partyCount] = MenuMaryIdChange(id);
                MenuWork.partyCount++;
            }
        }
        for (id = 1; id < 8; id++) {
            if (PartyFriendLockCheck(id, 1) != 0 && PartyAttackerCheck(id) == 0) {
                if (MenuWork.partyMode == 0) {
                    MenuCharParty[MenuWork.partyCount] = MenuMaryIdChange(id);
                    MenuWork.partyCount++;
                }
                MenuCharPartyReser[MenuWork.reserveCount] = MenuMaryIdChange(id);
                MenuWork.reserveCount++;
            }
        }
        MenuWork.view.chrNo = MenuCharParty[0];
        if (partyMode == 0) {
            MenuWork.state = 0x10;
            MenuModelMemorySet(10);
        } else {
            int n;

            MenuWork.state = 0xA0;
            for (n = 0; n < 3; n++) {
                MenuModelExtFuncSet(MenuModelOut[n], MenuBattleModelMain, 0);
            }
        }
        MenuModelMenuMotionLoad();
        MenuCharCameraSet();
        MenuCharParaSet(MenuWork.view.chrNo, 0);
        MenuKeepSelectReset();
        MenuWork.wait = 12;
    } else {
        if (MenuWork.wait != 0) {
            MenuWork.wait--;
        }
        MenuCharCameraMove();
        switch (MenuWork.state) {
        /* The status screen: pick the character. */
        case 0x10:
            MenuWork.state = 0x11;
        case 0x11:
            if (MenuWork.wait != 0) {
                break;
            }
            if (PadData.repeat & (PAD_UP | PAD_RIGHT | PAD_DOWN | PAD_LEFT)) {
                if (MenuWork.partyCount == 1) {
                    break;
                }
                switch (PadData.repeat) {
                case PAD_UP:
                    MenuWork.partyCursor -= 2;
                    break;
                case PAD_DOWN:
                    MenuWork.partyCursor += 2;
                    break;
                case PAD_LEFT:
                    MenuWork.partyCursor -= 1;
                    break;
                case PAD_RIGHT:
                    MenuWork.partyCursor += 1;
                    break;
                }
                if (MenuWork.partyCursor < 0) {
                    MenuWork.partyCursor += MenuWork.partyCount;
                }
                if (MenuWork.partyCursor >= MenuWork.partyCount) {
                    MenuWork.partyCursor -= MenuWork.partyCount;
                }
                xglSoundEffectNormalID(SE_CURSOR, 0);
                MenuWork.view.chrNo = MenuCharParty[MenuWork.partyCursor];
            }
            if (PadData.pressed & PAD_SQUARE) {
                MenuWork.leaderLocked = MenuCharLeaderMask(MenuWork.view.chrNo);
                if (MenuWork.leaderLocked == 0) {
                    PartyLeaderSet(MenuWork.view.chrNo);
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            }
            if (PadData.pressed & PAD_CIRCLE) {
                if (MenuWork.view.chrNo == CHR_GUEST) {
                    MenuWork.guestDenied = 1;
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                } else {
                    MenuWork.state = 0x20;
                    MenuWork.wait = 24;
                    MenuWork.guestDenied = 0;
                    MenuWork.equipKind = 0;
                    MenuWork.equipSlot = 0;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                }
            }
            if (PadData.pressed & PAD_CROSS) {
                MenuWork.state = 0xF0;
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
            break;

        /* The command menu of the chosen character. */
        case 0x20:
            MenuWork.state = 0x21;
            MenuWork.menuCount = 3;
            MenuCharParaSet(MenuWork.view.chrNo, 0);
        case 0x21:
            if (MenuWork.wait != 0 || (PadData.held & (PAD_L2 | PAD_R2))) {
                break;
            }
            MenuWork.menuCursor = MenuSelectMove(MenuWork.menuCursor, MenuWork.menuCount, 0);
            if (PadData.pressed & (PAD_L1 | PAD_R1)) {
                /* L1/R1 step to the next party member, skipping the guest. The
                 * step is re-entered through its own head: written as a
                 * do-while, the whole tail of the function is allocated
                 * differently from 0x002980c4 on and grows by one
                 * instruction. */
            retry:
                xglSoundEffectNormalID(SE_CURSOR, 0);
                if (PadData.pressed == PAD_L1) {
                    MenuWork.partyCursor--;
                }
                if (PadData.pressed == PAD_R1) {
                    MenuWork.partyCursor++;
                }
                if (MenuWork.partyCursor < 0) {
                    MenuWork.partyCursor = MenuWork.partyCount - 1;
                }
                if (MenuWork.partyCursor > MenuWork.partyCount - 1) {
                    MenuWork.partyCursor = 0;
                }
                MenuWork.view.chrNo = MenuCharParty[MenuWork.partyCursor];
                MenuCharParaSet(MenuWork.view.chrNo, 0);
                if (MenuWork.view.chrNo == CHR_GUEST) {
                    goto retry;
                }
                MenuWork.wait = 4;
                MenuWork.swapOutId = 1;
                MenuWork.modelSlot = 0;
                MenuWork.equipKind = 0;
                MenuWork.equipSlot = 0;
                MenuKeepSelectReset();
            } else if (PadData.pressed & PAD_CROSS) {
                MenuWork.state = 0x10;
                MenuWork.wait = 12;
                MenuKeepSelectReset();
                xglSoundEffectNormalID(SE_CANCEL, 0);
            } else if (PadData.pressed & PAD_CIRCLE) {
                if (MenuModelOut[MenuWork.modelSlot] == 0) {
                    /* Wait for the model; it is created below. */
                } else if (MenuWork.view.chrNo >= 1 && MenuWork.view.chrNo <= 7) {
                    if (MenuWork.menuCursor < 2) {
                        MenuWork.state = 0x40;
                    } else {
                        MenuWork.state = 0x80;
                        MenuWork.listSelect = -1;
                    }
                    MenuWork.wait = 8;
                    MenuWork.accessoryList = 0;
                    if (MenuCursorKeepCheck() == 0) {
                        MenuWork.equipKind = 0;
                        MenuWork.equipSlot = 0;
                    }
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            }
            if (MenuModelOut[MenuWork.modelSlot] == 0) {
                MenuModelCreate(&MenuModelOut[MenuWork.modelSlot], MenuWork.view.chrNo);
                MenuModelExtFuncSet(MenuModelOut[MenuWork.modelSlot], MenuCharModelMain, 0);
            }
            break;

        /* Equipment: pick the slot. */
        case 0x40:
            MenuWork.state = 0x41;
            MenuWork.listSelect = -1;
        case 0x41: {
            CharParaData *org;

            if (MenuWork.wait != 0) {
                break;
            }
            org = func_A191C0(MenuWork.view.chrNo);
            if (PadData.repeat == PAD_UP) {
                if (MenuWork.equipKind == 0) {
                    if (MenuWork.equipSlot == 0) {
                        if (PadData.repeatFirst == PadData.repeat) {
                            MenuWork.equipKind = 1;
                            MenuWork.equipSlot = 2;
                            xglSoundEffectNormalID(SE_CURSOR, 0);
                        }
                    } else {
                        xglSoundEffectNormalID(SE_CURSOR, 0);
                        MenuWork.equipSlot = 0;
                    }
                } else {
                    if (MenuWork.equipSlot == 0) {
                        MenuWork.equipKind = 0;
                        if (MenuBulletCheck2(org->weapon[setSlotTbl[MenuWork.view.chrNo - 1]], MenuWork.view.chrNo) == 0) {
                            MenuWork.equipSlot = 0;
                        } else {
                            MenuWork.equipSlot = 1;
                        }
                    } else {
                        MenuWork.equipSlot--;
                    }
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                }
            }
            if (PadData.repeat == PAD_DOWN) {
                if (MenuWork.equipKind == 0) {
                    if (MenuBulletCheck2(org->weapon[setSlotTbl[MenuWork.view.chrNo - 1]], MenuWork.view.chrNo) != 0) {
                        if (MenuWork.equipSlot == 0) {
                            MenuWork.equipSlot = 1;
                        } else {
                            MenuWork.equipSlot = 0;
                            MenuWork.equipKind = 1;
                        }
                    } else {
                        MenuWork.equipKind = 1;
                    }
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                } else {
                    if (MenuWork.equipSlot == 2) {
                        if (PadData.repeatFirst == PadData.repeat) {
                            MenuWork.equipKind = 0;
                            MenuWork.equipSlot = 0;
                            xglSoundEffectNormalID(SE_CURSOR, 0);
                        }
                    } else {
                        xglSoundEffectNormalID(SE_CURSOR, 0);
                        MenuWork.equipSlot++;
                    }
                }
            }
            if (PadData.repeat == PAD_CIRCLE) {
                if (MenuWork.menuCursor == 1) {
                    if (MenuWork.equipKind == 1) {
                        short accessory = org->accessory[MenuWork.equipSlot];

                        org->accessory[MenuWork.equipSlot] = 0;
                        MenuCharParaSet(MenuWork.view.chrNo, 1);
                        dataAccBoxInc(accessory);
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                    } else {
                        xglSoundEffectNormalID(SE_BUZZER, 0);
                    }
                } else {
                    if (MenuWork.equipKind != 0) {
                        MenuWork.state = 0x52;
                    } else if (MenuWork.equipSlot == 1) {
                        MenuWork.state = 0x54;
                    } else if (MenuWork.view.chrNo != CHR_FIXED_WEAPON) {
                        MenuWork.state = 0x52;
                    }
                    MenuWork.wait = 12;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                }
            }
            if (PadData.repeat == PAD_CROSS) {
                MenuWork.state = 0x20;
                MenuWork.wait = 8;
                xglSoundEffectNormalID(SE_CANCEL, 0);
                if (MenuCursorKeepCheck() == 0) {
                    MenuWork.menuCursor = 0;
                }
            }
            break;
        }

        /* Equipment: pick the weapon or accessory from the list. */
        case 0x52:
            MenuWork.state = 0x53;
            MenuWork.listSelect = -1;
            MenuWork.flags |= 2;
            MenuWork.bulletFits = 0;
            MenuWork.weaponId = 0;
            MenuWork.accessoryId = 0;
        case 0x53: {
            int selected;
            int n;

            if (MenuWork.wait != 0) {
                break;
            }
            if (PadData.pressed == PAD_CIRCLE && MenuWork.listSelect >= 0) {
                CharParaData *org = func_A191C0(MenuWork.view.chrNo);

                if (MenuWork.equipKind == 0) {
                    if (MenuWork.equipSlot == 0) {
                        int weaponSlot = setSlotTbl[MenuWork.view.chrNo - 1];
                        short weapon = org->weapon[weaponSlot];
                        short bullet = org->bullet[weaponSlot];

                        org->weapon[weaponSlot] = MenuWork.weaponId;
                        org->bullet[weaponSlot] = func_A1A3D8(MenuWork.weaponId)->bullet;
                        MenuCharParaSet(MenuWork.view.chrNo, 1);
                        dataWpnBoxDec(MenuWork.weaponId);
                        if (weapon != 0) {
                            dataWpnBoxInc(weapon);
                            if (bullet != 0 && MenuBulletCheck(weapon, -1) != 0) {
                                if (func_A1A3D8(weapon)->bullet == bullet) {
                                    if (dataBltBoxChk(bullet) == 0) {
                                        dataBltBoxInc(bullet);
                                    }
                                } else {
                                    dataBltBoxInc(bullet);
                                }
                            }
                        }
                        if (MenuWork.view.chrNo == CHR_DUAL_WEAPON) {
                            org->weapon[1] = MenuWork.weaponId;
                        }
                        MenuWork.state = 0x40;
                        MenuWork.wait = 12;
                    } else {
                        MenuWork.state = 0x40;
                        MenuWork.wait = 8;
                    }
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    if (MenuAccessoryEquipCheck(MenuWork.view.chrNo, MenuWork.accessoryId, MenuWork.equipSlot) != 0) {
                        short old = org->accessory[MenuWork.equipSlot];

                        org->accessory[MenuWork.equipSlot] = MenuWork.accessoryId;
                        MenuCharParaSet(MenuWork.view.chrNo, 0);
                        dataAccBoxDec(MenuWork.accessoryId);
                        if (old != 0) {
                            dataAccBoxInc(old);
                        }
                        MenuWork.state = 0x40;
                        MenuWork.wait = 8;
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                    } else {
                        xglSoundEffectNormalID(SE_BUZZER, 0);
                    }
                }
            }
            selected = WindowSPSelect(CharWinSP[0], PadData.repeat);
            if (MenuWork.listSelect != selected) {
                CharParaData *org = func_A191C0(MenuWork.view.chrNo);

                if (MenuWork.equipKind == 0) {
                    int weaponSlot = setSlotTbl[MenuWork.view.chrNo - 1];
                    short bullet;
                    short weapon;

                    MenuWork.weaponId = MenuSortGet(0, selected);
                    weapon = org->weapon[weaponSlot];
                    bullet = org->bullet[weaponSlot];
                    org->weapon[weaponSlot] = MenuWork.weaponId;
                    org->bullet[weaponSlot] = func_A1A3D8(MenuWork.weaponId)->bullet;
                    MenuCharParaSet(MenuWork.view.chrNo, 1);
                    org->weapon[weaponSlot] = weapon;
                    org->bullet[weaponSlot] = bullet;
                    if (MenuWork.equipSlot == 0) {
                        MenuWork.weaponChanged = 1;
                    }
                    WindowSPKeepSelect(&CharList->weapon, &MenuKeepSelect[KEEP_WEAPON * 5]);
                } else {
                    MenuWork.accessoryId = MenuSortGet(0, selected);
                    if (MenuAccessoryEquipCheck(MenuWork.view.chrNo, MenuWork.accessoryId, MenuWork.equipSlot) != 0) {
                        short old = org->accessory[MenuWork.equipSlot];

                        org->accessory[MenuWork.equipSlot] = MenuWork.accessoryId;
                        MenuCharParaSet(MenuWork.view.chrNo, 1);
                        org->accessory[MenuWork.equipSlot] = old;
                    } else {
                        MenuCharParaSet(MenuWork.view.chrNo, 0);
                    }
                    WindowSPKeepSelect(&CharList->weapon, &MenuKeepSelect[MenuWork.accessoryList * 5]);
                }
                MenuWork.listSelect = selected;
            }
            if (PadData.pressed == PAD_CROSS) {
                MenuCharParaSet(MenuWork.view.chrNo, 0);
                if (MenuWork.equipKind == 0 && MenuWork.equipSlot == 0) {
                    MenuWork.weaponId = func_A191C0(MenuWork.view.chrNo)->weapon[setSlotTbl[MenuWork.view.chrNo - 1]];
                    MenuWork.weaponChanged = 1;
                }
                xglSoundEffectNormalID(SE_CANCEL, 0);
                MenuWork.state = 0x40;
                MenuWork.wait = 8;
                for (n = 0; n < 8; n++) {
                    WindowSPKeepSelectCheck(&MenuKeepSelect[n * 5]);
                }
            }
            if (MenuWork.equipKind == 1) {
                if (PadData.repeat == PAD_RIGHT) {
                    WindowSPKeepSelectCheck(&MenuKeepSelect[MenuWork.accessoryList * 5]);
                    if (MenuWork.accessoryList == 2) {
                        MenuWork.accessoryList = 0;
                    } else {
                        MenuWork.accessoryList++;
                    }
                    MenuWork.listSelect = -1;
                    CharListMake_Acc();
                    MenuCharParaSet(MenuWork.view.chrNo, 0);
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                }
                if (PadData.repeat == PAD_LEFT) {
                    WindowSPKeepSelectCheck(&MenuKeepSelect[MenuWork.accessoryList * 5]);
                    if (MenuWork.accessoryList == 0) {
                        MenuWork.accessoryList = 2;
                    } else {
                        MenuWork.accessoryList--;
                    }
                    CharListMake_Acc();
                    MenuCharParaSet(MenuWork.view.chrNo, 1);
                    MenuWork.listSelect = -1;
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                }
            }
            break;
        }

        /* Equipment: pick the bullet for the weapon. */
        case 0x54:
            MenuWork.equipSlot = 1;
            MenuWork.listSelect = -1;
            MenuWork.bulletFits = 0;
            MenuWork.bulletId = 0;
            MenuWork.weaponId = func_A191C0(MenuWork.view.chrNo)->weapon[setSlotTbl[MenuWork.view.chrNo - 1]];
            MenuWork.state = 0x55;
        case 0x55: {
            CharParaData *org;
            int selected;

            if (MenuWork.wait != 0) {
                break;
            }
            org = func_A191C0(MenuWork.view.chrNo);
            selected = WindowSPSelect(CharWinSP[0], PadData.repeat);
            WindowSPKeepSelect(&CharList->weapon, &MenuKeepSelect[KEEP_BULLET * 5]);
            if (MenuWork.listSelect != selected) {
                MenuBulletData *bullet;
                int fitsWeapon;

                MenuWork.bulletId = MenuSortGet(0, selected);
                bullet = func_A1A428(MenuWork.bulletId);
                if (bullet != 0) {
                    fitsWeapon = bullet->weapon;
                } else {
                    fitsWeapon = 0;
                }
                if (org->weapon[setSlotTbl[MenuWork.view.chrNo - 1]] == fitsWeapon) {
                    MenuWork.bulletFits = 1;
                } else {
                    MenuWork.bulletFits = 0;
                }
                if (MenuWork.bulletFits != 0) {
                    short old = org->bullet[setSlotTbl[MenuWork.view.chrNo - 1]];

                    org->bullet[setSlotTbl[MenuWork.view.chrNo - 1]] = MenuWork.bulletId;
                    MenuCharParaSet(MenuWork.view.chrNo, 1);
                    org->bullet[setSlotTbl[MenuWork.view.chrNo - 1]] = old;
                } else {
                    MenuCharParaSet(MenuWork.view.chrNo, 0);
                }
                MenuWork.listSelect = selected;
            }
            if ((PadData.pressed & PAD_CIRCLE) && selected >= 0 && MenuWork.bulletFits != 0) {
                short old = org->bullet[setSlotTbl[MenuWork.view.chrNo - 1]];
                short weapon;

                org->bullet[setSlotTbl[MenuWork.view.chrNo - 1]] = MenuWork.bulletId;
                MenuCharParaSet(MenuWork.view.chrNo, 0);
                if (MenuWork.bulletId != 0) {
                    weapon = org->weapon[setSlotTbl[MenuWork.view.chrNo - 1]];
                    if (weapon != 0 && func_A1A3D8(weapon)->bullet != MenuWork.bulletId) {
                        dataBltBoxDec(MenuWork.bulletId);
                    }
                }
                if (old != 0) {
                    weapon = org->weapon[setSlotTbl[MenuWork.view.chrNo - 1]];
                    if (weapon != 0) {
                        if (func_A1A3D8(weapon)->bullet != old || dataBltBoxChk(old) == 0) {
                            dataBltBoxInc(old);
                        }
                    } else {
                        dataBltBoxInc(old);
                    }
                }
                if (MenuWork.view.chrNo == CHR_DUAL_WEAPON) {
                    org->bullet[1] = MenuWork.bulletId;
                }
                MenuWork.state = 0x40;
                MenuWork.wait = 8;
                xglSoundEffectNormalID(SE_DECIDE, 0);
            }
            if (PadData.pressed & PAD_CROSS) {
                MenuCharParaSet(MenuWork.view.chrNo, 0);
                MenuWork.state = 0x40;
                MenuWork.wait = 8;
                xglSoundEffectNormalID(SE_CANCEL, 0);
                WindowSPKeepSelectCheck(&MenuKeepSelect[KEEP_BULLET * 5]);
            }
            break;
        }

        /* Parameter points: pick the parameter. */
        case 0x80:
            MenuWork.state = 0x81;
        case 0x81:
            if (MenuWork.wait == 0) {
                MenuWork.listSelect = WindowSPSelect(CharWinSP[0], PadData.repeat);
                WindowSPKeepSelect(CharWinSP[0], &MenuKeepSelect[KEEP_PARAMETER * 5]);
                if (MenuWork.listSelect >= 0) {
                    MenuWork.paraIndex = MenuWork.listSelect;
                }
                if ((PadData.pressed & PAD_CIRCLE) && MenuWork.listSelect >= 0) {
                    if (MenuParaUpCheck(MenuWork.view.chrNo, MenuWork.paraIndex) == 0) {
                        MenuWork.state = 0x82;
                        MenuWork.wait = 4;
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                    } else {
                        xglSoundEffectNormalID(SE_BUZZER, 0);
                        MenuWork.state = 0x87;
                    }
                }
                if (PadData.pressed & PAD_CROSS) {
                    if (MenuCursorKeepCheck() == 0) {
                        MenuWork.menuCursor = 0;
                    }
                    WindowSPKeepSelectCheck(&MenuKeepSelect[KEEP_PARAMETER * 5]);
                    MenuWork.state = 0x21;
                    MenuWork.wait = 8;
                    xglSoundEffectNormalID(SE_CANCEL, 0);
                }
            } else {
                MenuWork.listSelect = WindowSPSelect(CharWinSP[0], 0);
                if (MenuWork.listSelect >= 0) {
                    MenuWork.paraIndex = MenuWork.listSelect;
                }
            }
            break;

        /* Parameter points: choose how many levels to add. */
        case 0x82:
            memset(UpPara, 0, sizeof(UpPara));
            MenuWork.paraCost = MenuParaNextPointGet(MenuWork.view.chrNo, MenuWork.paraIndex, 0);
            MenuWork.paraUpCount = 1;
            UpPara[MenuWork.paraIndex] = ParaPointTbl[MenuWork.paraIndex];
            if (UpPara[MenuWork.paraIndex] + MenuParaPtNowGet(MenuWork.view.chrNo, MenuWork.paraIndex)
                >= MenuParaUpMaxGet(MenuWork.paraIndex)) {
                UpPara[MenuWork.paraIndex] = MenuParaUpMaxGet(MenuWork.paraIndex)
                                           - MenuParaPtNowGet(MenuWork.view.chrNo, MenuWork.paraIndex);
            }
            MenuWork.state = 0x83;
        case 0x83:
            if (MenuWork.wait != 0) {
                break;
            }
            if (PadData.repeat & PAD_DOWN) {
                if (MenuWork.paraUpCount != 1) {
                    MenuWork.paraUpCount--;
                    MenuWork.paraCost -= MenuParaNextPointGet(MenuWork.view.chrNo, MenuWork.paraIndex, MenuWork.paraUpCount);
                    UpPara[MenuWork.paraIndex] = ParaPointTbl[MenuWork.paraIndex] * MenuWork.paraUpCount;
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            }
            if (PadData.repeat & PAD_UP) {
                CharPointData *unit = func_A19210(MenuWork.view.chrNo);
                int cost = MenuWork.paraCost;
                int next = UpPara[MenuWork.paraIndex] + ParaPointTbl[MenuWork.paraIndex]
                         + MenuParaPtNowGet(MenuWork.view.chrNo, MenuWork.paraIndex);
                int max = MenuParaUpMaxGet(MenuWork.paraIndex);

                next = next / ParaPointTbl[MenuWork.paraIndex] * ParaPointTbl[MenuWork.paraIndex];
                cost += MenuParaNextPointGet(MenuWork.view.chrNo, MenuWork.paraIndex, MenuWork.paraUpCount);
                if (max >= next && unit->points >= cost) {
                    MenuWork.paraCost = cost;
                    MenuWork.paraUpCount++;
                    UpPara[MenuWork.paraIndex] = ParaPointTbl[MenuWork.paraIndex] * MenuWork.paraUpCount;
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
                if (UpPara[MenuWork.paraIndex] + MenuParaPtNowGet(MenuWork.view.chrNo, MenuWork.paraIndex)
                    >= MenuParaUpMaxGet(MenuWork.paraIndex)) {
                    UpPara[MenuWork.paraIndex] = MenuParaUpMaxGet(MenuWork.paraIndex)
                                               - MenuParaPtNowGet(MenuWork.view.chrNo, MenuWork.paraIndex);
                }
            }
            if (PadData.pressed & PAD_CIRCLE) {
                xglSoundEffectNormalID(SE_DECIDE, 0);
                MenuWork.state = 0x84;
                MenuWork.wait = 8;
            }
            if (PadData.pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_CANCEL, 0);
                MenuWork.state = 0x81;
                MenuWork.wait = 4;
                memset(UpPara, 0, sizeof(UpPara));
            }
            break;

        /* Parameter points: confirm. */
        case 0x84:
            MenuWork.state = 0x85;
            MenuWork.confirmCursor = 0;
        case 0x85: {
            int n;

            if (MenuWork.wait != 0) {
                break;
            }
            MenuWork.confirmCursor = MenuSelectMove(MenuWork.confirmCursor, 2, 0);
            if (PadData.pressed & PAD_CIRCLE) {
                if (MenuWork.confirmCursor == 0) {
                    for (n = 0; n < MenuWork.paraUpCount; n++) {
                        MenuParaUp(MenuWork.view.chrNo, MenuWork.paraIndex);
                    }
                    memset(UpPara, 0, sizeof(UpPara));
                    MenuCharParaSet(MenuWork.view.chrNo, 0);
                    MenuWork.state = 0x86;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    MenuWork.state = 0x83;
                    xglSoundEffectNormalID(SE_CANCEL, 0);
                }
            }
            if (PadData.pressed & PAD_CROSS) {
                MenuWork.state = 0x83;
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
            break;
        }

        case 0x86:
            if (MenuWork.wait == 0 && (PadData.pressed & PAD_CIRCLE)) {
                MenuWork.state = 0x81;
                CharListMake_Para();
            }
            break;

        case 0x87:
            if (MenuWork.wait == 0 && PadData.pressed != 0) {
                MenuWork.state = 0x81;
            }
            break;

        /* Formation (from battle): pick the attacker. */
        case 0xA0:
            MenuWork.state = 0xA1;
            MenuWork.view.chrNo = MenuCharParty[MenuWork.partyCursor];
        case 0xA1:
            if (MenuWork.wait == 0) {
                if (PadData.repeat == PAD_CROSS) {
                    MenuWork.state = 0xF0;
                    xglSoundEffectNormalID(SE_CANCEL, 0);
                }
                if (PadData.repeat == PAD_CIRCLE) {
                    if (MenuWork.view.chrNo != 11 && MenuWork.view.chrNo != CHR_GUEST) {
                        MenuWork.state = 0xA2;
                        MenuWork.wait = 8;
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                    } else {
                        xglSoundEffectNormalID(SE_BUZZER, 0);
                    }
                }
                MenuWork.partyCursor = MenuSelectMove(MenuWork.partyCursor, MenuWork.partyCount, 1);
                MenuWork.view.chrNo = MenuCharParty[MenuWork.partyCursor];
            }
            break;

        /* Formation: position or swap. */
        case 0xA2:
            MenuWork.state = 0xA3;
            MenuWork.view.chrNo = MenuCharParty[MenuWork.partyCursor];
        case 0xA3:
            if (MenuWork.wait == 0) {
                MenuWork.commandCursor = MenuSelectMove(MenuWork.commandCursor, 3, 0);
                if (PadData.pressed & PAD_CROSS) {
                    MenuWork.state = 0xA0;
                    MenuWork.wait = 8;
                    xglSoundEffectNormalID(SE_CANCEL, 0);
                }
                if (PadData.pressed & PAD_CIRCLE) {
                    switch (MenuWork.commandCursor) {
                    case 0:
                        MenuWork.formationSlot = ((PartyDataPrefix *)PartyDataGet())
                                                     ->attack_slots[MenuWork.partyCursor].attack_position;
                        MenuWork.formationColumn = (MenuWork.formationSlot - 1) % 3;
                        if (MenuWork.formationSlot < 4) {
                            MenuWork.formationRow = 0;
                        } else {
                            MenuWork.formationRow = 1;
                        }
                        MenuWork.state = 0xB2;
                        MenuWork.wait = 16;
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                        break;
                    case 1:
                        if (MenuWork.reserveCount != 0
                            && PartyFriendLockCheck(MenuWork.view.chrNo, 2) != 0) {
                            MenuWork.state = 0xC2;
                            MenuWork.wait = 8;
                            xglSoundEffectNormalID(SE_DECIDE, 0);
                        } else {
                            xglSoundEffectNormalID(SE_BUZZER, 0);
                        }
                        break;
                    case 2:
                        MenuWork.state = 0xA0;
                        MenuWork.wait = 8;
                        xglSoundEffectNormalID(SE_CANCEL, 0);
                        break;
                    }
                }
            }
            break;

        /* Formation: move the cursor over the six positions. */
        case 0xB2:
            MenuWork.state = 0xB3;
        case 0xB3:
            if (MenuWork.wait == 0) {
                if (PadData.repeat & PAD_DOWN) {
                    if (MenuWork.formationColumn == 0) {
                        MenuWork.formationColumn = 2;
                    } else {
                        MenuWork.formationColumn--;
                    }
                    xglSoundEffectNormalID(SE_SLIDE, 0);
                }
                if (PadData.repeat & PAD_UP) {
                    if (MenuWork.formationColumn == 2) {
                        MenuWork.formationColumn = 0;
                    } else {
                        MenuWork.formationColumn++;
                    }
                    xglSoundEffectNormalID(SE_SLIDE, 0);
                }
                if (PadData.repeat & (PAD_RIGHT | PAD_LEFT)) {
                    MenuWork.formationRow = (MenuWork.formationRow + 1) & 1;
                    xglSoundEffectNormalID(SE_SLIDE, 0);
                }
                MenuWork.formationSlot = MenuWork.formationColumn + MenuWork.formationRow * 3 + 1;
                if (PadData.repeat & PAD_CIRCLE) {
                    int occupant = PartyAttackPosCheck(MenuWork.formationSlot);

                    if (occupant == 0) {
                        MenuWork.state = 0xB6;
                        MenuWork.wait = 4;
                        xglSoundEffectNormalID(SE_PLACE, 0);
                    } else if (occupant != MenuWork.view.chrNo) {
                        MenuWork.state = 0xB4;
                        MenuWork.wait = 4;
                        xglSoundEffectNormalID(SE_PLACE, 0);
                    } else {
                        xglSoundEffectNormalID(SE_BUZZER, 0);
                    }
                }
                if (PadData.repeat & PAD_CROSS) {
                    if (MenuCursorKeepCheck() == 0) {
                        MenuWork.commandCursor = 0;
                    }
                    MenuWork.state = 0xA0;
                    MenuWork.wait = 4;
                    xglSoundEffectNormalID(SE_CANCEL, 0);
                }
            }
            break;

        /* Formation: exchange positions with the occupant. */
        case 0xB4:
            MenuWork.state = 0xB5;
        case 0xB5:
            if (MenuWork.wait == 0) {
                if (PadData.pressed & PAD_CIRCLE) {
                    PartyAttackPosChange(MenuWork.view.chrNo, PartyAttackPosCheck(MenuWork.formationSlot));
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                    MenuWork.state = 0xBF;
                    MenuWork.wait = 8;
                }
                if (PadData.pressed & PAD_CROSS) {
                    MenuWork.state = 0xB2;
                    MenuWork.wait = 4;
                    xglSoundEffectNormalID(SE_CANCEL, 0);
                }
            }
            break;

        /* Formation: move to the empty position. */
        case 0xB6:
            if (MenuWork.wait == 0) {
                if (PadData.pressed & PAD_CIRCLE) {
                    PartyAttackPosSet(MenuWork.view.chrNo, MenuWork.formationSlot);
                    MenuWork.wait = 8;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                    MenuWork.state = 0xBF;
                }
                if (PadData.pressed & PAD_CROSS) {
                    xglSoundEffectNormalID(SE_CANCEL, 0);
                    MenuWork.state = 0xB2;
                }
            }
            break;

        case 0xBF:
            if (MenuWork.wait == 0) {
                MenuWork.state = 0xA2;
                MenuWork.wait = 8;
            }
            break;

        /* Formation: swap the attacker with a reserve friend. */
        case 0xC2:
            MenuWork.state = 0xC3;
            if (MenuCursorKeepCheck() == 0) {
                MenuWork.reserveCursor = 0;
            }
        case 0xC3: {
            if (MenuWork.wait != 0) {
                break;
            }
            if (PadData.repeat & PAD_UP) {
                if (MenuWork.reserveCursor == 0) {
                    MenuWork.reserveCursor = MenuWork.reserveCount - 1;
                } else {
                    MenuWork.reserveCursor--;
                }
                xglSoundEffectNormalID(SE_CURSOR, 0);
            }
            if (PadData.repeat & PAD_DOWN) {
                if (MenuWork.reserveCursor == MenuWork.reserveCount - 1) {
                    MenuWork.reserveCursor = 0;
                } else {
                    MenuWork.reserveCursor++;
                }
                xglSoundEffectNormalID(SE_CURSOR, 0);
            }
            if (PadData.pressed & PAD_CROSS) {
                if (MenuCursorKeepCheck() == 0) {
                    MenuWork.commandCursor = 0;
                }
                MenuWork.state = 0xA2;
                MenuWork.wait = 4;
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
            if (PadData.pressed & PAD_CIRCLE) {
                /* The slot's byte offset in the party state (4-byte slots
                 * from +0x30) is computed as an unsigned integer before it
                 * is added to the pointer (0x00299a18..0x00299a24). */
                PartyAttackPosition *slot = (PartyAttackPosition *)(PartyDataGet()
                    + (unsigned int)(MenuWork.partyCursor * 4 + 0x30));
                short inId = MenuCharPartyReser[MenuWork.reserveCursor];
                unsigned short outId = slot->party_id;
                unsigned short *attacker;
                unsigned short *reserve;
                unsigned short swap;
                int attackers[8];
                int n;

                slot->party_id = inId;
                if (PartyLeaderCheck(outId) != 0) {
                    PartyLeaderSet(inId);
                }
                attacker = &MenuCharParty[MenuWork.partyCursor];
                reserve = &MenuCharPartyReser[MenuWork.reserveCursor];
                swap = *attacker;
                *attacker = *reserve;
                *reserve = swap;
                MenuWork.swapOutId = outId;
                MenuWork.swapInId = inId;
                MenuWork.partyCount = PartyAttackerGet(attackers);
                for (n = 0; n < MenuWork.partyCount; n++) {
                    MenuCharParty[n] = attackers[n];
                }
                xglSoundEffectNormalID(SE_SWAP, 0);
                MenuWork.state = 0xC4;
                MenuWork.wait = 8;
            }
            break;
        }

        /* Formation: rebuild the swapped-in character's model. */
        case 0xC4:
            if (MenuWork.wait != 0 || MenuModelOut[MenuWork.partyCursor] != 0) {
                break;
            }
            MenuModelCreate(&MenuModelOut[MenuWork.partyCursor], MenuWork.swapInId);
            MenuModelExtFuncSet(MenuModelOut[MenuWork.partyCursor], MenuBattleModelMain, 0);
            MenuWork.wait = 1;
            MenuWork.state = 0xC5;
        case 0xC5:
            if (MenuWork.wait == 0) {
                MenuWork.state = 0xA2;
                MenuWork.wait = 4;
            }
            break;

        case 0xF0:
            MenuWork.state = 0xF1;
            MenuWork.wait = 12;
        case 0xF1:
            if (MenuWork.wait == 0) {
                MenuWork.state = MENU_WORK_END;
            }
            break;

        case MENU_WORK_END:
            ChangeTopLevel(0);
            MenuModelAllBreak();
            break;
        }
    }
    CharPasMain();
    CharInfoMain();
    CharPointMain();
    CharMenuMain();
    CharParameterMain();
    CharListMain();
    CharStatusMain();
    CharExMain();
    CharWeaponMain();
    CharSwitchMain();
    CharCategoryMain();
    CharL1R1Main();
    xglFontDebugPrintf(0, 0x40, D_004C67F0);
    xglFontDebugHex(0, 0x48, MenuWork.state, 2);
    xglFontDebugHex(0x10, 0x48, MenuWork.partyMode, 2);
    xglFontDebugHex(0, 0x38, MenuWork.accessoryList, 2);
    xglFontDebugHex(0, 0x50, MenuWork.listSelect, 2);
    xglFontDebugHex(0, 0x58, MenuWork.partyCursor, 2);
    xglFontDebugHex(0, 0x60, MenuWork.formationColumn, 2);
    xglFontDebugHex(0, 0x68, MenuWork.formationRow, 2);
    xglFontDebugHex(0, 0x70, MenuWork.formationSlot, 2);
    xglFontDebugHex(0, 0x78, MenuWork.unknown_47, 2);
    xglFontDebugHex(0, 0x88, MenuWork.reserveCount, 2);
    xglFontDebugHex(0, 0x90, MenuWork.reserveCursor, 2);
    xglFontDebugHex(0x40, 0x88, MenuWork.equipKind, 2);
    xglFontDebugHex(0x40, 0x90, MenuWork.equipSlot, 2);
    xglFontDebugHex(0, 0x98, MenuWork.unknown_52[0], 2);
    xglFontDebugHex(0, 0xA0, MenuWork.unknown_52[1], 2);
    xglFontDebugHex(0, 0xA8, MenuWork.unknown_52[2], 2);
    xglFontDebugHex(0, 0xB0, MenuWork.unknown_52[3], 2);
    xglFontDebugHex(0, 0xC0, MenuWork.wait, 2);
    xglFontDebugHex(0, 0xC8, MenuWork.unknown_4b[0], 2);
    xglFontDebugHex(0, 0xD0, MenuWork.unknown_4b[1], 2);
    xglFontDebugHex(0, 0xD8, MenuWork.partyCount, 2);
    xglFontDebugHex(0x20, 0xB0, MenuWork.weaponChanged, 8);
}
