#include "common.h"

/*
 * xglMpeg2InfoInit2's own body is still INCLUDE_ASM below, but its tail at
 * 0x00222818..0x00222828 computes align64(arena + allocated_size +
 * audio_buffer_size) into v0 immediately before return: that value is the
 * function's result, not a void return. xglMpeg2InfoInit calls it and
 * discards the result, but the forward declaration used here must still
 * spell the pointer-return signature the body actually implements.
 */
extern void *xglMpeg2InfoInit2(void *info, void *arena, int audio_buffer_size);

INCLUDE_ASM("asm/main/nonmatchings/xgl_1", myDmaDirectFromIPU);

INCLUDE_ASM("asm/main/nonmatchings/xgl_1", setD4_CHCR_00220CF8);

INCLUDE_ASM("asm/main/nonmatchings/xgl_1", make_gstrans);

INCLUDE_ASM("asm/main/nonmatchings/xgl_1", checkdma);

INCLUDE_ASM("asm/main/nonmatchings/xgl_1", xglMovieOpen);

INCLUDE_ASM("asm/main/nonmatchings/xgl_1", xglMoviePlay);

INCLUDE_ASM("asm/main/nonmatchings/xgl_1", xglMovieClose);

INCLUDE_ASM("asm/main/nonmatchings/xgl_1", xglMovieInfoInit);

INCLUDE_ASM("asm/main/nonmatchings/xgl_1", xglMovieMakeXtxHeader);

INCLUDE_ASM("asm/main/nonmatchings/xgl_1", fileRead);

INCLUDE_ASM("asm/main/nonmatchings/xgl_1", fillBuff);

INCLUDE_ASM("asm/main/nonmatchings/xgl_1", videoCallback);

INCLUDE_ASM("asm/main/nonmatchings/xgl_1", audioCallback);

INCLUDE_ASM("asm/main/nonmatchings/xgl_1", nodataCallback);

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
extern const char D_004DC328[];

static int errorCallback(int event, MpegErrorInfo *error, MpegCallbackState *state)
{
    printf(D_004DC328, error->message);
    state->errorCount++;
    state->errorFlag = 0;
    return 1;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_1", xglMpeg2Open);

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

INCLUDE_ASM("asm/main/nonmatchings/xgl_1", xglMpeg2InfoInit2);

void xglMpeg2InfoInit(void *info, void *arena)
{
    xglMpeg2InfoInit2(info, arena, 0x100000);
}

extern int sceIpuInit(void);

int xglMovieInit(void)
{
    return sceIpuInit();
}
