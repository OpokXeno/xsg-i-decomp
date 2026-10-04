#include "common.h"
#include "shared.h"
#include "main/xgl_packet.h"

extern void sceVif1PkCnt(XglPacket *packet, int count);
extern void sceVif1PkAddDataN(XglPacket *packet, const void *data, int count);
extern const u8 D_8000[];

typedef struct GifAdData {
    u64 value;
    u64 register_address;
} GifAdData;

typedef struct GifRgbaqData {
    u32 rgba;
    u32 q;
    u32 ignored_08;
    u32 ignored_0c;
} GifRgbaqData;

typedef struct GifXyz2Data {
    u16 x;
    u16 ignored_02;
    u16 y;
    u16 ignored_06;
    u32 z;
    u16 ignored_0c;
    u16 disable_drawing;
} GifXyz2Data;

typedef struct GameDiskChangeGifPacket {
    /* GIFtag control: NLOOP=1, EOP=1, PRE=1, PRIM=6, PACKED, NREG=4. */
    const u8 *tag_control_low;
    u32 tag_control_high;
    /* The low four nibbles select A+D, RGBAQ, XYZ2 and XYZ2 in that order. */
    u64 register_ids;
    GifAdData test_1;
    GifRgbaqData color;
    GifXyz2Data first_vertex;
    GifXyz2Data second_vertex;
} GameDiskChangeGifPacket;

typedef struct GameDiskChangeTestEnv {
    u32 vif_nop[3];
    u32 vif_direct;
    GameDiskChangeGifPacket gif;
} GameDiskChangeTestEnv;

/* VIF DIRECT sends the five-qword GIF packet that clears and outlines the
 * backdrop while GameDiskChange waits for its replacement disc. */
static GameDiskChangeTestEnv TestEnv_0_00369EF0;

static void drawbg(void)
{
    XglPacket *packet;

    packet = xglPacketGetCurrent();
    sceVif1PkCnt(packet, 0);
    sceVif1PkAddDataN(packet, &TestEnv_0_00369EF0, 0x18);
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

static u8 mes00_1[55];
static u8 mes01_2[85];
static u8 mes02_3[41];
extern const char D_004C0790[24];
extern const unsigned char D_004C07A8[16];
extern const unsigned char D_004C07B8[16];
static char *tbl_4[8];
extern const unsigned char D_004C07C8[40];
extern const char D_004D9F58[];
extern u8 GameDiskChangeT10K;
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

static GameDiskChangeTestEnv TestEnv_0_00369EF0 = {
    { 0, 0, 0 },
    0x50000005,
    {
        D_8000 + 1, 0x40034000, 0x551E,
        { 0x0000000000030000ULL, 0x47 },
        { 0, 0x40, 0, 0x80 },
        { 0x6FF8, 0, 0x71F7, 0, 0, 0, 0 },
        { 0x8FF8, 0, 0x8DF7, 0, 0, 0, 0 },
    }
};

static u8 mes00_1[55] = "\xA3\xC4\xA3\xC9\xA3\xD3\xA3\xC3\xA3\xB1\xA4\xF2\xBC\xE8\xA4\xEA\xBD\xD0\xA4\xB7\xA4\xC6\xA1\xA2\n\n\xA3\xC4\xA3\xC9\xA3\xD3\xA3\xC3\xA3\xB2\xA4\xF2\xC1\xDE\xC6\xFE\xA4\xB7\xA4\xC6\xB2\xBC\xA4\xB5\xA4\xA4\xA1\xA3";
static u8 mes01_2[85] = "\xA3\xC4\xA3\xC9\xA3\xD3\xA3\xC3\xA4\xAC\xB0\xE3\xA4\xA4\xA4\xDE\xA4\xB9\xA1\xA3\xA3\xC4\xA3\xC9\xA3\xD3\xA3\xC3\xA4\xF2\xBC\xE8\xA4\xEA\xBD\xD0\xA4\xB7\xA4\xC6\xA1\xA2\n\n\xA5\xBC\xA5\xCE\xA5\xB5\xA1\xBC\xA5\xAC\xA4\xCE\xA3\xC4\xA3\xC9\xA3\xD3\xA3\xC3\xA3\xB2\xA4\xF2\xC1\xDE\xC6\xFE\xA4\xB7\xA4\xC6\xB2\xBC\xA4\xB5\xA4\xA4\xA1\xA3";
static u8 mes02_3[41] = "\xA5\xBC\xA5\xCE\xA5\xB5\xA1\xBC\xA5\xAC\xA4\xCE\xA3\xC4\xA3\xC9\xA3\xD3\xA3\xC3\xA3\xB2\xA4\xF2\xC1\xDE\xC6\xFE\xA4\xB7\xA4\xC6\xB2\xBC\xA4\xB5\xA4\xA4\xA1\xA3";
const char D_004C0790[24] = "GameDiskChange:%d %d\n";
const unsigned char D_004C07A8[16] = "\xA3\xC4\xA3\xC9\xA3\xD3\xA3\xCB\xA3\xB1\xA1\xF5\xA3\xB2";
const unsigned char D_004C07B8[16] = "\xC7\xA7\xBC\xB1\xC3\xE6\xA1\xC4";
static char *tbl_4[8] = { (char *)D_004C07B8, (char *)mes02_3, (char *)mes01_2,
                   (char *)mes02_3, (char *)mes01_2, (char *)mes01_2,
                   (char *)D_004C07A8, (char *)mes01_2 };
const unsigned char D_004C07C8[40] = "\x19\x01\xA3\xD3\xA3\xD4\xA3\xC1\xA3\xD2\xA3\xD4\x19\x00\xA4\xC7\xB6\xAF\xC0\xA9\xC2\xB3\xB9\xD4";
const char D_004D9F58[4] = "\x0B\x0D\x03";
u8 GameDiskChangeT10K = 0;
