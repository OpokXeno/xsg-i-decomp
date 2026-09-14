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

#endif /* SRC_MAIN_DB_LIGHT_WRITE_H */
