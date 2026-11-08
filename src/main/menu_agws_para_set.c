#include "common.h"
#include "main/e_battle_win_open.h"
#include "main/menu_1.h"
#include "shared.h"
#include "menu_agws_para_set.h"
#include "main/menu_top_command_check.h"
#include "main/open_close_main.h"
#include "main/party.h"
#include "main/xgl_font.h"
#include "main/menu_para_pt_rate_get.h"
#include "main/end_print.h"
/* MenuWork.agws.state / pendingState values (high nibble = screen, low = step). */
#define AGWS_STATE_SELECT 0x10         /* pick the AGWS unit */
#define AGWS_STATE_COMMAND 0x22        /* command list of the chosen unit */
#define AGWS_STATE_EQUIP 0x40          /* equipment slots */
#define AGWS_STATE_WEAPON_MOUNT 0x42   /* choose the mount position */
#define AGWS_STATE_WEAPON_LIST 0x44
#define AGWS_STATE_AMMO_LIST 0x46
#define AGWS_STATE_ACCESSORY_LIST 0x62
#define AGWS_STATE_PILOT_LIST 0xC2
#define AGWS_STATE_CLOSE 0xF0
#define AGWS_STATE_CLOSE_WAIT 0xF1
#define AGWS_STATE_EXIT 0xFF

/* Frame waits loaded into MenuWork.agws.transitionTicks. */
#define AGWS_WAIT_SCROLL 4
#define AGWS_WAIT_LEAVE 12
#define AGWS_WAIT_SWITCH 16
#define AGWS_WAIT_COMMAND 24

#define AGWS_SE_EQUIP 0x10
#define AGWS_SE_REMOVE 0x11
#define AGWS_SE_PILOT_SET 0x12
#define AGWS_SE_PILOT_REMOVE 0x13

/* Work buffers carved from MainMenuWorkEnd by MenuAgws. */
#define AGWS_PAS_WORK_SIZE 1904
#define AGWS_INFO_WORK_SIZE 1104
#define AGWS_SELECT_WORK_SIZE 7736
#define AGWS_LIST_WORK_SIZE 0x5E10
#define AGWS_STATUS_WORK_SIZE 0x71C
#define AGWS_SWITCH_WORK_SIZE 0x1B8
#define AGWS_FACE_WORK_SIZE 0x1C4

#define AGWS_FIRST_UNIT_ID 0x11        /* PartyTakeAgwsCheck range start */
#define AGWS_UNIT_ID_END 0x21
#define AGWS_UNIT_MAX 0x10
#define AGWS_LIST_WINDOW_COUNT 4
#define AGWS_KEEP_SLOT_SIZE 5          /* MenuKeepSelect entries per list */
#define AGWS_WEAPON_HAND_ACCESSORY 3   /* weaponHand value for the accessory row */
#define AGWS_MODEL_MEMORY_ID 0xB


#define AGWS_WEAPON2_SETUP 0
#define AGWS_WEAPON2_MAIN  2
#define M2C_FIELD(expr, type_ptr, offset) (*(type_ptr)((s8 *)(expr) + (offset)))
#define NULL ((void *)0)

typedef struct AgwsWeapon2Sprite {
    unsigned char unmodeled_00[4];
    short x;                                /* +0x04 */
    short y;                                /* +0x06 */
    int color;                              /* +0x08 */
    unsigned char unmodeled_0c[0x28 - 0x0C];
} AgwsWeapon2Sprite;


typedef struct AgwsWeapon2Row {
    unsigned char unmodeled_00[0x14];
    WindowDX window;             /* +0x014 */
    EMessageParam message[3];               /* +0x1A8 */
    AgwsWeapon2Sprite sprite[3];            /* +0x274 */
    unsigned char unmodeled_2ec[0x300 - 0x2EC];
} AgwsWeapon2Row;

typedef struct AgwsWeapon2PanelView {
    unsigned char state;                    /* +0x00 */
    unsigned char mode;                     /* +0x01 */
    unsigned char unmodeled_02[2];
    int color;                              /* +0x04 */
    unsigned char cursorPrefix[0x10];
    AgwsWeapon2Row row[4];                  /* +0x18 */
    unsigned char unmodeled_c18[0x14];
} AgwsWeapon2PanelView;

typedef struct AgwsWeapon2CursolControl {
    signed char state;
    signed char active;
    signed char timer;
    signed char width;
    short x;
    short y;
    int work;
    signed char color[4];
} AgwsWeapon2CursolControl;

typedef struct AgwsWeapon2CursolData {
    AgwsWeapon2CursolControl control;
    unsigned char unmodeled_10[0x14];
} AgwsWeapon2CursolData;

typedef union AgwsWeapon2Work AgwsWeapon2Work;

union AgwsWeapon2Work {
    AgwsWeapon2PanelView panels;
    /* Row zero's leading bytes overlap the cursor sprite tail. */
    struct {
        unsigned char controlPrefix[8];
        AgwsWeapon2CursolData cursol;       /* +0x08 */
    } cursor;
};

typedef struct AgwsWeapon2MountRow {
    unsigned short icon[3];
} AgwsWeapon2MountRow;

typedef struct AgwsWeapon2MountSide {
    unsigned short icon[2];
} AgwsWeapon2MountSide;

typedef struct AgwsWeapon2Cursol {
    unsigned char state;                    /* +0x00 */
    unsigned char unmodeled_01[3];
    short x;                                /* +0x04 */
    short y;                                /* +0x06 */
    int color;                              /* +0x08 */
    unsigned char unmodeled_0c[0x24 - 0x0C];
} AgwsWeapon2Cursol;

typedef signed char s8;

typedef int s32;

typedef s32 M2C_UNK;

typedef struct AgwsNamePoint {
    short x, y;
    int depth;
    unsigned char r, g, b, a;
} AgwsNamePoint;


typedef struct AgwsNameWorkPrefix {
    unsigned char unmodeled_00[0x10];
    unsigned char state;
    unsigned char mode;
    unsigned char slide;
    signed char selection;
    int timer;
    int color;
    struct WindowDX window[8];
    AgwsWeapon2Sprite portrait[8];
    unsigned char unmodeled_dfc[0xF3C - 0xDFC];
    EMessageParam message[8][2];
    AgwsWeapon2Cursol cursol;
    AgwsNamePoint ribbon[8][20];
    unsigned char unmodeled_1b20[0x1D00 - 0x1B20];
} AgwsNameWorkPrefix;

extern const char infoClearText[];
extern AgwsWeapon2Work *AgwsWeapon2;
extern AgwsWeapon2MountRow weapon2MountRowTable[];
extern const AgwsWeapon2MountSide weapon2MountSideTable[];
extern AgwsNameWorkPrefix *AgwsName;
extern unsigned char *MainMenuWorkEnd;
extern const char equipPosFormat[];
extern char agwsTitleText[];
extern char paddedNumberFormat[];
extern void *MenuModelOut[4];
extern AgwsInfoWork *AgwsInfo;
static const char * msg00_1_0036D988[] ;
extern const char menuAgwsParaSetPeriodText[];
extern const char commaText[];
extern AgwsSelectWork *AgwsSelect;
extern char menuWindowTitleText[];
extern char selectWindowTitleText[];
extern const char emptyMessageText[];
extern const char MenuAgwsSelectPromptText[19];
extern const char MenuAgwsEquipHelpText[31];
extern const char MenuAgwsViewHelpText[42];
extern const char MenuAgwsAssignHelpText[26];
extern const char MenuAgwsUnequipHelpText[41];
extern const char MenuAgwsRightShoulderLabel[12];
extern const char MenuAgwsLeftArmLabel[7];
extern const char MenuAgwsRightArmLabel[7];
extern const char MenuAgwsRightAltLabel[7];
extern const char MenuAgwsLeftAltLabel[7];
extern const char MenuAgwsLeftShoulderLabel[12];
extern const char MenuAgwsAccessoryAvailableText[31];
extern const char MenuAgwsNothingEquippedText[18];
extern const char MenuAgwsSeparatorText[3];
extern const char MenuAgwsAmmoRemoveBlockedText[30];
extern const char MenuAgwsWeaponAvailableText[28];
extern const char MenuAgwsEquipWeaponOnText[17];
extern const char MenuAgwsAndEquipAnotherText[27];
extern const char MenuAgwsRemoveWeaponFromText[20];
extern const char MenuAgwsAndEquipOnText[15];
extern const char MenuAgwsIsAssignedToText[17];
extern const char assignVerbText[8];
extern const char MenuAgwsAsPilotOfThisText[31];
extern const char removeVerbText[8];
extern const char MenuAgwsPilotChoicesText[26];
extern const char MenuAgwsEquipUnequipPilotText[20];
extern const char MenuAgwsArmSlotChoicesText[16];
extern const char MenuAgwsLeftShoulderSlotText[13];
extern const char MenuAgwsShoulderSlotChoicesText[24];
extern const char MenuAgwsConfirmPromptText[15];
extern char menuAgwsParaSetYesNoText[8];
extern char rightArmText[8];
extern char leftAltText[8];
extern const char MenuAgwsAltSlotChoicesText[16];
extern const char MenuAgwsGrayedSlotChoicesText[76];
extern char rightAltText[8];
extern const char MenuAgwsRightShoulderSlotText[13];
extern char leftArmText[8];
extern const char MenuAgwsEquipUnequipCancelText[21];
extern char okayQuestionText[8];
extern const char MenuAgwsAssignRemoveCancelText[21];

void WindowSPSet(WindowSPCursor *cursor);
void WindowSPMain(WindowSPCursor *cursor);
void WindowSPSelectJump(WindowSPCursor *cursor, int selection);
int PartyAttackerCheck(int chrNo);
char *MenuTagTextGet(int index);
int MenuAgwsPilotCheck(int chrNo);
void eSpriteSet(void *sprite, short spriteId);
void eSpriteMain(void *sprite);
s32 MenuMainAgwsCheck(s16);
void MenuModelAllBreak(void);
void MenuModelCreate(void **output, int character);
void MenuModelExtFuncSet(void *, void (*)(MenuModelUnitAgwsView *), M2C_UNK);
void MenuModelMemorySet(int);
void MenuModelMenuMotionLoad(void);
s32 MenuRWeaponCheck(s16, s32);
void MenuWorkEndCheck(u8 *end);
int dataAccBoxDec(int);
void dataAccBoxInc(int);
s32 dataBltBoxChk(s16);
void dataBltBoxDec(int);
void dataBltBoxInc(int);
void dataWpnBoxDec(int);
void dataWpnBoxInc(int);
void xglFontDebugHex(int x, int y, int value, int digits);
extern int MenuSelectMove(int selection, int count, int wrap);
extern int MenuCursorKeepCheck(void);
void AgwsNameMain(void);
void AgwsWeapon2Main(void);
void *memset(void *destination, int value, unsigned int size);
int OptMntToWpnPos(int mountCode);
char *MenuCharNameGet(unsigned int charId);
char *strcat(char *destination, const char *source);

/* Text: \x01FHP */
const char menuAgwsParaSetFhpText[8] = "\001FHP";

/* Text: \x01DPOW */
const char dpowText[8] = "\001DPOW";

/* Text: \x01PARM */
const char parmText[8] = "\001PARM";

/* Text: \x01EARM */
const char earmText[8] = "\001EARM";

/* Text: \x01AGL */
const char aglText[8] = "\001AGL";

/* Text: \x01WAGL */
const char wagText[8] = "\001WAGL";

/* Text: \x01Weight */
const char menuAgwsParaSetWeightText[8] = "\001Weight";

static const char *msg00_0_0036D940[17] = {
    MenuAgwsNameLabel, MenuAgwsEquipCommandLabel, MenuAgwsUnequipCommandLabel, MenuAgwsPilotCommandLabel, MenuAgwsListCommandLabel,
    MenuAgwsRightArmCommandLabel, MenuAgwsLeftArmCommandLabel, MenuAgwsRightShoulderCommandLabel, MenuAgwsLeftShoulderCommandLabel, MenuAgwsRightAltCommandLabel,
    MenuAgwsLeftAltCommandLabel, MenuAgwsAmmoCommandLabel, MenuAgwsAccessoryCommandLabel, MenuAgwsAssignCommandLabel, MenuAgwsRemoveCommandLabel,
    MenuAgwsEngineUnitCommandLabel, MenuAgwsAssignPilotCommandLabel,
};

static const char *msg_10[7] = {
    menuAgwsParaSetFhpText, dpowText, parmText,
    earmText, aglText, wagText,
    menuAgwsParaSetWeightText,
};

AgwsParaDisplay MenuAgwsPara = {{0}};

AgwsParaDisplay MenuAgwsPara2 = {{0}};

AgwsSwitchSlideStep switchSlideStepTable[4] = {
    { { 1, -1 } }, { { 0, 0 } }, { { 0, 0 } }, { { 0, 0 } },
};

AgwsPasWork *AgwsPas = 0;

AgwsListWork *AgwsList = 0;

AgwsStatusWork *AgwsStatus = 0;

AgwsFaceWork *AgwsFace = 0;

AgwsSwitchWork *AgwsSwitch = 0;

static const char * msg01_2_0036D990[4];

static const char * msg02_3_0036D9A0[6];

static const char * msg03_4_0036D9B8[5];

static const char * msg04_5[4];

static const char * msg05_6[5];

static const char * msg_7_0036D9F8[17];

static const char * msg01_8_0036DA40[3];

static const char * msg00_1_0036D988[] = { MenuAgwsSelectPromptText };

static const char * msg01_2_0036D990[4] = { MenuAgwsEquipHelpText, MenuAgwsUnequipHelpText, MenuAgwsAssignHelpText, MenuAgwsViewHelpText };

static const char * msg02_3_0036D9A0[6] = { MenuAgwsRightArmLabel, MenuAgwsLeftArmLabel, MenuAgwsRightShoulderLabel, MenuAgwsLeftShoulderLabel, MenuAgwsRightAltLabel, MenuAgwsLeftAltLabel };

static const char * msg03_4_0036D9B8[5] = { MenuAgwsSeparatorText, MenuAgwsWeaponAvailableText, MenuAgwsNothingEquippedText, MenuAgwsAmmoRemoveBlockedText, MenuAgwsAccessoryAvailableText };

static const char * msg04_5[4] = { MenuAgwsAndEquipAnotherText, MenuAgwsRemoveWeaponFromText, MenuAgwsAndEquipOnText, MenuAgwsEquipWeaponOnText };

static const char * msg05_6[5] = { assignVerbText, removeVerbText, MenuAgwsIsAssignedToText, menuAgwsParaSetPeriodText, MenuAgwsAsPilotOfThisText };

static const char * msg_7_0036D9F8[17] = { 0, MenuAgwsEquipUnequipPilotText, MenuAgwsEquipUnequipCancelText, MenuAgwsEquipUnequipCancelText, MenuAgwsPilotChoicesText, MenuAgwsConfirmPromptText, menuAgwsParaSetYesNoText, MenuAgwsGrayedSlotChoicesText, MenuAgwsArmSlotChoicesText, MenuAgwsShoulderSlotChoicesText, MenuAgwsAltSlotChoicesText, rightArmText, leftArmText, MenuAgwsRightShoulderSlotText, MenuAgwsLeftShoulderSlotText, rightAltText, leftAltText };

