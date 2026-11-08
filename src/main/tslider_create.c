#include "common.h"
#include "shared.h"
#include "tslider_create.h"
#include "main/tslider_create.h"

/* Slider defaults (TSLIDER_create). */
#define TSLIDER_VALUE_OFFSET 0x58
#define TSLIDER_DEFAULT_DIGITS 4
#define TSLIDER_DEFAULT_MAX 100
#define TSLIDER_DEFAULT_WIDTH 0x60
#define TSLIDER_DEFAULT_HEIGHT 0x28
#define TSLIDER_DEFAULT_X 48.0f
#define TSLIDER_DEFAULT_Y 64.0f
/* Text-mode option bit: full-width digits, drawn 20 pixels wide instead of 10. */
#define TSLIDER_OPTION_FULLWIDTH 2
#define HALFWIDTH_CHAR_WIDTH 10
#define FULLWIDTH_CHAR_WIDTH 20
/* Value change of one pad step: the repeat step and the fine step. */
#define TSLIDER_STEP_COARSE 10
#define TSLIDER_STEP_FINE 1
/* Value reported when the slider is cancelled. */
#define TSLIDER_CANCELLED (-1)

/* EUC-JP lead byte of a JIS X 0208 character. Inside markup, such a byte and the
 * byte after it are copied as a pair. */
#define EUC_FIRST_JIS_LEAD 161

typedef struct TsliderLiteralPool {
    char close_command[24];
    char script_root[28];
    char trailing_empty_string[4];
} TsliderLiteralPool;

const TsliderLiteralPool tsliderLiteralTable = {
    "/[waitkey(0);close()]",
    "host0:/home/xeno/script/",
    ""
};

/* Creates a slider component in the requested slot and sets its default geometry,
 * value range and state. Returns 0 when no component could be created. */
TComponent *TSLIDER_create(int requestedSlot)
{
    TComponent *window;

    window = TWSYS_createComponent(requestedSlot, TCOMP_KIND_SLIDER);
    if (window != 0) {
        window->valueOffset = TSLIDER_VALUE_OFFSET;
        window->max_digits = TSLIDER_DEFAULT_DIGITS;
        window->max_value = TSLIDER_DEFAULT_MAX;
        window->flags = TCOMP_FLAG_ACTIVE | TCOMP_FLAG_RUN | TCOMP_FLAG_OPEN;
        window->state = TCOMP_STATE_OPENING;
        window->pixelWidth = TSLIDER_DEFAULT_WIDTH;
        window->pixelHeight = TSLIDER_DEFAULT_HEIGHT;
        window->x = TSLIDER_DEFAULT_X;
        window->y = TSLIDER_DEFAULT_Y;
        window->min_value = 0;
        window->value = 0;
        window->brightness = 0.0f;
        window->options = 0;
    }
    return window;
}

/* Clamps the value to the slider's range and sets the pixel width from the digit count. */
void TSLIDER_init(TComponent *slider)
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
    digitWidth = (slider->options & TSLIDER_OPTION_FULLWIDTH)
        ? FULLWIDTH_CHAR_WIDTH : HALFWIDTH_CHAR_WIDTH;
    slider->pixelWidth = slider->max_digits * digitWidth + 0x10;
}

/* Advances the slider state machine once per frame: the open and close transitions
 * and the pad handling in the input state. */
