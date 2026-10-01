#include "common.h"
#include "shared.h"
#include "game_debug_menu.h"

static void PauseMenuPage0(void)
{
    int y;
    int i;

    if (PadData.half_2a & PAD_CROSS) {
        GameResourceDump(0);
    }

    xglFontDebugPrintf(136, 52, "WrkEnd:%8x", WorkEnd);

    y = 64;
    for (i = 0; i < 16; i++) {
        xglFontDebugPrintf(8, y, D_004D9EF8, actor[i].inUseId);
        y += 8;
    }

    GameResourceDump(1);
}

INCLUDE_ASM("asm/main/nonmatchings/game_debug_menu", PauseMenuPage2);

INCLUDE_ASM("asm/main/nonmatchings/game_debug_menu", PauseMenuPage3);

INCLUDE_ASM("asm/main/nonmatchings/game_debug_menu", GameDebugMenu);
