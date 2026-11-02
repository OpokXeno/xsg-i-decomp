/*
 * OV02 original TU 0: 0x00a00000..0x00a00850 (6 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_2.h"
#include "main/xgl_dma.h"
#include "main/xgl_font.h"
#include "main/xgl_mc.h"
#include "main/xgl_render.h"
#include "main/xgl_sound.h"
#include "main/xgl_thread.h"
#include "ov02/ov02.h"

/* Position of the message the device checks print. */
#define LOGO_MESSAGE_X 16
#define LOGO_MESSAGE_Y 64

/* Frames xglMcMain is polled for after the memory cards settle. */
#define LOGO_CARD_SETTLE_FRAMES 8

/* ipuplay: the movie stream buffer, its size, and the buffer the frames are displayed from. */
#define LOGO_MOVIE_STREAM_BASE 0x01000000
#define LOGO_MOVIE_STREAM_SIZE 0x80000
#define LOGO_MOVIE_DISPLAY_BASE 0x01100000

/* Quadwords of TestEnv_1_00A0F690 sent to the GIF. */
#define LOGO_TEST_ENV_QWC 9

typedef struct HddCheckRequest {
    int result;
    u8 *buffer;
    int size;
} HddCheckRequest;

typedef struct HddNameTable {
    const char *name[3];
} HddNameTable;

typedef union McSlotState {
    int value;
    struct {
        s16 slot0;
        s16 slot1;
    } slot;
} McSlotState;

typedef struct McCheckRequest {
    int status;
    unsigned char unmodeled_04[4];
    unsigned char *workspace_end;
    unsigned char unmodeled_0c[4];
} McCheckRequest;

typedef struct LogoMovieInfo {
    u8 unmodeled_00[0x48];
    u32 frame_width;
    u8 unmodeled_4c[0x44];
    u32 stream_base;
    u32 stream_size;
    u8 unmodeled_98[0x18];
    u32 display_base;
    u32 display_end;
    u8 unmodeled_b8[0x11];
    u8 stopped;
    u8 unmodeled_ca[6];
} LogoMovieInfo;

typedef struct LogoGifTag {
    u32 control_low;
    u32 control_high;
    u32 registers_low;
    u32 registers_high;
} LogoGifTag;

typedef struct LogoGifAdCommand {
    u64 value;
    u32 register_address;
    u32 unused;
} LogoGifAdCommand;

typedef struct LogoGifRgbaq {
    u32 red;
    u32 green;
    u32 blue;
    u32 alpha;
} LogoGifRgbaq;

typedef struct LogoGifUv {
    u32 u;
    u32 v;
    u32 unused_08;
    u32 unused_0c;
} LogoGifUv;

typedef struct LogoGifXyz2 {
    u16 x;
    u16 unused_x;
    u16 y;
    u16 unused_y;
    u32 z;
    u32 unused;
} LogoGifXyz2;

typedef struct LogoTestEnvironment {
    LogoGifTag tag;
    LogoGifAdCommand flush_texture_cache;
    LogoGifAdCommand set_clamp_1;
    u64 command;
    u32 set_tex0_register_address;
    u32 set_tex0_unused;
    LogoGifRgbaq color;
    LogoGifUv texture_coordinates_0;
    LogoGifXyz2 vertex_0;
    LogoGifUv texture_coordinates_1;
    LogoGifXyz2 vertex_1;
} LogoTestEnvironment;

