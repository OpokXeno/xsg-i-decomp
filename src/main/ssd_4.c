#include "common.h"

#include "ssd_4.h"

typedef struct RssdRpcResponse {
    short error_code;              /* +0x00: negative -> per-command error */
    short result;                  /* +0x02: nonzero -> error status (-1) */
    int _unmodeled_04;             /* +0x04: 32-bit word, meaning not recovered */
    int value;                     /* +0x08: command result value */
    int _unmodeled_0c[5];          /* +0x0c..0x1f: 32-bit words, not read here */
} RssdRpcResponse;

/* canon: config/header-canon.json chose src/core/main-00240188/private.h over 1 other accepted spelling */
/*
 * RssdWork is the RSSD RPC / background-wave work area, main:0x004aa080,
 * ELF symbol size 0x200 bytes. The accepted RssdWorkFlags in
 * xeno/core/types.h (from RssdBackgroundNextWave, main:0x0023ff28) only
 * carries the leading `flags` word; this function evidences more of the same
 * object, so the type is extended here under the same tag and leading field
 * Only the evidenced members are modeled here. Bytes this TU does not access
 * stay explicit unmodeled spans.
 */
struct SsdMemoryBlock;

typedef struct RssdWorkFlags {
    int flags;                            /* +0x000: bit 3 cleared on RPC completion; bit 5 is the success status */
    unsigned char _unmodeled_004[4];      /* +0x004..0x007 */
    int request_parameter;                /* +0x008: copied by RssdBusy from its request */
    unsigned char _unmodeled_00c[4];      /* +0x00c..0x00f */
    unsigned short sample_rate;           /* +0x010: samples per second used by SsdGetTimeCode */
    short request_channel_count;          /* +0x012: copied by RssdBusy from its request */
    int request_size;                     /* +0x014: copied by RssdBusy from its request */
    unsigned char _unmodeled_018[0x08];   /* +0x018..0x01f */
    /*
     * Stored verbatim by SsdSetServerCallback (main:0x002403b0)
     * `sw $5,36($2)` / `sw $4,32($2)`, $2 = &RssdWork. No recovered function
     * reads them back, so their call signature is not evidenced and they
     * stay untyped pointers named after the store site.
     */
    void *server_callback;                /* +0x020: SsdSetServerCallback arg0 */
    void *server_callback_arg;            /* +0x024: SsdSetServerCallback arg1 */
    void (*complete_callback)(int status, RssdRpcResponse *response,
                              void *arg); /* +0x028: one-shot, cleared after use */
    void *callback_arg;                   /* +0x02c: third complete_callback argument */
    unsigned char _unmodeled_030[0x04];   /* +0x030..0x033 */
    RssdRpcResponse *response_source;     /* +0x034: SIF RPC receive buffer */
    unsigned char _unmodeled_038[0x48];   /* +0x038..0x07f */
    RssdRpcResponse response;             /* +0x080..0x09f: copy of *response_source */
    unsigned char _unmodeled_0a0[0xa8];   /* +0x0a0..0x147 */
    /*
     * The SCE SDK RPC server registration record: RssdInitIop
     * (main:0x0023fbb8, still asm) passes its address as the `sd` argument of
     * sceSifRegisterRpc (`addiu $4,$17,0x148`), and SsdQuit (main:0x0023fb08)
     * passes the same address to sceSifRemoveRpc to unregister it on the way
     * out. Its internal layout belongs to the SIF RPC middleware, not this
     * TU, so the reserved extent runs to the next evidenced field at +0x18c.
     */
    unsigned char rpc_server[0x44];       /* +0x148..0x18b: sceSifRpcServerData_t */
    /*
     * The SIF RPC receive queue RSsdSifRpcThread (main:0x0023fb90) hands to
     * sceSifRpcLoop after RssdInitIop returns (`addiu a0,v0,-24052` off
     * `lui v0,0x4b`, v0 = &RssdWork, i.e. &RssdWork + 0x18c). Its internal
     * layout belongs to the SIF RPC middleware, not this TU, and is not
     * evidenced by any recovered function; the reserved extent runs to the
     * next evidenced field at +0x1a4.
     */
    unsigned char rpc_queue[0x18];        /* +0x18c..0x1a3: sceSifRpcLoop queue */
    /*
     * The two service threads SsdInit (main:0x0023f960) creates, each stored
     * as the (thread id, stack) pair the create/start idiom produces
     * ($20 = &RssdWork throughout that function):
     * - +0x1a4/+0x1a8: RSsdSifRpcThread, `sw $2,0x1a4($20)` in the delay slot
     *   of `jal StartThread` at 0x0023fa88 and `sw $5,0x1a8($20)` at
     *   0x0023fa74 with $5 = MYthreadStack;
     * - +0x1ac/+0x1b0: RssdBackNextWaveThread, `sw $2,0x1ac($20)` in the
     *   delay slot of `jal StartThread` at 0x0023facc and `sw $3,0x1b0($20)`
     *   in the delay slot of its `jal CreateThread` at 0x0023fabc with
     *   $3 = MYwaveTransThStack.
     * Both ids are confirmed by a second, independent reader: SsdQuit
     * (main:0x0023fb08) passes +0x1a4 to TerminateThread and DeleteThread
     * (0x0023fb5c/0x0023fb64) and +0x1ac to the same pair
     * (0x0023fb6c/0x0023fb74), next to its DeleteSema of +0x1b4.
     * RssdBackgroundNextWave wakes +0x1ac, which is that same thread.
     */
    int rpc_thread_id;                    /* +0x1a4: RSsdSifRpcThread */
    void *rpc_thread_stack;               /* +0x1a8: MYthreadStack */
    int next_wave_thread_id;              /* +0x1ac: RssdBackNextWaveThread */
    void *next_wave_thread_stack;         /* +0x1b0: MYwaveTransThStack */
    int sema_id;                          /* +0x1b4: signalled when the RPC completes */
    int busy_sema_id;                     /* +0x1b8: signalled by RssdBusy */
    unsigned char _unmodeled_1bc[4];      /* +0x1bc..0x1bf */
    struct SsdMemoryBlock *first_block;   /* +0x1c0: first allocator-list node */
    int memory_end;                       /* +0x1c4: allocator arena end */
    unsigned char _unmodeled_1c8[4];     /* +0x1c8..0x1cb */
    int spu_bytes_remaining;              /* +0x1cc: drained by RssdBackNextWaveThread */
    /*
     * Running destination pointer for streamed sample data. RssdSpuRead
     * (main:0x0023feb8) copies each request's payload here with
     * SsdCopyMemory and advances it by the copied byte count; SsdSpuDirectRead
     * (main:0x00240690, still asm) sets it from its own destination argument.
     */
    unsigned char *spu_write_ptr;         /* +0x1d0: streamed sample write position */
    int wave_id;                          /* +0x1d4: active wave id */
    unsigned char _unmodeled_1d8[4];
    unsigned short wave_chunk_index;     /* +0x1dc: streamed chunk index */
    unsigned char _unmodeled_1de[0x0a];   /* +0x1de..0x1e7 */
    /*
     * Two (callback, argument) pairs stored verbatim by main/tu112:
     * SsdSetSampleDmaCallback (main:0x002410a0) `sw $4,488($2)` /
     * `sw $5,492($2)` and SsdSetSampleKeyoffCallback (main:0x002410b8)
     * `sw $4,496($2)` / `sw $5,500($2)`, $2 = &RssdWork. No recovered
     * function reads them back, so their call signature is not evidenced
     * and they stay untyped pointers named after the store site.
     */
    void *sample_dma_callback;            /* +0x1e8: SsdSetSampleDmaCallback arg0 */
    void *sample_dma_callback_arg;        /* +0x1ec: SsdSetSampleDmaCallback arg1 */
    void *sample_keyoff_callback;         /* +0x1f0: SsdSetSampleKeyoffCallback arg0 */
    void *sample_keyoff_callback_arg;     /* +0x1f4: SsdSetSampleKeyoffCallback arg1 */
    /*
     * Stored verbatim by SsdSetStreamEndCallback (main:0x002403e0)
     * `sw $5,508($2)` / `sw $4,504($2)`, $2 = &RssdWork. No recovered
     * function reads them back, so their call signature is not evidenced
     * and they stay untyped pointers named after the store site.
     */
    void *stream_end_callback;            /* +0x1f8: SsdSetStreamEndCallback arg0 */
    void *stream_end_callback_arg;        /* +0x1fc: SsdSetStreamEndCallback arg1 */
} RssdWorkFlags;

