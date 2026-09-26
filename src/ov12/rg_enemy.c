/*
 * OV12 original TU 6: 0x00a09ba0..0x00a0cbc8 (9 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_enemy.h"
#include "ov12/rg_weapon_db.h"
#include "ov12/rg_shot.h"

static int _GetPadEdge(void)
{
    return 0;
}

static int _GetPadRelease(void)
{
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_enemy", _dbgThinkTool);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_enemy", _GetWeaponTypeFlag);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_enemy", _CheckShootWeapon);

extern const char D_00A51D90[]; /* "../rg_enemy.euc.c" */

typedef struct RgWeapon RgWeapon;

extern RgGeomPoint *RgRobotGetGeom(RgRobot *pRobot);
extern void __RgGeomPointGetPos(RgGeomPoint *point, RgVector destination,
                                const char *source_file, int source_line);
extern float RgGeomRobotGetRotate(RgGeomPoint *point);
extern void RgGeomPointGetVel(RgGeomPoint *point, RgVector velocity);
extern float RgGeomPointGetSpeed(RgGeomPoint *point);
extern void XrgSubVector(RgVector destination, RgVector first, RgVector second);
extern float XrgNormalizeVector(RgVector destination, RgVector source);
extern float XrgLengthVector(RgVector vector);
extern int XrgRandInt(void);
extern double atan2(double y, double x);
extern float xglSin(float angle);
extern float xglCos(float angle);
extern RgWeapon *RgRobotGetWeapon(RgRobot *pRobot, unsigned int eSide);
extern unsigned int RgRobotGetStatusFlags(RgRobot *pRobot);
extern float RgRobotGetLife(RgRobot *pRobot);
extern int RgRobotIsDead(RgRobot *pRobot);
extern void RgRobotAccelarateRotate(RgRobot *pRobot, float rotate);
extern void RgRobotAccelarate(RgRobot *pRobot, RgVector velocity);
extern void RgRobotDash(RgRobot *pRobot, RgVector direction);
extern void RgRobotDashContinue(RgRobot *pRobot);
extern void RgRobotBreak(RgRobot *pRobot);
extern void RgRobotTargetting(RgRobot *pRobot);
extern void RgRobotShot(RgRobot *pRobot, unsigned int eType);
extern RgWeaponEssence *RgWeaponGetEss(RgWeapon *weapon);
extern struct RgWeaponShotEssence *RgWeaponEssCastToShot(RgWeaponEssence *essence);
extern float RgWeaponGetShotNum(RgWeapon *weapon);
extern float RgGetFrameTime(void);

static void _dbgThinkTool(unsigned int *pEnemyType);
static unsigned int _GetWeaponTypeFlag(RgWeapon *weapon, int side, int wait);
static int _CheckShootWeapon(RgWeapon *weapon, unsigned int param, float angle);

