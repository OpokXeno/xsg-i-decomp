/*
 * OV12 original TU 22: 0x00a14d38..0x00a15600 (17 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_heap.h"
#include "rg_simple_db.h"
#include "ov12/xrg_v_log.h"


/* Defined here: the assert and log strings at the end of the file. */
extern const char simpleDbAssertDbNotNil[];
extern const char simpleDbSourcePath[];
extern const char simpleDbAssertNameNotNil[];
extern const char simpleDbAssertDatNotNil[];
extern const char simpleDbAssertNumOfDataBelowCapacity[];
extern const char simpleDbAlreadyEntriedNameFormat[];
extern const char simpleDbAssertZero[];
extern const char simpleDbAssertDataIdInRange[];

static void _InitDB(RgSimpleDB *pDB, int capacity, int entry_size);
static void _ClearDB(RgSimpleDB *pDB);
static void _DisposeDB(RgSimpleDB *pDB);
static int _GetDataID(RgSimpleDB *pDB, const char *pszName);
static int _GetDataSize(RgSimpleDB *pDB);
static void _EntryDB(RgSimpleDB *pDB, void *pDat, const char *pszName);
static void *_GetDataDB(RgSimpleDB *pDB, int nDataID);
static char *_GetDataDBName(RgSimpleDB *pDB, int nDataID);

static void _InitDB(RgSimpleDB *pDB, int capacity, int entry_size)
{
    int i;
    unsigned int pointer_array_size;

    if (pDB == 0) {
        assert_prog(simpleDbAssertDbNotNil, simpleDbSourcePath, 35);
    }
    pDB->m_nNumOfData = 0;
    pointer_array_size = capacity * RG_SIMPLE_DB_ENTRY_STRIDE;
    pDB->m_nDataCapa = capacity;
    pDB->m_nEntrySize = entry_size;
    pDB->m_apData = RgHeapAlloc(InstanceOfRgHeap(), pointer_array_size,
                                simpleDbSourcePath, 39);
    pDB->m_apName = RgHeapAlloc(InstanceOfRgHeap(), pointer_array_size,
                                simpleDbSourcePath, 40);
    for (i = 0; i < capacity; i++) {
        pDB->m_apName[i] = RgHeapAlloc(InstanceOfRgHeap(), entry_size,
                                       simpleDbSourcePath, 42);
    }
}

static void _ClearDB(RgSimpleDB *pDB)
{
    int i;

    if (pDB == 0) {
        assert_prog(simpleDbAssertDbNotNil, simpleDbSourcePath, 50);
    }
    for (i = 0; i < pDB->m_nNumOfData; i++) {
        RgHeapFree(InstanceOfRgHeap(), pDB->m_apData[i], simpleDbSourcePath, 52);
    }
    pDB->m_nNumOfData = 0;
}

static void _DisposeDB(RgSimpleDB *pDB)
{
    int i;

    if (pDB == 0) {
        assert_prog(simpleDbAssertDbNotNil, simpleDbSourcePath, 60);
    }
    for (i = 0; i < pDB->m_nNumOfData; i++) {
        RgHeapFree(InstanceOfRgHeap(), pDB->m_apData[i], simpleDbSourcePath, 63);
    }
    RgHeapFree(InstanceOfRgHeap(), pDB->m_apData, simpleDbSourcePath, 64);
    pDB->m_apData = 0;
    for (i = 0; i < pDB->m_nDataCapa; i++) {
        RgHeapFree(InstanceOfRgHeap(), pDB->m_apName[i], simpleDbSourcePath, 68);
    }
    RgHeapFree(InstanceOfRgHeap(), pDB->m_apName, simpleDbSourcePath, 69);
    pDB->m_apName = 0;
}

static int _GetDataID(RgSimpleDB *pDB, const char *pszName)
{
    int i;

    if (pDB == 0) {
        assert_prog(simpleDbAssertDbNotNil, simpleDbSourcePath, 80);
    }
    if (pszName == 0) {
        assert_prog(simpleDbAssertNameNotNil, simpleDbSourcePath, 81);
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
        assert_prog(simpleDbAssertDbNotNil, simpleDbSourcePath, 94);
    }
    return pDB->m_nNumOfData;
}

