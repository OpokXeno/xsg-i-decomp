/*
 * OV12 original TU 70: 0x00a3c6a0..0x00a3cd10 (7 functions)
 */
#include "common.h"

extern int s_bIgnoreEventFlag;

extern int XrgEventIsUsablePlayer(unsigned int index);
extern int XrgEventIsUsableEnemy(unsigned int index);

void RgSelectIgnoreEventFlag(void)
{
    s_bIgnoreEventFlag = 1;
}

int RgSelectGetPlayerChars(void)
{
    unsigned int i;
    int mask;

    mask = 1;
    for (i = 0; i < 6; i++)
    {
        if (s_bIgnoreEventFlag || XrgEventIsUsablePlayer(i))
        {
            mask |= 1 << i;
        }
    }
    return mask;
}

int RgSelectGetEnemyChars(void)
{
    unsigned int i;
    int mask;

    mask = 1;
    for (i = 0; i < 6; i++)
    {
        if (s_bIgnoreEventFlag || XrgEventIsUsableEnemy(i))
        {
            mask |= 1 << i;
        }
    }
    return mask;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_subcon", _get_all_weapons);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_subcon", RgSelectWeapons);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_subcon", RgSelectGetEnemyChar);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_subcon", RgSelectGetEnemyWeapons);
