#ifndef INCLUDE_MAIN_E_MESSAGE_H
#define INCLUDE_MAIN_E_MESSAGE_H

#include "main/open_close_main.h"

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
