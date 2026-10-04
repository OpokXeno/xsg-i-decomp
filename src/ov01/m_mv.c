/*
 * OV01 original TU 19: 0x00a32e78..0x00a332e0 (8 functions)
 */
#include "common.h"
#include "shared.h"

/*
 * .bss for this TU is still scaffold-owned (config/tu-build.json
 * data_ownership); these keep the scaffold's D_00XXXXXX names. The original
 * ELF's own local symbol table names the flag word mvFlags (config/symbols/
 * ov01.txt disambiguates the two same-named words at 0x00a43720/0x00a43724 as
 * mvFlags_00A43720/mvFlags_00A43724); the movie-parameter block and the movie
 * handle at 0x00a5b630/0x00a5b6d0 have no ledger entry and keep the splat
 * default.
 */
static int mvFlags_00A43720 = 0;

#define MV_FLAG_INITIALIZED 0x1
#define MV_FLAG_PLAYING 0x2
#define MV_FLAG_SKIP_ENABLED 0x4
#define MV_PLAYING_BIT 1

/*
 * Partial view of the movie-parameter block MMvPlay2 sets up (still
 * INCLUDE_ASM in this TU); only the two fields MMvGetTime reads are modeled.
 */
typedef struct MvParams {
    unsigned char unmodeled_00[0xe4];
    short totalTime;   /* +0xe4 */
    short currentTime; /* +0xe6 */
    unsigned char unmodeled_e8[0x180 - 0xe8];
} MvParams;

static MvParams D_00A5B630;

/* Interior label in D_00A5B630 at the movie-info structure. */
#define D_00A5B6D0 ((unsigned char *)&D_00A5B630 + 0xa0)

typedef struct MvGifEnvironment {
    unsigned char unmodeled_00[0x30];
    u64 transfer_tag;
} MvGifEnvironment;

extern MvGifEnvironment mvEnv;

typedef struct MvRenderControl {
    unsigned char unmodeled_00[0x14];
    u16 movie_gif_control;
} MvRenderControl;

#define MV_RENDER_STATE ((MvRenderControl *) 0x004a90e0)
extern PadPrefix PadData;
extern void FlushCache(int mode);
extern void xglDmaDirectNormal(u32 channel, u32 address, u32 count);
extern int xglMoviePlay(void *movie);
void MMvStop(void);

/* Defined in src/main/xgl_1.c, still INCLUDE_ASM there. */
extern int xglMovieClose(void *movie);

typedef struct MvPlayOptions {
    int flags;
    const char *filename;
    int frame_x;
    int frame_y;
    int frame_width;
    int frame_height;
} MvPlayOptions;

int MMvPlay2(MvPlayOptions options);

int MMvInit(void)
{
    mvFlags_00A43720 = MV_FLAG_INITIALIZED | MV_FLAG_SKIP_ENABLED;
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_mv", MMvExec);

int MMvPlay(const char *filename)
{
    MvPlayOptions options;

    options.flags = 2;
    options.filename = filename;
    return MMvPlay2(options);
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_mv", MMvPlay2);

void MMvStop(void)
{
    if (mvFlags_00A43720 & MV_FLAG_PLAYING)
    {
        xglMovieClose(D_00A5B6D0);
        mvFlags_00A43720 &= ~MV_FLAG_PLAYING;
    }
}

int MMvIsPlaying(void)
{
    return (mvFlags_00A43720 >> MV_PLAYING_BIT) & 1;
}

int MMvGetTime(int *currentTime, int *totalTime)
{
    short movieCurrentTime = 0;
    short movieTotalTime = 0;

    if (mvFlags_00A43720 & MV_FLAG_PLAYING)
    {
        movieCurrentTime = D_00A5B630.currentTime;
        movieTotalTime = D_00A5B630.totalTime;
    }

    if (currentTime != 0)
    {
        *currentTime = movieCurrentTime;
    }
    if (totalTime != 0)
    {
        *totalTime = movieTotalTime;
    }

    return movieTotalTime - movieCurrentTime;
}

void MMvSkipEnabled(short enable)
{
    if (enable != 0)
    {
        mvFlags_00A43720 |= MV_FLAG_SKIP_ENABLED;
    }
    else
    {
        mvFlags_00A43720 &= ~MV_FLAG_SKIP_ENABLED;
    }
}