void TSLIDER_updateDefault(TComponent *slider)
{
    /* Entering a state arms its wait; the state logic runs on a later frame. */
    if ((slider->flags & TCOMP_FLAG_WAIT) == 0) {
        switch (slider->state) {
        case TCOMP_STATE_OPENING:
        case TCOMP_STATE_INPUT:
        case TCOMP_STATE_DONE:
            slider->waitFrames = 0;
            slider->flags |= TCOMP_FLAG_WAIT;
            break;
        case TCOMP_STATE_CLOSING:
            slider->waitFrames = TCOMP_CLOSE_FRAMES;
            slider->flags |= TCOMP_FLAG_WAIT;
            break;
        }
        return;
    }
    if ((slider->flags & TCOMP_FLAG_RUN) == 0) {
        return;
    }
    switch (slider->state) {
    case TCOMP_STATE_DONE:
        slider->state = TCOMP_STATE_CLOSING;
        slider->flags &= ~TCOMP_FLAG_WAIT;
        break;
    case TCOMP_STATE_INPUT:
        if (PadData.pressed & PAD_CIRCLE) {
            slider->state = TCOMP_STATE_DONE;
            slider->flags &= ~TCOMP_FLAG_WAIT;
        }
        if (PadData.pressed & PAD_CROSS) {
            slider->state = TCOMP_STATE_DONE;
            slider->flags &= ~TCOMP_FLAG_WAIT;
            slider->value = TSLIDER_CANCELLED;
        }
        slider->waitFrames = (slider->waitFrames + 1) & 0xFF;
        if (PadData.repeat & PAD_LEFT) {
            slider->waitFrames = 0;
            slider->value -= TSLIDER_STEP_COARSE;
            if (slider->value < slider->min_value) {
                slider->value = slider->min_value;
            }
        } else if (PadData.repeat & PAD_DOWN) {
            slider->waitFrames = 0;
            slider->value -= TSLIDER_STEP_FINE;
            if (slider->value < slider->min_value) {
                slider->value = slider->min_value;
            }
        }
        if (PadData.repeat & PAD_RIGHT) {
            slider->waitFrames = 0;
            slider->value += TSLIDER_STEP_COARSE;
            if (slider->max_value < slider->value) {
                slider->value = slider->max_value;
            }
        } else if (PadData.repeat & PAD_UP) {
            slider->waitFrames = 0;
            slider->value += TSLIDER_STEP_FINE;
            if (slider->max_value < slider->value) {
                slider->value = slider->max_value;
            }
        }
        break;
    case TCOMP_STATE_CLOSING:
        slider->waitFrames--;
        if (slider->waitFrames < 0) {
            slider->flags &= ~(TCOMP_FLAG_ACTIVE | TCOMP_FLAG_OPEN);
        }
        break;
    case TCOMP_STATE_OPENING:
        slider->waitFrames++;
        if (slider->waitFrames >= TCOMP_OPEN_FRAMES) {
            slider->state = TCOMP_STATE_INPUT;
            slider->flags &= ~TCOMP_FLAG_WAIT;
        }
        break;
    }
}

/* Draws the current value as full-width digits, indented by the digit cells it does not use. */
void TSLIDER_drawDefault(TComponent *window)
{
    unsigned char digits[0x10];
    unsigned int fullWidthDigits[8];
    long long textArg;
    unsigned char *digitsEnd;
    int x;
    int y;
    int brightness;
    int padding;

    if ((unsigned short) (window->state - TCOMP_STATE_OPENING) >= 2) {
        x = (int) window->x;
        y = (int) window->y;
        brightness = (int) window->brightness;
        digitsEnd = STRING_int(digits, window->value);
        *digitsEnd = 0;
        padding = (window->max_digits - (digitsEnd - digits)) * FULLWIDTH_CHAR_WIDTH;
        if (padding < 0) {
            padding = 0;
        }
        STRING_h2zEUC(fullWidthDigits, digits);
        textArg = (int) fullWidthDigits;
        xglFontPrintf(x + padding + 0xA, y + 8, 0x01FFFFF0 - brightness, &textArg);
    }
}

/*
 * Appends the query texts to the menu's text block, one row per line, and records the
 * widest row. Items after an all-zero entry are added as menu items.
 */
void TMENU_addQuery2(MenuNativeQueryView *menu, char **texts, int count)
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
    /* Copy each text, dropping [...] markup and splitting it at newlines. */
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
                        if (markupByte >= EUC_FIRST_JIS_LEAD) {
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
                if (current >= EUC_FIRST_MULTIBYTE_LEAD) {
                    *out++ = *src++;
                }
            }
        }
    }
    /* Size the window from the widest row, then place one row pointer per line. */
    textBlockEnd = menu->textEnd;
    textWidth = menu->rowWidth * HALFWIDTH_CHAR_WIDTH;
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
    menu->height += lines * TMENU_LINE_HEIGHT + TMENU_LINE_HEIGHT;
    if (firstItem > 0) {
        for (i = firstItem; i < count; i++) {
            TMENU_addItem(menu, texts[i]);
        }
    }
}

void TWIN_initCF(void *window)
{
    TComponent *textWindow = window;

    textWindow->kindAndGroup = (textWindow->kindAndGroup & TCOMP_SLOT_MASK) | TCOMP_KIND_TEXT_FIELD;
    TWIN_init2(window);
}

void TWIN_initScene(TComponent *window)
{
    window->param = TWIN_SCENE_LAYOUT_OFFSET;
    window->kind = TWIN_LINE_COUNT;
    window->kindAndGroup = (window->kindAndGroup & TCOMP_SLOT_MASK) | TCOMP_KIND_TEXT_SCENE;
    TWIN_init2(window);
    window->x = 0.0f;
    window->y = TWIN_SCENE_Y;
}

/* Creates a text window holding the text followed by the close command, and returns
 * its state field. */
void *createItemGetWin(const char *text)
{
    TComponent *window;
    int length;

    window = TWIN_create2(-1);
    length = strlen(text);
    window->kind = 2;
    window->param = length + 2;
    TWIN_initCF(window);
    MSG_print2(window, text, length);
    MSG_print2(window, tsliderLiteralTable.close_command, -1);
    return &window->state;
}
