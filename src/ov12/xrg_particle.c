/*
 * OV12 original TU 84: 0x00a48c08..0x00a49f70 (20 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_packet.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_singleton_id.h"
#include "ov12/rg_draw.h"
#include "ov12/xrg_particle.h"
#include "ov12/xrg_rand_int.h"
#include "main/nml_model_set.h"
#include "ov12/xrg_bxx_ps2.h"


/* The GS register _Disp resets; not in main/xgl_packet.h. */
#define SCE_GS_PABE 0x49

/* Particles drawn per packet before it is sent. */
#define XRG_PARTICLE_DISP_BATCH 32
/* Particles whose projected depth reaches this are skipped. */
#define XRG_PARTICLE_DEPTH_LIMIT 28000.0f
/* Largest projected half-size, in 12.4 fixed point, a sprite may have. */
#define XRG_PARTICLE_SPRITE_HALF_MAX 2047u
/* Upper bound of a particle's size (appearance.vec[0]). */
#define XRG_PARTICLE_SIZE_MAX 100.0f
/* The GS primitive coordinate range, 4096 pixels in 12.4 fixed point. */
#define XRG_PARTICLE_GS_COORD_LIMIT 0x10000
/* One sprite is a textured, alpha-blended triangle strip of eight vertices. */
#define XRG_PARTICLE_SPRITE_PRIM \
    (GS_PRIM_TRISTRIP | GS_PRIM_TME | GS_PRIM_ABE | GS_PRIM_FST)
/* Its register list: RGBAQ, then (UV, XYZF2) four times. */
#define XRG_PARTICLE_SPRITE_NREG 9
#define XRG_PARTICLE_SPRITE_REGS 0x434343431ULL

/*
 * _openVifGif and _openVifGifAD build the 128-bit VIF1 GIF tag
 * sceVif1PkOpenGifTag takes in the single register the original loads with
 * one `lq`; no wide arithmetic is done on it: bit 15 is EOP, bit 46 is PRE,
 * bits 47..57 are PRIM, bits 60..63 are NREG, and the high 64 bits are the
 * REGS descriptor. The union lets the two halves be stored as ordinary
 * 64-bit fields and the whole 16 bytes be read back as the single register
 * sceVif1PkOpenGifTag takes.
 */
typedef union XrgParticleGifTag {
    struct {
        u64 lo;
        u64 hi;
    } part;
    Quadword quad;
} XrgParticleGifTag;


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
const XrgParticleGifTag particleGifTag = {
    .part = { 0x1000000000008000ULL, 0x000000000000000EULL }
};

const char particleAssertDriverNotNil[] = "pDrv != NIL";

const char particleSourcePath[] = "../xrg_particle.euc.c";

const char particleAssertReqCalcNumBelowMax[] = "pDrv->m_uReqCalcNum < REQ_CALC_MAX";

const char particleAssertPointsNotNil[] = "pPoints != NIL";

const char particleAssertReqDispNumBelowMax[] = "pDrv->m_uReqDispNum < REQ_DISP_MAX";

const char particleAssertPicNotNil[] = "pPic != NIL";

const char particleAssertPNotNil[] = "p != NIL";

const char particleAssertArrayNotNil[24] = "pArray != NIL";

static void _openVifGifAD(XglPacket *packet)
{
    XrgParticleGifTag tag;

    tag = particleGifTag;
    sceVif1PkCnt(packet, 0);
    sceVif1PkAlign(packet, 2, 3);
    sceVif1PkOpenDirectCode(packet, 0);
    sceVif1PkOpenGifTag(packet, tag.quad);
}

/* Both libvifpk closers take the packet being built. */

static void _closeVifGif(XglPacket *packet)
{
    sceVif1PkCloseGifTag(packet);
    sceVif1PkCloseDirectHLCode(packet);
}

static void _InitDriver(XrgParticleDriver *driver)
{
    if (driver == 0) {
        assert_prog(particleAssertDriverNotNil, particleSourcePath, 128);
    }
    driver->params0 = 0;
    driver->params1 = 0;
    driver->m_uReqCalcNum = 0;
    driver->m_uReqDispNum = 0;
}

static void _DisposeDriver(XrgParticleDriver *driver)
{
    if (driver == 0) {
        assert_prog(particleAssertDriverNotNil, particleSourcePath, 138);
    }
}


