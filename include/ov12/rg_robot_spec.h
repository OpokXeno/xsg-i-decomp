#ifndef INCLUDE_OV12_RG_ROBOT_SPEC_H
#define INCLUDE_OV12_RG_ROBOT_SPEC_H

#include "shared.h"

/* Values of RgRobotSpec.type, set from the "dash-mode" keyword. */
#define RG_DASH_MODE_NORMAL 0
#define RG_DASH_MODE_VR 1
#define RG_DASH_MODE_NEO 2
#define RG_DASH_MODE_NEO2 3
#define RG_DASH_MODE_NEO3 4
#define RG_DASH_MODE_NUM 5

/* RgAutoHoming.flags, one bit per flag keyword. */
#define RG_AUTO_HOMING_SEE_TARGET 1
#define RG_AUTO_HOMING_LOCK_ON 2
#define RG_AUTO_HOMING_SHOT 4
#define RG_AUTO_HOMING_IN_SCRN 8

/* One auto-homing entry: a turn value and its flag keywords ORed together. */
typedef struct RgAutoHoming {
    float value;
    unsigned int flags;
} RgAutoHoming;

#define RG_AUTO_HOMING_ENTRY_NUM 2

/*
 * Tuning record of one robot, parsed by RgRobotSpecReadFromText from the
 * character text (keywords in the comments) and read by rg_robot.c.
 */
typedef struct RgRobotSpec {
    int charID;                      /* +0x00 */
    int type;                        /* +0x04: "dash-mode" (RG_DASH_MODE_*) */
    unsigned char unmodeled_08[4];   /* +0x08 */
    float moveSpeedMax;              /* +0x0c: "move-spd-max" */
    float dashSpeedMax;              /* +0x10: "dash-spd-max" */
    float turnRate;                  /* +0x14: 2*pi / "rot-spd-max" */
    float rotSpeedBottomRate;        /* +0x18: "rot-spd-bottom-rate" */
    float weight;                    /* +0x1c: "weight" */
    float enginePower;               /* +0x20: "engine-power"/"engine-impulse" */
    float rotateForce;               /* +0x24: from "roll-power" */
    float moveResist;                /* +0x28: "move-resist" */
    float rotResist;                 /* +0x2c: "rot-resist" */
    float dashTime;                  /* +0x30: "dash-time" */
    float dashPower;                 /* +0x34: "dash-power" */
    float dashCooldown;              /* +0x38: "dash-cooldown" */
    RgAutoHoming m_aAutoHoming[RG_AUTO_HOMING_ENTRY_NUM]; /* +0x3c */
    float life;                      /* +0x4c: "life" */
    float damageStiffTime;           /* +0x50: "damage-stiff-time" */
    float bodyAttackDamage;          /* +0x54: "body-attack-damage" */
    char jetEffect[0x20];            /* +0x58: "jet-eff" */
    char dashEffect[0x20];           /* +0x78: "dash-eff" */
    /*
     * InitRgRobotSpec copies its 8-byte default "UNARMED" here with one
     * aligned ld/sd pair (sd at 0x00a0d268), so the name shares storage
     * with a doubleword view, as RgBattleNames.weaponStorage does in
     * rg_main.c; sudeWords is never read on its own.
     */
    union {
        char sude[0x20];
        unsigned long long sudeWords[4];
    };                               /* +0x98 */
} RgRobotSpec;

struct RgReadText;

/* Functions and data used by other translation units. */
void InitRgRobotSpec(RgRobotSpec *pSpec);
void RgRobotSpecReadFromText(RgRobotSpec *pSpec, struct RgReadText *pReader);
void RgRobotSpecDump(RgRobotSpec *pSpec);

#endif /* INCLUDE_OV12_RG_ROBOT_SPEC_H */
