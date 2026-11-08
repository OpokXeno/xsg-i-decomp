#include "common.h"
#include "shared.h"
#include "main/e_number_main.h"
#include "main/end_print.h"
#include "main/open_close_main.h"

#define E_NUMBER_STORE_STRING(destination, string) (*(destination) = (string))

/* ENumberMainData.format: how eNumberMain lays the value out. Formats 0, 1
 * and 14 (plain digit run), 3-6 and 12-13 (two values side by side; 12 and 13
 * turn the first one red when it is under a fifth of the second) and 7-8
 * (signed) have no named constant. */
enum {
    ENUMBER_FORMAT_TIME = 2,        /* three two-digit fields split by separators */
    ENUMBER_FORMAT_TIME_BLANK = 11, /* the same layout with blank fields */
    ENUMBER_FORMAT_DASHES = 9,      /* digits count of dash sprites */
    ENUMBER_FORMAT_TEXT = 10        /* value is a string pointer */
};

/* Sprite ids eNumberMain draws as time separators, sign prefixes and dashes. */
enum {
    ENUMBER_SPRITE_TIME_SEPARATOR = 0x704,
    ENUMBER_SPRITE_PLUS = 0x705,
    ENUMBER_SPRITE_MINUS = 0x706
};

#define ENUMBER_PHASE_MASK 15 /* low nibble of ENumberMainData.mode */
#define ENUMBER_PHASE_DRAW 3  /* the phase in which the value is drawn */
#define ENUMBER_COLOR_NEUTRAL 128
#define ENUMBER_DIGIT_WIDTH 11
#define ENUMBER_TIME_FIELD_SPACING 32
#define ENUMBER_TIME_FIELD_COUNT 3
#define ENUMBER_TIME_SEPARATOR_OFFSET 22 /* from the field's left edge */

typedef struct ELinePoint {
    int x;
    int y;
    int work;
    signed char red;
    signed char green;
    signed char blue;
    signed char alpha;
} ELinePoint;

typedef struct ELineEntry {
    u8 state;
    u8 unmodeled_001[2];
    u8 duration;
    u8 progress[5];
    u8 unmodeled_009[3];
    ELinePoint points[5];
    unsigned int count;
    ELinePoint render[5];
} ELineEntry;

typedef struct ELineSetData {
    u8 state;
    u8 unmodeled_001[2];
    u8 duration;
    u8 progress[5];
    u8 unmodeled_009[3];
    ELinePoint points[5];
} ELineSetData;

typedef ENumberRenderDescriptor ENumberRenderArray[4];

extern void endSpriteSet(void *sprite, int mode);
/*
 * Draws a number during its draw phase, laying out its digits, separators
 * or text according to ENumberMainData.format.
 */
