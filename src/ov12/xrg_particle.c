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


struct XrgParticle {
    union {
        Quadword quad;
        RgVector vec;
    } vector[5];
};

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
    while (3.1415927f < angle) {
        angle -= 6.2831855f;
    }
    while (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    return angle;
}

static inline void _ClampState(struct XrgParticle *e, float zero, float hundred, float one)
{
    e->vector[3].vec[0] = (e->vector[3].vec[0] < zero)
        ? 0.0f : ((hundred < e->vector[3].vec[0]) ? 100.0f : e->vector[3].vec[0]);
    e->vector[3].vec[1] = (e->vector[3].vec[1] < zero)
        ? 0.0f : ((one < e->vector[3].vec[1]) ? 1.0f : e->vector[3].vec[1]);
    e->vector[3].vec[2] = _WrapAngle(e->vector[3].vec[2]);
    e->vector[3].vec[3] = _WrapAngle(e->vector[3].vec[3]);
}

static void _Calc(struct XrgParticle *particle, int count, float rate,
                  float elapsed)
{
    float tmp[4];
    float factor;
    float *scratch;
    int old;
    struct XrgParticle *e = particle;
    float zero = 0.0f;
    float hundred = 100.0f;
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
                float *dst = _ScaleFor(e->vector[1].vec, tmp, e->vector[2].vec, elapsed);

                __asm__ __volatile__("lqc2 vf30, 0(%1)\n" "lqc2 vf31, 0(%2)\n" "vadd.xyzw vf30xyzw, vf30xyzw, vf31xyzw\n" "sqc2 vf30, 0(%0)\n" : : "r" (dst), "r" (dst), "r" (scratch) : "memory");
            }
            {
                float *dst = _ScaleFor(e->vector[0].vec, tmp, e->vector[1].vec, elapsed);

                __asm__ __volatile__("lqc2 vf30, 0(%1)\n" "lqc2 vf31, 0(%2)\n" "vadd.xyzw vf30xyzw, vf30xyzw, vf31xyzw\n" "sqc2 vf30, 0(%0)\n" : : "r" (dst), "r" (dst), "r" (scratch) : "memory");
            }
            {
                float *dst = _ScaleFor(e->vector[3].vec, tmp, e->vector[4].vec, elapsed);

                __asm__ __volatile__("lqc2 vf30, 0(%1)\n" "lqc2 vf31, 0(%2)\n" "vadd.xyzw vf30xyzw, vf30xyzw, vf31xyzw\n" "sqc2 vf30, 0(%0)\n" : : "r" (dst), "r" (dst), "r" (scratch) : "memory");
            }
            _ClampState(e, zero, hundred, one);
            e++;
        }
        return;
    }
    factor = one - rate * elapsed;
    for (; (unsigned int) count-- != 0;) {
        {
            float *dst = _ScaleFor(e->vector[1].vec, tmp, e->vector[2].vec, elapsed);

            __asm__ __volatile__("lqc2 vf30, 0(%1)\n" "lqc2 vf31, 0(%2)\n" "vadd.xyzw vf30xyzw, vf30xyzw, vf31xyzw\n" "sqc2 vf30, 0(%0)\n" : : "r" (dst), "r" (dst), "r" (tmp) : "memory");
        }
        {
            float *dst = _ScaleFor(e->vector[0].vec, tmp, e->vector[1].vec, elapsed);

            __asm__ __volatile__("lqc2 vf30, 0(%1)\n" "lqc2 vf31, 0(%2)\n" "vadd.xyzw vf30xyzw, vf30xyzw, vf31xyzw\n" "sqc2 vf30, 0(%0)\n" : : "r" (dst), "r" (dst), "r" (tmp) : "memory");
        }
        {
            float *dst = _ScaleFor(e->vector[3].vec, tmp, e->vector[4].vec, elapsed);

            __asm__ __volatile__("lqc2 vf30, 0(%1)\n" "lqc2 vf31, 0(%2)\n" "vadd.xyzw vf30xyzw, vf30xyzw, vf31xyzw\n" "sqc2 vf30, 0(%0)\n" : : "r" (dst), "r" (dst), "r" (tmp) : "memory");
        }
        _Damp(e->vector[1].vec, factor);
        _ClampState(e, zero, hundred, one);
        e++;
    }
}

