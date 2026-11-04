/*
 * OV12 original TU 71: 0x00a3cd10..0x00a3e518 (26 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/act_1.h"
#include "main/act_2.h"
#include "main/xgl_font.h"
#include "main/xgl_light.h"
#include "main/xgl_studio.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_battle_common_data.h"
#include "ov12/rg_dispmodel.h"
#include "ov12/rg_error.h"
#include "ov12/rg_filesys.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_select_robot.h"
#include "ov12/xrg_actor.h"
#include "ov12/xrg_rand_int.h"

/* Completion codes of the xglCdReadFile callback. */
#define CD_READ_COMPLETE 4
#define CD_READ_ERROR_LIMIT (-2)

/* Mode of xglCdReadFile: read into the caller's buffer. */
#define CD_READ_MODE_BUFFER 1

/* Size of the heap arena of the preview actors, and of each file's region. */
#define SELECT_ROBOT_ARENA_SIZE 0xC00000U
#define SELECT_ROBOT_REGION_SIZE 0xC0000

/* Size of the weapon child records, which stop before heapBuffer. */
#define SELECT_ROBOT_CHILD_SIZE 356

#define SELECT_ROBOT_NO_ACTOR (-1)

/* Preview actor setup and fading. */
#define SELECT_ROBOT_EX_MOTION_BLEND 8
#define SELECT_ROBOT_WEAPON_MOTION_BASE 2048
#define SELECT_ROBOT_ALPHA 68
#define SELECT_ROBOT_SHADOW_KIND 7
#define SELECT_ROBOT_SHADOW_SIZE 48
#define SELECT_ROBOT_FADE_STEP 0.2f
#define SELECT_ROBOT_DISPOSE_DELAY 3
#define SELECT_ROBOT_YAW_FOLLOW 0.2f
#define SELECT_ROBOT_WEAPON_BLINK_FRAMES 10
#define SELECT_ROBOT_PI 3.1415927f
#define SELECT_ROBOT_TWO_PI 6.2831855f

typedef union AlignedLightVector {
    double alignment;
    Vector4 value;
    float components[4];
} AlignedLightVector;

typedef struct AlignedLightBlock {
    AlignedLightVector vectors[4];
} AlignedLightBlock;

/* main/xgl_cd.c's CD read cancellation (src/main/xgl_hdd.h declares it with an int result). */
extern void xglCdReadCancel(void);

/*
 * main/xgl_light.c takes the intensity as an XglLightQuadword; this TU's
 * 8-aligned AlignedLightVector blocks (measured: a 16-aligned union changes
 * the frame and the data alignment) are passed as their Vector4 member.
 */
extern void xglLightIntensityAmbient(StudioLight *light,
                                     const Vector4 *intensity);
extern void xglLightIntensityParallel(StudioLight *light, unsigned int index,
                                     const Vector4 *intensity);
/* Defined elsewhere (scaffold data): file-name and assertion strings. */
extern const char selectRobotAssertFileNumWithinDataMax[];
extern const char selectRobotFloorModelFileName[];
extern const char selectRobotDataDirectory[];
extern const char selectRobotFloorModelName[];
extern const char selectRobotAssertActStatOk[];
extern const char selectRobotAssertActDataNumAtLeastThree[];
extern const char selectRobotAssertActActorNil[];
extern const char selectRobotAssertNewNotNil[];
extern const char selectRobotAssertWepEssNotNil[];
extern const char selectRobotUnknownActStatError[];
extern const char selectRobotUnknownRobotKindFormat[];
extern const char selectRobotAssertActBufferNotNil[];
extern const char selectRobotDebugStateFormat[];
extern const char selectRobotDebugWeaponFormat[];

/* Declared here: the load state (defined below, in this order; the order
 * fixes the layout of the .bss). */
static int s_bLoading;
static unsigned int s_uReqNum;
static unsigned int s_uOkNum;
static void *s_pWhoAreYou;

static void _InitSelRob(RgSelectRobot *pCont);
static void _DestructSelRob(RgSelectRobot *pCont);
static void _JobAct(RgSelectRobot *pCont, float deltaTime);
static void *_InitAct(RgSelectRobot *pCont, unsigned char *cursor,
                      int regionBytes, int count);
