/*
 * TU-local declarations of main/tu149 (src/main/db_light_write.c).
 */

#ifndef SRC_MAIN_DB_LIGHT_WRITE_H
#define SRC_MAIN_DB_LIGHT_WRITE_H

#include "shared.h"

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

#define ACTOR_COUNT 64

#define ACTOR_IN_USE_ID_OFFSET 0x86

/*
 * cursor[0].x's fourth `sw`-not-`swc1` raw word (VW_setCursorMode's own
 * store target): the visualizer's current camera/cursor mode selector,
 * read back as an int by changeCameraMode. It shares the same slot's other
 * three words with the position float fields of CURSOR_CALLBACK_OFFSET's
 * comment; the mode selector is this array's first word, so its offset is 0.
 */
#define CURSOR_MODE_OFFSET 0x0

/* Callback registered on the visualizer cursor by VW_setCursorFunc, invoked
 * with the caller argument stored alongside it. */
typedef void (*CursorCallback)(void *argument);

/*
 * The engine's actor record (`actor`, 64-entry array at main 0x0043c1e0,
 * 0xa70-byte stride; fuller evidence in src/main/near_dir.h,
 * src/main/enemy_2.h and src/main/set_motion.h). prevActor only reads
 * +0x00 flags (bit 0x8, hidden) and +0x86, the in-use id ACT_create writes
 * and ACT_update skips a slot on when it is zero. updateCursorMode2 also
 * uses position xyz at +0x10, velocity xyz at +0x30, rotation.y at +0x54,
 * and the byte number ACT_create writes at +0x80. The vector extents and
 * names agree with the owner view in include/main/act_1.h. Other spans
 * remain unmodeled; neither cursor writes the vectors' fourth lanes.
 */
typedef struct {
    u32 flags;
    u8 unmodeled_04[0x10 - 0x04];
    Vector4 position;
    u8 unmodeled_20[0x30 - 0x20];
    Vector4 velocity;
    u8 unmodeled_40[0x50 - 0x40];
    Vector4 rotation;
    u8 unmodeled_60[0x80 - 0x60];
    u8 number;
    u8 unmodeled_81[ACTOR_IN_USE_ID_OFFSET - 0x81];
    short inUseId;
    u8 unmodeled_88[0xA70 - (ACTOR_IN_USE_ID_OFFSET + 2)];
} ActorHead;

struct CursorPadKeyResult {
    u16 held;
    u16 pressed;
    u16 repeat;
    u16 other_buttons;
    signed char flags[8];
};

extern ActorHead actor[ACTOR_COUNT];
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
void EvtTools(void);
void updateCursor(int mode);
void ACT_update(void);
void JTHREAD_cntl(void);
void MAP_updateUnit(void);
void PLAY_ctrl(void);
void TCAMERA_update(void);

#endif /* SRC_MAIN_DB_LIGHT_WRITE_H */
