#include "common.h"
#include "main/near_dir.h"
#include "near_dir.h"
#include "main/play.h"
#include "main/xgl_studio.h"
#include "main/Undulate.h"
#include "main/act_1.h"
#include "main/act_2.h"
#include "main/game_camera.h"
#include "main/nml_model_set.h"
#include "main/xgl_2.h"
#include "quadword.h"
#include "vu0_macro.h"

typedef struct {
    u32 active;
} StudioCameraActivePrefix;

/* MotionTrack is defined by this TU's owning near_dir.h. */
/* Additional sequence-owned Actor slots. The canonical Actor describes the
 * common head; ACT_updateNPC and ACT_initSequence witness these later slots. */
typedef struct NearActorSequenceFields {
    u8 unmodeled_000[0x4c8];
    u32 undulation;
    u8 unmodeled_4cc[4];
    short undulation_attr_mask;
    u8 unmodeled_4d2[0x8fc - 0x4d2];
    u32 cleared_on_init_words[2];
} NearActorSequenceFields;

/* ACT_initSequence advances the actor table by 0xa70 at main:0x0030b4d0. */
enum { ACTOR_RECORD_STRIDE = 0xa70 };

#define ACTOR_SECOND_FLAGS_OFFSET 0x6f0
#define ACTOR_FLAG_SEQUENCE_ACTIVE 0x2 /* cleared when no sequence channel runs */
#define ACTOR_FLAG_FOLLOW_GROUND 0x40  /* y follows the undulation height */
#define ACTOR_SHADOW_SIZE_DEFAULT 0x50

/* Walking and running speed of the debug-driven player (units per frame) and
 * the speed above which the run motion is used. */
#define ACTOR_WALK_SPEED 0.06666667f
#define ACTOR_RUN_SPEED 0.13333334f
#define ACTOR_RUN_MOTION_SPEED 0.083333336f
#define STICK_RUN_THRESHOLD 80.0f /* stick length from which the player runs */
#define STICK_MAX 128.0f

/* Direction bits indexing newsToDirection (north, east, west, south). */
#define NEWS_NORTH 0x1
#define NEWS_EAST 0x2
#define NEWS_WEST 0x4
#define NEWS_SOUTH 0x8

/* Addresses a sequence handler may point at: EE code, 4-byte aligned. */
#define EE_CODE_ADDRESS_MIN 0x1f0000

/* nmlModelSetFilter kinds. */
#define NML_FILTER_GUNO 1
#define NML_FILTER_STEALTH 2

#define ROTATION_FACE_X 0x1 /* RotationTrack.face_flags: turn towards the target */
#define ROTATION_FACE_Y 0x2
#define ROTATION_FACE_Z 0x4

/* Undulate.c (main); no header is published for it yet. */
typedef struct PlayerPad {
    u8 unmodeled_00[0x28];
    u16 held;
    u16 pressed;
    u8 unmodeled_2c[0x3a - 0x2c];
    signed char stick_x;
    signed char stick_y;
    u8 unmodeled_3c[0x66 - 0x3c];
    u16 stick_in_use;
} PlayerPad;


/* The 64-entry array itself (see the Actor doc comment above); no accepted
 * function in this TU has needed the symbol until now. */
extern u8 actor[ACTOR_COUNT * 0xa70];

/* The original's own tables and entry points, still asm or data elsewhere. */
extern Actor *GameLoopState[64];

extern float newsToDirection[16];

extern PlayerPad PadData;
extern float invPi;
extern float nearDirPi;
extern float nearDirTwoPi;
extern float piForDoubledWhole;

extern void xglStudioGetCamera(StudioCamera **camera_out, int camera_index);

/* play.c (main); no header is published for it yet. */
extern void *PLAY_getCurrent(void);

/* Defined below in this file. */
extern void SEQ_motion(Actor *actor);

/* Undulate.c (main); no header is published for it yet, and its own
 * UnduWork type is TU-local there, so the destination stays opaque here,
 * exactly as UnduGet2 above already does. */
extern void UnduParamInit(void *undulation);

extern void ACT_updateSequence(Actor *actor);
extern void ACT_updateNPC(Actor *actor);
/* Still original asm elsewhere. */
extern void SEQ_setMotion(Actor *actor);

extern void SEQ_setPositionRAND(Actor *actor);
extern void SEQ_moveNPC_XZ(Actor *actor);
/* Undulate.c (main); no header is published for it yet. */
extern float UnduCheck(const Vector4 *position, void *exclude, void *param);

/* Defined below in this file; forward-declared for the two callers here. */
static StudioCamera *getCurrentCamera(void);

extern void ACT_updateDefault(Actor *actor);
extern void xglVectorMulMat(Vector4 *destination, Matrix4 matrix, const Vector4 *vector);

ActSequenceEntry actSequence[64] = { 0 };

float nearDir(float from, float to)
{
    float difference = to - from;
    float scaled = difference * invPi;
    float magnitude = __builtin_fabsf(scaled);
    int doubled_whole = (int)scaled * 2;

    if (magnitude >= 2.0f) {
        int direction = scaled < 0.0f ? -1 : 1;
        int whole = (int)scaled;

        difference -= (float)whole * nearDirPi;
        if (whole & 1)
            difference += (float)direction * nearDirTwoPi;
    } else {
        difference -= (float)doubled_whole * piForDoubledWhole;
    }

    return difference;
}

