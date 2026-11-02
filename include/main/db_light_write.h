#ifndef INCLUDE_MAIN_DB_LIGHT_WRITE_H
#define INCLUDE_MAIN_DB_LIGHT_WRITE_H

#include "shared.h"

typedef struct Matrix4Block {
    Matrix4 value;
} Matrix4Block;

enum CursorMode {
    CURSOR_MODE_NONE = -1,
    CURSOR_MODE_0,
    CURSOR_MODE_1,
    CURSOR_MODE_2,
    CURSOR_MODE_3,
    CURSOR_MODE_4
};

/* Callback registered on the visualizer cursor by VW_setCursorFunc, invoked
 * with the caller argument stored alongside it. */
typedef void (*CursorCallback)(void *argument);

/*
 * The visualizer cursor: the retail ELF has a GLOBAL OBJECT named cursor at
 * 0x0099C7F0 with size 0x60. VW_setCursorMode writes the mode word at +0,
 * VW_setCursor, VW_getCursor and drawCursor use the position at +0x10, and
 * VW_setCursorFunc stores the callback pair at +0x50/+0x54.
 */
typedef struct CursorState {
    enum CursorMode mode;
    union { int flag_04; int submode; };
    int actor_index;
    int step;
    HomogeneousVector position;
    unsigned char unmodeled_20[0x10];
    int attach_state;
    union { int flag_34; struct Actor *attached_actor; };
    union {
        struct {
            unsigned char unmodeled_38[0x0c];
            float offset_44;
            unsigned char unmodeled_48[8];
        };
        struct {
            unsigned char unmodeled_38_to_40[8];
            HomogeneousVector attach_offset;
        };
    };
    CursorCallback callback;
    void *callback_argument;
    unsigned char unmodeled_58[8];
} CursorState;

extern CursorState cursor;

/* Functions and data used by other translation units. */
void EvtTools(void);

#endif /* INCLUDE_MAIN_DB_LIGHT_WRITE_H */
