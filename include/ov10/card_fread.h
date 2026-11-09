#ifndef INCLUDE_OV10_CARD_FREAD_H
#define INCLUDE_OV10_CARD_FREAD_H

#include "shared.h"
#include "ov10/cgp.h"
#include "ov10/card_play_sound1.h"

#define CARD_ID_COUNT 150            /* card ids: entries of CardCommandPlay and of CardGameWork.ownedCount */

/* Layout of the card data and of the owned-card counts. */
#define CARD_OPERATION_SLOTS 4       /* operation cards a side counts (OperationPos1P has four positions) */
#define CARD_DATA_BUFFER ((char *)0x01000000)   /* card text data; a definition's nameOffset is relative to it (0x00a08ef4) */
#define CARD_ICON_MODEL_BASE 0x01C87400   /* icon bank: attribute icons H, R, M, G, U, W (indices 0..5, CardDisp, CardDispAttr), generation markers (CGCPGeneDispSub) and the equipment icon */
#define CARD_MODEL_STRIDE 1024            /* models of one icon family are 0x400 apart */
/*
 * CardDispLight light bits (the fourth argument): each bit applies the card light
 * (CCColor, asDir) to one part before its model is entered.
 */
#define CARD_LIGHT_FRAME 0x01       /* frame, overlay and back models (andi 0x1, 0x00a09544) */
#define CARD_LIGHT_HIT_POINTS 0x02  /* hit point digits (andi 0x2, 0x00a09668) */
#define CARD_LIGHT_POWER 0x04       /* power digits (andi 0x4, 0x00a09834) */
#define CARD_LIGHT_RESERVE 0x08     /* reserve cost digit (andi 0x8, 0x00a09a78) */
#define CARD_LIGHT_ATTRIBUTE 0x10   /* attribute icon (andi 0x10, 0x00a09bc0) */
#define CARD_LIGHT_NEED 0x20        /* requirement icons (andi 0x20, 0x00a09d34) */
#define CARD_LIGHT_ATTACK 0x40      /* attack type icon (andi 0x40, 0x00a09e90) */
#define CARD_LIGHT_RARITY 0x80      /* rarity mark (andi 0x80, 0x00a09f14) */
#define CARD_LIGHT_ALL 0xFF         /* every part lit */

#define CARD_ICON_TEXTURE ((const char *)0x01C91000)    /* lui 0x00a096fc */
#define CARD_COUNT_BITS 2                    /* bits of one owned-copy count in CardSaveData.cardCounts */
#define CARD_COUNT_MAX 3                     /* most copies of a card that are counted */
#define CARD_COUNTS_PER_BYTE 4               /* card ids packed in one cardCounts byte (8 / CARD_COUNT_BITS) */
#define CARD_COUNT_MASK(field) (CARD_COUNT_MAX << ((field) * CARD_COUNT_BITS)) /* bits of the count of id % 4 == field: 0x03, 0x0C, 0x30, 0xC0 */
#define CARD_COUNT_STEP(field) (1 << ((field) * CARD_COUNT_BITS))              /* one copy in that field: 0x01, 0x04, 0x10, 0x40 */
#define CARD_COUNT_KEEP(field) (0xFF ^ CARD_COUNT_MASK(field))                 /* the other three fields: 0xFC, 0xF3, 0xCF, 0x3F */

/* Functions and data used by other translation units. */

/* ov10/card_fread.c: the card data file, the card and deck list displays, the deck lists and the board of the card game. */
extern u16 CardCommandPlay[CARD_ID_COUNT];

