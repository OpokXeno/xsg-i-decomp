/*
 * OV12 original TU 80: 0x00a454f8..0x00a48548 (63 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/act_1.h"
#include "main/nml_model_set.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_actor_charid.h"
#include "ov12/rg_actor_motid.h"
#include "ov12/rg_battle_common_data.h"
#include "ov12/rg_debug_flags.h"
#include "ov12/rg_draw.h"
#include "ov12/rg_error.h"
#include "ov12/rg_filesys.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_light.h"
#include "ov12/rg_matrices_effector.h"
#include "ov12/rg_vector.h"
#include "ov12/xrg_actor.h"
#include "ov12/xrg_rand_int.h"
#include "ov12/xrg_v_log.h"

/*
 * main/act_2.h declares ACT_setMotion2 with three parameters, but the call
 * sites in _SetMotionToXenoActorWithRgMotID pass the playback bits twice (a
 * fourth argument), so this TU declares the ACT_ functions it uses itself.
 */
extern void ACT_initMotion(Actor *pXenoAct);
extern void ACT_initExMotion(Actor *pXenoAct, unsigned int slot, void *motion);
extern int ACT_jointGetAccessories(Actor *pXenoAct, int jntID);
extern void ACT_updateMotion(Actor *pXenoAct);
extern int ACT_modelDraw(Actor *pXenoAct);
extern void ACT_setMotion2(Actor *pXenoAct, int motionId, int flags, int flagsAgain);
extern void ACT_setArms(Actor *pChildXenoAct, Actor *pParentXenoAct,
                        unsigned int accessoryId, int flags);
extern void ACT_setModelWrapper(Actor *pXenoAct, int flags);
extern void ACT_resetParent(Actor *pChildXenoAct, Actor *pParentXenoAct);
extern void ACT_setHumanHand(Actor *pXenoAct, int hand);

/* Numbers of the original tables and limits. */
#define XENO_DAT_COUNT 25
#define RAW_JNT_DATA_COUNT 12
#define RAW_JNT_ROOT_INDEX 32 /* jointMap entry that roots a constraint effector */
#define RAW_JNT_TOP_INDEX 33  /* jointMap entry that tops it, and roots a combine effector */
#define XRG_ACTOR_MAX_JOINT_ELEMENTS 64
#define XRG_ACTOR_EFFECT_LIMIT 4 /* live effectors a bank accepts */
#define XRG_ACTOR_MODEL_FILE_COUNT 3 /* .xtx, .lex and .jnt of a model */
#define XRG_ACTOR_FILE_NAME_SIZE 64  /* stride of XrgActorEssenceGetFileName's name buffer */

/* Actor.animation.slot.flags: the motion has played to its end. */
#define XRG_MOTION_FLAG_ENDED 0x1000

/* Actor.flags: dropping the weapon also hides the actor. */
#define XRG_ACTOR_FLAGS_DROP_WEAPON 0x408

/* Actor.drawEnv.flags bits the setters raise. */
#define XRG_DRAWENV_TRANSPARENT 0x3
#define XRG_DRAWENV_LIGHT_COST 0x40
#define XRG_DRAWENV_HIDDEN 0x100

/* Actor.shadow_kind/shadowSize and drawEnv.renderLevel pairs. */
#define XRG_ACTOR_SHADOW_KIND 7
#define XRG_ACTOR_SHADOW_SIZE 0x30
#define XRG_ACTOR_LIGHT_COST_SHADOW_KIND 9
#define XRG_ACTOR_LIGHT_COST_SHADOW_SIZE 0x70
#define XRG_ACTOR_LIGHT_COST_RENDER_LEVEL 0x12

/* GS ALPHA register value (Cs - Cd) * As + Cd that XrgActorSetTransparent stores. */
#define XRG_ACTOR_BLEND_ALPHA 0x44

/* nmlModelSetRenderLevel level _ActorDrawFunction raises a child actor to. */
#define XRG_ACTOR_CHILD_RENDER_LEVEL 0x10

/* ACT_setMotion2 id flag of the weapon's motion bank (RgMotID from RG_MOT_WEAPON_BASE up). */
#define XRG_ACT_MOTION_ID_EXTENDED 0x100

/* ACT_setMotion2 playback bits built from smoothPlay and loopPlay. */
#define XRG_PLAY_SMOOTH 1
#define XRG_PLAY_LOOP 8

/* ACT_initExMotion slots. */
#define XRG_EX_MOTION_SLOT_ACTOR 0
#define XRG_EX_MOTION_SLOT_WEAPON 1

typedef struct XenoDatEntry {
    int charID;                     /* 0x00 */
    int modelID;                    /* 0x04: the model id ACT_create loads (0x2001, 0x2105, ...) */
    const char *m_pszOptFile;       /* 0x08: optional name XrgActorEssenceGetFileName appends as the fourth entry */
    const char *m_pszFileBase;      /* 0x0C: base name XrgActorEssenceGetFileName copies when the essence has none;
                                       its assert text (actorAssertEntryModelNameNotNil) calls it m_pszMdlName */
} XenoDatEntry;

/*
 * The joint/skeleton data blob Actor.move points to: only the element count
 * _InitSkeMani reads back is named here.
 */
typedef struct SkeJointData {
    unsigned char unmodeled_00[6];
    unsigned short numElement; /* 0x06 */
} SkeJointData;

/* Each record begins with its actor/joint key, followed by the original
 * signed joint-remapping payload. The payload uses -1 as its list terminator.
 */
typedef struct XrgRawJntData {
    int id;
    int jntID;
    int jointMap[RAW_JNT_TOP_INDEX + 1];
} XrgRawJntData;

/* Other translation units. */

/* ACT_* animation module (main), config/symbols/main.txt. */

/* Scaffold-owned data: the joint table, the actor-data table and its strings. */
extern XenoDatEntry s_aDatTbl_1[XENO_DAT_COUNT];
extern const char actorInvalidCharIdFormat[]; /* "invalid actor char ID = %d" */
extern const char actorAlreadyDisposedFormat[]; /* "already disposed XrgActor %p" */
extern const char actorOver32JointText[];
extern const char actorCharEffectErrorFormat[];
extern int nKataJnt_2;
extern const char actorPakaPakaLogFormat[]; /* "actor %p Pakapaka %s -> %d : paka base %d" */
extern const char actorPakaPakaLogThree[]; /* "3\n" */
extern const char actorPakaPakaLogFour[]; /* "4\n" */
extern const char actorAssertActorAndWeaponNotNil[];
extern const char actorAssertEffectNotNil[];
extern const char actorEffectJointLogFormat[];
extern const char actorAssertEffectTypeInRange[];
extern const char actorAssertEffectCountBelowFour[];
extern const char actorAssertWeaponNotNil[];

