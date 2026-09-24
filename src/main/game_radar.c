#include "common.h"
#include "shared.h"
#include "main/xgl_cd.h"

INCLUDE_ASM("asm/main/nonmatchings/game_radar", GameRadarDraw);

INCLUDE_ASM("asm/main/nonmatchings/game_radar", DrawRadarPing);

INCLUDE_ASM("asm/main/nonmatchings/game_radar", DrawRadarInfo);

INCLUDE_ASM("asm/main/nonmatchings/game_radar", DrawActorPoint);

INCLUDE_ASM("asm/main/nonmatchings/game_radar", CheckActorExist);

extern u8 *image_004DC554;
extern u8 rate;
extern const char D_004C0618[];

void GameRadarInit(void)
{
    image_004DC554 = (u8 *) (((int) WorkEnd + 0xF) & ~0xF);
    WorkEnd = image_004DC554 + xglCdReadFile(D_004C0618, image_004DC554, 0, 0);
    rate = 0;
}
