#include "common.h"
#include "main/xgl_cd.h"
#include "main/xgl_packet.h"
#include "main/xgl_font.h"

#define FONT_STATE_FLAGS (FS.state.flags)
#define FONT_PRINT_QUEUE (fontPrintQueueBuffer.slots)

/*
 * buffer_reset also reaches two fields 0x20 bytes before the queue buffer:
 * a reset flag byte and the queue's free-cursor pointer. The header these
 * belong to is not otherwise evidenced within this allocation, so they are
 * reached by byte offset from the queue symbol instead of a named member
 * (matching src/ov11/res.c's RES_IsDebugMode).
 */
#define FONT_QUEUE_HEADER_OFFSET 0x20

#define FONT_QUEUE_HEADER_RESET_FLAG_OFFSET 0x11
#define FONT_QUEUE_HEADER_CURSOR_OFFSET 0x1C

#define FONT_ARGUMENT_START(list, last) \
    ((list) = (unsigned long long *)((char *)__builtin_next_arg(last) - \
              (8 - __builtin_args_info(2)) * 8))

struct FontImage {
    unsigned char kanji_texture_0[0x3c020];
    unsigned char kanji_texture_1[0x3c020];
    unsigned short proportional_widths[256];
};

struct FontState {
    struct FontImage *font_image;
    void *window_image;
    unsigned short flags;
    unsigned char font_selection;
    unsigned char debug_mode;
    unsigned short line_height;
    unsigned short character_spacing;
    unsigned char default_width;
    unsigned char queue_reset_flag;
    unsigned char proportional_mode;
    unsigned char horizontal_shift;
    unsigned char unmodeled_14[8];
    unsigned char *queue_cursor;
};

/* The partial state view shares the complete 0x20-byte retail allocation. */
typedef union FontStateStorage {
    struct FontState state;
    unsigned char bytes[0x20];
} FontStateStorage;

/*
 * Each 4-byte slot of the font print-queue buffer at fontPrintQueueBuffer (16 slots,
 * scaffold-owned splat name): link is the byte offset, from this array's
 * base, of the next queued record for this bucket (0 marks the bucket
 * empty -- xglFontFlush follows it as a chain), and selfOffset is the
 * slot's own byte offset within the array.
 */
typedef struct FontQueueSlot {
    unsigned short link;
    unsigned short selfOffset;
} FontQueueSlot;

typedef union FontPrintQueueStorage {
    unsigned long long alignment;
    FontQueueSlot slots[16];
    unsigned char bytes[0x2860];
} FontPrintQueueStorage;

typedef union FontImageStorage {
    struct FontImage image;
    unsigned char bytes[0x78800];
} FontImageStorage;

typedef struct FontState FontState;

struct FontExtCommand {
    unsigned char opcode;
    unsigned char function_high;
    unsigned char function_middle;
    unsigned char function_low;
    unsigned char argument_high;
    unsigned char argument_mid_high;
    unsigned char argument_mid_low;
    unsigned char argument_low;
    unsigned char terminator;
};

struct FontHexCommand {
    unsigned char opcode;
    unsigned char radix;
    unsigned char digits_minus_one;
    unsigned char value_high;
    unsigned char value_mid_high;
    unsigned char value_mid_low;
    unsigned char value_low;
    unsigned char terminator;
};

union FontScalePair {
    struct {
        unsigned short x;
        unsigned short y;
    } axis;
    unsigned int both;
};

struct FontPositionCommand {
    unsigned char opcode;
    unsigned char flags;
    unsigned char x_high;
    unsigned char x_low;
    unsigned char y_high;
    unsigned char y_low;
    unsigned char color_high;
    unsigned char color_middle;
    unsigned char color_low;
};

struct FontCompactPositionCommand {
    unsigned char x_high;
    unsigned char x_low;
    unsigned char y_high;
    unsigned char y_low;
    unsigned char opcode;
    unsigned char color_high;
    unsigned char color_middle;
    unsigned char color_low;
};

union FontPrintArgument {
    unsigned long long integer;
    double real;
    unsigned char *text;
};

struct FontNumericCommand {
    unsigned char opcode;
    unsigned char radix;
    unsigned char digits_minus_one;
    unsigned char value_high;
    unsigned char value_mid_high;
    unsigned char value_mid_low;
    unsigned char value_low;
};

struct FontSpriteCorner {
    unsigned int texture_u_or_x;
    unsigned int texture_v_or_y;
    unsigned int backup_x;
    unsigned int backup_y;
    unsigned int x;
    unsigned int y;
    unsigned int depth;
    unsigned int depth_upper;
};

struct FontSpritePacket {
    unsigned int tag_low;
    unsigned int tag_high;
    unsigned int registers_low;
    unsigned int registers_high;
    unsigned long long texture;
    unsigned char unmodeled_18[8];
    unsigned int red;
    unsigned int green;
    unsigned int blue;
    unsigned int alpha;
    struct FontSpriteCorner corners[4];
};

struct KanjiClutEnvironment {
    unsigned char unmodeled_00[0xb0];
    unsigned short code_by_character[32];
};

struct FontGsRegisterWrite {
    unsigned long long value;
    unsigned long long register_address;
};

struct FontModePacket {
    unsigned int vif_commands[4];
    unsigned long long gif_tag[2];
    struct FontGsRegisterWrite frame;
    struct FontGsRegisterWrite state_registers[5];
};

struct FontWindowTextureHeader {
    char signature[4];
    unsigned int total_size;
    int subimage_count;
    unsigned int table_offset;
};

struct FontWindowTextureEntry {
    unsigned int width_and_buffer_width;
    unsigned int height_word;
    unsigned int gs_offset;
    unsigned int dma_quadwords;
    unsigned int data_offset;
};

struct FontWindowTransferPacket {
    unsigned int vif_commands[4];
    unsigned long long gif_tag[2];
    struct FontGsRegisterWrite bitbltbuf;
    struct FontGsRegisterWrite trxpos;
    struct FontGsRegisterWrite trxreg;
    struct FontGsRegisterWrite trxdir;
};

struct FontLiteralHexCommand {
    unsigned char opcode;
    unsigned char digit_index;
    unsigned char value_high;
    unsigned char value_mid_high;
    unsigned char value_mid_low;
    unsigned char value_low;
};

struct FontPrimitiveStatePacket {
    unsigned int tag_low, tag_high;
    unsigned int register_low, register_high;
    unsigned int primitive_flags, primitive_mode;
    unsigned int register_address, register_address_high;
};

struct FontRealDigits {
    signed char value[16];
};

struct FontQueueRecord {
    unsigned char next_low;
    unsigned char next_high;
};

extern unsigned char spCodeParameterSizeTable[];
extern unsigned int LoopCount;
static signed char tbl_7[] ;

static void set_xyz(int x, int y, int color);
static void xglFontPrintSub(void *args);
static void xglFontPrintDirectCore(const char *text);

/* Prints text straight away into ordering-table slot ot (passed to set_ot). */
void xglFontPrintDirectOT(int ot, const char *text);

static void set_ot(int ot);

/*
 * Draws one glyph from the font texture: u and v are texel coordinates in
 * 1/16 units, size packs the glyph width (high byte) and height (low byte),
 * flags' low nibble selects the draw attributes and bit 4 can double the
 * drawn size.
 */
static void xglFontFlushSub(int u, int v, int size, int flags);

extern void sceVif1PkCloseDirectHLCode(XglPacket *packet);
extern int xglFontCheckProportional(unsigned char high, int low);
static void xglFontFlushCore(struct FontDrawCursor *cursor, unsigned char *text);
extern int WindowTexLoad(unsigned char *buffer, unsigned int request);
extern void sceVif1PkAddDataN(XglPacket *packet, const void *data, int words);
extern void sceVif1PkCnt(XglPacket *packet, int mode);
extern void sceVif1PkOpenDirectHLCode(XglPacket *packet, int mode);
extern float dptofp(unsigned long long value);
extern void sceVif1PkAddDirectDataN(XglPacket *packet, const void *data, int size);

