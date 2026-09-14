/*
 * OV01 original TU 17: 0x00a314b8..0x00a32db8 (36 functions)
 */
#include "common.h"

extern void MOutputDebugStringWarn(const char *format, ...);
extern const char zero_duration_message[];
extern const char invalid_interpolation_message[];

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamGetSysCam);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamGetCurrentBaseCoord);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamGetCurrentCoord);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamGetCurrentAngle);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamGetCurrentEye);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamGetLengthActor);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamInit);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamExec);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamDumpDisabled);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamTakeControl);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamReleaseControl);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamStoreParams);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamIsActive);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamSet);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamMove);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamStopMove);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamIsMoving);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamShake);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamIsShaking);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamGetActorCoord);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamCalcBaseCoord);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamCalcCoord);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamGetAtkRange);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamGetDefRange);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamExec_Control);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamExec_SetParams);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamExec_SetParams_Base);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamExec_DispParams);

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

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamMoveProc_Bank);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamMoveProc_Pers);

INCLUDE_ASM("asm/nonmatchings/ov01/m_cam", MCamShakeProc);
