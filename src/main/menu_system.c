#include "common.h"

extern void subMenuSystemInit(int workEnd);

extern void subMenuSystemMain(void);

extern int MainMenuWorkEnd;

/* The original stores the HDD mount result through this four-byte object. */
extern int MenuHddCheck;

extern void ChangeTopLevel(int level);

#include "shared.h"

#include "main/party.h"

#include "main/xgl_hdd.h"

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
    unsigned char unmodeled_21[0x22 - 0x21];
    signed char hddMounted;
    unsigned char unmodeled_23[0x0d];
    unsigned char soundOutput;
    unsigned char padMode;
    unsigned char radarMode;
    unsigned char displayMode;
    unsigned char unmodeled_34;
    unsigned char hddActive;
} MenuSystemWork;

extern MenuSystemWork MenuWork;

typedef struct MenuSystemPad {
    unsigned char unmodeled_00[0x56];
    unsigned char vibrationReset;
    unsigned char unmodeled_57[0xbe - 0x57];
    unsigned char vibrationStatus;
} MenuSystemPad;

typedef struct MenuSystemPartyDataPrefix {
    unsigned char unmodeled_00[0x2e];
    unsigned char radar_disp;
} MenuSystemPartyDataPrefix;

extern MenuSystemPad PadData;

void SsdSetOutputMode(int mode);

void PartyRadarDispSet(int enabled);

void tyaDisplaySetting(int mode);

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

MenuSystemInfoBlock *MenuSystemInfo = 0;

XglClock _CountTime = {0};

const MenuSystemMessageStorage D_004C9480 = {
    .words = {0x004C9338, 0x004C9380, 0x004C93A8, 0x004C93D0,
              0x004C93F8, 0x004C9428, 0x004C9460, 0}
};

extern void MenuInfoWindow(void);

extern void WindowDXSet(void *window);

extern void WindowDXMain(void *window);

extern void eMessageCpy(char *message, char *text);

extern void MoveSlide(short *value, short *destination, float speed);

extern void *memset(void *destination, int value, unsigned int count);

enum {
    SAVE_DATA_BYTE_28 = 0x28,
    SAVE_DATA_BYTE_44 = 0x44,
    SAVE_DATA_BYTE_3C = 0x3c,
    SAVE_DATA_BYTE_40 = 0x40
};

extern void endPrintInit(void);

extern void endPrintExtFunc(int kind, int id, void *data);

/*
 * Drives the system-options screen for one frame and reports whether it is
 * still running: MenuWork.state is 0xFF once the screen driven by
 * subMenuSystemMain has finished.
 */

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

MenuSystemPasBlock *MenuSystemPas = 0;

extern const char D_004C9328[];

static const char *msg00_0_0036DCF8[] = {D_004C9328};

extern void eMessageSet(void *message, const char *text);

extern void eMessageMain(void *message);

#include "main/window_tex_load.h"

typedef struct MenuSystemWindow {
    short x;
    short y;
    int previousColor;
    short width;
    short height;
    const char *title;
    unsigned char transition;
    unsigned char unmodeled_11[3];
    void (*window)(void);
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
    MenuSystemWindow titleWindow;
    MenuSystemMessage label[8];
    MenuSystemMessage value[8];
    MenuSystemCursor labelCursor;
    MenuSystemWindow choiceWindow;
    MenuSystemMessage choiceLabel[3];
    MenuSystemCursor choiceCursor;
    MenuSystemWindow confirmWindow;
    MenuSystemChoice confirm;
} MenuSystemMenuBlock;

MenuSystemMenuBlock *MenuSystemMenu = 0;

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
        WindowDXSet(&panel->window);
        panel->window.x = -288;
        panel->window.color = panel->color;
        panel->window.y = 8;
        panel->window.width = 272;
        panel->window.height = 30;
        panel->window.transition = 1;
        WindowDXMain(&panel->window);
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
    WindowDXMain(&panel->window);
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
    MenuSystemMessageTable messageTable = D_004C9480.table;
    short targetY;

    if (info->state != 0) {
        if (info->state != 2)
            return;
    } else {
        info->color = 0xFFFFF0;
        WindowDXSet(&info->x);
        info->x = -16;
        info->y = 480;
        info->width = 544;
        info->height = 78;
        info->previousColor = info->color;
        info->subwindow = &info->cursorX;
        info->cursorX = 0;
        info->window = MenuInfoWindow;
        info->transition = 1;
        info->cursorY = 0;
        info->message = 0;
        WindowDXMain(&info->x);
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
    WindowDXMain(&info->x);
}

INCLUDE_ASM("asm/main/nonmatchings/menu_system", MenuSystemMenuMain);

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

INCLUDE_ASM("asm/main/nonmatchings/menu_system", subMenuSystemMain);

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
