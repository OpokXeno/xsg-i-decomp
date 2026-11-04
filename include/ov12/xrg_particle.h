#ifndef INCLUDE_OV12_XRG_PARTICLE_H
#define INCLUDE_OV12_XRG_PARTICLE_H

#include "shared.h"
#include "ov12/rg_bxx.h"

/* One 16-byte row, read as a quadword, four words or four floats. */
typedef union XrgParticleRow {
    Quadword quad;
    int word[4];
    RgVector vec;
} XrgParticleRow;

/*
 * One 80-byte particle. _Calc integrates it every frame: velocity += acceleration * dt,
 * position += velocity * dt and appearance += appearanceRate * dt, then clamps the
 * appearance. CreateArrayOfXrgParticle clears every row and sets the position's
 * w lane to 1.0f.
 */
typedef struct XrgParticle {
    XrgParticleRow position;
    XrgParticleRow velocity;
    XrgParticleRow acceleration;
    /* [0] size (0..100), [1] intensity (0..1), [2] and [3] angles in -pi..pi. */
    XrgParticleRow appearance;
    XrgParticleRow appearanceRate;
} XrgParticle;

/* One entry of the pending-calc request array. */
typedef struct XrgParticleCalcReq {
    int count;
    XrgParticle *particle;
    float rate;
} XrgParticleCalcReq;

/* One entry of the pending-display request array. */
typedef struct XrgParticleDispReq {
    int count;
    XrgParticle *pPoints;
    struct RgBxxPic *pPic;
} XrgParticleDispReq;

#define REQ_CALC_MAX 16
#define REQ_DISP_MAX 1024

/* The particle driver singleton (RG_SINGLETON_PARTICLE_DRIVER), 0x30D0 bytes. */
typedef struct XrgParticleDriver {
    int params0;                          /* +0x00 */
    int params1;                          /* +0x04 */
    XrgParticleCalcReq calcReq[REQ_CALC_MAX];   /* +0x08 */
    unsigned int m_uReqCalcNum;           /* +0xC8 */
    XrgParticleDispReq dispReq[REQ_DISP_MAX];   /* +0xCC */
    unsigned int m_uReqDispNum;           /* +0x30CC */
} XrgParticleDriver;

/* Functions and data used by other translation units. */
XrgParticleDriver *InstanceOfXrgParticleDriver(void);
void DisposeXrgParticleDriver(XrgParticleDriver *driver);
void XrgParticleDriverSet(XrgParticleDriver *driver, int params0, int params1);
void XrgParticleDriverPassTimeReq(XrgParticleDriver *driver,
                                  XrgParticle *pPoints, int count, float rate);
void XrgParticleDriverPassTime(XrgParticleDriver *pDrv, float elapsed);
void XrgParticleDriverDispReq(XrgParticleDriver *driver, XrgParticle *pPoints,
                              int count, struct RgBxxPic *pPic);
void XrgParticleDriverDisp(XrgParticleDriver *driver);
XrgParticle *CreateArrayOfXrgParticle(int count);
void DisposeArrayOfXrgParticle(XrgParticle *array);

#endif /* INCLUDE_OV12_XRG_PARTICLE_H */
