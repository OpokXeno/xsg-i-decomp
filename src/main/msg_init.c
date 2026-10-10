#include "common.h"

typedef struct Mif2Argument {
    int type;
    union {
        int number;
        const char *text;
    } value;
} Mif2Argument;

static int ctrlCodeFlags = 0;

/*
 * Placeholder entry of the `mfunc` control-code table (listed twice there).
 * MSG_convert calls every entry as handler(out, argc, args) and continues
 * writing at the returned pointer; the other MIF2_* handlers emit their
 * control bytes at `out` and return the end. This one emits nothing: it only
 * walks the argument count and hands `out` back unchanged.
 */

int strcmp(const char *, const char *);

/*
 * One row of the mfunc[] control-code table, which a NULL name terminates.
 * MSG_convert (still asm) calls the matched row's handler as
 * handler(out, argc, args), including the placeholder MIF2_dummy row. That
 * handler ignores args and returns out unchanged.
 */

typedef struct MsgFuncEntry {
    char *(*handler)(char *out, int argc, const Mif2Argument *args);
    const char *name;
} MsgFuncEntry;

extern char *MIF2_clear(char *out, int argc, const Mif2Argument *args);

extern char *MIF2_waitkey(char *out, int argc, const Mif2Argument *args);

extern char *MIF2_wait(char *out, int argc, const Mif2Argument *args);

extern char *MIF2_color(char *out, int argc, const Mif2Argument *args);

extern char *MIF2_close(char *out, int argc, const Mif2Argument *args);

extern char *MIF2_ruby(char *out, int argc, const Mif2Argument *args);

extern char *MIF2_font(char *out, int argc, const Mif2Argument *args);

extern char *MIF2_label(char *out, int argc, const Mif2Argument *args);

char *MIF2_dummy(char *out, int argc, const Mif2Argument *args);
char *MIF2_gaiji(char *out, int argc, const Mif2Argument *args);
char *MIF2_code(char *out, int argc, const Mif2Argument *args);

const char D_004DA410[8] = "clear";

const char D_004DA408[8] = "waitkey";

const char D_004DA400[8] = "signal";

const char D_004DA3F8[8] = "wait";

const char D_004DA3F0[8] = "color";

const char D_004DA3E8[8] = "close";

const char D_004DA3E0[8] = "tips";

const char D_004DA3D8[8] = "ruby";

const char D_004DA3D0[8] = "font";

const char D_004DA3C8[8] = "label";

const char D_004DA3C0[8] = "gaiji";

const char D_004DA3B8[8] = "code";

static MsgFuncEntry mfunc[] = {
    {MIF2_clear, D_004DA410},
    {MIF2_waitkey, D_004DA408},
    {MIF2_dummy, D_004DA400},
    {MIF2_wait, D_004DA3F8},
    {MIF2_color, D_004DA3F0},
    {MIF2_close, D_004DA3E8},
    {MIF2_dummy, D_004DA3E0},
    {MIF2_ruby, D_004DA3D8},
    {MIF2_font, D_004DA3D0},
    {MIF2_label, D_004DA3C8},
    {MIF2_gaiji, D_004DA3C0},
    {MIF2_code, D_004DA3B8},
    {0, 0},
};

/*
 * Looks up name in the mfunc[] control-code table and returns the matching
 * row, or NULL if the table is empty or no row's name matches.
 */

/*
 * Each message buffer has a 16-byte header followed by its 0x800-byte
 * workspace. MBUF_create2 sets the active flag and capacity, and points the
 * cursor at the workspace. MBUF_create also uses the header's table pointer
 * for the per-entry pointers stored inside that workspace.
 */

typedef struct MsgBuffer {
    int active;
    unsigned int capacity;
    unsigned char **entry_table;
    unsigned char *cursor;
    unsigned char workspace[0x800];
} MsgBuffer;

MsgBuffer msg_buffer[8];

/*
 * MBUF_init/MBUF_create (not yet recovered) establish the rest of the mbuf
 * layout; only the leading word is evidenced here.
 */

extern int PARSE_int(const char *str, int len, int base);

/*
 * Message queue state: the text lives in buffer, read from head up to tail.
 * width is the pixel width of the current line, count the number of glyphs
 * handed out so far.
 */

typedef struct MsgQueue {
    unsigned char *buffer;
    unsigned char unmodeled_04[4];
    short head;
    short tail;
    short size;
    short count;
    short width;
    short last;
    short limit;
    unsigned char unmodeled_22[6];
    int label;
} MsgQueue;

