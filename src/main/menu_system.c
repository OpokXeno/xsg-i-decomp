#include "common.h"
#include "shared.h"
#include "main/e_battle_win_open.h"
#include "main/party.h"
#include "main/xgl_hdd.h"
#include "main/window_tex_load.h"
#include "main/menu_top_command_check.h"
#include "main/xgl_thread.h"

/* Existing option names designate entries in the contiguous option array. */
#define soundOutput options[0]

#define padMode options[1]
#define radarMode options[2]
#define displayMode options[3]
#define hddActive options[5]

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
    unsigned char options[7];
} MenuSystemWork;

typedef struct MenuSystemPad {
    unsigned char unmodeled_00[0x2a];
    unsigned short pressed;
    unsigned char unmodeled_2c[0x56 - 0x2c];
    unsigned char vibrationReset;
    unsigned char unmodeled_57[0xbe - 0x57];
    unsigned char vibrationStatus;
} MenuSystemPad;

typedef struct MenuSystemPartyDataPrefix {
    unsigned char unmodeled_00[0x2e];
    unsigned char radar_disp;
} MenuSystemPartyDataPrefix;

typedef struct MenuSystemInfoBlock {
    unsigned char state;
    unsigned char active;
    unsigned char unmodeled_02[2];
    int color;
    short x;
    short y;
    int previousColor;
    short width;
    short height;
    unsigned char unmodeled_14[4];
    unsigned char transition;
    unsigned char unmodeled_19[3];
    void (*window)(void);
    void *subwindow;
    unsigned char unmodeled_24[0x1a0 - 0x24];
    short cursorX;
    short cursorY;
    unsigned char unmodeled_1a4[4];
    int visible;
    unsigned char *message;
    unsigned char unmodeled_1b0[0x350 - 0x1b0];
    char text[128];
} MenuSystemInfoBlock;

typedef struct MenuSystemMessageTable {
    char *messages[7];
} MenuSystemMessageTable;

typedef union MenuSystemMessageStorage {
    MenuSystemMessageTable table;
    unsigned int words[8];
} MenuSystemMessageStorage;

enum {
    SAVE_DATA_BYTE_28 = 0x28,
    SAVE_DATA_BYTE_44 = 0x44,
    SAVE_DATA_BYTE_3C = 0x3c,
    SAVE_DATA_BYTE_40 = 0x40
};

typedef struct MenuSystemPanelWindow {
    short x;
    short y;
    int color;
    short width;
    short height;
    unsigned char unmodeled_0c[4];
    signed char transition;
    unsigned char unmodeled_11[0x194 - 0x11];
} MenuSystemPanelWindow;

typedef struct MenuSystemPanelMessage {
    unsigned char unmodeled_00;
    unsigned char mode;
    unsigned char unmodeled_02[2];
    short x;
    short y;
    int color;
    unsigned char unmodeled_0c[0x44 - 0x0c];
} MenuSystemPanelMessage;

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
    MenuSystemPanelWindow window;
    MenuSystemPanelMessage message[1];
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

typedef struct MenuSystemMessage {
    unsigned char unmodeled_00;
    unsigned char flag;
    unsigned char unmodeled_02[2];
    short x;
    short y;
    int color;
    unsigned char unmodeled_0c[4];
    unsigned char rgb[3];
    unsigned char unmodeled_13[5];
    const char *text;
    unsigned char unmodeled_1c[0x44 - 0x1c];
} MenuSystemMessage;

typedef struct MenuSystemCursor {
    unsigned char flag;
    unsigned char unmodeled_01[3];
    short x;
    short y;
    int color;
    unsigned char unmodeled_0c[0x24 - 0x0c];
} MenuSystemCursor;

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
    MenuSystemMessage label[8];
    MenuSystemMessage value[8];
    MenuSystemCursor labelCursor;
    WindowDX choiceWindow;
    MenuSystemMessage choiceLabel[3];
    MenuSystemCursor choiceCursor;
    WindowDX confirmWindow;
    MenuSystemChoice confirm;
} MenuSystemMenuBlock;

