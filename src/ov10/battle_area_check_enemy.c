/*
 * OV10 original TU 10: 0x00a34190..0x00a42034 (89 functions): the card game's
 * computer player. Board and hand checks, the set-, command- and move-phase
 * choices, and TodomeHyouka, the score every candidate move is compared by.
 */
#include "common.h"
#include "ov10/cgp.h"
#include "ov10/card_id.h"
#include "ov10/battle_area_check_enemy.h"
#include "ov10/card_enemy.h"
#include "ov10/card_fread.h"
#include "ov10/ccu_038_exec_sub.h"

/*
 * Slot codes. A slot index sits in the low halfword; CARD_SLOT_BATTLE marks a
 * battle slot (a disposal slot otherwise). The target CC_Kara_CommRequest
 * stores in commplay also uses CARD_SLOT_OTHER_SIDE for a slot of the other
 * side's board.
 */
#define CARD_SLOT_INDEX_MASK 0x0000FFFF
#define CARD_SLOT_BATTLE 0x000F0000
#define CARD_SLOT_OTHER_SIDE 0x00F00000

/*
 * A battle stack and the 12 bytes after it, the next slot's shunt position:
 * the span (0x1BC bytes, a CardBoardSlot's size) the AI copies when it
 * simulates a swap of battle slots, as CGPMoveDSBATTLEwork exchanges them.
 */
typedef struct CardStackSpan {
    CardLayerStack stack;
    float nextShunt[3];
} CardStackSpan;

/* Halfwords from one board slot's field to the same field of the next slot. */
#define CARD_SLOT_HALFWORDS (sizeof(CardBoardSlot) / sizeof(s16))

/*
 * GetFriendAttack's result for each own battle slot, as TodomeHyouka and the
 * set-phase choices read it: the attack power in the low halfword plus flags.
 */
#define ATTACK_POWER_MASK 0x0000FFFF
#define ATTACK_DOWNS_TARGET 0x00010000 /* the attack downs the card it hits (Lv 1 and Lv 10 Ziggy, AG-02-M, AG-04, Goblin); worth half the target's reward on top of the damage share */
#define ATTACK_HAND 0x00020000         /* CARD_ATTACK_HAND */
#define ATTACK_SPREAD 0x00040000       /* CARD_ATTACK_SPREAD: hits every target */
#define ATTACK_HOMING 0x00080000       /* CARD_ATTACK_HOMING: counts against the other side's deck */
#define ATTACK_SPLASH 0x00100000       /* one more point of damage to every target */

/*
 * TodomeHyouka scores. It adds TODOME_DECK_OUT_BONUS when the other side's draw pile would be down to
 * fewer than two cards, so any score from TODOME_DECK_OUT_SCORE up (and none below it)
 * includes the bonus; the checks read it as a winning move. The margins are how far a candidate has to
 * beat the current score before the AI plays it.
 */
#define TODOME_DECK_OUT_BONUS 1000
#define TODOME_DECK_OUT_SCORE 1001
#define TODOME_MARGIN 30
#define TODOME_MARGIN_SMALL 15 /* the margin of CheckCurryCommand, CheckTentCommand and CheckSeimitsuCommand */
#define TODOME_DISPOSE_MARGIN 120

/*
 * The event cards CC_Kara_CommRequest asks an evaluator about are named after
 * the disc's card names (ov10/card_id.h). The evaluators keep the AI's own
 * Japanese names: CheckExtCommand (Cure-All, Start Up), CheckCurryCommand,
 * CheckTentCommand (Bio Sphere), CheckTaikyakuCommand (Retreat, Woglinde),
 * CheckDamageCommand (Surprise Attack, Federation Cruiser), CheckNazoCommand
 * (Invitation), CheckGunoYokanCommand (Premonition), CheckDownGunoCommand
 * (Dragon Skull), CheckChudanCommand (Expulsion Order), CheckTeppekiCommand
 * (Impregnable Guard), Check413Command (Article 4-13), CheckSouteniHouCommand
 * (Phase Cannon), CheckTuukokuCommand (Notice), CheckKetugiCommand
 * (Resolution), CheckBujinCommand (Stim DX), CheckSensiCommand (War
 * Casualty), CheckXDamageCommand (X-Buster), CheckTotugekiCommand (Charge),
 * CheckSeimitsuCommand (Precise Shooting), CheckHatudouCommand (Invoke),
 * CheckKonranCommand (Confusion), CheckMilCommand (Miltian Conflict),
 * CheckShukumeiCommand (Destiny), CheckKyoukouCommand (Break Through).
 */

/* A slot code as CC_Kara_CommRequest stores it: the slot index is the low halfword. */
typedef union CardTargetCode {
    u32 code;
    u16 index;
} CardTargetCode;

/*
 * Computer-player state. commplay holds the target CC_Kara_CommRequest chose
 * (slot codes above); play413 and playteppeki are set when CC_Kara_CommPlay
 * plays the 413 and Teppeki command cards.
 */
static s32 commplay = 0;
static s32 play413 = 0;
static s32 playteppeki = 0;

/* Functions of this TU called before their definition. */
s32 CheckNowPower(s32 side, CardGameWork *work, s32 slot, s32 cardId);
s32 CheckEnemy12Attack(s32 side, CardGameWork *work);
s32 CheckCommandPlay(s32 side, CardGameWork *work, s32 cardId);
s32 GetShoukin(CardLayerStack *stack);
s32 TodomeHyouka(s32 side, CardGameWork *work, u32 slotA, u32 slotB, s32 *kinds, s32 *results);
s32 GetFriendAttack(s32 side, CardGameWork *work, s32 *kinds, s32 *results);
s32 CC_Kara_TodomeMove(s32 side, CardGameWork *work, s32 slot);
s32 VX10000Check(s32 side, CardGameWork *work);
s32 YoakimuMoMoCheck(s32 side, CardGameWork *work);
s32 MoMoCheck(s32 side, CardGameWork *work);
s32 CC_Kara_HaibiKabeMove(s32 side, CardGameWork *work, s32 slot);
s32 CC_Kara_KabeMove(s32 side, CardGameWork *work, s32 slot);
s32 LvUpCheck(s32 side, CardGameWork *work, s32 slot);
s32 CC_Kara_MoveFase_Type2(s32 side, CardGameWork *work, s32 slot);
s32 CC_Kara_MoveFase_Type3(s32 side, CardGameWork *work, s32 slot);
s32 CC_Kara_MoveFase_Type4(s32 side, CardGameWork *work, s32 slot);

/* Occupied battle slots on the side opposite `side`. */
s32 BattleAreaCheckEnemy(s32 side, CardGameWork *work)
{
    CardPlaySide *otherSide = &work->enemy;
    s16 *cardId;
    s32 count = 0;
    s32 remaining = 3;

    if (side) {
        otherSide = &work->player;
    }
    cardId = &otherSide->battle[0].battle.layers[0].cardId;
    do {
        s16 id = *cardId;
        cardId += CARD_SLOT_HALFWORDS;
        remaining -= 1;
        if (id >= 0) {
            count = count + 1;
        }
    } while (remaining >= 0);
    return count;
}

/* Occupied disposal slots on the side opposite `side`. */
s32 DisposeAreaCheckEnemy(s32 side, CardGameWork *work)
{
    CardPlaySide *otherSide = &work->enemy;
    s16 *cardId;
    s32 count = 0;
    s32 remaining = 3;

    if (side) {
        otherSide = &work->player;
    }
    cardId = &otherSide->disposal[0].battle.layers[0].cardId;
    do {
        s16 id = *cardId;
        cardId += CARD_SLOT_HALFWORDS;
        remaining -= 1;
        if (id >= 0) {
            count = count + 1;
        }
    } while (remaining >= 0);
    return count;
}

/* Occupied battle slots on the `side` side itself. */
s32 BattleAreaCheckFriend(s32 side, CardGameWork *work)
{
    CardPlaySide *ownSide = &work->player;
    s16 *cardId;
    s32 count = 0;
    s32 remaining = 3;

    if (side) {
        ownSide = &work->enemy;
    }
    cardId = &ownSide->battle[0].battle.layers[0].cardId;
    do {
        s16 id = *cardId;
        cardId += CARD_SLOT_HALFWORDS;
        remaining -= 1;
        if (id >= 0) {
            count = count + 1;
        }
    } while (remaining >= 0);
    return count;
}

/* Occupied disposal slots on the `side` side itself. */
s32 DisposeAreaCheckFriend(s32 side, CardGameWork *work)
{
    CardPlaySide *ownSide = &work->player;
    s16 *cardId;
    s32 count = 0;
    s32 remaining = 3;

    if (side) {
        ownSide = &work->enemy;
    }
    cardId = &ownSide->disposal[0].battle.layers[0].cardId;
    do {
        s16 id = *cardId;
        cardId += CARD_SLOT_HALFWORDS;
        remaining -= 1;
        if (id >= 0) {
            count = count + 1;
        }
    } while (remaining >= 0);
    return count;
}

/* Counts the cards on `side`'s disposal and battle slots whose deck metadata
 * classification (GetDeckData) is four. */
s32 CheckFriendType4(s32 side, CardGameWork *work)
{
    CardPlaySide *sideData = &work->player;
    s16 *cardId;
    s32 count = 0;
    s32 remaining = 3;

    if (side != 0) {
        sideData = &work->enemy;
    }
    cardId = &sideData->disposal[0].battle.layers[0].cardId;
    do {
        s16 id = *cardId;
        cardId += CARD_SLOT_HALFWORDS;
        if (id >= 0) {
            if (GetDeckData(side, work, id) == 4) {
                count = count + 1;
            }
        }
        remaining -= 1;
    } while (remaining >= 0);
    remaining = 3;
    cardId = &sideData->battle[0].battle.layers[0].cardId;
    do {
        s16 id = *cardId;
        cardId += CARD_SLOT_HALFWORDS;
        if (id >= 0) {
            if (GetDeckData(side, work, id) == 4) {
                count = count + 1;
            }
        }
        remaining -= 1;
    } while (remaining >= 0);
    return count;
}

/* Counts the battle-slot cards of `side` whose attack type (CardChkAttackType)
 * is one. */
s32 CheckFriendKakutouCnt(s32 side, CardGameWork *work)
{
    CardPlaySide *sideData = &work->player;
    s32 slotIndex = 0;
    s32 count = 0;
    s32 remaining = 3;

    if (side != 0) {
        sideData = &work->enemy;
    }
    {
        CardLayerStack *card = &sideData->battle[0].battle;
        s16 *cardId = &sideData->battle[0].battle.layers[0].cardId;

        do {
            s16 id = *cardId;

            cardId += CARD_SLOT_HALFWORDS;
            if (id >= 0) {
                if (CardChkAttackType(work->definitions, card) == CARD_ATTACK_HAND) {
                    count = count + 1;
                }
            }
            card = (CardLayerStack *)((unsigned char *)card + sizeof(CardBoardSlot));
            slotIndex += 1;
            remaining -= 1;
        } while (remaining >= 0);
    }
    return count;
}

/* -1 unless `side` has exactly one occupied battle slot. Otherwise the first
 * battle slot index (tagged 0xF0000 as in CheckShukumeiCommand) holding Albedo
 * whose CheckNowPower value differs from that card's attackPower. */
s32 AlbedoCheck(s32 side, CardGameWork *work)
{
    CardPlaySide *sideData = &work->player;
    s32 slotIndex;

    if (side != 0) {
        sideData = &work->enemy;
    }
    if (BattleAreaCheckFriend(side, work) != 1) {
        return -1;
    }
    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        if (sideData->battle[slotIndex].battle.layers[0].cardId >= 0) {
            if (sideData->battle[slotIndex].battle.layers[0].cardId == CARD_ID_ALBEDO) {
                if (CheckNowPower(side, work, slotIndex | CARD_SLOT_BATTLE, CARD_ID_ALBEDO) !=
                    work->definitions[sideData->battle[slotIndex].battle.layers[0].cardId].attackPower) {
                    return slotIndex;
                }
            }
        }
    }
    return -1;
}

/*
 * Special-card AI move policy: picks the caller's disposal or battle slot
 * `slot` (0xF0000 tags a battle slot), shuffles battle cards, then scores the
 * board with GetFriendAttack/TodomeHyouka.
 */
s32 GainanCheck(s32 side, CardGameWork *work)
{
    CardPlaySide *ownSide;
    CardPlaySide *otherSide;
    s32 index;
    s32 occupied = 0;
    s32 friendA[CARD_BOARD_SLOTS];
    s32 friendB[CARD_BOARD_SLOTS];
    s32 junkCount;
    s32 slot = -1;
    s32 score;

    if (side == 0) {
        ownSide = &work->player;
        otherSide = &work->enemy;
    } else {
        ownSide = &work->enemy;
        otherSide = &work->player;
    }

    for (index = 0; index < CARD_BOARD_SLOTS; index++) {
        if (ownSide->disposal[index].battle.layers[0].cardId >= 0) {
            occupied++;
            if (ownSide->disposal[index].battle.layers[0].cardId == CARD_ID_ALBEDO &&
                !(ownSide->disposal[index].battle.flags & CARD_STACK_FLAG_FROZEN)) {
                slot = index;
                break;
            }
        }
    }
    for (index = 0; index < CARD_BOARD_SLOTS; index++) {
        if (ownSide->battle[index].battle.layers[0].cardId >= 0) {
            occupied++;
            if (ownSide->battle[index].battle.layers[0].cardId == CARD_ID_ALBEDO &&
                !(ownSide->battle[index].battle.flags & CARD_STACK_FLAG_FROZEN)) {
                slot = index | CARD_SLOT_BATTLE;
                break;
            }
        }
    }
    junkCount = CardPlayJunkCnt(ownSide);
    if (slot == -1) {
        return 0;
    }
    if (junkCount < 4 || occupied >= 6) {
        return 0;
    }
    if (!(slot & CARD_SLOT_BATTLE)) {
        index = 0;
        do {
            if (ownSide->battle[index].battle.layers[0].cardId < 0) {
                CGPMoveDSBATTLEwork(&ownSide->battle[index].battle, &ownSide->disposal[slot].battle);
                break;
            }
        } while (++index < 4);
    } else {
        index = 0;
    }
    for (index = 0; index < CARD_BOARD_SLOTS; index++) {
        if (ownSide->battle[index].battle.layers[0].cardId >= 0 &&
            ownSide->battle[index].battle.layers[0].cardId != CARD_ID_ALBEDO) {
            s32 other;

            for (other = 0; other < CARD_BOARD_SLOTS; other++) {
                if (ownSide->disposal[other].battle.layers[0].cardId < 0 ||
                    ownSide->disposal[other].battle.layers[0].cardId == CARD_ID_ALBEDO) {
                    CGPMoveDSBATTLEwork(&ownSide->disposal[other].battle, &ownSide->battle[index].battle);
                }
            }
        }
    }
    for (index = 0; index < 3; index++) {
        s32 search;

        for (search = 0; search < CARD_BOARD_SLOTS; search++) {
            if (ownSide->battle[search].battle.layers[0].cardId >= 0) {
                if (ownSide->battle[search].battle.layers[0].cardId == CARD_ID_ALBEDO) {
                    slot = search;
                    break;
                }
            }
        }
        CC_Kara_TodomeMove(side, work, slot);
    }
    for (index = 0; index < CARD_BOARD_SLOTS; index++) {
        if (ownSide->battle[index].battle.layers[0].cardId >= 0) {
            if (ownSide->battle[index].battle.layers[0].cardId == CARD_ID_ALBEDO) {
                slot = index;
                break;
            }
        }
    }
    if (junkCount >= 7) {
        s32 handCount = CardPlayHandCnt(&ownSide->hand);
        s32 playable = 0;
        s32 emptyDisposal = 0;

        for (index = 0; index < handCount; index++) {
            s32 cardId = (u16) ownSide->hand.cards[index];

            if (cardId == CARD_ID_RETREAT || cardId == CARD_ID_WOGLINDE) {
                if (CheckCommandPlay(side, work, (s16) cardId) == 1) {
                    playable++;
                }
            }
        }
        for (index = 0; index < CARD_BOARD_SLOTS; index++) {
            if (otherSide->disposal[index].battle.layers[0].cardId < 0) {
                emptyDisposal++;
            }
        }
        switch (playable) {
        case 0:
            if (otherSide->battle[0].battle.layers[0].cardId < 0 &&
                otherSide->battle[2].battle.layers[0].cardId < 0) {
                CGPMoveDSBATTLEwork(&ownSide->battle[3].battle, &ownSide->battle[slot].battle);
                return 1;
            } else if (otherSide->battle[1].battle.layers[0].cardId < 0 &&
                       otherSide->battle[3].battle.layers[0].cardId < 0) {
                CGPMoveDSBATTLEwork(&ownSide->battle[2].battle, &ownSide->battle[slot].battle);
                return 1;
            }
            break;
        case 1:
            if (otherSide->battle[0].battle.layers[0].cardId < 0 &&
                otherSide->battle[2].battle.layers[0].cardId < 0) {
                CGPMoveDSBATTLEwork(&ownSide->battle[3].battle, &ownSide->battle[slot].battle);
                return 1;
            } else if (otherSide->battle[1].battle.layers[0].cardId < 0 &&
                       otherSide->battle[3].battle.layers[0].cardId < 0) {
                CGPMoveDSBATTLEwork(&ownSide->battle[2].battle, &ownSide->battle[slot].battle);
                return 1;
            } else if (emptyDisposal > 0) {
                if (otherSide->battle[0].battle.layers[0].cardId < 0 ||
                    otherSide->battle[2].battle.layers[0].cardId < 0) {
                    CGPMoveDSBATTLEwork(&ownSide->battle[3].battle, &ownSide->battle[slot].battle);
                    return 1;
                } else if (otherSide->battle[1].battle.layers[0].cardId < 0 ||
                           otherSide->battle[3].battle.layers[0].cardId < 0) {
                    CGPMoveDSBATTLEwork(&ownSide->battle[2].battle, &ownSide->battle[slot].battle);
                    return 1;
                }
            }
            break;
        default:
            if (otherSide->battle[0].battle.layers[0].cardId < 0 &&
                otherSide->battle[2].battle.layers[0].cardId < 0) {
                CGPMoveDSBATTLEwork(&ownSide->battle[3].battle, &ownSide->battle[slot].battle);
                return 1;
            } else if (otherSide->battle[1].battle.layers[0].cardId < 0 &&
                       otherSide->battle[3].battle.layers[0].cardId < 0) {
                CGPMoveDSBATTLEwork(&ownSide->battle[2].battle, &ownSide->battle[slot].battle);
                return 1;
            } else if (emptyDisposal > 0 &&
                       (otherSide->battle[0].battle.layers[0].cardId < 0 ||
                        otherSide->battle[2].battle.layers[0].cardId < 0)) {
                CGPMoveDSBATTLEwork(&ownSide->battle[3].battle, &ownSide->battle[slot].battle);
                return 1;
            } else if (emptyDisposal > 0 &&
                       (otherSide->battle[1].battle.layers[0].cardId < 0 ||
                        otherSide->battle[3].battle.layers[0].cardId < 0)) {
                CGPMoveDSBATTLEwork(&ownSide->battle[2].battle, &ownSide->battle[slot].battle);
                return 1;
            } else if (emptyDisposal >= 2 && CardPlayYamaCnt(ownSide) >= 3) {
                if (otherSide->battle[0].battle.layers[0].cardId < 0 ||
                    otherSide->battle[2].battle.layers[0].cardId < 0) {
                    CGPMoveDSBATTLEwork(&ownSide->battle[3].battle, &ownSide->battle[slot].battle);
                    return 1;
                } else if (otherSide->battle[1].battle.layers[0].cardId < 0 ||
                           otherSide->battle[3].battle.layers[0].cardId < 0) {
                    CGPMoveDSBATTLEwork(&ownSide->battle[2].battle, &ownSide->battle[slot].battle);
                    return 1;
                }
                return 1;
            }
        }
    }
    for (index = 0; index < 3; index++) {
        s32 search;

        for (search = 0; search < CARD_BOARD_SLOTS; search++) {
            if (ownSide->battle[search].battle.layers[0].cardId >= 0) {
                if (ownSide->battle[search].battle.layers[0].cardId == CARD_ID_ALBEDO) {
                    slot = search;
                    break;
                }
            }
        }
        CC_Kara_TodomeMove(side, work, slot);
    }
    GetFriendAttack(side, work, friendA, friendB);
    if (CheckEnemy12Attack(side == 0, work) >= junkCount + 2) {
        return 0;
    }
    score = TodomeHyouka(side, work, 0, 0, friendA, friendB);
    if (junkCount >= 7) {
        if (score <= junkCount * 10 + 20) {
            return 0;
        }
    } else if (score < junkCount * 10 + 40) {
        return 0;
    }
    return 1;
}

/* 1 when a disposal or battle slot of `side` holds a card whose definition
 * has flag 4 set (CardDefinition.flags), else 0. */
s32 CheckGuno(s32 side, CardGameWork *work)
{
    s32 slotIndex;
    CardPlaySide *sideData;
    s16 *cardId;
    s16 id;

    if (side == 0) {
        sideData = &work->player;
        slotIndex = 0;
    } else {
        sideData = &work->enemy;
        slotIndex = 0;
    }

    slotIndex = 0;
    cardId = &sideData->disposal[0].battle.layers[0].cardId;
    for (; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        id = *cardId;
        cardId += CARD_SLOT_HALFWORDS;
        if (id >= 0) {
            if (work->definitions[id].flags & CARD_ATTR_GNOSIS) {
                return 1;
            }
        }
    }
    cardId = &sideData->battle[0].battle.layers[0].cardId;
    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        id = *cardId;
        cardId += CARD_SLOT_HALFWORDS;
        if (id >= 0 && (work->definitions[id].flags & CARD_ATTR_GNOSIS)) {
            return 1;
        }
    }
    return 0;
}

s32 CheckREAL(s32 side, CardGameWork *work)
{
    s32 slotIndex;
    CardPlaySide *ownSide;
    s16 *diskCardId;
    s16 *battleCardId;
    s16 diskId;
    s16 battleId;

    if (side == 0) {
        ownSide = &work->player;
        slotIndex = 0;
    } else {
        ownSide = &work->enemy;
        slotIndex = 0;
    }

    slotIndex = 0;
    diskCardId = &ownSide->disposal[0].battle.layers[0].cardId;
    for (; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        diskId = *diskCardId;
        diskCardId += CARD_SLOT_HALFWORDS;
        if (diskId >= 0) {
            if (work->definitions[diskId].flags & CARD_ATTR_REALIAN) {
                return 1;
            }
        }
    }
    battleCardId = &ownSide->battle[0].battle.layers[0].cardId;
    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        battleId = *battleCardId;
        battleCardId += CARD_SLOT_HALFWORDS;
        if (battleId >= 0 && (work->definitions[battleId].flags & CARD_ATTR_REALIAN)) {
            return 1;
        }
    }
    return 0;
}

/*
 * CGPCalcPowerEnv value of the caller's card in a slot: a battle slot when
 * `slot` has 0xF0000 set (slot index in the low halfword), otherwise a
 * disposal slot. Zero when cardId or slot is negative, the slot index is not
 * below four, the slot's base layer is empty, or VX10000Check finds the VX-10000
 * on a battle board while that card's attack type is 4.
 */
s32 CheckNowPower(s32 side, CardGameWork *work, s32 slot, s32 cardId)
{
    CardPlaySide *ownSide = (side == 0) ? &work->player : &work->enemy;
    s32 power;

    if (cardId < 0) {
        return 0;
    }
    if (slot < 0) {
        return 0;
    }
    if ((slot & CARD_SLOT_INDEX_MASK) >= 4) {
        return 0;
    }
    if ((slot & CARD_SLOT_BATTLE) == 0) {
        CardLayerStack *card = &ownSide->disposal[slot].battle;

        power = CardPlayCntPower(work, card);
        if (ownSide->disposal[slot].battle.layers[0].cardId < 0) {
            return 0;
        }
        if (VX10000Check(side, work) == 1 &&
            CardChkAttackType(work->definitions, card) == CARD_ATTACK_HOMING) {
            return 0;
        }
        power = CGPCalcPowerEnv(side, work, power, cardId);
    } else {
        CardLayerStack *card;

        slot &= CARD_SLOT_INDEX_MASK;
        card = &ownSide->battle[slot].battle;
        power = CardPlayCntPower(work, card);
        if (ownSide->battle[slot].battle.layers[0].cardId < 0) {
            return 0;
        }
        if (VX10000Check(side, work) == 1 &&
            CardChkAttackType(work->definitions, card) == CARD_ATTACK_HOMING) {
            return 0;
        }
        power = CGPCalcPowerEnv(side, work, power, cardId);
    }
    return power;
}

static inline s8 CardLayerPower(CardLayerStack *card) {
    return card->power;
}

/*
 * Sum of the four largest eligible powers among the other side's battle and
 * disposal slots. The first four values are the battle slots and the last
 * four the disposal slots; a slot is eligible when its base card is on the
 * board, its layer power is below two and its attack type is 2 or 4.
 */
s32 CheckEnemyAttack(s32 side, CardGameWork *work)
{
    CardPlaySide *otherSide = (side == 0) ? &work->enemy : &work->player;
    s32 slotPower[8];
    s32 topPower[4];
    s32 *value;
    s32 best;
    s32 total;
    s32 i;
    s32 j;

    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        s32 attackType;

        slotPower[i] = 0;
        if (otherSide->battle[i].battle.layers[0].cardId >= 0) {
            attackType = CardChkAttackType(work->definitions, &otherSide->battle[i].battle);
            if (CardLayerPower(&otherSide->battle[i].battle) < 2 && (attackType == CARD_ATTACK_BALLISTIC || attackType == CARD_ATTACK_HOMING)) {
                slotPower[i] = CGPCalcPowerEnv(side == 0, work,
                                               CardPlayCntPower(work, &otherSide->battle[i].battle),
                                               otherSide->battle[i].battle.layers[0].cardId);
            }
        }
    }
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        s32 attackType;

        slotPower[4 + i] = 0;
        if (otherSide->disposal[i].battle.layers[0].cardId >= 0) {
            attackType = CardChkAttackType(work->definitions, &otherSide->disposal[i].battle);
            if (CardLayerPower(&otherSide->disposal[i].battle) < 2 && (attackType == CARD_ATTACK_BALLISTIC || attackType == CARD_ATTACK_HOMING)) {
                slotPower[4 + i] = CGPCalcPowerEnv(side == 0, work,
                                                   CardPlayCntPower(work, &otherSide->disposal[i].battle),
                                                   otherSide->disposal[i].battle.layers[0].cardId);
            }
        }
    }

    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        best = 0;
        for (j = 0; j < 8; j++) {
            if (slotPower[best] < slotPower[j]) {
                best = j;
            }
        }
        topPower[i] = slotPower[best];
        slotPower[best] = 0;
    }

    total = 0;
    value = topPower;
    i = 3;
    do {
        total += *value;
        value++;
        i -= 1;
    } while (i >= 0);
    return total;
}

/*
 * Highest and second-highest CGPCalcPowerEnv value among the other side's
 * category-one/two attack cards (power 0 or 1 in the disposal pile with
 * attack type 2 or 3, power 0 or 1 in the battle pile with attack type 2),
 * summed.
 */
s32 CheckEnemy12Attack(s32 side, CardGameWork *work)
{
    struct {
        s32 highest;
        s32 second;
    } best;
    CardPlaySide *otherSide;
    s32 slotIndex;

    memset(&best, 0, sizeof(best));
    otherSide = (side == 0) ? &work->enemy : &work->player;
    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        CardLayerStack *card = &otherSide->disposal[slotIndex].battle;

        if (card->layers[0].cardId >= 0) {
            s32 attackType = CardChkAttackType(work->definitions, card);

            if (CardLayerPower(card) == 1 && (u32) (attackType - CARD_ATTACK_BALLISTIC) < 2) {
                s32 power = CGPCalcPowerEnv(side == 0, work,
                                             CardPlayCntPower(work, card),
                                             card->layers[0].cardId);
                if (best.highest < power) {
                    best.second = best.highest;
                    best.highest = power;
                } else if (best.second < power) {
                    best.second = power;
                }
            }
        }
    }
    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        CardLayerStack *card = &otherSide->battle[slotIndex].battle;

        if (card->layers[0].cardId >= 0) {
            s32 attackType = CardChkAttackType(work->definitions, card);

            if (CardLayerPower(card) < 2 && attackType == CARD_ATTACK_BALLISTIC) {
                s32 power = CGPCalcPowerEnv(side == 0, work,
                                             CardPlayCntPower(work, card),
                                             card->layers[0].cardId);
                if (best.highest < power) {
                    best.second = best.highest;
                    best.highest = power;
                } else if (best.second < power) {
                    best.second = power;
                }
            }
        }
    }
    return best.highest + best.second;
}

/*
 * Highest CGPCalcPowerEnv value among the other side's category-one attack
 * cards (power 0 or 1, attack type 1), checking the battle pile then the
 * disposal pile.
 */
s32 CheckEnemyFight(s32 side, CardGameWork *work)
{
    CardPlaySide *otherSide = (side == 0) ? &work->enemy : &work->player;
    s32 best = 0;
    s32 power;
    s32 slotIndex;

    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        CardLayerStack *card = &otherSide->battle[slotIndex].battle;

        if (card->layers[0].cardId >= 0) {
            s32 attackType = CardChkAttackType(work->definitions, card);

            if (CardLayerPower(card) < 2 && attackType == CARD_ATTACK_HAND) {
                power = CGPCalcPowerEnv(side == 0, work,
                                         CardPlayCntPower(work, card),
                                         card->layers[0].cardId);
                if (power > best) {
                    best = power;
                }
            }
        }
    }
    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        CardLayerStack *card = &otherSide->disposal[slotIndex].battle;

        if (card->layers[0].cardId >= 0) {
            s32 attackType = CardChkAttackType(work->definitions, card);

            if (CardLayerPower(card) < 2 && attackType == CARD_ATTACK_HAND) {
                power = CGPCalcPowerEnv(side == 0, work,
                                         CardPlayCntPower(work, card),
                                         card->layers[0].cardId);
                if (power > best) {
                    best = power;
                }
            }
        }
    }
    return best;
}

