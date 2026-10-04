/*
 * OV12 original TU 84: 0x00a48c08..0x00a49f70 (20 functions)
 */
#include "common.h"
#include "shared.h"
#include "xrg_particle.h"
#include "ov12/rg_draw.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

/*
 * _openVifGif and _openVifGifAD build the 128-bit VIF1 GIF tag
 * sceVif1PkOpenGifTag takes in the single register the original loads with
 * one `lq`; no wide arithmetic is done on it: bit 15 is EOP, bit 46 is PRE,
 * bits 47..57 are PRIM, bits 60..63 are NREG, and the high 64 bits are the
 * REGS descriptor. The union lets the two halves be stored as ordinary
 * 64-bit fields and the whole 16 bytes be read back as the single register
 * sceVif1PkOpenGifTag takes.
 */
typedef unsigned int Quadword __attribute__((mode(TI)));

typedef union XrgParticleGifTag {
    struct {
        u64 lo;
        u64 hi;
    } part;
    Quadword quad;
} XrgParticleGifTag;

extern void sceVif1PkCnt(XglPacket *packet, int count);
extern void sceVif1PkAlign(XglPacket *packet, int align, int size);
extern void sceVif1PkOpenDirectCode(XglPacket *packet, int mode);
extern void sceVif1PkOpenDirectHLCode(XglPacket *packet, int mode);
extern void sceVif1PkOpenGifTag(XglPacket *packet, Quadword tag);

static void _openVifGif(XglPacket *packet, unsigned int prim,
                        unsigned int nreg, u64 regs)
{
    XrgParticleGifTag tag;

    tag.part.lo = (1u << 15) | (1ULL << 46) | ((u64)prim << 47) |
                  ((u64)nreg << 60);
    tag.part.hi = regs;
    sceVif1PkAlign(packet, 2, 3);
    sceVif1PkCnt(packet, 0);
    sceVif1PkOpenDirectHLCode(packet, 0);
    sceVif1PkOpenGifTag(packet, tag.quad);
}

/*
 * ov12:0x00a58f10 is a fixed GIF tag: EOP set, NREG 1, REGS 0xE (the A+D
 * address+data register code), the constant _openVifGifAD passes to
 * sceVif1PkOpenGifTag.
 */
const XrgParticleGifTag D_00A58F10 = {
    .part = { 0x1000000000008000ULL, 0x000000000000000EULL }
};
const char D_00A58F20[] = "pDrv != NIL";
const char D_00A58F30[] = "../xrg_particle.euc.c";
const char D_00A58F78[] = "pDrv->m_uReqCalcNum < REQ_CALC_MAX";
const char D_00A58FA0[] = "pPoints != NIL";
const char D_00A58FB0[] = "pDrv->m_uReqDispNum < REQ_DISP_MAX";
const char D_00A58FD8[] = "pPic != NIL";
const char D_00A59008[] = "p != NIL";
const char D_00A59018[] = "pArray != NIL";

static void _openVifGifAD(XglPacket *packet)
{
    XrgParticleGifTag tag;

    tag = D_00A58F10;
    sceVif1PkCnt(packet, 0);
    sceVif1PkAlign(packet, 2, 3);
    sceVif1PkOpenDirectCode(packet, 0);
    sceVif1PkOpenGifTag(packet, tag.quad);
}

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

extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(RgHeap *heap, unsigned int size,
                         const char *source_file, int line);

static XrgParticleDriver *CreateXrgParticleDriver(void)
{
    XrgParticleDriver *driver;

    driver = RgHeapAlloc(InstanceOfRgHeap(), sizeof(XrgParticleDriver),
                         D_00A58F30, 340);
    _InitDriver(driver);
    return driver;
}

extern void RgHeapFree(RgHeap *heap, void *pointer, const char *source_file,
                       int line);

void DisposeXrgParticleDriver(XrgParticleDriver *pDrv)
{
    if (pDrv == 0) {
        assert_prog(D_00A58F20, D_00A58F30, 347);
    }
    _DisposeDriver(pDrv);
    RgHeapFree(InstanceOfRgHeap(), pDrv, D_00A58F30, 349);
}

/*
 * The singleton destructor: it forwards the instance it is handed ($a0,
 * untouched before the tail call) to DisposeXrgParticleDriver, which asserts
 * it non-null and frees it.
 */
