#include "common.h"

/*
 * The retail ELF names these ov01 data-table accessors (still assembler in
 * their own TU, src/ov01/data_unit_org_get.c) dataEthGet (0x00a1a488) and
 * dataTecGet (0x00a1a378); until that TU recovers them, the synthetic
 * linker script only binds their scaffold func_<VA> spelling.
 */

typedef struct EtherStatus {
    unsigned char unmodeled_00[2];
    short flags; /* +2; tested as the 0x4000 acquisition bit */
    short capCost;        /* +4 */
    short tecId;          /* +6 */
    short pointCost;      /* +8 */
} EtherStatus;

typedef struct TecStatus {
    unsigned char unmodeled_00;
    signed char epCost;   /* +1 */
    unsigned char unmodeled_02[2];
    signed char type;     /* +4 */
} TecStatus;

extern EtherStatus *func_A1A488(int etherId);

/* dataEthGet */

extern TecStatus *func_A1A378(short tecId);

/* dataTecGet */

#include "shared.h"

typedef struct EtherUnitRecord {
    unsigned char unmodeled_00[0x17];
    signed char capUsed; /* +0x17 */
    unsigned char unmodeled_18[0x1E];
    short ep; /* +0x36 */
    unsigned char unmodeled_38[0x56];
    short etherId[12]; /* +0x8E */
} EtherUnitRecord;

typedef struct EtherCharRecord {
    unsigned char unmodeled_00[0x10];
    int points; /* +0x10 */
    unsigned char unmodeled_14[0x14];
    unsigned char etherMask[10]; /* +0x28 */
} EtherCharRecord;

typedef struct MenuEtherWork {
    unsigned char unmodeled_00[0x03];
    unsigned char state;            /* +0x03 */
    unsigned char unmodeled_04[0x10 - 0x04];
    signed char charDenied;         /* +0x10 */
    unsigned char unmodeled_11[0x20 - 0x11];
    unsigned char flags;            /* +0x20; bit 0 rebuilds ether windows */
    unsigned char nextState;        /* +0x21 */
    unsigned char unmodeled_22[0x2A - 0x22];
    unsigned char wait;             /* +0x2A */
    unsigned char unmodeled_2b[0x30 - 0x2B];
    signed char commandCursor;      /* +0x30 */
    signed char menuCursor;         /* +0x31 */
    signed char slotCursor;         /* +0x32 */
    signed char slotCount;          /* +0x33 */
    signed char confirmCursor;      /* +0x34 */
    unsigned char unmodeled_35[0x40 - 0x35];
    signed char chrNo;              /* +0x40 */
    signed char partyCount;         /* +0x41 */
    signed char partyCursor;        /* +0x42 */
    signed char targetCursor;       /* +0x43 */
    signed char etherId;            /* +0x44 */
    signed char slotEther;          /* +0x45 */
    signed char targetChrNo;        /* +0x46 */
    signed char useType;            /* +0x47 */
    signed char targetCount;        /* +0x48 */
    unsigned char unmodeled_49[0x50 - 0x49];
    int listSelect;                 /* +0x50 */
    unsigned char unmodeled_54[0x60 - 0x54];
    unsigned char targetList[8];    /* +0x60 */
} MenuEtherWork;

extern MenuEtherWork MenuWork;

typedef struct EtherListRow {
    const char *name;   /* +0x00 */
    int amount;         /* +0x04 */
    signed char flag;   /* +0x08 */
    signed char status; /* +0x09 */
    unsigned char unmodeled_0a[2];
} EtherListRow;

typedef struct MenuEtherPadData {
    unsigned char unmodeled_00[0x2A];
    unsigned short pressed; /* +0x2A */
    unsigned char unmodeled_2c[0x34 - 0x2C];
    unsigned short repeat;  /* +0x34 */
} MenuEtherPadData;

typedef struct EtherTextEntry {
    const char *name;
} EtherTextEntry;

extern EtherUnitRecord *func_A191C0(int chrNo);

extern EtherCharRecord *func_A19210(int chrNo);

EtherListRow *MenuListGet(int listIndex);

extern void MenuListMake(int listIndex, int mode);

extern int *MenuSortAddrGet(int listIndex);

int MenuSortCheck(int listIndex);

extern EtherTextEntry *MenuTextGet(int index);

int MenuEtherUseCheck(short etherId);

int func_00A11220(int chrNo, int etherId);

extern MenuEtherPadData PadData;

#define PAD_L1 0x0004

#define PAD_R1 0x0008

extern unsigned char (*MenuEtherDataBuf)[4];

typedef struct EtherL1R1SpriteIdPair {
    short id[2];
} EtherL1R1SpriteIdPair;

typedef struct EtherL1R1SlideStep {
    signed char side[2];
} EtherL1R1SlideStep;

extern const EtherL1R1SpriteIdPair D_004DB2E8[];

extern const EtherL1R1SlideStep D_004DB2F0[];

const char D_004C8870[32] = "data\\endou\\ether\\etree.bin";

const char D_004C7CD0[48] =
    "Choose a character.\0\0\0\0\0 cannot be chosen.\0\0\0\0\0";

const char *D_004DB278[2] = { D_004C7CD0, D_004C7CD0 + 24 };

char D_004DB280[8] = "Cancel.";

char D_004DB288[8] = ".";

char D_004DB290[8] = " to ";

char D_004DB298[8] = { 0 };

char D_004DB2A0[8] = ".\n";

char D_004DB2A8[8] = ". \x1f";

char D_004DB2B0[8] = "of ";

char D_004DB2B8[8] = "?";

char D_004DB2C0[8] = "Yes\nNo";

extern char D_004DB2C8[];

extern char D_004DB2D0[];

char D_004DB2D8[8] = "\x01Wt";

char D_004DB2E0[8] = "\x01" "E.Pts";

char D_004DB2F8[8] = "Result";

const char D_004C8290[] = "Evolve\nTransfer\nCancel";

const char D_004C82A8[] = "Use\nSet\nUse E.Pts\nCancel";

const char D_004C82C8[] = " Is this okay?";

/* The four-byte record of an ether (its slot ethers), or 0 outside 1..80. */

extern void MoveSlide(short *current, short *target, float rate);

extern void eSpriteSet(void *sprite, short spriteId);

extern void eSpriteMain(void *sprite);

extern void WindowDXSet(void *window);

extern void WindowDXMain(void *window);

extern void MenuSelectWindow();

extern void MenuPasWindow(void);

extern int MenuPasLengthGet(const char *text);

extern void endPrintExtFunc(int kind, int id, void *data);

