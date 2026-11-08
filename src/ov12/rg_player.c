/*
 * OV12 original TU 15: 0x00a10618..0x00a10f20 (17 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_camera.h"
#include "ov12/rg_draw.h"
#include "ov12/rg_equip_type.h"
#include "ov12/rg_geom_ball.h"
#include "ov12/rg_geom_group.h"
#include "ov12/rg_geom_point.h"
#include "ov12/rg_geom_robot.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_player.h"
#include "ov12/rg_robot.h"
#include "ov12/rg_robot_control.h"
#include "ov12/rg_weapon.h"
#include "ov12/xrg_actor.h"
#include "ov12/xrg_rand_int.h"
#include "ov12/xrg_sound.h"

/* Sound set and volume the player's robot is given. */
#define RG_PLAYER_SOUND_TYPE 1
#define RG_PLAYER_SOUND_VOLUME 1.0f

/* Radius of the ball that detects what lies in the robot's advance direction. */
#define RG_PLAYER_ADVANCE_RADIUS 0.1f

/* RgRobotIsAcceptedCommand command numbers the camera actions follow. */
#define RG_PLAYER_CMD_ADVANCE 0
#define RG_PLAYER_CMD_ROLL 1
#define RG_PLAYER_CMD_ATTACK 3

/* RgRobotGetStatusFlags bits the camera actions follow. */
#define RG_PLAYER_STATUS_SEE_TARGET 1
#define RG_PLAYER_STATUS_LOCKED_ON 2
#define RG_PLAYER_STATUS_DASH 4

/*
 * Partial view of RgWeaponCreateInfo (its third member is an int id there, and
 * storing the shot geometry group pointer through it reorders the stores): the
 * player's robot, the equip side, and the geometry groups of its shots and attacks.
 */
typedef struct RgWeaponCreateInfoPlayerView {
    RgStatus *robot;
    int side;
    RgGeomGroup *shotGeoms;
    RgGeomGroup *atkGeoms;
} RgWeaponCreateInfoPlayerView;


/*
 * RgGeomGroupCreateRobot is defined void (its body ends in a tail jump), but
 * the result register still holds the geometry it created and this caller
 * passes it on, so only this caller declares the pointer return.
 */
extern RgGeomPoint *RgGeomGroupCreateRobot(RgGeomGroup *pGroup, void *parent);

static void _InitRgPlayer(RgPlayer *pPlayer, RgPlayerEssence *pDat);
static void _PlayerSetFromEssence(RgPlayer *pPlayer, RgPlayerEssence *pDat);

const char playerAssertDatNotNil[16] = "pDat != NIL";
const char playerSourcePath[24] = "../rg_player.euc.c";
const char playerAssertPlayerAndDatNotNil[32] = "pPlayer != NIL && pDat != NIL";
const char playerAssertPlayerNotNil[16] = "pPlayer != NIL";

/* Resets a player essence: zero position, direction -Z, no spec, actor id -1 and no weapons. */
void InitRgPlayerEssence(RgPlayerEssence *pDat)
{
    int i;

    if (pDat == 0) {
        assert_prog(playerAssertDatNotNil, playerSourcePath, 41);
    }
    XrgClearVector(pDat->position);
    XrgClearVector(pDat->direction);
    pDat->direction[2] = -1.0f;
    pDat->spec = 0;
    pDat->actorID = -1;
    for (i = RG_EQUIP_TYPE_NUM - 1; i >= 0; i--) {
        pDat->weaponEssence[RG_EQUIP_TYPE_NUM - 1 - i] = 0;
        pDat->spareWeaponEssence[RG_EQUIP_TYPE_NUM - 1 - i] = 0;
    }
}

/*
 * Applies the essence to the player's robot: its position and direction, a fresh null
 * control, and the main and spare weapon of each equip side.
 */
