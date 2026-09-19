#ifndef INCLUDE_OV10_CGP_H
#define INCLUDE_OV10_CGP_H

#include "shared.h"

/* s16 is the project-wide spelling include/shared.h carries. */
typedef signed char s8;

/*
 * One side's hand of card ids inside CardGameWork. CardPlayHandCnt (ov10/tu002,
 * 0x00a0c6f8) starts 8 bytes into its argument and walks 40 consecutive
 * shorts (`lh`, 0x27 more iterations after the first), counting the negative
 * entries; CardEnemyCommandRequest and CardEnemyAnswer (ov10/tu009) pass it
 * the player's (+0x45C) or the enemy's (+0x2F08) record of the work area.
 * Only the card array is evidenced; the record may extend past it.
 */
typedef struct CardHand {
    u8 unmodeled_00[8];
    s16 cards[40];                 /* +0x08 */
} CardHand;

/*
 * Card game overlay work area. Only the members touched by the recovered
 * functions of this TU are named; everything else stays an unmodeled span
 * until a function that reads or writes it is recovered.
 */
typedef struct CardGameWork {
    u16 flags;                     /* +0x0000: bit set, see CGP_FLAG_ERROR_PLUS */
    u8 unmodeled_0002[0x000C - 0x0002];
    u8 *objFlags;                  /* +0x000C: pointer to a flags byte; CGPRotStageSub requires bit 0x10 set there */
    u8 unmodeled_0010[0x045C - 0x0010];
    CardHand playerHand;           /* +0x045C: side 0 (CardEnemyCommandRequest `addiu $2,$16,0x45C`) */
    u8 unmodeled_04B4[0x2F08 - 0x04B4];
    CardHand enemyHand;            /* +0x2F08: side 1 (`addiu $4,$16,0x2F08`) */
    u8 unmodeled_2F60[0x59B4 - 0x2F60];
    u16 fase;                      /* +0x59B4: card game phase counter; CGPRotStageSub's six callers are all
                                    * *FaseProc/*FaseSub phase handlers, and it only requests a stage rotation
                                    * while fase == 9 */
    u8 unmodeled_59B6[0x59BE - 0x59B6];
    u8 activeHelpPage;             /* +0x59BE: CardHelpPageInit clears it (`sb $0,0x59BE($4)`) */
    u8 unmodeled_59BF[0x59C0 - 0x59BF];
    u8 helpPage[256];              /* +0x59C0: help page states; CardHelpPageInit clears
                                    * +0x5ABF down to +0x59C0 */
    u8 unmodeled_5AC0[0x5AF0 - 0x5AC0];
    s8 errorMessCode;              /* +0x5AF0: CGPSetErrorMess/CGPSetErrorMessPlus first argument */
    u8 errorMessKind;              /* +0x5AF1: 1 = CGPSetErrorMess, 3 = CGPSetErrorMessPlus;
                                    * CardPlayDispInfo tests bit 0x02 to pick the Plus display */
    u8 unmodeled_5AF2[0x5AF5 - 0x5AF2];
    u8 errorMessReason;            /* +0x5AF5: CGPSetErrorMess/CGPSetErrorMessPlus third argument (read with lbu) */
    s16 errorMessValue;            /* +0x5AF6: -1 for CGPSetErrorMess, caller value for the Plus form */
    u8 unmodeled_5AF8[0x5B40 - 0x5AF8];
    char *mess;                    /* +0x5B40: CGPSetMessage, indexed from OLMessTbl */
    char *interruptMess;           /* +0x5B44: CGPSetInterruptMess, indexed from CardPlayTMessList */
    char *permanentMess;           /* +0x5B48: CGPSetPermanentMess, indexed from PermMessTbl or NULL */
    u8 unmodeled_5B4C[0x70F1 - 0x5B4C];
    u8 rotStageCooldown;           /* +0x70F1: nonzero while a pending CGPRotStageSub request is in effect;
                                    * CGPRotStageSub sets it to 20 when it stores a new rotStageId */
    u8 unmodeled_70F2[0x70F3 - 0x70F2];
    u8 rotStageId;                 /* +0x70F3: stage id CGPRotStageSub last stored for a rotation request */
    u8 unmodeled_70F4[0x7100 - 0x70F4];
    u16 modelFadeInTimer;          /* +0x7100: ticks left for CardModelFadeIn (ov10/tu002) to run */
    u16 fadeOutTimer;              /* +0x7102: ticks left for CardFadeOut (ov10/tu002) to run */
    u8 fadeOutStep;                /* +0x7104: amount CardFadeOut subtracts per tick (read with lbu) */
    u8 unmodeled_7105[0x7106 - 0x7105];
    u8 fadeOutColor[4];            /* +0x7106: one level per model slot, CardFadeOut clamps each at zero */
    u8 unmodeled_710A[0x7110 - 0x710A];
    float modelFadeStep;           /* +0x7110: amount CardModelFadeIn adds per tick */
    u8 unmodeled_7114[0x7120 - 0x7114];
    float modelColor[4][4];        /* +0x7120: current color per model slot (0x10-byte rows);
                                    * CardModelFadeIn raises channels 0..2 */
    float modelColorTarget[4][4];  /* +0x7160: per-slot cap CardModelFadeIn clamps modelColor to
                                    * (read 0x40 past modelColor) */
    /*
     * +0x71A0: end of the work area. CardGameRoot (ov10/tu000) passes its
     * function-static `proc` (bss 0x00a4fa10) to CardMainInit/CardMainProc;
     * the next bss symbol, s_inMulCol, is at 0x00a56bb0, 0x71A0 bytes later.
     */
} CardGameWork;

#endif /* INCLUDE_OV10_CGP_H */
