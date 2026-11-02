#include "common.h"
#include "shared.h"
#include "main/xgl_packet.h"
#include "tya_display_setting.h"
#include "main/game_over.h"

#define PLAY_RESET_SOUND_EFFECT()                                       \
    do {                                                                \
        xglSoundEffectNormalDirect(1);                                  \
    } while (0)


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
    sceVif1PkRef(packet, framebuffer, 0x7000, 0, 0x51007000, 0);
    sceVif1PkRef(packet, &TransEnv_1_0036AC20, 2, 0, 0, 0);
    sceVif1PkRef(packet, framebuffer + 0x70000, 0x7000, 0, 0x51007000, 0);
    sceVif1PkRef(packet, &FlushEnv_2_0036AC40, 3, 0, 0, 0);
}

void tyaDisplaySetting(u8 *framebuffer_or_mode)
{
    short saved_x;
    short saved_y;
    GsDispEnvDisplayView environment;

    sceGsSetDefDispEnv(&environment, 0, 512, 448, 0, 0);
    saved_x = ((TyaSaveDataRecord *)SaveData)->horizontal_display_offset;
    saved_y = ((TyaSaveDataRecord *)SaveData)->vertical_display_offset;

    if (framebuffer_or_mode == (u8 *)0) {
        DispEnv.display.dx = environment.display.dx + saved_x * 5;
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

                if (directions & 0xf000) {
                    xglSoundEffectNormalDirect(3);
                    directions = PadData.mappedRepeat;
                }
                if (directions & 0x2000)
                {
                    horizontal++;
                }
                if (directions & 0x8000)
                {
                    horizontal--;
                }
                if (directions & 0x4000)
                {
                    vertical++;
                }
                if (directions & 0x1000)
                {
                    vertical--;
                }

                {
                    /* The halfword event snapshot is refreshed after the sound call. */
                    int input_event = PadData.half_2a;
                    if (input_event & 0x80) {
                        PLAY_RESET_SOUND_EFFECT();
                        input_event = PadData.half_2a;
                        horizontal = 0;
                        vertical = 0;
                    }
                    {
                        if (input_event & 0x40) {
                            horizontal = saved_x;
                            vertical = saved_y;
                        }
                        if (horizontal < -20) {
                            horizontal = -20;
                        }
                        if (vertical < -20) {
                            vertical = -20;
                        }
                        if (horizontal > 20) {
                            horizontal = 20;
                        }
                        if (vertical > 20) {
                            vertical = 20;
                        }

                        ((TyaSaveDataRecord *)SaveData)->horizontal_display_offset = horizontal;
                        DispEnv.display.dx = environment.display.dx + horizontal * 5;
                        DispEnv.display.dy = environment.display.dy + vertical;
                        ((TyaSaveDataRecord *)SaveData)->vertical_display_offset = vertical;

                        if (input_event & 0x20) {
                            xglSoundEffectNormalDirect(1);
                        } else if (input_event & 0x40) {
                            xglSoundEffectNormalDirect(2);
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
