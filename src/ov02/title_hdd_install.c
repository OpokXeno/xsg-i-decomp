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

/* Gauge block read by tyaDrawGauge: the same layout as TyaGaugeArgs (src/main/tya_draw_gauge.h). */
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
static unsigned char count_3;
extern const char installingText[];
extern const char installStartLabel[];
extern const char createPartitionLabel[];
extern const char writeResourceLabel[];
extern const char createSubPartitionFormat[];
extern const char formatPartitionLabel[];
extern const char mountPartitionLabel[];
extern const char startInstallLabel[];
extern const char installAbortButtonText[];
extern const char installAbortPromptText[];
extern const char checkFileCreateLabel[];
extern const char checkFileWriteLabel[];
extern const char checkFileCloseLabel[];
extern const char checkFileUnmountLabel[];
extern const char checkFileMountLabel[];
extern const char installCompleteLabel[];
extern const char installCallbackFormat[];
extern const char installErrorFormat[];
static char anim_4[4];
static char sztmp_5[3];
static char sztmp_6[6];
static const char *mes_7[3];
extern const char installStartPromptText[];
extern const char installCompleteText[];
extern const char installFailedText[];
extern const char uninstallStartPromptText[];
extern const char uninstallCompleteText[];
extern const char uninstallFailedText[];
extern const char installArchivePath[];
extern const char installAccessFailText[];
extern const char installCancelText[];
extern const char uninstallCancelText[];

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
static const char installScreenFontText[8] = "\x0f\x44\x00\x80\x0d\x03";

/*
 * Sends the VIF packet with the render state and both transfer-buffer halves, then
 * draws the screen font.
 */
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
    xglFontPrintDirectOT(-1, installScreenFontText);
}

