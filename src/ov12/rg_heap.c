/*
 * OV12 original TU 19: 0x00a13260..0x00a140a0 (22 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_heap.h"
#include "ov12/assert_prog.h"
#include "ov12/xrg_v_log.h"
#include "ov12/rg_error.h"

#define RG_HEAP_ARENA_SIZE 0x800

/* Defined here: the strings and the heap singletons' arenas (below). */
extern const char heapAllocZeroSizeFormat[];
extern const char heapAssertHeapNotNilParenthesized[];
extern const char heapAllocEntryHeapPtrComment[];
extern const char heapAllocNoHeapFormat[];
extern const char heapNoDataHeapComment[];
extern const char heapAllocTooBigDataHeapFormat[];
extern const char heapNoNormalHeapComment[];
extern const char heapAllocTooBigNormalHeapFormat[];
extern const char heapNoUnknownHeapComment[];
extern const char heapAllocTooBigUnknownHeapFormat[];
extern const char heapAssertMatchBlockSizeEnough[];
extern const char heapBlockAlignComment[];
extern const char heapAssertAllocAligned[];
extern const char heapNotAlignedBlockComment[];
extern const char heapFreeEntryComment[];
extern const char heapFreeNilAddressFormat[];
extern const char heapErrorComment[];
extern const char heapAlreadyFreeFormat[];
extern const char heapNotAllocatedFreeFormat[];
extern const char heapBlockDestroyedFormat[];
extern const char heapPtrNotInAreaFormat[];
extern const char heapAllocListNodeFreeBlockMismatchFormat[];
extern const unsigned char heapMagicString[];


/*
 * _Error logs the tag, then the failed expression with its call site, and
 * when pHeap is not NIL dumps that heap ("heap = %p top = %p size = %d")
 * through RgHeapDump_sub; InitRgHeap has no heap to dump yet and passes NIL.
 */
static void _Error(const char *expression, const char *tag, RgHeap *pHeap,
                   const char *source_file, int line);
static struct RgHeapBlock *_SetHeapHead(void *pTop, u32 nBufSize);

const char heapAssertHeaderFormat[40] = "-------- Heap Assert %s ---------\n";

const char heapSourcePath[24] = "../rg_heap.euc.c";

const char heapAssertExpressionFormat[24] = "%s (in %s at %d)\n\n";

const char heapAssertHeapInfoFormat[32] = "heap = %p top = %p size = %d\n\n";

const char heapAssertDumpComment[16] = "heap-dump";

const char heapAssertHeapNilFormat[16] = "heap = NIL\n";

const char heapAssertHeadSizeBelowBlockSize[24] = "HEAD_SIZE < nBlockSize";

const char heapMakeBlockComment[16] = "make block";

/* Three adjacent diagnostics occupy this one original 48-byte string pool. */
const struct {
    char heapNotNil[16];
    char alignedHeaderSize[24];
    char initPhase[8];
} heapErrorTexts = {
    "pHeap != NIL",
    "(HEAD_SIZE & 0xf) == 0",
    "init-1"
};

const char heapAssertBufSizeAligned[24] = "(nBufSize & 0xf) == 0";

const char heapInitStageComment[8] = "init-2";

const char heapDumpSeparatorFormat[40] = "\n\n---------------------------------\n";

const char heapDumpHeadFormat[32] = "HEAD DUMP %p from %s %d\n";

const char heapDumpFreeMapFormat[40] = "***** FREE MAP top %p (comment %s)\n";

const char heapDumpFreeBlockFormat[32] = "%p : size = %x(%d) next:%p\n";

const char heapDumpAllocMapFormat[40] = "***** ALLOC MAP top %p (comment %s)\n";

const char heapDumpAllocBlockFormat[48] = "%p : size = %x(%d) next:%p allocated at %s,%d\n";

const char heapAssertHeapAndPtrNotNil[32] = "pHeap != NIL && pPtr != NIL";

const char heapDumpBlockHeaderFormat[24] = "------------ dump %p\n";

