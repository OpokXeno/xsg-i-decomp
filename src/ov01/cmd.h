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
/*
 * thinkTurnStartExec (0x00a1dab0) and thinkTurnEndExec (0x00a1db78) read two
 * further shorts of the same entry (lh $3,6($2) and lh $3,8($2)); the
 * remaining four bytes of the stride stay untouched by any function this TU
 * claims.
 */
/*
 * thinkInitExec (0x00a1da30) reads a fourth short of the same entry (lh
 * $2,4($3) at 0x00a1da4c), the current enemy set's optional initialization
 * script offset.
 */
typedef struct MonsTblEntry {
    short script_offset;          /* +0x00 */
    unsigned char unmodeled_2[2]; /* +0x02 */
    short initScript;             /* +0x04: thinkInitExec */
    short turnStartScript;        /* +0x06: thinkTurnStartExec */
    short turnEndScript;          /* +0x08: thinkTurnEndExec */
} MonsTblEntry;

/*
 * Monster-set table base thinkMonsTblGet (0x00a1d790) indexes by monster-set
 * number. Scaffold-owned data (docs/naming.md).
 */
extern MonsTblEntry *pMonsSetTop;

/*
 * The current map's camera-control table entry. camInitExec (0x00a1d820) is
 * the only claimed reader and touches only the initialization script offset
 * (lh $2,2($2) at 0x00a1d82c); the leading two bytes stay untouched by any
 * function this TU claims.
 */
typedef struct CamTopEntry {
    unsigned char unmodeled_0[2];
    short initScript;              /* +0x02: camInitExec */
} CamTopEntry;

/*
 * Camera-control table entry the map's own init data selects. Scaffold-owned
 * data (docs/naming.md).
 */
extern CamTopEntry *pCamTop;

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

/* calcUPGet's return type comes from its definer, ov01/tu004 calc.c
 * (published include/ov01/calc.h); calcUPGet itself is still asm there, so
 * it is redeclared here the same way src/ov01/snd.c, src/ov01/menu.c and
 * src/ov01/battle_init.c already do. */
#include "ov01/calc.h"
extern CalcUnitParam *calcUPGet(ObjectTask *unit);

/*
 * CalcUnitParam is owned by a different TU (ov01/tu004 calc.c) and is not
 * completed here. thinkUnitAtkExec (0x00a1dc40) reads a signed half two
 * levels down: the pointer this offset holds (into calcUPGet's unmodeled_44
 * span) points at one entry of this TU's own attack table (cmdAtktbl/
 * cmdAtkset/cmdAtktblSort/cmdAtktblSortSub/cmdAtktblSearch below, all still
 * asm), whose script-offset field thinkUnitAtkExec is the first claimed
 * reader of.
 */
#define CALC_UNIT_PARAM_ATK_TBL_OFF 0x48

/*
 * thinkUnitDmgExec (0x00a1dd28) reads a second signed half of the same
 * attack-table entry, immediately after script_offset: the script offset it
 * runs when the unit takes damage instead of when it attacks.
 */
typedef struct AtkTblEntry {
    unsigned char unmodeled_0[4];
    short script_offset;   /* +0x4: thinkUnitAtkExec */
    short dmgScript;        /* +0x6: thinkUnitDmgExec */
} AtkTblEntry;

/*
 * thinkUnitDmgExec's second argument: attacker (+0x0) is the other unit
 * involved, forwarded through unitNoGet the same way unit itself is;
 * reg8015/reg8016/reg8017 (+0x4/+0x8/+0xC) are unsigned halves forwarded
 * unchanged into the identically-numbered AI script registers, with no
 * further evidenced role (scenarioBatEnd, the only claimed caller, is still
 * asm).
 */
typedef struct UnitDmgInfo {
    ObjectTask *attacker;         /* +0x0 */
    unsigned short reg8015;       /* +0x4 */
    unsigned char unmodeled_6[2];
    unsigned short reg8016;       /* +0x8 */
    unsigned char unmodeled_A[2];
    unsigned short reg8017;       /* +0xC */
} UnitDmgInfo;

