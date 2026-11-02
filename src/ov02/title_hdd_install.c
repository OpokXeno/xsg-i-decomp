/*
 * OV02 original TU 4: 0x00a017f0..0x00a023a8 (5 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_packet.h"

struct HddInstallParams {
    u8 *image_data;
    u8 step;
    u8 unmodeled_05[3];
    u8 *transfer_buffer;
};

/* Gauge block read by tyaDrawGauge: the same layout as its TU-local TyaGaugeArgs. */
struct HddInstallGauge {
    short x;
    short y;
    unsigned int vertex_attribute;
    short width;
    short height;
    unsigned short current;
    unsigned short maximum;
    unsigned char filled_color[4];
    unsigned char unfilled_color[4];
};

/* Request block read by xglJpegDecode: the same layout as the shared JpegDecodeRequest. */
typedef struct {
    u8 *source;
    u8 *destination;
    u8 unmodeled_08[0x18];
} HddInstallJpegRequest;

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
extern void xglFontPrint(int x, int y, int color, const char *text);
extern void xglFontPrintf(int x, int y, unsigned int color, void *args);
extern void xglFontDebugPrintf(int x, int y, const char *format, ...);
extern void xglSoundEffectNormalDirect(int effect);
extern int xglHddCheck(void);
extern int xglJpegDecode(void *request_address);
extern int printf(const char *format, ...);
extern void tyaDrawGauge(struct HddInstallGauge *gauge);
extern PadPrefix PadData;
extern unsigned char D_00A13654;
extern const char D_00A11628[];
extern const char D_00A11688[];
extern const char D_00A11698[];
extern const char D_00A116B0[];
extern const char D_00A116C0[];
extern const char D_00A116E0[];
extern const char D_00A116F8[];
extern const char D_00A11708[];
extern const char D_00A11718[];
extern const char D_00A11738[];
extern const char D_00A117A0[];
extern const char D_00A117B8[];
extern const char D_00A117C8[];
extern const char D_00A117D8[];
extern const char D_00A117F0[];
extern const char D_00A11800[];
extern const char D_00A11818[];
extern const char D_00A11828[];
extern const char anim_4[4];
extern const char sztmp_5[3];
extern const char sztmp_6[6];
extern int xglHddInstall(int (*callback)(), int *status);
extern int xglHddCheck2(void);
extern int xglHddUninstall(void);
extern int xglHddActivate(int state);
extern int xglCdReadFile(const char *name, void *buffer, int mode, int flags);
extern u8 *WorkEnd;
extern const char *mes_7[];
extern const char D_00A118B8[];
extern const char D_00A11928[];
extern const char D_00A11970[];
extern const char D_00A119E0[];
extern const char D_00A11A58[];
extern const char D_00A11AA8[];
extern const char D_00A11B20[];
extern const char D_00A11C60[];
extern const char D_00A11C90[];
extern const char D_00A11CD8[];
#define PAD_CIRCLE 0x0020
#define PAD_CROSS 0x0040

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

