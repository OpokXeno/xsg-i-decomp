#include "common.h"
#include "shared.h"

typedef struct MovieXtxHeader {
    u32 signature;
    u32 content[3];
} MovieXtxHeader;

/* The movie playback resource owns two contiguous XTX header sequences. */
typedef struct MoviePlaybackWork {
    unsigned char unmodeled_00[0x4c];
    unsigned int xtx_header_count;
    unsigned char unmodeled_50[0x40];
    void *source_data;
    int source_size;
    unsigned char unmodeled_98[2];
    unsigned char opened;
    unsigned char unmodeled_9b[0x15];
    MovieXtxHeader *first_header;
    MovieXtxHeader *second_header;
    MovieXtxHeader *next_header;
    MovieXtxHeader *xtx_data;
    unsigned char unmodeled_c0[0x10];
    MovieXtxHeader xtx_headers[0];
} MoviePlaybackWork;

extern void *GameResourceAlloc(int size);
extern void *GameResourceRealloc(void *resource, int size);
extern void xglMovieInfoInit(MoviePlaybackWork *info);
extern int xglMovieOpen(MoviePlaybackWork *info, int mode);
extern MovieXtxHeader *xglMovieMakeXtxHeader(MoviePlaybackWork *info, MovieXtxHeader *header);

INCLUDE_ASM("asm/main/nonmatchings/unit6003", unit6003_draw);

static MoviePlaybackWork *movie_work_setup(void *source_data, int source_size)
{
    MoviePlaybackWork *info;
    MovieXtxHeader *header_cursor;
    int allocation_size;

    info = GameResourceAlloc(0xd0);
    xglMovieInfoInit(info);
    info->opened = 1;
    info->source_data = source_data;
    info->source_size = source_size;
    xglMovieOpen(info, 0);
    info->xtx_data = info->xtx_headers;
    header_cursor = xglMovieMakeXtxHeader(info, info->xtx_data);
    info->first_header = header_cursor;
    header_cursor += info->xtx_header_count;
    info->next_header = header_cursor;
    header_cursor = xglMovieMakeXtxHeader(info, header_cursor);
    info->second_header = header_cursor;
    header_cursor += info->xtx_header_count;
    allocation_size = (int)((unsigned char *)header_cursor - (unsigned char *)info);
    GameResourceRealloc(info, allocation_size);
    return info;
}

INCLUDE_ASM("asm/main/nonmatchings/unit6003", unit6003_update);
