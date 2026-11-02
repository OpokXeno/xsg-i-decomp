#ifndef INCLUDE_MAIN_E_MESSAGE_H
#define INCLUDE_MAIN_E_MESSAGE_H

#include "main/open_close_main.h"

/*
 * EMessageParam.flags. Low nibble is kept across eMessageMain's page starts;
 * TYPEWRITER reveals one glyph per frame, LINE_TIMING (set with it) restarts
 * the reveal counter on each line; PAGE_DONE is set once the whole page has
 * been output.
 */
#define EMESSAGE_FLAG_HEADER 0x01
#define EMESSAGE_FLAG_KEEP_MASK 0x0F
#define EMESSAGE_FLAG_TYPEWRITER 0x10
#define EMESSAGE_FLAG_LINE_TIMING 0x20
#define EMESSAGE_FLAG_PAGE_DONE 0x80

/* Modes beyond the shared E_MODE_* set: SHOW (0x20/0x21) prints at once,
 * OPEN (0x22/0x23) types the page glyph by glyph, LINE (0x24/0x25) line by line. */
#define EMESSAGE_MODE_LINE_START 0x24
#define EMESSAGE_MODE_LINE_STEP 0x25

typedef struct EMessageParam {
    unsigned char flags;
    unsigned char mode;
    unsigned char visible;
    unsigned char wrap_width_half;
    short x;
    short y;
    int draw_parameter;
    short text_x_offset;
    short text_y_offset;
    unsigned char color_r;
    unsigned char color_g;
    unsigned char color_b;
    unsigned char color_a;
    unsigned short text_timer;
    unsigned short frame;
    const char *text;
    unsigned char page_available;
    unsigned char page_index;
    unsigned char page_count;
    ECursolMainData cursor;
} EMessageParam;

/* Functions and data used by other translation units. */
void eMessageMain(EMessageParam *message);
void eMessageSet(EMessageParam *message, const char *text);
void eMessageTextChange(EMessageParam *message, const char *text);
void eMessageModeChange(EMessageParam *message, unsigned char mode);
int eMessageNextPage(EMessageParam *message, int reset_page);
void eMessageDraw(EMessageParam *message);
void eMessageCpy(char *destination, const char *source);
void eMessageCat(const char *source);
int eMessageNextGyouMaxGet(const char *text);

#endif /* INCLUDE_MAIN_E_MESSAGE_H */
