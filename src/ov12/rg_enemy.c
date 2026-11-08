/*
 * OV12 original TU 6: 0x00a09ba0..0x00a0cbc8 (9 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_2.h"
#include "main/xgl_font.h"
#include "main/xgl_thread.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_enemy.h"
#include "ov12/rg_geom_point.h"
#include "ov12/rg_geom_robot.h"
#include "ov12/rg_get_frame_time.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_robot.h"
#include "ov12/rg_robot_control.h"
#include "ov12/rg_shot.h"
#include "ov12/rg_weapon.h"
#include "ov12/rg_weapon_db.h"
#include "ov12/xrg_rand_int.h"

/* The debug think tool lists 13 rows: 0 is the level, 1-11 the behaviour switches, 12 all of them. */
#define RG_ENEMY_DBG_ROWS 13
#define RG_ENEMY_DBG_ALL_ROW 12

/* Weapon type flags _GetWeaponTypeFlag builds for _JobEnemy's shot job. */
#define RG_ENEMY_WEAPON_ATTACK 0x20
#define RG_ENEMY_WEAPON_SHIELD 0x40
#define RG_ENEMY_WEAPON_EMPTY 0x8000
/* Any flag above the shot bits: attack, shield or no weapon. */
#define RG_ENEMY_WEAPON_NOT_SHOT (RG_ENEMY_WEAPON_EMPTY | RG_ENEMY_WEAPON_SHIELD | RG_ENEMY_WEAPON_ATTACK)

static int _GetPadEdge(void)
{
    return 0;
}

static int _GetPadRelease(void)
{
    return 0;
}

static int csr_0;
static int i_1;
extern const char enemyDebugSwitchOnText[];
extern const char enemyDebugSwitchOffText[];
extern const char enemyDebugLevelMonkeyText[];
extern const char enemyDebugLevelAverageText[];
extern const char enemyDebugLevelMechanicalText[];
extern const char enemyDebugLevelGodText[];
extern const char enemyDebugThinkLevelLabelText[];
extern const char enemyDebugTurnWhileMovingText[];
extern const char enemyDebugDashHeavyUseText[];
extern const char enemyDebugHomingHeavyUseText[];
extern const char enemyDebugSatelliteHeavyUseText[];
extern const char enemyDebugBackAttackText[];
extern const char enemyDebugDodgeLockOnText[];
extern const char enemyDebugAvoidObstacleText[];
extern const char enemyDebugBreakObstacleText[];
extern const char enemyDebugPreciseShotText[];
extern const char enemyDebugDualGunText[];
extern const char enemyDebugMeleeLoverAutoOnText[];
extern const char enemyDebugAllOnOffText[];
extern const char enemyDebugCursorText[];

/*
 * Debug menu that edits the enemy type flags. Row 0 picks the level, rows 1-11 toggle
 * one switch each and row 12 covers all of them. It returns when Select is released.
 */
static void _dbgThinkTool(unsigned int *pEnemyType)
{
    unsigned int startPressed;
    int armed;
    unsigned int startEdge;

    armed = 0;
    startPressed = _GetPadEdge() & PAD_SELECT;
    startEdge = startPressed;

    for (;;) {
        unsigned int padEdge;

        if (startEdge != 0) {
            armed = 1;
        }
        /* The tool closes once Select is released, when Select was pressed on entry. */
        if (armed != 0 && (_GetPadRelease() & PAD_SELECT)) {
            break;
        }

        padEdge = _GetPadEdge();
        if (padEdge & (PAD_LEFT | PAD_RIGHT)) {
            if (csr_0 == RG_ENEMY_DBG_ALL_ROW) {
                if (padEdge & PAD_LEFT) {
                    *pEnemyType &= 3;
                } else {
                    *pEnemyType |= ~3;
                }
            } else if (csr_0 != 0) {
                *pEnemyType ^= 1 << (csr_0 + 1);
            } else {
                unsigned int enemyType;
                unsigned int keep;
                unsigned int level;

                enemyType = *pEnemyType;
                if (padEdge & PAD_LEFT) {
                    keep = enemyType & ~3;
                    level = (enemyType & 3) - 1;
                } else {
                    keep = enemyType & ~3;
                    level = (enemyType & 3) + 1;
                }
                *pEnemyType = (level & 3) | keep;
            }
        }

        if (_GetPadEdge() & PAD_CIRCLE) {
            if (csr_0 == RG_ENEMY_DBG_ALL_ROW) {
                for (i_1 = 1; i_1 < 12; i_1++) {
                    *pEnemyType ^= 1 << (i_1 + 1);
                }
            }
        }

        padEdge = _GetPadEdge();
        if (padEdge & PAD_UP) {
            csr_0--;
        }
        if (padEdge & PAD_DOWN) {
            csr_0++;
        }
        while (csr_0 < 0) {
            csr_0 += RG_ENEMY_DBG_ROWS;
        }
        csr_0 = csr_0 % RG_ENEMY_DBG_ROWS;

        for (i_1 = 1; i_1 < 12; i_1++) {
            if (((int)*pEnemyType >> (i_1 + 1)) & 1) {
                xglFontDebugPrintf(40, i_1 * 12 + 12, enemyDebugSwitchOnText);
            } else {
                xglFontDebugPrintf(40, i_1 * 12 + 12, enemyDebugSwitchOffText);
            }
        }

        switch ((int)(*pEnemyType & 3)) {
        case 0:
            xglFontDebugPrintf(96, 12, enemyDebugLevelMonkeyText);
            break;
        case 1:
            xglFontDebugPrintf(96, 12, enemyDebugLevelAverageText);
            break;
        case 2:
            xglFontDebugPrintf(96, 12, enemyDebugLevelMechanicalText);
            break;
        case 3:
            xglFontDebugPrintf(96, 12, enemyDebugLevelGodText);
            break;
        default:
            break;
        }
        xglFontDebugPrintf(40, 12, enemyDebugThinkLevelLabelText);
        xglFontDebugPrintf(80, 24, enemyDebugTurnWhileMovingText);
        xglFontDebugPrintf(80, 36, enemyDebugDashHeavyUseText);
        xglFontDebugPrintf(80, 48, enemyDebugHomingHeavyUseText);
        xglFontDebugPrintf(80, 60, enemyDebugSatelliteHeavyUseText);
        xglFontDebugPrintf(80, 72, enemyDebugBackAttackText);
        xglFontDebugPrintf(80, 84, enemyDebugDodgeLockOnText);
        xglFontDebugPrintf(80, 96, enemyDebugAvoidObstacleText);
        xglFontDebugPrintf(80, 108, enemyDebugBreakObstacleText);
        xglFontDebugPrintf(80, 120, enemyDebugPreciseShotText);
        xglFontDebugPrintf(80, 132, enemyDebugDualGunText);
        xglFontDebugPrintf(80, 144, enemyDebugMeleeLoverAutoOnText);
        xglFontDebugPrintf(80, 156, enemyDebugAllOnOffText);
        xglFontDebugPrintf(20, csr_0 * 12 + 12, enemyDebugCursorText);
        xglSleep();
    }
}

