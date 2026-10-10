/*
 * OV12 original TU 65: 0x00a33bf8..0x00a34c68 (16 functions)
 */
#include "common.h"
#include "rg_filesys.h"

#define PREPARE_MAX 4
#define FILE_MODE_PREPARED 2
#define PREPARED_REF_COUNT 2
struct RgFileSys {
    void *files;
    RgFileSysData *prepared_files[PREPARE_MAX];
    unsigned int prepared_count;
};
extern void assert_prog(const char *expression, const char *source_file,
                        int line);

extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(RgHeap *heap, unsigned int size,
                         const char *source_file, int line);

static RgFileSysData *_AllocFile(void)
{
    RgFileSysData *pFile;

    pFile = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgFileSysData),
                         rg_filesys_source_file, 61);
    pFile->allocated = 1;
    return pFile;
}

extern unsigned int RgVectorSize(void *vector);
extern void *RgVectorIndex(void *vector, unsigned int index,
                           const char *source_file, int line);
extern int strcmp(const char *string1, const char *string2);
extern const char D_00A559B0[]; /* "pszName != NIL" */

static int _FindFile(RgFileSys *pSys, const char *pszName)
{
    RgFileSysData *found;
    unsigned int index;
    unsigned int count;

    if (pSys == 0) {
        assert_prog(pSys_not_nil, rg_filesys_source_file, 74);
    }
    if (pszName == 0) {
        assert_prog(D_00A559B0, rg_filesys_source_file, 75);
    }
    count = RgVectorSize(pSys->files);
    for (index = 0; index < count; index++) {
        found = RgVectorIndex(pSys->files, index, rg_filesys_source_file, 78);
        if (strcmp(found->name, pszName) == 0) {
            return (int)found;
        }
    }
    return 0;
}

const char rg_filesys_source_file[24] = "../rg_filesys.euc.c";
const char pSys_not_nil[16] = "pSys != NIL";
const char D_00A559B0[16] = "pszName != NIL";
const char D_00A559F0[32] = "free -> %s (mode=%d ref=%d)\n";
const char D_00A55A10[32] = "strlen(pszName) <= NAME_LEN";
const char D_00A55A30[16] = "pFile != NIL";
const char D_00A55A40[40] = "read prepare data by normal read %s";
const char D_00A55A68[16] = "pOrg != NIL";
const char D_00A55A78[32] = "pDup->common.m_pBuf != NIL";
const char prepare_count_check[40] = "pSys->m_uPrepareNum < PREPARE_MAX";
const char D_00A55AC0[16] = "pPrepare != NIL";
const char D_00A55AD0[32] = "prepare '%s' is referenced (%d)";
const char D_00A55AF0[32] = "prepare '%s' collect dispose";
const char D_00A55B10[16] = "pBuf != NIL";
const char D_00A55B20[48] = "RgFileSysOnMemory failure (already loaded %s)\n";
const char D_00A55B50[40] = "RG_FILESYS : on memory (%s %p:%d)\n";
const char D_00A55B78[24] = "pFile->m_pSys != NIL";
const char D_00A55B90[24] = "pFile->m_pOrg != NIL";
const char D_00A55BA8[40] = "unknown filesys error (%s) call NIS!";
const char D_00A55BD0[48] = "************* file sys (file=%d) *************\n";
const char D_00A55C00[24] = "[%s]:ptr=%p alloc=%d\n";
const char D_00A55C18[16] = "  normal\n";
const char D_00A55C28[16] = "  dup org=%p\n";
const char D_00A55C38[16] = "  prepare\n";




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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_filesys", _FindInPrepare);

extern void *CreateRgVector(int capacity, const char *source_file, int line);

static void _InitFileSys(RgFileSys *pSys)
{
    void *files;

    if (pSys == 0) {
        assert_prog(pSys_not_nil, rg_filesys_source_file, 156);
    }
    files = CreateRgVector(0x80, rg_filesys_source_file, 157);
    pSys->prepared_count = 0;
    pSys->files = files;
}

extern void RgFileSysClear(RgFileSys *pSys);
extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file,
                       int line);
extern void DisposeRgVector(void *vector, const char *source_file, int line);

static void _WrapperDestruct(RgFileSys *pSys)
{
    RgFileSysClear(pSys);
    DisposeRgVector(pSys->files, rg_filesys_source_file, 195);
    RgHeapFree(InstanceOfRgHeap(), pSys, rg_filesys_source_file, 196);
}

RgFileSys *InstanceOfRgFileSys(void)
{
    RgFileSys *pSys;

    pSys = RgSingletonIDGet(5U);
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
        RgSingletonIDEntry(5, (RgSimpleDB *)pSys,
                           (void (*)(RgSimpleDB *))_WrapperDestruct);
    }
    return pSys;
}

extern void XrgFileSysFree(void *pBuf, const char *pszFile, int iLine);
extern void RgVectorClear(void *vector);
extern void XrgLog(const char *format, const char *source_file, int line, ...);
extern const char D_00A559F0[]; /* "free -> %s (mode=%d ref=%d)\n" */

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
        XrgLog(D_00A559F0, rg_filesys_source_file, 227, found->name,
               found->mode, found->ref_count);
        XrgFileSysFree(found->data, rg_filesys_source_file, 228);
        RgHeapFree(InstanceOfRgHeap(), found, rg_filesys_source_file, 229);
    }
    RgVectorClear(files);
}

