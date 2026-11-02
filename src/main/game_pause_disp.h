/*
 * TU-local declarations of main/tu117 (src/main/game_pause_disp.c).
 */

#ifndef SRC_MAIN_GAME_PAUSE_DISP_H
#define SRC_MAIN_GAME_PAUSE_DISP_H
#include "main/game.h"
#include "main/game_pause_disp.h"
#include "main/game_snap_shot.h"
#include "main/xgl_font.h"
#include "main/xgl_render.h"

typedef union GifCommandData {
    u64 value;
    struct {
        u32 low;
        u32 high;
    } words;
} GifCommandData;

typedef struct GifAdCommand {
    GifCommandData data;
    u32 register_address;
    u32 unused;
} GifAdCommand;

typedef struct GifTag {
    u32 control_low;
    u32 control_high;
    u32 registers_low;
    u32 registers_high;
} GifTag;

typedef struct PauseShadowCommand {
    u64 dma_tag;
    u32 vif_nop;
    u32 vif_direct;
    GifTag gif_tag;
    GifAdCommand set_shadow_context;
    GifAdCommand set_shadow_environment;
    GifAdCommand set_shadow_test;
    GifAdCommand set_shadow_alpha;
    GifAdCommand set_shadow_color;
} PauseShadowCommand;

/* The pause menu reads these fields from the game loop's global state. */
typedef struct PauseGameLoopState {
    unsigned char unmodeled_00[0xD0];
    signed char snapshot_slot;
    unsigned char unmodeled_d1[0x29E7F];
    unsigned char pause_mode;
    unsigned char unmodeled_29f51[0x1F];
    unsigned short snapshot_flags;
    unsigned short scanline_value;
} PauseGameLoopState;

extern PauseGameLoopState GameLoopState;
extern void sceVif1PkAddDirectDataN(void *packet, const void *data, int count);
extern void GamePauseDispCf(void);
extern int GameSnapShotSave(signed char slot, unsigned char *work);

#endif /* SRC_MAIN_GAME_PAUSE_DISP_H */
