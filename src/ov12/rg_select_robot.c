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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _ReqLoad);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _InitAct);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _ActorPosUpdate);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _ActivateAct);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _ReqAct);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _JobAct);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", _DrawAct);

extern RgHeap *InstanceOfRgHeapData(void);
static void _InitAct(RgSelectRobot *pCont, void *heap, int regionBytes, int count);

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
static void _ReqAct(RgSelectRobot *pCont, void *essence, int *accessoryIDs,
                     int *extra);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_select_robot", RgSelectRobotDisp);

static void _DumpHead(void)
{
}
