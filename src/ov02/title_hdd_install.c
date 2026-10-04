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

/* The installer keeps one transfer-buffer pointer in its own BSS. */
static struct HddInstallParams *HddInstallParam;

typedef struct UmnWorkData {
    u8 unmodeled_00;
    u8 mode;
    u8 unmodeled_02;
    u8 stateOrCategory;
    u8 unmodeled_04[0x46 - 0x04];
    signed char cursor;
    u8 unmodeled_47[0x52 - 0x47];
    signed char entry;
    u8 unmodeled_53[0x5D - 0x53];
    signed char dataBaseResult;
    signed char analysed;
    signed char modelFlag;
    u8 unmodeled_60[0x80 - 0x60];
} UmnWorkData;
UmnWorkData UmnWork = {0};

extern struct HddInstallRenderState sRender;
/* DMA/VIF tags followed by the GS register data sent by HddInstallSync. */
static u64 TestEnv_0_00A0FE10[12] = {
    0, 0x5100000500000000ULL, 0x1000000000000004ULL, 14,
    0, 0x50, 0, 0x51, 0x000001C000000200ULL, 0x52, 0, 0x53,
};
static u64 TransEnv_1_00A0FE70[4] = {
    0, 0x5100000100000000ULL, 0x0800000000007000ULL, 0,
};
static u64 FlushEnv_2_00A0FE90[6] = {
    0, 0x5100000200000000ULL, 0x1000000000008001ULL, 14, 0, 0x3F,
};

void *UmnWorkEnd = 0;
u32 UmnTexAddr = 0;
void *UmnBgCubeXtx = 0;
void *UmnBgCubeLex = 0;
void *UmnModelUkn = 0;
void *UmnModelSon = 0;
void *task_xmx = 0;
void *umn_task = 0;
void *UmnMemory = 0;
void *UmnWorkEndTop = 0;
void *UmnInterface2 = 0;
/* Font control bytes passed to the existing font-script renderer. */
static const char D_00A11620[8] = "\x0f\x44\x00\x80\x0d\x03";

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
