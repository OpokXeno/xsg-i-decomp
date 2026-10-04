/*
 * TU-local declarations of ov12/tu084 (src/ov12/xrg_particle.c).
 */

#ifndef SRC_OV12_XRG_PARTICLE_H
#define SRC_OV12_XRG_PARTICLE_H

#include "shared.h"

/*
 * The particle-effect driver singleton (RgSingletonIDGet id 7,
 * InstanceOfXrgParticleDriver ov12:0x00a498b8). CreateXrgParticleDriver
 * (ov12:0x00a497e0) requests sizeof(XrgParticleDriver) (0x30D0) bytes from
 * RgHeapAlloc, so the struct ends exactly at m_uReqDispNum.
 *
 * Only the offsets this allocation's own functions read or write are named:
 *   params0/1     the two extra arguments XrgParticleDriverSet
 *                 (ov12:0x00a49910) stores here verbatim; _InitDriver
 *                 (ov12:0x00a48d10) clears both to zero. No reader of
 *                 either word is evidenced in this translation unit.
 *   m_uReqCalcNum the pending-calc request count XrgParticleDriverPassTime
 *                 (ov12:0x00a49b50) and XrgParticleDriverPassTimeReq
 *                 (ov12:0x00a49970) index their 16-slot request array with;
 *                 _InitDriver clears it. Name and bound (REQ_CALC_MAX)
 *                 recovered from the original assertion text at
 *                 ov12:0x00a58f78, "pDrv->m_uReqCalcNum < REQ_CALC_MAX".
 *   m_uReqDispNum the pending-display request count
 *                 XrgParticleDriverDispReq (ov12:0x00a49a50) and
 *                 _DrawDriver (ov12:0x00a49be8) index their request array
 *                 with; _InitDriver and _ClearReqDisp (ov12:0x00a49d98)
 *                 clear it. Name and bound (REQ_DISP_MAX) recovered from
 *                 the original assertion text at ov12:0x00a58fb0,
 *                 "pDrv->m_uReqDispNum < REQ_DISP_MAX".
 * The two request arrays themselves (+0x08 and +0xCC) belong to the
 * functions that index them, outside this allocation.
 */
/*
 * One entry of the 16-slot pending-calc request array XrgParticleDriver
 * stores at +0x08: XrgParticleDriverPassTime (ov12:0x00a49b50) reads all
 * three fields of each of the m_uReqCalcNum live entries in order (count,
 * particle, rate) and forwards them unchanged, together with its own
 * elapsed-time argument, to _Calc (ov12:0x00a48da0, outside this
 * allocation), which dereferences the particle pointer as the base of a
 * per-object record.
 */
typedef struct XrgParticleCalcReq {
    int count;                          /* +0x00 */
    struct XrgParticle *particle;       /* +0x04 */
    float rate;                         /* +0x08 */
} XrgParticleCalcReq;

/*
 * RgBxxPic is defined by ov12/tu073 (src/ov12/rg_bxx.c); this allocation
 * only stores and forwards the pointer XrgParticleDriverDispReq receives, so
 * an opaque forward declaration is enough here.
 */
typedef struct RgBxxPic RgBxxPic;

/*
 * One entry of the 1024-slot pending-display request array XrgParticleDriver
 * stores at +0xCC: XrgParticleDriverDispReq (ov12:0x00a49a50) stores its own
 * three arguments here in order (count, points, pic) and _DrawDriver
 * (ov12:0x00a49be8) reads pPoints/pPic of each of the m_uReqDispNum live
 * entries back. count is stored but not read by any function of this
 * allocation. pPoints is asserted non-null with the same "pPoints != NIL"
 * text XrgParticleDriverPassTimeReq uses for its own particle-array
 * argument; pPic is asserted non-null with the original text at
 * ov12:0x00a58fd8, "pPic != NIL".
 */
typedef struct XrgParticleDispReq {
    int count;                          /* +0x00 */
    struct XrgParticle *pPoints;        /* +0x04 */
    RgBxxPic *pPic;                     /* +0x08 */
} XrgParticleDispReq;

typedef struct XrgParticleDriver {
    int params0;                        /* +0x00 */
    int params1;                        /* +0x04 */
    XrgParticleCalcReq calcReq[16];     /* +0x08 */
    unsigned int m_uReqCalcNum;         /* +0xC8 */
    XrgParticleDispReq dispReq[1024];   /* +0xCC */
    unsigned int m_uReqDispNum;         /* +0x30CC */
} XrgParticleDriver;

/*
 * Returns the registered singleton (RgSingletonIDGet id 7), creating and
 * registering it through CreateXrgParticleDriver and RgSingletonIDEntry when
 * absent.
 */
XrgParticleDriver *InstanceOfXrgParticleDriver(void);

/*
 * Asserts driver non-null (ov12:0x00a49840), runs _DisposeDriver on it and
 * returns its block to RgHeapFree.
 */
void DisposeXrgParticleDriver(XrgParticleDriver *driver);

/*
 * Appends one three-word pass-time request (points, count, rate) to the
 * driver's bounded REQ_CALC_MAX-entry queue.
 */
void XrgParticleDriverPassTimeReq(XrgParticleDriver *driver, struct XrgParticle *pPoints,
                                  int count, float rate);

/*
 * Runs _Calc (ov12:0x00a48da0, outside this allocation) for every queued
 * calc request and resets m_uReqCalcNum to 0.
 */
void XrgParticleDriverPassTime(XrgParticleDriver *pDrv, float elapsed);

/*
 * Appends one three-word display request (points, count, pic) to the
 * driver's bounded REQ_DISP_MAX-entry queue.
 */
void XrgParticleDriverDispReq(XrgParticleDriver *driver, struct XrgParticle *pPoints,
                              int count, RgBxxPic *pPic);

/*
 * Queues the driver's own draw and clear callbacks with RgDrawReq
 * (ov12:0x00a28028, outside this allocation).
 */
void XrgParticleDriverDisp(XrgParticleDriver *driver);

/*
 * DisposeArrayOfXrgParticle (ov12:0x00a49f08) only frees the block this
 * points at; the element layout is outside this allocation.
 */
typedef struct XrgParticle XrgParticle;

/*
 * The original literals at ov12:0x00a58f20 ("pDrv != NIL"),
 * ov12:0x00a58f30 ("../xrg_particle.euc.c") and ov12:0x00a59018
 * ("pArray != NIL") are scaffold-owned (config/tu-build.json
 * data_ownership: this .rodata window is still owner "asm") and have no
 * entry in config/symbols/ov12.txt, so they keep their splat names
 * (docs/naming.md, "Scaffold-owned data keeps its splat name").
 */

#endif /* SRC_OV12_XRG_PARTICLE_H */
