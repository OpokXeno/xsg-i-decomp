#ifndef INCLUDE_OV12_RG_DISPMODEL_H
#define INCLUDE_OV12_RG_DISPMODEL_H

#include "shared.h"

typedef struct RgDispModel RgDispModel;

typedef void (*RgDispModelDispFunc)(RgDispModel *pDispModel);

typedef void (*RgDispModelDestructFunc)(RgDispModel *pDispModel);

/* RgDispModel.mode bits. */
#define RG_DISP_MODEL_MODE_FOG 1     /* apply the studio's fog */
#define RG_DISP_MODEL_MODE_HIDDEN 2  /* RgDispModelDisplay draws nothing */
#define RG_DISP_MODEL_MODE_ALPHA 4   /* blend mode 0x48 while translucent */
#define RG_DISP_MODEL_MODE_MAPCLIP 8 /* clip against the map */

/*
 * A displayable model: its local matrix, the mode flags above, the
 * transparency and blight factors, and the per-instance display and destruct
 * callbacks RgDispModelDisplay and DisposeRgDispModel call. No size or
 * member beyond 0x50 is claimed.
 */
struct RgDispModel {
    RgMatrix local;
    int mode;
    float transparent;
    float blight;
    RgDispModelDispFunc dispMethod;
    RgDispModelDestructFunc destructMethod;
};

/* Functions and data used by other translation units. */
void InitRgDispModel(RgDispModel *pDispModel);
void DisposeRgDispModel(RgDispModel *pDispModel);
void RgDispModelDisplay(RgDispModel *pDispModel);
void RgDispModelSetMode(RgDispModel *pDispModel, int mode);
void RgDispModelResetMode(RgDispModel *pDispModel, int mode);
void RgDispModelSetLocal(RgDispModel *pDispModel, const RgMatrix source);
void RgDispModelGetLocal(RgDispModel *pDispModel, RgMatrix destination);
void RgDispModelSetTransparent(RgDispModel *pDispModel, float transparent);
void RgDispModelSetBlight(RgDispModel *pDispModel, float blight);
float RgDispModelGetTransparent(RgDispModel *pDispModel);
RgDispModel *CreateXrgDispModelImpl(void *archive, const char *variant);

#endif /* INCLUDE_OV12_RG_DISPMODEL_H */