static void _PlayerSetFromEssence(RgPlayer *pPlayer, RgPlayerEssence *pDat)
{
    RgWeaponCreateInfoPlayerView weaponInfo;
    void *geometry;
    RgWeaponEssence *weaponEssence;
    int side;

    if (pPlayer == 0 || pDat == 0) {
        assert_prog(playerAssertPlayerAndDatNotNil, playerSourcePath, 64);
    }
    geometry = RgRobotGetGeom(pPlayer->robot);
    RgGeomPointSetPos(geometry, pDat->position);
    RgGeomRobotSetDir(geometry, pDat->direction);
    if (pPlayer->control != 0) {
        DisposeRgRobotControl(pPlayer->control);
        pPlayer->control = 0;
    }
    pPlayer->traceCamera = 0;
    side = 0;
    pPlayer->control = CreateRgRobotControlNul(pPlayer->robot);
    do {
        weaponInfo.robot = pPlayer->robot;
        weaponInfo.shotGeoms = pPlayer->shotGeoms;
        weaponInfo.atkGeoms = pPlayer->atkGeoms;
        weaponEssence = pDat->weaponEssence[side];
        weaponInfo.side = side;
        if (weaponEssence != 0 &&
            (side != RG_EQUIP_TYPE_BACK ||
             RgWeaponEssCastToUnArmed(weaponEssence) == 0)) {
            RgRobotSetWeapon(pPlayer->robot, side,
                            CreateRgWeaponFromEssence(
                                weaponEssence,
                                (RgWeaponCreateInfo *)&weaponInfo));
        }
        weaponEssence = pDat->spareWeaponEssence[side];
        if (weaponEssence != 0 &&
            (side != RG_EQUIP_TYPE_BACK ||
             RgWeaponEssCastToUnArmed(weaponEssence) == 0)) {
            RgRobotSetSpareWeapon(pPlayer->robot, side,
                                  CreateRgWeaponFromEssence(
                                      weaponEssence,
                                      (RgWeaponCreateInfo *)&weaponInfo));
        }
        side++;
    } while (side < RG_EQUIP_TYPE_NUM);
}

/*
 * Creates the robot and its geometry groups, applies the spec, actor and sound, then
 * sets the player from the essence.
 */
static void _InitRgPlayer(RgPlayer *pPlayer, RgPlayerEssence *pDat)
{
    XrgActorEssence actorEssence;
    RgStatus *pRobot;
    RgGeomGroup *bodyGeoms;
    RgGeomGroup *shotGeoms;
    RgGeomGroup *atkGeoms;
    RgGeomGroup *eyeGeoms;
    RgGeomGroup *advGeoms;
    void *advanceGeom;
    void *spec;
    void *sound;

    if (pPlayer == 0) {
        assert_prog(playerAssertPlayerNotNil, playerSourcePath, 123);
    }
    if (pDat == 0) {
        assert_prog(playerAssertDatNotNil, playerSourcePath, 124);
    }
    pRobot = CreateRgRobot();
    pPlayer->control = 0;
    pPlayer->robot = pRobot;
    bodyGeoms = CreateRgGeomGroup();
    pPlayer->bodyGeoms = bodyGeoms;
    shotGeoms = CreateRgGeomGroup();
    pPlayer->shotGeoms = shotGeoms;
    atkGeoms = CreateRgGeomGroup();
    pPlayer->atkGeoms = atkGeoms;
    eyeGeoms = CreateRgGeomGroup();
    pPlayer->eyeGeoms = eyeGeoms;
    advGeoms = CreateRgGeomGroup();
    spec = pDat->spec;
    pPlayer->advGeoms = advGeoms;
    if (spec != 0) {
        RgRobotSetSpec(pPlayer->robot, spec);
    }
    RgRobotSetGeom(pPlayer->robot, RgGeomGroupCreateRobot(pPlayer->bodyGeoms, pPlayer->robot));
    RgRobotSetEyeGeom(pPlayer->robot, &RgGeomGroupCreateBall(pPlayer->eyeGeoms, pPlayer->robot)->point);
    advanceGeom = RgGeomGroupCreateBall(pPlayer->advGeoms, pPlayer->robot);
    RgGeomBallSetRadius(advanceGeom, RG_PLAYER_ADVANCE_RADIUS);
    RgRobotSetAdvanceGeom(pPlayer->robot, advanceGeom);
    InitXrgActorEssence(&actorEssence, pDat->actorID);
    RgRobotSetActor(pPlayer->robot, &actorEssence);
    sound = CreateXrgSound(RG_PLAYER_SOUND_TYPE, RgRobotGetCharID(pPlayer->robot));
    XrgSoundSetVolume(sound, RG_PLAYER_SOUND_VOLUME);
    RgRobotSetSoundDriver(pPlayer->robot, sound);
    _PlayerSetFromEssence(pPlayer, pDat);
}

RgPlayer *CreateRgPlayer(RgPlayerEssence *pDat)
{
    RgPlayer *pPlayer;

    pPlayer = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgPlayer),
                          playerSourcePath, 173);
    if (pPlayer == 0) {
        assert_prog(playerAssertPlayerNotNil, playerSourcePath, 174);
    }
    _InitRgPlayer(pPlayer, pDat);
    return pPlayer;
}