void SEQ_scaleSPL(Actor *actor)
{
    /* Same walk as SEQ_rotateSPL below, over the scale track at +0x1b8. */
    u8 *actor_bytes = (u8 *)actor;
    ActSequenceEntry *entry = &actSequence[actor_bytes[ACTOR_SLOT_NUMBER_OFFSET]];
    SequenceState *sequence = &entry->state;
    SplineTrack *track = &entry->scale_channel.spline;
    void *spline = track->spline;
    float sample[3];

    if ((sequence->flags & SEQ_CHANNEL_SCALE_ALL) == 0) {
        sequence->flags |= SEQ_CHANNEL_SCALE_ALL;
        track->frame = 0;
    }

    SPL_getValueXYZ(sample, spline, (float)track->frame);

    actor->scale.x = sample[0];
    actor->scale.y = sample[1];
    actor->scale.z = sample[2];

    track->frame++;
    if (track->last_frame < track->frame)
        sequence->state_flags &= ~SEQ_CHANNEL_SCALE_ALL;
}

void SEQ_scale(Actor *actor)
{
    ActSequenceEntry *sequence = &actSequence[actor->number];
    ScaleTrack *track = &sequence->scale_channel.scale;
    float *start = track->start;
    float *goal = track->goal;
    float *step = track->step;
    Vector4 *scale = &actor->scale;
    float distance[3];

    if ((sequence->state.flags & SEQ_CHANNEL_SCALE_X) == 0) {
        if (sequence->state.state_flags & SEQ_CHANNEL_SCALE_X) {
            start[0] = scale->x;
            if (track->duration[0] > 0)
                step[0] = (goal[0] - start[0]) * (1.0f / track->duration[0]);
            sequence->state.flags |= SEQ_CHANNEL_SCALE_X;
            sequence->frame_counter = 0;
        } else {
            step[0] = 0.0f;
            sequence->state.flags |= SEQ_CHANNEL_SCALE_X;
        }
    }
    if ((sequence->state.flags & SEQ_CHANNEL_SCALE_Y) == 0) {
        if (sequence->state.state_flags & SEQ_CHANNEL_SCALE_Y) {
            start[1] = scale->y;
            if (track->duration[1] > 0)
                step[1] = (goal[1] - start[1]) * (1.0f / track->duration[1]);
            sequence->state.flags |= SEQ_CHANNEL_SCALE_Y;
            sequence->frame_counter = 0;
        } else {
            step[1] = 0.0f;
            sequence->state.flags |= SEQ_CHANNEL_SCALE_Y;
        }
    }
    if ((sequence->state.flags & SEQ_CHANNEL_SCALE_Z) == 0) {
        if (sequence->state.state_flags & SEQ_CHANNEL_SCALE_Z) {
            start[2] = scale->z;
            if (track->duration[2] > 0)
                step[2] = (goal[2] - start[2]) * (1.0f / track->duration[2]);
            sequence->state.flags |= SEQ_CHANNEL_SCALE_Z;
            sequence->frame_counter = 0;
        } else {
            step[2] = 0.0f;
            sequence->state.flags |= SEQ_CHANNEL_SCALE_Z;
        }
    }

    if (sequence->state.state_flags & SEQ_CHANNEL_SCALE_X) {
        distance[0] = __builtin_fabsf(goal[0] - scale->x);
        if (distance[0] <= __builtin_fabsf(step[0])) {
            scale->x = goal[0];
            sequence->state.state_flags &= ~SEQ_CHANNEL_SCALE_X;
        } else {
            scale->x += step[0];
        }
    }
    if (sequence->state.state_flags & SEQ_CHANNEL_SCALE_Y) {
        distance[1] = __builtin_fabsf(goal[1] - scale->y);
        if (distance[1] <= __builtin_fabsf(step[1])) {
            scale->y = goal[1];
            sequence->state.state_flags &= ~SEQ_CHANNEL_SCALE_Y;
        } else {
            scale->y += step[1];
        }
    }
    if (sequence->state.state_flags & SEQ_CHANNEL_SCALE_Z) {
        distance[2] = __builtin_fabsf(goal[2] - scale->z);
        if (distance[2] <= __builtin_fabsf(step[2])) {
            scale->z = goal[2];
            sequence->state.state_flags &= ~SEQ_CHANNEL_SCALE_Z;
        } else {
            scale->z += step[2];
        }
    }
}

