#include "common.h"
#include "shared.h"
#include "main/xgl_packet.h"
#include "tya_display_setting.h"
#include "main/game_over.h"

#define PLAY_RESET_SOUND_EFFECT()                                       \
    do {                                                                \
        xglSoundEffectNormalDirect(1);                                  \
    } while (0)


/* Display adjustment screen: each step moves the picture by one scanline
 * vertically or HORIZONTAL_STEP dot-clock units sideways, up to LIMIT steps
 * either way. The test image is two halves of IMAGE_HALF_QWORDS quadwords. */
#define TYA_DISPLAY_WIDTH 512
#define TYA_DISPLAY_HEIGHT 448
#define TYA_DISPLAY_OFFSET_LIMIT 20
#define TYA_DISPLAY_HORIZONTAL_STEP 5
#define TYA_IMAGE_HALF_QWORDS 0x7000
#define TYA_IMAGE_HALF_BYTES 0x70000
#define TYA_PAD_ANY_DIRECTION (PAD_UP | PAD_RIGHT | PAD_DOWN | PAD_LEFT)

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

extern DrawImageRenderState sRender;

extern void sceVif1PkCnt(XglPacket *packet, int count);
extern void sceVif1PkAddDataN(XglPacket *packet, const void *data, int count);

const char displaySettingImagePath[24] = "data\\yajima\\mnt_set.jpg";

const char displayHalfwordFormat[20] = "%04x %04x\n%04x %04x";

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
    /* One packed A+D loop, with the end-of-packet flag set. */
    .gif_tag = { 0x00008001, 0x10000000, 0x0000000E, 0 },
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
    sceVif1PkRef(packet, framebuffer, TYA_IMAGE_HALF_QWORDS, 0, 0x51007000, 0);
    sceVif1PkRef(packet, &TransEnv_1_0036AC20, 2, 0, 0, 0);
    sceVif1PkRef(packet, framebuffer + TYA_IMAGE_HALF_BYTES, TYA_IMAGE_HALF_QWORDS, 0, 0x51007000, 0);
    sceVif1PkRef(packet, &FlushEnv_2_0036AC40, 3, 0, 0, 0);
}

void tyaDisplaySetting(u8 *framebuffer_or_mode)
{
    short saved_x;
    short saved_y;
    GsDispEnvDisplayView environment;

    sceGsSetDefDispEnv(&environment, 0, TYA_DISPLAY_WIDTH, TYA_DISPLAY_HEIGHT, 0, 0);
    saved_x = ((TyaSaveDataRecord *)SaveData)->horizontal_display_offset;
    saved_y = ((TyaSaveDataRecord *)SaveData)->vertical_display_offset;

    if (framebuffer_or_mode == (u8 *)0) {
        DispEnv.display.dx = environment.display.dx + saved_x * TYA_DISPLAY_HORIZONTAL_STEP;
        DispEnv.display.dy = environment.display.dy + saved_y;
        return;
    }
    if (framebuffer_or_mode == (u8 *)1) {
        ((TyaSaveDataRecord *)SaveData)->horizontal_display_offset = 0;
        DispEnv.display.dx = environment.display.dx;
        DispEnv.display.dy = environment.display.dy;
        ((TyaSaveDataRecord *)SaveData)->vertical_display_offset = 0;
        return;
    }

    {
        u8 *framebuffer = framebuffer_or_mode;
        u8 *image_source;

        image_source = framebuffer_or_mode + 0x100000;
        xglCdReadFile(displaySettingImagePath, image_source, 0, 1);
        {
            TyaJpegRequest jpeg_request;

            memset(&jpeg_request, 0, sizeof(jpeg_request));
            jpeg_request.source = image_source;
            jpeg_request.destination = framebuffer;
            xglJpegDecode(&jpeg_request);
        }
        {
            short horizontal;
            short vertical;
            int directions;

            for (;;) {
                horizontal = ((TyaSaveDataRecord *)SaveData)->horizontal_display_offset;
                vertical = ((TyaSaveDataRecord *)SaveData)->vertical_display_offset;
                directions = PadData.mappedRepeat;

                if (directions & TYA_PAD_ANY_DIRECTION) {
                    xglSoundEffectNormalDirect(SE_CURSOR);
                    directions = PadData.mappedRepeat;
                }
                if (directions & PAD_RIGHT)
                {
                    horizontal++;
                }
                if (directions & PAD_LEFT)
                {
                    horizontal--;
                }
                if (directions & PAD_DOWN)
                {
                    vertical++;
                }
                if (directions & PAD_UP)
                {
                    vertical--;
                }

                {
                    /* The halfword event snapshot is refreshed after the sound call. */
                    int input_event = PadData.half_2a;
                    if (input_event & PAD_SQUARE) {
                        PLAY_RESET_SOUND_EFFECT();
                        input_event = PadData.half_2a;
                        horizontal = 0;
                        vertical = 0;
                    }
                    {
                        if (input_event & PAD_CROSS) {
                            horizontal = saved_x;
                            vertical = saved_y;
                        }
                        if (horizontal < -TYA_DISPLAY_OFFSET_LIMIT) {
                            horizontal = -TYA_DISPLAY_OFFSET_LIMIT;
                        }
                        if (vertical < -TYA_DISPLAY_OFFSET_LIMIT) {
                            vertical = -TYA_DISPLAY_OFFSET_LIMIT;
                        }
                        if (horizontal > TYA_DISPLAY_OFFSET_LIMIT) {
                            horizontal = TYA_DISPLAY_OFFSET_LIMIT;
                        }
                        if (vertical > TYA_DISPLAY_OFFSET_LIMIT) {
                            vertical = TYA_DISPLAY_OFFSET_LIMIT;
                        }

                        ((TyaSaveDataRecord *)SaveData)->horizontal_display_offset = horizontal;
                        DispEnv.display.dx = environment.display.dx + horizontal * TYA_DISPLAY_HORIZONTAL_STEP;
                        DispEnv.display.dy = environment.display.dy + vertical;
                        ((TyaSaveDataRecord *)SaveData)->vertical_display_offset = vertical;

                        if (input_event & PAD_CIRCLE) {
                            xglSoundEffectNormalDirect(SE_DECIDE);
                        } else if (input_event & PAD_CROSS) {
                            xglSoundEffectNormalDirect(SE_CANCEL);
                        } else {
                            xglFontDebugPrintf(8, 8, displayHalfwordFormat,
                                DispEnv.display.dx,
                                DispEnv.display.dy,
                                ((TyaSaveDataRecord *)SaveData)->horizontal_display_offset,
                                ((TyaSaveDataRecord *)SaveData)->vertical_display_offset);
                            DrawImage(framebuffer);
                            xglSleep();
                            continue;
                        }
                        break;
                    }
                }
            }
        }
    }
    xglSleep();
}
