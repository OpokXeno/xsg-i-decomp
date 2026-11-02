#include "common.h"
#include "main/e_battle_win_open.h"
#include "main/e_message.h"
#include "main/menu_top_command_check.h"
#include "main/menu_1.h"
#include "main/party.h"
#include "main/window_tex_load.h"
#include "shared.h"
#include "main/e_number_main.h"
#include "main/end_print.h"
#include "main/menu_para_pt_rate_get.h"
#include "main/open_close_main.h"
#include "main/xgl_dma.h"

#define PAD_L1 0x0004
#define PAD_R1 0x0008

typedef struct MenuTecInfoDetail {
    short x;
    short y;
    unsigned char unmodeled_04[4];
    int visible;
    char *message;
} MenuTecInfoDetail;

typedef struct MenuTecInfoWork {
    unsigned char state;
    signed char active;
    signed char showCharacterHelp;
    unsigned char showActionHelp;
    int color;
    WindowDX window;
    unsigned char unmodeled_19c[4];
    MenuTecInfoDetail detail;
    unsigned char unmodeled_1b0[0x350 - 0x1b0];
    char message[0x80];
} MenuTecInfoWork;

typedef struct MenuTecData {
    unsigned char unmodeled_00[8];
    unsigned char levelPointIncrement[8];
    unsigned short levelPointBase[8];
    unsigned short speedPointCost[8];
    unsigned short waitPointCost[8];
} MenuTecData;

typedef struct MenuTecEquipRecord {
    unsigned char unmodeled_00[0x82];
    short techniqueId[6]; /* +0x82 */
} MenuTecEquipRecord;


typedef struct MenuTecCharTechnique {
    short levelAdjustment;
    short waitAdjustment;
    short useCount;
    short targetCount;
    unsigned char unmodeled_08[4];
} MenuTecCharTechnique;

typedef struct MenuTecCharacter {
    unsigned char unmodeled_00[0x0c];
    int points;
    unsigned char unmodeled_10[0x3c];
    MenuTecCharTechnique technique[8];
} MenuTecCharacter;

typedef struct MenuTecPasWork MenuTecPasWork;

typedef struct MenuTecMenuWork MenuTecMenuWork;

typedef struct MenuTecL1R1Work MenuTecL1R1Work;

typedef struct MenuTecListWork MenuTecListWork;

typedef struct MenuTecSetWinWork MenuTecSetWinWork;


typedef struct MenuTecPasBox {
    short x;                            /* +0x00 */
    short y;                            /* +0x02 */
    int color;                          /* +0x04 */
    short width;                        /* +0x08 */
    short height;                       /* +0x0A */
} MenuTecPasBox;

struct MenuTecPasWork {
    unsigned char state;                /* +0x000 */
    unsigned char unmodeled_01[3];
    int color;                          /* +0x004 */
    WindowDX window;            /* +0x008 */
    EMessageParam message[3];           /* +0x19C */
    unsigned char unmodeled_268[0x334 - 0x268];
    MenuTecPasBox box;                  /* +0x334 */
    unsigned char callbackWork[0x10];   /* +0x340 */
};


typedef struct MenuTecSetWinRow {
    ETagFontMainData label[2];      /* +0x00: the row name, then its status text */
    ENumberMainData number[2];      /* +0x40: the current level, then the next cost */
} MenuTecSetWinRow;

struct MenuTecSetWinWork {
    unsigned char state;                /* +0x000 */
    unsigned char flags;
    unsigned char unmodeled_02[2];
    int color;                          /* +0x004 */
    WindowDX window;             /* +0x008 */
    MenuTecSetWinRow row[3];            /* +0x19C */
    ETagFontMainData footer;            /* +0x5BC */
    unsigned char unmodeled_5dc[0x61C - 0x5DC];
    ECursolMainData cursor;             /* +0x61C */
};

typedef struct MenuTecList {
    unsigned char unmodeled_00[0x02];
    unsigned short hasPrompt;           /* +0x02 */
    int cursor;                         /* +0x04 */
    const char *items;                  /* +0x08 */
    const char *prompt;                 /* +0x0C */
    unsigned char unmodeled_10[0x5F8 - 0x10];
} MenuTecSelectList;

struct MenuTecMenuWork {
    unsigned char state;                /* +0x000 */
    unsigned char unmodeled_01[3];
    int color;                          /* +0x004 */
    WindowDX window[2];          /* +0x008 */
    MenuTecSelectList list[2];          /* +0x330 */
};

typedef struct MenuTecL1R1Sprite {
    unsigned char unmodeled_00[0x04];
    short x;    /* +0x04 */
    short y;    /* +0x06 */
    int work;   /* +0x08 */
    unsigned char unmodeled_0c[0x28 - 0x0c];
} MenuTecL1R1Sprite;

struct MenuTecL1R1Work {
    unsigned char state;         /* +0x00 */
    unsigned char unmodeled_01;
    signed char slideOffset[2];  /* +0x02: added into sprite[i].x while non-zero */
    int spriteWork;              /* +0x04: shared initial value for both sprites' work */
    MenuTecL1R1Sprite sprite[2]; /* +0x08: L1, then R1 */
};

struct MenuTecListWork {
    unsigned char state;                /* +0x00 */
    unsigned char unmodeled_01;
    unsigned char valid;                /* +0x02 */
    unsigned char side;                 /* +0x03 */
    int color;                          /* +0x04 */
    WindowSPCursor window;              /* +0x08 */
};

typedef struct MenuTecL1R1SpriteIds {
    short id[2];
} MenuTecL1R1SpriteIds;

typedef struct MenuTecL1R1SlideStep {
    signed char side[2];
} MenuTecL1R1SlideStep;

typedef struct MenuTecExRow {
    ERibbonMainData ribbon;
    ETagFontMainData label;
    ENumberMainData number;
} MenuTecExRow;

typedef struct MenuTecExWork {
    unsigned char state;
    unsigned char unmodeled_01[3];
    unsigned int drawOrder;
    MenuTecExRow row[1];
    /* MenuTec's constructor advances 0x248 bytes past this allocation. */
    unsigned char unmodeled_128[0x248 - 0x128];
} MenuTecExWork;

struct MenuStatusPanelWindow;

struct MenuStatusPanel;

typedef struct MenuTecStatusHeader {
    unsigned char state;
    unsigned char mode;
    unsigned char unmodeled_02[2];
    unsigned char verticalFirst;
    unsigned char compact;
    unsigned char transition;
    signed char previousCursor;
    int color;
} MenuTecStatusHeader;

typedef struct MenuTecStatusRecord {
    signed char characterNo;
    unsigned char unmodeled_01;
    unsigned char unmodeled_02[2];
    short offsetX;
    short offsetY;
    unsigned char unmodeled_08[0x834 - 8];
} MenuTecStatusRecord;

typedef struct MenuTecStatusWindow {
    short x;
    short y;
    int color;
    short width;
    short height;
    const char *title;
    unsigned char state;
    unsigned char ribbonInitialized;
    unsigned char unmodeled_12[2];
    void (*display)(WindowDX *, void *);
    MenuTecStatusRecord *displayData;
    unsigned char unmodeled_1c[0x188 - 0x1c];
} MenuTecStatusWindow;

typedef struct MenuTecStatusWindowEntry {
    MenuTecStatusHeader header;
    MenuTecStatusWindow window;
} MenuTecStatusWindowEntry;

typedef struct MenuTecStatusRecordEntry {
    MenuTecStatusHeader header;
    MenuTecStatusRecord record;
} MenuTecStatusRecordEntry;

typedef struct MenuTecStatusCursorEntry {
    MenuTecStatusHeader header;
    ECursolMainData cursor;
} MenuTecStatusCursorEntry;

typedef struct MenuTecStatusWork {
    MenuTecStatusWindowEntry window[8];
    MenuTecStatusRecordEntry detail[8];
    MenuTecStatusCursorEntry cursor[2];
    MenuTecStatusHeader endHeader;
} MenuTecStatusWork;

typedef struct MenuTecList2Row {
    WindowDX window;
    EMessageParam techniqueName[4];
    EMessageParam equipmentName[4];
    MenuTecL1R1Sprite icon[8];
    unsigned char unmodeled_4f4[0x6b4 - 0x4f4];
    MenuTecL1R1Sprite marker[4];
} MenuTecList2Row;

typedef struct MenuTecList2Work {
    unsigned char state;
    unsigned char mode;
    unsigned char unmodeled_02[2];
    int color;
    MenuTecList2Row row[2];
    WindowDX detailWindow;
    EMessageParam detailLabel[2];
    ENumberMainData detailNumber[2];
    ECursolMainData cursor;
} MenuTecList2Work;

typedef struct MenuTecList2SpriteIds {
    short id[10];
} MenuTecList2SpriteIds;

typedef struct MenuTecList2MessageIds {
    int id[6];
} MenuTecList2MessageIds;

extern signed char nextMenuTable[];
extern char tecLabel[];
extern char msFormat[];
extern char sliFormat[];
extern char lstFormat[];
extern char sctFormat[];
extern MenuTecInfoWork *MenuTecInfo;
extern unsigned char *MainMenuWorkEnd;
extern const char tecDataPath[];
extern MenuTecData *MenuTecDataBuf;
extern MenuWorkState MenuWork;
extern MenuTecPasWork *MenuTecPas;
extern MenuTecMenuWork *MenuTecMenu;
extern MenuTecL1R1Work *MenuTecL1R1;
extern MenuTecListWork *MenuTecList;
extern MenuTecSetWinWork *MenuTecSetWin;
static const char * msg00_1_0036DC78[] ;
static const char * msg01_2_0036DC80[] ;
static const char * msg02_9[] ;
static unsigned char MenuTecTrgTbl00[] ;
static unsigned char MenuTecTrgTbl01[] ;
extern char infoPeriodText[];
extern char infoSentenceEndText[];
extern MenuTecExWork *MenuTecEx;
static const char * msg00_3_0036DC88[] ;
extern MenuTecStatusWork *MenuTecStatus;
extern unsigned char MenuKeepSelect[];
extern const char techListTitleText[];
extern const char listTitleText[];
extern const MenuTecList2MessageIds list2MessageIdTable;
extern const MenuTecList2SpriteIds list2SpriteIdTable;
extern MenuTecList2Work *MenuTecList2;
extern const char setListHiTitleText[];
extern const char setListLwTitleText[];
static const char * msg01_5[] ;
extern const char MenuTecAttacksText[13];
extern const char MenuTecUseTpText[11];
extern const char MenuTecSetText[5];
extern const char techSetMenuText[24];
extern const char confirmYesNoText[8];
extern const char techConfirmQuestionText[24];
extern const char techPointsText[8];
extern const char timesUsedText[16];
extern const char spiritsAbsorbedText[24];
extern const char numberDefeatedText[16];
extern const char nearText[8];
extern const char farText[8];
extern const char waitText[8];
extern const char techText[8];
extern const char speedText[8];
extern const char upOkText[8];
extern const char completeText[16];
extern const char dashLineText[8];
extern const char waitLevelA8Text[8];
extern const char waitLevelA4Text[8];
extern const char waitLevelA6Text[8];
extern const char waitLevelA9Text[8];
extern const char waitLevelA7Text[8];
extern const char waitLevelA0Text[8];
extern const char waitLevelA3Text[8];
extern const char waitLevelA5Text[8];
extern const char waitLevelA1Text[8];
extern const char waitLevelA2Text[8];

