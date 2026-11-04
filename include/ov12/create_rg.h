#ifndef INCLUDE_OV12_CREATE_RG_H
#define INCLUDE_OV12_CREATE_RG_H

#include "shared.h"

/* Functions and data used by other translation units. */
void CreateRgHitEffect(RgMatrix local, const char *name1, const char *name2);
void CreateRgHitEffectPos(RgVector position, const char *name1, const char *name2);
void CreateRgHitEffectPosDir(RgVector position, RgVector direction,
                             const char *name1, const char *name2);

#endif /* INCLUDE_OV12_CREATE_RG_H */