/* dst += src * elapsed: scales into the shared temporary, then one VU0 add. */
static inline void _Damp(float *v, float factor)
{
    v[0] *= factor;
    v[1] *= factor;
    v[2] *= factor;
    v[3] *= factor;
}
static inline float *_ScaleFor(float *dst, float *tmp, float *src, float elapsed)
{
    tmp[0] = src[0] * elapsed;
    tmp[1] = src[1] * elapsed;
    tmp[2] = src[2] * elapsed;
    tmp[3] = src[3] * elapsed;
    return dst;
}

/* Wraps an angle into [-pi, pi] by steps of 2*pi. */
static inline float _WrapAngle(float angle)
{
    while (XRG_PI < angle) {
        angle -= XRG_TWO_PI;
    }
    while (angle < -XRG_PI) {
        angle += XRG_TWO_PI;
    }
    return angle;
}

static inline void _ClampState(XrgParticle *e, float zero, float sizeMax, float one)
{
    e->appearance.vec[0] = (e->appearance.vec[0] < zero)
        ? 0.0f : ((sizeMax < e->appearance.vec[0]) ? XRG_PARTICLE_SIZE_MAX : e->appearance.vec[0]);
    e->appearance.vec[1] = (e->appearance.vec[1] < zero)
        ? 0.0f : ((one < e->appearance.vec[1]) ? 1.0f : e->appearance.vec[1]);
    e->appearance.vec[2] = _WrapAngle(e->appearance.vec[2]);
    e->appearance.vec[3] = _WrapAngle(e->appearance.vec[3]);
}

static void _Calc(XrgParticle *particle, int count, float rate,
                  float elapsed)
{
    float tmp[4];
    float factor;
    float *scratch;
    int old;
    XrgParticle *e = particle;
    float zero = 0.0f;
    float sizeMax = XRG_PARTICLE_SIZE_MAX;
    float one = 1.0f;

    if (rate <= zero) {
        for (;;) {
            old = count;
            count--;
            if (old == 0) {
                break;
            }
            scratch = tmp;
            {
                float *dst = _ScaleFor(e->velocity.vec, tmp, e->acceleration.vec, elapsed);

                __asm__ __volatile__("lqc2 vf30, 0(%1)\n" "lqc2 vf31, 0(%2)\n" "vadd.xyzw vf30xyzw, vf30xyzw, vf31xyzw\n" "sqc2 vf30, 0(%0)\n" : : "r" (dst), "r" (dst), "r" (scratch) : "memory");
            }
            {
                float *dst = _ScaleFor(e->position.vec, tmp, e->velocity.vec, elapsed);

                __asm__ __volatile__("lqc2 vf30, 0(%1)\n" "lqc2 vf31, 0(%2)\n" "vadd.xyzw vf30xyzw, vf30xyzw, vf31xyzw\n" "sqc2 vf30, 0(%0)\n" : : "r" (dst), "r" (dst), "r" (scratch) : "memory");
            }
            {
                float *dst = _ScaleFor(e->appearance.vec, tmp, e->appearanceRate.vec, elapsed);

                __asm__ __volatile__("lqc2 vf30, 0(%1)\n" "lqc2 vf31, 0(%2)\n" "vadd.xyzw vf30xyzw, vf30xyzw, vf31xyzw\n" "sqc2 vf30, 0(%0)\n" : : "r" (dst), "r" (dst), "r" (scratch) : "memory");
            }
            _ClampState(e, zero, sizeMax, one);
            e++;
        }
        return;
    }
    factor = one - rate * elapsed;
    for (; (unsigned int) count-- != 0;) {
        {
            float *dst = _ScaleFor(e->velocity.vec, tmp, e->acceleration.vec, elapsed);

            __asm__ __volatile__("lqc2 vf30, 0(%1)\n" "lqc2 vf31, 0(%2)\n" "vadd.xyzw vf30xyzw, vf30xyzw, vf31xyzw\n" "sqc2 vf30, 0(%0)\n" : : "r" (dst), "r" (dst), "r" (tmp) : "memory");
        }
        {
            float *dst = _ScaleFor(e->position.vec, tmp, e->velocity.vec, elapsed);

            __asm__ __volatile__("lqc2 vf30, 0(%1)\n" "lqc2 vf31, 0(%2)\n" "vadd.xyzw vf30xyzw, vf30xyzw, vf31xyzw\n" "sqc2 vf30, 0(%0)\n" : : "r" (dst), "r" (dst), "r" (tmp) : "memory");
        }
        {
            float *dst = _ScaleFor(e->appearance.vec, tmp, e->appearanceRate.vec, elapsed);

            __asm__ __volatile__("lqc2 vf30, 0(%1)\n" "lqc2 vf31, 0(%2)\n" "vadd.xyzw vf30xyzw, vf30xyzw, vf31xyzw\n" "sqc2 vf30, 0(%0)\n" : : "r" (dst), "r" (dst), "r" (tmp) : "memory");
        }
        _Damp(e->velocity.vec, factor);
        _ClampState(e, zero, sizeMax, one);
        e++;
    }
}