void eNumberMain(ENumberMainData *number)
{
    unsigned char byteValue[4];
    short pairValue[2];
    unsigned char colors[2];
    int component;
    unsigned short componentX;
    unsigned short byteX;
    int componentWork;
    unsigned char componentValue;
    int blankWork;
    int blankComponent;
    int blankValue;
    int part;
    unsigned short partX;
    int dash;
    int signX;
    int dashX;
    int spacing;
    int magnitude;
    ENumberRenderDescriptor *sprite;
    ENumberSpriteDescriptor *separatorSprite;
    ENumberRenderDescriptor *text;
    const unsigned char tintHigh = 144;
    const unsigned char opacity = 128;
    const unsigned char tintLow = 80;
    const unsigned char alertLow = 64;

    if ((number->mode & ENUMBER_PHASE_MASK) != ENUMBER_PHASE_DRAW)
        return;

    switch (number->format) {
    case 0:
    case 1:
    case 14:
        number->render[0].x = number->x;
        number->render[0].y = number->y;
        number->render[0].work = number->work;
        number->render[0].payload.number.red = number->color_r;
        number->render[0].payload.number.green = number->color_g;
        number->render[0].payload.number.blue = number->color_b;
        number->render[0].payload.number.alpha = number->color_a;
        number->render[0].payload.number.value = number->value;
        number->render[0].argument1 = number->digits;
        if (number->format == 1)
            number->render[0].argument2 = number->colorModeFirst * 16 + 1;
        else
            number->render[0].argument2 = number->colorModeFirst * 16;
        endPrintExtFunc(number->work, END_PRINT_CMD_NUMBER,&number->render[0]);
        if (number->format == 14 && number->value >= 0) {
            number->separators[0].spriteId = 0x506;
            endSpriteSet(&number->separators[0], 255);
            number->separators[0].x = number->x + number->digits * ENUMBER_DIGIT_WIDTH;
            number->separators[0].work = number->work;
            number->separators[0].color[0] = number->separators[0].color[1] =
                number->separators[0].color[2] = number->separators[0].color[3] = ENUMBER_COLOR_NEUTRAL;
            number->separators[0].y = number->y;
            endPrintExtFunc(number->work, END_PRINT_CMD_SPRITE,&number->separators[0]);
        }
        break;
    case ENUMBER_FORMAT_TIME:
        /* This format interprets the stored word as three byte components. */
        __builtin_memcpy(byteValue, &number->value, sizeof byteValue);
        for (component = 0; component < ENUMBER_TIME_FIELD_COUNT; component++) {
            byteX = number->x;
            componentWork = number->work;
            byteX += component * ENUMBER_TIME_FIELD_SPACING;
            number->render[component].x = byteX;
            {
                unsigned short y = number->y;
                number->render[component].work = componentWork;
                number->render[component].y = y;
            }
            number->render[component].payload.number.red = number->color_r;
            number->render[component].payload.number.green = number->color_g;
            number->render[component].payload.number.blue = number->color_b;
            number->render[component].payload.number.alpha = number->color_a;
            componentValue = byteValue[3 - component];
            number->render[component].argument1 = 2;
            number->render[component].payload.number.value = componentValue;
            number->render[component].argument2 = 0;
            endPrintExtFunc(number->work, END_PRINT_CMD_NUMBER,&number->render[component]);
        }
        for (part = 0; part < 2; part++) {
            separatorSprite = &number->separators[part];
            number->separators[part].spriteId = ENUMBER_SPRITE_TIME_SEPARATOR;
            endSpriteSet(separatorSprite, 255);
            number->separators[part].x = number->x + part * ENUMBER_TIME_FIELD_SPACING + ENUMBER_TIME_SEPARATOR_OFFSET;
            number->separators[part].work = number->work;
            number->separators[part].y = number->y + 2;
            number->separators[part].color[0] = number->color_r;
            number->separators[part].color[1] = number->color_g;
            number->separators[part].color[2] = number->color_b;
            number->separators[part].color[3] = number->color_a;
            endPrintExtFunc(number->work, END_PRINT_CMD_SPRITE,separatorSprite);
        }
        break;
    case ENUMBER_FORMAT_TIME_BLANK:
        for (blankComponent = 0; blankComponent < ENUMBER_TIME_FIELD_COUNT; blankComponent++) {
            componentX = number->x;
            blankWork = number->work;
            componentX += blankComponent * ENUMBER_TIME_FIELD_SPACING;
            number->render[blankComponent].x = componentX;
            {
                unsigned short y = number->y;
                number->render[blankComponent].work = blankWork;
                number->render[blankComponent].y = y;
            }
            blankValue = -1;
            number->render[blankComponent].payload.number.red = number->color_r;
            number->render[blankComponent].payload.number.green = number->color_g;
            number->render[blankComponent].payload.number.blue = number->color_b;
            number->render[blankComponent].payload.number.alpha = number->color_a;
            number->render[blankComponent].argument1 = 2;
            number->render[blankComponent].payload.number.value = blankValue;
            number->render[blankComponent].argument2 = 0;
            endPrintExtFunc(number->work, END_PRINT_CMD_NUMBER,&number->render[blankComponent]);
        }
        for (part = 0; part < 2; part++) {
            separatorSprite = &number->separators[part];
            number->separators[part].spriteId = ENUMBER_SPRITE_TIME_SEPARATOR;
            endSpriteSet(separatorSprite, 255);
            number->separators[part].x = number->x + part * ENUMBER_TIME_FIELD_SPACING + ENUMBER_TIME_SEPARATOR_OFFSET;
            number->separators[part].work = number->work;
            number->separators[part].y = number->y + 2;
            number->separators[part].color[0] = number->color_r;
            number->separators[part].color[1] = number->color_g;
            number->separators[part].color[2] = number->color_b;
            number->separators[part].color[3] = number->color_a;
            endPrintExtFunc(number->work, END_PRINT_CMD_SPRITE,separatorSprite);
        }
        break;
    case 3: case 4: case 5: case 6: case 12: case 13:
        /* Paired formats interpret the stored word as two signed values. */
        __builtin_memcpy(pairValue, &number->value, sizeof pairValue);
        colors[0] = number->colorModeFirst;
        colors[1] = number->colorModeSecond;
        switch (number->format - 3) {
        case 0: case 9: number->separators[0].spriteId = 0x702; break;
        case 1: case 10: number->separators[0].spriteId = 0x703; break;
        case 2: case 3: number->separators[0].spriteId = 0x118; break;
        }
        endSpriteSet(&number->separators[0], 255);
        spacing = number->separators[0].width;
        if (number->firstOnly == 0) {
            number->separators[0].x = number->x;
            if (number->separators[0].spriteId == 0x118)
                number->separators[0].y = number->y + 3;
            else
                number->separators[0].y = number->y;
            number->separators[0].work = number->work;
            number->separators[0].color[0] = number->separators[0].color[1] =
                number->separators[0].color[2] = number->separators[0].color[3] = ENUMBER_COLOR_NEUTRAL;
            endPrintExtFunc(number->work, END_PRINT_CMD_SPRITE,&number->separators[0]);
        }
        if ((unsigned char)(number->format - 12) < 2) {
            int maximum = pairValue[1];
            int current = pairValue[0];
            if (current < maximum / 5)
                colors[0] = 3;
        }
        for (part = 0; part < 2; part++) {
            if (part == 1 && number->firstOnly != 0)
                break;
            number->render[part].work = number->work;
            partX = number->x + part * spacing;
            number->render[part].x = partX;
            number->render[part].y = number->y;
            switch (colors[part]) {
            case 0:
                number->render[part].payload.number.red = number->color_r;
                number->render[part].payload.number.green = number->color_g;
                number->render[part].payload.number.blue = number->color_b;
                number->render[part].payload.number.alpha = number->color_a;
                break;
            case 1:
                number->render[part].payload.number.red = tintHigh;
                number->render[part].payload.number.blue = number->render[part].payload.number.green = tintLow;
                number->render[part].payload.number.alpha = opacity;
                break;
            case 2:
                number->render[part].payload.number.green = number->render[part].payload.number.red = tintLow;
                number->render[part].payload.number.blue = tintHigh;
                number->render[part].payload.number.alpha = opacity;
                break;
            case 3:
                number->render[part].payload.number.red = tintHigh;
                number->render[part].payload.number.blue = number->render[part].payload.number.green = alertLow;
                number->render[part].payload.number.alpha = opacity;
                break;
            }
            number->render[part].payload.number.value = pairValue[part];
            number->render[part].argument1 = number->digits;
            number->render[part].argument2 = number->alignment + (1 - part) * 16;
            endPrintExtFunc(number->work, END_PRINT_CMD_NUMBER,&number->render[part]);
        }
        break;
    case 7: case 8:
        /* Signed value: the sign sprite comes first, then the magnitude. */
        signX = number->x;
        if (number->value < 0)
            number->separators[0].spriteId = ENUMBER_SPRITE_MINUS;
        else
            number->separators[0].spriteId = ENUMBER_SPRITE_PLUS;
        endSpriteSet(&number->separators[0], 255);
        number->separators[0].x = signX;
        number->separators[0].y = number->y - number->format * 2 + 15;
        number->separators[0].color[0] = number->color_r;
        number->separators[0].color[1] = number->color_g;
        number->separators[0].color[2] = number->color_b;
        number->separators[0].color[3] = number->color_a;
        number->separators[0].work = number->work;
        endPrintExtFunc(number->work, END_PRINT_CMD_SPRITE,&number->separators[0]);
        signX += number->separators[0].width;
        magnitude = number->value;
        if (magnitude < 0)
            magnitude = -magnitude;
        number->render[0].x = signX;
        number->render[0].y = number->y;
        number->render[0].payload.number.red = number->color_r;
        number->render[0].payload.number.green = number->color_g;
        number->render[0].payload.number.blue = number->color_b;
        number->render[0].payload.number.alpha = number->color_a;
        number->render[0].payload.number.value = magnitude;
        number->render[0].argument1 = number->digits;
        number->render[0].argument2 = number->format - 7;
        number->render[0].work = number->work;
        endPrintExtFunc(number->work, END_PRINT_CMD_NUMBER,&number->render[0]);
        break;
    case ENUMBER_FORMAT_DASHES:
        dashX = number->x;
        sprite = &number->render[0];
        for (dash = 0; dash < number->digits; dash++, sprite++) {
            sprite->argument1 = ENUMBER_SPRITE_MINUS;
            endSpriteSet(sprite, 255);
            sprite->x = dashX;
            sprite->work = number->work;
            sprite->y = number->y - 2;
            endPrintExtFunc(number->work, END_PRINT_CMD_SPRITE,sprite);
            dashX += sprite->payload.sprite.width;
        }
        break;
    case ENUMBER_FORMAT_TEXT:
        text = &number->render[0];
        text->x = number->x;
        text->work = number->work;
        text->y = number->y;
        text->payload.text.color[0] = number->color_r;
        text->payload.text.color[1] = number->color_g;
        text->payload.text.color[2] = number->color_b;
        text->payload.text.color[3] = number->color_a;
        E_NUMBER_STORE_STRING(&text->payload.text.text, (const char *)number->value);
        endPrintExtFunc(number->work, END_PRINT_CMD_TAG_FONT,text);
        break;
    }
}

