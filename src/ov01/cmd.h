/*
 * TU-local declarations of ov01/tu006 (src/ov01/cmd.c).
 */

#ifndef SRC_OV01_CMD_H
#define SRC_OV01_CMD_H

#include "shared.h"

typedef struct MessageTask MessageTask;

/*
 * The battle message task.  battleMsgPut (0x00a1ee90) and battleMsgPut2
 * (0x00a1ef80) both take a node from objEntryPure (0x00a004c8), which allocates
 * straight out of taskMan with xglTaskEntryNext and stores the callback in the
 * scheduler's own prefix, so the only member either entry point establishes
 * beyond that prefix is the frame count at +0x28: battleMsgPut writes its
 * fourth argument there (sw $16,0x28 at 0x00a1eed0) and msgObj counts it down
 * once per visit, closing the window when it reaches zero.  battleMsgPut2 does
 * not write it at all, which is why msgObj2 asks the window itself instead.
 * Nothing here reads the 24 bytes before it -- objEntryPure fetches no work
 * buffer, so the object-task work pointer at +0x10 is not established for this
 * node either -- and they stay a byte range.
 */
struct MessageTask {
    XglTaskPrefix task;              /* +0x00 */
    unsigned char unmodeled_10[24];  /* +0x10 */
    int remaining_frames;            /* +0x28 */
};

void msgObj(MessageTask *task);

extern void eBattleWinClose2(void);

extern void eBattleWinMain2(void);

extern MessageTask *pMsgObj;

void msgObj2(MessageTask *task);

extern int eBattleWinPageCheck4(void);

extern void eBattleWinClose4(void);

extern void eBattleWinMain4(void);

#endif /* SRC_OV01_CMD_H */
