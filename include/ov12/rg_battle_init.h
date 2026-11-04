#ifndef INCLUDE_OV12_RG_BATTLE_INIT_H
#define INCLUDE_OV12_RG_BATTLE_INIT_H

#include "shared.h"

struct RgPlayer;
struct RgGeomGroup;

#define RG_BATTLE_PLAYER_MAX 2
#define RG_BATTLE_WEAPON_SLOTS 3
#define RG_BATTLE_NAME_SIZE 64

/* RgBattleInit.mode is the bit mask of human-controlled players. */
#define RG_BATTLE_MODE_PLAYER1 1
#define RG_BATTLE_MODE_PLAYER2 2
#define RG_BATTLE_MODE_VS_CPU RG_BATTLE_MODE_PLAYER1
#define RG_BATTLE_MODE_TWO_PLAYERS (RG_BATTLE_MODE_PLAYER1 | RG_BATTLE_MODE_PLAYER2)

/* One 0x120-byte participant: actor, start pose and the three weapon names. */
typedef struct RgBattleInitPlayer {
    char actorName[RG_BATTLE_NAME_SIZE];
    RgVector position;
    RgVector direction;
    union {
        char names[RG_BATTLE_WEAPON_SLOTS][RG_BATTLE_NAME_SIZE];
        unsigned long long words[RG_BATTLE_WEAPON_SLOTS][RG_BATTLE_NAME_SIZE / 8];
    } weapons; /* the doubleword view is how the hard-mode names are copied */
} RgBattleInitPlayer;

/* The battle set-up record: participants, background name, mode and enemy level. */
typedef struct RgBattleInit {
    RgBattleInitPlayer players[RG_BATTLE_PLAYER_MAX]; /* +0x000 */
    char bg[RG_BATTLE_NAME_SIZE];                     /* +0x240 */
    int mode;                                         /* +0x280 */
    int enemyType;                                    /* +0x284: enemy level */
} RgBattleInit;

/* Functions and data used by other translation units. */
void InitRgBattleInit(RgBattleInit *pInfo);
void RgBattleInitCopy(RgBattleInit *pDst, RgBattleInit *pSrc);
int RgBattleInitCreatePlayers(RgBattleInit *pInfo, struct RgPlayer **ppPlayers,
                              int maxPlayers);
RgFileSysData *RgBattleInitCreateBg(RgBattleInit *pInfo, struct RgGeomGroup *pGroup);
void RgBattleInitDump(RgBattleInit *pDst);

#endif /* INCLUDE_OV12_RG_BATTLE_INIT_H */
