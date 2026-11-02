#include "common.h"
#include "shared.h"
#include "main/e_battle_win_open.h"
#include "main/party.h"
#include "main/xgl_hdd.h"
#include "main/window_tex_load.h"
#include "main/menu_top_command_check.h"
#include "main/xgl_thread.h"
#include "main/menu_system.h"
#include "main/end_print.h"
#include "main/menu_model.h"
#include "main/open_close_main.h"
#include "main/ssd_set.h"
#include "main/xgl_font.h"
#include "main/xgl_sound.h"

/* Rows of the system-options list; also the index into MenuWork.options. */
enum {
    MENU_SYSTEM_OPTION_SOUND_OUTPUT,
    MENU_SYSTEM_OPTION_CURSOR_MEMORY,
    MENU_SYSTEM_OPTION_VIBRATION,
    MENU_SYSTEM_OPTION_RADAR,
    MENU_SYSTEM_OPTION_MONITOR,
    MENU_SYSTEM_OPTION_HDD,
    MENU_SYSTEM_OPTION_DEFAULT,
    MENU_SYSTEM_OPTION_COUNT
};

/* MenuWork.state of the system-options screen: the list and one sub-state per
 * option (optionStateTable maps a row to its sub-state). */
enum {
    MENU_SYSTEM_STATE_LIST = 0x10,
    MENU_SYSTEM_STATE_SOUND_OUTPUT = 0x20,
    MENU_SYSTEM_STATE_RADAR = 0x30,
    MENU_SYSTEM_STATE_VIBRATION = 0x40,
    MENU_SYSTEM_STATE_CURSOR_MEMORY = 0x50,
    MENU_SYSTEM_STATE_DEFAULT = 0x60,
    MENU_SYSTEM_STATE_HDD = 0x70,
    MENU_SYSTEM_STATE_CLOSE = 0xf0,
    MENU_SYSTEM_STATE_DONE = 0xff
};

/* MenuWork.menuState steps of MenuSystem. */
enum {
    MENU_SYSTEM_TASK_INIT = 0,
    MENU_SYSTEM_TASK_RUN = 2,
    MENU_SYSTEM_TASK_EXIT = 4
};

/* MenuWork.inputDelay frames. */
enum {
    MENU_SYSTEM_DELAY_CHOICE = 8,
    MENU_SYSTEM_DELAY_OPEN = 12,
    MENU_SYSTEM_DELAY_CLOSE = 16
};

/* State of the panel, info and menu blocks drawn by the options screen. */
enum {
    MENU_SYSTEM_BLOCK_INIT = 0,
    MENU_SYSTEM_BLOCK_RUN = 2
};

#define MENU_SYSTEM_WINDOW_STYLE 0xfffff0
#define MENU_SYSTEM_CHOICE_COUNT 2 /* entries of every choice window */
#define MENU_SYSTEM_CHOICE_ROWS 3  /* text rows of the choice window */
#define MENU_SYSTEM_ROW_HEIGHT 24
#define MENU_SYSTEM_DISABLED_GRAY 64 /* dimmed HDD row without a mounted HDD */
#define MENU_SYSTEM_VIBRATION_TEST_STRENGTH 128
#define MENU_SYSTEM_VIBRATION_TEST_DURATION 32

/* Existing option names designate entries in the contiguous option array. */
#define soundOutput options[MENU_SYSTEM_OPTION_SOUND_OUTPUT]

#define padMode options[MENU_SYSTEM_OPTION_CURSOR_MEMORY]
#define radarMode options[MENU_SYSTEM_OPTION_VIBRATION]
#define displayMode options[MENU_SYSTEM_OPTION_RADAR]
#define hddActive options[MENU_SYSTEM_OPTION_HDD]

/* Offsets named here are touched by MenuSystemInfoMain, MenuSystemInitSet,
 * and the existing MenuSystem2 completion poll. */
typedef struct MenuSystemWork {
    unsigned char unmodeled_00[3];
    unsigned char menuState;
    unsigned char unmodeled_04[0x10 - 4];
    unsigned char flags;
    unsigned char state;
    unsigned char nextState;
    unsigned char inputDelay;
    unsigned char unmodeled_14[0x20 - 0x14];
    signed char messageIndex;
    signed char choiceIndex;
    signed long long hddMounted : 8;
    unsigned char unmodeled_23[0x0d];
    unsigned char options[MENU_SYSTEM_OPTION_COUNT];
} MenuSystemWork;

typedef struct MenuSystemPad {
    unsigned char unmodeled_00[0x2a];
    unsigned short pressed;
    unsigned char unmodeled_2c[0x56 - 0x2c];
    unsigned char vibrationReset;
    unsigned char unmodeled_57[0xbe - 0x57];
    unsigned char vibrationStatus;
} MenuSystemPad;


