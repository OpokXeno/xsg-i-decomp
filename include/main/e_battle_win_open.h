#ifndef INCLUDE_MAIN_E_BATTLE_WIN_OPEN_H
#define INCLUDE_MAIN_E_BATTLE_WIN_OPEN_H

#include "main/e_message.h"
#include "main/e_number_main.h"
#include "main/open_close_main.h"

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

/* WindowDX.state values (WindowDXFlagChange argument; WindowDXMain switches
 * on the low nibble). */
#define WINDOWDX_STATE_CLOSED 0
#define WINDOWDX_STATE_OPEN 1    /* request: initialise and open */
#define WINDOWDX_STATE_OPENING 2 /* frame animation running */
#define WINDOWDX_STATE_READY 3   /* fully open */
#define WINDOWDX_STATE_CLOSE 4   /* request: start closing (only from READY) */
#define WINDOWDX_STATE_CLOSING 5
#define WINDOWDX_STATE_FINISHED (-1) /* closing finished */

/* WindowSPCursor.state: the low nibble is the phase; for the OPEN phase the
 * high nibble selects fresh (0) or skip-the-animation (1). */
#define WINDOWSP_STATE_CLOSED 0
#define WINDOWSP_STATE_OPEN 1
#define WINDOWSP_STATE_OPEN_WAIT 2 /* wait for the window to be READY */
#define WINDOWSP_STATE_ACTIVE 3
#define WINDOWSP_STATE_CLOSE 4
#define WINDOWSP_STATE_CLOSING 5
#define WINDOWSP_OPEN_FRESH 0
#define WINDOWSP_OPEN_NO_ANIMATION 1

/* WindowSPCursor capacity: visible item rows (a message and a number each),
 * the two page counters after them in numbers[], and the cursor sprites
 * (selection, scroll up, scroll down). ROW_HEIGHT is one item row. */
#define WINDOWSP_ITEM_SLOTS 24
#define WINDOWSP_NUMBER_INDEX 24 /* numbers[]: selected index + 1 */
#define WINDOWSP_NUMBER_COUNT 25 /* numbers[]: item count */
#define WINDOWSP_NUMBER_SLOTS 26
#define WINDOWSP_CURSOR_COUNT 3
#define WINDOWSP_ROW_HEIGHT 24

/* WindowSPCursor.draw_flags: which parts subMWDraw draws. */
#define WINDOWSP_DRAW_CURSORS 1
#define WINDOWSP_DRAW_MESSAGES 2
#define WINDOWSP_DRAW_NUMBERS 4

/* What WindowDXMain calls through WindowDX.close_callback. */
typedef void (*WindowDXCallback)(WindowDX *window, void *arg);

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

typedef struct WindowDXSprite {
    unsigned char unmodeled_00[0x10];
    unsigned short kind;
    unsigned char unmodeled_12[0x14 - 0x12];
} WindowDXSprite;

typedef struct BattleWinCommand {
    const char *text;
    const char *description;
    unsigned char flags;
    signed char icon;
    unsigned char unmodeled_0a[2];
} BattleWinCommand;

struct WindowDX {
    short x;                           /* +0x00 */
    short y;                           /* +0x02 */
    int style;                         /* +0x04 */
    short width;                       /* +0x08 */
    short height;                      /* +0x0A */
    const char *tag_id;                 /* +0x0C: title text */
    signed char state;                 /* +0x10 */
    unsigned char ribbon_initialized;  /* +0x11 */
    unsigned char unmodeled_12[2];
    /* +0x14: a generic slot. The menus store void and int callbacks alike
     * (MenuStatusDisp, MenuSelectWindow, MenuInfoWindow) without casts,
     * and CharMenuMain's store schedule needs the data-pointer alias set
     * (user decision 2026-11-02). WindowDXMain calls it as a
     * WindowDXCallback. */
    void *close_callback;
    void *close_callback_arg;          /* +0x18 */
    unsigned char unmodeled_1c[4];
    WindowDXRect rect;                 /* +0x20 */
    unsigned char color_r;             /* +0x2C */
    unsigned char color_g;             /* +0x2D */
    unsigned char color_b;             /* +0x2E */
    unsigned char alpha;               /* +0x2F */
    unsigned char unmodeled_30[0x3C - 0x30];
    WindowDXFrame frame;               /* +0x3C */
    ETagFontMainData title_font;        /* +0x80 */
    ERibbonMainData ribbon_top;         /* +0xA0 */
    ERibbonMainData ribbon_bottom;      /* +0x110 */
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
    struct MenuListEntry *items;
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
    ECursolMainData cursors[WINDOWSP_CURSOR_COUNT];
    EMessageParam messages[WINDOWSP_ITEM_SLOTS];
    ENumberMainData numbers[WINDOWSP_NUMBER_SLOTS];
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
void WindowSPSet(WindowSPCursor *list);
void WindowSPMain(WindowSPCursor *list);
void WindowSPSetSelect(WindowSPCursor *list, unsigned char *keep);
void eBattleWinClose(void *window);
void eBattleWinClose3(void);
void eBattleWinInit(void);
void WindowSPKeepSelect(WindowSPCursor *cursor, WindowSPKeepBuffer keep);
void WindowSPKeepSelectCheck(WindowSPKeepBuffer keep);

/* Functions and data used by other translation units. */
void *eBattleWinInit2(void *arg);

#endif /* INCLUDE_MAIN_E_BATTLE_WIN_OPEN_H */
