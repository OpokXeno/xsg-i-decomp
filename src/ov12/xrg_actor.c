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
void _LoadXenoActor(XrgActor *pActor, XrgActorEssence *pEss);
void _DupXenoActor(XrgActor *pDup, XrgActor *pActor);
extern void XrgActorAttachChild(XrgActor *pParent, XrgActor *pChild, int attachID);
void _SetMotUseXenoActor(XrgActor *pActor, int motion);
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

typedef struct XenoDatEntry {
    int charID;                     /* 0x00 */
    const char *m_pszMdlName;       /* 0x04: model file name ACT_create loads */
    const char *m_pszOptFile;       /* 0x08: optional name XrgActorEssenceGetFileName appends as the fourth entry */
    const char *m_pszFileBase;      /* 0x0C: base name XrgActorEssenceGetFileName copies when the essence has none;
                                       its assert text (D_00A58B20) calls it m_pszMdlName */
} XenoDatEntry;

extern XenoDatEntry s_aDatTbl_1[25];
/* ov12:0x00a58958 "invalid actor char ID = %d" */
extern const char D_00A58958[];
extern void RgWarn(const char *format, const char *source_file, int line, ...);

/* Returns the table entry for charID, or warns and returns 0. */
XenoDatEntry *_GetXenoDatEntry(int charID)
{
    unsigned int i;

    for (i = 0; i < 25; i++) {
        if (s_aDatTbl_1[i].charID == charID) {
            return &s_aDatTbl_1[i];
        }
    }
    RgWarn(D_00A58958, D_00A587E0, 168, charID);
    return 0;
}

extern int RgHeapIsInvalidMemory(RgHeap *heap, void *ptr);
extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file, int line);
/* ov12:0x00a58b68 "already disposed XrgActor %p" */
extern const char D_00A58B68[];
void _DisposeUseXenoActor(XrgActor *pActor);
extern void DisposeRgFileSysData_sub(RgFileSysData *pFile, const char *sourceFile, int line);
extern void DisposeRgVector(void *pVector, const char *sourceFile, int line);
extern void DisposeRgMatricesEffector(struct MatrixEffector *pEffector);
extern void ACT_dispose(XenoAct *pXenoAct);
/* XrgUnitMatrix (ov12/rg_geom_poly.h) and XrgMulMatrix (ov12/rg_matrices_effector.h) are not included here. */
extern void XrgUnitMatrix(RgMatrix destination);
extern void XrgMulMatrix(RgMatrix destination, RgMatrix left, RgMatrix right);

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

/* * Copies the manipulator's joint matrix into matrix, translation column * dropped. A clear renderLevelFlag also multiplies it by the element-0 * matrix (translation dropped too). */ void _SkeManiGet(SkeMani *mani, RgMatrix matrix);
/* * Stores the manipulator's matrix (translation column dropped) into the * joint matrix block element. A clear renderLevelFlag first premultiplies * matrix by the inverse of the element-0 base matrix (translation dropped). */ void _SkeManiSet(SkeMani *mani, RgMatrix matrix);

/*
 * Copies the manipulator's joint matrix into matrix, translation column
 * dropped. A clear renderLevelFlag also multiplies it by the element-0
 * matrix (translation dropped too).
 */
void _SkeManiGet(SkeMani *mani, RgMatrix matrix)
{
    XenoAct *pXenoAct;
    RgMatrix *jointMatrices;
    RgMatrix tmp;

    pXenoAct = mani->pXenoAct;
    if (pXenoAct != 0) {
        jointMatrices = pXenoAct->jointMatrices;
        if (pXenoAct->renderLevelFlag != 0) {
            XrgCopyMatrix(matrix, jointMatrices[mani->elementIndex]);
            matrix[3] = 0.0f;
            matrix[7] = 0.0f;
            matrix[11] = 0.0f;
        } else {
            XrgCopyMatrix(matrix, jointMatrices[mani->elementIndex]);
            XrgCopyMatrix(tmp, jointMatrices[0]);
            matrix[3] = 0.0f;
            matrix[7] = 0.0f;
            matrix[11] = 0.0f;
            tmp[3] = 0.0f;
            tmp[7] = 0.0f;
            tmp[11] = 0.0f;
            XrgMulMatrix(matrix, tmp, matrix);
        }
    } else {
        XrgUnitMatrix(matrix);
    }
}

/*
 * Stores the manipulator's matrix (translation column dropped) into the
 * joint matrix block element. A clear renderLevelFlag first premultiplies
 * matrix by the inverse of the element-0 base matrix (translation dropped).
 */
