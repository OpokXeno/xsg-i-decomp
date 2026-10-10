#include "common.h"

#include "shared.h"

typedef struct MovieXtxHeader {
    u32 signature;
    u32 content[3];
} MovieXtxHeader;

/* The movie playback resource owns two contiguous XTX header sequences. */

typedef struct MoviePlaybackWork {
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
} MoviePlaybackWork;

extern void *GameResourceAlloc(int size);

extern void *GameResourceRealloc(void *resource, int size);

extern void xglMovieInfoInit(MoviePlaybackWork *info);

extern int xglMovieOpen(MoviePlaybackWork *info, int mode);

extern MovieXtxHeader *xglMovieMakeXtxHeader(MoviePlaybackWork *info, MovieXtxHeader *header);

/* The movie playback resource owns two contiguous XTX header sequences. */

enum {
    MOVIE_NEXT_HEADER = 0,
    MOVIE_XTX_DATA = 1,
};

#include "main/toolkit.h"

typedef struct Unit6003ResourceEntry {
    u32 address;
    int size;
    int handle;
    int state;
} Unit6003ResourceEntry;

typedef struct Unit6003ResourceHeader {
    u32 signature;
} Unit6003ResourceHeader;

typedef union Unit6003Coordinate {
    int screen;
    float world;
} Unit6003Coordinate;

typedef struct Unit6003Work {
    u32 flags;
    unsigned char unmodeled_04[0x08 - 0x04];
    void (*draw_callback)(void *unit);
    unsigned char unmodeled_0c[0xa1 - 0x0c];
    u8 active;
    u8 state;
    unsigned char unmodeled_a3[0xa6 - 0xa3];
    short render_mode;
    short transition_timer;
    unsigned char unmodeled_aa[0x1a0 - 0xaa];
    Unit6003Coordinate left;
    Unit6003Coordinate top;
    Unit6003Coordinate width;
    Unit6003Coordinate height;
    u32 resource_id;
    int use_screen_coordinates;
    unsigned int texture_width : 16;
    unsigned int frames_per_row : 16;
    unsigned int texture_height : 16;
    unsigned int frames_per_column : 16;
    u8 alpha;
    unsigned char unmodeled_1c1[3];
    u32 movie_advance_count;
    int frame_count;
    int playback_direction;
    float top_depth;
    float bottom_depth;
    u8 frame_index;
    u8 frame_timer;
    u8 frame_delay;
    u8 frame_type;
    unsigned char unmodeled_1dc[4];
    MoviePlaybackWork *movie_work;
} Unit6003Work;

extern Unit6003ResourceEntry GameResource[128];

extern int GameResourceGetIndex(int address);

extern int RES_loadFile(int command, int callback, int resource_id,
                        int flags);

extern int printf(const char *format, ...);

#include "main/xgl_studio.h"

#include "main/xgl_2.h"

struct MapUnitRecord;

extern void MAP_updateUnitDefault(struct MapUnitRecord *unit);

extern int xglMoviePlay(void *info);

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

extern Unit6003RenderState sRender;

extern float I2F(int value);

extern void nmlModelDirectSend(int mode, unsigned char *packet, int size);

extern void nmlModelDirectSendXtx(int mode, void *texture);

extern float xglRotTransPers(int *result, float matrix[4][4],
                             float *position, int camera);

extern const char D_004D2170[24];

static const char D_004D2188[24] =
    "fm%04d \244\362\306\311\244\337\271\376\244\337\244\336\244\271\n";

static const char D_004D21A0[24] =
    "mvs%04d \244\362\306\311\244\337\271\376\244\337\244\336\244\271\n";

static const char D_004D21B8[24] = "movie work broken!\n";

static const char D_004D21D0[16] = "%08x->%08x\n";

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

