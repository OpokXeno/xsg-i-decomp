/*
 * OV01 original TU 17: 0x00a314b8..0x00a32db8 (36 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov01/data_unit_org_get.h"

extern void MOutputDebugStringWarn(const char *format, ...);
/* The message body after the ASCII prefix is encoded in EUC-JP. */
const char D_00A50EF0[48] =
    "MCamReleaseControl: \xA5\xAB\xA5\xE1\xA5\xE9\xA4\xCF\xC0\xA9\xB8\xE6\xC3\xE6\xA4\xC7\xA4\xCF\xA4\xCA\xA4\xA4";
const char D_00A51000[40] = "MCamGetAtkRange: Invalid ID (%d)";
const char D_00A51028[40] = "MCamGetDefRange: Invalid ID (%d)";
const char zero_duration_message[40] = "MCamMoveProc_CalcRatio: d == 0 (IP:%d)";
const char invalid_interpolation_message[40] = "MCamMoveProc_CalcRatio: Invalid IPTYPE";

extern void bcopy(const void *source, void *destination, unsigned int count);
/*
 * Linear interpolation state shared by the per-parameter MoveProc callbacks
 * (mode/duration/elapsed feed MCamMoveProc_CalcRatio; start/end/current hold
 * the eased scalar value).
 */
typedef struct MCamMoveState {
    unsigned int mode;
    unsigned int duration;
    unsigned int elapsed;
    float start;
    float end;
    float current;
} MCamMoveState;

/*
 * MCamInit clears this complete 0x5B0-byte camera-parameter store. The
 * recovered float and perspective-move views are interior fields; the other
 * bytes remain part of the live store but their individual meanings are not
 * recovered yet.
 */
typedef struct MCamParameterStorage {
    unsigned char unmodeled_000[0x1A0];
    float actorLength[2];
    unsigned char unmodeled_1A8[0x4C];
    float perspectiveStart;
    unsigned char unmodeled_1F8[0x380];
    MCamMoveState perspectiveMove;
    unsigned char unmodeled_590[0x20];
} MCamParameterStorage;

static void *sysCam;
/* MCamTakeControl saves the StudioCamera returned by xglStudioGetCamera2. */
static StudioCamera sysCamBuff;
static int camFlags;
static MCamParameterStorage camParams;

/* Original local labels are interior views in camParams, not extra storage. */
#define D_00A5B220 (camParams.actorLength[0])
#define D_00A5B224 (camParams.actorLength[1])
#define D_00A5B5F8 (camParams.perspectiveMove)

#define CAM_FLAG_ACTIVE 0x1
#define CAM_FLAG_DUMP_DISABLED 0x10
#define CAM_SHAKE_BIT 12

void *MCamGetSysCam(void)
{
    return sysCam;
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamGetCurrentBaseCoord);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamGetCurrentCoord);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamGetCurrentAngle);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamGetCurrentEye);

float MCamGetLengthActor(int selector)
{
    float length = 0.0f;

    switch (selector) {
    case 0:
        length = D_00A5B220;
        break;
    case 1:
        length = D_00A5B224;
        break;
    }
    return length;
}

void MCamInit(void)
{
    camFlags = 0;
    memset(&camParams, 0, sizeof(camParams));
}

static void MCamExec_Control(void);
static void MCamExec_SetParams(void);
static void MCamExec_DispParams(void);

void MCamExec(void)
{
    if (camFlags & CAM_FLAG_ACTIVE) {
        MCamExec_Control();
        MCamExec_SetParams();
        MCamExec_DispParams();
    }
}

void MCamDumpDisabled(short disable)
{
    if (disable != 0) {
        camFlags |= CAM_FLAG_DUMP_DISABLED;
    } else {
        camFlags &= ~CAM_FLAG_DUMP_DISABLED;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamTakeControl);

int MCamReleaseControl(void)
{
    if (!(camFlags & CAM_FLAG_ACTIVE)) {
        MOutputDebugStringWarn(D_00A50EF0);
        return 0;
    }

    bcopy(&sysCamBuff, sysCam, sizeof(sysCamBuff));
    camFlags &= ~CAM_FLAG_ACTIVE;
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamStoreParams);

int MCamIsActive(void)
{
    return camFlags & CAM_FLAG_ACTIVE;
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamSet);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamMove);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamStopMove);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamIsMoving);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamShake);

int MCamIsShaking(void)
{
    return (camFlags >> CAM_SHAKE_BIT) & 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamGetActorCoord);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamCalcBaseCoord);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamCalcCoord);