/*
 * Histograms the side's field cards by generation, then, when the total is
 * below the deck record's threshold at byte 5, requires every generation
 * count to reach the deck's per-generation minimum (bytes 0..4).
 */
s32 CheckGeneration(s32 side, CardGameWork *work)
{
    u8 counts[5];
    CardPlaySide *ownSide;
    u8 *gene;
    s32 deck;
    s32 total = 0;
    s32 i;

    if (side == 0) {
        ownSide = &work->player;
        deck = work->playerDeckId;
    } else {
        deck = work->enemyDeckId;
        ownSide = &work->enemy;
    }
    gene = DeckGeneLst[deck];
    CardCountGeneration(counts, work->definitions, ownSide);
    for (i = 0; i < 5; i++) {
        total += counts[i];
    }
    if (total >= gene[5]) {
        return 1;
    }
    for (i = 0; i < 5; i++) {
        if (gene[i] > counts[i]) {
            return 0;
        }
    }
    return 1;
}

/*
 * Looks cardId up in the side's ROM deck (EnemyDeckLst) and returns the
 * parallel DeckDataLst byte for it, or 0 when cardId is negative or not in
 * the 40-card deck.
 */
u32 GetDeckData(int side, CardGameWork *work, int cardId)
{
    int deck;
    u8 *data;
    u8 *cards;
    int i;

    if (cardId < 0) {
        return 0;
    }
    if (side == 0) {
        deck = work->playerDeckId;
    } else {
        deck = work->enemyDeckId;
    }
    cards = EnemyDeckLst[deck];
    data = DeckDataLst[deck];
    for (i = 0; i < CPU_DECK_CARDS; i++) {
        if (cards[i] == cardId) {
            return data[i];
        }
    }
    return 0;
}

/*
 * Metadata byte of the CPU deck entry for cardId, adjusted when the side has
 * battle or disposal cards and CheckGeneration passes: a zero byte reads as 2
 * and a one byte as 3. Returns 3 when the side has no cards in play.
 */
u32 GetDeckDataAdjust(s32 side, CardGameWork *work, s32 cardId)
{
    u8 *data;
    u8 *cards;
    s32 deck;
    s32 total;
    s32 i;

    if (cardId < 0) {
        return 0;
    }
    if (side == 0) {
        deck = work->playerDeckId;
    } else {
        deck = work->enemyDeckId;
    }
    cards = EnemyDeckLst[deck];
    data = DeckDataLst[deck];
    total = BattleAreaCheckEnemy(side, work);
    total += DisposeAreaCheckEnemy(side, work);
    if (total == 0) {
        return 3;
    }
    for (i = 0; i < CPU_DECK_CARDS; i++) {
        if (cards[i] == cardId) {
            if (CheckGeneration(side, work)) {
                if (data[i] == 0) {
                    return 2;
                }
                if (data[i] == 1) {
                    return 3;
                }
            }
            return data[i];
        }
    }
    return 0;
}

/*
 * Whether the given side may play command card `cardId` now: a card that
 * reserves cards from the draw pile has to leave at least three of them
 * there, and neither the card's own condition nor the command condition may
 * refuse it.
 */
s32 CheckCommandPlay(s32 side, CardGameWork *work, s32 cardId)
{
    CardPlaySide *ownSide = (side == 0) ? &work->player : &work->enemy;

    if (work->definitions[cardId].reserveCost != 0 &&
        CardPlayYamaCnt(ownSide) - work->definitions[cardId].reserveCost < 3) {
        return 0;
    }
    if (CardPlayChkCondition(work->definitions, ownSide, cardId, work) == 0 &&
        CardPlayCommandCondition(side, cardId, work) == 0) {
        return 1;
    }
    return 0;
}

/* Cure-All and Start Up: the two command cards that revive a downed card. */
s32 CheckDownKaifukuCommand(s32 side, CardGameWork *work)
{
    CardPlaySide *sideData = &work->player;
    s32 cardIndex = 0;
    s32 handCount;

    if (side != 0) {
        sideData = &work->enemy;
    }
    handCount = CardPlayHandCnt(&sideData->hand);
    if (handCount <= 0) {
        return 0;
    }
    do {
        s16 cardId = sideData->hand.cards[cardIndex];

        if (cardId == CARD_ID_CURE_ALL || cardId == CARD_ID_START_UP) {
            s32 result = CheckCommandPlay(side, work, cardId);
            if (result == 1) {
                return result;
            }
        }
        cardIndex++;
    } while (cardIndex < handCount);
    return 0;
}

s32 CheckEnemyGuno(s32 side, CardGameWork *work)
{
    s32 slotIndex;
    CardPlaySide *otherSide;
    s16 *diskCardId;
    s16 *battleCardId;
    s16 diskId;
    s16 battleId;

    if (side == 0) {
        otherSide = &work->enemy;
        slotIndex = 0;
    } else {
        otherSide = &work->player;
        slotIndex = 0;
    }

    slotIndex = 0;
    diskCardId = &otherSide->disposal[0].battle.layers[0].cardId;
    for (; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        diskId = *diskCardId;
        diskCardId += CARD_SLOT_HALFWORDS;
        if (diskId >= 0) {
            if (work->definitions[diskId].flags & CARD_ATTR_GNOSIS) {
                return 1;
            }
        }
    }
    battleCardId = &otherSide->battle[0].battle.layers[0].cardId;
    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        battleId = *battleCardId;
        battleCardId += CARD_SLOT_HALFWORDS;
        if (battleId >= 0 && (work->definitions[battleId].flags & CARD_ATTR_GNOSIS)) {
            return 1;
        }
    }
    return 0;
}

/*
 * Operation-card choice for one side: plays the first hand card the rules
 * allow (index into the hand is passed to CardPlayDisployment), otherwise
 * returns 0.
 */
s32 CC_Kara_SetOpe(s32 side, CardGameWork *work)
{
    CardPlaySide *ownSide;
    s32 index;
    s32 handCount;
    s32 cardId;
    s32 battleFlagged;

    ownSide = &work->player;
    if (side != 0) {
        ownSide = &work->enemy;
    }
    if (CardPlayOperationCnt(ownSide) >= 4) {
        return 0;
    }
    handCount = CardPlayHandCnt(&ownSide->hand);
    if (play413 == 1) {
        for (index = 0; index < handCount; index++) {
            cardId = ownSide->hand.cards[index];
            if (work->definitions[cardId].kind == CARD_KIND_SITUATION && cardId == CARD_ID_ARMISTICE &&
                CGPDispoSub0(side, work, index) == 0) {
                if (CardPlayYamaCnt(ownSide) - work->definitions[CARD_ID_ARMISTICE].reserveCost <= 0) {
                    return 0;
                }
                ownSide->hand.playPoints[1] -= work->definitions[ownSide->hand.cards[index]].playCost;
                CardPlayDisployment(side, work, ownSide, index);
                return 1;
            }
        }
    }
    battleFlagged = 0;
    for (index = 0; index < CARD_BOARD_SLOTS; index++) {
        if (ownSide->battle[index].battle.layers[0].cardId >= 0 &&
            (work->definitions[ownSide->battle[index].battle.layers[0].cardId].flags & CARD_ATTR_HUMAN)) {
            battleFlagged = 1;
            goto flag_done;
        }
    }
flag_done:
    if (battleFlagged == 1) {
        for (index = 0; index < handCount; index++) {
            cardId = ownSide->hand.cards[index];
            if (work->definitions[cardId].kind != CARD_KIND_SITUATION || cardId != CARD_ID_STIM) {
                continue;
            }
            if (CGPDispoSub0(side, work, index) == 0) {
                ownSide->hand.playPoints[1] -= work->definitions[ownSide->hand.cards[index]].playCost;
                CardPlayDisployment(side, work, ownSide, index);
                return 1;
            }
        }
    }
    for (index = 0; index < handCount; index++) {
        s32 dispoResult;

        cardId = ownSide->hand.cards[index];
        if (work->definitions[cardId].kind != CARD_KIND_SITUATION) {
            continue;
        }
        dispoResult = CGPDispoSub0(side, work, index);
        if ((cardId == CARD_ID_STIM || cardId == CARD_ID_GOOD_LUCK) && battleFlagged == 0) {
            continue;
        }
        if (cardId == CARD_ID_HILBERT_EFFECT || cardId == CARD_ID_DSSS) {
            if (CheckEnemyGuno(side, work) == 0) {
                continue;
            }
        }
        if (cardId == CARD_ID_ARMISTICE) {
            s32 attack[CARD_BOARD_SLOTS];
            s32 status[CARD_BOARD_SLOTS];
            s32 flagCount;
            s32 hasSpecial;
            s32 scanSlot;

            flagCount = 0;
            hasSpecial = 0;
            for (scanSlot = 0; scanSlot < CARD_BOARD_SLOTS; scanSlot++) {
                if (ownSide->battle[scanSlot].battle.layers[0].cardId >= 0) {
                    if (ownSide->battle[scanSlot].battle.layers[0].cardId == CARD_ID_VIRGIL) {
                        hasSpecial = 1;
                    }
                    if (work->definitions[ownSide->battle[scanSlot].battle.layers[0].cardId].flags & CARD_ATTR_REALIAN) {
                        flagCount++;
                    }
                }
            }
            if (dispoResult != 0) {
                continue;
            }
            if (play413 != 0 || (hasSpecial == 1 && flagCount > 0)) {
                if (CardPlayYamaCnt(ownSide) - work->definitions[cardId].reserveCost <= 0) {
                    return 0;
                }
                ownSide->hand.playPoints[1] -= work->definitions[ownSide->hand.cards[index]].playCost;
                CardPlayDisployment(side, work, ownSide, index);
                return 1;
            }
            {
                s32 score;
                s32 ownTotal;
                s32 otherTotal;
                s32 attackScore;
                s32 friendScore;

                GetFriendAttack(side, work, attack, status);
                score = TodomeHyouka(side, work, 0, 0, attack, status);
                ownTotal = BattleAreaCheckEnemy(side, work) + DisposeAreaCheckEnemy(side, work);
                otherTotal = BattleAreaCheckFriend(side, work) + DisposeAreaCheckFriend(side, work);
                if (score >= 50) {
                    continue;
                }
                attackScore = CardPlayYamaCnt(ownSide) - 1;
                if (CheckEnemyAttack(side, work) >= attackScore) {
                    if (CardPlayYamaCnt(ownSide) - work->definitions[cardId].reserveCost <= 0) {
                        return 0;
                    }
                    ownSide->hand.playPoints[1] -= work->definitions[ownSide->hand.cards[index]].playCost;
                    CardPlayDisployment(side, work, ownSide, index);
                    return 1;
                }
                if (ownTotal < otherTotal) {
                    continue;
                }
                GetFriendAttack((side == 0), work, attack, status);
                friendScore = TodomeHyouka((side == 0), work, 0, 0, attack, status);
                if (!(score < friendScore)) {
                    continue;
                }
            }
        }
        if (dispoResult != 0) {
            continue;
        }
        if (CardPlayYamaCnt(ownSide) - work->definitions[cardId].reserveCost <= 0) {
            return 0;
        }
        ownSide->hand.playPoints[1] -= work->definitions[ownSide->hand.cards[index]].playCost;
        CardPlayDisployment(side, work, ownSide, index);
        return 1;
    }
    return 0;
}

/*
 * Whether the side's hand holds Stim (a kind-4
 * card) that CardPlayChkCondition accepts (returns 0); only the phase-1 and
 * phase-8 turns are checked.
 */
s32 CheckTamasii(s32 side, CardGameWork *work)
{
    CardPlaySide *sideData;
    CardHand *hand;
    s16 phase;
    s32 handCount;
    s32 cardIndex;

    if (side == 0) {
        sideData = &work->player;
        hand = &sideData->hand;
    } else {
        sideData = &work->enemy;
        hand = &sideData->hand;
    }
    phase = work->command.phase;
    if (phase == CARD_PHASE_MOVE || phase == CARD_PHASE_MOVE + CARD_PHASES_PER_SIDE) {
        handCount = CardPlayHandCnt(hand);
        for (cardIndex = 0; cardIndex < handCount; cardIndex++) {
            s16 cardId = hand->cards[cardIndex];
            if (work->definitions[cardId].kind == CARD_KIND_SITUATION && cardId == CARD_ID_STIM) {
                if (CardPlayChkCondition(work->definitions, sideData, CARD_ID_STIM, work) == 0) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

/* Weapon-card placement for the CPU hand. */
s32 CC_Kara_SetWeapon(s32 side, CardGameWork *work, s32 index)
{
    CardPlaySide *otherSide;
    CardDefinition *definitions = work->definitions;
    CardPlaySide *ownSide;
    CardCursorPosition held;
    CardCursorPosition disposalTarget;
    CardCursorPosition battleTarget;
    s32 slotCard[CARD_BOARD_SLOTS];
    s32 bestSlot;
    s32 i;

    if (side == 0) {
        ownSide = &work->player;
        otherSide = &work->enemy;
        held.zone = 9;
        disposalTarget.zone = CARD_ZONE_SIDE0_DISPOSAL;
        battleTarget.zone = CARD_ZONE_SIDE0_BATTLE;
    } else {
        ownSide = &work->enemy;
        otherSide = &work->player;
        held.zone = 0;
        disposalTarget.zone = CARD_ZONE_SIDE1_DISPOSAL;
        battleTarget.zone = CARD_ZONE_SIDE1_BATTLE;
    }
    if (CardPlayChkCondition(definitions, ownSide, ownSide->hand.cards[index], work) != 0) {
        return 0;
    }
    held.index = index;
    held.cardId = ownSide->hand.cards[index];
    if (held.cardId == CARD_ID_MISSILE_POD || held.cardId == CARD_ID_EYEGLASSES) {
        if (BattleAreaCheckEnemy(side, work) < 2) {
            return 0;
        }
    }

    if (held.cardId == CARD_ID_SHIELD) {
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (ownSide->battle[i].battle.layers[0].cardId == CARD_ID_AG_02_M) {
                if (ownSide->battle[i].battle.layers[1].cardId < 0) {
                    battleTarget.index = i;
                    battleTarget.cardId = ownSide->battle[i].battle.layers[0].cardId;
                    ownSide->hand.playPoints[0] -= work->definitions[ownSide->hand.cards[index]].playCost;
                    CGPDisposeWeaponSub(side, work, held, battleTarget);
                    return 1;
                }
                if (ownSide->battle[i].battle.layers[1].cardId == CARD_ID_SABER &&
                    otherSide->battle[0].battle.layers[0].cardId < 0 &&
                    otherSide->battle[1].battle.layers[0].cardId < 0) {
                    battleTarget.index = i;
                    battleTarget.cardId = ownSide->battle[i].battle.layers[0].cardId;
                    ownSide->hand.playPoints[0] -= work->definitions[ownSide->hand.cards[index]].playCost;
                    CGPDisposeWeaponSub(side, work, held, battleTarget);
                    return 1;
                }
            }
        }
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (ownSide->disposal[i].battle.layers[0].cardId == CARD_ID_AG_02_M) {
                if (ownSide->disposal[i].battle.layers[1].cardId < 0) {
                    disposalTarget.index = i;
                    disposalTarget.cardId = ownSide->disposal[i].battle.layers[0].cardId;
                    ownSide->hand.playPoints[0] -= work->definitions[ownSide->hand.cards[index]].playCost;
                    CGPDisposeWeaponSub(side, work, held, disposalTarget);
                    return 1;
                }
                if (ownSide->disposal[i].battle.layers[1].cardId == CARD_ID_SABER) {
                    disposalTarget.index = i;
                    disposalTarget.cardId = ownSide->disposal[i].battle.layers[0].cardId;
                    ownSide->hand.playPoints[0] -= work->definitions[ownSide->hand.cards[index]].playCost;
                    CGPDisposeWeaponSub(side, work, held, disposalTarget);
                    return 1;
                }
            }
        }
    }

    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        slotCard[i] = -1;
    }
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        if (ownSide->battle[i].battle.layers[0].cardId >= 0 && ownSide->battle[i].battle.layers[1].cardId < 0) {
            battleTarget.index = i;
            battleTarget.cardId = ownSide->battle[i].battle.layers[0].cardId;
            if (battleTarget.cardId != CARD_ID_LV_1_JR && battleTarget.cardId != CARD_ID_LV_10_JR &&
                CGPWeaponEquipChk(work, held.cardId, battleTarget.cardId) != 0) {
                if (held.cardId == CARD_ID_SHIELD || held.cardId == CARD_ID_BATTLE_SUIT || held.cardId == CARD_ID_EYEGLASSES ||
                    definitions[held.cardId].attackType != definitions[battleTarget.cardId].attackType ||
                    definitions[held.cardId].attackPower > definitions[battleTarget.cardId].attackPower) {
                    if (held.cardId != CARD_ID_AIRD || definitions[battleTarget.cardId].attackType != CARD_ATTACK_BALLISTIC) {
                        slotCard[i] = battleTarget.cardId;
                    }
                }
            }
        }
    }

    bestSlot = -1;
    for (i = 3; i >= 0; i--) {
        if (slotCard[i] == -1) {
            continue;
        }
        if (held.cardId == CARD_ID_SABER) {
            if (i >= 2) {
                continue;
            }
            if (i == 0 && otherSide->battle[1].battle.layers[0].cardId < 0) {
                continue;
            }
            if (i == 1 && otherSide->battle[0].battle.layers[0].cardId < 0) {
                continue;
            }
        }
        if (bestSlot == -1) {
            bestSlot = i;
        } else if (held.cardId == CARD_ID_EYEGLASSES) {
            s32 power = CheckNowPower(side, work, CARD_SLOT_BATTLE | i, slotCard[i]);
            if (CheckNowPower(side, work, CARD_SLOT_BATTLE | bestSlot, slotCard[bestSlot]) < power) {
                bestSlot = i;
            }
        } else {
            if (GetDeckData(side, work, slotCard[bestSlot]) < 3) {
                if (GetDeckData(side, work, slotCard[i]) >= 3) {
                    bestSlot = i;
                    continue;
                }
            }
            if (ownSide->battle[bestSlot].battle.layers[0].life < ownSide->battle[i].battle.layers[0].life) {
                if (GetDeckData(side, work, slotCard[bestSlot]) < 3) {
                    bestSlot = i;
                }
            }
        }
    }
    if (held.cardId == CARD_ID_EYEGLASSES && bestSlot != -1) {
        if (CheckNowPower(side, work, CARD_SLOT_BATTLE | bestSlot, slotCard[bestSlot]) < 2) {
            if (BattleAreaCheckEnemy(side, work) < 3) {
                bestSlot = -1;
            }
        }
    }

    if (bestSlot != -1) {
        s32 attack[CARD_BOARD_SLOTS];
        s32 status[CARD_BOARD_SLOTS];
        s32 result;
        GetFriendAttack(side, work, attack, status);
        result = TodomeHyouka(side, work, 0, 0, attack, status);
        if (work->definitions[held.cardId].attackType == CARD_ATTACK_BALLISTIC) {
            status[bestSlot] = work->definitions[held.cardId].attackPower;
        } else if (work->definitions[held.cardId].attackType == CARD_ATTACK_HAND) {
            status[bestSlot] = work->definitions[held.cardId].attackPower | ATTACK_HAND;
        } else if (work->definitions[held.cardId].attackType == CARD_ATTACK_SPREAD) {
            status[bestSlot] = work->definitions[held.cardId].attackPower | ATTACK_SPREAD;
        } else if (work->definitions[held.cardId].attackType == CARD_ATTACK_HOMING) {
            status[bestSlot] = work->definitions[held.cardId].attackPower | ATTACK_HOMING;
        }
        if (held.cardId == CARD_ID_EYEGLASSES) {
            status[bestSlot] = CheckNowPower(side, work, CARD_SLOT_BATTLE | bestSlot, slotCard[bestSlot]) | ATTACK_SPREAD;
        }
        if (held.cardId == CARD_ID_CANNON || held.cardId == CARD_ID_AIRD || held.cardId == CARD_ID_BEAM) {
            if (slotCard[bestSlot] == CARD_ID_DRONE) {
                status[bestSlot] = ATTACK_HAND;
            }
            status[bestSlot] &= ~ATTACK_POWER_MASK;
        }
        if (ownSide->battle[bestSlot].battle.layers[0].cardId == CARD_ID_SIMEON ||
            ownSide->battle[bestSlot].battle.layers[0].cardId == CARD_ID_GARGOYLE) {
            status[bestSlot] |= ATTACK_SPLASH;
        }
        if (TodomeHyouka(side, work, 0, 0, attack, status) >= result) {
            if (!(ownSide->battle[bestSlot].battle.flags & CARD_STACK_FLAG_FROZEN)) {
                battleTarget.index = bestSlot;
                battleTarget.cardId = ownSide->battle[bestSlot].battle.layers[0].cardId;
                ownSide->hand.playPoints[0] -= work->definitions[ownSide->hand.cards[index]].playCost;
                CGPDisposeWeaponSub(side, work, held, battleTarget);
                return 1;
            }
        }
    }

    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        slotCard[i] = -1;
    }
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        if (ownSide->disposal[i].battle.layers[0].cardId >= 0 && ownSide->disposal[i].battle.layers[1].cardId < 0) {
            disposalTarget.index = i;
            disposalTarget.cardId = ownSide->disposal[i].battle.layers[0].cardId;
            if (disposalTarget.cardId != CARD_ID_LV_1_JR && disposalTarget.cardId != CARD_ID_LV_10_JR &&
                CGPWeaponEquipChk(work, held.cardId, disposalTarget.cardId) != 0) {
                if (held.cardId == CARD_ID_SHIELD || held.cardId == CARD_ID_BATTLE_SUIT || held.cardId == CARD_ID_EYEGLASSES ||
                    definitions[held.cardId].attackType != definitions[disposalTarget.cardId].attackType ||
                    definitions[held.cardId].attackPower > definitions[disposalTarget.cardId].attackPower) {
                    if (held.cardId != CARD_ID_SABER && held.cardId != CARD_ID_AIRD) {
                        slotCard[i] = disposalTarget.cardId;
                    }
                }
            }
        }
    }

    bestSlot = -1;
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        if (slotCard[i] == -1) {
            continue;
        }
        if (bestSlot == -1) {
            bestSlot = i;
        } else if (held.cardId == CARD_ID_EYEGLASSES) {
            if (work->definitions[slotCard[bestSlot]].attackPower < work->definitions[slotCard[i]].attackPower) {
                bestSlot = i;
            }
        } else {
            if (GetDeckData(side, work, slotCard[bestSlot]) < 3) {
                if (GetDeckData(side, work, slotCard[i]) >= 3) {
                    bestSlot = i;
                    continue;
                }
            }
            if (ownSide->disposal[bestSlot].battle.layers[0].life < ownSide->disposal[i].battle.layers[0].life) {
                if (GetDeckData(side, work, slotCard[bestSlot]) < 3) {
                    bestSlot = i;
                }
            }
        }
    }
    if (bestSlot != -1) {
        if (held.cardId == CARD_ID_EYEGLASSES && work->definitions[slotCard[bestSlot]].attackPower < 2) {
            return 0;
        }
        disposalTarget.index = bestSlot;
        disposalTarget.cardId = ownSide->disposal[bestSlot].battle.layers[0].cardId;
        ownSide->hand.playPoints[0] -= work->definitions[ownSide->hand.cards[index]].playCost;
        CGPDisposeWeaponSub(side, work, held, disposalTarget);
        return 1;
    }
    return 0;
}

/*
 * Starts the level-ten set phase when the side's hand card at handIndex is
 * Lv 10 Shion and one of its four battle slots holds Lv 1 Shion.
 */
s32 CC_Kara_SetLv10(s32 side, CardGameWork *work, s32 handIndex)
{
    CardPlaySide *sideData;
    CardHand *hand;
    s32 i;

    if (side == 0) {
        sideData = &work->player;
        hand = &work->player.hand;
    } else {
        sideData = &work->enemy;
        hand = &work->enemy.hand;
    }
    if (hand->cards[handIndex] == CARD_ID_LV_10_SHION) {
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (sideData->battle[i].battle.layers[0].cardId == CARD_ID_LV_1_SHION) {
                work->command.phaseTimer = 20;
                CGPSetFaseProc(side, work, sideData);
                return 1;
            }
        }
    }
    return 0;
}

s32 SetGunoFinish(s32 side, CardGameWork *work, s32 index)
{
    CardPlaySide *ownSide;
    s32 attack[CARD_BOARD_SLOTS];
    s32 status[CARD_BOARD_SLOTS];
    s32 cardId;
    s32 power;
    s32 bestSlot;
    s32 bestValue;
    s32 result;
    s32 reserve;
    s32 i;

    ownSide = &work->player;
    if (side != 0) {
        ownSide = &work->enemy;
    }
    cardId = ownSide->hand.cards[index];
    bestSlot = bestValue = -1;
    power =work->definitions[cardId].attackPower;

    if (work->definitions[cardId].attackType == CARD_ATTACK_HAND) {
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (ownSide->battle[i].battle.layers[0].cardId < 0) {
                GetFriendAttack(side, work, attack, status);
                status[i] = power + ATTACK_HAND;
                if (cardId == CARD_ID_GOBLIN) {
                    status[i] |= ATTACK_DOWNS_TARGET;
                }
                result = TodomeHyouka(side, work, i, i, attack, status);
                if (bestValue < result) {
                    bestSlot = i;
                    bestValue = result;
                }
            }
        }
    } else if (work->definitions[cardId].attackType == CARD_ATTACK_BALLISTIC) {
        for (i = 3; i >= 0; i--) {
            if (ownSide->battle[i].battle.layers[0].cardId < 0) {
                GetFriendAttack(side, work, attack, status);
                status[i] = power;
                result = TodomeHyouka(side, work, i, i, attack, status);
                if (bestValue < result) {
                    bestSlot = i;
                    bestValue = result;
                }
            }
        }
    } else {
        for (i = 3; i >= 0; i--) {
            if (ownSide->battle[i].battle.layers[0].cardId < 0) {
                bestSlot = i;
                break;
            }
        }
    }

    if (bestSlot != -1) {
        ownSide->battle[bestSlot].battle.flags = 0;
        ownSide->battle[bestSlot].battle.power = work->definitions[cardId].basePower;
        ownSide->battle[bestSlot].battle.powerLimit = work->definitions[cardId].basePower;
        ownSide->battle[bestSlot].battle.layers[0].cardId = ownSide->hand.cards[index];
        ownSide->battle[bestSlot].battle.layers[0].life = work->definitions[cardId].operationDuration;
        ownSide->battle[bestSlot].battle.position[0] = ownSide->handMove[index].position[0];
        ownSide->battle[bestSlot].battle.position[1] = ownSide->handMove[index].position[1];
        ownSide->battle[bestSlot].battle.position[2] = ownSide->handMove[index].position[2];
        ownSide->battle[bestSlot].battle.step[0] = (BattlePos1P[bestSlot][0] - ownSide->handMove[index].position[0]) / 10.0f;
        ownSide->battle[bestSlot].battle.step[1] = (BattlePos1P[bestSlot][1] - ownSide->handMove[index].position[1]) / 10.0f;
        ownSide->battle[bestSlot].battle.step[2] = (BattlePos1P[bestSlot][2] - ownSide->handMove[index].position[2]) / 10.0f;
        ownSide->battle[bestSlot].battle.moveFrames = CARD_SLIDE_FRAMES;
    } else {
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (ownSide->disposal[i].battle.layers[0].cardId < 0) {
                ownSide->disposal[i].battle.flags = 0;
                ownSide->disposal[i].battle.power = work->definitions[cardId].basePower;
                ownSide->disposal[i].battle.powerLimit = work->definitions[cardId].basePower;
                ownSide->disposal[i].battle.layers[0].cardId = ownSide->hand.cards[index];
                ownSide->disposal[i].battle.layers[0].life = work->definitions[cardId].operationDuration;
                ownSide->disposal[i].battle.position[0] = ownSide->handMove[index].position[0];
                ownSide->disposal[i].battle.position[1] = ownSide->handMove[index].position[1];
                ownSide->disposal[i].battle.position[2] = ownSide->handMove[index].position[2];
                ownSide->disposal[i].battle.step[0] = (BattlePos1P[bestSlot][0] - ownSide->handMove[index].position[0]) / 10.0f;
                ownSide->disposal[i].battle.step[1] = (BattlePos1P[bestSlot][1] - ownSide->handMove[index].position[1]) / 10.0f;
                ownSide->disposal[i].battle.step[2] = (BattlePos1P[bestSlot][2] - ownSide->handMove[index].position[2]) / 10.0f;
                ownSide->disposal[i].battle.moveFrames = CARD_SLIDE_FRAMES;
                break;
            }
        }
    }

    reserve = work->definitions[cardId].reserveCost;
    ownSide->hand.cards[index] = -1;
    ownSide->hand.playPoints[0] -= work->definitions[cardId].playCost;
    if (reserve != 0) {
        i = reserve;
        do {
            i -= 1;
            CardPlayCostCard(ownSide);
        } while (i != 0);
    }
    CardPlayCleanHand(&ownSide->hand);
    return 1;
}

