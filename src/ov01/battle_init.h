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
/*
 * The one exception: objDispCur (0x00a106a4) loads the state record with
 * `lw $3,0x10($17)` from the cursor and its flags with `lw $2,0x0($3)`;
 * the state record also carries selectActive (+0x78) and the transform
 * block (+0x90..+0x110) the cursor functions read, each cited at its
 * member. The cursor record claims no size; unmodeled_00 spans just the
 * bytes before +0x10 (the state record's modelled span ends at +0x110).
 */
struct CursorObject {
    unsigned char unmodeled_00[0x10];
    CursorCommandState *state; /* +0x10 (CURSOR_STATE_OFF) */
};

/*
 * The cursor's transform block at state+0x90: the 0x80 bytes curPutSub
 * saves and restores with aligned ld/sd loops (0x00a10914, 0x00a10a30) around
 * grCalcMatrix, the same TransformState tail src/ov01/gr_gp_init.c reads
 * (position +0x90, offset +0xa0, rotation +0xb0, scale +0xc0) and
 * src/ov01/menu.c's MenuCursorTransform restates. curPutSub writes
 * position.y (sw $0,0x4), offset.y (swc1 0x14), rotation.y (swc1 0x24) and
 * scale x/y/z (swc1 0x30/0x34/0x38). The u64 view gives the eight-byte
 * alignment the ld/sd copy needs.
 */
typedef union CursorTransform {
    struct {
        Vector4 position; /* +0x00 */
        Vector4 offset;   /* +0x10 */
        Vector4 rotation; /* +0x20 */
        Vector4 scale;    /* +0x30 */
        unsigned char unmodeled_40[0x60 - 0x40];
        Vector4 goal;     /* +0x60: where curMove2Pos sends the cursor
                           * (sefGetNullPosition into state+0xF0 at
                           * 0x00a0fb34; swc1 0x60/0x64/0x68($17)) */
        unsigned char unmodeled_70[0x80 - 0x70];
    } fields;
    u64 storage[0x80 / sizeof(u64)];
} CursorTransform;

struct CursorCommandState {
    unsigned int flags; /* +0x00 (STATE_FLAGS_OFF) */
    unsigned char unmodeled_04[0x78 - 0x04];
    int selectActive; /* +0x78: objDispEnCur draws the target's name only
                       * while 1 (`lw $16,0x78($2)` at 0x00a105c0) */
    unsigned char unmodeled_7c[0x90 - 0x7C];
    CursorTransform transform; /* +0x90 */
};

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
    /* unsigned: objCur's switch tests it with `beql $3,$0` and `sltiu
     * $2,$3,0x4` (0x00a0f834/0x00a0f83c); as int it compiled to `blezl`
     * and `slti`. */
    unsigned int selector;
    int argument;      /* +0x04: a position id, or a BattleUnit * (cast)
                        * for the unit commands objCur handles */
    int word08;
    int word0c;
    int word10;
};

/* from src/math/ov01/w3-00a0f900-curcmdset/curCmdSet.c (unit math-ov01-w3-00a0f900, function curCmdSet @ 0x00a0f900) */
void curCmdSet(CursorObject *cursor, const CursorCommand *command);

/* from src/math/ov01/w3-00a0f900-curcmdset/private.h (unit math-ov01-w3-00a0f900, function curCmdSet @ 0x00a0f900) */
extern CursorCommand *objCmdTailGet(CursorObject *cursor);

#endif /* SRC_OV01_BATTLE_INIT_H */
