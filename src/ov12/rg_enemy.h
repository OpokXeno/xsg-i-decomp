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

/* A 128-bit GPR quantity (docs/native-ti.md). */
typedef unsigned int Quadword __attribute__((mode(TI)));

/*
 * One of the three behaviour jobs _JobEnemy runs each frame (turning,
 * moving and shooting). _InitEnemy clears the first quadword with a single
 * sq through the Quadword member, which also gives the job 16-byte
 * alignment: _JobEnemy's two-bit test of the shot job's param reads the
 * doubleword holding flags and param (ld 0($17)).
 */
typedef struct RgEnemyJob {
    union {
        Quadword work;            /* +0x00..+0x0F as one quadword */
        struct {
            unsigned int flags;   /* +0x00: move job request bits; shot job weapon slot */
            unsigned int param;   /* +0x04 */
            float angle;          /* +0x08: turn rate or move heading */
            unsigned int count;   /* +0x0C */
        };
    };
    int state;                    /* +0x10 */
    int wait;                     /* +0x14: frames before the state ends */
    unsigned char unmodeled_18[8];
} RgEnemyJob;

/*
 * The first 12 bytes are the robot-control base _InitEnemy fills through
 * InitRgRobotControlCommon (robot, destruct and job methods).
 */
struct RgEnemyControl {
    RgRobot *pRobot;              /* +0x00 */
    unsigned char unmodeled_04[8];
    RgRobot *pEnemyRobot;         /* +0x0C */
    unsigned int enemyType;       /* +0x10: level in bits 0-1, behaviour bits above */
    unsigned char unmodeled_14[0xC];
    RgEnemyJob turn;              /* +0x20 */
    RgEnemyJob move;              /* +0x40 */
    RgEnemyJob shot;              /* +0x60 */
    unsigned int idleTime;        /* +0x80: frames spent off the ground */
    unsigned int escapeTime;      /* +0x84 */
    int restTime;                 /* +0x88 */
    unsigned int boostTime;       /* +0x8C */
    int enemyState;               /* +0x90: _InitEnemy sets 1 */
    unsigned char unmodeled_94[0xC];
    RgVector lastVelocity;        /* +0xA0 */
};

/* The allocation size CreateRgEnemyControl requests for one RgEnemyControl. */
#define RG_ENEMY_CONTROL_SIZE 0xB0

#endif /* SRC_OV12_RG_ENEMY_H */