extern RgDrawView *s_pCurView_00A511F4;
extern void XrgUnitVector(RgVector vector);
extern float cosf(float x);
extern DoublewordAlignedVector4 D_00A58F50;
extern int sceVif1PkSize(XglPacket *packet);
extern void nmlModelDirectSend(void *pDst, unsigned int flags, int value);
extern void sceVif1PkInit(XglPacket *packet, unsigned int flags);
extern void sceVif1PkAddGsAD(XglPacket *packet, unsigned int reg, unsigned long long data);
extern void sceVif1PkAddGsPacked(XglPacket *packet, Quadword data);
typedef union XrgParticleRow {
    Quadword quad;
    int word[4];
} XrgParticleRow;

static void _Disp(struct XrgParticle *pPoints, unsigned int count, long long tex0, XrgParticleRow *rows)
{
    float depthLimit = 28000.0f;
    Quadword work[13];
    unsigned char *packet = (unsigned char *)work;
    RgDrawView *pView;
    unsigned int done;
    int n;
    int rem;
    int ret;
    int mask;
    Quadword *pStore;

    *(DoublewordAlignedVector4 *)&work[2] = D_00A58F50;
    pView = s_pCurView_00A511F4;
    sceVif1PkInit((XglPacket *)packet, 0x70000000);
    _openVifGifAD((XglPacket *)packet);
    sceVif1PkAddGsAD((XglPacket *)packet, 63, 0);
    sceVif1PkAddGsAD((XglPacket *)packet, 71, 0x73001ULL);
    sceVif1PkAddGsAD((XglPacket *)packet, 8, 0x3fc003fc000ULL);
    sceVif1PkAddGsAD((XglPacket *)packet, 73, 0);
    sceVif1PkAddGsAD((XglPacket *)packet, 66, 0x8000000068ULL);
    sceVif1PkAddGsAD((XglPacket *)packet, 6, (unsigned long long)tex0);
    _closeVifGif((sceVif1Packet *)packet);

    __asm__ __volatile__(
        "lqc2 vf27, 0(%0)\n"
        "lqc2 vf28, 16(%0)\n"
        "lqc2 vf29, 32(%0)\n"
        "lqc2 vf30, 48(%0)\n"
        :
        : "r"(pView)
        : "memory");
    __asm__ __volatile__(
        "lqc2 vf25, 0(%0)\n"
        "lqc2 vf26, 0(%1)\n"
        :
        : "r"((char *)pView + 64), "r"((char *)pView + 80)
        : "memory");
    __asm__ __volatile__(
        "lqc2 vf31, 0(%0)\n"
        "sqc2 vf31, 0(%1)\n"
        :
        : "r"((char *)pView + 64), "r"(&work[4])
        : "memory");
    ((float *)&work[4])[0] *= 0.375f;
    ((float *)&work[4])[1] *= 0.375f;
    XrgUnitVector((float *)&work[3]);
    pStore = &work[5];
    for (done = 0; done < count; done += 32) {
        n = 32;
        rem = count - done;
        if (!(n < rem)) {
            n = rem;
        }
        for (; n-- > 0; pPoints++) {
            Quadword *pDir = &work[3];
            Quadword *pScratch = &work[6];
            Quadword *pScale = &work[7];
            Quadword *pSprite = &work[12];
            Quadword *pConst = &work[2];

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
            if (((float *)&work[6])[2] < depthLimit) {
                int ix;
                int iy;
                int x0;
                int x1;
                Quadword corner;

                ((float *)&work[3])[1] = ((float *)&work[3])[0] = pPoints->vector[3].vec[0] * 0.5f;
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
                ((float *)&work[7])[0] = ((float *)&work[3])[0] * ((float *)&work[4])[0];
                ((float *)&work[7])[1] = ((float *)&work[3])[1] * ((float *)&work[4])[1];
                ((float *)&work[7])[2] = ((float *)&work[3])[2] * ((float *)&work[4])[2];
                ((float *)&work[7])[3] = ((float *)&work[3])[3] * ((float *)&work[4])[3];
                if (*(int *)&pPoints->vector[3].vec[2] != 0) {
                    ((float *)&work[7])[0] *= cosf(pPoints->vector[3].vec[2]);
                }
                if (*(int *)&pPoints->vector[3].vec[3] != 0) {
                    ((float *)&work[7])[1] *= cosf(pPoints->vector[3].vec[3]);
                }
                __asm__ __volatile__(
                    "lqc2 vf11, 0(%0)\n"
                    "vftoi4.xyzw vf11, vf11\n"
                    "sqc2 vf11, 0(%1)\n"
                    :
                    : "r"(pScale), "r"(pScale)
                    : "memory");
                ix = ((int *)&work[7])[0];
                if ((unsigned int)(ix - 1) >= 2047u) {
                    continue;
                }
                ((float *)&work[12])[0] = pPoints->vector[3].vec[1];
                __asm__ __volatile__(
                    "lqc2 vf10, 0(%0)\n"
                    "lqc2 vf11, 0(%1)\n"
                    "vmulx.xyzw vf10, vf10, vf11x\n"
                    "vftoi0.xyzw vf10, vf10\n"
                    "sqc2 vf10, 0(%2)\n"
                    :
                    : "r"(pConst), "r"(pSprite), "r"(pSprite)
                    : "memory");
                corner = work[5];
                ((XrgParticleRow *)&work[8])->quad = corner;
                ((XrgParticleRow *)&work[9])->quad = corner;
                x0 = ((XrgParticleRow *)&work[8])->word[0] - ix;
                x1 = ((XrgParticleRow *)&work[9])->word[0] + ix;
                ((XrgParticleRow *)&work[10])->quad = corner;
                ((XrgParticleRow *)&work[11])->quad = corner;
                ((XrgParticleRow *)&work[8])->word[0] = x0;
                ((XrgParticleRow *)&work[9])->word[0] = x1;
                if (x0 > 0) {
                    x0 = 0x10000;
                    if (x1 < x0) {
                        iy = ((int *)&work[7])[1];
                        ((XrgParticleRow *)&work[10])->word[0] -= ix;
                        ((XrgParticleRow *)&work[8])->word[1] -= iy;
                        ((XrgParticleRow *)&work[9])->word[1] -= iy;
                        ((XrgParticleRow *)&work[11])->word[0] += ix;
                        ((XrgParticleRow *)&work[10])->word[1] += iy;
                        ((XrgParticleRow *)&work[11])->word[1] += iy;
                        _openVifGif((XglPacket *)packet, 340, 9, 0x434343431ULL);
                        sceVif1PkAddGsPacked((XglPacket *)packet, work[12]);
                        sceVif1PkAddGsPacked((XglPacket *)packet, rows[0].quad);
                        sceVif1PkAddGsPacked((XglPacket *)packet, ((XrgParticleRow *)&work[8])->quad);
                        sceVif1PkAddGsPacked((XglPacket *)packet, rows[1].quad);
                        sceVif1PkAddGsPacked((XglPacket *)packet, ((XrgParticleRow *)&work[9])->quad);
                        sceVif1PkAddGsPacked((XglPacket *)packet, rows[2].quad);
                        sceVif1PkAddGsPacked((XglPacket *)packet, ((XrgParticleRow *)&work[10])->quad);
                        sceVif1PkAddGsPacked((XglPacket *)packet, rows[3].quad);
                        sceVif1PkAddGsPacked((XglPacket *)packet, ((XrgParticleRow *)&work[11])->quad);
                        _closeVifGif((sceVif1Packet *)packet);
                    }
                }
            }
        }
        if ((ret = sceVif1PkSize((XglPacket *)packet)) != 0) {
            nmlModelDirectSend(*(void **)((char *)pView + 96), 0x70000000, ret);
            sceVif1PkInit((XglPacket *)packet, 0x70000000);
        }
    }
}

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

