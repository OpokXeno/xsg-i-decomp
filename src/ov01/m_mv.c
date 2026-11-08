/*
 * OV01 original TU 19: 0x00a32e78..0x00a332e0 (8 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_dma.h"
#include "main/xgl_render.h"

static int mvFlags_00A43720 = 0;

#define MV_FLAG_INITIALIZED 0x1
#define MV_FLAG_PLAYING 0x2
#define MV_FLAG_SKIP_ENABLED 0x4
#define MV_PLAYING_BIT 1

/*
 * MvPlayOptions.flags: MV_PLAY_FRAME_MOVIE_SIZE sizes the frame to the movie,
 * MV_PLAY_FRAME_FULL uses the full 512x448 frame at the origin.
 */
#define MV_PLAY_FRAME_MOVIE_SIZE 0x1
#define MV_PLAY_FRAME_FULL 0x2
#define MV_FULL_FRAME_WIDTH 0x200
#define MV_FULL_FRAME_HEIGHT 0x1C0
/* Arena size handed to xglMovieOpen and the largest decode buffer MMvPlay2 accepts. */
#define MV_ARENA_SIZE 0x40000
#define MV_DECODE_BUFFER_LIMIT 0xC00

/*
 * Partial view of the movie-parameter block MMvPlay2 sets up; only the two
 * fields MMvGetTime reads are modeled.
 */
typedef struct MvParams {
    unsigned char unmodeled_00[0xe4];
    short totalTime;   /* +0xe4 */
    short currentTime; /* +0xe6 */
    unsigned char unmodeled_e8[0x180 - 0xe8];
} MvParams;

static MvParams mvParams;

/* The movie handle inside mvParams, at offset 0xa0. */
#define mvMovieContext ((unsigned char *)&mvParams + 0xa0)

typedef struct MvGifEnvironment {
    unsigned char unmodeled_00[0x30];
    u64 transfer_tag;                  /* +0x30 */
    unsigned char unmodeled_38[0x28];
    int x0 : 32;                       /* +0x60 */
    int y0 : 32;                       /* +0x64 */
    unsigned char unmodeled_68[8];
    int width : 32;                    /* +0x70 */
    int height : 32;                   /* +0x74 */
    unsigned char unmodeled_78[8];
    int x1 : 32;                       /* +0x80 */
    int y1 : 32;                       /* +0x84 */
    unsigned char unmodeled_88[8];
} MvGifEnvironment;



typedef struct MvRenderControl {
    unsigned char unmodeled_00[0x14];
    u16 movie_gif_control;
} MvRenderControl;

#define MV_RENDER_STATE ((MvRenderControl *) 0x004a90e0)

static MvGifEnvironment mvEnv;

#define MV_PAD_SKIP_HELD 0x100
#define MV_PAD_SKIP_PRESSED 0x40




void MMvStop(void);

/* xglMovieClose and xglMoviePlay are defined in src/main/xgl_1.c. */
extern int xglMovieClose(void *movie);
extern int xglMoviePlay(void *movie);
extern void FlushCache(int mode);
extern void xglDmaDirectNormal(u32 channel, u32 address, u32 count);

typedef struct MvPlayOptions {
    int flags;
    const char *filename;
    int frame_x;
    int frame_y;
    int frame_width;
    int frame_height;
} MvPlayOptions;

int MMvPlay2(MvPlayOptions *options);

int MMvInit(void)
{
    mvFlags_00A43720 = MV_FLAG_INITIALIZED | MV_FLAG_SKIP_ENABLED;
    return 1;
}

/*
 * Advances the movie by one frame while playing. A skip press stops it; otherwise
 * the frame is decoded, the GIF environment is sent, and the movie is closed at its end.
 */
