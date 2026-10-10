#ifndef SRC_MAIN_GAME_MOVIE_H
#define SRC_MAIN_GAME_MOVIE_H

#include "shared.h"

/* The movie handle has a 0xd0-byte extent. Playback reads state at +0x40,
 * returns its frame at +0x46 and writes transparency at +0xcb. */
typedef struct MovieInfo {
    unsigned char unmodeled_00[0x40];
    short state;
    unsigned char unmodeled_42[4];
    short frame;
    unsigned char unmodeled_48[0x83];
    unsigned char transparent;
    unsigned char unmodeled_cc[4];
} MovieInfo;

typedef struct MovieGifAd {
    u64 value;
    u64 register_address;
} MovieGifAd;

/* Thirteen quadwords sent through the renderer's DIRECT path. */
typedef struct MovieOverlayPacket {
    u64 dma_tag[2];
    u64 gif_tag[2];
    MovieGifAd registers[6];
    u32 color[4];
    u32 uv0[4];
    u32 vertex0[4];
    u32 uv1[4];
    u32 vertex1[4];
} MovieOverlayPacket;

/* These are bounded prefixes of foreign data, not allocations. */
typedef struct MovieGameLoopState {
    unsigned char unmodeled_00[0x10];
    unsigned int flags;
    unsigned char unmodeled_14[0x29f40 - 0x14];
    unsigned char input_delay;
} MovieGameLoopState;

typedef struct MovieRenderState {
    unsigned char unmodeled_00[0x14];
    unsigned short flip_base;
    unsigned char unmodeled_16[0x58 - 0x16];
    unsigned char scene_disabled;
} MovieRenderState;

/* xglMpeg2Play reports its current frame through +0x38 of this 0xc0-byte
 * context, initialized by xglMpeg2InfoInit2. */
typedef struct MovieMpegContext {
    unsigned char unmodeled_00[0x38];
    int frame;
    unsigned char unmodeled_3c[0x84];
} MovieMpegContext;

void GameMovieMain(void);
void GameMovieStop(void);
int GameMoviePlay(char *name);
void GameMpeg2Play(char *name, int mode);

#endif
