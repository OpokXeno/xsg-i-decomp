/*
 * TU-local declarations of main/tu110 (src/main/ssd_init.c).
 */

#ifndef SRC_MAIN_SSD_INIT_H
#define SRC_MAIN_SSD_INIT_H

/*
 * The 32-byte SIF RPC receive record copied into RssdWork.response.
 *
 * Evidence from RssdSifRpcCallback (main:0x00240188):
 * - +0x02 is a signed halfword (lh v0,130(a2), a2 = &RssdWork, so
 *   response+2): nonzero reports the -1 (error) status.
 * - The record is copied as a whole with unaligned doubleword pairs
 *   (ldl/ldr -> sdl/sdr, four of them): its type is not 8-byte aligned.
 * - After the copy the compiler reloads RssdWork.flags (lw v0,0(a2) before
 *   the sra/andi), i.e. the copied record may alias an `int` object under
 *   GCC's type-based aliasing. The words after the two halfwords are
 *   therefore `int`-typed.
 * - The sequence command wrappers in main/tu113 return the word at +0x08.
 *   Its protocol meaning is not yet known, so the neutral name is `value`.
 * - SsdGetResultValue (main:0x002402a0) reads +0x00 as a signed halfword
 *   (lh v0,128(a1)) and branches on it being negative before reporting the
 *   "Rssd get result error !" message, i.e. it is a per-command error code
 *   distinct from the RPC-level `result` at +0x02.
 */
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
 * (see result.json header_divergence). Bytes this allocation does not access
 * stay explicit unmodeled spans.
 */
typedef struct RssdWorkFlags {
    int flags;                            /* +0x000: bit 3 cleared on RPC completion; bit 5 is the success status */
    unsigned char _unmodeled_004[0x0c];   /* +0x004..0x00f */
    unsigned short sample_rate;           /* +0x010: samples per second used by SsdGetTimeCode */
    unsigned char _unmodeled_012[0x0e];   /* +0x012..0x01f */
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
    unsigned char _unmodeled_1b8[0x18];   /* +0x1b8..0x1cf */
    /*
     * Running destination pointer for streamed sample data. RssdSpuRead
     * (main:0x0023feb8) copies each request's payload here with
     * SsdCopyMemory and advances it by the copied byte count; SsdSpuDirectRead
     * (main:0x00240690, still asm) sets it from its own destination argument.
     */
    unsigned char *spu_write_ptr;         /* +0x1d0: streamed sample write position */
    unsigned char _unmodeled_1d4[0x14];   /* +0x1d4..0x1e7 */
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

void RssdBackgroundNextWave(const int *request);

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
int RssdCallFunc(int command, RssdRequest *request, void *data, int size);

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
void RssdSifRpcCallback(void);

#endif /* SRC_MAIN_SSD_INIT_H */