void _SkeManiSet(SkeMani *mani, RgMatrix matrix)
{
    XenoAct *pXenoAct;
    RgMatrix *jointMatrices;
    RgMatrix tmp;
    int elementIndex;
    int row;
    int col;

    pXenoAct = mani->pXenoAct;
    if (pXenoAct != 0) {
        elementIndex = mani->elementIndex;
        jointMatrices = pXenoAct->jointMatrices;
        if (pXenoAct->renderLevelFlag == 0) {
            XrgCopyMatrix(tmp, jointMatrices[0]);
            tmp[3] = 0.0f;
            tmp[7] = 0.0f;
            tmp[11] = 0.0f;
            XrgInvMatrix(tmp, tmp);
            XrgMulMatrix(matrix, tmp, matrix);
        }
        for (col = 0; col < 3; col++) {
            for (row = 0; row < 4; row++) {
                jointMatrices[elementIndex][4 * row + col] = matrix[4 * row + col];
            }
        }
    }
}

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

const char D_00A589B8[16] = "pUnit != NIL";

/*
 * Allocates a SkeMani record and binds it to pXenoAct's element elmID through
 * _InitSkeMani; the record is freed and NULL returned when the binding fails.
 */
SkeMani *_CreateSkeMani_sub(XenoAct *pXenoAct, int elmID, const char *sourceFile,
                            int line)
{
    SkeMani *pMani;

    pMani = RgHeapAlloc(InstanceOfRgHeap(), sizeof(SkeMani), sourceFile, line);
    if (pMani == 0) {
        assert_prog(D_00A589B8, D_00A587E0, 272);
    }
    if (_InitSkeMani(pMani, pXenoAct, elmID, sourceFile, line) == 0) {
        RgHeapFree(InstanceOfRgHeap(), pMani, D_00A587E0, 274);
        return 0;
    }
    return pMani;
}

const char D_00A58B10[16] = "pEntry != NIL";

SkeMani *_CreateAccMani_sub(XenoAct *pXenoAct, int jntID, const char *sourceFile, int line)
{
    SkeMani *pMani;

    pMani = RgHeapAlloc(InstanceOfRgHeap(), sizeof(SkeMani), D_00A587E0, 285);
    if (pMani == 0) {
        assert_prog(D_00A589B8, D_00A587E0, 286);
    }
    if (pXenoAct != 0) {
        if (_InitSkeMani(pMani, pXenoAct, (int) _AccIDToJoint(pXenoAct, jntID), sourceFile, line) == 0) {
            RgHeapFree(InstanceOfRgHeap(), pMani, D_00A587E0, 291);
            return 0;
        }
    } else {
        if (_InitSkeMani(pMani, 0, 0, sourceFile, line) == 0) {
            RgHeapFree(InstanceOfRgHeap(), pMani, D_00A587E0, 296);
            return 0;
        }
    }
    return pMani;
}

void _DisposeUseXenoActor(XrgActor *pActor)
{
    unsigned int i;
    unsigned int j;

    if (pActor->resourceFiles[0] != 0) {
        DisposeRgFileSysData_sub(pActor->resourceFiles[0], D_00A587E0, 321);
    }
    if (pActor->resourceFiles[1] != 0) {
        DisposeRgFileSysData_sub(pActor->resourceFiles[1], D_00A587E0, 323);
    }
    if (pActor->resourceFiles[2] != 0) {
        DisposeRgFileSysData_sub(pActor->resourceFiles[2], D_00A587E0, 325);
    }
    if (pActor->resourceFiles[3] != 0) {
        DisposeRgFileSysData_sub(pActor->resourceFiles[3], D_00A587E0, 327);
    }
    if (pActor->pXenoAct != 0) {
        ACT_dispose(pActor->pXenoAct);
    }
    for (i = 0; i < 2; i++) {
        for (j = 0; j < pActor->effectorCount[i]; j++) {
            DisposeRgMatricesEffector(pActor->effector[i][j]);
        }
        pActor->effectorCount[i] = 0;
    }
    DisposeRgVector(pActor->children, D_00A587E0, 340);
    pActor->children = 0;
}

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

extern void ACT_updateMotion(XenoAct *pXenoAct);
extern double atan2(double y, double x);
extern void RgMatricesEffectorJob(struct MatrixEffector *pEffector);
extern const char D_00A589F8[];
extern const char D_00A58A10[];
extern int D_00A5ACE4;