static const char * msg01_8_0036DA40[3] = { MenuAgwsAssignRemoveCancelText, okayQuestionText, menuAgwsParaSetYesNoText };

static unsigned short AgwsNo[16];

static WindowSPCursor * AgwsWinSP[4];

static char mmsg_9[128];

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
const char pilotListTitleText[24] = "Menu         Party";

const AgwsCameraPreset cameraSetPreset = {{
    { 0.5f, 2.8f, 4.5f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
}};

const AgwsCameraTargets cameraMoveTargets = {{
    { 0.5f, 2.8f, 4.5f, 1.0f },
    { -1.5f, 2.0f, 8.0f, 1.0f },
}};

const AgwsModelPositions modelPositionTable = {{
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 1.04719758f, 0.0f, 0.0f },
    { 0.0f, -1.04719758f, 0.0f, 0.0f },
    { 0.7853982f, 0.7853982f, 0.0f, 0.0f },
    { 0.7853982f, -0.7853982f, 0.0f, 0.0f },
    { 0.3926991f, 2.0943952f, 0.0f, 0.0f },
    { 0.3926991f, -2.0943952f, 0.0f, 0.0f },
}};

const DoublewordAlignedVector4 modelUnitScaleVector = { 1.0f, 1.0f, 1.0f, 1.0f };

/* Sums the WAGL value of the character's equipped weapons. */
static short WaglGet(AgwsCharPara *para)
{
    short total;
    short weaponId;
    int slot;

    total = 0;
    for (slot = 0; slot < 3; slot++) {
        weaponId = para->weapon[slot];
        if (weaponId != 0) {
            total += func_A1A3D8(MenuRWeaponCheck2(weaponId))->wagl;
        }
    }
    return total;
}

/*
 * Fills one parameter display block from the character's stats and the attack and defense bonuses.
 */
static void ParaSet(AgwsCharPara *para, AgwsParaDisplay *dest, int *attack, int *defense)
{
    dest->value[AGWS_PARA_MAX_HP] = para->maxHp;
    dest->value[AGWS_PARA_ATTACK] = para->attack + attack[0] + attack[1] + attack[2];
    dest->value[AGWS_PARA_PHY_DEFENSE] = para->phyDefense + defense[0];
    dest->value[AGWS_PARA_MAG_DEFENSE] = para->magDefense + defense[1];
    dest->value[AGWS_PARA_STAT8] = (signed char) para->stat8;
    dest->value[AGWS_PARA_WAGL] = WaglGet(para);
    dest->value[AGWS_PARA_STAT9] = para->stat9;
}

/*
 * Refreshes the parameter display blocks for one unit; chrNo 0 does nothing.
 * State 0 and 10 also fill MenuAgwsPara, and state 10 keeps the unit's HP.
 */
void MenuAgwsParaSet(int chrNo, int state)
{
    int attack[4];
    int defense[4];
    AgwsUnitOrg *org;
    short keptHp;
    AgwsCharPara *para;

    if (chrNo != 0) {
        org = func_A191C0(chrNo);
        keptHp = org->hp;
        para = func_00A11108(chrNo, attack, defense);
        ParaSet(para, &MenuAgwsPara2, attack, defense);
        if (state == 0) {
            ParaSet(para, &MenuAgwsPara, attack, defense);
        }
        if (state == 10) {
            ParaSet(para, &MenuAgwsPara, attack, defense);
            org->hp = keptHp;
        }
    }
}

/*
 * Opens the pas window on the first call, then slides it and its tabs
 * toward the targets for the current AGWS state.
 */
void AgwsPasMain(void)
{
    AgwsPasWork *pas = AgwsPas;
    int tabLength[17];
    short tabTarget[17];
    short slideTarget;
    int i;

    for (i = 0; i < 17; i++)
        tabLength[i] = MenuPasLengthGet(msg00_0_0036D940[i]);

    switch (pas->state) {
    case AGWS_PAS_START:
        pas->color = AGWS_PAS_COLOR;
        WindowDXSet(&pas->window);
        pas->window.x = AGWS_PAS_X_OUT;
        pas->window.y = 8;
        pas->window.style = pas->color;
        pas->window.width = AGWS_PAS_WIDTH;
        pas->window.height = 0x20;
        pas->window.close_callback = MenuPasWindow;
        pas->window.close_callback_arg = pas->callbackWork;
        pas->window.state = WINDOWDX_STATE_OPEN;
        WindowDXMain(&pas->window);
        pas->window.state = WINDOWDX_STATE_READY;
        for (i = 0; i < 17; i++) {
            eMessageSet(&pas->message[i], msg00_0_0036D940[i]);
            pas->message[i].mode = E_MODE_SHOW_START;
            pas->message[i].x = AGWS_PAS_TAB_PARKED;
            pas->message[i].y = 0xB;
            pas->message[i].draw_parameter = pas->color + 2;
        }
        pas->mask = 1;
        pas->selectNo = -1;
        pas->state = AGWS_PAS_SLIDE;
        /* fallthrough */
    case AGWS_PAS_SLIDE:
        break;
    default:
        return;
    }

    slideTarget = AGWS_PAS_X_IN;
    for (i = 0; i < 17; i++)
        tabTarget[i] = AGWS_PAS_TAB_PARKED;

    /* Choose the window and tab targets for the current menu state. */
    {
        AgwsMenuWork *menu = &MenuWork.agws;

        switch (menu->state) {
        case AGWS_STATE_SELECT:
        case 0x20:
        case AGWS_STATE_COMMAND:
            tabTarget[0] = 0x10;
            break;
        case AGWS_STATE_WEAPON_LIST:
            tabTarget[0] = -(tabLength[0] + AGWS_PAS_TAB_GAP);
            tabTarget[menu->reserveCursor + 1] = 0x10;
            tabTarget[menu->selectedIndex + AGWS_PAS_TAB_SLOT] =
                tabLength[menu->reserveCursor + 1] + 0x10;
            break;
        case AGWS_STATE_AMMO_LIST:
            tabTarget[0] = -(tabLength[0] +
                             tabLength[menu->reserveCursor + 1] +
                             AGWS_PAS_TAB_GAP);
            tabTarget[menu->reserveCursor + 1] =
                -(tabLength[menu->reserveCursor + 1] + AGWS_PAS_TAB_GAP);
            tabTarget[menu->selectedIndex + AGWS_PAS_TAB_SLOT] = 0x10;
            tabTarget[AGWS_PAS_TAB_AMMO] =
                tabLength[menu->selectedIndex + AGWS_PAS_TAB_SLOT] + 0x10;
            break;
        case AGWS_STATE_ACCESSORY_LIST:
            tabTarget[0] = -(tabLength[0] + AGWS_PAS_TAB_GAP);
            tabTarget[menu->reserveCursor + 1] = 0x10;
            tabTarget[AGWS_PAS_TAB_ACCESSORY] =
                tabLength[menu->reserveCursor + 1] + 0x10;
            break;
        case AGWS_STATE_EQUIP:
        case AGWS_STATE_WEAPON_MOUNT:
        case AGWS_STATE_PILOT_LIST:
            tabTarget[0] = 0x10;
            tabTarget[menu->reserveCursor + 1] = tabLength[0] + 0x10;
            break;
        default:
            slideTarget = AGWS_PAS_X_OUT;
            break;
        }
    }

    MoveSlide(&pas->window.x, &slideTarget, 3.0f);
    WindowDXMain(&pas->window);
    if (pas->window.state == WINDOWDX_STATE_READY) {
        pas->box.x = pas->window.x + 3;
        pas->box.y = pas->window.y + 3;
        pas->box.color = pas->color;
        pas->box.width = pas->window.width - 6;
        pas->box.height = pas->window.height - 6;
        endPrintExtFunc(pas->color, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_SCISSOR_SET,&pas->box);
        for (i = 0; i < 17; i++) {
            MoveSlide(&pas->message[i].x, &tabTarget[i], 3.0f);
            if (pas->message[i].x < AGWS_PAS_TAB_LIMIT)
                eMessageMain(&pas->message[i]);
        }
        endPrintExtFunc(pas->color, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_SCISSOR_RESET,0);
    }
}

/*
 * Builds the help text for the current AGWS state and slides the help
 * panel in or out.
 */
void AgwsInfoMain(void)
{
    AgwsInfoWork *info = AgwsInfo;
    AgwsUnitOrg *org;
    short targetY;
    int position;
    int selectedPosition;
    int selectedRow;
    MenuListEntry *row;

    switch (info->state) {
    case 0:
        info->color = 0x00FFFFF0;
        WindowDXSet(&info->window);
        info->window.x = -16;
        info->window.y = 480;
        info->window.width = 544;
        info->window.height = 54;
        info->window.state = 0;
        info->window.close_callback = MenuInfoWindow;
        info->window.close_callback_arg = &info->contents;
        info->contents.textOffset[0] = 0;
        info->window.style = info->color;
        info->window.state = WINDOWDX_STATE_OPEN;
        info->contents.textOffset[1] = 0;
        info->contents.text = 0;
        WindowDXMain(&info->window);
        info->window.state = WINDOWDX_STATE_READY;
        info->state = 2;
        info->detail = 0;
        memset(info->text, 0, sizeof(info->text));
        /* The initialized help panel draws its first message immediately. */
    case 2:
        org = func_A191C0(MenuWork.agws.chrNo);
        targetY = 386;
        if (info->detail == 0) {
            if (PadData.half_2a & PAD_CIRCLE)
                info->detail = 1;
        } else if (PadData.mappedRepeat != 0) {
            info->detail = 0;
        }
        /* Help text for each state. */
        switch (MenuWork.agws.state) {
        case AGWS_STATE_SELECT:
            eMessageCpy(info->text, msg00_1_0036D988[0]);
            break;
        case AGWS_STATE_COMMAND:
            eMessageCpy(info->text, msg01_2_0036D990[MenuWork.agws.reserveCursor]);
            break;
        case AGWS_STATE_EQUIP:
            if (MenuWork.agws.weaponHand != 3) {
                if (MenuWork.agws.equipSlot == 0) {
                    if (org->weaponId[MenuWork.agws.weaponHand] != 0) {
                        const char **mountText = msg02_3_0036D9A0;

                        position = OptMntToWpnPos(org->hand[MenuWork.agws.weaponHand]);
                        eMessageCpy(info->text, mountText[position - 1]);
                        eMessageCat(msg03_4_0036D9B8[0]);
                        eMessageCat(MenuTextGet((unsigned short)org->weaponId[MenuWork.agws.weaponHand] + 0x30000)->auxiliary);
                    } else if (MenuWork.agws.reserveCursor == 0) {
                        eMessageCpy(info->text, msg03_4_0036D9B8[1]);
                    } else {
                        eMessageCpy(info->text, msg03_4_0036D9B8[2]);
                    }
                } else {
                    eMessageCpy(info->text, MenuTextGet(org->ammoId[MenuWork.agws.weaponHand] + 0x40000)->auxiliary);
                    if (MenuWork.agws.reserveCursor == 1 && info->detail == 1)
                        eMessageCpy(info->text, msg03_4_0036D9B8[3]);
                }
            } else {
                short accessoryId = org->accessoryId[MenuWork.agws.equipSlot];

                if (accessoryId != 0) {
                    eMessageCpy(info->text,
                        MenuTextGet((unsigned short)accessoryId + 0x50000)->auxiliary);
                } else if (MenuWork.agws.reserveCursor == 0) {
                    eMessageCpy(info->text, msg03_4_0036D9B8[4]);
                } else {
                    eMessageCpy(info->text, msg03_4_0036D9B8[2]);
                }
            }
            break;
        case AGWS_STATE_WEAPON_MOUNT: {
            const char *mountingMessage;

            position = OptMntToWpnPos(org->hand[MenuWork.agws.weaponHand]) - 1;
            selectedPosition = MenuWork.agws.weaponSlot[MenuWork.agws.selectedIndex] - 1;
            if (position < 0) {
                eMessageCpy(info->text, msg04_5[3]);
                mountingMessage = msg02_3_0036D9A0[selectedPosition];
            } else if (position == selectedPosition) {
                eMessageCpy(info->text, msg04_5[1]);
                eMessageCat(msg02_3_0036D9A0[selectedPosition]);
                eMessageCat(commaText);
                eMessageCat(msg04_5[0]);
                break;
            } else {
                eMessageCpy(info->text, msg04_5[1]);
                eMessageCat(msg02_3_0036D9A0[position]);
                eMessageCat(msg04_5[2]);
                mountingMessage = msg02_3_0036D9A0[selectedPosition];
            }
            eMessageCat(mountingMessage);
            eMessageCat(menuAgwsParaSetPeriodText);
            break;
        }
        case AGWS_STATE_WEAPON_LIST:
            selectedRow = MenuWork.agws.selectedWeapon;
            if (selectedRow >= 0)
                eMessageCpy(info->text, MenuTextGet(MenuSortGet(0, selectedRow))->auxiliary);
            else
                eMessageCpy(info->text, infoClearText);
            break;
        case AGWS_STATE_AMMO_LIST:
            selectedRow = MenuWork.agws.selectedAmmo;
            if (selectedRow >= 0)
                eMessageCpy(info->text, MenuTextGet(MenuSortGet(0, selectedRow))->auxiliary);
            else
                eMessageCpy(info->text, infoClearText);
            break;
        case 0x60: {
            if (MenuWork.agws.weaponHand != 2) {
                const char **categoryText = msg01_2_0036D990;

                eMessageCpy(info->text, categoryText[5]);
                eMessageCat(categoryText[MenuWork.agws.weaponHand + 2]);
            } else {
                eMessageCpy(info->text, msg02_3_0036D9A0[0]);
            }
            break;
        }
        case AGWS_STATE_ACCESSORY_LIST:
            selectedRow = MenuWork.agws.selectedAccessory;
            if (selectedRow >= 0)
                eMessageCpy(info->text, MenuTextGet(MenuSortGet(0, selectedRow))->auxiliary);
            else
                eMessageCpy(info->text, infoClearText);
            break;
        case AGWS_STATE_PILOT_LIST:
            if (MenuWork.agws.listSelect >= 0) {
                row = &MenuListGet(0)[MenuWork.agws.listSelect];
                if (row->flag == 1) {
                    AgwsUnitOrg *selectedOrg = func_A191C0((short)MenuSortAddrGet(0)[MenuWork.agws.listSelect]);

                    eMessageCpy(info->text, row->name);
                    eMessageCat(msg05_6[2]);
                    eMessageCat(MenuCharNameGet(selectedOrg->agwsId));
                    eMessageCat(msg05_6[3]);
                } else {
                    if (row->status == 0)
                        eMessageCpy(info->text, msg05_6[0]);
                    else
                        eMessageCpy(info->text, msg05_6[1]);
                    eMessageCat(row->name);
                    eMessageCat(msg05_6[4]);
                }
            } else {
                eMessageCpy(info->text, infoClearText);
            }
            break;
        default:
            targetY = 512;
            break;
        }
        info->contents.text = info->text;
        MoveSlide(&info->window.y, &targetY, 3.0f);
        WindowDXMain(&info->window);
        break;
    }
}

