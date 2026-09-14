/*
 * TU-local declarations of main/tu116 (src/main/game.c).
 */

#ifndef SRC_MAIN_GAME_H
#define SRC_MAIN_GAME_H

#include "shared.h"

typedef void (*GameModeCameraCallback)(void);

/*
 * PARTIAL ACCESSED PREFIX of PadData (0xd0-byte object at 0x490d90).
 * GameModeDebugMenu reads only the halfword at +0x2e (masked with 0x100),
 * which the shared PadPrefix/PadDataLayout views (evidenced to +0x2c) do
 * not reach. held/pressed keep the accepted names at +0x28/+0x2a; the
 * +0x2c halfword is not accessed here. Neutral debug_buttons name: only the
 * 0x100 debug-menu exit test is evidenced.
 *
 * header_divergence (recorded, allowed): this spelling of PadData's layout
 * (PadDataDebugLayout, reaching +0x2e) differs from the shared
 * include/xeno/core/types.h PadDataLayout (accepted from
 * src/core/main-00244d20/private.h, evidenced only to +0x2c). Declared
 * locally, not through the shared header, because header_harvest.py cannot
 * regenerate/promote this native-TI unit yet (no native-TI promote backend;
 * review core-cc-20260911-b11, GameModeDebugMenu section): harvesting this
 * type into the shared header while this file also included it produced a
 * redefinition. Kept TU-local (like main-00244d20's own partial views)
 * until the infra gap is closed and a canon decision merges the two views.
 */
typedef struct PadDataDebugLayout {
    u8 _unmodeled_00[0x28];
    u16 held;
    u16 pressed;
    u16 _unmodeled_2c;
    u16 debug_buttons;
} PadDataDebugLayout;

#endif /* SRC_MAIN_GAME_H */
