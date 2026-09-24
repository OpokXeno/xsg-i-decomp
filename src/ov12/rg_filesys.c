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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_filesys", _FindFile);

extern unsigned int RgVectorSize(void *vector);
extern void *RgVectorIndex(void *vector, unsigned int index,
                           const char *source_file, int line);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_filesys", RgFileSysClear);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_filesys", RgFileSysRead);

extern void *XrgFileSysAlloc(unsigned int uSize, const char *pszFile,
                             int iLine);
extern void *memcpy(void *destination, const void *source, unsigned int count);
extern char *strcpy(char *destination, const char *source);
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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_filesys", RgFileSysDisposePrepares);

extern void RgVectorPush(void *vector, void *element);
extern void XrgLog(const char *format, const char *source_file, int line, ...);
extern int _FindFile(RgFileSys *pSys, const char *pszName);
extern const char D_00A559B0[]; /* "pszName != NIL" */
extern const char D_00A55B10[]; /* "pBuf != NIL" */
extern const char D_00A55B20[]; /* "RgFileSysOnMemory failure (already loaded %s)\n" */
extern const char D_00A55B50[]; /* "RG_FILESYS : on memory (%s %p:%d)\n" */
extern void RgError(const char *message, const char *source_file, int line, ...);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_filesys", DisposeRgFileSysData_sub);

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

extern const char D_00A55A30[]; /* "pFile != NIL" */

char *RgFileSysDataGetName(RgFileSysData *pFile)
{
    if (pFile == 0) {
        assert_prog(D_00A55A30, rg_filesys_source_file, 584);
    }

    return &((RgFileSysDataName *)pFile)->name;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_filesys", RgFileSysDump);
