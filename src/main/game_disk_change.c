#include "common.h"
#include "shared.h"
#include "main/xgl_packet.h"

extern void sceVif1PkCnt(XglPacket *packet, int count);
extern void sceVif1PkAddDataN(XglPacket *packet, const void *data, int count);
extern unsigned char TestEnv_0_00369EF0[];

static void drawbg(void)
{
    XglPacket *packet;

    packet = xglPacketGetCurrent();
    sceVif1PkCnt(packet, 0);
    sceVif1PkAddDataN(packet, TestEnv_0_00369EF0, 0x18);
}

static void replace(u8 *str, int disk_number) {
    u8 *ch;

    ch = str;
    if (*ch != 0) {
        do {
            /* The placeholder is a fixed two-byte code read as one unit. */
            if ((*(u16 *)ch & 0xF0FF) == 0xB0A3) {
                ch[1] = (u8)(disk_number - 0x50);
            }
            ch += 2;
        } while (*ch != 0);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/game_disk_change", GameDiskChange);
