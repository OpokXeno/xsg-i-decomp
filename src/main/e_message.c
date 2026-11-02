#include "common.h"
#include "main/e_message.h"
#include "main/end_print.h"
#include "main/xgl_font.h"

typedef struct EMessageSprite {
    short x, y;
    int draw_parameter;
    short width, height;
    unsigned char color[4];
    short sprite_id;
    unsigned char unmodeled_12;
    unsigned char colorize;
} EMessageSprite;

struct EMessageColor { unsigned char channel[4]; };

union EMessageWrapSlot {
    const char *position;
    int line;
};

struct GlyphWidthText {
    char command;
    char style;
    char character[2];
    char terminator;
};

enum EMessageFontWidthMode {
    EMESSAGE_FONT_WIDTH_FIXED = 0,
    EMESSAGE_FONT_WIDTH_PROPORTIONAL = 3
};

const char *eMessageNextWaitKeySearch(const char *text);

const char *eMessageNextGyou(const char *text);

void endSpriteSet(void *sprite, int attributes);
/* Number of message sprites currently allocated by the print routines. */
static int msg_spr_count = 0;

/* Font command defaults copied by eMessageDrawType01. */
static unsigned char otb[28] = {
    11,
    12, 128, 128, 128,      /* RGB channels, scaled by message color. */
    13, 2,
    14, 2, 2, 0, 0, 0,
    15, 68, 0, 127,        /* The last parameter is scaled by message alpha. */
    24, 0,
    25, 3,                /* Proportional font width mode. */
    21, 2, 1, 12,
    0, 0, 0
};

/* Current write position of the message text being built. */
static char *MessageCpyEnd;

/* Font command/text output buffer, ending where msg_spr begins. */
static unsigned char moji[4096];

static EMessageSprite msg_spr[16];

static inline int eMessageScaleChannel(unsigned char component,
                                      unsigned char strength)
{
    return (int)((float)component * ((float)strength * 0.0078125f));
}

static inline int eMessageIsFontCommand(unsigned int code)
{
    return code >= 1 && code <= 25;
}

void eMessageSpriteReset(void)
{
    msg_spr_count = 0;
}

void eMessageDrawType00(void)
{
}

int eMessageHalfSpaseCheck(const unsigned char *text)
{
    unsigned char character = *text++;
    int half_spaces = 0;

    while ((signed char)character == ' ') {
        character = *text++;
        half_spaces++;
    }
    return half_spaces;
}

const char *eMessageNextGyou(const char *text)
{
    int character = (unsigned char)*text;
    int half_spaces;

    while (*text != 0 && *text != '\n') {
        if ((unsigned int)(character - 1) < 25) {
            text += xglFontGetSPcodeSize((unsigned char)*text,
                                        (const unsigned char *)text) + 1;
        } else {
            switch ((signed char)character) {
            case 30:
                text += 2;
                break;
            default:
                if ((signed char)character == 31 ||
                    (character >= 32 && character < 127)) {
                    text++;
                } else {
                    half_spaces = eMessageHalfSpaseCheck((const unsigned char *)text);
                    text = half_spaces ? text + half_spaces : text + 2;
                }
                break;
            }
        }
        character = *text == 0 ? 0 : (*text & 255);
    }
    return text;
}

int eMessageNextGyouMaxGet(const char *text)
{
    int line_count = 0;
    const char *cursor = text;

    for (;;) {
        char character;

        line_count++;
        cursor = eMessageNextGyou(cursor);
        character = *cursor;
        if (character == '\0') {
            break;
        }
        if (character == 31) {
            break;
        }
        if (character == '\n') {
            cursor++;
        }
    }
    return line_count;
}

