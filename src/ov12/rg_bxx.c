/*
 * OV12 original TU 73: 0x00a3ea10..0x00a3f230 (15 functions)
 */
#include "common.h"
#include "rg_bxx.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern void RgError(const char *message, const char *source_file, int line,
                    ...);

extern char *strcpy(char *destination, const char *source);

extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(RgHeap *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file,
                       int line);
extern int RgHeapIsInvalidMemory(RgHeap *heap, void *ptr);

extern void *RgLinkDataGetIndex(RgLinkData *pAna, int index);

extern void DisposeRgFileSysData_sub(struct RgFileSysData *pFile,
                                     const char *pszFile, int iLine);

/*
 * ov12/tu065 (rg_filesys.c) owns RgFileSys/RgFileSysData; this allocation
 * only forwards InstanceOfRgFileSys's result into RgFileSysRead and reads
 * the result's `data` field, so incomplete types are enough here (matching
 * rg_effect_env.c's own declaration of the same pair).
 */
extern struct RgFileSys *InstanceOfRgFileSys(void);
extern struct RgFileSysData *RgFileSysRead(struct RgFileSys *pSys,
                                           const char *pszName,
                                           const char *pszRoot);

void RgBxxSetData(RgBxx *pBxx, void *pData);

static RgBxxHeader *_GetHeader(RgBxx *pBxx);
static RgBxxPic *_GetPicTop(RgBxx *pBxx);
static int _FindPicByName(RgBxx *pBxx, const char *pszName);

/* ov12:0x00a56f90 "pBxx != NIL" */
extern const char D_00A56F90[];
/* ov12:0x00a56fa0 "../rg_bxx.euc.c" */
extern const char D_00A56FA0[];
/* ov12:0x00a56fb0 "invalid memory (link) %s %d\n(created %s %d)" */
extern const char D_00A56FB0[];
/* ov12:0x00a56fe0 "invalid memory (load) %s %d\n(created %s %d)" */
extern const char D_00A56FE0[];
/* ov12:0x00a57010 "pszName" */
extern const char D_00A57010[];
/* ov12:0x00a57018 "data\\nisimori\\" */
extern const char D_00A57018[];
/* ov12:0x00a57028 "pData != NIL" */
extern const char D_00A57028[];
/* ov12:0x00a57078 "pBxx->m_pLink != NIL" */
extern const char D_00A57078[];
/* ov12:0x00a570c8 "texture ID error (id=%d count=%d)" */
extern const char D_00A570C8[];
/* ov12:0x00a57100 "unknown texture name %s" */
extern const char D_00A57100[];

static void _InitBxx(RgBxx *pBxx, const char *pszName, int nMode)
{
    if (pBxx == 0) {
        assert_prog(D_00A56F90, D_00A56FA0, 34);
    }
    pBxx->m_pLink = 0;
    pBxx->m_pLoad = 0;
    strcpy(pBxx->m_szName, pszName);
    pBxx->m_nMode = nMode;
}

static void _DestructBxx(RgBxx *pBxx, const char *pszFile, int nLine)
{
    RgHeap *heap;

    if (pBxx == 0) {
        assert_prog(D_00A56F90, D_00A56FA0, 43);
    }
    heap = InstanceOfRgHeap();
    if (RgHeapIsInvalidMemory(heap, pBxx->m_pLink)) {
        RgError(D_00A56FB0, D_00A56FA0, 46, pszFile, nLine, pBxx->m_szName,
                pBxx->m_nMode);
    }
    heap = InstanceOfRgHeap();
    if (RgHeapIsInvalidMemory(heap, pBxx->m_pLoad)) {
        RgError(D_00A56FE0, D_00A56FA0, 49, pszFile, nLine, pBxx->m_szName,
                pBxx->m_nMode);
    }
    if (pBxx->m_pLink != 0) {
        RgHeapFree(InstanceOfRgHeap(), pBxx->m_pLink, D_00A56FA0, 53);
    }
    if (pBxx->m_pLoad != 0) {
        DisposeRgFileSysData_sub(pBxx->m_pLoad, D_00A56FA0, 55);
    }
}

