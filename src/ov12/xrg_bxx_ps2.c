/*
 * OV12 original TU 81: 0x00a48548..0x00a487c0 (2 functions)
 */
#include "common.h"
#include "shared.h"

extern void RgError(const char *message, const char *source_file, int line,
                    ...);

/* These data labels are still supplied by the TU's generated data object. */
extern const char D_00A58D10[];
extern const char D_00A58D20[];

static int _Log2(u32 bucket_count)
{
    if (bucket_count < 3U) {
        return 1;
    }
    if (bucket_count < 5U) {
        return 2;
    }
    if (bucket_count < 9U) {
        return 3;
    }
    if (bucket_count < 17U) {
        return 4;
    }
    if (bucket_count < 33U) {
        return 5;
    }
    if (bucket_count < 65U) {
        return 6;
    }
    if (bucket_count < 129U) {
        return 7;
    }
    if (bucket_count < 257U) {
        return 8;
    }
    if (bucket_count < 513U) {
        return 9;
    }
    if (bucket_count < 1025U) {
        return 10;
    }
    if (bucket_count < 2049U) {
        return 11;
    }
    if (bucket_count < 4097U) {
        return 12;
    }

    RgError(D_00A58D10, D_00A58D20, 50, bucket_count);
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_bxx_ps2", XrgBxxPs2Tex0);