/*
 * The out parameter _FindInPrepare (still INCLUDE_ASM) fills when it locates
 * pszName inside the first prepared link archive: the archive's own record
 * (to bump its reference count), the payload pointer RgLinkDataGet returned,
 * and a size RgFileSysRead stores as-is into the new record.
 */
typedef struct RgFileSysLinkFound {
    RgFileSysData *dupOf;
    void *data;
    unsigned int size;
} RgFileSysLinkFound;

extern int _FindInPrepare(RgFileSys *pSys, const char *pszFile,
                          RgFileSysLinkFound *pFound);
extern int _FindFile(RgFileSys *pSys, const char *pszName);
extern unsigned int strlen(const char *string);
extern char *strcat(char *destination, const char *source);
extern char *strncpy(char *destination, const char *source, unsigned int count);
extern void *memset(void *destination, int value, unsigned int count);
extern void RgVectorPush(void *vector, void *element);
extern unsigned int XrgCdFileSize(const char *pszFile);
extern unsigned int XrgCdFileAlignmentSize(unsigned int size);
extern void *XrgFileSysAlloc(unsigned int uSize, const char *pszFile,
                             int iLine);
extern int XrgCdFileRead(const char *pszFile, void *pBuf);

extern void RgError(const char *message, const char *source_file, int line,
                    ...);
extern char *strcpy(char *destination, const char *source);
extern const char D_00A55A10[]; /* "strlen(pszName) <= NAME_LEN" */
extern const char D_00A55A30[]; /* "pFile != NIL" */
extern const char D_00A55A40[]; /* "read prepare data by normal read %s" */

RgFileSysData *RgFileSysRead(RgFileSys *pSys, const char *pszName,
                             const char *pszRoot)
{
    RgFileSysLinkFound found;
    RgFileSysData *pFile;
    RgFileSysData *pDup;

    if (pSys == 0) {
        assert_prog(pSys_not_nil, rg_filesys_source_file, 274);
    }
    if (!(strlen(pszName) < 64)) {
        assert_prog(D_00A55A10, rg_filesys_source_file, 275);
    }
    if (_FindInPrepare(pSys, pszName, &found) != 0) {
        pDup = _AllocFile();
        pDup->dupOf = found.dupOf;
        pDup->size = found.size;
        pDup->ref_count = 1;
        pDup->data = found.data;
        pDup->owner = pSys;
        found.dupOf->ref_count++;
        pDup->mode = 1;
        pDup->allocated = 0;
        strcpy(pDup->name, pszName);
        return pDup;
    }
    pFile = (RgFileSysData *)_FindFile(pSys, pszName);
    if (pFile == 0) {
        char szPath[128];
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
            assert_prog(D_00A55A30, rg_filesys_source_file, 317);
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
        strncpy(pFile->name, pszName, 63);
        pFile->owner = pSys;
        pFile->mode = 0;
        pFile->ref_count = 1;
        pFile->dupOf = 0;
        RgVectorPush(pSys->files, pFile);
        return pFile;
    }
    switch (pFile->mode) {
    case 0:
    case 1:
        pFile->ref_count++;
        break;
    case 2:
        RgError(D_00A55A40, rg_filesys_source_file, 356, pFile->name);
        break;
    }
    return pFile;
}


extern void *memcpy(void *destination, const void *source, unsigned int count);

extern const char D_00A55A68[]; /* "pOrg != NIL" */
extern const char D_00A55A78[]; /* "pDup->common.m_pBuf != NIL" */

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
        assert_prog(D_00A55A68, rg_filesys_source_file, 373);
    }
    pDup = _AllocFile();
    size = pOrg->size;
    pDup->size = size;
    pBuf = XrgFileSysAlloc(pOrg->size, rg_filesys_source_file, 384);
    pDup->data = pBuf;
    if (pBuf == 0) {
        assert_prog(D_00A55A78, rg_filesys_source_file, 385);
    }
    memcpy(pDup->data, pOrg->data, pOrg->size);
    pDup->ref_count = 1;
    strcpy(pDup->name, pOrg->name);
    owner = pOrg->owner;
    pDup->dupOf = pOrg;
    pDup->mode = 1;
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

extern void RgWarn(const char *format, const char *source_file, int line, ...);
extern const char D_00A55AC0[]; /* "pPrepare != NIL" */
extern const char D_00A55AD0[]; /* "prepare '%s' is referenced (%d)" */
extern const char D_00A55AF0[]; /* "prepare '%s' collect dispose" */
extern void DisposeRgFileSysData_sub(RgFileSysData *pFile,
                                     const char *source_file, int line);

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
            assert_prog(D_00A55AC0, rg_filesys_source_file, 438);
        }
        if (pPrepare->ref_count != PREPARED_REF_COUNT) {
            RgWarn(D_00A55AD0, rg_filesys_source_file, 440, pPrepare->name,
                   pPrepare->ref_count);
        } else {
            RgWarn(D_00A55AF0, rg_filesys_source_file, 442, pPrepare->name);
        }
        pPrepare->ref_count = 0;
        DisposeRgFileSysData_sub(pPrepare, rg_filesys_source_file, 445);
    }
    pSys->prepared_count = 0;
}




 /* "pszName != NIL" */