static void _WrapperDestruct(XrgParticleDriver *driver)
{
    DisposeXrgParticleDriver(driver);
}

extern void *RgSingletonIDGet(unsigned int singleton_id);
extern void RgSingletonIDEntry(int singleton_id, RgSimpleDB *database,
                               void (*destructor)(RgSimpleDB *database));

XrgParticleDriver *InstanceOfXrgParticleDriver(void)
{
    XrgParticleDriver *driver;

    driver = RgSingletonIDGet(7U);
    if (driver == 0) {
        driver = CreateXrgParticleDriver();
        RgSingletonIDEntry(7, (RgSimpleDB *)driver,
                           (void (*)(RgSimpleDB *))_WrapperDestruct);
    }
    return driver;
}

void XrgParticleDriverSet(XrgParticleDriver *driver, int params0, int params1)
{
    if (driver == 0) {
        assert_prog(D_00A58F20, D_00A58F30, 373);
    }
    driver->params0 = params0;
    driver->params1 = params1;
}

#define REQ_CALC_MAX 16

void XrgParticleDriverPassTimeReq(XrgParticleDriver *driver, struct XrgParticle *pPoints,
                                  int count, float rate)
{
    unsigned int idx;

    if (driver == 0) {
        assert_prog(D_00A58F20, D_00A58F30, 392);
    }
    if (driver->m_uReqCalcNum >= REQ_CALC_MAX) {
        assert_prog(D_00A58F78, D_00A58F30, 394);
    }
    if (pPoints == 0) {
        assert_prog(D_00A58FA0, D_00A58F30, 395);
    }
    idx = driver->m_uReqCalcNum++;
    driver->calcReq[idx].particle = pPoints;
    driver->calcReq[idx].count = count;
    driver->calcReq[idx].rate = rate;
}

#define REQ_DISP_MAX 1024

void XrgParticleDriverDispReq(XrgParticleDriver *driver, struct XrgParticle *pPoints,
                              int count, RgBxxPic *pPic)
{
    unsigned int idx;

    if (driver == 0) {
        assert_prog(D_00A58F20, D_00A58F30, 417);
    }
    if (driver->m_uReqDispNum >= REQ_DISP_MAX) {
        assert_prog(D_00A58FB0, D_00A58F30, 419);
    }
    if (pPoints == 0) {
        assert_prog(D_00A58FA0, D_00A58F30, 420);
    }
    if (pPic == 0) {
        assert_prog(D_00A58FD8, D_00A58F30, 421);
    }
    idx = driver->m_uReqDispNum++;
    driver->dispReq[idx].pPoints = pPoints;
    driver->dispReq[idx].count = count;
    driver->dispReq[idx].pPic = pPic;
}

static void _Calc(struct XrgParticle *particle, int count, float rate,
                  float elapsed);

void XrgParticleDriverPassTime(XrgParticleDriver *pDrv, float elapsed)
{
    unsigned int i;
    XrgParticleCalcReq *req;

    if (pDrv == 0) {
        assert_prog(D_00A58F20, D_00A58F30, 438);
    }
    for (i = 0; i < pDrv->m_uReqCalcNum; i++) {
        req = &pDrv->calcReq[i];
        _Calc(req->particle, req->count, req->rate, elapsed);
    }
    pDrv->m_uReqCalcNum = 0;
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_particle", _DrawDriver);

static void _ClearReqDisp(XrgParticleDriver *driver)
{
    if (driver == 0) {
        assert_prog(D_00A58F20, D_00A58F30, 504);
    }
    driver->m_uReqDispNum = 0;
}

extern RgDraw *InstanceOfRgDraw(void);
extern void RgDrawReq(RgDraw *pDraw, XrgParticleDriver *driver,
                      void (*drawFunc)(XrgParticleDriver *driver, void *pStudio),
                      void (*clearFunc)(XrgParticleDriver *driver, void *pStudio),
                      int prio, int drawID);
void _DrawDriver(XrgParticleDriver *driver, void *pStudio);

void XrgParticleDriverDisp(XrgParticleDriver *driver)
{
    if (driver == 0) {
        assert_prog(D_00A59008, D_00A58F30, 511);
    }
    RgDrawReq(InstanceOfRgDraw(), driver, _DrawDriver,
             (void (*)(XrgParticleDriver *, void *))_ClearReqDisp, 2, -2);
}

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
