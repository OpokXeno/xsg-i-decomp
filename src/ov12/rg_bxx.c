/*
 * OV12 original TU 73: 0x00a3ea10..0x00a3f230 (15 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_bxx.h"
#include "ov12/rg_error.h"
#include "ov12/rg_filesys.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_linkdata.h"






/* Size of one picture record (sizeof(RgBxxPic)). */
#define RG_BXX_PIC_SIZE 0x60
#define RG_BXX_LINK_DATA_COUNT 2

static RgBxxHeader *_GetHeader(RgBxx *pBxx);
static RgBxxPic *_GetPicTop(RgBxx *pBxx);
static int _FindPicByName(RgBxx *pBxx, const char *pszName);

extern const char bxxAssertBxxNotNil[];
extern const char bxxSourcePath[];
extern const char bxxInvalidLinkMemoryFormat[];
extern const char bxxInvalidLoadMemoryFormat[];
extern const char bxxNameParameterName[];
extern const char bxxDataDirectory[];
extern const char bxxAssertDataNotNil[];
extern const char bxxAssertLoadNil[];
extern const char bxxDataFormatErrorFormat[];
typedef struct RgBxxMessageBlock {
    char linkRequired[24];
    char pictureSize[24];
    char headerSize[28];
    char empty[4];
} RgBxxMessageBlock;
extern const RgBxxMessageBlock bxxAssertMessageBlock;
extern const char bxxTextureIdErrorFormat[];
extern const char bxxAssertNameNotNil[];
extern const char bxxUnknownTextureNameFormat[];

#define RG_BXX_NOT_NULL bxxAssertBxxNotNil
#define RG_BXX_SOURCE_FILE bxxSourcePath
#define RG_BXX_INVALID_LINK_MEMORY bxxInvalidLinkMemoryFormat
#define RG_BXX_INVALID_LOAD_MEMORY bxxInvalidLoadMemoryFormat
#define RG_BXX_NAME_PARAMETER bxxNameParameterName
#define RG_BXX_DATA_ROOT bxxDataDirectory
#define RG_BXX_DATA_REQUIRED bxxAssertDataNotNil
#define RG_BXX_LOAD_REQUIRED bxxAssertLoadNil
#define RG_BXX_DATA_FORMAT_ERROR bxxDataFormatErrorFormat
#define RG_BXX_LINK_REQUIRED bxxAssertMessageBlock.linkRequired
#define RG_BXX_TEXTURE_COUNT_ERROR bxxTextureIdErrorFormat
#define RG_BXX_NAME_REQUIRED bxxAssertNameNotNil
#define RG_BXX_UNKNOWN_TEXTURE bxxUnknownTextureNameFormat

static void _InitBxx(RgBxx *pBxx, const char *pszName, int nMode)
{
    if (pBxx == 0) {
        assert_prog(RG_BXX_NOT_NULL, RG_BXX_SOURCE_FILE, 34);
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
        assert_prog(RG_BXX_NOT_NULL, RG_BXX_SOURCE_FILE, 43);
    }
    heap = InstanceOfRgHeap();
    if (RgHeapIsInvalidMemory(heap, pBxx->m_pLink)) {
        RgError(RG_BXX_INVALID_LINK_MEMORY, RG_BXX_SOURCE_FILE, 46, pszFile, nLine, pBxx->m_szName,
                pBxx->m_nMode);
    }
    heap = InstanceOfRgHeap();
    if (RgHeapIsInvalidMemory(heap, pBxx->m_pLoad)) {
        RgError(RG_BXX_INVALID_LOAD_MEMORY, RG_BXX_SOURCE_FILE, 49, pszFile, nLine, pBxx->m_szName,
                pBxx->m_nMode);
    }
    if (pBxx->m_pLink != 0) {
        RgHeapFree(InstanceOfRgHeap(), pBxx->m_pLink, RG_BXX_SOURCE_FILE, 53);
    }
    if (pBxx->m_pLoad != 0) {
        DisposeRgFileSysData_sub(pBxx->m_pLoad, RG_BXX_SOURCE_FILE, 55);
    }
}