/* RssdWorkFlags.flags bit 5: set/cleared around an RssdCallFunc call to report the RPC's outcome. */
#define RSSD_FLAG_SUCCESS 0x20


extern RssdWorkFlags RssdWork;

/*
 * One argument word of an RSSD request. Most commands pass plain integers;
 * SsdTransferSampling/SsdTransferSamplingNext (main/tu112) and the sequence
 * data wrappers of main/tu113 pass a buffer address in the same slot, so the
 * word is a union of the two views (both are 32-bit on the EE).
 */
typedef union RssdRequestWord {
    int value;
    void *pointer;
    struct {
        unsigned short sample_rate;
        short request_channel_count;
    } rate_channel;
} RssdRequestWord;

/*
 * The 32-byte RSSD RPC request record.
 *
 * RssdCallFunc (main:0x0023fff0, still INCLUDE_ASM) copies all 32 bytes of a
 * non-null request into the SIF RPC buffer RssdWork.response_source with
 * four unaligned ldl/ldr -> sdl/sdr pairs (0x00240050..0x0024008c), then
 * writes two fields of that copy from its own arguments: the +0x00 halfword
 * (`sh $21,0($17)`, command) and the +0x0c word (`sw $16,12($17)`, size).
 * No caller writes header[0..3] (+0x00..+0x0f), but every wrapper reserves
 * the whole record on its stack (all of them have a 0x30-byte frame, however
 * many argument words the command uses).
 * arg[0..3] (+0x10..+0x1f) are the command's own argument words.
 */
