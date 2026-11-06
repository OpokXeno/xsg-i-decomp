/*
 * TU-local declarations of ov01/tu006 (src/ov01/cmd.c).
 */

#ifndef SRC_OV01_CMD_H
#define SRC_OV01_CMD_H

#include "shared.h"
#include "ov01/battle_init.h"

typedef struct ThinkContext ThinkContext;
typedef struct ThinkMonsEntry ThinkMonsEntry; /* defined by src/ov01/entry_first_init.h */

/* Script commands run through cmdTbl; command numbers below this dispatch. */
#define THINK_COMMAND_COUNT 0x63

/* Sizes of the process table and of the context's return and value stacks. */
#define THINK_PROCESS_COUNT 16
#define THINK_RETURN_STACK_SIZE 16
#define THINK_VALUE_STACK_SIZE 32

/*
 * Script register ids (thinkRegGet/thinkRegSet operands). 0x8000 marks a
 * register operand in a script word (cmdNum tests it).
 */
#define THINK_OPERAND_IS_REG 0x8000
#define THINK_REG_UNIT 0x8010       /* acting unit number (thinkUnitPtrGetReg) */
#define THINK_REG_COMMAND 0x8011    /* thinksetSub command */
#define THINK_REG_ARG 0x8012        /* thinksetSub argument, atkset/unit slot */
#define THINK_REG_TARGET 0x8013     /* thinksetSub target unit */
#define THINK_REG_ATTACKER 0x8014   /* attacking unit number (info->attacker) */
#define THINK_REG_CAMERA_FLAG 0x801B /* cameraFlagGet result */
#define THINK_REG_LIVE_NUM_SIDE0 0x8019 /* unitLiveNumGet(0) result */
#define THINK_REG_LIVE_NUM_SIDE1 0x801A /* unitLiveNumGet(1) result */
#define THINK_REG_ACCEPT 0x8018     /* nonzero when the script accepts an atktbl pair (cmdAtktblSearch) */

/* The process table saves each script base, program counter and context. */
typedef struct ThinkProcess {
    int dataTop;
    short pc;
    unsigned char unmodeled_6[2];
    ThinkContext *context;
    int waitActive;
    int kind;
    int sortMode;
    unsigned char unmodeled_18[0x24 - 0x18];
} ThinkProcess;

/* Keep the table before the following work area in the original storage order. */
static ThinkProcess processBuf[THINK_PROCESS_COUNT];

/*
 * The AI work area thinkSysInit (0x00a1d348) clears with a 0x1D0-byte memset,
 * immediately after processBuf. No function this TU claims establishes any
 * member layout for it either. C-owned data (docs/naming.md).
 */

/*
 * AI script data-segment base address: thinkTopSet (0x00a1d380) installs it
 * and thinkAdrGet (0x00a1d3a0) adds a script offset to it. C-owned
 * data (docs/naming.md).
 */
static int pDataTop;

/*
 * AI execution context thinkContextSet (0x00a1d390) installs; the script
 * register file (thinkRegGet, thinkRegSet) and the return and value stacks
 * (cmdGosub, cmdReturn, cmdRpush, cmdRpop) all read it through this pointer.
 * C-owned data (docs/naming.md).
 */
static ThinkContext *pContext;

/*
 * The AI execution context pContext points at. cmdGosub (0x00a1e210) and
 * cmdReturn (0x00a1e298) push and pop script return addresses on a
 * sixteen-entry stack at +0x80 (sh $3,0x80($2)) whose depth word sits at
 * +0xA0 (lw $2,0xA0($5), slti $2,$2,0x10); cmdRpush (0x00a1e878) and cmdRpop
 * (0x00a1e900) do the same for register values on a thirty-two-entry stack
 * at +0xA4 (sh $8,0xA4($3)) with its depth at +0xE4 (slti $3,$3,0x20).
 */
