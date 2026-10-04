/*
 * OV12 original TU 80: 0x00a454f8..0x00a48548 (63 functions)
 */
#include "common.h"
#include "shared.h"
#include "xrg_actor.h"
#include "ov12/rg_draw.h"

const char D_00A587B0[48] = "_ActJntToAccID_sub joint=%d error(%s:%d)";
const char D_00A587E0[24] = "../xrg_actor.euc.c";
const char D_00A587F8[16] = "pXenoAct != NIL";
const char D_00A58978[40] = "joint id error (jnt:%d elm:%d)\nat %s %d";
const char D_00A589A0[24] = "pJnt->numElement > 1";
/* The two diagnostics share a 48-byte pool, with the second starting at +16. */
const struct {
    char actorNotNil[16];
    char parentMatrixList[32];
} D_00A589C8 = {
    "pActor != NIL",
    "parent mat list nul (char %d)"
};
const char D_00A58A30[32] = "pActor != NIL && pChild != NIL";
const char D_00A58A50[72] = "RG_ACTOR_JNT_LEFT_HAND <= (eJntID) && (eJntID) < RG_ACTOR_JNT_MAX";
const char D_00A58AC8[24] = "pSrcActor != NIL";
const char D_00A58AE0[32] = "pActor->m_pXenoAct != NIL";
const char D_00A58B00[16] = "pEss != NIL";
const char D_00A58B40[16] = "pDup != NIL";
const char D_00A58B50[24] = "pNewParent != NIL";
const char D_00A58BC0[32] = "cannot delete %p from %p\n";

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

/* Assertion and source-file strings retain their scaffold names. */

extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size, const char *source_file,
                         int line);

/* Defined later in this TU (still INCLUDE_ASM); called by the functions below. */
extern void _LoadXenoActor(XrgActor *pActor, int actorID);
extern void _DupXenoActor(XrgActor *pDup, XrgActor *pActor);
extern void XrgActorAttachChild(XrgActor *pParent, XrgActor *pChild, int attachID);
extern void _SetMotUseXenoActor(XrgActor *pActor, int motion);
extern int _IsEndOfMotUseXenoActor(XrgActor *pActor);

/* ACT_* animation module (main), config/symbols/main.txt. */
extern void ACT_initMotion(XenoAct *pXenoAct);
extern void ACT_initExMotion(XenoAct *pXenoAct, int useExtended, int weaponMotion);

extern void *InstanceOfRgBattleCommonData(void);
extern int RgBattleCommonDataGetWeaponMot(void *battleCommonData);

/* The 21-entry table _ActJntToAccID_sub indexes; s_aJntIDToAccID_0
 * (config/symbols/ov12.txt) is 0x54 bytes, i.e. 21 ints. */
static int s_aJntIDToAccID_0[21] = {
    0x18, 0x20, 0x19, 0x21, 0x1A, 0x22, 0x1B, 0x23,
    0, 1, 2, 3, 4, 5, 6, 7, 0x11, 0x12, 0x0F, 0x10, 0x17
};
extern void RgError(const char *message, const char *source_file, int line, ...);

int _ActJntToAccID_sub(int jntID, const char *sourceFile, int line)
{
    if ((unsigned int) jntID >= 21) {
        RgError(D_00A587B0, D_00A587E0, 62, jntID, sourceFile, line);
    }
    return s_aJntIDToAccID_0[jntID];
}

int XrgActorJntIDtoAccID(int jntID)
{
    return _ActJntToAccID_sub(jntID, D_00A587E0, 71);
}

/* Returns the joint's accessory entry (accessories[jntID]) or the default. */
extern void *ACT_jointGetAccessories(XenoAct *pXenoAct, int jntID);

void *_AccIDToJoint(XenoAct *pXenoAct, int jntID)
{
    if (pXenoAct == 0) {
        assert_prog(D_00A587F8, D_00A587E0, 78);
    }
    return ACT_jointGetAccessories(pXenoAct, jntID);
}

extern int _ActJntToAccID_sub(int jntID, const char *sourceFile, int line);

void *_ActJntToJoint(XenoAct *pXenoAct, int jntID)
{
    if (pXenoAct == 0) {
        assert_prog(D_00A587F8, D_00A587E0, 86);
    }
    return _AccIDToJoint(pXenoAct, _ActJntToAccID_sub(jntID, D_00A587E0, 87));
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_actor", _GetXenoDatEntry);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_actor", _SkeManiGet);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_actor", _SkeManiSet);