void eNumberNumberChange(void)
{
}

/*
 * Sets a number to the neutral colour and draw phase with zero layout fields.
 * Callers pass a second argument, which this initializer does not read.
 */
void eNumberSet(ENumberMainData *number, int unused)
{
    number->color_r = number->color_g = number->color_b = number->color_a = ENUMBER_COLOR_NEUTRAL;
    number->mode = ENUMBER_PHASE_DRAW;
    number->colorModeFirst = number->colorModeSecond = number->alignment = number->firstOnly = 0;
}

/*
 * Copies the sprite's position, work and colour into its render fields and
 * queues it for printing.
 */
void eSpriteMain(ESpriteMainData *sprite)
{
    endSpriteSet(&sprite->renderX, 5);
    sprite->renderX = sprite->x;
    sprite->renderWork = sprite->work;
    sprite->renderY = sprite->y;
    sprite->renderColor[0] = sprite->color[0];
    sprite->renderColor[1] = sprite->color[1];
    sprite->renderColor[2] = sprite->color[2];
    sprite->renderColor[3] = sprite->color[3];
    endPrintExtFunc(sprite->work, END_PRINT_CMD_SPRITE,&sprite->renderX);
}

/* Resets a sprite's position, work and colour and stores its sprite id. */
void eSpriteSet(ESprite *sprite, short spriteId)
{
    sprite->x = 0;
    sprite->y = 0;
    sprite->work = 0;
    sprite->color[3] = -128;
    sprite->color[2] = -128;
    sprite->color[1] = -128;
    sprite->state = 0;
    sprite->color[0] = -128;
    sprite->spriteId = spriteId;
}

