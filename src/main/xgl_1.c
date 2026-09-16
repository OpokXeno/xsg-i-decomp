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

INCLUDE_ASM("asm/main/nonmatchings/xgl_1", errorCallback);

INCLUDE_ASM("asm/main/nonmatchings/xgl_1", xglMpeg2Open);

INCLUDE_ASM("asm/main/nonmatchings/xgl_1", setLoadImageTags);

INCLUDE_ASM("asm/main/nonmatchings/xgl_1", xglMpeg2Play);

INCLUDE_ASM("asm/main/nonmatchings/xgl_1", xglMpeg2Close);

INCLUDE_ASM("asm/main/nonmatchings/xgl_1", xglMpeg2InfoInit2);

void xglMpeg2InfoInit(void *info, void *arena)
{
    xglMpeg2InfoInit2(info, arena, 0x100000);
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_1", xglMovieInit);