static RgDrawView *s_pCurView_00A511F4;

static void _Disp(XrgParticle *pPoints, unsigned int count, long long tex0, XrgParticleRow *rows)
{
    float depthLimit = XRG_PARTICLE_DEPTH_LIMIT;
    XrgParticleRow work[13];
    unsigned char *packet = (unsigned char *)work;
    RgDrawView *pView;
    unsigned int done;
    int n;
    int rem;
    int ret;
    int mask;
    XrgParticleRow *pStore;

    *(DoublewordAlignedVector4 *)&work[2] = (DoublewordAlignedVector4){ 128.0f, 128.0f, 128.0f, 128.0f };
    pView = s_pCurView_00A511F4;
    sceVif1PkInit((XglPacket *)packet, SCRATCHPAD_BASE);
    _openVifGifAD((XglPacket *)packet);
    sceVif1PkAddGsAD((XglPacket *)packet, SCE_GS_TEXFLUSH, 0);
    sceVif1PkAddGsAD((XglPacket *)packet, SCE_GS_TEST_1, 0x73001ULL);
    sceVif1PkAddGsAD((XglPacket *)packet, SCE_GS_CLAMP_1, 0x3fc003fc000ULL);
    sceVif1PkAddGsAD((XglPacket *)packet, SCE_GS_PABE, 0);
    sceVif1PkAddGsAD((XglPacket *)packet, SCE_GS_ALPHA_1, 0x8000000068ULL);
    sceVif1PkAddGsAD((XglPacket *)packet, SCE_GS_TEX0_1, (unsigned long long)tex0);
    _closeVifGif((XglPacket *)packet);

    __asm__ __volatile__(
        "lqc2 vf27, 0(%0)\n"
        "lqc2 vf28, 16(%0)\n"
        "lqc2 vf29, 32(%0)\n"
        "lqc2 vf30, 48(%0)\n"
        :
        : "r"(pView->viewMatrix)
        : "memory");
    __asm__ __volatile__(
        "lqc2 vf25, 0(%0)\n"
        "lqc2 vf26, 0(%1)\n"
        :
        : "r"(pView->axisX), "r"(pView->axisY)
        : "memory");
    __asm__ __volatile__(
        "lqc2 vf31, 0(%0)\n"
        "sqc2 vf31, 0(%1)\n"
        :
        : "r"(pView->axisX), "r"(&work[4])
        : "memory");
    work[4].vec[0] *= 0.375f;
    work[4].vec[1] *= 0.375f;
    XrgUnitVector(work[3].vec);
    pStore = &work[5];
    for (done = 0; done < count; done += XRG_PARTICLE_DISP_BATCH) {
        n = XRG_PARTICLE_DISP_BATCH;
        rem = count - done;
        if (!(n < rem)) {
            n = rem;
        }
        for (; n-- > 0; pPoints++) {
            XrgParticleRow *pDir = &work[3];
            XrgParticleRow *pScratch = &work[6];
            XrgParticleRow *pScale = &work[7];
            XrgParticleRow *pSprite = &work[12];
            XrgParticleRow *pConst = &work[2];

            __asm__ __volatile__(
                "ctc2 $0,$vi18\n"
                "lqc2 vf31, 0(%1)\n"
                "vmulax.xyzw ACC, vf27, vf31x\n"
                "vmadday.xyzw ACC, vf28, vf31y\n"
                "vmaddaz.xyzw ACC, vf29, vf31z\n"
                "vmaddw.xyzw vf31, vf30, vf0w\n"
                "nop\n"
                "nop\n"
                "nop\n"
                "vclipw.xyz vf31, vf31w\n"
                "vdiv Q, vf0w, vf31w\n"
                "vwaitq\n"
                "vmulq.xyzw vf31, vf31, Q\n"
                "vmulaw.xyzw ACC, vf26, vf0w\n"
                "vmadd.xyzw vf31, vf31, vf25\n"
                "vsub.w vf31, vf31, vf31\n"
                "sqc2 vf31, 0(%2)\n"
                "vftoi4.xyzw vf20, vf31\n"
                "sqc2 vf20, 0(%3)\n"
                "cfc2 $2, $vi18\n"
                "nop\n"
                "andi $2, $2, 0x3f\n"
                "move %0, $2\n"
                : "=r"(mask)
                : "r"(pPoints), "r"(pScratch), "r"(pStore)
                : "$2", "memory");
            if (mask != 0) {
                continue;
            }
            if (work[6].vec[2] < depthLimit) {
                int ix;
                int iy;
                int x0;
                int x1;
                Quadword corner;

                work[3].vec[1] = work[3].vec[0] = pPoints->appearance.vec[0] * 0.5f;
                __asm__ __volatile__(
                    "lqc2 vf11, 0(%0)\n"
                    "vmulq.xyzw vf11, vf11, Q\n"
                    :
                    : "r"(pDir)
                    : "memory");
                __asm__ __volatile__(
                    "sqc2 vf11, 0(%0)\n"
                    :
                    : "r"(pDir)
                    : "memory");
                work[7].vec[0] = work[3].vec[0] * work[4].vec[0];
                work[7].vec[1] = work[3].vec[1] * work[4].vec[1];
                work[7].vec[2] = work[3].vec[2] * work[4].vec[2];
                work[7].vec[3] = work[3].vec[3] * work[4].vec[3];
                if (pPoints->appearance.word[2] != 0) {
                    work[7].vec[0] *= cosf(pPoints->appearance.vec[2]);
                }
                if (pPoints->appearance.word[3] != 0) {
                    work[7].vec[1] *= cosf(pPoints->appearance.vec[3]);
                }
                __asm__ __volatile__(
                    "lqc2 vf11, 0(%0)\n"
                    "vftoi4.xyzw vf11, vf11\n"
                    "sqc2 vf11, 0(%1)\n"
                    :
                    : "r"(pScale), "r"(pScale)
                    : "memory");
                ix = work[7].word[0];
                if ((unsigned int)(ix - 1) >= XRG_PARTICLE_SPRITE_HALF_MAX) {
                    continue;
                }
                work[12].vec[0] = pPoints->appearance.vec[1];
                __asm__ __volatile__(
                    "lqc2 vf10, 0(%0)\n"
                    "lqc2 vf11, 0(%1)\n"
                    "vmulx.xyzw vf10, vf10, vf11x\n"
                    "vftoi0.xyzw vf10, vf10\n"
                    "sqc2 vf10, 0(%2)\n"
                    :
                    : "r"(pConst), "r"(pSprite), "r"(pSprite)
                    : "memory");
                corner = work[5].quad;
                work[8].quad = corner;
                work[9].quad = corner;
                x0 = work[8].word[0] - ix;
                x1 = work[9].word[0] + ix;
                work[10].quad = corner;
                work[11].quad = corner;
                work[8].word[0] = x0;
                work[9].word[0] = x1;
                if (x0 > 0) {
                    x0 = XRG_PARTICLE_GS_COORD_LIMIT;
                    if (x1 < x0) {
                        iy = work[7].word[1];
                        work[10].word[0] -= ix;
                        work[8].word[1] -= iy;
                        work[9].word[1] -= iy;
                        work[11].word[0] += ix;
                        work[10].word[1] += iy;
                        work[11].word[1] += iy;
                        _openVifGif((XglPacket *)packet, XRG_PARTICLE_SPRITE_PRIM,
                                    XRG_PARTICLE_SPRITE_NREG,
                                    XRG_PARTICLE_SPRITE_REGS);
                        sceVif1PkAddGsPacked((XglPacket *)packet, work[12].quad);
                        sceVif1PkAddGsPacked((XglPacket *)packet, rows[0].quad);
                        sceVif1PkAddGsPacked((XglPacket *)packet, work[8].quad);
                        sceVif1PkAddGsPacked((XglPacket *)packet, rows[1].quad);
                        sceVif1PkAddGsPacked((XglPacket *)packet, work[9].quad);
                        sceVif1PkAddGsPacked((XglPacket *)packet, rows[2].quad);
                        sceVif1PkAddGsPacked((XglPacket *)packet, work[10].quad);
                        sceVif1PkAddGsPacked((XglPacket *)packet, rows[3].quad);
                        sceVif1PkAddGsPacked((XglPacket *)packet, work[11].quad);
                        _closeVifGif((XglPacket *)packet);
                    }
                }
            }
        }
        if ((ret = sceVif1PkSize((XglPacket *)packet)) != 0) {
            nmlModelDirectSend(pView->projectionMode, (void *)SCRATCHPAD_BASE, ret);
            sceVif1PkInit((XglPacket *)packet, SCRATCHPAD_BASE);
        }
    }
}

