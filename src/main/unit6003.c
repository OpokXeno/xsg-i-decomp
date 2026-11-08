#include "common.h"
#include "shared.h"
#include "main/toolkit.h"
#include "main/exm_2.h"
#include "main/xgl_studio.h"
#include "main/xgl_2.h"
#include "main/nml_model_set.h"
#include "main/res_get_path.h"

#define UNIT6003_FRAME_TYPE_ERROR_FORMAT "frame_type error(%d)\n"

/* JP: fm%04d を読み込みます\n
 * EN: Loading fm%04d\n */
#define UNIT6003_LOADING_FM_FORMAT \
    "fm%04d \244\362\306\311\244\337\271\376\244\337\244\336\244\271\n"

/* JP: mvs%04d を読み込みます\n
 * EN: Loading mvs%04d\n */
#define UNIT6003_LOADING_MVS_FORMAT \
    "mvs%04d \244\362\306\311\244\337\271\376\244\337\244\336\244\271\n"

#define UNIT6003_MOVIE_WORK_BROKEN_MESSAGE "movie work broken!\n"
#define UNIT6003_MOVIE_WORK_ADDRESS_FORMAT "%08x->%08x\n"

typedef struct MovieXtxHeader {
    u32 signature;
    u32 content[3];
} MovieXtxHeader;

/*
 * Partial view of XglMovieWork (src/main/xgl_1.c): the XTX header sequences
 * read here conflict with its packet and image address words (+0x46, +0x90,
 * +0x9a, +0xb0..+0xbf, +0xc8), so the full type changes this object.
 */
typedef struct XglMovieWorkXtxView {
    unsigned char unmodeled_00[0x40];
    unsigned short image_width;
    unsigned short image_height;
    short frame_count;
    u8 current_frame_index;
    unsigned char unmodeled_47[1];
    unsigned char unmodeled_48[4];
    unsigned int xtx_header_count;
    unsigned char unmodeled_50[0x40];
    void *source_data;
    int source_size;
    unsigned char unmodeled_98[2];
    unsigned char opened;
    unsigned char unmodeled_9b[0x15];
    MovieXtxHeader *first_header;
    MovieXtxHeader *second_header;
    MovieXtxHeader *headers[2];
    unsigned char unmodeled_c0[8];
    u8 header_index;
    unsigned char unmodeled_c9[7];
    MovieXtxHeader xtx_headers[0];
} XglMovieWorkXtxView;

enum {
    MOVIE_NEXT_HEADER = 0,
    MOVIE_XTX_DATA = 1,
};

typedef struct Unit6003ResourceEntry {
    void *address;
    int size;
    int handle;
    int state;
} Unit6003ResourceEntry;

typedef struct Unit6003ResourceHeader {
    u32 signature;
} Unit6003ResourceHeader;

/* unit6003.frame_type: how the current frame is stored. */
enum {
    UNIT6003_FRAME_SHEET = 0, /* XTX texture sheet cut into frames */
    UNIT6003_FRAME_MOVIE = 1, /* IPUM movie decoded into XTX headers */
    UNIT6003_FRAME_NONE_LOADED = 0xff
};

/* Sheet frames are cut from a 512x448 texture, 128x112 per frame by default. */
#define UNIT6003_SHEET_WIDTH 0x200
#define UNIT6003_SHEET_HEIGHT 0x1c0
#define UNIT6003_DEFAULT_FRAME_WIDTH 0x80
#define UNIT6003_DEFAULT_FRAME_HEIGHT 0x70
#define UNIT6003_FADE_FRAMES 0x0f
#define UNIT6003_CAMERA_COUNT 8
#define UNIT6003_RES_TYPE_MVS 10
#define UNIT6003_RES_TYPE_FM 11
#define UNIT6003_SCREEN_CLIP_MASK 0xffff0000u
#define UNIT6003_COLOR_NEUTRAL 0x80
#define UNIT6003_DRAW_MODE_MONITOR 5
#define UNIT6003_DRAW_MODE_DEFAULT 1

enum {
    UNIT6003_RESOURCE_GROUP_FM = 10000,
    UNIT6003_RESOURCE_GROUP_MVS = 20000,
    UNIT6003_RESOURCE_GROUP_SIZE = 10000,
    UNIT6003_XTX_SIGNATURE = 0x00585458,
    UNIT6003_IPUM_SIGNATURE = 0x6d757069,
};

