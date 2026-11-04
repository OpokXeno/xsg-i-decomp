#ifndef INCLUDE_OV12_RG_SHOT_EFFECT_H
#define INCLUDE_OV12_RG_SHOT_EFFECT_H

#include "shared.h"
#include "ov12/rg_dispmodel.h"
#include "ov12/rg_particle_effect.h"

/*
 * The visual of one shot: its model, its particle effect and a textured
 * line between the two end points. CreateRgShotEffect allocates sizeof() 0xA0.
 */
typedef struct RgShotEffect {
    RgDispModel *dispModel;              /* 0x00 */
    float scale;                         /* 0x04 */
    float yRotation;                     /* 0x08 */
    float transparent;                   /* 0x0C */
    RgParticleEffect *particleEffect;    /* 0x10 */
    int particleShootReverse;            /* 0x14 */
    struct RgBxxPic *texLinePic;         /* 0x18 */
    float texLineModelWidth;             /* 0x1C */
    float texLineModelHeight;            /* 0x20 */
    unsigned char unmodeled_24[0xC];     /* 0x24 */
    XrgPaint2DRGBA texLineModelColor;    /* 0x30 */
    RgMatrix matrix;                     /* 0x40 */
    RgVector startPos;                   /* 0x80 */
    RgVector endPos;                     /* 0x90 */
} RgShotEffect;

/* Functions and data used by other translation units. */
RgShotEffect *CreateRgShotEffect(void);
void DisposeRgShotEffect(RgShotEffect *pShotEff);
void RgShotEffectStopAlive(RgShotEffect *pEff, float time);
void RgShotEffectSetModel(RgShotEffect *pEff, const char *variant);
void RgShotEffectSetParticle(RgShotEffect *pEff, char *name);
void RgShotEffectSetParticleShootReverse(RgShotEffect *pEff);
RgParticleEffect *RgShotEffectGetPtclEffect(RgShotEffect *pEff);
void RgShotEffectSetTexLine(RgShotEffect *pEff, const char *name);
void RgShotEffectSetTexLineModelSize(RgShotEffect *pEff, float width,
                                     float height);
void RgShotEffectSetTexLineModelColor(RgShotEffect *pEff, const XrgPaint2DRGBA *color);
void RgShotEffectSetPos(RgShotEffect *pEff, RgVector startPos,
                        RgVector endPos);
void RgShotEffectSetScale(RgShotEffect *pEff, float scale);
void RgShotEffectSetYRot(RgShotEffect *pEff, float yRotation);
void RgShotEffectSetTransparent(RgShotEffect *pEff, float transparent);
void RgShotEffectSetAddAlpha(RgShotEffect *pEff);
void RgShotEffectPassTime(RgShotEffect *pEff, float time);
void RgShotEffectDisp(RgShotEffect *pEff);

#endif /* INCLUDE_OV12_RG_SHOT_EFFECT_H */