/* Original objects referenced by this TU's initialized DATA. */
extern const char speedLimitedValue[];

extern const char speedUnlimitedValue[];

void WindowSPSet(WindowSPCursor *cursor);
void WindowSPMain(WindowSPCursor *cursor);
void WindowSPSelectJump(WindowSPCursor *cursor, int selection);
void *MenuTecDataGet(int chrNo);
TecInitEntry *MenuTecSaveDataGet(int chrNo);
int MenuTecNextSpeedPointGet(int chrNo, int point);

/* dataPlChaGet (ov01/data_unit_org_get.c VA 0x00a19210, still INCLUDE_ASM
 * there and not yet linkable by that name): returns the running character's
 * control record, whose word at +0xC this function decrements. Kept under
 * the scaffold's undefined_funcs_auto.txt placeholder name until that TU
 * recovers it. */
MenuTecCharacter *func_A19210(int chrNo);

int MenuTecTLevLimitCheck(int chrNo, int point);
int MenuTecNextTLevPointGet(int chrNo, int point);
/* dataUnitOrgGet (ov01/data_unit_org_get.c VA 0x00a191c0, still INCLUDE_ASM
 * there and not yet linkable by that name): returns the running character's
 * parameter record (the full record is src/main/menu_para_pt_rate_get.c's
 * CharParaData); MenuTecEquipCheck only reads the six equipped technique ids
 * at +0x82. Kept under the scaffold's undefined_funcs_auto.txt placeholder
 * name until that TU recovers it. */
MenuTecEquipRecord *func_A191C0(int chrNo);

/* dataTecGet (ov01/data_unit_org_get.c VA 0x00a1a378, still INCLUDE_ASM there
 * and not yet linkable by that name): returns a pointer to the technique's
 * metadata record, or a NULL pointer plus a diagnostic print for a
 * non-positive index. Kept under the scaffold's undefined_funcs_auto.txt
 * placeholder name until that TU recovers it. */
unsigned char *func_A1A378(int tecBit);

/* Returns whether either of the technique's two low target-metadata bits is
 * set (elf_names annotation). */
int BitToTecNo(int bit);

int MenuTecNextWaitPointGet(int chrNo, int point);
struct MenuListEntry *MenuListGet(int list);
struct MenuListEntry *MenuListMake(int list, int mode);
int MenuSortCheck(int list);
void eSpriteSet(void *sprite, short spriteId);
void eSpriteMain(void *sprite);
int MenuTecSpeedLimitCheck(int chrNo, int point);
int MenuTecWaitLimitCheck(int chrNo, int point);
int MenuTecCharTLevUpCheck(void);
int MenuTecCharSpeedUpCheck(void);
int MenuTecCharWaitUpCheck(void);
int func_A197E8(int chrNo, int tecNo);
void *MenuSortAddrGet(int list);
int MenuSortGet(int listIndex, int entryIndex);
char *MenuCharNameGet(int characterNo);
char *MenuNumberTextGet(long long value, int width, int mode);
int PartyAttackerCheck(int characterNo);
const char *MenuTagTextGet(int tag);

static const char * msg_0_0036DC68[3];

static const char * msg00_7[3];

static const char * msg01_8_0036DCB8[3];

static const char * msg03_10[10];

static const char * msg00_4_0036DC90[3];

static const char * msg_0_0036DC68[3] = { MenuTecAttacksText, MenuTecSetText, MenuTecUseTpText };

static const char * msg00_1_0036DC78[] = { techSetMenuText };

static const char * msg01_2_0036DC80[] = { confirmYesNoText, techConfirmQuestionText };

static const char * msg00_3_0036DC88[] = { techPointsText };

static const char * msg00_4_0036DC90[3] = { timesUsedText, numberDefeatedText, spiritsAbsorbedText };

static const char * msg01_5[] = { nearText, farText };

static const char * msg00_7[3] = { techText, speedText, waitText };

static const char * msg01_8_0036DCB8[3] = { upOkText, completeText, dashLineText };

static const char * msg03_10[10] = { waitLevelA0Text, waitLevelA1Text, waitLevelA2Text, waitLevelA3Text, waitLevelA4Text, waitLevelA5Text, waitLevelA6Text, waitLevelA7Text, waitLevelA8Text, waitLevelA9Text };

static const char * msg02_9[] = { speedLimitedValue, speedUnlimitedValue };

static unsigned char MenuTecTrgTbl00[] = { 0, 1, 0, 0, 1, 0 };

static unsigned char MenuTecTrgTbl01[] = { 1, 1, 1, 1, 1, 1 };

static short MenuTecParty[8];

TecInitEntry *MenuTecSaveDataGet(int chrNo)
{
    int characterIndex = chrNo & 0xffff;
    PartyDataPrefix *party = PartyDataGet();
    TecInitEntry *records = party->tech_records;

    if ((unsigned int)(characterIndex - 1) >= 8U)
        return 0;
    return &records[characterIndex - 1];
}

void *MenuTecDataGet(int characterNo)
{
    unsigned int characterIndex = characterNo & 0xFFFF;
    MenuTecData *records = MenuTecDataBuf;

    if (characterIndex - 1 >= 8U)
        return 0;

    return &records[characterIndex - 1];
}

int BitToTecNo(int bit)
{
    int techniqueBit = bit & 0xffff;

    if (techniqueBit >= 45 && techniqueBit <= 52)
        return techniqueBit - 45;
    if (techniqueBit >= 53 && techniqueBit <= 59)
        return techniqueBit - 53;
    if (techniqueBit >= 60 && techniqueBit <= 67)
        return techniqueBit - 60;
    if (techniqueBit >= 68 && techniqueBit <= 75)
        return techniqueBit - 68;
    if (techniqueBit >= 76 && techniqueBit <= 82)
        return techniqueBit - 76;
    if (techniqueBit >= 83 && techniqueBit <= 90)
        return techniqueBit - 83;
    if (techniqueBit >= 91 && techniqueBit <= 98)
        return techniqueBit - 91;
    return -1;
}

int MenuTecNextTLevPointGet(int chrNo, int point)
{
    MenuTecData *data = MenuTecDataGet(chrNo);
    TecInitEntry *saveData = MenuTecSaveDataGet(chrNo);

    if (point < 0)
        return 0;
    return data->levelPointBase[point]
         + data->levelPointIncrement[point] * (saveData->level[point] + 1);
}

int MenuTecTLevLimitCheck(int chrNo, int point)
{
    TecInitEntry *saveData = MenuTecSaveDataGet(chrNo);

    if (point < 0)
        return 0;
    return saveData->level[point] < saveData->levelLimit[point];
}

void MenuTecTLevUp(int chrNo, int point)
{
    int maskedChrNo = chrNo & 0xffff;
    TecInitEntry *saveData = MenuTecSaveDataGet(maskedChrNo);
    MenuTecCharacter *character = func_A19210(maskedChrNo);

    if (point >= 0) {
        int points = MenuTecNextTLevPointGet(chrNo, point);

        saveData->level[point]++;
        character->technique[point].levelAdjustment = saveData->level[point] * 3 - 3;
        character->points -= points;
    }
}

int MenuTecNextSpeedPointGet(int chrNo, int point)
{
    MenuTecData *data = MenuTecDataGet(chrNo);

    if (point < 0)
        return 0;

    return data->speedPointCost[point];
}

int MenuTecSpeedLimitCheck(int chrNo, int point)
{
    TecInitEntry *saveData = MenuTecSaveDataGet(chrNo);

    if (point < 0)
        return 0;

    return saveData->speed[point] != 1;
}

void MenuTecSpeedUp(int chrNo, int point)
{
    int maskedChrNo = chrNo & 0xFFFF;
    TecInitEntry *saveData = MenuTecSaveDataGet(maskedChrNo);
    MenuTecCharacter *unit = func_A19210(maskedChrNo);

    if (point >= 0) {
        int points = MenuTecNextSpeedPointGet(chrNo, point);

        saveData->speed[point] = 1;
        unit->points -= points;
    }
}

int MenuTecNextWaitPointGet(int chrNo, int point)
{
    MenuTecData *data = MenuTecDataGet(chrNo);

    if (point < 0)
        return 0;

    return data->waitPointCost[point];
}

int MenuTecWaitLimitCheck(int chrNo, int point)
{
    TecInitEntry *saveData = MenuTecSaveDataGet(chrNo);

    if (point < 0) {
        return 0;
    }
    return saveData->wait[point] != 0;
}

void MenuTecWaitUp(int chrNo, int point)
{
    int maskedChrNo = chrNo & 0xffff;
    TecInitEntry *saveData = MenuTecSaveDataGet(chrNo);
    MenuTecCharacter *character = func_A19210(maskedChrNo);

    if (point >= 0) {
        int points = MenuTecNextWaitPointGet(chrNo, point);

        saveData->wait[point]--;
        character->technique[point].waitAdjustment = saveData->wait[point];
        character->points -= points;
    }
}

int MenuTecCharTLevUpCheck(void)
{
    MenuTecCharacter *characterData;
    int nextPoint;

    if (MenuTecTLevLimitCheck(MenuWork.characterNo, MenuWork.point) != 0) {
        characterData = func_A19210(MenuWork.characterNo);
        nextPoint = MenuTecNextTLevPointGet(MenuWork.characterNo, MenuWork.point);
        if (characterData->points >= nextPoint)
            return 1;
    }

    return 0;
}

int MenuTecCharSpeedUpCheck(void)
{
    MenuTecCharacter *characterData;
    int nextPoint;

    if (MenuTecSpeedLimitCheck(MenuWork.characterNo, MenuWork.point) != 0) {
        characterData = func_A19210(MenuWork.characterNo);
        nextPoint = MenuTecNextSpeedPointGet(MenuWork.characterNo, MenuWork.point);
        if (characterData->points >= nextPoint)
            return 1;
    }
    return 0;
}

int MenuTecCharWaitUpCheck(void)
{
    MenuTecCharacter *characterData;
    int nextPoint;

    if (MenuTecWaitLimitCheck(MenuWork.characterNo, MenuWork.point) != 0) {
        characterData = func_A19210(MenuWork.characterNo);
        nextPoint = MenuTecNextWaitPointGet(MenuWork.characterNo, MenuWork.point);
        if (characterData->points >= nextPoint)
            return 1;
    }
    return 0;
}

