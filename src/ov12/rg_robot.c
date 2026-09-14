/*
 * OV12 original TU 4: 0x00a02510..0x00a09480 (153 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_robot.h"

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitHomingThread);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _CreateHomingThread);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DisposeHomingThread);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _StartHomingThread);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _StopHomingThread);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _GetStatHomingThread);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _GetSleepTimeHomingThread);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _PassTimeHomingThread);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _CalcMaxSpeed);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _StopAllShotThread);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodyOnGround);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitStatus);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _CreateStatue);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _SetStatus);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DisposeStatus);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _StatusExecCmd);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _StatusPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _StatusDisp);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _AllowAllCmdInMoving);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _AllowDamageCmd);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _AllowDamageCmdNotVRDashHit);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _AllowDamageCmdNotPetrify);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _MovingPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitMovingStatus);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExecCmdInAttack);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _AttackPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _AttackExit);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitAttackStatus);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExecCmdInHissatu);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _HissatuPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _HissatuExit);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitStatusHissatu);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExecCmdInShield);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ShieldPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ShieldExit);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitShieldStatus);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExecCmdInTargetting);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _TargettingPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExitTargettingStatus);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitTargettingStatus);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExecCmdInBreaking);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BreakingPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BreakingExit);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitBreakingStatus);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DashExecCmd);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DashPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DashExit);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitDashStatus);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DashVRMainToFollow);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DashVRExecCmd);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DashVRPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DashVRExit);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitDashVRStatus);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DamageExecCmd);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DamagePassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitDamageStatus);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExecCmdInDrop);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DropPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitDropStatus);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _EntryCmdQueue);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitCmdQueue);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ClearCmdQueue);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _CreateCmdQueue);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DisposeCmdQueue);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodyPlayMotion);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodyPlayMotionLoop);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodyAdvanceForAttack);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodyGetTargetPos);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodySetSpec);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodySetGeom);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodySetEyeGeom);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodySetAdvanceGeom);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodySetSound);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodySetActor);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodySetTarget);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodyDropWeapon);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodyEquipWeapon);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodySetSpareWeapon);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodySetAutoHomingEnv);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodyExecCmd);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodyGeomPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _EyeGeomPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _AdvanceGeomPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodyAutoHomingPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodyPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _BodyDisp);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitBody);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DestructBody);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _CreateBody);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DisposeBody);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExecAccelarateCmd);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExecRotateCmd);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _IsBackWeaponReady);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExecShotOrAttackCmd);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExecTargettingCmd);

static int _ExecBreakCmd(RgStatus *status, RgBody *body, void *command)
{
    float brake_scale = 0.30000001192092895508f;

    (void)command;
    RgRobSubBreak(body->geometry, brake_scale, brake_scale);
    _InitBreakingStatus(status, body);
    return 0x40;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExecDashCmd);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExecDashContinueCmd);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _GetDropMotion);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExecDropWeaponCmd);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExecDamageCmd);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExecHitByBody);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _ExecWeakDamageCmd);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DullFlagsPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _PassTimeRobot);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DispRobot);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _DestructRobot);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", _InitRobot);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", CreateRgRobot);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", DisposeRgRobot);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotDisp);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotSetSpec);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotSetGeom);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotSetEyeGeom);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotSetAdvanceGeom);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotSetSoundDriver);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotSetActor);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotSetTarget);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotSetRgDrawView);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotSetMotSmooth);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotSetConfuse);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotSetWeapon);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotSetSpareWeapon);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotGetGeom);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotGetWeapon);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotGetTarget);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotIsDead);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotGetLife);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotGetLifeMax);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotGetActor);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotGetCharID);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotIsInvalidAttack);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotGetDashTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotGetSpec);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotIsConfused);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotIsAcceptedCommand);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotGetStatusFlags);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotAccelarate);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotAccelarateRotate);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotShot);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotTargetting);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotBreak);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotDash);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotDashContinue);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotGiveDamage);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotGiveWeakDamage);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotHitByBody);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotDropWeapon);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotHitWeaponAttack);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotInvalidAttack);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotHitBG);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot", RgRobotNearBG);