static int HddInstallCB(int command, int offset, int *status)
{
    struct HddInstallGauge gauge;
    u64 print_args[2];
    HddInstallJpegRequest jpeg_request;
    int percent;
    int step;
    u8 *base;
    PadPrefix *pad;

    if (*status != 0) {
        HddInstallSync();
        return *status;
    }
    xglFontPrint(32, 16, -1, D_00A11628);
    switch (command) {
    case 0:
        xglFontDebugPrintf(84, 208, D_00A11688);
        break;
    case 1:
        xglFontDebugPrintf(84, 208, D_00A11698);
        break;
    case 2:
        xglFontDebugPrintf(84, 208, D_00A116B0);
        break;
    case 3:
        xglFontDebugPrintf(84, 208, D_00A116C0, offset);
        break;
    case 4:
        xglFontDebugPrintf(84, 208, D_00A116E0);
        break;
    case 5:
        xglFontDebugPrintf(84, 208, D_00A116F8);
        break;
    case 6:
        if (offset == 0) {
            xglFontDebugPrintf(84, 208, D_00A11708);
            break;
        }
        gauge.x = 168;
        gauge.y = 416;
        gauge.vertex_attribute = -1;
        gauge.width = 320;
        gauge.height = 12;
        gauge.current = offset;
        gauge.maximum = offset >> 16;
        gauge.filled_color[0] = 0x80;
        gauge.filled_color[1] = 0xC0;
        gauge.filled_color[2] = 0x80;
        gauge.filled_color[3] = 0x80;
        gauge.unfilled_color[0] = 0x40;
        gauge.unfilled_color[1] = 0x40;
        gauge.unfilled_color[2] = 0x40;
        gauge.unfilled_color[3] = 0x80;
        tyaDrawGauge(&gauge);
        xglFontPrint(32, 80, -1, D_00A11718);
        print_args[0] = (u32)sztmp_5;
        print_args[1] = (u32)anim_4[(D_00A13654 >> 3) & 3];
        xglFontPrintf(152, 414, -1, print_args);
        D_00A13654++;
        print_args[0] = (u32)sztmp_6;
        percent = ((offset & 0xFFFF) * 100) / (offset >> 16);
        print_args[1] = percent;
        xglFontPrintf(168, 400, -1, print_args);
        step = print_args[1] / 20;
        if (step != HddInstallParam->step && step < 5) {
            base = HddInstallParam->image_data;
            memset(&jpeg_request, 0, sizeof(jpeg_request));
            HddInstallParam->step = step;
            jpeg_request.source = base + ((int *)base)[step] + 0x40;
            jpeg_request.destination = HddInstallParam->transfer_buffer;
            xglJpegDecode(&jpeg_request);
        }
        pad = &PadData;
        if ((pad->half_2a & 0x40) != 0) {
            xglSoundEffectNormalDirect(2);
            HddInstallSync();
            while (1) {
                xglFontPrint(32, 16, -1, D_00A11738);
                if ((PadData.half_2a & 0x20) != 0) {
                    xglSoundEffectNormalDirect(1);
                    *status = 1;
                    return 1;
                }
                if ((PadData.half_2a & 0x40) != 0) {
                    xglSoundEffectNormalDirect(2);
                    if (xglHddCheck() < 0) {
                        *status = 2;
                        return 2;
                    }
                    break;
                } else {
                    HddInstallSync();
                }
            }
        }
        break;
    case 7:
        switch (offset) {
        case 0:
            xglFontDebugPrintf(84, 208, D_00A117A0);
            break;
        case 1:
            xglFontDebugPrintf(84, 208, D_00A117B8);
            break;
        case 2:
            xglFontDebugPrintf(84, 208, D_00A117C8);
            break;
        case 3:
            xglFontDebugPrintf(84, 208, D_00A117D8);
            break;
        case 4:
            xglFontDebugPrintf(84, 208, D_00A117F0);
            break;
        }
        break;
    case 8:
        xglFontDebugPrintf(84, 208, D_00A11800);
        break;
    case -15:
    case -14:
    case -13:
    case -12:
    case -11:
    case -10:
    case -9:
    case -8:
    case -7:
    case -6:
    case -5:
    case -4:
    case -3:
    case -2:
    case -1:
        printf(D_00A11818, command, offset, offset);
        xglFontDebugPrintf(84, 208, D_00A11828, command, offset);
        break;
    }
    HddInstallSync();
    return 0;
}

static void TitleHddInstallSubInstall(void)
{
    int installStatus;
    int result;
    int frames;
    u16 pad;

    for (;;) {
        installStatus = 0;
        frames = 4;
        for (;;) {
            xglFontPrint(32, 16, -1, D_00A118B8);
            HddInstallSync();
            if (frames > 0) {
                frames--;
                continue;
            }
            pad = PadData.half_2a;
            if (pad & PAD_CIRCLE) {
                break;
            }
            if (pad & PAD_CROSS) {
                xglSoundEffectNormalDirect(2);
                return;
            }
        }
        xglSoundEffectNormalDirect(1);
        result = xglHddCheck();
        if (result == 0) {
            result = xglHddInstall(HddInstallCB, &installStatus);
        }
        if (result == 1) {
            HddInstallSync();
            return;
        }
        if (result == 0) {
            xglHddActivate(1);
            result = xglHddActivate(-1);
        } else {
            result = 0;
        }
        HddInstallSync();
        frames = 4;
        if (result == 1) {
            for (;;) {
                xglFontPrint(32, 16, -1, D_00A11928);
                HddInstallSync();
                if (frames > 0) {
                    frames--;
                    continue;
                }
                pad = PadData.half_2a;
                if (pad & PAD_CIRCLE) {
                    xglSoundEffectNormalDirect(1);
                    return;
                }
            }
        }
        for (;;) {
            xglFontPrint(32, 16, -1, D_00A11970);
            HddInstallSync();
            if (frames > 0) {
                frames--;
                continue;
            }
            pad = PadData.half_2a;
            if (pad & PAD_CIRCLE) {
                xglSoundEffectNormalDirect(1);
                return;
            }
            if (pad & PAD_CROSS) {
                break;
            }
        }
        xglSoundEffectNormalDirect(2);
        HddInstallSync();
    }
}

