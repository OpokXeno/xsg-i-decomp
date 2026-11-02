/*
 * OV02 original TU 4: 0x00a017f0..0x00a023a8 (5 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_packet.h"
#include "main/xgl_font.h"
#include "main/xgl_hdd.h"
#include "main/xgl_jpeg.h"
#include "main/xgl_cd.h"
#include "main/xgl_sound.h"
#include "main/xgl_dma.h"
#include "main/control_entry.h"
#include "ov02/ov02.h"
#include "ov02/title_hdd_install.h"
#include "ov02/umn_interface2.h"

/* TitleHddInstall screens: the command the title passes for the state of the hard disk. */
#define HDD_SCREEN_INSTALL 0
#define HDD_SCREEN_UNINSTALL 1
#define HDD_SCREEN_MESSAGE_FIRST 2 /* mes_7[command - 2] */
#define HDD_SCREEN_MESSAGE_LAST 4
#define HDD_SCREEN_REACTIVATE 5

/* Frames the screens ignore the pad after they appear. */
#define HDD_INPUT_DELAY 4

/* Position of the screen's message and of the installer's progress text. */
#define HDD_MESSAGE_X 32
#define HDD_MESSAGE_Y 16
#define HDD_STATUS_X 84
#define HDD_STATUS_Y 208

/* The install archive: a 0x40-byte header, then one jpeg for every fifth of the progress. */
#define HDD_ARCHIVE_HEADER_SIZE 0x40
#define HDD_PROGRESS_IMAGES 5
#define HDD_PROGRESS_PERCENT_PER_IMAGE 20

/* The decoded jpeg is sent in two halves of HDD_TEXTURE_QWC quadwords. */
#define HDD_TEXTURE_QWC 0x7000
#define HDD_TEXTURE_HALF_SIZE (HDD_TEXTURE_QWC * 16)
#define HDD_TRANSFER_BUFFER_SIZE (HDD_TEXTURE_HALF_SIZE * 2)

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

extern struct HddInstallRenderState sRender;
extern int xglHddInstall(int (*callback)(), int *status);
extern void tyaDrawGauge(struct HddInstallGauge *gauge);
extern int printf(const char *format, ...);
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

/* The installer keeps one transfer-buffer pointer in its own BSS. */
static struct HddInstallParams *HddInstallParam;

UmnWorkData UmnWork = {0};

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

