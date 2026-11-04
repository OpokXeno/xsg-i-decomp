/*
 * OV12 original TU 8: 0x00a0d178..0x00a0db58 (5 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_robot_spec.h"
#include "ov12/rg_text_info.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_error.h"
#include "ov12/xrg_v_log.h"

#define offsetof(type, member) ((unsigned int) &((type *) 0)->member)

extern const char robotSpecAssertSpecNotNil[]; /* "pSpec != NIL" */
extern const char robotSpecSourcePath[]; /* "../rg_robot_spec.euc.c" */
/*
 * Scaffold-owned rodata: the assert expression on the auto-homing index
 * ("nID < sizeof(pSpec->m_aAutoHoming) / ..."), the four flag keywords
 * "see-target", "lock-on", "shot", "in-scrn" and the unknown-keyword
 * message. The double pi the value is scaled by (0x00a51f28) is this
 * function's own literal, carved to the C object.
 */
extern const char robotSpecAssertAutoHomingIdInRange[];
extern const char robotSpecAutoHomingSeeTargetKeyword[];
extern const char robotSpecAutoHomingLockOnKeyword[];
extern const char robotSpecAutoHomingShotKeyword[];
extern const char robotSpecAutoHomingInScreenKeyword[];
extern const char robotSpecUnknownAutoHomingKeywordFormat[];
extern const char robotSpecEnginePowerKey[]; /* "engine-power" */
extern const char robotSpecEngineImpulseKey[]; /* "engine-impulse" */
extern const char robotSpecMoveSpeedMaxKey[]; /* "move-spd-max" */
extern const char robotSpecDashSpeedMaxKey[]; /* "dash-spd-max" */
extern const char robotSpecRotSpeedBottomRateKey[]; /* "rot-spd-bottom-rate" */
extern const char robotSpecWeightKey[]; /* "weight" */
extern const char robotSpecMoveResistKey[]; /* "move-resist" */
extern const char robotSpecRotResistKey[]; /* "rot-resist" */
extern const char robotSpecDashTimeKey[]; /* "dash-time" */
extern const char robotSpecDashPowerKey[]; /* "dash-power" */
extern const char robotSpecDashCooldownKey[]; /* "dash-cooldown" */
extern const char robotSpecLifeKey[]; /* "life" */
extern const char robotSpecDamageStiffTimeKey[]; /* "damage-stiff-time" */
extern const char robotSpecBodyAttackDamageKey[]; /* "body-attack-damage" */
extern const char robotSpecJetEffectKey[]; /* "jet-eff" */
extern const char robotSpecDashEffectKey[]; /* "dash-eff" */
extern const char robotSpecBareHandKey[]; /* "sude" */
extern const char robotSpecRollPowerKey[]; /* "roll-power" */
extern const char robotSpecAutoHomingKey[]; /* "auto-homing" */
extern const char robotSpecRotSpeedMaxKey[]; /* "rot-spd-max" */
extern const char robotSpecDashModeKey[]; /* "dash-mode" */
extern const char robotSpecDashModeNormalKeyword[]; /* "normal" */
extern const char robotSpecDashModeVrKeyword[]; /* "vr" */
extern const char robotSpecDashModeNeoKeyword[]; /* "neo" */
extern const char robotSpecDashModeNeo2Keyword[]; /* "neo2" */
extern const char robotSpecDashModeNeo3Keyword[]; /* "neo3" */
/* Dump labels: one "%f" line per tuning value; the two auto-homing lines
 * also print the flag word with "%x". */
extern const char robotSpecDumpEnginePowerFormat[]; /* "engine-power         %f\n" */
extern const char robotSpecDumpRollPowerFormat[]; /* "roll-power           %f\n" */
extern const char robotSpecDumpMoveSpeedMaxFormat[]; /* "move-spd-max         %f\n" */
extern const char robotSpecDumpRotSpeedMaxFormat[]; /* "rot-spd-max          %f\n" */
extern const char robotSpecDumpRotSpeedBottomRateFormat[]; /* "rot-spd-bottom-rate  %f\n" */
extern const char robotSpecDumpWeightFormat[]; /* "weight               %f\n" */
extern const char robotSpecDumpMoveResistFormat[]; /* "move-resist          %f\n" */
extern const char robotSpecDumpRotResistFormat[]; /* "rot-resist           %f\n" */
extern const char robotSpecDumpDashTimeFormat[]; /* "dash-time            %f\n" */
extern const char robotSpecDumpDashPowerFormat[]; /* "dash-power           %f\n" */
extern const char robotSpecDumpDashCooldownFormat[]; /* "dash-cooldown        %f\n" */
extern const char robotSpecDumpAutoHoming0Format[]; /* "auto-homing 0        %f (%x)\n" */
extern const char robotSpecDumpAutoHoming1Format[]; /* "auto-homing 1        %f (%x)\n" */
extern const char robotSpecDumpLifeFormat[]; /* "life                 %f\n" */
extern const char robotSpecDumpDamageStiffTimeFormat[]; /* "damage-stiff-time    %f\n" */