/*
 * Opens the four select windows on the first call, then slides them to the
 * targets for the current AGWS state.
 */
void AgwsSelectMain(void)
{
    short targetX[4];
    short targetY[4];
    AgwsSelectWork *select = AgwsSelect;
    int i;

    switch (select->state) {
    case 0: {
        select->color = 0x00FFFFF0;
        for (i = 0; i <= 3; i++) {
            const char **menuText;
            WindowDXSet((WindowDX *)(&select->window[i]));
            menuText = msg_7_0036D9F8;
            select->window[i].color = select->color;
            select->window[i].title = menuWindowTitleText;
            select->contents[i].selectedIndex = 0;
            select->contents[i].text = menuText[1];
            select->contents[i].mode = 0;
            select->window[i].callback.function =
                (void (*)(void *, void *))MenuSelectWindow;
            select->window[i].callbackArg.contents = &select->contents[i];
        }
        select->contents[2].text = msg01_8_0036DA40[0];
        select->window[3].title = selectWindowTitleText;
        select->contents[3].text = msg01_8_0036DA40[2];
        select->window[0].x = 544;
        select->window[0].y = 160;
        select->window[0].width = 160;
        select->window[0].height = 78;
        select->window[1].x = -192;
        select->window[1].y = 128;
        select->window[1].width = 166;
        select->window[1].height = 0;
        select->window[2].x = 544;
        select->window[2].y = 176;
        select->window[2].width = 176;
        select->window[2].height = 96;
        select->window[3].x = 544;
        select->window[3].y = 128;
        select->window[3].width = 176;
        select->window[3].height = 112;
        select->contents[3].mode = 1;
        select->contents[3].annotation = msg01_8_0036DA40[1];
        for (i = 0; i < 4; i++) {
            select->window[i].state = WINDOWDX_STATE_OPEN;
            WindowDXMain((WindowDX *)(&select->window[i]));
            select->window[i].state = WINDOWDX_STATE_READY;
        }
        select->visible = 1;
        select->selectNo = -1;
        select->state = 2;
    }
        /* The new windows immediately enter their slide animation. */
    case 2:
        targetX[0] = 544;
        targetY[0] = 160;
        targetX[1] = -192;
        targetY[1] = 128;
        targetX[2] = 544;
        targetY[2] = 176;
        targetX[3] = 544;
        targetY[3] = 128;
        /*
         * Per-state targets: the command window moves left, and the mount
         * window lists the equipped weapons.
         */
        switch (MenuWork.agws.state) {
        case AGWS_STATE_COMMAND:
            targetX[0] = 272;
            select->contents[0].selectedIndex = MenuWork.agws.reserveCursor;
            break;
        case AGWS_STATE_WEAPON_MOUNT:
            mmsg_9[0] = emptyMessageText[0];
            i = 0;
            while (MenuWork.agws.weaponSlot[i] != 0) {
                strcat(mmsg_9, msg_7_0036D9F8[MenuWork.agws.weaponSlot[i] + 10]);
                if (++i >= 6)
                    break;
            }
            select->window[1].height = MenuWork.agws.nameRow * 24 + 6;
            select->contents[1].text = mmsg_9;
            targetX[1] = 48;
            select->contents[1].selectedIndex = MenuWork.agws.selectedIndex;
            break;
        }
        for (i = 0; i < 4; i++) {
            MoveSlide(&select->window[i].x, &targetX[i], 3.0f);
            MoveSlide(&select->window[i].y, &targetY[i], 3.0f);
            WindowDXMain((WindowDX *)(&select->window[i]));
        }
        break;
    }
}

/*
 * Builds the weapon list for the selected slot; entries that fail MenuWeaponEquipPosCheck get flag
 * 0.
 */
void MenuAgwsListMake_Wpn(void)
{
    int *sortRow = MenuSortAddrGet(0);
    MenuListEntry *entries = MenuListGet(0);
    WindowSPCursor *window = &AgwsList->window[0];
    int count;
    int i;

    MenuSortSet(0, 4, MenuWork.agws.weaponSlot[MenuWork.agws.selectedIndex] | 0x2000);
    MenuListMake(0, 0);
    count = MenuSortCheck(0);
    for (i = 0; i < count; i++) {
        if (MenuWeaponEquipPosCheck(MenuWork.agws.chrNo, (short) sortRow[i],
                                    MenuWork.agws.weaponSlot[MenuWork.agws.selectedIndex],
                                    MenuWork.agws.weaponHand) != 0) {
            entries[i].flag = 0;
        } else {
            entries[i].flag = 1;
        }
    }
    window->width = 229;
    window->rows = 7;
    window->height = 174;
    window->columns = 1;
    window->draw_flags = 7;
    window->title = 0;
    window->items = MenuListGet(0);
    WindowSPItemChange(window);
    WindowSPSetSelect(window, &MenuKeepSelect[MenuWork.agws.weaponSlot[MenuWork.agws.selectedIndex] * 5 - 5]);
}

/* Builds the ammo list for the selected weapon; entries that fail MenuBulletCheck get flag 0. */
void MenuAgwsListMake_Gun(void)
{
    int *sortRow = MenuSortAddrGet(0);
    MenuListEntry *entries = MenuListGet(0);
    WindowSPCursor *window = &AgwsList->window[0];
    int count;
    int i;

    MenuSortSet(0, 8, -2);
    MenuListMake(0, 0);
    count = MenuSortCheck(0);
    for (i = 0; i < count; i++) {
        if (MenuBulletCheck(MenuWork.agws.weaponId, (short) sortRow[i]) != 0) {
            entries[i].flag = 0;
        } else {
            entries[i].flag = 1;
        }
    }
    window->width = 229;
    window->rows = 7;
    window->height = 174;
    window->columns = 1;
    window->draw_flags = 7;
    window->title = 0;
    window->items = MenuListGet(0);
    WindowSPItemChange(window);
    WindowSPSetSelect(window, &MenuKeepSelect[30]);
}

/*
 * Builds the accessory list for the selected equip slot; entries that fail MenuAccessoryEquipCheck
 * get flag 0.
 */
void MenuAgwsListMake_Acc(void)
{
    int *sortRow = MenuSortAddrGet(0);
    MenuListEntry *entries = MenuListGet(0);
    WindowSPCursor *window = &AgwsList->window[0];
    int count;
    int i;

    MenuSortSet(0, 16, -2);
    MenuListMake(0, 0);
    count = MenuSortCheck(0);
    for (i = 0; i < count; i++) {
        if (MenuAccessoryEquipCheck(MenuWork.agws.chrNo, (short) sortRow[i], MenuWork.agws.equipSlot) != 0) {
            entries[i].flag = 0;
        } else {
            entries[i].flag = 1;
        }
    }
    window->width = 229;
    window->height = 150;
    window->columns = 1;
    window->rows = 6;
    window->draw_flags = 7;
    window->title = 0;
    window->items = MenuListGet(0);
    WindowSPItemChange(window);
    WindowSPSetSelect(window, &MenuKeepSelect[35]);
}

/* Builds the pilot list; entries whose AGWS unit is the selected unit get status 1. */
void MenuAgwsListMake_Pilot(void)
{
    MenuListEntry *entries = MenuListGet(0);
    int *sortRow = MenuSortAddrGet(0);
    WindowSPCursor *window = &AgwsList->window[0];
    AgwsUnitOrg *org;
    int count;
    int i;

    MenuSortSet(0, 32, 1);
    MenuListMake(0, 0);
    count = MenuSortCheck(0);
    for (i = 0; i < count; i++) {
        org = func_A191C0((short) sortRow[i]);
        if (org->agwsId != MenuWork.agws.chrNo && org->agwsId != 0) {
            entries[i].flag = 1;
        } else {
            entries[i].flag = 0;
        }
        if (org->agwsId == MenuWork.agws.chrNo) {
            entries[i].status = 1;
        } else {
            entries[i].status = 0;
        }
    }
    window->width = 208;
    window->height = 144;
    window->columns = 1;
    window->rows = 5;
    window->draw_flags = 3;
    window->title = pilotListTitleText;
    window->items = MenuListGet(0);
    WindowSPItemChange(window);
    WindowSPSetSelect(window, &MenuKeepSelect[40]);
}

/*
 * Opens the list window for the current AGWS state, slides it in or out,
 * and draws the pilot tags while the pilot list is shown.
 */
void AgwsListMain(void)
{
    AgwsListWork *list = AgwsList;
    short targetX;
    int i;
    int count;
    MenuListEntry *rows;

    switch (list->state) {
    case 0:
        list->color = 0x00FFF000;
        list->window[0].x = 528;
        list->window[0].style = list->color;
        list->window[0].title = infoClearText;
        list->window[0].y = 64;
        list->window[0].draw_flags = 7;
        list->window[0].rows = 11;
        list->window[0].height = 270;
        list->window[0].state = 0;
        list->window[0].items = 0;
        WindowSPSet(&list->window[0]);
        list->visible = 1;
        list->state = 2;
        list->selectedRow = 0;
        /* fall through */
    case 2:
        list->visible = 0;
        switch (MenuWork.agws.state) {
        case AGWS_STATE_WEAPON_LIST: list->state = 10; break;
        case AGWS_STATE_AMMO_LIST: list->state = 20; break;
        case AGWS_STATE_ACCESSORY_LIST: list->state = 30; break;
        case AGWS_STATE_PILOT_LIST: list->state = 40; break;
        }
        break;
    case 10:
    case 20:
    case 30:
    case 40:
        if (list->state == 40) {
            list->window[0].x = 528;
            list->window[0].y = 128;
            MenuAgwsListMake_Pilot();
            list->visible = 2;
            list->state = 41;
        } else {
            list->window[0].x = -244;
            if (list->state == 30) {
                list->window[0].y = 126;
            } else {
                list->window[0].y = 104;
            }
            list->visible = 1;
            if (list->state == 20) {
                list->state = 21;
            } else {
                list->state = 11;
            }
            switch (list->state) {
            case 10: MenuAgwsListMake_Wpn(); break;
            case 20: MenuAgwsListMake_Gun(); break;
            case 30: MenuAgwsListMake_Acc(); break;
            }
        }
        list->window[0].state = 17;
        /* fall through */
    case 11:
    case 13:
    case 21:
    case 41:
    case 43:
        targetX = 16;
        switch (list->state) {
        case 41: targetX = 288; break;
        case 13: targetX = -244; break;
        case 43: targetX = 528; break;
        }
        MoveSlide(&list->window[0].x, &targetX, 3.0f);
        if (list->window[0].x == targetX) {
            switch (list->state) {
            case 21: list->state = 22; break;
            case 11:
            case 41: list->state = 12; break;
            case 13:
            case 43: list->state = 2; break;
            }
        }
        break;
    case 12:
    case 22:
        switch (MenuWork.agws.state) {
        case AGWS_STATE_WEAPON_MOUNT: list->state = 13; break;
        case AGWS_STATE_COMMAND: list->state = 43; break;
        case AGWS_STATE_EQUIP: list->state = 13; break;
        case AGWS_STATE_AMMO_LIST:
            if (list->state != 12) break;
            list->state = 20;
            break;
        }
        break;
    }
    /* Pilot tags and cursor; visible 2 is set only for the pilot list. */
    if (list->visible != 0) {
        WindowSPMain(&list->window[0]);
        if (list->visible == 2 && MenuWork.agws.listSelect >= 0) {
            count = MenuSortCheck(0);
            rows = MenuListGet(0);
            for (i = 0; i < count; i++) {
                short chrNo = MenuSortGet(0, i);
                if (PartyAttackerCheck(chrNo)) {
                    eTagFontSet(&list->tag[i], MenuTagTextGet(MENU_TAG_COMBATANT));
                } else {
                    eTagFontSet(&list->tag[i], MenuTagTextGet(MENU_TAG_RESERVE));
                }
                if (rows[i].flag == 1) {
                    list->tag[i].color[1] = 64;
                    list->tag[i].color[2] = 64;
                    list->tag[i].color[0] = 64;
                } else {
                    list->tag[i].color[1] = 128;
                    list->tag[i].color[2] = 128;
                    list->tag[i].color[0] = 128;
                }
                list->tag[i].x = list->window[0].x + 107;
                list->tag[i].work = list->window[0].style + 2;
                list->tag[i].y = list->window[0].y + i * 24 + 7;
                eTagFontMain(&list->tag[i]);
                if (chrNo == MenuAgwsPilotCheck(MenuWork.agws.chrNo)) {
                    eSpriteSet(&list->cursor, 0x60A);
                    list->cursor.x = list->window[0].x + list->window[0].width - 24;
                    list->cursor.y = list->window[0].y + i * 24 + 3;
                    list->cursor.depth = list->window[0].style + 2;
                    eSpriteMain(&list->cursor);
                }
            }
        }
    }
}

/*
 * Opens the status window on the first call, slides it for the current AGWS
 * state, and refreshes the six parameter rows.
 */