const char *eMessageNextWaitKeySearch(const char *text)
{
    signed char sentinel = *text;
    int character = sentinel == 0 ? 0 : (*text & 255);
    int half_spaces;

    while (sentinel != 0) {
        const char *command = text;

        if (sentinel == 31) {
            text++;
            break;
        }
        if (eMessageIsFontCommand(character)) {
            unsigned char command_byte = (unsigned char)*command;
            text = command + xglFontGetSPcodeSize(command_byte,
                                                  (const unsigned char *)command) + 1;
        } else {
            switch ((signed char)character) {
            case 30:
                text = command + 2;
                break;
            default:
                if (character >= 32 && character < 127) {
                    text = command + 1;
                } else {
                    half_spaces = eMessageHalfSpaseCheck((const unsigned char *)command);
                    text = half_spaces ? command + half_spaces : command + 2;
                }
                break;
            }
        }
        {
            int next_byte = (unsigned char)*text;
            character = next_byte;
            sentinel = (signed char)next_byte;
        }
    }
    return text;
}

/* Partial sprite record: byte 18 is not interpreted by this TU. */
void eMessageDrawType01(EMessageParam *message)
{
    struct EMessageColor color;
    struct EMessageColor initial_color;
    union EMessageWrapSlot wraps[32];
    int max_width;
    unsigned char width_high;
    unsigned char width_low;
    unsigned char *output = moji;
    struct GlyphWidthText glyph;
    char *glyph_end = &glyph.terminator;
    const unsigned int flags = message->flags;
    const unsigned char header_enabled = flags & 1;
    int line_timing;
    unsigned char incomplete;
    int font_style;
    int x_offset;
    int y_offset;
    int waiting;
    short remaining;
    const char *text;
    const char *scan;
    const char *word_start;
    int line_width;
    union EMessageWrapSlot *wrap_cursor;
    int i;
    int code;
    int sentinel;
    int size;

    initial_color.channel[0] = message->color_r;
    initial_color.channel[1] = message->color_g;
    initial_color.channel[2] = message->color_b;
    initial_color.channel[3] = message->color_a;
    max_width = message->wrap_width_half * 2;
    width_high = max_width / 256;
    width_low = max_width % 256;
    text = message->text;
    incomplete = 0;
    font_style = EMESSAGE_FONT_WIDTH_PROPORTIONAL;
    x_offset = 0;
    y_offset = 0;
    color = initial_color;
    wraps[0].position = 0;
    waiting = 0;
    remaining = message->frame;
    if (header_enabled) {
        *output++ = otb[0];
        *output++ = otb[21];
        *output++ = otb[22];
        *output++ = otb[23];
        *output++ = otb[24];
        *output++ = otb[1];
        *output++ = eMessageScaleChannel(otb[2], message->color_r);
        *output++ = eMessageScaleChannel(otb[3], message->color_g);
        *output++ = eMessageScaleChannel(otb[4], message->color_b);
        *output++ = otb[5];
        *output++ = otb[6];
        *output++ = otb[7];
        *output++ = otb[8];
        *output++ = otb[9];
        *output++ = otb[10];
        *output++ = otb[11];
        *output++ = otb[12];
        *output++ = otb[13];
        *output++ = otb[14];
        *output++ = otb[15];
        *output++ = eMessageScaleChannel(otb[16], message->color_a);
        *output++ = otb[17];
        *output++ = otb[18];
        *output++ = otb[19];
        *output++ = otb[20];
        *output++ = 21;
        *output++ = 2;
        *output++ = 2;
        *output++ = 4;
        *output++ = 21;
        *output++ = font_style;
        *output++ = font_style;
        *output++ = width_high;
        *output++ = width_low;
    }
    i = 0;
    while (i < message->page_index) {
        text = eMessageNextWaitKeySearch(text);
        i++;
    }
    wrap_cursor = wraps;
    scan = text;
    line_width = 0;
    word_start = text;
    while (*scan != 0) {
        code = *scan & 255;
        if (eMessageIsFontCommand(code)) {
            if ((signed char)code == 10) {
                line_width = 0;
            }
            scan += xglFontGetSPcodeSize((unsigned char)*scan,
                                       (const unsigned char *)scan) + 1;
        } else if ((signed char)code == 30) {
            scan += 2;
        } else if ((signed char)code == 31) {
            break;
        } else if ((signed char)code == 32) {
            line_width += 4;
            scan++;
            if (max_width < line_width) {
                (wrap_cursor++)->position = word_start;
                scan = word_start;
                line_width = 0;
                if (wrap_cursor > &wraps[31]) {
                    break;
                }
            } else {
                word_start = scan;
            }
        } else {
            glyph.command = 25;
            glyph.style = EMESSAGE_FONT_WIDTH_PROPORTIONAL;
            glyph.character[0] = *scan;
            if ((unsigned int)((unsigned char)scan[1] - 1) < 32 ||
                (unsigned int)((unsigned char)*scan - 32) < 96) {
                glyph.character[1] = 0;
                scan++;
            } else {
                glyph.character[1] = scan[1];
                scan++;
                if (*scan != 0) {
                    scan++;
                }
            }
            *glyph_end = 0;
            line_width += xglFontGetStringWidth((const char *)&glyph);
        }
    }
    wrap_cursor = wraps;
    code = *text & 255;
    if (code != 0) {
        line_timing = flags & 32;
        do {
            /* Exclude an exhausted list before forming its last-entry pointer. */
            /* The original fixed bound can inspect unwritten wrap slots. */
            if (wrap_cursor <= &wraps[31] && text == wrap_cursor->position) {
                wrap_cursor++;
                x_offset = 0;
                y_offset += 24;
            }
            if (eMessageIsFontCommand(code)) {
                int special_code = (signed char)code;
                size = xglFontGetSPcodeSize((unsigned char)*text,
                                          (const unsigned char *)text) + 1;
                switch (special_code) {
                case 12:
                    *output++ = *text++;
                    output[0] = eMessageScaleChannel((unsigned char)*text++,
                                                    message->color_r);
                    output[1] = eMessageScaleChannel((unsigned char)*text++,
                                                    message->color_g);
                    output[2] = eMessageScaleChannel((unsigned char)*text++,
                                                    message->color_b);
                    color.channel[0] = *output++;
                    color.channel[1] = *output++;
                    color.channel[2] = *output++;
                    break;
                case 15:
                    *output++ = *text++;
                    *output++ = *text++;
                    *output++ = *text++;
                    color.channel[3] = *output++ =
                        eMessageScaleChannel((unsigned char)*text++, message->color_a);
                    break;
                case 10:
                    if (line_timing) {
                        remaining = message->frame;
                    }
                    *output++ = *text++;
                    x_offset = 0;
                    y_offset += 24;
                    break;
                case 25: {
                    int style_code;
                    *output++ = *text++;
                    code = (unsigned char)*text;
                    style_code = code;
                    code = (signed char)code;
                    font_style = code == 0 || code == 2 ?
                        EMESSAGE_FONT_WIDTH_FIXED : EMESSAGE_FONT_WIDTH_PROPORTIONAL;
                    *output++ = style_code;
                    text++;
                    break;
                }
                default:
                    for (size--; size != -1; size--) {
                        *output++ = *text++;
                    }
                    break;
                }
            } else if ((signed char)code == 30) {
                text++;
                if (msg_spr_count < 16) {
                    unsigned short icon_ids[4] = {513, 512, 514, 515};
                    unsigned char icon_text[3] = {161, 161, 0};
                    int next_sprite_count;
                    EMessageSprite *sprite = &msg_spr[msg_spr_count];
                    int icon = *text;
                    if (icon < 4) {
                        sprite->sprite_id = icon_ids[(unsigned char)*text & 3];
                    } else if ((unsigned char)(*text - 4) < 23) {
                        sprite->sprite_id = icon + 0x14FC;
                    } else if ((unsigned char)(*text - 27) < 4) {
                        sprite->sprite_id = icon + 0x15E5;
                    } else if ((unsigned char)(*text - 31) < 4) {
                        sprite->sprite_id = icon + 0x15E5;
                    } else if ((unsigned char)(*text - 35) < 23) {
                        sprite->sprite_id = icon + 0x7DD;
                    } else if ((unsigned char)(*text - 58) < 20) {
                        sprite->sprite_id = icon + 0x16C6;
                    } else {
                        sprite->sprite_id = 0;
                    }
                    endSpriteSet(sprite, 255);
                    sprite->x = message->x + (message->text_x_offset + x_offset) - sprite->width / 2 + 10;
                    sprite->y = message->y + (message->text_y_offset + y_offset) - sprite->height / 2 + 12;
                    sprite->draw_parameter = message->draw_parameter;
                    if (*text < 4) {
                        if (color.channel[0] != 128 || color.channel[1] != color.channel[0] ||
                            color.channel[2] != color.channel[1]) {
                            sprite->colorize = 1;
                        }
                        sprite->color[3] = color.channel[3];
                    } else {
                        sprite->color[0] = color.channel[0];
                        sprite->color[1] = color.channel[1];
                        sprite->color[2] = color.channel[2];
                        sprite->color[3] = color.channel[3];
                    }
                    x_offset += 20;
                    endPrintExtFunc(message->draw_parameter, 2, sprite);
                    next_sprite_count = msg_spr_count + 1;
                    *output++ = 25;
                    *output++ = 0;
                    *output++ = icon_text[0];
                    *output++ = icon_text[1];
                    msg_spr_count = next_sprite_count;
                    *output++ = 25;
                    *output++ = font_style;
                }
                text++;
            } else if ((signed char)code == 31) {
                waiting = 1;
                break;
            } else {
                unsigned char second_byte;
                glyph.command = 25;
                glyph.style = font_style;
                glyph.character[0] = *text;
                second_byte = (unsigned char)text[1];
                if ((unsigned int)((unsigned char)second_byte - 1) < 31 ||
                    (unsigned int)((unsigned char)*text - 32) < 96) {
                    glyph.character[1] = 0;
                    *output++ = *text++;
                } else {
                    glyph.character[1] = (char)second_byte;
                    *output++ = *text++;
                    if (*text != 0) {
                        *output++ = *text++;
                    }
                }
                *glyph_end = 0;
                x_offset += xglFontGetStringWidth((const char *)&glyph);
                if (flags & 16) {
                    int next_remaining = remaining - 1;

                    if (remaining == 0) {
                        incomplete = 1;
                        if (line_timing) {
                            text = eMessageNextGyou(text);
                            remaining = message->frame;
                        } else {
                            break;
                        }
                    } else {
                        remaining = next_remaining;
                    }
                }
            }
            sentinel = (unsigned char)*text;
            code = *text == 0 ? 0 : (*text & 255);
        } while (sentinel != 0);
    }
    *output = 0;
    if (incomplete == 0) {
        message->flags |= 128;
        if (message->page_available == 1) {
            waiting = 1;
        }
    }
    xglFontPrint(message->x + message->text_x_offset,
                 message->y + message->text_y_offset - 1,
                 message->draw_parameter, (char *)moji);
    if (message->page_available != 0) {
        if (waiting != 0) {
            message->cursor.x = message->x + (message->text_x_offset + x_offset) + 2;
            message->cursor.y = message->y + (message->text_y_offset + y_offset) + 6;
            message->cursor.work = message->draw_parameter;
            eCursolModeChange(&message->cursor, 32);
            eCursolMain(&message->cursor);
        } else {
            message->cursor.state = 0;
        }
    }
}

