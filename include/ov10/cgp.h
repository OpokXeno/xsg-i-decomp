#ifndef INCLUDE_OV10_CGP_H
#define INCLUDE_OV10_CGP_H

#include "shared.h"

typedef signed char s8;
typedef signed int s32;

/* Board geometry shared by the card-game TUs. */
#define CARD_BOARD_SLOTS 4         /* battle slots per side, and disposal slots per side */
#define CARD_SLIDE_FRAMES 10       /* frames a card takes to slide to a new slot */

/*
 * Phases of a turn, in the order of the help screen's list (DRAW PHASE ...
 * ADJUST PHASE); CardCommandControl.phase counts them for side 0 and then,
 * CARD_PHASES_PER_SIDE later, for side 1.
 */
#define CARD_PHASE_DRAW 0
#define CARD_PHASE_MOVE 1
#define CARD_PHASE_EVENT 2
#define CARD_PHASE_SET 3
#define CARD_PHASE_BLOCK 4
#define CARD_PHASE_BATTLE 5
#define CARD_PHASE_ADJUST 6
#define CARD_PHASES_PER_SIDE 7

/* CardDefinition.kind ("There are three types of cards: Battle, event, and situation"). */
#define CARD_KIND_BATTLE 1
#define CARD_KIND_EVENT 2
#define CARD_KIND_SITUATION 4

/*
 * CardDefinition.attackType, in the order the help text lists the types
 * ("Hand, Ballistic, Spread, and Homing"). CheckFriendKakutouCnt counts the
 * hand-to-hand type; TodomeHyouka subtracts a spread attack from every
 * target's life and counts a homing attack against the other side's deck.
 */
#define CARD_ATTACK_HAND 1
#define CARD_ATTACK_BALLISTIC 2
#define CARD_ATTACK_SPREAD 3
#define CARD_ATTACK_HOMING 4

/*
 * CardDefinition.flags: the battle card's attribute icon (H Human, M Mech,
 * G Gnosis, R Realian). CheckREAL tests 2 and CheckGuno 4; Human and Mech are
 * the two remaining bits, Human being the attribute the Tamasii situation
 * card powers up (GetFriendAttack).
 */
#define CARD_ATTR_HUMAN 0x0001
#define CARD_ATTR_REALIAN 0x0002
#define CARD_ATTR_GNOSIS 0x0004
#define CARD_ATTR_MECH 0x0008

/* CardLayerStack.flags bits. */
#define CARD_STACK_FLAG_END 0x01       /* cleared by CardPlayClearEndflg */
#define CARD_STACK_FLAG_FROZEN 0x02    /* no countdown and no attack (CGP_STACK_FLAG_FROZEN in ov10/cgp.c) */

/*
 * A board or hand position: the zone code, the slot or hand index and the
 * card there (8 bytes, passed by value to CGPDisposeWeaponSub). ov10/cgp.c
 * keeps the same record as CGPCursorPosition.
 */
typedef struct CardCursorPosition {
    s16 zone;
    s16 index;
    s16 cardId;
    s16 stackedCard;               /* +0x06: the card stacked on the slot's base card */
} CardCursorPosition;

/*
 * One side's hand of card ids inside CardGameWork. CardPlayHandCnt (ov10/tu002,
 * 0x00a0c6f8) starts 8 bytes into its argument and walks 40 consecutive
 * shorts (`lh`, 0x27 more iterations after the first), counting the negative
 * entries; CardEnemyCommandRequest and CardEnemyAnswer (ov10/tu009) pass it
 * the player's (+0x45C) or the enemy's (+0x2F08) record of the work area.
 * Only the card array is evidenced; the record may extend past it.
 */
typedef struct CardHand {
    s8 playPoints[3];              /* +0x00: per-kind points available for playing cards; [2] is set to 1
                                    * together with the first two when the draw fase starts (sb 0x00a2a7c0),
                                    * no reader found */
    u8 commandFlags;               /* +0x03: CardPlayCommandPlay command 37 sets bit 0 */
    u8 deckCount;                  /* +0x04: cards left in the deck, CardPlayYamaCnt's result (CaedGameWinMode) */
    u8 suteCount;                  /* +0x05: cards in the discard pile, CardPlaySuteCnt's result (CGPHPCheckSub, sb 0x00a295ac) */
    u8 unmodeled_06[2];
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
    u8 unmodeled_04[8];
    u8 discardRequirements[4];     /* +0x0C: CGPSetFaseProc counts the set ones (lbu at
                                    * 0x00a2be20); one card is discarded for each */
    u8 reserveCost;               /* +0x10: cards this command reserves from the draw pile */
    u8 playCost;                   /* +0x11: points required to play this card */
    u8 operationDuration;          /* +0x12 */
    u8 attackType;                 /* +0x13: CardChkAttackType returns it (lbu at
                                    * 0x00a0cefc); type 4 spends a card's boosts
                                    * (CGPCalcPowerPlus), CCC38ExecSub tests type 1 */
    u8 attackPower;                /* +0x14 */
    u8 basePower;                  /* +0x15 */
    u8 unmodeled_16;
    u8 identity;                   /* +0x17 */
    u8 unmodeled_18[12];
} CardDefinition;

