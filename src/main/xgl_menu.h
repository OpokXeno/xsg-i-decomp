/*
 * TU-local declarations of main/tu091 (src/main/xgl_menu.c).
 */

#ifndef SRC_MAIN_XGL_MENU_H
#define SRC_MAIN_XGL_MENU_H

#include "shared.h"

/*
 * Partial view of one entry of the menutbl array (main 0x0021ce18/0x0021ce7c,
 * a 0x80-byte stride): xglMenuDrawType0 only touches the active flag at
 * offset 7, which xglMenuDraw (main 0x0021ce30) reads first and skips the
 * entry when it is zero.
 */
typedef struct {
    unsigned char unmodeled_00[7];
    unsigned char active;               /* +0x07 */
    /*
     * xglMenuInitial (main 0x0021ce98) indexes menutbl by this stride to
     * clear the active flag of every record; the rest of the record is not
     * recovered.
     */
    unsigned char unmodeled_08[0x78];
} XglMenuEntry;

#define MENU_TABLE_COUNT 16

extern XglMenuEntry menutbl[MENU_TABLE_COUNT];

#endif
