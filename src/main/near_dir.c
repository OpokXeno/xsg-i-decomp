#include "common.h"

#include "near_dir.h"

#include "main/play.h"

ActSequenceEntry actSequence[64] = { 0 };

#include "main/xgl_studio.h"

typedef struct {
    u32 active;
} StudioCameraActivePrefix;

extern void xglStudioGetCamera(StudioCamera **camera_out, int camera_index);

/* Defined below in this file; forward-declared for the two callers here. */

extern void ACT_updateMotion(Actor *actor);

/* Undulate.c (main); no header is published for it yet. */

extern float UnduGet(float x, float z);

/* play.c (main); no header is published for it yet. */

extern void *PLAY_getCurrent(void);

/* Defined below in this file. */

extern void SEQ_motion(Actor *actor);

/* Undulate.c (main); no header is published for it yet. */

extern float UnduGet2(void *destination, float x, float z);

/* include/main/xgl_2.h's own declaration: not included directly because it
 * (through include/shared.h) redefines struct Vector4, which this TU's own
 * near_dir.h already defines locally. */

extern void xglMatrixStackUnit(void);

/* Undulate.c (main); no header is published for it yet, and its own
 * UnduWork type is TU-local there, so the destination stays opaque here,
 * exactly as UnduGet2 above already does. */

extern void UnduParamInit(void *undulation);

/* The 64-entry array itself (see the Actor doc comment above); no accepted
 * function in this TU has needed the symbol until now. */

extern u8 actor[64 * 0xa70];

/* Still original asm (0x003083b8, main); ACT_setHand's own call to it is a
 * genuine tail call (j, not jal), so its return value, if any, is never
 * observed here. */

extern void ACT_setHumanHand(Actor *actor, int hand);

/* nml_model_set.c (main); no header is published for it yet. */

extern void nmlModelSetToumei(int enabled);

extern void nmlModelSetZwrite(int enabled);

extern void nmlModelSetStencil(int enabled);

extern void nmlModelSetFilter(int mode, float filter_param_1, float filter_param_2);

extern void nmlModelSetTransparency(float transparency);

extern void nmlModelSetReflTransparency(float transparency);

extern void ACT_updateSequence(Actor *actor);

extern void ACT_updateNPC(Actor *actor);

typedef struct MoveSplineTrack {
    void *spline;
    int frames;
    u8 unmodeled_08[0x10 - 0x08];
    Vector4 start;
    u8 unmodeled_20[0x2c - 0x20];
    float step;
} MoveSplineTrack;

typedef struct MovingSequence {
    u32 flags;
    u32 state_flags;
    u32 active_mask;
    u32 cleared_on_init;
    u32 unmodeled_10;
    u32 cleared_on_init_run[4];
    void *handler[4];
    u32 frame_counter;
    MoveSplineTrack move;
    u8 unmodeled_68[0x238 - 0x68];
    u32 idle_roll;
    u8 unmodeled_23c[0x240 - 0x23c];
    Vector4 anchor;
} MovingSequence;

extern float nearDir(float from, float to);

extern void ACT_setMotion(Actor *actor, unsigned int motion);