extern int xglHddMount(void);
extern int xglHddMcExist(HddCheckRequest *request);
extern int xglHddMcCheck(HddCheckRequest *request);
extern int xglHddActivate(int state);
extern u8 *WorkEnd;
extern u8 carddata_0[];
extern const HddNameTable D_00A11048;
extern const HddNameTable D_00A11148;
extern const char D_00A10D30[];
extern const char D_00A10E38[];
extern const char D_00A10E98[];
extern const char D_00A11158[];
extern const char D_00A11250[];
extern const char D_00A112F0[];
extern const char D_00A11338[];
extern int xglMcRequest(int card, int command, McCheckRequest *request, int *result);
extern const char D_00A11358[];
extern const char D_00A113C8[];
extern const char D_00A113E8[];
extern const char D_00A11430[];
extern const char D_00A11478[];
extern const char D_00A114C8[];
extern const char D_00A11520[];
extern void Logo(void);
extern const char D_00A115A0[];
extern void xglMovieInfoInit(LogoMovieInfo *movie);
extern int xglMovieOpen(LogoMovieInfo *movie, const char *path);
extern int xglMoviePlay(LogoMovieInfo *movie);
extern int xglMovieClose(LogoMovieInfo *movie);
extern void FlushCache(int mode);

static int ipuplay(const char *path);

/* Defined here: D_00A10D28, TestEnv_1_00A0F690, D_00A115A8, D_00A115B8 (data order is section order). */
const char D_00A10D28[8] = "\x19\x03";

static int device_check_sync(void)
{
    int status;

    status = xglMcMain();
    xglSleep();
    xglFontPrintDirectOT(-1, D_00A10D28);
    return status;
}




static int hdd_check(void)
{
    int status;
    HddNameTable names_a;
    HddNameTable names_b;
    HddCheckRequest request;
    const char *text_a;
    const char *text_b;

    status = xglHddMount();
    if (status == -1 || status == -99)
    {
        return 1;
    }

    for (;;)
    {
        if (status != -5)
        {
            break;
        }
        xglFontPrint(LOGO_MESSAGE_X, LOGO_MESSAGE_Y, -1, D_00A10D30);
        if (PadData.half_2a & PAD_CIRCLE)
        {
            xglSoundEffectNormalDirect(SE_DECIDE);
            break;
        }
        device_check_sync();
    }
    device_check_sync();

    if (status == -3)
    {
        text_a = D_00A10E38;
        text_b = D_00A10E98;
    }
    else
    {
        names_a = D_00A11048;
        names_b = D_00A11148;
        request.buffer = WorkEnd;
        request.result = -1;
        if (xglHddMcExist(&request) > 0)
        {
            return 0;
        }
        request.size = MC_SAVE_DATA_SIZE;
        request.buffer = carddata_0;
        status = xglHddMcCheck(&request);
        if (status == -1)
        {
            for (;;)
            {
                xglFontPrint(LOGO_MESSAGE_X, LOGO_MESSAGE_Y, -1, D_00A11158);
                if (PadData.half_2a & PAD_CIRCLE)
                {
                    break;
                }
                device_check_sync();
            }
            xglSoundEffectNormalDirect(SE_DECIDE);
            device_check_sync();
            xglHddActivate(2);
            return 1;
        }
        if (status < -1)
        {
            text_a = names_a.name[-status - 2];
            text_b = names_b.name[-status - 2];
        }
        else
        {
            return 0;
        }
    }

    for (;;)
    {
        xglFontPrint(LOGO_MESSAGE_X, LOGO_MESSAGE_Y, -1, text_a);
        xglFontPrintDirectOT(-1, D_00A11250);
        if (PadData.half_2a & PAD_CIRCLE)
        {
            xglSoundEffectNormalDirect(SE_DECIDE);
            break;
        }
        if (PadData.half_2a & PAD_CROSS)
        {
            xglSoundEffectNormalDirect(SE_CANCEL);
            device_check_sync();
            for (;;)
            {
                xglFontPrint(LOGO_MESSAGE_X, LOGO_MESSAGE_Y, -1, D_00A112F0);
                xglFontPrintDirectOT(-1, text_b);
                xglFontPrintDirectOT(-1, D_00A11338);
                if (PadData.half_2a & PAD_CIRCLE)
                {
                    break;
                }
                device_check_sync();
            }
            xglSoundEffectNormalDirect(SE_DECIDE);
            break;
        }
        device_check_sync();
    }
    device_check_sync();
    return 1;
}