/*
 * The joint/skeleton data blob XenoAct.jointData points to: only the
 * element count _InitSkeMani reads back is named here.
 */
typedef struct SkeJointData {
    unsigned char unmodeled_00[6];
    unsigned short numElement; /* 0x06 */
} SkeJointData;

/*
 * The skeleton-manipulator record _InitSkeMani fills: a small get/set
 * dispatch pair plus the joint data and element index it is bound to.
 */
typedef struct SkeMani {
    void (*get)(struct SkeMani *mani, RgMatrix matrix); /* 0x00: joint matrix out */
    void (*set)(struct SkeMani *mani, RgMatrix matrix); /* 0x04: joint matrix in */
    XenoAct *pXenoAct;  /* 0x08: NULL when this manipulator has no joint data */
    int elementIndex;    /* 0x0C: only set together with pXenoAct */
} SkeMani;

extern void RgWarn(const char *format, const char *source_file, int line, ...);
extern void _SkeManiGet(SkeMani *mani, RgMatrix matrix);
extern void _SkeManiSet(SkeMani *mani, RgMatrix matrix);

int _InitSkeMani(SkeMani *pMani, XenoAct *pXenoAct, int elmID,
                 const char *sourceFile, int line)
{
    int result;
    SkeJointData *pJointData;
    unsigned short numElement;

    pMani->get = _SkeManiGet;
    pMani->set = _SkeManiSet;
    if (pXenoAct != 0) {
        pJointData = pXenoAct->jointData;
        result = 0;
        if (elmID > 0) {
            numElement = pJointData->numElement;
            if (elmID >= (int) numElement) {
                RgWarn(D_00A58978, D_00A587E0, 256, elmID, numElement, sourceFile, line);
                if (pJointData->numElement < 2) {
                    assert_prog(D_00A589A0, D_00A587E0, 257);
                }
                elmID = 1;
            }
            pMani->pXenoAct = pXenoAct;
            pMani->elementIndex = elmID;
            goto done;
        }
    } else {
        pMani->pXenoAct = 0;
      done:
        result = 1;
    }
    return result;
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_actor", _CreateSkeMani_sub);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_actor", _CreateAccMani_sub);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_actor", _DisposeUseXenoActor);

/*
 * _copy_matrix copies src into dst and drops its translation column
 * (elements 3, 7 and 11 of the 4x4 RgMatrix), giving dst a rotation-only
 * copy of src.
 */
static void _copy_matrix(RgMatrix dst, const RgMatrix src)
{
    XrgCopyMatrix(dst, src);
    dst[3] = 0.0f;
    dst[7] = 0.0f;
    dst[11] = 0.0f;
}

/*
 * _store_matrix copies src into dst but keeps dst's own translation column
 * and bottom-right element (elements 3, 7, 11 and 15 of the 4x4 RgMatrix),
 * so dst takes src's rotation while keeping its position.
 */
static void _store_matrix(RgMatrix dst, const RgMatrix src)
{
    float m03;
    float m13;
    float m23;
    float m33;

    m03 = dst[3];
    m13 = dst[7];
    m23 = dst[11];
    m33 = dst[15];
    XrgCopyMatrix(dst, src);
    dst[3] = m03;
    dst[7] = m13;
    dst[11] = m23;
    dst[15] = m33;
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_actor", _PassTime_00A45D30);

#include "ov12/rg_debug_flags.h"

extern RgDebugFlags *InstanceOfRgDebugFlags(void);
extern void nmlModelSetFogCol(float *);
extern void nmlModelSetFogDist(float, float, float, float);
extern void nmlModelSetRenderLevel(int);
RgFog *s_pUseFog = 0;

void _ActorDrawFunction(XenoAct *pXenoAct)
{
    RgFog *pFog;
    float color[3];

    if (InstanceOfRgDebugFlags()->flags[2] != 0) {
        pFog = s_pUseFog;
        if (pFog != 0) {
            color[0] = pFog->color[0];
            color[1] = pFog->color[1];
            color[2] = pFog->color[2];
            nmlModelSetFogCol(color);
            nmlModelSetFogDist(pFog->dist[0], pFog->dist[1], pFog->dist[2], pFog->dist[3]);
            if (pXenoAct->renderLevelFlag != 0) {
                nmlModelSetRenderLevel(0x10);
            }
        }
    }
}

/* The initial current-fog block is an explicitly initialized null pointer. */
extern int ACT_modelDraw(XenoAct *pXenoAct);

void _DrawActor(XrgActor *pActor, void *pStudio)
{
    RgDrawStudio *pDrawStudio;

    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 527);
    }
    pDrawStudio = pStudio;
    s_pUseFog = pDrawStudio->m_pFog;
    ACT_modelDraw(pActor->pXenoAct);
}