typedef struct MenuSystemInfoBlock {
    unsigned char state;
    unsigned char active;
    unsigned char unmodeled_02[2];
    int color;
    WindowDX window;
    unsigned char unmodeled_19c[0x1a0 - 0x19c];
    short cursorX;
    short cursorY;
    unsigned char unmodeled_1a4[4];
    int visible;
    char *message;
    unsigned char unmodeled_1b0[0x350 - 0x1b0];
    char text[128];
} MenuSystemInfoBlock;

typedef struct MenuSystemMessageTable {
    char *messages[MENU_SYSTEM_OPTION_COUNT];
} MenuSystemMessageTable;

typedef union MenuSystemMessageStorage {
    MenuSystemMessageTable table;
    unsigned int words[8];
} MenuSystemMessageStorage;


typedef struct MenuSystemPanelBox {
    short x;
    short y;
    int color;
    short width;
    short height;
} MenuSystemPanelBox;

typedef struct MenuSystemPasBlock {
    unsigned char state;
    unsigned char unmodeled_01[3];
    int color;
    WindowDX window;
    EMessageParam message[1];
    unsigned char unmodeled_1e0[0x334 - 0x1e0];
    MenuSystemPanelBox box;
} MenuSystemPasBlock;

typedef struct MenuSystemWindow {
    short x;
    short y;
    int previousColor;
    short width;
    short height;
    const char *title;
    unsigned char transition;
    unsigned char unmodeled_11[3];
    void (*window)(struct MenuSystemWindow *window, void *work);
    void *subwindow;
    unsigned char unmodeled_1c[0x194 - 0x1c];
} MenuSystemWindow;

typedef struct MenuSystemChoice {
    unsigned char unmodeled_00[2];
    short enabled;
    int cursor;
    const char *choices;
    const char *question;
} MenuSystemChoice;

typedef struct MenuSystemMenuBlock {
    unsigned char state;
    unsigned char unmodeled_01[3];
    int color;
    WindowDX titleWindow;
    EMessageParam label[8];
    EMessageParam value[8];
    ECursolMainData labelCursor;
    WindowDX choiceWindow;
    EMessageParam choiceLabel[MENU_SYSTEM_CHOICE_ROWS];
    ECursolMainData choiceCursor;
    WindowDX confirmWindow;
    MenuSystemChoice confirm;
} MenuSystemMenuBlock;

typedef const char *MenuSystemOptionNames[2];

extern unsigned char *MainMenuWorkEnd;

/* The original stores the HDD mount result through this four-byte object. */
extern int MenuHddCheck;

extern MenuSystemWork MenuWork;
extern MenuSystemPad PadData;
extern MenuSystemInfoBlock *MenuSystemInfo;
extern MenuSystemPasBlock *MenuSystemPas;
extern const char MenuSystemOptionHelpText[];
extern MenuSystemMenuBlock *MenuSystemMenu;
extern const char configurationText[];
extern const char selectionText[];
static const char * msg01_2_0036DD20[] ;
static const char * msg02_3_0036DD28[] ;
static const char * msg03_4_0036DD30[] ;
extern MenuSystemOptionNames *const settingLabelNames[];
extern unsigned char *MenuModelWorkTop;
extern unsigned char optionStateTable[];
extern int outputModeTable[];
extern int displayModeTable[];
extern int vibrationModeTable[];
extern int padModeTable[];
extern int hddModeTable[];
extern char systemText[];
extern char messageDelayFormat[];
extern const char MenuSystemDefaultText[8];
extern const char MenuSystemHddText[8];
extern const char MenuSystemStereoText[8];
extern const char MenuSystemMonoText[8];
extern const char MenuSystemOffText[8];
extern const char MenuSystemOnText[8];
extern const char MenuSystemYesNoText[8];
extern void subMenuSystemInit(int workEnd);
extern void subMenuSystemMain(void);
void tyaDisplaySetting(int mode);
extern void *memset(void *destination, int value, unsigned int count);
extern void MenuBibrationSet(unsigned char pad, unsigned char mode,
                             unsigned char strength, unsigned char duration);

extern void MenuSystemMenuMain(void);
const MenuSystemMessageStorage messageTexts = {
    .words = {0x004C9338, 0x004C9380, 0x004C93A8, 0x004C93D0,
              0x004C93F8, 0x004C9428, 0x004C9460, 0}
};

const char monitorSettingsText[24] = "Monitor Settings";

const char radarDisplayText[16] = "Radar Display";

const char vibrationText[16] = "Vibration";

const char cursorLocationMemoryText[24] = "Cursor Location Memory";

const char soundOutputText[16] = "Sound Output";

const char proceedQuestionText[16] = " Proceed\?";

const char configurationText[16] = "Configuration";

const char selectionText[16] = "Selection";

static const char *msg00_0_0036DCF8[] = {MenuSystemOptionHelpText};

static const char * msg00_1_0036DD00[MENU_SYSTEM_OPTION_COUNT];

static const char * msg00_1_0036DD00[MENU_SYSTEM_OPTION_COUNT] = { soundOutputText, cursorLocationMemoryText, vibrationText, radarDisplayText, monitorSettingsText, MenuSystemHddText, MenuSystemDefaultText };

