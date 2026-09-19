#include "common.h"
#include "shared.h"
#include "xgl_prim_add.h"

INCLUDE_ASM("asm/main/nonmatchings/xgl_prim_add", xglPrimAddGifTagDirect);

void xglPrimAddGifTag(XglPrim *prim, int count) {
    xglPrimAddGifTagDirect(prim->packet, prim->data, count);
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_prim_add", xglPrimAddGouraudStripN);

INCLUDE_ASM("asm/main/nonmatchings/xgl_prim_add", xglPrimAddLineStripN);
