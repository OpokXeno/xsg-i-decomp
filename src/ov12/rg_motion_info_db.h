/*
 * TU-local declarations of ov12/tu013 (src/ov12/rg_motion_info_db.c).
 */

#ifndef SRC_OV12_RG_MOTION_INFO_DB_H
#define SRC_OV12_RG_MOTION_INFO_DB_H

#include "shared.h"

typedef struct RgMotionShotInfo RgMotionShotInfo;

/*
 * A per-motion table entry of the "rg_motion.info" text RgMotionInfoDBLoad
 * parses.  _TableSort and _TableGet keep the table sorted by motionNo;
 * _TableGetMatchChar walks the same-motionNo entries through next and returns
 * the one whose charId equals the requested character, else the one whose
 * charId is -1 (an entry for any character).  The loader's keywords evidence
 * the other members: "char" stores RgActorNameToCharID's result in charId,
 * "shot-body" and "shot-notfix" set bits 0 and 1 of shotFlags, and
 * _AddActionRgMotion asserts "pInfo->m_uActTblSiz < RG_MOTINFO_ACT_TABLE_SIZE"
 * on actionCount before filling the action array at +0x10.
 */
struct RgMotionShotInfo
{
    int motionNo;                      /* +0x00: table sort/search key */
    int charId;                        /* +0x04: character this entry is for; -1 = any */
    unsigned int shotFlags;            /* +0x08: bit 0 "shot-body", bit 1 "shot-notfix" */
    int actionCount;                   /* +0x0C: actions stored so far (m_uActTblSiz, limit 4) */
    unsigned char unmodeled_010[0x50]; /* +0x10..+0x5F: the action array, filled outside this allocation */
    RgMotionShotInfo *next;            /* +0x60: same-motionNo chain walked by _TableGetMatchChar; NULL-terminated */
};

typedef struct RgMotionInfoDB RgMotionInfoDB;

/*
 * The motion-info singleton _InitDB allocates at 0x280 bytes.  The sorted
 * entry-pointer table and its count belong to the _TableXxx helpers outside
 * this allocation; only the embedded default entry is evidenced here.
 */
struct RgMotionInfoDB
{
    unsigned char unmodeled_000[0x200]; /* +0x000..+0x1FF: the sorted entry-pointer table */
    int entryCount;                     /* +0x200: m_uTblSize, cleared by _TableInit */
    unsigned char unmodeled_204[0xC];   /* +0x204..+0x20F: untouched by this allocation */
    RgMotionShotInfo defaultShotInfo;   /* +0x210: fallback RgMotionInfoDBGet returns when no match is found */
};

RgMotionInfoDB *InstanceOfRgMotionInfoDB(void);
void RgMotionInfoDBDispose(RgMotionInfoDB *db);
RgMotionShotInfo *RgMotionInfoDBGet(RgMotionInfoDB *db, int motionNo,
                                    int charId);
int RgMotionInfoDBIsDefaultData(RgMotionInfoDB *db, int motionNo,
                                int charId);

#endif /* SRC_OV12_RG_MOTION_INFO_DB_H */