/* Every caller compiles as the original only with a void declaration (a
   discarded int result changes their register allocation around the call),
   while the original body does not tail-call eMessageDrawType01, as an int
   definition falling off its end would compile. The address-taken parameter
   reproduces that: GCC 2.96 makes no sibling call in a function using
   ADDRESSOF. */
void eMessageMain(EMessageParam *message)
{
    switch (message->mode) {
    case 0:
        message->visible = 0;
        message->mode = 1;
        /* fall through */
    case 1:
        message->mode = 240;
        break;
    case 32:
        message->visible = 1;
        message->mode = 33;
        message->flags &= 15;
        /* fall through */
    case 33:
        message->mode = 80;
        break;
    case 34:
        message->visible = 1;
        message->mode = 35;
        message->text_timer = 0;
        message->flags = (message->flags & 15) | 16;
        message->frame = 0;
        /* fall through */
    case 35:
        if (message->flags & 128) {
            message->mode = 80;
        } else {
            message->frame++;
            message->text_timer++;
        }
        break;
    case 36:
        message->visible = 1;
        message->mode = 37;
        message->text_timer = 0;
        message->flags = (message->flags & 15) | 48;
        message->frame = 0;
        /* fall through */
    case 37:
        if (message->flags & 128) {
            message->mode = 80;
        } else {
            message->frame++;
            message->text_timer++;
        }
        break;
    case 80:
        message->mode = 81;
        break;
    case 81:
        break;
    case 112:
        message->visible = 0;
        message->mode = 113;
        /* fall through */
    case 113:
        message->mode = 240;
        break;
    case 240:
        message->visible = 0;
        message->mode = 241;
        break;
    case 241:
        break;
    }
    if (message->text != 0 && message->visible != 0) {
        EMessageParam **drawn = &message;

        eMessageDrawType01(*drawn);
    }
}

