/*
 * OV12 original TU 65: 0x00a33bf8..0x00a34c68 (16 functions)
 */
#include "common.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_filesys.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_singleton_id.h"
#include "rg_filesys.h"
#include "ov12/rg_vector.h"
#include "ov12/rg_error.h"
#include "ov12/xrg_v_log.h"
#include "ov12/rg_linkdata.h"
#include "ov12/xrg_cdread.h"
#include "ov12/xrg_filesys_mode.h"

#define PREPARE_MAX 4

/* RgFileSysData::mode: how the record owns its data. */
#define FILE_MODE_NORMAL 0
#define FILE_MODE_DUP 1
#define FILE_MODE_PREPARED 2

#define PREPARED_REF_COUNT 2
#define FILE_NAME_LEN 64 /* longest name RgFileSysRead accepts, with the terminator */
#define FILE_PATH_LEN 128
#define FILE_VECTOR_CAPACITY 0x80

struct RgFileSys {
    void *files;
    RgFileSysData *prepared_files[PREPARE_MAX];
    unsigned int prepared_count;
};

/*
 * The out parameter _FindInPrepare fills when it locates
 * pszName inside the first prepared link archive: the archive's own record
 * (to bump its reference count), the payload pointer RgLinkDataGet returned,
 * and a size RgFileSysRead stores as-is into the new record.
 */
typedef struct RgFileSysLinkFound {
    RgFileSysData *dupOf;
    void *data;
    unsigned int size;
} RgFileSysLinkFound;

/* Defined here: the assert and log strings below the helpers. */
extern const char fileSysAssertNameNotNil[];
extern const char fileSysAssertFileNameNotNil[];
extern const char fileSysLinkDataTypeName[];
extern const char fileSysNotLinkDataFormat[];


static int _FindInPrepare(RgFileSys *pSys, const char *pszFile,
                          RgFileSysLinkFound *pFound);
static void _WrapperDestruct(void *object);

static RgFileSysData *_AllocFile(void)
{
    RgFileSysData *pFile;

    pFile = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgFileSysData),
                         rg_filesys_source_file, 61);
    pFile->allocated = 1;
    return pFile;
}


static RgFileSysData *_FindFile(RgFileSys *pSys, const char *pszName)
{
    RgFileSysData *found;
    unsigned int index;
    unsigned int count;

    if (pSys == 0) {
        assert_prog(pSys_not_nil, rg_filesys_source_file, 74);
    }
    if (pszName == 0) {
        assert_prog(fileSysAssertNameNotNil, rg_filesys_source_file, 75);
    }
    count = RgVectorSize(pSys->files);
    for (index = 0; index < count; index++) {
        found = RgVectorIndex(pSys->files, index, rg_filesys_source_file, 78);
        if (strcmp(found->name, pszName) == 0) {
            return found;
        }
    }
    return 0;
}

const char rg_filesys_source_file[24] = "../rg_filesys.euc.c";

const char pSys_not_nil[16] = "pSys != NIL";

const char fileSysAssertNameNotNil[16] = "pszName != NIL";

const char fileSysFreeFormat[32] = "free -> %s (mode=%d ref=%d)\n";

const char fileSysAssertNameLengthWithinLimit[32] = "strlen(pszName) <= NAME_LEN";

const char fileSysAssertFileNotNil[16] = "pFile != NIL";

const char fileSysNormalReadOfPreparedFormat[40] = "read prepare data by normal read %s";

const char fileSysAssertOriginalNotNil[16] = "pOrg != NIL";

const char fileSysAssertDuplicateBufNotNil[32] = "pDup->common.m_pBuf != NIL";

const char prepare_count_check[40] = "pSys->m_uPrepareNum < PREPARE_MAX";

const char fileSysAssertPrepareNotNil[16] = "pPrepare != NIL";

const char fileSysPrepareReferencedFormat[32] = "prepare '%s' is referenced (%d)";

const char fileSysPrepareCollectDisposeFormat[32] = "prepare '%s' collect dispose";

