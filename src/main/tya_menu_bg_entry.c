#include "common.h"

#include "main/xgl_2.h"

typedef struct MenuBgWork {
    u64 initial_word;
    u64 palette_packet;
    u64 color_packet;
    u64 trailing_word;
    u32 colors[512];
    int camera_id;
    u32 unmodeled_824;
    u32 tasks_finished;
    u8 red_intensity;
    u8 green_intensity;
    u8 blue_intensity;
    u8 unmodeled_82f;
    float rotation_x;
    u32 unmodeled_834;
    float rotation_z;
    float camera_position_z;
} MenuBgWork;

typedef struct MenuBgTask {
    XglTaskPrefix scheduler;
    u8 state;
    u8 index;
    u8 remaining_frames;
    u8 unmodeled_13;
    u16 position_x;
    u16 position_y;
    MenuBgWork *work;
    u32 unmodeled_1c;
    float horizontal_offset;
    float vertical_offset;
    float depth_offset;
    u32 unmodeled_2c;
    float horizontal_scale;
    u32 unmodeled_34;
    float mirrored_depth_offset;
} MenuBgTask;

typedef struct MenuBgQuad {
    u8 red_multiplier;
    u8 green_multiplier;
    u8 blue_multiplier;
    u8 alpha;
    u8 vertex_0;
    u8 vertex_1;
    u8 vertex_2;
    u8 vertex_3;
} MenuBgQuad;

typedef struct MenuBgTextureCoordinate {
    u32 s;
    u32 t;
    u32 unmodeled_08;
    u32 unmodeled_0c;
} MenuBgTextureCoordinate;

typedef int MenuBgProjectedPoint[4];

typedef struct MenuBgPacketVertex {
    MenuBgTextureCoordinate texture;
    MenuBgProjectedPoint projected;
} MenuBgPacketVertex;

typedef struct MenuBgDirectPacket {
    u64 initial_word;
    u64 vif_header;
    u64 gif_tag;
    u64 gif_registers;
    u32 color_red;
    u32 color_green;
    u32 color_blue;
    u32 color_alpha;
    MenuBgPacketVertex vertices[4];
} MenuBgDirectPacket;

typedef struct MenuBgScratch {
    Matrix4 saved_matrix;
    u8 unmodeled_040[0xc0];
    Vector4 points[8];
    MenuBgProjectedPoint projected_points[8];
    MenuBgDirectPacket packet;
} MenuBgScratch;

XglTaskPrefix *xglTaskEntryNext(XglTaskScheduler *scheduler,
                                int (*callback)(XglTaskPrefix *task),
                                XglTaskPrefix *entry);

extern float xglFRand(void);

extern float I2F(int value);

extern void xglCameraMove(StudioCamera *camera);

extern int xglTaskRemove(XglTaskPrefix *task);

extern void nmlModelDirectSend(int mode, u8 *data, int count);

extern void xglRotTransPersN(MenuBgProjectedPoint *destination,
                             Matrix4 *matrix, const Vector4 *points,
                             int count, int cameraId);

/* Original .lit4 words byte-pinned at their mapped VAs. */

#define MENUBG_VERTICAL_OFFSET_SCALE (0.4f) /* 0x004D7C88: cd cc cc 3e */

#define MENUBG_HORIZONTAL_SCALE_DECAY (0.7f) /* 0x004D7C8C: 33 33 33 3f */

#define MENUBG_MIN_HORIZONTAL_SCALE (0.001f) /* 0x004D7C90: 6f 12 83 3a */

#define D_004D7C88 MENUBG_VERTICAL_OFFSET_SCALE

#define D_004D7C8C MENUBG_HORIZONTAL_SCALE_DECAY

#define D_004D7C90 MENUBG_MIN_HORIZONTAL_SCALE

/* DMA/VIF header and GIF tag are stored little-endian on EE. These
 * initializer helpers express packet words, not opaque packet-byte blobs. */

#define EE_U32_BYTES(value) \
    (u8)((u32)(value)), (u8)((u32)(value) >> 8), \
    (u8)((u32)(value) >> 16), (u8)((u32)(value) >> 24)

#define EE_U64_BYTES(value) \
    EE_U32_BYTES((u32)(value)), EE_U32_BYTES((u32)((u64)(value) >> 32))

