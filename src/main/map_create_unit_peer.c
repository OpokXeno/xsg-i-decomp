#include "common.h"
#include "shared.h"
#include "main/xgl_2.h"
#include "map_create_unit_peer.h"
#include "main/map_2.h"
#include "main/play.h"
#include "main/map_create_unit_peer.h"
#include "quadword.h"
#include "vu0_macro.h"

/*
 * MapUnitGroupSequenceEntry.flags (a track was started) and .state (a track is
 * still running) share one bit layout: one bit per animated channel.
 */
#define MAP_SEQ_MOVE 0x01
#define MAP_SEQ_ROTATE_X 0x02
#define MAP_SEQ_ROTATE_Y 0x04
#define MAP_SEQ_ROTATE_Z 0x08
#define MAP_SEQ_ROTATE (MAP_SEQ_ROTATE_X | MAP_SEQ_ROTATE_Y | MAP_SEQ_ROTATE_Z)
#define MAP_SEQ_MOTION 0x10
#define MAP_SEQ_SCALE_X 0x20
#define MAP_SEQ_SCALE_Y 0x40
#define MAP_SEQ_SCALE_Z 0x80
#define MAP_SEQ_SCALE (MAP_SEQ_SCALE_X | MAP_SEQ_SCALE_Y | MAP_SEQ_SCALE_Z)

/* MapUnitTurnTrack.faceFlags: the axis whose goal is the direction to the target unit. */
#define MAP_FACE_X 0x1
#define MAP_FACE_Y 0x2
#define MAP_FACE_Z 0x4

/* One component of `goal` carried through a bone matrix: the three
 * rotation/scale rows, then the translation row. */
#define MAP_BONE_TRANSFORM(bone, column) \
    (goal.x * (bone)[0][column] + goal.y * (bone)[1][column] + \
     goal.z * (bone)[2][column] + (bone)[3][column])

/* Build the matrix from the position, rotation and scale kept by both
 * sequenced units and motion-pack units. */
#define MAP_BUILD_UNIT_TRANSFORM(unit) do { \
    xglMatrixStackUnit(); \
    xglMatrixStackTrans(&(unit)->position.x); \
    xglMatrixStackRotX((unit)->rotation.x); \
    xglMatrixStackRotY((unit)->rotation.y); \
    xglMatrixStackRotZ((unit)->rotation.z); \
    xglMatrixStackScale(&(unit)->scale.x); \
    xglMatrixStackSave((unit)->matrix); \
} while (0)

extern void ANM_resetDefault(MapUnitAnmState *anim, int param);
extern void *PLAY_getCurrent(void);
MapUnitGroupEntry *MAP_createUnitPeer(int slot, int serial)
{
    MapUnitGroupEntry *unit = (MapUnitGroupEntry *)MAP_createUnit(slot, serial);
    MapUnitGroupSequenceEntry *sequence;

    if (unit != 0) {
        sequence = &unitSequence[unit->serial];
        unit->animationMask = 0x1ff;
        unit->anim.speed = 0.033333335f;
        unit->serialFlags = 0;
        unit->scale.w = 1.0f;
        unit->scale.z = 1.0f;
        unit->scale.y = 1.0f;
        unit->peer = 0;
        unit->animationIndex = 0;
        unit->scale.x = 1.0f;
        if (serial != 0x6025) {
            unit->update = 0;
        }
        sequence->group = -1;
        sequence->update_callbacks[3] = 0;
        sequence->flags = 0;
        sequence->state = 0;
        sequence->pivot_x = 0;
        sequence->pivot_y = 0;
        sequence->pivot_z = 0;
        sequence->pivot_w = 1.0f;
        sequence->axis_x = 1.0f;
        sequence->axis_y = 0;
        sequence->axis_z = 0;
        sequence->sequenceMode = 0;
        sequence->sequenceParameters[0] = 0;
        sequence->sequenceParameters[1] = 0;
        sequence->sequenceParameters[2] = 0;
        sequence->sequenceParameters[3] = 0;
        sequence->update_callbacks[0] = 0;
        sequence->update_callbacks[1] = 0;
        sequence->update_callbacks[2] = 0;
        sequence->axis_w = 1.0f;
    }
    return unit;
}

