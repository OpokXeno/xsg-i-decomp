#ifndef INCLUDE_OV12_RG_MOTION_INFO_DB_H
#define INCLUDE_OV12_RG_MOTION_INFO_DB_H

#include "shared.h"

/* Actions one motion entry stores (assert "RG_MOTINFO_ACT_TABLE_SIZE"). */
#define RG_MOTINFO_ACT_TABLE_SIZE 4

/* Entries of the sorted motion table (assert "ENTRY_MAX"). */
#define RG_MOTINFO_ENTRY_MAX 0x80

/* RgMotionAction.actionNo: the loader keyword each action comes from. */
#define RG_MOTION_ACTION_ACTION 0    /* "action" and "action-f" */
#define RG_MOTION_ACTION_HIT_SHIFT 1 /* "hit-shift-f" */
#define RG_MOTION_ACTION_NEXT 2      /* "next" */
#define RG_MOTION_ACTION_EVENT 3     /* "event" */

/* RgMotionShotInfo.shotFlags bits. */
#define RG_MOTION_SHOT_BODY 1   /* "shot-body" */
#define RG_MOTION_SHOT_NOTFIX 2 /* "shot-notfix" */

/* The end time of an action that lasts the whole motion. */
#define RG_MOTION_ANY_TIME 100000000.0f

typedef struct RgMotionShotInfo RgMotionShotInfo;
typedef struct RgMotionInfoDB RgMotionInfoDB;

typedef struct RgMotionAction
{
    int actionNo;
    float startTime;
    float endTime;
    int shiftMotionNo;
    int weaponMotionNo;
} RgMotionAction;

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
    unsigned int shotFlags;            /* +0x08: RG_MOTION_SHOT_* bits */
    int actionCount;                   /* +0x0C: actions stored so far (m_uActTblSiz) */
    RgMotionAction actions[RG_MOTINFO_ACT_TABLE_SIZE]; /* +0x10..+0x5F: four 0x14-byte actions */
    RgMotionShotInfo *next;            /* +0x60: same-motionNo chain walked by _TableGetMatchChar; NULL-terminated */
};

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
    RgMotionShotInfo *table[RG_MOTINFO_ENTRY_MAX];
    int entryCount;                     /* +0x200: m_uTblSize, cleared by _TableInit */
    unsigned char unmodeled_204[0xC];   /* +0x204..+0x20F: untouched by this allocation */
    RgMotionShotInfo defaultShotInfo;   /* +0x210: fallback RgMotionInfoDBGet returns when no match is found */
};

/* Functions and data used by other translation units. */
RgMotionInfoDB *InstanceOfRgMotionInfoDB(void);
void RgMotionInfoDBLoad(RgMotionInfoDB *db);
void RgMotionInfoDBDispose(RgMotionInfoDB *db);
RgMotionShotInfo *RgMotionInfoDBGet(RgMotionInfoDB *db, int motionNo,
                                    int charId);
int RgMotionInfoDBIsDefaultData(RgMotionInfoDB *db, int motionNo, int charId);
RgMotionAction *RgMotionInfoGetActionInTime(RgMotionShotInfo *info,
                                            int actionNo, float time);
int RgMotionInfoIsBefore(RgMotionShotInfo *info, int actionNo, float time);
int RgMotionInfoIsAfter(RgMotionShotInfo *info, int actionNo, float time);
void RgMotionInfoDBDump(RgMotionInfoDB *db);

#endif /* INCLUDE_OV12_RG_MOTION_INFO_DB_H */
