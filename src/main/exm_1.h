/* exm_1.h: TU-local declarations of main/tu267 (src/main/exm_1.c).
 */
#ifndef SRC_MAIN_EXM_1_H
#define SRC_MAIN_EXM_1_H

#include "shared.h"

/*
 * The engine's actor record, defined here exactly as src/main/near_dir.h
 * (main/tu257), src/main/set_motion.h (main/tu193) and src/main/chr.h
 * (main/tu248) define its head; the full field-by-field evidence for
 * +0x00..+0x70 is in near_dir.h's copy and in
 * .work/analysis/layout-actor-family.md.
 *
 * EXM_InitMovedHair/EXM_OnMovedHair/EXM_OffMovedHair/EXM_SetDelayMovedHair
 * (this unit, main/tu267) extend it with the fields they read or write on
 * one entry of the 64-entry `actor` array (main 0x0043c1e0, 0xa70-byte
 * stride):
 *   +0xa40 moved_hair_mode: EXM_OnMovedHair sets 2, EXM_OffMovedHair sets 0,
 *          EXM_InitMovedHair defaults to 2; JNT_computeMatrix reads it with
 *          lbu (0x00314b70).
 *   +0xa44 moved_hair_delay: EXM_SetDelayMovedHair overwrites it from $f12,
 *          EXM_InitMovedHair defaults it to 0.5f; JNT_computeMatrix reads it
 *          as a float (0x00314c24).
 *   +0xa50 moved_hair_sway[4]: a four-float state JNT_computeMatrix filters
 *          every frame (0x00314bb0..0x00314c88, lwc1/swc1); EXM_InitMovedHair
 *          clears all four elements.
 */
typedef struct Actor {
    u32 flags;
    void (*update)(struct Actor *actor);
    void (*draw)(struct Actor *actor);
    u32 quadword_alignment_gap;
    Vector4 position;
    Vector4 previous_position;
    Vector4 velocity;
    Vector4 acceleration;
    Vector4 rotation;
    Vector4 scale;
    unsigned char unmodeled_070[0xa40 - 0x70];
    unsigned char moved_hair_mode;  /* +0xa40 */
    unsigned char unmodeled_a41[3];
    float moved_hair_delay;         /* +0xa44 */
    unsigned char unmodeled_a48[8];
    float moved_hair_sway[4];       /* +0xa50 */
} Actor;

#endif