void MenuTecPasMain(void)
{
    MenuTecPasWork *pas = MenuTecPas;
    int tabLength[3];
    short tabTarget[3];
    short slideTarget;
    int i;

    for (i = 0; i < 3; i++)
        tabLength[i] = MenuPasLengthGet(msg_0_0036DC68[i]);

    switch (pas->state) {
    case 0:
        pas->color = 0x00FFFFF0;
        WindowDXSet(&pas->window);
        pas->window.x = -288;
        pas->window.y = 8;
        pas->window.style = pas->color;
        pas->window.width = 272;
        pas->window.height = 30;
        pas->window.close_callback = MenuPasWindow;
        pas->window.close_callback_arg = pas->callbackWork;
        pas->window.state = 1;
        WindowDXMain(&pas->window);
        pas->window.state = 3;
        for (i = 0; i < 3; i++) {
            eMessageSet(&pas->message[i], msg_0_0036DC68[i]);
            pas->message[i].mode = 0x20;
            pas->message[i].x = 288;
            pas->message[i].y = 11;
            pas->message[i].draw_parameter = pas->color + 2;
        }
        pas->state = 2;
        /* fallthrough */
    case 2:
        break;
    default:
        return;
    }

    slideTarget = -16;
    for (i = 0; i < 3; i++)
        tabTarget[i] = 288;

    switch (MenuWork.menu_state[3]) {
    case 0x10:
    case 0x20:
        tabTarget[0] = 16;
        break;
    case 0x30:
    case 0x32:
    case 0x50:
    case 0x52:
    case 0x54:
    case 0x56:
        tabTarget[0] = 16;
        tabTarget[MenuWork.listCursor + 1] = tabLength[0] + 16;
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
        pas->box.color = pas->color;
        pas->box.width = pas->window.width - 6;
        pas->box.height = pas->window.height - 6;
        endPrintExtFunc(pas->color, 0x65, &pas->box);
        for (i = 0; i < 3; i++) {
            MoveSlide(&pas->message[i].x, &tabTarget[i], 5.0f);
            if (pas->message[i].x < 256)
                eMessageMain(&pas->message[i]);
        }
        endPrintExtFunc(pas->color, 0x66, 0);
    }
}

void MenuTecInfoMain(void)
{
    MenuTecInfoWork **workSlot = &MenuTecInfo;
    const char *characterHelp[] = {
        "Choose a character.",
        " cannot be chosen."
    };
    const char *menuHelp[] = {
        "Set Tech Attack. \nNote: Attacks must be set to use.",
        "Use T.Pts to improve Tech Attacks.",
        "Cancel."
    };
    const char *upgradeHelp[] = {
        "TECH",
        "SPEED",
        "WAIT",
        "XX's level",
        /* Text: is now XX.\x1F */
        "is now XX.\037"
    };
    const char *limitHelp[] = {
        "Level can be increased.",
        "\n(Increases attack power)",
        "\n(Enables Tech Attack to be set in HI Slot)",
        "\n(Recover faster after using Tech Attack)",
        "You do not have enough T.Pts.",
        " cannot be improved any further."
    };
    const char *pointHelp[] = {
        "XX",
        " will be changed to ",
        "HI",
        "LW",
        " is now "
    };
    const char *waitHelp[] = {
        /* JP: ０
         * EN: 0 */
        "\243\260",
        /* JP: １
         * EN: 1 */
        "\243\261",
        /* JP: ２
         * EN: 2 */
        "\243\262",
        /* JP: ３
         * EN: 3 */
        "\243\263",
        /* JP: ４
         * EN: 4 */
        "\243\264",
        /* JP: ５
         * EN: 5 */
        "\243\265",
        /* JP: ６
         * EN: 6 */
        "\243\266",
        /* JP: ７
         * EN: 7 */
        "\243\267",
        /* JP: ８
         * EN: 8 */
        "\243\270",
        /* JP: ９
         * EN: 9 */
        "\243\271",
        /* JP: Ａ
         * EN: A */
        "\243\301"
    };
    const char *listHelp[] = {
        "Set a HI SPEED Tech Attack in the HI Slot.",
        "Set a LW or HI SPEED Tech Attack in the LW Slot."
    };
    const char *equipmentHelp[] = {
        "Because this Tech Attack's SPEED is LW,\nit cannot be set in this slot.",
        "Because this Tech Attack's SPEED is LW\nand it conflicts with the slot range,\nit cannot be set in this slot.",
        "Because this Tech Attack conflicts with the slot range, it cannot be set here."
    };
    short targetY;
    unsigned char wait;
    MenuTecInfoWork *work = *workSlot;

    switch (work->state) {
    case 0:
        work->color = 0x00fffff0;
        WindowDXSet(&work->window);
        work->window.x = -16;
        work->window.style = work->color;
        work->window.y = 480;
        work->window.width = 544;
        work->window.height = 54;
        work->window.close_callback_arg = &work->detail;
        work->window.close_callback = MenuInfoWindow;
        work->detail.message = 0;
        work->window.state = 1;
        WindowDXMain(&work->window);
        work->window.state = 3;
        work->active = 1;
        work->showCharacterHelp = 0;
        work->showActionHelp = 0;
        memset(work->message, 0, sizeof(work->message));
        work->state = 2;
    case 2:
        targetY = 386;
        if (work->showCharacterHelp == 0) {
            if (PadData.half_2a & 0x20)
                work->showCharacterHelp = 1;
        } else if (PadData.mappedRepeat != 0) {
            work->showCharacterHelp = 0;
        }
        switch (MenuWork.menu_state[3]) {
        case 0x10:
            eMessageCpy(work->message, characterHelp[0]);
            if (work->showCharacterHelp && MenuWork.characterHelp != 0) {
                eMessageCpy(work->message, MenuCharNameGet(MenuWork.characterNo));
                eMessageCat(characterHelp[1]);
            }
            break;
        case 0x20:
            eMessageCpy(work->message, menuHelp[MenuWork.listCursor]);
            break;
        case 0x30:
            if (MenuWork.mode < 2)
                eMessageCpy(work->message, listHelp[0]);
            else
                eMessageCpy(work->message, listHelp[1]);
            break;
        case 0x32:
            if (MenuWork.selectedTechnique >= 0) {
                if (work->showActionHelp == 0) {
                    eMessageCpy(work->message,
                                MenuTextGet(MenuSortGet(0, MenuWork.selectedTechnique))->auxiliary);
                    if (MenuWork.mode < 2) {
                        if (MenuWork.resetFlags & 8) {
                            if (MenuWork.resetFlags & 4)
                                eMessageCpy(work->message, equipmentHelp[1]);
                            else
                                eMessageCpy(work->message, equipmentHelp[0]);
                            work->showActionHelp = 1;
                        } else if (MenuWork.resetFlags & 4) {
                            eMessageCpy(work->message, equipmentHelp[2]);
                            work->showActionHelp = 1;
                        }
                    } else if (MenuWork.resetFlags & 4) {
                        eMessageCpy(work->message, equipmentHelp[2]);
                        work->showActionHelp = 1;
                    }
                } else if (PadData.mappedRepeat & 0xfffd) {
                    work->showActionHelp = 0;
                }
            }
            break;
        case 0x50:
            if (MenuWork.selectedTechnique >= 0) {
                if (work->showActionHelp == 0) {
                    if (PadData.half_2a & 0x20) {
                        if (MenuTecCharTLevUpCheck() || MenuTecCharSpeedUpCheck() ||
                            MenuTecCharWaitUpCheck()) {
                            eMessageCpy(work->message,
                                        MenuTextGet(MenuSortGet(0, MenuWork.selectedTechnique))->auxiliary);
                        } else {
                            if (MenuTecTLevLimitCheck(MenuWork.characterNo, MenuWork.point) ||
                                MenuTecSpeedLimitCheck(MenuWork.characterNo, MenuWork.point) ||
                                MenuTecWaitLimitCheck(MenuWork.characterNo, MenuWork.point)) {
                                eMessageCpy(work->message, limitHelp[4]);
                                work->showActionHelp = 1;
                            } else {
                                eMessageCpy(work->message,
                                            MenuTextGet(MenuSortGet(0, MenuWork.selectedTechnique))->name);
                                eMessageCat(limitHelp[5]);
                                work->showActionHelp = 1;
                            }
                        }
                    } else {
                        eMessageCpy(work->message,
                                    MenuTextGet(MenuSortGet(0, MenuWork.selectedTechnique))->auxiliary);
                    }
                } else if (PadData.mappedRepeat & 0xfffd) {
                    work->showActionHelp = 0;
                }
            }
            break;
        case 0x52:
            switch (MenuWork.rowCursor) {
            case 0:
                if (MenuTecTLevLimitCheck(MenuWork.characterNo, MenuWork.point)) {
                    if (MenuTecCharTLevUpCheck()) {
                        eMessageCpy(work->message, limitHelp[0]);
                        eMessageCat(limitHelp[1]);
                    } else {
                        eMessageCpy(work->message, limitHelp[4]);
                    }
                } else {
                    eMessageCpy(work->message, upgradeHelp[0]);
                    eMessageCat(limitHelp[5]);
                }
                break;
            case 1:
                if (MenuTecSpeedLimitCheck(MenuWork.characterNo, MenuWork.point)) {
                    if (MenuTecCharSpeedUpCheck()) {
                        eMessageCpy(work->message, limitHelp[0]);
                        eMessageCat(limitHelp[2]);
                    } else {
                        eMessageCpy(work->message, limitHelp[4]);
                    }
                } else {
                    eMessageCpy(work->message, upgradeHelp[1]);
                    eMessageCat(limitHelp[5]);
                }
                break;
            case 2:
                if (MenuTecWaitLimitCheck(MenuWork.characterNo, MenuWork.point)) {
                    if (MenuTecCharWaitUpCheck()) {
                        eMessageCpy(work->message, limitHelp[0]);
                        eMessageCat(limitHelp[3]);
                    } else {
                        eMessageCpy(work->message, limitHelp[4]);
                    }
                } else {
                    eMessageCpy(work->message, upgradeHelp[2]);
                    eMessageCat(limitHelp[5]);
                }
                break;
            }
            break;
        case 0x54:
        case 0x56:
            if (MenuWork.resetFlags & 1) {
                int improved;
                long long level;
                int selected = 1;
                improved = MenuWork.menu_state[3] == 0x54 ? selected : 0;
                eMessageCpy(work->message, upgradeHelp[MenuWork.rowCursor]);
                eMessageCat(pointHelp[1]);
                switch (MenuWork.rowCursor) {
                case 0:
                    level = (long long)improved + MenuTecSaveDataGet(MenuWork.characterNo)->level[MenuWork.point];
                    eMessageCat(MenuNumberTextGet(level, -1, 0));
                    break;
                case 1:
                    eMessageCat(pointHelp[2]);
                    break;
                case 2:
                    wait = MenuTecSaveDataGet(MenuWork.characterNo)->wait[MenuWork.point];
                    eMessageCat(waitHelp[10]);
                    eMessageCat(waitHelp[wait - improved]);
                    break;
                }
                if (MenuWork.menu_state[3] == 0x54)
                    eMessageCat(infoPeriodText);
                else
                    eMessageCat(infoSentenceEndText);
            }
            break;
        default:
            targetY = 512;
            break;
        }
        work->detail.message = work->message;
        MoveSlide(&work->window.y, &targetY, 3.0f);
        WindowDXMain(&work->window);
        break;
    }
}

