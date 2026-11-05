#ifndef CCU_038_EXEC_SUB_H
#define CCU_038_EXEC_SUB_H

#include "ov10/cgp.h"
#include "ov10/card_fread.h"
#include "ov10/card_enemy.h"
#include "ov10/ccu_038_exec_sub.h"

/*
 * Zone codes of CardCursorPosition.zone: side 1 owns the rows 2..4, side 0
 * the rows 5..7.
 */

/* Effect codes CardSetEffect is called with (see CardSetEffect in ov10/cgp.c). */
#define CARD_EFFECT_SHOT 4             /* shot flying from the attacker's slot to the hit card */
#define CARD_EFFECT_HIT_NUMBER 11      /* damage number over a card, shown to the other side */
#define CARD_EFFECT_HIT_NUMBER_ROTATED 14 /* CARD_EFFECT_HIT_NUMBER for the board turned half a turn (rotStageId) */

/* Step a command jumps to when the player backs out: its handler completes the command there. */
#define CARD_STEP_FINISH 10

/* Marks CardCursorActive puts on a slot while a command asks for a target. */
#define CARD_CURSOR_SELECTABLE 1       /* a slot the player may choose */
#define CARD_CURSOR_CHOSEN 2           /* the slot already chosen */

/* Sounds the command cards play. */

#define CARD_PROMPT_TEXTURE ((void *) 0x01DB5800)   /* texture and model of the discard prompt's backdrop */
#define CARD_PROMPT_MODEL ((void *) 0x01DAAC00)
#define CARD_DAMAGE_TIMER_FRAMES 25    /* frames the damage number of a hit card stays (CardBattleLayer.damageTimer) */

/* Byte offsets CardPlayCommandExecute reaches the queue phase words by, from work + 12. */
#define CARD_QUEUE_BASE_OFFSET 12      /* addiu a3,s1,12 at 0x00a198d4 */
#define CARD_QUEUE_PLAYER_OFFSET 464   /* addiu v0,v0,464 at 0x00a198fc */
#define CARD_QUEUE_ENEMY_OFFSET 784    /* addiu v1,v0,784 at 0x00a198f8 */

/* Amounts of the command cards (cards.txt captions). */
#define CARD_SUPPLY_SHIP_RECOVERY 5    /* "Your deck recovers five cards" */
#define CARD_MARY_AND_SHELLEY_DRAWS 2  /* "Add two cards to your hand from the deck" */
#define CARD_UMN_INVASION_DRAWS 2      /* "Add two cards to your hand from the lost pile" */
#define CARD_ARTICLE_4_13_DAMAGE 6     /* "inflict 6 damage to the enemy's deck" per remaining R */
#define CARD_VIRGIL_DAMAGE 4           /* "4 damage is dealt to the enemy's deck for every R" */
#define CARD_WILHELM_DECK_DAMAGE_DELAY 20 /* frames from the card message to the 4 deck damage */
#define CARD_WILHELM_FINISH_DELAY 15   /* frames the deck damage effect plays before the command ends */

#define CARD_STACK_LAYERS 40          /* cards one board slot can stack */
#define CARD_DECK_VIEW_COUNT 5         /* cards of the deck a command lets the player look through */

/* The phase codes of side 1 follow those of side 0 by CARD_PHASES_PER_SIDE. */
#define CARD_PHASE_BATTLE_SIDE1 (CARD_PHASE_BATTLE + CARD_PHASES_PER_SIDE)
#define CARD_PHASE_ADJUST_SIDE1 (CARD_PHASE_ADJUST + CARD_PHASES_PER_SIDE)

/* Declared here: ov10/cgp.c defines CGPCursorMove as void; CardPlayCommandPlay's call sites compile
 * with an int result (measured: the register allocation of the loops after each call changes). */
extern s32 CGPCursorMove(s32 flag, CardGameWork *work);

/* Takes the work area untyped and converts it here: reading the member through
 * the caller's own pointer changes loop strength reduction in two of
 * CardPlayCommandPlay's search loops (commands 39 and 78). */
static inline CardDefinition *CardDefinitionsFor(void *gameWork) {
    CardGameWork *work = gameWork;
    return work->definitions;
}

static inline CardCursorControl *CardCursorFor(CardGameWork *work) {
    return &work->cursor;
}

static inline void CardCursorSetSelectionMode(CardGameWork *work, int mode) {
    work->cursor.selectionMode = mode;
}

static inline void CardCursorSetSide(CardGameWork *work, int side) {
    work->cursor.controllingSide = side;
}

static inline int CardCommandStep(CardGameWork *work) {
    return work->command.step;
}

static inline void CardCommandSetStep(CardGameWork *work, int step) {
    work->command.step = step;
}

static inline void CardCommandAdvance(CardGameWork *work) {
    work->command.step++;
}

static inline void CardCommandRewind(CardGameWork *work) {
    work->command.step--;
}

/* Unsigned: the queue entries take the phase word as is (lhu). */
static inline u16 CardCommandPhase(CardGameWork *work) {
    return work->command.phase;
}

static inline void CardCommandMarkRecovery(CardGameWork *work) {
    work->command.recoveryApplied = 1;
}

static inline u16 CardCommandHeld(CardGameWork *work, unsigned side) {
    if (side == 0)
        return work->command.input[0].held;
    return work->command.input[1].held;
}

static inline u16 CardCommandPressed(CardGameWork *work, unsigned side) {
    if (side == 0)
        return work->command.input[0].pressed;
    return work->command.input[1].pressed;
}

static inline u16 CardCommandRepeated(CardGameWork *work, unsigned side) {
    if (side == 0)
        return work->command.input[0].repeat;
    return work->command.input[1].repeat;
}

static inline CardLayerStack *CardSelectBattle(CardPlaySide *side, int index,
                                               int fromDisposal) {
    CardLayerStack *battling = &side->battle[index].battle;
    CardLayerStack *disposed = &side->disposal[index].battle;
    if (fromDisposal)
        battling = disposed;
    return battling;
}

/*
 * CardSelectSlot, CardBattleSlots and CardBattlePosition have no callers in
 * this unit. They stay because the object file keeps compiler-numbered local
 * labels ($L1003..$L1005 differ without them), not because any code uses them.
 */
static inline CardBoardSlot *CardSelectSlot(CardPlaySide *side, int index,
                                            int fromDisposal) {
    CardBoardSlot *battling = &side->battle[index];
    CardBoardSlot *disposed = &side->disposal[index];
    if (fromDisposal)
        battling = disposed;
    return battling;
}

static inline CardBoardSlot *CardBattleSlots(CardPlaySide *side) {
    return side->battle;
}

static inline float *CardBattlePosition(CardPlaySide *side, int index) {
    return side->battle[index].battle.position;
}

static inline CardPlaySide *CardPlayerSide(CardGameWork *work) {
    return &work->player;
}

static inline CardPlaySide *CardEnemySide(CardGameWork *work) {
    return &work->enemy;
}

#endif
