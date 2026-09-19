/*
 * OV12 original TU 15: 0x00a10618..0x00a10f20 (17 functions)
 */
#include "common.h"
#include "rg_player.h"

/*
 * These are external file-backed witnesses, not candidate-emitted data.
 *
 * ov12:0x00a52740 contains the assertion expression "pPlayer != NIL".
 * ov12:0x00a52708 contains the source filename "../rg_player.euc.c".
 */
extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern const char D_00A52740[];
extern const char D_00A52708[];

extern void *RgHeapAlloc(void *heap, unsigned int size, const char *source_file,
                         int line);
extern void RgHeapFree(void *heap, void *ptr, const char *source_file, int line);
extern RgHeap *InstanceOfRgHeap(void);

extern void _InitRgPlayer(RgPlayer *pPlayer, RgPlayerEssence *pDat);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_player", InitRgPlayerEssence);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_player", _PlayerSetFromEssence);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_player", _InitRgPlayer);

RgPlayer *CreateRgPlayer(RgPlayerEssence *pDat)
{
    RgPlayer *pPlayer;

    pPlayer = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgPlayer),
                          D_00A52708, 173);
    if (pPlayer == 0) {
        assert_prog(D_00A52740, D_00A52708, 174);
    }
    _InitRgPlayer(pPlayer, pDat);
    return pPlayer;
}

extern void DisposeRgRobot(RgRobot *pRobot);
extern void DisposeRgRobotControl(RgRobotControl *pControl);
extern void DisposeRgGeomGroup(RgGeomGroup *pGroup);

void DisposeRgPlayer(RgPlayer *pPlayer)
{
    RgRobotControl *pControl;

    if (pPlayer == 0) {
        assert_prog(D_00A52740, D_00A52708, 184);
    }
    if (pPlayer->robot != 0) {
        DisposeRgRobot(pPlayer->robot);
    }
    pControl = pPlayer->control;
    pPlayer->robot = 0;
    if (pControl != 0) {
        DisposeRgRobotControl(pControl);
    }
    pPlayer->control = 0;
    DisposeRgGeomGroup(pPlayer->bodyGeoms);
    DisposeRgGeomGroup(pPlayer->shotGeoms);
    DisposeRgGeomGroup(pPlayer->atkGeoms);
    DisposeRgGeomGroup(pPlayer->eyeGeoms);
    DisposeRgGeomGroup(pPlayer->advGeoms);
    RgHeapFree(InstanceOfRgHeap(), pPlayer, D_00A52708, 200);
}

