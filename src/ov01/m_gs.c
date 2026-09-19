/*
 * OV01 original TU 18: 0x00a32db8..0x00a32e78 (2 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_packet.h"

/* EE scratchpad RAM base. */
#define SCRATCHPAD_BASE ((void *)0x70000000)
/* Default packet capacity used when no custom buffer is supplied. */
#define MGS_PACKET_DEFAULT_CAPACITY 0x400
/* VIF FLUSH code (command 0x11 in bits 24..30, no immediate). */
#define VIF_CODE_FLUSH 0x11000000

/*
 * The GS direct-transfer packet MGsGPInit prepares and MGsGPTerm flushes.
 * By default it targets the scratchpad with a fixed capacity; a caller that
 * supplies its own buffer address also supplies that buffer's capacity
 * (MGsGPInit: movn on both the address and the size against the same
 * condition register).
 *
 *   data      the transfer base address: MGsGPInit's default/custom value,
 *             and the address MGsGPTerm passes to sceVif1PkAddDirectDataN.
 *   capacity  the buffer's byte capacity, set by MGsGPInit; no function in
 *             this TU reads it back.
 *   current   reset to data by both MGsGPInit and MGsGPTerm; the code that
 *             advances it while queuing packet contents is outside this TU.
 *   count     the queued transfer size, passed to sceVif1PkAddDirectDataN
 *             and reset to zero after each flush.
 *   auxCount  reset to zero alongside count by both functions; no further
 *             access is evidenced in this TU.
 */
typedef struct MGsPacket {
    void *data;      /* +0x00 */
    int capacity;    /* +0x04 */
    void *current;   /* +0x08 */
    int count;       /* +0x0C */
    int auxCount;    /* +0x10 */
} MGsPacket;

extern void sceVif1PkCnt(XglPacket *packet, int count);
extern void sceVif1PkAddCode(XglPacket *packet, unsigned int code);
extern void sceVif1PkOpenDirectHLCode(XglPacket *packet, int mode);
extern void sceVif1PkAddDirectDataN(XglPacket *packet, const void *data,
                                    int count);
extern void sceVif1PkCloseDirectHLCode(XglPacket *packet);
/*
 * Pads the packet to a quadword boundary, closes the open DMA tag and
 * returns the new write address (main 0x0020acd4); sceVif1PkCnt (main
 * 0x0020ad20) keeps that return as the address of its next DMA tag.
 */
extern void *sceVif1PkTerminate(XglPacket *packet);

void MGsGPInit(MGsPacket *packet, void *address, int size)
{
    void *base = (address != 0) ? address : SCRATCHPAD_BASE;

    packet->data = base;
    packet->capacity = (address != 0) ? size : MGS_PACKET_DEFAULT_CAPACITY;
    packet->current = base;
    packet->count = 0;
    packet->auxCount = 0;
}

void MGsGPTerm(MGsPacket *packet)
{
    if (packet->count != 0) {
        XglPacket *vif1Packet = xglPacketGetCurrent();

        sceVif1PkCnt(vif1Packet, 0);
        sceVif1PkAddCode(vif1Packet, VIF_CODE_FLUSH);
        sceVif1PkOpenDirectHLCode(vif1Packet, 0);
        sceVif1PkAddDirectDataN(vif1Packet, packet->data, packet->count);
        sceVif1PkCloseDirectHLCode(vif1Packet);
        sceVif1PkTerminate(vif1Packet);
        packet->current = packet->data;
        packet->count = 0;
        packet->auxCount = 0;
    }
}
