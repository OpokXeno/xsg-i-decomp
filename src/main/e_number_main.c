#include "common.h"

#include "shared.h"

/*
 * Partial view of the object eNumberSet initialises. eNumberMain (still
 * INCLUDE_ASM in this TU) copies the four bytes at +0x08..+0x0b verbatim into
 * the same +0x20..+0x23 render colour slot that eSpriteMain fills from its
 * own colour quad (and again into +0x74..+0x77), so they are a colour quad
 * set to 0x80 per channel, not digits; the number itself is the word at
 * +0x14 that eNumberMain formats in base 10. It runs only when the low
 * nibble of mode is 3 and reads flag[0] to pick a display format; the other
 * unmodeled bytes are untouched by eNumberSet.
 */

typedef struct ENumber {
    u8 unmodeled_000[8];    /* +0x00..0x07 */
    signed char color[4];   /* +0x08..0x0b */
    signed char mode;       /* +0x0c */
    u8 unmodeled_00d[3];    /* +0x0d..0x0f */
    signed char flag[4];    /* +0x10..0x13 */
} ENumber;

/* The first 16 bytes are the fields initialized by eSpriteSet. The following
 * coordinates, work word, and color quad form the descriptor submitted by
 * eSpriteMain. */

typedef struct ESpriteMainData {
    signed char state;
    u8 unmodeled_001[3];
    short x;
    short y;
    int work;
    signed char color[4];
    u8 unmodeled_010[4];
    short renderX;
    short renderY;
    int renderWork;
    u8 unmodeled_01c[4];
    signed char renderColor[4];
} ESpriteMainData;

extern void endSpriteSet(void *sprite, int mode);

extern void endPrintExtFunc(int work, int mode, void *descriptor);

/*
 * Partial view of the object eSpriteSet initialises. eSpriteMain copies x,
 * y and work verbatim into its own render fields, and copies mode together
 * with the three following bytes as one contiguous quad into a render
 * buffer, so they are modelled here as one 4-byte color quad rather than a
 * scalar mode plus three unrelated bytes; eTagFontMain evidences the same
 * grouping at the same offsets.
 */

typedef struct ESprite {
    signed char state;      /* +0x00 */
    u8 unmodeled_001[3];    /* +0x01..0x03 */
    short x;                 /* +0x04 */
    short y;                 /* +0x06 */
    int work;                /* +0x08 */
    signed char color[4];    /* +0x0c..0x0f */
    u8 unmodeled_010[0x14]; /* +0x10..0x23 */
    short spriteId;          /* +0x24 */
} ESprite;

/*
 * Partial view of the object eRibbonModeChange writes, limited to the mode
 * field it evidences.
 */

typedef struct ERibbonEntry {
    u8 unmodeled_000[0xc]; /* +0x00..0x0b */
    signed char mode;      /* +0x0c */
} ERibbonEntry;

typedef struct ERibbonPoint {
    short x;
    short y;
    int work;
    signed char red;
    signed char green;
    signed char blue;
    signed char alpha;
} ERibbonPoint;

typedef struct ERibbonSetData {
    short x;
    short y;
    int work;
    u8 unmodeled_008[4];
    signed char mode;
    unsigned char progress;
    unsigned char visible;
    unsigned char style;
    ERibbonPoint points[6];
} ERibbonSetData;

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

/* The source color quad is copied into the descriptor with its two middle
 * channels exchanged. Opcode 7 reads the descriptor's trailing text pointer. */

typedef struct ETagFontMainData {
    u8 unmodeled_000[4];
    short x;
    short y;
    int work;
    signed char color[4];
    struct {
        short x;
        short y;
        int work;
        signed char color[4];
        const char *text;
    } render;
} ETagFontMainData;

/*
 * Partial view of the object eTagFontSet initialises. eTagFontMain copies
 * the same 4-byte color quad at +0xc..+0xf into its own render buffer that
 * eSpriteMain copies from its object (same offsets, same grouping).
 */