void MMvExec(void)
{
    int finished;

    if (mvFlags_00A43720 & MV_FLAG_PLAYING)
    {
        if ((mvFlags_00A43720 & MV_FLAG_SKIP_ENABLED) &&
            (PadData.half_28 & MV_PAD_SKIP_HELD) &&
            (PadData.half_2a & MV_PAD_SKIP_PRESSED))
        {
            MMvStop();
            return;
        }

        finished = xglMoviePlay(mvMovieContext);
        /* The flip base occupies bits 5 and above of the low command word. */
        mvEnv.transfer_tag = 0x2000000640000000ULL | (u64)(0x24020000 | (sRender.flip_base << 5));
        FlushCache(0);
        xglDmaDirectNormal(2, (u32)&mvEnv, 9);

        if (finished)
        {
            xglMovieClose(mvMovieContext);
            mvFlags_00A43720 &= ~MV_FLAG_PLAYING;
        }
    }
}

/* Plays a movie in the full 512x448 frame at the origin. */
int MMvPlay(const char *filename)
{
    MvPlayOptions options;

    options.flags = MV_PLAY_FRAME_FULL;
    options.filename = filename;
    return MMvPlay2(&options);
}

/*
 * Movie handle as xglMovieOpen and the decoder fill it; only the fields
 * MMvPlay2 reads or writes are modeled.
 */
typedef struct MvMovieHandle {
    unsigned char unmodeled_00[0x40];
    short width;                       /* +0x40 */
    short height;                      /* +0x42 */
    unsigned char unmodeled_44[4];
    int bufferSize;                    /* +0x48 */
    unsigned char unmodeled_4c[0x44];
    void *arena;                       /* +0x90 */
    int arenaSize;                     /* +0x94 */
    unsigned char unmodeled_98[0x18];
    void *buf0;                        /* +0xb0 */
    void *buf1;                        /* +0xb4 */
    unsigned char unmodeled_b8[0x11];
    unsigned char flag_c9;             /* +0xc9 */
    unsigned char unmodeled_ca[0xd0 - 0xca];
} MvMovieHandle;

/* The 0x180-byte block at mvParams as MMvPlay2 lays it out. */
typedef struct MvPlayState {
    MvPlayOptions options;             /* +0x00 */
    char filename[0x80];               /* +0x18 */
    unsigned char unmodeled_98[8];
    MvMovieHandle movie;               /* +0xa0 */
    void *arena;                       /* +0x170 */
    void *buf0;                        /* +0x174 */
    void *buf1;                        /* +0x178 */
    unsigned char unmodeled_17c[4];
} MvPlayState;

extern const char mvMMvPlayNowPlayingWaitForTheText[];
extern const char mvMMvPlayFailedToOpenText[];
extern const char mvMMvPlayCouldNotPlayTheMovieText[];
extern const char mvMMvPlayTheSizeOfTheDecodeText[];
extern void MOutputDebugStringWarn(const char *format, ...);
extern void *memset(void *destination, int value, unsigned int size);
extern char *strncpy(char *dest, const char *src, unsigned int n);
extern void xglMovieInfoInit(void *info);
extern int xglMovieOpen(void *info, char *name);

/*
 * Starts a movie: copies the options and filename into the play-state block, opens
 * the movie, checks its decode buffer size and sets the frame rectangle.
 * Returns 1 when playback starts, 0 otherwise.
 */
