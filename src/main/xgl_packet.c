#include "common.h"
#include "shared.h"
#include "xgl_packet.h"

static XglPacket asPacketSource[2];
extern void sceVif1PkInit(XglPacket *packet, u32 buffer_address);

INCLUDE_ASM("asm/main/nonmatchings/xgl_packet", xglPacketTextureTrans);

INCLUDE_ASM("asm/main/nonmatchings/xgl_packet", xglPacketInterpolate);

static XglPacket *pCurrentPacket;

XglPacket *xglPacketGetCurrent(void)
{
    return pCurrentPacket;
}

static XglPacket *pSendPacket;

void xglPacketInit(void)
{
    sceVif1PkInit(&asPacketSource[0], 0x00c00000);
    sceVif1PkInit(&asPacketSource[1], 0x00e00000);

    asPacketSource[0].limit = 0x00e00000;
    asPacketSource[1].limit = 0x01000000;
    asPacketSource[0].cursor = (u8 *)0x00e00000;
    asPacketSource[1].cursor = (u8 *)0x01000000;
    pCurrentPacket = &asPacketSource[0];
    pSendPacket = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_packet", xglPacketMove);

extern void xglDmaDirectSrcChain(u32 channel, u32 address);

void xglPacketSend(void)
{
    if (pSendPacket != 0) {
        xglDmaDirectSrcChain(1, (u32)pSendPacket->start);
    }
}
