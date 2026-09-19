/*
 * OV12 original TU 13: 0x00a0f490..0x00a10318 (20 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_singleton_id.h"
#include "rg_motion_info_db.h"

/*
 * ov12:0x00a52460, "pInfo != NIL"
 * ov12:0x00a52470, "../rg_motion_info_db.euc.c"
 * ov12:0x00a524e0, "pDB != NIL"
 * Scaffold .rodata (config/tu-build.json data_ownership .rodata: owner asm),
 * so they keep their splat names.
 */
extern const char D_00A52460[];
extern const char D_00A52470[];
extern const char D_00A524E0[];

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size,
                         const char *source_file, int line);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_motion_info_db", _TableSort);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_motion_info_db", _TableGet);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_motion_info_db", _TableGetMatchChar);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_motion_info_db", _TableEntry);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_motion_info_db", _TableFree);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_motion_info_db", _InitDB_00A0F828);

/* Same TU, not part of this allocation. */
static void _TableFree(RgMotionInfoDB *db);

static void _DestructDB(RgMotionInfoDB *db)
{
    if (db == 0) {
        assert_prog(D_00A524E0, D_00A52470, 211);
    }
    _TableFree(db);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_motion_info_db", _WrapperDestruct_00A0F8E8);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_motion_info_db", RgMotionInfoGetActionInTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_motion_info_db", RgMotionInfoIsBefore);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_motion_info_db", RgMotionInfoIsAfter);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_motion_info_db", RgMotionInfoDBDump);