void SEQ_rotYCNSUnitChr(MapUnitRecord *unit)
{
    Vector4 *position = &unit->position;
    Vector4 *rotation = &unit->rotation;
    MapUnitGroupSequenceEntry *sequence = &unitSequence[unit->slot];
    MapUnitBoneAimTrack *track = &sequence->rotation.bone;
    Vector4 goal;
    Vector4 world;

    goal.w = 1.0f;
    goal.x = track->goal[0];
    goal.y = track->goal[1];
    goal.z = track->goal[2];
    sequence->state = 1;
    world.x = MAP_BONE_TRANSFORM(track->owner->matrices[sequence->rotation.bone.boneIndex], 0);
    world.y = MAP_BONE_TRANSFORM(track->owner->matrices[sequence->rotation.bone.boneIndex], 1);
    world.z = MAP_BONE_TRANSFORM(track->owner->matrices[sequence->rotation.bone.boneIndex], 2);
    rotation->y = xglAtan2(world.x - position->x, world.z - position->z);
}

void SEQ_transCNSUnitChr(MapUnitRecord *unit)
{
    Vector4 *position = &unit->position;
    MapUnitGroupSequenceEntry *sequence = &unitSequence[unit->slot];
    MapUnitBoneTrack *track = &sequence->move.bone;
    Vector4 goal;
    Vector4 world;

    goal.w = 1.0f;
    goal.x = track->offset.x;
    goal.y = track->offset.y;
    goal.z = track->offset.z;
    sequence->state = 1;
    world.x = MAP_BONE_TRANSFORM(track->owner->matrices[sequence->boneIndex], 0);
    world.y = MAP_BONE_TRANSFORM(track->owner->matrices[sequence->boneIndex], 1);
    world.z = MAP_BONE_TRANSFORM(track->owner->matrices[sequence->boneIndex], 2);
    position->w = goal.w;
    position->x = world.x;
    position->y = world.y;
    position->z = world.z;
}

void SEQ_rotateUnit(MapUnitRecord *unit)
{
    Vector4 *position = &unit->position;
    Vector4 *rotation = &unit->rotation;
    MapUnitGroupSequenceEntry *sequence = &unitSequence[unit->slot];
    MapUnitTurnTrack *track = &sequence->rotation.turn;
    float *start = track->start;
    float *goal = track->goal;
    float *step = track->step;
    MapUnitRecord *target = track->target;
    float distance[3];

    if ((sequence->flags & MAP_SEQ_ROTATE_X) == 0) {
        if (sequence->state & MAP_SEQ_ROTATE_X) {
            start[0] = rotation->x;
            if (track->faceFlags & 0x1)
                goal[1] = xglAtan2(target->position.y - position->y, target->position.z - position->z);
            if (track->duration[0] > 0) {
                float inverse = 1.0f / track->duration[0];

                step[0] = nearDir(start[0], goal[0]) * inverse;
            }
            sequence->frameCounter = 0;
            sequence->flags |= MAP_SEQ_ROTATE_X;
        } else {
            step[0] = 0.0f;
            sequence->flags |= MAP_SEQ_ROTATE_X;
        }
    }
    if ((sequence->flags & MAP_SEQ_ROTATE_Y) == 0) {
        if (sequence->state & MAP_SEQ_ROTATE_Y) {
            start[1] = rotation->y;
            if (track->faceFlags & 0x2)
                goal[1] = xglAtan2(target->position.x - position->x, target->position.z - position->z);
            if (track->duration[1] > 0) {
                float inverse = 1.0f / track->duration[1];

                step[1] = nearDir(start[1], goal[1]) * inverse;
            }
            sequence->frameCounter = 0;
            sequence->flags |= MAP_SEQ_ROTATE_Y;
        } else {
            step[1] = 0.0f;
            sequence->flags |= MAP_SEQ_ROTATE_Y;
        }
    }
    if ((sequence->flags & MAP_SEQ_ROTATE_Z) == 0) {
        if (sequence->state & MAP_SEQ_ROTATE_Z) {
            start[2] = rotation->z;
            if (track->faceFlags & 0x4)
                goal[1] = xglAtan2(unit->position.x - position->x, unit->position.y - position->y);
            if (track->duration[2] > 0) {
                float inverse = 1.0f / track->duration[2];

                step[2] = nearDir(start[2], goal[2]) * inverse;
            }
            sequence->frameCounter = 0;
            sequence->flags |= MAP_SEQ_ROTATE_Z;
        } else {
            step[2] = 0.0f;
            sequence->flags |= MAP_SEQ_ROTATE_Z;
        }
    }

    if (sequence->state & MAP_SEQ_ROTATE_X) {
        if (track->faceFlags & 0x1)
            goal[1] = xglAtan2(unit->position.y - position->y, unit->position.z - position->z);
        distance[0] = __builtin_fabsf(nearDir(goal[0], rotation->x));
        if (distance[0] <= __builtin_fabsf(step[0])) {
            sequence->state &= ~MAP_SEQ_ROTATE_X;
            rotation->x = goal[0];
        } else {
            rotation->x += step[0];
        }
    }
    if (sequence->state & MAP_SEQ_ROTATE_Y) {
        if (track->faceFlags & 0x2)
            goal[1] = xglAtan2(unit->position.x - position->x, unit->position.z - position->z);
        distance[1] = __builtin_fabsf(nearDir(goal[1], rotation->y));
        if (distance[1] <= __builtin_fabsf(step[1])) {
            sequence->state &= ~MAP_SEQ_ROTATE_Y;
            rotation->y = goal[1];
        } else {
            rotation->y += step[1];
        }
    }
    if (sequence->state & MAP_SEQ_ROTATE_Z) {
        if (track->faceFlags & 0x4)
            goal[1] = xglAtan2(unit->position.x - position->x, unit->position.y - position->y);
        distance[2] = __builtin_fabsf(nearDir(goal[2], rotation->z));
        if (distance[2] <= __builtin_fabsf(step[2])) {
            rotation->z = goal[2];
            sequence->state &= ~MAP_SEQ_ROTATE_Z;
        } else {
            rotation->z += step[2];
        }
    }
}

