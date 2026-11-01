#ifndef INCLUDE_MAIN_NEAR_DIR_H
#define INCLUDE_MAIN_NEAR_DIR_H

#include "shared.h"
#include "main/xgl_studio.h"

/*
 * The actSequence entry head. `flags` and `state_flags` are the two words
 * every function below reads; ACT_initSequenceAt additionally clears six
 * more words of the same entry when it (re)starts a slot:
 *
 *   +0x0c cleared_on_init
 *                    one word ACT_initSequenceAt clears; no function in this
 *                    TU reads it, so nothing beyond that is evidenced.
 *   +0x14 cleared_on_init_run[4]
 *                    four consecutive words (+0x14/+0x18/+0x1c/+0x20)
 *                    ACT_initSequenceAt clears the same way, otherwise
 *                    unread here.
 *   +0x24 handler[4] the sequence's four handler slots: ACT_updateSequence
 *                    walks +0x24..+0x30 and calls whichever of the four is
 *                    non-null with the actor (0x0030abe8..0x0030ac18,
 *                    src/main/chr.h's SEQ_HANDLER family names the same
 *                    layout), and SEQ_scale installs SEQ_scale itself into
 *                    the scale channel's own handler slot the same way
 *                    (main/tu248). ACT_initSequenceAt clears all four.
 *
 * +0x08 and +0x10 are between evidenced words and untouched by every
 * function in this TU, so they stay unmodeled gaps.
 */
typedef struct SequenceState {
    u32 flags;
    u32 state_flags;
    u32 unmodeled_08;
    u32 cleared_on_init;
    u32 unmodeled_10;
    void *cleared_on_init_run[4];
    void *handler[4];
} SequenceState;

typedef struct MoveTrack {
    struct Actor *target_actor;
    int frames;
    u8 unmodeled_08[0x10 - 0x08];
    Vector4 start;
    Vector4 target;
} MoveTrack;

typedef struct MotionTrack {
    float time;
    u8 unmodeled_04[0x08 - 0x04];
    u32 motion;
    u8 unmodeled_0c[0x10 - 0x0c];
    short loop_start;
    short loop_end;
    u8 blend;
    u8 unmodeled_15[0x18 - 0x15];
    u32 flags;
    float speed;
} MotionTrack;

typedef struct ActSequenceEntry {
    SequenceState state;
    u32 frame_counter;
    MoveTrack move;
    u8 unmodeled_68[0x138 - 0x68];
    MotionTrack motion;
    u8 unmodeled_158[0x238 - 0x158];
    u32 unmodeled_238;
    u32 unmodeled_23c;
    Vector4 anchor;
    u8 unmodeled_250[0x260 - 0x250];
} ActSequenceEntry;

/* The original array at 0x0046f460 has 64 records, totaling 0x9800 bytes. */
extern ActSequenceEntry actSequence[64];

#endif /* INCLUDE_MAIN_NEAR_DIR_H */
