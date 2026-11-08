/*
 * OV02 original TU 3: 0x00a017a0..0x00a017f0 (1 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/party.h"
#include "main/xgl_flags.h"
#include "ov02/ov02.h"
#include "ov02/title_game_init.h"

/* Clears GameLoopState, then initializes the xgl flags and party data and loads the font and window textures. */
void TitleGameInit(void)
{
    memset(&GameLoopState, 0, GAME_LOOP_STATE_SIZE);
    xglFlagsInitial();
    PartyDataInit();
    xglFontLoad(1, 0);
    WindowTexLoad(0, 0);
}