typedef struct Unit6003DrawVertex {
    float texture_s;
    float texture_t;
    float texture_q;
    unsigned int unmodeled_0c;
    unsigned int red;
    unsigned int green;
    unsigned int blue;
    unsigned int alpha;
    int x;
    int y;
    int z;
    unsigned int flags;
} Unit6003DrawVertex;

typedef struct Unit6003DrawEnvironment {
    unsigned char unmodeled_00[0x40];
    u64 texture;
    unsigned char unmodeled_48[0x48];
    Unit6003DrawVertex vertices[4];
} Unit6003DrawEnvironment;

typedef struct Unit6003RenderState {
    unsigned char unmodeled_00[0x1a];
    unsigned short texture_page;
} Unit6003RenderState;

typedef struct Unit6003Scratch {
    float matrix[4][4];
    int screen_left;
    int screen_top;
    int screen_depth;
    unsigned int unmodeled_4c;
    int screen_width;
    int screen_height;
    int screen_coordinates;
    unsigned int alpha;
    float position[4];
    float texture_left;
    float texture_top;
    float texture_width;
    float texture_height;
    float world_left;
    float world_top;
    float depth[2];
    float world_width;
    float world_height;
} Unit6003Scratch;

extern Unit6003ResourceEntry GameResource[128];
extern Unit6003RenderState sRender;

extern void *GameResourceRealloc(void *resource, int size);
extern void xglMovieInfoInit(XglMovieWorkXtxView *info);
extern int xglMovieOpen(XglMovieWorkXtxView *info, int mode);
extern MovieXtxHeader *xglMovieMakeXtxHeader(XglMovieWorkXtxView *info, MovieXtxHeader *header);
extern int GameResourceGetIndex(int address);

extern int RES_loadFile(int command, int callback, int resource_id,
                        int flags);

extern int printf(const char *format, ...);
extern void MAP_updateUnitDefault(MapUnitRecord *unit);
extern int xglMoviePlay(void *info);
extern float xglRotTransPers(int *result, float matrix[4][4],
                             float *position, int camera);

static Unit6003DrawEnvironment TestEnv_0_00490520 = {
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x51,
        0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10,
        0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    },
    0,
    {
        0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x47, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x80, 0x00, 0x00, 0x00, 0x40, 0x2e, 0xc0,
        0x12, 0x25, 0x51, 0x12, 0x25, 0x51, 0x00, 0x00,
    },
    {{0}},
};

static unsigned char clut_1[32] = {
    0x00, 0x04, 0x10, 0x14, 0x20, 0x24, 0x30, 0x34,
    0x40, 0x44, 0x50, 0x54, 0x60, 0x64, 0x70, 0x74,
    0x08, 0x0c, 0x18, 0x1c, 0x28, 0x2c, 0x38, 0x3c,
    0x48, 0x4c, 0x58, 0x5c, 0x68, 0x6c, 0x78, 0x7c,
};

/*
 * Draws one unit6003 frame as a quad with a neutral colour. The texture is
 * a sheet cell or the current movie frame. The corners are placed in world
 * or screen space, and the draw is skipped when a world corner is clipped.
 */
