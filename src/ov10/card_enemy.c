/*
 * OV10 original TU 9: 0x00a324b8..0x00a34190 (15 functions): the computer
 * player's turn handlers, its command and operation cards, and the ROM deck
 * tables.
 */
#include "common.h"
#include "ov10/cgp.h"
#include "ov10/card_id.h"
#include "ov10/battle_area_check_enemy.h"
#include "ov10/card_enemy.h"
#include "ov10/card_fread.h"
#include "ov10/ccu_038_exec_sub.h"

/* Frames the move and set phases wait after their AI handler finishes. */
#define CARD_ENEMY_WAIT_FRAMES 15

/* aiPhase values of CardEnemyMove. */
#define CARD_ENEMY_MOVE_START 0     /* shunt the positions, then fall into the search */
#define CARD_ENEMY_MOVE_SEARCH 1    /* run CC_Kara_MoveFase until it finishes */
#define CARD_ENEMY_MOVE_WAIT 2      /* count the wait down, then end the turn */

/* aiPhase values of CardEnemySet. */
#define CARD_ENEMY_SET_CHOOSE 0     /* run CC_Kara_SetFase until it finishes */
#define CARD_ENEMY_SET_WAIT 1       /* count the wait down, then clear the flags and end the turn */

/* Cards in hand from which CardEnemyEnd runs CC_Kara_EndFase. */
#define CARD_ENEMY_END_HAND_COUNT 7

/*
 * The cards the AI answers or acts on, by their ids (ov10/card_id.h):
 * Miyuki's Email and Song neutralize the other side's event card (CardEnemyAnswer plays them); Lv 10 Shion is the card
 * CardEnemyAnswerSionSearch draws from the deck (the same id CCU038ExecSub step 4.3 looks for);
 * the situation cards CardEnemyOperationPlay and CGPEnemyExecOperation act on, with what each one does:
 * So Weak! takes life off one battle slot of the opponent, Phenomenon Change discards one situation card of the opponent,
 * Rhine Maiden makes the opponent pay four cost cards and Unknown Armament takes life off every card of both boards
 * (CheckXDamageCommand decides). The situation cards sit in CardPlaySide.operations.
 */

/* Life CARD_ID_SO_WEAK takes off, and the threshold CheckDamageCommand is asked about. */
#define SO_WEAK_DAMAGE 3
/* Damage CheckXDamageCommand is asked about for CARD_ID_UNKNOWN_ARMAMENT. */
#define UNKNOWN_ARMAMENT_DAMAGE 2
/* Cards Rhine Maiden makes the opponent pay (the deck takes four damage). */
#define RHINE_MAIDEN_DAMAGE 4

/*
 * The characters CardEnemyAnswerCardxx answers for the AI, with the effect each one has when its E mark is removed and the
 * handler it runs: VX-9000 destroys a card of the opponent's standby area (CCU038ExecSub, the AI picks from its own
 * disposal row), Margulis a situation card (CCU055ExecSub), Testament downs a battlefield card (CCU058ExecSub), Tony a
 * standby card (CCU061ExecSub) and Allen moves a battlefield card back to the standby area (CCC07ExecSub).
 */

/* Investigation compares at most this many cards from the end of the draw pile;
 * Revive DX takes at most this many cards from the junk pile into the hand. */
#define CARD_DRAW_WINDOW 5
#define CARD_JUNK_PICKUP_MAX 2

/* An Lv10 card's id is its base card's id plus 7. */
#define CARD_LV10_ID_OFFSET 7

/* Results. The callers (CC_Kara_*) only test for zero. */
#define LV10_SET_OK 0               /* the `result = 0` initializer at function entry */
#define LV10_SET_NO_BASE_CARD (-1)  /* not an Lv10 card, or its base card is not on the board */
#define LV10_SET_NO_POINTS 12       /* same code as CGPDispoSub0's "needs more play points" */
#define LV10_SET_BASE_WAITING 9     /* the base card (id - 7) sits in a disposal slot */
#define LV10_SET_ALREADY_WAITING 5  /* the Lv10 card itself sits in a disposal slot */