/* Prototypes of functions used before their definition. */
void _LoadXenoActor(XrgActor *pActor, XrgActorEssence *pEss);
void _DupXenoActor(XrgActor *pDup, XrgActor *pActor);
void _SetMotUseXenoActor(XrgActor *pActor, int motion);
int _IsEndOfMotUseXenoActor(XrgActor *pActor);
void _DisposeUseXenoActor(XrgActor *pActor);
void _SkeManiGet(SkeMani *mani, RgMatrix matrix);
void _SkeManiSet(SkeMani *mani, RgMatrix matrix);
void _LoadXenoActorMdl(XrgActor *pActor, const char *pszMdlName);
void _InitXenoActor(XrgActor *pActor);
void _InitAfterLoadXenoActor(XrgActor *pActor);
void _AttachChildUseXenoActor(XrgActor *pParent, XrgActor *pChild, int eJntID);
void _DetachChildUseXenoActor(XrgActor *pParent, XrgActor *pChild);
void _DrawActor(void *pObject, RgDrawStudio *pStudio);
void _PassTime(XrgActor *pActor, float dt);
int _GetPakaPakaBaseJnt(int charID, int attachID);

const char actorJointToAccIdErrorFormat[48] = "_ActJntToAccID_sub joint=%d error(%s:%d)";

const char actorSourcePath[24] = "../xrg_actor.euc.c";

const char actorAssertXenoActNotNil[16] = "pXenoAct != NIL";

const char actorJointIdErrorFormat[40] = "joint id error (jnt:%d elm:%d)\nat %s %d";

const char actorAssertJointHasMultipleElements[24] = "pJnt->numElement > 1";

/* The two diagnostics share a 48-byte pool, with the second starting at +16. */
const struct {
    char actorNotNil[16];
    char parentMatrixList[32];
} actorAssertTexts = {
    "pActor != NIL",
    "parent mat list nul (char %d)"
};

const char actorAssertActorAndChildNotNil[32] = "pActor != NIL && pChild != NIL";

const char actorAssertJointIdInRange[72] = "RG_ACTOR_JNT_LEFT_HAND <= (eJntID) && (eJntID) < RG_ACTOR_JNT_MAX";

const char actorAssertSrcActorNotNil[24] = "pSrcActor != NIL";

const char actorAssertActorXenoActNotNil[32] = "pActor->m_pXenoAct != NIL";

const char actorAssertEssenceNotNil[16] = "pEss != NIL";

const char actorAssertDupNotNil[16] = "pDup != NIL";

const char actorAssertNewParentNotNil[24] = "pNewParent != NIL";

const char actorCannotDeleteChildFormat[32] = "cannot delete %p from %p\n";


/* The table _ActJntToAccID_sub indexes: one accessory id per joint id. */
static int s_aJntIDToAccID_0[RG_ACTOR_JNT_MAX] = {
    0x18, 0x20, 0x19, 0x21, 0x1A, 0x22, 0x1B, 0x23,
    0, 1, 2, 3, 4, 5, 6, 7, 0x11, 0x12, 0x0F, 0x10, 0x17
};


int _ActJntToAccID_sub(int jntID, const char *sourceFile, int line)
{
    if ((unsigned int) jntID >= RG_ACTOR_JNT_MAX) {
        RgError(actorJointToAccIdErrorFormat, actorSourcePath, 62, jntID, sourceFile, line);
    }
    return s_aJntIDToAccID_0[jntID];
}

int XrgActorJntIDtoAccID(int jntID)
{
    return _ActJntToAccID_sub(jntID, actorSourcePath, 71);
}

/* Returns the joint's accessory entry (accessories[jntID]) or the default. */

int _AccIDToJoint(Actor *pXenoAct, int jntID)
{
    if (pXenoAct == 0) {
        assert_prog(actorAssertXenoActNotNil, actorSourcePath, 78);
    }
    return ACT_jointGetAccessories(pXenoAct, jntID);
}


int _ActJntToJoint(Actor *pXenoAct, int jntID)
{
    if (pXenoAct == 0) {
        assert_prog(actorAssertXenoActNotNil, actorSourcePath, 86);
    }
    return _AccIDToJoint(pXenoAct, _ActJntToAccID_sub(jntID, actorSourcePath, 87));
}


/* Returns the table entry for charID, or warns and returns 0. */
XenoDatEntry *_GetXenoDatEntry(int charID)
{
    unsigned int i;

    for (i = 0; i < XENO_DAT_COUNT; i++) {
        if (s_aDatTbl_1[i].charID == charID) {
            return &s_aDatTbl_1[i];
        }
    }
    RgWarn(actorInvalidCharIdFormat, actorSourcePath, 168, charID);
    return 0;
}



/*
 * Copies the manipulator's joint matrix into matrix, translation column
 * dropped. A manipulator whose actor has no parent also multiplies it by the element-0
 * matrix (translation dropped too).
 */