static void _ActorPosUpdate(RgSelectRobot *pCont);
static void _ActivateAct(RgSelectRobot *pCont);
static void _ReqAct(RgSelectRobot *pCont, XrgActorEssence *essence,
                    RgWeaponEssence **weapons, int *extra);
static void _DestructAct(RgSelectRobot *pCont);
static void _DrawAct(RgSelectRobot *pCont);
static int _ReqLoad(const char *const *names, void *const *buffers,
                    unsigned int count, void *owner);
/* The published _DumpHead takes no parameter; the original call passes
   one node pointer, which its empty body ignores. */
static void _DumpHead();

static int s_bLoading = 0;

static unsigned int s_uReqNum;
static unsigned int s_uOkNum;
static char s_aszData[RG_SELECT_ROBOT_FILE_MAX][RG_SELECT_ROBOT_NAME_SIZE];
static void *s_apBuf[RG_SELECT_ROBOT_FILE_MAX];
static void *s_pWhoAreYou;

const char selectRobotWhoAreYouWarning[] = "who are you ?";

const char selectRobotSourcePath[] = "../rg_select_robot.euc.c";

const char selectRobotAssertActNotNil[] = "pAct != NIL";

const AlignedLightBlock selectRobotLightIntensityBlock = {
    .vectors = {
        { .components = { 0x1.0p-2f, 0x1.0p-2f, 0x1.0p-2f, 0x1.0p+0f } },
        { .components = { 0x1.99999ap-1f, 0x1.99999ap-1f, 0x1.99999ap-1f, 0x1.0p+0f } },
        { .components = { 0x1.0p-1f, 0x1.0p-1f, 0x1.0p-1f, 0x1.0p+0f } },
        { .components = { 0x1.0p-1f, 0x1.0p-1f, 0x1.0p-1f, 0x1.0p+0f } }
    }
};

const char selectRobotAssertContNotNil[16] = "pCont != NIL";

/*
 * xglCdReadFile's completion callback: only completion code 4 does
 * anything, and only when a requester (s_pWhoAreYou) is on file to receive
 * the count; any other requester-less code-4 completion is logged instead
 * of counted.  Codes below -2 and codes 1..3 are guarded out and ignored.
 */
static void _ReadCallback(int completionCode)
{
    if (completionCode < CD_READ_ERROR_LIMIT) {
        return;
    }
    if (completionCode < CD_READ_COMPLETE) {
        return;
    }
    if (completionCode != CD_READ_COMPLETE) {
        return;
    }
    if (s_pWhoAreYou != 0) {
        s_uOkNum++;
    } else {
        RgWarn(selectRobotWhoAreYouWarning, selectRobotSourcePath, 55);
    }
    s_bLoading = 0;
}

static void _KillLoad(void)
{
    if (s_bLoading != 0) {
        xglCdReadCancel();
        s_bLoading = 0;
    }
    s_uReqNum = (s_uOkNum = 0);
    s_pWhoAreYou = 0;
}

static void _KillLoadIfMe(void *owner)
{
    if (s_uReqNum != 0) {
        if (s_pWhoAreYou == owner) {
            _KillLoad();
        }
    }
}

/*
 * Cancels the current asynchronous load, then installs the next request:
 * names[i] is copied into slot i and buffers[i] becomes its buffer.  A
 * request held by another owner refuses the call and returns 0.
 */
static int _ReqLoad(const char *const *names, void *const *buffers,
                    unsigned int count, void *owner)
{
    unsigned int i;

    if (count > RG_SELECT_ROBOT_FILE_MAX) {
        assert_prog(selectRobotAssertFileNumWithinDataMax, selectRobotSourcePath, 94);
    }
    if (s_pWhoAreYou == owner || s_pWhoAreYou == 0) {
        _KillLoad();
        for (i = 0; i < count; i++) {
            strcpy(s_aszData[i], names[i]);
            s_apBuf[i] = buffers[i];
        }
        s_uReqNum = count;
        s_pWhoAreYou = owner;
        s_uOkNum = 0;
        return 1;
    }
    return 0;
}

static int _IsEndOfLoad(void)
{
    if (s_bLoading == 0) {
        if (s_uReqNum != 0) {
            if (s_uOkNum >= s_uReqNum) return 1;
        }
    }
    return 0;
}