typedef struct RssdRequest {
    int header[4];
    RssdRequestWord arg[4];
} RssdRequest;

/*
 * Sends one RSSD command over SIF RPC (end function RssdSifRpcCallback):
 * `request` may be null, otherwise it is copied as above, and `size` bytes
 * of `data` are copied after the record (SsdCopyMemory into buffer +0x20).
 * Returns -1 when the rounded payload exceeds the buffer, otherwise the
 * sceSifCallRpc result.
 */

/*
 * SIF RPC end callback for the RSSD work area: clears flag bit 3, copies the
 * 32-byte receive record into RssdWork, reports a status (-1 on a nonzero
 * result, else flag bit 5) through the registered one-shot completion
 * callback, then signals the waiting thread's semaphore.
 *
 * It runs from the SIF RPC interrupt context, so it ends with the
 * ee-interrupt-handler-return primitive (docs/ps2-capabilities.md): `sync.l`
 * orders every earlier load/store, including the iSignalSema effects, before
 * `ei` re-enables interrupts on the way out.
 */



/*
 * libkernel syscall stubs (main:0x00208fe0 / main:0x00209028).
 */

extern int DIntr(void);

extern int EIntr(void);

/*
 * iSsdNewMemoryPtr is this TU's own low-address block allocator (its body is
 * still scaffold below, main:0x00242150). `tag` is stored verbatim at +0xc of
 * the returned block's header; nothing recovered in this TU reads it back.
 */

extern void *iSsdNewMemoryPtr(int size, int tag);

/*
 * iSsdNewMemoryPtr2 is this TU's own high-address block allocator (its body
 * is still scaffold below, main:0x00242278); same header convention as
 * iSsdNewMemoryPtr.
 */