typedef const char *MenuSystemOptionNames[2];

extern int MainMenuWorkEnd;

/* The original stores the HDD mount result through this four-byte object. */
extern int MenuHddCheck;

extern MenuSystemWork MenuWork;
extern MenuSystemPad PadData;
extern MenuSystemInfoBlock *MenuSystemInfo;
extern XglClock _CountTime;
extern MenuSystemPasBlock *MenuSystemPas;
extern const char MenuSystemOptionHelpText[];
extern MenuSystemMenuBlock *MenuSystemMenu;
extern const char configurationText[];
extern const char selectionText[];
static const char * msg01_2_0036DD20[] ;
static const char * msg02_3_0036DD28[] ;
static const char * msg03_4_0036DD30[] ;
extern MenuSystemOptionNames *const settingLabelNames[];
extern int MenuModelWorkTop;
extern unsigned char optionStateTable[];
extern int outputModeTable[];
extern int displayModeTable[];
extern int vibrationModeTable[];
extern int padModeTable[];
extern int hddModeTable[];
extern char systemText[];
extern char messageDelayFormat[];
extern const char MenuSkillTitleText[8];
extern const char MenuSystemDefaultText[8];
extern const char MenuSystemHddText[8];
extern const char MenuSystemStereoText[8];
extern const char MenuSystemMonoText[8];
extern const char MenuSystemOffText[8];
extern const char MenuSystemOnText[8];
extern const char MenuSystemYesNoText[8];
extern const char MenuSkillSetSuffixText[8];

extern void subMenuSystemInit(int workEnd);
extern void subMenuSystemMain(void);
extern void ChangeTopLevel(int level);
void SsdSetOutputMode(int mode);
void PartyRadarDispSet(int enabled);
void tyaDisplaySetting(int mode);
extern void eMessageCpy(char *message, char *text);
extern void MoveSlide(short *value, short *destination, float speed);
extern void *memset(void *destination, int value, unsigned int count);
extern void endPrintInit(void);
extern void endPrintExtFunc(int kind, int id, void *data);
extern void eMessageSet(void *message, const char *text);
extern void eMessageMain(void *message);
extern void eCursolSet(void *cursor, signed char width);
extern void eCursolMain(void *cursor);
extern int MenuSelectMove(int selection, int count, int wrap);
extern void MenuModelMemorySet(int mode);

extern void MenuBibrationSet(unsigned char pad, unsigned char mode,
                             unsigned char strength, unsigned char duration);

extern void MenuSystemMenuMain(void);
extern void xglSoundEffectNormalID(int id, int mode);
extern void xglFontDebugPrintf(int x, int y, const char *format, ...);

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

static const char * msg00_1_0036DD00[7];

static const char * msg00_1_0036DD00[7] = { soundOutputText, cursorLocationMemoryText, vibrationText, radarDisplayText, monitorSettingsText, MenuSystemHddText, MenuSystemDefaultText };

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

    if (panel->state != 0) {
        if (panel->state != 2)
            return;
    } else {
        panel->color = 0xfffff0;
        WindowDXSet((WindowDX *)&panel->window);
        panel->window.x = -288;
        panel->window.color = panel->color;
        panel->window.y = 8;
        panel->window.width = 272;
        panel->window.height = 30;
        panel->window.transition = 1;
        WindowDXMain((WindowDX *)&panel->window);
        panel->window.transition = 3;
        for (row = 0; row < 1; row++) {
            eMessageSet(&panel->message[row], msg00_0_0036DCF8[row]);
            panel->message[row].mode = 32;
            panel->message[row].x = 288;
            panel->message[row].y = 11;
            panel->message[row].color = panel->color + 2;
        }
        panel->state = 2;
    }
    windowTarget[0] = -16;
    messageTarget[0] = 288;
    switch (MenuWork.state) {
    case 0x10:
    case 0x20:
    case 0x30:
    case 0x40:
    case 0x50:
    case 0x60:
    case 0x70:
        messageTarget[0] = 16;
        break;
    default:
        windowTarget[0] = -288;
        break;
    }
    MoveSlide(&panel->window.x, windowTarget, 3.0f);
    WindowDXMain((WindowDX *)&panel->window);
    if (panel->window.transition == 3) {
        panel->box.x = panel->window.x + 3;
        panel->box.y = panel->window.y + 3;
        panel->box.width = panel->window.width - 6;
        panel->box.height = panel->window.height - 6;
        panel->box.color = panel->color;
        endPrintExtFunc(panel->color, 101, &panel->box);
        MoveSlide(&panel->message[0].x, messageTarget, 5.0f);
        if (panel->message[0].x < 256)
            eMessageMain(&panel->message[0]);
        endPrintExtFunc(panel->color, 102, 0);
    }
}

