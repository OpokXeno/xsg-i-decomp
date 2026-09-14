/*
 * OV02 original TU 3: 0x00a017a0..0x00a017f0 (1 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/party.h"
#include "main/xgl_flags.h"
#include "title_game_init.h"

extern u8 GameLoopState[];

void TitleGameInit(void)
{
    memset(GameLoopState, 0, 0x2a030);
    xglFlagsInitial();
    PartyDataInit();
    xglFontLoad(1, 0);
    WindowTexLoad(0, 0);
}