/*
 * Advances a ribbon through its show, run and end modes and queues it for
 * printing when it is visible.
 */
void eRibbonMain(ERibbonMainData *ribbon)
{
    int right;

    switch (ribbon->mode) {
    case E_MODE_SHOW_START:
        ribbon->points[1].x = ribbon->points[0].x = ribbon->x;
        ribbon->points[2].y = ribbon->points[0].y = ribbon->y;
        ribbon->points[3].y = ribbon->points[1].y = ribbon->y + ribbon->height;
        ribbon->points[3].work = ribbon->points[2].work = ribbon->points[0].work = ribbon->work;
        ribbon->visible = 1;
        ribbon->mode = E_MODE_SHOW_STEP;
        ribbon->progress = 0;
    case E_MODE_SHOW_STEP:
        right = (int)((float)ribbon->x + (float)ribbon->width * ((float)ribbon->progress * 0.125f));
        ribbon->progress++;
        ribbon->points[3].x = ribbon->points[2].x = right;
        if (ribbon->progress > 8)
            ribbon->mode = -16;
        break;
    case 0x2E:
        ribbon->mode = 0x2F;
    case E_MODE_INIT:
    case 0x2F:
        ribbon->mode = E_MODE_RUN_START;
        break;
    case E_MODE_RUN_START:
        ribbon->mode = E_MODE_RUN_STEP;
        ribbon->visible = 1;
    case E_MODE_RUN_STEP:
        if (ribbon->style == 0) {
            ribbon->points[0].x = ribbon->points[1].x = ribbon->x;
            ribbon->points[2].x = ribbon->points[3].x = ribbon->x + ribbon->width / 2;
            ribbon->points[4].x = ribbon->points[5].x = ribbon->x + ribbon->width;
        } else {
            ribbon->points[0].x = ribbon->points[1].x = ribbon->x;
            ribbon->points[2].x = ribbon->points[3].x = ribbon->x + ribbon->width / 10;
            ribbon->points[4].x = ribbon->points[5].x = ribbon->x + ribbon->width;
        }
        ribbon->points[0].y = ribbon->points[2].y = ribbon->points[4].y = ribbon->y;
        ribbon->points[1].y = ribbon->points[3].y = ribbon->points[5].y = ribbon->y + ribbon->height;
        ribbon->points[0].work = ribbon->points[1].work = ribbon->points[2].work = ribbon->points[3].work = ribbon->points[4].work = ribbon->points[5].work = ribbon->work;
        ribbon->renderCount = 0;
        break;
    case 0x0F:
    case E_MODE_END_START:
    case E_MODE_END_STEP:
        break;
    }
    if (ribbon->visible)
        endPrintExtFunc(ribbon->points[0].work, END_PRINT_CMD_RIBBON,&ribbon->points[0]);
}