static void _JobLoad(void)
{
    if (s_uReqNum != 0 && s_uOkNum < s_uReqNum) {
        if (s_bLoading == 0 &&
            xglCdReadFile(s_aszData[s_uOkNum], s_apBuf[s_uOkNum],
                          CD_READ_MODE_BUFFER, (int) _ReadCallback) >= 0) {
            s_bLoading = 1;
        }
        return;
    }
    s_pWhoAreYou = 0;
}

/*
 * Reset the paired request-completion counters through the helper interface
 * used by the load-state owner.  Its parameter order is completion then
 * request, while the reset order is request then completion.
 */
static __inline__ void ResetLoadCounts(unsigned int *completed_count,
                                      unsigned int *request_count)
{
    *request_count = 0;
    *completed_count = 0;
}

/* Original OV12 local function at 0x00a3d008. */
static void _InitLoad(void)
{
    s_bLoading = 0;
    ResetLoadCounts(&s_uOkNum, &s_uReqNum);
    s_pWhoAreYou = 0;
    xglCdReadCancel();
}

static void *_InitAct(RgSelectRobot *pCont, unsigned char *cursor,
                      int regionBytes, int count)
{
    RgSelectRobot *child;
    unsigned int i;

    if (pCont == 0) {
        assert_prog(selectRobotAssertActNotNil, selectRobotSourcePath, 183);
    }
    pCont->screenPos = 0.0f;
    pCont->actorID = SELECT_ROBOT_NO_ACTOR;
    pCont->state = RG_SELECT_ROBOT_STATE_IDLE;
    pCont->actor = 0;
    pCont->nameCount = 0;
    pCont->parent = 0;
    pCont->drawCount = 0;
    pCont->displayYaw = pCont->screenPos;
    pCont->displayPosition = pCont->screenPos;

    for (i = 0; i < RG_SELECT_ROBOT_FILE_MAX; i++) {
        pCont->names[i][0] = 0;
        pCont->buffers[i] = cursor;
        cursor += regionBytes;
    }
    if (count != 0) {
        pCont->hasWeapons = 1;
        for (i = 0; i < RG_SELECT_ROBOT_WEAPON_NUM; i++) {
            child = RgHeapAlloc(InstanceOfRgHeap(), SELECT_ROBOT_CHILD_SIZE,
                                selectRobotSourcePath, 204);
            pCont->weapons[i] = child;
            cursor = _InitAct(child, cursor, regionBytes, 0);
            child->parent = pCont;
            child->weaponSlot = i;
            pCont->weaponActive[i] = 0;
            child->weapon = 0;
        }

        pCont->fileData =
            RgFileSysRead(InstanceOfRgFileSys(), selectRobotFloorModelFileName, selectRobotDataDirectory);
        if (pCont->fileData != 0) {
            pCont->displayModel =
                CreateXrgDispModelImpl(pCont->fileData->data, selectRobotFloorModelName);
        }
    } else {
        pCont->displayModel = 0;
        pCont->hasWeapons = 0;
        do {
            pCont->fileData = 0;
        } while (0);
    }
    return cursor;
}

void _select_light(StudioLight *light)
{
    static AlignedLightBlock asDirection = {
        .vectors = {
            { .components = { 0.0f, 0.0f, 0.0f, 0x1.0p+0f } },
            { .components = { -0x1.0c3e5p-2f, 0x1.0ffa8ep-1f, -0x1.45216cp-6f, 0x1.0p+0f } },
            { .components = { 0x1.2d97c8p+1f, 0x1.45facp+0f, 0x1.921fb6p+1f, 0x1.0p+0f } },
            { .components = { 0x1.71e67ep-2f, 0x1.e350ap+1f, 0x1.d38938p-6f, 0x1.0p+0f } }
        }
    };
    AlignedLightBlock intensity;
    int i;

    intensity.vectors[0] = selectRobotLightIntensityBlock.vectors[0];
    intensity.vectors[1] = selectRobotLightIntensityBlock.vectors[1];
    intensity.vectors[2] = selectRobotLightIntensityBlock.vectors[2];
    intensity.vectors[3] = selectRobotLightIntensityBlock.vectors[3];

    for (i = 0; i < 4; i++) {
        intensity.vectors[i].components[0] *= 0.8f;
        intensity.vectors[i].components[1] *= 0.8f;
        intensity.vectors[i].components[2] *= 0.8f;
    }

    for (i = 0; i < 4; i++) {
        if (i == 0) {
            xglLightIntensityAmbient(light, &intensity.vectors[0].value);
        } else {
            xglLightIntensityParallel(light, i - 1,
                                      &intensity.vectors[i].value);
            xglLightAngle(light, i - 1,
                          &asDirection.vectors[i].value);
        }
    }
}

