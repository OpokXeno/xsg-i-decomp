#include "common.h"
#include "shared.h"
#include "e_battle_win_open.h"

INCLUDE_ASM("asm/main/nonmatchings/e_battle_win_open", eBattleWinOpen);

void eBattleWinClose(void *window)
{
    BW->state = 4;
    BW->window.state = 4;
    BW->active = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/e_battle_win_open", eBattleWinMain);

INCLUDE_ASM("asm/main/nonmatchings/e_battle_win_open", eBattleWinOpen2);

INCLUDE_ASM("asm/main/nonmatchings/e_battle_win_open", eBattleWinMain2);

void eBattleWinClose2(void)
{
    BW2->window.state = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/e_battle_win_open", eBattleWinOpen3);

INCLUDE_ASM("asm/main/nonmatchings/e_battle_win_open", eBattleWinMain3);

void eBattleWinClose3(void)
{
    BW3->window.state = 4;
    BW3->secondary_state = 4;
}

INCLUDE_ASM("asm/main/nonmatchings/e_battle_win_open", eBattleWinOpen4);

INCLUDE_ASM("asm/main/nonmatchings/e_battle_win_open", eBattleWinMain4);

void eBattleWinClose4(void)
{
    BW4->window.state = 4;
}

int eBattleWinPageCheck4(void)
{
    return BW4->page;
}

INCLUDE_ASM("asm/main/nonmatchings/e_battle_win_open", eBattleWinInit2);

void eBattleWinInit(void)
{
    eBattleWinInit2(MenuWorkEndGet());
}

INCLUDE_ASM("asm/main/nonmatchings/e_battle_win_open", WindowDXFlagChange);

void WindowDXSet(WindowDX *window)
{
    /*
     * These nine stores are independent of each other, so -O2 schedules them
     * out of source order: it fills the jr delay slot with the third-to-last
     * store below and moves the last store to right after the two immediate
     * loads. color_r is written last here so it lands in that early slot,
     * matching the original's own first store to window.
     */
    window->alpha = 0x60;
    window->tag_id = 0;
    window->close_callback = 0;
    window->close_callback_arg = 0;
    window->state = 0;
    window->ribbon_initialized = 0;
    window->color_g = 0x80;
    window->color_b = 0x80;
    window->color_r = 0x80;
}

INCLUDE_ASM("asm/main/nonmatchings/e_battle_win_open", WindowDXMain);

INCLUDE_ASM("asm/main/nonmatchings/e_battle_win_open", subMWModeExSet);

INCLUDE_ASM("asm/main/nonmatchings/e_battle_win_open", subMWPosSet);

INCLUDE_ASM("asm/main/nonmatchings/e_battle_win_open", subMWDraw);

INCLUDE_ASM("asm/main/nonmatchings/e_battle_win_open", subMWControlType00);

INCLUDE_ASM("asm/main/nonmatchings/e_battle_win_open", subMWControlType01);

INCLUDE_ASM("asm/main/nonmatchings/e_battle_win_open", WindowSPSelect);

INCLUDE_ASM("asm/main/nonmatchings/e_battle_win_open", WindowSPSet);

INCLUDE_ASM("asm/main/nonmatchings/e_battle_win_open", WindowSPItemChange);

INCLUDE_ASM("asm/main/nonmatchings/e_battle_win_open", WindowSPSelectJump);

INCLUDE_ASM("asm/main/nonmatchings/e_battle_win_open", WindowSPMain);

void WindowSPKeepSelect(WindowSPCursor *cursor, WindowSPKeepBuffer keep)
{
    if (cursor->item_count != 0) {
        keep[0] = (unsigned char)cursor->item_count;
        keep[1] = (unsigned char)cursor->selected_index;
        keep[2] = cursor->visible_top;
        keep[3] = cursor->visible_bottom;
        keep[4] = cursor->current_row;
    }
}

/* main/menu_1.c, still assembler; called here with the same keep-select
 * buffer, matching the pass-through argument this call site proves. */
extern int MenuCursorKeepCheck(WindowSPKeepBuffer keep);

void WindowSPKeepSelectCheck(WindowSPKeepBuffer keep)
{
    if (MenuCursorKeepCheck(keep) == 0) {
        memset(keep, 0, 5);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/e_battle_win_open", WindowSPSetSelect);