static const char * msg01_2_0036DD20[] = { MenuSystemStereoText, MenuSystemMonoText };

static const char * msg02_3_0036DD28[] = { MenuSystemOnText, MenuSystemOffText };

static const char * msg03_4_0036DD30[] = { proceedQuestionText, MenuSystemYesNoText };

/*
 * Drives the system-options screen for one frame and reports whether it is
 * still running: MenuWork.state is 0xFF once the screen driven by
 * subMenuSystemMain has finished.
 */
void MenuSystemPasMain(void)
{
    MenuSystemPasBlock *panel = MenuSystemPas;
    short messageTarget[8];
    short windowTarget[8];
    int row;

    if (panel->state != MENU_SYSTEM_BLOCK_INIT) {
        if (panel->state != MENU_SYSTEM_BLOCK_RUN)
            return;
    } else {
        panel->color = MENU_SYSTEM_WINDOW_STYLE;
        WindowDXSet(&panel->window);
        panel->window.x = -288;
        panel->window.style = panel->color;
        panel->window.y = 8;
        panel->window.width = 272;
        panel->window.height = 30;
        panel->window.state = WINDOWDX_STATE_OPEN;
        WindowDXMain(&panel->window);
        panel->window.state = WINDOWDX_STATE_READY;
        for (row = 0; row < 1; row++) {
            eMessageSet(&panel->message[row], msg00_0_0036DCF8[row]);
            panel->message[row].mode = E_MODE_SHOW_START;
            panel->message[row].x = 288;
            panel->message[row].y = 11;
            panel->message[row].draw_parameter = panel->color + 2;
        }
        panel->state = MENU_SYSTEM_BLOCK_RUN;
    }
    windowTarget[0] = -16;
    messageTarget[0] = 288;
    switch (MenuWork.state) {
    case MENU_SYSTEM_STATE_LIST:
    case MENU_SYSTEM_STATE_SOUND_OUTPUT:
    case MENU_SYSTEM_STATE_RADAR:
    case MENU_SYSTEM_STATE_VIBRATION:
    case MENU_SYSTEM_STATE_CURSOR_MEMORY:
    case MENU_SYSTEM_STATE_DEFAULT:
    case MENU_SYSTEM_STATE_HDD:
        messageTarget[0] = 16;
        break;
    default:
        windowTarget[0] = -288;
        break;
    }
    MoveSlide(&panel->window.x, windowTarget, 3.0f);
    WindowDXMain(&panel->window);
    if (panel->window.state == WINDOWDX_STATE_READY) {
        panel->box.x = panel->window.x + 3;
        panel->box.y = panel->window.y + 3;
        panel->box.width = panel->window.width - 6;
        panel->box.height = panel->window.height - 6;
        panel->box.color = panel->color;
        endPrintExtFunc(panel->color, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_SCISSOR_SET,&panel->box);
        MoveSlide(&panel->message[0].x, messageTarget, 5.0f);
        if (panel->message[0].x < 256)
            eMessageMain(&panel->message[0]);
        endPrintExtFunc(panel->color, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_SCISSOR_RESET, 0);
    }
}

void MenuSystemInfoMain(void)
{
    MenuSystemInfoBlock *info = MenuSystemInfo;
    MenuSystemMessageTable messageTable = messageTexts.table;
    short targetY;

    if (info->state != MENU_SYSTEM_BLOCK_INIT) {
        if (info->state != MENU_SYSTEM_BLOCK_RUN)
            return;
    } else {
        info->color = MENU_SYSTEM_WINDOW_STYLE;
        WindowDXSet(&info->window);
        info->window.x = -16;
        info->window.y = 480;
        info->window.width = 544;
        info->window.height = 78;
        info->window.style = info->color;
        info->window.close_callback_arg = &info->cursorX;
        info->cursorX = 0;
        info->window.close_callback = MenuInfoWindow;
        info->window.state = WINDOWDX_STATE_OPEN;
        info->cursorY = 0;
        info->message = 0;
        WindowDXMain(&info->window);
        info->window.state = WINDOWDX_STATE_READY;
        info->visible = 1;
        info->active = (info->window.state == WINDOWDX_STATE_READY);
        memset(info->text, 0, sizeof(info->text));
        info->state = MENU_SYSTEM_BLOCK_RUN;
    }

    targetY = 362;
    switch (MenuWork.state) {
    case MENU_SYSTEM_STATE_LIST:
    case MENU_SYSTEM_STATE_SOUND_OUTPUT:
    case MENU_SYSTEM_STATE_RADAR:
    case MENU_SYSTEM_STATE_VIBRATION:
    case MENU_SYSTEM_STATE_CURSOR_MEMORY:
    case MENU_SYSTEM_STATE_DEFAULT:
    case MENU_SYSTEM_STATE_HDD:
        eMessageCpy(info->text, messageTable.messages[MenuWork.messageIndex]);
        break;
    default:
        targetY = 512;
        break;
    }
    MoveSlide(&info->window.y, &targetY, 3.0f);
    info->message = info->text;
    WindowDXMain(&info->window);
}

