/*
 * TU-local declarations of main/tu126 (src/main/game_debug_menu.c).
 */

#ifndef SRC_MAIN_GAME_DEBUG_MENU_H
#define SRC_MAIN_GAME_DEBUG_MENU_H

#include "shared.h"

extern void xglFontDebugPrintf(int x, int y, const char *format, ...);

/*
 * PadData's pressed-button halfword (include/shared.h PadPrefix.half_2a).
 * PauseMenuPage0 and GameDebugMenu both test bit 0x40 of it; other TUs name
 * the same bit PAD_CANCEL (src/main/game_over.c) or PAD_BATU
 * (src/ov12/xrg_pad.c) for their own contexts. This TU's uses cycle/dismiss
 * the debug pages, so it keeps the physical button name.
 */
#define PAD_CROSS 0x0040
#define PAD_L1 0x0004
#define PAD_R1 0x0008
extern PadPrefix PadData;

/*
 * The engine's actor record (`actor`, 64-entry array at main 0x0043c1e0,
 * 0xa70-byte stride; fuller evidence in src/main/near_dir.h,
 * src/main/enemy_2.h, src/main/set_motion.h and src/main/db_light_write.h).
 * PauseMenuPage0 reads only `inUseId` (+0x86, the in-use id ACT_create
 * writes and ACT_update skips a slot on when it is zero); the offsets
 * between the head and it are not evidenced by this TU and stay unmodeled.
 */
#define ACTOR_COUNT 64
#define ACTOR_IN_USE_ID_OFFSET 0x86

typedef struct {
    u32 flags;
    u8 unmodeled_04[ACTOR_IN_USE_ID_OFFSET - 0x04];
    short inUseId;
    u8 unmodeled_88[0xA70 - (ACTOR_IN_USE_ID_OFFSET + 2)];
} ActorHead;

extern ActorHead actor[ACTOR_COUNT];

void GameResourceDump(int dump);

/* GameDebugMenu selects six pages and resets this byte when cycling pages.
 * The surrounding GameLoopState bytes are not accessed by this TU. */
typedef struct {
    unsigned char unmodeled_00[0x29f50];
    unsigned char debugPage;
    unsigned char debugPageInitialized;
} DebugMenuPageState;

extern DebugMenuPageState GameLoopState;
static void PauseMenuPage2(void);
static void PauseMenuPage3(void);
extern void PauseMenuPageUwamono(void);
extern void PauseMenuPageEnemy(void);
extern void PauseMenuPagePartyDebug(void);

#endif /* SRC_MAIN_GAME_DEBUG_MENU_H */
