/*
 * OV10 original TU 9: 0x00a324b8..0x00a34190 (15 functions)
 */
#include "common.h"
#include "ov10/cgp.h"
#include "ov10/battle_area_check_enemy.h"

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

/* Advances the card game turn; defined in ov10/tu008 (still assembler). */
extern void CGPNextTurnSub(CardGameWork *work);

/* Board moves of the side's positions; defined in ov10/cgp.c. */
extern void CGPShuntPosSub(CardPlaySide *side);
extern void CGPRecalcPosSub(CardPlaySide *side);

/* Frames the move and set phases wait after their AI handler finishes
 * (`addiu $3,$0,0xF` and `sb` at 0x00a3260c..0x00a32610 and 0x00a326b8..0x00a326c0). */
#define CARD_ENEMY_WAIT_FRAMES 15

/* aiPhase values of CardEnemyMove (switch at 0x00a3259c..0x00a325c0). */
#define CARD_ENEMY_MOVE_START 0     /* shunt the positions, then fall into the search */
#define CARD_ENEMY_MOVE_SEARCH 1    /* run CC_Kara_MoveFase until it finishes */
#define CARD_ENEMY_MOVE_WAIT 2      /* count the wait down, then end the turn */

void CardEnemyMove(int side, CardGameWork *work) {
    /* Frames left in the wait (signed byte, `lb` at 0x00a32624, `sb` at 0x00a32610). */
    static s8 Timer;
    CardPlaySide *ownSide = side == 0 ? &work->player : &work->enemy;

    switch (work->command.aiPhase) {
    case CARD_ENEMY_MOVE_START:
        CGPShuntPosSub(ownSide);
        work->command.aiPhase++;
        /* fall through */
    case CARD_ENEMY_MOVE_SEARCH:
        if (CC_Kara_MoveFase(side, work) == 0) {
            CGPRecalcPosSub(ownSide);
            work->command.aiPhase++;
            Timer = CARD_ENEMY_WAIT_FRAMES;
            /* Returns instead of leaving the switch: the break form is 8 bytes longer. */
            return;
        }
        break;
    case CARD_ENEMY_MOVE_WAIT:
        if (Timer > 0) {
            Timer--;
        } else {
            CGPNextTurnSub(work);
        }
        break;
    }
}

/* Battle slot count of one side (ov10/card_fread.c, still assembler). */
extern int CardPlayBattleCnt(CardPlaySide *side);

/* aiPhase values of CardEnemySet (switch at 0x00a32688..0x00a32690). */
#define CARD_ENEMY_SET_CHOOSE 0     /* run CC_Kara_SetFase until it finishes */
#define CARD_ENEMY_SET_WAIT 1       /* count the wait down, then clear the flags and end the turn */

/* Card whose base-layer flag byte the set phase clears once the wait is over
 * (`lh` / `bnel` against 0xE at 0x00a32710..0x00a32714). */
#define CARD_ID_FLAG_CLEARED 14

void CardEnemySet(int side, CardGameWork *work) {
    /* Frames left in the wait (signed byte, `lb` at 0x00a326d4, `sb` at 0x00a326c0). */
    static s8 Timer;
    CardPlaySide *ownSide = side == 0 ? &work->player : &work->enemy;
    int i;
    /*
     * The value the battle count is compared with. The original tests it against
     * a register that already holds 1 (`beq $2,$17` at 0x00a326f8, $17 being the
     * phase in this arm); the literal 1 in the comparison changes the entry block
     * (0x00a32664..0x00a3266c) and the branch at 0x00a32692, a local keeps it.
     */
    int singleBattleCnt = 1;

    switch (work->command.aiPhase) {
    case CARD_ENEMY_SET_CHOOSE:
        if (CC_Kara_SetFase(side, work) == 0) {
            Timer = CARD_ENEMY_WAIT_FRAMES;
            work->command.aiPhase++;
            /* Returns instead of leaving the switch: the break form is 8 bytes longer. */
            return;
        }
        break;
    case CARD_ENEMY_SET_WAIT:
        if (Timer > 0) {
            Timer--;
        } else {
            if (CardPlayBattleCnt(ownSide) != singleBattleCnt) {
                for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                    if (ownSide->battle[i].battle.layers[0].cardId == CARD_ID_FLAG_CLEARED) {
                        if (ownSide->battle[i].battle.layers[0].flag != 0) {
                            ownSide->battle[i].battle.layers[0].flag = 0;
                        }
                    }
                }
            }
            CGPNextTurnSub(work);
        }
        break;
    }
}