#define NML_DIRECT_HEADER(direct) \
    EE_U64_BYTES(0), EE_U32_BYTES(0), EE_U32_BYTES(direct)

#define GIF_TAG_WORDS(control_low, control_high, registers_low, registers_high) \
    EE_U32_BYTES(control_low), EE_U32_BYTES(control_high), \
    EE_U32_BYTES(registers_low), EE_U32_BYTES(registers_high)

#define GIF_AD_ENTRY(value, register_id) \
    EE_U64_BYTES(value), EE_U32_BYTES(register_id), EE_U32_BYTES(0)

#define NML_ZERO_QWORD EE_U64_BYTES(0), EE_U64_BYTES(0)

/* 11-qword palette transfer: DMA/VIF header, GIFtag, four A+D writes,
 * then five original zero qwords retained by the original transfer count. */

static u8 TestEnv_2_0036AAA0[176] = {
    NML_DIRECT_HEADER(0x51000005),
    GIF_TAG_WORDS(0x8004, 0x10000000, 0x0000000e, 0),
    GIF_AD_ENTRY(0x0008380000000000ULL, 0x50),
    GIF_AD_ENTRY(0, 0x51),
    GIF_AD_ENTRY(0x0000000800000040ULL, 0x52),
    GIF_AD_ENTRY(0, 0x53),
    NML_ZERO_QWORD, NML_ZERO_QWORD, NML_ZERO_QWORD,
    NML_ZERO_QWORD, NML_ZERO_QWORD,
};

/* 7-qword entry transfer: DMA/VIF header, GIFtag, and five A+D writes. */

static u8 TestEnv_3_0036AB50[112] = {
    NML_DIRECT_HEADER(0x51000006),
    GIF_TAG_WORDS(0x8001, 0x50000000, 0x000eeeee, 0),
    GIF_AD_ENTRY(0x0000000000070000ULL, 0x47),
    GIF_AD_ENTRY(0x0000000000000060ULL, 0x14),
    GIF_AD_ENTRY(0x20000004d8023800ULL, 0x06),
    GIF_AD_ENTRY(0x0000000000000044ULL, 0x42),
    GIF_AD_ENTRY(0, 0x08),
};

#undef NML_ZERO_QWORD

#undef GIF_AD_ENTRY

#undef GIF_TAG_WORDS

#undef NML_DIRECT_HEADER

#undef EE_U64_BYTES

#undef EE_U32_BYTES

static int task_menubg(XglTaskPrefix *task);

static void task_menubg_sub(XglTaskPrefix *task, float scale);

/* Original .lit4 words byte-pinned at their mapped VAs. */

/* DMA/VIF header and GIF tag are stored little-endian on EE. These
 * initializer helpers express packet words, not opaque packet-byte blobs. */

/* 11-qword palette transfer: DMA/VIF header, GIFtag, four A+D writes,
 * then five original zero qwords retained by the original transfer count. */

/* 7-qword entry transfer: DMA/VIF header, GIFtag, and five A+D writes. */