extern void ACT_setMotion2(XenoAct *, int, int, int);

void _SetMotionToXenoActorWithRgMotID(XrgActor *pActor, int rgMotID, int playbackParam)
{
    if (rgMotID >= 0x800 && pActor->weaponMotionActive != 0) {
        ACT_setMotion2(pActor->pXenoAct, (rgMotID - 0x800) | 0x100, playbackParam, playbackParam);
        return;
    }
    ACT_setMotion2(pActor->pXenoAct, rgMotID, playbackParam, playbackParam);
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_actor", _SetMotUseXenoActor);

extern void ACT_allocMatrix(XenoAct *pXenoAct, int flags);
extern void ACT_setArms(XenoAct *pChildXenoAct, XenoAct *pParentXenoAct,
                        int accID, int flags);
extern void ACT_setModelWrapper(XenoAct *pXenoAct, int flags);

/*
 * The joint-ID bounds RG_ACTOR_JNT_LEFT_HAND/RG_ACTOR_JNT_MAX, also used by
 * XrgActorAttachChild's own assert message (0x00a58a50).
 */
#define RG_ACTOR_JNT_LEFT_HAND 0
#define RG_ACTOR_JNT_MAX 21


void _AttachChildUseXenoActor(XrgActor *pParent, XrgActor *pChild, int eJntID)
{
    if (pParent == 0 || pChild == 0) {
        assert_prog(D_00A58A30, D_00A587E0, 606);
    }
    if (!(RG_ACTOR_JNT_LEFT_HAND <= eJntID && eJntID < RG_ACTOR_JNT_MAX)) {
        assert_prog(D_00A58A50, D_00A587E0, 607);
    }
    if (pParent->pXenoAct == 0) {
        return;
    }
    if (pChild->pXenoAct == 0) {
        return;
    }
    ACT_initMotion(pChild->pXenoAct);
    ACT_allocMatrix(pChild->pXenoAct, -1);
    ACT_setModelWrapper(pChild->pXenoAct, 0);
    ACT_setArms(pChild->pXenoAct, pParent->pXenoAct,
               _ActJntToAccID_sub(eJntID, D_00A587E0, 615), 0);
}

extern void ACT_resetParent(XenoAct *pChildXenoAct, XenoAct *pParentXenoAct);
extern void ACT_setHumanHand(XenoAct *pXenoAct, int hand);

void _DetachChildUseXenoActor(XrgActor *pParent, XrgActor *pChild)
{
    if (pParent == 0 || pChild == 0) {
        assert_prog(D_00A58A30, D_00A587E0, 623);
    }
    if (pParent->pXenoAct == 0) {
        return;
    }
    if (pChild->pXenoAct == 0) {
        return;
    }
    ACT_resetParent(pChild->pXenoAct, pParent->pXenoAct);
    ACT_setHumanHand(pParent->pXenoAct, 0);
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_actor", _IsEndOfMotUseXenoActor);

void _DumpHead(void) {

}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_actor", _LoadXenoActorMdl);

extern RgFileSys *InstanceOfRgFileSys(void);
extern char *RgFileSysDataGetName(RgFileSysData *pFile);
extern RgFileSysData *RgFileSysDup(RgFileSys *pSys, const char *pszName,
                                   const char *pszRoot);
extern RgFileSysData *RgFileSysRead(RgFileSys *pSys, const char *pszName,
                                    const char *pszRoot);

void _DupLoadXenoActor(XrgActor *pActor, XrgActor *pSrcActor)
{
    RgFileSysData *pDupFile;
    RgFileSysData *pAuxFile;
    RgFileSysData *pJointFile;
    XenoAct *pXenoAct;

    pDupFile = 0;
    pAuxFile = 0;
    pJointFile = 0;
    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 718);
    }
    if (pSrcActor == 0) {
        assert_prog(D_00A58AC8, D_00A587E0, 719);
    }
    pXenoAct = pActor->pXenoAct;
    if (pXenoAct == 0) {
        assert_prog(D_00A587F8, D_00A587E0, 723);
    }
    if (pSrcActor->resourceFiles[1] != 0) {
        pAuxFile = RgFileSysRead(InstanceOfRgFileSys(),
                                 RgFileSysDataGetName(pSrcActor->resourceFiles[1]), 0);
        pActor->resourceFiles[1] = pAuxFile;
    }
    if (pSrcActor->resourceFiles[3] != 0) {
        pJointFile = RgFileSysRead(InstanceOfRgFileSys(),
                                   RgFileSysDataGetName(pSrcActor->resourceFiles[3]), 0);
        pActor->resourceFiles[3] = pJointFile;
    }
    if (pSrcActor->resourceFiles[2] != 0) {
        pDupFile = RgFileSysDup(InstanceOfRgFileSys(),
                                RgFileSysDataGetName(pSrcActor->resourceFiles[2]), 0);
        pActor->resourceFiles[2] = pDupFile;
    }
    if (pSrcActor->resourceFiles[0] != 0) {
        pActor->resourceFiles[0] = RgFileSysRead(InstanceOfRgFileSys(),
                                   RgFileSysDataGetName(pSrcActor->resourceFiles[0]), 0);
    }
    if (pDupFile != 0) {
        pXenoAct->dupFileData = pDupFile->data;
    }
    if (pAuxFile != 0) {
        pXenoAct->auxFileData = pAuxFile->data;
    }
    if (pJointFile != 0) {
        pXenoAct->jointData = pJointFile->data;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_actor", _InitXenoActor);

void _SetBattleCommonWepMotion(XrgActor *pActor)
{
    int weaponMotion;
    XenoAct *pXenoAct;

    weaponMotion = RgBattleCommonDataGetWeaponMot(InstanceOfRgBattleCommonData());
    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 776);
    }
    pXenoAct = pActor->pXenoAct;
    if (pXenoAct == 0) {
        assert_prog(D_00A587F8, D_00A587E0, 778);
    }
    if (weaponMotion != 0) {
        pActor->weaponMotionActive = 1;
        ACT_initExMotion(pXenoAct, 1, weaponMotion);
    }
}