/*
 * Partial view of the picture record this TU reads (RgBxxPic, 96 bytes):
 * x, y, width and height at +0x2C..+0x38 and the owning archive back-pointer
 * at +0x58 are the members _DrawDriver loads. rg_help.h and rg_bxx.h hold
 * the owner's view; this tag stays identical to them where they overlap.
 */
struct RgBxxPic {
    unsigned char unmodeled_000[0x2C];
    int x;                          /* +0x2C */
    int y;                          /* +0x30 */
    int width;                      /* +0x34 */
    int height;                     /* +0x38 */
    unsigned char unmodeled_03C[0x58 - 0x3C];
    struct RgBxx *pOwner;           /* +0x58 */
    unsigned char unmodeled_05C[0x60 - 0x5C];
};

/*
 * "pEnv != NIL" and "pView != NIL" sit in the scaffold-owned gap of this
 * overlay's .rodata (tu084 owns 0xa58f78-0xa58fe8 and 0xa59008-0xa59030 only),
 * so they are referenced, not defined, here.
 */
extern const char D_00A58FE8[];
extern const char D_00A58FF8[];
extern RgDrawView *s_pCurView_00A511F4;
extern RgDrawView *RgDrawStudioGetView(RgDrawStudio *pStudio);
extern void *RgBxxGetXtx(struct RgBxx *pBxx);
extern void nmlModelDirectSendXtx(int projectionMode, void *xtx);
extern long long XrgBxxPs2Tex0(void *texture);

