/*
 * OV01 original TU 17: 0x00a314b8..0x00a32db8 (36 functions)
 */
#include "common.h"
#include "shared.h"

extern void MOutputDebugStringWarn(const char *format, ...);
extern const char zero_duration_message[];
extern const char invalid_interpolation_message[];
extern const char D_00A50EF0[];
extern const char D_00A51000[];
extern const char D_00A51028[];

extern void bcopy(const void *source, void *destination, unsigned int count);
/*
 * Partial view of the unit init record returned by dataUnitInitGet (its
 * definer, ov01/data_unit_org_get.c, is not recovered yet); only the two
 * ranges this TU reads are modeled. Ranges are stored in hundredths.
 */
typedef struct UnitInitData {
    unsigned char unmodeled_00[0x10];
    short atkRange;
    short defRange;
} UnitInitData;

extern UnitInitData *dataUnitInitGet(int id);

/*
 * .bss for this TU is still scaffold-owned (config/tu-build.json
 * data_ownership); the scaffold defines it under the original ELF's own
 * local symbol names, imported through config/symbols/ov01.txt.
 */
extern void *sysCam;
extern unsigned char sysCamBuff[0x5F0];
extern int camFlags;
extern unsigned char camParams[0x5B0];

#define CAM_FLAG_ACTIVE 0x1
#define CAM_FLAG_DUMP_DISABLED 0x10
#define CAM_SHAKE_BIT 12

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

void *MCamGetSysCam(void)
{
    return sysCam;
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamGetCurrentBaseCoord);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamGetCurrentCoord);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamGetCurrentAngle);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamGetCurrentEye);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamGetLengthActor);

void MCamInit(void)
{
    camFlags = 0;
    memset(camParams, 0, sizeof(camParams));
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

    bcopy(sysCamBuff, sysCam, sizeof(sysCamBuff));
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

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamMove_Pers);


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