void _SkeManiGet(SkeMani *mani, RgMatrix matrix)
{
    Actor *pXenoAct;
    RgMatrix *jointMatrices;
    RgMatrix tmp;

    pXenoAct = mani->pXenoAct;
    if (pXenoAct != 0) {
        jointMatrices = pXenoAct->jointMatrices;
        if (pXenoAct->parent != 0) {
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
 * joint matrix block element. A manipulator whose actor has no parent first premultiplies
 * matrix by the inverse of the element-0 base matrix (translation dropped).
 */
void _SkeManiSet(SkeMani *mani, RgMatrix matrix)
{
    Actor *pXenoAct;
    RgMatrix *jointMatrices;
    RgMatrix tmp;
    int elementIndex;
    int row;
    int col;

    pXenoAct = mani->pXenoAct;
    if (pXenoAct != 0) {
        elementIndex = mani->elementIndex;
        jointMatrices = pXenoAct->jointMatrices;
        if (pXenoAct->parent == 0) {
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

int _InitSkeMani(SkeMani *pMani, Actor *pXenoAct, int elmID,
                 const char *sourceFile, int line)
{
    int result;
    SkeJointData *pJointData;
    unsigned short numElement;

    pMani->get = _SkeManiGet;
    pMani->set = _SkeManiSet;
    if (pXenoAct != 0) {
        pJointData = pXenoAct->move;
        result = 0;
        if (elmID > 0) {
            numElement = pJointData->numElement;
            if (elmID >= numElement) {
                RgWarn(actorJointIdErrorFormat, actorSourcePath, 256, elmID, numElement, sourceFile, line);
                if (pJointData->numElement < 2) {
                    assert_prog(actorAssertJointHasMultipleElements, actorSourcePath, 257);
                }
                elmID = 1;
            }
            pMani->pXenoAct = pXenoAct;
            pMani->elementIndex = elmID;
            goto bound;
        }
    } else {
        pMani->pXenoAct = 0;
      bound:
        result = 1;
    }
    return result;
}

const char actorAssertUnitNotNil[16] = "pUnit != NIL";

/*
 * Allocates a SkeMani record and binds it to pXenoAct's element elmID through
 * _InitSkeMani; the record is freed and NULL returned when the binding fails.
 */
SkeMani *_CreateSkeMani_sub(Actor *pXenoAct, int elmID, const char *sourceFile,
                            int line)
{
    SkeMani *pMani;

    pMani = RgHeapAlloc(InstanceOfRgHeap(), sizeof(SkeMani), sourceFile, line);
    if (pMani == 0) {
        assert_prog(actorAssertUnitNotNil, actorSourcePath, 272);
    }
    if (_InitSkeMani(pMani, pXenoAct, elmID, sourceFile, line) == 0) {
        RgHeapFree(InstanceOfRgHeap(), pMani, actorSourcePath, 274);
        return 0;
    }
    return pMani;
}

const char actorAssertEntryNotNil[16] = "pEntry != NIL";

SkeMani *_CreateAccMani_sub(Actor *pXenoAct, int jntID, const char *sourceFile, int line)
{
    SkeMani *pMani;

    pMani = RgHeapAlloc(InstanceOfRgHeap(), sizeof(SkeMani), actorSourcePath, 285);
    if (pMani == 0) {
        assert_prog(actorAssertUnitNotNil, actorSourcePath, 286);
    }
    if (pXenoAct != 0) {
        if (_InitSkeMani(pMani, pXenoAct, _AccIDToJoint(pXenoAct, jntID), sourceFile, line) == 0) {
            RgHeapFree(InstanceOfRgHeap(), pMani, actorSourcePath, 291);
            return 0;
        }
    } else {
        if (_InitSkeMani(pMani, 0, 0, sourceFile, line) == 0) {
            RgHeapFree(InstanceOfRgHeap(), pMani, actorSourcePath, 296);
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
        DisposeRgFileSysData_sub(pActor->resourceFiles[0], actorSourcePath, 321);
    }
    if (pActor->resourceFiles[1] != 0) {
        DisposeRgFileSysData_sub(pActor->resourceFiles[1], actorSourcePath, 323);
    }
    if (pActor->resourceFiles[2] != 0) {
        DisposeRgFileSysData_sub(pActor->resourceFiles[2], actorSourcePath, 325);
    }
    if (pActor->resourceFiles[3] != 0) {
        DisposeRgFileSysData_sub(pActor->resourceFiles[3], actorSourcePath, 327);
    }
    if (pActor->pXenoAct != 0) {
        ACT_dispose(pActor->pXenoAct);
    }
    for (i = 0; i < XRG_ACTOR_EFFECT_BANKS; i++) {
        for (j = 0; j < pActor->effectorCount[i]; j++) {
            DisposeRgMatricesEffector(pActor->effector[i][j]);
        }
        pActor->effectorCount[i] = 0;
    }
    DisposeRgVector(pActor->children, actorSourcePath, 340);
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


void _PassTime(XrgActor *pActor, float dt)
{
    RgMatrix locals[XRG_ACTOR_MAX_JOINT_ELEMENTS];
    RgMatrix a;
    RgMatrix b;
    RgMatrix c;
    Actor *pXenoAct;
    XrgActor *pParent;
    Actor *pParentAct;
    SkeJointData *pJntData;
    RgMatrix *pOwnMatrices;
    float *pRoot;
    RgMatrix *pParentMatrices;
    unsigned int numElement;
    unsigned int i;
    unsigned int j;
    float step;
    const float frameTime = 1.0f / 30.0f;

    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 371);
    }
    pXenoAct = pActor->pXenoAct;
    if (pXenoAct == 0) {
        return;
    }
    pParent = pActor->parent;
    if (pParent == 0) {
        pParentAct = 0;
        pXenoAct->position.x = pActor->local[12];
        pXenoAct->position.y = pActor->local[13] - 1.5f;
        pXenoAct->position.z = pActor->local[14];
        pXenoAct->rotation.y = atan2(-pActor->local[8], -pActor->local[10]);
    } else {
        pParentAct = pParent->pXenoAct;
    }
    if (pActor->motionComaStep != 0) {
        step = pActor->motionComaStepAccum + dt;
        pActor->motionComaStepAccum = step;
        if (!(frameTime <= step)) {
            pXenoAct->animation.slot.speed = 0.0f;
        } else {
            pActor->motionComaStepAccum = step - frameTime;
            pXenoAct->animation.slot.speed = frameTime;
        }
    } else {
        pXenoAct->animation.slot.speed = dt;
    }
    ACT_updateMotion(pXenoAct);
    if (pActor->pakaPaka != 0 && pParentAct != 0) {
        nKataJnt_2 = pActor->pakaBaseJnt;
        pJntData = pXenoAct->move;
        pOwnMatrices = pXenoAct->jointMatrices;
        pParentMatrices = pParentAct->jointMatrices;
        if (pJntData != 0 && pParentMatrices != 0 && pOwnMatrices != 0) {
            pRoot = pOwnMatrices[1];
            _ActJntToJoint(pParentAct, pActor->attachID);
            if (pActor->pakaBaseMatrixSet == 0) {
                _copy_matrix(b, pParentMatrices[0]);
                _copy_matrix(a, pParentMatrices[nKataJnt_2]);
                XrgMulMatrix(a, b, a);
                _copy_matrix(b, pRoot);
                XrgInvMatrix(c, a);
                XrgMulMatrix(pActor->pakaBaseMatrix, c, b);
                pActor->pakaBaseMatrixSet = 1;
            }
            numElement = pJntData->numElement;
            if (numElement > XRG_ACTOR_MAX_JOINT_ELEMENTS) {
                RgWarn(actorOver32JointText, actorSourcePath, 456);
            }
            _copy_matrix(a, pRoot);
            XrgInvMatrix(c, a);
            for (i = 1; i < numElement; i++) {
                _copy_matrix(a, pOwnMatrices[i]);
                XrgMulMatrix(locals[i], c, a);
            }
            _copy_matrix(b, pParentMatrices[0]);
            _copy_matrix(a, pParentMatrices[nKataJnt_2]);
            XrgMulMatrix(b, b, a);
            XrgMulMatrix(b, b, pActor->pakaBaseMatrix);
            for (i = 1; i < numElement; i++) {
                XrgMulMatrix(a, b, locals[i]);
                _store_matrix(pOwnMatrices[i], a);
            }
        }
    }
    for (i = 0; i < XRG_ACTOR_EFFECT_BANKS; i++) {
        for (j = 0; j < pActor->effectorCount[i]; j++) {
            if (pActor->effector[i][j] != 0) {
                RgMatricesEffectorJob(pActor->effector[i][j]);
            } else {
                RgWarn(actorCharEffectErrorFormat, actorSourcePath, 487, pActor->charID, i, j);
            }
        }
    }
}


RgFog *s_pUseFog = 0;

void _ActorDrawFunction(Actor *pXenoAct)
{
    RgFog *pFog;
    Vector4 color;

    if (InstanceOfRgDebugFlags()->flags[2] != 0) {
        pFog = s_pUseFog;
        if (pFog != 0) {
            color.x = pFog->color[0];
            color.y = pFog->color[1];
            color.z = pFog->color[2];
            nmlModelSetFogCol(&color);
            nmlModelSetFogDist(pFog->dist[0], pFog->dist[1], pFog->dist[2], pFog->dist[3]);
            if (pXenoAct->parent != 0) {
                nmlModelSetRenderLevel(XRG_ACTOR_CHILD_RENDER_LEVEL);
            }
        }
    }
}


void _DrawActor(void *pObject, RgDrawStudio *pStudio)
{
    XrgActor *pActor = pObject;
    RgDrawStudio *pDrawStudio;

    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 527);
    }
    pDrawStudio = pStudio;
    s_pUseFog = pDrawStudio->m_pFog;
    ACT_modelDraw(pActor->pXenoAct);
}


void _SetMotionToXenoActorWithRgMotID(XrgActor *pActor, int rgMotID, int playbackParam)
{
    if (rgMotID >= RG_MOT_WEAPON_BASE && pActor->weaponMotionActive != 0) {
        ACT_setMotion2(pActor->pXenoAct, (rgMotID - RG_MOT_WEAPON_BASE) | XRG_ACT_MOTION_ID_EXTENDED, playbackParam, playbackParam);
        return;
    }
    ACT_setMotion2(pActor->pXenoAct, rgMotID, playbackParam, playbackParam);
}

void _SetMotUseXenoActor(XrgActor *pActor, int motion)
{
    Actor *pXenoAct;
    int playbackParam;

    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 563);
    }
    pXenoAct = pActor->pXenoAct;
    if (pXenoAct != 0) {
        playbackParam = pActor->smoothPlay != 0;
        if (pActor->loopPlay != 0) {
            playbackParam |= XRG_PLAY_LOOP;
        }
        pActor->smoothPlay = 0;
        pActor->loopPlay = 0;
        if (pActor->parent == 0) {
            ACT_initMotion(pXenoAct);
        }
        _SetMotionToXenoActorWithRgMotID(pActor, motion, playbackParam);
        pActor->motionID = motion;
        if (pActor->charID == RG_ACTOR_CHAR_VX02 && motion == 63 ||
            pActor->charID == RG_ACTOR_CHAR_AG04) {
            pActor->motionComaStep = 1;
        } else {
            pActor->motionComaStep = 0;
        }
        pActor->motionComaStepAccum = 0.0f;
    }
}


void _AttachChildUseXenoActor(XrgActor *pParent, XrgActor *pChild, int eJntID)
{
    if (pParent == 0 || pChild == 0) {
        assert_prog(actorAssertActorAndChildNotNil, actorSourcePath, 606);
    }
    if (!(RG_ACTOR_JNT_LEFT_HAND <= eJntID && eJntID < RG_ACTOR_JNT_MAX)) {
        assert_prog(actorAssertJointIdInRange, actorSourcePath, 607);
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
               _ActJntToAccID_sub(eJntID, actorSourcePath, 615), 0);
}


void _DetachChildUseXenoActor(XrgActor *pParent, XrgActor *pChild)
{
    if (pParent == 0 || pChild == 0) {
        assert_prog(actorAssertActorAndChildNotNil, actorSourcePath, 623);
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
    Actor *pXenoAct = pActor->pXenoAct;

    if (pActor->motionID == -1) {
        return 1;
    }
    if (pXenoAct == 0 || (pXenoAct->animation.slot.flags & XRG_MOTION_FLAG_ENDED) == 0) {
        return 0;
    }
    return 1;
}

void _DumpHead(RgFileSysData *pFile) {

}

const char actorAssertModelNameNotNil[18] = "pszMdlName != NIL";

const char actorTextureExtension[5] = ".xtx";

const char actorLexExtension[5] = ".lex";

const char actorJointExtension[5] = ".jnt";

void _LoadXenoActorMdl(XrgActor *pActor, const char *pszMdlName)
{
    Actor *pXenoAct;
    char szFileName[XRG_ACTOR_MODEL_FILE_COUNT][128];
    RgFileSysData *pFile;
    RgFileSysData *pJoint;
    RgFileSysData *pDup;

    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 681);
    }
    if (pszMdlName == 0) {
        assert_prog(actorAssertModelNameNotNil, actorSourcePath, 682);
    }
    pXenoAct = pActor->pXenoAct;
    if (pXenoAct == 0) {
        assert_prog(actorAssertXenoActNotNil, actorSourcePath, 685);
    }
    strcat(strcpy(szFileName[0], pszMdlName), actorTextureExtension);
    strcat(strcpy(szFileName[1], pszMdlName), actorLexExtension);
    strcat(strcpy(szFileName[2], pszMdlName), actorJointExtension);
    pFile = RgFileSysRead(InstanceOfRgFileSys(), szFileName[0], 0);
    pActor->resourceFiles[1] = pFile;
    pJoint = RgFileSysRead(InstanceOfRgFileSys(), szFileName[2], 0);
    pActor->resourceFiles[3] = pJoint;
    pDup = RgFileSysDup(InstanceOfRgFileSys(), szFileName[1], 0);
    pActor->resourceFiles[2] = pDup;
    if (pDup != 0) {
        pXenoAct->modelResource = pDup->data;
        _DumpHead(pDup);
    }
    if (pFile != 0) {
        pXenoAct->matrixResource = pFile->data;
        _DumpHead(pFile);
    }
    if (pJoint != 0) {
        pXenoAct->move = pJoint->data;
        _DumpHead(pJoint);
    }
}


void _DupLoadXenoActor(XrgActor *pActor, XrgActor *pSrcActor)
{
    RgFileSysData *pDupFile;
    RgFileSysData *pAuxFile;
    RgFileSysData *pJointFile;
    Actor *pXenoAct;

    pDupFile = 0;
    pAuxFile = 0;
    pJointFile = 0;
    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 718);
    }
    if (pSrcActor == 0) {
        assert_prog(actorAssertSrcActorNotNil, actorSourcePath, 719);
    }
    pXenoAct = pActor->pXenoAct;
    if (pXenoAct == 0) {
        assert_prog(actorAssertXenoActNotNil, actorSourcePath, 723);
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
        pXenoAct->modelResource = pDupFile->data;
    }
    if (pAuxFile != 0) {
        pXenoAct->matrixResource = pAuxFile->data;
    }
    if (pJointFile != 0) {
        pXenoAct->move = pJointFile->data;
    }
}