void MenuSystemMenuMain(void)
{
    MenuSystemMenuBlock *menu = MenuSystemMenu;
    short titleTarget;
    short choiceTarget;
    short confirmTarget;
    int row;
    ECursolMainData *labelCursor;
    WindowDX *confirmWindow;
    WindowDX *choiceWindow;
    ECursolMainData *choiceCursor;

    switch (menu->state) {
    case MENU_SYSTEM_BLOCK_INIT:
        menu->color = MENU_SYSTEM_WINDOW_STYLE;
        WindowDXSet(&menu->titleWindow);
        menu->titleWindow.x = 528;
        menu->titleWindow.y = 116;
        menu->titleWindow.style = menu->color;
        menu->titleWindow.width = 397;
        menu->titleWindow.height = 174;
        menu->titleWindow.tag_id = configurationText;
        menu->titleWindow.state = WINDOWDX_STATE_OPEN;
        WindowDXMain(&menu->titleWindow);
        menu->titleWindow.state = WINDOWDX_STATE_READY;
        for (row = 0; row < MENU_SYSTEM_OPTION_COUNT; row++) {
            eMessageSet(&menu->label[row], 0);
            menu->label[row].mode = E_MODE_SHOW_START;
            menu->label[row].draw_parameter = menu->color + 2;
            eMessageSet(&menu->value[row], 0);
            menu->value[row].mode = E_MODE_SHOW_START;
            menu->value[row].draw_parameter = menu->color + 2;
        }
        if (MenuWork.hddMounted == 0) {
            menu->label[MENU_SYSTEM_OPTION_HDD].color_b = MENU_SYSTEM_DISABLED_GRAY;
            menu->label[MENU_SYSTEM_OPTION_HDD].color_g = MENU_SYSTEM_DISABLED_GRAY;
            menu->label[MENU_SYSTEM_OPTION_HDD].color_r = MENU_SYSTEM_DISABLED_GRAY;
            menu->value[MENU_SYSTEM_OPTION_HDD].color_b = MENU_SYSTEM_DISABLED_GRAY;
            menu->value[MENU_SYSTEM_OPTION_HDD].color_g = MENU_SYSTEM_DISABLED_GRAY;
            menu->value[MENU_SYSTEM_OPTION_HDD].color_r = MENU_SYSTEM_DISABLED_GRAY;
        }
        labelCursor = &menu->labelCursor;
        choiceWindow = &menu->choiceWindow;
        eCursolSet(labelCursor, 0);
        menu->labelCursor.work = menu->color + 2;
        labelCursor->state = E_MODE_SHOW_START;
        WindowDXSet(choiceWindow);
        choiceWindow->x = 528;
        menu->choiceWindow.y = 116;
        menu->choiceWindow.style = menu->color;
        menu->choiceWindow.width = 113;
        menu->choiceWindow.height = 70;
        menu->choiceWindow.tag_id = selectionText;
        menu->choiceWindow.state = WINDOWDX_STATE_OPEN;
        WindowDXMain(choiceWindow);
        menu->choiceWindow.state = WINDOWDX_STATE_READY;
        for (row = 0; row < MENU_SYSTEM_CHOICE_ROWS; row++) {
            eMessageSet(&menu->choiceLabel[row], msg01_2_0036DD20[0]);
            menu->choiceLabel[row].mode = E_MODE_SHOW_START;
            menu->choiceLabel[row].draw_parameter = menu->color + 2;
        }
        choiceCursor = &menu->choiceCursor;
        confirmWindow = &menu->confirmWindow;
        eCursolSet(choiceCursor, 0);
        menu->choiceCursor.work = menu->color + 2;
        choiceCursor->state = E_MODE_SHOW_START;
        WindowDXSet(confirmWindow);
        confirmWindow->x = 544;
        menu->confirmWindow.y = 116;
        menu->confirmWindow.style = menu->color;
        menu->confirmWindow.width = 169;
        menu->confirmWindow.height = 102;
        menu->confirmWindow.tag_id = selectionText;
        menu->confirm.choices = msg03_4_0036DD30[1];
        menu->confirmWindow.close_callback = MenuSelectWindow;
        menu->confirmWindow.close_callback_arg = &menu->confirm;
        menu->confirm.question = msg03_4_0036DD30[0];
        menu->confirm.cursor = 0;
        menu->confirmWindow.state = menu->confirm.enabled = 1;
        WindowDXMain(confirmWindow);
        menu->confirmWindow.state = WINDOWDX_STATE_READY;
        menu->state = MENU_SYSTEM_BLOCK_RUN;
        break;
    case MENU_SYSTEM_BLOCK_RUN:
        labelCursor = &menu->labelCursor;
        choiceWindow = &menu->choiceWindow;
        choiceCursor = &menu->choiceCursor;
        confirmWindow = &menu->confirmWindow;
        break;
    default:
        return;
    }

    titleTarget = 528;
    choiceTarget = 528;
    confirmTarget = 528;
    switch (MenuWork.state) {
    case MENU_SYSTEM_STATE_LIST:
        titleTarget = 48;
        break;
    case MENU_SYSTEM_STATE_SOUND_OUTPUT:
        if ((unsigned char)(MenuWork.flags & MENU_FLAG_ENTER)) {
            menu->choiceWindow.height = 54;
            menu->choiceLabel[0].text = msg01_2_0036DD20[0];
            menu->choiceLabel[1].text = msg01_2_0036DD20[1];
            menu->choiceLabel[2].text = 0;
        }
        titleTarget = 0;
        choiceTarget = 400;
        break;
    case MENU_SYSTEM_STATE_RADAR:
    case MENU_SYSTEM_STATE_VIBRATION:
    case MENU_SYSTEM_STATE_CURSOR_MEMORY:
    case MENU_SYSTEM_STATE_HDD:
        if ((unsigned char)(MenuWork.flags & MENU_FLAG_ENTER)) {
            menu->choiceWindow.height = 54;
            menu->choiceLabel[0].text = msg02_3_0036DD28[0];
            menu->choiceLabel[1].text = msg02_3_0036DD28[1];
            menu->choiceLabel[2].text = 0;
        }
        titleTarget = 0;
        choiceTarget = 400;
        break;
    case MENU_SYSTEM_STATE_DEFAULT:
        titleTarget = -56;
        confirmTarget = 344;
        break;
    }
    for (row = 0; row < 1; row++) {
        WindowDX *titleWindow = &menu->titleWindow + row;
        MoveSlide(&titleWindow->x, &titleTarget, 3.0f);
        WindowDXMain(titleWindow);
    }
    {
        MenuSystemOptionNames *settingLabels[MENU_SYSTEM_OPTION_COUNT];
        __builtin_memcpy(settingLabels, settingLabelNames, sizeof(settingLabels));
        for (row = 0; row < MENU_SYSTEM_OPTION_COUNT; row++) {
            if (row == MENU_SYSTEM_OPTION_HDD)
                continue;
            menu->label[row].x = menu->titleWindow.x + 19;
            menu->label[row].text = msg00_1_0036DD00[row];
            menu->label[row].y = menu->titleWindow.y + row * MENU_SYSTEM_ROW_HEIGHT + 3;
            eMessageMain(&menu->label[row]);
            if (row != MENU_SYSTEM_OPTION_MONITOR && row != MENU_SYSTEM_OPTION_DEFAULT) {
                if (row == MENU_SYSTEM_OPTION_HDD && MenuWork.hddMounted == 0)
                    menu->value[MENU_SYSTEM_OPTION_HDD].text = msg02_3_0036DD28[1];
                else
                    menu->value[row].text = (*settingLabels[row])[MenuWork.options[row]];
                menu->value[row].x = menu->titleWindow.x + 303;
                menu->value[row].y = menu->titleWindow.y + row * MENU_SYSTEM_ROW_HEIGHT + 3;
                eMessageMain(&menu->value[row]);
            }
        }
    }
    menu->labelCursor.x = menu->titleWindow.x + 3;
    menu->labelCursor.y = menu->titleWindow.y + MenuWork.messageIndex * MENU_SYSTEM_ROW_HEIGHT + 7;
    eCursolMain(labelCursor);
    MoveSlide(&choiceWindow->x, &choiceTarget, 3.0f);
    WindowDXMain(choiceWindow);
    for (row = 0; row < MENU_SYSTEM_CHOICE_ROWS; row++) {
        menu->choiceLabel[row].x = menu->choiceWindow.x + 19;
        menu->choiceLabel[row].y = menu->choiceWindow.y + row * MENU_SYSTEM_ROW_HEIGHT + 3;
        eMessageMain(&menu->choiceLabel[row]);
    }
    menu->choiceCursor.x = menu->choiceWindow.x + 3;
    menu->choiceCursor.y = menu->choiceWindow.y + MenuWork.choiceIndex * MENU_SYSTEM_ROW_HEIGHT + 7;
    eCursolMain(choiceCursor);
    MoveSlide(&confirmWindow->x, &confirmTarget, 3.0f);
    menu->confirm.cursor = MenuWork.choiceIndex;
    WindowDXMain(confirmWindow);
}