int MMvPlay2(MvPlayOptions *options)
{
    MvPlayState *st;
    MvGifEnvironment *env;
    MvMovieHandle *mv;
    void *buf;
    void *arena = (void *)0x01D9E000;
    int h;
    int w;
    int x;
    int y;

    if (mvFlags_00A43720 & MV_FLAG_PLAYING) {
        MOutputDebugStringWarn(mvMMvPlayNowPlayingWaitForTheText);
        return 0;
    }
    /* Set up the play state, then open the movie into it. */
    st = (MvPlayState *)&mvParams;
    memset(st, 0, sizeof(MvPlayState));
    st->arena = arena;
    st->options = *options;
    buf = (void *)0x01E1E000;
    st->buf0 = buf;
    buf = (void *)0x01E2A000;
    st->buf1 = buf;
    strncpy(st->filename, options->filename, sizeof(st->filename));
    st->filename[sizeof(st->filename) - 1] = '\0';
    mv = &st->movie;
    xglMovieInfoInit(mv);
    mv->arena = st->arena;
    mv->arenaSize = MV_ARENA_SIZE;
    if (xglMovieOpen(mv, st->filename) != 0) {
        MOutputDebugStringWarn(mvMMvPlayFailedToOpenText, st->filename);
        return 0;
    }
    if (mv->bufferSize > MV_DECODE_BUFFER_LIMIT) {
        MOutputDebugStringWarn(mvMMvPlayCouldNotPlayTheMovieText);
        MOutputDebugStringWarn(mvMMvPlayTheSizeOfTheDecodeText);
        xglMovieClose(mv);
        return 0;
    }
    /* Size the GIF environment from the movie, then place the frame rectangle. */
    env = &mvEnv;
    h = mv->height;
    w = mv->width;
    mv->buf0 = st->buf0;
    mv->buf1 = st->buf1;
    env->width = w << 4;
    env->height = h << 4;
    mv->flag_c9 = 0;
    if (st->options.flags & MV_PLAY_FRAME_MOVIE_SIZE) {
        st->options.frame_width = w;
        st->options.frame_height = h;
    }
    if (st->options.flags & MV_PLAY_FRAME_FULL) {
        st->options.frame_width = MV_FULL_FRAME_WIDTH;
        st->options.frame_height = MV_FULL_FRAME_HEIGHT;
        st->options.frame_x = 0;
        st->options.frame_y = 0;
    }
    x = st->options.frame_x;
    y = st->options.frame_y;
    env->x0 = x * 16 + 0x6FF8;
    env->y0 = y * 16 + 0x71F7;
    env->x1 = (x + st->options.frame_width) * 16 + 0x6FF8;
    env->y1 = (y + st->options.frame_height) * 16 + 0x71F7;
    mvFlags_00A43720 |= MV_FLAG_PLAYING;
    return 1;
}

void MMvStop(void)
{
    if (mvFlags_00A43720 & MV_FLAG_PLAYING)
    {
        xglMovieClose(mvMovieContext);
        mvFlags_00A43720 &= ~MV_FLAG_PLAYING;
    }
}

int MMvIsPlaying(void)
{
    return (mvFlags_00A43720 >> MV_PLAYING_BIT) & 1;
}

/*
 * Returns the remaining movie time and stores the current and total times in the
 * non-null outputs. All three are 0 when no movie is playing.
 */
int MMvGetTime(int *currentTime, int *totalTime)
{
    short movieCurrentTime = 0;
    short movieTotalTime = 0;

    if (mvFlags_00A43720 & MV_FLAG_PLAYING)
    {
        movieCurrentTime = mvParams.currentTime;
        movieTotalTime = mvParams.totalTime;
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



static MvGifEnvironment mvEnv = {
    { 0x1, 0x80, 0x0, 0x0, 0x0, 0x40, 0x8b, 0x80, 0xee, 0x1e, 0x53, 0x53, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3f, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf0, 0xff, 0x7f, 0x0, 0x0, 0x0, 0x0, 0x0, 0x8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 },
    0x0ULL,
    { 0x6, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x80, 0x0, 0x0, 0x0, 0x80, 0x0, 0x0, 0x0, 0x80, 0x0, 0x0, 0x0, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 },
    28664, 29175,
    { 0x0, 0x0, 0x0, 0x80, 0x0, 0x0, 0x0, 0x0 },
    8192, 7168,
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 },
    36856, 36343,
    { 0x0, 0x0, 0x0, 0x80, 0x0, 0x0, 0x0, 0x0 }
};

const char mvMMvPlayNowPlayingWaitForTheText[48] = "MMvPlay: Now playing, wait for the end of movie";

const char mvMMvPlayFailedToOpenText[32] = "MMvPlay: Failed to open '%s'";

const char mvMMvPlayCouldNotPlayTheMovieText[48] = "MMvPlay: Could not play the movie, because";

const char mvMMvPlayTheSizeOfTheDecodeText[64] = "MMvPlay: the size of the decode buffer is not sufficient";
