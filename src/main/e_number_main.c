#include "common.h"
#include "shared.h"

INCLUDE_ASM("asm/main/nonmatchings/e_number_main", eNumberMain);

void eNumberNumberChange(void)
{
}

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

void eNumberSet(ENumber *number)
{
    number->color[0] = number->color[1] = number->color[2] = number->color[3] = -128;
    number->mode = 3;
    number->flag[0] = number->flag[1] = number->flag[2] = number->flag[3] = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/e_number_main", eSpriteMain);

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

INCLUDE_ASM("asm/main/nonmatchings/e_number_main", eRibbonMain);

/*
 * Partial view of the object eRibbonModeChange writes, limited to the mode
 * field it evidences.
 */
typedef struct ERibbonEntry {
    u8 unmodeled_000[0xc]; /* +0x00..0x0b */
    signed char mode;      /* +0x0c */
} ERibbonEntry;

void eRibbonModeChange(ERibbonEntry *entry, signed char mode)
{
    entry->mode = mode;
}

INCLUDE_ASM("asm/main/nonmatchings/e_number_main", eRibbonSet);

INCLUDE_ASM("asm/main/nonmatchings/e_number_main", eLineMain);

INCLUDE_ASM("asm/main/nonmatchings/e_number_main", eLineSet);

INCLUDE_ASM("asm/main/nonmatchings/e_number_main", eTagFontMain);

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
    int value;               /* +0x1c */
} ETagFont;

void eTagFontSet(ETagFont *tag, int value)
{
    tag->state = 0;
    tag->subState = 0;
    tag->color[3] = -128;
    tag->color[1] = -128;
    tag->color[2] = -128;
    tag->color[0] = -128;
    tag->value = value;
}