void _PassTime(XrgActor *pActor, float dt)
{
    RgMatrix locals[64];
    RgMatrix a;
    RgMatrix b;
    RgMatrix c;
    XenoAct *pXenoAct;
    XrgActor *pParent;
    XenoAct *pParentAct;
    void *pJntData;
    RgMatrix *pOwnMatrices;
    float *pRoot;
    RgMatrix *pParentMatrices;
    unsigned int numElement;
    unsigned int i;
    unsigned int j;
    float step;
    const float frameTime = 1.0f / 30.0f;

    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 371);
    }
    pXenoAct = pActor->pXenoAct;
    if (pXenoAct == 0) {
        return;
    }
    pParent = pActor->parent;
    if (pParent == 0) {
        pParentAct = 0;
        pXenoAct->position[0] = pActor->local[12];
        pXenoAct->position[1] = pActor->local[13] - 1.5f;
        pXenoAct->position[2] = pActor->local[14];
        pXenoAct->rotationY = atan2(-pActor->local[8], -pActor->local[10]);
    } else {
        pParentAct = pParent->pXenoAct;
    }
    if (pActor->motionComaStep != 0) {
        step = pActor->motionComaStepAccum + dt;
        pActor->motionComaStepAccum = step;
        if (!(frameTime <= step)) {
            pXenoAct->motion.step = 0.0f;
        } else {
            pActor->motionComaStepAccum = step - frameTime;
            pXenoAct->motion.step = frameTime;
        }
    } else {
        pXenoAct->motion.step = dt;
    }
    ACT_updateMotion(pXenoAct);
    if (pActor->pakaPaka != 0 && pParentAct != 0) {
        D_00A5ACE4 = pActor->pakaBaseJnt;
        pJntData = pXenoAct->jointData;
        pOwnMatrices = pXenoAct->jointMatrices;
        pParentMatrices = pParentAct->jointMatrices;
        if (pJntData != 0 && pParentMatrices != 0 && pOwnMatrices != 0) {
            pRoot = pOwnMatrices[1];
            _ActJntToJoint(pParentAct, pActor->attachID);
            if (pActor->unmodeled_160 == 0) {
                _copy_matrix(b, pParentMatrices[0]);
                _copy_matrix(a, pParentMatrices[D_00A5ACE4]);
                XrgMulMatrix(a, b, a);
                _copy_matrix(b, pRoot);
                XrgInvMatrix(c, a);
                XrgMulMatrix(pActor->pakaBaseMatrix, c, b);
                pActor->unmodeled_160 = 1;
            }
            numElement = *(unsigned short *) ((char *) pJntData + 6);
            if (numElement >= 65) {
                RgWarn(D_00A589F8, D_00A587E0, 456);
            }
            _copy_matrix(a, pRoot);
            XrgInvMatrix(c, a);
            for (i = 1; i < numElement; i++) {
                _copy_matrix(a, pOwnMatrices[i]);
                XrgMulMatrix(locals[i], c, a);
            }
            _copy_matrix(b, pParentMatrices[0]);
            _copy_matrix(a, pParentMatrices[D_00A5ACE4]);
            XrgMulMatrix(b, b, a);
            XrgMulMatrix(b, b, pActor->pakaBaseMatrix);
            for (i = 1; i < numElement; i++) {
                XrgMulMatrix(a, b, locals[i]);
                _store_matrix(pOwnMatrices[i], a);
            }
        }
    }
    for (i = 0; i < 2; i++) {
        for (j = 0; j < pActor->effectorCount[i]; j++) {
            if (pActor->effector[i][j] != 0) {
                RgMatricesEffectorJob(pActor->effector[i][j]);
            } else {
                RgWarn(D_00A58A10, D_00A587E0, 487, pActor->charID, i, j);
            }
        }
    }
}

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

void _SetMotUseXenoActor(XrgActor *pActor, int motion)
{
    XenoAct *pXenoAct;
    int playbackParam;

    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 563);
    }
    pXenoAct = pActor->pXenoAct;
    if (pXenoAct != 0) {
        playbackParam = pActor->smoothPlay != 0;
        if (pActor->loopPlay != 0) {
            playbackParam |= 8;
        }
        pActor->smoothPlay = 0;
        pActor->loopPlay = 0;
        if (pActor->parent == 0) {
            ACT_initMotion(pXenoAct);
        }
        _SetMotionToXenoActorWithRgMotID(pActor, motion, playbackParam);
        pActor->motionID = motion;
        if (pActor->charID == 2 && motion == 63 || pActor->charID == 3) {
            pActor->motionComaStep = 1;
        } else {
            pActor->motionComaStep = 0;
        }
        pActor->motionComaStepAccum = 0.0f;
    }
}

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