void SEQ_rotate(Actor *actor)
{
    Vector4 *rotation = &actor->rotation;
    Vector4 *position = &actor->position;
    ActSequenceEntry *sequence = &actSequence[actor->number];
    RotationTrack *track = &sequence->rotation_channel.rotation;
    float *start = track->start;
    float *goal = track->goal;
    float *step = track->step;
    Actor *target = track->target;
    float distance[3];

    if ((sequence->state.flags & SEQ_CHANNEL_ROTATE_X) == 0) {
        if (sequence->state.state_flags & SEQ_CHANNEL_ROTATE_X) {
            start[0] = rotation->x;
            if (track->face_flags & ROTATION_FACE_X)
                goal[1] = xglAtan2(target->position.y - position->y, target->position.z - position->z);
            if (track->duration[0] > 0) {
                float inverse = 1.0f / track->duration[0];

                step[0] = nearDir(start[0], goal[0]) * inverse;
            }
            sequence->state.flags |= SEQ_CHANNEL_ROTATE_X;
            sequence->frame_counter = 0;
        } else {
            step[0] = 0.0f;
            sequence->state.flags |= SEQ_CHANNEL_ROTATE_X;
        }
    }
    if ((sequence->state.flags & SEQ_CHANNEL_ROTATE_Y) == 0) {
        if (sequence->state.state_flags & SEQ_CHANNEL_ROTATE_Y) {
            start[1] = rotation->y;
            if (track->face_flags & ROTATION_FACE_Y)
                goal[1] = xglAtan2(target->position.x - position->x, target->position.z - position->z);
            if (track->duration[1] > 0) {
                float inverse = 1.0f / track->duration[1];

                step[1] = nearDir(start[1], goal[1]) * inverse;
            }
            sequence->state.flags |= SEQ_CHANNEL_ROTATE_Y;
            sequence->frame_counter = 0;
        } else {
            step[1] = 0.0f;
            sequence->state.flags |= SEQ_CHANNEL_ROTATE_Y;
        }
    }
    if ((sequence->state.flags & SEQ_CHANNEL_ROTATE_Z) == 0) {
        if (sequence->state.state_flags & SEQ_CHANNEL_ROTATE_Z) {
            start[2] = rotation->z;
            if (track->face_flags & ROTATION_FACE_Z)
                goal[1] = xglAtan2(target->position.x - position->x, target->position.y - position->y);
            if (track->duration[2] > 0) {
                float inverse = 1.0f / track->duration[2];

                step[2] = nearDir(start[2], goal[2]) * inverse;
            }
            sequence->state.flags |= SEQ_CHANNEL_ROTATE_Z;
            sequence->frame_counter = 0;
        } else {
            step[2] = 0.0f;
            sequence->state.flags |= SEQ_CHANNEL_ROTATE_Z;
        }
    }

    if (sequence->state.state_flags & SEQ_CHANNEL_ROTATE_X) {
        if (track->face_flags & ROTATION_FACE_X)
            goal[1] = xglAtan2(target->position.y - position->y, target->position.z - position->z);
        distance[0] = __builtin_fabsf(nearDir(goal[0], rotation->x));
        if (distance[0] <= __builtin_fabsf(step[0])) {
            sequence->state.state_flags &= ~SEQ_CHANNEL_ROTATE_X;
            rotation->x = goal[0];
            if ((sequence->state.state_flags & ~SEQ_CHANNEL_MOTION) == 0)
                sequence->state.state_flags = 0;
        } else {
            rotation->x += step[0];
        }
    }
    if (sequence->state.state_flags & SEQ_CHANNEL_ROTATE_Y) {
        if (track->face_flags & ROTATION_FACE_Y)
            goal[1] = xglAtan2(target->position.x - position->x, target->position.z - position->z);
        distance[1] = __builtin_fabsf(nearDir(goal[1], rotation->y));
        if (distance[1] <= __builtin_fabsf(step[1])) {
            sequence->state.state_flags &= ~SEQ_CHANNEL_ROTATE_Y;
            rotation->y = goal[1];
            if ((sequence->state.state_flags & ~SEQ_CHANNEL_MOTION) == 0)
                sequence->state.state_flags = 0;
        } else {
            rotation->y += step[1];
        }
    }
    if (sequence->state.state_flags & SEQ_CHANNEL_ROTATE_Z) {
        if (track->face_flags & ROTATION_FACE_Z)
            goal[1] = xglAtan2(target->position.x - position->x, target->position.y - position->y);
        distance[2] = __builtin_fabsf(nearDir(goal[2], rotation->z));
        if (distance[2] <= __builtin_fabsf(step[2])) {
            sequence->state.state_flags &= ~SEQ_CHANNEL_ROTATE_Z;
            rotation->z = goal[2];
            if ((sequence->state.state_flags & ~SEQ_CHANNEL_MOTION) == 0)
                sequence->state.state_flags = 0;
        } else {
            rotation->z += step[2];
        }
    }
}

void SEQ_rotateSPL(Actor *actor)
{
    /* Byte +0x80 is the actor's own slot in the 64-entry `actor` array, the
     * number ACT_create stores there (0x00305de0) and the one ACT_info prints
     * as "ACT[%02x]"; it selects this actor's 0x260-byte actSequence entry.
     * It is past the recovered head of Actor, so it keeps the byte view. */
    u8 *actor_bytes = (u8 *)actor;
    ActSequenceEntry *entry = &actSequence[actor_bytes[ACTOR_SLOT_NUMBER_OFFSET]];
    SequenceState *sequence = &entry->state;
    /* The sequence entry carries several spline tracks; +0xb8 is the rotation
     * track, as +0x38 is the movement track SEQ_moveSPL below walks and
     * Java_xeno_Chr_move fills in (0x002fd398..0x002fd3e4). */
    SplineTrack *track = &entry->rotation_channel.spline;
    void *spline = track->spline;
    float sample[3];

    if ((sequence->flags & SEQ_CHANNEL_ROTATE_ALL) == 0) {
        sequence->flags |= SEQ_CHANNEL_ROTATE_ALL;
        track->frame = 0;
    }

    SPL_getValueXYZ(sample, spline, (float)track->frame);

    actor->rotation.x = sample[0] / 180.0f * 3.1415927f;
    actor->rotation.y = sample[1] / 180.0f * 3.1415927f;
    actor->rotation.z = sample[2] / 180.0f * 3.1415927f;

    track->frame++;
    if (track->last_frame < track->frame)
        sequence->state_flags &= ~SEQ_CHANNEL_ROTATE_ALL;
}