extern void eMessageSet(void *message, const char *text);

extern void eMessageMain(void *message);

typedef struct EtherPasWindow {
    short x; /* +0x00 */
    short y; /* +0x02 */
    int color; /* +0x04 */
    short width; /* +0x08 */
    short height; /* +0x0A */
    unsigned char unmodeled_0c[0x10 - 0x0C];
    signed char state; /* +0x10 */
    unsigned char unmodeled_11[0x14 - 0x11];
    void (*callback)(void); /* +0x14 */
    void *callbackArg; /* +0x18 */
    unsigned char unmodeled_1c[0x194 - 0x1C];
} EtherPasWindow;

typedef struct EtherPasMessage {
    unsigned char unmodeled_00;
    unsigned char mode; /* +0x01 */
    unsigned char unmodeled_02[0x04 - 0x02];
    short x; /* +0x04 */
    short y; /* +0x06 */
    int color; /* +0x08 */
    unsigned char unmodeled_0c[0x44 - 0x0C];
} EtherPasMessage;

typedef struct EtherPasBox {
    short x; /* +0x00 */
    short y; /* +0x02 */
    int kind; /* +0x04 */
    short width; /* +0x08 */
    short height; /* +0x0A */
} EtherPasBox;

typedef struct EtherPasWork {
    unsigned char state; /* +0x000 */
    unsigned char unmodeled_01[3];
    int kind; /* +0x004 */
    EtherPasWindow window; /* +0x008 */
    EtherPasMessage messages[6]; /* +0x19C */
    EtherPasBox box; /* +0x334 */
    unsigned char callbackWork[1]; /* +0x340 */
} EtherPasWork;

EtherPasWork *MenuEtherPas = 0;

static const char *msg_0_0036DC00[6] = {
    "Ether",
    "/Use",
    "/Set",
    "/Use E.Pts",
    "/Evolve",
    "/Transfer"
};

typedef struct EtherMenuWindowDX {
    short x; /* +0x00 */
    short y; /* +0x02 */
    int color; /* +0x04 */
    short width; /* +0x08 */
    short height; /* +0x0A */
    const char *title; /* +0x0C */
    unsigned char state; /* +0x10: 1 while the frame opens, 3 once it is up */
    unsigned char unmodeled_11[0x14 - 0x11];
    void *select; /* +0x14 */
    void *selectArg; /* +0x18 */
    unsigned char unmodeled_1c[0x194 - 0x1C];
} EtherMenuWindowDX;

typedef struct EtherMenuList {
    unsigned char unmodeled_00[0x02];
    unsigned short hasPrompt; /* +0x02 */
    int cursor; /* +0x04 */
    const char *items; /* +0x08 */
    const char *prompt; /* +0x0C */
    unsigned char unmodeled_10[0x5F8 - 0x10];
} EtherMenuList;

typedef struct EtherMenuScreen {
    unsigned char state; /* +0x000 */
    unsigned char unmodeled_01[3];
    int color; /* +0x004 */
    EtherMenuWindowDX window[2]; /* +0x008 */
    EtherMenuList list[2]; /* +0x330 */
} EtherMenuScreen;

EtherMenuScreen *MenuEtherMenu = 0;

extern char *msg00_1_0036DC18[];

extern char *msg01_2_0036DC20[];

typedef struct EtherL1R1Sprite {
    unsigned char unmodeled_00[0x04];
    short x; /* +0x04 */
    short y; /* +0x06 */
    int work; /* +0x08 */
    unsigned char unmodeled_0c[0x28 - 0x0c];
} EtherL1R1Sprite;

typedef struct EtherL1R1Work {
    unsigned char state; /* +0x00 */
    unsigned char unmodeled_01;
    signed char slideOffset[2]; /* +0x02: added into sprite[i].x while non-zero */
    int spriteWork; /* +0x04: initial work value of both sprites */
    EtherL1R1Sprite sprite[2]; /* +0x08: L1, then R1 */
} EtherL1R1Work;

EtherL1R1Work *MenuEtherL1R1 = 0;

#define MENU_STATE_CHANGED 0x01

#define PAD_CIRCLE   0x0020

#define PAD_CROSS    0x0040

#define PAD_SQUARE   0x0080

#define PAD_UP       0x1000

#define PAD_RIGHT    0x2000

#define PAD_DOWN     0x4000

#define PAD_LEFT     0x8000

#define SE_DECIDE 1

#define SE_CANCEL 2

#define SE_CURSOR 3

#define SE_BUZZER 5

typedef struct EtherScreenWork {
    unsigned char state;
} EtherScreenWork;

typedef struct EtherListSPWindow {
    unsigned char unmodeled_00[0x26];
    unsigned char state;            /* +0x26 */
} EtherListSPWindow;

typedef struct EtherListWork {
    unsigned char state;
    unsigned char unmodeled_01[0x0C - 0x01];
    EtherListSPWindow window;       /* +0x0C */
} EtherListWork;

#define ETHER_DATA_SIZE    0x800

#define ETHER_PAS_SIZE     0x3B0

#define ETHER_INFO_SIZE    0x450

#define ETHER_STATUS_SIZE  0x4EF4

#define ETHER_STATUS2_SIZE 0x4F64

#define ETHER_MENU_SIZE    0xF20

#define ETHER_LIST_SIZE    0x343C

#define ETHER_LIST2_SIZE   0x43C

#define ETHER_EX_SIZE      0x28C

#define ETHER_L1R1_SIZE    0x58

EtherScreenWork *MenuEtherInfo = 0;

EtherScreenWork *MenuEtherEx = 0;

EtherScreenWork *MenuEtherStatus = 0;

EtherScreenWork *MenuEtherStatus2 = 0;

EtherListWork *MenuEtherList = 0;

EtherScreenWork *MenuEtherList2 = 0;

extern short MenuEtherParty[16];

extern int MainMenuWorkEnd;

extern unsigned char MenuKeepSelect[0x64];

typedef struct EtherPartySlot {
    unsigned short party_id;
    unsigned char unmodeled_02[0x02];
} EtherPartySlot;

typedef struct EtherPartyData {
    unsigned char unmodeled_00[0x30];
    EtherPartySlot attack_slots[3]; /* +0x30 */
} EtherPartyData;

void func_A194E0(int chrNo, int etherId);

int func_A19578(int chrNo, int etherId);

void func_00A11270(int chrNo, int targetChrNo, int etherId);

int EtherTreeInit();

void EtherTreeMain(void);

void MenuEtherCapSet(void);

void MenuEtherPasMain(void);

