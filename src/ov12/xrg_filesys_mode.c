/*
 * OV12 original TU 83: 0x00a48918..0x00a48c08 (8 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_heap.h"
#include "ov12/xrg_filesys_mode.h"
#include "ov12/rg_error.h"

#define MODE_STACK_DEPTH 8
#define MODE_STACK_BOTTOM 1 /* slot 0 is the base mode and is never popped */

static int s_aeModeStack[MODE_STACK_DEPTH] = {0};
static unsigned int s_uStackTop = MODE_STACK_BOTTOM;

const char mode_stack_depth_check[32] = "s_uStackTop < MODE_STACK_DEPTH";
const char mode_source_file[32] = "../xrg_filesys_mode.euc.c";
const char mode_stack_top_check[16] = "s_uStackTop > 1";
const char file_sys_size_check[16] = "uSize > 0";
const char unknown_filesys_mode[32] = "unknown filesys mode %d\n";
const char allocate_error[16] = "allocate error";
const char fileSysModeAssertBufNotNil[16] = "pBuf != NIL";


static void _PushMode(int eMode)
{
    if (s_uStackTop >= MODE_STACK_DEPTH)
        assert_prog(mode_stack_depth_check, mode_source_file, 23);
    s_aeModeStack[s_uStackTop] = eMode;
    ++s_uStackTop;
}

static void _PopMode(void)
{
    if (s_uStackTop <= MODE_STACK_BOTTOM)
        assert_prog(mode_stack_top_check, mode_source_file, 29);
    --s_uStackTop;
}

static int _CurMode(void)
{
    return s_aeModeStack[s_uStackTop - 1];
}

void XrgFileSysPushMode(int eMode)
{
    _PushMode(eMode);
}

void XrgFileSysPopMode(void)
{
    _PopMode();
}

int XrgFileSysModeGet(void)
{
    return _CurMode();
}

/* Allocates uSize bytes from the heap selected by the current filesys mode. */
void *XrgFileSysAlloc(unsigned int uSize, const char *pszFile, int iLine)
{
    void *pBuf = 0;
    int eMode;

    if (!(uSize > 0))
        assert_prog(file_sys_size_check, mode_source_file, 60);

    eMode = _CurMode();
    switch (eMode) {
    case XRG_FILESYS_MODE_DATA_HEAP:
        pBuf = RgHeapAlloc(InstanceOfRgHeapData(), uSize, pszFile, iLine);
        break;
    case XRG_FILESYS_MODE_CODE_HEAP:
        pBuf = RgHeapAlloc(InstanceOfRgHeap(), uSize, pszFile, iLine);
        break;
    default:
        RgError(unknown_filesys_mode, mode_source_file, 73, _CurMode());
        break;
    }
    if (pBuf == 0)
        RgError(allocate_error, mode_source_file, 77);
    return pBuf;
}

/* Frees pBuf from the code heap if it lies in that heap, otherwise from the data heap. */
void XrgFileSysFree(void *pBuf, const char *pszFile, int iLine)
{
    if (pBuf == 0)
        assert_prog(fileSysModeAssertBufNotNil, mode_source_file, 84);

    if (RgHeapIsInSelf(InstanceOfRgHeap(), pBuf))
        RgHeapFree(InstanceOfRgHeap(), pBuf, pszFile, iLine);
    else
        RgHeapFree(InstanceOfRgHeapData(), pBuf, pszFile, iLine);
}