const char heapDumpBlockLinksFormat[16] = "pre %p next %p\n";

const char heapDumpBlockSizeMarkFormat[24] = "size %p mark %d\n";

const char heapDumpBlockModuleFormat[24] = "module '%s' line %d\n";

const char heapDumpBlockNotInHeapFormat[32] = "  this block is not in heap %p\n";

/*
 * External file-backed witnesses, not candidate-emitted data: this window is
 * asm-owned scaffold data (splat names, no config/symbols/ov12.txt entry).
 *
 * ov12:0x00a52a68 contains the source filename "../rg_heap.euc.c".
 * ov12:0x00a52b18 contains the assertion expression "pHeap != NIL".
 * ov12:0x00a52b48 contains the assertion expression "(nBufSize & 0xf) == 0".
 * ov12:0x00a52b60 contains the tag "init-2".
 * ov12:0x00a52f70 contains the assertion expression
 *      "pHeap != NIL && pPtr != NIL".
 * ov12:0x00a52f90 contains the format string "------------ dump %p\n".
 * ov12:0x00a52fa8 contains the format string "pre %p next %p\n".
 * ov12:0x00a52fb8 contains the format string "size %p mark %d\n".
 * ov12:0x00a52fd0 contains the format string "module '%s' line %d\n".
 * ov12:0x00a52fe8 contains the format string
 *      "  this block is not in heap %p\n".
 */
static void _Error(const char *expression, const char *tag, RgHeap *pHeap,
                   const char *source_file, int line)
{
    XrgLog(heapAssertHeaderFormat, heapSourcePath, 19, tag);
    XrgLog(heapAssertExpressionFormat, heapSourcePath, 20, expression, source_file, line);
    if (pHeap != 0) {
        XrgLog(heapAssertHeapInfoFormat, heapSourcePath, 22, pHeap, pHeap->top, pHeap->size);
        RgHeapDump_sub(pHeap, heapAssertDumpComment, heapSourcePath, 23);
    } else {
        XrgLog(heapAssertHeapNilFormat, heapSourcePath, 25);
    }
    for (;;) {
    }
}

static void _Link(struct RgHeapBlock *pBlock, struct RgHeapBlock *pPre,
                  struct RgHeapBlock *pNext)
{
    pBlock->pre = pPre;
    pBlock->next = pNext;
}

static void _InsertPre(struct RgHeapBlock *pBlock, struct RgHeapBlock *pNew)
{
    _Link(pNew, pBlock->pre, pBlock);
    pBlock->pre->next = pNew;
    pBlock->pre = pNew;
}

static void _InsertNext(struct RgHeapBlock *pBlock, struct RgHeapBlock *pNew)
{
    _Link(pNew, pBlock, pBlock->next);
    pBlock->next->pre = pNew;
    pBlock->next = pNew;
}

static void _Unlink(struct RgHeapBlock *pBlock)
{
    pBlock->next->pre = pBlock->pre;
    pBlock->pre->next = pBlock->next;
}

static void _MarkAlloc(struct RgHeapBlock *pBlock)
{
    pBlock->mark = 1;
}

static void _UnmarkAlloc(struct RgHeapBlock *pBlock)
{
    pBlock->mark = 0;
}

static int _IsAllocated(struct RgHeapBlock *pBlock)
{
    return pBlock->mark != 0;
}

/*
 * s_szMagicString is a pointer to the 15-character stamp copied into every
 * block's magic field (lw of the symbol itself, not an inline array).
 */
static const unsigned char *s_szMagicString = heapMagicString;

static void _SetMagicString(struct RgHeapBlock *pBlock)
{
    int i;

    for (i = 0; i < RG_HEAP_MAGIC_LENGTH; i++) {
        pBlock->magic[i] = s_szMagicString[i];
    }
}

static int _IsCollectMagicString(struct RgHeapBlock *pBlock)
{
    const char *magic_string = (const char *)s_szMagicString;
    int i;

    for (i = 0; i < RG_HEAP_MAGIC_LENGTH; i++) {
        if ((unsigned char)pBlock->magic[i] != magic_string[i]) {
            return 0;
        }
    }
    return 1;
}