static void unit6003_draw(MapUnitRecord *unit)
{
    Unit6003Scratch *scratch = (Unit6003Scratch *)SCRATCHPAD_BASE;
    Unit6003DrawVertex *vertex;
    int draw_mode = unit->monitor_priority == 0 ? UNIT6003_DRAW_MODE_MONITOR : UNIT6003_DRAW_MODE_DEFAULT;
    int camera_index;
    int corner;
    int texture_page;
    float texture_scale;

    /* Send the sheet or the current movie frame's XTX data. */
    switch (unit->unit6003.frame_type) {
    case UNIT6003_FRAME_SHEET:
        nmlModelDirectSendXtx(draw_mode, unit->unit6003.movie_work.resource);
        break;
    case UNIT6003_FRAME_MOVIE: {
        XglMovieWorkXtxView *movie = unit->unit6003.movie_work.movie;
        nmlModelDirectSendXtx(draw_mode,
                              movie->headers[movie->header_index]);
        break;
    }
    default:
        printf(UNIT6003_FRAME_TYPE_ERROR_FORMAT, unit->unit6003.frame_type);
        break;
    }

    /* Texture register word of the draw environment for this frame type. */
    switch (unit->unit6003.frame_type) {
    case UNIT6003_FRAME_SHEET:
        texture_page = sRender.texture_page << 5;
        TestEnv_0_00490520.texture =
            (0x25320000 | texture_page) |
            ((u64)(texture_page + clut_1[unit->unit6003.frame_index] + 0x380) << 37) |
            0x2000000640000000ULL;
        break;
    case UNIT6003_FRAME_MOVIE:
        TestEnv_0_00490520.texture =
            (0x20010000 | (sRender.texture_page << 5)) |
            0x2000000600000000ULL;
        break;
    }
    vertex = TestEnv_0_00490520.vertices;
    for (camera_index = 0; camera_index < UNIT6003_CAMERA_COUNT; ++camera_index) {
        if (xglStudioGetCamera2(camera_index)->active != 0)
            break;
    }

    /* Texture rectangle of the frame inside its texture, as fractions. */
    switch (unit->unit6003.frame_type) {
    case UNIT6003_FRAME_SHEET:
        texture_scale = 1.0f / 512.0f;
        scratch->texture_left = I2F(
            (unit->unit6003.frame_index % unit->unit6003.frames_per_row) *
            unit->unit6003.texture_width) / 512.0f;
        scratch->texture_top = I2F(
            (unit->unit6003.frame_index / unit->unit6003.frames_per_row) *
            unit->unit6003.texture_height) / 512.0f;
        scratch->texture_width = I2F(unit->unit6003.texture_width) * texture_scale;
        scratch->texture_height = I2F(unit->unit6003.texture_height) * texture_scale;
        break;
    case UNIT6003_FRAME_MOVIE:
        scratch->texture_left = 0.0f;
        scratch->texture_top = 0.0f;
        texture_scale = 1.0f / 256.0f;
        scratch->texture_width = I2F(unit->unit6003.texture_width) * texture_scale;
        scratch->texture_height = I2F(unit->unit6003.texture_height) * texture_scale;
        break;
    }

    /* Placement: world-space rectangle, or screen rectangle when use_screen_coordinates is set. */
    scratch->screen_coordinates = unit->unit6003.use_screen_coordinates;
    scratch->alpha = unit->unit6003.alpha;
    if (scratch->screen_coordinates == 0) {
        scratch->world_left = unit->unit6003.left.world - unit->unit6003.width.world * 0.5f;
        scratch->world_top = unit->unit6003.top.world + unit->unit6003.height.world * 0.5f;
        scratch->depth[0] = unit->unit6003.top_depth;
        scratch->depth[1] = unit->unit6003.bottom_depth;
        scratch->world_width = unit->unit6003.width.world;
        scratch->world_height = unit->unit6003.height.world;
    } else {
        scratch->screen_left = unit->unit6003.left.screen * 16 + 0x6ff8;
        {
            int top = unit->unit6003.top.screen;
            scratch->screen_depth = -1;
            scratch->screen_top = top * 16 + 0x71f8;
        }
        scratch->screen_width = unit->unit6003.width.screen * 16;
        scratch->screen_height = unit->unit6003.height.screen * 16;
    }
    xglMatrixStackSave(scratch->matrix);

    /* Transform the four corners of the quad. */
    for (corner = 0; corner < 4; ++corner, ++vertex) {
        int right = corner & 1;
        int bottom;
        float perspective;

        if (scratch->screen_coordinates == 0) {
            scratch->position[0] = scratch->world_left +
                (right != 0 ? scratch->world_width : 0.0f);
            bottom = corner & 2;
            scratch->position[1] = scratch->world_top -
                (bottom != 0 ? scratch->world_height : 0.0f);
            scratch->position[2] = scratch->depth[bottom >> 1];
            scratch->position[3] = 1.0f;
            perspective = xglRotTransPers(&vertex->x, scratch->matrix,
                                           scratch->position, camera_index);
            vertex->flags = 0;
            if ((vertex->x & UNIT6003_SCREEN_CLIP_MASK) != 0 ||
                (vertex->y & UNIT6003_SCREEN_CLIP_MASK) != 0 || vertex->z < 0)
                return;
        } else {
            vertex->x = scratch->screen_left +
                (right != 0 ? scratch->screen_width : 0);
            vertex->y = scratch->screen_top +
                ((corner & 2) != 0 ? scratch->screen_height : 0);
            perspective = 1.0f;
            vertex->z = scratch->screen_depth;
            vertex->flags = 0;
        }
        vertex->texture_s = (scratch->texture_left +
            (right != 0 ? scratch->texture_width : 0.0f)) * perspective;
        vertex->texture_t = (scratch->texture_top +
            ((corner & 2) != 0 ? scratch->texture_height : 0.0f)) * perspective;
        vertex->red = UNIT6003_COLOR_NEUTRAL;
        vertex->green = UNIT6003_COLOR_NEUTRAL;
        vertex->blue = UNIT6003_COLOR_NEUTRAL;
        vertex->texture_q = perspective;
        vertex->alpha = scratch->alpha;
    }
    nmlModelDirectSend(draw_mode, &TestEnv_0_00490520, 0x15);
}

