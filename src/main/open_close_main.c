#include "common.h"

INCLUDE_ASM("asm/main/nonmatchings/open_close_main", OpenSubType00);

INCLUDE_ASM("asm/main/nonmatchings/open_close_main", CloseSubType00);

INCLUDE_ASM("asm/main/nonmatchings/open_close_main", OpenSubType01);

INCLUDE_ASM("asm/main/nonmatchings/open_close_main", OpenCloseMain);

INCLUDE_ASM("asm/main/nonmatchings/open_close_main", eWindowMain);

/*
 * Partial view of the object eWindowSet initialises. eWindowMain (still
 * INCLUDE_ASM in this TU) reads x/y/width/height back as the low 16 bits of
 * these words and work as a full word, copying all five into a render packet
 * at +0x1c..+0x27; it also dispatches on mode through a dozen-plus state
 * constants and gates that same packet's use on visible. flag is cleared here
 * but not read by any function in this TU.
 */
typedef struct EWindow {
    int x;               /* +0x00 */
    int y;                /* +0x04 */
    int work;             /* +0x08 */
    int width;             /* +0x0c */
    int height;             /* +0x10 */
    signed char color[4];   /* +0x14..0x17 */
    signed char mode;        /* +0x18 */
    signed char flag;         /* +0x19 */
    signed char visible;       /* +0x1a */
} EWindow;

void eWindowSet(EWindow *window)
{
    window->color[3] = -128;
    window->height = 0x10;
    window->flag = 0;
    window->x = 0;
    window->y = 0;
    window->work = 0;
    window->color[0] = -128;
    window->color[1] = -128;
    /* Kept together: without this grouping cc1 hoists color[2]/width/mode to
     * the front of the function instead of storing them here. */
    do {
        window->color[2] = -128;
        window->width = 0x10;
    } while (0);
    window->visible = 0;
    window->mode = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/open_close_main", eCursolMain);

INCLUDE_ASM("asm/main/nonmatchings/open_close_main", eCursolModeChange);

/*
 * Partial view of the object eCursolSet initialises. eCursolMain (still
 * INCLUDE_ASM in this TU) dispatches on state through a jump table, gates its
 * render call on active, advances timer once per call to drive a blink
 * pattern, and uses width in the cursor's box-position arithmetic; x, y and
 * work are copied into an embedded sprite record at +0x10 the same way
 * eSpriteSet's own x/y/work are used (src/main/e_number_main.c), and color is
 * the same four-channel quad convention as that file's other e* widgets.
 */
typedef struct ECursol {
    signed char state;    /* +0x00 */
    signed char active;    /* +0x01 */
    signed char timer;      /* +0x02 */
    signed char width;       /* +0x03 */
    short x;                  /* +0x04 */
    short y;                   /* +0x06 */
    int work;                   /* +0x08 */
    signed char color[4];        /* +0x0c..0x0f */
} ECursol;

void eCursolSet(ECursol *cursol, signed char width)
{
    cursol->width = width;
    cursol->color[0] = -128;
    cursol->x = 0;
    cursol->y = 0;
    cursol->work = 0;
    cursol->color[3] = -128;
    cursol->color[2] = -128;
    /* Kept together: matches eWindowSet's color[2]/width barrier (this file,
     * CP-0337); without the do/while(0) barrier around color[1]/state/timer
     * cc1 hoists this run to the front instead (measured:
     * attempt-95dd36f8157e build/form-03, first difference 0x00000e8c,
     * 55.357%; build/form-04, byte-identical, 100%). */
    do {
        cursol->color[1] = -128;
        cursol->state = 0;
        cursol->timer = 0;
    } while (0);
    cursol->active = 0;
}