int _IsEndOfMotUseXenoActor(XrgActor *pActor)
{
    XenoAct *pXenoAct = pActor->pXenoAct;

    if (pActor->motionID == -1) {
        return 1;
    }
    if (pXenoAct == 0 || (pXenoAct->motion.flags & 0x1000) == 0) {
        return 0;
    }
    return 1;
}

void _DumpHead(RgFileSysData *pFile) {

}

const char D_00A58A98[18] = "pszMdlName != NIL";

const char D_00A58AB0[5] = ".xtx";

const char D_00A58AB8[5] = ".lex";

const char D_00A58AC0[5] = ".jnt";

void _LoadXenoActorMdl(XrgActor *pActor, const char *pszMdlName)
{
    XenoAct *pXenoAct;
    char szFileName[3][128];
    RgFileSysData *pFile;
    RgFileSysData *pJoint;
    RgFileSysData *pDup;

    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 681);
    }
    if (pszMdlName == 0) {
        assert_prog(D_00A58A98, D_00A587E0, 682);
    }
    pXenoAct = pActor->pXenoAct;
    if (pXenoAct == 0) {
        assert_prog(D_00A587F8, D_00A587E0, 685);
    }
    strcat(strcpy(szFileName[0], pszMdlName), D_00A58AB0);
    strcat(strcpy(szFileName[1], pszMdlName), D_00A58AB8);
    strcat(strcpy(szFileName[2], pszMdlName), D_00A58AC0);
    pFile = RgFileSysRead(InstanceOfRgFileSys(), szFileName[0], 0);
    pActor->resourceFiles[1] = pFile;
    pJoint = RgFileSysRead(InstanceOfRgFileSys(), szFileName[2], 0);
    pActor->resourceFiles[3] = pJoint;
    pDup = RgFileSysDup(InstanceOfRgFileSys(), szFileName[1], 0);
    pActor->resourceFiles[2] = pDup;
    if (pDup != 0) {
        pXenoAct->dupFileData = pDup->data;
        _DumpHead(pDup);
    }
    if (pFile != 0) {
        pXenoAct->auxFileData = pFile->data;
        _DumpHead(pFile);
    }
    if (pJoint != 0) {
        pXenoAct->jointData = pJoint->data;
        _DumpHead(pJoint);
    }
}

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

extern void *CreateRgVector(unsigned int capacity, const char *sourceFile, int line);

/*
 * Resets every field of a freshly allocated actor: its charID and parent
 * links, a new children vector, the effector counts, the resource file
 * slots, the identity local matrix, and the motion and light flags.
 */
void _InitXenoActor(XrgActor *pActor)
{
    unsigned int i;

    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 745);
    }
    pActor->charID = 0;
    pActor->parent = 0;
    pActor->children = CreateRgVector(4, D_00A587E0, 748);
    for (i = 0; i < 2; i++) {
        pActor->effectorCount[i] = 0;
    }
    pActor->resourceFiles[0] = 0;
    pActor->attachID = -1;
    pActor->resourceFiles[2] = 0;
    pActor->resourceFiles[1] = 0;
    pActor->resourceFiles[3] = 0;
    pActor->weaponMotionActive = 0;
    XrgUnitMatrix(pActor->local);
    pActor->pXenoAct = 0;
    pActor->motionID = -1;
    pActor->pakaPaka = 0;
    pActor->smoothPlay = 0;
    do {
        pActor->loopPlay = 0;
    } while (0);
}

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

const char D_00A58B20[28] = "pEntry->m_pszMdlName != NIL";

extern XenoAct *ACT_create(int number, const char *mdlName);
void _LoadXenoActorMdl(XrgActor *pActor, const char *pszMdlName);
/* * Resets every field of a freshly allocated actor: its charID and parent * links, a new children vector, the effector counts, the resource file * slots, the identity local matrix, and the motion and light flags. */ void _InitXenoActor(XrgActor *pActor);
extern void _InitAfterLoadXenoActor(XrgActor *pActor);
void XrgActorSetLight(XrgActor *pActor, RgVector *light);
extern void *InstanceOfGlobalLight(void);