static void unit6003_draw(void *object)
{
    Unit6003Work *unit = object;
    Unit6003Scratch *scratch = (Unit6003Scratch *)0x70000000;
    Unit6003DrawVertex *vertex;
    int draw_mode = unit->render_mode == 0 ? 5 : 1;
    int camera_index;
    int corner;
    int texture_page;
    float texture_scale;

    switch (unit->frame_type) {
    case 0:
        nmlModelDirectSendXtx(draw_mode, unit->movie_work);
        break;
    case 1: {
        MoviePlaybackWork *movie = unit->movie_work;
        nmlModelDirectSendXtx(draw_mode,
                              movie->headers[movie->header_index]);
        break;
    }
    default:
        printf(D_004D2170, unit->frame_type);
        break;
    }

    switch (unit->frame_type) {
    case 0:
        texture_page = sRender.texture_page << 5;
        TestEnv_0_00490520.texture =
            (0x25320000 | texture_page) |
            ((u64)(texture_page + clut_1[unit->frame_index] + 0x380) << 37) |
            0x2000000640000000ULL;
        break;
    case 1:
        TestEnv_0_00490520.texture =
            (0x20010000 | (sRender.texture_page << 5)) |
            0x2000000600000000ULL;
        break;
    }
    vertex = TestEnv_0_00490520.vertices;
    for (camera_index = 0; camera_index < 8; ++camera_index) {
        if (xglStudioGetCamera2(camera_index)->active != 0)
            break;
    }

    switch (unit->frame_type) {
    case 0:
        texture_scale = 1.0f / 512.0f;
        scratch->texture_left = I2F(
            (unit->frame_index % unit->frames_per_row) *
            unit->texture_width) / 512.0f;
        scratch->texture_top = I2F(
            (unit->frame_index / unit->frames_per_row) *
            unit->texture_height) / 512.0f;
        scratch->texture_width = I2F(unit->texture_width) * texture_scale;
        scratch->texture_height = I2F(unit->texture_height) * texture_scale;
        break;
    case 1:
        scratch->texture_left = 0.0f;
        scratch->texture_top = 0.0f;
        texture_scale = 1.0f / 256.0f;
        scratch->texture_width = I2F(unit->texture_width) * texture_scale;
        scratch->texture_height = I2F(unit->texture_height) * texture_scale;
        break;
    }

    scratch->screen_coordinates = unit->use_screen_coordinates;
    scratch->alpha = unit->alpha;
    if (scratch->screen_coordinates == 0) {
        scratch->world_left = unit->left.world - unit->width.world * 0.5f;
        scratch->world_top = unit->top.world + unit->height.world * 0.5f;
        scratch->depth[0] = unit->top_depth;
        scratch->depth[1] = unit->bottom_depth;
        scratch->world_width = unit->width.world;
        scratch->world_height = unit->height.world;
    } else {
        scratch->screen_left = unit->left.screen * 16 + 0x6ff8;
        {
            int top = unit->top.screen;
            scratch->screen_depth = -1;
            scratch->screen_top = top * 16 + 0x71f8;
        }
        scratch->screen_width = unit->width.screen * 16;
        scratch->screen_height = unit->height.screen * 16;
    }
    xglMatrixStackSave(scratch->matrix);

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
            if ((vertex->x & 0xffff0000) != 0 ||
                (vertex->y & 0xffff0000) != 0 || vertex->z < 0)
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
        vertex->red = 0x80;
        vertex->green = 0x80;
        vertex->blue = 0x80;
        vertex->texture_q = perspective;
        vertex->alpha = scratch->alpha;
    }
    nmlModelDirectSend(draw_mode, (unsigned char *)&TestEnv_0_00490520, 0x15);
}

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
    info->headers[MOVIE_XTX_DATA] = info->xtx_headers;
    header_cursor = xglMovieMakeXtxHeader(info, info->headers[MOVIE_XTX_DATA]);
    info->first_header = header_cursor;
    header_cursor += info->xtx_header_count;
    info->headers[MOVIE_NEXT_HEADER] = header_cursor;
    header_cursor = xglMovieMakeXtxHeader(info, header_cursor);
    info->second_header = header_cursor;
    header_cursor += info->xtx_header_count;
    allocation_size = (int)((unsigned char *)header_cursor - (unsigned char *)info);
    GameResourceRealloc(info, allocation_size);
    return info;
}