void eRibbonModeChange(ERibbonMainData *ribbon, signed char mode)
{
    ribbon->mode = mode;
}

/*
 * Sets the ribbon style, resets its position and progress, and fills the
 * vertex colours and alphas for that style.
 */
void eRibbonSet(ERibbonMainData *ribbon, int style)
{
    int point;

    ribbon->mode = E_MODE_RUN_START;
    ribbon->style = style;
    ribbon->x = 0;
    ribbon->y = 0;
    ribbon->work = 0;
    ribbon->progress = 0;
    ribbon->visible = 0;
    switch (ribbon->style) {
    case 3:
        for (point = 0; point < 6; point++) {
            if (point & 1) {
                ribbon->points[point].red = 0;
                ribbon->points[point].green = 32;
                ribbon->points[point].blue = 52;
            } else {
                ribbon->points[point].red = 96;
                ribbon->points[point].green = 64;
                ribbon->points[point].blue = -128;
            }
            ribbon->points[point].alpha = 48;
        }
        break;
    case 0:
        ribbon->points[0].blue = ribbon->points[1].blue = ribbon->points[2].blue = ribbon->points[3].blue = 0;
        ribbon->points[0].green = ribbon->points[1].green = ribbon->points[2].green = ribbon->points[3].green = 0;
        ribbon->points[0].red = ribbon->points[1].red = ribbon->points[2].red = ribbon->points[3].red = 0;
        ribbon->points[0].alpha = ribbon->points[1].alpha = ribbon->points[2].alpha = ribbon->points[3].alpha = -128;
        break;
    case 1:
    case 2:
        if (style == 1) {
            ribbon->points[0].alpha = 0;
            ribbon->points[1].alpha = 0;
            ribbon->points[2].alpha = -128;
            ribbon->points[3].alpha = -128;
            ribbon->points[4].alpha = 0;
            ribbon->points[5].alpha = 0;
        } else {
            ribbon->points[3].alpha = 64;
            ribbon->points[2].alpha = 112;
            ribbon->points[0].alpha = 120;
            ribbon->points[1].alpha = 0;
        }
        ribbon->points[0].red = ribbon->points[1].red = ribbon->points[2].red = ribbon->points[3].red = 32;
        ribbon->points[4].red = ribbon->points[5].red = 0;
        ribbon->points[0].green = ribbon->points[1].green = ribbon->points[2].green = ribbon->points[3].green = 52;
        ribbon->points[4].green = ribbon->points[5].green = 32;
        ribbon->points[0].blue = ribbon->points[1].blue = ribbon->points[2].blue = ribbon->points[3].blue = 52;
        ribbon->points[4].blue = ribbon->points[5].blue = 52;
        ribbon->points[1].alpha = ribbon->points[0].alpha = 0;
        ribbon->points[5].alpha = ribbon->points[4].alpha = ribbon->points[3].alpha = ribbon->points[2].alpha = -128;
        break;
    }
}