/*
 * Guno placement for hand card `handIndex`: counts the card's discard
 * requirements, collects the disposal and battle slots whose price or reward
 * is below the card's CardDat value, and when there are enough of them plays
 * the chosen slots away through CardPlayDesertBattle before SetGunoFinish.
 * Returns 1 when SetGunoFinish ran.
 */
s32 CC_Kara_SetGuno(s32 side, CardGameWork *work, s32 handIndex)
{
    CardPlaySide *ownSide = (side == 0) ? &work->player : &work->enemy;
    u8 *requirement;
    s32 price = CardDat[ownSide->hand.cards[handIndex]];
    s32 discards = 0;
    s32 candidates;
    s32 remaining;
    s32 disposalCardId[CARD_BOARD_SLOTS];
    s32 battleCardId[CARD_BOARD_SLOTS];
    s32 *picked;
    CardBoardSlot *slot;
    CardLayerStack *stack;
    s32 i;
    s32 cardId;
    s32 cardKind;

    remaining = 3;
    cardId = ownSide->hand.cards[handIndex];
    cardKind = work->definitions[cardId].kind;
    requirement = work->definitions[cardId].discardRequirements;
    do {
        if (*requirement != 0) {
            discards++;
        }
        requirement++;
        remaining -= 1;
    } while (remaining >= 0);

    candidates = 0;
    for (remaining = 0; remaining < CARD_BOARD_SLOTS; remaining++) {
        disposalCardId[remaining] = -1;
        if (ownSide->disposal[remaining].battle.layers[0].cardId >= 0 &&
            (work->definitions[ownSide->disposal[remaining].battle.layers[0].cardId].flags & CARD_ATTR_HUMAN) &&
            !(ownSide->disposal[remaining].battle.flags & CARD_STACK_FLAG_END) &&
            GetPrice(&ownSide->disposal[remaining].battle) < price) {
            candidates++;
            disposalCardId[remaining] = ownSide->disposal[remaining].battle.layers[0].cardId;
        }
    }

    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        battleCardId[i] = -1;
        if (ownSide->battle[i].battle.layers[0].cardId >= 0 &&
            (work->definitions[ownSide->battle[i].battle.layers[0].cardId].flags & CARD_ATTR_HUMAN) &&
            GetShoukin(&ownSide->battle[i].battle) < price) {
            candidates++;
            battleCardId[i] = ownSide->battle[i].battle.layers[0].cardId;
        }
    }

    if (candidates < discards) {
        return 0;
    }
    if (BattleAreaCheckFriend(side, work) == 4) {
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (battleCardId[i] != -1) {
                CardPlayDesertBattle(ownSide, &ownSide->battle[i].battle);
                discards--;
            }
            if (discards <= 0) {
                SetGunoFinish(side, work, handIndex);
                return 1;
            }
        }
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (disposalCardId[i] != -1) {
                CardPlayDesertBattle(ownSide, &ownSide->disposal[i].battle);
                discards--;
            }
            if (discards <= 0) {
                SetGunoFinish(side, work, handIndex);
                return 1;
            }
        }
    } else {
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (disposalCardId[i] != -1) {
                CardPlayDesertBattle(ownSide, &ownSide->disposal[i].battle);
                discards--;
            }
            if (discards <= 0) {
                SetGunoFinish(side, work, handIndex);
                return 1;
            }
        }
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (battleCardId[i] != -1) {
                CardPlayDesertBattle(ownSide, &ownSide->battle[i].battle);
                discards--;
            }
            if (discards <= 0) {
                SetGunoFinish(side, work, handIndex);
                return 1;
            }
        }
    }
    return 0;
}

s32 CC_Kara_SetFase_1(s32 side, CardGameWork *work)
{
    CardDefinition *definitions;
    CardPlaySide *ownSide;
    CardPlaySide *otherSide;
    s32 handPick[40];
    s16 sideScratch[3][8];
    s32 i;
    s32 cost;
    s32 handCount;
    s32 yama;
    s32 pickA;
    s32 pickB;
    s32 price1;
    s32 price2;
    s32 emptyDisposal;

    CC_Kara_SetOpe(side, work);
    definitions = work->definitions;
    if (side == 0) {
        sideScratch[0][0] = 9;
        sideScratch[1][0] = CARD_ZONE_SIDE0_DISPOSAL;
        sideScratch[2][0] = CARD_ZONE_SIDE0_BATTLE;
        ownSide = &work->player;
        otherSide = &work->enemy;
    } else {
        sideScratch[1][0] = CARD_ZONE_SIDE1_DISPOSAL;
        sideScratch[2][0] = CARD_ZONE_SIDE1_BATTLE;
        sideScratch[0][0] = 0;
        ownSide = &work->enemy;
        otherSide = &work->player;
    }

    for (i = 0; i < 40; i++) {
        handPick[i] = -1;
    }
    handCount = CardPlayHandCnt(&ownSide->hand);
    for (i = 0; i < handCount; i++) {
        if (definitions[ownSide->hand.cards[i]].kind == CARD_KIND_BATTLE) {
            handPick[i] = CGPDispoSub0(side, work, i);
        }
    }

    yama = CardPlayYamaCnt(ownSide);
    if (yama >= 5) {
        for (i = 0; i < handCount; i++) {
            if (ownSide->hand.cards[i] == CARD_ID_LV_10_SHION || ownSide->hand.cards[i] == CARD_ID_LV_10_ZIGGY ||
                ownSide->hand.cards[i] == CARD_ID_LV_10_MOMO || ownSide->hand.cards[i] == CARD_ID_LV_10_KOS_MOS ||
                ownSide->hand.cards[i] == CARD_ID_LV_10_JR || ownSide->hand.cards[i] == CARD_ID_LV_10_CHAOS) {
                if (CGPEnemyLv10SetSub(side, work, i) == 0) {
                    return 1;
                }
            }
        }
    }

    if (yama < 10) {
        for (i = 0; i < handCount; i++) {
            if (handPick[i] == -1) {
                continue;
            }
            cost = CGPChkCardPlayCost(work, ownSide, ownSide->hand.cards[i]);
            if (cost == 1) {
                if (yama - 1 < 4) {
                    handPick[i] = -1;
                }
            } else if (cost != 0 && yama - cost < 6) {
                handPick[i] = -1;
            }
        }
    }

    emptyDisposal = 0;
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        if (ownSide->disposal[i].battle.layers[0].cardId < 0) {
            emptyDisposal = 1;
        }
    }
    if (emptyDisposal == 0) {
        for (i = 0; i < handCount; i++) {
            if (handPick[i] > 15 || handPick[i] < 13) {
                handPick[i] = -1;
            }
        }
    }

    pickA = -1;
    pickB = -1;
    for (i = 0; i < handCount; i++) {
        if (handPick[i] == -1) {
            continue;
        }
        switch (handPick[i]) {
        case 0:
            if (pickA == -1) {
                pickA = i;
            } else {
                price2 = ownSide->hand.cards[i];
                price1 = ownSide->hand.cards[pickA];
                if (CardDat[price2] > CardDat[price1]) {
                    pickA = i;
                }
            }
            break;
        case 15:
            if (pickB == -1) {
                pickB = i;
                if (VX10000Check(side, work) == 1 &&
                    work->definitions[ownSide->hand.cards[i]].attackType == CARD_ATTACK_HOMING) {
                    pickB = -1;
                    continue;
                }
            }
            price2 = ownSide->hand.cards[i];
            price1 = ownSide->hand.cards[pickB];
            if (CardDat[price2] > CardDat[price1]) {
                pickB = i;
            }
            if (ownSide->hand.cards[pickB] == CARD_ID_GOBLIN && handCount < 8 &&
                otherSide->battle[0].battle.layers[0].cardId < 0 &&
                otherSide->battle[1].battle.layers[0].cardId < 0) {
                pickB = -1;
            }
            break;
        }
    }

    price1 = 0;
    price2 = 0;
    if (pickA != -1) {
        price1 = CardDat[ownSide->hand.cards[pickA]];
    }
    if (pickB != -1) {
        price2 = CardDat[ownSide->hand.cards[pickB]];
    }

    for (i = 0; i < handCount; i++) {
        if (handPick[i] == -1) {
            continue;
        }
        if (handPick[i] >= 15) {
            continue;
        }
        if (handPick[i] < 13) {
            continue;
        }
        if (ownSide->hand.cards[i] == CARD_ID_EYEGLASSES) {
            if (CC_Kara_SetWeapon(side, work, i) == 1) {
                return 1;
            }
        }
    }

    if (price2 < price1) {
        if (pickA != -1) {
            ownSide->hand.playPoints[0] -= work->definitions[ownSide->hand.cards[pickA]].playCost;
            CardPlayDisployment(side, work, ownSide, pickA);
            return 1;
        }
        if (pickB != -1 && AlbedoCheck(side, work) == -1 && CC_Kara_SetGuno(side, work, pickB) == 1) {
            return 1;
        }
    } else {
        if (pickB != -1 && AlbedoCheck(side, work) == -1 && CC_Kara_SetGuno(side, work, pickB) == 1) {
            return 1;
        }
        if (pickA != -1) {
            ownSide->hand.playPoints[0] -= work->definitions[ownSide->hand.cards[pickA]].playCost;
            CardPlayDisployment(side, work, ownSide, pickA);
            return 1;
        }
    }

    for (i = 0; i < handCount; i++) {
        if (handPick[i] == -1) {
            continue;
        }
        if (handPick[i] >= 15) {
            continue;
        }
        if (handPick[i] < 13) {
            continue;
        }
        if (ownSide->hand.cards[i] == CARD_ID_AIRD && VX10000Check(side, work) == 1) {
            continue;
        }
        if (CC_Kara_SetWeapon(side, work, i) == 1) {
            return 1;
        }
    }

    for (i = 0; i < handCount; i++) {
        if (ownSide->hand.cards[i] == CARD_ID_LV_10_SHION || ownSide->hand.cards[i] == CARD_ID_LV_10_ZIGGY ||
            ownSide->hand.cards[i] == CARD_ID_LV_10_MOMO || ownSide->hand.cards[i] == CARD_ID_LV_10_KOS_MOS ||
            ownSide->hand.cards[i] == CARD_ID_LV_10_JR || ownSide->hand.cards[i] == CARD_ID_LV_10_CHAOS) {
            if (CGPEnemyLv10SetSub(side, work, i) == 0) {
                return 1;
            }
        }
    }
    return 0;
}

s32 CC_Kara_CommFase(s32 side, CardGameWork *work)
{
    /* The command card chosen from the hand and its hand index. */
    static s32 cardno = 0;
    static s32 no = 0;
    CardPlaySide *ownSide;
    s32 handCount;
    s32 i;
    s32 cardId;
    s32 delta;

    i = 0;
    delta = 0;
    if (side == 0) {
        ownSide = &work->player;
    } else {
        ownSide = &work->enemy;
    }
    handCount = CardPlayHandCnt(&ownSide->hand);
    if (work != 0) {
        i += delta;
        delta += i;
    }
    switch (work->aiStep) {
    case 0: {
        i = 0;
        if (handCount > i) {
            do {
                cardId = ownSide->hand.cards[i];

                switch (cardId) {
                case CARD_ID_SUPPLY_SHIP:
                case CARD_ID_SUPPLY_FLEET:
                    if (CheckCommandPlay(side, work, cardId)) {
                        if (5 <= CardPlaySuteCnt(ownSide)) {
                            no = i;
                            cardno = ownSide->hand.cards[no];
                            work->aiStep = 1;
                            return 0;
                        }
                    }
                    break;
                case CARD_ID_MARY_AND_SHELLEY:
                    if (CheckCommandPlay(side, work, CARD_ID_MARY_AND_SHELLEY) && CardPlayYamaCnt(ownSide) >= 11) {
                        if (7 > handCount) {
                            no = i;
                            cardno = ownSide->hand.cards[no];
                            work->aiStep = 1;
                            return 0;
                        }
                    }
                    break;
                case CARD_ID_UMN_INVASION:
                    if (CheckCommandPlay(side, work, CARD_ID_UMN_INVASION) && CardPlaySuteCnt(ownSide) >= 2) {
                        if (7 > handCount) {
                            cardno = ownSide->hand.cards[i];
                            no = i;
                            work->aiStep = 1;
                            return 0;
                        }
                    }
                    break;
                }
                i += 1;
            } while (i < handCount);
        }
        break;
    }
    case 1: {
        s32 ret = CardPlayCommandPlay(side, work->commandIds[cardno], work, 0, 0);

        if (ret != 0) {
            CGPCommFinishSub(work, ownSide, no);
            work->aiStep = 0;
            return 0;
        }
        return ret;
    }
    }
    return 1;
}

s32 CheckDamageCommand(s32 side, CardGameWork *work, s32 threshold)
{
    s32 attack[CARD_BOARD_SLOTS];
    s32 status[CARD_BOARD_SLOTS];
    s32 total;
    s16 *cardCursor;
    s32 bonus;
    CardPlaySide *ownSide;
    CardPlaySide *otherSide;
    s32 handCount;
    s32 baseline;
    s32 bestValue;
    s32 phase;
    s16 *opCursor;
    s32 remaining;
    s32 i;
    u16 cardId;
    s32 value;
    s32 score;
    s32 bestSlot;

    bestSlot = -1;
    if (side == 0) {
        ownSide = &work->player;
        otherSide = &work->enemy;
    } else {
        ownSide = &work->enemy;
        otherSide = &work->player;
    }

    bonus = 0;
    handCount = CardPlayHandCnt(&ownSide->hand);
    if (handCount > 0) {
        cardCursor = ownSide->hand.cards;
        i = handCount;
        do {
            cardId = (u16) *cardCursor;
            cardCursor++;
            if (cardId - CARD_ID_SURPRISE_ATTACK < 2U) {
                if (CheckCommandPlay(side, work, (s16) cardId) == 1) {
                    bonus += 2;
                }
            }
            i -= 1;
        } while (i != 0);
    }
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        if (ownSide->operations[i] == CARD_ID_SO_WEAK) {
            bonus += 3;
        }
    }

    if (bonus > threshold) {
        threshold = bonus;
    }
    phase = work->command.phase;
    if (phase == CARD_PHASE_EVENT || phase == CARD_PHASE_EVENT + CARD_PHASES_PER_SIDE) {
        GetFriendAttack(side, work, attack, status);
        bestValue = TodomeHyouka(side, work, 0, 0, attack, status);
        baseline = bestValue;
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (otherSide->battle[i].battle.layers[0].cardId >= 0) {
                GetFriendAttack(side, work, attack, status);
                total = attack[i] - threshold;
                if (total > 0) {
                    value = (GetShoukin(&otherSide->battle[i].battle) / 10.0f) * threshold;
                    attack[i] = attack[i] - threshold;
                } else {
                    value = GetShoukin(&otherSide->battle[i].battle) - total;
                    attack[i] = 0;
                }
                total = value + TodomeHyouka(side, work, 0, 0, attack, status);
                if (total > bestValue) {
                    bestValue = total;
                    bestSlot = CARD_SLOT_BATTLE | i;
                }
            }
        }
        if ((baseline + threshold * 13) >= bestValue) {
            bestSlot = -1;
        }
        return bestSlot;
    } else {
        bestValue = 0;
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (otherSide->battle[i].battle.layers[0].cardId >= 0) {
                GetFriendAttack(side, work, attack, status);
                total = attack[i] - threshold;
                if (0 < total) {
                    score = (GetShoukin(&otherSide->battle[i].battle) / 10.0f) * threshold;
                    attack[i] = attack[i] - threshold;
                } else {
                    score = GetShoukin(&otherSide->battle[i].battle) - total;
                    attack[i] = 0;
                }
                if (bestValue < score) {
                    bestValue = score;
                    bestSlot = CARD_SLOT_BATTLE | i;
                }
            }
        }
        if (bestValue > threshold * 10) {
            return bestSlot;
        }
        bestSlot = -1;
        return bestSlot;
    }
}

s32 CheckTaikyakuCommand(s32 side, CardGameWork *work)
{
    s32 bonus;
    s32 albedo;
    s32 attack[CARD_BOARD_SLOTS];
    s32 status[CARD_BOARD_SLOTS];
    s32 initialScore;
    s32 i;
    CardPlaySide *ownSide;
    CardPlaySide *otherSide;
    s32 handCount;
    s32 total;
    s32 bestSlot;
    s32 best;
    s32 phase;
    u16 cardId;

    bestSlot = -1;
    if (side == 0) {
        ownSide = &work->player;
        otherSide = &work->enemy;
    } else {
        ownSide = &work->enemy;
        otherSide = &work->player;
    }

    albedo = AlbedoCheck(side, work);
    if (albedo != -1) {
        bonus = 0;
        handCount = CardPlayHandCnt(&ownSide->hand);
        for (i = 0; i < handCount; i++) {
            cardId = (u16) ownSide->hand.cards[i];
            if (cardId - CARD_ID_RETREAT < 2U) {
                if (CheckCommandPlay(side, work, (s16) cardId) == 1) {
                    bonus += 1;
                }
            }
        }
        if (bonus >= 2) {
            switch (albedo) {
            case 0:
            case 2:
                if (otherSide->battle[1].battle.layers[0].cardId >= 0) {
                    bestSlot = 1;
                } else if (otherSide->battle[3].battle.layers[0].cardId >= 0) {
                    bestSlot = 3;
                }
                return bestSlot;
            case 1:
            case 3:
                if (otherSide->battle[0].battle.layers[0].cardId >= 0) {
                    bestSlot = 0;
                } else if (otherSide->battle[2].battle.layers[0].cardId >= 0) {
                    bestSlot = 2;
                }
                return bestSlot;
            }
        }
    }

    phase = work->command.phase;
    if (phase == CARD_PHASE_BLOCK + CARD_PHASES_PER_SIDE || phase == CARD_PHASE_BLOCK) {
        GetFriendAttack(side == 0, work, attack, status);
        best = TodomeHyouka(side == 0, work, 0, 0, attack, status);
        initialScore = best;
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (otherSide->battle[i].battle.layers[0].cardId >= 0) {
                GetFriendAttack(side == 0, work, attack, status);
                status[i] = 0;
                total = TodomeHyouka(side == 0, work, 0, 0, attack, status);
                if (total < best) {
                    best = total;
                    bestSlot = CARD_SLOT_BATTLE | i;
                }
            }
        }
        if (initialScore < best + TODOME_MARGIN) {
            bestSlot = -1;
        }
    } else {
        GetFriendAttack(side, work, attack, status);
        best = TodomeHyouka(side, work, 0, 0, attack, status);
        initialScore = best;
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (otherSide->battle[i].battle.layers[0].cardId >= 0) {
                GetFriendAttack(side, work, attack, status);
                attack[i] = 0;
                total = TodomeHyouka(side, work, 0, 0, attack, status);
                if (best < total) {
                    best = total;
                    bestSlot = i | CARD_SLOT_BATTLE;
                }
            }
        }
        if (best < initialScore + TODOME_MARGIN) {
            bestSlot = -1;
        }
    }
    return bestSlot;
}

/* Battle slot of the other side whose TodomeHyouka score is lowest and more
 * than 30 below the score of the initial board (-1 if none qualifies). */
s32 CheckTotugekiCommand(s32 side, CardGameWork *work)
{
    s32 slotIndex;
    s32 bestSlot;
    CardPlaySide *otherSide = (side == 0) ? &work->enemy : &work->player;
    s32 attackVectors[CARD_BOARD_SLOTS];
    s32 statusVectors[CARD_BOARD_SLOTS];
    s32 best;
    s32 value;
    s32 initial;
    s32 *initialPtr = &initial;

    bestSlot = -1;
    initial = 0;
    slotIndex = initial;
    if (work->command.phase == CARD_PHASE_EVENT || work->command.phase == CARD_PHASE_EVENT + CARD_PHASES_PER_SIDE) {
        return -1;
    }
    GetFriendAttack(side == 0, work, attackVectors, statusVectors);
    initial = TodomeHyouka(side == 0, work, 0, 0, attackVectors, statusVectors);
    best = initial;
    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        if (otherSide->battle[slotIndex].battle.layers[0].cardId < 0) {
            continue;
        }
        GetFriendAttack(side == 0, work, attackVectors, statusVectors);
        value = TodomeHyouka(side == 0, work, 0, 0, attackVectors, statusVectors);
        if (value < best) {
            best = value;
            bestSlot = slotIndex;
        }
    }
    if (best + TODOME_MARGIN < initial) {
        return -1;
    }
    return bestSlot;
}

s32 CheckKonranCommand(s32 side, CardGameWork *work)
{
    CardPlaySide *target;
    s32 attackVec[CARD_BOARD_SLOTS];
    s32 statusVec[CARD_BOARD_SLOTS];
    CardStackSpan savedBattle[CARD_BOARD_SLOTS];
    s32 score;
    s32 rescore;
    s32 other;
    CardBoardSlot *battleBase;
    s32 i = 0;
    s16 phase;
    s32 insufficient;

    target = (side == 0) ? &work->enemy : &work->player;
    phase = work->command.phase;
    if (phase == CARD_PHASE_EVENT || phase == CARD_PHASE_EVENT + CARD_PHASES_PER_SIDE) {
        GetFriendAttack(side, work, attackVec, statusVec);
        score = TodomeHyouka(side, work, 0, 0, attackVec, statusVec);
        battleBase = target->battle;
        for (i = 0; i < CARD_BOARD_SLOTS; battleBase++, i++) {
            savedBattle[i] = *(CardStackSpan *)&battleBase->battle;
        }
        *(CardStackSpan *)&target->battle[0].battle = savedBattle[2];
        *(CardStackSpan *)&target->battle[1].battle = savedBattle[3];
        *(CardStackSpan *)&target->battle[2].battle = savedBattle[0];
        *(CardStackSpan *)&target->battle[3].battle = savedBattle[1];
        GetFriendAttack(side, work, attackVec, statusVec);
        rescore = TodomeHyouka(side, work, 0, 0, attackVec, statusVec);
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            s32 slotOffset = i * 444 + 400;
            CardBoardSlot *slot = (CardBoardSlot *)((u8 *)target + slotOffset);
            *(CardStackSpan *)&slot->battle = savedBattle[i];
        }
        insufficient = rescore < score + TODOME_MARGIN;
        if (insufficient) {
            return 0;
        }
        return 1;
    }
    other = (side == 0);
    GetFriendAttack(other, work, attackVec, statusVec);
    score = TodomeHyouka(other, work, 0, 0, attackVec, statusVec);
    for (; i < CARD_BOARD_SLOTS; i++) {
        s32 slotOffset = i * 444 + 400;
        CardBoardSlot *slot = (CardBoardSlot *)((u8 *)target + slotOffset);
        CardStackSpan *dest = &savedBattle[i];
        CardStackSpan *src = (CardStackSpan *)&slot->battle;
        *dest = *src;
    }
    *(CardStackSpan *)&target->battle[0].battle = savedBattle[2];
    *(CardStackSpan *)&target->battle[1].battle = savedBattle[3];
    *(CardStackSpan *)&target->battle[2].battle = savedBattle[0];
    *(CardStackSpan *)&target->battle[3].battle = savedBattle[1];
    GetFriendAttack(other, work, attackVec, statusVec);
    rescore = TodomeHyouka(other, work, 0, 0, attackVec, statusVec);
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        s32 slotOffset = i * 444 + 400;
        CardBoardSlot *slot = (CardBoardSlot *)((u8 *)target + slotOffset);
        *(CardStackSpan *)&slot->battle = savedBattle[i];
    }
    insufficient = score < rescore + TODOME_MARGIN;
    if (insufficient) {
        return 0;
    }
    return 1;
}

/*
 * Chooses a curry target: the battle slot of the side whose operation
 * duration is lowest, or -1 when the command is not played now.
 */
s32 CheckCurryCommand(s32 side, CardGameWork *work)
{
    s32 slotIndex;
    s32 bestSlot;
    CardPlaySide *ownSide = (side == 0) ? &work->player : &work->enemy;
    s32 durations[CARD_BOARD_SLOTS];
    s32 flags[CARD_BOARD_SLOTS];
    s32 best;
    s32 value;
    s32 initial;

    bestSlot = -1;
    initial = 0;
    slotIndex = initial;
    if (work->command.phase == CARD_PHASE_EVENT || work->command.phase == CARD_PHASE_EVENT + CARD_PHASES_PER_SIDE) {
        return -1;
    }
    GetFriendAttack(side == 0, work, durations, flags);
    for (slotIndex = initial; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        flags[slotIndex] &= ~ATTACK_DOWNS_TARGET;
    }
    initial = TodomeHyouka(side == 0, work, 0, 0, durations, flags);
    best = initial;
    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        CardBoardSlot *slot = &ownSide->battle[slotIndex];
        s16 cardId = slot->battle.layers[0].cardId;

        if (cardId < 0) {
            continue;
        }
        if (work->definitions[cardId].operationDuration - slot->battle.layers[0].life < 2) {
            continue;
        }
        if (work->definitions[cardId].flags == CARD_ATTR_GNOSIS) {
            continue;
        }
        GetFriendAttack(side == 0, work, durations, flags);
        {
            s32 k;

            for (k = 0; k < CARD_BOARD_SLOTS; k++) {
                flags[k] &= 0x00F0FFFF;
            }
        }
        durations[slotIndex] = work->definitions[slot->battle.layers[0].cardId].operationDuration;
        value = TodomeHyouka(side == 0, work, 0, 0, durations, flags);
        if (value < best) {
            best = value;
            bestSlot = slotIndex | CARD_SLOT_BATTLE;
        }
    }
    if (initial < best + TODOME_MARGIN_SMALL) {
        return -1;
    }
    return bestSlot;
}

s32 CheckTentCommand(s32 side, CardGameWork *work)
{
    s16 *cardIdPtr;
    CardPlaySide *ownSide;
    s32 durations[CARD_BOARD_SLOTS];
    s32 flags[CARD_BOARD_SLOTS];
    s32 value;
    s32 slotIndex;
    s32 initial;
    s32 below;
    s32 result;

    initial = 0;
    slotIndex = initial;
    ownSide = (side == 0) ? &work->player : &work->enemy;
    if (work->command.phase == CARD_PHASE_EVENT || work->command.phase == CARD_PHASE_EVENT + CARD_PHASES_PER_SIDE) {
        result = 0;
    } else {
        GetFriendAttack(side == 0, work, durations, flags);
        for (slotIndex = initial; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
            flags[slotIndex] &= ~ATTACK_DOWNS_TARGET;
        }
        cardIdPtr = &ownSide->battle[0].battle.layers[0].cardId;
        initial = TodomeHyouka(side == 0, work, 0, 0, durations, flags);
        for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
            s16 cardId = *cardIdPtr;

            cardIdPtr += CARD_SLOT_HALFWORDS;
            if (cardId < 0 || work->definitions[cardId].flags == CARD_ATTR_GNOSIS) {
                continue;
            }
            durations[slotIndex] += 3;
            if (work->definitions[cardId].operationDuration < durations[slotIndex]) {
                durations[slotIndex] = work->definitions[cardId].operationDuration;
            }
        }
        value = TodomeHyouka(side == 0, work, 0, 0, durations, flags);
        slotIndex = 0;
        while (slotIndex < 4) {
            flags[slotIndex] &= 0x00F0FFFF;
            slotIndex++;
        }
        below = initial < value + TODOME_MARGIN_SMALL;
        result = below == 0;
    }
    return result;
}

/* Compares the reward totals of the two battle boards: returns 1 when the
 * caller's side collects less than the other side (flags 4 and 8 cards). */
s32 CheckSouteniHouCommand(s32 side, CardGameWork *work)
{
    CardPlaySide *ownSide;
    CardPlaySide *opponent;
    s16 cardId;
    s32 slotIndex;
    s32 opponentTotal = 0;
    s32 ownTotal = 0;

    if (side == 0) {
        ownSide = &work->player;
        opponent = &work->enemy;
    } else {
        ownSide = &work->enemy;
        opponent = &work->player;
    }
    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        cardId = ownSide->battle[slotIndex].battle.layers[0].cardId;
        if (cardId >= 0 && (work->definitions[cardId].flags == CARD_ATTR_GNOSIS || work->definitions[cardId].flags == CARD_ATTR_MECH)) {
            ownTotal += GetShoukin(&ownSide->battle[slotIndex].battle);
        }
        cardId = opponent->battle[slotIndex].battle.layers[0].cardId;
        if (cardId >= 0 && (work->definitions[cardId].flags == CARD_ATTR_GNOSIS || work->definitions[cardId].flags == CARD_ATTR_MECH)) {
            opponentTotal += GetShoukin(&opponent->battle[slotIndex].battle);
        }
    }
    return ownTotal < opponentTotal;
}

s32 CheckHatudouCommand(s32 side, CardGameWork *work)
{
    s32 slotIndex;
    CardPlaySide *ownSide;
    CardPlaySide *opponent;
    s32 opponentTotal = 0;
    s32 ownTotal = 0;

    if (side == 0) {
        ownSide = &work->player;
        opponent = &work->enemy;
    } else {
        ownSide = &work->enemy;
        opponent = &work->player;
    }
    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        if (ownSide->battle[slotIndex].battle.layers[0].cardId >= 0) {
            ownTotal += GetShoukin(&ownSide->battle[slotIndex].battle);
        }
        if (ownSide->disposal[slotIndex].battle.layers[0].cardId >= 0) {
            ownTotal += GetPrice(&ownSide->disposal[slotIndex].battle);
        }
        if (opponent->battle[slotIndex].battle.layers[0].cardId >= 0) {
            opponentTotal += GetShoukin(&opponent->battle[slotIndex].battle);
        }
        if (opponent->disposal[slotIndex].battle.layers[0].cardId >= 0) {
            opponentTotal += GetPrice(&opponent->disposal[slotIndex].battle);
        }
    }
    return ownTotal < opponentTotal;
}

