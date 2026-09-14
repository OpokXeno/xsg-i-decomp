/*
 * OV12 original TU 83: 0x00a48918..0x00a48c08 (8 functions)
 */
#include "common.h"
#include "shared.h"

#define MODE_STACK_DEPTH 8
extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern int s_aeModeStack[MODE_STACK_DEPTH];
extern unsigned int strlen(const char *string);
extern char *strcpy(char *destination, const char *source);
extern char *strcat(char *destination, const char *source);
extern int xglCdGetFileSize(const char *name);
extern int xglCdReadFile(const char *name, void *buffer, int mode, int flags);
extern unsigned int s_uStackTop;
extern const char root_name_name_check[];
extern const char root_name_source_file[];
extern const char root_name_result_check[];
extern const char mode_stack_depth_check[];
extern const char mode_source_file[];
extern const char mode_stack_top_check[];
extern const char file_sys_size_check[];
extern const char unknown_filesys_mode[];
extern const char allocate_error[];
extern void *RgHeapAlloc(void *heap, unsigned int size, const char *source_file,
                         int line);
extern void RgError(const char *message, const char *source_file, int line, ...);
extern RgHeap *InstanceOfRgHeapData(void);
extern RgHeap *InstanceOfRgHeap(void);

static void _PushMode(int eMode)
{
    if (s_uStackTop >= MODE_STACK_DEPTH)
        assert_prog(mode_stack_depth_check, mode_source_file, 23);
    s_aeModeStack[s_uStackTop] = eMode;
    ++s_uStackTop;
}

static void _PopMode(void)
{
    if (s_uStackTop <= 1)
        assert_prog(mode_stack_top_check, mode_source_file, 29);
    --s_uStackTop;
}

static int _CurMode(void)
{
    return s_aeModeStack[s_uStackTop - 1];
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_filesys_mode", XrgFileSysPushMode);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_filesys_mode", XrgFileSysPopMode);

int XrgFileSysModeGet(void)
{
    return _CurMode();
}

void *XrgFileSysAlloc(unsigned int uSize, const char *pszFile, int iLine)
{
    void *pBuf = 0;
    int eMode;

    if (!(uSize > 0))
        assert_prog(file_sys_size_check, mode_source_file, 60);

    eMode = _CurMode();
    switch (eMode) {
    case 0:
        pBuf = RgHeapAlloc(InstanceOfRgHeapData(), uSize, pszFile, iLine);
        break;
    case 1:
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

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_filesys_mode", XrgFileSysFree);