/* The two positions _ActorPos interpolates between, weighted by
   RgSelectRobot::screenPos: index 1 is the weighted argument, index 0 the
   complement-weighted one. */
static void _ActorPos(RgSelectRobot *pCont, RgVector position)
{
    static RgVector s_aPos[2] = {
        { 0x1.333334p+1f, -0x1.8p+0f, -0x1.a66666p+2f, 0.0f },
        { -0x1.333334p+1f, -0x1.8p+0f, -0x1.a66666p+2f, 0.0f }
    };
    XrgLinearIntpVector(position, s_aPos[1], s_aPos[0], pCont->screenPos);
}

static void _ActorPosUpdate(RgSelectRobot *pCont)
{
    Actor *actor;
    RgVector position;
    float yawDelta;
    float yaw;
    float wrappedYaw;

    if (pCont->actor == 0) {
        return;
    }
    if (pCont->parent != 0) {
        return;
    }
    _ActorPos(pCont, position);
    actor = pCont->actor;
    actor->position.x = position[0];
    actor->position.y = position[1];
    actor->position.z = position[2];
    actor->rotation.x = 0.0f;
    actor->rotation.y = pCont->displayYaw;
    actor->rotation.z = 0.0f;
    ACT_updateMotion(actor);
    yawDelta = pCont->displayPosition - pCont->displayYaw;
    while (yawDelta > SELECT_ROBOT_PI) {
        yawDelta -= SELECT_ROBOT_TWO_PI;
    }
    while (yawDelta < -SELECT_ROBOT_PI) {
        yawDelta += SELECT_ROBOT_TWO_PI;
    }
    yaw = pCont->displayYaw + yawDelta * SELECT_ROBOT_YAW_FOLLOW;
    pCont->displayYaw = yaw;
    wrappedYaw = yaw;
    while (wrappedYaw > SELECT_ROBOT_PI) {
        wrappedYaw -= SELECT_ROBOT_TWO_PI;
    }
    while (wrappedYaw < -SELECT_ROBOT_PI) {
        wrappedYaw += SELECT_ROBOT_TWO_PI;
    }
    pCont->displayYaw = wrappedYaw;
}