void eMessageModeChange(EMessageParam *message, unsigned char mode)
{
    message->mode = mode;
}

void eMessageSet(EMessageParam *message, const char *text)
{
    message->x = 0;
    message->y = 0;
    message->draw_parameter = 0;
    message->color_a = message->color_b = message->color_g = message->color_r = 128;
    message->text = text;
    message->flags = 1;
    message->wrap_width_half = 240;
    message->mode = 0;
    message->visible = 0;
    message->text_timer = 0;
    message->frame = 0;
    message->text_x_offset = 0;
    message->text_y_offset = 0;
    message->page_index = 0;
    message->page_available = 0;
    eCursolSet(&message->cursor, 3);
}

void eMessageTextChange(EMessageParam *message, const char *text)
{
    message->text = text;
    if (message->page_available != 0) {
        message->page_index = 0;
        message->page_count = 0;
        while (*(text = eMessageNextWaitKeySearch(text)) != '\0') {
            message->page_count++;
        }
    }
}

int eMessageNextPage(EMessageParam *message, int reset_page)
{
    int next_page;
    unsigned char wrapped_page;
    unsigned char next_mode;

    if (message->page_available != 0) {
        if (reset_page == 0) {
            if ((message->flags & 0x80) != 0) {
                next_page = message->page_index + 1;
                wrapped_page = next_page;
                message->page_index = next_page;
                if (message->page_count < wrapped_page) {
                    message->page_index = message->page_count;
                    return 0;
                }
                next_mode = 34;
            } else {
                next_mode = 32;
            }
            message->mode = next_mode;
            return 1;
        } else {
            message->page_index = 0;
        }
    }
    return 0;
}