void SEQ_scaleUnit(MapUnitRecord *unit)
{
    MapUnitGroupSequenceEntry *sequence = &unitSequence[unit->slot];
    MapUnitScaleTrack *track = &sequence->scale;
    float *start = track->start;
    float *goal = track->goal;
    float *step = track->step;
    Vector4 *scale = &unit->scale;
    float distance[3];

    if ((sequence->flags & 0x20) == 0) {
        if (sequence->state & 0x20) {
            start[0] = scale->x;
            if (track->duration[0] > 0)
                step[0] = (goal[0] - start[0]) * (1.0f / track->duration[0]);
            sequence->flags |= 0x20;
            sequence->frameCounter = 0;
        } else {
            step[0] = 0.0f;
            sequence->flags |= 0x20;
        }
    }
    if ((sequence->flags & 0x40) == 0) {
        if (sequence->state & 0x40) {
            start[1] = scale->y;
            if (track->duration[1] > 0)
                step[1] = (goal[1] - start[1]) * (1.0f / track->duration[1]);
            sequence->flags |= 0x40;
            sequence->frameCounter = 0;
        } else {
            step[1] = 0.0f;
            sequence->flags |= 0x40;
        }
    }
    if ((sequence->flags & 0x80) == 0) {
        if (sequence->state & 0x80) {
            start[2] = scale->z;
            if (track->duration[2] > 0)
                step[2] = (goal[2] - start[2]) * (1.0f / track->duration[2]);
            sequence->flags |= 0x80;
            sequence->frameCounter = 0;
        } else {
            step[2] = 0.0f;
            sequence->flags |= 0x80;
        }
    }

    if (sequence->state & 0x20) {
        distance[0] = __builtin_fabsf(goal[0] - scale->x);
        if (distance[0] <= __builtin_fabsf(step[0])) {
            scale->x = goal[0];
            sequence->state &= ~0x20;
        } else {
            scale->x += step[0];
        }
    }
    if (sequence->state & 0x40) {
        distance[1] = __builtin_fabsf(goal[1] - scale->y);
        if (distance[1] <= __builtin_fabsf(step[1])) {
            scale->y = goal[1];
            sequence->state &= ~0x40;
        } else {
            scale->y += step[1];
        }
    }
    if (sequence->state & 0x80) {
        distance[2] = __builtin_fabsf(goal[2] - scale->z);
        if (distance[2] <= __builtin_fabsf(step[2])) {
            scale->z = goal[2];
            sequence->state &= ~0x80;
        } else {
            scale->z += step[2];
        }
    }
}