/* CardPlayHandCnt (still assembler, ov10/tu002) counts the negative card ids
 * of one side's CardHand (include/ov10/cgp.h, owned by ov10/tu008). */
extern int CardPlayHandCnt(CardHand *hand);

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

/* True when the side may still play the operation card; defined in ov10/cgp.c. */
extern int CGPChkOpePlayable(int side, CardGameWork *work, s16 command);

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

void CardEnemyCommandPlay(int side, CardGameWork *work) {
    CC_Kara_CommPlay(side, work);
}

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
extern void CGPSetErrorMessPlus(s8 side, CardGameWork *work, u8 reason, s16 value);

/* Command handlers for the cards the AI answers with; defined in
 * ov10/ccu_038_exec_sub.c. */
extern void CCU038ExecSub(CardHand *hand, CardCursorPosition *position);
extern void CCU055ExecSub(CardHand *hand, CardCursorPosition *position);
extern void CCU058ExecSub(int side, CardPlaySide *owner, CardCursorPosition *position);
extern void CCU061ExecSub(CardPlaySide *side, CardCursorPosition *position);
extern void CCC07ExecSub(CardGameWork *work, CardHand *hand, CardCursorPosition *position);

int CardEnemyAnswerCardxx(int side, CardGameWork *work, s16 cardId) {
    CardPlaySide *ownSide;
    CardPlaySide *oppSide;
    CardCursorPosition position;
    CardCursorPosition *target;
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

/*
 * Cursor zones the command targets name: 6 and 2 are the disposal rows
 * (CardEnemyAnswerCardxx: discardZone is 6 for side 0 and 2 for side 1);
 * every other zone is a battle slot. The first test loads the zone with `lh`
 * (0x00a332b0) and compares it with both values.
 */
#define CARD_ZONE_DISCARD_A 6
#define CARD_ZONE_DISCARD_B 2

/* Operation cards per side (loop bound `slti $2,$5,4` at 0x00a33414). */
#define CARD_OPERATION_SLOTS 4

/* Case 109 (CardSetEffect kinds at 0x00a3364c / 0x00a33678, sound id `lui`/`ori` at 0x00a33668 / 0x00a3367c). */
#define CARD_EFFECT_TARGET_DOWN 10
#define CARD_EFFECT_POINTS 12
#define CARD_EFFECT_SOUND 0x2000D

/* Case 95 compares at most this many cards from the end of the draw pile
 * (`slti $2,$21,6` at 0x00a338c4, counter starts at 1); case 106 takes at most
 * this many cards from the junk pile into the hand (`slti $2,$11,2` at 0x00a33830). */
#define CARD_DRAW_WINDOW 5
#define CARD_JUNK_PICKUP_MAX 2

/* Command card handlers defined in ov10/ccu_038_exec_sub.c (CCC07ExecSub is declared above). */
extern void CCC02ExecSub(u8 *commandState);
extern void CCC03ExecSub(int side, CardGameWork *work, CardHand *hand, CardLayerStack *card);
extern void CCC06ExecSub(int side, CardGameWork *work, CardHand *hand);
extern void CCC10ExecSub(int side, CardGameWork *work, CardHand *hand, CardCursorPosition *position);
extern void CCC26ExecSub(int side, CardGameWork *work, CardPlaySide *owner, CardCursorPosition *position);
extern void CCC30ExecSub(int side, CardGameWork *work, CardLayerStack *card);
extern void CCC31ExecSub(int side, CardGameWork *work, CardLayerStack *card);
extern void CCC38ExecSub(int side, CardGameWork *work, CardPlaySide *owner, CardCursorPosition *position);

/* Draw-pile and junk-pile helpers of ov10/card_fread.c. */
extern int CardPlayDrawCard(CardHand *hand);
extern int CardPlayRecavery(CardHand *hand);
extern int CardPlayJunkCnt(CardPlaySide *side);
extern void CardPlayCleanJunk(CardPlaySide *side);
extern int CardPlayCommandPlay(u16 side, u16 command, CardGameWork *work, s16 operand, s16 target);

/* Board effect request; still assembler (ov10/cgp.c declares the same
 * prototype). */
extern void CardSetEffect(int side, CardGameWork *work, u8 kind, s8 amount,
                          int sound, void *position, void *target);

/* Discards one operation card of a side; still assembler (ov10/card_fread.c). */
extern void CardPlayDesertOperation(CardPlaySide *side, int operationIndex);

/* Compacts the draw pile after a card leaves it; still assembler (ov10/card_fread.c). */
extern void CardPlayCleanYama(CardPlaySide *side);

/* Reshuffles the draw pile; still assembler (ov10/card_fread.c). */
extern void CardPlayShuffleYama(CardPlaySide *side);

/* The battle or disposal stack a cursor names; zones 6 and 2 are discard zones. */
static inline CardLayerStack *CardEnemyTargetStack(CardPlaySide *side, CardCursorPosition *target) {
    if (target->zone == CARD_ZONE_DISCARD_A || target->zone == CARD_ZONE_DISCARD_B) {
        return &side->disposal[target->index].battle;
    }
    return &side->battle[target->index].battle;
}

/*
 * Command card ids dispatched by CGPEnemyExecCommand: the jump table at 0x00a4f7b0 has 37 entries
 * for ids 83..119 (`addiu $3,$3,-0x53`, `sltiu $2,$3,0x25` at 0x00a33280/0x00a33284); ids without
 * a case here share the table's default target 0x00a33938 and play through the generic handler.
 * The names are the ones ov10/ccu_038_exec_sub.c gives the same ids in CardPlayCommandCondition
 * (they describe the play condition of the card); each comment is the case's target address in
 * CGPEnemyExecCommand.
 */
enum CardCommandId {
    CARD_COND_OWN_FLAG2_CARD_83 = 83,  /* 0x00a332a8 */
    CARD_COND_OWN_CARD_NOT_FLAG4_84 = 84,  /* 0x00a33318 */
    CARD_COND_OWN_ANY_CARD_87 = 87,  /* 0x00a33390 */
    CARD_COND_OTHER_BATTLE_EMPTY_DISPOSAL_88 = 88,  /* 0x00a333a8 */
    CARD_COND_OTHER_BATTLE_EMPTY_DISPOSAL_89 = 89,  /* 0x00a333a8 */
    CARD_COND_OTHER_BATTLE_CARD_90 = 90,  /* 0x00a333c8 */
    CARD_COND_OTHER_BATTLE_CARD_91 = 91,  /* 0x00a333c8 */
    CARD_COND_ALWAYS_MET_93 = 93,  /* 0x00a333e8 */
    CARD_COND_ALWAYS_MET_95 = 95,  /* 0x00a33850 */
    CARD_COND_OTHER_OPERATION_96 = 96,  /* 0x00a33400 */
    CARD_COND_OTHER_FLAG1_CARD_OR_OPERATION_97 = 97,  /* 0x00a33448 */
    CARD_COND_OTHER_DISPOSAL_FLAG2_DEF4_98 = 98,  /* 0x00a33498 */
    CARD_COND_OTHER_DISPOSAL_DEF_FLAG1_99 = 99,  /* 0x00a334f8 */
    CARD_COND_OTHER_TWO_LAYERS_103 = 103,  /* 0x00a33528 */
    CARD_COND_JUNK_FLAGS_105 = 105,  /* 0x00a3330c */
    CARD_COND_TWO_JUNK_FLAG1_106 = 106,  /* 0x00a33798 */
    CARD_COND_OWN_BATTLE_NOT_CHKSUB_107 = 107,  /* 0x00a33588 */
    CARD_COND_OWN_SLOT_CARD_109 = 109,  /* 0x00a335a8 */
    CARD_COND_OTHER_DISPOSAL_CARD_111 = 111,  /* 0x00a33698 */
    CARD_COND_OWN_SLOT_CARD_112 = 112,  /* 0x00a33708 */
    CARD_COND_OWN_FLAG2_CARD_117 = 117,  /* 0x00a332a8 */
    CARD_COND_JUNK_IDENTITY_OWN_FLAG1_118 = 118,  /* 0x00a3330c */
    CARD_COND_OTHER_DISPOSAL_CARD_119 = 119  /* 0x00a33778 */
};

int CGPEnemyExecCommand(int side, CardGameWork *work, CardCursorPosition *targets, int count) {
    CardPlaySide *ownSide;
    CardPlaySide *oppSide;
    int opponentSide;
    int result;

    opponentSide = 1;
    ownSide = &work->player;
    result = 0;
    oppSide = &work->enemy;
    if (side != 0) {
        ownSide = &work->enemy;
        oppSide = &work->player;
        opponentSide = 0;
    }
    /*
     * The command card id is the halfword at +0x198 of the side (`lh` at 0x00a3327c), the
     * playedCard member of the CardBoardSlot union (battle[0]); the other slots never use it.
     */
    /*
     * Cases are command card ids 83..119, dispatched through the jump table at
     * 0x00a4f7b0 (`addiu $3,$3,-0x53`, `sltiu $2,$3,0x25`); every other id
     * plays through the generic command handler in the default arm.
     */
    switch (ownSide->battle[0].cmdView.playedCard) {
    case CARD_COND_OWN_FLAG2_CARD_83:
    case CARD_COND_OWN_FLAG2_CARD_117:
        if (targets == 0) {
            break;
        }
        CCC02ExecSub(&CardEnemyTargetStack(ownSide, targets)->flags);
        result = 1;
        break;
    case CARD_COND_OWN_CARD_NOT_FLAG4_84:
        if (targets == 0) {
            break;
        }
        CCC03ExecSub(side, work, &ownSide->hand, CardEnemyTargetStack(ownSide, targets));
        result = 1;
        break;
    case CARD_COND_JUNK_FLAGS_105:
    case CARD_COND_JUNK_IDENTITY_OWN_FLAG1_118:
        result = 1;
        break;
    case CARD_COND_OWN_ANY_CARD_87:
        CCC06ExecSub(side, work, &ownSide->hand);
        result = 1;
        break;
    case CARD_COND_OTHER_BATTLE_EMPTY_DISPOSAL_88:
    case CARD_COND_OTHER_BATTLE_EMPTY_DISPOSAL_89:
        if (targets == 0) {
            break;
        }
        CCC07ExecSub(work, &oppSide->hand, targets);
        result = 1;
        break;
    case CARD_COND_OTHER_BATTLE_CARD_90:
    case CARD_COND_OTHER_BATTLE_CARD_91:
        if (targets == 0) {
            break;
        }
        CCC10ExecSub(opponentSide, work, &oppSide->hand, targets);
        result = 1;
        break;
    case CARD_COND_ALWAYS_MET_93:
        CardPlayDrawCard(&ownSide->hand);
        CardPlayDrawCard(&ownSide->hand);
        result = 1;
        break;
    case CARD_COND_OTHER_OPERATION_96: {
        int i;

        for (i = 0; i < CARD_OPERATION_SLOTS; i++) {
            if (oppSide->operations[i] >= 0) {
                CardPlayDesertOperation(oppSide, i);
                result = 1;
                break;
            }
        }
        break;
    }
    case CARD_COND_OTHER_FLAG1_CARD_OR_OPERATION_97:
        if (targets == 0) {
            break;
        }
        if (targets->zone == CARD_ZONE_DISCARD_A || targets->zone == CARD_ZONE_DISCARD_B) {
            oppSide->disposal[targets->index].battle.layers[0].life = 0;
        } else {
            CardPlayDesertOperation(oppSide, targets->index);
        }
        result = 1;
        break;
    case CARD_COND_OTHER_DISPOSAL_FLAG2_DEF4_98:
        if (targets == 0) {
            break;
        }
        if (targets->zone == CARD_ZONE_DISCARD_A || targets->zone == CARD_ZONE_DISCARD_B) {
            oppSide->disposal[targets->index].battle.layers[0].life = 0;
        } else {
            oppSide->battle[targets->index].battle.layers[0].life = 0;
        }
        result = 1;
        break;
    case CARD_COND_OTHER_DISPOSAL_DEF_FLAG1_99:
        if (targets == 0) {
            break;
        }
        result = 1;
        oppSide->disposal[targets->index].battle.layers[0].life = 0;
        break;
    case CARD_COND_OTHER_TWO_LAYERS_103:
        if (targets == 0) {
            break;
        }
        if (targets->zone == CARD_ZONE_DISCARD_A || targets->zone == CARD_ZONE_DISCARD_B) {
            oppSide->disposal[targets->index].battle.layers[1].life = 0;
        } else {
            oppSide->battle[targets->index].battle.layers[1].life = 0;
        }
        result = 1;
        break;
    case CARD_COND_OWN_BATTLE_NOT_CHKSUB_107:
        if (targets == 0) {
            break;
        }
        CCC26ExecSub(side, work, ownSide, targets);
        result = 1;
        break;
    case CARD_COND_OWN_SLOT_CARD_109: {
        int i;

        if (targets == 0) {
            break;
        }
        for (i = 0; i < count; i++, targets++) {
            if (targets->zone == CARD_ZONE_DISCARD_A || targets->zone == CARD_ZONE_DISCARD_B) {
                ownSide->disposal[targets->index].battle.layers[0].life = 0;
            } else {
                ownSide->battle[targets->index].battle.layers[0].life = 0;
            }
            CardPlayRecavery(&ownSide->hand);
            CardPlayRecavery(&ownSide->hand);
            CardPlayRecavery(&ownSide->hand);
            CardPlayRecavery(&ownSide->hand);
        }
        /* Four cards come back per target: four CardPlayRecavery calls above, and four points reported per target. */
        CardSetEffect(side, work, CARD_EFFECT_TARGET_DOWN, 0, 0, 0, 0);
        CardSetEffect(side, work, CARD_EFFECT_POINTS, (s8) (count * 4), CARD_EFFECT_SOUND, 0, 0);
        result = 1;
        break;
    }
    case CARD_COND_OTHER_DISPOSAL_CARD_111:
        if (targets == 0) {
            break;
        }
        CCC30ExecSub(opponentSide, work, CardEnemyTargetStack(oppSide, targets));
        result = 1;
        break;
    case CARD_COND_OWN_SLOT_CARD_112:
        if (targets == 0) {
            break;
        }
        CCC31ExecSub(opponentSide, work, CardEnemyTargetStack(oppSide, targets));
        result = 1;
        break;
    case CARD_COND_OTHER_DISPOSAL_CARD_119:
        if (targets == 0) {
            break;
        }
        CCC38ExecSub(opponentSide, work, oppSide, targets);
        result = 1;
        break;
    case CARD_COND_TWO_JUNK_FLAG1_106: {
        int handCount;
        int junkCount;
        int taken;
        int i;

        handCount = CardPlayHandCnt(&ownSide->hand);
        junkCount = CardPlayJunkCnt(ownSide);
        taken = 0;
        /* Bit 0 of the card's kind and bit 0 of its flags both have to be set; their meaning is not
         * evidenced beyond these two tests. */
        for (i = 0; i < junkCount && taken < CARD_JUNK_PICKUP_MAX; i++) {
            if (work->definitions[ownSide->junk[i]].kind & 1) {
                if (work->definitions[ownSide->junk[i]].flags & 1) {
                    ownSide->hand.cards[handCount + taken] = ownSide->junk[i];
                    ownSide->junk[i] = -1;
                    taken++;
                }
            }
        }
        CardPlayCleanJunk(ownSide);
        result = 1;
        break;
    }
    case CARD_COND_ALWAYS_MET_95: {
        int pileCount;
        int best;
        int bestId;
        int tries;
        int handCount;
        int pileIndex;
        int id;
        int idData;
        int next;

        pileCount = CardPlayYamaCnt(ownSide);
        tries = 1;
        best = pileCount - 1;
        bestId = ownSide->deck[best];
        if (best >= 0) {
            pileIndex = best;
            do {
                id = ownSide->deck[pileIndex];
                idData = GetDeckData(side, work, id);
                if (GetDeckData(side, work, bestId) < idData) {
                    bestId = id;
                    best = pileIndex;
                }
                tries++;
                if (tries > CARD_DRAW_WINDOW) {
                    break;
                }
                next = pileCount - tries;
                pileIndex = next;
            } while (next >= 0);
        }
        handCount = CardPlayHandCnt(&ownSide->hand);
        ownSide->hand.cards[handCount] = ownSide->deck[best];
        ownSide->deck[best] = -1;
        CardPlayCleanYama(ownSide);
        CardPlayShuffleYama(ownSide);
        CardPlayShuffleYama(ownSide);
        CardPlayShuffleYama(ownSide);
        result = 1;
        break;
    }
    default:
        result = CardPlayCommandPlay((u16) side,
                                     work->commandIds[ownSide->battle[0].cmdView.playedCard],
                                     work, 0, 0);
        break;
    }
    return result;
}

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

/* Board slot positions, 12 bytes per slot (only the 1P table is read, as in
 * CardPlayDisployment, ov10/cgp.c). */
extern float BattlePos1P[][3];

/* Why the card at the hand index cannot be played, or zero (ov10/cgp.c, published C). */
extern int CGPDispoSub0(int side, CardGameWork *work, int index);

/* Same shapes as ov10/ccu_038_exec_sub.c; CardPlayCleanHand returns nothing (void in ov10/cgp.h). */
extern int CardPlayDesertBattle(CardHand *hand, CardLayerStack *card);
extern void CardPlayCleanHand(CardHand *hand);

/* An Lv10 card's id is its base card's id plus 7 (cardId - 7 at 0x00a33d50, 0x00a33f6c, 0x00a33fb0). */
#define CARD_LV10_ID_OFFSET 7

/* Results; the callers (CC_Kara_*, 0x00a37618 / 0x00a37b4c) only test for zero. */
#define LV10_SET_OK 0               /* the `result = 0` initializer at function entry */
#define LV10_SET_NO_BASE_CARD (-1)  /* not an Lv10 card, or its base card is not on the board; `li t1,-1` at 0x00a33d54 */
#define LV10_SET_NO_POINTS 12       /* same code as CGPDispoSub0's "needs more play points"; `li t1,12` at 0x00a33d48 */
#define LV10_SET_BASE_WAITING 9     /* the base card (id - 7) sits in a disposal slot; `li t1,9` at 0x00a33d68 */
#define LV10_SET_ALREADY_WAITING 5  /* the Lv10 card itself sits in a disposal slot; `li t1,5` at 0x00a33d74 */

/*
 * Cards CGPEnemyLv10SetSub accepts: the hand card id must be 7, 9 or 10..13 (`beq v1,t0` with
 * t0 = 7 at 0x00a33ca8, `slti v0,v1,7` / `,14` / `,9` at 0x00a33cac/0x00a33cb8/0x00a33cc4,
 * `li v0,9` at 0x00a33cd0, `slti v0,v1,10` at 0x00a33cd8, `beq v1,t0` at 0x00a33ce4).
 * Each is the Lv10 version of the card id - CARD_LV10_ID_OFFSET.
 */
enum CardLv10Id {
    CARD_LV10_7 = 7,
    CARD_LV10_9 = 9,
    CARD_LV10_10 = 10,
    CARD_LV10_11 = 11,
    CARD_LV10_12 = 12,
    CARD_LV10_13 = 13
};

/*
 * Plays the Lv10 version of a character from the hand over its base card on the board: the base
 * card's stack is cleared, the new card takes its place keeping the damage already taken, slides
 * from the hand to the slot and the play cost is paid. Returns 0 when it did.
 */
int CGPEnemyLv10SetSub(int side, CardGameWork *work, int index) {
    CardPlaySide *ownSide;
    CardCursorPosition position;  /* zone, hand index and card id of the card being played; never read back */
    int result = 0;
    int i;

    position.zone = 5;  /* zone 5 for side 0 (`sh` at 0x00a33c78), 4 for side 1 (0x00a33c84) */
    ownSide = &work->player;
    if (side != 0) {
        position.zone = 4;
        ownSide = &work->enemy;
    }
    position.cardId = ownSide->hand.cards[index];
    position.index = index;

    switch (ownSide->hand.cards[index]) {
    case CARD_LV10_7:
    case CARD_LV10_9:
    case CARD_LV10_10:
    case CARD_LV10_11:
    case CARD_LV10_12:
    case CARD_LV10_13:
        switch (ownSide->hand.cards[index]) {
        case CARD_LV10_10:
        case CARD_LV10_11:
        case CARD_LV10_12:
        case CARD_LV10_13:
            result = CGPDispoSub0(side, work, index);
            if (result == LV10_SET_ALREADY_WAITING) {
                result = 0;
            }
            /* falls through: cards 7 and 9 skip CGPDispoSub0 */
        case CARD_LV10_7:
        case CARD_LV10_9:
            if (result == 0) {
                if (ownSide->hand.playPoints[0]
                    < work->definitions[ownSide->hand.cards[index]].playCost) {
                    result = LV10_SET_NO_POINTS;
                } else {
                    result = LV10_SET_NO_BASE_CARD;
                    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                        /* Declared inside the body so the loop optimiser leaves the loop alone
                         * and the subtraction stays in each iteration (0x00a33f6c). */
                        int baseCardId = ownSide->hand.cards[index] - CARD_LV10_ID_OFFSET;

                        if (baseCardId == ownSide->disposal[i].battle.layers[0].cardId) {
                            result = LV10_SET_BASE_WAITING;
                            break;
                        }
                        if (ownSide->hand.cards[index]
                            == ownSide->disposal[i].battle.layers[0].cardId) {
                            result = LV10_SET_ALREADY_WAITING;
                            break;
                        }
                    }
                    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                        if (ownSide->hand.cards[index] - CARD_LV10_ID_OFFSET
                            == ownSide->battle[i].battle.layers[0].cardId) {
                            CardLayerStack *stack = &ownSide->battle[i].battle;
                            int lifeLost = stack->layers[0].life
                                - work->definitions[stack->layers[0].cardId].operationDuration;

                            CardPlayDesertBattle(&ownSide->hand, &ownSide->battle[i].battle);
                            stack->power = work->definitions[position.cardId].basePower;
                            stack->powerLimit = work->definitions[position.cardId].basePower;
                            stack->layers[0].cardId = position.cardId;
                            stack->layers[0].life
                                = work->definitions[position.cardId].operationDuration + lifeLost;
                            stack->position[0]
                                = ((CardPlaySideMoveView *) ownSide)->handMove[position.index].position[0];
                            stack->position[1]
                                = ((CardPlaySideMoveView *) ownSide)->handMove[position.index].position[1];
                            stack->position[2]
                                = ((CardPlaySideMoveView *) ownSide)->handMove[position.index].position[2];
                            stack->step[0]
                                = (BattlePos1P[i][0]
                                   - ((CardPlaySideMoveView *) ownSide)->handMove[position.index].position[0])
                                  / 10.0f;
                            stack->step[1]
                                = (BattlePos1P[i][1]
                                   - ((CardPlaySideMoveView *) ownSide)->handMove[position.index].position[1])
                                  / 10.0f;
                            stack->step[2]
                                = (BattlePos1P[i][2]
                                   - ((CardPlaySideMoveView *) ownSide)->handMove[position.index].position[2])
                                  / 10.0f;
                            stack->moveFrames = CARD_SLIDE_FRAMES;
                            ownSide->hand.playPoints[0]
                                -= work->definitions[ownSide->hand.cards[position.index]].playCost;
                            ownSide->hand.cards[position.index] = -1;
                            CardPlayCleanHand(&ownSide->hand);
                            result = LV10_SET_OK;
                            break;
                        }
                    }
                }
            }
            break;
        }
        break;
    default:
        result = LV10_SET_NO_BASE_CARD;
        break;
    }
    return result;
}

void CardEnemyComm(int side, CardGameWork *work) {
    if (CC_Kara_CommFase(side, work) == 1) {
        CGPNextTurnSub(work);
    }
}

/*
 * Card id 7 is the AI's search target here: the same draw-to-hand/CleanYama/
 * ShuffleYama sequence, keyed off the same card id, is already accepted for
 * the manual case in ccu_038_exec_sub.c (CCU038ExecSub step 4.3,
 * `ownSide->deck[NowNo_32] == 7`).
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
