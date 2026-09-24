/*
 * OV10 original TU 2: 0x00a08670..0x00a12a90 (101 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov10/cgp.h"
#include "card_fread.h"

/* The card-model fade state in CardGameWork (+0x7100..+0x71A0, ov10/cgp.h):
 * four model slots, of whose four color channels the fades touch 0..2. */
#define CARD_MODEL_SLOT_COUNT 4
#define CARD_MODEL_COLOR_CHANNELS 3

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardFread);

/*
 * Mirrors CardModelFadeIn's timer/step/color fields (same offsets in
 * CardGameWork, ov10/cgp.h) but subtracts modelFadeStep and clamps each
 * channel at zero instead of raising it toward modelColorTarget.
 */
void CardModelFadeOut(CardGameWork *work)
{
    int slot;
    int channel;
    float color;

    if (work->modelFadeInTimer == 0) {
        return;
    }
    work->modelFadeInTimer -= 1;
    for (slot = 0; slot < CARD_MODEL_SLOT_COUNT; slot++) {
        for (channel = 0; channel < CARD_MODEL_COLOR_CHANNELS; channel++) {
            color = work->modelColor[slot][channel] - work->modelFadeStep;
            work->modelColor[slot][channel] = color;
            if (color < 0.0f) {
                work->modelColor[slot][channel] = 0.0f;
            }
        }
    }
}

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

typedef int s32;

/* Compact deck-build list CardMakeDeckProc/CardLoadDeckProc/CardSaveDeckProc
 * edit: a card-type group count followed by one 4-byte group record per
 * group, whose selected-copy count is its first halfword. */
typedef struct CardDeckGroupEntry {
    s16 copyCount;              /* +0x00 */
    u8 unmodeled_02[2];
} CardDeckGroupEntry;

typedef struct CardDeckGroupList {
    u8 unmodeled_00[0x12];
    s16 groupCount;             /* +0x12 */
    u8 unmodeled_14[2];
    CardDeckGroupEntry groups[1]; /* +0x16: groupCount entries */
} CardDeckGroupList;

s32 CardGetDeckAllCnt(CardDeckGroupList *deck)
{
    s32 total;
    int i;

    total = 0;
    for (i = 0; i < deck->groupCount; i++) {
        total += deck->groups[i].copyCount;
    }
    return total;
}

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

extern unsigned char SampleDeck0[];

void CardSampleInitDeck0(s16 *deck)
{
    int i;

    for (i = 0; i < 40; i++) {
        deck[i] = SampleDeck0[i];
    }
}

extern unsigned char SampleDeck1[];

void CardSampleInitDeck1(s16 *deck)
{
    int i;

    for (i = 0; i < 40; i++) {
        deck[i] = SampleDeck1[i];
    }
}

extern unsigned char SampleDeck2[];

void CardSampleInitDeck2(s16 *deck)
{
    int i;

    for (i = 0; i < 40; i++) {
        deck[i] = SampleDeck2[i];
    }
}

extern unsigned char SampleDeck3[];

void CardSampleInitDeck3(s16 *deck)
{
    int i;

    for (i = 0; i < 40; i++) {
        deck[i] = SampleDeck3[i];
    }
}

extern unsigned char SampleDeck4[];

void CardSampleInitDeck4(s16 *deck)
{
    int i;

    for (i = 0; i < 40; i++) {
        deck[i] = SampleDeck4[i];
    }
}

extern unsigned char SampleDeck5[];

void CardSampleInitDeck5(s16 *deck)
{
    int i;

    for (i = 0; i < 40; i++) {
        deck[i] = SampleDeck5[i];
    }
}

extern unsigned char SampleDeck6[];

void CardSampleInitDeck6(s16 *deck)
{
    int i;

    for (i = 0; i < 40; i++) {
        deck[i] = SampleDeck6[i];
    }
}

extern unsigned char SampleDeck7[];

void CardSampleInitDeck7(s16 *deck)
{
    int i;

    for (i = 0; i < 40; i++) {
        deck[i] = SampleDeck7[i];
    }
}

extern unsigned char SampleDeck8[];

void CardSampleInitDeck8(s16 *deck)
{
    int i;

    for (i = 0; i < 40; i++) {
        deck[i] = SampleDeck8[i];
    }
}

extern unsigned char SampleDeck9[];

void CardSampleInitDeck9(s16 *deck)
{
    int i;

    for (i = 0; i < 40; i++) {
        deck[i] = SampleDeck9[i];
    }
}

