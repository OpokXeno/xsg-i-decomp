/*
 * TU-local declarations of main/tu096 (src/main/xgl_prim_add.c).
 */

#ifndef SRC_MAIN_XGL_PRIM_ADD_H
#define SRC_MAIN_XGL_PRIM_ADD_H

#include "shared.h"

/*
 * Partial view of the primitive descriptor xglPrimAddGifTag receives: it
 * touches only the packet pointer at offset 0 and the data pointer at
 * offset 0xc; the span between them is untouched by this file and stays
 * unmodeled.
 */
typedef struct {
    XglPacket *packet;                  /* +0x00 */
    unsigned char unmodeled_04[8];
    const void *data;                   /* +0x0c */
} XglPrim;

extern void xglPrimAddGifTagDirect(XglPacket *packet, const void *data, int count);

#endif