/*
 * Resets every field of a freshly allocated actor: its charID and parent
 * links, a new children vector, the effector counts, the resource file
 * slots, the identity local matrix, and the motion and light flags.
 */
void _InitXenoActor(XrgActor *pActor)
{
    unsigned int i;

    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 745);
    }
    pActor->charID = 0;
    pActor->parent = 0;
    pActor->children = CreateRgVector(4, actorSourcePath, 748);
    for (i = 0; i < XRG_ACTOR_EFFECT_BANKS; i++) {
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
    void *weaponMotion;
    Actor *pXenoAct;

    weaponMotion = RgBattleCommonDataGetWeaponMot(InstanceOfRgBattleCommonData());
    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 776);
    }
    pXenoAct = pActor->pXenoAct;
    if (pXenoAct == 0) {
        assert_prog(actorAssertXenoActNotNil, actorSourcePath, 778);
    }
    if (weaponMotion != 0) {
        pActor->weaponMotionActive = 1;
        ACT_initExMotion(pXenoAct, XRG_EX_MOTION_SLOT_WEAPON, weaponMotion);
    }
}


void _InitAfterLoadXenoActor(XrgActor *pActor)
{
    Actor *pXenoAct;

    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 790);
    }
    pXenoAct = pActor->pXenoAct;
    if (pXenoAct == 0) {
        assert_prog(actorAssertActorXenoActNotNil, actorSourcePath, 791);
        pXenoAct = pActor->pXenoAct;
    }
    pActor->motionComaStep = 0;
    pActor->motionComaStepAccum = 0.0f;
    xglLightSetDefault(&pXenoAct->light);
    pXenoAct->draw = _ActorDrawFunction;
    pXenoAct->flags |= 0x8000;
    ACT_allocMatrix(pXenoAct, -1);
    ACT_setModelWrapper(pXenoAct, 0);
    if (pActor->resourceFiles[0] != 0) {
        ACT_initMotion(pXenoAct);
        ACT_initExMotion(pXenoAct, XRG_EX_MOTION_SLOT_ACTOR, pActor->resourceFiles[0]->data);
        _SetBattleCommonWepMotion(pActor);
    }
    pXenoAct->shadow_kind = XRG_ACTOR_SHADOW_KIND;
    pXenoAct->shadowSize = XRG_ACTOR_SHADOW_SIZE;
    pXenoAct->drawEnv.flags |= XRG_DRAWENV_LIGHT_COST;
}

