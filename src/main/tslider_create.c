#include "common.h"
#include "shared.h"
#include "tslider_create.h"

INCLUDE_ASM("asm/main/nonmatchings/tslider_create", TSLIDER_create);

INCLUDE_ASM("asm/main/nonmatchings/tslider_create", TSLIDER_init);

INCLUDE_ASM("asm/main/nonmatchings/tslider_create", TSLIDER_updateDefault);

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