const char fileSysAssertBufNotNil[16] = "pBuf != NIL";

const char fileSysOnMemoryAlreadyLoadedFormat[48] = "RgFileSysOnMemory failure (already loaded %s)\n";

const char fileSysOnMemoryLogFormat[40] = "RG_FILESYS : on memory (%s %p:%d)\n";

const char fileSysAssertSysNotNil[24] = "pFile->m_pSys != NIL";

const char fileSysAssertOrgNotNil[24] = "pFile->m_pOrg != NIL";

const char fileSysUnknownErrorFormat[40] = "unknown filesys error (%s) call NIS!";

const char fileSysDumpHeaderFormat[48] = "************* file sys (file=%d) *************\n";

const char fileSysDumpEntryFormat[24] = "[%s]:ptr=%p alloc=%d\n";

const char fileSysDumpNormalFormat[16] = "  normal\n";

const char fileSysDumpDuplicateFormat[16] = "  dup org=%p\n";

const char fileSysDumpPrepareFormat[16] = "  prepare\n";

static RgFileSysData *_FindFileObj(RgFileSys *pSys, RgFileSysData *pFile)
{
    RgFileSysData *found;
    unsigned int index;
    unsigned int count;

    if (pSys == 0) {
        assert_prog(pSys_not_nil, rg_filesys_source_file, 90);
    }
    count = RgVectorSize(pSys->files);
    for (index = 0; index < count; index++) {
        found = RgVectorIndex(pSys->files, index, rg_filesys_source_file, 93);
        if (found == pFile) {
            return found;
        }
    }
    return 0;
}



static int _FindInPrepare(RgFileSys *pSys, const char *pszFile,
                          RgFileSysLinkFound *pFound)
{
    RgFileSysData *pArchive;
    const char *pLast;
    char szName[FILE_PATH_LEN];
    RgLinkData linkData;
    RgFileSys *sys = pSys;
    unsigned int index;
    const char *p;
    void *payload;
    unsigned int size;

    if (pSys == 0) {
        assert_prog(pSys_not_nil, rg_filesys_source_file, 114);
    }
    if (pszFile == 0) {
        assert_prog(fileSysAssertFileNameNotNil, rg_filesys_source_file, 115);
    }
    pLast = 0;
    for (p = pszFile; *p != 0; p++) {
        if (*p == '\\' || *p == '/') {
            pLast = p;
        }
    }
    if (pLast != 0) {
        strcpy(szName, pLast + 1);
    } else {
        strcpy(szName, pszFile);
    }
    for (index = 0; index < sys->prepared_count; index++) {
        pArchive = sys->prepared_files[index];
        InitRgLinkData(&linkData, pArchive->data);
        if (RgLinkDataMatchDataName(&linkData, fileSysLinkDataTypeName) == 0) {
            RgError(fileSysNotLinkDataFormat, rg_filesys_source_file, 137, pArchive->name);
        }
        payload = RgLinkDataGet(&linkData, szName);
        size = RgLinkDataGetSize(&linkData, szName);
        if (payload != 0 && size != 0) {
            if (pFound != 0) {
                pFound->dupOf = pArchive;
                pFound->data = payload;
                pFound->size = size;
            }
            return 1;
        }
        return 0;
    }
    return 0;
}


static void _InitFileSys(RgFileSys *pSys)
{
    void *files;

    if (pSys == 0) {
        assert_prog(pSys_not_nil, rg_filesys_source_file, 156);
    }
    files = CreateRgVector(FILE_VECTOR_CAPACITY,rg_filesys_source_file, 157);
    pSys->prepared_count = 0;
    pSys->files = files;
}


static void _WrapperDestruct(void *object)
{
    RgFileSys *pSys = object;

    RgFileSysClear(pSys);
    DisposeRgVector(pSys->files, rg_filesys_source_file, 195);
    RgHeapFree(InstanceOfRgHeap(), pSys, rg_filesys_source_file, 196);
}