/*
 * ov12:0x00a52af0 contains the assertion expression
 *      "HEAD_SIZE < nBlockSize".
 * ov12:0x00a52b08 contains the tag "make block".
 */
static struct RgHeapBlock *_SetHeapHead(void *pTop, u32 nBufSize)
{
    struct RgHeapBlock *pHead;

    pHead = (struct RgHeapBlock *)pTop;
    if (nBufSize <= RG_HEAP_HEAD_SIZE) {
        _Error(heapAssertHeadSizeBelowBlockSize, heapMakeBlockComment, 0, heapSourcePath, 130);
    }
    _Link(pHead, pHead, pHead);
    pHead->size = nBufSize;
    return pHead;
}

/*
 * Walks the circular free list once and folds every block that ends exactly
 * where its successor starts into that block.
 */
static void _MergeBlocks(struct RgHeapBlock *pHead)
{
    struct RgHeapBlock *pBlock = pHead;
    struct RgHeapBlock *pNext;

    if (pHead == 0) {
        return;
    }
    do {
        pNext = pBlock->next;
        while ((u8 *)pBlock + pBlock->size == (u8 *)pNext) {
            pBlock->next = pNext->next;
            pBlock->size += pNext->size;
            pNext->next->pre = pBlock;
            pNext = pBlock->next;
        }
        pBlock = pNext;
    } while (pBlock != pHead);
}

RgHeap *InstanceOfRgHeap(void)
{
    static int s_bInit1 = 0;
    static RgHeap s_inHeap;
    static u8 s_aTmp[RG_HEAP_ARENA_SIZE];

    if (s_bInit1 == 0) {
        s_bInit1 = 1;
        InitRgHeap(&s_inHeap, s_aTmp, RG_HEAP_ARENA_SIZE);
    }
    return &s_inHeap;
}

RgHeap *InstanceOfRgHeapData(void)
{
    static int s_bInit2 = 0;
    static RgHeap s_inHeap;
    static u8 s_aTmp[RG_HEAP_ARENA_SIZE];

    if (s_bInit2 == 0) {
        s_bInit2 = 1;
        InitRgHeap(&s_inHeap, s_aTmp, RG_HEAP_ARENA_SIZE);
    }
    return &s_inHeap;
}

void ClearRgHeap(RgHeap *pHeap)
{
    if (pHeap == 0) {
        assert_prog(heapErrorTexts.heapNotNil, heapSourcePath, 188);
    }
    pHeap->pHead = _SetHeapHead(pHeap->top, pHeap->size);
}

void InitRgHeap(RgHeap *pHeap, void *pTop, u32 nBufSize)
{
    struct RgHeapBlock *pHead;

    if (pHeap == 0) {
        assert_prog(heapErrorTexts.heapNotNil, heapSourcePath, 193);
    }
    if (nBufSize & RG_HEAP_ALIGN_MASK) {
        _Error(heapAssertBufSizeAligned, heapInitStageComment, 0, heapSourcePath, 195);
    }
    memset(pTop, 0, nBufSize);
    pHeap->top = pTop;
    pHeap->size = nBufSize;
    pHead = _SetHeapHead(pTop, nBufSize);
    pHeap->pAlloc = 0;
    pHeap->pHead = pHead;
}