void SEQ_rotateUnitSPL(MapUnitRecord *unit)
{
    MapUnitGroupSequenceEntry *sequence = &unitSequence[unit->slot];
    MapUnitSplineTrack *track = &sequence->rotation.spline;
    void *spline = track->spline;
    float sample[3];

    if ((sequence->flags & 0xe) == 0) {
        sequence->flags |= 0xe;
        track->frame = 0;
    }

    SPL_getValueXYZ(sample, spline, (float)track->frame);

    unit->rotation.x = sample[0] / 180.0f * 3.1415927f;
    unit->rotation.y = sample[1] / 180.0f * 3.1415927f;
    unit->rotation.z = sample[2] / 180.0f * 3.1415927f;

    track->frame++;
    if (track->lastFrame < track->frame)
        sequence->state &= ~0xe;
}

void SEQ_scaleUnitSPL(MapUnitRecord *unit)
{
    MapUnitGroupSequenceEntry *sequence = &unitSequence[unit->slot];
    MapUnitScaleTrack *track = &sequence->scale;
    void *spline = track->spline;
    float sample[3];

    if ((sequence->flags & 0xe0) == 0) {
        sequence->flags |= 0xe0;
        track->frame = 0;
    }

    SPL_getValueXYZ(sample, spline, (float)track->frame);

    unit->scale.x = sample[0];
    unit->scale.y = sample[1];
    unit->scale.z = sample[2];

    track->frame++;
    if (track->lastFrame < track->frame)
        sequence->state &= ~0xe0;
}

void SEQ_moveUnitSPL(MapUnitRecord *unit)
{
    MapUnitGroupSequenceEntry *sequence = &unitSequence[unit->slot];
    MapUnitSplineTrack *track = &sequence->move.spline;
    void *spline = track->spline;
    float sample[3];

    if ((sequence->flags & 0x1) == 0) {
        sequence->flags |= 0x1;
        track->frame = 0;
    }

    SPL_getValueXYZ(sample, spline, (float)track->frame);

    if ((sequence->state & MAP_SEQ_ROTATE_Y) == 0) {
        if (track->flags & 0x10)
            unit->rotation.y = xglAtan2(sample[0] - unit->position.x, sample[2] - unit->position.z);
    }

    track->frame++;
    unit->position.x = sample[0];
    unit->position.y = sample[1];
    unit->position.z = sample[2];
    if (track->lastFrame < track->frame)
        sequence->state &= ~0x1;
}

void SEQ_moveUnitChr(MapUnitRecord *unit)
{
    Vector4 *position = &unit->position;
    Vector4 *rotation = &unit->rotation;
    MapUnitGroupSequenceEntry *sequence = &unitSequence[unit->slot];
    MapUnitWalkTrack *track = &sequence->move.walk;
    Vector4 *start = &track->start;
    float *step = &track->destination.w;
    Vector4 *target = &track->followed->position;
    float yaw;
    Vector4 velocity;
    Vector4 delta;

    if ((sequence->flags & 0x1) == 0) {
        sequence->flags |= 0x1;
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
        sequence->frameCounter = 0;
    }

    yaw = xglAtan2(target->x - position->x, target->z - position->z);
    if ((sequence->state & MAP_SEQ_ROTATE_Y) == 0)
        rotation->y = yaw;

    delta.x = target->x - position->x;
    delta.z = target->z - position->z;
    delta.w = delta.x * delta.x + delta.z * delta.z;
    delta.w = __builtin_sqrtf(delta.w);
    if (delta.w < __builtin_sqrtf((*step + 0.5f) * (*step + 0.5f))) {
        sequence->state &= ~0x1;
        if ((sequence->state & ~0x10) == 0)
            sequence->state = 0;
    } else {
        position->x += xglSin(yaw) * *step;
        position->z += xglCos(yaw) * *step;
        sequence->frameCounter++;
    }
}

