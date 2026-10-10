#include "common.h"

#include "shared.h"

const char D_004DC328[];

/*
 * xglMpeg2InfoInit2's own body is still INCLUDE_ASM below, but its tail at
 * 0x00222818..0x00222828 computes align64(arena + allocated_size +
 * audio_buffer_size) into v0 immediately before return: that value is the
 * function's result, not a void return. xglMpeg2InfoInit calls it and
 * discards the result, but the forward declaration used here must still
 * spell the pointer-return signature the body actually implements.
 */

extern void *xglMpeg2InfoInit2(void *info, void *arena, int audio_buffer_size);

typedef struct CdStreamParam CdStreamParam;

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

typedef struct XglXtxEntry {
    unsigned int imageWidth;
    unsigned int imageHeight;
    unsigned int imageReserved;
    unsigned int imageQwordOffset;
} XglXtxEntry;

typedef struct XglXtxHeader {
    unsigned long identity;
    unsigned long imageCount;
    XglXtxEntry image;
    unsigned long dataOffset;
    unsigned long zero28;
    unsigned long zero30;
    XglXtxEntry texture;
    unsigned long zero48;
} XglXtxHeader;

typedef struct XglDmaRefTag {
    unsigned int tag;
    unsigned int address;
    unsigned long upperQword;
} XglDmaRefTag;

typedef struct XglMovieSourceHeader XglMovieSourceHeader;

typedef struct XglMovieWork {
    unsigned char unmodeled_00[8];
    unsigned char streamState;      /* +0x08 */
    unsigned char unmodeled_09[0x0b];
    int sourceLength;               /* +0x14 */
    int streamingMode;              /* +0x18 */
    unsigned char unmodeled_1c[4];
    unsigned char *ringBuffer;      /* +0x20 */
    int ringCapacity;               /* +0x24 */
    int ringWrite;                  /* +0x28 */
    int ringRead;                   /* +0x2c */
    unsigned char unmodeled_30[0x10];
    short width;                    /* +0x40 */
    short height;                   /* +0x42 */
    short frameCount;               /* +0x44 */
    short currentFrame;             /* +0x46 */
    int stripQwords;                /* +0x48 */
    unsigned int imageQwords;       /* +0x4c */
    XglDmaRefTag chain[4];          /* +0x50 */
    XglMovieSourceHeader *source;   /* +0x90 */
    int sourceSize;                 /* +0x94 */
    unsigned char decodeState;      /* +0x98 */
    unsigned char streamOpen;       /* +0x99 */
    unsigned char loop;             /* +0x9a */
    unsigned char unmodeled_9b[4];
    unsigned char bitPosition;      /* +0x9f */
    unsigned int dmaControl;        /* +0xa0 */
    unsigned int dmaAddress;        /* +0xa4 */
    unsigned int dmaQwords;         /* +0xa8 */
    XglDmaRefTag *dmaTag;           /* +0xac */
    unsigned int imageAddress[2];   /* +0xb0 */
    unsigned int packetAddress[2];  /* +0xb8 */
    unsigned long long bitbltbuf;   /* +0xc0 */
    unsigned char bufferIndex;      /* +0xc8 */
    unsigned char transMode;        /* +0xc9 */
    unsigned char threshold1;       /* +0xca */
    unsigned char threshold0;       /* +0xcb */
    unsigned char unmodeled_cc[4];
} XglMovieWork;

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

extern int xglCdStreamClose(CdStreamParam *stream);

extern void xglCdStreamParamInit(struct CdStreamParam *param);

extern void xglCdStreamReadRingCore(struct CdStreamParam *stream);

extern void xglCdStreamReadRing(struct CdStreamParam *stream, int byte_count);

extern int xglCdStreamOpen();

extern void sceMpegInit(void);

extern void sceMpegCreate(void *mpeg, void *work, int work_size);

extern void sceMpegDelete(void *mpegHandle);

extern void sceMpegAddCallback(void *mpeg, int type, int (*callback)(), void *argument);

extern void sceMpegAddStrCallback(void *mpeg, int stream, int channel,
                                  int (*callback)(), void *argument);

extern int sceMpegDemuxPss(void *mpeg, unsigned char *source, int size);

extern int SsdGetResultValue(int *value);

extern void SsdInitVagStreamStereo(int size, int channels);

static int fillBuff(XglMovieInfo *info, int wait);

static int nodataCallback(int event, void *data, XglMovieInfo *info);

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

