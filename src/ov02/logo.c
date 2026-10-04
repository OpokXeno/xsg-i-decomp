/*
 * OV02 original TU 0: 0x00a00000..0x00a00850 (6 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_thread.h"

extern int xglMcMain(void);
extern void xglFontPrintDirectOT(int color, void *param);
const char D_00A10D28[8] = "\x19\x03";

static int device_check_sync(void)
{
    int status;

    status = xglMcMain();
    xglSleep();
    xglFontPrintDirectOT(-1, &D_00A10D28);
    return status;
}

INCLUDE_ASM("asm/nonmatchings/ov02/logo", hdd_check);

INCLUDE_ASM("asm/nonmatchings/ov02/logo", mc_check);

INCLUDE_ASM("asm/nonmatchings/ov02/logo", LogoFirst);

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
typedef struct LogoRenderState {
    u8 unmodeled_00[0x14];
    u16 display_buffer;
} LogoRenderState;
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
extern LogoRenderState sRender;
extern void xglSoundEffectNormalDirect(int effect_id);
extern PadPrefix PadData;
extern void xglMovieInfoInit(LogoMovieInfo *movie);
extern int xglMovieOpen(LogoMovieInfo *movie, const char *path);
extern int xglMoviePlay(LogoMovieInfo *movie);
extern int xglMovieClose(LogoMovieInfo *movie);
extern float xglFRand(void);
extern void FlushCache(int mode);
extern void xglDmaDirectNormal(u32 channel, u32 address, u32 count);

static int ipuplay(char *path)
{
    LogoMovieInfo movie;
    int skipped;
    u32 frame_buffer_base;

    skipped = 0;
    xglMovieInfoInit(&movie);
    frame_buffer_base = 0x01000000;
    movie.stream_base = frame_buffer_base;
    movie.stream_size = 0x80000;
    movie.stopped = 0;
    frame_buffer_base = 0x01100000;
    xglMovieOpen(&movie, path);
    movie.display_base = frame_buffer_base;
    frame_buffer_base += movie.frame_width * 16;
    movie.display_end = frame_buffer_base;

    for (;;)
    {
        xglSRand();
        xglFRand();

        if (PadData.half_2a & 0x800)
        {
            skipped = 1;
            xglSoundEffectNormalDirect(1);
            break;
        }

        if (xglMoviePlay(&movie) != 0)
        {
            break;
        }

        /* The display-buffer value occupies bits 5 and above of the low command word. */
        TestEnv_1_00A0F690.command = 0x2000000640000000ULL |
                                     (u64)~(~0x24020000U & ~((u32)sRender.display_buffer << 5));
        FlushCache(0);
        xglDmaDirectNormal(2, (u32)&TestEnv_1_00A0F690, 9);
        xglSleep();
    }

    xglMovieClose(&movie);
    xglSleep();
    return skipped;
}

static int ipuplay(char *path);
extern void xglRenderClearColor(u32 color);
extern void xglRenderClearDepth(void);
extern void xglRenderClearFrame(void);
const char D_00A115A8[] = "data\\namco.ipu";
const char D_00A115B8[] = "data\\logo_msi.ipu";

void Logo(void)
{
    xglRenderClearFrame();
    xglRenderClearColor(0x80000000);
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
