/*
 * OV12 original TU 13: 0x00a0f490..0x00a10318 (20 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_actor_charid.h"
#include "ov12/rg_actor_motid.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_motion_info_db.h"
#include "ov12/rg_read_text.h"
#include "ov12/rg_singleton_id.h"
#include "ov12/xrg_v_log.h"

/* libgcc float-to-double helper (shared.h cannot declare it: main/matrix_print.c uses another result type). */
extern double fptodp(float value);

/* Bytes of the singleton's allocation, and of the loader's token buffer. */
#define RG_MOTION_INFO_DB_ALLOC_BYTES 0x280
#define RG_MOTINFO_TOKEN_SIZE 0x80

/* Motion frames per second: "action-f" and "hit-shift-f" times are frames. */
#define RG_MOTION_FRAME_RATE 30.0f

/* Defaults of the action InitDB seeds RgMotionInfoDB.defaultShotInfo with. */
#define RG_MOTION_DEFAULT_START 0.1f
#define RG_MOTION_DEFAULT_END 0.2f


/* Scaffold-owned data (splat names): the text file name and the keywords. */
extern const char motionInfoDbFileName[];
extern const char motionInfoDbActionToken[];
extern const char motionInfoDbAssertStartTimeBeforeEndTime[];
extern const char motionInfoDbActionFrameToken[];
extern const char motionInfoDbAssertStartBeforeEnd[];
extern const char motionInfoDbNextToken[];
extern const char motionInfoDbAssertActNotNil[];
extern const char motionInfoDbEventToken[];
extern const char motionInfoDbHitShiftFrameToken[];
extern const char motionInfoDbShotBodyToken[];
extern const char motionInfoDbShotNotFixToken[];
extern const char motionInfoDbCharToken[];

static void _TableFree(RgMotionInfoDB *db);
static void _InitDB(RgMotionInfoDB *db);
static void _WrapperDestruct(void *db);
RgMotionShotInfo *_TableGetMatchChar(RgMotionInfoDB *db, int motionNo,
                                     int charId);

const char motionInfoDbAssertInfoNotNil[16] = "pInfo != NIL";

const char motionInfoDbSourcePath[32] = "../rg_motion_info_db.euc.c";

const char motionInfoDbAssertActTableSizeBelowMax[48] = "pInfo->m_uActTblSiz < RG_MOTINFO_ACT_TABLE_SIZE";

const char motionInfoDbAssertTableSizeBelowEntryMax[32] = "pTable->m_uTblSize < ENTRY_MAX";

const char motionInfoDbAssertDbNotNil[16] = "pDB != NIL";

const char motionInfoDbAssertMotInfoNotNil[16] = "pMotInfo != NIL";

const char motionInfoDbDumpHeaderFormat[48] = "dump shot info %d ----------------------\n";

const char motionInfoDbDumpActionFormat[24] = "--- list mot %d ---\n";

const char motionInfoDbDumpCharFormat[24] = "  ** char %d **\n";

const char motionInfoDbDumpMotionIdFormat[16] = "   mot id = %d\n";

const char motionInfoDbDumpMotionFlagFormat[24] = "   mot flag = %x\n";

const char motionInfoDbDumpActionEntryFormat[48] = "    [action %d (%f,%f) shift-mot %d wep %d]\n";

static void _InitRgMotionShotInfo(RgMotionShotInfo *info, int motionNo)
{
    if (info == 0) {
        assert_prog(motionInfoDbAssertInfoNotNil, motionInfoDbSourcePath, 51);
    }
    info->motionNo = motionNo;
    info->next = 0;
    info->actionCount = info->shotFlags = 0;
    info->charId = -1;
}

