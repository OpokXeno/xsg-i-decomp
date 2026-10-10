#include "common.h"

#include "shared.h"

#include "main/xgl_2.h"

#include "map_create_unit_peer.h"

#include "main/play.h"

extern void ANM_resetDefault(MapUnitAnmState *anim, int param);

extern void *PLAY_getCurrent(void);

extern void SEQ_motionUnit(MapUnitMotionRecord *unit);

INCLUDE_ASM("asm/main/nonmatchings/map_create_unit_peer", MAP_createUnitPeer);

INCLUDE_ASM("asm/main/nonmatchings/map_create_unit_peer", SEQ_rotYCNSUnitChr);

INCLUDE_ASM("asm/main/nonmatchings/map_create_unit_peer", SEQ_transCNSUnitChr);

INCLUDE_ASM("asm/main/nonmatchings/map_create_unit_peer", SEQ_rotateUnit);

INCLUDE_ASM("asm/main/nonmatchings/map_create_unit_peer", SEQ_scaleUnit);

INCLUDE_ASM("asm/main/nonmatchings/map_create_unit_peer", SEQ_rotateUnitSPL);

INCLUDE_ASM("asm/main/nonmatchings/map_create_unit_peer", SEQ_scaleUnitSPL);

INCLUDE_ASM("asm/main/nonmatchings/map_create_unit_peer", SEQ_moveUnitSPL);

INCLUDE_ASM("asm/main/nonmatchings/map_create_unit_peer", SEQ_moveUnitChr);

INCLUDE_ASM("asm/main/nonmatchings/map_create_unit_peer", SEQ_moveUnitXZ);

INCLUDE_ASM("asm/main/nonmatchings/map_create_unit_peer", SEQ_motionUnit);

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

    sequence_entry = &unitSequence[unit->serial];
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

        if (ground_height != -1000.0f) {
            unit->position.y = ground_height;
        }
    }

    MAP_BUILD_UNIT_TRANSFORM(unit);
}

void MAP_updateUnitDefault(MapUnitRecord *unit)
{
    if (unit->flags & MAP_UNIT_FOLLOW_GROUND) {
        float undulation = UnduGet(unit->position.x, unit->position.z);

        if (undulation != -1000.0f)
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

INCLUDE_ASM("asm/main/nonmatchings/map_create_unit_peer", MAP_updateUnitPartsSequence);

INCLUDE_ASM("asm/main/nonmatchings/map_create_unit_peer", MAP_updateUnitPartsSequence2);
