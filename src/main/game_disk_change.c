#include "common.h"
#include "shared.h"
#include "main/xgl_packet.h"
#include "main/game_disk_change.h"
#include "main/xgl_cd.h"
#include "main/xgl_dma.h"
#include "main/xgl_font.h"
#include "main/xgl_thread.h"

#define DISK_CHANGE_DISC_COUNT 3
#define DISK_CHANGE_TEXT_WHITE 0xFFFFFF
#define DISK_CHANGE_TEXT_X 0x40
#define DISK_CHANGE_TEXT_Y 0x40
#define DISK_CHANGE_PROMPT_Y 0x100

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

extern const u8 D_8000[];
extern const char diskChangeFormat[24];
extern const unsigned char disk1And2Text[16];
extern const unsigned char detectingText[16];
extern const char pressStartForceContinueText[40];
extern const char diskChangeEffectControl[];
extern void sceVif1PkCnt(XglPacket *packet, int count);
extern void sceVif1PkAddDataN(XglPacket *packet, const void *data, int count);
extern int printf(const char *format, ...);
extern int xglCdDiskCheck(void);
extern void xglFontPrintDirectOT(int ot, const char *text);

const char diskChangeFormat[24] = "GameDiskChange:%d %d\n";

/* JP: ＤＩＳＫ１＆２
 * EN: DISK 1 & 2 */
const unsigned char disk1And2Text[16] = "\xA3\xC4\xA3\xC9\xA3\xD3\xA3\xCB\xA3\xB1\xA1\xF5\xA3\xB2";

/* JP: 認識中…
 * EN: Detecting... */
const unsigned char detectingText[16] = "\xC7\xA7\xBC\xB1\xC3\xE6\xA1\xC4";

/* JP: ＳＴＡＲＴで強制続行
 * EN: Press START to force continuation */
const char pressStartForceContinueText[40] = "\x19\x01\xA3\xD3\xA3\xD4\xA3\xC1\xA3\xD2\xA3\xD4\x19\x00\xA4\xC7\xB6\xAF\xC0\xA9\xC2\xB3\xB9\xD4";

/* VIF DIRECT sends the five-quadword GIF packet that draws the backdrop sprite,
 * which drawbg() sends while GameDiskChange waits for the disc. */
static GameDiskChangeTestEnv TestEnv_0_00369EF0;

static u8 mes00_1[55];

static u8 mes01_2[85];

static u8 mes02_3[41];

static char *tbl_4[8];

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

/* JP: ＤＩＳＣ１を取り出して、\n\nＤＩＳＣ２を挿入して下さい。
 * EN: Please remove DISC 1,\n\nand insert DISC 2. */
static u8 mes00_1[55] = "\xA3\xC4\xA3\xC9\xA3\xD3\xA3\xC3\xA3\xB1\xA4\xF2\xBC\xE8\xA4\xEA\xBD\xD0\xA4\xB7\xA4\xC6\xA1\xA2\n\n\xA3\xC4\xA3\xC9\xA3\xD3\xA3\xC3\xA3\xB2\xA4\xF2\xC1\xDE\xC6\xFE\xA4\xB7\xA4\xC6\xB2\xBC\xA4\xB5\xA4\xA4\xA1\xA3";

/* JP: ＤＩＳＣが違います。ＤＩＳＣを取り出して、\n\nゼノサーガのＤＩＳＣ２を挿入して下さい。
 * EN: Wrong DISC. Please remove the DISC,\n\nand insert Xenosaga DISC 2. */
static u8 mes01_2[85] = "\xA3\xC4\xA3\xC9\xA3\xD3\xA3\xC3\xA4\xAC\xB0\xE3\xA4\xA4\xA4\xDE\xA4\xB9\xA1\xA3\xA3\xC4\xA3\xC9\xA3\xD3\xA3\xC3\xA4\xF2\xBC\xE8\xA4\xEA\xBD\xD0\xA4\xB7\xA4\xC6\xA1\xA2\n\n\xA5\xBC\xA5\xCE\xA5\xB5\xA1\xBC\xA5\xAC\xA4\xCE\xA3\xC4\xA3\xC9\xA3\xD3\xA3\xC3\xA3\xB2\xA4\xF2\xC1\xDE\xC6\xFE\xA4\xB7\xA4\xC6\xB2\xBC\xA4\xB5\xA4\xA4\xA1\xA3";