void *RgHeapAlloc(RgHeap *pHeap, u32 nSize, const char *source_file, int line)
{
    struct RgHeapBlock *pHead;
    struct RgHeapBlock *pCur;
    struct RgHeapBlock *pBest;
    struct RgHeapBlock *pNew;
    u32 nNeed;
    u32 nBestSize;
    u32 nRemain;
    u32 nBlockSize;
    void *pPtr;

    if (nSize == 0) {
        RgError(heapAllocZeroSizeFormat, heapSourcePath, 217, source_file, line);
    }
    if (pHeap == 0) {
        _Error(heapAssertHeapNotNilParenthesized, heapAllocEntryHeapPtrComment, 0, heapSourcePath, 218);
    }
    pHead = pHeap->pHead;
    if (pHead == 0) {
        RgError(heapAllocNoHeapFormat, heapSourcePath, 223, source_file, line);
        return 0;
    }
    nNeed = ((nSize + RG_HEAP_ALIGN_MASK) & ~RG_HEAP_ALIGN_MASK) +
            sizeof(struct RgHeapBlock);

    pBest = 0;
    nBestSize = 0;
    pCur = pHead;
    do {
        if (pCur->size >= nNeed && (pCur->size < nBestSize || nBestSize == 0)) {
            nBestSize = pCur->size;
            pBest = pCur;
        }
        pCur = pCur->next;
    } while (pCur != pHead);

    if (pBest == 0) {
        if (pHeap == InstanceOfRgHeapData()) {
            RgHeapDump_sub(pHeap, heapNoDataHeapComment, heapSourcePath, 245);
            RgError(heapAllocTooBigDataHeapFormat, heapSourcePath, 246, nSize, source_file, line);
        } else if (pHeap == InstanceOfRgHeap()) {
            RgHeapDump_sub(pHeap, heapNoNormalHeapComment, heapSourcePath, 248);
            RgError(heapAllocTooBigNormalHeapFormat, heapSourcePath, 249, nSize, source_file, line);
        } else {
            RgHeapDump_sub(pHeap, heapNoUnknownHeapComment, heapSourcePath, 251);
            RgError(heapAllocTooBigUnknownHeapFormat, heapSourcePath, 252, nSize, source_file, line);
        }
        return 0;
    }
    if (nBestSize < nNeed) {
        _Error(heapAssertMatchBlockSizeEnough, heapBlockAlignComment, pHeap, heapSourcePath, 262);
    }
    nRemain = nBestSize - nNeed;
    if (nRemain > RG_HEAP_HEAD_SIZE) {
        nBlockSize = nNeed;
        pNew = (struct RgHeapBlock *)((u8 *)pBest + nNeed);
        _SetHeapHead(pNew, nRemain);
    } else {
        nBlockSize = nBestSize;
        pNew = 0;
    }
    if (pNew != 0) {
        _InsertNext(pBest, pNew);
    }
    if (pBest == pHead) {
        pHeap->pHead = pBest->next;
    }
    _Unlink(pBest);
    _SetHeapHead(pBest, nBlockSize);
    _MarkAlloc(pBest);

    if (source_file != 0) {
        strncpy(pBest->module, source_file, RG_HEAP_MODULE_LENGTH);
        pBest->module[RG_HEAP_MODULE_LENGTH] = 0;
        pBest->line = line;
    } else {
        pBest->module[0] = 0;
        pBest->line = 0;
    }
    _SetMagicString(pBest);

    if (pHeap->pAlloc == 0) {
        pHeap->pAlloc = pBest;
        _Link(pBest, pBest, pBest);
    } else {
        _InsertNext(pHeap->pAlloc, pBest);
    }
    pPtr = (void *)(pBest + 1);
    if ((u32)pPtr & RG_HEAP_ALIGN_MASK) {
        _Error(heapAssertAllocAligned, heapNotAlignedBlockComment, pHeap, heapSourcePath, 304);
    }
    return pPtr;
}