void SEQ_moveSPL(Actor *actor)
{
    /* +0x80 is this actor's slot number and +0x38 the sequence entry's movement
     * track, the one Java_xeno_Chr_move fills in before it installs this
     * routine (0x002fd398..0x002fd3e4). */
    u8 *actor_bytes = (u8 *)actor;
    ActSequenceEntry *entry = &actSequence[actor_bytes[ACTOR_SLOT_NUMBER_OFFSET]];
    SequenceState *sequence = &entry->state;
    SplineTrack *track = &entry->move_channel.spline;
    void *spline = track->spline;
    Vector4 sample;

    if ((sequence->flags & SEQ_CHANNEL_MOVE) == 0) {
        sequence->flags |= SEQ_CHANNEL_MOVE;
        track->frame = 0;
    }

    SPL_getValueXYZ(&sample.x, spline, (float)track->frame);

    if ((sequence->state_flags & SEQ_CHANNEL_ROTATE_Y) == 0) {
        if (track->flags & 0x10u) {
            float dx = sample.x - actor->position.x;
            float dz = sample.z - actor->position.z;
            actor->rotation.y = xglAtan2(dx, dz);
        }
    }

    if ((sequence->state_flags & SEQ_CHANNEL_MOTION) == 0) {
        /* +0x6f0 is the actor's second flags word: ACT_info prints it with the
         * same "FLAGS %08x" it uses for +0x00 (0x00306374) and ACT_pauseUpdate
         * sets and clears bit 0x80000 in it across all 64 entries
         * (0x00305ff0/0x00306060). It is past the recovered head of Actor. */
        u32 *move_flags = (u32 *)(actor_bytes + ACTOR_SECOND_FLAGS_OFFSET);
        Vector4 delta;
        float length;

        Vu0SubVectorXYZ(&delta, &sample, &actor->position);

        /* The original computes the distance and never reads it: `length` is
         * written by the call and dead afterwards. The call is the observable
         * effect, not its result. */
        xglVectorLength(&length, &delta);

        /* The original really does test the flag before setting it - the branch
         * is in its bytes - so this is not a redundant read-modify-write. */
        if ((*move_flags & 0x8u) == 0)
            *move_flags |= 0x8u;
    }

    actor->position.x = sample.x;
    actor->position.y = sample.y;
    actor->position.z = sample.z;

    track->frame++;
    if (track->last_frame < track->frame)
        sequence->state_flags &= ~SEQ_CHANNEL_MOVE;
}

void SEQ_moveChr(Actor *actor)
{
    Vector4 *rotation = &actor->rotation;
    Vector4 *position = &actor->position;
    ActSequenceEntry *sequence = &actSequence[actor->number];
    MoveTrack *track = &sequence->move_channel.move;
    Vector4 *start = &track->start;
    float *step = &track->target.w;
    Vector4 *target = &track->target_actor->position;
    float yaw;
    Vector4 velocity;
    Vector4 delta;

    if ((sequence->state.flags & SEQ_CHANNEL_MOVE) == 0) {
        sequence->state.flags |= SEQ_CHANNEL_MOVE;
        start->x = position->x;
        start->y = position->y;
        start->z = position->z;
        if (track->frames > 0) {
            float inverse = 1.0f / track->frames;

            velocity.x = (target->x - start->x) * inverse;
            velocity.z = (target->z - start->z) * inverse;
            velocity.y = 0.0f;
            xglVectorLength(step, &velocity);
        }
        xglAtan2(target->x - position->x, target->z - position->z);
        sequence->frame_counter = 0;
    }

    yaw = xglAtan2(target->x - position->x, target->z - position->z);
    if ((sequence->state.state_flags & SEQ_CHANNEL_ROTATE_Y) == 0)
        rotation->y = yaw;

    delta.x = target->x - position->x;
    delta.z = target->z - position->z;
    delta.w = delta.x * delta.x + delta.z * delta.z;
    delta.w = __builtin_sqrtf(delta.w);
    if (delta.w < __builtin_sqrtf((*step + 0.5f) * (*step + 0.5f))) {
        sequence->state.state_flags &= ~SEQ_CHANNEL_MOVE;
        if ((sequence->state.state_flags & ~SEQ_CHANNEL_MOTION) == 0)
            sequence->state.state_flags = 0;
    } else {
        position->x += xglSin(yaw) * *step;
        position->z += xglCos(yaw) * *step;
        if ((sequence->state.state_flags & SEQ_CHANNEL_MOTION) == 0) {
            u32 *move_flags = &actor->animation.slot.flags;

            if ((*move_flags & 0x8) == 0)
                *move_flags |= 0x8;
            if (*step <= ACTOR_RUN_MOTION_SPEED)
                ACT_setMotion(actor, 1);
            else
                ACT_setMotion(actor, 3);
        }
        sequence->frame_counter++;
    }
}

/* The four idle motion ids SEQ_setMotion picks from (data of this TU, to be
 * defined here once every user of its literals is recovered). */
void SEQ_setMotion(Actor *actor)
{
    ActSequenceEntry *entry = &actSequence[actor->number];
    SequenceState *sequence = &entry->state;
    MotionTrack *track = &entry->motion;
    /* The four idle motions; the original names this function-scope table mtnIDLE.0. */
    static u32 mtnIDLE[4] = { 1, 6, 11, 9 };
    unsigned int variant;

    sequence->state_flags |= SEQ_CHANNEL_MOTION;
    variant = xglSRand() & 3;
    track->flags = 1;
    track->loop_start = -1;
    track->motion = mtnIDLE[variant];
    track->speed = 1.0f;
    track->loop_end = -1;
    track->blend = 8;
}

void SEQ_setPositionRAND(Actor *actor)
{
    ActSequenceEntry *sequence = &actSequence[actor->number];
    MoveTrack *track = &sequence->move_channel.move;
    Vector4 *target = &track->target;

    track->target.w = 1.0f / 24.0f;
    sequence->state.state_flags |= SEQ_CHANNEL_MOVE;
    track->frames = -1;
    target->x = (float)(xglSRand() & 7) - 4.0f;
    target->z = (float)(xglSRand() & 7) - 4.0f;
    if (__builtin_fabsf(target->x) < __builtin_fabsf(target->z)) {
        target->x += sequence->anchor.x;
        target->z = actor->position.z;
    } else {
        target->x = actor->position.x;
        target->z += sequence->anchor.z;
    }
}

void SEQ_setRotY2Player(Actor *actor)
{
    ActSequenceEntry *entry = &actSequence[actor->number];
    SequenceState *sequence = &entry->state;
    RotationTrack *track = &entry->rotation_channel.rotation;

    sequence->handler[1] = (void *)SEQ_rotate;
    sequence->state_flags |= SEQ_CHANNEL_ROTATE_Y;
    track->target = GameLoopState[1];
    track->duration[1] = 10;
    track->face_flags |= ROTATION_FACE_Y;
    ACT_setMotion(actor, 7);
}