int UmnWorkEnd = 0;
int UmnTexAddr = 0;
int UmnBgCubeXtx = 0;
int UmnBgCubeLex = 0;
MenuModelUnit *UmnModelUkn = 0;
MenuModelUnit *UmnModelSon = 0;
XglTaskScheduler *task_xmx = 0;
XglTaskScheduler *umn_task = 0;
int UmnMemory = 0;
int UmnWorkEndTop = 0;
UmnInterface2State *UmnInterface2 = 0;
/* Font control bytes passed to the existing font-script renderer. */
static const char D_00A11620[8] = "\x0f\x44\x00\x80\x0d\x03";

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
    sceVif1PkRef(packet, transfer_buffer, HDD_TEXTURE_QWC, 0, 0x51007000, 0);
    sceVif1PkRef(packet, TransEnv_1_00A0FE70, 2, 0, 0, 0);
    sceVif1PkRef(packet, transfer_buffer + HDD_TEXTURE_HALF_SIZE, HDD_TEXTURE_QWC, 0, 0x51007000, 0);
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
    xglFontPrint(HDD_MESSAGE_X, HDD_MESSAGE_Y, -1, D_00A11628);
    switch (command) {
    case HDD_INSTALL_STAGE_BEGIN:
        xglFontDebugPrintf(HDD_STATUS_X, HDD_STATUS_Y, D_00A11688);
        break;
    case HDD_INSTALL_STAGE_RESOURCE_READ:
        xglFontDebugPrintf(HDD_STATUS_X, HDD_STATUS_Y, D_00A11698);
        break;
    case HDD_INSTALL_STAGE_PARTITION_OPEN:
        xglFontDebugPrintf(HDD_STATUS_X, HDD_STATUS_Y, D_00A116B0);
        break;
    case HDD_INSTALL_STAGE_EXTEND:
        xglFontDebugPrintf(HDD_STATUS_X, HDD_STATUS_Y, D_00A116C0, offset);
        break;
    case HDD_INSTALL_STAGE_FORMAT:
        xglFontDebugPrintf(HDD_STATUS_X, HDD_STATUS_Y, D_00A116E0);
        break;
    case HDD_INSTALL_STAGE_MOUNT:
        xglFontDebugPrintf(HDD_STATUS_X, HDD_STATUS_Y, D_00A116F8);
        break;
    case HDD_INSTALL_STAGE_COPY:
        if (offset == 0) {
            xglFontDebugPrintf(HDD_STATUS_X, HDD_STATUS_Y, D_00A11708);
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
        xglFontPrint(HDD_MESSAGE_X, 80, -1, D_00A11718);
        print_args[0] = (u32)sztmp_5;
        print_args[1] = (u32)anim_4[(D_00A13654 >> 3) & 3];
        xglFontPrintf(152, 414, -1, print_args);
        D_00A13654++;
        print_args[0] = (u32)sztmp_6;
        percent = ((offset & 0xFFFF) * 100) / (offset >> 16);
        print_args[1] = percent;
        xglFontPrintf(168, 400, -1, print_args);
        step = print_args[1] / HDD_PROGRESS_PERCENT_PER_IMAGE;
        if (step != HddInstallParam->step && step < HDD_PROGRESS_IMAGES) {
            base = HddInstallParam->image_data;
            memset(&jpeg_request, 0, sizeof(jpeg_request));
            HddInstallParam->step = step;
            jpeg_request.source = base + ((int *)base)[step] + HDD_ARCHIVE_HEADER_SIZE;
            jpeg_request.destination = HddInstallParam->transfer_buffer;
            xglJpegDecode(&jpeg_request);
        }
        pad = &PadData;
        if ((pad->half_2a & PAD_CROSS) != 0) {
            xglSoundEffectNormalDirect(SE_CANCEL);
            HddInstallSync();
            while (1) {
                xglFontPrint(HDD_MESSAGE_X, HDD_MESSAGE_Y, -1, D_00A11738);
                if ((PadData.half_2a & PAD_CIRCLE) != 0) {
                    xglSoundEffectNormalDirect(SE_DECIDE);
                    *status = 1;
                    return 1;
                }
                if ((PadData.half_2a & PAD_CROSS) != 0) {
                    xglSoundEffectNormalDirect(SE_CANCEL);
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
    case HDD_INSTALL_STAGE_FINISH:
        switch (offset) {
        case 0:
            xglFontDebugPrintf(HDD_STATUS_X, HDD_STATUS_Y, D_00A117A0);
            break;
        case 1:
            xglFontDebugPrintf(HDD_STATUS_X, HDD_STATUS_Y, D_00A117B8);
            break;
        case 2:
            xglFontDebugPrintf(HDD_STATUS_X, HDD_STATUS_Y, D_00A117C8);
            break;
        case 3:
            xglFontDebugPrintf(HDD_STATUS_X, HDD_STATUS_Y, D_00A117D8);
            break;
        case 4:
            xglFontDebugPrintf(HDD_STATUS_X, HDD_STATUS_Y, D_00A117F0);
            break;
        }
        break;
    case HDD_INSTALL_STAGE_DONE:
        xglFontDebugPrintf(HDD_STATUS_X, HDD_STATUS_Y, D_00A11800);
        break;
    case HDD_INSTALL_FAIL_REMOUNT:
    case HDD_INSTALL_FAIL_UNMOUNT:
    case HDD_INSTALL_FAIL_CHECK_CLOSE:
    case HDD_INSTALL_FAIL_CHECK_WRITE:
    case HDD_INSTALL_FAIL_CHECK_OPEN:
    case HDD_INSTALL_FAIL_DATA_CLOSE:
    case HDD_INSTALL_FAIL_DATA_WRITE:
    case HDD_INSTALL_FAIL_DATA_OPEN:
    case HDD_INSTALL_FAIL_MOUNT:
    case HDD_INSTALL_FAIL_FORMAT:
    case HDD_INSTALL_FAIL_PARTITION_CLOSE:
    case HDD_INSTALL_FAIL_EXTEND:
    case HDD_INSTALL_FAIL_WRITE_HEADER:
    case HDD_INSTALL_FAIL_PARTITION_OPEN:
    case HDD_INSTALL_FAIL_RESOURCE:
        printf(D_00A11818, command, offset, offset);
        xglFontDebugPrintf(HDD_STATUS_X, HDD_STATUS_Y, D_00A11828, command, offset);
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
        frames = HDD_INPUT_DELAY;
        for (;;) {
            xglFontPrint(HDD_MESSAGE_X, HDD_MESSAGE_Y, -1, D_00A118B8);
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
                xglSoundEffectNormalDirect(SE_CANCEL);
                return;
            }
        }
        xglSoundEffectNormalDirect(SE_DECIDE);
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
        frames = HDD_INPUT_DELAY;
        if (result == 1) {
            for (;;) {
                xglFontPrint(HDD_MESSAGE_X, HDD_MESSAGE_Y, -1, D_00A11928);
                HddInstallSync();
                if (frames > 0) {
                    frames--;
                    continue;
                }
                pad = PadData.half_2a;
                if (pad & PAD_CIRCLE) {
                    xglSoundEffectNormalDirect(SE_DECIDE);
                    return;
                }
            }
        }
        for (;;) {
            xglFontPrint(HDD_MESSAGE_X, HDD_MESSAGE_Y, -1, D_00A11970);
            HddInstallSync();
            if (frames > 0) {
                frames--;
                continue;
            }
            pad = PadData.half_2a;
            if (pad & PAD_CIRCLE) {
                xglSoundEffectNormalDirect(SE_DECIDE);
                return;
            }
            if (pad & PAD_CROSS) {
                break;
            }
        }
        xglSoundEffectNormalDirect(SE_CANCEL);
        HddInstallSync();
    }
}

static void TitleHddInstallSubUninstall(void)
{
    int result;
    int frames;
    u16 pad;

    for (;;) {
        frames = HDD_INPUT_DELAY;
        for (;;) {
            xglFontPrint(HDD_MESSAGE_X, HDD_MESSAGE_Y, -1, D_00A119E0);
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
                xglSoundEffectNormalDirect(SE_CANCEL);
                return;
            }
        }
        xglSoundEffectNormalDirect(SE_DECIDE);
        result = xglHddCheck2();
        if (result == 0) {
            result = xglHddUninstall();
        }
        frames = HDD_INPUT_DELAY;
        HddInstallSync();
        if (result == 0) {
            for (;;) {
                xglFontPrint(HDD_MESSAGE_X, HDD_MESSAGE_Y, -1, D_00A11A58);
                HddInstallSync();
                if (frames > 0) {
                    frames--;
                    continue;
                }
                pad = PadData.half_2a;
                if (pad & PAD_CIRCLE) {
                    xglSoundEffectNormalDirect(SE_DECIDE);
                    return;
                }
            }
        }
        for (;;) {
            xglFontPrint(HDD_MESSAGE_X, HDD_MESSAGE_Y, -1, D_00A11AA8);
            HddInstallSync();
            if (frames > 0) {
                frames--;
                continue;
            }
            pad = PadData.half_2a;
            if (pad & PAD_CIRCLE) {
                xglSoundEffectNormalDirect(SE_DECIDE);
                xglHddCheck();
                return;
            }
            if (pad & PAD_CROSS) {
                break;
            }
        }
        xglSoundEffectNormalDirect(SE_CANCEL);
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
    if (command == HDD_SCREEN_INSTALL) {
        if (xglHddCheck() != 0) {
            command = HDD_SCREEN_MESSAGE_FIRST;
        }
    } else if (command == HDD_SCREEN_UNINSTALL) {
        if (xglHddCheck2() != 0) {
            xglHddActivate(0);
            command = HDD_SCREEN_REACTIVATE;
        }
    }
    buffer = WorkEnd;
    size = xglCdReadFile(D_00A11B20, buffer, 0, 1);
    transfer_buffer = (u8 *)(((u32)buffer + size + 63) & ~63);
    WorkEnd = transfer_buffer + HDD_TRANSFER_BUFFER_SIZE;
    params.image_data = buffer;
    params.step = 0;
    params.transfer_buffer = transfer_buffer;
    memset(&request, 0, 32);
    request.source = buffer + HDD_ARCHIVE_HEADER_SIZE;
    request.destination = transfer_buffer;
    xglJpegDecode(&request);
    HddInstallParam = &params;
    HddInstallSync();
    xglFontPrintDirectOT(-1, D_00A11620);
    switch (command) {
    case HDD_SCREEN_INSTALL:
        TitleHddInstallSubInstall();
        break;
    case HDD_SCREEN_UNINSTALL:
        TitleHddInstallSubUninstall();
        break;
    case HDD_SCREEN_MESSAGE_FIRST:
    case HDD_SCREEN_MESSAGE_FIRST + 1:
    case HDD_SCREEN_MESSAGE_LAST:
        count = HDD_INPUT_DELAY;
        for (;;) {
            xglFontPrint(HDD_MESSAGE_X, HDD_MESSAGE_Y, -1, mes_7[command - HDD_SCREEN_MESSAGE_FIRST]);
            xglFontPrintDirectOT(-1, D_00A11C90);
            HddInstallSync();
            if (count > 0) {
                count--;
            } else if ((PadData.half_2a & PAD_CIRCLE) != 0) {
                xglSoundEffectNormalDirect(SE_DECIDE);
                break;
            }
        }
        break;
    case HDD_SCREEN_REACTIVATE:
        count = HDD_INPUT_DELAY;
        for (;;) {
            xglFontPrint(HDD_MESSAGE_X, HDD_MESSAGE_Y, -1, D_00A11C60);
            xglFontPrintDirectOT(-1, D_00A11CD8);
            HddInstallSync();
            if (count > 0) {
                count--;
            } else if ((PadData.half_2a & PAD_CIRCLE) != 0) {
                xglSoundEffectNormalDirect(SE_DECIDE);
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