/* The native SDK returns the previous writer and advances it by words * 4. */
extern void *sceVif1PkReserve(XglPacket *packet, int words);

static const char fontTexture0Path[16] = "data\\font0.tex";

static const char fontTexture1Path[16] = "data\\font1.tex";

static unsigned short ascii2euc[96];

static struct KanjiClutEnvironment KanjiClutEnv;

static unsigned char spcode_0[32];

static struct FontModePacket ModeEnv[2];

static unsigned int KanjiTex0Env_8[28];

static unsigned int KanjiTex1Env_9[28];

static unsigned int TestEnv_11_00490BC0[24];

static unsigned int TestEnv_12_00490C20[24];

static unsigned int TestEnv_13[24];

static struct FontWindowTransferPacket WinTexEnv_10;

static signed char tbl_1_004DC2A8[4][2];

static unsigned int normal_2[16];

static unsigned int back0_3[8];

static unsigned int back1_4[8];

static unsigned int biliner_6[8];

static unsigned int nearest_5[8];

static unsigned int zbuf_14[24];

static unsigned int test_15[8];

static struct FontModePacket ModeEnv[2] = { { .vif_commands = { 0U, 0U, 0U, 1358954503U }, .gif_tag = { 1152921504606879750ULL, 14ULL }, .frame = { .value = 0ULL, .register_address = 76ULL }, .state_registers = { { .value = 0ULL, .register_address = 24ULL }, { .value = 62768919827382272ULL, .register_address = 64ULL }, { .value = 4294967296ULL, .register_address = 78ULL }, { .value = 196608ULL, .register_address = 71ULL }, { .value = 0ULL, .register_address = 34ULL } } }, { .vif_commands = { 0U, 0U, 0U, 1358954503U }, .gif_tag = { 1152921504606879750ULL, 14ULL }, .frame = { .value = 0ULL, .register_address = 76ULL }, .state_registers = { { .value = 0ULL, .register_address = 24ULL }, { .value = 62768919827382272ULL, .register_address = 64ULL }, { .value = 4294967296ULL, .register_address = 78ULL }, { .value = 196608ULL, .register_address = 71ULL }, { .value = 0ULL, .register_address = 34ULL } } } };

static unsigned short ascii2euc[96] = { 41377, 53685, 53698, 53733, 53929, 53734, 53928, 53696, 53707, 53708, 53684, 53758, 53690, 53757, 53689, 53716, 41904, 41905, 41906, 41907, 41908, 41909, 41910, 41911, 41912, 41913, 53692, 53691, 53703, 53756, 53704, 53687, 53677, 41921, 41922, 41923, 41924, 41925, 41926, 41927, 41928, 41929, 41930, 41931, 41932, 41933, 41934, 41935, 41936, 41937, 41938, 41939, 41940, 41941, 41942, 41943, 41944, 41945, 41946, 53709, 53669, 53710, 53720, 53715, 53722, 41953, 41954, 41955, 41956, 41957, 41958, 41959, 41960, 41961, 41962, 41963, 41964, 41965, 41966, 41967, 41968, 41969, 41970, 41971, 41972, 41973, 41974, 41975, 41976, 41977, 41978, 53711, 53713, 53712, 53736, 41377 };

static struct FontWindowTransferPacket WinTexEnv_10 = { .vif_commands = { 0U, 0U, 285212672U, 1358954501U }, .gif_tag = { 1152921504606846980ULL, 14ULL }, .bitbltbuf = { .value = 0ULL, .register_address = 80ULL }, .trxpos = { .value = 0ULL, .register_address = 81ULL }, .trxreg = { .value = 0ULL, .register_address = 82ULL }, .trxdir = { .value = 0ULL, .register_address = 83ULL } };

static unsigned int normal_2[16] = { 32771U, 268435456U, 14U, 0U, 0U, 0U, 20U, 0U, 458752U, 0U, 71U, 0U, 0U, 0U, 66U, 0U };

static unsigned int back0_3[8] = { 32769U, 268435456U, 14U, 0U, 458765U, 0U, 71U, 0U };

static unsigned int back1_4[8] = { 32769U, 268435456U, 14U, 0U, 458752U, 0U, 71U, 0U };

static unsigned int nearest_5[8] = { 32769U, 268435456U, 14U, 0U, 0U, 0U, 20U, 0U };

static unsigned int biliner_6[8] = { 32769U, 268435456U, 14U, 0U, 96U, 0U, 20U, 0U };

static unsigned int KanjiTex0Env_8[28] = { 0U, 0U, 285212672U, 1358954502U, 5U, 268435456U, 14U, 0U, 0U, 0U, 63U, 0U, 0U, 342048U, 80U, 0U, 0U, 0U, 81U, 0U, 320U, 192U, 82U, 0U, 0U, 0U, 83U, 0U };

static unsigned int KanjiTex1Env_9[28] = { 0U, 0U, 285212672U, 1358954502U, 5U, 268435456U, 14U, 0U, 0U, 0U, 63U, 0U, 0U, 343040U, 80U, 0U, 0U, 0U, 81U, 0U, 320U, 192U, 82U, 0U, 0U, 0U, 83U, 0U };

static unsigned int TestEnv_11_00490BC0[24] = { 0U, 0U, 0U, 1358954501U, 32772U, 268435456U, 14U, 0U, 0U, 0U, 63U, 0U, 0U, 0U, 66U, 0U, 0U, 0U, 20U, 0U, 5U, 0U, 8U, 0U };

static unsigned int TestEnv_12_00490C20[24] = { 0U, 0U, 0U, 1358954501U, 32772U, 268435456U, 14U, 0U, 822083584U, 0U, 78U, 0U, 0U, 0U, 73U, 0U, 0U, 0U, 74U, 0U, 458765U, 0U, 71U, 0U };

static unsigned int TestEnv_13[24] = { 0U, 0U, 0U, 1358954501U, 32772U, 268435456U, 14U, 0U, 822083584U, 0U, 78U, 0U, 0U, 0U, 73U, 0U, 0U, 0U, 74U, 0U, 458752U, 0U, 71U, 0U };

static unsigned int zbuf_14[24] = { 32773U, 268435456U, 14U, 0U, 28672U, 29184U, 24U, 0U, 33488896U, 29294592U, 64U, 0U, 822083584U, 0U, 78U, 0U, 458752U, 0U, 71U, 0U, 524400U, 0U, 76U, 0U };

static unsigned int test_15[8] = { 32769U, 268435456U, 14U, 0U, 0U, 0U, 63U, 0U };

static struct KanjiClutEnvironment KanjiClutEnv = { { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x51, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x05, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x52, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x53, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x55, 0x80, 0xaa, 0xaa, 0xaa, 0x80, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x55, 0x80, 0xaa, 0xaa, 0xaa, 0x80, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x55, 0x80, 0x55, 0x55, 0x55, 0x80, 0x55, 0x55, 0x55, 0x80, 0x55, 0x55, 0x55, 0x80 }, { 0x0000, 0x0000, 0x5555, 0x8055, 0xaaaa, 0x80aa, 0xffff, 0x80ff, 0x0000, 0x0000, 0x5555, 0x8055, 0xaaaa, 0x80aa, 0xffff, 0x80ff, 0xaaaa, 0x80aa, 0xaaaa, 0x80aa, 0xaaaa, 0x80aa, 0xaaaa, 0x80aa, 0xffff, 0x80ff, 0xffff, 0x80ff, 0xffff, 0x80ff, 0xffff, 0x80ff } };

static unsigned char spcode_0[32] = { 0, 1, 1, 1, 1, 1, 3, 0, 255, 0, 0, 0, 3, 1, 5, 3, 5, 6, 2, 2, 0, 255, 0, 7, 1, 1, 0, 1, 0, 0, 0, 0 };

static signed char tbl_7[] = { -16, 10, 37 };

static FontStateStorage FS;

static FontPrintQueueStorage fontPrintQueueBuffer;

