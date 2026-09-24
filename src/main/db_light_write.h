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

/*
 * The engine's actor record (`actor`, 64-entry array at main 0x0043c1e0,
 * 0xa70-byte stride; fuller evidence in src/main/near_dir.h,
 * src/main/enemy_2.h and src/main/set_motion.h). prevActor only reads
 * +0x00 flags (bit 0x8, hidden) and +0x86, the in-use id ACT_create writes
 * and ACT_update skips a slot on when it is zero; the offsets between them
 * are not evidenced by this TU and stay unmodeled.
 */
#define ACTOR_COUNT 64
#define ACTOR_IN_USE_ID_OFFSET 0x86

typedef struct {
    u32 flags;
    u8 unmodeled_04[ACTOR_IN_USE_ID_OFFSET - 0x04];
    short inUseId;
    u8 unmodeled_88[0xA70 - (ACTOR_IN_USE_ID_OFFSET + 2)];
} ActorHead;

extern ActorHead actor[ACTOR_COUNT];

void MAP_serach(void);

void changeCameraMode(void);

/*
 * cursor[0].x's fourth `sw`-not-`swc1` raw word (VW_setCursorMode's own
 * store target): the visualizer's current camera/cursor mode selector,
 * read back as an int by changeCameraMode. It shares the same slot's other
 * three words with the position float fields of CURSOR_CALLBACK_OFFSET's
 * comment; the mode selector is this array's first word, so its offset is 0.
 */
#define CURSOR_MODE_OFFSET 0x0

void drawAxis(Matrix4 matrix, float scale);

void EvtTools(void);
void updateCursor(int mode);
void ACT_update(void);
void JTHREAD_cntl(void);
void MAP_updateUnit(void);
void PLAY_ctrl(void);
void TCAMERA_update(void);
extern PadPrefix PadData;
extern int mode_004DC5A8;

#endif /* SRC_MAIN_DB_LIGHT_WRITE_H */