extern void *iSsdNewMemoryPtr2(int size, int tag);

/*
 * iSsdDisposeMemoryPtr is this TU's own block-release routine (its body is
 * still scaffold below, main:0x002423b0): `ptr` minus the fixed 0x40-byte
 * header size is the block header, returned to the free list.
 */

extern void iSsdDisposeMemoryPtr(void *ptr);

/*
 * Returns the payload size recorded in a sound-memory allocation header:
 * `ptr` minus one SsdMemoryBlock header is the header itself (same
 * convention as SsdDisposeMemoryPtr above), and `end` is the absolute
 * address just past the payload.
 */

/*
 * libkernel syscall stubs (main:0x00208fe0 / main:0x00209028).
 */

void SsdClearMemory(void *buffer, int size);

INCLUDE_ASM("asm/main/nonmatchings/ssd_4", SsdInitMemoryManager);

void *SsdNewMemoryPtr(int size, int tag)
{
    void *ptr;

    DIntr();
    ptr = iSsdNewMemoryPtr(size, tag);
    EIntr();
    return ptr;
}

void *iSsdNewMemoryPtr(int size, int tag)
{
    SsdMemoryBlock *previous;
    SsdMemoryBlock *next;
    SsdMemoryBlock *block;
    void *payload;
    int needed;
    int start;
    int gap;

    needed = ((size + 63) & ~0x3f) + 64;
    previous = RssdWork.first_block;
    do {
        next = previous->next;
        if (next == 0) {
            break;
        }
        start = previous->end;
        gap = (int)next - start;
        if (gap >= needed) {
            break;
        }
        previous = next;
    } while (1);
    if (next == 0) {
        start = previous->end;
        gap = RssdWork.memory_end - start;
        if (gap < needed) {
            return 0;
        }
    }

    block = (SsdMemoryBlock *)((start + 63) & ~0x3f);
    payload = block + 1;
    block->tag = tag;
    block->state = 2;
    block->end = (int)payload + size;
    block->next = 0;
    block->flags = 0;
    SsdClearMemory(payload, size);
    block->next = previous->next;
    previous->next = block;
    return payload;
}

void *SsdNewMemoryPtr2(int size, int tag)
{
    void *ptr;

    DIntr();
    ptr = iSsdNewMemoryPtr2(size, tag);
    EIntr();
    return ptr;
}

void *iSsdNewMemoryPtr2(int size, int tag)
{
    SsdMemoryBlock *current;
    SsdMemoryBlock *next;
    SsdMemoryBlock *bestPrevious;
    SsdMemoryBlock *block;
    void *payload;
    int bestEnd;
    int needed;
    int start;
    int gap;
    int tailEnd;

    bestEnd = 0;
    bestPrevious = 0;
    needed = ((size + 63) & ~0x3f) + 64;
    current = RssdWork.first_block;
    do {
        next = current->next;
        if (next == 0) {
            tailEnd = current->end;
            gap = RssdWork.memory_end - tailEnd;
            if (gap >= needed) {
                bestPrevious = current;
                bestEnd = RssdWork.memory_end;
            }
            break;
        }
        start = current->end;
        gap = (int)next - start;
        if (gap >= needed) {
            bestPrevious = current;
            bestEnd = (int)next;
        }
        current = next;
    } while (1);

    if (bestPrevious == 0) {
        return 0;
    }
    bestEnd -= needed;
    block = (SsdMemoryBlock *)((bestEnd + 63) & ~0x3f);
    payload = block + 1;
    block->state = 18;
    block->tag = tag;
    block->end = (int)payload + size;
    block->next = 0;
    block->flags = 0;
    SsdClearMemory(payload, size);
    block->next = bestPrevious->next;
    bestPrevious->next = block;
    return payload;
}

void SsdDisposeMemoryPtr(void *ptr)
{
    DIntr();
    iSsdDisposeMemoryPtr(ptr);
    EIntr();
}