extern void xglLightSetDefault(StudioLight *light);

void _InitAfterLoadXenoActor(XrgActor *pActor)
{
    XenoAct *pXenoAct;

    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 790);
    }
    pXenoAct = pActor->pXenoAct;
    if (pXenoAct == 0) {
        assert_prog(D_00A58AE0, D_00A587E0, 791);
        pXenoAct = pActor->pXenoAct;
    }
    pActor->motionComaStep = 0;
    pActor->motionComaStepAccum = 0.0f;
    xglLightSetDefault(&pXenoAct->light);
    pXenoAct->drawFunc = _ActorDrawFunction;
    pXenoAct->flags |= 0x8000;
    ACT_allocMatrix(pXenoAct, -1);
    ACT_setModelWrapper(pXenoAct, 0);
    if (pActor->resourceFiles[0] != 0) {
        ACT_initMotion(pXenoAct);
        ACT_initExMotion(pXenoAct, 0, (int) pActor->resourceFiles[0]->data);
        _SetBattleCommonWepMotion(pActor);
    }
    pXenoAct->lightCost[0] = 7;
    pXenoAct->lightCost[1] = 0x30;
    pXenoAct->renderFlags |= 0x40;
}

void XrgActorSetLightCost(XrgActor *pActor)
{
    XenoAct *pXenoAct;

    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 841);
    }
    pXenoAct = pActor->pXenoAct;
    pXenoAct->lightCost[0] = 9;
    pXenoAct->lightCost[1] = 0x70;
    pXenoAct->renderFlags |= 0x40;
    pXenoAct->lightCostLevel = 0x12;
}

void XrgActorSetDropWeapon(XrgActor *pActor)
{
    XenoAct *pXenoAct;

    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 859);
    }
    pXenoAct = pActor->pXenoAct;
    if (pXenoAct != 0) {
        pXenoAct->flags |= 0x408;
    }
}

void XrgActorSetPlayMotionComaStep(XrgActor *pActor, int step)
{
    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 872);
    }
    pActor->motionComaStep = step;
    pActor->motionComaStepAccum = 0.0f;
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_actor", _LoadXenoActor);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_actor", _DupXenoActor);

