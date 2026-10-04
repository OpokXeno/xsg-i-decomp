#include "common.h"
#include "shared.h"
#include "main/xgl_packet.h"

/*
 * sRender's destination-buffer selector: folded (with a fixed high bit)
 * into the register DrawImage primes at TestEnv0[4] before the two-part
 * image transfer below. Only this halfword is evidenced here (see
 * src/main/game_over.c's DrawImage for the same global read through its
 * own local view).
 */
typedef struct {
    u8 unmodeled_00[0x20];
    u16 buffer_select;
} DrawImageRenderState;

extern DrawImageRenderState sRender;

extern void sceVif1PkCnt(XglPacket *packet, int count);
extern void sceVif1PkAddDataN(XglPacket *packet, const void *data, int count);

typedef union {
    u64 value;
    struct {
        u32 low;
        u32 high;
    } words;
} GifRegisterData;

typedef struct {
    u32 control_low;
    u32 control_high;
    u32 registers_low;
    u32 registers_high;
} GifTag;

typedef struct {
    GifRegisterData data;
    u32 register_address;
    u32 unused;
} GifAdCommand;

typedef struct {
    u64 dma_tag;
    u32 vif_nop;
    u32 vif_direct;
    GifTag gif_tag;
    GifAdCommand set_pmode;
    GifAdCommand set_smode2;
    GifAdCommand set_dispfb2;
    GifAdCommand set_display2;
} TestEnvironmentPacket;

typedef struct {
    u64 dma_tag;
    u32 vif_nop;
    u32 vif_direct;
    GifTag gif_tag;
} TransferEnvironmentPacket;

typedef struct {
    u64 dma_tag;
    u32 vif_nop;
    u32 vif_direct;
    GifTag gif_tag;
    GifAdCommand finish;
} FlushEnvironmentPacket;

static TestEnvironmentPacket TestEnv_0_0036ABC0 = {
    .dma_tag = 0,
    .vif_nop = 0,
    .vif_direct = 0x51000005,
    .gif_tag = { 4, 0x10000000, 0x0000000E, 0 },
    .set_pmode = { { .value = 0 }, 0x50, 0 },
    .set_smode2 = { { .value = 0 }, 0x51, 0 },
    .set_dispfb2 = { { .value = 0x000001C000000200ULL }, 0x52, 0 },
    .set_display2 = { { .value = 0 }, 0x53, 0 },
};

static TransferEnvironmentPacket TransEnv_1_0036AC20 = {
    .dma_tag = 0,
    .vif_nop = 0,
    .vif_direct = 0x51000001,
    .gif_tag = { 0x00007000, 0x08000000, 0, 0 },
};

static FlushEnvironmentPacket FlushEnv_2_0036AC40 = {
    .dma_tag = 0,
    .vif_nop = 0,
    .vif_direct = 0x51000002,
    .gif_tag = { 1 | (1 << 15), 0x10000000, 0x0000000E, 0 },
    .finish = { { .value = 0 }, 0x3F, 0 },
};

static void DrawImage(u8 *framebuffer)
{
    XglPacket *packet;

    packet = xglPacketGetCurrent();
    TestEnv_0_0036ABC0.set_pmode.data.value =
        ((u64)sRender.buffer_select << 0x25) | ((u64)0x8000 << 0x24);
    sceVif1PkCnt(packet, 0);
    sceVif1PkAddDataN(packet, &TestEnv_0_0036ABC0, 0x18);
    sceVif1PkRef(packet, &TransEnv_1_0036AC20, 2, 0, 0, 0);
    sceVif1PkRef(packet, framebuffer, 0x7000, 0, 0x51007000, 0);
    sceVif1PkRef(packet, &TransEnv_1_0036AC20, 2, 0, 0, 0);
    sceVif1PkRef(packet, framebuffer + 0x70000, 0x7000, 0, 0x51007000, 0);
    sceVif1PkRef(packet, &FlushEnv_2_0036AC40, 3, 0, 0, 0);
}

INCLUDE_ASM("asm/main/nonmatchings/tya_display_setting", tyaDisplaySetting);