static void _ActivateAct(RgSelectRobot *pCont)
{
    Actor *actor;
    RgSelectRobot *parent;
    void *modelResource;
    void *matrixResource;
    void *moveResource;
    void *extraResource;
    void *exMotion;
    unsigned int i;
    unsigned int code;
    int value;
    void *weapon;

    if (pCont == 0) {
        assert_prog(selectRobotAssertActNotNil, selectRobotSourcePath, 324);
    }
    if (pCont->state != RG_SELECT_ROBOT_STATE_ACTIVE) {
        assert_prog(selectRobotAssertActStatOk, selectRobotSourcePath, 325);
    }
    if (pCont->nameCount < 3) {
        assert_prog(selectRobotAssertActDataNumAtLeastThree, selectRobotSourcePath, 326);
    }
    if (pCont->actor != 0) {
        assert_prog(selectRobotAssertActActorNil, selectRobotSourcePath, 327);
    }
    parent = pCont->parent;
    if (parent != 0) {
        if (parent->state != RG_SELECT_ROBOT_STATE_ACTIVE) {
            return;
        }
        if (parent->actor == 0) {
            return;
        }
    }
    matrixResource = pCont->buffers[0];
    modelResource = pCont->buffers[1];
    moveResource = pCont->buffers[2];
    extraResource = 0;
    if (pCont->nameCount == RG_SELECT_ROBOT_FILE_MAX) {
        extraResource = pCont->buffers[3];
    }
    _DumpHead(modelResource);
    _DumpHead(matrixResource);
    _DumpHead(moveResource);
    actor = ACT_create(-1, 1);
    pCont->actor = actor;
    if (actor == 0) {
        assert_prog(selectRobotAssertNewNotNil, selectRobotSourcePath, 346);
    }
    for (i = 0; i < ACTOR_RESOURCE_COUNT; i++) {
        ACTOR_RESOURCES(actor)[i] = 0;
    }
    actor->modelResource = modelResource;
    actor->matrixResource = matrixResource;
    actor->move = moveResource;
    ACT_allocMatrix(actor, -1);
    ACT_setModelWrapper(actor, 0);
    _select_light(&actor->light);
    actor->flags |= 0x8000 | ACTOR_FLAG_TRANSLUCENT;
    actor->drawEnv.flags |= 3;
    actor->drawEnv.alpha = SELECT_ROBOT_ALPHA;
    actor->drawEnv.transparency = 0.0f;
    ACT_initMotion(actor);
    if ((exMotion = extraResource) != 0) {
        ACT_initExMotion(actor, 0, exMotion);
        ACT_setMotion2(actor, 0, SELECT_ROBOT_EX_MOTION_BLEND);
    }
    actor->shadow_kind = SELECT_ROBOT_SHADOW_KIND;
    actor->shadowSize = SELECT_ROBOT_SHADOW_SIZE;
    if (pCont->parent != 0) {
        RgSelectRobot *up = pCont->parent;
        RgWeaponEssence *table = pCont->weapon;
        int slot = pCont->weaponSlot;
        int mount = RgEquipMountPos(&table->equip, slot, up->actorID,
                                    table->databaseName);
        int jnt = RgEquipPosToJntID(mount);
        int acc = XrgActorJntIDtoAccID(jnt);
        ACT_setArms(actor, up->actor, acc, 0);
        code = up->actorID;
        if (code < RG_SELECT_CHAR_NUM) {
            value = table->shotEndMotionTable[code][slot];
            if (value != -1) {
                if (value >= SELECT_ROBOT_WEAPON_MOTION_BASE) {
                    if ((weapon = RgBattleCommonDataGetWeaponMot(InstanceOfRgBattleCommonData())) != 0) {
                        ACT_initExMotion(actor, 0, weapon);
                        ACT_setMotion2(actor,
                                       value - SELECT_ROBOT_WEAPON_MOTION_BASE,
                                       SELECT_ROBOT_EX_MOTION_BLEND);
                    }
                }
            }
        }
    }
    if (pCont->parent != 0) {
        _ActorPosUpdate(pCont->parent);
    }
    _ActorPosUpdate(pCont);
}

static void _ReqAct(RgSelectRobot *pCont, XrgActorEssence *essence,
                    RgWeaponEssence **weapons, int *extra)
{
    unsigned int robotKind;
    char nameBuf[RG_SELECT_ROBOT_FILE_MAX][RG_SELECT_ROBOT_NAME_SIZE];
    int count;
    unsigned int i;
    unsigned int code;
    int failed;

    code = -1;
    failed = 0;
    if (pCont == 0) {
        assert_prog(selectRobotAssertActNotNil, selectRobotSourcePath, 447);
    }
    if (extra != 0) {
        for (i = 0; i < RG_SELECT_ROBOT_WEAPON_NUM; i++) {
            pCont->weaponActive[i] = extra[i];
        }
    }
    if (essence == 0 || essence->actorID == SELECT_ROBOT_NO_ACTOR) {
        count = 0;
        failed = 1;
        for (i = 0; i < RG_SELECT_ROBOT_FILE_MAX; i++) {
            pCont->names[i][0] = 0;
        }
    } else {
        code = essence->actorID;
        count = XrgActorEssenceGetFileName(essence, nameBuf[0]);
        for (i = 0; i < count; i++) {
            if (strcmp(pCont->names[i], nameBuf[i]) != 0) {
                failed = 1;
                break;
            }
        }
    }

    if (failed == 0) {
        if (pCont->hasWeapons == 0) {
            return;
        }
        if (weapons == 0) {
            assert_prog(selectRobotAssertWepEssNotNil, selectRobotSourcePath, 479);
        }
        for (i = 0; i < RG_SELECT_ROBOT_WEAPON_NUM; i++) {
            _ReqAct(pCont->weapons[i], &weapons[i]->actorSpec[i], 0, 0);
            pCont->weapons[i]->weapon = weapons[i];
        }
        return;
    }

    robotKind = pCont->state;
    pCont->actorID = code;
    switch (robotKind) {
    case RG_SELECT_ROBOT_STATE_ACTIVE:
        if (pCont->actor != 0) {
            Actor *actor = pCont->actor;
            RgSelectRobot *parent;
            unsigned int bits;
            int clearMask;

            pCont->disposeCounter = SELECT_ROBOT_DISPOSE_DELAY;
            clearMask = ~ACTOR_FLAGS_INITIAL;
            parent = pCont->parent;
            bits = actor->flags;
            pCont->state = RG_SELECT_ROBOT_STATE_DISPOSING;
            actor->flags = (bits | ACTOR_FLAG_HIDDEN) & clearMask;
            if (parent != 0) {
                ACT_resetParent(actor, parent->actor);
            }
        } else {
            pCont->state = RG_SELECT_ROBOT_STATE_IDLE;
        }
        /* fall through */
    case RG_SELECT_ROBOT_STATE_IDLE:
    case RG_SELECT_ROBOT_STATE_LOADING:
    case RG_SELECT_ROBOT_STATE_DISPOSING:
        pCont->nameCount = count;
        for (i = 0; i < count; i++) {
            strcpy(pCont->names[i], nameBuf[i]);
        }
        if (pCont->state == RG_SELECT_ROBOT_STATE_LOADING) {
            pCont->state = RG_SELECT_ROBOT_STATE_IDLE;
        }
        _KillLoadIfMe(pCont);
        break;
    default:
        RgError(selectRobotUnknownActStatError, selectRobotSourcePath, 521);
        break;
    }

    if (pCont->hasWeapons != 0) {
        for (i = 0; i < RG_SELECT_ROBOT_WEAPON_NUM; i++) {
            RgSelectRobot *child = pCont->weapons[i];
            _ReqAct(child, 0, 0, 0);
            robotKind = child->state;
            if ((unsigned int)(robotKind - RG_SELECT_ROBOT_STATE_LOADING) < 2) {
                RgError(selectRobotUnknownRobotKindFormat, selectRobotSourcePath, 531, robotKind);
            }
        }
    }
}

