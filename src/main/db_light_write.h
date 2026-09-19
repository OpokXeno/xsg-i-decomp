/*
 * TU-local declarations of main/tu149 (src/main/db_light_write.c).
 */

#ifndef SRC_MAIN_DB_LIGHT_WRITE_H
#define SRC_MAIN_DB_LIGHT_WRITE_H

#include "shared.h"

void VW_getCursor(HomogeneousVector *destination);

/*
 * updateCursorMode2 supplies the cursor and an actor position here.  A
 * candidate is eligible only when its xyz distance is below radius; the
 * distance is returned for nearest-candidate selection, otherwise -1.0f.
 */
float ball2point(const Vector4 *cursor_position,
                 const Vector4 *actor_position, float radius);

void VW_setCursor(const Vector4 *position);

/* Callback registered on the visualizer cursor by VW_setCursorFunc, invoked
 * with the caller argument stored alongside it. */
typedef void (*CursorCallback)(void *argument);

/*
 * cursor's demonstrated HomogeneousVector layout (include/shared.h) covers
 * only the position slot at +0x10. VW_setCursorFunc stores a callback
 * pointer and its argument as two raw words at +0x50/+0x54 (evidenced by
 * `sw`, not `swc1`), which is neither of those float fields, so both are
 * reached by byte offset from the cursor array instead of through a
 * HomogeneousVector member.
 */
#define CURSOR_CALLBACK_OFFSET 0x50
#define CURSOR_CALLBACK_ARGUMENT_OFFSET 0x54

void VW_setCursorFunc(CursorCallback callback, void *argument);

#endif /* SRC_MAIN_DB_LIGHT_WRITE_H */