void SEQ_moveNPC_XZ(Actor *actor)
{
    Vector4 *rotation = &actor->rotation;
    Vector4 *position = &actor->position;
    ActSequenceEntry *sequence = &actSequence[actor->number];
    MoveTrack *track = &sequence->move_channel.move;
    Vector4 *start = &track->start;
    Vector4 *target = &track->target;
    float *step = &track->target.w;
    float yaw;
    Vector4 velocity;
    Vector4 delta;

    if ((sequence->state.flags & SEQ_CHANNEL_MOVE) == 0) {
        sequence->state.flags |= SEQ_CHANNEL_MOVE;
        start->x = position->x;
        start->y = position->y;
        start->z = position->z;
        if (track->frames > 0) {
            float inverse = 1.0f / track->frames;

            velocity.x = (target->x - start->x) * inverse;
            velocity.y = 0.0f;
            velocity.z = (target->z - start->z) * inverse;
            xglVectorLength(step, &velocity);
            xglAtan2(target->x - position->x, target->z - position->z);
        }
        sequence->frame_counter = 0;
    }

    yaw = xglAtan2(target->x - position->x, target->z - position->z);
    if ((sequence->state.state_flags & SEQ_CHANNEL_ROTATE_Y) == 0)
        rotation->y += nearDir(rotation->y, yaw) * 0.1f;
    if ((sequence->state.state_flags & SEQ_CHANNEL_MOTION) == 0) {
        u32 *move_flags = &actor->animation.slot.flags;

        if ((*move_flags & 0x8) == 0)
            *move_flags |= 0x8;
        if (*step <= ACTOR_RUN_MOTION_SPEED)
            ACT_setMotion(actor, 2);
        else
            ACT_setMotion(actor, 4);
    }

    delta.x = target->x - position->x;
    delta.z = target->z - position->z;
    delta.w = delta.x * delta.x + delta.z * delta.z;
    delta.w = __builtin_sqrtf(delta.w);
    if (delta.w <= __builtin_sqrtf(*step * *step)) {
        sequence->state.state_flags &= ~SEQ_CHANNEL_MOVE;
        if ((sequence->state.state_flags & ~SEQ_CHANNEL_MOTION) == 0)
            sequence->state.state_flags = 0;
    }
    position->x += xglSin(yaw) * *step;
    position->z += xglCos(yaw) * *step;
    sequence->frame_counter++;
}

void SEQ_moveXZ(Actor *actor)
{
    Vector4 *rotation = &actor->rotation;
    Vector4 *position = &actor->position;
    ActSequenceEntry *sequence = &actSequence[actor->number];
    MoveTrack *track = &sequence->move_channel.move;
    Vector4 *start = &track->start;
    Vector4 *target = &track->target;
    float *step = &track->target.w;
    float yaw;
    Vector4 velocity;
    Vector4 delta;

    if ((sequence->state.flags & SEQ_CHANNEL_MOVE) == 0) {
        sequence->state.flags |= SEQ_CHANNEL_MOVE;
        start->x = position->x;
        start->y = position->y;
        start->z = position->z;
        if (track->frames > 0) {
            float inverse = 1.0f / track->frames;

            velocity.x = (target->x - start->x) * inverse;
            velocity.y = 0.0f;
            velocity.z = (target->z - start->z) * inverse;
            xglVectorLength(step, &velocity);
            xglAtan2(target->x - position->x, target->z - position->z);
        }
        sequence->frame_counter = 0;
    }

    yaw = xglAtan2(target->x - position->x, target->z - position->z);
    if ((sequence->state.state_flags & SEQ_CHANNEL_ROTATE_Y) == 0)
        rotation->y += nearDir(rotation->y, yaw) * 0.1f;
    if ((sequence->state.state_flags & SEQ_CHANNEL_MOTION) == 0) {
        u32 *move_flags = &actor->animation.slot.flags;

        if ((*move_flags & 0x8) == 0)
            *move_flags |= 0x8;
    }

    delta.x = target->x - position->x;
    delta.z = target->z - position->z;
    delta.w = delta.x * delta.x + delta.z * delta.z;
    delta.w = __builtin_sqrtf(delta.w);
    if (delta.w <= __builtin_sqrtf(*step * *step)) {
        sequence->state.state_flags &= ~SEQ_CHANNEL_MOVE;
        if ((sequence->state.state_flags & ~SEQ_CHANNEL_MOTION) == 0)
            sequence->state.state_flags = 0;
    }
    position->x += xglSin(yaw) * *step;
    position->z += xglCos(yaw) * *step;
    sequence->frame_counter++;
}

void SEQ_motion(Actor *actor)
{
    ActSequenceEntry *sequence = &actSequence[actor->number];
    MotionTrack *track = &sequence->motion;
    ActorAnimationState *animation = &actor->animation;
    float seconds_per_frame;
    float blend;
    float speed;

    if ((sequence->state.flags & SEQ_CHANNEL_MOTION) == 0) {
        seconds_per_frame = 0.033333335f;
        sequence->state.flags |= SEQ_CHANNEL_MOTION;
        animation->slot.flags |= 0x1;
        blend = track->blend * seconds_per_frame;
        animation->slot.flags &= ~0x1;
        track->flags |= 0x20000000;
        animation->blendDuration = blend;
        animation->slot.speed = track->speed * seconds_per_frame;
        ACT_setMotion2(actor, track->motion, track->flags);
        if (track->loop_start >= 0) {
            float loop_end = track->loop_end * seconds_per_frame;
            float loop_start = track->loop_start * seconds_per_frame;

            animation->slot.reverseLimit = loop_start;
            animation->slot.forwardLimit = loop_end;
            if (animation->slot.speed < 0.0f)
                animation->slot.frame = loop_end;
            else
                animation->slot.frame = loop_start;
        }
    }
    speed = animation->slot.speed;

    if ((sequence->state.state_flags & ~SEQ_CHANNEL_MOTION) == 0) {
        if (animation->slot.flags & 0x1000)
            sequence->state.state_flags = 0;
    }
    track->time += speed;
}