static float MCamGetAtkRange(int id)
{
    UnitInitData *unit = dataUnitInitGet(id);

    if (unit == 0) {
        MOutputDebugStringWarn(D_00A51000, id);
        return 0.0f;
    }
    return (float)unit->atkRange / 100.0f;
}

static float MCamGetDefRange(int id)
{
    UnitInitData *unit = dataUnitInitGet(id);

    if (unit == 0) {
        MOutputDebugStringWarn(D_00A51028, id);
        return 0.0f;
    }
    return (float)unit->defRange / 100.0f;
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamExec_Control);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamExec_SetParams);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamExec_SetParams_Base);

static void MCamExec_DispParams(void)
{
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamMove_Coord);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamMove_Bank);

/*
 * The caller's own move-request object (its definer is outside this
 * allocation, called through MCamMove, also outside this allocation): only
 * the three fields MCamMove_Pers reads are evidenced.
 */
typedef struct MCamMoveRequest {
    unsigned char unmodeled_00[0xA4];
    float angle;
    unsigned char unmodeled_a8[0xB4 - 0xA8];
    int mode;
    int duration;
} MCamMoveRequest;

static void MCamMove_Pers(MCamMoveRequest *request)
{
    MCamMoveState *state = &D_00A5B5F8;
    int mode;
    int duration;
    float angle;
    float angleRad;
    float savedStart;

    mode = request->mode;
    duration = request->duration;
    /* MCamMove_Pers's start value is the float at state - 0x384. */
    state->start = camParams.perspectiveStart;
    state->mode = mode;
    angle = request->angle;
    state->duration = duration;
    state->elapsed = 0;
    if (angle != 0.0f) {
        angleRad = angle * 0.017453292f;
    } else {
        angleRad = 0.6981317f;
    }
    savedStart = state->start;
    state->end = angleRad;
    state->current = savedStart;
    camFlags |= 0x800;
}


static float MCamMoveProc_CalcRatio(unsigned int mode, unsigned int elapsed, unsigned int duration)
{
    float ratio;

    if (duration == 0) {
        MOutputDebugStringWarn(zero_duration_message, mode);
        return 0.0f;
    }

    ratio = (float)elapsed / (float)duration;

    switch (mode) {
    case 1: {
        float sine1;
        ratio = ratio * 3.14159265f + 4.712389f;
        __asm__ __volatile__(
            "mfc1    $8, %1\n\t"
            "qmtc2   $8, $vf4\n\t"
            "vcallms 0x20\n\t"
            "qmfc2.i $8, $vf1\n\t"
            "mtc1    $8, %0\n\t"
            : "=f"(sine1)
            : "f"(ratio)
            : "$8", "memory"
        );
        ratio = (sine1 + 1.0f) * 0.5f;
        break;
    }
    case 0:
        break;
    case 2: {
        float sine2;
        ratio = ratio * 1.57079633f + 4.712389f;
        __asm__ __volatile__(
            "mfc1    $8, %1\n\t"
            "qmtc2   $8, $vf4\n\t"
            "vcallms 0x20\n\t"
            "qmfc2.i $8, $vf1\n\t"
            "mtc1    $8, %0\n\t"
            : "=f"(sine2)
            : "f"(ratio)
            : "$8", "memory"
        );
        ratio = sine2 + 1.0f;
        break;
    }
    case 3: {
        float sine3;
        ratio = ratio * 1.57079633f;
        __asm__ __volatile__(
            "mfc1    $8, %1\n\t"
            "qmtc2   $8, $vf4\n\t"
            "vcallms 0x20\n\t"
            "qmfc2.i $8, $vf1\n\t"
            "mtc1    $8, %0\n\t"
            : "=f"(sine3)
            : "f"(ratio)
            : "$8", "memory"
        );
        ratio = sine3;
        break;
    }
    default:
        MOutputDebugStringWarn(invalid_interpolation_message);
        break;
    }

    return ratio;
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamMoveProc_Coord);

static int MCamMoveProc_Bank(MCamMoveState *state)
{
    float start;
    float ratio;

    ratio = MCamMoveProc_CalcRatio(state->mode, ++state->elapsed, state->duration);
    start = state->start;
    state->current = start + (state->end - start) * ratio;
    return state->elapsed < state->duration;
}

static int MCamMoveProc_Pers(MCamMoveState *state)
{
    float start;
    float ratio;

    ratio = MCamMoveProc_CalcRatio(state->mode, ++state->elapsed, state->duration);
    start = state->start;
    state->current = start + (state->end - start) * ratio;
    return state->elapsed < state->duration;
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamShakeProc);
