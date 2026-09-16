#include "common.h"
/*
 * Only the byte member beginning at SaveData + 0x74 is observed here.  This
 * external byte view intentionally does not describe the unknown SaveData
 * prefix or a complete SaveData layout.
 */

#include "shared.h"
#include "main/xgl_flags.h"
#include "xgl_flags.h"

int xglFlagsSet(int bit_offset, int bit_count, int value);

INCLUDE_ASM("asm/main/nonmatchings/xgl_flags", xglFlagsSet);

int xglFlagsSet1(int bit_offset, int value)
{
    return xglFlagsSet(bit_offset, 1, value);
}

int xglFlagsSet2(int bit_offset, int value)
{
    return xglFlagsSet(bit_offset, 2, value);
}

int xglFlagsSet4(int bit_offset, int value)
{
    return xglFlagsSet(bit_offset, 4, value);
}

int xglFlagsSet8(int bit_offset, int value)
{
    return xglFlagsSet(bit_offset, 8, value);
}

int xglFlagsSet16(int bit_offset, int value)
{
    return xglFlagsSet(bit_offset, 16, value);
}

int xglFlagsSet32(int bit_offset, int value)
{
    return xglFlagsSet(bit_offset, 32, value);
}

long long xglFlagsSet64(int bit_offset, long long value)
{
    unsigned long long low;
    unsigned long long high;

    /*
     * Unsigned composition mirrors xglFlagsGet64: keeps the high << 32 shift
     * defined (high may be negative as a plain 32-bit call result) while
     * reproducing the same bit pattern the target's sign-extending register
     * convention produces.
     */
    low = xglFlagsSet(bit_offset, 32, (int)value);
    high = xglFlagsSet(bit_offset + 32, 32, value >> 32);
    return (long long)((high << 32) + low);
}

int xglFlagsGet(int bit_offset, int bit_count)
{
    union XglPackedFlagWindow value;
    int byte_index;

    for (byte_index = 0;
         byte_index < ((bit_count + (bit_offset & 7) + 7) >> 3);
         byte_index++) {
        value.bytes[byte_index] =
            SaveData[0x74 + (bit_offset >> 3) + byte_index];
    }
    /*
     * Preserve the original EE SLLV/ADDIU sequence.  SLLV uses only the low
     * five count bits, so the target count 32 makes this mask zero after the
     * subtract-one instruction while retaining the preceding memory reads.
     * The unguarded C shift is intentionally target-specific and is not an
     * ISO C defined-width-32 contract.  The Java caller forwards signed
     * offsets and widths unchecked; sufficient access bounds are not enforced
     * by this function.
     */
    return (int)((value.signed_value >> (bit_offset & 7)) & ((1u << bit_count) - 1u));
}

int xglFlagsGet1(int bit_offset)
{
    return xglFlagsGet(bit_offset, 1);
}

int xglFlagsGet2(int bit_offset)
{
    return xglFlagsGet(bit_offset, 2);
}

int xglFlagsGet4(int bit_offset)
{
    return xglFlagsGet(bit_offset, 4);
}

int xglFlagsGet8(int bit_offset)
{
    return xglFlagsGet(bit_offset, 8);
}

int xglFlagsGet16(int bit_offset)
{
    return xglFlagsGet(bit_offset, 16);
}

int xglFlagsGet32(int bit_offset)
{
    return xglFlagsGet(bit_offset, 32);
}

unsigned long long xglFlagsGet64(int bit_offset)
{
    unsigned long long low;
    unsigned long long high;

    /*
     * Preserve the two observed width-32 calls and their signed-int to
     * unsigned-long-long composition.  On normal target completion each
     * width-32 mask is zero but the calls still read their byte windows; this
     * is not a general 64-bit packed-field read contract.
     */
    low = xglFlagsGet(bit_offset, 32);
    high = xglFlagsGet(bit_offset + 32, 32);
    return (high << 32) + low;
}

void xglFlagsInitial(void)
{
    unsigned int byte_index;

    /* The observed byte window is cleared exactly from 0x74 for 0x10000 bytes. */
    for (byte_index = 0x10000; byte_index != 0; byte_index--) {
        SaveData[0x74 + 0x10000 - byte_index] = 0;
    }
}
