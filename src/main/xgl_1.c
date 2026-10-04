#include "common.h"
#include "shared.h"

extern const char D_004DC328[];

/*
 * xglMpeg2InfoInit2's own body is still INCLUDE_ASM below, but its tail at
 * 0x00222818..0x00222828 computes align64(arena + allocated_size +
 * audio_buffer_size) into v0 immediately before return: that value is the
 * function's result, not a void return. xglMpeg2InfoInit calls it and
 * discards the result, but the forward declaration used here must still
 * spell the pointer-return signature the body actually implements.
 */
extern void *xglMpeg2InfoInit2(void *info, void *arena, int audio_buffer_size);

struct CdStreamParam;

typedef struct XglMovieInfo {
    unsigned char unmodeled_00[8];
    unsigned char streamState;          /* +0x08 */
    unsigned char unmodeled_09[0x17];
    unsigned char *ringBuffer;          /* +0x20 */
    int ringCapacity;                   /* +0x24 */
    int ringWrite;                      /* +0x28 */
    int ringRead;                       /* +0x2c */
    unsigned char mpeg[0x10];           /* +0x30 */
    short width;                        /* +0x40 */
    short height;                       /* +0x42 */
    unsigned char unmodeled_44[2];
    short currentFrame;                 /* +0x46 */
    unsigned char unmodeled_48[0x30];
    unsigned char *arena;               /* +0x78 */
    int reservedSize;                   /* +0x7c */
    unsigned char *videoBuffer;         /* +0x80 */
    unsigned char *mpegBuffer;          /* +0x84 */
    unsigned char *readBuffer;          /* +0x88 */
    int pendingBytes;                   /* +0x8c */
    int audioSampleRate;                /* +0x90 */
    unsigned char audioHeaderRead;      /* +0x94 */
    unsigned char unmodeled_95[3];
    unsigned char decodeState;          /* +0x98 */
    unsigned char streamOpen;           /* +0x99 */
    unsigned char missCount;            /* +0x9a */
    unsigned char videoCount;           /* +0x9b */
    unsigned char *dmaBuffer;           /* +0x9c */
    int dmaSize;                        /* +0xa0 */
    int dmaWrite;                       /* +0xa4 */
    int dmaRead;                        /* +0xa8 */
    int dmaNextRead;                    /* +0xac */
    unsigned char *soundBuffer;         /* +0xb0 */
    int soundBufferSize;                /* +0xb4 */
    int soundWrite;                     /* +0xb8 */
    int soundRead;                      /* +0xbc */
} XglMovieInfo;

typedef struct XglMovieInitInfo {
    unsigned char unmodeled_00[8];
    unsigned char streamState;          /* +0x08 */
    unsigned char unmodeled_09[0x17];
    unsigned char *ringBuffer;          /* +0x20 */
    int ringCapacity;                   /* +0x24 */
    int ringWrite;                      /* +0x28 */
    int ringRead;                       /* +0x2c */
    unsigned char unmodeled_30[0x16];
    short currentFrame;                 /* +0x46 */
    unsigned char unmodeled_48[0x40];
    unsigned char *readBuffer;          /* +0x88 */
    unsigned char unmodeled_8c[0x0e];
    unsigned char missCount;            /* +0x9a */
    unsigned char unmodeled_9b[0x1d];
    unsigned int packetAddress[2];      /* +0xb8 */
    unsigned char unmodeled_c0[9];
    unsigned char transMode;            /* +0xc9 */
    unsigned char threshold1;           /* +0xca */
    unsigned char threshold0;           /* +0xcb */
} XglMovieInitInfo;

typedef struct MpegStreamData {
    unsigned char unmodeled_00[8];
    unsigned char *buffer;              /* +0x08 */
    int length;                         /* +0x0c */
} MpegStreamData;