void ACT_updateSequence(Actor *actor)
{
    ActSequenceEntry *sequence = &actSequence[actor->number];
    void **handler = sequence->state.handler;
    int remaining = SEQ_HANDLER_COUNT - 1;

    do {
        unsigned int address = (unsigned int)*handler++;

        if (address >= EE_CODE_ADDRESS_MIN && address < EE_RAM_END && (address & 0x3) == 0)
            ((void (*)(Actor *))address)(actor);
    } while (--remaining >= 0);

    if (sequence->state.flags & SEQ_USE_CHANNEL_MASK) {
        unsigned int state = sequence->state.state_flags;

        if ((state & SEQ_CHANNEL_MOVE) == 0)
            sequence->state.handler[0] = 0;
        if ((state & SEQ_CHANNEL_ROTATE_ALL) == 0)
            sequence->state.handler[1] = 0;
        if ((state & SEQ_CHANNEL_MOTION) == 0)
            sequence->state.handler[2] = 0;
        if ((state & sequence->state.channel_mask) == 0)
            actor->flags &= ~ACTOR_FLAG_SEQUENCE_ACTIVE;
    } else if (sequence->state.state_flags == 0) {
        sequence->state.handler[3] = 0;
        actor->flags &= ~ACTOR_FLAG_SEQUENCE_ACTIVE;
        sequence->state.handler[0] = 0;
        sequence->state.handler[1] = 0;
        sequence->state.handler[2] = 0;
        sequence->state.flags = 0;
    }

    if (actor->flags & ACTOR_FLAG_FOLLOW_GROUND) {
        float height = UnduGet(actor->position.x, actor->position.z);

        if (height != GROUND_HEIGHT_NONE)
            actor->position.y = height;
    }
    ACT_updateMotion(actor);
}

void ACT_updateNPC(Actor *actor)
{
    ActSequenceEntry *sequence = &actSequence[actor->number];
    int remaining;
    Vector4 moved;
    float value;
    float expected;
    float x;
    float z;

    moved.x = actor->position.x;
    moved.y = actor->position.y;
    moved.z = actor->position.z;

    for (remaining = 0; remaining < SEQ_HANDLER_COUNT; remaining++) {
        unsigned int address = (unsigned int)sequence->state.handler[remaining];

        if (address >= EE_CODE_ADDRESS_MIN && address < EE_RAM_END && (address & 0x3) == 0)
            ((void (*)(Actor *))address)(actor);
    }

    if (sequence->state.state_flags == 0) {
        sequence->state.handler[0] = 0;
        sequence->state.handler[1] = 0;
        actor->flags &= ~ACTOR_FLAG_SEQUENCE_ACTIVE;
        sequence->state.handler[2] = 0;
        sequence->state.flags = 0;
        sequence->state.handler[3] = 0;
        if (sequence->idle_roll == 0) {
            sequence->state.handler[2] = (void *)SEQ_motion;
            SEQ_setMotion(actor);
        } else {
            SEQ_setPositionRAND(actor);
            sequence->state.handler[0] = (void *)SEQ_moveNPC_XZ;
        }
        sequence->idle_roll = xglSRand() & 0x3;
    }

    actor->velocity.x = actor->position.x - moved.x;
    actor->velocity.y = 0.0f;
    actor->velocity.z = actor->position.z - moved.z;
    x = actor->position.x;
    z = actor->position.z;
    actor->position.x = moved.x;
    actor->position.z = moved.z;
    moved.x = x;
    moved.z = z;
    {
        NearActorSequenceFields *sequence_fields = (void *)actor;

        sequence_fields->undulation_attr_mask =
            (sequence_fields->undulation_attr_mask & ~0x7) | 0x2;
    }
    value = UnduCheck(&actor->position, &actor->velocity, &actor->undulation);
    if (value != GROUND_HEIGHT_NONE)
        actor->position.y = value;
    xglVectorLength(&value, &actor->velocity);
    if (value > 0.0f) {
        xglVectorLength(&expected, &moved);
        xglVectorLength(&value, &actor->position);
        if (expected - value != 0.0f) {
            if (sequence->state.state_flags & SEQ_CHANNEL_MOVE)
                sequence->state.state_flags &= ~SEQ_CHANNEL_MOVE;
        }
    }
    ACT_updateMotion(actor);
}

void ACT_updateDefault(Actor *actor)
{
    if (actor->flags & ACTOR_FLAG_FOLLOW_GROUND)
        actor->position.y = UnduGet(actor->position.x, actor->position.z);
    ACT_updateMotion(actor);
}

void ACT_updateRECTRand(Actor *actor)
{
    ACT_updateMotion(actor);
}

