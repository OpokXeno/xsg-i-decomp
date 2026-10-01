#include "common.h"
#include "shared.h"
#include "xgl_prim_add.h"

#include "main/xgl_packet.h"

#define NULL ((void *)0)

extern void sceVif1PkCnt(XglPacket *packet, int count);
extern void sceVif1PkAddCode(XglPacket *packet, unsigned int code);
extern void sceVif1PkOpenDirectHLCode(XglPacket *packet, int mode);
extern void sceVif1PkAddDirectDataN(XglPacket *packet, const void *data, int count);
extern void sceVif1PkCloseDirectHLCode(XglPacket *packet);

#define XGL_PRIM_ADD_MODE_INDEX8 0
#define XGL_PRIM_ADD_MODE_INDEX16 1
#define XGL_PRIM_ADD_MODE_DIRECT 2
#define XGL_PRIM_ADD_FLAG_POSITION_OFFSET 0x10
#define XGL_PRIM_ADD_FLAG_FLUSH_DIRECT 0x20
#define XGL_PRIM_ADD_POSITION_Z_MASK 0x00ffffffu

void xglPrimAddGifTagDirect(XglPacket *packet, const void *data, int count) {
    if (packet == NULL) {
        packet = xglPacketGetCurrent();
    }

    if (packet->directCode != 0 && packet->directState != 0) {
        sceVif1PkAddDirectDataN(packet, data, count);
        return;
    }

    sceVif1PkCnt(packet, 0);
    sceVif1PkAddCode(packet, 0x11000000);
    sceVif1PkOpenDirectHLCode(packet, 0);
    sceVif1PkAddDirectDataN(packet, data, count);
    sceVif1PkCloseDirectHLCode(packet);
}

void xglPrimAddGifTag(XglPrim *prim, int count) {
    xglPrimAddGifTagDirect(prim->packet, prim->data, count);
}

void xglPrimAddGouraudStripN(XglPrim *prim, int vertex_count)
{
    u32 *output_buffer;
    u32 *output;
    XglPrimAddPosition position_offset;
    const XglPrimAddPosition *positions;
    const XglPrimAddColor *colors;
    int mode;
    int index;

    output_buffer = prim->output_buffer;
    output = output_buffer;
    if (prim->flags & XGL_PRIM_ADD_FLAG_FLUSH_DIRECT) {
        xglPrimAddGifTagDirect(prim->packet, prim->data, prim->direct_count);
        output_buffer = prim->output_buffer;
    }

    output[0] = (u32)vertex_count + 0x8000u;
    output[1] = ((u32)prim->tag_control << 18) ^ 0x20064000u;
    output[2] = 81;
    output[3] = 0;
    output += 4;

    positions = prim->positions;
    colors = prim->colors;

    if (prim->flags & XGL_PRIM_ADD_FLAG_POSITION_OFFSET) {
        position_offset.component[0] = prim->position_offset.component[0];
        position_offset.component[1] = prim->position_offset.component[1];
        position_offset.component[2] = prim->position_offset.component[2];
        position_offset.component[3] = prim->position_offset.component[3];
    } else {
        position_offset.component[0] = 0;
        position_offset.component[1] = 0;
        position_offset.component[2] = 0;
        position_offset.component[3] = 0;
    }

    mode = prim->flags & 0x0f;
    switch (mode) {
    case XGL_PRIM_ADD_MODE_INDEX8: {
        const u8 *position_index = prim->position_indices;
        const u8 *color_index = prim->color_indices;

        index = 0;
        if (vertex_count > 0) {
            do {
                const XglPrimAddColor *color = &colors[*color_index++];
                const XglPrimAddPosition *position;

                output[0] = color->channel[0];
                output[1] = color->channel[1];
                output[2] = color->channel[2];
                output[3] = color->channel[3];
                position = &positions[*position_index++];
                output[4] = position->component[0] + position_offset.component[0];
                output[5] = position->component[1] + position_offset.component[1];
                output[6] = (position->component[2] + position_offset.component[2]) & XGL_PRIM_ADD_POSITION_Z_MASK;
                output[7] = position->component[3] + position_offset.component[3];
                output += 8;
                ++index;
            } while (index < vertex_count);
        }
        break;
    }
    case XGL_PRIM_ADD_MODE_INDEX16: {
        const u16 *position_index = prim->position_indices;
        const u16 *color_index = prim->color_indices;

        index = 0;
        if (vertex_count > 0) {
            do {
                const XglPrimAddColor *color = &colors[*color_index++];
                const XglPrimAddPosition *position = &positions[*position_index++];

                output[0] = color->channel[0];
                output[1] = color->channel[1];
                output[2] = color->channel[2];
                output[3] = color->channel[3];
                output[4] = position->component[0] + position_offset.component[0];
                output[5] = position->component[1] + position_offset.component[1];
                output[6] = (position->component[2] + position_offset.component[2]) & XGL_PRIM_ADD_POSITION_Z_MASK;
                output[7] = position->component[3] + position_offset.component[3];
                output += 8;
                ++index;
            } while (index < vertex_count);
        }
        break;
    }
    case XGL_PRIM_ADD_MODE_DIRECT: {
        if (vertex_count > 0) {
            index = vertex_count;

            do {
                --index;
                output[0] = colors->channel[0];
                output[1] = colors->channel[1];
                output[2] = colors->channel[2];
                output[3] = colors->channel[3];
                output[4] = positions->component[0] + position_offset.component[0];
                output[5] = positions->component[1] + position_offset.component[1];
                output[6] = (positions->component[2] + position_offset.component[2]) & XGL_PRIM_ADD_POSITION_Z_MASK;
                output[7] = positions->component[3] + position_offset.component[3];
                ++colors;
                ++positions;
                output += 8;
            } while (index != 0);
        }
        break;
    }
    default:
        break;
    }

    xglPrimAddGifTagDirect(prim->packet, output_buffer, vertex_count * 2 + 1);
}