void XrgActorSetLightCost(XrgActor *pActor)
{
    Actor *pXenoAct;

    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 841);
    }
    pXenoAct = pActor->pXenoAct;
    pXenoAct->shadow_kind = XRG_ACTOR_LIGHT_COST_SHADOW_KIND;
    pXenoAct->shadowSize = XRG_ACTOR_LIGHT_COST_SHADOW_SIZE;
    pXenoAct->drawEnv.flags |= XRG_DRAWENV_LIGHT_COST;
    pXenoAct->drawEnv.renderLevel = XRG_ACTOR_LIGHT_COST_RENDER_LEVEL;
}

void XrgActorSetDropWeapon(XrgActor *pActor)
{
    Actor *pXenoAct;

    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 859);
    }
    pXenoAct = pActor->pXenoAct;
    if (pXenoAct != 0) {
        pXenoAct->flags |= XRG_ACTOR_FLAGS_DROP_WEAPON;
    }
}

void XrgActorSetPlayMotionComaStep(XrgActor *pActor, int step)
{
    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 872);
    }
    pActor->motionComaStep = step;
    pActor->motionComaStepAccum = 0.0f;
}

const char actorAssertEntryModelNameNotNil[28] = "pEntry->m_pszMdlName != NIL";


void _LoadXenoActor(XrgActor *pActor, XrgActorEssence *pEss)
{
    XenoDatEntry *pEntry;
    int actorID;

    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 882);
    }
    if (pEss == 0) {
        assert_prog(actorAssertEssenceNotNil, actorSourcePath, 883);
    }
    actorID = pEss->actorID;
    pEntry = _GetXenoDatEntry(actorID);
    if (pEntry == 0) {
        assert_prog(actorAssertEntryNotNil, actorSourcePath, 887);
    }
    _InitXenoActor(pActor);
    pActor->charID = actorID;
    if (pEntry != 0) {
        pActor->pXenoAct = ACT_create(-1, pEntry->modelID);
        if (pActor->pXenoAct == 0) {
            assert_prog(actorAssertXenoActNotNil, actorSourcePath, 894);
        }
        if (pEss->fileName[0] == 0) {
            if (pEntry->m_pszFileBase == 0) {
                assert_prog(actorAssertEntryModelNameNotNil, actorSourcePath, 899);
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
        XrgActorSetLight(pActor, InstanceOfGlobalLight());
    }
    pActor->pakaPaka = 0;
    pActor->pakaBaseMatrixSet = 0;
}

void _DupXenoActor(XrgActor *pDup, XrgActor *pActor)
{
    XenoDatEntry *pEntry;

    if (pDup == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 933);
    }
    if (pActor == 0) {
        assert_prog(actorAssertSrcActorNotNil, actorSourcePath, 934);
    }
    pEntry = _GetXenoDatEntry(pActor->charID);
    if (pEntry == 0) {
        assert_prog(actorAssertEntryNotNil, actorSourcePath, 937);
    }
    pDup->charID = pActor->charID;
    pDup->motionComaStep = 0;
    _InitXenoActor(pDup);
    pDup->pXenoAct = ACT_create(-1, pEntry->modelID);
    _DupLoadXenoActor(pDup, pActor);
    _InitAfterLoadXenoActor(pDup);
    pDup->pakaPaka = 0;
}

XrgActor *CreateXrgActor(XrgActorEssence *pEss)
{
    XrgActor *pActor;

    pActor = RgHeapAlloc(InstanceOfRgHeap(), XRG_ACTOR_SIZE, actorSourcePath, 957);
    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 958);
    }
    _LoadXenoActor(pActor, pEss);
    return pActor;
}

XrgActor *DuplicateXrgActor(XrgActor *pActor, XrgActor *pNewParent)
{
    XrgActor *pDup;

    pDup = RgHeapAlloc(InstanceOfRgHeap(), XRG_ACTOR_SIZE, actorSourcePath, 968);
    if (pDup == 0) {
        assert_prog(actorAssertDupNotNil, actorSourcePath, 969);
    }
    _DupXenoActor(pDup, pActor);
    if (pActor->attachID != -1) {
        if (pNewParent == 0) {
            assert_prog(actorAssertNewParentNotNil, actorSourcePath, 974);
        }
        XrgActorAttachChild(pNewParent, pDup, pActor->attachID);
    }
    return pDup;
}