void _LoadXenoActor(XrgActor *pActor, XrgActorEssence *pEss)
{
    XenoDatEntry *pEntry;
    int actorID;

    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 882);
    }
    if (pEss == 0) {
        assert_prog(D_00A58B00, D_00A587E0, 883);
    }
    actorID = pEss->actorID;
    pEntry = _GetXenoDatEntry(actorID);
    if (pEntry == 0) {
        assert_prog(D_00A58B10, D_00A587E0, 887);
    }
    _InitXenoActor(pActor);
    pActor->charID = actorID;
    if (pEntry != 0) {
        pActor->pXenoAct = ACT_create(-1, pEntry->m_pszMdlName);
        if (pActor->pXenoAct == 0) {
            assert_prog(D_00A587F8, D_00A587E0, 894);
        }
        if (pEss->fileName[0] == 0) {
            if (pEntry->m_pszFileBase == 0) {
                assert_prog(D_00A58B20, D_00A587E0, 899);
            }
            _LoadXenoActorMdl(pActor, pEntry->m_pszFileBase);
        } else {
            _LoadXenoActorMdl(pActor, pEss->fileName);
        }
        if (pEntry->m_pszOptFile != 0) {
            pActor->resourceFiles[0] = RgFileSysRead(InstanceOfRgFileSys(), pEntry->m_pszOptFile, 0);
        }
        _InitAfterLoadXenoActor(pActor);
    } else {
        pActor->pXenoAct = 0;
    }
    if (pEss->light != 0) {
        XrgActorSetLight(pActor, pEss->light);
    } else {
        XrgActorSetLight(pActor, (RgVector *) InstanceOfGlobalLight());
    }
    pActor->pakaPaka = 0;
    pActor->unmodeled_160 = 0;
}

void _DupXenoActor(XrgActor *pDup, XrgActor *pActor)
{
    XenoDatEntry *pEntry;

    if (pDup == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 933);
    }
    if (pActor == 0) {
        assert_prog(D_00A58AC8, D_00A587E0, 934);
    }
    pEntry = _GetXenoDatEntry(pActor->charID);
    if (pEntry == 0) {
        assert_prog(D_00A58B10, D_00A587E0, 937);
    }
    pDup->charID = pActor->charID;
    pDup->motionComaStep = 0;
    _InitXenoActor(pDup);
    pDup->pXenoAct = ACT_create(-1, pEntry->m_pszMdlName);
    _DupLoadXenoActor(pDup, pActor);
    _InitAfterLoadXenoActor(pDup);
    pDup->pakaPaka = 0;
}

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

void DisposeXrgActor(XrgActor *pActor)
{
    if (RgHeapIsInvalidMemory(InstanceOfRgHeap(), pActor) != 0) {
        RgError(D_00A58B68, D_00A587E0, 985, pActor);
    }
    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 987);
    }
    _DisposeUseXenoActor(pActor);
    RgHeapFree(InstanceOfRgHeap(), pActor, D_00A587E0, 989);
}

void XrgActorSetLocal(XrgActor *pActor, const RgMatrix matrix)
{
    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 999);
    }
    XrgCopyMatrix(pActor->local, matrix);
}

void XrgActorSetLight(XrgActor *pActor, RgVector *light)
{
    XenoAct *pXenoAct;
    int i;

    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 1008);
    }
    pXenoAct = pActor->pXenoAct;
    if (pXenoAct != 0) {
        if (light != 0) {
            XrgCopyVector(pXenoAct->light.ambient_color_row, light[0]);
            for (i = 0; i < 3; i++) {
                XrgCopyVector(pXenoAct->light.lights[i].color_row, light[4 + i]);
                XrgCopyVector(pXenoAct->light.lights[i].direction_row, light[1 + i]);
            }
        } else {
            xglLightSetDefault(&pXenoAct->light);
        }
    }
}

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
        pActor->pXenoAct->motion.frame = frame;
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

/* The current motion's length, or 0 when no motion is set or loaded. */
float XrgActorGetMotionLength(XrgActor *pActor)
{
    XenoAct *pXenoAct;

    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 1127);
    }
    if (pActor->motionID == -1 || (pXenoAct = pActor->pXenoAct) == 0) {
        return 0.0f;
    }
    return pXenoAct->motion.length;
}

/* The current motion's playing time, or 0 when no motion is set or loaded. */
float XrgActorGetPlayingMotTime(XrgActor *pActor)
{
    XenoAct *pXenoAct;

    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 1138);
    }
    if (pActor->motionID == -1 || (pXenoAct = pActor->pXenoAct) == 0) {
        return 0.0f;
    }
    return pXenoAct->motion.frame;
}

void XrgActorGetLocal(XrgActor *pActor, RgMatrix matrix)
{
    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 1149);
    }
    XrgCopyMatrix(matrix, pActor->local);
}