void MenuEtherInfoMain(void);

void MenuEtherMenuMain(void);

void MenuEtherExMain(void);

void MenuEtherL1R1Main(void);

void MenuEtherStatusMain(void);

void MenuEtherStatusMain2(void);

void MenuEtherListMain(void);

void MenuEtherListMain2(void);

void MenuEtherListMake00(void);

void MenuEtherListMake01(void);

void MenuEtherListMake02(int mode);

void MenuEtherListMake03(int mode);

void MenuEtherEquip(void);

int xglCdReadFile(const char *name, void *buffer, int mode, int flags);

void xglSoundEffectNormalID(int sound_id, int variant);

void xglFontDebugPrintf(int x, int y, const char *format, ...);

unsigned char *PartyDataGet(void);

int PartyFriendLockCheck(int id, int flag);

int PartyAttackerCheck(int id);

int MenuMaryIdChange(int id);

int MenuMainCharCheck(int chrNo);

int MenuCharHpCheck(int chrNo);

void MenuWorkEndCheck(unsigned char *end);

void MenuKeepSelectReset(void);

int MenuCursorKeepCheck(void);

int MenuSelectMove(int cursor, int count, int wrap);

void MenuSortSet(int listIndex, int type, int order);

int MenuSortGet(int listIndex, int index);

int WindowSPSelect(EtherListSPWindow *window, int pad);

void WindowSPSelectJump(EtherListSPWindow *window, int index);

void WindowSPKeepSelect(EtherListSPWindow *window, unsigned char *keep);

void WindowSPKeepSelectCheck(unsigned char *keep);

void WindowSPItemChange(EtherListSPWindow *window);

void ChangeTopLevel(int level);

/*
 * The ether menu. The first call loads etree.bin, lays out the sub-screen
 * work blocks after it and builds the party list; every later call advances
 * the MenuWork.state machine and runs the sub-screens.
 */

unsigned char *MenuEtherDataGet(int etherId)
{
    unsigned char (*records)[4] = MenuEtherDataBuf;

    etherId &= 0xFFFF;
    if (etherId - 1U >= 80) {
        return 0;
    }
    return records[etherId - 1];
}

int MenuEtherJoutoCheck(int etherId) {
    etherId &= 0xFFFF;
    if ((unsigned int)(etherId - 1) >= 0x50U) {
        return 0;
    }
    return (func_A1A488(etherId)->flags & 0x4000) < 1;
}

int MenuEtherCharPointCheck(unsigned short chrNo, unsigned short etherId) {
    int etherPoints = func_A19210(chrNo)->points;

    if (etherPoints < func_A1A488(etherId)->pointCost) {
        return 0;
    }
    return 1;
}

int MenuEtherWhoCheck(int etherId) {
    int checkedEtherId = etherId & 0xFFFF;

    if ((unsigned int)(checkedEtherId - 1) < 0x10U) {
        return 3;
    }
    if ((unsigned int)(checkedEtherId - 0x11) < 0xAU) {
        return 2;
    }
    if ((unsigned int)(checkedEtherId - 0x1B) < 0xAU) {
        return 1;
    }
    if ((unsigned int)(checkedEtherId - 0x25) < 0xCU) {
        return 6;
    }
    if ((unsigned int)(checkedEtherId - 0x31) < 0xAU) {
        return 7;
    }
    if ((unsigned int)(checkedEtherId - 0x3B) < 0xDU) {
        return 4;
    }
    if ((unsigned int)(checkedEtherId - 0x48) < 8U) {
        return 5;
    }
    return 0;
}

int MenuEtherSetCheck(unsigned short chrNo, unsigned short etherId) {
    EtherUnitRecord *character = func_A191C0(chrNo);
    int i;

    for (i = 0; i < 12; i++) {
        if (character->etherId[i] == etherId) {
            return 1;
        }
    }
    return 0;
}

signed char MenuEtherTypeGet(int etherId) {
    return func_A1A378(func_A1A488((unsigned short)etherId)->tecId)->type;
}

