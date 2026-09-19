/*
 * TU-local declarations of ov01/tu006 (src/ov01/cmd.c).
 */

#ifndef SRC_OV01_CMD_H
#define SRC_OV01_CMD_H

#include "shared.h"

/*
 * The AI process table thinkSysInit (0x00a1d348) clears with a 0x240-byte
 * memset; no function this TU claims establishes any member layout for it
 * yet, so it stays a byte range. Scaffold-owned data (docs/naming.md).
 */
extern unsigned char processBuf[0x240];

/*
 * The AI work area thinkSysInit (0x00a1d348) clears with a 0x1D0-byte memset,
 * immediately after processBuf. No function this TU claims establishes any
 * member layout for it either. Scaffold-owned data (docs/naming.md).
 */
extern unsigned char context[0x1D0];

/*
 * AI script data-segment base address: thinkTopSet (0x00a1d380) installs it
 * and thinkAdrGet (0x00a1d3a0) adds a script offset to it. Scaffold-owned
 * data (docs/naming.md).
 */
extern int pDataTop;

/*
 * AI execution-context value that thinkContextSet (0x00a1d390) installs
 * (sw $4 at 0x00a1d398); no function this TU claims reads it back, so its
 * use beyond storage is not yet evidenced. Scaffold-owned data
 * (docs/naming.md).
 */
extern int pContext;

/*
 * AI script data-segment base thinkMonsTblGet (0x00a1d790) installs through
 * thinkTopSet before it looks up a monster-set entry. Scaffold-owned data
 * (docs/naming.md).
 */
extern int pThinkTop;

/*
 * One entry of the monster-set table pMonsSetTop below indexes. thinkMonsTblGet
 * (0x00a1d790) is the only claimed reader and touches only the leading
 * script_offset field (lh $4,0($2) at 0x00a1d7cc); the remaining eight bytes
 * of the 10-byte stride (sll $2,$16,0x2 / addu $2,$2,$16 / sll $2,$2,0x1 at
 * 0x00a1d7c0..0x00a1d7c8, i.e. index*4 + index, doubled) are untouched by any
 * function this TU claims.
 */
typedef struct MonsTblEntry {
    short script_offset;          /* +0x00 */
    unsigned char unmodeled_2[8]; /* +0x02 */
} MonsTblEntry;

/*
 * Monster-set table base thinkMonsTblGet (0x00a1d790) indexes by monster-set
 * number. Scaffold-owned data (docs/naming.md).
 */
extern MonsTblEntry *pMonsSetTop;

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

/*
 * Copies a camera parameter block into the camera state for one of four modes
 * (src/ov01/m_cam.c, 0x00a31920; still INCLUDE_ASM there): its own body
 * dispatches on $a0 = 0..3 (anything else reaches MOutputDebugStringWarn) and
 * copies from the block $a1 points at. myMCamSet forwards both of its own
 * arguments unchanged through the tail jump at 0x00a20bf8; its caller
 * cmdCamSetPos passes the camMode mode and a parameter block. The block's
 * layout is not modeled yet, hence void *.
 */
void MCamSet(int mode, void *params);

/* "** unit No err -> %d\n" at 0x00a46578 (ov01 .rodata), the format string
 * unitNoChk hands to printf when its argument is out of the zero-to-seven
 * unit range. */
extern int printf(const char *format, ...);
extern const char D_00A46578[];

/*
 * Camera reference/view mode selector. cmdCamRefMode/cmdCamCamMode/
 * cmdCamRefMoveMode/cmdCamCamMoveMode below write 1, 0, -2 and -1 to it in
 * turn; scaffold-owned data under its original ELF symbol name
 * (docs/naming.md).
 */
extern int camMode;

/*
 * Perspective-camera mode selector. cmdCamPersMode/cmdCamPersMoveMode below
 * write 3 and -3 to it; scaffold-owned data (docs/naming.md).
 */
extern int persMode;

/*
 * Bank-camera mode selector. cmdCamBankMode/cmdCamBankMoveMode below write 2
 * and -4 to it; scaffold-owned data (docs/naming.md).
 */
extern int bankMode;

/*
 * The current AI unit. thinkUnitPtrGetReg (0x00a1f348) substitutes it for
 * unitNoGet when a script register holds the self-reference sentinel 0x7FFF.
 * Scaffold-owned data under its original ELF symbol name (docs/naming.md).
 */
extern ObjectTask *pThinkUnit;

/*
 * unitNoGet/unitPtrGet (src/ov01/unit_cmd.c, still asm) search and index the
 * eight-slot unit table that TU owns: unitNoGet returns the slot whose
 * stored pointer equals its argument (-1 and a printed "unit No err" message
 * if none does) and unitPtrGet returns the pointer stored at a slot (0 and
 * its own printed message if the slot is out of range). thinkUnitPtrGetReg
 * (0x00a1f348) chains unitNoGet's result straight into unitPtrGet, and
 * cmdLineChk (0x00a1f490) chains unitPtrGet's result straight into
 * calcLineChk below, which forwards it unchanged into calcUPGet
 * (ObjectTask *) -- fixing both as ObjectTask *.
 */
extern int unitNoGet(ObjectTask *unit);
extern ObjectTask *unitPtrGet(int unitNo);

/*
 * calcLineChk (src/ov01/calc.c, still asm) takes the unit pointer
 * unitPtrGet returns and reports whether that unit has reached its line
 * (nonzero) or not (0); cmdLineChk (0x00a1f490) is the only claimed caller.
 */
extern int calcLineChk(ObjectTask *unit);

/*
 * monsSetNoGet (src/ov01/battle_init.c, still asm) returns the active
 * monster-set number; it takes no arguments.
 */
extern int monsSetNoGet(void);

