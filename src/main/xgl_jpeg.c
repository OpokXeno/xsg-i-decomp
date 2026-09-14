#include "common.h"
#include "shared.h"

/*
 * The JPEG work area the codec keeps behind the gp-relative `sw` pointer.
 * Only the two regions FFIDCT uses are recovered here: the 8x8 block that holds
 * the result of the first one-dimensional pass, and the quadword pair each pass
 * stages one line through on its way to and from VU0.  The area is quadword
 * aligned, which is what the lqc2/sqc2 pair in FFIDCT needs.  Everything before
 * those two regions belongs to the parts of the codec that are still assembler
 * and stays an opaque reserved span.
 */
typedef struct JpegWork {
    u8 reserved0[5232];
    float idct_block[8][8];
    float idct_stage[8];
} JpegWork;

extern JpegWork *sw;

INCLUDE_ASM("asm/main/nonmatchings/xgl_jpeg", ConvertYUV2MCU);

INCLUDE_ASM("asm/main/nonmatchings/xgl_jpeg", FFDCT);

INCLUDE_ASM("asm/main/nonmatchings/xgl_jpeg", Quantize);

INCLUDE_ASM("asm/main/nonmatchings/xgl_jpeg", RebuildQuantizeTable);

INCLUDE_ASM("asm/main/nonmatchings/xgl_jpeg", PutBits);

INCLUDE_ASM("asm/main/nonmatchings/xgl_jpeg", EncodeDc);

INCLUDE_ASM("asm/main/nonmatchings/xgl_jpeg", EncodeAc);

INCLUDE_ASM("asm/main/nonmatchings/xgl_jpeg", HuffmanEncode);

INCLUDE_ASM("asm/main/nonmatchings/xgl_jpeg", ExtractHuffmanTableSub);

INCLUDE_ASM("asm/main/nonmatchings/xgl_jpeg", xglJpegEncode);

INCLUDE_ASM("asm/main/nonmatchings/xgl_jpeg", DefineRestartInterval);

INCLUDE_ASM("asm/main/nonmatchings/xgl_jpeg", DefineQuantizeTable);

INCLUDE_ASM("asm/main/nonmatchings/xgl_jpeg", StartOfFrame);

INCLUDE_ASM("asm/main/nonmatchings/xgl_jpeg", DefineHuffmanTable);

INCLUDE_ASM("asm/main/nonmatchings/xgl_jpeg", GetBit);

INCLUDE_ASM("asm/main/nonmatchings/xgl_jpeg", decode_sub);

INCLUDE_ASM("asm/main/nonmatchings/xgl_jpeg", HuffmanDecode);

/*
 * Two-dimensional inverse DCT of one 8x8 block of floating-point coefficients,
 * in place.
 *
 * StartOfScan leaves the one-dimensional transform in the VU0 register file and
 * it stays there for every block of the scan: vf1-vf8 for the first pass and
 * vf11-vf18, the same rows scaled through I, for the second.  A pass therefore
 * reduces to one matrix-by-vector product per line, which is the only part of
 * this routine that cannot be written in C.  The even-indexed samples of the
 * line go to vf9 and the odd-indexed ones to vf10, and the accumulator chain
 * multiplies them by the transform; what VU0 returns is the butterfly's four
 * sums and four differences, which the scalar code below combines into the
 * eight output samples.
 *
 * The first pass reads the columns of the caller's block and writes the rows of
 * the decoder's intermediate block; the second pass reads the columns of that
 * intermediate block and writes the rows of the caller's block back.
 */
