#ifndef INCLUDE_MAIN_E_NUMBER_MAIN_H
#define INCLUDE_MAIN_E_NUMBER_MAIN_H

#include "shared.h"

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

typedef struct ERibbonPoint {
    short x;
    short y;
    int work;
    signed char red;
    signed char green;
    signed char blue;
    signed char alpha;
} ERibbonPoint;

typedef struct ETagFontMainData {
    signed char state;                  /* +0x00 */
    signed char subState;               /* +0x01 */
    u8 unmodeled_002[2];                /* +0x02..0x03 */
    short x;                            /* +0x04 */
    short y;                            /* +0x06 */
    int work;                           /* +0x08 */
    unsigned char color[4];             /* +0x0c..0x0f */
    struct {
        short x;
        short y;
        int work;
        unsigned char color[4];
        const char *text;               /* +0x1c */
    } render;
} ETagFontMainData;

/* One ribbon object for every ribbon screen: eRibbonSet fills its colour
 * points (set and drawn through the same record), eRibbonMain animates the
 * geometry. The record is 0x70 bytes in the windows that embed it. */
typedef struct ERibbonMainData {
    short x;                            /* +0x00 */
    short y;                            /* +0x02 */
    int work;                           /* +0x04 */
    short width;                        /* +0x08 */
    short height;                       /* +0x0a */
    unsigned char mode;                 /* +0x0c */
    unsigned char progress;             /* +0x0d */
    unsigned char visible;              /* +0x0e */
    unsigned char style;                /* +0x0f */
    ERibbonPoint points[6];             /* +0x10..0x57 */
    u8 unmodeled_058[4];                /* +0x58..0x5b */
    int renderCount;                    /* +0x5c */
    u8 unmodeled_060[0x10];             /* +0x60..0x6f */
} ERibbonMainData;

/* Print opcode 10 takes a number descriptor, opcode 2 takes a sprite
 * descriptor, and opcode 7 takes a text descriptor in the same render slots.
 * The format byte selects the active descriptor representation. */
typedef struct ENumberSpriteDescriptor {
    short x, y;
    int work;
    short width, height;
    unsigned char color[4];
    short spriteId;
    short unmodeled_012;
} ENumberSpriteDescriptor;

typedef struct ENumberRenderDescriptor {
    unsigned short x, y;
    int work;
    union {
        struct {
            signed char red, green, blue, alpha;
            int value;
        } number;
        struct {
            short width, height;
            unsigned char color[4];
        } sprite;
        struct {
            unsigned char color[4];
            const char *text;
        } text;
    } payload;
    /* Opcode 10: digit count and flags; opcode 2: sprite ID in argument1. */
    unsigned short argument1, argument2;
} ENumberRenderDescriptor;

typedef ENumberRenderDescriptor ENumberRenderArray[4];

typedef struct ENumberMainData {
    short x, y;
    int work;
    unsigned char color_r;
    unsigned char color_g;
    unsigned char color_b;
    unsigned char color_a;
    unsigned char mode;
    unsigned char unmodeled_00d;
    unsigned char format;
    unsigned char digits;
    unsigned char colorModeFirst;
    unsigned char colorModeSecond;
    unsigned char alignment;
    unsigned char firstOnly;
    /* The number to print; the text format (opcode 7) stores the address
     * of its string here instead. Callers store a plain int: a union
     * member store schedules differently (eBattleWinOpen3). */
    int value;
    ENumberRenderArray render;
    ENumberSpriteDescriptor separators[2];
} ENumberMainData;

void eNumberMain(ENumberMainData *number);

/* Callers pass a second argument, which this initializer does not read. */
/* eNumberSet, eTagFontSet and eRibbonSet take a second argument every call
 * site of this menu passes and the initialisers themselves leave unused
 * (move a1,zero at 0x002931e8, daddu $5,$0,$0 at 0x00295aa8); eTagFontSet
 * takes a pointer to the label text it draws. */
void eNumberSet(ENumberMainData *number, int unused);

void eRibbonMain(ERibbonMainData *ribbon);
void eRibbonSet(ERibbonMainData *ribbon, int style);
void eTagFontMain(ETagFontMainData *tag);
void eTagFontSet(ETagFontMainData *tag, const char *text);

#endif /* INCLUDE_MAIN_E_NUMBER_MAIN_H */