/* One card of a board slot's stack (10 bytes). */
typedef struct CardBattleLayer {
    u8 state;
    s8 flag;                       /* +0x01: signed byte; CardEnemySet tests it with `lb` (0x00a3271c) and
                                    * clears it with `sb` for the base layer of a slot holding card 14 */
    u8 boostCount;                 /* +0x02: CCC26ExecSub adds one */
    s8 life;                       /* +0x03: CCC38ExecSub subtracts two, clamped at zero */
    u8 shownLife;                  /* +0x04: copy of life CGPHPCheckSub refreshes while the damage timer is
                                    * zero (lbu 0x00a295e0, sb 0x00a295e4) */
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
    u8 moveFrames;                 /* +0x01: frames remaining in the current movement */
    u8 power;
    u8 powerLimit;
    u8 previousFlags;              /* +0x04: copy of flags CGPHPCheckSub makes (lbu 0x00a295b0, sb 0x00a295b8) */
    u8 unmodeled_05[3];
    CardBattleLayer layers[40];    /* +0x008 */
    float position[3];             /* +0x198: current effect position (CardSetEffect) */
    float step[3];                 /* +0x1A4: per-frame movement toward the next slot */
} CardLayerStack;

/* Same record under the name the effect table uses. */
typedef CardLayerStack CardLayerStackView;