RgBxx *CreateRgBxx_sub(const char *pszName, int nMode)
{
    RgHeap *heap;
    RgBxx *pBxx;

    heap = InstanceOfRgHeap();
    pBxx = RgHeapAlloc(heap, sizeof(RgBxx), D_00A56FA0, 62);
    _InitBxx(pBxx, pszName, nMode);
    return pBxx;
}

RgBxx *LoadRgBxx_sub(const char *pszName, const char *pszArchiveName,
                     int nMode)
{
    struct RgFileSysData *pFile;
    RgBxx *pBxx;

    pBxx = CreateRgBxx_sub(pszArchiveName, nMode);
    if (pszName == 0) {
        assert_prog(D_00A57010, D_00A56FA0, 71);
    }
    pFile = RgFileSysRead(InstanceOfRgFileSys(), pszName, D_00A57018);
    if (pFile == 0) {
        assert_prog(D_00A57028, D_00A56FA0, 73);
    }
    RgBxxSetData(pBxx, pFile->data);
    pBxx->m_pLoad = pFile;
    return pBxx;
}

void DisposeRgBxx_sub(RgBxx *pBxx, const char *pszFile, int nLine)
{
    if (pBxx == 0) {
        assert_prog(D_00A56F90, D_00A56FA0, 81);
    }
    _DestructBxx(pBxx, pszFile, nLine);
    RgHeapFree(InstanceOfRgHeap(), pBxx, D_00A56FA0, 83);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_bxx", RgBxxSetData);

static RgBxxHeader *_GetHeader(RgBxx *pBxx)
{
    if (pBxx == 0) {
        assert_prog(D_00A56F90, D_00A56FA0, 124);
    }
    if (pBxx->m_pLink == 0) {
        assert_prog(D_00A57078, D_00A56FA0, 125);
    }
    return RgLinkDataGetIndex(pBxx->m_pLink, 0);
}

static RgBxxPic *_GetPicTop(RgBxx *pBxx)
{
    RgBxxHeader *header;

    if (pBxx == 0) {
        assert_prog(D_00A56F90, D_00A56FA0, 132);
    }
    if (pBxx->m_pLink == 0) {
        assert_prog(D_00A57078, D_00A56FA0, 133);
    }
    header = _GetHeader(pBxx);
    return (RgBxxPic *)(header + 1);
}

void RgBxxGetHeader(RgBxx *pBxx)
{
    _GetHeader(pBxx);
}

int RgBxxGetTexNum(RgBxx *pBxx)
{
    return _GetHeader(pBxx)->numTex;
}

/*
 * rg_help.h already defines the RgBxxPic tag with a partial view (no size
 * this allocation could take), so the 0x60 (96)-byte record step below is
 * the literal step count arithmetic ("id * 0x60") and pointer indexing
 * ("pic + 96 bytes") originally used, in place of a sizeof this allocation
 * cannot take of a type it does not own.
 */
RgBxxPic *RgBxxGetPicID(RgBxx *pBxx, unsigned int id)
{
    RgBxxHeader *header;
    RgBxxPic *picTop;
    unsigned int count;

    header = _GetHeader(pBxx);
    picTop = _GetPicTop(pBxx);
    count = header->numTex;
    if (id >= count) {
        RgError(D_00A570C8, D_00A56FA0, 162, id, count);
    }
    return (RgBxxPic *)((char *)picTop + id * 0x60);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_bxx", _FindPicByName);

int RgBxxGetFindPic(RgBxx *pBxx, const char *pszName)
{
    int index;

    index = _FindPicByName(pBxx, pszName);
    if (index < 0) {
        RgError(D_00A57100, D_00A56FA0, 192, pszName);
    }
    return index;
}

RgBxxPic *RgBxxGetPic(RgBxx *pBxx, const char *pszName)
{
    RgBxxPic *picTop;
    int index;

    picTop = _GetPicTop(pBxx);
    index = _FindPicByName(pBxx, pszName);
    if (index < 0) {
        return 0;
    }
    return (RgBxxPic *)((char *)picTop + index * 0x60);
}

void RgBxxGetXtx(RgBxx *pBxx)
{
    if (pBxx == 0) {
        assert_prog(D_00A56F90, D_00A56FA0, 212);
    }
    if (pBxx->m_pLink == 0) {
        assert_prog(D_00A57078, D_00A56FA0, 213);
    }
    RgLinkDataGetIndex(pBxx->m_pLink, 1);
}
