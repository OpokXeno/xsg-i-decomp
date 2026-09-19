#include "common.h"
#include "shared.h"
#include "game_pause_disp.h"
#include "main/xgl_packet.h"

static void DrawShadow(PauseDrawContext *context)
{
    sceVif1PkAddDirectDataN(context->packet, ShadowEnv, 7);
}

INCLUDE_ASM("asm/main/nonmatchings/game_pause_disp", DrawCredit);

INCLUDE_ASM("asm/main/nonmatchings/game_pause_disp", PauseMenu);

void GamePauseDispBG(void)
{
    sceVif1PkRef(xglPacketGetCurrent(), ShadowEnv, 7, 0, 0, 0);
}

void GamePauseDispCf(void)
{
    xglFontPrint(256 - xglFontGetStringWidth(pause_0) / 2,
                 200, 0x00ffffff, pause_0);
    GamePauseDispBG();
}

void GamePauseDispEvent(void)
{
    GamePauseDispCf();
    xglFontPrint(256 - xglFontGetStringWidth(msg1_1) / 2,
                 256, 0x00ffffff, msg1_1);
    xglFontPrint(256 - xglFontGetStringWidth(msg2_2) / 2,
                 288, 0x00ffffff, msg2_2);
}