void MenuSystemInfoMain(void)
{
    MenuSystemInfoBlock *info = MenuSystemInfo;
    MenuSystemMessageTable messageTable = messageTexts.table;
    short targetY;

    if (info->state != 0) {
        if (info->state != 2)
            return;
    } else {
        info->color = 0xFFFFF0;
        WindowDXSet((WindowDX *)&info->x);
        info->x = -16;
        info->y = 480;
        info->width = 544;
        info->height = 78;
        info->previousColor = info->color;
        info->subwindow = &info->cursorX;
        info->cursorX = 0;
        info->window = (void (*)(void))MenuInfoWindow;
        info->transition = 1;
        info->cursorY = 0;
        info->message = 0;
        WindowDXMain((WindowDX *)&info->x);
        info->transition = 3;
        info->visible = 1;
        info->active = (info->transition == 3);
        memset(info->text, 0, sizeof(info->text));
        info->state = 2;
    }

    targetY = 362;
    switch (MenuWork.state) {
    case 0x10:
    case 0x20:
    case 0x30:
    case 0x40:
    case 0x50:
    case 0x60:
    case 0x70:
        eMessageCpy(info->text, messageTable.messages[MenuWork.messageIndex]);
        break;
    default:
        targetY = 512;
        break;
    }
    MoveSlide(&info->y, &targetY, 3.0f);
    info->message = info->text;
    WindowDXMain((WindowDX *)&info->x);
}

