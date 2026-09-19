/*
 * OV12 original TU 49: 0x00a2af20..0x00a2b5e8 (6 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_battle_init.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

extern const char D_00A54E18[]; /* "pInfo != NIL" */
extern const char D_00A54E28[]; /* "../rg_battle_init.euc.c" */

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battle_init", InitRgBattleInit);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battle_init", RgBattleInitCopy);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battle_init", RgBattleInitCreatePlayers);

/* Defined later in this TU (a local sibling still in asm). */
static void _CreateBg(int bgId, void *dst);

void RgBattleInitCreateBg(RgBattleInitInfo *pInfo, int bgId)
{
    if (pInfo == 0) {
        assert_prog(D_00A54E18, D_00A54E28, 0x83);
    }
    _CreateBg(bgId, pInfo->bg);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battle_init", _CreateBg);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battle_init", RgBattleInitDump);
