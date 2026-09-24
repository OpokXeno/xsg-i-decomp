#ifndef INCLUDE_OV10_CGP_H
#define INCLUDE_OV10_CGP_H

#include "shared.h"

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
    u8 unmodeled_00[3];
    u8 commandFlags;               /* +0x03: CardPlayCommandPlay command 37 sets bit 0 */
    u8 unmodeled_04[4];
    s16 cards[40];                 /* +0x08 */
} CardHand;

/*
 * Card master record (0x24 bytes). CardGameWork.definitions points at the table;
 * CardPlayCommandPlay (ov10/tu003) indexes it by card id.
 */
typedef struct CardDefinition {
    u8 kind;
    u8 unmodeled_01;
    u16 flags;
    u8 unmodeled_04[12];
    u8 reserveCost;               /* +0x10: cards this command reserves from the draw pile */
    u8 unmodeled_11;
    u8 operationDuration;          /* +0x12 */
    u8 unmodeled_13;
    u8 attackPower;                /* +0x14 */
    u8 basePower;                  /* +0x15 */
    u8 unmodeled_16;
    u8 identity;                   /* +0x17 */
    u8 unmodeled_18[12];
} CardDefinition;

/* One card of a board slot's stack (10 bytes). */
typedef struct CardBattleLayer {
    u8 state;
    u8 unmodeled_01;
    u8 boostCount;                 /* +0x02: CCC26ExecSub adds one */
    s8 life;                       /* +0x03: CCC38ExecSub subtracts two, clamped at zero */
    u8 unmodeled_04;
    u8 damageTimer;                /* +0x05 */
    s16 cardId;                    /* +0x06 */
    u8 unmodeled_08[2];
} CardBattleLayer;

/*
 * The cards stacked on one board slot. The base layer (index zero) is the
 * played command card; layer one is the first card stacked on it.
 */
typedef struct CardLayerStack {
    u8 flags;
    u8 unmodeled_01;
    u8 power;
    u8 powerLimit;
    u8 unmodeled_04[4];
    CardBattleLayer layers[40];    /* +0x008 */
    float position[4];             /* +0x198: effect position (CardSetEffect) */
} CardLayerStack;

/* One battle or disposal slot of a side's board (0x1BC bytes). */
typedef struct CardBoardSlot {
    float shuntPosition[3];        /* +0x000: where the card of the slot before
                                    * this one is shunted to; CGPShuntPosSub
                                    * (0x00a21768) copies that slot's effect
                                    * position 0x1BC bytes forward into it */
    CardLayerStack battle;         /* +0x00C */
    u8 unmodeled_1B4[8];
} CardBoardSlot;

/*
 * One side's cards (0x106C bytes): the hand record, the deck and junk piles,
 * four battle and four disposal slots and the operation cards in play.
 */
typedef struct CardPlaySide {
    CardHand hand;
    s16 deck[40];                  /* +0x058 */
    s16 sute[40];                  /* +0x0A8: discard counts CGPPrintSute
                                    * (ov10/tu008, 0x00a21d78) prints, ten per
                                    * line ("Sute[%2d]:" row header, "%3d,"
                                    * per entry) */
    s16 junk[40];                  /* +0x0F8 */
    u8 unmodeled_0148[0x190 - 0x148];
    CardBoardSlot battle[4];       /* +0x190 */
    CardBoardSlot disposal[4];     /* +0x880 */
    float lastShuntPosition[3];    /* +0xF70: shunt position of the last disposal
                                    * slot, the record CGPShuntPosSub writes one
                                    * stride past disposal[3] */
    s16 operations[40];            /* +0xF7C */
    s16 operationFlags[40];        /* +0xFCC */
    s16 operationDuration[40];     /* +0x101C */
} CardPlaySide;

/* One side's controller snapshot for the command being played. */
typedef struct CardCommandInput {
    u16 held;
    u16 pressed;
    u16 directions;                /* +0x04: copy of the pad record's d-pad halfword at +0x2c (CardMainProc
                                    * 0x00a03afc `lhu $3,44($6)` / 0x00a03b00 `sh $3,0x5AD4($30)`);
                                    * CardOpenProc tests 0x2000/0x8000 (right/left) in it */
    u16 repeat;
} CardCommandInput;

