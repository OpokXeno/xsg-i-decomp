#include "common.h"
#include "main/menu_top_command_check.h"
#include "main/e_battle_win_open.h"
#include "main/menu_1.h"

#include "shared.h"
#include "main/e_number_main.h"
#include "main/end_print.h"
#include "main/menu_para_pt_rate_get.h"
#include "main/open_close_main.h"
#include "main/party.h"
#include "main/xgl_cd.h"
#include "main/xgl_font.h"
#include "main/xgl_sound.h"

#define PAD_L1 0x0004
#define PAD_R1 0x0008
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
    union {
        int listSelect;             /* +0x50 */
        unsigned char listSelectBytes[4];
    };
    unsigned char unmodeled_54[0x60 - 0x54];
    unsigned char targetList[8];    /* +0x60 */
} MenuEtherWork;

typedef struct MenuEtherPadData {
    unsigned char unmodeled_00[0x2A];
    unsigned short pressed; /* +0x2A */
    unsigned char unmodeled_2c[0x34 - 0x2C];
    unsigned short repeat;  /* +0x34 */
} MenuEtherPadData;

typedef MenuTextEntry EtherTextEntry;

typedef struct EtherL1R1SpriteIdPair {
    short id[2];
} EtherL1R1SpriteIdPair;

typedef struct EtherL1R1SlideStep {
    signed char side[2];
} EtherL1R1SlideStep;

typedef struct EtherInfoCharacterHelp { const char *text[2]; } EtherInfoCharacterHelp;

typedef struct EtherPasBox {
    short x; /* +0x00 */
    unsigned short y; /* +0x02 */
    int kind; /* +0x04 */
    short width; /* +0x08 */
    short height; /* +0x0A */
} EtherPasBox;

typedef struct EtherPasWork {
    unsigned char state; /* +0x000 */
    unsigned char unmodeled_01[3];
    int kind; /* +0x004 */
    WindowDX window; /* +0x008 */
    EMessageParam messages[6]; /* +0x19C */
    EtherPasBox box; /* +0x334 */
    unsigned char callbackWork[1]; /* +0x340 */
} EtherPasWork;


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
    WindowDX window[2]; /* +0x008 */
    EtherMenuList list[2]; /* +0x330 */
} EtherMenuScreen;

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

typedef struct EtherInfoCommandHelp {
    const char *text[5];
} EtherInfoCommandHelp;

typedef struct EtherInfoActionHelp {
    const char *text[3];
} EtherInfoActionHelp;

typedef struct EtherInfoAcquireHelp {
    const char *text[9];
} EtherInfoAcquireHelp;

typedef struct EtherInfoTransferHelp {
    const char *text[7];
} EtherInfoTransferHelp;

typedef struct EtherInfoMessage {
    short x;
    short y;
    unsigned char unmodeled_04[4];
    int visible;
    char *text;
    unsigned char unmodeled_10[0x1B0 - 0x10];
} EtherInfoMessage;


typedef struct EtherInfoWork {
    unsigned char state;
    unsigned char active;
    signed char previous;
    unsigned char helpHeld;
    int work;
    WindowDX window;
    unsigned char unmodeled_19c[4];
    EtherInfoMessage message;
    char text[256];
} EtherInfoWork;

typedef struct EtherStatusPanelNumber {
    short x;
    short y;
    int color;
    unsigned char unmodeled_08[0x90 - 0x08];
} EtherStatusPanelNumber;

typedef struct EtherStatusPanelEarly {
    signed char chrNo;
    unsigned char enabled;
    short guestChrNo;
    short offsetX;
    short offsetY;
    unsigned char unmodeled_08[0x14 - 0x08];
    EtherStatusPanelNumber number[6];
    unsigned char unmodeled_374[0x840 - 0x374];
} EtherStatusPanelEarly;

typedef struct EtherStatusWindowEarly {
    short x;
    short y;
    int color;
    short width;
    short height;
    const char *title;
    unsigned char state;
    unsigned char enabled;
    unsigned char unmodeled_12[2];
    void *callback;
    void *callbackArg;
    unsigned char unmodeled_1c[0x9C - 0x1C];
    const char *renderTitle;
    unsigned char unmodeled_a0[0x194 - 0xA0];
} EtherStatusWindowEarly;


typedef struct EtherStatusRecordHeaderEarly {
    unsigned char state;
    signed char active;
    unsigned char unmodeled_02[2];
    unsigned char verticalFirst;
    unsigned char collapsed;
    signed char resetByte;
    signed char previousParty;
    int color;
} EtherStatusRecordHeaderEarly;

typedef struct EtherPartyStatusWorkEarly {
    EtherStatusRecordHeaderEarly header;
    EtherStatusWindowEarly window[8];
    EtherStatusPanelEarly panel[8];
    ECursolMainData cursor[2];
} EtherPartyStatusWorkEarly;

typedef struct EtherStatusPanel {
    signed char chrNo;
    signed char mode;
    unsigned char unmodeled_02[2];
    short x;
    short y;
    unsigned char unmodeled_08[0x14 - 0x08];
    EtherStatusPanelNumber number[6];
    unsigned char unmodeled_374[0x840 - 0x374];
} EtherStatusPanel;

typedef struct EtherStatusWindow {
    short x;
    short y;
    int color;
    short width;
    short height;
    const char *title;
    unsigned char state;
    unsigned char enabled;
    unsigned char unmodeled_12[2];
    void (*callback)(struct WindowDX *, void *);
    struct EtherStatusPanel *callbackArg;
    unsigned char unmodeled_1c[0x9C - 0x1C];
    const char *renderTitle;
    unsigned char unmodeled_a0[0x194 - 0xA0];
} EtherStatusWindow;


typedef struct EtherTargetStatusWork {
    unsigned char state;
    unsigned char active;
    unsigned char unmodeled_02;
    unsigned char targetInitialized;
    signed char visibleRow;
    signed char scrollOffset;
    signed char previousCursor;
    unsigned char drawFrame;
    signed char scrollLimit;
    signed char listMode;
    signed char multiCursor;
    signed char visibleCount;
    int color;
    EtherStatusWindow window[8];
    EtherStatusPanel panel[8];
    ECursolMainData cursor[3];
    EtherPasBox frame[2][2];
    EtherPasBox scrollBar;
    unsigned char unmodeled_4f58[0x4F64 - 0x4F58];
} EtherTargetStatusWork;

/* MenuEtherList: the two ether list windows (window[1] shows list 1, opened
 * in menu state 132) with the marks and point numbers drawn over window[0]. */
typedef struct EtherMenuListsWork {
    unsigned char state;
    unsigned char unmodeled_01;
    unsigned char configured;
    unsigned char acquireLayout;
    unsigned char showNumbers;
    unsigned char unmodeled_05[3];
    int work;
    WindowSPCursor window[2];       /* +0x0C */
    EtherL1R1Sprite sprite[8];
    ENumberMainData number[8];
} EtherMenuListsWork;



typedef struct EtherResultWork {
    unsigned char state;
    unsigned char unmodeled_01;
    unsigned char rightPanel;
    unsigned char unmodeled_03;
    int work;
    WindowDX window;
    EMessageParam messages[3];
    ENumberMainData numbers[3];
    ECursolMainData cursor;
} EtherResultWork;


typedef struct EtherDetailRow {
    ERibbonMainData ribbon;
    ETagFontMainData font;
    ENumberMainData number;
} EtherDetailRow;

typedef struct EtherDetailWork {
    unsigned char state;
    unsigned char unmodeled_01[3];
    int work;
    EtherDetailRow rows[2];
    EMessageParam message;
} EtherDetailWork;

extern const char etherEvolveMenuText[];
extern const char etherUseMenuText[];
extern const char etherConfirmText[];
extern char etherYesNoText[8];
extern char etherWtText[8];
extern char etherPointsText[8];
extern const char etherSetSearchText[20];
extern MenuEtherWork MenuWork;
extern MenuEtherPadData PadData;
extern unsigned char (*MenuEtherDataBuf)[4];
extern const EtherL1R1SpriteIdPair etherL1R1SpriteTable[];
extern const EtherL1R1SlideStep etherL1R1SlideTable[];
extern const char etherTreePath[32];
extern const char etherChooseCharacterText[48];
extern EtherInfoCharacterHelp etherCharacterHelp[];
extern char etherCancelText[8];
extern char etherPeriodText[];
extern char etherToText[8];
extern char etherEmptyText[];
extern char etherPeriodNewlineText[];
extern char etherPeriodEndText[];
extern char etherOfText[];
extern char etherQuestionText[];
extern char etherMenuTitleText[];
extern char etherSelectTitleText[];
extern char etherResultTitleText[];
extern EtherPasWork *MenuEtherPas;
extern EtherMenuScreen *MenuEtherMenu;
extern EtherL1R1Work *MenuEtherL1R1;
extern EtherInfoWork *MenuEtherInfo;
extern EtherDetailWork *MenuEtherEx;
extern EtherPartyStatusWorkEarly *MenuEtherStatus;
extern EtherTargetStatusWork *MenuEtherStatus2;
extern EtherMenuListsWork *MenuEtherList;
extern EtherResultWork *MenuEtherList2;
extern unsigned char *MainMenuWorkEnd;
extern unsigned char MenuKeepSelect[0x64];
extern const EtherInfoCommandHelp etherCommandHelp;
extern const EtherInfoActionHelp etherActionHelp;
extern const EtherInfoAcquireHelp etherAcquireHelp;
extern const EtherInfoTransferHelp etherTransferHelp;
extern const char etherCapLimitText[];
extern const char etherCompleteText[];
extern const char etherListEpTitleText[9];
extern const char etherListWtTitleText[9];
extern const char etherListEPointsTitleText[12];
extern const char etherListSubTitleText[12];
extern const char etherTransferRenderTitle[];

