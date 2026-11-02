#ifndef INCLUDE_MAIN_SSD_INIT_H
#define INCLUDE_MAIN_SSD_INIT_H

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
    int data_size; /* +0xc */
    unsigned char unmodeled_010[0x20 - 0x10];          /* +0x0c..0x1f: 32-bit words, not read here */
} RssdRpcResponse;

/*
 * RssdWork is the RSSD RPC / background-wave work area, main:0x004aa080,
 * ELF symbol size 0x200 bytes. Only the evidenced members are modeled here;
 * bytes that no recovered function accesses stay explicit unmodeled spans.
 */
typedef struct RssdWorkFlags {
    int flags;                            /* +0x000: bit 3 cleared on RPC completion; bit 5 is the success status */
    int rpc_buffer_size;                  /* +0x004: RPC buffer capacity */
    int request_parameter;                /* +0x008: copied by RssdBusy from its request */
    int init_parameter;                   /* +0x00c: SsdInit argument */
    unsigned short sample_rate;           /* +0x010: samples per second used by SsdGetTimeCode */
    short request_channel_count;          /* +0x012: copied by RssdBusy from its request */
    int request_size;                     /* +0x014: copied by RssdBusy from its request */
    unsigned char _unmodeled_018[0x04];   /* +0x018..0x01b */
    int server_call_count;                /* +0x01c: server request count */
    /*
     * Stored verbatim by SsdSetServerCallback (main:0x002403b0)
     * `sw $5,36($2)` / `sw $4,32($2)`, $2 = &RssdWork. RssdSifRpcServer
     * (main:0x0023fc90) calls the callback (`jalr $2`, +0x20) for every
     * command it does not handle itself, with the command in $a0, the
     * request record in $a1 and the stored argument (+0x24) in $a2.
     */
    void (*server_callback)(unsigned int command, void *request,
                            void *arg);   /* +0x020: SsdSetServerCallback arg0 */
    void *server_callback_arg;            /* +0x024: SsdSetServerCallback arg1 */
    void (*complete_callback)(int status, RssdRpcResponse *response,
                              void *arg); /* +0x028: one-shot, cleared after use */
    void *callback_arg;                   /* +0x02c: third complete_callback argument */
    void *client_buffer_base;             /* +0x030: original client storage */
    RssdRpcResponse *response_source;     /* +0x034: SIF RPC receive buffer */
    unsigned char _unmodeled_038[0x48];   /* +0x038..0x07f */
    RssdRpcResponse response;             /* +0x080..0x09f: copy of *response_source */
    void *server_buffer_base;             /* +0x0a0: original server storage */
    void *server_buffer;                  /* +0x0a4: aligned server buffer */
    unsigned char unmodeled_0a8[0x100 - 0xa8];
    RssdRpcResponse server_packet; /* +0x100 */
    unsigned char rpc_client[0x24]; /* +0x120 */
    void *rpc_client_serve; /* +0x144 */   /* +0x0a8..0x147 */
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
    int memory_size;                      /* +0x1c8: aligned arena size */
    int spu_bytes_remaining;              /* +0x1cc: drained by RssdBackNextWaveThread */
    /*
     * Running destination pointer for streamed sample data. RssdSpuRead
     * (main:0x0023feb8) copies each request's payload here with
     * SsdCopyMemory and advances it by the copied byte count; SsdSpuDirectRead
     * (main:0x00240690, still asm) sets it from its own destination argument.
     */
    unsigned char *spu_write_ptr;         /* +0x1d0: streamed sample write position */
    int wave_id;                          /* +0x1d4: active wave id */
    unsigned char _unmodeled_1d8[4];      /* +0x1d8..0x1db */
    unsigned short wave_chunk_index;      /* +0x1dc: streamed chunk index */
    unsigned char _unmodeled_1de[0x0a];   /* +0x1de..0x1e7 */
    /*
     * Two (callback, argument) pairs stored verbatim by main/tu112:
     * SsdSetSampleDmaCallback (main:0x002410a0) `sw $4,488($2)` /
     * `sw $5,492($2)` and SsdSetSampleKeyoffCallback (main:0x002410b8)
     * `sw $4,496($2)` / `sw $5,500($2)`, $2 = &RssdWork. RssdSifRpcServer
     * (main:0x0023fc90) calls the DMA callback (`lw $4,492($6)`) with the
     * stored argument for command 218 and the key-off callback with
     * `$a0 = 0` and the stored argument in $a1 for command 219.
     */
    void (*sample_dma_callback)(void *arg);          /* +0x1e8: SsdSetSampleDmaCallback arg0 */
    void *sample_dma_callback_arg;        /* +0x1ec: SsdSetSampleDmaCallback arg1 */
    void (*sample_keyoff_callback)(int channel, void *arg);       /* +0x1f0: SsdSetSampleKeyoffCallback arg0 */
    void *sample_keyoff_callback_arg;     /* +0x1f4: SsdSetSampleKeyoffCallback arg1 */
    /*
     * Stored verbatim by SsdSetStreamEndCallback (main:0x002403e0)
     * `sw $5,508($2)` / `sw $4,504($2)`, $2 = &RssdWork. RssdSifRpcServer
     * (main:0x0023fc90) calls the callback for command 10 once per set bit
     * 0..3 of the request word at +0x10, with the bit index in $a0 and the
     * stored argument in $a1.
     */
    void (*stream_end_callback)(int channel, void *arg);          /* +0x1f8: SsdSetStreamEndCallback arg0 */
    void *stream_end_callback_arg;        /* +0x1fc: SsdSetStreamEndCallback arg1 */
} RssdWorkFlags;

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
 * Sends one RSSD command over SIF RPC; see main:0x0023fff0 (still
 * INCLUDE_ASM, main/tu110). `request` may be null, otherwise its 32 bytes
 * are copied into the SIF RPC buffer, and `size` bytes of `data` are copied
 * after it.
 */
int RssdCallFunc(int command, RssdRequest *request, void *data, int size);

/* Functions and data used by other translation units. */
int SsdGetResultValue(int *value);

#endif /* INCLUDE_MAIN_SSD_INIT_H */