static void _DestructAct(RgSelectRobot *pCont)
{
    unsigned int i;

    if (pCont == 0) {
        assert_prog(selectRobotAssertActNotNil, selectRobotSourcePath, 540);
    }

    if (pCont->actor != 0) {
        pCont->actor->flags =
            (pCont->actor->flags | ACTOR_FLAG_HIDDEN) & ~ACTOR_FLAGS_INITIAL;
    }

    if (pCont->hasWeapons != 0) {
        for (i = 0; i < RG_SELECT_ROBOT_WEAPON_NUM; i++) {
            if (pCont->weapons[i] == 0) {
                continue;
            }
            if (pCont->weapons[i]->actor != 0) {
                pCont->weapons[i]->actor->flags =
                    (pCont->weapons[i]->actor->flags | ACTOR_FLAG_HIDDEN) &
                    ~ACTOR_FLAGS_INITIAL;
            }
            RgHeapFree(InstanceOfRgHeap(), pCont->weapons[i], selectRobotSourcePath, 553);
        }
    }

    if (pCont->displayModel != 0) {
        DisposeRgDispModel(pCont->displayModel);
    }
    if (pCont->fileData != 0) {
        DisposeRgFileSysData_sub(pCont->fileData, selectRobotSourcePath, 561);
    }
}