void AgwsStatusMain(void)
{
    AgwsStatusWork *status = AgwsStatus;
    int i;

    switch (status->state) {
    case AGWS_STATUS_START:
        status->color = AGWS_STATUS_COLOR;
        WindowDXSet(&status->window);
        status->window.x = AGWS_STATUS_X_OUT;
        status->window.y = 304;
        status->window.style = status->color;
        status->window.width = 480;
        status->window.height = 64;
        status->window.state = WINDOWDX_STATE_OPEN;
        WindowDXMain(&status->window);
        status->window.state = WINDOWDX_STATE_READY;
        for (i = 0; i < 7; i++) {
            eTagFontSet(&status->label[i], msg_10[i]);
            status->label[i].work = status->color + 2;
            status->label[i].color[0] = 112;
        }
        {
            unsigned char digit[7] = { 4, 3, 3, 3, 2, 2, 4 };

            for (i = 0; i < 7; i++) {
                eNumberSet(&status->number[i], 0);
                status->number[i].work = status->color + 2;
                status->number[i].format = 5;
                status->number[i].digits = digit[i];
            }
        }
        status->selectNo = -1;
        status->state = AGWS_STATUS_SLIDE;
        status->mask = 0;
        /* fallthrough */
    case AGWS_STATUS_SLIDE:
    {
        short slideTarget = AGWS_STATUS_X_OUT;

        switch (MenuWork.agws.state) {
        case AGWS_STATE_EQUIP:
            slideTarget = AGWS_STATUS_X_IN;
            if (MenuWork.agws.reserveCursor == 1) {
                status->mask = 1;
            } else {
                status->mask = 0;
            }
            break;
        case AGWS_STATE_COMMAND:
            slideTarget = AGWS_STATUS_X_IN;
            break;
        case AGWS_STATE_WEAPON_MOUNT:
        case AGWS_STATE_WEAPON_LIST:
        case AGWS_STATE_AMMO_LIST:
        case AGWS_STATE_ACCESSORY_LIST:
        case AGWS_STATE_PILOT_LIST:
            status->mask = 0;
            slideTarget = AGWS_STATUS_X_IN;
            break;
        }
        MoveSlide(&status->window.x, &slideTarget, 3.0f);
        WindowDXMain(&status->window);
        if (status->window.state == WINDOWDX_STATE_READY) {
            for (i = 0; i < 6; i++) {
                int rows = 3;

                status->label[i].x = status->window.x + (i / rows) * 160 + 48;
                status->label[i].y = status->window.y + (i % rows) * 20 + 3;
                eTagFontMain(&status->label[i]);
            }
            for (i = 0; i < 6; i++) {
                status->number[i].value =
                    MenuAgwsPara.value[i] + (MenuAgwsPara2.value[i] << 16);
            }
            /* The color mode comes from comparing the two packed values of each row. */
            for (i = 0; i < 6; i++) {
                unsigned short shown[2];

                shown[0] = status->number[i].value;
                shown[1] = status->number[i].value >> 16;
                if (shown[0] < shown[1]) {
                    status->number[i].colorModeSecond = 2;
                    status->number[i].firstOnly = 0;
                } else if (shown[1] < shown[0]) {
                    status->number[i].colorModeSecond = 1;
                    status->number[i].firstOnly = 0;
                } else {
                    status->number[i].colorModeSecond = 0;
                    status->number[i].firstOnly = 1;
                }
                {
                    int rows = 3;

                    status->number[i].x =
                        status->window.x + (i / rows) * 160 + 100;
                    status->number[i].y =
                        status->window.y + (i % rows) * 20 + 3;
                }
                eNumberMain(&status->number[i]);
            }
        }
        break;
    }
    }
}

/*
 * Lays out the weapon panel rows on the first call, then slides them for the
 * current AGWS state and draws each row's weapon, ammo and mount icons.
 */
void AgwsWeapon2Main(void)
{
    int i;
    short *bottom;
    int digit;
    AgwsUnitOrg *para;
    int one;
    int lines;
    AgwsWeapon2Work *storage = AgwsWeapon2;
    AgwsWeapon2PanelView *work = &storage->panels;
    int position;
    int lineY;
    int textIndex;
    short px, py, sy;
    short x[4];
    short y[4];
    unsigned int bullet[3];
    unsigned int equipped[3];
    const char *text[2];
    AgwsWeapon2MountRow mountRow;
    AgwsWeapon2MountSide mountSide;
    short icon[2];

    switch (work->state) {
    case AGWS_WEAPON2_SETUP:
        work->color = 0xFFFF00;
        for (i = 0; i < 4; i++) {
            WindowDXSet(&work->row[i].window);
            work->row[i].window.y = 0x30 + i * 0x36;
            work->row[i].window.style = work->color;
            work->row[i].window.height = 0x36;
            work->row[i].window.x = -0x102;
            work->row[i].window.width = 0xE5;
            if (i == 3) {
                bottom = (short *) ((char *) work + 0x920);
                bottom[11] = 0x4E;
            } else {
                work->row[i].window.height = 0x36;
            }
            work->row[i].window.state = WINDOWDX_STATE_OPEN;
            WindowDXMain(&work->row[i].window);
            work->row[i].window.state = WINDOWDX_STATE_READY;
        }
        eCursolSet((ECursolMainData *)&storage->cursor.cursol, 0);
        work->mode = 0;
        work->state = AGWS_WEAPON2_MAIN;
        /* fallthrough */
    case AGWS_WEAPON2_MAIN:
        para = func_A191C0(MenuWork.agws.chrNo);
        for (i = 0; i < 4; i++) {
            x[i] = -0x102;
            y[i] = 0x30 + i * 0x36;
        }
        switch (MenuWork.agws.state) {
        case AGWS_STATE_EQUIP:
            for (i = 0; i < 4; i++)
                x[i] = 0x10;
            storage->cursor.cursol.control.state = E_MODE_SHOW_START;
            break;
        case AGWS_STATE_COMMAND:
            storage->cursor.cursol.control.state = 0;
            break;
        case AGWS_STATE_WEAPON_MOUNT:
        case AGWS_STATE_WEAPON_LIST:
        case AGWS_STATE_AMMO_LIST:
            x[MenuWork.agws.weaponHand] = 0x10;
            y[MenuWork.agws.weaponHand] = 0x30;
            break;
        case AGWS_STATE_ACCESSORY_LIST:
            x[3] = 0x10;
            y[3] = 0x30;
            break;
        case AGWS_STATE_PILOT_LIST:
            break;
        }
        for (i = 0; i < 4; i++) {
            MoveSlide(&work->row[i].window.x, &x[i], 3.0f);
            MoveSlide(&work->row[i].window.y, &y[i], 3.0f);
        }
        /* Rows 0 to 2: weapon name, ammo name, and mount icons. */
        for (i = 0; i < 3; i++) {
            WindowDXMain(&work->row[i].window);
            one = 1;
            if (para->weaponId[i] != 0)
                equipped[i] = one;
            else
                equipped[i] = 0;
            bullet[i] = MenuBulletCheck(para->weaponId[i], -1);
            text[0] = MenuTextGet(MENU_TEXT_WEAPON + (unsigned short)para->weaponId[i])->name;
            text[1] = MenuTextGet(MENU_TEXT_BULLET + para->ammoId[i])->name;
            lines = 1;
            if (equipped[i] != 0) {
                lines = 2;
                if (bullet[i] == 0)
                    lines = 1;
            }
            for (textIndex = 0; textIndex < lines; textIndex++) {
                eMessageSet(&work->row[i].message[textIndex], text[textIndex]);
                work->row[i].message[textIndex].mode = E_MODE_SHOW_START;
                work->row[i].message[textIndex].draw_parameter = work->row[i].window.style + 2;
                work->row[i].message[textIndex].x = work->row[i].window.x + 39;
                work->row[i].message[textIndex].y = work->row[i].window.y + ((lines & 1) * 12 + textIndex * 24) + 3;
                eMessageMain(&work->row[i].message[textIndex]);
            }
            if (equipped[i] != 0) {
                mountRow = weapon2MountRowTable[0];
                mountSide = weapon2MountSideTable[0];
                position = OptMntToWpnPos(para->hand[i]) - 1;
                icon[0] = mountRow.icon[position / 2];
                icon[1] = mountSide.icon[position & 1];
                for (digit = 0; digit < 2; digit++) {
                    eSpriteSet(&work->row[i].sprite[digit], icon[digit]);
                    px = work->row[i].window.x + digit + 19;
                    work->row[i].sprite[digit].x = px;
                    sy = work->row[i].window.y + (digit * 14 + (bullet[i] ^ 1) * 12) + 3;
                    work->row[i].sprite[digit].y = sy;
                    work->row[i].sprite[digit].color = work->row[i].window.style + digit * 2 + 2;
                    eSpriteMain(&work->row[i].sprite[digit]);
                }
                if (bullet[i] != 0) {
                    eSpriteSet(&work->row[i].sprite[2], 0x31B);
                    px = work->row[i].window.x + 19;
                    work->row[i].sprite[2].x = px;
                    work->row[i].sprite[2].color = work->row[i].window.style + 2;
                    py = work->row[i].window.y + 27;
                    work->row[i].sprite[2].y = py;
                    eSpriteMain(&work->row[i].sprite[2]);
                }
            } else {
                eSpriteSet(&work->row[i].sprite[2], 0x321);
                work->row[i].sprite[2].x = work->row[i].window.x + 19;
                work->row[i].sprite[2].color = work->row[i].window.style + 2;
                work->row[i].sprite[2].y = work->row[i].window.y + 15;
                eSpriteMain(&work->row[i].sprite[2]);
            }
        }
        /* Row 3: the three accessory slots. */
        WindowDXMain(&work->row[3].window);
        for (i = 0; i < 3; i++) {
            eSpriteSet(&work->row[3].sprite[i], 0x315);
            work->row[3].sprite[i].x = work->row[3].window.x + 19;
            work->row[3].sprite[i].color = work->row[3].window.style + 2;
            work->row[3].sprite[i].y = work->row[3].window.y + i * 24 + 3;
            eSpriteMain(&work->row[3].sprite[i]);
            eMessageSet(&work->row[3].message[i],
                        MenuTextGet(MenuIdChange(para->accessoryId[i], 5))->name);
            work->row[3].message[i].mode = E_MODE_SHOW_START;
            work->row[3].message[i].x = work->row[3].window.x + 39;
            work->row[3].message[i].draw_parameter = work->row[3].window.style + 2;
            work->row[3].message[i].y = work->row[3].window.y + i * 24 + 3;
            eMessageMain(&work->row[3].message[i]);
        }
        /* The cursor follows the selected hand and equip slot. */
        storage->cursor.cursol.control.x = work->row[MenuWork.agws.weaponHand].window.x + 3;
        lineY = (unsigned short)work->row[MenuWork.agws.weaponHand].window.y
                + MenuWork.agws.equipSlot * 24;
        storage->cursor.cursol.control.y = lineY + 7;
        if (MenuWork.agws.weaponHand != 3
                && (equipped[MenuWork.agws.weaponHand] == 0
                    || bullet[MenuWork.agws.weaponHand] == 0))
            storage->cursor.cursol.control.y = lineY + 19;
        storage->cursor.cursol.control.work = work->color + 2;
        eCursolMain((ECursolMainData *)&storage->cursor.cursol);
        break;
    }
}

/*
 * Drives the name panels of the AGWS units: builds them on the first call,
 * then slides and selects them for the current AGWS state.
 */
