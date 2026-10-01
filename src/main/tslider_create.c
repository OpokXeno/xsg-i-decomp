#include "common.h"
#include "shared.h"
#include "tslider_create.h"

TwinWindow2 *TSLIDER_create(int requestedSlot)
{
    TwinWindow2 *window;

    window = TWSYS_createComponent(requestedSlot, 2);
    if (window != 0) {
        window->valueOffset = 0x58;
        window->max_digits = 4;
        window->max_value = 100;
        window->flags = 0x15;
        window->state = 1;
        window->width = 0x60;
        window->height = 0x28;
        window->x = 48.0f;
        window->y = 64.0f;
        window->min_value = 0;
        window->value = 0;
        window->brightness = 0.0f;
        window->options = 0;
    }
    return window;
}

void TSLIDER_init(TwinWindow2 *slider)
{
    int value = slider->value;
    int digitWidth;

    if (value < slider->min_value) {
        slider->value = slider->min_value;
        value = slider->min_value;
    }
    if (slider->max_value < value) {
        slider->value = slider->max_value;
    }
    digitWidth = (slider->options & 2) ? 20 : 10;
    slider->width = slider->max_digits * digitWidth + 0x10;
}

/*
 * One frame of the slider component. flags bit 0x2 records that the phase of
 * the current state has been armed: while it is clear this arms the phase of
 * `state` (the frame counter it runs on) and returns, and while it is set it
 * advances that phase. State 1 opens the slider over 11 frames and then hands
 * over to the idle state 14; 14 reads the pad, where circle confirms and cross
 * confirms with value -1 (both go to 15) and the directions step the value;
 * 15 starts the closing state 2, which counts the 10 frames back down and then
 * clears the component-alive and update bits TSLIDER_create set.
 */
void TSLIDER_updateDefault(TwinWindow2 *slider)
{
    if ((slider->flags & 2) == 0) {
        switch (slider->state) {
        case 1:
        case 14:
        case 15:
            slider->timer = 0;
            slider->flags |= 2;
            break;
        case 2:
            slider->timer = 10;
            slider->flags |= 2;
            break;
        }
        return;
    }
    if ((slider->flags & 4) == 0) {
        return;
    }
    switch (slider->state) {
    case 15:
        slider->state = 2;
        slider->flags &= ~2;
        break;
    case 14:
        if (PadData.pressed & PAD_CIRCLE) {
            slider->state = 15;
            slider->flags &= ~2;
        }
        if (PadData.pressed & PAD_CROSS) {
            slider->state = 15;
            slider->flags &= ~2;
            slider->value = -1;
        }
        slider->timer = (slider->timer + 1) & 0xFF;
        if (PadData.repeat & PAD_LEFT) {
            slider->timer = 0;
            slider->value -= 10;
            if (slider->value < slider->min_value) {
                slider->value = slider->min_value;
            }
        } else if (PadData.repeat & PAD_DOWN) {
            slider->timer = 0;
            slider->value -= 1;
            if (slider->value < slider->min_value) {
                slider->value = slider->min_value;
            }
        }
        if (PadData.repeat & PAD_RIGHT) {
            slider->timer = 0;
            slider->value += 10;
            if (slider->max_value < slider->value) {
                slider->value = slider->max_value;
            }
        } else if (PadData.repeat & PAD_UP) {
            slider->timer = 0;
            slider->value += 1;
            if (slider->max_value < slider->value) {
                slider->value = slider->max_value;
            }
        }
        break;
    case 2:
        slider->timer--;
        if (slider->timer < 0) {
            slider->flags &= ~0x11;
        }
        break;
    case 1:
        slider->timer++;
        if (slider->timer >= 11) {
            slider->state = 14;
            slider->flags &= ~2;
        }
        break;
    }
}

void TSLIDER_drawDefault(TwinWindow2 *window)
{
    unsigned char digits[0x10];
    unsigned int fullWidthDigits[8];
    long long textArg;
    unsigned char *digitsEnd;
    int x;
    int y;
    int brightness;
    int padding;

    if ((unsigned short) (window->state - 1) >= 2) {
        x = (int) window->x;
        y = (int) window->y;
        brightness = (int) window->brightness;
        digitsEnd = STRING_int(digits, window->value);
        *digitsEnd = 0;
        padding = (window->max_digits - (digitsEnd - digits)) * 0x14;
        if (padding < 0) {
            padding = 0;
        }
        STRING_h2zEUC(fullWidthDigits, digits);
        textArg = (long long) (int) fullWidthDigits;
        xglFontPrintf(x + padding + 0xA, y + 8, 0x01FFFFF0 - brightness, &textArg);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/tslider_create", TMENU_addQuery2);

/*
 * The field-conversation entry point of the toolkit text window: it marks the
 * record's mode byte with 3 and lets TWIN_init2 apply the matching layout
 * (WIN_initCF, src/main/twsys_init.c), the way TWIN_initScene below marks the
 * same byte with 4 for the scene-VM window. The window is spelled void * here
 * like TWIN_init2's own parameter, because this TU reaches the one record
 * through both of its partial views: createItemGetWin holds the TwinWindow
 * view and only the TwinWindow2 view names the mode halfword.
 */
void TWIN_initCF(void *window)
{
    TwinWindow2 *textWindow = window;

    textWindow->mode = (textWindow->mode & 0xFF00) | 3;
    TWIN_init2(window);
}

void TWIN_initScene(TwinWindow2 *window)
{
    window->param = 0x30;
    window->kind = 3;
    window->mode = (window->mode & 0xFF00) | 4;
    TWIN_init2(window);
    window->x = 0.0f;
    window->y = 324.0f;
}

extern const char D_004C1D18[]; /* "/[waitkey(0);close()]" */

void *createItemGetWin(const char *text)
{
    TwinWindow *window;
    int length;

    window = TWIN_create2(-1);
    length = strlen(text);
    window->kind = 2;
    window->param = length + 2;
    TWIN_initCF(window);
    MSG_print2(window, text, length);
    MSG_print2(window, D_004C1D18, -1);
    return window->body;
}