/* Allocates a movie work over source_data, opens it and builds its two XTX header arrays. */
static XglMovieWorkXtxView *movie_work_setup(void *source_data, int source_size)
{
    XglMovieWorkXtxView *info;
    MovieXtxHeader *header_cursor;
    int allocation_size;

    info = GameResourceAlloc(0xd0);
    xglMovieInfoInit(info);
    info->opened = 1;
    info->source_data = source_data;
    info->source_size = source_size;
    xglMovieOpen(info, 0);
    info->headers[MOVIE_XTX_DATA] = info->xtx_headers;
    header_cursor = xglMovieMakeXtxHeader(info, info->headers[MOVIE_XTX_DATA]);
    info->first_header = header_cursor;
    header_cursor += info->xtx_header_count;
    info->headers[MOVIE_NEXT_HEADER] = header_cursor;
    header_cursor = xglMovieMakeXtxHeader(info, header_cursor);
    info->second_header = header_cursor;
    header_cursor += info->xtx_header_count;
    allocation_size = sizeof(XglMovieWorkXtxView) +
        (header_cursor - info->xtx_headers) * sizeof(MovieXtxHeader);
    GameResourceRealloc(info, allocation_size);
    return info;
}

/*
 * Runs the unit6003 state machine for one frame. The first step loads the
 * resource and sets its frame type (sheet or movie). Then the unit fades in
 * and out with its signal, and the frame index advances after frame_delay ticks.
 */