void eMessageDraw(EMessageParam *message)
{
    eMessageMain(message);
}

void eMessageCpy(char *dst, const char *src)
{
    MessageCpyEnd = dst;
    eMessageCat(src);
}

void eMessageCat(const char *src)
{
    char *dst = MessageCpyEnd;
    int count;
    int copied;
    int encoded_byte;

    if (src != 0) {
        encoded_byte = (unsigned char)*src;
        while (*src != 0) {
            if ((unsigned int)(encoded_byte - 1) < 25) {
                char command_byte = *src;
                count = xglFontGetSPcodeSize((unsigned char)command_byte,
                                           (const unsigned char *)src) + 1;
                for (copied = 0; copied < count; copied++) {
                    *dst++ = *src++;
                }
            } else {
                int control = *src;

                if (control == 30) {
                    *dst++ = *src++;
                    *dst++ = *src++;
                } else if (control == 31 ||
                           (unsigned char)(*src - 32) < 95) {
                    *dst++ = *src;
                    src++;
                } else {
                    count = eMessageHalfSpaseCheck((const unsigned char *)src);
                    if (count != 0) {
                        for (copied = 0; copied < count; copied++) {
                            *dst++ = *src++;
                        }
                    } else {
                        *dst++ = *src++;
                        *dst++ = *src++;
                    }
                }
            }
            encoded_byte = (unsigned char)*src;
        }
        *dst = 0;
        MessageCpyEnd = dst;
    }
}