/*
 * Returns the type flags of a weapon for _JobEnemy. A missing weapon is empty in slot 2
 * and an attack weapon in the other slots. A shot weapon also gets bit 0 when its busy
 * time fits within wait frames.
 */
static unsigned int _GetWeaponTypeFlag(RgWeapon *weapon, int side, int wait)
{
    RgWeaponEssence *essence;
    struct RgWeaponShotEssence *shotEssence;
    unsigned int essenceFlags;
    unsigned int typeFlags;

    if (weapon == 0) {
        return side == 2 ? RG_ENEMY_WEAPON_EMPTY : RG_ENEMY_WEAPON_ATTACK;
    }

    essence = RgWeaponGetEss(weapon);
    if (essence == 0) {
        return RG_ENEMY_WEAPON_EMPTY;
    }

    essenceFlags = essence->controlFlags;
    typeFlags = 4;
    if (essenceFlags & RG_WEAPON_CONTROL_LOCKON) {
        typeFlags = 0;
    }

    if (RgWeaponEssCastToAttack(essence) != 0) {
        return typeFlags | RG_ENEMY_WEAPON_ATTACK;
    }
    if (RgWeaponEssCastToShield(essence) != 0) {
        return typeFlags | RG_ENEMY_WEAPON_SHIELD;
    }

    if ((shotEssence = RgWeaponEssCastToShot(essence)) != 0) {
        if (!(shotEssence->busyTime + shotEssence->busyTime <
              wait * RG_FRAME_TIME)) {
            return typeFlags;
        }
        return typeFlags | 1;
    }
    return typeFlags | 2;
}

/*
 * Returns nonzero when the weapon may fire. A locked-on targeting weapon always may.
 * With param bit 4 set, a weapon fires only when aimed closely (bit 8) or when there is
 * no weapon.
 */
static int _CheckShootWeapon(RgWeapon *weapon, unsigned int param, float angle)
{
    if (weapon != 0 && RgWeaponIsTargetting(weapon) != 0 &&
        RgWeaponIsLockedOn(weapon) != 0) {
        return 1;
    }

    if (param & 4) {
        if ((param & 8) && angle < 0.049087387f) {
            return 1;
        }
        if (weapon != 0) {
            return 0;
        }
    }
    return 1;
}

static void _dbgThinkTool(unsigned int *pEnemyType);
static unsigned int _GetWeaponTypeFlag(RgWeapon *weapon, int side, int wait);
static int _CheckShootWeapon(RgWeapon *weapon, unsigned int param, float angle);

#define RG_PI XRG_PI
#define RG_ENEMY_NO_WEAPON 0x10000

/* Move job states (RgEnemyJob.state of RgEnemyControl.move). */
enum {
    RG_ENEMY_MOVE_IDLE,
    RG_ENEMY_MOVE_APPROACH,  /* toward the enemy */
    RG_ENEMY_MOVE_RETREAT,   /* away from the enemy */
    RG_ENEMY_MOVE_FORWARD,   /* along our own facing */
    RG_ENEMY_MOVE_BACKWARD,
    RG_ENEMY_MOVE_STRAFE,    /* diagonal around the enemy */
    RG_ENEMY_MOVE_HOLD,      /* wait without moving */
    RG_ENEMY_MOVE_UNSTICK,   /* leave a spot where velocity stalled */
    RG_ENEMY_MOVE_STOP       /* the enemy is dead */
};

static inline float _Abs(float x)
{
    return (x < 0.0f) ? -x : x;
}

/* 1, -1 or 0 by the sign of x. */
static inline int _Sign(float x)
{
    return (x > 0.0f) ? 1 : ((x < 0.0f) ? -1 : 0);
}

/* Keeps an angle within -pi..pi. */
static inline float _RoundAngle(float angle)
{
    while (angle > RG_PI) {
        angle -= RG_PI * 2;
    }
    while (angle < -RG_PI) {
        angle += RG_PI * 2;
    }
    return angle;
}

/*
 * A random frame count in base..base+range, shortened by the enemy level
 * (enemyType bits 0-1).
 */
static inline int _EnemyTime(RgEnemyControl *pCtrl, int range, int base)
{
    int level = pCtrl->enemyType & RG_ENEMY_TYPE_LEVEL_MASK;
    int time = (XrgRandInt() & range) + base;

    time = (time >> level) - level;
    if (time < 0) {
        time = 0;
    }
    return time;
}

/* The cosine of the angle between two vectors, 0 for a null vector. */
static inline float _GetCosine(RgVector first, RgVector second)
{
    float length = XrgLengthVector(first);

    length *= XrgLengthVector(second);
    if (length > 0.0f) {
        return (first[0] * second[0] + first[1] * second[1] +
                first[2] * second[2]) / length;
    }
    return 0.0f;
}

