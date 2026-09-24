#ifndef CCU_038_EXEC_SUB_H
#define CCU_038_EXEC_SUB_H

#include "ov10/cgp.h"

/* Takes the work area untyped and converts it here: reading the member through
 * the caller's own pointer changes loop strength reduction in two of
 * CardPlayCommandPlay's search loops (commands 39 and 78). */
static inline CardDefinition *CardDefinitionsFor(void *gameWork) {
    CardGameWork *work = gameWork;
    return work->definitions;
}

static inline CardCursorControl *CardCursorFor(CardGameWork *work) {
    return &work->cursor;
}

static inline void CardCursorSetSelectionMode(CardGameWork *work, int mode) {
    work->cursor.selectionMode = mode;
}

static inline void CardCursorSetSide(CardGameWork *work, int side) {
    work->cursor.controllingSide = side;
}

static inline int CardCommandStep(CardGameWork *work) {
    return work->command.step;
}

static inline void CardCommandSetStep(CardGameWork *work, int step) {
    work->command.step = step;
}

static inline void CardCommandAdvance(CardGameWork *work) {
    work->command.step++;
}

static inline void CardCommandRewind(CardGameWork *work) {
    work->command.step--;
}

static inline int CardCommandPhase(CardGameWork *work) {
    return work->command.phase;
}

static inline void CardCommandMarkRecovery(CardGameWork *work) {
    work->command.recoveryApplied = 1;
}

static inline u16 CardCommandHeld(CardGameWork *work, unsigned side) {
    if (side == 0)
        return work->command.input[0].held;
    return work->command.input[1].held;
}

static inline u16 CardCommandPressed(CardGameWork *work, unsigned side) {
    if (side == 0)
        return work->command.input[0].pressed;
    return work->command.input[1].pressed;
}

static inline u16 CardCommandRepeated(CardGameWork *work, unsigned side) {
    if (side == 0)
        return work->command.input[0].repeat;
    return work->command.input[1].repeat;
}

#endif
