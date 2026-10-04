/*
 * OV01 original TU 20: 0x00a332e0..0x00a33648 (6 functions)
 */
#include "common.h"
#include "shared.h"

/*
 * .bss for this TU is still scaffold-owned: the flag word keeps its ledger
 * name (config/symbols/ov01.txt) and the MPEG context its splat name.
 */
static int mvFlags_00A43724 = 0;
static unsigned char D_00A5B838[0xC0];

#define MV2_FLAG_INITIALIZED 0x1
#define MV2_FLAG_PLAYING 0x2
#define MV2_FLAG_SKIP_ENABLED 0x4
#define MV2_PLAYING_BIT 1

extern void xglRenderCopyDisp2Draw(void);
extern int xglMpeg2Close(unsigned char *mpegContext);

/*
 * D_00A5B7B0 (0x88 bytes, scaffold-owned, immediately precedes D_00A5B838)
 * is the movie-parameter block MMv2Play fills in: MMv2Play strncpy()s its
 * filename argument into the first 0x80 bytes (n=128, byte 0x7f forced to
 * '\0') and MMv2Exec/MMv2Play both read, increment and clear a retry counter
 * at +0x80 (lhu/lh/sh 128(base)). The 6 bytes past the counter are untouched
 * by either function.
 */
typedef struct Mv2Params {
    char filename[0x80];
    short retryCount;
    unsigned char unmodeled_82[6];
} Mv2Params;

static Mv2Params D_00A5B7B0;

extern void xglSleep(void);
extern int xglMpeg2Play(unsigned char *mpegContext);
extern int xglMpeg2Open(unsigned char *mpegContext, const char *filename);
extern void *xglMpeg2InfoInit2(void *info, void *arena, int audio_buffer_size);
extern void *memset(void *destination, int value, unsigned int count);
extern char *strncpy(char *dest, const char *src, unsigned int n);
extern void MOutputDebugStringWarn(const char *format, ...);
extern PadPrefix PadData;
extern void MMv2Stop(void);

int MMv2Init(void)
{
    mvFlags_00A43724 = MV2_FLAG_INITIALIZED | MV2_FLAG_SKIP_ENABLED;
    return 1;
}

void MMv2Exec(void)
{
    unsigned char *mpegContext = D_00A5B838;
    int retryFailed = 0;
    int playResult;

    if (!(mvFlags_00A43724 & MV2_FLAG_PLAYING)) {
        return;
    }

    if ((mvFlags_00A43724 & MV2_FLAG_SKIP_ENABLED) && (PadData.half_2a & 0x800)) {
        MMv2Stop();
        return;
    }

    playResult = xglMpeg2Play(mpegContext);

    if (playResult < 0) {
        int i;

        D_00A5B7B0.retryCount++;
        xglMpeg2Close(mpegContext);

        for (i = 9; i >= 0; i--) {
            xglSleep();
        }

        if (D_00A5B7B0.retryCount >= 2) {
            retryFailed = 1;
        } else {
            MOutputDebugStringWarn("MMv2Exec: Retrying...(%d/%d)", D_00A5B7B0.retryCount, 1);
            memset(mpegContext, 0, sizeof(D_00A5B838));
            xglMpeg2InfoInit2(mpegContext, (void *) 0x01b9e000, 0x80000);
            if (xglMpeg2Open(mpegContext, D_00A5B7B0.filename) != 0) {
                MOutputDebugStringWarn("MMv2Exec: Failed to open '%s'", D_00A5B7B0.filename);
                retryFailed = 1;
            }
        }
    }

    if (retryFailed) {
        MOutputDebugStringWarn("MMv2Exec: Failed to retry, abort");
        mvFlags_00A43724 &= ~MV2_FLAG_PLAYING;
        return;
    }

    if (playResult > 0) {
        xglRenderCopyDisp2Draw();
        xglMpeg2Close(mpegContext);
        mvFlags_00A43724 &= ~MV2_FLAG_PLAYING;
    }
}

int MMv2Play(const char *filename)
{
    unsigned char *mpegContext = D_00A5B838;
    Mv2Params *params;

    if (mvFlags_00A43724 & MV2_FLAG_PLAYING) {
        MOutputDebugStringWarn("MMv2Play: Now playing, wait for the end of movie");
        return 0;
    }

    params = (Mv2Params *) (mpegContext - sizeof(Mv2Params));
    memset(params, 0, sizeof(Mv2Params) + sizeof(D_00A5B838));
    strncpy(params->filename, filename, sizeof(params->filename));
    params->filename[sizeof(params->filename) - 1] = '\0';
    params->retryCount = 0;
    xglMpeg2InfoInit2(mpegContext, (void *) 0x01b9e000, 0x80000);

    if (xglMpeg2Open(mpegContext, params->filename) != 0) {
        MOutputDebugStringWarn("MMv2Play: Failed to open '%s'", params->filename);
        return 0;
    }

    mvFlags_00A43724 |= MV2_FLAG_PLAYING;
    return 1;
}

void MMv2Stop(void)
{
    unsigned char *mpegContext = D_00A5B838;

    if (mvFlags_00A43724 & MV2_FLAG_PLAYING) {
        xglRenderCopyDisp2Draw();
        xglMpeg2Close(mpegContext);
        mvFlags_00A43724 &= ~MV2_FLAG_PLAYING;
    }
}

int MMv2IsPlaying(void)
{
    return (mvFlags_00A43724 >> MV2_PLAYING_BIT) & 1;
}

void MMv2SkipEnabled(short enable)
{
    if (enable != 0)
        mvFlags_00A43724 |= MV2_FLAG_SKIP_ENABLED;
    else
        mvFlags_00A43724 &= ~MV2_FLAG_SKIP_ENABLED;
}