static void FFIDCT(float block[8][8])
{
    JpegWork *work;
    float *column;
    float *stage;
    float *line;
    float *out;
    int i;

    stage = sw->idct_stage;

    for (i = 0; i < 8; i++) {
        stage[0] = block[0][i];
        stage[1] = block[2][i];
        stage[2] = block[4][i];
        stage[3] = block[6][i];
        stage[4] = block[1][i];
        stage[5] = block[3][i];
        stage[6] = block[5][i];
        stage[7] = block[7][i];
        __asm__ __volatile__(
            "lqc2 vf9,0(%0)\n"
            "lqc2 vf10,16(%0)\n"
            "vmulax.xyzw ACCxyzw,vf1xyzw,vf9x\n"
            "vmadday.xyzw ACCxyzw,vf2xyzw,vf9y\n"
            "vmaddaz.xyzw ACCxyzw,vf3xyzw,vf9z\n"
            "vmaddw.xyzw vf19xyzw,vf4xyzw,vf9w\n"
            "vmulax.xyzw ACCxyzw,vf5xyzw,vf10x\n"
            "vmadday.xyzw ACCxyzw,vf6xyzw,vf10y\n"
            "vmaddaz.xyzw ACCxyzw,vf7xyzw,vf10z\n"
            "vmaddw.xyzw vf20xyzw,vf8xyzw,vf10w\n"
            "sqc2 vf19,0(%0)\n"
            "sqc2 vf20,16(%0)\n"
            :
            : "r"(stage)
            : "memory");
        work = sw;
        line = work->idct_block[i];
        line[0] = stage[0] + stage[2] + stage[4];
        line[1] = stage[1] + stage[3] + stage[5];
        line[2] = stage[1] - stage[3] + stage[6];
        line[3] = stage[0] - stage[2] + stage[7];
        line[4] = stage[0] - stage[2] - stage[7];
        line[5] = stage[1] - stage[3] - stage[6];
        line[6] = stage[1] + stage[3] - stage[5];
        line[7] = stage[0] + stage[2] - stage[4];
    }

    /*
     * The work-area pointer the second pass reads is the one the first pass
     * left behind, so the first column starts without reloading it; every
     * later column re-reads sw, because the VU0 block above clobbers memory.
     * The original's loop is entered past that reload for exactly that reason
     * (see the style note in the attempt report: the structured spelling of
     * this loop compiles to a strength-reduced destination walk instead of the
     * original's recomputed sw + 5232 + i * 4 and block + i * 32).
     */
    i = 0;
    goto transform_column;
reload_work:
    work = sw;
    for (;;) {
transform_column:
        column = &work->idct_block[0][i];
        stage[0] = column[0 * 8];
        stage[1] = column[2 * 8];
        stage[2] = column[4 * 8];
        stage[3] = column[6 * 8];
        stage[4] = column[1 * 8];
        stage[5] = column[3 * 8];
        stage[6] = column[5 * 8];
        stage[7] = column[7 * 8];
        __asm__ __volatile__(
            "lqc2 vf9,0(%0)\n"
            "lqc2 vf10,16(%0)\n"
            "vmulax.xyzw ACCxyzw,vf11xyzw,vf9x\n"
            "vmadday.xyzw ACCxyzw,vf12xyzw,vf9y\n"
            "vmaddaz.xyzw ACCxyzw,vf13xyzw,vf9z\n"
            "vmaddw.xyzw vf19xyzw,vf14xyzw,vf9w\n"
            "vmulax.xyzw ACCxyzw,vf15xyzw,vf10x\n"
            "vmadday.xyzw ACCxyzw,vf16xyzw,vf10y\n"
            "vmaddaz.xyzw ACCxyzw,vf17xyzw,vf10z\n"
            "vmaddw.xyzw vf20xyzw,vf18xyzw,vf10w\n"
            "sqc2 vf19,0(%0)\n"
            "sqc2 vf20,16(%0)\n"
            :
            : "r"(stage)
            : "memory");
        out = block[i];
        out[0] = stage[0] + stage[2] + stage[4];
        out[1] = stage[1] + stage[3] + stage[5];
        out[2] = stage[1] - stage[3] + stage[6];
        out[3] = stage[0] - stage[2] + stage[7];
        out[4] = stage[0] - stage[2] - stage[7];
        out[5] = stage[1] - stage[3] - stage[6];
        out[6] = stage[1] + stage[3] - stage[5];
        out[7] = stage[0] + stage[2] - stage[4];
        if (++i >= 8) {
            break;
        }
        goto reload_work;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_jpeg", ConvertMCU2YUV);

INCLUDE_ASM("asm/main/nonmatchings/xgl_jpeg", ConvertYUV2RGB);

INCLUDE_ASM("asm/main/nonmatchings/xgl_jpeg", StartOfScan);

INCLUDE_ASM("asm/main/nonmatchings/xgl_jpeg", xglJpegDecode);
