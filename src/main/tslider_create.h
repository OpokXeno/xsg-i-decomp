/*
 * TU-local declarations of main/tu144 (src/main/tslider_create.c).
 */

#ifndef SRC_MAIN_TSLIDER_CREATE_H
#define SRC_MAIN_TSLIDER_CREATE_H

#include "shared.h"

typedef struct TwinWindow TwinWindow;

/*
 * The TWIN_create2 window object, as far as this TU's accepted code
 * touches it. createItemGetWin returns the address of `body` (+0x14);
 * taskItemGet (main:0x002c6604) keeps that pointer and later compares its
 * first halfword to 2 (main:0x002c6628), so MSG_print2 (called on the
 * window right after with the same text/length pair) writes through it.
 * TWIN_initScene (this TU, still ASM, main:0x00260600) writes the same
 * +0x186/+0x187 pair with 0x30/3 for the scene-VM window, so `kind` marks
 * which TWIN_init* built the window and `param` is a kind-specific value
 * rather than a fixed-role field. Nothing before +0x14 or between `body`
 * and +0x186 is touched here.
 */
struct TwinWindow {
    unsigned char unmodeled_00[0x14];
    unsigned char body[0x172];
    signed char param;  /* +0x186 */
    signed char kind;   /* +0x187 */
};

extern TwinWindow *TWIN_create2(int component_id);

extern void MSG_print2(TwinWindow *window, const char *text, int length);

extern unsigned int strlen(const char *string);

void *createItemGetWin(const char *text);

/*
 * The same TWIN_create2 object as TwinWindow (createItemGetWin returns
 * &window->body at the same +0x14 both types agree on); named separately
 * because TwinWindow already names that region as one opaque `body` array,
 * so it is left untouched here. TWIN_initScene and TSLIDER_drawDefault
 * evidence these members inside it: state (+0x14) is
 * the halfword taskItemGet compares to 2 (main:0x002c6628), and
 * TSLIDER_drawDefault treats it as a wrapping counter (draws unless state
 * is 1 or 2). mode (+0x16) low byte: TWIN_initScene sets it to 4 for the
 * scene-VM window (TWIN_initCF sets the CF path's window to 3,
 * main:0x002605d8). x/y (+0x20/+0x24) are the on-screen origin:
 * TWIN_initScene sets them to (0, 324) for the scene-VM window, and
 * TSLIDER_drawDefault offsets its printed text from them. brightness
 * (+0x28) is subtracted from a fixed 0x01FFFFF0 color mask when
 * TSLIDER_drawDefault prints. max_digits (+0x54) and value (+0x58) drive
 * the right-aligned slider value text: value is converted to decimal by
 * STRING_int, and the padding printed before it is (max_digits - digit
 * count) * 0x14. param/kind at +0x186/+0x187 match TwinWindow
 * (TWIN_initScene writes 0x30/3 for the scene-VM window).
 */
typedef struct TwinWindow2 TwinWindow2;
struct TwinWindow2 {
    unsigned char unmodeled_00[0x0c];
    unsigned short width;      /* +0x0c */
    unsigned short height;     /* +0x0e */
    unsigned int flags;        /* +0x10 */
    unsigned short state;      /* +0x14 */
    unsigned short mode;       /* +0x16 */
    unsigned char unmodeled_18[8];
    float x;                   /* +0x20 */
    float y;                   /* +0x24 */
    float brightness;          /* +0x28 */
    unsigned char unmodeled_2c[4];
    short timer;               /* +0x30 */
    unsigned char unmodeled_32[0x40 - 0x32];
    int valueOffset;           /* +0x40 */
    unsigned char unmodeled_44[0x50 - 0x44];
    unsigned int options;      /* +0x50 */
    unsigned char max_digits;  /* +0x54 */
    unsigned char unmodeled_55[3];
    int value;                 /* +0x58 */
    int min_value;             /* +0x5c */
    int max_value;             /* +0x60 */
    unsigned char unmodeled_64[0x186 - 0x64];
    signed char param;         /* +0x186 */
    signed char kind;          /* +0x187 */
};

extern void TWIN_init2(void *window);
extern TwinWindow2 *TWSYS_createComponent(int slot, int type);

/* This TU reads two consecutive PadData halfwords at +0x2a and +0x2c. */
typedef struct TSliderPadData {
    unsigned char unmodeled_00[0x2a];
    unsigned short pressed; /* +0x2a */
    unsigned short repeat;  /* +0x2c */
} TSliderPadData;
extern TSliderPadData PadData;

#define PAD_CIRCLE 0x0020
#define PAD_CROSS  0x0040
#define PAD_UP     0x1000
#define PAD_RIGHT  0x2000
#define PAD_DOWN   0x4000
#define PAD_LEFT   0x8000

extern unsigned char *STRING_int(unsigned char *buffer, int value);

extern void STRING_h2zEUC(void *dst, const unsigned char *src);

extern void xglFontPrintf(int x, int y, unsigned int color, void *arg);

void TWIN_initCF(void *window);

void TWIN_initScene(TwinWindow2 *window);

void TSLIDER_drawDefault(TwinWindow2 *window);

/* The TMENU_addQuery2 view of MenuNative's query text and row storage. */
typedef struct MenuNative {
    unsigned char unmodeled_00[0x0c];
    short width;                   /* +0x0c */
    unsigned short height;         /* +0x0e */
    unsigned char unmodeled_10[0x57 - 0x10];
    unsigned char rowWidth;        /* +0x57 */
    unsigned char unmodeled_58[0x141 - 0x58];
    unsigned char rowCount;        /* +0x141 */
    unsigned char selectedRow;     /* +0x142 */
    unsigned char scroll;          /* +0x143 */
    unsigned char **rows;          /* +0x144 */
    unsigned char *textStart;      /* +0x148 */
    unsigned char unmodeled_14c[0x154 - 0x14c];
    unsigned char *textEnd;        /* +0x154 */
} MenuNative;

extern void TMENU_addItem(MenuNative *menu, const char *text);
void TMENU_addQuery2(MenuNative *menu, char **texts, int count);

#endif /* SRC_MAIN_TSLIDER_CREATE_H */
