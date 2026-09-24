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
extern u64 TestEnv_0_0036ABC0[12];
extern unsigned char TransEnv_1_0036AC20[];
extern unsigned char FlushEnv_2_0036AC40[];

static void DrawImage(u8 *framebuffer)
{
    XglPacket *packet;

    packet = xglPacketGetCurrent();
    TestEnv_0_0036ABC0[4] = ((u64)sRender.buffer_select << 0x25) | ((u64)0x8000 << 0x24);
    sceVif1PkCnt(packet, 0);
    sceVif1PkAddDataN(packet, TestEnv_0_0036ABC0, 0x18);
    sceVif1PkRef(packet, TransEnv_1_0036AC20, 2, 0, 0, 0);
    sceVif1PkRef(packet, framebuffer, 0x7000, 0, 0x51007000, 0);
    sceVif1PkRef(packet, TransEnv_1_0036AC20, 2, 0, 0, 0);
    sceVif1PkRef(packet, framebuffer + 0x70000, 0x7000, 0, 0x51007000, 0);
    sceVif1PkRef(packet, FlushEnv_2_0036AC40, 3, 0, 0, 0);
}

INCLUDE_ASM("asm/main/nonmatchings/tya_display_setting", tyaDisplaySetting);
