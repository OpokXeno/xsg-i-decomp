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

typedef union ENumberValue {
    int integer;
    const char *text;
} ENumberValue;

typedef struct ENumberMainData {
    unsigned short x, y;
    int work;
    unsigned char color[4];
    unsigned char mode;
    unsigned char unmodeled_00d;
    unsigned char format;
    unsigned char digits;
    unsigned char colorMode[2];
    unsigned char alignment;
    unsigned char firstOnly;
    ENumberValue value;
    ENumberRenderArray render;
    ENumberSpriteDescriptor separators[2];
} ENumberMainData;

void eNumberMain(ENumberMainData *number);

/* Callers pass a second argument, which this initializer does not read. */
void eNumberSet(void *data, int unused);

void eRibbonMain(ERibbonMainData *ribbon);
void eRibbonSet(ERibbonSetData *ribbon, int style);
void eTagFontMain(ETagFontMainData *tag);
void eTagFontSet(ETagFont *tag, const char *text);

#endif /* INCLUDE_MAIN_E_NUMBER_MAIN_H */