int XrgActorGetJointLocal(XrgActor *pActor, int jntID, RgMatrix matrix)
{
    XenoAct *pXenoAct;
    RgMatrix *jointMatrices;
    int jointIndex;
    int result;

    result = 0;
    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 1161);
    }
    pXenoAct = pActor->pXenoAct;
    if (pXenoAct == 0) {
        assert_prog(D_00A587F8, D_00A587E0, 1163);
    }
    if (!(RG_ACTOR_JNT_LEFT_HAND <= jntID && jntID < RG_ACTOR_JNT_MAX)) {
        assert_prog(D_00A58A50, D_00A587E0, 1164);
    }
    if (jntID < 20) {
        if (jntID >= 0) {
            jointIndex = (int) _ActJntToJoint(pXenoAct, jntID);
            if (jointIndex >= 0) {
                jointMatrices = pXenoAct->jointMatrices;
                if (jointIndex < ((SkeJointData *) pXenoAct->jointData)->numElement) {
                    XrgCopyMatrix(matrix, jointMatrices[jointIndex]);
                    result = 1;
                }
            }
        }
    }
    return result;
}

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

/*
 * Scaffold-owned (.rodata still owner: asm): ov12:0x00a58b88 holds the
 * "actor %p Pakapaka %s -> %d : paka base %d" log line.
 */
extern const char D_00A58B88[];
extern const char *RgActorCharIDToName(int charID);
int _GetPakaPakaBaseJnt(int charID, int attachID);
extern void XrgLog(const char *format, const char *sourceFile, int line, ...);

void XrgActorCheckPakaPaka(XrgActor *pActor)
{
    XrgActor *pParent;
    int baseJnt;
    int charID;

    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 1240);
    }
    pParent = pActor->parent;
    if (pParent != 0) {
        baseJnt = _GetPakaPakaBaseJnt(pParent->charID, pActor->attachID);
        pActor->pakaBaseJnt = baseJnt;
        if (baseJnt >= 0) {
            /* Read before the pakaPaka store, as at ov12:0x00a4783c. */
            charID = pParent->charID;
            pActor->pakaPaka = 1;
            /* do/while (0) keeps the original's jal: no sibling call. */
            do {
                XrgLog(D_00A58B88, D_00A587E0, 1250, pActor,
                       RgActorCharIDToName(charID), pActor->attachID,
                       pActor->pakaBaseJnt);
            } while (0);
        }
    }
}

extern int RgVectorRemove(void *vector, void *element, const char *source_file,
                          int line);
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

void _PassTime(XrgActor *pActor, float dt);

void XrgActorPassTime(XrgActor *pActor, float dt)
{
    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 1285);
    }
    _PassTime(pActor, dt);
}

void InitXrgActorEssence(XrgActorEssence *pEss, int actorID)
{
    if (pEss == 0) {
        assert_prog(D_00A58B00, D_00A587E0, 1295);
    }
    pEss->actorID = actorID;
    pEss->fileName[0] = 0;
    pEss->light = 0;
}

const char D_00A58CB0[] = "actor %p (xeno %p) ---------\n";

const char D_00A58CD0[] = "time = %f start %f end %f\n";

const char D_00A58CF0[] = "flags = %x(end %x) next %x\n";

extern char *strcpy(char *destination, const char *source);

int XrgActorEssenceGetFileName(XrgActorEssence *pEss, char *pFileName)
{
    XenoDatEntry *pEntry;
    int count;

    if (pEss == 0) {
        assert_prog(D_00A58B00, D_00A587E0, 1308);
    }
    pEntry = _GetXenoDatEntry(pEss->actorID);
    if (pEntry == 0) {
        return 0;
    }
    count = 0;
    if (pEss->fileName[0] == 0) {
        if (pEntry->m_pszFileBase == 0) {
            assert_prog(D_00A58B20, D_00A587E0, 1318);
        }
        if (pFileName != 0) {
            count = 3;
            strcat(strcpy(pFileName, pEntry->m_pszFileBase), D_00A58AB0);
            strcat(strcpy(pFileName + 64, pEntry->m_pszFileBase), D_00A58AB8);
            strcat(strcpy(pFileName + 128, pEntry->m_pszFileBase), D_00A58AC0);
        }
    } else {
        if (pFileName != 0) {
            count = 3;
            strcat(strcpy(pFileName, pEss->fileName), D_00A58AB0);
            strcat(strcpy(pFileName + 64, pEss->fileName), D_00A58AB8);
            strcat(strcpy(pFileName + 128, pEss->fileName), D_00A58AC0);
        }
    }
    if (pEntry->m_pszOptFile != 0) {
        strcpy(pFileName + count * 64, pEntry->m_pszOptFile);
        count++;
    }
    return count;
}

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

/*
 * Scaffold-owned (.rodata still owner: asm): the two bare debug strings
 * _GetPakaPakaBaseJnt logs. ov12:0x00a58be0 holds "3\n", ov12:0x00a58be8
 * holds "4\n".
 */
extern const char D_00A58BE0[];
extern const char D_00A58BE8[];

