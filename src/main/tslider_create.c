#include "common.h"

#include "shared.h"

#include "tslider_create.h"

typedef struct TsliderLiteralPool {
    char close_command[24];
    char script_root[28];
    char trailing_empty_string[4];
} TsliderLiteralPool;

const TsliderLiteralPool D_004C1D18 = {
    "/[waitkey(0);close()]",
    "host0:/home/xeno/script/",
    ""
};

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

void TMENU_addQuery2(MenuNative *menu, char **texts, int count)
{
    unsigned char *out = menu->textEnd;
    int i;
    int row;
    int lines;
    int firstItem;
    int textWidth;
    int current;
    int lastByte;
    int windowWidth;
    unsigned char *lineStart;
    unsigned char *textBlockEnd;

    if (menu->textStart == 0) {
        menu->textStart = out;
    }
    firstItem = -1;
    lines = 0;
    for (i = 0; i < count; i++) {
        unsigned char *src = (unsigned char *) texts[i];

        if (((src[0] << 16) | (src[1] << 8) | src[2]) == 0) {
            *out++ = 0;
            *out++ = 0;
            menu->textEnd = out;
            firstItem = i + 1;
            break;
        }
        for (;;) {
            current = *src++;
            if (current == 0) {
                lineStart = menu->textEnd;
                if (menu->rowWidth < out - lineStart) {
                    menu->rowWidth = out - lineStart;
                }
                lastByte = out[-1];
                if (i == count - 1) {
                    if (lastByte != '\n') {
                        *out++ = 0;
                        *out++ = 0;
                        menu->textEnd = out;
                    } else {
                        out[-1] = 0;
                        *out++ = 0;
                        break;
                    }
                } else if (lastByte != '\n') {
                    *out++ = '\n';
                    menu->textEnd = out;
                } else {
                    break;
                }
                lines++;
                break;
            }
            if (current == '/') {
                if (*src == '[') {
                    int markupByte;
                    for (;;) {
                        markupByte = *src++;
                        if (markupByte >= 161) {
                            *out++ = markupByte;
                            *out++ = *src++;
                            continue;
                        }
                        if (markupByte == ']') {
                            break;
                        }
                    }
                } else {
                    *out++ = current;
                }
            } else if (current == '\n') {
                int length = out - menu->textEnd;
                if (menu->rowWidth < length) {
                    menu->rowWidth = length;
                }
                *out++ = current;
                menu->textEnd = out;
                lines++;
            } else {
                *out++ = current;
                if (current >= 160) {
                    *out++ = *src++;
                }
            }
        }
    }
    textBlockEnd = menu->textEnd;
    textWidth = menu->rowWidth * 10;
    windowWidth = menu->textStart == 0 ? textWidth + 56 : textWidth + 136;
    if (menu->width < windowWidth) {
        menu->width = windowWidth;
    }
    menu->rows = (unsigned char **) (((unsigned int) textBlockEnd + 3) >> 2 << 2);
    menu->textEnd = (unsigned char *) (menu->rows + lines);
    for (row = 0; row < lines; row++) {
        menu->rows[row] = menu->textEnd;
        *menu->textEnd = 0;
        menu->textEnd += menu->rowWidth + 32;
    }
    menu->rowCount = lines;
    menu->selectedRow = 0;
    menu->scroll = 0;
    menu->height += lines * 24 + 24;
    if (firstItem > 0) {
        for (i = firstItem; i < count; i++) {
            TMENU_addItem(menu, texts[i]);
        }
    }
}

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
    MSG_print2(window, D_004C1D18.close_command, -1);
    return window->body;
}