static void _EntryDB(RgSimpleDB *pDB, void *pDat, const char *pszName)
{
    int nIndex;

    nIndex = pDB->m_nNumOfData;
    if (pDat == 0) {
        assert_prog(simpleDbAssertDatNotNil, simpleDbSourcePath, 105);
    }
    if (pszName == 0) {
        assert_prog(simpleDbAssertNameNotNil, simpleDbSourcePath, 106);
    }
    if (!(pDB->m_nNumOfData < pDB->m_nDataCapa)) {
        assert_prog(simpleDbAssertNumOfDataBelowCapacity, simpleDbSourcePath, 107);
    }
    if (_GetDataID(pDB, pszName) >= 0) {
        XrgLog(simpleDbAlreadyEntriedNameFormat, simpleDbSourcePath, 111, pszName);
        assert_prog(simpleDbAssertZero, simpleDbSourcePath, 112);
    }
    pDB->m_apData[nIndex] = pDat;
    strncpy(pDB->m_apName[nIndex], pszName, pDB->m_nEntrySize - 1);
    pDB->m_apName[nIndex][pDB->m_nEntrySize - 1] = 0;
    pDB->m_nNumOfData++;
}

static void *_GetDataDB(RgSimpleDB *pDB, int nDataID)
{
    if (pDB == 0) {
        assert_prog(simpleDbAssertDbNotNil, simpleDbSourcePath, 130);
    }
    if (!(0 <= nDataID && nDataID < pDB->m_nNumOfData)) {
        assert_prog(simpleDbAssertDataIdInRange, simpleDbSourcePath, 131);
    }
    return pDB->m_apData[nDataID];
}

static char *_GetDataDBName(RgSimpleDB *pDB, int nDataID)
{
    if (pDB == 0) {
        assert_prog(simpleDbAssertDbNotNil, simpleDbSourcePath, 138);
    }
    if (!(0 <= nDataID && nDataID < pDB->m_nNumOfData)) {
        assert_prog(simpleDbAssertDataIdInRange, simpleDbSourcePath, 139);
    }
    return pDB->m_apName[nDataID];
}

RgSimpleDB *CreateRgSimpleDB(int capacity, int entry_size)
{
    RgSimpleDB *pDB;

    pDB = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgSimpleDB), simpleDbSourcePath, 151);
    if (pDB == 0) {
        assert_prog(simpleDbAssertDbNotNil, simpleDbSourcePath, 152);
    }
    _InitDB(pDB, capacity, entry_size);
    return pDB;
}

void DisposeRgSimpleDB(RgSimpleDB *pDB)
{
    if (pDB == 0) {
        assert_prog(simpleDbAssertDbNotNil, simpleDbSourcePath, 158);
    }
    _DisposeDB(pDB);
    RgHeapFree(InstanceOfRgHeap(), pDB, simpleDbSourcePath, 160);
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
        assert_prog(simpleDbAssertDbNotNil, simpleDbSourcePath, 207);
    }
    _ClearDB(pDB);
}

/* Scaffold data in the same rodata window. */
extern const char simpleDbDumpHeaderFormat[];
extern const char rgSimpleDBDumpFormat[] __asm__("simpleDbDumpEntryFormat");

void RgSimpleDBDump(RgSimpleDB *pDB)
{
    int i;

    if (pDB == 0) {
        assert_prog(simpleDbAssertDbNotNil, simpleDbSourcePath, 218);
    }
    XrgLog(simpleDbDumpHeaderFormat, simpleDbSourcePath, 220, pDB);
    for (i = 0; i < pDB->m_nNumOfData; ++i) {
        XrgLog(rgSimpleDBDumpFormat, simpleDbSourcePath, 222, i, pDB->m_apName[i],
               pDB->m_apData[i]);
    }
}

const char simpleDbAssertDbNotNil[16] = "pDB != NIL";

const char simpleDbSourcePath[24] = "../rg_simple_db.euc.c";

const char simpleDbAssertNameNotNil[16] = "pszName != NIL";

const char simpleDbAssertDatNotNil[16] = "pDat != NIL";

const char simpleDbAssertNumOfDataBelowCapacity[40] = "pDB->m_nNumOfData < pDB->m_nDataCapa";

const char simpleDbAlreadyEntriedNameFormat[40] = "\n***** already entried name '%s'\n\n";

const char simpleDbAssertZero[8] = "0";

const char simpleDbAssertDataIdInRange[48] = "0 <= nDataID && nDataID < pDB->m_nNumOfData";



const char simpleDbDumpHeaderFormat[16] = "data base - %p\n";

const char simpleDbDumpEntryFormat[24] = "data %d : %s (%p)\n";