/* Per-enemy ROM deck lists (each entry points at a 40-byte card-id array). */
extern unsigned char *EnemyDeckLst[];

void CardCopyRom2Deck(s16 *deck, s32 enemyId)
{
    unsigned char *romDeck;
    int i;

    romDeck = EnemyDeckLst[enemyId];
    for (i = 0; i < 40; i++) {
        deck[i] = romDeck[i];
    }
}

/* The stored decks repeat CardSaveData.deck as 42-byte records, so the record
 * of slot n starts n * 42 bytes into the save block; the address is formed as
 * n * 32 plus n * 10 (0x00a0ae48..0x00a0ae60). Only the forty card ids at the
 * start of a record are copied, one byte each into a halfword entry. */
void CardCopyRam2Deck(s16 *deck, CardGameWork *work, s32 slot)
{
    unsigned char *stored;
    int i;

    stored = ((CardSaveData *)((slot << 5) + (unsigned int)work->save + slot * 10))->deck;
    for (i = 39; i >= 0; i--) {
        *deck++ = *stored++;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardCopyRam2CLD);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardCopyCLD2Ram);

/*
 * The one's-complement checksum of a stored deck's 40 card bytes
 * (CardCopyRam2Deck's comment documents the record's 42-byte stride and the
 * CardSaveData reuse for the deck bytes themselves); the stored checksum
 * sits 2 bytes before that same deck array, inside CardSaveData's own
 * unmodeled_02 span (+0x2A).
 */
s32 CardChkDeckRam(CardGameWork *work, int unused, s32 slot)
{
    u8 *deckByte;
    u16 storedChecksum;
    u8 sum;
    s32 remaining;

    deckByte = ((CardSaveData *) ((slot << 5) + (unsigned int) work->save + slot * 10))->deck;
    sum = 0;
    remaining = 0x27;
    do {
        sum += *deckByte;
        deckByte += 1;
        remaining -= 1;
    } while (remaining >= 0);
    storedChecksum = ((CardDeckRecord *) ((unsigned int) work->save + slot * 0x2A))->checksum;
    return (u8) ~sum == storedChecksum;
}

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

s32 CardPlayYamaCnt(CardPlaySide *side)
{
    s16 *card;
    s16 id;
    s32 count;
    s32 remaining;

    card = side->deck;
    count = 0;
    remaining = 0x27;
    do {
        id = *card;
        card += 1;
        remaining -= 1;
        if (id >= 0) {
            count = count + 1;
        }
    } while (remaining >= 0);
    return count;
}

s32 CardPlaySuteCnt(CardPlaySide *side)
{
    s16 *card;
    s16 id;
    s32 count;
    s32 remaining;

    card = side->sute;
    count = 0;
    remaining = 0x27;
    do {
        id = *card;
        card += 1;
        remaining -= 1;
        if (id >= 0) {
            count = count + 1;
        }
    } while (remaining >= 0);
    return count;
}

s32 CardPlayJunkCnt(CardPlaySide *side)
{
    s16 *card;
    s16 id;
    s32 count;
    s32 remaining;

    card = side->junk;
    count = 0;
    remaining = 0x27;
    do {
        id = *card;
        card += 1;
        remaining -= 1;
        if (id >= 0) {
            count = count + 1;
        }
    } while (remaining >= 0);
    return count;
}

s32 CardPlayOperationCnt(CardPlaySide *side)
{
    s16 *op;
    s16 id;
    s32 count;
    s32 remaining;

    op = side->operations;
    count = 0;
    remaining = 0x27;
    do {
        id = *op;
        op += 1;
        remaining -= 1;
        if (id >= 0) {
            count = count + 1;
        }
    } while (remaining >= 0);
    return count;
}

s32 CardPlayDisposeCnt(CardPlaySide *side)
{
    s16 *cardId;
    s16 id;
    s32 count;
    s32 remaining;

    cardId = &side->disposal[0].battle.layers[0].cardId;
    count = 0;
    remaining = 3;
    do {
        id = *cardId;
        cardId += 0xDE;
        remaining -= 1;
        if (id >= 0) {
            count = count + 1;
        }
    } while (remaining >= 0);
    return count;
}