#define D_ENABLEW ((volatile u32 *) 0x1000f590)
#define D3_CHCR   ((volatile u32 *) 0x1000b000)
#define D3_MADR   ((volatile u32 *) 0x1000b010)
#define D3_QWC    ((volatile u32 *) 0x1000b020)
#define D4_MADR   ((volatile u32 *) 0x1000b410)
#define D4_QWC    ((volatile u32 *) 0x1000b420)
#define D4_CHCR   ((volatile u32 *) 0x1000b400)
#define IPU_CMD   ((volatile u32 *) 0x10002000)
#define IPU_CTRL  ((volatile u32 *) 0x10002010)
#define UNCACHED(address) \
    ((void *) (((unsigned int) (address) & 0x0FFFFFFF) | 0x20000000))

extern int DIntr(void);
extern int EIntr(void);
extern void *memcpy(void *destination, const void *source, unsigned int count);
extern void FlushCache(int mode);
extern int sceIpuSync(int mode, int timeout);
extern int xglCdStreamClose(struct CdStreamParam *stream);
extern void xglCdStreamParamInit(struct CdStreamParam *param);
extern void xglCdStreamReadRingCore(struct CdStreamParam *stream);
extern void xglCdStreamReadRing(struct CdStreamParam *stream, int byte_count);
extern int xglCdStreamOpen(struct CdStreamParam *stream);
extern void sceMpegInit(void);
extern void sceMpegCreate(void *mpeg, void *work, int work_size);
extern void sceMpegDelete(void *mpeg);
extern void sceMpegAddCallback(void *mpeg, int type, int (*callback)(), void *argument);
extern void sceMpegAddStrCallback(void *mpeg, int stream, int channel,
                                  int (*callback)(), void *argument);
extern int sceMpegDemuxPss(void *mpeg, unsigned char *source, int size);
extern int SsdGetResultValue(int *value);
extern void SsdInitVagStreamStereo(int size, int channels);
static int fillBuff(XglMovieInfo *info, int wait);
static int nodataCallback(int event, void *data, XglMovieInfo *info);

/*
 * Maps a scratchpad address (0x7000xxxx) into the DMA scratchpad alias
 * before programming D3_MADR/D3_QWC, then starts DMA channel 3 (fromIPU) in
 * direct mode (D3_CHCR = 0x100) with the D_ENABLEW bit 0x10000 held around
 * the CHCR write.
 */
static int myDmaDirectFromIPU(u32 address, u32 count)
{
    if ((address >> 16) != 0x7000)
        *D3_MADR = address;
    else
        *D3_MADR = (address & 0x3fff) + 0x80000000;
    *D3_QWC = count;

    DIntr();
    *D_ENABLEW = *D_ENABLEW | 0x10000;
    *D3_CHCR = 0x100;
    *D_ENABLEW = *D_ENABLEW & 0xfffeffff;
    return EIntr();
}

/*
 * Writes value to D4_CHCR (DMA channel 4, toIPU) with the D_ENABLEW bit
 * 0x10000 held around the write, like myDmaDirectFromIPU above.
 */
