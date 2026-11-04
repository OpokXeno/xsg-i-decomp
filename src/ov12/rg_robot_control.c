/*
 * OV12 original TU 5: 0x00a09480..0x00a09ba0 (9 functions)
 */
#include "common.h"
#include "rg_robot_control.h"
#include "init_xrg_input.h"
#include "ov12/rg_camera.h"
#include "ov12/rg_equip_type.h"
#include "ov12/rg_geom_robot.h"
#include "ov12/rg_get_frame_time.h"
#include "ov12/rg_heap.h"
#include "ov12/assert_prog.h"
#include "ov12/xrg_rand_int.h"
#include "ov12/rg_robot.h"

/* Action bits of XrgInput.flags without a name in init_xrg_input.h: both
 * sticks pushed apart (brake) and pushed together (target). */
#define RG_CONTROL_INPUT_BREAK 0x20
#define RG_CONTROL_INPUT_TARGETTING 0x40


static void _InitControlInput(RgControlInput *pInput, RgStatus *pRobot,
                              struct RgCamera *pCamera, int padId);
static void _jobControlInput(RgRobotControl *pControl);

const char robotControlAssertControlNotNil[] = "pControl != NIL";
const char robotControlSourcePath[] = "../rg_robot_control.euc.c";
const char robotControlAssertCameraNotNil[] = "pCam != NIL";

static void _jobControlInput(RgRobotControl *pControl)
{
    RgControlInput *controlInput;
    XrgInput *input;
    RgStatus *robot;
    struct RgCamera *camera;
    unsigned int flags;
    RgVector firstStick;
    RgVector secondStick;
    RgVector movement;
    RgMatrix cameraMatrix;
    RgVector dashDirection;
    RgGeomPoint *geometry;
    float firstSine;
    float secondSine;
    float secondCosine;
    float firstMagnitude;
    float secondMagnitude;
    float cosineDifference;
    float heading;
    float rotation;
    float absoluteDifference;
    float absoluteRotation;
    int hasMovement;

    controlInput = (RgControlInput *)pControl;
    input = controlInput->buffer;
    robot = pControl->robot;
    camera = controlInput->camera;
    flags = input->flags;
    hasMovement = 0;
    rotation = 0.0f;

    XrgInputDeviceCheck(input, RgGetFrameTime());

    XrgClearVector(firstStick);
    firstSine = sinf(input->angle[0]);
    firstStick[2] = cosf(input->angle[0]);
    firstStick[0] = firstSine;

    XrgClearVector(secondStick);
    secondSine = sinf(input->angle[1]);
    secondCosine = cosf(input->angle[1]);
    secondStick[0] = secondSine;
    secondStick[2] = secondCosine;

    firstMagnitude = input->magnitude[0];
    secondMagnitude = input->magnitude[1];
    XrgClearVector(movement);
    movement[0] = (firstStick[0] * firstMagnitude) +
                   (secondStick[0] * secondMagnitude);
    movement[2] = (firstStick[2] * firstMagnitude) +
                   (secondStick[2] * secondMagnitude);

    if (XrgNormalizeVector(movement, movement) > 0.5f) {
        hasMovement = 1;
        RgCameraLocal(camera, cameraMatrix);
        XrgApplyVector(movement, cameraMatrix, movement);
        movement[1] = rotation;
    }

    if (hasMovement != 0) {
        RgRobotAccelarate(robot, movement);
    }

    if ((flags & XRG_INPUT_FLAG_STICK_CLICK) != 0) {
        if (hasMovement != 0) {
            RgRobotDash(robot, movement);
        } else {
            geometry = RgRobotGetGeom(robot);
            heading = RgGeomRobotGetRotate(&geometry->geom);
            XrgSetVectorXYZ(dashDirection, sinf(heading), 0.0f,
                            cosf(heading));
            RgRobotDash(robot, dashDirection);
        }
    }
    if ((flags & XRG_INPUT_FLAG_BOTH_STICK_CLICKS) != 0) {
        RgRobotDashContinue(robot);
    }

    if ((firstMagnitude > 0.0f) && (secondMagnitude > 0.0f)) {
        cosineDifference = firstStick[2] - secondStick[2];
        absoluteDifference = cosineDifference;
        if (cosineDifference < 0.0f) {
            absoluteDifference = -cosineDifference;
        }
        if (absoluteDifference > 0.7853982f) {
            rotation = cosineDifference;
        }
    }
    absoluteRotation = rotation;
    if (rotation < 0.0f) {
        absoluteRotation = -rotation;
    }
    if (absoluteRotation > 0.0f) {
        RgRobotAccelarateRotate(robot, rotation);
    }

    if ((flags & XRG_INPUT_FLAG_LEFT_SHOULDER) != 0) {
        RgRobotShot(robot, RG_EQUIP_TYPE_LEFT);
    }
    if ((flags & XRG_INPUT_FLAG_RIGHT_SHOULDER) != 0) {
        RgRobotShot(robot, RG_EQUIP_TYPE_RIGHT);
    }
    if ((flags & XRG_INPUT_FLAG_BOTH_SHOULDERS) != 0) {
        RgRobotShot(robot, RG_EQUIP_TYPE_BACK);
    }
    if ((flags & RG_CONTROL_INPUT_BREAK) != 0) {
        RgRobotBreak(robot);
    }
    if ((flags & RG_CONTROL_INPUT_TARGETTING) != 0) {
        RgRobotTargetting(robot);
    }
    if ((flags & XRG_INPUT_FLAG_LEFT_PAIR) != 0) {
        RgRobotDropWeapon(robot, RG_EQUIP_TYPE_LEFT);
    }
    if ((flags & XRG_INPUT_FLAG_RIGHT_PAIR) != 0) {
        RgRobotDropWeapon(robot, RG_EQUIP_TYPE_RIGHT);
    }
    if ((flags & XRG_INPUT_FLAG_BOTH_PAIRS) != 0) {
        RgRobotDropWeapon(robot, RG_EQUIP_TYPE_BACK);
    }
}