int _GetPakaPakaBaseJnt(int charID, int attachID)
{
    switch (charID) {
    case 0:
        switch (attachID) {
        case 4:
            return 0x35;
        case 5:
            return 0x43;
        }
        break;
    case 2:
        switch (attachID) {
        case 4:
            XrgLog(D_00A58BE0, D_00A587E0, 0x5A8);
            return 0x31;
        case 5:
            XrgLog(D_00A58BE8, D_00A587E0, 0x5AB);
            return 0x40;
        }
        break;
    }
    return -1;
}

extern const char D_00A58BF0[];
extern const char D_00A58C10[];
extern const char D_00A58C20[];
extern const char D_00A58C38[];
extern const char D_00A58C68[];
extern const char D_00A58C88[];
extern void RgMatEffConstraintSetRoot(void *constraint, void *root, void *top);
extern void RgMatEffConstraintSetTop(void *constraint, void *top);
extern void RgMatEffCombineSetRoot(void *combine, void *root, void *top);
extern void RgMatricesEffectorAddManipulator(void *effector, void *unit);
extern void RgMatricesEffectorAddDoubleMani(void *effector, void *first, void *second);
struct MatrixEffector *XrgActorCreateEffConstraint(XrgActor *pActor, unsigned int eEffID, XrgActor *pWep)
{
    void *pCon;
    XrgRawJntData *pRaw;
    SkeJointData *pJointData;
    SkeMani *pMani;
    int accID;
    int i;
    const char *loadFile;

    pCon = CreateRgMatEffConstraint();
    if (pActor == 0 || pWep == 0) {
        assert_prog(D_00A58BF0, D_00A587E0, 1471);
    }
    if (pCon == 0) {
        assert_prog(D_00A58C10, D_00A587E0, 1472);
    }
    pRaw = _FindRawJntData(pActor->charID, eEffID);
    if (pRaw == 0) {
        DisposeRgMatricesEffector(pCon);
        return 0;
    }
    for (i = 0; pRaw->jointMap[i] >= 0; i++) {
        pMani = _CreateSkeMani_sub(pActor->pXenoAct, pRaw->jointMap[i], D_00A587E0, 1483);
        RgMatricesEffectorAddManipulator(pCon, pMani);
    }
    loadFile = D_00A587E0;
    RgMatEffConstraintSetRoot(pCon,
                              _CreateSkeMani_sub(pActor->pXenoAct, pRaw->jointMap[32], loadFile, 1488),
                              _CreateSkeMani_sub(pActor->pXenoAct, pRaw->jointMap[33], loadFile, 1489));
    accID = _ActJntToAccID_sub(8, loadFile, 1492);
    XrgLog(D_00A58C20, loadFile, 1492, 8, accID);
    accID = _ActJntToAccID_sub(8, loadFile, 1493);
    pMani = _CreateAccMani_sub(pWep->pXenoAct, accID, loadFile, 1493);
    RgMatEffConstraintSetTop(pCon, pMani);
    pJointData = pWep->pXenoAct->jointData;
    for (i = 1; i < pJointData->numElement; i++) {
        if (pWep->pXenoAct != 0) {
            pMani = _CreateSkeMani_sub(pWep->pXenoAct, i, D_00A587E0, 1500);
            RgMatricesEffectorAddManipulator(pCon, pMani);
        }
    }
    if (eEffID >= 2) {
        assert_prog(D_00A58C38, D_00A587E0, 1505);
    }
    if ((unsigned int) pActor->effectorCount[eEffID] >= 4) {
        assert_prog(D_00A58C68, D_00A587E0, 1506);
    }
    RgMatricesEffectorSetActivity(pCon, 0);
    pActor->effector[eEffID][pActor->effectorCount[eEffID]++] = pCon;
    return pCon;
}

