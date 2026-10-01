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
    u32 unmodeled_82c;
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

XglTaskPrefix *xglTaskEntryNext(XglTaskScheduler *scheduler,
                                int (*callback)(XglTaskPrefix *task),
                                XglTaskPrefix *entry);
extern float xglFRand(void);
extern float I2F(int value);
extern void xglCameraMove(StudioCamera *camera);
extern int xglTaskRemove(XglTaskPrefix *task);
extern void nmlModelDirectSend(int mode, u8 *data, int count);
extern const float D_004D7C88;
extern const float D_004D7C8C;
extern const float D_004D7C90;
extern u8 TestEnv_2_0036AAA0[];
extern u8 TestEnv_3_0036AB50[];
static int task_menubg(XglTaskPrefix *task);
static void task_menubg_sub(XglTaskPrefix *task, float scale);

INCLUDE_ASM("asm/main/nonmatchings/tya_menu_bg_entry", task_menubg_sub);

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