static FontImageStorage FontImage;

static unsigned char WindowImage[264192];

void xglFontDebugMode(int mode)
{
    if (mode >= 0) {
        struct FontModePacket *packets = ModeEnv;
        int value0;
        unsigned long long value1;

        FS.state.debug_mode = mode;
        value1 = FS.state.debug_mode | 0x50000ULL;
        value0 = 0x50000;
        value0 |= ((unsigned int)FS.state.debug_mode & 0xff) + 0x23;

        packets[0].frame.value = value0;
        packets[1].frame.value = value1;
    }
}

int xglFontGetKanjiClutUV(int code, int *clut)
{
    unsigned short index = code;
    unsigned char column = index >> 8;
    unsigned char row = index;
    int first_range_or_texture_column;
    int second_range;

    first_range_or_texture_column = row < 0xad;
    second_range = row < 0xb0;
    index = (column + row * 94) - 0xa1;
    if (first_range_or_texture_column) {
        index += 0xc4e2;
    } else if (second_range) {
        index += 0xc30c;
    } else {
        index += 0xc250;
    }
    column = index & 0x1f;
    row = index >> 5;
    first_range_or_texture_column = column;
    if (clut != 0) {
        *clut = ((row >> 5) << 12) + 0x1004;
    }
    return ((row & 0x1f) * 0x01800000) +
        (first_range_or_texture_column * 0x140);
}

static void set_ot(int ot)
{
    FontState *state = (FontState *)(fontPrintQueueBuffer.bytes - sizeof(FontState));
    FontQueueSlot *slot = &fontPrintQueueBuffer.slots[((ot >> 22) & 0x3c) / 4];
    unsigned char *queue_bytes = (unsigned char *)&fontPrintQueueBuffer;
    unsigned short offset = state->queue_cursor - queue_bytes;
    unsigned char *previous = queue_bytes + slot->selfOffset;

    previous[0] = offset;
    previous[1] = offset >> 8;
    slot->selfOffset = offset;
    state->queue_cursor[0] = 0;
    state->queue_cursor[1] = 0;
    state->queue_cursor += 2;
}

static void set_xyz(int x, int y, int color)
{
    set_ot(color);
    if ((unsigned int)x >= 768 || y < 0 || y >= 512) {
        struct FontPositionCommand *command = (struct FontPositionCommand *)FS.state.queue_cursor;

        command->opcode = 8;
        command->flags = 7;
        command->x_high = x >> 8;
        command->x_low = x;
        command->y_high = y >> 8;
        command->y_low = y;
        command->color_high = color >> 16;
        command->color_middle = color >> 8;
        command->color_low = color;
        FS.state.queue_cursor += sizeof(*command);
    } else {
        struct FontCompactPositionCommand *command = (struct FontCompactPositionCommand *)FS.state.queue_cursor;

        command->x_high = (x >> 8) + 1;
        command->x_low = x;
        command->y_high = (y >> 8) + 4;
        command->y_low = y;
        command->opcode = 6;
        command->color_high = color >> 16;
        command->color_middle = color >> 8;
        command->color_low = color;
        FS.state.queue_cursor += sizeof(*command);
    }
}

int xglFontGetSPcodeSize(int code, const unsigned char *text)
{
    unsigned char size;

    code &= 0xff;
    size = spcode_0[code];

    if (size == 255) {
        switch (code) {
        case 8: {
            unsigned char parameter_sizes[8];
            unsigned char flags = text[1];
            int bit;

            __builtin_memcpy(parameter_sizes, spCodeParameterSizeTable,
                             sizeof(parameter_sizes));
            size = 1;
            for (bit = 0; bit < 8; bit++) {
                if (flags & 1) {
                    size += parameter_sizes[bit];
                }
                flags >>= 1;
            }
            break;
        }
        case 21:
            size = text[1] + 1;
            break;
        }
    }

    return size;
}

static void xglFontPrintSub(void *args)
{
    unsigned long long *list = (unsigned long long *)args;
    const char *text = (const char *)(int)list[0];
    unsigned char ahead = *text;

    list++;
    for (;;) {
        unsigned int code;

        if (*text == 0)
            break;
        code = *text & 0xff;

        if (code < 32) {
            int remaining = xglFontGetSPcodeSize(code, (const unsigned char *)text);

            for (; remaining >= 0; remaining--)
                *FS.state.queue_cursor++ = *text++;
        } else if (code == '%') {
            int width = 0;
            unsigned int radix;

            text++;
            for (;;) {
                code = (unsigned char)*text++;

                switch (code) {
                case '0': case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9':
                    width = width * 10 + (code - '0');
                    continue;
                case 'b':
                    radix = 2;
                    goto numeric_tail;
                case 'd':
                    radix = 10;
                    goto numeric_tail;
                case 'x':
                    radix = 16;
                    goto numeric_tail;
                numeric_tail:
                    {
                        unsigned long long value = (unsigned int)list[0];
                        struct FontNumericCommand *command = (struct FontNumericCommand *)FS.state.queue_cursor;

                        list++;
                        if (width == 0)
                            width = 1;
                        command->opcode = 17;
                        command->radix = radix;
                        command->digits_minus_one = (unsigned char)(width - 1);
                        command->value_high = value >> 24;
                        command->value_mid_high = value >> 16;
                        command->value_mid_low = value >> 8;
                        command->value_low = value;
                        FS.state.queue_cursor += sizeof(*command);
                    }
                    break;
                case 'f':
                    goto float_tail;
                float_tail:
                    {
                        float value = dptofp(list[0]);
                        unsigned int bits = *(unsigned int *)&value;
                        struct FontNumericCommand *command = (struct FontNumericCommand *)FS.state.queue_cursor;

                        list++;
                        command->opcode = 17;
                        command->radix = 0xff;
                        command->digits_minus_one = 0xff;
                        command->value_high = bits >> 24;
                        command->value_mid_high = bits >> 16;
                        command->value_mid_low = bits >> 8;
                        command->value_low = bits;
                        FS.state.queue_cursor += sizeof(*command);
                    }
                    break;
                case 's': {
                    const unsigned char *string = (const unsigned char *)(int)list[0];

                    int c;

                    list++;
                    while ((c = *string) != 0) {
                        *FS.state.queue_cursor++ = *string++;
                    }
                    break;
                }
                case 'c':
                    code = (unsigned char)list[0];
                    list++;
                    /* fall through */
                case '%':
                    *FS.state.queue_cursor++ = code;
                    break;
                default:
                    break;
                }
                break;
            }
        } else {
            text++;
            *FS.state.queue_cursor++ = ahead;
        }
        ahead = *text;
    }
    *FS.state.queue_cursor++ = 0;
}

static void xglFontPrintDirectCore(const char *text)
{
    while (*text != 0) {
        unsigned char code = *text;

        if (code < 32) {
            int remaining = xglFontGetSPcodeSize(code, (const unsigned char *)text);

            while (remaining > 0) {
                *FS.state.queue_cursor++ = *text++;
                remaining--;
            }
        }
        *FS.state.queue_cursor++ = *text++;
    }
    *FS.state.queue_cursor++ = 0;
}

void xglFontPrintf(int x, int y, unsigned int color, void *arg)
{
    if (FONT_STATE_FLAGS & 1 & 0xFFFF) {
        set_xyz(x, y, color);
        xglFontPrintSub(arg);
    }
}

void xglFontPrint(int x, int y, int color, const char *text)
{
    if (FONT_STATE_FLAGS & 1 & 0xFFFF) {
        set_xyz(x, y, color);
        xglFontPrintDirectCore(text);
    }
}

void xglFontPrintDirect(const char *text)
{
    xglFontPrintDirectOT(0, text);
}

void xglFontPrintDirectOT(int ot, const char *text)
{
    if (FONT_STATE_FLAGS & 1 & 0xFFFF) {
        set_ot(ot);
        xglFontPrintDirectCore(text);
    }
}