/*
 * Steps a line's progress animation, then computes the render points by
 * interpolating between the control points.
 */
void eLineMain(ELineEntry *line)
{
    int point;
    unsigned int segment;
    unsigned int reset;
    int remaining;
    float progress;
    int *sourceX;
    int *sourceY;
    int *sourceWork;
    ELinePoint *render;

    switch (line->state) {
    case E_MODE_INIT:
        line->state = E_MODE_RUN_START;
        break;
    case E_MODE_IDLE:
        break;
    case 0x10:
        for (reset = 0; reset < line->count; reset++)
            line->progress[reset] = 0;
        line->state = 0x11;
    case 0x11:
        for (segment = 0; segment < line->count; segment++) {
            line->progress[segment]++;
            if (line->progress[segment] >= line->duration)
                line->progress[segment] = line->duration;
            else
                break;
        }
        remaining = 0;
        for (segment = 0; segment < line->count; segment++)
            remaining += line->progress[segment] - line->duration;
        if (remaining == 0)
            line->state = E_MODE_RUN_START;
        break;
    case E_MODE_SHOW_START:
        for (reset = 0; reset < line->count; reset++)
            line->progress[reset] = 0;
        line->state = E_MODE_SHOW_STEP;
    case E_MODE_SHOW_STEP:
        for (segment = 0; segment < line->count; segment++) {
            line->progress[segment]++;
            if (line->progress[segment] >= line->duration)
                line->progress[segment] = line->duration;
        }
        remaining = 0;
        for (segment = 0; segment < line->count; segment++)
            remaining += line->progress[segment] - line->duration;
        if (remaining == 0)
            line->state = E_MODE_RUN_START;
        break;
    case E_MODE_RUN_START:
        line->state = E_MODE_RUN_STEP;
        break;
    }
    /* The first render point copies its control point; the others are interpolated. */
    sourceX = &line->points[0].x;
    sourceY = &line->points[0].y;
    sourceWork = &line->points[0].work;
    render = line->render;
    render->x = *sourceX;
    line->render[0].y = *sourceY;
    line->render[0].work = *sourceWork;
    render->red = line->points[0].red;
    render->green = line->points[0].green;
    render->blue = line->points[0].blue;
    render->alpha = line->points[0].alpha;
    for (point = 1; point < line->count; point++) {
        progress = (float)line->progress[point - 1] / (float)line->duration;
        line->render[point].x = (int)((float)(line->points[point].x - line->render[point - 1].x) * progress + (float)line->render[point - 1].x);
        line->render[point].y = (int)((float)(line->points[point].y - line->render[point - 1].y) * progress + (float)line->render[point - 1].y);
        line->render[point].work = line->points[point].work;
        line->render[point].red = line->points[point].red;
        line->render[point].green = line->points[point].green;
        line->render[point].blue = line->points[point].blue;
        line->render[point].alpha = line->points[point].alpha;
    }
}

