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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_filesys", _AllocFile);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_filesys", _FindFile);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_filesys", _FindFileObj);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_filesys", _FindInPrepare);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_filesys", _InitFileSys);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_filesys", _WrapperDestruct_00A33FB0);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_filesys", InstanceOfRgFileSys);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_filesys", RgFileSysClear);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_filesys", RgFileSysRead);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_filesys", RgFileSysDup);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_filesys", RgFileSysOnMemory);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_filesys", DisposeRgFileSysData_sub);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_filesys", RgFileSysDataGetName);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_filesys", RgFileSysDump);