static RgMotionAction *_AddActionRgMotion(RgMotionShotInfo *info, int actionNo,
                                          float startTime, float endTime)
{
    RgMotionAction *action;

    if (!((unsigned int)info->actionCount < RG_MOTINFO_ACT_TABLE_SIZE)) {
        assert_prog(motionInfoDbAssertActTableSizeBelowMax, motionInfoDbSourcePath, 65);
    }
    action = &info->actions[info->actionCount++];
    action->actionNo = actionNo;
    action->startTime = startTime;
    action->endTime = endTime;
    action->shiftMotionNo = -1;
    return action;
}

static void _TableInit(RgMotionInfoDB *db)
{
    db->entryCount = 0;
}

static void _TableSort(RgMotionInfoDB *db)
{
    unsigned int count;
    unsigned int first;
    unsigned int second;
    RgMotionShotInfo *entry;
    RgMotionShotInfo **table;

    count = db->entryCount;
    table = db->table;
    for (first = 0; first < count; first++) {
        for (second = first + 1; second < count; second++) {
            if (table[second]->motionNo < table[first]->motionNo) {
                entry = table[second];
                table[second] = table[first];
                table[first] = entry;
            }
        }
    }
}

RgMotionShotInfo *_TableGet(RgMotionInfoDB *db, int motionNo)
{
    unsigned int count;
    int low;
    int high;
    int middle;
    RgMotionShotInfo *entry;
    RgMotionShotInfo **table;

    count = db->entryCount;
    if (count == 0) {
        return 0;
    }
    low = 0;
    high = count - 1;
    table = db->table;
    do {
        middle = (low + high) / 2;
        entry = table[middle];
        if (entry->motionNo == motionNo) {
            return entry;
        }
        if (motionNo < entry->motionNo) {
            high = middle - 1;
        } else {
            low = middle + 1;
        }
    } while (high >= low && (unsigned int)low < count && high >= 0);
    return 0;
}

RgMotionShotInfo *_TableGetMatchChar(RgMotionInfoDB *db, int motionNo,
                                     int charId)
{
    RgMotionShotInfo *info;
    RgMotionShotInfo *defaultInfo;

    info = _TableGet(db, motionNo);
    defaultInfo = info;
    if (info != 0 && info->charId != charId) {
        do {
            if (info->charId == -1) {
                defaultInfo = info;
            }
            info = info->next;
        } while (info != 0 && info->charId != charId);
    }
    return info != 0 ? info : defaultInfo;
}

static void _TableEntry(RgMotionInfoDB *db, RgMotionShotInfo *entry)
{
    if ((unsigned int)db->entryCount >= RG_MOTINFO_ENTRY_MAX) {
        assert_prog(motionInfoDbAssertTableSizeBelowEntryMax, motionInfoDbSourcePath, 176);
    }
    db->table[db->entryCount++] = entry;
}

static void _TableFree(RgMotionInfoDB *db)
{
    u32 i;
    RgMotionShotInfo *info;
    RgMotionShotInfo *next;

    for (i = 0; i < (u32) db->entryCount; i++) {
        info = db->table[i];
        while (info != 0) {
            next = info->next;
            RgHeapFree(InstanceOfRgHeap(), info, motionInfoDbSourcePath, 189);
            info = next;
        }
    }
    db->entryCount = 0;
}

static void _InitDB(RgMotionInfoDB *db) {
    RgMotionShotInfo *info;

    info = &db->defaultShotInfo;
    if (db == 0) {
        assert_prog(motionInfoDbAssertDbNotNil, motionInfoDbSourcePath, 200);
    }
    _TableInit(db);
    _InitRgMotionShotInfo(info, -1);
    _AddActionRgMotion(info, RG_MOTION_ACTION_ACTION, RG_MOTION_DEFAULT_START,
                        RG_MOTION_DEFAULT_END);
}

static void _DestructDB(RgMotionInfoDB *db)
{
    if (db == 0) {
        assert_prog(motionInfoDbAssertDbNotNil, motionInfoDbSourcePath, 211);
    }
    _TableFree(db);
}

