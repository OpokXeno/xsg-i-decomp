#ifndef INCLUDE_OV12_RG_ENEMY_H
#define INCLUDE_OV12_RG_ENEMY_H

#include "shared.h"
#include "ov12/rg_robot_control.h"

/*
 * enemyType: the AI profile word the battle setup passes to
 * CreateRgEnemyControl. Bits 0-1 are the thinking level (the debug tool
 * labels them "saru ika", "hito nami", "kikai teki" and "kami"); each higher
 * bit is one behaviour switch, named after the debug tool's row for it.
 */
#define RG_ENEMY_TYPE_LEVEL_MASK 3
#define RG_ENEMY_TYPE_TURN_WHILE_MOVING 0x4   /* debug row: turn while moving */
#define RG_ENEMY_TYPE_DASH_OFTEN 0x8          /* debug row: dash often */
#define RG_ENEMY_TYPE_HOMING_OFTEN 0x10       /* debug row: homing often */
#define RG_ENEMY_TYPE_SATELLITE_OFTEN 0x20    /* debug row: satellite often */
#define RG_ENEMY_TYPE_BEHIND 0x40             /* debug row: take the back */
#define RG_ENEMY_TYPE_AVOID_LOCKON 0x80       /* debug row: avoid being locked on */
#define RG_ENEMY_TYPE_AVOID_OBSTACLES 0x100   /* debug row: avoid obstacles */
#define RG_ENEMY_TYPE_BREAK_OBSTACLES 0x200   /* debug row: destroy obstacles */
#define RG_ENEMY_TYPE_ACCURATE_SHOT 0x400     /* debug row: accurate shooting */
#define RG_ENEMY_TYPE_DUAL_GUNS 0x800         /* debug row: two guns */
#define RG_ENEMY_TYPE_CLOSE_COMBAT 0x1000     /* debug row: likes close combat */

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
 * The enemy-control object: the robot-control base _InitEnemy fills through
 * InitRgRobotControlCommon (robot, destruct and job methods), then the
 * target robot, the AI profile and the three jobs.
 */
typedef struct RgEnemyControl {
    RgRobotControl base;          /* +0x00 */
    RgStatus *pEnemyRobot;         /* +0x0C */
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
} RgEnemyControl;

/* The allocation size CreateRgEnemyControl requests for one RgEnemyControl. */
#define RG_ENEMY_CONTROL_SIZE 0xB0

/* Functions and data used by other translation units. */
RgEnemyControl *CreateRgEnemyControl(RgStatus *pRobot, RgStatus *pEnemyRobot,
                                     int enemyType);

#endif /* INCLUDE_OV12_RG_ENEMY_H */
