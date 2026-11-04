/*
 * OV12 original TU 71: 0x00a3cd10..0x00a3e518 (26 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_select_robot.h"
#include "main/xgl_studio.h"

static int s_bLoading = 0;

static unsigned int s_uReqNum;
static unsigned int s_uOkNum;
static char s_aszData[4][64];
static void *s_apBuf[4];
static void *s_pWhoAreYou;

typedef union AlignedLightVector {
    double alignment;
    Vector4 value;
    float components[4];
} AlignedLightVector;

typedef struct AlignedLightBlock {
    AlignedLightVector vectors[4];
} AlignedLightBlock;

const char D_00A56D90[] = "who are you ?";

const char D_00A56DA0[] = "../rg_select_robot.euc.c";

const char D_00A56DD8[] = "pAct != NIL";

const AlignedLightBlock D_00A56E10 = {
    .vectors = {
        { .components = { 0x1.0p-2f, 0x1.0p-2f, 0x1.0p-2f, 0x1.0p+0f } },
        { .components = { 0x1.99999ap-1f, 0x1.99999ap-1f, 0x1.99999ap-1f, 0x1.0p+0f } },
        { .components = { 0x1.0p-1f, 0x1.0p-1f, 0x1.0p-1f, 0x1.0p+0f } },
        { .components = { 0x1.0p-1f, 0x1.0p-1f, 0x1.0p-1f, 0x1.0p+0f } }
    }
};

const char D_00A56EF8[] = "pCont != NIL";

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgHeapFree(void *heap, void *ptr, const char *source_file,
                       int line);
static void _InitSelRob(RgSelectRobot *pCont);
static void _DestructSelRob(RgSelectRobot *pCont);
static void _JobAct(RgSelectRobot *pCont, float deltaTime);
/* jal RgWarn(format, file, line, ...), the XrgLogSys-shaped debug warning
   (also declared this way by src/ov12/rg_main.c, its accepted caller). */
extern void RgWarn(const char *format, const char *file, int line, ...);

/*
 * xglCdReadFile's completion callback: only completion code 4 does
 * anything, and only when a requester (s_pWhoAreYou) is on file to receive
 * the count; any other requester-less code-4 completion is logged instead
 * of counted.  Codes below -2 and codes 1..3 are guarded out and ignored.
 */