void SEQ_moveUnitXZ(MapUnitRecord *unit)
{
    Vector4 *position = &unit->position;
    Vector4 *rotation = &unit->rotation;
    MapUnitGroupSequenceEntry *sequence = &unitSequence[unit->slot];
    MapUnitWalkTrack *track = &sequence->move.walk;
    Vector4 *start = &track->start;
    Vector4 *target = &track->destination;
    float *step = &track->destination.w;
    float yaw;
    Vector4 velocity;
    Vector4 delta;

    if ((sequence->flags & 0x1) == 0) {
        sequence->flags |= 0x1;
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
        sequence->frameCounter = 0;
    }

    yaw = xglAtan2(target->x - position->x, target->z - position->z);
    if ((sequence->state & MAP_SEQ_ROTATE_Y) == 0)
        rotation->y = yaw;

    delta.x = target->x - position->x;
    delta.z = target->z - position->z;
    delta.w = delta.x * delta.x + delta.z * delta.z;
    delta.w = __builtin_sqrtf(delta.w);
    if (delta.w <= __builtin_sqrtf(*step * *step)) {
        sequence->state &= ~0x1;
        if ((sequence->state & ~0x10) == 0)
            sequence->state = 0;
    }
    position->x += xglSin(yaw) * *step;
    position->z += xglCos(yaw) * *step;
    sequence->frameCounter++;
}

void SEQ_motionUnit(MapUnitMotionRecord *unit)
{
    MapUnitGroupSequenceEntry *sequence = &unitSequence[unit->serial];
    MapUnitMotionTrack *track = &sequence->motion;
    MapUnitAnmState *anim = &unit->anim;

    if ((sequence->flags & 0x10) == 0) {
        sequence->flags |= 0x10;
        MAP_setUnitMotion(unit, track->motion);
        anim->flags = track->flags;
        if (track->loopStart >= 0) {
            float loop_start = track->loopStart * 0.033333335f;
            float loop_end = track->loopEnd * 0.033333335f;

            anim->loopStart = loop_start;
            anim->motionTime = loop_start;
            anim->loopEnd = loop_end;
        }
        anim->speed = track->speed * 0.033333335f;
    }

    if ((sequence->state & ~0x10) == 0) {
        if (anim->flags & 0x1000)
            sequence->state = 0;
    }
}

void MAP_setUnitMotion(MapUnitMotionRecord *unit, int motionId)
{
    unit->anim.motionId = (short)motionId;
    ANM_resetDefault(&unit->anim, unit->animResetParam);
}

void MAP_updateUnitMotion(void)
{
}

int MAP_callUnitGroup(int group, void (*callback)(int *))
{
    MapUnitGroupSequenceEntry *sequence_entry;
    int index;

    if (callback != 0) {
        for (index = 0; index < 64; index++) {
            sequence_entry = &unitSequence[index];
            if (sequence_entry->group == group) {
                callback(&MapUnit[index].flags);
            }
        }
    }
    return 0;
}

void MAP_updateUnitMPack(MapUnitMotionRecord *unit)
{
    Play *play = PLAY_getCurrent();

    MAP_BUILD_UNIT_TRANSFORM(unit);
    SEQ_motionUnit(unit);

    /* PLAY_setupDefault and PLAY_ctrl establish currentTime at +0x44. */
    unit->anim.motionTime = play->currentTime;
}

void MAP_updateUnitSequence(MapUnitRecord *unit)
{
    MapUnitGroupSequenceEntry *sequence_entry;
    int callback_index;

    sequence_entry = &unitSequence[unit->slot];
    for (callback_index = 0; callback_index < 4; callback_index++) {
        if (sequence_entry->update_callbacks[callback_index] != 0) {
            sequence_entry->update_callbacks[callback_index](unit);
        }
    }

    if (sequence_entry->state == 0) {
        unit->flags &= ~2u;
        sequence_entry->flags = 0;
        sequence_entry->update_callbacks[0] = 0;
        sequence_entry->update_callbacks[1] = 0;
        sequence_entry->update_callbacks[2] = 0;
        sequence_entry->update_callbacks[3] = 0;
    }

    if (unit->flags & MAP_UNIT_FOLLOW_GROUND) {
        float ground_height = UnduGet(unit->position.x, unit->position.z);

        if (ground_height != GROUND_HEIGHT_NONE) {
            unit->position.y = ground_height;
        }
    }

    MAP_BUILD_UNIT_TRANSFORM(unit);
}