typedef struct ETagFont {
    signed char state;      /* +0x00 */
    signed char subState;   /* +0x01 */
    u8 unmodeled_002[10];   /* +0x02..0x0b */
    signed char color[4];    /* +0x0c..0x0f */
    u8 unmodeled_010[0xc];  /* +0x10..0x1b */
    const char *text;        /* +0x1c */
} ETagFont;


typedef struct ERibbonRenderPoint {
    short x;
    short y;
    int work;
    u8 unmodeled_008[4];
} ERibbonRenderPoint;

typedef struct ERibbonMainData {
    short x;
    short y;
    int work;
    short width;
    short height;
    signed char mode;
    unsigned char progress;
    unsigned char visible;
    unsigned char style;
    ERibbonRenderPoint points[6];
    u8 unmodeled_058[4];
    int renderCount;
} ERibbonMainData;

INCLUDE_ASM("asm/main/nonmatchings/e_number_main", eNumberMain);

void eNumberNumberChange(void)
{
}

void eNumberSet(ENumber *number)
{
    number->color[0] = number->color[1] = number->color[2] = number->color[3] = -128;
    number->mode = 3;
    number->flag[0] = number->flag[1] = number->flag[2] = number->flag[3] = 0;
}

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
    endPrintExtFunc(sprite->work, 2, &sprite->renderX);
}

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

void eRibbonMain(ERibbonMainData *ribbon)
{
    int right;

    switch ((unsigned char)ribbon->mode) {
    case 0x20:
        ribbon->points[1].x = ribbon->points[0].x = ribbon->x;
        ribbon->points[2].y = ribbon->points[0].y = ribbon->y;
        ribbon->points[3].y = ribbon->points[1].y = ribbon->y + ribbon->height;
        ribbon->points[3].work = ribbon->points[2].work = ribbon->points[0].work = ribbon->work;
        ribbon->visible = 1;
        ribbon->mode = 0x21;
        ribbon->progress = 0;
    case 0x21:
        right = (int)((float)ribbon->x + (float)ribbon->width * ((float)ribbon->progress * 0.125f));
        ribbon->progress++;
        ribbon->points[3].x = ribbon->points[2].x = right;
        if (ribbon->progress > 8)
            ribbon->mode = -16;
        break;
    case 0x2E:
        ribbon->mode = 0x2F;
    case 0:
    case 0x2F:
        ribbon->mode = 0x50;
        break;
    case 0x50:
        ribbon->mode = 0x51;
        ribbon->visible = 1;
    case 0x51:
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
    case 0xF0:
    case 0xF1:
        break;
    }
    if (ribbon->visible)
        endPrintExtFunc(ribbon->points[0].work, 3, &ribbon->points[0]);
}

void eRibbonModeChange(ERibbonEntry *entry, signed char mode)
{
    entry->mode = mode;
}

void eRibbonSet(ERibbonSetData *ribbon, int style)
{
    int point;

    ribbon->mode = 0x50;
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
    case 0:
        line->state = 0x50;
        break;
    case 1:
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
            line->state = 0x50;
        break;
    case 0x20:
        for (reset = 0; reset < line->count; reset++)
            line->progress[reset] = 0;
        line->state = 0x21;
    case 0x21:
        for (segment = 0; segment < line->count; segment++) {
            line->progress[segment]++;
            if (line->progress[segment] >= line->duration)
                line->progress[segment] = line->duration;
        }
        remaining = 0;
        for (segment = 0; segment < line->count; segment++)
            remaining += line->progress[segment] - line->duration;
        if (remaining == 0)
            line->state = 0x50;
        break;
    case 0x50:
        line->state = 0x51;
        break;
    }
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
        endPrintExtFunc(tag->work, 7, &tag->render);
    }
}

void eTagFontSet(ETagFont *tag, const char *text)
{
    tag->state = 0;
    tag->subState = 0;
    tag->color[3] = -128;
    tag->color[1] = -128;
    tag->color[2] = -128;
    tag->color[0] = -128;
    tag->text = text;
}
