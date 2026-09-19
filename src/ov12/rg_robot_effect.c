/*
 * OV12 original TU 10: 0x00a0df70..0x00a0ece8 (30 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_robot_effect.h"
#include "ov12/xrg_rand_int.h"

/*
 * ov12:0x00a52378, "pGenerator != NIL && pFunc != NIL"
 * ov12:0x00a523a0, "../rg_robot_effect.euc.c"
 * ov12:0x00a523e0, "pRobEff != NIL"
 * All three strings are scaffold .rodata (config/tu-build.json data_ownership
 * .rodata: owner asm) with no config/symbols/ov12.txt entry, so they keep
 * their splat names.
 */
extern const char D_00A52378[];
extern const char D_00A523A0[];
extern const char D_00A523E0[];

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern char *strcpy(char *destination, const char *source);
extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file,
                       int line);
extern void RgGeomPointGetVel(RgGeomPoint *point, RgVector velocity);

/* Same TU, not part of this allocation. */
extern void _InitRobEff(RgRobotEffect *effect);
extern void _ClearEffect(RgRobotEffectItem *item);
extern void RgRobotEffectTermJet(RgRobotEffect *effect);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_effect", _InitJetDefault);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_effect", _InitDashDefault);

static void _InitEffect(RgRobotEffectItem *item)
{
    item->inertia = 0;
    item->particle_count = 0;
    item->clear_time = 999999.0f;
    item->shoot_generator = 0;
    item->shoot_func = 0;
    item->stop_time = 999999.0f;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_effect", _ClearEffect);

static void _SetShootLocalGenEffect(RgRobotEffectItem *item,
                                    RgRobotEffect *generator,
                                    RgRobotEffectShootLocalFunc func)
{
    if (generator == 0 || func == 0) {
        assert_prog(D_00A52378, D_00A523A0, 113);
    }
    item->shoot_generator = generator;
    item->shoot_func = func;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_effect", _AddEffect);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_effect", _StopEffect);

static void _SetStopTimeEffect(RgRobotEffectItem *item, float stop_time)
{
    item->stop_time = stop_time;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_effect", _PassTimeEffect);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_effect", _DispEffect);

static void _SetInertiaEffect(RgRobotEffectItem *item, int inertia)
{
    item->inertia = inertia;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_effect", _InitRobEff);

static void _DestructRobEff(RgRobotEffect *effect)
{
    if (effect == 0) {
        assert_prog(D_00A523E0, D_00A523A0, 230);
    }
    _ClearEffect(&effect->jet);
    _ClearEffect(&effect->dash);
}

static void _GetGeomVel(RgRobotEffect *effect, RgVector velocity)
{
    RgGeomPoint *geom;

    geom = effect->geom;
    if (geom != 0) {
        RgGeomPointGetVel(geom, velocity);
        return;
    }
    XrgClearVector(velocity);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_effect", _GetJetShootLocal);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_effect", _GetDashShootLocal);

RgRobotEffect *CreateRgRobotEffect(void)
{
    RgRobotEffect *effect;

    effect = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgRobotEffect),
                         D_00A523A0, 284);
    _InitRobEff(effect);
    return effect;
}

void DisposeRgRobotEffect(RgRobotEffect *effect)
{
    if (effect == 0) {
        assert_prog(D_00A523E0, D_00A523A0, 292);
    }
    _DestructRobEff(effect);
    RgHeapFree(InstanceOfRgHeap(), effect, D_00A523A0, 294);
}

void RgRobotEffectSetActor(RgRobotEffect *effect, void *actor)
{
    if (effect == 0) {
        assert_prog(D_00A523E0, D_00A523A0, 304);
    }
    effect->actor = actor;
}

void RgRobotEffectSetGeom(RgRobotEffect *effect, RgGeomPoint *geom)
{
    if (effect == 0) {
        assert_prog(D_00A523E0, D_00A523A0, 311);
    }
    effect->geom = geom;
}

void RgRobotEffectSetJetPtclName(RgRobotEffect *effect, const char *name)
{
    if (effect == 0) {
        assert_prog(D_00A523E0, D_00A523A0, 319);
    }
    strcpy(effect->jet.ptcl_name, name);
}

void RgRobotEffectSetDashPtclName(RgRobotEffect *effect, const char *name)
{
    if (effect == 0) {
        assert_prog(D_00A523E0, D_00A523A0, 327);
    }
    strcpy(effect->dash.ptcl_name, name);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_effect", RgRobotEffectSetJetInertia);

void RgRobotEffectTermAll(RgRobotEffect *effect)
{
    RgRobotEffectTermJet(effect);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_effect", RgRobotEffectStartJet);

void RgRobotEffectTermJet(RgRobotEffect *effect)
{
    if (effect == 0) {
        assert_prog(D_00A523E0, D_00A523A0, 387);
    }
    if (effect->actor != 0) {
        _StopEffect(&effect->jet);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_effect", RgRobotEffectStartDash);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_effect", RgRobotEffectTermDash);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_effect", RgRobotEffectPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_effect", RgRobotEffectDisp);