void AgwsNameMain(void) {
    int settledRows;
    int rowIndex;
    short slideTargetY;
    int titleLeft;
    int titleTop;
    int nameRight;
    int nameLeft;
    int nameTop;
    int pilot;
    int portraitLeft;
    int portraitRight;
    int portraitBottom;
    int pilotLeft;
    int pilotRight;
    int pilotTop;
    int titleRight;
    int messageDepth;
    int portraitTop;
    int pilotBottom;
    int titleBottom;
    int nameBottom;
    int portraitDepth;
    int pilotDepth;
    int titleDepth;
    int nameDepth;
    int cursorY;
    unsigned short character;
    AgwsNameWorkPrefix *name;
    char *pilotText;
    AgwsNamePoint *portraitRibbon;
    AgwsNamePoint *pilotRibbon;
    AgwsNamePoint *titleRibbon;
    AgwsNamePoint *nameRibbon;
    AgwsWeapon2Cursol *cursor;

    name = AgwsName;
    func_A191C0(MenuWork.agws.chrNo);
    switch (name->state) {
    case 0x0:
    {
        int band;
        int pilot;
        int face;
        int depth;

        name->color = 0xFFFFF0;
        for (rowIndex = 0; rowIndex < MenuWork.agws.agwsCount; rowIndex++) {
            int character = AgwsNo[rowIndex];
            func_A191C0(character);
            pilot = MenuAgwsPilotCheck(character);
            WindowDXSet(&name->window[rowIndex]);
            name->window[rowIndex].x = (rowIndex & 1) * 0x320 - 0x100;
            name->window[rowIndex].y = rowIndex / 2 * 0x60 + 0x40;
            name->window[rowIndex].style = name->color;
            name->window[rowIndex].width = 0xE0;
            name->window[rowIndex].height = 0x36;
            if (pilot == 0)
                name->window[rowIndex].tag_id = MenuTagTextGet(MENU_TAG_RESERVE);
            else if (PartyAttackerCheck(pilot) != 0)
                name->window[rowIndex].tag_id = MenuTagTextGet(MENU_TAG_REGISTERED_COMBATANT);
            else
                name->window[rowIndex].tag_id = MenuTagTextGet(MENU_TAG_REGISTERED_RESERVE);
            name->window[rowIndex].ribbon_initialized = 1;
            name->window[rowIndex].state = WINDOWDX_STATE_OPEN;
            WindowDXMain(&name->window[rowIndex]);
            name->window[rowIndex].state = WINDOWDX_STATE_READY;
            eMessageSet(&name->message[rowIndex][0], MenuCharNameGet(character));
            name->message[rowIndex][0].draw_parameter = name->color + 2;
            name->message[rowIndex][0].mode = E_MODE_SHOW_START;
            eMessageSet(&name->message[rowIndex][1], MenuCharNameGet(pilot));
            name->message[rowIndex][1].draw_parameter = name->color + 4;
            name->message[rowIndex][1].mode = E_MODE_SHOW_START;
            for (band = 0; band < 4; band++) {
                AgwsNamePoint *ribbonColors;
                ribbonColors = &name->ribbon[rowIndex][band * 5 + 3];
                ribbonColors[0].b = 1;
                ribbonColors[-1].b = 1;
                ribbonColors[-2].b = 1;
                ribbonColors[-3].b = 1;
                ribbonColors[0].g = 1;
                ribbonColors[-1].g = 1;
                ribbonColors[-2].g = 1;
                ribbonColors[-3].g = 1;
                ribbonColors[0].r = 1;
                ribbonColors[-1].r = 1;
                ribbonColors[-2].r = 1;
                ribbonColors[-3].r = 1;
                depth = name->color;
                name->ribbon[rowIndex][band * 5 + 3].depth = depth;
                name->ribbon[rowIndex][band * 5 + 2].depth = depth;
                name->ribbon[rowIndex][band * 5 + 1].depth = depth;
                name->ribbon[rowIndex][band * 5 + 0].depth = depth;
                name->ribbon[rowIndex][band * 5 + 4].depth = 0;
                ribbonColors[-2].a = 0x80;
                ribbonColors[-3].a = 0x80;
                ribbonColors[0].a = 0;
                ribbonColors[-1].a = 0;
            }
            face = MenuFaceEpidGet(character, 0);
            eSpriteSet(&name->portrait[rowIndex], face);
            name->portrait[rowIndex].color = name->color + 1;
        }
        eCursolSet((ECursolMainData *)&name->cursol, 0);
        name->mode = 0;
        name->selection = -1;
        name->state = 0xA;
        name->slide = 0;
        name->timer = 0;
        break;
    }
    case 0xA:
        for (rowIndex = 0; rowIndex < MenuWork.agws.agwsCount; rowIndex++) {
            name->window[rowIndex].y = rowIndex / 2 * 0x60 + 0x40;
            name->window[rowIndex].x = (rowIndex & 1) * 0x320 - 0x100;
        }
        name->cursol.state = E_MODE_SHOW_START;
        name->state = 0xB;
        /* fallthrough */
    /* 0xB slides the panels into two columns; 0x33 slides them back to the off-screen x they started from. */
    case 0xB:
    case 0x33:
        settledRows = 0;
        for (rowIndex = 0; rowIndex < MenuWork.agws.agwsCount; rowIndex++) {
            short target;
            if (name->state == 0xB)
                target = (rowIndex & 1) * 0xF0 + 0x18;
            else
                target = (rowIndex & 1) * 0x320 - 0x100;
            MoveSlide(&name->window[rowIndex].x, &target, 3.0f);
            if (name->window[rowIndex].x == target)
                settledRows++;
        }
        if (settledRows == MenuWork.agws.agwsCount) {
            if (name->state == 0xB)
                name->state = 0xC;
            else
                name->state = 2;
        }
        break;
    case 0xC:
        switch (MenuWork.agws.state) {
        case AGWS_STATE_COMMAND:
            name->state = 0x14;
            break;
        case AGWS_STATE_CLOSE_WAIT:
            name->state = 0x33;
            break;
        }
        break;
    case 0x14:
        name->cursol.state = 0;
        name->state = 0x15;
        /* fallthrough */
    /*
     * 0x15 moves the selected unit's panel to the front position; the others return to their grid
     * cells.
     */
    case 0x15:
        settledRows = 0;
        for (rowIndex = 0; rowIndex < MenuWork.agws.agwsCount; rowIndex++) {
            short targetX, targetY;
            if (rowIndex == MenuWork.agws.selectedAgws) {
                targetX = 0x110;
                targetY = 0x18;
            } else {
                targetX = (rowIndex & 1) * 0x320 - 0x100;
                targetY = rowIndex / 2 * 0x60 + 0x40;
            }
            MoveSlide(&name->window[rowIndex].x, &targetX, 3.0f);
            if (name->window[rowIndex].x == targetX) {
                MoveSlide(&name->window[rowIndex].y, &targetY, 3.0f);
                if (name->window[rowIndex].y == targetY)
                    settledRows++;
            }
        }
        if (settledRows == MenuWork.agws.agwsCount)
            name->state = 0x16;
        break;
    case 0x3D:
        settledRows = 0;
        for (rowIndex = 0; rowIndex < MenuWork.agws.agwsCount; rowIndex++) {
            short targetY;
            short targetX;
            targetX = (rowIndex & 1) * 0xF0 + 0x18;
            targetY = rowIndex / 2 * 0x60 + 0x40;
            MoveSlide(&name->window[rowIndex].y, &targetY, 3.0f);
            if (name->window[rowIndex].y == targetY) {
                MoveSlide(&name->window[rowIndex].x, &targetX, 3.0f);
                if (name->window[rowIndex].x == targetX)
                    settledRows++;
            }
        }
        if (settledRows == MenuWork.agws.agwsCount)
            name->state = 0xC;
        break;
    case 0x16:
        switch (MenuWork.agws.state) {
        case AGWS_STATE_SELECT:
            name->cursol.state = E_MODE_SHOW_START;
            name->state = 0x3D;
            break;
        case AGWS_STATE_EQUIP:
            name->state = 0x1E;
            break;
        }
        for (rowIndex = 0; rowIndex < MenuWork.agws.agwsCount; rowIndex++) {
            if (rowIndex == MenuWork.agws.selectedAgws) {
                name->window[rowIndex].x = 0x110;
                name->window[rowIndex].y = 0x18;
            } else {
                name->window[rowIndex].x = (rowIndex & 1) * 0x320 - 0x100;
                name->window[rowIndex].y = rowIndex / 2 * 0x60 + 0x40;
            }
        }
        break;
    case 0x1E:
    case 0x28:
        slideTargetY = 0x18;
        if (name->state == 0x1E)
            slideTargetY = -0x40;
        MoveSlide(&name->window[MenuWork.agws.selectedAgws].y, &slideTargetY, 3.0f);
        if (name->window[MenuWork.agws.selectedAgws].y == slideTargetY) {
            if (name->state == 0x1E)
                name->state = 0x20;
            else
                name->state = 0x16;
        }
        break;
    case 0x20:
        if (MenuWork.agws.state == 0x22)
            name->state = 0x28;
        break;
    }
    cursor = &name->cursol;
    rowIndex = 0;
    if (MenuWork.agws.agwsCount > 0) {
        for (rowIndex = 0; rowIndex < MenuWork.agws.agwsCount; rowIndex++) {
            func_A191C0(AgwsNo[rowIndex]);
            if (rowIndex == MenuWork.agws.selectedAgws) {
                name->window[rowIndex].style = 0xFFFF00;
            } else {
                name->window[rowIndex].style = 0xFFF000;
            }
            pilot = MenuAgwsPilotCheck(AgwsNo[rowIndex]);
            if (pilot == 0) {
                name->window[rowIndex].title_font.render.text =MenuTagTextGet(MENU_TAG_RESERVE);
            } else if (PartyAttackerCheck(pilot) != 0) {
                name->window[rowIndex].title_font.render.text =MenuTagTextGet(MENU_TAG_REGISTERED_COMBATANT);
            } else {
                name->window[rowIndex].title_font.render.text =MenuTagTextGet(MENU_TAG_REGISTERED_RESERVE);
            }
            WindowDXMain(&name->window[rowIndex]);
            portraitRibbon = &name->ribbon[rowIndex][0];
            portraitLeft = (unsigned short)name->window[rowIndex].x + 3;
            portraitDepth = name->window[rowIndex].style;
            portraitRibbon[1].x = (short) portraitLeft;
            portraitRibbon[0].x = (short) portraitLeft;
            name->ribbon[rowIndex][3].depth = portraitDepth;
            portraitRight = (unsigned short)name->window[rowIndex].x + 0x53;
            name->ribbon[rowIndex][3].x = (short) portraitRight;
            name->ribbon[rowIndex][1].depth = portraitDepth;
            name->ribbon[rowIndex][2].x = (short) portraitRight;
            name->ribbon[rowIndex][2].depth = portraitDepth;
            name->ribbon[rowIndex][0].depth = portraitDepth;
            portraitTop = (unsigned short)name->window[rowIndex].y + 3;
            name->ribbon[rowIndex][2].y = (short) portraitTop;
            name->ribbon[rowIndex][0].y = (short) portraitTop;
            portraitBottom = ((unsigned short)name->window[rowIndex].y + (unsigned short)name->window[rowIndex].height) - 3;
            name->ribbon[rowIndex][3].y = (short) portraitBottom;
            name->ribbon[rowIndex][1].y = (short) portraitBottom;
            endPrintExtFunc(name->color, END_PRINT_CMD_RIBBON,portraitRibbon);
            pilotRibbon = &name->ribbon[rowIndex][5];
            pilotLeft = ((unsigned short)name->window[rowIndex].x + (unsigned short)name->window[rowIndex].width) - 0x8E;
            pilotDepth = name->window[rowIndex].style;
            name->ribbon[rowIndex][6].x = (short) pilotLeft;
            name->ribbon[rowIndex][5].x = (short) pilotLeft;
            name->ribbon[rowIndex][8].depth = pilotDepth;
            name->ribbon[rowIndex][6].depth = pilotDepth;
            pilotRight = ((unsigned short)name->window[rowIndex].x + (unsigned short)name->window[rowIndex].width) - 3;
            name->ribbon[rowIndex][7].depth = pilotDepth;
            name->ribbon[rowIndex][5].depth = pilotDepth;
            name->ribbon[rowIndex][8].x = (short) pilotRight;
            name->ribbon[rowIndex][7].x = (short) pilotRight;
            pilotTop = (unsigned short)name->window[rowIndex].y + 0x1B;
            name->ribbon[rowIndex][7].y = (short) pilotTop;
            name->ribbon[rowIndex][5].y = (short) pilotTop;
            pilotBottom = ((unsigned short)name->window[rowIndex].y + (unsigned short)name->window[rowIndex].height) - 3;
            name->ribbon[rowIndex][8].y = (short) pilotBottom;
            name->ribbon[rowIndex][6].y = (short) pilotBottom;
            endPrintExtFunc(name->color, END_PRINT_CMD_RIBBON,pilotRibbon);
            titleRibbon = &name->ribbon[rowIndex][10];
            titleRight = ((unsigned short)name->window[rowIndex].x + (unsigned short)name->window[rowIndex].width) - 3;
            titleDepth = name->window[rowIndex].style;
            name->ribbon[rowIndex][11].x = (short) titleRight;
            name->ribbon[rowIndex][10].x = (short) titleRight;
            name->ribbon[rowIndex][13].depth = titleDepth;
            name->ribbon[rowIndex][11].depth = titleDepth;
            titleLeft = ((unsigned short)name->window[rowIndex].x + (unsigned short)name->window[rowIndex].width) - 0xA0;
            name->ribbon[rowIndex][12].depth = titleDepth;
            name->ribbon[rowIndex][10].depth = titleDepth;
            name->ribbon[rowIndex][13].x = (short) titleLeft;
            name->ribbon[rowIndex][12].x = (short) titleLeft;
            titleTop = (unsigned short)name->window[rowIndex].y + 3;
            name->ribbon[rowIndex][12].y = (short) titleTop;
            name->ribbon[rowIndex][10].y = (short) titleTop;
            titleBottom = (unsigned short)name->window[rowIndex].y + 0x1B;
            name->ribbon[rowIndex][13].y = (short) titleBottom;
            name->ribbon[rowIndex][11].y = (short) titleBottom;
            endPrintExtFunc(name->color, END_PRINT_CMD_RIBBON,titleRibbon);
            name->portrait[rowIndex].x = (short) ((unsigned short)name->window[rowIndex].x + 0x13);
            name->portrait[rowIndex].color = name->window[rowIndex].style;
            name->portrait[rowIndex].y = (unsigned short)name->window[rowIndex].y + 3;
            eSpriteMain(&name->portrait[rowIndex]);
            nameRibbon = &name->ribbon[rowIndex][15];
            nameRight = ((unsigned short)name->window[rowIndex].x + (unsigned short)name->window[rowIndex].width) - 0x8C;
            nameDepth = name->window[rowIndex].style;
            name->ribbon[rowIndex][16].x = (short) nameRight;
            name->ribbon[rowIndex][15].x = (short) nameRight;
            name->ribbon[rowIndex][18].depth = nameDepth;
            name->ribbon[rowIndex][16].depth = nameDepth;
            name->ribbon[rowIndex][17].depth = nameDepth;
            name->ribbon[rowIndex][15].depth = nameDepth;
            nameLeft = ((unsigned short)name->window[rowIndex].x + (unsigned short)name->window[rowIndex].width) - 0xB4;
            name->ribbon[rowIndex][18].x = (short) nameLeft;
            name->ribbon[rowIndex][17].x = (short) nameLeft;
            nameTop = (unsigned short)name->window[rowIndex].y + 0x1B;
            name->ribbon[rowIndex][17].y = (short) nameTop;
            name->ribbon[rowIndex][15].y = (short) nameTop;
            nameBottom = ((unsigned short)name->window[rowIndex].y + (unsigned short)name->window[rowIndex].height) - 3;
            name->ribbon[rowIndex][18].y = (short) nameBottom;
            name->ribbon[rowIndex][16].y = (short) nameBottom;
            endPrintExtFunc(name->color, END_PRINT_CMD_RIBBON,nameRibbon);
            name->message[rowIndex][0].x = (short) ((unsigned short)name->window[rowIndex].x + 0x3C);
            name->message[rowIndex][0].draw_parameter = name->window[rowIndex].style + 2;
            name->message[rowIndex][0].y = (unsigned short)name->window[rowIndex].y + 0x1B;
            eMessageMain(&name->message[rowIndex][0]);
            name->message[rowIndex][1].x = ((unsigned short)name->window[rowIndex].x + (unsigned short)name->window[rowIndex].width) - 0x53;
            name->message[rowIndex][1].y = (unsigned short)name->window[rowIndex].y + 3;
            character = AgwsNo[rowIndex];
            pilotText = MenuCharNameGet(MenuAgwsPilotCheck(character));
            messageDepth = name->window[rowIndex].style;
            name->message[rowIndex][1].text = pilotText;
            name->message[rowIndex][1].draw_parameter = messageDepth + 2;
            eMessageMain(&name->message[rowIndex][1]);
        }
    }
    name->cursol.x = (unsigned short)name->window[MenuWork.agws.selectedAgws].x + 3;
    cursorY = (unsigned short)name->window[MenuWork.agws.selectedAgws].y + 0x13;
    name->cursol.color = name->color + 3;
    name->cursol.y = cursorY;
    eCursolMain((ECursolMainData *)cursor);
    endPrintExtFunc(name->color, END_PRINT_CMD_SCISSOR_RESET, NULL);
}

/*
 * Opens the face panel on the first call, then slides it in while the pilot
 * list is open and draws the face of the highlighted pilot.
 */
void AgwsFaceMain(void)
{
    AgwsFaceWork *self = AgwsFace;
    short target;

    switch (self->state) {
    case 0:
        self->color = 0x00FFFFF0;
        WindowDXSet(&self->window);
        self->window.x = -103;
        self->window.style = self->color;
        self->window.y = 128;
        self->window.width = 71;
        self->window.height = 100;
        self->window.state = WINDOWDX_STATE_OPEN;
        WindowDXMain(&self->window);
        self->window.state = WINDOWDX_STATE_READY;
        eSpriteSet(&self->face, 0);
        self->visible = 0;
        self->state = 2;
        break;
    case 2:
        break;
    default:
        return;
    }

    target = -103;
    if (MenuWork.agws.state == 0xC2) {
        if (MenuWork.agws.flags & 1) {
            self->visible = 0;
        } else {
            self->visible = 1;
        }
        target = 208;
    }
    if (self->visible != 0) {
        MoveSlide(&self->window.x, &target, 3.0f);
        if (self->window.x == -103) {
            self->visible = 0;
        }
        WindowDXMain(&self->window);
        if (MenuWork.agws.listSelect >= 0) {
            eSpriteSet(&self->face, MenuFaceEpidGet((short)MenuSortGet(0, MenuWork.agws.listSelect), 0));
            self->face.x = self->window.x + 3;
            self->face.y = self->window.y + 3;
            self->face.work = self->window.style + 2;
            eSpriteMain(&self->face);
        }
    }
}

/*
 * Opens the two switch sprites on the first call, then slides them for the
 * current AGWS state. In state 34, L1 and R1 nudge the matching sprite.
 */
void AgwsSwitchMain(void)
{
    AgwsSwitchWork *self = AgwsSwitch;
    int i;

    switch (self->state) {
    case 0:
        self->spriteWork = 0x00FFFFFF;
        eSpriteSet(&self->sprite[0], 0x112);
        eSpriteSet(&self->sprite[1], 0x110);
        self->sprite[0].x = -45;
        self->sprite[1].x = 557;
        for (i = 0; i < 2; i++) {
            self->sprite[i].work = self->spriteWork;
            self->sprite[i].y = 224;
        }
        self->slideOffset[1] = 0;
        self->state = 2;
        self->slideOffset[0] = 0;
        break;
    case 2:
        break;
    default:
        return;
    }

    {
        short target[2];

        target[0] = -45;
        target[1] = 557;
        if (MenuWork.agws.state == 34) {
            target[0] = 16;
            target[1] = 467;
            if (PadData.half_2a & PAD_L1) {
                self->slideOffset[0] = -6;
            } else if (PadData.half_2a & PAD_R1) {
                self->slideOffset[1] = 6;
            }
        }

        {
            AgwsSwitchSlideStep step = switchSlideStepTable[0];

            for (i = 0; i < 2; i++) {
                if (self->slideOffset[i] != 0) {
                    self->slideOffset[i] = self->slideOffset[i] + step.side[i];
                }
            }
        }

        for (i = 0; i < 2; i++) {
            MoveSlide(&self->sprite[i].x, &target[i], 3.0f);
            self->sprite[i].x = self->sprite[i].x + self->slideOffset[i];
            eSpriteMain(&self->sprite[i]);
        }
    }
}