s32 CheckXDamageCommand(s32 side, CardGameWork *work, s32 damage)
{
    s32 slotIndex;
    s32 otherTotal = 0;
    s32 ownTotal = 0;
    CardPlaySide *ownSide;
    CardPlaySide *otherSide;

    if (side == 0) {
        ownSide = &work->player;
        otherSide = &work->enemy;
    } else {
        ownSide = &work->enemy;
        otherSide = &work->player;
    }
    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        if (ownSide->disposal[slotIndex].battle.layers[0].cardId >= 0) {
            if (ownSide->disposal[slotIndex].battle.layers[0].life - damage <= 0) {
                ownTotal += GetPrice(&ownSide->disposal[slotIndex].battle);
            } else {
                s32 price = GetPrice(&ownSide->disposal[slotIndex].battle);
                ownTotal = ownTotal + price / 10.0 * damage;
            }
        }
        if (otherSide->disposal[slotIndex].battle.layers[0].cardId >= 0) {
            if (otherSide->disposal[slotIndex].battle.layers[0].life - damage <= 0) {
                otherTotal += GetPrice(&otherSide->disposal[slotIndex].battle);
            } else {
                s32 price = GetPrice(&otherSide->disposal[slotIndex].battle);
                otherTotal = otherTotal + price / 10.0 * damage;
            }
        }
        if (ownSide->battle[slotIndex].battle.layers[0].cardId >= 0) {
            if (ownSide->battle[slotIndex].battle.layers[0].life - damage <= 0) {
                ownTotal += GetShoukin(&ownSide->battle[slotIndex].battle);
            } else {
                s32 reward = GetShoukin(&ownSide->battle[slotIndex].battle);
                ownTotal = ownTotal + reward / 10.0 * damage;
            }
        }
        if (otherSide->battle[slotIndex].battle.layers[0].cardId >= 0) {
            if (otherSide->battle[slotIndex].battle.layers[0].life - damage <= 0) {
                otherTotal += GetShoukin(&otherSide->battle[slotIndex].battle);
            } else {
                s32 reward = GetShoukin(&otherSide->battle[slotIndex].battle);
                otherTotal = otherTotal + reward / 10.0 * damage;
            }
        }
    }
    return otherTotal > ownTotal;
}

/* Reward total of the caller's battle slots 2 and 3: a slot counts 90% of
 * its reward when its base card is set. Returns whether the total exceeds 80. */
s32 CheckMilCommand(s32 side, CardGameWork *work)
{
    CardPlaySide *sideData = (side == 0) ? &work->enemy : &work->player;
    s32 reward = 0;

    if (sideData->battle[2].battle.layers[0].cardId >= 0) {
        reward = GetShoukin(&sideData->battle[2].battle) * 0.9f + reward;
    }
    if (sideData->battle[3].battle.layers[0].cardId >= 0) {
        reward = reward + GetShoukin(&sideData->battle[3].battle) * 0.9f;
    }
    if (reward < 81) {
        return 0;
    }
    return 1;
}

/* Requires an empty opposing front (battle slots 0 and 1), then checks whether
 * the friendly melee power totals at least four. */
s32 CheckNazoCommand(s32 side, CardGameWork *work)
{
    CardPlaySide *ownSide;
    CardPlaySide *opponent;
    s32 total = 0;
    s32 slotIndex;

    if (side == 0) {
        ownSide = &work->player;
        opponent = &work->enemy;
    } else {
        ownSide = &work->enemy;
        opponent = &work->player;
    }
    if (opponent->battle[0].battle.layers[0].cardId < 0 &&
        opponent->battle[1].battle.layers[0].cardId < 0) {
        for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
            if (ownSide->battle[slotIndex].battle.layers[0].cardId >= 0 &&
                CardChkAttackType(work->definitions, &ownSide->battle[slotIndex].battle) == CARD_ATTACK_HAND) {
                total += CheckNowPower(side, work, slotIndex | CARD_SLOT_BATTLE,
                                       ownSide->battle[slotIndex].battle.layers[0].cardId);
            }
        }
    }
    if (total < 4) {
        return 0;
    }
    return 1;
}

/*
 * Target for the Seimitsu command: the flagged battle or disposal slot whose
 * adjusted reward plus TodomeHyouka score beats the baseline score by at
 * least 15, encoded as a slot index (battle slots carry the 0xF0000 tag);
 * -1 when none does.
 */
s32 CheckSeimitsuCommand(s32 side, CardGameWork *work)
{
    CardPlaySide *otherSide = (side == 0) ? &work->enemy : &work->player;
    s32 friendAttack[CARD_BOARD_SLOTS];
    s32 friendStatus[CARD_BOARD_SLOTS];
    s32 baseScore;
    s32 bestScore;
    s32 bestSlot = -1;
    s32 slotIndex;

    GetFriendAttack(side, work, friendAttack, friendStatus);
    baseScore = TodomeHyouka(side, work, 0, 0, friendAttack, friendStatus);
    bestScore = baseScore;
    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        if (otherSide->battle[slotIndex].battle.layers[0].cardId >= 0) {
            s32 remaining;
            s32 reward;
            s32 score;

            GetFriendAttack(side, work, friendAttack, friendStatus);
            remaining = friendAttack[slotIndex] - 1;
            if (remaining > 0) {
                reward = GetShoukin(&otherSide->battle[slotIndex].battle);
                friendAttack[slotIndex] -= 1;
                score = reward / 10.0f;
            } else {
                reward = GetShoukin(&otherSide->battle[slotIndex].battle);
                friendAttack[slotIndex] = 0;
                score = reward - remaining;
            }
            score += TodomeHyouka(side, work, 0, 0, friendAttack, friendStatus);
            if (bestScore < score) {
                bestScore = score;
                bestSlot = slotIndex | CARD_SLOT_BATTLE;
            }
        }
    }
    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        if (otherSide->disposal[slotIndex].battle.layers[0].cardId >= 0) {
            s32 remaining;
            s32 reward;
            s32 score;

            GetFriendAttack(side, work, friendAttack, friendStatus);
            remaining = friendAttack[slotIndex] - 1;
            if (remaining > 0) {
                reward = GetPrice(&otherSide->disposal[slotIndex].battle);
                friendAttack[slotIndex] -= 1;
                score = reward / 10.0f;
            } else {
                reward = GetPrice(&otherSide->disposal[slotIndex].battle);
                friendAttack[slotIndex] = 0;
                score = reward - remaining;
            }
            score += TodomeHyouka(side, work, 0, 0, friendAttack, friendStatus);
            if (bestScore < score) {
                bestScore = score;
                bestSlot = slotIndex;
            }
        }
    }
    if (bestScore < baseScore + TODOME_MARGIN_SMALL) {
        return -1;
    }
    return bestSlot;
}

/* Highest-price flagged card on the other side's disposal board (flag byte
 * equal to 1), encoded as a disposal slot index (-1 if none qualifies). */
s32 CheckGunoYokanCommand(s32 side, CardGameWork *work)
{
    CardPlaySide *otherSide = (side == 0) ? &work->enemy : &work->player;
    s32 bestPrice = 0;
    s32 bestSlot = -1;
    s32 slotIndex;

    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        if (otherSide->disposal[slotIndex].battle.layers[0].cardId >= 0 &&
            otherSide->disposal[slotIndex].battle.flags == CARD_STACK_FLAG_END) {
            if (bestPrice < GetPrice(&otherSide->disposal[slotIndex].battle)) {
                bestSlot = slotIndex;
                bestPrice = GetPrice(&otherSide->disposal[slotIndex].battle);
            }
        }
    }
    return bestSlot;
}

/* Highest-reward flagged card on the caller's own side: the battle board
 * first (GetShoukin, battle slot index tagged 0xF0000), then the disposal
 * board (GetPrice, plain disposal slot index); -1 if none qualifies. */
s32 CheckExtCommand(s32 side, CardGameWork *work)
{
    CardPlaySide *ownSide = (side == 0) ? &work->player : &work->enemy;
    s32 bestReward = 0;
    s32 bestSlot = -1;
    s32 slotIndex;

    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        if (ownSide->battle[slotIndex].battle.layers[0].cardId >= 0 &&
            (ownSide->battle[slotIndex].battle.flags & CARD_STACK_FLAG_FROZEN)) {
            s32 reward = GetShoukin(&ownSide->battle[slotIndex].battle);
            if (bestReward < reward) {
                bestReward = reward;
                bestSlot = slotIndex | CARD_SLOT_BATTLE;
            }
        }
    }
    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        if (ownSide->disposal[slotIndex].battle.layers[0].cardId >= 0 &&
            (ownSide->disposal[slotIndex].battle.flags & CARD_STACK_FLAG_FROZEN)) {
            s32 price = GetPrice(&ownSide->disposal[slotIndex].battle);
            if (bestReward < price) {
                bestReward = price;
                bestSlot = slotIndex;
            }
        }
    }
    return bestSlot;
}

/*
 * Highest-reward flagged category-four entity on the other side's battle
 * board (GetShoukin) then disposal board (GetPrice), encoded as a slot index
 * (battle slots carry the 0xF0000 tag; -1 if none qualifies).
 */
s32 CheckDownGunoCommand(s32 side, CardGameWork *work)
{
    CardPlaySide *otherSide = (side == 0) ? &work->enemy : &work->player;
    s32 bestReward = 0;
    s32 bestSlot = -1;
    s32 slotIndex;

    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        s16 cardId = otherSide->battle[slotIndex].battle.layers[0].cardId;

        if (cardId >= 0 && work->definitions[cardId].flags == CARD_ATTR_GNOSIS &&
            (otherSide->battle[slotIndex].battle.flags & CARD_STACK_FLAG_FROZEN)) {
            s32 reward = GetShoukin(&otherSide->battle[slotIndex].battle);
            if (bestReward < reward) {
                bestReward = reward;
                bestSlot = slotIndex | CARD_SLOT_BATTLE;
            }
        }
    }
    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        s16 cardId = otherSide->disposal[slotIndex].battle.layers[0].cardId;

        if (cardId >= 0 && work->definitions[cardId].flags == CARD_ATTR_GNOSIS &&
            (otherSide->disposal[slotIndex].battle.flags & CARD_STACK_FLAG_FROZEN)) {
            s32 reward = GetPrice(&otherSide->disposal[slotIndex].battle);
            if (bestReward < reward) {
                bestReward = reward;
                bestSlot = slotIndex;
            }
        }
    }
    return bestSlot;
}

/* Highest-price category-one card among the flagged slots of the other side's
 * disposal board, encoded as a disposal slot index (-1 if none qualifies). */
s32 CheckChudanCommand(s32 side, CardGameWork *work)
{
    CardPlaySide *otherSide = (side == 0) ? &work->enemy : &work->player;
    s32 bestPrice = 0;
    s32 bestSlot = -1;
    s32 slotIndex;

    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        s16 cardId = otherSide->disposal[slotIndex].battle.layers[0].cardId;

        if (cardId >= 0 && work->definitions[cardId].flags == CARD_ATTR_HUMAN &&
            (otherSide->disposal[slotIndex].battle.flags & CARD_STACK_FLAG_END)) {
            s32 price = GetPrice(&otherSide->disposal[slotIndex].battle);
            if (bestPrice < price) {
                bestPrice = price;
                bestSlot = slotIndex;
            }
        }
    }
    return bestSlot;
}

/* Command 413 is allowed when the caller's battle board holds two type-two
 * cards, or one and the opposing draw pile has fewer than thirteen cards. */
s32 Check413Command(s32 side, CardGameWork *work)
{
    CardPlaySide *ownSide;
    CardPlaySide *otherSide;
    s32 remaining;
    s32 typeTwoCount = 0;
    s16 *cardId;
    s32 id;

    if (side == 0) {
        ownSide = &work->player;
        otherSide = &work->enemy;
    } else {
        ownSide = &work->enemy;
        otherSide = &work->player;
    }
    if (play413 == 1) {
        return 0;
    }
    cardId = &ownSide->battle[0].battle.layers[0].cardId;
    for (remaining = 0; remaining < CARD_BOARD_SLOTS; remaining++) {
        id = *cardId;
        cardId += CARD_SLOT_HALFWORDS;
        if (id >= 0) {
            if (2 == work->definitions[id].flags) {
                ++typeTwoCount;
            }
        }
    }
    if (1 < typeTwoCount) {
        return 1;
    }
    if (typeTwoCount == 1) {
        if (!(CardPlayYamaCnt(otherSide) >= 13)) {
            return 1;
        }
    }
    return 0;
}

/* Highest-price flagged card on the other side's disposal board, encoded as
 * a disposal slot index (-1 if none qualifies). */
s32 CheckKyoukouCommand(s32 side, CardGameWork *work)
{
    CardPlaySide *otherSide = (side == 0) ? &work->enemy : &work->player;
    s32 bestPrice = 0;
    s32 bestSlot = -1;
    s32 slotIndex;

    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        if (otherSide->disposal[slotIndex].battle.layers[0].cardId >= 0 &&
            (otherSide->disposal[slotIndex].battle.flags & CARD_STACK_FLAG_FROZEN)) {
            s32 price = GetPrice(&otherSide->disposal[slotIndex].battle);
            if (bestPrice < price) {
                bestPrice = price;
                bestSlot = slotIndex;
            }
        }
    }
    return bestSlot;
}

/* Lowest-reward active category-one card on the caller's own battle board,
 * encoded as a battle slot index (0xF0000 tags it as a battle, not disposal,
 * slot for the caller). */
s32 CheckShukumeiCommand(s32 side, CardGameWork *work)
{
    CardPlaySide *ownSide;
    CardPlaySide *opponent;
    s32 bestReward = 0x32;
    s32 bestSlot = -1;
    s32 slotIndex;

    if (side == 0) {
        ownSide = &work->player;
        opponent = &work->enemy;
    } else {
        ownSide = &work->enemy;
        opponent = &work->player;
    }
    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        s16 *cardIdSlot = &ownSide->battle[slotIndex].battle.layers[0].cardId;
        s16 cardId = *cardIdSlot;

        if (cardId >= 0 && work->definitions[cardId].flags == CARD_ATTR_HUMAN) {
            s32 reward = GetShoukin(&opponent->battle[slotIndex].battle);
            if (reward < bestReward) {
                bestReward = reward;
                bestSlot = slotIndex | CARD_SLOT_BATTLE;
            }
        }
    }
    return bestSlot;
}

/*
 * Target for the Bujin command, on the caller's own battle board: a slot
 * whose doubled attack (the low halfword of its friendStatus word) lifts the
 * TodomeHyouka score above the baseline by at least 30. The first slot with
 * a stacked Eyeglasses card that clears the threshold is returned at once; otherwise
 * the best of the remaining battle slots is returned if it clears it. Battle
 * slots are encoded with the 0xF0000 tag, -1 means none.
 */
s32 CheckBujinCommand(s32 side, CardGameWork *work)
{
    CardPlaySide *ownSide = &work->enemy;
    s32 friendAttack[CARD_BOARD_SLOTS];
    s32 friendStatus[CARD_BOARD_SLOTS];
    s32 baseScore;
    s32 bestScore;
    s32 bestSlot;
    s32 score;
    s32 threshold;
    s32 slotIndex;
    CardBoardSlot *slot;

    bestSlot = -1;
    if (side == 0) {
        ownSide = &work->player;
        score = 0;
    } else {
        score = 1;
    }
    GetFriendAttack(side, work, friendAttack, friendStatus);
    baseScore = TodomeHyouka(side, work, 0, 0, friendAttack, friendStatus);
    bestScore = baseScore;
    threshold = baseScore + TODOME_MARGIN;
    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        slot = &ownSide->battle[slotIndex];
        if (slot->battle.layers[0].cardId < 0) {
            continue;
        }
        if (slot->battle.flags & CARD_STACK_FLAG_FROZEN) {
            continue;
        }
        if (slot->battle.layers[1].cardId != CARD_ID_EYEGLASSES) {
            continue;
        }
        GetFriendAttack(side, work, friendAttack, friendStatus);
        friendStatus[slotIndex] = (friendStatus[slotIndex] & ~ATTACK_POWER_MASK) +
                                  ((friendStatus[slotIndex] & ATTACK_POWER_MASK) * 2);
        score = TodomeHyouka(side, work, 0, 0, friendAttack, friendStatus);
        if (threshold < score) {
            return slotIndex | CARD_SLOT_BATTLE;
        }
    }
    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        s16 cardId = ownSide->battle[slotIndex].battle.layers[0].cardId;

        if (cardId >= 0 && (ownSide->battle[slotIndex].battle.flags & CARD_STACK_FLAG_FROZEN) == 0) {
            s32 attackType = CardChkAttackType(work->definitions, &ownSide->battle[slotIndex].battle);

            if ((u32)(cardId - CARD_ID_PROTO_DORA) >= 2 && cardId != CARD_ID_VX_7000 && cardId != CARD_ID_CANNON &&
                attackType != CARD_ATTACK_HOMING) {
                GetFriendAttack(side, work, friendAttack, friendStatus);
                friendStatus[slotIndex] = (friendStatus[slotIndex] & ~ATTACK_POWER_MASK) +
                                          ((friendStatus[slotIndex] & ATTACK_POWER_MASK) * 2);
                score = TodomeHyouka(side, work, 0, 0, friendAttack, friendStatus);
                if (bestScore < score) {
                    bestScore = score;
                    bestSlot = slotIndex | CARD_SLOT_BATTLE;
                }
            }
        }
    }
    return (bestScore < baseScore + TODOME_MARGIN) ? -1 : bestSlot;
}

/* Greatest CardDat value of the card stacked on an occupied battle or
 * disposal slot of the other side: a battle slot index is tagged 0xF0000,
 * a disposal slot index is plain (-1 if none qualifies). */
s32 CheckTuukokuCommand(s32 side, CardGameWork *work)
{
    CardPlaySide *otherSide = (side == 0) ? &work->enemy : &work->player;
    s32 bestSlot = -1;
    s32 bestValue = 0;
    s32 slotIndex;

    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        if (otherSide->battle[slotIndex].battle.layers[0].cardId >= 0 &&
            otherSide->battle[slotIndex].battle.layers[1].cardId >= 0) {
            s32 value = CardDat[otherSide->battle[slotIndex].battle.layers[1].cardId];
            if (bestValue < value) {
                bestValue = value;
                bestSlot = slotIndex | CARD_SLOT_BATTLE;
            }
        }
    }
    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        if (otherSide->disposal[slotIndex].battle.layers[0].cardId >= 0 &&
            otherSide->disposal[slotIndex].battle.layers[1].cardId >= 0) {
            s32 value = CardDat[otherSide->disposal[slotIndex].battle.layers[1].cardId];
            if (bestValue < value) {
                bestValue = value;
                bestSlot = slotIndex;
            }
        }
    }
    return bestSlot;
}

/*
 * Adds up the CardDat value of the card stacked on each occupied slot (layers
 * one, with layers zero present) of both sides' battle and disposal boards,
 * then reports whether the acting side's total is below the other side's.
 */
s32 CheckKetugiCommand(s32 side, CardGameWork *work)
{
    CardPlaySide *ownSide;
    CardPlaySide *otherSide;
    s32 otherTotal = 0;
    s32 ownTotal = 0;
    s32 i;

    if (side == 0) {
        ownSide = &work->player;
        otherSide = &work->enemy;
    } else {
        ownSide = &work->enemy;
        otherSide = &work->player;
    }

    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        if (ownSide->battle[i].battle.layers[0].cardId >= 0) {
            if (ownSide->battle[i].battle.layers[1].cardId >= 0) {
                ownTotal += CardDat[ownSide->battle[i].battle.layers[1].cardId];
            }
        }
        if (otherSide->battle[i].battle.layers[0].cardId >= 0) {
            if (otherSide->battle[i].battle.layers[1].cardId >= 0) {
                otherTotal += CardDat[otherSide->battle[i].battle.layers[1].cardId];
            }
        }
    }
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        if (ownSide->disposal[i].battle.layers[0].cardId >= 0) {
            if (ownSide->disposal[i].battle.layers[1].cardId >= 0) {
                ownTotal += CardDat[ownSide->disposal[i].battle.layers[1].cardId];
            }
        }
        if (otherSide->disposal[i].battle.layers[0].cardId >= 0) {
            if (otherSide->disposal[i].battle.layers[1].cardId >= 0) {
                otherTotal += CardDat[otherSide->disposal[i].battle.layers[1].cardId];
            }
        }
    }
    return ownTotal < otherTotal;
}

s32 CheckTeppekiCommand(s32 side, CardGameWork *work)
{
    CardPlaySide *otherSide;
    CardPlaySide *ownSide;
    s32 friendAttack[CARD_BOARD_SLOTS];
    s32 friendStatus[CARD_BOARD_SLOTS];
    s32 attack;
    s32 slotIndex;
    CardBoardSlot *slot;

    if (side == 0) {
        ownSide = &work->player;
        otherSide = &work->enemy;
    } else {
        ownSide = &work->enemy;
        otherSide = &work->player;
    }
    if (playteppeki == 1) {
        return 0;
    }
    attack = CheckEnemyAttack(side, work);
    if (VX10000Check(side, work) == 0) {
        CardLayerStack *card = &otherSide->battle[0].battle;

        slot = otherSide->battle;
        for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++, slot++) {
            if (slot->battle.layers[0].cardId >= 0) {
                s32 attackType = CardChkAttackType(work->definitions, card);

                if (attackType == CARD_ATTACK_HOMING && CardLayerPower(&slot->battle) < 2) {
                    return 1;
                }
            }
            card = (CardLayerStack *)((char *)card + sizeof(CardBoardSlot));
        }
    }
    GetFriendAttack((side == 0), work, friendAttack, friendStatus);
    if (TodomeHyouka((side == 0), work, 0, 0, friendAttack, friendStatus) >= TODOME_DECK_OUT_SCORE) {
        return 1;
    }
    return CardPlayYamaCnt(ownSide) < attack;
}

/*
 * Lowest-price card on the chosen side's disposal board, then lowest-reward
 * card on its battle board, for the Sensi command. The command is refused
 * when the discard pile holds fewer than eight cards, the draw pile has
 * eleven or more, or the command phase is 2 or 9. The result is a disposal
 * slot index, or a battle slot index tagged with 0xF0000, or -1.
 */
s32 CheckSensiCommand(s32 side, CardGameWork *work)
{
    CardPlaySide *sideData = &work->player;
    s32 slotIndex = 0;
    s32 bestPrice;
    s32 bestSlot;
    s32 price;
    s32 reward;
    float shuntX;

    if (side == 0) {
        sideData = &work->player;
    } else {
        sideData = &work->enemy;
    }

    bestSlot = -1;
    bestPrice = 0x50;
    work++;
    work--;
    if (CardPlaySuteCnt(sideData) < 8) {
        return -1;
    }
    if (CardPlayYamaCnt(sideData) >= 11) {
        return -1;
    }
    if (work->command.phase == CARD_PHASE_EVENT || work->command.phase == CARD_PHASE_EVENT + CARD_PHASES_PER_SIDE) {
        return -1;
    }
    for (; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        if (sideData->disposal[slotIndex].battle.layers[0].cardId >= 0) {
            price = GetPrice(&sideData->disposal[slotIndex].battle);

            if (price < bestPrice) {
                bestPrice = price;
                bestSlot = slotIndex;
            }
        }
    }

    slotIndex = 0;
    shuntX = sideData->battle[0].shuntPosition[0];
    if (sideData != 0) {
        sideData->battle[0].shuntPosition[0] = shuntX;
    }
    for (; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        if (sideData->battle[slotIndex].battle.layers[0].cardId >= 0) {
            reward = GetShoukin(&sideData->battle[slotIndex].battle);

            if (reward < bestPrice) {
                bestPrice = reward;
                bestSlot = slotIndex | CARD_SLOT_BATTLE;
            }
        }
    }
    return bestSlot;
}

s32 SensiEvent(s32 side, CardGameWork *work)
{
    s32 i;
    s32 flagBattle[CARD_BOARD_SLOTS];
    s32 flagDisposal[CARD_BOARD_SLOTS];
    CardCursorPosition targets[8];
    s32 round;
    CardPlaySide *own;
    s32 yama;
    s32 typeBattle;
    s32 typeDisposal;
    s32 count;
    s32 sute;
    s32 bestBattle;
    s32 bestDisposal;

    if (side == 0) {
        own = &work->player;
        typeBattle = CARD_ZONE_SIDE0_BATTLE;
        typeDisposal = CARD_ZONE_SIDE0_DISPOSAL;
    } else {
        own = &work->enemy;
        typeBattle = CARD_ZONE_SIDE1_BATTLE;
        typeDisposal = CARD_ZONE_SIDE1_DISPOSAL;
    }
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        flagBattle[i] = -1;
        flagDisposal[i] = -1;
    }
    yama = CardPlayYamaCnt(own);
    sute = CardPlaySuteCnt(own);
    i = 0;
    do {
        if (own->battle[i].battle.layers[0].cardId >= 0) {
            if (own->battle[i].battle.layers[0].life < 5) {
                flagBattle[i] = 1;
            } else {
                flagDisposal[i] = -1;
            }
        } else {
            flagBattle[i] = -1;
        }
        if (own->disposal[i].battle.layers[0].cardId >= 0) {
            if (own->battle[i].battle.layers[0].life < 5) {
                bestDisposal = i;
                flagDisposal[i] = 1;
            } else {
                flagDisposal[i] = -1;
            }
        } else {
            flagDisposal[i] = -1;
        }
        i++;
    } while (i < 4);
    count = 0;
    round = 0;
    do {
        bestBattle = -1;
        bestDisposal = -1;
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (flagBattle[i] != -1) {
                if (bestBattle == -1 || GetShoukin(&own->battle[i].battle) < GetShoukin(&own->battle[bestBattle].battle)) {
                    bestBattle = i;
                }
            }
            if (flagDisposal[i] != -1) {
                if (bestDisposal == -1 || GetPrice(&own->disposal[i].battle) < GetPrice(&own->disposal[bestDisposal].battle)) {
                    bestDisposal = i;
                }
            }
        }
        if (yama >= WAR_CASUALTY_DECK_LIMIT || sute < CARD_WAR_CASUALTY_RECOVERY) {
            break;
        }
        if (bestDisposal != -1) {
            sute -= CARD_WAR_CASUALTY_RECOVERY;
            targets[count].zone = typeDisposal;
            targets[count].index = bestDisposal;
            yama += CARD_WAR_CASUALTY_RECOVERY;
            flagDisposal[bestDisposal] = -1;
            count++;
        }
        if (yama >= WAR_CASUALTY_DECK_LIMIT || sute < CARD_WAR_CASUALTY_RECOVERY) {
            break;
        }
        if (bestBattle != -1) {
            sute -= CARD_WAR_CASUALTY_RECOVERY;
            targets[count].zone = typeBattle;
            targets[count].index = bestBattle;
            yama += CARD_WAR_CASUALTY_RECOVERY;
            flagBattle[bestBattle] = -1;
            count++;
        }
        if (yama >= WAR_CASUALTY_DECK_LIMIT || sute < CARD_WAR_CASUALTY_RECOVERY) {
            break;
        }
        round++;
    } while (round < 4);
    if (count > 0) {
        i = count;
        do {
        } while (--i != 0);
    }
    return CGPEnemyExecCommand(side, work, targets, count);
}

int CC_Kara_DisposeEvent(s32 side, CardGameWork *work)
{
    CardPlaySide *own;
    s32 attackVec[CARD_BOARD_SLOTS];
    s32 statusVec[CARD_BOARD_SLOTS];
    s32 idxJuli;
    s32 idxPellegri;
    s32 idxZoharEmulator;
    s32 i;
    s32 cardId;
    s32 score;
    s32 rescore;

    own = (side == 0) ? &work->player : &work->enemy;
    idxJuli = -1;
    idxPellegri = -1;
    idxZoharEmulator = -1;
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        if (own->battle[i].battle.layers[0].cardId == CARD_ID_JULI_MIZRAHI) {
            idxJuli = i;
        } else if (own->battle[i].battle.layers[0].cardId == CARD_ID_PELLEGRI) {
            idxPellegri = i;
        } else if (own->battle[i].battle.layers[0].cardId == CARD_ID_ZOHAR_EMULATOR) {
            idxZoharEmulator = i;
        }
    }
    GetFriendAttack(side, work, attackVec, statusVec);
    score = TodomeHyouka(side, work, 0, 0, attackVec, statusVec);
    if (idxJuli != -1) {
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (own->battle[i].battle.layers[0].cardId >= 0 && (work->definitions[own->battle[i].battle.layers[0].cardId].flags & CARD_ATTR_HUMAN) != 0) {
                statusVec[i] += 1;
            }
            statusVec[idxJuli] = 0;
        }
        rescore = TodomeHyouka(side, work, 0, 0, attackVec, statusVec);
        if (score + TODOME_DISPOSE_MARGIN < rescore) {
            own->battle[idxJuli].battle.layers[0].life = 0;
            CCU063ExecSub(work, own);
        }
    }
    if (idxPellegri != -1) {
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (own->battle[i].battle.layers[0].cardId >= 0 && (work->definitions[own->battle[i].battle.layers[0].cardId].flags & CARD_ATTR_MECH) != 0) {
                statusVec[i] += 1;
            }
            statusVec[idxPellegri] = 0;
        }
        rescore = TodomeHyouka(side, work, 0, 0, attackVec, statusVec);
        if (score + TODOME_DISPOSE_MARGIN < rescore) {
            own->battle[idxPellegri].battle.layers[0].life = 0;
            CCU064ExecSub(work, own);
        }
    }
    if (idxZoharEmulator != -1) {
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (own->battle[i].battle.layers[0].cardId >= 0 && (work->definitions[own->battle[i].battle.layers[0].cardId].flags & CARD_ATTR_GNOSIS) != 0) {
                statusVec[i] += 2;
            }
            statusVec[idxZoharEmulator] = 0;
        }
        rescore = TodomeHyouka(side, work, 0, 0, attackVec, statusVec);
        if (score + TODOME_DISPOSE_MARGIN < rescore) {
            own->battle[idxZoharEmulator].battle.layers[0].life = 0;
            CCU065ExecSub(work, own);
        }
    }
    return 0;
}