/* State of the command card being played (CardPlayCommandPlay, 0x30 bytes). */
typedef struct CardCommandControl {
    u8 unmodeled_00[2];
    u16 phase;                     /* +0x02 */
    u8 unmodeled_04[2];
    s16 step;                      /* +0x06: CardPlayCommandPlay's step within the command */
    s16 flowRequest;               /* +0x08: pending game-flow request, 0 = none. CardGameProc reads it
                                    * with lh (0x00a31900) and dispatches 1..6 through a jump table
                                    * (0x00a4f620):
                                    *   1  confirm prompt, requested with Square when CGPChkFaseLock
                                    *      allows it (0x00a31ce0..0x00a31d00); answered with
                                    *      Circle/Cross; cleared once handled;
                                    *   3/4  a side's deck ran out (CardPlayYamaCnt, 0x00a317d0/0x00a317f0);
                                    *   5  the final round was played (CardMainProc 0x00a06004);
                                    *   3..5 go to CaedGameWinMode and stay set (marking the game
                                    *      finished, tested by CardGameProc and CardPlayDispInfo) until
                                    *      CardGameInit clears it (0x00a25288);
                                    *   6  START menu, requested by either controller
                                    *      (0x00a31d28..0x00a31d7c); handled by NewCMPGameModeExit,
                                    *      then cleared.
                                    * CardPlayDispInfo also reads it with lh */
    u8 unmodeled_0A;
    u8 recoveryApplied;            /* +0x0B */
    u16 commandIndex;              /* +0x0C */
    u8 unmodeled_0E[2];
    CardCommandInput input[2];     /* +0x10: player, enemy */
    u8 unmodeled_20[8];
    u16 slideDirection;            /* +0x28: nonzero slides the offset down (CardMainProc 0x00a0859c lhu) */
    u16 slideTimer;                /* +0x2A: frames the slide still runs (0x00a08594 lhu, 0x00a085bc sh) */
    float slideOffset;             /* +0x2C: CardMainProc steps it by 0.1 per frame (0x00a085cc lwc1,
                                    * 0x00a085ec swc1) */
} CardCommandControl;

/* The board cursor (CardCursorActive/CardCursorPassive). */
typedef struct CardCursorControl {
    s16 selectionMode;
    u8 unmodeled_02[4];
    s16 controllingSide;           /* +0x06 */
} CardCursorControl;

/*
 * The card game part of the save data CardGameWork.save points to. Every
 * access is a byte access through the pointer loaded from work +0x0C.
 */
typedef struct CardSaveData {
    u8 flags;                      /* +0x00: bit set (lbu/sb), see CARD_SAVE_ROTATE_STAGE below */
    u8 background;                 /* +0x01: title background, 0..5; CardMainProc wraps it at 6
                                    * (0x00a03fc8 `lbu $2,1($3)` / `sltiu $2,$2,6`) before CMIBGInit */
    u8 unmodeled_02[0x2C - 0x02];
    u8 deck[40];                   /* +0x2C: the first stored deck, card ids read with lbu
                                    * (CardMainProc 0x00a07400 `addiu $3,$4,44`, 40 iterations);
                                    * CardCopyRam2Deck (ov10 0x00a0ae40) reads stored deck n at
                                    * +0x2C + n * 42, so the stored decks are 42-byte records whose
                                    * last two bytes are not modeled */
} CardSaveData;

/*
 * One model CardMainProc's common tail draws (0x50 bytes; the table at work
 * +0x6BF0 is walked down from models[15] with a -80 stride, 0x00a04dc0
 * `addiu $2,$2,-80`).
 */
typedef struct CardModelEntry {
    int visible;                   /* +0x00: sw 0/1; the draw loop skips a zero entry (0x00a083b0 `lw $2,0($16)`) */
    void *model;                   /* +0x04: model data address (0x00a04028 `sw $3,0x6E24($30)` for models[7]) */
    void *texture;                 /* +0x08: texture address */
    u8 unmodeled_0C[4];
    float matrix[4][4];            /* +0x10: placement passed to nmlModelSetPlace (0x00a0839c `addiu $18,$30,0x6C00`) */
} CardModelEntry;

/*
 * Card game overlay work area. Only the members touched by the recovered
 * functions of this TU are named; everything else stays an unmodeled span
 * until a function that reads or writes it is recovered.
 */
