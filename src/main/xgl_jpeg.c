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

/*
 * A Huffman code table entry: the variable-length bit pattern PutBits emits
 * and its bit count. EncodeAc indexes such a table by (run << 4) + size, the
 * scaled run of preceding zero coefficients plus the bit length of the
 * nonzero coefficient that ends the run; a run alone (size 0) is the table's
 * zero-run control code (end-of-block or ZRL).
 */
typedef struct JpegHuffCode {
    u16 code;
    s16 length;
} JpegHuffCode;

static void PutBits(u16 code, int length);

static void EncodeAc(int value, int runIndex, JpegHuffCode *table)
{
    int magnitude;
    int negative;
    int remaining;
    int length;
    u16 bits;

    magnitude = value;
    if (magnitude == 0) {
        PutBits(table[runIndex].code, table[runIndex].length);
        return;
    }

    negative = 0;
    if (magnitude < 0) {
        negative = 1;
        magnitude = -magnitude;
    }

    remaining = magnitude;
    length = 0;
    while (remaining != 0) {
        remaining >>= 1;
        length++;
    }

    PutBits(table[length + runIndex].code, table[length + runIndex].length);

    if (negative) {
        bits = (u16) (~magnitude & 0xFFFF);
    } else {
        bits = (u16) (magnitude & 0xFFFF);
    }
    PutBits(bits, length);
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_jpeg", HuffmanEncode);

INCLUDE_ASM("asm/main/nonmatchings/xgl_jpeg", ExtractHuffmanTableSub);

INCLUDE_ASM("asm/main/nonmatchings/xgl_jpeg", xglJpegEncode);

/*
 * Parses a DRI (Define Restart Interval) marker segment: marker[0..1] is the
 * segment length, marker[2..3] the big-endian restart interval. Stores it and
 * that value plus one into the codec work area past the JpegWork extent
 * modeled above (lw/sw at main 0x002248ac/0x002248b4, +0x2ba0/+0x2ba4 of
 * `sw`); nothing in this allocation reads either field back, so their
 * consuming role is not evidenced here. Modeling them as named JpegWork
 * members needs a published extension of that struct (an
 * unmodeled_1590[0x1610] gap then two u32 members), which is a shared-header
 * change outside a single function's additive edit and is reported rather
 * than made here. Returns the byte after the segment, as the marker dispatch
 * table's other parsers do.
 */
static void *DefineRestartInterval(void *marker)
{
    u8 *segment = marker;
    u8 *work = (u8 *)sw;
    u32 interval;

    interval = (segment[2] << 8) + segment[3];
    *(u32 *)(work + 0x2ba0) = interval;
    *(u32 *)(work + 0x2ba4) = interval + 1;
    return segment + 4;
}

extern float I2F(int value);

/*
 * The luminance and chrominance quantization tables the segment's byte
 * coefficients are converted into lie inside the JpegWork work area at
 * fixed offsets from `sw`: table id 0 (luminance) at +0xD0, any other id
 * (chrominance) at +0x1D0. Both offsets fall inside the reserved0 span
 * documented above; naming them as JpegWork members would edit that
 * already-published struct, which is a shared-header change outside a
 * single function's additive edit and is reported rather than made here
 * (the same constraint DefineRestartInterval's own note records for its
 * fields).
 */
static u8 *DefineQuantizeTable(u8 *marker)
{
    u8 *segment;
    u16 count;
    float *dest;
    u8 value;

    segment = marker;
    count = (segment[0] << 8) + segment[1] - 3;
    if (segment[2] == 0) {
        dest = (float *)((u8 *)sw + 0xD0);
    } else {
        dest = (float *)((u8 *)sw + 0x1D0);
    }
    segment += 3;

    while (count != 0) {
        value = segment[0];
        segment += 1;
        *dest = I2F(value);
        dest += 1;
        count--;
    }
    return segment;
}

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