/*
 * Plays the next command card the acting side can use: the hand is scanned
 * in order, each playable command card asks its evaluator, and the first
 * one that accepts stores its target in commplay and returns the hand index.
 * -1 when no card qualifies; a `commplay` of zero is the no-target answer.
 */
s32 CC_Kara_CommRequest(s32 side, CardGameWork *work)
{
    CardPlaySide *ownSide;
    s16 *cards;
    s32 handCount;
    s32 index;
    s32 sum;
    s32 result;

    ownSide = &work->enemy;
    if (side == 0) {
        ownSide = &work->player;
        if (work != 0) {
            CardDefinition *definitions = work->definitions;

            work->definitions = definitions;
        }
    }
    if (work->command.phase == CARD_PHASE_EVENT || work->command.phase == CARD_PHASE_EVENT + CARD_PHASES_PER_SIDE) {
        CC_Kara_DisposeEvent(side, work);
    }
    index = 0;
    handCount = CardPlayHandCnt(&ownSide->hand);
    for (; index < handCount; index++) {
        s16 *cardPtr;

        cards = ownSide->hand.cards;
        cardPtr = &cards[index];

        if (work->definitions[*cardPtr].kind != CARD_KIND_EVENT) {
            continue;
        }
        if (CheckCommandPlay(side, work, *cardPtr) != 1) {
            continue;
        }
        if (work->definitions[*cardPtr].reserveCost != 0 &&
            CardPlayYamaCnt(ownSide) - work->definitions[*cardPtr].reserveCost < 3) {
            continue;
        }
        switch (*cardPtr) {
        case CARD_ID_CURE_ALL:
            result = CheckExtCommand(side, work);
            if (result != -1) {
                commplay = result;
                return index;
            }
            break;
        case CARD_ID_CURRY:
            result = CheckCurryCommand(side, work);
            if (result != -1) {
                commplay = result;
                return index;
            }
            break;
        case CARD_ID_BIO_SPHERE:
            if (CheckTentCommand(side, work) == 1) {
                commplay = 0;
                return index;
            }
            break;
        case CARD_ID_SUPPLY_SHIP:
            if (work->command.phase != CARD_PHASE_EVENT && work->command.phase != CARD_PHASE_EVENT + CARD_PHASES_PER_SIDE &&
                CardPlaySuteCnt(ownSide) >= 5) {
                commplay = 0;
                return index;
            }
            break;
        case CARD_ID_SUPPLY_FLEET:
            if (work->command.phase == CARD_PHASE_EVENT || work->command.phase == CARD_PHASE_EVENT + CARD_PHASES_PER_SIDE) {
                break;
            }
            sum = DisposeAreaCheckFriend(side, work);
            sum += BattleAreaCheckFriend(side, work);
            if (sum < 4) {
                if (CardPlayYamaCnt(ownSide) < 10) {
                    return index;
                }
            } else if (CardPlaySuteCnt(ownSide) >= sum) {
                return index;
            }
            break;
        case CARD_ID_RETREAT:
        case CARD_ID_WOGLINDE:
            result = CheckTaikyakuCommand(side, work);
            if (result != -1) {
                commplay = result | CARD_SLOT_OTHER_SIDE | CARD_SLOT_BATTLE;
                return index;
            }
            break;
        case CARD_ID_SURPRISE_ATTACK:
        case CARD_ID_FEDERATION_CRUISER:
            result = CheckDamageCommand(side, work, 2);
            if (result != -1) {
                commplay = result | CARD_SLOT_OTHER_SIDE | CARD_SLOT_BATTLE;
                return index;
            }
            break;
        case CARD_ID_INVITATION:
            if (CheckNazoCommand(side, work) == 1) {
                commplay = 0;
                return index;
            }
            break;
        case CARD_ID_MARY_AND_SHELLEY:
            if (work->command.phase == CARD_PHASE_EVENT || work->command.phase == CARD_PHASE_EVENT + CARD_PHASES_PER_SIDE) {
                if (CardPlayYamaCnt(ownSide) >= 11 && handCount < 7) {
                    commplay = 0;
                    return index;
                }
            } else if (CardPlayYamaCnt(ownSide) >= 11) {
                commplay = 0;
                return index;
            }
            break;
        case CARD_ID_UMN_INVASION:
            if (work->command.phase == CARD_PHASE_EVENT || work->command.phase == CARD_PHASE_EVENT + CARD_PHASES_PER_SIDE) {
                if (CardPlaySuteCnt(ownSide) >= 2 && handCount < 7) {
                    commplay = 0;
                    return index;
                }
            } else if (CardPlaySuteCnt(ownSide) >= 2) {
                commplay = 0;
                return index;
            }
            break;
        case CARD_ID_INVESTIGATION:
        case CARD_ID_LANDING_OPERATIONS:
        case CARD_ID_RECYCLER:
        case CARD_ID_SIMULATION:
            return index;
        case CARD_ID_PREMONITION:
            result = CheckGunoYokanCommand(side, work);
            goto commit;
        case CARD_ID_DRAGON_SKULL:
            result = CheckDownGunoCommand(side, work);
            goto commit;
        case CARD_ID_EXPULSION_ORDER:
            result = CheckChudanCommand(side, work);
            goto commit;
        case CARD_ID_IMPREGNABLE_GUARD:
            if (CheckTeppekiCommand(side, work) == 1) {
                commplay = 0;
                return index;
            }
            break;
        case CARD_ID_ARTICLE_4_13:
            if (Check413Command(side, work) == 1) {
                commplay = 0;
                return index;
            }
            break;
        case CARD_ID_PHASE_CANNON:
            if (CheckSouteniHouCommand(side, work) == 1) {
                commplay = 0;
                return index;
            }
            break;
        case CARD_ID_NOTICE:
            result = CheckTuukokuCommand(side, work);
            goto commit;
        case CARD_ID_RESOLUTION:
            if (CheckKetugiCommand(side, work) != 0) {
                commplay = 0;
                return index;
            }
            break;
        case CARD_ID_REVIVE_DX:
            if (handCount < 7) {
                return index;
            }
            break;
        case CARD_ID_STIM_DX:
            result = CheckBujinCommand(side, work);
            if (result != -1) {
                commplay = result | CARD_SLOT_BATTLE;
                return index;
            }
            break;
        case CARD_ID_WAR_CASUALTY:
            result = CheckSensiCommand(side, work);
            if (result != -1) {
                commplay = result;
                return index;
            }
            break;
        case CARD_ID_X_BUSTER:
            if (CheckXDamageCommand(side, work, 1) == 1) {
                commplay = 0;
                return index;
            }
            break;
        case CARD_ID_CHARGE:
            result = CheckTotugekiCommand(side, work);
            if (result != -1) {
                commplay = result | CARD_SLOT_OTHER_SIDE | CARD_SLOT_BATTLE;
                return index;
            }
            break;
        case CARD_ID_PRECISE_SHOOTING:
            result = CheckSeimitsuCommand(side, work);
        commit:
            if (result != -1) {
                commplay = result | CARD_SLOT_OTHER_SIDE;
                return index;
            }
            break;
        case CARD_ID_INVOKE:
            if (CheckHatudouCommand(side, work) == 1) {
                commplay = 0;
                return index;
            }
            break;
        case CARD_ID_CONFUSION:
            if (CheckKonranCommand(side, work) == 1) {
                commplay = 0;
                return index;
            }
            break;
        case CARD_ID_MILTIAN_CONFLICT:
            if (CheckMilCommand(side, work) == 1) {
                commplay = 0;
                return index;
            }
            break;
        case CARD_ID_START_UP:
            result = CheckExtCommand(side, work);
            if (result != -1) {
                commplay = result;
                return index;
            }
            break;
        case CARD_ID_DESTINY:
            result = CheckShukumeiCommand(side, work);
            if (result != -1) {
                commplay = result;
                return index;
            }
            break;
        case CARD_ID_BREAK_THROUGH:
            result = CheckKyoukouCommand(side, work);
            goto commit;
        default:
            break;
        }
    }
    return -1;
}

/*
 * Decodes the stored AI action target (commplay: its low halfword is the
 * index, the masks pick the zone) and either dispatches the command through
 * CGPEnemyExecCommand or plays the Sensi event for the played card.
 */
s32 CC_Kara_CommPlay(s32 side, CardGameWork *work)
{
    u16 index;
    s16 ownBattleZone;
    s16 otherBattleZone;
    s16 ownDisposalZone;
    s16 otherDisposalZone;
    CardPlaySide *ownSide;
    CardCursorPosition cursor;
    u32 *commPtr;

    if (side == 0) {
        ownSide = &work->player;
        ownBattleZone = CARD_ZONE_SIDE0_BATTLE;
        otherBattleZone = CARD_ZONE_SIDE1_BATTLE;
        ownDisposalZone = CARD_ZONE_SIDE0_DISPOSAL;
        otherDisposalZone = CARD_ZONE_SIDE1_DISPOSAL;
    } else {
        ownSide = &work->enemy;
        ownBattleZone = CARD_ZONE_SIDE1_BATTLE;
        otherBattleZone = CARD_ZONE_SIDE0_BATTLE;
        ownDisposalZone = CARD_ZONE_SIDE1_DISPOSAL;
        otherDisposalZone = CARD_ZONE_SIDE0_DISPOSAL;
    }
    commPtr = &commplay;
    if (!(*commPtr & CARD_SLOT_OTHER_SIDE)) {
        if (!(*commPtr & CARD_SLOT_BATTLE)) {
            cursor.zone = ownDisposalZone;
            cursor.index = *(u16 *) commPtr;
        } else {
            cursor.zone = ownBattleZone;
            cursor.index = *(u16 *) commPtr;
        }
    } else {
        if (!(*commPtr & CARD_SLOT_BATTLE)) {
            cursor.zone = otherDisposalZone;
            cursor.index = *(u16 *) commPtr;
        } else {
            cursor.zone = otherBattleZone;
            cursor.index = *(u16 *) commPtr;
        }
    }
    switch (ownSide->battle[0].cmdView.playedCard) {
    case CARD_ID_CURE_ALL:
        break;
    case CARD_ID_IMPREGNABLE_GUARD:
        playteppeki = 1;
        break;
    case CARD_ID_ARTICLE_4_13:
        play413 = 1;
        break;
    case CARD_ID_WAR_CASUALTY:
        return SensiEvent(side, work);
    case CARD_ID_BREAK_THROUGH:
        break;
    default:
        break;
    }
    return CGPEnemyExecCommand(side, work, &cursor, 0);
}

/* Frames between two set attempts, and the number of attempts after which the set phase ends. */
#define CC_KARA_SET_WAIT_FRAMES 15
#define CC_KARA_SET_MAX_ATTEMPTS 41

/* Runs timed AI set attempts: the first call of a phase arms aiStep and the
 * wait counter, later calls count the wait down, then aiStep counts the
 * attempts and CC_Kara_SetFase_1 runs while fewer than 41 were made. */
s32 CC_Kara_SetFase(s32 side, CardGameWork *work)
{
    /* Frames left before the next set attempt. */
    static s32 timer = 0;
    s32 attempts;

    if (work->aiStep == 0) {
        work->aiStep = 1;
        timer = CC_KARA_SET_WAIT_FRAMES;
    }
    if (timer == 0) {
        attempts = work->aiStep + 1;
        work->aiStep = attempts;
        if ((u8)attempts >= CC_KARA_SET_MAX_ATTEMPTS) {
            return 0;
        }
        timer = CC_KARA_SET_WAIT_FRAMES;
        return CC_Kara_SetFase_1(side, work);
    }
    timer = timer - 1;
    return 1;
}

/* Reward of the base card in the stack plus, if present, the card stacked on
 * it (CardDat[cardId], indexed directly by card id). Zero if the base layer
 * is empty. */
s32 GetShoukin(CardLayerStack *stack)
{
    s16 cardId0 = stack->layers[0].cardId;
    s32 reward;
    s16 cardId1;

    if (cardId0 < 0) {
        return 0;
    }
    reward = CardDat[cardId0];
    cardId1 = stack->layers[1].cardId;
    if (cardId1 >= 0) {
        reward += CardDat[cardId1];
    }
    return reward;
}

/* Price of the base card in the stack plus, if present, the card stacked on
 * it (CardDat[cardId], indexed directly by card id). Zero if the base layer
 * is empty. */
s32 GetPrice(CardLayerStack *stack)
{
    s16 cardId0 = stack->layers[0].cardId;
    s32 price;
    s16 cardId1;

    if (cardId0 < 0) {
        return 0;
    }
    price = CardDat[cardId0];
    cardId1 = stack->layers[1].cardId;
    if (cardId1 >= 0) {
        price += CardDat[cardId1];
    }
    return price;
}

s32 TodomeHyouka(s32 side, CardGameWork *work, u32 slotA, u32 slotB, s32 *kinds, s32 *results) {
    s32 life[CARD_BOARD_SLOTS];
    s32 flags[CARD_BOARD_SLOTS];
    s32 bonusFlag[CARD_BOARD_SLOTS];
    s32 bonus[CARD_BOARD_SLOTS];
    s32 gain;
    s32 *flagPtr;
    s32 pinnedA;
    s32 pinnedB;
    s32 total;
    s32 momoBonus;
    s32 counted;
    s32 spreadPower;
    s32 homingPower;
    CardPlaySide *other;
    CardPlaySide *own;
    CardLayerStack *stack;
    s32 score;
    s32 specialOps;
    s32 i;
    s16 cardId;
    s32 temp;
    s32 value;
    s32 remain;
    s8 ownLifeA;
    s8 ownLifeB;

    pinnedA = 0;
    pinnedB = 0;
    total = 0;
    momoBonus = -1;
    counted = 0;
    spreadPower = 0;
    homingPower = 0;
    score = 0;
    if (side == 0) {
        own = &work->player;
        other = &work->enemy;
    } else {
        own = &work->enemy;
        other = &work->player;
    }
    i = (side == 0);
    if (YoakimuMoMoCheck(i, work) == 1) {
        momoBonus = MoMoCheck(i, work) & CARD_SLOT_INDEX_MASK;
    }
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        life[i] = kinds[i];
        flags[i] = results[i];
    }
    flagPtr = &flags[2];
    specialOps = 0;
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        if (other->operations[i] == CARD_ID_TUNE_UP) {
            specialOps += 1;
        }
    }
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        cardId = other->battle[i].battle.layers[0].cardId;
        bonus[i] = 0;
        if (cardId >= 0) {
            if (other->battle[i].battle.layers[1].cardId == CARD_ID_SHIELD || other->battle[i].battle.layers[1].cardId == CARD_ID_BATTLE_SUIT) {
                bonus[i] = 1;
            }
            if (work->definitions[cardId].flags == CARD_ATTR_MECH) {
                bonus[i] += specialOps;
            }
        }
    }
    if (slotA != slotB) {
        remain = flags[slotA];
        flags[slotA] = flags[slotB];
        flags[slotB] = remain;
        if ((own->battle[slotB].battle.layers[0].cardId < 0 || own->battle[slotA].battle.layers[0].cardId < 0) && !(results[slotB] & ATTACK_HAND)) {
            temp = CheckEnemyFight(side, work);
            if (temp != 0) {
                if (slotB < 2U) {
                    if ((slotA - 2U) < 2U) {
                        score = temp * 10;
                    }
                }
                if ((slotB - 2U) < 2U) {
                    if (slotA < 2U) {
                        score = -(temp * 10);
                    }
                }
            } else {
                if (slotB < 2U) {
                    score = ((slotA - 2U) < 2U) * 8;
                }
                if ((slotB - 2U) < 2U) {
                    score = (slotA < 2U) ? -8 : score;
                }
            }
        }
    }
    if (!(flags[0] & ATTACK_HAND) && !(flags[2] & ATTACK_HAND) && flags[0] != 0 && flags[2] != 0) {
        score += (flags[0] & ATTACK_POWER_MASK) < (flags[2] & ATTACK_POWER_MASK);
        score -= (flags[2] & ATTACK_POWER_MASK) < (flags[0] & ATTACK_POWER_MASK);
    }
    if (!(flags[1] & ATTACK_HAND) && !(flags[3] & ATTACK_HAND) && flags[1] != 0 && flags[3] != 0) {
        score += (flags[1] & ATTACK_POWER_MASK) < (flags[3] & ATTACK_POWER_MASK);
        score -= (flags[3] & ATTACK_POWER_MASK) < (flags[1] & ATTACK_POWER_MASK);
    }
    if (flags[0] & ATTACK_HAND) {
        if (!(flags[2] & ATTACK_HAND)) {
            score = (flags[2] & ATTACK_POWER_MASK) ? (score + 0xA) : score;
        }
    }
    if ((flags[1] & ATTACK_HAND) && !(flags[3] & ATTACK_HAND)) {
        score = (flags[3] & ATTACK_POWER_MASK) ? (score + 0xA) : score;
    }
    if ((flags[0] != 0) && (flags[2] & ATTACK_HAND)) {
        score = (flags[2] & ATTACK_POWER_MASK) ? (score - 0xA) : score;
    }
    if ((flags[1] != 0) && (flags[3] & ATTACK_HAND)) {
        score = (flags[3] & ATTACK_POWER_MASK) ? (score - 0xA) : score;
    }
    if (flagPtr[0] & (ATTACK_SPREAD | ATTACK_HOMING)) {
        score += 2;
        if ((slotA != 0) && (slotB != 0)) {
            if (own->battle[0].battle.layers[0].cardId >= 0) {
                ownLifeA = own->battle[0].battle.layers[0].life;
                score += ownLifeA;
            }
        } else if (slotA == 0) {
            if (own->battle[slotB].battle.layers[0].cardId >= 0) {
                ownLifeA = own->battle[slotB].battle.layers[0].life;
                score += ownLifeA;
            }
        } else {
            if (own->battle[slotA].battle.layers[0].cardId >= 0) {
                ownLifeA = own->battle[slotA].battle.layers[0].life;
                score += ownLifeA;
            }
        }
    }
    if (flagPtr[1] & (ATTACK_SPREAD | ATTACK_HOMING)) {
        score += 2;
        if ((slotA != 1) && (slotB != 1)) {
            if (own->battle[1].battle.layers[0].cardId >= 0) {
                ownLifeB = own->battle[1].battle.layers[0].life;
                score += ownLifeB;
            }
        } else if (slotA == 1) {
            if (own->battle[slotB].battle.layers[0].cardId >= 0) {
                ownLifeB = own->battle[slotB].battle.layers[0].life;
                score += ownLifeB;
            }
        } else {
            if (own->battle[slotA].battle.layers[0].cardId >= 0) {
                ownLifeB = own->battle[slotA].battle.layers[0].life;
                score += ownLifeB;
            }
        }
    }
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        value = flags[i];
        if (value & ATTACK_SPLASH) {
            counted++;
        }
        if (value & ATTACK_SPREAD) {
            spreadPower += value & ATTACK_POWER_MASK;
            value = 0;
            flags[i] = 0;
        }
        if (value & ATTACK_HOMING) {
            flags[i] = 0;
            homingPower += value & ATTACK_POWER_MASK;
        }
    }
    if (flags[0] & ATTACK_HAND) {
        pinnedA = flags[0] & ATTACK_POWER_MASK;
        flags[0] &= ~ATTACK_POWER_MASK;
    }
    if (flags[1] & ATTACK_HAND) {
        pinnedB = flags[1] & ATTACK_POWER_MASK;
        flags[1] &= ~ATTACK_POWER_MASK;
    }
    if (flags[2] & ATTACK_HAND) {
        flags[2] = 0;
    }
    if (flags[3] & ATTACK_HAND) {
        flags[3] = 0;
    }
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        if (flags[i] & ATTACK_DOWNS_TARGET) {
            bonusFlag[i] = 1;
        } else {
            bonusFlag[i] = 0;
        }
        flags[i] &= ATTACK_POWER_MASK;
    }
    if (life[0] != 0) {
        remain = life[0] - flags[1];
        if (pinnedB != 0) {
            remain = life[0] - pinnedB;
        }
        if ((bonus[0] != 0) && ((flags[1] != 0) || (pinnedB != 0))) {
            remain += bonus[0];
        }
        if (remain > 0) {
            remain -= flags[3];
            if (bonus[0] != 0) {
                remain = (flags[3] != 0) ? (remain + bonus[0]) : remain;
            }
            if (remain > 0) {
                if ((bonusFlag[1] == 1) || (bonusFlag[3] == 1)) {
                    stack = &other->battle[0].battle;
                    score = score + (GetShoukin(stack) * 0.5f);
                    score = score + ((GetShoukin(stack) / 20.0f) * (life[0] - remain));
                } else {
                    score = score + ((GetShoukin(&other->battle[0].battle) / 10.0f) * (life[0] - remain));
                }
                life[0] = remain;
            } else if (momoBonus != 0) {
                score += GetShoukin(&other->battle[0].battle) + remain;
                life[0] = 0;
            }
        } else {
            life[0] = 0;
            if (momoBonus != 0) {
                score += GetShoukin(&other->battle[0].battle) + remain;
                if (life[2] != 0) {
                    remain = life[2] - flags[3];
                    if (bonus[2] != 0) {
                        remain = (flags[3] != 0) ? (remain + bonus[2]) : remain;
                    }
                    if (remain > 0) {
                        if (bonusFlag[3] == 1) {
                            stack = &other->battle[2].battle;
                            score = score + (GetShoukin(stack) * 0.5f);
                            score = score + ((GetShoukin(stack) / 20.0f) * (life[2] - remain));
                        } else {
                            score = score + ((GetShoukin(&other->battle[2].battle) / 10.0f) * (life[2] - remain));
                        }
                        life[2] = remain;
                    } else if (momoBonus != 2) {
                        score += GetShoukin(&other->battle[2].battle) + remain;
                        life[2] = 0;
                    }
                } else {
                    s32 carried;

                    carried = flags[3];
                    score += carried;
                    total = carried;
                }
            }
        }
    } else if (life[2] != 0) {
        remain = life[2] - flags[1];
        if (bonus[2] != 0) {
            remain = (flags[1] != 0) ? (remain + bonus[2]) : remain;
        }
        if (remain > 0) {
            remain -= flags[3];
            if (bonus[2] != 0) {
                remain = (flags[3] != 0) ? (remain + bonus[2]) : remain;
            }
            if (remain > 0) {
                if ((pinnedB == 0) && ((bonusFlag[1] == 1) || (bonusFlag[3] == 1))) {
                    stack = &other->battle[2].battle;
                    score = score + (GetShoukin(stack) * 0.5f);
                    score = score + ((GetShoukin(stack) / 20.0f) * (life[2] - remain));
                } else {
                    score = score + ((GetShoukin(&other->battle[2].battle) / 10.0f) * (life[2] - remain));
                }
                life[2] = remain;
            } else if (momoBonus != 2) {
                score += GetShoukin(&other->battle[2].battle) + remain;
                life[2] = 0;
            }
        } else if (momoBonus != 2) {
            s32 carried;

            life[2] = 0;
            score += GetShoukin(&other->battle[2].battle) + remain;
            carried = flags[3];
            score += carried;
            total = carried;
        }
    } else {
        gain = flags[1] + flags[3];
        score += gain;
        total = gain;
    }
    if (life[1] != 0) {
        remain = life[1] - flags[0];
        if (pinnedA != 0) {
            remain = life[1] - pinnedA;
        }
        if ((bonus[1] != 0) && ((flags[0] != 0) || (pinnedA != 0))) {
            remain += bonus[1];
        }
        if (remain > 0) {
            remain -= flags[2];
            if (bonus[1] != 0) {
                remain = (flags[2] != 0) ? (remain + bonus[1]) : remain;
            }
            if (remain > 0) {
                if ((bonusFlag[0] == 1) || (bonusFlag[2] == 1)) {
                    stack = &other->battle[1].battle;
                    score = score + (GetShoukin(stack) * 0.5f);
                    score = score + ((GetShoukin(stack) / 20.0f) * (life[1] - remain));
                } else {
                    score = score + ((GetShoukin(&other->battle[1].battle) / 10.0f) * (life[1] - remain));
                }
                life[1] = remain;
            } else if (momoBonus != 1) {
                score += GetShoukin(&other->battle[1].battle) + remain;
                life[1] = 0;
            }
        } else if (momoBonus != 1) {
            score += GetShoukin(&other->battle[1].battle) + remain;
            if (life[3] != 0) {
                remain = life[3] - flags[2];
                if (bonus[3] != 0) {
                    remain = (flags[2] != 0) ? (remain + bonus[3]) : remain;
                }
                if (remain > 0) {
                    if (bonusFlag[2] == 1) {
                        stack = &other->battle[3].battle;
                        score = score + (GetShoukin(stack) * 0.5f);
                        score = score + ((GetShoukin(stack) / 20.0f) * (life[3] - remain));
                    } else {
                        score = score + ((GetShoukin(&other->battle[3].battle) / 10.0f) * (life[3] - remain));
                    }
                    life[3] = remain;
                } else if (momoBonus != 3) {
                    score += GetShoukin(&other->battle[3].battle) + remain;
                    life[3] = 0;
                }
            } else {
                score += flags[2];
                total += flags[2];
            }
        }
    } else if (life[3] != 0) {
        remain = life[3] - flags[0];
        if (bonus[3] != 0) {
            remain = (flags[0] != 0) ? (remain + bonus[3]) : remain;
        }
        if (remain > 0) {
            remain -= flags[2];
            if (bonus[3] != 0) {
                remain = (flags[2] != 0) ? (remain + bonus[3]) : remain;
            }
            if (remain > 0) {
                if ((pinnedA == 0) && ((bonusFlag[0] == 1) || (bonusFlag[2] == 1))) {
                    stack = &other->battle[3].battle;
                    score = score + (GetShoukin(stack) * 0.5f);
                    score = score + ((GetShoukin(stack) / 20.0f) * (life[3] - remain));
                } else {
                    score = score + ((GetShoukin(&other->battle[3].battle) / 10.0f) * (life[3] - remain));
                }
                life[3] = remain;
            } else if (momoBonus != 3) {
                score += GetShoukin(&other->battle[3].battle) + remain;
                life[3] = 0;
            }
        } else if (momoBonus != 3) {
            life[3] = 0;
            score += GetShoukin(&other->battle[3].battle) + remain;
            score += flags[2];
            total += flags[2];
        }
    } else {
        score += flags[0] + flags[2];
        total += flags[0] + flags[2];
    }
    if (counted != 0) {
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (life[i] > 0) {
                life[i] -= counted;
            }
        }
    }
    if (spreadPower != 0) {
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (life[i] > 0) {
                life[i] -= spreadPower;
                score = score + ((GetShoukin(&other->battle[i].battle) / 10.0f) * spreadPower);
            }
        }
    }
    if (homingPower != 0) {
        score += homingPower;
        total += homingPower;
    }
    score = ((CardPlayYamaCnt(other) - total) < 2) ? (score + TODOME_DECK_OUT_BONUS) : score;
    if (AlbedoCheck(side, work) != -1) {
        score += total * 0xA;
    }
    return score;
}

