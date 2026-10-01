#include "common.h"
#include "shared.h"

extern unsigned char TestEnv_0_00367AA0[];

static void clear(void)
{
    sceVif1PkRef(xglPacketGetCurrent(), TestEnv_0_00367AA0, 6, 0, 0, 0);
}

/* Background draw type 0 draws nothing; Game calls it directly. */
void GameBgDrawType0(void)
{
}

/*
 * The do-nothing hook GameBgDrawType1Entry and GameBgDrawType2Entry install
 * at GameLoopState+0x30; GameDrawSync calls it with no arguments.
 */
static void nullfunc(void)
{
}

typedef struct GameBgDrawType1Particle {
    u16 x;
    u16 y;
    s16 velocity_x;
    s16 velocity_y;
} GameBgDrawType1Particle;

typedef struct GameBgDrawType1Parameters {
    u8 initialized;
    u8 unmodeled_01[2];
    u8 particle_count;
    s16 spawn_x;
    s16 spawn_y;
    u16 entry_clear[2];
    GameBgDrawType1Particle particles[256];
} GameBgDrawType1Parameters;

typedef struct GameLoopBackgroundCallbacks {
    u8 unmodeled_00[0x2c];
    void (*background_draw)(void);
    void (*background_sync)(void);
    u8 unmodeled_34[0x2a030 - 0x34];
} GameLoopBackgroundCallbacks;

extern GameBgDrawType1Parameters GameBgDrawType1Param;
extern GameLoopBackgroundCallbacks GameLoopState;
void GameBgDrawType1(void);

INCLUDE_ASM("asm/main/nonmatchings/game_bg_draw_type", GameBgDrawType1);

void GameBgDrawType1Entry(void)
{
    GameBgDrawType1Param.particle_count = -16;
    GameBgDrawType1Param.entry_clear[1] = 0;
    GameBgDrawType1Param.spawn_x = -32768;
    GameBgDrawType1Param.entry_clear[0] = 0;
    GameLoopState.background_draw = GameBgDrawType1;
    GameLoopState.background_sync = nullfunc;
    GameBgDrawType1Param.spawn_y = -32768;
    GameBgDrawType1Param.initialized = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/game_bg_draw_type", GameBgDrawType2);

INCLUDE_ASM("asm/main/nonmatchings/game_bg_draw_type", GameBgDrawType2Entry);
