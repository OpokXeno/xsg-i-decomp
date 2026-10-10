#include "common.h"

#include "shared.h"

#include "xgl_packet.h"

static XglPacket asPacketSource[2];

extern void sceVif1PkInit(XglPacket *packet, u32 buffer_address);

static XglPacket *pCurrentPacket;

static XglPacket *pSendPacket;

extern void xglDmaDirectSrcChain(u32 channel, u32 address);

extern struct XglPacketRenderState sRender;

extern void sceVif1PkCnt(XglPacket *packet, int count);

extern void sceVif1PkAddCode(XglPacket *packet, u32 code);

extern void sceVif1PkOpenDirectCode(XglPacket *packet, int mode);

extern void sceVif1PkOpenDirectHLCode(XglPacket *packet, int mode);

extern void sceVif1PkAddDirectDataN(XglPacket *packet, const void *data, int count);

extern void sceVif1PkCloseDirectCode(XglPacket *packet);

extern void sceGsSetDefLoadImage(XglLoadImage *image, short address, short width, short psm, short x, short y, short w, short h);

/* Direct-mode GS setup block: four quadwords, then (from +0x50) seven more. */

static u64 TestPrim_0_004A8D00[24] = {
    0xb0ab400000008001ULL, 0x0000053531eeeeeeULL,
    0x0000000000000000ULL, 0x000000000000003fULL,
    0x000006fc007fc00aULL, 0x0000000000000008ULL,
    0x0000000000000060ULL, 0x0000000000000014ULL,
    0x0000000000000000ULL, 0x0000000000000006ULL,
    0x0000000000031001ULL, 0x0000000000000047ULL,
    0x0000004000000064ULL, 0x0000000000000042ULL,
    0x0000008000000080ULL, 0x0000008000000080ULL,
    0x0000001000000000ULL, 0x0000000000000000ULL,
    0x000071f700006ff8ULL, 0x0000000000000000ULL,
    0x00001c1000002000ULL, 0x0000000000000000ULL,
    0x00008df700008ff8ULL, 0x0000000000000000ULL,
};

#define XGL_SCRATCH ((u64 *)0x70000000)

extern void sceVif1PkEnd(XglPacket *packet, int mode);

extern void sceVif1PkTerminate(XglPacket *packet);

extern void sceVif1PkReset(XglPacket *packet);

extern void FlushCache(int mode);

/* Direct-mode GS setup block: four quadwords, then (from +0x50) seven more. */

void xglPacketTextureTrans(XglTextureTransSet *set)
{
    XglLoadImage image;
    XglTextureTransEntry *entry;
    int i;

    entry = (XglTextureTransEntry *)((u8 *)set + set->entryOffset);
    for (i = 0; i < set->count; i++) {
        int width;
        u32 packed;
        unsigned short w;
        int address;

        sceVif1PkCnt(pCurrentPacket, 0);
        sceVif1PkAddCode(pCurrentPacket, 0x11000000);
        address = (sRender.textureBufferBase << 11) + entry->vramOffset;
        packed = entry->size;
        w = packed;
        if (packed == w) {
            width = (w + 63) >> 6;
        } else {
            width = packed >> 16;
        }
        sceGsSetDefLoadImage(&image,
                             address >> 6,
                             width, 0, 0, 0, (short)w, entry->height);
        image.giftag |= 0x8000;
        sceVif1PkOpenDirectCode(pCurrentPacket, 0);
        sceVif1PkAddDirectDataN(pCurrentPacket, &image, 5);
        sceVif1PkCloseDirectCode(pCurrentPacket);
        sceVif1PkRef(pCurrentPacket, (u8 *)set + entry->dataOffset, entry->dataSize, 0, 0, 0);
        entry++;
    }
}

void xglPacketInterpolate(void)
{
    XglPacket *packet = pCurrentPacket;
    u64 *scratch;

    sceVif1PkCnt(packet, 0);
    sceVif1PkAddCode(packet, 0x11000000);
    sceVif1PkOpenDirectHLCode(packet, 0);
    sceVif1PkAddDirectDataN(packet, TestPrim_0_004A8D00, 4);
    scratch = XGL_SCRATCH;
    scratch[0] = 0x24020000 | (sRender.displayBufferBase << 5) | ((u64)0xc800 << 19);
    scratch[1] = 6;
    sceVif1PkAddDirectDataN(packet, XGL_SCRATCH, 1);
    sceVif1PkAddDirectDataN(packet, &TestPrim_0_004A8D00[10], 7);
    sceVif1PkCloseDirectCode(packet);
}

XglPacket *xglPacketGetCurrent(void)
{
    return pCurrentPacket;
}

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

void xglPacketMove(void)
{
    u8 **cursor;

    sceVif1PkEnd(pCurrentPacket, 0);
    sceVif1PkTerminate(pCurrentPacket);
    FlushCache(0);
    /* The store through a plain pointer may alias pCurrentPacket, which the
     * original reloads before publishing it as pSendPacket. */
    cursor = &pCurrentPacket->cursor;
    *cursor = (u8 *)pCurrentPacket->limit;
    pSendPacket = pCurrentPacket;
    if (pCurrentPacket == &asPacketSource[1]) {
        pCurrentPacket = &asPacketSource[0];
    } else {
        pCurrentPacket = &asPacketSource[1];
    }
    sceVif1PkReset(pCurrentPacket);
}

void xglPacketSend(void)
{
    if (pSendPacket != 0) {
        xglDmaDirectSrcChain(1, (u32)pSendPacket->start);
    }
}