typedef struct ThinkContext {
    unsigned char unmodeled_0[0x80];
    short returnStack[THINK_RETURN_STACK_SIZE];  /* +0x80: cmdGosub/cmdReturn */
    int returnDepth;                /* +0xA0 */
    short valueStack[THINK_VALUE_STACK_SIZE];    /* +0xA4: cmdRpush/cmdRpop */
    int valueDepth;                 /* +0xE4 */
} ThinkContext;

static ThinkContext context[2];

/*
 * AI script data-segment base thinkMonsTblGet (0x00a1d790) installs through
 * thinkTopSet before it looks up a monster-set entry. C-owned data
 * (docs/naming.md).
 */
static int pThinkTop;

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
    short mapNo;                  /* +0x02: thinkInit */
    short initScript;             /* +0x04: thinkInitExec */
    short turnStartScript;        /* +0x06: thinkTurnStartExec */
    short turnEndScript;          /* +0x08: thinkTurnEndExec */
} MonsTblEntry;

/*
 * Monster-set table base thinkMonsTblGet (0x00a1d790) indexes by monster-set
 * number. C-owned data (docs/naming.md).
 */
static MonsTblEntry *pMonsSetTop;

/* Spawn table of monster set monsSetNo (entry_first_init.c reads it). */
extern ThinkMonsEntry *thinkMonsTblGet(int monsSetNo);

/*
 * The current map's camera-control table entry. camInitExec (0x00a1d820)
 * reads the initialization script offset (lh $2,2($2) at 0x00a1d82c).
 */
typedef struct CamTopEntry {
    short camTblOffset;            /* +0x00: camInit (0x00a1d6e8, this TU) is the only claimed reader (lh $4,0x0($2) at 0x00a1d728), passing it to thinkAdrGet to install pCamSetTop */
    short initScript;              /* +0x02: camInitExec */
} CamTopEntry;

/*
 * Camera-control table entry the map's own init data selects. C-owned
 * data (docs/naming.md).
 */
static CamTopEntry *pCamTop;

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
    ObjectTask task;                 /* +0x00: the object task prefix (work at +0x10 unestablished) */
    unsigned char unmodeled_14[0x28 - 0x14];
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
 * (src/ov01/m_cam.c, 0x00a31920): its own body
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
extern const char cmdUnitNoErrText[];

/*
 * Camera reference/view mode selector. cmdCamRefMode/cmdCamCamMode/
 * cmdCamRefMoveMode/cmdCamCamMoveMode below write 1, 0, -2 and -1 to it in
 * turn; C-owned data under its original ELF symbol name
 * (docs/naming.md).
 */
static int camMode;

/*
 * Perspective-camera mode selector. cmdCamPersMode/cmdCamPersMoveMode below
 * write 3 and -3 to it; C-owned data (docs/naming.md).
 */
static int persMode;

/*
 * Bank-camera mode selector. cmdCamBankMode/cmdCamBankMoveMode below write 2
 * and -4 to it; C-owned data (docs/naming.md).
 */
static int bankMode;

/*
 * The current AI unit. thinkUnitPtrGetReg (0x00a1f348) substitutes it for
 * unitNoGet when a script register holds the self-reference sentinel 0x7FFF.
 * C-owned data under its original ELF symbol name (docs/naming.md).
 */
static BattleUnit *pThinkUnit;

/*
 * unitNoGet/unitPtrGet (src/ov01/unit_cmd.c) search and index the eight-slot
 * battle unit table: unitNoGet returns the slot whose stored pointer equals
 * its argument (-1 and a printed "unit No err" message if none does) and
 * unitPtrGet returns the unit stored at a slot (0 and its own printed message
 * if the slot is out of range).
 */
extern int unitNoGet(BattleUnit *unit);
extern BattleUnit *unitPtrGet(int unitNo);

/*
 * calcLineChk (src/ov01/calc.c) reports whether a battle unit has reached its
 * line (nonzero) or not (0); cmdLineChk is the only claimed caller here.
 */
extern int calcLineChk(BattleUnit *unit);

