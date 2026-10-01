/*
 * TU-local declarations of main/tu096 (src/main/xgl_prim_add.c).
 */

#ifndef SRC_MAIN_XGL_PRIM_ADD_H
#define SRC_MAIN_XGL_PRIM_ADD_H

#include "shared.h"

typedef struct {
    int component[4];
} XglPrimAddPosition;

typedef struct {
    u8 channel[4];
} XglPrimAddColor;

/*
 * Partial view of the primitive descriptor xglPrimAddGifTag receives: it
 * touches only the packet pointer at offset 0 and the data pointer at
 * offset 0xc; the span between them is untouched by this file and stays
 * unmodeled.
 */
typedef struct {
    XglPacket *packet;                  /* +0x00 */
    u32 *output_buffer;                   /* +0x04 */
    u8 flags;                             /* +0x08 */
    u8 tag_control;                       /* +0x09 */
    s16 direct_count;                     /* +0x0a */
    const void *data;                   /* +0x0c */
    XglPrimAddPosition position_offset;  /* +0x10 */
    const XglPrimAddPosition *positions;  /* +0x20 */
    unsigned char unmodeled_24[4];
    const XglPrimAddColor *colors;        /* +0x28 */
    const void *position_indices;        /* +0x2c */
    unsigned char unmodeled_30[4];
    const void *color_indices;           /* +0x34 */
} XglPrim;

extern void xglPrimAddGifTagDirect(XglPacket *packet, const void *data, int count);

#endif