typedef struct RotationTrack {
    Actor *target;
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

/* The motion track of a sequence entry (+0x138): what SEQ_motion plays and
 * how it hands the values to the actor. */

/* MotionTrack is defined by this TU's owning near_dir.h. */

/* Additional sequence-owned Actor slots. The canonical Actor describes the
 * common head; ACT_updateNPC and ACT_initSequence witness these later slots. */
typedef struct NearActorSequenceFields {
    u8 unmodeled_000[0x4c0];
    void *java_object_ref;
    u8 unmodeled_4c4[4];
    u32 undulation;
    u8 unmodeled_4cc[4];
    short undulation_attr_mask;
    u8 unmodeled_4d2[0x8fc - 0x4d2];
    u32 cleared_on_init_words[2];
} NearActorSequenceFields;

/* ACT_initSequence advances the actor table by 0xa70 at main:0x0030b4d0. */
enum { ACTOR_RECORD_STRIDE = 0xa70 };

/* The four idle motion ids SEQ_setMotion picks from (data of this TU, to be
 * defined here once every user of its literals is recovered). */

extern unsigned short xglSRand(void);

/* The original's own tables and entry points, still asm or data elsewhere. */

extern Actor *GameLoopState[64];

/* Still original asm elsewhere. */

extern void SEQ_setMotion(Actor *actor);

extern void SEQ_setPositionRAND(Actor *actor);

extern void SEQ_moveNPC_XZ(Actor *actor);

extern float UnduCheck(const Vector4 *position, void *exclude, void *param);

/* Defined below in this file; forward-declared for the two callers here. */

/* Undulate.c (main); no header is published for it yet. */

static StudioCamera *getCurrentCamera(void);

extern void ACT_updateDefault(Actor *actor);

extern Actor *ACT_create(int type, int id);

/* play.c (main); no header is published for it yet. */

/* Defined below in this file. */

/* Undulate.c (main); no header is published for it yet. */

/* include/main/xgl_2.h's own declaration: not included directly because it
 * (through include/shared.h) redefines struct Vector4, which this TU's own
 * near_dir.h already defines locally. */

/* Undulate.c (main); no header is published for it yet, and its own
 * UnduWork type is TU-local there, so the destination stays opaque here,
 * exactly as UnduGet2 above already does. */

/* The 64-entry array itself (see the Actor doc comment above); no accepted
 * function in this TU has needed the symbol until now. */

/* Still original asm (0x003083b8, main); ACT_setHand's own call to it is a
 * genuine tail call (j, not jal), so its return value, if any, is never
 * observed here. */

/* nml_model_set.c (main); no header is published for it yet. */

extern float newsToDirection[16];

extern float xglSin(float angle);

extern float xglCos(float angle);

extern void ACT_setMotion2(Actor *actor, unsigned int motion, unsigned int flags);

/* Defined below in this file; forward-declared for the two callers here. */

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

extern PlayerPad PadData;

typedef struct CameraBasis {
    u8 unmodeled_00[0x170];
    Matrix4 matrix;
} CameraBasis;

extern void xglVectorMulMat(Vector4 *destination, Matrix4 matrix, const Vector4 *vector);

INCLUDE_ASM("asm/main/nonmatchings/near_dir", nearDir);

void SEQ_scaleSPL(Actor *actor)
{
    /* Same walk as SEQ_rotateSPL below, over the scale track at +0x1b8. */
    u8 *actor_bytes = (u8 *)actor;
    SequenceState *sequence =
        &actSequence[actor_bytes[0x80]].state;
    SplineTrack *track = (SplineTrack *)((u8 *)sequence + 0x1b8);
    void *spline = track->spline;
    float sample[3];

    if ((sequence->flags & 0xe0u) == 0) {
        sequence->flags |= 0xe0u;
        track->frame = 0;
    }

    SPL_getValueXYZ(sample, spline, (float)track->frame);

    actor->scale.x = sample[0];
    actor->scale.y = sample[1];
    actor->scale.z = sample[2];

    track->frame++;
    if (track->last_frame < (short)track->frame)
        sequence->state_flags &= ~0xe0u;
}

void SEQ_scale(Actor *actor)
{
    MovingSequence *sequence = (MovingSequence *)&actSequence[actor->number];
    ScaleTrack *track = (ScaleTrack *)((u8 *)sequence + 0x1b8);
    float *start = track->start;
    float *goal = track->goal;
    float *step = track->step;
    Vector4 *scale = &actor->scale;
    float distance[3];

    if ((sequence->flags & 0x20) == 0) {
        if (sequence->state_flags & 0x20) {
            start[0] = scale->x;
            if (track->duration[0] > 0)
                step[0] = (goal[0] - start[0]) * (1.0f / track->duration[0]);
            sequence->flags |= 0x20;
            sequence->frame_counter = 0;
        } else {
            step[0] = 0.0f;
            sequence->flags |= 0x20;
        }
    }
    if ((sequence->flags & 0x40) == 0) {
        if (sequence->state_flags & 0x40) {
            start[1] = scale->y;
            if (track->duration[1] > 0)
                step[1] = (goal[1] - start[1]) * (1.0f / track->duration[1]);
            sequence->flags |= 0x40;
            sequence->frame_counter = 0;
        } else {
            step[1] = 0.0f;
            sequence->flags |= 0x40;
        }
    }
    if ((sequence->flags & 0x80) == 0) {
        if (sequence->state_flags & 0x80) {
            start[2] = scale->z;
            if (track->duration[2] > 0)
                step[2] = (goal[2] - start[2]) * (1.0f / track->duration[2]);
            sequence->flags |= 0x80;
            sequence->frame_counter = 0;
        } else {
            step[2] = 0.0f;
            sequence->flags |= 0x80;
        }
    }

    if (sequence->state_flags & 0x20) {
        distance[0] = __builtin_fabsf(goal[0] - scale->x);
        if (distance[0] <= __builtin_fabsf(step[0])) {
            scale->x = goal[0];
            sequence->state_flags &= ~0x20;
        } else {
            scale->x += step[0];
        }
    }
    if (sequence->state_flags & 0x40) {
        distance[1] = __builtin_fabsf(goal[1] - scale->y);
        if (distance[1] <= __builtin_fabsf(step[1])) {
            scale->y = goal[1];
            sequence->state_flags &= ~0x40;
        } else {
            scale->y += step[1];
        }
    }
    if (sequence->state_flags & 0x80) {
        distance[2] = __builtin_fabsf(goal[2] - scale->z);
        if (distance[2] <= __builtin_fabsf(step[2])) {
            scale->z = goal[2];
            sequence->state_flags &= ~0x80;
        } else {
            scale->z += step[2];
        }
    }
}

void SEQ_rotate(Actor *actor)
{
    Vector4 *rotation = &actor->rotation;
    Vector4 *position = &actor->position;
    MovingSequence *sequence = (MovingSequence *)&actSequence[actor->number];
    RotationTrack *track = (RotationTrack *)((u8 *)sequence + 0xb8);
    float *start = track->start;
    float *goal = track->goal;
    float *step = track->step;
    Actor *target = track->target;
    float distance[3];

    if ((sequence->flags & 0x2) == 0) {
        if (sequence->state_flags & 0x2) {
            start[0] = rotation->x;
            if (track->face_flags & 0x1)
                goal[1] = xglAtan2(target->position.y - position->y, target->position.z - position->z);
            if (track->duration[0] > 0) {
                float inverse = 1.0f / track->duration[0];

                step[0] = nearDir(start[0], goal[0]) * inverse;
            }
            sequence->flags |= 0x2;
            sequence->frame_counter = 0;
        } else {
            step[0] = 0.0f;
            sequence->flags |= 0x2;
        }
    }
    if ((sequence->flags & 0x4) == 0) {
        if (sequence->state_flags & 0x4) {
            start[1] = rotation->y;
            if (track->face_flags & 0x2)
                goal[1] = xglAtan2(target->position.x - position->x, target->position.z - position->z);
            if (track->duration[1] > 0) {
                float inverse = 1.0f / track->duration[1];

                step[1] = nearDir(start[1], goal[1]) * inverse;
            }
            sequence->flags |= 0x4;
            sequence->frame_counter = 0;
        } else {
            step[1] = 0.0f;
            sequence->flags |= 0x4;
        }
    }
    if ((sequence->flags & 0x8) == 0) {
        if (sequence->state_flags & 0x8) {
            start[2] = rotation->z;
            if (track->face_flags & 0x4)
                goal[1] = xglAtan2(target->position.x - position->x, target->position.y - position->y);
            if (track->duration[2] > 0) {
                float inverse = 1.0f / track->duration[2];

                step[2] = nearDir(start[2], goal[2]) * inverse;
            }
            sequence->flags |= 0x8;
            sequence->frame_counter = 0;
        } else {
            step[2] = 0.0f;
            sequence->flags |= 0x8;
        }
    }

    if (sequence->state_flags & 0x2) {
        if (track->face_flags & 0x1)
            goal[1] = xglAtan2(target->position.y - position->y, target->position.z - position->z);
        distance[0] = __builtin_fabsf(nearDir(goal[0], rotation->x));
        if (distance[0] <= __builtin_fabsf(step[0])) {
            sequence->state_flags &= ~0x2;
            rotation->x = goal[0];
            if ((sequence->state_flags & ~0x10) == 0)
                sequence->state_flags = 0;
        } else {
            rotation->x += step[0];
        }
    }
    if (sequence->state_flags & 0x4) {
        if (track->face_flags & 0x2)
            goal[1] = xglAtan2(target->position.x - position->x, target->position.z - position->z);
        distance[1] = __builtin_fabsf(nearDir(goal[1], rotation->y));
        if (distance[1] <= __builtin_fabsf(step[1])) {
            sequence->state_flags &= ~0x4;
            rotation->y = goal[1];
            if ((sequence->state_flags & ~0x10) == 0)
                sequence->state_flags = 0;
        } else {
            rotation->y += step[1];
        }
    }
    if (sequence->state_flags & 0x8) {
        if (track->face_flags & 0x4)
            goal[1] = xglAtan2(target->position.x - position->x, target->position.y - position->y);
        distance[2] = __builtin_fabsf(nearDir(goal[2], rotation->z));
        if (distance[2] <= __builtin_fabsf(step[2])) {
            sequence->state_flags &= ~0x8;
            rotation->z = goal[2];
            if ((sequence->state_flags & ~0x10) == 0)
                sequence->state_flags = 0;
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
    SequenceState *sequence =
        &actSequence[actor_bytes[0x80]].state;
    /* The sequence entry carries several spline tracks; +0xb8 is the rotation
     * track, as +0x38 is the movement track SEQ_moveSPL below walks and
     * Java_xeno_Chr_move fills in (0x002fd398..0x002fd3e4). */
    SplineTrack *track = (SplineTrack *)((u8 *)sequence + 0xb8);
    void *spline = track->spline;
    float sample[3];

    if ((sequence->flags & 0xeu) == 0) {
        sequence->flags |= 0xeu;
        track->frame = 0;
    }

    SPL_getValueXYZ(sample, spline, (float)track->frame);

    actor->rotation.x = sample[0] / 180.0f * 3.1415927f;
    actor->rotation.y = sample[1] / 180.0f * 3.1415927f;
    actor->rotation.z = sample[2] / 180.0f * 3.1415927f;

    track->frame++;
    if (track->last_frame < (short)track->frame)
        sequence->state_flags &= ~0xeu;
}

void SEQ_moveSPL(Actor *actor)
{
    /* +0x80 is this actor's slot number and +0x38 the sequence entry's movement
     * track, the one Java_xeno_Chr_move fills in before it installs this
     * routine (0x002fd398..0x002fd3e4). */
    u8 *actor_bytes = (u8 *)actor;
    SequenceState *sequence =
        &actSequence[actor_bytes[0x80]].state;
    SplineTrack *track = (SplineTrack *)((u8 *)sequence + 0x38);
    void *spline = track->spline;
    Vector4 sample;

    if ((sequence->flags & 0x1u) == 0) {
        sequence->flags |= 0x1u;
        track->frame = 0;
    }

    SPL_getValueXYZ(&sample.x, spline, (float)track->frame);

    if ((sequence->state_flags & 0x4u) == 0) {
        if (track->flags & 0x10u) {
            float dx = sample.x - actor->position.x;
            float dz = sample.z - actor->position.z;
            actor->rotation.y = xglAtan2(dx, dz);
        }
    }

    if ((sequence->state_flags & 0x10u) == 0) {
        /* +0x6f0 is the actor's second flags word: ACT_info prints it with the
         * same "FLAGS %08x" it uses for +0x00 (0x00306374) and ACT_pauseUpdate
         * sets and clears bit 0x80000 in it across all 64 entries
         * (0x00305ff0/0x00306060). It is past the recovered head of Actor. */
        u32 *move_flags = (u32 *)(actor_bytes + 0x6f0);
        Vector4 delta;
        float length;

        __asm__ __volatile__(
            "lqc2 $vf3,0(%1)\n\t"
            "lqc2 $vf2,0(%2)\n\t"
            "vsub.xyz $vf2xyz,$vf2xyz,$vf3xyz\n\t"
            "sqc2 $vf2,0(%0)\n\t"
            :
            : "r"(&delta), "r"(&actor->position), "r"(&sample)
            : "memory"
        );

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
    if (track->last_frame < (short)track->frame)
        sequence->state_flags &= ~0x1u;
}

void SEQ_moveChr(Actor *actor)
{
    Vector4 *rotation = &actor->rotation;
    Vector4 *position = &actor->position;
    ActSequenceEntry *sequence = &actSequence[actor->number];
    MoveTrack *track = &sequence->move;
    Vector4 *start = &track->start;
    float *step = &track->target.w;
    Vector4 *target = &track->target_actor->position;
    float yaw;
    Vector4 velocity;
    Vector4 delta;

    if ((sequence->state.flags & 0x1) == 0) {
        sequence->state.flags |= 0x1;
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
    if ((sequence->state.state_flags & 0x4) == 0)
        rotation->y = yaw;

    delta.x = target->x - position->x;
    delta.z = target->z - position->z;
    delta.w = delta.x * delta.x + delta.z * delta.z;
    delta.w = __builtin_sqrtf(delta.w);
    if (delta.w < __builtin_sqrtf((*step + 0.5f) * (*step + 0.5f))) {
        sequence->state.state_flags &= ~0x1;
        if ((sequence->state.state_flags & ~0x10) == 0)
            sequence->state.state_flags = 0;
    } else {
        position->x += xglSin(yaw) * *step;
        position->z += xglCos(yaw) * *step;
        if ((sequence->state.state_flags & 0x10) == 0) {
            u32 *move_flags = &actor->move_flags;

            if ((*move_flags & 0x8) == 0)
                *move_flags |= 0x8;
            if (*step <= 0.083333336f)
                ACT_setMotion(actor, 1);
            else
                ACT_setMotion(actor, 3);
        }
        sequence->frame_counter++;
    }
}

void SEQ_setMotion(Actor *actor)
{
    SequenceState *sequence = &actSequence[actor->number].state;
    MotionTrack *track = (MotionTrack *)((u8 *)sequence + 0x138);
    /* The four idle motions; the original names this function-scope table mtnIDLE.0. */
    static u32 mtnIDLE[4] = { 1, 6, 11, 9 };
    unsigned int variant;

    sequence->state_flags |= 0x10;
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
    MoveTrack *track = &sequence->move;
    Vector4 *target = &track->target;

    track->target.w = 1.0f / 24.0f;
    sequence->state.state_flags |= 0x1;
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
    SequenceState *sequence = &actSequence[actor->number].state;
    RotationTrack *track = (RotationTrack *)((u8 *)sequence + 0xb8);

    sequence->handler[1] = (void *)SEQ_rotate;
    sequence->state_flags |= 0x4;
    track->target = GameLoopState[1];
    track->duration[1] = 10;
    track->face_flags |= 0x2;
    ACT_setMotion(actor, 7);
}

void SEQ_moveNPC_XZ(Actor *actor)
{
    Vector4 *rotation = &actor->rotation;
    Vector4 *position = &actor->position;
    ActSequenceEntry *sequence = &actSequence[actor->number];
    MoveTrack *track = &sequence->move;
    Vector4 *start = &track->start;
    Vector4 *target = &track->target;
    float *step = &track->target.w;
    float yaw;
    Vector4 velocity;
    Vector4 delta;

    if ((sequence->state.flags & 0x1) == 0) {
        sequence->state.flags |= 0x1;
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
    if ((sequence->state.state_flags & 0x4) == 0)
        rotation->y += nearDir(rotation->y, yaw) * 0.1f;
    if ((sequence->state.state_flags & 0x10) == 0) {
        u32 *move_flags = &actor->move_flags;

        if ((*move_flags & 0x8) == 0)
            *move_flags |= 0x8;
        if (*step <= 0.083333336f)
            ACT_setMotion(actor, 2);
        else
            ACT_setMotion(actor, 4);
    }

    delta.x = target->x - position->x;
    delta.z = target->z - position->z;
    delta.w = delta.x * delta.x + delta.z * delta.z;
    delta.w = __builtin_sqrtf(delta.w);
    if (delta.w <= __builtin_sqrtf(*step * *step)) {
        sequence->state.state_flags &= ~0x1;
        if ((sequence->state.state_flags & ~0x10) == 0)
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
    MoveTrack *track = &sequence->move;
    Vector4 *start = &track->start;
    Vector4 *target = &track->target;
    float *step = &track->target.w;
    float yaw;
    Vector4 velocity;
    Vector4 delta;

    if ((sequence->state.flags & 0x1) == 0) {
        sequence->state.flags |= 0x1;
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
    if ((sequence->state.state_flags & 0x4) == 0)
        rotation->y += nearDir(rotation->y, yaw) * 0.1f;
    if ((sequence->state.state_flags & 0x10) == 0) {
        u32 *move_flags = &actor->move_flags;

        if ((*move_flags & 0x8) == 0)
            *move_flags |= 0x8;
    }

    delta.x = target->x - position->x;
    delta.z = target->z - position->z;
    delta.w = delta.x * delta.x + delta.z * delta.z;
    delta.w = __builtin_sqrtf(delta.w);
    if (delta.w <= __builtin_sqrtf(*step * *step)) {
        sequence->state.state_flags &= ~0x1;
        if ((sequence->state.state_flags & ~0x10) == 0)
            sequence->state.state_flags = 0;
    }
    position->x += xglSin(yaw) * *step;
    position->z += xglCos(yaw) * *step;
    sequence->frame_counter++;
}

void SEQ_motion(Actor *actor)
{
    /* The actor's motion block at +0x6F0, addressed from one base. */
    typedef struct ActorMotionState {
        u32 move_flags;           /* +0x6F0 */
        float time;               /* +0x6F4 */
        float speed;              /* +0x6F8 */
        float loop_start;         /* +0x6FC */
        float loop_end;           /* +0x700 */
        u8 unmodeled_704[0x714 - 0x704];
        float blend;              /* +0x714 */
    } ActorMotionState;
    ActSequenceEntry *sequence = &actSequence[actor->number];
    MotionTrack *track = &sequence->motion;
    ActorMotionState *motion = (ActorMotionState *)&actor->move_flags;
    float seconds_per_frame;
    float blend;
    float speed;

    if ((sequence->state.flags & 0x10) == 0) {
        seconds_per_frame = 0.033333335f;
        sequence->state.flags |= 0x10;
        motion->move_flags |= 0x1;
        blend = track->blend * seconds_per_frame;
        motion->move_flags &= ~0x1;
        track->flags |= 0x20000000;
        motion->blend = blend;
        motion->speed = track->speed * seconds_per_frame;
        ACT_setMotion2(actor, track->motion, track->flags);
        if (track->loop_start >= 0) {
            float loop_end = track->loop_end * seconds_per_frame;
            float loop_start = track->loop_start * seconds_per_frame;

            motion->loop_start = loop_start;
            motion->loop_end = loop_end;
            if (motion->speed < 0.0f)
                motion->time = loop_end;
            else
                motion->time = loop_start;
        }
    }
    speed = motion->speed;

    if ((sequence->state.state_flags & ~0x10) == 0) {
        if (motion->move_flags & 0x1000)
            sequence->state.state_flags = 0;
    }
    track->time += speed;
}

void ACT_updateSequence(Actor *actor)
{
    MovingSequence *sequence = (MovingSequence *)&actSequence[actor->number];
    void **handler = sequence->handler;
    int remaining = 3;

    do {
        unsigned int address = (unsigned int)*handler++;

        if (address >= 0x1f0000 && address < 0x2000000 && (address & 0x3) == 0)
            ((void (*)(Actor *))address)(actor);
    } while (--remaining >= 0);

    if (sequence->flags & 0x100) {
        unsigned int state = sequence->state_flags;

        if ((state & 0x1) == 0)
            sequence->handler[0] = 0;
        if ((state & 0xe) == 0)
            sequence->handler[1] = 0;
        if ((state & 0x10) == 0)
            sequence->handler[2] = 0;
        if ((state & sequence->active_mask) == 0)
            actor->flags &= ~0x2;
    } else if (sequence->state_flags == 0) {
        sequence->handler[3] = 0;
        actor->flags &= ~0x2;
        sequence->handler[0] = 0;
        sequence->handler[1] = 0;
        sequence->handler[2] = 0;
        sequence->flags = 0;
    }

    if (actor->flags & 0x40) {
        float height = UnduGet(actor->position.x, actor->position.z);

        if (height != -1000.0f)
            actor->position.y = height;
    }
    ACT_updateMotion(actor);
}

void ACT_updateNPC(Actor *actor)
{
    MovingSequence *sequence = (MovingSequence *)&actSequence[actor->number];
    int remaining;
    Vector4 moved;
    float value;
    float expected;
    float x;
    float z;

    moved.x = actor->position.x;
    moved.y = actor->position.y;
    moved.z = actor->position.z;

    for (remaining = 0; remaining < 4; remaining++) {
        unsigned int address = (unsigned int)sequence->handler[remaining];

        if (address >= 0x1f0000 && address < 0x2000000 && (address & 0x3) == 0)
            ((void (*)(Actor *))address)(actor);
    }

    if (sequence->state_flags == 0) {
        sequence->handler[0] = 0;
        sequence->handler[1] = 0;
        actor->flags &= ~0x2;
        sequence->handler[2] = 0;
        sequence->flags = 0;
        sequence->handler[3] = 0;
        if (sequence->idle_roll == 0) {
            sequence->handler[2] = (void *)SEQ_motion;
            SEQ_setMotion(actor);
        } else {
            SEQ_setPositionRAND(actor);
            sequence->handler[0] = (void *)SEQ_moveNPC_XZ;
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
    if (value != -1000.0f)
        actor->position.y = value;
    xglVectorLength(&value, &actor->velocity);
    if (value > 0.0f) {
        xglVectorLength(&expected, &moved);
        xglVectorLength(&value, &actor->position);
        if (expected - value != 0.0f) {
            if (sequence->state_flags & 0x1)
                sequence->state_flags &= ~0x1;
        }
    }
    ACT_updateMotion(actor);
}

void ACT_updateDefault(Actor *actor)
{
    if (actor->flags & 0x40)
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

    if (pad->held & 0x100) {
        if ((u16)(pad->pressed & 0x1)) {
            position->x = 0.0f;
            position->z = 0.0f;
        }
        if (pad->pressed & 0x4)
            actor->cleared_on_scene_init = ((u16)actor->cleared_on_scene_init + 1) & 0x1;
    }
    if (pad->pressed & 0x8)
        position->y += 0.5f;
    if (pad->pressed & 0x2)
        position->y -= 0.25f;

    if (pad->stick_in_use) {
        stick.x = pad->stick_x;
        stick.y = 0.0f;
        stick.z = pad->stick_y;
        stick.w = 1.0f;
        moved = 1;
        xglVectorLength(&speed, &stick);
        if (!(speed < 80.0f))
            speed = 0.13333334f;
        else
            speed = 0.06666667f;
    } else if (pad->held & 0xf000) {
        int direction_bits = 0;
        float heading;

        if (pad->held & 0x1000)
            direction_bits = 1;
        if (pad->held & 0x2000)
            direction_bits |= 0x2;
        if (pad->held & 0x8000)
            direction_bits |= 0x4;
        if (pad->held & 0x4000)
            direction_bits |= 0x8;
        moved = 1;
        if (pad->held & 0x40)
            speed = 0.13333334f;
        else
            speed = 0.06666667f;
        heading = newsToDirection[direction_bits];
        stick.x = xglSin(heading) * 128.0f;
        stick.y = 0.0f;
        stick.z = xglCos(heading) * 128.0f;
        stick.w = 1.0f;
    }

    if (moved) {
        float yaw;
        float difference;
        float turn;

        /* sceVu0CopyMatrix shape: the camera basis is copied as four quadwords
         * through a fixed scratch register. */
        __asm__ __volatile__(
            "lq $2, 0(%1)\n"
            "sq $2, 0(%0)\n"
            "lq $2, 16(%1)\n"
            "sq $2, 16(%0)\n"
            "lq $2, 32(%1)\n"
            "sq $2, 32(%0)\n"
            "lq $2, 48(%1)\n"
            "sq $2, 48(%0)\n"
            :
            : "r"(&matrix), "r"(&((CameraBasis *)camera)->matrix)
            : "$2", "memory");
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
        if (!(0.083333336f < speed))
            ACT_setMotion(actor, 1);
        else
            ACT_setMotion(actor, 3);
    } else {
        ACT_setMotion(actor, 0);
    }

    if (__builtin_fabsf(actor->rotation.y * 0.31830987f) >= 2.0f)
        actor->rotation.y -= (int)(actor->rotation.y * 0.31830987f) * 3.1415927f;
    if (actor->cleared_on_scene_init == 0)
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
    for (i = 0; i < 64; i++) {
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

    for (i = 0; i < 64; i++) {
        NearActorSequenceFields *sequence_fields = (void *)entry;

        entry->java_object_ref = 0;
        entry->flags = 0x20;
        entry->signal = 0;
        entry->cleared_on_create_half = 0;
        entry->update = ACT_updateDefault;
        entry->draw = 0;
        sequence_fields->cleared_on_init_words[1] = 0;
        sequence_fields->cleared_on_init_words[0] = 0;
        UnduParamInit(&entry->undulation);
        entry->shadow_kind = 1;
        entry->shadow_size = 0x50;
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
        for (i = 0; i < 64; i++) {
            entry->flags = 0x20;
            entry->shadow_kind = 1;
            entry->shadow_size = 0x50;
            entry->java_object_ref = 0;
        }
    } else {
        entry->java_object_ref = 0;
    }
}

Actor *ACT_createChr(int type, int id)
{
    Actor *actor = ACT_create(type, id);

    if (actor != 0) {
        SequenceState *sequence = &actSequence[actor->number].state;

        actor->shadow_kind = 1;
        actor->signal = 0;
        actor->java_object_ref = 0;
        actor->flags = 0x20;
        actor->update = ACT_updateSequence;
        actor->cleared_on_create_half = 0;
        actor->scale.x = 1.0f;
        actor->scale.y = 1.0f;
        actor->scale.z = 1.0f;
        actor->scale.w = 1.0f;
        actor->shadow_size = 0x50;
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
        actor->cleared_on_create_half = 0;
        actor->java_object_ref = 0;
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

    for (i = 0; i < 64; i++) {
    }
}

static StudioCamera *getCurrentCamera(void)
{
    StudioCamera *camera;
    int camera_index;

    for (camera_index = 0; camera_index < 8; camera_index++) {
        xglStudioGetCamera(&camera, camera_index);
        if (((StudioCameraActivePrefix *)camera)->active)
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
    actor->motion_time = play->currentTime;
    actor->undulation = 0;

    linked_actor = actor->linked_actor;
    UnduGet2(&actor->undulation, linked_actor->velocity.x,
             linked_actor->velocity.z);
}

void ACT_initScene(void)
{
    Actor *entry;
    int remaining;

    entry = (Actor *)actor;
    remaining = 63;
    do {
        remaining--;
        entry->flags = 0x20;
        entry->shadow_kind = 1;
        entry->shadow_size = 0x50;
        entry->cleared_on_scene_init = 0;
        entry->state_flags = 0;
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

    if (actor->state_flags & 0xf000) {
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
    nmlModelSetFilter(1, actor->filter_param_1, actor->filter_param_2);
    nmlModelSetTransparency(actor->transparency);
    nmlModelSetReflTransparency(actor->refl_transparency);
}

void ACT_filterStealth(Actor *actor)
{
    nmlModelSetToumei(1);
    nmlModelSetZwrite(1);
    nmlModelSetStencil(1);
    nmlModelSetFilter(2, actor->filter_param_1, actor->filter_param_2);
    nmlModelSetTransparency(actor->transparency);
    nmlModelSetReflTransparency(actor->refl_transparency);
}