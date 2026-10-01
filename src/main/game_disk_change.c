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

extern int printf(const char *format, ...);

extern void xglSleep(void);
extern int xglCdDiskCheck(void);
extern void xglCdArcCheck(void);
extern void xglFontPrint(int x, int y, int color, const char *text);
extern void xglFontPrintDirectOT(int ot, const char *text);

extern u8 GameDiskChangeT10K;
extern u8 mes00_1[];
extern u8 mes01_2[];
extern u8 mes02_3[];
extern char *tbl_4[];
extern const char D_004C0790[];
extern const char D_004C07C8[];
extern const char D_004D9F58[];
extern PadPrefix PadData;

int GameDiskChange(int disk_number)
{
    int disk_check;
    int retry;

    printf(D_004C0790, disk_number, GameDiskChangeT10K);

    if (disk_number < 1 || disk_number > 3) {
        return -1;
    }

    disk_check = xglCdDiskCheck();
    switch (disk_check) {
    case 1:
        /* Low byte of the Shift-JIS fullwidth digit (0xA3 0xB1) naming the disc in the drive. */
        mes00_1[9] = 0xB1;
        break;
    case 2:
        mes00_1[9] = 0xB2;
        break;
    case 4:
        mes00_1[9] = 0xB3;
        break;
    case 0x7F:
        return 0;
    default:
        break;
    }

    retry = 0;
    replace(&mes00_1[0x10], disk_number);
    replace(mes01_2, disk_number);
    replace(mes02_3, disk_number);

    /* From here on the requested disc is held as its bit in the drive's disc mask. */
    disk_number = 1 << (disk_number - 1);
    while ((disk_check = xglCdDiskCheck()) <= 0 || !(disk_check & disk_number)) {
        if (disk_check == -2) {
            retry = 1;
        }
        drawbg();
        xglFontPrintDirectOT(0xFFFFFF, D_004D9F58);
        if (retry == 0) {
            xglFontPrint(0x40, 0x40, 0xFFFFFF, (const char *)mes00_1);
        } else {
            xglFontPrint(0x40, 0x40, 0xFFFFFF, tbl_4[disk_check + 3]);
        }
        xglFontPrint(0x40, 0x100, 0xFFFFFF, D_004C07C8);
        if (PadData.half_2a & 0x800) {
            break;
        }
        xglSleep();
    }

    xglCdArcCheck();
    xglSleep();
    return 0;
}