/* Queues a draw callback: the command stores the function address and the
 * argument as big-endian bytes, and xglFontFlushCore calls it back. */
void xglFontPrintExtFunc(int ot, FontExtFunc function, void *argument)
{
    struct FontState *state = &FS.state;
    struct FontExtCommand *command;
    unsigned int function_address = (unsigned int)function;
    unsigned int argument_word = (unsigned int)argument;

    if (state->flags & 1 & 0xffff) {
        set_ot(ot);
        command = (struct FontExtCommand *)state->queue_cursor;
        command->opcode = 23;
        command->function_high = function_address >> 16;
        command->function_middle = function_address >> 8;
        command->function_low = function_address;
        command->argument_high = argument_word >> 24;
        command->argument_mid_high = argument_word >> 16;
        command->argument_mid_low = argument_word >> 8;
        command->argument_low = argument_word;
        command->terminator = 0;
        state->queue_cursor += sizeof(*command);
    }
}

void xglFontDebugPrintf(int x, int y, const char *format, ...)
{
    unsigned long long arguments[32];
    unsigned long long *output;
    unsigned long long *list;

    if ((FONT_STATE_FLAGS & 3) == 3) {
        arguments[0] = (unsigned int)format;
        {
        int count;

        FONT_ARGUMENT_START(list, format);
        output = arguments + 1;
        count = 30;
        do {
            unsigned long long *argument = list++;
            unsigned long long value = *argument;

            count--;
            *output = value;
            output++;
        } while (count >= 0);
        }
        if (FS.state.debug_mode == 0) {
            x <<= 1;
            y <<= 1;
        }
        set_xyz(x, y, -1);
        *FS.state.queue_cursor++ = 11;
        xglFontPrintSub(arguments);
    }
}

void xglFontDebugHex(int x, int y, unsigned int value, int digits)
{
    struct FontState *state = &FS.state;
    struct FontHexCommand *command;

    if ((state->flags & 3) == 3 && digits != 0) {
        if (state->debug_mode == 0) {
            x <<= 1;
            y <<= 1;
        }
        set_xyz(x, y, -1);
        command = (struct FontHexCommand *)state->queue_cursor;
        command->opcode = 11;
        command->radix = 16;
        command->digits_minus_one = digits - 1;
        command->value_high = value >> 24;
        command->value_mid_high = value >> 16;
        command->value_mid_low = value >> 8;
        command->value_low = value;
        command->terminator = 0;
        state->queue_cursor += sizeof(*command);
    }
}

static void buffer_reset(void)
{
    FontQueueSlot *slot;
    struct FontState *state;
    int remaining;

    remaining = 15;
    slot = FONT_PRINT_QUEUE;
    do {
        slot->selfOffset = (unsigned char *)slot - (unsigned char *)FONT_PRINT_QUEUE;
        remaining--;
        slot->link = 0;
        slot++;
    } while (remaining >= 0);

    state = (struct FontState *)(fontPrintQueueBuffer.bytes - sizeof(struct FontState));
    state->queue_cursor = (unsigned char *)slot;
    state->queue_reset_flag = 0;
}

static void xglFontFlushSub(int texture_u, int texture_v, int size, int flags)
{
    struct FontDrawCursor *cursor = (struct FontDrawCursor *)0x70000000;
    struct FontSpritePacket *sprite = (struct FontSpritePacket *)0x70000100;
    int glyph_width = size >> 8;
    unsigned int glyph_height = size & 0xff;
    int texture_width;
    int texture_height;
    int draw_width;
    int draw_height;
    int effect_x;
    int effect_y;
    int cursor_x = (short)cursor->x;
    int cursor_y = (short)cursor->y;
    int offset_x = (short)cursor->offset_x;
    int offset_y = (short)cursor->offset_y;
    unsigned int left = ((offset_x + cursor_x) * 16) - 8;
    unsigned int top = ((offset_y + cursor_y) * 16) - 9;

    int attributes = flags & 15;
    texture_width = glyph_width * 16;
    texture_height = glyph_height * 16;
    cursor->attributes = attributes;
    if (!FS.state.debug_mode && (flags & 16)) {
        draw_width = texture_width * 2;
        draw_height = texture_height * 2;
    } else {
        draw_width = glyph_width * 16;
        draw_height = glyph_height * 16;
    }
    draw_width = (draw_width * (short)cursor->scale_x) >> 4;
    effect_x = cursor->effect_x * 16;
    effect_y = cursor->effect_y * 16;
    draw_height = (draw_height * (short)cursor->scale_y) >> 4;
    cursor->x += draw_width / 16;
    if ((FS.state.flags & 8) && (left > 0xf000U || top > 0xf000U)) {
        return;
    }
    {
        unsigned long long texture;

        sprite->tag_low = 0x8001;
        sprite->tag_high = 0x60ab4000;
        sprite->registers_low = 0x535316;
        sprite->registers_high = 0;
        texture = (attributes & 2) ? 0x2a942bc00ULL : 0x2a942b820ULL;
        sprite->texture = texture | ((!FS.state.debug_mode) ? 0x2000000400000000ULL : 0x2000000000000000ULL) |
                   (((unsigned long long)((attributes & 1) + 0x3800)) << 37);
        sprite->red = cursor->red;
        sprite->green = cursor->green;
        sprite->blue = cursor->blue;
        sprite->alpha = cursor->alpha;
        sprite->corners[0].texture_u_or_x = texture_u;
        sprite->corners[0].texture_v_or_y = texture_v;
        if (FS.state.horizontal_shift) {
            unsigned int shifted_left = left + FS.state.horizontal_shift * 16;
            int u_end = texture_u + texture_width;

            sprite->corners[0].y = top;
            sprite->corners[2].texture_u_or_x = texture_u;
            sprite->corners[1].texture_v_or_y = texture_v;
            sprite->tag_low = 0x8001;
            sprite->tag_high = 0xa0aa4000;
            sprite->registers_low = 0x53535316;
            sprite->registers_high = 0x53;
            sprite->corners[0].x = shifted_left;
            sprite->corners[0].depth = cursor->depth;
            sprite->corners[0].depth_upper = 0;
            sprite->corners[1].y = top;
            sprite->corners[1].x = shifted_left + draw_width;
            sprite->corners[1].texture_u_or_x = u_end;
            sprite->corners[1].depth = cursor->depth;
            sprite->corners[1].depth_upper = 0;
            sprite->corners[2].x = left;
            sprite->corners[2].texture_v_or_y = texture_v + texture_height;
            sprite->corners[2].y = top + draw_height;
            sprite->corners[2].depth = cursor->depth;
            sprite->corners[2].depth_upper = 0;
            sprite->corners[3].texture_u_or_x = u_end;
            sprite->corners[3].texture_v_or_y = texture_v + texture_height;
            sprite->corners[3].x = left + draw_width;
            sprite->corners[3].y = top + draw_height;
            sprite->corners[3].depth = cursor->depth;
            sprite->corners[3].depth_upper = 0;
            sceVif1PkAddDirectDataN(cursor->packet, sprite, 11);
            return;
        }
        sprite->corners[0].x = left;
        sprite->corners[0].y = top;
        sprite->corners[0].depth = cursor->depth;
        sprite->corners[0].depth_upper = 0;
        sprite->corners[1].texture_u_or_x = texture_u + texture_width;
        sprite->corners[1].texture_v_or_y = texture_v + texture_height;
        sprite->corners[1].x = left + draw_width;
        sprite->corners[1].y = top + draw_height;
        sprite->corners[1].depth = cursor->depth;
        sprite->corners[1].depth_upper = 0;
        sceVif1PkAddDirectDataN(cursor->packet, sprite, 7);
        sprite->corners[0].depth--;
        sprite->corners[1].depth--;
        if (cursor->effect == 4) {
            unsigned int old_left = sprite->corners[0].x;
            unsigned int old_right = sprite->corners[1].x;
            unsigned int old_top = sprite->corners[0].y;
            unsigned int old_bottom = sprite->corners[1].y;

            sprite->corners[0].x = old_left + 16;
            sprite->corners[2].texture_v_or_y = old_top;
            sprite->corners[2].backup_x = old_right;
            sprite->corners[2].backup_y = old_bottom;
            sprite->corners[0].y = old_top;
            sprite->corners[1].y = old_bottom;
            sprite->corners[1].x = old_right + 16;
            sprite->corners[2].texture_u_or_x = old_left;
            sceVif1PkAddDirectDataN(cursor->packet, sprite, 7);
            sprite->corners[0].x = sprite->corners[2].texture_u_or_x;
            sprite->corners[0].y = sprite->corners[2].texture_v_or_y + 16;
            sprite->corners[1].x = sprite->corners[2].backup_x;
            sprite->corners[1].y = sprite->corners[2].backup_y + 16;
            sceVif1PkAddDirectDataN(cursor->packet, sprite, 7);
        } else if (cursor->effect >= 2) {
            unsigned char effect_red = cursor->effect_red;
            unsigned char effect_green = cursor->effect_green;
            unsigned char effect_blue = cursor->effect_blue;
            unsigned int old_left = sprite->corners[0].x;
            unsigned int old_top = sprite->corners[0].y;
            unsigned int old_right = sprite->corners[1].x;
            unsigned int old_bottom = sprite->corners[1].y;

            sprite->red = effect_red;
            sprite->green = effect_green;
            sprite->blue = effect_blue;
            sprite->corners[2].texture_u_or_x = old_left;
            sprite->corners[2].texture_v_or_y = old_top;
            sprite->corners[2].backup_x = old_right;
            sprite->corners[2].backup_y = old_bottom;
            if (cursor->effect == 2) {
                sprite->corners[0].x = old_left + effect_x;
                sprite->corners[0].y = old_top + effect_y;
                sprite->corners[1].x = old_right + effect_x;
                sprite->corners[1].y = old_bottom + effect_y;
                sceVif1PkAddDirectDataN(cursor->packet, sprite, 7);
            } else {
                int row;
                int column;
                int direction;

                for (row = 16; row <= effect_y; row += 16) {
                    for (column = 16; column <= effect_x; column += 16) {
                        for (direction = 0; direction < 4; direction++) {
                            int offset_x = tbl_1_004DC2A8[direction][0] * column;
                            int offset_y = tbl_1_004DC2A8[direction][1] * row;

                            sprite->corners[0].x = sprite->corners[2].texture_u_or_x + offset_x;
                            sprite->corners[0].y = sprite->corners[2].texture_v_or_y + offset_y;
                            sprite->corners[1].x = sprite->corners[2].backup_x + offset_x;
                            sprite->corners[1].y = sprite->corners[2].backup_y + offset_y;
                            sceVif1PkAddDirectDataN(cursor->packet, sprite, 7);
                        }
                    }
                }
            }
        }
    }
}