struct MatrixEffector *XrgActorCreateEffArmCombine(XrgActor *pActor, XrgActor *pSrcActor, XrgActor *pWpn, XrgActor *pSubWpn, unsigned int eEffID)
{
    void *pCon;
    XrgRawJntData *pRaw;
    SkeJointData *pJointData;
    SkeMani *pRoot;
    SkeMani *pJoint;
    XenoAct *pXenoAct;
    XenoAct *pSrcXeno;
    XenoAct *pWpnXeno;
    XenoAct *pSubXeno;
    int effectorIndex;
    int i;
    int jointID;
    SkeMani *jointSubMani;
    SkeMani *jointMani;

    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 1528);
    }
    if (pSrcActor == 0) {
        assert_prog(D_00A58AC8, D_00A587E0, 1529);
    }
    if (pWpn == 0) {
        assert_prog(D_00A58C88, D_00A587E0, 1530);
    }
    pSrcXeno = pSrcActor->pXenoAct;
    pXenoAct = pActor->pXenoAct;
    pRaw = _FindRawJntData(pActor->charID, eEffID);
    if (pRaw == 0) {
        return 0;
    }
    pCon = CreateRgMatEffCombine();
    for (i = 0; pRaw->jointMap[i] >= 0; i++) {
        jointID = pRaw->jointMap[i];
        jointMani = _CreateSkeMani_sub(pXenoAct, jointID, D_00A587E0, 1553);
        jointSubMani = _CreateSkeMani_sub(pSrcXeno, jointID, D_00A587E0, 1554);
        RgMatricesEffectorAddDoubleMani(pCon, jointMani, jointSubMani);
    }
    i = pRaw->jointMap[33];
    pRoot = _CreateSkeMani_sub(pXenoAct, i, D_00A587E0, 1560);
    pJoint = _CreateSkeMani_sub(pSrcXeno, i, D_00A587E0, 1560);
    RgMatEffCombineSetRoot(pCon, pRoot, pJoint);
    if (pWpn != 0) {
        if (pSubWpn != 0) {
            pWpnXeno = pWpn->pXenoAct;
            pSubXeno = pSubWpn->pXenoAct;
            pJointData = pWpnXeno->jointData;
            for (i = 1; i < pJointData->numElement; i++) {
                SkeMani *pMani;
                SkeMani *pSubMani;
                pMani = _CreateSkeMani_sub(pWpnXeno, i, D_00A587E0, 1569);
                pSubMani = _CreateSkeMani_sub(pSubXeno, i, D_00A587E0, 1570);
                if (pMani != 0 && pSubMani != 0) {
                    RgMatricesEffectorAddDoubleMani(pCon, pMani, pSubMani);
                }
            }
        }
    }
    if (eEffID >= 2) {
        assert_prog(D_00A58C38, D_00A587E0, 1577);
    }
    if ((unsigned int) pActor->effectorCount[eEffID] >= 4) {
        assert_prog(D_00A58C68, D_00A587E0, 1578);
    }
    RgMatricesEffectorSetActivity(pCon, 0);
    effectorIndex = pActor->effectorCount[eEffID];
    pActor->effectorCount[eEffID] = effectorIndex + 1;
    pActor->effector[eEffID][effectorIndex] = pCon;
    return pCon;
}

const char D_00A58C98[18] = "eff over (%d)->%d";

void XrgActorDisposeEffector(XrgActor *pActor, struct MatrixEffector *pEffector)
{
    struct MatrixEffector *kept[2][4];
    int keptCount[2];
    unsigned int bank;
    unsigned int i;
    int cnt;

    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 1595);
    }
    for (bank = 0; bank < 2; bank++) {
        keptCount[bank] = pActor->effectorCount[bank];
        for (i = 0; i < pActor->effectorCount[bank]; i++) {
            kept[bank][i] = pActor->effector[bank][i];
        }
    }
    for (bank = 0; bank < 2; bank++) {
        for (i = 0; i < keptCount[bank]; i++) {
            if (kept[bank][i] == pEffector) {
                if (pEffector != 0) {
                    DisposeRgMatricesEffector(pEffector);
                    kept[bank][i] = 0;
                }
            }
        }
    }
    for (bank = 0; bank < 2; bank++) {
        cnt = 0;
        for (i = 0; i < keptCount[bank]; i++) {
            if (kept[bank][i] != 0) {
                pActor->effector[bank][cnt] = kept[bank][i];
                cnt++;
            }
        }
        pActor->effectorCount[bank] = cnt;
        if (cnt >= 4) {
            RgError(D_00A58C98, D_00A587E0, 1623, bank, keptCount[bank]);
        }
    }
}

void XrgActorDebugDisp(XrgActor *pActor)
{
    XenoAct *pXenoAct;
    XenoMotion *pMotion;

    if (pActor == 0) {
        assert_prog(D_00A589C8.actorNotNil, D_00A587E0, 1634);
    }
    pXenoAct = pActor->pXenoAct;
    if (pXenoAct != 0) {
        pMotion = &pXenoAct->motion;
        XrgLog(D_00A58CB0, D_00A587E0, 1638, pActor, pXenoAct);
        XrgLog(D_00A58CD0, D_00A587E0, 1639, pMotion->frame, pMotion->start, pMotion->length);
        XrgLog(D_00A58CF0, D_00A587E0, 1640, pMotion->flags, 0x1000, pMotion->next);
    }
}
