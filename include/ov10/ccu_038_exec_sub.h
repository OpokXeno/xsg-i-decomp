/*
 * ov10/tu003 (src/ov10/ccu_038_exec_sub.c): the card game's command cards.
 * CardPlayCommandPlay runs one queued command a step per call; the *ExecSub
 * functions are its effect handlers, shared with the computer player.
 */
#ifndef INCLUDE_OV10_CCU_038_EXEC_SUB_H
#define INCLUDE_OV10_CCU_038_EXEC_SUB_H

#include "ov10/cgp.h"

/* CardDefinition.flags of weapon cards: the attribute that may equip them ("Only M can equip", "Only H and R can equip"). */
#define CARD_ATTR_MECH_WEAPON 0x0020
#define CARD_ATTR_HUMAN_WEAPON 0x0040

/*
 * Command ids: what CardPlayCommandPlay switches on. CardCommandPlay[cardId]
 * (ov10/card_fread.c) gives a command card its id, 19..57 in card order
 * (Miyuki's Email to Break Through), 2 and 58 for the Realian pair. The ids
 * ending in _TRIGGER are queued by the battle code when the named character
 * card's condition arises; they queue the id that follows, which shows the
 * card (CGP_ERR_USE_CARD_EFFECT) and applies its effect.
 */
enum CardCommand {
    CARD_COMMAND_DRAW_CARD = 1,                 /* shows the card id in the operand, then draws */
    CARD_COMMAND_REALIAN_MALE = 2,
    CARD_COMMAND_LV_1_SHION_TRIGGER = 3,
    CARD_COMMAND_LV_1_SHION = 4,                /* fetch a Lv 10 Shion from the deck */
    CARD_COMMAND_VX_9000_TRIGGER = 7,
    CARD_COMMAND_VX_9000 = 8,
    CARD_COMMAND_CHERENKOV_TRIGGER = 9,
    CARD_COMMAND_CHERENKOV = 10,
    CARD_COMMAND_MARGULIS_TRIGGER = 11,
    CARD_COMMAND_MARGULIS = 12,
    CARD_COMMAND_TESTAMENT_TRIGGER = 13,
    CARD_COMMAND_TESTAMENT = 14,
    CARD_COMMAND_ALLEN_TRIGGER = 15,
    CARD_COMMAND_ALLEN = 16,
    CARD_COMMAND_TERRITORY_SHIFT_TRIGGER = 17,
    CARD_COMMAND_TERRITORY_SHIFT = 18,
    CARD_COMMAND_MIYUKIS_EMAIL = 19,
    CARD_COMMAND_CURE_ALL = 20,
    CARD_COMMAND_CURRY = 21,
    CARD_COMMAND_BIO_SPHERE = 22,
    CARD_COMMAND_SUPPLY_SHIP = 23,
    CARD_COMMAND_SUPPLY_FLEET = 24,
    CARD_COMMAND_RETREAT = 25,
    CARD_COMMAND_WOGLINDE = 26,
    CARD_COMMAND_SURPRISE_ATTACK = 27,
    CARD_COMMAND_FEDERATION_CRUISER = 28,
    CARD_COMMAND_INVITATION = 29,
    CARD_COMMAND_MARY_AND_SHELLEY = 30,
    CARD_COMMAND_UMN_INVASION = 31,
    CARD_COMMAND_INVESTIGATION = 32,
    CARD_COMMAND_LANDING_OPERATIONS = 33,
    CARD_COMMAND_PREMONITION = 34,
    CARD_COMMAND_DRAGON_SKULL = 35,
    CARD_COMMAND_EXPULSION_ORDER = 36,
    CARD_COMMAND_IMPREGNABLE_GUARD = 37,
    CARD_COMMAND_ARTICLE_4_13 = 38,
    CARD_COMMAND_ARTICLE_4_13_ADJUST = 39,      /* queued by 38 for the enemy's adjust phase */
    CARD_COMMAND_PHASE_CANNON = 40,
    CARD_COMMAND_NOTICE = 41,
    CARD_COMMAND_RESOLUTION = 42,
    CARD_COMMAND_RECYCLER = 43,
    CARD_COMMAND_REVIVE_DX = 44,
    CARD_COMMAND_STIM_DX = 45,
    CARD_COMMAND_SIMULATION = 46,
    CARD_COMMAND_WAR_CASUALTY = 47,
    CARD_COMMAND_X_BUSTER = 48,
    CARD_COMMAND_CHARGE = 49,
    CARD_COMMAND_PRECISE_SHOOTING = 50,
    CARD_COMMAND_INVOKE = 51,
    CARD_COMMAND_CONFUSION = 52,
    CARD_COMMAND_MILTIAN_CONFLICT = 53,
    CARD_COMMAND_SONG = 54,
    CARD_COMMAND_START_UP = 55,
    CARD_COMMAND_DESTINY = 56,
    CARD_COMMAND_BREAK_THROUGH = 57,
    CARD_COMMAND_REALIAN_FEMALE = 58,
    CARD_COMMAND_DESTROY_OWN_BOARD = 59,        /* drops the life of every own battle and disposal card to 0 */
    CARD_COMMAND_TONY_TRIGGER = 60,
    CARD_COMMAND_TONY = 61,
    CARD_COMMAND_BUNNIE_TRIGGER = 62,
    CARD_COMMAND_BUNNIE = 63,
    CARD_COMMAND_GAIGNUN_TRIGGER = 64,
    CARD_COMMAND_GAIGNUN = 65,
    CARD_COMMAND_U_TIC_SOLDIER_TRIGGER = 66,
    CARD_COMMAND_U_TIC_SOLDIER = 67,
    CARD_COMMAND_VX_9000_BATTLE_TRIGGER = 68,
    CARD_COMMAND_WILHELM_TRIGGER = 69,
    CARD_COMMAND_WILHELM = 70,
    CARD_COMMAND_JOACHIM_MIZRAHI_TRIGGER = 71,
    CARD_COMMAND_JOACHIM_MIZRAHI = 72,
    CARD_COMMAND_GARGOYLE_TRIGGER = 73,
    CARD_COMMAND_GARGOYLE = 74,
    CARD_COMMAND_SUPPLY_ORDER_TRIGGER = 75,
    CARD_COMMAND_SUPPLY_ORDER = 76,
    CARD_COMMAND_VIRGIL_TRIGGER = 77,
    CARD_COMMAND_VIRGIL = 78,
    CARD_COMMAND_IMPREGNABLE_GUARD_TRIGGER = 79,
    CARD_COMMAND_IMPREGNABLE_GUARD_NOTICE = 80
};

