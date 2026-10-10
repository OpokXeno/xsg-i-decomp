#include "common.h"

struct FontImage {
    unsigned char unmodeled_00000[0x78040];
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

static FontStateStorage FS;

#define D_00881188 (FS.state.flags)

static void set_xyz(int x, int y, int color);

static void xglFontPrintSub(void *args);

static void xglFontPrintDirectCore(const char *text);

/* Prints text straight away into ordering-table slot ot (passed to set_ot). */

extern void xglFontPrintDirectOT(int ot, const char *text);

static void set_ot(int ot);

/*
 * Each 4-byte slot of the font print-queue buffer at D_008811A0 (16 slots,
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

static FontPrintQueueStorage D_008811A0;

#define FONT_PRINT_QUEUE (D_008811A0.slots)

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

/*
 * Draws one glyph from the font texture: u and v are texel coordinates in
 * 1/16 units, size packs the glyph width (high byte) and height (low byte),
 * flags' low nibble selects the draw attributes and bit 4 can double the
 * drawn size.
 */

static void xglFontFlushSub(int u, int v, int size, int flags);

/* Hex digit glyphs are 6x8 texels, 8 texels apart at v 0x2F00 (row 752). */

extern int xglFontGetStringWidth2(const char *text, int);

typedef union FontImageStorage {
    struct FontImage image;
    unsigned char bytes[0x78800];
} FontImageStorage;

static FontImageStorage FontImage;

typedef struct FontState FontState;

#include "main/xgl_cd.h"

#include "main/xgl_packet.h"

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

struct FontEffectScratchPacket {
    unsigned long long vif_tag;
    unsigned long long vif_count;
    unsigned long long effect_state;
    unsigned long long payload_count;
};

struct FontDrawCursor {
    XglPacket *packet;
    unsigned short origin_x;
    unsigned char unmodeled_06[2];
    unsigned short x;
    unsigned short y;
    int depth;
    unsigned short offset_x;
    unsigned short offset_y;
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char unmodeled_17;
    unsigned char flags;
    unsigned char effect;
    unsigned char attributes;
    unsigned char draw_state_high;
    unsigned char unmodeled_1c[4];
    signed char effect_x;
    signed char effect_y;
    unsigned char unmodeled_22[2];
    unsigned char effect_red;
    unsigned char effect_green;
    unsigned char effect_blue;
    unsigned char unmodeled_27;
    unsigned char primitive_flags;
    unsigned char primitive_mode;
    unsigned char alpha;
    unsigned char unmodeled_2b;
    unsigned short scale_x;
    unsigned short scale_y;
    struct FontEffectScratchPacket effect_packet;
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

#define FONT_ARGUMENT_START(list, last) \
    ((list) = (unsigned long long *)((char *)__builtin_next_arg(last) - \
              (8 - __builtin_args_info(2)) * 8))

extern unsigned short ascii2euc[96];

struct KanjiClutEnvironment {
    unsigned char unmodeled_00[0xb0];
    unsigned short code_by_character[128];
};

extern struct KanjiClutEnvironment KanjiClutEnv;

static const char D_004D2448[16] = "data\\font0.tex";

static const char D_004D2458[16] = "data\\font1.tex";

extern void xglFontReloadTexture(struct FontDrawCursor *cursor, int flags);

extern int xglFontCheckProportional(unsigned int high, unsigned int low);

extern unsigned int xglFontGetProportionalSize(int code);

extern unsigned int xglFontAscii2Euc(signed char character, signed char **text);

static void xglFontFlushCore(struct FontDrawCursor *cursor, const unsigned char *text);

extern unsigned char WindowImage[];

extern int WindowTexLoad(unsigned char *buffer, unsigned int request);

extern const unsigned char D_004DC2A0[];

extern unsigned char spcode_0[32];

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontDebugMode);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontGetKanjiClutUV);

static void set_ot(int ot)
{
    FontState *state = (FontState *)(D_008811A0.bytes - sizeof(FontState));
    FontQueueSlot *slot = &D_008811A0.slots[((ot >> 22) & 0x3c) / 4];
    unsigned char *queue_bytes = (unsigned char *)&D_008811A0;
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

            __builtin_memcpy(parameter_sizes, D_004DC2A0,
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

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontPrintSub);

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
    if (D_00881188 & 1 & 0xFFFF) {
        set_xyz(x, y, color);
        xglFontPrintSub(arg);
    }
}

void xglFontPrint(int x, int y, int color, const char *text)
{
    if (D_00881188 & 1 & 0xFFFF) {
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
    if (D_00881188 & 1 & 0xFFFF) {
        set_ot(ot);
        xglFontPrintDirectCore(text);
    }
}

void xglFontPrintExtFunc(int ot, unsigned int function_address, unsigned int argument)
{
    struct FontState *state = &FS.state;
    struct FontExtCommand *command;

    if (state->flags & 1 & 0xffff) {
        set_ot(ot);
        command = (struct FontExtCommand *)state->queue_cursor;
        command->opcode = 23;
        command->function_high = function_address >> 16;
        command->function_middle = function_address >> 8;
        command->function_low = function_address;
        command->argument_high = argument >> 24;
        command->argument_mid_high = argument >> 16;
        command->argument_mid_low = argument >> 8;
        command->argument_low = argument;
        command->terminator = 0;
        state->queue_cursor += sizeof(*command);
    }
}

void xglFontDebugPrintf(int x, int y, const char *format, ...)
{
    unsigned long long arguments[32];
    unsigned long long *output;
    unsigned long long *list;

    if ((D_00881188 & 3) == 3) {
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

    state = (struct FontState *)(D_008811A0.bytes - sizeof(struct FontState));
    state->queue_cursor = (unsigned char *)slot;
    state->queue_reset_flag = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontFlushSub);

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

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontFlushCore);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontCheckProportional);

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

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontReloadTexture);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontFlush);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontGetStringWidth2);

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
    return D_00881188;
}

void xglFontSetFlags(int flags)
{
    D_00881188 = flags & 0xFFFD;
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
        xglCdReadFile(D_004D2448, FS.state.font_image, read_mode, load_flags);
    } else {
        xglCdReadFile(D_004D2458, FS.state.font_image, read_mode, load_flags);
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
