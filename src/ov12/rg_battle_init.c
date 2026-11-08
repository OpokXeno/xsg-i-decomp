/*
 * OV12 original TU 49: 0x00a2af20..0x00a2b5e8 (6 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_actor_charid.h"
#include "ov12/rg_battle_init.h"
#include "ov12/rg_bg_builder.h"
#include "ov12/rg_error.h"
#include "ov12/rg_light.h"
#include "ov12/rg_player.h"
#include "ov12/rg_read_text.h"
#include "ov12/rg_robot_db.h"
#include "ov12/rg_robot_spec.h"
#include "ov12/rg_weapon_db.h"
#include "ov12/xrg_rand_int.h"
#include "ov12/xrg_v_log.h"

extern const char battleInitAssertInfoNotNil[];
extern const char battleInitSourcePath[];
extern const unsigned char battleInitEmptyString[8];
extern const char battleInitAssertDstAndSrcNotNil[];
extern const char battleInitUnarmedWeaponName[];
extern const char battleInitBgInfoFileName[];
extern const char battleInitAssertBgGroupAndNameNotNil[];
extern const char battleInitCannotOpenFormat[];
extern const char battleInitAssertDstNotNil[];
extern const char battleInitDumpHeaderFormat[];
extern const char battleInitDumpBgFormat[];
extern const char battleInitDumpPlayerFormat[];
extern const char battleInitDumpPositionAndWeaponFormats[];

static RgFileSysData *_CreateBg(struct RgGeomGroup *pGroup, char *pszBgName);

/* Resets every player to an empty name with default positions and directions,
 * and clears the background name, mode and enemy type. */
void InitRgBattleInit(RgBattleInit *pInfo)
{
    unsigned int playerIndex;
    unsigned int weaponIndex;

    if (pInfo == 0) {
        assert_prog(battleInitAssertInfoNotNil, battleInitSourcePath, 0x1E);
    }
    for (playerIndex = 0; playerIndex < RG_BATTLE_PLAYER_MAX; playerIndex++) {
        pInfo->players[playerIndex].actorName[0] = battleInitEmptyString[0];
        XrgClearVector(pInfo->players[playerIndex].position);
        XrgClearVector(pInfo->players[playerIndex].direction);
        pInfo->players[playerIndex].direction[2] = 1.0f;
        for (weaponIndex = 0; weaponIndex < RG_BATTLE_WEAPON_SLOTS; weaponIndex++) {
            pInfo->players[playerIndex].weapons.names[weaponIndex][0] =
                battleInitEmptyString[0];
        }
    }
    XrgSetVectorXYZ(pInfo->players[0].position, 0.0f, 2.0f, -10.0f);
    XrgSetVectorXYZ(pInfo->players[1].position, 0.0f, 2.0f, 10.0f);
    XrgSetVectorXYZ(pInfo->players[0].direction, 0.0f, 0.0f, 1.0f);
    XrgSetVectorXYZ(pInfo->players[1].direction, 0.0f, 0.0f, -1.0f);
    pInfo->bg[0] = battleInitEmptyString[0];
    pInfo->mode = 0;
    pInfo->enemyType = 0;
}

/* Copies the background name, and each player's name, position, direction
 * and weapon names, from pSrc to pDst. */
void RgBattleInitCopy(RgBattleInit *pDst,
                      RgBattleInit *pSrc)
{
    int playerIndex;
    int weaponCount;
    char (*pSrcWeapon)[RG_BATTLE_NAME_SIZE];
    char (*pDstWeapon)[RG_BATTLE_NAME_SIZE];

    if ((pDst == 0) || (pSrc == 0)) {
        assert_prog(battleInitAssertDstAndSrcNotNil, battleInitSourcePath, 57);
    }
    strcpy(pDst->bg, pSrc->bg);
    for (playerIndex = 0; playerIndex < RG_BATTLE_PLAYER_MAX; playerIndex++) {
        strcpy(pDst->players[playerIndex].actorName,
               pSrc->players[playerIndex].actorName);
        XrgCopyVector(pDst->players[playerIndex].position,
                      pSrc->players[playerIndex].position);
        XrgCopyVector(pDst->players[playerIndex].direction,
                      pSrc->players[playerIndex].direction);
        pSrcWeapon = pSrc->players[playerIndex].weapons.names;
        pDstWeapon = pDst->players[playerIndex].weapons.names;
        for (weaponCount = RG_BATTLE_WEAPON_SLOTS - 1; weaponCount >= 0; weaponCount--) {
            strcpy(*pDstWeapon, *pSrcWeapon);
            pDstWeapon++;
            pSrcWeapon++;
        }
    }
}

/* Creates a player for each non-empty entry, up to maxPlayers, and returns
 * how many were created. */
int RgBattleInitCreatePlayers(RgBattleInit *pInfo,
                              struct RgPlayer **ppPlayers, int maxPlayers)
{
    struct RgPlayerEssence essence;
    RgSimpleDB *pWeaponDB;
    RgWeaponEssence **pSpareEssence;
    RgWeaponEssence *weaponEssence;
    RgRobotSpec *spec;
    unsigned int playerIndex;
    unsigned int weaponIndex;
    unsigned int createdCount;
    RgBattleInitPlayer *pPlayer;

