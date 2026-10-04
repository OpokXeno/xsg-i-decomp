/*
 * OV12 original TU 5: 0x00a09480..0x00a09ba0 (9 functions)
 */
#include "common.h"
#include "rg_robot_control.h"
#include "ov12/rg_camera.h"

const char D_00A51C20[] = "pControl != NIL";
const char D_00A51C30[] = "../rg_robot_control.euc.c";
const char D_00A51C50[] = "pCam != NIL";

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(RgHeap *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgHeapFree(RgHeap *heap, void *pointer, const char *source_file,
                       int line);

/*
 * External file-backed witnesses, not candidate-emitted data: this window is
 * asm-owned scaffold data (splat names, no config/symbols/ov12.txt entry).
 *
 * ov12:0x00a51c20 contains the assertion expression "pControl != NIL".
 * ov12:0x00a51c30 contains the source filename "../rg_robot_control.euc.c".
 */
extern const char D_00A51C20[];
extern const char D_00A51C30[];

/*
 * ov12:0x00a51c50 contains the assertion expression "pCam != NIL", the same
 * asm-owned scaffold data window as above; _InitControlInput reuses it as a
 * generic non-null check on its pEssence argument.
 */
extern const char D_00A51C50[];

extern void InitXrgInput(void *pBuffer, int padId);

static void _InitControlInput(RgControlInput *pInput, RgRobot *pRobot,
                              void *pEssence, int padId);
static void _jobControlInput(RgRobotControl *pControl);

/* The input handler reads the two stick angles and magnitudes and the action
 * flags from the buffer allocated by _InitControlInput (0x00a09968). */
typedef struct RgControlInputState {
    int padId;
    float angle[2];
    float magnitude[2];
    unsigned int flags;
} RgControlInputState;

extern float cosf(float angle);
extern float RgGetFrameTime(void);
extern RgGeomPoint *RgRobotGetGeom(RgRobot *pRobot);
extern float RgGeomRobotGetRotate(RgGeomPoint *point);
extern void RgCameraLocal(struct RgCamera *pCam, RgMatrix matrix);
extern void RgRobotAccelarate(RgRobot *pRobot, RgVector velocity);
extern void RgRobotAccelarateRotate(RgRobot *pRobot, float rotate);
extern void RgRobotBreak(RgRobot *pRobot);
extern void RgRobotDash(RgRobot *pRobot, RgVector direction);
extern void RgRobotDashContinue(RgRobot *pRobot);
extern void RgRobotDropWeapon(RgRobot *pRobot, unsigned int eType);
extern void RgRobotShot(RgRobot *pRobot, unsigned int eType);
extern void RgRobotTargetting(RgRobot *pRobot);
extern void XrgApplyVector(RgVector destination, RgMatrix matrix,
                           RgVector source);
extern void XrgInputDeviceCheck(void *pInput, float frameTime);
extern float XrgNormalizeVector(RgVector destination, RgVector source);
extern void XrgSetVectorXYZ(RgVector destination, float x, float y, float z);

static void _jobControlInput(RgRobotControl *pControl)
{
    RgControlInput *controlInput;
    RgControlInputState *input;
    RgRobot *robot;
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
    input = (RgControlInputState *)controlInput->buffer;
    robot = pControl->robot;
    camera = controlInput->essence;
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

    if ((flags & 0x80) != 0) {
        if (hasMovement != 0) {
            RgRobotDash(robot, movement);
        } else {
            geometry = RgRobotGetGeom(robot);
            heading = RgGeomRobotGetRotate(geometry);
            XrgSetVectorXYZ(dashDirection, sinf(heading), 0.0f,
                            cosf(heading));
            RgRobotDash(robot, dashDirection);
        }
    }
    if ((flags & 0x100) != 0) {
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

    if ((flags & 0x1) != 0) {
        RgRobotShot(robot, 0);
    }
    if ((flags & 0x2) != 0) {
        RgRobotShot(robot, 1);
    }
    if ((flags & 0x4) != 0) {
        RgRobotShot(robot, 2);
    }
    if ((flags & 0x20) != 0) {
        RgRobotBreak(robot);
    }
    if ((flags & 0x40) != 0) {
        RgRobotTargetting(robot);
    }
    if ((flags & 0x200) != 0) {
        RgRobotDropWeapon(robot, 0);
    }
    if ((flags & 0x400) != 0) {
        RgRobotDropWeapon(robot, 1);
    }
    if ((flags & 0x800) != 0) {
        RgRobotDropWeapon(robot, 2);
    }
}

void RgRobotControlSetRobot(RgRobotControl *pControl, RgRobot *pRobot)
{
    if (pControl == 0) {
        assert_prog(D_00A51C20, D_00A51C30, 175);
    }
    pControl->robot = pRobot;
}

static void _DestructControlInput(RgRobotControl *pControl)
{
    RgControlInput *pInput;

    if (pControl == 0) {
        assert_prog(D_00A51C20, D_00A51C30, 185);
    }
    pInput = (RgControlInput *)pControl;
    RgHeapFree(InstanceOfRgHeap(), pInput->buffer, D_00A51C30, 186);
}

void InitRgRobotControlCommon(RgRobotControl *pControl, RgRobot *pRobot)
{
    if (pControl == 0) {
        assert_prog(D_00A51C20, D_00A51C30, 195);
    }
    pControl->robot = pRobot;
    pControl->jobMethod = 0;
    pControl->destructMethod = 0;
}

static void _InitControlInput(RgControlInput *pInput, RgRobot *pRobot,
                              void *pEssence, int padId)
{
    void *pBuffer;

    if (pInput == 0) {
        assert_prog(D_00A51C20, D_00A51C30, 204);
    }
    if (pEssence == 0) {
        assert_prog(D_00A51C50, D_00A51C30, 205);
    }
    InitRgRobotControlCommon(&pInput->control, pRobot);
    pInput->control.destructMethod = _DestructControlInput;
    pInput->control.jobMethod = _jobControlInput;
    pBuffer = RgHeapAlloc(InstanceOfRgHeap(), 48, D_00A51C30, 213);
    pInput->buffer = pBuffer;
    InitXrgInput(pBuffer, padId);
    pInput->essence = pEssence;
}

RgRobotControl *CreateRgRobotControlNul(RgRobot *pRobot)
{
    RgRobotControl *pControl;

    pControl = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgRobotControl), D_00A51C30, 224);
    if (pControl == 0) {
        assert_prog(D_00A51C20, D_00A51C30, 225);
    }
    InitRgRobotControlCommon(pControl, pRobot);
    return pControl;
}

RgRobotControl *CreateRgRobotControlInput(RgRobot *pRobot, void *pEssence,
                                          int padId)
{
    RgControlInput *pInput;

    pInput = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgControlInput), D_00A51C30,
                         233);
    if (pInput == 0) {
        assert_prog(D_00A51C20, D_00A51C30, 234);
    }
    _InitControlInput(pInput, pRobot, pEssence, padId);
    return &pInput->control;
}

void DisposeRgRobotControl(RgRobotControl *pControl)
{
    if (pControl == 0) {
        assert_prog(D_00A51C20, D_00A51C30, 242);
    }
    if (pControl->destructMethod != 0) {
        pControl->destructMethod(pControl);
    }
    RgHeapFree(InstanceOfRgHeap(), pControl, D_00A51C30, 245);
}

void RgRobotControlJob(RgRobotControl *pControl)
{
    if (pControl == 0) {
        assert_prog(D_00A51C20, D_00A51C30, 253);
    }
    if (pControl->jobMethod != 0) {
        pControl->jobMethod(pControl);
    }
}