void MAP_updateUnitDefault(MapUnitRecord *unit)
{
    if (unit->flags & MAP_UNIT_FOLLOW_GROUND) {
        float undulation = UnduGet(unit->position.x, unit->position.z);

        if (undulation != GROUND_HEIGHT_NONE)
            unit->position.y = undulation;
    }

    xglMatrixStackUnit();
    xglMatrixStackTrans(&unit->position.x);
    xglMatrixStackRotX(unit->rotation.x);
    xglMatrixStackRotY(unit->rotation.y);
    xglMatrixStackRotZ(unit->rotation.z);
    xglMatrixStackScale(&unit->scale.x);
    xglMatrixStackSave(unit->matrix);
}

void MAP_initUnitSequance(void)
{
    MapUnitGroupEntry *unit = MapUnit;
    MapUnitGroupSequenceEntry *sequence = unitSequence;
    int remaining = 63;

    do {
        remaining--;
        sequence->flags = 0;
        sequence->state = 0;
        sequence->sequenceMode = 0;
        sequence->sequenceParameters[0] = 0;
        sequence->sequenceParameters[1] = 0;
        sequence->sequenceParameters[2] = 0;
        sequence->sequenceParameters[3] = 0;
        sequence->update_callbacks[0] = 0;
        sequence->update_callbacks[1] = 0;
        sequence->update_callbacks[2] = 0;
        sequence->update_callbacks[3] = 0;
        sequence++;
    } while (remaining >= 0);

    remaining = 63;
    do {
        remaining--;
        unit->peer = 0;
        unit->flags = 0;
        unit->serialFlags = 0;
        unit->animationIndex = 0;
        unit->update = MAP_updateUnitDefault;
        unit++;
    } while (remaining >= 0);
}

void MAP_updateUnitPartsSequence(MapUnitRecord *unit)
{
    MapUnitGroupSequenceEntry *sequence_entry;
    MapPartsDisplay *display;
    int callback_index;

    sequence_entry = &unitSequence[unit->slot];
    for (callback_index = 0; callback_index < 4; callback_index++) {
        if (sequence_entry->update_callbacks[callback_index] != 0) {
            sequence_entry->update_callbacks[callback_index](unit);
        }
    }

    if (sequence_entry->state == 0) {
        unit->flags &= ~2u;
        sequence_entry->flags = 0;
        sequence_entry->update_callbacks[0] = 0;
        sequence_entry->update_callbacks[1] = 0;
        sequence_entry->update_callbacks[2] = 0;
        sequence_entry->update_callbacks[3] = 0;
    }

    if (unit->flags & MAP_UNIT_FOLLOW_GROUND) {
        unit->position.y = UnduGet(unit->position.x, unit->position.z);
    }

    if (unit->resourceModel == 0) {
        display = GameLoopState.display;
        if (display != 0 && display->model != 0) {
            Matrix4 *matrices = (Matrix4 *)((unsigned char *)display->model + display->model->matrixOffset);

            xglMatrixStackUnit();
            xglMatrixStackTrans(&unit->position.x);
            xglMatrixStackRotX(unit->rotation.x);
            xglMatrixStackRotY(unit->rotation.y);
            xglMatrixStackRotZ(unit->rotation.z);
            xglMatrixStackScale(&unit->scale.x);
            if (display->model->partCount > unit->partIndex) {
                xglMatrixStackSave(matrices[unit->partIndex]);
            }
        }
    }
}