static void task_menubg_sub(XglTaskPrefix *task_prefix, float scale)
{
    static Vector4 model[8] = {
        {0.0f, 0.4f, -0.8f, 1.0f}, {7.0f, 0.4f, -0.8f, 1.0f},
        {0.0f, 0.4f,  0.0f, 1.0f}, {7.0f, 0.4f,  0.0f, 1.0f},
        {0.0f,-0.4f,  0.0f, 1.0f}, {7.0f,-0.4f,  0.0f, 1.0f},
        {0.0f,-0.4f, -0.8f, 1.0f}, {7.0f,-0.4f, -0.8f, 1.0f},
    };
    static MenuBgQuad index[4] = {
        {0xb0, 0xb0, 0xb0, 0x80, 1, 3, 0, 2},
        {0x80, 0x80, 0x80, 0x80, 3, 5, 2, 4},
        {0x50, 0x50, 0x50, 0x80, 5, 7, 4, 6},
        {0x40, 0x40, 0x40, 0x80, 0, 2, 6, 4},
    };
    MenuBgTask *task = (MenuBgTask *)task_prefix;
    MenuBgScratch *scratch = (MenuBgScratch *)0x70000000;
    MenuBgProjectedPoint *screen_points = scratch->projected_points;
    int i;
    int texture_x;
    int texture_right;
    int texture_y;
    int texture_bottom;
    const u8 *quad_bytes;
    float depth_adjustment;
    float base_depth;
    /* The scratchpad x word is the zero scale sentinel. */
    ((volatile MenuBgScratch *)0x70000000)->points[0].x = 0.0f;
    scratch->points[0].y = task->vertical_offset;
    depth_adjustment = ((volatile MenuBgScratch *)0x70000000)->points[0].x;
    base_depth = task->depth_offset;

    if (scale < depth_adjustment) {
        scratch->points[0].z = base_depth + task->mirrored_depth_offset;
    } else {
        scratch->points[0].z = base_depth + depth_adjustment;
    }
    scratch->points[0].w = 1.0f;

    xglMatrixStackUnit();
    xglMatrixStackRotX(task->work->rotation_x);
    xglMatrixStackRotZ(task->work->rotation_z);
    xglMatrixStackTrans(&scratch->points[0].x);
    xglMatrixStackSave(scratch->saved_matrix);

    for (i = 0; i < 8; i++) {
        if (model[i].x == 0.0f) {
            scratch->points[i].x =
                task->horizontal_offset + task->horizontal_scale * scale;
        } else {
            scratch->points[i].x =
                (task->horizontal_offset + model[i].x) * scale;
        }
        scratch->points[i].y = model[i].y;
        scratch->points[i].z = model[i].z;
        scratch->points[i].w = 1.0f;
    }

    xglRotTransPersN(screen_points, &scratch->saved_matrix, scratch->points, 8,
                     task->work->camera_id);
    for (i = 0; i < 8; i++) {
        int *screen_vertex = screen_points[i] + 2;

        if (((u32)screen_vertex[-2] & 0xffff0000u) != 0 ||
            ((u32)screen_vertex[-1] & 0xffff0000u) != 0 ||
            screen_vertex[0] < 0) {
            return;
        }
    }

    /* The byte cursor follows the interleaved color and vertex-index records. */
    quad_bytes = (const u8 *)index;
    texture_x = task->position_x;
    texture_y = task->position_y;
    if (scale < 0.0f) {
        texture_y = (u16)(texture_y + 16);
    }
    texture_right = texture_x + 1008;
    texture_bottom = texture_y + 32;

    scratch->packet.initial_word = 0;
    scratch->packet.vif_header = 0x5100000a00000000ULL;
    scratch->packet.gif_tag = 0x908a400000008001ULL;
    scratch->packet.gif_registers = 0x0000000535353531ULL;

    for (i = 0; i < 4; i++) {
        MenuBgProjectedPoint *screen_vertex;
        u8 alpha;

        scratch->packet.color_red =
            (task->work->red_intensity * quad_bytes[0]) >> 7;
        scratch->packet.color_green =
            (task->work->green_intensity * quad_bytes[1]) >> 7;
        scratch->packet.color_blue =
            (task->work->blue_intensity * quad_bytes[2]) >> 7;
        alpha = quad_bytes[3];
        quad_bytes += 4;

        scratch->packet.color_alpha = alpha;
        screen_vertex = screen_points + *quad_bytes++;
        scratch->packet.vertices[0].texture.s = texture_x;
        scratch->packet.vertices[0].texture.t = texture_y;
        scratch->packet.vertices[0].projected[0] = (*screen_vertex)[0];
        scratch->packet.vertices[0].projected[1] = (*screen_vertex)[1];
        scratch->packet.vertices[0].projected[2] = (*screen_vertex)[2];
        scratch->packet.vertices[0].projected[3] = 0;

        screen_vertex = screen_points + *quad_bytes++;
        scratch->packet.vertices[1].texture.s = texture_right;
        scratch->packet.vertices[1].texture.t = texture_y;
        scratch->packet.vertices[1].projected[0] = (*screen_vertex)[0];
        scratch->packet.vertices[1].projected[1] = (*screen_vertex)[1];
        scratch->packet.vertices[1].projected[2] = (*screen_vertex)[2];
        scratch->packet.vertices[1].projected[3] = 0;

        screen_vertex = screen_points + *quad_bytes++;
        scratch->packet.vertices[2].texture.s = texture_x;
        scratch->packet.vertices[2].texture.t = texture_bottom;
        scratch->packet.vertices[2].projected[0] = (*screen_vertex)[0];
        scratch->packet.vertices[2].projected[1] = (*screen_vertex)[1];
        scratch->packet.vertices[2].projected[2] = (*screen_vertex)[2];
        scratch->packet.vertices[2].projected[3] = 0;

        screen_vertex = screen_points + *quad_bytes++;
        scratch->packet.vertices[3].texture.s = texture_right;
        scratch->packet.vertices[3].texture.t = texture_bottom;
        scratch->packet.vertices[3].projected[0] = (*screen_vertex)[0];
        scratch->packet.vertices[3].projected[1] = (*screen_vertex)[1];
        scratch->packet.vertices[3].projected[2] = (*screen_vertex)[2];
        scratch->packet.vertices[3].projected[3] = 0;

        nmlModelDirectSend(1, (u8 *)&scratch->packet, 0xb);
    }
}

