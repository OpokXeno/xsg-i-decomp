#ifndef SRC_MAIN_MENU_TOP_COMMAND_CHECK_H
#define SRC_MAIN_MENU_TOP_COMMAND_CHECK_H

#include "main/e_battle_win_open.h"
#include "shared.h"
#include "main/party.h"
#include "main/data.h"
#include "main/e_number_main.h"
#include "main/end_print.h"
#include "main/menu_1.h"
#include "main/menu_model.h"
#include "main/menu_top_command_check.h"
#include "main/window_tex_load.h"
#include "main/xgl_dma.h"
#include "main/xgl_font.h"
#include "main/xgl_sound.h"

typedef struct WindowDX WindowDX;

/* Partial menu records retain the offsets used by the original consumers. */
typedef struct MenuTopWork {
    unsigned char unmodeled_00;
    unsigned char command;
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

typedef struct MenuTopESprite {
    unsigned char unmodeled_00[4];
    short x;
    short y;
    int color;
    unsigned char unmodeled_0c[0x1C];
} MenuTopESprite;


/*
 * Partial view of XglClock (include/shared.h): the play time read as one
 * packed word. The full type would need a union in shared.h, which this
 * TU does not own, so this view stays here.
 */
typedef struct XglClockPackedView {
    unsigned int packed;
    unsigned int unmodeled_04;
} XglClockPackedView;

typedef struct MenuInfoScrollSprite {
    short x;
    short y;
    int color;
    unsigned char unmodeled_08[4];
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
    short image;
    unsigned char unmodeled_12[2];
} MenuInfoScrollSprite;

typedef struct MenuInfoBox {
    short x;
    short y;
    int color;
    short width;
    short height;
} MenuInfoBox;

typedef struct MenuInfoVertex {
    int x;
    int y;
    int color;
    unsigned char unmodeled_0c[4];
} MenuInfoVertex;

typedef struct MenuInfoColor {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} MenuInfoColor;

typedef struct MenuInfoQuad {
    unsigned char mode;
    unsigned char flag;
    short image;
    unsigned char unmodeled_04[12];
    MenuInfoVertex vertex[4];
    MenuInfoColor rgba[4];
} MenuInfoQuad;

typedef struct MenuTopInfoPanel {
    short x;
    short y;
    signed char flag;
    unsigned char page;
    unsigned char lineCount;
    unsigned char blink;
    int inputLock;
    const char *text;
    EMessageParam message;
    MenuInfoScrollSprite scroll;
    MenuInfoBox box;
    MenuInfoBox clip;
    int texture;
    unsigned char unmodeled_84[12];
    MenuInfoQuad footer;
    MenuInfoQuad header;
    MenuInfoQuad border;
} MenuTopInfoPanel;

typedef union MenuFormationPositions {
    Vector4 vector[6];
    unsigned long long words[12];
} MenuFormationPositions;

typedef struct MenuModelTransform {
    Vector4 rotation;
    AlignedVector4 position;
    Vector4 scale;
    AlignedVector4 target;
    Matrix4 place;
} MenuModelTransform;

typedef struct MenuTaiTask {
    XglTaskPrefix link;
    unsigned char state;
    unsigned char enabled;
    unsigned char initialized;
    unsigned char index;
    short previousCommand;
    unsigned char unmodeled_16[2];
    MenuModelTransform *model;
} MenuTaiTask;

typedef struct MenuModelProjectedPoint {
    int x;
    int y;
    int depth;
    unsigned char unmodeled_0c[4];
} MenuModelProjectedPoint;

typedef struct MenuModelLinePoint {
    MenuModelProjectedPoint point;
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
    unsigned char unmodeled_14[12];
} MenuModelLinePoint;

typedef union MenuLightingSources {
    struct {
        float ambient[4];
        float intensity[3][4];
        float direction[3][4];
    } value;
    unsigned long long words[14];
} MenuLightingSources;

typedef struct MenuTopPanel {
    unsigned char member;
    unsigned char enabled;
    unsigned char unmodeled_02[2];
    short x;
    short y;
    unsigned char unmodeled_08[0x840 - 8];
} MenuTopPanel;

typedef struct MenuTopInfo {
    unsigned char state;
    unsigned char enabled;
    signed char slide;
    unsigned char unmodeled_03;
    WindowDX window;
    unsigned char unmodeled_198[8];
    MenuTopInfoPanel panel;
} MenuTopInfo;


typedef struct MenuTopMenuWin {
    int color;
    unsigned char state;
    unsigned char enabled;
    unsigned char unmodeled_06[2];
    WindowDX window;
    MenuTopMenuPanel panel;
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
    unsigned char unmodeled_10[0xC];
    ERibbonMainData ribbon[3];
    ETagFontMainData tag[3];
    MenuTopESprite sprite[6];
    ENumberMainData number[2];
    XglClockPackedView time;
} MenuTopFaceEx;

typedef struct MenuTopStatusWin {
    unsigned char state;
    unsigned char unmodeled_01[2];
    unsigned char count;
    int color;
    WindowDX window[3];
    MenuStatusPanel panel[3];
} MenuTopStatusWin;

extern MenuTopWork MenuWork;
extern char topMenuEmptyText[];
extern const char *msg_1_0036C360[3];
extern MenuTopStatusWin *TopStatusWin;
extern MenuTopInfo *TopInfo;
extern MenuTopMenuWin *TopMenuWin;
extern MenuTopFaceEx *TopFaceExWin;
extern unsigned char *MainMenuWorkEnd;
extern int MenuScenarioNo;
extern int debug_flag;
extern void *MenuModelOut[];
extern char topMenuDebugLabel[];
extern char topMenuTitleText[];
extern unsigned char MenuTopCommand[10];
extern const char *text[12];
extern MenuTopCommandEntry command_0[11];

extern void MenuModelCreate(void **slot, int partyId);
extern void xglFontDebugHex(int x, int y, int value, int digits);
extern int PartyAttackerCheck(int partyId);
extern XglClockPackedView *PartyTimeUpDate(void);
extern void PartyTimeDispChange(XglClockPackedView *time);
extern void eSpriteSet(MenuTopESprite *sprite, short image);
extern void eSpriteMain(MenuTopESprite *sprite);
MenuStatusPara *func_00A11108(int chrNo, int *attack, int *defense);
MenuStatusExp *func_A19210(int chrNo);
char *MenuCharNameGet(int chrNo);
void endSpriteSet(void *sprite, int mode);
void TopStatusWinMain(void);
void TopInfoMain(void);
void TopMenuWinMain(void);
void TopFaceExWinMain(void);
int MenuTopCommandCheck(int commandId);
int MenuCursorKeepCheck(void);
#endif
