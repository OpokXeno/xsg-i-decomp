/*
 * ov10/tu010 (src/ov10/battle_area_check_enemy.c): the card game's computer
 * player. Functions used by other translation units.
 */
#ifndef INCLUDE_OV10_BATTLE_AREA_CHECK_ENEMY_H
#define INCLUDE_OV10_BATTLE_AREA_CHECK_ENEMY_H

#include "ov10/cgp.h"

/* Phase handlers CardEnemy* (ov10/card_enemy.c) runs for the computer side. */
s32 CC_Kara_SetFase(s32 side, CardGameWork *work);
s32 CC_Kara_CommFase(s32 side, CardGameWork *work);
s32 CC_Kara_CommRequest(s32 side, CardGameWork *work);
void CC_Kara_CommPlay(s32 side, CardGameWork *work);
s32 CC_Kara_MoveFase(s32 side, CardGameWork *work);
s32 CC_Kara_1stAnswer(s32 side, CardGameWork *work);
s32 CC_Kara_EndFase(s32 side, CardGameWork *work);

/* Answers the command evaluators give CardEnemyAnswerCardxx. */
s32 CheckDamageCommand(s32 side, CardGameWork *work, s32 threshold);
s32 CheckTaikyakuCommand(s32 side, CardGameWork *work);
s32 CheckXDamageCommand(s32 side, CardGameWork *work, s32 damage);

/* Price of a board slot's stack (CardDat of its base and stacked card). */
s32 GetPrice(CardLayerStack *stack);

/* DeckDataLst byte of cardId in the side's ROM deck, 0 when absent. */
u32 GetDeckData(int side, CardGameWork *work, int cardId);

#endif /* INCLUDE_OV10_BATTLE_AREA_CHECK_ENEMY_H */