void RgRobotControlSetRobot(RgRobotControl *pControl, RgStatus *pRobot)
{
    if (pControl == 0) {
        assert_prog(robotControlAssertControlNotNil, robotControlSourcePath, 175);
    }
    pControl->robot = pRobot;
}

static void _DestructControlInput(RgRobotControl *pControl)
{
    RgControlInput *pInput;

    if (pControl == 0) {
        assert_prog(robotControlAssertControlNotNil, robotControlSourcePath, 185);
    }
    pInput = (RgControlInput *)pControl;
    RgHeapFree(InstanceOfRgHeap(), pInput->buffer, robotControlSourcePath, 186);
}

void InitRgRobotControlCommon(RgRobotControl *pControl, RgStatus *pRobot)
{
    if (pControl == 0) {
        assert_prog(robotControlAssertControlNotNil, robotControlSourcePath, 195);
    }
    pControl->robot = pRobot;
    pControl->jobMethod = 0;
    pControl->destructMethod = 0;
}

static void _InitControlInput(RgControlInput *pInput, RgStatus *pRobot,
                              struct RgCamera *pCamera, int padId)
{
    void *pBuffer;

    if (pInput == 0) {
        assert_prog(robotControlAssertControlNotNil, robotControlSourcePath, 204);
    }
    if (pCamera == 0) {
        assert_prog(robotControlAssertCameraNotNil, robotControlSourcePath, 205);
    }
    InitRgRobotControlCommon(&pInput->control, pRobot);
    pInput->control.destructMethod = _DestructControlInput;
    pInput->control.jobMethod = _jobControlInput;
    pBuffer = RgHeapAlloc(InstanceOfRgHeap(), sizeof(XrgInput), robotControlSourcePath, 213);
    pInput->buffer = pBuffer;
    InitXrgInput(pBuffer, padId);
    pInput->camera = pCamera;
}

RgRobotControl *CreateRgRobotControlNul(RgStatus *pRobot)
{
    RgRobotControl *pControl;

    pControl = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgRobotControl), robotControlSourcePath, 224);
    if (pControl == 0) {
        assert_prog(robotControlAssertControlNotNil, robotControlSourcePath, 225);
    }
    InitRgRobotControlCommon(pControl, pRobot);
    return pControl;
}

RgRobotControl *CreateRgRobotControlInput(RgStatus *pRobot, struct RgCamera *pCamera,
                                          int padId)
{
    RgControlInput *pInput;

    pInput = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgControlInput), robotControlSourcePath,
                         233);
    if (pInput == 0) {
        assert_prog(robotControlAssertControlNotNil, robotControlSourcePath, 234);
    }
    _InitControlInput(pInput, pRobot, pCamera, padId);
    return &pInput->control;
}

void DisposeRgRobotControl(RgRobotControl *pControl)
{
    if (pControl == 0) {
        assert_prog(robotControlAssertControlNotNil, robotControlSourcePath, 242);
    }
    if (pControl->destructMethod != 0) {
        pControl->destructMethod(pControl);
    }
    RgHeapFree(InstanceOfRgHeap(), pControl, robotControlSourcePath, 245);
}

void RgRobotControlJob(RgRobotControl *pControl)
{
    if (pControl == 0) {
        assert_prog(robotControlAssertControlNotNil, robotControlSourcePath, 253);
    }
    if (pControl->jobMethod != 0) {
        pControl->jobMethod(pControl);
    }
}
