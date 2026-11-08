#include "common.h"

/*
 * Only the byte member beginning at SaveData + 0x74 is observed here.  This
 * external byte view intentionally does not describe the unknown SaveData
 * prefix or a complete SaveData layout.
 */
#include "shared.h"

#include "main/xgl_flags.h"
#include "xgl_flags.h"

/* The packed script flags are the byte range of SaveData starting at +0x74. */
#define XGL_FLAGS_OFFSET 0x74
#define XGL_FLAGS_AREA_SIZE 0x10000

/* Stores the low `bit_count` bits of `value` at bit `bit_offset` of the flag area; returns the previous field value. */
int xglFlagsSet(int bit_offset, int bit_count, int value)
{
    union XglPackedFlagWindow flag_window;
    int bit_shift;
    int byte_count;
    int byte_offset;
    signed long long mask;
    int byte_index;
    signed long long old_word;
    signed long long shifted_mask;
    signed long long replacement_word;
    int old_value;

    bit_shift = bit_offset & 7;
    /* The callers use up to 32 field bits; the EE word shift masks its count
       to five bits, so a count of 32 produces a zero mask in the binary. */
    mask = (int)((1u << bit_count) - 1u);
    byte_count = (bit_count + bit_shift + 7) >> 3;
    byte_offset = bit_offset >> 3;

    for (byte_index = 0; byte_index < byte_count; byte_index++) {
        flag_window.bytes[byte_index] =
            SaveData[XGL_FLAGS_OFFSET + byte_offset + byte_index];
    }

    /* Like xglFlagsGet, the word load observes the byte window after only
       its requested prefix was copied from SaveData. */
    old_word = flag_window.signed_value;
    old_value = (int)((old_word >> bit_shift) & mask);
    shifted_mask = (signed long long)((unsigned long long)mask << bit_shift);
    replacement_word =
        (old_word & ~shifted_mask) |
        ((signed long long)((unsigned long long)(value & mask) << bit_shift));
    flag_window.signed_value = replacement_word;

    for (byte_index = 0; byte_index < byte_count; byte_index++) {
        SaveData[XGL_FLAGS_OFFSET + byte_offset + byte_index] =
            flag_window.bytes[byte_index];
    }

    return old_value;
}

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

/* Stores the 64-bit `value` as two 32-bit fields from `bit_offset`; returns the previous value. */
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

/* Returns the `bit_count`-bit field at bit `bit_offset` of the flag area. */
int xglFlagsGet(int bit_offset, int bit_count)
{
    union XglPackedFlagWindow value;
    int byte_index;

    for (byte_index = 0;
         byte_index < ((bit_count + (bit_offset & 7) + 7) >> 3);
         byte_index++) {
        value.bytes[byte_index] =
            SaveData[XGL_FLAGS_OFFSET + (bit_offset >> 3) + byte_index];
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
    return (value.signed_value >> (bit_offset & 7)) & ((1u << bit_count) - 1u);
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

/* Returns the 64-bit field at `bit_offset`, composed of two 32-bit reads. */
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

    /* Clears the 0x10000 bytes of the flag area that start at SaveData + 0x74. */
    for (byte_index = XGL_FLAGS_AREA_SIZE; byte_index != 0; byte_index--) {
        SaveData[XGL_FLAGS_OFFSET + XGL_FLAGS_AREA_SIZE - byte_index] = 0;
    }
}
