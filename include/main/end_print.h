#ifndef INCLUDE_MAIN_END_PRINT_H
#define INCLUDE_MAIN_END_PRINT_H

#include "shared.h"
#include "main/xgl_packet.h"

/* Sprite coordinates, color and packed UV/CLUT indices consumed by the draw routines. */
typedef struct PrintSpriteInfo {
    short x, y;
    int depth;
    short width, height;
    unsigned char color[4];
    short uv; /* low byte: UV table entry; high byte: CLUT table entry */
    unsigned char column, row;
} PrintSpriteInfo;

void endPrintInit(void);
void endPrintExtFunc(int order, int command, void *param);
void endPrintDirectFrameCopy(XglPacket *packet, int tagLow, int tagHigh);

#endif /* INCLUDE_MAIN_END_PRINT_H */