/*
 * Runs one AI step for an enemy robot. It picks the turn, move and shot commands from the
 * enemy's position, speed and status, and applies them to the robot. Select opens the
 * debug think tool.
 */
static void _JobEnemy(RgEnemyControl *pCtrl)
{
    RgVector position;
    RgVector enemyPosition;
    RgVector direction;
    RgWeapon *weapons[3];
    RgVector velocity;
    RgVector enemyVelocity;
    RgVector move;
    unsigned int weaponFlags[3];
    int targetting;
    RgStatus *pRobot;
    RgStatus *pEnemy;
    RgGeomPoint *pGeom;
    RgGeomPoint *pEnemyGeom;
    RgEnemyJob *pJob;
    unsigned int status;
    unsigned int enemyStatus;
    unsigned int landing;
    int state;
    int wait;
    float rotate;
    float enemyAngle;
    float relAngle;
    float absAngle;
    float distance;
    float range;
    float speed;
    float enemySpeed;
    float cosine;
    float diff;

    targetting = 0;
    pRobot = pCtrl->base.robot;
    pEnemy = pCtrl->pEnemyRobot;
    pGeom = RgRobotGetGeom(pRobot);
    pEnemyGeom = RgRobotGetGeom(pEnemy);
    __RgGeomPointGetPos(pGeom, position, "../rg_enemy.euc.c", 427);
    __RgGeomPointGetPos(pEnemyGeom, enemyPosition, "../rg_enemy.euc.c", 428);
    XrgSubVector(direction, enemyPosition, position);
    direction[1] = 0.0f;
    direction[3] = 0.0f;
    rotate = RgGeomRobotGetRotate(&pGeom->geom);
    enemyAngle = atan2(direction[0], direction[2]);
    distance = XrgNormalizeVector(direction, direction);
    relAngle = _RoundAngle(enemyAngle - rotate);
    absAngle = _Abs(relAngle);

    weapons[0] = RgRobotGetWeapon(pRobot, 0);
    weapons[1] = RgRobotGetWeapon(pRobot, 1);
    weapons[2] = RgRobotGetWeapon(pRobot, 2);
    RgGeomPointGetVel(pGeom, velocity);
    speed = RgGeomPointGetSpeed(pGeom);
    RgGeomPointGetVel(pEnemyGeom, enemyVelocity);
    enemySpeed = RgGeomPointGetSpeed(pEnemyGeom);
    status = RgRobotGetStatusFlags(pRobot);
    enemyStatus = RgRobotGetStatusFlags(pEnemy);

    if (pCtrl->enemyState == 1) {
        pCtrl->enemyState = 0;
    }
    if (RgRobotGetLife(pRobot) > RgRobotGetLife(pEnemy)) {
        range = (RgRobotGetLife(pRobot) - RgRobotGetLife(pEnemy)) * 3.0f + 10.0f;
    } else {
        range = 10.0f;
    }
    if (pCtrl->enemyType & RG_ENEMY_TYPE_CLOSE_COMBAT) {
        range -= 45.0f;
    }

    landing = status & 1;
    if (landing) {
        pCtrl->idleTime = 0;
    } else if (pCtrl->idleTime < 0x200) {
        pCtrl->idleTime++;
    }

    if (pCtrl->move.state == RG_ENEMY_MOVE_STRAFE) {
        if (pCtrl->escapeTime > _EnemyTime(pCtrl, 0xF0, 0x78) + 200) {
            pCtrl->restTime = _EnemyTime(pCtrl, 0x320, 0x190) + 0x100;
        } else {
            pCtrl->escapeTime += 2;
        }
    } else if (pCtrl->escapeTime != 0) {
        pCtrl->escapeTime--;
    }

    if (pCtrl->restTime != 0) {
        pCtrl->restTime--;
        if ((pCtrl->enemyType & RG_ENEMY_TYPE_SATELLITE_OFTEN) && pCtrl->restTime != 0) {
            pCtrl->restTime--;
        }
        range -= 5.0f;
        if (pCtrl->escapeTime != 0) {
            pCtrl->escapeTime--;
        }
    }

    if (status & 0x10) {
        pCtrl->boostTime += 2;
    } else {
        pCtrl->boostTime >>= 1;
    }

    if (RgRobotIsDead(pEnemy)) {
        pCtrl->enemyState = 2;
        pCtrl->turn.state = 4;
        pCtrl->move.state = RG_ENEMY_MOVE_STOP;
        pCtrl->shot.state = 5;
    }

    /* Turning. */
    state = pCtrl->turn.state;
    wait = pCtrl->turn.wait;
    pJob = &pCtrl->turn;
    if (state == 0) {
        if (pCtrl->idleTime < 500 - _EnemyTime(pCtrl, 0x190, 0xC8) || landing ||
            distance < 50.0f) {
            if (absAngle > RG_PI / 8) {
                /* The enemy's facing relative to ours. */
                diff = RgGeomRobotGetRotate(&pEnemyGeom->geom);
                diff = _RoundAngle(_RoundAngle(diff) - _RoundAngle(rotate));
                if (position[1] <= 1.51f &&
                    ((!(status & 0x100) && absAngle > RG_PI * 3 / 4 &&
                      _EnemyTime(pCtrl, 0x50, 0x28) < pJob->count) ||
                     ((pCtrl->enemyType & RG_ENEMY_TYPE_HOMING_OFTEN) && absAngle > RG_PI / 2 &&
                      pCtrl->move.state != RG_ENEMY_MOVE_STRAFE && diff > RG_PI / 2))) {
                    wait = _EnemyTime(pCtrl, 0x1E, 0xF);
                    pJob->count = 0;
                    targetting = 1;
                    state = 3;
                } else {
                    state = 1;
                    wait = _EnemyTime(pCtrl, 0x3C, 0x1E);
                    pJob->count++;
                    pJob->param = 0;
                    pJob->angle = (relAngle > 0.0f) ? RG_PI / 4 : -RG_PI / 4;
                }
            } else if (pJob->count != 0) {
                pJob->count--;
            }
        } else {
            state = 2;
            wait = 60;
            pJob->param = 0;
            pJob->count++;
            if (_Abs(pJob->angle) < RG_PI / 4) {
                pJob->angle = (relAngle > 0.0f) ? RG_PI / 4 : -RG_PI / 4;
            } else if (_Sign(pJob->angle) != _Sign(relAngle)) {
                pJob->angle = -pJob->angle;
            }
        }
        if ((!(pCtrl->enemyType & RG_ENEMY_TYPE_TURN_WHILE_MOVING) || (status & 0x100)) && pCtrl->move.state != 0 &&
            state != 3) {
            state = 0;
            wait = 0;
        }
    }
    switch (state) {
    case 1:
        RgRobotAccelarateRotate(pRobot, pJob->angle);
        if (pJob->param == 0) {
            if (absAngle < RG_PI / 16) {
                pJob->param = _EnemyTime(pCtrl, 0xF, 7);
            }
        } else {
            pJob->param--;
            if (pJob->param == 0) {
                pJob->angle = -pJob->angle;
            }
        }
        if (wait <= 0) {
            state = 0;
        }
        break;
    case 2:
        RgRobotAccelarateRotate(pRobot, pJob->angle);
        if (pJob->param == 0) {
            if (landing && (absAngle < RG_PI / 2 || (int)(pCtrl->enemyType & RG_ENEMY_TYPE_LEVEL_MASK) >= 2)) {
                pJob->param = _EnemyTime(pCtrl, 0xF, 7);
            }
        } else {
            pJob->param--;
            if (pJob->param == 0) {
                wait = 0;
            }
        }
        if (wait <= 0) {
            state = 0;
        }
        break;
    case 3:
        if (targetting || velocity[1] > 0.01f) {
            wait++;
        }
        if (wait <= 0) {
            state = 0;
        }
        break;
    }
    if (wait > 0) {
        wait--;
    }
    pCtrl->turn.state = state;
    pCtrl->turn.wait = wait;

    /* Moving. */
    state = pCtrl->move.state;
    wait = pCtrl->move.wait;
    pJob = &pCtrl->move;
    if ((pCtrl->enemyType & RG_ENEMY_TYPE_AVOID_OBSTACLES) && state != RG_ENEMY_MOVE_HOLD && state != RG_ENEMY_MOVE_IDLE &&
        state != RG_ENEMY_MOVE_STOP) {
        cosine = _GetCosine(pCtrl->lastVelocity, velocity);
        if (state != RG_ENEMY_MOVE_UNSTICK && _Abs(cosine) < 0.5f && speed < 0.2f) {
            state = RG_ENEMY_MOVE_UNSTICK;
            if (relAngle > 0.0f) {
                pJob->param = 1;
            } else {
                pJob->param = 0;
            }
            pJob->angle = atan2(pCtrl->lastVelocity[0], pCtrl->lastVelocity[2]);
            if (cosine < 0.0f) {
                pJob->angle += RG_PI * 3 / 8;
            } else {
                pJob->angle += RG_PI * 5 / 8;
            }
            pJob->angle = _RoundAngle(pJob->angle);
            wait = _EnemyTime(pCtrl, 0x32, 0x19);
        }
    }
    if (state == 0) {
        if (absAngle < RG_PI * 3 / 8) {
            if (!(enemyStatus & 0x62) && enemySpeed < 0.05f && landing &&
                pCtrl->boostTime == 0) {
                if (distance > 5.0f) {
                    pJob->flags &= ~1;
                    state = 3;
                    wait = _EnemyTime(pCtrl, 0xA, 5);
                }
            } else if (distance < range + 70.0f) {
                if (pCtrl->restTime == 0 &&
                    ((pCtrl->enemyType & RG_ENEMY_TYPE_SATELLITE_OFTEN) || (enemyStatus & 2))) {
                    if ((pCtrl->enemyType & RG_ENEMY_TYPE_AVOID_LOCKON) && (enemyStatus & 2)) {
                        if (pCtrl->shot.state == 0 && absAngle < RG_PI / 8) {
                            pJob->param ^= 1;
                        }
                        pJob->flags |= 1;
                    } else if (distance < 5.0f && !(pCtrl->enemyType & RG_ENEMY_TYPE_CLOSE_COMBAT)) {
                        /* A random number is drawn and discarded. */
                        XrgRandInt();
                        if (pCtrl->enemyType & RG_ENEMY_TYPE_DASH_OFTEN) {
                            pJob->flags |= 1;
                        }
                    } else if ((pCtrl->enemyType & RG_ENEMY_TYPE_BEHIND) || !(pJob->flags & 4)) {
                        diff = RgGeomRobotGetRotate(&pEnemyGeom->geom);
                        diff = _RoundAngle(_RoundAngle(diff) - _RoundAngle(rotate));
                        if (_Abs(diff) < RG_PI * 3 / 8) {
                            if ((pCtrl->enemyType & RG_ENEMY_TYPE_DASH_OFTEN) || !(absAngle < RG_PI / 8)) {
                                pJob->flags |= 1;
                            }
                        } else if (diff > 0.0f) {
                            pJob->param = 0;
                        } else {
                            pJob->param = 1;
                        }
                    } else if (speed < 1.0f) {
                        pJob->param ^= 1;
                    }
                    if ((pCtrl->enemyType & RG_ENEMY_TYPE_DASH_OFTEN) && absAngle < RG_PI / 16) {
                        pJob->flags |= 1;
                    }
                    state = 5;
                    wait = _EnemyTime(pCtrl, 0x3C, 0x1E);
                } else {
                    if ((pCtrl->enemyType & RG_ENEMY_TYPE_AVOID_LOCKON) && (enemyStatus & 2)) {
                        state = 5;
                        pJob->param ^= 1;
                        wait = _EnemyTime(pCtrl, 0x1E, 0xF) + 60;
                    } else {
                        unsigned int closeRange = enemyStatus & 0x40;

                        if (!closeRange && distance < range + 20.0f) {
                            wait = _EnemyTime(pCtrl, 0x32, 0x19);
                            state = 2;
                        } else if (!(enemyStatus & 2) && distance < range + 40.0f &&
                                   !(pCtrl->enemyType & RG_ENEMY_TYPE_CLOSE_COMBAT)) {
                            wait = _EnemyTime(pCtrl, 0x14, 0xA);
                            state = 6;
                        } else if (!closeRange && distance < range + 70.0f) {
                            state = 3;
                            if (distance > 100.0f) {
                                pJob->flags |= 1;
                            }
                            wait = _EnemyTime(pCtrl, 0x14, 0xA);
                        } else {
                            state = 5;
                            /* The random time is based on the new state number. */
                            wait = _EnemyTime(pCtrl, 0xA, state) + 20;
                            if (pCtrl->enemyType & RG_ENEMY_TYPE_DASH_OFTEN) {
                                pJob->flags |= 1;
                            }
                        }
                    }
                    if ((pCtrl->enemyType & RG_ENEMY_TYPE_DASH_OFTEN) && absAngle < RG_PI / 16) {
                        pJob->flags |= 1;
                    }
                    if (enemyStatus & 0x40) {
                        pJob->flags |= 1;
                    }
                }
            } else {
                state = 1;
                wait = _EnemyTime(pCtrl, 0x32, 0x19);
            }
        } else {
            state = 4;
            wait = _EnemyTime(pCtrl, 0x32, 0x19);
        }
        pJob->flags &= ~4;
    }
    if ((!(pCtrl->enemyType & RG_ENEMY_TYPE_TURN_WHILE_MOVING) || (status & 0x100)) && pCtrl->turn.state != 0) {
        state = 0;
        wait = 0;
        pJob->flags &= ~3;
    }

    move[0] = move[2] = 0.0f;
    switch (state) {
    case RG_ENEMY_MOVE_APPROACH:
        move[0] = direction[0];
        move[2] = direction[2];
        if (wait <= 0) {
            state = 0;
        }
        break;
    case RG_ENEMY_MOVE_RETREAT:
        move[0] = -direction[0];
        move[2] = -direction[2];
        if (pCtrl->enemyType & RG_ENEMY_TYPE_CLOSE_COMBAT) {
            state = RG_ENEMY_MOVE_APPROACH;
        }
        if (wait <= 0) {
            state = 0;
        }
        break;
    case RG_ENEMY_MOVE_FORWARD:
        move[0] = xglSin(rotate);
        move[2] = xglCos(rotate);
        if (wait <= 0) {
            state = 0;
        }
        break;
    case RG_ENEMY_MOVE_BACKWARD:
        move[0] = -xglSin(rotate);
        move[2] = -xglCos(rotate);
        if (wait <= 0) {
            state = 0;
        }
        break;
    case RG_ENEMY_MOVE_STRAFE:
        if (pJob->flags & 1) {
            enemyAngle = _RoundAngle(enemyAngle);
            if (pJob->param & 1) {
                move[0] = xglSin(enemyAngle + RG_PI / 4);
                move[2] = xglCos(enemyAngle + RG_PI / 4);
            } else {
                move[0] = xglSin(enemyAngle - RG_PI / 4);
                move[2] = xglCos(enemyAngle - RG_PI / 4);
            }
        } else {
            if (pJob->param & 1) {
                move[0] = xglSin(rotate + RG_PI / 4);
                move[2] = xglCos(rotate + RG_PI / 4);
            } else {
                move[0] = xglSin(rotate - RG_PI / 4);
                move[2] = xglCos(rotate - RG_PI / 4);
            }
            if (distance < range + 20.0f) {
                move[0] -= direction[0];
                move[2] -= direction[2];
            }
        }
        if (wait <= 0) {
            state = 0;
            pJob->flags |= 4;
        }
        break;
    case RG_ENEMY_MOVE_HOLD:
        if (wait <= 0) {
            state = 0;
        }
        break;
    case RG_ENEMY_MOVE_UNSTICK:
        move[0] = xglSin(pJob->angle);
        move[2] = xglCos(pJob->angle);
        if (wait <= 0) {
            if (_Abs(_GetCosine(pCtrl->lastVelocity, velocity)) > 0.5f &&
                _Abs(pJob->angle - RG_PI / 2) < RG_PI * 2) {
                if (pJob->param) {
                    pJob->angle += RG_PI / 4;
                } else {
                    pJob->angle -= RG_PI / 4;
                }
                wait = _EnemyTime(pCtrl, 0x14, 0xA) + 10;
            } else {
                pJob->param = 0;
                state = 0;
            }
        }
        break;
    }

    if ((pJob->flags & 0x10) && !(status & 4)) {
        pJob->flags &= ~0x10;
    }
    if (pCtrl->boostTime > _EnemyTime(pCtrl, 0x3C, 0x1E) && !(pJob->flags & 0x10)) {
        pJob->flags |= 8;
        if (pCtrl->boostTime & 1) {
            move[0] = xglSin(enemyAngle + RG_PI / 4);
            move[2] = xglCos(enemyAngle + RG_PI / 4);
        } else {
            move[0] = xglSin(enemyAngle - RG_PI / 4);
            move[2] = xglCos(enemyAngle - RG_PI / 4);
        }
    }
    /* Dash, brake or accelerate with the move chosen above. */
    if (pJob->flags & 8) {
        RgRobotDash(pRobot, move);
        pJob->flags = (pJob->flags & ~9) | 0x10;
    } else if (pJob->flags & 2) {
        RgRobotBreak(pRobot);
        pJob->flags &= ~3;
    } else {
        move[1] = move[3] = 0.0f;
        if ((status & 0x104) == 4 && state != RG_ENEMY_MOVE_UNSTICK &&
            (((enemyStatus & 2) && !(status & 2) && wait <= 0 && !(pJob->flags & 0x10)) ||
             ((pJob->flags & 0x10) && _EnemyTime(pCtrl, 0x50, 0x28) < pCtrl->boostTime))) {
            RgRobotDashContinue(pRobot);
        }
        if ((pJob->flags & 1) && (wait <= 0 || (pCtrl->enemyType & RG_ENEMY_TYPE_DASH_OFTEN))) {
            RgRobotDash(pRobot, move);
            pJob->flags &= ~1;
        } else if ((status & 0x100) &&
                   ((absAngle < RG_PI / 4 && !(enemyStatus & 2)) || !(enemyStatus & 1) ||
                    distance > 150.0f) &&
                   state != RG_ENEMY_MOVE_UNSTICK) {
            RgRobotBreak(pRobot);
            pJob->flags &= ~3;
        } else {
            RgRobotAccelarate(pRobot, move);
        }
    }
    __asm__ __volatile__(
        "lqc2 $vf31, 0(%1)\n"
        "sqc2 $vf31, 0(%0)\n"
        :
        : "r" (pCtrl->lastVelocity), "r" (velocity)
        : "memory");
    if (wait > 0) {
        wait--;
    }
    pCtrl->move.state = state;
    pCtrl->move.wait = wait;
    if (targetting) {
        RgRobotTargetting(pRobot);
    }

    /* Shooting. */
    state = pCtrl->shot.state;
    wait = pCtrl->shot.wait;
    pJob = &pCtrl->shot;
    switch (state) {
    case 1:
        if ((pJob->param & 1) &&
            _CheckShootWeapon(weapons[pJob->flags], pJob->param, absAngle)) {
            RgRobotShot(pRobot, pJob->flags);
        }
        if (wait <= 0) {
            if (_CheckShootWeapon(weapons[pJob->flags], pJob->param, absAngle)) {
                RgRobotShot(pRobot, pJob->flags);
            }
            if ((pJob->param & 2) && !(pJob->param & 1)) {
                pJob->param |= 1;
                wait = _EnemyTime(pCtrl, 0x14, 0xA);
            } else {
                state = 0;
            }
        }
        break;
    case 2:
        if (!(pJob->param & 1) &&
            _CheckShootWeapon(weapons[pJob->flags], pJob->param, absAngle)) {
            RgRobotShot(pRobot, pJob->flags);
            if (!(pJob->param & 2)) {
                pJob->param |= 1;
            }
        }
        if (wait <= 0) {
            state = 0;
        }
        break;
    case 3:
        if (_CheckShootWeapon(weapons[pJob->flags], pJob->param, absAngle)) {
            RgRobotShot(pRobot, pJob->flags);
        }
        if (wait <= 0) {
            state = 0;
        }
        break;
    case 4:
        if (_CheckShootWeapon(weapons[pJob->flags], pJob->param, absAngle)) {
            RgRobotShot(pRobot, pJob->flags);
        }
        if ((enemyStatus & 0xC2) != 0x42 || !landing || distance < 14.0f ||
            absAngle > RG_PI / 4 || weapons[pJob->flags] == 0 || wait <= 0) {
            pJob->param++;
        } else if (pJob->param >= 4) {
            pJob->param -= 4;
        }
        if ((4 - (pCtrl->enemyType & RG_ENEMY_TYPE_LEVEL_MASK)) * 4 < pJob->param) {
            state = 0;
        }
        break;
    }

    /* Pick a shot state and a weapon when no shot is pending. */
    if (state == 0 && !(status & 0x100)) {
        if (pCtrl->move.state != RG_ENEMY_MOVE_UNSTICK &&
            ((status & 2) ||
             ((absAngle < RG_PI / 8 || !(pCtrl->enemyType & RG_ENEMY_TYPE_ACCURATE_SHOT)) &&
              absAngle < RG_PI * 3 / 8)) &&
            (landing || (pCtrl->enemyType & RG_ENEMY_TYPE_BREAK_OBSTACLES))) {
            if ((enemyStatus & 0x10) || enemyVelocity[1] > 0.01f ||
                (enemySpeed < 0.1f && distance < 20.0f)) {
                state = 3;
                if (distance > 35.0f) {
                    wait = _EnemyTime(pCtrl, 0x23, 0x11);
                } else {
                    wait = _EnemyTime(pCtrl, 0xF, 7);
                }
            } else if (distance < 35.0f) {
                wait = _EnemyTime(pCtrl, 0x14, 0xA);
                state = 2;
            } else {
                wait = _EnemyTime(pCtrl, 0x1E, 0xF);
                state = 1;
            }
        }
        if (state != 0) {
            unsigned int lastWeapon;
            unsigned int allFlags;
            int found;

            pJob->param = 0;
            lastWeapon = pJob->flags;
            pJob->flags = RG_ENEMY_NO_WEAPON;
            weaponFlags[0] = _GetWeaponTypeFlag(weapons[0], 0, wait);
            weaponFlags[1] = _GetWeaponTypeFlag(weapons[1], 1, wait);
            weaponFlags[2] = _GetWeaponTypeFlag(weapons[2], 2, wait);
            allFlags = weaponFlags[0] | weaponFlags[1] | weaponFlags[2];
            if (!(pCtrl->enemyType & RG_ENEMY_TYPE_CLOSE_COMBAT) &&
                (weaponFlags[0] & RG_ENEMY_WEAPON_NOT_SHOT) &&
                (weaponFlags[1] & RG_ENEMY_WEAPON_NOT_SHOT) &&
                (weaponFlags[2] & RG_ENEMY_WEAPON_NOT_SHOT)) {
                pCtrl->enemyType |= RG_ENEMY_TYPE_CLOSE_COMBAT;
            }
            if ((allFlags & RG_ENEMY_WEAPON_SHIELD) && (enemyStatus & 0x42) && landing &&
                distance > 16.0f &&
                distance < 70.0f && !(enemyStatus & 0x80) &&
                RgRobotGetLife(pRobot) > RgRobotGetLife(pEnemy) + 10.0f &&
                absAngle < RG_PI / 8) {
                if ((weaponFlags[0] & RG_ENEMY_WEAPON_SHIELD) && relAngle > 0.0f) {
                    pJob->flags = 0;
                } else if ((weaponFlags[1] & RG_ENEMY_WEAPON_SHIELD) && relAngle < 0.0f) {
                    pJob->flags = 1;
                } else if (weaponFlags[2] & RG_ENEMY_WEAPON_SHIELD) {
                    pJob->flags = 2;
                } else if (weaponFlags[0] & RG_ENEMY_WEAPON_SHIELD) {
                    pJob->flags = 0;
                } else {
                    pJob->flags = 1;
                }
                wait += _EnemyTime(pCtrl, 0x320, 0x190) + 200;
                state = 4;
                pJob->param = 0;
            } else if ((allFlags & RG_ENEMY_WEAPON_ATTACK) && distance < 15.0f &&
                       absAngle < RG_PI / 4 && !(enemyStatus & 0x40)) {
                if ((weaponFlags[0] & RG_ENEMY_WEAPON_ATTACK) && relAngle > 0.0f) {
                    pJob->flags = 0;
                } else if ((weaponFlags[1] & RG_ENEMY_WEAPON_ATTACK) && relAngle < 0.0f) {
                    pJob->flags = 1;
                } else if (weaponFlags[2] & RG_ENEMY_WEAPON_ATTACK) {
                    pJob->flags = 2;
                } else if (weaponFlags[0] & RG_ENEMY_WEAPON_ATTACK) {
                    pJob->flags = 0;
                } else {
                    pJob->flags = 1;
                }
                state = 1;
                if (distance < 10.0f) {
                    state = 2;
                }
                if (absAngle > RG_PI / 8) {
                    pJob->param |= 8;
                }
                wait += _EnemyTime(pCtrl, 0x14, 0xA);
            } else {
                found = 0;
                if ((pCtrl->enemyType & RG_ENEMY_TYPE_DUAL_GUNS) && (int)weaponFlags[0] < RG_ENEMY_WEAPON_ATTACK &&
                    (int)weaponFlags[1] < RG_ENEMY_WEAPON_ATTACK) {
                    RgWeaponEssence *essence;
                    struct RgWeaponShotEssence *shotEssence;
                    float lastBusyTime = 0.0f;
                    float time;

                    if (weapons[lastWeapon] != 0 &&
                        (essence = RgWeaponGetEss(weapons[lastWeapon])) != 0 &&
                        (shotEssence = RgWeaponEssCastToShot(essence)) != 0) {
                        lastBusyTime = shotEssence->busyTime;
                    }
                    pJob->flags = lastWeapon + 1;
                    if (pJob->flags >= 2) {
                        pJob->flags = 0;
                    }
                    if (weapons[pJob->flags] != 0 &&
                        (essence = RgWeaponGetEss(weapons[pJob->flags])) != 0 &&
                        (shotEssence = RgWeaponEssCastToShot(essence)) != 0) {
                        time = RgGetFrameTime();
                        if (!(lastBusyTime + lastBusyTime < shotEssence->busyTime) &&
                            !(shotEssence->busyTime + shotEssence->busyTime < lastBusyTime)) {
                            /* From the frame time to the busy time in frames. */
                            time = shotEssence->busyTime / (time + time);
                            if (wait < time) {
                                wait = time;
                            }
                            found = 1;
                        }
                    }
                    if (weaponFlags[pJob->flags] & 1) {
                        pJob->param |= 2;
                    }
                    if (weaponFlags[pJob->flags] & 4) {
                        pJob->param |= 8;
                    }
                    if (pCtrl->enemyType & RG_ENEMY_TYPE_ACCURATE_SHOT) {
                        pJob->param |= 4;
                    }
                }
                if (!found) {
                    if ((int)weaponFlags[0] < RG_ENEMY_WEAPON_ATTACK && relAngle > RG_PI / 32) {
                        pJob->flags = 0;
                    } else if ((int)weaponFlags[1] < RG_ENEMY_WEAPON_ATTACK && relAngle < -RG_PI / 32) {
                        pJob->flags = 1;
                    } else if ((int)weaponFlags[2] < RG_ENEMY_WEAPON_ATTACK) {
                        pJob->flags = 2;
                    } else if ((int)weaponFlags[0] < RG_ENEMY_WEAPON_ATTACK) {
                        pJob->flags = 0;
                    } else if ((int)weaponFlags[1] < RG_ENEMY_WEAPON_ATTACK) {
                        pJob->flags = 1;
                    }
                    if (pJob->flags != RG_ENEMY_NO_WEAPON) {
                        if (weaponFlags[pJob->flags] & 1) {
                            pJob->param |= 2;
                        }
                        if (weaponFlags[pJob->flags] & 4) {
                            pJob->param |= 8;
                        }
                        if (pCtrl->enemyType & RG_ENEMY_TYPE_ACCURATE_SHOT) {
                            pJob->param |= 4;
                        }
                    }
                }
            }
            if (pJob->flags == RG_ENEMY_NO_WEAPON) {
                pJob->flags = lastWeapon;
                state = 0;
            } else {
                if ((int)weaponFlags[pJob->flags] < RG_ENEMY_WEAPON_ATTACK && (pJob->param & 4) &&
                    weapons[pJob->flags] != 0) {
                    RgWeaponEssence *essence = RgWeaponGetEss(weapons[pJob->flags]);

                    if ((pCtrl->enemyType & RG_ENEMY_TYPE_ACCURATE_SHOT) && distance > 25.0f) {
                        float chance;

                        chance = _Abs(_GetCosine(direction, enemyVelocity));

                        if (essence != 0 && essence->capacity > 0.0f) {
                            chance += RgWeaponGetShotNum(weapons[pJob->flags]) /
                                      essence->capacity;
                        } else {
                            chance += 1.0f;
                        }
                        if (chance < 1.1f) {
                            state = 0;
                        }
                    }
                    /*
                     * The shot's reach: speed times most of its life. Every
                     * shot kind keeps its speed at the normal shot's offset.
                     */
                    if (essence != 0 &&
                        ((RgNormalShotEssence *)essence->shotEssence)->speed *
                                (essence->shotEssence->life * 0.9f) + 3.0f <
                            distance) {
                        state = 0;
                    }
                }
                if (state != 0) {
                    pCtrl->move.flags &= ~1;
                } else {
                    pJob->flags = lastWeapon;
                }
            }
        }
    }
    if (wait > 0) {
        wait--;
    }
    pCtrl->shot.state = state;
    pCtrl->shot.wait = wait;

    /* Releasing Select opens the debug think tool. */
    if (_GetPadRelease() & PAD_SELECT) {
        _dbgThinkTool(&pCtrl->enemyType);
    }
}


