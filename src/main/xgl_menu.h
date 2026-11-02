/*
 * TU-local declarations of main/tu091 (src/main/xgl_menu.c).
 */

#ifndef SRC_MAIN_XGL_MENU_H
#define SRC_MAIN_XGL_MENU_H

#include "shared.h"
#include "main/xgl_menu.h"
#include "main/xgl_font.h"

#define EE_SCRATCHPAD_BASE ((XglMenuDrawContext *)0x70000000)
#define MENU_LIST_INDENT 8
#define MENU_TABLE_COUNT 16

extern XglMenuEntry menutbl[MENU_TABLE_COUNT];

#endif
