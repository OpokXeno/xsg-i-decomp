#include "common.h"
#include "shared.h"

#define NULL ((void *)0)

INCLUDE_ASM("asm/main/nonmatchings/game_defocus", GetTex0);

INCLUDE_ASM("asm/main/nonmatchings/game_defocus", DefocusMainType01);

INCLUDE_ASM("asm/main/nonmatchings/game_defocus", DefocusMainType02);

INCLUDE_ASM("asm/main/nonmatchings/game_defocus", DefocusMainType03);

INCLUDE_ASM("asm/main/nonmatchings/game_defocus", DefocusMainType05);

INCLUDE_ASM("asm/main/nonmatchings/game_defocus", DefocusMainType06);

INCLUDE_ASM("asm/main/nonmatchings/game_defocus", DefocusMainType07);

INCLUDE_ASM("asm/main/nonmatchings/game_defocus", DefocusMainType08);

INCLUDE_ASM("asm/main/nonmatchings/game_defocus", DefocusMainType09);

INCLUDE_ASM("asm/main/nonmatchings/game_defocus", DefocusMainType09Final);

INCLUDE_ASM("asm/main/nonmatchings/game_defocus", DefocusMainType10);

INCLUDE_ASM("asm/main/nonmatchings/game_defocus", DefocusMainType11);

INCLUDE_ASM("asm/main/nonmatchings/game_defocus", DefocusMain);

/*
 * Partial view of one GameDefocusParam layer entry: 16 entries, stride
 * 0x44 (see the index arithmetic in GameDefocusCheck at main:0x0024fe70
 * and GameDefocusFinalize at main:0x0024fed8). type selects the active
 * DefocusMainTypeNN handler (0 = layer inactive); state is each handler's
 * own per-type working byte, read back and rewritten by the
 * DefocusMainTypeNN functions (e.g. DefocusMainType09, DefocusMainType10);
 * index records the entry's own position in the array, written back by
 * GameDefocusFinalize. The remaining bytes are per-type storage not
 * evidenced by these two functions.
 */
typedef struct {
    u8 type;
    u8 state;
    u8 index;
    u8 unmodeled_03[0x41];
} DefocusLayer;

extern DefocusLayer GameDefocusParam[16];

/*
 * Partial view of the shared render-state global sRender (see
 * src/main/game_over.c's DrawImageRenderState, src/main/map_1.c's
 * MapRenderState and src/main/window_tex_load.c's UmnRenderSize for other
 * TUs' views of the same object). callback is the per-frame defocus draw
 * routine GameDefocusCheck installs once any layer is active, matching
 * DefocusMain's own parameter.
 */
typedef struct {
    u8 unmodeled_00[0x34];
    void (*callback)(XglPacket *packet);
} DefocusRenderState;

extern DefocusRenderState sRender;

extern void DefocusMain(XglPacket *packet);
extern void DefocusMainType09Final(DefocusLayer *layer);

void GameDefocusCheck(void)
{
    int layer_index;
    int active;

    active = 0;
    layer_index = 0;
    if (GameDefocusParam[0].type == 0) {
next_layer:
        layer_index++;
        if (layer_index < 16) {
            if (GameDefocusParam[layer_index].type != 0) {
                goto layer_active;
            }
            goto next_layer;
        }
    } else {
layer_active:
        active = 1;
    }
    if (active == 0) {
        sRender.callback = NULL;
        return;
    }
    sRender.callback = DefocusMain;
}

static void GameDefocusFinalize(int index)
{
    DefocusLayer *layer;

    layer = &GameDefocusParam[index];
    if (layer->type == 9) {
        DefocusMainType09Final(layer);
    }
    layer->index = index;
    layer->state = 0;
    layer->type = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/game_defocus", GameDefocusSet);

INCLUDE_ASM("asm/main/nonmatchings/game_defocus", GameDefocusQuickSet);