void RgPlayerSetControl(RgPlayer *pPlayer, RgRobotControl *pControl)
{
    if (pPlayer == 0) {
        assert_prog(D_00A52740, D_00A52708, 211);
    }
    if (pPlayer->control != 0) {
        DisposeRgRobotControl(pPlayer->control);
    }
    pPlayer->control = pControl;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_player", RgPlayerSetTraceCamera);

extern RgActor *RgRobotGetActor(RgRobot *pRobot);
extern void XrgActorSetLightCost(RgActor *pActor);

void RgPlayerSetLightCost(RgPlayer *pPlayer)
{
    RgRobot *pRobot;
    RgActor *pActor;

    if (pPlayer == 0) {
        assert_prog(D_00A52740, D_00A52708, 234);
    }
    pRobot = pPlayer->robot;
    if (pRobot != 0) {
        pActor = RgRobotGetActor(pRobot);
        if (pActor != 0) {
            XrgActorSetLightCost(pActor);
        }
    }
}

RgRobot *RgPlayerGetRobot(RgPlayer *pPlayer)
{
    if (pPlayer == 0) {
        assert_prog(D_00A52740, D_00A52708, 249);
    }
    return pPlayer->robot;
}

RgGeomGroup *RgPlayerGetBodyGeoms(RgPlayer *pPlayer)
{
    if (pPlayer == 0) {
        assert_prog(D_00A52740, D_00A52708, 256);
    }
    return pPlayer->bodyGeoms;
}

RgGeomGroup *RgPlayerGetShotGeoms(RgPlayer *pPlayer)
{
    if (pPlayer == 0) {
        assert_prog(D_00A52740, D_00A52708, 263);
    }
    return pPlayer->shotGeoms;
}

RgGeomGroup *RgPlayerGetAtkGeoms(RgPlayer *pPlayer)
{
    if (pPlayer == 0) {
        assert_prog(D_00A52740, D_00A52708, 270);
    }
    return pPlayer->atkGeoms;
}

RgGeomGroup *RgPlayerGetEyeGeoms(RgPlayer *pPlayer)
{
    if (pPlayer == 0) {
        assert_prog(D_00A52740, D_00A52708, 277);
    }
    return pPlayer->eyeGeoms;
}

RgGeomGroup *RgPlayerGetAdvGeoms(RgPlayer *pPlayer)
{
    if (pPlayer == 0) {
        assert_prog(D_00A52740, D_00A52708, 284);
    }
    return pPlayer->advGeoms;
}

extern void RgRobotControlJob(RgRobotControl *pControl);
extern unsigned int RgRobotGetStatusFlags(RgRobot *pRobot);
extern int RgRobotIsAcceptedCommand(RgRobot *pRobot, int command);
extern void RgRobotSetRgDrawView(RgRobot *pRobot, RgDrawView *view);

extern RgDrawStudio *RgCameraGetStudio(RgCamera *pCamera);
extern RgDrawView *RgDrawStudioGetView(RgDrawStudio *pStudio);
extern void RgCameraSetActionAdvance(RgCamera *pCamera, int flag);
extern void RgCameraSetActionAttack(RgCamera *pCamera, int flag);
extern void RgCameraSetActionDash(RgCamera *pCamera, int flag);
extern void RgCameraSetActionLockedOn(RgCamera *pCamera, int flag);
extern void RgCameraSetActionRoll(RgCamera *pCamera, int flag);
extern void RgCameraSetActionSeeTarget(RgCamera *pCamera, int flag);

void RgPlayerControl(RgPlayer *pPlayer)
{
    RgRobot *pRobot;
    RgCamera *pCamera;
    unsigned int statusFlags;

    if (pPlayer->control != 0) {
        RgRobotControlJob(pPlayer->control);
    }
    pCamera = pPlayer->traceCamera;
    if (pCamera != 0) {
        pRobot = pPlayer->robot;
        statusFlags = RgRobotGetStatusFlags(pRobot);
        RgRobotSetRgDrawView(pRobot, RgDrawStudioGetView(RgCameraGetStudio(pCamera)));
        if (RgRobotIsAcceptedCommand(pRobot, 0) != 0) {
            RgCameraSetActionAdvance(pCamera, 1);
        }
        if (RgRobotIsAcceptedCommand(pRobot, 1) != 0) {
            RgCameraSetActionRoll(pCamera, 1);
        }
        if (RgRobotIsAcceptedCommand(pRobot, 3) != 0) {
            RgCameraSetActionAttack(pCamera, 1);
        }
        if (statusFlags & 1) {
            RgCameraSetActionSeeTarget(pCamera, 1);
        }
        if (statusFlags & 2) {
            RgCameraSetActionLockedOn(pCamera, 1);
        }
        if (statusFlags & 4) {
            RgCameraSetActionDash(pCamera, 1);
        }
    }
}

extern void RgRobotDisp(RgRobot *pRobot);

void RgPlayerDisp(RgPlayer *pPlayer)
{
    if (pPlayer->robot != 0) {
        RgRobotDisp(pPlayer->robot);
    }
}

extern void RgRobotPassTime(RgRobot *pRobot, float deltaTime);

void RgPlayerPassTime(RgPlayer *pPlayer, float deltaTime)
{
    if (pPlayer->robot != 0) {
        RgRobotPassTime(pPlayer->robot, deltaTime);
    }
}
