#include "common.h"

#include "shared.h"

/* The codec work area is quadword aligned for FFIDCT's VU0 transfers.
 * Quantization tables occupy +0xD0 and +0x1D0; bit input, IDCT storage and
 * restart counters are modeled below. Other regions remain explicit partial
 * spans. */

typedef struct JpegWork {
    u8 unmodeled_0000[0xD0];
    float luminanceQuantization[64];    /* +0xD0 */
    float chrominanceQuantization[64];  /* +0x1D0 */
    u8 unmodeled_02d0[0x350 - 0x2D0];
    u8 *buffer;
    u8 unmodeled_0354[0x358 - 0x354];
    signed char bitsRemaining;
    u8 bufferedByte;
    u8 unmodeled_035a[0x1470 - 0x35a];
    float idct_block[8][8];
    float idct_stage[8];
    u8 unmodeled_1590[0x2ba0 - 0x1590];
    u32 restart_interval;
    u32 restart_interval_plus_one;
} JpegWork;

static JpegWork *sw = (JpegWork *)0x70000000;

extern int F2I(float value);

typedef struct JpegHuffCode {
    u16 code;
    s16 length;
} JpegHuffCode;

static void PutBits(u16 code, int length);

/* A DRI marker carries a big-endian restart interval in bytes 2 and 3.
 * The parser stores that value and its successor at +0x2BA0/+0x2BA4,
 * then returns the next segment byte. */

extern float I2F(int value);

/* Each quantization table has 64 floats. DefineQuantizeTable selects
 * luminance for table ID 0 and chrominance otherwise; HuffmanDecode uses
 * the same bases (main:0x224c24/0x224c30) and indexes 64 coefficients. */

static int GetBit(void);

INCLUDE_ASM("asm/main/nonmatchings/xgl_jpeg", ConvertYUV2MCU);

INCLUDE_ASM("asm/main/nonmatchings/xgl_jpeg", FFDCT);

static void Quantize(int *quantized, const float *coefficients, const float *reciprocals)
{
    float coefficient;
    float reciprocal;
    float scaled;
    int index;

    for (index = 63; index >= 0; index--) {
        coefficient = *coefficients;
        coefficients++;
        reciprocal = *reciprocals;
        reciprocals++;
        scaled = coefficient * reciprocal;
        if (coefficient < 0.0f) {
            scaled -= 0.5f;
        } else {
            scaled += 0.5f;
        }
        *quantized = F2I(scaled);
        quantized++;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_jpeg", RebuildQuantizeTable);

INCLUDE_ASM("asm/main/nonmatchings/xgl_jpeg", PutBits);

INCLUDE_ASM("asm/main/nonmatchings/xgl_jpeg", EncodeDc);

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

static void *DefineRestartInterval(void *marker)
{
    u8 *segment = marker;
    JpegWork *work = sw;
    u32 interval;

    interval = (segment[2] << 8) + segment[3];
    work->restart_interval = interval;
    work->restart_interval_plus_one = interval + 1;
    return segment + 4;
}

static u8 *DefineQuantizeTable(u8 *marker)
{
    u8 *segment;
    u16 count;
    float *dest;
    u8 value;

    segment = marker;
    count = (segment[0] << 8) + segment[1] - 3;
    if (segment[2] == 0) {
        dest = sw->luminanceQuantization;
    } else {
        dest = sw->chrominanceQuantization;
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

static int GetBit(void)
{
    u8 *buffer;
    JpegWork *work;
    u8 value;

    if (sw->bitsRemaining == 0) {
        work = sw;
        buffer = work->buffer;
        value = *buffer;
        if (value == 0xFF) {
            work->buffer = buffer + 1;
        }
        work->bufferedByte = value;
        work->buffer++;
        sw->bitsRemaining = 7;
    } else {
        sw->bitsRemaining--;
    }
    return (sw->bufferedByte >> sw->bitsRemaining) & 1;
}

static s16 decode_sub(s16 *codes)
{
    u32 code = 0;
    s16 terminator = 0xFE;

    for (;;) {
        code = code * 2 + GetBit();
        if (codes[1] != terminator) {
            do {
                if ((u16) codes[0] == code) {
                    return codes[1];
                }
                codes += 2;
            } while (codes[1] != terminator);
        }
        codes += 2;
    }
}

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
