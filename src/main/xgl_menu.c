#include "common.h"
#include "shared.h"
#include "xgl_menu.h"

INCLUDE_ASM("asm/main/nonmatchings/xgl_menu", xglMenuOpen);

static void xglMenuDrawType0(XglMenuEntry *entry) {
    entry->active = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_menu", xglMenuDrawType1Sub);

INCLUDE_ASM("asm/main/nonmatchings/xgl_menu", xglMenuDrawType1);

INCLUDE_ASM("asm/main/nonmatchings/xgl_menu", xglMenuDraw);

INCLUDE_ASM("asm/main/nonmatchings/xgl_menu", xglMenuInitial);