/* Resets the AGWS camera to the preset position and rotation. */
void MenuAgwsCameraSet(void)
{
    AgwsCameraPreset preset = cameraSetPreset;
    StudioCamera *camera;

    camera = xglStudioGetCamera2(0);
    xglCameraInit(camera);
    camera->position.vector = preset.vectors[0];
    camera->state = 1;
    camera->rotation.vector = preset.vectors[1];
}

/* Moves the AGWS camera a tenth of the way toward the target for the current state. */
void MenuAgwsCameraMove(void)
{
    Vector4 *position = &xglStudioGetCamera2(0)->position.vector;
    AgwsCameraTargets targets = cameraMoveTargets;
    int target;

    switch (MenuWork.agws.state) {
    case AGWS_STATE_SELECT:
    case AGWS_STATE_COMMAND:
    case AGWS_STATE_PILOT_LIST:
        target = 0;
        break;
    default:
        target = 1;
        break;
    }
    position->x += (targets.value[target][0] - position->x) * 0.1f;
    position->y += (targets.value[target][1] - position->y) * 0.1f;
    position->z += (targets.value[target][2] - position->z) * 0.1f;
}

/*
 * Per-frame update of one AGWS model unit: moves it toward its slot position,
 * then runs its open, weapon-refresh and close states.
 */
void MenuAgwsModelMain(MenuModelUnitAgwsView *self)
{
    AgwsModelActor *actor = self->actor;
    AgwsModelTransform *transform;
    int weaponIndex;

    if (actor == 0) {
        if (MenuWork.agws.modelBreak != 0 || MenuWork.agws.state == 0x10) {
            MenuModelUnitBreak(self);
            MenuWork.agws.modelBreak = 0;
        }
    }
    if (self->active == 0) {
        return;
    }
    transform = &actor->transform;
    if (MenuWork.agws.state == 0x22) {
        MenuModelControl2(self, 1);
    } else {
        AgwsModelPositions positions = modelPositionTable;
        DoublewordAlignedVector4 rotation = modelUnitScaleVector;
        Vector4 *position = &transform->position;
        Vector4 *rotationOf = &transform->rotation;
        Vector4 *from = &positions.position[0];

        switch (MenuWork.agws.state) {
        case AGWS_STATE_WEAPON_MOUNT:
        case AGWS_STATE_WEAPON_LIST:
        case AGWS_STATE_AMMO_LIST:
            from = &positions.position[MenuWork.agws.weaponSlot[MenuWork.agws.selectedIndex]];
            break;
        }
        position->x += (from->x - position->x) * 0.2f;
        position->y += (from->y - position->y) * 0.2f;
        position->z += (from->z - position->z) * 0.2f;
        rotationOf->x += (rotation.x - rotationOf->x) * 0.2f;
        rotationOf->y += (rotation.y - rotationOf->y) * 0.2f;
        rotationOf->z += (rotation.z - rotationOf->z) * 0.2f;
    }
    if (MenuWork.agws.modelBreak != 0 || MenuWork.agws.state == 0x10) {
        self->state = 30;
        MenuWork.agws.modelBreak = 0;
    }
    /* 0 opens the unit; 10 and 20 refresh a changed weapon; 30 and 31 close it. */
    switch (self->state) {
    case 0:
        MenuModelUnitOpen(self, 2);
        if (MenuEquipStealMaskCheck() == 0) {
            for (weaponIndex = 0; weaponIndex < 3; weaponIndex++) {
                MenuModelWeaponCreate(self, 0, weaponIndex);
            }
        }
        self->state = 10;
        break;
    case 10:
        if (MenuWork.agws.weaponChange != 0) {
            if (MenuWork.agws.weaponHand >= 0) {
                if (MenuWork.agws.weaponHand < 3) {
                    int hand = MenuWork.agws.weaponHand;
                    short keptWeaponId = self->org->weaponId[hand];
                    signed char keptHand = self->org->hand[hand];

                    /*
                     * Change 1 builds the model for the new weapon, then restores the stored
                     * weapon.
                     */
                    if (MenuWork.agws.weaponChange == 1) {
                        self->org->weaponId[hand] = MenuWork.agws.weaponId;
                        self->org->hand[hand] = MenuWork.agws.weaponHandKind;
                        MenuModelWeaponCreate(self, 0, MenuWork.agws.weaponHand);
                        self->org->weaponId[hand] = keptWeaponId;
                        self->org->hand[hand] = keptHand;
                    } else {
                        MenuModelWeaponCreate(self, 0, MenuWork.agws.weaponHand);
                    }
                    MenuWork.agws.weaponChange = 0;
                }
            }
        }
        self->state = 20;
        /* fallthrough */
    case 20:
        if (MenuWork.agws.weaponChange != 0) {
            self->state = 10;
        }
        break;
    case 30:
        MenuModelUnitOpen(self, 1);
        self->state = 31;
        /* fallthrough */
    case 31:
        if (transform->fade == 0.0f) {
            self->taskPhase = -1;
        }
        break;
    }
}

/*
 * Runs one frame of the equipment menu: the first frame carves the work
 * buffers, then pad input drives the state machine and the sub-screens update.
 */
