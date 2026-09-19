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

INCLUDE_ASM("asm/main/nonmatchings/map_create_unit_peer", MAP_setUnitMotion);

void MAP_updateUnitMotion(void)
{
}

INCLUDE_ASM("asm/main/nonmatchings/map_create_unit_peer", MAP_callUnitGroup);

INCLUDE_ASM("asm/main/nonmatchings/map_create_unit_peer", MAP_updateUnitMPack);

INCLUDE_ASM("asm/main/nonmatchings/map_create_unit_peer", MAP_updateUnitSequence);

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