s32 GetFriendAttack(s32 side, CardGameWork *work, s32 *kinds, s32 *results)
{
    CardPlaySide *ownSide;
    CardPlaySide *otherSide;
    s32 friendIndex = -1;
    s32 index;
    s32 type;
    s32 cardId;
    s32 kind;

    if (side == 0) {
        ownSide = &work->player;
        otherSide = &work->enemy;
    } else {
        ownSide = &work->enemy;
        otherSide = &work->player;
    }
    for (index = 0; index < CARD_BOARD_SLOTS; index++) {
        kinds[index] = 0;
        results[index] = 0;
        if (otherSide->battle[index].battle.layers[0].cardId >= 0) {
            kinds[index] = otherSide->battle[index].battle.layers[0].life;
        }
        cardId = ownSide->battle[index].battle.layers[0].cardId;
        if (cardId < 0) {
            continue;
        }
        type = CardChkAttackType(work->definitions, &ownSide->battle[index].battle);
        if (ownSide->battle[index].battle.power < 2) {
            if (type == CARD_ATTACK_BALLISTIC) {
                kind = index | CARD_SLOT_BATTLE;
                results[index] = CheckNowPower(side, work, kind, ownSide->battle[index].battle.layers[0].cardId);
                if (LvUpCheck(side, work, kind) != -1) {
                    results[index] = LvUpCheck(side, work, kind);
                }
                if (cardId == CARD_ID_AG_02_M || cardId == CARD_ID_LV_10_ZIGGY) {
                    results[index] |= ATTACK_DOWNS_TARGET;
                }
                if (cardId == CARD_ID_ALBEDO) {
                    friendIndex = index;
                }
            } else if (type == CARD_ATTACK_HAND) {
                kind = index | CARD_SLOT_BATTLE;
                results[index] = CheckNowPower(side, work, kind, ownSide->battle[index].battle.layers[0].cardId);
                if (LvUpCheck(side, work, kind) != -1) {
                    results[index] = LvUpCheck(side, work, kind);
                }
                results[index] |= ATTACK_HAND;
                if (cardId == CARD_ID_LV_1_ZIGGY || cardId == CARD_ID_LV_10_ZIGGY || cardId == CARD_ID_AG_04 || cardId == CARD_ID_GOBLIN) {
                    results[index] |= ATTACK_DOWNS_TARGET;
                }
            } else if (type == CARD_ATTACK_SPREAD) {
                kind = index | CARD_SLOT_BATTLE;
                results[index] = CheckNowPower(side, work, kind, ownSide->battle[index].battle.layers[0].cardId) | ATTACK_SPREAD;
            } else if (type == CARD_ATTACK_HOMING) {
                kind = index | CARD_SLOT_BATTLE;
                results[index] = CheckNowPower(side, work, kind, ownSide->battle[index].battle.layers[0].cardId) | ATTACK_HOMING;
            }
        }
        if (type == CARD_ATTACK_SPREAD) {
            results[index] |= ATTACK_SPREAD;
        }
        if (type == CARD_ATTACK_HOMING) {
            results[index] |= ATTACK_HOMING;
        }
        if (cardId == CARD_ID_SIMEON || cardId == CARD_ID_GARGOYLE) {
            results[index] |= ATTACK_SPLASH;
        }
        if (ownSide->battle[index].battle.flags & CARD_STACK_FLAG_FROZEN) {
            results[index] &= ~ATTACK_POWER_MASK;
            results[index] &= ~ATTACK_DOWNS_TARGET;
            results[index] &= ~ATTACK_SPLASH;
        }
    }
    if (friendIndex != -1) {
        if (BattleAreaCheckFriend(side, work) == 1) {
            if (results[friendIndex] == 2) {
                results[friendIndex] = CardPlayJunkCnt(ownSide) + 2;
            }
        }
    }
    if (CheckTamasii(side, work) == 1) {
        s16 *cardIdPtr = &ownSide->battle[0].battle.layers[0].cardId;
        s32 *resultSlot = results;

        for (index = 3; index >= 0; index--) {
            s16 slotCardId = *cardIdPtr;

            cardIdPtr += CARD_SLOT_HALFWORDS;
            if (slotCardId >= 0 && (work->definitions[slotCardId].flags & CARD_ATTR_HUMAN)) {
                *resultSlot += 1;
            }
            resultSlot++;
        }
    }
    return 1;
}

s32 CC_Kara_TodomeMove(s32 side, CardGameWork *work, s32 slot)
{
    s32 friendA[CARD_BOARD_SLOTS];
    s32 friendB[CARD_BOARD_SLOTS];
    s32 hyouka[CARD_BOARD_SLOTS];
    s32 best;
    s32 bestSlot;
    s32 i;
    s32 noSlot = -1;
    CardPlaySide *sideData;

    if (side == 0) {
        sideData = &work->player;
    } else {
        sideData = &work->enemy;
    }
    GetFriendAttack(side, work, friendA, friendB);
    slot &= CARD_SLOT_INDEX_MASK;
    bestSlot = noSlot;
    best = TodomeHyouka(side, work, slot, slot, friendA, friendB);
    hyouka[0] = TodomeHyouka(side, work, 0, slot, friendA, friendB);
    hyouka[1] = TodomeHyouka(side, work, 1, slot, friendA, friendB);
    hyouka[2] = TodomeHyouka(side, work, 2, slot, friendA, friendB);
    hyouka[3] = TodomeHyouka(side, work, 3, slot, friendA, friendB);

    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        if (i == slot) {
            continue;
        }
        if (best < hyouka[i]) {
            if (hyouka[i] >= TODOME_DECK_OUT_SCORE) {
                best = hyouka[i];
                bestSlot = i;
            } else {
                s16 *cardIdPtr = &sideData->battle[i].battle.layers[0].cardId;
                s32 cardId = *cardIdPtr;

                if (cardId < 0 || GetDeckDataAdjust(side, work, cardId) < 4) {
                    best = hyouka[i];
                    bestSlot = i;
                }
            }
        }
    }
    if (bestSlot == noSlot) {
        return 1;
    }
    CGPMoveDSBATTLEwork(&sideData->battle[bestSlot].battle,
                        &sideData->battle[slot].battle);
    return 1;
}

/*
 * Routes the attack card in battle slot `index` of `side` through the
 * finisher movement: attack type 2 and type 1 and every other type all
 * tail-call CC_Kara_TodomeMove with flag 1.
 */
s32 CC_Kara_AttackMove(s32 side, CardGameWork *work, s32 slot)
{
    u16 index = slot;
    CardPlaySide *ownSide = (side == 0) ? &work->player : &work->enemy;
    s32 attackType = CardChkAttackType(work->definitions, &ownSide->battle[index].battle);
    if (attackType == CARD_ATTACK_BALLISTIC) {
        return CC_Kara_TodomeMove(side, work, index);
    }
    if (attackType == CARD_ATTACK_HAND) {
        return CC_Kara_TodomeMove(side, work, index);
    }
    return CC_Kara_TodomeMove(side, work, index);
}

s32 CC_Kara_BackTodomeMove(s32 side, CardGameWork *work, s32 slot)
{
    s32 attack[CARD_BOARD_SLOTS];
    s32 status[CARD_BOARD_SLOTS];
    s32 score;
    s32 cardId;
    CardPlaySide *ownSide;

    if (side == 0) {
        ownSide = &work->player;
    } else {
        ownSide = &work->enemy;
    }
    GetFriendAttack(side, work, attack, status);
    switch (slot) {
    default:
        return 1;
    case 0:
    {
        score = TodomeHyouka(side, work, 0, 0, attack, status);
        if (score < TodomeHyouka(side, work, 2, 0, attack, status)) {
            cardId = ownSide->battle[2].battle.layers[0].cardId;
            if (0 > cardId || GetDeckDataAdjust(side, work, cardId) < 4) {
                CGPMoveDSBATTLEwork(&ownSide->battle[2].battle,
                                    &ownSide->battle[0].battle);
                return 1;
            }
            if (TODOME_DECK_OUT_BONUS < TodomeHyouka(side, work, 2, 0, attack, status)) {
                CGPMoveDSBATTLEwork(&ownSide->battle[2].battle,
                                    &ownSide->battle[0].battle);
                return 1;
            }
        }
        if (TodomeHyouka(side, work, 3, 0, attack, status) > score) {
            cardId = ownSide->battle[3].battle.layers[0].cardId;
            if (cardId < 0 || GetDeckDataAdjust(side, work, cardId) < 4) {
                CGPMoveDSBATTLEwork(&ownSide->battle[3].battle,
                                    &ownSide->battle[slot].battle);
                return 1;
            }
            if (TodomeHyouka(side, work, 3, 0, attack, status) >= TODOME_DECK_OUT_SCORE) {
                CGPMoveDSBATTLEwork(&ownSide->battle[3].battle,
                                    &ownSide->battle[slot].battle);
                return 1;
            }
        }
        return 1;
    }
    case 1:
    {
        score = TodomeHyouka(side, work, 1, 1, attack, status);
        if (score < TodomeHyouka(side, work, 2, 1, attack, status)) {
            cardId = ownSide->battle[2].battle.layers[0].cardId;
            if (cardId < 0 || GetDeckDataAdjust(side, work, cardId) < 4) {
                CGPMoveDSBATTLEwork(&ownSide->battle[2].battle,
                                    &ownSide->battle[1].battle);
                return 1;
            }
            if (TodomeHyouka(side, work, 2, 1, attack, status) >= TODOME_DECK_OUT_SCORE) {
                CGPMoveDSBATTLEwork(&ownSide->battle[2].battle,
                                    &ownSide->battle[1].battle);
                return 1;
            }
        }
        if (TodomeHyouka(side, work, 3, 1, attack, status) > score) {
            cardId = ownSide->battle[3].battle.layers[0].cardId;
            if (cardId < 0 || GetDeckDataAdjust(side, work, cardId) < 4) {
                CGPMoveDSBATTLEwork(&ownSide->battle[3].battle,
                                    &ownSide->battle[slot].battle);
                return 1;
            }
            if (TodomeHyouka(side, work, 3, 1, attack, status) >= TODOME_DECK_OUT_SCORE) {
                CGPMoveDSBATTLEwork(&ownSide->battle[3].battle,
                                    &ownSide->battle[slot].battle);
                return 1;
            }
        }
        return 1;
    }
    case 2:
    {
        score = TodomeHyouka(side, work, 2, 2, attack, status);
        if (TodomeHyouka(side, work, 3, 2, attack, status) > score) {
            if (ownSide->battle[3].battle.layers[0].cardId < 0 ||
                GetDeckDataAdjust(side, work, ownSide->battle[3].battle.layers[0].cardId) < 4) {
                CGPMoveDSBATTLEwork(&ownSide->battle[3].battle,
                                    &ownSide->battle[2].battle);
                return 1;
            }
            if (TodomeHyouka(side, work, 3, 2, attack, status) >= TODOME_DECK_OUT_SCORE) {
                CGPMoveDSBATTLEwork(&ownSide->battle[3].battle,
                                    &ownSide->battle[2].battle);
                return 1;
            }
        }
        return 1;
    }
    case 3:
    {
        score = TodomeHyouka(side, work, 3, 3, attack, status);
        if (score < TodomeHyouka(side, work, 2, 3, attack, status)) {
            if (ownSide->battle[2].battle.layers[0].cardId < 0 ||
                GetDeckDataAdjust(side, work, ownSide->battle[2].battle.layers[0].cardId) < 4) {
                CGPMoveDSBATTLEwork(&ownSide->battle[2].battle,
                                    &ownSide->battle[3].battle);
                return 1;
            }
            if (TodomeHyouka(side, work, 2, 3, attack, status) >= TODOME_DECK_OUT_SCORE) {
                CGPMoveDSBATTLEwork(&ownSide->battle[2].battle,
                                    &ownSide->battle[3].battle);
                return 1;
            }
        }
        return 1;
    }
    }
}

s32 CC_Kara_FrontTodomeMove(s32 side, CardGameWork *work, s32 slot)
{
    CardPlaySide *mySide;
    CardPlaySide *otherSide;
    s32 friendA[CARD_BOARD_SLOTS];
    s32 friendB[CARD_BOARD_SLOTS];
    s32 first;
    s32 second;
    s16 cardId;

    if (side == 0) {
        mySide = &work->player;
        otherSide = &work->enemy;
    } else {
        mySide = &work->enemy;
        otherSide = &work->player;
    }
    slot &= CARD_SLOT_INDEX_MASK;
    GetFriendAttack(side, work, friendA, friendB);
    switch (slot) {
    case 0:
        if (friendB[0] != 0) {
            first = TodomeHyouka(side, work, 0, 0, friendA, friendB);
            second = TodomeHyouka(side, work, 1, 0, friendA, friendB);
            if (first + 10 < second) {
                cardId = mySide->battle[1].battle.layers[0].cardId;
                if (cardId < 0 || GetDeckDataAdjust(side, work, cardId) < 3) {
                    CGPMoveDSBATTLEwork(&mySide->battle[1].battle,
                                        &mySide->battle[0].battle);
                    return 1;
                } else if (second >= TODOME_DECK_OUT_SCORE) {
                    CGPMoveDSBATTLEwork(&mySide->battle[1].battle,
                                        &mySide->battle[0].battle);
                    return 1;
                }
            } else if (second < first - 10) {
                if (CardChkAttackType(work->definitions, &mySide->battle[0].battle) != CARD_ATTACK_HAND ||
                    otherSide->battle[1].battle.layers[0].cardId >= 0) {
                    break;
                }
                CC_Kara_KabeMove(side, work, 0);
                return 1;
            } else if (first < second) {
                if (mySide->battle[1].battle.layers[0].cardId < 0 &&
                    mySide->battle[3].battle.layers[0].cardId < 0) {
                    CGPMoveDSBATTLEwork(&mySide->battle[1].battle,
                                        &mySide->battle[0].battle);
                    return 1;
                }
            }
        }
        /* fall through */
    case 2:
    case 3:
        CC_Kara_KabeMove(side, work, slot);
        /* fall through */
    default:
        return 1;
    case 1:
        if (friendB[1] != 0) {
            first = TodomeHyouka(side, work, 1, 1, friendA, friendB);
            second = TodomeHyouka(side, work, 0, 1, friendA, friendB);
            if (first + 10 < second) {
                cardId = mySide->battle[0].battle.layers[0].cardId;
                if (cardId < 0 || GetDeckDataAdjust(side, work, cardId) < 3) {
                    CGPMoveDSBATTLEwork(&mySide->battle[0].battle,
                                        &mySide->battle[1].battle);
                    return 1;
                } else if (second >= TODOME_DECK_OUT_SCORE) {
                    CGPMoveDSBATTLEwork(&mySide->battle[0].battle,
                                        &mySide->battle[1].battle);
                    return 1;
                }
            } else if (second < first - 10) {
                if (CardChkAttackType(work->definitions, &mySide->battle[1].battle) != CARD_ATTACK_HAND ||
                    otherSide->battle[0].battle.layers[0].cardId >= 0) {
                    break;
                }
                CC_Kara_KabeMove(side, work, 1);
                return 1;
            } else if (first < second) {
                if (mySide->battle[0].battle.layers[0].cardId < 0 &&
                    mySide->battle[2].battle.layers[0].cardId < 0) {
                    CGPMoveDSBATTLEwork(&mySide->battle[0].battle,
                                        &mySide->battle[1].battle);
                    return 1;
                }
            }
        }
        CC_Kara_KabeMove(side, work, slot);
        return 1;
    }
    return 1;
}

s32 CC_Kara_BackAttackMove(s32 side, CardGameWork *work, s32 battleSlot)
{
    u16 slot = battleSlot;
    CardPlaySide *sideData = &work->player;
    CardLayerStack *card;
    s32 attackType;

    if (side != 0) {
        sideData = &work->enemy;
    }
    card = &sideData->battle[slot].battle;
    attackType = CardChkAttackType(work->definitions, card);
    if (attackType == CARD_ATTACK_BALLISTIC) {
        if (BattleAreaCheckEnemy(side, work) == 0) {
            if (slot < 2) {
                if (sideData->battle[2].battle.layers[0].cardId >= 0) {
                    if (sideData->battle[3].battle.layers[0].cardId < 0) {
                        CGPMoveDSBATTLEwork(&sideData->battle[3].battle, card);
                        return 1;
                    }
                } else {
                    CGPMoveDSBATTLEwork(&sideData->battle[2].battle, card);
                    return 1;
                }
            }
            return 0;
        }
        return CC_Kara_BackTodomeMove(side, work, slot);
    }
    if (attackType == CARD_ATTACK_HAND) {
        return CC_Kara_BackTodomeMove(side, work, slot);
    }
    if (BattleAreaCheckEnemy(side, work) == 0) {
        if (slot < 2) {
            if (sideData->battle[2].battle.layers[0].cardId < 0) {
                CGPMoveDSBATTLEwork(&sideData->battle[2].battle, card);
                return 1;
            }
            if (sideData->battle[3].battle.layers[0].cardId < 0) {
                CGPMoveDSBATTLEwork(&sideData->battle[3].battle, card);
                return 1;
            }
        }
    }
    return 0;
}

/* The VX-10000 card on either side's battle board, checking both sides'
 * slot i together for i in 0..3. */
s32 VX10000Check(s32 side, CardGameWork *work)
{
    CardPlaySide *sideA;
    CardPlaySide *sideB;
    s32 slotIndex;

    if (!side) {
        sideA = &work->player;
        sideB = &work->enemy;
    } else {
        sideA = &work->enemy;
        sideB = &work->player;
    }
    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        if (sideA->battle[slotIndex].battle.layers[0].cardId == CARD_ID_VX_10000) {
            return 1;
        }
        if (sideB->battle[slotIndex].battle.layers[0].cardId == CARD_ID_VX_10000) {
            return 1;
        }
    }
    return 0;
}

/* Whether `side`'s battle board has Joachim Mizrahi (Yoakimu) in some slot and
 * one of the MoMo cards (Lv 1, Swim Suit, Lv 10) in some slot. */
s32 YoakimuMoMoCheck(s32 side, CardGameWork *work)
{
    CardPlaySide *sideData = &work->player;
    s16 *cardIdPtr;
    s32 yoakimuSlot = -1;
    s32 moMoSlot = -1;
    s32 slotIndex;

    if (side) {
        sideData = &work->enemy;
    }
    cardIdPtr = &sideData->battle[0].battle.layers[0].cardId;
    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        s16 cardId = *cardIdPtr;

        cardIdPtr += CARD_SLOT_HALFWORDS;
        if (cardId == CARD_ID_JOACHIM_MIZRAHI) {
            yoakimuSlot = slotIndex;
        }
        if (cardId == CARD_ID_LV_1_MOMO || cardId == CARD_ID_SWIM_SUIT_MOMO || cardId == CARD_ID_LV_10_MOMO) {
            moMoSlot = slotIndex;
        }
    }
    if (yoakimuSlot != -1) {
        if (moMoSlot != -1) {
            return 1;
        }
    }
    return 0;
}

/* Index of the first MoMo card (Lv 1, Swim Suit or Lv 10) on `side`'s disposal board
 * (slot index), else on its battle board (slot index | 0xF0000), else -1. */
s32 MoMoCheck(s32 side, CardGameWork *work)
{
    CardPlaySide *ownSide = (side == 0) ? &work->player : &work->enemy;
    s32 slotIndex;
    s16 cardId;

    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        cardId = *(&ownSide->disposal->battle.layers[0].cardId + slotIndex * CARD_SLOT_HALFWORDS);
        if (cardId == CARD_ID_LV_1_MOMO || cardId == CARD_ID_SWIM_SUIT_MOMO || cardId == CARD_ID_LV_10_MOMO) {
            return slotIndex;
        }
    }
    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        cardId = *(&ownSide->battle[0].battle.layers[0].cardId + slotIndex * CARD_SLOT_HALFWORDS);
        if (cardId == CARD_ID_LV_1_MOMO || cardId == CARD_ID_SWIM_SUIT_MOMO || cardId == CARD_ID_LV_10_MOMO) {
            return slotIndex | CARD_SLOT_BATTLE;
        }
    }
    return -1;
}

/* Joachim Mizrahi (Yoakimu) moves to the battle slot MoMoCheck names, or checks
 * the hand and the power of the disposal slot it was played from. A slot
 * with bits 0xF0000 set names a battle slot instead. */
s32 YoakimuMove(s32 side, CardGameWork *work, s32 slot)
{
    CardPlaySide *sideData = &work->player;
    s32 moMoResult;
    s32 battleSlot;
    s32 handCount;
    s32 i;

    if (side != 0) {
        sideData = &work->enemy;
    }
    if ((slot & CARD_SLOT_BATTLE) == 0) {
        if (sideData->disposal[slot].battle.layers[0].cardId == CARD_ID_JOACHIM_MIZRAHI) {
        moMoResult = MoMoCheck(side, work);
        if (moMoResult != -1 && (moMoResult & CARD_SLOT_BATTLE) != 0) {
            switch (moMoResult & CARD_SLOT_INDEX_MASK) {
            case 0:
                CGPMoveDSBATTLEwork(&sideData->battle[2].battle,
                                    &sideData->disposal[slot].battle);
                return 1;
            case 1:
                CGPMoveDSBATTLEwork(&sideData->battle[3].battle,
                                    &sideData->disposal[slot].battle);
                return 1;
            case 2:
                CGPMoveDSBATTLEwork(&sideData->battle[0].battle,
                                    &sideData->battle[2].battle);
                CGPMoveDSBATTLEwork(&sideData->battle[2].battle,
                                    &sideData->disposal[slot].battle);
                return 1;
            case 3:
                CGPMoveDSBATTLEwork(&sideData->battle[1].battle,
                                    &sideData->battle[3].battle);
                CGPMoveDSBATTLEwork(&sideData->battle[3].battle,
                                    &sideData->disposal[slot].battle);
                return 1;
            }
        }
        handCount = CardPlayHandCnt(&sideData->hand);
        for (i = 0; i < handCount; i++) {
            u16 cardId = (u16) sideData->hand.cards[i];

            if ((u32) (cardId - CARD_ID_RIFLE) < 2U) {
                return 0;
            }
        }
        return CheckNowPower(side, work, slot, sideData->disposal[slot].battle.layers[0].cardId) == 0;
        }
        return 0;
    }
    battleSlot = slot & CARD_SLOT_INDEX_MASK;
    if (sideData->battle[battleSlot].battle.layers[0].cardId == CARD_ID_JOACHIM_MIZRAHI) {
    moMoResult = MoMoCheck(side, work);
    if (moMoResult != -1 && (moMoResult & CARD_SLOT_BATTLE) != 0) {
        switch (moMoResult & CARD_SLOT_INDEX_MASK) {
        case 0:
            if (battleSlot != 2) {
                CGPMoveDSBATTLEwork(&sideData->battle[2].battle,
                                    &sideData->battle[battleSlot].battle);
            }
            return 1;
        case 1:
            if (battleSlot != 3) {
                CGPMoveDSBATTLEwork(&sideData->battle[3].battle,
                                    &sideData->battle[battleSlot].battle);
            }
            return 1;
        }
    }
    return CheckNowPower(side, work, slot | CARD_SLOT_BATTLE,
                         sideData->battle[battleSlot].battle.layers[0].cardId) != 0;
    }
    return 0;
}

s32 MoMoMove(s32 side, CardGameWork *work, s32 slot)
{
    s32 attack[CARD_BOARD_SLOTS];
    s32 status[CARD_BOARD_SLOTS];
    CardPlaySide *ownSide;
    s32 idx;
    s32 i;
    s32 scoreB;
    s32 scoreA;
    s32 swapHi;

    ownSide = &work->player;
    if (side != 0) {
        ownSide = &work->enemy;
    }

    i = 0;
    idx = -1;
    if (ownSide->battle[0].battle.layers[0].cardId == CARD_ID_JOACHIM_MIZRAHI) {
        idx = 0;
    } else {
    scanLoop:
        i++;
        if (i < CARD_BOARD_SLOTS) {
            if (ownSide->battle[i].battle.layers[0].cardId != CARD_ID_JOACHIM_MIZRAHI) {
                goto scanLoop;
            }
            idx = i;
        }
    }
    if (idx == -1) {
        return 0;
    }

    if ((slot & CARD_SLOT_BATTLE) == 0) {
        if (CheckNowPower(side, work, idx | CARD_SLOT_BATTLE, ownSide->battle[idx].battle.layers[0].cardId) == 0
            && (ownSide->disposal[slot].battle.flags & CARD_STACK_FLAG_FROZEN)
            && CardPlayYamaCnt(ownSide) >= 11
            && CheckDownKaifukuCommand(side, work) == 0) {
            return 1;
        }
        switch (idx) {
        case 0:
            CGPMoveDSBATTLEwork(&ownSide->battle[2].battle, &ownSide->battle[0].battle);
            CGPMoveDSBATTLEwork(&ownSide->battle[0].battle, &ownSide->battle[slot].battle);
            break;
        case 1:
            CGPMoveDSBATTLEwork(&ownSide->battle[3].battle, &ownSide->battle[1].battle);
            CGPMoveDSBATTLEwork(&ownSide->battle[1].battle, &ownSide->battle[slot].battle);
            break;
        case 2:
            CGPMoveDSBATTLEwork(&ownSide->battle[0].battle, &ownSide->disposal[slot].battle);
            break;
        case 3:
            CGPMoveDSBATTLEwork(&ownSide->battle[1].battle, &ownSide->disposal[slot].battle);
            break;
        default:
            return 1;
        }
        return 1;
    }

    slot &= CARD_SLOT_INDEX_MASK;
    if (CheckNowPower(side, work, idx | CARD_SLOT_BATTLE, ownSide->battle[idx].battle.layers[0].cardId) == 0
        && GetDeckDataAdjust(side, work, ownSide->battle[slot].battle.layers[0].cardId) != 2
        && (ownSide->battle[slot].battle.flags & CARD_STACK_FLAG_FROZEN)
        && CardPlayYamaCnt(ownSide) >= 11) {
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (ownSide->disposal[i].battle.layers[0].cardId < 0
                && CheckDownKaifukuCommand(side, work) == 0) {
                CGPMoveDSBATTLEwork(&ownSide->disposal[i].battle, &ownSide->battle[slot].battle);
                return 1;
            }
        }
    }

    switch (idx) {
    case 0:
        CGPMoveDSBATTLEwork(&ownSide->battle[2].battle, &ownSide->battle[0].battle);
        return 1;
    case 1:
        CGPMoveDSBATTLEwork(&ownSide->battle[3].battle, &ownSide->battle[1].battle);
        return 1;
    case 2:
        if (slot != 0) {
            CGPMoveDSBATTLEwork(&ownSide->battle[0].battle, &ownSide->battle[slot].battle);
        }
        GetFriendAttack(side, work, attack, status);
        scoreA = TodomeHyouka(side, work, 0, 0, attack, status);
        swapHi = status[3];
        status[3] = status[2];
        status[2] = swapHi;
        scoreB = TodomeHyouka(side, work, 1, 0, attack, status);
        if (scoreA < scoreB) {
            if (ownSide->battle[1].battle.layers[0].cardId < 0
                || GetDeckDataAdjust(side, work, ownSide->battle[1].battle.layers[0].cardId) < 4) {
                CGPMoveDSBATTLEwork(&ownSide->battle[1].battle, &ownSide->battle[0].battle);
                CGPMoveDSBATTLEwork(&ownSide->battle[3].battle, &ownSide->battle[2].battle);
                return 1;
            }
            if (scoreB >= TODOME_DECK_OUT_SCORE) {
                CGPMoveDSBATTLEwork(&ownSide->battle[1].battle, &ownSide->battle[0].battle);
                CGPMoveDSBATTLEwork(&ownSide->battle[3].battle, &ownSide->battle[2].battle);
                return 1;
            }
        }
        return 1;
    case 3:
        if (slot != 1) {
            CGPMoveDSBATTLEwork(&ownSide->battle[1].battle, &ownSide->battle[slot].battle);
        }
        GetFriendAttack(side, work, attack, status);
        scoreA = TodomeHyouka(side, work, 1, 1, attack, status);
        swapHi = status[3];
        status[3] = status[2];
        status[2] = swapHi;
        scoreB = TodomeHyouka(side, work, 0, 1, attack, status);
        if (scoreA < scoreB) {
            if (ownSide->battle[0].battle.layers[0].cardId < 0
                || GetDeckDataAdjust(side, work, ownSide->battle[0].battle.layers[0].cardId) < 4) {
                CGPMoveDSBATTLEwork(&ownSide->battle[0].battle, &ownSide->battle[1].battle);
                CGPMoveDSBATTLEwork(&ownSide->battle[2].battle, &ownSide->battle[3].battle);
                return 1;
            }
            if (scoreB >= TODOME_DECK_OUT_SCORE) {
                CGPMoveDSBATTLEwork(&ownSide->battle[0].battle, &ownSide->battle[1].battle);
                CGPMoveDSBATTLEwork(&ownSide->battle[2].battle, &ownSide->battle[3].battle);
                return 1;
            }
        }
        return 1;
    default:
        return 1;
    }
}

/*
 * Whether one of the side's own disposal or battle slots holds a card whose
 * GetDeckDataAdjust value is 2. Negative card ids are empty slots.
 */
s32 CheckFriendKabe(s32 side, CardGameWork *work)
{
    CardPlaySide *ownSide = &work->enemy;
    s32 cardIndex;

    if (side == 0) {
        ownSide = &work->player;
        cardIndex = 0;
    } else {
        cardIndex = 0;
    }
    cardIndex = 0;
    while (cardIndex < 4) {
        s16 cardNo = *&ownSide->disposal[cardIndex].battle.layers[0].cardId;

        if (cardNo >= 0 && 2 == GetDeckDataAdjust(side, work, cardNo)) {
            return 1;
        }
        cardIndex++;
    }
    cardIndex = 0;
    {
        s16 *battleCardId = &ownSide->battle[0].battle.layers[0].cardId;

        while (cardIndex < 4) {
            s16 battleNo = *battleCardId;

            if (battleNo >= 0 && GetDeckDataAdjust(side, work, battleNo) == 2) {
                return 1;
            }
            battleCardId += CARD_SLOT_HALFWORDS;
            cardIndex++;
        }
    }
    return 0;
}

/*
 * Whether one of the side's own disposal slots holds a card whose
 * GetDeckDataAdjust value is at least 3. Negative card ids are empty slots.
 */
s32 CheckFriendSensi(s32 side, CardGameWork *work)
{
    s32 cardIndex;
    CardPlaySide *ownSide = &work->enemy;

    if (side == 0) {
        ownSide = &work->player;
        cardIndex = 0;
    } else {
        cardIndex = 0;
    }
    cardIndex = 0;
    while (cardIndex < 4) {
        s16 cardNo = *&ownSide->disposal[cardIndex].battle.layers[0].cardId;

        if (cardNo >= 0 && GetDeckDataAdjust(side, work, cardNo) >= 3) {
            return 1;
        }
        cardIndex++;
    }
    return 0;
}