static void _DestructEnemy(RgEnemyControl *pBaka)
{
    if (pBaka == 0) {
        assert_prog("pBaka != NIL", "../rg_enemy.euc.c", 1562);
    }
}

/*
 * Sets the enemy's job and destruct methods, its enemy robot and type, and clears its
 * turn, move and shot state.
 */
static void _InitEnemy(RgEnemyControl *pControl, RgStatus *pRobot,
                       RgStatus *pEnemyRobot, int enemyType)
{
    float zeroVelocity;

    if (pControl == 0) {
        assert_prog("pBaka != NIL", "../rg_enemy.euc.c", 1570);
    }

    InitRgRobotControlCommon(&pControl->base, pRobot);
    pControl->base.jobMethod = (RgRobotControlJobFunc)_JobEnemy;
    pControl->base.destructMethod = (RgRobotControlDestructFunc)_DestructEnemy;
    pControl->pEnemyRobot = pEnemyRobot;
    pControl->enemyType = enemyType;
    pControl->turn.work = 0;
    pControl->move.work = 0;
    pControl->shot.work = 0;

    zeroVelocity = 0.0f;
    pControl->turn.state = 0;
    pControl->turn.wait = 0;
    pControl->move.state = 0;
    pControl->move.wait = 0;
    pControl->shot.state = 0;
    pControl->shot.wait = 0;
    pControl->boostTime = 0;
    pControl->restTime = 0;
    pControl->escapeTime = 0;
    pControl->idleTime = 0;
    pControl->lastVelocity[3] = 0.0f;
    pControl->lastVelocity[2] = 0.0f;
    pControl->lastVelocity[1] = zeroVelocity;
    pControl->lastVelocity[0] = 0.0f;
    pControl->enemyState = 1;
}