/*
 * CalcUnitParam is owned by a different TU (ov01/tu004 calc.c) and is not
 * completed here, the same way src/ov01/unit_cmd.c's
 * CALC_UNIT_PARAM_UNK44_OFF and src/ov01/battle_init.c's
 * CALC_PARAM_BOOST_*_OFF already read other bytes of its unmodeled spans.
 * cmdHpPerGet (0x00a22028) reads a current-HP short at +0x34 (inside
 * calcUPGet's unmodeled_1e span) and a max-HP short at +0x00 (inside its
 * unmodeled_00 span) to compute a percentage.
 */
#define CALC_UNIT_PARAM_HP_OFF 0x34
#define CALC_UNIT_PARAM_MAX_HP_OFF 0x00

/*
 * The battle actor object behind ObjectTask.work, as far as cmdStatChk
 * reads it. Several TUs already model the same real object under other
 * tags with more members evidenced (src/ov01/calc.h's CalcActorRecord,
 * src/ov01/unit_cmd.h's Actor); this TU names only the one flags word it
 * touches (bit-tested against the script operand).
 */
typedef struct CmdActorFlags {
    int flags;   /* +0x00 */
} CmdActorFlags;

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
/*
 * cmdWaitCnt (0x00a21dc8) reuses +0x14 as its own down-counter (initialized
 * from its script operand on the first dispatch, decremented on every later
 * poll) and cmdWaitCamMove (0x00a21e30) reuses it as a 0/1 camera-moving
 * selector it passes to MCamIsMoving; neither is the sort mode
 * cmdAtktblSort/cmdAtktblSortRev store there.
 */
/*
 * This is the same processBuf entry thinkProcessExec (0x00a1d4f8) walks and
 * hands to thinkProcessExecSub (0x00a1d558): +0x0 is the process's saved AI
 * script data-segment base (thinkTopSet's argument), zero when the slot is
 * free (thinkProcessDel clears it) and nonzero while a process is live;
 * +0x8 is its saved AI execution context (thinkContextSet's argument).
 */