void RgHeapFree(RgHeap *pHeap, void *pPtr, const char *source_file, int line)
{
    struct RgHeapBlock *pHdr = (struct RgHeapBlock *)pPtr - 1;
    struct RgHeapBlock *pCur;
    struct RgHeapBlock *pAlloc;
    struct RgHeapBlock *pNext;

    if (pHeap == 0) {
        _Error(heapErrorTexts.heapNotNil, heapFreeEntryComment, pHeap, heapSourcePath, 316);
    }
    if (pPtr == 0) {
        RgError(heapFreeNilAddressFormat, heapSourcePath, 320, source_file, line);
    }
    if (!_IsAllocated(pHdr)) {
        if (_IsCollectMagicString(pHdr)) {
            RgHeapDump_sub(pHeap, heapErrorComment, heapSourcePath, 329);
            RgError(heapAlreadyFreeFormat, heapSourcePath, 335, pPtr, source_file, line,
                    pHdr->module, pHdr->line);
        } else {
            RgError(heapNotAllocatedFreeFormat, heapSourcePath, 339, pPtr, source_file, line);
        }
    }
    if (!_IsCollectMagicString(pHdr)) {
        RgError(heapBlockDestroyedFormat, heapSourcePath, 345, pPtr, source_file, line);
    }
    if (!RgHeapIsInSelf(pHeap, pHdr)) {
        RgError(heapPtrNotInAreaFormat, heapSourcePath, 350, pHeap, source_file, line);
    }
    pAlloc = pHeap->pAlloc;
    pNext = pAlloc->next;
    if (pNext == pAlloc) {
        if (pHdr != pNext) {
            RgError(heapAllocListNodeFreeBlockMismatchFormat, heapSourcePath, 359);
        }
        pHeap->pAlloc = 0;
    } else {
        if (pAlloc == pHdr) {
            _Unlink(pHdr);
            pHeap->pAlloc = pNext;
        } else {
            _Unlink(pHdr);
        }
    }
    _UnmarkAlloc(pHdr);
    if (pHeap->pHead == 0) {
        pHeap->pHead = _SetHeapHead(pHdr, pHdr->size);
        return;
    }
    pCur = pHeap->pHead;
    _Link(pHdr, 0, 0);
    do {
        if (pHdr < pCur) {
            _InsertPre(pCur, pHdr);
            if (pHdr < pHeap->pHead) {
                pHeap->pHead = pHdr;
            }
            break;
        }
        pCur = pCur->next;
    } while (pCur != pHeap->pHead);
    if (pHdr->next == 0) {
        _InsertPre(pHeap->pHead, pHdr);
    }
    _MergeBlocks(pHeap->pHead);
}

int RgHeapIsInSelf(RgHeap *pHeap, void *pPtr)
{
    if (pHeap == 0) {
        assert_prog(heapErrorTexts.heapNotNil, heapSourcePath, 403);
    }
    if ((u8 *)pPtr >= (u8 *)pHeap->top && (u8 *)pPtr < (u8 *)pHeap->top + pHeap->size) {
        return 1;
    }
    return 0;
}

int RgHeapIsInvalidMemory(RgHeap *pHeap, void *pPtr)
{
    struct RgHeapBlock *pBlock;

    if (pPtr == 0) {
        return 1;
    }
    pBlock = (struct RgHeapBlock *)pPtr - 1;
    if (!_IsAllocated(pBlock)) {
        return 1;
    }
    if (!_IsCollectMagicString(pBlock)) {
        return 1;
    }
    return !RgHeapIsInSelf(pHeap, pBlock);
}

/*
 * ov12:0x00a52e88 contains the format string
 *      "\n\n---------------------------------\n".
 * ov12:0x00a52eb0 contains the format string "HEAD DUMP %p from %s %d\n".
 * ov12:0x00a52ed0 contains the format string
 *      "***** FREE MAP top %p (comment %s)\n".
 * ov12:0x00a52ef8 contains the format string
 *      "%p : size = %x(%d) next:%p\n".
 * ov12:0x00a52f18 contains the format string
 *      "***** ALLOC MAP top %p (comment %s)\n".
 * ov12:0x00a52f40 contains the format string
 *      "%p : size = %x(%d) next:%p allocated at %s,%d\n".
 */