    pWeaponDB = InstanceOfRgWeaponDB();
    if (pInfo == 0) {
        assert_prog(battleInitAssertInfoNotNil, battleInitSourcePath, 82);
    }
    createdCount = 0;
    playerIndex = 0;
    if (maxPlayers != 0) {
        pPlayer = pInfo->players;
        do {
            if (strcmp(pPlayer->actorName, battleInitEmptyString) != 0) {
                weaponIndex = 0;
                InitRgPlayerEssence(&essence);
                essence.actorID = RgActorNameToCharID(pPlayer->actorName);
                XrgCopyVector(essence.position, pPlayer->position);
                XrgCopyVector(essence.direction, pPlayer->direction);
                essence.spec = RgRobotDBGetByID(InstanceOfRgRobotDB(),
                                                essence.actorID);
                pSpareEssence = essence.spareWeaponEssence;
                do {
                    weaponEssence = RgWeaponDBGetEssence(
                        pWeaponDB, pPlayer->weapons.names[weaponIndex]);
                    spec = essence.spec;
                    essence.weaponEssence[weaponIndex] = weaponEssence;
                    *pSpareEssence = (spec == 0)
                        ? RgWeaponDBGetEssence(pWeaponDB, battleInitUnarmedWeaponName)
                        : RgWeaponDBGetEssence(pWeaponDB, spec->sude);
                    pSpareEssence++;
                    weaponIndex++;
                } while (weaponIndex < RG_BATTLE_WEAPON_SLOTS);
                ppPlayers[createdCount] =
                    CreateRgPlayer(&essence);
                createdCount++;
            }
            pPlayer++;
            playerIndex++;
        } while ((playerIndex < RG_BATTLE_PLAYER_MAX) && (createdCount < maxPlayers));
    }
    return createdCount;
}

/* Builds the background named by the battle init into pGroup. */
RgFileSysData *RgBattleInitCreateBg(RgBattleInit *pInfo, struct RgGeomGroup *pGroup)
{
    if (pInfo == 0) {
        assert_prog(battleInitAssertInfoNotNil, battleInitSourcePath, 0x83);
    }
    return _CreateBg(pGroup, pInfo->bg);
}

/* Reads the background table from rg_bg.info and builds the named background
 * into pGroup. Returns its load data, or 0 when the table cannot be opened. */
static RgFileSysData *_CreateBg(struct RgGeomGroup *pGroup, char *pszBgName)
{
    struct RgReadText *pReader;
    struct RgBgBuilder *pBuilder;
    RgFileSysData *pLoadData;

    pReader = CreateRgReadText(battleInitBgInfoFileName);
    if ((pGroup == 0) || (pszBgName == 0)) {
        assert_prog(battleInitAssertBgGroupAndNameNotNil, battleInitSourcePath, 148);
    }
    if (pReader == 0) {
        RgWarn(battleInitCannotOpenFormat, battleInitSourcePath, 151, battleInitBgInfoFileName);
        return 0;
    }
    pBuilder = CreateRgBgBuilder(pGroup);
    RgBgBuilderBuildFromText(pBuilder, pReader, pszBgName);
    pLoadData = RgBgBuilderGetLoadData(pBuilder);
    RgBgBuilderGetLight(pBuilder, InstanceOfGlobalLight());
    DisposeRgBgBuilder(pBuilder);
    DisposeRgReadText(pReader);
    return pLoadData;
}

/* Logs the battle init header, background, players and weapon names. */
void RgBattleInitDump(RgBattleInit *pDst)
{
    RgBattleInitPlayer *pPlayer;
    int playerIndex;
    int weaponIndex;

    if (pDst == 0) {
        assert_prog(battleInitAssertDstNotNil, battleInitSourcePath, 173);
    }
    XrgLog(battleInitDumpHeaderFormat, battleInitSourcePath, 175, pDst);
    XrgLog(battleInitDumpBgFormat, battleInitSourcePath, 176, pDst->bg);
    playerIndex = 0;
    for (; playerIndex < RG_BATTLE_PLAYER_MAX; playerIndex++) {
        pPlayer = &pDst->players[playerIndex];
        XrgLog(battleInitDumpPlayerFormat, battleInitSourcePath, 179, pPlayer->actorName);
        XrgLog(battleInitDumpPositionAndWeaponFormats, battleInitSourcePath, 180, pPlayer->position[0],
               pPlayer->position[1], pPlayer->position[2]);
        for (weaponIndex = 0; weaponIndex < RG_BATTLE_WEAPON_SLOTS; weaponIndex++) {
            XrgLog(battleInitDumpPositionAndWeaponFormats + 24, battleInitSourcePath, 182, weaponIndex,
                   pPlayer->weapons.names[weaponIndex]);
        }
    }
}

const char battleInitAssertInfoNotNil[] = "pInfo != NIL";

const char battleInitSourcePath[] = "../rg_battle_init.euc.c";

const unsigned char battleInitEmptyString[8] = "";

const char battleInitAssertDstAndSrcNotNil[32] = "pDst != NIL && pSrc != NIL";



const char battleInitUnarmedWeaponName[8] = "UNARMED";

const char battleInitBgInfoFileName[16] = "rg_bg.info";

const char battleInitAssertBgGroupAndNameNotNil[40] = "pBgGroup != NIL && pszBgName != NIL";

const char battleInitCannotOpenFormat[24] = "cannot open '%s'";

const char battleInitAssertDstNotNil[16] = "pDst != NIL";

const char battleInitDumpHeaderFormat[32] = "**** dump battle init %p ****\n";

const char battleInitDumpBgFormat[16] = "  BG : %s\n";

const char battleInitDumpPlayerFormat[16] = "  PLAYER : %s\n";

const char battleInitDumpPositionAndWeaponFormats[24] = "  POS    : %f %f %f\n";

const char battleInitWeaponDumpFormat[24] = "     WPN (%d) : %s\n";
