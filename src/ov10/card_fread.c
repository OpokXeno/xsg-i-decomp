/*
 * OV10 original TU 2: 0x00a08670..0x00a12a90 (101 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov10/cgp.h"

/* The card-model fade state in CardGameWork (+0x7100..+0x71A0, ov10/cgp.h):
 * four model slots, of whose four color channels the fades touch 0..2. */
#define CARD_MODEL_SLOT_COUNT 4
#define CARD_MODEL_COLOR_CHANNELS 3

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardFread);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardModelFadeOut);

void CardModelFadeIn(CardGameWork *work)
{
    int slot;
    int channel;

    if (work->modelFadeInTimer == 0) {
        return;
    }
    work->modelFadeInTimer -= 1;
    for (slot = 0; slot < CARD_MODEL_SLOT_COUNT; slot++) {
        for (channel = 0; channel < CARD_MODEL_COLOR_CHANNELS; channel++) {
            work->modelColor[slot][channel] += work->modelFadeStep;
            if (work->modelColorTarget[slot][channel] < work->modelColor[slot][channel]) {
                work->modelColor[slot][channel] = work->modelColorTarget[slot][channel];
            }
        }
    }
}

void CardFadeOut(CardGameWork *work)
{
    int slot;
    int level;

    if (work->fadeOutTimer == 0) {
        return;
    }
    work->fadeOutTimer -= 1;
    for (slot = 0; slot < CARD_MODEL_SLOT_COUNT; slot++) {
        level = work->fadeOutColor[slot] - work->fadeOutStep;
        work->fadeOutColor[slot] = (level < 0) ? 0 : level;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardFadeIn);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardMakeDispList);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardCopyDeckWork);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardGetDeckAllCnt);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardGetDeckCardCnt);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardCleanUpDeck);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardAddDeck);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardDelDeck);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardNameConvert);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardDeckDisp);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CDTSGetStrLen);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardDispTxtSub);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardDispTxtSub2);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardDispLight);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardDispCard);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardDisp);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardSampleInitDeck0);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardSampleInitDeck1);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardSampleInitDeck2);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardSampleInitDeck3);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardSampleInitDeck4);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardSampleInitDeck5);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardSampleInitDeck6);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardSampleInitDeck7);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardSampleInitDeck8);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardSampleInitDeck9);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardCopyRom2Deck);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardCopyRam2Deck);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardCopyRam2CLD);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardCopyCLD2Ram);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardChkDeckRam);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardCntList);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardAddList);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayInitWork);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CPIWsub);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayNoInitWork);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayInitWorkFake);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayShuffleYama);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayDrawCard);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayDrawSute);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayRecavery);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayCleanHand);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayCleanYama);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayCleanJunk);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayDesertCard);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayDesert2Card);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayDesertOperation);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayDesertBattle);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayCostCard);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayReturnCard);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayYamaCnt);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlaySuteCnt);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayJunkCnt);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayOperationCnt);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayDisposeCnt);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayHandCnt);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayBattleCnt);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayTypeSort);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayChkCost);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardCountGeneration);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayChkCondition);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardChkAttackType);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CPCMSub);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayCursorMove);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardListDisp);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardDeckListDisp);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardStrListDisp);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardEnemyListDisp);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayCntPower);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayDispBattle);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayDispHand);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayDispJunk);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayDispDispose);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayDispOperation);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayDispYama);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayDispSute);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardCursorPassive);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardCursorActive);

/* Maps a cursor-area code to a fixed cursor model resource address;
 * codes other than 2, 3 and 4 (including 0 and 1) share the same address. */
int CAMASub(int areaCode) {
    int code;
    int addr;

    code = areaCode & 0xFF;
    addr = 0x01D0C000;
    if ((code != 2) &&
        ((code < 3) ||
         (((addr = 0x01D10000), (code != 3)) &&
          ((addr = 0x01D12000), (code != 4))))) {
        addr = 0x01D08000;
    }
    return addr;
}

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayDispCursor);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayIdentityCheck);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CGCPHandDispSubCore);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CGCPNeedDispSub);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CGCPGeneDispSub);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CGCPHandDispSub3);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CGCPHandDispSub2);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CGCPHandDispSub);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CGCPYamaDispSub);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CGCPJunkDispSub);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayCntAllOpe);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayCntOpe);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayCntDsp);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayCntBtl);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayCntLight);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayCntHeavy);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardEquipCntHeavy);

/* still INCLUDE_ASM above in this TU; CardHand * matches CardPlayHandCnt's
 * argument (ov10/cgp.h), the same playerHand/enemyHand records this
 * function indexes. */
extern int CardPlayCntDsp(CardHand *hand, s16 cardId);
extern int CardPlayCntBtl(CardHand *hand, s16 cardId);

int CardPlayCntFld(CardGameWork *work, s16 cardId) {
    CardHand *playerHand;
    CardHand *enemyHand;
    int count;

    playerHand = &work->playerHand;
    enemyHand = &work->enemyHand;
    count = CardPlayCntDsp(playerHand, cardId);
    count = count + CardPlayCntBtl(playerHand, cardId);
    count = count + CardPlayCntDsp(enemyHand, cardId);
    return count + CardPlayCntBtl(enemyHand, cardId);
}