void DisposeXrgActor(XrgActor *pActor)
{
    if (RgHeapIsInvalidMemory(InstanceOfRgHeap(), pActor) != 0) {
        RgError(actorAlreadyDisposedFormat, actorSourcePath, 985, pActor);
    }
    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 987);
    }
    _DisposeUseXenoActor(pActor);
    RgHeapFree(InstanceOfRgHeap(), pActor, actorSourcePath, 989);
}

void XrgActorSetLocal(XrgActor *pActor, const RgMatrix matrix)
{
    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 999);
    }
    XrgCopyMatrix(pActor->local, matrix);
}

void XrgActorSetLight(XrgActor *pActor, RgLight *light)
{
    Actor *pXenoAct;
    int i;

    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 1008);
    }
    pXenoAct = pActor->pXenoAct;
    if (pXenoAct != 0) {
        if (light != 0) {
            XrgCopyVector(pXenoAct->light.ambient_color_row, light->ambient_color);
            for (i = 0; i < 3; i++) {
                XrgCopyVector(pXenoAct->light.lights[i].color_row, light->color[i]);
                XrgCopyVector(pXenoAct->light.lights[i].direction_row, light->direction[i]);
            }
        } else {
            xglLightSetDefault(&pXenoAct->light);
        }
    }
}

void XrgActorSetMotion(XrgActor *pActor, int motion)
{
    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 1030);
    }
    if (motion != -1) {
        _SetMotUseXenoActor(pActor, motion);
    }
}

void XrgActorSetSmoothPlay(XrgActor *pActor, int smoothPlay)
{
    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 1042);
    }
    pActor->smoothPlay = smoothPlay;
}

void XrgActorSetLoopPlay(XrgActor *pActor, int loopPlay)
{
    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 1050);
    }
    pActor->loopPlay = loopPlay;
}

void XrgActorDeriveMotion(XrgActor *pActor, XrgActor *pSrcActor)
{
    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 1059);
    }
    if (pSrcActor == 0) {
        assert_prog(actorAssertSrcActorNotNil, actorSourcePath, 1060);
    }
    if (pSrcActor->resourceFiles[0] == 0) {
        return;
    }
    ACT_initMotion(pActor->pXenoAct);
    ACT_initExMotion(pActor->pXenoAct, XRG_EX_MOTION_SLOT_ACTOR, pSrcActor->resourceFiles[0]->data);
    _SetBattleCommonWepMotion(pActor);
}

void XrgActorSetMotionFrame(XrgActor *pActor, float frame)
{
    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 1072);
    }
    if ((pActor->pXenoAct != 0) && (pActor->motionID != -1)) {
        if (pActor->parent == 0) {
            ACT_initMotion(pActor->pXenoAct);
        }
        pActor->pXenoAct->animation.slot.frame = frame;
    }
}

void XrgActorSetDraw(XrgActor *pActor, int draw)
{
    Actor *pXenoAct;

    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 1084);
    }
    pXenoAct = pActor->pXenoAct;
    if (pXenoAct != 0) {
        if (draw == 0) {
            pXenoAct->flags |= ACTOR_FLAG_HIDDEN;
            pXenoAct->drawEnv.flags |= XRG_DRAWENV_HIDDEN;
        } else {
            pXenoAct->flags &= ~ACTOR_FLAG_HIDDEN;
        }
    }
}

void XrgActorSetTransparent(XrgActor *pActor, float transparency)
{
    Actor *pXenoAct;

    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 1100);
    }
    pXenoAct = pActor->pXenoAct;
    if (pXenoAct != 0) {
        pXenoAct->drawEnv.transparency = transparency;
        pXenoAct->flags |= ACTOR_FLAG_TRANSLUCENT;
        pXenoAct->drawEnv.alpha = XRG_ACTOR_BLEND_ALPHA;
        pXenoAct->drawEnv.flags |= XRG_DRAWENV_TRANSPARENT;
    }
}

int XrgActorIsEndOfMotion(XrgActor *pActor)
{
    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 1118);
    }
    return _IsEndOfMotUseXenoActor(pActor);
}

/* The current motion's length, or 0 when no motion is set or loaded. */
float XrgActorGetMotionLength(XrgActor *pActor)
{
    Actor *pXenoAct;

    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 1127);
    }
    if (pActor->motionID == -1 || (pXenoAct = pActor->pXenoAct) == 0) {
        return 0.0f;
    }
    return pXenoAct->animation.slot.forwardLimit;
}

/* The current motion's playing time, or 0 when no motion is set or loaded. */
float XrgActorGetPlayingMotTime(XrgActor *pActor)
{
    Actor *pXenoAct;

    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 1138);
    }
    if (pActor->motionID == -1 || (pXenoAct = pActor->pXenoAct) == 0) {
        return 0.0f;
    }
    return pXenoAct->animation.slot.frame;
}

void XrgActorGetLocal(XrgActor *pActor, RgMatrix matrix)
{
    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 1149);
    }
    XrgCopyMatrix(matrix, pActor->local);
}

int XrgActorGetJointLocal(XrgActor *pActor, int jntID, RgMatrix matrix)
{
    Actor *pXenoAct;
    RgMatrix *jointMatrices;
    int jointIndex;
    int result;

    result = 0;
    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 1161);
    }
    pXenoAct = pActor->pXenoAct;
    if (pXenoAct == 0) {
        assert_prog(actorAssertXenoActNotNil, actorSourcePath, 1163);
    }
    if (!(RG_ACTOR_JNT_LEFT_HAND <= jntID && jntID < RG_ACTOR_JNT_MAX)) {
        assert_prog(actorAssertJointIdInRange, actorSourcePath, 1164);
    }
    if (jntID < 20) {
        if (jntID >= 0) {
            jointIndex = _ActJntToJoint(pXenoAct, jntID);
            if (jointIndex >= 0) {
                jointMatrices = pXenoAct->jointMatrices;
                if (jointIndex < ((SkeJointData *) pXenoAct->move)->numElement) {
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
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 1208);
    }
    if (pActor->parent == 0) {
        return -1;
    }
    return pActor->attachID;
}


void XrgActorAttachChild(XrgActor *pParent, XrgActor *pChild, int eJntID)
{
    if (pParent == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 1222);
    }
    if (!(RG_ACTOR_JNT_LEFT_HAND <= eJntID && eJntID < RG_ACTOR_JNT_MAX)) {
        assert_prog(actorAssertJointIdInRange, actorSourcePath, 1223);
    }
    if (pChild != 0) {
        RgVectorPush(pParent->children, pChild);
        _AttachChildUseXenoActor(pParent, pChild, eJntID);
        pChild->parent = pParent;
        pChild->attachID = eJntID;
    }
}