void MAP_updateUnitPartsSequence2(MapUnitRecord *unit)
{
    MapUnitGroupSequenceEntry *sequence_entry;
    MapPartsDisplay *display;
    int callback_index;

    sequence_entry = &unitSequence[unit->slot];
    for (callback_index = 0; callback_index < 4; callback_index++) {
        if (sequence_entry->update_callbacks[callback_index] != 0) {
            sequence_entry->update_callbacks[callback_index](unit);
        }
    }

    if (sequence_entry->state == 0) {
        unit->flags &= ~2u;
        sequence_entry->flags = 0;
        sequence_entry->update_callbacks[0] = 0;
        sequence_entry->update_callbacks[1] = 0;
        sequence_entry->update_callbacks[2] = 0;
        sequence_entry->update_callbacks[3] = 0;
    }

    if (unit->flags & MAP_UNIT_FOLLOW_GROUND) {
        unit->position.y = UnduGet(unit->position.x, unit->position.z);
    }

    if (unit->resourceModel == 0) {
        display = GameLoopState.display;
        if (display != 0 && display->model != 0) {
            Matrix4 *matrices = (Matrix4 *)((unsigned char *)display->model + display->model->matrixOffset);
            Matrix4 local;
            Matrix4 aligned;
            Matrix4 inverse;
            Vector4 delta;
            Vector4 angles;
            Vector4 centre;
            Vector4 negated;
            float rot_x;
            float rot_y;
            float rz;

            /* Aligned matrix copies use the SDK's scratch-register quartet. */
            QuadwordCopyMatrix(local, unit->matrix);
            local[3][0] = 0.0f;
            local[3][3] = 1.0f;
            local[3][1] = 0.0f;
            local[3][2] = 0.0f;
            switch ((int)sequence_entry->axis_w) {
            case 1:
                alignXAxis(&sequence_entry->axis_x, aligned);
                break;
            case 2:
                alignYAxis(&sequence_entry->axis_x, aligned);
                break;
            case 3:
                alignZAxis(&sequence_entry->axis_x, aligned);
                break;
            }
            xglMatrixStackLoad(aligned);
            xglMatrixStackReverse();
            xglMatrixStackSave(inverse);
            xglMatrixStackLoad(aligned);
            xglMatrixStackMul(local);
            Vu0SubVector(&delta, &sequence_entry->pivot_x, unit->matrix[3]);
            rot_x = unit->rotation.x;
            rot_y = unit->rotation.y;
            rz = unit->rotation.z;
            angles.x = (rot_x * local[1][0] + rot_x * local[1][1] + rot_x * local[1][2]) * local[2][0]
                     + (rot_x * local[1][0] + rot_x * local[1][1] + rot_x * local[1][2]) * local[2][1]
                     + (rot_x * local[1][0] + rot_x * local[1][1] + rot_x * local[1][2]) * local[2][2];
            angles.y = (rot_y * local[0][0] + rot_y * local[0][1] + rot_y * local[0][2]) * local[2][0]
                     + (rot_y * local[0][0] + rot_y * local[0][1] + rot_y * local[0][2]) * local[2][1]
                     + (rot_y * local[0][0] + rot_y * local[0][1] + rot_y * local[0][2]) * local[2][2];
            angles.z = (rz * local[0][0] + rz * local[0][1] + rz * local[0][2]) * local[1][0]
                     + (rz * local[0][0] + rz * local[0][1] + rz * local[0][2]) * local[1][1]
                     + (rz * local[0][0] + rz * local[0][1] + rz * local[0][2]) * local[1][2];
            centre.x = local[0][0] * delta.x + local[0][1] * delta.y + local[0][2] * delta.z;
            negated.x = -centre.x;
            centre.y = local[1][0] * delta.x + local[1][1] * delta.y + local[1][2] * delta.z;
            negated.y = -centre.y;
            centre.z = local[2][0] * delta.x + local[2][1] * delta.y + local[2][2] * delta.z;
            negated.z = -centre.z;
            centre.w = 1.0f;
            delta.x = centre.x;
            delta.y = centre.y;
            delta.z = centre.z;
            negated.w = 1.0f;
            xglMatrixStackUnit();
            xglMatrixStackTrans(&centre.x);
            xglMatrixStackMul(inverse);
            xglMatrixStackRotX(angles.x);
            xglMatrixStackRotY(angles.y);
            xglMatrixStackRotZ(angles.z);
            xglMatrixStackScale(&unit->scale.x);
            xglMatrixStackMul(local);
            xglMatrixStackMul(aligned);
            xglMatrixStackTrans(&negated.x);
            xglMatrixStackSave(local);
            angles.x = unit->position.x * inverse[0][0] + unit->position.y * inverse[1][0] + unit->position.z * inverse[2][0];
            angles.y = unit->position.x * inverse[0][1] + unit->position.y * inverse[1][1] + unit->position.z * inverse[2][1];
            angles.z = unit->position.x * inverse[0][2] + unit->position.y * inverse[1][2] + unit->position.z * inverse[2][2];
            local[3][0] = local[3][0] + angles.x + unit->matrix[3][0];
            local[3][1] = local[3][1] + angles.y + unit->matrix[3][1];
            local[3][2] = local[3][2] + angles.z + unit->matrix[3][2];
            if (unit->partIndex < display->model->partCount) {
                QuadwordCopyMatrix(unit->partIndex * sizeof(Matrix4) + (unsigned int)matrices, local);
            }
        }
    }
}