static void _WrapperDestruct(void *db) {
    _DestructDB(db);
    RgHeapFree(InstanceOfRgHeap(), db, motionInfoDbSourcePath, 215);
}

RgMotionInfoDB *InstanceOfRgMotionInfoDB(void)
{
    RgMotionInfoDB *db;

    db = RgSingletonIDGet(RG_SINGLETON_MOTION_INFO_DB);
    if (db == 0) {
        db = RgHeapAlloc(InstanceOfRgHeap(), RG_MOTION_INFO_DB_ALLOC_BYTES, motionInfoDbSourcePath, 225);
        _InitDB(db);
        RgSingletonIDEntry(RG_SINGLETON_MOTION_INFO_DB, db, _WrapperDestruct);
    }
    return db;
}

void RgMotionInfoDBLoad(RgMotionInfoDB *db)
{
    RgReadText *reader;
    RgMotionShotInfo *info;
    RgMotionShotInfo *existing;
    RgMotionAction *action;
    char szToken[RG_MOTINFO_TOKEN_SIZE];
    int motionNo;

    if (db == 0) {
        assert_prog(motionInfoDbAssertDbNotNil, motionInfoDbSourcePath, 239);
    }
    reader = CreateRgReadText(motionInfoDbFileName);
    if (reader != 0) {
        while (RgReadTextIsEOF(reader) == 0) {
            RgReadTextGetString(reader, szToken);
            motionNo = RgActorMotIDGetFromName(szToken);
            info = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgMotionShotInfo), motionInfoDbSourcePath, 258);
            _InitRgMotionShotInfo(info, motionNo);

            while (RgReadTextIsEOF(reader) == 0) {
                RgReadTextGetString(reader, szToken);
                if (strcasecmp(szToken, motionInfoDbActionToken) == 0) {
                    float startTime;
                    float endTime;

                    startTime = RgReadTextGetFloat(reader);
                    endTime = RgReadTextGetFloat(reader);
                    if (!(startTime <= endTime)) {
                        assert_prog(motionInfoDbAssertStartTimeBeforeEndTime, motionInfoDbSourcePath, 271);
                    }
                    _AddActionRgMotion(info, RG_MOTION_ACTION_ACTION, startTime, endTime);
                } else if (strcasecmp(szToken, motionInfoDbActionFrameToken) == 0) {
                    float startTime;
                    float endTime;
                    float frameEnd;
                    float frameStart;

                    startTime = RgReadTextGetFloat(reader);
                    endTime = RgReadTextGetFloat(reader);
                    frameStart = startTime / RG_MOTION_FRAME_RATE;
                    frameEnd = endTime / RG_MOTION_FRAME_RATE;
                    if (!(startTime <= endTime)) {
                        assert_prog(motionInfoDbAssertStartBeforeEnd, motionInfoDbSourcePath, 280);
                    }
                    _AddActionRgMotion(info, RG_MOTION_ACTION_ACTION, frameStart, frameEnd);
                } else if (strcasecmp(szToken, motionInfoDbNextToken) == 0) {
                    action = _AddActionRgMotion(info, RG_MOTION_ACTION_NEXT, 0.0f,
                                                RG_MOTION_ANY_TIME);
                    if (action == 0) {
                        assert_prog(motionInfoDbAssertActNotNil, motionInfoDbSourcePath, 286);
                    }
                    RgReadTextGetString(reader, szToken);
                    action->shiftMotionNo = RgActorMotIDGetFromName(szToken);
                    RgReadTextGetString(reader, szToken);
                    action->weaponMotionNo = RgActorMotIDGetFromName(szToken);
                } else if (strcasecmp(szToken, motionInfoDbEventToken) == 0) {
                    action = _AddActionRgMotion(info, RG_MOTION_ACTION_EVENT, 0.0f,
                                                RG_MOTION_ANY_TIME);
                    if (action == 0) {
                        assert_prog(motionInfoDbAssertActNotNil, motionInfoDbSourcePath, 296);
                    }
                    RgReadTextGetString(reader, szToken);
                    action->shiftMotionNo = RgActorMotIDGetFromName(szToken);
                    RgReadTextGetString(reader, szToken);
                    action->weaponMotionNo = RgActorMotIDGetFromName(szToken);
                } else if (strcasecmp(szToken, motionInfoDbHitShiftFrameToken) == 0) {
                    float startTime;
                    float endTime;

                    startTime = RgReadTextGetFloat(reader);
                    endTime = RgReadTextGetFloat(reader);
                    action = _AddActionRgMotion(info, RG_MOTION_ACTION_HIT_SHIFT,
                                                startTime / RG_MOTION_FRAME_RATE,
                                                endTime / RG_MOTION_FRAME_RATE);
                    if (action == 0) {
                        assert_prog(motionInfoDbAssertActNotNil, motionInfoDbSourcePath, 311);
                    }
                    if (!(startTime <= endTime)) {
                        assert_prog(motionInfoDbAssertStartBeforeEnd, motionInfoDbSourcePath, 312);
                    }
                    RgReadTextGetString(reader, szToken);
                    action->shiftMotionNo = RgActorMotIDGetFromName(szToken);
                    RgReadTextGetString(reader, szToken);
                    action->weaponMotionNo = RgActorMotIDGetFromName(szToken);
                } else if (strcasecmp(szToken, motionInfoDbShotBodyToken) == 0) {
                    info->shotFlags |= RG_MOTION_SHOT_BODY;
                } else if (strcasecmp(szToken, motionInfoDbShotNotFixToken) == 0) {
                    info->shotFlags |= RG_MOTION_SHOT_NOTFIX;
                } else if (strcasecmp(szToken, motionInfoDbCharToken) == 0) {
                    RgReadTextGetString(reader, szToken);
                    info->charId = RgActorNameToCharID(szToken);
                } else {
                    RgReadTextUnget(reader, szToken);
                    break;
                }
            }

            existing = _TableGet(db, info->motionNo);
            if (existing != 0) {
                info->next = existing->next;
                existing->next = info;
            } else {
                _TableEntry(db, info);
                _TableSort(db);
            }
        }
        DisposeRgReadText(reader);
    }
}

