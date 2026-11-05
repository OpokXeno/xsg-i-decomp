#ifndef INCLUDE_OV10_CARD_DECK_CHK_H
#define INCLUDE_OV10_CARD_DECK_CHK_H

#include "shared.h"
#include "ov10/cgp.h"
#include "ov10/card_play_sound1.h"

/* Functions and data used by other translation units. */
extern char ZenkakuListA[20];
extern char ZenkakuListB[20];
extern char CardConvertNameLst[20][24];
extern unsigned char ST1RariLst[];
extern unsigned char ST1CardLst[39];
extern unsigned char BS1RariLst[16];
extern unsigned char BS1UncoLst[23];
extern unsigned char BS1ComoLst[44];
extern unsigned char BS2RariLst[20];
extern unsigned char BS2UncoLst[9];
extern unsigned char BS2ComoLst[17];

/* Deck screens: `list` is the slot or card list window, `deck` the deck window. */
extern int CardDeckChk(s16 *cards);
extern int CardMakeDeckProc(int exiting, CardListWindow *list, CardListWindow *deck, CardGameWork *work);
extern int CardLoadDeckProc(int exiting, CardListWindow *list, CardListWindow *deck, CardGameWork *work,
                            const char *title);
extern int CardSaveDeckProc(int exiting, CardListWindow *list, CardListWindow *deck, CardGameWork *work,
                            const char *title);
extern int CardLoadDeckProc2(int exiting, CardListWindow *list, CardListWindow *deck, CardGameWork *work,
                             const char *title);

#endif /* INCLUDE_OV10_CARD_DECK_CHK_H */
