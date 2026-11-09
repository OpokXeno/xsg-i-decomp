#include "common.h"
#include "shared.h"
#include "main/nml_model_set.h"
#include "main/ssd_3.h"
#include "main/ssd_init.h"
#include "main/xgl_dma.h"
#include "main/xgl_packet.h"

/* D_ENABLEW bit 16 (CPND): holds all DMA transfers while a channel register is written. */
#define D_ENABLE_HOLD 0x10000

/* Dn_CHCR values of the IPU DMA channels (D3 = IPU_from, D4 = IPU_to). */
#define MOVIE_D4_CHCR_CHAIN_STOP (DMA_CHCR_MOD_CHAIN | DMA_CHCR_DIR)
#define MOVIE_D4_CHCR_CHAIN_START (DMA_CHCR_STR | DMA_CHCR_MOD_CHAIN | DMA_CHCR_DIR)
#define MOVIE_D4_CHCR_CHAIN_START_REF (DMA_TAG_ID_REF | MOVIE_D4_CHCR_CHAIN_START)
#define MOVIE_D4_CHCR_NORMAL_START (DMA_CHCR_STR | DMA_CHCR_DIR)

/* IPU registers: IPU_CTRL.RST and the IPU commands (opcode in bits 28..31). */
#define IPU_CTRL_RESET 0x40000000
#define IPU_CMD_BCLR 0
#define IPU_CMD_IDEC 0x10000000
#define IPU_CMD_FDEC 0x40000000
#define IPU_CMD_SETTH 0x90000000
#define IPU_IDEC_QSC_1 0x10000 /* quantiser step code 1 */
/* sceIpuSync timeout, and the busy-poll bound used throughout the movie code. */
#define IPU_SYNC_TIMEOUT 0x40
#define MOVIE_POLL_LIMIT 0xFFFFFF

/* XTX texture header written by xglMovieMakeXtxHeader: "XTX" signature word and the offset of the pixel data. */
#define XTX_HEADER_IDENTITY 0x5000585458
#define XTX_DATA_OFFSET 0x30

/* The "ipum" signature of the movie source header. */
#define MOVIE_SIGNATURE_IPUM 0x6d757069
/* checkdma: the DMA is finished once decodeState reaches this value. */
#define MOVIE_DECODE_DONE 99
/* transMode values: which GS buffer a decoded strip is uploaded to, or memory only. */
#define MOVIE_TRANS_BUFFER_0 0
#define MOVIE_TRANS_BUFFER_1 1
#define MOVIE_TRANS_BITBLTBUF 2
#define MOVIE_TRANS_MEMORY 0xff
/* A strip is one 16-pixel-high row of macroblocks, each macroblock 16x16 RGBA32 pixels. */
#define MOVIE_MACROBLOCK 16
#define MOVIE_MACROBLOCK_BYTES 0x400
#define MOVIE_MACROBLOCK_QWORDS 0x40
#define MOVIE_DMA_QWORD 16
#define MOVIE_DMA_MAX_QWORDS 0xffff
/* GIF packets of the strip upload: A+D settings, then one IMAGE tag per macroblock. */
#define MOVIE_GIF_TAG_AD_2 (GIF_TAG_NREG_1 | GIF_TAG_EOP | 2)
#define MOVIE_GIF_TAG_IMAGE_BLOCK (GIF_TAG_FLG_IMAGE | GIF_TAG_EOP | MOVIE_MACROBLOCK_QWORDS)
#define MOVIE_BITBLTBUF_DBW_512 0x8000000000000
#define MOVIE_BITBLTBUF_DBW_896 (0xe000L << 36)
#define MOVIE_TRXREG_MACROBLOCK 0x1000000010
/* Scratchpad staging areas. */
#define MOVIE_PACKET_ADDRESS_0 (SCRATCHPAD_BASE + 0x40)
#define MOVIE_PACKET_ADDRESS_1 (SCRATCHPAD_BASE + 0x1000)

/* MPEG2 stream: callback and stream identifiers of the sceMpeg API. */
#define SCE_MPEG_CB_ERROR 0
#define SCE_MPEG_CB_NODATA 1
#define SCE_MPEG_STR_M2V 0
#define SCE_MPEG_STR_ADPCM 3
#define MPEG_PICTURE_TYPE_MASK 7
#define MPEG_PICTURE_TYPE_I 1
/* nodataCallback gives up feeding after this many consecutive misses. */
#define MPEG_MISS_LIMIT 0x41
#define MPEG_NODATA_DECODE_STATE 0x10
/* Sound handed to the SSD VAG stream in 4 KiB chunks. */
#define MPEG_AUDIO_CHUNK_BYTES 0x1000
#define MPEG_AUDIO_VOLUME_MAX 0x7f
#define MPEG_AUDIO_BASE_RATE 48000
/* PSS audio packet: first packet carries a 0x2C-byte header with the little-endian sample rate at +0x10. */
#define MPEG_AUDIO_HEADER_BYTES 0x2C
#define MPEG_AUDIO_HEADER_RATE_OFFSET 0x10
#define MPEG_AUDIO_PACKET_HEADER_BYTES 4
/* Sizes of the buffers carved out of the movie arena. */
#define MPEG_WORK_SIZE 0xFD768
#define MPEG_VIDEO_BUFFER_SIZE 0xE0000
#define MPEG_PACKET_BUFFER_SIZE 0x15040
#define MPEG_READ_BUFFER_SIZE 0x4000
#define MPEG_DMA_BUFFER_SIZE 0x80000
#define MPEG_SOUND_BUFFER_SIZE 0x20000
#define MPEG_RING_BUFFER_SIZE 0x100000
#define MPEG_DMA_START_OFFSET 0x10
/* nodataCallback feeds the IPU in chunks of this many bytes. */
#define MPEG_DMA_CHUNK_BYTES 0x1000

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