void xglPrimAddLineStripN(XglPrim *prim, int vertex_count)
{
    u32 *output_buffer;
    u32 *output;
    XglPrimAddPosition position_offset;
    const XglPrimAddPosition *positions;
    const XglPrimAddColor *colors;
    int mode;
    int index;

    output_buffer = prim->output_buffer;
    output = output_buffer;
    if (prim->flags & XGL_PRIM_ADD_FLAG_FLUSH_DIRECT) {
        xglPrimAddGifTagDirect(prim->packet, prim->data, prim->direct_count);
        output_buffer = prim->output_buffer;
    }

    positions = prim->positions;
    colors = prim->colors;

    output[0] = 1;
    output[1] = ((u32)prim->tag_control << 18) ^ 0x10014000u;
    output[2] = 1;
    output[3] = 0;

    output[4] = colors[0].channel[0];
    output[5] = colors[0].channel[1];
    output[6] = colors[0].channel[2];
    output[7] = colors[0].channel[3];
    output += 8;

    output[0] = (u32)vertex_count + 0x8000u;
    output[1] = 0x10000000u;
    output[2] = 5;
    output[3] = 0;
    output += 4;

    if (prim->flags & XGL_PRIM_ADD_FLAG_POSITION_OFFSET) {
        position_offset.component[0] = prim->position_offset.component[0];
        position_offset.component[1] = prim->position_offset.component[1];
        position_offset.component[2] = prim->position_offset.component[2];
        position_offset.component[3] = prim->position_offset.component[3];
    } else {
        position_offset.component[0] = 0;
        position_offset.component[1] = 0;
        position_offset.component[2] = 0;
        position_offset.component[3] = 0;
    }

    mode = prim->flags & 0x0f;
    switch (mode) {
    case XGL_PRIM_ADD_MODE_INDEX8: {
        const u8 *position_index = prim->position_indices;

        index = 0;
        if (vertex_count > 0) {
            do {
                const XglPrimAddPosition *position = &positions[*position_index++];

                output[0] = position->component[0] + position_offset.component[0];
                output[1] = position->component[1] + position_offset.component[1];
                output[2] = (position->component[2] + position_offset.component[2]) & XGL_PRIM_ADD_POSITION_Z_MASK;
                output[3] = position->component[3] + position_offset.component[3];
                output += 4;
                ++index;
            } while (index < vertex_count);
        }
        break;
    }
    case XGL_PRIM_ADD_MODE_INDEX16: {
        const u16 *position_index = prim->position_indices;

        index = 0;
        if (vertex_count > 0) {
            do {
            const XglPrimAddPosition *position = &positions[*position_index++];

                output[0] = position->component[0] + position_offset.component[0];
                output[1] = position->component[1] + position_offset.component[1];
                output[2] = (position->component[2] + position_offset.component[2]) & XGL_PRIM_ADD_POSITION_Z_MASK;
                output[3] = position->component[3] + position_offset.component[3];
                output += 4;
                ++index;
            } while (index < vertex_count);
        }
        break;
    }
    case XGL_PRIM_ADD_MODE_DIRECT: {
        if (vertex_count > 0) {
            index = vertex_count;

            do {
                --index;
                output[0] = positions->component[0] + position_offset.component[0];
                output[1] = positions->component[1] + position_offset.component[1];
                output[2] = (positions->component[2] + position_offset.component[2]) & XGL_PRIM_ADD_POSITION_Z_MASK;
                output[3] = positions->component[3] + position_offset.component[3];
                ++positions;
                output += 4;
            } while (index != 0);
        }
        break;
    }
    default:
        break;
    }

    xglPrimAddGifTagDirect(prim->packet, output_buffer, vertex_count + 3);
}
