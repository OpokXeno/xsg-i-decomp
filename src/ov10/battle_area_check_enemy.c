/*
 * OV10 original TU 10: 0x00a34190..0x00a42034 (89 functions)
 */
#include "common.h"
#include "ov10/cgp.h"

typedef signed int s32;

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", BattleAreaCheckEnemy);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", DisposeAreaCheckEnemy);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", BattleAreaCheckFriend);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", DisposeAreaCheckFriend);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckFriendType4);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckFriendKakutouCnt);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", AlbedoCheck);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", GainanCheck);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckGuno);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckREAL);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckNowPower);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckEnemyAttack);

extern s32 CardChkAttackType(CardDefinition *definitions, CardLayerStack *card);
extern s32 CardPlayCntPower(CardGameWork *work, CardLayerStack *card);
extern s32 CGPCalcPowerEnv(s32 isOwnSide, CardGameWork *work, s32 count, s16 cardId);

/*
 * CardLayerStack.power (ov10/cgp.h) reads unsigned everywhere it is stored so
 * far, but CheckEnemy12Attack and CheckEnemyFight both compare it with `lb`
 * (0x00a35168, 0x00a35204, 0x00a3530c, 0x00a35384): it is signed here.
 */
static inline s8 CardLayerPower(CardLayerStack *card) {
    return *(s8 *) &card->power;
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
    for (slotIndex = 0; slotIndex < 4; slotIndex++) {
        CardLayerStack *card = &otherSide->disposal[slotIndex].battle;

        if (card->layers[0].cardId >= 0) {
            s32 attackType = CardChkAttackType(work->definitions, card);

            if (CardLayerPower(card) == 1 && (u32) (attackType - 2) < 2) {
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
    for (slotIndex = 0; slotIndex < 4; slotIndex++) {
        CardLayerStack *card = &otherSide->battle[slotIndex].battle;

        if (card->layers[0].cardId >= 0) {
            s32 attackType = CardChkAttackType(work->definitions, card);

            if (CardLayerPower(card) < 2 && attackType == 2) {
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

    for (slotIndex = 0; slotIndex < 4; slotIndex++) {
        CardLayerStack *card = &otherSide->battle[slotIndex].battle;

        if (card->layers[0].cardId >= 0) {
            s32 attackType = CardChkAttackType(work->definitions, card);

            if (CardLayerPower(card) < 2 && attackType == 1) {
                power = CGPCalcPowerEnv(side == 0, work,
                                         CardPlayCntPower(work, card),
                                         card->layers[0].cardId);
                if (power > best) {
                    best = power;
                }
            }
        }
    }
    for (slotIndex = 0; slotIndex < 4; slotIndex++) {
        CardLayerStack *card = &otherSide->disposal[slotIndex].battle;

        if (card->layers[0].cardId >= 0) {
            s32 attackType = CardChkAttackType(work->definitions, card);

            if (CardLayerPower(card) < 2 && attackType == 1) {
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

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckGeneration);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", GetDeckData);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", GetDeckDataAdjust);

extern s32 CardPlayYamaCnt(CardPlaySide *side);
extern s32 CardPlayChkCondition(CardDefinition *definitions, CardPlaySide *side, s32 cardId, CardGameWork *work);
extern s32 CardPlayCommandCondition(u16 side, u16 cardId, CardGameWork *work);

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

extern s32 CardPlayHandCnt(CardHand *hand);
extern s32 CheckCommandPlay(s32 side, CardGameWork *work, s32 cardId);

/* Card ids 0x53/0x75: the two Down/Kaifuku recovery command cards. */
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

        if (cardId == 0x53 || cardId == 0x75) {
            s32 result = CheckCommandPlay(side, work, cardId);
            if (result == 1) {
                return result;
            }
        }
        cardIndex++;
    } while (cardIndex < handCount);
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckEnemyGuno);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CC_Kara_SetOpe);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckTamasii);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CC_Kara_SetWeapon);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CC_Kara_SetLv10);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", SetGunoFinish);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CC_Kara_SetGuno);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CC_Kara_SetFase_1);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CC_Kara_CommFase);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckDamageCommand);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckTaikyakuCommand);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckTotugekiCommand);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckKonranCommand);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckCurryCommand);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckTentCommand);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckSouteniHouCommand);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckHatudouCommand);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckXDamageCommand);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckMilCommand);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckNazoCommand);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckSeimitsuCommand);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckGunoYokanCommand);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckExtCommand);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckDownGunoCommand);

s32 GetPrice(CardLayerStack *stack);

/* Highest-price category-one card among the flagged slots of the other side's
 * disposal board, encoded as a disposal slot index (-1 if none qualifies). */
s32 CheckChudanCommand(s32 side, CardGameWork *work)
{
    CardPlaySide *otherSide = (side == 0) ? &work->enemy : &work->player;
    s32 bestPrice = 0;
    s32 bestSlot = -1;
    s32 slotIndex;

    for (slotIndex = 0; slotIndex < 4; slotIndex++) {
        s16 cardId = otherSide->disposal[slotIndex].battle.layers[0].cardId;

        if (cardId >= 0 && work->definitions[cardId].flags == 1 &&
            (otherSide->disposal[slotIndex].battle.flags & 1)) {
            s32 price = GetPrice(&otherSide->disposal[slotIndex].battle);
            if (bestPrice < price) {
                bestPrice = price;
                bestSlot = slotIndex;
            }
        }
    }
    return bestSlot;
}

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", Check413Command);

s32 GetPrice(CardLayerStack *stack);

/* Highest-price flagged card on the other side's disposal board, encoded as
 * a disposal slot index (-1 if none qualifies). */
s32 CheckKyoukouCommand(s32 side, CardGameWork *work)
{
    CardPlaySide *otherSide = (side == 0) ? &work->enemy : &work->player;
    s32 bestPrice = 0;
    s32 bestSlot = -1;
    s32 slotIndex;

    for (slotIndex = 0; slotIndex < 4; slotIndex++) {
        if (otherSide->disposal[slotIndex].battle.layers[0].cardId >= 0 &&
            (otherSide->disposal[slotIndex].battle.flags & 2)) {
            s32 price = GetPrice(&otherSide->disposal[slotIndex].battle);
            if (bestPrice < price) {
                bestPrice = price;
                bestSlot = slotIndex;
            }
        }
    }
    return bestSlot;
}

s32 GetShoukin(CardLayerStack *stack);

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
    for (slotIndex = 0; slotIndex < 4; slotIndex++) {
        s16 *cardIdSlot = &ownSide->battle[slotIndex].battle.layers[0].cardId;
        s16 cardId = *cardIdSlot;

        if (cardId >= 0 && work->definitions[cardId].flags == 1) {
            s32 reward = GetShoukin(&opponent->battle[slotIndex].battle);
            if (reward < bestReward) {
                bestReward = reward;
                bestSlot = slotIndex | 0xF0000;
            }
        }
    }
    return bestSlot;
}

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckBujinCommand);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckTuukokuCommand);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckKetugiCommand);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckTeppekiCommand);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckSensiCommand);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", SensiEvent);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CC_Kara_DisposeEvent);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CC_Kara_CommRequest);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CC_Kara_CommPlay);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CC_Kara_SetFase);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", GetShoukin);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", GetPrice);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", TodomeHyouka);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", GetFriendAttack);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CC_Kara_TodomeMove);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CC_Kara_AttackMove);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CC_Kara_BackTodomeMove);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CC_Kara_FrontTodomeMove);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CC_Kara_BackAttackMove);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", VX10000Check);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", YoakimuMoMoCheck);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", MoMoCheck);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", YoakimuMove);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", MoMoMove);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckFriendKabe);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckFriendSensi);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CC_Kara_SensiMove);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CC_Kara_HaibiKabeMove);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CC_Kara_KabeMove);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", EscapeMove);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", LvUpCheck);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CC_Kara_MoveFase_Type0);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CC_Kara_MoveFase_Type1);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CC_Kara_MoveFase_Type2);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CC_Kara_MoveFase_Type3);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CC_Kara_MoveFase_Type4);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CC_Kara_MoveFase_2);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CC_Kara_MoveFase_1);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CheckComboMove);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CC_Kara_MoveFase);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CC_Kara_1stAnswer);

INCLUDE_ASM("asm/nonmatchings/ov10/battle_area_check_enemy", CC_Kara_EndFase);
