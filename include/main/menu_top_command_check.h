#ifndef INCLUDE_MAIN_MENU_TOP_COMMAND_CHECK_H
#define INCLUDE_MAIN_MENU_TOP_COMMAND_CHECK_H

#include "main/e_battle_win_open.h"
#include "main/party.h"

typedef struct MenuStatusPanelWindow MenuStatusPanelWindow;

typedef struct MenuTopInfoPanel MenuTopInfoPanel;

/* Number formats 4, 6 and 13 carry the current and maximum as two halfwords;
 * other formats use the same word as a scalar value. */
typedef union MenuStatusValuePair {
    short halfwords[2];
    int scalar;
} MenuStatusValuePair;

typedef struct MenuStatusNumber {
    short x;
    short y;
    int color;
    unsigned char unmodeled_08[0x0E - 0x08];
    unsigned char format;       /* +0x0E */
    unsigned char digits;       /* +0x0F */
    unsigned char flag[4];      /* +0x10 */
    MenuStatusValuePair value;   /* +0x14 */
    unsigned char unmodeled_18[0x90 - 0x18];
} MenuStatusNumber;

typedef struct MenuStatusMessage {
    unsigned char unmodeled_00;
    unsigned char mode;         /* +0x01 */
    unsigned char unmodeled_02[2];
    short x;
    short y;
    int color;
    unsigned char unmodeled_0c[0x18 - 0x0C];
    char *text;                 /* +0x18 */
    unsigned char unmodeled_1c[0x44 - 0x1C];
} MenuStatusMessage;

typedef struct MenuStatusCorner {
    short x;
    short y;
    int color;
} MenuStatusCorner;

typedef struct MenuStatusLabel {
    short x;
    short y;
    int color;
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
    char *text;
} MenuStatusLabel;

typedef struct MenuStatusBox {
    short x;
    short y;
    int color;
    short width;
    short height;
} MenuStatusBox;

typedef struct MenuStatusSprite {
    short x;
    short y;
    int color;
    unsigned char unmodeled_08[0x10 - 0x08];
    short spriteId;             /* +0x10 */
    unsigned char unmodeled_12[2];
} MenuStatusSprite;

typedef struct MenuStatusTag {
    unsigned char unmodeled_00[4];
    int color;
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
    int value;
    short type;
    short mode;
} MenuStatusTag;

typedef struct MenuTopCommandEntry {
    const char *name;
    int unmodeled_04;
    signed char locked;
    unsigned char unmodeled_09[3];
} MenuTopCommandEntry;

typedef struct MenuStatusVertex {
    short x;
    short y;
    int color;
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} MenuStatusVertex;

typedef struct MenuStatusQuad {
    MenuStatusVertex vertex[4];
    unsigned char unmodeled_30[4];
    int flags;                  /* +0x34 */
    unsigned char unmodeled_38[4];
} MenuStatusQuad;

typedef struct MenuStatusLine {
    MenuStatusVertex vertex[2];
    unsigned char unmodeled_18[4];
    int flags;                  /* +0x1C */
    unsigned char unmodeled_20[4];
} MenuStatusLine;

typedef struct MenuSelectCursor {
    unsigned char mode;
    unsigned char unmodeled_01[3];
    short x;
    short y;
    int color;
    unsigned char unmodeled_0c[0x24 - 0x0C];
} MenuSelectCursor;

typedef struct MenuSelectMessage {
    unsigned char unmodeled_00;
    unsigned char mode;
    unsigned char unmodeled_02[2];
    short x;
    short y;
    int color;
    unsigned char unmodeled_0c[4];
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char unmodeled_13[5];
    const char *text;
    unsigned char unmodeled_1c[0x44 - 0x1C];
} MenuSelectMessage;

typedef struct MenuTopMenuPanel {
    unsigned char unmodeled_00[2];
    unsigned short rows;
    int cursor;
    const char *text;
    const char *heading;
    struct MenuTopCommandEntry *commands;
    unsigned char unmodeled_14[0x1A8 - 0x14];
    MenuSelectCursor marker;
    MenuSelectMessage message[12];
    MenuStatusLine headingLine;
    MenuStatusQuad cursorBand;
    MenuStatusQuad cursorShade;
    MenuStatusQuad cursorFill;
    MenuStatusLine divider;
} MenuTopMenuPanel;

typedef struct MenuStatusRibbon {
    short x;
    short y;
    int color;
    short width;
    short height;
    unsigned char unmodeled_0c[4];
    MenuStatusVertex vertex[6];
} MenuStatusRibbon;

typedef struct MenuStatusPanel {
    signed char chrNo;
    unsigned char enabled;
    short guestChrNo;
    short offsetX;
    short offsetY;
    unsigned char unmodeled_08[4];
    unsigned short maxHp;           /* +0x0C: with the pending point spend */
    unsigned char hpMark;           /* +0x0E: 2 marks the row with the ribbon */
    unsigned char unmodeled_0f;
    unsigned short maxEp;           /* +0x10: with the pending point spend */
    unsigned char epMark;           /* +0x12 */
    unsigned char unmodeled_13;
    MenuStatusNumber number[6];     /* +0x14 */
    MenuStatusMessage name;         /* +0x374 */
    MenuStatusCorner shadeCorner[4]; /* +0x3B8 */
    unsigned char shadeRgba[4][4];  /* +0x3D8 */
    int shadeTexture;               /* +0x3E8 */
    MenuStatusLabel label[6];       /* +0x3EC */
    MenuStatusBox box;              /* +0x44C */
    unsigned char unmodeled_458[0x46C - 0x458];
    MenuStatusSprite leader;        /* +0x46C */
    unsigned char unmodeled_480[0x4F8 - 0x480];
    MenuStatusSprite face;          /* +0x4F8 */
    unsigned char unmodeled_50c[0x548 - 0x50C];
    MenuStatusTag tag[8];           /* +0x548 */
    unsigned char unmodeled_5e8[0x638 - 0x5E8];
    MenuStatusQuad nameShade;       /* +0x638 */
    MenuStatusQuad headerBand;      /* +0x674 */
    MenuStatusQuad statsBand;       /* +0x6B0 */
    MenuStatusQuad headerFill;      /* +0x6EC */
    MenuStatusQuad statsFill;       /* +0x728 */
    MenuStatusLine headerLine;      /* +0x764 */
    MenuStatusLine statsLine;       /* +0x788 */
    MenuStatusLine divider;         /* +0x7AC */
    MenuStatusRibbon ribbon;        /* +0x7D0 */
    unsigned char unmodeled_828[0x840 - 0x828];
} MenuStatusPanel;

int MenuSelectWindow(MenuStatusPanelWindow *window, MenuTopMenuPanel *panel);
int MenuInfoWindow(MenuStatusPanelWindow *window, MenuTopInfoPanel *panel);
void MenuPasWindow(void);
void MenuStatusDisp(MenuStatusPanelWindow *window, MenuStatusPanel *work);

#endif /* INCLUDE_MAIN_MENU_TOP_COMMAND_CHECK_H */