RgFileSys *InstanceOfRgFileSys(void)
{
    RgFileSys *pSys;

    pSys = RgSingletonIDGet(RG_SINGLETON_FILESYS);
    if (pSys == 0) {
        pSys = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgFileSys),
                            rg_filesys_source_file, 205);
        _InitFileSys(pSys);
        /*
         * The registry keeps any singleton with its destructor; its shared
         * prototype is spelled for the RgSimpleDB singletons, so this one
         * converts at the call, as rg_motion_info_db.c's
         * InstanceOfRgMotionInfoDB does for the same registry.
         */
        RgSingletonIDEntry(RG_SINGLETON_FILESYS, pSys, _WrapperDestruct);
    }
    return pSys;
}


void RgFileSysClear(RgFileSys *pSys)
{
    void *files;
    unsigned int index;
    unsigned int count;
    RgFileSysData *found;

    if (pSys == 0) {
        assert_prog(pSys_not_nil, rg_filesys_source_file, 222);
    }
    files = pSys->files;
    count = RgVectorSize(files);
    for (index = 0; index < count; index++) {
        found = RgVectorIndex(files, index, rg_filesys_source_file, 226);
        XrgLog(fileSysFreeFormat, rg_filesys_source_file, 227, found->name,
               found->mode, found->ref_count);
        XrgFileSysFree(found->data, rg_filesys_source_file, 228);
        RgHeapFree(InstanceOfRgHeap(), found, rg_filesys_source_file, 229);
    }
    RgVectorClear(files);
}


RgFileSysData *RgFileSysRead(RgFileSys *pSys, const char *pszName,
                             const char *pszRoot)
{
    RgFileSysLinkFound found;
    RgFileSysData *pFile;
    RgFileSysData *pDup;

    if (pSys == 0) {
        assert_prog(pSys_not_nil, rg_filesys_source_file, 274);
    }
    if (!(strlen(pszName) < FILE_NAME_LEN)) {
        assert_prog(fileSysAssertNameLengthWithinLimit, rg_filesys_source_file, 275);
    }
    if (_FindInPrepare(pSys, pszName, &found) != 0) {
        pDup = _AllocFile();
        pDup->dupOf = found.dupOf;
        pDup->size = found.size;
        pDup->ref_count = 1;
        pDup->data = found.data;
        pDup->owner = pSys;
        found.dupOf->ref_count++;
        pDup->mode = FILE_MODE_DUP;
        pDup->allocated = 0;
        strcpy(pDup->name, pszName);
        return pDup;
    }
    pFile = _FindFile(pSys, pszName);
    if (pFile == 0) {
        char szPath[FILE_PATH_LEN];
        unsigned int size;
        unsigned int alignSize;
        void *pBuf;

        if (pszRoot != 0) {
            strcat(strcpy(szPath, pszRoot), pszName);
        } else {
            strcpy(szPath, pszName);
        }
        pFile = _AllocFile();
        if (pFile == 0) {
            assert_prog(fileSysAssertFileNotNil, rg_filesys_source_file, 317);
        }
        size = XrgCdFileSize(szPath);
        pFile->size = size;
        if (size == 0) {
            return 0;
        }
        alignSize = XrgCdFileAlignmentSize(size);
        pBuf = XrgFileSysAlloc(alignSize, rg_filesys_source_file, 326);
        pFile->data = pBuf;
        memset(pBuf, 0, alignSize);
        if (XrgCdFileRead(szPath, pFile->data) == 0) {
            RgHeapFree(InstanceOfRgHeap(), pFile, rg_filesys_source_file, 333);
            return 0;
        }
        strncpy(pFile->name, pszName, FILE_NAME_LEN - 1);
        pFile->owner = pSys;
        pFile->mode = FILE_MODE_NORMAL;
        pFile->ref_count = 1;
        pFile->dupOf = 0;
        RgVectorPush(pSys->files, pFile);
        return pFile;
    }
    switch (pFile->mode) {
    case FILE_MODE_NORMAL:
    case FILE_MODE_DUP:
        pFile->ref_count++;
        break;
    case FILE_MODE_PREPARED:
        RgError(fileSysNormalReadOfPreparedFormat, rg_filesys_source_file, 356, pFile->name);
        break;
    }
    return pFile;
}


