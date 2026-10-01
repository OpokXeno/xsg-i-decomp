#ifndef INCLUDE_MAIN_TYA_MENU_BG_ENTRY_H
#define INCLUDE_MAIN_TYA_MENU_BG_ENTRY_H

#include "shared.h"

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

#endif /* INCLUDE_MAIN_TYA_MENU_BG_ENTRY_H */
