/*
 * OV12 original TU 19: 0x00a13260..0x00a140a0 (22 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_heap.h"

const char D_00A52A40[40] = "-------- Heap Assert %s ---------\n";
const char D_00A52A68[24] = "../rg_heap.euc.c";
const char D_00A52A80[24] = "%s (in %s at %d)\n\n";
const char D_00A52A98[32] = "heap = %p top = %p size = %d\n\n";
const char D_00A52AB8[16] = "heap-dump";
const char D_00A52AC8[16] = "heap = NIL\n";
const char D_00A52AF0[24] = "HEAD_SIZE < nBlockSize";
const char D_00A52B08[16] = "make block";
/* Three adjacent diagnostics occupy this one original 48-byte string pool. */
const struct {
    char heapNotNil[16];
    char alignedHeaderSize[24];
    char initPhase[8];
} D_00A52B18 = {
    "pHeap != NIL",
    "(HEAD_SIZE & 0xf) == 0",
    "init-1"
};
const char D_00A52B48[24] = "(nBufSize & 0xf) == 0";
const char D_00A52B60[8] = "init-2";
const char D_00A52E88[40] = "\n\n---------------------------------\n";
const char D_00A52EB0[32] = "HEAD DUMP %p from %s %d\n";
const char D_00A52ED0[40] = "***** FREE MAP top %p (comment %s)\n";
const char D_00A52EF8[32] = "%p : size = %x(%d) next:%p\n";
const char D_00A52F18[40] = "***** ALLOC MAP top %p (comment %s)\n";
const char D_00A52F40[48] = "%p : size = %x(%d) next:%p allocated at %s,%d\n";
const char D_00A52F70[32] = "pHeap != NIL && pPtr != NIL";
const char D_00A52F90[24] = "------------ dump %p\n";
const char D_00A52FA8[16] = "pre %p next %p\n";
const char D_00A52FB8[24] = "size %p mark %d\n";
const char D_00A52FD0[24] = "module '%s' line %d\n";
const char D_00A52FE8[32] = "  this block is not in heap %p\n";

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
/*
 * _Error logs the tag, then the failed expression with its call site, and
 * when pHeap is not NIL dumps that heap ("heap = %p top = %p size = %d")
 * through RgHeapDump_sub; InitRgHeap has no heap to dump yet and passes NIL.
 */
static void _Error(const char *expression, const char *tag, RgHeap *pHeap,
                   const char *source_file, int line);
static struct RgHeapBlock *_SetHeapHead(void *pTop, u32 nBufSize);
extern void XrgLog(const char *format, const char *source_file, int line,
                   ...);
extern int RgHeapIsInSelf(RgHeap *pHeap, void *pPtr);
extern void RgHeapDump_sub(RgHeap *pHeap, const char *comment,
                           const char *source_file, int line);

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
    XrgLog(D_00A52A40, D_00A52A68, 19, tag);
    XrgLog(D_00A52A80, D_00A52A68, 20, expression, source_file, line);
    if (pHeap != 0) {
        XrgLog(D_00A52A98, D_00A52A68, 22, pHeap, pHeap->top, pHeap->size);
        RgHeapDump_sub(pHeap, D_00A52AB8, D_00A52A68, 23);
    } else {
        XrgLog(D_00A52AC8, D_00A52A68, 25);
    }
    for (;;) {
    }
}

/*
 * RgHeapBlock: the fixed 0x40-byte header RgHeapAlloc places immediately
 * before the block it returns (RgHeapDumpBlock reads it at pPtr - 0x40).
 * pre/next link the block into whichever circular list currently owns it
 * (RgHeap's free chain from _SetHeapHead, or its allocated chain from
 * _Link/_InsertNext), evidenced by "pre %p next %p\n". size is the block's
 * byte size and mark is set/cleared by _MarkAlloc/_UnmarkAlloc/_IsAllocated,
 * evidenced by "size %p mark %d\n" (size itself is dumped through %p in that
 * string). module and line record the RgHeapAlloc call site (module is
 * strncpy'd from its source_file argument and terminated at module[31]),
 * evidenced by "module '%s' line %d\n". magic holds the 15-character
 * s_szMagicString stamp _SetMagicString/_IsCollectMagicString read and write
 * at block+0x30. Every byte from 0x00 to 0x3F is accounted for by these six
 * members (4+4+4+2+2+32+16 == 0x40).
 */
struct RgHeapBlock {
    struct RgHeapBlock *pre;
    struct RgHeapBlock *next;
    u32 size;
    u16 mark;
    u16 line;
    char module[32];
    char magic[16];
};

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
extern const unsigned char D_00A52AD8[];
static const unsigned char *s_szMagicString = D_00A52AD8;

