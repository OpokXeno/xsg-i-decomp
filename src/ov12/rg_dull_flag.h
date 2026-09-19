/*
 * TU-local declarations of ov12/tu031 (src/ov12/rg_dull_flag.c).
 */

#ifndef SRC_OV12_RG_DULL_FLAG_H
#define SRC_OV12_RG_DULL_FLAG_H

#include "shared.h"

typedef struct RgDullFlag RgDullFlag;

/*
 * The observed access view: the active suppression countdown at 0x00
 * and the configured suppression duration at 0x04. InitRgDullFlag
 * writes both; no member beyond 0x04 is claimed.
 */
struct RgDullFlag {
    float time;
    float dullTime;
};

void InitRgDullFlag(RgDullFlag *pDull);
void RgDullFlagSetDullTime(RgDullFlag *pDull, float fTime);
void RgDullFlagSetFlag(RgDullFlag *pDull);
void RgDullFlagResetFlag(RgDullFlag *pDull);
int RgDullFlagGet(RgDullFlag *pDull);
void RgDullFlagPassTime(RgDullFlag *pDull, float fTime);

#endif
