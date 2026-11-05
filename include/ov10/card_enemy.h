/*
 * ov10/tu009 (src/ov10/card_enemy.c): the card game's computer-player turn
 * handlers and the ROM deck tables. Functions and data used by other
 * translation units.
 */
#ifndef INCLUDE_OV10_CARD_ENEMY_H
#define INCLUDE_OV10_CARD_ENEMY_H

#include "ov10/cgp.h"

/* Computer decks in deckdata.dat, and the cards of each (the deck and data fields of a record). */
#define CPU_DECK_COUNT 48
#define CPU_DECK_CARDS 40

/*
 * One CPU deck record inside cpudeck (0x80 bytes). InitCpuDeck slices
 * CPU_DECK_COUNT consecutive records into EnemyDeckListStr, DeckGeneLst,
 * EnemyDeckLst and DeckDataLst; CardCopyRom2Deck (ov10/card_fread.c) reads the
 * deck field of EnemyDeckLst[enemyId] as the ROM deck's card ids, which fixes
 * that field's width.
 */
typedef struct CpuDeckRecord {
    unsigned char name[0x20];
    unsigned char gene[0x10];
    unsigned char deck[CPU_DECK_CARDS];
    unsigned char data[CPU_DECK_CARDS];
} CpuDeckRecord;

/* Functions and data used by other translation units. */
extern CpuDeckRecord cpudeck[CPU_DECK_COUNT];
extern unsigned char *EnemyDeckListStr[CPU_DECK_COUNT];
extern unsigned char *DeckGeneLst[CPU_DECK_COUNT];
extern unsigned char *EnemyDeckLst[CPU_DECK_COUNT];
extern unsigned char *DeckDataLst[CPU_DECK_COUNT];

void InitCpuDeck(void);

void CardEnemyMove(int side, CardGameWork *work);
void CardEnemySet(int side, CardGameWork *work);
s32 CardEnemyCommandRequest(int side, CardGameWork *work);
int CardEnemyOperationPlay(int side, CardGameWork *work);
s32 CardEnemyCommandPlay(int side, CardGameWork *work);
s32 CardEnemy1stAnswer(int side, CardGameWork *work);
int CardEnemyAnswer(int side, CardGameWork *work);
int CardEnemyAnswerCardxx(int side, CardGameWork *work, s16 cardId);
void CardEnemyComm(int side, CardGameWork *work);
void CardEnemyAnswerSionSearch(int side, CardGameWork *work);
void CardEnemyEnd(int side, CardGameWork *work);

/* Plays the command card the side has chosen; targets holds count positions. */
int CGPEnemyExecCommand(int side, CardGameWork *work, CardCursorPosition *targets, int count);
/* Plays the Lv10 version of a character from the hand over its base card; 0 when it did. */
int CGPEnemyLv10SetSub(int side, CardGameWork *work, int index);

#endif /* INCLUDE_OV10_CARD_ENEMY_H */
