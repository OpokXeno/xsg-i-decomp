#ifndef INCLUDE_OV12_RG_ROBOT_CONTROL_H
#define INCLUDE_OV12_RG_ROBOT_CONTROL_H

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

RgRobotControl *CreateRgRobotControlNul(RgRobot *pRobot);

#endif /* INCLUDE_OV12_RG_ROBOT_CONTROL_H */