void iSsdDisposeMemoryPtr(void *ptr)
{
    SsdMemoryBlock *block;
    SsdMemoryBlock *previous;
    SsdMemoryBlock *next;

    block = (SsdMemoryBlock *)ptr - 1;
    previous = RssdWork.first_block;
    next = previous->next;
    while (next != block) {
        if (next == 0) {
            return;
        }
        previous = next;
        next = previous->next;
    }
    previous->next = block->next;
}

int SsdGetBlockMemorySize(void *ptr)
{
    SsdMemoryBlock *block;

    block = (SsdMemoryBlock *)ptr - 1;
    return block->end - (int)block;
}

int SsdGetMemoryFreeSize(void)
{
    SsdMemoryBlock *block;
    SsdMemoryBlock *nextBlock;
    SsdMemoryBlock *following;
    int largest;
    int gap;

    DIntr();
    largest = 0;
    block = RssdWork.first_block;
    nextBlock = block->next;
    if (nextBlock != 0) {
        do {
            gap = ((int)nextBlock - block->end) & ~0x3f;
            block = nextBlock;
            following = block->next;
            if (largest < gap) {
                largest = gap;
            }
            nextBlock = following;
        } while (following != 0);
    }
    gap = (RssdWork.memory_end - block->end) & ~0x3f;
    if (largest < gap) {
        largest = gap;
    }
    EIntr();
    return largest;
}

int SsdGetMemoryBlocks(void)
{
    SsdMemoryBlock *block;
    SsdMemoryBlock *nextBlock;
    int count;

    DIntr();
    count = 0;
    block = ((SsdMemoryBlock *)RssdWork.first_block)->next;
    if (block != 0) {
        do {
            nextBlock = block->next;
            count++;
            block = nextBlock;
        } while (nextBlock != 0);
    }
    EIntr();
    return count;
}

void SsdCopyMemory(void *destination, void *source, int byte_count)
{
    int *destination_words = destination;
    const unsigned int *source_words = source;
    unsigned char *destination_bytes;
    const unsigned char *source_bytes;
    long copy_count;
    long long source_word0;
    long long source_word1;
    long long source_word2;
    long long source_word3;

    copy_count = byte_count >> 4;
    while (copy_count != 0) {
        source_word0 = source_words[0];
        source_word1 = source_words[1];
        source_word2 = source_words[2];
        source_word3 = source_words[3];
        source_words += 4;
        destination_words[0] = source_word0;
        destination_words[1] = source_word1;
        destination_words[2] = source_word2;
        destination_words[3] = source_word3;
        destination_words += 4;
        copy_count--;
    }

    copy_count = (byte_count >> 2) & 3;
    while (copy_count != 0) {
        *destination_words++ = (int)*source_words++;
        copy_count--;
    }

    copy_count = byte_count & 3;
    if (copy_count != 0) {
        source_bytes = (const unsigned char *)source_words;
        destination_bytes = (unsigned char *)destination_words;
        do {
            *destination_bytes++ = *source_bytes++;
            copy_count--;
        } while (copy_count != 0);
    }
}

void SsdClearMemory(void *buffer, int size)
{
    unsigned long long *destination64 = buffer;
    unsigned int *destination32;
    unsigned char *destination_bytes;
    long block_count;
    long byte_count;

    block_count = size >> 5;
    if (block_count != 0) {
        do {
            destination64[3] = 0;
            destination64[2] = 0;
            destination64[1] = 0;
            destination64[0] = 0;
            destination64 += 4;
            block_count--;
        } while (block_count != 0);
    }

    destination32 = (unsigned int *)destination64;
    block_count = (size >> 4) & 1;
    if (block_count != 0) {
        do {
            destination32[3] = 0;
            destination32[2] = 0;
            destination32[1] = 0;
            destination32[0] = 0;
            destination32 += 4;
            block_count--;
        } while (block_count != 0);
    }

    destination_bytes = (unsigned char *)destination32;
    byte_count = size & 0xf;
    while (byte_count != 0) {
        *destination_bytes++ = 0;
        byte_count--;
    }
}
