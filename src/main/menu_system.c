#include "common.h"

extern void subMenuSystemInit(int workEnd);
extern void subMenuSystemMain(void);
extern int MainMenuWorkEnd;
extern void ChangeTopLevel(int level);

INCLUDE_ASM("asm/main/nonmatchings/menu_system", MenuSystemPasMain);

#include "shared.h"
#include "main/party.h"
#include "main/xgl_hdd.h"
/* Offsets named here are touched by MenuSystemInfoMain, MenuSystemInitSet,
 * and the existing MenuSystem2 completion poll. */
typedef struct MenuSystemWork {
    unsigned char unmodeled_00[3];
    unsigned char menuState;
    unsigned char unmodeled_04[0x11 - 4];
    unsigned char state;
    unsigned char unmodeled_12[0x20 - 0x12];
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
    unsigned char text[128];
} MenuSystemInfoBlock;
typedef struct MenuSystemMessageTable {
    int messages[7];
} MenuSystemMessageTable;
extern MenuSystemInfoBlock *MenuSystemInfo;
extern const MenuSystemMessageTable D_004C9480;
extern void MenuInfoWindow(void);
extern void WindowDXSet(void *window);
extern void WindowDXMain(void *window);
extern void eMessageCpy(unsigned char *message, int id);
extern void MoveSlide(short *value, short *destination, float speed);
extern void *memset(void *destination, int value, unsigned int count);

void MenuSystemInfoMain(void)
{
    MenuSystemInfoBlock *info = MenuSystemInfo;
    MenuSystemMessageTable messageTable = D_004C9480;
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

enum {
    SAVE_DATA_BYTE_28 = 0x28,
    SAVE_DATA_BYTE_44 = 0x44,
    SAVE_DATA_BYTE_3C = 0x3c,
    SAVE_DATA_BYTE_40 = 0x40
};

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

INCLUDE_ASM("asm/main/nonmatchings/menu_system", subMenuSystemInit);

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

extern void endPrintInit(void);
extern void subMenuSystemInit(int workEnd);
extern int MainMenuWorkEnd;

void MenuSystem2Init(void) {
    endPrintInit();
    subMenuSystemInit(MainMenuWorkEnd);
}

extern void subMenuSystemMain(void);
extern void endPrintExtFunc(int kind, int id, void *data);

/*
 * Drives the system-options screen for one frame and reports whether it is
 * still running: MenuWork.state is 0xFF once the screen driven by
 * subMenuSystemMain has finished.
 */
int MenuSystem2(void)
{
    subMenuSystemMain();
    endPrintExtFunc(0, 100, 0);
    return MenuWork.state != 0xFF;
}
