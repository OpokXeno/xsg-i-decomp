/*
 * TU-local declarations of ov01/tu003 (src/ov01/battle_init.c).
 */

#ifndef SRC_OV01_BATTLE_INIT_H
#define SRC_OV01_BATTLE_INIT_H

/* from src/math/ov01/w3-00a0f900-curcmdset/private.h (unit math-ov01-w3-00a0f900, function curCmdSet @ 0x00a0f900) */
typedef struct CursorObject CursorObject;

/* from src/math/ov01/w3-00a0f900-curcmdset/private.h (unit math-ov01-w3-00a0f900, function curCmdSet @ 0x00a0f900) */
typedef struct CursorCommandState CursorCommandState;

/* from src/math/ov01/w3-00a0f900-curcmdset/private.h (unit math-ov01-w3-00a0f900, function curCmdSet @ 0x00a0f900) */
typedef struct CursorCommand CursorCommand;

/* from src/math/ov01/w3-00a0f900-curcmdset/private.h (unit math-ov01-w3-00a0f900, function curCmdSet @ 0x00a0f900) */
/*
 * Opaque state/message handles. The ELF symbol table proves the function
 * names only; no debug record proves the historical struct layouts, so no
 * member names or padding extents are declared here. Word offsets below are
 * the only evidenced facts (each observed as an explicit load/store width
 * in the original body) and are documented at the accessors in candidate.c.
 */
struct CursorObject;

/* from src/math/ov01/w3-00a0f900-curcmdset/private.h (unit math-ov01-w3-00a0f900, function curCmdSet @ 0x00a0f900) */
struct CursorCommandState;

/* from src/math/ov01/w3-00a0f900-curcmdset/private.h (unit math-ov01-w3-00a0f900, function curCmdSet @ 0x00a0f900) */
/*
 * Twenty-byte queue record. Only the total size (20 bytes: ldl/ldr + sdl/sdr
 * pairs for bytes 0..15 plus the word at 0x10) and the first two word roles
 * are evidenced:
 *   +0x00 selector: branched against zero, callers pass 1, 2 or 3;
 *   +0x04 argument: copied word, caller-supplied value;
 *   +0x08/+0x0C/+0x10 tail words: copied but otherwise unresolved, so they
 *   are named as opaque words without invented field meanings.
 */
struct CursorCommand {
    int selector;
    int argument;
    int word08;
    int word0c;
    int word10;
};

/* from src/math/ov01/w3-00a0f900-curcmdset/curCmdSet.c (unit math-ov01-w3-00a0f900, function curCmdSet @ 0x00a0f900) */
void curCmdSet(CursorObject *cursor, const CursorCommand *command);

/* from src/math/ov01/w3-00a0f900-curcmdset/private.h (unit math-ov01-w3-00a0f900, function curCmdSet @ 0x00a0f900) */
extern CursorCommand *objCmdTailGet(CursorObject *cursor);

#endif /* SRC_OV01_BATTLE_INIT_H */
