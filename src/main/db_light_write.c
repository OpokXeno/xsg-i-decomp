#include "common.h"
#include "shared.h"
#include "db_light_write.h"

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", DB_lightWrite);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", drawRect2);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", drawBox2);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", drawCircle2);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", drawVector2);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", drawVector4S);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", drawGrid);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", getCurrentCamera_00266210);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", drawAxis);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", drawCircle3);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", drawTags);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", drawVector4);

/*
 * updateCursorMode2 supplies the cursor and an actor position here.  A
 * candidate is eligible only when its xyz distance is below radius; the
 * distance is returned for nearest-candidate selection, otherwise -1.0f.
 */
float ball2point(const Vector4 *cursor_position,
                 const Vector4 *actor_position, float radius)
{
    Vector4 difference;
    float distance;

    difference.x = cursor_position->x - actor_position->x;
    difference.y = cursor_position->y - actor_position->y;
    difference.z = cursor_position->z - actor_position->z;
    xglVectorLength(&distance, &difference);
    if (distance < radius)
        return distance;
    return -1.0f;
}

void *prevActor(int startIndex)
{
    int i;

    for (i = startIndex; i >= 0; i--) {
        if (actor[i].inUseId != 0 && !(actor[i].flags & 8)) {
            return &actor[i];
        }
    }
    return 0;
}

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", nextActor);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", updateCursorMode1);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", updateCursorMode2);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", updateCursorModePlane);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", updateCursorMode0);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", changeCameraMode);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", updateCursor);

void drawCursor(void)
{
    Matrix4 matrix;
    HomogeneousVector *position;

    position = &cursor[1];
    position->w = 1.0f;
    xglMatrixStackUnit();
    xglMatrixStackTrans(&position->x);
    xglMatrixStackSave(matrix);
    drawAxis(matrix, 1.0f);
}

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", initCursor);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", printM);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", MAP_serach);

void EvtTools(void)
{
    if ((PadData.half_2a & 0x10) && (PadData.half_28 & 0x100)) {
        mode_004DC5A8 = (mode_004DC5A8 + 1) & 1;
    }
    MAP_serach();
    if (mode_004DC5A8 == 1) {
        JTHREAD_cntl();
        PLAY_ctrl();
    }
    TCAMERA_update();
    if (mode_004DC5A8 == 1) {
        ACT_update();
        MAP_updateUnit();
    }
    updateCursor(0);
    drawCursor();
}

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", initLight3);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", initLight2);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", initLight);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", updateLight);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", updateWind);

void VW_setCursorMode(int mode)
{
    *(int *)((unsigned char *)cursor + CURSOR_MODE_OFFSET) = mode;
    changeCameraMode();
}

void VW_setCursorFunc(CursorCallback callback, void *argument)
{
    unsigned char *cursorBytes = (unsigned char *)cursor;
    *(CursorCallback *)(cursorBytes + CURSOR_CALLBACK_OFFSET) = callback;
    *(void **)(cursorBytes + CURSOR_CALLBACK_ARGUMENT_OFFSET) = argument;
}

void VW_setCursor(const Vector4 *position)
{
    cursor[1].x = position->x;
    cursor[1].y = position->y;
    cursor[1].z = position->z;
}

void VW_getCursor(HomogeneousVector *destination)
{
    destination->x = cursor[1].x;
    destination->y = cursor[1].y;
    destination->z = cursor[1].z;
    destination->w = 1.0f;
}