void MenuSystemMenuMain(void)
{
    MenuSystemMenuBlock *menu = MenuSystemMenu;
    short titleTarget;
    short choiceTarget;
    short confirmTarget;
    int row;
    MenuSystemCursor *labelCursor;
    WindowDX *confirmWindow;
    WindowDX *choiceWindow;
    MenuSystemCursor *choiceCursor;

    switch (menu->state) {
    case 0:
        menu->color = 0xfffff0;
        WindowDXSet(&menu->titleWindow);
        menu->titleWindow.x = 528;
        menu->titleWindow.y = 116;
        menu->titleWindow.style = menu->color;
        menu->titleWindow.width = 397;
        menu->titleWindow.height = 174;
        menu->titleWindow.tag_id = configurationText;
        menu->titleWindow.state = 1;
        WindowDXMain(&menu->titleWindow);
        menu->titleWindow.state = 3;
        for (row = 0; row < 7; row++) {
            eMessageSet(&menu->label[row], 0);
            menu->label[row].flag = 0x20;
            menu->label[row].color = menu->color + 2;
            eMessageSet(&menu->value[row], 0);
            menu->value[row].flag = 0x20;
            menu->value[row].color = menu->color + 2;
        }
        if (MenuWork.hddMounted == 0) {
            menu->label[5].rgb[2] = 64;
            menu->label[5].rgb[1] = 64;
            menu->label[5].rgb[0] = 64;
            menu->value[5].rgb[2] = 64;
            menu->value[5].rgb[1] = 64;
            menu->value[5].rgb[0] = 64;
        }
        labelCursor = &menu->labelCursor;
        choiceWindow = &menu->choiceWindow;
        eCursolSet(labelCursor, 0);
        menu->labelCursor.color = menu->color + 2;
        labelCursor->flag = 0x20;
        WindowDXSet(choiceWindow);
        choiceWindow->x = 528;
        menu->choiceWindow.y = 116;
        menu->choiceWindow.style = menu->color;
        menu->choiceWindow.width = 113;
        menu->choiceWindow.height = 70;
        menu->choiceWindow.tag_id = selectionText;
        menu->choiceWindow.state = 1;
        WindowDXMain(choiceWindow);
        menu->choiceWindow.state = 3;
        for (row = 0; row < 3; row++) {
            eMessageSet(&menu->choiceLabel[row], msg01_2_0036DD20[0]);
            menu->choiceLabel[row].flag = 0x20;
            menu->choiceLabel[row].color = menu->color + 2;
        }
        choiceCursor = &menu->choiceCursor;
        confirmWindow = &menu->confirmWindow;
        eCursolSet(choiceCursor, 0);
        menu->choiceCursor.color = menu->color + 2;
        choiceCursor->flag = 0x20;
        WindowDXSet(confirmWindow);
        confirmWindow->x = 544;
        menu->confirmWindow.y = 116;
        menu->confirmWindow.style = menu->color;
        menu->confirmWindow.width = 169;
        menu->confirmWindow.height = 102;
        menu->confirmWindow.tag_id = selectionText;
        menu->confirm.choices = msg03_4_0036DD30[1];
        menu->confirmWindow.close_callback = (void (*)(WindowDX *, void *))MenuSelectWindow;
        menu->confirmWindow.close_callback_arg = &menu->confirm;
        menu->confirm.question = msg03_4_0036DD30[0];
        menu->confirm.cursor = 0;
        menu->confirmWindow.state = menu->confirm.enabled = 1;
        WindowDXMain(confirmWindow);
        menu->confirmWindow.state = 3;
        menu->state = 2;
        break;
    case 2:
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
    case 0x10:
        titleTarget = 48;
        break;
    case 0x20:
        if ((unsigned char)(MenuWork.flags & 1)) {
            menu->choiceWindow.height = 54;
            menu->choiceLabel[0].text = msg01_2_0036DD20[0];
            menu->choiceLabel[1].text = msg01_2_0036DD20[1];
            menu->choiceLabel[2].text = 0;
        }
        titleTarget = 0;
        choiceTarget = 400;
        break;
    case 0x30:
    case 0x40:
    case 0x50:
    case 0x70:
        if ((unsigned char)(MenuWork.flags & 1)) {
            menu->choiceWindow.height = 54;
            menu->choiceLabel[0].text = msg02_3_0036DD28[0];
            menu->choiceLabel[1].text = msg02_3_0036DD28[1];
            menu->choiceLabel[2].text = 0;
        }
        titleTarget = 0;
        choiceTarget = 400;
        break;
    case 0x60:
        titleTarget = -56;
        confirmTarget = 344;
        break;
    }
    for (row = 0; row < 1; row++) {
        WindowDX *titleWindow = (WindowDX *)((unsigned char *)menu + row + 8);
        MoveSlide(&titleWindow->x, &titleTarget, 3.0f);
        WindowDXMain(titleWindow);
    }
    {
        MenuSystemOptionNames *settingLabels[7];
        __builtin_memcpy(settingLabels, settingLabelNames, sizeof(settingLabels));
        for (row = 0; row < 7; row++) {
            if (row == 5)
                continue;
            menu->label[row].x = menu->titleWindow.x + 19;
            menu->label[row].text = msg00_1_0036DD00[row];
            menu->label[row].y = menu->titleWindow.y + row * 24 + 3;
            eMessageMain(&menu->label[row]);
            if (row != 4 && row != 6) {
                if (row == 5 && MenuWork.hddMounted == 0)
                    menu->value[5].text = msg02_3_0036DD28[1];
                else
                    menu->value[row].text = (*settingLabels[row])[MenuWork.options[row]];
                menu->value[row].x = menu->titleWindow.x + 303;
                menu->value[row].y = menu->titleWindow.y + row * 24 + 3;
                eMessageMain(&menu->value[row]);
            }
        }
    }
    menu->labelCursor.x = menu->titleWindow.x + 3;
    menu->labelCursor.y = menu->titleWindow.y + MenuWork.messageIndex * 24 + 7;
    eCursolMain(labelCursor);
    MoveSlide(&choiceWindow->x, &choiceTarget, 3.0f);
    WindowDXMain(choiceWindow);
    for (row = 0; row < 3; row++) {
        menu->choiceLabel[row].x = menu->choiceWindow.x + 19;
        menu->choiceLabel[row].y = menu->choiceWindow.y + row * 24 + 3;
        eMessageMain(&menu->choiceLabel[row]);
    }
    menu->choiceCursor.x = menu->choiceWindow.x + 3;
    menu->choiceCursor.y = menu->choiceWindow.y + MenuWork.choiceIndex * 24 + 7;
    eCursolMain(choiceCursor);
    MoveSlide(&confirmWindow->x, &confirmTarget, 3.0f);
    menu->confirm.cursor = MenuWork.choiceIndex;
    WindowDXMain(confirmWindow);
}