static void TitleHddInstallSubUninstall(void)
{
    int result;
    int frames;
    u16 pad;

    for (;;) {
        frames = 4;
        for (;;) {
            xglFontPrint(32, 16, -1, D_00A119E0);
            HddInstallSync();
            if (frames > 0) {
                frames--;
                continue;
            }
            pad = PadData.half_2a;
            if (pad & PAD_CIRCLE) {
                break;
            }
            if (pad & PAD_CROSS) {
                xglSoundEffectNormalDirect(2);
                return;
            }
        }
        xglSoundEffectNormalDirect(1);
        result = xglHddCheck2();
        if (result == 0) {
            result = xglHddUninstall();
        }
        frames = 4;
        HddInstallSync();
        if (result == 0) {
            for (;;) {
                xglFontPrint(32, 16, -1, D_00A11A58);
                HddInstallSync();
                if (frames > 0) {
                    frames--;
                    continue;
                }
                pad = PadData.half_2a;
                if (pad & PAD_CIRCLE) {
                    xglSoundEffectNormalDirect(1);
                    return;
                }
            }
        }
        for (;;) {
            xglFontPrint(32, 16, -1, D_00A11AA8);
            HddInstallSync();
            if (frames > 0) {
                frames--;
                continue;
            }
            pad = PadData.half_2a;
            if (pad & PAD_CIRCLE) {
                xglSoundEffectNormalDirect(1);
                xglHddCheck();
                return;
            }
            if (pad & PAD_CROSS) {
                break;
            }
        }
        xglSoundEffectNormalDirect(2);
        HddInstallSync();
    }
}

void TitleHddInstall(int command)
{
    struct HddInstallParams params;
    HddInstallJpegRequest request;
    u8 *buffer;
    u8 *saved_work_end;
    u8 *transfer_buffer;
    int size;
    int count;

    saved_work_end = WorkEnd;
    if (command == 0) {
        if (xglHddCheck() != 0) {
            command = 2;
        }
    } else if (command == 1) {
        if (xglHddCheck2() != 0) {
            xglHddActivate(0);
            command = 5;
        }
    }
    buffer = WorkEnd;
    size = xglCdReadFile(D_00A11B20, buffer, 0, 1);
    transfer_buffer = (u8 *)(((u32)buffer + size + 63) & ~63);
    WorkEnd = (u8 *)((u32)transfer_buffer + 0x000E0000);
    params.image_data = buffer;
    params.step = 0;
    params.transfer_buffer = transfer_buffer;
    memset(&request, 0, 32);
    request.source = buffer + 0x40;
    request.destination = transfer_buffer;
    xglJpegDecode(&request);
    HddInstallParam = &params;
    HddInstallSync();
    xglFontPrintDirectOT(-1, D_00A11620);
    switch (command) {
    case 0:
        TitleHddInstallSubInstall();
        break;
    case 1:
        TitleHddInstallSubUninstall();
        break;
    case 2:
    case 3:
    case 4:
        count = 4;
        for (;;) {
            xglFontPrint(32, 16, -1, mes_7[command - 2]);
            xglFontPrintDirectOT(-1, D_00A11C90);
            HddInstallSync();
            if (count > 0) {
                count--;
            } else if ((PadData.half_2a & 0x20) != 0) {
                xglSoundEffectNormalDirect(1);
                break;
            }
        }
        break;
    case 5:
        count = 4;
        for (;;) {
            xglFontPrint(32, 16, -1, D_00A11C60);
            xglFontPrintDirectOT(-1, D_00A11CD8);
            HddInstallSync();
            if (count > 0) {
                count--;
            } else if ((PadData.half_2a & 0x20) != 0) {
                xglSoundEffectNormalDirect(1);
                break;
            }
        }
        xglHddActivate(259);
        xglHddCheck();
        break;
    }
    HddInstallSync();
    WorkEnd = saved_work_end;
}
