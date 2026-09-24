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
    unsigned char unmodeled_00[0x14];
    unsigned short state;      /* +0x14 */
    unsigned short mode;       /* +0x16 */
    unsigned char unmodeled_18[8];
    float x;                   /* +0x20 */
    float y;                   /* +0x24 */
    float brightness;          /* +0x28 */
    unsigned char unmodeled_2c[0x54 - 0x2c];
    unsigned char max_digits;  /* +0x54 */
    unsigned char unmodeled_55[3];
    int value;                 /* +0x58 */
    unsigned char unmodeled_5c[0x186 - 0x5c];
    signed char param;         /* +0x186 */
    signed char kind;          /* +0x187 */
};

extern void TWIN_init2(void *window);

extern unsigned char *STRING_int(unsigned char *buffer, int value);

extern void STRING_h2zEUC(void *dst, const unsigned char *src);

extern void xglFontPrintf(int x, int y, unsigned int color, void *arg);

void TWIN_initCF(void *window);

void TWIN_initScene(TwinWindow2 *window);

void TSLIDER_drawDefault(TwinWindow2 *window);

#endif /* SRC_MAIN_TSLIDER_CREATE_H */
