/*
 * OV10 original TU 3: 0x00a12a90..0x00a19a20 (29 functions)
 */
#include "common.h"
#include "ov10/cgp.h"
#include "ov10/card_id.h"
#include "ov10/battle_area_check_enemy.h"
#include "ov10/ccu_038_exec_sub.h"
#include "ccu_038_exec_sub.h"

const char cardPlayDiscardPrompt[40] = "\x0b\x19\x03\x0d\0Select card to discard.";
static int cnt_2;
static int cnt_3;
static int cnt_8;
static int cnt_9;
static int cnt_10;
static int NowNo_11;
static int NowNo_17;
static int NowNo_18;
static int lpcnt_19;
static int NowNo_32;
static int lp_22;
static int NowNo_24;
static int NowNo_26;
static s8 timer_33;
static CardCursorPosition work_0;
static CardCursorPosition work_1;
static CardCursorPosition work_4;
static CardCursorPosition work_5;
static CardCursorPosition work_6;
static CardCursorPosition work_7;
static CardCursorPosition work_12;
static CardCursorPosition work_13;
static CardCursorPosition work_14;
static CardCursorPosition work_15;
static CardCursorPosition work_16;
static CardCursorPosition work_20;
static CardCursorPosition work_21;
static CardCursorPosition work_23;
static CardCursorPosition work_25;
static CardCursorPosition work_27;
static CardCursorPosition work_28;
static CardCursorPosition work_29;
static CardCursorPosition work_30;
static CardCursorPosition work_31;

/* Zeroes a disposal slot's base-layer life when the slot holds a card. */
void CCU038ExecSub(CardPlaySide *owner, CardCursorPosition *position)
{
    if (position != 0) {
        if (owner->disposal[position->index].battle.layers[0].cardId >= 0)
            owner->disposal[position->index].battle.layers[0].life = 0;
    }
}

void CCU055ExecSub(CardPlaySide *owner, CardCursorPosition *position) {
    if (position != 0)
        CardPlayDesertOperation(owner, position->index);
}

/* Freezes a battle slot when the slot holds a card. */
void CCU058ExecSub(int side, CardPlaySide *owner, CardCursorPosition *position)
{
    if (position != 0) {
        if (owner->battle[position->index].battle.layers[0].cardId >= 0)
            owner->battle[position->index].battle.flags |= CARD_STACK_FLAG_FROZEN;
    }
}

/* Freezes a disposal slot when the slot holds a card. */
void CCU061ExecSub(CardPlaySide *side, CardCursorPosition *position)
{
    if (position != 0) {
        if (side->disposal[position->index].battle.layers[0].cardId >= 0)
            side->disposal[position->index].battle.flags |= CARD_STACK_FLAG_FROZEN;
    }
}

void CCU062ExecSub(CardPlaySide *owner, CardCursorPosition *position)
{
    if (position != 0) {
        int cardCount = CardPlayHandCnt(&owner->hand);
        int layerIndex;

        for (layerIndex = 0; layerIndex < CARD_STACK_LAYERS; layerIndex++) {
            if (owner->disposal[position->index].battle.layers[layerIndex].cardId >= 0) {
                owner->hand.cards[cardCount] =
                    owner->disposal[position->index].battle.layers[layerIndex].cardId;
                cardCount++;
                owner->disposal[position->index].battle.layers[layerIndex].cardId = -1;
            }
        }
    }
}

/* Raises the state of every battle card of one attribute: Human (063), Mech (064), Gnosis (065). */
void CCU063ExecSub(CardGameWork *work, CardPlaySide *owner) {
    int i;

    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        CardBoardSlot *slot = &owner->battle[i];
        if (slot->battle.layers[0].cardId >= 0 &&
            (CardDefinitionsFor(work)[slot->battle.layers[0].cardId].flags & CARD_ATTR_HUMAN)) {
            slot->battle.layers[0].powerPlus++;
        }
    }
}

void CCU063ExecCPUSub(CardGameWork *work, CardPlaySide *owner) {
    CGPSetErrorMessPlus(-1, work, CGP_MESS_CONFIRM_CARD, CARD_ID_JULI_MIZRAHI);
    CCU063ExecSub(work, owner);
}

void CCU064ExecSub(CardGameWork *work, CardPlaySide *owner) {
    int i;

    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        CardBoardSlot *slot = &owner->battle[i];
        if (slot->battle.layers[0].cardId >= 0 &&
            (CardDefinitionsFor(work)[slot->battle.layers[0].cardId].flags & CARD_ATTR_MECH)) {
            slot->battle.layers[0].powerPlus++;
        }
    }
}

void CCU064ExecCPUSub(CardGameWork *work, CardPlaySide *owner) {
    CGPSetErrorMessPlus(-1, work, CGP_MESS_CONFIRM_CARD, CARD_ID_PELLEGRI);
    CCU064ExecSub(work, owner);
}

void CCU065ExecSub(CardGameWork *work, CardPlaySide *owner) {
    int i;

    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        CardBoardSlot *slot = &owner->battle[i];
        if (slot->battle.layers[0].cardId >= 0 &&
            (CardDefinitionsFor(work)[slot->battle.layers[0].cardId].flags & CARD_ATTR_GNOSIS)) {
            slot->battle.layers[0].powerPlus += 2;
        }
    }
}

void CCU065ExecCPUSub(CardGameWork *work, CardPlaySide *owner) {
    CGPSetErrorMessPlus(-1, work, CGP_MESS_CONFIRM_CARD, CARD_ID_ZOHAR_EMULATOR);
    CCU065ExecSub(work, owner);
}

/* Stim DX's test: whether a slot's base card, or the weapon stacked on it,
 * attacks by penetration or homing (Proto Dora, AG-05, VX-7000, VX-20000,
 * Lizardman, Hydra; Cannon, Aird), which the doubled ATK does not affect. */
s32 CCC107ChkSub(CardLayerStack *stack) {
    s32 matched = 0;
    u16 cardId = stack->layers[0].cardId;

    /* The card id is read unsigned (lhu), so the signed comparisons need the
     * explicit s16 conversion. */
    if ((u32) (cardId - CARD_ID_PROTO_DORA) < 2U || (s16) cardId == CARD_ID_VX_7000 ||
        (s16) cardId == CARD_ID_VX_20000 || (s16) cardId == CARD_ID_LIZARDMAN ||
        (s16) cardId == CARD_ID_HYDRA) {
        matched = 1;
    }
    if (matched == 0 && stack->layers[1].cardId >= 0 &&
        (stack->layers[1].cardId == CARD_ID_CANNON || stack->layers[1].cardId == CARD_ID_AIRD)) {
        matched = 1;
    }
    return matched;
}

void CCC02ExecSub(u8 *flags) {
    *flags &= ~CARD_STACK_FLAG_FROZEN;
}

void CCC03ExecSub(int side, CardGameWork *work, CardHand *hand, CardLayerStack *card)
{
    float *position = card->position;
    int oldLife = card->layers[0].life;
    int amount = CardDefinitionsFor(work)[card->layers[0].cardId].operationDuration;

    card->layers[0].life = amount;
    amount -= oldLife;
    CardSetEffect(side, work, CARD_EFFECT_SWIRL, 0, 0, position, 0);
    CardSetEffect(side, work, CARD_EFFECT_NUMBER, amount, CARD_SE_REGEN, position, card);
}

void CCC04ExecSub(int side, CardGameWork *work, CardPlaySide *owner)
{
    int i;

    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        int cardId;
        int life;

        cardId = owner->battle[i].battle.layers[0].cardId;
        if (cardId < 0)
            continue;
        if (CardDefinitionsFor(work)[cardId].flags & CARD_ATTR_GNOSIS)
            continue;

        life = (u8) owner->battle[i].battle.layers[0].life + 3;
        owner->battle[i].battle.layers[0].life = life;
        if (CardDefinitionsFor(work)[cardId].operationDuration < (s8) life) {
            owner->battle[i].battle.layers[0].life =
                CardDefinitionsFor(work)[cardId].operationDuration;
        }
        CardSetEffect(side, work, CARD_EFFECT_SWIRL, 0, 0, owner->battle[i].battle.position, 0);
        CardSetEffect(side, work, CARD_EFFECT_NUMBER, 3, CARD_SE_REGEN, owner->battle[i].battle.position,
                      &owner->battle[i].battle);
    }
}

void CCC06ExecSub(int side, CardGameWork *work, CardPlaySide *owner)
{
    int cardCount = 0;
    int i;

    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        if (owner->disposal[i].battle.layers[0].cardId >= 0)
            cardCount++;
    }
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        if (owner->battle[i].battle.layers[0].cardId >= 0)
            cardCount++;
    }
    if (cardCount > 0) {
        i = cardCount;
        do {
            CardPlayRecavery(owner);
        } while (--i != 0);
    }
    if (cardCount == 0)
        return;

    CardSetEffect(side, work, CARD_EFFECT_SWIRL, 0, 0, 0, 0);
    CardSetEffect(side, work, CARD_EFFECT_NUMBER, cardCount, CARD_SE_REGEN, 0, 0);
}

void CCC07ExecSub(CardGameWork *work, CardPlaySide *owner, CardCursorPosition *position)
{
    int destinationIndex;

    for (destinationIndex = 0; destinationIndex < CARD_BOARD_SLOTS; destinationIndex++) {
        if (owner->disposal[destinationIndex].battle.layers[0].cardId < 0)
            break;
    }

    CGPMoveDSBATTLEwork(&owner->disposal[destinationIndex].battle,
                        &owner->battle[position->index].battle);
}

void CCC10ExecSub(int side, CardGameWork *work, CardPlaySide *owner,
                  CardCursorPosition *position)
{
    owner->battle[position->index].battle.layers[0].life -= 2;
    if (owner->battle[position->index].battle.layers[0].life < 0)
        owner->battle[position->index].battle.layers[0].life = 0;

    CardSetEffect(side, work, CARD_EFFECT_SWIRL, 0, 0,
                  owner->battle[position->index].battle.position, 0);
    CardSetEffect(side, work, CARD_EFFECT_DAMAGE_NUMBER, -2, CARD_SE_HIT,
                  owner->battle[position->index].battle.position,
                  &owner->battle[position->index].battle);
}

void CCC26ExecSub(int side, CardGameWork *work, CardPlaySide *owner,
                  CardCursorPosition *position) {
    owner->battle[position->index].battle.layers[0].boostCount++;
    CardSetEffect(side, work, CARD_EFFECT_SWIRL, 0, CARD_SE_REGEN, owner->battle[position->index].battle.position, 0);
    CardCursorPassive(CardCursorFor(work));
}

void CCC30ExecSub(int side, CardGameWork *work, CardLayerStack *card)
{
    card->flags |= CARD_STACK_FLAG_FROZEN;
    CardSetEffect(side, work, CARD_EFFECT_SWIRL, 0, CARD_SE_REGEN, card->position, 0);
}

void CCC31ExecSub(int side, CardGameWork *work, CardLayerStack *card)
{
    float *position = card->position;

    card->layers[0].life--;
    if (card->layers[0].life < 0)
        card->layers[0].life = 0;

    CardSetEffect(side, work, CARD_EFFECT_SWIRL, 0, 0, position, 0);
    CardSetEffect(side, work, CARD_EFFECT_DAMAGE_NUMBER, -1, CARD_SE_HIT, position, card);
}

void CCC38ExecSub(int side, CardGameWork *work, CardPlaySide *owner,
                  CardCursorPosition *position) {
    owner->disposal[position->index].battle.layers[0].life -= 2;
    if (owner->disposal[position->index].battle.layers[0].life < 0)
        owner->disposal[position->index].battle.layers[0].life = 0;
    CardSetEffect(side, work, CARD_EFFECT_SWIRL, 0, 0, owner->disposal[position->index].battle.position, 0);
    CardSetEffect(side, work, CARD_EFFECT_DAMAGE_NUMBER, -2, CARD_SE_HIT, owner->disposal[position->index].battle.position,
                  &owner->disposal[position->index].battle);
    CardCursorPassive(CardCursorFor(work));
}

/* Phases Invitation, Article 4-13, Stim DX and Invoke can be played in (event phase of each side). */
enum CardCommandConditionPhase {
    CARD_COND_PHASE_SIDE0 = CARD_PHASE_EVENT,
    CARD_COND_PHASE_SIDE1 = CARD_PHASE_EVENT + CARD_PHASES_PER_SIDE
};

/* Result codes of CardPlayCommandCondition: 0, or the ErrorMessTbl row the caller shows
 * (1 "Placement Error", 11 "Phase Error", 13 "Command Error"). */
enum CardCommandConditionResult {
    CARD_CONDITION_MET = 0,
    CARD_CONDITION_UNMET = 0xD,
    CARD_CONDITION_WRONG_PHASE = 0xB,
    CARD_CONDITION_OTHER_DISPOSAL_FULL = 1
};

/*
 * Checks whether the play condition of the event cards (card ids 82..119) is
 * met for a side. Invitation, Article 4-13, Stim DX and Invoke first require
 * the side's event phase; cards without a case are always playable.
 *
 * Each do { ... } while (0) group ends in the shared "nothing found" store
 * (`if (!met) result = CARD_CONDITION_UNMET`). Cases that already settled
 * `result` themselves (Miyuki's Email, Song, Retreat, Woglinde, Revive DX)
 * leave through `goto done` instead, skipping that store.
 */
