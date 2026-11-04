/*
 * TU-local declarations of ov12/tu001 (src/ov12/rg_main.c).
 */

#ifndef SRC_OV12_RG_MAIN_H
#define SRC_OV12_RG_MAIN_H

#include "shared.h"
#include "ov12/rg_battle_common_data.h"
#include "ov12/rg_battle_init.h"
#include "ov12/rg_debug_flags.h"
#include "ov12/rg_disp_life.h"
#include "ov12/rg_draw.h"
#include "ov12/rg_effect_env.h"
#include "ov12/rg_motion_info_db.h"
#include "ov12/rg_read_text.h"
#include "ov12/rg_robot_db.h"
#include "ov12/rg_shot_db.h"
#include "ov12/rg_weapon_db.h"
#include "ov12/xrg_paint2d.h"
#include "ov12/xrg_particle.h"
#include "ov12/xrg_sound.h"
#include "ov12/xrg_sysinit.h"

/*
 * Robot-game battle state. Evidenced fields only: _GameInfoInit,
 * _GameInfoNextStage, _GameInfoIsEndOfGame, _GameInfoGetStage,
 * _GameInfoGetCurrentPlayTime and _GameInfoGetCurrentDamage together
 * account for every byte through offset 0x2c.
 */
/* A VS-CPU run is this many stages long; the enemy tables wrap at the same count. */
#define RG_STAGE_COUNT 4
/* Rounds a player must win to take a stage. */
#define RG_ROUNDS_TO_WIN 2

typedef struct RgGameInfo {
    int playerResult[RG_BATTLE_PLAYER_MAX]; /* 0x00: win tally per player; 2 ends the game */
    int stage;           /* 0x08: current stage index */
    int activePlayer;    /* 0x0c: which player's turn is active */
    int playTime[RG_STAGE_COUNT];     /* 0x10: accumulated play time, indexed by stage */
    float damage[RG_STAGE_COUNT];     /* 0x20: accumulated damage, indexed by stage */
} RgGameInfo;

#endif /* SRC_OV12_RG_MAIN_H */
