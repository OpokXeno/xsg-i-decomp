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
 *   therefore `int`-typed; their meaning is not evidenced by this function,
 *   so they stay an explicit unmodeled word span rather than named fields.
 */
typedef struct RssdRpcResponse {
    unsigned short _unmodeled_00; /* +0x00 */
    short result;                 /* +0x02: nonzero -> error status (-1) */
    int _unmodeled_04[7];         /* +0x04..0x1f: 32-bit words, not read here */
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
    unsigned char _unmodeled_004[0x24];   /* +0x004..0x027 */
    void (*complete_callback)(int status, RssdRpcResponse *response,
                              void *arg); /* +0x028: one-shot, cleared after use */
    void *callback_arg;                   /* +0x02c: third complete_callback argument */
    unsigned char _unmodeled_030[0x04];   /* +0x030..0x033 */
    RssdRpcResponse *response_source;     /* +0x034: SIF RPC receive buffer */
    unsigned char _unmodeled_038[0x48];   /* +0x038..0x07f */
    RssdRpcResponse response;             /* +0x080..0x09f: copy of *response_source */
    unsigned char _unmodeled_0a0[0x104];  /* +0x0a0..0x1a3 */
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
    unsigned char _unmodeled_1b8[0x48];   /* +0x1b8..0x1ff */
} RssdWorkFlags;

void RssdBackgroundNextWave(const int *request);

extern RssdWorkFlags RssdWork;

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
