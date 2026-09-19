#ifndef INCLUDE_OV12_RG_DISPMODEL_H
#define INCLUDE_OV12_RG_DISPMODEL_H

#include "shared.h"

typedef struct RgDispModel RgDispModel;

typedef void (*RgDispModelDispFunc)(RgDispModel *pDispModel);

typedef void (*RgDispModelDestructFunc)(RgDispModel *pDispModel);

/*
 * The observed access view: the local four-by-four matrix at 0x00,
 * the mode flag word RgDispModelDisplay tests bit 0x2 of at 0x40, the
 * transparency and blight floats RgDispModelGetTransparent/
 * RgDispModelSetTransparent and RgDispModelSetBlight access at 0x44 and
 * 0x48, and the per-instance display and destruct callbacks
 * RgDispModelDisplay and DisposeRgDispModel call at 0x4C and 0x50.
 * InitRgDispModel writes every one of these offsets; no size or member
 * beyond 0x50 is claimed.
 */
struct RgDispModel {
    RgMatrix local;
    int mode;
    float transparent;
    float blight;
    RgDispModelDispFunc dispMethod;
    RgDispModelDestructFunc destructMethod;
};

#endif /* INCLUDE_OV12_RG_DISPMODEL_H */