extern int CardFread(void *address, char *filename);
extern void CardModelFadeOut(CardGameWork *work);
extern void CardModelFadeIn(CardGameWork *work);
extern void CardFadeOut(CardGameWork *work);
extern void CardFadeIn(CardGameWork *work);
extern int CardMakeDispList(CardListEntry *entries, u16 filter, CardDefinition *definitions, int cardCount);
extern s32 CardGetDeckAllCnt(CardListWindow *deck);
extern s32 CardGetDeckCardCnt(s16 cardId, CardListWindow *deck);
extern s32 CardAddDeck(CardListWindow *deck, s16 cardId);
extern int CardDelDeck(CardListWindow *deck, s16 cardId);
extern void CardDeckDisp(CardListWindow *deck, CardDefinition *definitions);
extern void CardDispTxtSub(u16 x, u16 y, unsigned char *text, u16 colorIndex);
extern void CardDispTxtSub2(u16 x, u16 y, unsigned char *text);
extern void CardDispLight(Matrix4 *matrix, s16 cardId, s16 enabled, u8 light);
extern void CardDispCard(Matrix4 *matrix, s16 cardId, s16 enabled);
extern void CardDisp(Matrix4 *matrix, s16 cardId, s16 owned, s16 selected);
extern void CardSampleInitDeck0(s16 *deck);
extern void CardSampleInitDeck1(s16 *deck);
extern void CardSampleInitDeck2(s16 *deck);
extern void CardSampleInitDeck3(s16 *deck);
extern void CardSampleInitDeck4(s16 *deck);
extern void CardSampleInitDeck5(s16 *deck);
extern void CardSampleInitDeck6(s16 *deck);
extern void CardSampleInitDeck7(s16 *deck);
extern void CardSampleInitDeck8(s16 *deck);
extern void CardSampleInitDeck9(s16 *deck);
extern void CardCopyRom2Deck(s16 *deck, s32 enemyId);
extern void CardCopyRam2Deck(s16 *deck, CardGameWork *work, s32 slot);
extern void CardCopyRam2CLD(CardListWindow *list, CardGameWork *work, s32 slot);
extern void CardCopyCLD2Ram(CardListWindow *list, CardGameWork *work, s32 slot);
extern s32 CardChkDeckRam(CardGameWork *work, int unused, s32 slot);
extern int CardCntList(CardGameWork *work, int index);
extern int CardAddList(CardGameWork *work, int cardId);
extern void CardPlayInitWork(CardPlaySide *side, s16 *deck);
extern void CPIWsub(CardPlaySide *side, s16 *deck);
extern void CardPlayNoInitWork(CardPlaySide *side, s16 *deck);
extern void CardPlayInitWorkFake(CardPlaySide *side, s16 *deck, CardDefinition *definitions);
extern void CardPlayShuffleYama(CardPlaySide *side);
extern s32 CardPlayDrawCard(CardPlaySide *side);
extern s32 CardPlayDrawSute(CardPlaySide *side);
extern s32 CardPlayRecavery(CardPlaySide *side);
extern void CardPlayCleanHand(CardHand *hand);
extern void CardPlayCleanYama(CardPlaySide *side);
extern void CardPlayCleanJunk(CardPlaySide *side);
extern s32 CardPlayDesertCard(CardPlaySide *side, s32 index);
extern int CardPlayDesert2Card(CardPlaySide *side, s32 firstIndex, s32 secondIndex);
extern s32 CardPlayDesertOperation(CardPlaySide *side, s32 operationIndex);
extern s32 CardPlayDesertBattle(CardPlaySide *side, CardLayerStack *battle);
extern s32 CardPlayCostCard(CardPlaySide *side);
extern int CardPlayReturnCard(CardHand *hand, CardLayerStack *battle, s16 index);
extern s32 CardPlayYamaCnt(CardPlaySide *side);
extern s32 CardPlaySuteCnt(CardPlaySide *side);
extern s32 CardPlayJunkCnt(CardPlaySide *side);
extern s32 CardPlayOperationCnt(CardPlaySide *side);
extern s32 CardPlayDisposeCnt(CardPlaySide *side);
extern s32 CardPlayHandCnt(CardHand *hand);
extern s32 CardPlayBattleCnt(CardPlaySide *side);
extern void CardPlayTypeSort(CardDefinition *definitions, s16 *cards, s32 count, s8 *order);
extern s32 CardPlayChkCost(CardDefinition *definitions, CardPlaySide *side, s32 cardId, CardGameWork *work);
extern void CardCountGeneration(u8 *counts, CardDefinition *definitions, CardPlaySide *side);
extern s32 CardPlayChkCondition(CardDefinition *definitions, CardPlaySide *side, s32 cardId, CardGameWork *work);
extern s32 CardChkAttackType(CardDefinition *definitions, CardLayerStack *card);
extern void CPCMSub(CardCursorControl *cursor, CardPlaySide *side, u16 buttons, u16 drawFlags, s32 previousZone);
extern void CardPlayCursorMove(CardCursorControl *cursor, CardPlaySide *side, u16 buttons, u16 drawFlags);
extern void CardListDisp(CardListWindow *list, CardGameWork *work);
extern void CardDeckListDisp(CardListWindow *list);
extern void CardStrListDisp(CardListWindow *list, char **strings, int index);
extern void CardEnemyListDisp(CardListWindow *list, s32 *entries, int index);
extern s32 CardPlayCntPower(CardGameWork *work, CardLayerStack *slot);
extern void CardPlayDispBattle(s32 side, CardGameWork *work, CardPlaySide *ownSide, Matrix4 matrix);
extern void CardPlayDispHand(s32 side, s32 faceUp, CardPlaySide *ownSide, Matrix4 matrix);
extern void CardPlayDispJunk(s32 side, CardPlaySide *ownSide, Matrix4 matrix);
extern void CardPlayDispDispose(s32 side, CardPlaySide *ownSide, Matrix4 matrix);
extern void CardPlayDispOperation(s32 side, CardPlaySide *ownSide, Matrix4 matrix);
extern void CardPlayDispYama(s32 side, CardPlaySide *ownSide, Matrix4 matrix);
extern void CardPlayDispSute(s32 side, CardPlaySide *ownSide, Matrix4 matrix);
extern void CardCursorPassive(CardCursorControl *cursor);
extern void CardCursorActive(CardCursorControl *cursor, CardCursorPosition *position, int mode);
extern void CardPlayDispCursor(s32 mode, CardGameWork *work, Matrix4 matrix);
extern int CardPlayIdentityCheck(CardGameWork *work, CardPlaySide *side, u16 identity);
extern void CGCPHandDispSubCore(CardHand *hand, int selected, Matrix4 *matrix, int flipped);
extern void CGCPNeedDispSub(int flipped, u8 *need, Matrix4 *matrix, u8 *counts);
extern void CGCPGeneDispSub(int flipped, CardDefinition *definitions, CardPlaySide *side, Matrix4 baseMatrix, u8 *counts);
extern void CGCPHandDispSub3(CardDefinition *definitions, CardPlaySide *side, int selected, int mode, int option);
extern void CGCPHandDispSub2(CardDefinition *definitions, CardPlaySide *side, int selected, int mode);
extern void CGCPHandDispSub(CardHand *hand, int selected, int flipped);
extern void CGCPYamaDispSub(CardPlaySide *side, int selectedOffset, int count, u16 flags, int style);
extern void CGCPJunkDispSub(CardPlaySide *side, int selected);
extern s32 CardPlayCntAllOpe(CardGameWork *work, s16 opId);
extern s32 CardPlayCntOpe(CardPlaySide *side, s16 opId);
extern int CardPlayCntDsp(CardPlaySide *side, s16 cardId);
extern int CardPlayCntBtl(CardPlaySide *side, s16 cardId);
extern s32 CardPlayCntLight(CardGameWork *work, CardPlaySide *side);
extern s32 CardPlayCntHeavy(CardGameWork *work, CardPlaySide *side);
extern s32 CardEquipCntHeavy(CardGameWork *work, CardPlaySide *side, s32 cardId);
extern int CardPlayCntFld(CardGameWork *work, s16 cardId);

/* nmlModelSet* setters as main/nml_model_set.h declares them (card_disp.c cannot include that header: it
 * declares nmlModelSetRenderLevel locally with a void return). */
extern void nmlModelSetLight(Matrix4 *colorMatrix, Matrix4 *normalMatrix);

#endif /* INCLUDE_OV10_CARD_FREAD_H */