struct MsgPrintContext {
    unsigned char unmodeled_00[0x16];
    unsigned short render_mode;
    unsigned char unmodeled_18[0x94];
    MsgQueue queue;
};

unsigned int MSG_convert(unsigned char *output, int capacity, const char *text, int length);

int MSG_queuePush(MsgQueue *queue, const unsigned char *text, int capacity, int length);


typedef struct RubyFontContext {
    unsigned char unmodeled_00[8];
    short x;
    short y;
    unsigned char unmodeled_0c[12];
    unsigned char flags;
} RubyFontContext;

static int rubyX1;

static int rubyX2;

static int rubyY;

#define MIF2_FONT_ESCAPE_BYTE 0x19u

#define MIF2_FONT_BEGIN_BYTE 0x01u

#define MIF2_FONT_END_BYTE 0x00u

#define MIF2_COLOR_BYTE 0x0cu

#define MIF2_LABEL_BYTE 0x85u

#define MIF2_CLEAR_BYTE 0x81u

#define MIF2_CLOSE_BYTE 0x84u

#define MIF2_WAITKEY_BYTE 0x80u

#define MIF2_WAIT_BYTE 0x83u

#define MSG_QUEUE_RESET_CONTROL_CODE_FLAG 0x100u

#define MSG_QUEUE_RESET_MODE_MASK 0xffu

#define MSG_QUEUE_RESET_CLEAR_ALL 0u

#define MSG_QUEUE_RESET_CLEAR_COUNT_AND_WIDTH 10u

#define MSG_CONTROL_CODE_FLAG_ENABLED 1u

void MSG_init(void)
{
    ctrlCodeFlags = 0;
}

void MSG_print2(void *window, const char *text, int length)
{
    struct MsgPrintContext *context = window;
    unsigned char message[0x400];

    if (context->render_mode == 4) {
        message[0] = MIF2_FONT_ESCAPE_BYTE;
        message[1] = MIF2_FONT_BEGIN_BYTE;
        MSG_convert(&message[2], 0x3FE, text, length);
    } else {
        MSG_convert(message, 0x400, text, length);
    }
    MSG_queuePush(&context->queue, message, 0x400, -1);
}

void MSG_send(void)
{
}

int MSG_getSize(const unsigned char *message)
{
    int code;
    const unsigned char *cursor = message;

    for (;;) {
        code = *cursor++;
        if (code == '/' && *cursor == '[') {
            const unsigned char *bracketCursor = cursor;
            int bracketCode;

            do {
                bracketCode = *bracketCursor++;
                if (bracketCode < 0x20) {
                    switch (bracketCode) {
                    case 0x01:
                    case 0x02:
                    case 0x03:
                    case 0x04:
                    case 0x05:
                    case 0x0d:
                    case 0x18:
                    case 0x19:
                        bracketCursor++;
                        break;
                    case 0x07:
                    case 0x09:
                    case 0x0a:
                    case 0x0b:
                    case 0x10:
                    case 0x11:
                    case 0x12:
                    case 0x13:
                    case 0x14:
                    case 0x15:
                    case 0x16:
                        break;
                    case 0x08:
                        bracketCode = *bracketCursor++;
                        if (bracketCode & 1) {
                            bracketCursor += 2;
                        }
                        if (bracketCode & 2) {
                            bracketCursor += 2;
                        }
                        if (!(bracketCode & 4)) {
                            break;
                        }
                        /* The optional color has the same three-byte extent. */
                    case 0x06:
                    case 0x0c:
                    case 0x0f:
                        bracketCursor += 3;
                        break;
                    case 0x0e:
                        bracketCursor += 5;
                        break;
                    case 0x17:
                        bracketCursor += 7;
                        break;
                    default:
                        break;
                    }
                } else if (bracketCode > 0x8d) {
                    bracketCursor++;
                } else if (bracketCode == ']') {
                    break;
                }
            } while (1);
            cursor = bracketCursor;
            continue;
        }

        if (code == 0x0a) {
            continue;
        }
        if (code == 0) {
            break;
        }
        if (code < 0x20) {
            switch (code) {
            case 0x01:
            case 0x02:
            case 0x03:
            case 0x04:
            case 0x05:
            case 0x0d:
            case 0x18:
            case 0x19:
                cursor++;
                break;
            case 0x07:
            case 0x09:
            case 0x0b:
            case 0x10:
            case 0x11:
            case 0x12:
            case 0x13:
            case 0x14:
            case 0x15:
            case 0x16:
                break;
            case 0x08:
                code = *cursor++;
                if (code & 1) {
                    cursor += 2;
                }
                if (code & 2) {
                    cursor += 2;
                }
                if (!(code & 4)) {
                    break;
                }
                /* The optional color has the same three-byte extent. */
            case 0x06:
            case 0x0c:
            case 0x0f:
                cursor += 3;
                break;
            case 0x0e:
                cursor += 5;
                break;
            case 0x17:
                cursor += 7;
                break;
            default:
                break;
            }
        } else if (code >= 0x8e) {
            cursor++;
        }
    }
    return cursor - message - 1;
}