#define D4_TADR   ((volatile u32 *) 0x1000b430)
#define IPU_BP    ((volatile u32 *) 0x10002020)

typedef struct CdStreamParam CdStreamParam;

/* The sceMpeg decoder handle embedded in the MPEG2 movie record. */
typedef struct XglMpegState {
    int width;                          /* +0x00 */
    int height;                         /* +0x04 */
    int frameCount;                     /* +0x08 */
    unsigned char unmodeled_0c[0x14];
    unsigned long pictureType;          /* +0x20 */
} XglMpegState;

typedef struct XglMovieInfo {
    unsigned char unmodeled_00[8];
    unsigned char streamState;          /* +0x08 */
    unsigned char unmodeled_09[0x0f];
    int errorState;                     /* +0x18 */
    unsigned char unmodeled_1c[4];
    unsigned char *ringBuffer;          /* +0x20 */
    int ringCapacity;                   /* +0x24 */
    int ringWrite;                      /* +0x28 */
    int ringRead;                       /* +0x2c */
    XglMpegState mpeg;                  /* +0x30 */
    unsigned char unmodeled_58[0x20];
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
    unsigned int length;                /* +0x0c */
} MpegStreamData;

typedef struct {
    unsigned char unmodeled_00[4];
    const char *message;
} MpegErrorInfo;

typedef struct {
    unsigned char unmodeled_00[0x98];
    unsigned char errorCount;
    unsigned char errorFlag;
} MpegCallbackState;

/*
 * CdStreamParam is only forwarded here as a pointer. This file does not access
 * its members, so the tag stays opaque.
 */
typedef struct CdStreamParam CdStreamParam;

typedef unsigned int Quadword __attribute__((mode(TI)));

struct XglMovieSourceHeader {
    unsigned int signature;             /* "ipum" */
    unsigned char unmodeled_04[4];
    unsigned short width;               /* +0x08 */
    unsigned short height;              /* +0x0a */
    unsigned short frameCount;          /* +0x0c */
    unsigned char unmodeled_0e[2];
};

typedef struct XglMovieRenderState {
    unsigned char unmodeled_00[0x14];
    unsigned short bufferBase0;         /* +0x14 */
    unsigned char unmodeled_16[4];
    unsigned short bufferBase1;         /* +0x1a */
    unsigned char unmodeled_1c[0x40];
} XglMovieRenderState;

const char errorMessageFormat[];
extern const char frameDropMessage[];
extern XglMovieRenderState sRender;

const char mpegGetPictureFailedMessage[];

/* xglMpeg2InfoInit2 returns a pointer, which xglMpeg2InfoInit discards. */
extern void *xglMpeg2InfoInit2(void *info, void *arena, int ring_size);

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
static int fillBuff(XglMovieInfo *info, int wait);
static int nodataCallback(int event, void *data, XglMovieInfo *info);
extern int printf(const char *format, ...);
/* sceMpegReset and sceMpegDelete have no public prototype, so the embedded MPEG handle stays an untyped pointer. */
extern void sceMpegReset(void *mpegHandle);

extern int sceIpuInit(void);

extern void SyncDCache(void *start, void *end);
int xglMovieClose(XglMovieInfo *info);
extern int sceMpegIsEnd(void *mpeg);
extern int sceMpegGetPicture(void *mpeg, unsigned char *destination, int size);
extern int sceGsSyncPath(int mode, int timeout);
extern int xglCdStreamRewind(struct CdStreamParam *stream);
static int checkdma(XglMovieWork *info);

const char errorMessageFormat[] = "%s\n";

/* Starts a DMA transfer from the IPU (channel D3) of `count` quadwords to `address`. */
static int myDmaDirectFromIPU(u32 address, u32 count)
{
    if ((address >> 16) != DMA_SPR_ADDRESS_HIGH)
        *D3_MADR = address;
    else
        *D3_MADR = (address & DMA_SPR_OFFSET_MASK) + DMA_SPR_FLAG;
    *D3_QWC = count;

    DIntr();
    *D_ENABLEW = *D_ENABLEW | D_ENABLE_HOLD;
    *D3_CHCR = DMA_CHCR_STR;
    *D_ENABLEW = *D_ENABLEW & ~D_ENABLE_HOLD;
    return EIntr();
}