static XrgParticleDriver *CreateXrgParticleDriver(void)
{
    XrgParticleDriver *driver;

    driver = RgHeapAlloc(InstanceOfRgHeap(), sizeof(XrgParticleDriver),
                         particleSourcePath, 340);
    _InitDriver(driver);
    return driver;
}

void DisposeXrgParticleDriver(XrgParticleDriver *pDrv)
{
    if (pDrv == 0) {
        assert_prog(particleAssertDriverNotNil, particleSourcePath, 347);
    }
    _DisposeDriver(pDrv);
    RgHeapFree(InstanceOfRgHeap(), pDrv, particleSourcePath, 349);
}

/* The singleton destructor. */
static void _WrapperDestruct(void *driver)
{
    DisposeXrgParticleDriver(driver);
}

XrgParticleDriver *InstanceOfXrgParticleDriver(void)
{
    XrgParticleDriver *driver;

    driver = RgSingletonIDGet(RG_SINGLETON_PARTICLE_DRIVER);
    if (driver == 0) {
        driver = CreateXrgParticleDriver();
        RgSingletonIDEntry(RG_SINGLETON_PARTICLE_DRIVER, driver, _WrapperDestruct);
    }
    return driver;
}

void XrgParticleDriverSet(XrgParticleDriver *driver, int params0, int params1)
{
    if (driver == 0) {
        assert_prog(particleAssertDriverNotNil, particleSourcePath, 373);
    }
    driver->params0 = params0;
    driver->params1 = params1;
}