void MenuTecMenuMain(void)
{
    MenuTecMenuWork *work = MenuTecMenu;
    short target[2];
    int i;

    switch (work->state) {
    case 0: {
        work->color = 0x00FFFFF0;
        WindowDXSet(&work->window[0]);
        work->window[0].x = 528;
        work->window[0].y = 192;
        work->window[0].style = work->color;
        work->window[0].width = 193;
        work->window[0].height = 78;
        work->window[0].tag_id = "Menu";
        work->window[0].close_callback = MenuSelectWindow;
        work->window[0].close_callback_arg = &work->list[0];
        work->list[0].cursor = 0;
        work->list[0].hasPrompt = 0;
        work->list[0].items = msg00_1_0036DC78[0];
        work->window[0].state = 1;
        WindowDXMain(&work->window[0]);
        work->window[0].state = 3;
        WindowDXSet(&work->window[1]);
        work->window[1].x = 528;
        work->window[1].y = 240;
        work->window[1].style = work->color;
        work->window[1].width = 169;
        work->window[1].height = 102;
        work->window[1].tag_id = "Select";
        work->window[1].close_callback = MenuSelectWindow;
        work->window[1].close_callback_arg = &work->list[1];
        work->list[1].cursor = 0;
        work->list[1].hasPrompt = 1;
        work->list[1].items = msg01_2_0036DC80[0];
        work->list[1].prompt = msg01_2_0036DC80[1];
        work->window[1].state = 1;
        WindowDXMain(&work->window[1]);
        work->window[1].state = 3;
        work->state = 2;
    }
        /* fallthrough */
    case 2:
        target[0] = target[1] = 528;
        switch (MenuWork.menu_state[3]) {
        case 0x20:
            if (MenuWork.resetFlags & 1) {
                work->window[0].x = 528;
                work->list[0].cursor = 0;
            }
            target[0] = 288;
            work->list[0].cursor = MenuWork.listCursor;
            break;
        case 0x54:
            if (MenuWork.resetFlags & 1) {
                work->window[1].x = 528;
                work->list[1].cursor = 0;
            }
            target[1] = 288;
            work->list[1].cursor = MenuWork.selectCursor;
            break;
        }
        for (i = 0; i < 2; i++) {
            MoveSlide(&work->window[i].x, &target[i], 3.0f);
            WindowDXMain(&work->window[i]);
        }
        break;
    }
}

void MenuTecExMain(void)
{
    MenuTecExWork *work = MenuTecEx;
    int i;

    switch (work->state) {
    case 0: {
        const char **labelWords = msg00_3_0036DC88;
        unsigned char digitCount = 4;

        work->drawOrder = 0x00fffff0;
        for (i = 0; i < 1; i++) {
            short *position = &work->row[i].ribbon.x;
            short *dimensions = &work->row[i].ribbon.width;

            eRibbonSet(&work->row[i].ribbon, 3);
            position[0] = -256;
            position[1] = 48;
            work->row[i].ribbon.work = work->drawOrder;
            dimensions[0] = 256;
            dimensions[1] = 24;
            eTagFontSet(&work->row[i].label, labelWords[i]);
            work->row[i].label.work = work->drawOrder + 2;
            eNumberSet(&work->row[i].number, 0);
            work->row[i].number.work = work->drawOrder + 2;
            work->row[i].number.digits = digitCount;
            work->row[i].number.format = 0;
            work->row[i].number.value = 0;
        }
        work->state = 2;
        break;
    }
    case 2:
        break;
    default:
        return;
    }

    {
        short target[1];

        target[0] = -256;
        switch (MenuWork.menu_state[3]) {
        case 0x20:
        case 0x30:
        case 0x32:
        case 0x50:
        case 0x52:
        case 0x54:
        case 0x56: {
            MenuTecCharacter *character = func_A19210(MenuWork.characterNo);

            work->row[0].number.value = character->points;
            target[0] = 0;
            break;
        }
        }
        for (i = 0; i < 1; i++) {
            MoveSlide(&work->row[i].ribbon.x, &target[i], 3.0f);
            eRibbonMain(&work->row[i].ribbon);
            work->row[i].label.x = work->row[i].ribbon.x + work->row[i].ribbon.width - 68;
            work->row[i].label.y = work->row[i].ribbon.y + 4;
            eTagFontMain(&work->row[i].label);
            work->row[i].number.x = work->row[i].ribbon.x + work->row[i].ribbon.width - 60;
            work->row[i].number.y = work->row[i].ribbon.y + 4;
            eNumberMain(&work->row[i].number);
        }
    }
}