/* Writes the control register of DMA channel D4 with the transfer hold set. */
static void setD4_CHCR(u32 value)
{
    DIntr();
    *D_ENABLEW = *D_ENABLEW | D_ENABLE_HOLD;
    *D4_CHCR = value;
    *D_ENABLEW = *D_ENABLEW & ~D_ENABLE_HOLD;
    EIntr();
}

/*
 * Builds the GIF packet that uploads one row of macroblocks at row `y` into the
 * current transfer buffer, and returns the packet end.
 */
static unsigned long *make_gstrans(XglMovieWork *info, int y)
{
    int width = info->width;
    unsigned long *packet = (unsigned long *) info->packetAddress[info->bufferIndex];
    unsigned int image = info->imageAddress[info->bufferIndex];
    int x;

    packet[0] = DMA_TAG_ID_CNT | 3;
    packet[2] = MOVIE_GIF_TAG_AD_2;
    packet[3] = GIF_REG_AD;
    packet[1] = 0;
    switch (info->transMode) {
    case MOVIE_TRANS_BUFFER_0:
    default:
        packet[4] = ((unsigned long) sRender.bufferBase0 << 37) | MOVIE_BITBLTBUF_DBW_512;
        break;
    case MOVIE_TRANS_BUFFER_1:
        packet[4] = ((unsigned long) sRender.bufferBase1 << 37) | MOVIE_BITBLTBUF_DBW_512;
        break;
    case MOVIE_TRANS_BITBLTBUF:
        packet[4] = info->bitbltbuf;
        break;
    }
    packet[5] = SCE_GS_BITBLTBUF;
    packet[6] = MOVIE_TRXREG_MACROBLOCK;
    packet[7] = SCE_GS_TRXREG;
    packet += 8;
    for (x = 0; x < width; x += MOVIE_MACROBLOCK) {
        packet[1] = 0;
        packet[4] = ((unsigned long) x << 32) | ((unsigned long) y << 48);
        packet[0] = DMA_TAG_ID_CNT | 4;
        packet[2] = MOVIE_GIF_TAG_AD_2;
        packet[3] = GIF_REG_AD;
        packet[5] = SCE_GS_TRXPOS;
        packet[6] = 0;
        packet[7] = SCE_GS_TRXDIR;
        packet[8] = MOVIE_GIF_TAG_IMAGE_BLOCK;
        packet[9] = 0;
        packet += 10;
        ((XglDmaRefTag *) packet)->address = image;
        image += MOVIE_MACROBLOCK_BYTES;
        ((XglDmaRefTag *) packet)->tag = DMA_TAG_ID_REF | MOVIE_MACROBLOCK_QWORDS;
        ((XglDmaRefTag *) packet)->upperQword = 0;
        packet += 2;
    }
    ((XglDmaRefTag *) packet)[-1].tag &= DMA_TAG_ID_CLEAR_MASK;
    return packet;
}

/*
 * Advances the stream's DMA chain once the IPU has consumed the current source
 * chunk. Returns 1 when the caller must wait for more stream data.
 */