static int task_menubg(XglTaskPrefix *task_prefix)
{
    MenuBgTask *task = (MenuBgTask *)task_prefix;
    int frame_delay;

    switch (task->state) {
    case 0:
        task->horizontal_offset = 2.0f * (xglFRand() - 1.5f);
        task->vertical_offset = I2F((8 - task->index) * 2) * D_004D7C88;
        task->depth_offset = (xglFRand() - 1.5f) * 0.5f;
        task->horizontal_scale = 2.0f * xglFRand() + 7.0f;
        task->mirrored_depth_offset = (xglFRand() - 1.5f) * 0.5f;
        frame_delay = xglSRand() & 7;
        task->state = 1;
        task->remaining_frames = frame_delay;
        /* fall through */
    case 1:
        if (task->remaining_frames != 0) {
            task->remaining_frames--;
            break;
        }
        task->state = 2;
        /* fall through */
    case 2:
        task->horizontal_scale *= D_004D7C8C;
        if (task->horizontal_scale < D_004D7C90) {
            task->horizontal_scale = 0.0f;
            task->state = 3;
        }
        break;
    }

    if (task->index == 0) {
        StudioCamera *camera;

        nmlModelDirectSend(2, TestEnv_2_0036AAA0, 11);
        nmlModelDirectSend(1, (u8 *)task->work, 130);
        camera = xglStudioGetCamera2(task->work->camera_id);
        camera->position.x = 0.0f;
        camera->position.y = 0.0f;
        camera->position.z = task->work->camera_position_z;
        camera->position.w = 1.0f;
        camera->rotation.w = 1.0f;
        camera->rotation.x = 0.0f;
        camera->rotation.y = 0.0f;
        camera->rotation.z = 0.0f;
        xglCameraMove(camera);
    }

    nmlModelDirectSend(2, TestEnv_3_0036AB50, 7);
    task_menubg_sub(task_prefix, 1.0f);
    task_menubg_sub(task_prefix, -1.0f);
    if (task->work->tasks_finished == 0) {
        return (int)task->work;
    }
    return xglTaskRemove(task_prefix);
}

void tyaMenuBgEntry(XglTaskScheduler *scheduler, MenuBgWork *work)
{
    int color_index;
    int task_index;
    int (*task_callback)(XglTaskPrefix *task);
    u32 *color_cursor = work->colors;

    work->tasks_finished = 0;
    work->initial_word = 0;
    work->palette_packet = 0x5100008100000000ULL;
    work->color_packet = 0x0800000000008080ULL;
    work->trailing_word = 0;
    for (color_index = 511; color_index >= 0; color_index--) {
        u32 shade = xglSRand() & 0x30;

        *color_cursor = shade * 0x010101 + 0x80808080;
        color_cursor++;
    }

    task_callback = task_menubg;
    for (task_index = 0; task_index < 16; task_index++) {
        XglTaskPrefix *entry = 0;
        MenuBgTask *task;
        u32 randomized_y;

        if (scheduler != 0) {
            entry = scheduler->active_tail;
        }
        task = (MenuBgTask *)xglTaskEntryNext(scheduler, task_callback, entry);
        if (task != 0) {
            task->index = task_index;
            task->state = 0;
            task->position_x = 0;
            randomized_y = xglSRand() & 0x3f;
            task->work = work;
            task->position_y = randomized_y;
        }
    }
}
