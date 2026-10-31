/*
 * OV12 original TU 8: 0x00a0d178..0x00a0db58 (5 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_robot_spec.h"
#include "rg_robot.h"

static int s_nAnaAutoHomingID = 0;

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern const char D_00A51E68[]; /* "pSpec != NIL" */
extern const char D_00A51E78[]; /* "../rg_robot_spec.euc.c" */

/*
 * Default spec: no character, the built-in tuning values and the
 * "UNARMED" sude (an 8-byte literal at 0x00a51e90, carved to the C object).
 */
void InitRgRobotSpec(RgRobotSpec *pSpec)
{
    if (pSpec == 0) {
        assert_prog(D_00A51E68, D_00A51E78, 22);
    }
    pSpec->charID = -1;
    pSpec->type = 0;
    pSpec->baseSpeed = 20.0f;
    pSpec->dashSpeedMax = 40.0f;
    pSpec->turnRate = 0.35298797f;
    pSpec->rotSpeedBottomRate = 0.1f;
    pSpec->speedRating = 1000.0f;
    pSpec->accelRate = 60000.0f;
    pSpec->rotateForce = 62831.855f;
    pSpec->moveResist = 2.8f;
    pSpec->rotResist = 9.0f;
    pSpec->dashTime = 0.3f;
    pSpec->attackAdvanceRate = 3.0f;
    pSpec->dashCooldown = 0.2f;
    pSpec->m_aAutoHoming[0].value = 0.03f;
    pSpec->m_aAutoHoming[0].flags = 3;
    pSpec->m_aAutoHoming[1].value = 0.34f;
    pSpec->m_aAutoHoming[1].flags = 6;
    pSpec->life = 100.0f;
    pSpec->damageStiffTime = 0.04f;
    pSpec->bodyAttackDamage = 10.0f;
    pSpec->jetEffect[0] = 0;
    pSpec->dashEffect[0] = 0;
    __builtin_strcpy(pSpec->sude, "UNARMED");
}

static void _ResetAnaAutoHoming(void)
{
    s_nAnaAutoHomingID = -1;
}

extern double atof(const char *nptr);
extern int strcmp(const char *s1, const char *s2);
extern void RgError(const char *message, const char *source_file, int line, ...);

/*
 * Scaffold-owned rodata: the assert expression on the auto-homing index
 * ("nID < sizeof(pSpec->m_aAutoHoming) / ..."), the four flag keywords
 * "see-target", "lock-on", "shot", "in-scrn" and the unknown-keyword
 * message. The double pi the value is scaled by (0x00a51f28) is this
 * function's own literal, carved to the C object.
 */
extern const char D_00A51E98[];
extern const char D_00A51EE0[];
extern const char D_00A51EF0[];
extern const char D_00A51EF8[];
extern const char D_00A51F00[];
extern const char D_00A51F08[];

/*
 * RgRobotSpecReadFromText's "auto-homing" callback: the first token
 * (index 0) opens the next m_aAutoHoming entry with its value in units of
 * pi, every further token ORs one flag keyword into that entry.
 */

static void _AnaAutoHoming(RgRobotSpec *pSpec, char *pszString, int index)
{
    unsigned int nID;

    if (index == 0) {
        nID = ++s_nAnaAutoHomingID;
        if (nID >= 2) {
            assert_prog(D_00A51E98, D_00A51E78, 77);
        }
        pSpec->m_aAutoHoming[nID].value = atof(pszString) * 3.1415927410125732;
        pSpec->m_aAutoHoming[nID].flags = 0;
        return;
    }
    nID = s_nAnaAutoHomingID;
    if (nID >= 2) {
        assert_prog(D_00A51E98, D_00A51E78, 83);
    }
    if (strcmp(pszString, D_00A51EE0) == 0) {
        pSpec->m_aAutoHoming[nID].flags |= 1;
    } else if (strcmp(pszString, D_00A51EF0) == 0) {
        pSpec->m_aAutoHoming[nID].flags |= 2;
    } else if (strcmp(pszString, D_00A51EF8) == 0) {
        pSpec->m_aAutoHoming[nID].flags |= 4;
    } else if (strcmp(pszString, D_00A51F00) == 0) {
        pSpec->m_aAutoHoming[nID].flags |= 8;
    } else {
        RgError(D_00A51F08, D_00A51E78, 95, pszString);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_spec", RgRobotSpecReadFromText);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_spec", RgRobotSpecDump);