void XrgActorCheckPakaPaka(XrgActor *pActor)
{
    XrgActor *pParent;
    int baseJnt;
    int charID;

    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 1240);
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
                XrgLog(actorPakaPakaLogFormat, actorSourcePath, 1250, pActor,
                       RgActorCharIDToName(charID), pActor->attachID,
                       pActor->pakaBaseJnt);
            } while (0);
        }
    }
}


void XrgActorDettachChild(XrgActor *pParent, XrgActor *pChild)
{
    if (pParent == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 1259);
    }
    if (pChild != 0) {
        if (RgVectorRemove(pParent->children, pChild, actorSourcePath, 1261) == 0) {
            RgWarn(actorCannotDeleteChildFormat, actorSourcePath, 1262, pParent->children, pChild);
        }
        _DetachChildUseXenoActor(pParent, pChild);
        pChild->parent = 0;
        pChild->attachID = -1;
    }
}


void XrgActorDraw(XrgActor *pActor)
{
    RgDrawReq(InstanceOfRgDraw(), pActor, &_DrawActor, 0, 0, -1);
}


void XrgActorPassTime(XrgActor *pActor, float dt)
{
    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 1285);
    }
    _PassTime(pActor, dt);
}

void InitXrgActorEssence(XrgActorEssence *pEss, int actorID)
{
    if (pEss == 0) {
        assert_prog(actorAssertEssenceNotNil, actorSourcePath, 1295);
    }
    pEss->actorID = actorID;
    pEss->fileName[0] = 0;
    pEss->light = 0;
}

const char actorDebugHeaderFormat[] = "actor %p (xeno %p) ---------\n";

const char actorDebugTimeFormat[] = "time = %f start %f end %f\n";

const char actorDebugFlagsFormat[] = "flags = %x(end %x) next %x\n";


int XrgActorEssenceGetFileName(XrgActorEssence *pEss, char *pFileName)
{
    XenoDatEntry *pEntry;
    int count;

    if (pEss == 0) {
        assert_prog(actorAssertEssenceNotNil, actorSourcePath, 1308);
    }
    pEntry = _GetXenoDatEntry(pEss->actorID);
    if (pEntry == 0) {
        return 0;
    }
    count = 0;
    if (pEss->fileName[0] == 0) {
        if (pEntry->m_pszFileBase == 0) {
            assert_prog(actorAssertEntryModelNameNotNil, actorSourcePath, 1318);
        }
        if (pFileName != 0) {
            count = XRG_ACTOR_MODEL_FILE_COUNT;
            strcat(strcpy(pFileName, pEntry->m_pszFileBase), actorTextureExtension);
            strcat(strcpy(pFileName + XRG_ACTOR_FILE_NAME_SIZE, pEntry->m_pszFileBase), actorLexExtension);
            strcat(strcpy(pFileName + 2 * XRG_ACTOR_FILE_NAME_SIZE, pEntry->m_pszFileBase), actorJointExtension);
        }
    } else {
        if (pFileName != 0) {
            count = XRG_ACTOR_MODEL_FILE_COUNT;
            strcat(strcpy(pFileName, pEss->fileName), actorTextureExtension);
            strcat(strcpy(pFileName + XRG_ACTOR_FILE_NAME_SIZE, pEss->fileName), actorLexExtension);
            strcat(strcpy(pFileName + 2 * XRG_ACTOR_FILE_NAME_SIZE, pEss->fileName), actorJointExtension);
        }
    }
    if (pEntry->m_pszOptFile != 0) {
        strcpy(pFileName + count * XRG_ACTOR_FILE_NAME_SIZE, pEntry->m_pszOptFile);
        count++;
    }
    return count;
}

XrgRawJntData s_aRawJntData[RAW_JNT_DATA_COUNT] = {
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
    for (i = 0; i < RAW_JNT_DATA_COUNT; i++, entry++) {
        if (entry->id == id && entry->jntID == jntID) {
            return entry;
        }
    }
    return 0;
}


int _GetPakaPakaBaseJnt(int charID, int attachID)
{
    switch (charID) {
    case RG_ACTOR_CHAR_VX01:
        switch (attachID) {
        case 4:
            return 0x35;
        case 5:
            return 0x43;
        }
        break;
    case RG_ACTOR_CHAR_VX02:
        switch (attachID) {
        case 4:
            XrgLog(actorPakaPakaLogThree, actorSourcePath, 0x5A8);
            return 0x31;
        case 5:
            XrgLog(actorPakaPakaLogFour, actorSourcePath, 0x5AB);
            return 0x40;
        }
        break;
    }
    return -1;
}

MatrixEffector *XrgActorCreateEffConstraint(XrgActor *pActor, unsigned int eEffID, XrgActor *pWep)
{
    MatrixConstraint *pCon;
    XrgRawJntData *pRaw;
    SkeJointData *pJointData;
    SkeMani *pMani;
    int accID;
    int i;
    const char *loadFile;

    pCon = CreateRgMatEffConstraint();
    if (pActor == 0 || pWep == 0) {
        assert_prog(actorAssertActorAndWeaponNotNil, actorSourcePath, 1471);
    }
    if (pCon == 0) {
        assert_prog(actorAssertEffectNotNil, actorSourcePath, 1472);
    }
    pRaw = _FindRawJntData(pActor->charID, eEffID);
    if (pRaw == 0) {
        DisposeRgMatricesEffector(&pCon->effector);
        return 0;
    }
    for (i = 0; pRaw->jointMap[i] >= 0; i++) {
        pMani = _CreateSkeMani_sub(pActor->pXenoAct, pRaw->jointMap[i], actorSourcePath, 1483);
        RgMatricesEffectorAddManipulator(&pCon->effector, pMani);
    }
    loadFile = actorSourcePath;
    RgMatEffConstraintSetRoot(pCon,
                              _CreateSkeMani_sub(pActor->pXenoAct, pRaw->jointMap[RAW_JNT_ROOT_INDEX], loadFile, 1488),
                              _CreateSkeMani_sub(pActor->pXenoAct, pRaw->jointMap[RAW_JNT_TOP_INDEX], loadFile, 1489));
    accID = _ActJntToAccID_sub(8, loadFile, 1492);
    XrgLog(actorEffectJointLogFormat, loadFile, 1492, 8, accID);
    accID = _ActJntToAccID_sub(8, loadFile, 1493);
    pMani = _CreateAccMani_sub(pWep->pXenoAct, accID, loadFile, 1493);
    RgMatEffConstraintSetTop(pCon, pMani);
    pJointData = pWep->pXenoAct->move;
    for (i = 1; i < pJointData->numElement; i++) {
        if (pWep->pXenoAct != 0) {
            pMani = _CreateSkeMani_sub(pWep->pXenoAct, i, actorSourcePath, 1500);
            RgMatricesEffectorAddManipulator(&pCon->effector, pMani);
        }
    }
    if (eEffID >= XRG_ACTOR_EFFECT_BANKS) {
        assert_prog(actorAssertEffectTypeInRange, actorSourcePath, 1505);
    }
    if ((unsigned int) pActor->effectorCount[eEffID] >= XRG_ACTOR_EFFECT_LIMIT) {
        assert_prog(actorAssertEffectCountBelowFour, actorSourcePath, 1506);
    }
    RgMatricesEffectorSetActivity(&pCon->effector, 0);
    pActor->effector[eEffID][pActor->effectorCount[eEffID]++] = &pCon->effector;
    return &pCon->effector;
}