void MSG_dump(const unsigned char *data, int size)
{
    char line[16];
    int ch;
    int row;
    int col;

    for (row = 0; row < size / 16; row++) {
        for (col = 0; col < 16; col++) {
            ch = data[col];
            if (ch < 32) {
                line[col] = '.';
            } else {
                line[col] = ch;
            }
        }
        data += 16;
    }
}

void MSG_queueReset(MsgQueue *queue, unsigned int flags)
{
    if (flags & MSG_QUEUE_RESET_CONTROL_CODE_FLAG) {
        ctrlCodeFlags |= MSG_CONTROL_CODE_FLAG_ENABLED;
    } else {
        ctrlCodeFlags = 0;
    }
    flags &= MSG_QUEUE_RESET_MODE_MASK;
    if (flags == MSG_QUEUE_RESET_CLEAR_ALL) {
        queue->head = 0;
        queue->tail = 0;
        queue->count = 0;
        queue->width = 0;
    } else if (flags == MSG_QUEUE_RESET_CLEAR_COUNT_AND_WIDTH) {
        queue->count = 0;
        queue->width = 0;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/msg_init", MSG_queueGetInfo);

INCLUDE_ASM("asm/main/nonmatchings/msg_init", MSG_queuePop);

INCLUDE_ASM("asm/main/nonmatchings/msg_init", MSG_copyln);

INCLUDE_ASM("asm/main/nonmatchings/msg_init", MSG_queuePush);

char *matchBracket(const unsigned char *cursor)
{
    unsigned char open = *cursor++;
    unsigned char close;
    unsigned char ch;
    int depth = 1;

    switch (open) {
    case '[':
        close = ']';
        break;
    case '(':
        close = ')';
        break;
    case '{':
        close = '}';
        break;
    default:
        return 0;
    }
    while ((ch = *cursor++) != 0) {
        if (ch == open) {
            depth++;
        } else if (ch == close) {
            depth--;
            if (depth <= 0) {
                break;
            }
        }
    }
    cursor--;
    return *cursor == close ? (char *)cursor : 0;
}

char *MIF2_dummy(char *out, int argc, const Mif2Argument *args)
{
    int i;

    for (i = 0; i < argc; i++) {
    }
    return out;
}

void MSG_setArgs(Mif2Argument *args, int argc, const char **strs)
{
    const unsigned char *str;
    unsigned char ch;
    unsigned char c1;
    int i;

    for (i = 0; i < argc; i++) {
        str = (const unsigned char *)strs[i];
        ch = str[0];
        c1 = str[1];
        if (ch == '$') {
            continue;
        }
        if ((unsigned)(ch - '0') < 10 || (ch == '-' && (unsigned)(c1 - '0') < 10)) {
            args[i].type = 0;
            if (ch == '0' && c1 == 'x') {
                args[i].value.number = PARSE_int((const char *)str + 2, -1, 16);
            } else {
                args[i].value.number = PARSE_int((const char *)str, -1, 10);
            }
        } else {
            args[i].value.text = (const char *)str;
            args[i].type = 1;
        }
    }
}

static void rubyFont(RubyFontContext *context, unsigned int command)
{
    unsigned int mode = command & 0xf0000000;

    context->flags |= 0x40;
    switch (mode) {
    case 0:
        rubyX1 = context->x;
        rubyY = context->y;
        return;
    case 0x10000000: {
        int x = context->x;
        int delta = x - rubyX1;

        rubyX2 = x;
        context->y -= 4;
        context->x -= (short)(delta / 2 + (short)((command & 0xff) << 2));
        return;
    }
    case 0x20000000:
        context->x = (short)rubyX2;
        context->y += 4;
        break;
    default:
        return;
    }
    return;
}

INCLUDE_ASM("asm/main/nonmatchings/msg_init", MIF2_ruby);

char *MIF2_font(char *out, int argc, const Mif2Argument *args)
{
    const unsigned char *text = (const unsigned char *)args[0].value.text;
    unsigned char ch;

    *out++ = (char)(unsigned char)MIF2_FONT_ESCAPE_BYTE;
    *out++ = (char)(unsigned char)MIF2_FONT_BEGIN_BYTE;
    while ((ch = *text++) != '/' && ch != 0) {
        *out++ = ch;
    }
    *out++ = (char)(unsigned char)MIF2_FONT_ESCAPE_BYTE;
    *out++ = (char)(unsigned char)MIF2_FONT_END_BYTE;
    *out = 0;
    return out;
}

char *MIF2_color(char *out, int argc, const Mif2Argument *args)
{
    char *end;
    unsigned int rgb = args[0].value.number;

    out[0] = (char)(unsigned char)MIF2_COLOR_BYTE;
    out[1] = rgb >> 16;
    out[2] = rgb >> 8;
    out[3] = rgb;
    end = &out[4];
    *end = '\0';
    return end;
}

char *MIF2_label(char *out, int argc, const Mif2Argument *args)
{
    const unsigned char *text = (const unsigned char *)args[0].value.text;
    unsigned char ch;

    *out++ = '/';
    *out++ = '[';
    *out++ = (char)(unsigned char)MIF2_LABEL_BYTE;
    while ((ch = (*out++ = *text++)) != 0) {
    }
    *out++ = ']';
    *out = 0;
    return out;
}

char *MIF2_clear(char *out, int argc, const Mif2Argument *args)
{
    char *end;

    out[0] = '/';
    out[1] = '[';
    out[2] = (char)(unsigned char)MIF2_CLEAR_BYTE;
    out[3] = ']';
    end = &out[4];
    *end = '\0';
    return end;
}

char *MIF2_close(char *out, int argc, const Mif2Argument *args)
{
    char *end;

    out[0] = '/';
    out[1] = '[';
    out[2] = (char)(unsigned char)MIF2_CLOSE_BYTE;
    out[3] = ']';
    end = &out[4];
    *end = '\0';
    return end;
}

char *MIF2_waitkey(char *out, int argc, const Mif2Argument *args)
{
    char *end;
    int key = args[0].value.number;

    out[0] = '/';
    out[1] = '[';
    out[2] = (char)(unsigned char)MIF2_WAITKEY_BYTE;
    out[3] = key;
    out[4] = ']';
    end = &out[5];
    *end = '\0';
    return end;
}

char *MIF2_wait(char *out, int argc, const Mif2Argument *args)
{
    char *end;
    int frames = args[0].value.number;

    out[0] = '/';
    out[1] = '[';
    out[2] = (char)(unsigned char)MIF2_WAIT_BYTE;
    out[3] = frames;
    out[4] = ']';
    end = &out[5];
    *end = '\0';
    return end;
}

char *MIF2_gaiji(char *out, int argc, const Mif2Argument *args)
{
    int glyph = args[0].value.number + 160;
    char *end = &out[2];

    out[0] = -83;
    out[1] = glyph;
    *end = '\0';
    return end;
}

char *MIF2_code(char *out, int argc, const Mif2Argument *args)
{
    char *end = &out[1];
    unsigned char code = args[0].value.number;

    out[0] = code;
    end[0] = '\0';
    return end;
}

static MsgFuncEntry *findMsgFunc(const char *name)
{
    MsgFuncEntry *entry = mfunc;

    if (entry->name != 0) {
        do {
            if (strcmp(name, entry->name) == 0) {
                return entry;
            }
            entry++;
        } while (entry->name != 0);
    }
    return 0;
}

INCLUDE_ASM("asm/main/nonmatchings/msg_init", MSG_convert);

void MBUF_init(void)
{
    int slot;

    slot = 7;
    do {
        msg_buffer[slot].active = 0;
        slot--;
    } while (slot >= 0);
}

MsgBuffer *MBUF_create2(void)
{
    int slot = -1;
    int i;
    MsgBuffer *buffer;

    for (i = 0; i < 8; i++) {
        buffer = &msg_buffer[i];
        if (!(buffer->active & 1)) {
            slot = i;
            break;
        }
    }
    if (slot < 0) {
        return 0;
    }
    buffer = &msg_buffer[slot];
    buffer->active = 1;
    buffer->capacity = 2048;
    buffer->cursor = buffer->workspace;
    return buffer;
}

INCLUDE_ASM("asm/main/nonmatchings/msg_init", MBUF_create);

void MBUF_dispose(int *mbuf)
{
    mbuf[0] = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/msg_init", _copyCTRLCode);

INCLUDE_ASM("asm/main/nonmatchings/msg_init", _skipCTRLCode);