/*
 * xglCdStreamClose's own parameter type (CdStreamParam, main/tu092
 * src/main/xgl_cd.c) is TU-local to that file; this TU only forwards a
 * pointer to it and does not access its members, so the tag stays opaque.
 */

typedef struct CdStreamParam CdStreamParam;

extern void SsdDisposeVagStream(void);

extern void SsdStopVagStream(int channel);

extern void sceMpegReset(void *mpegHandle);

/*
 * arg0+0x30 (temp_16 = s1+0x30 in the original) is passed to sceMpegReset
 * and sceMpegDelete as a raw address; sceMpegReset/sceMpegDelete have no
 * public prototype (docs/naming.md), so the embedded MPEG handle stays an
 * untyped pointer computed by byte offset from the stream object.
 */

extern int sceIpuInit(void);

typedef XglMovieWork MoviePlaybackWork;

extern const char D_004D2580[];

typedef unsigned int Quadword __attribute__((mode(TI)));

typedef struct XglMpegState {
    int width;                          /* +0x00 */
    int height;                         /* +0x04 */
    int frameCount;                     /* +0x08 */
    unsigned char unmodeled_0c[0x14];
    unsigned long pictureType;          /* +0x20 */
} XglMpegState;

typedef struct XglMpeg2Work {
    unsigned char unmodeled_00[0x18];
    int errorState;                     /* +0x18 */
    unsigned char unmodeled_1c[0x14];
    XglMpegState mpeg;                  /* +0x30 */
    unsigned char unmodeled_58[0x28];
    unsigned char *videoBuffer;         /* +0x80 */
    unsigned char *mpegBuffer;          /* +0x84 */
    unsigned char unmodeled_88[8];
    int audioSampleRate;                /* +0x90 */
    unsigned char unmodeled_94[4];
    unsigned char decodeState;          /* +0x98 */
    unsigned char streamOpen;           /* +0x99 */
    unsigned char missCount;            /* +0x9a */
    unsigned char videoCount;           /* +0x9b */
    unsigned char unmodeled_9c[0x14];
    unsigned char *soundBuffer;         /* +0xb0 */
    int soundBufferSize;                /* +0xb4 */
    int soundWrite;                     /* +0xb8 */
    int soundRead;                      /* +0xbc */
} XglMpeg2Work;

struct XglMovieSourceHeader {
    unsigned int signature;             /* "ipum" */
    unsigned char unmodeled_04[4];
    unsigned short width;               /* +0x08 */
    unsigned short height;              /* +0x0a */
    unsigned short frameCount;          /* +0x0c */
    unsigned char unmodeled_0e[2];
};

#define D4_TADR   ((volatile u32 *) 0x1000b430)

#define IPU_BP    ((volatile u32 *) 0x10002020)

typedef struct XglMovieRenderState {
    unsigned char unmodeled_00[0x14];
    unsigned short bufferBase0;         /* +0x14 */
    unsigned char unmodeled_16[4];
    unsigned short bufferBase1;         /* +0x1a */
    unsigned char unmodeled_1c[0x40];
} XglMovieRenderState;

extern XglMovieRenderState sRender;

extern void SyncDCache(void *start, void *end);

int xglMovieClose(XglMovieInfo *info);

extern int sceMpegIsEnd(void *mpeg);

extern int sceMpegGetPicture(void *mpeg, unsigned char *destination, int size);

extern void SsdPlayVagStream(int channel, int start, int loop);

extern int SsdGetVagStreamStatusStereo(void);

extern void SsdSetVagStreamDataStereo(int channel, int address, int size);

extern int sceGsSyncPath(int mode, int timeout);

extern void xglDmaDirectSrcChain(unsigned int channel, unsigned int address);

extern void nmlModelDirectSend(int mode, u8 *data, int count);

extern unsigned long TestEnv_0_004A7B60[24];

extern int xglCdStreamRewind(struct CdStreamParam *stream);

static int checkdma(XglMovieWork *info);

/*
 * errorCallback (main:0x00222158) is registered as the MPEG movie decoder's
 * error handler. The original bytes read a pointer at arg1+0x04 and pass it
 * as the "%s\n" argument of D_004DC328's printf, then read, increment and
 * store back an unsigned byte at arg2+0x98 and clear a byte at arg2+0x99
 * right after it (lbu/sb at those fixed offsets).
 */

const char D_004D2590[];

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

