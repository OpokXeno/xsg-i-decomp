/*
 * OV12 original TU 44: 0x00a26a00..0x00a26c78 (3 functions)
 */
#include "common.h"
#include "rg_disp.h"

extern RgWeaponEssence *RgWeaponGetEss(RgWeapon *weapon);
extern unsigned int RgWeaponGetRestUnit(RgWeapon *weapon);
extern float RgWeaponGetShotNum(RgWeapon *weapon);

/* ov12:0x00a54970 "%d/%d" */
extern const char D_00A54970[];
/* ov12:0x00a54978 "%d%%" */
extern const char D_00A54978[];

int RgDispWpnDat_CreateRestNumStr(RgWeapon *weapon, char *buffer)
{
    RgWeaponEssence *ess;
    unsigned int restUnit;
    float shotNum;
    float capacity;

    ess = RgWeaponGetEss(weapon);
    restUnit = RgWeaponGetRestUnit(weapon);
    shotNum = RgWeaponGetShotNum(weapon);
    capacity = ess->capacity;
    switch (restUnit) {
    case 0:
        return sprintf(buffer, D_00A54970, (int) shotNum, (int) capacity);
    case 1:
        return sprintf(buffer, D_00A54978, (int) (shotNum / capacity * 100.0f));
    case 2:
        *buffer = 0;
        break;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp", RgDispWpnDat_GetNameUVWH);

extern RgGeomPoint *RgRobotGetGeom(RgStatus *robot);
extern void RgGeomPointGetVel(RgGeomPoint *point, RgVector velocity);
extern float XrgLengthVector(RgVector vector);
extern int XrgRandInt(void);

/* ov12:0x00a54ab0 "%d k" */
extern const char D_00A54AB0[];

void RgDispWpnDat_CreateSpeedStr(RgStatus *robot, char *buffer)
{
    RgVector velocity;
    int speed;

    if (robot == 0) {
        *buffer = 0;
        return;
    }
    RgGeomPointGetVel(RgRobotGetGeom(robot), velocity);
    velocity[1] = 0.0f;
    speed = (int) ((float) (int) XrgLengthVector(velocity) * 6.0f * 6.0f / 10.0f);
    if (speed > 0) {
        speed = speed + (XrgRandInt() & 3) - 1;
    }
    sprintf(buffer, D_00A54AB0, speed);
}