typedef struct ThinkProcess {
    int dataTop;                    /* +0x0: thinkProcessExec/thinkProcessExecSub */
    short pc;                      /* +0x4 */
    unsigned char unmodeled_6[2];
    int context;                    /* +0x8: thinkProcessExecSub */
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
extern int cmdThinksetSub(ThinkProcess *proc);
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

/*
 * thinkExec (this TU, still asm) runs the AI script at the given offset;
 * thinkTurnStartExec/thinkTurnEndExec/thinkUnitAtkExec (0x00a1dab0,
 * 0x00a1db78, 0x00a1dc40) tail-call it with the monster-set/attack-table
 * script offset they just read. unitLiveNumGet (src/ov01/battle_init.c,
 * still asm) returns the live unit count for a side (0 or 1); the same three
 * functions pass it to thinkRegSet for AI script registers 0x8019/0x801A.
 */
extern int thinkExec(short scriptOffset);
extern int unitLiveNumGet(int side);

/* "** thinkTurnStart ** %X\n" at 0x00a46160 (ov01 .rodata), the format
 * string thinkTurnStartExec (0x00a1dab0) hands to printf with the script
 * offset it is about to run. */
extern const char D_00A46160[];

/* "** thinkTurnEnd ** %X\n" at 0x00a46180 (ov01 .rodata), the format string
 * thinkTurnEndExec (0x00a1db78) hands to printf with the script offset it is
 * about to run. */
extern const char D_00A46180[];

/* "** think Atk (%d) ** %X\n" at 0x00a46198 (ov01 .rodata), the format
 * string thinkUnitAtkExec (0x00a1dc40) hands to printf with the acting
 * unit's number and the script offset it is about to run. */
extern const char D_00A46198[];

/* "** think Dmg (%d) ** %X\n" at 0x00a461b8 (ov01 .rodata), the format
 * string thinkUnitDmgExec (0x00a1dd28) hands to printf with the damaged
 * unit's number and the script offset it is about to run. */
extern const char D_00A461B8[];

/*
 * cmdUnitpara (this TU, still asm) reads (mode 0) or writes (mode 1) one of
 * a unit's AI-visible parameters selected by category; cmdUnitparaGet
 * (0x00a1f508) and cmdUnitparaSet (0x00a1f5b0) are its only claimed callers.
 */
extern int cmdUnitpara(ObjectTask *unit, int category, int value, int mode);

/*
 * cmdTecpara/dataTecGet (this TU and src/ov01/data_unit_org_get.c, both
 * still asm) look up a technique's parameter; cmdTecparaGet (0x00a20518) is
 * the only claimed caller here.
 */
extern int cmdTecpara(int tec, int category);
extern int dataTecGet(int tecId);

/*
 * mapMulSet's own parameter block is a different TU's union
 * (MapMultiplier, src/ov01/map_disp.h, ov01/tu010) that a TU-local header
 * cannot restate; cmdMapMulSet (0x00a21f38) only ever writes four
 * consecutive scaled floats into it, the same layout as Vector4, so it is
 * declared with that shared, layout-compatible type instead.
 */
extern void mapMulSet(Vector4 *value);

/*
 * cmdCamOffsAng (0x00a21548) writes its own three-float block right after
 * offset below, using the same +0x70 enable flag (set to 2 here, instead of
 * the 1 cmdCamOffsPos uses): x is scaled by 0.01 like offset.x/y/z, while y
 * and z convert their operand from degrees to radians before storing it.
 */
typedef struct McamAngleOffset {
    float x;   /* +0x00: scaled by 0.01 */
    float y;   /* +0x04: degrees converted to radians */
    float z;   /* +0x08: degrees converted to radians */
} McamAngleOffset;

/*
 * mcamPtrGet (this TU, still asm) returns the camera-offset parameter block
 * cmdCamOffsPos (0x00a21458) fills in; only the members that function
 * touches are modeled. cmdCamOffsAng (this TU, still asm) reaches the same
 * +0x70 enable flag for its own angle block, so it stays a separate,
 * unmodeled span here.
 */
typedef struct McamParams {
    unsigned char unmodeled_0[0x70];
    int offsEnabled;                  /* +0x70 */
    unsigned char unmodeled_74[0x80 - 0x74];
    Vector4 offset;                   /* +0x80: x/y/z scaled by 0.01, w fixed at 1.0 */
    McamAngleOffset angleOffset;      /* +0x90: cmdCamOffsAng */
} McamParams;

extern McamParams *mcamPtrGet(void);

/*
 * StudioLight's ambient_color (include/main/xgl_studio.h, main/tu101) is the
 * quadword xglLightIntensityAmbient (src/main/xgl_light.c, still asm there)
 * copies in verbatim; cmdLightAmb (0x00a20840) is the only claimed caller
 * here.
 */
#include "main/xgl_studio.h"
extern void xglLightIntensityAmbient(StudioLight *light, Vector4 *ambient);

/*
 * xglLightIntensityParallel/xglLightDirection (src/main/xgl_light.c,
 * main/tu101) set one of a light block's three parallel entries' color or
 * direction by index; cmdLightCol (0x00a20918) and cmdLightDir (0x00a20a18)
 * are this TU's only callers. That TU still models the same light block
 * under its own TU-local tag (XglLightSet, src/main/xgl_light.h, not yet
 * published to include/main), which this TU cannot restate; xglLightDirection
 * is already accepted there with that tag, so both declarations here reuse
 * StudioLight instead, the same already-published, layout-compatible type
 * cmdLightAmb above already gets from xglStudioGetLight (the same
 * substitution mapMulSet's parameter block above uses for a different TU's
 * type).
 */
extern void xglLightIntensityParallel(StudioLight *light, unsigned int index, const Vector4 *color);
extern void xglLightDirection(StudioLight *light, unsigned int index, const Vector4 *direction);

#endif /* SRC_OV01_CMD_H */