void MenuSystemInitSet(void)
{
    PartyDataPrefix *party;

    SsdSetOutputMode(1);
    SaveData[SAVE_DATA_SOUND_OUTPUT] = 1;
    MenuWork.soundOutput = 0;
    SaveData[SAVE_DATA_CURSOR_MEMORY] = 0;
    MenuWork.padMode = 0;
    SaveData[SAVE_DATA_VIBRATION] = 0;
    PadData.vibrationReset = 0;
    PadData.vibrationStatus = 0;
    MenuWork.radarMode = 0;
    PartyRadarDispSet(1);
    party = PartyDataGet();
    MenuWork.displayMode = party->radar_disp ^ 1;
    tyaDisplaySetting(1);
    if (MenuWork.hddMounted != 0) {
        SaveData[SAVE_DATA_HDD] = 0;
        xglHddActivate(1);
        MenuWork.hddActive = SaveData[SAVE_DATA_HDD];
    }
}

void subMenuSystemMain(void)
{
    unsigned char optionStates[7];
    int outputModes[2];
    int displayModes[2];
    int vibrationModes[2];
    int padModes[2];
    int hddModes[2];

    if (MenuWork.state != MenuWork.nextState) {
        MenuWork.state = MenuWork.nextState;
        MenuWork.flags |= MENU_FLAG_ENTER;
    } else {
        MenuWork.flags &= ~MENU_FLAG_ENTER;
    }
    if (MenuWork.inputDelay != 0)
        MenuWork.inputDelay--;

    switch (MenuWork.state) {
    case MENU_SYSTEM_STATE_LIST:
        if (MenuWork.inputDelay == 0) {
            signed char previousSelection = MenuWork.messageIndex;
            int selection = MenuSelectMove(previousSelection, MENU_SYSTEM_OPTION_COUNT, 0);
            MenuWork.messageIndex = selection;
            if (MenuWork.messageIndex == MENU_SYSTEM_OPTION_HDD)
                MenuWork.messageIndex = selection + (selection - previousSelection);
            if (PadData.pressed & PAD_CIRCLE) {
                __builtin_memcpy(optionStates, optionStateTable, sizeof(optionStates));
                if (MenuWork.messageIndex == MENU_SYSTEM_OPTION_MONITOR) {
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                    MenuModelMemorySet(0);
                    xglSleep();
                    tyaDisplaySetting((int)MenuModelWorkTop);
                } else if (MenuWork.messageIndex == MENU_SYSTEM_OPTION_HDD && MenuWork.hddMounted == 0) {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                } else if (optionStates[MenuWork.messageIndex] != 0) {
                    MenuWork.nextState = optionStates[MenuWork.messageIndex];
                    MenuWork.inputDelay = MENU_SYSTEM_DELAY_CHOICE;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                }
            }
            if (PadData.pressed & PAD_CROSS) {
                MenuWork.nextState = MENU_SYSTEM_STATE_CLOSE;
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
        }
        break;
    case MENU_SYSTEM_STATE_SOUND_OUTPUT:
        if ((unsigned char)(MenuWork.flags & MENU_FLAG_ENTER))
            MenuWork.choiceIndex = 0;
        if (MenuWork.inputDelay != 0)
            break;
        {
            MenuWork.choiceIndex = MenuSelectMove(MenuWork.choiceIndex, MENU_SYSTEM_CHOICE_COUNT, 0);
            if (PadData.pressed & PAD_CIRCLE) {
                __builtin_memcpy(outputModes, outputModeTable, sizeof(outputModes));
                SsdSetOutputMode(outputModes[MenuWork.choiceIndex]);
                SaveData[SAVE_DATA_SOUND_OUTPUT] = outputModes[MenuWork.choiceIndex];
                MenuWork.nextState = MENU_SYSTEM_STATE_LIST;
                MenuWork.inputDelay = MENU_SYSTEM_DELAY_CHOICE;
                MenuWork.soundOutput = SaveData[SAVE_DATA_SOUND_OUTPUT] ^ 1;
                xglSoundEffectNormalID(SE_DECIDE, 0);
            }
            if (PadData.pressed & PAD_CROSS) {
                MenuWork.nextState = MENU_SYSTEM_STATE_LIST;
                MenuWork.inputDelay = MENU_SYSTEM_DELAY_CHOICE;
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
        }
        break;
    case MENU_SYSTEM_STATE_RADAR:
        if ((unsigned char)(MenuWork.flags & MENU_FLAG_ENTER))
            MenuWork.choiceIndex = 0;
        if (MenuWork.inputDelay != 0)
            break;
        {
            MenuWork.choiceIndex = MenuSelectMove(MenuWork.choiceIndex, MENU_SYSTEM_CHOICE_COUNT, 0);
            if (PadData.pressed & PAD_CIRCLE) {
                PartyDataPrefix *party;
                __builtin_memcpy(displayModes, displayModeTable, sizeof(displayModes));
                PartyRadarDispSet(displayModes[MenuWork.choiceIndex]);
                party = PartyDataGet();
                MenuWork.displayMode = party->radar_disp ^ 1;
                MenuWork.nextState = MENU_SYSTEM_STATE_LIST;
                MenuWork.inputDelay = MENU_SYSTEM_DELAY_CHOICE;
                xglSoundEffectNormalID(SE_DECIDE, 0);
            }
            if (PadData.pressed & PAD_CROSS) {
                MenuWork.nextState = MENU_SYSTEM_STATE_LIST;
                MenuWork.inputDelay = MENU_SYSTEM_DELAY_CHOICE;
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
        }
        break;
    case MENU_SYSTEM_STATE_VIBRATION:
        if ((unsigned char)(MenuWork.flags & MENU_FLAG_ENTER))
            MenuWork.choiceIndex = 0;
        if (MenuWork.inputDelay != 0)
            break;
        {
            MenuWork.choiceIndex = MenuSelectMove(MenuWork.choiceIndex, MENU_SYSTEM_CHOICE_COUNT, 0);
            if (PadData.pressed & PAD_CIRCLE) {
                __builtin_memcpy(vibrationModes, vibrationModeTable, sizeof(vibrationModes));
                PadData.vibrationReset = vibrationModes[MenuWork.choiceIndex];
                SaveData[SAVE_DATA_VIBRATION] = vibrationModes[MenuWork.choiceIndex];
                PadData.vibrationStatus = vibrationModes[MenuWork.choiceIndex];
                MenuWork.radarMode = vibrationModes[MenuWork.choiceIndex];
                if (vibrationModes[MenuWork.choiceIndex] == 0)
                    MenuBibrationSet(0, 1, MENU_SYSTEM_VIBRATION_TEST_STRENGTH,
                                     MENU_SYSTEM_VIBRATION_TEST_DURATION);
                MenuWork.nextState = MENU_SYSTEM_STATE_LIST;
                MenuWork.inputDelay = MENU_SYSTEM_DELAY_CHOICE;
                xglSoundEffectNormalID(SE_DECIDE, 0);
            }
            if (PadData.pressed & PAD_CROSS) {
                MenuWork.nextState = MENU_SYSTEM_STATE_LIST;
                MenuWork.inputDelay = MENU_SYSTEM_DELAY_CHOICE;
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
        }
        break;
    case MENU_SYSTEM_STATE_CURSOR_MEMORY:
        if ((unsigned char)(MenuWork.flags & MENU_FLAG_ENTER))
            MenuWork.choiceIndex = 0;
        if (MenuWork.inputDelay != 0)
            break;
        {
            MenuWork.choiceIndex = MenuSelectMove(MenuWork.choiceIndex, MENU_SYSTEM_CHOICE_COUNT, 0);
            if (PadData.pressed & PAD_CIRCLE) {
                __builtin_memcpy(padModes, padModeTable, sizeof(padModes));
                SaveData[SAVE_DATA_CURSOR_MEMORY] = padModes[MenuWork.choiceIndex];
                MenuWork.padMode = padModes[MenuWork.choiceIndex];
                MenuWork.nextState = MENU_SYSTEM_STATE_LIST;
                MenuWork.inputDelay = MENU_SYSTEM_DELAY_CHOICE;
                xglSoundEffectNormalID(SE_DECIDE, 0);
            }
            if (PadData.pressed & PAD_CROSS) {
                MenuWork.nextState = MENU_SYSTEM_STATE_LIST;
                MenuWork.inputDelay = MENU_SYSTEM_DELAY_CHOICE;
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
        }
        break;
    case MENU_SYSTEM_STATE_DEFAULT:
        if ((unsigned char)(MenuWork.flags & MENU_FLAG_ENTER))
            MenuWork.choiceIndex = 0;
        if (MenuWork.inputDelay != 0)
            break;
        {
            MenuWork.choiceIndex = MenuSelectMove(MenuWork.choiceIndex, MENU_SYSTEM_CHOICE_COUNT, 0);
            if (PadData.pressed & PAD_CIRCLE) {
                if (MenuWork.choiceIndex == 0) {
                    MenuSystemInitSet();
                    MenuWork.nextState = MENU_SYSTEM_STATE_LIST;
                    MenuWork.inputDelay = MENU_SYSTEM_DELAY_CHOICE;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    MenuWork.nextState = MENU_SYSTEM_STATE_LIST;
                    MenuWork.inputDelay = MENU_SYSTEM_DELAY_CHOICE;
                    xglSoundEffectNormalID(SE_CANCEL, 0);
                }
            }
            if (PadData.pressed & PAD_CROSS) {
                MenuWork.nextState = MENU_SYSTEM_STATE_LIST;
                MenuWork.inputDelay = MENU_SYSTEM_DELAY_CHOICE;
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
        }
        break;
    case MENU_SYSTEM_STATE_HDD:
        if ((unsigned char)(MenuWork.flags & MENU_FLAG_ENTER))
            MenuWork.choiceIndex = 0;
        if (MenuWork.inputDelay != 0)
            break;
        {
            MenuWork.choiceIndex = MenuSelectMove(MenuWork.choiceIndex, MENU_SYSTEM_CHOICE_COUNT, 0);
            if (PadData.pressed & PAD_CIRCLE) {
                __builtin_memcpy(hddModes, hddModeTable, sizeof(hddModes));
                SaveData[SAVE_DATA_HDD] = hddModes[MenuWork.choiceIndex] ^ 1;
                xglHddActivate(hddModes[MenuWork.choiceIndex]);
                MenuWork.hddActive = SaveData[SAVE_DATA_HDD];
                MenuWork.nextState = MENU_SYSTEM_STATE_LIST;
                MenuWork.inputDelay = MENU_SYSTEM_DELAY_CHOICE;
                xglSoundEffectNormalID(SE_DECIDE, 0);
            }
            if (PadData.pressed & PAD_CROSS) {
                MenuWork.nextState = MENU_SYSTEM_STATE_LIST;
                MenuWork.inputDelay = MENU_SYSTEM_DELAY_CHOICE;
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
        }
        break;
    case MENU_SYSTEM_STATE_CLOSE:
        if ((unsigned char)(MenuWork.flags & MENU_FLAG_ENTER))
            MenuWork.inputDelay = MENU_SYSTEM_DELAY_CLOSE;
        if (MenuWork.inputDelay == 0)
            MenuWork.nextState = MENU_SYSTEM_STATE_DONE;
        break;
    }
    MenuSystemPasMain();
    MenuSystemInfoMain();
    MenuSystemMenuMain();
    xglFontDebugPrintf(0, 64, systemText);
    xglFontDebugPrintf(0, 80, messageDelayFormat, MenuWork.messageIndex);
}

void subMenuSystemInit(int workEnd)
{
    PartyDataPrefix *party;
    void *cursor = (void *)((workEnd + 127) & -128);

    MenuSystemPas = cursor;
    cursor += sizeof(*MenuSystemPas);
    MenuSystemPas->state = MENU_SYSTEM_BLOCK_INIT;
    MenuSystemInfo = cursor;
    cursor += sizeof(*MenuSystemInfo);
    MenuSystemInfo->state = MENU_SYSTEM_BLOCK_INIT;
    MenuSystemMenu = cursor;
    cursor += sizeof(*MenuSystemMenu);
    MenuSystemMenu->state = MENU_SYSTEM_BLOCK_INIT;
    MenuWork.nextState = MENU_SYSTEM_STATE_LIST;
    MenuWork.flags = 0;
    MenuWork.soundOutput = SaveData[SAVE_DATA_SOUND_OUTPUT] ^ 1;
    MenuWork.inputDelay = MENU_SYSTEM_DELAY_OPEN;
    MenuWork.padMode = SaveData[SAVE_DATA_CURSOR_MEMORY];
    MenuWork.radarMode = SaveData[SAVE_DATA_VIBRATION];
    party = PartyDataGet();
    MenuWork.displayMode = party->radar_disp ^ 1;
    MenuHddCheck = xglHddMount();
    if (MenuHddCheck == 1 && xglHddActivate(-1) != 0) {
        MenuWork.hddMounted = 1;
        MenuWork.hddActive = SaveData[SAVE_DATA_HDD];
    } else {
        MenuWork.hddActive = 1;
        MenuWork.hddMounted = 0;
    }
}

void MenuSystem(void)
{
    switch (MenuWork.menuState) {
        /* Keep both completion paths in this single dispatch scope. */
        do {
        case MENU_SYSTEM_TASK_INIT:
            subMenuSystemInit((int)MainMenuWorkEnd);
            MenuWork.menuState = MENU_SYSTEM_TASK_RUN;
            /* fall through */
        case MENU_SYSTEM_TASK_RUN:
            subMenuSystemMain();
            if (MenuWork.state != MENU_SYSTEM_STATE_DONE)
                break;
            MenuWork.menuState = MENU_SYSTEM_TASK_EXIT;
            /* fall through */
        case MENU_SYSTEM_TASK_EXIT:
            ChangeTopLevel(0);
            break;
        } while (0);
    }
}

void MenuSystem2Init(void) {
    endPrintInit();
    subMenuSystemInit((int)MainMenuWorkEnd);
}

int MenuSystem2(void)
{
    subMenuSystemMain();
    endPrintExtFunc(0, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_FLUSH, 0);
    return MenuWork.state != MENU_SYSTEM_STATE_DONE;
}

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
MenuSystemOptionNames *const settingLabelNames[MENU_SYSTEM_OPTION_COUNT] = { &msg01_2_0036DD20, &msg02_3_0036DD28, &msg02_3_0036DD28, &msg02_3_0036DD28, &msg02_3_0036DD28, &msg02_3_0036DD28, &msg02_3_0036DD28 };

unsigned char optionStateTable[8] = { 0x20, 0x50, 0x40, 0x30, 0x00, 0x70, 0x60, 0x00 };

int outputModeTable[2] = { 1, 0 };

int displayModeTable[2] = { 1, 0 };

int vibrationModeTable[2] = { 0, 1 };

int padModeTable[2] = { 0, 1 };

int hddModeTable[2] = { 1, 0 };

/* Text: {reset}system */
char systemText[8] = "\013system";

/* Text: {reset}ms %2d */
char messageDelayFormat[8] = "\013ms %2d";

MenuSystemPasBlock *MenuSystemPas = 0;

MenuSystemInfoBlock *MenuSystemInfo = 0;

MenuSystemMenuBlock *MenuSystemMenu = 0;

XglClock _CountTime = {0};

const char MenuSkillSetSuffixText[8] = "/Set";

const char MenuSkillTitleText[8] = "Skills";