/* Functions of this TU called before their definition. */
int CGPEnemyExecOperation(int side, CardGameWork *work, int operationIndex,
                          CGPBoardArea *arg);

/* Loads deckdata.dat and copies its CPU deck entries into the enemy deck tables. */
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

/*
 * Runs the computer player's move phase: shunts the battle positions, runs
 * CC_Kara_MoveFase until it finishes, then waits before ending the turn.
 */
void CardEnemyMove(int side, CardGameWork *work) {
    /* Frames left in the wait (signed byte). */
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

/*
 * Runs the computer player's set phase: chooses the set with CC_Kara_SetFase,
 * then waits before ending the turn. Unless exactly one battle is in play, the
 * ALBEDO cards on the board lose their flag first.
 */
void CardEnemySet(int side, CardGameWork *work) {
    /* Frames left in the wait (signed byte). */
    static s8 Timer;
    CardPlaySide *ownSide = side == 0 ? &work->player : &work->enemy;
    int i;
    /*
     * The value the battle count is compared with. The original tests it against
     * a register that already holds 1; the literal 1 in the comparison would
     * change the entry block and the branch, so a local keeps it.
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
                    if (ownSide->battle[i].battle.layers[0].cardId == CARD_ID_ALBEDO) {
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

s32 CardEnemyCommandRequest(int side, CardGameWork *work) {
    CardPlayHandCnt(side == 0
        ? &work->player.hand
        : &work->enemy.hand);
    return CC_Kara_CommRequest(side, work);
}

/* Plays the playable operation cards in the side's operation slots, each with its own effect. Returns 1. */
int CardEnemyOperationPlay(int side, CardGameWork *work) {
    CardPlaySide *ownSide;
    CardPlaySide *oppSide;
    CGPBoardArea arg;
    int targetArea;
    int damageSlot;
    int i;
    int j;

    if (side == 0) {
        ownSide = &work->player;
        oppSide = &work->enemy;
        targetArea = CARD_ZONE_SIDE1_BATTLE;
    } else {
        ownSide = &work->enemy;
        oppSide = &work->player;
        targetArea = CARD_ZONE_SIDE0_BATTLE;
    }
    for (i = 0; i < CARD_OPERATION_SLOTS; i++) {
        if (ownSide->operations[i] < 0) {
            continue;
        }
        if (CGPChkOpePlayable(side, work, ownSide->operations[i]) == 0) {
            continue;
        }
        switch (ownSide->operations[i]) {
        case CARD_ID_SO_WEAK:
            damageSlot = CheckDamageCommand(side, work, SO_WEAK_DAMAGE);
            if (damageSlot == -1) {
                continue;
            }
            arg.slot = damageSlot;
            arg.area = targetArea;
            CGPEnemyExecOperation(side, work, i, &arg);
            break;
        case CARD_ID_PHENOMENON_CHANGE:
            for (j = 0; j < CARD_OPERATION_SLOTS; j++) {
                if (oppSide->operations[j] < 0) {
                    continue;
                }
                if (side == 0) {
                    arg.area = CARD_ZONE_SIDE1_OPERATION;
                } else {
                    arg.area = CARD_ZONE_SIDE0_OPERATION;
                }
                arg.slot = j;
                CGPEnemyExecOperation(side, work, i, &arg);
                break;
            }
            break;
        case CARD_ID_RHINE_MAIDEN:
            CGPEnemyExecOperation(side, work, i, 0);
            break;
        case CARD_ID_UNKNOWN_ARMAMENT:
            if (CheckXDamageCommand(side, work, UNKNOWN_ARMAMENT_DAMAGE) == 1) {
                CGPEnemyExecOperation(side, work, i, 0);
            }
            break;
        }
    }
    return 1;
}

s32 CardEnemyCommandPlay(int side, CardGameWork *work) {
    return CC_Kara_CommPlay(side, work);
}

s32 CardEnemy1stAnswer(int side, CardGameWork *work) {
    CardPlayHandCnt(side == 0
        ? &work->player.hand
        : &work->enemy.hand);
    return CC_Kara_1stAnswer(side, work);
}

/*
 * Answers with Miyuki's Email or Song from the hand when the side's draw pile
 * has at least two cards and the card's condition allows it. Returns 1 when a
 * card was answered, 0 otherwise.
 */
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
        if (hand->cards[i] == CARD_ID_MIYUKIS_EMAIL || hand->cards[i] == CARD_ID_SONG) {
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

/*
 * Runs one step of the AI's answer card cardId. On step 0 the card finds its
 * target and sets its effect message; on step 1 it applies the effect. Returns
 * 1 when the answer is finished, 0 while it is still running.
 */
int CardEnemyAnswerCardxx(int side, CardGameWork *work, s16 cardId) {
    CardPlaySide *ownSide;
    CardPlaySide *oppSide;
    CardCursorPosition position;
    CardCursorPosition *target;
    int ownDisposalZone;
    int oppOperationZone;
    int oppDisposalZone;
    int oppBattleZone;
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

    oppDisposalZone = CARD_ZONE_SIDE1_DISPOSAL;
    target = 0;
    result = 0;
    oppBattleZone = CARD_ZONE_SIDE1_BATTLE;
    ownSide = &work->player;
    oppSide = &work->enemy;
    oppOperationZone = CARD_ZONE_SIDE1_OPERATION;
    ownDisposalZone = CARD_ZONE_SIDE0_DISPOSAL;
    if (side != 0) {
        ownSide = &work->enemy;
        oppSide = &work->player;
        oppOperationZone = CARD_ZONE_SIDE0_OPERATION;
        oppDisposalZone = CARD_ZONE_SIDE0_DISPOSAL;
        ownDisposalZone = CARD_ZONE_SIDE1_DISPOSAL;
        oppBattleZone = CARD_ZONE_SIDE0_BATTLE;
    }
    /* One case per answer character; each case's step 0 looks for a target and step 1 acts on it. */
    switch (cardId) {
    case CARD_ID_VX_9000:
        switch (work->command.step) {
        case 0:
            found = 0;
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
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
            CGPSetErrorMessPlus(-1, work, CGP_ERR_USE_CARD_EFFECT, CARD_ID_VX_9000);
            break;
        case 1:
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (ownSide->disposal[i].battle.layers[0].cardId >= 0) {
                    position.zone = ownDisposalZone;
                    position.index = i;
                    target = &position;
                    break;
                }
            }
            CCU038ExecSub(ownSide, target);
            result = 1;
            break;
        }
        break;
    case CARD_ID_MARGULIS:
        switch (work->command.step) {
        case 0:
            foundOperation = 0;
            for (i = 0; i < CARD_OPERATION_SLOTS; i++) {
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
            CGPSetErrorMessPlus(-1, work, CGP_ERR_USE_CARD_EFFECT, CARD_ID_MARGULIS);
            break;
        case 1:
            for (i = 0; i < CARD_OPERATION_SLOTS; i++) {
                if (oppSide->operations[i] >= 0) {
                    position.zone = oppOperationZone;
                    position.index = i;
                    target = &position;
                    break;
                }
            }
            CCU055ExecSub(oppSide, target);
            result = 1;
            break;
        }
        break;
    case CARD_ID_TESTAMENT:
        switch (work->command.step) {
        case 0:
            foundBattle = 0;
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (oppSide->battle[i].battle.layers[0].cardId >= 0
                    && (oppSide->battle[i].battle.flags & CARD_STACK_FLAG_FROZEN) == 0) {
                    foundBattle = 1;
                    break;
                }
            }
            if (foundBattle == 0) {
                result = 1;
                break;
            }
            work->command.step++;
            CGPSetErrorMessPlus(-1, work, CGP_ERR_USE_CARD_EFFECT, CARD_ID_TESTAMENT);
            break;
        case 1:
            result = 0;
            best = 0;
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (oppSide->battle[i].battle.layers[0].cardId >= 0
                    && best < GetPrice(&oppSide->battle[i].battle)
                    && (oppSide->battle[i].battle.flags & CARD_STACK_FLAG_FROZEN) == 0) {
                    result = i;
                    best = GetPrice(&oppSide->battle[i].battle);
                }
            }
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (oppSide->battle[i].battle.layers[0].cardId >= 0) {
                    position.zone = oppDisposalZone;
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
    case CARD_ID_TONY:
        switch (work->command.step) {
        case 0:
            foundDisposal = 0;
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (oppSide->disposal[i].battle.layers[0].cardId >= 0
                    && (oppSide->disposal[i].battle.flags & CARD_STACK_FLAG_FROZEN) == 0) {
                    foundDisposal = 1;
                    break;
                }
            }
            if (foundDisposal == 0) {
                result = 1;
                break;
            }
            work->command.step++;
            CGPSetErrorMessPlus(-1, work, CGP_ERR_USE_CARD_EFFECT, CARD_ID_TONY);
            break;
        case 1:
            result = 0;
            best = 0;
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (oppSide->disposal[i].battle.layers[0].cardId >= 0
                    && best < GetPrice(&oppSide->disposal[i].battle)
                    && (oppSide->disposal[i].battle.flags & CARD_STACK_FLAG_FROZEN) == 0) {
                    result = i;
                    best = GetPrice(&oppSide->disposal[i].battle);
                }
            }
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (oppSide->disposal[i].battle.layers[0].cardId >= 0) {
                    position.zone = oppDisposalZone;
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
    case CARD_ID_ALLEN:
        switch (work->command.step) {
        case 0:
            foundRetreat = 0;
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (oppSide->disposal[i].battle.layers[0].cardId >= 0) {
                    foundRetreat = 1;
                    break;
                }
            }
            answered = 0;
            if (foundRetreat != 0) {
                for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                    if (oppSide->battle[i].battle.layers[0].cardId < 0) {
                        continue;
                    }
                    if (CheckTaikyakuCommand(side, work) == -1) {
                        result = 1;
                        break;
                    }
                    work->command.step++;
                    CGPSetErrorMessPlus(-1, work, CGP_ERR_USE_CARD_EFFECT, CARD_ID_ALLEN);
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
                for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                    if (oppSide->battle[i].battle.layers[0].cardId >= 0) {
                        position.index = i;
                        position.zone = oppBattleZone;
                        target = &position;
                        break;
                    }
                }
            } else {
                position.index = retreatSlot;
                position.zone = oppBattleZone;
                target = &position;
            }
            CCC07ExecSub(work, oppSide, target);
            result = 1;
            break;
        }
        break;
    }
    return result;
}

/* The battle or disposal stack a cursor names; zones 6 and 2 are the disposal zones.
 * Kept after CardEnemyAnswerCardxx: an earlier place renumbers the local labels. */
static inline CardLayerStack *CardEnemyTargetStack(CardPlaySide *side, CardCursorPosition *target) {
    if (target->zone == CARD_ZONE_SIDE0_DISPOSAL || target->zone == CARD_ZONE_SIDE1_DISPOSAL) {
        return &side->disposal[target->index].battle;
    }
    return &side->battle[target->index].battle;
}

/*
 * Applies the command card the side played to the given targets. Command card
 * ids 83 (Cure-All) to 119 (Break Through) are dispatched by a switch; ids
 * without a case of their own go to the default case, which plays the card
 * through CardPlayCommandPlay. Recycler and Destiny end the turn with no effect
 * applied here.
 */
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
     * The command card id is the playedCard member of the CardBoardSlot union in
     * battle[0] (at +0x198 of the side); the other slots never use it.
     */
    switch (ownSide->battle[0].cmdView.playedCard) {
    case CARD_ID_CURE_ALL:
    case CARD_ID_START_UP:
        if (targets == 0) {
            break;
        }
        CCC02ExecSub(&CardEnemyTargetStack(ownSide, targets)->flags);
        result = 1;
        break;
    case CARD_ID_CURRY:
        if (targets == 0) {
            break;
        }
        CCC03ExecSub(side, work, &ownSide->hand, CardEnemyTargetStack(ownSide, targets));
        result = 1;
        break;
    case CARD_ID_RECYCLER:
    case CARD_ID_DESTINY:
        result = 1;
        break;
    case CARD_ID_SUPPLY_FLEET:
        CCC06ExecSub(side, work, ownSide);
        result = 1;
        break;
    case CARD_ID_RETREAT:
    case CARD_ID_WOGLINDE:
        if (targets == 0) {
            break;
        }
        CCC07ExecSub(work, oppSide, targets);
        result = 1;
        break;
    case CARD_ID_SURPRISE_ATTACK:
    case CARD_ID_FEDERATION_CRUISER:
        if (targets == 0) {
            break;
        }
        CCC10ExecSub(opponentSide, work, oppSide, targets);
        result = 1;
        break;
    case CARD_ID_MARY_AND_SHELLEY:
        CardPlayDrawCard(ownSide);
        CardPlayDrawCard(ownSide);
        result = 1;
        break;
    case CARD_ID_LANDING_OPERATIONS: {
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
    case CARD_ID_PREMONITION:
        if (targets == 0) {
            break;
        }
        if (targets->zone == CARD_ZONE_SIDE0_DISPOSAL || targets->zone == CARD_ZONE_SIDE1_DISPOSAL) {
            oppSide->disposal[targets->index].battle.layers[0].life = 0;
        } else {
            CardPlayDesertOperation(oppSide, targets->index);
        }
        result = 1;
        break;
    case CARD_ID_DRAGON_SKULL:
        if (targets == 0) {
            break;
        }
        if (targets->zone == CARD_ZONE_SIDE0_DISPOSAL || targets->zone == CARD_ZONE_SIDE1_DISPOSAL) {
            oppSide->disposal[targets->index].battle.layers[0].life = 0;
        } else {
            oppSide->battle[targets->index].battle.layers[0].life = 0;
        }
        result = 1;
        break;
    case CARD_ID_EXPULSION_ORDER:
        if (targets == 0) {
            break;
        }
        result = 1;
        oppSide->disposal[targets->index].battle.layers[0].life = 0;
        break;
    case CARD_ID_NOTICE:
        if (targets == 0) {
            break;
        }
        if (targets->zone == CARD_ZONE_SIDE0_DISPOSAL || targets->zone == CARD_ZONE_SIDE1_DISPOSAL) {
            oppSide->disposal[targets->index].battle.layers[1].life = 0;
        } else {
            oppSide->battle[targets->index].battle.layers[1].life = 0;
        }
        result = 1;
        break;
    case CARD_ID_STIM_DX:
        if (targets == 0) {
            break;
        }
        CCC26ExecSub(side, work, ownSide, targets);
        result = 1;
        break;
    case CARD_ID_WAR_CASUALTY: {
        int i;

        if (targets == 0) {
            break;
        }
        for (i = 0; i < count; i++, targets++) {
            if (targets->zone == CARD_ZONE_SIDE0_DISPOSAL || targets->zone == CARD_ZONE_SIDE1_DISPOSAL) {
                ownSide->disposal[targets->index].battle.layers[0].life = 0;
            } else {
                ownSide->battle[targets->index].battle.layers[0].life = 0;
            }
            CardPlayRecavery(ownSide);
            CardPlayRecavery(ownSide);
            CardPlayRecavery(ownSide);
            CardPlayRecavery(ownSide);
        }
        /* Four cards come back per target: four CardPlayRecavery calls above, and four points reported per target. */
        CardSetEffect(side, work, CARD_EFFECT_SWIRL, 0, 0, 0, 0);
        CardSetEffect(side, work, CARD_EFFECT_NUMBER, count * CARD_WAR_CASUALTY_RECOVERY, CARD_SE_REGEN, 0, 0);
        result = 1;
        break;
    }
    case CARD_ID_CHARGE:
        if (targets == 0) {
            break;
        }
        CCC30ExecSub(opponentSide, work, CardEnemyTargetStack(oppSide, targets));
        result = 1;
        break;
    case CARD_ID_PRECISE_SHOOTING:
        if (targets == 0) {
            break;
        }
        CCC31ExecSub(opponentSide, work, CardEnemyTargetStack(oppSide, targets));
        result = 1;
        break;
    case CARD_ID_BREAK_THROUGH:
        if (targets == 0) {
            break;
        }
        CCC38ExecSub(opponentSide, work, oppSide, targets);
        result = 1;
        break;
    case CARD_ID_REVIVE_DX: {
        int handCount;
        int junkCount;
        int taken;
        int i;

        handCount = CardPlayHandCnt(&ownSide->hand);
        junkCount = CardPlayJunkCnt(ownSide);
        taken = 0;
        /* Revive DX: battle cards (kind bit CARD_KIND_BATTLE) with the Human attribute only. */
        for (i = 0; i < junkCount && taken < CARD_JUNK_PICKUP_MAX; i++) {
            if (work->definitions[ownSide->junk[i]].kind & CARD_KIND_BATTLE) {
                if (work->definitions[ownSide->junk[i]].flags & CARD_ATTR_HUMAN) {
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
    case CARD_ID_INVESTIGATION: {
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
        result = CardPlayCommandPlay(side,
                                     work->commandIds[ownSide->battle[0].cmdView.playedCard],
                                     work, 0, 0);
        break;
    }
    return result;
}

/*
 * Applies the operation card at operationIndex of the side; arg, when given,
 * names its target. A card that took effect is discarded. Returns -1 when the
 * card took effect, 0 otherwise.
 */
int CGPEnemyExecOperation(int side, CardGameWork *work, int operationIndex,
                          CGPBoardArea *arg) {
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
    CGPSetErrorMessPlus(-1, work, CGP_ERR_USE_CARD_EFFECT,ownSide->operations[operationIndex]);
    switch (ownSide->operations[operationIndex]) {
    case CARD_ID_SO_WEAK:
        if (arg == 0) {
            break;
        }
        oppSide->battle[arg->slot].battle.layers[0].life -= SO_WEAK_DAMAGE;
        if (oppSide->battle[arg->slot].battle.layers[0].life < 0) {
            oppSide->battle[arg->slot].battle.layers[0].life = 0;
        }
        switch (arg->area) {
        case CARD_ZONE_SIDE0_BATTLE:
            effectSide = 0;
            break;
        case CARD_ZONE_SIDE1_BATTLE:
        default:
            effectSide = 1;
            break;
        }
        result = -1;
        CardSetEffect(effectSide, work, CARD_EFFECT_DECK_BURST, 0, CARD_SE_HIT,
                      oppSide->battle[arg->slot].battle.position,
                      &oppSide->battle[arg->slot].battle);
        CardSetEffect(effectSide, work, CARD_EFFECT_DAMAGE_NUMBER, SO_WEAK_DAMAGE, 0,
                      oppSide->battle[arg->slot].battle.position,
                      &oppSide->battle[arg->slot].battle);
        break;
    case CARD_ID_PHENOMENON_CHANGE:
        CardPlayDesertOperation(oppSide, arg->slot);
        result = -1;
        break;
    case CARD_ID_RHINE_MAIDEN:
        for (i = 3; i >= 0; i--) {
            CardPlayCostCard(oppSide);
        }
        CardSetEffect(side, work, CARD_EFFECT_DECK_BURST, 0, CARD_SE_HIT, 0, 0);
        CardSetEffect(side, work, CARD_EFFECT_DAMAGE_NUMBER, RHINE_MAIDEN_DAMAGE, 0, 0, 0);
        result = -1;
        break;
    case CARD_ID_UNKNOWN_ARMAMENT:
        CCO143ExecSub(side, work, ownSide, oppSide);
        result = -1;
        break;
    }
    if (result < 0) {
        CardPlayDesertOperation(ownSide, operationIndex);
    }
    return result;
}

/*
 * Cards CGPEnemyLv10SetSub accepts: the Lv 10 characters, each the Lv 1
 * character's id + CARD_LV10_ID_OFFSET.
 *
 * Plays the Lv10 version of a character from the hand over its base card on the board: the base
 * card's stack is cleared, the new card takes its place keeping the damage already taken, slides
 * from the hand to the slot and the play cost is paid. Returns 0 when it did.
 */
int CGPEnemyLv10SetSub(int side, CardGameWork *work, int index) {
    CardPlaySide *ownSide;
    CardCursorPosition position;  /* zone, hand index and card id of the card being played */
    int result = 0;
    int i;

    position.zone = CARD_ZONE_SIDE0_BATTLE;
    ownSide = &work->player;
    if (side != 0) {
        position.zone = CARD_ZONE_SIDE1_BATTLE;
        ownSide = &work->enemy;
    }
    position.cardId = ownSide->hand.cards[index];
    position.index = index;

    switch (ownSide->hand.cards[index]) {
    case CARD_ID_LV_10_SHION:
    case CARD_ID_LV_10_ZIGGY:
    case CARD_ID_LV_10_MOMO:
    case CARD_ID_LV_10_KOS_MOS:
    case CARD_ID_LV_10_JR:
    case CARD_ID_LV_10_CHAOS:
        switch (ownSide->hand.cards[index]) {
        case CARD_ID_LV_10_MOMO:
        case CARD_ID_LV_10_KOS_MOS:
        case CARD_ID_LV_10_JR:
        case CARD_ID_LV_10_CHAOS:
            result = CGPDispoSub0(side, work, index);
            if (result == LV10_SET_ALREADY_WAITING) {
                result = 0;
            }
            /* falls through: Lv 10 Shion and Lv 10 Ziggy skip CGPDispoSub0 */
        case CARD_ID_LV_10_SHION:
        case CARD_ID_LV_10_ZIGGY:
            if (result == 0) {
                if (ownSide->hand.playPoints[0]
                    < work->definitions[ownSide->hand.cards[index]].playCost) {
                    result = LV10_SET_NO_POINTS;
                } else {
                    result = LV10_SET_NO_BASE_CARD;
                    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                        /* Declared inside the body so the loop optimiser leaves the loop alone
                         * and the subtraction stays in each iteration. */
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

                            CardPlayDesertBattle(ownSide, &ownSide->battle[i].battle);
                            stack->power = work->definitions[position.cardId].basePower;
                            stack->powerLimit = work->definitions[position.cardId].basePower;
                            stack->layers[0].cardId = position.cardId;
                            stack->layers[0].life
                                = work->definitions[position.cardId].operationDuration + lifeLost;
                            stack->position[0]
                                = ownSide->handMove[position.index].position[0];
                            stack->position[1]
                                = ownSide->handMove[position.index].position[1];
                            stack->position[2]
                                = ownSide->handMove[position.index].position[2];
                            stack->step[0]
                                = (BattlePos1P[i][0]
                                   - ownSide->handMove[position.index].position[0])
                                  / 10.0f;
                            stack->step[1]
                                = (BattlePos1P[i][1]
                                   - ownSide->handMove[position.index].position[1])
                                  / 10.0f;
                            stack->step[2]
                                = (BattlePos1P[i][2]
                                   - ownSide->handMove[position.index].position[2])
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
 * Lv 10 Shion is the AI's search target here. The same draw-to-hand, CleanYama
 * and ShuffleYama sequence, keyed off the same card id, is used by the manual
 * case in ccu_038_exec_sub.c (CCU038ExecSub step 4.3, `ownSide->deck[NowNo_32] == 7`).
 */
void CardEnemyAnswerSionSearch(int side, CardGameWork *work) {
    CardPlaySide *ownSide = side == 0 ? &work->player : &work->enemy;
    int count;
    int i;

    count = CardPlayYamaCnt(ownSide);
    for (i = 0; i < count; i++) {
        if (ownSide->deck[i] == CARD_ID_LV_10_SHION) {
            ownSide->hand.cards[CardPlayHandCnt(&ownSide->hand)] = ownSide->deck[i];
            ownSide->deck[i] = -1;
            CardPlayCleanYama(ownSide);
            CardPlayShuffleYama(ownSide);
        }
    }
}

/*
 * Runs CC_Kara_EndFase once the hand holds CARD_ENEMY_END_HAND_COUNT cards, and
 * ends the turn when it returns 0.
 */
void CardEnemyEnd(int side, CardGameWork *work) {
    int endFaseResult = 1;

    if (CardPlayHandCnt(side == 0
            ? &work->player.hand
            : &work->enemy.hand) >= CARD_ENEMY_END_HAND_COUNT) {
        endFaseResult = CC_Kara_EndFase(side, work);
    }
    if (endFaseResult == 0) {
        CGPNextTurnSub(work);
    }
}
