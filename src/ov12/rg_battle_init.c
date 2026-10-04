/*
 * OV12 original TU 49: 0x00a2af20..0x00a2b5e8 (6 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_battle_init.h"
#include "ov12/xrg_rand_int.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

extern const char D_00A54E18[];
extern const char D_00A54E28[];
extern const unsigned char D_00A54E40[8];
extern const char D_00A54E48[];
extern void XrgSetVectorXYZ(RgVector destination, float x, float y, float z);
extern char *strcpy(char *destination, const char *source);

void InitRgBattleInit(RgBattleInitState *pInfo)
{
    unsigned int playerIndex;
    unsigned int weaponIndex;

    if (pInfo == 0) {
        assert_prog(D_00A54E18, D_00A54E28, 0x1E);
    }
    for (playerIndex = 0; playerIndex < 2; playerIndex++) {
        pInfo->players[playerIndex].actorName[0] = D_00A54E40[0];
        XrgClearVector(pInfo->players[playerIndex].position);
        XrgClearVector(pInfo->players[playerIndex].direction);
        pInfo->players[playerIndex].direction[2] = 1.0f;
        for (weaponIndex = 0; weaponIndex < 3; weaponIndex++) {
            pInfo->players[playerIndex].weaponName[weaponIndex][0] =
                D_00A54E40[0];
        }
    }
    XrgSetVectorXYZ(pInfo->players[0].position, 0.0f, 2.0f, -10.0f);
    XrgSetVectorXYZ(pInfo->players[1].position, 0.0f, 2.0f, 10.0f);
    XrgSetVectorXYZ(pInfo->players[0].direction, 0.0f, 0.0f, 1.0f);
    XrgSetVectorXYZ(pInfo->players[1].direction, 0.0f, 0.0f, -1.0f);
    pInfo->bg[0] = D_00A54E40[0];
    pInfo->battleMode = 0;
    pInfo->enemyType = 0;
}

void RgBattleInitCopy(RgBattleInitState *pDst,
                      RgBattleInitState *pSrc)
{
    int playerIndex;
    int weaponCount;
    char (*pSrcWeapon)[64];
    char (*pDstWeapon)[64];

    if ((pDst == 0) || (pSrc == 0)) {
        assert_prog(D_00A54E48, D_00A54E28, 57);
    }
    strcpy(pDst->bg, pSrc->bg);
    for (playerIndex = 0; playerIndex < 2; playerIndex++) {
        strcpy(pDst->players[playerIndex].actorName,
               pSrc->players[playerIndex].actorName);
        XrgCopyVector(pDst->players[playerIndex].position,
                      pSrc->players[playerIndex].position);
        XrgCopyVector(pDst->players[playerIndex].direction,
                      pSrc->players[playerIndex].direction);
        pSrcWeapon = pSrc->players[playerIndex].weaponName;
        pDstWeapon = pDst->players[playerIndex].weaponName;
        for (weaponCount = 2; weaponCount >= 0; weaponCount--) {
            strcpy(*pDstWeapon, *pSrcWeapon);
            pDstWeapon++;
            pSrcWeapon++;
        }
    }
}

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

const char D_00A54E18[] = "pInfo != NIL";
const char D_00A54E28[] = "../rg_battle_init.euc.c";
const unsigned char D_00A54E40[8] = "";
const char D_00A54E48[] = "pDst != NIL && pSrc != NIL";