void DisposeRgPlayer(RgPlayer *pPlayer)
{
    RgRobotControl *pControl;

    if (pPlayer == 0) {
        assert_prog(playerAssertPlayerNotNil, playerSourcePath, 184);
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
    RgHeapFree(InstanceOfRgHeap(), pPlayer, playerSourcePath, 200);
}

void RgPlayerSetControl(RgPlayer *pPlayer, RgRobotControl *pControl)
{
    if (pPlayer == 0) {
        assert_prog(playerAssertPlayerNotNil, playerSourcePath, 211);
    }
    if (pPlayer->control != 0) {
        DisposeRgRobotControl(pPlayer->control);
    }
    pPlayer->control = pControl;
}

void RgPlayerSetTraceCamera(RgPlayer *pPlayer, RgCamera *pCamera)
{
    void *sound;

    if (pPlayer == 0) {
        assert_prog(playerAssertPlayerNotNil, playerSourcePath, 222);
    }
    pPlayer->traceCamera = pCamera;
    sound = CreateXrgSound(RG_PLAYER_SOUND_TYPE, RgRobotGetCharID(pPlayer->robot));
    XrgSoundSetVolume(sound, RG_PLAYER_SOUND_VOLUME);
    RgRobotSetSoundDriver(pPlayer->robot, sound);
}

void RgPlayerSetLightCost(RgPlayer *pPlayer)
{
    RgStatus *pRobot;
    XrgActor *pActor;

    if (pPlayer == 0) {
        assert_prog(playerAssertPlayerNotNil, playerSourcePath, 234);
    }
    pRobot = pPlayer->robot;
    if (pRobot != 0) {
        pActor = RgRobotGetActor(pRobot);
        if (pActor != 0) {
            XrgActorSetLightCost(pActor);
        }
    }
}

RgStatus *RgPlayerGetRobot(RgPlayer *pPlayer)
{
    if (pPlayer == 0) {
        assert_prog(playerAssertPlayerNotNil, playerSourcePath, 249);
    }
    return pPlayer->robot;
}

RgGeomGroup *RgPlayerGetBodyGeoms(RgPlayer *pPlayer)
{
    if (pPlayer == 0) {
        assert_prog(playerAssertPlayerNotNil, playerSourcePath, 256);
    }
    return pPlayer->bodyGeoms;
}

RgGeomGroup *RgPlayerGetShotGeoms(RgPlayer *pPlayer)
{
    if (pPlayer == 0) {
        assert_prog(playerAssertPlayerNotNil, playerSourcePath, 263);
    }
    return pPlayer->shotGeoms;
}

RgGeomGroup *RgPlayerGetAtkGeoms(RgPlayer *pPlayer)
{
    if (pPlayer == 0) {
        assert_prog(playerAssertPlayerNotNil, playerSourcePath, 270);
    }
    return pPlayer->atkGeoms;
}

RgGeomGroup *RgPlayerGetEyeGeoms(RgPlayer *pPlayer)
{
    if (pPlayer == 0) {
        assert_prog(playerAssertPlayerNotNil, playerSourcePath, 277);
    }
    return pPlayer->eyeGeoms;
}

RgGeomGroup *RgPlayerGetAdvGeoms(RgPlayer *pPlayer)
{
    if (pPlayer == 0) {
        assert_prog(playerAssertPlayerNotNil, playerSourcePath, 284);
    }
    return pPlayer->advGeoms;
}

/*
 * Runs the robot's control job, then, while a trace camera is attached, forwards the
 * robot's accepted commands and status flags to the camera as its actions.
 */
void RgPlayerControl(RgPlayer *pPlayer)
{
    RgStatus *pRobot;
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
        if (RgRobotIsAcceptedCommand(pRobot, RG_PLAYER_CMD_ADVANCE) != 0) {
            RgCameraSetActionAdvance(pCamera, 1);
        }
        if (RgRobotIsAcceptedCommand(pRobot, RG_PLAYER_CMD_ROLL) != 0) {
            RgCameraSetActionRoll(pCamera, 1);
        }
        if (RgRobotIsAcceptedCommand(pRobot, RG_PLAYER_CMD_ATTACK) != 0) {
            RgCameraSetActionAttack(pCamera, 1);
        }
        if (statusFlags & RG_PLAYER_STATUS_SEE_TARGET) {
            RgCameraSetActionSeeTarget(pCamera, 1);
        }
        if (statusFlags & RG_PLAYER_STATUS_LOCKED_ON) {
            RgCameraSetActionLockedOn(pCamera, 1);
        }
        if (statusFlags & RG_PLAYER_STATUS_DASH) {
            RgCameraSetActionDash(pCamera, 1);
        }
    }
}

void RgPlayerDisp(RgPlayer *pPlayer)
{
    if (pPlayer->robot != 0) {
        RgRobotDisp(pPlayer->robot);
    }
}

void RgPlayerPassTime(RgPlayer *pPlayer, float deltaTime)
{
    if (pPlayer->robot != 0) {
        RgRobotPassTime(pPlayer->robot, deltaTime);
    }
}