/*
 * The AI script command argument the interpreter (thinkExec and friends,
 * still asm) hands to every opcode handler in this TU's cmd* family:
 * cmdLineChk, cmdAtktblSort, cmdAtktblSortRev, cmdMonsSetNoGet and
 * cmdAtktblSortSub (this TU, still asm) all take the same pointer. Each
 * handler reads a 16-bit operand from the running AI script through the
 * cursor at +0x4 (thinkAdrGet(cursor) resolves it to an address) and
 * advances the cursor by 2 per operand it consumes; cmdAtktblSort and
 * cmdAtktblSortRev additionally write the sort mode at +0x14 that
 * cmdAtktblSortSub reads back (0 from cmdAtktblSort, 1 from
 * cmdAtktblSortRev). Bytes no claimed function touches stay unmodeled_.
 */
/*
 * cmdWaitMsg (0x00a21e90, this TU) tests this field to tell a wait command's
 * first dispatch (its operand not read yet) from a later poll of the same
 * process; cmdWaitEvent, cmdWaitCnt and cmdWaitCamMove (this TU, still asm)
 * test the same offset the same way for their own wait conditions. No
 * claimed function establishes what sets it.
 */
typedef struct ThinkProcess {
    unsigned char unmodeled_0[4];
    short pc;                      /* +0x4 */
    unsigned char unmodeled_6[0xC - 0x6];
    int waitActive;                /* +0xC */
    unsigned char unmodeled_10[0x14 - 0x10];
    int sortMode;                  /* +0x14 */
} ThinkProcess;

/*
 * thinkRegNo/thinkRegGet/thinkRegSet/cmdNum/cmdThinksetSub/cmdAtktblSortSub
 * are this TU's own siblings, still asm; forward-declared here for the
 * functions above that call them.
 */
extern short thinkRegNo(short pc);
extern int thinkRegGet(int regIndex);
extern void thinkRegSet(int regIndex, int value);
extern int cmdNum(short pc);
extern void cmdThinksetSub(ThinkProcess *proc);
extern void cmdAtktblSortSub(ThinkProcess *proc);

/*
 * thinkProcessKindChk (0x00a1d4b8, this TU, still asm) walks the AI process
 * table thinkSysInit clears (sixteen 0x24-byte entries starting at
 * processBuf) and counts the live entries (offset 0 nonzero) whose kind
 * field (offset 0x10) equals its argument. thinkExecChk (0x00a1ded0) passes
 * kind 0 and reports whether that count is zero.
 */
extern int thinkProcessKindChk(int kind);

/* "** reg err %d\n" at 0x00a461e8 (ov01 .rodata), the format string regChk
 * (0x00a1dfc8) hands to printf when the operand's high bit (0x8000) is not
 * set. */
extern const char D_00A461E8[];

/*
 * dataVPadSet (src/ov01/data_unit_org_get.c, still asm) installs the
 * decoded virtual-pad mask; cmdVPadSet (0x00a21ef8) is the only claimed
 * caller here.
 */
extern void dataVPadSet(int padMask);

/*
 * menuTimeGet (src/ov01/menu.c) returns the current battle event-timer
 * slot; cmdEventSlotGet (0x00a220e8) is the only claimed caller here.
 */
extern int menuTimeGet(void);

/*
 * cfEncountGet (src/ov01/battle_init.c, still asm) returns the active
 * encounter-configuration flags; cmdCfEncountGet (0x00a22250) is the only
 * claimed caller here.
 */
extern int cfEncountGet(void);

/*
 * cfEventGet (src/ov01/battle_init.c, still asm) returns the active
 * event-configuration flags; cmdCfEventGet (0x00a222a8) is the only claimed
 * caller here.
 */
extern int cfEventGet(void);

/*
 * thinkNoGet (src/ov01/battle_init.c) returns the current AI script
 * number; cmdThinkNoGet (0x00a22300) is the only claimed caller here.
 */
extern int thinkNoGet(void);

/*
 * Camera table index. cmdCamTblSet (0x00a21c48, this TU) sets it to the
 * decoded script operand when that value is below thinkCamTblNumGet's
 * count; scaffold-owned data under its original ELF symbol name
 * (docs/naming.md).
 */
extern short camTblIdx;

/*
 * thinkCamTblNumGet (this TU, still asm) returns the camera-table entry
 * count that cmdCamTblSet (0x00a21c48) compares the decoded index against.
 */
extern int thinkCamTblNumGet(void);

/*
 * MCamStopMove (src/ov01/m_cam.c, still asm) stops camera movement;
 * cmdCamMoveStop (0x00a21ca0) is the only claimed caller here.
 */
extern void MCamStopMove(int stop);

/*
 * dataVPadModeSet (src/ov01/data_unit_org_get.c, still asm) enables or
 * disables virtual-pad input; cmdVPadEneble/cmdVPadDisable (0x00a21eb8,
 * 0x00a21ed8) are its only claimed callers here.
 */
extern void dataVPadModeSet(int enable);

/*
 * "@" at 0x00a46300 (ov01 .rodata), the message cmdRput (0x00a1ead8) prints
 * when the decoded register index requests indirect addressing (bit 0x4000
 * set).
 */
extern const char D_00A46300[];

/*
 * "R%02d = %d\n" at 0x00a46308 (ov01 .rodata), the format string cmdRput
 * (0x00a1ead8) hands to printf with the register index and its value.
 */
extern const char D_00A46308[];

/*
 * Script command-trace flag. cmdCmdputon/cmdCmdputoff (0x00a1ed10,
 * 0x00a1ed28) set and clear it; scaffold-owned data under its original ELF symbol
 * name (docs/naming.md).
 */
extern int cmdPutFlag;

#endif /* SRC_OV01_CMD_H */