RgFileSysData *RgFileSysDup(RgFileSys *pSys, const char *pszName,
                            const char *pszRoot)
{
    RgFileSys *owner;
    RgFileSysData *pOrg;
    unsigned int size;
    RgFileSysData *pDup;
    void *pBuf;

    pOrg = RgFileSysRead(pSys, pszName, pszRoot);
    if (pSys == 0) {
        assert_prog(pSys_not_nil, rg_filesys_source_file, 372);
    }
    if (pOrg == 0) {
        assert_prog(fileSysAssertOriginalNotNil, rg_filesys_source_file, 373);
    }
    pDup = _AllocFile();
    size = pOrg->size;
    pDup->size = size;
    pBuf = XrgFileSysAlloc(pOrg->size, rg_filesys_source_file, 384);
    pDup->data = pBuf;
    if (pBuf == 0) {
        assert_prog(fileSysAssertDuplicateBufNotNil, rg_filesys_source_file, 385);
    }
    memcpy(pDup->data, pOrg->data, pOrg->size);
    pDup->ref_count = 1;
    strcpy(pDup->name, pOrg->name);
    owner = pOrg->owner;
    pDup->dupOf = pOrg;
    pDup->mode = FILE_MODE_DUP;
    pDup->owner = owner;
    return pDup;
}

void RgFileSysPrepareFile(RgFileSys *pSys, const char *pszName,
                          const char *pszRoot)
{
    RgFileSysData *pFile;

    if (pSys == 0)
        assert_prog(pSys_not_nil, rg_filesys_source_file, 413);

    pFile = RgFileSysRead(pSys, pszName, pszRoot);
    if (pFile == 0)
        return;

    if (!(pSys->prepared_count < PREPARE_MAX)) {
        assert_prog(prepare_count_check, rg_filesys_source_file, 422);
    }

    pSys->prepared_files[pSys->prepared_count++] =
        (pFile->mode = FILE_MODE_PREPARED, pFile);
    pFile->ref_count = PREPARED_REF_COUNT;
}


void RgFileSysDisposePrepares(RgFileSys *pSys)
{
    unsigned int index;
    RgFileSysData *pPrepare;

    if (pSys == 0) {
        assert_prog(pSys_not_nil, rg_filesys_source_file, 434);
    }
    for (index = 0; index < pSys->prepared_count; index++) {
        pPrepare = pSys->prepared_files[index];
        if (pPrepare == 0) {
            assert_prog(fileSysAssertPrepareNotNil, rg_filesys_source_file, 438);
        }
        if (pPrepare->ref_count != PREPARED_REF_COUNT) {
            RgWarn(fileSysPrepareReferencedFormat, rg_filesys_source_file, 440, pPrepare->name,
                   pPrepare->ref_count);
        } else {
            RgWarn(fileSysPrepareCollectDisposeFormat, rg_filesys_source_file, 442, pPrepare->name);
        }
        pPrepare->ref_count = 0;
        DisposeRgFileSysData_sub(pPrepare, rg_filesys_source_file, 445);
    }
    pSys->prepared_count = 0;
}


RgFileSysData *RgFileSysOnMemory(RgFileSys *pSys, const char *pszName,
                                 void *pBuf, unsigned int nSize)
{
    RgFileSysData *pFile;

    if (pSys == 0) {
        assert_prog(pSys_not_nil, rg_filesys_source_file, 460);
    }
    if (pBuf == 0) {
        assert_prog(fileSysAssertBufNotNil, rg_filesys_source_file, 461);
    }
    if (pszName == 0) {
        assert_prog(fileSysAssertNameNotNil, rg_filesys_source_file, 462);
    }
    if (_FindFile(pSys, pszName) != 0) {
        RgError(fileSysOnMemoryAlreadyLoadedFormat, rg_filesys_source_file, 467, pszName);
    }
    pFile = _AllocFile();
    pFile->data = pBuf;
    pFile->size = nSize;
    pFile->owner = pSys;
    pFile->ref_count = 1;
    pFile->mode = FILE_MODE_NORMAL;
    pFile->allocated = 0;
    pFile->dupOf = 0;
    strcpy(pFile->name, pszName);
    RgVectorPush(pSys->files, pFile);
    XrgLog(fileSysOnMemoryLogFormat, rg_filesys_source_file, 479, pszName, pBuf, nSize);
    return pFile;
}


