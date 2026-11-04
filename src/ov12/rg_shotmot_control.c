#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_equip_type.h"
#include "ov12/rg_shotmot_control.h"
#include "ov12/rg_error.h"
#include "ov12/rg_matrices_effector.h"
#include "ov12/xrg_actor.h"



/* Assertion expressions, source file and error format used below. */
const char shotMotControlAssertParentActorNotNil[24] = "pParentActor != NIL";
const char shotMotControlSourcePath[32] = "../rg_shotmot_control.euc.c";
const char shotMotControlAssertArmTypeInRange[72] = "RG_EQUIP_TYPE_MIN <= (eArmType) && (eArmType) < RG_EQUIP_TYPE_NUM";
const char shotMotControlAssertArmTypeNotBack[32] = "eArmType != RG_EQUIP_BACK";
const char shotMotControlInvalidArmTypeFormat[24] = "invalid arm type %d";
const char shotMotControlAssertContNotNil[16] = "pCont != NIL";

const char shotMotControlAssertMatEffectorNotNil[24] = "pCont->m_pMatEff != NIL";
const char shotMotControlAssertParentNotNil[24] = "pCont->m_pParent != NIL";

static void _DestructCont(RgShotMotCont *pCont);

static MatrixEffector *_CreateMatEffector(XrgActor *pParentActor,
                                          XrgActor *shadowActor,
                                          XrgActor *attachActor,
                                          XrgActor *attachedActor,
                                          unsigned int eArmType)
{
    int combineSelector;

    combineSelector = 0;
    if (pParentActor == 0) {
        assert_prog(shotMotControlAssertParentActorNotNil, shotMotControlSourcePath, 47);
    }
    if (eArmType >= RG_EQUIP_TYPE_NUM) {
        assert_prog(shotMotControlAssertArmTypeInRange, shotMotControlSourcePath, 48);
    }
    if (eArmType == RG_EQUIP_TYPE_BACK) {
        assert_prog(shotMotControlAssertArmTypeNotBack, shotMotControlSourcePath, 49);
    }
    if (eArmType != RG_EQUIP_TYPE_LEFT) {
        if (eArmType == RG_EQUIP_TYPE_RIGHT) {
            combineSelector = 1;
        } else {
            RgError(shotMotControlInvalidArmTypeFormat, shotMotControlSourcePath, 59, eArmType);
        }
    }
    return XrgActorCreateEffArmCombine(pParentActor, shadowActor, attachActor,
                                       attachedActor, combineSelector);
}

static void _InitCont(RgShotMotCont *pCont, XrgActor *pParentActor,
                      XrgActor *attachActor, unsigned int eArmType)
{
    if (pCont == 0) {
        assert_prog(shotMotControlAssertContNotNil, shotMotControlSourcePath, 71);
    }
    if (eArmType >= RG_EQUIP_TYPE_NUM) {
        assert_prog(shotMotControlAssertArmTypeInRange, shotMotControlSourcePath, 72);
    }
    pCont->active = 0;
    pCont->m_pParent = pParentActor;
    pCont->shadowActor = DuplicateXrgActor(pParentActor, 0);
    pCont->attachedActor = DuplicateXrgActor(attachActor, pCont->shadowActor);
    pCont->effector = _CreateMatEffector(pParentActor, pCont->shadowActor, attachActor,
                                         pCont->attachedActor, eArmType);
    if (pCont->effector == 0) {
        assert_prog(shotMotControlAssertMatEffectorNotNil, shotMotControlSourcePath, 83);
    }
    RgMatricesEffectorSetActivity(pCont->effector, 0);
    XrgActorSetDraw(pCont->shadowActor, 0);
    XrgActorSetDraw(pCont->attachedActor, 0);
    pCont->motion = -1;
}

static void _DestructCont(RgShotMotCont *pCont)
{
    if (pCont == 0) {
        assert_prog(shotMotControlAssertContNotNil, shotMotControlSourcePath, 96);
    }
    DisposeXrgActor(pCont->attachedActor);
    DisposeXrgActor(pCont->shadowActor);
    if (pCont->effector == 0) {
        return;
    }
    if (pCont->m_pParent == 0) {
        assert_prog(shotMotControlAssertParentNotNil, shotMotControlSourcePath, 100);
    }
    XrgActorDisposeEffector(pCont->m_pParent, pCont->effector);
}

RgShotMotCont *CreateRgShotMotCont(XrgActor *pParentActor, XrgActor *attachActor,
                                   unsigned int eArmType)
{
    RgShotMotCont *pCont;

    pCont = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgShotMotCont), shotMotControlSourcePath,
                       110);
    if (pCont == 0) {
        assert_prog(shotMotControlAssertContNotNil, shotMotControlSourcePath, 111);
    }
    _InitCont(pCont, pParentActor, attachActor, eArmType);
    pCont->motion = -1;
    return pCont;
}

void DisposeRgShotMotCont(RgShotMotCont *pCont)
{
    if (pCont == 0) {
        assert_prog(shotMotControlAssertContNotNil, shotMotControlSourcePath, 120);
    }
    _DestructCont(pCont);
    RgHeapFree(InstanceOfRgHeap(), pCont, shotMotControlSourcePath, 122);
}

void RgShotMotContPlay(RgShotMotCont *pCont, int motion)
{
    if (pCont == 0) {
        assert_prog(shotMotControlAssertContNotNil, shotMotControlSourcePath, 133);
    }
    pCont->active = 1;
    RgMatricesEffectorSetActivity(pCont->effector, 1);
    XrgActorSetMotion(pCont->shadowActor, motion);
    pCont->motion = motion;
}

void RgShotMotContSetFrame(RgShotMotCont *pCont, float frame)
{
    if (pCont == 0) {
        assert_prog(shotMotControlAssertContNotNil, shotMotControlSourcePath, 165);
    }
    if (!XrgActorIsEndOfMotion(pCont->shadowActor)) {
        XrgActorSetMotionFrame(pCont->shadowActor, frame);
        XrgActorSetMotionFrame(pCont->attachedActor, frame);
    }
}

void RgShotMotContStop(RgShotMotCont *pCont)
{
    if (pCont == 0) {
        assert_prog(shotMotControlAssertContNotNil, shotMotControlSourcePath, 182);
    }
    pCont->active = 0;
    RgMatricesEffectorSetActivity(pCont->effector, 0);
}

int RgShotMotContIsActive(RgShotMotCont *pCont)
{
    if (pCont == 0) {
        assert_prog(shotMotControlAssertContNotNil, shotMotControlSourcePath, 195);
    }
    return pCont->active;
}

int RgShotMotContIsEndOfShadowMotion(RgShotMotCont *pCont)
{
    if (pCont == 0) {
        assert_prog(shotMotControlAssertContNotNil, shotMotControlSourcePath, 203);
    }
    return XrgActorIsEndOfMotion(pCont->shadowActor);
}

void RgShotMotContPassTime(RgShotMotCont *pCont, float deltaTime)
{
    if (pCont == 0) {
        assert_prog(shotMotControlAssertContNotNil, shotMotControlSourcePath, 214);
    }
    if (pCont->active != 0) {
        XrgActorPassTime(pCont->shadowActor, deltaTime);
        XrgActorPassTime(pCont->attachedActor, deltaTime);
        if (XrgActorIsEndOfMotion(pCont->shadowActor) != 0) {
            pCont->active = 0;
            RgMatricesEffectorSetActivity(pCont->effector, 0);
        }
    }
}