static void setD4_CHCR(u32 value)
{
    DIntr();
    *D_ENABLEW = *D_ENABLEW | 0x10000;
    *D4_CHCR = value;
    *D_ENABLEW = *D_ENABLEW & 0xfffeffff;
    EIntr();
}

static unsigned long *make_gstrans(XglMovieWork *info, int y)
{
    int width = info->width;
    unsigned long *packet = (unsigned long *) info->packetAddress[info->bufferIndex];
    unsigned int image = info->imageAddress[info->bufferIndex];
    int x;

    packet[0] = 0x10000003;
    packet[2] = 0x1000000000008002;
    packet[3] = 0xe;
    packet[1] = 0;
    switch (info->transMode) {
    case 0:
    default:
        packet[4] = ((unsigned long) sRender.bufferBase0 << 37) | 0x8000000000000;
        break;
    case 1:
        packet[4] = ((unsigned long) sRender.bufferBase1 << 37) | 0x8000000000000;
        break;
    case 2:
        packet[4] = info->bitbltbuf;
        break;
    }
    packet[5] = 0x50;
    packet[6] = 0x1000000010;
    packet[7] = 0x52;
    packet += 8;
    for (x = 0; x < width; x += 16) {
        packet[1] = 0;
        packet[4] = ((unsigned long) x << 32) | ((unsigned long) y << 48);
        packet[0] = 0x10000004;
        packet[2] = 0x1000000000008002;
        packet[3] = 0xe;
        packet[5] = 0x51;
        packet[6] = 0;
        packet[7] = 0x53;
        packet[8] = 0x0800000000008040;
        packet[9] = 0;
        packet += 10;
        ((XglDmaRefTag *) packet)->address = image;
        image += 0x400;
        ((XglDmaRefTag *) packet)->tag = 0x30000040;
        ((XglDmaRefTag *) packet)->upperQword = 0;
        packet += 2;
    }
    ((XglDmaRefTag *) packet)[-1].tag &= 0x0fffffff;
    return packet;
}

static int checkdma(MoviePlaybackWork *info)
{
    int wait = 0;
    unsigned int consumed;

    if (info->decodeState == 99)
        return 0;
    setD4_CHCR(5);
    consumed = *D4_MADR - (unsigned int)info->source;
    switch (info->decodeState) {
    case 0:
        if ((unsigned int)info->sourceSize < consumed) {
            if (info->ringRead != info->ringWrite &&
                (unsigned int)(info->ringWrite - consumed) <
                (unsigned int)(info->sourceSize / 2) && !info->streamState) {
                wait = 1;
                break;
            }
            info->ringRead = info->sourceSize;
            if (!info->streamingMode) {
                *D4_TADR = (unsigned int)&info->chain[3];
                info->decodeState = 3;
            } else {
                *D4_TADR = (unsigned int)&info->chain[1];
                info->decodeState = 1;
            }
        } else {
            *D4_TADR = (unsigned int)&info->chain[2];
        }
        break;
    case 1:
        if (consumed < (unsigned int)info->sourceSize) {
            if (info->ringRead != info->ringWrite &&
                (unsigned int)(info->ringWrite - consumed) <
                (unsigned int)(info->sourceSize / 2) && !info->streamState) {
                wait = 1;
                break;
            }
            info->ringRead = 0;
            if (!info->streamingMode) {
                *D4_TADR = (unsigned int)&info->chain[3];
                info->decodeState = 2;
            } else {
                *D4_TADR = (unsigned int)&info->chain[2];
                info->decodeState = 0;
            }
        } else {
            *D4_TADR = (unsigned int)&info->chain[1];
        }
        break;
    case 2:
        if ((unsigned int)info->sourceSize < consumed)
            info->decodeState = 99;
        break;
    case 3:
        if (consumed < (unsigned int)info->sourceSize)
            info->decodeState = 99;
        break;
    }
    if (info->decodeState != 99)
        setD4_CHCR(0x30000105);
    else
        setD4_CHCR(0x105);
    return wait;
}