void MenuTecL1R1Main(void)
{
    MenuTecL1R1Work *self = MenuTecL1R1;
    signed char *slideOffset;
    int i;

    switch (self->state) {
    case 0: {
        MenuTecL1R1SpriteIds spriteIds = { { 0x112, 0x110 } };

        self->spriteWork = 0x00FFFFFF;
        for (i = 0; i < 2; i++) {
            eSpriteSet(&self->sprite[i], spriteIds.id[i]);
            self->sprite[i].work = self->spriteWork;
            self->sprite[i].y = 140;
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

        target[0] = -45;
        target[1] = 528;
        if (MenuWork.menu_state[3] == 0x20) {
            target[0] = 8;
            target[1] = 475;
            if (PadData.half_2a & PAD_L1) {
                self->slideOffset[0] = -6;
            } else if (PadData.half_2a & PAD_R1) {
                self->slideOffset[1] = 6;
            }
        }

        {
            MenuTecL1R1SlideStep step = { { 1, -1 } };

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

void MenuTecStatusMain(void)
{
    MenuTecStatusWork *work = MenuTecStatus;
    short targetX[8];
    short targetY[8];
    short targetWidth[8];
    short targetHeight[8];
    short targetDetail[8];
    int i;

    switch (work->window[0].header.state) {
    case 0:
        work->window[0].header.color = 0x00fff000;
        for (i = 0; i < MenuWork.rosterCount; i++) {
            short characterNo = MenuTecParty[i];

            WindowDXSet((WindowDX *)(void *)(&work->window[i].window));
            work->window[i].window.x = (i & 1) * 816 - 272;
            work->window[i].window.color = work->window[0].header.color;
            work->window[i].window.y = i / 2 * 104 + 64;
            work->window[i].window.width = 240;
            work->window[i].window.height = 82;
            work->window[i].window.ribbonInitialized = 1;
            if (PartyAttackerCheck(characterNo))
                work->window[i].window.title = MenuTagTextGet(1);
            else
                work->window[i].window.title = MenuTagTextGet(2);
            work->window[i].window.display = MenuStatusDisp;
            work->window[i].window.displayData = &work->detail[i].record;
            work->detail[i].record.offsetX = 0;
            work->detail[i].record.characterNo = characterNo;
            work->detail[i].record.unmodeled_01 = 0;
            work->detail[i].record.offsetY = 0;
            work->window[i].window.state = 1;
            WindowDXMain((WindowDX *)(void *)(&work->window[i].window));
            work->window[i].window.state = 3;
        }
        for (i = 0; i < 1; i++) {
            eCursolSet(&work->cursor[i].cursor, 0);
            work->cursor[i].cursor.state = 0;
            work->cursor[i].cursor.work = 0x00ffffff;
        }
        work->window[0].header.mode = 0;
        work->window[0].header.compact = 0;
        work->window[0].header.transition = 0;
        work->window[0].header.verticalFirst = 0;
        work->window[0].header.previousCursor = MenuWork.rosterCursor;
        work->window[0].header.state = 2;
        /* fallthrough */
    case 2:
        for (i = 0; i < MenuWork.rosterCount; i++) {
            targetX[i] = (i & 1) * 816 - 272;
            targetY[i] = i / 2 * 104 + 64;
            targetWidth[i] = 240;
            targetHeight[i] = 82;
            targetDetail[i] = 0;
        }
        switch (MenuWork.menu_state[3]) {
        case 0x10:
            if (MenuWork.resetFlags & 1) {
                work->window[0].header.mode = 1;
                work->cursor[0].cursor.state = 0x20;
                work->window[0].header.verticalFirst = 1;
                if (work->window[0].header.compact & 1) {
                    if (i != MenuWork.rosterCursor) {
                        work->window[i].window.y = 24;
                        work->window[i].window.x = (i & 1) * 816 - 272;
                    }
                    work->window[0].header.compact &= ~1;
                }
            }
            for (i = 0; i < MenuWork.rosterCount; i++)
                targetX[i] = (i & 1) * 240 + 16;
            break;
        case 0x20:
        case 0x30:
        case 0x32:
        case 0x50:
        case 0x52:
        case 0x54:
        case 0x56:
            if (MenuWork.resetFlags & 1) {
                if (MenuWork.menu_state[3] == 0x20) {
                    work->cursor[0].cursor.state = 0;
                    work->window[0].header.previousCursor = MenuWork.rosterCursor;
                }
            }
            if (work->window[0].header.previousCursor != MenuWork.rosterCursor) {
                work->window[MenuWork.rosterCursor].window.x = 264;
                work->window[MenuWork.rosterCursor].window.y = 32;
                work->window[0].header.previousCursor = MenuWork.rosterCursor;
            }
            if (work->window[0].header.compact & 1) {
                for (i = 0; i < MenuWork.rosterCount; i++) {
                    if (i == MenuWork.rosterCursor) {
                        targetX[i] = 264;
                        targetY[i] = 32;
                    } else {
                        targetX[i] = (i & 1) * 816 - 272;
                        work->window[i].window.x = (i & 1) * 816 - 272;
                    }
                }
            } else {
                for (i = 0; i < MenuWork.rosterCount; i++) {
                    if (i == MenuWork.rosterCursor) {
                        targetX[i] = 264;
                        targetY[i] = 32;
                    }
                }
                if (targetY[MenuWork.rosterCursor] ==
                    work->window[MenuWork.rosterCursor].window.y)
                    work->window[0].header.compact |= 1;
            }
            work->window[0].header.mode = 0;
            work->window[0].header.verticalFirst = 0;
            break;
        }
        for (i = 0; i < MenuWork.rosterCount; i++) {
            if (!work->window[0].header.verticalFirst) {
                MoveSlide(&work->window[i].window.width, &targetWidth[i], 3.0f);
                MoveSlide(&work->window[i].window.height, &targetHeight[i], 3.0f);
                MoveSlide(&work->detail[i].record.offsetX, &targetDetail[i], 3.0f);
                MoveSlide(&work->window[i].window.x, &targetX[i], 3.0f);
                if (work->window[i].window.x == targetX[i])
                    MoveSlide(&work->window[i].window.y, &targetY[i], 3.0f);
            } else {
                MoveSlide(&work->window[i].window.y, &targetY[i], 3.0f);
                if (work->window[i].window.y == targetY[i]) {
                    MoveSlide(&work->window[i].window.width, &targetWidth[i], 3.0f);
                    MoveSlide(&work->detail[i].record.offsetX, &targetDetail[i], 3.0f);
                    MoveSlide(&work->window[i].window.x, &targetX[i], 3.0f);
                }
            }
            if (i == MenuWork.rosterCursor)
                work->window[i].window.color = 0x7f00;
            else
                work->window[i].window.color = 0x7e00;
            WindowDXMain((WindowDX *)(void *)(&work->window[i].window));
        }
        work->cursor[0].cursor.x = work->window[MenuWork.rosterCursor].window.x + 3;
        work->cursor[0].cursor.y = work->window[MenuWork.rosterCursor].window.y +
                         work->window[MenuWork.rosterCursor].window.height / 2 - 4;
        work->cursor[0].cursor.work = 0x00ffffff;
        eCursolMain(&work->cursor[0].cursor);
        break;
    }
}

void MenuTecListMain(void)
{
    short target[1];
    MenuTecListWork *work = MenuTecList;
    unsigned char state = work->state;

    switch (state) {
    case 0:
        work->color = 0x00fffff8;
        work->window.style = work->color;
        work->window.x = 528;
        work->window.y = 144;
        work->window.draw_flags = 7;
        work->window.rows = 8;
        work->window.height = 198;
        work->window.title = 0;
        work->window.columns = 1;
        work->window.items = 0;
        WindowSPSet(&work->window);
        work->state = 2;
        work->valid = 1;
        /* fallthrough */
    case 2: {
        if (!work->side)
            target[0] = 528;
        else
            target[0] = -224;
        switch (MenuWork.menu_state[3]) {
        case 0x30:
            if (MenuWork.resetFlags & 1) {
                if (!work->valid) {
                    MenuTecListWork *const *list = &work;
                    WindowSPCursor *window = &(*list)->window;

                    work->window.width = 216;
                    work->window.height = 126;
                    work->window.rows = 5;
                    work->window.draw_flags = 7;
                    work->window.x = 528;
                    work->window.title = techListTitleText;
                    work->window.columns = 1;
                    work->window.items = MenuListGet(0);
                    WindowSPItemChange(window);
                    WindowSPSetSelect(window, MenuKeepSelect);
                    work->window.mode = 1;
                    window->state = 17;
                    WindowSPMain(window);
                    work->valid = 1;
                }
                work->side = 0;
                work->window.draw_flags &= ~1;
            }
            target[0] = 288;
            break;
        case 0x32:
            if (MenuWork.resetFlags & 1)
                work->window.draw_flags |= 1;
            target[0] = 288;
            break;
        case 0x50:
            if (MenuWork.resetFlags & 1) {
                if (!work->valid) {
                    MenuTecListWork *const *list = &work;
                    WindowSPCursor *window = &(*list)->window;

                    work->window.width = 200;
                    work->window.height = 198;
                    work->window.rows = 8;
                    work->window.x = -224;
                    work->window.draw_flags = 3;
                    work->window.title = listTitleText;
                    work->window.columns = 1;
                    work->window.items = MenuListGet(0);
                    WindowSPItemChange(window);
                    WindowSPSetSelect(window, &MenuKeepSelect[5]);
                    work->window.mode = 1;
                    window->state = 17;
                    WindowSPMain(window);
                    work->valid = 1;
                }
                work->side = 1;
            }
            /* fallthrough */
        case 0x52:
        case 0x54:
        case 0x56:
            target[0] = 32;
            break;
        default:
            work->valid = 0;
            break;
        }
        MoveSlide(&work->window.x, target, 3.0f);
        WindowSPMain(&work->window);
        break;
    }
    default:
        break;
    }
}

void MenuTecListMain2(void)
{
    int i;
    int j;
    MenuTecList2Work *work = MenuTecList2;

    void subMenuTecListChange(void)
    {
        MenuTecEquipRecord *equipment = func_A191C0(MenuWork.characterNo);

        if (MenuWork.characterNo == 7) {
            for (i = 0; i < 2; i++) {
                for (j = 0; j < i * 2 + 2; j++) {
                    work->row[i].equipmentName[j].text = msg01_5[1];
                }
            }
        } else {
            MenuTecList2MessageIds messageIds = list2MessageIdTable;

            for (i = 0; i < 2; i++) {
                for (j = 0; j < i * 2 + 2; j++) {
                    work->row[i].equipmentName[j].text =
                        msg01_5[messageIds.id[j + i * 2]];
                }
            }
        }
        for (i = 0; i < 2; i++) {
            for (j = 0; j < i * 2 + 2; j++) {
                short techniqueId = equipment->techniqueId[j + i * 2];

                if (techniqueId) {
                    work->row[i].techniqueName[j].text =
                        MenuTextGet(0x000a0000 + (unsigned short)techniqueId)->name;
                } else {
                    work->row[i].techniqueName[j].text = 0;
                }
            }
        }
    }

    MenuTecList2SpriteIds spriteIds;
    short targetX[8];
    short targetY[8];
    short targetDetail;

    switch (work->state) {
    case 0:
        work->color = 0x00fffff8;
        for (i = 0; i < 2; i++) {
            int positionY;
            int rowHeight;

            WindowDXSet(&work->row[i].window);
            positionY = i * 68 + 192;
            rowHeight = i * 48 + 54;
            work->row[i].window.x = -272;
            work->row[i].window.y = positionY;
            work->row[i].window.style = work->color;
            work->row[i].window.width = 285;
            work->row[i].window.height = rowHeight;
            if (i == 0)
                work->row[i].window.tag_id = setListHiTitleText;
            else
                work->row[i].window.tag_id = setListLwTitleText;
            work->row[i].window.state = 1;
            WindowDXMain(&work->row[i].window);
            work->row[i].window.state = 3;
            for (j = 0; j < 4; j++) {
                eMessageSet(&work->row[i].techniqueName[j], 0);
                work->row[i].techniqueName[j].mode = 0x20;
                work->row[i].techniqueName[j].draw_parameter = work->color + 2;
                eMessageSet(&work->row[i].equipmentName[j], 0);
                work->row[i].equipmentName[j].mode = 0x20;
                work->row[i].equipmentName[j].draw_parameter = work->color + 2;
            }
            {
                spriteIds = list2SpriteIdTable;

                for (j = 0; j < i * 6 + 2; j++) {
                    eSpriteSet(&work->row[i].icon[j], spriteIds.id[j + i * 2]);
                    work->row[i].icon[j].work = work->color + 2;
                }
            }
            for (j = 0; j < 4; j++) {
                eSpriteSet(&work->row[i].marker[j], 0x60b);
                work->row[i].marker[j].work = work->color + 1;
            }
        }
        WindowDXSet(&work->detailWindow);
        {
            MenuTecList2Work *detailWork = work;

            detailWork->detailWindow.x = -272;
            detailWork->detailWindow.y = 314;
            detailWork->detailWindow.style = detailWork->color;
            detailWork->detailWindow.width = 285;
            detailWork->detailWindow.height = 54;
            detailWork->detailWindow.state = 1;
        }
        WindowDXMain(&work->detailWindow);
        work->detailWindow.state = 3;
        for (i = 0; i < 2; i++) {
            eMessageSet(&work->detailLabel[i], 0);
            work->detailLabel[i].mode = 0x20;
            work->detailLabel[i].draw_parameter = work->color + 2;
        }
        for (i = 0; i < 2; i++) {
            eNumberSet(&work->detailNumber[i], 0);
            work->detailNumber[i].work = work->color + 2;
            work->detailNumber[i].format = 0;
            work->detailNumber[i].digits = 5;
            work->detailNumber[i].value = 0;
        }
        eCursolSet(&work->cursor, 0);
        work->cursor.state = 0;
        {
            MenuTecList2Work *cursorWork = work;

            cursorWork->mode = 0;
            cursorWork->cursor.work = cursorWork->color + 2;
        }
        work->state = 2;
        /* fallthrough */
    case 2:
        for (i = 0; i < 2; i++) {
            targetX[i] = -304;
            targetY[i] = i * 68 + 192;
        }
        targetDetail = -304;
        switch (MenuWork.menu_state[3]) {
        case 0x20:
            targetX[1] = 0;
            targetX[0] = 0;
            work->cursor.state = 0;
            subMenuTecListChange();
            break;
        case 0x30:
            if (MenuWork.resetFlags & 1)
                eCursolModeChange(&work->cursor, 0x20);
            /* fallthrough */
        case 0x32: {
            MenuTecCharacter *character = func_A19210(MenuWork.characterNo);

            if (MenuWork.point >= 0) {
                work->detailNumber[0].value =
                    character->technique[MenuWork.point].useCount;
                work->detailNumber[1].value =
                    character->technique[MenuWork.point].targetCount;
            } else {
                work->detailNumber[0].value = -1;
                work->detailNumber[1].value = -1;
            }
            targetY[0] = 144;
            targetY[1] = 212;
            targetX[1] = 0;
            targetX[0] = 0;
            targetDetail = 0;
            subMenuTecListChange();
            break;
        }
        default:
            work->cursor.state = 0;
            break;
        }
        for (i = 0; i < 2; i++) {
            MoveSlide(&work->row[i].window.x, &targetX[i], 3.0f);
            MoveSlide(&work->row[i].window.y, &targetY[i], 3.0f);
            WindowDXMain(&work->row[i].window);
            if (i == 0) {
                for (j = 0; j < 2; j++) {
                    work->row[i].marker[j].x = work->row[i].window.x + 115;
                    work->row[i].marker[j].y = work->row[i].window.y + j * 24 + 3;
                    eSpriteMain(&work->row[i].marker[j]);
                    work->row[i].equipmentName[j].x = work->row[i].window.x + 67;
                    work->row[i].equipmentName[j].y = work->row[i].window.y + j * 24 + 3;
                    eMessageMain(&work->row[i].equipmentName[j]);
                    work->row[i].techniqueName[j].x = work->row[i].window.x + 123;
                    work->row[i].techniqueName[j].y = work->row[i].window.y + j * 24 + 3;
                    eMessageMain(&work->row[i].techniqueName[j]);
                    work->row[i].icon[j].x = work->row[i].window.x + 19;
                    work->row[i].icon[j].y = work->row[i].window.y + j * 24 + 3;
                    eSpriteMain(&work->row[i].icon[j]);
                }
            } else {
                for (j = 0; j < 4; j++) {
                    work->row[i].marker[j].x = work->row[i].window.x + 115;
                    work->row[i].marker[j].y = work->row[i].window.y + j * 24 + 3;
                    eSpriteMain(&work->row[i].marker[j]);
                    work->row[i].equipmentName[j].x = work->row[i].window.x + 67;
                    work->row[i].equipmentName[j].y = work->row[i].window.y + j * 24 + 3;
                    eMessageMain(&work->row[i].equipmentName[j]);
                    work->row[i].techniqueName[j].x = work->row[i].window.x + 123;
                    work->row[i].techniqueName[j].y = work->row[i].window.y + j * 24 + 3;
                    eMessageMain(&work->row[i].techniqueName[j]);
                }
                for (j = 0; j < 8; j++) {
                    work->row[i].icon[j].x = work->row[i].window.x + (j & 1) * 24 + 19;
                    work->row[i].icon[j].y = work->row[i].window.y + j / 2 * 24 + 3;
                    eSpriteMain(&work->row[i].icon[j]);
                }
            }
        }
        MoveSlide(&work->detailWindow.x, &targetDetail, 3.0f);
        WindowDXMain(&work->detailWindow);
        work->detailLabel[0].text = msg00_4_0036DC90[0];
        work->detailLabel[0].x = work->detailWindow.x + 80;
        if (MenuWork.tecBit == 0x61) {
            work->detailLabel[1].x = work->detailWindow.x + 23;
            work->detailLabel[1].text = msg00_4_0036DC90[2];
        } else {
            work->detailLabel[1].x = work->detailWindow.x + 36;
            work->detailLabel[1].text = msg00_4_0036DC90[1];
        }
        for (j = 0; j < 2; j++) {
            work->detailLabel[j].y = work->detailWindow.y + j * 24 + 3;
            eMessageMain(&work->detailLabel[j]);
        }
        for (j = 0; j < 2; j++) {
            work->detailNumber[j].x = work->detailWindow.x + 215;
            work->detailNumber[j].y = work->detailWindow.y + j * 24 + 7;
            eNumberMain(&work->detailNumber[j]);
        }
        work->cursor.x = work->row[0].window.x + 3;
        if (MenuWork.mode < 2)
            work->cursor.y = work->row[0].window.y + MenuWork.mode * 24 + 7;
        else
            work->cursor.y = work->row[1].window.y + MenuWork.mode * 24 - 41;
        eCursolMain(&work->cursor);
        break;
    }
}

void MenuTecSetWinMain(void)
{
    MenuTecSetWinWork *work = MenuTecSetWin;
    int i;
    short slide;
    int j;

    /* Fills the three rows from the technique under the cursor. */
    void subListChange(void)
    {
        work->row[0].number[0].value = MenuTecSaveDataGet(MenuWork.characterNo)->level[MenuWork.point];
        work->row[1].number[0].format = 10;
        if (MenuTecSpeedLimitCheck(MenuWork.characterNo, MenuWork.point) != 0) {
            work->row[1].number[0].value = (int)msg02_9[0];
        } else {
            work->row[1].number[0].value = (int)msg02_9[1];
        }
        work->row[2].number[0].format = 10;
        work->row[2].number[0].value = (int)msg03_10[MenuTecSaveDataGet(MenuWork.characterNo)->wait[MenuWork.point]];
        if (MenuTecTLevLimitCheck(MenuWork.characterNo, MenuWork.point) != 0) {
            work->row[0].number[1].format = 0;
            work->row[0].number[1].value = MenuTecNextTLevPointGet(MenuWork.characterNo, MenuWork.point);
        } else {
            work->row[0].number[1].format = 9;
        }
        if (MenuTecSpeedLimitCheck(MenuWork.characterNo, MenuWork.point) != 0) {
            work->row[1].number[1].format = 0;
            work->row[1].number[1].value = MenuTecNextSpeedPointGet(MenuWork.characterNo, MenuWork.point);
        } else {
            work->row[1].number[1].format = 9;
        }
        if (MenuTecWaitLimitCheck(MenuWork.characterNo, MenuWork.point) != 0) {
            work->row[2].number[1].format = 0;
            work->row[2].number[1].value = MenuTecNextWaitPointGet(MenuWork.characterNo, MenuWork.point);
        } else {
            work->row[2].number[1].format = 9;
        }
        if (MenuTecTLevLimitCheck(MenuWork.characterNo, MenuWork.point) != 0 && MenuTecCharTLevUpCheck() != 0) {
            work->row[0].label[1].render.text = msg01_8_0036DCB8[0];
        } else {
            work->row[0].label[1].render.text = 0;
        }
        if (MenuTecSpeedLimitCheck(MenuWork.characterNo, MenuWork.point) != 0 && MenuTecCharSpeedUpCheck() != 0) {
            work->row[1].label[1].render.text = msg01_8_0036DCB8[0];
        } else {
            work->row[1].label[1].render.text = 0;
        }
        if (MenuTecWaitLimitCheck(MenuWork.characterNo, MenuWork.point) != 0 && MenuTecCharWaitUpCheck() != 0) {
            work->row[2].label[1].render.text = msg01_8_0036DCB8[0];
        } else {
            work->row[2].label[1].render.text = 0;
        }
        for (i = 0; i < 3; i++) {
            work->row[i].label[1].color[0] = 96;
            work->row[i].label[1].color[2] = 96;
            work->row[i].label[1].color[1] = 0x80;
        }
    }

    switch (work->state) {
    case 0:
        work->color = 0x00FFFFF8;
        WindowDXSet(&work->window);
        work->window.x = 528;
        work->window.y = 144;
        work->window.style = work->color;
        work->window.width = 240;
        work->window.height = 64;
        /* Text: Set\x03,Lv{x 530}T.Pts */
        work->window.tag_id = "Set\003,Lv\003\022T.Pts";
        work->window.state = 1;
        WindowDXMain(&work->window);
        work->window.state = 3;
        for (i = 0; i < 3; i++) {
            eTagFontSet(&work->row[i].label[0], msg00_7[i]);
            eTagFontSet(&work->row[i].label[1], 0);
            eNumberSet(&work->row[i].number[0], 0);
            eNumberSet(&work->row[i].number[1], 0);
            work->row[i].number[0].digits = 2;
            work->row[i].number[1].digits = 4;
            for (j = 0; j < 2; j++) {
                work->row[i].label[j].work = work->color + 2;
                work->row[i].number[j].work = work->color + 2;
                work->row[i].number[j].format = 0;
                work->row[i].number[j].value = 0;
            }
        }
        eTagFontSet(&work->footer, 0);
        eCursolSet(&work->cursor, 0);
        work->cursor.state = 0;
        work->cursor.work = work->color + 2;
        work->flags = 0;
        work->state = 2;
        /* fallthrough */
    case 2:
        slide = 528;
        switch (MenuWork.menu_state[3]) {
        case 0x32:
            if (MenuWork.resetFlags & 1) {
                work->cursor.state = 0;
                work->window.y = 290;
                work->window.width = 216;
            }
            subListChange();
            slide = 288;
            break;
        case 0x50:
        case 0x56:
            if (MenuWork.resetFlags & 1) {
                work->cursor.state = 0;
                work->window.y = 144;
                work->window.width = 240;
            }
            subListChange();
            slide = 264;
            break;
        case 0x52:
        case 0x54:
            if (MenuWork.resetFlags & 1) {
                eCursolModeChange(&work->cursor, 32);
                work->window.y = 144;
            }
            subListChange();
            slide = 264;
            break;
        default:
            work->cursor.state = 0;
            break;
        }
        MoveSlide(&work->window.x, &slide, 3.0f);
        WindowDXMain(&work->window);
        for (i = 0; i < 3; i++) {
            work->row[i].label[0].x = work->window.x + 67;
            work->row[i].label[0].y = work->window.y + i * 18 + 6;
            eTagFontMain(&work->row[i].label[0]);
            work->row[i].number[0].x = work->window.x + 75;
            work->row[i].number[0].y = work->window.y + i * 18 + 6;
            eNumberMain(&work->row[i].number[0]);
            work->row[i].number[1].x = work->window.x + 105;
            work->row[i].number[1].y = work->window.y + i * 18 + 6;
            eNumberMain(&work->row[i].number[1]);
            work->row[i].label[1].x = work->window.x + 157;
            work->row[i].label[1].y = work->window.y + i * 18 + 6;
            eTagFontMain(&work->row[i].label[1]);
        }
        work->cursor.x = work->window.x + 3;
        work->cursor.y = work->window.y + MenuWork.rowCursor * 18 + 6;
        eCursolMain(&work->cursor);
        break;
    }
}

void MenuTecSortSet00(void)
{
    int *sortRow = MenuSortAddrGet(0);
    int tecNo;
    struct {
        short first;
        short last;
    } range[7] = {
        { 0x2D, 0x34 }, { 0x35, 0x3B }, { 0x3C, 0x43 }, { 0x44, 0x4B },
        { 0x4C, 0x52 }, { 0x53, 0x5A }, { 0x5B, 0x62 },
    };

    for (tecNo = range[MenuWork.characterNo - 1].first; tecNo <= range[MenuWork.characterNo - 1].last; tecNo++) {
        if (func_A197E8(MenuWork.characterNo, tecNo) != 0)
            *sortRow++ = 0xA0000 + (tecNo & 0xFFFF);
    }
    *sortRow = 0;
}

int MenuTecEquipCheck(int chrNo, int tecNo)
{
    int maskedTecNo = tecNo & 0xFFFF;
    MenuTecEquipRecord *equip = func_A191C0(chrNo & 0xFFFF);
    int count = 0;
    int i;

    for (i = 0; i < 6; i++) {
        if (equip->techniqueId[i] == maskedTecNo)
            count++;
    }

    return count;
}

int MenuTecTrgCheck(int tecBit)
{
    return (func_A1A378(tecBit & 0xFFFF)[0] & 3) != 0;
}

int MenuTecTypeCheck(int chrNo, int bit)
{
    int tecNo;

    chrNo = chrNo & 0xFFFF;
    tecNo = BitToTecNo(bit);
    return MenuTecSaveDataGet(chrNo)->speed[tecNo] == 1;
}

void MenuTecListMake00(void)
{
    short *techniqueIds;
    int sortCount;
    struct MenuListEntry *rows;

    rows = MenuListGet(0);
    techniqueIds = MenuSortAddrGet(0);
    sortCount = MenuSortCheck(0);
    MenuListMake(0, 0);
    if (sortCount > 0) {
        int remaining = sortCount;

        do {
            short techniqueId = *techniqueIds;
            int equippedCount;

            techniqueIds += 2;
            remaining--;
            equippedCount = MenuTecEquipCheck(MenuWork.characterNo, techniqueId);
            rows->flag = 0;
            rows->amount = equippedCount;
            rows++;
        } while (remaining != 0);
    }
}

void MenuTecListMake00_2(void)
{
    MenuListEntry *rows = MenuListGet(0);
    int *sortRow = MenuSortAddrGet(0);
    int count = MenuSortCheck(0);
    int lowMode = MenuWork.mode < 2;
    int i;

    for (i = 0; i < count; i++) {
        int learned = MenuTecTypeCheck(MenuWork.characterNo, sortRow[i]);

        if (lowMode == learned || learned == 1) {
            const unsigned char *table = MenuWork.characterNo == 7 ? MenuTecTrgTbl01 : MenuTecTrgTbl00;
            unsigned char wanted = table[MenuWork.mode];

            if (wanted == MenuTecTrgCheck(sortRow[i]))
                rows[i].flag = 0;
            else
                rows[i].flag = 1;
        } else {
            rows[i].flag = 1;
        }
    }
}

void MenuTecListMake01(void)
{
    MenuListEntry *rows = MenuListGet(0);
    int *sortRow = MenuSortAddrGet(0);
    int count = MenuSortCheck(0);
    signed char savedBit = MenuWork.tecBit;
    signed char savedPoint = MenuWork.point;
    int i;

    MenuListMake(0, 0);
    for (i = 0; i < count; i++) {
        rows[i].amount = MenuTecEquipCheck(MenuWork.characterNo, (short)sortRow[i]);
        MenuWork.tecBit = sortRow[i];
        MenuWork.point = BitToTecNo(MenuWork.tecBit);
        if (MenuTecCharTLevUpCheck() != 0 || MenuTecCharWaitUpCheck() != 0)
            rows[i].flag = 0;
        else if (MenuTecCharSpeedUpCheck() != 0)
            rows[i].flag = 0;
        else
            rows[i].flag = 1;
    }
    MenuWork.tecBit = savedBit;
    MenuWork.point = savedPoint;
}

void MenuTec(void)
{
    unsigned char *heap;
    int dataSize;
    PartyDataPrefix *party;
    int i;
    int character;
    int convertedCharacter;
    int oldCursor;
    int selected;
    int validTarget;
    int validType;
    signed char nextMenu[3];
    int canImprove[3];

    if (MenuWork.menu_state[3] == 0) {
        MenuTecPasWork *pas;
        MenuTecInfoWork *info;
        MenuTecStatusWork *status;
        MenuTecMenuWork *menu;
        MenuTecListWork *list;
        MenuTecList2Work *list2;
        MenuTecSetWinWork *setWin;
        MenuTecExWork *extra;
        MenuTecL1R1Work *shoulder;
        heap = MainMenuWorkEnd;
        MenuTecDataBuf = (void *)heap;
        dataSize = xglCdReadFile(tecDataPath, heap, 0, 1);
        heap = (unsigned char *)MenuTecDataBuf + dataSize;
        pas = (void *)heap;
        MenuTecPas = pas;
        heap += 0x3b0;
        pas->state = 0;
        info = (void *)heap;
        MenuTecInfo = info;
        heap += 0x3d0;
        info->state = 0;
        status = (void *)heap;
        MenuTecStatus = status;
        heap += 0x4ef4;
        status->window[0].header.state = 0;
        menu = (void *)heap;
        MenuTecMenu = menu;
        heap += 0xf20;
        menu->state = 0;
        list = (void *)heap;
        MenuTecList = list;
        heap += 0x1740;
        list->state = 0;
        list2 = (void *)heap;
        MenuTecList2 = list2;
        heap += 0x1210;
        list2->state = 0;
        setWin = (void *)heap;
        MenuTecSetWin = setWin;
        heap += 0x640;
        setWin->state = 0;
        extra = (void *)heap;
        MenuTecEx = extra;
        heap += 0x248;
        extra->state = 0;
        shoulder = (void *)heap;
        MenuTecL1R1 = shoulder;
        shoulder->state = 0;
        MenuWork.nextState = 0x10;
        MenuWork.inputDelay = 12;
        MenuWork.resetFlags = 0;
        MenuWork.rosterCursor = 0;
        MenuWork.rosterCount = 0;
        MenuKeepSelectReset();
        party = PartyDataGet();
        for (i = 0; i < 3; i++) {
            character = party->attack_slots[i].party_id;
            if (character != 0) {
                convertedCharacter = MenuMaryIdChange(character);
                MenuTecParty[MenuWork.rosterCount] = convertedCharacter;
                MenuWork.rosterCount++;
            }
        }
        for (character = 1; character < 8; character++) {
            if (PartyFriendLockCheck(character, 1) && !PartyAttackerCheck(character)) {
                convertedCharacter = MenuMaryIdChange(character);
                MenuTecParty[MenuWork.rosterCount] = convertedCharacter;
                MenuWork.rosterCount++;
            }
        }
    }
    if (MenuWork.menu_state[3] != MenuWork.nextState) {
        MenuWork.menu_state[3] = MenuWork.nextState;
        MenuWork.resetFlags |= 1;
    } else {
        MenuWork.resetFlags &= 0xfe;
    }
    if (MenuWork.inputDelay != 0)
        MenuWork.inputDelay--;
    switch (MenuWork.menu_state[3]) {
    case 0x10:
        if (MenuWork.resetFlags & 1) {
            MenuWork.listCursor = 0;
            MenuWork.characterNo = MenuTecParty[MenuWork.rosterCursor];
        }
        if (MenuWork.inputDelay != 0)
            break;
        if (PadData.mappedRepeat & 0xf000) {
            if (MenuWork.rosterCount == 1)
                break;
            switch (PadData.mappedRepeat) {
            case 0x1000: MenuWork.rosterCursor -= 2; break;
            case 0x4000: MenuWork.rosterCursor += 2; break;
            case 0x8000: MenuWork.rosterCursor--; break;
            case 0x2000: MenuWork.rosterCursor++; break;
            }
            if (MenuWork.rosterCursor < 0)
                MenuWork.rosterCursor += MenuWork.rosterCount;
            if (MenuWork.rosterCursor >= MenuWork.rosterCount)
                MenuWork.rosterCursor -= MenuWork.rosterCount;
            MenuWork.characterNo = MenuTecParty[MenuWork.rosterCursor];
            xglSoundEffectNormalID(3, 0);
        } else if (PadData.half_2a == 0x20) {
            if (MenuMainCharCheck(MenuWork.characterNo)) {
                MenuWork.nextState = 0x20;
                MenuWork.inputDelay = 24;
                xglSoundEffectNormalID(1, 0);
                MenuWork.characterHelp = 0;
            } else {
                xglSoundEffectNormalID(5, 0);
                MenuWork.characterHelp = 1;
            }
        } else if (PadData.half_2a == 0x40) {
            xglSoundEffectNormalID(2, 0);
            MenuWork.nextState = 0xf0;
        }
        break;
    case 0x20:
        if (MenuWork.inputDelay != 0)
            break;
        if (PadData.half_2a == 4) {
            for (i = 0; i < MenuWork.rosterCount; i++) {
                if (MenuWork.rosterCursor == 0)
                    MenuWork.rosterCursor = MenuWork.rosterCount - 1;
                else
                    MenuWork.rosterCursor--;
                MenuWork.characterNo = MenuTecParty[MenuWork.rosterCursor];
                if (MenuMainCharCheck(MenuWork.characterNo))
                    break;
            }
            xglSoundEffectNormalID(3, 0);
            MenuKeepSelectReset();
        }
        if (PadData.half_2a == 8) {
            for (i = 0; i < MenuWork.rosterCount; i++) {
                if (MenuWork.rosterCursor == MenuWork.rosterCount - 1)
                    MenuWork.rosterCursor = 0;
                else
                    MenuWork.rosterCursor++;
                MenuWork.characterNo = MenuTecParty[MenuWork.rosterCursor];
                if (MenuMainCharCheck(MenuWork.characterNo))
                    break;
            }
            xglSoundEffectNormalID(3, 0);
            MenuKeepSelectReset();
        }
        oldCursor = MenuWork.listCursor;
        MenuWork.listCursor = MenuSelectMove(MenuWork.listCursor, 3, 0);
        if (MenuWork.listCursor == oldCursor) {
            if (PadData.half_2a == 0x20) {
                __builtin_memcpy(nextMenu, nextMenuTable, sizeof(nextMenu));
                MenuWork.inputDelay = 8;
                MenuWork.nextState = nextMenu[MenuWork.listCursor];
                if (!MenuCursorKeepCheck()) {
                    MenuWork.mode = 0;
                    MenuWork.rowCursor = 0;
                }
                if (MenuWork.listCursor == 2) {
                    MenuKeepSelectReset();
                    xglSoundEffectNormalID(2, 0);
                } else {
                    xglSoundEffectNormalID(1, 0);
                }
            }
            if (PadData.half_2a == 0x40) {
                MenuWork.nextState = 0x10;
                MenuWork.inputDelay = 12;
                xglSoundEffectNormalID(2, 0);
                MenuKeepSelectReset();
            }
        }
        break;
    case 0x30:
        if (MenuWork.resetFlags & 1) {
            MenuTecSortSet00();
            MenuTecListMake00();
        }
        if (MenuWork.inputDelay == 0) {
            MenuWork.mode = MenuSelectMove(MenuWork.mode, 6, 0);
            if (PadData.half_2a == 0x20) {
                if (MenuSortCheck(0)) {
                    MenuWork.nextState = 0x32;
                    xglSoundEffectNormalID(1, 0);
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
            }
            if (PadData.half_2a == 0x40) {
                if (!MenuCursorKeepCheck())
                    MenuWork.listCursor = 0;
                WindowSPKeepSelectCheck(MenuKeepSelect);
                xglSoundEffectNormalID(2, 0);
                MenuWork.inputDelay = 8;
                MenuWork.nextState = 0x20;
            }
        }
        MenuWork.tecBit = func_A191C0(MenuWork.characterNo)->techniqueId[MenuWork.mode];
        MenuWork.point = BitToTecNo(MenuWork.tecBit);
        break;
    case 0x32:
        if (MenuWork.resetFlags & 1)
            MenuTecListMake00_2();
        if (MenuWork.inputDelay == 0) {
            MenuWork.selectedTechnique = WindowSPSelect(&MenuTecList->window, PadData.mappedRepeat);
            WindowSPKeepSelect(&MenuTecList->window, MenuKeepSelect);
            if (MenuWork.selectedTechnique >= 0) {
                MenuWork.tecBit = MenuSortGet(0, MenuWork.selectedTechnique);
                MenuWork.point = BitToTecNo(MenuWork.tecBit);
                MenuWork.resetFlags &= 0xf3;
                if (PadData.half_2a == 0x20) {
                    validTarget = MenuTecTrgCheck(MenuWork.tecBit);
                    if (MenuWork.characterNo == 7)
                        selected = MenuTecTrgTbl01[MenuWork.mode];
                    else
                        selected = MenuTecTrgTbl00[MenuWork.mode];
                    if (validTarget == selected)
                        MenuWork.resetFlags &= 0xfb;
                    else
                        MenuWork.resetFlags |= 4;
                    validType = MenuTecTypeCheck(MenuWork.characterNo, MenuWork.tecBit);
                    if (validType == (MenuWork.mode < 2))
                        MenuWork.resetFlags &= 0xf7;
                    else
                        MenuWork.resetFlags |= 8;
                    if ((MenuWork.resetFlags & 4) == 0) {
                        if (validType == 1 || (MenuWork.resetFlags & 8) == 0) {
                            MenuTecEquipRecord *equip = func_A191C0(MenuWork.characterNo);
                            int slot = MenuWork.mode;
                            int technique = MenuWork.tecBit;
                            selected = equip->techniqueId[slot];
                            if (selected == technique) {
                                xglSoundEffectNormalID(12, 0);
                            } else {
                                func_A16688(MenuWork.characterNo, slot, technique);
                                xglSoundEffectNormalID(12, 0);
                            }
                            MenuTecListMake00();
                            MenuTecListMake00_2();
                        } else {
                            xglSoundEffectNormalID(5, 0);
                        }
                    } else {
                        xglSoundEffectNormalID(5, 0);
                    }
                }
            }
            if (PadData.half_2a == 0x40) {
                xglSoundEffectNormalID(2, 0);
                MenuWork.nextState = 0x30;
            }
        }
        break;
    case 0x50:
        if (MenuWork.resetFlags & 1) {
            MenuTecSortSet00();
            MenuTecListMake01();
        }
        if (MenuWork.inputDelay == 0) {
            MenuWork.selectedTechnique = WindowSPSelect(&MenuTecList->window, PadData.mappedRepeat);
            WindowSPKeepSelect(&MenuTecList->window, &MenuKeepSelect[5]);
            if (MenuWork.selectedTechnique >= 0) {
                MenuWork.tecBit = MenuSortGet(0, MenuWork.selectedTechnique);
                MenuWork.point = BitToTecNo(MenuWork.tecBit);
                if (PadData.mappedPressed == 0x20) {
                    if (MenuTecCharTLevUpCheck() || MenuTecCharSpeedUpCheck() || MenuTecCharWaitUpCheck()) {
                        MenuWork.nextState = 0x52;
                        xglSoundEffectNormalID(1, 0);
                    } else {
                        xglSoundEffectNormalID(5, 0);
                    }
                }
            }
            if (PadData.half_2a == 0x40) {
                if (!MenuCursorKeepCheck())
                    MenuWork.listCursor = 0;
                WindowSPKeepSelectCheck(&MenuKeepSelect[5]);
                xglSoundEffectNormalID(2, 0);
                MenuWork.nextState = 0x20;
                MenuWork.inputDelay = 8;
            }
        } else {
            MenuWork.selectedTechnique = WindowSPSelect(&MenuTecList->window, 0);
            MenuWork.tecBit = MenuSortGet(0, MenuWork.selectedTechnique);
            MenuWork.point = BitToTecNo(MenuWork.tecBit);
        }
        break;
    case 0x52:
        if ((MenuWork.resetFlags & 1) && !MenuCursorKeepCheck())
            MenuWork.rowCursor = 0;
        if (MenuWork.inputDelay == 0) {
            canImprove[0] = MenuTecCharTLevUpCheck();
            canImprove[1] = MenuTecCharSpeedUpCheck();
            canImprove[2] = MenuTecCharWaitUpCheck();
            MenuWork.rowCursor = MenuSelectMove(MenuWork.rowCursor, 3, 0);
            if (PadData.half_2a == 0x20) {
                if (canImprove[0] + canImprove[1] + canImprove[2] != 0) {
                    if (canImprove[MenuWork.rowCursor] != 0) {
                        MenuWork.nextState = 0x54;
                        MenuWork.inputDelay = 8;
                        xglSoundEffectNormalID(1, 0);
                    } else {
                        xglSoundEffectNormalID(5, 0);
                    }
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
            }
            if (PadData.half_2a == 0x40) {
                xglSoundEffectNormalID(2, 0);
                MenuWork.nextState = 0x50;
            }
        }
        break;
    case 0x54:
        if (MenuWork.resetFlags & 1)
            MenuWork.selectCursor = 0;
        if (MenuWork.inputDelay == 0) {
            MenuWork.selectCursor = MenuSelectMove(MenuWork.selectCursor, 2, 0);
            if (PadData.half_2a == 0x20) {
                if (MenuWork.selectCursor == 0) {
                    switch (MenuWork.rowCursor) {
                    case 0: MenuTecTLevUp(MenuWork.characterNo, MenuWork.point); break;
                    case 1: MenuTecSpeedUp(MenuWork.characterNo, MenuWork.point); break;
                    case 2: MenuTecWaitUp(MenuWork.characterNo, MenuWork.point); break;
                    }
                    xglSoundEffectNormalID(1, 0);
                    MenuWork.nextState = 0x56;
                } else {
                    MenuWork.nextState = 0x52;
                    xglSoundEffectNormalID(2, 0);
                }
                MenuTecListMake01();
            }
            if (PadData.half_2a == 0x40) {
                xglSoundEffectNormalID(2, 0);
                MenuWork.nextState = 0x52;
            }
        }
        break;
    case 0x56:
        if (MenuWork.inputDelay == 0 && (PadData.half_2a & 0x20))
            MenuWork.nextState = 0x52;
        break;
    case 0xf0:
        if (MenuWork.resetFlags & 1)
            MenuWork.inputDelay = 16;
        if (MenuWork.inputDelay == 0)
            MenuWork.nextState = 0xff;
        break;
    case 0xff:
        ChangeTopLevel(0);
        break;
    }
    MenuTecPasMain();
    MenuTecInfoMain();
    MenuTecStatusMain();
    MenuTecMenuMain();
    MenuTecListMain();
    MenuTecSetWinMain();
    MenuTecListMain2();
    MenuTecExMain();
    MenuTecL1R1Main();
    MenuWork.resetFlags |= 2;
    xglFontDebugPrintf(0, 64, tecLabel);
    xglFontDebugPrintf(0, 80, msFormat, MenuWork.listCursor);
    xglFontDebugPrintf(0, 88, sliFormat, MenuWork.mode);
    xglFontDebugPrintf(0, 96, lstFormat, MenuWork.selectedTechnique);
    xglFontDebugPrintf(0, 104, sctFormat, MenuWork.rowCursor);
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
/* Defined after MenuTec: the original .sdata holds it at 0x004DB518, after
 * every string literal of the TU's functions (MenuTec's ends at 0x004DB517). */
MenuTecData *MenuTecDataBuf = 0;

const char techSetMenuText[24] = "Set\nUse T.Pts\nCancel";

const char techConfirmQuestionText[24] = " Is this okay\?";

/* Text: List\x03P\x03>Eqp */
const char techListTitleText[16] = "List\003P\003>Eqp";

const char spiritsAbsorbedText[24] = "Spirits Absorbed";

const char numberDefeatedText[16] = "Number Defeated";

const char timesUsedText[16] = "Times Used";

const MenuTecList2MessageIds list2MessageIdTable = { .id = { 0, 1, 0, 0, 1, 0 } };

const char setListHiTitleText[16] = "SetList/HI";

const char setListLwTitleText[16] = "SetList/LW";

const MenuTecList2SpriteIds list2SpriteIdTable = { .id = { 513, 514, 513, 513, 513, 514, 514, 514, 514, 513 } };

const char completeText[16] = "Complete";

const char tecDataPath[32] = "data\\endou\\tec\\tecdata.bin";

/* Text: Game Options | Set all sound output during game to stereo or mono. \nDefault: Stereo | Remember cursor location.\nDefault: On | Set vibration On/Off.\nDefault: On | Set radar display On/Off.\nDefault: On | Adjust game display position on TV monitor. | Set data location.\nOn: Hard Disk Drive\nOff: DVD-ROM | Return to default settings. */
const char MenuSystemOptionHelpText[344] = "Game Options\000\000\000\000Set all sound output during game to stereo or mono. \nDefault: Stereo\000\000\000\000Remember cursor location.\nDefault: On\000\000\000Set vibration On/Off.\nDefault: On\000\000\000\000\000\000\000Set radar display On/Off.\nDefault: On\000\000\000Adjust game display position on TV monitor.\000\000\000\000\000Set data location.\nOn: Hard Disk Drive\nOff: DVD-ROM\000\000\000\000\000Return to default settings.";

char infoPeriodText[8] = ".";

const char confirmYesNoText[8] = "Yes\nNo";

/* Text: \x01T.Pts */
const char techPointsText[8] = "\001T.Pts";

const char listTitleText[8] = "List";

const char farText[8] = "Far";

const char nearText[8] = "Near";

/* Text: \x01Wait */
const char waitText[8] = "\001Wait";

/* Text: \x01Speed */
const char speedText[8] = "\001Speed";

/* Text: \x01Tech */
const char techText[8] = "\001Tech";

const char dashLineText[8] = "-----";

const char upOkText[8] = "UpOK";

const char waitLevelA9Text[8] = "A9";

const char waitLevelA8Text[8] = "A8";

const char waitLevelA7Text[8] = "A7";

const char waitLevelA6Text[8] = "A6";

const char waitLevelA5Text[8] = "A5";

const char waitLevelA4Text[8] = "A4";

const char waitLevelA3Text[8] = "A3";

const char waitLevelA2Text[8] = "A2";

const char waitLevelA1Text[8] = "A1";

const char waitLevelA0Text[8] = "A0";

/* Text: {reset}tec */
char tecLabel[8] = "\013tec";

/* Text: {reset}ms %2d */
char msFormat[8] = "\013ms %2d";

/* Text: {reset}sli%2d */
char sliFormat[8] = "\013sli%2d";

/* Text: {reset}lst%2d */
char lstFormat[8] = "\013lst%2d";

/* Text: {reset}sct%2d */
char sctFormat[8] = "\013sct%2d";

MenuTecPasWork * MenuTecPas = 0;

MenuTecInfoWork * MenuTecInfo = 0;

MenuTecMenuWork * MenuTecMenu = 0;

MenuTecExWork * MenuTecEx = 0;

MenuTecL1R1Work * MenuTecL1R1 = 0;

MenuTecStatusWork * MenuTecStatus = 0;

MenuTecListWork * MenuTecList = 0;

MenuTecList2Work * MenuTecList2 = 0;

MenuTecSetWinWork * MenuTecSetWin = 0;

const char MenuSystemDefaultText[8] = "Default";

const char MenuSystemHddText[8] = "HDD";

const char MenuSystemMonoText[8] = "Mono";

const char MenuSystemStereoText[8] = "Stereo";

const char MenuSystemOffText[8] = "Off";

const char MenuSystemOnText[8] = "On";

const char MenuSystemYesNoText[8] = "Yes\nNo";

/* Text: . \x1F */
char infoSentenceEndText[8] = ". \037";

signed char nextMenuTable[8] = { 48, 80, 16, 0, 0, 0, 0, 0 };
