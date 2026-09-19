/*
 * OV12 original TU 41: 0x00a245d8..0x00a251a8 (13 functions)
 */
#include "common.h"
#include "rg_disp_wpn1p.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

extern void *RgHeapAlloc(RgHeap *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file,
                       int line);
extern RgHeap *InstanceOfRgHeap(void);

extern RgGauge *CreateRgGauge(void);
extern void DisposeRgGauge(RgGauge *gauge);
extern void RgGaugeSetValue(RgGauge *gauge, float value);
extern void RgGaugePassTime(RgGauge *gauge, float deltaTime);

extern float RgWeaponGetShotNum(RgWeapon *weapon);

extern void _InitDisp(RgDispWpn1P *pDisp);
extern void _DestructDisp(RgDispWpn1P *pDisp);

/* ov12:0x00a54678 "pDisp != NIL" */
extern const char D_00A54678[];
/* ov12:0x00a54688 "../rg_disp_wpn1p.euc.c" (source filename, scaffold-owned
 * per config/tu-build.json data_ownership: this .rodata window is still
 * owner "asm"). */
extern const char D_00A54688[];
/* ov12:0x00a546a0 "pBxx != NIL" */
extern const char D_00A546A0[];

static void _InitWepDisp(WepDisp *pWepDisp, RgBxx *pBxx)
{
    if (pWepDisp == 0) {
        assert_prog(D_00A54678, D_00A54688, 41);
    }
    if (pBxx == 0) {
        assert_prog(D_00A546A0, D_00A54688, 42);
    }
    pWepDisp->gauge = CreateRgGauge();
    pWepDisp->bxx = pBxx;
}

static void _DestructWepDisp(WepDisp *pWepDisp)
{
    DisposeRgGauge(pWepDisp->gauge);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_wpn1p", _SetWepDisp);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_wpn1p", _UpdateWepDisp);

static void _PassTimeWepDisp(WepDisp *pWepDisp, float deltaTime)
{
    RgWeapon *weapon;

    weapon = pWepDisp->weapon;
    if (weapon != 0) {
        RgGaugeSetValue(pWepDisp->gauge, RgWeaponGetShotNum(weapon));
        RgGaugePassTime(pWepDisp->gauge, deltaTime);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_wpn1p", _DispWepDisp);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_wpn1p", _InitDisp_00A24BC0);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_wpn1p", _DestructDisp_00A24D68);

RgDispWpn1P *CreateRgDispWpn1P(void)
{
    RgDispWpn1P *pDisp;

    pDisp = RgHeapAlloc(InstanceOfRgHeap(), 0x58, D_00A54688, 417);
    _InitDisp(pDisp);
    return pDisp;
}

void DisposeRgDispWpn1P(RgDispWpn1P *pDisp)
{
    if (pDisp == 0) {
        assert_prog(D_00A54678, D_00A54688, 425);
    }
    _DestructDisp(pDisp);
    RgHeapFree(InstanceOfRgHeap(), pDisp, D_00A54688, 427);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_wpn1p", RgDispWpn1PSetRobot);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_wpn1p", RgDispWpn1PPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_wpn1p", RgDispWpn1PDisp);