void unit6003_update(Unit6003Work *unit)
{
    switch (unit->state) {
    case 0: {
        u32 resource_index = unit->resource_id;
        Unit6003ResourceHeader *resource;


        if (resource_index - UNIT6003_RESOURCE_GROUP_FM <
            UNIT6003_RESOURCE_GROUP_SIZE) {
            unsigned int resource_file_id =
                resource_index % UNIT6003_RESOURCE_GROUP_SIZE;
            printf(D_004D2188, resource_file_id);
            resource_index = GameResourceGetIndex(
                RES_loadFile(-1, 11, resource_file_id, 0));
            unit->resource_id = resource_index;
        } else if (resource_index - UNIT6003_RESOURCE_GROUP_MVS <
                   UNIT6003_RESOURCE_GROUP_SIZE) {
            unsigned int resource_file_id =
                resource_index % UNIT6003_RESOURCE_GROUP_SIZE;
            printf(D_004D21A0, resource_file_id);
            resource_index = GameResourceGetIndex(
                RES_loadFile(-1, 10, resource_file_id, 0));
            unit->resource_id = resource_index;
        }

        unit->state = 1;
        unit->flags |= 4;
        unit->draw_callback = 0;
        unit->frame_index = 0;
        unit->frame_timer = 0;
        if (unit->playback_direction < 0)
            unit->frame_delay = -(u8)unit->playback_direction;
        else
            unit->frame_delay = (u8)unit->playback_direction;

        resource =
            (Unit6003ResourceHeader *)GameResource[unit->resource_id].address;
        unit->frame_type = 0xff;
        if (resource != 0) {
            if (resource->signature == UNIT6003_XTX_SIGNATURE) {
                if (unit->texture_width == 0)
                    unit->texture_width = 0x80;
                if (unit->texture_height == 0)
                    unit->texture_height = 0x70;
                unit->frames_per_row =
                    (u16)(0x200 / (int)unit->texture_width);
                unit->frames_per_column =
                    (u16)(0x1c0 / (int)unit->texture_height);
                unit->frame_type = 0;
            } else if (resource->signature == UNIT6003_IPUM_SIGNATURE) {
                u32 played;
                MoviePlaybackWork *movie_work;

                movie_work = movie_work_setup(
                    resource, GameResource[unit->resource_id].size);
                played = 0;
                unit->texture_width = movie_work->image_width;
                unit->texture_height = movie_work->image_height;
                do {
                    played += 1;
                    xglMoviePlay(movie_work);
                } while (played <= unit->movie_advance_count);
                __builtin_memset(&unit->movie_advance_count, 0,
                                 sizeof(unit->movie_advance_count));
                {
                    const short *movie_frame_count_source = &movie_work->frame_count;
                    short movie_frame_count = *movie_frame_count_source;

                    unit->frame_type = 1;
                    unit->frame_count = movie_frame_count;
                }
                resource = (Unit6003ResourceHeader *)movie_work;
            }
        }
        unit->movie_work = (MoviePlaybackWork *)resource;
    }
        /* fall through */
    case 1:
        if (unit->active != 0) {
            unit->state = 2;
            unit->transition_timer = 0x0f;
            unit->draw_callback = unit6003_draw;
    case 2:
            if (unit->transition_timer == 0) {
                unit->state = 3;
    case 3:
                if (unit->active == 0) {
                    unit->state = 4;
                    unit->transition_timer = 0x0f;
                }
            } else {
                unit->transition_timer =
                    (short)(unit->transition_timer - 1);
            }
        }
        break;
    case 4:
        if (unit->transition_timer != 0) {
            unit->transition_timer = (short)(unit->transition_timer - 1);
            break;
        }
        unit->draw_callback = 0;
        unit->state = 1;
        break;
    }

    if (unit->frame_type == 1) {
        MoviePlaybackWork *movie_work = unit->movie_work;

        if (GameResourceGetIndex((int)movie_work) == -1 ||
            movie_work->source_data !=
                (void *)GameResource[unit->resource_id].address ||
            movie_work->headers[MOVIE_NEXT_HEADER]->signature !=
                UNIT6003_XTX_SIGNATURE ||
            movie_work->headers[MOVIE_XTX_DATA]->signature !=
                UNIT6003_XTX_SIGNATURE) {
            MoviePlaybackWork *repaired_work;

            printf(D_004D21B8);
            repaired_work = movie_work_setup(
                (void *)GameResource[unit->resource_id].address,
                GameResource[unit->resource_id].size);
            printf(D_004D21D0, (u32)unit->movie_work,
                   (u32)repaired_work);
            unit->movie_work = repaired_work;
        }
    }

    if (unit->active != 0) {
        unsigned int frame_timer = unit->frame_timer + 1;

        unit->frame_timer = frame_timer;
        if ((u8)frame_timer >= unit->frame_delay) {
            unit->frame_timer = 0;
            switch (unit->frame_type) {
            case 0: {
                unit->frame_index++;
                if (unit->frame_index > (u32)unit->frame_count) {
                    if (unit->playback_direction < 0)
                        unit->frame_index = 0;
                    else
                        unit->frame_index = (u8)unit->frame_count;
                }
                break;
            }
            case 1: {
                MoviePlaybackWork *movie_work = unit->movie_work;

                if (unit->movie_advance_count == 0) {
                    if (xglMoviePlay(movie_work) != 0) {
                        if (unit->playback_direction >= 0)
                            unit->movie_advance_count = 1;
                    }
                    unit->frame_index = movie_work->current_frame_index;
                }
                break;
            }
            }
        }
    }

    MAP_updateUnitDefault((struct MapUnitRecord *)unit);
}