void MenuSystemInitSet(void)
{
    MenuSystemPartyDataPrefix *party;

    SsdSetOutputMode(1);
    SaveData[SAVE_DATA_BYTE_28] = 1;
    MenuWork.soundOutput = 0;
    SaveData[SAVE_DATA_BYTE_44] = 0;
    MenuWork.padMode = 0;
    SaveData[SAVE_DATA_BYTE_3C] = 0;
    PadData.vibrationReset = 0;
    PadData.vibrationStatus = 0;
    MenuWork.radarMode = 0;
    PartyRadarDispSet(1);
    party = (MenuSystemPartyDataPrefix *)PartyDataGet();
    MenuWork.displayMode = party->radar_disp ^ 1;
    tyaDisplaySetting(1);
    if (MenuWork.hddMounted != 0) {
        SaveData[SAVE_DATA_BYTE_40] = 0;
        xglHddActivate(1);
        MenuWork.hddActive = SaveData[SAVE_DATA_BYTE_40];
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
        MenuWork.flags |= 1;
    } else {
        MenuWork.flags &= 0xfe;
    }
    if (MenuWork.inputDelay != 0)
        MenuWork.inputDelay--;

    switch (MenuWork.state) {
    case 0x10:
        if (MenuWork.inputDelay == 0) {
            signed char previousSelection = MenuWork.messageIndex;
            int selection = MenuSelectMove(previousSelection, 7, 0);
            MenuWork.messageIndex = selection;
            if (MenuWork.messageIndex == 5)
                MenuWork.messageIndex = selection + (selection - previousSelection);
            if (PadData.pressed & 0x20) {
                __builtin_memcpy(optionStates, optionStateTable, sizeof(optionStates));
                if (MenuWork.messageIndex == 4) {
                    xglSoundEffectNormalID(1, 0);
                    MenuModelMemorySet(0);
                    xglSleep();
                    tyaDisplaySetting(MenuModelWorkTop);
                } else if (MenuWork.messageIndex == 5 && MenuWork.hddMounted == 0) {
                    xglSoundEffectNormalID(5, 0);
                } else if (optionStates[MenuWork.messageIndex] != 0) {
                    MenuWork.nextState = optionStates[MenuWork.messageIndex];
                    MenuWork.inputDelay = 8;
                    xglSoundEffectNormalID(1, 0);
                }
            }
            if (PadData.pressed & 0x40) {
                MenuWork.nextState = 0xf0;
                xglSoundEffectNormalID(2, 0);
            }
        }
        break;
    case 0x20:
        if ((unsigned char)(MenuWork.flags & 1))
            MenuWork.choiceIndex = 0;
        if (MenuWork.inputDelay != 0)
            break;
        {
            MenuWork.choiceIndex = MenuSelectMove(MenuWork.choiceIndex, 2, 0);
            if (PadData.pressed & 0x20) {
                __builtin_memcpy(outputModes, outputModeTable, sizeof(outputModes));
                SsdSetOutputMode(outputModes[MenuWork.choiceIndex]);
                SaveData[0x28] = outputModes[MenuWork.choiceIndex];
                MenuWork.nextState = 0x10;
                MenuWork.inputDelay = 8;
                MenuWork.soundOutput = SaveData[0x28] ^ 1;
                xglSoundEffectNormalID(1, 0);
            }
            if (PadData.pressed & 0x40) {
                MenuWork.nextState = 0x10;
                MenuWork.inputDelay = 8;
                xglSoundEffectNormalID(2, 0);
            }
        }
        break;
    case 0x30:
        if ((unsigned char)(MenuWork.flags & 1))
            MenuWork.choiceIndex = 0;
        if (MenuWork.inputDelay != 0)
            break;
        {
            MenuWork.choiceIndex = MenuSelectMove(MenuWork.choiceIndex, 2, 0);
            if (PadData.pressed & 0x20) {
                MenuSystemPartyDataPrefix *party;
                __builtin_memcpy(displayModes, displayModeTable, sizeof(displayModes));
                PartyRadarDispSet(displayModes[MenuWork.choiceIndex]);
                party = (MenuSystemPartyDataPrefix *)PartyDataGet();
                MenuWork.displayMode = party->radar_disp ^ 1;
                MenuWork.nextState = 0x10;
                MenuWork.inputDelay = 8;
                xglSoundEffectNormalID(1, 0);
            }
            if (PadData.pressed & 0x40) {
                MenuWork.nextState = 0x10;
                MenuWork.inputDelay = 8;
                xglSoundEffectNormalID(2, 0);
            }
        }
        break;
    case 0x40:
        if ((unsigned char)(MenuWork.flags & 1))
            MenuWork.choiceIndex = 0;
        if (MenuWork.inputDelay != 0)
            break;
        {
            MenuWork.choiceIndex = MenuSelectMove(MenuWork.choiceIndex, 2, 0);
            if (PadData.pressed & 0x20) {
                __builtin_memcpy(vibrationModes, vibrationModeTable, sizeof(vibrationModes));
                PadData.vibrationReset = vibrationModes[MenuWork.choiceIndex];
                SaveData[0x3c] = vibrationModes[MenuWork.choiceIndex];
                PadData.vibrationStatus = vibrationModes[MenuWork.choiceIndex];
                MenuWork.radarMode = vibrationModes[MenuWork.choiceIndex];
                if (vibrationModes[MenuWork.choiceIndex] == 0)
                    MenuBibrationSet(0, 1, 128, 32);
                MenuWork.nextState = 0x10;
                MenuWork.inputDelay = 8;
                xglSoundEffectNormalID(1, 0);
            }
            if (PadData.pressed & 0x40) {
                MenuWork.nextState = 0x10;
                MenuWork.inputDelay = 8;
                xglSoundEffectNormalID(2, 0);
            }
        }
        break;
    case 0x50:
        if ((unsigned char)(MenuWork.flags & 1))
            MenuWork.choiceIndex = 0;
        if (MenuWork.inputDelay != 0)
            break;
        {
            MenuWork.choiceIndex = MenuSelectMove(MenuWork.choiceIndex, 2, 0);
            if (PadData.pressed & 0x20) {
                __builtin_memcpy(padModes, padModeTable, sizeof(padModes));
                SaveData[0x44] = padModes[MenuWork.choiceIndex];
                MenuWork.padMode = padModes[MenuWork.choiceIndex];
                MenuWork.nextState = 0x10;
                MenuWork.inputDelay = 8;
                xglSoundEffectNormalID(1, 0);
            }
            if (PadData.pressed & 0x40) {
                MenuWork.nextState = 0x10;
                MenuWork.inputDelay = 8;
                xglSoundEffectNormalID(2, 0);
            }
        }
        break;
    case 0x60:
        if ((unsigned char)(MenuWork.flags & 1))
            MenuWork.choiceIndex = 0;
        if (MenuWork.inputDelay != 0)
            break;
        {
            MenuWork.choiceIndex = MenuSelectMove(MenuWork.choiceIndex, 2, 0);
            if (PadData.pressed & 0x20) {
                if (MenuWork.choiceIndex == 0) {
                    MenuSystemInitSet();
                    MenuWork.nextState = 0x10;
                    MenuWork.inputDelay = 8;
                    xglSoundEffectNormalID(1, 0);
                } else {
                    MenuWork.nextState = 0x10;
                    MenuWork.inputDelay = 8;
                    xglSoundEffectNormalID(2, 0);
                }
            }
            if (PadData.pressed & 0x40) {
                MenuWork.nextState = 0x10;
                MenuWork.inputDelay = 8;
                xglSoundEffectNormalID(2, 0);
            }
        }
        break;
    case 0x70:
        if ((unsigned char)(MenuWork.flags & 1))
            MenuWork.choiceIndex = 0;
        if (MenuWork.inputDelay != 0)
            break;
        {
            MenuWork.choiceIndex = MenuSelectMove(MenuWork.choiceIndex, 2, 0);
            if (PadData.pressed & 0x20) {
                __builtin_memcpy(hddModes, hddModeTable, sizeof(hddModes));
                SaveData[0x40] = hddModes[MenuWork.choiceIndex] ^ 1;
                xglHddActivate(hddModes[MenuWork.choiceIndex]);
                MenuWork.hddActive = SaveData[0x40];
                MenuWork.nextState = 0x10;
                MenuWork.inputDelay = 8;
                xglSoundEffectNormalID(1, 0);
            }
            if (PadData.pressed & 0x40) {
                MenuWork.nextState = 0x10;
                MenuWork.inputDelay = 8;
                xglSoundEffectNormalID(2, 0);
            }
        }
        break;
    case 0xf0:
        if ((unsigned char)(MenuWork.flags & 1))
            MenuWork.inputDelay = 16;
        if (MenuWork.inputDelay == 0)
            MenuWork.nextState = 0xff;
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
    MenuSystemPartyDataPrefix *party;
    unsigned char *cursor = (unsigned char *)((workEnd + 127) & -128);

    MenuSystemPas = (MenuSystemPasBlock *)cursor;
    cursor += sizeof(*MenuSystemPas);
    MenuSystemPas->state = 0;
    MenuSystemInfo = (MenuSystemInfoBlock *)cursor;
    cursor += sizeof(*MenuSystemInfo);
    MenuSystemInfo->state = 0;
    MenuSystemMenu = (MenuSystemMenuBlock *)cursor;
    cursor += sizeof(*MenuSystemMenu);
    MenuSystemMenu->state = 0;
    MenuWork.nextState = 0x10;
    MenuWork.flags = 0;
    MenuWork.soundOutput = SaveData[40] ^ 1;
    MenuWork.inputDelay = 12;
    MenuWork.padMode = SaveData[68];
    MenuWork.radarMode = SaveData[60];
    party = (MenuSystemPartyDataPrefix *)PartyDataGet();
    MenuWork.displayMode = party->radar_disp ^ 1;
    MenuHddCheck = xglHddMount();
    if (MenuHddCheck == 1 && xglHddActivate(-1) != 0) {
        MenuWork.hddMounted = 1;
        MenuWork.hddActive = SaveData[64];
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
        case 0:
            subMenuSystemInit(MainMenuWorkEnd);
            MenuWork.menuState = 2;
            /* fall through */
        case 2:
            subMenuSystemMain();
            if (MenuWork.state != 0xFF)
                break;
            MenuWork.menuState = 4;
            /* fall through */
        case 4:
            ChangeTopLevel(0);
            break;
        } while (0);
    }
}

void MenuSystem2Init(void) {
    endPrintInit();
    subMenuSystemInit(MainMenuWorkEnd);
}

int MenuSystem2(void)
{
    subMenuSystemMain();
    endPrintExtFunc(0, 100, 0);
    return MenuWork.state != 0xFF;
}

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
MenuSystemOptionNames *const settingLabelNames[7] = { &msg01_2_0036DD20, &msg02_3_0036DD28, &msg02_3_0036DD28, &msg02_3_0036DD28, &msg02_3_0036DD28, &msg02_3_0036DD28, &msg02_3_0036DD28 };

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