static void _JobAct(RgSelectRobot *pCont, float deltaTime)
{
    const char *names[RG_SELECT_ROBOT_FILE_MAX];
    void *buffers[RG_SELECT_ROBOT_FILE_MAX];
    unsigned int i;
    unsigned int child;
    float timer;
    unsigned int id;

    if (pCont == 0) {
        assert_prog(selectRobotAssertActNotNil, selectRobotSourcePath, 568);
    }
    id = pCont->state;
    switch (id) {
        case RG_SELECT_ROBOT_STATE_IDLE:
            if (pCont->nameCount != 0) {
                for (i = 0; i < pCont->nameCount; i++) {
                    if (pCont->buffers[i] == 0) {
                        assert_prog(selectRobotAssertActBufferNotNil, selectRobotSourcePath, 576);
                    }
                    names[i] = pCont->names[i];
                    buffers[i] = pCont->buffers[i];
                }
                if (pCont->parent == 0 ||
                    pCont->parent->state == RG_SELECT_ROBOT_STATE_ACTIVE) {
                    if (_ReqLoad(names, buffers, pCont->nameCount, pCont) != 0) {
                        pCont->state = RG_SELECT_ROBOT_STATE_LOADING;
                    }
                }
            }
            break;
        case RG_SELECT_ROBOT_STATE_LOADING:
            if (_IsEndOfLoad()) {
                pCont->state = RG_SELECT_ROBOT_STATE_ACTIVE;
            }
            break;
        case RG_SELECT_ROBOT_STATE_ACTIVE: {
            Actor *actor = pCont->actor;
            if (actor == 0) {
                if (pCont->nameCount != 0) {
                    _ActivateAct(pCont);
                }
            } else {
                timer = actor->drawEnv.transparency + SELECT_ROBOT_FADE_STEP;
                if (1.0f < timer) {
                    actor->drawEnv.flags &= ~1u;
                    timer = 1.0f;
                }
                actor->drawEnv.transparency = timer;
                _ActorPosUpdate(pCont);
            }
            break;
        }
        case RG_SELECT_ROBOT_STATE_DISPOSING: {
            Actor *actor = pCont->actor;
            if (actor != 0) {
                if (pCont->disposeCounter-- <= 0) {
                    ACT_dispose(actor);
                    pCont->actor = 0;
                    pCont->state = RG_SELECT_ROBOT_STATE_IDLE;
                }
            }
            break;
        }
    default:
        break;
    }
    if (pCont->hasWeapons != 0) {
        for (child = 0; child < RG_SELECT_ROBOT_WEAPON_NUM; child++) {
            _JobAct(pCont->weapons[child], deltaTime);
        }
    }
}

static void _DrawAct(RgSelectRobot *pCont)
{
    unsigned int count;
    unsigned int i;
    unsigned int index;
    RgVector position;
    RgMatrix matrix;
    int ids[RG_SELECT_ROBOT_WEAPON_NUM];

    if (pCont == 0) {
        assert_prog(selectRobotAssertActNotNil, selectRobotSourcePath, 653);
    }
    if (pCont->displayModel != 0) {
        _ActorPos(pCont, position);
        XrgUnitMatrix(matrix);
        XrgRotMatrixY(matrix, matrix, pCont->displayYaw);
        matrix[12] = position[0];
        matrix[13] = position[1];
        matrix[14] = position[2];
        RgDispModelSetLocal(pCont->displayModel, matrix);
        RgDispModelDisplay(pCont->displayModel);
    }
    if (pCont->hasWeapons != 0) {
        count = 0;
        for (i = 0; i < RG_SELECT_ROBOT_WEAPON_NUM; i++) {
            if (pCont->weaponActive[i] != 0) {
                ids[count] = i;
                count++;
            } else {
                Actor *actor;
                RgSelectRobot *weapon;

                weapon = pCont->weapons[i];
                actor = weapon->actor;
                if (weapon->state == RG_SELECT_ROBOT_STATE_ACTIVE) {
                    if (actor != 0) {
                        actor->flags &= ~ACTOR_FLAG_HIDDEN;
                    }
                }
            }
        }
        pCont->drawCount++;
        if (count != 0) {
            index = (pCont->drawCount / SELECT_ROBOT_WEAPON_BLINK_FRAMES) % (count + 1);
            for (i = 0; i < count; i++) {
                Actor *actor;
                RgSelectRobot *weapon = pCont->weapons[ids[i]];

                actor = weapon->actor;
                if (weapon->state == RG_SELECT_ROBOT_STATE_ACTIVE) {
                    if (actor != 0) {
                        if (index == i) {
                            actor->flags &= ~ACTOR_FLAG_HIDDEN;
                        } else {
                            actor->flags |= ACTOR_FLAG_HIDDEN;
                        }
                    }
                }
            }
        }
    }
}

void _InitSelRob(RgSelectRobot *pCont)
{
    void *heapBuffer;

    if (pCont == 0) {
        assert_prog(selectRobotAssertContNotNil, selectRobotSourcePath, 720);
    }
    _InitLoad();
    heapBuffer = RgHeapAlloc(InstanceOfRgHeapData(), SELECT_ROBOT_ARENA_SIZE,
                             selectRobotSourcePath, 726);
    pCont->heapBuffer = heapBuffer;
    _InitAct(pCont, heapBuffer, SELECT_ROBOT_REGION_SIZE, 1);
}

void _DestructSelRob(RgSelectRobot *pCont)
{
    if (pCont == 0) {
        assert_prog(selectRobotAssertContNotNil, selectRobotSourcePath, 734);
    }
    _KillLoad();
    _DestructAct(pCont);
    RgHeapFree(InstanceOfRgHeapData(), pCont->heapBuffer, selectRobotSourcePath, 737);
    ACT_init();
}