void unit6003_update(MapUnitRecord *unit)
{
    switch (unit->actionSub) {
    case 0: {
        u32 resource_index = unit->unit6003.resource_id;
        Unit6003MovieSource source;

        if (resource_index - UNIT6003_RESOURCE_GROUP_FM <
            UNIT6003_RESOURCE_GROUP_SIZE) {
            unsigned int resource_file_id =
                resource_index % UNIT6003_RESOURCE_GROUP_SIZE;
            printf(UNIT6003_LOADING_FM_FORMAT, resource_file_id);
            resource_index = GameResourceGetIndex(
                RES_loadFile(-1, UNIT6003_RES_TYPE_FM, resource_file_id, 0));
            unit->unit6003.resource_id = resource_index;
        } else if (resource_index - UNIT6003_RESOURCE_GROUP_MVS <
                   UNIT6003_RESOURCE_GROUP_SIZE) {
            unsigned int resource_file_id =
                resource_index % UNIT6003_RESOURCE_GROUP_SIZE;
            printf(UNIT6003_LOADING_MVS_FORMAT, resource_file_id);
            resource_index = GameResourceGetIndex(
                RES_loadFile(-1, UNIT6003_RES_TYPE_MVS, resource_file_id, 0));
            unit->unit6003.resource_id = resource_index;
        }

        unit->actionSub = 1;
        unit->flags |= 4;
        unit->draw = 0;
        unit->unit6003.frame_index = 0;
        unit->unit6003.frame_timer = 0;
        if (unit->unit6003.playback_direction < 0)
            unit->unit6003.frame_delay = -unit->unit6003.playback_direction;
        else
            unit->unit6003.frame_delay = unit->unit6003.playback_direction;

        source.resource = GameResource[unit->unit6003.resource_id].address;
        unit->unit6003.frame_type = UNIT6003_FRAME_NONE_LOADED;
        if (source.resource != 0) {
            if (source.resource->signature == UNIT6003_XTX_SIGNATURE) {
                if (unit->unit6003.texture_width == 0)
                    unit->unit6003.texture_width = UNIT6003_DEFAULT_FRAME_WIDTH;
                if (unit->unit6003.texture_height == 0)
                    unit->unit6003.texture_height = UNIT6003_DEFAULT_FRAME_HEIGHT;
                unit->unit6003.frames_per_row = UNIT6003_SHEET_WIDTH / unit->unit6003.texture_width;
                unit->unit6003.frames_per_column = UNIT6003_SHEET_HEIGHT / unit->unit6003.texture_height;
                unit->unit6003.frame_type = UNIT6003_FRAME_SHEET;
            } else if (source.resource->signature == UNIT6003_IPUM_SIGNATURE) {
                u32 played;
                XglMovieWorkXtxView *movie_work;

                movie_work = movie_work_setup(
                    source.resource, GameResource[unit->unit6003.resource_id].size);
                played = 0;
                unit->unit6003.texture_width = movie_work->image_width;
                unit->unit6003.texture_height = movie_work->image_height;
                do {
                    played += 1;
                    xglMoviePlay(movie_work);
                } while (played <= unit->unit6003.movie_advance_count);
                __builtin_memset(&unit->unit6003.movie_advance_count, 0,
                                 sizeof(unit->unit6003.movie_advance_count));
                {
                    const short *movie_frame_count_source = &movie_work->frame_count;
                    short movie_frame_count = *movie_frame_count_source;

                    unit->unit6003.frame_type = UNIT6003_FRAME_MOVIE;
                    unit->unit6003.frame_count = movie_frame_count;
                }
                source.movie = movie_work;
            }
        }
        unit->unit6003.movie_work = source;
    }
        /* fall through */
    /* Idle until the signal arrives, then fade in while drawing. */
    case 1:
        if (unit->signal != 0) {
            unit->actionSub = 2;
            unit->sequenceNo = UNIT6003_FADE_FRAMES;
            unit->draw = unit6003_draw;
    case 2:
            if (unit->sequenceNo == 0) {
                unit->actionSub = 3;
    case 3:
                if (unit->signal == 0) {
                    unit->actionSub = 4;
                    unit->sequenceNo = UNIT6003_FADE_FRAMES;
                }
            } else {
                unit->sequenceNo =
                    unit->sequenceNo - 1;
            }
        }
        break;
    /* Fade out, then stop drawing and return to the idle state. */
    case 4:
        if (unit->sequenceNo != 0) {
            unit->sequenceNo = unit->sequenceNo - 1;
            break;
        }
        unit->draw = 0;
        unit->actionSub = 1;
        break;
    }

    /* Rebuild the movie work when its source or XTX headers no longer match. */
    if (unit->unit6003.frame_type == UNIT6003_FRAME_MOVIE) {
        XglMovieWorkXtxView *movie_work = unit->unit6003.movie_work.movie;

        if (GameResourceGetIndex((int)movie_work) == -1 ||
            movie_work->source_data !=
                GameResource[unit->unit6003.resource_id].address ||
            movie_work->headers[MOVIE_NEXT_HEADER]->signature !=
                UNIT6003_XTX_SIGNATURE ||
            movie_work->headers[MOVIE_XTX_DATA]->signature !=
                UNIT6003_XTX_SIGNATURE) {
            XglMovieWorkXtxView *repaired_work;

            printf(UNIT6003_MOVIE_WORK_BROKEN_MESSAGE);
            repaired_work = movie_work_setup(
                GameResource[unit->unit6003.resource_id].address,
                GameResource[unit->unit6003.resource_id].size);
            printf(UNIT6003_MOVIE_WORK_ADDRESS_FORMAT,
                   (u32)unit->unit6003.movie_work.movie, (u32)repaired_work);
            unit->unit6003.movie_work.movie = repaired_work;
        }
    }

    if (unit->signal != 0) {
        unsigned int frame_timer = unit->unit6003.frame_timer + 1;

        unit->unit6003.frame_timer = frame_timer;
        if ((u8)frame_timer >= unit->unit6003.frame_delay) {
            unit->unit6003.frame_timer = 0;
            switch (unit->unit6003.frame_type) {
            case UNIT6003_FRAME_SHEET: {
                unit->unit6003.frame_index++;
                if (unit->unit6003.frame_index > unit->unit6003.frame_count) {
                    if (unit->unit6003.playback_direction < 0)
                        unit->unit6003.frame_index = 0;
                    else
                        unit->unit6003.frame_index = unit->unit6003.frame_count;
                }
                break;
            }
            case UNIT6003_FRAME_MOVIE: {
                XglMovieWorkXtxView *movie_work = unit->unit6003.movie_work.movie;

                if (unit->unit6003.movie_advance_count == 0) {
                    if (xglMoviePlay(movie_work) != 0) {
                        if (unit->unit6003.playback_direction >= 0)
                            unit->unit6003.movie_advance_count = 1;
                    }
                    unit->unit6003.frame_index = movie_work->current_frame_index;
                }
                break;
            }
            }
        }
    }

    MAP_updateUnitDefault(unit);
}