/*
 * Declared again here, after its definition above. CreateRgEnemyControl
 * passes enemyType through without checking it.
 */
static void _InitEnemy(RgEnemyControl *pControl, RgStatus *pRobot,
                       RgStatus *pEnemyRobot, int enemyType);

/*
 * Allocates an enemy control from the heap and initializes it for pRobot against
 * pEnemyRobot. Returns the new control.
 */
RgEnemyControl *CreateRgEnemyControl(RgStatus *pRobot, RgStatus *pEnemyRobot,
                                     int enemyType)
{
    RgEnemyControl *pControl;

    if (pRobot == 0) {
        assert_prog("pRobot != NIL", "../rg_enemy.euc.c", 1612);
    }
    if (pEnemyRobot == 0) {
        assert_prog("pEnemyRobot != NIL", "../rg_enemy.euc.c", 1613);
    }
    pControl = RgHeapAlloc(InstanceOfRgHeap(), RG_ENEMY_CONTROL_SIZE,
                           "../rg_enemy.euc.c", 1615);
    _InitEnemy(pControl, pRobot, pEnemyRobot, enemyType);
    return pControl;
}



static int csr_0 = 0;

const char enemyDebugSwitchOnText[8] = "\013\243\317\243\316";

const char enemyDebugSwitchOffText[8] = "\013\243\317\243\306\243\306";

