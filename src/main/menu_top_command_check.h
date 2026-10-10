#ifndef SRC_MAIN_MENU_TOP_COMMAND_CHECK_H
#define SRC_MAIN_MENU_TOP_COMMAND_CHECK_H

#include "shared.h"
#include "main/party.h"

/* Partial menu records retain the offsets used by the original consumers. */
typedef struct MenuTopWork {
    unsigned char unmodeled_00[2];
    signed char countdown;
    unsigned char state;
    unsigned char unmodeled_04;
    signed char topCursor;
    unsigned char unmodeled_06[0x80 - 6];
} MenuTopWork;

typedef struct MenuStatusPanelWindow {
    short x;
    short y;
    int color;
    short width;
    short height;
    unsigned char unmodeled_0c[0x10 - 0x0C];
    signed char state;
} MenuStatusPanelWindow;

typedef struct MenuStatusPara {
    short maxHp;                /* +0x00 */
    short maxEp;                /* +0x02 */
    unsigned char unmodeled_04[0x14 - 0x04];
    signed char level;          /* +0x14 */
    unsigned char unmodeled_15[0x34 - 0x15];
    short hp;                   /* +0x34 */
    short ep;                   /* +0x36 */
    unsigned char unmodeled_38[0x3C - 0x38];
    signed char boost;          /* +0x3C: the boost stock calcBoostChk and calcBp read */
} MenuStatusPara;

typedef struct MenuStatusExp {
    int exp;                    /* +0x00 */
    int nextExp;                /* +0x04 */
} MenuStatusExp;

typedef struct MenuStatusTagInit {
    signed char type;
    signed char mode;
    int value;
} MenuStatusTagInit;

typedef struct MenuTopWindow {
    short x;
    short y;
    int color;
    short width;
    short height;
    char *title;
    unsigned char state;
    unsigned char enabled;
    unsigned char unmodeled_12[2];
    void (*draw)(void);
    void *panel;
    unsigned char unmodeled_1c[0x194 - 0x1C];
} MenuTopWindow;

typedef struct MenuTopERibbon {
    short x;
    short y;
    int color;
    short width;
    short height;
    unsigned char unmodeled_0c[0x64 - 0xC];
} MenuTopERibbon;

typedef struct MenuTopETagFont {
    unsigned char unmodeled_00[4];
    short x;
    short y;
    int color;
    unsigned char unmodeled_0c[0x14];
} MenuTopETagFont;

typedef struct MenuTopESprite {
    unsigned char unmodeled_00[4];
    short x;
    short y;
    int color;
    unsigned char unmodeled_0c[0x1C];
} MenuTopESprite;

typedef struct MenuTopENumber {
    short x;
    short y;
    int color;
    unsigned char unmodeled_08[6];
    unsigned char digits;
    unsigned char width;
    unsigned char unmodeled_10[4];
    int value;
    unsigned char unmodeled_18[0x78];
} MenuTopENumber;

typedef struct MenuTopPlayTime {
    unsigned int packed;
    unsigned int unmodeled_04;
} MenuTopPlayTime;

typedef struct MenuTopInfoPanel {
    short x;
    short y;
    signed char flag;
    unsigned char unmodeled_05[7];
    const char *text;
} MenuTopInfoPanel;

typedef struct MenuTopPartySlot {
    unsigned short id;
    unsigned char unmodeled_02[2];
} MenuTopPartySlot;

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

typedef struct MenuTopPanel {
    unsigned char member;
    unsigned char enabled;
    unsigned char unmodeled_02[2];
    short x;
    short y;
    unsigned char unmodeled_08[0x840 - 8];
} MenuTopPanel;

