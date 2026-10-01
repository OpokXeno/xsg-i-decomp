/*
 * OV02 original TU 4: 0x00a017f0..0x00a023a8 (5 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_packet.h"

struct HddInstallParams {
    u8 unmodeled_00[8];
    u8 *transfer_buffer;
};

struct HddInstallRenderState {
    u8 unmodeled_00[0x20];
    u16 buffer_select;
};

extern struct HddInstallParams *HddInstallParam;
extern struct HddInstallRenderState sRender;
extern u64 TestEnv_0_00A0FE10[];
extern u8 TransEnv_1_00A0FE70[];
extern u8 FlushEnv_2_00A0FE90[];
extern const char D_00A11620[];

extern void sceVif1PkCnt(XglPacket *packet, int count);
extern void sceVif1PkAddDataN(XglPacket *packet, const void *data, int count);
extern void xglFontPrintDirectOT(int color, const char *text);

static void HddInstallSync(void)
{
    XglPacket *packet;
    u8 *transfer_buffer;

    packet = xglPacketGetCurrent();
    transfer_buffer = HddInstallParam->transfer_buffer;
    TestEnv_0_00A0FE10[4] = ((u64)sRender.buffer_select << 37) | ((u64)0x8000 << 36);
    sceVif1PkCnt(packet, 0);
    sceVif1PkAddDataN(packet, TestEnv_0_00A0FE10, 0x18);
    sceVif1PkRef(packet, TransEnv_1_00A0FE70, 2, 0, 0, 0);
    sceVif1PkRef(packet, transfer_buffer, 0x7000, 0, 0x51007000, 0);
    sceVif1PkRef(packet, TransEnv_1_00A0FE70, 2, 0, 0, 0);
    sceVif1PkRef(packet, transfer_buffer + 0x70000, 0x7000, 0, 0x51007000, 0);
    sceVif1PkRef(packet, FlushEnv_2_00A0FE90, 3, 0, 0, 0);
    xglSleep();
    xglFontPrintDirectOT(-1, D_00A11620);
}

INCLUDE_ASM("asm/nonmatchings/ov02/title_hdd_install", HddInstallCB);

INCLUDE_ASM("asm/nonmatchings/ov02/title_hdd_install", TitleHddInstallSubInstall);

INCLUDE_ASM("asm/nonmatchings/ov02/title_hdd_install", TitleHddInstallSubUninstall);

INCLUDE_ASM("asm/nonmatchings/ov02/title_hdd_install", TitleHddInstall);