static void setD4_CHCR(u32 value)
{
    DIntr();
    *D_ENABLEW = *D_ENABLEW | 0x10000;
    *D4_CHCR = value;
    *D_ENABLEW = *D_ENABLEW & 0xfffeffff;
    EIntr();
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_1", make_gstrans);

INCLUDE_ASM("asm/main/nonmatchings/xgl_1", checkdma);

INCLUDE_ASM("asm/main/nonmatchings/xgl_1", xglMovieOpen);

INCLUDE_ASM("asm/main/nonmatchings/xgl_1", xglMoviePlay);

int xglMovieClose(XglMovieInfo *info)
{
    if (info->streamOpen) {
        xglCdStreamClose((struct CdStreamParam *) info);
    }
    setD4_CHCR(0);
    *IPU_CTRL = 0x40000000;
    if (sceIpuSync(0, 0x40) < 0) {
        return -1;
    }
    *IPU_CMD = 0;
    return sceIpuSync(0, 0x40) < 0 ? -1 : 0;
}

void xglMovieInfoInit(XglMovieInitInfo *info)
{
    xglCdStreamParamInit((struct CdStreamParam *) info);
    info->packetAddress[0] = 0x70000040;
    info->packetAddress[1] = 0x70001000;
    info->transMode = info->currentFrame = -1;
    info->missCount = 0;
    info->threshold1 = 0;
    info->threshold0 = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_1", xglMovieMakeXtxHeader);

static int fileRead(XglMovieInfo *info)
{
    unsigned char *destination;
    int retry;
    int length;
    int available;
    int tail;

    available = 0;
    length = 0x4000;
    destination = info->readBuffer;
    for (retry = 0; retry <= 0xFFFFFF; retry++) {
        xglCdStreamReadRingCore((struct CdStreamParam *) info);
        available = (info->ringWrite - info->ringRead + info->ringCapacity) %
                    info->ringCapacity;
        if (available == 0) {
            available = info->ringCapacity;
        }
        if (info->streamState != 0) {
            break;
        }
        if (available > length) {
            break;
        }
    }
    if (available < length) {
        length = available;
    }
    tail = info->ringCapacity - info->ringRead;
    if (length <= tail) {
        memcpy(destination, info->ringBuffer + info->ringRead, length);
    } else {
        memcpy(destination, info->ringBuffer + info->ringRead, tail);
        memcpy(destination + tail, info->ringBuffer, length - tail);
    }
    info->ringRead = (info->ringRead + length) % info->ringCapacity;
    return length;
}

static int fillBuff(XglMovieInfo *info, int wait)
{
    unsigned char *position;
    int remaining;
    int count;
    int retry;

    remaining = info->pendingBytes;
    if (remaining != 0) {
        position = info->readBuffer + 0x4000 - remaining;
    } else {
        for (retry = 0; retry <= 0xFFFFFF; retry++) {
            count = fileRead(info);
            info->pendingBytes = count;
            if (count == 0) {
                if (wait == 0) {
                    return count;
                }
            } else {
                break;
            }
        }
        position = info->readBuffer;
        remaining = info->pendingBytes;
    }
    count = sceMpegDemuxPss(info->mpeg, position, remaining);
    info->pendingBytes -= count;
    return count;
}

static int videoCallback(int event, MpegStreamData *data, XglMovieInfo *info)
{
    int free;
    int overflow;
    int write;

    info->videoCount++;
    write = info->dmaWrite;
    free = info->dmaRead - write;
    if (free < 0) {
        free += info->dmaSize;
    }
    if ((unsigned int) free < (unsigned int) data->length) {
        return 0;
    }
    overflow = write + data->length - info->dmaSize;
    if (overflow > 0) {
        memcpy(UNCACHED(info->dmaBuffer + write), data->buffer,
               data->length - overflow);
        memcpy(UNCACHED(info->dmaBuffer), data->buffer + (data->length - overflow),
               overflow);
        info->dmaWrite = overflow;
    } else {
        memcpy(UNCACHED(info->dmaBuffer + write), data->buffer, data->length);
        info->dmaWrite += data->length;
    }
    return 1;
}

static int audioCallback(int event, MpegStreamData *data, XglMovieInfo *info)
{
    unsigned char *source;
    int length;
    int free;
    int tail;

    source = data->buffer;
    length = data->length;
    if (info->audioHeaderRead == 0) {
        info->audioSampleRate = (source[0x13] << 24) + (source[0x12] << 16) + (source[0x11] << 8) + source[0x10];
        info->audioHeaderRead = 1;
        length -= 0x2C;
        source += 0x2C;
    } else {
        source += 4;
        length -= 4;
    }
    free = info->soundRead - info->soundWrite;
    if (free <= 0) {
        free += info->soundBufferSize;
    }
    if (free <= length) {
        return 0;
    }
    tail = info->soundBufferSize - info->soundWrite;
    if (length <= tail) {
        memcpy(info->soundBuffer + info->soundWrite, source, length);
    } else {
        memcpy(info->soundBuffer + info->soundWrite, source, tail);
        memcpy(info->soundBuffer, source + tail, length - tail);
    }
    info->soundWrite = (info->soundWrite + length) % info->soundBufferSize;
    return 1;
}

static int nodataCallback(int event, void *data, XglMovieInfo *info)
{
    unsigned int *pattern;
    int i;
    int retry;
    int available;
    int length;
    unsigned int dma_address;
    int qwc;
    /* DMA channel 4 is programmed in QWC, MADR, then CHCR order. */
    volatile u32 *qwc_register;

    if (++info->missCount >= 0x41) {
        info->decodeState = 0x10;
        pattern = (unsigned int *) info->dmaBuffer;
        for (i = 0x1000 - 4; i >= 0; i -= 4) {
            *pattern = 0x100;
            pattern++;
        }
        FlushCache(0);
        qwc_register = D4_QWC;
        dma_address = (unsigned int) info->dmaBuffer;
        qwc = 0x1000;
        *qwc_register = qwc;
        *D4_MADR = dma_address;
        *D4_CHCR = 0x101;
        return 1;
    }

    info->dmaRead = info->dmaNextRead;
    available = info->dmaWrite - info->dmaRead;
    if (available < 0) {
        available += info->dmaSize;
    }
    for (retry = 0; retry <= 0xFFFFFF; retry++) {
        if (available >= 0x1000) {
            break;
        }
        if (fillBuff(info, 1) == 0) {
            available += 0xF;
            break;
        }
        available = info->dmaWrite - info->dmaRead;
        if (available < 0) {
            available += info->dmaSize;
        }
    }
    length = available & ~0xF;
    if (length > 0x1000) {
        length = 0x1000;
    }
    if (info->dmaSize < info->dmaRead + length) {
        length = info->dmaSize - info->dmaRead;
    }
    *D4_QWC = length / 16;
    *D4_MADR = (unsigned int) info->dmaBuffer + info->dmaRead;
    *D4_CHCR = 0x101;
    info->dmaNextRead = (info->dmaRead + length) % info->dmaSize;
    return 1;
}

/*
 * errorCallback (main:0x00222158) is registered as the MPEG movie decoder's
 * error handler. The original bytes read a pointer at arg1+0x04 and pass it
 * as the "%s\n" argument of D_004DC328's printf, then read, increment and
 * store back an unsigned byte at arg2+0x98 and clear a byte at arg2+0x99
 * right after it (lbu/sb at those fixed offsets).
 */
typedef struct {
    unsigned char unmodeled_00[4];
    const char *message;
} MpegErrorInfo;

typedef struct {
    unsigned char unmodeled_00[0x98];
    unsigned char errorCount;
    unsigned char errorFlag;
} MpegCallbackState;

extern int printf(const char *format, ...);

static int errorCallback(int event, MpegErrorInfo *error, MpegCallbackState *state)
{
    printf(D_004DC328, error->message);
    state->errorCount++;
    state->errorFlag = 0;
    return 1;
}

int xglMpeg2Open(XglMovieInfo *info)
{
    int result;
    int retry;

    if (xglCdStreamOpen((struct CdStreamParam *) info) < 0) {
        return -1;
    }
    xglCdStreamReadRing((struct CdStreamParam *) info, info->ringCapacity / 2);
    info->ringRead = 0;
    sceMpegInit();
    sceMpegCreate(info->mpeg, info->arena, info->reservedSize);
    sceMpegAddStrCallback(info->mpeg, 0, 0, videoCallback, info);
    sceMpegAddStrCallback(info->mpeg, 3, 0, audioCallback, info);
    sceMpegAddCallback(info->mpeg, 1, nodataCallback, info);
    sceMpegAddCallback(info->mpeg, 0, errorCallback, info);
    info->pendingBytes = 0;
    info->audioHeaderRead = 0;
    info->audioSampleRate = 0;
    info->decodeState = 0;
    info->streamOpen = 0;
    info->missCount = 0;
    info->videoCount = 0;
    for (retry = 0; retry <= 0xFFFFFF; retry++) {
        if (fillBuff(info, 1) == 0) {
            break;
        }
    }
    SsdInitVagStreamStereo(0x1000, 4);
    do {
    } while (SsdGetResultValue(&result) < 0);
    return 0;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_1", setLoadImageTags);

INCLUDE_ASM("asm/main/nonmatchings/xgl_1", xglMpeg2Play);

/*
 * xglCdStreamClose's own parameter type (CdStreamParam, main/tu092
 * src/main/xgl_cd.c) is TU-local to that file; this TU only forwards a
 * pointer to it and does not access its members, so the tag stays opaque.
 */
typedef struct CdStreamParam CdStreamParam;

extern void SsdDisposeVagStream(void);
extern int SsdGetResultValue(int *value);
extern void SsdStopVagStream(int channel);
extern void sceMpegDelete(void *mpegHandle);
extern void sceMpegReset(void *mpegHandle);
extern int xglCdStreamClose(CdStreamParam *stream);

/*
 * arg0+0x30 (temp_16 = s1+0x30 in the original) is passed to sceMpegReset
 * and sceMpegDelete as a raw address; sceMpegReset/sceMpegDelete have no
 * public prototype (docs/naming.md), so the embedded MPEG handle stays an
 * untyped pointer computed by byte offset from the stream object.
 */
int xglMpeg2Close(unsigned char *stream)
{
    void *mpegHandle;
    int value;

    mpegHandle = stream + 0x30;
    sceMpegReset(mpegHandle);
    sceMpegDelete(mpegHandle);
    SsdStopVagStream(0);
    do {
    } while (SsdGetResultValue(&value) < 0);
    SsdDisposeVagStream();
    do {
    } while (SsdGetResultValue(&value) < 0);
    xglCdStreamClose((CdStreamParam *) stream);
    return 0;
}

void *xglMpeg2InfoInit2(void *info, void *arena, int audio_buffer_size)
{
    XglMovieInfo *movie;
    unsigned char *next;

    movie = info;
    movie->arena = arena;
    next = (unsigned char *) (((unsigned int) arena + 0xFD768 + 0x3F) & ~0x3F);
    movie->videoBuffer = next;
    next = (unsigned char *) (((unsigned int) next + 0xE0000 + 0x3F) & ~0x3F);
    movie->mpegBuffer = next;
    next = (unsigned char *) (((unsigned int) next + 0x15040 + 0x3F) & ~0x3F);
    movie->readBuffer = next;
    next = (unsigned char *) (((unsigned int) next + 0x4000 + 0x3F) & ~0x3F);
    movie->dmaBuffer = next;
    next = (unsigned char *) (((unsigned int) next + 0x80000 + 0x3F) & ~0x3F);
    movie->reservedSize = 0xFD768;
    movie->dmaSize = 0x80000;
    movie->dmaWrite = 0x10;
    movie->dmaNextRead = 0x10;
    movie->soundBuffer = next;
    next = (unsigned char *) (((unsigned int) next + 0x20000 + 0x3F) & ~0x3F);
    movie->soundBufferSize = 0x20000;
    movie->dmaRead = 0;
    movie->soundWrite = 0;
    movie->soundRead = 0;
    xglCdStreamParamInit((struct CdStreamParam *) movie);
    movie->ringBuffer = next;
    movie->ringCapacity = audio_buffer_size;
    return (void *) (((unsigned int) next + audio_buffer_size + 0x3F) & ~0x3F);
}

void xglMpeg2InfoInit(void *info, void *arena)
{
    xglMpeg2InfoInit2(info, arena, 0x100000);
}

extern int sceIpuInit(void);

int xglMovieInit(void)
{
    return sceIpuInit();
}

const char D_004DC328[] = "%s\n";