s32 CardPlayHandCnt(CardHand *hand)
{
    s16 *card;
    s16 id;
    s32 count;
    s32 remaining;

    card = hand->cards;
    count = 0;
    remaining = 0x27;
    do {
        id = *card;
        card += 1;
        remaining -= 1;
        if (id >= 0) {
            count = count + 1;
        }
    } while (remaining >= 0);
    return count;
}

s32 CardPlayBattleCnt(CardPlaySide *side)
{
    s16 *cardId;
    s16 id;
    s32 count;
    s32 remaining;

    cardId = &side->battle[0].battle.layers[0].cardId;
    count = 0;
    remaining = 3;
    do {
        id = *cardId;
        cardId += 0xDE;
        remaining -= 1;
        if (id >= 0) {
            count = count + 1;
        }
    } while (remaining >= 0);
    return count;
}

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

/*
 * CGPCalcPowerPlus (ov10/tu008 cgp.c, already accepted C) takes CardPowerCard
 * *card, declared there a second time from the same memory CardLayerStack
 * covers because its own loads at +0x08..+0x0A are signed where
 * CardBattleLayer's accepted accesses of the same bytes are unsigned
 * (ov10/cgp.h's own comment on CardPowerCard). CardPlayCntPower always
 * passes a whole CardLayerStack's address, so the call below casts to the
 * accepted definition's own parameter type instead of redeclaring the
 * extern with a different one.
 */
extern s32 CGPCalcPowerPlus(CardGameWork *work, CardPowerCard *card, s32 power);

s32 CardPlayCntPower(CardGameWork *work, CardLayerStack *slot)
{
    CardDefinition *def;
    s16 topCardId;
    s32 power;

    power = 0;
    if (!(slot->flags & 0x80)) {
        def = &work->definitions[slot->layers[0].cardId];
        topCardId = slot->layers[1].cardId;
        if (topCardId >= 0 && topCardId != 0x35 && topCardId != 0x2F && topCardId != 0x2C) {
            def = &work->definitions[topCardId];
        }
        power = CGPCalcPowerPlus(work, (CardPowerCard *) slot, def->attackPower);
    }
    return power;
}

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

/*
 * CardPlayIdentityCheck (ov10:0x00a10b60): `hand` is a side's CardHand,
 * CardPlaySide's first member (ov10/cgp.h), so it is also the address of the
 * enclosing side; this walks that side's four battle and four disposal board
 * slots, at every stacked layer, for a card whose CardDefinition.identity
 * matches. `identity` masks to 16 bits at entry (`andi $a2,$a2,0xffff`, the
 * function's first instruction) rather than to a byte, so the parameter
 * itself is 16 bits wide; ccu_038_exec_sub.c's extern still declares it
 * `u8` and needs updating to match this definition.
 */
int CardPlayIdentityCheck(CardGameWork *work, CardHand *hand, u16 identity)
{
    s16 *cardIdSlot;    /* walks disposal[3]'s cardId column, one layer per iteration */
    int layer;
    int found;
    s16 cardId;

    found = 0;
    if (identity < 0x80) {
        cardIdSlot = (s16 *)((u8 *)hand + 0xDCE); /* &((CardPlaySide *)hand)->disposal[3].battle.layers[0].cardId */

        /*
         * battle[0..3] and disposal[0..3] are 8 consecutive CardBoardSlot
         * records; each check below reads the same layer of a different
         * slot, at a fixed distance (a multiple of sizeof(CardBoardSlot))
         * behind cardIdSlot's current slot, disposal[3].
         */
        layer = 0;
        do {
            cardId = *(s16 *)((u8 *)cardIdSlot - 7 * sizeof(CardBoardSlot)); /* battle[0] */
            if (cardId >= 0 && work->definitions[cardId].identity == identity) {
                found = 1;
                break;
            }
            cardId = *(s16 *)((u8 *)cardIdSlot - 6 * sizeof(CardBoardSlot)); /* battle[1] */
            if (cardId >= 0 && work->definitions[cardId].identity == identity) {
                found = 1;
                break;
            }
            cardId = *(s16 *)((u8 *)cardIdSlot - 5 * sizeof(CardBoardSlot)); /* battle[2] */
            if (cardId >= 0 && work->definitions[cardId].identity == identity) {
                found = 1;
                break;
            }
            cardId = *(s16 *)((u8 *)cardIdSlot - 4 * sizeof(CardBoardSlot)); /* battle[3] */
            if (cardId >= 0 && work->definitions[cardId].identity == identity) {
                found = 1;
                break;
            }
            cardId = *(s16 *)((u8 *)cardIdSlot - 3 * sizeof(CardBoardSlot)); /* disposal[0] */
            if (cardId >= 0 && work->definitions[cardId].identity == identity) {
                found = 1;
                break;
            }
            cardId = *(s16 *)((u8 *)cardIdSlot - 2 * sizeof(CardBoardSlot)); /* disposal[1] */
            if (cardId >= 0 && work->definitions[cardId].identity == identity) {
                found = 1;
                break;
            }
            cardId = *(s16 *)((u8 *)cardIdSlot - sizeof(CardBoardSlot));    /* disposal[2] */
            if (cardId >= 0 && work->definitions[cardId].identity == identity) {
                found = 1;
                break;
            }
            cardId = *cardIdSlot;                                          /* disposal[3] */
            if (cardId >= 0 && work->definitions[cardId].identity == identity) {
                found = 1;
                break;
            }
            cardIdSlot = (s16 *)((u8 *)cardIdSlot + sizeof(CardBattleLayer));
            layer++;
        } while (layer < 40);
    }
    return found;
}

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CGCPHandDispSubCore);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CGCPNeedDispSub);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CGCPGeneDispSub);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CGCPHandDispSub3);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CGCPHandDispSub2);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CGCPHandDispSub);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CGCPYamaDispSub);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CGCPJunkDispSub);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayCntAllOpe);

