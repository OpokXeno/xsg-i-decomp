/*
 * OV12 original TU 29: 0x00a1e880..0x00a1eea0 (11 functions)
 */
#include "common.h"
#include "shared.h"

typedef struct XrgActor XrgActor;
typedef struct MatrixEffector MatrixEffector;

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern void RgError(const char *message, const char *source_file, int line,
                    ...);
extern RgHeap *InstanceOfRgHeap(void);
extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file,
                       int line);
extern void RgMatricesEffectorSetActivity(MatrixEffector *effector,
                                          int activity);
extern void XrgActorSetMotion(XrgActor *pActor, int motion);
extern void XrgActorSetMotionFrame(XrgActor *pActor, float frame);
extern int XrgActorIsEndOfMotion(XrgActor *pActor);
extern void XrgActorPassTime(XrgActor *pActor, float deltaTime);
extern MatrixEffector *XrgActorCreateEffArmCombine(XrgActor *pParentActor,
                                                   XrgActor *shadowActor,
                                                   XrgActor *attachActor,
                                                   XrgActor *attachedActor,
                                                   int combineSelector);
extern void *RgHeapAlloc(RgHeap *heap, unsigned int size,
                         const char *source_file, int line);
extern XrgActor *DuplicateXrgActor(XrgActor *pActor, XrgActor *pNewParent);
extern void XrgActorSetDraw(XrgActor *pActor, int draw);
extern void DisposeXrgActor(XrgActor *pActor);
extern void XrgActorDisposeEffector(XrgActor *pActor, MatrixEffector *effector);

/*
 * ov12:0x00a53d18 "pParentActor != NIL"
 * ov12:0x00a53d30 "../rg_shotmot_control.euc.c"
 * ov12:0x00a53d50 "RG_EQUIP_TYPE_MIN <= (eArmType) && (eArmType) < RG_EQUIP_TYPE_NUM"
 * ov12:0x00a53d98 "eArmType != RG_EQUIP_BACK"
 * ov12:0x00a53db8 "invalid arm type %d"
 * ov12:0x00a53dd0 "pCont != NIL"
 * All six strings are scaffold .rodata (config/tu-build.json data_ownership
 * .rodata: owner asm) with no config/symbols/ov12.txt entry, so they keep
 * their splat names.
 */
extern const char D_00A53D18[];
extern const char D_00A53D30[];
extern const char D_00A53D50[];
extern const char D_00A53D98[];
extern const char D_00A53DB8[];
extern const char D_00A53DD0[];

/*
 * ov12:0x00a53de0 "pCont->m_pMatEff != NIL"
 * ov12:0x00a53df8 "pCont->m_pParent != NIL"
 * Same scaffold .rodata as the six strings above, no config/symbols/ov12.txt
 * entry.
 */
extern const char D_00A53DE0[];
extern const char D_00A53DF8[];

/*
 * RgEquipType slot count and the BACK slot _CreateMatEffector rejects,
 * evidenced by this TU's own assert strings above (0x00a53d50, 0x00a53d98);
 * RG_EQUIP_TYPE_NUM matches the constant already evidenced the same way in
 * src/ov12/rg_robot.h. The unsigned comparison below only tests the upper
 * bound because eArmType is unsigned, so "RG_EQUIP_TYPE_MIN <= eArmType" in
 * the original assert text is always true and was compiled away.
 */
#define RG_EQUIP_TYPE_NUM 3
#define RG_EQUIP_BACK 2

/*
 * Partial RgShotMotCont layout. Only the fields this allocation's own
 * functions read or write are named; _InitCont/_DestructCont/
 * CreateRgShotMotCont (still INCLUDE_ASM in this TU) evidence the rest but
 * are not delivered by this allocation, so their bytes stay unmodeled:
 *   +0x00 active        RgShotMotContIsActive (lw 0(s0), ov12:0x00a1edac),
 *                        RgShotMotContPlay/Stop/PassTime set or test it
 *   +0x08 effector       RgMatricesEffectorSetActivity's argument
 *   +0x0c shadowActor    RgShotMotContIsEndOfShadowMotion (lw 12(s0),
 *                        ov12:0x00a1edec) queries this one's end of motion
 *   +0x10 attachedActor  kept at the same frame/time as shadowActor by
 *                        RgShotMotContSetFrame/PassTime, its own end of
 *                        motion is never queried by this allocation
 *   +0x14 motion         the motion id RgShotMotContPlay last set
 */
typedef struct RgShotMotCont {
    int active;                     /* 0x00 */
    XrgActor *m_pParent;            /* 0x04 */
    MatrixEffector *effector;       /* 0x08 */
    XrgActor *shadowActor;          /* 0x0c */
    XrgActor *attachedActor;        /* 0x10 */
    int motion;                     /* 0x14 */
} RgShotMotCont;

