/*
 * OV12 original TU 8: 0x00a0d178..0x00a0db58 (5 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_robot_spec.h"
#include "rg_robot.h"
#include "ov12/rg_text_info.h"

static int s_nAnaAutoHomingID = 0;

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern const char D_00A51E68[];
/* "pSpec != NIL" */
extern const char D_00A51E78[];

/* "../rg_robot_spec.euc.c" */
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
    pSpec->moveSpeedMax = 20.0f;
    pSpec->dashSpeedMax = 40.0f;
    pSpec->turnRate = 0.35298797f;
    pSpec->rotSpeedBottomRate = 0.1f;
    pSpec->weight = 1000.0f;
    pSpec->enginePower = 60000.0f;
    pSpec->rotateForce = 62831.855f;
    pSpec->moveResist = 2.8f;
    pSpec->rotResist = 9.0f;
    pSpec->dashTime = 0.3f;
    pSpec->dashPower = 3.0f;
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
        pSpec->m_aAutoHoming[nID].value = atof(pszString) * 3.1415927f;
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



extern const char D_00A51F30[]; /* "engine-power" */
extern const char D_00A51F40[]; /* "engine-impulse" */
extern const char D_00A51F50[]; /* "move-spd-max" */
extern const char D_00A51F60[]; /* "dash-spd-max" */
extern const char D_00A51F70[]; /* "rot-spd-bottom-rate" */
extern const char D_00A51F88[]; /* "weight" */
extern const char D_00A51F90[]; /* "move-resist" */
extern const char D_00A51FA0[]; /* "rot-resist" */
extern const char D_00A51FB0[]; /* "dash-time" */
extern const char D_00A51FC0[]; /* "dash-power" */
extern const char D_00A51FD0[]; /* "dash-cooldown" */
extern const char D_00A51FE0[]; /* "life" */
extern const char D_00A51FE8[]; /* "damage-stiff-time" */
extern const char D_00A52000[]; /* "body-attack-damage" */
extern const char D_00A52018[]; /* "jet-eff" */
extern const char D_00A52020[]; /* "dash-eff" */
extern const char D_00A52030[]; /* "sude" */
extern const char D_00A52038[]; /* "roll-power" */
extern const char D_00A52048[]; /* "auto-homing" */
extern const char D_00A52058[]; /* "rot-spd-max" */
extern const char D_00A52068[]; /* "dash-mode" */
extern const char D_00A52078[]; /* "normal" */
extern const char D_00A52080[]; /* "vr" */
extern const char D_00A52088[]; /* "neo" */
extern const char D_00A52090[]; /* "neo2" */
extern const char D_00A52098[]; /* "neo3" */

void RgRobotSpecReadFromText(RgRobotSpec *pSpec, struct RgReadText *pReader)
{
    float rotSpdMax;
    float vec[3];
    char szMode[64];
    struct RgTextInfoEntry paraList[] = {
        {.type = RG_TEXT_INFO_FLOAT_OFFSET, .pszName = D_00A51F30,
         .destination = (void *) 32, .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_FLOAT_OFFSET, .pszName = D_00A51F40,
         .destination = (void *) 32, .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_FLOAT_OFFSET, .pszName = D_00A51F50,
         .destination = (void *) 12, .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_FLOAT_OFFSET, .pszName = D_00A51F60,
         .destination = (void *) 16, .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_FLOAT_OFFSET, .pszName = D_00A51F70,
         .destination = (void *) 24, .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_FLOAT_OFFSET, .pszName = D_00A51F88,
         .destination = (void *) 28, .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_FLOAT_OFFSET, .pszName = D_00A51F90,
         .destination = (void *) 40, .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_FLOAT_OFFSET, .pszName = D_00A51FA0,
         .destination = (void *) 44, .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_FLOAT_OFFSET, .pszName = D_00A51FB0,
         .destination = (void *) 48, .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_FLOAT_OFFSET, .pszName = D_00A51FC0,
         .destination = (void *) 52, .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_FLOAT_OFFSET, .pszName = D_00A51FD0,
         .destination = (void *) 56, .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_FLOAT_OFFSET, .pszName = D_00A51FE0,
         .destination = (void *) 76, .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_FLOAT_OFFSET, .pszName = D_00A51FE8,
         .destination = (void *) 80, .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_FLOAT_OFFSET, .pszName = D_00A52000,
         .destination = (void *) 84, .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_STRING_OFFSET, .pszName = D_00A52018,
         .destination = (void *) 88, .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_STRING_OFFSET, .pszName = D_00A52020,
         .destination = (void *) 120, .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_STRING_OFFSET, .pszName = D_00A52030,
         .destination = (void *) 152, .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_FLOAT_ARRAY_ADDRESS, .pszName = D_00A52038,
         .destination = vec, .auxiliary = (void *) 3},
        {.type = RG_TEXT_INFO_CALLBACK_ADDRESS, .pszName = D_00A52048,
         .destination = pSpec, .auxiliary = _AnaAutoHoming},
        {.type = RG_TEXT_INFO_FLOAT_ADDRESS, .pszName = D_00A52058,
         .destination = &rotSpdMax, .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_STRING_ADDRESS, .pszName = D_00A52068,
         .destination = szMode, .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_END}
    };

    szMode[0] = 0;
    _ResetAnaAutoHoming();
    RgTextInfoRead(pReader, paraList, (char *)pSpec);

    pSpec->rotateForce = vec[0] * 6.2831855f * vec[1] / vec[2];
    pSpec->turnRate = 6.2831855f / rotSpdMax;

    if (strcasecmp(szMode, D_00A52078) == 0) {
        pSpec->type = 0;
    } else if (strcasecmp(szMode, D_00A52080) == 0) {
        pSpec->type = 1;
    } else if (strcasecmp(szMode, D_00A52088) == 0) {
        pSpec->type = 2;
    } else if (strcasecmp(szMode, D_00A52090) == 0) {
        pSpec->type = 3;
    } else if (strcasecmp(szMode, D_00A52098) == 0) {
        pSpec->type = 4;
    }
}