RgBxx *CreateRgBxx_sub(const char *pszName, int nMode)
{
    RgHeap *heap;
    RgBxx *pBxx;

    heap = InstanceOfRgHeap();
    pBxx = RgHeapAlloc(heap, sizeof(RgBxx), RG_BXX_SOURCE_FILE, 62);
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
        assert_prog(RG_BXX_NAME_PARAMETER, RG_BXX_SOURCE_FILE, 71);
    }
    pFile = RgFileSysRead(InstanceOfRgFileSys(), pszName, RG_BXX_DATA_ROOT);
    if (pFile == 0) {
        assert_prog(RG_BXX_DATA_REQUIRED, RG_BXX_SOURCE_FILE, 73);
    }
    RgBxxSetData(pBxx, pFile->data);
    pBxx->m_pLoad = pFile;
    return pBxx;
}

void DisposeRgBxx_sub(RgBxx *pBxx, const char *pszFile, int nLine)
{
    if (pBxx == 0) {
        assert_prog(RG_BXX_NOT_NULL, RG_BXX_SOURCE_FILE, 81);
    }
    _DestructBxx(pBxx, pszFile, nLine);
    RgHeapFree(InstanceOfRgHeap(), pBxx, RG_BXX_SOURCE_FILE, 83);
}

void RgBxxSetData(RgBxx *pBxx, void *pData)
{
    RgHeap *heap;
    RgLinkData *pLink;
    RgBxxPic *pic;
    unsigned int count;
    unsigned int picIndex;

    if (pBxx == 0) {
        assert_prog(RG_BXX_NOT_NULL, RG_BXX_SOURCE_FILE, 93);
    }
    if (pBxx->m_pLoad != 0) {
        assert_prog(RG_BXX_LOAD_REQUIRED, RG_BXX_SOURCE_FILE, 94);
    }
    if (pBxx->m_pLink != 0) {
        heap = InstanceOfRgHeap();
        RgHeapFree(heap, pBxx->m_pLink, RG_BXX_SOURCE_FILE, 96);
        pBxx->m_pLink = 0;
    }
    if (pData != 0) {
        heap = InstanceOfRgHeap();
        pLink = RgHeapAlloc(heap, 4, RG_BXX_SOURCE_FILE, 100);
        pBxx->m_pLink = pLink;
        InitRgLinkData(pLink, pData);
        if (RgLinkDataNumOfData(pBxx->m_pLink) != RG_BXX_LINK_DATA_COUNT) {
            RgError(RG_BXX_DATA_FORMAT_ERROR, RG_BXX_SOURCE_FILE, 103,
                    RgLinkDataNumOfData(pBxx->m_pLink));
        }
        count = RgBxxGetTexNum(pBxx);
        pic = _GetPicTop(pBxx);
        picIndex = 0;
        if (count != 0) {
            do {
                picIndex++;
                pic->pOwner = pBxx;
                pic++;
            } while (picIndex < count);
        }
    }
}

static RgBxxHeader *_GetHeader(RgBxx *pBxx)
{
    if (pBxx == 0) {
        assert_prog(RG_BXX_NOT_NULL, RG_BXX_SOURCE_FILE, 124);
    }
    if (pBxx->m_pLink == 0) {
        assert_prog(RG_BXX_LINK_REQUIRED, RG_BXX_SOURCE_FILE, 125);
    }
    return RgLinkDataGetIndex(pBxx->m_pLink, 0);
}

