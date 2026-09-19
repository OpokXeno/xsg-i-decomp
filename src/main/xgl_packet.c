#include "common.h"
#include "shared.h"
#include "xgl_packet.h"

INCLUDE_ASM("asm/main/nonmatchings/xgl_packet", xglPacketTextureTrans);

INCLUDE_ASM("asm/main/nonmatchings/xgl_packet", xglPacketInterpolate);

extern XglPacket *pCurrentPacket;

XglPacket *xglPacketGetCurrent(void)
{
    return pCurrentPacket;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_packet", xglPacketInit);

INCLUDE_ASM("asm/main/nonmatchings/xgl_packet", xglPacketMove);

extern XglPacket *pSendPacket;
extern void xglDmaDirectSrcChain(u32 channel, u32 address);

void xglPacketSend(void)
{
    if (pSendPacket != 0) {
        xglDmaDirectSrcChain(1, (u32)pSendPacket->start);
    }
}