void XrgParticleDriverPassTimeReq(XrgParticleDriver *driver, XrgParticle *pPoints,
                                  int count, float rate)
{
    unsigned int idx;

    if (driver == 0) {
        assert_prog(particleAssertDriverNotNil, particleSourcePath, 392);
    }
    if (driver->m_uReqCalcNum >= REQ_CALC_MAX) {
        assert_prog(particleAssertReqCalcNumBelowMax, particleSourcePath, 394);
    }
    if (pPoints == 0) {
        assert_prog(particleAssertPointsNotNil, particleSourcePath, 395);
    }
    idx = driver->m_uReqCalcNum++;
    driver->calcReq[idx].particle = pPoints;
    driver->calcReq[idx].count = count;
    driver->calcReq[idx].rate = rate;
}

void XrgParticleDriverDispReq(XrgParticleDriver *driver, XrgParticle *pPoints,
                              int count, RgBxxPic *pPic)
{
    unsigned int idx;

    if (driver == 0) {
        assert_prog(particleAssertDriverNotNil, particleSourcePath, 417);
    }
    if (driver->m_uReqDispNum >= REQ_DISP_MAX) {
        assert_prog(particleAssertReqDispNumBelowMax, particleSourcePath, 419);
    }
    if (pPoints == 0) {
        assert_prog(particleAssertPointsNotNil, particleSourcePath, 420);
    }
    if (pPic == 0) {
        assert_prog(particleAssertPicNotNil, particleSourcePath, 421);
    }
    idx = driver->m_uReqDispNum++;
    driver->dispReq[idx].pPoints = pPoints;
    driver->dispReq[idx].count = count;
    driver->dispReq[idx].pPic = pPic;
}

void XrgParticleDriverPassTime(XrgParticleDriver *pDrv, float elapsed)
{
    unsigned int i;
    XrgParticleCalcReq *req;

    if (pDrv == 0) {
        assert_prog(particleAssertDriverNotNil, particleSourcePath, 438);
    }
    for (i = 0; i < pDrv->m_uReqCalcNum; i++) {
        req = &pDrv->calcReq[i];
        _Calc(req->particle, req->count, req->rate, elapsed);
    }
    pDrv->m_uReqCalcNum = 0;
}