void ACT_updatePlayer(Actor *actor)
{
    PlayerPad *pad = &PadData;
    Vector4 *position = &actor->position;
    int moved = 0;
    StudioCamera *camera = getCurrentCamera();
    Vector4 stick;
    Vector4 direction;
    Matrix4 matrix;
    float speed;

    if (pad->held & PAD_SELECT) {
        if ((u16)(pad->pressed & PAD_L2)) {
            position->x = 0.0f;
            position->z = 0.0f;
        }
        if (pad->pressed & PAD_L1)
            actor->state84 = (actor->state84 + 1) & 0x1;
    }
    if (pad->pressed & PAD_R1)
        position->y += 0.5f;
    if (pad->pressed & PAD_R2)
        position->y -= 0.25f;

    if (pad->stick_in_use) {
        stick.x = pad->stick_x;
        stick.y = 0.0f;
        stick.z = pad->stick_y;
        stick.w = 1.0f;
        moved = 1;
        xglVectorLength(&speed, &stick);
        if (!(speed < STICK_RUN_THRESHOLD))
            speed = ACTOR_RUN_SPEED;
        else
            speed = ACTOR_WALK_SPEED;
    } else if (pad->held & (PAD_UP | PAD_RIGHT | PAD_DOWN | PAD_LEFT)) {
        int direction_bits = 0;
        float heading;

        if (pad->held & PAD_UP)
            direction_bits = NEWS_NORTH;
        if (pad->held & PAD_RIGHT)
            direction_bits |= NEWS_EAST;
        if (pad->held & PAD_LEFT)
            direction_bits |= NEWS_WEST;
        if (pad->held & PAD_DOWN)
            direction_bits |= NEWS_SOUTH;
        moved = 1;
        if (pad->held & PAD_CROSS)
            speed = ACTOR_RUN_SPEED;
        else
            speed = ACTOR_WALK_SPEED;
        heading = newsToDirection[direction_bits];
        stick.x = xglSin(heading) * STICK_MAX;
        stick.y = 0.0f;
        stick.z = xglCos(heading) * STICK_MAX;
        stick.w = 1.0f;
    }

    if (moved) {
        float yaw;
        float difference;
        float turn;

        /* sceVu0CopyMatrix shape: the camera basis is copied as four quadwords
         * through a fixed scratch register. */
        QuadwordCopyMatrix(&matrix, &camera->cameraToWorldMatrix);
        matrix[3][0] = 0.0f;
        matrix[3][1] = 0.0f;
        matrix[3][2] = 0.0f;
        xglVectorMulMat(&direction, matrix, &stick);
        yaw = xglAtan2(direction.x, direction.z);
        difference = nearDir(actor->rotation.y, yaw);
        if (0.87266469f < __builtin_fabsf(difference))
            turn = difference * 0.25f;
        else
            turn = difference * 0.125f;
        actor->rotation.y += turn;
        if (__builtin_fabsf(difference) < 0.174532935f) {
            position->x += xglSin(actor->rotation.y) * speed;
            position->z += xglCos(actor->rotation.y) * speed;
        } else {
            position->x += xglSin(yaw) * speed;
            position->z += xglCos(yaw) * speed;
        }
        if (!(ACTOR_RUN_MOTION_SPEED < speed))
            ACT_setMotion(actor, 1);
        else
            ACT_setMotion(actor, 3);
    } else {
        ACT_setMotion(actor, 0);
    }

    if (__builtin_fabsf(actor->rotation.y * 0.31830987f) >= 2.0f)
        actor->rotation.y -= (int)(actor->rotation.y * 0.31830987f) * 3.1415927f;
    if (actor->state84 == 0)
        actor->position.y = UnduGet2(&actor->undulation, actor->position.x, actor->position.z);
    ACT_updateMotion(actor);
}

void ACT_initSequenceAt(Actor *actor)
{
    SequenceState *sequence =
        &actSequence[actor->number].state;

    sequence->flags = 0;
    sequence->state_flags = 0;
    sequence->cleared_on_init = 0;
    sequence->cleared_on_init_run[0] = 0;
    sequence->cleared_on_init_run[1] = 0;
    sequence->cleared_on_init_run[2] = 0;
    sequence->cleared_on_init_run[3] = 0;
    sequence->handler[0] = 0;
    sequence->handler[1] = 0;
    sequence->handler[2] = 0;
    sequence->handler[3] = 0;
}

void ACT_initSequence(void)
{
    ActSequenceEntry *sequence;
    Actor *entry;
    int i;

    entry = (Actor *)actor;
    sequence = actSequence;
    for (i = 0; i < ACTOR_COUNT; i++) {
        sequence->state.flags = 0;
        sequence->state.state_flags = 0;
        sequence->state.cleared_on_init = 0;
        sequence->state.cleared_on_init_run[0] = 0;
        sequence->state.cleared_on_init_run[1] = 0;
        sequence->state.cleared_on_init_run[2] = 0;
        sequence->state.cleared_on_init_run[3] = 0;
        sequence->state.handler[0] = 0;
        sequence->state.handler[1] = 0;
        sequence->state.handler[2] = 0;
        sequence->state.handler[3] = 0;
        sequence++;
    }

    for (i = 0; i < ACTOR_COUNT; i++) {
        NearActorSequenceFields *sequence_fields = (void *)entry;

        entry->java_object = 0;
        entry->flags = ACTOR_FLAGS_INITIAL;
        entry->signal = 0;
        entry->frame_counter = 0;
        entry->update = ACT_updateDefault;
        entry->draw = 0;
        sequence_fields->cleared_on_init_words[1] = 0;
        sequence_fields->cleared_on_init_words[0] = 0;
        UnduParamInit(&entry->undulation);
        entry->shadow_kind = 1;
        entry->shadowSize = ACTOR_SHADOW_SIZE_DEFAULT;
        /* ACT_initSequence's original array stride is 0xa70. Actor is a
         * partial shared prefix and its C sizeof does not express that stride. */
        entry = (Actor *)((u8 *)entry + ACTOR_RECORD_STRIDE);
    }
}

void ACT_initVMObject(Actor *entry)
{
    int i;

    if (entry == 0) {
        entry = (Actor *)actor;
        for (i = 0; i < ACTOR_COUNT; i++) {
            entry->flags = ACTOR_FLAGS_INITIAL;
            entry->shadow_kind = 1;
            entry->shadowSize = ACTOR_SHADOW_SIZE_DEFAULT;
            entry->java_object = 0;
        }
    } else {
        entry->java_object = 0;
    }
}