/* calcUPGet, calcStatGet, calcStatSet and calcStatReset: include/ov01/calc.h. */
#include "ov01/calc.h"

/*
 * sefGetNullPosition (src/main/sef.c) reads a scene-effect null's world
 * position off an actor; cmdCamUnitHeight (0x00a21948) is the only claimed
 * caller here, redeclared the same way src/ov01/m_cam.c and src/main/sdv.c
 * already do.
 */
extern void sefGetNullPosition(Vector4 *position, void *actor, int part, int flag);

/*
 * CalcUnitParam is owned by a different TU (ov01/tu004 calc.c) and is not
 * completed here. thinkUnitAtkExec (0x00a1dc40) reads a signed half two
 * levels down: the pointer this offset holds (into calcUPGet's unmodeled_44
 * span) points at one entry of this TU's own attack table (cmdAtktbl/
 * cmdAtkset/cmdAtktblSort/cmdAtktblSortSub/cmdAtktblSearch below), whose
 * script-offset field thinkUnitAtkExec is the first claimed
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
 * further evidenced role (scenarioBatEnd is the only claimed caller).
 */
typedef struct UnitDmgInfo {
    BattleUnit *attacker;         /* +0x0 */
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
 * monsSetNoGet (src/ov01/battle_init.c) returns the active
 * monster-set number; it takes no arguments.
 */
extern int monsSetNoGet(void);

/*
 * The AI script command argument the interpreter (thinkExec and friends)
 * hands to every opcode handler in this TU's cmd* family:
 * cmdLineChk, cmdAtktblSort, cmdAtktblSortRev, cmdMonsSetNoGet and
 * cmdAtktblSortSub (this TU) all take the same pointer. Each
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
 * process; cmdWaitEvent, cmdWaitCnt and cmdWaitCamMove (this TU)
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
/*
 * thinkRegNo/thinkRegGet/thinkRegSet/cmdNum/cmdThinksetSub/cmdAtktblSortSub
 * are this TU's own siblings; forward-declared here for the functions above
 * that call them. cmdAtktblSortSub returns int: `daddu $2,$0,$0` at 0x00a202dc
 * sets its result on the way out.
 */
extern short thinkRegNo(short pc);
/*
 * thinkRegGet, cmdReg and cmdNum return the 16-bit script value as short:
 * cmdAtktbl passes a stored cmdNum result straight to thinkAdrGet(short)
 * (lw $4,0x8($29) at 0x00a1ff5c with no sll/sra) and cmdAtktblSearch does
 * the same from a register (daddu $4,$16,$0 at 0x00a203c4), which gcc only
 * omits for a value it already holds sign-extended.
 */
extern short thinkRegGet(int regIndex);
extern void thinkRegSet(int regIndex, int value);

extern short cmdNum(short pc);
extern int cmdThinksetSub(ThinkProcess *proc);
extern int cmdAtktblSortSub(ThinkProcess *proc);
extern int thinkProcessChk(int slot);

/*
 * thinkProcessKindChk (0x00a1d4b8, this TU) walks the AI process
 * table thinkSysInit clears (sixteen 0x24-byte entries starting at
 * processBuf) and counts the live entries (offset 0 nonzero) whose kind
 * field (offset 0x10) equals its argument. thinkExecChk (0x00a1ded0) passes
 * kind 0 and reports whether that count is zero.
 */
extern int thinkProcessKindChk(int kind);

/* "** reg err %d\n" at 0x00a461e8 (ov01 .rodata), the format string regChk
 * (0x00a1dfc8) hands to printf when the operand's high bit (0x8000) is not
 * set. */
extern const char cmdRegErrText[];

/*
 * dataVPadSet (src/ov01/data_unit_org_get.c) installs the
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
 * Camera-event request record cmdThinkCamEvent (0x00a22140) fills on its
 * stack for camEventExec (this TU): type at +0x0
 * (sw $3,0x0($29) after addiu $3,$0,0x19), the script operand at +0x4
 * (sw $2,0x4($29), cmdNum's result), the acting unit pThinkUnit at +0x8
 * (sw $5,0x8($29)) and no target at +0xC (sw $0,0xC($29)); the same four
 * words as src/ov01/battle_init.c's CamEvent; the units are BattleUnit here
 * too (pThinkUnit).
 */
typedef struct ThinkCamEvent {
    int type;            /* +0x00 */
    union {
        int param;           /* +0x04: cmdThinkCamEvent stores the script operand word */
        unsigned short arg;  /* +0x04: camEventExec reads its low halfword (lhu) */
    } operand;
    BattleUnit *unit;    /* +0x08 */
    BattleUnit *target;  /* +0x0C */
} ThinkCamEvent;
#define CAM_EVENT_THINK 0x19
/*
 * Declared returning int, as src/ov01/entry_first_init.c does (the value is
 * not read): with a void declaration sched1 ranks the param store
 * (sw $2,0x4($29), the live cmdNum result) last of the four record stores
 * and emits it before sw $3,0x0($29) / sw $5,0x8($29) (ready list
 * 71 65 58 68 instead of 71 68 65 58 in the -dS dump; the dependence links
 * of the four stores are identical in both forms); the int declaration
 * keeps the original order type, unit, param, target.
 */
extern int camEventExec(ThinkCamEvent *event);

/*
 * cfEncountGet (src/ov01/battle_init.c) returns the active
 * encounter-configuration flags; cmdCfEncountGet (0x00a22250) is the only
 * claimed caller here.
 */
extern int cfEncountGet(void);

/*
 * cfEventGet (src/ov01/battle_init.c) returns the active
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
 * count; C-owned data under its original ELF symbol name
 * (docs/naming.md).
 */
static short camTblIdx;

/*
 * thinkCamTblNumGet (this TU) returns the camera-table entry
 * count that cmdCamTblSet (0x00a21c48) compares the decoded index against.
 */
extern int thinkCamTblNumGet(void);

/*
 * MCamStopMove (src/ov01/m_cam.c) stops camera movement;
 * cmdCamMoveStop (0x00a21ca0) is the only claimed caller here.
 */
extern void MCamStopMove(int stop);

/*
 * dataVPadModeSet (src/ov01/data_unit_org_get.c) enables or
 * disables virtual-pad input; cmdVPadEneble/cmdVPadDisable (0x00a21eb8,
 * 0x00a21ed8) are its only claimed callers here.
 */
extern void dataVPadModeSet(int enable);

/*
 * "@" at 0x00a46300 (ov01 .rodata), the message cmdRput (0x00a1ead8) prints
 * when the decoded register index requests indirect addressing (bit 0x4000
 * set).
 */
extern const char cmdJpText[];

/*
 * "R%02d = %d\n" at 0x00a46308 (ov01 .rodata), the format string cmdRput
 * (0x00a1ead8) hands to printf with the register index and its value.
 */
extern const char cmdRText[];

/*
 * Script command-trace flag. cmdCmdputon/cmdCmdputoff (0x00a1ed10,
 * 0x00a1ed28) set and clear it; C-owned data under its original ELF symbol
 * name (docs/naming.md).
 */
static int cmdPutFlag;

/*
 * thinkExec (this TU) runs the AI script at the given offset;
 * thinkTurnStartExec/thinkTurnEndExec/thinkUnitAtkExec (0x00a1dab0,
 * 0x00a1db78, 0x00a1dc40) tail-call it with the monster-set/attack-table
 * script offset they just read. unitLiveNumGet (src/ov01/battle_init.c)
 * returns the live unit count for a side (0 or 1); the same three
 * functions pass it to thinkRegSet for AI script registers 0x8019/0x801A.
 */
extern int thinkExec(short scriptOffset);
extern int unitLiveNumGet(int side);

/* "** thinkTurnStart ** %X\n" at 0x00a46160 (ov01 .rodata), the format
 * string thinkTurnStartExec (0x00a1dab0) hands to printf with the script
 * offset it is about to run. */
extern const char cmdThinkTurnStartText[];

/* "** thinkTurnEnd ** %X\n" at 0x00a46180 (ov01 .rodata), the format string
 * thinkTurnEndExec (0x00a1db78) hands to printf with the script offset it is
 * about to run. */
extern const char cmdThinkTurnEndText[];

/* "** think Atk (%d) ** %X\n" at 0x00a46198 (ov01 .rodata), the format
 * string thinkUnitAtkExec (0x00a1dc40) hands to printf with the acting
 * unit's number and the script offset it is about to run. */
extern const char cmdThinkAtkText[];

/* "** think Dmg (%d) ** %X\n" at 0x00a461b8 (ov01 .rodata), the format
 * string thinkUnitDmgExec (0x00a1dd28) hands to printf with the damaged
 * unit's number and the script offset it is about to run. */
extern const char cmdThinkDmgText[];

/*
 * cmdUnitpara (this TU) reads (mode 0) or writes (mode 1) one of
 * a unit's AI-visible parameters selected by category; cmdUnitparaGet
 * (0x00a1f508) and cmdUnitparaSet (0x00a1f5b0) are its only claimed callers.
 */
extern int cmdUnitpara(BattleUnit *unit, int category, int value, int mode);

/*
 * cmdTecpara/dataTecGet (this TU and src/ov01/data_unit_org_get.c) look up a
 * technique's parameter; cmdTecparaGet (0x00a20518) is
 * the only claimed caller here.
 */
/*
 * dataTecGet's record (TecRecord, TU-local to src/ov01/data_unit_org_get.c),
 * as far as this TU reads it: cmdAtktbl loads the byte at +0x00 (lbu $5,0x0($2)
 * at 0x00a1fe50) as calcTrg2PE's target-selection flags, the same byte
 * src/ov01/calc.c's TecKindView names attr. cmdTecpara/cmdTecparaSub only
 * forward the pointer.
 */
typedef struct CmdTecRecord {
    unsigned char attr;    /* +0x00 */
} CmdTecRecord;

extern int cmdTecpara(CmdTecRecord *tecRecord, int category);
extern CmdTecRecord *dataTecGet(int tecId);

/*
 * calcTrg2PE and calcAtkChk (src/ov01/calc.c, C there) are declared the way
 * that TU declares them; cmdAtktbl is this TU's only caller of either.
 */
extern int calcTrg2PE(BattleUnit *unit, int trgFlags);

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
 * The unit's motion actor (BattleUnit.actor, +0x14) is the block cmdDeadTime
 * reaches: cmdDeadTime writes +0xD0 of it, and cmdCamSetAct/cmdCamSetAct3
 * only forward the pointer into McamParams/McamActorTarget without
 * dereferencing it. Only the field cmdDeadTime writes is modeled here; the
 * same +0xD0 word is Actor.posSnapX in src/ov01/unit_cmd.h, a different
 * reading of one slot (measured view, not unified).
 */
typedef struct UnitDeadTimeBlock {
    unsigned char unmodeled_0[0xD0];
    float value;    /* +0xD0: cmdDeadTime, reciprocal-scaled duration */
} UnitDeadTimeBlock;

/*
 * One tracked unit of cmdCamSetAct3's two-unit camera: the same three words
 * cmdCamSetAct stores at +0x20..+0x28 of McamParams for a single unit
 * (sw $3,0x20($18) target / sw $4,0x24($18) charaId / sw $2,0x28($18) actId
 * at 0x00a20f8c/0x00a20fa4/0x00a20fc4), repeated per unit at +0x2C/+0x38
 * (sw $3,0x2C($18) at 0x00a212e8 / sw $3,0x38($18) at 0x00a21350).
 */
typedef struct McamActorTarget {
    Actor *targetBlock;                /* +0x0: the unit's motion actor (BattleUnit.actor) */
    int charaId;                       /* +0x4: CalcUnitParam.charaId, widened to a word */
    int actId;                         /* +0x8: raw script operand */
} McamActorTarget;

/*
 * mcamPtrGet (this TU) returns the camera-offset parameter block
 * cmdCamOffsPos (0x00a21458) fills in. cmdCamOffsAng (C in this TU) reaches
 * the same +0x70 enable flag for its own angle block, so that block stays a
 * separate, unmodeled span here. cmdCamSetAct (0x00a20f28) reaches a
 * disjoint span at the head of the same block: +0x00 (sw $19,0x0($18), a
 * one/zero flag -- the same unitTrackMode word below, observed value 1
 * here), +0x20/+0x24/+0x28 (the single-unit form of McamActorTarget below).
 * cmdCamSetAct3 (0x00a21288) reaches the two-unit form at +0x2C/+0x38.
 */
typedef struct McamParams {
    int unitTrackMode;                 /* +0x00: mode selector, written 0 by cmdCamCopy (sw $0,0x0($2) at 0x00a20e00) and cmdCamSetPos (sw $0,0x0($17) at 0x00a20e6c), 1 by cmdCamSetAct (sw $19,0x0($18) at 0x00a20f70), 2 by cmdCamSetAct2 (sw $2,0x0($18) at 0x00a210d8) and 3 by cmdCamSetAct3 (sw $19,0x0($18) at 0x00a212b0); none of this TU's other C functions rely on further values */
    unsigned char unmodeled_4[0x10 - 0x4];
    Vector4 lockPosition;              /* +0x10: cmdCamCopy (swc1 0x10/0x14/0x18, w 1.0f at 0x1C) */
    Actor *targetBlock;                /* +0x20: cmdCamSetAct (sw $3,0x20($18) at 0x00a20f8c) */
    int charaId;                       /* +0x24: cmdCamSetAct (sw $4,0x24($18) at 0x00a20fa4) */
    int actId;                         /* +0x28: cmdCamSetAct (sw $2,0x28($18) at 0x00a20fc4) */
    McamActorTarget pairTarget[2];     /* +0x2C/+0x38: cmdCamSetAct3 */
    unsigned char unmodeled_44[0x6C - 0x44];
    float ratio;                       /* +0x6C: cmdCamSetAct2 (swc1 $f0,0x6C($18) at 0x00a21264), the same offset src/ov01/m_cam.c's MCamPointParams names ratio */
    int offsEnabled;                  /* +0x70 */
    unsigned char unmodeled_74[0x80 - 0x74];
    Vector4 offset;                   /* +0x80: x/y/z scaled by 0.01, w fixed at 1.0 */
    McamAngleOffset angleOffset;      /* +0x90: cmdCamOffsAng */
    unsigned char unmodeled_9C[0xB0 - 0x9C];
} McamParams;

extern McamParams *mcamPtrGet(void);

/*
 * MCamGetCurrentCoord (src/ov01/m_cam.c) fills a coordinate with the given
 * camera point (0 or 1); cmdCamCopy (0x00a20d98) passes the point opposite
 * the current camMode.
 */
extern Vector4 *MCamGetCurrentCoord(Vector4 *coord, int point);

/* "** CamCopy err **\n" at 0x00a465a8 (ov01 .rodata): cmdCamCopy's message
 * when camMode is neither 0 nor 1. */
extern const char cmdCamCopyErrText[];

/*
 * StudioLight's ambient_color (include/main/xgl_studio.h, main/tu101) is the
 * quadword xglLightIntensityAmbient (src/main/xgl_light.c)
 * copies in verbatim; cmdLightAmb (0x00a20840) is the only claimed caller
 * here.
 */
#include "main/xgl_studio.h"
extern void xglLightIntensityAmbient(StudioLight *light, Vector4 *ambient);

/*
 * xglLightIntensityParallel/xglLightDirection (src/main/xgl_light.c,
 * main/tu101) set one of a light block's three parallel entries' color or
 * direction by index; cmdLightCol (0x00a20918) and cmdLightDir (0x00a20a18)
 * are this TU's only callers. That TU models the same light block
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
