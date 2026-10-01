/*
 * OV02 original TU 0: 0x00a00000..0x00a00850 (6 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_thread.h"

extern int xglMcMain(void);
extern void xglFontPrintDirectOT(int color, void *param);
extern int D_00A10D28;

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
typedef struct LogoTestEnvironment {
    u8 unmodeled_00[0x30];
    u64 command;
} LogoTestEnvironment;
typedef struct LogoRenderState {
    u8 unmodeled_00[0x14];
    u16 display_buffer;
} LogoRenderState;
extern LogoTestEnvironment TestEnv_1_00A0F690;
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
extern char D_00A115A8[];
extern char D_00A115B8[];

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