MatrixEffector *XrgActorCreateEffArmCombine(XrgActor *pActor, XrgActor *pSrcActor, XrgActor *pWpn, XrgActor *pSubWpn, unsigned int eEffID)
{
    MatrixCombine *pCon;
    XrgRawJntData *pRaw;
    SkeJointData *pJointData;
    SkeMani *pRoot;
    SkeMani *pJoint;
    Actor *pXenoAct;
    Actor *pSrcXeno;
    Actor *pWpnXeno;
    Actor *pSubXeno;
    int effectorIndex;
    int i;
    int jointID;
    SkeMani *jointSubMani;
    SkeMani *jointMani;

    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 1528);
    }
    if (pSrcActor == 0) {
        assert_prog(actorAssertSrcActorNotNil, actorSourcePath, 1529);
    }
    if (pWpn == 0) {
        assert_prog(actorAssertWeaponNotNil, actorSourcePath, 1530);
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
        jointMani = _CreateSkeMani_sub(pXenoAct, jointID, actorSourcePath, 1553);
        jointSubMani = _CreateSkeMani_sub(pSrcXeno, jointID, actorSourcePath, 1554);
        RgMatricesEffectorAddDoubleMani(&pCon->effector, jointMani, jointSubMani);
    }
    i = pRaw->jointMap[RAW_JNT_TOP_INDEX];
    pRoot = _CreateSkeMani_sub(pXenoAct, i, actorSourcePath, 1560);
    pJoint = _CreateSkeMani_sub(pSrcXeno, i, actorSourcePath, 1560);
    RgMatEffCombineSetRoot(pCon, pRoot, pJoint);
    if (pWpn != 0) {
        if (pSubWpn != 0) {
            pWpnXeno = pWpn->pXenoAct;
            pSubXeno = pSubWpn->pXenoAct;
            pJointData = pWpnXeno->move;
            for (i = 1; i < pJointData->numElement; i++) {
                SkeMani *pMani;
                SkeMani *pSubMani;
                pMani = _CreateSkeMani_sub(pWpnXeno, i, actorSourcePath, 1569);
                pSubMani = _CreateSkeMani_sub(pSubXeno, i, actorSourcePath, 1570);
                if (pMani != 0 && pSubMani != 0) {
                    RgMatricesEffectorAddDoubleMani(&pCon->effector, pMani, pSubMani);
                }
            }
        }
    }
    if (eEffID >= XRG_ACTOR_EFFECT_BANKS) {
        assert_prog(actorAssertEffectTypeInRange, actorSourcePath, 1577);
    }
    if ((unsigned int) pActor->effectorCount[eEffID] >= XRG_ACTOR_EFFECT_LIMIT) {
        assert_prog(actorAssertEffectCountBelowFour, actorSourcePath, 1578);
    }
    RgMatricesEffectorSetActivity(&pCon->effector, 0);
    effectorIndex = pActor->effectorCount[eEffID];
    pActor->effectorCount[eEffID] = effectorIndex + 1;
    pActor->effector[eEffID][effectorIndex] = &pCon->effector;
    return &pCon->effector;
}

const char actorEffectOverflowFormat[18] = "eff over (%d)->%d";

void XrgActorDisposeEffector(XrgActor *pActor, MatrixEffector *pEffector)
{
    MatrixEffector *kept[XRG_ACTOR_EFFECT_BANKS][XRG_ACTOR_EFFECT_LIMIT];
    int keptCount[XRG_ACTOR_EFFECT_BANKS];
    unsigned int bank;
    unsigned int i;
    int cnt;

    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 1595);
    }
    for (bank = 0; bank < XRG_ACTOR_EFFECT_BANKS; bank++) {
        keptCount[bank] = pActor->effectorCount[bank];
        for (i = 0; i < pActor->effectorCount[bank]; i++) {
            kept[bank][i] = pActor->effector[bank][i];
        }
    }
    for (bank = 0; bank < XRG_ACTOR_EFFECT_BANKS; bank++) {
        for (i = 0; i < keptCount[bank]; i++) {
            if (kept[bank][i] == pEffector) {
                if (pEffector != 0) {
                    DisposeRgMatricesEffector(pEffector);
                    kept[bank][i] = 0;
                }
            }
        }
    }
    for (bank = 0; bank < XRG_ACTOR_EFFECT_BANKS; bank++) {
        cnt = 0;
        for (i = 0; i < keptCount[bank]; i++) {
            if (kept[bank][i] != 0) {
                pActor->effector[bank][cnt] = kept[bank][i];
                cnt++;
            }
        }
        pActor->effectorCount[bank] = cnt;
        if (cnt >= XRG_ACTOR_EFFECT_LIMIT) {
            RgError(actorEffectOverflowFormat, actorSourcePath, 1623, bank, keptCount[bank]);
        }
    }
}

void XrgActorDebugDisp(XrgActor *pActor)
{
    Actor *pXenoAct;
    ActorAnimSlot *pMotion;

    if (pActor == 0) {
        assert_prog(actorAssertTexts.actorNotNil, actorSourcePath, 1634);
    }
    pXenoAct = pActor->pXenoAct;
    if (pXenoAct != 0) {
        pMotion = &pXenoAct->animation.slot;
        XrgLog(actorDebugHeaderFormat, actorSourcePath, 1638, pActor, pXenoAct);
        XrgLog(actorDebugTimeFormat, actorSourcePath, 1639, pMotion->frame, pMotion->reverseLimit, pMotion->forwardLimit);
        XrgLog(actorDebugFlagsFormat, actorSourcePath, 1640, pMotion->flags, XRG_MOTION_FLAG_ENDED, pMotion->nextDataId);
    }
}
