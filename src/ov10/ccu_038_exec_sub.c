/*
 * OV10 original TU 3: 0x00a12a90..0x00a19a20 (29 functions)
 */
#include "common.h"
#include "ov10/cgp.h"
#include "ov10/battle_area_check_enemy.h"

/* Sets the card game work area's error message; defined in ov10/tu008
 * (src/ov10/cgp.c), still assembler-scaffolded here. */
extern void CGPSetErrorMessPlus(int code, CardGameWork *work, u8 reason, s16 value);

#include "ccu_038_exec_sub.h"

enum CardQueueWord {
    CARD_QUEUE_PHASE,
    CARD_QUEUE_COMMAND,
    CARD_QUEUE_OPERAND,
    CARD_QUEUE_TARGET
};

static inline CardLayerStack *CardSelectBattle(CardPlaySide *side, int index,
                                               int fromDisposal) {
    CardLayerStack *battling = &side->battle[index].battle;
    CardLayerStack *disposed = &side->disposal[index].battle;
    if (fromDisposal)
        battling = disposed;
    return battling;
}

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

extern int CardPlayDrawCard(CardHand *hand);
extern int CardPlayDrawSute(CardHand *hand);
extern int CardPlayRecavery(CardHand *hand);
extern int CardPlayCostCard(CardHand *hand);
extern int CardPlayDesertBattle(CardHand *hand, CardLayerStack *card);
extern int CardPlayDesertOperation(CardHand *hand, int index);
extern int CardPlayBattleCnt(CardHand *hand);
extern int CardPlayDisposeCnt(CardHand *hand);
extern int CardPlayHandCnt(CardHand *hand);
extern int CardPlayJunkCnt(CardHand *hand);
extern int CardPlayYamaCnt(CardHand *hand);
extern int CardPlayOperationCnt(CardHand *hand);
extern int CardEnemyAnswerCardxx(int side, CardGameWork *work, s16 cardId);
extern void CGPDispErrorMess(int side, u16 message);
extern void CCU038ExecSub(CardHand *hand, CardCursorPosition *position);
extern void CCU055ExecSub(CardHand *hand, CardCursorPosition *position);
extern void CardPlayCleanYama(CardHand *hand);
extern void CardPlayCleanJunk(CardHand *hand);
extern void CardPlayShuffleYama(CardHand *hand);
extern int CardPlayReturnCard(CardHand *hand, CardLayerStack *battle, s16 index);
extern void CPCPSwapDSBATTLE(void *destination, void *source);
extern void CGCPJunkDispSub(CardHand *hand, int index);
extern void CGCPYamaDispSub(CardHand *hand, int index, int count, int buttons, int rotated);
extern void CGCPHandDispSub2(CardDefinition *definitions, CardHand *hand, int selected, int mode);
extern void CGCPHandDispSub3(CardDefinition *definitions, CardHand *hand, int selected, int mode, int option);
extern void CardEnemyAnswerSionSearch(int side, CardGameWork *work);
extern void CGPDispErrorMessPlus(int side, int message, s16 cardId);
extern int CardPlayDesertCard(CardHand *hand, int index);
extern int CardPlayIdentityCheck(CardGameWork *work, CardHand *hand, u8 identity);
void xglMatrixTrans(Matrix4 destination, const Matrix4 source,
                    const float translation[4]);
void xglMatrixScale(Matrix4 destination, const Matrix4 source,
                    const float scale[4]);
extern void nmlModelSetPlace(float matrix[4][4]);
extern void nmlModelSetTexture(void *texture);
extern void nmlModelEntryCard(void *model);
extern void CCC04ExecSub(int side, CardGameWork *work, CardHand *hand);
extern void CCC03ExecSub(int side, CardGameWork *work, CardHand *hand, CardLayerStack *card);
extern void CCC07ExecSub(CardGameWork *work, CardHand *hand, CardCursorPosition *position);
extern void CCC10ExecSub(int side, CardGameWork *work, CardHand *hand, CardCursorPosition *position);
extern void CCC30ExecSub(int side, CardGameWork *work, CardLayerStack *card);
extern void CCC31ExecSub(int side, CardGameWork *work, CardLayerStack *card);
extern int CardChkAttackType(CardDefinition *definitions, CardLayerStack *card);
extern int CGPCalcPowerPlus(CardGameWork *work, CardLayerStack *card, u8 power);
extern void xglSoundEffectNormalID(int sound, int channel);
extern void CGPSetPermanentMess(CardGameWork *work, int message);
extern void CardCursorPassive(CardCursorControl *cursor);
extern void CardCursorActive(CardCursorControl *cursor, CardCursorPosition *position, int mode);
extern int CGPCursorMove(int side, CardGameWork *work);
extern void CGPCursor2Pos(CardCursorPosition *position, CardGameWork *work);
extern void CardSetEffect(int side, CardGameWork *work, u8 kind, s8 amount,
                          int sound, void *position, void *target);
const char D_00A4D7C8[40] = "\x0b\x19\x03\x0d\0Select card to discard.";
static int D_00A57760;
static int D_00A57764;
static int D_00A57788;
static int D_00A5778C;
static int D_00A57790;
static int D_00A57794;
static int D_00A577C0;
static int D_00A577C4;
static int D_00A577C8;
static int D_00A57830;
static int D_00A577E0;
static int D_00A577F0;
static int D_00A57800;
static s8 D_00A57834;
static CardCursorPosition D_00A57750;
static CardCursorPosition D_00A57758;
static CardCursorPosition D_00A57768;
static CardCursorPosition D_00A57770;
static CardCursorPosition D_00A57778;
static CardCursorPosition D_00A57780;
static CardCursorPosition D_00A57798;
static CardCursorPosition D_00A577A0;
static CardCursorPosition D_00A577A8;
static CardCursorPosition D_00A577B0;
static CardCursorPosition D_00A577B8;
static CardCursorPosition D_00A577D0;
static CardCursorPosition D_00A577D8;
static CardCursorPosition D_00A577E8;
static CardCursorPosition D_00A577F8;
static CardCursorPosition D_00A57808;
static CardCursorPosition D_00A57810;
static CardCursorPosition D_00A57818;
static CardCursorPosition D_00A57820;
static CardCursorPosition D_00A57828;
extern s16 D_00A5779A;
extern s16 D_00A577B2;
extern s16 D_00A5780A;
extern s16 D_00A5781A;
/* This TU saw an int first parameter: CardPlayCommandPlay passes its u16 side
 * number without the s8 sign extension a narrower prototype would add. */

/*
 * Zeroes a disposal slot's base-layer life when the slot holds a card. The
 * caller passes the side's hand record, whose address is also the address
 * of the enclosing CardPlaySide (CardHand is its first member).
 */
void CCU038ExecSub(CardHand *hand, CardCursorPosition *position)
{
    if (position != 0) {
        if (((CardPlaySide *) hand)->disposal[position->index].battle.layers[0].cardId >= 0)
            ((CardPlaySide *) hand)->disposal[position->index].battle.layers[0].life = 0;
    }
}

void CCU055ExecSub(CardHand *hand, CardCursorPosition *position) {
    if (position != 0)
        CardPlayDesertOperation(hand, position->index);
}

/* Sets flags bit 0x02 on a battle slot when the slot holds a card. */
void CCU058ExecSub(int side, CardPlaySide *owner, CardCursorPosition *position)
{
    if (position != 0) {
        if (owner->battle[position->index].battle.layers[0].cardId >= 0)
            owner->battle[position->index].battle.flags |= 2;
    }
}

/* Sets flags bit 0x02 on a disposal slot when the slot holds a card. */
void CCU061ExecSub(CardPlaySide *side, CardCursorPosition *position)
{
    if (position != 0) {
        if (side->disposal[position->index].battle.layers[0].cardId >= 0)
            side->disposal[position->index].battle.flags |= 2;
    }
}

void CCU062ExecSub(CardHand *hand, CardCursorPosition *position)
{
    if (position != 0) {
        CardPlaySide *owner = (CardPlaySide *) hand;
        int cardCount = CardPlayHandCnt(hand);
        int layerIndex;

        for (layerIndex = 0; layerIndex < 40; layerIndex++) {
            if (owner->disposal[position->index].battle.layers[layerIndex].cardId >= 0) {
                hand->cards[cardCount] =
                    owner->disposal[position->index].battle.layers[layerIndex].cardId;
                cardCount++;
                owner->disposal[position->index].battle.layers[layerIndex].cardId = -1;
            }
        }
    }
}

void CCU063ExecSub(CardGameWork *work, int context) {
    CardPlaySide *owner = (CardPlaySide *) context;
    int i;

    for (i = 0; i < 4; i++) {
        CardBoardSlot *slot = &owner->battle[i];
        if (slot->battle.layers[0].cardId >= 0 &&
            (CardDefinitionsFor(work)[slot->battle.layers[0].cardId].flags & 1)) {
            slot->battle.layers[0].state++;
        }
    }
}

/* CPU-driven card command handlers; the plain *ExecSub siblings they tail-call
 * are still assembler in this TU. */
extern void CCU063ExecSub(CardGameWork *work, s32 context);
extern void CCU064ExecSub(CardGameWork *work, s32 context);
extern void CCU065ExecSub(CardGameWork *work, s32 context);

void CCU063ExecCPUSub(CardGameWork *work, s32 context) {
    CGPSetErrorMessPlus(-1, work, 0x17, 0x3F);
    CCU063ExecSub(work, context);
}

void CCU064ExecSub(CardGameWork *work, s32 context) {
    CardPlaySide *owner = (CardPlaySide *) context;
    int i;

    for (i = 0; i < 4; i++) {
        CardBoardSlot *slot = &owner->battle[i];
        if (slot->battle.layers[0].cardId >= 0 &&
            (CardDefinitionsFor(work)[slot->battle.layers[0].cardId].flags & 8)) {
            slot->battle.layers[0].state++;
        }
    }
}

void CCU064ExecCPUSub(CardGameWork *work, s32 context) {
    CGPSetErrorMessPlus(-1, work, 0x17, 0x40);
    CCU064ExecSub(work, context);
}

void CCU065ExecSub(CardGameWork *work, s32 context) {
    CardPlaySide *owner = (CardPlaySide *) context;
    int i;

    for (i = 0; i < 4; i++) {
        CardBoardSlot *slot = &owner->battle[i];
        if (slot->battle.layers[0].cardId >= 0 &&
            (CardDefinitionsFor(work)[slot->battle.layers[0].cardId].flags & 4)) {
            slot->battle.layers[0].state += 2;
        }
    }
}

void CCU065ExecCPUSub(CardGameWork *work, s32 context) {
    CGPSetErrorMessPlus(-1, work, 0x17, 0x41);
    CCU065ExecSub(work, context);
}

/* Tests the command card of a board slot (its base layer) and the first card
 * stacked on it. */
s32 CCC107ChkSub(CardLayerStack *stack) {
    s32 matched = 0;
    u16 command = stack->layers[0].cardId;

    if ((u32) (command - 0x22) < 2U || (s16) command == 0x25 ||
        (s16) command == 0x29 || (s16) command == 0x46 ||
        (s16) command == 0x47) {
        matched = 1;
    }
    if (matched == 0 && stack->layers[1].cardId >= 0 &&
        (stack->layers[1].cardId == 0x2B || stack->layers[1].cardId == 0x2D)) {
        matched = 1;
    }
    return matched;
}

void CCC02ExecSub(u8 *commandState) {
    *commandState &= ~0x02;
}

void CCC03ExecSub(int side, CardGameWork *work, CardHand *hand, CardLayerStack *card)
{
    float *position = card->position;
    int oldLife = card->layers[0].life;
    int amount = CardDefinitionsFor(work)[card->layers[0].cardId].operationDuration;

    card->layers[0].life = amount;
    amount -= oldLife;
    CardSetEffect(side, work, 0xA, 0, 0, position, 0);
    CardSetEffect(side, work, 0xC, amount, 0x2000D, position, card);
}

void CCC04ExecSub(int side, CardGameWork *work, CardHand *hand)
{
    CardPlaySide *owner = (CardPlaySide *) hand;
    int i;

    for (i = 0; i < 4; i++) {
        int cardId;
        int life;

        cardId = owner->battle[i].battle.layers[0].cardId;
        if (cardId < 0)
            continue;
        if (CardDefinitionsFor(work)[cardId].flags & 4)
            continue;

        life = (u8) owner->battle[i].battle.layers[0].life + 3;
        owner->battle[i].battle.layers[0].life = life;
        if (CardDefinitionsFor(work)[cardId].operationDuration < (s8) life) {
            owner->battle[i].battle.layers[0].life =
                CardDefinitionsFor(work)[cardId].operationDuration;
        }
        CardSetEffect(side, work, 10, 0, 0, owner->battle[i].battle.position, 0);
        CardSetEffect(side, work, 12, 3, 0x2000D, owner->battle[i].battle.position,
                      &owner->battle[i].battle);
    }
}

void CCC06ExecSub(int side, CardGameWork *work, CardHand *hand)
{
    CardPlaySide *owner = (CardPlaySide *) hand;
    int cardCount = 0;
    int i;

    for (i = 0; i < 4; i++) {
        if (owner->disposal[i].battle.layers[0].cardId >= 0)
            cardCount++;
    }
    for (i = 0; i < 4; i++) {
        if (owner->battle[i].battle.layers[0].cardId >= 0)
            cardCount++;
    }
    if (cardCount > 0) {
        i = cardCount;
        do {
            CardPlayRecavery(hand);
        } while (--i != 0);
    }
    if (cardCount == 0)
        return;

    CardSetEffect(side, work, 10, 0, 0, 0, 0);
    CardSetEffect(side, work, 12, cardCount, 0x2000D, 0, 0);
}

void CCC07ExecSub(CardGameWork *work, CardHand *hand, CardCursorPosition *position)
{
    CardPlaySide *side = (CardPlaySide *) hand;
    int destinationIndex;

    for (destinationIndex = 0; destinationIndex < 4; destinationIndex++) {
        if (side->disposal[destinationIndex].battle.layers[0].cardId < 0)
            break;
    }

    CGPMoveDSBATTLEwork(&side->disposal[destinationIndex].battle,
                        &side->battle[position->index].battle);
}

void CCC10ExecSub(int side, CardGameWork *work, CardHand *hand,
                  CardCursorPosition *position)
{
    CardPlaySide *owner = (CardPlaySide *) hand;

    owner->battle[position->index].battle.layers[0].life -= 2;
    if (owner->battle[position->index].battle.layers[0].life < 0)
        owner->battle[position->index].battle.layers[0].life = 0;

    CardSetEffect(side, work, 10, 0, 0,
                  owner->battle[position->index].battle.position, 0);
    CardSetEffect(side, work, 13, -2, 0x20008,
                  owner->battle[position->index].battle.position,
                  &owner->battle[position->index].battle);
}

void CCC26ExecSub(int side, CardGameWork *work, CardPlaySide *owner,
                  CardCursorPosition *position) {
    owner->battle[position->index].battle.layers[0].boostCount++;
    CardSetEffect(side, work, 10, 0, 0x2000D, owner->battle[position->index].battle.position, 0);
    CardCursorPassive(CardCursorFor(work));
}

void CCC30ExecSub(int side, CardGameWork *work, CardLayerStack *card)
{
    card->flags |= 2;
    CardSetEffect(side, work, 10, 0, 0x2000D, card->position, 0);
}

void CCC31ExecSub(int side, CardGameWork *work, CardLayerStack *card)
{
    float *position = card->position;

    card->layers[0].life--;
    if (card->layers[0].life < 0)
        card->layers[0].life = 0;

    CardSetEffect(side, work, 10, 0, 0, position, 0);
    CardSetEffect(side, work, 13, -1, 0x20008, position, card);
}

void CCC38ExecSub(int side, CardGameWork *work, CardPlaySide *owner,
                  CardCursorPosition *position) {
    owner->disposal[position->index].battle.layers[0].life -= 2;
    if (owner->disposal[position->index].battle.layers[0].life < 0)
        owner->disposal[position->index].battle.layers[0].life = 0;
    CardSetEffect(side, work, 10, 0, 0, owner->disposal[position->index].battle.position, 0);
    CardSetEffect(side, work, 13, -2, 0x20008, owner->disposal[position->index].battle.position,
                  &owner->disposal[position->index].battle);
    CardCursorPassive(CardCursorFor(work));
}