s32 CC_Kara_SensiMove(s32 side, CardGameWork *work, s32 slot)
{
    CardPlaySide *otherSide;
    CardPlaySide *ownSide;
    s32 i;
    s32 disposalAttack;
    s32 attack;
    s32 powerA;
    s32 powerB;
    s32 friendAttack;
    s32 disposeResult;

    if (side == 0) {
        ownSide = &work->player;
        otherSide = &work->enemy;
    } else {
        ownSide = &work->enemy;
        otherSide = &work->player;
    }

    if ((slot & CARD_SLOT_BATTLE) == 0) {
        if ((ownSide->disposal[slot].battle.flags & CARD_STACK_FLAG_FROZEN) != 0) {
            if (11 <= CardPlayYamaCnt(ownSide)) {
                if (CheckDownKaifukuCommand(side, work) == 0) {
                    return 1;
                }
            }
        }

        disposalAttack = CardChkAttackType(work->definitions, &ownSide->disposal[slot].battle);
        if (disposalAttack == 1) {
            if (otherSide->battle[1].battle.layers[0].cardId >= 0) {
                if (ownSide->battle[0].battle.layers[0].cardId < 0) {
                    CGPMoveDSBATTLEwork(&ownSide->battle[0].battle,
                                        &ownSide->disposal[slot].battle);
                    return 1;
                }
                powerA = CheckNowPower(side, work, CARD_SLOT_BATTLE, ownSide->battle[0].battle.layers[0].cardId);
                powerB = CheckNowPower(side, work, slot, ownSide->disposal[slot].battle.layers[0].cardId);
                if (powerA < powerB && YoakimuMoMoCheck(side, work) == 0) {
                    CGPMoveDSBATTLEwork(&ownSide->battle[0].battle,
                                        &ownSide->disposal[slot].battle);
                    return 1;
                }
            }
            if (otherSide->battle[0].battle.layers[0].cardId >= 0) {
                if (ownSide->battle[1].battle.layers[0].cardId < 0) {
                    CGPMoveDSBATTLEwork(&ownSide->battle[1].battle,
                                        &ownSide->disposal[slot].battle);
                    return 1;
                }
                powerA = CheckNowPower(side, work, CARD_SLOT_BATTLE | 1, ownSide->battle[1].battle.layers[0].cardId);
                powerB = CheckNowPower(side, work, slot, ownSide->disposal[slot].battle.layers[0].cardId);
                if (powerA < powerB) {
                    if (YoakimuMoMoCheck(side, work) == 0) {
                        CGPMoveDSBATTLEwork(&ownSide->battle[1].battle,
                                            &ownSide->disposal[slot].battle);
                        return 1;
                    }
                }
            }
            if (BattleAreaCheckFriend(side, work) > 0) {
                CC_Kara_HaibiKabeMove(side, work, slot);
                return 1;
            }
            return 0;
        }

        for (i = 3; i >= 0; i--) {
            if (ownSide->battle[i].battle.layers[0].cardId < 0) {
                CGPMoveDSBATTLEwork(&ownSide->battle[i].battle,
                                    &ownSide->disposal[slot].battle);
                return 1;
            }
            if (CardChkAttackType(work->definitions, &ownSide->battle[i].battle) == CARD_ATTACK_HAND) {
                if (i == 0 && otherSide->battle[1].battle.layers[0].cardId >= 0) {
                    powerA = CheckNowPower(side, work, CARD_SLOT_BATTLE, ownSide->battle[0].battle.layers[0].cardId);
                    powerB = CheckNowPower(side, work, slot, ownSide->disposal[slot].battle.layers[0].cardId);
                    if (powerA >= powerB && YoakimuMoMoCheck(side, work) == 0) {
                        continue;
                    }
                }
                if (i == 1 && otherSide->battle[0].battle.layers[0].cardId >= 0) {
                    powerA = CheckNowPower(side, work, CARD_SLOT_BATTLE | 1, ownSide->battle[1].battle.layers[0].cardId);
                    powerB = CheckNowPower(side, work, slot, ownSide->disposal[slot].battle.layers[0].cardId);
                    if (powerA >= powerB && YoakimuMoMoCheck(side, work) == 0) {
                        continue;
                    }
                }
                if (GetDeckDataAdjust(side, work, ownSide->battle[i].battle.layers[0].cardId) == 4) {
                    continue;
                }
                CGPMoveDSBATTLEwork(&ownSide->battle[i].battle,
                                    &ownSide->disposal[slot].battle);
                return 1;
            }
        }

        for (i = 3; i >= 0; i--) {
            if (GetDeckDataAdjust(side, work, ownSide->battle[i].battle.layers[0].cardId) == 4) {
                continue;
            }
            powerA = CheckNowPower(side, work, CARD_SLOT_BATTLE | i, ownSide->battle[i].battle.layers[0].cardId);
            powerB = CheckNowPower(side, work, slot, ownSide->disposal[slot].battle.layers[0].cardId);
            if (ownSide->battle[i].battle.layers[0].cardId >= 0) {
                s32 attackType;
                s32 ret;
                ret = BattleAreaCheckEnemy(side, work);
                attackType = CardChkAttackType(work->definitions, &ownSide->battle[i].battle);
                if (attackType == CARD_ATTACK_SPREAD) {
                    powerA = powerA * ret;
                }
                if ((&ownSide->battle[i].battle.layers[0])->cardId == CARD_ID_SIMEON) {
                    powerA = powerA + 3;
                }
                if (ownSide->disposal[slot].battle.layers[0].cardId == CARD_ID_SIMEON) {
                    powerA = powerA + 3;
                }
                if (3 == disposalAttack) {
                    powerB = powerB * ret;
                }
            }
            if (powerA >= powerB) {
                continue;
            }
            if (YoakimuMoMoCheck(side, work) != 0) {
                continue;
            }
            CGPMoveDSBATTLEwork(&ownSide->battle[i].battle,
                                &ownSide->disposal[slot].battle);
            return 1;
        }
        return 0;
    }

    slot &= CARD_SLOT_INDEX_MASK;
    if (ownSide->battle[slot].battle.flags & CARD_STACK_FLAG_FROZEN) {
        if (11 <= CardPlayYamaCnt(ownSide)) {
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (ownSide->disposal[i].battle.layers[0].cardId < 0) {
                    if (CheckDownKaifukuCommand(side, work) == 0) {
                        CGPMoveDSBATTLEwork(&ownSide->disposal[i].battle,
                                            &ownSide->battle[slot].battle);
                        return 1;
                    }
                }
            }
        }
    }
    if ((u32) slot < 2) {
        attack = CardChkAttackType(work->definitions, &ownSide->battle[slot].battle);
        if (attack == 1 && otherSide->battle[0].battle.layers[0].cardId < 0
            && otherSide->battle[1].battle.layers[0].cardId < 0) {
            if (CardPlayYamaCnt(ownSide) >= 11) {
                s32 count = CardPlayYamaCnt(ownSide);
                if (count > CheckEnemyAttack(side, work)) {
                    friendAttack = BattleAreaCheckFriend(side, work);
                    disposeResult = DisposeAreaCheckFriend(side, work);
                    if (friendAttack == attack && disposeResult == 0) {
                        CGPMoveDSBATTLEwork(&ownSide->disposal[0].battle,
                                            &ownSide->battle[slot].battle);
                        return 1;
                    }
                }
            }
            return CC_Kara_KabeMove(side, work, slot);
        }
    }
    CC_Kara_AttackMove(side, work, slot);
    return 0;
}

/*
 * Moves the disposal slot `slot` into one of the side's battle slots (the
 * first empty one, or the one whose card has the lower life) when the deck
 * data and the battle board allow it. Always returns 1.
 */
s32 CC_Kara_HaibiKabeMove(s32 side, CardGameWork *work, s32 slot)
{
    CardPlaySide *ownSide = &work->player;

    if (side) {
        ownSide = &work->enemy;
    }
    if ((slot & CARD_SLOT_BATTLE) == 0) {
        slot &= CARD_SLOT_INDEX_MASK;
        if (ownSide->battle[3].battle.layers[0].cardId >= 0) {
            if (ownSide->battle[1].battle.layers[0].cardId < 0) {
                CGPMoveDSBATTLEwork(&ownSide->battle[1].battle,
                                    &ownSide->disposal[slot].battle);
                return 1;
            } else if (ownSide->battle[0].battle.layers[0].cardId < 0) {
                CGPMoveDSBATTLEwork(&ownSide->battle[0].battle,
                                    &ownSide->disposal[slot].battle);
                return 1;
            }
            if (YoakimuMoMoCheck(side, work) != 0) {
                return 1;
            }
            if (GetDeckDataAdjust(side, work, ownSide->battle[1].battle.layers[0].cardId) == 2 &&
                ownSide->battle[1].battle.layers[0].life < ownSide->disposal[slot].battle.layers[0].life) {
                CGPMoveDSBATTLEwork(&ownSide->battle[1].battle,
                                    &ownSide->disposal[slot].battle);
                return 1;
            }
            return 1;
        } else {
            if (ownSide->battle[2].battle.layers[0].cardId >= 0) {
                if (ownSide->battle[0].battle.layers[0].cardId < 0) {
                    CGPMoveDSBATTLEwork(&ownSide->battle[0].battle,
                                        &ownSide->disposal[slot].battle);
                    return 1;
                }
                if (ownSide->battle[1].battle.layers[0].cardId < 0) {
                    CGPMoveDSBATTLEwork(&ownSide->battle[1].battle,
                                        &ownSide->disposal[slot].battle);
                    return 1;
                }
                if (YoakimuMoMoCheck(side, work) != 0) {
                    return 1;
                }
                if (GetDeckDataAdjust(side, work, ownSide->battle[0].battle.layers[0].cardId) == 2) {
                    if (ownSide->battle[0].battle.layers[0].life < ownSide->disposal[slot].battle.layers[0].life) {
                        CGPMoveDSBATTLEwork(&ownSide->battle[0].battle,
                                            &ownSide->disposal[slot].battle);
                        return 1;
                    }
                }
                if (GetDeckDataAdjust(side, work, ownSide->battle[1].battle.layers[0].cardId) != 2) {
                    return 1;
                }
                if (ownSide->battle[1].battle.layers[0].life < ownSide->disposal[slot].battle.layers[0].life) {
                    CGPMoveDSBATTLEwork(&ownSide->battle[1].battle,
                                        &ownSide->disposal[slot].battle);
                    return 1;
                }
            }
        }
    }
    return 1;
}

/*
 * Wall placement for the caller's own battle slot `slot`: moves a card into
 * the empty or weaker neighbouring battle slot, chosen from the deck data,
 * life and attack type of the cards around it. Returns 1 when a card moved.
 */
s32 CC_Kara_KabeMove(s32 side, CardGameWork *work, s32 slot)
{
    CardPlaySide *own;
    s32 count2;
    s32 count3;
    s32 dispFriend;
    s32 ownType;
    CardLayerStack *slotStack;
    s32 attackType0;
    s32 attackType1;
    s32 attackType2;
    s32 attackType3;
    s16 cardId;

    own = &work->player;
    if (side != 0) {
        own = &work->enemy;
    }
    count2 = GetShoukin(&own->battle[2].battle);
    count3 = GetShoukin(&own->battle[3].battle);
    count2 += GetDeckData(side, work, own->battle[2].battle.layers[0].cardId) * 10;
    count3 += GetDeckData(side, work, own->battle[3].battle.layers[0].cardId) * 10;
    dispFriend = DisposeAreaCheckFriend(side, work);
    if (GetDeckData(side, work, own->battle[2].battle.layers[0].cardId) == 4) {
        count2 += 100;
    }
    if (GetDeckData(side, work, own->battle[3].battle.layers[0].cardId) == 4) {
        count3 += 100;
    }
    slot &= CARD_SLOT_INDEX_MASK;
    slotStack = &own->battle[slot].battle;
    ownType = CardChkAttackType(work->definitions, slotStack);

    switch (slot) {
    case 0:
        if (count2 < count3) {
            if (own->battle[1].battle.layers[0].cardId < 0) {
                CGPMoveDSBATTLEwork(&own->battle[1].battle,
                                    &own->battle[0].battle);
                return 1;
            }
            if (GetDeckDataAdjust(side, work, own->battle[1].battle.layers[0].cardId) == 2 &&
                own->battle[0].battle.layers[0].life > own->battle[1].battle.layers[0].life &&
                YoakimuMoMoCheck(side, work) == 0) {
                CGPMoveDSBATTLEwork(&own->battle[1].battle,
                                    &own->battle[0].battle);
                return 1;
            }
        }
        if (own->battle[2].battle.layers[0].cardId >= 0 &&
            GetDeckDataAdjust(side, work, own->battle[2].battle.layers[0].cardId) == 2) {
            attackType2 = CardChkAttackType(work->definitions, &own->battle[2].battle);
            attackType0 = CardChkAttackType(work->definitions, &own->battle[0].battle);
            if (attackType0 != 1 && attackType2 == 1) {
                CGPMoveDSBATTLEwork(&own->battle[2].battle,
                                    &own->battle[0].battle);
                return 1;
            }
        }
        if (ownType == 1) {
            break;
        }
        if (CheckFriendSensi(side, work) != 0) {
            break;
        }
        if (own->battle[2].battle.layers[0].cardId < 0) {
            if (own->battle[1].battle.layers[0].cardId >= 0 &&
                own->battle[3].battle.layers[0].cardId >= 0) {
                CGPMoveDSBATTLEwork(&own->battle[2].battle,
                                    &own->battle[0].battle);
                return 1;
            }
        }
        break;
    case 1:
        if (count3 < count2) {
            if (own->battle[0].battle.layers[0].cardId < 0) {
                CGPMoveDSBATTLEwork(&own->battle[0].battle,
                                    &own->battle[1].battle);
                return 1;
            }
            if (GetDeckDataAdjust(side, work, own->battle[0].battle.layers[0].cardId) == 2 &&
                own->battle[1].battle.layers[0].life > own->battle[0].battle.layers[0].life &&
                YoakimuMoMoCheck(side, work) == 0) {
                CGPMoveDSBATTLEwork(&own->battle[0].battle,
                                    slotStack);
                return 1;
            }
        }
        if (own->battle[3].battle.layers[0].cardId >= 0 &&
            GetDeckDataAdjust(side, work, own->battle[3].battle.layers[0].cardId) == 2) {
            attackType3 = CardChkAttackType(work->definitions, &own->battle[3].battle);
            attackType1 = CardChkAttackType(work->definitions, &own->battle[1].battle);
            if (attackType1 != 1 && attackType3 == 1) {
                CGPMoveDSBATTLEwork(&own->battle[2].battle,
                                    &own->battle[0].battle);
                return 1;
            }
        }
        if (ownType == 1) {
            break;
        }
        if (CheckFriendSensi(side, work) != 0) {
            break;
        }
        if (own->battle[3].battle.layers[0].cardId < 0 &&
            own->battle[0].battle.layers[0].cardId >= 0 &&
            own->battle[2].battle.layers[0].cardId >= 0) {
            CGPMoveDSBATTLEwork(&own->battle[3].battle,
                                &own->battle[1].battle);
            return 1;
        }
        break;
    case 2:
    case 3:
        if (dispFriend > 0 && CheckFriendSensi(side, work) == 1) {
            if (own->battle[0].battle.layers[0].cardId < 0) {
                CGPMoveDSBATTLEwork(&own->battle[0].battle,
                                    &own->battle[slot].battle);
                return 1;
            }
            if (own->battle[1].battle.layers[0].cardId < 0) {
                CGPMoveDSBATTLEwork(&own->battle[1].battle,
                                    &own->battle[slot].battle);
                return 1;
            }
        }
        cardId = own->battle[0].battle.layers[0].cardId;
        if (cardId >= 0 && GetDeckDataAdjust(side, work, cardId) == 2 &&
            own->battle[0].battle.layers[0].life < own->battle[slot].battle.layers[0].life) {
            if (CardChkAttackType(work->definitions, &own->battle[0].battle) != CARD_ATTACK_HAND &&
                YoakimuMoMoCheck(side, work) == 0) {
                CGPMoveDSBATTLEwork(&own->battle[0].battle,
                                    &own->battle[slot].battle);
                return 1;
            }
        }
        if (own->battle[1].battle.layers[0].cardId >= 0 &&
            GetDeckDataAdjust(side, work, own->battle[1].battle.layers[0].cardId) == 2 &&
            own->battle[1].battle.layers[0].life < own->battle[slot].battle.layers[0].life) {
            if (CardChkAttackType(work->definitions, &own->battle[1].battle) != CARD_ATTACK_HAND &&
                YoakimuMoMoCheck(side, work) == 0) {
                CGPMoveDSBATTLEwork(&own->battle[1].battle,
                                    &own->battle[slot].battle);
                return 1;
            }
        }
        if (slot == 3 && own->battle[0].battle.layers[0].cardId < 0 &&
            own->battle[2].battle.layers[0].cardId >= 0) {
            CGPMoveDSBATTLEwork(&own->battle[0].battle,
                                &own->battle[3].battle);
            return 1;
        }
        if (slot == 2 && own->battle[1].battle.layers[0].cardId < 0 &&
            own->battle[3].battle.layers[0].cardId >= 0) {
            CGPMoveDSBATTLEwork(&own->battle[1].battle,
                                &own->battle[2].battle);
            return 1;
        }
        break;
    }
    return 0;
}

/* Escape command: the friend and enemy counts on the board must allow it,
 * then a card that is not out-attacked moves to an empty disposal slot or
 * runs MoveFase_Type4. A slot with bits 0xF0000 set names a battle slot. */
s32 EscapeMove(s32 side, CardGameWork *work, s32 slot)
{
    CardPlaySide *sideData = &work->player;
    s32 disposeFriends;
    s32 battleFriends;
    s32 enemyBattle;
    s32 enemyAttack;
    s32 attackType;
    s32 i;

    if (side != 0) {
        sideData = &work->enemy;
    }
    disposeFriends = DisposeAreaCheckFriend(side, work);
    battleFriends = BattleAreaCheckFriend(side, work);
    if (disposeFriends + battleFriends >= 3) {
        return 0;
    }
    if (CheckEnemy12Attack(side == 0, work) >= 3) {
        return 0;
    }
    enemyBattle = BattleAreaCheckEnemy(side, work);
    if (enemyBattle + DisposeAreaCheckEnemy(side, work) < 2) {
        return 0;
    }
    if (CheckFriendKabe(side, work) != 1) {
        enemyAttack = CheckEnemy12Attack(side, work);
        if ((slot & CARD_SLOT_BATTLE) == 0) {
            attackType = CardChkAttackType(work->definitions, &sideData->disposal[slot].battle);
            if (sideData->disposal[slot].battle.power < 2 && attackType == CARD_ATTACK_SPREAD) {
                if (BattleAreaCheckEnemy(side, work) >= 2) {
                    return 0;
                }
            }
            if (enemyAttack >= sideData->disposal[slot].battle.layers[0].life) {
                CC_Kara_MoveFase_Type4(side, work, slot);
                return 1;
            }
            return 0;
        } else {
            slot &= CARD_SLOT_INDEX_MASK;
            attackType = CardChkAttackType(work->definitions, &sideData->battle[slot].battle);
            if (sideData->battle[slot].battle.power < 2 && attackType == CARD_ATTACK_SPREAD) {
                if (BattleAreaCheckEnemy(side, work) >= 2) {
                    return 0;
                }
            }
            if (battleFriends == 1) {
                if (enemyAttack >= sideData->battle[slot].battle.layers[0].life) {
                    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                        if (sideData->disposal[i].battle.layers[0].cardId < 0) {
                            CGPMoveDSBATTLEwork(&sideData->disposal[i].battle,
                                                &sideData->battle[slot].battle);
                            return 1;
                        }
                    }
                }
                return 0;
            }
            CC_Kara_MoveFase_Type4(side, work, slot | CARD_SLOT_BATTLE);
            return 1;
        }
    }
    return 0;
}

/*
 * Level-up result for the card in slot `slot` of `side`: 0x0F0000 tags a
 * battle slot, otherwise a disposal slot. The card's id selects which hand
 * card is looked for; 3, 2 or -1 is returned.
 */
s32 LvUpCheck(s32 side, CardGameWork *work, s32 slot)
{
    CardPlaySide *sideData = &work->player;
    s32 handCount;
    s32 i;

    if (side != 0) {
        sideData = &work->enemy;
    }
    handCount = CardPlayHandCnt(&sideData->hand);
    if ((slot & CARD_SLOT_BATTLE) == 0) {
        switch (sideData->disposal[slot].battle.layers[0].cardId) {
        case CARD_ID_LV_1_SHION:
            for (i = 0; i < handCount; i++) {
                if (sideData->hand.cards[i] == CARD_ID_LV_10_SHION) {
                    return 3;
                }
            }
            break;
        case CARD_ID_LV_1_ZIGGY:
            for (i = 0; i < handCount; i++) {
                if (sideData->hand.cards[i] == CARD_ID_LV_10_ZIGGY) {
                    return 2;
                }
            }
            break;
        case CARD_ID_LV_1_MOMO:
            for (i = 0; i < handCount; i++) {
                if (sideData->hand.cards[i] == CARD_ID_LV_10_MOMO) {
                    return 3;
                }
            }
            break;
        case CARD_ID_LV_1_KOS_MOS:
            for (i = 0; i < handCount; i++) {
                if (sideData->hand.cards[i] == CARD_ID_LV_10_KOS_MOS) {
                    return -1;
                }
            }
            break;
        case CARD_ID_LV_1_JR:
            for (i = 0; i < handCount; i++) {
                if (sideData->hand.cards[i] == CARD_ID_LV_10_JR) {
                    return 2;
                }
            }
            break;
        case CARD_ID_LV_1_CHAOS:
            for (i = 0; i < handCount; i++) {
                if (sideData->hand.cards[i] == CARD_ID_LV_10_CHAOS) {
                    return 3;
                }
            }
            break;
        }
    } else {
        switch (sideData->battle[slot & CARD_SLOT_INDEX_MASK].battle.layers[0].cardId) {
        case CARD_ID_LV_1_SHION:
            for (i = 0; i < handCount; i++) {
                if (sideData->hand.cards[i] == CARD_ID_LV_10_SHION) {
                    return 3;
                }
            }
            break;
        case CARD_ID_LV_1_ZIGGY:
            for (i = 0; i < handCount; i++) {
                if (sideData->hand.cards[i] == CARD_ID_LV_10_ZIGGY) {
                    return 2;
                }
            }
            break;
        case CARD_ID_LV_1_MOMO:
            for (i = 0; i < handCount; i++) {
                if (sideData->hand.cards[i] == CARD_ID_LV_10_MOMO) {
                    return 3;
                }
            }
            break;
        case CARD_ID_LV_1_KOS_MOS:
            for (i = 0; i < handCount; i++) {
                if (sideData->hand.cards[i] == CARD_ID_LV_10_KOS_MOS) {
                    return -1;
                }
            }
            break;
        case CARD_ID_LV_1_JR:
            for (i = 0; i < handCount; i++) {
                if (sideData->hand.cards[i] == CARD_ID_LV_10_JR) {
                    return 2;
                }
            }
            break;
        case CARD_ID_LV_1_CHAOS:
            for (i = 0; i < handCount; i++) {
                if (sideData->hand.cards[i] == CARD_ID_LV_10_CHAOS) {
                    return 3;
                }
            }
            break;
        }
    }
    return -1;
}

s32 CC_Kara_MoveFase_Type0(s32 side, CardGameWork *work, s32 slot)
{
    CardPlaySide *sideData = &work->player;
    s32 attackType;
    s32 i;

    if (side != 0) {
        sideData = &work->enemy;
    }
    if (LvUpCheck(side, work, slot) != -1) {
        return CC_Kara_MoveFase_Type2(side, work, slot);
    }
    CardPlayHandCnt(&sideData->hand);
    if (CardPlayYamaCnt(sideData) < 10 ||
        CardPlayYamaCnt(sideData) < CheckEnemyAttack(side, work)) {
        return CC_Kara_MoveFase_Type2(side, work, slot);
    }
    if (CheckGeneration(side, work) != 0) {
        if ((slot & CARD_SLOT_BATTLE) == 0) {
            attackType = CardChkAttackType(work->definitions, &sideData->disposal[slot].battle);
            if (attackType != CARD_ATTACK_HAND) {
                return CC_Kara_MoveFase_Type2(side, work, slot);
            }
            if (DisposeAreaCheckFriend(side, work) >= 2) {
                if (CheckFriendKakutouCnt(side, work) < 2) {
                    return CC_Kara_MoveFase_Type2(side, work, slot);
                }
            }
        } else {
            return CC_Kara_MoveFase_Type2(side, work, slot);
        }
    }
    if ((slot & CARD_SLOT_BATTLE) != 0) {
        slot &= CARD_SLOT_INDEX_MASK;
        if (CheckGeneration(side, work) == 0) {
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (sideData->disposal[i].battle.layers[0].cardId < 0) {
                    CGPMoveDSBATTLEwork(&sideData->disposal[i].battle,
                                        &sideData->battle[slot].battle);
                    return 1;
                }
            }
        }
    }
    return 0;
}

/*
 * Type-one AI move: a level-up check, then the hand and deck counts and the
 * attack type of the chosen card, falling back to the generation and the
 * disposal-to-battle move when neither applies.
 */
s32 CC_Kara_MoveFase_Type1(s32 side, CardGameWork *work, s32 slot)
{
    CardPlaySide *ownSide = &work->player;
    s32 attackType;
    s32 slotIndex;

    if (side != 0) {
        ownSide = &work->enemy;
    }
    if (LvUpCheck(side, work, slot) != -1) {
        return CC_Kara_MoveFase_Type3(side, work, slot);
    }
    CardPlayHandCnt(&ownSide->hand);
    if (CardPlayYamaCnt(ownSide) < 10 ||
        CardPlayYamaCnt(ownSide) < CheckEnemyAttack(side, work)) {
        if ((slot & CARD_SLOT_BATTLE) == 0) {
            attackType = CardChkAttackType(work->definitions,
                                           &ownSide->disposal[slot].battle);
        } else {
            attackType = CardChkAttackType(work->definitions,
                                           &ownSide->battle[slot & CARD_SLOT_INDEX_MASK].battle);
        }
        if (attackType != CARD_ATTACK_HAND) {
            return CC_Kara_MoveFase_Type3(side, work, slot);
        }
        return CC_Kara_MoveFase_Type2(side, work, slot);
    }
    if (CheckGeneration(side, work) != 0) {
        return CC_Kara_MoveFase_Type3(side, work, slot);
    }
    if (slot & CARD_SLOT_BATTLE) {
        slot &= CARD_SLOT_INDEX_MASK;
        if (CheckGeneration(side, work) == 0) {
            for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
                if (ownSide->disposal[slotIndex].battle.layers[0].cardId < 0) {
                    CGPMoveDSBATTLEwork(&ownSide->disposal[slotIndex].battle,
                                        &ownSide->battle[slot].battle);
                    return 1;
                }
            }
        }
    }
    return 0;
}

/*
 * Move decision for one slot of the caller's board. `slot` is a disposal slot
 * index, or a battle slot index tagged with 0xF0000.
 */