/*
 * Progress callback for xglHddInstall. Draws the stage text and the copy gauge,
 * decodes the next progress image and handles the abort prompt. Returns nonzero to
 * stop the install.
 */
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
    xglFontPrint(HDD_MESSAGE_X, HDD_MESSAGE_Y, -1, installingText);
    switch (command) {
    case HDD_INSTALL_STAGE_BEGIN:
        xglFontDebugPrintf(HDD_STATUS_X, HDD_STATUS_Y, installStartLabel);
        break;
    case HDD_INSTALL_STAGE_RESOURCE_READ:
        xglFontDebugPrintf(HDD_STATUS_X, HDD_STATUS_Y, createPartitionLabel);
        break;
    case HDD_INSTALL_STAGE_PARTITION_OPEN:
        xglFontDebugPrintf(HDD_STATUS_X, HDD_STATUS_Y, writeResourceLabel);
        break;
    case HDD_INSTALL_STAGE_EXTEND:
        xglFontDebugPrintf(HDD_STATUS_X, HDD_STATUS_Y, createSubPartitionFormat, offset);
        break;
    case HDD_INSTALL_STAGE_FORMAT:
        xglFontDebugPrintf(HDD_STATUS_X, HDD_STATUS_Y, formatPartitionLabel);
        break;
    case HDD_INSTALL_STAGE_MOUNT:
        xglFontDebugPrintf(HDD_STATUS_X, HDD_STATUS_Y, mountPartitionLabel);
        break;
    case HDD_INSTALL_STAGE_COPY:
        if (offset == 0) {
            xglFontDebugPrintf(HDD_STATUS_X, HDD_STATUS_Y, startInstallLabel);
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
        xglFontPrint(HDD_MESSAGE_X, 80, -1, installAbortButtonText);
        print_args[0] = (u32)sztmp_5;
        print_args[1] = (u32)anim_4[(count_3 >> 3) & 3];
        xglFontPrintf(152, 414, -1, print_args);
        count_3++;
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
        /* Cross asks for an abort: Circle aborts, Cross resumes the copy. */
        pad = &PadData;
        if ((pad->half_2a & PAD_CROSS) != 0) {
            xglSoundEffectNormalDirect(SE_CANCEL);
            HddInstallSync();
            while (1) {
                xglFontPrint(HDD_MESSAGE_X, HDD_MESSAGE_Y, -1, installAbortPromptText);
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
            xglFontDebugPrintf(HDD_STATUS_X, HDD_STATUS_Y, checkFileCreateLabel);
            break;
        case 1:
            xglFontDebugPrintf(HDD_STATUS_X, HDD_STATUS_Y, checkFileWriteLabel);
            break;
        case 2:
            xglFontDebugPrintf(HDD_STATUS_X, HDD_STATUS_Y, checkFileCloseLabel);
            break;
        case 3:
            xglFontDebugPrintf(HDD_STATUS_X, HDD_STATUS_Y, checkFileUnmountLabel);
            break;
        case 4:
            xglFontDebugPrintf(HDD_STATUS_X, HDD_STATUS_Y, checkFileMountLabel);
            break;
        }
        break;
    case HDD_INSTALL_STAGE_DONE:
        xglFontDebugPrintf(HDD_STATUS_X, HDD_STATUS_Y, installCompleteLabel);
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
        printf(installCallbackFormat, command, offset, offset);
        xglFontDebugPrintf(HDD_STATUS_X, HDD_STATUS_Y, installErrorFormat, command, offset);
        break;
    }
    HddInstallSync();
    return 0;
}

/*
 * Runs the install screen: the start prompt, the install, then the completion or
 * failure prompt. Returns when the player leaves the screen.
 */
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
            xglFontPrint(HDD_MESSAGE_X, HDD_MESSAGE_Y, -1, installStartPromptText);
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
        /* Check the disk, then run the install when the check passed. */
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
                xglFontPrint(HDD_MESSAGE_X, HDD_MESSAGE_Y, -1, installCompleteText);
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
        /* Failure prompt: Circle leaves the screen, Cross returns to the start prompt. */
        for (;;) {
            xglFontPrint(HDD_MESSAGE_X, HDD_MESSAGE_Y, -1, installFailedText);
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

/* Runs the uninstall screen with the same prompt flow as the install screen. */
static void TitleHddInstallSubUninstall(void)
{
    int result;
    int frames;
    u16 pad;

    for (;;) {
        frames = HDD_INPUT_DELAY;
        for (;;) {
            xglFontPrint(HDD_MESSAGE_X, HDD_MESSAGE_Y, -1, uninstallStartPromptText);
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
                xglFontPrint(HDD_MESSAGE_X, HDD_MESSAGE_Y, -1, uninstallCompleteText);
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
            xglFontPrint(HDD_MESSAGE_X, HDD_MESSAGE_Y, -1, uninstallFailedText);
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

/*
 * Entry point of the HDD install screen. Reads the install archive, decodes its first
 * image, then shows the install, uninstall or message screen selected by command.
 */
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
    /* Read the archive into the free work area and place the transfer buffer after it. */
    buffer = WorkEnd;
    size = xglCdReadFile(installArchivePath, buffer, 0, 1);
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
    xglFontPrintDirectOT(-1, installScreenFontText);
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
            xglFontPrintDirectOT(-1, installCancelText);
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
            xglFontPrint(HDD_MESSAGE_X, HDD_MESSAGE_Y, -1, installAccessFailText);
            xglFontPrintDirectOT(-1, uninstallCancelText);
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

extern const char installUnformattedText[56];
extern const char installNoSpaceText[];
extern const char installAccessFailText[];

static char anim_4[4] = "-\177|/";

static char sztmp_5[3] = "%c";

static char sztmp_6[6] = "%3d\033\022";

static const char * mes_7[3] = { installAccessFailText, installUnformattedText, installNoSpaceText };

int UmnVecMat = 0;

int UmnModel[320] = { 0 };

u32 UmnBackRgba = 0;

const char installingText[96] = "\031\001\245\244\245\363\245\271\245\310\241\274\245\353\303\346\244\307\244\271\241\243\241\310\243\320\243\354\243\341\243\371\243\323\243\364\243\341\243\364\243\351\243\357\243\356\010\210\000\010\243\262\241\311\313\334\302\316\244\316\n\n\305\305\270\273\244\362\300\332\244\351\244\312\244\244\244\307\244\257\244\300\244\265\244\244\241\243";

const char installStartLabel[16] = "Install Start";

const char createPartitionLabel[24] = "Create Partition";

const char writeResourceLabel[16] = "Write Resource";

const char createSubPartitionFormat[32] = "Create Sub Partition %1d";

const char formatPartitionLabel[24] = "Format Partition";

const char mountPartitionLabel[16] = "Mount Partition";

const char startInstallLabel[16] = "Start Install";

const char installAbortButtonText[32] = "\031\000\014  \200\241\337\014\200\200\200\245\334\245\277\245\363\241\247\303\346\303\307";

const char installAbortPromptText[104] = "\245\244\245\363\245\271\245\310\241\274\245\353\244\317\244\336\244\300\264\260\316\273\244\267\244\306\244\244\244\336\244\273\244\363\241\243\n\n\303\346\303\307\244\267\244\336\244\271\244\253\241\251\n\n\014\200  \241\373\014\200\200\200\245\334\245\277\245\363\241\247\303\346\303\307\241\277\014  \200\241\337\014\200\200\200\245\334\245\277\245\363\241\247\267\321\302\263";

const char checkFileCreateLabel[24] = "CheckFile Create";

const char checkFileWriteLabel[16] = "CheckFile Write";

const char checkFileCloseLabel[16] = "CheckFile Close";

const char checkFileUnmountLabel[24] = "CheckFile Unmount";

const char checkFileMountLabel[16] = "CheckFile Mount";

const char installCompleteLabel[24] = "Install Complete";

const char installCallbackFormat[16] = "%d:%d(%08x)\n";

const char installErrorFormat[24] = "Error %2d/%8x";

const char installStartPromptText[112] = "\245\274\245\316\245\265\241\274\245\254\244\316\245\244\245\363\245\271\245\310\241\274\245\353\244\362\263\253\273\317\244\267\244\336\244\271\241\243\n\n\244\350\244\355\244\267\244\244\244\307\244\271\244\253\241\251\n\n\014\200  \241\373\014\200\200\200\245\334\245\277\245\363\241\247\263\253\273\317\241\277\014  \200\241\337\014\200\200\200\245\334\245\277\245\363\241\247\245\255\245\343\245\363\245\273\245\353";

const char installCompleteText[72] = "\245\244\245\363\245\271\245\310\241\274\245\353\244\254\300\265\276\357\244\313\275\252\316\273\244\267\244\336\244\267\244\277\241\243\n\n\014\200  \241\373\014\200\200\200\245\334\245\277\245\363\241\247\245\277\245\244\245\310\245\353\244\313\314\341\244\353";

const char installFailedText[112] = "\245\244\245\363\245\271\245\310\241\274\245\353\244\313\274\272\307\324\244\267\244\336\244\267\244\277\241\243\n\n\245\244\245\363\245\271\245\310\241\274\245\353\244\362\303\346\273\337\244\267\244\336\244\271\244\253\241\251\n\n\014\200  \241\373\014\200\200\200\245\334\245\277\245\363\241\247\244\317\244\244\241\277\014  \200\241\337\014\200\200\200\245\334\245\277\245\363\241\247\244\244\244\244\244\250";

const char uninstallStartPromptText[120] = "\245\274\245\316\245\265\241\274\245\254\244\316\245\242\245\363\245\244\245\363\245\271\245\310\241\274\245\353\244\362\263\253\273\317\244\267\244\336\244\271\241\243\n\n\244\350\244\355\244\267\244\244\244\307\244\271\244\253\241\251\n\n\014\200  \241\373\014\200\200\200\245\334\245\277\245\363\241\247\263\253\273\317\241\277\014  \200\241\337\014\200\200\200\245\334\245\277\245\363\241\247\245\255\245\343\245\363\245\273\245\353";

const char uninstallCompleteText[80] = "\245\242\245\363\245\244\245\363\245\271\245\310\241\274\245\353\244\254\300\265\276\357\244\313\275\252\316\273\244\267\244\336\244\267\244\277\241\243\n\n\014\200  \241\373\014\200\200\200\245\334\245\277\245\363\241\247\245\277\245\244\245\310\245\353\244\313\314\341\244\353";

const char uninstallFailedText[120] = "\245\242\245\363\245\244\245\363\245\271\245\310\241\274\245\353\244\313\274\272\307\324\244\267\244\336\244\267\244\277\241\243\n\n\245\242\245\363\245\244\245\363\245\271\245\310\241\274\245\353\244\362\303\346\273\337\244\267\244\336\244\271\244\253\241\251\n\n\014\200  \241\373\014\200\200\200\245\334\245\277\245\363\241\247\244\317\244\244\241\277\014  \200\241\337\014\200\200\200\245\334\245\277\245\363\241\247\244\244\244\244\244\250";

const char installArchivePath[24] = "data\\yajima\\hddi.bin";

const char installNoSpaceText[240] = "\245\317\241\274\245\311\245\307\245\243\245\271\245\257\245\311\245\351\245\244\245\326\244\313\241\242\245\274\245\316\245\265\241\274\245\254\244\316\n\n\245\244\245\363\245\271\245\310\241\274\245\353\244\313\311\254\315\327\244\312\266\365\244\255\315\306\316\314\244\254\302\255\244\352\244\336\244\273\244\363\241\243\n\n\010\210\377\370\241\312\266\365\244\255\315\306\316\314\244\310\244\267\244\306\031\001\243\261\243\267\243\271\243\262\243\315\243\302\031\000\260\312\276\345\311\254\315\327\244\307\244\271\241\243\010\210\377\370\241\313\n\n\n\n\302\276\244\316\245\242\245\327\245\352\245\261\241\274\245\267\245\347\245\363\244\362\245\242\245\363\245\244\245\363\245\271\245\310\241\274\245\353\244\271\244\353\244\310\241\242\n\n\245\244\245\363\245\271\245\310\241\274\245\353\244\307\244\255\244\353\244\350\244\246\244\313\244\312\244\353\262\304\307\275\300\255\244\254\244\242\244\352\244\336\244\271\241\243";

const char installUnformattedText[56] = "\245\317\241\274\245\311\245\307\245\243\245\271\245\257\245\311\245\351\245\244\245\326\244\254\245\325\245\251\241\274\245\336\245\303\245\310\244\265\244\354\244\306\n\n\244\244\244\336\244\273\244\363\241\243";

const char installAccessFailText[48] = "\245\317\241\274\245\311\245\307\245\243\245\271\245\257\245\311\245\351\245\244\245\326\244\313\245\242\245\257\245\273\245\271\244\307\244\255\244\336\244\273\244\363\241\243";

const char installCancelText[72] = "\n\n\n\n\245\244\245\363\245\271\245\310\241\274\245\353\244\362\303\346\273\337\244\267\244\336\244\271\241\243\n\n\014\200  \241\373\014\200\200\200\245\334\245\277\245\363\241\247\245\277\245\244\245\310\245\353\244\313\314\341\244\353";

const char uninstallCancelText[72] = "\n\n\n\n\245\242\245\363\245\244\245\363\245\271\245\310\241\274\245\353\244\362\303\346\273\337\244\267\244\336\244\271\241\243\n\n\014\200  \241\373\014\200\200\200\245\334\245\277\245\363\241\247\245\277\245\244\245\310\245\353\244\313\314\341\244\353";

static unsigned char count_3;