int MenuEtherUseCheck(short etherId) {
    switch ((unsigned short)etherId) {
    case 1:
    case 8:
    case 0x25:
        return 1;
    case 4:
    case 0x1C:
        return 2;
    case 0x4A:
        return 3;
    default:
        return 0;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/menu_ether", MenuEtherCapSet);

void MenuEtherPasMain(void) {
    EtherPasWork *pas = MenuEtherPas;
    int tabLength[6];
    short tabTarget[6];
    short slideTarget;
    int i;

    for (i = 0; i < 6; i++) {
        tabLength[i] = MenuPasLengthGet(msg_0_0036DC00[i]);
    }

    switch (pas->state) {
    case 0:
        pas->kind = 0x00FFFFF0;
        WindowDXSet(&pas->window);
        pas->window.x = -288;
        pas->window.y = 8;
        pas->window.color = pas->kind;
        pas->window.width = 272;
        pas->window.height = 30;
        pas->window.callback = MenuPasWindow;
        pas->window.callbackArg = pas->callbackWork;
        pas->window.state = 1;
        WindowDXMain(&pas->window);
        pas->window.state = 3;
        for (i = 0; i < 6; i++) {
            eMessageSet(&pas->messages[i], msg_0_0036DC00[i]);
            pas->messages[i].mode = 0x20;
            pas->messages[i].x = 288;
            pas->messages[i].y = 11;
            pas->messages[i].color = pas->kind + 2;
        }
        pas->state = 2;
        break;
    case 2:
        break;
    default:
        return;
    }

    slideTarget = -16;
    for (i = 0; i < 6; i++) {
        tabTarget[i] = 288;
    }

    switch (MenuWork.state) {
    case 16:
    case 32:
        tabTarget[0] = 16;
        break;
    case 48:
    case 50:
    case 80:
    case 112:
    case 114:
        tabTarget[0] = 16;
        tabTarget[MenuWork.commandCursor + 1] = tabLength[0] + 16;
        break;
    case 116:
    case 118:
    case 120:
    case 128:
    case 130:
    case 132:
    case 134:
        tabTarget[0] = -(tabLength[0] + 16);
        tabTarget[MenuWork.commandCursor + 1] = 16;
        tabTarget[MenuWork.menuCursor + 4] = tabLength[MenuWork.commandCursor + 1] + 16;
        break;
    default:
        slideTarget = -288;
        break;
    }

    MoveSlide(&pas->window.x, &slideTarget, 3.0f);
    WindowDXMain(&pas->window);
    if (pas->window.state == 3) {
        pas->box.x = pas->window.x + 3;
        pas->box.y = pas->window.y + 3;
        pas->box.kind = pas->kind;
        pas->box.width = pas->window.width - 6;
        pas->box.height = pas->window.height - 6;
        endPrintExtFunc(pas->kind, 101, &pas->box);
        for (i = 0; i < 6; i++) {
            MoveSlide(&pas->messages[i].x, &tabTarget[i], 5.0f);
            if (pas->messages[i].x < 256) {
                eMessageMain(&pas->messages[i]);
            }
        }
        endPrintExtFunc(pas->kind, 102, 0);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/menu_ether", MenuEtherInfoMain);

void MenuEtherMenuMain(void) {
    EtherMenuScreen *work = MenuEtherMenu;
    short target[2];
    int i;

    switch (work->state) {
    case 0:
        work->color = 0x00FFFFF0;
        WindowDXSet(&work->window[0]);
        work->window[0].x = 528;
        work->window[0].title = D_004DB2C8;
        work->window[0].color = work->color;
        work->window[0].width = 145;
        work->window[0].selectArg = &work->list[0];
        work->list[0].items = msg00_1_0036DC18[0];
        work->window[0].state = 1;
        work->window[0].y = 192;
        work->window[0].height = 102;
        work->window[0].select = MenuSelectWindow;
        work->list[0].cursor = 0;
        work->list[0].hasPrompt = 0;
        WindowDXMain(&work->window[0]);
        work->window[0].state = 3;
        WindowDXSet(&work->window[1]);
        work->window[1].x = 528;
        work->window[1].title = D_004DB2D0;
        work->window[1].y = 192;
        work->window[1].color = work->color;
        work->window[1].width = 169;
        work->list[1].items = msg01_2_0036DC20[0];
        work->list[1].prompt = msg01_2_0036DC20[1];
        work->window[1].height = 102;
        work->window[1].select = MenuSelectWindow;
        work->window[1].selectArg = &work->list[1];
        work->list[1].cursor = 0;
        work->list[1].cursor = 0;
        work->window[1].state = work->list[1].hasPrompt = 1;
        WindowDXMain(&work->window[1]);
        work->window[1].state = 3;
        work->state = 2;
	    /* fallthrough */
    case 2:
        target[1] = 528;
        target[0] = 528;
        switch (MenuWork.state) {
        case 32:
            if ((unsigned char)(MenuWork.flags & 1)) {
                work->window[0].y = 192;
                work->window[0].x = 528;
                work->window[0].width = 145;
                work->window[0].height = 102;
                work->list[0].items = msg00_1_0036DC18[0];
                work->list[0].cursor = 0;
            }
            target[0] = 192;
            work->list[0].cursor = MenuWork.commandCursor;
            break;
        case 114:
            if ((unsigned char)(MenuWork.flags & 1)) {
                work->window[0].x = 528;
                work->window[0].y = 288;
                work->window[0].width = 125;
                work->window[0].height = 78;
                work->list[0].items = msg00_1_0036DC18[1];
                work->list[0].cursor = 0;
            }
            target[0] = 131;
            work->list[0].cursor = MenuWork.menuCursor;
            break;
        case 118:
        case 132:
            if ((unsigned char)(MenuWork.flags & 1)) {
                if (MenuWork.state == 118) {
                    work->window[1].y = 128;
                } else {
                    work->window[1].y = 240;
                }
                work->list[1].cursor = 0;
                work->window[1].x = 528;
            }
            target[1] = 285;
            work->list[1].cursor = MenuWork.confirmCursor;
            break;
        }
        for (i = 0; i < 2; i++) {
            MoveSlide(&work->window[i].x, &target[i], 3.0f);
            WindowDXMain(&work->window[i]);
        }
        break;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/menu_ether", MenuEtherExMain);

void MenuEtherL1R1Main(void) {
    EtherL1R1Work *self = MenuEtherL1R1;
    EtherL1R1SpriteIdPair spriteIds;
    signed char *slideOffset;
    int i;

    switch (self->state) {
    case 0:
        spriteIds = D_004DB2E8[0];
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
    case 2:
        slideOffset = self->slideOffset;
        break;
    default:
        return;
    }

    {
        short target[2];
        int menuState = MenuWork.state;

        target[0] = -45;
        target[1] = 528;
        if (menuState == 32) {
            target[0] = 8;
            target[1] = 475;
            if (PadData.pressed & PAD_L1) {
                self->slideOffset[0] = -6;
            } else if (PadData.pressed & PAD_R1) {
                self->slideOffset[1] = 6;
            }
        }

        {
            EtherL1R1SlideStep step = D_004DB2F0[0];

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

INCLUDE_ASM("asm/main/nonmatchings/menu_ether", MenuEtherStatusMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_ether", MenuEtherStatusMain2);

INCLUDE_ASM("asm/main/nonmatchings/menu_ether", MenuEtherListMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_ether", MenuEtherListMain2);

void MenuEtherListMake00(void) {
    EtherListRow *rows = MenuListGet(0);
    int *sortRow = MenuSortAddrGet(0);
    int count = MenuSortCheck(0);
    EtherUnitRecord *unit = func_A191C0(MenuWork.chrNo);
    int i;

    MenuListMake(0, 0);
    for (i = 0; i < count; i++) {
        func_A1A378(func_A1A488((short)sortRow[i])->tecId);
        rows[i].name = MenuTextGet(sortRow[i])->name;
        rows[i].amount = func_00A11220(MenuWork.chrNo, (short)sortRow[i]);
        if (unit->ep >= rows[i].amount) {
            rows[i].flag = 0;
            rows[i].status = 0;
        } else {
            rows[i].flag = 1;
            rows[i].status = 1;
        }
        if (MenuEtherUseCheck((short)sortRow[i]) == 0) {
            rows[i].flag = 1;
            rows[i].status = 2;
        }
    }
}

void MenuEtherListMake01(void) {
    int i;
    EtherListRow *rows = MenuListGet(0);
    int *sortRow = MenuSortAddrGet(0);
    int count = MenuSortCheck(0);
    EtherUnitRecord *unit = func_A191C0(MenuWork.chrNo);

    MenuListMake(0, 0);
    for (i = 0; i < count; i++) {
        EtherStatus *ether = func_A1A488((short)sortRow[i]);
        int j;

        func_A1A378(ether->tecId);
        rows[i].name = MenuTextGet(sortRow[i])->name;
        rows[i].amount = ether->capCost;
        if (unit->capUsed + rows[i].amount < 13) {
            rows[i].flag = 0;
        } else {
            rows[i].flag = 1;
        }
        for (j = 0; j < 12; j++) {
            if (unit->etherId[j] == (short)sortRow[i]) {
                break;
            }
        }
        if (j != 12) {
            rows[i].status = 1;
        } else {
            rows[i].status = 0;
        }
    }
}

void MenuEtherListMake02(int listIndex) {
    EtherListRow *rows = MenuListGet(listIndex);
    int *sortRow = MenuSortAddrGet(listIndex);
    int count = MenuSortCheck(listIndex);
    EtherCharRecord *chr = func_A19210(MenuWork.chrNo);
    int i;

    MenuListMake(listIndex, 0);
    for (i = 0; i < count; i++) {
        int who;

        rows[i].amount = func_A1A488((short)sortRow[i])->pointCost;
        who = MenuEtherWhoCheck((short)sortRow[i]);
        if (rows[i].amount == 0) {
            rows[i].flag = 1;
            rows[i].amount = -1;
        } else if (MenuWork.chrNo != who) {
            rows[i].flag = 1;
        } else if (chr->points >= rows[i].amount / 2) {
            rows[i].flag = 0;
        } else {
            rows[i].flag = 1;
        }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/menu_ether", MenuEtherListMake03);

INCLUDE_ASM("asm/main/nonmatchings/menu_ether", MenuEtherEquip);

void MenuEther(void)
{
    if (MenuWork.state == 0) {
        int heap;
        EtherPartySlot *slot;
        int i;
        int id;

        heap = MainMenuWorkEnd;
        MenuEtherDataBuf = (unsigned char (*)[4])((heap + 0x7FF) & ~0x7FF);
        xglCdReadFile((char *)D_004C8870, MenuEtherDataBuf, 0, 1);
        heap = (int)MenuEtherDataBuf + ETHER_DATA_SIZE;
        MenuEtherPas = (EtherPasWork *)heap;
        heap = (int)MenuEtherPas + ETHER_PAS_SIZE;
        MenuEtherPas->state = 0;
        MenuEtherInfo = (EtherScreenWork *)heap;
        heap = (int)MenuEtherInfo + ETHER_INFO_SIZE;
        MenuEtherInfo->state = 0;
        MenuEtherStatus = (EtherScreenWork *)heap;
        heap = (int)MenuEtherStatus + ETHER_STATUS_SIZE;
        MenuEtherStatus->state = 0;
        MenuEtherStatus2 = (EtherScreenWork *)heap;
        heap = (int)MenuEtherStatus2 + ETHER_STATUS2_SIZE;
        MenuEtherStatus2->state = 0;
        MenuEtherMenu = (EtherMenuScreen *)heap;
        heap = (int)MenuEtherMenu + ETHER_MENU_SIZE;
        MenuEtherMenu->state = 0;
        MenuEtherList = (EtherListWork *)heap;
        heap = (int)MenuEtherList + ETHER_LIST_SIZE;
        MenuEtherList->state = 0;
        MenuEtherList2 = (EtherScreenWork *)heap;
        heap = (int)MenuEtherList2 + ETHER_LIST2_SIZE;
        MenuEtherList2->state = 0;
        MenuEtherEx = (EtherScreenWork *)heap;
        heap = (int)MenuEtherEx + ETHER_EX_SIZE;
        MenuEtherEx->state = 0;
        MenuEtherL1R1 = (EtherL1R1Work *)heap;
        heap = (int)MenuEtherL1R1 + ETHER_L1R1_SIZE;
        MenuEtherL1R1->state = 0;
        MenuWorkEndCheck((unsigned char *)EtherTreeInit(heap, MenuEtherDataBuf));
        MenuWork.nextState = 0x10;
        MenuWork.wait = 12;
        MenuWork.flags = 0;
        MenuWork.partyCursor = 0;
        MenuWork.partyCount = 0;
        MenuEtherCapSet();
        MenuKeepSelectReset();
        slot = ((EtherPartyData *)PartyDataGet())->attack_slots;
        for (i = 0; i < 3; i++, slot++) {
            id = slot->party_id;
            if (id != 0) {
                MenuEtherParty[MenuWork.partyCount] = MenuMaryIdChange(id);
                MenuWork.partyCount++;
            }
        }
        for (id = 1; id < 8; id++) {
            if (PartyFriendLockCheck(id, 1) != 0 && PartyAttackerCheck(id) == 0) {
                MenuEtherParty[MenuWork.partyCount] = MenuMaryIdChange(id);
                MenuWork.partyCount++;
            }
        }
    }
    if (MenuWork.state != MenuWork.nextState) {
        MenuWork.state = MenuWork.nextState;
        MenuWork.flags |= MENU_STATE_CHANGED;
    } else {
        MenuWork.flags &= ~MENU_STATE_CHANGED;
    }
    if (MenuWork.wait != 0) {
        MenuWork.wait--;
    }
    switch (MenuWork.state) {
    /* Pick the character. */
    case 0x10:
        if (MenuWork.flags & MENU_STATE_CHANGED) {
            MenuWork.commandCursor = 0;
            MenuWork.chrNo = MenuEtherParty[MenuWork.partyCursor];
        }
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
            MenuWork.chrNo = MenuEtherParty[MenuWork.partyCursor];
            xglSoundEffectNormalID(SE_CURSOR, 0);
        } else if (PadData.pressed & PAD_CIRCLE) {
            if (MenuMainCharCheck(MenuWork.chrNo) != 0) {
                MenuWork.nextState = 0x20;
                MenuWork.wait = 24;
                xglSoundEffectNormalID(SE_DECIDE, 0);
                MenuWork.charDenied = 0;
            } else {
                xglSoundEffectNormalID(SE_BUZZER, 0);
                MenuWork.charDenied = 1;
            }
        } else if (PadData.pressed & PAD_CROSS) {
            MenuWork.nextState = 0xF0;
            xglSoundEffectNormalID(SE_CANCEL, 0);
        }
        break;

    /* The command menu: use, equip, transfer or back. */
    case 0x20:
        if (MenuWork.flags & MENU_STATE_CHANGED) {
            MenuWork.targetCursor = 0;
        }
        if (MenuWork.wait != 0) {
            break;
        }
        if (PadData.pressed & PAD_L1) {
            int tries;

            for (tries = 0; tries < MenuWork.partyCount; tries++) {
                if (MenuWork.partyCursor == 0) {
                    MenuWork.partyCursor = MenuWork.partyCount - 1;
                } else {
                    MenuWork.partyCursor--;
                }
                MenuWork.chrNo = MenuEtherParty[MenuWork.partyCursor];
                if (MenuMainCharCheck(MenuWork.chrNo) != 0) {
                    break;
                }
            }
            xglSoundEffectNormalID(SE_CURSOR, 0);
            MenuKeepSelectReset();
        } else if (PadData.pressed & PAD_R1) {
            int tries;

            for (tries = 0; tries < MenuWork.partyCount; tries++) {
                if (MenuWork.partyCursor == MenuWork.partyCount - 1) {
                    MenuWork.partyCursor = 0;
                } else {
                    MenuWork.partyCursor++;
                }
                MenuWork.chrNo = MenuEtherParty[MenuWork.partyCursor];
                if (MenuMainCharCheck(MenuWork.chrNo) != 0) {
                    break;
                }
            }
            xglSoundEffectNormalID(SE_CURSOR, 0);
            MenuKeepSelectReset();
        }
        {
            int cursor = MenuWork.commandCursor;

            MenuWork.commandCursor = MenuSelectMove(cursor, 4, 0);
            if (cursor != MenuWork.commandCursor) {
                break;
            }
            if (PadData.pressed & PAD_CIRCLE) {
                unsigned char commandState[4] = { 0x30, 0x50, 0x70, 0x10 };

                MenuWork.nextState = commandState[cursor];
                MenuWork.targetChrNo = MenuEtherParty[0];
                MenuWork.wait = 12;
                if (cursor == 3) {
                    MenuKeepSelectReset();
                    xglSoundEffectNormalID(SE_CANCEL, 0);
                } else {
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                }
            }
            if (PadData.pressed & PAD_CROSS) {
                MenuWork.nextState = 0x10;
                MenuWork.wait = 12;
                xglSoundEffectNormalID(SE_CANCEL, 0);
                MenuKeepSelectReset();
            }
        }
        break;

    /* Use: pick the ether. */
    case 0x30:
        if (MenuWork.flags & MENU_STATE_CHANGED) {
            MenuSortSet(0, 0x80, MenuWork.chrNo + 0x10000);
            MenuEtherListMake00();
        }
        if (MenuWork.wait != 0) {
            break;
        }
        MenuWork.listSelect = WindowSPSelect(&MenuEtherList->window, PadData.repeat);
        WindowSPKeepSelect(&MenuEtherList->window, &MenuKeepSelect[0]);
        if (PadData.pressed & PAD_CIRCLE) {
            if (MenuWork.listSelect >= 0) {
                short etherId = MenuSortGet(0, MenuWork.listSelect);

                MenuWork.useType = MenuEtherUseCheck(etherId);
                if (MenuWork.useType != 0) {
                    EtherUnitRecord *unit;

                    etherId = MenuSortGet(0, MenuWork.listSelect);
                    unit = func_A191C0(MenuWork.chrNo);
                    if (func_00A11220(MenuWork.chrNo, etherId) <= unit->ep) {
                        MenuWork.nextState = 0x32;
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                    } else {
                        xglSoundEffectNormalID(SE_BUZZER, 0);
                    }
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            }
        } else if (PadData.pressed & PAD_CROSS) {
            if (MenuCursorKeepCheck() == 0) {
                MenuWork.commandCursor = 0;
            }
            WindowSPKeepSelectCheck(&MenuKeepSelect[0]);
            MenuWork.nextState = 0x20;
            MenuWork.wait = 12;
            xglSoundEffectNormalID(SE_CANCEL, 0);
        }
        break;

    /* Use: pick who the ether is used on. */
    case 0x32:
        if (MenuWork.wait != 0) {
            break;
        }
        if (MenuWork.useType == 1 || MenuWork.useType == 3) {
            if (PadData.repeat & PAD_UP) {
                if (MenuWork.targetCursor != 0) {
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                    MenuWork.targetCursor--;
                }
                MenuWork.targetChrNo = MenuEtherParty[MenuWork.targetCursor];
            }
            if (PadData.repeat & PAD_DOWN) {
                if (MenuWork.targetCursor != MenuWork.partyCount - 1) {
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                    MenuWork.targetCursor++;
                }
                MenuWork.targetChrNo = MenuEtherParty[MenuWork.targetCursor];
            }
        }
        MenuWork.targetChrNo = MenuEtherParty[MenuWork.targetCursor];
        if (PadData.pressed & PAD_CIRCLE) {
            int user = MenuWork.chrNo;
            short etherId = MenuSortGet(0, MenuWork.listSelect);
            EtherUnitRecord *unit;

            if (MenuWork.useType == 1) {
                if (MenuCharHpCheck(MenuWork.targetChrNo) == 0) {
                    func_00A11270(user, MenuWork.targetChrNo, etherId);
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            } else if (MenuWork.useType == 3) {
                if (MenuWork.chrNo == MenuWork.targetChrNo && MenuCharHpCheck(MenuWork.chrNo) == 0) {
                    func_00A11270(user, MenuWork.targetChrNo, etherId);
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            } else {
                EtherPartyData *party = (EtherPartyData *)PartyDataGet();

                if (MenuCharHpCheck(-1) == 0) {
                    if (party->attack_slots[0].party_id != 0) {
                        func_00A11270(user, party->attack_slots[0].party_id, etherId);
                    }
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            }
            unit = func_A191C0(MenuWork.chrNo);
            etherId = MenuSortGet(0, MenuWork.listSelect);
            if (unit->ep < func_A1A378(func_A1A488(etherId)->tecId)->epCost) {
                MenuWork.nextState = 0x30;
            }
            MenuEtherListMake00();
        }
        if (PadData.pressed & PAD_CROSS) {
            xglSoundEffectNormalID(SE_CANCEL, 0);
            MenuWork.nextState = 0x30;
        }
        break;

    /* Equip: pick the ether to equip or remove. */
    case 0x50:
        if (MenuWork.flags & MENU_STATE_CHANGED) {
            MenuSortSet(0, 0x80, MenuWork.chrNo);
            MenuEtherListMake01();
        }
        if (MenuWork.wait == 0) {
            MenuWork.listSelect = WindowSPSelect(&MenuEtherList->window, PadData.repeat);
            WindowSPKeepSelect(&MenuEtherList->window, &MenuKeepSelect[5]);
            if ((PadData.pressed & PAD_SQUARE) && MenuWork.listSelect >= 0) {
                int start = MenuWork.listSelect + 1;
                int count = MenuSortCheck(0);
                EtherListRow *rows = MenuListGet(0);
                int i;
                int row = 0;

                for (i = 0; i < count; i++) {
                    row = (start + i) % count;
                    if (rows[row].status != 0) {
                        break;
                    }
                }
                WindowSPSelectJump(&MenuEtherList->window, row);
                xglSoundEffectNormalID(SE_DECIDE, 0);
            }
            if ((PadData.repeat & PAD_CIRCLE) && MenuWork.listSelect >= 0) {
                MenuWork.etherId = MenuSortGet(0, MenuWork.listSelect);
                MenuEtherEquip();
                MenuEtherListMake01();
                xglSoundEffectNormalID(SE_DECIDE, 0);
            }
            if (PadData.pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_CANCEL, 0);
                MenuWork.nextState = 0x20;
                MenuWork.wait = 12;
                if (MenuCursorKeepCheck() == 0) {
                    MenuWork.commandCursor = 0;
                }
                WindowSPKeepSelectCheck(&MenuKeepSelect[5]);
            }
        }
        MenuWork.listSelect = WindowSPSelect(&MenuEtherList->window, 0);
        if (MenuWork.listSelect >= 0) {
            MenuWork.etherId = MenuSortGet(0, MenuWork.listSelect);
        }
        break;

    /* Transfer: pick the ether. */
    case 0x70:
        if (MenuWork.flags & MENU_STATE_CHANGED) {
            MenuSortSet(0, 0x80, MenuWork.chrNo);
            MenuEtherListMake02(0);
            MenuWork.menuCursor = 0;
            MenuWork.slotCount = 0;
            MenuWork.etherId = 0;
        }
        if (MenuWork.wait == 0) {
            MenuWork.listSelect = WindowSPSelect(&MenuEtherList->window, PadData.repeat);
            WindowSPKeepSelect(&MenuEtherList->window, &MenuKeepSelect[10]);
            if (MenuWork.listSelect >= 0) {
                unsigned char *slots = MenuEtherDataGet(MenuSortGet(0, MenuWork.listSelect));
                int i;

                MenuWork.slotCount = 0;
                for (i = 0; i < 3; i++) {
                    if (slots[i] != 0) {
                        MenuWork.slotCount++;
                    }
                }
                MenuWork.etherId = MenuSortGet(0, MenuWork.listSelect);
            }
            if (PadData.pressed & PAD_CIRCLE) {
                if (MenuWork.listSelect >= 0) {
                    int owner = MenuEtherWhoCheck(MenuWork.etherId);

                    if (MenuWork.chrNo == owner) {
                        MenuWork.nextState = 0x72;
                        MenuWork.wait = 8;
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                    } else {
                        xglSoundEffectNormalID(SE_BUZZER, 0);
                    }
                }
            }
            if (PadData.pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_CANCEL, 0);
                MenuWork.nextState = 0x20;
                MenuWork.wait = 12;
                if (MenuCursorKeepCheck() == 0) {
                    MenuWork.commandCursor = 0;
                }
                WindowSPKeepSelectCheck(&MenuKeepSelect[10]);
            }
        }
        MenuWork.listSelect = WindowSPSelect(&MenuEtherList->window, 0);
        if (MenuWork.listSelect >= 0) {
            MenuWork.etherId = MenuSortGet(0, MenuWork.listSelect);
        }
        break;

    /* Transfer: the command for the ether (slot, give, back). */
    case 0x72:
        if (MenuWork.wait != 0) {
            break;
        }
        MenuWork.menuCursor = MenuSelectMove(MenuWork.menuCursor, 3, 0);
        if (PadData.pressed & PAD_CIRCLE) {
            short cost = func_A1A488(MenuWork.etherId)->pointCost;
            int points = func_A19210(MenuWork.chrNo)->points;
            int owner = MenuEtherWhoCheck(MenuWork.etherId);

            switch (MenuWork.menuCursor) {
            case 0:
            case 1:
                if (MenuWork.chrNo != owner) {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                } else {
                    switch (MenuWork.menuCursor) {
                    case 0:
                        if (points >= cost && MenuWork.slotCount > 0) {
                            MenuWork.nextState = 0x74;
                            MenuWork.wait = 8;
                            xglSoundEffectNormalID(SE_DECIDE, 0);
                        } else {
                            xglSoundEffectNormalID(SE_BUZZER, 0);
                        }
                        break;
                    case 1:
                        if (points >= cost / 2 && MenuEtherJoutoCheck(MenuWork.etherId) != 0
                            && MenuWork.partyCount >= 2) {
                            MenuWork.nextState = 0x82;
                            MenuWork.targetCursor = 0;
                            MenuWork.wait = 12;
                            xglSoundEffectNormalID(SE_DECIDE, 0);
                        } else {
                            xglSoundEffectNormalID(SE_BUZZER, 0);
                        }
                        break;
                    }
                }
                break;
            case 2:
                MenuWork.nextState = 0x70;
                MenuWork.wait = 4;
                xglSoundEffectNormalID(SE_CANCEL, 0);
                break;
            }
        }
        if (PadData.pressed & PAD_CROSS) {
            xglSoundEffectNormalID(SE_CANCEL, 0);
            MenuWork.nextState = 0x70;
            MenuWork.wait = 4;
        }
        break;

    /* Transfer to a slot: pick the slot. */
    case 0x74:
        if (MenuWork.flags & MENU_STATE_CHANGED) {
            unsigned char *slots = MenuEtherDataGet(MenuWork.etherId);

            MenuWork.slotEther = slots[0];
            MenuWork.slotCursor = 0;
        }
        if (MenuWork.wait != 0) {
            break;
        }
        {
            unsigned char *slots = MenuEtherDataGet(MenuWork.etherId);

            MenuWork.slotCursor = MenuSelectMove(MenuWork.slotCursor, MenuWork.slotCount, 0);
            MenuWork.slotEther = slots[MenuWork.slotCursor];
        }
        if (PadData.pressed & PAD_CIRCLE) {
            if (func_A19578(MenuWork.chrNo, MenuWork.slotEther) == 0) {
                short cost = func_A1A488(MenuWork.etherId)->pointCost;

                if (func_A19210(MenuWork.chrNo)->points >= cost) {
                    MenuWork.nextState = 0x76;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                }
            } else {
                xglSoundEffectNormalID(SE_BUZZER, 0);
            }
        }
        if (PadData.pressed & PAD_CROSS) {
            if (MenuCursorKeepCheck() == 0) {
                MenuWork.menuCursor = 0;
            }
            MenuWork.nextState = 0x72;
            xglSoundEffectNormalID(SE_CANCEL, 0);
        }
        break;

    /* Transfer to a slot: confirm. */
    case 0x76:
        if (MenuWork.flags & MENU_STATE_CHANGED) {
            MenuWork.confirmCursor = 0;
        }
        if (MenuWork.wait != 0) {
            break;
        }
        MenuWork.confirmCursor = MenuSelectMove(MenuWork.confirmCursor, 2, 0);
        if (PadData.pressed & PAD_CIRCLE) {
            if (MenuWork.confirmCursor == 0) {
                EtherCharRecord *character;

                func_A194E0(MenuWork.chrNo, MenuEtherDataGet(MenuWork.etherId)[MenuWork.slotCursor]);
                character = func_A19210(MenuWork.chrNo);
                character->points -= (short)func_A1A488(MenuWork.etherId)->pointCost;
                MenuSortSet(0, 0x80, MenuWork.chrNo);
                MenuEtherListMake02(0);
                WindowSPItemChange(&MenuEtherList->window);
                MenuEtherList->window.state = 4;
                xglSoundEffectNormalID(SE_DECIDE, 0);
                MenuWork.nextState = 0x78;
            } else {
                xglSoundEffectNormalID(SE_CANCEL, 0);
                MenuWork.nextState = 0x74;
            }
        }
        if (PadData.pressed & PAD_CROSS) {
            xglSoundEffectNormalID(SE_CANCEL, 0);
            MenuWork.nextState = 0x74;
        }
        break;

    case 0x78:
        if (MenuWork.wait == 0 && (PadData.pressed & PAD_CIRCLE)) {
            MenuWork.nextState = 0x70;
        }
        break;

    /* Give to a party member: pick who. */
    case 0x82:
        if (MenuWork.flags & MENU_STATE_CHANGED) {
            unsigned char *target = MenuWork.targetList;
            int i;

            MenuWork.targetCount = 0;
            for (i = 0; i < MenuWork.partyCount; i++) {
                if (MenuEtherParty[i] != MenuWork.chrNo && MenuMainCharCheck(MenuEtherParty[i]) != 0) {
                    *target++ = i;
                    MenuWork.targetCount++;
                }
            }
        }
        if (MenuWork.wait != 0) {
            break;
        }
        if ((PadData.repeat & PAD_UP) && MenuWork.targetCursor != 0) {
            xglSoundEffectNormalID(SE_CURSOR, 0);
            MenuWork.targetCursor--;
        }
        if ((PadData.repeat & PAD_DOWN) && MenuWork.targetCursor != MenuWork.targetCount - 1) {
            xglSoundEffectNormalID(SE_CURSOR, 0);
            MenuWork.targetCursor++;
        }
        MenuWork.targetChrNo = MenuEtherParty[MenuWork.targetList[MenuWork.targetCursor]];
        if (PadData.pressed & PAD_CIRCLE) {
            if (func_A19578(MenuWork.targetChrNo, MenuWork.etherId) == 0) {
                MenuWork.nextState = 0x84;
                MenuWork.wait = 8;
                xglSoundEffectNormalID(SE_DECIDE, 0);
            } else {
                xglSoundEffectNormalID(SE_BUZZER, 0);
            }
        }
        if (PadData.pressed & PAD_CROSS) {
            if (MenuCursorKeepCheck() == 0) {
                MenuWork.menuCursor = 0;
            }
            xglSoundEffectNormalID(SE_CANCEL, 0);
            MenuWork.nextState = 0x72;
        }
        break;

    /* Give to a party member: confirm. */
    case 0x84:
        if (MenuWork.flags & MENU_STATE_CHANGED) {
            MenuEtherListMake03(1);
            MenuWork.confirmCursor = 0;
        }
        if (MenuWork.wait != 0) {
            break;
        }
        MenuWork.confirmCursor = MenuSelectMove(MenuWork.confirmCursor, 2, 0);
        if (PadData.pressed & PAD_CIRCLE) {
            if (MenuWork.confirmCursor == 0) {
                EtherCharRecord *character;
                EtherListRow *rows;
                int count;
                int i;

                func_A194E0(MenuWork.targetChrNo, MenuWork.etherId);
                character = func_A19210(MenuWork.chrNo);
                character->points -= (short)func_A1A488(MenuWork.etherId)->pointCost / 2;
                rows = MenuListGet(1);
                count = MenuSortCheck(1);
                for (i = 0; i < count; i++) {
                    rows[i].flag = 0;
                }
                xglSoundEffectNormalID(SE_DECIDE, 0);
                MenuWork.nextState = 0x86;
            } else {
                xglSoundEffectNormalID(SE_CANCEL, 0);
                MenuWork.nextState = 0x82;
            }
        }
        if (PadData.pressed & PAD_CROSS) {
            xglSoundEffectNormalID(SE_CANCEL, 0);
            MenuWork.nextState = 0x82;
        }
        break;

    case 0x86:
        if (MenuWork.wait == 0 && (PadData.pressed & PAD_CIRCLE)) {
            MenuWork.nextState = 0x70;
            MenuWork.wait = 12;
        }
        break;

    /* Leave the menu. */
    case 0xF0:
        if (MenuWork.flags & MENU_STATE_CHANGED) {
            MenuWork.wait = 16;
        }
        if (MenuWork.wait == 0) {
            MenuWork.nextState = 0xFF;
        }
        break;

    case 0xFF:
        ChangeTopLevel(0);
        break;
    }
    EtherTreeMain();
    MenuEtherPasMain();
    MenuEtherInfoMain();
    MenuEtherStatusMain();
    MenuEtherStatusMain2();
    MenuEtherMenuMain();
    MenuEtherListMain();
    MenuEtherListMain2();
    MenuEtherExMain();
    MenuEtherL1R1Main();
    xglFontDebugPrintf(0, 0x40, "\vether");
    xglFontDebugPrintf(0, 0x50, "\vms %2d", MenuWork.commandCursor);
    xglFontDebugPrintf(0, 0x58, "\vms2%2d", MenuWork.menuCursor);
    xglFontDebugPrintf(0, 0x60, "\vcs %2d/%2d", MenuWork.partyCursor, MenuWork.partyCount);
    xglFontDebugPrintf(0, 0x68, "\vcs2%2d", MenuWork.targetCursor);
    xglFontDebugPrintf(0, 0x70, "\vlst%2d", MenuWork.listSelect);
    xglFontDebugPrintf(0, 0x78, "\vyn %2d", MenuWork.confirmCursor);
    xglFontDebugPrintf(0, 0x80, "\vum %2d", MenuWork.slotCursor);
    xglFontDebugPrintf(0, 0x88, "\vumm%2d", MenuWork.slotCount);
}
