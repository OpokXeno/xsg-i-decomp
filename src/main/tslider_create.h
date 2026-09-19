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

extern void TWIN_initCF(TwinWindow *window);

extern void MSG_print2(TwinWindow *window, const char *text, int length);

extern unsigned int strlen(const char *string);

void *createItemGetWin(const char *text);

#endif /* SRC_MAIN_TSLIDER_CREATE_H */
