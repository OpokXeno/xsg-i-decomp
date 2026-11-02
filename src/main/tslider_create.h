/*
 * TU-local declarations of main/tu144 (src/main/tslider_create.c).
 */

#ifndef SRC_MAIN_TSLIDER_CREATE_H
#define SRC_MAIN_TSLIDER_CREATE_H

#include "shared.h"
#include "main/twsys_init.h"

#define PAD_CIRCLE 0x0020
#define PAD_CROSS  0x0040
#define PAD_UP     0x1000
#define PAD_RIGHT  0x2000
#define PAD_DOWN   0x4000
#define PAD_LEFT   0x8000

/* This TU reads two consecutive PadData halfwords at +0x2a and +0x2c. */
typedef struct TSliderPadData {
    unsigned char unmodeled_00[0x2a];
    unsigned short pressed; /* +0x2a */
    unsigned short repeat;  /* +0x2c */
} TSliderPadData;

/* Partial view of MenuNativeQueryView (src/main/menu_2.c): the query text and row
 * storage TMENU_addQuery2 uses. Merging it into MenuNativeQueryView changes this TU's
 * .text (GCC 2.96 alias sets), so it stays a separate view. */
typedef struct MenuNativeQueryView {
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
} MenuNativeQueryView;

extern TSliderPadData PadData;

extern void MSG_print2(TComponent *window, const char *text, int length);
extern unsigned int strlen(const char *string);
void *createItemGetWin(const char *text);
extern void TWIN_init2(void *window);
extern void STRING_h2zEUC(void *dst, const unsigned char *src);
extern void xglFontPrintf(int x, int y, unsigned int color, void *arg);
void TWIN_initCF(void *window);
void TWIN_initScene(TComponent *window);
void TSLIDER_drawDefault(TComponent *window);
extern void TMENU_addItem(MenuNativeQueryView *menu, const char *text);
void TMENU_addQuery2(MenuNativeQueryView *menu, char **texts, int count);

#endif /* SRC_MAIN_TSLIDER_CREATE_H */