static int mc_check(void)
{
    McSlotState screen_state[4];
    int slot_status[2];
    McCheckRequest request;
    int free_space[2];
    McSlotState wait_state[4];
    const char *warning;
    const char *capacity_warning;
    int warning_mask;
    int i;

    for (i = LOGO_CARD_SETTLE_FRAMES - 1; i >= 0; i--)
    {
        screen_state[0].value = device_check_sync();
    }

    slot_status[0] = 0;
    slot_status[1] = 0;
    warning = 0;
    free_space[0] = 0;
    free_space[1] = 0;

    for (;;)
    {
        xglMcRequest(0, MC_REQUEST_FREE_SPACE, 0, &slot_status[0]);
        xglMcRequest(1, MC_REQUEST_FREE_SPACE, 0, &slot_status[1]);
        request.status = -1;
        request.workspace_end = WorkEnd;
        xglMcRequest(0, MC_REQUEST_DIRECTORY, &request, &free_space[0]);
        xglMcRequest(1, MC_REQUEST_DIRECTORY, &request, &free_space[1]);

        do
        {
            screen_state[0].value = device_check_sync();
        } while (screen_state[0].slot.slot0 == MC_STATUS_BUSY || screen_state[0].slot.slot0 == MC_STATUS_ERROR ||
                 screen_state[0].slot.slot1 == MC_STATUS_BUSY || screen_state[0].slot.slot1 == MC_STATUS_ERROR);

        for (i = LOGO_CARD_SETTLE_FRAMES - 1; i >= 0; i--)
        {
            screen_state[0].value = device_check_sync();
        }

        capacity_warning = D_00A11358;
        if (screen_state[0].slot.slot0 != MC_STATUS_READY && screen_state[0].slot.slot0 != MC_STATUS_UNFORMATTED &&
            screen_state[0].slot.slot1 != MC_STATUS_READY && screen_state[0].slot.slot1 != MC_STATUS_UNFORMATTED)
        {
            warning = D_00A113C8;
        }
        else
        {
            warning_mask = 0;
            do
            {
                if (screen_state[0].slot.slot0 == MC_STATUS_UNFORMATTED || screen_state[0].slot.slot1 == MC_STATUS_UNFORMATTED)
                {
                    break;
                }
                if (screen_state[0].slot.slot0 == MC_STATUS_READY)
                {
                    if (free_space[0] <= 0 && slot_status[0] < 165)
                    {
                        warning_mask = 1;
                    }
                    else
                    {
                        break;
                    }
                }
                if (screen_state[0].slot.slot1 == MC_STATUS_READY)
                {
                    if (free_space[1] <= 0 && slot_status[1] < 165)
                    {
                        warning_mask = warning_mask | 2;
                    }
                    else
                    {
                        warning_mask = 0;
                    }
                }
            } while (0);
            switch (warning_mask)
            {
                case 0:
                    warning = 0;
                    break;
                case 1:
                    warning = D_00A113E8;
                    break;
                case 2:
                    warning = D_00A11430;
                    break;
                case 3:
                    warning = D_00A11478;
                    break;
                default:
                    break;
            }
        }

        while (warning != 0)
        {
            xglFontPrint(LOGO_MESSAGE_X, LOGO_MESSAGE_Y, -1, warning);
            if (capacity_warning != 0)
            {
                xglFontPrintDirectOT(-1, capacity_warning);
            }
            xglFontPrintDirectOT(-1, D_00A114C8);

            if (PadData.half_2a & PAD_CIRCLE)
            {
                xglSoundEffectNormalDirect(SE_DECIDE);
                warning = 0;
                break;
            }

            if (PadData.half_2a & PAD_CROSS)
            {
                xglSoundEffectNormalDirect(SE_CANCEL);
                do
                {
                    wait_state[0].value = device_check_sync();
                    xglFontPrint(LOGO_MESSAGE_X, LOGO_MESSAGE_Y, -1, D_00A11520);
                    if (PadData.half_2a & PAD_CROSS)
                    {
                        xglSoundEffectNormalDirect(SE_DECIDE);
                        break;
                    }
                } while (screen_state[0].value == wait_state[0].value);
                break;
            }

            wait_state[0].value = device_check_sync();
            if (screen_state[0].value != wait_state[0].value)
            {
                break;
            }
        }

        if (warning == 0)
        {
            break;
        }
    }

    return device_check_sync();
}