s32 CardPlayCommandCondition(u16 side, u16 cardId, CardGameWork *work) {
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

    switch (cardId) {
    case CARD_ID_MIYUKIS_EMAIL:
    case CARD_ID_SONG:
        result = CARD_CONDITION_UNMET;
        goto done;

    do {
    case CARD_ID_CURE_ALL:
    case CARD_ID_START_UP:
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (own->disposal[i].battle.layers[0].cardId >= 0 && (own->disposal[i].battle.flags & CARD_STACK_FLAG_FROZEN)) {
                met = 1;
                break;
            }
        }
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (own->battle[i].battle.layers[0].cardId >= 0 && (own->battle[i].battle.flags & CARD_STACK_FLAG_FROZEN)) {
                met = 1;
                break;
            }
        }
        break;

    case CARD_ID_CURRY:
    case CARD_ID_BIO_SPHERE:
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (own->disposal[i].battle.layers[0].cardId >= 0 && !(work->definitions[own->disposal[i].battle.layers[0].cardId].flags & CARD_ATTR_GNOSIS)) {
                met = 1;
                break;
            }
        }
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (own->battle[i].battle.layers[0].cardId >= 0 && !(work->definitions[own->battle[i].battle.layers[0].cardId].flags & CARD_ATTR_GNOSIS)) {
                met = 1;
                break;
            }
        }
        break;

    case CARD_ID_SUPPLY_FLEET:
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (own->disposal[i].battle.layers[0].cardId >= 0) {
                met = 1;
                break;
            }
        }
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (own->battle[i].battle.layers[0].cardId >= 0) {
                met = 1;
                break;
            }
        }
        break;

    case CARD_ID_CHARGE:
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (other->disposal[i].battle.layers[0].cardId >= 0) {
                met = 1;
                break;
            }
        }
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (other->battle[i].battle.layers[0].cardId >= 0) {
                met = 1;
                break;
            }
        }
        break;

    case CARD_ID_SURPRISE_ATTACK:
    case CARD_ID_FEDERATION_CRUISER:
    case CARD_ID_CONFUSION:
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (other->battle[i].battle.layers[0].cardId >= 0) {
                met = 1;
                break;
            }
        }
        break;

    case CARD_ID_RETREAT:
    case CARD_ID_WOGLINDE:
        met = 0; /* already 0 here, but the store is in the original */
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (other->battle[i].battle.layers[0].cardId >= 0) {
                met = 1;
                break;
            }
        }
        if (!met)
            break;
        met = 0;
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (other->disposal[i].battle.layers[0].cardId < 0) {
                met = 1;
                break;
            }
        }
        if (!met)
            result = CARD_CONDITION_OTHER_DISPOSAL_FULL;
        goto done;

    case CARD_ID_INVITATION:
        if (side == 0) {
            if (work->command.phase != CARD_COND_PHASE_SIDE0)
                result = CARD_CONDITION_WRONG_PHASE;
        } else
            result = work->command.phase == CARD_COND_PHASE_SIDE1 ? CARD_CONDITION_MET : CARD_CONDITION_WRONG_PHASE;
        if (result)
            return result;
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (own->battle[i].battle.layers[0].cardId >= 0 && work->definitions[own->battle[i].battle.layers[0].cardId].attackType == CARD_ATTACK_HAND) {
                met = 1;
                break;
            }
        }
        break;

    case CARD_ID_LANDING_OPERATIONS:
        for (i = 0; i < CARD_OPERATION_SLOTS; i++) {
            if (other->operations[i] >= 0) {
                met = 1;
                break;
            }
        }
        break;

    case CARD_ID_PREMONITION:
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (other->disposal[i].battle.layers[0].cardId >= 0 && (other->disposal[i].battle.flags & CARD_STACK_FLAG_END)) {
                met = 1;
                break;
            }
        }
        for (i = 0; i < CARD_OPERATION_SLOTS; i++) {
            if (other->operations[i] >= 0 && (other->operationFlags[i] & 1)) {
                met = 1;
                break;
            }
        }
        break;

    case CARD_ID_DRAGON_SKULL:
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            /* The disposal flag is read at 0x00a13b84 (lbu v0,1788(a1)) from the same base
             * register as the battle[i] slot, i.e. as the slot four entries past
             * battle[i]; that is disposal[i] because CardPlaySide stores disposal[4]
             * right after battle[4]. Writing it as
             * other->disposal[i].battle.flags shrinks the function by 16 bytes and
             * shifts every branch target after this case. */
            CardBoardSlot *battleRow = &other->battle[i];
            s16 id = other->disposal[i].battle.layers[0].cardId;
            if (id >= 0 && (battleRow[4].battle.flags & CARD_STACK_FLAG_FROZEN) && (work->definitions[id].flags & CARD_ATTR_GNOSIS)) {
                met = 1;
                break;
            }
            id = other->battle[i].battle.layers[0].cardId;
            if (id >= 0 && (other->battle[i].battle.flags & CARD_STACK_FLAG_FROZEN) && (work->definitions[id].flags & CARD_ATTR_GNOSIS)) {
                met = 1;
                break;
            }
        }
        break;

    case CARD_ID_EXPULSION_ORDER:
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            s16 id = other->disposal[i].battle.layers[0].cardId;
            if (id >= 0 && (other->disposal[i].battle.flags & CARD_STACK_FLAG_END) && (work->definitions[id].flags & CARD_ATTR_HUMAN)) {
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
    case CARD_ID_ARTICLE_4_13:
        if (side == 0) {
            if (work->command.phase != CARD_COND_PHASE_SIDE0)
                result = CARD_CONDITION_WRONG_PHASE;
        } else
            result = work->command.phase == CARD_COND_PHASE_SIDE1 ? CARD_CONDITION_MET : CARD_CONDITION_WRONG_PHASE;
        if (result)
            return result;
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (own->battle[i].battle.layers[0].cardId >= 0 && (work->definitions[own->battle[i].battle.layers[0].cardId].flags & CARD_ATTR_REALIAN)) {
                met = 1;
                break;
            }
        }
        break;

    case CARD_ID_INVOKE:
        if (side == 0) {
            if (work->command.phase != CARD_COND_PHASE_SIDE0)
                result = CARD_CONDITION_WRONG_PHASE;
        } else
            result = work->command.phase == CARD_COND_PHASE_SIDE1 ? CARD_CONDITION_MET : CARD_CONDITION_WRONG_PHASE;
        if (result)
            return result;
        for (i = 0; i < CARD_OPERATION_SLOTS; i++) {
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

    case CARD_ID_PHASE_CANNON:
    case CARD_ID_X_BUSTER:
    case CARD_ID_MILTIAN_CONFLICT:
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
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
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
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

    case CARD_ID_NOTICE:
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (other->disposal[i].battle.layers[0].cardId >= 0 && other->disposal[i].battle.layers[1].cardId >= 0) {
                met = 1;
                break;
            }
        }
        if (met)
            break;
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (other->battle[i].battle.layers[0].cardId >= 0 && other->battle[i].battle.layers[1].cardId >= 0) {
                met = 1;
                break;
            }
        }
        break;

    case CARD_ID_WAR_CASUALTY:
    case CARD_ID_PRECISE_SHOOTING:
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (own->battle[i].battle.layers[0].cardId >= 0) {
                met = 1;
                break;
            }
        }
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (own->disposal[i].battle.layers[0].cardId >= 0) {
                met = 1;
                break;
            }
        }
        break;

    case CARD_ID_RESOLUTION:
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
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
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
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
    case CARD_ID_RECYCLER: {
        junkCount = CardPlayJunkCnt(own);
        for (i = 0; i < junkCount; i++) {
            if (work->definitions[own->junk[i]].flags & (CARD_ATTR_MECH | CARD_ATTR_MECH_WEAPON | CARD_ATTR_HUMAN_WEAPON)) {
                met = 1;
                break;
            }
        }
        break;
    }

    case CARD_ID_REVIVE_DX: {
        junkCount = CardPlayJunkCnt(own);
        met = 0;
        for (i = 0; i < junkCount; i++) {
            if (work->definitions[own->junk[i]].flags & CARD_ATTR_HUMAN)
                met++;
        }
        if (met < 2)
            result = CARD_CONDITION_UNMET;
        goto done;
    }

    case CARD_ID_STIM_DX:
        if (side == 0) {
            if (work->command.phase != CARD_COND_PHASE_SIDE0) {
                met = 1;
                result = CARD_CONDITION_WRONG_PHASE;
            }
        } else {
            if (work->command.phase != CARD_COND_PHASE_SIDE1) {
                met = 1;
                result = CARD_CONDITION_WRONG_PHASE;
            }
        }
        if (result == CARD_CONDITION_MET) {
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
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

    case CARD_ID_SIMULATION: {
        junkCount = CardPlayJunkCnt(own) - 1;
        if (junkCount < 0)
            break;
        if (work->definitions[own->junk[junkCount]].kind & CARD_KIND_SITUATION)
            met = 1;
        break;
    }

    } while (0);
    if (!met)
        result = CARD_CONDITION_UNMET;
    break;

    case CARD_ID_DESTINY: {
        junkCount = CardPlayJunkCnt(own);
        for (i = 0; i < junkCount; i++) {
            if ((work->definitions[own->junk[i]].flags & CARD_ATTR_HUMAN) && CardPlayIdentityCheck(work, own, work->definitions[own->junk[i]].identity) == 0) {
                met = 1;
                break;
            }
        }
        if (met) {
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (own->battle[i].battle.layers[0].cardId >= 0 && (work->definitions[own->battle[i].battle.layers[0].cardId].flags & CARD_ATTR_HUMAN)) {
                    met = 2;
                    break;
                }
            }
        }
        if (met != 2)
            result = CARD_CONDITION_UNMET;
        break;
    }

    case CARD_ID_BREAK_THROUGH:
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
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
        for (i = 0; i < CARD_QUEUE_SLOTS; i++) {
            if (work->commandQueue[0][i][CARD_QUEUE_PHASE] < 0)
                break;
        }
        if (i == CARD_QUEUE_SLOTS)
            return 0;
        return work->commandQueue[0][i];
    }
    for (i = 0; i < CARD_QUEUE_SLOTS; i++) {
        if (work->commandQueue[1][i][CARD_QUEUE_PHASE] < 0)
            break;
    }
    if (i == CARD_QUEUE_SLOTS)
        return 0;
    return work->commandQueue[1][i];
}

