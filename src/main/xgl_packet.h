/*
 * TU-local declarations of main/tu103 (src/main/xgl_packet.c).
 */

#ifndef SRC_MAIN_XGL_PACKET_H
#define SRC_MAIN_XGL_PACKET_H

#include "shared.h"
#include "main/xgl_packet.h"

/* One texture entry; the original loop advances by 20 bytes. */
typedef struct XglTextureTransEntry {
    u32 size;
    short height;
    unsigned char unmodeled_06[2];
    u32 vramOffset;
    u32 dataSize;
    u32 dataOffset;
} XglTextureTransEntry;

/* Fields read from the render state by the selected packet functions. */
struct XglPacketRenderState {
    unsigned char unmodeled_00[0x1a];
    unsigned short textureBufferBase;
    unsigned char unmodeled_1c[4];
    unsigned short displayBufferBase;
};

#endif /* SRC_MAIN_XGL_PACKET_H */
