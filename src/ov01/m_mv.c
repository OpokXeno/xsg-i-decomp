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
extern int mvFlags_00A43720;

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
} MvParams;

extern MvParams D_00A5B630;

/* Movie playback handle MMvPlay opens and MMvStop closes; still opaque. */
extern unsigned char D_00A5B6D0[];

/* Defined in src/main/xgl_1.c, still INCLUDE_ASM there. */
extern int xglMovieClose(void *movie);

int MMvInit(void)
{
    mvFlags_00A43720 = MV_FLAG_INITIALIZED | MV_FLAG_SKIP_ENABLED;
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_mv", MMvExec);

INCLUDE_ASM("asm/nonmatchings/ov01/m_mv", MMvPlay);

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
