/*
 * OV10 original TU 9: 0x00a324b8..0x00a34190 (15 functions)
 */
#include "common.h"
#include "ov10/cgp.h"

INCLUDE_ASM("asm/nonmatchings/ov10/card_enemy", InitCpuDeck);

INCLUDE_ASM("asm/nonmatchings/ov10/card_enemy", CardEnemyMove);

INCLUDE_ASM("asm/nonmatchings/ov10/card_enemy", CardEnemySet);

/* CardPlayHandCnt (still assembler, ov10/tu002) counts the negative card ids
 * of one side's CardHand (include/ov10/cgp.h, owned by ov10/tu008). */
extern int CardPlayHandCnt(CardHand *hand);

/* Byte offsets of the two CardHand records inside CardGameWork (owned by
 * ov10/tu008), which names them playerHand and enemyHand. */
#define CARD_HAND_PLAYER_OFFSET 0x45C
#define CARD_HAND_ENEMY_OFFSET 0x2F08

/* AI command-request handler; defined in ov10/tu010 (still assembler). */
extern void CC_Kara_CommRequest(int side, CardGameWork *work);

void CardEnemyCommandRequest(int side, CardGameWork *work) {
    CardPlayHandCnt(side == 0
        ? (CardHand *) ((u8 *) work + CARD_HAND_PLAYER_OFFSET)
        : (CardHand *) ((u8 *) work + CARD_HAND_ENEMY_OFFSET));
    CC_Kara_CommRequest(side, work);
}

INCLUDE_ASM("asm/nonmatchings/ov10/card_enemy", CardEnemyOperationPlay);

/* AI command-play handler; defined in ov10/tu010 (still assembler). */
extern void CC_Kara_CommPlay(void);

void CardEnemyCommandPlay(void) {
    CC_Kara_CommPlay();
}

/* AI first-answer handler; defined in ov10/tu010 (still assembler). */
extern void CC_Kara_1stAnswer(int side, CardGameWork *work);

void CardEnemy1stAnswer(int side, CardGameWork *work) {
    CardPlayHandCnt(side == 0
        ? (CardHand *) ((u8 *) work + CARD_HAND_PLAYER_OFFSET)
        : (CardHand *) ((u8 *) work + CARD_HAND_ENEMY_OFFSET));
    CC_Kara_1stAnswer(side, work);
}

INCLUDE_ASM("asm/nonmatchings/ov10/card_enemy", CardEnemyAnswer);

INCLUDE_ASM("asm/nonmatchings/ov10/card_enemy", CardEnemyAnswerCardxx);

INCLUDE_ASM("asm/nonmatchings/ov10/card_enemy", CGPEnemyExecCommand);

INCLUDE_ASM("asm/nonmatchings/ov10/card_enemy", CGPEnemyExecOperation);

INCLUDE_ASM("asm/nonmatchings/ov10/card_enemy", CGPEnemyLv10SetSub);

INCLUDE_ASM("asm/nonmatchings/ov10/card_enemy", CardEnemyComm);

INCLUDE_ASM("asm/nonmatchings/ov10/card_enemy", CardEnemyAnswerSionSearch);

/* AI end-of-phase handler; defined in ov10/tu010 (still assembler). */
extern int CC_Kara_EndFase(int side, CardGameWork *work);

/* Advances the card game turn; defined in ov10/tu008 (still assembler). */
extern void CGPNextTurnSub(CardGameWork *work);

void CardEnemyEnd(int side, CardGameWork *work) {
    int endFaseResult = 1;

    if (CardPlayHandCnt(side == 0
            ? (CardHand *) ((u8 *) work + CARD_HAND_PLAYER_OFFSET)
            : (CardHand *) ((u8 *) work + CARD_HAND_ENEMY_OFFSET)) >= 7) {
        endFaseResult = CC_Kara_EndFase(side, work);
    }
    if (endFaseResult == 0) {
        CGPNextTurnSub(work);
    }
}
