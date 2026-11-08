#include "common.h"
#include "shared.h"
#include "xgl_timer0.h"
#include "main/control_entry.h"

/*
 * EE Timer 0.  T0_COUNT is the free-running counter and T0_MODE its control
 * word; bit 7 (0x80) of T0_MODE is CUE, which starts the count.
 */
#define T0_COUNT ((volatile u32 *)0x10000000)

#define T0_MODE  ((volatile u32 *)0x10000010)
#define T0_MODE_CUE 128
#define T0_COUNT_MASK 0xffff

/*
 * ARX stream layout, in words: magic "ARX\0", decoded size in bytes, packed
 * payload size in bytes, flags (must be zero), 30 table entries, then the
 * bit stream.  The table is copied to the 64-byte aligned address after
 * WorkEnd; the 32 words that follow it receive the unread tail of the stream
 * when the output cursor catches up with the read pointer.
 */
#define ARX_MAGIC 0x585241

#define ARX_MAGIC_WORD 0
#define ARX_SIZE_WORD 1
#define ARX_PAYLOAD_SIZE_WORD 2
#define ARX_FLAGS_WORD 3
#define ARX_TABLE_WORD 4
#define ARX_TABLE_COUNT 30
#define ARX_TABLE_SLOTS 32
#define ARX_DATA_WORD 34
#define ARX_TABLE_ALIGN 64

/*
 * One output word: a clear flag bit takes the next stream word as is; a set
 * flag bit is followed by a unary length prefix that selects a table entry
 * together with `length` further bits.
 */
#define ARX_DECODE_WORD(value) \
    do { \
        if (getbit1(&reader) == 0) { \
            value = *reader.data++; \
        } else { \
            length = 1; \
            index = 0; \
            unit = 1; \
            while (getbit1(&reader) != 0) { \
                index += unit << length; \
                length++; \
            } \
            value = table[index + getbits(&reader, length)]; \
        } \
    } while (0)

typedef struct ArxBitReader {
    u32 value;
    u32 bit_count;
    u32 *data;
} ArxBitReader;

/* Clears the counter and starts it with `mode` plus the CUE bit. */
void xglTimer0Reset(int mode)
{
    *T0_COUNT = 0;
    *T0_MODE = mode + T0_MODE_CUE;
}

/* Returns the 16-bit timer count. */
int xglTimer0Get(void)
{
    return *T0_COUNT & T0_COUNT_MASK;
}

/* Returns the next stream bit, most significant bit first, loading a new word when the current one is used up. */
static u32 getbit1(ArxBitReader *reader)
{
    u32 bit;

    if (reader->bit_count == 0) {
        reader->value = *reader->data++;
    }
    reader->bit_count = (reader->bit_count - 1) & 0x1f;
    bit = reader->value >> 31;
    reader->value <<= 1;
    return bit;
}

/* Returns the next `count` stream bits. */
static u32 getbits(ArxBitReader *reader, u32 count)
{
    u32 high = 0;
    u32 bits = reader->bit_count;
    u32 result;

    if (bits < count) {
        high = reader->value >> (32 - bits);
        reader->value = *reader->data++;
        count -= bits;
        reader->bit_count = 32;
        bits = 32;
    }
    result = high << count | reader->value >> -count;
    reader->bit_count = bits - count;
    reader->value <<= count;
    return result;
}

/*
 * Decompresses the ARX stream at `src` into `dest`, writing the decoded size in bytes.
 * Returns that size, or -1 when the header is not a valid ARX stream.
 */
int xglArxExtract(u32 *dest, u32 *src)
{
    ArxBitReader reader;
    u32 *table;
    u32 *cursor = dest;
    int size;
    int payloadSize;
    int i;
    u32 length;
    u32 index;
    u32 value;
    u32 unit;

    if (src[ARX_MAGIC_WORD] != ARX_MAGIC || src[ARX_FLAGS_WORD] != 0) {
        return -1;
    }
    size = src[ARX_SIZE_WORD];
    payloadSize = src[ARX_PAYLOAD_SIZE_WORD];
    table = (u32 *)(((u32)WorkEnd + ARX_TABLE_ALIGN - 1) & ~(ARX_TABLE_ALIGN - 1));
    for (i = 0; i < ARX_TABLE_COUNT; i++) {
        table[i] = src[ARX_TABLE_WORD + i];
    }
    reader.value = 0;
    reader.bit_count = 0;
    reader.data = src + ARX_DATA_WORD;
    /* Overlapping source and destination take the tail-relocation path below. */
    if (!((reader.data < dest || dest + size / 4 < reader.data) &&
          (reader.data + payloadSize / 4 < dest ||
           dest + size / 4 < reader.data + payloadSize / 4))) {
        if (cursor < (u32 *)((u8 *)dest + size)) {
            u32 *payloadEnd = src + payloadSize / 4;

            while (1) {
                ARX_DECODE_WORD(value);
                *cursor++ = value;
                /* The output reached the read pointer: move the unread stream tail into the table area. */
                if (cursor >= reader.data) {
                    u32 *from = reader.data;
                    u32 *to = table + ARX_TABLE_SLOTS;
                    u32 *limit = payloadEnd + ARX_DATA_WORD;

                    reader.data = to;
                    while (from <= limit) {
                        *to++ = *from++;
                    }
                    break;
                }
                if (cursor >= (u32 *)((u8 *)dest + size)) {
                    break;
                }
            }
        }
    }
    while (cursor < (u32 *)((u8 *)dest + size)) {
        ARX_DECODE_WORD(value);
        *cursor++ = value;
    }
    return size;
}