static int s_nAnaAutoHomingID = 0;

/*
 * Default spec: no character, the built-in tuning values and the
 * "UNARMED" sude (an 8-byte literal at 0x00a51e90, carved to the C object).
 */
void InitRgRobotSpec(RgRobotSpec *pSpec)
{
    if (pSpec == 0) {
        assert_prog(robotSpecAssertSpecNotNil, robotSpecSourcePath, 22);
    }
    pSpec->charID = -1;
    pSpec->type = RG_DASH_MODE_NORMAL;
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
    pSpec->m_aAutoHoming[0].flags = RG_AUTO_HOMING_SEE_TARGET | RG_AUTO_HOMING_LOCK_ON;
    pSpec->m_aAutoHoming[1].value = 0.34f;
    pSpec->m_aAutoHoming[1].flags = RG_AUTO_HOMING_LOCK_ON | RG_AUTO_HOMING_SHOT;
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
        if (nID >= RG_AUTO_HOMING_ENTRY_NUM) {
            assert_prog(robotSpecAssertAutoHomingIdInRange, robotSpecSourcePath, 77);
        }
        pSpec->m_aAutoHoming[nID].value = atof(pszString) * 3.1415927f;
        pSpec->m_aAutoHoming[nID].flags = 0;
        return;
    }
    nID = s_nAnaAutoHomingID;
    if (nID >= RG_AUTO_HOMING_ENTRY_NUM) {
        assert_prog(robotSpecAssertAutoHomingIdInRange, robotSpecSourcePath, 83);
    }
    if (strcmp(pszString, robotSpecAutoHomingSeeTargetKeyword) == 0) {
        pSpec->m_aAutoHoming[nID].flags |= RG_AUTO_HOMING_SEE_TARGET;
    } else if (strcmp(pszString, robotSpecAutoHomingLockOnKeyword) == 0) {
        pSpec->m_aAutoHoming[nID].flags |= RG_AUTO_HOMING_LOCK_ON;
    } else if (strcmp(pszString, robotSpecAutoHomingShotKeyword) == 0) {
        pSpec->m_aAutoHoming[nID].flags |= RG_AUTO_HOMING_SHOT;
    } else if (strcmp(pszString, robotSpecAutoHomingInScreenKeyword) == 0) {
        pSpec->m_aAutoHoming[nID].flags |= RG_AUTO_HOMING_IN_SCRN;
    } else {
        RgError(robotSpecUnknownAutoHomingKeywordFormat, robotSpecSourcePath, 95, pszString);
    }
}

