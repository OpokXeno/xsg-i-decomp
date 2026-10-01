/*
 * OV10 original TU 4: 0x00a19a20..0x00a1c1b8 (5 functions)
 */
#include "common.h"
#include "shared.h"

int CardDeckChk(s16 *cards) {
    enum { CARD_COUNT = 40, MAX_COPIES = 3 };
    int first_index;
    int second_index;
    int matching_copies;

    for (first_index = 0; first_index < CARD_COUNT; first_index++) {
        matching_copies = 0;

        for (second_index = first_index + 1; second_index < CARD_COUNT; second_index++) {
            if (cards[second_index] < 0) {
                return 0;
            }

            if (cards[first_index] == cards[second_index]) {
                matching_copies++;
                if (matching_copies >= MAX_COPIES) {
                    return 0;
                }
            }
        }
    }

    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov10/card_deck_chk", CardMakeDeckProc);

INCLUDE_ASM("asm/nonmatchings/ov10/card_deck_chk", CardLoadDeckProc);

INCLUDE_ASM("asm/nonmatchings/ov10/card_deck_chk", CardSaveDeckProc);

INCLUDE_ASM("asm/nonmatchings/ov10/card_deck_chk", CardLoadDeckProc2);
