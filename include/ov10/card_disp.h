#ifndef INCLUDE_OV10_CARD_DISP_H
#define INCLUDE_OV10_CARD_DISP_H

#include "shared.h"
#include "ov10/cgp.h"

/*
 * Help pages: page ids are the indices of CardTitleTxtLst (0x00a483e8, 59 entries) and of the
 * jump table in CardDispHelpCard; each name spells that page's title string. Pages whose
 * title repeats get an _A/_B/_C suffix in id order. CardGameWork.helpTopic holds one of them.
 */
enum CardHelpPage {
    CARD_HELP_PAGE_WHAT_IS_XENO_CARD = 0,
    CARD_HELP_PAGE_CONTROLS = 1,
    CARD_HELP_PAGE_CARD_OUTLINE = 2,
    CARD_HELP_PAGE_GAME_PROGRESSION = 3,
    CARD_HELP_PAGE_PROGRESSION_PHASE = 4,
    CARD_HELP_PAGE_PROGRESSION_TURN = 5,
    CARD_HELP_PAGE_POSITIONING = 6,
    CARD_HELP_PAGE_DECK = 7,
    CARD_HELP_PAGE_LOST_PILE = 8,
    CARD_HELP_PAGE_HAND = 9,
    CARD_HELP_PAGE_JUNK_PILE = 10,
    CARD_HELP_PAGE_STANDBY_AREA = 11,
    CARD_HELP_PAGE_SITUATION_AREA = 12,
    CARD_HELP_PAGE_BATTLEFIELD = 13,
    CARD_HELP_PAGE_FIELD = 14,
    CARD_HELP_PAGE_CARD = 15,
    CARD_HELP_PAGE_BATTLE_CARD = 16,
    CARD_HELP_PAGE_EVENT_CARD = 17,
    CARD_HELP_PAGE_SITUATION_CARD = 18,
    CARD_HELP_PAGE_DETAILS = 19,
    CARD_HELP_PAGE_TYPES = 20,
    CARD_HELP_PAGE_NAMES = 21,
    CARD_HELP_PAGE_ATTRIBUTES = 22,
    CARD_HELP_PAGE_REQUIREMENTS_A = 23,
    CARD_HELP_PAGE_REQUIREMENTS_B = 24,
    CARD_HELP_PAGE_COST = 25,
    CARD_HELP_PAGE_HP = 26,
    CARD_HELP_PAGE_ATK = 27,
    CARD_HELP_PAGE_ATTACK_TYPE = 28,
    CARD_HELP_PAGE_TEXT = 29,
    CARD_HELP_PAGE_HOW_TO_PLAY = 30,
    CARD_HELP_PAGE_TURNS_A = 31,
    CARD_HELP_PAGE_TURNS_B = 32,
    CARD_HELP_PAGE_DRAW_PHASE = 33,
    CARD_HELP_PAGE_MOVE_PHASE = 34,
    CARD_HELP_PAGE_EVENT_PHASE_A = 35,
    CARD_HELP_PAGE_EVENT_PHASE_B = 36,
    CARD_HELP_PAGE_EVENT_PHASE_C = 37,
    CARD_HELP_PAGE_SET_PHASE_A = 38,
    CARD_HELP_PAGE_SET_PHASE_B = 39,
    CARD_HELP_PAGE_BLOCK_PHASE = 40,
    CARD_HELP_PAGE_BATTLE_PHASE_A = 41,
    CARD_HELP_PAGE_BATTLE_PHASE_B = 42,
    CARD_HELP_PAGE_ADJUST_PHASE = 43,
    CARD_HELP_PAGE_SPECIAL_CARD = 44,
    CARD_HELP_PAGE_WEAPONS_A = 45,
    CARD_HELP_PAGE_WEAPONS_B = 46,
    CARD_HELP_PAGE_GNOSIS = 47,
    CARD_HELP_PAGE_TERMINOLOGY = 48,
    CARD_HELP_PAGE_PENETRATE = 49,
    CARD_HELP_PAGE_DOWN = 50,
    CARD_HELP_PAGE_E_MARK = 51,
    CARD_HELP_PAGE_EVENT_BLOCK = 52,
    CARD_HELP_PAGE_OPENING_CARDS = 53,
    CARD_HELP_PAGE_VIEWING_CARDS = 54,
    CARD_HELP_PAGE_CREATING_DECKS = 55,
    CARD_HELP_PAGE_SAVING_DECKS = 56,
    CARD_HELP_PAGE_TWO_PLAYER_BATTLE = 57,
    CARD_HELP_PAGE_SELECTING_DECKS = 58,
    CARD_HELP_PAGE_COUNT = 59  /* slti 0x3B at 0x00a20d68 */
};
#define CARD_HELP_PAGE_LAST (CARD_HELP_PAGE_COUNT - 1)  /* li 0x3A at 0x00a20da8 */

/* CardGameWork.helpState value that opens the help screen: CardHelpProc resets both lists and runs the INDEX screen
 * (case at 0x00a209bc); the START menu's Help item and the card game proper set it (sb 0x00a01fb4). */
#define CARD_HELP_STATE_OPEN 0x30

/*
 * Card number digits drawn by the help page counter (CardHelpProc) and the deck builder
 * (CardMakeDeckProc): ten models 0x400 apart, indexed by the digit, all with one texture.
 */
#define CARD_DIGIT_MODEL_BASE 0x01C8A400
#define CARD_DIGIT_MODEL_SHIFT 10
#define CARD_DIGIT_TEXTURE ((const char *)0x01C990A0)

/* Render level of every card model (NML_LEVEL_NO_SPECULAR in include/main/nml_model_set.h). */
#define CARD_MODEL_RENDER_LEVEL 2

/* Functions and data used by other translation units. */
extern void CardHelpFileLoad(void);
extern void CardHelpProc(CardGameWork *work);
extern void CardHelpPageInit(CardGameWork *work);

#endif /* INCLUDE_OV10_CARD_DISP_H */
