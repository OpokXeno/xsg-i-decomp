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

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", prevActor);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", nextActor);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", updateCursorMode1);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", updateCursorMode2);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", updateCursorModePlane);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", updateCursorMode0);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", changeCameraMode);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", updateCursor);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", drawCursor);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", initCursor);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", printM);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", MAP_serach);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", EvtTools);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", initLight3);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", initLight2);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", initLight);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", updateLight);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", updateWind);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", VW_setCursorMode);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", VW_setCursorFunc);

INCLUDE_ASM("asm/main/nonmatchings/db_light_write", VW_setCursor);

void VW_getCursor(HomogeneousVector *destination)
{
    destination->x = cursor[1].x;
    destination->y = cursor[1].y;
    destination->z = cursor[1].z;
    destination->w = 1.0f;
}
