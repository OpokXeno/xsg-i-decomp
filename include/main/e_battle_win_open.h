#ifndef INCLUDE_MAIN_E_BATTLE_WIN_OPEN_H
#define INCLUDE_MAIN_E_BATTLE_WIN_OPEN_H

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
    struct BattleWinItem *items;
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

extern void eBattleWinClose2(void);
extern int eBattleWinPageCheck4(void);
extern void eBattleWinClose4(void);
void WindowDXSet(WindowDX *window);
void WindowDXMain(WindowDX *window);
int WindowSPSelect(WindowSPCursor *cursor, int button_flags);
void WindowSPItemChange(WindowSPCursor *list);
void WindowSPSetSelect(WindowSPCursor *list, unsigned char *keep);
void eBattleWinClose(void *window);
void eBattleWinClose3(void);
void eBattleWinInit(void);
void WindowSPKeepSelect(WindowSPCursor *cursor, WindowSPKeepBuffer keep);
void WindowSPKeepSelectCheck(WindowSPKeepBuffer keep);

#endif /* INCLUDE_MAIN_E_BATTLE_WIN_OPEN_H */
