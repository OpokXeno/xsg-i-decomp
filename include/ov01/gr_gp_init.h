#ifndef INCLUDE_OV01_GR_GP_INIT_H
#define INCLUDE_OV01_GR_GP_INIT_H

#include "shared.h"

/* grGpInit's argument: the VIF1 DIRECT packet grPacketSend transmits, a
 * data buffer and its length. grGpInit points data at the scratchpad
 * (0x70000000). */
typedef struct GrPacket {
    void *data;  /* +0x0 */
    int count;   /* +0x4 */
} GrPacket;

/* grPutF2's line: two end points and one shared RGBA color. */
typedef struct GrF2Vertices {
    Vector4 points[2];      /* +0x00 */
    unsigned char color[4]; /* +0x20: R, G, B, A */
} GrF2Vertices;

/* Functions and data used by other translation units. */
void grGpInit(GrPacket *packet);
void grOpenF2(GrPacket *packet);
void grPutF2(GrPacket *packet, GrF2Vertices *line, const Matrix4 matrix);
void grCloseF2(void);
void grPacketSend(GrPacket *packet);

#endif /* INCLUDE_OV01_GR_GP_INIT_H */