s32 CC_Kara_MoveFase_Type2(s32 side, CardGameWork *work, s32 slot)
{
    CardPlaySide *own;
    CardPlaySide *other;
    u16 index;
    s32 result;
    s32 dispFriend;
    s32 k;
    s32 m;

    if (side == 0) {
        own = &work->player;
        other = &work->enemy;
    } else {
        own = &work->enemy;
        other = &work->player;
    }

    if ((slot & CARD_SLOT_BATTLE) == 0) {
        if (own->disposal[slot].battle.layers[0].cardId == CARD_ID_LV_1_MOMO ||
            own->disposal[slot].battle.layers[0].cardId == CARD_ID_SWIM_SUIT_MOMO ||
            own->disposal[slot].battle.layers[0].cardId == CARD_ID_LV_10_MOMO) {
            if (MoMoMove(side, work, slot) == 1) {
                return 1;
            }
        }
        if (own->disposal[slot].battle.layers[0].cardId >= 0 &&
            own->disposal[slot].battle.layers[1].cardId != CARD_ID_SHIELD &&
            own->disposal[slot].battle.layers[1].cardId != CARD_ID_BATTLE_SUIT) {
            CC_Kara_SensiMove(side, work, slot);
            return 1;
        }
    } else {
        index = slot;
        if (own->battle[index].battle.layers[0].cardId == CARD_ID_LV_1_MOMO ||
            own->battle[index].battle.layers[0].cardId == CARD_ID_SWIM_SUIT_MOMO ||
            own->battle[index].battle.layers[0].cardId == CARD_ID_LV_10_MOMO) {
            if (MoMoMove(side, work, index | CARD_SLOT_BATTLE) == 1) {
                return 1;
            }
        }
        if (own->battle[index].battle.layers[0].cardId >= 0 &&
            own->battle[index].battle.layers[1].cardId != CARD_ID_SHIELD &&
            own->battle[index].battle.layers[1].cardId != CARD_ID_BATTLE_SUIT) {
            CC_Kara_SensiMove(side, work, slot);
            return 1;
        }
    }

    if (slot & CARD_SLOT_BATTLE) {
        goto tagged;
    }

    if (own->battle[3].battle.layers[0].cardId >= 0) {
        if (own->battle[1].battle.layers[0].cardId < 0) {
            CGPMoveDSBATTLEwork(&own->battle[1].battle, &own->disposal[slot].battle);
            return 1;
        }
        if (own->battle[0].battle.layers[0].cardId < 0) {
            CGPMoveDSBATTLEwork(&own->battle[0].battle, &own->disposal[slot].battle);
            return 1;
        }
        if (YoakimuMoMoCheck(side, work) != 0) {
            goto cardChk;
        }
        if (GetDeckDataAdjust(side, work, own->battle[1].battle.layers[0].cardId) == 2 &&
            own->battle[1].battle.layers[0].life < own->disposal[slot].battle.layers[0].life) {
            CGPMoveDSBATTLEwork(&own->battle[1].battle, &own->disposal[slot].battle);
            return 1;
        }
        goto cardChk;
    }
    if (own->battle[2].battle.layers[0].cardId < 0) {
        goto cardChk;
    }
    if (own->battle[0].battle.layers[0].cardId < 0) {
        CGPMoveDSBATTLEwork(&own->battle[0].battle, &own->disposal[slot].battle);
        return 1;
    }
    if (own->battle[1].battle.layers[0].cardId < 0) {
        CGPMoveDSBATTLEwork(&own->battle[1].battle, &own->disposal[slot].battle);
        return 1;
    }
    if (YoakimuMoMoCheck(side, work) != 0) {
        goto cardChk;
    }
    if (GetDeckDataAdjust(side, work, own->battle[0].battle.layers[0].cardId) == 2 &&
        own->battle[0].battle.layers[0].life < own->disposal[slot].battle.layers[0].life) {
        CGPMoveDSBATTLEwork(&own->battle[0].battle, &own->disposal[slot].battle);
        return 1;
    }
    if (GetDeckDataAdjust(side, work, own->battle[1].battle.layers[0].cardId) == 2 &&
        own->battle[1].battle.layers[0].life < own->disposal[slot].battle.layers[0].life) {
        CGPMoveDSBATTLEwork(&own->battle[1].battle, &own->disposal[slot].battle);
        return 1;
    }

    do {
    cardChk:
        if (CardChkAttackType(work->definitions, &own->disposal[slot].battle) != CARD_ATTACK_HAND) {
            for (m = 3; m >= 0; m--) {
                if (own->battle[m].battle.layers[0].cardId < 0) {
                    CGPMoveDSBATTLEwork(&own->battle[m].battle, &own->disposal[slot].battle);
                    return 1;
                }
            }
            return 1;
        }
    } while (0);
    if (other->battle[0].battle.layers[0].cardId >= 0 &&
        own->battle[1].battle.layers[0].cardId < 0) {
        CGPMoveDSBATTLEwork(&own->battle[1].battle, &own->disposal[slot].battle);
        return 1;
    }
    if (other->battle[1].battle.layers[0].cardId >= 0 &&
        own->battle[0].battle.layers[0].cardId < 0) {
        CGPMoveDSBATTLEwork(&own->battle[0].battle, &own->disposal[slot].battle);
        return 1;
    }
    for (m = 0; m < CARD_BOARD_SLOTS; m++) {
        if (own->battle[m].battle.layers[0].cardId < 0) {
            CGPMoveDSBATTLEwork(&own->battle[m].battle, &own->disposal[slot].battle);
            return 1;
        }
    }
    return 1;

tagged:
    slot &= CARD_SLOT_INDEX_MASK;
    result = BattleAreaCheckFriend(side, work);
    dispFriend = DisposeAreaCheckFriend(side, work);
    if (result == 1 && dispFriend == 0) {
        CC_Kara_AttackMove(side, work, slot);
        return 1;
    }
    if ((u32) slot < 2 && own->battle[0].battle.layers[0].cardId >= 0 &&
        own->battle[1].battle.layers[0].cardId >= 0) {
        for (m = 0; m < CARD_BOARD_SLOTS; m++) {
            if (own->disposal[m].battle.layers[0].cardId >= 0 &&
                GetDeckDataAdjust(side, work, own->disposal[m].battle.layers[0].cardId) == 2 &&
                own->battle[slot].battle.layers[0].life < own->disposal[m].battle.layers[0].life) {
                CGPMoveDSBATTLEwork(&own->disposal[m].battle, &own->battle[slot].battle);
                return 1;
            }
        }
    }
    if ((other->battle[0].battle.layers[0].cardId >= 0 ||
         other->battle[1].battle.layers[0].cardId >= 0) &&
        YoakimuMoMoCheck(side, work) == 0) {
        CC_Kara_FrontTodomeMove(side, work, slot);
        return 1;
    }
    CC_Kara_KabeMove(side, work, slot);
    return 0;
}

/*
 * Type-three AI move: a move of the Momo cards (Lv 1, Swim Suit and Lv 10) first,
 * then the attack-type and escape checks, ending in the Sensi, Haibi-kabe or
 * kabe move for the chosen slot.
 */
s32 CC_Kara_MoveFase_Type3(s32 side, CardGameWork *work, s32 slot)
{
    CardPlaySide *ownSide;
    s32 cardId;

    if (side == 0) {
        ownSide = &work->player;
    } else {
        ownSide = &work->enemy;
    }

    if ((slot & CARD_SLOT_BATTLE) == 0) {
        cardId = ownSide->disposal[slot].battle.layers[0].cardId;
        if (cardId == CARD_ID_LV_1_MOMO || cardId == CARD_ID_SWIM_SUIT_MOMO || cardId == CARD_ID_LV_10_MOMO) {
            if (MoMoMove(side, work, slot) == 1) {
                return 1;
            }
        }
    } else {
        cardId = ownSide->battle[slot & CARD_SLOT_INDEX_MASK].battle.layers[0].cardId;
        if (cardId == CARD_ID_LV_1_MOMO || cardId == CARD_ID_SWIM_SUIT_MOMO || cardId == CARD_ID_LV_10_MOMO) {
            if (MoMoMove(side, work, (slot & CARD_SLOT_INDEX_MASK) | CARD_SLOT_BATTLE) == 1) {
                return 1;
            }
        }
    }
    if (CardPlayYamaCnt(ownSide) < 10 ||
        CardPlayYamaCnt(ownSide) < CheckEnemyAttack(side, work)) {
        s32 attackType;

        if ((slot & CARD_SLOT_BATTLE) == 0) {
            attackType = CardChkAttackType(work->definitions,
                                           &ownSide->disposal[slot].battle);
        } else {
            attackType = CardChkAttackType(work->definitions,
                                           &ownSide->battle[slot & CARD_SLOT_INDEX_MASK].battle);
        }
        if (attackType == CARD_ATTACK_HAND) {
            if ((slot & CARD_SLOT_BATTLE) == 0) {
                return CC_Kara_HaibiKabeMove(side, work, slot);
            }
            return CC_Kara_KabeMove(side, work, slot);
        }
    } else if (EscapeMove(side, work, slot) == 1) {
        return 1;
    }
    CC_Kara_SensiMove(side, work, slot);
    return 0;
}

/*
 * Type-four AI move: a special-card check, the attack and friend-count
 * gates, then the disposal-to-battle move into the first open battle slot
 * that qualifies, or the rear-attack move for a battle slot.
 */
s32 CC_Kara_MoveFase_Type4(s32 side, CardGameWork *work, s32 slot)
{
    CardPlaySide *ownSide = &work->player;
    s32 cardId2;
    s32 cardId3;
    s32 slotIndex;

    if (side != 0) {
        ownSide = &work->enemy;
    }
    if (YoakimuMove(side, work, slot) == 1) {
        return 1;
    }
    if (CardPlayYamaCnt(ownSide) < CheckEnemyAttack(side, work)) {
        return CC_Kara_MoveFase_Type3(side, work, slot);
    }
    if (CheckFriendType4(side, work) >= 4) {
        return CC_Kara_SensiMove(side, work, slot);
    }
    if ((slot & CARD_SLOT_BATTLE) == 0) {
        if (ownSide->battle[0].battle.layers[0].cardId >= 0) {
            cardId2 = ownSide->battle[2].battle.layers[0].cardId;
            if (cardId2 < 0 || GetDeckData(side, work, cardId2) < 4) {
                CGPMoveDSBATTLEwork(&ownSide->battle[2].battle,
                                    &ownSide->disposal[slot].battle);
                return 1;
            }
        }
        if (ownSide->battle[1].battle.layers[0].cardId >= 0) {
            cardId3 = ownSide->battle[3].battle.layers[0].cardId;
            if (cardId3 < 0 || GetDeckData(side, work, cardId3) < 4) {
                CGPMoveDSBATTLEwork(&ownSide->battle[3].battle,
                                    &ownSide->disposal[slot].battle);
                return 1;
            }
        }
        return 0;
    }
    slot &= CARD_SLOT_INDEX_MASK;
    if (slot < 2U) {
        for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
            if (ownSide->disposal[slotIndex].battle.layers[0].cardId < 0) {
                CGPMoveDSBATTLEwork(&ownSide->disposal[slotIndex].battle,
                                    &ownSide->battle[slot].battle);
                return 1;
            }
        }
    } else if (slot == 2 && ownSide->battle[0].battle.layers[0].cardId < 0) {
        for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
            if (ownSide->disposal[slotIndex].battle.layers[0].cardId < 0) {
                CGPMoveDSBATTLEwork(&ownSide->disposal[slotIndex].battle,
                                    &ownSide->battle[slot].battle);
                return 1;
            }
        }
    } else if (slot == 3 && ownSide->battle[1].battle.layers[0].cardId < 0) {
        for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
            if (ownSide->disposal[slotIndex].battle.layers[0].cardId < 0) {
                CGPMoveDSBATTLEwork(&ownSide->disposal[slotIndex].battle,
                                    &ownSide->battle[slot].battle);
                return 1;
            }
        }
    }
    if (ownSide->battle[0].battle.layers[0].cardId >= 0 &&
        ownSide->battle[1].battle.layers[0].cardId >= 0) {
        CC_Kara_BackAttackMove(side, work, slot);
        return 1;
    }
    return 0;
}

/*
 * Movement policy for the `side` side's board: the disposal slots first, then
 * the battle slots. A slot whose second stacked card is one of the three
 * command cards is skipped while the other side has fewer than two occupied
 * battle slots and the draw pile holds at least eleven cards.
 */
s32 CC_Kara_MoveFase_2(s32 side, CardGameWork *work)
{
    CardPlaySide *ownSide;
    s32 otherBattle;
    s32 otherDisposal;
    s32 i;
    s32 k;

    ownSide = &work->player;
    if (side != 0) {
        ownSide = &work->enemy;
    }
    i = 0;
    otherBattle = BattleAreaCheckEnemy(side, work);
    otherDisposal = DisposeAreaCheckEnemy(side, work);

    for (; i < CARD_BOARD_SLOTS; i++) {
        s32 slotOffset = i * (s32) sizeof(CardBoardSlot) + 2176;
        CardBoardSlot *slot = (CardBoardSlot *) ((u8 *) ownSide + slotOffset);

        if (slot->battle.layers[0].cardId >= 0) {
            if (otherBattle < 2 && CardPlayYamaCnt(ownSide) >= 11) {
                s32 layer1Id = ownSide->disposal[i].battle.layers[1].cardId;

                if (CardLayerPower(&slot->battle) < 2) {
                    if (layer1Id == CARD_ID_EYEGLASSES) {
                        continue;
                    }
                    if (layer1Id == CARD_ID_MISSILE_POD) {
                        continue;
                    }
                    if (layer1Id == CARD_ID_BEAM) {
                        continue;
                    }
                }
            }
            if (otherBattle + otherDisposal == 0) {
                CC_Kara_MoveFase_Type3(side, work, i);
            } else {
                u32 deckKind = GetDeckData(side, work, ownSide->disposal[i].battle.layers[0].cardId);

                switch (deckKind) {
                case 0:
                    CC_Kara_MoveFase_Type0(side, work, i);
                    break;
                case 1:
                    CC_Kara_MoveFase_Type1(side, work, i);
                    break;
                case 2:
                    CC_Kara_MoveFase_Type2(side, work, i);
                    break;
                case 3:
                    CC_Kara_MoveFase_Type3(side, work, i);
                    break;
                case 4:
                    CC_Kara_MoveFase_Type4(side, work, i);
                    break;
                }
            }
        }
    }

    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        s32 slotOffset = i * (s32) sizeof(CardBoardSlot) + 400;
        CardBoardSlot *slot = (CardBoardSlot *) ((u8 *) ownSide + slotOffset);

        if (slot->battle.layers[0].cardId >= 0) {
            if (otherBattle + otherDisposal == 0) {
                CC_Kara_MoveFase_Type3(side, work, i | CARD_SLOT_BATTLE);
            } else {
                u32 deckKind;

                if (otherBattle < 2 && CardPlayYamaCnt(ownSide) >= 11) {
                    s32 layer1Id = ownSide->battle[i].battle.layers[1].cardId;

                    if (CardLayerPower(&slot->battle) < 2 &&
                        (layer1Id == CARD_ID_EYEGLASSES || layer1Id == CARD_ID_MISSILE_POD || layer1Id == CARD_ID_BEAM)) {
                        for (k = 0; k < CARD_BOARD_SLOTS; k++) {
                            if (ownSide->disposal[k].battle.layers[0].cardId < 0) {
                                CGPMoveDSBATTLEwork(&ownSide->disposal[k].battle,
                                                    &ownSide->battle[i].battle);
                            }
                        }
                    }
                }
                deckKind = GetDeckData(side, work, ownSide->battle[i].battle.layers[0].cardId);
                switch (deckKind) {
                case 0:
                    CC_Kara_MoveFase_Type0(side, work, i | CARD_SLOT_BATTLE);
                    break;
                case 1:
                    CC_Kara_MoveFase_Type1(side, work, i | CARD_SLOT_BATTLE);
                    break;
                case 2:
                    CC_Kara_MoveFase_Type2(side, work, i | CARD_SLOT_BATTLE);
                    break;
                case 3:
                    CC_Kara_MoveFase_Type3(side, work, i | CARD_SLOT_BATTLE);
                    break;
                case 4:
                    CC_Kara_MoveFase_Type4(side, work, i | CARD_SLOT_BATTLE);
                    break;
                }
            }
        }
    }
    return 0;
}

/*
 * Moves disposal stacks into open battle slots. First every open,
 * unflagged battle stack takes the empty disposal stacks; then each
 * occupied disposal stack moves into the battle slot its attack type picks.
 */
s32 CC_Kara_MoveFase_1(s32 side, CardGameWork *work)
{
    CardPlaySide *ownSide;
    CardPlaySide *otherSide;
    s32 slotIndex;
    s32 disposalIndex;

    if (side == 0) {
        ownSide = &work->player;
        otherSide = &work->enemy;
    } else {
        ownSide = &work->enemy;
        otherSide = &work->player;
    }

    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        if (ownSide->battle[slotIndex].battle.layers[0].cardId >= 0) {
            if (ownSide->battle[slotIndex].battle.flags & CARD_STACK_FLAG_FROZEN) {
                for (disposalIndex = 0; disposalIndex < CARD_BOARD_SLOTS; disposalIndex++) {
                    if (ownSide->disposal[disposalIndex].battle.layers[0].cardId < 0) {
                        CGPMoveDSBATTLEwork(&ownSide->disposal[disposalIndex].battle,
                                            &ownSide->battle[slotIndex].battle);
                    }
                }
            }
        }
    }

    for (slotIndex = 0; slotIndex < CARD_BOARD_SLOTS; slotIndex++) {
        if (ownSide->disposal[slotIndex].battle.layers[0].cardId >= 0 && !(ownSide->disposal[slotIndex].battle.flags & CARD_STACK_FLAG_FROZEN)) {
            if (CardChkAttackType(work->definitions, &ownSide->disposal[slotIndex].battle) == CARD_ATTACK_HAND) {
                if (ownSide->battle[0].battle.layers[0].cardId < 0) {
                    if (ownSide->battle[1].battle.layers[0].cardId < 0 &&
                        otherSide->battle[0].battle.layers[0].cardId >= 0 &&
                        otherSide->battle[1].battle.layers[0].cardId < 0) {
                        CGPMoveDSBATTLEwork(&ownSide->battle[1].battle, &ownSide->disposal[slotIndex].battle);
                    } else {
                        CGPMoveDSBATTLEwork(&ownSide->battle[0].battle, &ownSide->disposal[slotIndex].battle);
                    }
                    return 1;
                } else if (ownSide->battle[1].battle.layers[0].cardId < 0) {
                    CGPMoveDSBATTLEwork(&ownSide->battle[1].battle, &ownSide->disposal[slotIndex].battle);
                    return 1;
                }
            } else if (ownSide->battle[2].battle.layers[0].cardId < 0) {
                CGPMoveDSBATTLEwork(&ownSide->battle[2].battle, &ownSide->disposal[slotIndex].battle);
                return 1;
            } else if (ownSide->battle[3].battle.layers[0].cardId < 0) {
                CGPMoveDSBATTLEwork(&ownSide->battle[3].battle, &ownSide->disposal[slotIndex].battle);
                return 1;
            } else if (ownSide->battle[0].battle.layers[0].cardId < 0) {
                CGPMoveDSBATTLEwork(&ownSide->battle[0].battle, &ownSide->disposal[slotIndex].battle);
                return 1;
            } else if (ownSide->battle[1].battle.layers[0].cardId < 0) {
                CGPMoveDSBATTLEwork(&ownSide->battle[1].battle, &ownSide->disposal[slotIndex].battle);
                return 1;
            }
        }
    }
    return 0;
}

s32 CheckComboMove(s32 side, CardGameWork *work)
{
    CardPlaySide *own;
    CardPlaySide *other;
    s32 handCount;
    CardDefinition *definitions;
    s32 friendSum;
    s32 attack0[CARD_BOARD_SLOTS];
    s32 attack16[CARD_BOARD_SLOTS];
    s32 i;
    s32 k;
    s32 m;
    s32 slotId;
    s32 dispId;
    s32 cardId;
    s32 yama;
    s32 ownYamaCount;
    s32 pick;
    s32 pick2;
    s32 virgilOnBoard;
    s32 n;
    s32 count;
    s32 virgilInBattle;

    if (side == 0) {
        own = &work->player;
        other = &work->enemy;
    } else {
        own = &work->enemy;
        other = &work->player;
    }
    play413 = 0;
    playteppeki = 0;
    definitions = work->definitions;
    handCount = CardPlayHandCnt(&own->hand);
    friendSum = BattleAreaCheckFriend(side, work);
    friendSum += DisposeAreaCheckFriend(side, work);
    if (friendSum == 0) {
        return 0;
    }
    GetFriendAttack(side, work, attack0, attack16);
    if (TodomeHyouka(side, work, 0, 0, attack0, attack16) >= TODOME_DECK_OUT_SCORE) {
        return 0;
    }

    for (i = 0; i < handCount; i++) {
        if (own->hand.cards[i] == CARD_ID_PHASE_CANNON && CheckCommandPlay(side, work, CARD_ID_PHASE_CANNON) == 1 &&
            BattleAreaCheckEnemy(side, work) > 0) {
            for (k = 0; k < CARD_BOARD_SLOTS; k++) {
                cardId = own->battle[k].battle.layers[0].cardId;
                if (cardId >= 0 && (definitions[cardId].flags == CARD_ATTR_GNOSIS || definitions[cardId].flags == CARD_ATTR_MECH)) {
                    for (m = 0; m < CARD_BOARD_SLOTS; m++) {
                        dispId = own->disposal[m].battle.layers[0].cardId;
                        if (dispId < 0) {
                            CGPMoveDSBATTLEwork(&own->disposal[m].battle,
                                                &own->battle[k].battle);
                            break;
                        }
                        if (definitions[dispId].flags == CARD_ATTR_GNOSIS || definitions[dispId].flags == CARD_ATTR_MECH) {
                            continue;
                        }
                        CGPMoveDSBATTLEwork(&own->disposal[m].battle,
                                            &own->battle[k].battle);
                        break;
                    }
                }
            }
            if (CheckSouteniHouCommand(side, work) == 1) {
                return 1;
            }
        }
    }

    ownYamaCount = CardPlayYamaCnt(own);
    pick = 0;
    pick2 = 0;
    virgilOnBoard = 0;
    for (i = 0; i < handCount; i++) {
        if (ownYamaCount >= 2) {
                if (own->hand.cards[i] == CARD_ID_ARTICLE_4_13 && CheckCommandPlay(side, work, CARD_ID_ARTICLE_4_13) == 1) {
                    pick = 1;
                }
                if (own->hand.cards[i] == CARD_ID_ARMISTICE && CGPDispoSub0(side, work, i) == 0) {
                    pick2 = 1;
                }
            }
        }

    if (pick == 1 && pick2 == 1) {
        n = 0;
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            cardId = own->disposal[i].battle.layers[0].cardId;
            if (cardId >= 0 && definitions[cardId].flags == CARD_ATTR_REALIAN) {
                for (m = 0; m < CARD_BOARD_SLOTS; m++) {
                    if (own->battle[m].battle.layers[0].cardId < 0) {
                        CGPMoveDSBATTLEwork(&own->battle[m].battle,
                                            &own->disposal[i].battle);
                        n++;
                        break;
                    }
                    if (definitions[own->battle[m].battle.layers[0].cardId].flags != CARD_ATTR_REALIAN) {
                        CGPMoveDSBATTLEwork(&own->battle[m].battle,
                                            &own->disposal[i].battle);
                        n++;
                        break;
                    }
                }
            }
        }
        if (n >= 2) {
            return 1;
        }
        if (n == 1 && CardPlayYamaCnt(other) < 13) {
            return 1;
        }
    }

    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        s32 slotCard;

        if (own->disposal[i].battle.layers[0].cardId == CARD_ID_VIRGIL) {
            virgilOnBoard = 1;
            break;
        }
        slotCard = own->battle[i].battle.layers[0].cardId;
        if (slotCard == CARD_ID_VIRGIL) {
            virgilOnBoard = 1;
            break;
        }
    }
    if (pick2 == 1 && virgilOnBoard == 1) {
        virgilInBattle = 0;
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            cardId = own->disposal[i].battle.layers[0].cardId;
            if (cardId >= 0 && definitions[cardId].flags == CARD_ATTR_REALIAN) {
                for (m = 0; m < CARD_BOARD_SLOTS; m++) {
                    if (own->battle[m].battle.layers[0].cardId < 0) {
                        CGPMoveDSBATTLEwork(&own->battle[m].battle,
                                            &own->disposal[i].battle);
                        break;
                    }
                    if (definitions[own->battle[m].battle.layers[0].cardId].flags != CARD_ATTR_REALIAN) {
                        CGPMoveDSBATTLEwork(&own->battle[m].battle,
                                            &own->disposal[i].battle);
                        break;
                    }
                }
            }
        }

        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (own->disposal[i].battle.layers[0].cardId == CARD_ID_VIRGIL) {
                for (m = 0; m < CARD_BOARD_SLOTS; m++) {
                    cardId = own->battle[m].battle.layers[0].cardId;
                    if (cardId < 0) {
                        CGPMoveDSBATTLEwork(&own->battle[m].battle,
                                            &own->disposal[i].battle);
                        break;
                    }
                    if (definitions[cardId].flags != CARD_ATTR_REALIAN) {
                        CGPMoveDSBATTLEwork(&own->battle[m].battle,
                                            &own->disposal[i].battle);
                        break;
                    }
                    if (m == 3) {
                        CGPMoveDSBATTLEwork(&own->battle[3].battle,
                                            &own->disposal[i].battle);
                        break;
                    }
                }
                break;
            }
        }

        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (own->battle[i].battle.layers[0].cardId == CARD_ID_VIRGIL) {
                virgilInBattle = 1;
                break;
            }
        }

        count = 0;
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (own->battle[i].battle.layers[0].cardId >= 0 &&
                definitions[own->battle[i].battle.layers[0].cardId].flags == CARD_ATTR_REALIAN) {
                count++;
            }
        }
        if (count > 0 && virgilInBattle == 1) {
            return 1;
        }
    }
    return 0;
}

s32 CC_Kara_MoveFase(s32 side, CardGameWork *work)
{
    CardPlaySide *player;
    CardPlaySide *enemy;
    CardPlaySide *ownSide;
    CardPlaySide *otherSide;
    s32 i;

    if (side == 0) {
        ownSide = &work->player;
        otherSide = &work->enemy;
        player = &work->player;
        enemy = &work->enemy;
    } else {
        ownSide = &work->enemy;
        otherSide = &work->player;
        enemy = &work->enemy;
        player = &work->player;
    }
    if (CheckComboMove(side, work) == 1) {
        return 0;
    }
    if (CardPlayCntOpe(player, CARD_ID_DSSS) != 0 || CardPlayCntOpe(enemy, CARD_ID_DSSS) != 0) {
        if (CheckGuno(side, work) != 0) {
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                s32 k;

                if (ownSide->disposal[i].battle.layers[0].cardId < 0) {
                    continue;
                }
                if (!(work->definitions[ownSide->disposal[i].battle.layers[0].cardId].flags & CARD_ATTR_GNOSIS)) {
                    continue;
                }
                for (k = 0; k < CARD_BOARD_SLOTS; k++) {
                    if (ownSide->battle[k].battle.layers[0].cardId < 0) {
                        CGPMoveDSBATTLEwork(&ownSide->battle[k].battle,
                                            &ownSide->disposal[i].battle);
                        break;
                    }
                }
            }
            return 0;
        }
    }
    if (CardPlayCntOpe(player, CARD_ID_COMPULSORY_SORTIE) != 0 || CardPlayCntOpe(enemy, CARD_ID_COMPULSORY_SORTIE) != 0) {
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            s32 k;

            if (ownSide->disposal[i].battle.layers[0].cardId >= 0) {
                for (k = 0; k < CARD_BOARD_SLOTS; k++) {
                    if (ownSide->battle[k].battle.layers[0].cardId < 0) {
                        CGPMoveDSBATTLEwork(&ownSide->battle[k].battle,
                                            &ownSide->disposal[i].battle);
                        break;
                    }
                }
            }
        }
        return 0;
    }
    if (CardPlayCntOpe(player, CARD_ID_OVERRIDE_CODE) != 0 || CardPlayCntOpe(enemy, CARD_ID_OVERRIDE_CODE) != 0) {
        if (CheckREAL(side, work) != 0) {
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                s32 k;

                if (ownSide->disposal[i].battle.layers[0].cardId >= 0 &&
                    (work->definitions[ownSide->disposal[i].battle.layers[0].cardId].flags & CARD_ATTR_REALIAN)) {
                    for (k = 0; k < CARD_BOARD_SLOTS; k++) {
                        if (ownSide->battle[k].battle.layers[0].cardId < 0) {
                            CGPMoveDSBATTLEwork(&ownSide->battle[k].battle,
                                                &ownSide->disposal[i].battle);
                            break;
                        }
                    }
                }
            }
            return 0;
        }
    }
    if (CardPlayCntOpe(player, CARD_ID_HILBERT_EFFECT) != 0 || CardPlayCntOpe(enemy, CARD_ID_HILBERT_EFFECT) != 0) {
        if (CheckGuno(side, work) != 0) {
            return 0;
        }
    }
    if (CardPlayCntOpe(player, CARD_ID_A_RAT_IN_A_TRAP) != 0 || CardPlayCntOpe(enemy, CARD_ID_A_RAT_IN_A_TRAP) != 0) {
        return 0;
    }
    if (CheckGuno(side, work) != 0) {
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (ownSide->battle[i].battle.layers[0].cardId == CARD_ID_LV_10_MOMO ||
                otherSide->battle[i].battle.layers[0].cardId == CARD_ID_LV_10_MOMO) {
                s32 j;

                for (j = 0; j < CARD_BOARD_SLOTS; j++) {
                    s32 k;

                    if (ownSide->disposal[j].battle.layers[0].cardId >= 0) {
                        for (k = 0; k < CARD_BOARD_SLOTS; k++) {
                            if (ownSide->battle[k].battle.layers[0].cardId < 0) {
                                CGPMoveDSBATTLEwork(&ownSide->battle[k].battle,
                                                    &ownSide->disposal[j].battle);
                                break;
                            }
                        }
                    }
                }
                return 0;
            }
        }
    }
    if (GainanCheck(side, work) == 1) {
        return 0;
    }
    for (i = 9; i >= 0; i--) {
        CC_Kara_MoveFase_2(side, work);
    }
    return 0;
}

/*
 * Whether the side's hand holds Miyuki's Email or Song, whose play condition
 * CardPlayChkCondition accepts (it returns 0 for an allowed card).
 */
s32 CC_Kara_1stAnswer(s32 side, CardGameWork *work)
{
    CardPlaySide *ownSide = &work->enemy;
    CardHand *hand;
    s32 handCount;
    s32 cardIndex;

    if (side == 0) {
        ownSide = &work->player;
        hand = &ownSide->hand;
    } else {
        hand = &ownSide->hand;
    }
    handCount = CardPlayHandCnt(hand);
    cardIndex = 0;
    if (handCount > 0) {
        do {
            s16 cardId = hand->cards[cardIndex];

            if (cardId == CARD_ID_MIYUKIS_EMAIL || cardId == CARD_ID_SONG) {
                if (CardPlayChkCondition(work->definitions, ownSide, cardId, work) == 0) {
                    return 1;
                }
            }
            cardIndex++;
        } while (cardIndex < handCount);
    }
    return 0;
}

/*
 * Deserts the first card of the side's hand whose GetDeckData value is zero
 * or one (the search stops there); with no such card, the first hand slot is
 * deserted. Always returns 1.
 */
s32 CC_Kara_EndFase(s32 side, CardGameWork *work)
{
    s32 cardIndex;
    CardPlaySide *ownSide = &work->enemy;
    s32 handCount;

    if (side == 0) {
        ownSide = &work->player;
        cardIndex = 0;
    } else {
        cardIndex = 0;
    }
    cardIndex = 0;
    handCount = CardPlayHandCnt(&ownSide->hand);
    if (0 < handCount) {
        do {
            s16 cardId = ownSide->hand.cards[cardIndex];

            if (GetDeckData(side, work, cardId) == 0) {
                CardPlayDesertCard(ownSide, cardIndex);
                return 1;
            }
            if (GetDeckData(side, work, cardId) == 1) {
                CardPlayDesertCard(ownSide, cardIndex);
                return 1;
            }
            cardIndex++;
        } while (cardIndex < handCount);
    }
    CardPlayDesertCard(ownSide, 0);
    return 1;
}
