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

INCLUDE_ASM("asm/main/nonmatchings/xgl_prim_add", xglPrimAddGouraudStripN);

INCLUDE_ASM("asm/main/nonmatchings/xgl_prim_add", xglPrimAddLineStripN);
