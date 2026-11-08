/*
 * OV12 original TU 82: 0x00a487c0..0x00a48918 (5 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_cd.h"
#include "ov12/assert_prog.h"
#include "xrg_cdread.h"


const char cdReadEmptyPath[8] = {0};
const char root_name_name_check[] = "pszName != NIL";
const char root_name_source_file[] = "../xrg_cdread.euc.c";
const char root_name_result_check[] = "pszResult != NIL";

char *_GetLocalPath(void)
{
    return (char *) cdReadEmptyPath;
}

/* Builds pszResult from pszRoot (or an empty string) followed by the part of pszName after its last path separator. */
static char *_RootName(const char *pszName, char *pszResult, const char *pszRoot)
{
    int length;
    char *last;

    if (pszName == 0)
        assert_prog(root_name_name_check, root_name_source_file, 57);
    if (pszResult == 0)
        assert_prog(root_name_result_check, root_name_source_file, 58);

    length = strlen(pszName);
    last = (char *)pszName + length - 1;
    while (length-- > 0) {
        if (*last == '\\' || *last == '/')
            break;
        --last;
    }
    if (pszRoot != 0)
        strcpy(pszResult, pszRoot);
    else
        *pszResult = 0;
    return strcat(pszResult, last + 1);
}

int XrgCdFileSize(const char *pszName)
{
    return xglCdGetFileSize(pszName);
}

int XrgCdFileRead(const char *pszName, void *pBuf)
{
    int result;

    result = xglCdReadFile(pszName, pBuf, 0, 0);
    return result >= 0;
}

int XrgCdFileAlignmentSize(int uSize)
{
    return (uSize + (XRG_CD_SECTOR_SIZE - 1)) & ~(XRG_CD_SECTOR_SIZE - 1);
}
