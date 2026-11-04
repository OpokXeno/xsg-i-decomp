#ifndef INCLUDE_OV12_RG_DULL_FLAG_H
#define INCLUDE_OV12_RG_DULL_FLAG_H

/*
 * A suppression countdown: `time` is the active remaining time, `dullTime`
 * the duration RgDullFlagSetFlag reloads it with.
 */
typedef struct RgDullFlag {
    float time;     /* +0x00 */
    float dullTime; /* +0x04 */
} RgDullFlag;

/* Functions and data used by other translation units. */
void InitRgDullFlag(RgDullFlag *pDull);
void RgDullFlagSetDullTime(RgDullFlag *pDull, float fTime);
void RgDullFlagSetFlag(RgDullFlag *pDull);
void RgDullFlagResetFlag(RgDullFlag *pDull);
int RgDullFlagGet(RgDullFlag *pDull);
void RgDullFlagPassTime(RgDullFlag *pDull, float fTime);

#endif /* INCLUDE_OV12_RG_DULL_FLAG_H */
