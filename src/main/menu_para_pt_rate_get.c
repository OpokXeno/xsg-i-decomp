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
typedef struct MenuWorkData {
    unsigned char unmodeled_00[0x03];
    unsigned char state;            /* +0x03 */
    unsigned char unmodeled_04[0x31 - 0x04];
    signed char partyMode;          /* +0x31 */
    unsigned char unmodeled_32[0x46 - 0x32];
    signed char formationSlot;      /* +0x46 */
    unsigned char unmodeled_47[0x80 - 0x47];
} MenuWorkData;

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
typedef struct CharParaData {
    short maxHp;                /* +0x00 */
    short maxEp;                /* +0x02 */
    unsigned short attack;      /* +0x04 */
    unsigned short phyDefense;  /* +0x06 */
    unsigned char unmodeled_08[0x0A - 0x08];
    unsigned short magDefense;  /* +0x0A */
    unsigned char unmodeled_0c[0x34 - 0x0C];
    short hp;                   /* +0x34 */
    short ep;                   /* +0x36 */
    unsigned char unmodeled_38[0x64 - 0x38];
    short accessory[3];         /* +0x64 */
    unsigned char unmodeled_6a[0x180 - 0x6A];
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

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", MenuParaPtNowGet);

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", MenuParaUpMaxGet);

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", MenuParaNextPointGet);

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", MenuParaUp);

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", MenuParaUpCheck);

int MenuCharWeaponAttCheck(void) {
    return 0;
}

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", MenuCharLeaderMask);

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", MenuPasLengthGet);

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", CharPasMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", MenuCharEquipCalcPointGet);

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", CharInfoMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", CharCategoryMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", CharMenuMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", CharExMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", CharL1R1Main);

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", CharStatusMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", CharParameterMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", CharWeaponMain);

/* MenuListEntry is defined in src/main/menu_1.c (not yet published); used
 * here only as an opaque pointer, matching that TU's MenuListMake/
 * MenuListGet definitions exactly. */
typedef struct MenuListEntry MenuListEntry;

MenuListEntry *MenuListMake(int listIndex, int mode);
MenuListEntry *MenuListGet(int listIndex);

/* MenuSortSet, WindowSPItemChange and WindowSPSetSelect are not yet
 * recovered (main/menu_1.c and main/e_battle_win_open.c, still
 * assembler). */
void MenuSortSet(int listIndex, int type, int order);

/*
 * CharListSPWindow is the scrollable list window CharListMake_Wpn builds at
 * CharList+0x504 and hands to WindowSPItemChange/WindowSPSetSelect. Its
 * +0x14/+0x15 row/column grid and +0x1C item list match the fields those
 * still-assembler functions read at the same offsets for every WindowSP*
 * list they manage (main/e_battle_win_open.c). Only the members this call
 * writes are modeled; CharListMake_Gun/_Acc/_Para (still assembler) build
 * the same kind of window at other offsets from CharList.
 */
typedef struct CharListSPWindow {
    unsigned char unmodeled_00[0x01];
    unsigned char rowCount;    /* +0x01 */
    unsigned char unmodeled_02[0x0C - 0x02];
    short width;                /* +0x0C */
    short height;                /* +0x0E */
    int flags;                  /* +0x10 */
    unsigned char columns;      /* +0x14 */
    unsigned char rows;         /* +0x15 */
    unsigned char unmodeled_16[0x1C - 0x16];
    MenuListEntry *items;       /* +0x1C */
} CharListSPWindow;

void WindowSPItemChange(CharListSPWindow *window);
void WindowSPSetSelect(CharListSPWindow *window, unsigned char *savedSelection);

/*
 * CharListWork is the equip-list screen's work block; only the weapon
 * sub-window (+0x504) this allocation touches is modeled.
 */
typedef struct CharListWork {
    unsigned char unmodeled_00[0x504];
    CharListSPWindow weapon;   /* +0x504 */
} CharListWork;

extern CharListWork *CharList;

/* Saved WindowSP selections, one 5-byte snapshot per list; index 15 = the
 * 4th slot, this screen's weapon list. */
extern unsigned char MenuKeepSelect[0x64];

void CharListMake_Wpn(void)
{
    CharListSPWindow *window = &CharList->weapon;

    /* MenuWork+0x60 (short): not yet modeled in MenuWorkData (published
     * struct, additive only); read here as MenuSortSet's order argument. */
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

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", CharListMake_Gun);

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", CharListMake_Acc);

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", CharListMake_Para);

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", CharListMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", CharSwitchMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", CharPointMain);

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

INCLUDE_ASM("asm/main/nonmatchings/menu_para_pt_rate_get", MenuCharactor);
