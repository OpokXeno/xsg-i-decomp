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

extern int strcasecmp(const char *left, const char *right);

/*
 * s_aWepPicTbl_0 (ov12:0x00a4fc00, size 0x480, scaffold-owned .rodata) holds
 * 36 32-byte entries: a weapon-name pointer RgDispWpnDat_GetNameUVWH matches
 * with strcasecmp, followed by a 16-byte, 16-byte-aligned block it copies
 * whole with a COP2 quadword transfer (evidenced size/alignment only; no
 * caller dereferences its fields yet).
 */
typedef struct {
    const char *name;
    unsigned char unmodeled_04[12];
    u64 uvwh[2];
} RgWpnPicEntry;

extern RgWpnPicEntry s_aWepPicTbl_0[36];

int RgDispWpnDat_GetNameUVWH(const char *name, u64 *uvwh)
{
    unsigned int i;

    for (i = 0; i < 36; i++) {
        if (strcasecmp(s_aWepPicTbl_0[i].name, name) == 0) {
            __asm__ __volatile__(
                "lqc2 $vf31, 0(%1)\n\t"
                "sqc2 $vf31, 0(%0)\n\t"
                : : "r"(uvwh), "r"(s_aWepPicTbl_0[i].uvwh) : "memory");
            return 1;
        }
    }
    return 0;
}

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
