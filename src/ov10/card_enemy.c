/*
 * OV10 original TU 9: 0x00a324b8..0x00a34190 (15 functions)
 */
#include "common.h"
#include "ov10/cgp.h"

/*
 * One CPU deck record inside cpudeck (0x80 bytes). InitCpuDeck slices 48
 * consecutive records into EnemyDeckListStr, DeckGeneLst, EnemyDeckLst and
 * DeckDataLst; CardCopyRom2Deck (ov10/card_fread.c) reads the deck field of
 * EnemyDeckLst[enemyId] as the ROM deck's card ids, which fixes that field's
 * width. The other fields' contents are not read by any recovered function yet.
 */
typedef struct CpuDeckRecord {
    char name[0x20];
    char gene[0x10];
    char deck[0x28];
    char data[0x28];
} CpuDeckRecord;

#define CPU_DECK_COUNT 48

extern CpuDeckRecord cpudeck[CPU_DECK_COUNT];
extern char *EnemyDeckListStr[CPU_DECK_COUNT];
extern char *DeckGeneLst[CPU_DECK_COUNT];
extern char *EnemyDeckLst[CPU_DECK_COUNT];
extern char *DeckDataLst[CPU_DECK_COUNT];

void InitCpuDeck(void) {
    int i;

    xglCdReadFile("data\\carddata\\deckdata\\deckdata.dat", cpudeck, 0, 1);
    for (i = 0; i < CPU_DECK_COUNT; i++) {
        EnemyDeckListStr[i] = cpudeck[i].name;
        DeckGeneLst[i] = cpudeck[i].gene;
        EnemyDeckLst[i] = cpudeck[i].deck;
        DeckDataLst[i] = cpudeck[i].data;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov10/card_enemy", CardEnemyMove);

INCLUDE_ASM("asm/nonmatchings/ov10/card_enemy", CardEnemySet);

/* CardPlayHandCnt (still assembler, ov10/tu002) counts the negative card ids
 * of one side's CardHand (include/ov10/cgp.h, owned by ov10/tu008). */
extern int CardPlayHandCnt(CardHand *hand);

/* AI command-request handler; defined in ov10/tu010 (still assembler). */
extern void CC_Kara_CommRequest(int side, CardGameWork *work);

void CardEnemyCommandRequest(int side, CardGameWork *work) {
    CardPlayHandCnt(side == 0
        ? &work->player.hand
        : &work->enemy.hand);
    CC_Kara_CommRequest(side, work);
}

/*
 * The operation slot CGPEnemyExecOperation acts on: its first field picks the
 * effect (4 and 5 for the AI's own operation cards, 3 and 7 for an opponent
 * slot) and its second field the board slot the effect applies to.
 */
typedef struct CardEnemyOpeArg {
    s16 effect;
    s16 slot;
} CardEnemyOpeArg;

/* True when the side may still play the operation card; defined in ov10/cgp.c
 * (still assembler). */
extern int CGPChkOpePlayable(int side, CardGameWork *work, int cardId);

/* Damage-command searches over the board; both defined in
 * ov10/battle_area_check_enemy.c (still assembler). CheckDamageCommand answers
 * with a slot index, or -1 when it finds none. */
extern int CheckDamageCommand(int side, CardGameWork *work, int kind);
extern int CheckXDamageCommand(int side, CardGameWork *work, int kind);

/* Runs one operation card for the AI; defined below in this TU. */
extern int CGPEnemyExecOperation(int side, CardGameWork *work, int operationIndex,
                                 CardEnemyOpeArg *arg);

int CardEnemyOperationPlay(int side, CardGameWork *work) {
    CardPlaySide *ownSide;
    CardPlaySide *oppSide;
    CardEnemyOpeArg arg;
    int effect;
    int damageSlot;
    int i;
    int j;

    if (side == 0) {
        ownSide = &work->player;
        oppSide = &work->enemy;
        effect = 4;
    } else {
        ownSide = &work->enemy;
        oppSide = &work->player;
        effect = 5;
    }
    for (i = 0; i < 4; i++) {
        if (ownSide->operations[i] < 0) {
            continue;
        }
        if (CGPChkOpePlayable(side, work, ownSide->operations[i]) == 0) {
            continue;
        }
        switch (ownSide->operations[i]) {
        case 139:
            damageSlot = CheckDamageCommand(side, work, 3);
            if (damageSlot == -1) {
                continue;
            }
            arg.slot = damageSlot;
            arg.effect = effect;
            CGPEnemyExecOperation(side, work, i, &arg);
            break;
        case 141:
            for (j = 0; j < 4; j++) {
                if (oppSide->operations[j] < 0) {
                    continue;
                }
                if (side == 0) {
                    arg.effect = 3;
                } else {
                    arg.effect = 7;
                }
                arg.slot = j;
                CGPEnemyExecOperation(side, work, i, &arg);
                break;
            }
            break;
        case 142:
            CGPEnemyExecOperation(side, work, i, (CardEnemyOpeArg *) 0);
            break;
        case 143:
            if (CheckXDamageCommand(side, work, 2) == 1) {
                CGPEnemyExecOperation(side, work, i, (CardEnemyOpeArg *) 0);
            }
            break;
        }
    }
    return 1;
}

/* AI command-play handler; defined in ov10/tu010 (still assembler). */
extern void CC_Kara_CommPlay(int side, CardGameWork *work);

void CardEnemyCommandPlay(int side, CardGameWork *work) {
    CC_Kara_CommPlay(side, work);
}

/* AI first-answer handler; defined in ov10/tu010 (still assembler). */
extern void CC_Kara_1stAnswer(int side, CardGameWork *work);

void CardEnemy1stAnswer(int side, CardGameWork *work) {
    CardPlayHandCnt(side == 0
        ? &work->player.hand
        : &work->enemy.hand);
    CC_Kara_1stAnswer(side, work);
}

/* The draw pile's non-negative entries (ov10/card_fread.c). */
extern int CardPlayYamaCnt(CardPlaySide *side);

/* Card master condition check for the AI; defined in ov10/card_fread.c
 * (still assembler). */
extern int CardPlayChkCondition(CardDefinition *definitions, CardPlaySide *side,
                                int cardId, CardGameWork *work);

/* Plays the AI's chosen answer command card; defined in ov10/cgp.c (still
 * assembler). */
extern void CGPAnswerSub(CardGameWork *work, CardPlaySide *side, int handIndex);

int CardEnemyAnswer(int side, CardGameWork *work) {
    CardPlaySide *ownSide;
    int handCount;
    int i;

    CardHand *hand;

    if (side == 0) {
        ownSide = &work->player;
        hand = &work->player.hand;
    } else {
        ownSide = &work->enemy;
        hand = &work->enemy.hand;
    }
    handCount = CardPlayHandCnt(hand);
    for (i = 0; i < handCount; i++) {
        if (hand->cards[i] == 0x52 || hand->cards[i] == 0x74) {
            if (CardPlayYamaCnt(ownSide) < 2) {
                return 0;
            }
            if (CardPlayChkCondition(work->definitions, ownSide,
                                     hand->cards[i], work) == 0) {
                CGPAnswerSub(work, ownSide, i);
                return 1;
            }
        }
    }
    return 0;
}

/* Sets the card game work area's error message; defined in ov10/cgp.c. */
extern void CGPSetErrorMessPlus(s8 code, CardGameWork *work, u8 reason, s16 value);

/*
 * The board position this TU hands to the CCU/CCC handlers of ov10/tu003.
 * That TU keeps its own record of the same storage TU-local, so no header
 * declares it yet and each caller models the members it fills.
 */
typedef struct CardEnemyCursor {
    s16 zone;
    s16 index;
    s16 cardId;
    u8 unmodeled_06[2];
} CardEnemyCursor;

/* Command handlers for the cards the AI answers with; defined in
 * ov10/ccu_038_exec_sub.c. */
extern void CCU038ExecSub(CardHand *hand, CardEnemyCursor *position);
extern void CCU055ExecSub(CardHand *hand, CardEnemyCursor *position);
extern void CCU058ExecSub(int side, CardPlaySide *owner, CardEnemyCursor *position);
extern void CCU061ExecSub(CardPlaySide *side, CardEnemyCursor *position);
extern void CCC07ExecSub(CardGameWork *work, CardHand *hand, CardEnemyCursor *position);

/* The value of one board slot's stack; defined in
 * ov10/battle_area_check_enemy.c. */
extern int GetPrice(CardLayerStack *stack);

/* Answers with the retreat command's target slot, or -1 when there is none;
 * defined in ov10/battle_area_check_enemy.c (still assembler). */
extern int CheckTaikyakuCommand(int side, CardGameWork *work);

int CardEnemyAnswerCardxx(int side, CardGameWork *work, s16 cardId) {
    CardPlaySide *ownSide;
    CardPlaySide *oppSide;
    CardEnemyCursor position;
    CardEnemyCursor *target;
    int discardZone;
    int operationZone;
    int boardZone;
    int retreatZone;
    int result;
    int best;
    int found;
    int foundOperation;
    int foundBattle;
    int foundDisposal;
    int foundRetreat;
    int answered;
    int retreatSlot;
    int i;

    boardZone = 2;
    target = 0;
    result = 0;
    retreatZone = 4;
    ownSide = &work->player;
    oppSide = &work->enemy;
    operationZone = 3;
    discardZone = 6;
    if (side != 0) {
        ownSide = &work->enemy;
        oppSide = &work->player;
        operationZone = 7;
        boardZone = 6;
        discardZone = 2;
        retreatZone = 5;
    }
    switch (cardId) {
    case 38:
        switch (work->command.step) {
        case 0:
            found = 0;
            for (i = 0; i < 4; i++) {
                if (ownSide->disposal[i].battle.layers[0].cardId >= 0) {
                    found = 1;
                    break;
                }
            }
            if (found == 0) {
                result = 1;
                break;
            }
            work->command.step++;
            CGPSetErrorMessPlus(-1, work, 36, 38);
            break;
        case 1:
            for (i = 0; i < 4; i++) {
                if (ownSide->disposal[i].battle.layers[0].cardId >= 0) {
                    position.zone = discardZone;
                    position.index = i;
                    target = &position;
                    break;
                }
            }
            CCU038ExecSub(&ownSide->hand, target);
            result = 1;
            break;
        }
        break;
    case 55:
        switch (work->command.step) {
        case 0:
            foundOperation = 0;
            for (i = 0; i < 4; i++) {
                if (oppSide->operations[i] >= 0) {
                    foundOperation = 1;
                    break;
                }
            }
            if (foundOperation == 0) {
                result = 1;
                break;
            }
            work->command.step++;
            CGPSetErrorMessPlus(-1, work, 36, 55);
            break;
        case 1:
            for (i = 0; i < 4; i++) {
                if (oppSide->operations[i] >= 0) {
                    position.zone = operationZone;
                    position.index = i;
                    target = &position;
                    break;
                }
            }
            CCU055ExecSub(&oppSide->hand, target);
            result = 1;
            break;
        }
        break;
    case 58:
        switch (work->command.step) {
        case 0:
            foundBattle = 0;
            for (i = 0; i < 4; i++) {
                if (oppSide->battle[i].battle.layers[0].cardId >= 0
                    && (oppSide->battle[i].battle.flags & 2) == 0) {
                    foundBattle = 1;
                    break;
                }
            }
            if (foundBattle == 0) {
                result = 1;
                break;
            }
            work->command.step++;
            CGPSetErrorMessPlus(-1, work, 36, 58);
            break;
        case 1:
            result = 0;
            best = 0;
            for (i = 0; i < 4; i++) {
                if (oppSide->battle[i].battle.layers[0].cardId >= 0
                    && best < GetPrice(&oppSide->battle[i].battle)
                    && (oppSide->battle[i].battle.flags & 2) == 0) {
                    result = i;
                    best = GetPrice(&oppSide->battle[i].battle);
                }
            }
            for (i = 0; i < 4; i++) {
                if (oppSide->battle[i].battle.layers[0].cardId >= 0) {
                    position.zone = boardZone;
                    position.index = result;
                    target = &position;
                    break;
                }
            }
            CCU058ExecSub(side, oppSide, target);
            result = 1;
            break;
        }
        break;
    case 61:
        switch (work->command.step) {
        case 0:
            foundDisposal = 0;
            for (i = 0; i < 4; i++) {
                if (oppSide->disposal[i].battle.layers[0].cardId >= 0
                    && (oppSide->disposal[i].battle.flags & 2) == 0) {
                    foundDisposal = 1;
                    break;
                }
            }
            if (foundDisposal == 0) {
                result = 1;
                break;
            }
            work->command.step++;
            CGPSetErrorMessPlus(-1, work, 36, 61);
            break;
        case 1:
            result = 0;
            best = 0;
            for (i = 0; i < 4; i++) {
                if (oppSide->disposal[i].battle.layers[0].cardId >= 0
                    && best < GetPrice(&oppSide->disposal[i].battle)
                    && (oppSide->disposal[i].battle.flags & 2) == 0) {
                    result = i;
                    best = GetPrice(&oppSide->disposal[i].battle);
                }
            }
            for (i = 0; i < 4; i++) {
                if (oppSide->disposal[i].battle.layers[0].cardId >= 0) {
                    position.zone = boardZone;
                    position.index = result;
                    target = &position;
                    break;
                }
            }
            CCU061ExecSub(oppSide, target);
            result = 1;
            break;
        }
        break;
    case 62:
        switch (work->command.step) {
        case 0:
            foundRetreat = 0;
            for (i = 0; i < 4; i++) {
                if (oppSide->disposal[i].battle.layers[0].cardId >= 0) {
                    foundRetreat = 1;
                    break;
                }
            }
            answered = 0;
            if (foundRetreat != 0) {
                for (i = 0; i < 4; i++) {
                    if (oppSide->battle[i].battle.layers[0].cardId < 0) {
                        continue;
                    }
                    if (CheckTaikyakuCommand(side, work) == -1) {
                        result = 1;
                        break;
                    }
                    work->command.step++;
                    CGPSetErrorMessPlus(-1, work, 36, 62);
                    answered = 1;
                    break;
                }
            } else {
                result = 1;
            }
            if (answered == 0) {
                result = 1;
            }
            break;
        case 1:
            retreatSlot = CheckTaikyakuCommand(side, work);
            if (retreatSlot == -1) {
                for (i = 0; i < 4; i++) {
                    if (oppSide->battle[i].battle.layers[0].cardId >= 0) {
                        position.index = i;
                        position.zone = retreatZone;
                        target = &position;
                        break;
                    }
                }
            } else {
                position.index = retreatSlot;
                position.zone = retreatZone;
                target = &position;
            }
            CCC07ExecSub(work, &oppSide->hand, target);
            result = 1;
            break;
        }
        break;
    }
    return result;
}

INCLUDE_ASM("asm/nonmatchings/ov10/card_enemy", CGPEnemyExecCommand);

/* Reports the operation card the AI plays; defined in ov10/cgp.c. */
extern void CGPSetErrorMessPlus(s8 code, CardGameWork *work, u8 reason, s16 value);

/* Board effect request; still assembler (ov10/cgp.c declares the same
 * prototype). */
extern void CardSetEffect(int side, CardGameWork *work, u8 kind, s8 amount,
                          int sound, void *position, void *target);

/* Discards one operation card of a side; still assembler (ov10/card_fread.c). */
extern void CardPlayDesertOperation(CardPlaySide *side, int operationIndex);

/* Pays one card of the side's cost pile; still assembler (ov10/card_fread.c).
 * ov10/cgp.c declares it over its own partial play record; here the whole
 * side record is at hand and the hand it reads starts at offset zero of it. */
extern void CardPlayCostCard(CardPlaySide *side);

/* The card 143 handler; defined in ov10/cgp.c (still assembler). */
extern void CCO143ExecSub(int side, CardGameWork *work, CardPlaySide *ownSide,
                          CardPlaySide *oppSide);

int CGPEnemyExecOperation(int side, CardGameWork *work, int operationIndex,
                          CardEnemyOpeArg *arg) {
    CardPlaySide *ownSide;
    CardPlaySide *oppSide;
    int result;
    int effectSide;
    int i;

    ownSide = &work->player;
    oppSide = &work->enemy;
    result = 0;
    if (side != 0) {
        ownSide = &work->enemy;
        oppSide = &work->player;
    }
    CGPSetErrorMessPlus(-1, work, 36, ownSide->operations[operationIndex]);
    switch (ownSide->operations[operationIndex]) {
    case 139:
        if (arg == (CardEnemyOpeArg *) 0) {
            break;
        }
        oppSide->battle[arg->slot].battle.layers[0].life -= 3;
        if (oppSide->battle[arg->slot].battle.layers[0].life < 0) {
            oppSide->battle[arg->slot].battle.layers[0].life = 0;
        }
        switch (arg->effect) {
        case 5:
            effectSide = 0;
            break;
        case 4:
        default:
            effectSide = 1;
            break;
        }
        result = -1;
        CardSetEffect(effectSide, work, 5, 0, 0x20008,
                      oppSide->battle[arg->slot].battle.position,
                      &oppSide->battle[arg->slot].battle);
        CardSetEffect(effectSide, work, 13, 3, 0,
                      oppSide->battle[arg->slot].battle.position,
                      &oppSide->battle[arg->slot].battle);
        break;
    case 141:
        CardPlayDesertOperation(oppSide, arg->slot);
        result = -1;
        break;
    case 142:
        for (i = 3; i >= 0; i--) {
            CardPlayCostCard(oppSide);
        }
        CardSetEffect(side, work, 5, 0, 0x20008, 0, 0);
        CardSetEffect(side, work, 13, 4, 0, 0, 0);
        result = -1;
        break;
    case 143:
        CCO143ExecSub(side, work, ownSide, oppSide);
        result = -1;
        break;
    }
    if (result < 0) {
        CardPlayDesertOperation(ownSide, operationIndex);
    }
    return result;
}

INCLUDE_ASM("asm/nonmatchings/ov10/card_enemy", CGPEnemyLv10SetSub);

/* AI comm-phase handler; defined in ov10/tu010 (still assembler). */
extern int CC_Kara_CommFase(int side, CardGameWork *work);

/* Advances the card game turn; defined in ov10/tu008 (still assembler). */
extern void CGPNextTurnSub(CardGameWork *work);

void CardEnemyComm(int side, CardGameWork *work) {
    if (CC_Kara_CommFase(side, work) == 1) {
        CGPNextTurnSub(work);
    }
}

/* The draw pile's non-negative entries (ov10/card_fread.c). */
extern int CardPlayYamaCnt(CardPlaySide *side);

/* Compacts the draw pile after a card leaves it; still assembler (ov10/card_fread.c). */
extern void CardPlayCleanYama(CardPlaySide *side);

/* Reshuffles the draw pile; still assembler (ov10/card_fread.c). */
extern void CardPlayShuffleYama(CardPlaySide *side);

/*
 * Card id 7 is the AI's search target here: the same draw-to-hand/CleanYama/
 * ShuffleYama sequence, keyed off the same card id, is already accepted for
 * the manual case in ccu_038_exec_sub.c (CCU038ExecSub step 4.3,
 * `ownSide->deck[D_00A57830] == 7`).
 */
void CardEnemyAnswerSionSearch(int side, CardGameWork *work) {
    CardPlaySide *ownSide = side == 0 ? &work->player : &work->enemy;
    int count;
    int i;

    count = CardPlayYamaCnt(ownSide);
    for (i = 0; i < count; i++) {
        if (ownSide->deck[i] == 7) {
            ownSide->hand.cards[CardPlayHandCnt(&ownSide->hand)] = ownSide->deck[i];
            ownSide->deck[i] = -1;
            CardPlayCleanYama(ownSide);
            CardPlayShuffleYama(ownSide);
        }
    }
}

/* AI end-of-phase handler; defined in ov10/tu010 (still assembler). */
extern int CC_Kara_EndFase(int side, CardGameWork *work);

/* Advances the card game turn; defined in ov10/tu008 (still assembler). */
extern void CGPNextTurnSub(CardGameWork *work);

void CardEnemyEnd(int side, CardGameWork *work) {
    int endFaseResult = 1;

    if (CardPlayHandCnt(side == 0
            ? &work->player.hand
            : &work->enemy.hand) >= 7) {
        endFaseResult = CC_Kara_EndFase(side, work);
    }
    if (endFaseResult == 0) {
        CGPNextTurnSub(work);
    }
}