/* The assertion texts "pEnv != NIL" and "pView != NIL", defined elsewhere. */
extern const char particleAssertEnvNotNil[];
extern const char particleAssertViewNotNil[];

void _DrawDriver(void *pObject, RgDrawStudio *pStudio)
{
    XrgParticleDriver *driver = pObject;
    RgDrawView *pView;
    RgBxx *pOwner = 0;
    XrgParticleDispReq *req;
    RgBxxPic *pic;
    XrgParticleRow rows[4];
    long long tex0;
    unsigned int i;
    int dx;
    int dy;
    int py;

    if (driver == 0) {
        assert_prog(particleAssertDriverNotNil, particleSourcePath, 459);
    }
    if (pStudio == 0) {
        assert_prog(particleAssertEnvNotNil, particleSourcePath, 460);
    }
    pView = RgDrawStudioGetView(pStudio);
    if (pView == 0) {
        assert_prog(particleAssertViewNotNil, particleSourcePath, 462);
    }
    s_pCurView_00A511F4 = pView;
    for (i = 0; i < driver->m_uReqDispNum; i++) {
        req = &driver->dispReq[i];
        pic = req->pPic;
        if (pic->pOwner != pOwner) {
            pOwner = pic->pOwner;
            nmlModelDirectSendXtx(pView->projectionMode, RgBxxGetXtx(pOwner));
        }
        tex0 = XrgBxxPs2Tex0(pic);
        rows[0].quad = 0;
        rows[0].word[0] = pic->x * 16 + 8;
        rows[0].word[1] = pic->y * 16 + 8;
        rows[1].quad = rows[0].quad;
        rows[2].quad = rows[0].quad;
        rows[3].quad = rows[0].quad;
        dx = pic->width * 16 - 16;
        dy = pic->height * 16 - 16;
        rows[3].word[0] += dx;
        rows[1].word[0] += dx;
        rows[3].word[1] += dy;
        rows[2].word[1] += dy;
        _Disp(req->pPoints, req->count, tex0, rows);
    }
}

static void _ClearReqDisp(void *pObject)
{
    XrgParticleDriver *driver = pObject;

    if (driver == 0) {
        assert_prog(particleAssertDriverNotNil, particleSourcePath, 504);
    }
    driver->m_uReqDispNum = 0;
}

void XrgParticleDriverDisp(XrgParticleDriver *driver)
{
    if (driver == 0) {
        assert_prog(particleAssertPNotNil, particleSourcePath, 511);
    }
    RgDrawReq(InstanceOfRgDraw(), driver, _DrawDriver, _ClearReqDisp, 2, -2);
}

/*
 * Creates the particle array: count elements of sizeof(XrgParticle), each
 * vector cleared and the first vector's w lane set to 1.0f. The array is
 * asserted non-null before it is initialised.
 */
XrgParticle *CreateArrayOfXrgParticle(int count)
{
    XrgParticle *pArray;
    XrgParticle *pElement;
    unsigned int i;

    pArray = RgHeapAlloc(InstanceOfRgHeapData(), count * sizeof(XrgParticle),
                         particleSourcePath, 524);
    if (pArray == 0) {
        assert_prog(particleAssertPointsNotNil, particleSourcePath, 526);
    }
    pElement = pArray;
    for (i = 0; i < count; i++) {
        pElement->position.quad = 0;
        pElement->velocity.quad = 0;
        pElement->acceleration.quad = 0;
        pElement->appearance.quad = 0;
        pElement->appearanceRate.quad = 0;
        pElement->position.vec[3] = 1.0f;
        pElement++;
    }
    return pArray;
}

void DisposeArrayOfXrgParticle(XrgParticle *array)
{
    if (array == 0) {
        assert_prog(particleAssertArrayNotNil, particleSourcePath, 543);
    }
    RgHeapFree(InstanceOfRgHeapData(), array, particleSourcePath, 544);
}



static RgDrawView * s_pCurView_00A511F4 = 0;


const char particleAssertD128Size[] = "sizeof(D128) == 16";

const char particleAssertEnvNotNil[16] = "pEnv != NIL";

const char particleAssertViewNotNil[16] = "pView != NIL";
