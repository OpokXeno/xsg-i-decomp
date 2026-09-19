/*
 * OV12 original TU 19: 0x00a13260..0x00a140a0 (22 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_heap.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
/*
 * _Error logs the tag, then the failed expression with its call site, and
 * when pHeap is not NIL dumps that heap ("heap = %p top = %p size = %d")
 * through RgHeapDump_sub; InitRgHeap has no heap to dump yet and passes NIL.
 */
extern void _Error(const char *expression, const char *tag, RgHeap *pHeap,
                   const char *source_file, int line);
extern struct RgHeapBlock *_SetHeapHead(void *pTop, u32 nBufSize);
extern void XrgLog(const char *format, const char *source_file, int line,
                   ...);
extern int RgHeapIsInSelf(RgHeap *pHeap, void *pPtr);

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
extern const char D_00A52A68[];
extern const char D_00A52B18[];
extern const char D_00A52B48[];
extern const char D_00A52B60[];
extern const char D_00A52F70[];
extern const char D_00A52F90[];
extern const char D_00A52FA8[];
extern const char D_00A52FB8[];
extern const char D_00A52FD0[];
extern const char D_00A52FE8[];

INCLUDE_ASM("asm/nonmatchings/ov12/rg_heap", _Error_00A13260);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_heap", _InsertPre);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_heap", _InsertNext);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_heap", _Unlink);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_heap", _SetMagicString);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_heap", _IsCollectMagicString);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_heap", _SetHeapHead);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_heap", _MergeBlocks);

/*
 * ov12:0x00a59aa0 (0x10 bytes) and 0x00a5a2b0 (0x10 bytes) are the two
 * RgHeap singletons; 0x00a59ab0 and 0x00a5a2c0 (0x800 bytes each) are the
 * raw memory arenas InitRgHeap hands to _SetHeapHead. asm-owned scaffold
 * data, no config/symbols/ov12.txt entry.
 */
extern RgHeap D_00A59AA0;
extern u8 D_00A59AB0[0x800];
extern int s_bInit1_0;

RgHeap *InstanceOfRgHeap(void)
{
    if (s_bInit1_0 == 0) {
        s_bInit1_0 = 1;
        InitRgHeap(&D_00A59AA0, D_00A59AB0, 0x800);
    }
    return &D_00A59AA0;
}

extern RgHeap D_00A5A2B0;
extern u8 D_00A5A2C0[0x800];
extern int s_bInit2_3;

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
        assert_prog(D_00A52B18, D_00A52A68, 188);
    }
    pHeap->pHead = _SetHeapHead(pHeap->top, pHeap->size);
}

void InitRgHeap(RgHeap *pHeap, void *pTop, u32 nBufSize)
{
    struct RgHeapBlock *pHead;

    if (pHeap == 0) {
        assert_prog(D_00A52B18, D_00A52A68, 193);
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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_heap", RgHeapIsInvalidMemory);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_heap", RgHeapDump_sub);

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