/* Command ids handled by CardPlayCommandCondition (switch cases 82..119).
 * Each comment gives the case target in the jump table at 0x00a4d730. */
enum CardCommandCondition {
    CARD_COND_NOT_MET_82 = 82,  /* 0x00a141a0 */
    CARD_COND_OWN_FLAG2_CARD_83 = 83,  /* 0x00a13688 */
    CARD_COND_OWN_CARD_NOT_FLAG4_84 = 84,  /* 0x00a13750 */
    CARD_COND_OWN_CARD_NOT_FLAG4_85 = 85,  /* 0x00a13750 */
    CARD_COND_ALWAYS_MET_86 = 86,  /* 0x00a1439c */
    CARD_COND_OWN_ANY_CARD_87 = 87,  /* 0x00a137e8 */
    CARD_COND_OTHER_BATTLE_EMPTY_DISPOSAL_88 = 88,  /* 0x00a13950 */
    CARD_COND_OTHER_BATTLE_EMPTY_DISPOSAL_89 = 89,  /* 0x00a13950 */
    CARD_COND_OTHER_BATTLE_CARD_90 = 90,  /* 0x00a13908 */
    CARD_COND_OTHER_BATTLE_CARD_91 = 91,  /* 0x00a13908 */
    CARD_COND_OWN_BATTLE_BYTE13_92 = 92,  /* 0x00a139f0 */
    CARD_COND_ALWAYS_MET_93 = 93,  /* 0x00a1439c */
    CARD_COND_ALWAYS_MET_94 = 94,  /* 0x00a1439c */
    CARD_COND_ALWAYS_MET_95 = 95,  /* 0x00a1439c */
    CARD_COND_OTHER_OPERATION_96 = 96,  /* 0x00a13a70 */
    CARD_COND_OTHER_FLAG1_CARD_OR_OPERATION_97 = 97,  /* 0x00a13ab0 */
    CARD_COND_OTHER_DISPOSAL_FLAG2_DEF4_98 = 98,  /* 0x00a13b70 */
    CARD_COND_OTHER_DISPOSAL_DEF_FLAG1_99 = 99,  /* 0x00a13c08 */
    CARD_COND_ALWAYS_MET_100 = 100,  /* 0x00a1439c */
    CARD_COND_OWN_BATTLE_DEF_FLAG2_101 = 101,  /* 0x00a13c70 */
    CARD_COND_ANY_SLOT_CARD_102 = 102,  /* 0x00a13d88 */
    CARD_COND_OTHER_TWO_LAYERS_103 = 103,  /* 0x00a13e60 */
    CARD_COND_TWO_LAYERS_OWN_OR_OTHER_104 = 104,  /* 0x00a13fb0 */
    CARD_COND_JUNK_FLAGS_105 = 105,  /* 0x00a140d0 */
    CARD_COND_TWO_JUNK_FLAG1_106 = 106,  /* 0x00a14140 */
    CARD_COND_OWN_BATTLE_NOT_CHKSUB_107 = 107,  /* 0x00a141a8 */
    CARD_COND_LAST_JUNK_KIND4_108 = 108,  /* 0x00a14228 */
    CARD_COND_OWN_SLOT_CARD_109 = 109,  /* 0x00a13f20 */
    CARD_COND_ANY_SLOT_CARD_110 = 110,  /* 0x00a13d88 */
    CARD_COND_OTHER_DISPOSAL_CARD_111 = 111,  /* 0x00a13878 */
    CARD_COND_OWN_SLOT_CARD_112 = 112,  /* 0x00a13f20 */
    CARD_COND_OPERATIONS_OR_CARDS_113 = 113,  /* 0x00a13cf8 */
    CARD_COND_OTHER_BATTLE_CARD_114 = 114,  /* 0x00a13908 */
    CARD_COND_ANY_SLOT_CARD_115 = 115,  /* 0x00a13d88 */
    CARD_COND_NOT_MET_116 = 116,  /* 0x00a141a0 */
    CARD_COND_OWN_FLAG2_CARD_117 = 117,  /* 0x00a13688 */
    CARD_COND_JUNK_IDENTITY_OWN_FLAG1_118 = 118,  /* 0x00a14278 */
    CARD_COND_OTHER_DISPOSAL_CARD_119 = 119  /* 0x00a14350 */
};

/* Command phases the gated cases (92, 101, 107, 113) require. */
enum CardCommandConditionPhase {
    CARD_COND_PHASE_SIDE0 = 2,
    CARD_COND_PHASE_SIDE1 = 9
};

/* Result codes of CardPlayCommandCondition. */
enum CardCommandConditionResult {
    CARD_CONDITION_MET = 0,
    CARD_CONDITION_UNMET = 0xD,
    CARD_CONDITION_WRONG_PHASE = 0xB,
    CARD_CONDITION_OTHER_DISPOSAL_FULL = 1
};

/*
 * Checks whether the play condition of command ids 82..119 is met for a side.
 * Cases 92, 101, 107 and 113 first require the side's command phase (2 for
 * side 0, 9 for side 1).
 *
 * Source shape: cc1's cross-jumping merges the "found" blocks (`met = 1`
 * then a jump to the exit) of cases that share an exit label. The
 * do { ... } while (0) wrappers give groups of cases their own exit label and
 * the `goto done` exits skip the not-met store, which reproduces the
 * original's tail layout and the targets of the jump table at 0x00a4d730
 * (cases 82 and 116 enter the first copy of the not-met tail). Tested
 * alternatives: a single wrapper over cases 83..113 (case 97's loop peel merges
 * into a foreign tail: 2 hunks, 880 instead of 878 instructions), one
 * shared exit tail for the found blocks (724 instructions), `return result`
 * in place of `goto done` (the jump table no longer matches), and cutting the
 * wrapper before cases 84..98 or 103..109 (4 to 9 hunks). Cutting before 101
 * and 105 matches.
 */
s32 CardPlayCommandCondition(u16 side, u16 command, CardGameWork *work) {
    CardPlaySide *own = CardPlayerSide(work);
    CardPlaySide *other = CardEnemySide(work);
    int met = 0;
    int junkCount;
    int outcome;
    int result = CARD_CONDITION_MET;
    int i;

    if (side) {
        own = CardEnemySide(work);
        other = CardPlayerSide(work);
    }

    switch (command) {
    case CARD_COND_NOT_MET_82:
    case CARD_COND_NOT_MET_116:
        result = CARD_CONDITION_UNMET;
        goto done;

    do {
    case CARD_COND_OWN_FLAG2_CARD_83:
    case CARD_COND_OWN_FLAG2_CARD_117:
        for (i = 0; i < 4; i++) {
            if (own->disposal[i].battle.layers[0].cardId >= 0 && (own->disposal[i].battle.flags & 2)) {
                met = 1;
                break;
            }
        }
        for (i = 0; i < 4; i++) {
            if (own->battle[i].battle.layers[0].cardId >= 0 && (own->battle[i].battle.flags & 2)) {
                met = 1;
                break;
            }
        }
        break;

    case CARD_COND_OWN_CARD_NOT_FLAG4_84:
    case CARD_COND_OWN_CARD_NOT_FLAG4_85:
        for (i = 0; i < 4; i++) {
            if (own->disposal[i].battle.layers[0].cardId >= 0 && !(work->definitions[own->disposal[i].battle.layers[0].cardId].flags & 4)) {
                met = 1;
                break;
            }
        }
        for (i = 0; i < 4; i++) {
            if (own->battle[i].battle.layers[0].cardId >= 0 && !(work->definitions[own->battle[i].battle.layers[0].cardId].flags & 4)) {
                met = 1;
                break;
            }
        }
        break;

    case CARD_COND_OWN_ANY_CARD_87:
        for (i = 0; i < 4; i++) {
            if (own->disposal[i].battle.layers[0].cardId >= 0) {
                met = 1;
                break;
            }
        }
        for (i = 0; i < 4; i++) {
            if (own->battle[i].battle.layers[0].cardId >= 0) {
                met = 1;
                break;
            }
        }
        break;

    case CARD_COND_OTHER_DISPOSAL_CARD_111:
        for (i = 0; i < 4; i++) {
            if (other->disposal[i].battle.layers[0].cardId >= 0) {
                met = 1;
                break;
            }
        }
        for (i = 0; i < 4; i++) {
            if (other->battle[i].battle.layers[0].cardId >= 0) {
                met = 1;
                break;
            }
        }
        break;

    case CARD_COND_OTHER_BATTLE_CARD_90:
    case CARD_COND_OTHER_BATTLE_CARD_91:
    case CARD_COND_OTHER_BATTLE_CARD_114:
        for (i = 0; i < 4; i++) {
            if (other->battle[i].battle.layers[0].cardId >= 0) {
                met = 1;
                break;
            }
        }
        break;

    case CARD_COND_OTHER_BATTLE_EMPTY_DISPOSAL_88:
    case CARD_COND_OTHER_BATTLE_EMPTY_DISPOSAL_89:
        met = 0; /* already 0 here, but the store is in the original */
        for (i = 0; i < 4; i++) {
            if (other->battle[i].battle.layers[0].cardId >= 0) {
                met = 1;
                break;
            }
        }
        if (!met)
            break;
        met = 0;
        for (i = 0; i < 4; i++) {
            if (other->disposal[i].battle.layers[0].cardId < 0) {
                met = 1;
                break;
            }
        }
        if (!met)
            result = CARD_CONDITION_OTHER_DISPOSAL_FULL;
        goto done;

    case CARD_COND_OWN_BATTLE_BYTE13_92:
        /* The phase is read with lh (signed halfword), hence the s16 casts;
         * dropping them selects lhu and moves the jump-table targets. */
        if (side == 0) {
            if ((s16)work->command.phase != CARD_COND_PHASE_SIDE0)
                result = CARD_CONDITION_WRONG_PHASE;
        } else
            result = (s16)work->command.phase == CARD_COND_PHASE_SIDE1 ? CARD_CONDITION_MET : CARD_CONDITION_WRONG_PHASE;
        if (result)
            return result;
        /* 0x00a139f8: lbu reads the card definition byte at +0x13 and compares it
         * with 1; the field has no name yet, so the unmodeled span is read here. */
        for (i = 0; i < 4; i++) {
            if (own->battle[i].battle.layers[0].cardId >= 0 && work->definitions[own->battle[i].battle.layers[0].cardId].attackType == 1) {
                met = 1;
                break;
            }
        }
        break;

    case CARD_COND_OTHER_OPERATION_96:
        for (i = 0; i < 4; i++) {
            if (other->operations[i] >= 0) {
                met = 1;
                break;
            }
        }
        break;

    case CARD_COND_OTHER_FLAG1_CARD_OR_OPERATION_97:
        for (i = 0; i < 4; i++) {
            if (other->disposal[i].battle.layers[0].cardId >= 0 && (other->disposal[i].battle.flags & 1)) {
                met = 1;
                break;
            }
        }
        for (i = 0; i < 4; i++) {
            if (other->operations[i] >= 0 && (other->operationFlags[i] & 1)) {
                met = 1;
                break;
            }
        }
        break;

    case CARD_COND_OTHER_DISPOSAL_FLAG2_DEF4_98:
        for (i = 0; i < 4; i++) {
            /* The disposal flag is read at 0x00a13b84 (lbu v0,1788(a1)) from the same base
             * register as the battle[i] slot, i.e. as the slot four entries past
             * battle[i]; that is disposal[i] because CardPlaySide stores disposal[4]
             * right after battle[4]. Writing it as
             * other->disposal[i].battle.flags shrinks the function by 16 bytes and
             * shifts every branch target after this case. */
            CardBoardSlot *battleRow = &other->battle[i];
            s16 id = other->disposal[i].battle.layers[0].cardId;
            if (id >= 0 && (battleRow[4].battle.flags & 2) && (work->definitions[id].flags & 4)) {
                met = 1;
                break;
            }
            id = other->battle[i].battle.layers[0].cardId;
            if (id >= 0 && (other->battle[i].battle.flags & 2) && (work->definitions[id].flags & 4)) {
                met = 1;
                break;
            }
        }
        break;

    case CARD_COND_OTHER_DISPOSAL_DEF_FLAG1_99:
        for (i = 0; i < 4; i++) {
            s16 id = other->disposal[i].battle.layers[0].cardId;
            if (id >= 0 && (other->disposal[i].battle.flags & 1) && (work->definitions[id].flags & 1)) {
                met = 1;
                break;
            }
        }
        break;

    } while (0);
    if (!met)
        result = CARD_CONDITION_UNMET;
    break;

    do {
    case CARD_COND_OWN_BATTLE_DEF_FLAG2_101:
        if (side == 0) {
            if ((s16)work->command.phase != CARD_COND_PHASE_SIDE0)
                result = CARD_CONDITION_WRONG_PHASE;
        } else
            result = (s16)work->command.phase == CARD_COND_PHASE_SIDE1 ? CARD_CONDITION_MET : CARD_CONDITION_WRONG_PHASE;
        if (result)
            return result;
        for (i = 0; i < 4; i++) {
            if (own->battle[i].battle.layers[0].cardId >= 0 && (work->definitions[own->battle[i].battle.layers[0].cardId].flags & 2)) {
                met = 1;
                break;
            }
        }
        break;

    case CARD_COND_OPERATIONS_OR_CARDS_113:
        if (side == 0) {
            if ((s16)work->command.phase != CARD_COND_PHASE_SIDE0)
                result = CARD_CONDITION_WRONG_PHASE;
        } else
            result = (s16)work->command.phase == CARD_COND_PHASE_SIDE1 ? CARD_CONDITION_MET : CARD_CONDITION_WRONG_PHASE;
        if (result)
            return result;
        for (i = 0; i < 4; i++) {
            if (own->operations[i] >= 0) {
                met = 1;
                break;
            }
            if (other->operations[i] >= 0) {
                met = 1;
                break;
            }
        }
        /* falls through into the shared 102/110/115 scan */

    case CARD_COND_ANY_SLOT_CARD_102:
    case CARD_COND_ANY_SLOT_CARD_110:
    case CARD_COND_ANY_SLOT_CARD_115:
        for (i = 0; i < 4; i++) {
            s16 id = own->disposal[i].battle.layers[0].cardId;
            if (id >= 0) {
                met = 1;
                break;
            }
            if (other->disposal[i].battle.layers[0].cardId >= 0) {
                met = 1;
                break;
            }
        }
        for (i = 0; i < 4; i++) {
            s16 id = own->battle[i].battle.layers[0].cardId;
            if (id >= 0) {
                met = 1;
                break;
            }
            if (other->battle[i].battle.layers[0].cardId >= 0) {
                met = 1;
                break;
            }
        }
        break;

    case CARD_COND_OTHER_TWO_LAYERS_103:
        for (i = 0; i < 4; i++) {
            if (other->disposal[i].battle.layers[0].cardId >= 0 && other->disposal[i].battle.layers[1].cardId >= 0) {
                met = 1;
                break;
            }
        }
        if (met)
            break;
        for (i = 0; i < 4; i++) {
            if (other->battle[i].battle.layers[0].cardId >= 0 && other->battle[i].battle.layers[1].cardId >= 0) {
                met = 1;
                break;
            }
        }
        break;

    case CARD_COND_OWN_SLOT_CARD_109:
    case CARD_COND_OWN_SLOT_CARD_112:
        for (i = 0; i < 4; i++) {
            if (own->battle[i].battle.layers[0].cardId >= 0) {
                met = 1;
                break;
            }
        }
        for (i = 0; i < 4; i++) {
            if (own->disposal[i].battle.layers[0].cardId >= 0) {
                met = 1;
                break;
            }
        }
        break;

    case CARD_COND_TWO_LAYERS_OWN_OR_OTHER_104:
        for (i = 0; i < 4; i++) {
            s16 id = own->disposal[i].battle.layers[0].cardId;
            if (id >= 0 && own->disposal[i].battle.layers[1].cardId >= 0) {
                met = 1;
                break;
            }
            if (other->disposal[i].battle.layers[0].cardId >= 0 && other->disposal[i].battle.layers[1].cardId >= 0) {
                met = 1;
                break;
            }
        }
        for (i = 0; i < 4; i++) {
            s16 id = own->battle[i].battle.layers[0].cardId;
            if (id >= 0 && own->battle[i].battle.layers[1].cardId >= 0) {
                met = 1;
                break;
            }
            if (other->battle[i].battle.layers[0].cardId >= 0 && other->battle[i].battle.layers[1].cardId >= 0) {
                met = 1;
                break;
            }
        }
        break;

    } while (0);
    if (!met)
        result = CARD_CONDITION_UNMET;
    break;

    do {
    case CARD_COND_JUNK_FLAGS_105: {
        junkCount = CardPlayJunkCnt(&own->hand);
        for (i = 0; i < junkCount; i++) {
            if (work->definitions[own->junk[i]].flags & 0x68) {
                met = 1;
                break;
            }
        }
        break;
    }

    case CARD_COND_TWO_JUNK_FLAG1_106: {
        junkCount = CardPlayJunkCnt(&own->hand);
        met = 0;
        for (i = 0; i < junkCount; i++) {
            if (work->definitions[own->junk[i]].flags & 1)
                met++;
        }
        if (met < 2)
            result = CARD_CONDITION_UNMET;
        goto done;
    }

    case CARD_COND_OWN_BATTLE_NOT_CHKSUB_107:
        if (side == 0) {
            if ((s16)work->command.phase != CARD_COND_PHASE_SIDE0) {
                met = 1;
                result = CARD_CONDITION_WRONG_PHASE;
            }
        } else {
            if ((s16)work->command.phase != CARD_COND_PHASE_SIDE1) {
                met = 1;
                result = CARD_CONDITION_WRONG_PHASE;
            }
        }
        if (result == CARD_CONDITION_MET) {
            for (i = 0; i < 4; i++) {
                if (own->battle[i].battle.layers[0].cardId >= 0 && CCC107ChkSub(&own->battle[i].battle) == 0) {
                    met = 1;
                    break;
                }
            }
            /* Assigned through a temporary: `result = met ? result : ...` merges
             * two arms and moves the case 107 tail. */
            outcome = met ? result : CARD_CONDITION_UNMET;
            result = outcome;
        }
        break;

    case CARD_COND_LAST_JUNK_KIND4_108: {
        junkCount = CardPlayJunkCnt(&own->hand) - 1;
        if (junkCount < 0)
            break;
        if (work->definitions[own->junk[junkCount]].kind & 4)
            met = 1;
        break;
    }

    } while (0);
    if (!met)
        result = CARD_CONDITION_UNMET;
    break;

    case CARD_COND_JUNK_IDENTITY_OWN_FLAG1_118: {
        junkCount = CardPlayJunkCnt(&own->hand);
        for (i = 0; i < junkCount; i++) {
            if ((work->definitions[own->junk[i]].flags & 1) && CardPlayIdentityCheck(work, &own->hand, work->definitions[own->junk[i]].identity) == 0) {
                met = 1;
                break;
            }
        }
        if (met) {
            for (i = 0; i < 4; i++) {
                if (own->battle[i].battle.layers[0].cardId >= 0 && (work->definitions[own->battle[i].battle.layers[0].cardId].flags & 1)) {
                    met = 2;
                    break;
                }
            }
        }
        if (met != 2)
            result = CARD_CONDITION_UNMET;
        break;
    }

    case CARD_COND_OTHER_DISPOSAL_CARD_119:
        for (i = 0; i < 4; i++) {
            if (other->disposal[i].battle.layers[0].cardId >= 0) {
                met = 1;
                break;
            }
        }
        result = met ? CARD_CONDITION_MET : CARD_CONDITION_UNMET;
        break;

    }
done:
    return result;
}