static RgBxxPic *_GetPicTop(RgBxx *pBxx)
{
    RgBxxHeader *header;

    if (pBxx == 0) {
        assert_prog(RG_BXX_NOT_NULL, RG_BXX_SOURCE_FILE, 132);
    }
    if (pBxx->m_pLink == 0) {
        assert_prog(RG_BXX_LINK_REQUIRED, RG_BXX_SOURCE_FILE, 133);
    }
    header = _GetHeader(pBxx);
    return (RgBxxPic *)(header + 1);
}

RgBxxHeader *RgBxxGetHeader(RgBxx *pBxx)
{
    return _GetHeader(pBxx);
}

int RgBxxGetTexNum(RgBxx *pBxx)
{
    return _GetHeader(pBxx)->numTex;
}

RgBxxPic *RgBxxGetPicID(RgBxx *pBxx, unsigned int id)
{
    RgBxxHeader *header;
    RgBxxPic *picTop;
    unsigned int count;

    header = _GetHeader(pBxx);
    picTop = _GetPicTop(pBxx);
    count = header->numTex;
    if (id >= count) {
        RgError(RG_BXX_TEXTURE_COUNT_ERROR, RG_BXX_SOURCE_FILE, 162, id, count);
    }
    return &picTop[id];
}

static int _FindPicByName(RgBxx *pBxx, const char *pszName)
{
    RgBxxHeader *header;
    RgBxxPic *picTop;
    unsigned int nameLength;
    unsigned int picIndex;

    header = _GetHeader(pBxx);
    picTop = _GetPicTop(pBxx);
    if (pszName == 0) {
        assert_prog(RG_BXX_NAME_REQUIRED, RG_BXX_SOURCE_FILE, 175);
    }
    nameLength = strlen(pszName);
    picIndex = 0;
    if (header->numTex != 0) {
        do {
            if (strncmp(pszName,
                        picTop[picIndex].name,
                        nameLength) == 0) {
                return picIndex;
            }
            picIndex++;
        } while (picIndex < (unsigned int)header->numTex);
    }
    return -1;
}

int RgBxxGetFindPic(RgBxx *pBxx, const char *pszName)
{
    int index;

    index = _FindPicByName(pBxx, pszName);
    if (index < 0) {
        RgError(RG_BXX_UNKNOWN_TEXTURE, RG_BXX_SOURCE_FILE, 192, pszName);
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
    return &picTop[index];
}

void *RgBxxGetXtx(RgBxx *pBxx)
{
    if (pBxx == 0) {
        assert_prog(RG_BXX_NOT_NULL, RG_BXX_SOURCE_FILE, 212);
    }
    if (pBxx->m_pLink == 0) {
        assert_prog(RG_BXX_LINK_REQUIRED, RG_BXX_SOURCE_FILE, 213);
    }
    return RgLinkDataGetIndex(pBxx->m_pLink, 1);
}

const char bxxAssertBxxNotNil[16] = "pBxx != NIL";
const char bxxSourcePath[16] = "../rg_bxx.euc.c";
const char bxxInvalidLinkMemoryFormat[48] = "invalid memory (link) %s %d\n(created %s %d)";
const char bxxInvalidLoadMemoryFormat[48] = "invalid memory (load) %s %d\n(created %s %d)";
const char bxxNameParameterName[8] = "pszName";
const char bxxDataDirectory[16] = "data\\nisimori\\";
const char bxxAssertDataNotNil[16] = "pData != NIL";
const char bxxAssertLoadNil[24] = "pBxx->m_pLoad == NIL";
const char bxxDataFormatErrorFormat[40] = "bxx data format error (link num %d)";
const RgBxxMessageBlock bxxAssertMessageBlock = {
    "pBxx->m_pLink != NIL",
    "sizeof(RgBxxPic) == 96",
    "sizeof(RgBxxHeader) == 48",
    ""
};
const char bxxTextureIdErrorFormat[40] = "texture ID error (id=%d count=%d)";
const char bxxAssertNameNotNil[16] = "pszName != NIL";
const char bxxUnknownTextureNameFormat[24] = "unknown texture name %s";
