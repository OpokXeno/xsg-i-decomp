/*
 * OV12 original TU 84: 0x00a48c08..0x00a49f70 (20 functions)
 */
#include "common.h"
#include "shared.h"
#include "xrg_particle.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_particle", _openVifGif_00A48C08);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_particle", _openVifGifAD_00A48C80);

/*
 * Both libvifpk closers take the packet being built: _closeVifGif keeps its
 * argument in $a0 for sceVif1PkCloseGifTag and restores it from $s0 for
 * sceVif1PkCloseDirectHLCode (the same helper and prototypes as
 * src/ov12/xrg_paint2d.c).
 */
typedef struct sceVif1Packet sceVif1Packet;
extern void sceVif1PkCloseGifTag(sceVif1Packet *packet);
extern void sceVif1PkCloseDirectHLCode(sceVif1Packet *packet);

static void _closeVifGif(sceVif1Packet *packet)
{
    sceVif1PkCloseGifTag(packet);
    sceVif1PkCloseDirectHLCode(packet);
}

static void _InitDriver(XrgParticleDriver *driver)
{
    if (driver == 0) {
        assert_prog(D_00A58F20, D_00A58F30, 128);
    }
    driver->params0 = 0;
    driver->params1 = 0;
    driver->m_uReqCalcNum = 0;
    driver->m_uReqDispNum = 0;
}

static void _DisposeDriver(XrgParticleDriver *driver)
{
    if (driver == 0) {
        assert_prog(D_00A58F20, D_00A58F30, 138);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_particle", _Calc);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_particle", _Disp_00A49318);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_particle", CreateXrgParticleDriver);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_particle", DisposeXrgParticleDriver);

/*
 * The singleton destructor: it forwards the instance it is handed ($a0,
 * untouched before the tail call) to DisposeXrgParticleDriver, which asserts
 * it non-null and frees it.
 */
static void _WrapperDestruct(XrgParticleDriver *driver)
{
    DisposeXrgParticleDriver(driver);
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_particle", InstanceOfXrgParticleDriver);

void XrgParticleDriverSet(XrgParticleDriver *driver, int params0, int params1)
{
    if (driver == 0) {
        assert_prog(D_00A58F20, D_00A58F30, 373);
    }
    driver->params0 = params0;
    driver->params1 = params1;
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_particle", XrgParticleDriverPassTimeReq);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_particle", XrgParticleDriverDispReq);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_particle", XrgParticleDriverPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_particle", _DrawDriver);

static void _ClearReqDisp(XrgParticleDriver *driver)
{
    if (driver == 0) {
        assert_prog(D_00A58F20, D_00A58F30, 504);
    }
    driver->m_uReqDispNum = 0;
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_particle", XrgParticleDriverDisp);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_particle", CreateArrayOfXrgParticle);

extern RgHeap *InstanceOfRgHeapData(void);
extern void RgHeapFree(RgHeap *heap, void *pointer, const char *source_file,
                       int line);

void DisposeArrayOfXrgParticle(XrgParticle *array)
{
    if (array == 0) {
        assert_prog(D_00A59018, D_00A58F30, 543);
    }
    RgHeapFree(InstanceOfRgHeapData(), array, D_00A58F30, 544);
}