/* pStudio is accepted and ignored. */
RgSelectRobot *CreateRgSelectRobot(struct RgDrawStudio *pStudio)
{
    RgSelectRobot *pCont;

    pCont = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgSelectRobot), selectRobotSourcePath,
                        745);
    _InitSelRob(pCont);
    return pCont;
}

void DisposeRgSelectRobot(RgSelectRobot *pCont)
{
    if (pCont == 0) {
        assert_prog(selectRobotAssertContNotNil, selectRobotSourcePath, 753);
    }
    _DestructSelRob(pCont);
    RgHeapFree(InstanceOfRgHeap(), pCont, selectRobotSourcePath, 755);
    xglCdReadCancel();
}

void RgSelectRobotScreenPos(RgSelectRobot *pCont, float screenPos)
{
    if (pCont == 0) {
        assert_prog(selectRobotAssertContNotNil, selectRobotSourcePath, 766);
    }
    pCont->screenPos = screenPos;
}

void RgSelectRobotSetMode(RgSelectRobot *pCont, int mode)
{
    if (pCont == 0) {
        assert_prog(selectRobotAssertContNotNil, selectRobotSourcePath, 774);
    }
    switch (mode) {
    case -1:
        pCont->displayPosition = 0.0f;
        break;
    case 0:
        pCont->displayPosition = -0.98172f;
        break;
    case 1:
        pCont->displayPosition = 1.178125f;
        break;
    case 2:
        pCont->displayPosition = -2.748864f;
        break;
    default:
        break;
    }
}

void RgSelectRobotSet(RgSelectRobot *pCont, struct RgSelectRobotChoice *selection)
{
    XrgActorEssence essence;

    if (pCont == 0) {
        assert_prog(selectRobotAssertContNotNil, selectRobotSourcePath, 801);
    }
    InitXrgActorEssence(&essence, selection->actorID);
    _ReqAct(pCont, &essence, selection->weapons, selection->weaponActive);
}

void RgSelectRobotPassTime(RgSelectRobot *pCont, float deltaTime)
{
    if (pCont == 0) {
        assert_prog(selectRobotAssertContNotNil, selectRobotSourcePath, 820);
    }
    _JobAct(pCont, deltaTime);
}

void RgSelectRobotDisp(RgSelectRobot *pCont)
{
    unsigned int i;
    int weapon;

    if (pCont == 0) {
        assert_prog(selectRobotAssertContNotNil, selectRobotSourcePath, 0x33D);
    }
    xglFontDebugPrintf(0, 8, selectRobotDebugStateFormat, pCont->state);
    /* y and the weapon pointer are left to gcc's induction variables. */
    for (i = 0; i < RG_SELECT_ROBOT_WEAPON_NUM; i++) {
        weapon = pCont->weapons[i]->state;
        xglFontDebugPrintf(0, 16 + i * 8, selectRobotDebugWeaponFormat, i, weapon);
    }
    ACT_draw();
    _DrawAct(pCont);
    _JobLoad();
}

static void _DumpHead(void)
{
}



const char selectRobotAssertFileNumWithinDataMax[24] = "uFileNum <= DAT_MAX";

const char selectRobotFloorModelFileName[16] = "sel_yuka.rbg";

const char selectRobotDataDirectory[16] = "data\\nisimori\\";

const char selectRobotFloorModelName[8] = "SA_yuka";

const char selectRobotAssertActStatOk[24] = "pAct->m_eStat == OK";

const char selectRobotAssertActDataNumAtLeastThree[24] = "pAct->m_uDataNum >= 3";

const char selectRobotAssertActActorNil[24] = "pAct->m_pActor == NIL";

const char selectRobotAssertNewNotNil[16] = "pNew != NIL";

const char selectRobotAssertWepEssNotNil[16] = "apWepEss != NIL";

const char selectRobotUnknownActStatError[24] = "unknow Act stat %d";

const char selectRobotUnknownRobotKindFormat[16] = "hoge \? %d";

const char selectRobotAssertActBufferNotNil[24] = "pAct->m_apBuf[i] != NIL";

const char selectRobotDebugStateFormat[8] = "ROB %d";

const char selectRobotDebugWeaponFormat[16] = "WEP(%d) %d";
