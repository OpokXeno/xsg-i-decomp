/*
 * OV12 original TU 44: 0x00a26a00..0x00a26c78 (3 functions)
 */
#include "common.h"
#include "rg_disp.h"

extern RgWeaponEssence *RgWeaponGetEss(RgWeapon *weapon);
extern unsigned int RgWeaponGetRestUnit(RgWeapon *weapon);
extern float RgWeaponGetShotNum(RgWeapon *weapon);

/* ov12:0x00a54970 "%d/%d" */
const char D_00A54970[] = "%d/%d";
/* ov12:0x00a54978 "%d%%" */
const char D_00A54978[] = "%d%%";

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
    unsigned int uvwh[4];
} RgWpnPicEntry;

static RgWpnPicEntry s_aWepPicTbl_0[36] = {
    {"SWD21AG", {0}, {0, 0, 64, 16}},
    {"LG24VX", {0}, {0, 16, 64, 16}},
    {"HG45VX", {0}, {0, 32, 64, 16}},
    {"LG10AG", {0}, {0, 48, 64, 16}},
    {"SMG99AG", {0}, {0, 64, 64, 16}},
    {"GRD20AG", {0}, {0, 80, 64, 16}},
    {"HMP33AG", {0}, {64, 0, 64, 16}},
    {"PB55AG", {0}, {64, 16, 64, 16}},
    {"SHD02AG", {0}, {64, 32, 64, 16}},
    {"SHD12VX", {0}, {64, 48, 64, 16}},
    {"LG100VX", {0}, {64, 64, 64, 16}},
    {"SWD34AG", {0}, {64, 80, 64, 16}},
    {"HMR55AG", {0}, {64, 96, 64, 16}},
    {"CB85VX", {0}, {64, 112, 64, 16}},
    {"ECM1-VX", {0}, {64, 128, 64, 16}},
    {"ER-VX", {0}, {64, 144, 64, 16}},
    {"BMP45VX", {0}, {128, 0, 64, 16}},
    {"HG75VX", {0}, {128, 16, 64, 16}},
    {"GLG76AG", {0}, {128, 32, 64, 16}},
    {"FLM64AG", {0}, {128, 48, 64, 16}},
    {"DLC02AG4", {0}, {128, 64, 64, 16}},
    {"SHB67AG", {0}, {128, 80, 64, 16}},
    {"BSW13AG", {0}, {128, 96, 64, 16}},
    {"BL24AG", {0}, {128, 112, 64, 16}},
    {"AXE11AG", {0}, {128, 128, 64, 16}},
    {"SMP53AG", {0}, {128, 144, 64, 16}},
    {"SMG32VX", {0}, {128, 160, 64, 16}},
    {"ECM2-VX", {0}, {128, 176, 64, 16}},
    {"LM11VX", {0}, {192, 0, 64, 16}},
    {"BA15VX", {0}, {192, 16, 64, 16}},
    {"AIRD-AG2", {0}, {192, 32, 64, 16}},
    {"BMP-AG5", {0}, {192, 48, 64, 16}},
    {"HMP-AG5", {0}, {192, 64, 64, 16}},
    {"HGG-AG5", {0}, {192, 80, 64, 16}},
    {"LC-AG6", {0}, {192, 96, 64, 16}},
    {"LW-VX4", {0}, {192, 112, 64, 16}},
};

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
const char D_00A54AB0[] = "%d k";

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
