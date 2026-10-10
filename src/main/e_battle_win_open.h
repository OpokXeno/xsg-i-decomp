/*
 * TU-local declarations of main/tu166 (src/main/e_battle_win_open.c).
 */

#ifndef SRC_MAIN_E_BATTLE_WIN_OPEN_H
#define SRC_MAIN_E_BATTLE_WIN_OPEN_H

#include "shared.h"

/*
 * WindowDX is the sub-window control block WindowDXSet/WindowDXMain/
 * WindowDXFlagChange share, and the type every "*Main" screen task that
 * calls WindowDXSet embeds one of (TopStatusWinMain, the Char/Agws/Menu*Main
 * family, eBattleWinOpen/2/3/4, ...). Only the members WindowDXSet itself
 * writes are modelled here:
 *   +0x0C tag_id             -- borrowed title text passed as eTagFontSet's
 *                                second argument (`lw $5,0xC($16)` in the
 *                                delay slot of `jal eTagFontSet`, $4 =
 *                                window+0x80).
 *   +0x10 state               -- WindowDXFlagChange stores its flag argument
 *                                (0..4) here; WindowDXMain switches on it.
 *   +0x11 ribbon_initialized  -- WindowDXMain skips its eRibbonMain re-init
 *                                block once this is set.
 *   +0x14 close_callback      -- WindowDXMain loads it and, once non-null,
 *                                calls close_callback(window,
 *                                close_callback_arg).
 *   +0x18 close_callback_arg  -- the callback's second argument (`lw
 *                                $5,0x18($16)` in the jalr delay slot).
 *   +0x2C..0x2F color         -- set by WindowDXSet itself to a translucent
 *                                gray (0x80,0x80,0x80 RGB, 0x60 alpha).
 * Everything else is unmodeled until a function that reads or writes it is
 * recovered.
 */
typedef struct WindowDX WindowDX;

typedef struct WindowDXRect {
    short x;
    short y;
    int style;
    short width;
    short height;
} WindowDXRect;

typedef struct WindowDXFrame {
    unsigned char unmodeled_00[4];
    short x;
    short y;
    int style;
    short width;
    short height;
    signed char progress;
    unsigned char mode;
    unsigned char unmodeled_12[0x44 - 0x12];
} WindowDXFrame;

typedef struct WindowDXRibbonCell {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
    unsigned char unmodeled_04[8];
} WindowDXRibbonCell;

typedef struct WindowDXRibbon {
    short x;
    short y;
    int style;
    short width;
    short height;
    unsigned char unmodeled_0c[0x18 - 0x0c];
    WindowDXRibbonCell cells[6];
    unsigned char unmodeled_60[0x70 - 0x60];
} WindowDXRibbon;

typedef struct WindowDXSprite {
    unsigned char unmodeled_00[0x10];
    unsigned short kind;
    unsigned char unmodeled_12[0x14 - 0x12];
} WindowDXSprite;

typedef struct BattleWinTagFont {
    unsigned char unmodeled_00[4];
    short x;
    short y;
    int style;
    unsigned char color_r;
    unsigned char color_g;
    unsigned char color_b;
    unsigned char color_a;
    unsigned char unmodeled_10[0x20 - 0x10];
} BattleWinTagFont;

typedef struct BattleWinMessage {
    unsigned char unmodeled_00;
    unsigned char mode;
    unsigned char unmodeled_02[2];
    short x;
    short y;
    int extent;
    unsigned char unmodeled_0c[4];
    unsigned char color_r;
    unsigned char color_g;
    unsigned char color_b;
    unsigned char unmodeled_13[0x18 - 0x13];
    const char *text;
    unsigned char paged;
    unsigned char unmodeled_1d[0x44 - 0x1d];
} BattleWinMessage;

typedef struct BattleWinNumber {
    short x;
    short y;
    int style;
    unsigned char color_r;
    unsigned char color_g;
    unsigned char color_b;
    unsigned char unmodeled_0b;
    unsigned char font;
    unsigned char unmodeled_0d;
    unsigned char option;
    unsigned char places;
    unsigned char unmodeled_10[4];
    int value;
    unsigned char unmodeled_18[0x90 - 0x18];
} BattleWinNumber;

typedef struct BattleWinCursor {
    unsigned char mode;
    unsigned char shown;
    unsigned char unmodeled_02[2];
    short x;
    short y;
    int style;
    unsigned char unmodeled_0c[0x24 - 0x0c];
} BattleWinCursor;

typedef struct BattleWinCommand {
    const char *text;
    const char *description;
    unsigned char flags;
    signed char icon;
    unsigned char unmodeled_0a[2];
} BattleWinCommand;

typedef struct BattleWinItem {
    const char *text;
    int icon;
    signed char flags;
    unsigned char unmodeled_09[3];
} BattleWinItem;