s16 *CPCSearchCommWk(u16 side, CardGameWork *work) {
    int i;

    if (side == 0) {
        for (i = 0; i < 40; i++) {
            if (work->commandQueue[0][i][CARD_QUEUE_PHASE] < 0)
                break;
        }
        if (i == 40)
            return 0;
        return work->commandQueue[0][i];
    }
    for (i = 0; i < 40; i++) {
        if (work->commandQueue[1][i][CARD_QUEUE_PHASE] < 0)
            break;
    }
    if (i == 40)
        return 0;
    return work->commandQueue[1][i];
}

/*
 * Copies a battle or disposal slot's header bytes and, for each of its forty
 * layers, the state, life and card id.
 */
void CPCPSwapDSBATTLE(void *destination, void *source)
{
    CardLayerStackView *dst = destination;
    CardLayerStackView *src = source;
    int i;

    dst->flags = src->flags;
    dst->moveFrames = src->moveFrames;
    dst->power = src->power;
    dst->powerLimit = src->powerLimit;
    for (i = 0; i < 40; i++) {
        dst->layers[i].life = src->layers[i].life;
        dst->layers[i].state = src->layers[i].state;
        dst->layers[i].cardId = src->layers[i].cardId;
    }
}

static inline int CardPlayerAiFlag(int flags) {
    return flags & 0x40;
}

static inline int CardEnemyAiFlag(int flags) {
    return flags & 0x20;
}

static inline int CardLowByteAiFlag(u8 flags, u8 mask) {
    return flags & mask;
}