void RgMotionInfoDBDispose(RgMotionInfoDB *db)
{
    if (db == 0) {
        assert_prog(motionInfoDbAssertDbNotNil, motionInfoDbSourcePath, 365);
    }
}

RgMotionShotInfo *RgMotionInfoDBGet(RgMotionInfoDB *db, int motionNo,
                                    int charId)
{
    RgMotionShotInfo *info;

    if (db == 0) {
        assert_prog(motionInfoDbAssertDbNotNil, motionInfoDbSourcePath, 374);
    }
    info = _TableGetMatchChar(db, motionNo, charId);
    if (info == 0) {
        return &db->defaultShotInfo;
    }
    return info;
}

int RgMotionInfoDBIsDefaultData(RgMotionInfoDB *db, int motionNo, int charId)
{
    if (db == 0) {
        assert_prog(motionInfoDbAssertDbNotNil, motionInfoDbSourcePath, 388);
    }
    return _TableGetMatchChar(db, motionNo, charId) == 0;
}

RgMotionAction *RgMotionInfoGetActionInTime(RgMotionShotInfo *info,
                                             int actionNo, float time)
{
    RgMotionAction *action;
    unsigned int i;

    if (info == 0) {
        assert_prog(motionInfoDbAssertMotInfoNotNil, motionInfoDbSourcePath, 405);
    }
    for (i = 0; i < (unsigned int)info->actionCount; i++) {
        action = &info->actions[i];
        if (action->actionNo == actionNo) {
            if (time == RG_MOTION_ANY_TIME) {
                return action;
            }
            if (action->startTime <= time && time <= action->endTime) {
                return action;
            }
        }
    }
    return 0;
}