/* One battle or disposal slot of a side's board (0x1BC bytes). */
typedef struct CardBoardSlot {
    union {
        float shuntPosition[3];    /* +0x000: where the card of the slot before
                                    * this one is shunted to; CGPShuntPosSub
                                    * (0x00a21768) copies that slot's effect
                                    * position 0x1BC bytes forward into it */
        struct {
            u8 unmodeled_000[8];
            s16 playedCard;        /* +0x008: only battle[0] (CardPlaySide+0x198) is
                                    * read: the command card id the side is playing
                                    * (`lh` at 0x00a3327c, CGPEnemyExecCommand);
                                    * ov10/cgp.c calls the same halfword lastCardValue */
        } cmdView;
    };
    CardLayerStack battle;         /* +0x00C */
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

/*
 * Per-frame movement record of a hand card (28 bytes each), at +0x192C of a
 * side (CardMoveRecord / CardPlayField.handMove in ov10/cgp.c).
 */
typedef struct CardHandMoveRecord {
    u8 unmodeled_00[4];
    float position[3];             /* +0x04 */
    float step[3];                 /* +0x10 */
} CardHandMoveRecord;

/*
 * Partial view of a side past the end of CardPlaySide (measured: CardPlaySide
 * stops at 0x106C, and making handMove a real member breaks the ov10/tu000
 * data carve, "allocation refused: D_00A4FA10 cc1 directives prove
 * size/alignment 0x8be0/0x8, record claims 0x71a0/0x8"). Callers cast their
 * CardPlaySide pointer; a separately computed pointer changes the schedule.
 */
typedef struct CardPlaySideMoveView {
    u8 unmodeled_0000[0x192C];
    CardHandMoveRecord handMove[40];   /* +0x192C */
} CardPlaySideMoveView;

/* One side's controller snapshot for the command being played. */
typedef struct CardCommandInput {
    union {
        u64 all;                   /* CardOpenProc reads held..repeat as one doubleword and masks 0x00600000
                                    * (= pressed & 0x60): `ld $2,0x5AD0($18)` at 0x00a1e37c and 0x00a1e538;
                                    * case 9 tests the same bits with `lhu $2,0x5AD2($18); andi $2,$2,0x60`
                                    * at 0x00a1de40 */
        struct {
            u16 held;
            u16 pressed;
            u16 directions;        /* +0x04: copy of the pad record's d-pad halfword at +0x2c (CardMainProc
                                    * 0x00a03afc `lhu $3,44($6)` / 0x00a03b00 `sh $3,0x5AD4($30)`);
                                    * CardOpenProc tests 0x2000/0x8000 (right/left) in it */
            u16 repeat;
        };
    };
} CardCommandInput;

/* State of the command card being played (CardPlayCommandPlay, 0x30 bytes). */
typedef struct CardCommandControl {
    s16 turn;                      /* +0x00: current turn number */
    u16 phase;                     /* +0x02 */
    u16 phaseTimer;                /* +0x04: frames spent in the current phase */
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
    u8 turnCountdown;              /* +0x0A: counted down by CGPEndFaseSub while nonzero (lbu/sb 0x00a2fb24 / 0x00a2fb30) and
                                    * read by CGPBattleFaseProc (lbu 0x00a30438); CardGameInit clears it (sb 0x00a2528c) */
    u8 recoveryApplied;            /* +0x0B */
    u16 commandIndex;              /* +0x0C */
    u16 aiPhase;                   /* +0x0E: phase of the AI handler of the current phase; CardEnemyMove and
                                    * CardEnemySet switch on it (`lhu` 0x00a32598 / 0x00a32684 from work
                                    * +0x5ACE) and advance it with `sh` */
    CardCommandInput input[2];     /* +0x10: player, enemy */
    s16 interruptMessId;           /* +0x20: id of the interrupt message on display, 0 = none; CGPNextTurnSub
                                    * stores the phase message id (`sh` at 0x00a25ae8) and CardGameProc waits
                                    * on it (`lh` 0x00a31728) */
    s16 interruptMessTimer;        /* +0x22: frames the interrupt message stays; CardGameProc counts it down
                                    * (`lh` 0x00a31778, `sh` 0x00a31790) and clears both with the message pointer */
    s16 exitMenuItem;              /* +0x24: START-menu cursor, 0..3 (NewCMPGameModeExit, lh 0x00a01e5c) */
    s16 exitMenuSub;               /* +0x26: option row inside the item, -1 = none (lh 0x00a01e50) */
    u16 slideDirection;            /* +0x28: nonzero slides the offset down (CardMainProc 0x00a0859c lhu) */
    u16 slideTimer;                /* +0x2A: frames the slide still runs (0x00a08594 lhu, 0x00a085bc sh) */
    float slideOffset;             /* +0x2C: CardMainProc steps it by 0.1 per frame (0x00a085cc lwc1,
                                    * 0x00a085ec swc1) */
} CardCommandControl;

/* The board cursor (CardCursorActive/CardCursorPassive). */
typedef struct CardCursorControl {
    s16 selectionMode;
    s16 selectionSubMode;          /* +0x02: stored with selectionMode when a fase starts (sh 0x00a2a7d0 / 0x00a2a7d8,
                                    * 0x00a2b3cc / 0x00a2b3c4, 0x00a2bca4 / 0x00a2bcb0); no reader found */
    u8 unmodeled_04[2];
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
    u8 unmodeled_02[0x04 - 0x02];
    u8 cardCounts[38];             /* +0x04: owned copies (0..3) of each card id, 2 bits each, four ids per
                                    * byte, least significant bits first, 150 ids; CardOpenProc case 10
                                    * unpacks them into CardGameWork.ownedCount (0x00a1dfb4 `lbu $4,0($6+(i>>2))`,
                                    * 0x00a1dfc4 `srav`, 0x00a1dfcc `andi $4,$4,3`), CardCntList reads them
                                    * one card at a time (0x00a0b068 `lbu $3,4($6)`, masks 3/0xC/0x30/0xC0) */
    u8 unmodeled_2A[0x2C - 0x2A];
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

/* One visual effect in the card game's 64-entry effect table (0x40 bytes). */
typedef struct CardEffect {
    u8 side;
    u8 code;
    u8 phase;
    u8 timer;
    s8 value;
    u8 unmodeled_05[3];
    int soundId;
    CardLayerStackView *stack;
    float position[4];
    float velocity[4];
    float scale[4];
} CardEffect;

/*
 * Card game overlay work area. Only the members touched by the recovered
 * functions of this TU are named; everything else stays an unmodeled span
 * until a function that reads or writes it is recovered.
 */
typedef struct CardGameWork {
    u16 flags;                     /* +0x0000: bit set, see CGP_FLAG_ERROR_PLUS */
    u16 cardCount;                 /* +0x0002: number of entries in the definitions table; CardViewProc passes
                                    * it to CardMakeDispList (0x00a1c458 `lhu`, which walks that many 0x24-byte
                                    * records from +0x08) and uses it as the CardCntList loop bound (0x00a1c6a4) */
    u16 *commandIds;               /* +0x0004: command id per card id */
    CardDefinition *definitions;   /* +0x0008: card master table */
    CardSaveData *save;            /* +0x000C: card game save data (lw, then byte accesses through it);
                                    * CGPRotStageSub requires CARD_SAVE_ROTATE_STAGE */
    s16 ownedCount[150];           /* +0x0010: owned copies of each card id, unpacked from save->cardCounts by
                                    * CardOpenProc case 10 (0x00a1dfd0 `sh $4,0($5)`, $5 = work + 0x10, stride 2);
                                    * CardViewProc reads it with `lh` (0x00a1c698) and passes only the nonzero
                                    * test to CardDisp (`sltu $6,$0,$6`) */
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
    u8 unmodeled_59B8;
    u8 aiStep;                     /* +0x59B9: step of the computer player's routine for the current phase,
                                    * counted up and tested in CC_Kara_SetFase (`lbu` 0x00a3bccc) and
                                    * CC_Kara_CommFase (0x00a37bec); cleared by CardGameInit (0x00a2529c)
                                    * and CGPNextTurnSub (`sb` 0x00a25af0) */
    u8 playerDeckId;               /* +0x59BA: the side-0 counterpart of enemyDeckId: GetDeckData,
                                    * GetDeckDataAdjust and CheckGeneration (ov10/tu010) read it
                                    * (`lbu` 0x00a354e4) where side 1 reads enemyDeckId; no writer
                                    * found */
    u8 enemyDeckId;                /* +0x59BB: ROM deck the computer opponent plays; CardMainProc stores it
                                    * (0x00a05efc `sb $3,0x59BB($30)`), GetDeckData (ov10/tu010) reads it */
    u8 helpState;                  /* +0x59BC: nonzero while the help screen replaces the fase; CardHelpProc
                                    * dispatches on its high nibble (0x10/0x20/0x30) */
    u8 menuPlayer;                 /* +0x59BD: controller that opened the START menu: CardGameProc
                                    * stores 0 or 1 with the flowRequest 6 (0x00a31d48 / 0x00a31d84)
                                    * and passes it to NewCMPGameModeExit (0x00a31c80), which stores
                                    * it back; CardHelpProc reads input[1] instead of input[0] when it is set */
    u8 activeHelpPage;             /* +0x59BE: CardHelpPageInit clears it (`sb $0,0x59BE($4)`) */
    s8 helpTopic;                  /* +0x59BF: help topic of the running screen, set on entry (CardViewProc
                                    * 0x00a1c210 `sb` 0x36, CardOpenProc 0x00a1cc3c `sb` 0x35, CardGameInit
                                    * 0x00a25270 0x21, CGPNextTurnSub 0x00a25b4c stores a byte loaded from a table
                                    * rather than a constant); CardHelpProc steps it and wraps it at 0x3B
                                    * (0x00a20d58..0x00a20d78) */
    u8 helpPage[256];              /* +0x59C0: help page states; CardHelpPageInit clears
                                    * +0x5ABF down to +0x59C0 */
    CardCommandControl command;    /* +0x5AC0 */
    s8 errorMessSide;              /* +0x5AF0: side the message is lit for (first argument of CGPSetErrorMess/CGPSetErrorMessPlus; -1 lights neither) */
    u8 errorMessKind;              /* +0x5AF1: 1 = CGPSetErrorMess, 3 = CGPSetErrorMessPlus;
                                    * CardPlayDispInfo tests bit 0x02 to pick the Plus display */
    u16 infoPanelPhase;            /* +0x5AF2: information-panel animation phase */
    s8 resultWait;                 /* +0x5AF4: frames to wait after a game before input is read (lb/sb);
                                    * CardMainProc sets 90 and waits while it is positive */
    u8 errorMessReason;            /* +0x5AF5: CGPSetErrorMess/CGPSetErrorMessPlus third argument (read with lbu) */
    s16 errorMessValue;            /* +0x5AF6: -1 for CGPSetErrorMess, caller value for the Plus form */
    u8 unmodeled_5AF8[0x5B00 - 0x5AF8];
    float infoPanelPos[4];         /* +0x5B00: information-panel position */
    float infoPanelPosStep[4];     /* +0x5B10: per-frame position change */
    float infoPanelScale[4];       /* +0x5B20: information-panel scale */
    float infoPanelScaleStep[4];   /* +0x5B30: per-frame scale change */
    char *mess;                    /* +0x5B40: CGPSetMessage, indexed from OLMessTbl */
    char *interruptMess;           /* +0x5B44: CGPSetInterruptMess, indexed from CardPlayTMessList */
    char *permanentMess;           /* +0x5B48: CGPSetPermanentMess, indexed from PermMessTbl or NULL */
    u8 unmodeled_5B4C[0x5B50 - 0x5B4C];
    CardCursorControl cursor;      /* +0x5B50 */
    union {
        u8 cursorMoveStep[2];      /* +0x5B58: one movement counter per side */
        u16 cursorMoveAny;         /* both counters at once (lhu 0x00a2b0cc) */
    };
    u8 junkPick[2];                /* +0x5B5A: junk pile card under the cursor while one is browsed, one per pile
                                    * (player pile, enemy pile); CGPCursorMove2 steps it with lbu/sb */
    s16 cursorIndex;               /* +0x5B5C: position inside the selected zone */
    s16 cursorZone;                /* +0x5B5E: selected board zone */
    float highlightX;              /* +0x5B60 */
    float highlightY;              /* +0x5B64 */
    float highlightScaleX;         /* +0x5B68 */
    float highlightScaleY;         /* +0x5B6C */
    float cursorPos[4];            /* +0x5B70: cursor world position, w = 1.0 (CardMainInit 0x00a00ca8) */
    u8 unmodeled_5B80[0x5BBC - 0x5B80];
    void *cursorLex[4];            /* +0x5BBC: layout file of each cursor kind, loaded by CardMainInit */
    void *cursorXtx[4];            /* +0x5BCC: texture file of each cursor kind */
    u8 unmodeled_5BDC[0x5BE0 - 0x5BDC];
    u16 effectsActive;             /* +0x5BE0: only write is CardGameInit's clear (sh 0x00a252cc); no reader found */
    u16 effectsSet;                /* +0x5BE2: number of effect entries registered */
    u8 unmodeled_5BE4[0x5BF0 - 0x5BE4];
    CardEffect effects[64];        /* +0x5BF0: card-game effect table */
    CardModelEntry models[16];     /* +0x6BF0: models drawn after every fase (0x00a083a0 `addiu $16,$30,0x6BF0`) */
    u8 rotStageFlag;               /* +0x70F0: rotation-request byte cleared beside rotStageCooldown/rotStageId
                                    * (CardTitleInit 0x00a00978, CardMainInit 0x00a00d04, CardGameInit 0x00a253d4);
                                    * no reader found in the overlay */
    u8 rotStageCooldown;           /* +0x70F1: nonzero while a pending CGPRotStageSub request is in effect;
                                    * CGPRotStageSub sets it to 20 when it stores a new rotStageId */
    u8 rotStageActive;             /* +0x70F2: CardGameInit sets it to 1 (0x00a253b8); CardTitleInit (0x00a00984) and
                                    * CardMainInit (0x00a00d10) clear it; no reader found in the overlay */
    u8 rotStageId;                 /* +0x70F3: stage id CGPRotStageSub last stored for a rotation request */
    float rotStageVector[3];       /* +0x70F4: CardGameInit stores -0.9f, -20.0f, 0.0f (0x00a25368, 0x00a25364,
                                    * 0x00a253e0); CardTitleInit zeroes all three (0x00a00970, 0x00a00964, 0x00a00974) */
    u16 modelFadeInTimer;          /* +0x7100: ticks left for CardModelFadeIn (ov10/tu002) to run */
    u16 fadeOutTimer;              /* +0x7102: ticks left for CardFadeOut (ov10/tu002) to run */
    u8 fadeOutStep;                /* +0x7104: amount CardFadeOut subtracts per tick (read with lbu) */
    u8 fadeOutFlag;                /* +0x7105: only write is CardMainInit's clear (0x00a00cfc); no reader found */
    u8 fadeOutColor[4];            /* +0x7106: one level per model slot, CardFadeOut clamps each at zero */
    u8 fadeBaseColor[6];           /* +0x710A: CardMainInit sets the first four to 0x80 beside fadeOutColor;
                                    * nothing evidences the last two */
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

/*
 * Swap a disposal slot's stack with a battle slot's and start the ten-frame
 * slide that carries each of them to the other's position. Each argument is
 * the `battle` member of a CardBoardSlot; the swap also exchanges the 12 bytes
 * that follow the stack, which are the next slot's shunt position (ov10/cgp.c
 * models that span as its own CardMoveWork record).
 */
void CGPMoveDSBATTLEwork(CardLayerStack *disposal, CardLayerStack *battle);

#endif /* INCLUDE_OV10_CGP_H */