/* Hex digit glyphs are 6x8 texels, 8 texels apart at v 0x2F00 (row 752). */
static void xglFontFlushSubHex(int digit)
{
    xglFontFlushSub(digit << 7, 0x2F00, (6 << 8) | 8, 0x13);
}

static void xglFontFlushSubCRLF(void)
{
    struct FontDrawCursor *cursor = (struct FontDrawCursor *)0x70000000;
    int next_y = cursor->y + FS.state.line_height;
    int normal_y;

    cursor->y = next_y;
    cursor->x = cursor->origin_x;
    normal_y = next_y + FS.state.line_height;
    if (FS.state.debug_mode == 0) {
        cursor->y = normal_y;
    }
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
static const unsigned char kanaFontCodeTable[96];

static void xglFontFlushCore(struct FontDrawCursor *cursor, unsigned char *text)
{
    struct FontRealDigits real_digits;
    signed char integer_digits[32];
    unsigned char kana[96];

    while (*text != 0) {
        unsigned char opcode = *text;
        switch (opcode) {
        case 1: case 2: case 3: {
            unsigned int position_high = text[0];
            unsigned int position_low = text[1];
            int position;
            text += 2;
            position = position_low + (position_high << 8) - 0x100;
            cursor->x = position;
            cursor->origin_x = position;
            break;
        }
        case 4: case 5: {
            unsigned int position_high = text[0];
            unsigned int position_low = text[1];
            int position;
            text += 2;
            position = position_low + (position_high << 8) - 0x400;
            cursor->y = position;
            break;
        }
        case 6: {
            int depth = (text[1] << 16) + (text[2] << 8) + text[3];
            text += 4;
            cursor->depth = depth;
            break;
        }
        case 7:
            text++;
            break;
        case 8: {
            unsigned char flags = text[1];
            text += 2;
            if (flags & 1) {
                cursor->x = text[1] + (text[0] << 8);
                if (!(flags & 0x80)) cursor->origin_x = cursor->x;
                text += 2;
            }
            if (flags & 2) {
                cursor->y = text[1] + (text[0] << 8);
                text += 2;
            }
            if (flags & 4) {
                cursor->depth = (text[0] << 16) + (text[1] << 8) + text[2];
                text += 3;
            }
            if (flags & 8) {
                cursor->x += text[1] + (text[0] << 8);
                if (!(flags & 0x80)) cursor->origin_x = cursor->x;
                text += 2;
            }
            if (flags & 16) {
                cursor->y += text[1] + (text[0] << 8);
                text += 2;
            }
            break;
        }
        case 9:
            cursor->x = (cursor->x + 64) & 0xffc0;
            text++;
            break;
        case 10:
            xglFontFlushSubCRLF();
            text++;
            break;
        case 11:
            cursor->effect = 1;
            cursor->red = cursor->green = cursor->blue = 0x80;
            cursor->primitive_flags = 0;
            cursor->scale_x = 16;
            cursor->alpha = 0x80;
            cursor->scale_y = 16;
            cursor->flags &= 0xf8;
            if (!FS.state.debug_mode) sceVif1PkAddDirectDataN(cursor->packet, normal_2, 4);
            FS.state.default_width = 4;
            FS.state.character_spacing = 0;
            text++;
            break;
        case 12:
            cursor->red = text[1]; cursor->green = text[2]; cursor->blue = text[3];
            text += 4;
            break;
        case 13: {
            cursor->effect = text[1];
            switch (cursor->effect) {
            case 0:
            case 2:
            case 3:
            case 4:
                sceVif1PkAddDirectDataN(cursor->packet, back0_3, 2);
                break;
            case 1:
            default:
                sceVif1PkAddDirectDataN(cursor->packet, back1_4, 2);
                break;
            }
            text += 2;
            break;
        }
        case 14:
            cursor->effect_x = text[1]; cursor->effect_y = text[2];
            cursor->effect_red = text[3]; cursor->effect_green = text[4]; cursor->effect_blue = text[5];
            text += 6;
            break;
        case 15: {
            struct FontPrimitiveStatePacket *packet;
            cursor->primitive_flags = text[1]; cursor->primitive_mode = text[2]; cursor->alpha = text[3];
            packet = sceVif1PkReserve(cursor->packet, 8);
            packet->tag_low = 0x8001; packet->tag_high = 0x10000000;
            packet->register_low = 14; packet->register_high = 0;
            packet->primitive_flags = cursor->primitive_flags;
            packet->primitive_mode = cursor->primitive_mode;
            packet->register_address = 0x42; packet->register_address_high = 0;
            text += 4;
            break;
        }
        case 16: {
            const struct FontLiteralHexCommand *command =
                (const struct FontLiteralHexCommand *)text;
            unsigned int value = ((unsigned int)command->value_high << 24) +
                ((unsigned int)command->value_mid_high << 16) +
                ((unsigned int)command->value_mid_low << 8) + command->value_low;
            int digit = command->digit_index;
            while (digit >= 0) {
                xglFontFlushSubHex((value >> (digit * 4)) & 15);
                digit--;
            }
            text += sizeof(*command);
            break;
        }
        case 17: {
            const struct FontNumericCommand *command =
                (const struct FontNumericCommand *)text;
            if (command->radix == 255) {
                unsigned int mid_high = (unsigned int)command->value_mid_high << 16;
                unsigned int high = (unsigned int)command->value_high << 24;
                unsigned int mid = ((unsigned int)command->value_mid_low << 8) + mid_high;
                unsigned int bits = high + command->value_low + mid;
                unsigned int mantissa = (bits & 0x7fffff) + 0x800000;
                unsigned int shift = 150 - ((bits >> 23) & 255);
                long long integer;
                unsigned long long mask;
                unsigned long long fraction;
                int digit;
                int printed;
                int remaining;
                signed char *fraction_digit;

                if ((int)bits < 0)
                    xglFontFlushSubHex(16);
                else
                    xglFontFlushSubHex(32);
                if (shift >= 32) {
                    integer = 0;
                    mask = (0x100000000ULL << (shift - 32)) - 1;
                } else {
                    integer = mantissa >> shift;
                    mask = (int)((1U << shift) - 1);
                }
                fraction = mantissa & mask;
                for (digit = 7; digit >= 0; digit--) {
                    real_digits.value[digit] = integer % 10;
                    integer /= 10;
                    fraction *= 10;
                    if (shift >= 32)
                        real_digits.value[digit + 8] = ((long long)fraction >> 32) >> (shift - 32);
                    else
                        real_digits.value[digit + 8] = (long long)fraction >> shift;
                    fraction &= mask;
                }
                printed = 0;
                for (digit = 0; digit < 7; digit++) {
                    if (real_digits.value[digit] || printed) {
                        printed++;
                        xglFontFlushSubHex(real_digits.value[digit]);
                    }
                }
                xglFontFlushSubHex(real_digits.value[7]);
                xglFontFlushSubHex(17);
                printed += 2;
                remaining = 7;
                for (; remaining >= 0 && printed < 9; remaining--) {
                    printed++;
                    xglFontFlushSubHex(real_digits.value[remaining + 8]);
                }
            } else {
                int suppress_zero = 0;
                int radix = command->radix;
                int digit = command->digits_minus_one;
                unsigned int value = ((unsigned int)command->value_high << 24) +
                    ((unsigned int)command->value_mid_high << 16) +
                    ((unsigned int)command->value_mid_low << 8) + command->value_low;
                if (radix == 10) {
                    suppress_zero = 1;
                    if ((int)value < 0) { value = -value; suppress_zero = 2; }
                }
                integer_digits[digit + 1] = -1;
                while (digit >= 0) {
                    integer_digits[digit] = value % radix;
                    value /= radix;
                    digit--;
                }
                for (digit = 0; integer_digits[digit] >= 0; digit++) {
                    if (suppress_zero && !integer_digits[digit] && integer_digits[digit + 1] >= 0) {
                        if (suppress_zero == 2 && integer_digits[digit + 1] > 0) xglFontFlushSubHex(16);
                        else xglFontFlushSubHex(32);
                    } else {
                        xglFontFlushSubHex(integer_digits[digit]);
                        suppress_zero = 0;
                    }
                }
            }
            text += sizeof(*command);
            break;
        }
        case 18:
            cursor->scale_x = text[1]; cursor->scale_y = text[2];
            if (((union FontScalePair *)&cursor->scale_x)->both == 0x100010)
                sceVif1PkAddDirectDataN(cursor->packet, nearest_5, 2);
            else sceVif1PkAddDirectDataN(cursor->packet, biliner_6, 2);
            text += 3;
            break;
        case 19: {
            struct FontEffectScratchPacket *packet = (struct FontEffectScratchPacket *)
                (((unsigned int)&cursor->work.effect_packet + 15) & ~15);
            long long horizontal_mode;
            long long horizontal_mask;
            long long vertical_mask;
            long long vertical_mode;
            long long horizontal_random;
            long long vertical_random;
            packet->vif_tag = 0x1000000000008001ULL;
            packet->vif_count = 14;
            if (!text[1]) {
                horizontal_mode = 1; horizontal_mask = horizontal_random = 0;
            } else {
                unsigned long long mask_byte = text[1];
                horizontal_mode = 3; horizontal_mask = mask_byte ^ 0x3ff;
                if (mask_byte & 0x80) {
                    unsigned short random = xglSRand();
                    horizontal_random = text[1] & random;
                } else {
                    horizontal_random = 0;
                }
            }
            if (!text[2]) {
                vertical_mode = 1; vertical_mask = vertical_random = 0;
            } else {
                vertical_mode = 3; vertical_mask = text[1] ^ 0x3ff;
                if (text[2] & 0x80) {
                    unsigned short random = xglSRand();
                    vertical_random = text[1] & random;
                } else {
                    vertical_random = 0;
                }
            }
            packet->payload_count = 8;
            packet->effect_state = horizontal_mode | (vertical_mode << 2) | (horizontal_mask << 4) |
                (horizontal_random << 14) | (vertical_mask << 24) | (vertical_random << 34);
            sceVif1PkAddDirectDataN(cursor->packet, packet, 2);
            text += 3;
            break;
        }
        case 21: {
            const unsigned char *payload = text + 2;
            int length = text[1];
            text += length + 2;
            switch (payload[0]) {
            case 1: if (length == 2) FS.state.line_height = payload[1]; break;
            case 2: if (length == 2) FS.state.default_width = payload[1]; break;
            case 3: if (length == 3) FS.state.character_spacing = (payload[1] << 8) + payload[2]; break;
            case 4: if (length == 2) FS.state.proportional_mode = payload[1]; break;
            case 5: if (length == 2) FS.state.horizontal_shift = payload[1]; break;
            }
            break;
        }
        case 23: {
            const struct FontExtCommand *command =
                (const struct FontExtCommand *)text;
            FontExtFunc callback = (FontExtFunc)
                (((unsigned int)command->function_high << 16) +
                 ((unsigned int)command->function_middle << 8) +
                 command->function_low);
            void *argument = (void *)(((unsigned int)command->argument_high << 24) +
                ((unsigned int)command->argument_mid_high << 16) +
                ((unsigned int)command->argument_mid_low << 8) +
                command->argument_low);
            text += 8;
            cursor->flags &= 0xbf;
            callback(cursor, argument);
            break;
        }
        case 24: {
            unsigned char flags = cursor->flags & 0xfe;
            unsigned char enable = text[1];
            text += 2;
            if (enable) flags |= 1;
            cursor->flags = flags;
            break;
        }
        case 25: {
            unsigned char bits = text[1];
            unsigned char flags = cursor->flags & 0xf9;
            text += 2;
            if (bits & 1) flags |= 2;
            if (bits & 2) flags |= 4;
            cursor->flags = flags;
            break;
        }
        case 27: {
            signed char character = text[1];
            if (character < 64) xglFontFlushSub((character & 63) << 7, 0x2f00, 0x808, 0x13);
            else xglFontFlushSub((character & 63) << 7, 0x2f80, 0x808, 0x13);
            text += 2;
            break;
        }
        case 20: case 22: case 26:
        case 28: case 29: case 30: case 31:
            break;
        default: {
            signed char sign;
            if ((short)FS.state.character_spacing > 0 && opcode == 32 &&
                (short)cursor->x - cursor->origin_x + xglFontGetStringWidth2((const char *)text, 1) > (short)FS.state.character_spacing) {
                xglFontFlushSubCRLF();
            } else if ((sign = *text) >= 0 && !(cursor->flags & 4)) {
                if (sign < 64) xglFontFlushSub((sign & 63) << 7, 0x2f00, 0x808, 0x13);
                else xglFontFlushSub((sign & 63) << 7, 0x2f80, 0x808, 0x13);
            } else {
                if ((unsigned char)(*text + 128) < 32) {
                    xglFontFlushSub(0x1f80, 0x2f80, 0x808, 0x13);
                } else {
                    unsigned char high;
                    unsigned int low;
                    int character;
                    if ((signed char)*text >= 0) {
                        unsigned int euc = xglFontAscii2Euc((signed char)*text, (signed char **)&text);
                        character = euc;
                        high = (euc >> 8) & 255; low = euc & 255;
                    } else {
                        high = *text++;
                        low = *text;
                        character = ((high << 8) + low) & 0xffff;
                    }
                    if ((cursor->flags & 1) &&
                        ((unsigned char)(high + 0x5d) < 3 || character == 0xa1a6 || character == 0xa1bc || character == 0xa1dd)) {
                        if (character == 0xa1a6 || character == 0xa1bc || character == 0xa1dd) {
                            switch (character) {
                            case 0xa1a6:
                                xglFontFlushSub(0x280, 0x2e80, 0x808, 3);
                                break;
                            case 0xa1bc:
                            case 0xa1dd:
                                xglFontFlushSub(0x800, 0x2e80, 0x808, 3);
                                break;
                            }
                        } else {
                            switch ((int)high) {
                            case 0xa3:
                                if (low < 0xc0) xglFontFlushSub((low & 63) << 7, 0x2f00, 0x808, 3);
                                else xglFontFlushSub((low & 63) << 7, 0x2f80, 0x808, 3);
                                break;
                            case 0xa4:
                            case 0xa5: {
                                __builtin_memcpy(kana, kanaFontCodeTable, sizeof(kana));
                                low = kana[low - 0xa0];
                                xglFontFlushSub((low & 63) << 7, (0x101 - high) << 7, 0x808, 3);
                                if (low & 64) xglFontFlushSub((low & 128) + 0x1f00, 0x2e80, 0x808, 3);
                                break;
                            }
                            }
                        }
                    } else {
                        int proportional = xglFontCheckProportional(high, low);
                        if ((cursor->flags & 2) && proportional >= 0) {
                            unsigned int bounds = xglFontGetProportionalSize(proportional);
                            int row = proportional >> 5;
                            unsigned int left = bounds & 255;
                            unsigned int right = (bounds >> 8) & 255;
                            if (row >= 2) row = 1 - row;
                            xglFontFlushSub(((proportional & 31) * 20 + left) * 16, row * 384 + 0x2a00,
                                (right - left) * 256 + 0x118, 3);
                        } else {
                            unsigned int glyph = (low + high * 94 - 0xa1) & 0xffff;
                            if (high < 0xa7) glyph = (glyph + 0xc4e2) & 0xffff;
                            else if (high < 0xad) glyph = (glyph + 0xc4be) & 0xffff;
                            else if (high < 0xb0) glyph = (glyph + 0xc30c) & 0xffff;
                            else if (high < 0xd0) glyph = (glyph + 0xc250) & 0xffff;
                            else glyph = (glyph + 0xc240) & 0xffff;
                            low = glyph & 31;
                            high = (glyph >> 5) & 255;
                            xglFontFlushSub(low * 320, (high & 31) * 384, 0x1418, high >> 5);
                        }
                    }
                }
            }
            text++;
            break;
        }
        }
    }
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
static const unsigned char kanaFontCodeTable[96] = {
    0x00, 0x07, 0x11, 0x08, 0x12, 0x09, 0x13, 0x0A, 0x14, 0x0B, 0x15, 0x16,
    0x56, 0x17, 0x57, 0x18, 0x58, 0x19, 0x59, 0x1A, 0x5A, 0x1B, 0x5B, 0x1C,
    0x5C, 0x1D, 0x5D, 0x1E, 0x5E, 0x1F, 0x5F, 0x20, 0x60, 0x21, 0x61, 0x0F,
    0x22, 0x62, 0x23, 0x63, 0x24, 0x64, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A,
    0x6A, 0xEA, 0x2B, 0x6B, 0xEB, 0x2C, 0x6C, 0xEC, 0x2D, 0x6D, 0xED, 0x2E,
    0x6E, 0xEE, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x0C, 0x34, 0x0D, 0x35, 0x0E,
    0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x00, 0x3C, 0x00, 0x00, 0x06, 0x3D,
    0x53, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

int xglFontCheckProportional(unsigned char row, int column)
{
    column &= 0xff;

    if (row == 0xa7) {
        switch (column) {
        case 0xa3:
            return 0x41;
        case 0xa1:
            return 0x40;
        case 0xa6:
            return 0x42;
        default:
            break;
        }
        return -1;
    }
    if (row == 0xa1) {
        if (column == 0xa1) {
            return 0x25;
        }
        if (column == 0xa5) {
            return 0x0a;
        }
    }
    if (row == 0xa3) {
        return tbl_7[(column - 0xa0) >> 5] + (column & 0x1f);
    }
    if (row >= 0xd0) {
        unsigned short glyph = column + row * 94 - 0x4d21;

        row = ~(glyph >> 5) & 7;
        if (row < 2) {
            row ^= 1;
        }
        return (row << 5) + (glyph & 0x1f);
    }
    return -1;
}

unsigned int xglFontGetProportionalSize(int code)
{
    struct FontState *state = &FS.state;
    struct FontImage *font_image = state->font_image;
    unsigned int width;
    signed char average_width;

    width = (unsigned short)(font_image->proportional_widths[code] + 0xff);
    if ((width & 0xff) == 0xff) {
        width = state->default_width << 8;
    }
    if (state->proportional_mode != 0) {
        average_width = ((int)((width & 0xff) + (width >> 8)) >> 1) - 5;
        if (average_width < 0) {
            average_width = 0;
        }
        if (average_width >= 0x0b) {
            average_width = 0x0a;
        }
        width = (((average_width + 9) << 8) + average_width) & 0xffff;
    }
    return width;
}

static unsigned int hex2val(signed char digit)
{
    unsigned int digit_value;

    digit_value = (digit - '0') & 0xff;
    if (digit_value >= 10U) {
        if ((unsigned int)(digit - 'a') < 6U) {
            return (digit - 'W') & 0xff;
        }
        if ((unsigned int)(digit - 'A') < 6U) {
            return (digit - '7') & 0xff;
        }
        return 0U;
    }
    return digit_value;
}

unsigned int xglFontAscii2Euc(signed char character, signed char **text)
{
    unsigned int code = 0xa1a1;

    if (character >= 32) {
        if (character == '#' && text != 0) {
            signed char *cursor = *text;
            signed char next = cursor[1];

            if (next == character) {
                cursor++;
                code = ascii2euc[3];
            } else {
                unsigned int high = hex2val(next);
                unsigned int low = hex2val(cursor[2]);

                cursor += 2;
                code = (high * 16 + low + 32) & 0xffff;

                code = (((code / 94U & 255) << 8) + code % 94U -
                        0x2f5f) & 0xffff;
            }
            *text = cursor;
        } else {
            code = KanjiClutEnv.code_by_character[character];
        }
    }
    return code;
}

void xglFontReloadTexture(struct FontDrawCursor *cursor, int flags)
{
    XglPacket *packet = cursor->packet;
    unsigned int *saved_direct_state;

    cursor->flags |= 0x40;
    if (packet->directCode && (saved_direct_state = packet->directState)) {
        sceVif1PkCloseDirectHLCode(packet);
        if (*saved_direct_state == 0x51000000)
            cursor->packet->write_cursor -= 4;
    }

    flags &= 3;
    if (flags == 1) {
        sceVif1PkRef(cursor->packet, KanjiTex0Env_8, 7, 0, 0, 0);
        sceVif1PkRef(cursor->packet, FS.state.font_image, 15362, 0, 0, 0);
        sceVif1PkRef(cursor->packet, &KanjiClutEnv, 15, 0, 0, 0);
        sceVif1PkRef(cursor->packet, KanjiTex1Env_9, 7, 0, 0, 0);
        sceVif1PkRef(cursor->packet,
                    FS.state.font_image->kanji_texture_1,
                    15362, 0, 0, 0);
    }
    if (flags == 2) {
        struct FontWindowTextureHeader *window = FS.state.window_image;
        unsigned char *window_bytes = FS.state.window_image;
        struct FontWindowTextureEntry *entry =
            (struct FontWindowTextureEntry *)(window_bytes + window->table_offset);
        int subimage = 0;

        while (subimage < window->subimage_count) {
            unsigned int buffer_width = entry->width_and_buffer_width >> 16;
            unsigned int transfer_width = entry->width_and_buffer_width & 0xffff;
            unsigned int gs_block = (entry->gs_offset + 0xf0000) >> 6;

            WinTexEnv_10.bitbltbuf.value =
                ((unsigned long long)gs_block << 32) |
                ((unsigned long long)buffer_width << 48);
            WinTexEnv_10.trxreg.value = transfer_width |
                ((unsigned long long)entry->height_word << 32);
            sceVif1PkCnt(cursor->packet, 0);
            sceVif1PkAddDataN(cursor->packet, &WinTexEnv_10, 24);
            sceVif1PkRef(cursor->packet, window_bytes + entry->data_offset,
                        entry->dma_quadwords, 0, 0, 0);
            entry++;
            subimage++;
        }
    }

    sceVif1PkRef(cursor->packet, TestEnv_11_00490BC0, 6, 0, 0, 0);
    if (FS.state.debug_mode) {
        sceVif1PkRef(cursor->packet, &ModeEnv[LoopCount & 1], 8, 0, 0, 0);
    } else {
        const unsigned int *environment;

        switch (cursor->effect) {
        case 0:
        case 2:
        case 3:
        case 4:
            environment = TestEnv_12_00490C20;
            break;
        case 1:
        default:
            environment = TestEnv_13;
            break;
        }
        sceVif1PkRef(cursor->packet, environment, 6, 0, 0, 0);
    }
    sceVif1PkCnt(cursor->packet, 0);
    sceVif1PkOpenDirectHLCode(cursor->packet, 0);
}

void xglFontFlush(void)
{
    /* The font drawing cursor resides at the base of EE scratchpad RAM. */
    struct FontDrawCursor *cursor = (struct FontDrawCursor *)0x70000000;
    FontQueueSlot *slot;
    int remaining;

    *FS.state.queue_cursor = 0;
    cursor->depth = -1;
    cursor->blue = 127;
    cursor->green = 127;
    cursor->red = 127;
    if (FS.state.debug_mode) {
        cursor->offset_x = 0;
        cursor->offset_y = 0;
    } else {
        cursor->offset_x = 0x700;
        cursor->offset_y = 0x720;
    }
    cursor->flags = 0;
    cursor->draw_state_high = 0;
    cursor->attributes = 255;
    cursor->effect_y = cursor->effect_x = cursor->effect = 1;
    cursor->effect_red = 0;
    cursor->scale_x = 16;
    cursor->effect_green = 0;
    cursor->primitive_flags = 0x44;
    cursor->alpha = 0x80;
    cursor->effect_blue = 0;
    cursor->primitive_mode = 0;
    cursor->scale_y = 16;
    remaining = 15;
    cursor->packet = xglPacketGetCurrent();
    xglFontReloadTexture(cursor, 0x80000001);

    slot = fontPrintQueueBuffer.slots;
    do {
        unsigned short offset = slot->link;
        if (offset) {
            do {
                struct FontQueueRecord *record =
                    (struct FontQueueRecord *)&fontPrintQueueBuffer.bytes[offset];
                unsigned char *text = (unsigned char *)(record + 1);
                offset = record->next_low + (record->next_high << 8);
                xglFontFlushCore(cursor, text);
            } while (offset);
        }
        remaining--;
        slot++;
    } while (remaining >= 0);
    if (FS.state.debug_mode)
        sceVif1PkAddDirectDataN(cursor->packet, zbuf_14, 6);
    sceVif1PkAddDirectDataN(cursor->packet, test_15, 2);
    sceVif1PkCloseDirectHLCode(cursor->packet);
    buffer_reset();
}

int xglFontGetStringWidth2(const char *text, int delimiter)
{
    struct FontState *state;
    int width = 0;
    signed char compact = 0;
    int first = 1;
    unsigned char high;

    state = &FS.state;
    while (*text != 0 &&
           (high = *text,
            !(!first && delimiter > 0 && high == ' ') &&
                !(!first && delimiter == 2 && high == '/') &&
                high != '\n')) {
        unsigned char low;
        int proportional_code;

        if (high < 32) {
            int remaining;
            switch (high) {
            case 21:
                switch (text[2]) {
                case 2:
                    state->default_width = text[3];
                    break;
                case 4:
                    state->proportional_mode = text[3];
                    break;
                }
                break;
            case 24:
                compact = text[1];
                break;
            case 25:
                state->queue_reset_flag = text[1];
                break;
            }
            remaining = xglFontGetSPcodeSize(
                high, (const unsigned char *)text);
            while (remaining >= 0) {
                remaining--;
                text++;
            }
        } else {
            if (high < 0xa0) {
                if ((signed char)high >= 0) {
                    if (!(state->queue_reset_flag & 2)) {
                        width += 16;
                        text++;
                        first = 0;
                        continue;
                    }
                }
            }
            if ((signed char)high >= 0) {
                unsigned int encoded = xglFontAscii2Euc(high, (signed char **)&text);
                low = encoded;
                high = (encoded >> 8) & 0xff;
                text++;
            } else {
                low = text[1];
                text += 2;
            }
            if (compact && (unsigned char)(high + 0x5d) < 3) {
                width += 8;
            } else {
                proportional_code = xglFontCheckProportional(high, low);
                if ((state->queue_reset_flag & 1) && proportional_code >= 0) {
                    unsigned int bounds = xglFontGetProportionalSize(proportional_code);
                    width += ((bounds >> 8) & 0xff) - (bounds & 0xff) + 1;
                } else {
                    width += 20;
                }
            }
        }
        first = 0;
    }
    return width;
}

int xglFontGetStringWidth(const char *text)
{
    return xglFontGetStringWidth2(text, 0);
}

void *xglFontGetLoadAddress(void)
{
    return FS.state.font_image;
}

unsigned short xglFontGetFlags(void)
{
    return FONT_STATE_FLAGS;
}

void xglFontSetFlags(int flags)
{
    FONT_STATE_FLAGS = flags & 0xFFFD;
}

unsigned char xglFontLoad(int font_index, int load_flags)
{
    unsigned char previous_font;
    int read_mode;

    previous_font = FS.state.font_selection;
    FS.state.font_selection = (unsigned char)font_index;
    read_mode = 1;
    if (load_flags == 0) {
        /*
         * Both fonts take the same synchronous defaults, but the original
         * tests the font here too: the inner branch ends the extended block,
         * so each read below re-addresses FS (lw %lo(FS)) as it does.
         */
        if (font_index != 0) {
            read_mode = 0;
            load_flags = 1;
        } else {
            read_mode = 0;
            load_flags = 1;
        }
    }
    if (font_index == 0) {
        xglCdReadFile(fontTexture0Path, FS.state.font_image, read_mode, load_flags);
    } else {
        xglCdReadFile(fontTexture1Path, FS.state.font_image, read_mode, load_flags);
    }
    return previous_font;
}

void xglFontInitial(void)
{
    FS.state.font_selection = 0xFF;
    FS.state.font_image = &FontImage.image;
    xglFontLoad(1, 0);
    FS.state.window_image = WindowImage;
    WindowTexLoad(WindowImage, 0);
    buffer_reset();
    FS.state.flags = 9;
    FS.state.line_height = 8;
    FS.state.default_width = 4;
    FS.state.debug_mode = 0;
    FS.state.proportional_mode = 0;
    FS.state.horizontal_shift = 0;
    FS.state.character_spacing = 0;
}

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
unsigned char spCodeParameterSizeTable[8] = { 2, 2, 3, 2, 2, 0, 0, 0 };

static signed char tbl_1_004DC2A8[4][2] = { { 0, -1 }, { -1, 0 }, { 1, 0 }, { 0, 1 } };
