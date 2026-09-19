/*
 * OV10 original TU 3: 0x00a12a90..0x00a19a20 (29 functions)
 */
#include "common.h"
#include "ov10/cgp.h"

/* Sets the card game work area's error message; defined in ov10/tu008
 * (src/ov10/cgp.c), still assembler-scaffolded here. */
extern void CGPSetErrorMessPlus(s8 code, CardGameWork *work, u8 reason, s16 value);

INCLUDE_ASM("asm/nonmatchings/ov10/ccu_038_exec_sub", CCU038ExecSub);

INCLUDE_ASM("asm/nonmatchings/ov10/ccu_038_exec_sub", CCU055ExecSub);

INCLUDE_ASM("asm/nonmatchings/ov10/ccu_038_exec_sub", CCU058ExecSub);

INCLUDE_ASM("asm/nonmatchings/ov10/ccu_038_exec_sub", CCU061ExecSub);

INCLUDE_ASM("asm/nonmatchings/ov10/ccu_038_exec_sub", CCU062ExecSub);

INCLUDE_ASM("asm/nonmatchings/ov10/ccu_038_exec_sub", CCU063ExecSub);

typedef signed int s32;

/* CPU-driven card command handlers; the plain *ExecSub siblings they tail-call
 * are still assembler in this TU. */
extern void CCU063ExecSub(CardGameWork *work, s32 context);
extern void CCU064ExecSub(CardGameWork *work, s32 context);
extern void CCU065ExecSub(CardGameWork *work, s32 context);

void CCU063ExecCPUSub(CardGameWork *work, s32 context) {
    CGPSetErrorMessPlus(-1, work, 0x17, 0x3F);
    CCU063ExecSub(work, context);
}

INCLUDE_ASM("asm/nonmatchings/ov10/ccu_038_exec_sub", CCU064ExecSub);

void CCU064ExecCPUSub(CardGameWork *work, s32 context) {
    CGPSetErrorMessPlus(-1, work, 0x17, 0x40);
    CCU064ExecSub(work, context);
}

INCLUDE_ASM("asm/nonmatchings/ov10/ccu_038_exec_sub", CCU065ExecSub);

void CCU065ExecCPUSub(CardGameWork *work, s32 context) {
    CGPSetErrorMessPlus(-1, work, 0x17, 0x41);
    CCU065ExecSub(work, context);
}

/* Only the command id at +0xE and the card id at +0x18 are evidenced by
 * CCC107ChkSub; the rest of the record is scaffold-owned. */
typedef struct CardCommandInfo {
    u8 unmodeled_00[0xE];
    u16 commandId;                 /* +0xE */
    u8 unmodeled_10[0x18 - 0x10];
    s16 cardId;                    /* +0x18 */
} CardCommandInfo;

s32 CCC107ChkSub(CardCommandInfo *cmd) {
    s32 matched = 0;

    if ((u32) (cmd->commandId - 0x22) < 2U || (s16) cmd->commandId == 0x25 ||
        (s16) cmd->commandId == 0x29 || (s16) cmd->commandId == 0x46 ||
        (s16) cmd->commandId == 0x47) {
        matched = 1;
    }
    if (matched == 0 && cmd->cardId >= 0 &&
        (cmd->cardId == 0x2B || cmd->cardId == 0x2D)) {
        matched = 1;
    }
    return matched;
}

void CCC02ExecSub(u8 *commandState) {
    *commandState &= ~0x02;
}

INCLUDE_ASM("asm/nonmatchings/ov10/ccu_038_exec_sub", CCC03ExecSub);

INCLUDE_ASM("asm/nonmatchings/ov10/ccu_038_exec_sub", CCC04ExecSub);

INCLUDE_ASM("asm/nonmatchings/ov10/ccu_038_exec_sub", CCC06ExecSub);

INCLUDE_ASM("asm/nonmatchings/ov10/ccu_038_exec_sub", CCC07ExecSub);

INCLUDE_ASM("asm/nonmatchings/ov10/ccu_038_exec_sub", CCC10ExecSub);

INCLUDE_ASM("asm/nonmatchings/ov10/ccu_038_exec_sub", CCC26ExecSub);

INCLUDE_ASM("asm/nonmatchings/ov10/ccu_038_exec_sub", CCC30ExecSub);

INCLUDE_ASM("asm/nonmatchings/ov10/ccu_038_exec_sub", CCC31ExecSub);

INCLUDE_ASM("asm/nonmatchings/ov10/ccu_038_exec_sub", CCC38ExecSub);

INCLUDE_ASM("asm/nonmatchings/ov10/ccu_038_exec_sub", CardPlayCommandCondition);

INCLUDE_ASM("asm/nonmatchings/ov10/ccu_038_exec_sub", CPCSearchCommWk);

INCLUDE_ASM("asm/nonmatchings/ov10/ccu_038_exec_sub", CPCPSwapDSBATTLE);

INCLUDE_ASM("asm/nonmatchings/ov10/ccu_038_exec_sub", CardPlayCommandPlay);

INCLUDE_ASM("asm/nonmatchings/ov10/ccu_038_exec_sub", CardPlayCommandBattleExecute);

INCLUDE_ASM("asm/nonmatchings/ov10/ccu_038_exec_sub", CardPlayCommandExecute);

INCLUDE_ASM("asm/nonmatchings/ov10/ccu_038_exec_sub", CardPlayCommandCheck);
