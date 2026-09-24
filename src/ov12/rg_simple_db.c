/*
 * OV12 original TU 22: 0x00a14d38..0x00a15600 (17 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_simple_db.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(RgHeap *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file,
                       int line);
extern int strcmp(const char *left, const char *right);
extern char *strncpy(char *dest, const char *src, unsigned int n);
extern void XrgLog(const char *format, const char *source_file, int line,
                   ...);

/*
 * External file-backed witnesses, not candidate-emitted data: this window is
 * asm-owned scaffold data (splat names, no config/symbols/ov12.txt entry).
 *
 * ov12:0x00a531f0 contains the assertion expression "pDB != NIL".
 * ov12:0x00a53200 contains the source filename "../rg_simple_db.euc.c".
 */
extern const char D_00A531F0[];
extern const char D_00A53200[];

/*
 * More of the same asm-owned OV12 rodata window, used only by _GetDataID,
 * _EntryDB, _GetDataDB and _GetDataDBName.
 *
 * ov12:0x00a53218 contains the assertion expression "pszName != NIL".
 * ov12:0x00a53228 contains the assertion expression "pDat != NIL".
 * ov12:0x00a53238 contains the assertion expression
 * "pDB->m_nNumOfData < pDB->m_nDataCapa".
 * ov12:0x00a53260 contains the format string
 * "\n***** already entried name '%s'\n\n".
 * ov12:0x00a53288 contains the assertion expression "0" (an unconditional
 * assert_prog call on the duplicate-name path).
 * ov12:0x00a53290 contains the assertion expression
 * "0 <= nDataID && nDataID < pDB->m_nNumOfData".
 */
extern const char D_00A53218[];
extern const char D_00A53228[];
extern const char D_00A53238[];
extern const char D_00A53260[];
extern const char D_00A53288[];
extern const char D_00A53290[];

static void _InitDB(RgSimpleDB *pDB, int capacity, int entry_size);
static void _ClearDB(RgSimpleDB *pDB);
static void _DisposeDB(RgSimpleDB *pDB);
static int _GetDataID(RgSimpleDB *pDB, const char *pszName);
static int _GetDataSize(RgSimpleDB *pDB);
static void _EntryDB(RgSimpleDB *pDB, void *pDat, const char *pszName);
static void *_GetDataDB(RgSimpleDB *pDB, int nDataID);
static char *_GetDataDBName(RgSimpleDB *pDB, int nDataID);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_simple_db", _InitDB_00A14D38);

static void _ClearDB(RgSimpleDB *pDB)
{
    int i;

    if (pDB == 0) {
        assert_prog(D_00A531F0, D_00A53200, 50);
    }
    for (i = 0; i < pDB->m_nNumOfData; i++) {
        RgHeapFree(InstanceOfRgHeap(), pDB->m_apData[i], D_00A53200, 52);
    }
    pDB->m_nNumOfData = 0;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_simple_db", _DisposeDB);

static int _GetDataID(RgSimpleDB *pDB, const char *pszName)
{
    int i;

    if (pDB == 0) {
        assert_prog(D_00A531F0, D_00A53200, 80);
    }
    if (pszName == 0) {
        assert_prog(D_00A53218, D_00A53200, 81);
    }
    for (i = 0; i < pDB->m_nNumOfData; i++) {
        if (strcmp(pDB->m_apName[i], pszName) == 0) {
            return i;
        }
    }
    return -1;
}

static int _GetDataSize(RgSimpleDB *pDB)
{
    if (pDB == 0) {
        assert_prog(D_00A531F0, D_00A53200, 94);
    }
    return pDB->m_nNumOfData;
}

static void _EntryDB(RgSimpleDB *pDB, void *pDat, const char *pszName)
{
    int nIndex;

    nIndex = pDB->m_nNumOfData;
    if (pDat == 0) {
        assert_prog(D_00A53228, D_00A53200, 105);
    }
    if (pszName == 0) {
        assert_prog(D_00A53218, D_00A53200, 106);
    }
    if (!(pDB->m_nNumOfData < pDB->m_nDataCapa)) {
        assert_prog(D_00A53238, D_00A53200, 107);
    }
    if (_GetDataID(pDB, pszName) >= 0) {
        XrgLog(D_00A53260, D_00A53200, 111, pszName);
        assert_prog(D_00A53288, D_00A53200, 112);
    }
    pDB->m_apData[nIndex] = pDat;
    strncpy(pDB->m_apName[nIndex], pszName, pDB->m_nEntrySize - 1);
    pDB->m_apName[nIndex][pDB->m_nEntrySize - 1] = 0;
    pDB->m_nNumOfData++;
}

static void *_GetDataDB(RgSimpleDB *pDB, int nDataID)
{
    if (pDB == 0) {
        assert_prog(D_00A531F0, D_00A53200, 130);
    }
    if (!(0 <= nDataID && nDataID < pDB->m_nNumOfData)) {
        assert_prog(D_00A53290, D_00A53200, 131);
    }
    return pDB->m_apData[nDataID];
}

static char *_GetDataDBName(RgSimpleDB *pDB, int nDataID)
{
    if (pDB == 0) {
        assert_prog(D_00A531F0, D_00A53200, 138);
    }
    if (!(0 <= nDataID && nDataID < pDB->m_nNumOfData)) {
        assert_prog(D_00A53290, D_00A53200, 139);
    }
    return pDB->m_apName[nDataID];
}

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