extern const char D_00A55B10[]; /* "pBuf != NIL" */
extern const char D_00A55B20[]; /* "RgFileSysOnMemory failure (already loaded %s)\n" */
extern const char D_00A55B50[]; /* "RG_FILESYS : on memory (%s %p:%d)\n" */


RgFileSysData *RgFileSysOnMemory(RgFileSys *pSys, const char *pszName,
                                 void *pBuf, unsigned int nSize)
{
    RgFileSysData *pFile;

    if (pSys == 0) {
        assert_prog(pSys_not_nil, rg_filesys_source_file, 460);
    }
    if (pBuf == 0) {
        assert_prog(D_00A55B10, rg_filesys_source_file, 461);
    }
    if (pszName == 0) {
        assert_prog(D_00A559B0, rg_filesys_source_file, 462);
    }
    if (_FindFile(pSys, pszName) != 0) {
        RgError(D_00A55B20, rg_filesys_source_file, 467, pszName);
    }
    pFile = _AllocFile();
    pFile->data = pBuf;
    pFile->size = nSize;
    pFile->owner = pSys;
    pFile->ref_count = 1;
    pFile->mode = 0;
    pFile->allocated = 0;
    pFile->dupOf = 0;
    strcpy(pFile->name, pszName);
    RgVectorPush(pSys->files, pFile);
    XrgLog(D_00A55B50, rg_filesys_source_file, 479, pszName, pBuf, nSize);
    return pFile;
}

 /* "pFile != NIL" */
extern const char D_00A55B78[]; /* "pFile->m_pSys != NIL" */
extern const char D_00A55B90[]; /* "pFile->m_pOrg != NIL" */
extern const char D_00A55BA8[]; /* "unknown filesys error (%s) call NIS!" */
extern int RgVectorRemove(void *vector, void *element, const char *source_file,
                          int line);


void DisposeRgFileSysData_sub(RgFileSysData *pFile, const char *source_file,
                              int line)
{
    if (pFile == 0) {
        assert_prog(D_00A55A30, source_file, line);
    }
    if (pFile->owner == 0) {
        assert_prog(D_00A55B78, rg_filesys_source_file, 494);
    }
    if (pFile->mode == 1) {
        if (pFile->dupOf == 0) {
            assert_prog(D_00A55B90, rg_filesys_source_file, 502);
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
    if (pFile->mode == 2) {
        if (_FindFileObj(pFile->owner, pFile) == 0) {
            RgError(D_00A55BA8, rg_filesys_source_file, 522, pFile->name);
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
        RgError(D_00A55BA8, rg_filesys_source_file, 551, pFile->name);
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

/*
 * This accessor's own additive view of RgFileSysData (completed in
 * include/shared.h up to +0x14): only the inline name storage its address
 * computation evidences is named. Shared-header need: extend the owned
 * RgFileSysData with this field once header_harvest can regenerate its
 * published layout; a name field cannot be restated here since
 * include/shared.h already completes the tag.
 */
typedef struct RgFileSysDataName {
    unsigned char unmodeled_00[0x1c];
    char name; /* +0x1C */
} RgFileSysDataName;

 /* "pFile != NIL" */

char *RgFileSysDataGetName(RgFileSysData *pFile)
{
    if (pFile == 0) {
        assert_prog(D_00A55A30, rg_filesys_source_file, 584);
    }

    return &((RgFileSysDataName *)pFile)->name;
}

extern const char D_00A55BD0[]; /* "************* file sys (file=%d) *************\n" */
extern const char D_00A55C00[]; /* "[%s]:ptr=%p alloc=%d\n" */
extern const char D_00A55C18[]; /* "  normal\n" */
extern const char D_00A55C28[]; /* "  dup org=%p\n" */
extern const char D_00A55C38[]; /* "  prepare\n" */

void RgFileSysDump(RgFileSys *pSys)
{
    unsigned int index;
    unsigned int count;
    RgFileSysData *found;

    if (pSys == 0) {
        assert_prog(pSys_not_nil, rg_filesys_source_file, 596);
    }
    count = RgVectorSize(pSys->files);
    XrgLog(D_00A55BD0, rg_filesys_source_file, 599, count);
    for (index = 0; index < count; index++) {
        found = RgVectorIndex(pSys->files, index, rg_filesys_source_file, 601);
        XrgLog(D_00A55C00, rg_filesys_source_file, 602, found->name, found,
               found->allocated);
        switch (found->mode) {
        case 0:
            XrgLog(D_00A55C18, rg_filesys_source_file, 605);
            break;
        case 1:
            XrgLog(D_00A55C28, rg_filesys_source_file, 608, found->dupOf);
            break;
        case 2:
            XrgLog(D_00A55C38, rg_filesys_source_file, 611);
            break;
        }
    }
}
