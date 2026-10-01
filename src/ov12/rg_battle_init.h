/*
 * TU-local declarations of ov12/tu049 (src/ov12/rg_battle_init.c).
 */

#ifndef SRC_OV12_RG_BATTLE_INIT_H
#define SRC_OV12_RG_BATTLE_INIT_H

#include "shared.h"

/*
 * The battle-init info object RgBattleInitCreateBg receives. Only the
 * background sub-object at +0x240 is modelled here (RgBattleInitCreateBg
 * hands its address to _CreateBg as the destination to build into); the
 * fields other functions of this translation unit use stay unmodelled.
 */
typedef struct RgBattleInitInfo RgBattleInitInfo;

struct RgBattleInitInfo {
    unsigned char unmodeled_000[0x240]; /* +0x00..+0x23f */
    unsigned char bg[1]; /* +0x240: address handed to _CreateBg as its
                           * destination; its own layout is outside this
                           * allocation */
};

/* One 0x120-byte player entry in the complete battle-init record. */
typedef struct RgBattleInitPlayerInfo {
    char actorName[64];
    float position[4];
    float direction[4];
    char weaponName[3][64];
} RgBattleInitPlayerInfo;

/* Complete fields initialized by InitRgBattleInit. */
typedef struct RgBattleInitState {
    RgBattleInitPlayerInfo players[2];
    char bg[64];
    int battleMode;
    int enemyType;
} RgBattleInitState;

#endif /* SRC_OV12_RG_BATTLE_INIT_H */
