/*
 * TU-local declarations of main/tu166 (src/main/e_battle_win_open.c).
 */

#ifndef SRC_MAIN_E_BATTLE_WIN_OPEN_H
#define SRC_MAIN_E_BATTLE_WIN_OPEN_H

#include "shared.h"
#include "main/e_battle_win_open.h"
#include "main/e_message.h"
#include "main/e_number_main.h"
#include "main/end_print.h"
#include "main/menu_1.h"
#include "main/open_close_main.h"
#include "main/window_tex_load.h"
#include "main/xgl_sound.h"

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

/*
 * A saved snapshot of one WindowSPCursor: item_count, selected_index,
 * visible_top, visible_bottom and current_row packed into 5 bytes in that
 * order (see WindowSPKeepSelect). WindowSPKeepSelectCheck clears it when
 * MenuCursorKeepCheck (main/menu_1.c, still assembler) reports nothing to
 * restore.
 */
typedef unsigned char WindowSPKeepBuffer[5];

typedef struct BattleWinPad {
    unsigned char unmodeled_00[0x2A];
    unsigned short half_2a;
    unsigned char unmodeled_2c[6];
    unsigned short half_32;
    unsigned short half_34;
} BattleWinPad;

typedef struct BattleWinTask {
    unsigned char unmodeled_00[0x1C];
    int buttons;
    int selection;
} BattleWinTask;

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
    struct BattleWinCommand *commands;      /* +0x0004 */
    WindowSPCursor list;             /* +0x0008 */
    WindowDX window;                 /* +0x1740 */
    EMessageParam message;        /* +0x18D4 */
};

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
    EMessageParam message;          /* +0x019C */
};

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
    EMessageParam messages[3];  /* +0x0198 */
    ETagFontMainData tag_font;     /* +0x0264 */
    ENumberMainData number;        /* +0x0284 */
    WindowDX window2;              /* +0x0314 */
    ETagFontMainData tag_fonts[5]; /* +0x04A8 */
    ENumberMainData numbers[5];    /* +0x0548 */
    EMessageParam messages2[4]; /* +0x0818 */
};

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
    EMessageParam message; /* +0x019C */
};

extern BattleWinPad PadData;
extern BattleWindow *BW;
extern BattleWindow2 *BW2;
extern BattleWindow3 *BW3;
extern BattleWindow4 *BW4;

void WindowSPSet(WindowSPCursor *list);
void WindowSPSelectJump(WindowSPCursor *list, int index);
void WindowSPMain(WindowSPCursor *list);
void subMWModeExSet(WindowSPCursor *list);
void subMWPosSet(WindowSPCursor *list);
void subMWDraw(WindowSPCursor *list);
void subMWControlType00(WindowSPCursor *list);
void subMWControlType01(WindowSPCursor *list);
void OpenCloseMain(void *controller);
void endSpriteSet(void *sprite, int mode);
#endif /* SRC_MAIN_E_BATTLE_WIN_OPEN_H */