s32 CardPlayCommandPlay(u16 side, u16 command, CardGameWork *work,
                         s16 operand, s16 target) {
    s16 *queued;
    CardCursorPosition position;
    CardBoardSlot swapFirst;
    CardBoardSlot swapSecond;
    float translation[4];
    float scale[4];
    float matrix[4][4];
    int complete = 0;
    int pressed;
    int opponentPressed;
    int repeated;
    CardPlaySide *ownSide;
    u16 opponentRepeated;
    CardPlaySide *otherSide;
    int controlledByAi;
    int held;
    int opponentSide;
    int opponentControlledByAi;
    int i;
    int destructionIndex;
    int slotIndex;
    int ownJunkCount;
    int damageIndex;
    int rotated;
    int flags;
    /* Function-scope pointer the original spills beside the side pointers: the
     * attacker's position is addressed from the side record plus whole slots. */
    CardPlaySide *attackerRecord;
    CardPlaySide *enemySide;
    CardPlaySide *playerSide;

    if (side == 0) {
        playerSide = CardPlayerSide(work);
        enemySide = CardEnemySide(work);
        flags = work->flags;
        controlledByAi = CardPlayerAiFlag(flags) != 0;
        held = CardCommandHeld(work, 0);
        pressed = CardCommandPressed(work, 0);
        opponentPressed = CardCommandPressed(work, 1);
        repeated = CardCommandRepeated(work, 0);
        opponentRepeated = CardCommandRepeated(work, 1);
        ownSide = playerSide;
        otherSide = enemySide;
        opponentSide = 1;
        opponentControlledByAi = 0;
        if (CardEnemyAiFlag(flags))
            opponentControlledByAi = 1;
    } else {
        enemySide = CardEnemySide(work);
        playerSide = CardPlayerSide(work);
        ownSide = enemySide;
        otherSide = playerSide;
        flags = work->flags;
        held = CardCommandHeld(work, 1);
        controlledByAi = CardEnemyAiFlag(flags) != 0;
        pressed = CardCommandPressed(work, 1);
        opponentPressed = CardCommandPressed(work, 0);
        repeated = CardCommandRepeated(work, 1);
        opponentRepeated = CardCommandRepeated(work, 0);
        opponentSide = 0;
        opponentControlledByAi = CardPlayerAiFlag(flags) != 0;
    }

    switch (command) {
    case 59: {
        for (i = 0; i < 4; i++) {
            if (ownSide->battle[i].battle.layers[0].cardId >= 0)
                ownSide->battle[i].battle.layers[0].life = 0;
        }
        for (i = 0; i < 4; i++) {
            if (ownSide->disposal[i].battle.layers[0].cardId >= 0)
                ownSide->disposal[i].battle.layers[0].life = 0;
        }
        complete = 1;
        break;
    }
    case 1:
        switch (CardCommandStep(work)) {
        case 0:
            if (operand >= 0)
                CGPSetErrorMessPlus(side, work, 0x24, operand);
            CardCommandAdvance(work);
            break;
        case 1:
            complete = 1;
            CardPlayDrawCard(&ownSide->hand);
            break;
        }
        break;
    case 2:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = 1;
            queued[CARD_QUEUE_PHASE] = 6;
            queued[CARD_QUEUE_OPERAND] = 0x17;
            if (side != 0)
                queued[CARD_QUEUE_PHASE] = 13;
        }
        complete = 1;
        break;
    case 58:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = 1;
            queued[CARD_QUEUE_PHASE] = 6;
            queued[CARD_QUEUE_OPERAND] = 0x18;
            if (side != 0)
                queued[CARD_QUEUE_PHASE] = 13;
        }
        complete = 1;
        break;
    case 3:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = 4;
            queued[CARD_QUEUE_PHASE] = CardCommandPhase(work);
            queued[CARD_QUEUE_OPERAND] = operand;
        }
        complete = 1;
        break;
    case 7:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = 8;
            queued[CARD_QUEUE_PHASE] = CardCommandPhase(work);
        }
        complete = 1;
        break;
    case 20:
    case 55: {
        int disposeZone;
        int battleZone;
        disposeZone = 6;
        battleZone = 5;
        if (side != 0) {
            disposeZone = 2;
            battleZone = 4;
        }
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetPermanentMess(work, 11);
            CardCursorPassive(CardCursorFor(work));
            for (i = 0; i < 4; i++) {
                if (ownSide->disposal[i].battle.layers[0].cardId >= 0 &&
                    (ownSide->disposal[i].battle.flags & 2)) {
                    position.zone = disposeZone;
                    position.index = i;
                    CardCursorActive(CardCursorFor(work), &position, 1);
                }
            }
            for (i = 0; i < 4; i++) {
                if (ownSide->battle[i].battle.layers[0].cardId >= 0 &&
                    (ownSide->battle[i].battle.flags & 2)) {
                    position.zone = battleZone;
                    position.index = i;
                    CardCursorActive(CardCursorFor(work), &position, 1);
                }
            }
            CardCommandAdvance(work);
            break;
        case 1:
            CGPCursorMove(side, work);
            CGPCursor2Pos(&D_00A57750, work);
            if (pressed & 0x20) {
                if ((disposeZone == D_00A57750.zone || battleZone == D_00A57750.zone) &&
                    D_00A57750.cardId >= 0 &&
                    (CardSelectBattle(ownSide, D_00A57750.index,
                                      disposeZone == D_00A57750.zone)->flags & 2)) {
                    CardCommandAdvance(work);
                    xglSoundEffectNormalID(1, 0);
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
            } else if (pressed & 0x40) {
                xglSoundEffectNormalID(5, 0);
            }
            break;
        case 2:
            complete = 1;
            CCC02ExecSub(&CardSelectBattle(ownSide, D_00A57750.index,
                                           disposeZone == D_00A57750.zone)->flags);
            CardCursorPassive(CardCursorFor(work));
            break;
        }
        break;
    }
    case 21: {
        int disposeZone;
        int battleZone;
        disposeZone = 6;
        battleZone = 5;
        if (side != 0) {
            disposeZone = 2;
            battleZone = 4;
        }
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetPermanentMess(work, 7);
            CardCursorPassive(CardCursorFor(work));
            for (i = 0; i < 4; i++) {
                s16 *commandId = &ownSide->disposal[i].battle.layers[0].cardId;
                int cardId = (s16) *commandId;
                if (cardId >= 0 && !(CardDefinitionsFor(work)[cardId].flags & 4)) {
                    position.zone = disposeZone;
                    position.index = i;
                    CardCursorActive(CardCursorFor(work), &position, 1);
                }
            }
            for (i = 0; i < 4; i++) {
                s16 *commandId = &ownSide->battle[i].battle.layers[0].cardId;
                int cardId = (s16) *commandId;
                if (cardId >= 0 && !(CardDefinitionsFor(work)[cardId].flags & 4)) {
                    position.zone = battleZone;
                    position.index = i;
                    CardCursorActive(CardCursorFor(work), &position, 1);
                }
            }
            CardCommandAdvance(work);
            break;
        case 1:
            CGPCursorMove(side, work);
            CGPCursor2Pos(&D_00A57758, work);
            if (pressed & 0x20) {
                if ((disposeZone == D_00A57758.zone || battleZone == D_00A57758.zone) &&
                    D_00A57758.cardId >= 0 && !(CardDefinitionsFor(work)[D_00A57758.cardId].flags & 4)) {
                    CardCommandAdvance(work);
                    xglSoundEffectNormalID(1, 0);
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
            } else if (pressed & 0x40) {
                xglSoundEffectNormalID(5, 0);
            }
            break;
        case 2:
            complete = 1;
            CCC03ExecSub(side, work, &ownSide->hand,
                         CardSelectBattle(ownSide, D_00A57758.index,
                                          disposeZone == D_00A57758.zone));
            CardCursorPassive(CardCursorFor(work));
            break;
        }
        break;
    }
    case 22:
        complete = 1;
        CCC04ExecSub(side, work, &ownSide->hand);
        break;
    case 23:
        switch (CardCommandStep(work)) {
        case 0:
            CardCommandAdvance(work);
            D_00A57760 = 5;
            CardSetEffect(side, work, 12, 5, 0x2000D, 0, 0);
            /* The first recovery occurs in the same tick as setup. */
        case 1:
            CardPlayRecavery(&ownSide->hand);
            CardSetEffect(side, work, 10, 0, 0, 0, 0);
            {
                /* Typed reference: the original schedules this store apart
                 * from the step store below. */
                int *remaining = &D_00A57760;
                (*remaining)--;
            }
            CardCommandAdvance(work);
            /* Recheck the count without waiting another tick. */
        case 2:
            if (D_00A57760 > 0)
                CardCommandRewind(work);
            else
                CardCommandAdvance(work);
            break;
        case 3:
            complete = 1;
            break;
        }
        break;
    case 24: {
        int i;
        switch (CardCommandStep(work)) {
        case 0:
            D_00A57764 = 0;
            for (i = 0; i < 4; i++) {
                if (ownSide->disposal[i].battle.layers[0].cardId > 0)
                    D_00A57764++;
            }
            for (i = 0; i < 4; i++) {
                if (ownSide->battle[i].battle.layers[0].cardId > 0)
                    D_00A57764++;
            }
            CardSetEffect(side, work, 12, D_00A57764, 0x2000D, 0, 0);
            CardCommandAdvance(work);
        case 1:
            CardPlayRecavery(&ownSide->hand);
            CardSetEffect(side, work, 10, 0, 0, 0, 0);
            {
                /* Typed reference: the original schedules this store apart
                 * from the step store below. */
                int *remaining = &D_00A57764;
                (*remaining)--;
            }
            CardCommandAdvance(work);
        case 2:
            if (D_00A57764 > 0)
                CardCommandRewind(work);
            else
                CardCommandAdvance(work);
            break;
        case 3:
            complete = 1;
            break;
        }
        break;
    }
    case 25:
    case 26: {
        int battleZone;
        battleZone = 4;
        if (side != 0)
            battleZone = 5;
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetPermanentMess(work, 14);
            CardCursorPassive(CardCursorFor(work));
            for (i = 0; i < 4; i++) {
                if (otherSide->battle[i].battle.layers[0].cardId >= 0) {
                    position.zone = battleZone;
                    position.index = i;
                    CardCursorActive(CardCursorFor(work), &position, 1);
                }
            }
            CardCommandAdvance(work);
            break;
        case 1:
            CGPCursorMove(side, work);
            CGPCursor2Pos(&D_00A57768, work);
            if (pressed & 0x20) {
                if (battleZone == D_00A57768.zone && D_00A57768.cardId >= 0) {
                    CardCommandAdvance(work);
                    xglSoundEffectNormalID(1, 0);
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
            } else if (pressed & 0x40) {
                xglSoundEffectNormalID(5, 0);
            }
            break;
        case 2:
            CCC07ExecSub(work, &otherSide->hand, &D_00A57768);
            CardCursorPassive(CardCursorFor(work));
            complete = 1;
            break;
        }
        break;
    }
    case 50: {
        int disposalTargetZone;
        int battleTargetZone;
        disposalTargetZone = 2;
        battleTargetZone = 4;
        if (side != 0) {
            disposalTargetZone = 6;
            battleTargetZone = 5;
        }
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetPermanentMess(work, 9);
            CardCursorPassive(CardCursorFor(work));
            for (i = 0; i < 4; i++) {
                if (otherSide->disposal[i].battle.layers[0].cardId >= 0) {
                    position.zone = disposalTargetZone;
                    position.index = i;
                    CardCursorActive(CardCursorFor(work), &position, 1);
                }
            }
            for (i = 0; i < 4; i++) {
                if (otherSide->battle[i].battle.layers[0].cardId >= 0) {
                    position.zone = battleTargetZone;
                    position.index = i;
                    CardCursorActive(CardCursorFor(work), &position, 1);
                }
            }
            CardCommandAdvance(work);
            break;
        case 1:
            CGPCursorMove(side, work);
            CGPCursor2Pos(&D_00A57770, work);
            if (pressed & 0x20) {
                if ((disposalTargetZone == D_00A57770.zone || battleTargetZone == D_00A57770.zone) && D_00A57770.cardId >= 0) {
                    CardCommandAdvance(work);
                    xglSoundEffectNormalID(1, 0);
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
            } else if (pressed & 0x40) {
                xglSoundEffectNormalID(5, 0);
            }
            break;
        case 2: {
            CardLayerStack *selected;
            if (disposalTargetZone == D_00A57770.zone)
                selected = &otherSide->disposal[D_00A57770.index].battle;
            else
                selected = &otherSide->battle[D_00A57770.index].battle;
            complete = 1;
            CCC31ExecSub(opponentSide, work, selected);
            CardCursorPassive(CardCursorFor(work));
            break;
        }
        }
        break;
    }
    case 52:
        CPCPSwapDSBATTLE(&swapFirst, &otherSide->battle[0].battle);
        CPCPSwapDSBATTLE(&swapSecond, &otherSide->battle[1].battle);
        CPCPSwapDSBATTLE(&otherSide->battle[0].battle, &otherSide->battle[2].battle);
        complete = 1;
        CPCPSwapDSBATTLE(&otherSide->battle[1].battle, &otherSide->battle[3].battle);
        CPCPSwapDSBATTLE(&otherSide->battle[2].battle, &swapFirst);
        CPCPSwapDSBATTLE(&otherSide->battle[3].battle, &swapSecond);
        break;
    case 57: {
        int disposalTargetZone;
        disposalTargetZone = 2;
        if (side != 0)
            disposalTargetZone = 6;
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetPermanentMess(work, 9);
            CardCursorPassive(CardCursorFor(work));
            for (i = 0; i < 4; i++) {
                if (otherSide->disposal[i].battle.layers[0].cardId >= 0) {
                    position.zone = disposalTargetZone;
                    position.index = i;
                    CardCursorActive(CardCursorFor(work), &position, 1);
                }
            }
            CardCommandAdvance(work);
            break;
        case 1:
            CGPCursorMove(side, work);
            CGPCursor2Pos(&D_00A57778, work);
            if (pressed & 0x20) {
                if (disposalTargetZone == D_00A57778.zone && D_00A57778.cardId >= 0) {
                    CardCommandAdvance(work);
                    xglSoundEffectNormalID(1, 0);
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
            } else if (pressed & 0x40) {
                xglSoundEffectNormalID(5, 0);
            }
            break;
        case 2:
            complete = 1;
            CCC38ExecSub(opponentSide, work, otherSide, &D_00A57778);
            break;
        default:
            break;
        }
        break;
    }
    case 27:
    case 28: {
        int battleZone;
        battleZone = 4;
        if (side != 0)
            battleZone = 5;
        switch (CardCommandStep(work)) {
        case 0:
            CardCursorPassive(CardCursorFor(work));
            for (i = 0; i < 4; i++) {
                if (otherSide->battle[i].battle.layers[0].cardId >= 0) {
                    position.zone = battleZone;
                    position.index = i;
                    CardCursorActive(CardCursorFor(work), &position, 1);
                }
            }
            CardCommandAdvance(work);
            break;
        case 1:
            CGPCursorMove(side, work);
            CGPCursor2Pos(&D_00A57780, work);
            if (pressed & 0x20) {
                if (battleZone == D_00A57780.zone && D_00A57780.cardId >= 0) {
                    CardCommandAdvance(work);
                    xglSoundEffectNormalID(1, 0);
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
            } else if (pressed & 0x40) {
                xglSoundEffectNormalID(5, 0);
            }
            break;
        case 2:
            complete = 1;
            CCC10ExecSub(opponentSide, work, &otherSide->hand, &D_00A57780);
            CardCursorPassive(CardCursorFor(work));
            break;
        }
        break;
    }
    case 29: {
        int i;
        switch (CardCommandStep(work)) {
        case 0:
            D_00A57788 = 0;
            for (i = 0; i < 4; i++) {
                CardLayerStack *card = &ownSide->battle[i].battle;
                if (card->layers[0].cardId >= 0 && CardChkAttackType(CardDefinitionsFor(work), card) == 1) {
                    int power = CardDefinitionsFor(work)[card->layers[0].cardId].attackPower;
                    if (card->layers[1].cardId >= 0)
                        power = CardDefinitionsFor(work)[card->layers[1].cardId].attackPower;
                    D_00A57788 += CGPCalcPowerPlus(work, card, power);
                }
            }
            CardCommandAdvance(work);
            break;
        case 1: {
            int remainingRecovery;
            CardPlayRecavery(&ownSide->hand);
            CardSetEffect(side, work, 10, 0, 0, 0, 0);
            CardSetEffect(side, work, 12, 1, 0x2000D, 0, 0);
            remainingRecovery = D_00A57788 - 1;
            CardCommandAdvance(work);
            D_00A57788 = remainingRecovery;
        }
        case 2:
            if (D_00A57788 > 0)
                CardCommandRewind(work);
            else
                CardCommandAdvance(work);
            break;
        case 3:
            CardCommandMarkRecovery(work);
            complete = 1;
            break;
        }
        break;
    }
    case 30:
        switch (CardCommandStep(work)) {
        case 0:
            CardCommandAdvance(work);
            D_00A5778C = 2;
            break;
        case 1: {
            int drawsRemaining;
            CardPlayDrawCard(&ownSide->hand);
            drawsRemaining = D_00A5778C - 1;
            CardCommandAdvance(work);
            D_00A5778C = drawsRemaining;
            break;
        }
        case 2:
            if (D_00A5778C > 0)
                CardCommandRewind(work);
            else
                CardCommandAdvance(work);
            break;
        case 3:
            complete = 1;
            break;
        }
        break;
    case 31:
        switch (CardCommandStep(work)) {
        case 0:
            CardCommandAdvance(work);
            D_00A57790 = 2;
            break;
        case 1: {
            int drawsRemaining;
            CardPlayDrawSute(&ownSide->hand);
            drawsRemaining = D_00A57790 - 1;
            CardCommandAdvance(work);
            D_00A57790 = drawsRemaining;
        }
        case 2:
            if (D_00A57790 > 0)
                CardCommandRewind(work);
            else
                CardCommandAdvance(work);
            break;
        case 3:
            complete = 1;
            break;
        }
        break;
    case 32:
        switch (CardCommandStep(work)) {
        case 0:
            CardCommandAdvance(work);
            CGPSetPermanentMess(work, 3);
            break;
        case 1: {
            int count = CardPlayYamaCnt(&ownSide->hand);
            int rotated;
            if (count >= 6)
                count = 5;
            if (repeated & 0x2000) {
                D_00A57794++;
                xglSoundEffectNormalID(3, 0);
                if (D_00A57794 >= count)
                    D_00A57794 = count - 1;
            }
            if (repeated & 0x8000) {
                D_00A57794--;
                xglSoundEffectNormalID(3, 0);
                if (D_00A57794 < 0)
                    D_00A57794 = 0;
            }
            if (pressed & 0x20) {
                xglSoundEffectNormalID(1, 0);
                CardCommandAdvance(work);
            } else if (pressed & 0x40) {
                xglSoundEffectNormalID(5, 0);
            }
            rotated = 1;
            if (!(held & 0x10)) {
                if ((side != 0 && work->rotStageId == 0) ||
                    (side == 0 && work->rotStageId != 0)) {
                    CGCPHandDispSub3(CardDefinitionsFor(work), &ownSide->hand, -1, 0, 0);
                } else {
                    rotated = 0;
                    CGCPHandDispSub2(CardDefinitionsFor(work), &ownSide->hand, -1, 0);
                }
            }
            CGCPYamaDispSub(&ownSide->hand, D_00A57794, count, held, rotated);
            break;
        }
        case 2: {
            int first = CardPlayYamaCnt(&ownSide->hand);
            if (first >= 6)
                first -= 5;
            else
                first = 0;
            ownSide->hand.cards[CardPlayHandCnt(&ownSide->hand)] = ownSide->deck[first + D_00A57794];
            ownSide->deck[first + D_00A57794] = -1;
            CardPlayCleanYama(&ownSide->hand);
            CardPlayShuffleYama(&ownSide->hand);
            CardCommandAdvance(work);
            break;
        }
        case 3:
        case 10:
            complete = 1;
            break;
        }
        break;
    case 33: {
        int battleZone;
        battleZone = 3;
        if (side != 0)
            battleZone = 7;
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetPermanentMess(work, 12);
            CardCursorPassive(CardCursorFor(work));
            for (i = 0; i < 4; i++) {
                if (otherSide->operations[i] >= 0) {
                    position.zone = battleZone;
                    position.index = i;
                    CardCursorActive(CardCursorFor(work), &position, 1);
                }
            }
            CardCommandAdvance(work);
            break;
        case 1:
            CGPCursorMove(side, work);
            CGPCursor2Pos(&D_00A57798, work);
            if (pressed & 0x20) {
                if (battleZone == D_00A57798.zone && D_00A57798.cardId >= 0) {
                    CardCommandAdvance(work);
                    xglSoundEffectNormalID(1, 0);
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
            } else if (pressed & 0x40) {
                xglSoundEffectNormalID(5, 0);
            }
            break;
        case 2:
            complete = 1;
            CardPlayDesertOperation(&otherSide->hand, D_00A5779A);
            CardCursorPassive(CardCursorFor(work));
            break;
        }
        break;
    }
    case 34: {
        int i;
        int disposalTargetZone;
        int operationTargetZone;
        disposalTargetZone = 2;
        operationTargetZone = 3;
        if (side != 0) {
            disposalTargetZone = 6;
            operationTargetZone = 7;
        }
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetPermanentMess(work, 12);
            CardCursorPassive(CardCursorFor(work));
            for (i = 0; i < 4; i++) {
                if (otherSide->disposal[i].battle.layers[0].cardId >= 0 &&
                    (otherSide->disposal[i].battle.flags & 1)) {
                    D_00A577A0.zone = disposalTargetZone;
                    D_00A577A0.index = i;
                    CardCursorActive(CardCursorFor(work), &D_00A577A0, 1);
                }
            }
            for (i = 0; i < 4; i++) {
                if (otherSide->operations[i] >= 0 && (otherSide->operationFlags[i] & 1)) {
                    D_00A577A0.zone = operationTargetZone;
                    D_00A577A0.index = i;
                    CardCursorActive(CardCursorFor(work), &D_00A577A0, 1);
                }
            }
            CardCommandAdvance(work);
            break;
        case 1:
            CGPCursorMove(side, work);
            CGPCursor2Pos(&D_00A577A0, work);
            if (pressed & 0x20) {
                if (D_00A577A0.cardId >= 0) {
                    if (disposalTargetZone == D_00A577A0.zone) {
                        if (otherSide->disposal[D_00A577A0.index].battle.flags & 1)
                            CardCommandAdvance(work);
                    } else if (operationTargetZone == D_00A577A0.zone &&
                               (otherSide->operationFlags[D_00A577A0.index] & 1)) {
                        CardCommandAdvance(work);
                    }
                }
                if (CardCommandStep(work) == 2)
                    xglSoundEffectNormalID(1, 0);
                else
                    xglSoundEffectNormalID(5, 0);
            } else if (pressed & 0x40) {
                xglSoundEffectNormalID(5, 0);
            }
            break;
        case 2:
            if (disposalTargetZone == D_00A577A0.zone)
                otherSide->disposal[D_00A577A0.index].battle.layers[0].life = 0;
            else
                CardPlayDesertOperation(&otherSide->hand, D_00A577A0.index);
            complete = 1;
            CardCursorPassive(CardCursorFor(work));
            break;
        }
        break;
    }
    case 35: {
        int disposeZone;
        int battleZone;
        CardLayerStack *card;
        disposeZone = 2;
        battleZone = 4;
        if (side != 0) {
            disposeZone = 6;
            battleZone = 5;
        }
        switch (CardCommandStep(work)) {
        case 0: {
            CardBoardSlot *slot;
            CardCommandAdvance(work);
            CardCursorPassive(CardCursorFor(work));
            D_00A577A8.zone = disposeZone;
            for (i = 0; i < 4; i++) {
                slot = &otherSide->disposal[i];
                if (slot->battle.layers[0].cardId >= 0 && (slot->battle.flags & 2) &&
                    (CardDefinitionsFor(work)[slot->battle.layers[0].cardId].flags & 4)) {
                    D_00A577A8.index = i;
                    CardCursorActive(CardCursorFor(work), &D_00A577A8, 1);
                }
            }
            D_00A577A8.zone = battleZone;
            for (i = 0; i < 4; i++) {
                slot = &otherSide->battle[i];
                if (slot->battle.layers[0].cardId >= 0 && (slot->battle.flags & 2) &&
                    (CardDefinitionsFor(work)[slot->battle.layers[0].cardId].flags & 4)) {
                    D_00A577A8.index = i;
                    CardCursorActive(CardCursorFor(work), &D_00A577A8, 1);
                }
            }
            CGPSetPermanentMess(work, 12);
            break;
        }
        case 1:
            CGPCursorMove(side, work);
            CGPCursor2Pos(&D_00A577A8, work);
            if (pressed & 0x20) {
                if (D_00A577A8.cardId >= 0 &&
                    (disposeZone == D_00A577A8.zone || battleZone == D_00A577A8.zone)) {
                    card = &otherSide->disposal[D_00A577A8.index].battle;
                    if (battleZone == D_00A577A8.zone)
                        card = &otherSide->battle[D_00A577A8.index].battle;
                    if ((CardDefinitionsFor(work)[card->layers[0].cardId].flags & 4) && (card->flags & 2)) {
                        CardCursorPassive(CardCursorFor(work));
                        xglSoundEffectNormalID(1, 0);
                        CardCommandAdvance(work);
                    } else {
                        xglSoundEffectNormalID(5, 0);
                    }
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
            } else if (pressed & 0x40) {
                xglSoundEffectNormalID(5, 0);
            }
            break;
        case 2: {
            CardLayerStack *stack = &otherSide->disposal[D_00A577A8.index].battle;
            if (battleZone == D_00A577A8.zone)
                stack = &otherSide->battle[D_00A577A8.index].battle;
            complete = 1;
            stack->layers[0].life = 0;
            CardSetEffect(side, work, 10, 0, 0x20008, stack->position, 0);
            break;
        }
        }
        break;
    }
    case 36: {
        int targetZone;
        targetZone = 2;
        if (side != 0)
            targetZone = 6;
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetPermanentMess(work, 12);
            CardCursorPassive(CardCursorFor(work));
            for (i = 0; i < 4; i++) {
                CardBoardSlot *slot = &otherSide->disposal[i];
                if (slot->battle.layers[0].cardId >= 0 && (CardDefinitionsFor(work)[slot->battle.layers[0].cardId].flags & 1) && (slot->battle.flags & 1)) {
                    D_00A577B0.zone = targetZone;
                    D_00A577B0.index = i;
                    CardCursorActive(CardCursorFor(work), &D_00A577B0, 1);
                }
            }
            CardCommandAdvance(work);
            break;
        case 1:
            CGPCursorMove(side, work);
            CGPCursor2Pos(&D_00A577B0, work);
            if (pressed & 0x20) {
                if (D_00A577B0.cardId >= 0 && targetZone == D_00A577B0.zone) {
                    if ((CardDefinitionsFor(work)[otherSide->disposal[D_00A577B0.index].battle.layers[0].cardId].flags & 1) &&
                        (otherSide->disposal[D_00A577B0.index].battle.flags & 1)) {
                        xglSoundEffectNormalID(1, 0);
                        CardCommandAdvance(work);
                    } else {
                        xglSoundEffectNormalID(5, 0);
                    }
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
            } else if (pressed & 0x40) {
                xglSoundEffectNormalID(5, 0);
            }
            break;
        case 2:
            complete = 1;
            otherSide->disposal[D_00A577B2].battle.layers[0].life = 0;
            CardCursorPassive(CardCursorFor(work));
            break;
        }
        break;
    }
    case 37:
        ownSide->hand.commandFlags |= 1;
        complete = 1;
        break;
    case 79:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = 80;
            queued[CARD_QUEUE_PHASE] = CardCommandPhase(work);
        }
        complete = 1;
        break;
    case 80:
        CGPSetErrorMessPlus(side, work, 0x24, 100);
        complete = 1;
        break;
    case 38: {
        int i;
        for (i = 0; i < 4; i++) {
            if (ownSide->battle[i].battle.layers[0].cardId >= 0 &&
                (CardDefinitionsFor(work)[ownSide->battle[i].battle.layers[0].cardId].flags & 2)) {
                ownSide->battle[i].battle.layers[0].life = 1;
                ownSide->battle[i].battle.flags |= 0x80;
                CardSetEffect(side, work, 10, 0, 0x2000D, ownSide->battle[i].battle.position, 0);
            }
        }
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = 39;
            queued[CARD_QUEUE_PHASE] = 13;
            if (side != 0)
                queued[CARD_QUEUE_PHASE] = 6;
        }
        complete = 1;
        break;
    }
    case 39: {
        int i;
        int count;
        switch (CardCommandStep(work)) {
        case 0: {
            count = 0;
            for (i = 0; i < 4; i++) {
                s16 *commandId = &ownSide->battle[i].battle.layers[0].cardId;
                int cardId = (s16) *commandId;
                if (cardId >= 0 && (CardDefinitionsFor(work)[cardId].flags & 2)) {
                    count = 1;
                    break;
                }
            }
            if (count) {
                CGPSetErrorMessPlus(side, work, 0x24, 0x65);
                CardCommandAdvance(work);
            } else {
                complete = 1;
            }
            break;
        }
        case 1: {
            count = 0;
            for (i = 0; i < 4; i++) {
                if (ownSide->battle[i].battle.layers[0].cardId >= 0 &&
                    (CardDefinitionsFor(work)[ownSide->battle[i].battle.layers[0].cardId].flags & 2) &&
                    ownSide->battle[i].battle.layers[0].life != 0) {
                    int remaining;
                    CardPlayDesertBattle(&ownSide->hand, &ownSide->battle[i].battle);
                    count += 6;
                    remaining = 5;
                    do {
                        remaining--;
                        CardPlayCostCard(&otherSide->hand);
                    } while (remaining >= 0);
                }
            }
            if (count != 0) {
                CardSetEffect(side, work, 11, count, 0x20008, 0, 0);
                CardSetEffect(side, work, 5, 0, 0, 0, 0);
            }
            complete = 1;
            break;
        }
        }
        break;
    }
    case 77: {
        int i;
        for (i = 0; i < 4; i++) {
            if (ownSide->battle[i].battle.layers[0].cardId >= 0 &&
                (CardDefinitionsFor(work)[ownSide->battle[i].battle.layers[0].cardId].flags & 2)) {
                ownSide->battle[i].battle.layers[0].life = 1;
                ownSide->battle[i].battle.flags |= 0x80;
                CardSetEffect(side, work, 10, 0, 0x2000D, ownSide->battle[i].battle.position, 0);
            }
        }
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = 78;
            queued[CARD_QUEUE_PHASE] = 13;
            if (side != 0)
                queued[CARD_QUEUE_PHASE] = 6;
        }
        complete = 1;
        break;
    }
    case 78: {
        int i;
        int count;
        switch (CardCommandStep(work)) {
        case 0: {
            count = 0;
            for (i = 0; i < 4; i++) {
                s16 *commandId = &ownSide->battle[i].battle.layers[0].cardId;
                int cardId = (s16) *commandId;
                if (cardId >= 0 && (CardDefinitionsFor(work)[cardId].flags & 2)) {
                    count = 1;
                    break;
                }
            }
            if (count) {
                CGPSetErrorMessPlus(side, work, 0x24, 0x36);
                CardCommandAdvance(work);
            } else {
                complete = 1;
            }
            break;
        }
        case 1: {
            count = 0;
            for (i = 0; i < 4; i++) {
                if (ownSide->battle[i].battle.layers[0].cardId >= 0 &&
                    (CardDefinitionsFor(work)[ownSide->battle[i].battle.layers[0].cardId].flags & 2) &&
                    ownSide->battle[i].battle.layers[0].life != 0) {
                    int remaining;
                    CardPlayDesertBattle(&ownSide->hand, &ownSide->battle[i].battle);
                    count += 4;
                    remaining = 3;
                    do {
                        remaining--;
                        CardPlayCostCard(&otherSide->hand);
                    } while (remaining >= 0);
                }
            }
            if (count != 0) {
                CardSetEffect(side, work, 11, count, 0x20008, 0, 0);
                CardSetEffect(side, work, 5, 0, 0, 0, 0);
            }
            complete = 1;
            break;
        }
        }
        break;
    }
    case 51: {
        int i;
        int lifeLoss;
        for (i = 0; i < 4; i++) {
            if (ownSide->operations[i] >= 0)
                CardPlayDesertOperation(&ownSide->hand, i);
            if (otherSide->operations[i] >= 0)
                CardPlayDesertOperation(&otherSide->hand, i);
        }
        for (i = 0; i < 4; i++) {
            if (ownSide->disposal[i].battle.layers[0].cardId >= 0) {
                lifeLoss = -ownSide->disposal[i].battle.layers[0].life;
                ownSide->disposal[i].battle.layers[0].life = 0;
                CardSetEffect(side, work, 10, 0, 0, ownSide->disposal[i].battle.position, 0);
                CardSetEffect(side, work, 13, lifeLoss, 0,
                              ownSide->disposal[i].battle.position, &ownSide->disposal[i].battle);
            }
            if (otherSide->disposal[i].battle.layers[0].cardId >= 0) {
                lifeLoss = -otherSide->disposal[i].battle.layers[0].life;
                otherSide->disposal[i].battle.layers[0].life = 0;
                CardSetEffect(opponentSide, work, 10, 0, 0,
                              otherSide->disposal[i].battle.position, 0);
                CardSetEffect(opponentSide, work, 13, lifeLoss, 0,
                              otherSide->disposal[i].battle.position,
                              &otherSide->disposal[i].battle);
            }
        }
        for (i = 0; i < 4; i++) {
            if (ownSide->battle[i].battle.layers[0].cardId >= 0) {
                lifeLoss = -ownSide->battle[i].battle.layers[0].life;
                ownSide->battle[i].battle.layers[0].life = 0;
                CardSetEffect(side, work, 10, 0, 0,
                              ownSide->battle[i].battle.position, 0);
                CardSetEffect(side, work, 13, lifeLoss, 0,
                              ownSide->battle[i].battle.position, &ownSide->disposal[i].battle);
            }
            if (otherSide->battle[i].battle.layers[0].cardId >= 0) {
                lifeLoss = -otherSide->battle[i].battle.layers[0].life;
                otherSide->battle[i].battle.layers[0].life = 0;
                CardSetEffect(opponentSide, work, 10, 0, 0,
                              otherSide->battle[i].battle.position, 0);
                CardSetEffect(opponentSide, work, 13, lifeLoss, 0,
                              otherSide->battle[i].battle.position, &otherSide->battle[i].battle);
            }
        }
        complete = 1;
        xglSoundEffectNormalID(0x20008, 0);
        break;
    }
    case 40: {
        int lifeLoss;
        for (destructionIndex = 0; destructionIndex < 4; destructionIndex++) {
            if (ownSide->battle[destructionIndex].battle.layers[0].cardId >= 0 &&
                (CardDefinitionsFor(work)[ownSide->battle[destructionIndex].battle.layers[0].cardId].flags & 12)) {
                lifeLoss = -ownSide->battle[destructionIndex].battle.layers[0].life;
                ownSide->battle[destructionIndex].battle.layers[0].life = 0;
                CardSetEffect(side, work, 10, 0, 0, ownSide->battle[destructionIndex].battle.position, 0);
                CardSetEffect(side, work, 13, lifeLoss, 0, ownSide->battle[destructionIndex].battle.position,
                              &ownSide->disposal[destructionIndex].battle);
            }
            if (otherSide->battle[destructionIndex].battle.layers[0].cardId >= 0 &&
                (CardDefinitionsFor(work)[otherSide->battle[destructionIndex].battle.layers[0].cardId].flags & 12)) {
                lifeLoss = -otherSide->battle[destructionIndex].battle.layers[0].life;
                otherSide->battle[destructionIndex].battle.layers[0].life = 0;
                CardSetEffect(opponentSide, work, 10, 0, 0, otherSide->battle[destructionIndex].battle.position, 0);
                CardSetEffect(opponentSide, work, 13, lifeLoss, 0,
                              otherSide->battle[destructionIndex].battle.position,
                              &otherSide->battle[destructionIndex].battle);
            }
        }
        complete = 1;
        xglSoundEffectNormalID(0x20008, 0);
        break;
    }
    case 41: {
        int disposalTargetZone;
        int battleTargetZone;
        disposalTargetZone = 2;
        battleTargetZone = 4;
        if (side != 0) {
            disposalTargetZone = 6;
            battleTargetZone = 5;
        }
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetPermanentMess(work, 13);
            CardCursorPassive(CardCursorFor(work));
            for (i = 0; i < 4; i++) {
                if (otherSide->disposal[i].battle.layers[0].cardId >= 0 && otherSide->disposal[i].battle.layers[1].cardId >= 0) {
                    D_00A577B8.zone = disposalTargetZone;
                    D_00A577B8.index = i;
                    CardCursorActive(CardCursorFor(work), &D_00A577B8, 1);
                }
            }
            for (i = 0; i < 4; i++) {
                if (otherSide->battle[i].battle.layers[0].cardId >= 0 && otherSide->battle[i].battle.layers[1].cardId >= 0) {
                    D_00A577B8.zone = battleTargetZone;
                    D_00A577B8.index = i;
                    CardCursorActive(CardCursorFor(work), &D_00A577B8, 1);
                }
            }
            CardCommandAdvance(work);
            break;
        case 1:
            CGPCursorMove(side, work);
            CGPCursor2Pos(&D_00A577B8, work);
            if (pressed & 0x20) {
                if (D_00A577B8.cardId >= 0 &&
                    (disposalTargetZone == D_00A577B8.zone || battleTargetZone == D_00A577B8.zone)) {
                    CardLayerStack *card = &otherSide->battle[D_00A577B8.index].battle;
                    if (disposalTargetZone == D_00A577B8.zone)
                        card = &otherSide->disposal[D_00A577B8.index].battle;
                    if (card->layers[1].cardId >= 0) {
                        xglSoundEffectNormalID(1, 0);
                        CardCommandAdvance(work);
                    } else {
                        xglSoundEffectNormalID(5, 0);
                    }
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
            } else if (pressed & 0x40) {
                xglSoundEffectNormalID(5, 0);
            }
            break;
        case 2: {
            CardLayerStack *stack;
            int junkCount;
            int layerIndex;
            stack = &otherSide->battle[D_00A577B8.index].battle;
            if (disposalTargetZone == D_00A577B8.zone)
                stack = &otherSide->disposal[D_00A577B8.index].battle;
            if (stack->layers[1].cardId != 0x2C) {
                stack->power = CardDefinitionsFor(work)[stack->layers[0].cardId].basePower;
                stack->powerLimit = CardDefinitionsFor(work)[stack->layers[0].cardId].basePower;
            }
            junkCount = CardPlayJunkCnt(&otherSide->hand);
            for (layerIndex = 1; layerIndex < 40; layerIndex++) {
                if (stack->layers[layerIndex].cardId >= 0) {
                    otherSide->junk[junkCount++] = stack->layers[layerIndex].cardId;
                    stack->layers[layerIndex].life = 0;
                    stack->layers[layerIndex].state = 0;
                    stack->layers[layerIndex].cardId = -1;
                }
            }
            CardSetEffect(opponentSide, work, 10, 0, 0x2000D, stack->position, 0);
            CardCursorPassive(CardCursorFor(work));
            complete = 1;
            break;
        }
        }
        break;
    }
    case 42: {
        int opponentJunkCount;
        int layerIndex;
        ownJunkCount = CardPlayJunkCnt(&ownSide->hand);
        opponentJunkCount = CardPlayJunkCnt(&otherSide->hand);
        /* Walk corresponding slots together; append only occupied layers to
         * each player's junk pile. The base layer at index zero stays in place. */
        for (slotIndex = 0; slotIndex < 4; slotIndex++) {
            if (ownSide->disposal[slotIndex].battle.layers[1].cardId >= 0) {
                if (ownSide->disposal[slotIndex].battle.layers[1].cardId != 0x2C) {
                    ownSide->disposal[slotIndex].battle.power = CardDefinitionsFor(work)[ownSide->disposal[slotIndex].battle.layers[0].cardId].basePower;
                    ownSide->disposal[slotIndex].battle.powerLimit = CardDefinitionsFor(work)[ownSide->disposal[slotIndex].battle.layers[0].cardId].basePower;
                }
                for (layerIndex = 1; layerIndex < 40; layerIndex++) {
                    if (ownSide->disposal[slotIndex].battle.layers[layerIndex].cardId >= 0) {
                        CardSetEffect(side, work, 10, 0, 0, ownSide->disposal[slotIndex].battle.position, 0);
                        ownSide->junk[ownJunkCount++] = ownSide->disposal[slotIndex].battle.layers[layerIndex].cardId;
                        ownSide->disposal[slotIndex].battle.layers[layerIndex].life = 0;
                        ownSide->disposal[slotIndex].battle.layers[layerIndex].state = 0;
                        ownSide->disposal[slotIndex].battle.layers[layerIndex].cardId = -1;
                    }
                }
            }
            if (otherSide->disposal[slotIndex].battle.layers[1].cardId >= 0) {
                if (otherSide->disposal[slotIndex].battle.layers[1].cardId != 0x2C) {
                    otherSide->disposal[slotIndex].battle.power = CardDefinitionsFor(work)[otherSide->disposal[slotIndex].battle.layers[0].cardId].basePower;
                    otherSide->disposal[slotIndex].battle.powerLimit = CardDefinitionsFor(work)[otherSide->disposal[slotIndex].battle.layers[0].cardId].basePower;
                }
                for (layerIndex = 1; layerIndex < 40; layerIndex++) {
                    if (otherSide->disposal[slotIndex].battle.layers[layerIndex].cardId >= 0) {
                        CardSetEffect(opponentSide, work, 10, 0, 0, otherSide->disposal[slotIndex].battle.position, 0);
                        otherSide->junk[opponentJunkCount++] = otherSide->disposal[slotIndex].battle.layers[layerIndex].cardId;
                        otherSide->disposal[slotIndex].battle.layers[layerIndex].life = 0;
                        otherSide->disposal[slotIndex].battle.layers[layerIndex].state = 0;
                        otherSide->disposal[slotIndex].battle.layers[layerIndex].cardId = -1;
                    }
                }
            }
        }
        for (slotIndex = 0; slotIndex < 4; slotIndex++) {
            if (ownSide->battle[slotIndex].battle.layers[1].cardId >= 0) {
                if (ownSide->battle[slotIndex].battle.layers[1].cardId != 0x2C) {
                    ownSide->battle[slotIndex].battle.power = CardDefinitionsFor(work)[ownSide->battle[slotIndex].battle.layers[0].cardId].basePower;
                    ownSide->battle[slotIndex].battle.powerLimit = CardDefinitionsFor(work)[ownSide->battle[slotIndex].battle.layers[0].cardId].basePower;
                }
                for (layerIndex = 1; layerIndex < 40; layerIndex++) {
                    if (ownSide->battle[slotIndex].battle.layers[layerIndex].cardId >= 0) {
                        CardSetEffect(side, work, 10, 0, 0, ownSide->battle[slotIndex].battle.position, 0);
                        ownSide->junk[ownJunkCount++] = ownSide->battle[slotIndex].battle.layers[layerIndex].cardId;
                        ownSide->battle[slotIndex].battle.layers[layerIndex].life = 0;
                        ownSide->battle[slotIndex].battle.layers[layerIndex].state = 0;
                        ownSide->battle[slotIndex].battle.layers[layerIndex].cardId = -1;
                    }
                }
            }
            if (otherSide->battle[slotIndex].battle.layers[1].cardId >= 0) {
                if (otherSide->battle[slotIndex].battle.layers[1].cardId != 0x2C) {
                    otherSide->battle[slotIndex].battle.power = CardDefinitionsFor(work)[otherSide->battle[slotIndex].battle.layers[0].cardId].basePower;
                    otherSide->battle[slotIndex].battle.powerLimit = CardDefinitionsFor(work)[otherSide->battle[slotIndex].battle.layers[0].cardId].basePower;
                }
                for (layerIndex = 1; layerIndex < 40; layerIndex++) {
                    if (otherSide->battle[slotIndex].battle.layers[layerIndex].cardId >= 0) {
                        CardSetEffect(opponentSide, work, 10, 0, 0, otherSide->battle[slotIndex].battle.position, 0);
                        otherSide->junk[opponentJunkCount++] = otherSide->battle[slotIndex].battle.layers[layerIndex].cardId;
                        otherSide->battle[slotIndex].battle.layers[layerIndex].life = 0;
                        otherSide->battle[slotIndex].battle.layers[layerIndex].state = 0;
                        otherSide->battle[slotIndex].battle.layers[layerIndex].cardId = -1;
                    }
                }
            }
        }
        complete = 1;
        xglSoundEffectNormalID(0x2000D, 0);
        break;
    }
    case 43:
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetPermanentMess(work, 3);
            CardCommandAdvance(work);
            break;
        case 1: {
            int count = CardPlayJunkCnt(&ownSide->hand);
            if (pressed & 0x2000) {
                D_00A577C0++;
                xglSoundEffectNormalID(3, 0);
                if (D_00A577C0 >= count)
                    D_00A577C0 = count - 1;
            }
            if (pressed & 0x8000) {
                D_00A577C0--;
                xglSoundEffectNormalID(3, 0);
                if (D_00A577C0 < 0)
                    D_00A577C0 = 0;
            }
            if (pressed & 0x20) {
                int cardId = ownSide->junk[D_00A577C0];
                if (CardDefinitionsFor(work)[cardId].kind == 1 &&
                    (CardDefinitionsFor(work)[cardId].flags & 0x68)) {
                    xglSoundEffectNormalID(1, 0);
                    CardCommandAdvance(work);
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
            } else if (pressed & 0x40) {
                xglSoundEffectNormalID(5, 0);
            }
            CGCPJunkDispSub(&ownSide->hand, D_00A577C0);
            break;
        }
        case 2:
            ownSide->hand.cards[CardPlayHandCnt(&ownSide->hand)] = ownSide->junk[D_00A577C0];
            ownSide->junk[D_00A577C0] = -1;
            CardPlayCleanJunk(&ownSide->hand);
            CardCommandAdvance(work);
            break;
        case 3:
            complete = 1;
            break;
        }
        break;
    case 44:
        switch (CardCommandStep(work)) {
        case 0:
            CardCommandAdvance(work);
            D_00A577C8 = 1;
            CGPSetPermanentMess(work, 3);
            break;
        case 1: {
            int count = CardPlayJunkCnt(&ownSide->hand);
            if (pressed & 0x2000) {
                D_00A577C4++;
                xglSoundEffectNormalID(3, 0);
                if (D_00A577C4 >= count)
                    D_00A577C4 = count - 1;
            }
            if (pressed & 0x8000) {
                D_00A577C4--;
                xglSoundEffectNormalID(3, 0);
                if (D_00A577C4 < 0)
                    D_00A577C4 = 0;
            }
            if (pressed & 0x20) {
                if (CardDefinitionsFor(work)[ownSide->junk[D_00A577C4]].flags & 1) {
                    xglSoundEffectNormalID(1, 0);
                    CardCommandAdvance(work);
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
            } else if (pressed & 0x40) {
                xglSoundEffectNormalID(5, 0);
            }
            CGCPJunkDispSub(&ownSide->hand, D_00A577C4);
            break;
        }
        case 2:
            ownSide->hand.cards[CardPlayHandCnt(&ownSide->hand)] = ownSide->junk[D_00A577C4];
            ownSide->junk[D_00A577C4] = -1;
            CardPlayCleanJunk(&ownSide->hand);
            CardCommandAdvance(work);
            break;
        case 3:
            if (D_00A577C8 != 0) {
                D_00A577C8--;
                CardCommandSetStep(work, 1);
            } else {
                CardCommandSetStep(work, 10);
            }
            break;
        case 10:
            complete = 1;
            break;
        }
        break;
    case 45: {
        int battleZone;
        battleZone = 5;
        if (side != 0)
            battleZone = 4;
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetPermanentMess(work, 8);
            CardCursorPassive(CardCursorFor(work));
            for (i = 0; i < 4; i++) {
                if (ownSide->battle[i].battle.layers[0].cardId >= 0 && CCC107ChkSub(&ownSide->battle[i].battle) == 0) {
                    D_00A577D0.zone = battleZone;
                    D_00A577D0.index = i;
                    CardCursorActive(CardCursorFor(work), &D_00A577D0, 1);
                }
            }
            CardCommandAdvance(work);
            break;
        case 1:
            CGPCursorMove(side, work);
            CGPCursor2Pos(&D_00A577D0, work);
            if (pressed & 0x20) {
                if (D_00A577D0.cardId >= 0 && battleZone == D_00A577D0.zone &&
                    CCC107ChkSub(&ownSide->battle[D_00A577D0.index].battle) == 0) {
                    xglSoundEffectNormalID(1, 0);
                    CardCommandAdvance(work);
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
            } else if (pressed & 0x40) {
                xglSoundEffectNormalID(5, 0);
            }
            break;
        case 2:
            complete = 1;
            CCC26ExecSub(side, work, ownSide, &D_00A577D0);
            break;
        default:
            break;
        }
        break;
    }
    case 46: {
        int junkIndex = CardPlayJunkCnt(&ownSide->hand) - 1;
        int operation;
        for (operation = 0; operation < 4; operation++) {
            if (ownSide->operations[operation] < 0)
                break;
        }
        complete = 1;
        ownSide->operations[operation] = ownSide->junk[junkIndex];
        ownSide->operationDuration[operation] = CardDefinitionsFor(work)[ownSide->junk[junkIndex]].operationDuration;
        ownSide->junk[junkIndex] = -1;
        break;
    }
    case 47: {
        int disposeZone;
        int battleZone;
        disposeZone = 6;
        battleZone = 5;
        if (side != 0) {
            disposeZone = 2;
            battleZone = 4;
        }
        switch (CardCommandStep(work)) {
        case 0: {
            int disposeCount;
            CGPSetPermanentMess(work, 12);
            CardCursorPassive(CardCursorFor(work));
            disposeCount = CardPlayDisposeCnt(&ownSide->hand);
            if (disposeCount + CardPlayBattleCnt(&ownSide->hand) == 0) {
                complete = 1;
            } else {
                for (i = 0; i < 4; i++) {
                    if (ownSide->disposal[i].battle.layers[0].cardId >= 0) {
                        D_00A577D8.zone = disposeZone;
                        D_00A577D8.index = i;
                        CardCursorActive(CardCursorFor(work), &D_00A577D8, 1);
                    }
                }
                for (i = 0; i < 4; i++) {
                    if (ownSide->battle[i].battle.layers[0].cardId >= 0) {
                        D_00A577D8.zone = battleZone;
                        D_00A577D8.index = i;
                        CardCursorActive(CardCursorFor(work), &D_00A577D8, 1);
                    }
                }
            }
            CardCommandAdvance(work);
            break;
        }
        case 1:
            CGPCursorMove(side, work);
            CGPCursor2Pos(&D_00A577D8, work);
            if (pressed & 0x20) {
                if ((disposeZone == D_00A577D8.zone || battleZone == D_00A577D8.zone) &&
                    D_00A577D8.cardId >= 0) {
                    CardCursorActive(CardCursorFor(work), &D_00A577D8, 2);
                    CardCommandAdvance(work);
                    xglSoundEffectNormalID(1, 0);
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
            } else if (pressed & 0x40) {
                complete = 1;
                CardCursorPassive(CardCursorFor(work));
                xglSoundEffectNormalID(2, 0);
            }
            break;
        case 2:
            CGPDispErrorMess(side, 0x19);
            if (pressed & 0x20) {
                xglSoundEffectNormalID(1, 0);
                CardCommandAdvance(work);
            } else if (pressed & 0x40) {
                xglSoundEffectNormalID(2, 0);
                CardCommandSetStep(work, 0);
            }
            break;
        case 3:
            if (disposeZone == D_00A577D8.zone)
                ownSide->disposal[D_00A577D8.index].battle.layers[0].life = 0;
            else
                ownSide->battle[D_00A577D8.index].battle.layers[0].life = 0;
            CardCommandAdvance(work);
            D_00A577E0 = 4;
            xglSoundEffectNormalID(0x2000D, 0);
            break;
        case 4: {
            int remainingRecovery;
            CardPlayRecavery(&ownSide->hand);
            CardSetEffect(side, work, 10, 0, 0, 0, 0);
            CardSetEffect(side, work, 12, 1, 0, 0, 0);
            remainingRecovery = D_00A577E0 - 1;
            CardCommandAdvance(work);
            D_00A577E0 = remainingRecovery;
            break;
        }
        case 5:
            if (D_00A577E0 > 0)
                CardCommandRewind(work);
            else
                CardCommandSetStep(work, 0);
            break;
        }
        break;
    }
    case 48: {
        for (damageIndex = 0; damageIndex < 4; damageIndex++) {
            if (ownSide->disposal[damageIndex].battle.layers[0].cardId >= 0) {
                ownSide->disposal[damageIndex].battle.layers[0].life--;
                CardSetEffect(side, work, 10, 0, 0, ownSide->disposal[damageIndex].battle.position, 0);
                CardSetEffect(side, work, 13, -1, 0, ownSide->disposal[damageIndex].battle.position,
                              &ownSide->disposal[damageIndex].battle);
                if (ownSide->disposal[damageIndex].battle.layers[0].life < 0)
                    ownSide->disposal[damageIndex].battle.layers[0].life = 0;
            }
            if (otherSide->disposal[damageIndex].battle.layers[0].cardId >= 0) {
                otherSide->disposal[damageIndex].battle.layers[0].life--;
                CardSetEffect(opponentSide, work, 10, 0, 0, otherSide->disposal[damageIndex].battle.position, 0);
                CardSetEffect(opponentSide, work, 13, -1, 0, otherSide->disposal[damageIndex].battle.position,
                              &otherSide->disposal[damageIndex].battle);
                if (otherSide->disposal[damageIndex].battle.layers[0].life < 0)
                    otherSide->disposal[damageIndex].battle.layers[0].life = 0;
            }
        }
        for (damageIndex = 0; damageIndex < 4; damageIndex++) {
            if (ownSide->battle[damageIndex].battle.layers[0].cardId >= 0) {
                ownSide->battle[damageIndex].battle.layers[0].life--;
                CardSetEffect(side, work, 10, 0, 0, ownSide->battle[damageIndex].battle.position, 0);
                CardSetEffect(side, work, 13, -1, 0, ownSide->battle[damageIndex].battle.position,
                              &ownSide->battle[damageIndex].battle);
                if (ownSide->battle[damageIndex].battle.layers[0].life < 0)
                    ownSide->battle[damageIndex].battle.layers[0].life = 0;
            }
            if (otherSide->battle[damageIndex].battle.layers[0].cardId >= 0) {
                otherSide->battle[damageIndex].battle.layers[0].life--;
                CardSetEffect(opponentSide, work, 10, 0, 0, otherSide->battle[damageIndex].battle.position, 0);
                CardSetEffect(opponentSide, work, 13, -1, 0, otherSide->battle[damageIndex].battle.position,
                              &otherSide->battle[damageIndex].battle);
                if (otherSide->battle[damageIndex].battle.layers[0].life < 0)
                    otherSide->battle[damageIndex].battle.layers[0].life = 0;
            }
        }
        complete = 1;
        xglSoundEffectNormalID(0x20008, 0);
        break;
    }
    case 49: {
        int i;
        int disposeZone;
        int battleZone;
        disposeZone = 2;
        battleZone = 4;
        if (side != 0) {
            disposeZone = 6;
            battleZone = 5;
        }
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetPermanentMess(work, 10);
            CardCursorPassive(CardCursorFor(work));
            for (i = 0; i < 4; i++) {
                if (otherSide->disposal[i].battle.layers[0].cardId >= 0) {
                    D_00A577E8.zone = disposeZone;
                    D_00A577E8.index = i;
                    CardCursorActive(CardCursorFor(work), &D_00A577E8, 1);
                }
            }
            for (i = 0; i < 4; i++) {
                if (otherSide->battle[i].battle.layers[0].cardId >= 0) {
                    D_00A577E8.zone = battleZone;
                    D_00A577E8.index = i;
                    CardCursorActive(CardCursorFor(work), &D_00A577E8, 1);
                }
            }
            CardCommandAdvance(work);
            break;
        case 1:
            CGPCursorMove(side, work);
            CGPCursor2Pos(&D_00A577E8, work);
            if (pressed & 0x20) {
                if ((disposeZone == D_00A577E8.zone || battleZone == D_00A577E8.zone) && D_00A577E8.cardId >= 0) {
                    CardCommandAdvance(work);
                    xglSoundEffectNormalID(1, 0);
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
            } else if (pressed & 0x40) {
                xglSoundEffectNormalID(5, 0);
            }
            break;
        case 2:
            complete = 1;
            CCC30ExecSub(opponentSide, work,
                         CardSelectBattle(otherSide, D_00A577E8.index,
                                          disposeZone == D_00A577E8.zone));
            CardCursorPassive(CardCursorFor(work));
            break;
        }
        break;
    }
    case 53: {
        int i;
        for (i = 2; i < 4; i++) {
            if (otherSide->battle[i].battle.layers[0].cardId >= 0) {
                CardSetEffect(opponentSide, work, 10, 0, 0x2000D,
                              otherSide->battle[i].battle.position, 0);
                otherSide->battle[i].battle.flags |= 2;
            }
        }
        complete = 1;
        break;
    }
    case 56: {
        int battleZone;
        battleZone = 5;
        if (side != 0)
            battleZone = 4;
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetPermanentMess(work, 6);
            CardCommandAdvance(work);
        case 1:
            CardCursorPassive(CardCursorFor(work));
            for (i = 0; i < 4; i++) {
                const s16 *commandId = &ownSide->battle[i].battle.layers[0].cardId;
                int cardId = (s16) *commandId;
                if (cardId >= 0 && (CardDefinitionsFor(work)[cardId].flags & 1)) {
                    D_00A577F8.zone = battleZone;
                    D_00A577F8.index = i;
                    CardCursorActive(CardCursorFor(work), &D_00A577F8, 1);
                }
            }
            CardCommandAdvance(work);
            break;
        case 2:
            CGPCursorMove(side, work);
            CGPCursor2Pos(&D_00A577F8, work);
            if (pressed & 0x20) {
                if (battleZone == D_00A577F8.zone && D_00A577F8.cardId >= 0 &&
                    CardDefinitionsFor(work)[D_00A577F8.index].kind == 1 &&
                    CardDefinitionsFor(work)[D_00A577F8.cardId].flags == 1) {
                    CardCursorActive(CardCursorFor(work), &D_00A577F8, 2);
                    CardCommandAdvance(work);
                    xglSoundEffectNormalID(1, 0);
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
            } else if (pressed & 0x40) {
                xglSoundEffectNormalID(5, 0);
            }
            break;
        case 3:
            CGPDispErrorMess(side, 0x19);
            if (pressed & 0x20) {
                u16 nextStep;
                xglSoundEffectNormalID(1, 0);
                nextStep = work->command.step;
                nextStep++;
                D_00A577F0 = 0;
                CardCommandSetStep(work, nextStep);
            }
            if (pressed & 0x40) {
                CardCursorPassive(CardCursorFor(work));
                xglSoundEffectNormalID(2, 0);
                CardCommandSetStep(work, 1);
            }
            break;
        case 4: {
            int junkCount = CardPlayJunkCnt(&ownSide->hand);
            if (repeated & 0x2000) {
                if (D_00A577F0 < junkCount - 1) {
                    xglSoundEffectNormalID(3, 0);
                    D_00A577F0++;
                }
            } else if ((repeated & 0x8000) && D_00A577F0 > 0) {
                xglSoundEffectNormalID(3, 0);
                D_00A577F0--;
            }
            if (pressed & 0x20) {
                int cardId = ownSide->junk[D_00A577F0];
                if ((CardDefinitionsFor(work)[cardId].flags & 1) &&
                    CardPlayIdentityCheck(work, &ownSide->hand,
                                          CardDefinitionsFor(work)[cardId].identity) == 0) {
                    xglSoundEffectNormalID(1, 0);
                    CardCommandAdvance(work);
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
            } else if (pressed & 0x40) {
                xglSoundEffectNormalID(2, 0);
                CardCommandSetStep(work, 1);
            }
            CGCPJunkDispSub(&ownSide->hand, D_00A577F0);
            break;
        }
        case 5:
            CGPDispErrorMess(side, 0x18);
            if (pressed & 0x20) {
                xglSoundEffectNormalID(1, 0);
                CardCommandAdvance(work);
            } else if (pressed & 0x40) {
                xglSoundEffectNormalID(2, 0);
                CardCommandSetStep(work, 4);
            }
            CGCPJunkDispSub(&ownSide->hand, D_00A577F0);
            break;
        case 6:
            complete = 1;
            CardPlayDesertBattle(&ownSide->hand, &ownSide->battle[D_00A577F8.index].battle);
            ownSide->battle[D_00A577F8.index].battle.layers[0].cardId = ownSide->junk[D_00A577F0];
            ownSide->junk[D_00A577F0] = -1;
            CardPlayCleanJunk(&ownSide->hand);
            CardCursorPassive(CardCursorFor(work));
            break;
        }
        break;
    }
    case 17:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = 18;
            queued[CARD_QUEUE_PHASE] = 5;
            if (side != 0)
                queued[CARD_QUEUE_PHASE] = 12;
        }
        complete = 1;
        break;
    case 18: {
        int count = CardPlayHandCnt(&otherSide->hand);
        int step = CardCommandStep(work);
        switch (step) {
        case 0:
            if (count != 0) {
                unsigned int nextStep;
                int *selectedCard;
                CGPSetErrorMessPlus(opponentSide, work, 0x24, 0x87);
                selectedCard = &D_00A57800;
                nextStep = (u16) work->command.step;
                *selectedCard = 0;
                nextStep++;
                CardCommandSetStep(work, nextStep);
            } else {
                complete = 1;
            }
            break;
        case 1:
            if (opponentControlledByAi) {
                CC_Kara_EndFase(opponentSide, work);
                complete = 1;
                break;
            }
            if (opponentRepeated & 0x8000) {
                if (D_00A57800 > 0) {
                    xglSoundEffectNormalID(3, 0);
                    D_00A57800--;
                }
            } else if (opponentRepeated & 0x2000) {
                if (D_00A57800 < count - 1) {
                    xglSoundEffectNormalID(3, 0);
                    D_00A57800++;
                }
            }
            if (opponentRepeated & 0x20) {
                CardCommandAdvance(work);
                xglSoundEffectNormalID(1, 0);
            }
            xglFontPrint(143, 213, 0xFFF8, D_00A4D7C8);
            scale[0] = 1.5f;
            scale[1] = scale[2] = scale[3] = 1.0f;
            translation[0] = translation[1] = 0.0f;
            translation[2] = 0.91f;
            translation[3] = 1.0f;
            xglMatrixUnit(matrix);
            xglMatrixTrans(matrix, (const float (*)[4])matrix, translation);
            xglMatrixScale(matrix, (const float (*)[4])matrix, scale);
            nmlModelSetPlace(matrix);
            nmlModelSetTexture((void *) 0x01DB5800);
            nmlModelEntryCard((void *) 0x01DAAC00);
            if ((side != 0 && work->rotStageId == 0) ||
                (side == 0 && work->rotStageId != 0)) {
                CGCPHandDispSub2(CardDefinitionsFor(work), &otherSide->hand, D_00A57800, 0);
            } else {
                CGCPHandDispSub3(CardDefinitionsFor(work), &otherSide->hand, D_00A57800, 0, 0);
            }
            break;
        case 2:
            CGPDispErrorMessPlus(opponentSide, 0x19, otherSide->hand.cards[D_00A57800]);
            if (opponentRepeated & 0x20) {
                xglSoundEffectNormalID(1, 0);
                CardCommandAdvance(work);
                CardPlayDesertCard(&otherSide->hand, D_00A57800);
            } else if (opponentRepeated & 0x40) {
                xglSoundEffectNormalID(1, 0);
                CardCommandRewind(work);
            }
            break;
        case 3:
            CardCommandAdvance(work);
            break;
        case 4:
            complete = 1;
            break;
        }
        break;
    }
    case 13:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = 14;
            queued[CARD_QUEUE_PHASE] = 5;
            if (side != 0)
                queued[CARD_QUEUE_PHASE] = 12;
        }
        complete = 1;
        break;
    case 14: {
        int i;
        int battleZone;
        if ((side == 0 && CardLowByteAiFlag(flags, 0x40)) ||
            (side != 0 && CardLowByteAiFlag(flags, 0x20)))
            return CardEnemyAnswerCardxx(side, work, 0x3A);
        battleZone = 4;
        if (side != 0)
            battleZone = 5;
        switch (CardCommandStep(work)) {
        case 0:
            CardCursorPassive(CardCursorFor(work));
            if (CardPlayBattleCnt(&otherSide->hand) != 0) {
                CGPSetErrorMessPlus(side, work, 0x24, 0x3A);
                position.zone = battleZone;
                for (i = 0; i < 4; i++) {
                    if (otherSide->battle[i].battle.layers[0].cardId >= 0) {
                        position.index = i;
                        CardCursorActive(CardCursorFor(work), &position, 1);
                    }
                }
                CardCommandAdvance(work);
            } else {
                complete = 1;
            }
            break;
        case 1:
            CGPSetPermanentMess(work, 10);
            CGPCursorMove(side, work);
            CGPCursor2Pos(&D_00A57808, work);
            if (pressed & 0x20) {
                if (battleZone == D_00A57808.zone && D_00A57808.cardId >= 0) {
                    CardCommandAdvance(work);
                    xglSoundEffectNormalID(1, 0);
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
            }
            break;
        case 2:
            CGPDispErrorMess(side, 0x18);
            if (pressed & 0x20) {
                otherSide->battle[D_00A5780A].battle.flags |= 2;
                xglSoundEffectNormalID(1, 0);
                CardCommandAdvance(work);
            } else if (pressed & 0x40) {
                xglSoundEffectNormalID(2, 0);
                CardCommandRewind(work);
            }
            break;
        case 3:
            complete = 1;
            break;
        }
        break;
    }
    case 15:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = 16;
            queued[CARD_QUEUE_PHASE] = 5;
            if (side != 0)
                queued[CARD_QUEUE_PHASE] = 12;
        }
        complete = 1;
        break;
    case 16: {
        int i;
        int targetZone;
        if ((side == 0 && CardLowByteAiFlag(flags, 0x40)) ||
            (side != 0 && CardLowByteAiFlag(flags, 0x20)))
            return CardEnemyAnswerCardxx(side, work, 0x3E);
        targetZone = 4;
        if (side != 0)
            targetZone = 5;
        switch (CardCommandStep(work)) {
        case 0:
            CardCursorPassive(CardCursorFor(work));
            if (CardPlayBattleCnt(&otherSide->hand) == 0) {
                complete = 1;
            } else if (CardPlayDisposeCnt(&otherSide->hand) == 4) {
                complete = 1;
            } else {
                CGPSetErrorMessPlus(side, work, 0x24, 0x3E);
                position.zone = targetZone;
                for (i = 0; i < 4; i++) {
                    if (otherSide->battle[i].battle.layers[0].cardId >= 0) {
                        position.index = i;
                        CardCursorActive(CardCursorFor(work), &position, 1);
                    }
                }
                CardCommandAdvance(work);
            }
            break;
        case 1:
            CGPSetPermanentMess(work, 5);
            CGPCursorMove(side, work);
            CGPCursor2Pos(&D_00A57810, work);
            if (pressed & 0x20) {
                if (targetZone == D_00A57810.zone && D_00A57810.cardId >= 0) {
                    CardCommandAdvance(work);
                    xglSoundEffectNormalID(1, 0);
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
            }
            break;
        case 2:
            CGPDispErrorMess(side, 0x18);
            if (pressed & 0x20) {
                CCC07ExecSub(work, &otherSide->hand, &D_00A57810);
                xglSoundEffectNormalID(1, 0);
                CardCommandAdvance(work);
            } else if (pressed & 0x40) {
                xglSoundEffectNormalID(2, 0);
                CardCommandRewind(work);
            }
            break;
        case 3:
            complete = 1;
            break;
        }
        break;
    }
    case 60:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = 61;
            queued[CARD_QUEUE_PHASE] = 5;
            if (side != 0)
                queued[CARD_QUEUE_PHASE] = 12;
        }
        complete = 1;
        break;
    case 61: {
        int i;
        int targetZone;
        if ((side == 0 && CardLowByteAiFlag(flags, 0x40)) ||
            (side != 0 && CardLowByteAiFlag(flags, 0x20)))
            return CardEnemyAnswerCardxx(side, work, 0x3D);
        targetZone = 2;
        if (side != 0)
            targetZone = 6;
        switch (CardCommandStep(work)) {
        case 0:
            CardCursorPassive(CardCursorFor(work));
            if (CardPlayDisposeCnt(&otherSide->hand) != 0) {
                CGPSetErrorMessPlus(side, work, 0x24, 0x3D);
                position.zone = targetZone;
                for (i = 0; i < 4; i++) {
                    if (otherSide->disposal[i].battle.layers[0].cardId >= 0) {
                        position.index = i;
                        CardCursorActive(CardCursorFor(work), &position, 1);
                    }
                }
                CardCommandAdvance(work);
            } else {
                complete = 1;
            }
            break;
        case 1:
            CGPSetPermanentMess(work, 10);
            CGPCursorMove(side, work);
            CGPCursor2Pos(&D_00A57818, work);
            if (pressed & 0x20) {
                if (targetZone == D_00A57818.zone && D_00A57818.cardId >= 0) {
                    CardCommandAdvance(work);
                    xglSoundEffectNormalID(1, 0);
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
            }
            break;
        case 2:
            CGPDispErrorMess(side, 0x18);
            if (pressed & 0x20) {
                otherSide->disposal[D_00A5781A].battle.flags |= 2;
                xglSoundEffectNormalID(1, 0);
                CardCommandAdvance(work);
            } else if (pressed & 0x40) {
                xglSoundEffectNormalID(2, 0);
                CardCommandRewind(work);
            }
            break;
        case 3:
            complete = 1;
            break;
        }
        break;
    }
    case 11:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = 12;
            queued[CARD_QUEUE_PHASE] = 5;
            if (side != 0)
                queued[CARD_QUEUE_PHASE] = 12;
        }
        complete = 1;
        break;
    case 12: {
        int i;
        int battleZone;
        if ((side == 0 && CardLowByteAiFlag(flags, 0x40)) ||
            (side != 0 && CardLowByteAiFlag(flags, 0x20)))
            return CardEnemyAnswerCardxx(side, work, 0x37);
        switch (CardCommandStep(work)) {
        case 0:
            if (CardPlayOperationCnt(&otherSide->hand) != 0) {
                CGPSetErrorMessPlus(side, work, 0x24, 0x37);
                CardCursorPassive(CardCursorFor(work));
                position.zone = 3;
                if (side != 0)
                    position.zone = 7;
                for (i = 0; i < 4; i++) {
                    if (otherSide->operations[i] >= 0) {
                        position.index = i;
                        CardCursorActive(CardCursorFor(work), &position, 1);
                    }
                }
                CGPSetPermanentMess(work, 12);
                CardCommandAdvance(work);
            } else {
                complete = 1;
            }
            break;
        case 1:
            battleZone = 3;
            if (side != 0)
                battleZone = 7;
            CGPCursorMove(side, work);
            CGPCursor2Pos(&D_00A57820, work);
            if (pressed & 0x20) {
                if (battleZone == D_00A57820.zone && D_00A57820.cardId >= 0) {
                    CardCommandAdvance(work);
                    xglSoundEffectNormalID(1, 0);
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
            }
            break;
        case 2:
            CGPDispErrorMess(side, 0x18);
            if (pressed & 0x20) {
                CardCommandAdvance(work);
                CCU055ExecSub(&otherSide->hand, &D_00A57820);
                xglSoundEffectNormalID(1, 0);
            } else if (pressed & 0x40) {
                xglSoundEffectNormalID(2, 0);
                CardCommandRewind(work);
            }
            break;
        case 3:
            complete = 1;
            break;
        }
        break;
    }
    case 62:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = 63;
            queued[CARD_QUEUE_PHASE] = 5;
            if (side != 0)
                queued[CARD_QUEUE_PHASE] = 12;
        }
        complete = 1;
        break;
    case 63: {
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetErrorMessPlus(side, work, 0x24, 0x1D);
            CardCommandAdvance(work);
            break;
        case 1:
            CardCommandAdvance(work);
            break;
        case 2: {
            for (i = 0; i < 4; i++) {
                if (work->player.battle[i].battle.layers[0].cardId >= 0)
                    work->player.battle[i].battle.flags |= 2;
                if (work->enemy.battle[i].battle.layers[0].cardId >= 0)
                    work->enemy.battle[i].battle.flags |= 2;
            }
            for (i = 0; i < 4; i++) {
                if (work->player.disposal[i].battle.layers[0].cardId >= 0)
                    work->player.disposal[i].battle.flags |= 2;
                if (work->enemy.disposal[i].battle.layers[0].cardId >= 0)
                    work->enemy.disposal[i].battle.flags |= 2;
            }
            CardCommandAdvance(work);
            break;
        }
        case 3:
            complete = 1;
            break;
        }
        break;
    }
    case 64:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = 65;
            queued[CARD_QUEUE_PHASE] = 5;
            if (side != 0)
                queued[CARD_QUEUE_PHASE] = 12;
        }
        complete = 1;
        break;
    case 65: {
        int i;
        switch (CardCommandStep(work)) {
        case 0: {
            int found = 0;
            for (i = 0; i < 4; i++) {
                s16 *commandId = &ownSide->battle[i].battle.layers[0].cardId;
                s16 *otherCommandId = &otherSide->battle[i].battle.layers[0].cardId;
                int cardId = (s16) *commandId;
                if (cardId >= 0 && (CardDefinitionsFor(work)[cardId].flags & 2)) {
                    found = 1;
                    break;
                }
                cardId = (s16) *otherCommandId;
                if (cardId >= 0 && (CardDefinitionsFor(work)[cardId].flags & 2)) {
                    found = 1;
                    break;
                }
            }
            if (found) {
                CGPSetErrorMessPlus(side, work, 0x24, 0x39);
                CardCommandAdvance(work);
            } else {
                complete = 1;
            }
            break;
        }
        case 1:
            CardCommandAdvance(work);
            break;
        case 2:
        {
            for (i = 0; i < 4; i++) {
                int cardId = work->player.battle[i].battle.layers[0].cardId;
                if (cardId >= 0 && (CardDefinitionsFor(work)[cardId].flags & 2)) {
                    CardPlayReturnCard(&playerSide->hand,
                                       &work->player.battle[0].battle, i);
                }
                {
                    s16 *otherCommandId = &work->enemy.battle[i].battle.layers[0].cardId;
                    cardId = (s16) *otherCommandId;
                    if (cardId >= 0 && (CardDefinitionsFor(work)[cardId].flags & 2)) {
                        CardPlayReturnCard(&enemySide->hand,
                                           &work->enemy.battle[0].battle, i);
                    }
                }
            }
            CardCommandAdvance(work);
            break;
        }
        case 3:
            complete = 1;
            break;
        }
        break;
    }
    case 9: {
        int junkIndex = CardPlayJunkCnt(&ownSide->hand) - 1;
        if (junkIndex >= 0) {
            int cardId = ownSide->junk[junkIndex];
            if (CardDefinitionsFor(work)[cardId].kind == 1 &&
                (CardDefinitionsFor(work)[cardId].flags & 11)) {
                queued = CPCSearchCommWk(side, work);
                if (queued != 0) {
                    queued[CARD_QUEUE_COMMAND] = 10;
                    queued[CARD_QUEUE_PHASE] = 13;
                    if (side != 0)
                        queued[CARD_QUEUE_PHASE] = 6;
                    queued[CARD_QUEUE_OPERAND] = operand;
                    queued[CARD_QUEUE_TARGET] = junkIndex;
                }
            }
        }
        complete = 1;
        break;
    }
    case 10:
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetErrorMessPlus(opponentSide, work, 0x24, 0x3C);
            CardCommandAdvance(work);
            break;
        case 1: {
            u16 restoredCardId;
            otherSide->battle[operand].battle.flags = 0;
            restoredCardId = otherSide->junk[target];
            otherSide->battle[operand].battle.layers[0].life = 1;
            otherSide->battle[operand].battle.layers[0].cardId = restoredCardId;
            otherSide->junk[target] = -1;
            complete = 1;
            CardPlayCleanJunk(&otherSide->hand);
            break;
        }
        }
        break;
    case 68:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = 8;
            queued[CARD_QUEUE_PHASE] = CardCommandPhase(work);
            queued[CARD_QUEUE_OPERAND] = -1;
            queued[CARD_QUEUE_TARGET] = -1;
        }
        complete = 1;
        break;
    case 8: {
        int ownDisposalZone;
        if ((opponentSide == 0 && CardLowByteAiFlag(flags, 0x40)) ||
            (opponentSide != 0 && CardLowByteAiFlag(flags, 0x20)))
            return CardEnemyAnswerCardxx(opponentSide, work, 0x3D);
        ownDisposalZone = 6;
        if (side != 0)
            ownDisposalZone = 2;
        switch (CardCommandStep(work)) {
        case 0:
            if (CardPlayDisposeCnt(&ownSide->hand) != 0) {
                CGPSetErrorMessPlus(opponentSide, work, 0x24, 0x26);
                CardCursorPassive(CardCursorFor(work));
                CGPSetPermanentMess(work, 12);
                CardCursorSetSelectionMode(work, 1);
                D_00A57828.zone = ownDisposalZone;
                for (i = 0; i < 4; i++) {
                    if (ownSide->disposal[i].battle.layers[0].cardId > 0) {
                        D_00A57828.index = i;
                        CardCursorActive(CardCursorFor(work), &D_00A57828, 1);
                    }
                }
                CardCommandAdvance(work);
            } else {
                complete = 1;
            }
            break;
        case 1:
            CardCursorSetSide(work, opponentSide);
            CGPCursorMove(opponentSide, work);
            CGPCursor2Pos(&D_00A57828, work);
            if (opponentPressed & 0x20) {
                if (ownDisposalZone == D_00A57828.zone && D_00A57828.cardId >= 0) {
                    CardCommandAdvance(work);
                    xglSoundEffectNormalID(1, 0);
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
            } else if (opponentPressed & 0x80) {
                xglSoundEffectNormalID(2, 0);
                CardCursorSetSide(work, -1);
                complete = 1;
            }
            break;
        case 2:
            CGPDispErrorMess(opponentSide, 0x18);
            if (opponentPressed & 0x20) {
                CardCommandAdvance(work);
                CCU038ExecSub(&ownSide->hand, &D_00A57828);
                xglSoundEffectNormalID(1, 0);
                CardCursorSetSelectionMode(work, 0);
            } else if (opponentPressed & 0x40) {
                xglSoundEffectNormalID(2, 0);
                CardCommandRewind(work);
            }
            break;
        case 3:
            complete = 1;
            CardCursorSetSide(work, -1);
            break;
        }
        break;
    }
    case 4:
        switch (CardCommandStep(work)) {
        case 0:
            CardCommandAdvance(work);
            break;
        case 1:
            if (controlledByAi) {
                CardEnemyAnswerSionSearch(side, work);
                complete = 1;
                CGPSetErrorMessPlus(-1, work, 0x24, 0);
            } else {
                work->flags = flags | 2;
                CGPDispErrorMessPlus(side, 0x17, 0);
                if (pressed & 0x20) {
                    u16 nextStep;
                    CGPSetPermanentMess(work, 1);
                    nextStep = work->command.step;
                    nextStep++;
                    D_00A57830 = 0;
                    CardCommandSetStep(work, nextStep);
                }
                if (pressed & 0x40)
                    CardCommandSetStep(work, 10);
            }
            break;
        case 2: {
            int count = CardPlayYamaCnt(&ownSide->hand);
            if (repeated & 0x2000) {
                D_00A57830++;
                xglSoundEffectNormalID(3, 0);
                if (D_00A57830 >= count)
                    D_00A57830 = count - 1;
            }
            if (repeated & 0x8000) {
                D_00A57830--;
                xglSoundEffectNormalID(3, 0);
                if (D_00A57830 < 0)
                    D_00A57830 = 0;
            }
            if (pressed & 0x20) {
                if (ownSide->deck[D_00A57830] == 7) {
                    xglSoundEffectNormalID(1, 0);
                    CardCommandAdvance(work);
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
            } else if (pressed & 0x40) {
                xglSoundEffectNormalID(2, 0);
                CardCommandSetStep(work, 10);
            }
            CGCPYamaDispSub(&ownSide->hand, D_00A57830, count, held, 0);
            break;
        }
        case 3:
            ownSide->hand.cards[CardPlayHandCnt(&ownSide->hand)] = ownSide->deck[D_00A57830];
            ownSide->deck[D_00A57830] = -1;
            CardPlayCleanYama(&ownSide->hand);
            CardPlayShuffleYama(&ownSide->hand);
            CardCommandAdvance(work);
            break;
        case 4:
        case 10:
            complete = 1;
            work->flags = flags & ~2;
            break;
        }
        break;
    case 66:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = 67;
            queued[CARD_QUEUE_PHASE] = CardCommandPhase(work);
            queued[CARD_QUEUE_OPERAND] = operand;
            queued[CARD_QUEUE_TARGET] = -1;
        }
        complete = 1;
        break;
    case 67:
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetErrorMessPlus(operand, work, 0x24, 0x16);
            CardCommandAdvance(work);
            break;
        case 1:
            complete = 1;
            CardPlayCostCard(&otherSide->hand);
            break;
        }
        break;
    case 69:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = 70;
            queued[CARD_QUEUE_PHASE] = CardCommandPhase(work);
            queued[CARD_QUEUE_OPERAND] = operand;
            queued[CARD_QUEUE_TARGET] = -1;
        }
        complete = 1;
        break;
    case 70:
        switch (CardCommandStep(work)) {
        case 0: {
            u16 nextStep;
            s8 *timer;
            CGPSetErrorMessPlus(opponentSide, work, 0x24, 0x3B);
            timer = &D_00A57834;
            nextStep = work->command.step;
            nextStep++;
            *timer = 20;
            CardCommandSetStep(work, nextStep);
            break;
        }
        case 1:
            CardCommandAdvance(work);
            break;
        case 2:
            ownSide->battle[operand].battle.layers[0].life = 0;
            xglSoundEffectNormalID(0x20008, 0);
            CardCommandAdvance(work);
            break;
        case 3:
            if (D_00A57834 != 0) {
                D_00A57834--;
            } else {
                u16 nextStep;
                CardPlayCostCard(&ownSide->hand);
                CardPlayCostCard(&ownSide->hand);
                CardPlayCostCard(&ownSide->hand);
                CardPlayCostCard(&ownSide->hand);
                CardSetEffect(opponentSide, work, 5, 0, 0x20008, 0, 0);
                CardSetEffect(opponentSide, work, 13, 4, 0, 0, 0);
                nextStep = work->command.step;
                nextStep++;
                D_00A57834 = 15;
                CardCommandSetStep(work, nextStep);
            }
            break;
        case 4:
            if (D_00A57834 != 0)
                D_00A57834--;
            else
                complete = 1;
            break;
        }
        break;
    case 71:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = 72;
            queued[CARD_QUEUE_PHASE] = CardCommandPhase(work);
            queued[CARD_QUEUE_OPERAND] = operand;
            queued[CARD_QUEUE_TARGET] = -1;
        }
        complete = 1;
        break;
    case 72:
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetErrorMessPlus(opponentSide, work, 0x24, 0x51);
            CardCommandAdvance(work);
            break;
        case 1:
            complete = 1;
            break;
        }
        break;
    case 73:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = 74;
            queued[CARD_QUEUE_PHASE] = CardCommandPhase(work);
            queued[CARD_QUEUE_OPERAND] = operand;
            queued[CARD_QUEUE_TARGET] = -1;
        }
        complete = 1;
        break;
    case 74: {
        int victimIndex;
        rotated = (side == 0 && work->rotStageId == 0) ||
                  (side != 0 && work->rotStageId != 0);
        for (victimIndex = 0; victimIndex < 4; victimIndex++) {
            if (otherSide->disposal[victimIndex].battle.layers[0].cardId >= 0 &&
                otherSide->disposal[victimIndex].battle.layers[1].cardId != 0x2C &&
                otherSide->disposal[victimIndex].battle.layers[0].cardId != 0x2F) {
                otherSide->disposal[victimIndex].battle.layers[0].damageTimer = 25;
                otherSide->disposal[victimIndex].battle.layers[0].life--;
                attackerRecord = (void *) &((CardBoardSlot *)(void *) ownSide)[operand];
                CardSetEffect(side, work, 4, 0, 0, attackerRecord->battle[0].battle.position,
                              &otherSide->disposal[victimIndex].battle);
                if (rotated) {
                    CardSetEffect(side, work, 14, 1, 0, otherSide->disposal[victimIndex].battle.position,
                                  &otherSide->disposal[victimIndex].battle);
                } else {
                    CardSetEffect(side, work, 11, 1, 0, otherSide->disposal[victimIndex].battle.position,
                                  &otherSide->disposal[victimIndex].battle);
                }
            }
        }
        for (victimIndex = 0; victimIndex < 4; victimIndex++) {
            if (otherSide->battle[victimIndex].battle.layers[0].cardId >= 0 &&
                otherSide->disposal[victimIndex].battle.layers[1].cardId != 0x2C &&
                otherSide->disposal[victimIndex].battle.layers[0].cardId != 0x2F) {
                CardLayerStack *card = &otherSide->battle[victimIndex].battle;
                otherSide->battle[victimIndex].battle.layers[0].damageTimer = 25;
                otherSide->battle[victimIndex].battle.layers[0].life--;
                CardSetEffect(side, work, 4, 0, 0, ownSide->battle[operand].battle.position, card);
                CardSetEffect(side, work, 11, 1, 0, otherSide->battle[victimIndex].battle.position, card);
            }
        }
        complete = 1;
        xglSoundEffectNormalID(0x20008, 0);
        break;
    }
    case 75:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = 76;
            queued[CARD_QUEUE_PHASE] = CardCommandPhase(work);
            queued[CARD_QUEUE_OPERAND] = operand;
            queued[CARD_QUEUE_TARGET] = -1;
        }
        complete = 1;
        break;
    case 76: {
        CardHand *drawHand = &playerSide->hand;
        if (operand != 0)
            drawHand = &enemySide->hand;
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetErrorMessPlus(operand, work, 0x24, 0x81);
            CardCommandAdvance(work);
            break;
        case 1:
            CardPlayDrawCard(drawHand);
            complete = 1;
            break;
        }
        break;
    }
    case 0:
        complete = 1;
        break;
    case 5:
    case 6:
    case 19:
    case 54:
    default:
        complete = 1;
        break;
    }
    return complete;
}