void WindowSPSet(WindowSPCursor *cursor);
void WindowSPMain(WindowSPCursor *cursor);
void WindowSPSelectJump(WindowSPCursor *cursor, int selection);
/*
 * The retail ELF names these ov01 data-table accessors (still assembler in
 * their own TU, src/ov01/data_unit_org_get.c) dataEthGet (0x00a1a488) and
 * dataTecGet (0x00a1a378); until that TU recovers them, the synthetic
 * linker script only binds their scaffold func_<VA> spelling.
 */
/* dataEthGet */
extern EtherStatus *func_A1A488(int etherId);

/* dataTecGet */
extern TecStatus *func_A1A378(short tecId);

extern MenuTextEntry *func_A2C5A8(unsigned int etherId);
extern EtherUnitRecord *func_A191C0(int chrNo);
extern EtherCharRecord *func_A19210(int chrNo);
MenuListEntry *MenuListGet(int listIndex);
extern void MenuListMake(int listIndex, int mode);
extern int *MenuSortAddrGet(int listIndex);
int MenuSortCheck(int listIndex);
int MenuEtherUseCheck(short etherId);
int func_00A11220(int chrNo, int etherId);

extern void MoveSlide(short *current, short *target, float rate);

extern void eSpriteSet(void *sprite, short spriteId);
extern void eSpriteMain(void *sprite);
void func_A194E0(int chrNo, int etherId);
int func_A19578(int chrNo, int etherId);
void func_00A11270(int chrNo, int targetChrNo, int etherId);
unsigned char *EtherTreeInit();
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
void MenuEtherListMake02(int listIndex);
void MenuEtherListMake03(int listIndex);
int MenuEtherEquip(void);
int PartyAttackerCheck(int id);
int MenuCharHpCheck(int chrNo);
void MenuWorkEndCheck(unsigned char *end);
void MenuKeepSelectReset(void);
int MenuCursorKeepCheck(void);
int MenuSelectMove(int cursor, int count, int wrap);
int MenuSortGet(int listIndex, int index);
void ChangeTopLevel(int level);
extern const char *MenuCharNameGet(unsigned int charId);
extern const char *MenuTagTextGet(int index);
static const char *msg_0_0036DC00[6] = {
    "Ether",
    "/Use",
    "/Set",
    "/Use E.Pts",
    "/Evolve",
    "/Transfer"
};

static const char * msg00_1_0036DC18[] = { etherUseMenuText, etherEvolveMenuText };

static const char * msg01_2_0036DC20[] = { etherYesNoText, etherConfirmText };

static const char * msg00_3_0036DC28[] = { etherPointsText, etherWtText };

/* Genuine conversion: the colour bytes of this .rodata text are rewritten at
 * run time (MenuEtherExMain), as through the original's char string literal. */
static char * msg01_4_0036DC30[] = { (char *)etherSetSearchText };