/* Defined later in this TU (still INCLUDE_ASM); called by the function below. */
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
        assert_prog(D_00A53D18, D_00A53D30, 47);
    }
    if (eArmType >= RG_EQUIP_TYPE_NUM) {
        assert_prog(D_00A53D50, D_00A53D30, 48);
    }
    if (eArmType == RG_EQUIP_BACK) {
        assert_prog(D_00A53D98, D_00A53D30, 49);
    }
    if (eArmType != 0) {
        if (eArmType == 1) {
            combineSelector = 1;
        } else {
            RgError(D_00A53DB8, D_00A53D30, 59, eArmType);
        }
    }
    return XrgActorCreateEffArmCombine(pParentActor, shadowActor, attachActor,
                                       attachedActor, combineSelector);
}

static void _InitCont(RgShotMotCont *pCont, XrgActor *pParentActor,
                      XrgActor *attachActor, unsigned int eArmType)
{
    if (pCont == 0) {
        assert_prog(D_00A53DD0, D_00A53D30, 71);
    }
    if (eArmType >= RG_EQUIP_TYPE_NUM) {
        assert_prog(D_00A53D50, D_00A53D30, 72);
    }
    pCont->active = 0;
    pCont->m_pParent = pParentActor;
    pCont->shadowActor = DuplicateXrgActor(pParentActor, 0);
    pCont->attachedActor = DuplicateXrgActor(attachActor, pCont->shadowActor);
    pCont->effector = _CreateMatEffector(pParentActor, pCont->shadowActor, attachActor,
                                         pCont->attachedActor, eArmType);
    if (pCont->effector == 0) {
        assert_prog(D_00A53DE0, D_00A53D30, 83);
    }
    RgMatricesEffectorSetActivity(pCont->effector, 0);
    XrgActorSetDraw(pCont->shadowActor, 0);
    XrgActorSetDraw(pCont->attachedActor, 0);
    pCont->motion = -1;
}

static void _DestructCont(RgShotMotCont *pCont)
{
    if (pCont == 0) {
        assert_prog(D_00A53DD0, D_00A53D30, 96);
    }
    DisposeXrgActor(pCont->attachedActor);
    DisposeXrgActor(pCont->shadowActor);
    if (pCont->effector == 0) {
        return;
    }
    if (pCont->m_pParent == 0) {
        assert_prog(D_00A53DF8, D_00A53D30, 100);
    }
    XrgActorDisposeEffector(pCont->m_pParent, pCont->effector);
}

RgShotMotCont *CreateRgShotMotCont(XrgActor *pParentActor, XrgActor *attachActor,
                                   unsigned int eArmType)
{
    RgShotMotCont *pCont;

    pCont = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgShotMotCont), D_00A53D30, 110);
    if (pCont == 0) {
        assert_prog(D_00A53DD0, D_00A53D30, 111);
    }
    _InitCont(pCont, pParentActor, attachActor, eArmType);
    pCont->motion = -1;
    return pCont;
}

void DisposeRgShotMotCont(RgShotMotCont *pCont)
{
    if (pCont == 0) {
        assert_prog(D_00A53DD0, D_00A53D30, 120);
    }
    _DestructCont(pCont);
    RgHeapFree(InstanceOfRgHeap(), pCont, D_00A53D30, 122);
}

void RgShotMotContPlay(RgShotMotCont *pCont, int motion)
{
    if (pCont == 0) {
        assert_prog(D_00A53DD0, D_00A53D30, 133);
    }
    pCont->active = 1;
    RgMatricesEffectorSetActivity(pCont->effector, 1);
    XrgActorSetMotion(pCont->shadowActor, motion);
    pCont->motion = motion;
}

void RgShotMotContSetFrame(RgShotMotCont *pCont, float frame)
{
    if (pCont == 0) {
        assert_prog(D_00A53DD0, D_00A53D30, 165);
    }
    if (!XrgActorIsEndOfMotion(pCont->shadowActor)) {
        XrgActorSetMotionFrame(pCont->shadowActor, frame);
        XrgActorSetMotionFrame(pCont->attachedActor, frame);
    }
}

void RgShotMotContStop(RgShotMotCont *pCont)
{
    if (pCont == 0) {
        assert_prog(D_00A53DD0, D_00A53D30, 182);
    }
    pCont->active = 0;
    RgMatricesEffectorSetActivity(pCont->effector, 0);
}

int RgShotMotContIsActive(RgShotMotCont *pCont)
{
    if (pCont == 0) {
        assert_prog(D_00A53DD0, D_00A53D30, 195);
    }
    return pCont->active;
}

int RgShotMotContIsEndOfShadowMotion(RgShotMotCont *pCont)
{
    if (pCont == 0) {
        assert_prog(D_00A53DD0, D_00A53D30, 203);
    }
    return XrgActorIsEndOfMotion(pCont->shadowActor);
}

void RgShotMotContPassTime(RgShotMotCont *pCont, float deltaTime)
{
    if (pCont == 0) {
        assert_prog(D_00A53DD0, D_00A53D30, 214);
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