void CardPlayCommandBattleExecute(s16 side, s16 commandId, CardGameWork *work) {
    s16 *entry;
    int checked;

    entry = work->commandQueue[1][0];
    if (side == 0)
        entry = work->commandQueue[0][0];
    checked = 0;
    do {
        if (entry[0] == commandId) {
            work->command.commandIndex = 1;
            if (CardPlayCommandPlay((u16)side, (u16)entry[1], work, entry[2], entry[3]) != 0) {
                CGPSetPermanentMess(work, 0);
                work->command.commandIndex = 0;
                work->command.step = 0;
                entry[0] = -1;
                entry[2] = -1;
                entry[3] = -1;
            }
            return;
        }
        entry += 4;
        checked++;
    } while (checked < 40);
}

#define CARD_QUEUE_SLOTS 40            /* slti a1,a2,40 at 0x00a19910 */
#define CARD_QUEUE_BASE_OFFSET 12      /* addiu a3,s1,12 at 0x00a198d4 */
#define CARD_QUEUE_PLAYER_OFFSET 464   /* addiu v0,v0,464 at 0x00a198fc */
#define CARD_QUEUE_ENEMY_OFFSET 784    /* addiu v1,v0,784 at 0x00a198f8 */

int CardPlayCommandExecute(s16 commandId, CardGameWork *work) {
    int i;
    s16 *own;
    s16 *foe;
    int ownOffset;
    int foeOffset;

    for (i = 0; i < CARD_QUEUE_SLOTS; i++) {
        /* The phase words are addressed as bytes from work + 12: the player's queue
         * entry i (commandQueue[0][i]) lies at base + 464 + 8 * i and the enemy's
         * (commandQueue[1][i]) at base + 784 + 8 * i, both from one scaled index.
         * The enemy address is formed after the first test so that the scan's
         * scheduling matches. Tested alternatives (matching rows of 77): the
         * commandQueue[side][i][...] members 42, one walking pointer per side 67,
         * the enemy address formed before the first test 70; a sub-structure
         * view at +0xC loses the base arithmetic (cc1 canonicalises it). */
        u8 *base = (u8 *)work + CARD_QUEUE_BASE_OFFSET;
        ownOffset = i;
        ownOffset <<= 3;
        foeOffset = ownOffset + CARD_QUEUE_ENEMY_OFFSET;
        ownOffset += CARD_QUEUE_PLAYER_OFFSET;
        own = (s16 *)(base + ownOffset);
        if (own[CARD_QUEUE_PHASE] == commandId) {
            work->command.commandIndex = 1;
            if (CardPlayCommandPlay(0, (u16)work->commandQueue[0][i][CARD_QUEUE_COMMAND], work,
                                    work->commandQueue[0][i][CARD_QUEUE_OPERAND],
                                    work->commandQueue[0][i][CARD_QUEUE_TARGET]) != 0) {
                CGPSetPermanentMess(work, 0);
                work->command.commandIndex = 0;
                work->command.step = 0;
                own[CARD_QUEUE_PHASE] = -1;
                work->commandQueue[0][i][CARD_QUEUE_OPERAND] = -1;
                work->commandQueue[0][i][CARD_QUEUE_TARGET] = -1;
            }
            break;
        }
        foe = (s16 *)(base + foeOffset);
        if (foe[CARD_QUEUE_PHASE] == commandId) {
            work->command.commandIndex = 1;
            if (CardPlayCommandPlay(1, (u16)work->commandQueue[1][i][CARD_QUEUE_COMMAND], work,
                                    work->commandQueue[1][i][CARD_QUEUE_OPERAND],
                                    work->commandQueue[1][i][CARD_QUEUE_TARGET]) != 0) {
                CGPSetPermanentMess(work, 0);
                work->command.commandIndex = 0;
                work->command.step = 0;
                foe[CARD_QUEUE_PHASE] = -1;
                work->commandQueue[1][i][CARD_QUEUE_OPERAND] = -1;
                work->commandQueue[1][i][CARD_QUEUE_TARGET] = -1;
            }
            break;
        }
    }
    return 1;
}

int CardPlayCommandCheck(s16 command, s16 *queue) {
    int i;
    int count = 0;

    for (i = 0; i < 40; i++) {
        if (queue[i * 4] == command)
            count++;
    }
    return count;
}
