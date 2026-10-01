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

/*
 * dataPlChaGet (ov01/data_unit_org_get.c VA 0x00a19210, config/symbols/ov01.txt):
 * the running character's persistent record. SeisanCountInit1 reads the word
 * at +0x04 (next_exp, see src/main/menu_skill.h and src/main/menu_para_pt_rate_get.c,
 * which model this same record under the same scaffold label). Kept under the
 * scaffold's undefined_funcs_auto.txt placeholder name func_A19210 until ov01
 * recovers dataPlChaGet itself.
 */
typedef struct SeisanCharRecord {
    unsigned char unmodeled_00[4];
    int nextExp;                    /* +0x04 */
} SeisanCharRecord;

extern SeisanCharRecord *func_A19210(int chrNo);

/*
 * SeisanCN (0x00534f00, config/symbols/main.txt, size 0xb0): SeisanCountInit1
 * zeroes the whole block (memset at 0x0029f468) and then fills twelve
 * consecutive words at +0x70 (addiu 0x0029f46c, sw 0x0029f484) with each
 * character's dataPlChaGet(chrNo)->nextExp, for chrNo 1..12.
 */
typedef struct SeisanCountData {
    unsigned char unmodeled_00[0x70];
    int nextExp[12];                /* +0x70: character 1..12's next_exp, set by SeisanCountInit1 */
    unsigned char unmodeled_A0[0x10];
} SeisanCountData;

extern SeisanCountData SeisanCN;

/*
 * SeisanResult (0x004db0cc, config/symbols/main.txt, size 0x4): a pointer to
 * the settlement screen's per-entry result table. subSeisanHissatuCheck00 and
 * subSeisanHissatuCheck01 each walk SeisanWork[6] entries of 0x3c bytes
 * starting at +0x38 (addiu 0x002a1428/0x002a1478), reading the word at +0x00
 * of each (lw 0x002a142c/0x002a147c): Check00 returns 0x50 once any entry's
 * count is nonzero, Check01 returns 0x58 once one reaches 5 or more.
 */
typedef struct SeisanHissatuEntry {
    int count;                      /* +0x00 */
    unsigned char unmodeled_04[0x38];
} SeisanHissatuEntry;

typedef struct SeisanResultData {
    unsigned char unmodeled_00[0x38];
    SeisanHissatuEntry entries[1];  /* +0x38: indexed dynamically, count from SeisanWork[6] */
} SeisanResultData;

extern SeisanResultData *SeisanResult;

/*
 * D_004C7350 (.rodata, 0x3c bytes): SeisanDisp copies it onto its stack with
 * the ldl/ldr/sdl/sdr unaligned-doubleword sequence at 0x002a1900..0x002a1974
 * and passes the copy's address to endPrintDirectRibbon (still asm, main/tu164
 * src/main/end_print.c); the fields it holds are not otherwise evidenced here.
 */
typedef struct SeisanRibbonData {
    unsigned int unmodeled_00[15];
} SeisanRibbonData;

extern const SeisanRibbonData D_004C7350;

extern void endPrintDirectRibbon(SeisanRibbonData *ribbon);

/*
 * endPrintExtFunc (still asm, main/tu164 src/main/end_print.c) and
 * MenuLoadSync/MenuBgTaskMain (still asm, main/tu160 src/main/window_tex_load.c)
 * are declared locally here until those TUs recover them; see
 * src/main/ether_tree.c and src/main/window_tex_load.h for the same
 * signatures under the same scaffold labels.
 */
extern void endPrintExtFunc(int kind, int id, void *data);
extern int MenuLoadSync(void);
extern void MenuBgTaskMain(void);

extern XglTaskScheduler *SeisanBgTask;
extern XglTaskScheduler *SeisanTask;

extern void SeisanFadeMain(void);

#endif /* SRC_MAIN_SEISAN_COUNT_INIT1_H */
