/*
 * TU-local declarations of ov12/tu006 (src/ov12/rg_enemy.c).
 */

#ifndef SRC_OV12_RG_ENEMY_H
#define SRC_OV12_RG_ENEMY_H

#include "shared.h"

/*
 * Opaque handle: this allocation only ever passes RgRobot pointers through,
 * matching the same forward-only view src/ov12/rg_robot_control.h and
 * src/ov12/rg_player.h already use for it.
 */
typedef struct RgRobot RgRobot;

/*
 * The enemy-control record CreateRgEnemyControl allocates and _InitEnemy
 * (unallocated in this TU) fills in; no member is evidenced by this
 * allocation.
 */
typedef struct RgEnemyControl RgEnemyControl;

/* The allocation size CreateRgEnemyControl requests for one RgEnemyControl. */
#define RG_ENEMY_CONTROL_SIZE 0xB0

#endif /* SRC_OV12_RG_ENEMY_H */