void LogoFirst(void)
{
    xglRenderClearFrame();
    xglRenderClearColor(OV02_CLEAR_COLOR_BLACK);
    xglSleep();
    xglRenderClearOff();
    if (hdd_check() != 0)
    {
        mc_check();
    }
    xglFontPrintDirectOT(-1, D_00A115A0);
    Logo();
}

static LogoTestEnvironment TestEnv_1_00A0F690 = {
    .tag = { 0x00008001, 0x808B4000, 0x53531EEE, 0 },
    .flush_texture_cache = { 0, 0x3F, 0 },
    .set_clamp_1 = { 0x00000000007FFFF0ULL, 0x08, 0 },
    .command = 0,
    .set_tex0_register_address = 0x06,
    .set_tex0_unused = 0,
    .color = { 0x80, 0x80, 0x80, 0x80 },
    .texture_coordinates_0 = { 0, 0, 0, 0 },
    .vertex_0 = { 0x6FF8, 0, 0x71F7, 0, 0x40000000, 0 },
    .texture_coordinates_1 = { 0x2000, 0x1C00, 0, 0 },
    .vertex_1 = { 0x8FF8, 0, 0x8DF7, 0, 0x40000000, 0 },
};

static int ipuplay(const char *path)
{
    LogoMovieInfo movie;
    int skipped;
    u32 frame_buffer_base;

    skipped = 0;
    xglMovieInfoInit(&movie);
    frame_buffer_base = LOGO_MOVIE_STREAM_BASE;
    movie.stream_base = frame_buffer_base;
    movie.stream_size = LOGO_MOVIE_STREAM_SIZE;
    movie.stopped = 0;
    frame_buffer_base = LOGO_MOVIE_DISPLAY_BASE;
    xglMovieOpen(&movie, path);
    movie.display_base = frame_buffer_base;
    frame_buffer_base += movie.frame_width * 16;
    movie.display_end = frame_buffer_base;

    for (;;)
    {
        xglSRand();
        xglFRand();

        if (PadData.half_2a & PAD_START)
        {
            skipped = 1;
            xglSoundEffectNormalDirect(SE_DECIDE);
            break;
        }

        if (xglMoviePlay(&movie) != 0)
        {
            break;
        }

        /* The display-buffer value occupies bits 5 and above of the low command word. */
        TestEnv_1_00A0F690.command = 0x2000000640000000ULL |
                                     (u64)~(~0x24020000U & ~((u32)sRender.flip_base << 5));
        FlushCache(0);
        xglDmaDirectNormal(XGL_DMA_CHANNEL_GIF, (u32)&TestEnv_1_00A0F690, LOGO_TEST_ENV_QWC);
        xglSleep();
    }

    xglMovieClose(&movie);
    xglSleep();
    return skipped;
}

const char D_00A115A8[] = "data\\namco.ipu";
const char D_00A115B8[] = "data\\logo_msi.ipu";

void Logo(void)
{
    xglRenderClearFrame();
    xglRenderClearColor(OV02_CLEAR_COLOR_BLACK);
    xglSleep();
    xglRenderClearDepth();
    if (ipuplay(D_00A115A8) == 0)
    {
        /*
         * The original calls ipuplay with jal and returns through the
         * shared epilogue instead of a sibling jump. Under this TU's
         * compiler the call stays out of tail position only inside a
         * loop construct, which is the shape a do/while (0) statement
         * gives it; the call alone as the block's last statement
         * compiles to `j ipuplay`.
         */
        do {
            ipuplay(D_00A115B8);
        } while (0);
    }
}