s32 CardPlayCntOpe(CardPlaySide *side, s16 opId)
{
    s16 *op;
    s16 id;
    s16 want;
    s32 count;
    s32 remaining;

    want = opId;
    op = side->operations;
    count = 0;
    remaining = 3;
    do {
        id = *op;
        op += 1;
        remaining -= 1;
        if ((want ^ id) == 0) {
            count = count + 1;
        }
    } while (remaining >= 0);
    return count;
}

/*
 * hand is CardHand only by its call sites' declared type (CardPlayCntFld,
 * outside this TU's allocation, passes &work->player.hand/&work->enemy.hand);
 * CardHand is CardPlaySide's own first member (ov10/cgp.h, offset +0x000), so
 * that address is numerically CardPlaySide's own and this reaches
 * disposal[0]'s top card id, sibling to CardPlayCntBtl's battle[0].
 */
int CardPlayCntDsp(CardHand *hand, s16 cardId)
{
    CardPlaySide *side;
    s16 *cardIdField;
    s16 id;
    s16 want;
    s32 count;
    s32 remaining;

    want = cardId;
    side = (CardPlaySide *) hand;
    cardIdField = &side->disposal[0].battle.layers[0].cardId;
    count = 0;
    remaining = 3;
    do {
        id = *cardIdField;
        cardIdField += 0xDE;
        remaining -= 1;
        if ((want ^ id) == 0) {
            count = count + 1;
        }
    } while (remaining >= 0);
    return count;
}

/* Same base-address identity as CardPlayCntDsp, reaching battle[0] instead. */
int CardPlayCntBtl(CardHand *hand, s16 cardId)
{
    CardPlaySide *side;
    s16 *cardIdField;
    s16 id;
    s16 want;
    s32 count;
    s32 remaining;

    want = cardId;
    side = (CardPlaySide *) hand;
    cardIdField = &side->battle[0].battle.layers[0].cardId;
    count = 0;
    remaining = 3;
    do {
        id = *cardIdField;
        cardIdField += 0xDE;
        remaining -= 1;
        if ((want ^ id) == 0) {
            count = count + 1;
        }
    } while (remaining >= 0);
    return count;
}

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayCntLight);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayCntHeavy);

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardEquipCntHeavy);

/* still INCLUDE_ASM above in this TU; CardHand * matches CardPlayHandCnt's
 * argument (ov10/cgp.h), the same player.hand/enemy.hand records this
 * function indexes. */
extern int CardPlayCntDsp(CardHand *hand, s16 cardId);
extern int CardPlayCntBtl(CardHand *hand, s16 cardId);

int CardPlayCntFld(CardGameWork *work, s16 cardId) {
    CardHand *playerHand;
    CardHand *enemyHand;
    int count;

    playerHand = &work->player.hand;
    enemyHand = &work->enemy.hand;
    count = CardPlayCntDsp(playerHand, cardId);
    count = count + CardPlayCntBtl(playerHand, cardId);
    count = count + CardPlayCntDsp(enemyHand, cardId);
    return count + CardPlayCntBtl(enemyHand, cardId);
}