int xglMovieOpen(XglMovieWork *info, int stream)
{
    XglMovieSourceHeader *source;
    XglDmaRefTag *chain;

    if (stream) {
        info->ringCapacity = info->sourceSize * 2;
        info->ringBuffer = (unsigned char *) info->source;
        info->streamOpen = 1;
        xglCdStreamOpen((struct CdStreamParam *) info, stream);
        chain = info->chain;
        chain[0].tag = info->sourceSize / 16 - 1;
        chain[0].address = (unsigned int) (info->source + 1);
        chain[0].upperQword = 0;
        chain[1].tag = info->sourceSize / 16;
        chain[1].address = (unsigned int) info->source;
        chain[1].upperQword = 0;
        chain[2].tag = info->sourceSize / 16;
        chain[2].address = (unsigned int) info->source + info->sourceSize;
        chain[2].upperQword = 0;
        chain[3].tag = (info->sourceLength % info->sourceSize + 15) / 16;
        chain[3].address = (unsigned int) info->source
                                 + info->sourceSize * (info->sourceLength / info->sourceSize & 1);
        chain[3].upperQword = 0;
        xglCdStreamReadRing((struct CdStreamParam *) info, info->sourceSize);
        info->ringRead = 0;
        info->dmaControl = 0x105;
        info->dmaTag = chain;
        info->decodeState = 0;
        info->dmaAddress = 0;
        info->dmaQwords = 0;
        info->bitPosition = 0;
    } else {
        info->dmaControl = 0x105;
        info->dmaAddress = (unsigned int) (info->source + 1);
        info->dmaQwords = (info->sourceSize - 1) / 16;
        info->streamOpen = 0;
        info->dmaTag = 0;
        info->bitPosition = 0;
    }
    source = info->source;
    if (source->signature != 0x6d757069) {
        xglMovieClose((XglMovieInfo *) info);
        return 1;
    }
    info->width = source->width;
    info->stripQwords = (info->width >> 4) << 6;
    info->height = source->height;
    info->imageQwords = (info->height >> 4) * info->stripQwords;
    info->frameCount = source->frameCount;
    info->currentFrame = 0;
    info->bufferIndex = 0;
    SyncDCache(info, info + 1);
    return 0;
}