int RgMotionInfoIsBefore(RgMotionShotInfo *info, int actionNo, float time)
{
    RgMotionAction *action;
    unsigned int i;

    if (info == 0) {
        assert_prog(motionInfoDbAssertMotInfoNotNil, motionInfoDbSourcePath, 425);
    }
    for (i = 0; i < (unsigned int)info->actionCount; i++) {
        action = &info->actions[i];
        if (action->actionNo == actionNo && action->startTime < time) {
            return 0;
        }
    }
    return 1;
}

int RgMotionInfoIsAfter(RgMotionShotInfo *info, int actionNo, float time)
{
    RgMotionAction *action;
    unsigned int i;

    if (info == 0) {
        assert_prog(motionInfoDbAssertMotInfoNotNil, motionInfoDbSourcePath, 442);
    }
    for (i = 0; i < (unsigned int)info->actionCount; i++) {
        action = &info->actions[i];
        if (action->actionNo == actionNo && time < action->endTime) {
            return 0;
        }
    }
    return 1;
}

void RgMotionInfoDBDump(RgMotionInfoDB *db)
{
    unsigned int tableIndex;
    unsigned int actionIndex;
    int actionNo;
    int shiftedMotionNo;
    int weaponMotionNo;
    double startSeconds;
    double endSeconds;
    RgMotionShotInfo *info;
    RgMotionShotInfo *current;

    if (db == 0) {
        assert_prog(motionInfoDbAssertDbNotNil, motionInfoDbSourcePath, 459);
    }
    XrgLog(motionInfoDbDumpHeaderFormat, motionInfoDbSourcePath, 461, db->entryCount);
    for (tableIndex = 0; tableIndex < (unsigned int) db->entryCount;
         tableIndex++) {
        current = db->table[tableIndex];
        actionNo = current->motionNo;
        XrgLog(motionInfoDbDumpActionFormat, motionInfoDbSourcePath, 465, actionNo);
        info = current;
        for (; info != 0; info = info->next) {
            XrgLog(motionInfoDbDumpCharFormat, motionInfoDbSourcePath, 469, info->charId);
            XrgLog(motionInfoDbDumpMotionIdFormat, motionInfoDbSourcePath, 470, info->motionNo);
            XrgLog(motionInfoDbDumpMotionFlagFormat, motionInfoDbSourcePath, 471, info->shotFlags);
            for (actionIndex = 0;
                 actionIndex < (unsigned int) info->actionCount;
                 actionIndex++) {
                actionNo = info->actions[actionIndex].actionNo;
                startSeconds = fptodp(info->actions[actionIndex].startTime);
                endSeconds = fptodp(info->actions[actionIndex].endTime);
                shiftedMotionNo = info->actions[actionIndex].shiftMotionNo;
                weaponMotionNo = info->actions[actionIndex].weaponMotionNo;
                XrgLog(motionInfoDbDumpActionEntryFormat, motionInfoDbSourcePath, 473, actionNo,
                       startSeconds, endSeconds, shiftedMotionNo,
                       weaponMotionNo);
            }
        }
    }
}



const char motionInfoDbFileName[16] = "rg_motion.info";

const char motionInfoDbActionToken[8] = "action";

const char motionInfoDbAssertStartTimeBeforeEndTime[24] = "fStartTime <= fEndTime";

const char motionInfoDbActionFrameToken[16] = "action-f";

const char motionInfoDbAssertStartBeforeEnd[16] = "fStart <= fEnd";

const char motionInfoDbNextToken[8] = "next";

const char motionInfoDbAssertActNotNil[16] = "pAct != NIL";

const char motionInfoDbEventToken[8] = "event";

const char motionInfoDbHitShiftFrameToken[16] = "hit-shift-f";

const char motionInfoDbShotBodyToken[16] = "shot-body";

const char motionInfoDbShotNotFixToken[16] = "shot-notfix";

const char motionInfoDbCharToken[8] = "char";
