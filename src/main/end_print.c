#include "common.h"

int ePrintWHGet(void)
{
    return 0;
}

INCLUDE_ASM("asm/main/nonmatchings/end_print", endPrintInit);

INCLUDE_ASM("asm/main/nonmatchings/end_print", endPrintWinTexLoad);

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintFlush);

INCLUDE_ASM("asm/main/nonmatchings/end_print", endPrintExtFuncPack);

INCLUDE_ASM("asm/main/nonmatchings/end_print", endPrintInfoSet);

INCLUDE_ASM("asm/main/nonmatchings/end_print", endSpriteSet);

INCLUDE_ASM("asm/main/nonmatchings/end_print", subPrintSprite);

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintSprite00);

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintSprite01);

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintPoint);

INCLUDE_ASM("asm/main/nonmatchings/end_print", subPrintLine);

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintLine);

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintLine2);

INCLUDE_ASM("asm/main/nonmatchings/end_print", endPrintDirectRibbon);

INCLUDE_ASM("asm/main/nonmatchings/end_print", subPrintRibbon);

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintRibbon);

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintTagFont);

INCLUDE_ASM("asm/main/nonmatchings/end_print", ScissorSet);

INCLUDE_ASM("asm/main/nonmatchings/end_print", ScissorReset);

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintNumber);

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintWindow);

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintFrame);

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintThumbnail);

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintCircleCore);

#include "main/xgl_packet.h"

/* EE scratchpad RAM base (src/ov01/gr_gp_init.c's grPacketSend and
 * src/ov01/m_gs.c's MGsGPTerm build the same VIF1 direct-mode packet at the
 * same address). */
#define SCRATCHPAD_BASE ((void *)0x70000000)

extern void sceVif1PkCnt(XglPacket *packet, int count);
extern void sceVif1PkOpenDirectHLCode(XglPacket *packet, int mode);
extern void sceVif1PkCloseDirectHLCode(XglPacket *packet);

/* PrintCircleCore (this TU, still asm) draws into the scratchpad buffer this
 * function points it at. */
extern void PrintCircleCore(XglPacket *packet, void *buffer, int radius);

void endPrintDirectCircle(int radius) {
    XglPacket *vif1Packet;

    vif1Packet = xglPacketGetCurrent();
    sceVif1PkCnt(vif1Packet, 0);
    sceVif1PkOpenDirectHLCode(vif1Packet, 0);
    PrintCircleCore(vif1Packet, SCRATCHPAD_BASE, radius);
    sceVif1PkCloseDirectHLCode(vif1Packet);
}

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintCircle);

INCLUDE_ASM("asm/main/nonmatchings/end_print", FontTexReload);

INCLUDE_ASM("asm/main/nonmatchings/end_print", FontTexChange);

INCLUDE_ASM("asm/main/nonmatchings/end_print", DrawBackSet);

INCLUDE_ASM("asm/main/nonmatchings/end_print", DrawBackReset);

INCLUDE_ASM("asm/main/nonmatchings/end_print", ScreenClear);

INCLUDE_ASM("asm/main/nonmatchings/end_print", ZScissor);

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintBackSprite);

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintBackSprite2);

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintEtherLine);

INCLUDE_ASM("asm/main/nonmatchings/end_print", endPrintExtFunc);

INCLUDE_ASM("asm/main/nonmatchings/end_print", endPrintDirectFrameCopy);

INCLUDE_ASM("asm/main/nonmatchings/end_print", endPrintDirectLine);

INCLUDE_ASM("asm/main/nonmatchings/end_print", endDecodeJpeg);

INCLUDE_ASM("asm/main/nonmatchings/end_print", endPrintJpeg);