int xglMoviePlay(XglMovieWork *info)
{
    int strip;
    int retry;
    unsigned int header;
    int pitch;
    int x;
    int row;
    Quadword *source;
    Quadword *destination;

    if ((*D4_CHCR & 0x100) == 0) {
        *IPU_CTRL = 0x40000000;
        if (sceIpuSync(0, 0x40) < 0) {
            return -1;
        }
        *IPU_CMD = 0x90000000 | (info->threshold1 << 16) | info->threshold0;
        if (sceIpuSync(0, 0x40) < 0) {
            return -1;
        }
        *IPU_CMD = info->bitPosition;
        if (sceIpuSync(0, 0x40) < 0) {
            return -1;
        }
        *D4_MADR = info->dmaAddress;
        if (info->dmaQwords > 0xffff) {
            *D4_QWC = 0xffff;
        } else {
            *D4_QWC = info->dmaQwords;
        }
        *D4_TADR = (unsigned int) info->dmaTag;
        setD4_CHCR(info->dmaControl);
    }
    if (info->streamOpen) {
        if (checkdma(info) != 0) {
            printf(D_004D2580);
            return -1;
        }
    }
    if (info->transMode != 0xff) {
        sceGsSyncPath(0, 0);
        for (strip = 0; strip < info->height; strip += 16) {
            myDmaDirectFromIPU(info->imageAddress[info->bufferIndex], info->stripQwords);
            if (strip == 0) {
                *IPU_CMD = 0x40000000;
                if (sceIpuSync(0, 0x40) < 0) {
                    return -1;
                }
                header = *IPU_CMD >> 24;
                *IPU_CMD = 0x40000008;
                if (sceIpuSync(0, 0x40) < 0) {
                    return -1;
                }
                *IPU_CTRL = (header & 0xfb) << 16;
                *IPU_CMD = (((header & 4) >> 2) << 24) | 0x10010000;
            }
            make_gstrans(info, strip);
            for (retry = 0; retry <= 0xFFFFFF; retry++) {
                if ((*D3_CHCR & 0x100) == 0) {
                    break;
                }
            }
            xglDmaDirectSrcChain(2, info->packetAddress[info->bufferIndex]);
            info->bufferIndex ^= 1;
        }
        sceGsSyncPath(0, 0);
    } else {
        for (strip = 0; strip < info->height; strip += 16) {
            myDmaDirectFromIPU(0x70000000, info->stripQwords);
            if (strip == 0) {
                *IPU_CMD = 0x40000000;
                if (sceIpuSync(0, 0x40) < 0) {
                    return -1;
                }
                header = *IPU_CMD >> 24;
                *IPU_CMD = 0x40000008;
                if (sceIpuSync(0, 0x40) < 0) {
                    return -1;
                }
                *IPU_CTRL = (header & 0xfb) << 16;
                *IPU_CMD = (((header & 4) >> 2) << 24) | 0x10010000;
            }
            for (retry = 0; retry <= 0xFFFFFF; retry++) {
                if ((*D3_CHCR & 0x100) == 0) {
                    break;
                }
            }
            pitch = info->width / 4;
            source = (Quadword *) 0x70000000;
            destination = (Quadword *) (info->imageAddress[info->bufferIndex] + info->width * strip * 4);
            for (x = 0; x < info->width; x += 16) {
                for (row = 15; row >= 0; row--) {
                    destination[0] = source[0];
                    destination[1] = source[1];
                    destination[2] = source[2];
                    destination[3] = source[3];
                    source += 4;
                    destination += pitch;
                }
                destination = destination - pitch * 16 + 4;
            }
        }
        info->bufferIndex ^= 1;
    }
    if (sceIpuSync(0, 0x40) < 0) {
        return -1;
    }
    *IPU_CMD = 0x40000020;
    if (sceIpuSync(0, 0x40) < 0) {
        return -1;
    }
    info->currentFrame++;
    if (info->currentFrame >= info->frameCount) {
        if (info->loop == 0) {
            info->currentFrame = info->frameCount;
        } else {
            info->currentFrame = 0;
            setD4_CHCR(5);
            DIntr();
            *D_ENABLEW = *D_ENABLEW | 0x10000;
            *D3_CHCR = 0;
            *D_ENABLEW = *D_ENABLEW & 0xfffeffff;
            EIntr();
            if (info->streamOpen) {
                xglCdStreamRewind((struct CdStreamParam *) info);
                info->ringWrite = 0;
                info->ringRead = 0;
                xglCdStreamReadRing((struct CdStreamParam *) info, info->sourceSize);
                info->ringRead = 0;
                info->dmaControl = 0x105;
                info->dmaTag = info->chain;
                info->decodeState = 0;
                info->dmaAddress = 0;
                info->dmaQwords = 0;
                info->bitPosition = 0;
            } else {
                info->dmaControl = 0x105;
                info->dmaAddress = (unsigned int) (info->source + 1);
                info->dmaQwords = (info->sourceSize - 1) / 16;
                info->dmaTag = 0;
                info->bitPosition = 0;
            }
            SyncDCache(info, info + 1);
        }
        return 1;
    }
    if (info->streamOpen) {
        unsigned int bitstream;

        info->dmaControl = *D4_CHCR;
        setD4_CHCR(5);
        bitstream = *IPU_BP;
        info->dmaAddress = *D4_MADR - (((bitstream >> 16) & 3) + ((bitstream >> 8) & 0xf)) * 16;
        info->dmaQwords = *D4_QWC + (((bitstream >> 16) & 3) + ((bitstream >> 8) & 0xf));
        info->dmaTag = (XglDmaRefTag *) *D4_TADR;
        info->bitPosition = bitstream & 0x7f;
    } else {
        unsigned int bitstream;
        unsigned int consumed;

        info->dmaControl = *D4_CHCR;
        setD4_CHCR(5);
        bitstream = *IPU_BP;
        consumed = *D4_MADR - info->dmaAddress - (((bitstream >> 16) & 3) + ((bitstream >> 8) & 0xf)) * 16;
        info->dmaAddress += consumed;
        info->dmaQwords -= consumed / 16;
        info->dmaTag = (XglDmaRefTag *) *D4_TADR;
        info->bitPosition = bitstream & 0x7f;
    }
    return 0;
}

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

XglXtxHeader *xglMovieMakeXtxHeader(XglMovieWork *info, XglXtxHeader *header)
{
    unsigned int base = info->imageQwords;
    XglXtxEntry *entry = &header->image;

    header->identity = 0x5000585458;
    header->imageCount = 0x1000000001;
    entry->imageWidth = 0x40000 + info->width;
    entry->imageHeight = info->height;
    entry->imageReserved = 0;
    entry->imageQwordOffset = base + 2;
    header->dataOffset = 0x30;
    header->zero28 = 0;
    header->zero30 = 0;
    entry = &header->texture;
    entry->imageWidth = 0;
    entry->imageHeight = 0x50000001 + base;
    entry->imageReserved = 0x8000 + base;
    entry->imageQwordOffset = 0x08000000;
    header->zero48 = 0;
    return header + 1;
}

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

