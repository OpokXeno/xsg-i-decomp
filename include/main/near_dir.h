#ifndef INCLUDE_MAIN_NEAR_DIR_H
#define INCLUDE_MAIN_NEAR_DIR_H

#include "shared.h"
#include "main/xgl_studio.h"
#include "main/act_1.h"

/*
 * Channels of a sequence entry. SequenceState.flags marks the channels that
 * have been started and state_flags the channels still running; both use
 * these bits. Rotation and scale have one bit per axis.
 */
#define SEQ_CHANNEL_MOVE 0x1
#define SEQ_CHANNEL_ROTATE_X 0x2
#define SEQ_CHANNEL_ROTATE_Y 0x4
#define SEQ_CHANNEL_ROTATE_Z 0x8
#define SEQ_CHANNEL_ROTATE_ALL 0xe
#define SEQ_CHANNEL_MOTION 0x10
#define SEQ_CHANNEL_SCALE_X 0x20
#define SEQ_CHANNEL_SCALE_Y 0x40
#define SEQ_CHANNEL_SCALE_Z 0x80
#define SEQ_CHANNEL_SCALE_ALL 0xe0
/* SequenceState.flags: ACT_updateSequence trims handlers by channel_mask. */
#define SEQ_USE_CHANNEL_MASK 0x100
#define SEQ_HANDLER_COUNT 4

/* Spline channel of a sequence entry (near_dir.c and chr.c read it at +0x38/+0xb8/+0x1b8). */
typedef struct {
    void *spline;
    short frame;
    short last_frame;
    u16 flags;
} SplineTrack;

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
 * +0x08 channel_mask
 *                    the spline natives in chr.c select their channel group
 *                    here (SequenceState's view of this entry).
 *
 * +0x10 is between evidenced words and untouched by every function in this
 * TU, so it stays an unmodeled gap.
 */
typedef struct SequenceState {
    u32 flags;
    u32 state_flags;
    u32 channel_mask;
    u32 cleared_on_init;
    u32 unmodeled_10;
    void *cleared_on_init_run[4];
    void *handler[SEQ_HANDLER_COUNT];
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

typedef struct RotationTrack {
    struct Actor *target;
    u8 unmodeled_04[0x10 - 0x04];
    int duration[3];
    u32 face_flags;
    float start[3];
    u8 unmodeled_2c[0x30 - 0x2c];
    float goal[3];
    u8 unmodeled_3c[0x40 - 0x3c];
    float step[3];
} RotationTrack;

/* The scale track of a sequence entry (+0x1b8): per-axis frame counts, then
 * the start, goal and per-frame step SEQ_scale walks the actor's scale
 * through. */
typedef struct ScaleTrack {
    u8 unmodeled_00[0x10];
    int duration[3];
    u8 unmodeled_1c[0x20 - 0x1c];
    float start[3];
    u8 unmodeled_2c[0x30 - 0x2c];
    float goal[3];
    u8 unmodeled_3c[0x40 - 0x3c];
    float step[3];
} ScaleTrack;

typedef struct ActSequenceEntry {
    SequenceState state;
    u32 frame_counter;
    /* +0x38: the movement channel (SEQ_move) or, in the spline form, the spline channel. */
    union {
        SplineTrack spline;
        MoveTrack move;
    } move_channel;
    u8 unmodeled_68[0xb8 - 0x68];
    /* +0xb8: the rotation channel (SEQ_rotate) or, in the spline form, the spline channel. */
    union {
        SplineTrack spline;
        RotationTrack rotation;
    } rotation_channel;
    u8 unmodeled_104[0x138 - 0x104];
    MotionTrack motion;
    u8 unmodeled_158[0x1b8 - 0x158];
    /* +0x1b8: the scale channel (SEQ_scale) or, in the spline form, the spline channel. */
    union {
        SplineTrack spline;
        ScaleTrack scale;
    } scale_channel;
    u8 unmodeled_204[0x238 - 0x204];
    u32 idle_roll;
    u32 unmodeled_23c;
    Vector4 anchor;
    u8 unmodeled_250[0x260 - 0x250];
} ActSequenceEntry;

/* The original array at 0x0046f460 has 64 records, totaling 0x9800 bytes. */
extern ActSequenceEntry actSequence[ACTOR_COUNT];

/* Functions and data used by other translation units. */
extern float nearDir(float from, float to);
void ACT_initScene(void);

#endif /* INCLUDE_MAIN_NEAR_DIR_H */