/* JP: ゼノサーガのＤＩＳＣ２を挿入して下さい。
 * EN: Please insert Xenosaga DISC 2. */
static u8 mes02_3[41] = "\xA5\xBC\xA5\xCE\xA5\xB5\xA1\xBC\xA5\xAC\xA4\xCE\xA3\xC4\xA3\xC9\xA3\xD3\xA3\xC3\xA3\xB2\xA4\xF2\xC1\xDE\xC6\xFE\xA4\xB7\xA4\xC6\xB2\xBC\xA4\xB5\xA4\xA4\xA1\xA3";

static char *tbl_4[8] = { (char *)detectingText, (char *)mes02_3, (char *)mes01_2,
                   (char *)mes02_3, (char *)mes01_2, (char *)mes01_2,
                   (char *)disk1And2Text, (char *)mes01_2 };

/* Text: {reset}{effect 3} */
const char diskChangeEffectControl[4] = "\x0B\x0D\x03";

u8 GameDiskChangeT10K = 0;

static void drawbg(void)
{
    XglPacket *packet;

    packet = xglPacketGetCurrent();
    sceVif1PkCnt(packet, 0);
    sceVif1PkAddDataN(packet, &TestEnv_0_00369EF0, 0x18);
}

/* Replaces each fullwidth-digit placeholder in `str` with the digit for `disk_number`. */
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

/*
 * Waits until disc `disk_number` (1 to 3) is in the drive, showing the prompt meanwhile;
 * START skips the wait.  Returns -1 for an invalid disc number, otherwise 0.
 */
int GameDiskChange(int disk_number)
{
    int disk_check;
    int retry;

    printf(diskChangeFormat, disk_number, GameDiskChangeT10K);

    if (disk_number < 1 || disk_number > DISK_CHANGE_DISC_COUNT) {
        return -1;
    }

    disk_check = xglCdDiskCheck();
    switch (disk_check) {
    case CD_DISC_FLAG_DISC1:
        /* Low byte of the Shift-JIS fullwidth digit (0xA3 0xB1) naming the disc in the drive. */
        mes00_1[9] = 0xB1;
        break;
    case CD_DISC_FLAG_DISC2:
        mes00_1[9] = 0xB2;
        break;
    case CD_DISC_FLAG_DISC3:
        mes00_1[9] = 0xB3;
        break;
    case CD_DISC_FLAGS_DUAL_LAYER:
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
    /* Redraw the prompt each frame until the wanted disc is found or START is pressed. */
    while ((disk_check = xglCdDiskCheck()) <= 0 || !(disk_check & disk_number)) {
        if (disk_check == CD_CHECK_SHELL_OPEN) {
            retry = 1;
        }
        drawbg();
        xglFontPrintDirectOT(DISK_CHANGE_TEXT_WHITE, diskChangeEffectControl);
        if (retry == 0) {
            xglFontPrint(DISK_CHANGE_TEXT_X, DISK_CHANGE_TEXT_Y, DISK_CHANGE_TEXT_WHITE, (const char *)mes00_1);
        } else {
            xglFontPrint(DISK_CHANGE_TEXT_X, DISK_CHANGE_TEXT_Y, DISK_CHANGE_TEXT_WHITE, tbl_4[disk_check + 3]);
        }
        xglFontPrint(DISK_CHANGE_TEXT_X, DISK_CHANGE_PROMPT_Y, DISK_CHANGE_TEXT_WHITE, pressStartForceContinueText);
        if (PadData.half_2a & PAD_START) {
            break;
        }
        xglSleep();
    }

    xglCdArcCheck();
    xglSleep();
    return 0;
}
