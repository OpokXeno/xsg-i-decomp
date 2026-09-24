/*
 * TU-local declarations of ov10/tu002 (src/ov10/card_fread.c).
 */

#ifndef SRC_OV10_CARD_FREAD_H
#define SRC_OV10_CARD_FREAD_H

#include "shared.h"
#include "ov10/cgp.h"

/*
 * A stored deck record: CardSaveData (ov10/cgp.h) reused at a 42-byte
 * stride, as the comment above CardCopyRam2Deck documents. The checksum
 * CardChkDeckRam validates sits 2 bytes before the 40 deck-id bytes
 * CardCopyRam2Deck already reads through CardSaveData.deck.
 */
typedef struct CardDeckRecord {
    u8 unmodeled_00[0x2A];
    u16 checksum;                /* +0x2A */
} CardDeckRecord;

#endif /* SRC_OV10_CARD_FREAD_H */