XrgActor *CreateXrgActor(int actorID)
{
    XrgActor *pActor;

    pActor = RgHeapAlloc(InstanceOfRgHeap(), XRG_ACTOR_SIZE, D_00A587E0, 957);
    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 958);
    }
    _LoadXenoActor(pActor, actorID);
    return pActor;
}

XrgActor *DuplicateXrgActor(XrgActor *pActor, XrgActor *pNewParent)
{
    XrgActor *pDup;

    pDup = RgHeapAlloc(InstanceOfRgHeap(), XRG_ACTOR_SIZE, D_00A587E0, 968);
    if (pDup == 0) {
        assert_prog(D_00A58B40, D_00A587E0, 969);
    }
    _DupXenoActor(pDup, pActor);
    if (pActor->attachID != -1) {
        if (pNewParent == 0) {
            assert_prog(D_00A58B50, D_00A587E0, 974);
        }
        XrgActorAttachChild(pNewParent, pDup, pActor->attachID);
    }
    return pDup;
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_actor", DisposeXrgActor);

void XrgActorSetLocal(XrgActor *pActor, const RgMatrix matrix)
{
    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 999);
    }
    XrgCopyMatrix(pActor->local, matrix);
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_actor", XrgActorSetLight);

void XrgActorSetMotion(XrgActor *pActor, int motion)
{
    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 1030);
    }
    if (motion != -1) {
        _SetMotUseXenoActor(pActor, motion);
    }
}

void XrgActorSetSmoothPlay(XrgActor *pActor, int smoothPlay)
{
    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 1042);
    }
    pActor->smoothPlay = smoothPlay;
}

void XrgActorSetLoopPlay(XrgActor *pActor, int loopPlay)
{
    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 1050);
    }
    pActor->loopPlay = loopPlay;
}

void XrgActorDeriveMotion(XrgActor *pActor, XrgActor *pSrcActor)
{
    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 1059);
    }
    if (pSrcActor == 0) {
        assert_prog(D_00A58AC8, D_00A587E0, 1060);
    }
    if (pSrcActor->resourceFiles[0] == 0) {
        return;
    }
    ACT_initMotion(pActor->pXenoAct);
    ACT_initExMotion(pActor->pXenoAct, 0, (int) pSrcActor->resourceFiles[0]->data);
    _SetBattleCommonWepMotion(pActor);
}

void XrgActorSetMotionFrame(XrgActor *pActor, float frame)
{
    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 1072);
    }
    if ((pActor->pXenoAct != 0) && (pActor->motionID != -1)) {
        if (pActor->parent == 0) {
            ACT_initMotion(pActor->pXenoAct);
        }
        pActor->pXenoAct->motionFrame = frame;
    }
}

void XrgActorSetDraw(XrgActor *pActor, int draw)
{
    XenoAct *pXenoAct;

    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 1084);
    }
    pXenoAct = pActor->pXenoAct;
    if (pXenoAct != 0) {
        if (draw == 0) {
            pXenoAct->flags |= 8;
            pXenoAct->renderFlags |= 0x100;
        } else {
            pXenoAct->flags &= ~8;
        }
    }
}

void XrgActorSetTransparent(XrgActor *pActor, float transparency)
{
    XenoAct *pXenoAct;

    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 1100);
    }
    pXenoAct = pActor->pXenoAct;
    if (pXenoAct != 0) {
        pXenoAct->transparency = transparency;
        pXenoAct->flags |= 0x800;
        pXenoAct->blendMode = 0x44;
        pXenoAct->renderFlags |= 0x3;
    }
}

int XrgActorIsEndOfMotion(XrgActor *pActor)
{
    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 1118);
    }
    return _IsEndOfMotUseXenoActor(pActor);
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_actor", XrgActorGetMotionLength);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_actor", XrgActorGetPlayingMotTime);

void XrgActorGetLocal(XrgActor *pActor, RgMatrix matrix)
{
    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 1149);
    }
    XrgCopyMatrix(matrix, pActor->local);
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_actor", XrgActorGetJointLocal);

int XrgActorGetAttachedJoint(XrgActor *pActor)
{
    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 1208);
    }
    if (pActor->parent == 0) {
        return -1;
    }
    return pActor->attachID;
}

extern void RgVectorPush(void *vector, void *element);
extern void _AttachChildUseXenoActor(XrgActor *pParent, XrgActor *pChild,
                                     int eJntID);