void DisposeRgFileSysData_sub(RgFileSysData *pFile, const char *source_file,
                              int line)
{
    if (pFile == 0) {
        assert_prog(fileSysAssertFileNotNil, source_file, line);
    }
    if (pFile->owner == 0) {
        assert_prog(fileSysAssertSysNotNil, rg_filesys_source_file, 494);
    }
    if (pFile->mode == FILE_MODE_DUP) {
        if (pFile->dupOf == 0) {
            assert_prog(fileSysAssertOrgNotNil, rg_filesys_source_file, 502);
        }
        DisposeRgFileSysData_sub(pFile->dupOf, source_file, line);
        pFile->ref_count--;
        if (pFile->ref_count == 0) {
            if (pFile->data != 0 && pFile->allocated != 0) {
                XrgFileSysFree(pFile->data, source_file, line);
            }
            RgHeapFree(InstanceOfRgHeap(), pFile, source_file, line);
        }
        return;
    }
    if (pFile->mode == FILE_MODE_PREPARED) {
        if (_FindFileObj(pFile->owner, pFile) == 0) {
            RgError(fileSysUnknownErrorFormat, rg_filesys_source_file, 522, pFile->name);
        }
        if (pFile->ref_count != 0) {
            pFile->ref_count--;
        } else {
            if (pFile->data != 0 && pFile->allocated != 0) {
                XrgFileSysFree(pFile->data, source_file, line);
            }
            RgVectorRemove(pFile->owner->files, pFile, rg_filesys_source_file,
                          539);
            RgHeapFree(InstanceOfRgHeap(), pFile, source_file, line);
        }
        return;
    }
    if (_FindFileObj(pFile->owner, pFile) == 0) {
        RgError(fileSysUnknownErrorFormat, rg_filesys_source_file, 551, pFile->name);
    }
    pFile->ref_count--;
    if (pFile->ref_count == 0) {
        if (pFile->data != 0 && pFile->allocated != 0) {
            XrgFileSysFree(pFile->data, source_file, line);
        }
        RgVectorRemove(pFile->owner->files, pFile, rg_filesys_source_file,
                      565);
        RgHeapFree(InstanceOfRgHeap(), pFile, source_file, line);
    }
}

char *RgFileSysDataGetName(RgFileSysData *pFile)
{
    if (pFile == 0) {
        assert_prog(fileSysAssertFileNotNil, rg_filesys_source_file, 584);
    }

    return pFile->name;
}


void RgFileSysDump(RgFileSys *pSys)
{
    unsigned int index;
    unsigned int count;
    RgFileSysData *found;

    if (pSys == 0) {
        assert_prog(pSys_not_nil, rg_filesys_source_file, 596);
    }
    count = RgVectorSize(pSys->files);
    XrgLog(fileSysDumpHeaderFormat, rg_filesys_source_file, 599, count);
    for (index = 0; index < count; index++) {
        found = RgVectorIndex(pSys->files, index, rg_filesys_source_file, 601);
        XrgLog(fileSysDumpEntryFormat, rg_filesys_source_file, 602, found->name, found,
               found->allocated);
        switch (found->mode) {
        case FILE_MODE_NORMAL:
            XrgLog(fileSysDumpNormalFormat, rg_filesys_source_file, 605);
            break;
        case FILE_MODE_DUP:
            XrgLog(fileSysDumpDuplicateFormat, rg_filesys_source_file, 608, found->dupOf);
            break;
        case FILE_MODE_PREPARED:
            XrgLog(fileSysDumpPrepareFormat, rg_filesys_source_file, 611);
            break;
        }
    }
}
