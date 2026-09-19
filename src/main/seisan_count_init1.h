/*
 * TU-local declarations of main/tu172 (src/main/seisan_count_init1.c).
 */

#ifndef SRC_MAIN_SEISAN_COUNT_INIT1_H
#define SRC_MAIN_SEISAN_COUNT_INIT1_H

#include "shared.h"

void tskTskMain2(TskObject *task);

/*
 * The settlement (seisan) screen's state bytes. subSeisanMain (0x002a15c0)
 * drives them as a state machine:
 *   [1] the current state: subSeisanMain copies the requested state at [3]
 *       into it when they differ (lbu 0x3/lbu 0x1/sb 0x1 at
 *       0x002a15cc..0x002a15dc), and subSeisanCheck and SeisanMain switch on
 *       or test it (0x002a14d0, 0x002a18d8).
 *   [2] flags: bit 0 is set on the frame the state changes and cleared
 *       otherwise (0x002a15e4..0x002a15f4); subSeisanCheck sets bit 2 when no
 *       check selected a state of its own.
 *   [4] the next state: subSeisanCheck stores each check's result here
 *       (sb 0x4 at 0x002a1524..0x002a1584) and subSeisanMain copies it into
 *       the requested state at [3] (lbu 0x4/sb 0x3 at 0x002a170c/0x002a1714).
 * Only these bytes are named here; the work area itself is not modeled.
 */
extern unsigned char *SeisanWork;

#define SEISAN_WORK_STATE 1
#define SEISAN_WORK_FLAGS 2
#define SEISAN_WORK_NEXT_STATE 4

/*
 * The closing state subSeisanCheck selects when no check selected one:
 * subSeisanMain's case for it (li 0xf0 at 0x002a166c) waits 0x10 frames,
 * calls MenuBgTaskBreak and then sets the current and requested states to
 * 0xff, the finished state SeisanMain reports as "no longer running".
 */
#define SEISAN_STATE_CLOSING 0xf0
#define SEISAN_STATE_FINISHED 0xff

extern int subSeisanHissatuCheck00(unsigned char *seisan_work);
extern int subSeisanHissatuCheck01(void);
extern void subSeisanMain(void);

#endif /* SRC_MAIN_SEISAN_COUNT_INIT1_H */