/*
 * Resets a line to state 0 and a duration of seven frames, with every point
 * at colour -1 and alpha -128.
 */
void eLineSet(ELineSetData *line)
{
    int point;

    line->duration = 7;
    line->state = 0;
    for (point = 0; point < 5; point++) {
        line->progress[point] = line->duration;
        line->points[point].blue = -1;
        line->points[point].green = -1;
        line->points[point].red = -1;
        line->points[point].alpha = -128;
    }
}

/*
 * Copies the tag's position and colour into its render block and prints its
 * text when one is set. The green and blue entries are swapped.
 */
void eTagFontMain(ETagFontMainData *tag)
{
    if (tag->render.text != 0) {
        tag->render.x = tag->x;
        tag->render.y = tag->y;
        tag->render.color[0] = tag->color[0];
        tag->render.color[1] = tag->color[2];
        tag->render.color[2] = tag->color[1];
        tag->render.color[3] = tag->color[3];
        tag->render.work = tag->work;
        endPrintExtFunc(tag->work, END_PRINT_CMD_TAG_FONT,&tag->render);
    }
}

/* Resets a tag's state and colour and sets the text it prints. */
void eTagFontSet(ETagFontMainData *tag, const char *text)
{
    tag->state = 0;
    tag->subState = 0;
    tag->color[3] = -128;
    tag->color[1] = -128;
    tag->color[2] = -128;
    tag->color[0] = -128;
    tag->render.text = text;
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const char TopMenuCommandListText[88] = "Items\nEther\nTech Attack\nSkills\nCharacters\nA.G.W.S.\nBattle Formation\nU.M.N.\nGame Options";

const char TopMenuSaveLoadDescriptionText[16] = "Save/Load data.";

const char TopMenuGameOptionsDescriptionText[32] = "Set various game play options.";

const char TopMenuUmnDescriptionText[48] = "Access the U.M.N. (Unus Mundus Network).";

const char TopMenuFormationDescriptionText[48] = "Select combatants and change battle formation.";

const char TopMenuAgwsDescriptionText[56] = "Equip A.G.W.S. weapons and accessories. Assign pilots.";

const char TopMenuCharactersDescriptionText[72] = "Equip character weapons and accessories. Change on-screen characters.";

const char TopMenuSkillsDescriptionText[56] = "Set skills and extract new skills from accessories.";

const char TopMenuTechDescriptionText[32] = "Set and improve Tech Attacks.";

const char TopMenuEtherDescriptionText[64] = "Use, set, and evolve Ether. Transfer Ether to other characters.";

const char TopMenuItemsDescriptionText[80] = "Use items. View your inventory of special items, weapons, and accessories.";

const char TopMenuGameOptionsText[16] = "Game Options";

const char TopMenuBattleFormationText[24] = "Battle Formation";

const char TopMenuAgwsText[16] = "A.G.W.S.";

const char TopMenuCharactersText[16] = "Characters";

const char TopMenuTechAttacksText[16] = "Tech Attacks";

/* Text: \x01Reserve */
const char TopMenuReserveText[24] = "\001Reserve";