static void _ReadCallback(int completionCode)
{
    if (completionCode < -2) {
        return;
    }
    if (completionCode < 4) {
        return;
    }
    if (completionCode != 4) {
        return;
    }
    if (s_pWhoAreYou != 0) {
        s_uOkNum++;
    } else {
        RgWarn(D_00A56D90, D_00A56DA0, 55);
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

extern char *strcpy(char *destination, const char *source);
extern const char D_00A56DC0[];

/*
 * Cancels the current asynchronous load, then installs the next request:
 * names[i] is copied into slot i and buffers[i] becomes its buffer.  A
 * request held by another owner refuses the call and returns 0.
 */
static int _ReqLoad(const char *const *names, void *const *buffers,
                    unsigned int count, void *owner)
{
    unsigned int i;

    if (count >= 5) {
        assert_prog(D_00A56DC0, D_00A56DA0, 94);
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

extern int xglCdReadFile(const char *name, void *buffer, int mode, int flags);

static void _JobLoad(void)
{
    if (s_uReqNum != 0 && s_uOkNum < s_uReqNum) {
        if (s_bLoading == 0 &&
            xglCdReadFile(s_aszData[s_uOkNum], s_apBuf[s_uOkNum], 1,
                          (int) _ReadCallback) >= 0) {
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

#include "main/act_1.h"

extern RgFileSys *InstanceOfRgFileSys(void);
extern RgFileSysData *RgFileSysRead(RgFileSys *pSys, const char *pszName,
                                    const char *pszRoot);
extern RgDispModel *CreateXrgDispModelImpl(const char *name,
                                           const char *variant);
extern const char D_00A56DE8[];
extern const char D_00A56DF8[];
extern const char D_00A56E08[];

static void *_InitAct(RgSelectRobot *pCont, unsigned char *cursor, int regionBytes, int count)
{
    RgSelectRobot *child;
    unsigned int i;

    if (pCont == 0) {
        assert_prog(D_00A56DD8, D_00A56DA0, 183);
    }
    pCont->screenPos = 0.0f;
    pCont->bufferSlots[4] = (void *)-1;
    pCont->robotID = 0;
    pCont->actorFlags = 0;
    pCont->nameCount = 0;
    pCont->parent = 0;
    pCont->drawCount = 0;
    pCont->displayYaw = pCont->screenPos;
    pCont->displayPosition = pCont->screenPos;

    for (i = 0; i < 4; i++) {
        pCont->names[i][0] = 0;
        pCont->bufferSlots[i] = cursor;
        cursor += regionBytes;
    }
    if (count != 0) {
        pCont->hasActions = 1;
        for (i = 0; i < 3; i++) {
            child = RgHeapAlloc(InstanceOfRgHeap(), 356, D_00A56DA0, 204);
            pCont->actions[i] = (RgSelectAction *)child;
            cursor = _InitAct(child, cursor, regionBytes, 0);
            child->parent = pCont;
            child->equipSlot = i;
            pCont->actionActive[i] = 0;
            child->equipTable = 0;
        }

        pCont->fileData = RgFileSysRead(InstanceOfRgFileSys(), D_00A56DE8, D_00A56DF8);
        if (pCont->fileData != 0) {
            pCont->displayModel = CreateXrgDispModelImpl(pCont->fileData->data, D_00A56E08);
        }
    } else {
        pCont->displayModel = 0;
        pCont->hasActions = 0;
        do {
            pCont->fileData = 0;
        } while (0);
    }
    return cursor;
}

extern void xglLightIntensityAmbient(StudioLight *light,
                                     const Vector4 *intensity);
extern void xglLightIntensityParallel(StudioLight *light, unsigned int index,
                                     const Vector4 *intensity);
extern void xglLightAngle(StudioLight *light, unsigned int index,
                         const Vector4 *direction);

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

    intensity.vectors[0] = D_00A56E10.vectors[0];
    intensity.vectors[1] = D_00A56E10.vectors[1];
    intensity.vectors[2] = D_00A56E10.vectors[2];
    intensity.vectors[3] = D_00A56E10.vectors[3];

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

extern void XrgLinearIntpVector(RgVector destination, RgVector first,
                                 RgVector second, float weight);

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

typedef struct RgSelectActorObject {
    unsigned char unmodeled_00[0x10];
    RgVector position;
    unsigned char unmodeled_20[0x30];
    float unmodeled_50;
    float unmodeled_54;
    int unmodeled_58;
    unsigned char unmodeled_5c[0x9a0 - 0x5c];
    unsigned int motionFlags;        /* +0x9a0 */
    unsigned char unmodeled_9a4[0x9c0 - 0x9a4];
    float motionTimer;               /* +0x9c0 */
} RgSelectActorObject;

#include "main/act_2.h"

static void _ActorPosUpdate(RgSelectRobot *pCont)
{
    RgSelectActorObject *actor;
    RgVector position;
    float yawDelta;
    float yaw;
    float wrappedYaw;

    if (pCont->actorFlags == 0) {
        return;
    }
    if (pCont->parent != 0) {
        return;
    }
    _ActorPos(pCont, position);
    actor = (RgSelectActorObject *)pCont->actorFlags;
    actor->position[0] = position[0];
    actor->position[1] = position[1];
    actor->position[2] = position[2];
    actor->unmodeled_50 = 0.0f;
    actor->unmodeled_54 = pCont->displayYaw;
    actor->unmodeled_58 = 0;
    ACT_updateMotion(actor);
    yawDelta = pCont->displayPosition - pCont->displayYaw;
    while (yawDelta > 3.1415927f) {
        yawDelta -= 6.2831855f;
    }
    while (yawDelta < -3.1415927f) {
        yawDelta += 6.2831855f;
    }
    yaw = pCont->displayYaw + yawDelta * 0.2f;
    pCont->displayYaw = yaw;
    wrappedYaw = yaw;
    while (wrappedYaw > 3.1415927f) {
        wrappedYaw -= 6.2831855f;
    }
    while (wrappedYaw < -3.1415927f) {
        wrappedYaw += 6.2831855f;
    }
    pCont->displayYaw = wrappedYaw;
}

#include "main/act_2.h"

extern const char D_00A56E50[];
extern const char D_00A56E68[];
extern const char D_00A56E80[];
extern const char D_00A56E98[];
extern int RgEquipMountPos(void *table, int slot, int code, void *extra);
extern int RgEquipPosToJntID(int mount);
extern int XrgActorJntIDtoAccID(int jointId);
extern void *InstanceOfRgBattleCommonData(void);
extern void *RgBattleCommonDataGetWeaponMot(void *common);
/* The published _DumpHead takes no parameter; the original call passes
   one node pointer, which its empty body ignores. */
static void _DumpHead();
static void _ActorPosUpdate(RgSelectRobot *pCont);

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
        assert_prog(D_00A56DD8, D_00A56DA0, 324);
    }
    if (pCont->robotID != 2) {
        assert_prog(D_00A56E50, D_00A56DA0, 325);
    }
    if (pCont->nameCount < 3) {
        assert_prog(D_00A56E68, D_00A56DA0, 326);
    }
    if (pCont->actorFlags != 0) {
        assert_prog(D_00A56E80, D_00A56DA0, 327);
    }
    parent = pCont->parent;
    if (parent != 0) {
        if (parent->robotID != 2) {
            return;
        }
        if (parent->actorFlags == 0) {
            return;
        }
    }
    matrixResource = pCont->bufferSlots[0];
    modelResource = pCont->bufferSlots[1];
    moveResource = pCont->bufferSlots[2];
    extraResource = 0;
    if (pCont->nameCount == 4) {
        extraResource = pCont->bufferSlots[3];
    }
    _DumpHead(modelResource);
    _DumpHead(matrixResource);
    _DumpHead(moveResource);
    actor = ACT_create(-1, 1);
    pCont->actorFlags = (unsigned int *)actor;
    if (actor == 0) {
        assert_prog(D_00A56E98, D_00A56DA0, 346);
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
    actor->flags |= 0x8800;
    actor->drawEnv.flags |= 3;
    actor->drawEnv.alpha = 68;
    actor->drawEnv.transparency = 0.0f;
    ACT_initMotion(actor);
    if ((exMotion = extraResource) != 0) {
        ACT_initExMotion(actor, 0, exMotion);
        ACT_setMotion2(actor, 0, 8);
    }
    actor->shadow_kind = 7;
    actor->shadowSize = 48;
    if (pCont->parent != 0) {
        RgSelectRobot *up = pCont->parent;
        unsigned char *table = pCont->equipTable;
        int slot = pCont->equipSlot;
        int mount = RgEquipMountPos(table + 0x70, slot, up->bufferSlots[4], table + 0x330);
        int jnt = RgEquipPosToJntID(mount);
        int acc = XrgActorJntIDtoAccID(jnt);
        ACT_setArms(actor, (Actor *)up->actorFlags, acc, 0);
        code = (unsigned int)up->bufferSlots[4];
        if (code < 6) {
            value = *(int *)(table + 320 + 4 * (3 * code + slot));
            if (value != -1) {
                if (value >= 2048) {
                    if ((weapon = RgBattleCommonDataGetWeaponMot(InstanceOfRgBattleCommonData())) != 0) {
                        ACT_initExMotion(actor, 0, weapon);
                        ACT_setMotion2(actor, value - 2048, 8);
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

/* jal RgError(format, file, line, ...), the variadic error report. */
extern void RgError(const char *format, const char *file, int line, ...);
extern int strcmp(const char *s1, const char *s2);
extern unsigned int XrgActorEssenceGetFileName(void *essence, char (*nameBuf)[64]);
extern const char D_00A56EA8[];
extern const char D_00A56EB8[];
extern const char D_00A56ED0[];

static void _ReqAct(RgSelectRobot *pCont, void *essence, int *accessoryIDs,
                    int *extra)
{
    unsigned int robotKind;
    char nameBuf[4][64];
    int count;
    unsigned int i;
    unsigned int code;
    int failed;

    code = -1;
    failed = 0;
    if (pCont == 0) {
        assert_prog(D_00A56DD8, D_00A56DA0, 447);
    }
    if (extra != 0) {
        for (i = 0; i < 3; i++) {
            pCont->actionActive[i] = extra[i];
        }
    }
    if (essence == 0 || *(int *)essence == -1) {
        count = 0;
        failed = 1;
        for (i = 0; i < 4; i++) {
            pCont->names[i][0] = 0;
        }
    } else {
        code = *(int *)essence;
        count = XrgActorEssenceGetFileName(essence, nameBuf);
        for (i = 0; i < count; i++) {
            if (strcmp(pCont->names[i], nameBuf[i]) != 0) {
                failed = 1;
                break;
            }
        }
    }

    if (failed == 0) {
        if (pCont->hasActions == 0) {
            return;
        }
        if (accessoryIDs == 0) {
            assert_prog(D_00A56EA8, D_00A56DA0, 479);
        }
        for (i = 0; i < 3; i++) {
            _ReqAct((RgSelectRobot *)pCont->actions[i],
                    (void *)((char *)accessoryIDs[i] + i * 136 + 400), 0, 0);
            ((RgSelectRobot *)pCont->actions[i])->equipTable =
                (unsigned char *)accessoryIDs[i];
        }
        return;
    }

    robotKind = (unsigned int)pCont->robotID;
    pCont->bufferSlots[4] = (void *)code;
    switch (robotKind) {
    case 2:
        if (pCont->actorFlags != 0) {
            unsigned int *flags = pCont->actorFlags;
            RgSelectRobot *parent;
            unsigned int bits;
            int clearMask;

            pCont->disposeCounter = 3;
            clearMask = ~0x20;
            parent = pCont->parent;
            bits = *flags;
            pCont->robotID = 3;
            *flags = (bits | 0x8) & clearMask;
            if (parent != 0) {
                ACT_resetParent((Actor *)flags, (Actor *)parent->actorFlags);
            }
        } else {
            pCont->robotID = 0;
        }
        /* fall through */
    case 0:
    case 1:
    case 3:
        pCont->nameCount = count;
        for (i = 0; i < count; i++) {
            strcpy(pCont->names[i], nameBuf[i]);
        }
        if (pCont->robotID == 1) {
            pCont->robotID = 0;
        }
        _KillLoadIfMe(pCont);
        break;
    default:
        RgError(D_00A56EB8, D_00A56DA0, 521);
        break;
    }

    if (pCont->hasActions != 0) {
        for (i = 0; i < 3; i++) {
            RgSelectRobot *child = (RgSelectRobot *)pCont->actions[i];
            _ReqAct(child, 0, 0, 0);
            robotKind = (unsigned int)child->robotID;
            if ((unsigned int)(robotKind - 1) < 2) {
                RgError(D_00A56ED0, D_00A56DA0, 531, robotKind);
            }
        }
    }
}

extern void DisposeRgDispModel(RgDispModel *pDispModel);
extern void DisposeRgFileSysData_sub(RgFileSysData *pFile,
                                     const char *sourceFile, int line);

static void _DestructAct(RgSelectRobot *pCont)
{
    unsigned int i;

    if (pCont == 0) {
        assert_prog(D_00A56DD8, D_00A56DA0, 540);
    }

    if (pCont->actorFlags != 0) {
        *pCont->actorFlags = (*pCont->actorFlags | 0x8) & ~0x20;
    }

    if (pCont->hasActions != 0) {
        for (i = 0; i < 3; i++) {
            if (pCont->actions[i] == 0) {
                continue;
            }
            if (pCont->actions[i]->actorFlags != 0) {
                *pCont->actions[i]->actorFlags =
                    (*pCont->actions[i]->actorFlags | 0x8) & ~0x20;
            }
            RgHeapFree(InstanceOfRgHeap(), pCont->actions[i], D_00A56DA0, 553);
        }
    }

    if (pCont->displayModel != 0) {
        DisposeRgDispModel(pCont->displayModel);
    }
    if (pCont->fileData != 0) {
        DisposeRgFileSysData_sub(pCont->fileData, D_00A56DA0, 561);
    }
}

/* * Cancels the current asynchronous load, then installs the next request: * names[i] is copied into slot i and buffers[i] becomes its buffer. A * request held by another owner refuses the call and returns 0. */ static int _ReqLoad(const char *const *names, void *const *buffers, unsigned int count, void *owner);
static void _ActivateAct(RgSelectRobot *pCont);
extern void ACT_dispose(void *actor);
extern const char D_00A56EE0[];

static void _JobAct(RgSelectRobot *pCont, float deltaTime)
{
    const char *names[4];
    void *buffers[4];
    unsigned int i;
    unsigned int child;
    float timer;
    u32 id;

    if (pCont == 0) {
        assert_prog(D_00A56DD8, D_00A56DA0, 568);
    }
    id = pCont->robotID;
    switch (id) {
        case 0:
            if (pCont->nameCount != 0) {
                for (i = 0; i < pCont->nameCount; i++) {
                    if (pCont->bufferSlots[i] == 0) {
                        assert_prog(D_00A56EE0, D_00A56DA0, 576);
                    }
                    names[i] = (char *)pCont + 12 + i * 64;
                    buffers[i] = pCont->bufferSlots[i];
                }
                if (pCont->parent == 0 || pCont->parent->robotID == 2) {
                    if (_ReqLoad(names, buffers, pCont->nameCount, pCont) != 0) {
                        pCont->robotID = 1;
                    }
                }
            }
            break;
        case 1:
            if (_IsEndOfLoad()) {
                pCont->robotID = 2;
            }
            break;
        case 2: {
            RgSelectActorObject *actor = (RgSelectActorObject *)pCont->actorFlags;
            if (actor == 0) {
                if (pCont->nameCount != 0) {
                    _ActivateAct(pCont);
                }
            } else {
                timer = actor->motionTimer + 0.2f;
                if (1.0f < timer) {
                    actor->motionFlags &= ~1u;
                    timer = 1.0f;
                }
                actor->motionTimer = timer;
                _ActorPosUpdate(pCont);
            }
            break;
        }
        case 3: {
            RgSelectActorObject *actor = (RgSelectActorObject *)pCont->actorFlags;
            if (actor != 0) {
                if (pCont->disposeCounter-- <= 0) {
                    ACT_dispose(actor);
                    pCont->actorFlags = 0;
                    pCont->robotID = 0;
                }
            }
            break;
        }
    default:
        break;
    }
    if (pCont->hasActions != 0) {
        for (child = 0; child < 3; child++) {
            _JobAct((RgSelectRobot *)pCont->actions[child], deltaTime);
        }
    }
}

extern void XrgUnitMatrix(RgMatrix destination);
extern void XrgRotMatrixY(RgMatrix destination, RgMatrix source, float angle);
extern void RgDispModelSetLocal(RgDispModel *pDispModel, const RgMatrix source);
extern void RgDispModelDisplay(RgDispModel *pDispModel);

static void _DrawAct(RgSelectRobot *pCont)
{
    unsigned int count;
    unsigned int i;
    unsigned int index;
    RgVector position;
    RgMatrix matrix;
    int ids[3];

    if (pCont == 0) {
        assert_prog(D_00A56DD8, D_00A56DA0, 653);
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
    if (pCont->hasActions != 0) {
        count = 0;
        for (i = 0; i < 3; i++) {
            if (pCont->actionActive[i] != 0) {
                ids[count] = i;
                count++;
            } else {
                unsigned int *actorFlags;
                RgSelectAction *action;

                action = pCont->actions[i];
                actorFlags = action->actorFlags;
                if (action->weaponID == 2) {
                    if (actorFlags != 0) {
                        *actorFlags &= ~0x8;
                    }
                }
            }
        }
        pCont->drawCount++;
        if (count != 0) {
            index = (pCont->drawCount / 10) % (count + 1);
            for (i = 0; i < count; i++) {
                unsigned int *actorFlags;
                RgSelectAction *action = pCont->actions[ids[i]];

                actorFlags = action->actorFlags;
                if (action->weaponID == 2) {
                    if (actorFlags != 0) {
                        if (index == i) {
                            *actorFlags &= ~0x8;
                        } else {
                            *actorFlags |= 0x8;
                        }
                    }
                }
            }
        }
    }
}

extern RgHeap *InstanceOfRgHeapData(void);
static void *_InitAct(RgSelectRobot *pCont, unsigned char *cursor, int regionBytes, int count);

void _InitSelRob(RgSelectRobot *pCont)
{
    void *heapBuffer;

    if (pCont == 0) {
        assert_prog(D_00A56EF8, D_00A56DA0, 720);
    }
    _InitLoad();
    heapBuffer = RgHeapAlloc(InstanceOfRgHeapData(), 0xC00000U, D_00A56DA0, 726);
    pCont->heapBuffer = heapBuffer;
    _InitAct(pCont, heapBuffer, 0xC0000, 1);
}

extern void ACT_init(void);
static void _DestructAct(RgSelectRobot *pCont);

void _DestructSelRob(RgSelectRobot *pCont)
{
    if (pCont == 0) {
        assert_prog(D_00A56EF8, D_00A56DA0, 734);
    }
    _KillLoad();
    _DestructAct(pCont);
    RgHeapFree(InstanceOfRgHeapData(), pCont->heapBuffer, D_00A56DA0, 737);
    ACT_init();
}

RgSelectRobot *CreateRgSelectRobot(void)
{
    RgSelectRobot *pCont;

    pCont = RgHeapAlloc(InstanceOfRgHeap(), RG_SELECT_ROBOT_SIZE, D_00A56DA0,
                        745);
    _InitSelRob(pCont);
    return pCont;
}

void DisposeRgSelectRobot(RgSelectRobot *pCont)
{
    if (pCont == 0) {
        assert_prog(D_00A56EF8, D_00A56DA0, 753);
    }
    _DestructSelRob(pCont);
    RgHeapFree(InstanceOfRgHeap(), pCont, D_00A56DA0, 755);
    xglCdReadCancel();
}

void RgSelectRobotScreenPos(RgSelectRobot *pCont, float screenPos)
{
    if (pCont == 0) {
        assert_prog(D_00A56EF8, D_00A56DA0, 766);
    }
    pCont->screenPos = screenPos;
}

void RgSelectRobotSetMode(RgSelectRobot *pCont, int mode)
{
    if (pCont == 0) {
        assert_prog(D_00A56EF8, D_00A56DA0, 774);
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

extern void InitXrgActorEssence(void *essence, int actorID);
static void _ReqAct(RgSelectRobot *pCont, void *essence, int *accessoryIDs, int *extra);

void RgSelectRobotSet(RgSelectRobot *pCont, int *selection)
{
    /* Sized to match InitXrgActorEssence's record (xrg_actor.c,
       ov12/tu080); RgSelectRobotSet never reads or writes its fields
       directly. */
    unsigned char essence[0x88];

    if (pCont == 0) {
        assert_prog(D_00A56EF8, D_00A56DA0, 801);
    }
    InitXrgActorEssence(essence, selection[0]);
    _ReqAct(pCont, essence, selection + 1, selection + 4);
}

void RgSelectRobotPassTime(RgSelectRobot *pCont, float deltaTime)
{
    if (pCont == 0) {
        assert_prog(D_00A56EF8, D_00A56DA0, 820);
    }
    _JobAct(pCont, deltaTime);
}

extern void xglFontDebugPrintf(int x, int y, const char *format, ...);
extern void ACT_draw(void);
static void _DrawAct(RgSelectRobot *pCont);
/* ov12:0x00a56f08 holds "ROB %d", ov12:0x00a56f10 holds "WEP(%d) %d". */
extern const char D_00A56F08[];
extern const char D_00A56F10[];

void RgSelectRobotDisp(RgSelectRobot *pCont)
{
    unsigned int i;
    int weapon;

    if (pCont == 0) {
        assert_prog(D_00A56EF8, D_00A56DA0, 0x33D);
    }
    xglFontDebugPrintf(0, 8, D_00A56F08, pCont->robotID);
    /* y and the action pointer are left to gcc's induction variables. */
    for (i = 0; i < 3; i++) {
        weapon = pCont->actions[i]->weaponID;
        xglFontDebugPrintf(0, 16 + i * 8, D_00A56F10, i, weapon);
    }
    ACT_draw();
    _DrawAct(pCont);
    _JobLoad();
}

static void _DumpHead(void)
{
}