const char enemyDebugLevelMonkeyText[8] = "\013\261\356\260\312\262\274";

const char enemyDebugLevelAverageText[8] = "\013\277\315\312\302\244\337";

const char enemyDebugLevelMechanicalText[8] = "\013\265\241\263\243\305\252";

const char enemyDebugLevelGodText[8] = "\013\277\300\241\252";

const char enemyDebugThinkLevelLabelText[16] = "\013\273\327\271\315\245\354\245\331\245\353\241\247";

const char enemyDebugTurnWhileMovingText[16] = "\013\260\334\306\260\303\346\300\373\262\363\262\304\307\275";

const char enemyDebugDashHeavyUseText[16] = "\013\245\300\245\303\245\267\245\345\302\277\315\321";

const char enemyDebugHomingHeavyUseText[16] = "\013\245\333\241\274\245\337\245\363\245\260\302\277\315\321";

const char enemyDebugSatelliteHeavyUseText[16] = "\013\245\265\245\306\245\351\245\244\245\310\302\277\315\321";

const char enemyDebugBackAttackText[16] = "\013\307\330\314\314\274\350\244\352";

const char enemyDebugDodgeLockOnText[24] = "\013\310\357\245\355\245\303\245\257\245\252\245\363\262\363\310\362";

const char enemyDebugAvoidObstacleText[16] = "\013\276\343\263\262\312\252\262\363\310\362";

const char enemyDebugBreakObstacleText[16] = "\013\276\343\263\262\312\252\307\313\262\365";

const char enemyDebugPreciseShotText[16] = "\013\300\265\263\316\274\315\267\342";

const char enemyDebugDualGunText[16] = "\013\306\363\304\362\267\375\275\306";

const char enemyDebugMeleeLoverAutoOnText[40] = "\013\300\334\266\341\300\357\271\245\244\255\241\312\301\264\311\360\264\357\303\306\300\332\244\354\274\253\306\260\243\317\243\316\241\313";

const char enemyDebugAllOnOffText[24] = "\013\260\354\263\347\243\317\243\316\241\277\243\317\243\306\243\306";

const char enemyDebugCursorText[8] = "\013\241\344";

static int i_1;