/* Entries of a side's command queue (CardGameWork.commandQueue[side]). */
#define CARD_QUEUE_SLOTS 40

/* Words of one command queue entry. */
enum CardQueueWord {
    CARD_QUEUE_PHASE,
    CARD_QUEUE_COMMAND,
    CARD_QUEUE_OPERAND,
    CARD_QUEUE_TARGET,
    CARD_QUEUE_WORDS
};

/* Effect handlers of the board cards. */
void CCU038ExecSub(CardPlaySide *owner, CardCursorPosition *position);
void CCU055ExecSub(CardPlaySide *owner, CardCursorPosition *position);
void CCU058ExecSub(int side, CardPlaySide *owner, CardCursorPosition *position);
void CCU061ExecSub(CardPlaySide *side, CardCursorPosition *position);
void CCU062ExecSub(CardPlaySide *owner, CardCursorPosition *position);
void CCU063ExecSub(CardGameWork *work, CardPlaySide *owner);
void CCU063ExecCPUSub(CardGameWork *work, CardPlaySide *owner);
void CCU064ExecSub(CardGameWork *work, CardPlaySide *owner);
void CCU064ExecCPUSub(CardGameWork *work, CardPlaySide *owner);
void CCU065ExecSub(CardGameWork *work, CardPlaySide *owner);
void CCU065ExecCPUSub(CardGameWork *work, CardPlaySide *owner);
s32 CCC107ChkSub(CardLayerStack *stack);
void CCC02ExecSub(u8 *commandState);
void CCC03ExecSub(int side, CardGameWork *work, CardHand *hand, CardLayerStack *card);
void CCC04ExecSub(int side, CardGameWork *work, CardPlaySide *owner);
void CCC06ExecSub(int side, CardGameWork *work, CardPlaySide *owner);
void CCC07ExecSub(CardGameWork *work, CardPlaySide *owner, CardCursorPosition *position);
void CCC10ExecSub(int side, CardGameWork *work, CardPlaySide *owner, CardCursorPosition *position);
void CCC26ExecSub(int side, CardGameWork *work, CardPlaySide *owner, CardCursorPosition *position);
void CCC30ExecSub(int side, CardGameWork *work, CardLayerStack *card);
void CCC31ExecSub(int side, CardGameWork *work, CardLayerStack *card);
void CCC38ExecSub(int side, CardGameWork *work, CardPlaySide *owner, CardCursorPosition *position);

/* Functions and data used by other translation units. */
s32 CardPlayCommandCondition(u16 side, u16 cardId, CardGameWork *work);
s16 *CPCSearchCommWk(u16 side, CardGameWork *work);
void CPCPSwapDSBATTLE(void *destination, void *source);
s32 CardPlayCommandPlay(u16 side, u16 command, CardGameWork *work, s16 operand, s16 target);
void CardPlayCommandBattleExecute(s16 side, s16 phase, CardGameWork *work);
int CardPlayCommandExecute(s16 phase, CardGameWork *work);
int CardPlayCommandCheck(s16 phase, s16 *queue);

#endif /* INCLUDE_OV10_CCU_038_EXEC_SUB_H */