void MenuAgws(void) {
    int occupiedMount[3];
    unsigned char previousWeaponHand;
    AgwsPasWork *pas;
    AgwsUnitOrg *temp_19_3;
    AgwsUnitOrg *temp_21_3;
    AgwsUnitOrg *temp_2_3;
    AgwsUnitOrg *temp_2_8;
    AgwsUnitOrg *temp_3_4;
    AgwsUnitOrg *temp_4_2;
    AgwsUnitOrg *temp_6_2;
    s16 temp_16_2;
    s16 temp_16_5;
    s16 temp_17_3;
    s16 temp_19_2;
    s16 temp_21;
    s16 temp_22;
    s16 temp_23_2;
    s16 temp_4_3;
    s32 temp_18_2;
    s32 temp_21_2;
    s32 temp_22_2;
    s32 temp_2_10;
    s32 temp_2_4;
    s32 temp_2_5;
    s32 temp_2_7;
    s32 temp_3_3;
    int temp_19;
    s8 temp_23;
    u16 temp_16_3;
    u16 temp_17_4;

    /* First frame: carve the work buffers and collect the party units. */
    if (MenuWork.agws.state == 0) {
        void *workCursor;
        unsigned char *blockBase;
        unsigned short *acceptedAgws;
        AgwsNameWorkPrefix *name;
        AgwsMenuWork *menu;
        int windowIndex;
        s32 var_16;
        s32 var_17;

        windowIndex = AGWS_LIST_WINDOW_COUNT - 1;
        workCursor = MainMenuWorkEnd;
        pas = workCursor;
        AgwsPas = pas;
        blockBase = workCursor;
        workCursor += AGWS_PAS_WORK_SIZE;
        pas->state = 0;
        AgwsInfo = workCursor;
        blockBase = workCursor;
        workCursor += AGWS_INFO_WORK_SIZE;
        *blockBase = 0;
        AgwsSelect = workCursor;
        blockBase = workCursor;
        workCursor += AGWS_SELECT_WORK_SIZE;
        *blockBase = 0;
        AgwsList = workCursor;
        blockBase = workCursor;
        workCursor += AGWS_LIST_WORK_SIZE;
        *blockBase = 0;
        do {
            AgwsWinSP[windowIndex] = &AgwsList->window[windowIndex];
            windowIndex--;
        } while (windowIndex >= 0);
        AgwsStatus = workCursor;
        blockBase = workCursor;
        workCursor += AGWS_STATUS_WORK_SIZE;
        *blockBase = 0;
        AgwsWeapon2 = workCursor;
        blockBase = workCursor;
        workCursor += sizeof(AgwsWeapon2Work);
        *blockBase = 0;
        AgwsSwitch = workCursor;
        blockBase = workCursor;
        workCursor += AGWS_SWITCH_WORK_SIZE;
        *blockBase = 0;
        AgwsFace = workCursor;
        blockBase = workCursor;
        workCursor += AGWS_FACE_WORK_SIZE;
        *blockBase = 0;
        AgwsName = workCursor;
        name = AgwsName;
        workCursor += sizeof(AgwsNameWorkPrefix);
        name->state = 0;
        MenuWorkEndCheck(workCursor);
        var_17 = 0;
        MenuWork.agws.agwsCount = 0;
        var_16 = AGWS_FIRST_UNIT_ID;
        acceptedAgws = AgwsNo;
        for (; var_16 < AGWS_UNIT_ID_END; var_16++) {
            if (PartyTakeAgwsCheck(var_16) != 0) {
                var_17 += 1;
                *acceptedAgws++ = (unsigned short) var_16;
                MenuWork.agws.agwsCount++;
            }
        }
        for (; var_17 < AGWS_UNIT_MAX; var_17++)
            AgwsNo[var_17] = 0;
        MenuWork.agws.chrNo = (s16)AgwsNo[0];
        MenuAgwsParaSet((s32) (s16) AgwsNo[0], 0);
        MenuAgwsCameraSet();
        MenuModelMemorySet(AGWS_MODEL_MEMORY_ID);
        MenuModelMenuMotionLoad();
        MenuModelOut[0] = 0;
        MenuKeepSelectReset();
        menu = &MenuWork.agws;
        menu->state = 0;
        menu->flags = 0;
        menu->pendingState = AGWS_STATE_SELECT;
        menu->transitionTicks = AGWS_WAIT_SWITCH;
    }
    if (MenuWork.agws.state != MenuWork.agws.pendingState) {
        MenuWork.agws.state = MenuWork.agws.pendingState;
        MenuWork.agws.flags |= MENU_FLAG_ENTER;
        MenuWork.agws.commandState = MenuWork.agws.pendingState;
    } else {
        MenuWork.agws.flags &= ~MENU_FLAG_ENTER;
    }
    if (MenuWork.agws.transitionTicks != 0) {
        MenuWork.agws.transitionTicks = MenuWork.agws.transitionTicks + 0xFF;
    }
    MenuAgwsCameraMove();
    switch (MenuWork.agws.state) {
    /*
     * Unit select: the d-pad moves between units, circle opens the command list, cross closes the
     * menu.
     */
    case AGWS_STATE_SELECT:
        if (MenuWork.agws.transitionTicks == 0) {
            if (PadData.mappedRepeat & (PAD_UP | PAD_RIGHT | PAD_DOWN | PAD_LEFT)) {
                switch (PadData.mappedRepeat) {
                case PAD_UP:
                    MenuWork.agws.selectedAgws -= 2;
                    break;
                case PAD_DOWN:
                    MenuWork.agws.selectedAgws += 2;
                    break;
                case PAD_LEFT:
                    MenuWork.agws.selectedAgws--;
                    break;
                case PAD_RIGHT:
                    MenuWork.agws.selectedAgws++;
                    break;
                }
                {
                    AgwsMenuWork *selectionMenu = &MenuWork.agws;
                    if (selectionMenu->selectedAgws < 0) {
                        int lastAgws = (unsigned char)selectionMenu->agwsCount - 1;
                        selectionMenu->selectedAgws = lastAgws;
                    }
                    if (selectionMenu->selectedAgws >= selectionMenu->agwsCount) {
                        selectionMenu->selectedAgws = 0;
                    }
                    selectionMenu->chrNo = AgwsNo[selectionMenu->selectedAgws];
                }
                xglSoundEffectNormalID(SE_CURSOR, 0);
            }
            if (PadData.half_2a & PAD_CIRCLE) {
                MenuWork.agws.pendingState = AGWS_STATE_COMMAND;
                MenuWork.agws.transitionTicks = AGWS_WAIT_COMMAND;
                MenuWork.agws.weaponHand = 0;
                MenuWork.agws.equipSlot = 0;
                MenuWork.agws.selectedIndex = 0;
                xglSoundEffectNormalID(SE_DECIDE, 0);
                MenuModelCreate(MenuModelOut, MenuWork.agws.chrNo);
                MenuModelExtFuncSet(MenuModelOut[0], MenuAgwsModelMain, 0);
                MenuAgwsParaSet(MenuWork.agws.chrNo, 0);
            }
            if (PadData.half_2a & PAD_CROSS) {
                MenuWork.agws.pendingState = (u8) AGWS_STATE_CLOSE;
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
        }
        break;
    /*
     * Command list: L1 and R1 switch units, circle opens the highlighted command, cross returns to
     * unit select.
     */
    case AGWS_STATE_COMMAND:
        if (MenuWork.agws.transitionTicks == 0) {
            if (PadData.half_28 & (PAD_L2 | PAD_R2))
                break;
            if (PadData.half_2a & (PAD_L1 | PAD_R1)) {
                unsigned short character;
                switch (PadData.half_2a) {
                case PAD_L1:
                    MenuWork.agws.selectedAgws--;
                    break;
                case PAD_R1:
                    MenuWork.agws.selectedAgws++;
                    break;
                }
                if (MenuWork.agws.selectedAgws < 0)
                    MenuWork.agws.selectedAgws = MenuWork.agws.selectedAgws
                                           + MenuWork.agws.agwsCount;
                else if (MenuWork.agws.selectedAgws >= MenuWork.agws.agwsCount)
                    MenuWork.agws.selectedAgws = MenuWork.agws.selectedAgws
                                           - MenuWork.agws.agwsCount;
                MenuWork.agws.modelBreak = 1;
                MenuWork.agws.weaponHand = 0;
                character = AgwsNo[MenuWork.agws.selectedAgws];
                MenuWork.agws.equipSlot = 0;
                MenuWork.agws.chrNo = character;
                MenuWork.agws.selectedIndex = 0;
                MenuAgwsParaSet((short)character, 0);
                xglSoundEffectNormalID(SE_CURSOR, 0);
                MenuKeepSelectReset();
                MenuWork.agws.transitionTicks = AGWS_WAIT_SCROLL;
            } else {
                int nextCursor = MenuSelectMove(MenuWork.agws.reserveCursor, 3, 0);
                if (MenuWork.agws.reserveCursor != nextCursor) {
                    MenuWork.agws.reserveCursor = nextCursor;
                } else if (MenuModelOut[0] != 0) {
                    if (PadData.half_2a & PAD_CIRCLE) {
                        if (MenuMainAgwsCheck(MenuWork.agws.chrNo) != 0) {
                            int reserve = MenuWork.agws.reserveCursor;
                            if (reserve != 2) {
                                if (reserve < 3) {
                                    if (reserve >= 0)
                                        MenuWork.agws.pendingState = AGWS_STATE_EQUIP;
                                }
                            } else {
                                MenuWork.agws.pendingState = AGWS_STATE_PILOT_LIST;
                            }
                            MenuWork.agws.transitionTicks = AGWS_WAIT_SWITCH;
                            if (MenuCursorKeepCheck() == 0) {
                                MenuWork.agws.weaponHand = 0;
                                MenuWork.agws.equipSlot = 0;
                            }
                            xglSoundEffectNormalID(SE_DECIDE, 0);
                        } else {
                            xglSoundEffectNormalID(SE_BUZZER, 0);
                        }
                    } else if (PadData.half_2a & PAD_CROSS) {
                        MenuWork.agws.pendingState = AGWS_STATE_SELECT;
                        MenuWork.agws.transitionTicks = AGWS_WAIT_SWITCH;
                        MenuKeepSelectReset();
                        xglSoundEffectNormalID(SE_CANCEL, 0);
                    }
                }
            }
            if (MenuModelOut[0] == 0) {
                MenuModelCreate(MenuModelOut, MenuWork.agws.chrNo);
                MenuModelExtFuncSet(MenuModelOut[0], MenuAgwsModelMain, 0);
            }
        }
        MenuWork.agws.menuSelection = 1;
        break;
    /*
     * Equipment slots: the d-pad moves between hands and slots, circle opens a list or removes the
     * part, cross returns to the command list.
     */
    case AGWS_STATE_EQUIP:
        if (MenuWork.agws.flags & 1 & 0xFF) {
            MenuWork.agws.weaponId = 0;
            MenuWork.agws.ammoId = 0;
            MenuWork.agws.accessoryId = 0;
        }
        if (MenuWork.agws.transitionTicks == 0) {
            temp_16_3 = PadData.mappedRepeat;
            if (temp_16_3 == PAD_UP) {
                temp_6_2 = func_A191C0(MenuWork.agws.chrNo);
                previousWeaponHand = MenuWork.agws.weaponHand;
                if (MenuWork.agws.weaponHand == 3) {
                    if (MenuWork.agws.equipSlot == 0) {
                        if (MenuBulletCheck(temp_6_2->accessoryId[0], -1) != 0) {
                            MenuWork.agws.equipSlot = 1;
                        } else {
                            MenuWork.agws.equipSlot = 0;
                        }
                        MenuWork.agws.weaponHand = 2;
                    } else {
                        MenuWork.agws.equipSlot--;
                    }
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                } else if (MenuWork.agws.equipSlot == 1) {
                    MenuWork.agws.equipSlot = 0;
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                } else if (MenuWork.agws.weaponHand == 0) {
                    if (PadData.mappedPressed == temp_16_3) {
                        MenuWork.agws.weaponHand = 3;
                        MenuWork.agws.equipSlot = 2;
                    }
                } else {
                    temp_3_3 = previousWeaponHand - 1;
                    MenuWork.agws.weaponHand = (s8) temp_3_3;
                    if (MenuBulletCheck(temp_6_2->weaponId[MenuWork.agws.weaponHand], -1) != 0) {
                        MenuWork.agws.equipSlot = (s8) 1;
                    }
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                }
            }
            temp_17_4 = PadData.mappedRepeat;
            if (temp_17_4 == PAD_DOWN) {
                temp_4_2 = func_A191C0(MenuWork.agws.chrNo);
                if (MenuWork.agws.weaponHand == 3) {
                    if (MenuWork.agws.equipSlot == 2) {
                        if (PadData.mappedPressed == temp_17_4) {
                            MenuWork.agws.equipSlot = 0;
                            MenuWork.agws.weaponHand = 0;
                        }
                    } else {
                        MenuWork.agws.equipSlot = MenuWork.agws.equipSlot + 1;
                        xglSoundEffectNormalID(SE_CURSOR, 0);
                    }
                } else {
                    if (MenuWork.agws.equipSlot == 0
                            && (temp_2_10 = MenuBulletCheck(temp_4_2->weaponId[MenuWork.agws.weaponHand], -1)) == 1) {
                        MenuWork.agws.equipSlot = (s8) temp_2_10;
                    } else {
                        MenuWork.agws.equipSlot = (s8) 0;
                        MenuWork.agws.weaponHand = (s8) (MenuWork.agws.weaponHand + 1);
                    }
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                }
            }
            if (PadData.half_2a == PAD_CIRCLE) {
                if (MenuWork.agws.reserveCursor == 0) {
                    if (MenuWork.agws.weaponHand != 3) {
                        if (MenuWork.agws.equipSlot == 0) {
                            AgwsMenuWork *selectionMenu = &MenuWork.agws;
                            unsigned char nextState = 0x42;
                            selectionMenu->selectedWeapon = -1;
                            selectionMenu->pendingState = nextState;
                            selectionMenu->selectedIndex = 0;
                        } else {
                            MenuWork.agws.pendingState = AGWS_STATE_AMMO_LIST;
                            MenuWork.agws.selectedAmmo = -1;
                        }
                    } else {
                        MenuWork.agws.pendingState = AGWS_STATE_ACCESSORY_LIST;
                        MenuWork.agws.selectedAccessory = -1;
                    }
                    MenuWork.agws.transitionTicks = AGWS_WAIT_SWITCH;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    temp_3_4 = func_A191C0(MenuWork.agws.chrNo);
                    if (MenuWork.agws.weaponHand != 3) {
                        if (MenuWork.agws.equipSlot != 1) {
                            temp_18_2 = MenuRWeaponCheck2(temp_3_4->weaponId[MenuWork.agws.weaponHand]);
                            temp_3_4->weaponId[MenuWork.agws.weaponHand] = 0;
                            temp_3_4->hand[MenuWork.agws.weaponHand] = 0;
                            if (temp_18_2 != 0) {
                                dataWpnBoxInc(temp_18_2);
                            }
                            MenuWork.agws.weaponChange = 1;
                            temp_16_5 = temp_3_4->ammoId[MenuWork.agws.weaponHand];
                            temp_3_4->ammoId[MenuWork.agws.weaponHand] = 0;
                            if ((MenuBulletCheck(temp_18_2, -1) != 0) && (temp_16_5 != 0)) {
                                if ((func_A1A3D8(temp_18_2)->defaultAmmoId != temp_16_5) || (dataBltBoxChk(temp_16_5) == 0)) {
                                    dataBltBoxInc(temp_16_5);
                                }
                            }
                        }
                    } else {
                        int accessorySlot = MenuWork.agws.equipSlot;
                        temp_4_3 = temp_3_4->accessoryId[accessorySlot];
                        temp_3_4->accessoryId[accessorySlot] = 0;
                        if (temp_4_3 != 0) {
                            dataAccBoxInc(temp_4_3);
                        }
                    }
                    MenuAgwsParaSet(MenuWork.agws.chrNo, 0);
                    xglSoundEffectNormalID(AGWS_SE_REMOVE, 0);
                }
            }
            if (PadData.half_2a == PAD_CROSS) {
                if (MenuCursorKeepCheck() == 0) {
                    MenuWork.agws.reserveCursor = 0;
                }
                    MenuWork.agws.pendingState = AGWS_STATE_COMMAND;
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
        }
        break;
    /*
     * Mount position: circle opens the weapon list for the chosen position, cross returns to
     * equipment.
     */
    case AGWS_STATE_WEAPON_MOUNT:
        if (MenuWork.agws.flags & 1 & 0xFF) {
            AgwsUnitOrg *org = func_A191C0(MenuWork.agws.chrNo);
            int hand;
            int position;

            MenuWork.agws.nameRow = 0;
            for (hand = 0; hand < 3; hand++) {
                if (hand == MenuWork.agws.weaponHand)
                    occupiedMount[hand] = 0;
                else
                    occupiedMount[hand] = OptMntToWpnPos(org->hand[hand]);
            }
            /* Collect the mount positions that no other hand uses. */
            for (position = 1; position < 6; position++) {
                for (hand = 0; hand < 3; hand++) {
                    if (occupiedMount[hand] == position)
                        break;
                }
                if (hand == 3) {
                    MenuWork.agws.weaponSlot[MenuWork.agws.nameRow] = position;
                    MenuWork.agws.nameRow++;
                    MenuWork.agws.weaponSlot[MenuWork.agws.nameRow] = 0;
                }
            }
        }
        if (MenuWork.agws.transitionTicks == 0) {
            MenuWork.agws.selectedIndex = (s8) MenuSelectMove(MenuWork.agws.selectedIndex, MenuWork.agws.nameRow, 0);
            if (PadData.half_2a & PAD_CROSS) {
                MenuWork.agws.pendingState = (u8) AGWS_STATE_EQUIP;
                MenuWork.agws.transitionTicks = (u8) AGWS_WAIT_SWITCH;
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
            if (PadData.half_2a & PAD_CIRCLE) {
                MenuWork.agws.pendingState = AGWS_STATE_WEAPON_LIST;
                MenuWork.agws.transitionTicks = AGWS_WAIT_SWITCH;
                xglSoundEffectNormalID(SE_DECIDE, 0);
            }
        }
        break;
    /*
     * Weapon list: the cursor previews each weapon on the selected hand, circle equips it, cross
     * returns to the mount position.
     */
    case AGWS_STATE_WEAPON_LIST:
        if (MenuWork.agws.flags & 1 & 0xFF) {
            MenuAgwsListMake_Wpn();
            MenuWork.agws.selectedWeapon = -1;
            MenuWork.agws.weaponId = 0;
        } else if (MenuWork.agws.transitionTicks == 0) {
                temp_19 = WindowSPSelect(AgwsWinSP[0], PadData.mappedRepeat);
                WindowSPKeepSelect(AgwsWinSP[0], &MenuKeepSelect[MenuWork.agws.weaponSlot[MenuWork.agws.selectedIndex] * 5 - 5]);
                if (MenuWork.agws.selectedWeapon != temp_19) {
                    temp_2_3 = func_A191C0(MenuWork.agws.chrNo);
                    MenuWork.agws.selectedWeapon = (s8) temp_19;
                    temp_2_4 = MenuSortGet(0, MenuWork.agws.selectedWeapon);
                    MenuWork.agws.weaponId = (s16) temp_2_4;
                    temp_2_5 = MenuWeaponEquipPosCheck(MenuWork.agws.chrNo, (s16) temp_2_4, (s32) MenuWork.agws.weaponSlot[MenuWork.agws.selectedIndex], MenuWork.agws.weaponHand);
                    MenuWork.agws.weaponHandKind = (short) temp_2_5;
                    if (MenuWork.agws.weaponHandKind != 0) {
                        temp_23 = temp_2_3->hand[MenuWork.agws.weaponHand];
                        temp_22 = temp_2_3->weaponId[MenuWork.agws.weaponHand];
                        temp_21 = temp_2_3->ammoId[MenuWork.agws.weaponHand];
                        temp_2_3->weaponId[MenuWork.agws.weaponHand] = MenuWork.agws.weaponId;
                        temp_2_3->hand[MenuWork.agws.weaponHand] = MenuWork.agws.weaponHandKind;
                        if (temp_2_3->weaponId[MenuWork.agws.weaponHand] != 0) {
                            temp_2_3->ammoId[MenuWork.agws.weaponHand] = func_A1A3D8(MenuWork.agws.weaponId)->defaultAmmoId;
                        } else {
                            temp_2_3->ammoId[MenuWork.agws.weaponHand] = 0;
                        }
                        MenuAgwsParaSet(MenuWork.agws.chrNo, 1);
                        temp_2_3->ammoId[MenuWork.agws.weaponHand] = temp_21;
                        temp_2_3->weaponId[MenuWork.agws.weaponHand] = temp_22;
                        temp_2_3->hand[MenuWork.agws.weaponHand] = temp_23;
                        MenuWork.agws.weaponChange = 1;
                    } else {
                        MenuAgwsParaSet(MenuWork.agws.chrNo, 0);
                        MenuWork.agws.weaponChange = 2;
                    }
                    MenuWork.agws.selectedWeapon = (s8) temp_19;
                }
                if ((PadData.half_2a == PAD_CIRCLE) && (MenuWork.agws.selectedWeapon >= 0)) {
                    temp_2_7 = MenuWeaponEquipPosCheck(MenuWork.agws.chrNo, MenuWork.agws.weaponId, (s32) MenuWork.agws.weaponSlot[MenuWork.agws.selectedIndex], MenuWork.agws.weaponHand);
                    if (temp_2_7 != 0) {
                        temp_2_8 = func_A191C0(MenuWork.agws.chrNo);
                        temp_21_2 = MenuRWeaponCheck2(temp_2_8->weaponId[MenuWork.agws.weaponHand]);
                        temp_19_2 = temp_2_8->ammoId[MenuWork.agws.weaponHand];
                        temp_2_8->weaponId[MenuWork.agws.weaponHand] = MenuRWeaponCheck(MenuWork.agws.weaponId, temp_2_7);
                        temp_2_8->hand[MenuWork.agws.weaponHand] = temp_2_7;
                        temp_2_8->ammoId[MenuWork.agws.weaponHand] = func_A1A3D8(MenuWork.agws.weaponId)->defaultAmmoId;
                        if (temp_21_2 != 0) {
                            dataWpnBoxInc(temp_21_2);
                        }
                        dataWpnBoxDec(MenuWork.agws.weaponId);
                        if (temp_19_2 != 0) {
                            if (MenuBulletCheck(temp_21_2, -1) != 0) {
                                if (func_A1A3D8(temp_21_2)->defaultAmmoId == temp_19_2) {
                                    if (dataBltBoxChk(temp_19_2) == 0) {
                                        dataBltBoxInc(temp_19_2);
                                    }
                                } else {
                                    dataBltBoxInc(temp_19_2);
                                }
                            }
                        }
                        MenuAgwsParaSet(MenuWork.agws.chrNo, 0);
                        MenuWork.agws.pendingState = AGWS_STATE_EQUIP;
                        MenuWork.agws.transitionTicks = AGWS_WAIT_SWITCH;
                        xglSoundEffectNormalID(AGWS_SE_EQUIP, 0);
                    } else {
                        xglSoundEffectNormalID(SE_BUZZER, 0);
                    }
                }
                if (PadData.half_2a == PAD_CROSS) {
                    MenuAgwsParaSet(MenuWork.agws.chrNo, 0);
                    MenuWork.agws.pendingState = AGWS_STATE_WEAPON_MOUNT;
                    MenuWork.agws.weaponChange = 2;
                    MenuWork.agws.transitionTicks = AGWS_WAIT_SWITCH;
                    xglSoundEffectNormalID(SE_CANCEL, 0);
                    if (MenuCursorKeepCheck() == 0) {
                        MenuWork.agws.selectedIndex = 0;
                    }
                    {
                        int savedSelection;
                        for (savedSelection = 0; savedSelection < 8; savedSelection++)
                            WindowSPKeepSelectCheck(&MenuKeepSelect[savedSelection * 5]);
                    }
                }
        }
        break;
    /*
     * Ammo list: the cursor previews each ammo type, circle equips it, cross returns to equipment.
     */
    case AGWS_STATE_AMMO_LIST:
        if (MenuWork.agws.flags & 1 & 0xFF) {
            AgwsUnitOrg *org = func_A191C0(MenuWork.agws.chrNo);
            MenuWork.agws.weaponId = org->weaponId[MenuWork.agws.weaponHand];
            MenuAgwsListMake_Gun();
            MenuWork.agws.selectedAmmo = -1;
        } else if (MenuWork.agws.transitionTicks == 0) {
                int selection = WindowSPSelect(AgwsWinSP[0], PadData.mappedRepeat);
                WindowSPKeepSelect(AgwsWinSP[0], &MenuKeepSelect[0x1E]);
                if (MenuWork.agws.selectedAmmo != selection) {
                    AgwsUnitOrg *org = func_A191C0(MenuWork.agws.chrNo);
                    MenuWork.agws.selectedAmmo = selection;
                    MenuWork.agws.ammoId = MenuSortGet(0, MenuWork.agws.selectedAmmo);
                    if (MenuBulletCheck(MenuWork.agws.weaponId, MenuWork.agws.ammoId) != 0) {
                        short previousAmmo = org->ammoId[MenuWork.agws.weaponHand];
                        org->ammoId[MenuWork.agws.weaponHand] = MenuWork.agws.ammoId;
                        MenuAgwsParaSet(MenuWork.agws.chrNo, 1);
                        org->ammoId[MenuWork.agws.weaponHand] = previousAmmo;
                    } else {
                        MenuAgwsParaSet(MenuWork.agws.chrNo, 0);
                    }
                }
                if (PadData.half_2a & PAD_CIRCLE) {
                    if (MenuBulletCheck(MenuWork.agws.weaponId, MenuWork.agws.ammoId) != 0) {
                        AgwsUnitOrg *org = func_A191C0(MenuWork.agws.chrNo);
                        short previousAmmo = org->ammoId[MenuWork.agws.weaponHand];
                        short weapon;
                        org->ammoId[MenuWork.agws.weaponHand] = MenuWork.agws.ammoId;
                        MenuAgwsParaSet(MenuWork.agws.chrNo, 0);
                        if (previousAmmo != 0) {
                            weapon = org->weaponId[MenuWork.agws.weaponHand];
                            if (weapon != 0) {
                                if (func_A1A3D8(weapon)->defaultAmmoId != previousAmmo
                                        || dataBltBoxChk(previousAmmo) == 0)
                                    dataBltBoxInc(previousAmmo);
                            } else {
                                dataBltBoxInc(previousAmmo);
                            }
                        }
                        if (MenuWork.agws.ammoId != 0) {
                            weapon = org->weaponId[MenuWork.agws.weaponHand];
                            if (weapon != 0
                                    && func_A1A3D8(weapon)->defaultAmmoId != MenuWork.agws.ammoId)
                                dataBltBoxDec(MenuWork.agws.ammoId);
                        }
                        MenuWork.agws.pendingState = AGWS_STATE_EQUIP;
                        MenuWork.agws.transitionTicks = AGWS_WAIT_SWITCH;
                        xglSoundEffectNormalID(AGWS_SE_EQUIP, 0);
                    } else {
                        xglSoundEffectNormalID(SE_BUZZER, 0);
                    }
                }
                if (PadData.half_2a & PAD_CROSS) {
                    MenuAgwsParaSet(MenuWork.agws.chrNo, 0);
                    MenuWork.agws.pendingState = AGWS_STATE_EQUIP;
                    MenuWork.agws.transitionTicks = AGWS_WAIT_SWITCH;
                    xglSoundEffectNormalID(SE_CANCEL, 0);
                    WindowSPKeepSelectCheck(&MenuKeepSelect[0x1E]);
                }
        }
        break;
    /*
     * Accessory list: the cursor previews each accessory, circle equips it, cross returns to
     * equipment.
     */
    case AGWS_STATE_ACCESSORY_LIST:
        if (MenuWork.agws.flags & 1 & 0xFF) {
            MenuAgwsListMake_Acc();
            MenuWork.agws.selectedAccessory = -1;
        } else if (MenuWork.agws.transitionTicks == 0) {
                signed char selection = WindowSPSelect(AgwsWinSP[0], PadData.mappedRepeat);
                unsigned short buttons;
                WindowSPKeepSelect(AgwsWinSP[0], &MenuKeepSelect[0x23]);
                if (MenuWork.agws.selectedAccessory != selection) {
                    MenuWork.agws.selectedAccessory = selection;
                    MenuWork.agws.accessoryId = MenuSortGet(0, selection);
                    if (MenuAccessoryEquipCheck(MenuWork.agws.chrNo,
                                               MenuWork.agws.accessoryId,
                                               MenuWork.agws.equipSlot) != 0) {
                        AgwsUnitOrg *org = func_A191C0(MenuWork.agws.chrNo);
                        short previousAccessory = org->accessoryId[MenuWork.agws.equipSlot];
                        org->accessoryId[MenuWork.agws.equipSlot] = MenuWork.agws.accessoryId;
                        MenuAgwsParaSet(MenuWork.agws.chrNo, 1);
                        org->accessoryId[MenuWork.agws.equipSlot] = previousAccessory;
                    } else {
                        MenuAgwsParaSet(MenuWork.agws.chrNo, 0);
                    }
                }
                if (MenuWork.agws.selectedAccessory >= 0 && PadData.half_2a == PAD_CIRCLE) {
                    if (MenuAccessoryEquipCheck(MenuWork.agws.chrNo,
                                               MenuWork.agws.accessoryId,
                                               MenuWork.agws.equipSlot) != 0) {
                        AgwsUnitOrg *org = func_A191C0(MenuWork.agws.chrNo);
                        short previousAccessory = org->accessoryId[MenuWork.agws.equipSlot];
                        org->accessoryId[MenuWork.agws.equipSlot] = MenuWork.agws.accessoryId;
                        MenuAgwsParaSet(MenuWork.agws.chrNo, 0);
                        if (previousAccessory != 0)
                            dataAccBoxInc(previousAccessory);
                        dataAccBoxDec(MenuWork.agws.accessoryId);
                        MenuWork.agws.pendingState = AGWS_STATE_EQUIP;
                        xglSoundEffectNormalID(AGWS_SE_EQUIP, 0);
                    } else {
                        xglSoundEffectNormalID(SE_BUZZER, 0);
                    }
                }
                buttons = PadData.half_2a;
                if (buttons == PAD_CROSS) {
                    MenuAgwsParaSet(MenuWork.agws.chrNo, 0);
                    MenuWork.agws.pendingState = buttons;
                    xglSoundEffectNormalID(SE_CANCEL, 0);
                    WindowSPKeepSelectCheck(&MenuKeepSelect[0x23]);
                }
        }
        break;
    /*
     * Pilot list: circle assigns or removes the pilot for this unit, cross returns to the command
     * list.
     */
    case AGWS_STATE_PILOT_LIST:
        if (MenuWork.agws.flags & 1 & 0xFF) {
            MenuAgwsListMake_Pilot();
        } else {
            if (MenuWork.agws.transitionTicks == 0) {
                MenuWork.agws.listSelect = (s8) WindowSPSelect(AgwsWinSP[0], PadData.mappedRepeat);
                WindowSPKeepSelect(AgwsWinSP[0], MenuKeepSelect + 0x28);
                temp_22_2 = MenuAgwsPilotCheck(MenuWork.agws.chrNo);
                temp_23_2 = MenuSortGet(0, MenuWork.agws.listSelect);
                temp_21_3 = func_A191C0(temp_22_2);
                temp_19_3 = func_A191C0(temp_23_2);
                temp_17_3 = temp_21_3->agwsId;
                temp_16_2 = temp_19_3->agwsId;
                temp_21_3->agwsId = 0;
                temp_19_3->agwsId = MenuWork.agws.chrNo;
                MenuAgwsParaSet(MenuWork.agws.chrNo, 1);
                temp_19_3->agwsId = temp_16_2;
                temp_21_3->agwsId = temp_17_3;
                if (PadData.half_2a & PAD_CIRCLE) {
                    if (temp_22_2 == 0) {
                        if (temp_19_3->agwsId == 0) {
                            temp_19_3->agwsId = MenuWork.agws.chrNo;
                            xglSoundEffectNormalID(AGWS_SE_PILOT_SET, 0);
                            MenuAgwsParaSet(MenuWork.agws.chrNo, 0);
                        } else {
                            xglSoundEffectNormalID(SE_BUZZER, 0);
                        }
                    } else if (temp_22_2 == temp_23_2) {
                        temp_19_3->agwsId = 0;
                        xglSoundEffectNormalID(AGWS_SE_PILOT_REMOVE, 0);
                        MenuAgwsParaSet(MenuWork.agws.chrNo, 0);
                    } else {
                        if (temp_19_3->agwsId == 0) {
                            temp_19_3->agwsId = MenuWork.agws.chrNo;
                            temp_21_3->agwsId = 0;
                            xglSoundEffectNormalID(AGWS_SE_PILOT_SET, 0);
                            MenuAgwsParaSet(MenuWork.agws.chrNo, 0);
                        } else {
                            xglSoundEffectNormalID(SE_BUZZER, 0);
                        }
                    }
                    MenuAgwsListMake_Pilot();
                }
                if (PadData.half_2a & PAD_CROSS) {
                    MenuAgwsParaSet(MenuWork.agws.chrNo, 0);
                    MenuWork.agws.pendingState = AGWS_STATE_COMMAND;
                    MenuWork.agws.transitionTicks = AGWS_WAIT_SWITCH;
                    xglSoundEffectNormalID(SE_CANCEL, 0);
                    if (MenuCursorKeepCheck() == 0) {
                        MenuWork.agws.reserveCursor = 0;
                    }
                    WindowSPKeepSelectCheck(MenuKeepSelect + 0x28);
                }
            } else {
                MenuWork.agws.listSelect = (s8) WindowSPSelect(AgwsWinSP[0], 0U);
                WindowSPKeepSelect(AgwsWinSP[0], MenuKeepSelect + 0x28);
            }
        }
        break;
    /* Close: start the closing transition, wait for it, then leave the menu. */
    case AGWS_STATE_CLOSE:
        MenuWork.agws.state = (u8) AGWS_STATE_CLOSE_WAIT;
        MenuWork.agws.pendingState = (u8) AGWS_STATE_CLOSE_WAIT;
        MenuWork.agws.transitionTicks = (u8) AGWS_WAIT_LEAVE;
        /* fallthrough */
    case AGWS_STATE_CLOSE_WAIT:
        if (MenuWork.agws.transitionTicks != 0)
            break;
        MenuWork.agws.state = (u8) AGWS_STATE_EXIT;
        /* fallthrough */
    case AGWS_STATE_EXIT:
        ChangeTopLevel(0);
        MenuModelAllBreak();
        break;
    }
    /* Update the sub-screens for this frame. */
    AgwsPasMain();
    AgwsInfoMain();
    AgwsSelectMain();
    AgwsListMain();
    AgwsStatusMain();
    AgwsWeapon2Main();
    AgwsSwitchMain();
    AgwsFaceMain();
    AgwsNameMain();
    /* Debug text overlay with the current menu values. */
    xglFontDebugPrintf(0, 0x38, agwsTitleText);
    xglFontDebugHex(0, 0x48, MenuWork.agws.state, 2);
    xglFontDebugHex(0, 0x60, MenuWork.agws.menuSelection, 2);
    xglFontDebugHex(0, 0x68, MenuWork.agws.selectedAgws, 2);
    xglFontDebugHex(0, 0x70, MenuWork.agws.listKind, 1);
    xglFontDebugHex(0, 0x78, MenuWork.agws.reserveCursor, 2);
    xglFontDebugHex(0, 0x80, MenuWork.agws.weaponHand, 2);
    xglFontDebugHex(0, 0x88, MenuWork.agws.equipSlot, 2);
    xglFontDebugHex(0, 0x90, MenuWork.agws.agwsCount, 1);
    xglFontDebugPrintf(0x40, 0x50, paddedNumberFormat, MenuWork.agws.selectedWeapon);
    xglFontDebugPrintf(0x40, 0x58, paddedNumberFormat, MenuWork.agws.selectedAmmo);
    xglFontDebugPrintf(0x40, 0x60, paddedNumberFormat, MenuWork.agws.selectedAccessory);
    xglFontDebugPrintf(0x40, 0x68, paddedNumberFormat, MenuWork.agws.listSelect);
    xglFontDebugPrintf(0x40, 0x78, equipPosFormat, MenuWork.agws.pilotId);
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const char menuAgwsParaSetPeriodText[2] = ".";

const char equipPosFormat[16] = "equip_pos %4d";

/* Text: \x01Clear Time */
const char SeisanClearTimeText[16] = "\001Clear Time";

const char SeisanNoItemsText[16] = "No items.";

const char SeisanItemAcquiredText[16] = "Item acquired!";

const char SeisanLevelUpText[16] = "Level up";

const char SeisanNewEtherText[16] = "New Ether!";

const char SeisanNewAttackText[16] = "New attack!";

const char removeVerbText[8] = "Remove ";

const char assignVerbText[8] = "Assign ";

const char commaText[8] = ",";

const char infoClearText[8] = "";

char leftAltText[8] = "L. Alt\n";

char rightAltText[8] = "R. Alt\n";

char leftArmText[8] = "L. Arm\n";

char rightArmText[8] = "R. Arm\n";

char menuAgwsParaSetYesNoText[8] = "Yes\nNo";

char okayQuestionText[8] = "Okay\?";

char menuWindowTitleText[8] = "Menu";

char selectWindowTitleText[8] = "Select";

const char emptyMessageText[8] = "";

const AgwsWeapon2MountSide weapon2MountSideTable[2] = { { .icon = { 799, 800 } }, { .icon = { 0, 0 } } };

/* Text: {reset}AGWS */
char agwsTitleText[8] = "\013AGWS";

char paddedNumberFormat[4] = "%4d";

AgwsInfoWork * AgwsInfo = 0;

AgwsSelectWork * AgwsSelect = 0;

AgwsWeapon2Work * AgwsWeapon2 = 0;

AgwsNameWorkPrefix * AgwsName = 0;

const char SeisanDigit9Text[8] = "9";

const char SeisanDigit8Text[8] = "8";

const char SeisanDigit7Text[8] = "7";

const char SeisanDigit6Text[8] = "6";

const char SeisanDigit5Text[8] = "5";

const char SeisanDigit4Text[8] = "4";

const char SeisanDigit3Text[8] = "3";

const char SeisanDigit2Text[8] = "2";

const char SeisanDigit1Text[8] = "1";

const char SeisanDigit0Text[8] = "0";

/* Text: \x01Money */
const char SeisanMoneyText[8] = "\001Money";

const char SeisanPartyText[8] = "Party";

const char SeisanEndHintText[8] = " : End";

const char SeisanNextHintText[8] = " : Next";

AgwsWeapon2MountRow weapon2MountRowTable[1] = { { { 0x031c, 0x031e, 0x031d } } };
