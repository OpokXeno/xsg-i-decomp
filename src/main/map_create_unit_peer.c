#include "common.h"
#include "shared.h"
#include "main/xgl_2.h"
#include "map_create_unit_peer.h"

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

/* Still asm; declared here until its own TU is published. */
extern void ANM_resetDefault(MapUnitAnmState *anim, int param);

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

/* play.c (main); no header is published for it yet (also declared this way
 * in src/main/near_dir.c). */
extern void *PLAY_getCurrent(void);
/* Still asm; declared here until its own TU is published. */
extern void SEQ_motionUnit(MapUnitMotionRecord *unit);

void MAP_updateUnitMPack(MapUnitMotionRecord *unit)
{
    void *play = PLAY_getCurrent();

    xglMatrixStackUnit();
    xglMatrixStackTrans(&unit->position.x);
    xglMatrixStackRotX(unit->rotation.x);
    xglMatrixStackRotY(unit->rotation.y);
    xglMatrixStackRotZ(unit->rotation.z);
    xglMatrixStackScale(&unit->scale.x);
    xglMatrixStackSave(unit->matrix);
    SEQ_motionUnit(unit);

    /* PLAY_getCurrent's object (play.c) is not recovered; +0x44 is the one
     * float this TU reads from it, mirrored into the unit's own motionTime. */
    unit->anim.motionTime = *(float *)((unsigned char *)play + 0x44);
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

    xglMatrixStackUnit();
    xglMatrixStackTrans(&unit->position.x);
    xglMatrixStackRotX(unit->rotation.x);
    xglMatrixStackRotY(unit->rotation.y);
    xglMatrixStackRotZ(unit->rotation.z);
    xglMatrixStackScale(&unit->scale.x);
    /* Preserve the matrix-save call before the shared register epilogue. */
    do {
        xglMatrixStackSave(unit->matrix);
    } while (0);
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

INCLUDE_ASM("asm/main/nonmatchings/map_create_unit_peer", MAP_initUnitSequance);

INCLUDE_ASM("asm/main/nonmatchings/map_create_unit_peer", MAP_updateUnitPartsSequence);

INCLUDE_ASM("asm/main/nonmatchings/map_create_unit_peer", MAP_updateUnitPartsSequence2);
