#include "common.h"

#include "shared.h"

#include "game_debug_menu.h"

char D_004D9EF8[];

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

void GameDebugMenu(void)
{
    int pageStep;

    pageStep = 0;
    if (PadData.half_2a & PAD_L1) {
        pageStep--;
    }
    if (PadData.half_2a & PAD_R1) {
        pageStep = 1;
    }
    if (pageStep) {
        unsigned int nextPage =
            (GameLoopState.debugPage + pageStep + 6) % 6;
        GameLoopState.debugPageInitialized = 0;
        GameLoopState.debugPage = nextPage;
    }
    switch (GameLoopState.debugPage) {
    case 0:
        PauseMenuPage3();
        return;
    case 1:
        PauseMenuPage0();
        return;
    case 2:
        PauseMenuPage2();
        return;
    case 3:
        PauseMenuPageUwamono();
        return;
    case 4:
        PauseMenuPageEnemy();
        return;
    case 5:
        PauseMenuPagePartyDebug();
        break;
    }
}

char D_004D9EF8[8] = "%4x";