void XrgActorAttachChild(XrgActor *pParent, XrgActor *pChild, int eJntID)
{
    if (pParent == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 1222);
    }
    if (!(RG_ACTOR_JNT_LEFT_HAND <= eJntID && eJntID < RG_ACTOR_JNT_MAX)) {
        assert_prog(D_00A58A50, D_00A587E0, 1223);
    }
    if (pChild != 0) {
        RgVectorPush(pParent->children, pChild);
        _AttachChildUseXenoActor(pParent, pChild, eJntID);
        pChild->parent = pParent;
        pChild->attachID = eJntID;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_actor", XrgActorCheckPakaPaka);

extern int RgVectorRemove(void *vector, void *element, const char *source_file,
                          int line);
extern void RgWarn(const char *format, const char *source_file, int line, ...);
extern void _DetachChildUseXenoActor(XrgActor *pParent, XrgActor *pChild);

void XrgActorDettachChild(XrgActor *pParent, XrgActor *pChild)
{
    if (pParent == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 1259);
    }
    if (pChild != 0) {
        if (RgVectorRemove(pParent->children, pChild, D_00A587E0, 1261) == 0) {
            RgWarn(D_00A58BC0, D_00A587E0, 1262, pParent->children, pChild);
        }
        _DetachChildUseXenoActor(pParent, pChild);
        pChild->parent = 0;
        pChild->attachID = -1;
    }
}

extern RgDraw *InstanceOfRgDraw(void);
extern void RgDrawReq(RgDraw *pDraw, XrgActor *pActor,
                      void (*drawFunc)(XrgActor *pActor, void *pStudio),
                      void (*clearFunc)(XrgActor *pActor, void *pStudio),
                      int prio, int drawID);
void _DrawActor(XrgActor *pActor, void *pStudio);

void XrgActorDraw(XrgActor *pActor)
{
    RgDrawReq(InstanceOfRgDraw(), pActor, &_DrawActor, 0, 0, -1);
}

static void _PassTime_00A45D30(XrgActor *pActor, float dt);

void XrgActorPassTime(XrgActor *pActor, float dt)
{
    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 1285);
    }
    _PassTime_00A45D30(pActor, dt);
}


void InitXrgActorEssence(XrgActorEssence *pEss, int actorID)
{
    if (pEss == 0) {
        assert_prog(D_00A58B00, D_00A587E0, 1295);
    }
    pEss->actorID = actorID;
    pEss->fileName[0] = 0;
    pEss->state = 0;
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_actor", XrgActorEssenceGetFileName);

/* Each record begins with its actor/joint key, followed by the original
 * signed joint-remapping payload. The payload uses -1 as its list terminator.
 */
typedef struct XrgRawJntData {
    int id;
    int jntID;
    int jointMap[34];
} XrgRawJntData;

XrgRawJntData s_aRawJntData[12] = {
    {0, 0, {53, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 80, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 53, 53}},
    {0, 1, {67, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 83, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 67, 67}},
    {1, 0, {128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 138, 138}},
    {1, 1, {139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 149, 149}},
    {3, 0, {73, 76, 119, 120, 121, 122, 123, 124, 125, 126, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 126, 126}},
    {3, 1, {87, 90, 127, 128, 129, 130, 131, 132, 133, 134, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 134, 134}},
    {2, 0, {50, 53, 54, 55, 56, 57, 58, 59, 77, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 49, 77}},
    {2, 1, {65, 68, 69, 70, 71, 72, 73, 74, 80, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, 80}},
    {4, 0, {40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 40, 40}},
    {4, 1, {59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59, 59}},
    {5, 0, {118, 119, 120, 121, 122, 123, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 119, 119}},
    {5, 1, {125, 126, 127, 128, 129, 130, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 126, 126}}
};

XrgRawJntData *_FindRawJntData(int id, int jntID)
{
    unsigned int i;
    XrgRawJntData *entry;

    entry = s_aRawJntData;
    for (i = 0; i < 12; i++, entry++) {
        if (entry->id == id && entry->jntID == jntID) {
            return entry;
        }
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_actor", _GetPakaPakaBaseJnt);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_actor", XrgActorCreateEffConstraint);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_actor", XrgActorCreateEffArmCombine);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_actor", XrgActorDisposeEffector);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_actor", XrgActorDebugDisp);