static void setLoadImageTags(unsigned int destination, unsigned int image, int width, int height)
{
    unsigned long *packet = UNCACHED(destination);
    int x;
    int y;

    packet[0] = 0x10000003;
    packet[2] = 0x1000000000008002;
    packet[3] = 0xe;
    packet[4] = 0xe000L << 36;
    packet[5] = 0x50;
    packet[6] = 0x1000000010;
    packet[7] = 0x52;
    packet[1] = 0;
    packet += 8;
    for (x = 0; x < width; x += 16) {
        for (y = 0; y < height; y += 16) {
            packet[1] = 0;
            packet[0] = 0x10000004;
            packet[2] = 0x1000000000008002;
            packet[3] = 0xe;
            packet[4] = ((long) x << 32) | ((long) y << 48);
            packet[5] = 0x51;
            packet[6] = 0;
            packet[7] = 0x53;
            packet[8] = 0x0800000000008040;
            packet[9] = 0;
            packet += 10;
            ((XglDmaRefTag *) packet)->address = image;
            image += 0x400;
            ((XglDmaRefTag *) packet)->tag = 0x30000040;
            ((XglDmaRefTag *) packet)->upperQword = 0;
            packet += 2;
        }
    }
    ((XglDmaRefTag *) packet)[-1].tag &= 0x0fffffff;
}

int xglMpeg2Play(XglMpeg2Work *info)
{
    int result;
    int status;
    int free;
    int i;
    unsigned long *packet;
    unsigned long *env;

    if (info->decodeState >= 5 && info->errorState != 0) {
        return -1;
    }
    if (sceMpegIsEnd(&info->mpeg)) {
        return info->missCount ? -1 : 1;
    }
    if (sceMpegGetPicture(&info->mpeg, info->videoBuffer, 896) < 0) {
        printf(D_004D2590);
        return -1;
    }
    info->missCount = 0;
    if ((info->mpeg.pictureType & 7) == 1) {
        if (info->videoCount == 0) {
            info->decodeState++;
        }
        info->streamOpen++;
        info->videoCount = 0;
        if (info->decodeState != 0 && info->streamOpen >= 5) {
            info->streamOpen = 0;
            info->decodeState--;
        }
    }
    if (info->mpeg.frameCount == 0) {
        setLoadImageTags((unsigned int) info->mpegBuffer, (unsigned int) info->videoBuffer,
                         info->mpeg.width, info->mpeg.height);
    }
    if (info->mpeg.frameCount == 0) {
        SsdPlayVagStream(0x7f, 0, (info->audioSampleRate << 12) / 48000);
        do {
        } while (SsdGetResultValue(&result) < 0);
    }
    free = info->soundWrite - info->soundRead;
    if (free <= 0) {
        free += info->soundBufferSize;
    }
    if (free > 0x1000) {
        SsdGetVagStreamStatusStereo();
        do {
        } while (SsdGetResultValue(&status) < 0);
        if (status >> 8 != 0) {
            SsdSetVagStreamDataStereo(0, (int) (info->soundBuffer + info->soundRead), 0x1000);
            info->soundRead = (info->soundRead + 0x1000) % info->soundBufferSize;
        }
    }
    for (i = 15; i >= 0; i--) {
        fillBuff((XglMovieInfo *) info, 0);
    }
    sceGsSyncPath(0, 0);
    packet = UNCACHED(info->mpegBuffer);
    packet[4] = ((unsigned long) sRender.bufferBase0 << 37) | 0x8000000000000;
    xglDmaDirectSrcChain(2, (unsigned int) info->mpegBuffer);
    sceGsSyncPath(0, 0);
    env = TestEnv_0_004A7B60;
    env[8] = (0x24020000 | sRender.bufferBase0 << 5) | 0x2000000640000000;
    nmlModelDirectSend(5, (u8 *) env, 12);
    return 0;
}

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

int xglMovieInit(void)
{
    return sceIpuInit();
}

const char D_004DC328[] = "%s\n";
const char D_004D2590[] = {'s', 'c', 'e', 'M', 'p', 'e', 'g', 'G', 'e', 't', 'P', 'i', 'c', 't', 'u', 'r', 'e', ' ', 'f', 'a', 'i', 'l', 'e', 'd', 10, 0};