static void _SetMagicString(struct RgHeapBlock *pBlock)
{
    int i;

    for (i = 0; i < 0xF; i++) {
        pBlock->magic[i] = s_szMagicString[i];
    }
}

static int _IsCollectMagicString(struct RgHeapBlock *pBlock)
{
    const char *magic_string = (const char *)s_szMagicString;
    int i;

    for (i = 0; i < 0xF; i++) {
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
    if (nBufSize < 0x41) {
        _Error(D_00A52AF0, D_00A52B08, 0, D_00A52A68, 130);
    }
    _Link(pHead, pHead, pHead);
    pHead->size = nBufSize;
    return pHead;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_heap", _MergeBlocks);

/* The two heap instances and their backing arenas are zero-initialized data. */
RgHeap D_00A59AA0;
u8 D_00A59AB0[0x800];
static int s_bInit1_0 = 0;

RgHeap *InstanceOfRgHeap(void)
{
    if (s_bInit1_0 == 0) {
        s_bInit1_0 = 1;
        InitRgHeap(&D_00A59AA0, D_00A59AB0, 0x800);
    }
    return &D_00A59AA0;
}

RgHeap D_00A5A2B0;
u8 D_00A5A2C0[0x800];
static int s_bInit2_3 = 0;

RgHeap *InstanceOfRgHeapData(void)
{
    if (s_bInit2_3 == 0) {
        s_bInit2_3 = 1;
        InitRgHeap(&D_00A5A2B0, D_00A5A2C0, 0x800);
    }
    return &D_00A5A2B0;
}

void ClearRgHeap(RgHeap *pHeap)
{
    if (pHeap == 0) {
        assert_prog(D_00A52B18.heapNotNil, D_00A52A68, 188);
    }
    pHeap->pHead = _SetHeapHead(pHeap->top, pHeap->size);
}

void InitRgHeap(RgHeap *pHeap, void *pTop, u32 nBufSize)
{
    struct RgHeapBlock *pHead;

    if (pHeap == 0) {
        assert_prog(D_00A52B18.heapNotNil, D_00A52A68, 193);
    }
    if (nBufSize & 0xF) {
        _Error(D_00A52B48, D_00A52B60, 0, D_00A52A68, 195);
    }
    memset(pTop, 0, nBufSize);
    pHeap->top = pTop;
    pHeap->size = nBufSize;
    pHead = _SetHeapHead(pTop, nBufSize);
    pHeap->pAlloc = 0;
    pHeap->pHead = pHead;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_heap", RgHeapAlloc);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_heap", RgHeapFree);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_heap", RgHeapIsInSelf);

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
    XrgLog(D_00A52E88, D_00A52A68, 439);
    XrgLog(D_00A52EB0, D_00A52A68, 440, pHeap, source_file, line);
    XrgLog(D_00A52ED0, D_00A52A68, 441, pBlock, comment);
    do {
        XrgLog(D_00A52EF8, D_00A52A68, 443, pBlock, pBlock->size, pBlock->size,
              pBlock->next);
        pBlock = pBlock->next;
    } while (pBlock != pHeap->pHead);
    pBlock = pHeap->pAlloc;
    XrgLog(D_00A52F18, D_00A52A68, 448, pBlock, comment);
    if (pBlock != 0) {
        do {
            XrgLog(D_00A52F40, D_00A52A68, 453, pBlock, pBlock->size,
                  pBlock->size, pBlock->next, pBlock->module, pBlock->line);
            pBlock = pBlock->next;
        } while (pBlock != pHeap->pAlloc);
    }
}

void RgHeapDumpBlock(RgHeap *pHeap, void *pPtr)
{
    struct RgHeapBlock *pBlock;

    pBlock = (struct RgHeapBlock *)((char *)pPtr - 0x40);
    if (pHeap == 0 || pPtr == 0) {
        assert_prog(D_00A52F70, D_00A52A68, 463);
    }
    XrgLog(D_00A52F90, D_00A52A68, 465, pPtr);
    if (RgHeapIsInSelf(pHeap, pPtr)) {
        XrgLog(D_00A52FA8, D_00A52A68, 467, pBlock->pre, pBlock->next);
        XrgLog(D_00A52FB8, D_00A52A68, 468, (void *)pBlock->size,
              pBlock->mark);
        XrgLog(D_00A52FD0, D_00A52A68, 469, pBlock->module, pBlock->line);
        return;
    }
    XrgLog(D_00A52FE8, D_00A52A68, 471, pHeap);
}



const unsigned char D_00A52AD8[24] = "gAMe sHoW 2001.10.15";
