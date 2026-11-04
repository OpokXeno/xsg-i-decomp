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

extern int strcmp(const char *left, const char *right);
extern const char D_00A54E68[];
extern RgSimpleDB *InstanceOfRgWeaponDB(void);
extern RgSimpleDB *InstanceOfRgRobotDB(void);
extern int RgActorNameToCharID(const char *pszName);
extern void *RgRobotDBGetByID(RgSimpleDB *database, int charID);
extern void *RgWeaponDBGetEssence(RgSimpleDB *pDB, const char *pszName);
extern void InitRgPlayerEssence(struct RgPlayerEssence *pDat);
extern struct RgPlayer *CreateRgPlayer(struct RgPlayerEssence *pDat);

/* The player set-up block InitRgPlayerEssence and CreateRgPlayer read; the
 * same 0x40-byte layout tu015 keeps as RgPlayerEssenceFields. */
struct RgBattlePlayerEssence {
    RgVector position;
    RgVector direction;
    void *spec;
    void *weaponEssence[3];
    void *spareWeaponEssence[3];
    int actorID;
};

int RgBattleInitCreatePlayers(RgBattleInitState *pInfo,
                              struct RgPlayer **ppPlayers, int maxPlayers)
{
    struct RgBattlePlayerEssence essence;
    RgSimpleDB *pWeaponDB;
    void **pSpareEssence;
    void *weaponEssence;
    void *spec;
    unsigned int playerIndex;
    unsigned int weaponIndex;
    unsigned int createdCount;
    RgBattleInitPlayerInfo *pPlayer;

    pWeaponDB = InstanceOfRgWeaponDB();
    if (pInfo == 0) {
        assert_prog(D_00A54E18, D_00A54E28, 82);
    }
    createdCount = 0;
    playerIndex = 0;
    if (maxPlayers != 0) {
        pPlayer = pInfo->players;
        do {
            if (strcmp(pPlayer->actorName, D_00A54E40) != 0) {
                weaponIndex = 0;
                InitRgPlayerEssence((struct RgPlayerEssence *)&essence);
                essence.actorID = RgActorNameToCharID(pPlayer->actorName);
                XrgCopyVector(essence.position, pPlayer->position);
                XrgCopyVector(essence.direction, pPlayer->direction);
                essence.spec = RgRobotDBGetByID(InstanceOfRgRobotDB(),
                                                essence.actorID);
                pSpareEssence = essence.spareWeaponEssence;
                do {
                    weaponEssence = RgWeaponDBGetEssence(
                        pWeaponDB, pPlayer->weaponName[weaponIndex]);
                    spec = essence.spec;
                    essence.weaponEssence[weaponIndex] = weaponEssence;
                    *pSpareEssence = (spec == 0)
                        ? RgWeaponDBGetEssence(pWeaponDB, D_00A54E68)
                        : RgWeaponDBGetEssence(pWeaponDB, (char *)spec + 152);
                    pSpareEssence++;
                    weaponIndex++;
                } while (weaponIndex < 3);
                ppPlayers[createdCount] =
                    CreateRgPlayer((struct RgPlayerEssence *)&essence);
                createdCount++;
            }
            pPlayer++;
            playerIndex++;
        } while ((playerIndex < 2) && (createdCount < maxPlayers));
    }
    return createdCount;
}

static void *_CreateBg(int bgId, char *pszBgName);

void RgBattleInitCreateBg(RgBattleInitInfo *pInfo, int bgId)
{
    if (pInfo == 0) {
        assert_prog(D_00A54E18, D_00A54E28, 0x83);
    }
    _CreateBg(bgId, pInfo->bg);
}

extern struct RgReadText *CreateRgReadText(const char *pszName);
extern void DisposeRgReadText(struct RgReadText *pReader);
extern struct RgBgBuilder *CreateRgBgBuilder(void *pGroup);
extern void DisposeRgBgBuilder(struct RgBgBuilder *pBuilder);
extern void RgBgBuilderBuildFromText(struct RgBgBuilder *pBuilder,
                                     struct RgReadText *pReader,
                                     const char *pszStageName);
extern void *RgBgBuilderGetLoadData(struct RgBgBuilder *pBuilder);
extern void RgBgBuilderGetLight(struct RgBgBuilder *pBuilder, void *pGetBuf);
extern void *InstanceOfGlobalLight(void);
extern void RgWarn(const char *pszFormat, const char *pszSourceFile, int line,
                   ...);
extern const char D_00A54E70[];
extern const char D_00A54E80[];
extern const char D_00A54EA8[];

static void *_CreateBg(int bgId, char *pszBgName)
{
    struct RgReadText *pReader;
    struct RgBgBuilder *pBuilder;
    void *pLoadData;

    pReader = CreateRgReadText(D_00A54E70);
    if ((bgId == 0) || (pszBgName == 0)) {
        assert_prog(D_00A54E80, D_00A54E28, 148);
    }
    if (pReader == 0) {
        RgWarn(D_00A54EA8, D_00A54E28, 151, D_00A54E70);
        return 0;
    }
    pBuilder = CreateRgBgBuilder((void *)bgId);
    RgBgBuilderBuildFromText(pBuilder, pReader, pszBgName);
    pLoadData = RgBgBuilderGetLoadData(pBuilder);
    RgBgBuilderGetLight(pBuilder, InstanceOfGlobalLight());
    DisposeRgBgBuilder(pBuilder);
    DisposeRgReadText(pReader);
    return pLoadData;
}

extern void XrgLog(const char *pszFormat, const char *pszSourceFile, int line,
                   ...);
extern const char D_00A54EC0[];
extern const char D_00A54ED0[];
extern const char D_00A54EF0[];
extern const char D_00A54F00[];
extern const char D_00A54F10[];

void RgBattleInitDump(RgBattleInitState *pDst)
{
    RgBattleInitPlayerInfo *pPlayer;
    int playerIndex;
    int weaponIndex;

    if (pDst == 0) {
        assert_prog(D_00A54EC0, D_00A54E28, 173);
    }
    XrgLog(D_00A54ED0, D_00A54E28, 175, pDst);
    XrgLog(D_00A54EF0, D_00A54E28, 176, pDst->bg);
    playerIndex = 0;
    for (; playerIndex < 2; playerIndex++) {
        pPlayer = &pDst->players[playerIndex];
        XrgLog(D_00A54F00, D_00A54E28, 179, pPlayer->actorName);
        XrgLog(D_00A54F10, D_00A54E28, 180, pPlayer->position[0],
               pPlayer->position[1], pPlayer->position[2]);
        for (weaponIndex = 0; weaponIndex < 3; weaponIndex++) {
            XrgLog(D_00A54F10 + 24, D_00A54E28, 182, weaponIndex,
                   pPlayer->weaponName[weaponIndex]);
        }
    }
}

const char D_00A54E18[] = "pInfo != NIL";

const char D_00A54E28[] = "../rg_battle_init.euc.c";

const unsigned char D_00A54E40[8] = "";

const char D_00A54E48[] = "pDst != NIL && pSrc != NIL";
