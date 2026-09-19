/*
 * OV10 original TU 1: 0x00a00410..0x00a08670 (15 functions)
 */
#include "common.h"
#include "shared.h"

INCLUDE_ASM("asm/nonmatchings/ov10/card_play_sound1", CardPlaySound1);

INCLUDE_ASM("asm/nonmatchings/ov10/card_play_sound1", CardPlaySound3);

INCLUDE_ASM("asm/nonmatchings/ov10/card_play_sound1", CardPlaySound2);

INCLUDE_ASM("asm/nonmatchings/ov10/card_play_sound1", CardPlaySound4);

INCLUDE_ASM("asm/nonmatchings/ov10/card_play_sound1", CardDataLoad);

typedef signed int s32;

extern s32 CardFread(u32 mode, char *filename);
extern char D_00A4BD00[];

u32 CardGraphicLoad(void) {
    return (u32) ~CardFread(0x0100C000, D_00A4BD00) >> 0x1F;
}

INCLUDE_ASM("asm/nonmatchings/ov10/card_play_sound1", CardTitleInit);

INCLUDE_ASM("asm/nonmatchings/ov10/card_play_sound1", CMIBGInit);

INCLUDE_ASM("asm/nonmatchings/ov10/card_play_sound1", CardMainInit);

INCLUDE_ASM("asm/nonmatchings/ov10/card_play_sound1", CardMainItemGetSub);

INCLUDE_ASM("asm/nonmatchings/ov10/card_play_sound1", CardMainAddItemSub);

INCLUDE_ASM("asm/nonmatchings/ov10/card_play_sound1", NewCMPGameModeExit);

INCLUDE_ASM("asm/nonmatchings/ov10/card_play_sound1", CaedGameWinMode);

extern char D_00A4C788[];

void CMTSetFontColor(s32 count, s32 player)
{
    if (count == player) {
        xglFontPrint(0, 0, 0, D_00A4C788);
    } else if (count < player) {
        xglFontPrint(0, 0, 0, D_00A4C788);
    } else {
        xglFontPrint(0, 0, 0, D_00A4C788);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov10/card_play_sound1", CardMainProc);