typedef struct CardGameWork {
    u16 flags;                     /* +0x0000: bit set, see CGP_FLAG_ERROR_PLUS */
    u8 unmodeled_0002[0x0008 - 0x0002];
    CardDefinition *definitions;   /* +0x0008: card master table */
    CardSaveData *save;            /* +0x000C: card game save data (lw, then byte accesses through it);
                                    * CGPRotStageSub requires CARD_SAVE_ROTATE_STAGE */
    u8 unmodeled_0010[0x013C - 0x0010];
    s16 playerDeck[40];            /* +0x013C: deck of side 0; CardMainProc passes it to CardCopyRam2Deck
                                    * and CardPlayInitWork (0x00a051c4 `addiu $4,$30,0x13C`); CardCopyRam2Deck
                                    * fills it with 40 halfwords (`sh $2,0($4)`, stride 2), and enemyDeck
                                    * follows 0x50 bytes later */
    s16 enemyDeck[40];             /* +0x018C: deck of side 1 (0x00a052d8 `addiu $4,$30,0x18C`) */
    s16 commandQueue[2][40][4];    /* +0x01DC: queued commands of the player and the enemy;
                                    * a negative first word marks a free entry (CPCSearchCommWk) */
    CardPlaySide player;           /* +0x045C: side 0 (CardEnemyCommandRequest `addiu $2,$16,0x45C`) */
    u8 unmodeled_14C8[0x2F08 - 0x14C8];
    CardPlaySide enemy;            /* +0x2F08: side 1 (`addiu $4,$16,0x2F08`) */
    u8 unmodeled_3F74[0x59B4 - 0x3F74];
    u16 fase;                      /* +0x59B4: card game phase counter; CGPRotStageSub's six callers are all
                                    * *FaseProc/*FaseSub phase handlers, and it only requests a stage rotation
                                    * while fase == 9 */
    u16 faseStep;                  /* +0x59B6: step within the current fase (lhu/sh only) */
    u8 unmodeled_59B8[3];
    u8 enemyDeckId;                /* +0x59BB: ROM deck the computer opponent plays; CardMainProc stores it
                                    * (0x00a05efc `sb $3,0x59BB($30)`), GetDeckData (ov10/tu010) reads it */
    u8 helpState;                  /* +0x59BC: nonzero while the help screen replaces the fase; CardHelpProc
                                    * dispatches on its high nibble (0x10/0x20/0x30) */
    u8 menuPlayer;                 /* +0x59BD: controller that opened the START menu: CardGameProc
                                    * stores 0 or 1 with the flowRequest 6 (0x00a31d48 / 0x00a31d84)
                                    * and passes it to NewCMPGameModeExit (0x00a31c80), which stores
                                    * it back; CardHelpProc reads input[1] instead of input[0] when it is set */
    u8 activeHelpPage;             /* +0x59BE: CardHelpPageInit clears it (`sb $0,0x59BE($4)`) */
    u8 unmodeled_59BF[0x59C0 - 0x59BF];
    u8 helpPage[256];              /* +0x59C0: help page states; CardHelpPageInit clears
                                    * +0x5ABF down to +0x59C0 */
    CardCommandControl command;    /* +0x5AC0 */
    s8 errorMessCode;              /* +0x5AF0: CGPSetErrorMess/CGPSetErrorMessPlus first argument */
    u8 errorMessKind;              /* +0x5AF1: 1 = CGPSetErrorMess, 3 = CGPSetErrorMessPlus;
                                    * CardPlayDispInfo tests bit 0x02 to pick the Plus display */
    u8 unmodeled_5AF2[2];
    s8 resultWait;                 /* +0x5AF4: frames to wait after a game before input is read (lb/sb);
                                    * CardMainProc sets 90 and waits while it is positive */
    u8 errorMessReason;            /* +0x5AF5: CGPSetErrorMess/CGPSetErrorMessPlus third argument (read with lbu) */
    s16 errorMessValue;            /* +0x5AF6: -1 for CGPSetErrorMess, caller value for the Plus form */
    u8 unmodeled_5AF8[0x5B40 - 0x5AF8];
    char *mess;                    /* +0x5B40: CGPSetMessage, indexed from OLMessTbl */
    char *interruptMess;           /* +0x5B44: CGPSetInterruptMess, indexed from CardPlayTMessList */
    char *permanentMess;           /* +0x5B48: CGPSetPermanentMess, indexed from PermMessTbl or NULL */
    u8 unmodeled_5B4C[0x5B50 - 0x5B4C];
    CardCursorControl cursor;      /* +0x5B50 */
    u8 unmodeled_5B58[0x6BF0 - 0x5B58];
    CardModelEntry models[16];     /* +0x6BF0: models drawn after every fase (0x00a083a0 `addiu $16,$30,0x6BF0`) */
    u8 unmodeled_70F0;
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

/*
 * The card in play CGPCalcPowerPlus and CardChkAttackType receive. It is the
 * memory a board slot's CardLayerStack covers - CardPlayCntPower (ov10/tu002,
 * 0x00a0da40) reads CardLayerStack.flags and layers[0]/layers[1].cardId from
 * the pointer it hands on - but CGPCalcPowerPlus loads the three bytes at
 * +0x08..+0x0A signed (`lb` at 0x00a29268, 0x00a2926c and 0x00a2928c) where
 * the accepted CardBattleLayer accesses of the same bytes load them unsigned,
 * so the record was declared a second time, as CardHand and CardPlayHand are.
 */
typedef struct CardPowerCard {
    u8 unmodeled_00[8];
    s8 powerPlus[2];                /* +0x08: both are added to the card's power */
    s8 boostCount;                  /* +0x0A: the power doubles once per boost;
                                     * cleared for attack type 4 */
    u8 unmodeled_0B[3];
    s16 cardId;                     /* +0x0E: negative while the slot is empty */
} CardPowerCard;

#endif /* INCLUDE_OV10_CGP_H */