/*
 * Dump labels: one "%f" line per tuning value; the two auto-homing lines
 * also print the flag word with "%x".
 */
extern const char D_00A520A0[];
/* "engine-power         %f\n" */
extern const char D_00A520C0[];
/* "roll-power           %f\n" */
extern const char D_00A520E0[];
/* "move-spd-max         %f\n" */
extern const char D_00A52100[];
/* "rot-spd-max          %f\n" */
extern const char D_00A52120[];
/* "rot-spd-bottom-rate  %f\n" */
extern const char D_00A52140[];
/* "weight               %f\n" */
extern const char D_00A52160[];
/* "move-resist          %f\n" */
extern const char D_00A52180[];
/* "rot-resist           %f\n" */
extern const char D_00A521A0[];
/* "dash-time            %f\n" */
extern const char D_00A521C0[];
/* "dash-power           %f\n" */
extern const char D_00A521E0[];
/* "dash-cooldown        %f\n" */
extern const char D_00A52200[];
/* "auto-homing 0        %f (%x)\n" */
extern const char D_00A52220[];
/* "auto-homing 1        %f (%x)\n" */
extern const char D_00A52240[];
/* "life                 %f\n" */
extern const char D_00A52260[];
/* "damage-stiff-time    %f\n" */
extern void XrgLog(const char *format, const char *source_file, int line, ...);

void RgRobotSpecDump(RgRobotSpec *pSpec)
{
    if (pSpec == 0) {
        assert_prog(D_00A51E68, D_00A51E78, 158);
    }
    XrgLog(D_00A520A0, D_00A51E78, 160, pSpec->enginePower);
    XrgLog(D_00A520C0, D_00A51E78, 161, pSpec->rotateForce);
    XrgLog(D_00A520E0, D_00A51E78, 162, pSpec->moveSpeedMax);
    XrgLog(D_00A52100, D_00A51E78, 163, pSpec->turnRate);
    XrgLog(D_00A52120, D_00A51E78, 164, pSpec->rotSpeedBottomRate);
    XrgLog(D_00A52140, D_00A51E78, 165, pSpec->weight);
    XrgLog(D_00A52160, D_00A51E78, 166, pSpec->moveResist);
    XrgLog(D_00A52180, D_00A51E78, 167, pSpec->rotResist);
    XrgLog(D_00A521A0, D_00A51E78, 168, pSpec->dashTime);
    XrgLog(D_00A521C0, D_00A51E78, 169, pSpec->dashPower);
    XrgLog(D_00A521E0, D_00A51E78, 170, pSpec->dashCooldown);
    XrgLog(D_00A52200, D_00A51E78, 172, pSpec->m_aAutoHoming[0].value,
           pSpec->m_aAutoHoming[0].flags);
    XrgLog(D_00A52220, D_00A51E78, 173, pSpec->m_aAutoHoming[1].value,
           pSpec->m_aAutoHoming[1].flags);
    XrgLog(D_00A52240, D_00A51E78, 174, pSpec->life);
    XrgLog(D_00A52260, D_00A51E78, 175, pSpec->damageStiffTime);
}
