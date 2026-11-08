#include "common.h"
#include "shared.h"
#include "main/xgl_packet.h"
#include "xgl_packet.h"
#include "main/xgl_dma.h"

#define XGL_SCRATCH ((u64 *)SCRATCHPAD_BASE)

/* The two VIF1 packet buffers: each is 2 MiB of main memory, filled forwards from its
 * base and, for the user-visible cursor, backwards from its limit. */
#define XGL_PACKET_BUFFER_0 0x00c00000
#define XGL_PACKET_BUFFER_1 0x00e00000
#define XGL_PACKET_BUFFER_END 0x01000000

extern struct XglPacketRenderState sRender;

extern void sceVif1PkInit(XglPacket *packet, u32 buffer_address);
extern void sceVif1PkCnt(XglPacket *packet, int count);
extern void sceVif1PkAddCode(XglPacket *packet, u32 code);
extern void sceVif1PkOpenDirectCode(XglPacket *packet, int mode);
extern void sceVif1PkOpenDirectHLCode(XglPacket *packet, int mode);
extern void sceVif1PkAddDirectDataN(XglPacket *packet, const void *data, int count);
extern void sceVif1PkCloseDirectCode(XglPacket *packet);
extern void sceGsSetDefLoadImage(XglLoadImage *image, short address, short width, short psm, short x, short y, short w, short h);
extern void sceVif1PkEnd(XglPacket *packet, int mode);
extern void sceVif1PkTerminate(XglPacket *packet);
extern void sceVif1PkReset(XglPacket *packet);
extern void FlushCache(int mode);

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

static XglPacket asPacketSource[2];

static XglPacket *pCurrentPacket;

static XglPacket *pSendPacket;

/* For each texture transfer entry of `set`, appends a load-image packet and a reference to its data. */
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
        sceVif1PkAddCode(pCurrentPacket, VIF_CODE_FLUSHE);
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
                             width, SCE_GS_PSMCT32, 0, 0, (short)w, entry->height);
        image.giftag |= GIF_TAG_EOP;
        sceVif1PkOpenDirectCode(pCurrentPacket, 0);
        sceVif1PkAddDirectDataN(pCurrentPacket, &image, 5);
        sceVif1PkCloseDirectCode(pCurrentPacket);
        sceVif1PkRef(pCurrentPacket, (u8 *)set + entry->dataOffset, entry->dataSize, 0, 0, 0);
        entry++;
    }
}

/* Appends a direct block to the current packet that sets TEX0_1 from the display buffer, then the setup block. */
void xglPacketInterpolate(void)
{
    XglPacket *packet = pCurrentPacket;
    u64 *scratch;

    sceVif1PkCnt(packet, 0);
    sceVif1PkAddCode(packet, VIF_CODE_FLUSHE);
    sceVif1PkOpenDirectHLCode(packet, 0);
    sceVif1PkAddDirectDataN(packet, TestPrim_0_004A8D00, 4);
    scratch = XGL_SCRATCH;
    scratch[0] = 0x24020000 | (sRender.displayBufferBase << 5) | ((u64)0xc800 << 19);
    scratch[1] = SCE_GS_TEX0_1;
    sceVif1PkAddDirectDataN(packet, XGL_SCRATCH, 1);
    sceVif1PkAddDirectDataN(packet, &TestPrim_0_004A8D00[10], 7);
    sceVif1PkCloseDirectCode(packet);
}

XglPacket *xglPacketGetCurrent(void)
{
    return pCurrentPacket;
}

/* Initializes the two packet buffers and makes buffer 0 the current packet. */
void xglPacketInit(void)
{
    sceVif1PkInit(&asPacketSource[0], XGL_PACKET_BUFFER_0);
    sceVif1PkInit(&asPacketSource[1], XGL_PACKET_BUFFER_1);

    asPacketSource[0].limit = XGL_PACKET_BUFFER_1;
    asPacketSource[1].limit = XGL_PACKET_BUFFER_END;
    asPacketSource[0].cursor = (u8 *)XGL_PACKET_BUFFER_1;
    asPacketSource[1].cursor = (u8 *)XGL_PACKET_BUFFER_END;
    pCurrentPacket = &asPacketSource[0];
    pSendPacket = 0;
}

/* Ends the current packet, makes it the pending send packet, and switches the current packet to the other buffer. */
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

/* Starts a VIF1 DMA chain for the pending send packet, if there is one. */
void xglPacketSend(void)
{
    if (pSendPacket != 0) {
        xglDmaDirectSrcChain(XGL_DMA_CHANNEL_VIF1, pSendPacket->start);
    }
}