struct WindowDX {
    short x;                           /* +0x00 */
    short y;                           /* +0x02 */
    int style;                         /* +0x04 */
    short width;                       /* +0x08 */
    short height;                      /* +0x0A */
    const char *tag_id;                 /* +0x0C: title text */
    unsigned char state;               /* +0x10 */
    unsigned char ribbon_initialized;  /* +0x11 */
    unsigned char unmodeled_12[2];
    void (*close_callback)(WindowDX *window, void *arg); /* +0x14 */
    void *close_callback_arg;          /* +0x18 */
    unsigned char unmodeled_1c[4];
    WindowDXRect rect;                 /* +0x20 */
    unsigned char color_r;             /* +0x2C */
    unsigned char color_g;             /* +0x2D */
    unsigned char color_b;             /* +0x2E */
    unsigned char alpha;               /* +0x2F */
    unsigned char unmodeled_30[0x3C - 0x30];
    WindowDXFrame frame;               /* +0x3C */
    BattleWinTagFont title_font;        /* +0x80 */
    WindowDXRibbon ribbon_top;          /* +0xA0 */
    WindowDXRibbon ribbon_bottom;       /* +0x110 */
    WindowDXSprite sprite;              /* +0x180 */
};

void WindowDXSet(WindowDX *window);

typedef struct WindowSPCursor {
    unsigned char state;
    unsigned char draw_flags;
    unsigned char anim;
    unsigned char unmodeled_03;
    short x;
    short y;
    int style;
    short width;
    short height;
    const char *title;
    signed char columns;
    signed char rows;
    unsigned char unmodeled_16[2];
    int buttons;
    BattleWinItem *items;
    signed char visible_top;
    signed char visible_bottom;
    signed char visible_cols;
    signed char visible_rows;
    signed char current_row;
    signed char max_scroll;
    signed char mode;
    unsigned char unmodeled_27;
    short selected_index;
    short item_count;
    WindowDX window;
    BattleWinCursor cursors[3];
    BattleWinMessage messages[24];
    BattleWinNumber numbers[26];
    WindowDXRect rect;
} WindowSPCursor;

typedef unsigned char WindowSPKeepBuffer[5];

typedef struct BattleWinPad {
    unsigned char unmodeled_00[0x2A];
    unsigned short half_2a;
    unsigned char unmodeled_2c[6];
    unsigned short half_32;
    unsigned short half_34;
} BattleWinPad;

extern BattleWinPad PadData;

typedef struct BattleWinTask {
    unsigned char unmodeled_00[0x1C];
    int buttons;
    int selection;
} BattleWinTask;

void WindowDXMain(WindowDX *window);
int WindowSPSelect(WindowSPCursor *cursor, int button_flags);
void WindowSPSet(WindowSPCursor *list);
void WindowSPItemChange(WindowSPCursor *list);
void WindowSPSelectJump(WindowSPCursor *list, int index);
void WindowSPMain(WindowSPCursor *list);
void WindowSPSetSelect(WindowSPCursor *list, unsigned char *keep);
void subMWModeExSet(WindowSPCursor *list);
void subMWPosSet(WindowSPCursor *list);
void subMWDraw(WindowSPCursor *list);
void subMWControlType00(WindowSPCursor *list);
void subMWControlType01(WindowSPCursor *list);
void endPrintExtFunc(int color, int id, void *data);
void eTagFontMain(void *tag);
void eNumberMain(void *number);
void eMessageSet(void *message, const char *text);
void eMessageTextChange(void *message, const char *text);
void eMessageMain(void *message);
int eMessageNextPage(void *message, int reset_page);
void OpenCloseMain(void *controller);
void endSpriteSet(void *sprite, int mode);
void eTagFontSet(void *tag, const char *text);
void eRibbonSet(void *ribbon, int kind);
void eRibbonMain(void *ribbon);
void eNumberSet(void *number, int mode);
void eCursolSet(void *cursor, int index);
void eCursolMain(void *cursor);
void eCursolModeChange(void *cursor, int mode);
void xglSoundEffectNormalID(int id, int volume);

/*
 * BW is the work area eBattleWinOpen/eBattleWinMain/eBattleWinClose share
 * for the item/ether "get" screens: menuCloseEth and menuCloseItm
 * (src/ov01/menu.c) each call eBattleWinClose on their own screen object,
 * but eBattleWinClose ignores that argument and always clears BW itself
 * ($a0 is overwritten before it is ever read), which is why the parameter
 * below is unused. eBattleWinOpen embeds a WindowDX at +0x1740
 * (`addiu $4,$4,0x1740` right before `jal WindowDXSet`, with $4 = BW), so
 * window.state lands at BW+0x1750.
 */
typedef struct BattleWindow BattleWindow;
struct BattleWindow {
    unsigned char unmodeled_00;
    unsigned char mode;              /* +0x0001 */
    unsigned char unmodeled_02[2];
    BattleWinCommand *commands;      /* +0x0004 */
    WindowSPCursor list;             /* +0x0008 */
    WindowDX window;                 /* +0x1740 */
    BattleWinMessage message;        /* +0x18D4 */
};