/*
 * One 16-byte row of the 4-row local _DrawDriver builds for _Disp: the
 * 128-bit zero or the copied row, read and written as four words.
 */


void _DrawDriver(XrgParticleDriver *driver, void *pStudio)
{
    RgDrawView *pView;
    struct RgBxx *pOwner = 0;
    XrgParticleDispReq *req;
    RgBxxPic *pic;
    XrgParticleRow rows[4];
    long long tex0;
    unsigned int i;
    int dx;
    int dy;
    int py;

    if (driver == 0) {
        assert_prog(D_00A58F20, D_00A58F30, 459);
    }
    if (pStudio == 0) {
        assert_prog(D_00A58FE8, D_00A58F30, 460);
    }
    pView = RgDrawStudioGetView(pStudio);
    if (pView == 0) {
        assert_prog(D_00A58FF8, D_00A58F30, 462);
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

/*
 * One 80-byte particle element CreateArrayOfXrgParticle initialises: five
 * 16-byte vectors at +0x00, +0x10, +0x20, +0x30 and +0x40, the layout _Calc
 * steps with VU0 adds. Creation clears each vector with a 128-bit zero store
 * and then sets the w lane of the first vector (+0x0C) to 1.0f. The tag is
 * completed here, beside the Quadword type its 128-bit members use.
 */
extern RgHeap *InstanceOfRgHeapData(void);


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
                         D_00A58F30, 524);
    if (pArray == 0) {
        assert_prog(D_00A58FA0, D_00A58F30, 526);
    }
    pElement = pArray;
    for (i = 0; i < count; i++) {
        pElement->vector[0].quad = 0;
        pElement->vector[1].quad = 0;
        pElement->vector[2].quad = 0;
        pElement->vector[3].quad = 0;
        pElement->vector[4].quad = 0;
        pElement->vector[0].vec[3] = 1.0f;
        pElement++;
    }
    return pArray;
}

void DisposeArrayOfXrgParticle(XrgParticle *array)
{
    if (array == 0) {
        assert_prog(D_00A59018, D_00A58F30, 543);
    }
    RgHeapFree(InstanceOfRgHeapData(), array, D_00A58F30, 544);
}