short MenuEtherParty[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static inline EtherInfoWork *EtherInfoWorkGet(EtherInfoWork **slot) {
    return *slot;
}

/* The four-byte record of an ether (its slot ethers), or 0 outside 1..80. */
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

void MenuEtherCapSet(void) {
    int chrNo;
    for (chrNo = 1; chrNo < 8; chrNo++) {
        EtherUnitRecord *unit = func_A191C0(chrNo);
        short *equippedEther = unit->etherId;
        int slot;
        unit->capUsed = 0;
        for (slot = 0; slot < 12; slot++) {
            int etherId = *equippedEther++;
            if (etherId != 0) {
                unit->capUsed = unit->capUsed +
                    (unsigned char)func_A1A488(etherId)->capCost;
            } else {
                break;
            }
        }
    }
}

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
        pas->window.style = pas->kind;
        pas->window.width = 272;
        pas->window.height = 30;
        pas->window.close_callback = MenuPasWindow;
        pas->window.close_callback_arg = pas->callbackWork;
        pas->window.state = 1;
        WindowDXMain(&pas->window);
        pas->window.state = 3;
        for (i = 0; i < 6; i++) {
            eMessageSet(&pas->messages[i], msg_0_0036DC00[i]);
            pas->messages[i].mode = 0x20;
            pas->messages[i].x = 288;
            pas->messages[i].y = 11;
            pas->messages[i].draw_parameter = pas->kind + 2;
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

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const char etherChooseCharacterText[48] =
    /* Text: Choose a character. |  cannot be chosen. */
    "Choose a character.\0\0\0\0\0 cannot be chosen.\0\0\0\0\0";

const char etherUtilizeLeadText[28] = "Utilize Ether available to ";

const char etherSetHelpText[85] = "Set Ether. Caution: You must complete this step in order to utilize any Ether skill.";

const char etherUsePointsHelpText[79] = "Use E.Pts to obtain new Ether skills and/or transfer them to other characters.";

const EtherInfoCommandHelp etherCommandHelp = {
    { etherUtilizeLeadText, etherSetHelpText, etherUsePointsHelpText, etherCancelText, etherPeriodText }
};

const char etherObtainHelpText[38] = "Use E.Pts to obtain new Ether skills.";

const char etherTransferHelpText[72] = "Use E.Pts to transfer Ether skills to other characters.\n(Uses 1/2 E.Pt)";

const EtherInfoActionHelp etherActionHelp = {
    { etherObtainHelpText, etherTransferHelpText, etherCancelText }
};

const char etherAcquireLeadText[26] = "Acquire new Ether skill, ";

const char etherAcquiredText[14] = "You acquired ";

const char etherNotEnoughPointsText[18] = "Not enough E.Pts.";

const char etherMaxLevelText[40] = "This is the highest level of evolution.";

const char etherAlreadyAcquiredText[27] = "You have already acquired ";

const char etherTransferredLockedText[69] = "Transferred Ether skills cannot be evolved or transferred to others.";

const char etherBattleOnlyText[50] = "This Ether skill is only available during battle.";

const char etherNoTransferText[40] = "This Ether skill cannot be transferred.";

const EtherInfoAcquireHelp etherAcquireHelp = {
    { etherAcquireLeadText, etherAcquiredText, etherNotEnoughPointsText, etherMaxLevelText, etherAlreadyAcquiredText,
      etherNotEnoughPointsText, etherTransferredLockedText, etherBattleOnlyText, etherNoTransferText }
};

const char etherTransferLeadText[10] = "Transfer ";

const char etherCannotTransferText[23] = "Cannot be transferred.";

const char etherCharAlreadyAcquiredText[37] = "This character has already acquired ";

const char etherSelectRecipientText[43] = ".\nSelect a recipient for this Ether skill.";

const EtherInfoTransferHelp etherTransferHelp = {
    { etherTransferLeadText, etherTransferLeadText, etherToText, etherToText, etherCannotTransferText,
      etherCharAlreadyAcquiredText, etherSelectRecipientText }
};

const char etherCapLimitText[58] = "The maximum allotment \nfor Ether skills has been reached.";

/* Text:  complete. \x1F */
const char etherCompleteText[13] = " complete. \037";

EtherInfoCharacterHelp etherCharacterHelp[1] = {
    {{etherChooseCharacterText, etherChooseCharacterText + 24}}
};

char etherCancelText[8] = "Cancel.";

char etherPeriodText[8] = ".";

char etherToText[8] = " to ";

char etherEmptyText[8] = { 0 };

char etherPeriodNewlineText[8] = ".\n";

/* Text: . \x1F */
char etherPeriodEndText[8] = ". \x1f";

char etherOfText[8] = "of ";

char etherQuestionText[8] = "?";

void MenuEtherInfoMain(void) {
    EtherInfoWork *screen = EtherInfoWorkGet(&MenuEtherInfo);
    const char *capHelp = etherCapLimitText;
    EtherInfoCharacterHelp characterHelp = etherCharacterHelp[0];
    EtherInfoCommandHelp commandHelp = etherCommandHelp;
    EtherInfoActionHelp actionHelp = etherActionHelp;
    EtherInfoAcquireHelp acquireHelp = etherAcquireHelp;
    EtherInfoTransferHelp transferHelp = etherTransferHelp;
    short targetY;
    WindowDX *window;
    char *text;

    switch (screen->state) {
    case 0:
        screen->work = 0x00FFFFF0;
        window = &screen->window;
        text = screen->text;
        WindowDXSet(window);
        screen->window.x = -16;
        screen->window.y = 480;
        screen->window.style = screen->work;
        screen->window.width = 544;
        screen->window.height = 54;
        screen->window.close_callback =
            (void (*)(struct WindowDX *, void *))MenuInfoWindow;
        {
            EtherInfoMessage *message = &screen->message;
            message->x = 0;
            screen->window.close_callback_arg = message;
        }
        screen->message.y = 0;
        __builtin_memset(&screen->message.text, 0, sizeof(screen->message.text));
        screen->window.state = 1;
        WindowDXMain(window);
        screen->window.state = 3;
        screen->previous = -1;
        screen->active = 1;
        screen->helpHeld = 0;
        memset(text, 0, 256);
        screen->state = 2;
        break;
    case 2:
        window = &screen->window;
        text = screen->text;
        break;
    default:
        return;
    }

    targetY = 386;
    if ((unsigned char)(MenuWork.flags & 1)) {
        screen->helpHeld = 0;
    }
    if (screen->helpHeld == 0) {
        if (PadData.pressed & 0x20) {
            screen->helpHeld = 1;
        }
    } else if (PadData.repeat != 0) {
        screen->helpHeld = 0;
    }

    switch (MenuWork.state) {
    case 16:
        eMessageCpy(text, characterHelp.text[0]);
        if (screen->helpHeld != 0 && MenuWork.charDenied != 0) {
            eMessageCpy(text, MenuCharNameGet(MenuWork.chrNo));
            eMessageCat(characterHelp.text[1]);
        }
        break;
    case 32:
        if (MenuWork.commandCursor == 0) {
            eMessageCpy(text, commandHelp.text[0]);
            eMessageCat(MenuTextGet((MenuWork.chrNo & 0xFFFF) + 0x60000)->name);
            eMessageCat(commandHelp.text[4]);
        } else {
            eMessageCpy(text, commandHelp.text[MenuWork.commandCursor]);
        }
        break;
    case 48:
        if (MenuWork.listSelect < 0) {
            eMessageCpy(text, etherEmptyText);
        } else {
            eMessageCpy(text, MenuTextGet(MenuSortGet(0, MenuWork.listSelect))->auxiliary);
            if (screen->helpHeld == 1) {
                int status = MenuListGet(0)[MenuWork.listSelect].status;
                if (status == 1) {
                    eMessageCpy(text, acquireHelp.text[5]);
                } else if (status == 2) {
                    eMessageCpy(text, acquireHelp.text[7]);
                }
            }
        }
        break;
    case 50:
        if (MenuWork.listSelect < 0) {
            eMessageCpy(text, etherEmptyText);
        } else {
            eMessageCpy(text, MenuTextGet(MenuSortGet(0, MenuWork.listSelect))->auxiliary);
        }
        break;
    case 80:
        if (MenuWork.listSelect < 0) {
            eMessageCpy(text, etherEmptyText);
        } else {
            eMessageCpy(text, MenuTextGet(MenuSortGet(0, MenuWork.listSelect))->auxiliary);
            if (screen->helpHeld == 1) {
                MenuListEntry *rows = MenuListGet(0);
                if (rows[MenuWork.listSelect].flag == 1 &&
                    rows[MenuWork.listSelect].status == 0) {
                    eMessageCpy(text, capHelp);
                }
            }
        }
        break;
    case 112:
        if (MenuWork.listSelect >= 0) {
            eMessageCpy(text, MenuTextGet(MenuSortGet(0, MenuWork.listSelect))->auxiliary);
            if (screen->helpHeld == 1 &&
                MenuWork.chrNo != MenuEtherWhoCheck(MenuWork.etherId) &&
                MenuListGet(0)[MenuWork.listSelect].flag == 1) {
                eMessageCpy(text, acquireHelp.text[6]);
            }
        } else {
            eMessageCpy(text, etherEmptyText);
        }
        break;
    case 114:
        eMessageCpy(text, actionHelp.text[MenuWork.menuCursor]);
        if (screen->helpHeld == 1) {
            short cost = func_A1A488(MenuWork.etherId)->pointCost;
            int points = func_A19210(MenuWork.chrNo)->points;
            if (MenuWork.menuCursor == 1) {
                cost /= 2;
            }
            if (cost < points) {
                if (MenuEtherJoutoCheck(MenuWork.etherId) == 0) {
                    if (MenuWork.menuCursor == 0) {
                        eMessageCpy(text, acquireHelp.text[3]);
                    } else {
                        eMessageCpy(text, acquireHelp.text[8]);
                    }
                } else if (MenuWork.slotCount == 0) {
                    if (MenuWork.menuCursor == 0) {
                        eMessageCpy(text, acquireHelp.text[3]);
                    } else {
                        eMessageCpy(text, acquireHelp.text[8]);
                    }
                }
            } else {
                eMessageCpy(text, acquireHelp.text[2]);
            }
        }
        break;
    case 116:
        eMessageCpy(text, acquireHelp.text[0]);
        eMessageCat(func_A2C5A8(MenuWork.slotEther)->name);
        eMessageCat(etherPeriodNewlineText);
        eMessageCat(func_A2C5A8(MenuWork.slotEther)->auxiliary);
        if (screen->helpHeld == 1 && func_A19578(MenuWork.chrNo, MenuWork.slotEther) != 0) {
            eMessageCpy(text, acquireHelp.text[4]);
            eMessageCat(func_A2C5A8(MenuWork.slotEther)->name);
            eMessageCat(etherPeriodText);
        }
        break;
    case 118:
    case 120:
        if (MenuWork.state == 118) {
            eMessageCpy(text, acquireHelp.text[0]);
        } else {
            eMessageCpy(text, acquireHelp.text[1]);
        }
        eMessageCat(func_A2C5A8(MenuWork.slotEther)->name);
        if (MenuWork.state == 118) {
            eMessageCat(etherPeriodText);
        } else {
            eMessageCat(etherPeriodEndText);
        }
        break;
    case 130:
        eMessageCpy(text, transferHelp.text[0]);
        eMessageCat(func_A2C5A8(MenuWork.etherId)->name);
        eMessageCat(etherPeriodText);
        if (screen->helpHeld == 1 && func_A19578(MenuWork.targetChrNo, MenuWork.etherId) != 0) {
            eMessageCpy(text, transferHelp.text[5]);
            eMessageCat(func_A2C5A8(MenuWork.etherId)->name);
            eMessageCat(etherPeriodText);
        }
        break;
    case 132:
    case 134:
        {
            short chrNo = MenuEtherParty[MenuWork.targetList[MenuWork.targetCursor]];
            eMessageCpy(text, transferHelp.text[1]);
            if (MenuWork.state == 132) {
                eMessageCat(etherEmptyText);
            } else {
                eMessageCat(etherOfText);
            }
            eMessageCat(func_A2C5A8(MenuWork.etherId)->name);
            if (MenuWork.state == 132) {
                eMessageCat(transferHelp.text[2]);
            } else {
                eMessageCat(transferHelp.text[3]);
            }
            eMessageCat(MenuCharNameGet(chrNo));
            if (MenuWork.state == 132) {
                eMessageCat(etherQuestionText);
            } else {
                eMessageCat(etherCompleteText);
            }
        }
        break;
    default:
        targetY = 512;
        break;
    }
    screen->message.text = text;
    MoveSlide(&screen->window.y, &targetY, 3.0f);
    WindowDXMain(window);
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const char etherEvolveMenuText[] = "Evolve\nTransfer\nCancel";

const char etherUseMenuText[] = "Use\nSet\nUse E.Pts\nCancel";

const char etherConfirmText[] = " Is this okay?";

char etherYesNoText[8] = "Yes\nNo";

char etherMenuTitleText[5] = "Menu";

char etherSelectTitleText[7] = "Select";

void MenuEtherMenuMain(void) {
    EtherMenuScreen *work = MenuEtherMenu;
    short target[2];
    int i;

    switch (work->state) {
    case 0:
        work->color = 0x00FFFFF0;
        WindowDXSet(&work->window[0]);
        work->window[0].x = 528;
        work->window[0].tag_id = etherMenuTitleText;
        work->window[0].style = work->color;
        work->window[0].width = 145;
        work->window[0].close_callback_arg = &work->list[0];
        work->list[0].items = msg00_1_0036DC18[0];
        work->window[0].state = 1;
        work->window[0].y = 192;
        work->window[0].height = 102;
        work->window[0].close_callback = MenuSelectWindow;
        work->list[0].cursor = 0;
        work->list[0].hasPrompt = 0;
        WindowDXMain(&work->window[0]);
        work->window[0].state = 3;
        WindowDXSet(&work->window[1]);
        work->window[1].x = 528;
        work->window[1].tag_id = etherSelectTitleText;
        work->window[1].y = 192;
        work->window[1].style = work->color;
        work->window[1].width = 169;
        work->list[1].items = msg01_2_0036DC20[0];
        work->list[1].prompt = msg01_2_0036DC20[1];
        work->window[1].height = 102;
        work->window[1].close_callback = MenuSelectWindow;
        work->window[1].close_callback_arg = &work->list[1];
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

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
/* JP: ： Set Search
 * EN: : Set Search */
const char etherSetSearchText[20] = "\014\200\200\200\036\000\241\247 Set Search";

/* Text: \x01Wt */
char etherWtText[8] = "\x01Wt";

/* Text: \x1E\x2EPts */
char etherPointsText[8] = "\x01" "E.Pts";

void MenuEtherExMain(void)
{
  EtherDetailWork *screen = MenuEtherEx;
  short rowTargets[2];
  short messageTarget;
  int i;
  int equippedSum;
  EtherUnitRecord *unit;
  char *messageColor;
  switch (screen->state)
  {
    case 0:
      screen->work = 0x00FFFFF0;
      for (i = 0; i < 2; i++)
    {
      eRibbonSet(&screen->rows[i].ribbon, 3);
      screen->rows[i].ribbon.x = -256;
      screen->rows[i].ribbon.work = screen->work;
      screen->rows[i].ribbon.y = 48;
      screen->rows[i].ribbon.width = 256;
      screen->rows[i].ribbon.height = 24;
      eTagFontSet(&screen->rows[i].font, msg00_3_0036DC28[i]);
      screen->rows[i].font.work = screen->work + 2;
      eNumberSet(&screen->rows[i].number, 0);
      screen->rows[i].number.work = screen->work + 2;
    }

      eMessageSet(&screen->message, msg01_4_0036DC30[0]);
      screen->message.mode = 32;
      screen->message.x = 528;
      screen->message.y = 120;
      screen->message.draw_parameter = screen->work;
      screen->state = 2;

    case 2:
      break;

    default:
      return;

  }

  messageTarget = 528;
  rowTargets[1] = -256;
  rowTargets[0] = -256;
  switch (MenuWork.state)
  {
    case 80:
      rowTargets[1] = 0;
      messageTarget = 48;
      break;

    case 32:

    case 112:

    case 114:

    case 116:

    case 118:

    case 120:

    case 128:

    case 130:

    case 132:

    case 134:
      rowTargets[0] = 0;
      break;

  }

  for (i = 0; i < 2; i++)
  {
    /* The row's widget updates use the initialized screen work. */
    EtherDetailWork *renderWork = screen;
    MoveSlide(&screen->rows[i].ribbon.x, &rowTargets[i], 3.0f);
    eRibbonMain(&renderWork->rows[i].ribbon);
    if (i == 0)
    {
      renderWork->rows[i].font.x = (screen->rows[i].ribbon.x + renderWork->rows[i].ribbon.width) - 68;
    }
    else
    {
      screen->rows[i].font.x = (renderWork->rows[i].ribbon.x + screen->rows[i].ribbon.width) - 78;
    }
    renderWork->rows[i].font.y = renderWork->rows[i].ribbon.y + 4;
    eTagFontMain(&screen->rows[i].font);
    screen->rows[i].number.y = screen->rows[i].ribbon.y + 4;
    if (i == 0)
    {
      EtherCharRecord *character = func_A19210(MenuWork.chrNo);
      renderWork->rows[i].number.x = (renderWork->rows[i].ribbon.x + renderWork->rows[i].ribbon.width) - 60;
      screen->rows[i].number.digits = 4;
      renderWork->rows[i].number.format = 0;
      screen->rows[i].number.value = character->points;
    }
    else
    {
      unit = func_A191C0(MenuWork.chrNo);
      renderWork->rows[i].number.x = (screen->rows[i].ribbon.x + renderWork->rows[i].ribbon.width) - 48;
      screen->rows[i].number.digits = 2;
      screen->rows[i].number.format = 3;
      screen->rows[i].number.value = 0x000C0000 + unit->capUsed;
    }
    eNumberMain(&renderWork->rows[i].number);
  }

  equippedSum = 0;
  messageColor = &msg01_4_0036DC30[0][1];
  unit = func_A191C0(MenuWork.chrNo);
  for (i = 0; i < 12; i++)
  {
    equippedSum += unit->etherId[i];
  }

  if (equippedSum == 0)
  {
    messageColor[2] = 64;
    messageColor[1] = 64;
    messageColor[0] = 64;
  }
  else
  {
    messageColor[2] = -128;
    messageColor[1] = -128;
    messageColor[0] = -128;
  }
  MoveSlide(&screen->message.x, &messageTarget, 3.0f);
  eMessageMain(&screen->message);
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const EtherL1R1SpriteIdPair etherL1R1SpriteTable[1] = { { .id = { 274, 272 } } };

const EtherL1R1SlideStep etherL1R1SlideTable[1] = { { .side = { 1, -1 } } };

void MenuEtherL1R1Main(void) {
    EtherL1R1Work *self = MenuEtherL1R1;
    EtherL1R1SpriteIdPair spriteIds;
    signed char *slideOffset;
    int i;

    switch (self->state) {
    case 0:
        spriteIds = etherL1R1SpriteTable[0];
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
            EtherL1R1SlideStep step = etherL1R1SlideTable[0];

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

void MenuEtherStatusMain(void) {
    EtherPartyStatusWorkEarly *screen = MenuEtherStatus;
    short targetX[8];
    short targetY[8];
    short targetWidth[8];
    short targetHeight[8];
    short targetPanelX[8];
    int i;

    switch (screen->header.state) {
    case 0:
        screen->header.color = 0x00F00000;
        {
        for (i = 0; i < MenuWork.partyCount; i++) {
            short chrNo = MenuEtherParty[i];
            int windowX;
            WindowDXSet((WindowDX *)(void *)(screen->window + i));
            windowX = (i & 1) * 816 - 272;
            screen->window[i].x = windowX;
            screen->window[i].y = (i / 2) * 104 + 64;
            screen->window[i].color = screen->header.color;
            screen->window[i].width = 240;
            screen->window[i].height = 82;
            screen->window[i].enabled = 1;
            if (PartyAttackerCheck(chrNo)) {
                screen->window[i].title = MenuTagTextGet(1);
            } else {
                screen->window[i].title = MenuTagTextGet(2);
            }
            screen->window[i].callback = (void *)MenuStatusDisp;
            screen->window[i].callbackArg = screen->panel + i;
            screen->panel[i].offsetX = 0;
            screen->panel[i].chrNo = chrNo;
            screen->panel[i].enabled = 0;
            screen->panel[i].offsetY = 0;
            screen->window[i].state = 1;
            WindowDXMain((WindowDX *)&screen->window[i]);
            screen->window[i].state = 3;
        }
        }
        for (i = 0; i < 1; i++) {
            eCursolSet(&screen->cursor[i], 0);
            screen->cursor[i].state = 0;
            screen->cursor[i].work = 0x00FFFFFF;
        }
        screen->header.active = 0;
        screen->header.collapsed = 0;
        screen->header.resetByte = 0;
        screen->header.verticalFirst = 0;
        {
            unsigned char initialParty = MenuWork.partyCursor;

            screen->header.state = 2;
            screen->header.previousParty = initialParty;
        }
        /* fall through */
    case 2:
    for (i = 0; i < MenuWork.partyCount; i++) {
        targetX[i] = (i & 1) * 816 - 272;
        targetY[i] = (i / 2) * 104 + 64;
        targetWidth[i] = 240;
        targetHeight[i] = 82;
        targetPanelX[i] = 0;
    }

    switch (MenuWork.state) {
    case 16:
        if ((unsigned char)(MenuWork.flags & 1)) {
            screen->header.active = 1;
            screen->cursor[0].state = 32;
            screen->header.verticalFirst = 1;
            if ((unsigned char)(screen->header.collapsed & 1)) {
                if (i != MenuWork.partyCursor) {
                    screen->window[i].y = 24;
                    screen->window[i].x = (i & 1) * 816 - 272;
                }
                screen->header.collapsed &= 0xFE;
            }
        }
        for (i = 0; i < MenuWork.partyCount; i++) {
            targetX[i] = (i & 1) * 240 + 16;
        }
        break;
    case 32:
    case 48:
    case 50:
    case 80:
    case 112:
    case 114:
    case 116:
    case 118:
    case 120:
    case 128:
    case 130:
    case 132:
    case 134:
        if ((unsigned char)(MenuWork.flags & 1)) {
            switch (MenuWork.state) {
            case 32:
                screen->cursor[0].state = 0;
                screen->header.previousParty = MenuWork.partyCursor;
                /* The character selection also refreshes the attacker tags. */
            case 112:
            case 114:
                for (i = 0; i < MenuWork.partyCount; i++) {
                    short chrNo = MenuEtherParty[i];
                    PartyAttackerCheck(chrNo);
                    if (PartyAttackerCheck(chrNo) != 0) {
                        screen->window[i].renderTitle = MenuTagTextGet(1);
                    } else {
                        screen->window[i].renderTitle = MenuTagTextGet(2);
                    }
                }
                break;
            case 48:
                for (i = 0; i < MenuWork.partyCount; i++) {
                    screen->window[i].renderTitle = MenuTagTextGet(5);
                }
                break;
            case 130:
                for (i = 0; i < MenuWork.partyCount; i++) {
                    screen->window[i].renderTitle = MenuTagTextGet(6);
                }
                break;
            }
        }
        if (screen->header.previousParty != MenuWork.partyCursor) {
            screen->window[MenuWork.partyCursor].x = 264;
            screen->window[MenuWork.partyCursor].y = 32;
            screen->header.previousParty = MenuWork.partyCursor;
        }
        if ((unsigned char)(screen->header.collapsed & 1)) {
            for (i = 0; i < MenuWork.partyCount; i++) {
                if (i == MenuWork.partyCursor) {
                    targetX[i] = 264;
                    targetY[i] = 32;
                } else {
                    targetX[i] = (i & 1) * 816 - 272;
                    screen->window[i].x = targetX[i];
                }
            }
        } else {
            for (i = 0; i < MenuWork.partyCount; i++) {
                if (i == MenuWork.partyCursor) {
                    targetX[i] = 264;
                    targetY[i] = 32;
                }
            }
            if (targetY[MenuWork.partyCursor] == screen->window[MenuWork.partyCursor].y) {
                screen->header.collapsed |= 1;
            }
        }
        screen->header.active = 0;
        screen->header.verticalFirst = 0;
        break;
    }

    for (i = 0; i < MenuWork.partyCount; i++) {
        if (screen->header.verticalFirst == 0) {
            MoveSlide(&screen->window[i].width, &targetWidth[i], 3.0f);
            MoveSlide(&screen->window[i].height, &targetHeight[i], 3.0f);
            MoveSlide(&screen->panel[i].offsetX, &targetPanelX[i], 3.0f);
            MoveSlide(&screen->window[i].x, &targetX[i], 3.0f);
            if (screen->window[i].x == targetX[i]) {
                MoveSlide(&screen->window[i].y, &targetY[i], 3.0f);
            }
        } else {
            MoveSlide(&screen->window[i].y, &targetY[i], 3.0f);
            if (screen->window[i].y == targetY[i]) {
                MoveSlide(&screen->window[i].width, &targetWidth[i], 3.0f);
                MoveSlide(&screen->panel[i].offsetX, &targetPanelX[i], 3.0f);
                MoveSlide(&screen->window[i].x, &targetX[i], 3.0f);
            }
        }
        if (i == MenuWork.partyCursor) {
            screen->window[i].color = 0x00F07F00;
        } else {
            screen->window[i].color = 0x00F07E00;
        }
        WindowDXMain((WindowDX *)&screen->window[i]);
    }
    screen->cursor[0].x = screen->window[MenuWork.partyCursor].x + 3;
    {
        signed char yParty = MenuWork.partyCursor;

        screen->cursor[0].work = 0x00FFFFFF;
        screen->cursor[0].y = screen->window[yParty].y + screen->window[yParty].height / 2 - 4;
    }
    eCursolMain(&screen->cursor[0]);
        break;
    }
}

void MenuEtherStatusMain2(void)
{
    EtherTargetStatusWork *screen = MenuEtherStatus2;
    int i;

    if (MenuWork.state == 255) {
        return;
    }
    switch (screen->state) {
    case 0:
        screen->color = 0x00FFF000;
        for (i = 0; i < MenuWork.partyCount; i++) {
            WindowDXSet((WindowDX *)&screen->window[i]);
            screen->window[i].x = -272;
            screen->window[i].width = 240;
            screen->window[i].height = 82;
            screen->window[i].color = screen->color;
            screen->window[i].y = i * 104 + 64;
            screen->window[i].enabled = 1;
            screen->window[i].title = MenuTagTextGet(7);
            screen->window[i].callback =
                MenuStatusDisp;
            screen->window[i].callbackArg = &screen->panel[i];
            screen->panel[i].chrNo = MenuEtherParty[i];
            screen->panel[i].x = 0;
            screen->panel[i].y = 0;
            screen->window[i].state = 1;
            WindowDXMain((WindowDX *)&screen->window[i]);
            screen->window[i].state = 3;
        }
        for (i = 0; i < 3; i++) {
            eCursolSet(&screen->cursor[i], i);
            screen->cursor[i].state = 32;
            screen->cursor[i].work = screen->color + 16;
        }
        for (i = 0; i < 2; i++) {
            screen->frame[0][i].kind = 0x00FFFFFF;
            screen->frame[1][i].kind = 0;
        }
        screen->scrollOffset = 0;
        screen->visibleRow = 0;
        {
            unsigned char initialTargetCursor = MenuWork.targetCursor;

            screen->state = 2;
            screen->previousCursor = initialTargetCursor;
        }
        screen->targetInitialized = 0;
        screen->drawFrame = 0;
        screen->multiCursor = 0;
        screen->listMode = 0;
        screen->scrollLimit = 0;
        screen->visibleCount = 0;
        screen->active = 0;
        break;
    case 2:
        break;
    default:
        return;
    }

    {
        short targetX[8];
        short targetY[8];

        for (i = 0; i < 8; i++) {
            targetX[i] = -272;
            targetY[i] = i * 104 + 64;
        }
        switch (MenuWork.state) {
        case 48:
            screen->active = 1;
            screen->drawFrame = 1;
            screen->visibleCount = MenuWork.partyCount;
            /* fall through */
        case 50:
            if (MenuWork.flags & 1) {
                for (i = 0; i < MenuWork.partyCount; i++) {
                    screen->panel[i].chrNo = MenuEtherParty[i];
                    screen->window[i].renderTitle = MenuTagTextGet(7);
                }
                if (MenuWork.state == 48) {
                    screen->cursor[0].state = 0;
                } else {
                    screen->cursor[0].state = 32;
                }
                {
                    int viewportWidth = 256;
                    MenuEtherWork *frameMenu;

                    screen->frame[0][0].height = 48;
                    frameMenu = &MenuWork;
                    screen->frame[0][0].x = 0;
                    screen->frame[0][1].width = screen->frame[0][0].width = viewportWidth;
                    screen->frame[0][1].y = 354;
                    screen->frame[0][1].height = 158;
                    screen->frame[0][0].y = 0;
                    screen->frame[0][1].x = 0;
                    screen->previousCursor = frameMenu->targetCursor;
                    screen->scrollLimit = frameMenu->partyCount - 3;
                }
            }
            if (screen->previousCursor != MenuWork.targetCursor) {
                if (MenuWork.targetCursor < screen->previousCursor) {
                    if (screen->visibleRow == 0) {
                        screen->scrollOffset--;
                        MenuWork.wait = 32;
                    } else {
                        screen->visibleRow--;
                    }
                } else {
                    if (screen->visibleRow == 2) {
                        screen->scrollOffset++;
                        MenuWork.wait = 32;
                    } else {
                        screen->visibleRow++;
                    }
                }
                screen->previousCursor = MenuWork.targetCursor;
            }
            for (i = 0; i < MenuWork.partyCount; i++) {
                targetX[i] = 16;
                targetY[i] = (i - screen->scrollOffset) * 104 + 64;
            }
            if (screen->window[0].x == targetX[0]) {
                screen->listMode = 1;
            }
            if (screen->window[MenuWork.targetCursor].y == targetY[MenuWork.targetCursor]) {
                if (MenuWork.state == 50 && screen->cursor[0].state == 0) {
                    screen->cursor[0].state = 32;
                }
                MenuWork.wait = 0;
            }
            break;
        case 130:
            if (MenuWork.flags & 1) {
                unsigned char targetsInitialized = screen->targetInitialized;

                screen->drawFrame = 1;
                screen->active = 1;
                screen->visibleCount = MenuWork.targetCount;
                if (targetsInitialized == 0) {
                    for (i = 0; i < MenuWork.targetCount; i++) {
                        screen->panel[i].chrNo = MenuEtherParty[MenuWork.targetList[i]];
                        screen->window[i].renderTitle = MenuTagTextGet(8);
                        screen->window[i].x = -272;
                        screen->window[i].y = i * 100 + 96;
                    }
                    screen->cursor[0].state = 32;
                    {
                        unsigned char targetCursor = MenuWork.targetCursor;

                        screen->targetInitialized = 1;
                        screen->previousCursor = targetCursor;
                    }
                }
                screen->frame[0][0].height = 80;
                screen->frame[0][0].x = 0;
                screen->frame[0][1].width = screen->frame[0][0].width = 256;
                screen->frame[0][1].y = 378;
                screen->frame[0][1].height = 134;
                screen->frame[0][0].y = 0;
                screen->frame[0][1].x = 0;
                screen->scrollLimit = MenuWork.targetCount - 3;
            }
            if (screen->previousCursor != MenuWork.targetCursor) {
                if (MenuWork.targetCursor < screen->previousCursor) {
                    if (screen->visibleRow == 0) {
                        screen->scrollOffset--;
                        MenuWork.wait = 32;
                    } else {
                        screen->visibleRow--;
                    }
                } else {
                    if (screen->visibleRow == 2) {
                        screen->scrollOffset++;
                        MenuWork.wait = 32;
                    } else {
                        screen->visibleRow++;
                    }
                }
                screen->previousCursor = MenuWork.targetCursor;
            }
            for (i = 0; i < MenuWork.targetCount; i++) {
                targetX[i] = 16;
                targetY[i] = (i - screen->scrollOffset) * 100 + 96;
            }
            if (screen->window[0].x == targetX[0]) {
                screen->listMode = 2;
            }
            if (screen->window[MenuWork.targetCursor].y == targetY[MenuWork.targetCursor]) {
                eCursolModeChange(&screen->cursor[0], 32);
                MenuWork.wait = 0;
            }
            break;
        case 132:
            screen->listMode = 0;
            screen->drawFrame = 1;
            /* fall through */
        case 134:
            for (i = 0; i < MenuWork.partyCount; i++) {
                targetY[i] = (i - screen->scrollOffset) * 100 + 96;
            }
            targetX[MenuWork.targetCursor] = 16;
            targetY[MenuWork.targetCursor] = 96;
            break;
        default:
            screen->scrollOffset = 0;
            screen->visibleRow = 0;
            screen->targetInitialized = 0;
            if (screen->window[0].x == targetX[0]) {
                screen->drawFrame = 0;
            }
            screen->listMode = 0;
            break;
        }

        if (screen->active == 0) {
            return;
        }
        if (screen->drawFrame != 0) {
            for (i = 0; i < 2; i++) {
                endPrintExtFunc(screen->color, 12, &screen->frame[0][i]);
            }
            screen->scrollBar.width = 256;
            screen->scrollBar.y = 48;
            screen->scrollBar.x = 0;
            if (screen->listMode == 2) {
                screen->scrollBar.y = 80;
            }
            screen->scrollBar.height = 354 - screen->scrollBar.y;
            endPrintExtFunc(screen->color, 101, &screen->scrollBar);
        }
        for (i = 0; i < screen->visibleCount; i++) {
            short *xTarget = &targetX[i];

            MoveSlide(&screen->window[i].x, xTarget, 3.0f);
            if (screen->window[i].x == *xTarget) {
                MoveSlide(&screen->window[i].y, &targetY[i], 3.0f);
            }
            {
                unsigned short clipOffset = screen->window[i].y + 39;

                if (clipOffset < 415) {
                    WindowDXMain((WindowDX *)&screen->window[i]);
                }
            }
        }
        if (MenuWork.useType == 1 || MenuWork.useType == 3 || MenuWork.state == 130) {
            screen->cursor[0].x = screen->window[MenuWork.targetCursor].x + 3;
            screen->cursor[0].y = screen->window[MenuWork.targetCursor].y +
                                screen->window[MenuWork.targetCursor].height / 2 - 4;
            eCursolMain(&screen->cursor[0]);
            if (screen->scrollLimit > 0 && screen->cursor[0].state != 0 && screen->listMode != 0) {
                if (screen->scrollOffset != 0) {
                    screen->cursor[1].x = 240;
                    if (screen->listMode == 1) {
                        screen->cursor[1].y = 50;
                    } else {
                        screen->cursor[1].y = 82;
                    }
                    eCursolMain(&screen->cursor[1]);
                }
                if ((unsigned char)screen->scrollOffset != (unsigned char)screen->scrollLimit) {
                    screen->cursor[2].x = 240;
                    if (screen->listMode == 1) {
                        screen->cursor[2].y = 338;
                    } else {
                        screen->cursor[2].y = 362;
                    }
                    eCursolMain(&screen->cursor[2]);
                }
            }
        } else if (MenuWork.useType == 2) {
            int count = MenuWork.partyCount;
            if (count >= 4) {
                count = 3;
            }

            if (screen->multiCursor == count - 1) {
                screen->multiCursor = 0;
            } else {
                screen->multiCursor++;
            }
            screen->cursor[0].x = screen->window[screen->multiCursor].x + 3;
            screen->cursor[0].y = screen->multiCursor * 104 + 101;
            eCursolMain(&screen->cursor[0]);
        }
        if (screen->drawFrame != 0) {
            EtherPasBox *nextFrame = screen->frame[1];

            for (i = 0; i < 2; i++) {
                EtherPasBox *callFrame = nextFrame;

                screen->frame[1][i].width = screen->frame[0][i].width;
                screen->frame[1][i].x = screen->frame[0][i].x;
                screen->frame[1][i].y = screen->frame[0][i].y;
                screen->frame[1][i].height = screen->frame[0][i].height;
                nextFrame++;
                endPrintExtFunc(screen->color, 12, callFrame);
            }
            endPrintExtFunc(screen->color, 102, 0);
        }
    }
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
/* Text: List{x 668}Ep */
const char etherListEpTitleText[9] = "List\003\234Ep";

/* Text: List{x 669}Wt */
const char etherListWtTitleText[9] = "List\003\235Wt";

/* Text: List{x 668}E.Pts */
const char etherListEPointsTitleText[12] = "List\003\234E.Pts";

/* Text: List{x 666}E.Pts */
const char etherListSubTitleText[12] = "List\003\232E.Pts";

void MenuEtherListMain(void)
{
    short targetX[2];
    EtherMenuListsWork *screen = MenuEtherList;
    int i;

    switch (screen->state) {
    case 0:
        screen->work = 0x00FFFFF8;
        for (i = 0; i < 2; i++) {
            screen->window[i].style = screen->work;
            screen->window[i].items = 0;
            screen->window[i].x = 528;
            screen->window[i].y = 64;
            WindowSPSet(&screen->window[i]);
        }
        for (i = 0; i < 8; i++) {
            eSpriteSet(&screen->sprite[i], 1546);
            screen->sprite[i].work = 0x00FFFFFF;
        }
        for (i = 0; i < 8; i++) {
            eNumberSet(&screen->number[i], 0);
            screen->number[i].work = 0x00FFFFFA;
        }
        screen->configured = 0;
        screen->state = 2;
        screen->acquireLayout = 0;
        screen->showNumbers = 0;
        break;
    case 2:
        break;
    default:
        return;
    }

    if (screen->acquireLayout == 0) {
        targetX[0] = 528;
        targetX[1] = -262;
    } else {
        targetX[1] = -262;
        targetX[0] = -262;
    }
    switch (MenuWork.state) {
    case 48:
        if ((MenuWork.flags & 1) && screen->configured == 0) {
            WindowSPCursor *window = &screen->window[0];

            screen->window[0].x = 528;
            screen->window[0].y = 144;
            screen->window[0].height = 222;
            screen->window[0].rows = 9;
            screen->window[0].width = 232;
            screen->window[0].title = etherListEpTitleText;
            screen->window[0].columns = 1;
            screen->window[0].items = MenuListGet(0);
            screen->window[0].draw_flags = 7;
            WindowSPItemChange(window);
            WindowSPSetSelect(window, MenuKeepSelect);
            screen->window[0].state = 17;
            WindowSPMain(window);
            screen->configured = 1;
            screen->acquireLayout = 0;
            screen->showNumbers = 0;
        }
        /* fall through */
    case 50:
        targetX[0] = 272;
        break;
    case 80:
        if (MenuWork.flags & 1) {
            WindowSPCursor *window = &screen->window[0];

            screen->window[0].x = 528;
            screen->window[0].y = 160;
            screen->window[0].height = 198;
            screen->window[0].rows = 8;
            screen->window[0].width = 256;
            screen->window[0].title = etherListWtTitleText;
            screen->window[0].columns = 1;
            screen->window[0].items = MenuListGet(0);
            screen->window[0].draw_flags = 3;
            WindowSPItemChange(window);
            WindowSPSetSelect(window, &MenuKeepSelect[5]);
            screen->window[0].state = 17;
            WindowSPMain(window);
            screen->acquireLayout = 0;
            screen->showNumbers = 1;
        }
        targetX[0] = 16;
        break;
    case 112:
        if ((MenuWork.flags & 1) && screen->configured == 0) {
            WindowSPCursor *window = &screen->window[0];

            screen->window[0].x = -262;
            screen->window[0].y = 92;
            screen->window[0].height = 174;
            screen->window[0].width = 246;
            screen->window[0].title = etherListEPointsTitleText;
            screen->window[0].rows = 7;
            screen->window[0].columns = 1;
            screen->window[0].items = MenuListGet(0);
            screen->window[0].draw_flags = 7;
            WindowSPItemChange(window);
            WindowSPSetSelect(window, &MenuKeepSelect[10]);
            screen->window[0].mode = 4;
            screen->window[0].state = 17;
            WindowSPMain(window);
            screen->showNumbers = 0;
            screen->configured = 1;
            screen->acquireLayout = 1;
        }
        /* fall through */
    case 114:
    case 116:
    case 118:
    case 120:
    case 128:
        targetX[0] = 10;
        break;
    case 132: {
        unsigned char initializeList = MenuWork.flags & 1;

        if (initializeList) {
            WindowSPCursor *window = &screen->window[1];
            i = 0;

            screen->window[1].x = -262;
            screen->window[1].y = 196;
            screen->window[1].width = 246;
            screen->window[1].height = 174;
            screen->window[1].draw_flags = 7;
            screen->window[1].columns = 1;
            screen->window[1].rows = 7;
            screen->window[1].title = etherListSubTitleText;
            screen->window[1].items = MenuListGet(1);
            WindowSPItemChange(window);
            screen->window[1].mode = 4;
            screen->window[1].state = 17;
            WindowSPMain(window);
            {
                int *sortRows = MenuSortAddrGet(1);
                int count = MenuSortCheck(1);

                while (i < count) {
                    short rowEther = sortRows[i];
                    signed char targetEther = MenuWork.etherId;

                    if (rowEther == targetEther) {
                        break;
                    }
                    i++;
                }
                WindowSPSelectJump(window, i);
            }
            screen->configured = 2;
        }
    }
        /* fall through */
    case 134:
        targetX[1] = 10;
        break;
    default:
        screen->configured = 0;
        break;
    }
    for (i = 0; i < 2; i++) {
        MoveSlide(&screen->window[i].x, &targetX[i], 3.0f);
        WindowSPMain(&screen->window[i]);
    }
    if (screen->showNumbers != 0) {
        unsigned int iconIndex = 0;
        int displayRow;

        for (displayRow = 0; displayRow < 8; displayRow++) {
            int row = screen->window[0].current_row + displayRow;

            if (row < screen->window[0].item_count) {
                if (screen->window[0].items[row].status != 0) {
                    screen->sprite[iconIndex].x =
                        (screen->window[0].x + screen->window[0].width) - 39;
                    screen->sprite[iconIndex].y =
                        (screen->window[0].y + displayRow * 24) + 3;
                    eSpriteMain(&screen->sprite[iconIndex]);
                    iconIndex++;
                }
                screen->number[displayRow].x =
                    (screen->window[0].x + screen->window[0].width) - 63;
                {
                    unsigned short numberWindowY = screen->window[0].y;

                    screen->number[displayRow].digits = 2;
                    screen->number[displayRow].format = 0;
                    screen->number[displayRow].y = (numberWindowY + displayRow * 24) + 7;
                }
                screen->number[displayRow].value = screen->window[0].items[row].amount;
                eNumberMain(&screen->number[displayRow]);
            }
        }
    }
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const char etherTransferRenderTitle[14] = "TransferEther";

char etherResultTitleText[8] = "Result";

void MenuEtherListMain2(void) {
    EtherResultWork *screen = MenuEtherList2;
    WindowDX *window;
    short targetX;
    short targetY;
    int i;

    switch (screen->state) {
    case 0:
        screen->work = 0x00FFFFF8;
        window = &screen->window;
        WindowDXSet(window);
        screen->window.x = -240;
        screen->window.y = 280;
        screen->window.width = 185;
        screen->window.height = 78;
        screen->window.style = screen->work;
        screen->window.tag_id = etherResultTitleText;
        screen->window.state = 1;
        WindowDXMain(&screen->window);
        screen->window.state = 3;
        for (i = 0; i < 3; i++) {
            eMessageSet(&screen->messages[i], 0);
            screen->messages[i].mode = 32;
            screen->messages[i].draw_parameter = screen->work + 2;
        }
        for (i = 0; i < 3; i++) {
            eNumberSet(&screen->numbers[i], 0);
            screen->numbers[i].work = screen->work + 2;
            screen->numbers[i].format = 0;
            screen->numbers[i].digits = 4;
            screen->numbers[i].value = 0;
        }
        eCursolSet(&screen->cursor, 0);
        screen->cursor.state = 0;
        screen->rightPanel = 0;
        screen->state = 2;
        screen->cursor.work = screen->work + 2;
        break;
    case 2:
        window = &screen->window;
        break;
    default:
        return;
    }

    if (screen->rightPanel == 0) {
        targetX = -243;
        targetY = 288;
    } else {
        targetX = 528;
        targetY = 144;
    }
    switch (MenuWork.state) {
    case 116:
        if ((unsigned char)(MenuWork.flags & 1)) {
            unsigned char *slotEthers;
            screen->cursor.state = 32;
            screen->window.x = -240;
            screen->window.y = 288;
            screen->window.height = 78;
            screen->window.title_font.render.text =etherResultTitleText;
            screen->rightPanel = 0;
            slotEthers = MenuEtherDataGet(MenuSortGet(0, MenuWork.listSelect));
            for (i = 0; i < 3; i++) {
                if (slotEthers[i] != 0) {
                    screen->messages[i].text = func_A2C5A8(slotEthers[i])->name;
                    screen->numbers[i].value = func_A1A488(slotEthers[i])->pointCost;
                    if (func_A19578(MenuWork.chrNo, slotEthers[i]) != 0) {
                        screen->messages[i].color_b = 64;
                        screen->messages[i].color_g = 64;
                        screen->messages[i].color_r = 64;
                    } else {
                        screen->messages[i].color_b = 0x80;
                        screen->messages[i].color_g = 0x80;
                        screen->messages[i].color_r = 0x80;
                    }
                } else {
                    screen->messages[i].text = 0;
                    screen->numbers[i].value = 0;
                }
            }
        }
    case 118:
        targetX = 71;
        break;
    case 130:
        if ((unsigned char)(MenuWork.flags & 1)) {
            screen->rightPanel = 1;
            screen->window.x = 528;
            screen->window.y = 144;
            screen->window.height = 30;
            screen->window.title_font.render.text =etherTransferRenderTitle;
            {
                EtherTextEntry *entry;
                int next;
                signed char normalShade = -128;
                int *firstValue = &screen->numbers[0].value;

                i = 0;
                entry = func_A2C5A8(MenuWork.etherId);
                *firstValue = 0;
                screen->messages[i].text = entry->name;
                screen->messages[i].color_b = normalShade;
                screen->messages[i].color_g = normalShade;
                screen->messages[i].color_r = normalShade;
                for (i = 1; i < 3; i = next) {
                    next = i + 1;
                    screen->messages[i].text = 0;
                    screen->numbers[i].value = 0;
                }
            }
        }
    case 132:
    case 134:
        targetX = 272;
        targetY = 144;
        break;
    default:
        screen->cursor.state = 0;
        break;
    }

    MoveSlide(&window->x, &targetX, 3.0f);
    if (window->x == targetX) {
        MoveSlide(&screen->window.y, &targetY, 3.0f);
    }
    WindowDXMain(window);
    for (i = 0; i < 3; i++) {
        screen->messages[i].x = screen->window.x + 19;
        screen->messages[i].y = screen->window.y + 24 * i + 3;
        eMessageMain(&screen->messages[i]);
    }
    screen->cursor.x = screen->window.x + 3;
    screen->cursor.y = screen->window.y + MenuWork.slotCursor * 24 + 7;
    eCursolMain(&screen->cursor);
}

void MenuEtherListMake00(void) {
    MenuListEntry *rows = MenuListGet(0);
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
    MenuListEntry *rows = MenuListGet(0);
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
    MenuListEntry *rows = MenuListGet(listIndex);
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

void MenuEtherListMake03(int listIndex) {
    MenuListEntry *rows = MenuListGet(listIndex);
    int *sortRow = MenuSortAddrGet(listIndex);
    EtherCharRecord *character = func_A19210(MenuWork.targetChrNo);
    int maskByte = (MenuWork.etherId - 1) / 8;
    int count;
    int i;

    character->etherMask[maskByte] |= 1 << ((MenuWork.etherId - 1) % 8);
    MenuSortSet(listIndex, 128, MenuWork.targetChrNo);
    character->etherMask[maskByte] &= ~(1 << ((MenuWork.etherId - 1) % 8));
    MenuListMake(listIndex, 0);
    count = MenuSortCheck(listIndex);
    for (i = 0; i < count; i++) {
        rows[i].amount = func_A1A488((short)sortRow[i])->pointCost;
        if ((short)sortRow[i] == MenuWork.etherId) {
            rows[i].flag = 1;
        } else {
            rows[i].flag = 0;
        }
    }
}

int MenuEtherEquip(void) {
    MenuListEntry *rows = MenuListGet(0);
    int *sortRow = MenuSortAddrGet(0);
    EtherUnitRecord *unit = func_A191C0(MenuWork.chrNo);
    int slot;

    if (MenuWork.listSelect < 0) {
        return 0;
    }
    if (rows[MenuWork.listSelect].status != 0) {
        for (slot = 0; slot < 12; slot++) {
            if (unit->etherId[slot] == (short)sortRow[MenuWork.listSelect]) {
                unit->etherId[slot] = 0;
                unit->capUsed = unit->capUsed -
                    (unsigned char)rows[MenuWork.listSelect].amount;
                break;
            }
        }
    } else {
        if (rows[MenuWork.listSelect].flag != 0) {
            return 0;
        }
        {
            short *slotId;
            slotId = unit->etherId;
            for (slot = 0; slot < 12; slot++, slotId++) {
                if (unit->etherId[slot] == 0) {
                    __builtin_memcpy(slotId, &sortRow[MenuWork.listSelect],
                                     sizeof(unit->etherId[slot]));
                    unit->capUsed = unit->capUsed +
                        (unsigned char)rows[MenuWork.listSelect].amount;
                    break;
                }
            }
        }
    }
    for (slot = 0; slot < 11; slot++) {
        if (unit->etherId[slot] == 0) {
            int nextSlot;
            for (nextSlot = slot + 1; nextSlot < 12; nextSlot++) {
                if (unit->etherId[nextSlot] != 0) {
                    unit->etherId[slot] = unit->etherId[nextSlot];
                    unit->etherId[nextSlot] = 0;
                    break;
                }
            }
            if (nextSlot == 12) {
                break;
            }
        }
    }
    return 1;
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const char etherTreePath[32] = "data\\endou\\ether\\etree.bin";

/*
 * The ether menu. The first call loads etree.bin, lays out the sub-screen
 * work blocks after it and builds the party list; every later call advances
 * the MenuWork.state machine and runs the sub-screens.
 */
void MenuEther(void)
{
    if (MenuWork.state == 0) {
        int heap;
        PartyAttackPosition *slot;
        int i;
        int id;

        heap = (int)MainMenuWorkEnd;
        MenuEtherDataBuf = (unsigned char (*)[4])((heap + 0x7FF) & ~0x7FF);
        xglCdReadFile(etherTreePath, MenuEtherDataBuf, 0, 1);
        heap = (int)MenuEtherDataBuf + ETHER_DATA_SIZE;
        MenuEtherPas = (EtherPasWork *)heap;
        heap = (int)MenuEtherPas + ETHER_PAS_SIZE;
        MenuEtherPas->state = 0;
        MenuEtherInfo = (EtherInfoWork *)heap;
        heap = (int)MenuEtherInfo + ETHER_INFO_SIZE;
        MenuEtherInfo->state = 0;
        MenuEtherStatus = (EtherPartyStatusWorkEarly *)heap;
        heap = (int)MenuEtherStatus + ETHER_STATUS_SIZE;
        MenuEtherStatus->header.state = 0;
        MenuEtherStatus2 = (EtherTargetStatusWork *)heap;
        heap = (int)MenuEtherStatus2 + ETHER_STATUS2_SIZE;
        MenuEtherStatus2->state = 0;
        MenuEtherMenu = (EtherMenuScreen *)heap;
        heap = (int)MenuEtherMenu + ETHER_MENU_SIZE;
        MenuEtherMenu->state = 0;
        MenuEtherList = (EtherMenuListsWork *)heap;
        heap = (int)MenuEtherList + ETHER_LIST_SIZE;
        MenuEtherList->state = 0;
        MenuEtherList2 = (EtherResultWork *)heap;
        heap = (int)MenuEtherList2 + ETHER_LIST2_SIZE;
        MenuEtherList2->state = 0;
        MenuEtherEx = (EtherDetailWork *)heap;
        heap = (int)MenuEtherEx + ETHER_EX_SIZE;
        MenuEtherEx->state = 0;
        MenuEtherL1R1 = (EtherL1R1Work *)heap;
        heap = (int)MenuEtherL1R1 + ETHER_L1R1_SIZE;
        MenuEtherL1R1->state = 0;
        MenuWorkEndCheck(EtherTreeInit(heap, MenuEtherDataBuf));
        MenuWork.nextState = 0x10;
        MenuWork.wait = 12;
        MenuWork.flags = 0;
        MenuWork.partyCursor = 0;
        MenuWork.partyCount = 0;
        MenuEtherCapSet();
        MenuKeepSelectReset();
        slot = PartyDataGet()->attack_slots;
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
        MenuWork.listSelect = WindowSPSelect(&MenuEtherList->window[0], PadData.repeat);
        WindowSPKeepSelect(&MenuEtherList->window[0], &MenuKeepSelect[0]);
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
                PartyDataPrefix *party = PartyDataGet();

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
            MenuWork.listSelect = WindowSPSelect(&MenuEtherList->window[0], PadData.repeat);
            WindowSPKeepSelect(&MenuEtherList->window[0], &MenuKeepSelect[5]);
            if ((PadData.pressed & PAD_SQUARE) && MenuWork.listSelect >= 0) {
                int start = MenuWork.listSelect + 1;
                int count = MenuSortCheck(0);
                MenuListEntry *rows = MenuListGet(0);
                int i;
                int row = 0;

                for (i = 0; i < count; i++) {
                    row = (start + i) % count;
                    if (rows[row].status != 0) {
                        break;
                    }
                }
                WindowSPSelectJump(&MenuEtherList->window[0], row);
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
        MenuWork.listSelect = WindowSPSelect(&MenuEtherList->window[0], 0);
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
            MenuWork.listSelect = WindowSPSelect(&MenuEtherList->window[0], PadData.repeat);
            WindowSPKeepSelect(&MenuEtherList->window[0], &MenuKeepSelect[10]);
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
        MenuWork.listSelect = WindowSPSelect(&MenuEtherList->window[0], 0);
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
                character->points -= func_A1A488(MenuWork.etherId)->pointCost;
                MenuSortSet(0, 0x80, MenuWork.chrNo);
                MenuEtherListMake02(0);
                WindowSPItemChange(&MenuEtherList->window[0]);
                MenuEtherList->window[0].mode = 4;
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
                MenuListEntry *rows;
                int count;
                int i;

                func_A194E0(MenuWork.targetChrNo, MenuWork.etherId);
                character = func_A19210(MenuWork.chrNo);
                character->points -= func_A1A488(MenuWork.etherId)->pointCost / 2;
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
    /* Text: {reset}ether */
    xglFontDebugPrintf(0, 0x40, "\vether");
    /* Text: {reset}ms %2d */
    xglFontDebugPrintf(0, 0x50, "\vms %2d", MenuWork.commandCursor);
    /* Text: {reset}ms2%2d */
    xglFontDebugPrintf(0, 0x58, "\vms2%2d", MenuWork.menuCursor);
    /* Text: {reset}cs %2d/%2d */
    xglFontDebugPrintf(0, 0x60, "\vcs %2d/%2d", MenuWork.partyCursor, MenuWork.partyCount);
    /* Text: {reset}cs2%2d */
    xglFontDebugPrintf(0, 0x68, "\vcs2%2d", MenuWork.targetCursor);
    /* Text: {reset}lst%2d */
    xglFontDebugPrintf(0, 0x70, "\vlst%2d", MenuWork.listSelect);
    /* Text: {reset}yn %2d */
    xglFontDebugPrintf(0, 0x78, "\vyn %2d", MenuWork.confirmCursor);
    /* Text: {reset}um %2d */
    xglFontDebugPrintf(0, 0x80, "\vum %2d", MenuWork.slotCursor);
    /* Text: {reset}umm%2d */
    xglFontDebugPrintf(0, 0x88, "\vumm%2d", MenuWork.slotCount);
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
unsigned char (*MenuEtherDataBuf)[4] = 0;

EtherPasWork *MenuEtherPas = 0;

EtherInfoWork *MenuEtherInfo = 0;

EtherMenuScreen *MenuEtherMenu = 0;

EtherDetailWork *MenuEtherEx = 0;

EtherL1R1Work *MenuEtherL1R1 = 0;

EtherPartyStatusWorkEarly *MenuEtherStatus = 0;

EtherTargetStatusWork *MenuEtherStatus2 = 0;

EtherMenuListsWork *MenuEtherList = 0;

EtherResultWork *MenuEtherList2 = 0;

const char MenuTecUseTpText[11] = "/Use T.Pts";

const char MenuTecAttacksText[13] = "Tech Attacks";

const char MenuTecSetText[5] = "/Set";
