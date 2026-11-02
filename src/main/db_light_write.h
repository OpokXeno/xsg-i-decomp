/*
 * TU-local declarations of main/tu149 (src/main/db_light_write.c).
 */

#ifndef SRC_MAIN_DB_LIGHT_WRITE_H
#define SRC_MAIN_DB_LIGHT_WRITE_H

#include "shared.h"
#include "main/act_1.h"
#include "main/db_light_write.h"
#include "main/jthread.h"
#include "main/map_2.h"
#include "main/play.h"
#include "main/tcamera.h"

#define ACTOR_COUNT 64

struct CursorPadKeyResult {
    u16 held;
    u16 pressed;
    u16 repeat;
    u16 other_buttons;
    signed char flags[8];
};

extern Actor actor[ACTOR_COUNT];
extern int mode_004DC5A8;

void VW_getCursor(HomogeneousVector *destination);

/* The studio-camera table accessor is defined in main/tu104. */
extern void xglStudioGetCamera(StudioCamera **camera_out, int camera_index);

/*
 * updateCursorMode2 supplies the cursor and an actor position here.  A
 * candidate is eligible only when its xyz distance is below radius; the
 * distance is returned for nearest-candidate selection, otherwise -1.0f.
 */
float ball2point(const Vector4 *cursor_position,
                 const Vector4 *actor_position, float radius);

void VW_setCursor(const Vector4 *position);
void VW_setCursorFunc(CursorCallback callback, void *argument);
void MAP_serach(void);
void changeCameraMode(void);
void drawAxis(Matrix4 matrix, float scale);
void updateCursor(int mode);
#endif /* SRC_MAIN_DB_LIGHT_WRITE_H */
