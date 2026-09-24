/*
 * TU-local declarations of ov12/tu005 (src/ov12/rg_robot_control.c).
 */

#ifndef SRC_OV12_RG_ROBOT_CONTROL_H
#define SRC_OV12_RG_ROBOT_CONTROL_H

#include "shared.h"

typedef struct RgRobot RgRobot;
typedef struct RgRobotControl RgRobotControl;

typedef void (*RgRobotControlDestructFunc)(RgRobotControl *pControl);
typedef void (*RgRobotControlJobFunc)(RgRobotControl *pControl);

/*
 * Base object of the robot-control family. InitRgRobotControlCommon writes
 * every field (RgRobotControlSetRobot also writes the robot field on its
 * own). CreateRgRobotControlNul (ov12:0x00a099b0, outside this allocation)
 * allocates exactly these 12 bytes for its do-nothing control variant
 * (RgHeapAlloc size 12, ov12:0x00a099d8), which is the size evidence for
 * this struct. DisposeRgRobotControl reads destructMethod (lw 4($16),
 * ov12:0x00a09b04/0x00a09b0c); RgRobotControlJob reads jobMethod (lw
 * 8($16), ov12:0x00a09b7c).
 */
struct RgRobotControl {
    RgRobot *robot;                            /* +0x00 */
    RgRobotControlDestructFunc destructMethod; /* +0x04 */
    RgRobotControlJobFunc jobMethod;           /* +0x08 */
};

/*
 * Partial layout of the player-input control object CreateRgRobotControlInput
 * allocates (RgHeapAlloc size 0x20, ov12:0x00a09a68). _InitControlInput
 * (ov12:0x00a098d0, outside this allocation) embeds the RgRobotControl base
 * at offset 0, then allocates its own 0x30-byte buffer (RgHeapAlloc size 48,
 * ov12:0x00a09968) and stores it at offset 0x0C (sw v0,12(s0),
 * ov12:0x00a09984) -- the field _DestructControlInput frees (lw a1,12(s0),
 * ov12:0x00a09854). _InitControlInput separately asserts its own third
 * argument (pEssence) non-null and stores it unchanged at offset 0x10
 * (sw s3,16(s0), ov12:0x00a0998c). The object's size is the 0x20 bytes
 * CreateRgRobotControlInput allocates; no function here touches +0x14..+0x1F.
 */
typedef struct RgControlInput {
    RgRobotControl control; /* +0x00 */
    void *buffer;           /* +0x0C, freed by _DestructControlInput */
    void *essence;          /* +0x10, _InitControlInput's pEssence */
    unsigned char unmodeled_14[0x20 - 0x14];
} RgControlInput;

void RgRobotControlSetRobot(RgRobotControl *pControl, RgRobot *pRobot);
void InitRgRobotControlCommon(RgRobotControl *pControl, RgRobot *pRobot);
RgRobotControl *CreateRgRobotControlNul(RgRobot *pRobot);
RgRobotControl *CreateRgRobotControlInput(RgRobot *pRobot, void *pEssence,
                                          int padId);
void DisposeRgRobotControl(RgRobotControl *pControl);
void RgRobotControlJob(RgRobotControl *pControl);

#endif /* SRC_OV12_RG_ROBOT_CONTROL_H */