static int checkdma(XglMovieWork *info)
{
    int wait = 0;
    unsigned int consumed;

    if (info->decodeState == MOVIE_DECODE_DONE)
        return 0;
    setD4_CHCR(MOVIE_D4_CHCR_CHAIN_STOP);
    consumed = *D4_MADR - (unsigned int)info->source;
    /* decodeState selects which chain entry the DMA continues with. */
    switch (info->decodeState) {
    case 0:
        if ((unsigned int)info->sourceSize < consumed) {
            if (info->ringRead != info->ringWrite &&
                (info->ringWrite - consumed) <
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
                (info->ringWrite - consumed) <
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
            info->decodeState = MOVIE_DECODE_DONE;
        break;
    case 3:
        if (consumed < (unsigned int)info->sourceSize)
            info->decodeState = MOVIE_DECODE_DONE;
        break;
    }
    if (info->decodeState != MOVIE_DECODE_DONE)
        setD4_CHCR(MOVIE_D4_CHCR_CHAIN_START_REF);
    else
        setD4_CHCR(MOVIE_D4_CHCR_CHAIN_START);
    return wait;
}

/*
 * Prepares a movie for playback: a CD stream when `stream` is set, otherwise the
 * source already in memory. Returns 1 when the source header signature is wrong.
 */
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
        chain[0].tag = info->sourceSize / MOVIE_DMA_QWORD - 1;
        chain[0].address = (unsigned int) (info->source + 1);
        chain[0].upperQword = 0;
        chain[1].tag = info->sourceSize / MOVIE_DMA_QWORD;
        chain[1].address = (unsigned int) info->source;
        chain[1].upperQword = 0;
        chain[2].tag = info->sourceSize / MOVIE_DMA_QWORD;
        chain[2].address = (unsigned int) info->source + info->sourceSize;
        chain[2].upperQword = 0;
        chain[3].tag = (info->sourceLength % info->sourceSize + 15) / MOVIE_DMA_QWORD;
        chain[3].address = (unsigned int) info->source
                                 + info->sourceSize * (info->sourceLength / info->sourceSize & 1);
        chain[3].upperQword = 0;
        xglCdStreamReadRing((struct CdStreamParam *) info, info->sourceSize);
        info->ringRead = 0;
        info->dmaControl = MOVIE_D4_CHCR_CHAIN_START;
        info->dmaTag = chain;
        info->decodeState = 0;
        info->dmaAddress = 0;
        info->dmaQwords = 0;
        info->bitPosition = 0;
    } else {
        info->dmaControl = MOVIE_D4_CHCR_CHAIN_START;
        info->dmaAddress = (unsigned int) (info->source + 1);
        info->dmaQwords = (info->sourceSize - 1) / MOVIE_DMA_QWORD;
        info->streamOpen = 0;
        info->dmaTag = 0;
        info->bitPosition = 0;
    }
    source = info->source;
    if (source->signature != MOVIE_SIGNATURE_IPUM) {
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

/*
 * Decodes the next frame through the IPU and uploads it, either to the GS
 * transfer buffers or to memory. Returns 1 when the last frame is reached,
 * -1 on an IPU sync failure or a dropped frame, and 0 otherwise.
 */
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

    /* Reset the IPU and restart the decode DMA when the channel has stopped. */
    if ((*D4_CHCR & DMA_CHCR_STR) == 0) {
        *IPU_CTRL = IPU_CTRL_RESET;
        if (sceIpuSync(0, IPU_SYNC_TIMEOUT) < 0) {
            return -1;
        }
        *IPU_CMD = IPU_CMD_SETTH | (info->threshold1 << 16) | info->threshold0;
        if (sceIpuSync(0, IPU_SYNC_TIMEOUT) < 0) {
            return -1;
        }
        *IPU_CMD = info->bitPosition;
        if (sceIpuSync(0, IPU_SYNC_TIMEOUT) < 0) {
            return -1;
        }
        *D4_MADR = info->dmaAddress;
        if (info->dmaQwords > MOVIE_DMA_MAX_QWORDS) {
            *D4_QWC = MOVIE_DMA_MAX_QWORDS;
        } else {
            *D4_QWC = info->dmaQwords;
        }
        *D4_TADR = (unsigned int) info->dmaTag;
        setD4_CHCR(info->dmaControl);
    }
    if (info->streamOpen) {
        if (checkdma(info) != 0) {
            printf(frameDropMessage);
            return -1;
        }
    }
    /* Upload each strip through the GS transfer buffers. */
    if (info->transMode != MOVIE_TRANS_MEMORY) {
        sceGsSyncPath(0, 0);
        for (strip = 0; strip < info->height; strip += MOVIE_MACROBLOCK) {
            myDmaDirectFromIPU(info->imageAddress[info->bufferIndex], info->stripQwords);
            if (strip == 0) {
                *IPU_CMD = IPU_CMD_FDEC;
                if (sceIpuSync(0, IPU_SYNC_TIMEOUT) < 0) {
                    return -1;
                }
                header = *IPU_CMD >> 24;
                *IPU_CMD = IPU_CMD_FDEC | 8;
                if (sceIpuSync(0, IPU_SYNC_TIMEOUT) < 0) {
                    return -1;
                }
                *IPU_CTRL = (header & 0xfb) << 16;
                *IPU_CMD = (((header & 4) >> 2) << 24) | (IPU_CMD_IDEC | IPU_IDEC_QSC_1);
            }
            make_gstrans(info, strip);
            for (retry = 0; retry <= MOVIE_POLL_LIMIT; retry++) {
                if ((*D3_CHCR & DMA_CHCR_STR) == 0) {
                    break;
                }
            }
            xglDmaDirectSrcChain(XGL_DMA_CHANNEL_GIF, info->packetAddress[info->bufferIndex]);
            info->bufferIndex ^= 1;
        }
        sceGsSyncPath(0, 0);
    } else {
        /* Copy each strip from the scratchpad into the image buffer in memory. */
        for (strip = 0; strip < info->height; strip += MOVIE_MACROBLOCK) {
            myDmaDirectFromIPU(SCRATCHPAD_BASE, info->stripQwords);
            if (strip == 0) {
                *IPU_CMD = IPU_CMD_FDEC;
                if (sceIpuSync(0, IPU_SYNC_TIMEOUT) < 0) {
                    return -1;
                }
                header = *IPU_CMD >> 24;
                *IPU_CMD = IPU_CMD_FDEC | 8;
                if (sceIpuSync(0, IPU_SYNC_TIMEOUT) < 0) {
                    return -1;
                }
                *IPU_CTRL = (header & 0xfb) << 16;
                *IPU_CMD = (((header & 4) >> 2) << 24) | (IPU_CMD_IDEC | IPU_IDEC_QSC_1);
            }
            for (retry = 0; retry <= MOVIE_POLL_LIMIT; retry++) {
                if ((*D3_CHCR & DMA_CHCR_STR) == 0) {
                    break;
                }
            }
            pitch = info->width / 4;
            source = (Quadword *) SCRATCHPAD_BASE;
            destination = (Quadword *) (info->imageAddress[info->bufferIndex] + info->width * strip * 4);
            for (x = 0; x < info->width; x += MOVIE_MACROBLOCK) {
                for (row = MOVIE_MACROBLOCK - 1; row >= 0; row--) {
                    destination[0] = source[0];
                    destination[1] = source[1];
                    destination[2] = source[2];
                    destination[3] = source[3];
                    source += 4;
                    destination += pitch;
                }
                destination = destination - pitch * MOVIE_MACROBLOCK + 4;
            }
        }
        info->bufferIndex ^= 1;
    }
    if (sceIpuSync(0, IPU_SYNC_TIMEOUT) < 0) {
        return -1;
    }
    *IPU_CMD = IPU_CMD_FDEC | 0x20;
    if (sceIpuSync(0, IPU_SYNC_TIMEOUT) < 0) {
        return -1;
    }
    info->currentFrame++;
    if (info->currentFrame >= info->frameCount) {
        if (info->loop == 0) {
            info->currentFrame = info->frameCount;
        } else {
            info->currentFrame = 0;
            setD4_CHCR(MOVIE_D4_CHCR_CHAIN_STOP);
            DIntr();
            *D_ENABLEW = *D_ENABLEW | D_ENABLE_HOLD;
            *D3_CHCR = 0;
            *D_ENABLEW = *D_ENABLEW & ~D_ENABLE_HOLD;
            EIntr();
            if (info->streamOpen) {
                xglCdStreamRewind((struct CdStreamParam *) info);
                info->ringWrite = 0;
                info->ringRead = 0;
                xglCdStreamReadRing((struct CdStreamParam *) info, info->sourceSize);
                info->ringRead = 0;
                info->dmaControl = MOVIE_D4_CHCR_CHAIN_START;
                info->dmaTag = info->chain;
                info->decodeState = 0;
                info->dmaAddress = 0;
                info->dmaQwords = 0;
                info->bitPosition = 0;
            } else {
                info->dmaControl = MOVIE_D4_CHCR_CHAIN_START;
                info->dmaAddress = (unsigned int) (info->source + 1);
                info->dmaQwords = (info->sourceSize - 1) / MOVIE_DMA_QWORD;
                info->dmaTag = 0;
                info->bitPosition = 0;
            }
            SyncDCache(info, info + 1);
        }
        return 1;
    }
    /* Save where the next DMA resumes, using the IPU bit pointer. */
    if (info->streamOpen) {
        unsigned int bitstream;

        info->dmaControl = *D4_CHCR;
        setD4_CHCR(MOVIE_D4_CHCR_CHAIN_STOP);
        bitstream = *IPU_BP;
        info->dmaAddress = *D4_MADR - (((bitstream >> 16) & 3) + ((bitstream >> 8) & 0xf)) * 16;
        info->dmaQwords = *D4_QWC + (((bitstream >> 16) & 3) + ((bitstream >> 8) & 0xf));
        info->dmaTag = (XglDmaRefTag *) *D4_TADR;
        info->bitPosition = bitstream & 0x7f;
    } else {
        unsigned int bitstream;
        unsigned int consumed;

        info->dmaControl = *D4_CHCR;
        setD4_CHCR(MOVIE_D4_CHCR_CHAIN_STOP);
        bitstream = *IPU_BP;
        consumed = *D4_MADR - info->dmaAddress - (((bitstream >> 16) & 3) + ((bitstream >> 8) & 0xf)) * 16;
        info->dmaAddress += consumed;
        info->dmaQwords -= consumed / MOVIE_DMA_QWORD;
        info->dmaTag = (XglDmaRefTag *) *D4_TADR;
        info->bitPosition = bitstream & 0x7f;
    }
    return 0;
}

/* Closes the movie stream if one is open and resets the IPU. Returns -1 if an IPU sync fails. */
int xglMovieClose(XglMovieInfo *info)
{
    if (info->streamOpen) {
        xglCdStreamClose((struct CdStreamParam *) info);
    }
    setD4_CHCR(0);
    *IPU_CTRL = IPU_CTRL_RESET;
    if (sceIpuSync(0, IPU_SYNC_TIMEOUT) < 0) {
        return -1;
    }
    *IPU_CMD = IPU_CMD_BCLR;
    return sceIpuSync(0, IPU_SYNC_TIMEOUT) < 0 ? -1 : 0;
}

/* Sets the default packet addresses and transfer thresholds of a movie init record. */
void xglMovieInfoInit(XglMovieInitInfo *info)
{
    xglCdStreamParamInit((struct CdStreamParam *) info);
    info->packetAddress[0] = MOVIE_PACKET_ADDRESS_0;
    info->packetAddress[1] = MOVIE_PACKET_ADDRESS_1;
    info->transMode = info->currentFrame = -1;
    info->missCount = 0;
    info->threshold1 = 0;
    info->threshold0 = 0;
}

/* Fills an XTX texture header for the movie frame and returns the address just past it. */
XglXtxHeader *xglMovieMakeXtxHeader(XglMovieWork *info, XglXtxHeader *header)
{
    unsigned int base = info->imageQwords;
    XglXtxEntry *entry = &header->image;

    header->identity = XTX_HEADER_IDENTITY;
    header->imageCount = 0x1000000001;
    entry->imageWidth = 0x40000 + info->width;
    entry->imageHeight = info->height;
    entry->imageReserved = 0;
    entry->imageQwordOffset = base + 2;
    header->dataOffset = XTX_DATA_OFFSET;
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

/* Copies the next stream bytes from the ring buffer into the read buffer, wrapping at the ring end. Returns the byte count. */
static int fileRead(XglMovieInfo *info)
{
    unsigned char *destination;
    int retry;
    int length;
    int available;
    int tail;

    available = 0;
    length = MPEG_READ_BUFFER_SIZE;
    destination = info->readBuffer;
    for (retry = 0; retry <= MOVIE_POLL_LIMIT; retry++) {
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

/* Feeds the MPEG demuxer from the stream, waiting for data when `wait` is set. Returns the bytes consumed. */
static int fillBuff(XglMovieInfo *info, int wait)
{
    unsigned char *position;
    int remaining;
    int count;
    int retry;

    remaining = info->pendingBytes;
    if (remaining != 0) {
        position = info->readBuffer + MPEG_READ_BUFFER_SIZE - remaining;
    } else {
        for (retry = 0; retry <= MOVIE_POLL_LIMIT; retry++) {
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
    count = sceMpegDemuxPss(&info->mpeg, position, remaining);
    info->pendingBytes -= count;
    return count;
}

/* Copies one video packet into the DMA ring, wrapping at its end. Returns 0 when there is no room. */
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
    if ((unsigned int) free < data->length) {
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

/*
 * Copies one audio packet into the sound ring. The first packet also carries the
 * sample rate header. Returns 0 when there is no room.
 */
static int audioCallback(int event, MpegStreamData *data, XglMovieInfo *info)
{
    unsigned char *source;
    int length;
    int free;
    int tail;

    source = data->buffer;
    length = data->length;
    if (info->audioHeaderRead == 0) {
        info->audioSampleRate = (source[MPEG_AUDIO_HEADER_RATE_OFFSET + 3] << 24) + (source[MPEG_AUDIO_HEADER_RATE_OFFSET + 2] << 16) + (source[MPEG_AUDIO_HEADER_RATE_OFFSET + 1] << 8) + source[MPEG_AUDIO_HEADER_RATE_OFFSET];
        info->audioHeaderRead = 1;
        length -= MPEG_AUDIO_HEADER_BYTES;
        source += MPEG_AUDIO_HEADER_BYTES;
    } else {
        source += MPEG_AUDIO_PACKET_HEADER_BYTES;
        length -= MPEG_AUDIO_PACKET_HEADER_BYTES;
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

/*
 * Supplies the decoder when the stream has no data: a filler pattern after too
 * many misses, otherwise the next DMA chunk from the ring.
 */
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

    /* Too many misses in a row: send a fixed filler pattern instead of stream data. */
    if (++info->missCount >= MPEG_MISS_LIMIT) {
        info->decodeState = MPEG_NODATA_DECODE_STATE;
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
        *D4_CHCR = MOVIE_D4_CHCR_NORMAL_START;
        return 1;
    }

    info->dmaRead = info->dmaNextRead;
    available = info->dmaWrite - info->dmaRead;
    if (available < 0) {
        available += info->dmaSize;
    }
    for (retry = 0; retry <= MOVIE_POLL_LIMIT; retry++) {
        if (available >= MPEG_DMA_CHUNK_BYTES) {
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
    if (length > MPEG_DMA_CHUNK_BYTES) {
        length = MPEG_DMA_CHUNK_BYTES;
    }
    if (info->dmaSize < info->dmaRead + length) {
        length = info->dmaSize - info->dmaRead;
    }
    *D4_QWC = length / MOVIE_DMA_QWORD;
    *D4_MADR = (unsigned int) info->dmaBuffer + info->dmaRead;
    *D4_CHCR = MOVIE_D4_CHCR_NORMAL_START;
    info->dmaNextRead = (info->dmaRead + length) % info->dmaSize;
    return 1;
}

/*
 * Error handler of the MPEG decoder: prints the error message, then increments
 * errorCount (+0x98) and clears errorFlag (+0x99) in the callback state.
 */
static int errorCallback(int event, MpegErrorInfo *error, MpegCallbackState *state)
{
    printf(errorMessageFormat, error->message);
    state->errorCount++;
    state->errorFlag = 0;
    return 1;
}

/* Opens the MPEG2 movie: starts the CD stream, creates the decoder with its callbacks and primes the buffers. Returns -1 if the stream cannot be opened. */
int xglMpeg2Open(XglMovieInfo *info)
{
    int result;
    int retry;

    if (xglCdStreamOpen(info) < 0) {
        return -1;
    }
    xglCdStreamReadRing((struct CdStreamParam *) info, info->ringCapacity / 2);
    info->ringRead = 0;
    sceMpegInit();
    sceMpegCreate(&info->mpeg, info->arena, info->reservedSize);
    sceMpegAddStrCallback(&info->mpeg, SCE_MPEG_STR_M2V, 0, videoCallback, info);
    sceMpegAddStrCallback(&info->mpeg, SCE_MPEG_STR_ADPCM, 0, audioCallback, info);
    sceMpegAddCallback(&info->mpeg, SCE_MPEG_CB_NODATA, nodataCallback, info);
    sceMpegAddCallback(&info->mpeg, SCE_MPEG_CB_ERROR, errorCallback, info);
    info->pendingBytes = 0;
    info->audioHeaderRead = 0;
    info->audioSampleRate = 0;
    info->decodeState = 0;
    info->streamOpen = 0;
    info->missCount = 0;
    info->videoCount = 0;
    for (retry = 0; retry <= MOVIE_POLL_LIMIT; retry++) {
        if (fillBuff(info, 1) == 0) {
            break;
        }
    }
    SsdInitVagStreamStereo(MPEG_AUDIO_CHUNK_BYTES, 4);
    do {
    } while (SsdGetResultValue(&result) < 0);
    return 0;
}

/* Builds the GIF packet list that loads the decoded picture's macroblocks into the GS buffer. */
static void setLoadImageTags(unsigned int destination, unsigned int image, int width, int height)
{
    unsigned long *packet = UNCACHED(destination);
    int x;
    int y;

    packet[0] = DMA_TAG_ID_CNT | 3;
    packet[2] = MOVIE_GIF_TAG_AD_2;
    packet[3] = GIF_REG_AD;
    packet[4] = MOVIE_BITBLTBUF_DBW_896;
    packet[5] = SCE_GS_BITBLTBUF;
    packet[6] = MOVIE_TRXREG_MACROBLOCK;
    packet[7] = SCE_GS_TRXREG;
    packet[1] = 0;
    packet += 8;
    for (x = 0; x < width; x += MOVIE_MACROBLOCK) {
        for (y = 0; y < height; y += MOVIE_MACROBLOCK) {
            packet[1] = 0;
            packet[0] = DMA_TAG_ID_CNT | 4;
            packet[2] = MOVIE_GIF_TAG_AD_2;
            packet[3] = GIF_REG_AD;
            packet[4] = ((long) x << 32) | ((long) y << 48);
            packet[5] = SCE_GS_TRXPOS;
            packet[6] = 0;
            packet[7] = SCE_GS_TRXDIR;
            packet[8] = MOVIE_GIF_TAG_IMAGE_BLOCK;
            packet[9] = 0;
            packet += 10;
            ((XglDmaRefTag *) packet)->address = image;
            image += MOVIE_MACROBLOCK_BYTES;
            ((XglDmaRefTag *) packet)->tag = DMA_TAG_ID_REF | MOVIE_MACROBLOCK_QWORDS;
            ((XglDmaRefTag *) packet)->upperQword = 0;
            packet += 2;
        }
    }
    ((XglDmaRefTag *) packet)[-1].tag &= DMA_TAG_ID_CLEAR_MASK;
}

/*
 * Decodes the next MPEG2 picture into the video buffer, feeds the sound stream
 * and uploads the frame. Returns 1 at the end of the stream and -1 on a decode error.
 */
int xglMpeg2Play(XglMovieInfo *info)
{
    static unsigned long TestEnv[24] = { 0x0000000000000000ULL, 0x5100000b00000000ULL, 0xa08b400000008001ULL, 0x00000053531eeeeeULL, 0x0000000000000000ULL, 0x000000000000003fULL, 0x0000000000000060ULL, 0x0000000000000014ULL, 0x0000000000000000ULL, 0x0000000000000006ULL, 0x0000000000000000ULL, 0x0000000000000008ULL, 0x000000000007000dULL, 0x0000000000000047ULL, 0x0000008000000080ULL, 0x0000008000000080ULL, 0x0000000000000000ULL, 0x0000000000000000ULL, 0x000071f700006ff8ULL, 0x0000000000f00000ULL, 0x00001c0000002000ULL, 0x0000000000000000ULL, 0x00008df700008ff8ULL, 0x0000000000f00000ULL };

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
        printf(mpegGetPictureFailedMessage);
        return -1;
    }
    info->missCount = 0;
    if ((info->mpeg.pictureType & MPEG_PICTURE_TYPE_MASK) == MPEG_PICTURE_TYPE_I) {
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
        SsdPlayVagStream(MPEG_AUDIO_VOLUME_MAX, 0, (info->audioSampleRate << 12) / MPEG_AUDIO_BASE_RATE);
        do {
        } while (SsdGetResultValue(&result) < 0);
    }
    free = info->soundWrite - info->soundRead;
    if (free <= 0) {
        free += info->soundBufferSize;
    }
    if (free > MPEG_AUDIO_CHUNK_BYTES) {
        SsdGetVagStreamStatusStereo();
        do {
        } while (SsdGetResultValue(&status) < 0);
        if (status >> 8 != 0) {
            SsdSetVagStreamDataStereo(0, (int) (info->soundBuffer + info->soundRead), MPEG_AUDIO_CHUNK_BYTES);
            info->soundRead = (info->soundRead + MPEG_AUDIO_CHUNK_BYTES) % info->soundBufferSize;
        }
    }
    for (i = 15; i >= 0; i--) {
        fillBuff(info, 0);
    }
    sceGsSyncPath(0, 0);
    packet = UNCACHED(info->mpegBuffer);
    packet[4] = ((unsigned long) sRender.bufferBase0 << 37) | MOVIE_BITBLTBUF_DBW_512;
    xglDmaDirectSrcChain(XGL_DMA_CHANNEL_GIF, (unsigned int) info->mpegBuffer);
    sceGsSyncPath(0, 0);
    env = TestEnv;
    env[8] = (0x24020000 | sRender.bufferBase0 << 5) | 0x2000000640000000;
    nmlModelDirectSend(5, env, 12);
    return 0;
}

/* Tears down the MPEG decoder, the sound stream and the CD stream. */
int xglMpeg2Close(XglMovieInfo *info)
{
    XglMpegState *mpegHandle;
    int value;

    mpegHandle = &info->mpeg;
    sceMpegReset(mpegHandle);
    sceMpegDelete(mpegHandle);
    SsdStopVagStream(0);
    do {
    } while (SsdGetResultValue(&value) < 0);
    SsdDisposeVagStream();
    do {
    } while (SsdGetResultValue(&value) < 0);
    xglCdStreamClose((CdStreamParam *) info);
    return 0;
}

/*
 * Lays out the movie buffers in arena, each 64-byte aligned, with the CD
 * stream ring of ring_size bytes last. Returns the aligned end of that ring.
 */
void *xglMpeg2InfoInit2(void *info, void *arena, int ring_size)
{
    XglMovieInfo *movie;
    unsigned char *next;

    movie = info;
    movie->arena = arena;
    next = (unsigned char *) (((unsigned int) arena + MPEG_WORK_SIZE + 0x3F) & ~0x3F);
    movie->videoBuffer = next;
    next = (unsigned char *) (((unsigned int) next + MPEG_VIDEO_BUFFER_SIZE + 0x3F) & ~0x3F);
    movie->mpegBuffer = next;
    next = (unsigned char *) (((unsigned int) next + MPEG_PACKET_BUFFER_SIZE + 0x3F) & ~0x3F);
    movie->readBuffer = next;
    next = (unsigned char *) (((unsigned int) next + MPEG_READ_BUFFER_SIZE + 0x3F) & ~0x3F);
    movie->dmaBuffer = next;
    next = (unsigned char *) (((unsigned int) next + MPEG_DMA_BUFFER_SIZE + 0x3F) & ~0x3F);
    movie->reservedSize = MPEG_WORK_SIZE;
    movie->dmaSize = MPEG_DMA_BUFFER_SIZE;
    movie->dmaWrite = MPEG_DMA_START_OFFSET;
    movie->dmaNextRead = MPEG_DMA_START_OFFSET;
    movie->soundBuffer = next;
    next = (unsigned char *) (((unsigned int) next + MPEG_SOUND_BUFFER_SIZE + 0x3F) & ~0x3F);
    movie->soundBufferSize = MPEG_SOUND_BUFFER_SIZE;
    movie->dmaRead = 0;
    movie->soundWrite = 0;
    movie->soundRead = 0;
    xglCdStreamParamInit((struct CdStreamParam *) movie);
    movie->ringBuffer = next;
    movie->ringCapacity = ring_size;
    return (void *) (((unsigned int) next + ring_size + 0x3F) & ~0x3F);
}

void xglMpeg2InfoInit(void *info, void *arena)
{
    xglMpeg2InfoInit2(info, arena, MPEG_RING_BUFFER_SIZE);
}

int xglMovieInit(void)
{
    return sceIpuInit();
}

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
const char frameDropMessage[16] = "frame drop\n";

const char mpegGetPictureFailedMessage[] = {'s', 'c', 'e', 'M', 'p', 'e', 'g', 'G', 'e', 't', 'P', 'i', 'c', 't', 'u', 'r', 'e', ' ', 'f', 'a', 'i', 'l', 'e', 'd', 10, 0};