extern BattleWindow *BW;

void eBattleWinClose(void *window);

/*
 * BW2 is BW's counterpart for the second "get" screen: eBattleWinOpen2
 * embeds a WindowDX at +0x8 (`addiu $4,$2,0x8` with $2 = BW2 right before
 * `jal WindowDXSet`), so window.state lands at BW2+0x18.
 */
typedef struct BattleWindow2 BattleWindow2;
struct BattleWindow2 {
    int active;                        /* +0x00 */
    int text_offset;                   /* +0x04 */
    WindowDX window;                   /* +0x0008 */
    BattleWinMessage message;          /* +0x019C */
};

extern BattleWindow2 *BW2;

void eBattleWinClose2(void);

/*
 * BW3 is BW's counterpart for the third "get" screen: eBattleWinOpen3
 * embeds a WindowDX at +0x4 (`addiu $4,$3,0x4` with $3 = BW3 right before
 * `jal WindowDXSet`), so window.state lands at BW3+0x14. secondary_state is
 * set to the same value as window.state whenever eBattleWinClose3 runs, but
 * nothing in this allocation reads it back.
 */
typedef struct BattleWindow3 BattleWindow3;
struct BattleWindow3 {
    int active;                    /* +0x0000 */
    WindowDX window;               /* +0x0004 */
    BattleWinMessage messages[3];  /* +0x0198 */
    BattleWinTagFont tag_font;     /* +0x0264 */
    BattleWinNumber number;        /* +0x0284 */
    WindowDX window2;              /* +0x0314 */
    BattleWinTagFont tag_fonts[5]; /* +0x04A8 */
    BattleWinNumber numbers[5];    /* +0x0548 */
    BattleWinMessage messages2[4]; /* +0x0818 */
};

extern BattleWindow3 *BW3;

void eBattleWinClose3(void);

/*
 * BW4 is BW's counterpart for the fourth "get" screen: eBattleWinOpen4
 * embeds a WindowDX at +0x8 (`addiu $4,$2,0x8` with $2 = BW4 right before
 * `jal WindowDXSet`), so window.state lands at BW4+0x18. page is the word
 * eBattleWinPageCheck4 returns.
 */
typedef struct BattleWindow4 BattleWindow4;
struct BattleWindow4 {
    int active;               /* +0x0000 */
    int page;                 /* +0x0004 */
    WindowDX window;          /* +0x0008 */
    BattleWinMessage message; /* +0x019C */
};

extern BattleWindow4 *BW4;

void eBattleWinClose4(void);

int eBattleWinPageCheck4(void);

void *eBattleWinInit2(void *arg);

/* Not yet recovered (src/main/window_tex_load.c); matches the prototype
 * already used at its other call sites (src/main/party.c). */
void *MenuWorkEndGet(void);

void eBattleWinInit(void);

/*
 * WindowSPCursor is the SP (scrollable list) cursor record every WindowSP*
 * function of this TU shares (WindowSPSelect, WindowSPSet,
 * WindowSPItemChange, WindowSPSelectJump, WindowSPSetSelect -- all still
 * assembler), and that WindowSPKeepSelect/WindowSPKeepSelectCheck save and
 * clear on behalf of the seven MenuXxx screens (MenuItem, MenuCharactor,
 * MenuAgws, MenuShopCore, MenuEther, MenuTec, MenuSkill -- outside this TU,
 * not yet recovered) that call them. Only the members this allocation and
 * its siblings touch are modeled:
 *   +0x20 visible_top     WindowSPItemChange (0x00283cf0) clamps this
 *                          against +0x22 (0x00283e18..0x00283e34), the shape
 *                          of a scrolled list's first visible row.
 *   +0x21 visible_bottom  the same function's symmetric clamp against +0x23
 *                          (0x00283e64..0x00283e80).
 *   +0x24 current_row     set from a row WindowSPItemChange computes by
 *                          dividing item_count by the row/column grid at
 *                          +0x14/+0x15 (0x00283d94..0x00283da8).
 *   +0x28 selected_index  a signed halfword; WindowSPSet (0x00283b88) resets
 *                          it to -1 ("nothing selected") and
 *                          WindowSPItemChange advances it while it stays
 *                          below item_count.
 *   +0x2A item_count      a halfword; WindowSPSet walks a linked list from
 *                          +0x1C to count it (0x00283bc4..0x00283bf4) and
 *                          stores the result here.
 * Everything else is unmodeled until a function that reads or writes it is
 * recovered.
 */
/*
 * A saved snapshot of one WindowSPCursor: item_count, selected_index,
 * visible_top, visible_bottom and current_row packed into 5 bytes in that
 * order (see WindowSPKeepSelect). WindowSPKeepSelectCheck clears it when
 * MenuCursorKeepCheck (main/menu_1.c, still assembler) reports nothing to
 * restore.
 */
typedef unsigned char WindowSPKeepBuffer[5];

#endif /* SRC_MAIN_E_BATTLE_WIN_OPEN_H */