void RgHeapDump_sub(RgHeap *pHeap, const char *comment, const char *source_file,
                    int line)
{
    struct RgHeapBlock *pBlock;

    pBlock = pHeap->pHead;
    XrgLog(heapDumpSeparatorFormat, heapSourcePath, 439);
    XrgLog(heapDumpHeadFormat, heapSourcePath, 440, pHeap, source_file, line);
    XrgLog(heapDumpFreeMapFormat, heapSourcePath, 441, pBlock, comment);
    do {
        XrgLog(heapDumpFreeBlockFormat, heapSourcePath, 443, pBlock, pBlock->size, pBlock->size,
              pBlock->next);
        pBlock = pBlock->next;
    } while (pBlock != pHeap->pHead);
    pBlock = pHeap->pAlloc;
    XrgLog(heapDumpAllocMapFormat, heapSourcePath, 448, pBlock, comment);
    if (pBlock != 0) {
        do {
            XrgLog(heapDumpAllocBlockFormat, heapSourcePath, 453, pBlock, pBlock->size,
                  pBlock->size, pBlock->next, pBlock->module, pBlock->line);
            pBlock = pBlock->next;
        } while (pBlock != pHeap->pAlloc);
    }
}

void RgHeapDumpBlock(RgHeap *pHeap, void *pPtr)
{
    struct RgHeapBlock *pBlock;

    pBlock = (struct RgHeapBlock *)pPtr - 1;
    if (pHeap == 0 || pPtr == 0) {
        assert_prog(heapAssertHeapAndPtrNotNil, heapSourcePath, 463);
    }
    XrgLog(heapDumpBlockHeaderFormat, heapSourcePath, 465, pPtr);
    if (RgHeapIsInSelf(pHeap, pPtr)) {
        XrgLog(heapDumpBlockLinksFormat, heapSourcePath, 467, pBlock->pre, pBlock->next);
        XrgLog(heapDumpBlockSizeMarkFormat, heapSourcePath, 468, (void *)pBlock->size,
              pBlock->mark);
        XrgLog(heapDumpBlockModuleFormat, heapSourcePath, 469, pBlock->module, pBlock->line);
        return;
    }
    XrgLog(heapDumpBlockNotInHeapFormat, heapSourcePath, 471, pHeap);
}

const unsigned char heapMagicString[24] = "gAMe sHoW 2001.10.15";



const char heapAllocZeroSizeFormat[40] = "size <= 0 error allocated at %s,%d";

const char heapAssertHeapNotNilParenthesized[16] = "(pHeap != NIL)";

const char heapAllocEntryHeapPtrComment[24] = "alloc-entry-heap-ptr";

const char heapAllocNoHeapFormat[48] = "there is no heap for allocation at %s,%d";

const char heapNoDataHeapComment[16] = "no heap 1";

const char heapAllocTooBigDataHeapFormat[56] = "too big size (%d) of allocation\nat %s,%d\nfrom data heap";

const char heapNoNormalHeapComment[16] = "no heap 2";

const char heapAllocTooBigNormalHeapFormat[64] = "too big size (%d) of allocation\nat %s,%d\nfrom normal heap";

const char heapNoUnknownHeapComment[16] = "no heap 3";

const char heapAllocTooBigUnknownHeapFormat[64] = "too big size (%d) of allocation\nat %s,%d\nfrom unknown heap";

const char heapAssertMatchBlockSizeEnough[40] = "(nMatchBlockSiz >= nNeedBlockSiz)";

const char heapBlockAlignComment[16] = "block align";

const char heapAssertAllocAligned[48] = "(((int)(BYTE*)pAlloc + HEAD_SIZE) & 0xf) == 0";

const char heapNotAlignedBlockComment[24] = "not aligned block";

const char heapFreeEntryComment[16] = "free entry";

const char heapFreeNilAddressFormat[32] = "free NIL address at %s,%d";

const char heapErrorComment[16] = "heap error";

const char heapAlreadyFreeFormat[64] = "%p is already free at %s,%d\n(MAYBE allocated at %s,%d !\?)";

const char heapNotAllocatedFreeFormat[32] = "not allocated %p free at %s,%d";

const char heapBlockDestroyedFormat[56] = "heap block is destroyed or not in heap %p free at %s,%d";

const char heapPtrNotInAreaFormat[48] = "%p is not in %p HEAP memory area\nfree at %s %d";

const char heapAllocListNodeFreeBlockMismatchFormat[48] = "alloc list node 1 && free block is not this";
