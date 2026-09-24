#ifndef INCLUDE_OV12_RG_MOTION_INFO_DB_H
#define INCLUDE_OV12_RG_MOTION_INFO_DB_H

typedef struct RgMotionShotInfo RgMotionShotInfo;

/*
 * The motion-info singleton _InitDB allocates at 0x280 bytes.  The sorted
 * entry-pointer table and its count belong to the _TableXxx helpers outside
 * this allocation; only the embedded default entry is evidenced here.
 */
struct RgMotionInfoDB
{
    /*
     * +0x000..+0x1FF: the sorted entry-pointer table, indexed 0..entryCount-1;
     * each slot heads a next-chain of same-index RgMotionShotInfo entries
     * (_TableFree walks and frees them).
     */
    RgMotionShotInfo *table[0x80];
    int entryCount;                     /* +0x200: m_uTblSize, cleared by _TableInit */
    unsigned char unmodeled_204[0xC];   /* +0x204..+0x20F: untouched by this allocation */
    RgMotionShotInfo defaultShotInfo;   /* +0x210: fallback RgMotionInfoDBGet returns when no match is found */
};

#endif /* INCLUDE_OV12_RG_MOTION_INFO_DB_H */
