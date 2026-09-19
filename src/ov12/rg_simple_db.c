/*
 * OV12 original TU 22: 0x00a14d38..0x00a15600 (17 functions)
 */
#include "common.h"
#include "shared.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(RgHeap *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file,
                       int line);

/*
 * External file-backed witnesses, not candidate-emitted data: this window is
 * asm-owned scaffold data (splat names, no config/symbols/ov12.txt entry).
 *
 * ov12:0x00a531f0 contains the assertion expression "pDB != NIL".
 * ov12:0x00a53200 contains the source filename "../rg_simple_db.euc.c".
 */
extern const char D_00A531F0[];
extern const char D_00A53200[];

extern void _InitDB(RgSimpleDB *pDB, int capacity, int entry_size);
extern void _ClearDB(RgSimpleDB *pDB);
extern void _DisposeDB(RgSimpleDB *pDB);
extern int _GetDataID(RgSimpleDB *pDB, const char *pszName);
extern int _GetDataSize(RgSimpleDB *pDB);
extern void _EntryDB(RgSimpleDB *pDB, void *pDat, const char *pszName);
extern void *_GetDataDB(RgSimpleDB *pDB, int nDataID);
extern char *_GetDataDBName(RgSimpleDB *pDB, int nDataID);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_simple_db", _InitDB_00A14D38);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_simple_db", _ClearDB);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_simple_db", _DisposeDB);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_simple_db", _GetDataID);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_simple_db", _GetDataSize);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_simple_db", _EntryDB);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_simple_db", _GetDataDB);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_simple_db", _GetDataDBName);

RgSimpleDB *CreateRgSimpleDB(int capacity, int entry_size)
{
    RgSimpleDB *pDB;

    /* 20 bytes: the size of the opaque RgSimpleDB descriptor. */
    pDB = RgHeapAlloc(InstanceOfRgHeap(), 20, D_00A53200, 151);
    if (pDB == 0) {
        assert_prog(D_00A531F0, D_00A53200, 152);
    }
    _InitDB(pDB, capacity, entry_size);
    return pDB;
}

void DisposeRgSimpleDB(RgSimpleDB *pDB)
{
    if (pDB == 0) {
        assert_prog(D_00A531F0, D_00A53200, 158);
    }
    _DisposeDB(pDB);
    RgHeapFree(InstanceOfRgHeap(), pDB, D_00A53200, 160);
}

int RgSimpleDBFind(RgSimpleDB *pDB, const char *pszName)
{
    return _GetDataID(pDB, pszName);
}

int RgSimpleDBSize(RgSimpleDB *pDB)
{
    return _GetDataSize(pDB);
}

void *RgSimpleDBGet(RgSimpleDB *pDB, int nDataID)
{
    return _GetDataDB(pDB, nDataID);
}

char *RgSimpleDBGetName(RgSimpleDB *pDB, int nDataID)
{
    return _GetDataDBName(pDB, nDataID);
}

void RgSimpleDBEntry(RgSimpleDB *pDB, void *pDat, const char *pszName)
{
    _EntryDB(pDB, pDat, pszName);
}

void RgSimpleDBClear(RgSimpleDB *pDB)
{
    if (pDB == 0) {
        assert_prog(D_00A531F0, D_00A53200, 207);
    }
    _ClearDB(pDB);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_simple_db", RgSimpleDBDump);