typedef struct MenuTopCommandEntry {
    const char *name;
    int unmodeled_04;
    unsigned char locked;
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

typedef struct MenuTopInfo {
    unsigned char state;
    unsigned char enabled;
    signed char slide;
    unsigned char unmodeled_03;
    MenuTopWindow window;
    unsigned char unmodeled_198[8];
    MenuTopInfoPanel panel;
    unsigned char unmodeled_1b0[0x350 - 0x1B0];
} MenuTopInfo;

typedef struct MenuTopPartyData {
    unsigned char unmodeled_00[0x2A];
    unsigned short takeAgwsMask;
    unsigned char unmodeled_2c[4];
    MenuTopPartySlot slot[3];
} MenuTopPartyData;

typedef struct MenuTopStatusWin {
    unsigned char state;
    unsigned char unmodeled_01[2];
    unsigned char count;
    int color;
    MenuTopWindow window[3];
    MenuTopPanel panel[3];
} MenuTopStatusWin;

typedef struct MenuTopMenuPanel {
    unsigned char unmodeled_00[2];
    short rows;
    int cursor;
    unsigned char unmodeled_08[8];
    MenuTopCommandEntry *commands;
} MenuTopMenuPanel;

typedef struct MenuTopFaceRibbon {
    unsigned char unmodeled_00[0xC];
    MenuTopERibbon ribbon;
} MenuTopFaceRibbon;

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

typedef struct MenuStatusRibbon {
    short x;
    short y;
    int color;
    short width;
    short height;
    unsigned char unmodeled_0c[4];
    MenuStatusVertex vertex[6];
} MenuStatusRibbon;

typedef struct MenuTopMenuWin {
    int color;
    unsigned char state;
    unsigned char enabled;
    unsigned char unmodeled_06[2];
    MenuTopWindow window;
    MenuTopMenuPanel panel;
    unsigned char unmodeled_1b0[0x794 - 0x1B0];
} MenuTopMenuWin;

typedef struct MenuTopFaceEx {
    unsigned char state;
    unsigned char enabled;
    unsigned char first;
    unsigned char unmodeled_03;
    int color;
    unsigned char count;
    unsigned char unmodeled_09;
    unsigned short ids[3];
    MenuTopFaceRibbon ribbon[3];
    unsigned char unmodeled_160[0xC];
    MenuTopETagFont tag[3];
    MenuTopESprite sprite[6];
    MenuTopENumber number[2];
    MenuTopPlayTime time;
} MenuTopFaceEx;

typedef struct MenuStatusPanel {
    signed char chrNo;
    unsigned char unmodeled_01;
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
} MenuStatusPanel;

extern MenuTopWork MenuWork;
extern char D_004DA890[];
extern const char *msg_1_0036C360[3];
extern MenuTopStatusWin *TopStatusWin;
extern MenuTopInfo *TopInfo;
extern MenuTopMenuWin *TopMenuWin;
extern MenuTopFaceEx *TopFaceExWin;
extern unsigned char *MainMenuWorkEnd;
extern int MenuScenarioNo;
extern int debug_flag;
extern int MenuModelOut[];
extern char D_004DA8D0[];
extern PadPrefix PadData;
extern void MenuModelMemorySet(int size);
extern void MenuModelMenuMotionLoad(void);
extern void MenuModelCreate(int *slot, int partyId);
extern signed char MenuSelectMove(signed char cursor, int count, int wrap);
extern void ChangeTopLevel(int command);
extern void xglSoundEffectNormalID(int effect, int mode);
extern void xglFontDebugPrintf(int x, int y, const char *format, ...);
extern void xglFontDebugHex(int x, int y, int value, int digits);
extern int PartyFriendLockCheck(int partyId, int mode);
extern int PartyAttackerCheck(int partyId);
extern MenuTopPlayTime *PartyTimeUpDate(void);
extern void PartyTimeDispChange(MenuTopPlayTime *time);
extern int dataMoneyBoxChk(void);
extern int MenuFaceEpidGet(int partyId, int mode);
extern void eSpriteSet(MenuTopESprite *sprite, short image);
extern void eSpriteMain(MenuTopESprite *sprite);
extern void eTagFontSet(MenuTopETagFont *tag, const char *text);
extern void eTagFontMain(MenuTopETagFont *tag);
extern char D_004DA8B8[];
extern void MenuSelectWindow(void);
extern void MenuInfoWindow(void);
extern unsigned char *PartyDataGet(void);
extern void MoveSlide(short *current, short *target, float rate);
extern void WindowDXSet(MenuTopWindow *window);
extern void WindowDXMain(MenuTopWindow *window);
MenuStatusPara *func_00A11108(int chrNo, int *attack, int *defense);
MenuStatusExp *func_A19210(int chrNo);
char *MenuCharNameGet(int chrNo);
int PartyLeaderCheck(int chrNo);
void eMessageSet(void *message, char *text);
void eMessageDraw(void *message);
void endSpriteSet(void *sprite, int mode);
void endPrintExtFunc(int work, int type, void *data);
extern unsigned char MenuTopCommand[10];
void TopStatusWinMain(void);
void TopInfoMain(void);
void TopMenuWinMain(void);
void TopFaceExWinMain(void);
extern const char *text[12];

extern MenuTopCommandEntry command_0[11];
int MenuTopCommandCheck(int command_id);
int MenuCursorKeepCheck(void);
void eRibbonSet(void *ribbon, int mode);
void eRibbonMain(void *ribbon);
void eNumberSet(void *number, int value);
void eNumberMain(void *number);
#endif