Actor *ACT_createChr(int type, int id)
{
    Actor *actor = ACT_create(type, id);

    if (actor != 0) {
        SequenceState *sequence = &actSequence[actor->number].state;

        actor->shadow_kind = 1;
        actor->signal = 0;
        actor->java_object = 0;
        actor->flags = ACTOR_FLAGS_INITIAL;
        actor->update = ACT_updateSequence;
        actor->frame_counter = 0;
        actor->scale.x = 1.0f;
        actor->scale.y = 1.0f;
        actor->scale.z = 1.0f;
        actor->scale.w = 1.0f;
        actor->shadowSize = ACTOR_SHADOW_SIZE_DEFAULT;
        sequence->flags = 0;
        sequence->state_flags = 0;
        sequence->cleared_on_init = 0;
        sequence->cleared_on_init_run[0] = 0;
        sequence->cleared_on_init_run[1] = 0;
        sequence->cleared_on_init_run[2] = 0;
        sequence->cleared_on_init_run[3] = 0;
        sequence->handler[0] = 0;
        sequence->handler[1] = 0;
        sequence->handler[2] = 0;
        sequence->handler[3] = 0;
    }
    return actor;
}

Actor *ACT_createNPC(int type, int id)
{
    Actor *actor = ACT_create(type, id);

    if (actor != 0) {
        SequenceState *sequence;

        UnduParamInit(&actor->undulation);
        actor->update = ACT_updateNPC;
        actor->signal = 0;
        actor->frame_counter = 0;
        actor->java_object = 0;
        sequence = &actSequence[actor->number].state;
        sequence->flags = 0;
        sequence->state_flags = 0;
        sequence->cleared_on_init = 0;
        sequence->cleared_on_init_run[0] = 0;
        sequence->cleared_on_init_run[1] = 0;
        sequence->cleared_on_init_run[2] = 0;
        sequence->cleared_on_init_run[3] = 0;
        sequence->handler[0] = 0;
        sequence->handler[1] = 0;
        sequence->handler[2] = 0;
        sequence->handler[3] = 0;
    }
    return actor;
}

void ACT_info(void)
{
    int i;

    for (i = 0; i < ACTOR_COUNT; i++) {
    }
}

static StudioCamera *getCurrentCamera(void)
{
    StudioCamera *camera;
    int camera_index;

    for (camera_index = 0; camera_index < STUDIO_CAMERA_COUNT; camera_index++) {
        xglStudioGetCamera(&camera, camera_index);
        if (camera->active)
            return camera;
    }

    return 0;
}

void ACT_updateMPack(Actor *actor)
{
    Play *play = PLAY_getCurrent();
    Actor *linked_actor;

    xglMatrixStackUnit();
    SEQ_motion(actor);
    ACT_updateMotion(actor);

    /* PLAY_setupDefault and PLAY_ctrl establish currentTime at +0x44. */
    actor->animation.slot.frame = play->currentTime;
    /* A u32 access: the union member form reorders this store (tucheck, 2026-11-01). */
    *(u32 *)&actor->undulation = 0;

    linked_actor = actor->jointMatrices;
    UnduGet2(&actor->undulation, linked_actor->velocity.x,
             linked_actor->velocity.z);
}

void ACT_initScene(void)
{
    Actor *entry;
    int remaining;

    entry = (Actor *)actor;
    remaining = ACTOR_COUNT - 1;
    do {
        remaining--;
        entry->flags = ACTOR_FLAGS_INITIAL;
        entry->shadow_kind = 1;
        entry->shadowSize = ACTOR_SHADOW_SIZE_DEFAULT;
        entry->state84 = 0;
        entry->inUseId = 0;
        entry->update = 0;
        entry->draw = 0;
        entry->position.w = 1.0f;
        entry->velocity.w = 0.0f;
        entry->acceleration.w = 0.0f;
        UnduParamInit(&entry->undulation);
        entry = (Actor *)((u8 *)entry + ACTOR_RECORD_STRIDE);
    } while (remaining >= 0);
}

void ACT_setHand(Actor *actor, int hand)
{
    int category;
    int code;

    if (actor->inUseId & 0xf000) {
        return;
    }

    category = hand & 0xf0;
    code = hand & 0xf;
    switch (category) {
    case 0x00:
        code |= code << 8;
        break;
    case 0x10:
        code |= 0x8000;
        break;
    case 0x20:
        code = (code << 8) | 0x80;
        break;
    }
    ACT_setHumanHand(actor, code);
}

void ACT_filterGuno(Actor *actor)
{
    nmlModelSetToumei(1);
    nmlModelSetZwrite(1);
    nmlModelSetStencil(1);
    nmlModelSetFilter(NML_FILTER_GUNO,actor->drawEnv.filter_param[2], actor->drawEnv.filter_param[1]);
    nmlModelSetTransparency(actor->drawEnv.transparency);
    nmlModelSetReflTransparency(actor->drawEnv.filter_param[0]);
}

void ACT_filterStealth(Actor *actor)
{
    nmlModelSetToumei(1);
    nmlModelSetZwrite(1);
    nmlModelSetStencil(1);
    nmlModelSetFilter(NML_FILTER_STEALTH,actor->drawEnv.filter_param[2], actor->drawEnv.filter_param[1]);
    nmlModelSetTransparency(actor->drawEnv.transparency);
    nmlModelSetReflTransparency(actor->drawEnv.filter_param[0]);
}

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
float invPi = 0.31830987334251404f;

float nearDirPi = 3.1415927410125732f;

float nearDirTwoPi = 6.2831854820251465f;

float piForDoubledWhole = 3.1415927410125732f;