#define RG_PI 3.1415927f
#define RG_ENEMY_NO_WEAPON 0x10000

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
    int level = pCtrl->enemyType & 3;
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
    RgRobot *pRobot;
    RgRobot *pEnemy;
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
    pRobot = pCtrl->pRobot;
    pEnemy = pCtrl->pEnemyRobot;
    pGeom = RgRobotGetGeom(pRobot);
    pEnemyGeom = RgRobotGetGeom(pEnemy);
    __RgGeomPointGetPos(pGeom, position, D_00A51D90, 427);
    __RgGeomPointGetPos(pEnemyGeom, enemyPosition, D_00A51D90, 428);
    XrgSubVector(direction, enemyPosition, position);
    direction[1] = 0.0f;
    direction[3] = 0.0f;
    rotate = RgGeomRobotGetRotate(pGeom);
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
    if (pCtrl->enemyType & 0x1000) {
        range -= 45.0f;
    }

    landing = status & 1;
    if (landing) {
        pCtrl->idleTime = 0;
    } else if (pCtrl->idleTime < 0x200) {
        pCtrl->idleTime++;
    }

    if (pCtrl->move.state == 5) {
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
        if ((pCtrl->enemyType & 0x20) && pCtrl->restTime != 0) {
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
        pCtrl->move.state = 8;
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
                diff = RgGeomRobotGetRotate(pEnemyGeom);
                diff = _RoundAngle(_RoundAngle(diff) - _RoundAngle(rotate));
                if (position[1] <= 1.51f &&
                    ((!(status & 0x100) && absAngle > RG_PI * 3 / 4 &&
                      _EnemyTime(pCtrl, 0x50, 0x28) < pJob->count) ||
                     ((pCtrl->enemyType & 0x10) && absAngle > RG_PI / 2 &&
                      pCtrl->move.state != 5 && diff > RG_PI / 2))) {
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
        if ((!(pCtrl->enemyType & 4) || (status & 0x100)) && pCtrl->move.state != 0 &&
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
            if (landing && (absAngle < RG_PI / 2 || (int)(pCtrl->enemyType & 3) >= 2)) {
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
    if ((pCtrl->enemyType & 0x100) && state != 6 && state != 0 && state != 8) {
        cosine = _GetCosine(pCtrl->lastVelocity, velocity);
        if (state != 7 && _Abs(cosine) < 0.5f && speed < 0.2f) {
            state = 7;
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
                    ((pCtrl->enemyType & 0x20) || (enemyStatus & 2))) {
                    if ((pCtrl->enemyType & 0x80) && (enemyStatus & 2)) {
                        if (pCtrl->shot.state == 0 && absAngle < RG_PI / 8) {
                            pJob->param ^= 1;
                        }
                        pJob->flags |= 1;
                    } else if (distance < 5.0f && !(pCtrl->enemyType & 0x1000)) {
                        /* A random number is drawn and discarded. */
                        XrgRandInt();
                        if (pCtrl->enemyType & 8) {
                            pJob->flags |= 1;
                        }
                    } else if ((pCtrl->enemyType & 0x40) || !(pJob->flags & 4)) {
                        diff = RgGeomRobotGetRotate(pEnemyGeom);
                        diff = _RoundAngle(_RoundAngle(diff) - _RoundAngle(rotate));
                        if (_Abs(diff) < RG_PI * 3 / 8) {
                            if ((pCtrl->enemyType & 8) || !(absAngle < RG_PI / 8)) {
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
                    if ((pCtrl->enemyType & 8) && absAngle < RG_PI / 16) {
                        pJob->flags |= 1;
                    }
                    state = 5;
                    wait = _EnemyTime(pCtrl, 0x3C, 0x1E);
                } else {
                    if ((pCtrl->enemyType & 0x80) && (enemyStatus & 2)) {
                        state = 5;
                        pJob->param ^= 1;
                        wait = _EnemyTime(pCtrl, 0x1E, 0xF) + 60;
                    } else {
                        unsigned int closeRange = enemyStatus & 0x40;

                        if (!closeRange && distance < range + 20.0f) {
                            wait = _EnemyTime(pCtrl, 0x32, 0x19);
                            state = 2;
                        } else if (!(enemyStatus & 2) && distance < range + 40.0f &&
                                   !(pCtrl->enemyType & 0x1000)) {
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
                            if (pCtrl->enemyType & 8) {
                                pJob->flags |= 1;
                            }
                        }
                    }
                    if ((pCtrl->enemyType & 8) && absAngle < RG_PI / 16) {
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
    if ((!(pCtrl->enemyType & 4) || (status & 0x100)) && pCtrl->turn.state != 0) {
        state = 0;
        wait = 0;
        pJob->flags &= ~3;
    }

    move[0] = move[2] = 0.0f;
    switch (state) {
    case 1:
        move[0] = direction[0];
        move[2] = direction[2];
        if (wait <= 0) {
            state = 0;
        }
        break;
    case 2:
        move[0] = -direction[0];
        move[2] = -direction[2];
        if (pCtrl->enemyType & 0x1000) {
            state = 1;
        }
        if (wait <= 0) {
            state = 0;
        }
        break;
    case 3:
        move[0] = xglSin(rotate);
        move[2] = xglCos(rotate);
        if (wait <= 0) {
            state = 0;
        }
        break;
    case 4:
        move[0] = -xglSin(rotate);
        move[2] = -xglCos(rotate);
        if (wait <= 0) {
            state = 0;
        }
        break;
    case 5:
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
    case 6:
        if (wait <= 0) {
            state = 0;
        }
        break;
    case 7:
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
    if (pJob->flags & 8) {
        RgRobotDash(pRobot, move);
        pJob->flags = (pJob->flags & ~9) | 0x10;
    } else if (pJob->flags & 2) {
        RgRobotBreak(pRobot);
        pJob->flags &= ~3;
    } else {
        move[1] = move[3] = 0.0f;
        if ((status & 0x104) == 4 && state != 7 &&
            (((enemyStatus & 2) && !(status & 2) && wait <= 0 && !(pJob->flags & 0x10)) ||
             ((pJob->flags & 0x10) && _EnemyTime(pCtrl, 0x50, 0x28) < pCtrl->boostTime))) {
            RgRobotDashContinue(pRobot);
        }
        if ((pJob->flags & 1) && (wait <= 0 || (pCtrl->enemyType & 8))) {
            RgRobotDash(pRobot, move);
            pJob->flags &= ~1;
        } else if ((status & 0x100) &&
                   ((absAngle < RG_PI / 4 && !(enemyStatus & 2)) || !(enemyStatus & 1) ||
                    distance > 150.0f) &&
                   state != 7) {
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
        if ((4 - (pCtrl->enemyType & 3)) * 4 < pJob->param) {
            state = 0;
        }
        break;
    }

    if (state == 0 && !(status & 0x100)) {
        if (pCtrl->move.state != 7 &&
            ((status & 2) ||
             ((absAngle < RG_PI / 8 || !(pCtrl->enemyType & 0x400)) &&
              absAngle < RG_PI * 3 / 8)) &&
            (landing || (pCtrl->enemyType & 0x200))) {
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
            if (!(pCtrl->enemyType & 0x1000) && (weaponFlags[0] & 0x8060) &&
                (weaponFlags[1] & 0x8060) && (weaponFlags[2] & 0x8060)) {
                pCtrl->enemyType |= 0x1000;
            }
            if ((allFlags & 0x40) && (enemyStatus & 0x42) && landing && distance > 16.0f &&
                distance < 70.0f && !(enemyStatus & 0x80) &&
                RgRobotGetLife(pRobot) > RgRobotGetLife(pEnemy) + 10.0f &&
                absAngle < RG_PI / 8) {
                if ((weaponFlags[0] & 0x40) && relAngle > 0.0f) {
                    pJob->flags = 0;
                } else if ((weaponFlags[1] & 0x40) && relAngle < 0.0f) {
                    pJob->flags = 1;
                } else if (weaponFlags[2] & 0x40) {
                    pJob->flags = 2;
                } else if (weaponFlags[0] & 0x40) {
                    pJob->flags = 0;
                } else {
                    pJob->flags = 1;
                }
                wait += _EnemyTime(pCtrl, 0x320, 0x190) + 200;
                state = 4;
                pJob->param = 0;
            } else if ((allFlags & 0x20) && distance < 15.0f && absAngle < RG_PI / 4 &&
                       !(enemyStatus & 0x40)) {
                if ((weaponFlags[0] & 0x20) && relAngle > 0.0f) {
                    pJob->flags = 0;
                } else if ((weaponFlags[1] & 0x20) && relAngle < 0.0f) {
                    pJob->flags = 1;
                } else if (weaponFlags[2] & 0x20) {
                    pJob->flags = 2;
                } else if (weaponFlags[0] & 0x20) {
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
                if ((pCtrl->enemyType & 0x800) && (int)weaponFlags[0] < 0x20 &&
                    (int)weaponFlags[1] < 0x20) {
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
                    if (pCtrl->enemyType & 0x400) {
                        pJob->param |= 4;
                    }
                }
                if (!found) {
                    if ((int)weaponFlags[0] < 0x20 && relAngle > RG_PI / 32) {
                        pJob->flags = 0;
                    } else if ((int)weaponFlags[1] < 0x20 && relAngle < -RG_PI / 32) {
                        pJob->flags = 1;
                    } else if ((int)weaponFlags[2] < 0x20) {
                        pJob->flags = 2;
                    } else if ((int)weaponFlags[0] < 0x20) {
                        pJob->flags = 0;
                    } else if ((int)weaponFlags[1] < 0x20) {
                        pJob->flags = 1;
                    }
                    if (pJob->flags != RG_ENEMY_NO_WEAPON) {
                        if (weaponFlags[pJob->flags] & 1) {
                            pJob->param |= 2;
                        }
                        if (weaponFlags[pJob->flags] & 4) {
                            pJob->param |= 8;
                        }
                        if (pCtrl->enemyType & 0x400) {
                            pJob->param |= 4;
                        }
                    }
                }
            }
            if (pJob->flags == RG_ENEMY_NO_WEAPON) {
                pJob->flags = lastWeapon;
                state = 0;
            } else {
                if ((int)weaponFlags[pJob->flags] < 0x20 && (pJob->param & 4) &&
                    weapons[pJob->flags] != 0) {
                    RgWeaponEssence *essence = RgWeaponGetEss(weapons[pJob->flags]);

                    if ((pCtrl->enemyType & 0x400) && distance > 25.0f) {
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

    if (_GetPadRelease() & 0x100) {
        _dbgThinkTool(&pCtrl->enemyType);
    }
}

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

extern const char D_00A51D90[]; /* "../rg_enemy.euc.c" */
extern const char D_00A51DD0[]; /* "pBaka != NIL" */

static void _DestructEnemy(RgEnemyControl *pBaka)
{
    if (pBaka == 0) {
        assert_prog(D_00A51DD0, D_00A51D90, 1562);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_enemy", _InitEnemy);

extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size,
                         const char *source_file, int line);

extern const char D_00A51DE0[]; /* "pRobot != NIL" */
extern const char D_00A51DF0[]; /* "pEnemyRobot != NIL" */

/*
 * _InitEnemy is defined later in this TU (a local sibling still in asm).
 * enemyType is forwarded to it untouched and unasserted; no stronger
 * evidence for its role exists within this allocation.
 */
static void _InitEnemy(RgEnemyControl *pControl, RgRobot *pRobot,
                       RgRobot *pEnemyRobot, int enemyType);

RgEnemyControl *CreateRgEnemyControl(RgRobot *pRobot, RgRobot *pEnemyRobot,
                                     int enemyType)
{
    RgEnemyControl *pControl;

    if (pRobot == 0) {
        assert_prog(D_00A51DE0, D_00A51D90, 1612);
    }
    if (pEnemyRobot == 0) {
        assert_prog(D_00A51DF0, D_00A51D90, 1613);
    }
    pControl = RgHeapAlloc(InstanceOfRgHeap(), RG_ENEMY_CONTROL_SIZE,
                           D_00A51D90, 1615);
    _InitEnemy(pControl, pRobot, pEnemyRobot, enemyType);
    return pControl;
}