void RgRobotSpecReadFromText(RgRobotSpec *pSpec, struct RgReadText *pReader)
{
    float rotSpdMax;
    float vec[3];
    char szMode[64];
    struct RgTextInfoEntry paraList[] = {
        {.type = RG_TEXT_INFO_FLOAT_OFFSET, .pszName = robotSpecEnginePowerKey,
         .destination = (void *) offsetof(RgRobotSpec, enginePower), .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_FLOAT_OFFSET, .pszName = robotSpecEngineImpulseKey,
         .destination = (void *) offsetof(RgRobotSpec, enginePower), .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_FLOAT_OFFSET, .pszName = robotSpecMoveSpeedMaxKey,
         .destination = (void *) offsetof(RgRobotSpec, moveSpeedMax), .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_FLOAT_OFFSET, .pszName = robotSpecDashSpeedMaxKey,
         .destination = (void *) offsetof(RgRobotSpec, dashSpeedMax), .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_FLOAT_OFFSET, .pszName = robotSpecRotSpeedBottomRateKey,
         .destination = (void *) offsetof(RgRobotSpec, rotSpeedBottomRate), .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_FLOAT_OFFSET, .pszName = robotSpecWeightKey,
         .destination = (void *) offsetof(RgRobotSpec, weight), .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_FLOAT_OFFSET, .pszName = robotSpecMoveResistKey,
         .destination = (void *) offsetof(RgRobotSpec, moveResist), .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_FLOAT_OFFSET, .pszName = robotSpecRotResistKey,
         .destination = (void *) offsetof(RgRobotSpec, rotResist), .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_FLOAT_OFFSET, .pszName = robotSpecDashTimeKey,
         .destination = (void *) offsetof(RgRobotSpec, dashTime), .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_FLOAT_OFFSET, .pszName = robotSpecDashPowerKey,
         .destination = (void *) offsetof(RgRobotSpec, dashPower), .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_FLOAT_OFFSET, .pszName = robotSpecDashCooldownKey,
         .destination = (void *) offsetof(RgRobotSpec, dashCooldown), .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_FLOAT_OFFSET, .pszName = robotSpecLifeKey,
         .destination = (void *) offsetof(RgRobotSpec, life), .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_FLOAT_OFFSET, .pszName = robotSpecDamageStiffTimeKey,
         .destination = (void *) offsetof(RgRobotSpec, damageStiffTime), .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_FLOAT_OFFSET, .pszName = robotSpecBodyAttackDamageKey,
         .destination = (void *) offsetof(RgRobotSpec, bodyAttackDamage), .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_STRING_OFFSET, .pszName = robotSpecJetEffectKey,
         .destination = (void *) offsetof(RgRobotSpec, jetEffect), .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_STRING_OFFSET, .pszName = robotSpecDashEffectKey,
         .destination = (void *) offsetof(RgRobotSpec, dashEffect), .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_STRING_OFFSET, .pszName = robotSpecBareHandKey,
         .destination = (void *) offsetof(RgRobotSpec, sude), .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_FLOAT_ARRAY_ADDRESS, .pszName = robotSpecRollPowerKey,
         .destination = vec, .auxiliary = (void *) 3},
        {.type = RG_TEXT_INFO_CALLBACK_ADDRESS, .pszName = robotSpecAutoHomingKey,
         .destination = pSpec, .auxiliary = _AnaAutoHoming},
        {.type = RG_TEXT_INFO_FLOAT_ADDRESS, .pszName = robotSpecRotSpeedMaxKey,
         .destination = &rotSpdMax, .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_STRING_ADDRESS, .pszName = robotSpecDashModeKey,
         .destination = szMode, .auxiliary = (void *) 0},
        {.type = RG_TEXT_INFO_END}
    };

    szMode[0] = 0;
    _ResetAnaAutoHoming();
    RgTextInfoRead(pReader, paraList, (char *)pSpec);

    pSpec->rotateForce = vec[0] * 6.2831855f * vec[1] / vec[2];
    pSpec->turnRate = 6.2831855f / rotSpdMax;

    if (strcasecmp(szMode, robotSpecDashModeNormalKeyword) == 0) {
        pSpec->type = RG_DASH_MODE_NORMAL;
    } else if (strcasecmp(szMode, robotSpecDashModeVrKeyword) == 0) {
        pSpec->type = RG_DASH_MODE_VR;
    } else if (strcasecmp(szMode, robotSpecDashModeNeoKeyword) == 0) {
        pSpec->type = RG_DASH_MODE_NEO;
    } else if (strcasecmp(szMode, robotSpecDashModeNeo2Keyword) == 0) {
        pSpec->type = RG_DASH_MODE_NEO2;
    } else if (strcasecmp(szMode, robotSpecDashModeNeo3Keyword) == 0) {
        pSpec->type = RG_DASH_MODE_NEO3;
    }
}

void RgRobotSpecDump(RgRobotSpec *pSpec)
{
    if (pSpec == 0) {
        assert_prog(robotSpecAssertSpecNotNil, robotSpecSourcePath, 158);
    }
    XrgLog(robotSpecDumpEnginePowerFormat, robotSpecSourcePath, 160, pSpec->enginePower);
    XrgLog(robotSpecDumpRollPowerFormat, robotSpecSourcePath, 161, pSpec->rotateForce);
    XrgLog(robotSpecDumpMoveSpeedMaxFormat, robotSpecSourcePath, 162, pSpec->moveSpeedMax);
    XrgLog(robotSpecDumpRotSpeedMaxFormat, robotSpecSourcePath, 163, pSpec->turnRate);
    XrgLog(robotSpecDumpRotSpeedBottomRateFormat, robotSpecSourcePath, 164, pSpec->rotSpeedBottomRate);
    XrgLog(robotSpecDumpWeightFormat, robotSpecSourcePath, 165, pSpec->weight);
    XrgLog(robotSpecDumpMoveResistFormat, robotSpecSourcePath, 166, pSpec->moveResist);
    XrgLog(robotSpecDumpRotResistFormat, robotSpecSourcePath, 167, pSpec->rotResist);
    XrgLog(robotSpecDumpDashTimeFormat, robotSpecSourcePath, 168, pSpec->dashTime);
    XrgLog(robotSpecDumpDashPowerFormat, robotSpecSourcePath, 169, pSpec->dashPower);
    XrgLog(robotSpecDumpDashCooldownFormat, robotSpecSourcePath, 170, pSpec->dashCooldown);
    XrgLog(robotSpecDumpAutoHoming0Format, robotSpecSourcePath, 172, pSpec->m_aAutoHoming[0].value,
           pSpec->m_aAutoHoming[0].flags);
    XrgLog(robotSpecDumpAutoHoming1Format, robotSpecSourcePath, 173, pSpec->m_aAutoHoming[1].value,
           pSpec->m_aAutoHoming[1].flags);
    XrgLog(robotSpecDumpLifeFormat, robotSpecSourcePath, 174, pSpec->life);
    XrgLog(robotSpecDumpDamageStiffTimeFormat, robotSpecSourcePath, 175, pSpec->damageStiffTime);
}