/*
 * Copies a battle or disposal slot's header bytes and, for each of its forty
 * layers, the state, life and card id. The parameters are untyped: Confusion
 * passes the start of its slot-sized CardBoardSlot temporaries, and typing
 * them CardLayerStack * moves them by CardBoardSlot's 12-byte header
 * (measured: stack frame and every temporary address change).
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
    for (i = 0; i < CARD_STACK_LAYERS; i++) {
        dst->layers[i].life = src->layers[i].life;
        dst->layers[i].powerPlus = src->layers[i].powerPlus;
        dst->layers[i].cardId = src->layers[i].cardId;
    }
}

static inline int CardPlayerAiFlag(int flags) {
    return flags & CGP_FLAG_SIDE0_AUTO;
}

static inline int CardEnemyAiFlag(int flags) {
    return flags & CGP_FLAG_SIDE1_AUTO;
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
    case CARD_COMMAND_DESTROY_OWN_BOARD: {
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (ownSide->battle[i].battle.layers[0].cardId >= 0)
                ownSide->battle[i].battle.layers[0].life = 0;
        }
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (ownSide->disposal[i].battle.layers[0].cardId >= 0)
                ownSide->disposal[i].battle.layers[0].life = 0;
        }
        complete = 1;
        break;
    }
    case CARD_COMMAND_DRAW_CARD:
        switch (CardCommandStep(work)) {
        case 0:
            if (operand >= 0)
                CGPSetErrorMessPlus(side, work, CGP_ERR_USE_CARD_EFFECT, operand);
            CardCommandAdvance(work);
            break;
        case 1:
            complete = 1;
            CardPlayDrawCard(ownSide);
            break;
        }
        break;
    case CARD_COMMAND_REALIAN_MALE:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = CARD_COMMAND_DRAW_CARD;
            queued[CARD_QUEUE_PHASE] = CARD_PHASE_ADJUST;
            queued[CARD_QUEUE_OPERAND] = CARD_ID_REALIAN_MALE;
            if (side != 0)
                queued[CARD_QUEUE_PHASE] = CARD_PHASE_ADJUST_SIDE1;
        }
        complete = 1;
        break;
    case CARD_COMMAND_REALIAN_FEMALE:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = CARD_COMMAND_DRAW_CARD;
            queued[CARD_QUEUE_PHASE] = CARD_PHASE_ADJUST;
            queued[CARD_QUEUE_OPERAND] = CARD_ID_REALIAN_FEMALE;
            if (side != 0)
                queued[CARD_QUEUE_PHASE] = CARD_PHASE_ADJUST_SIDE1;
        }
        complete = 1;
        break;
    case CARD_COMMAND_LV_1_SHION_TRIGGER:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = CARD_COMMAND_LV_1_SHION;
            queued[CARD_QUEUE_PHASE] = CardCommandPhase(work);
            queued[CARD_QUEUE_OPERAND] = operand;
        }
        complete = 1;
        break;
    case CARD_COMMAND_VX_9000_TRIGGER:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = CARD_COMMAND_VX_9000;
            queued[CARD_QUEUE_PHASE] = CardCommandPhase(work);
        }
        complete = 1;
        break;
    case CARD_COMMAND_CURE_ALL:
    case CARD_COMMAND_START_UP: {
        int disposeZone;
        int battleZone;
        disposeZone = CARD_ZONE_SIDE0_DISPOSAL;
        battleZone = CARD_ZONE_SIDE0_BATTLE;
        if (side != 0) {
            disposeZone = CARD_ZONE_SIDE1_DISPOSAL;
            battleZone = CARD_ZONE_SIDE1_BATTLE;
        }
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetPermanentMess(work, CGP_PERM_MESS_SELECT_REMOVE_DOWN);
            CardCursorPassive(CardCursorFor(work));
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (ownSide->disposal[i].battle.layers[0].cardId >= 0 &&
                    (ownSide->disposal[i].battle.flags & CARD_STACK_FLAG_FROZEN)) {
                    position.zone = disposeZone;
                    position.index = i;
                    CardCursorActive(CardCursorFor(work), &position, CARD_CURSOR_SELECTABLE);
                }
            }
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (ownSide->battle[i].battle.layers[0].cardId >= 0 &&
                    (ownSide->battle[i].battle.flags & CARD_STACK_FLAG_FROZEN)) {
                    position.zone = battleZone;
                    position.index = i;
                    CardCursorActive(CardCursorFor(work), &position, CARD_CURSOR_SELECTABLE);
                }
            }
            CardCommandAdvance(work);
            break;
        case 1:
            CGPCursorMove(side, work);
            CGPCursor2Pos(&work_0, work);
            if (pressed & PAD_CIRCLE) {
                if ((disposeZone == work_0.zone || battleZone == work_0.zone) &&
                    work_0.cardId >= 0 &&
                    (CardSelectBattle(ownSide, work_0.index,
                                      disposeZone == work_0.zone)->flags & CARD_STACK_FLAG_FROZEN)) {
                    CardCommandAdvance(work);
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            } else if (pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_BUZZER, 0);
            }
            break;
        case 2:
            complete = 1;
            CCC02ExecSub(&CardSelectBattle(ownSide, work_0.index,
                                           disposeZone == work_0.zone)->flags);
            CardCursorPassive(CardCursorFor(work));
            break;
        }
        break;
    }
    case CARD_COMMAND_CURRY: {
        int disposeZone;
        int battleZone;
        disposeZone = CARD_ZONE_SIDE0_DISPOSAL;
        battleZone = CARD_ZONE_SIDE0_BATTLE;
        if (side != 0) {
            disposeZone = CARD_ZONE_SIDE1_DISPOSAL;
            battleZone = CARD_ZONE_SIDE1_BATTLE;
        }
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetPermanentMess(work, CGP_PERM_MESS_SELECT_RECOVER_HP);
            CardCursorPassive(CardCursorFor(work));
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                s16 *slotCardId = &ownSide->disposal[i].battle.layers[0].cardId;
                int cardId = *slotCardId;
                if (cardId >= 0 && !(CardDefinitionsFor(work)[cardId].flags & CARD_ATTR_GNOSIS)) {
                    position.zone = disposeZone;
                    position.index = i;
                    CardCursorActive(CardCursorFor(work), &position, CARD_CURSOR_SELECTABLE);
                }
            }
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                s16 *slotCardId = &ownSide->battle[i].battle.layers[0].cardId;
                int cardId = *slotCardId;
                if (cardId >= 0 && !(CardDefinitionsFor(work)[cardId].flags & CARD_ATTR_GNOSIS)) {
                    position.zone = battleZone;
                    position.index = i;
                    CardCursorActive(CardCursorFor(work), &position, CARD_CURSOR_SELECTABLE);
                }
            }
            CardCommandAdvance(work);
            break;
        case 1:
            CGPCursorMove(side, work);
            CGPCursor2Pos(&work_1, work);
            if (pressed & PAD_CIRCLE) {
                if ((disposeZone == work_1.zone || battleZone == work_1.zone) &&
                    work_1.cardId >= 0 && !(CardDefinitionsFor(work)[work_1.cardId].flags & CARD_ATTR_GNOSIS)) {
                    CardCommandAdvance(work);
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            } else if (pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_BUZZER, 0);
            }
            break;
        case 2:
            complete = 1;
            CCC03ExecSub(side, work, &ownSide->hand,
                         CardSelectBattle(ownSide, work_1.index,
                                          disposeZone == work_1.zone));
            CardCursorPassive(CardCursorFor(work));
            break;
        }
        break;
    }
    case CARD_COMMAND_BIO_SPHERE:
        complete = 1;
        CCC04ExecSub(side, work, ownSide);
        break;
    case CARD_COMMAND_SUPPLY_SHIP:
        switch (CardCommandStep(work)) {
        case 0:
            CardCommandAdvance(work);
            cnt_2 = CARD_SUPPLY_SHIP_RECOVERY;
            CardSetEffect(side, work, CARD_EFFECT_NUMBER, CARD_SUPPLY_SHIP_RECOVERY, CARD_SE_REGEN, 0, 0);
            /* The first recovery occurs in the same tick as setup. */
        case 1:
            CardPlayRecavery(ownSide);
            CardSetEffect(side, work, CARD_EFFECT_SWIRL, 0, 0, 0, 0);
            {
                /* Typed reference: the original schedules this store apart
                 * from the step store below. */
                int *remaining = &cnt_2;
                (*remaining)--;
            }
            CardCommandAdvance(work);
            /* Recheck the count without waiting another tick. */
        case 2:
            if (cnt_2 > 0)
                CardCommandRewind(work);
            else
                CardCommandAdvance(work);
            break;
        case 3:
            complete = 1;
            break;
        }
        break;
    case CARD_COMMAND_SUPPLY_FLEET: {
        int i;
        switch (CardCommandStep(work)) {
        case 0:
            cnt_3 = 0;
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (ownSide->disposal[i].battle.layers[0].cardId > 0)
                    cnt_3++;
            }
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (ownSide->battle[i].battle.layers[0].cardId > 0)
                    cnt_3++;
            }
            CardSetEffect(side, work, CARD_EFFECT_NUMBER, cnt_3, CARD_SE_REGEN, 0, 0);
            CardCommandAdvance(work);
        case 1:
            CardPlayRecavery(ownSide);
            CardSetEffect(side, work, CARD_EFFECT_SWIRL, 0, 0, 0, 0);
            {
                /* Typed reference: the original schedules this store apart
                 * from the step store below. */
                int *remaining = &cnt_3;
                (*remaining)--;
            }
            CardCommandAdvance(work);
        case 2:
            if (cnt_3 > 0)
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
    case CARD_COMMAND_RETREAT:
    case CARD_COMMAND_WOGLINDE: {
        int battleZone;
        battleZone = CARD_ZONE_SIDE1_BATTLE;
        if (side != 0)
            battleZone = CARD_ZONE_SIDE0_BATTLE;
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetPermanentMess(work, CGP_PERM_MESS_SELECT_RETURN_TO_STANDBY);
            CardCursorPassive(CardCursorFor(work));
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (otherSide->battle[i].battle.layers[0].cardId >= 0) {
                    position.zone = battleZone;
                    position.index = i;
                    CardCursorActive(CardCursorFor(work), &position, CARD_CURSOR_SELECTABLE);
                }
            }
            CardCommandAdvance(work);
            break;
        case 1:
            CGPCursorMove(side, work);
            CGPCursor2Pos(&work_4, work);
            if (pressed & PAD_CIRCLE) {
                if (battleZone == work_4.zone && work_4.cardId >= 0) {
                    CardCommandAdvance(work);
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            } else if (pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_BUZZER, 0);
            }
            break;
        case 2:
            CCC07ExecSub(work, otherSide, &work_4);
            CardCursorPassive(CardCursorFor(work));
            complete = 1;
            break;
        }
        break;
    }
    case CARD_COMMAND_PRECISE_SHOOTING: {
        int disposalTargetZone;
        int battleTargetZone;
        disposalTargetZone = CARD_ZONE_SIDE1_DISPOSAL;
        battleTargetZone = CARD_ZONE_SIDE1_BATTLE;
        if (side != 0) {
            disposalTargetZone = CARD_ZONE_SIDE0_DISPOSAL;
            battleTargetZone = CARD_ZONE_SIDE0_BATTLE;
        }
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetPermanentMess(work, CGP_PERM_MESS_SELECT_ATTACK);
            CardCursorPassive(CardCursorFor(work));
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (otherSide->disposal[i].battle.layers[0].cardId >= 0) {
                    position.zone = disposalTargetZone;
                    position.index = i;
                    CardCursorActive(CardCursorFor(work), &position, CARD_CURSOR_SELECTABLE);
                }
            }
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (otherSide->battle[i].battle.layers[0].cardId >= 0) {
                    position.zone = battleTargetZone;
                    position.index = i;
                    CardCursorActive(CardCursorFor(work), &position, CARD_CURSOR_SELECTABLE);
                }
            }
            CardCommandAdvance(work);
            break;
        case 1:
            CGPCursorMove(side, work);
            CGPCursor2Pos(&work_5, work);
            if (pressed & PAD_CIRCLE) {
                if ((disposalTargetZone == work_5.zone || battleTargetZone == work_5.zone) && work_5.cardId >= 0) {
                    CardCommandAdvance(work);
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            } else if (pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_BUZZER, 0);
            }
            break;
        case 2: {
            CardLayerStack *selected;
            if (disposalTargetZone == work_5.zone)
                selected = &otherSide->disposal[work_5.index].battle;
            else
                selected = &otherSide->battle[work_5.index].battle;
            complete = 1;
            CCC31ExecSub(opponentSide, work, selected);
            CardCursorPassive(CardCursorFor(work));
            break;
        }
        }
        break;
    }
    case CARD_COMMAND_CONFUSION:
        CPCPSwapDSBATTLE(&swapFirst, &otherSide->battle[0].battle);
        CPCPSwapDSBATTLE(&swapSecond, &otherSide->battle[1].battle);
        CPCPSwapDSBATTLE(&otherSide->battle[0].battle, &otherSide->battle[2].battle);
        complete = 1;
        CPCPSwapDSBATTLE(&otherSide->battle[1].battle, &otherSide->battle[3].battle);
        CPCPSwapDSBATTLE(&otherSide->battle[2].battle, &swapFirst);
        CPCPSwapDSBATTLE(&otherSide->battle[3].battle, &swapSecond);
        break;
    case CARD_COMMAND_BREAK_THROUGH: {
        int disposalTargetZone;
        disposalTargetZone = CARD_ZONE_SIDE1_DISPOSAL;
        if (side != 0)
            disposalTargetZone = CARD_ZONE_SIDE0_DISPOSAL;
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetPermanentMess(work, CGP_PERM_MESS_SELECT_ATTACK);
            CardCursorPassive(CardCursorFor(work));
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (otherSide->disposal[i].battle.layers[0].cardId >= 0) {
                    position.zone = disposalTargetZone;
                    position.index = i;
                    CardCursorActive(CardCursorFor(work), &position, CARD_CURSOR_SELECTABLE);
                }
            }
            CardCommandAdvance(work);
            break;
        case 1:
            CGPCursorMove(side, work);
            CGPCursor2Pos(&work_6, work);
            if (pressed & PAD_CIRCLE) {
                if (disposalTargetZone == work_6.zone && work_6.cardId >= 0) {
                    CardCommandAdvance(work);
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            } else if (pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_BUZZER, 0);
            }
            break;
        case 2:
            complete = 1;
            CCC38ExecSub(opponentSide, work, otherSide, &work_6);
            break;
        default:
            break;
        }
        break;
    }
    case CARD_COMMAND_SURPRISE_ATTACK:
    case CARD_COMMAND_FEDERATION_CRUISER: {
        int battleZone;
        battleZone = CARD_ZONE_SIDE1_BATTLE;
        if (side != 0)
            battleZone = CARD_ZONE_SIDE0_BATTLE;
        switch (CardCommandStep(work)) {
        case 0:
            CardCursorPassive(CardCursorFor(work));
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (otherSide->battle[i].battle.layers[0].cardId >= 0) {
                    position.zone = battleZone;
                    position.index = i;
                    CardCursorActive(CardCursorFor(work), &position, CARD_CURSOR_SELECTABLE);
                }
            }
            CardCommandAdvance(work);
            break;
        case 1:
            CGPCursorMove(side, work);
            CGPCursor2Pos(&work_7, work);
            if (pressed & PAD_CIRCLE) {
                if (battleZone == work_7.zone && work_7.cardId >= 0) {
                    CardCommandAdvance(work);
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            } else if (pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_BUZZER, 0);
            }
            break;
        case 2:
            complete = 1;
            CCC10ExecSub(opponentSide, work, otherSide, &work_7);
            CardCursorPassive(CardCursorFor(work));
            break;
        }
        break;
    }
    case CARD_COMMAND_INVITATION: {
        int i;
        switch (CardCommandStep(work)) {
        case 0:
            cnt_8 = 0;
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                CardLayerStack *card = &ownSide->battle[i].battle;
                if (card->layers[0].cardId >= 0 && CardChkAttackType(CardDefinitionsFor(work), card) == CARD_ATTACK_HAND) {
                    int power = CardDefinitionsFor(work)[card->layers[0].cardId].attackPower;
                    if (card->layers[1].cardId >= 0)
                        power = CardDefinitionsFor(work)[card->layers[1].cardId].attackPower;
                    cnt_8 += CGPCalcPowerPlus(work, card, power);
                }
            }
            CardCommandAdvance(work);
            break;
        case 1: {
            int remainingRecovery;
            CardPlayRecavery(ownSide);
            CardSetEffect(side, work, CARD_EFFECT_SWIRL, 0, 0, 0, 0);
            CardSetEffect(side, work, CARD_EFFECT_NUMBER, 1, CARD_SE_REGEN, 0, 0);
            remainingRecovery = cnt_8 - 1;
            CardCommandAdvance(work);
            cnt_8 = remainingRecovery;
        }
        case 2:
            if (cnt_8 > 0)
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
    case CARD_COMMAND_MARY_AND_SHELLEY:
        switch (CardCommandStep(work)) {
        case 0:
            CardCommandAdvance(work);
            cnt_9 = CARD_MARY_AND_SHELLEY_DRAWS;
            break;
        case 1: {
            int drawsRemaining;
            CardPlayDrawCard(ownSide);
            drawsRemaining = cnt_9 - 1;
            CardCommandAdvance(work);
            cnt_9 = drawsRemaining;
            break;
        }
        case 2:
            if (cnt_9 > 0)
                CardCommandRewind(work);
            else
                CardCommandAdvance(work);
            break;
        case 3:
            complete = 1;
            break;
        }
        break;
    case CARD_COMMAND_UMN_INVASION:
        switch (CardCommandStep(work)) {
        case 0:
            CardCommandAdvance(work);
            cnt_10 = CARD_UMN_INVASION_DRAWS;
            break;
        case 1: {
            int drawsRemaining;
            CardPlayDrawSute(ownSide);
            drawsRemaining = cnt_10 - 1;
            CardCommandAdvance(work);
            cnt_10 = drawsRemaining;
        }
        case 2:
            if (cnt_10 > 0)
                CardCommandRewind(work);
            else
                CardCommandAdvance(work);
            break;
        case 3:
            complete = 1;
            break;
        }
        break;
    case CARD_COMMAND_INVESTIGATION:
        switch (CardCommandStep(work)) {
        case 0:
            CardCommandAdvance(work);
            CGPSetPermanentMess(work, CGP_PERM_MESS_SELECT_ADD_TO_HAND);
            break;
        case 1: {
            int count = CardPlayYamaCnt(ownSide);
            int rotated;
            if (count > CARD_DECK_VIEW_COUNT)
                count = CARD_DECK_VIEW_COUNT;
            if (repeated & PAD_RIGHT) {
                NowNo_11++;
                xglSoundEffectNormalID(SE_CURSOR, 0);
                if (NowNo_11 >= count)
                    NowNo_11 = count - 1;
            }
            if (repeated & PAD_LEFT) {
                NowNo_11--;
                xglSoundEffectNormalID(SE_CURSOR, 0);
                if (NowNo_11 < 0)
                    NowNo_11 = 0;
            }
            if (pressed & PAD_CIRCLE) {
                xglSoundEffectNormalID(SE_DECIDE, 0);
                CardCommandAdvance(work);
            } else if (pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_BUZZER, 0);
            }
            rotated = 1;
            if (!(held & PAD_TRIANGLE)) {
                if ((side != 0 && work->rotStageId == 0) ||
                    (side == 0 && work->rotStageId != 0)) {
                    CGCPHandDispSub3(CardDefinitionsFor(work), ownSide, -1, 0, 0);
                } else {
                    rotated = 0;
                    CGCPHandDispSub2(CardDefinitionsFor(work), ownSide, -1, 0);
                }
            }
            CGCPYamaDispSub(ownSide, NowNo_11, count, held, rotated);
            break;
        }
        case 2: {
            int first = CardPlayYamaCnt(ownSide);
            if (first > CARD_DECK_VIEW_COUNT)
                first -= CARD_DECK_VIEW_COUNT;
            else
                first = 0;
            ownSide->hand.cards[CardPlayHandCnt(&ownSide->hand)] = ownSide->deck[first + NowNo_11];
            ownSide->deck[first + NowNo_11] = -1;
            CardPlayCleanYama(ownSide);
            CardPlayShuffleYama(ownSide);
            CardCommandAdvance(work);
            break;
        }
        case 3:
        case CARD_STEP_FINISH:
            complete = 1;
            break;
        }
        break;
    case CARD_COMMAND_LANDING_OPERATIONS: {
        int operationZone;
        operationZone = CARD_ZONE_SIDE1_OPERATION;
        if (side != 0)
            operationZone = CARD_ZONE_SIDE0_OPERATION;
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetPermanentMess(work, CGP_PERM_MESS_SELECT_DISCARDED);
            CardCursorPassive(CardCursorFor(work));
            for (i = 0; i < CARD_OPERATION_SLOTS; i++) {
                if (otherSide->operations[i] >= 0) {
                    position.zone = operationZone;
                    position.index = i;
                    CardCursorActive(CardCursorFor(work), &position, CARD_CURSOR_SELECTABLE);
                }
            }
            CardCommandAdvance(work);
            break;
        case 1:
            CGPCursorMove(side, work);
            CGPCursor2Pos(&work_12, work);
            if (pressed & PAD_CIRCLE) {
                if (operationZone == work_12.zone && work_12.cardId >= 0) {
                    CardCommandAdvance(work);
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            } else if (pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_BUZZER, 0);
            }
            break;
        case 2:
            complete = 1;
            CardPlayDesertOperation(otherSide, work_12.index);
            CardCursorPassive(CardCursorFor(work));
            break;
        }
        break;
    }
    case CARD_COMMAND_PREMONITION: {
        int i;
        int disposalTargetZone;
        int operationTargetZone;
        disposalTargetZone = CARD_ZONE_SIDE1_DISPOSAL;
        operationTargetZone = CARD_ZONE_SIDE1_OPERATION;
        if (side != 0) {
            disposalTargetZone = CARD_ZONE_SIDE0_DISPOSAL;
            operationTargetZone = CARD_ZONE_SIDE0_OPERATION;
        }
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetPermanentMess(work, CGP_PERM_MESS_SELECT_DISCARDED);
            CardCursorPassive(CardCursorFor(work));
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (otherSide->disposal[i].battle.layers[0].cardId >= 0 &&
                    (otherSide->disposal[i].battle.flags & CARD_STACK_FLAG_END)) {
                    work_13.zone = disposalTargetZone;
                    work_13.index = i;
                    CardCursorActive(CardCursorFor(work), &work_13, CARD_CURSOR_SELECTABLE);
                }
            }
            for (i = 0; i < CARD_OPERATION_SLOTS; i++) {
                if (otherSide->operations[i] >= 0 && (otherSide->operationFlags[i] & 1)) {
                    work_13.zone = operationTargetZone;
                    work_13.index = i;
                    CardCursorActive(CardCursorFor(work), &work_13, CARD_CURSOR_SELECTABLE);
                }
            }
            CardCommandAdvance(work);
            break;
        case 1:
            CGPCursorMove(side, work);
            CGPCursor2Pos(&work_13, work);
            if (pressed & PAD_CIRCLE) {
                if (work_13.cardId >= 0) {
                    if (disposalTargetZone == work_13.zone) {
                        if (otherSide->disposal[work_13.index].battle.flags & CARD_STACK_FLAG_END)
                            CardCommandAdvance(work);
                    } else if (operationTargetZone == work_13.zone &&
                               (otherSide->operationFlags[work_13.index] & 1)) {
                        CardCommandAdvance(work);
                    }
                }
                if (CardCommandStep(work) == 2)
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                else
                    xglSoundEffectNormalID(SE_BUZZER, 0);
            } else if (pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_BUZZER, 0);
            }
            break;
        case 2:
            if (disposalTargetZone == work_13.zone)
                otherSide->disposal[work_13.index].battle.layers[0].life = 0;
            else
                CardPlayDesertOperation(otherSide, work_13.index);
            complete = 1;
            CardCursorPassive(CardCursorFor(work));
            break;
        }
        break;
    }
    case CARD_COMMAND_DRAGON_SKULL: {
        int disposeZone;
        int battleZone;
        CardLayerStack *card;
        disposeZone = CARD_ZONE_SIDE1_DISPOSAL;
        battleZone = CARD_ZONE_SIDE1_BATTLE;
        if (side != 0) {
            disposeZone = CARD_ZONE_SIDE0_DISPOSAL;
            battleZone = CARD_ZONE_SIDE0_BATTLE;
        }
        switch (CardCommandStep(work)) {
        case 0: {
            CardBoardSlot *slot;
            CardCommandAdvance(work);
            CardCursorPassive(CardCursorFor(work));
            work_14.zone = disposeZone;
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                slot = &otherSide->disposal[i];
                if (slot->battle.layers[0].cardId >= 0 && (slot->battle.flags & CARD_STACK_FLAG_FROZEN) &&
                    (CardDefinitionsFor(work)[slot->battle.layers[0].cardId].flags & CARD_ATTR_GNOSIS)) {
                    work_14.index = i;
                    CardCursorActive(CardCursorFor(work), &work_14, CARD_CURSOR_SELECTABLE);
                }
            }
            work_14.zone = battleZone;
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                slot = &otherSide->battle[i];
                if (slot->battle.layers[0].cardId >= 0 && (slot->battle.flags & CARD_STACK_FLAG_FROZEN) &&
                    (CardDefinitionsFor(work)[slot->battle.layers[0].cardId].flags & CARD_ATTR_GNOSIS)) {
                    work_14.index = i;
                    CardCursorActive(CardCursorFor(work), &work_14, CARD_CURSOR_SELECTABLE);
                }
            }
            CGPSetPermanentMess(work, CGP_PERM_MESS_SELECT_DISCARDED);
            break;
        }
        case 1:
            CGPCursorMove(side, work);
            CGPCursor2Pos(&work_14, work);
            if (pressed & PAD_CIRCLE) {
                if (work_14.cardId >= 0 &&
                    (disposeZone == work_14.zone || battleZone == work_14.zone)) {
                    card = &otherSide->disposal[work_14.index].battle;
                    if (battleZone == work_14.zone)
                        card = &otherSide->battle[work_14.index].battle;
                    if ((CardDefinitionsFor(work)[card->layers[0].cardId].flags & CARD_ATTR_GNOSIS) && (card->flags & CARD_STACK_FLAG_FROZEN)) {
                        CardCursorPassive(CardCursorFor(work));
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                        CardCommandAdvance(work);
                    } else {
                        xglSoundEffectNormalID(SE_BUZZER, 0);
                    }
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            } else if (pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_BUZZER, 0);
            }
            break;
        case 2: {
            CardLayerStack *stack = &otherSide->disposal[work_14.index].battle;
            if (battleZone == work_14.zone)
                stack = &otherSide->battle[work_14.index].battle;
            complete = 1;
            stack->layers[0].life = 0;
            CardSetEffect(side, work, CARD_EFFECT_SWIRL, 0, CARD_SE_HIT, stack->position, 0);
            break;
        }
        }
        break;
    }
    case CARD_COMMAND_EXPULSION_ORDER: {
        int targetZone;
        targetZone = CARD_ZONE_SIDE1_DISPOSAL;
        if (side != 0)
            targetZone = CARD_ZONE_SIDE0_DISPOSAL;
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetPermanentMess(work, CGP_PERM_MESS_SELECT_DISCARDED);
            CardCursorPassive(CardCursorFor(work));
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                CardBoardSlot *slot = &otherSide->disposal[i];
                if (slot->battle.layers[0].cardId >= 0 && (CardDefinitionsFor(work)[slot->battle.layers[0].cardId].flags & CARD_ATTR_HUMAN) && (slot->battle.flags & CARD_STACK_FLAG_END)) {
                    work_15.zone = targetZone;
                    work_15.index = i;
                    CardCursorActive(CardCursorFor(work), &work_15, CARD_CURSOR_SELECTABLE);
                }
            }
            CardCommandAdvance(work);
            break;
        case 1:
            CGPCursorMove(side, work);
            CGPCursor2Pos(&work_15, work);
            if (pressed & PAD_CIRCLE) {
                if (work_15.cardId >= 0 && targetZone == work_15.zone) {
                    if ((CardDefinitionsFor(work)[otherSide->disposal[work_15.index].battle.layers[0].cardId].flags & CARD_ATTR_HUMAN) &&
                        (otherSide->disposal[work_15.index].battle.flags & CARD_STACK_FLAG_END)) {
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                        CardCommandAdvance(work);
                    } else {
                        xglSoundEffectNormalID(SE_BUZZER, 0);
                    }
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            } else if (pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_BUZZER, 0);
            }
            break;
        case 2:
            complete = 1;
            otherSide->disposal[work_15.index].battle.layers[0].life = 0;
            CardCursorPassive(CardCursorFor(work));
            break;
        }
        break;
    }
    case CARD_COMMAND_IMPREGNABLE_GUARD:
        ownSide->hand.commandFlags |= 1;
        complete = 1;
        break;
    case CARD_COMMAND_IMPREGNABLE_GUARD_TRIGGER:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = CARD_COMMAND_IMPREGNABLE_GUARD_NOTICE;
            queued[CARD_QUEUE_PHASE] = CardCommandPhase(work);
        }
        complete = 1;
        break;
    case CARD_COMMAND_IMPREGNABLE_GUARD_NOTICE:
        CGPSetErrorMessPlus(side, work, CGP_ERR_USE_CARD_EFFECT, CARD_ID_IMPREGNABLE_GUARD);
        complete = 1;
        break;
    case CARD_COMMAND_ARTICLE_4_13: {
        int i;
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (ownSide->battle[i].battle.layers[0].cardId >= 0 &&
                (CardDefinitionsFor(work)[ownSide->battle[i].battle.layers[0].cardId].flags & CARD_ATTR_REALIAN)) {
                ownSide->battle[i].battle.layers[0].life = 1;
                ownSide->battle[i].battle.flags |= CARD_STACK_FLAG_NO_DAMAGE;
                CardSetEffect(side, work, CARD_EFFECT_SWIRL, 0, CARD_SE_REGEN, ownSide->battle[i].battle.position, 0);
            }
        }
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = CARD_COMMAND_ARTICLE_4_13_ADJUST;
            queued[CARD_QUEUE_PHASE] = CARD_PHASE_ADJUST_SIDE1;
            if (side != 0)
                queued[CARD_QUEUE_PHASE] = CARD_PHASE_ADJUST;
        }
        complete = 1;
        break;
    }
    case CARD_COMMAND_ARTICLE_4_13_ADJUST: {
        int i;
        int count;
        switch (CardCommandStep(work)) {
        case 0: {
            count = 0;
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                s16 *slotCardId = &ownSide->battle[i].battle.layers[0].cardId;
                int cardId = *slotCardId;
                if (cardId >= 0 && (CardDefinitionsFor(work)[cardId].flags & CARD_ATTR_REALIAN)) {
                    count = 1;
                    break;
                }
            }
            if (count) {
                CGPSetErrorMessPlus(side, work, CGP_ERR_USE_CARD_EFFECT, CARD_ID_ARTICLE_4_13);
                CardCommandAdvance(work);
            } else {
                complete = 1;
            }
            break;
        }
        case 1: {
            count = 0;
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (ownSide->battle[i].battle.layers[0].cardId >= 0 &&
                    (CardDefinitionsFor(work)[ownSide->battle[i].battle.layers[0].cardId].flags & CARD_ATTR_REALIAN) &&
                    ownSide->battle[i].battle.layers[0].life != 0) {
                    int remaining;
                    CardPlayDesertBattle(ownSide, &ownSide->battle[i].battle);
                    count += CARD_ARTICLE_4_13_DAMAGE;
                    remaining = CARD_ARTICLE_4_13_DAMAGE - 1;
                    do {
                        remaining--;
                        CardPlayCostCard(otherSide);
                    } while (remaining >= 0);
                }
            }
            if (count != 0) {
                CardSetEffect(side, work, CARD_EFFECT_HIT_NUMBER, count, CARD_SE_HIT, 0, 0);
                CardSetEffect(side, work, CARD_EFFECT_DECK_BURST, 0, 0, 0, 0);
            }
            complete = 1;
            break;
        }
        }
        break;
    }
    case CARD_COMMAND_VIRGIL_TRIGGER: {
        int i;
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (ownSide->battle[i].battle.layers[0].cardId >= 0 &&
                (CardDefinitionsFor(work)[ownSide->battle[i].battle.layers[0].cardId].flags & CARD_ATTR_REALIAN)) {
                ownSide->battle[i].battle.layers[0].life = 1;
                ownSide->battle[i].battle.flags |= CARD_STACK_FLAG_NO_DAMAGE;
                CardSetEffect(side, work, CARD_EFFECT_SWIRL, 0, CARD_SE_REGEN, ownSide->battle[i].battle.position, 0);
            }
        }
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = CARD_COMMAND_VIRGIL;
            queued[CARD_QUEUE_PHASE] = CARD_PHASE_ADJUST_SIDE1;
            if (side != 0)
                queued[CARD_QUEUE_PHASE] = CARD_PHASE_ADJUST;
        }
        complete = 1;
        break;
    }
    case CARD_COMMAND_VIRGIL: {
        int i;
        int count;
        switch (CardCommandStep(work)) {
        case 0: {
            count = 0;
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                s16 *slotCardId = &ownSide->battle[i].battle.layers[0].cardId;
                int cardId = *slotCardId;
                if (cardId >= 0 && (CardDefinitionsFor(work)[cardId].flags & CARD_ATTR_REALIAN)) {
                    count = 1;
                    break;
                }
            }
            if (count) {
                CGPSetErrorMessPlus(side, work, CGP_ERR_USE_CARD_EFFECT, CARD_ID_VIRGIL);
                CardCommandAdvance(work);
            } else {
                complete = 1;
            }
            break;
        }
        case 1: {
            count = 0;
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (ownSide->battle[i].battle.layers[0].cardId >= 0 &&
                    (CardDefinitionsFor(work)[ownSide->battle[i].battle.layers[0].cardId].flags & CARD_ATTR_REALIAN) &&
                    ownSide->battle[i].battle.layers[0].life != 0) {
                    int remaining;
                    CardPlayDesertBattle(ownSide, &ownSide->battle[i].battle);
                    count += CARD_VIRGIL_DAMAGE;
                    remaining = CARD_VIRGIL_DAMAGE - 1;
                    do {
                        remaining--;
                        CardPlayCostCard(otherSide);
                    } while (remaining >= 0);
                }
            }
            if (count != 0) {
                CardSetEffect(side, work, CARD_EFFECT_HIT_NUMBER, count, CARD_SE_HIT, 0, 0);
                CardSetEffect(side, work, CARD_EFFECT_DECK_BURST, 0, 0, 0, 0);
            }
            complete = 1;
            break;
        }
        }
        break;
    }
    case CARD_COMMAND_INVOKE: {
        int i;
        int lifeLoss;
        for (i = 0; i < CARD_OPERATION_SLOTS; i++) {
            if (ownSide->operations[i] >= 0)
                CardPlayDesertOperation(ownSide, i);
            if (otherSide->operations[i] >= 0)
                CardPlayDesertOperation(otherSide, i);
        }
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (ownSide->disposal[i].battle.layers[0].cardId >= 0) {
                lifeLoss = -ownSide->disposal[i].battle.layers[0].life;
                ownSide->disposal[i].battle.layers[0].life = 0;
                CardSetEffect(side, work, CARD_EFFECT_SWIRL, 0, 0, ownSide->disposal[i].battle.position, 0);
                CardSetEffect(side, work, CARD_EFFECT_DAMAGE_NUMBER, lifeLoss, 0,
                              ownSide->disposal[i].battle.position, &ownSide->disposal[i].battle);
            }
            if (otherSide->disposal[i].battle.layers[0].cardId >= 0) {
                lifeLoss = -otherSide->disposal[i].battle.layers[0].life;
                otherSide->disposal[i].battle.layers[0].life = 0;
                CardSetEffect(opponentSide, work, CARD_EFFECT_SWIRL, 0, 0,
                              otherSide->disposal[i].battle.position, 0);
                CardSetEffect(opponentSide, work, CARD_EFFECT_DAMAGE_NUMBER, lifeLoss, 0,
                              otherSide->disposal[i].battle.position,
                              &otherSide->disposal[i].battle);
            }
        }
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (ownSide->battle[i].battle.layers[0].cardId >= 0) {
                lifeLoss = -ownSide->battle[i].battle.layers[0].life;
                ownSide->battle[i].battle.layers[0].life = 0;
                CardSetEffect(side, work, CARD_EFFECT_SWIRL, 0, 0,
                              ownSide->battle[i].battle.position, 0);
                CardSetEffect(side, work, CARD_EFFECT_DAMAGE_NUMBER, lifeLoss, 0,
                              ownSide->battle[i].battle.position, &ownSide->disposal[i].battle);
            }
            if (otherSide->battle[i].battle.layers[0].cardId >= 0) {
                lifeLoss = -otherSide->battle[i].battle.layers[0].life;
                otherSide->battle[i].battle.layers[0].life = 0;
                CardSetEffect(opponentSide, work, CARD_EFFECT_SWIRL, 0, 0,
                              otherSide->battle[i].battle.position, 0);
                CardSetEffect(opponentSide, work, CARD_EFFECT_DAMAGE_NUMBER, lifeLoss, 0,
                              otherSide->battle[i].battle.position, &otherSide->battle[i].battle);
            }
        }
        complete = 1;
        xglSoundEffectNormalID(CARD_SE_HIT, 0);
        break;
    }
    case CARD_COMMAND_PHASE_CANNON: {
        int lifeLoss;
        for (destructionIndex = 0; destructionIndex < CARD_BOARD_SLOTS; destructionIndex++) {
            if (ownSide->battle[destructionIndex].battle.layers[0].cardId >= 0 &&
                (CardDefinitionsFor(work)[ownSide->battle[destructionIndex].battle.layers[0].cardId].flags & (CARD_ATTR_GNOSIS | CARD_ATTR_MECH))) {
                lifeLoss = -ownSide->battle[destructionIndex].battle.layers[0].life;
                ownSide->battle[destructionIndex].battle.layers[0].life = 0;
                CardSetEffect(side, work, CARD_EFFECT_SWIRL, 0, 0, ownSide->battle[destructionIndex].battle.position, 0);
                CardSetEffect(side, work, CARD_EFFECT_DAMAGE_NUMBER, lifeLoss, 0, ownSide->battle[destructionIndex].battle.position,
                              &ownSide->disposal[destructionIndex].battle);
            }
            if (otherSide->battle[destructionIndex].battle.layers[0].cardId >= 0 &&
                (CardDefinitionsFor(work)[otherSide->battle[destructionIndex].battle.layers[0].cardId].flags & (CARD_ATTR_GNOSIS | CARD_ATTR_MECH))) {
                lifeLoss = -otherSide->battle[destructionIndex].battle.layers[0].life;
                otherSide->battle[destructionIndex].battle.layers[0].life = 0;
                CardSetEffect(opponentSide, work, CARD_EFFECT_SWIRL, 0, 0, otherSide->battle[destructionIndex].battle.position, 0);
                CardSetEffect(opponentSide, work, CARD_EFFECT_DAMAGE_NUMBER, lifeLoss, 0,
                              otherSide->battle[destructionIndex].battle.position,
                              &otherSide->battle[destructionIndex].battle);
            }
        }
        complete = 1;
        xglSoundEffectNormalID(CARD_SE_HIT, 0);
        break;
    }
    case CARD_COMMAND_NOTICE: {
        int disposalTargetZone;
        int battleTargetZone;
        disposalTargetZone = CARD_ZONE_SIDE1_DISPOSAL;
        battleTargetZone = CARD_ZONE_SIDE1_BATTLE;
        if (side != 0) {
            disposalTargetZone = CARD_ZONE_SIDE0_DISPOSAL;
            battleTargetZone = CARD_ZONE_SIDE0_BATTLE;
        }
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetPermanentMess(work, CGP_PERM_MESS_SELECT_DISCARD_WEAPON);
            CardCursorPassive(CardCursorFor(work));
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (otherSide->disposal[i].battle.layers[0].cardId >= 0 && otherSide->disposal[i].battle.layers[1].cardId >= 0) {
                    work_16.zone = disposalTargetZone;
                    work_16.index = i;
                    CardCursorActive(CardCursorFor(work), &work_16, CARD_CURSOR_SELECTABLE);
                }
            }
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (otherSide->battle[i].battle.layers[0].cardId >= 0 && otherSide->battle[i].battle.layers[1].cardId >= 0) {
                    work_16.zone = battleTargetZone;
                    work_16.index = i;
                    CardCursorActive(CardCursorFor(work), &work_16, CARD_CURSOR_SELECTABLE);
                }
            }
            CardCommandAdvance(work);
            break;
        case 1:
            CGPCursorMove(side, work);
            CGPCursor2Pos(&work_16, work);
            if (pressed & PAD_CIRCLE) {
                if (work_16.cardId >= 0 &&
                    (disposalTargetZone == work_16.zone || battleTargetZone == work_16.zone)) {
                    CardLayerStack *card = &otherSide->battle[work_16.index].battle;
                    if (disposalTargetZone == work_16.zone)
                        card = &otherSide->disposal[work_16.index].battle;
                    if (card->layers[1].cardId >= 0) {
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                        CardCommandAdvance(work);
                    } else {
                        xglSoundEffectNormalID(SE_BUZZER, 0);
                    }
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            } else if (pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_BUZZER, 0);
            }
            break;
        case 2: {
            CardLayerStack *stack;
            int junkCount;
            int layerIndex;
            stack = &otherSide->battle[work_16.index].battle;
            if (disposalTargetZone == work_16.zone)
                stack = &otherSide->disposal[work_16.index].battle;
            if (stack->layers[1].cardId != CARD_ID_SHIELD) {
                stack->power = CardDefinitionsFor(work)[stack->layers[0].cardId].basePower;
                stack->powerLimit = CardDefinitionsFor(work)[stack->layers[0].cardId].basePower;
            }
            junkCount = CardPlayJunkCnt(otherSide);
            for (layerIndex = 1; layerIndex < CARD_STACK_LAYERS; layerIndex++) {
                if (stack->layers[layerIndex].cardId >= 0) {
                    otherSide->junk[junkCount++] = stack->layers[layerIndex].cardId;
                    stack->layers[layerIndex].life = 0;
                    stack->layers[layerIndex].powerPlus = 0;
                    stack->layers[layerIndex].cardId = -1;
                }
            }
            CardSetEffect(opponentSide, work, CARD_EFFECT_SWIRL, 0, CARD_SE_REGEN, stack->position, 0);
            CardCursorPassive(CardCursorFor(work));
            complete = 1;
            break;
        }
        }
        break;
    }
    case CARD_COMMAND_RESOLUTION: {
        int opponentJunkCount;
        int layerIndex;
        ownJunkCount = CardPlayJunkCnt(ownSide);
        opponentJunkCount = CardPlayJunkCnt(otherSide);
        /* Walk corresponding slots together; append only occupied layers to
         * each player's junk pile. The base layer at index zero stays in place. */
        for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
            if (ownSide->disposal[slotIndex].battle.layers[1].cardId >= 0) {
                if (ownSide->disposal[slotIndex].battle.layers[1].cardId != CARD_ID_SHIELD) {
                    ownSide->disposal[slotIndex].battle.power = CardDefinitionsFor(work)[ownSide->disposal[slotIndex].battle.layers[0].cardId].basePower;
                    ownSide->disposal[slotIndex].battle.powerLimit = CardDefinitionsFor(work)[ownSide->disposal[slotIndex].battle.layers[0].cardId].basePower;
                }
                for (layerIndex = 1; layerIndex < CARD_STACK_LAYERS; layerIndex++) {
                    if (ownSide->disposal[slotIndex].battle.layers[layerIndex].cardId >= 0) {
                        CardSetEffect(side, work, CARD_EFFECT_SWIRL, 0, 0, ownSide->disposal[slotIndex].battle.position, 0);
                        ownSide->junk[ownJunkCount++] = ownSide->disposal[slotIndex].battle.layers[layerIndex].cardId;
                        ownSide->disposal[slotIndex].battle.layers[layerIndex].life = 0;
                        ownSide->disposal[slotIndex].battle.layers[layerIndex].powerPlus = 0;
                        ownSide->disposal[slotIndex].battle.layers[layerIndex].cardId = -1;
                    }
                }
            }
            if (otherSide->disposal[slotIndex].battle.layers[1].cardId >= 0) {
                if (otherSide->disposal[slotIndex].battle.layers[1].cardId != CARD_ID_SHIELD) {
                    otherSide->disposal[slotIndex].battle.power = CardDefinitionsFor(work)[otherSide->disposal[slotIndex].battle.layers[0].cardId].basePower;
                    otherSide->disposal[slotIndex].battle.powerLimit = CardDefinitionsFor(work)[otherSide->disposal[slotIndex].battle.layers[0].cardId].basePower;
                }
                for (layerIndex = 1; layerIndex < CARD_STACK_LAYERS; layerIndex++) {
                    if (otherSide->disposal[slotIndex].battle.layers[layerIndex].cardId >= 0) {
                        CardSetEffect(opponentSide, work, CARD_EFFECT_SWIRL, 0, 0, otherSide->disposal[slotIndex].battle.position, 0);
                        otherSide->junk[opponentJunkCount++] = otherSide->disposal[slotIndex].battle.layers[layerIndex].cardId;
                        otherSide->disposal[slotIndex].battle.layers[layerIndex].life = 0;
                        otherSide->disposal[slotIndex].battle.layers[layerIndex].powerPlus = 0;
                        otherSide->disposal[slotIndex].battle.layers[layerIndex].cardId = -1;
                    }
                }
            }
        }
        for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
            if (ownSide->battle[slotIndex].battle.layers[1].cardId >= 0) {
                if (ownSide->battle[slotIndex].battle.layers[1].cardId != CARD_ID_SHIELD) {
                    ownSide->battle[slotIndex].battle.power = CardDefinitionsFor(work)[ownSide->battle[slotIndex].battle.layers[0].cardId].basePower;
                    ownSide->battle[slotIndex].battle.powerLimit = CardDefinitionsFor(work)[ownSide->battle[slotIndex].battle.layers[0].cardId].basePower;
                }
                for (layerIndex = 1; layerIndex < CARD_STACK_LAYERS; layerIndex++) {
                    if (ownSide->battle[slotIndex].battle.layers[layerIndex].cardId >= 0) {
                        CardSetEffect(side, work, CARD_EFFECT_SWIRL, 0, 0, ownSide->battle[slotIndex].battle.position, 0);
                        ownSide->junk[ownJunkCount++] = ownSide->battle[slotIndex].battle.layers[layerIndex].cardId;
                        ownSide->battle[slotIndex].battle.layers[layerIndex].life = 0;
                        ownSide->battle[slotIndex].battle.layers[layerIndex].powerPlus = 0;
                        ownSide->battle[slotIndex].battle.layers[layerIndex].cardId = -1;
                    }
                }
            }
            if (otherSide->battle[slotIndex].battle.layers[1].cardId >= 0) {
                if (otherSide->battle[slotIndex].battle.layers[1].cardId != CARD_ID_SHIELD) {
                    otherSide->battle[slotIndex].battle.power = CardDefinitionsFor(work)[otherSide->battle[slotIndex].battle.layers[0].cardId].basePower;
                    otherSide->battle[slotIndex].battle.powerLimit = CardDefinitionsFor(work)[otherSide->battle[slotIndex].battle.layers[0].cardId].basePower;
                }
                for (layerIndex = 1; layerIndex < CARD_STACK_LAYERS; layerIndex++) {
                    if (otherSide->battle[slotIndex].battle.layers[layerIndex].cardId >= 0) {
                        CardSetEffect(opponentSide, work, CARD_EFFECT_SWIRL, 0, 0, otherSide->battle[slotIndex].battle.position, 0);
                        otherSide->junk[opponentJunkCount++] = otherSide->battle[slotIndex].battle.layers[layerIndex].cardId;
                        otherSide->battle[slotIndex].battle.layers[layerIndex].life = 0;
                        otherSide->battle[slotIndex].battle.layers[layerIndex].powerPlus = 0;
                        otherSide->battle[slotIndex].battle.layers[layerIndex].cardId = -1;
                    }
                }
            }
        }
        complete = 1;
        xglSoundEffectNormalID(CARD_SE_REGEN, 0);
        break;
    }
    case CARD_COMMAND_RECYCLER:
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetPermanentMess(work, CGP_PERM_MESS_SELECT_ADD_TO_HAND);
            CardCommandAdvance(work);
            break;
        case 1: {
            int count = CardPlayJunkCnt(ownSide);
            if (pressed & PAD_RIGHT) {
                NowNo_17++;
                xglSoundEffectNormalID(SE_CURSOR, 0);
                if (NowNo_17 >= count)
                    NowNo_17 = count - 1;
            }
            if (pressed & PAD_LEFT) {
                NowNo_17--;
                xglSoundEffectNormalID(SE_CURSOR, 0);
                if (NowNo_17 < 0)
                    NowNo_17 = 0;
            }
            if (pressed & PAD_CIRCLE) {
                int cardId = ownSide->junk[NowNo_17];
                if (CardDefinitionsFor(work)[cardId].kind == CARD_KIND_BATTLE &&
                    (CardDefinitionsFor(work)[cardId].flags & (CARD_ATTR_MECH | CARD_ATTR_MECH_WEAPON | CARD_ATTR_HUMAN_WEAPON))) {
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                    CardCommandAdvance(work);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            } else if (pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_BUZZER, 0);
            }
            CGCPJunkDispSub(ownSide, NowNo_17);
            break;
        }
        case 2:
            ownSide->hand.cards[CardPlayHandCnt(&ownSide->hand)] = ownSide->junk[NowNo_17];
            ownSide->junk[NowNo_17] = -1;
            CardPlayCleanJunk(ownSide);
            CardCommandAdvance(work);
            break;
        case 3:
            complete = 1;
            break;
        }
        break;
    case CARD_COMMAND_REVIVE_DX:
        switch (CardCommandStep(work)) {
        case 0:
            CardCommandAdvance(work);
            lpcnt_19 = 1;
            CGPSetPermanentMess(work, CGP_PERM_MESS_SELECT_ADD_TO_HAND);
            break;
        case 1: {
            int count = CardPlayJunkCnt(ownSide);
            if (pressed & PAD_RIGHT) {
                NowNo_18++;
                xglSoundEffectNormalID(SE_CURSOR, 0);
                if (NowNo_18 >= count)
                    NowNo_18 = count - 1;
            }
            if (pressed & PAD_LEFT) {
                NowNo_18--;
                xglSoundEffectNormalID(SE_CURSOR, 0);
                if (NowNo_18 < 0)
                    NowNo_18 = 0;
            }
            if (pressed & PAD_CIRCLE) {
                if (CardDefinitionsFor(work)[ownSide->junk[NowNo_18]].flags & CARD_ATTR_HUMAN) {
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                    CardCommandAdvance(work);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            } else if (pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_BUZZER, 0);
            }
            CGCPJunkDispSub(ownSide, NowNo_18);
            break;
        }
        case 2:
            ownSide->hand.cards[CardPlayHandCnt(&ownSide->hand)] = ownSide->junk[NowNo_18];
            ownSide->junk[NowNo_18] = -1;
            CardPlayCleanJunk(ownSide);
            CardCommandAdvance(work);
            break;
        case 3:
            if (lpcnt_19 != 0) {
                lpcnt_19--;
                CardCommandSetStep(work, 1);
            } else {
                CardCommandSetStep(work, CARD_STEP_FINISH);
            }
            break;
        case CARD_STEP_FINISH:
            complete = 1;
            break;
        }
        break;
    case CARD_COMMAND_STIM_DX: {
        int battleZone;
        battleZone = CARD_ZONE_SIDE0_BATTLE;
        if (side != 0)
            battleZone = CARD_ZONE_SIDE1_BATTLE;
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetPermanentMess(work, CGP_PERM_MESS_SELECT_DOUBLE_ATK);
            CardCursorPassive(CardCursorFor(work));
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (ownSide->battle[i].battle.layers[0].cardId >= 0 && CCC107ChkSub(&ownSide->battle[i].battle) == 0) {
                    work_20.zone = battleZone;
                    work_20.index = i;
                    CardCursorActive(CardCursorFor(work), &work_20, CARD_CURSOR_SELECTABLE);
                }
            }
            CardCommandAdvance(work);
            break;
        case 1:
            CGPCursorMove(side, work);
            CGPCursor2Pos(&work_20, work);
            if (pressed & PAD_CIRCLE) {
                if (work_20.cardId >= 0 && battleZone == work_20.zone &&
                    CCC107ChkSub(&ownSide->battle[work_20.index].battle) == 0) {
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                    CardCommandAdvance(work);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            } else if (pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_BUZZER, 0);
            }
            break;
        case 2:
            complete = 1;
            CCC26ExecSub(side, work, ownSide, &work_20);
            break;
        default:
            break;
        }
        break;
    }
    case CARD_COMMAND_SIMULATION: {
        int junkIndex = CardPlayJunkCnt(ownSide) - 1;
        int operation;
        for (operation = 0; operation < CARD_OPERATION_SLOTS; operation++) {
            if (ownSide->operations[operation] < 0)
                break;
        }
        complete = 1;
        ownSide->operations[operation] = ownSide->junk[junkIndex];
        ownSide->operationDuration[operation] = CardDefinitionsFor(work)[ownSide->junk[junkIndex]].operationDuration;
        ownSide->junk[junkIndex] = -1;
        break;
    }
    case CARD_COMMAND_WAR_CASUALTY: {
        int disposeZone;
        int battleZone;
        disposeZone = CARD_ZONE_SIDE0_DISPOSAL;
        battleZone = CARD_ZONE_SIDE0_BATTLE;
        if (side != 0) {
            disposeZone = CARD_ZONE_SIDE1_DISPOSAL;
            battleZone = CARD_ZONE_SIDE1_BATTLE;
        }
        switch (CardCommandStep(work)) {
        case 0: {
            int disposeCount;
            CGPSetPermanentMess(work, CGP_PERM_MESS_SELECT_DISCARDED);
            CardCursorPassive(CardCursorFor(work));
            disposeCount = CardPlayDisposeCnt(ownSide);
            if (disposeCount + CardPlayBattleCnt(ownSide) == 0) {
                complete = 1;
            } else {
                for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                    if (ownSide->disposal[i].battle.layers[0].cardId >= 0) {
                        work_21.zone = disposeZone;
                        work_21.index = i;
                        CardCursorActive(CardCursorFor(work), &work_21, CARD_CURSOR_SELECTABLE);
                    }
                }
                for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                    if (ownSide->battle[i].battle.layers[0].cardId >= 0) {
                        work_21.zone = battleZone;
                        work_21.index = i;
                        CardCursorActive(CardCursorFor(work), &work_21, CARD_CURSOR_SELECTABLE);
                    }
                }
            }
            CardCommandAdvance(work);
            break;
        }
        case 1:
            CGPCursorMove(side, work);
            CGPCursor2Pos(&work_21, work);
            if (pressed & PAD_CIRCLE) {
                if ((disposeZone == work_21.zone || battleZone == work_21.zone) &&
                    work_21.cardId >= 0) {
                    CardCursorActive(CardCursorFor(work), &work_21, CARD_CURSOR_CHOSEN);
                    CardCommandAdvance(work);
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            } else if (pressed & PAD_CROSS) {
                complete = 1;
                CardCursorPassive(CardCursorFor(work));
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
            break;
        case 2:
            CGPDispErrorMess(side, CGP_MESS_SELECT_DISCARD);
            if (pressed & PAD_CIRCLE) {
                xglSoundEffectNormalID(SE_DECIDE, 0);
                CardCommandAdvance(work);
            } else if (pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_CANCEL, 0);
                CardCommandSetStep(work, 0);
            }
            break;
        case 3:
            if (disposeZone == work_21.zone)
                ownSide->disposal[work_21.index].battle.layers[0].life = 0;
            else
                ownSide->battle[work_21.index].battle.layers[0].life = 0;
            CardCommandAdvance(work);
            lp_22 = CARD_WAR_CASUALTY_RECOVERY;
            xglSoundEffectNormalID(CARD_SE_REGEN, 0);
            break;
        case 4: {
            int remainingRecovery;
            CardPlayRecavery(ownSide);
            CardSetEffect(side, work, CARD_EFFECT_SWIRL, 0, 0, 0, 0);
            CardSetEffect(side, work, CARD_EFFECT_NUMBER, 1, 0, 0, 0);
            remainingRecovery = lp_22 - 1;
            CardCommandAdvance(work);
            lp_22 = remainingRecovery;
            break;
        }
        case 5:
            if (lp_22 > 0)
                CardCommandRewind(work);
            else
                CardCommandSetStep(work, 0);
            break;
        }
        break;
    }
    case CARD_COMMAND_X_BUSTER: {
        for (damageIndex = 0; damageIndex < CARD_BOARD_SLOTS; damageIndex++) {
            if (ownSide->disposal[damageIndex].battle.layers[0].cardId >= 0) {
                ownSide->disposal[damageIndex].battle.layers[0].life--;
                CardSetEffect(side, work, CARD_EFFECT_SWIRL, 0, 0, ownSide->disposal[damageIndex].battle.position, 0);
                CardSetEffect(side, work, CARD_EFFECT_DAMAGE_NUMBER, -1, 0, ownSide->disposal[damageIndex].battle.position,
                              &ownSide->disposal[damageIndex].battle);
                if (ownSide->disposal[damageIndex].battle.layers[0].life < 0)
                    ownSide->disposal[damageIndex].battle.layers[0].life = 0;
            }
            if (otherSide->disposal[damageIndex].battle.layers[0].cardId >= 0) {
                otherSide->disposal[damageIndex].battle.layers[0].life--;
                CardSetEffect(opponentSide, work, CARD_EFFECT_SWIRL, 0, 0, otherSide->disposal[damageIndex].battle.position, 0);
                CardSetEffect(opponentSide, work, CARD_EFFECT_DAMAGE_NUMBER, -1, 0, otherSide->disposal[damageIndex].battle.position,
                              &otherSide->disposal[damageIndex].battle);
                if (otherSide->disposal[damageIndex].battle.layers[0].life < 0)
                    otherSide->disposal[damageIndex].battle.layers[0].life = 0;
            }
        }
        for (damageIndex = 0; damageIndex < CARD_BOARD_SLOTS; damageIndex++) {
            if (ownSide->battle[damageIndex].battle.layers[0].cardId >= 0) {
                ownSide->battle[damageIndex].battle.layers[0].life--;
                CardSetEffect(side, work, CARD_EFFECT_SWIRL, 0, 0, ownSide->battle[damageIndex].battle.position, 0);
                CardSetEffect(side, work, CARD_EFFECT_DAMAGE_NUMBER, -1, 0, ownSide->battle[damageIndex].battle.position,
                              &ownSide->battle[damageIndex].battle);
                if (ownSide->battle[damageIndex].battle.layers[0].life < 0)
                    ownSide->battle[damageIndex].battle.layers[0].life = 0;
            }
            if (otherSide->battle[damageIndex].battle.layers[0].cardId >= 0) {
                otherSide->battle[damageIndex].battle.layers[0].life--;
                CardSetEffect(opponentSide, work, CARD_EFFECT_SWIRL, 0, 0, otherSide->battle[damageIndex].battle.position, 0);
                CardSetEffect(opponentSide, work, CARD_EFFECT_DAMAGE_NUMBER, -1, 0, otherSide->battle[damageIndex].battle.position,
                              &otherSide->battle[damageIndex].battle);
                if (otherSide->battle[damageIndex].battle.layers[0].life < 0)
                    otherSide->battle[damageIndex].battle.layers[0].life = 0;
            }
        }
        complete = 1;
        xglSoundEffectNormalID(CARD_SE_HIT, 0);
        break;
    }
    case CARD_COMMAND_CHARGE: {
        int i;
        int disposeZone;
        int battleZone;
        disposeZone = CARD_ZONE_SIDE1_DISPOSAL;
        battleZone = CARD_ZONE_SIDE1_BATTLE;
        if (side != 0) {
            disposeZone = CARD_ZONE_SIDE0_DISPOSAL;
            battleZone = CARD_ZONE_SIDE0_BATTLE;
        }
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetPermanentMess(work, CGP_PERM_MESS_SELECT_DOWN);
            CardCursorPassive(CardCursorFor(work));
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (otherSide->disposal[i].battle.layers[0].cardId >= 0) {
                    work_23.zone = disposeZone;
                    work_23.index = i;
                    CardCursorActive(CardCursorFor(work), &work_23, CARD_CURSOR_SELECTABLE);
                }
            }
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (otherSide->battle[i].battle.layers[0].cardId >= 0) {
                    work_23.zone = battleZone;
                    work_23.index = i;
                    CardCursorActive(CardCursorFor(work), &work_23, CARD_CURSOR_SELECTABLE);
                }
            }
            CardCommandAdvance(work);
            break;
        case 1:
            CGPCursorMove(side, work);
            CGPCursor2Pos(&work_23, work);
            if (pressed & PAD_CIRCLE) {
                if ((disposeZone == work_23.zone || battleZone == work_23.zone) && work_23.cardId >= 0) {
                    CardCommandAdvance(work);
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            } else if (pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_BUZZER, 0);
            }
            break;
        case 2:
            complete = 1;
            CCC30ExecSub(opponentSide, work,
                         CardSelectBattle(otherSide, work_23.index,
                                          disposeZone == work_23.zone));
            CardCursorPassive(CardCursorFor(work));
            break;
        }
        break;
    }
    case CARD_COMMAND_MILTIAN_CONFLICT: {
        int i;
        for (i = 2; i < CARD_BOARD_SLOTS; i++) {
            if (otherSide->battle[i].battle.layers[0].cardId >= 0) {
                CardSetEffect(opponentSide, work, CARD_EFFECT_SWIRL, 0, CARD_SE_REGEN,
                              otherSide->battle[i].battle.position, 0);
                otherSide->battle[i].battle.flags |= CARD_STACK_FLAG_FROZEN;
            }
        }
        complete = 1;
        break;
    }
    case CARD_COMMAND_DESTINY: {
        int battleZone;
        battleZone = CARD_ZONE_SIDE0_BATTLE;
        if (side != 0)
            battleZone = CARD_ZONE_SIDE1_BATTLE;
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetPermanentMess(work, CGP_PERM_MESS_SELECT_REPLACED);
            CardCommandAdvance(work);
        case 1:
            CardCursorPassive(CardCursorFor(work));
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                const s16 *slotCardId = &ownSide->battle[i].battle.layers[0].cardId;
                int cardId = *slotCardId;
                if (cardId >= 0 && (CardDefinitionsFor(work)[cardId].flags & CARD_ATTR_HUMAN)) {
                    work_25.zone = battleZone;
                    work_25.index = i;
                    CardCursorActive(CardCursorFor(work), &work_25, CARD_CURSOR_SELECTABLE);
                }
            }
            CardCommandAdvance(work);
            break;
        case 2:
            CGPCursorMove(side, work);
            CGPCursor2Pos(&work_25, work);
            if (pressed & PAD_CIRCLE) {
                if (battleZone == work_25.zone && work_25.cardId >= 0 &&
                    CardDefinitionsFor(work)[work_25.index].kind == CARD_KIND_BATTLE &&
                    CardDefinitionsFor(work)[work_25.cardId].flags == CARD_ATTR_HUMAN) {
                    CardCursorActive(CardCursorFor(work), &work_25, CARD_CURSOR_CHOSEN);
                    CardCommandAdvance(work);
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            } else if (pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_BUZZER, 0);
            }
            break;
        case 3:
            CGPDispErrorMess(side, CGP_MESS_SELECT_DISCARD);
            if (pressed & PAD_CIRCLE) {
                u16 nextStep;
                xglSoundEffectNormalID(SE_DECIDE, 0);
                nextStep = work->command.step;
                nextStep++;
                NowNo_24 = 0;
                CardCommandSetStep(work, nextStep);
            }
            if (pressed & PAD_CROSS) {
                CardCursorPassive(CardCursorFor(work));
                xglSoundEffectNormalID(SE_CANCEL, 0);
                CardCommandSetStep(work, 1);
            }
            break;
        case 4: {
            int junkCount = CardPlayJunkCnt(ownSide);
            if (repeated & PAD_RIGHT) {
                if (NowNo_24 < junkCount - 1) {
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                    NowNo_24++;
                }
            } else if ((repeated & PAD_LEFT) && NowNo_24 > 0) {
                xglSoundEffectNormalID(SE_CURSOR, 0);
                NowNo_24--;
            }
            if (pressed & PAD_CIRCLE) {
                int cardId = ownSide->junk[NowNo_24];
                if ((CardDefinitionsFor(work)[cardId].flags & CARD_ATTR_HUMAN) &&
                    CardPlayIdentityCheck(work, ownSide,
                                          CardDefinitionsFor(work)[cardId].identity) == 0) {
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                    CardCommandAdvance(work);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            } else if (pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_CANCEL, 0);
                CardCommandSetStep(work, 1);
            }
            CGCPJunkDispSub(ownSide, NowNo_24);
            break;
        }
        case 5:
            CGPDispErrorMess(side, CGP_MESS_SELECT_CARD);
            if (pressed & PAD_CIRCLE) {
                xglSoundEffectNormalID(SE_DECIDE, 0);
                CardCommandAdvance(work);
            } else if (pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_CANCEL, 0);
                CardCommandSetStep(work, 4);
            }
            CGCPJunkDispSub(ownSide, NowNo_24);
            break;
        case 6:
            complete = 1;
            CardPlayDesertBattle(ownSide, &ownSide->battle[work_25.index].battle);
            ownSide->battle[work_25.index].battle.layers[0].cardId = ownSide->junk[NowNo_24];
            ownSide->junk[NowNo_24] = -1;
            CardPlayCleanJunk(ownSide);
            CardCursorPassive(CardCursorFor(work));
            break;
        }
        break;
    }
    case CARD_COMMAND_TERRITORY_SHIFT_TRIGGER:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = CARD_COMMAND_TERRITORY_SHIFT;
            queued[CARD_QUEUE_PHASE] = CARD_PHASE_BATTLE;
            if (side != 0)
                queued[CARD_QUEUE_PHASE] = CARD_PHASE_BATTLE_SIDE1;
        }
        complete = 1;
        break;
    case CARD_COMMAND_TERRITORY_SHIFT: {
        int count = CardPlayHandCnt(&otherSide->hand);
        int step = CardCommandStep(work);
        switch (step) {
        case 0:
            if (count != 0) {
                u16 nextStep;
                int *selectedCard;
                CGPSetErrorMessPlus(opponentSide, work, CGP_ERR_USE_CARD_EFFECT, CARD_ID_TERRITORY_SHIFT);
                selectedCard = &NowNo_26;
                nextStep = work->command.step;
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
            if (opponentRepeated & PAD_LEFT) {
                if (NowNo_26 > 0) {
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                    NowNo_26--;
                }
            } else if (opponentRepeated & PAD_RIGHT) {
                if (NowNo_26 < count - 1) {
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                    NowNo_26++;
                }
            }
            if (opponentRepeated & PAD_CIRCLE) {
                CardCommandAdvance(work);
                xglSoundEffectNormalID(SE_DECIDE, 0);
            }
            xglFontPrint(143, 213, 0xFFF8, cardPlayDiscardPrompt);
            scale[0] = 1.5f;
            scale[1] = scale[2] = scale[3] = 1.0f;
            translation[0] = translation[1] = 0.0f;
            translation[2] = 0.91f;
            translation[3] = 1.0f;
            xglMatrixUnit(matrix);
            xglMatrixTrans(matrix, matrix, translation);
            xglMatrixScale(matrix, matrix, scale);
            nmlModelSetPlace(&matrix);
            nmlModelSetTexture(CARD_PROMPT_TEXTURE);
            nmlModelEntryCard(CARD_PROMPT_MODEL);
            if ((side != 0 && work->rotStageId == 0) ||
                (side == 0 && work->rotStageId != 0)) {
                CGCPHandDispSub2(CardDefinitionsFor(work), otherSide, NowNo_26, 0);
            } else {
                CGCPHandDispSub3(CardDefinitionsFor(work), otherSide, NowNo_26, 0, 0);
            }
            break;
        case 2:
            CGPDispErrorMessPlus(opponentSide, CGP_MESS_SELECT_DISCARD, otherSide->hand.cards[NowNo_26]);
            if (opponentRepeated & PAD_CIRCLE) {
                xglSoundEffectNormalID(SE_DECIDE, 0);
                CardCommandAdvance(work);
                CardPlayDesertCard(otherSide, NowNo_26);
            } else if (opponentRepeated & PAD_CROSS) {
                xglSoundEffectNormalID(SE_DECIDE, 0);
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
    case CARD_COMMAND_TESTAMENT_TRIGGER:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = CARD_COMMAND_TESTAMENT;
            queued[CARD_QUEUE_PHASE] = CARD_PHASE_BATTLE;
            if (side != 0)
                queued[CARD_QUEUE_PHASE] = CARD_PHASE_BATTLE_SIDE1;
        }
        complete = 1;
        break;
    case CARD_COMMAND_TESTAMENT: {
        int i;
        int battleZone;
        if ((side == 0 && CardLowByteAiFlag(flags, CGP_FLAG_SIDE0_AUTO)) ||
            (side != 0 && CardLowByteAiFlag(flags, CGP_FLAG_SIDE1_AUTO)))
            return CardEnemyAnswerCardxx(side, work, CARD_ID_TESTAMENT);
        battleZone = CARD_ZONE_SIDE1_BATTLE;
        if (side != 0)
            battleZone = CARD_ZONE_SIDE0_BATTLE;
        switch (CardCommandStep(work)) {
        case 0:
            CardCursorPassive(CardCursorFor(work));
            if (CardPlayBattleCnt(otherSide) != 0) {
                CGPSetErrorMessPlus(side, work, CGP_ERR_USE_CARD_EFFECT, CARD_ID_TESTAMENT);
                position.zone = battleZone;
                for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                    if (otherSide->battle[i].battle.layers[0].cardId >= 0) {
                        position.index = i;
                        CardCursorActive(CardCursorFor(work), &position, CARD_CURSOR_SELECTABLE);
                    }
                }
                CardCommandAdvance(work);
            } else {
                complete = 1;
            }
            break;
        case 1:
            CGPSetPermanentMess(work, CGP_PERM_MESS_SELECT_DOWN);
            CGPCursorMove(side, work);
            CGPCursor2Pos(&work_27, work);
            if (pressed & PAD_CIRCLE) {
                if (battleZone == work_27.zone && work_27.cardId >= 0) {
                    CardCommandAdvance(work);
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            }
            break;
        case 2:
            CGPDispErrorMess(side, CGP_MESS_SELECT_CARD);
            if (pressed & PAD_CIRCLE) {
                otherSide->battle[work_27.index].battle.flags |= CARD_STACK_FLAG_FROZEN;
                xglSoundEffectNormalID(SE_DECIDE, 0);
                CardCommandAdvance(work);
            } else if (pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_CANCEL, 0);
                CardCommandRewind(work);
            }
            break;
        case 3:
            complete = 1;
            break;
        }
        break;
    }
    case CARD_COMMAND_ALLEN_TRIGGER:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = CARD_COMMAND_ALLEN;
            queued[CARD_QUEUE_PHASE] = CARD_PHASE_BATTLE;
            if (side != 0)
                queued[CARD_QUEUE_PHASE] = CARD_PHASE_BATTLE_SIDE1;
        }
        complete = 1;
        break;
    case CARD_COMMAND_ALLEN: {
        int i;
        int targetZone;
        if ((side == 0 && CardLowByteAiFlag(flags, CGP_FLAG_SIDE0_AUTO)) ||
            (side != 0 && CardLowByteAiFlag(flags, CGP_FLAG_SIDE1_AUTO)))
            return CardEnemyAnswerCardxx(side, work, CARD_ID_ALLEN);
        targetZone = CARD_ZONE_SIDE1_BATTLE;
        if (side != 0)
            targetZone = CARD_ZONE_SIDE0_BATTLE;
        switch (CardCommandStep(work)) {
        case 0:
            CardCursorPassive(CardCursorFor(work));
            if (CardPlayBattleCnt(otherSide) == 0) {
                complete = 1;
            } else if (CardPlayDisposeCnt(otherSide) == CARD_BOARD_SLOTS) {
                complete = 1;
            } else {
                CGPSetErrorMessPlus(side, work, CGP_ERR_USE_CARD_EFFECT, CARD_ID_ALLEN);
                position.zone = targetZone;
                for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                    if (otherSide->battle[i].battle.layers[0].cardId >= 0) {
                        position.index = i;
                        CardCursorActive(CardCursorFor(work), &position, CARD_CURSOR_SELECTABLE);
                    }
                }
                CardCommandAdvance(work);
            }
            break;
        case 1:
            CGPSetPermanentMess(work, CGP_PERM_MESS_SELECT_RETURN_TO_HAND);
            CGPCursorMove(side, work);
            CGPCursor2Pos(&work_28, work);
            if (pressed & PAD_CIRCLE) {
                if (targetZone == work_28.zone && work_28.cardId >= 0) {
                    CardCommandAdvance(work);
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            }
            break;
        case 2:
            CGPDispErrorMess(side, CGP_MESS_SELECT_CARD);
            if (pressed & PAD_CIRCLE) {
                CCC07ExecSub(work, otherSide, &work_28);
                xglSoundEffectNormalID(SE_DECIDE, 0);
                CardCommandAdvance(work);
            } else if (pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_CANCEL, 0);
                CardCommandRewind(work);
            }
            break;
        case 3:
            complete = 1;
            break;
        }
        break;
    }
    case CARD_COMMAND_TONY_TRIGGER:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = CARD_COMMAND_TONY;
            queued[CARD_QUEUE_PHASE] = CARD_PHASE_BATTLE;
            if (side != 0)
                queued[CARD_QUEUE_PHASE] = CARD_PHASE_BATTLE_SIDE1;
        }
        complete = 1;
        break;
    case CARD_COMMAND_TONY: {
        int i;
        int targetZone;
        if ((side == 0 && CardLowByteAiFlag(flags, CGP_FLAG_SIDE0_AUTO)) ||
            (side != 0 && CardLowByteAiFlag(flags, CGP_FLAG_SIDE1_AUTO)))
            return CardEnemyAnswerCardxx(side, work, CARD_ID_TONY);
        targetZone = CARD_ZONE_SIDE1_DISPOSAL;
        if (side != 0)
            targetZone = CARD_ZONE_SIDE0_DISPOSAL;
        switch (CardCommandStep(work)) {
        case 0:
            CardCursorPassive(CardCursorFor(work));
            if (CardPlayDisposeCnt(otherSide) != 0) {
                CGPSetErrorMessPlus(side, work, CGP_ERR_USE_CARD_EFFECT, CARD_ID_TONY);
                position.zone = targetZone;
                for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                    if (otherSide->disposal[i].battle.layers[0].cardId >= 0) {
                        position.index = i;
                        CardCursorActive(CardCursorFor(work), &position, CARD_CURSOR_SELECTABLE);
                    }
                }
                CardCommandAdvance(work);
            } else {
                complete = 1;
            }
            break;
        case 1:
            CGPSetPermanentMess(work, CGP_PERM_MESS_SELECT_DOWN);
            CGPCursorMove(side, work);
            CGPCursor2Pos(&work_29, work);
            if (pressed & PAD_CIRCLE) {
                if (targetZone == work_29.zone && work_29.cardId >= 0) {
                    CardCommandAdvance(work);
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            }
            break;
        case 2:
            CGPDispErrorMess(side, CGP_MESS_SELECT_CARD);
            if (pressed & PAD_CIRCLE) {
                otherSide->disposal[work_29.index].battle.flags |= CARD_STACK_FLAG_FROZEN;
                xglSoundEffectNormalID(SE_DECIDE, 0);
                CardCommandAdvance(work);
            } else if (pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_CANCEL, 0);
                CardCommandRewind(work);
            }
            break;
        case 3:
            complete = 1;
            break;
        }
        break;
    }
    case CARD_COMMAND_MARGULIS_TRIGGER:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = CARD_COMMAND_MARGULIS;
            queued[CARD_QUEUE_PHASE] = CARD_PHASE_BATTLE;
            if (side != 0)
                queued[CARD_QUEUE_PHASE] = CARD_PHASE_BATTLE_SIDE1;
        }
        complete = 1;
        break;
    case CARD_COMMAND_MARGULIS: {
        int i;
        int operationZone;
        if ((side == 0 && CardLowByteAiFlag(flags, CGP_FLAG_SIDE0_AUTO)) ||
            (side != 0 && CardLowByteAiFlag(flags, CGP_FLAG_SIDE1_AUTO)))
            return CardEnemyAnswerCardxx(side, work, CARD_ID_MARGULIS);
        switch (CardCommandStep(work)) {
        case 0:
            if (CardPlayOperationCnt(otherSide) != 0) {
                CGPSetErrorMessPlus(side, work, CGP_ERR_USE_CARD_EFFECT, CARD_ID_MARGULIS);
                CardCursorPassive(CardCursorFor(work));
                position.zone = CARD_ZONE_SIDE1_OPERATION;
                if (side != 0)
                    position.zone = CARD_ZONE_SIDE0_OPERATION;
                for (i = 0; i < CARD_OPERATION_SLOTS; i++) {
                    if (otherSide->operations[i] >= 0) {
                        position.index = i;
                        CardCursorActive(CardCursorFor(work), &position, CARD_CURSOR_SELECTABLE);
                    }
                }
                CGPSetPermanentMess(work, CGP_PERM_MESS_SELECT_DISCARDED);
                CardCommandAdvance(work);
            } else {
                complete = 1;
            }
            break;
        case 1:
            operationZone = CARD_ZONE_SIDE1_OPERATION;
            if (side != 0)
                operationZone = CARD_ZONE_SIDE0_OPERATION;
            CGPCursorMove(side, work);
            CGPCursor2Pos(&work_30, work);
            if (pressed & PAD_CIRCLE) {
                if (operationZone == work_30.zone && work_30.cardId >= 0) {
                    CardCommandAdvance(work);
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            }
            break;
        case 2:
            CGPDispErrorMess(side, CGP_MESS_SELECT_CARD);
            if (pressed & PAD_CIRCLE) {
                CardCommandAdvance(work);
                CCU055ExecSub(otherSide, &work_30);
                xglSoundEffectNormalID(SE_DECIDE, 0);
            } else if (pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_CANCEL, 0);
                CardCommandRewind(work);
            }
            break;
        case 3:
            complete = 1;
            break;
        }
        break;
    }
    case CARD_COMMAND_BUNNIE_TRIGGER:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = CARD_COMMAND_BUNNIE;
            queued[CARD_QUEUE_PHASE] = CARD_PHASE_BATTLE;
            if (side != 0)
                queued[CARD_QUEUE_PHASE] = CARD_PHASE_BATTLE_SIDE1;
        }
        complete = 1;
        break;
    case CARD_COMMAND_BUNNIE: {
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetErrorMessPlus(side, work, CGP_ERR_USE_CARD_EFFECT, CARD_ID_BUNNIE);
            CardCommandAdvance(work);
            break;
        case 1:
            CardCommandAdvance(work);
            break;
        case 2: {
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (work->player.battle[i].battle.layers[0].cardId >= 0)
                    work->player.battle[i].battle.flags |= CARD_STACK_FLAG_FROZEN;
                if (work->enemy.battle[i].battle.layers[0].cardId >= 0)
                    work->enemy.battle[i].battle.flags |= CARD_STACK_FLAG_FROZEN;
            }
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (work->player.disposal[i].battle.layers[0].cardId >= 0)
                    work->player.disposal[i].battle.flags |= CARD_STACK_FLAG_FROZEN;
                if (work->enemy.disposal[i].battle.layers[0].cardId >= 0)
                    work->enemy.disposal[i].battle.flags |= CARD_STACK_FLAG_FROZEN;
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
    case CARD_COMMAND_GAIGNUN_TRIGGER:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = CARD_COMMAND_GAIGNUN;
            queued[CARD_QUEUE_PHASE] = CARD_PHASE_BATTLE;
            if (side != 0)
                queued[CARD_QUEUE_PHASE] = CARD_PHASE_BATTLE_SIDE1;
        }
        complete = 1;
        break;
    case CARD_COMMAND_GAIGNUN: {
        int i;
        switch (CardCommandStep(work)) {
        case 0: {
            int found = 0;
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                s16 *slotCardId = &ownSide->battle[i].battle.layers[0].cardId;
                s16 *otherSlotCardId = &otherSide->battle[i].battle.layers[0].cardId;
                int cardId = *slotCardId;
                if (cardId >= 0 && (CardDefinitionsFor(work)[cardId].flags & CARD_ATTR_REALIAN)) {
                    found = 1;
                    break;
                }
                cardId = *otherSlotCardId;
                if (cardId >= 0 && (CardDefinitionsFor(work)[cardId].flags & CARD_ATTR_REALIAN)) {
                    found = 1;
                    break;
                }
            }
            if (found) {
                CGPSetErrorMessPlus(side, work, CGP_ERR_USE_CARD_EFFECT, CARD_ID_GAIGNUN);
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
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                int cardId = work->player.battle[i].battle.layers[0].cardId;
                if (cardId >= 0 && (CardDefinitionsFor(work)[cardId].flags & CARD_ATTR_REALIAN)) {
                    CardPlayReturnCard(&playerSide->hand,
                                       &work->player.battle[0].battle, i);
                }
                {
                    s16 *otherSlotCardId = &work->enemy.battle[i].battle.layers[0].cardId;
                    cardId = *otherSlotCardId;
                    if (cardId >= 0 && (CardDefinitionsFor(work)[cardId].flags & CARD_ATTR_REALIAN)) {
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
    case CARD_COMMAND_CHERENKOV_TRIGGER: {
        int junkIndex = CardPlayJunkCnt(ownSide) - 1;
        if (junkIndex >= 0) {
            int cardId = ownSide->junk[junkIndex];
            if (CardDefinitionsFor(work)[cardId].kind == CARD_KIND_BATTLE &&
                (CardDefinitionsFor(work)[cardId].flags & (CARD_ATTR_HUMAN | CARD_ATTR_REALIAN | CARD_ATTR_MECH))) {
                queued = CPCSearchCommWk(side, work);
                if (queued != 0) {
                    queued[CARD_QUEUE_COMMAND] = CARD_COMMAND_CHERENKOV;
                    queued[CARD_QUEUE_PHASE] = CARD_PHASE_ADJUST_SIDE1;
                    if (side != 0)
                        queued[CARD_QUEUE_PHASE] = CARD_PHASE_ADJUST;
                    queued[CARD_QUEUE_OPERAND] = operand;
                    queued[CARD_QUEUE_TARGET] = junkIndex;
                }
            }
        }
        complete = 1;
        break;
    }
    case CARD_COMMAND_CHERENKOV:
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetErrorMessPlus(opponentSide, work, CGP_ERR_USE_CARD_EFFECT, CARD_ID_CHERENKOV);
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
            CardPlayCleanJunk(otherSide);
            break;
        }
        }
        break;
    case CARD_COMMAND_VX_9000_BATTLE_TRIGGER:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = CARD_COMMAND_VX_9000;
            queued[CARD_QUEUE_PHASE] = CardCommandPhase(work);
            queued[CARD_QUEUE_OPERAND] = -1;
            queued[CARD_QUEUE_TARGET] = -1;
        }
        complete = 1;
        break;
    case CARD_COMMAND_VX_9000: {
        int ownDisposalZone;
        if ((opponentSide == 0 && CardLowByteAiFlag(flags, CGP_FLAG_SIDE0_AUTO)) ||
            (opponentSide != 0 && CardLowByteAiFlag(flags, CGP_FLAG_SIDE1_AUTO)))
            return CardEnemyAnswerCardxx(opponentSide, work, CARD_ID_TONY);
        ownDisposalZone = CARD_ZONE_SIDE0_DISPOSAL;
        if (side != 0)
            ownDisposalZone = CARD_ZONE_SIDE1_DISPOSAL;
        switch (CardCommandStep(work)) {
        case 0:
            if (CardPlayDisposeCnt(ownSide) != 0) {
                CGPSetErrorMessPlus(opponentSide, work, CGP_ERR_USE_CARD_EFFECT, CARD_ID_VX_9000);
                CardCursorPassive(CardCursorFor(work));
                CGPSetPermanentMess(work, CGP_PERM_MESS_SELECT_DISCARDED);
                CardCursorSetSelectionMode(work, 1);
                work_31.zone = ownDisposalZone;
                for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                    if (ownSide->disposal[i].battle.layers[0].cardId > 0) {
                        work_31.index = i;
                        CardCursorActive(CardCursorFor(work), &work_31, CARD_CURSOR_SELECTABLE);
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
            CGPCursor2Pos(&work_31, work);
            if (opponentPressed & PAD_CIRCLE) {
                if (ownDisposalZone == work_31.zone && work_31.cardId >= 0) {
                    CardCommandAdvance(work);
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            } else if (opponentPressed & PAD_SQUARE) {
                xglSoundEffectNormalID(SE_CANCEL, 0);
                CardCursorSetSide(work, -1);
                complete = 1;
            }
            break;
        case 2:
            CGPDispErrorMess(opponentSide, CGP_MESS_SELECT_CARD);
            if (opponentPressed & PAD_CIRCLE) {
                CardCommandAdvance(work);
                CCU038ExecSub(ownSide, &work_31);
                xglSoundEffectNormalID(SE_DECIDE, 0);
                CardCursorSetSelectionMode(work, 0);
            } else if (opponentPressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_CANCEL, 0);
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
    case CARD_COMMAND_LV_1_SHION:
        switch (CardCommandStep(work)) {
        case 0:
            CardCommandAdvance(work);
            break;
        case 1:
            if (controlledByAi) {
                CardEnemyAnswerSionSearch(side, work);
                complete = 1;
                CGPSetErrorMessPlus(-1, work, CGP_ERR_USE_CARD_EFFECT, CARD_ID_LV_1_SHION);
            } else {
                work->flags = flags | CGP_FLAG_ERROR_PLUS;
                CGPDispErrorMessPlus(side, CGP_MESS_CONFIRM_CARD, CARD_ID_LV_1_SHION);
                if (pressed & PAD_CIRCLE) {
                    u16 nextStep;
                    CGPSetPermanentMess(work, CGP_PERM_MESS_SELECT_LV10_SHION);
                    nextStep = work->command.step;
                    nextStep++;
                    NowNo_32 = 0;
                    CardCommandSetStep(work, nextStep);
                }
                if (pressed & PAD_CROSS)
                    CardCommandSetStep(work, CARD_STEP_FINISH);
            }
            break;
        case 2: {
            int count = CardPlayYamaCnt(ownSide);
            if (repeated & PAD_RIGHT) {
                NowNo_32++;
                xglSoundEffectNormalID(SE_CURSOR, 0);
                if (NowNo_32 >= count)
                    NowNo_32 = count - 1;
            }
            if (repeated & PAD_LEFT) {
                NowNo_32--;
                xglSoundEffectNormalID(SE_CURSOR, 0);
                if (NowNo_32 < 0)
                    NowNo_32 = 0;
            }
            if (pressed & PAD_CIRCLE) {
                if (ownSide->deck[NowNo_32] == CARD_ID_LV_10_SHION) {
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                    CardCommandAdvance(work);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            } else if (pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_CANCEL, 0);
                CardCommandSetStep(work, CARD_STEP_FINISH);
            }
            CGCPYamaDispSub(ownSide, NowNo_32, count, held, 0);
            break;
        }
        case 3:
            ownSide->hand.cards[CardPlayHandCnt(&ownSide->hand)] = ownSide->deck[NowNo_32];
            ownSide->deck[NowNo_32] = -1;
            CardPlayCleanYama(ownSide);
            CardPlayShuffleYama(ownSide);
            CardCommandAdvance(work);
            break;
        case 4:
        case CARD_STEP_FINISH:
            complete = 1;
            work->flags = flags & ~CGP_FLAG_ERROR_PLUS;
            break;
        }
        break;
    case CARD_COMMAND_U_TIC_SOLDIER_TRIGGER:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = CARD_COMMAND_U_TIC_SOLDIER;
            queued[CARD_QUEUE_PHASE] = CardCommandPhase(work);
            queued[CARD_QUEUE_OPERAND] = operand;
            queued[CARD_QUEUE_TARGET] = -1;
        }
        complete = 1;
        break;
    case CARD_COMMAND_U_TIC_SOLDIER:
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetErrorMessPlus(operand, work, CGP_ERR_USE_CARD_EFFECT, CARD_ID_U_TIC_SOLDIER);
            CardCommandAdvance(work);
            break;
        case 1:
            complete = 1;
            CardPlayCostCard(otherSide);
            break;
        }
        break;
    case CARD_COMMAND_WILHELM_TRIGGER:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = CARD_COMMAND_WILHELM;
            queued[CARD_QUEUE_PHASE] = CardCommandPhase(work);
            queued[CARD_QUEUE_OPERAND] = operand;
            queued[CARD_QUEUE_TARGET] = -1;
        }
        complete = 1;
        break;
    case CARD_COMMAND_WILHELM:
        switch (CardCommandStep(work)) {
        case 0: {
            u16 nextStep;
            s8 *timer;
            CGPSetErrorMessPlus(opponentSide, work, CGP_ERR_USE_CARD_EFFECT, CARD_ID_WILHELM);
            timer = &timer_33;
            nextStep = work->command.step;
            nextStep++;
            *timer = CARD_WILHELM_DECK_DAMAGE_DELAY;
            CardCommandSetStep(work, nextStep);
            break;
        }
        case 1:
            CardCommandAdvance(work);
            break;
        case 2:
            ownSide->battle[operand].battle.layers[0].life = 0;
            xglSoundEffectNormalID(CARD_SE_HIT, 0);
            CardCommandAdvance(work);
            break;
        case 3:
            if (timer_33 != 0) {
                timer_33--;
            } else {
                u16 nextStep;
                CardPlayCostCard(ownSide);
                CardPlayCostCard(ownSide);
                CardPlayCostCard(ownSide);
                CardPlayCostCard(ownSide);
                CardSetEffect(opponentSide, work, CARD_EFFECT_DECK_BURST, 0, CARD_SE_HIT, 0, 0);
                CardSetEffect(opponentSide, work, CARD_EFFECT_DAMAGE_NUMBER, 4, 0, 0, 0);
                nextStep = work->command.step;
                nextStep++;
                timer_33 = CARD_WILHELM_FINISH_DELAY;
                CardCommandSetStep(work, nextStep);
            }
            break;
        case 4:
            if (timer_33 != 0)
                timer_33--;
            else
                complete = 1;
            break;
        }
        break;
    case CARD_COMMAND_JOACHIM_MIZRAHI_TRIGGER:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = CARD_COMMAND_JOACHIM_MIZRAHI;
            queued[CARD_QUEUE_PHASE] = CardCommandPhase(work);
            queued[CARD_QUEUE_OPERAND] = operand;
            queued[CARD_QUEUE_TARGET] = -1;
        }
        complete = 1;
        break;
    case CARD_COMMAND_JOACHIM_MIZRAHI:
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetErrorMessPlus(opponentSide, work, CGP_ERR_USE_CARD_EFFECT, CARD_ID_JOACHIM_MIZRAHI);
            CardCommandAdvance(work);
            break;
        case 1:
            complete = 1;
            break;
        }
        break;
    case CARD_COMMAND_GARGOYLE_TRIGGER:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = CARD_COMMAND_GARGOYLE;
            queued[CARD_QUEUE_PHASE] = CardCommandPhase(work);
            queued[CARD_QUEUE_OPERAND] = operand;
            queued[CARD_QUEUE_TARGET] = -1;
        }
        complete = 1;
        break;
    case CARD_COMMAND_GARGOYLE: {
        int victimIndex;
        rotated = (side == 0 && work->rotStageId == 0) ||
                  (side != 0 && work->rotStageId != 0);
        for (victimIndex = 0; victimIndex < CARD_BOARD_SLOTS; victimIndex++) {
            if (otherSide->disposal[victimIndex].battle.layers[0].cardId >= 0 &&
                otherSide->disposal[victimIndex].battle.layers[1].cardId != CARD_ID_SHIELD &&
                otherSide->disposal[victimIndex].battle.layers[0].cardId != CARD_ID_BATTLE_SUIT) {
                otherSide->disposal[victimIndex].battle.layers[0].damageTimer = CARD_DAMAGE_TIMER_FRAMES;
                otherSide->disposal[victimIndex].battle.layers[0].life--;
                attackerRecord = (void *) &((CardBoardSlot *)(void *) ownSide)[operand];
                CardSetEffect(side, work, CARD_EFFECT_SHOT, 0, 0, attackerRecord->battle[0].battle.position,
                              &otherSide->disposal[victimIndex].battle);
                if (rotated) {
                    CardSetEffect(side, work, CARD_EFFECT_HIT_NUMBER_ROTATED, 1, 0, otherSide->disposal[victimIndex].battle.position,
                                  &otherSide->disposal[victimIndex].battle);
                } else {
                    CardSetEffect(side, work, CARD_EFFECT_HIT_NUMBER, 1, 0, otherSide->disposal[victimIndex].battle.position,
                                  &otherSide->disposal[victimIndex].battle);
                }
            }
        }
        for (victimIndex = 0; victimIndex < CARD_BOARD_SLOTS; victimIndex++) {
            if (otherSide->battle[victimIndex].battle.layers[0].cardId >= 0 &&
                otherSide->disposal[victimIndex].battle.layers[1].cardId != CARD_ID_SHIELD &&
                otherSide->disposal[victimIndex].battle.layers[0].cardId != CARD_ID_BATTLE_SUIT) {
                CardLayerStack *card = &otherSide->battle[victimIndex].battle;
                otherSide->battle[victimIndex].battle.layers[0].damageTimer = CARD_DAMAGE_TIMER_FRAMES;
                otherSide->battle[victimIndex].battle.layers[0].life--;
                CardSetEffect(side, work, CARD_EFFECT_SHOT, 0, 0, ownSide->battle[operand].battle.position, card);
                CardSetEffect(side, work, CARD_EFFECT_HIT_NUMBER, 1, 0, otherSide->battle[victimIndex].battle.position, card);
            }
        }
        complete = 1;
        xglSoundEffectNormalID(CARD_SE_HIT, 0);
        break;
    }
    case CARD_COMMAND_SUPPLY_ORDER_TRIGGER:
        queued = CPCSearchCommWk(side, work);
        if (queued != 0) {
            queued[CARD_QUEUE_COMMAND] = CARD_COMMAND_SUPPLY_ORDER;
            queued[CARD_QUEUE_PHASE] = CardCommandPhase(work);
            queued[CARD_QUEUE_OPERAND] = operand;
            queued[CARD_QUEUE_TARGET] = -1;
        }
        complete = 1;
        break;
    case CARD_COMMAND_SUPPLY_ORDER: {
        CardPlaySide *drawSide = playerSide;
        if (operand != 0)
            drawSide = enemySide;
        switch (CardCommandStep(work)) {
        case 0:
            CGPSetErrorMessPlus(operand, work, CGP_ERR_USE_CARD_EFFECT, CARD_ID_SUPPLY_ORDER);
            CardCommandAdvance(work);
            break;
        case 1:
            CardPlayDrawCard(drawSide);
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
    case CARD_COMMAND_MIYUKIS_EMAIL:
    case CARD_COMMAND_SONG:
    default:
        complete = 1;
        break;
    }
    return complete;
}

void CardPlayCommandBattleExecute(s16 side, s16 phase, CardGameWork *work) {
    s16 *entry;
    int checked;

    entry = work->commandQueue[1][0];
    if (side == 0)
        entry = work->commandQueue[0][0];
    checked = 0;
    do {
        if (entry[CARD_QUEUE_PHASE] == phase) {
            work->command.commandIndex = 1;
            if (CardPlayCommandPlay(side, entry[CARD_QUEUE_COMMAND], work, entry[CARD_QUEUE_OPERAND], entry[CARD_QUEUE_TARGET]) != 0) {
                CGPSetPermanentMess(work, 0);
                work->command.commandIndex = 0;
                work->command.step = 0;
                entry[CARD_QUEUE_PHASE] = -1;
                entry[CARD_QUEUE_OPERAND] = -1;
                entry[CARD_QUEUE_TARGET] = -1;
            }
            return;
        }
        entry += CARD_QUEUE_WORDS;
        checked++;
    } while (checked < CARD_QUEUE_SLOTS);
}

int CardPlayCommandExecute(s16 phase, CardGameWork *work) {
    int i;
    s16 *own;
    s16 *foe;
    int ownOffset;
    int foeOffset;

    for (i = 0; i < CARD_QUEUE_SLOTS; i++) {
        /* The phase words are addressed as bytes from work + 12: the player's queue
         * entry i (commandQueue[0][i]) lies at base + 464 + 8 * i and the enemy's
         * (commandQueue[1][i]) at base + 784 + 8 * i, both from one scaled index.
         * The enemy address is formed after the first test. */
        u8 *base = (u8 *)work + CARD_QUEUE_BASE_OFFSET;
        ownOffset = i;
        ownOffset <<= 3;
        foeOffset = ownOffset + CARD_QUEUE_ENEMY_OFFSET;
        ownOffset += CARD_QUEUE_PLAYER_OFFSET;
        own = (s16 *)(base + ownOffset);
        if (own[CARD_QUEUE_PHASE] == phase) {
            work->command.commandIndex = 1;
            if (CardPlayCommandPlay(0, work->commandQueue[0][i][CARD_QUEUE_COMMAND], work,
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
        if (foe[CARD_QUEUE_PHASE] == phase) {
            work->command.commandIndex = 1;
            if (CardPlayCommandPlay(1, work->commandQueue[1][i][CARD_QUEUE_COMMAND], work,
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

int CardPlayCommandCheck(s16 phase, s16 *queue) {
    int i;
    int count = 0;

    for (i = 0; i < CARD_QUEUE_SLOTS; i++) {
        if (queue[i * CARD_QUEUE_WORDS + CARD_QUEUE_PHASE] == phase)
            count++;
    }
    return count;
}
