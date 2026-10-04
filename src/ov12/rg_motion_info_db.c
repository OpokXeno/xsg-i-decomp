/*
 * OV12 original TU 13: 0x00a0f490..0x00a10318 (20 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_singleton_id.h"
#include "rg_motion_info_db.h"

const char D_00A52460[16] = "pInfo != NIL";
const char D_00A52470[32] = "../rg_motion_info_db.euc.c";
const char D_00A524C0[32] = "pTable->m_uTblSize < ENTRY_MAX";
const char D_00A524E0[16] = "pDB != NIL";
const char D_00A52598[16] = "pMotInfo != NIL";
const char D_00A525A8[48] = "dump shot info %d ----------------------\n";
const char D_00A525D8[24] = "--- list mot %d ---\n";
const char D_00A525F0[24] = "  ** char %d **\n";
const char D_00A52608[16] = "   mot id = %d\n";
const char D_00A52618[24] = "   mot flag = %x\n";
const char D_00A52630[48] = "    [action %d (%f,%f) shift-mot %d wep %d]\n";

#define RG_MOTION_ANY_TIME 100000000.0f

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size,
                         const char *source_file, int line);
extern void XrgLog(const char *format, const char *source_file, int line, ...);
extern double fptodp(float value);

static void _InitRgMotionShotInfo(RgMotionShotInfo *info, int motionNo)
{
    if (info == 0) {
        assert_prog(D_00A52460, D_00A52470, 51);
    }
    info->motionNo = motionNo;
    info->next = 0;
    info->actionCount = info->shotFlags = 0;
    info->charId = -1;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_motion_info_db", _AddActionRgMotion);

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
    if ((unsigned int)db->entryCount >= 0x80) {
        assert_prog(D_00A524C0, D_00A52470, 176);
    }
    db->table[db->entryCount++] = entry;
}

extern void RgHeapFree(RgHeap *heap, void *pointer, const char *source_file,
                       int line);

static void _TableFree(RgMotionInfoDB *db)
{
    u32 i;
    RgMotionShotInfo *info;
    RgMotionShotInfo *next;

    for (i = 0; i < (u32) db->entryCount; i++) {
        info = db->table[i];
        while (info != 0) {
            next = info->next;
            RgHeapFree(InstanceOfRgHeap(), info, D_00A52470, 189);
            info = next;
        }
    }
    db->entryCount = 0;
}

/* Same TU, not part of this allocation. */
extern void _AddActionRgMotion(RgMotionShotInfo *, int, float, float);

static void _InitDB(RgMotionInfoDB *db) {
    RgMotionShotInfo *info;

    info = &db->defaultShotInfo;
    if (db == 0) {
        assert_prog(D_00A524E0, D_00A52470, 200);
    }
    _TableInit(db);
    _InitRgMotionShotInfo(info, -1);
    _AddActionRgMotion(info, 0, 0.1f, 0.2f);
}

/* Same TU, not part of this allocation. */
static void _TableFree(RgMotionInfoDB *db);

static void _DestructDB(RgMotionInfoDB *db)
{
    if (db == 0) {
        assert_prog(D_00A524E0, D_00A52470, 211);
    }
    _TableFree(db);
}

static void _WrapperDestruct(RgMotionInfoDB *db) {
    _DestructDB(db);
    RgHeapFree(InstanceOfRgHeap(), db, D_00A52470, 215);
}

/* Same TU, not part of this allocation. */
static void _InitDB(RgMotionInfoDB *db);
static void _WrapperDestruct(RgMotionInfoDB *db);

RgMotionInfoDB *InstanceOfRgMotionInfoDB(void)
{
    RgMotionInfoDB *db;

    db = RgSingletonIDGet(12);
    if (db == 0) {
        db = RgHeapAlloc(InstanceOfRgHeap(), 0x280, D_00A52470, 225);
        _InitDB(db);
        /*
         * The registry keeps any singleton with its destructor; its shared
         * prototype (include/shared.h, config/header-canon.json) is spelled
         * for the RgSimpleDB singletons, so this one converts at the call.
         */
        RgSingletonIDEntry(12, (RgSimpleDB *)db,
                           (void (*)(RgSimpleDB *))_WrapperDestruct);
    }
    return db;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_motion_info_db", RgMotionInfoDBLoad);

void RgMotionInfoDBDispose(RgMotionInfoDB *db)
{
    if (db == 0) {
        assert_prog(D_00A524E0, D_00A52470, 365);
    }
}

/* Same TU, not part of this allocation. */
extern RgMotionShotInfo *_TableGetMatchChar(RgMotionInfoDB *db, int motionNo,
                                            int charId);

RgMotionShotInfo *RgMotionInfoDBGet(RgMotionInfoDB *db, int motionNo,
                                    int charId)
{
    RgMotionShotInfo *info;

    if (db == 0) {
        assert_prog(D_00A524E0, D_00A52470, 374);
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
        assert_prog(D_00A524E0, D_00A52470, 388);
    }
    return _TableGetMatchChar(db, motionNo, charId) == 0;
}

RgMotionAction *RgMotionInfoGetActionInTime(RgMotionShotInfo *info,
                                             int actionNo, float time)
{
    RgMotionAction *action;
    unsigned int i;

    if (info == 0) {
        assert_prog(D_00A52598, D_00A52470, 405);
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
        assert_prog(D_00A52598, D_00A52470, 425);
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
        assert_prog(D_00A52598, D_00A52470, 442);
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
        assert_prog(D_00A524E0, D_00A52470, 459);
    }
    XrgLog(D_00A525A8, D_00A52470, 461, db->entryCount);
    for (tableIndex = 0; tableIndex < (unsigned int) db->entryCount;
         tableIndex++) {
        current = db->table[tableIndex];
        actionNo = current->motionNo;
        XrgLog(D_00A525D8, D_00A52470, 465, actionNo);
        info = current;
        for (; info != 0; info = info->next) {
            XrgLog(D_00A525F0, D_00A52470, 469, info->charId);
            XrgLog(D_00A52608, D_00A52470, 470, info->motionNo);
            XrgLog(D_00A52618, D_00A52470, 471, info->shotFlags);
            for (actionIndex = 0;
                 actionIndex < (unsigned int) info->actionCount;
                 actionIndex++) {
                actionNo = info->actions[actionIndex].actionNo;
                startSeconds = fptodp(info->actions[actionIndex].startTime);
                endSeconds = fptodp(info->actions[actionIndex].endTime);
                shiftedMotionNo = info->actions[actionIndex].shiftMotionNo;
                weaponMotionNo = info->actions[actionIndex].weaponMotionNo;
                XrgLog(D_00A52630, D_00A52470, 473, actionNo,
                       startSeconds, endSeconds, shiftedMotionNo,
                       weaponMotionNo);
            }
        }
    }
}
