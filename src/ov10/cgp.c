/*
 * OV10 original TU 8: 0x00a21768..0x00a324b8 (68 functions)
 */
#include "common.h"
#include "cgp.h"
#include "main/xgl_font.h"
#include "main/xgl_sound.h"
#include "main/xgl_2.h"
#include "main/nml_model_set.h"
#include "ov10/card_id.h"
#include "ov10/ccu_038_exec_sub.h"
#include "ov10/card_play_sound1.h"
#include "ov10/card_fread.h"
#include "ov10/card_enemy.h"
#include "ov10/card_deck_chk.h"
#include "ov10/card_disp.h"

/*
 * Card models are loaded into a fixed EE runtime arena.  card_play_sound1.c
 * fills that arena with CardFread at 0x01900000 and 0x01930000, then passes
 * those addresses to nmlModelEntryCard.  These tables retain the original
 * arena addresses; they do not define storage for the model payloads.
 */
#define CARD_MODEL_ARENA_BASE 0x01900000
#define CARD_MODEL_AT(offset) ((void *)(CARD_MODEL_ARENA_BASE + (offset)))

/* Cards a hand holds at most before one must be discarded. */
#define CGP_HAND_LIMIT 7

/* Layout CGPCheckTeMax gives the hand's card list window. */
#define CGP_HAND_LIST_BASE_COLOR 7
#define CGP_HAND_LIST_CURSOR_COLOR 15
#define CGP_HAND_LIST_X 0x150
#define CGP_HAND_LIST_Y 0x30
#define CGP_HAND_LIST_TEXT_LENGTH 14
#define CGP_HAND_LIST_ROWS 10

/* Phases of a turn: 0..6 belong to side 0 and 7..13 to the other side. */
#define CGP_PHASE_COUNT 14
#define CGP_SIDE1_FIRST_PHASE 7

/* Sound ids CGPNextTurnSub plays. */
#define CGP_SE_NEXT_TURN 0x20001     /* a new turn and the switch to the other side */
#define CGP_SE_NEXT_PHASE 6          /* every other phase change */



/* Error message id CGPDispoSub0 and CGPDisposeSub use for "play points fall short" (CGPSetErrorMess reason 12). */
#define CGP_ERR_NOT_ENOUGH_POINTS 12
#define CGP_ERR_CARDS_TO_PLAY 14         /* "Move Error: There are cards still available to play." */

/* Cards 7, 9 and 10..13 are paired with the card seven ids below them. */
#define CGP_PAIRED_CARD_OFFSET 7

/*
 * Board areas of the cursor, as CGPCursor2Pos reports them in its zone field
 * (the cases that index disposal and battle slots): each side's disposal row
 * and battle row.
 */
#define CGP_AREA_ENEMY_HAND 0
#define CGP_AREA_ENEMY_DISPOSAL 2
#define CGP_AREA_ENEMY_BATTLE 4
#define CGP_AREA_PLAYER_BATTLE 5
#define CGP_AREA_PLAYER_DISPOSAL 6
#define CGP_AREA_PLAYER_HAND 9

/* Frames the interrupt message of a new phase stays up. */
#define CGP_INTERRUPT_MESS_FRAMES 45


/* Text colour of the card game's font strings. */
#define CGP_TEXT_COLOR 0xFFF0

/* Vertical distance between the text lines of one error message. */
#define CGP_ERROR_MESS_LINE_PITCH 24

/* Fixed EE runtime addresses of the textures the game screen draws with. CGP_PANEL_TEXTURE is the one texture
 * of the whole model group from 0x01DA0000 (title logo, hand, piles, message window, prompts, card backdrop,
 * phase lamp and turn plate); CGP_MAT_FG_TEXTURE is game\mat_fg.xtx and CGP_CARD_INFO_TEXTURE CardInfo0.bin
 * (ov10/card_play_sound1.c loads both). */
#define CGP_PANEL_TEXTURE ((void *) 0x01DB5800)
#define CGP_MAT_FG_TEXTURE ((void *) 0x01F70200)
#define CGP_CARD_INFO_TEXTURE ((void *) 0x01C91000)

/* Models of the information panel and the phase guide, fixed arena addresses (0x01C80000: the ten digit models,
 * 0x400 bytes apart). */
#define CGP_TURN_DIGIT_MODEL_BASE 0x01C80000
#define CGP_TURN_DIGIT_MODEL_SIZE 0x400
#define CGP_PHASE_LAMP_MODEL ((void *) 0x01DB0200)    /* lit marker drawn over each entry of CPIPhaseMdlLst */
#define CGP_TURN_PLATE_MODEL ((void *) 0x01DB2D00)    /* turn counter plate, lit with turnColor */
#define CGP_TURN_NUMBER_MODEL ((void *) 0x01921800)   /* turn counter number frame, 0.05 in front of the plate */
#define CGP_INFO_PANEL_SIDE_MODEL 0x01DAD700           /* side nameplate, lit 1P or 2P */

/* Fixed EE runtime address of the message window model. */
#define CGP_ERROR_MESS_MODEL ((void *) 0x01DA2B00)

#define CARD_EFFECT_COUNT 64
#define CARD_EFFECT_SE_HIT 0x20008
#define CARD_EFFECT_SE_BURST 0x2000A
#define CARD_EFFECT_ALPHA 0x48                /* nmlModelSetAlpha argument */
#define CARD_EFFECT_SPRITE_MODEL ((void *) 0x01A00000)   /* entry model of an unscaled effect sprite */
#define CARD_EFFECT_SCALED_MODEL ((void *) 0x01A00800)   /* entry model after xglMatrixScale */

/* Animation strips: frame n of a strip is at base + n * frame size. */
#define CARD_EFFECT_SMALL_FRAME 0x1480        /* 41 * 128 */
#define CARD_EFFECT_LARGE_FRAME 0x1D80        /* 59 * 128 */
#define CARD_EFFECT_FRAME(base, index, size) ((void *) ((base) + (index) * (size)))
#define CARD_EFFECT_STRIP_TRAVEL_1 0x1BF0000
#define CARD_EFFECT_STRIP_TRAVEL_2 0x1C04000
#define CARD_EFFECT_STRIP_TRAVEL_3 0x1A14000
#define CARD_EFFECT_STRIP_TRAVEL_4 0x1A01000
#define CARD_EFFECT_STRIP_BURST_1 0x1BD8000
#define CARD_EFFECT_STRIP_BURST_2 0x1AA0000
#define CARD_EFFECT_STRIP_BURST_3 0x1A85000
#define CARD_EFFECT_STRIP_BURST_4 0x1A67000
#define CARD_EFFECT_STRIP_FADE 0x1A28000
#define CARD_EFFECT_STRIP_SWIRL 0x1A40000     /* code 10 */

/* Damage digits: model n of a digit set is at base + (n << 10). */
#define CARD_EFFECT_DIGIT_TEXTURE ((void *) 0x1C990A0)
#define CARD_EFFECT_DIGITS_FIRST 0x1C8A400    /* code 12 while phase is 0 */
#define CARD_EFFECT_DIGITS 0x1C8CC00
#define CARD_EFFECT_DIGIT_MODEL(base, n) ((void *) ((base) + ((n) << 10)))

/* Arena the card game loads its model, texture and effect files into (see CPISMdlLst). */
#define CGP_ARENA(offset) CARD_MODEL_AT(offset)

#define CGP_FLAG_HELD_TRIANGLE 0x80   /* triangle was held when the game started */
/* Steps of the information panel's animation (infoPanelPhase, CardPlayDispInfo): it appears scaled up and shrinks in,
 * waits, slides toward its corner and rests there; the draw fades it in during CGP_INFO_PANEL_ZOOM. */
#define CGP_INFO_PANEL_INIT 0
#define CGP_INFO_PANEL_ZOOM 1
#define CGP_INFO_PANEL_HOLD 2
#define CGP_INFO_PANEL_SLIDE 3
#define CGP_INFO_PANEL_SETTLE 4

#define CGP_EFFECT_COUNT 64           /* counts down to zero */
#define CGP_QUEUE_SLOTS 40
#define CGP_QUEUE_SIDE_DISTANCE 160   /* halfwords from the enemy's queue back to the player's */

/* Rotation vector the stage starts from. */
#define CGP_ROT_STAGE_START_X (-0.9f)
#define CGP_ROT_STAGE_START_Y (-20.0f)
#define CGP_ROT_STAGE_COOLDOWN 20          /* frames before the stage may rotate again (CGPRotStageSub and CardGameProc) */

/* CardDefinition.kind: a battle card is paid from playPoints[0] and a situation card from playPoints[1]; the
 * situation card is the one CardPlayDesertOperation removes; an event card cannot be played from the hand
 * (error 11) and is the one the hand discard step accepts. */
/* CardLayerStack.flags bit set on a locked battle slot. */
/* Operation card 122 in play on the defending side lowers the attack power. */

/*
 * The cursor of a controller steps over the board zones (CGP_AREA_*), four
 * slots per zone. A card pile can also be walked: cursorMoveStep[0] is the
 * player's discard pile and cursorMoveStep[1] the enemy's.
 */
#define CGP_PILE_BOARD 0              /* the cursor is on the board */
#define CGP_PILE_TOP 1                /* the cursor sits on the pile and shows its top card */
#define CGP_PILE_BROWSE 2             /* the cards of the pile are walked one by one */

/*
 * Cursor slot that stands for a pile: slot 3 of the player's hand zone and slot 0
 * of the enemy's hand zone.
 */
#define CGP_PLAYER_PILE_SLOT 3

/* Highlight of the cursor over a hand. */
#define CGP_HAND_HIGHLIGHT 0.175f

/* Card id bits 8..11 are cleared before the preview draws the card. */
#define CGP_CARD_ID_MASK 0xF0FF

/*
 * Card preview, shown while the preview button is held: the card under the
 * cursor in the middle, the card stacked under it on the right and the card
 * the side is playing on the left. CardDisp's last argument is the display
 * mode: CGP_CARD_MODE_FULL draws the card at full size with its text,
 * CGP_CARD_MODE_ICON a plain card. The two side cards are shown small and
 * enlarged while their zoom button is held (PAD_L1/RIGHT); small
 * they get a frame model, the enlarged ones sit nearer the middle.
 */
#define CGP_CARD_MODE_ICON 0
#define CGP_CARD_MODE_FULL 5
#define CGP_PREVIEW_FULL_Y 0.369f
#define CGP_PREVIEW_FULL_Z 0.5f
#define CGP_PREVIEW_FULL_SCALE 1.1f
#define CGP_PREVIEW_SMALL_Y 0.096f
#define CGP_PREVIEW_SMALL_Z 0.48f
#define CGP_PREVIEW_SMALL_SCALE 0.55f
#define CGP_PREVIEW_SIDE_X 1.904f     /* small side cards stand this far from the middle */
#define CGP_PREVIEW_ZOOMED_X 0.9f     /* enlarged side cards */
#define CGP_PREVIEW_FRAME_Z 0.29f     /* frame model behind a small side card */
#define CGP_PREVIEW_FRAME_TEXTURE CARD_MODEL_AT(0x39800)
#define CGP_PREVIEW_FRAME_RIGHT CARD_MODEL_AT(0x29800)
#define CGP_PREVIEW_FRAME_LEFT CARD_MODEL_AT(0x2B000)

/* Situation cards CGPChkOpePlayable checks the board for before they may be discarded (switch cases 0x8B, 0x8D, 0x8F). */


/*
 * While this operation card is in play a dying CARD_ATTR_GNOSIS card goes to
 * the discard (sute) pile instead of the junk pile.
 */

/*
 * A card leaving the board slides to its pile in this many frames: the move
 * record's timer and the divisor of its per-frame step.
 */
#define CGP_PILE_MOVE_FRAMES 10
#define CGP_PILE_MOVE_Z 0.12f        /* height of the sliding card */
#define CGP_SUTE_PILE_X 1.265f       /* discard pile position */
#define CGP_SUTE_PILE_Y (-1.81f)

/*
 * Attack types CardChkAttackType reports for the card in play and the battle
 * command an attack runs under (CGPBattleSub attacks only when they agree).
 */
#define CGP_ATTACK_FRONT 1        /* front slots only, at the opposing card */
#define CGP_ATTACK_PIERCE 2       /* also hits the card behind its target */
#define CGP_ATTACK_SUPPORT 3      /* strikes the stack passed as `support` */
#define CGP_ATTACK_DIRECT 4       /* no target card */

#define CGP_STACK_TOP_LAYER 39    /* highest layer searched for the top card */

#define CGP_HAND_FLAG_NO_DAMAGE 0x01   /* CardHand.commandFlags bit 0 */
#define CGP_LIFE_WEAR_LIMIT 64         /* a stacked card below this life wears down */
#define CGP_POWER_MAX 99

#define CGP_SE_ATTACK 0x20004
#define CGP_SE_ATTACK_ALT 0x20009
#define CGP_SE_FRONT_ATTACK 0x20002
#define CGP_SE_FRONT_ATTACK_ALT 0x20005
#define CGP_SE_DIRECT_ATTACK 0x20007
#define CGP_SE_DIRECT_VOLUME 60
#define CGP_SE_DIRECT_PAN 64

/* Steps of CGPDrawFaseProc (command.phaseTimer). */
#define CGP_DRAW_STEP_BEGIN 0
#define CGP_DRAW_STEP_EXCHANGE_OFFER 1
#define CGP_DRAW_STEP_EXCHANGE_PROMPT 2
#define CGP_DRAW_STEP_EXCHANGE 3
#define CGP_DRAW_STEP_EFFECT_BEGIN 10
#define CGP_DRAW_STEP_EFFECT_COUNT 11
#define CGP_DRAW_STEP_EFFECT_PAY 12
#define CGP_DRAW_STEP_EFFECT_WAIT 13
#define CGP_DRAW_STEP_OPE_CURSOR 20
#define CGP_DRAW_STEP_ADD_PROMPT 21
#define CGP_DRAW_STEP_BONUS_COUNT 30
#define CGP_DRAW_STEP_DRAW 31
#define CGP_DRAW_STEP_DRAW_WAIT 32

/* Interrupt message the fase shows on entry. */
#define CGP_DRAW_INTERRUPT_MESS 22
/* Exchanges of the opening hand a side is offered. */
#define CGP_DRAW_EXCHANGES 3
/* Cards drawn for the opening hand. */
#define CGP_DRAW_HAND_SIZE 6
/* Frames the draw step waits after each card. */
#define CGP_DRAW_WAIT_FRAMES 50

/*
 * Error messages (ErrorMessTbl rows) of the fase: 33..35 "Exchange Cards - Is
 * this hand okay? You have 3/2/1 more exchange(s)", 26 "Select Card - Add this
 * card to your hand?" and 36 "Use Card Effect - Executing this card's text".
 */
#define CGP_ERR_EXCHANGE_THREE 33
#define CGP_ERR_EXCHANGE_TWO 34
#define CGP_ERR_EXCHANGE_ONE 35
#define CGP_ERR_ADD_TO_HAND 26

/*
 * Operation card 132 (0x84): its duration is refilled from definitions[132].operationDuration
 * while a card with CARD_ATTR_GNOSIS stands on the board. CGPDrawFaseProc highlights it on the board.
 */

/* Operation slot rows of the cursor (CGP_AREA_* continued). */
#define CGP_AREA_ENEMY_OPERATION 3
#define CGP_AREA_PLAYER_OPERATION 7

/* Whether the card list of the side is drawn upside down: the player's hand when the stage is rotated, the other hand when it is not. */
#define CGP_HAND_FLIPPED(side, work) \
    (((side) != 0 && (work)->rotStageId == 0) || ((side) == 0 && (work)->rotStageId != 0))

/* CGPMoveFaseProc steps (command.phaseTimer). */
#define CGP_MOVE_STEP_PICK 0          /* put the cursor on the first occupied slot */
#define CGP_MOVE_STEP_LIST 1          /* light up the slots a card can be picked from */
#define CGP_MOVE_STEP_SELECT 2        /* confirm the card to move */
#define CGP_MOVE_STEP_TARGET 3        /* confirm the slot to move it to */
#define CGP_MOVE_STEP_DONE 4          /* back to the first step */
#define CGP_MOVE_STEP_END 20          /* nothing left to move: wait one frame */
#define CGP_MOVE_STEP_NEXT_TURN 21

/* Cursor modes CardCursorActive takes. */
#define CGP_CURSOR_SELECTABLE 4
#define CGP_CURSOR_BLOCKED 3          /* a Gnosis card while Hilbert Effect is in play */
#define CGP_CURSOR_PICKED 2           /* the card that was picked up */
#define CGP_CURSOR_TARGET 1           /* a slot it can be moved to */

/* CardSaveData.flags bits that make the side's moves automatic. */
#define CGP_SAVE_FLAG_SIDE0_AUTO 0x08
#define CGP_SAVE_FLAG_SIDE1_AUTO 0x04

#define CGP_ERR_NO_MOVABLE_CARD 6
#define CGP_ERR_NO_CARD_THERE 11      /* hand area chosen */
#define CGP_ERR_MOVE_REFUSED 15

/*
 * Steps of CGPSetFaseProc (command.phaseTimer): the fase in which the side puts hand cards on the
 * board. The hand is sorted first (0), then the cursor picks a card (1). A card that fits is placed by
 * CGPDisposeSub; weapon cards go through the attach steps (2, 3); a card that finds no free slot needs
 * cards discarded first (10..13) or, for the Lv 10 cards, replaces the Lv 1 card seven ids below it (20).
 */
#define CGP_SET_STEP_SORT_HAND 0
#define CGP_SET_STEP_PICK_CARD 1
#define CGP_SET_STEP_ATTACH_LIGHT 2   /* light the cards the picked weapon card can attach to */
#define CGP_SET_STEP_ATTACH_PICK 3
#define CGP_SET_STEP_DISCARD_BEGIN 10
#define CGP_SET_STEP_DISCARD_PICK 11  /* "Cards to be discarded: N" */
#define CGP_SET_STEP_DISCARD_NEXT 12
#define CGP_SET_STEP_PUT_DOWN 13      /* "Where do you want to put it?" */
#define CGP_SET_STEP_PAIRED_SLOT 20

/* CGPDisposeSub's result for a card that finds no free slot. */
#define CGP_DISPOSE_NO_FREE_SLOT 15

#define CGP_CARD_FLAG_RESET 14        /* its layer flag is cleared unless the side has exactly one battle card */

#define CGP_ERR_FLAG_RESET 0x25
#define CGP_ERR_NO_WEAPON_TARGET 2

/* Cursor zones the sorted hand starts on: the zone beside the hand while the hand is empty. */
#define CGP_AREA_PLAYER_NO_HAND 8
#define CGP_AREA_ENEMY_NO_HAND (-1)

/* CardCursorActive mode that lights a slot as selectable. */
#define CGP_CURSOR_LIGHT 1
/* CGCPHandDispSub2/3 mode of the hand while a card is picked. */
#define CGP_HAND_DISP_PICK 5

#define CGP_SET_DISCARD_TEXT_X 143
#define CGP_SET_DISCARD_COUNT_X 363
#define CGP_SET_PUT_DOWN_TEXT_X 160
#define CGP_SET_PROMPT_Y 213
#define CGP_SET_TEXT_COLOR 0xFFF8
#define CGP_ZENKAKU_GLYPH_SIZE 2      /* full-width digit tables hold two bytes per glyph */

/* Prompt panel: model and texture of the card frame, placed 0.91 in front for the discard prompt. */
#define CGP_SET_PROMPT_MODEL ((void *) 0x01DAAC00)
#define CGP_SET_PROMPT_SCALE_X 1.5f
#define CGP_SET_PROMPT_Z 0.91f

/* A card put on the board starts its slide at this height and takes ten frames. */
#define CGP_PLACE_START_Z 0.12f
#define CGP_PLACE_FRAMES 10
#define CGP_PLACE_SLIDE_DIVISOR 10.0f

/* CardDefinition.discardRequirements has one byte per requirement. */
#define CGP_REQUIREMENT_COUNT 4

/*
 * Message ids the command steps hand to CGPSetErrorMess / CGPDispErrorMess /
 * CGPDispErrorMessPlus (rows of ErrorMessTbl).
 */
#define CGP_ERR_COMMAND_FAILED 13      /* "Command Error" */

/* Sound CardSetEffect starts with an operation effect. */
#define CGP_SE_COMMAND_EFFECT 0x20008

/* Prompt "Cards to be discarded" of command 131 and its card model. */
#define CGP_DISCARD_PROMPT_X 0x8F
#define CGP_DISCARD_PROMPT_Y 0xD5
#define CGP_DISCARD_PROMPT_COLOR 0xFFF8
#define CGP_DISCARD_PROMPT_MODEL ((void *) 0x01DAAC00)
#define CGP_DISCARD_PROMPT_SCALE_X 1.5f

/* Life command 139 takes off the card it targets. */
#define CGP_TARGET_LIFE_LOSS 3

/* Frames Rhine Maiden pays its cost cards for, and Unknown Armament waits before and after its effect. */
#define CGP_COST_REPEAT 4
#define CGP_EFFECT_DELAY 10
#define CGP_EFFECT_HOLD 15

/* Cursor modes CardCursorActive takes for the selectable cards of these commands. */
#define CGP_CURSOR_PICK 1

/*
 * Models and textures in the card game's arena: the card backdrop and its texture
 * (CardViewProc draws the same ones) and the cursor frame model that CardMainInit
 * loads from game\\Cur_WakuW.lex, which the prompt reuses as its frame.
 */
#define CGP_CARD_BACKDROP_MODEL ((void *) 0x01DA5600)
#define CGP_ANSWER_FRAME_MODEL ((void *) 0x01D12000)

#define CGP_ANSWER_PROMPT_X 0x7C
#define CGP_ANSWER_PROMPT_Y 0x82
#define CGP_ANSWER_HINT_X 0x18
#define CGP_ANSWER_HINT_Y 0x127

/*
 * Answer cards are drawn in a row: each one 0.5663 wide in x and 0.02 higher in z than
 * the one before, starting at -1.4193; the right-hand panel is shifted by half a pitch.
 */
#define CGP_ANSWER_CARD_PITCH 0.5663f
#define CGP_ANSWER_CARD_START_X (-1.4193f)
#define CGP_ANSWER_CARD_STEP_Z 0.02f
#define CGP_ANSWER_PANEL_SHIFT 0.28315f
#define CGP_ANSWER_ROW_Y 0.25f
#define CGP_ANSWER_CARD_SCALE 0.35f

/* Placement of the prompt's backdrop and of the card being answered. */
#define CGP_BACKDROP_Z 0.1f
#define CGP_BACKDROP_SCALE_Y 0.75f
#define CGP_MAIN_CARD_X (-1.9785f)
#define CGP_MAIN_CARD_Z 0.01f
#define CGP_MAIN_CARD_SCALE 0.4f

/* Cards in the second row face the other way: a half turn about z. */
#define CGP_HALF_TURN 3.1415927f

/*
 * The answer cards follow the card list at +0x148 of a side's hand: 160 halfwords
 * past hand->cards (CardPlayHand.answerCards).
 */
#define CGP_ANSWER_OFFSET 160
#define CGP_ANSWER_MAX 40

/* Steps of CGPCommFaseSub (command.phaseTimer). */
#define CGP_COMM_STEP_JUNK_CHECK 0
#define CGP_COMM_STEP_SORT_HAND 1
#define CGP_COMM_STEP_PICK 2
#define CGP_COMM_STEP_CONFIRM 3
#define CGP_COMM_STEP_OPERATION 4
#define CGP_COMM_STEP_RESTART 5
#define CGP_COMM_STEP_CONDITION 10
#define CGP_COMM_STEP_DISCARD 11
#define CGP_COMM_STEP_OFFER 12
#define CGP_COMM_STEP_SHOW 13
#define CGP_COMM_STEP_PLAY 14
#define CGP_COMM_STEP_AFTER_PLAY 15
#define CGP_COMM_STEP_ADVANCE 20
#define CGP_COMM_STEP_FOE_ANSWER 21
#define CGP_COMM_STEP_OWN_ANSWER 22
#define CGP_COMM_STEP_JUNK_ANSWERS 23
#define CGP_COMM_STEP_ANSWER_NOTE 24
#define CGP_COMM_STEP_FINISH 25
#define CGP_COMM_STEP_END_PHASE 30
#define CGP_COMM_STEP_NEXT_TURN 31
#define CGP_COMM_STEP_ENEMY_REQUEST 40
#define CGP_COMM_STEP_ENEMY_OPERATION 41

/* Messages of CGPCommFaseSub (CGPSetErrorMess / CGPDispErrorMess reasons). */
#define CGP_MESS_JUNK_COUNTED 0x24     /* card 14 takes the junk count */
#define CGP_ERR_NO_PLAYABLE_CARD 30
#define CGP_ERR_NOT_PLAYABLE 17        /* CGPChkPlayable refused the card */
#define CGP_ERR_NOT_DISCARDABLE 18     /* hand card without kind bit 1 */
#define CGP_ERR_SLOT_LOCKED 38         /* battle slot with flag 2 set */
#define CGP_ERR_WRONG_AREA 19
#define CGP_ERR_ANSWERS_DONE 29
#define CGP_MESS_ANSWER_OFFER 22       /* shown to the answering side */
#define CGP_MESS_ANSWER_NOTE 28

#define CGP_PAD_DISMISS 0x00F0         /* any face button closes the answer note */

#define CGP_COMMAND_CARD_Z 0.02f
#define CGP_COMMAND_CARD_SCALE 0.355f
#define CGP_DEGREES_HALF_TURN 180.0f
#define CGP_COMMAND_BACK_MODEL ((void *) 0x01D0C000)

/*
 * CardLayerStack.flags bit CGPBattleAfterEffect sets on an attacking stack; the
 * end phase clears it on every slot of the side.
 */

/*
 * An operation card with a duration below this counts down once per end phase;
 * higher durations never expire.
 */
#define CGP_OPE_PERMANENT_DURATION 64


/* CardSetEffect codes of the attack and damage effects: CardDispEffect plays codes 1-4 as a travelling strip
 * followed by a burst, 21-24 and 31-34 as the variants after one and several destroyed cards, and 11/15/16 as the
 * damage number for the same three cases. */
#define CGP_EFFECT_FRONT_ATTACK 1
#define CGP_EFFECT_PIERCE_ATTACK 2
#define CGP_EFFECT_SUPPORT_ATTACK 3    /* CardSetEffect turns code 23 into it; CardDispEffect plays it as the third strip */
#define CGP_EFFECT_DIRECT_ATTACK 4
#define CGP_EFFECT_BURST 5
#define CGP_EFFECT_FRONT_ATTACK_ONE_DESTROYED 21
#define CGP_EFFECT_PIERCE_ATTACK_ONE_DESTROYED 22
#define CGP_EFFECT_DIRECT_ATTACK_ONE_DESTROYED 24
#define CGP_EFFECT_DOUBLE_BURST 25     /* a burst that repeats once, 0.25 further along */
#define CGP_EFFECT_FRONT_ATTACK_MANY_DESTROYED 31
#define CGP_EFFECT_PIERCE_ATTACK_MANY_DESTROYED 32
#define CGP_EFFECT_DIRECT_ATTACK_MANY_DESTROYED 34
#define CGP_EFFECT_SWIRL 10            /* swirl around a card (strip CARD_EFFECT_STRIP_SWIRL) */
#define CGP_EFFECT_DAMAGE 11
#define CGP_EFFECT_NUMBER 12           /* number that rises over a card */
#define CGP_SE_REGEN 0x2000D

/* CardEffect.phase of the attack effects 1-4 (travel, burst, check whether the card was destroyed, fade), of
 * the delayed attack effects 21-24 and 31-34 (travel, hold, burst, check, fade) and of the damage numbers
 * 11 and 14-16 (wait, show). */
#define CGP_EFFECT_PHASE_TRAVEL 0
#define CGP_EFFECT_PHASE_BURST 1
#define CGP_EFFECT_PHASE_CHECK 2
#define CGP_EFFECT_PHASE_FADE 3
#define CGP_EFFECT_PHASE_DELAY 1
#define CGP_EFFECT_PHASE_DELAYED_BURST 2
#define CGP_EFFECT_PHASE_DELAYED_CHECK 3
#define CGP_EFFECT_PHASE_DELAYED_FADE 4
#define CGP_EFFECT_PHASE_WAIT 0
#define CGP_EFFECT_PHASE_SHOW 1

/* CardEffect.phase of CGP_EFFECT_NUMBER: where the number rises (y -1.81 at the bottom, 1.81 at the top) and which digit set it uses. */
#define CGP_NUMBER_BOTTOM 0
#define CGP_NUMBER_TOP 1
#define CGP_EFFECT_NUMBER_TOP 13       /* CardSetEffect turns it into CGP_EFFECT_NUMBER with phase 1: the number rises at the top of the screen (y 1.81) with the second digit set */
#define CGP_EFFECT_DAMAGE_LEFT 14      /* damage-style number whose digits sit left of the position (-0.35, -0.25) */
#define CGP_EFFECT_DAMAGE_ONE_DESTROYED 15
#define CGP_EFFECT_DAMAGE_MANY_DESTROYED 16


/*
 * Steps of CGPBattleFaseProc (command.phaseTimer), the values of its switch on the s16 phaseTimer.
 */
#define CGP_BATTLE_STEP_BEGIN 0            /* check that a battle can take place */
#define CGP_BATTLE_STEP_COMMAND 1          /* queued battle command, then the front attacks */
#define CGP_BATTLE_STEP_END 5              /* nothing (more) to fight: next turn */
#define CGP_BATTLE_STEP_FRONT 10           /* CGP_ATTACK_FRONT attacks, one slot per step */
#define CGP_BATTLE_STEP_FRONT_WAIT 11
#define CGP_BATTLE_STEP_PIERCE 20          /* CGP_ATTACK_PIERCE attacks */
#define CGP_BATTLE_STEP_PIERCE_WAIT 21
#define CGP_BATTLE_STEP_PIERCE_COMMAND 22  /* after the command of a CARD_ID_GARGOYLE card */
#define CGP_BATTLE_STEP_COUNTER 30         /* the foe's counter cards strike back */
#define CGP_BATTLE_STEP_COUNTER_ATTACK 31
#define CGP_BATTLE_STEP_COUNTER_WAIT 32
#define CGP_BATTLE_STEP_SUPPORT_CHARGE 40  /* support cards count down and show their effect */
#define CGP_BATTLE_STEP_SUPPORT 41         /* CGP_ATTACK_SUPPORT attacks on each foe slot */
#define CGP_BATTLE_STEP_SUPPORT_NEXT 42
#define CGP_BATTLE_STEP_SUPPORT_COMMAND 43 /* after the command of a CARD_ID_SIMEON card */
#define CGP_BATTLE_STEP_DIRECT 50          /* CGP_ATTACK_DIRECT attacks */
#define CGP_BATTLE_STEP_DIRECT_WAIT 51
#define CGP_BATTLE_STEP_FINISH 52
#define CGP_BATTLE_STEP_CLOSE 60
#define CGP_BATTLE_STEP_CLOSE_WAIT 61

/* Foe battle cards that strike back during this side's battle. */
/* Operation card in play on either side that stops the battle. */
/* Card whose attack is followed by its queued command. */
/* Support card that plays CARD_COMMAND_GARGOYLE_TRIGGER after its attacks. */
/* A support card's second layer below this life wears down after the attacks. */
#define CGP_SUPPORT_WEAR_LIMIT 80

/* Error messages of the phase. */
#define CGP_ERR_NO_BATTLE 6
#define CGP_ERR_BATTLE_BLOCKED 20
#define CGP_ERR_BATTLE_DONE 21
/* Value shown with CGP_ERR_BATTLE_BLOCKED while command.recoveryApplied is set. */

/* Frames the phase waits after an attack  and after a strong support effect. */
#define CGP_BATTLE_WAIT_FRAMES 30
#define CGP_BATTLE_SUPPORT_WAIT_FRAMES 45
/* Frames of the slide of the command window and of the closing wait. */
#define CGP_BATTLE_SLIDE_FRAMES 10

/* CardSetEffect codes and sound of a charged support card. Power 3 or more shows the strong effect. */
#define CGP_EFFECT_SUPPORT_STRONG 33
#define CGP_EFFECT_SUPPORT 23
#define CGP_SUPPORT_STRONG_POWER 3
#define CGP_SE_SUPPORT 0x20006

/* Interrupt message CGPEndFaseProc shows when the hand is over the limit ("Too many cards in your hand"). */
#define CGP_MESS_HAND_FULL 0x2B
/* Error message of the discard prompt ("Throw away this card?"). */
#define CGP_ERR_DISCARD_CONFIRM 25
/* Frames the hand list stays inert after it opens. */
#define CGP_DISCARD_INPUT_DELAY 25

/* Where the enlarged card of the discard selection is drawn. */
#define CGP_DISCARD_PREVIEW_Y 0.369f
#define CGP_DISCARD_PREVIEW_Z 0.3f

/* Steps of CGPEndFaseProc (command.phaseTimer). */
#define CGP_END_STEP_BEGIN 0
#define CGP_END_STEP_DRAW 1
#define CGP_END_STEP_HAND_CHECK 2
#define CGP_END_STEP_SELECT 3
#define CGP_END_STEP_CONFIRM 4
#define CGP_END_STEP_NEXT_TURN 10

/* Halfword index of command.input[0].held in CardGameWork (+0x5AD0 / 2). The pad-flip loop walks the work area
 * as halfwords: pointers formed as &work->command.input[n].held + k make GCC keep both words of a pass in one
 * base register and drop four instructions of the byte-offset induction. */
#define CARD_PAD_INPUT0_WORD 0x2D68
#define CARD_CMD_INPUT_WORDS (sizeof(CardCommandInput) / sizeof(u16))
/* 0xC000 sign-extended: the loop keeps it in $9 ahead of the work + 8 base in $10. */
#define CARD_PAD_UP_PAIR (-0x4000)

/*
 * CARD_PAD_FLIP(pad, mask, turned): flips the d-pad bits of the halfword `*pad` (two statements,
 * `turned` is a u16 temporary, `mask` is the 0xC000 pair as the sign-extended register constant the
 * loop keeps, see CARD_PAD_UP_PAIR). The 0xF000 d-pad nibble moves: the 0xC000 bit pair shifts left
 * by 2, the 0x3000 pair shifts right by 2, and the low 12 bits stay.
 */
#define CARD_PAD_FLIP(pad, mask, turned)                                     \
    turned = ((*(pad) & 0xF000) << 2) & mask;                                 \
    *(pad) = (((*(pad) & 0xF000) >> 2) & 0x3000) | (turned | (*(pad) & 0x0FFF))

/* Situation cards CardGameProc checks on either side. D.S.S.S., Compulsory Sortie and Override Code refuse the end of
 * a turn while the turn's side still has a card to place and a disposal card that is a Gnosis (D.S.S.S.), a Realian
 * (Override Code) or any disposal card (Compulsory Sortie), waiting. Analyze shows the hands. */

/* Buttons that dismiss the error message window. */
#define CGP_PAD_FACE_BUTTONS 0x00F0
/* Button that asks for the phase change. */
#define CGP_PAD_SQUARE 0x0080
/* Start button of either pad, which opens the menu. */
#define CGP_PAD_START 0x0800
/* Button that turns the stage. */
#define CGP_PAD_ROTATE_STAGE 0x0001
/* CardGameWork.flags bit that hides the hands. */
#define CGP_FLAG_HIDE_HANDS 0x80
/* Fixed EE runtime address of the model the error window of CardGameProc enters after the texture. */
#define CGP_GAME_MESS_MODEL ((void *) 0x01DA8100)
#define PI_F 3.1415927f

/*
 * Partial view of CardDefinition. The canonical record carries u32 text offsets, which give it word
 * alignment; GCC then folds the kind and flags tests of CGPCFPlayCommOperation into one word access,
 * while this TU's code keeps a byte load and a halfword load. Nested ifs or a local CardDefinition
 * pointer keep the two loads but stop the loop from being strength reduced.
 */
typedef struct CardDefinitionView {
    u8 kind;
    u8 unmodeled_01;
    u16 flags;
    u8 unmodeled_04[0x20];
} CardDefinitionView;

/* Movement record fields used by the slot swap. */
typedef struct CardMoveLayer {
    u8 powerPlus;
    u8 preservedByte01;
    u8 boostCount;
    u8 life;
    u8 preservedByte04;
    u8 damageTimer;
    s16 cardId;
    u8 unmodeled_08[2];
} CardMoveLayer;

typedef struct CardMoveWork {
    u8 flags;
    u8 moveStep;
    u8 power;
    u8 powerLimit;
    u8 unmodeled_04[4];
    CardMoveLayer layers[40];
    float position[3];
    float velocity[3];
    float shuntPosition[3];
} CardMoveWork;

typedef unsigned int Quadword __attribute__((mode(TI)));

/* The placement matrix as floats, or as four quadwords for the per-effect copy. */
typedef union CardEffectMatrix {
    float m[4][4];
    Quadword q[4];
} CardEffectMatrix;

/*
 * A battle slot as the command code walks it, from the card stack on: the
 * stack, then the shunt position, 0x1BC bytes in all (partial view of
 * CardBoardSlot). Indexing a CardBoardSlot array instead puts the slot's 12 byte
 * head into every address and moves the base register.
 */
typedef struct CGPStackSlot {
    CardLayerStack stack;
    float shuntPosition[3];
} CGPStackSlot;


/* Defined in this translation unit, used before its definition. */
extern s32 CGPCursor2Area(CardGameWork *work);

/* Defined here: data that is read before its definition. */
extern char PermMess000Txt[];
extern char PermMess001Txt[];
extern char PermMess002Txt[];
extern char PermMess003Txt[];
extern char PermMess004Txt[];
extern char PermMess005Txt[];
extern char PermMess006Txt[];
extern char PermMess007Txt[];
extern char PermMess008Txt[];
extern char PermMess009Txt[];
extern char PermMess010Txt[];
extern char PermMess011Txt[];
extern char PermMess012Txt[];
extern char PermMess013Txt[];
extern char PermMess014Txt[];
extern const char cgpPrintYamaFormat[];
extern const char cgpPrintCardFormat[];
extern const char cgpPrintNewline[];
extern const char cgpPrintJunkFormat[];
extern const char cgpPrintSuteFormat[];
extern const char cgpMessFontCode[8];
extern const char cgpMessColorCode[8];
extern float *CursorTbl[32];
extern u8 HelpPageList[14];
extern const char cgpDiscardTwoPrompt[];
extern const char cgpDiscardOnePrompt[];
extern const char cgpBlockEventPrompt[];
extern const char cgpBlockFurtherPrompt[];
extern const char cgpBlockSelectCardHint[];
extern char EMTitle03Txt[24];
extern char EMMess072Txt[32];
extern char EMMess016Txt[32];
extern char EMMess060Txt[32];
extern char EMTitle90Txt[24];
extern char EMMess091Txt[32];
extern char EMMess092Txt[32];
extern char EMMess102Txt[40];
extern char EMTitle95Txt[24];
extern char EMMess018Txt[32];
extern char EMMess084Txt[40];
extern char EMMess902Txt[];
extern char EMMess062Txt[40];
extern char EMMess991Txt[16];
extern char EMMess017Txt[32];
extern char EMMess057Txt[8];
extern char EMTitle93Txt[24];
extern char EMMess078Txt[32];
extern char EMMess900Txt[24];
extern char EMMess103Txt[8];
extern char EMMess001Txt[24];
extern char EMMess070Txt[32];
extern char EMMess020Txt[40];
extern char EMMess014Txt[40];
extern char EMMess012Txt[40];
extern char EMMess000Txt[32];
extern char EMMess052Txt[40];
extern char EMMess013Txt[24];
extern char EMMess074Txt[32];
extern char EMMess110Txt[40];
extern char EMMess073Txt[24];
extern char EMMess111Txt[24];
extern char EMMess094Txt[40];
extern char EMMess054Txt[24];
extern char EMMess077Txt[32];
extern char EMMess063Txt[32];
extern char EMMess061Txt[32];
extern char EMMess010Txt[32];
extern char EMMess040Txt[32];
extern char EMTitle02Txt[24];
extern char EMMess910Txt[24];
extern char EMMess056Txt[32];
extern char EMMess080Txt[24];
extern char EMMess093Txt[32];
extern char EMTitle01Txt[24];
extern char EMMess041Txt[32];
extern char EMTitle00Txt[24];
extern char EMTitle99Txt[24];
extern char EMMess030Txt[32];
extern char EMMess076Txt[8];
extern char EMMess990Txt[16];
extern char EMTitle94Txt[24];
extern char EMMess903Txt[];
extern char EMMess050Txt[32];
extern char EMMess101Txt[8];
extern char EMTitle04Txt[32];
extern char EMMess015Txt[24];
extern char EMTitle92Txt[24];
extern char EMMess071Txt[24];
extern char EMMess090Txt[24];
extern char EMMess901Txt[];
extern char EMMess083Txt[40];
extern char EMMess031Txt[24];
extern char EMMess082Txt[24];
extern char EMMess053Txt[32];
extern char EMTitle91Txt[24];
extern char EMMess055Txt[32];
extern char EMMess051Txt[32];
extern char EMMess019Txt[24];
extern char EMMess100Txt[40];
extern char EMMess075Txt[40];
extern char EMMess011Txt[24];
extern char EMMess081Txt[40];

static char OLMess0000Txt[8];
static char OLMess0010Txt[32];
static char OLMess0011Txt[48];
static char OLMess0020Txt[56];
static char OLMess0030Txt[48];
static char OLMess0040Txt[56];
static char OLMess0041Txt[40];
static char OLMess0042Txt[40];
static char OLMess0050Txt[32];
static char OLMess0060Txt[32];
static char OLMess0061Txt[48];
static char OLMess0070Txt[56];
static char OLMess0080Txt[48];
static char OLMess0090Txt[56];


static void *CPISMdlLst[14] = {
    CARD_MODEL_AT(0xA800), CARD_MODEL_AT(0xB800), CARD_MODEL_AT(0xD800),
    CARD_MODEL_AT(0xC800), CARD_MODEL_AT(0x10800), CARD_MODEL_AT(0xE800),
    CARD_MODEL_AT(0xF800), CARD_MODEL_AT(0x1A800), CARD_MODEL_AT(0x1B800),
    CARD_MODEL_AT(0x1D800), CARD_MODEL_AT(0x1C800), CARD_MODEL_AT(0x20800),
    CARD_MODEL_AT(0x1E800), CARD_MODEL_AT(0x1F800)
};

static void *CPIPhaseMdlLst[14] = {
    CARD_MODEL_AT(0x22800), CARD_MODEL_AT(0x23800), CARD_MODEL_AT(0x25800),
    CARD_MODEL_AT(0x24800), CARD_MODEL_AT(0x26800), CARD_MODEL_AT(0x27800),
    CARD_MODEL_AT(0x28800), CARD_MODEL_AT(0x22800), CARD_MODEL_AT(0x23800),
    CARD_MODEL_AT(0x25800), CARD_MODEL_AT(0x24800), CARD_MODEL_AT(0x26800),
    CARD_MODEL_AT(0x27800), CARD_MODEL_AT(0x28800)
};

float Player1Color[4][4] = {
    {0.0f, 0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
    {0.8f, 0.8f, 1.2f, 1.0f}
};

float Player2Color[4][4] = {
    {0.0f, 0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
    {1.2f, 0.8f, 0.8f, 1.0f}
};

char *PermMessTbl[16] = {
    PermMess000Txt, PermMess001Txt, PermMess002Txt, PermMess003Txt, PermMess004Txt, PermMess005Txt, PermMess006Txt, PermMess007Txt, PermMess008Txt, PermMess009Txt, PermMess010Txt, PermMess011Txt, PermMess012Txt, PermMess013Txt, PermMess014Txt, PermMess000Txt
};

char *OLMessTbl[16] = {
    OLMess0010Txt, OLMess0020Txt, OLMess0040Txt, OLMess0030Txt, OLMess0042Txt, OLMess0050Txt, OLMess0000Txt, OLMess0060Txt, OLMess0070Txt, OLMess0090Txt, OLMess0080Txt, OLMess0041Txt, OLMess0050Txt, OLMess0000Txt, OLMess0011Txt, OLMess0061Txt
};

static s16 CGPCPTbl[9] = { 0x003F, 0x0040, 0x0041, 0x0083, 0x0085, 0x008B, 0x008D, 0x008E, 0x008F };

unsigned char SampleDeck0[40] = {
    0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x04, 0x04, 0x93, 0x93, 0x93, 0x54, 0x93, 0x00, 0x21, 0x32, 0x03, 0x00, 0x21, 0x02
};

unsigned char SampleDeck1[40] = {
    0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x93, 0x16, 0x16, 0x1B, 0x1C, 0x1C, 0x1C, 0x1B, 0x17, 0x16
};

unsigned char SampleDeck2[40] = {
    0x13, 0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x16, 0x16, 0x16, 0x18, 0x1B, 0x1B, 0x1B, 0x1C, 0x1C, 0x1C, 0x31, 0x31, 0x31, 0x32, 0x32, 0x32, 0x57, 0x57, 0x57, 0x6A, 0x6A, 0x6A, 0x6D, 0x6D, 0x6D, 0x11, 0x81, 0x17, 0x17, 0x17, 0x18, 0x18, 0x18
};

unsigned char SampleDeck3[40] = {
    0x00, 0x00, 0x00, 0x04, 0x04, 0x04, 0x07, 0x07, 0x0F, 0x0F, 0x0F, 0x11, 0x11, 0x11, 0x12, 0x12, 0x1F, 0x1F, 0x1F, 0x21, 0x21, 0x21, 0x24, 0x24, 0x24, 0x27, 0x27, 0x27, 0x2B, 0x2B, 0x2B, 0x2C, 0x2C, 0x2C, 0x2D, 0x2D, 0x2D, 0x56, 0x56, 0x56
};

unsigned char SampleDeck4[40] = {
    0x00, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x14, 0x14, 0x14, 0x43, 0x43, 0x43, 0x45, 0x45, 0x45, 0x4A, 0x4A, 0x4A, 0x4B, 0x4B, 0x4B, 0x58, 0x58, 0x58, 0x5A, 0x5A, 0x5A, 0x61, 0x61, 0x61, 0x72, 0x72, 0x72
};

unsigned char SampleDeck5[40] = {
    0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x0F, 0x0F, 0x0F, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x16, 0x16, 0x16, 0x1E, 0x1E, 0x1F, 0x1F, 0x24, 0x24, 0x24, 0x32, 0x32, 0x32, 0x33, 0x33, 0x33, 0x7B, 0x7B, 0x7B
};

unsigned char SampleDeck6[40] = {
    0x00, 0x00, 0x04, 0x04, 0x07, 0x0F, 0x0F, 0x14, 0x14, 0x15, 0x16, 0x16, 0x16, 0x1E, 0x1E, 0x1E, 0x1F, 0x1F, 0x20, 0x23, 0x25, 0x27, 0x2C, 0x2C, 0x2C, 0x52, 0x52, 0x52, 0x55, 0x55, 0x56, 0x56, 0x5B, 0x5B, 0x5D, 0x5D, 0x5D, 0x78, 0x7A, 0x7A
};

unsigned char SampleDeck7[40] = {
    0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x14, 0x14, 0x14, 0x1F, 0x1F, 0x24, 0x24, 0x24, 0x38, 0x38, 0x38, 0x56, 0x56, 0x56, 0x57, 0x57, 0x57, 0x58, 0x6A, 0x6A, 0x6A, 0x6D, 0x6D, 0x6D, 0x80, 0x80, 0x8C, 0x8C
};

unsigned char SampleDeck8[40] = {
    0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x76, 0x76, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x76, 0x76
};

unsigned char SampleDeck9[40] = {
    0x10, 0x10, 0x10, 0x16, 0x5F, 0x5F, 0x5F, 0x5F, 0x18, 0x18, 0x18, 0x37, 0x37, 0x37, 0x39, 0x39, 0x39, 0x3A, 0x3A, 0x3A, 0x3E, 0x3E, 0x3E, 0x3D, 0x3D, 0x3D, 0x1D, 0x1D, 0x1D, 0x5B, 0x74, 0x74, 0x65, 0x76, 0x18, 0x0C, 0x0D, 0x8B, 0x8B, 0x8B
};


unsigned char *EnemyDeckLst[CPU_DECK_COUNT] = {0};

unsigned char *DeckDataLst[CPU_DECK_COUNT] = {0};

unsigned char *DeckGeneLst[CPU_DECK_COUNT] = {0};

unsigned char *EnemyDeckListStr[CPU_DECK_COUNT] = {0};

CpuDeckRecord cpudeck[CPU_DECK_COUNT] = {{0}};

unsigned char CardDat[0x100] = {
    0x3C, 0x1E, 0x3C, 0x37, 0x50, 0x50, 0x41, 0x64, 0x1E, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x3C, 0x1E, 0x1E, 0x1E, 0x1E, 0x32, 0x32, 0x4B, 0x14, 0x14, 0x14, 0x14, 0x28, 0x28, 0x0F, 0x50, 0x69, 0x82, 0x4B, 0x6E, 0x64, 0x46, 0x4F, 0x50, 0x50, 0x50, 0x50, 0x1E, 0x3C, 0x19, 0x32, 0x32, 0x14, 0x0A, 0x1E, 0x28, 0x32, 0x32, 0x32, 0x64, 0x3C, 0x64, 0x32, 0x32, 0x32, 0x46, 0x46, 0x32, 0x2D, 0x1E, 0x2D, 0x3C, 0x32, 0x46, 0x32, 0x3C, 0x3C, 0x6E, 0x3C, 0x64, 0x5A, 0x8C, 0x3C, 0x50, 0x32, 0x64, 0x37, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};





















/*
 * Copies each occupied slot's effect position into the shunt position of the next slot,
 * the disposal board first and then the battle board. CardEnemyMove calls it before the
 * computer opponent moves.
 */
void CGPShuntPosSub(CardPlaySide *side)
{
    s32 i;

    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        if (side->disposal[i].battle.layers[0].cardId >= 0) {
            side->disposal[i + 1].shuntPosition[0] = side->disposal[i].battle.position[0];
            side->disposal[i + 1].shuntPosition[1] = side->disposal[i].battle.position[1];
            side->disposal[i + 1].shuntPosition[2] = side->disposal[i].battle.position[2];
        }
    }

    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        if (side->battle[i].battle.layers[0].cardId >= 0) {
            side->battle[i + 1].shuntPosition[0] = side->battle[i].battle.position[0];
            side->battle[i + 1].shuntPosition[1] = side->battle[i].battle.position[1];
            side->battle[i + 1].shuntPosition[2] = side->battle[i].battle.position[2];
        }
    }
}


/*
 * Retargets the cards in play after the board shifted: each occupied slot keeps its old
 * destination, takes the shunt position of the next slot as its new position, and steps
 * there over ten frames.
 */
void CGPRecalcPosSub(CardPlaySide *side)
{
    CardLayerStack *stack;
    float dest[3];
    s32 i;

    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        stack = &side->disposal[i].battle;
        if (stack->layers[0].cardId >= 0) {
            dest[0] = stack->position[0] + stack->step[0] * stack->moveFrames;
            dest[1] = stack->position[1] + stack->step[1] * stack->moveFrames;
            dest[2] = stack->position[2] + stack->step[2] * stack->moveFrames;
            stack->position[0] = side->disposal[i + 1].shuntPosition[0];
            stack->position[1] = side->disposal[i + 1].shuntPosition[1];
            stack->position[2] = side->disposal[i + 1].shuntPosition[2];
            stack->step[0] = (dest[0] - stack->position[0]) / 10.0f;
            stack->step[1] = (dest[1] - stack->position[1]) / 10.0f;
            stack->step[2] = (dest[2] - stack->position[2]) / 10.0f;
        }
    }
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        stack = &side->battle[i].battle;
        if (stack->layers[0].cardId >= 0) {
            dest[0] = stack->position[0] + stack->step[0] * stack->moveFrames;
            dest[1] = stack->position[1] + stack->step[1] * stack->moveFrames;
            dest[2] = stack->position[2] + stack->step[2] * stack->moveFrames;
            stack->position[0] = side->battle[i + 1].shuntPosition[0];
            stack->position[1] = side->battle[i + 1].shuntPosition[1];
            stack->position[2] = side->battle[i + 1].shuntPosition[2];
            stack->step[0] = (dest[0] - stack->position[0]) / 10.0f;
            stack->step[1] = (dest[1] - stack->position[1]) / 10.0f;
            stack->step[2] = (dest[2] - stack->position[2]) / 10.0f;
        }
    }
}


/*
 * Unknown Armament takes two life off every card on a battle or disposal slot of either side,
 * clamped at zero, and starts effects 5 and 13 on each of those stacks. Effects of the foe's
 * cards are played for the other side.
 */
void CCO143ExecSub(s32 side, CardGameWork *work, CardPlaySide *ownSide,
                   CardPlaySide *foeSide)
{
    s32 otherSide;
    s32 i;

    xglSoundEffectNormalID(0x20008, 0);
    otherSide = (side == 0);
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        if (ownSide->battle[i].battle.layers[0].cardId >= 0) {
            ownSide->battle[i].battle.layers[0].life -= 2;
            if (ownSide->battle[i].battle.layers[0].life < 0) {
                ownSide->battle[i].battle.layers[0].life = 0;
            }
            CardSetEffect(side, work, CGP_EFFECT_BURST,0, 0, ownSide->battle[i].battle.position, 0);
            CardSetEffect(side, work, 13, 2, 0, ownSide->battle[i].battle.position,
                          &ownSide->battle[i].battle);
        }
        if (foeSide->battle[i].battle.layers[0].cardId >= 0) {
            foeSide->battle[i].battle.layers[0].life -= 2;
            if (foeSide->battle[i].battle.layers[0].life < 0) {
                foeSide->battle[i].battle.layers[0].life = 0;
            }
            CardSetEffect(otherSide, work, CGP_EFFECT_BURST,0, 0, foeSide->battle[i].battle.position, 0);
            CardSetEffect(otherSide, work, 13, 2, 0, foeSide->battle[i].battle.position,
                          &foeSide->battle[i].battle);
        }
    }
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        if (ownSide->disposal[i].battle.layers[0].cardId >= 0) {
            ownSide->disposal[i].battle.layers[0].life -= 2;
            if (ownSide->disposal[i].battle.layers[0].life < 0) {
                ownSide->disposal[i].battle.layers[0].life = 0;
            }
            CardSetEffect(side, work, CGP_EFFECT_BURST,0, 0, ownSide->disposal[i].battle.position, 0);
            CardSetEffect(side, work, 13, 2, 0, ownSide->disposal[i].battle.position,
                          &ownSide->disposal[i].battle);
        }
        if (foeSide->disposal[i].battle.layers[0].cardId >= 0) {
            foeSide->disposal[i].battle.layers[0].life -= 2;
            if (foeSide->disposal[i].battle.layers[0].life < 0) {
                foeSide->disposal[i].battle.layers[0].life = 0;
            }
            CardSetEffect(otherSide, work, CGP_EFFECT_BURST,0, 0, foeSide->disposal[i].battle.position, 0);
            CardSetEffect(otherSide, work, 13, 2, 0, foeSide->disposal[i].battle.position,
                          &foeSide->disposal[i].battle);
        }
    }
}


/* Prints a side's deck, ten card ids per line, each row headed by its first index. */
void CGPPrintYama(CardPlaySide *side)
{
    s32 col;
    s32 i;

    col = 0;
    for (i = 0; i < 40; i++) {
        if (col == 0) {
            printf(cgpPrintYamaFormat, i);
        }
        col++;
        printf(cgpPrintCardFormat, side->deck[i]);
        if (col == 10) {
            col = 0;
            printf(cgpPrintNewline);
        }
    }
    printf(cgpPrintNewline);
}

/* Prints a side's junk pile, ten card ids per line, each row headed by its first index. */
void CGPPrintJunk(CardPlaySide *side)
{
    s32 col;
    s32 i;

    col = 0;
    for (i = 0; i < 40; i++) {
        if (col == 0) {
            printf(cgpPrintJunkFormat, i);
        }
        col++;
        printf(cgpPrintCardFormat, side->junk[i]);
        if (col == 10) {
            col = 0;
            printf(cgpPrintNewline);
        }
    }
    printf(cgpPrintNewline);
}





/* Prints a side's sute pile, ten card ids per line, each row headed by its first index. */
void CGPPrintSute(CardPlaySide *side)
{
    s32 col;
    s32 i;

    col = 0;
    for (i = 0; i < 40; i++) {
        if (col == 0) {
            printf(cgpPrintSuteFormat, i);
        }
        col++;
        printf(cgpPrintCardFormat, side->sute[i]);
        if (col == 10) {
            col = 0;
            printf(cgpPrintNewline);
        }
    }
    printf(cgpPrintNewline);
}

void CGPSetMessage(CardGameWork *work, s32 index)
{
    work->mess = OLMessTbl[index];
}

void CGPSetInterruptMess(CardGameWork *work, s32 index)
{
    work->interruptMess = CardPlayTMessList[index];
}

void CGPSetPermanentMess(CardGameWork *work, s32 index)
{
    if (index == 0) {
        work->permanentMess = 0;
        return;
    }
    work->permanentMess = PermMessTbl[index];
}

void CGPSetErrorMess(s32 side, CardGameWork *work, s32 reason)
{
    work->errorMessSide = side;
    work->errorMessKind = 1;
    work->errorMessReason = reason;
    work->errorMessValue = -1;
}

void CGPSetErrorMessPlus(s32 side, CardGameWork *work, u8 reason, s16 value)
{
    work->errorMessSide = side;
    work->flags |= CGP_FLAG_ERROR_PLUS;
    work->errorMessKind = 3;
    work->errorMessReason = reason;
    work->errorMessValue = value;
}





/*
 * Draws the lines of one ErrorMessTbl row, then the message window model at the given
 * position, scaled 1.35 across and lit for the side that caused the message (a negative
 * side keeps the current light).
 */
void CGPDispErrorMessCore(s32 side, u16 reason, u16 x, u16 y, const float position[4])
{
    float matrix[4][4];
    float scale[4];

    xglFontPrint(x, y, CGP_TEXT_COLOR, cgpMessFontCode);
    xglFontPrint(0, 0, 0, cgpMessColorCode);
    xglFontPrint(x, y, CGP_TEXT_COLOR, ErrorMessTbl[reason][0]);
    if (ErrorMessTbl[reason][1] != 0) {
        xglFontPrint(x, y + 1 * CGP_ERROR_MESS_LINE_PITCH, CGP_TEXT_COLOR, cgpMessFontCode);
        xglFontPrint(0, 0, 0, cgpMessColorCode);
        xglFontPrint(x, y + 1 * CGP_ERROR_MESS_LINE_PITCH, CGP_TEXT_COLOR, ErrorMessTbl[reason][1]);
    }
    if (ErrorMessTbl[reason][2] != 0) {
        xglFontPrint(x, y + 2 * CGP_ERROR_MESS_LINE_PITCH, CGP_TEXT_COLOR, cgpMessFontCode);
        xglFontPrint(0, 0, 0, cgpMessColorCode);
        xglFontPrint(x, y + 2 * CGP_ERROR_MESS_LINE_PITCH, CGP_TEXT_COLOR, ErrorMessTbl[reason][2]);
    }
    if (ErrorMessTbl[reason][3] != 0) {
        xglFontPrint(x, y + 3 * CGP_ERROR_MESS_LINE_PITCH, CGP_TEXT_COLOR, cgpMessFontCode);
        xglFontPrint(0, 0, 0, cgpMessColorCode);
        xglFontPrint(x, y + 3 * CGP_ERROR_MESS_LINE_PITCH, CGP_TEXT_COLOR, ErrorMessTbl[reason][3]);
    }
    if (ErrorMessTbl[reason][4] != 0) {
        xglFontPrint(x, y + 4 * CGP_ERROR_MESS_LINE_PITCH, CGP_TEXT_COLOR, cgpMessFontCode);
        xglFontPrint(0, 0, 0, cgpMessColorCode);
        xglFontPrint(x, y + 4 * CGP_ERROR_MESS_LINE_PITCH, CGP_TEXT_COLOR, ErrorMessTbl[reason][4]);
    }
    xglMatrixUnit(matrix);
    xglMatrixTrans(matrix, matrix, position);
    scale[0] = 1.35f;
    scale[1] = scale[2] = scale[3] = 1.0f;
    xglMatrixScale(matrix, matrix, scale);
    nmlModelSetPlace(&matrix);
    if (side >= 0) {
        if (side == 0) {
            nmlModelSetLight(&Player1Color, &asDir);
        } else {
            nmlModelSetLight(&Player2Color, &asDir);
        }
    }
    nmlModelSetTexture(CGP_PANEL_TEXTURE);
    nmlModelEntryCard(CGP_ERROR_MESS_MODEL);
}

/* Shows the error message at the right-hand screen position (x 1.1). */
void CGPDispErrorMess3(s32 side, s32 reason)
{
    float position[4];

    position[0] = 1.1f;
    position[1] = 0.0f;
    position[2] = 0.5f;
    position[3] = 1.0f;
    CGPDispErrorMessCore(side, reason, 230, 164, position);
}

/*
 * Shows the message of CGPDispErrorMess3 and the card it is about, at 0.8 scale, in the
 * lower left corner of that line. CardDisp's last two arguments are the owned flag (1) and
 * the display mode (5).
 */
void CGPDispErrorMessPlus(s32 side, u16 reason, s32 cardId)
{
    float matrix[4][4];
    float vec[4];

    CGPDispErrorMess3(side, reason);
    xglMatrixUnit(matrix);
    vec[0] = -1.5f;
    vec[1] = 0.2f;
    vec[2] = 0.3f;
    vec[3] = 1.0f;
    xglMatrixTrans(matrix, matrix, vec);
    vec[0] = vec[1] = 0.8f;
    vec[2] = 1.0f;
    xglMatrixScale(matrix, matrix, vec);
    CardDisp(&matrix, cardId, 1, 5);
}

/* Shows the error message at the standard position for the side. */
void CGPDispErrorMess(s32 side, s32 reason)
{
    float position[4];

    position[0] = 0.0f;
    position[1] = 0.0f;
    position[2] = 0.5f;
    position[3] = 1.0f;
    CGPDispErrorMessCore(side, reason, 124, 164, position);
}

/* Shows the error message at the second fixed position (y 0.48). */
void CGPDispErrorMess2(s32 side, s32 reason)
{
    float position[4];

    position[0] = 0.0f;
    position[1] = 0.48f;
    position[2] = 0.5f;
    position[3] = 1.0f;
    CGPDispErrorMessCore(side, reason, 124, 116, position);
}

/*
 * Draws the message line, the error message, the information panel with the turn counter,
 * and the phase guide of the side whose phase is running. Phases 0..6 belong to the player
 * and 7..13 to the opponent; a negative phase means no side is playing.
 */
void CardPlayDispInfo(CardGameWork *workAddress)
{
    CardGameWork *work = workAddress;
    float place[4][4];
    float guidePlace[4][4];
    float shadowPos[4];
    float pos[4];
    float guideScale[4];
    float color1P[4][4];
    float color2P[4][4];
    float turnColor[4][4];
    char *text;
    s32 flag;
    void *model;
    s32 area;
    float transparency;
    s32 tens;
    s32 i;

    /* The interrupt and permanent messages replace the base message. */
    text = work->mess;
    if (work->interruptMess != 0) {
        text = work->interruptMess;
    }
    if (work->permanentMess != 0) {
        text = work->permanentMess;
    }

    /* Error message for the side that caused it, or for both sides when that side plays automatically. */
    if (work->errorMessKind != 0) {
        s32 bothSides;

        bothSides = 0;
        if (work->errorMessSide == 0) {
            if (work->flags & CGP_FLAG_SIDE0_AUTO) {
                bothSides = 1;
            }
        } else {
            if (work->flags & CGP_FLAG_SIDE1_AUTO) {
                bothSides = 1;
            }
        }
        if (bothSides == 0) {
            if (work->errorMessKind & CGP_FLAG_ERROR_PLUS) {
                CGPDispErrorMessPlus(work->errorMessSide, work->errorMessReason,
                                     work->errorMessValue);
            } else {
                CGPDispErrorMess(work->errorMessSide,
                                 work->errorMessReason);
            }
        } else if (work->errorMessKind & CGP_FLAG_ERROR_PLUS) {
            CGPDispErrorMessPlus(-1, work->errorMessReason, work->errorMessValue);
        } else {
            CGPDispErrorMess(-1, work->errorMessReason);
        }
    }

    flag = 1;
    if (work->infoPanelPhase == CGP_INFO_PANEL_ZOOM) {
        flag = work->resultWait < 4;
    }
    switch (work->command.phase) {
    case CARD_PHASE_EVENT:
    case CARD_PHASE_BLOCK:
    case CARD_PHASE_EVENT + CARD_PHASES_PER_SIDE:
    case CARD_PHASE_BLOCK + CARD_PHASES_PER_SIDE:
        if ((u32) (work->command.phaseTimer - 20) < 10) {
            flag = 0;
        }
        break;
    }
    if (flag && text != 0) {
        xglFontPrint(2, 2, CGP_TEXT_COLOR, cgpMessFontCode);
        xglFontPrint(0, 0, 0, cgpMessColorCode);
        xglFontPrint(2, 2, CGP_TEXT_COLOR, text);
    }

    model = CPISMdlLst[work->command.phase];
    flag = 1;
    if (work->command.phase < CARD_PHASES_PER_SIDE) {
        if (work->command.phase >= 0) {
            flag = 0;
        }
    }
    /* Animation step of the information panel. */
    switch (work->infoPanelPhase) {
    case CGP_INFO_PANEL_INIT:
    default:
        work->infoPanelPos[0] = 0.0f;
        work->infoPanelPos[1] = 0.0f;
        work->infoPanelPos[2] = 0.5f;
        work->infoPanelPos[3] = 1.0f;
        work->infoPanelPosStep[0] = 0.0f;
        work->infoPanelPosStep[1] = 0.0f;
        work->infoPanelPosStep[2] = 0.0f;
        work->infoPanelPosStep[3] = 1.0f;
        work->infoPanelScale[0] = 20.0f;
        work->infoPanelScale[1] = 20.0f;
        work->infoPanelScale[2] = 1.0f;
        work->infoPanelScale[3] = 1.0f;
        work->infoPanelScaleStep[0] = -1.9f;
        work->infoPanelScaleStep[1] = -1.9f;
        work->infoPanelScaleStep[2] = 0.0f;
        work->infoPanelScaleStep[3] = 1.0f;
        work->resultWait = 10;
        work->infoPanelPhase = CGP_INFO_PANEL_ZOOM;
        break;
    case CGP_INFO_PANEL_ZOOM:
        if (work->resultWait <= 0) {
            work->resultWait = 15;
            work->infoPanelPhase++;
        } else {
            work->resultWait--;
            work->infoPanelScale[0] += work->infoPanelScaleStep[0];
            work->infoPanelScale[1] += work->infoPanelScaleStep[1];
            work->infoPanelScale[2] += work->infoPanelScaleStep[2];
            work->infoPanelPos[0] += work->infoPanelPosStep[0];
            work->infoPanelPos[1] += work->infoPanelPosStep[1];
            work->infoPanelPos[2] += work->infoPanelPosStep[2];
        }
        break;
    case CGP_INFO_PANEL_HOLD:
        if (work->resultWait <= 0) {
            work->infoPanelPhase++;
            work->resultWait = 6;
            if (work->command.phase < CARD_PHASES_PER_SIDE) {
                if (work->command.phase >= 0) {
                    work->infoPanelPosStep[0] = -0.25833334f;
                    work->infoPanelPosStep[1] = 0.27166668f;
                } else {
                    work->infoPanelPosStep[0] = 0.25833334f;
                    work->infoPanelPosStep[1] = -0.305f;
                }
            } else {
                work->infoPanelPosStep[0] = 0.25833334f;
                work->infoPanelPosStep[1] = -0.305f;
            }
            work->infoPanelScaleStep[0] = -0.083333336f;
            work->infoPanelPosStep[2] = -0.033333333f;
            work->infoPanelScaleStep[1] = -0.083333336f;
        } else {
            work->resultWait--;
        }
        break;
    case CGP_INFO_PANEL_SLIDE:
        if (work->resultWait <= 0) {
            work->infoPanelPhase++;
        } else {
            work->resultWait--;
            work->infoPanelPos[0] += work->infoPanelPosStep[0];
            work->infoPanelPos[1] += work->infoPanelPosStep[1];
            work->infoPanelPos[2] += work->infoPanelPosStep[2];
            work->infoPanelScale[0] += work->infoPanelScaleStep[0];
            work->infoPanelScale[1] += work->infoPanelScaleStep[1];
            work->infoPanelScale[2] += work->infoPanelScaleStep[2];
        }
        break;
    case CGP_INFO_PANEL_SETTLE:
        if (work->resultWait > 0) {
            work->resultWait--;
            work->infoPanelPos[0] += work->infoPanelPosStep[0];
            work->infoPanelPos[1] += work->infoPanelPosStep[1];
        }
        break;
    }

    if ((work->command.phase == CARD_PHASE_BATTLE || work->command.phase == CARD_PHASE_BATTLE + CARD_PHASES_PER_SIDE) &&
        work->infoPanelPhase >= CGP_INFO_PANEL_SETTLE && (s16) work->command.phaseTimer >= 10) {
        return;
    }
    if (work->command.flowRequest == CARD_FLOW_ENEMY_DECK_OUT) {
        return;
    }
    if (work->command.flowRequest == CARD_FLOW_PLAYER_DECK_OUT) {
        return;
    }
    if (work->command.flowRequest == CARD_FLOW_FINAL_ROUND) {
        return;
    }

    /* Information panel and its shadow, lit for player 1 when flag is 0 and for player 2 otherwise. */
    xglMatrixUnit(place);
    xglMatrixTrans(place, place, work->infoPanelPos);
    xglMatrixScale(place, place, work->infoPanelScale);
    nmlModelSetLight(&asColor, &asDir);
    nmlModelSetPlace(&place);
    nmlModelSetTexture(CGP_MAT_FG_TEXTURE);
    if (work->infoPanelPhase == CGP_INFO_PANEL_ZOOM) {
        transparency = 1.0f;
        transparency -= (work->resultWait * 0.3f) / 10.0f;
        nmlModelSetToumei(1);
        nmlModelSetTransparency(transparency);
    }
    nmlModelEntryCard(model);

    memset(color1P, 0, sizeof(color1P));
    color1P[0][3] = 1.0f;
    color1P[1][3] = 1.0f;
    color1P[2][3] = 1.0f;
    color1P[3][0] = 0.5f;
    color1P[3][1] = 0.5f;
    color1P[3][2] = 1.0f;
    color1P[3][3] = 1.0f;
    memset(color2P, 0, sizeof(color2P));
    color2P[0][3] = 1.0f;
    color2P[1][3] = 1.0f;
    color2P[2][3] = 1.0f;
    color2P[3][0] = 1.0f;
    color2P[3][1] = 0.4f;
    color2P[3][2] = 0.4f;
    color2P[3][3] = 1.0f;
    memset(turnColor, 0, sizeof(turnColor));
    turnColor[0][3] = 1.0f;
    turnColor[1][3] = 1.0f;
    turnColor[2][3] = 1.0f;
    turnColor[3][0] = 1.0f;
    turnColor[3][1] = 0.9f;
    turnColor[3][2] = 0.4f;
    turnColor[3][3] = 1.0f;

    xglMatrixUnit(place);
    shadowPos[0] = work->infoPanelPos[0];
    shadowPos[1] = work->infoPanelPos[1];
    shadowPos[2] = work->infoPanelPos[2] - 0.01f;
    shadowPos[3] = 1.0f;
    xglMatrixTrans(place, place, shadowPos);
    xglMatrixScale(place, place, work->infoPanelScale);
    if (flag == 0) {
        nmlModelSetLight(&color1P, &asDir);
    } else {
        nmlModelSetLight(&color2P, &asDir);
    }
    nmlModelSetPlace(&place);
    nmlModelSetTexture(CGP_PANEL_TEXTURE);
    if (work->infoPanelPhase == CGP_INFO_PANEL_ZOOM) {
        transparency = 1.0f;
        transparency -= (work->resultWait * 0.3f) / 10.0f;
        nmlModelSetToumei(1);
        nmlModelSetTransparency(transparency);
    }
    nmlModelEntry(CGP_INFO_PANEL_SIDE_MODEL);

    /* Turn counter: the plate, then the number frame and the two digit models. */
    pos[0] = 0.0f;
    pos[1] = 1.72f;
    pos[2] = 0.0f;
    pos[3] = 1.0f;
    xglMatrixUnit(place);
    xglMatrixTrans(place, place, pos);
    nmlModelSetLight(&turnColor, &asDir);
    nmlModelSetPlace(&place);
    nmlModelSetTexture(CGP_PANEL_TEXTURE);
    nmlModelEntryCard(CGP_TURN_PLATE_MODEL);

    pos[0] = 0.0f;
    pos[1] = -0.02f;
    pos[2] = 0.05f;
    pos[3] = 1.0f;
    xglMatrixTrans(place, place, pos);
    nmlModelSetPlace(&place);
    nmlModelSetTexture(CGP_MAT_FG_TEXTURE);
    nmlModelEntryCard(CGP_TURN_NUMBER_MODEL);

    tens = (work->command.turn + 1) / 10;
    if (tens != 0) {
        model = (void *) (CGP_TURN_DIGIT_MODEL_BASE + tens * CGP_TURN_DIGIT_MODEL_SIZE);
        pos[0] = 0.213f;
        pos[1] = -0.003f;
        pos[2] = 0.0f;
        xglMatrixTrans(guidePlace, place, pos);
        nmlModelSetPlace(&guidePlace);
        nmlModelSetTexture(CGP_CARD_INFO_TEXTURE);
        nmlModelEntryCard(model);
    }
    model = (void *) (CGP_TURN_DIGIT_MODEL_BASE + ((work->command.turn + 1) % 10) * CGP_TURN_DIGIT_MODEL_SIZE);
    pos[0] = 0.311f;
    pos[1] = -0.003f;
    pos[2] = 0.0f;
    xglMatrixTrans(guidePlace, place, pos);
    nmlModelSetPlace(&guidePlace);
    nmlModelSetTexture(CGP_CARD_INFO_TEXTURE);
    nmlModelEntryCard(model);

    pos[0] = -2.3f;
    pos[2] = 0.0f;
    pos[3] = 1.0f;
    guideScale[0] = 0.5f;
    guideScale[1] = 0.5f;
    guideScale[2] = 1.0f;
    guideScale[3] = 1.0f;
    if (work->save->flags & CARD_SAVE_HIDE_TURN) {
        return;
    }

    /* Phase guide of the side whose phase is running: one lamp model per phase of that side. */
    if (work->command.phase < CARD_PHASES_PER_SIDE) {
        area = CGPCursor2Area(workAddress);
        pos[1] = 1.53f;
        if (work->rotStageId == 0) {
            if (area == 3 || work->cursorMoveStep[1] != 0) {
                pos[1] = -0.31000006f;
            }
        } else {
            pos[1] = -0.31000006f;
            if (area == 2) {
                pos[1] = 1.53f;
            }
        }
        xglMatrixUnit(place);
        for (i = 0; i < 7; i++) {
            model = CPIPhaseMdlLst[i];
            xglMatrixTrans(guidePlace, place, pos);
            xglMatrixScale(guidePlace, guidePlace, guideScale);
            nmlModelSetPlace(&guidePlace);
            nmlModelSetLight(&asColor, &asDir);
            nmlModelSetTexture(CGP_MAT_FG_TEXTURE);
            nmlModelEntryCard(model);
            xglMatrixTrans(guidePlace, place, pos);
            xglMatrixScale(guidePlace, guidePlace, guideScale);
            if (i == 4) {
                if (work->command.phase == i) {
                    nmlModelSetLight(&CLP2Color, &asDir);
                } else {
                    nmlModelSetLight(&color2P, &asDir);
                }
            } else if (work->command.phase == i) {
                nmlModelSetLight(&CLP1Color, &asDir);
            } else {
                nmlModelSetLight(&color1P, &asDir);
            }
            nmlModelSetPlace(&guidePlace);
            nmlModelSetTexture(CGP_PANEL_TEXTURE);
            nmlModelEntryCard(CGP_PHASE_LAMP_MODEL);
            pos[1] -= 0.23f;
        }
    } else {
        pos[0] = 2.3f;
        area = CGPCursor2Area(workAddress);
        pos[1] = -0.31000006f;
        if (work->rotStageId == 0) {
            if (area == 7 || work->cursorMoveStep[0] != 0) {
                pos[1] = 1.53f;
            }
        } else {
            pos[1] = 1.53f;
            if (area == 6) {
                pos[1] = -0.31000006f;
            }
        }
        xglMatrixUnit(place);
        for (i = 0; i < 7; i++) {
            model = CPIPhaseMdlLst[i];
            xglMatrixTrans(guidePlace, place, pos);
            xglMatrixScale(guidePlace, guidePlace, guideScale);
            nmlModelSetPlace(&guidePlace);
            nmlModelSetLight(&asColor, &asDir);
            nmlModelSetTexture(CGP_MAT_FG_TEXTURE);
            nmlModelEntryCard(model);
            xglMatrixTrans(guidePlace, place, pos);
            xglMatrixScale(guidePlace, guidePlace, guideScale);
            if (i == 4) {
                if (work->command.phase - CARD_PHASES_PER_SIDE == i) {
                    nmlModelSetLight(&CLP1Color, &asDir);
                } else {
                    nmlModelSetLight(&color1P, &asDir);
                }
            } else if (work->command.phase - CARD_PHASES_PER_SIDE == i) {
                nmlModelSetLight(&CLP2Color, &asDir);
            } else {
                nmlModelSetLight(&color2P, &asDir);
            }
            nmlModelSetPlace(&guidePlace);
            nmlModelSetTexture(CGP_PANEL_TEXTURE);
            nmlModelEntryCard(CGP_PHASE_LAMP_MODEL);
            pos[1] -= 0.23f;
        }
    }
}

/*
 * Whether the Square request for the confirm prompt (CARD_FLOW_CONFIRM) is allowed in the
 * current phase. Phases 0, 5, 6, 7, 12 and 13 never allow it and a phase outside 0..13 always
 * does. Phases 1 and 8 refuse it when their flag is set; phases 2, 3, 4, 9, 10 and 11 refuse it
 * then too and also once the phase timer has reached 3.
 */
s32 CGPChkFaseLock(CardGameWork *work)
{
    s32 allowed;

    allowed = 1;
    switch (work->command.phase) {
    case CARD_PHASE_DRAW:
    case CARD_PHASE_BATTLE:
    case CARD_PHASE_ADJUST:
    case CARD_PHASE_DRAW + CARD_PHASES_PER_SIDE:
    case CARD_PHASE_BATTLE + CARD_PHASES_PER_SIDE:
    case CARD_PHASE_ADJUST + CARD_PHASES_PER_SIDE:
        allowed = 0;
        break;
    case CARD_PHASE_MOVE:
        if (work->flags & CGP_FLAG_SIDE0_AUTO) {
            allowed = 0;
        }
        break;
    case CARD_PHASE_MOVE + CARD_PHASES_PER_SIDE:
        if (work->flags & CGP_FLAG_SIDE1_AUTO) {
            allowed = 0;
        }
        break;
    case CARD_PHASE_EVENT:
    case CARD_PHASE_SET:
    case CARD_PHASE_BLOCK + CARD_PHASES_PER_SIDE:
        if ((work->flags & CGP_FLAG_SIDE0_AUTO) || (s16) work->command.phaseTimer >= 3) {
            allowed = 0;
        }
        break;
    case CARD_PHASE_BLOCK:
    case CARD_PHASE_EVENT + CARD_PHASES_PER_SIDE:
    case CARD_PHASE_SET + CARD_PHASES_PER_SIDE:
        if ((work->flags & CGP_FLAG_SIDE1_AUTO) || (s16) work->command.phaseTimer >= 3) {
            allowed = 0;
        }
        break;
    }
    return allowed;
}

/*
 * Registers one effect of the card game in the first free entry of the work's effect table
 * (64 entries; nothing happens when all are busy). position is where the effect starts (NULL
 * leaves the start each code picks), stack the board slot it travels to.
 */
void CardSetEffect(s32 side, CardGameWork *workAddress, u8 code, s8 value, s32 soundId,
                   float *position, CardLayerStack *stack)
{
    CardGameWork *work = workAddress;
    CardEffect *effect;
    s32 i;

    effect = 0;
    for (i = 0; i < 64; i++) {
        if (work->effects[i].code == 0) {
            effect = &work->effects[i];
            break;
        }
    }
    if (effect == 0) {
        return;
    }

    effect->side = side;
    effect->timer = 15;
    effect->soundId = soundId;
    effect->phase = 0;
    effect->code = code;
    effect->stack = 0;
    if (position != 0) {
        effect->position[0] = position[0];
        effect->position[1] = position[1];
        effect->position[2] = position[2] + 0.1f;
    }
    effect->scale[0] = effect->scale[1] = effect->scale[2] = 1.0f;
    effect->velocity[0] = effect->velocity[1] = effect->velocity[2] = 0.0f;
    effect->scale[3] = 1.0f;
    effect->velocity[3] = 1.0f;
    effect->position[3] = 1.0f;

    /* Per-code start position, timer, sound and travel velocity. */
    switch (code) {
    case CGP_EFFECT_SWIRL:
        xglSoundEffectNormalID(CGP_SE_REGEN, 0);
        if (position == 0) {
            if (value == 0) {
                effect->position[1] = -1.81f;
            } else {
                effect->position[1] = 1.81f;
            }
            effect->position[2] = 0.1f;
        }
        effect->timer = 30;
        if (stack != 0) {
            effect->stack = stack;
            effect->velocity[0] = (-stack->position[0] - position[0]) / 15.0f;
            effect->velocity[1] = (-stack->position[1] - position[1]) / 15.0f;
        }
        effect->scale[1] = 1.5f;
        effect->scale[0] = 1.5f;
        effect->scale[2] = 1.0f;
        effect->scale[3] = 1.0f;
        effect->velocity[3] = 1.0f;
        effect->position[3] = 1.0f;
        break;
    case CGP_EFFECT_DAMAGE:
        effect->stack = stack;
        effect->position[2] = 0.1f;
        effect->position[0] = 0.0f;
        effect->position[1] = 0.0f;
        effect->scale[3] = 1.0f;
        effect->velocity[3] = 1.0f;
        effect->position[3] = 1.0f;
        if (value != 0) {
            if (value >= 100) {
                value = 99;
            }
            effect->value = value;
            effect->velocity[1] = 0.0053333333f;
        } else {
            effect->code = 0;
        }
        break;
    case CGP_EFFECT_DAMAGE_ONE_DESTROYED:
        effect->timer = 24;
        effect->stack = stack;
        effect->position[2] = 0.1f;
        effect->position[0] = 0.0f;
        effect->position[1] = 0.0f;
        effect->scale[3] = 1.0f;
        effect->velocity[3] = 1.0f;
        effect->position[3] = 1.0f;
        if (value != 0) {
            if (value >= 100) {
                value = 99;
            }
            effect->value = value;
            effect->velocity[1] = 0.0053333333f;
        } else {
            effect->code = 0;
        }
        break;
    case CGP_EFFECT_DAMAGE_MANY_DESTROYED:
        effect->timer = 33;
        effect->stack = stack;
        effect->position[2] = 0.1f;
        effect->position[0] = 0.0f;
        effect->position[1] = 0.0f;
        effect->scale[3] = 1.0f;
        effect->velocity[3] = 1.0f;
        effect->position[3] = 1.0f;
        if (value != 0) {
            if (value >= 100) {
                value = 99;
            }
            effect->value = value;
            effect->velocity[1] = 0.0053333333f;
        } else {
            effect->code = 0;
        }
        break;
    case CGP_EFFECT_NUMBER:
        effect->stack = stack;
        effect->code = CGP_EFFECT_NUMBER;
        effect->position[2] = 0.1f;
        effect->phase = CGP_NUMBER_BOTTOM;
        effect->position[0] = 0.0f;
        effect->position[1] = 0.0f;
        effect->scale[3] = 1.0f;
        effect->velocity[3] = 1.0f;
        effect->position[3] = 1.0f;
        if (value >= 0) {
            if (value == 0) {
                effect->code = 0;
            } else {
                effect->value = value;
                effect->velocity[1] = 0.0053333333f;
            }
        } else {
            effect->value = -value;
            effect->velocity[1] = -0.0033333334f;
        }
        break;
    case CGP_EFFECT_NUMBER_TOP:
        effect->stack = stack;
        effect->phase = CGP_NUMBER_TOP;
        effect->code = CGP_EFFECT_NUMBER;
        effect->position[2] = 0.1f;
        effect->position[0] = 0.0f;
        effect->position[1] = 0.0f;
        effect->scale[3] = 1.0f;
        effect->velocity[3] = 1.0f;
        effect->position[3] = 1.0f;
        if (value >= 0) {
            if (value == 0) {
                effect->code = 0;
            } else {
                effect->value = value;
                effect->velocity[1] = 0.0053333333f;
            }
        } else {
            effect->value = -value;
            effect->velocity[1] = -0.0033333334f;
        }
        break;
    case CGP_EFFECT_DAMAGE_LEFT:
        effect->stack = stack;
        effect->position[2] = 0.1f;
        effect->position[0] = 0.0f;
        effect->position[1] = 0.0f;
        if (value == 0) {
            effect->code = 0;
        } else {
            effect->value = value;
            effect->velocity[1] = 0.0053333333f;
        }
        break;
    case CGP_EFFECT_BURST:
        if (effect->soundId != 0) {
            xglSoundEffectNormalID(effect->soundId, 0);
        }
        if (position == 0) {
            effect->position[1] = 1.81f;
            effect->position[2] = 0.15f;
            effect->position[0] = 0.0f;
        }
        effect->scale[1] = 1.5f;
        effect->scale[0] = 1.5f;
        effect->scale[2] = 1.0f;
        effect->scale[3] = 1.0f;
        effect->velocity[3] = 1.0f;
        effect->position[3] = 1.0f;
        break;
    case CGP_EFFECT_DOUBLE_BURST:
        if (effect->soundId != 0) {
            xglSoundEffectNormalID(effect->soundId, 0);
        }
        if (position == 0) {
            effect->position[1] = 1.81f;
            effect->position[2] = 0.15f;
            effect->position[0] = 0.0f;
        }
        effect->position[0] -= 0.125f;
        effect->position[1] -= 0.125f;
        effect->scale[1] = 1.5f;
        effect->scale[0] = 1.5f;
        effect->scale[2] = 1.0f;
        effect->scale[3] = 1.0f;
        effect->velocity[3] = 1.0f;
        effect->position[3] = 1.0f;
        break;
    default:
        if (effect->soundId != 0) {
            xglSoundEffectNormalID(effect->soundId, 0);
        }
        if (stack != 0) {
            if (code == CGP_EFFECT_SUPPORT) {
                effect->code = CGP_EFFECT_SUPPORT_ATTACK;
                effect->velocity[2] = 0.006666667f;
                effect->velocity[0] = -effect->position[0] / 15.0f;
                effect->velocity[1] = (0.7885f - effect->position[1]) / 15.0f;
            } else if (code == CGP_EFFECT_SUPPORT_STRONG) {
                effect->velocity[2] = 0.006666667f;
                effect->velocity[0] = -effect->position[0] / 15.0f;
                effect->velocity[1] = (0.7885f - effect->position[1]) / 15.0f;
            }
            effect->stack = stack;
            effect->velocity[0] = (-stack->position[0] - position[0]) / 15.0f;
            effect->velocity[1] = (-stack->position[1] - position[1]) / 15.0f;
        } else {
            switch (code) {
            case CGP_EFFECT_PIERCE_ATTACK:
            case CGP_EFFECT_DIRECT_ATTACK:
            case CGP_EFFECT_PIERCE_ATTACK_ONE_DESTROYED:
            case CGP_EFFECT_DIRECT_ATTACK_ONE_DESTROYED:
            case CGP_EFFECT_PIERCE_ATTACK_MANY_DESTROYED:
            case CGP_EFFECT_DIRECT_ATTACK_MANY_DESTROYED:
                effect->velocity[2] = 0.006666667f;
                effect->velocity[0] = -effect->position[0] / 15.0f;
                effect->velocity[1] = (1.81f - effect->position[1]) / 15.0f;
                break;
            case CGP_EFFECT_SUPPORT:
                effect->code = CGP_EFFECT_SUPPORT_ATTACK;
                /* fallthrough */
            case CGP_EFFECT_SUPPORT_STRONG:
                effect->velocity[2] = 0.006666667f;
                effect->velocity[0] = -effect->position[0] / 15.0f;
                effect->velocity[1] = (0.7885f - effect->position[1]) / 15.0f;
                break;
            }
        }
        effect->scale[1] = 1.5f;
        effect->scale[0] = 1.5f;
        effect->scale[2] = 1.0f;
        effect->scale[3] = 1.0f;
        effect->velocity[3] = 1.0f;
        effect->position[3] = 1.0f;
        break;
    }
    work->effectsSet++;
}

/*
 * Resets an effect's transform matrix to identity while preserving its
 * fourth row: CardDispEffect and CardSetEffect keep four color or scale
 * values there, aliased with the row xglMatrixUnit would otherwise clear.
 */
void CDEMatrixSub(float matrix[4][4])
{
    float row3[4];

    row3[0] = matrix[3][0];
    row3[1] = matrix[3][1];
    row3[2] = matrix[3][2];
    row3[3] = matrix[3][3];
    xglMatrixUnit(matrix);
    matrix[3][0] = row3[0];
    matrix[3][1] = row3[1];
    matrix[3][2] = row3[2];
    matrix[3][3] = row3[3];
}






/*
 * Draws and advances the 64 card-game effects of one side. Each effect starts from a fresh
 * copy of the caller's placement matrix; the matrix is read as volatile, so the copy at the top
 * of the loop is never moved into the loop branch (the original keeps the epilogue there).
 */
void CardDispEffect(s32 side, CardGameWork *work, volatile CardEffectMatrix *matrix)
{
    CardEffect *effect;
    s32 i;

    effect = work->effects;
    for (i = 0; i < CARD_EFFECT_COUNT; i++, effect++) {
        CardEffectMatrix local;
        float offset[4];
        CardEffectMatrix quad;
        void *texture;
        void *digitModel;
        s32 digit;
        Quadword row;

        row = matrix->q[0];
        local.q[0] = row;
        row = matrix->q[1];
        local.q[1] = row;
        row = matrix->q[2];
        local.q[2] = row;
        row = matrix->q[3];
        local.q[3] = row;

        /* Attack effects 1-4: travelling strip, burst, destroyed check, then fade. */
        switch (effect->code) {
        case CGP_EFFECT_FRONT_ATTACK:
        case CGP_EFFECT_PIERCE_ATTACK:
        case CGP_EFFECT_SUPPORT_ATTACK:
        case CGP_EFFECT_DIRECT_ATTACK:
            if (side != effect->side) {
                break;
            }
            switch (effect->phase) {
            case CGP_EFFECT_PHASE_TRAVEL:
                if (effect->timer != 0) {
                    effect->timer--;
                    switch (effect->code) {
                    case CGP_EFFECT_FRONT_ATTACK:
                        texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_TRAVEL_1, 14 - effect->timer, CARD_EFFECT_SMALL_FRAME);
                        break;
                    case CGP_EFFECT_PIERCE_ATTACK:
                        texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_TRAVEL_2, 14 - effect->timer, CARD_EFFECT_SMALL_FRAME);
                        break;
                    case CGP_EFFECT_SUPPORT_ATTACK:
                        texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_TRAVEL_3, 14 - effect->timer, CARD_EFFECT_SMALL_FRAME);
                        break;
                    default:
                        texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_TRAVEL_4, 14 - effect->timer, CARD_EFFECT_SMALL_FRAME);
                        break;
                    }
                    effect->position[0] += effect->velocity[0];
                    effect->position[1] += effect->velocity[1];
                    effect->position[2] += effect->velocity[2];
                    xglMatrixTrans(local.m, local.m, effect->position);
                    nmlModelSetTexture(texture);
                    nmlModelSetPlace(&local.m);
                    nmlModelSetToumei(1);
                    nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                    nmlModelEntryCard(CARD_EFFECT_SPRITE_MODEL);
                } else {
                    effect->phase = CGP_EFFECT_PHASE_BURST;
                    switch (effect->code) {
                    case CGP_EFFECT_FRONT_ATTACK:
                        effect->timer = 13;
                        xglSoundEffectNormalID(CARD_EFFECT_SE_BURST, 0);
                        break;
                    case CGP_EFFECT_PIERCE_ATTACK:
                        xglSoundEffectNormalID(CARD_EFFECT_SE_HIT, 0);
                        effect->timer = 16;
                        break;
                    case CGP_EFFECT_SUPPORT_ATTACK:
                        xglSoundEffectNormalID(CARD_EFFECT_SE_HIT, 0);
                        effect->timer = 11;
                        break;
                    case CGP_EFFECT_DIRECT_ATTACK:
                        xglSoundEffectNormalID(CARD_EFFECT_SE_HIT, 0);
                        effect->timer = 15;
                        break;
                    }
                }
                break;
            case CGP_EFFECT_PHASE_BURST:
                switch (effect->code) {
                case CGP_EFFECT_FRONT_ATTACK:
                    if (effect->timer != 0) {
                        effect->timer--;
                        texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_1, 12 - effect->timer, CARD_EFFECT_LARGE_FRAME);
                        xglMatrixTrans(local.m, local.m, effect->position);
                        xglMatrixScale(local.m, local.m, effect->scale);
                        nmlModelSetTexture(texture);
                        nmlModelSetPlace(&local.m);
                        nmlModelSetToumei(1);
                        nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                        nmlModelEntryCard(CARD_EFFECT_SCALED_MODEL);
                    } else {
                        effect->phase = CGP_EFFECT_PHASE_CHECK;
                    }
                    break;
                case CGP_EFFECT_PIERCE_ATTACK:
                    if (effect->timer != 0) {
                        effect->timer--;
                        texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_2, 15 - effect->timer, CARD_EFFECT_LARGE_FRAME);
                        xglMatrixTrans(local.m, local.m, effect->position);
                        xglMatrixScale(local.m, local.m, effect->scale);
                        nmlModelSetTexture(texture);
                        nmlModelSetPlace(&local.m);
                        nmlModelSetToumei(1);
                        nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                        nmlModelEntryCard(CARD_EFFECT_SCALED_MODEL);
                    } else {
                        effect->phase = CGP_EFFECT_PHASE_CHECK;
                    }
                    break;
                case CGP_EFFECT_SUPPORT_ATTACK:
                    if (effect->timer != 0) {
                        effect->timer--;
                        texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_3, 10 - effect->timer, CARD_EFFECT_LARGE_FRAME);
                        xglMatrixTrans(local.m, local.m, effect->position);
                        xglMatrixScale(local.m, local.m, effect->scale);
                        nmlModelSetTexture(texture);
                        nmlModelSetPlace(&local.m);
                        nmlModelSetToumei(1);
                        nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                        nmlModelEntryCard(CARD_EFFECT_SCALED_MODEL);
                    } else {
                        effect->phase = CGP_EFFECT_PHASE_CHECK;
                    }
                    break;
                case CGP_EFFECT_DIRECT_ATTACK:
                    if (effect->timer == 0) {
                        effect->code = 0;
                        break;
                    }
                    effect->timer--;
                    texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_4, 14 - effect->timer, CARD_EFFECT_LARGE_FRAME);
                    xglMatrixTrans(local.m, local.m, effect->position);
                    xglMatrixScale(local.m, local.m, effect->scale);
                    nmlModelSetTexture(texture);
                    nmlModelSetPlace(&local.m);
                    nmlModelSetToumei(1);
                    nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                    nmlModelEntryCard(CARD_EFFECT_SCALED_MODEL);
                    break;
                }
                break;
            case CGP_EFFECT_PHASE_CHECK:
                if (effect->stack == 0 || effect->stack->layers[0].life > 0) {
                    effect->code = 0;
                } else {
                    effect->phase = CGP_EFFECT_PHASE_FADE;
                    effect->timer = 20;
                }
                break;
            case CGP_EFFECT_PHASE_FADE:
                if (effect->timer == 0) {
                    effect->code = 0;
                    break;
                }
                effect->timer--;
                texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_FADE, 10 - effect->timer / 2, CARD_EFFECT_SMALL_FRAME);
                xglMatrixTrans(local.m, local.m, effect->position);
                CDEMatrixSub(local.m);
                nmlModelSetTexture(texture);
                nmlModelSetPlace(&local.m);
                nmlModelSetToumei(1);
                nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                nmlModelEntryCard(CARD_EFFECT_SPRITE_MODEL);
                break;
            }
            break;
        /* Delayed attack effects 21-24: travel, hold, burst, destroyed check, then fade. */
        case CGP_EFFECT_FRONT_ATTACK_ONE_DESTROYED:
        case CGP_EFFECT_PIERCE_ATTACK_ONE_DESTROYED:
        case CGP_EFFECT_SUPPORT:
        case CGP_EFFECT_DIRECT_ATTACK_ONE_DESTROYED:
            if (side != effect->side) {
                break;
            }
            switch (effect->phase) {
            case CGP_EFFECT_PHASE_TRAVEL:
                if (effect->timer != 0) {
                    effect->timer--;
                    effect->position[0] += effect->velocity[0];
                    effect->position[1] += effect->velocity[1];
                    effect->position[2] += effect->velocity[2];
                } else {
                    effect->timer = 9;
                    effect->phase++;
                }
                break;
            case CGP_EFFECT_PHASE_DELAY:
                if (effect->timer != 0) {
                    effect->timer--;
                    break;
                }
                effect->phase++;
                switch (effect->code) {
                case CGP_EFFECT_FRONT_ATTACK_ONE_DESTROYED:
                    effect->timer = 13;
                    xglSoundEffectNormalID(CARD_EFFECT_SE_BURST, 0);
                    break;
                case CGP_EFFECT_PIERCE_ATTACK_ONE_DESTROYED:
                    xglSoundEffectNormalID(CARD_EFFECT_SE_HIT, 0);
                    effect->timer = 16;
                    break;
                case CGP_EFFECT_SUPPORT:
                    xglSoundEffectNormalID(CARD_EFFECT_SE_HIT, 0);
                    effect->timer = 11;
                    break;
                case CGP_EFFECT_DIRECT_ATTACK_ONE_DESTROYED:
                    xglSoundEffectNormalID(CARD_EFFECT_SE_HIT, 0);
                    effect->timer = 15;
                    break;
                }
                break;
            case CGP_EFFECT_PHASE_DELAYED_BURST:
                switch (effect->code) {
                case CGP_EFFECT_FRONT_ATTACK_ONE_DESTROYED:
                    if (effect->timer != 0) {
                        effect->timer--;
                        texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_1, 12 - effect->timer, CARD_EFFECT_LARGE_FRAME);
                        xglMatrixTrans(local.m, local.m, effect->position);
                        xglMatrixScale(local.m, local.m, effect->scale);
                        nmlModelSetTexture(texture);
                        nmlModelSetPlace(&local.m);
                        nmlModelSetToumei(1);
                        nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                        nmlModelEntryCard(CARD_EFFECT_SCALED_MODEL);
                    } else {
                        effect->phase++;
                    }
                    break;
                case CGP_EFFECT_PIERCE_ATTACK_ONE_DESTROYED:
                    if (effect->timer != 0) {
                        effect->timer--;
                        texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_2, 15 - effect->timer, CARD_EFFECT_LARGE_FRAME);
                        xglMatrixTrans(local.m, local.m, effect->position);
                        xglMatrixScale(local.m, local.m, effect->scale);
                        nmlModelSetTexture(texture);
                        nmlModelSetPlace(&local.m);
                        nmlModelSetToumei(1);
                        nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                        nmlModelEntryCard(CARD_EFFECT_SCALED_MODEL);
                    } else {
                        effect->phase++;
                    }
                    break;
                case CGP_EFFECT_SUPPORT:
                    if (effect->timer != 0) {
                        effect->timer--;
                        texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_3, 10 - effect->timer, CARD_EFFECT_LARGE_FRAME);
                        xglMatrixTrans(local.m, local.m, effect->position);
                        xglMatrixScale(local.m, local.m, effect->scale);
                        nmlModelSetTexture(texture);
                        nmlModelSetPlace(&local.m);
                        nmlModelSetToumei(1);
                        nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                        nmlModelEntryCard(CARD_EFFECT_SCALED_MODEL);
                    } else {
                        effect->phase++;
                    }
                    break;
                case CGP_EFFECT_DIRECT_ATTACK_ONE_DESTROYED:
                    if (effect->timer == 0) {
                        effect->code = 0;
                        break;
                    }
                    effect->timer--;
                    texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_4, 14 - effect->timer, CARD_EFFECT_LARGE_FRAME);
                    xglMatrixTrans(local.m, local.m, effect->position);
                    xglMatrixScale(local.m, local.m, effect->scale);
                    nmlModelSetTexture(texture);
                    nmlModelSetPlace(&local.m);
                    nmlModelSetToumei(1);
                    nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                    nmlModelEntryCard(CARD_EFFECT_SCALED_MODEL);
                    break;
                }
                break;
            case CGP_EFFECT_PHASE_DELAYED_CHECK:
                if (effect->stack == 0 || effect->stack->layers[0].life > 0) {
                    effect->code = 0;
                } else {
                    effect->timer = 20;
                    effect->phase++;
                }
                break;
            case CGP_EFFECT_PHASE_DELAYED_FADE:
                if (effect->timer == 0) {
                    effect->code = 0;
                    break;
                }
                effect->timer--;
                texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_FADE, 10 - effect->timer / 2, CARD_EFFECT_SMALL_FRAME);
                xglMatrixTrans(local.m, local.m, effect->position);
                CDEMatrixSub(local.m);
                nmlModelSetTexture(texture);
                nmlModelSetPlace(&local.m);
                nmlModelSetToumei(1);
                nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                nmlModelEntryCard(CARD_EFFECT_SPRITE_MODEL);
                break;
            }
            break;
        /* Delayed attack effects 31-34: the same steps as 21-24. */
        case CGP_EFFECT_FRONT_ATTACK_MANY_DESTROYED:
        case CGP_EFFECT_PIERCE_ATTACK_MANY_DESTROYED:
        case CGP_EFFECT_DIRECT_ATTACK_MANY_DESTROYED:
            if (side != effect->side) {
                break;
            }
            switch (effect->phase) {
            case CGP_EFFECT_PHASE_TRAVEL:
                if (effect->timer != 0) {
                    effect->timer--;
                    effect->position[0] += effect->velocity[0];
                    effect->position[1] += effect->velocity[1];
                    effect->position[2] += effect->velocity[2];
                } else {
                    effect->timer = 18;
                    effect->phase++;
                }
                break;
            case CGP_EFFECT_PHASE_DELAY:
                if (effect->timer != 0) {
                    effect->timer--;
                    break;
                }
                effect->phase++;
                switch (effect->code) {
                case CGP_EFFECT_FRONT_ATTACK_MANY_DESTROYED:
                    effect->timer = 13;
                    xglSoundEffectNormalID(CARD_EFFECT_SE_BURST, 0);
                    break;
                case CGP_EFFECT_PIERCE_ATTACK_MANY_DESTROYED:
                    xglSoundEffectNormalID(CARD_EFFECT_SE_HIT, 0);
                    effect->timer = 16;
                    break;
                case CGP_EFFECT_SUPPORT_STRONG:
                    xglSoundEffectNormalID(CARD_EFFECT_SE_HIT, 0);
                    effect->timer = 11;
                    break;
                case CGP_EFFECT_DIRECT_ATTACK_MANY_DESTROYED:
                    xglSoundEffectNormalID(CARD_EFFECT_SE_HIT, 0);
                    effect->timer = 15;
                    break;
                }
                break;
            case CGP_EFFECT_PHASE_DELAYED_BURST:
                switch (effect->code) {
                case CGP_EFFECT_FRONT_ATTACK_MANY_DESTROYED:
                    if (effect->timer != 0) {
                        effect->timer--;
                        texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_1, 12 - effect->timer, CARD_EFFECT_LARGE_FRAME);
                        xglMatrixTrans(local.m, local.m, effect->position);
                        xglMatrixScale(local.m, local.m, effect->scale);
                        nmlModelSetTexture(texture);
                        nmlModelSetPlace(&local.m);
                        nmlModelSetToumei(1);
                        nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                        nmlModelEntryCard(CARD_EFFECT_SCALED_MODEL);
                    } else {
                        effect->phase++;
                    }
                    break;
                case CGP_EFFECT_PIERCE_ATTACK_MANY_DESTROYED:
                    if (effect->timer != 0) {
                        effect->timer--;
                        texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_2, 15 - effect->timer, CARD_EFFECT_LARGE_FRAME);
                        xglMatrixTrans(local.m, local.m, effect->position);
                        xglMatrixScale(local.m, local.m, effect->scale);
                        nmlModelSetTexture(texture);
                        nmlModelSetPlace(&local.m);
                        nmlModelSetToumei(1);
                        nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                        nmlModelEntryCard(CARD_EFFECT_SCALED_MODEL);
                    } else {
                        effect->phase++;
                    }
                    break;
                case CGP_EFFECT_SUPPORT_STRONG:
                    if (effect->timer != 0) {
                        effect->timer--;
                        texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_3, 10 - effect->timer, CARD_EFFECT_LARGE_FRAME);
                        xglMatrixTrans(local.m, local.m, effect->position);
                        xglMatrixScale(local.m, local.m, effect->scale);
                        nmlModelSetTexture(texture);
                        nmlModelSetPlace(&local.m);
                        nmlModelSetToumei(1);
                        nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                        nmlModelEntryCard(CARD_EFFECT_SCALED_MODEL);
                    } else {
                        effect->phase++;
                    }
                    break;
                case CGP_EFFECT_DIRECT_ATTACK_MANY_DESTROYED:
                    if (effect->timer == 0) {
                        effect->code = 0;
                        break;
                    }
                    effect->timer--;
                    texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_4, 14 - effect->timer, CARD_EFFECT_LARGE_FRAME);
                    xglMatrixTrans(local.m, local.m, effect->position);
                    xglMatrixScale(local.m, local.m, effect->scale);
                    nmlModelSetTexture(texture);
                    nmlModelSetPlace(&local.m);
                    nmlModelSetToumei(1);
                    nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                    nmlModelEntryCard(CARD_EFFECT_SCALED_MODEL);
                    break;
                }
                break;
            case CGP_EFFECT_PHASE_DELAYED_CHECK:
                if (effect->stack == 0 || effect->stack->layers[0].life > 0) {
                    effect->code = 0;
                } else {
                    effect->timer = 20;
                    effect->phase++;
                }
                break;
            case CGP_EFFECT_PHASE_DELAYED_FADE:
                if (effect->timer == 0) {
                    effect->code = 0;
                    break;
                }
                effect->timer--;
                texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_FADE, 10 - effect->timer / 2, CARD_EFFECT_SMALL_FRAME);
                xglMatrixTrans(local.m, local.m, effect->position);
                CDEMatrixSub(local.m);
                nmlModelSetTexture(texture);
                nmlModelSetPlace(&local.m);
                nmlModelSetToumei(1);
                nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                nmlModelEntryCard(CARD_EFFECT_SPRITE_MODEL);
                break;
            }
            break;
        /* Strong support attack: travel, delay, then the burst repeated at four corners around the target. */
        case CGP_EFFECT_SUPPORT_STRONG:
            if (side != effect->side) {
                break;
            }
            switch (effect->phase) {
            case CGP_EFFECT_PHASE_TRAVEL:
                if (effect->timer != 0) {
                    effect->timer--;
                    effect->position[0] += effect->velocity[0];
                    effect->position[1] += effect->velocity[1];
                    effect->position[2] += effect->velocity[2];
                    texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_TRAVEL_3, 14 - effect->timer, CARD_EFFECT_SMALL_FRAME);
                    xglMatrixTrans(local.m, local.m, effect->position);
                    nmlModelSetTexture(texture);
                    nmlModelSetPlace(&local.m);
                    nmlModelSetToumei(1);
                    nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                    nmlModelEntryCard(CARD_EFFECT_SPRITE_MODEL);
                } else {
                    effect->phase = CGP_EFFECT_PHASE_DELAY;
                    xglSoundEffectNormalID(CARD_EFFECT_SE_HIT, 0);
                    effect->timer = 11;
                }
                break;
            case CGP_EFFECT_PHASE_DELAY:
                if (effect->timer != 0) {
                    effect->timer--;
                    texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_3, 10 - effect->timer, CARD_EFFECT_LARGE_FRAME);
                    xglMatrixTrans(local.m, local.m, effect->position);
                    xglMatrixScale(local.m, local.m, effect->scale);
                    nmlModelSetTexture(texture);
                    nmlModelSetPlace(&local.m);
                    nmlModelSetToumei(1);
                    nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                    nmlModelEntryCard(CARD_EFFECT_SCALED_MODEL);
                } else {
                    effect->timer = 15;
                    effect->phase = CGP_EFFECT_PHASE_DELAYED_BURST;
                    xglSoundEffectNormalID(CARD_EFFECT_SE_HIT, 0);
                }
                break;
            case CGP_EFFECT_PHASE_DELAYED_BURST:
                if (effect->timer == 0) {
                    effect->code = 0;
                    break;
                }
                effect->timer--;
                texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_2, 15 - effect->timer, CARD_EFFECT_LARGE_FRAME);
                offset[2] = effect->position[2];
                offset[3] = 1.0f;
                offset[0] = effect->position[0] - 0.54f;
                offset[1] = effect->position[1] + 0.3915f;
                xglMatrixTrans(quad.m, local.m, offset);
                CDEMatrixSub(quad.m);
                nmlModelSetTexture(texture);
                nmlModelSetPlace(&quad.m);
                nmlModelSetToumei(1);
                nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                nmlModelEntryCard(CARD_EFFECT_SCALED_MODEL);
                offset[0] = effect->position[0] + 0.54f;
                offset[1] = effect->position[1] + 0.3915f;
                xglMatrixTrans(quad.m, local.m, offset);
                CDEMatrixSub(quad.m);
                nmlModelSetTexture(texture);
                nmlModelSetPlace(&quad.m);
                nmlModelSetToumei(1);
                nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                nmlModelEntryCard(CARD_EFFECT_SCALED_MODEL);
                offset[0] = effect->position[0] - 0.54f;
                offset[1] = effect->position[1] - 0.3915f;
                xglMatrixTrans(quad.m, local.m, offset);
                CDEMatrixSub(quad.m);
                nmlModelSetTexture(texture);
                nmlModelSetPlace(&quad.m);
                nmlModelSetToumei(1);
                nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                nmlModelEntryCard(CARD_EFFECT_SCALED_MODEL);
                offset[0] = effect->position[0] + 0.54f;
                offset[1] = effect->position[1] - 0.3915f;
                xglMatrixTrans(quad.m, local.m, offset);
                CDEMatrixSub(quad.m);
                nmlModelSetTexture(texture);
                nmlModelSetPlace(&quad.m);
                nmlModelSetToumei(1);
                nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                nmlModelEntryCard(CARD_EFFECT_SCALED_MODEL);
                break;
            }
            break;
        case CGP_EFFECT_BURST:
            if (side != effect->side) {
                break;
            }
            if (effect->timer == 0) {
                effect->code = 0;
                break;
            }
            effect->timer--;
            texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_2, 15 - effect->timer, CARD_EFFECT_LARGE_FRAME);
            xglMatrixTrans(local.m, local.m, effect->position);
            CDEMatrixSub(local.m);
            nmlModelSetTexture(texture);
            nmlModelSetPlace(&local.m);
            nmlModelSetToumei(1);
            nmlModelSetAlpha(CARD_EFFECT_ALPHA);
            nmlModelEntryCard(CARD_EFFECT_SPRITE_MODEL);
            break;
        case CGP_EFFECT_DOUBLE_BURST:
            if (side != effect->side) {
                break;
            }
            if (effect->timer != 0) {
                effect->timer--;
                texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_BURST_2, 15 - effect->timer, CARD_EFFECT_LARGE_FRAME);
                xglMatrixTrans(local.m, local.m, effect->position);
                CDEMatrixSub(local.m);
                nmlModelSetTexture(texture);
                nmlModelSetPlace(&local.m);
                nmlModelSetToumei(1);
                nmlModelSetAlpha(CARD_EFFECT_ALPHA);
                nmlModelEntryCard(CARD_EFFECT_SPRITE_MODEL);
            } else if (effect->phase != 0) {
                effect->code = 0;
            } else {
                effect->phase++;
                effect->timer = 15;
                xglSoundEffectNormalID(CARD_EFFECT_SE_HIT, 0);
                effect->position[0] += 0.25f;
                effect->position[1] += 0.25f;
            }
            break;
        case CGP_EFFECT_SWIRL:
            if (side != effect->side) {
                break;
            }
            if (effect->timer == 0) {
                effect->code = 0;
                break;
            }
            effect->timer--;
            texture = CARD_EFFECT_FRAME(CARD_EFFECT_STRIP_SWIRL, 29 - effect->timer, CARD_EFFECT_SMALL_FRAME);
            xglMatrixTrans(local.m, local.m, effect->position);
            CDEMatrixSub(local.m);
            nmlModelSetTexture(texture);
            xglMatrixScale(local.m, local.m, effect->scale);
            nmlModelSetPlace(&local.m);
            nmlModelSetToumei(1);
            nmlModelSetAlpha(CARD_EFFECT_ALPHA);
            nmlModelEntryCard(CARD_EFFECT_SPRITE_MODEL);
            break;
        /* Damage digits, drawn for the side other than the effect's own: wait, then show the value. */
        case CGP_EFFECT_DAMAGE:
            if (side == effect->side) {
                break;
            }
            switch (effect->phase) {
            case CGP_EFFECT_PHASE_WAIT:
                if (effect->timer != 0) {
                    effect->timer--;
                } else {
                    effect->phase = CGP_EFFECT_PHASE_SHOW;
                    effect->timer = 15;
                }
                break;
            case CGP_EFFECT_PHASE_SHOW:
                if (effect->timer != 0) {
                    effect->timer--;
                        offset[3] = 1.0f;
                        if (effect->stack != 0) {
                            offset[0] = effect->stack->position[0];
                            offset[1] = effect->stack->position[1];
                            offset[2] = effect->stack->position[2];
                        } else {
                            offset[0] = 0.0f;
                            offset[1] = -1.81f;
                            offset[2] = 0.0f;
                        }
                        xglMatrixTrans(local.m, local.m, offset);
                        CDEMatrixSub(local.m);
                        xglMatrixTrans(local.m, local.m, effect->position);
                        offset[0] = 0.15f;
                        offset[1] = 0.0f;
                        offset[2] = 0.1f;
                        offset[3] = 1.0f;
                        digit = effect->value / 10;
                        if (digit != 0) {
                            xglMatrixTrans(quad.m, local.m, offset);
                            digitModel = CARD_EFFECT_DIGIT_MODEL(CARD_EFFECT_DIGITS, digit);
                            nmlModelSetPlace(&quad.m);
                            nmlModelSetTexture(CARD_EFFECT_DIGIT_TEXTURE);
                            nmlModelSetLight(&asColor, &asDir);
                            nmlModelEntryCard(digitModel);
                        }
                        offset[0] = 0.25f;
                        digit = effect->value % 10;
                        digitModel = CARD_EFFECT_DIGIT_MODEL(CARD_EFFECT_DIGITS, digit);
                        xglMatrixTrans(quad.m, local.m, offset);
                        nmlModelSetPlace(&quad.m);
                        nmlModelSetTexture(CARD_EFFECT_DIGIT_TEXTURE);
                        nmlModelSetLight(&asColor, &asDir);
                        nmlModelEntryCard(digitModel);
                        effect->position[0] += effect->velocity[0];
                        effect->position[1] += effect->velocity[1];
                        effect->position[2] += effect->velocity[2];
                } else {
                    effect->code = 0;
                }
                break;
            }
            break;
        case CGP_EFFECT_DAMAGE_ONE_DESTROYED:
            if (side == effect->side) {
                break;
            }
            switch (effect->phase) {
            case CGP_EFFECT_PHASE_WAIT:
                if (effect->timer != 0) {
                    effect->timer--;
                } else {
                    effect->phase = CGP_EFFECT_PHASE_SHOW;
                    effect->timer = 15;
                }
                break;
            case CGP_EFFECT_PHASE_SHOW:
                if (effect->timer != 0) {
                    effect->timer--;
                        offset[3] = 1.0f;
                        if (effect->stack != 0) {
                            offset[0] = effect->stack->position[0];
                            offset[1] = effect->stack->position[1];
                            offset[2] = effect->stack->position[2];
                        } else {
                            offset[0] = 0.0f;
                            offset[1] = -1.81f;
                            offset[2] = 0.0f;
                        }
                        xglMatrixTrans(local.m, local.m, offset);
                        CDEMatrixSub(local.m);
                        xglMatrixTrans(local.m, local.m, effect->position);
                        offset[0] = 0.15f;
                        offset[1] = 0.0f;
                        offset[2] = 0.1f;
                        offset[3] = 1.0f;
                        digit = effect->value / 10;
                        if (digit != 0) {
                            xglMatrixTrans(quad.m, local.m, offset);
                            digitModel = CARD_EFFECT_DIGIT_MODEL(CARD_EFFECT_DIGITS, digit);
                            nmlModelSetPlace(&quad.m);
                            nmlModelSetTexture(CARD_EFFECT_DIGIT_TEXTURE);
                            nmlModelSetLight(&asColor, &asDir);
                            nmlModelEntryCard(digitModel);
                        }
                        offset[0] = 0.25f;
                        digit = effect->value % 10;
                        digitModel = CARD_EFFECT_DIGIT_MODEL(CARD_EFFECT_DIGITS, digit);
                        xglMatrixTrans(quad.m, local.m, offset);
                        nmlModelSetPlace(&quad.m);
                        nmlModelSetTexture(CARD_EFFECT_DIGIT_TEXTURE);
                        nmlModelSetLight(&asColor, &asDir);
                        nmlModelEntryCard(digitModel);
                        effect->position[0] += effect->velocity[0];
                        effect->position[1] += effect->velocity[1];
                        effect->position[2] += effect->velocity[2];
                } else {
                    effect->code = 0;
                }
                break;
            }
            break;
        /* Number effect: digits at the stack or at the bottom or top of the screen. */
        case CGP_EFFECT_NUMBER:
            if (side != effect->side) {
                break;
            }
            if (effect->timer == 0) {
                effect->code = 0;
                break;
            }
            effect->timer--;
            offset[3] = 1.0f;
            if (effect->stack != 0) {
                offset[0] = effect->stack->position[0];
                offset[1] = effect->stack->position[1];
                offset[2] = effect->stack->position[2];
            } else {
                if (effect->phase == CGP_NUMBER_BOTTOM) {
                    offset[1] = -1.81f;
                } else {
                    offset[1] = 1.81f;
                }
                offset[0] = 0.0f;
                offset[2] = 0.0f;
            }
            xglMatrixTrans(local.m, local.m, offset);
            CDEMatrixSub(local.m);
            xglMatrixTrans(local.m, local.m, effect->position);
            offset[0] = 0.15f;
            offset[1] = 0.0f;
            offset[2] = 0.1f;
            digit = effect->value / 10;
            if (digit != 0) {
                xglMatrixTrans(quad.m, local.m, offset);
                if (effect->phase == CGP_NUMBER_BOTTOM) {
                    digitModel = CARD_EFFECT_DIGIT_MODEL(CARD_EFFECT_DIGITS_FIRST, digit);
                } else {
                    digitModel = CARD_EFFECT_DIGIT_MODEL(CARD_EFFECT_DIGITS, digit);
                }
                nmlModelSetPlace(&quad.m);
                nmlModelSetTexture(CARD_EFFECT_DIGIT_TEXTURE);
                nmlModelSetLight(&asColor, &asDir);
                nmlModelEntryCard(digitModel);
            }
            offset[0] = 0.25f;
            digit = effect->value % 10;
            if (effect->phase == CGP_NUMBER_BOTTOM) {
                digitModel = CARD_EFFECT_DIGIT_MODEL(CARD_EFFECT_DIGITS_FIRST, digit);
            } else {
                digitModel = CARD_EFFECT_DIGIT_MODEL(CARD_EFFECT_DIGITS, digit);
            }
            xglMatrixTrans(quad.m, local.m, offset);
            nmlModelSetPlace(&quad.m);
            nmlModelSetTexture(CARD_EFFECT_DIGIT_TEXTURE);
            nmlModelSetLight(&asColor, &asDir);
            nmlModelEntryCard(digitModel);
            effect->position[0] += effect->velocity[0];
            effect->position[1] += effect->velocity[1];
            effect->position[2] += effect->velocity[2];
            break;
        case CGP_EFFECT_DAMAGE_LEFT:
            if (side == effect->side) {
                break;
            }
            switch (effect->phase) {
            case CGP_EFFECT_PHASE_WAIT:
                if (effect->timer != 0) {
                    effect->timer--;
                } else {
                    effect->phase = CGP_EFFECT_PHASE_SHOW;
                    effect->timer = 15;
                }
                break;
            case CGP_EFFECT_PHASE_SHOW:
                if (effect->timer != 0) {
                    effect->timer--;
                        offset[3] = 1.0f;
                        if (effect->stack != 0) {
                            offset[0] = effect->stack->position[0];
                            offset[1] = effect->stack->position[1];
                            offset[2] = effect->stack->position[2];
                        } else {
                            offset[0] = 0.0f;
                            offset[1] = -1.81f;
                            offset[2] = 0.0f;
                        }
                        xglMatrixTrans(local.m, local.m, offset);
                        CDEMatrixSub(local.m);
                        xglMatrixTrans(local.m, local.m, effect->position);
                        offset[0] = -0.35f;
                        offset[1] = 0.0f;
                        offset[2] = 0.1f;
                        offset[3] = 1.0f;
                        digit = effect->value / 10;
                        if (digit != 0) {
                            xglMatrixTrans(quad.m, local.m, offset);
                            digitModel = CARD_EFFECT_DIGIT_MODEL(CARD_EFFECT_DIGITS, digit);
                            nmlModelSetPlace(&quad.m);
                            nmlModelSetTexture(CARD_EFFECT_DIGIT_TEXTURE);
                            nmlModelSetLight(&asColor, &asDir);
                            nmlModelEntryCard(digitModel);
                        }
                        offset[0] = -0.25f;
                        digit = effect->value % 10;
                        digitModel = CARD_EFFECT_DIGIT_MODEL(CARD_EFFECT_DIGITS, digit);
                        xglMatrixTrans(quad.m, local.m, offset);
                        nmlModelSetPlace(&quad.m);
                        nmlModelSetTexture(CARD_EFFECT_DIGIT_TEXTURE);
                        nmlModelSetLight(&asColor, &asDir);
                        nmlModelEntryCard(digitModel);
                        effect->position[0] += effect->velocity[0];
                        effect->position[1] += effect->velocity[1];
                        effect->position[2] += effect->velocity[2];
                } else {
                    effect->code = 0;
                }
                break;
            }
            break;
        case CGP_EFFECT_DAMAGE_MANY_DESTROYED:
            if (side == effect->side) {
                break;
            }
            switch (effect->phase) {
            case CGP_EFFECT_PHASE_WAIT:
                if (effect->timer != 0) {
                    effect->timer--;
                } else {
                    effect->phase = CGP_EFFECT_PHASE_SHOW;
                    effect->timer = 15;
                }
                break;
            case CGP_EFFECT_PHASE_SHOW:
                if (effect->timer != 0) {
                    effect->timer--;
                        offset[3] = 1.0f;
                        if (effect->stack != 0) {
                            offset[0] = effect->stack->position[0];
                            offset[1] = effect->stack->position[1];
                            offset[2] = effect->stack->position[2];
                        } else {
                            offset[0] = 0.0f;
                            offset[1] = -1.81f;
                            offset[2] = 0.0f;
                        }
                        xglMatrixTrans(local.m, local.m, offset);
                        CDEMatrixSub(local.m);
                        xglMatrixTrans(local.m, local.m, effect->position);
                        offset[0] = 0.15f;
                        offset[1] = 0.0f;
                        offset[2] = 0.1f;
                        offset[3] = 1.0f;
                        digit = effect->value / 10;
                        if (digit != 0) {
                            xglMatrixTrans(quad.m, local.m, offset);
                            digitModel = CARD_EFFECT_DIGIT_MODEL(CARD_EFFECT_DIGITS, digit);
                            nmlModelSetPlace(&quad.m);
                            nmlModelSetTexture(CARD_EFFECT_DIGIT_TEXTURE);
                            nmlModelSetLight(&asColor, &asDir);
                            nmlModelEntryCard(digitModel);
                        }
                        offset[0] = 0.25f;
                        digit = effect->value % 10;
                        digitModel = CARD_EFFECT_DIGIT_MODEL(CARD_EFFECT_DIGITS, digit);
                        xglMatrixTrans(quad.m, local.m, offset);
                        nmlModelSetPlace(&quad.m);
                        nmlModelSetTexture(CARD_EFFECT_DIGIT_TEXTURE);
                        nmlModelSetLight(&asColor, &asDir);
                        nmlModelEntryCard(digitModel);
                        effect->position[0] += effect->velocity[0];
                        effect->position[1] += effect->velocity[1];
                        effect->position[2] += effect->velocity[2];
                } else {
                    effect->code = 0;
                }
                break;
            }
            break;
        }
    }
}

/*
 * Opens a card list window on the cards of a hand that has reached the hand limit. Returns
 * -1 once the window is laid out and 0 when the hand is below the limit.
 */
s32 CGPCheckTeMax(CardListWindow *list, CardHand *hand)
{
    s32 count;
    s32 i;

    count = CardPlayHandCnt(hand);
    if (count >= CGP_HAND_LIMIT) {
        list->baseColor = CGP_HAND_LIST_BASE_COLOR;
        list->cursorColor = CGP_HAND_LIST_CURSOR_COLOR;
        list->x = CGP_HAND_LIST_X;
        list->y = CGP_HAND_LIST_Y;
        list->textLength = CGP_HAND_LIST_TEXT_LENGTH;
        list->rows = CGP_HAND_LIST_ROWS;
        list->active = 0;
        list->cursor = 0;
        list->top = 0;
        list->count = count;
        for (i = 0; i < count; i++) {
            list->entries[i].cardId = hand->cards[i];
        }
        return -1;
    }
    return 0;
}

/*
 * Finishes a played command: takes the card from hand slot index (when index is not -1), pays
 * its reserve cost and puts the card on the junk pile.
 */
void CGPCommFinishSub(CardGameWork *work, CardPlaySide *side, s32 index)
{
    s32 cost;
    s32 j;
    s32 junkCnt;

    if (index >= 0) {
        side->battle[0].cmdView.playedCard = side->hand.cards[index];
        side->hand.cards[index] = -1;
        CardPlayCleanHand(&side->hand);
    }

    cost = work->definitions[side->battle[0].cmdView.playedCard].reserveCost;
    for (j = 0; j < cost; j++) {
        CardPlayCostCard(side);
    }

    junkCnt = CardPlayJunkCnt(side);
    side->junk[junkCnt] = side->battle[0].cmdView.playedCard;
    side->battle[0].cmdView.playedCard = -1;
}





/*
 * Starts a card game: clears the game flags, the command control, the messages,
 * the effect table and both command queues, resets the cursor and the stage
 * models, then reads the game's model, texture and effect files into the arena.
 */
void CardGameInit(CardGameWork *work)
{
    s32 i;
    s32 shown;

    /* Game flags, command control and messages. */
    work->flags = 0;
    if (work->command.input[0].held & PAD_TRIANGLE) {
        work->flags = CGP_FLAG_HELD_TRIANGLE;
    }
    work->command.interruptMessId = 1;
    work->command.interruptMessTimer = CGP_INTERRUPT_MESS_FRAMES;
    work->command.turn = 0;
    work->command.phase = 0;
    work->command.phaseTimer = 0;
    work->command.step = 0;
    work->command.flowRequest = 0;
    work->command.turnCountdown = 0;
    work->command.recoveryApplied = 0;
    work->command.commandIndex = 0;
    work->command.exitMenuItem = 0;
    work->aiStep = 0;
    work->command.aiPhase = 0;
    work->errorMessSide = 0;
    work->errorMessKind = 0;
    work->infoPanelPhase = CGP_INFO_PANEL_INIT;
    work->resultWait = 0;
    work->errorMessReason = 0;
    work->mess = 0;
    work->interruptMess = 0;
    work->permanentMess = 0;
    work->cursorMoveStep[0] = 0;
    work->cursorMoveStep[1] = 0;
    work->effectsActive = 0;
    work->effectsSet = 0;
    work->helpTopic = CARD_HELP_PAGE_DRAW_PHASE;
    work->errorMessValue = -1;
    for (i = 0; i < CGP_EFFECT_COUNT; i++) {
        work->effects[i].side = 0;
        work->effects[i].code = 0;
        work->effects[i].phase = 0;
        work->effects[i].timer = 0;
    }
    {
        s32 freeMark = -1;
        s16 *queueCommand = &work->commandQueue[1][0][1];
        s16 *queuePhase = &work->commandQueue[1][0][0];

        /* The player's queue lies 40 entries of four halfwords before the enemy's. */
        for (i = 0; i < CGP_QUEUE_SLOTS; i++) {
            queuePhase[-CGP_QUEUE_SIDE_DISTANCE] = freeMark;
            queueCommand[-CGP_QUEUE_SIDE_DISTANCE] = 0;
            queuePhase[0] = freeMark;
            queueCommand[0] = 0;
            queuePhase += 4;
            queueCommand += 4;
        }
    }
    /* Cursor, stage models and rotation. */
    CardCursorPassive(&work->cursor);
    CGPSetMessage(work, 0);
    work->command.slideDirection = 0;
    work->rotStageVector[1] = CGP_ROT_STAGE_START_Y;
    work->rotStageVector[0] = CGP_ROT_STAGE_START_X;
    for (i = 15; i >= 0; i--) {
        work->models[i].visible = 0;
    }
    /* models 4..7 and 15 are drawn from the start, together with the rotating stage */
    shown = 1;
    work->models[4].visible = shown;
    work->models[5].visible = shown;
    work->models[6].visible = shown;
    work->models[7].visible = shown;
    work->models[15].visible = shown;
    work->rotStageActive = shown;
    work->rotStageCooldown = 0;
    work->rotStageFlag = 0;
    work->rotStageId = 0;
    work->rotStageVector[1] = CGP_ROT_STAGE_START_Y;
    work->rotStageVector[2] = 0.0f;
    work->rotStageVector[0] = CGP_ROT_STAGE_START_X;
    /* Model, texture and effect files of the card game. */
    CardFread(CGP_ARENA(0x0), "game\\E_L.lex");
    CardFread(CGP_ARENA(0x400), "game\\E_S.lex");
    CardFread(CGP_ARENA(0x800), "game\\down_L.lex");
    CardFread(CGP_ARENA(0xC00), "game\\down_s.lex");
    CardFread(CGP_ARENA(0x1000), "game\\on.lex");
    CardFread(CGP_ARENA(0x1400), "game\\off.lex");
    CardFread(CGP_ARENA(0x1800), "game\\NewYama.lex");
    CardFread(CGP_ARENA(0x2800), "game\\Ptern.lex");
    CardFread(CGP_ARENA(0x3800), "game\\Etern.lex");
    CardFread(CGP_ARENA(0x4800), "game\\P1tern.lex");
    CardFread(CGP_ARENA(0x5800), "game\\P2tern.lex");
    CardFread(CGP_ARENA(0x6800), "game\\Pwin.lex");
    CardFread(CGP_ARENA(0x7800), "game\\Ewin.lex");
    CardFread(CGP_ARENA(0x8800), "game\\P1win.lex");
    CardFread(CGP_ARENA(0x9800), "game\\P2win.lex");
    CardFread(CGP_ARENA(0xA800), "game\\dlow.lex");
    CardFread(CGP_ARENA(0xB800), "game\\move.lex");
    CardFread(CGP_ARENA(0xC800), "game\\set.lex");
    CardFread(CGP_ARENA(0xD800), "game\\event.lex");
    CardFread(CGP_ARENA(0xE800), "game\\battle.lex");
    CardFread(CGP_ARENA(0xF800), "game\\adjust.lex");
    CardFread(CGP_ARENA(0x10800), "game\\block.lex");
    CardFread(CGP_ARENA(0x11800), "game\\NewYama1P.lex");
    CardFread(CGP_ARENA(0x15800), "game\\NewYama2P.lex");
    CardFread(CGP_ARENA(0x19800), "game\\HPbar.lex");
    CardFread(CGP_ARENA(0x1A800), "game\\dlow2.lex");
    CardFread(CGP_ARENA(0x1B800), "game\\move2.lex");
    CardFread(CGP_ARENA(0x1C800), "game\\set2.lex");
    CardFread(CGP_ARENA(0x1D800), "game\\event2.lex");
    CardFread(CGP_ARENA(0x1E800), "game\\battle2.lex");
    CardFread(CGP_ARENA(0x1F800), "game\\adjust2.lex");
    CardFread(CGP_ARENA(0x20800), "game\\block2.lex");
    CardFread(CGP_ARENA(0x21800), "game\\tern.lex");
    CardFread(CGP_ARENA(0x22800), "game\\phase1.lex");
    CardFread(CGP_ARENA(0x23800), "game\\phase2.lex");
    CardFread(CGP_ARENA(0x24800), "game\\phase3.lex");
    CardFread(CGP_ARENA(0x25800), "game\\phase4.lex");
    CardFread(CGP_ARENA(0x26800), "game\\phase5.lex");
    CardFread(CGP_ARENA(0x27800), "game\\phase6.lex");
    CardFread(CGP_ARENA(0x28800), "game\\phase7.lex");
    CardFread(CGP_ARENA(0x29800), "game\\up_R.lex");
    CardFread(CGP_ARENA(0x2B000), "game\\up_L.lex");
    CardFread(CGP_ARENA(0x39800), "game\\up_LR.bin");
    CardFread(CGP_ARENA(0x83400), "game\\yama.xtx");
    CardFread(CGP_ARENA(0x85400), "game\\HPbar.xtx");
    CardFread(CGP_ARENA(0x96000), "game\\mat_mojiJ.lex");
    CardFread(CGP_ARENA(0x9A000), "game\\mat_mojiJ.xtx");
    CardFread(CGP_ARENA(0x30000), "deckmake\\battlewin1.lex");
    CardFread(CGP_ARENA(0x31800), "deckmake\\battlewin2.lex");
    CardFread(CGP_ARENA(0x33000), "deckmake\\battlewin3.lex");
    CardFread(CGP_ARENA(0x34800), "deckmake\\battlewin4.lex");
    CardFread(CGP_ARENA(0x36000), "deckmake\\battlewin5.lex");
    CardFread(CGP_ARENA(0x37800), "deckmake\\battlewin.bin");
    CardFread(CGP_ARENA(0x40000), "game\\pop1.lex");
    CardFread(CGP_ARENA(0x44000), "game\\pop2.lex");
    CardFread(CGP_ARENA(0x49000), "game\\pop.xtx");
    CardFread(CGP_ARENA(0xA8000), "game\\NewPop1P.lex");
    CardFread(CGP_ARENA(0xAC000), "game\\NewPop2P.lex");
    CardFread(CGP_ARENA(0xB0000), "game\\NewPop.xtx");
    CardFread(CGP_ARENA(0x50000), "game\\victory1.lex");
    CardFread(CGP_ARENA(0x50510), "game\\victory2.lex");
    CardFread(CGP_ARENA(0x50A20), "game\\victory3.lex");
    CardFread(CGP_ARENA(0x50F30), "game\\victory4.lex");
    CardFread(CGP_ARENA(0x51440), "game\\victory5.lex");
    CardFread(CGP_ARENA(0x51950), "game\\victory6.lex");
    CardFread(CGP_ARENA(0x51E60), "game\\victory7.lex");
    CardFread(CGP_ARENA(0x52370), "game\\NewLost1.lex");
    CardFread(CGP_ARENA(0x52880), "game\\NewLost2.lex");
    CardFread(CGP_ARENA(0x52D90), "game\\victory.bin");
    CardFread(CGP_ARENA(0x60000), "game\\NewLost1.xtx");
    CardFread(CGP_ARENA(0x63680), "game\\NewLost2.xtx");
    CardFread(CGP_ARENA(0x66D00), "game\\NewLost3.xtx");
    CardFread(CGP_ARENA(0x100000), "effect\\effect0.lex");
    CardFread(CGP_ARENA(0x100800), "effect\\effect1.lex");
    CardFread(CGP_ARENA(0x101000), "effect\\atac_a.bin");
    CardFread(CGP_ARENA(0x114000), "effect\\atac_b.bin");
    CardFread(CGP_ARENA(0x128000), "effect\\bom.bin");
    CardFread(CGP_ARENA(0x140000), "effect\\flas.bin");
    CardFread(CGP_ARENA(0x167000), "effect\\misl.bin");
    CardFread(CGP_ARENA(0x185000), "effect\\rang.bin");
    CardFread(CGP_ARENA(0x1A0000), "effect\\shot.bin");
    CardFread(CGP_ARENA(0x2D8000), "effect\\zan.bin");
    CardFread(CGP_ARENA(0x2F0000), "effect\\atac_c.bin");
    CardFread(CGP_ARENA(0x304000), "effect\\atac_d.bin");
}

/*
 * Starts the next phase: phases 0..6 belong to one side and 7..13 to the other, and after
 * phase 13 the turn counter advances. Clears the per-phase state and shows the phase's
 * messages. Called by the computer player's phase handlers (card_enemy.c) and the phase
 * routines of this TU.
 */
void CGPNextTurnSub(CardGameWork *work)
{
    work->infoPanelPhase = CGP_INFO_PANEL_INIT;
    work->command.phase++;
    if (work->command.phase >= CGP_PHASE_COUNT) {
        work->command.phase = 0;
        work->command.turn++;
        xglSoundEffectNormalID(CGP_SE_NEXT_TURN, 0);
    } else if (work->command.phase == CGP_SIDE1_FIRST_PHASE) {
        xglSoundEffectNormalID(CGP_SE_NEXT_TURN, 0);
    } else {
        xglSoundEffectNormalID(CGP_SE_NEXT_PHASE, 0);
    }
    work->command.interruptMessId = work->command.phase + 1;
    work->command.interruptMessTimer = CGP_INTERRUPT_MESS_FRAMES;
    work->command.phaseTimer = 0;
    work->command.step = 0;
    work->aiStep = 0;
    work->command.aiPhase = 0;
    CardCursorPassive(&work->cursor);
    work->cursor.controllingSide = -1;
    work->cursorMoveStep[0] = 0;
    work->cursorMoveStep[1] = 0;
    CGPSetMessage(work, work->command.phase);
    CGPSetInterruptMess(work, work->command.phase + 1);
    work->helpTopic = HelpPageList[work->command.phase];
    work->rotStageVector[1] = -20.0f;
    work->command.slideDirection = 0;
    work->rotStageVector[0] = -0.9f;
}



/*
 * Swap a disposal slot's stack with a battle slot's and start the ten-frame
 * slide that carries each of them to the other's position.
 */
void CGPMoveDSBATTLEwork(CardLayerStack *disposalStack, CardLayerStack *battleStack)
{
    CardMoveWork *disposal = (CardMoveWork *) disposalStack;
    CardMoveWork *battle = (CardMoveWork *) battleStack;
    CardMoveWork moved;
    s32 i;

    if (disposal->moveStep != 0) {
        float steps = disposal->moveStep;

        disposal->position[0] += disposal->velocity[0] * steps;
        disposal->position[1] += disposal->velocity[1] * steps;
        disposal->position[2] += disposal->velocity[2] * steps;
    }
    if (battle->moveStep != 0) {
        float steps = battle->moveStep;

        battle->position[0] += battle->velocity[0] * steps;
        battle->position[1] += battle->velocity[1] * steps;
        battle->position[2] += battle->velocity[2] * steps;
    }

    moved.flags = disposal->flags;
    moved.power = disposal->power;
    moved.powerLimit = disposal->powerLimit;
    moved.position[0] = disposal->position[0];
    moved.position[1] = disposal->position[1];
    moved.position[2] = disposal->position[2];
    moved.shuntPosition[0] = disposal->shuntPosition[0];
    moved.shuntPosition[1] = disposal->shuntPosition[1];
    moved.shuntPosition[2] = disposal->shuntPosition[2];
    for (i = 0; i < 40; i++) {
        moved.layers[i].powerPlus = disposal->layers[i].powerPlus;
        moved.layers[i].preservedByte01 = disposal->layers[i].preservedByte01;
        moved.layers[i].boostCount = disposal->layers[i].boostCount;
        moved.layers[i].life = disposal->layers[i].life;
        moved.layers[i].preservedByte04 = disposal->layers[i].preservedByte04;
        moved.layers[i].damageTimer = disposal->layers[i].damageTimer;
        moved.layers[i].cardId = disposal->layers[i].cardId;
    }

    disposal->flags = battle->flags;
    disposal->power = battle->power;
    disposal->powerLimit = battle->powerLimit;
    disposal->moveStep = 10;
    disposal->velocity[0] = (disposal->position[0] - battle->position[0]) / 10.0f;
    disposal->velocity[1] = (disposal->position[1] - battle->position[1]) / 10.0f;
    disposal->velocity[2] = 0.0f;
    disposal->position[0] = battle->position[0];
    disposal->position[1] = battle->position[1];
    disposal->position[2] = 0.12f;
    disposal->shuntPosition[0] = battle->shuntPosition[0];
    disposal->shuntPosition[1] = battle->shuntPosition[1];
    disposal->shuntPosition[2] = battle->shuntPosition[2];
    for (i = 0; i < 40; i++) {
        disposal->layers[i].life = battle->layers[i].life;
        disposal->layers[i].cardId = battle->layers[i].cardId;
    }

    battle->flags = moved.flags;
    battle->power = moved.power;
    battle->powerLimit = moved.powerLimit;
    battle->moveStep = 10;
    battle->velocity[0] = (battle->position[0] - moved.position[0]) / 10.0f;
    battle->velocity[1] = (battle->position[1] - moved.position[1]) / 10.0f;
    battle->velocity[2] = 0.0f;
    battle->position[0] = moved.position[0];
    battle->position[1] = moved.position[1];
    battle->position[2] = 0.12f;
    for (i = 0; i < 40; i++) {
        battle->layers[i].powerPlus = moved.layers[i].powerPlus;
        battle->layers[i].preservedByte01 = moved.layers[i].preservedByte01;
        battle->layers[i].boostCount = moved.layers[i].boostCount;
        battle->layers[i].life = moved.layers[i].life;
        battle->layers[i].preservedByte04 = moved.layers[i].preservedByte04;
        battle->layers[i].damageTimer = moved.layers[i].damageTimer;
        battle->layers[i].cardId = moved.layers[i].cardId;
        battle->shuntPosition[0] = moved.shuntPosition[0];
        battle->shuntPosition[1] = moved.shuntPosition[1];
        battle->shuntPosition[2] = moved.shuntPosition[2];
    }
}


/*
 * Attaches the equipment card at hand position index to a weapon stack: the
 * weapon's displaced previous equipment card goes to the junk pile, the new
 * card becomes layer one with its operation duration as life, takes the card's
 * reserve cost from the draw pile and its play cost from the play points, and
 * leaves the hand. Shield, Battle Suit and Eyeglasses keep the stack's power.
 */
void CGPequipSub(CardGameWork *work, CardPlaySide *side, CardLayerStack *stack, s32 index)
{
    s32 i;
    s32 junkCount;

    if (stack->layers[1].cardId >= 0) {
        junkCount = CardPlayJunkCnt(side);
        stack->power = work->definitions[stack->layers[0].cardId].basePower;
        stack->powerLimit = work->definitions[stack->layers[0].cardId].basePower;
        side->junk[junkCount] = stack->layers[1].cardId;
    }
    stack->layers[1].life = work->definitions[side->hand.cards[index]].operationDuration;
    if (side->hand.cards[index] != CARD_ID_SHIELD && side->hand.cards[index] != CARD_ID_BATTLE_SUIT &&
        side->hand.cards[index] != CARD_ID_EYEGLASSES) {
        stack->power = work->definitions[side->hand.cards[index]].basePower;
        stack->powerLimit = work->definitions[side->hand.cards[index]].basePower;
    }
    stack->layers[1].powerPlus = 0;
    stack->layers[1].flag = 0;
    stack->layers[1].boostCount = 0;
    stack->layers[1].cardId = side->hand.cards[index];
    for (i = 0; i < work->definitions[side->hand.cards[index]].reserveCost; i++) {
        CardPlayCostCard(side);
    }
    side->hand.playPoints[0] -= work->definitions[side->hand.cards[index]].playCost;
    side->hand.cards[index] = -1;
    CardPlayCleanHand(&side->hand);
}

/*
 * Whether one equipment card may be attached to one weapon card. Both must be battle cards.
 * A Mech-only weapon takes a Mech card (not Lv 1 or Lv 10 KOS-MOS, Durandal or Zohar Emulator;
 * Lv 10 KOS-MOS only on the Third Armament). A Human weapon takes a Human or Realian card, except
 * the Ziggy cards and Albedo. Every other weapon takes none.
 */
s32 CGPWeaponEquipChk(CardGameWork *work, s32 weaponId, s32 equipId)
{
    u8 weaponKind;
    u8 equipKind;
    u16 weaponFlags;
    u16 equipFlags;
    s32 allowed;

    weaponKind = work->definitions[weaponId].kind;
    equipKind = work->definitions[equipId].kind;
    weaponFlags = work->definitions[weaponId].flags;
    equipFlags = work->definitions[equipId].flags;
    allowed = 0;
    if ((weaponKind & CARD_KIND_BATTLE) && (equipKind & CARD_KIND_BATTLE)) {
        switch (weaponFlags) {
        case CARD_ATTR_WEAPON_MECH:
            if ((equipFlags & CARD_ATTR_MECH) && equipId != CARD_ID_LV_1_KOS_MOS && equipId != CARD_ID_DURANDAL &&
                equipId != CARD_ID_ZOHAR_EMULATOR)
            {
                if (equipId != CARD_ID_LV_10_KOS_MOS) {
                    allowed = 1;
                } else if (weaponId == CARD_ID_THIRD_ARMAMENT) {
                    allowed = 1;
                }
            }
            break;
        case CARD_ATTR_WEAPON_HUMAN:
            if (((equipFlags & CARD_ATTR_HUMAN) || (equipFlags & CARD_ATTR_REALIAN)) &&
                equipId != CARD_ID_LV_1_ZIGGY && equipId != CARD_ID_LV_10_ZIGGY && equipId != CARD_ID_ALBEDO)
            {
                allowed = 1;
            }
            break;
        }
    }
    return allowed;
}

/* Equips the held hand card to the target stack, plays the decide sound and restarts the phase timer. */
static inline void CGPAttachHeldCard(CardGameWork *work, CardPlaySide *ownSide,
                                     CardLayerStack *stack, s32 handIndex)
{
    CGPequipSub(work, ownSide, stack, handIndex);
    xglSoundEffectNormalID(SE_DECIDE, 0);
    work->command.phaseTimer = 1;
}

/*
 * Attaches the picked-up hand card (held) to the card under the cursor (target), which must hold a card
 * in the acting side's disposal or battle row; every refusal plays the buzzer. The Third Armament attaches
 * only to Lv 10 KOS-MOS, a Mech weapon to a Mech card except Lv 1 and Lv 10 KOS-MOS, Durandal and Zohar
 * Emulator, and a Human weapon to a Human or Realian card other than the Ziggy cards and Albedo.
 */
void CGPDisposeWeaponSub(s32 side, CardGameWork *work, CardCursorPosition held,
                         CardCursorPosition target)
{
    CardPlaySide *ownSide;
    CardLayerStack *stack;
    s32 disposalArea;
    s32 battleArea;
    s32 area;
    u16 targetFlags;

    ownSide = &work->player;
    disposalArea = CGP_AREA_PLAYER_DISPOSAL;
    battleArea = CGP_AREA_PLAYER_BATTLE;
    if (side != 0) {
        ownSide = &work->enemy;
        disposalArea = CGP_AREA_ENEMY_DISPOSAL;
        battleArea = CGP_AREA_ENEMY_BATTLE;
    }
    area = target.zone;
    if ((disposalArea == area || battleArea == area) && target.cardId >= 0) {
        if (disposalArea == area) {
            stack = &ownSide->disposal[target.index].battle;
        } else {
            stack = &ownSide->battle[target.index].battle;
        }
        if (held.cardId == CARD_ID_THIRD_ARMAMENT) {
            if (target.cardId == CARD_ID_LV_10_KOS_MOS) {
                CGPAttachHeldCard(work, ownSide, stack, held.index);
            } else {
                xglSoundEffectNormalID(SE_BUZZER, 0);
            }
        } else if ((work->definitions[held.cardId].flags & CARD_ATTR_WEAPON_MECH) &&
                   (work->definitions[target.cardId].flags & CARD_ATTR_MECH)) {
            if (target.cardId != CARD_ID_LV_1_KOS_MOS && target.cardId != CARD_ID_LV_10_KOS_MOS &&
                target.cardId != CARD_ID_DURANDAL && target.cardId != CARD_ID_ZOHAR_EMULATOR) {
                CGPAttachHeldCard(work, ownSide, stack, held.index);
            } else {
                xglSoundEffectNormalID(SE_BUZZER, 0);
            }
        } else if (work->definitions[held.cardId].flags & CARD_ATTR_WEAPON_HUMAN) {
            targetFlags = work->definitions[target.cardId].flags;
            if (((targetFlags & CARD_ATTR_HUMAN) || (targetFlags & CARD_ATTR_REALIAN)) && target.cardId != CARD_ID_LV_1_ZIGGY &&
                target.cardId != CARD_ID_LV_10_ZIGGY && target.cardId != CARD_ID_ALBEDO) {
                CGPAttachHeldCard(work, ownSide, stack, held.index);
            } else {
                xglSoundEffectNormalID(SE_BUZZER, 0);
            }
        } else {
            xglSoundEffectNormalID(SE_BUZZER, 0);
        }
    } else {
        xglSoundEffectNormalID(SE_BUZZER, 0);
    }
}

/*
 * Board area the cursor points at. Zone 0 is the enemy hand (area 0 on one of its cards, 1 past
 * them) and zone 9 the player hand (area 9 on a card, 8 past them). Zones 1..4 are the enemy board
 * row (area 2 for its last column, 3 for its first, 4 otherwise) and zones 5..8 the player's
 * (area 7 for its last column, 6 for its first, 5 otherwise).
 */
s32 CGPCursor2Area(CardGameWork *work)
{
    s32 area;
    s32 count;

    if (work->cursorZone < 5) {
        area = 4;
        if (work->cursorZone == 0) {
            count = CardPlayHandCnt(&work->enemy.hand);
            area = 1;
            if (work->cursorIndex < count) {
                area = 0;
            }
        } else {
            if (work->cursorIndex == 0) {
                area = 3;
            }
            if (work->cursorIndex == 3) {
                area = 2;
            }
        }
    } else {
        area = 5;
        if (work->cursorZone == 9) {
            count = CardPlayHandCnt(&work->player.hand);
            area = 8;
            if (work->cursorIndex < count) {
                area = 9;
            }
        } else {
            if (work->cursorIndex == 0) {
                area = 6;
            }
            if (work->cursorIndex == 3) {
                area = 7;
            }
        }
    }
    return area;
}

/*
 * Fills the cursor position for the area under the cursor: its slot index, the card id there and
 * the card stacked on it (-1 when none).
 */
void CGPCursor2Pos(CardCursorPosition *position, CardGameWork *work)
{
    position->cardId = -1;
    position->stackedCard = -1;
    position->zone = CGPCursor2Area(work);
    switch (position->zone) {
    case 0:
        position->index = work->cursorIndex;
        position->cardId = work->enemy.hand.cards[position->index];
        break;
    case 9:
        position->index = work->cursorIndex;
        position->cardId = work->player.hand.cards[position->index];
        break;
    case 2:
        position->index = 4 - work->cursorZone;
        position->cardId = work->enemy.disposal[position->index].battle.layers[0].cardId;
        if (position->cardId >= 0 &&
            work->enemy.disposal[position->index].battle.layers[1].cardId >= 0)
        {
            position->stackedCard = work->enemy.disposal[position->index].battle.layers[1].cardId;
        }
        break;
    case 3:
        position->index = 4 - work->cursorZone;
        position->cardId = work->enemy.operations[position->index];
        break;
    case 4:
        position->index = 5 - work->cursorIndex - work->cursorZone;
        position->cardId = work->enemy.battle[position->index].battle.layers[0].cardId;
        if (position->cardId >= 0 &&
            work->enemy.battle[position->index].battle.layers[1].cardId >= 0)
        {
            position->stackedCard = work->enemy.battle[position->index].battle.layers[1].cardId;
        }
        break;
    case 6:
        position->index = work->cursorZone - 5;
        position->cardId = work->player.disposal[position->index].battle.layers[0].cardId;
        if (position->cardId >= 0 &&
            work->player.disposal[position->index].battle.layers[1].cardId != 0)
        {
            position->stackedCard = work->player.disposal[position->index].battle.layers[1].cardId;
        }
        break;
    case 7:
        position->index = work->cursorZone - 5;
        position->cardId = work->player.operations[position->index];
        break;
    case 5:
        position->index = work->cursorIndex + work->cursorZone - 6;
        position->cardId = work->player.battle[position->index].battle.layers[0].cardId;
        if (position->cardId >= 0 &&
            work->player.battle[position->index].battle.layers[1].cardId != 0)
        {
            position->stackedCard = work->player.battle[position->index].battle.layers[1].cardId;
        }
        break;
    case 8:
        position->index = work->cursorIndex - CardPlayHandCnt(&work->player.hand);
        position->cardId = work->player.junk[position->index];
        break;
    case 1:
        position->index = work->cursorIndex - CardPlayHandCnt(&work->enemy.hand);
        position->cardId = work->enemy.junk[position->index];
        break;
    }
}






/*
 * Clamps the cursor index to 0..3 and resolves the card under the cursor (CGPCursor2Pos). Zones 2,
 * 3, 6 and 7 get a highlight of 0.175; zones 4 and 5 get 0.355 and their cursor zone is rounded down
 * to the nearest odd value. Returns the resolved card id.
 */
s16 CGPCMSub(CardGameWork *work)
{
    CardCursorPosition position;

    work->highlightScaleX = work->highlightScaleY = 1.0f;
    if (work->cursorIndex < 0) {
        work->cursorIndex = 3;
    }
    if (work->cursorIndex >= 4) {
        work->cursorIndex = 0;
    }
    CGPCursor2Pos(&position, work);
    switch (position.zone) {
    case 2:
    case 3:
    case 6:
    case 7:
        work->highlightX = work->highlightY = 0.175f;
        break;
    case 4:
    case 5:
        work->cursorZone = ((work->cursorZone - 1) & 0xFE) + 1;
        work->highlightX = work->highlightY = 0.355f;
        break;
    }
    return position.cardId;
}






/*
 * Spacing of the cards of a row: the distance from its first to its last
 * position (entries 0 and 3 of the position table) divided by the gaps between
 * count cards. A pile is placed as a row of one card, whose gap count is -1.
 */
static inline float CGPRowSpacing(float pos[][3], s32 count)
{
    return (pos[0][0] - pos[3][0]) / (count - 1);
}

/*
 * Puts the cursor on the table position of its zone and slot (zones 1..8, four
 * slots each) and resolves the zone's highlight (CGPCMSub).
 */
static inline void CGPCursorToSlot(CardGameWork *work)
{
    float *slot;

    CGPCMSub(work);
    slot = CursorTbl[(work->cursorZone - 1) * 4 + work->cursorIndex];
    work->cursorPos[0] = slot[0];
    work->cursorPos[1] = slot[1];
    work->cursorPos[2] = slot[2];
}

/*
 * Moves one controller's cursor (side 0 or 1) over the board zones and the two card piles, and draws
 * the card preview while the preview button is held (returns 1 when it was drawn). Entering the player's
 * pile slot (zone 9, slot 3) or the enemy's (zone 0, slot 0) walks that pile when it holds cards; a move
 * that starts a pile leaves the position's zone unset, as the original does.
 */
s32 CGPCursorMove2(s32 side, CardGameWork *work)
{
    CardCursorPosition position;
    CardPlaySide *ownSide;
    u16 held;
    u16 repeat;
    u16 pressed;
    s32 canPreview = 1;

    ownSide = &work->player;
    held = work->command.input[0].held;
    pressed = work->command.input[0].pressed;
    repeat = work->command.input[0].repeat;
    if (side != 0) {
        held = work->command.input[1].held;
        ownSide = &work->enemy;
        pressed = work->command.input[1].pressed;
        repeat = work->command.input[1].repeat;
    }
    if (work->cursorMoveStep[0] != 0) {
        s32 junkCnt;

        junkCnt = CardPlayJunkCnt(&work->player);
        work->cursorPos[0] = JunkPos1P[0][0];
        work->cursorPos[1] = JunkPos1P[0][1];
        work->cursorPos[2] = JunkPos1P[0][2];
        switch (work->cursorMoveStep[0]) {
        case CGP_PILE_TOP:
            if (pressed & PAD_UP) {
                work->cursorMoveStep[0] = CGP_PILE_BOARD;
                work->cursorZone--;
                xglSoundEffectNormalID(SE_CURSOR, 0);
            } else if (pressed & PAD_CIRCLE) {
                work->junkPick[0] = 0;
                work->cursorMoveStep[0] = CGP_PILE_BROWSE;
                xglSoundEffectNormalID(SE_DECIDE, 0);
                CardPlayCleanJunk(&work->player);
            }
            position.stackedCard = -1;
            position.cardId = work->player.junk[junkCnt - 1];
            break;
        case CGP_PILE_BROWSE:
            if (repeat & PAD_LEFT) {
                if (work->junkPick[0] != 0) {
                    work->junkPick[0]--;
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                }
            } else if (repeat & PAD_RIGHT) {
                if (work->junkPick[0] < junkCnt - 1) {
                    work->junkPick[0]++;
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                }
            }
            if (pressed & PAD_CROSS) {
                work->cursorMoveStep[0] = CGP_PILE_TOP;
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
            CGCPJunkDispSub(&work->player, work->junkPick[0]);
            position.cardId = work->player.junk[work->junkPick[0]];
            position.stackedCard = -1;
            break;
        }
    } else if (work->cursorMoveStep[1] != 0) {
        CardPlaySide *enemy;
        s32 junkCnt;

        enemy = &work->enemy;
        junkCnt = CardPlayJunkCnt(enemy);
        work->cursorPos[0] = JunkPos2P[0][0];
        work->cursorPos[1] = JunkPos2P[0][1];
        work->cursorPos[2] = JunkPos2P[0][2];
        switch (work->cursorMoveStep[1]) {
        case CGP_PILE_TOP:
            if (pressed & PAD_DOWN) {
                work->cursorMoveStep[1] = CGP_PILE_BOARD;
                work->cursorZone++;
                xglSoundEffectNormalID(SE_CURSOR, 0);
            } else if (pressed & PAD_CIRCLE) {
                work->junkPick[1] = 0;
                work->cursorMoveStep[1] = CGP_PILE_BROWSE;
                xglSoundEffectNormalID(SE_DECIDE, 0);
                CardPlayCleanJunk(enemy);
            }
            position.stackedCard = -1;
            position.cardId = work->enemy.junk[junkCnt - 1];
            break;
        case CGP_PILE_BROWSE:
            if (repeat & PAD_LEFT) {
                if (work->junkPick[1] != 0) {
                    work->junkPick[1]--;
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                }
            } else if (repeat & PAD_RIGHT) {
                if (work->junkPick[1] < junkCnt - 1) {
                    work->junkPick[1]++;
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                }
            }
            if (pressed & PAD_CROSS) {
                work->cursorMoveStep[1] = CGP_PILE_TOP;
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
            CGCPJunkDispSub(enemy, work->junkPick[1]);
            position.cardId = work->enemy.junk[work->junkPick[1]];
            position.stackedCard = -1;
            break;
        }
    } else {
        float spacing;
        s16 prevZone;
        s16 area;
        s32 flag;
        s32 started;

        if (pressed & (PAD_UP | PAD_RIGHT | PAD_DOWN | PAD_LEFT)) {
            xglSoundEffectNormalID(SE_CURSOR, 0);
        }
        prevZone = work->cursorZone;
        area = CGPCursor2Area(work);
        /* flag: the cursor stands in one of the two middle columns of a board row, which up/down skips */
        flag = area == CGP_AREA_ENEMY_BATTLE || area == CGP_AREA_PLAYER_BATTLE;
        if (pressed & PAD_UP) {
            work->cursorZone--;
            if (flag && work->cursorZone != 0) {
                work->cursorZone--;
            }
        }
        if (pressed & PAD_DOWN) {
            work->cursorZone++;
            if (flag && work->cursorZone != CGP_AREA_PLAYER_HAND) {
                work->cursorZone++;
            }
        }
        if (work->cursorZone < 0) {
            work->cursorZone = CGP_AREA_PLAYER_HAND;
        }
        if (work->cursorZone >= 10) {
            work->cursorZone = 0;
        }
        /* Entering a pile slot starts walking that pile when it holds cards. */
        started = 0;
        if (prevZone != CGP_AREA_PLAYER_HAND && work->cursorZone == CGP_AREA_PLAYER_HAND &&
            work->cursorIndex == CGP_PLAYER_PILE_SLOT) {
            s32 junkCnt = CardPlayJunkCnt(&work->player);

            if (junkCnt != 0) {
                started++;
                work->cursorMoveStep[0] = CGP_PILE_TOP;
                position.cardId = work->player.junk[junkCnt - 1];
            }
        }
        if (prevZone != CGP_AREA_ENEMY_HAND && work->cursorZone == CGP_AREA_ENEMY_HAND &&
            work->cursorIndex == 0) {
            s32 junkCnt = CardPlayJunkCnt(&work->enemy);

            if (junkCnt != 0) {
                work->cursorMoveStep[1] = CGP_PILE_TOP;
                started++;
                position.cardId = work->enemy.junk[junkCnt - 1];
            }
        }
        if (started == 0) {
            area = CGPCursor2Area(work);
            /*
             * Left and right swap when the stage is rotated, because the hand row is mirrored.
             * Phases 1-3 of a side use the player hand row (zone 9) and phase 4 the other one (zone 0);
             * CGP_SIDE1_FIRST_PHASE gives the phases of the second side.
             */
            flag = 0;
            switch (work->command.phase) {
            case CARD_PHASE_MOVE:
            case CARD_PHASE_EVENT:
            case CARD_PHASE_SET:
            case CARD_PHASE_BLOCK + CARD_PHASES_PER_SIDE:
                if (work->cursorZone == CGP_AREA_PLAYER_HAND && work->rotStageId != 0) {
                    flag = 1;
                }
                break;
            case CARD_PHASE_BLOCK:
            case CARD_PHASE_MOVE + CARD_PHASES_PER_SIDE:
            case CARD_PHASE_EVENT + CARD_PHASES_PER_SIDE:
            case CARD_PHASE_SET + CARD_PHASES_PER_SIDE:
                if (work->cursorZone == CGP_AREA_ENEMY_HAND) {
                    flag = work->rotStageId != 0;
                }
                break;
            }
            if (flag) {
                if (pressed & PAD_RIGHT) {
                    work->cursorIndex--;
                }
                if (pressed & PAD_LEFT) {
                    work->cursorIndex++;
                }
            } else {
                if (pressed & PAD_RIGHT) {
                    work->cursorIndex++;
                }
                if (pressed & PAD_LEFT) {
                    work->cursorIndex--;
                }
            }
            if (work->cursorZone == CGP_AREA_ENEMY_HAND) {
                CardPlaySide *enemy = &work->enemy;
                s32 handCnt = CardPlayHandCnt(&enemy->hand);
                s32 junkCnt = CardPlayJunkCnt(enemy);

                work->highlightX = work->highlightY = CGP_HAND_HIGHLIGHT;
                work->highlightScaleX = work->highlightScaleY = 1.0f;
                if (handCnt == 0 && junkCnt == 0) {
                    work->cursorZone = 1;
                    CGPCursorToSlot(work);
                } else {
                    if (work->cursorIndex < 0) {
                        work->cursorIndex = handCnt - 1;
                    }
                    if (work->cursorIndex >= handCnt) {
                        work->cursorIndex = 0;
                    }
                    /* the enemy's hand is laid out from the right: slot 0 is the last card */
                    work->cursorPos[0] = HandPos2P[0][0] -
                        (HandPos2P[0][0] - HandPos2P[3][0]) / (handCnt - 1) * (handCnt - work->cursorIndex - 1);
                    work->cursorPos[1] = HandPos2P[0][1];
                    work->cursorPos[2] = HandPos2P[0][2];
                }
            } else if (work->cursorZone == CGP_AREA_PLAYER_HAND) {
                s32 handCnt = CardPlayHandCnt(&work->player.hand);
                s32 junkCnt = CardPlayJunkCnt(&work->player);

                work->highlightX = work->highlightY = CGP_HAND_HIGHLIGHT;
                work->highlightScaleX = work->highlightScaleY = 1.0f;
                if (handCnt == 0 && junkCnt == 0) {
                    work->cursorZone = 1;
                    CGPCursorToSlot(work);
                } else {
                    if (prevZone != work->cursorZone) {
                        /* coming from another zone: the left side lands on the first card */
                        if (work->cursorIndex < 3) {
                            work->cursorIndex = 0;
                        } else {
                            work->cursorIndex = handCnt;
                        }
                    }
                    if (work->cursorIndex < 0) {
                        work->cursorIndex = handCnt - 1;
                    }
                    if (work->cursorIndex >= handCnt) {
                        work->cursorIndex = 0;
                    }
                    if (handCnt != 0 && work->cursorIndex < handCnt) {
                        spacing = CGPRowSpacing(HandPos1P, handCnt);
                        work->cursorPos[0] = HandPos1P[0][0] - spacing * work->cursorIndex;
                        work->cursorPos[1] = HandPos1P[0][1];
                        work->cursorPos[2] = HandPos1P[0][2];
                    } else {
                        spacing = CGPRowSpacing(JunkPos1P, 0);
                        work->cursorPos[0] = JunkPos1P[0][0] - spacing * (work->cursorIndex - handCnt);
                        work->cursorPos[1] = JunkPos1P[0][1];
                        work->cursorPos[2] = JunkPos1P[0][2];
                    }
                }
            } else {
                CGPCursorToSlot(work);
            }
        }
        if (work->cursorMoveAny == 0) {
            CGPCursor2Pos(&position, work);
        }
        /*
         * A card in the hand row that is not shown (the opponent's, or the
         * player's in the phases the zone-0 row is the active one) is not
         * previewed.
         */
        switch (work->command.phase) {
        case CARD_PHASE_DRAW:
        case CARD_PHASE_MOVE:
        case CARD_PHASE_EVENT:
        case CARD_PHASE_SET:
        case CARD_PHASE_BATTLE:
        case CARD_PHASE_ADJUST:
        case CARD_PHASE_BLOCK + CARD_PHASES_PER_SIDE:
            if (position.zone == CGP_AREA_ENEMY_HAND) {
                canPreview = 0;
            }
            break;
        default:
            canPreview = position.zone != CGP_AREA_PLAYER_HAND;
            break;
        }
    }
    if (held & PAD_TRIANGLE) {
        if (canPreview && position.cardId >= 0) {
            Matrix4 matrix;
            float place[4];
            float scale[4];
            s32 mode;

            position.cardId &= CGP_CARD_ID_MASK;
            place[0] = 0.0f;
            place[1] = CGP_PREVIEW_FULL_Y;
            place[2] = CGP_PREVIEW_FULL_Z;
            place[3] = 1.0f;
            scale[1] = CGP_PREVIEW_FULL_SCALE;
            scale[0] = CGP_PREVIEW_FULL_SCALE;
            scale[3] = 1.0f;
            scale[2] = 1.0f;
            mode = CGP_CARD_MODE_FULL;
            if ((position.stackedCard >= 0 && (held & PAD_R1)) ||
                (ownSide->battle[0].cmdView.playedCard >= 0 && (held & PAD_L1))) {
                /* a side card is enlarged: the middle card shrinks to make room */
                mode = CGP_CARD_MODE_ICON;
                place[1] = CGP_PREVIEW_SMALL_Y;
                place[2] = CGP_PREVIEW_SMALL_Z;
                scale[1] = CGP_PREVIEW_SMALL_SCALE;
                scale[0] = CGP_PREVIEW_SMALL_SCALE;
            }
            xglMatrixUnit(matrix);
            xglMatrixTrans(matrix, matrix, place);
            xglMatrixScale(matrix, matrix, scale);
            CardDisp(&matrix, position.cardId, 1, mode);
            if (position.stackedCard >= 0) {
                mode = CGP_CARD_MODE_ICON;
                place[0] = CGP_PREVIEW_SIDE_X;
                place[1] = CGP_PREVIEW_SMALL_Y;
                place[2] = CGP_PREVIEW_SMALL_Z;
                scale[1] = CGP_PREVIEW_SMALL_SCALE;
                scale[0] = CGP_PREVIEW_SMALL_SCALE;
                scale[3] = 1.0f;
                scale[2] = 1.0f;
                if (held & PAD_R1) {
                    mode = CGP_CARD_MODE_FULL;
                    place[0] = CGP_PREVIEW_ZOOMED_X;
                    place[1] = CGP_PREVIEW_FULL_Y;
                    place[2] = CGP_PREVIEW_FULL_Z;
                    scale[1] = CGP_PREVIEW_FULL_SCALE;
                    scale[0] = CGP_PREVIEW_FULL_SCALE;
                }
                xglMatrixUnit(matrix);
                xglMatrixTrans(matrix, matrix, place);
                xglMatrixScale(matrix, matrix, scale);
                CardDisp(&matrix, position.stackedCard, 1, mode);
                if (!(held & PAD_R1)) {
                    place[0] = CGP_PREVIEW_SIDE_X;
                    place[1] = CGP_PREVIEW_SMALL_Y;
                    place[2] = CGP_PREVIEW_FRAME_Z;
                    xglMatrixUnit(matrix);
                    xglMatrixTrans(matrix, matrix, place);
                    nmlModelSetPlace(&matrix);
                    nmlModelSetTexture(CGP_PREVIEW_FRAME_TEXTURE);
                    nmlModelEntryCard(CGP_PREVIEW_FRAME_RIGHT);
                }
            }
            if (ownSide->battle[0].cmdView.playedCard >= 0) {
                mode = CGP_CARD_MODE_ICON;
                place[0] = -CGP_PREVIEW_SIDE_X;
                place[1] = CGP_PREVIEW_SMALL_Y;
                place[2] = CGP_PREVIEW_SMALL_Z;
                scale[1] = CGP_PREVIEW_SMALL_SCALE;
                scale[0] = CGP_PREVIEW_SMALL_SCALE;
                scale[3] = 1.0f;
                scale[2] = 1.0f;
                if (held & PAD_L1) {
                    mode = CGP_CARD_MODE_FULL;
                    place[0] = -CGP_PREVIEW_ZOOMED_X;
                    place[1] = CGP_PREVIEW_FULL_Y;
                    place[2] = CGP_PREVIEW_FULL_Z;
                    scale[1] = CGP_PREVIEW_FULL_SCALE;
                    scale[0] = CGP_PREVIEW_FULL_SCALE;
                }
                xglMatrixUnit(matrix);
                xglMatrixTrans(matrix, matrix, place);
                xglMatrixScale(matrix, matrix, scale);
                CardDisp(&matrix, ownSide->battle[0].cmdView.playedCard, 1, mode);
                if (!(held & PAD_L1)) {
                    place[0] = -CGP_PREVIEW_SIDE_X;
                    place[1] = CGP_PREVIEW_SMALL_Y;
                    place[2] = CGP_PREVIEW_FRAME_Z;
                    xglMatrixUnit(matrix);
                    xglMatrixTrans(matrix, matrix, place);
                    nmlModelSetPlace(&matrix);
                    nmlModelSetTexture(CGP_PREVIEW_FRAME_TEXTURE);
                    nmlModelEntryCard(CGP_PREVIEW_FRAME_LEFT);
                }
            }
            return 1;
        }
    }
    return 0;
}

/*
 * Moves the cursor (CGPCursorMove2) and discards its result. It is void here, but
 * ov10/ccu_038_exec_sub.h declares it as s32 for its callers, so it is not in
 * include/ov10/cgp.h.
 */
void CGPCursorMove(s32 flag, CardGameWork *work)
{
    CGPCursorMove2(flag, work);
}

/*
 * Puts the cursor on a board area and slot, undoing any pile walk in progress. The area
 * codes are the ones CGPCursor2Area returns: 0 and 1 the enemy hand, 2 and 3 the last and
 * first column of the enemy board row, 4 its two middle columns, 5 the two middle columns
 * of the player board row, 6 and 7 its first and last column, 8 and 9 the player hand.
 */
void CGPSetCursor(CardGameWork *work, u16 area, u16 slot)
{
    work->cursorMoveStep[0] = 0;
    work->cursorMoveStep[1] = 0;
    switch (area) {
    case 0:
    case 1:
        work->cursorZone = 0;
        work->cursorIndex = 0;
        break;
    case 2:
        work->cursorIndex = 3;
        work->cursorZone = 4 - slot;
        break;
    case 3:
        work->cursorIndex = 0;
        work->cursorZone = 4 - slot;
        break;
    case 4:
        if (slot < 2) {
            work->cursorZone = 3;
            work->cursorIndex = 2 - (slot & 1);
        } else {
            work->cursorZone = 2;
            work->cursorIndex = 2 - (slot & 1);
        }
        break;
    case 5:
        if (slot < 2) {
            work->cursorZone = 5;
            work->cursorIndex = (slot & 1) + 1;
        } else {
            work->cursorZone = 6;
            work->cursorIndex = (slot & 1) + 1;
        }
        break;
    case 6:
        work->cursorIndex = 0;
        work->cursorZone = slot + 5;
        break;
    case 7:
        work->cursorIndex = 3;
        work->cursorZone = slot + 5;
        break;
    case 8:
    case 9:
        work->cursorIndex = 0;
        work->cursorZone = 9;
        break;
    }
    CGPCMSub(work);
    if (work->cursorZone == 9) {
        work->cursorPos[0] = HandPos1P[0][0];
        work->cursorPos[1] = HandPos1P[0][1];
        work->cursorPos[2] = HandPos1P[0][2];
    } else {
        work->cursorPos[0] = HandPos2P[0][0];
        work->cursorPos[1] = HandPos2P[0][1];
        work->cursorPos[2] = HandPos2P[0][2];
    }
}

const char cgpPrintYamaFormat[16] = "Yama[%2d]:";
const char cgpPrintCardFormat[8] = "%3d,";
const char cgpPrintNewline[8] = "\n";
const char cgpPrintJunkFormat[16] = "Junk[%2d]:";
const char cgpPrintSuteFormat[16] = "Sute[%2d]:";
const char cgpMessFontCode[8] = "\x0b\x0d";
const char cgpMessColorCode[8] = "\x19\x03";
/* Returns 1 when the command id is one of the entries of CGPCPTbl, otherwise 0. */
s32 CGPChkPlayable(s16 command)
{
    s32 i;

    for (i = 0; i < 9; i++) {
        if (command == CGPCPTbl[i]) {
            return 1;
        }
    }
    return 0;
}


/* Playability test for a command card: 0 when CGPChkPlayable refuses the id, otherwise it checks
 * the cards in play on both sides. */
s32 CGPChkOpePlayable(s32 side, CardGameWork *work, s16 command)
{
    CardPlaySide *own;
    CardPlaySide *foe;
    s32 playable;
    s32 found;
    s32 i;

    own = &work->player;
    foe = &work->enemy;
    playable = CGPChkPlayable(command);
    if (playable == 0) {
        return 0;
    }
    /* The two arms below are identical and emit no code. They mark a join point between the
     * playability test and the switch; without it the compiler folds the side base offsets
     * into the work pointer and the two base registers are lost. */
    if (command == 0) {
        found = 0;
        i = 0;
    } else {
        found = 0;
        i = 0;
    }
    switch (command) {
    case CARD_ID_SO_WEAK:
        found = 0;
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            s32 ownCard = own->battle[i].battle.layers[0].cardId;
            if (ownCard >= 0) {
                found = 1;
                break;
            }
            if (foe->battle[i].battle.layers[0].cardId >= 0) {
                found = 1;
                break;
            }
        }
        if (found) {
            break;
        }
        return 0;
    case CARD_ID_PHENOMENON_CHANGE:
        found = 0;
        for (i = 0; i < CARD_OPERATION_SLOTS; i++) {
            if (foe->operations[i] >= 0) {
                found = 1;
                break;
            }
        }
        if (found) {
            break;
        }
        return 0;
    case CARD_ID_UNKNOWN_ARMAMENT:
        found = 0;
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            s32 ownCard = own->disposal[i].battle.layers[0].cardId;
            if (ownCard >= 0) {
                found = 1;
                break;
            }
            if (foe->disposal[i].battle.layers[0].cardId >= 0) {
                found = 1;
                break;
            }
        }
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            s32 ownCard = own->battle[i].battle.layers[0].cardId;
            if (ownCard >= 0) {
                found = 1;
                break;
            }
            if (foe->battle[i].battle.layers[0].cardId >= 0) {
                found = 1;
                break;
            }
        }
        if (found) {
            break;
        }
        return 0;
    default:
        return 1;
    }
    return 1;
}

/* Clears the end flag of every disposal slot of the side. */
void CardPlayClearEndflg(CardPlaySide *side)
{
    s32 i;

    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        side->disposal[i].battle.flags &= ~CARD_STACK_FLAG_END;
    }
    for (i = 0; i < CARD_OPERATION_SLOTS; i++) {
        side->operationFlags[i] &= ~1;
    }
}

/*
 * Cards a card reserves from the draw pile, zero once its partner is already in
 * play on the side that would play it: VX-7000 is free while Lv 1 or Lv 10 Jr.
 * stands on the board, Lv 1 KOS-MOS and VX-10000 while a Shion card does.
 */
s32 CGPChkCardPlayCost(CardGameWork *work, CardPlaySide *side, s32 command)
{
    s32 cost;

    cost = work->definitions[command].reserveCost;
    switch (command) {
    case CARD_ID_LV_1_KOS_MOS:
    case CARD_ID_VX_10000:
        if (CardPlayCntBtl(side, CARD_ID_LV_1_SHION) || CardPlayCntBtl(side, CARD_ID_SWIM_SUIT_SHION) ||
            CardPlayCntBtl(side, CARD_ID_LV_10_SHION)) {
            cost = 0;
        }
        break;
    case CARD_ID_VX_7000:
        if (CardPlayCntBtl(side, CARD_ID_LV_1_JR) || CardPlayCntBtl(side, CARD_ID_LV_10_JR)) {
            cost = 0;
        }
        break;
    }
    return cost;
}

/* Plays the hand card at index: a battle card runs its command and takes the first empty disposal
 * slot; nothing happens when no slot is free. */
void CardPlayDisployment(u16 side, CardGameWork *workAddress, CardPlaySide *ownSide, s32 index)
{
    CardGameWork *work = workAddress;
    CardDefinition *defs;
    s32 cost;
    s32 i;
    s32 slot;

    defs = work->definitions;
    cost = 0;
    switch (defs[ownSide->hand.cards[index]].kind) {
    case CARD_KIND_BATTLE:
        for (i = 0, slot = -1; i < CARD_BOARD_SLOTS; i++) {
            if (ownSide->disposal[i].battle.layers[0].cardId < 0) {
                slot = i;
                break;
            }
        }
        if (slot < 0) {
            return;
        }
        CardPlayCommandPlay(side, work->commandIds[ownSide->hand.cards[index]],
                            workAddress, 0, 0);
        ownSide->disposal[slot].battle.flags = 1;
        ownSide->disposal[slot].battle.power =
            defs[ownSide->hand.cards[index]].basePower;
        ownSide->disposal[slot].battle.powerLimit =
            defs[ownSide->hand.cards[index]].basePower;
        ownSide->disposal[slot].battle.layers[0].cardId = ownSide->hand.cards[index];
        ownSide->disposal[slot].battle.layers[0].life =
            defs[ownSide->hand.cards[index]].operationDuration;
        cost = CGPChkCardPlayCost(workAddress, ownSide,
                                  ownSide->hand.cards[index]);
        ownSide->disposal[slot].battle.position[0] = ownSide->handMove[index].position[0];
        ownSide->disposal[slot].battle.position[1] = ownSide->handMove[index].position[1];
        ownSide->disposal[slot].battle.position[2] = ownSide->handMove[index].position[2];
        ownSide->disposal[slot].battle.step[0] =
            (DisposePos1P[slot][0] - ownSide->handMove[index].position[0]) / 10.0f;
        ownSide->disposal[slot].battle.step[1] =
            (DisposePos1P[slot][1] - ownSide->handMove[index].position[1]) / 10.0f;
        ownSide->disposal[slot].battle.step[2] =
            (DisposePos1P[slot][2] - ownSide->handMove[index].position[2]) / 10.0f;
        ownSide->disposal[slot].battle.moveFrames = 10;
        ownSide->hand.cards[index] = -1;
        break;
    case CARD_KIND_SITUATION:
        for (i = 0, slot = -1; i < CARD_OPERATION_SLOTS; i++) {
            if (ownSide->operations[i] < 0) {
                slot = i;
                break;
            }
        }
        if (slot < 0) {
            return;
        }
        ownSide->operationFlags[slot] |= 1;
        ownSide->operations[slot] = ownSide->hand.cards[index];
        ownSide->operationDuration[slot] =
            defs[ownSide->hand.cards[index]].operationDuration;
        cost = CGPChkCardPlayCost(workAddress, ownSide,
                                  ownSide->hand.cards[index]);
        ownSide->operationMove[slot].position[0] = ownSide->handMove[index].position[0];
        ownSide->operationMove[slot].position[1] = ownSide->handMove[index].position[1];
        ownSide->operationMove[slot].position[2] = ownSide->handMove[index].position[2];
        ownSide->operationMove[slot].step[0] =
            (OperationPos1P[slot][0] - ownSide->handMove[index].position[0]) / 10.0f;
        ownSide->operationMove[slot].step[1] =
            (OperationPos1P[slot][1] - ownSide->handMove[index].position[1]) / 10.0f;
        ownSide->operationMove[slot].step[2] =
            (OperationPos1P[slot][2] - ownSide->handMove[index].position[2]) / 10.0f;
        ownSide->operationMove[slot].moveTimer = 10;
        ownSide->hand.cards[index] = -1;
        break;
    }

    for (i = 0; i < cost; i++) {
        CardPlayCostCard(ownSide);
    }
    CardPlayCleanHand(&ownSide->hand);
}



/*
 * Why one side may not play the card at the given hand position, or zero when
 * it may. Event cards are never playable (11) and battle and situation cards
 * need enough play points (12); a Gnosis card or weapon also has to pass the
 * equipment checks, where a Gnosis card has no free slot (15), a Human weapon
 * needs a light slot (13) and every other such card a heavy one (14), and 16
 * means the card is equipment the slot search accepted.
 */
s32 CGPDispoSub0(s32 side, CardGameWork *work, s32 index)
{
    s32 error;
    CardPlaySide *ownSide;
    ownSide = &work->player;
    if (side != 0) {
        ownSide = &work->enemy;
    }
    error = 0;
    if (work->definitions[ownSide->hand.cards[index]].kind == CARD_KIND_EVENT) {
        error = 11;
    } else {
        if (work->definitions[ownSide->hand.cards[index]].kind == CARD_KIND_BATTLE) {
            if (ownSide->hand.playPoints[0] < work->definitions[ownSide->hand.cards[index]].playCost) {
                error = 12;
            }
        } else if (work->definitions[ownSide->hand.cards[index]].kind == CARD_KIND_SITUATION) {
            if (ownSide->hand.playPoints[1] < work->definitions[ownSide->hand.cards[index]].playCost) {
                error = 12;
            }
        }
        if (error == 0) {
            if (work->definitions[ownSide->hand.cards[index]].flags & (CARD_ATTR_GNOSIS | CARD_ATTR_WEAPON)) {
                error = CardPlayChkCost(work->definitions, ownSide, ownSide->hand.cards[index], work);
                if (error == 0) {
                    error = CardPlayChkCondition(work->definitions, ownSide,
                                                 ownSide->hand.cards[index], work);
                    if (error == 0) {
                        error = 16;
                        if (work->definitions[ownSide->hand.cards[index]].flags & CARD_ATTR_GNOSIS) {
                            error = 15;
                        } else if (work->definitions[ownSide->hand.cards[index]].flags & CARD_ATTR_WEAPON_HUMAN) {
                            if (CardPlayCntLight(work, ownSide) != 0) {
                                error = 13;
                            }
                        } else if (CardEquipCntHeavy(work, ownSide, ownSide->hand.cards[index]) != 0) {
                            error = 14;
                        }
                    }
                }
            } else {
                error = CardPlayChkCost(work->definitions, ownSide, ownSide->hand.cards[index], work);
                if (error == 0) {
                    error = CardPlayChkCondition(work->definitions, ownSide,
                                                 ownSide->hand.cards[index], work);
                }
            }
        }
    }
    return error;
}

/*
 * Plays the hand card at the given position for the side whose controller just
 * pressed the confirm button, and returns the result code the caller acts on:
 * zero (played, or nothing to do), 13..15 (CGPDispoSub0's codes for a card that
 * still needs a board slot, answered with the decide sound) or 15 as well when
 * the card paired with the played one (the id 7 below it) stands on the battle
 * row. The Lv 10 cards are refused when the play points fall short (12)
 * or when the card, or the card paired with it, is already in the disposal row
 * (5 and 9); a refusal shows its error message with the buzzer and returns zero.
 */
s32 CGPDisposeSub(s32 side, CardGameWork *work, s32 index)
{
    CardPlaySide *ownSide;
    u16 pressed;
    s32 result;
    s32 pairedOnBattle;
    s32 i;

    result = 0;
    ownSide = &work->player;
    pressed = work->command.input[0].pressed;
    if (side != 0) {
        pressed = work->command.input[1].pressed;
        ownSide = &work->enemy;
    }
    if (index < CardPlayHandCnt(&ownSide->hand) && (pressed & PAD_CIRCLE)) {
        pairedOnBattle = 0;
        switch (ownSide->hand.cards[index]) {
        case CARD_ID_LV_10_MOMO:
        case CARD_ID_LV_10_KOS_MOS:
        case CARD_ID_LV_10_JR:
        case CARD_ID_LV_10_CHAOS:
            result = CGPDispoSub0(side, work, index);
            if (result == 5 || result == 1) {
                result = 0;
            }
            /* fall through */
        case CARD_ID_LV_10_SHION:
        case CARD_ID_LV_10_ZIGGY:
            if (result == 0) {
                if (ownSide->hand.playPoints[0] <
                    work->definitions[ownSide->hand.cards[index]].playCost) {
                    result = CGP_ERR_NOT_ENOUGH_POINTS;
                } else {
                    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                        s32 slotCard = ownSide->disposal[i].battle.layers[0].cardId;

                        if (ownSide->hand.cards[index] - CGP_PAIRED_CARD_OFFSET == slotCard) {
                            result = 9;
                            break;
                        }
                        if (ownSide->hand.cards[index] == ownSide->disposal[i].battle.layers[0].cardId) {
                            result = 5;
                            break;
                        }
                    }
                    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                        if (ownSide->hand.cards[index] - CGP_PAIRED_CARD_OFFSET ==
                            ownSide->battle[i].battle.layers[0].cardId) {
                            pairedOnBattle = 1;
                            result = 15;
                            break;
                        }
                    }
                }
            }
        }
        if (!pairedOnBattle) {
            if (result == 0) {
                result = CGPDispoSub0(side, work, index);
            }
            switch (result) {
            case 0:
                if (work->definitions[ownSide->hand.cards[index]].kind == CARD_KIND_BATTLE) {
                    ownSide->hand.playPoints[0] -=
                        work->definitions[ownSide->hand.cards[index]].playCost;
                } else {
                    ownSide->hand.playPoints[1] -=
                        work->definitions[ownSide->hand.cards[index]].playCost;
                }
                CardPlayDisployment(side, work, ownSide, index);
                /* fall through */
            case 13:
            case 14:
            case 15:
                xglSoundEffectNormalID(SE_DECIDE, 0);
                return result;
            default:
                if (result == CGP_ERR_NOT_ENOUGH_POINTS) {
                    if (work->definitions[ownSide->hand.cards[index]].kind == CARD_KIND_BATTLE) {
                        CGPSetErrorMess(side, work, CGP_ERR_NOT_ENOUGH_POINTS);
                    } else {
                        CGPSetErrorMess(side, work, 10);
                    }
                } else {
                    CGPSetErrorMess(side, work, result);
                }
                xglSoundEffectNormalID(SE_BUZZER, 0);
                result = 0;
                break;
            }
        }
    }
    return result;
}

/*
 * Moves the cursor (CGPCursorMove2) and resolves the card at its new
 * position (CGPCursor2Pos), then clears the resolved card id to -1 unless
 * the zone is 2 or 4 (nonzero flag) or 5 or 6 (zero flag). `unused` is never read.
 */
s16 CGPMoveSub(s32 flag, void *unused, CardGameWork *work)
{
    CardCursorPosition position;

    position.index = -1;
    CGPCursorMove2(flag, work);
    CGPCursor2Pos(&position, work);
    if (flag == 0) {
        if (position.zone != 5 && position.zone != 6) {
            position.cardId = -1;
        }
    } else {
        if (position.zone != 4 && position.zone != 2) {
            position.cardId = -1;
        }
    }
    return position.cardId;
}

/*
 * Raise or lower an attack's power before the damage is worked out. Operation
 * cards in play on either side act through the attacking card's flags, and
 * three attacking cards act on their own: Hammer counts the Mech cards in play on
 * its own board, Albedo looks for Kirschwasser there and Gargoyle plays its
 * trigger command. A power that fell below zero counts as none.
 */
s32 CGPBattleBeforeEffect(s32 side, CardGameWork *work, s32 power, CardLayerStack *stack,
                          s32 slot)
{
    CardPlaySide *own;
    CardPlaySide *foe;
    s32 cardId;
    s32 i;

    cardId = stack->layers[0].cardId;
    if (side == 0) {
        own = &work->player;
        foe = &work->enemy;
    } else {
        own = &work->enemy;
        foe = &work->player;
    }

    if (CardPlayCntOpe(own, CARD_ID_HILBERT_EFFECT) != 0 || CardPlayCntOpe(foe, CARD_ID_HILBERT_EFFECT) != 0) {
        if (work->definitions[cardId].flags & CARD_ATTR_GNOSIS) {
            power--;
        }
    }

    i = CardPlayCntOpe(own, CARD_ID_STIM);
    if (i != 0) {
        if (work->definitions[cardId].flags & CARD_ATTR_HUMAN) {
            while (i > 0) {
                i--;
                power++;
            }
        }
    }

    i = CardPlayCntOpe(own, CARD_ID_MYSTERIOUS_GIRL) + CardPlayCntOpe(foe, CARD_ID_MYSTERIOUS_GIRL);
    if (i != 0) {
        if (work->definitions[cardId].flags & CARD_ATTR_GNOSIS) {
            while (i > 0) {
                i--;
                power++;
            }
        }
    }

    i = CardPlayCntOpe(own, CARD_ID_ZOHAR) + CardPlayCntOpe(foe, CARD_ID_ZOHAR);
    if (i != 0) {
        while (i > 0) {
            i--;
            power++;
        }
    }

    switch (cardId) {
    case CARD_ID_HAMMER:
        if (stack->layers[1].cardId != CARD_ID_EYEGLASSES && stack->layers[1].cardId != CARD_ID_MISSILE_POD) {
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (own->battle[i].battle.layers[0].cardId >= 0) {
                    if (work->definitions[own->battle[i].battle.layers[0].cardId].flags & CARD_ATTR_MECH) {
                        power++;
                    }
                }
            }
        }
        break;
    case CARD_ID_ALBEDO:
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (own->battle[i].battle.layers[0].cardId == CARD_ID_KIRSCHWASSER) {
                power++;
                break;
            }
        }
        break;
    case CARD_ID_GARGOYLE:
        CardPlayCommandPlay(side, CARD_COMMAND_GARGOYLE_TRIGGER, work, slot, 0);
        break;
    }

    return (power < 0) ? 0 : power;
}



/*
 * Extra battle effects the attacking card scores, added to the effect count
 * CGPBattleCalcuration keeps. An attacker whose stack has the no-damage flag scores
 * nothing at all. The MOMO cards score one for Kirschwasser standing anywhere on
 * their own battle row; they and the 100 Series score one more when the defending
 * card is a Gnosis, and any attacker stacking a Saber scores three for the
 * same reason.
 */
s32 CGPBattleAttackEffect(s32 side, CardGameWork *work, s32 effect,
                          CardLayerStack *attacker, CardLayerStack *defender)
{
    CardPlaySide *own;
    s16 attackCard;
    s16 i;

    own = &work->player;
    if (side != 0) {
        own = &work->enemy;
    }
    attackCard = attacker->layers[0].cardId;
    if (attacker->flags & CARD_STACK_FLAG_NO_DAMAGE) {
        effect = 0;
    } else if (defender != 0) {
        switch (attackCard) {
        case CARD_ID_LV_1_MOMO:
        case CARD_ID_SWIM_SUIT_MOMO:
        case CARD_ID_LV_10_MOMO:
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (own->battle[i].battle.layers[0].cardId == CARD_ID_KIRSCHWASSER) {
                    effect++;
                    break;
                }
            }
            if (defender->layers[0].cardId >= 0 &&
                (work->definitions[defender->layers[0].cardId].flags & CARD_ATTR_GNOSIS)) {
                effect++;
            }
            break;
        case CARD_ID_100_SERIES:
            if (defender->layers[0].cardId >= 0 &&
                (work->definitions[defender->layers[0].cardId].flags & CARD_ATTR_GNOSIS)) {
                effect++;
            }
            break;
        }
        if (attacker->layers[1].cardId == CARD_ID_SABER &&
            defender->layers[0].cardId >= 0 &&
            (work->definitions[defender->layers[0].cardId].flags & CARD_ATTR_GNOSIS)) {
            effect += 3;
        }
    }
    return effect;
}

/*
 * Attack power left after the defender's protections: it drops by one for every
 * Tune Up in play on the defending side when the defender's base card is a
 * Mech, and by one for each defender layer that is a Shield or a Battle Suit,
 * or Lv 10 Shion while the attacker's base card is a Realian. Never below zero.
 */
s32 CGPBattleDiffenceEffect(s32 side, CardGameWork *work, s32 power, CardLayerStack *defender,
                            CardLayerStack *attacker)
{
    CardPlaySide *defenderSide;
    CardDefinition *defs;
    s32 cardId;
    s16 value;
    s16 i;

    defenderSide = (side == 0) ? &work->enemy : &work->player;
    value = CardPlayCntOpe(defenderSide, CARD_ID_TUNE_UP);
    cardId = defender->layers[0].cardId;
    defs = work->definitions;
    if (defs[cardId].flags & CARD_ATTR_MECH) {
        for (i = 0; i < value; i++) {
            power--;
        }
    }
    for (i = 0; i < 40; i++) {
        value = defender->layers[i].cardId;
        if (value >= 0) {
            switch (value) {
            case CARD_ID_LV_10_SHION:
                if (attacker->layers[0].cardId >= 0 &&
                    (defs[attacker->layers[0].cardId].flags & CARD_ATTR_REALIAN)) {
                    power--;
                }
                break;
            case CARD_ID_SHIELD:
            case CARD_ID_BATTLE_SUIT:
                power--;
                break;
            }
        }
    }
    return (power < 0) ? 0 : power;
}

/*
 * What a destroyed card leaves behind. A MOMO card survives with one life left
 * while the other side has Joachim Mizrahi in play; U-Tic Soldier, VX-9000, Wilhelm
 * and Cherenkov each play a command of their own, and Cherenkov queues the last
 * card of the other side's junk pile again when a copy of it is destroyed there.
 * Once the card is really gone, Human Sacrifice recovers as many cards as the card
 * reserved and Supply Order plays its trigger command for every side that has one
 * in play.
 */
s32 CGPBAEsub(s32 side, CardGameWork *work, s32 power, CardLayerStack *attacker,
              CardLayerStack *defender)
{
    CardPlaySide *own;
    CardPlaySide *foe;
    s16 *command;
    s16 foeSide;
    s16 ownSlot;
    s16 i;
    s32 card;
    s32 cardId;
    s32 found;
    s32 junkIndex;

    cardId = defender->layers[0].cardId;
    if (side == 0) {
        foeSide = 1;
        own = &work->player;
        foe = &work->enemy;
    } else {
        foeSide = 0;
        own = &work->enemy;
        foe = &work->player;
    }
    if (defender->layers[0].life <= 0) {
        defender->layers[0].life = 0;

        ownSlot = -1;
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (attacker == &own->battle[i].battle) {
                ownSlot = i;
                break;
            }
        }
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (defender == &foe->battle[i].battle) {
                break;
            }
        }

        switch (cardId) {
        case CARD_ID_LV_1_MOMO:
        case CARD_ID_SWIM_SUIT_MOMO:
        case CARD_ID_LV_10_MOMO:
            if (CardPlayCntBtl(foe, CARD_ID_JOACHIM_MIZRAHI) != 0) {
                defender->layers[0].life = 1;
                CardPlayCommandPlay(side, CARD_COMMAND_JOACHIM_MIZRAHI_TRIGGER, work, foeSide, 0);
            }
            break;
        case CARD_ID_U_TIC_SOLDIER:
            CardPlayCommandPlay(side, CARD_COMMAND_U_TIC_SOLDIER_TRIGGER, work, foeSide, 0);
            break;
        case CARD_ID_VX_9000:
            CardPlayCommandPlay(side, CARD_COMMAND_VX_9000_BATTLE_TRIGGER, work, 0, 0);
            break;
        case CARD_ID_WILHELM:
            CardPlayCommandPlay(side, CARD_COMMAND_WILHELM_TRIGGER, work, ownSlot, 0);
            break;
        case CARD_ID_CHERENKOV:
            found = 0;
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (foe->battle[i].battle.layers[0].cardId == CARD_ID_CHERENKOV &&
                    foe->battle[i].battle.layers[0].life <= 0) {
                    found = 1;
                    break;
                }
            }
            if (found != 0) {
                junkIndex = CardPlayJunkCnt(foe) - 1;
                if (junkIndex >= 0) {
                    card = foe->junk[junkIndex];
                    if (work->definitions[card].kind == CARD_KIND_BATTLE) {
                        if ((work->definitions[card].flags & (CARD_ATTR_HUMAN | CARD_ATTR_REALIAN | CARD_ATTR_MECH)) != 0) {
                            if (CardPlayIdentityCheck(work, foe,
                                                      work->definitions[card].identity) == 0) {
                                command = CPCSearchCommWk(side, work);
                                if (command != 0) {
                                    command[CARD_QUEUE_COMMAND] = CARD_COMMAND_CHERENKOV;
                                    command[CARD_QUEUE_PHASE] = CARD_PHASE_ADJUST;
                                    if (side != 0) {
                                        command[CARD_QUEUE_PHASE] = CARD_PHASE_ADJUST + CARD_PHASES_PER_SIDE;
                                    }
                                    command[CARD_QUEUE_OPERAND] = i;
                                    command[CARD_QUEUE_TARGET] = junkIndex;
                                }
                            }
                        }
                    }
                }
            }
            break;
        }

        if (defender->layers[0].life <= 0) {
            if (CardPlayCntOpe(foe, CARD_ID_HUMAN_SACRIFICE) != 0) {
                for (i = 0; i < work->definitions[defender->layers[0].cardId].reserveCost;
                     i++) {
                    CardPlayRecavery(foe);
                }
            }
            if (work->definitions[defender->layers[0].cardId].flags & CARD_ATTR_REALIAN) {
                if (CardPlayCntOpe(own, CARD_ID_SUPPLY_ORDER) != 0) {
                    CardPlayCommandPlay(side, CARD_COMMAND_SUPPLY_ORDER_TRIGGER, work, side, 0);
                }
                if (CardPlayCntOpe(foe, CARD_ID_SUPPLY_ORDER) != 0) {
                    CardPlayCommandPlay(side, CARD_COMMAND_SUPPLY_ORDER_TRIGGER, work, foeSide, 0);
                }
            }
        }
    }
    return 0;
}



/*
 * Effects that resolve once a battle is over. While the battle produced an
 * effect, the defending Ziggy cards down the attacking stack, AG-04, AG-02 M,
 * Goblin and Cerberus down it as well, and Lv 1 Shion plays its trigger command
 * once the attacking stack has run out of life. Both the attacking and the supporting
 * stack then run their own after-effects.
 */
void CGPBattleAfterEffect(s32 side, CardGameWork *work, s32 effect, s32 operand,
                          CardLayerStack *defender, CardLayerStack *attacker,
                          CardLayerStack *support)
{
    s16 card;

    card = defender->layers[0].cardId;
    if (card == CARD_ID_LV_1_ZIGGY || card == CARD_ID_LV_10_ZIGGY) {
        if (effect != 0 && attacker != 0) {
            attacker->flags |= CARD_STACK_FLAG_FROZEN;
        }
    }
    if (effect != 0) {
        switch (card) {
        case CARD_ID_LV_1_SHION:
            if (attacker->layers[0].life <= 0) {
                CardPlayCommandPlay(side, CARD_COMMAND_LV_1_SHION_TRIGGER, work, operand, 0);
            }
            break;
        case CARD_ID_AG_04:
        case CARD_ID_AG_02_M:
            attacker->flags |= CARD_STACK_FLAG_FROZEN;
            break;
        case CARD_ID_GOBLIN:
        case CARD_ID_CERBERUS:
            attacker->flags |= CARD_STACK_FLAG_FROZEN;
            break;
        }
    }
    if (attacker != 0) {
        CGPBAEsub(side, work, effect, defender, attacker);
    }
    if (support != 0) {
        CGPBAEsub(side, work, effect, defender, support);
    }
}

/*
 * Damage one attack deals. The attacker's effects raise the power, the
 * defender's lower it, and what is left is taken off the defending card. A card
 * stacked on the attacker that pierces (Proto Dora, AG-05, VX-7000 or Cannon) passes
 * the damage beyond the defending card's life to the card behind it. *destroyed is set
 * to 1 when the defending card is destroyed and to 2 when the card behind is too.
 * The damage still left over is returned.
 */
s32 CGPBattleCalcuration(s32 side, CardGameWork *work, s32 power, s32 *battleDone,
                         CardLayerStack *attacker, CardLayerStack *defender,
                         CardLayerStack *behind, s32 *destroyed)
{
    s32 damage;
    s32 taken;
    s32 pierces;
    s32 i;

    power = CGPBattleAttackEffect(side, work, power, attacker, defender);
    damage = CGPBattleDiffenceEffect(side, work, power, defender, attacker);
    *battleDone = 1;
    pierces = 0;
    if (damage != 0) {
        for (i = 39; i >= 0; i--) {
            if (attacker->layers[i].cardId >= 0) {
                if (attacker->layers[i].cardId == CARD_ID_PROTO_DORA || attacker->layers[i].cardId == CARD_ID_AG_05 ||
                    attacker->layers[i].cardId == CARD_ID_VX_7000 || attacker->layers[i].cardId == CARD_ID_CANNON) {
                    pierces = 1;
                    break;
                }
            }
        }
        if (pierces != 0 && defender->layers[0].life < damage) {
            *destroyed = 1;
            taken = defender->layers[0].life;
            defender->layers[0].life = 0;
            CardSetEffect(side, work, CGP_EFFECT_DAMAGE,taken, 0, defender->position, defender);
            damage -= taken;
            if (behind != 0) {
                if (behind->layers[0].life < damage) {
                    taken = behind->layers[0].life;
                    behind->layers[0].life = 0;
                    damage -= taken;
                    *destroyed = 2;
                } else {
                    taken = damage;
                    damage = 0;
                    behind->layers[0].life -= taken;
                }
                CardSetEffect(side, work, CGP_EFFECT_PIERCE_ATTACK_ONE_DESTROYED, 0, 0, attacker->position, behind);
                CardSetEffect(side, work, CGP_EFFECT_DAMAGE_ONE_DESTROYED,taken, 0, behind->position, behind);
            }
        } else {
            CardSetEffect(side, work, CGP_EFFECT_DAMAGE,damage, 0, defender->position, defender);
            defender->layers[0].life -= damage;
            damage = 0;
        }
    }
    if (defender != 0) {
        if (defender->layers[0].life < 0) {
            defender->layers[0].life = 0;
        }
    }
    if (behind != 0) {
        if (behind->layers[0].life < 0) {
            behind->layers[0].life = 0;
        }
    }
    return damage;
}

/*
 * Base attack power of the topmost card on a stack, scanning down from the
 * last layer for the highest occupied one; Shield, Battle Suit and Eyeglasses
 * only modify a card, so the base layer (index 0) is used instead when the
 * topmost occupied layer holds one of them.
 */
s32 CGPCalcPower(CardGameWork *work, CardLayerStack *stack)
{
    s32 i;
    s32 cardId;

    for (i = 39; i >= 0; i--) {
        if (stack->layers[i].cardId >= 0) {
            break;
        }
    }
    cardId = stack->layers[i].cardId;
    if (cardId == CARD_ID_SHIELD || cardId == CARD_ID_BATTLE_SUIT || cardId == CARD_ID_EYEGLASSES) {
        i = 0;
    }
    return work->definitions[stack->layers[i].cardId].attackPower;
}

/*
 * Power a card in play attacks with: its base power plus the two bonuses it
 * carries, doubled once per boost. A card whose attack type is 4 spends its
 * boosts instead of doubling.
 */
s32 CGPCalcPowerPlus(CardGameWork *work, CardLayerStack *card, s32 power)
{
    s32 boosts;

    if (card->layers[0].cardId < 0) {
        power = 0;
    } else {
        power += card->layers[0].powerPlus;
        power += card->layers[0].flag;
        if (CardChkAttackType(work->definitions, card) == 4) {
            card->layers[0].boostCount = 0;
        }
        boosts = card->layers[0].boostCount;
        if (boosts > 0) {
            do {
                boosts--;
                power *= 2;
            } while (boosts != 0);
        }
    }
    return power;
}

/*
 * Power a card gains or loses from the situation cards in play. Hilbert Effect
 * on either side costs a Gnosis card one point; every copy of Stim on this side
 * adds one to a Human, every copy of Mysterious Girl on either side one to a
 * Gnosis, and every copy of Zohar one to any card. Albedo also gains a point while Kirschwasser stands on one of
 * this side's battle slots, and Hammer one for every battle slot holding a Mech.
 */
s32 CGPCalcPowerEnv(s32 side, CardGameWork *work, s32 power, s32 cardId)
{
    CardPlaySide *ownSide;
    CardPlaySide *foeSide;
    s32 i;

    if (side == 0) {
        ownSide = &work->player;
        foeSide = &work->enemy;
    } else {
        ownSide = &work->enemy;
        foeSide = &work->player;
    }
    if (CardPlayCntOpe(ownSide, CARD_ID_HILBERT_EFFECT) != 0 || CardPlayCntOpe(foeSide, CARD_ID_HILBERT_EFFECT) != 0) {
        if (work->definitions[cardId].flags & CARD_ATTR_GNOSIS) {
            power--;
        }
    }
    i = CardPlayCntOpe(ownSide, CARD_ID_STIM);
    if (i != 0) {
        if (work->definitions[cardId].flags & CARD_ATTR_HUMAN) {
            while (i > 0) {
                i--;
                power++;
            }
        }
    }
    i = CardPlayCntOpe(ownSide, CARD_ID_MYSTERIOUS_GIRL) + CardPlayCntOpe(foeSide, CARD_ID_MYSTERIOUS_GIRL);
    if (i != 0) {
        if (work->definitions[cardId].flags & CARD_ATTR_GNOSIS) {
            while (i > 0) {
                i--;
                power++;
            }
        }
    }
    i = CardPlayCntOpe(ownSide, CARD_ID_ZOHAR) + CardPlayCntOpe(foeSide, CARD_ID_ZOHAR);
    if (i != 0) {
        while (i > 0) {
            i--;
            power++;
        }
    }
    switch (cardId) {
    case CARD_ID_HAMMER:
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (ownSide->battle[i].battle.layers[0].cardId >= 0 &&
                (work->definitions[ownSide->battle[i].battle.layers[0].cardId].flags & CARD_ATTR_MECH)) {
                power++;
            }
        }
        break;
    case CARD_ID_ALBEDO:
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (ownSide->battle[i].battle.layers[0].cardId == CARD_ID_KIRSCHWASSER) {
                power++;
                break;
            }
        }
        break;
    }
    return power;
}






/*
 * Upkeep of one board slot: refreshes the side's deck and discard counts, counts
 * the damage timer down or copies the life for display, and keeps the power in
 * line with the base card's definition while nothing is stacked on it. A base
 * card without life left (once the damage timer ran out) sends every card of the
 * stack to the discard pile under Gnosis Phenomenon and to the junk pile otherwise;
 * a stacked card without life goes to the junk pile. Each moved card gets a move record that slides it to the pile.
 * `place` names the board area (CGP_AREA_*) and slot the stack stands on.
 */
void CGPHPCheckSub(CardDefinition *definitions, CardPlaySide *side,
                   CardLayerStack *stack, CGPBoardArea *place)
{
    s32 count;
    s32 i;

    side->hand.deckCount = CardPlayYamaCnt(side);
    side->hand.suteCount = CardPlaySuteCnt(side);
    stack->previousFlags = stack->flags;
    if (stack->layers[0].cardId < 0) {
        return;
    }
    if (stack->layers[0].damageTimer != 0) {
        stack->layers[0].damageTimer--;
    } else {
        stack->layers[0].shownLife = stack->layers[0].life;
    }
    if (stack->layers[1].cardId < 0) {
        /* powerLimit is compared as a signed byte. */
        if (stack->powerLimit != definitions[stack->layers[0].cardId].basePower) {
            stack->power = definitions[stack->layers[0].cardId].basePower;
            stack->powerLimit = definitions[stack->layers[0].cardId].basePower;
        }
    }
    if (stack->layers[0].damageTimer == 0 && stack->layers[0].life <= 0) {
        if (CardPlayCntOpe(side, CARD_ID_GNOSIS_PHENOMENON) &&
            (definitions[stack->layers[0].cardId].flags & CARD_ATTR_GNOSIS)) {
            count = CardPlaySuteCnt(side);
            for (i = 0; i < 40; i++) {
                if (stack->layers[i].cardId >= 0) {
                    side->suteMove[count].moveTimer = CGP_PILE_MOVE_FRAMES;
                    side->suteMove[count].kind = 0;
                    side->suteMove[count].position[0] = stack->position[0];
                    side->sute[count] = stack->layers[i].cardId;
                    side->suteMove[count].position[1] = stack->position[1];
                    side->suteMove[count].position[2] = CGP_PILE_MOVE_Z;
                    stack->layers[i].cardId = -1;
                    side->suteMove[count].step[0] =
                        (CGP_SUTE_PILE_X - stack->position[0]) / CGP_PILE_MOVE_FRAMES;
                    side->suteMove[count].step[1] =
                        (CGP_SUTE_PILE_Y - stack->position[1]) / CGP_PILE_MOVE_FRAMES;
                    side->suteMove[count].step[2] = 0.0f;
                    count++;
                }
            }
        } else {
            count = CardPlayJunkCnt(side);
            for (i = 0; i < 40; i++) {
                if (stack->layers[i].cardId >= 0) {
                    if (place->area == CGP_AREA_ENEMY_DISPOSAL ||
                        place->area == CGP_AREA_PLAYER_DISPOSAL) {
                        side->junkMove[count].position[0] =
                            side->disposal[place->slot].battle.position[0];
                        side->junkMove[count].position[1] =
                            side->disposal[place->slot].battle.position[1];
                        side->junkMove[count].position[2] = CGP_PILE_MOVE_Z;
                    } else {
                        side->junkMove[count].position[0] =
                            side->battle[place->slot].battle.position[0];
                        side->junkMove[count].position[1] =
                            side->battle[place->slot].battle.position[1];
                        side->junkMove[count].position[2] = CGP_PILE_MOVE_Z;
                    }
                    side->junkMove[count].step[0] =
                        (JunkPos1P[1][0] - side->junkMove[count].position[0]) / CGP_PILE_MOVE_FRAMES;
                    side->junkMove[count].step[1] =
                        (JunkPos1P[1][1] - side->junkMove[count].position[1]) / CGP_PILE_MOVE_FRAMES;
                    side->junkMove[count].step[2] = 0.0f;
                    side->junkMove[count].moveTimer = CGP_PILE_MOVE_FRAMES;
                    side->junk[count] = stack->layers[i].cardId;
                    stack->layers[i].cardId = -1;
                    count++;
                }
            }
        }
    }
    if (stack->layers[1].cardId >= 0 && stack->layers[1].life <= 0) {
        stack->power = definitions[stack->layers[0].cardId].basePower;
        stack->powerLimit = definitions[stack->layers[0].cardId].basePower;
        count = CardPlayJunkCnt(side);
        if (place->area == CGP_AREA_ENEMY_DISPOSAL ||
            place->area == CGP_AREA_PLAYER_DISPOSAL) {
            side->junkMove[count].position[0] = side->disposal[place->slot].battle.position[0];
            side->junkMove[count].position[1] = side->disposal[place->slot].battle.position[1];
            side->junkMove[count].position[2] = CGP_PILE_MOVE_Z;
        } else {
            side->junkMove[count].position[0] = side->battle[place->slot].battle.position[0];
            side->junkMove[count].position[1] = side->battle[place->slot].battle.position[1];
            side->junkMove[count].position[2] = CGP_PILE_MOVE_Z;
        }
        side->junkMove[count].step[0] =
            (JunkPos1P[1][0] - side->junkMove[count].position[0]) / CGP_PILE_MOVE_FRAMES;
        side->junkMove[count].step[1] =
            (JunkPos1P[1][1] - side->junkMove[count].position[1]) / CGP_PILE_MOVE_FRAMES;
        side->junkMove[count].step[2] = 0.0f;
        side->junkMove[count].moveTimer = CGP_PILE_MOVE_FRAMES;
        side->junk[count] = stack->layers[1].cardId;
        stack->layers[1].cardId = -1;
    }
}

/*
 * Recheck the hit points of every card standing on either board: first the
 * four disposal slots of both sides, then their four battle slots.
 */
void CGPRefreshFieldHP(CardGameWork *work)
{
    CGPBoardArea playerPlace;
    CGPBoardArea enemyPlace;
    CardPlaySide *player;
    CardPlaySide *enemy;
    s32 i;

    player = &work->player;
    enemy = &work->enemy;
    playerPlace.area = 6;
    enemyPlace.area = 2;
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        playerPlace.slot = i;
        enemyPlace.slot = i;
        CGPHPCheckSub(work->definitions, player,
                      &player->disposal[i].battle, &playerPlace);
        CGPHPCheckSub(work->definitions, enemy,
                      &enemy->disposal[i].battle, &enemyPlace);
    }
    playerPlace.area = 5;
    enemyPlace.area = 4;
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        playerPlace.slot = i;
        enemyPlace.slot = i;
        CGPHPCheckSub(work->definitions, player,
                      &player->battle[i].battle, &playerPlace);
        CGPHPCheckSub(work->definitions, enemy,
                      &enemy->battle[i].battle, &enemyPlace);
    }
}






/*
 * One battle step of the card in battle slot `slot` of `side`. A queued battle
 * command runs first; otherwise the top card of the stack decides the attack
 * type and power. When the command matches that type the slot is highlighted,
 * its countdown runs, and once it reaches zero the card attacks: the opposing
 * card (and the one behind it when that slot is occupied), the `support` stack, or
 * the other side directly. The damage left over is shown and paid from the foe's
 * hand, one card per point of damage. Returns 1 when a queued command ran, otherwise 0.
 */
s32 CGPBattleSub(s32 side, u16 command, s32 slot, CardGameWork *work,
                 CardPlaySide *ownSide, CardPlaySide *foeSide, CardLayerStack *support)
{
    CardCursorPosition cursor;
    s32 battleDone;
    s32 destroyed;
    s16 *queue;
    s32 result = 0;
    s32 otherSide = (side == 0);
    s32 attackType;
    s32 power;
    s32 hasUpperLayer;
    s32 i;

    destroyed = 0;
    queue = &work->commandQueue[0][0][0];
    if (side != 0) {
        queue = &work->commandQueue[1][0][0];
    }
    if (CardPlayCommandCheck(work->command.phase, queue)) {
        CardPlayCommandBattleExecute(side, work->command.phase, work);
        result = 1;
        CGPRefreshFieldHP(work);
    } else if (ownSide->battle[slot].battle.layers[0].cardId >= 0) {
        for (i = CGP_STACK_TOP_LAYER; i >= 0; i--) {
            if (ownSide->battle[slot].battle.layers[i].cardId >= 0) {
                break;
            }
        }
        hasUpperLayer = (i != 0);
        if (ownSide->battle[slot].battle.layers[i].cardId == CARD_ID_SHIELD ||
            ownSide->battle[slot].battle.layers[i].cardId == CARD_ID_BATTLE_SUIT ||
            ownSide->battle[slot].battle.layers[i].cardId == CARD_ID_EYEGLASSES) {
            i = 0;
        }
        power = work->definitions[ownSide->battle[slot].battle.layers[i].cardId].attackPower;
        attackType = CardChkAttackType(work->definitions, &ownSide->battle[slot].battle);
        power = CGPCalcPowerPlus(work, &ownSide->battle[slot].battle, power);
        if (command == attackType) {
            cursor.zone = CGP_AREA_PLAYER_BATTLE;
            if (side != 0) {
                cursor.zone = CGP_AREA_ENEMY_BATTLE;
            }
            cursor.index = slot;
            CardCursorActive(&work->cursor, &cursor, 2);
            if (attackType != CGP_ATTACK_SUPPORT) {
                if (ownSide->battle[slot].battle.power > 0) {
                    if (!(ownSide->battle[slot].battle.flags & CARD_STACK_FLAG_FROZEN)) {
                        ownSide->battle[slot].battle.power = ownSide->battle[slot].battle.power - 1;
                    }
                }
            }
            if (ownSide->battle[slot].battle.power <= 0 && !(ownSide->battle[slot].battle.flags & CARD_STACK_FLAG_FROZEN)) {
                if (command != CGP_ATTACK_SUPPORT) {
                    ownSide->battle[slot].battle.power = ownSide->battle[slot].battle.powerLimit;
                }
                if (hasUpperLayer) {
                    if (work->definitions[ownSide->battle[slot].battle.layers[1].cardId].attackType != CGP_ATTACK_SUPPORT) {
                        if (ownSide->battle[slot].battle.layers[1].life < CGP_LIFE_WEAR_LIMIT) {
                            ownSide->battle[slot].battle.layers[1].life--;
                        }
                    }
                }
                power = CGPBattleBeforeEffect(side, work, power, &ownSide->battle[slot].battle, slot);
                if (attackType == CGP_ATTACK_DIRECT) {
                    if (CardPlayCntBtl(ownSide, CARD_ID_VX_10000) != 0 || CardPlayCntBtl(foeSide, CARD_ID_VX_10000) != 0) {
                        power = 0;
                    }
                }
                if (ownSide->battle[slot].battle.flags & CARD_STACK_FLAG_NO_DAMAGE) {
                    power = 0;
                }
                if (power != 0) {
                    CardLayerStack *target;
                    CardLayerStack *behind;

                    battleDone = 0;
                    target = 0;
                    behind = 0;
                    if (command == CGP_ATTACK_SUPPORT) {
                        if (support != 0 && support->layers[0].cardId >= 0) {
                            target = support;
                            if (CardPlayCntOpe(foeSide, CARD_ID_GOOD_LUCK)) {
                                if (work->definitions[target->layers[0].cardId].flags & CARD_ATTR_HUMAN) {
                                    power = 0;
                                }
                            }
                            if (power != 0) {
                                CGPBattleCalcuration(side, work, power, &battleDone, &ownSide->battle[slot].battle,
                                                     target, 0, &destroyed);
                                CardSetEffect(otherSide, work, CGP_EFFECT_BURST,0, CARD_EFFECT_SE_HIT, target->position, target);
                            }
                        }
                        power = 0;
                    } else {
                        if (foeSide->battle[1 - (slot & 1)].battle.layers[0].cardId >= 0) {
                            target = &foeSide->battle[1 - (slot & 1)].battle;
                            if (foeSide->battle[3 - (slot & 1)].battle.layers[0].cardId >= 0) {
                                behind = &foeSide->battle[3 - (slot & 1)].battle;
                            }
                        } else if (!(attackType & 1)) {
                            if (foeSide->battle[3 - (slot & 1)].battle.layers[0].cardId >= 0) {
                                target = &foeSide->battle[3 - (slot & 1)].battle;
                            }
                        }
                        if (CardPlayCntOpe(foeSide, CARD_ID_GOOD_LUCK) && target != 0) {
                            if (work->definitions[target->layers[0].cardId].flags & CARD_ATTR_HUMAN) {
                                if (behind != 0) {
                                    target = behind;
                                    behind = 0;
                                    if (work->definitions[target->layers[0].cardId].flags & CARD_ATTR_HUMAN) {
                                        target = 0;
                                    }
                                } else {
                                    target = 0;
                                }
                            }
                        }
                        if (slot < 2) {
                            switch (attackType) {
                            case CGP_ATTACK_FRONT:
                                if (target != 0) {
                                    CGPBattleCalcuration(side, work, power, &battleDone, &ownSide->battle[slot].battle,
                                                         target, 0, &destroyed);
                                    if (work->definitions[ownSide->battle[slot].battle.layers[0].cardId].flags & CARD_ATTR_MECH) {
                                        CardSetEffect(side, work, CGP_EFFECT_FRONT_ATTACK, 0,CGP_SE_FRONT_ATTACK_ALT, ownSide->battle[slot].battle.position, target);
                                    } else {
                                        CardSetEffect(side, work, CGP_EFFECT_FRONT_ATTACK, 0,CGP_SE_FRONT_ATTACK, ownSide->battle[slot].battle.position, target);
                                    }
                                }
                                power = 0;
                                break;
                            case CGP_ATTACK_PIERCE:
                                if (target != 0) {
                                    s32 sound;

                                    sound = CGP_SE_ATTACK;
                                    if (work->definitions[ownSide->battle[slot].battle.layers[0].cardId].flags & CARD_ATTR_MECH) {
                                        sound = CGP_SE_ATTACK_ALT;
                                    }
                                    power = CGPBattleCalcuration(side, work, power, &battleDone,
                                                                 &ownSide->battle[slot].battle, target, behind, &destroyed);
                                    CardSetEffect(side, work, CGP_EFFECT_PIERCE_ATTACK, 0, sound, ownSide->battle[slot].battle.position, target);
                                }
                                break;
                            }
                        } else {
                            switch (attackType) {
                            case CGP_ATTACK_FRONT:
                                power = 0;
                                break;
                            case CGP_ATTACK_PIERCE:
                                if (target != 0) {
                                    s32 sound;

                                    sound = CGP_SE_ATTACK;
                                    if (work->definitions[ownSide->battle[slot].battle.layers[0].cardId].flags & CARD_ATTR_MECH) {
                                        sound = CGP_SE_ATTACK_ALT;
                                    }
                                    power = CGPBattleCalcuration(side, work, power, &battleDone,
                                                                 &ownSide->battle[slot].battle, target, behind, &destroyed);
                                    CardSetEffect(side, work, CGP_EFFECT_PIERCE_ATTACK, 0, sound, ownSide->battle[slot].battle.position, target);
                                }
                                break;
                            }
                        }
                    }
                    CGPBattleAfterEffect(side, work, battleDone, slot, &ownSide->battle[slot].battle, target, behind);
                }
                if (power > CGP_POWER_MAX) {
                    power = CGP_POWER_MAX;
                }
                if (power != 0) {
                    s32 sound;

                    sound = CGP_SE_ATTACK;
                    if (work->definitions[ownSide->battle[slot].battle.layers[0].cardId].flags & CARD_ATTR_MECH) {
                        sound = CGP_SE_ATTACK_ALT;
                    }
                    if (attackType == CGP_ATTACK_PIERCE) {
                        if (destroyed == 0) {
                            CardSetEffect(side, work, CGP_EFFECT_PIERCE_ATTACK, 0, sound, ownSide->battle[slot].battle.position, 0);
                        } else if (destroyed == 1) {
                            CardSetEffect(side, work, CGP_EFFECT_PIERCE_ATTACK_ONE_DESTROYED, 0, sound, ownSide->battle[slot].battle.position, 0);
                        } else {
                            CardSetEffect(side, work, CGP_EFFECT_PIERCE_ATTACK_MANY_DESTROYED, 0, sound, ownSide->battle[slot].battle.position, 0);
                        }
                    } else if (attackType == CGP_ATTACK_DIRECT) {
                        xglSoundEffectParamDirect(CGP_SE_DIRECT_ATTACK, CGP_SE_DIRECT_VOLUME, CGP_SE_DIRECT_PAN);
                        CardSetEffect(side, work, CGP_EFFECT_DIRECT_ATTACK, 0, 0, ownSide->battle[slot].battle.position, 0);
                    }
                    if (!(foeSide->hand.commandFlags & CGP_HAND_FLAG_NO_DAMAGE)) {
                        i = CardPlayCntOpe(ownSide, CARD_ID_TERRITORY_SHIFT) + CardPlayCntOpe(foeSide, CARD_ID_TERRITORY_SHIFT);
                        if (i != 0) {
                            if (work->definitions[ownSide->battle[slot].battle.layers[0].cardId].flags & CARD_ATTR_GNOSIS) {
                                if (ownSide->hand.playPoints[2] > 0) {
                                    ownSide->hand.playPoints[2]--;
                                    CardPlayCommandPlay(side, CARD_COMMAND_TERRITORY_SHIFT_TRIGGER, work, 0, 0);
                                }
                            }
                        }
                        if (destroyed == 0) {
                            CardSetEffect(side, work, CGP_EFFECT_DAMAGE, power, CARD_EFFECT_SE_HIT, 0, 0);
                        } else if (destroyed == 1) {
                            CardSetEffect(side, work, CGP_EFFECT_DAMAGE_ONE_DESTROYED, power, CARD_EFFECT_SE_HIT, 0, 0);
                        } else {
                            CardSetEffect(side, work, CGP_EFFECT_DAMAGE_MANY_DESTROYED, power, CARD_EFFECT_SE_HIT, 0, 0);
                        }
                        for (i = 0; i < power; i++) {
                            CardPlayCostCard(foeSide);
                        }
                    } else {
                        CardPlayCommandPlay(side, CARD_COMMAND_IMPREGNABLE_GUARD_TRIGGER, work, 0, 0);
                    }
                }
            }
        }
    }
    return result;
}

/* Starts a stage rotation to stage when the save enables it, in the 2P battle phase, once the
 * cooldown has run out. */
void CGPRotStageSub(CardGameWork *work, u32 stage)
{
    if ((work->save->flags & CARD_SAVE_ROTATE_STAGE) && work->fase == CARD_FASE_BATTLE_2P &&
        work->rotStageId != stage && work->rotStageCooldown == 0)
    {
        work->rotStageId = stage;
        work->rotStageCooldown = CGP_ROT_STAGE_COOLDOWN;
    }
}









/* Frames left to wait after a draw. */
static u8 timer_1;
/* Exchanges of the opening hand still on offer. */
static u8 change_2;
/* Hand index the exchange prompt and the draw steps highlight. */
static s8 cardno_3;
/* Cards still to pay for in the effect steps, then cards still to draw. */
static s32 cnt_0;

/*
 * First phase of a turn for one side (called with the side, the work area and
 * both sides' card records): offers the opening hand for up to three exchanges
 * ("Is this hand okay?"; cross takes a new hand, circle keeps it), highlights
 * and pays for the board cards that act at the start of the turn, offers the
 * top card of the deck ("Add this card to your hand?"), then draws the cards
 * of the turn one per step and hands over to CGPNextTurnSub once they are all
 * drawn.
 */
void CGPDrawFaseProc(s32 side, CardGameWork *work, CardPlaySide *ownSide, CardPlaySide *foeSide)
{
    CardPlaySide *activeSide;      /* the record of `side`, the same address as ownSide */
    u16 held;
    u16 pressed;
    s32 autoPlay;
    s32 i;
    CardCursorPosition position;
    CardCursorPosition operationPosition;

    autoPlay = 0;
    if (side == 0) {
        activeSide = &work->player;
        held = work->command.input[0].held;
        pressed = work->command.input[0].pressed;
        if (work->flags & CGP_FLAG_SIDE0_AUTO) {
            autoPlay = 1;
        }
    } else {
        activeSide = &work->enemy;
        held = work->command.input[1].held;
        pressed = work->command.input[1].pressed;
        {
            s32 autoFlag = work->flags & CGP_FLAG_SIDE1_AUTO;

            autoPlay = autoFlag != 0;
        }
    }
    switch ((s16) work->command.phaseTimer) {
    case CGP_DRAW_STEP_BEGIN:
        cnt_0 = 0;
        change_2 = CGP_DRAW_EXCHANGES;
        cardno_3 = 0;
        work->command.interruptMessId = CGP_DRAW_INTERRUPT_MESS;
        work->command.interruptMessTimer = CGP_INTERRUPT_MESS_FRAMES;
        ownSide->hand.playPoints[0] = 1;
        ownSide->hand.playPoints[1] = 1;
        ownSide->hand.playPoints[2] = 1;
        ownSide->hand.commandFlags = 0;
        work->cursor.selectionSubMode = 0;
        work->cursor.selectionMode = 0;
        CGPRotStageSub(work, side);
        work->command.phaseTimer++;
        break;
    case CGP_DRAW_STEP_EXCHANGE_OFFER:
        if (work->command.turn != 0) {
            work->command.phaseTimer = CGP_DRAW_STEP_EFFECT_BEGIN;
        } else if (side == 0 ? (work->flags & CGP_FLAG_SIDE0_AUTO) : (work->flags & CGP_FLAG_SIDE1_AUTO)) {
            work->command.phaseTimer = CGP_DRAW_STEP_EFFECT_BEGIN;
        } else if (change_2 != 0) {
            work->command.phaseTimer++;
        } else {
            work->command.phaseTimer = CGP_DRAW_STEP_EFFECT_BEGIN;
        }
        break;
    case CGP_DRAW_STEP_EXCHANGE_PROMPT:
    {
        s32 count = CardPlayHandCnt(&activeSide->hand);
        s32 message = 0;

        switch (change_2) {
        case 1:
            message = CGP_ERR_EXCHANGE_ONE;
            break;
        case 2:
            message = CGP_ERR_EXCHANGE_TWO;
            break;
        case 3:
            message = CGP_ERR_EXCHANGE_THREE;
            break;
        }
        /* With the stage rotated the pad's left and right swap their meaning. */
        if ((work->rotStageId == 0 && (pressed & PAD_RIGHT)) ||
            (work->rotStageId != 0 && (pressed & PAD_LEFT))) {
            if (cardno_3 < count - 1) {
                xglSoundEffectNormalID(SE_CURSOR, 0);
                cardno_3++;
            }
        } else if ((work->rotStageId == 0 && (pressed & PAD_LEFT)) ||
                   (work->rotStageId != 0 && (pressed & PAD_RIGHT))) {
            if (cardno_3 > 0) {
                xglSoundEffectNormalID(SE_CURSOR, 0);
                cardno_3--;
            }
        }
        if (pressed & PAD_CROSS) {
            xglSoundEffectNormalID(SE_DECIDE, 0);
            work->command.phaseTimer++;
        } else if (pressed & PAD_CIRCLE) {
            xglSoundEffectNormalID(SE_CANCEL, 0);
            work->command.phaseTimer = CGP_DRAW_STEP_EFFECT_BEGIN;
        }
        if (held & PAD_TRIANGLE) {
            CGPDispErrorMessPlus(side, message, activeSide->hand.cards[cardno_3]);
        } else {
            CGPDispErrorMess(side, message);
        }
        if (CGP_HAND_FLIPPED(side, work)) {
            CGCPHandDispSub3(work->definitions, activeSide, cardno_3, 0, 0);
        } else {
            CGCPHandDispSub2(work->definitions, activeSide, cardno_3, 0);
        }
        break;
    }
    case CGP_DRAW_STEP_EXCHANGE:
    {
        s32 count = CardPlayHandCnt(&activeSide->hand);
        s32 deckCount = CardPlayYamaCnt(activeSide);

        /* The hand goes back under the deck, the deck is shuffled and a new hand is drawn. */
        for (i = 0; i < count; i++) {
            activeSide->deck[deckCount + i] = activeSide->hand.cards[i];
            activeSide->hand.cards[i] = -1;
        }
        CardPlayShuffleYama(activeSide);
        for (i = CGP_DRAW_HAND_SIZE - 1; i >= 0; i--) {
            CardPlayDrawCard(activeSide);
        }
        work->command.phaseTimer = CGP_DRAW_STEP_EXCHANGE_OFFER;
        change_2--;
        break;
    }
    case CGP_DRAW_STEP_EFFECT_BEGIN:
        work->command.phaseTimer++;
        /* fall through */
    case CGP_DRAW_STEP_EFFECT_COUNT:
    {
        s32 slot;

        /* Counts the Matthews cards on both sides: one point each in a disposal slot, two in a battle slot. */
        cnt_0 = 0;
        if (CardPlayCntDsp(ownSide, CARD_ID_MATTHEWS)) {
            cnt_0 += 1;
        }
        if (CardPlayCntBtl(ownSide, CARD_ID_MATTHEWS)) {
            cnt_0 += 2;
        }
        if (CardPlayCntDsp(foeSide, CARD_ID_MATTHEWS)) {
            cnt_0 += 1;
        }
        if (CardPlayCntBtl(foeSide, CARD_ID_MATTHEWS)) {
            cnt_0 += 2;
        }
        if (cnt_0 == 0) {
            work->command.phaseTimer = CGP_DRAW_STEP_OPE_CURSOR;
            break;
        }
        if (!autoPlay) {
            CGPSetErrorMessPlus(side, work, CGP_ERR_USE_CARD_EFFECT, CARD_ID_MATTHEWS);
        }
        for (slot = 0; slot < CARD_BOARD_SLOTS; slot++) {
            if (work->player.battle[slot].battle.layers[0].cardId == CARD_ID_MATTHEWS) {
                position.zone = CGP_AREA_PLAYER_BATTLE;
                position.index = slot;
                CardCursorActive(&work->cursor, &position, 2);
            }
            if (work->enemy.battle[slot].battle.layers[0].cardId == CARD_ID_MATTHEWS) {
                position.zone = CGP_AREA_ENEMY_BATTLE;
                position.index = slot;
                CardCursorActive(&work->cursor, &position, 2);
            }
        }
        for (slot = 0; slot < CARD_BOARD_SLOTS; slot++) {
            if (work->player.disposal[slot].battle.layers[0].cardId == CARD_ID_MATTHEWS) {
                position.zone = CGP_AREA_PLAYER_DISPOSAL;
                position.index = slot;
                CardCursorActive(&work->cursor, &position, 2);
            }
            if (work->enemy.disposal[slot].battle.layers[0].cardId == CARD_ID_MATTHEWS) {
                position.zone = CGP_AREA_ENEMY_DISPOSAL;
                position.index = slot;
                CardCursorActive(&work->cursor, &position, 2);
            }
        }
        work->command.phaseTimer++;
        break;
    }
    case CGP_DRAW_STEP_EFFECT_PAY:
        CardPlayCostCard(ownSide);
        cnt_0--;
        work->command.phaseTimer++;
        break;
    case CGP_DRAW_STEP_EFFECT_WAIT:
        if (cnt_0 <= 0) {
            work->command.phaseTimer = CGP_DRAW_STEP_OPE_CURSOR;
        } else {
            work->command.phaseTimer = (s16) work->command.phaseTimer - 1;
        }
        break;
    case CGP_DRAW_STEP_OPE_CURSOR:
        CardCursorPassive(&work->cursor);
        cnt_0 = 0;
        if (!autoPlay) {
            if (CardPlayCntOpe(ownSide, CARD_ID_COLLECTIVE) != 0) {
                for (i = 0; i < CARD_OPERATION_SLOTS; i++) {
                    if (ownSide->operations[i] == CARD_ID_COLLECTIVE) {
                        operationPosition.zone = CGP_AREA_PLAYER_OPERATION;
                        if (side != 0) {
                            operationPosition.zone = CGP_AREA_ENEMY_OPERATION;
                        }
                        operationPosition.index = i;
                        CardCursorActive(&work->cursor, &operationPosition, 2);
                    }
                }
                work->command.phaseTimer++;
            } else {
                work->command.phaseTimer = CGP_DRAW_STEP_BONUS_COUNT;
            }
        } else {
            work->command.phaseTimer = CGP_DRAW_STEP_BONUS_COUNT;
        }
        break;
    case CGP_DRAW_STEP_ADD_PROMPT:
        CGPDispErrorMess3(side, CGP_ERR_ADD_TO_HAND);
        if (pressed & PAD_CROSS) {
            s16 top;
            s32 deckCount;
            s32 j;

            /* Declining sends the top card of the deck to the bottom. */
            xglSoundEffectNormalID(SE_DECIDE, 0);
            deckCount = CardPlayYamaCnt(ownSide);
            top = ownSide->deck[deckCount - 1];
            for (j = deckCount - 1; j > 0; j--) {
                ownSide->deck[j] = ownSide->deck[j - 1];
            }
            ownSide->deck[0] = top;
            work->command.phaseTimer = CGP_DRAW_STEP_BONUS_COUNT;
        } else if (pressed & PAD_CIRCLE) {
            xglSoundEffectNormalID(SE_DECIDE, 0);
            work->command.phaseTimer = CGP_DRAW_STEP_BONUS_COUNT;
        }
        {
            s32 rotated = 1;

            if (!(held & PAD_TRIANGLE)) {
                if (CGP_HAND_FLIPPED(side, work)) {
                    CGCPHandDispSub3(work->definitions, activeSide, -1, 0, 0);
                } else {
                    rotated = 0;
                    CGCPHandDispSub2(work->definitions, activeSide, -1, 0);
                }
            }
            CGCPYamaDispSub(ownSide, 0, 1, held, rotated);
        }
        break;
    case CGP_DRAW_STEP_BONUS_COUNT:
        CardCursorPassive(&work->cursor);
        cnt_0 = CardPlayCntAllOpe(work, CARD_ID_WASTE_MONEY);
        if (cnt_0 != 0) {
            CGPSetErrorMessPlus(side, work, CGP_ERR_USE_CARD_EFFECT, CARD_ID_WASTE_MONEY);
        }
        if (CardPlayCntBtl(ownSide, CARD_ID_SANCTUARY_SHIP)) {
            cnt_0 += 1;
        }
        cnt_0 += 1;
        work->command.phaseTimer++;
        break;
    case CGP_DRAW_STEP_DRAW:
        if (!autoPlay) {
            if (CGP_HAND_FLIPPED(side, work)) {
                CGCPHandDispSub3(work->definitions, activeSide, cardno_3, 0, 0);
            } else {
                CGCPHandDispSub2(work->definitions, activeSide, cardno_3, 0);
            }
        }
        if (CardPlayCntOpe(ownSide, CARD_ID_IMAGINARY_SPACE) || CardPlayCntBtl(ownSide, CARD_ID_SANCTUARY_SHIP)) {
            CardPlayDrawSute(ownSide);
            timer_1 = CGP_DRAW_WAIT_FRAMES;
        } else {
            CardPlayDrawCard(ownSide);
            timer_1 = CGP_DRAW_WAIT_FRAMES;
        }
        work->command.phaseTimer++;
        cnt_0--;
        break;
    case CGP_DRAW_STEP_DRAW_WAIT:
        if (!autoPlay) {
            if (CGP_HAND_FLIPPED(side, work)) {
                CGCPHandDispSub3(work->definitions, activeSide, cardno_3, 0, 0);
            } else {
                CGCPHandDispSub2(work->definitions, activeSide, cardno_3, 0);
            }
        }
        if (timer_1 != 0) {
            timer_1--;
        } else if (cnt_0 == 0) {
            CGPNextTurnSub(work);
        } else {
            work->command.phaseTimer = (s16) work->command.phaseTimer - 1;
        }
        break;
    }
}


/* Card id of the last card CGPMoveSub hovered, or -1 when the cursor is on no card. */
static s16 no_4;
/* Board position the held card was picked up from. */
static CardCursorPosition mas_5;






/* Refuses the move with an error message and the buzzer; the caller returns. */
static inline void CGPMoveRefuse(s32 side, CardGameWork *work, s16 reason)
{
    CGPSetErrorMessPlus(side, work, CGP_ERR_MOVE_REFUSED, reason);
    xglSoundEffectNormalID(SE_BUZZER, 0);
}

/*
 * Move phase for one side: picks a card from the disposal or battle row, lists the slots it can
 * go to and moves it there. Lv 10 MOMO, Hilbert Effect and A Rat in a Trap refuse some moves.
 */
void CGPMoveFaseProc(s32 side, CardGameWork *work, CardPlaySide *ownSide)
{
    u16 pressed;
    s32 disposalArea;
    s32 battleArea;
    s32 handArea;
    s32 i;
    CardCursorPosition position;
    CardCursorPosition activePosition;
    CardCursorPosition target;

    if (side == 0) {
        pressed = work->command.input[0].pressed;
        disposalArea = CGP_AREA_PLAYER_DISPOSAL;
        battleArea = CGP_AREA_PLAYER_BATTLE;
        handArea = CGP_AREA_PLAYER_HAND;
    } else {
        pressed = work->command.input[1].pressed;
        disposalArea = CGP_AREA_ENEMY_DISPOSAL;
        battleArea = CGP_AREA_ENEMY_BATTLE;
        handArea = CGP_AREA_ENEMY_HAND;
    }
    CGPCursor2Pos(&position, work);
    if ((s16) work->command.phaseTimer != 0 && handArea == position.zone && work->cursorMoveAny == 0) {
        work->cursor.selectionMode = 0;
        if (CGP_HAND_FLIPPED(side, work)) {
            CGCPHandDispSub3(work->definitions, ownSide, position.index, 0, 0);
        } else {
            CGCPHandDispSub2(work->definitions, ownSide, position.index, 0);
        }
    } else {
        work->cursor.selectionMode = 1;
    }
    switch ((s16) work->command.phaseTimer) {
    case CGP_MOVE_STEP_PICK:
    {
        s32 disposeCount;
        s32 battleCount;

        no_4 = -1;
        disposeCount = CardPlayDisposeCnt(ownSide);
        battleCount = CardPlayBattleCnt(ownSide);
        if (disposeCount == 0 && battleCount == 0) {
            work->command.phaseTimer = CGP_MOVE_STEP_END;
            if ((side == 0 && (work->flags & CGP_FLAG_SIDE0_AUTO)) || (side != 0 && (work->flags & CGP_FLAG_SIDE1_AUTO))) {
                return;
            }
            if ((side == 0 && (work->save->flags & CGP_SAVE_FLAG_SIDE0_AUTO)) || (side != 0 && (work->save->flags & CGP_SAVE_FLAG_SIDE1_AUTO))) {
                return;
            }
            CGPSetErrorMess(side, work, CGP_ERR_NO_MOVABLE_CARD);
            return;
        }
        if (side == 0) {
            if (disposeCount != 0) {
                for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                    if (ownSide->disposal[i].battle.layers[0].cardId >= 0) {
                        work->cursorIndex = 0;
                        work->cursorZone = 5 + i;
                        break;
                    }
                }
            } else {
                for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                    if (ownSide->battle[i].battle.layers[0].cardId >= 0) {
                        work->cursorIndex = 1 + (i & 1);
                        work->cursorZone = 5 + (i & 2);
                        break;
                    }
                }
            }
        } else {
            if (disposeCount != 0) {
                for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                    if (ownSide->disposal[i].battle.layers[0].cardId >= 0) {
                        work->cursorIndex = 3;
                        work->cursorZone = 4 - i;
                        break;
                    }
                }
            } else {
                for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                    if (ownSide->battle[i].battle.layers[0].cardId >= 0) {
                        work->cursorIndex = 2 - (i & 1);
                        work->cursorZone = 4 - (i & 2);
                        break;
                    }
                }
            }
        }
        CGPRotStageSub(work, side);
        work->cursor.selectionSubMode = 1;
        work->cursor.selectionMode = 1;
        work->command.phaseTimer++;
        break;
    }
    case CGP_MOVE_STEP_LIST:
    {
        CardCursorControl *cursor = &work->cursor;

        CardCursorPassive(cursor);
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (ownSide->disposal[i].battle.layers[0].cardId >= 0) {
                mas_5.zone = disposalArea;
                mas_5.index = i;
                CardCursorActive(cursor, &mas_5, CGP_CURSOR_SELECTABLE);
            }
        }
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (ownSide->battle[i].battle.layers[0].cardId >= 0) {
                s32 mode;

                mas_5.zone = battleArea;
                mas_5.index = i;
                mode = CGP_CURSOR_SELECTABLE;
                if (work->definitions[ownSide->battle[i].battle.layers[0].cardId].flags & CARD_ATTR_GNOSIS) {
                    if (CardPlayCntOpe(&work->player, CARD_ID_HILBERT_EFFECT) + CardPlayCntOpe(&work->enemy, CARD_ID_HILBERT_EFFECT)) {
                        mode = CGP_CURSOR_BLOCKED;
                    }
                }
                CardCursorActive(cursor, &mas_5, mode);
            }
        }
        work->command.phaseTimer++;
        break;
    }
    case CGP_MOVE_STEP_SELECT:
    {
        s16 *hovered = &no_4;

        *hovered = CGPMoveSub(side, ownSide, work);
        if (!(pressed & PAD_CIRCLE)) {
            return;
        }
        if (work->cursorMoveAny != 0) {
            return;
        }
        if (*hovered >= 0) {
            CGPCursor2Pos(&mas_5, work);
            if (disposalArea == mas_5.zone) {
                xglSoundEffectNormalID(SE_DECIDE, 0);
                CardCursorPassive(&work->cursor);
                CardCursorActive(&work->cursor, &mas_5, CGP_CURSOR_PICKED);
                for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                    activePosition.zone = battleArea;
                    activePosition.index = i;
                    CardCursorActive(&work->cursor, &activePosition, CGP_CURSOR_TARGET);
                }
                work->command.phaseTimer++;
            } else if (battleArea == mas_5.zone) {
                if (work->definitions[*hovered].flags & CARD_ATTR_GNOSIS) {
                    if (CardPlayCntBtl(&work->player, CARD_ID_LV_10_MOMO) || CardPlayCntBtl(&work->enemy, CARD_ID_LV_10_MOMO)) {
                        CGPMoveRefuse(side, work, CARD_ID_LV_10_MOMO);
                        return;
                    }
                    if (work->definitions[*hovered].flags & CARD_ATTR_GNOSIS) {
                        if (CardPlayCntOpe(&work->player, CARD_ID_HILBERT_EFFECT) || CardPlayCntOpe(&work->enemy, CARD_ID_HILBERT_EFFECT)) {
                            CGPMoveRefuse(side, work, CARD_ID_HILBERT_EFFECT);
                            return;
                        }
                    }
                }
                xglSoundEffectNormalID(SE_DECIDE, 0);
                CardCursorPassive(&work->cursor);
                CardCursorActive(&work->cursor, &mas_5, CGP_CURSOR_PICKED);
                for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                    if (battleArea != mas_5.zone || i != mas_5.index) {
                        target.zone = battleArea;
                        target.index = i;
                        CardCursorActive(&work->cursor, &target, CGP_CURSOR_TARGET);
                    }
                }
                for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                    target.zone = disposalArea;
                    target.index = i;
                    CardCursorActive(&work->cursor, &target, CGP_CURSOR_TARGET);
                }
                work->command.phaseTimer++;
            }
        } else {
            CGPCursor2Pos(&target, work);
            if (handArea == target.zone) {
                CGPSetErrorMess(side, work, CGP_ERR_NO_CARD_THERE);
            }
            xglSoundEffectNormalID(SE_BUZZER, 0);
            CGPCursor2Pos(&mas_5, work);
        }
        break;
    }
    case CGP_MOVE_STEP_TARGET:
        CGPMoveSub(side, ownSide, work);
        if (pressed & PAD_CROSS) {
            work->command.phaseTimer = CGP_MOVE_STEP_LIST;
            xglSoundEffectNormalID(SE_CANCEL, 0);
            CardCursorPassive(&work->cursor);
        }
        if (!(pressed & PAD_CIRCLE)) {
            return;
        }
        CGPCursor2Pos(&target, work);
        if (battleArea == target.zone) {
            if (disposalArea == mas_5.zone) {
                if (target.cardId >= 0) {
                    if (CardPlayCntOpe(&work->player, CARD_ID_A_RAT_IN_A_TRAP) || CardPlayCntOpe(&work->enemy, CARD_ID_A_RAT_IN_A_TRAP)) {
                        CGPMoveRefuse(side, work, CARD_ID_A_RAT_IN_A_TRAP);
                        return;
                    }
                }
                if (work->definitions[target.cardId].flags & CARD_ATTR_GNOSIS) {
                    if (CardPlayCntBtl(&work->player, CARD_ID_LV_10_MOMO) || CardPlayCntBtl(&work->enemy, CARD_ID_LV_10_MOMO)) {
                        CGPMoveRefuse(side, work, CARD_ID_LV_10_MOMO);
                        return;
                    }
                    if (work->definitions[target.cardId].flags & CARD_ATTR_GNOSIS) {
                        if (CardPlayCntOpe(&work->player, CARD_ID_HILBERT_EFFECT) || CardPlayCntOpe(&work->enemy, CARD_ID_HILBERT_EFFECT)) {
                            CGPMoveRefuse(side, work, CARD_ID_HILBERT_EFFECT);
                            return;
                        }
                    }
                }
                CGPMoveDSBATTLEwork(&ownSide->battle[target.index].battle,
                                    &ownSide->disposal[mas_5.index].battle);
                work->command.phaseTimer++;
            } else {
                if (mas_5.index == target.index) {
                    return;
                }
                if (work->definitions[target.cardId].flags & CARD_ATTR_GNOSIS) {
                    if (CardPlayCntBtl(&work->player, CARD_ID_LV_10_MOMO) || CardPlayCntBtl(&work->enemy, CARD_ID_LV_10_MOMO)) {
                        CGPMoveRefuse(side, work, CARD_ID_LV_10_MOMO);
                        return;
                    }
                    if (work->definitions[target.cardId].flags & CARD_ATTR_GNOSIS) {
                        if (CardPlayCntOpe(&work->player, CARD_ID_HILBERT_EFFECT) || CardPlayCntOpe(&work->enemy, CARD_ID_HILBERT_EFFECT)) {
                            CGPMoveRefuse(side, work, CARD_ID_HILBERT_EFFECT);
                            return;
                        }
                    }
                }
                CGPMoveDSBATTLEwork(&ownSide->battle[target.index].battle,
                                    &ownSide->battle[mas_5.index].battle);
                work->command.phaseTimer++;
            }
        } else if (disposalArea == target.zone) {
            if (CardPlayCntOpe(&work->player, CARD_ID_A_RAT_IN_A_TRAP) || CardPlayCntOpe(&work->enemy, CARD_ID_A_RAT_IN_A_TRAP)) {
                CGPMoveRefuse(side, work, CARD_ID_A_RAT_IN_A_TRAP);
                return;
            }
            if (work->definitions[target.cardId].flags & CARD_ATTR_GNOSIS) {
                if (CardPlayCntBtl(&work->player, CARD_ID_LV_10_MOMO) || CardPlayCntBtl(&work->enemy, CARD_ID_LV_10_MOMO)) {
                    CGPMoveRefuse(side, work, CARD_ID_LV_10_MOMO);
                    return;
                }
                if (work->definitions[target.cardId].flags & CARD_ATTR_GNOSIS) {
                    if (CardPlayCntOpe(&work->player, CARD_ID_HILBERT_EFFECT) || CardPlayCntOpe(&work->enemy, CARD_ID_HILBERT_EFFECT)) {
                        CGPMoveRefuse(side, work, CARD_ID_HILBERT_EFFECT);
                        return;
                    }
                }
            }
            if (battleArea == mas_5.zone) {
                CGPMoveDSBATTLEwork(&ownSide->disposal[target.index].battle,
                                    &ownSide->battle[mas_5.index].battle);
                work->command.phaseTimer++;
            } else {
                xglSoundEffectNormalID(SE_BUZZER, 0);
            }
        } else {
            xglSoundEffectNormalID(SE_BUZZER, 0);
        }
        break;
    case CGP_MOVE_STEP_DONE:
        xglSoundEffectNormalID(SE_DECIDE, 0);
        CardCursorPassive(&work->cursor);
        work->command.phaseTimer = CGP_MOVE_STEP_LIST;
        break;
    case CGP_MOVE_STEP_END:
        work->command.phaseTimer++;
        break;
    case CGP_MOVE_STEP_NEXT_TURN:
        CGPNextTurnSub(work);
        break;
    }
}











/* The hand card the cursor picked (CGPCursor2Pos fills it in step 1); every later step works on it. */
static CardCursorPosition mas_6;
/* Cards still to be discarded. */
static s32 OfferingCnt_7;

/*
 * Set phase for one side, run once per frame. zoneA, zoneB and zoneC start as the side's hand,
 * disposal row and battle row; the discard steps reassign zoneA and zoneB.
 */
void CGPSetFaseProc(s32 side, CardGameWork *work, CardPlaySide *ownSide)
{
    u8 order[3];
    CardCursorPosition position;
    CardCursorPosition target;
    CardCursorPosition highlight;
    char digits[3];
    float matrix[4][4];
    float place[4];
    float scale[4];
    u16 pressed;
    s32 zoneA;
    s32 zoneB;
    s32 zoneC;
    s32 handCount;
    s32 result;

    pressed = work->command.input[0].pressed;
    zoneA = CGP_AREA_PLAYER_HAND;
    zoneB = CGP_AREA_PLAYER_DISPOSAL;
    zoneC = CGP_AREA_PLAYER_BATTLE;
    if (side != 0) {
        pressed = work->command.input[1].pressed;
        zoneA = CGP_AREA_ENEMY_HAND;
        zoneB = CGP_AREA_ENEMY_DISPOSAL;
        zoneC = CGP_AREA_ENEMY_BATTLE;
    }
    switch ((s16) work->command.phaseTimer) {
    case CGP_SET_STEP_SORT_HAND:
        handCount = CardPlayHandCnt(&ownSide->hand);
        if (side == 0) {
            work->cursorIndex = 0;
            work->cursorZone = CGP_AREA_PLAYER_NO_HAND;
            if (handCount != 0) {
                work->cursorZone = CGP_AREA_PLAYER_HAND;
            }
        } else {
            work->cursorIndex = 0;
            work->cursorZone = CGP_AREA_ENEMY_NO_HAND;
            if (handCount != 0) {
                work->cursorZone = CGP_AREA_ENEMY_HAND;
            }
        }
        CGPRotStageSub(work, side);
        work->cursor.selectionSubMode = 1;
        work->cursor.selectionMode = 1;
        work->command.phaseTimer++;
        /* Sort the hand by card kind: kinds 1, 4 and 2 first. */
        order[0] = 1;
        order[1] = 4;
        order[2] = 2;
        CardPlayTypeSort(work->definitions, ownSide->hand.cards, handCount, order);
        break;
    case CGP_SET_STEP_PICK_CARD:
    {
        s32 i;

        if (CardPlayBattleCnt(ownSide) != 1) {
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (ownSide->battle[i].battle.layers[0].cardId == CGP_CARD_FLAG_RESET) {
                    if (ownSide->battle[i].battle.layers[0].flag != 0) {
                        CGPSetErrorMessPlus(side, work, CGP_ERR_FLAG_RESET, CGP_CARD_FLAG_RESET);
                        ownSide->battle[i].battle.layers[0].flag = 0;
                    }
                }
            }
        }
        CardCursorPassive(&work->cursor);
        CGPCursorMove2(side, work);
        CGPCursor2Pos(&mas_6, work);
        work->cursor.selectionMode = 1;
        if (work->cursorMoveAny != 0) {
            return;
        }
        if (zoneA == mas_6.zone) {
            work->cursor.selectionMode = 0;
            result = CGPDisposeSub(side, work, mas_6.index);
            if (result != 0) {
                if (result == CGP_DISPOSE_NO_FREE_SLOT) {
                    /* The Lv 10 cards pair with the Lv 1 card seven ids below them (CGP_PAIRED_CARD_OFFSET). */
                    switch (mas_6.cardId) {
                    case CARD_ID_LV_10_SHION:
                    case CARD_ID_LV_10_ZIGGY:
                    case CARD_ID_LV_10_MOMO:
                    case CARD_ID_LV_10_KOS_MOS:
                    case CARD_ID_LV_10_JR:
                    case CARD_ID_LV_10_CHAOS:
                        work->command.phaseTimer = CGP_SET_STEP_PAIRED_SLOT;
                        break;
                    default:
                        {
                            s32 j;

                            /* One card to discard for every requirement byte of the picked card that is set. */
                            OfferingCnt_7 = 0;
                            for (j = CGP_REQUIREMENT_COUNT - 1; j >= 0; j--) {
                                if (work->definitions[mas_6.cardId].discardRequirements[CGP_REQUIREMENT_COUNT - 1 - j] != 0) {
                                    OfferingCnt_7++;
                                }
                            }
                        }
                        work->command.phaseTimer = CGP_SET_STEP_DISCARD_BEGIN;
                        break;
                    }
                } else if (mas_6.cardId == CARD_ID_THIRD_ARMAMENT) {
                    if (CardPlayCntBtl(ownSide, CARD_ID_LV_10_KOS_MOS) &&
                        (CardPlayCntBtl(ownSide, CARD_ID_LV_1_SHION) || CardPlayCntBtl(ownSide, CARD_ID_SWIM_SUIT_SHION) ||
                         CardPlayCntBtl(ownSide, CARD_ID_LV_10_SHION))) {
                        work->command.phaseTimer++;
                    } else {
                        CGPSetErrorMess(side, work, CGP_ERR_NO_WEAPON_TARGET);
                        xglSoundEffectNormalID(SE_BUZZER, 0);
                    }
                } else {
                    work->command.phaseTimer++;
                }
            }
            if (CGP_HAND_FLIPPED(side, work)) {
                CGCPHandDispSub3(work->definitions, ownSide, mas_6.index, CGP_HAND_DISP_PICK, 0);
            } else {
                CGCPHandDispSub2(work->definitions, ownSide, mas_6.index, CGP_HAND_DISP_PICK);
            }
        } else if (pressed & PAD_CIRCLE) {
            xglSoundEffectNormalID(SE_BUZZER, 0);
        }
        break;
    }
    case CGP_SET_STEP_ATTACH_LIGHT:
    {
        s32 i;

        CardCursorPassive(&work->cursor);
        work->cursor.selectionMode = 1;
        if (mas_6.cardId == CARD_ID_THIRD_ARMAMENT) {
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (ownSide->battle[i].battle.layers[0].cardId == CARD_ID_LV_10_KOS_MOS) {
                    position.zone = zoneC;
                    position.index = i;
                    CardCursorActive(&work->cursor, &position, CGP_CURSOR_LIGHT);
                }
            }
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (ownSide->disposal[i].battle.layers[0].cardId == CARD_ID_LV_10_KOS_MOS) {
                    position.zone = zoneB;
                    position.index = i;
                    CardCursorActive(&work->cursor, &position, CGP_CURSOR_LIGHT);
                }
            }
        } else {
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (ownSide->battle[i].battle.layers[0].cardId >= 0 &&
                    CGPWeaponEquipChk(work, mas_6.cardId, ownSide->battle[i].battle.layers[0].cardId)) {
                    position.zone = zoneC;
                    position.index = i;
                    CardCursorActive(&work->cursor, &position, CGP_CURSOR_LIGHT);
                }
            }
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (ownSide->disposal[i].battle.layers[0].cardId >= 0 &&
                    CGPWeaponEquipChk(work, mas_6.cardId, ownSide->disposal[i].battle.layers[0].cardId)) {
                    position.zone = zoneB;
                    position.index = i;
                    CardCursorActive(&work->cursor, &position, CGP_CURSOR_LIGHT);
                }
            }
        }
        work->command.phaseTimer++;
        break;
    }
    case CGP_SET_STEP_ATTACH_PICK:
        CGPCursorMove2(side, work);
        CGPCursor2Pos(&position, work);
        if (pressed & PAD_CIRCLE) {
            CGPDisposeWeaponSub(side, work, mas_6, position);
            return;
        }
        if (pressed & PAD_CROSS) {
            xglSoundEffectNormalID(SE_CANCEL, 0);
            work->command.phaseTimer = CGP_SET_STEP_PICK_CARD;
        }
        break;
    case CGP_SET_STEP_DISCARD_BEGIN:
        work->cursor.selectionMode = 1;
        work->command.phaseTimer++;
        break;
    case CGP_SET_STEP_DISCARD_PICK:
    {
        s32 area;
        s32 i;

        zoneA = CGP_AREA_PLAYER_DISPOSAL;
        zoneB = CGP_AREA_PLAYER_BATTLE;
        if (side != 0) {
            zoneA = CGP_AREA_ENEMY_DISPOSAL;
            zoneB = CGP_AREA_ENEMY_BATTLE;
        }
        if (ownSide->hand.cards[mas_6.index] == CARD_ID_SANCTUARY_SHIP) {
            CardCursorPassive(&work->cursor);
            highlight.zone = zoneA;
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                highlight.index = i;
                CardCursorActive(&work->cursor, &highlight, CGP_CURSOR_LIGHT);
                if (ownSide->disposal[i].battle.layers[0].cardId >= 0) {
                    ownSide->disposal[i].battle.layers[0].life = 0;
                }
            }
            highlight.zone = zoneB;
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                highlight.index = i;
                CardCursorActive(&work->cursor, &highlight, CGP_CURSOR_LIGHT);
                if (ownSide->battle[i].battle.layers[0].cardId >= 0) {
                    ownSide->battle[i].battle.layers[0].life = 0;
                }
            }
            work->command.phaseTimer = CGP_SET_STEP_PUT_DOWN;
            break;
        }
        CGPCursorMove2(side, work);
        CGPCursor2Pos(&target, work);
        CardCursorPassive(&work->cursor);
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (ownSide->battle[i].battle.layers[0].cardId >= 0 &&
                (work->definitions[ownSide->battle[i].battle.layers[0].cardId].flags & CARD_ATTR_HUMAN)) {
                highlight.zone = zoneB;
                highlight.index = i;
                CardCursorActive(&work->cursor, &highlight, CGP_CURSOR_LIGHT);
            }
        }
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (ownSide->disposal[i].battle.layers[0].cardId >= 0 &&
                !(ownSide->disposal[i].battle.flags & CARD_STACK_FLAG_END) &&
                (work->definitions[ownSide->disposal[i].battle.layers[0].cardId].flags & CARD_ATTR_HUMAN)) {
                highlight.zone = zoneA;
                highlight.index = i;
                CardCursorActive(&work->cursor, &highlight, CGP_CURSOR_LIGHT);
            }
        }
        digits[0] = ZenkakuListB[OfferingCnt_7 * CGP_ZENKAKU_GLYPH_SIZE];
        digits[1] = ZenkakuListB[OfferingCnt_7 * CGP_ZENKAKU_GLYPH_SIZE + 1];
        digits[2] = 0;
        xglFontPrint(CGP_SET_DISCARD_TEXT_X, CGP_SET_PROMPT_Y, CGP_SET_TEXT_COLOR, "\v\031\003\r\0Cards to be discarded:");
        xglFontPrint(CGP_SET_DISCARD_COUNT_X, CGP_SET_PROMPT_Y, CGP_SET_TEXT_COLOR, digits);
        scale[0] = CGP_SET_PROMPT_SCALE_X;
        scale[1] = scale[2] = scale[3] = 1.0f;
        place[0] = place[1] = 0.0f;
        place[2] = CGP_SET_PROMPT_Z;
        place[3] = 1.0f;
        xglMatrixUnit(matrix);
        xglMatrixTrans(matrix, matrix, place);
        xglMatrixScale(matrix, matrix, scale);
        nmlModelSetPlace(&matrix);
        nmlModelSetTexture(CGP_PANEL_TEXTURE);
        nmlModelEntryCard(CGP_SET_PROMPT_MODEL);
        if (pressed & PAD_CIRCLE) {
            if (target.cardId < 0 || !(work->definitions[target.cardId].flags & CARD_ATTR_HUMAN)) {
                xglSoundEffectNormalID(SE_BUZZER, 0);
                break;
            }
            area = target.zone;
            if (zoneA == area || zoneB == area) {
                if (zoneA == area) {
                    if (ownSide->disposal[target.index].battle.flags & CARD_STACK_FLAG_END) {
                        xglSoundEffectNormalID(SE_BUZZER, 0);
                        break;
                    }
                    CardPlayDesertBattle(ownSide, &ownSide->disposal[target.index].battle);
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                    work->command.phaseTimer++;
                } else {
                    CardPlayDesertBattle(ownSide, &ownSide->battle[target.index].battle);
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                    work->command.phaseTimer++;
                }
            } else {
                xglSoundEffectNormalID(SE_BUZZER, 0);
            }
        } else if (pressed & PAD_CROSS) {
            xglSoundEffectNormalID(SE_BUZZER, 0);
        }
        break;
    }
    case CGP_SET_STEP_DISCARD_NEXT:
    {
        s32 area;
        s32 i;

        OfferingCnt_7--;
        if (OfferingCnt_7 > 0) {
            work->command.phaseTimer = (s16) work->command.phaseTimer - 1;
            break;
        }
        zoneA = CGP_AREA_PLAYER_DISPOSAL;
        zoneB = CGP_AREA_PLAYER_BATTLE;
        if (side != 0) {
            zoneA = CGP_AREA_ENEMY_DISPOSAL;
            zoneB = CGP_AREA_ENEMY_BATTLE;
        }
        CardCursorPassive(&work->cursor);
        highlight.zone = zoneA;
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (ownSide->disposal[i].battle.layers[0].cardId < 0) {
                highlight.index = i;
                CardCursorActive(&work->cursor, &highlight, CGP_CURSOR_LIGHT);
            }
        }
        highlight.zone = zoneB;
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (ownSide->battle[i].battle.layers[0].cardId < 0) {
                highlight.index = i;
                CardCursorActive(&work->cursor, &highlight, CGP_CURSOR_LIGHT);
            }
        }
        work->command.phaseTimer++;
        break;
    }
    case CGP_SET_STEP_PUT_DOWN:
    {
        s32 area;
        s32 i;

        zoneA = CGP_AREA_PLAYER_DISPOSAL;
        zoneB = CGP_AREA_PLAYER_BATTLE;
        if (side != 0) {
            zoneA = CGP_AREA_ENEMY_DISPOSAL;
            zoneB = CGP_AREA_ENEMY_BATTLE;
        }
        CGPCursorMove2(side, work);
        CGPCursor2Pos(&target, work);
        xglFontPrint(CGP_SET_PUT_DOWN_TEXT_X, CGP_SET_PROMPT_Y, CGP_SET_TEXT_COLOR, "\v\031\003\r\0Where do you want to put it?");
        scale[0] = CGP_SET_PROMPT_SCALE_X;
        scale[1] = scale[2] = scale[3] = 1.0f;
        xglMatrixUnit(matrix);
        xglMatrixScale(matrix, matrix, scale);
        nmlModelSetPlace(&matrix);
        nmlModelSetTexture(CGP_PANEL_TEXTURE);
        nmlModelEntryCard(CGP_SET_PROMPT_MODEL);
        if (pressed & PAD_CIRCLE) {
            if (target.cardId >= 0) {
                return;
            }
            area = target.zone;
            if (zoneA != area && zoneB != area) {
                return;
            }
            if (zoneB == area) {
                ownSide->battle[target.index].battle.flags = 0;
                ownSide->battle[target.index].battle.power = work->definitions[ownSide->hand.cards[mas_6.index]].basePower;
                ownSide->battle[target.index].battle.powerLimit = work->definitions[ownSide->hand.cards[mas_6.index]].basePower;
                ownSide->battle[target.index].battle.layers[0].cardId = ownSide->hand.cards[mas_6.index];
                ownSide->battle[target.index].battle.layers[0].life = work->definitions[ownSide->hand.cards[mas_6.index]].operationDuration;
                ownSide->battle[target.index].battle.position[0] = ownSide->handMove[mas_6.index].position[0];
                ownSide->battle[target.index].battle.position[1] = ownSide->handMove[mas_6.index].position[1];
                ownSide->battle[target.index].battle.position[2] = CGP_PLACE_START_Z;
                ownSide->battle[target.index].battle.step[0] = (BattlePos1P[target.index][0] - ownSide->handMove[mas_6.index].position[0]) / CGP_PLACE_SLIDE_DIVISOR;
                ownSide->battle[target.index].battle.step[1] = (BattlePos1P[target.index][1] - ownSide->handMove[mas_6.index].position[1]) / CGP_PLACE_SLIDE_DIVISOR;
                ownSide->battle[target.index].battle.step[2] = 0;
                ownSide->battle[target.index].battle.moveFrames = CGP_PLACE_FRAMES;
            } else {
                ownSide->disposal[target.index].battle.power = work->definitions[ownSide->hand.cards[mas_6.index]].basePower;
                ownSide->disposal[target.index].battle.powerLimit = work->definitions[ownSide->hand.cards[mas_6.index]].basePower;
                ownSide->disposal[target.index].battle.layers[0].cardId = ownSide->hand.cards[mas_6.index];
                ownSide->disposal[target.index].battle.layers[0].life = work->definitions[ownSide->hand.cards[mas_6.index]].operationDuration;
                ownSide->disposal[target.index].battle.position[0] = ownSide->handMove[mas_6.index].position[0];
                ownSide->disposal[target.index].battle.position[1] = ownSide->handMove[mas_6.index].position[1];
                ownSide->disposal[target.index].battle.position[2] = CGP_PLACE_START_Z;
                ownSide->disposal[target.index].battle.step[0] = (DisposePos1P[target.index][0] - ownSide->handMove[mas_6.index].position[0]) / CGP_PLACE_SLIDE_DIVISOR;
                ownSide->disposal[target.index].battle.step[1] = (DisposePos1P[target.index][1] - ownSide->handMove[mas_6.index].position[1]) / CGP_PLACE_SLIDE_DIVISOR;
                ownSide->disposal[target.index].battle.step[2] = 0;
                ownSide->disposal[target.index].battle.moveFrames = CGP_PLACE_FRAMES;
            }
            {
                s32 cost = work->definitions[ownSide->hand.cards[mas_6.index]].reserveCost;

                ownSide->hand.playPoints[0] -= work->definitions[ownSide->hand.cards[mas_6.index]].playCost;
                ownSide->hand.cards[mas_6.index] = -1;
                for (i = 0; i < cost; i++) {
                    CardPlayCostCard(ownSide);
                }
            }
            CardPlayCleanHand(&ownSide->hand);
            CardCursorPassive(&work->cursor);
            xglSoundEffectNormalID(SE_DECIDE, 0);
            work->command.phaseTimer = CGP_SET_STEP_PICK_CARD;
        }
        break;
    }
    case CGP_SET_STEP_PAIRED_SLOT:
    {
        CardLayerStack *stack;
        s32 slot;
        s32 i;
        s32 life;

        slot = 0;
        stack = 0;
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (mas_6.cardId - CGP_PAIRED_CARD_OFFSET == ownSide->battle[i].battle.layers[0].cardId) {
                slot = i;
                stack = &ownSide->battle[i].battle;
                break;
            }
        }
        if (stack != 0) {
            life = stack->layers[0].life - work->definitions[stack->layers[0].cardId].operationDuration;
            CardPlayDesertBattle(ownSide, stack);
            stack->power = work->definitions[mas_6.cardId].basePower;
            stack->powerLimit = work->definitions[mas_6.cardId].basePower;
            stack->layers[0].cardId = mas_6.cardId;
            stack->layers[0].life = work->definitions[mas_6.cardId].operationDuration + life;
            stack->position[0] = ownSide->handMove[mas_6.index].position[0];
            stack->position[1] = ownSide->handMove[mas_6.index].position[1];
            stack->position[2] = CGP_PLACE_START_Z;
            stack->step[0] = (BattlePos1P[slot][0] - ownSide->handMove[mas_6.index].position[0]) / CGP_PLACE_SLIDE_DIVISOR;
            stack->step[1] = (BattlePos1P[slot][1] - ownSide->handMove[mas_6.index].position[1]) / CGP_PLACE_SLIDE_DIVISOR;
            stack->step[2] = 0;
            stack->moveFrames = CGP_PLACE_FRAMES;
            ownSide->hand.playPoints[0] -= work->definitions[ownSide->hand.cards[mas_6.index]].playCost;
            ownSide->hand.cards[mas_6.index] = -1;
            CardPlayCleanHand(&ownSide->hand);
            xglSoundEffectNormalID(SE_DECIDE, 0);
        }
        work->command.phaseTimer = CGP_SET_STEP_PICK_CARD;
        break;
    }
    }
}










/*
 * Runs the steps of one command card, selected by its card id and work->command.step, for the
 * side given. Returns -1 when the command is finished.
 */
s32 CGPCFPlayCommOperation(s16 side, CardGameWork *work, s16 command)
{
    CardPlaySide *own;
    CardPlaySide *foe;
    u16 held;
    u16 pressed;
    u16 repeat;
    s32 result;
    float firstScale[4];
    Matrix4 firstMatrix;
    float secondScale[4];
    Matrix4 secondMatrix;
    static CardCursorPosition cursor;        /* cursor */
    static s16 card0[1];                    /* card0: first card picked to throw away */
    static s16 card1;                       /* card1: second card */
    static CardCursorPosition handCursor;    /* work: hand slot under the cursor */
    static s32 NowNo;                       /* NowNo: deck position */
    static s32 lp;                          /* lp: cost cards still to pay */
    static s32 timer;                       /* timer: frames to wait */
    CardCursorPosition ownPos;
    CardCursorPosition foePos;
    CardCursorPosition opePos;

    result = 0;
    if (side == 0) {
        own = &work->player;
        foe = &work->enemy;
        held = work->command.input[0].held;
        pressed = work->command.input[0].pressed;
        repeat = work->command.input[0].repeat;
    } else {
        own = &work->enemy;
        foe = &work->player;
        held = work->command.input[1].held;
        pressed = work->command.input[1].pressed;
        repeat = work->command.input[1].repeat;
    }
    switch (command) {
    case CARD_ID_JULI_MIZRAHI:
        switch (work->command.step) {
        case 0:
            work->command.step++;
            break;
        case 1:
            CCU063ExecSub(work, own);
            work->command.step++;
            break;
        case 2:
            result = -1;
            break;
        }
        break;
    case CARD_ID_PELLEGRI:
        switch (work->command.step) {
        case 0:
            work->command.step++;
            break;
        case 1:
            CCU064ExecSub(work, own);
            work->command.step++;
            break;
        case 2:
            result = -1;
            break;
        }
        break;
    case CARD_ID_ZOHAR_EMULATOR:
        switch (work->command.step) {
        case 0:
            work->command.step++;
            break;
        case 1:
            CCU065ExecSub(work, own);
            work->command.step++;
            break;
        case 2:
            result = -1;
            break;
        }
        break;
    /* Gnosify: needs two cards in hand; the player picks two different cards, which are discarded. */
    case CARD_ID_GNOSIFY:
    {
        s16 handCount = CardPlayHandCnt(&own->hand);

        switch (work->command.step) {
        case 0:
            card0[0] = 0;
            card1 = 0;
            if (handCount < 2) {
                CGPSetErrorMess(side, work, CGP_ERR_COMMAND_FAILED);
                result = 1;
                xglSoundEffectNormalID(SE_BUZZER, 0);
                break;
            }
            work->command.step++;
            break;
        case 1:
            xglFontPrint(CGP_DISCARD_PROMPT_X, CGP_DISCARD_PROMPT_Y, CGP_DISCARD_PROMPT_COLOR, cgpDiscardTwoPrompt);
            firstScale[0] = CGP_DISCARD_PROMPT_SCALE_X;
            firstScale[3] = 1.0f;
            firstScale[2] = 1.0f;
            firstScale[1] = 1.0f;
            xglMatrixUnit(firstMatrix);
            xglMatrixScale(firstMatrix, firstMatrix, firstScale);
            nmlModelSetPlace(&firstMatrix);
            nmlModelSetTexture(CGP_PANEL_TEXTURE);
            nmlModelEntryCard(CGP_DISCARD_PROMPT_MODEL);
            if (CGP_HAND_FLIPPED(side, work)) {
                CGCPHandDispSub3(work->definitions, own, card0[0], 0, 0);
            } else {
                CGCPHandDispSub2(work->definitions, own, card0[0], 0);
            }
            if (repeat & PAD_LEFT) {
                if (card0[0] > 0) {
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                    card0[0]--;
                }
            } else if (repeat & PAD_RIGHT) {
                if (card0[0] < handCount - 1) {
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                    card0[0]++;
                }
            }
            if (pressed & PAD_CIRCLE) {
                xglSoundEffectNormalID(SE_DECIDE, 0);
                work->command.step++;
            }
            break;
        case 2:
            xglFontPrint(CGP_DISCARD_PROMPT_X, CGP_DISCARD_PROMPT_Y, CGP_DISCARD_PROMPT_COLOR, cgpDiscardOnePrompt);
            secondScale[0] = CGP_DISCARD_PROMPT_SCALE_X;
            secondScale[3] = 1.0f;
            secondScale[2] = 1.0f;
            secondScale[1] = 1.0f;
            xglMatrixUnit(secondMatrix);
            xglMatrixScale(secondMatrix, secondMatrix, secondScale);
            nmlModelSetPlace(&secondMatrix);
            nmlModelSetTexture(CGP_PANEL_TEXTURE);
            nmlModelEntryCard(CGP_DISCARD_PROMPT_MODEL);
            if (CGP_HAND_FLIPPED(side, work)) {
                CGCPHandDispSub3(work->definitions, own, card1, 0, 0);
            } else {
                CGCPHandDispSub2(work->definitions, own, card1, 0);
            }
            if (repeat & PAD_LEFT) {
                if (card1 > 0) {
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                    card1--;
                }
            } else if (repeat & PAD_RIGHT) {
                if (card1 < handCount - 1) {
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                    card1++;
                }
            }
            if (pressed & PAD_CIRCLE) {
                if (card0[0] == card1) {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                    break;
                }
                xglSoundEffectNormalID(SE_DECIDE, 0);
                work->command.step++;
            }
            break;
        case 3:
            CGPDispErrorMess(side, CGP_MESS_SELECT_CARD);
            if (pressed & PAD_CIRCLE) {
                xglSoundEffectNormalID(SE_DECIDE, 0);
                work->command.step++;
            } else if (pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_CANCEL, 0);
                work->command.step = 1;
            }
            break;
        case 4:
            result = 1;
            CardPlayDesert2Card(own, card0[0], card1);
            CardPlayDrawSute(own);
            break;
        }
        break;
    }
    case CARD_ID_TRANSFIGURATION:
        switch (work->command.step) {
        case 0:
        {
            s32 count = CardPlayHandCnt(&own->hand);
            s32 found = 0;
            s32 i;

            for (i = 0; i < count; i++) {
                CardDefinitionView *definitions = (CardDefinitionView *) work->definitions;

                if (definitions[own->hand.cards[i]].kind == CARD_KIND_BATTLE &&
                    (definitions[own->hand.cards[i]].flags & CARD_ATTR_GNOSIS)) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                xglSoundEffectNormalID(SE_CANCEL, 0);
                CGPSetErrorMess(side, work, CGP_ERR_COMMAND_FAILED);
                result = 1;
            } else {
                handCursor.index = 0;
                work->command.step++;
            }
            break;
        }
        case 1:
        {
            s32 count;
            s32 flipped;

            if (side == 0) {
                flipped = work->rotStageId != 0;
            } else {
                flipped = work->rotStageId == 0;
            }
            CGCPHandDispSub(&own->hand, handCursor.index, flipped);
            count = CardPlayHandCnt(&own->hand);
            if (repeat & PAD_RIGHT) {
                if (handCursor.index < count - 1) {
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                    handCursor.index++;
                }
            } else if (repeat & PAD_LEFT) {
                if (handCursor.index > 0) {
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                    handCursor.index--;
                }
            }
            if (pressed & PAD_CIRCLE) {
                if (!(work->definitions[own->hand.cards[handCursor.index]].flags & CARD_ATTR_GNOSIS)) {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                    break;
                }
                handCursor.zone = CGP_AREA_PLAYER_HAND;
                if (side != 0) {
                    handCursor.zone = CGP_AREA_ENEMY_HAND;
                }
                work->command.step++;
                xglSoundEffectNormalID(SE_DECIDE, 0);
                break;
            }
            break;
        }
        case 2:
            CGPDispErrorMessPlus(side, CGP_MESS_SELECT_DISCARD, own->hand.cards[handCursor.index]);
            if (pressed & PAD_CIRCLE) {
                NowNo = 0;
                work->command.step++;
                CardPlayDesertCard(own, handCursor.index);
            }
            if (pressed & PAD_CROSS) {
                work->command.phaseTimer = (s16) work->command.phaseTimer - 1;
            }
            break;
        case 3:
        {
            s32 yamaCount = CardPlayYamaCnt(own);

            if (pressed & PAD_CIRCLE) {
                if (work->definitions[own->deck[NowNo]].flags & CARD_ATTR_GNOSIS) {
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                    work->command.step++;
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            }
            if (pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_CANCEL, 0);
                work->command.step = 0;
                result = 1;
            }
            if (repeat & PAD_RIGHT) {
                NowNo++;
                xglSoundEffectNormalID(SE_CURSOR, 0);
                if (NowNo >= yamaCount) {
                    NowNo = yamaCount - 1;
                }
            }
            if (repeat & PAD_LEFT) {
                NowNo--;
                xglSoundEffectNormalID(SE_CURSOR, 0);
                if (NowNo < 0) {
                    NowNo = 0;
                }
            }
            CGCPYamaDispSub(own, NowNo, yamaCount, held, 0);
            break;
        }
        case 4:
            work->command.step++;
            break;
        case 5:
        {
            s32 count = CardPlayHandCnt(&own->hand);

            own->hand.cards[count] = own->deck[NowNo];
            own->deck[NowNo] = -1;
            CardPlayCleanYama(own);
            CardPlayShuffleYama(own);
            work->command.step++;
            break;
        }
        case 6:
            work->command.step = 10;
            break;
        case 10:
            result = 1;
            break;
        }
        break;
    case CARD_ID_SO_WEAK:
        switch (work->command.step) {
        case 0:
        {
            s32 i;

            ownPos.zone = CGP_AREA_PLAYER_BATTLE;
            foePos.zone = CGP_AREA_ENEMY_BATTLE;
            if (side != 0) {
                ownPos.zone = CGP_AREA_ENEMY_BATTLE;
                foePos.zone = CGP_AREA_PLAYER_BATTLE;
            }
            CardCursorPassive(&work->cursor);
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (own->battle[i].battle.layers[0].cardId >= 0) {
                    ownPos.index = i;
                    CardCursorActive(&work->cursor, &ownPos, CGP_CURSOR_PICK);
                }
                if (foe->battle[i].battle.layers[0].cardId >= 0) {
                    foePos.index = i;
                    CardCursorActive(&work->cursor, &foePos, CGP_CURSOR_PICK);
                }
            }
            work->command.step++;
            break;
        }
        case 1:
            CGPCursorMove2(side, work);
            CGPCursor2Pos(&cursor, work);
            if (pressed & PAD_CIRCLE) {
                if (cursor.cardId < 0 ||
                    !(cursor.zone == CGP_AREA_ENEMY_BATTLE || cursor.zone == CGP_AREA_PLAYER_BATTLE)) {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                    break;
                }
                xglSoundEffectNormalID(SE_DECIDE, 0);
                work->command.step++;
            }
            break;
        case 2:
            CGPDispErrorMess(side, CGP_MESS_SELECT_CARD);
            if (pressed & PAD_CIRCLE) {
                xglSoundEffectNormalID(SE_DECIDE, 0);
                work->command.step++;
            } else if (pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_CANCEL, 0);
                work->command.step = 1;
            }
            break;
        case 3:
        {
            CGPStackSlot *slots;
            s32 targetSide;

            if (cursor.zone == CGP_AREA_ENEMY_BATTLE) {
                slots = (CGPStackSlot *) &work->enemy.battle[0].battle;
                targetSide = 1;
            } else if (cursor.zone == CGP_AREA_PLAYER_BATTLE) {
                slots = (CGPStackSlot *) &work->player.battle[0].battle;
                targetSide = 0;
            } else {
                slots = (CGPStackSlot *) &work->enemy.battle[0].battle;
                targetSide = 1;
            }
            slots[cursor.index].stack.layers[0].life -= CGP_TARGET_LIFE_LOSS;
            if (slots[cursor.index].stack.layers[0].life < 0) {
                slots[cursor.index].stack.layers[0].life = 0;
            }
            CardSetEffect(targetSide, work, CGP_EFFECT_BURST,0, CGP_SE_COMMAND_EFFECT,
                          slots[cursor.index].stack.position,
                          &slots[cursor.index].stack);
            CardSetEffect(targetSide, work, CGP_EFFECT_NUMBER_TOP, 3, 0,
                          slots[cursor.index].stack.position,
                          &slots[cursor.index].stack);
            result = -1;
            break;
        }
        }
        break;
    case CARD_ID_PHENOMENON_CHANGE:
    {
        s32 opeZone = CGP_AREA_ENEMY_OPERATION;

        if (side != 0) {
            opeZone = CGP_AREA_PLAYER_OPERATION;
        }

        switch (work->command.step) {
        case 0:
        {
            s32 i;

            CardCursorPassive(&work->cursor);
            opePos.zone = opeZone;
            for (i = 0; i < CARD_OPERATION_SLOTS; i++) {
                if (foe->operations[i] >= 0) {
                    opePos.index = i;
                    CardCursorActive(&work->cursor, &opePos, CGP_CURSOR_PICK);
                }
            }
            work->command.step++;
            break;
        }
        case 1:
            CGPCursorMove2(side, work);
            CGPCursor2Pos(&cursor, work);
            if (pressed & PAD_CIRCLE) {
                if (cursor.cardId < 0 || opeZone != cursor.zone) {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                    break;
                }
                work->command.step++;
            }
            break;
        case 2:
            CGPDispErrorMess(side, CGP_MESS_SELECT_CARD);
            if (pressed & PAD_CIRCLE) {
                xglSoundEffectNormalID(SE_DECIDE, 0);
                work->command.step++;
            } else if (pressed & PAD_CROSS) {
                xglSoundEffectNormalID(SE_CANCEL, 0);
                work->command.step = 1;
            }
            break;
        case 3:
            result = -1;
            CardPlayDesertOperation(foe, cursor.index);
            break;
        }
        break;
    }
    /* Rhine Maiden: the foe pays CGP_COST_REPEAT cost cards, one per pass. */
    case CARD_ID_RHINE_MAIDEN:
        switch (work->command.step) {
        case 0:
            work->command.step++;
            lp = CGP_COST_REPEAT;
            CardSetEffect(side, work, CGP_EFFECT_BURST,0, CGP_SE_COMMAND_EFFECT, 0, 0);
            CardSetEffect(side, work, 13, 4, 0, 0, 0);
            break;
        case 1:
            CardPlayCostCard(foe);
            work->command.step++;
            break;
        case 2:
            if (--lp != 0) {
                work->command.step--;
            } else {
                work->command.step++;
            }
            break;
        case 3:
            result = -1;
            break;
        }
        break;
    /* Unknown Armament: waits CGP_EFFECT_DELAY frames before its effect. */
    case CARD_ID_UNKNOWN_ARMAMENT:
        switch (work->command.step) {
        case 0:
            work->command.step++;
            timer = CGP_EFFECT_DELAY;
            break;
        case 1:
            if (timer != 0) {
                timer--;
                break;
            }
            CCO143ExecSub(side, work, own, foe);
            timer = CGP_EFFECT_HOLD;
            work->command.step++;
            break;
        case 2:
            if (timer != 0) {
                timer--;
                break;
            }
            result = -1;
            break;
        }
        break;
    }
    return result;
}








/*
 * Draws the "block" prompt for the command card cardId: the card on its backdrop,
 * the prompt text ("Block this event?" while the other side has no answer card,
 * "Event blocked. Block further?" once it has one), the hint line ("If yes, select
 * card: ... No: ...") and the answer cards of both sides in two rows. The calls
 * from CGPCommFaseSub also pass a fifth halfword (p2no_18) that is not read here.
 */
void CGPCFSDispSub(s32 frameSide, s32 side, CardGameWork *work, s16 cardId, s16 unusedIndex)
{
    float scale[4];
    float position[4];
    Matrix4 base;
    Matrix4 place;
    CardHand *ownHand;
    CardHand *foeHand;
    s32 found;
    s32 i;
    s32 count;
    float xShift;
    float spacing;
    s16 *cards;

    ownHand = &work->player.hand;
    foeHand = &work->enemy.hand;
    xglMatrixUnit(base);
    position[2] = CGP_BACKDROP_Z;
    scale[1] = CGP_BACKDROP_SCALE_Y;
    position[0] = 0.0f;
    position[1] = 0.0f;
    position[3] = 1.0f;
    scale[0] = 1.0f;
    scale[3] = 1.0f;
    scale[2] = 1.0f;
    xglMatrixTrans(base, base, position);
    spacing = CGP_ANSWER_CARD_PITCH;
    xglMatrixScale(place, base, scale);
    if (side == 0) {
        nmlModelSetLight(&Player1Color, &asDir);
    } else {
        nmlModelSetLight(&Player2Color, &asDir);
    }
    found = 0;
    nmlModelSetTexture(CGP_PANEL_TEXTURE);
    nmlModelSetPlace(&place);
    nmlModelEntryCard(CGP_CARD_BACKDROP_MODEL);
    position[0] = CGP_MAIN_CARD_X;
    position[1] = 0.0f;
    position[2] = CGP_MAIN_CARD_Z;
    position[3] = 1.0f;
    scale[1] = CGP_MAIN_CARD_SCALE;
    scale[0] = CGP_MAIN_CARD_SCALE;
    scale[3] = 1.0f;
    scale[2] = 1.0f;
    xglMatrixTrans(place, base, position);
    xglMatrixScale(place, place, scale);
    CardDispCard(&place, cardId, 1);
    if (side == 0) {
        for (i = 0; i < CGP_ANSWER_MAX; i++) {
            if (foeHand->cards[CGP_ANSWER_OFFSET + i] >= 0) {
                found = 1;
                break;
            }
        }
    } else {
        for (i = 0; i < CGP_ANSWER_MAX; i++) {
            if (ownHand->cards[CGP_ANSWER_OFFSET + i] >= 0) {
                found = 1;
                break;
            }
        }
    }
    if (found == 0) {
        if (frameSide == 0) {
            nmlModelSetLight(&CLP1Color, &asDir);
        } else {
            nmlModelSetLight(&CLP2Color, &asDir);
        }
        nmlModelSetPlace(&place);
        nmlModelSetTexture(work->cursorXtx[2]);
        nmlModelEntryCard(CGP_ANSWER_FRAME_MODEL);
        xglFontPrint(CGP_ANSWER_PROMPT_X, CGP_ANSWER_PROMPT_Y, CGP_TEXT_COLOR, cgpBlockEventPrompt);
    } else {
        xglFontPrint(CGP_ANSWER_PROMPT_X, CGP_ANSWER_PROMPT_Y, CGP_TEXT_COLOR, cgpBlockFurtherPrompt);
    }
    xglFontPrint(CGP_ANSWER_HINT_X, CGP_ANSWER_HINT_Y, CGP_TEXT_COLOR, cgpBlockSelectCardHint);
    scale[1] = CGP_ANSWER_CARD_SCALE;
    scale[0] = CGP_ANSWER_CARD_SCALE;
    scale[3] = 1.0f;
    scale[2] = 1.0f;
    xShift = 0.0f;
    if (frameSide == 0) {
        xShift = CGP_ANSWER_PANEL_SHIFT;
    }
    count = 0;
    cards = ownHand->cards;
    for (i = 0; i < CGP_ANSWER_MAX; i++) {
        if (cards[CGP_ANSWER_OFFSET + i] >= 0) {
            count++;
        }
    }
    position[1] = -CGP_ANSWER_ROW_Y;
    if (work->rotStageId != 0) {
        position[1] = -position[1];
    }
    for (i = 0; i < count; i++) {
        position[0] = spacing * i + CGP_ANSWER_CARD_START_X + xShift;
        position[2] = i * CGP_ANSWER_CARD_STEP_Z + CGP_ANSWER_CARD_STEP_Z;
        xglMatrixTrans(place, base, position);
        xglMatrixScale(place, place, scale);
        if (work->rotStageId != 0) {
            xglMatrixRotZ(place, place, CGP_HALF_TURN);
        }
        CardDispCard(&place, ownHand->cards[CGP_ANSWER_OFFSET + i], 1);
    }
    if (count != 0 && side != 0) {
        nmlModelSetLight(&CLP1Color, &asDir);
        nmlModelSetPlace(&place);
        nmlModelSetTexture(work->cursorXtx[2]);
        nmlModelEntryCard(CGP_ANSWER_FRAME_MODEL);
    }
    count = 0;
    cards = foeHand->cards;
    for (i = 0; i < CGP_ANSWER_MAX; i++) {
        if (cards[CGP_ANSWER_OFFSET + i] >= 0) {
            count++;
        }
    }
    xShift = 0.0f;
    if (frameSide != 0) {
        xShift = CGP_ANSWER_PANEL_SHIFT;
    }
    position[1] = CGP_ANSWER_ROW_Y;
    if (work->rotStageId != 0) {
        position[1] = -position[1];
    }
    scale[1] = CGP_ANSWER_CARD_SCALE;
    scale[3] = 1.0f;
    scale[0] = CGP_ANSWER_CARD_SCALE;
    scale[2] = 1.0f;
    for (i = 0; i < count; i++) {
        position[0] = spacing * i + CGP_ANSWER_CARD_START_X + xShift;
        position[2] = i * CGP_ANSWER_CARD_STEP_Z + CGP_ANSWER_CARD_STEP_Z;
        xglMatrixTrans(place, base, position);
        xglMatrixScale(place, place, scale);
        if (work->rotStageId == 0) {
            xglMatrixRotZ(place, place, CGP_HALF_TURN);
        }
        CardDispCard(&place, foeHand->cards[CGP_ANSWER_OFFSET + i], 1);
    }
    if (count != 0 && side == 0) {
        nmlModelSetLight(&CLP2Color, &asDir);
        nmlModelSetPlace(&place);
        nmlModelSetTexture(work->cursorXtx[2]);
        nmlModelEntryCard(CGP_ANSWER_FRAME_MODEL);
    }
}

/*
 * Records the answered card (hand->cards[cardIndex]) in the first empty slot
 * of hand->answerCards, pays its cost by calling CardPlayCostCard that many
 * times, removes it from hand->cards and tail-calls CardPlayCleanHand.
 */
void CGPAnswerSub(CardGameWork *work, CardPlaySide *side, s32 cardIndex)
{
    s32 i;
    s32 cost;

    for (i = 0; i < 40; i++) {
        if (side->answerCards[i] < 0) {
            break;
        }
    }
    side->answerCards[i] = side->hand.cards[cardIndex];
    cost = work->definitions[side->hand.cards[cardIndex]].reserveCost;
    for (i = 0; i < cost; i++) {
        CardPlayCostCard(side);
    }
    side->hand.cards[cardIndex] = -1;
    CardPlayCleanHand(&side->hand);
}

/*
 * Commands one side's disposal slots play once its command phase ends. A slot
 * holding one of these cards whose stack carries flag 1 plays the command that
 * card ends the phase with: Bunnie, Testament, Allen, Tony, Gaignun and Margulis
 * each play their own trigger command.
 */
void CGPCommFaseEndSub(s32 side, CardPlaySide *ownSide, CardGameWork *work)
{
    s32 i;

    if (CardPlayCntDsp(ownSide, CARD_ID_BUNNIE) != 0) {
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (ownSide->disposal[i].battle.layers[0].cardId == CARD_ID_BUNNIE) {
                if (ownSide->disposal[i].battle.flags & CARD_STACK_FLAG_END) {
                    CardPlayCommandPlay(side, CARD_COMMAND_BUNNIE_TRIGGER, work, 0, 0);
                }
            }
        }
    }
    if (CardPlayCntDsp(ownSide, CARD_ID_TESTAMENT) != 0) {
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (ownSide->disposal[i].battle.layers[0].cardId == CARD_ID_TESTAMENT) {
                if (ownSide->disposal[i].battle.flags & CARD_STACK_FLAG_END) {
                    CardPlayCommandPlay(side, CARD_COMMAND_TESTAMENT_TRIGGER, work, 0, 0);
                }
            }
        }
    }
    if (CardPlayCntDsp(ownSide, CARD_ID_ALLEN) != 0) {
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (ownSide->disposal[i].battle.layers[0].cardId == CARD_ID_ALLEN) {
                if (ownSide->disposal[i].battle.flags & CARD_STACK_FLAG_END) {
                    CardPlayCommandPlay(side, CARD_COMMAND_ALLEN_TRIGGER, work, 0, 0);
                }
            }
        }
    }
    if (CardPlayCntDsp(ownSide, CARD_ID_TONY) != 0) {
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (ownSide->disposal[i].battle.layers[0].cardId == CARD_ID_TONY) {
                if (ownSide->disposal[i].battle.flags & CARD_STACK_FLAG_END) {
                    CardPlayCommandPlay(side, CARD_COMMAND_TONY_TRIGGER, work, 0, 0);
                }
            }
        }
    }
    if (CardPlayCntDsp(ownSide, CARD_ID_GAIGNUN) != 0) {
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (ownSide->disposal[i].battle.layers[0].cardId == CARD_ID_GAIGNUN) {
                if (ownSide->disposal[i].battle.flags & CARD_STACK_FLAG_END) {
                    CardPlayCommandPlay(side, CARD_COMMAND_GAIGNUN_TRIGGER, work, 0, 0);
                }
            }
        }
    }
    if (CardPlayCntDsp(ownSide, CARD_ID_MARGULIS) != 0) {
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (ownSide->disposal[i].battle.layers[0].cardId == CARD_ID_MARGULIS) {
                if (ownSide->disposal[i].battle.flags & CARD_STACK_FLAG_END) {
                    CardPlayCommandPlay(side, CARD_COMMAND_MARGULIS_TRIGGER, work, 0, 0);
                }
            }
        }
    }
}






static CardCursorPosition mas_15;
static s16 no_16;
static s16 p1no_17;
static s16 p2no_18;

/*
 * Command phase of one side, one step per frame (CGP_COMM_STEP_*): checks the junk pile for
 * Flag Reset, sorts and picks a card from the hand, plays it and runs the answer steps of both
 * sides. cpuTurn is set when the computer plays this side.
 */
void CGPCommFaseSub(s32 side, CardGameWork *work, u16 cpuTurn, u16 cpuFoe)
{
    u8 order[3];
    Matrix4 matrix;
    float scale[4];
    float position[4];
    Matrix4 backMatrix;
    float backScale[4];
    float backPosition[4];
    CardPlaySide *ownSide;
    CardPlaySide *foeSide;
    u16 pressed;
    u16 pressedFoe;
    s32 foeIndex;
    s32 operationArea;
    s32 handArea;
    s32 battleArea;

    if (side == 0) {
        foeIndex = 1;
        ownSide = &work->player;
        foeSide = &work->enemy;
        pressedFoe = work->command.input[1].pressed;
        pressed = work->command.input[0].pressed;
        operationArea = CGP_AREA_PLAYER_OPERATION;
        battleArea = CGP_AREA_PLAYER_BATTLE;
        handArea = CGP_AREA_PLAYER_HAND;
    } else {
        foeIndex = 0;
        ownSide = &work->enemy;
        foeSide = &work->player;
        pressedFoe = work->command.input[0].pressed;
        pressed = work->command.input[1].pressed;
        operationArea = CGP_AREA_ENEMY_OPERATION;
        battleArea = CGP_AREA_ENEMY_BATTLE;
        handArea = CGP_AREA_ENEMY_HAND;
    }
    switch ((s16) work->command.phaseTimer) {
    case CGP_COMM_STEP_JUNK_CHECK:
    {
        s32 i;

        if (work->command.phase == CARD_PHASE_EVENT || work->command.phase == CARD_PHASE_EVENT + CARD_PHASES_PER_SIDE) {
            if (CardPlayCntBtl(ownSide, CGP_CARD_FLAG_RESET) != 0 && CardPlayBattleCnt(ownSide) == 1) {
                s32 junkCount = CardPlayJunkCnt(ownSide);

                if (junkCount != 0) {
                    CGPSetErrorMessPlus(side, work, CGP_MESS_JUNK_COUNTED, CGP_CARD_FLAG_RESET);
                    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                        if (ownSide->battle[i].battle.layers[0].cardId == CGP_CARD_FLAG_RESET) {
                            ownSide->battle[i].battle.layers[0].flag = junkCount;
                            break;
                        }
                    }
                    for (i = 0; i < junkCount; i++) {
                        ownSide->junk[i] = -1;
                    }
                }
            }
        }
        if (cpuTurn != 0) {
            work->command.phaseTimer = CGP_COMM_STEP_ENEMY_REQUEST;
        } else {
            work->command.phaseTimer++;
        }
        break;
    }
    case CGP_COMM_STEP_SORT_HAND:
    {
        s32 playable;
        s32 value;
        s32 i;

        value = CGP_AREA_PLAYER_HAND;
        if (side != 0) {
            value = CGP_AREA_ENEMY_HAND;
        }
        CGPSetCursor(work, value, 0);
        value = CardPlayHandCnt(&ownSide->hand);
        order[0] = 2;
        order[1] = 1;
        order[2] = 4;
        CardPlayTypeSort(work->definitions, ownSide->hand.cards, value, order);
        playable = 0;
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (ownSide->disposal[i].battle.layers[0].cardId >= 0) {
                playable++;
                break;
            }
        }
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (ownSide->battle[i].battle.layers[0].cardId >= 0) {
                playable++;
                break;
            }
        }
        for (i = 0; i < CARD_OPERATION_SLOTS; i++) {
            if (ownSide->operations[i] >= 0) {
                playable++;
                break;
            }
        }
        if (playable != 0) {
            playable = 0;
            for (i = 0; i < value; i++) {
                s32 card = ownSide->hand.cards[i];

                if (work->definitions[card].kind == CARD_KIND_EVENT) {
                    if (card != CARD_ID_MIYUKIS_EMAIL) {
                        if (card != CARD_ID_SONG) {
                            playable++;
                        }
                    }
                }
            }
            for (i = 0; i < CARD_OPERATION_SLOTS; i++) {
                if (ownSide->operations[i] >= 0) {
                    if (CGPChkPlayable(ownSide->operations[i])) {
                        playable++;
                    }
                }
            }
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (ownSide->battle[i].battle.layers[0].cardId >= 0) {
                    if (CGPChkPlayable(ownSide->battle[i].battle.layers[0].cardId)) {
                        playable++;
                    }
                }
            }
        }
        if (playable == 0) {
            work->command.phaseTimer = CGP_COMM_STEP_END_PHASE;
            if ((side == 0 && (work->flags & CGP_FLAG_SIDE0_AUTO)) || (side != 0 && (work->flags & CGP_FLAG_SIDE1_AUTO))) {
                return;
            }
            if ((side == 0 && (work->save->flags & CGP_SAVE_FLAG_SIDE0_AUTO)) || (side != 0 && (work->save->flags & CGP_SAVE_FLAG_SIDE1_AUTO))) {
                return;
            }
            CGPSetErrorMess(side, work, CGP_ERR_NO_PLAYABLE_CARD);
            break;
        }
        CGPRotStageSub(work, side);
        work->command.phaseTimer++;
        work->cursor.selectionMode = 1;
        break;
    }
    case CGP_COMM_STEP_PICK:
    {
        u8 acted;

        CGPMoveSub(side, ownSide, work);
        CGPCursor2Pos(&mas_15, work);
        no_16 = mas_15.cardId;
        acted = 0;
        work->cursor.selectionMode = 1;
        if (work->cursorMoveAny == 0 && handArea == mas_15.zone) {
            if (CGP_HAND_FLIPPED(side, work)) {
                work->cursor.selectionMode = 0;
                CGCPHandDispSub3(work->definitions, ownSide, mas_15.index, 2, 0);
            } else {
                work->cursor.selectionMode = 0;
                CGCPHandDispSub2(work->definitions, ownSide, mas_15.index, 2);
            }
        }
        if (pressed & PAD_CIRCLE) {
            if (work->cursorMoveAny == 0 && no_16 >= 0) {
                CGPCursor2Pos(&mas_15, work);
                if ((operationArea == mas_15.zone && (ownSide->operationFlags[mas_15.index] & 1) == 0) ||
                    (battleArea == mas_15.zone &&
                     (ownSide->battle[mas_15.index].battle.flags & CARD_STACK_FLAG_FROZEN) == 0)) {
                    if (CGPChkPlayable(no_16)) {
                        s32 allowed = 1;

                        if (battleArea != mas_15.zone) {
                            allowed = CGPChkOpePlayable(side, work, mas_15.cardId) != 0;
                        }
                        if (allowed) {
                            CardCursorActive(&work->cursor, &mas_15, 2);
                            xglSoundEffectNormalID(SE_DECIDE, 0);
                            work->cursor.selectionMode = 1;
                            acted = 1;
                            work->command.phaseTimer++;
                        } else {
                            CGPSetErrorMess(side, work, CGP_ERR_COMMAND_FAILED);
                            xglSoundEffectNormalID(SE_BUZZER, 0);
                        }
                    } else {
                        CGPSetErrorMess(side, work, CGP_ERR_NOT_PLAYABLE);
                        xglSoundEffectNormalID(SE_BUZZER, 0);
                    }
                } else if (handArea == mas_15.zone) {
                    if (work->definitions[mas_15.cardId].kind & CARD_KIND_EVENT) {
                        work->cursor.selectionMode = 0;
                        work->command.phaseTimer = CGP_COMM_STEP_CONDITION;
                        acted = 1;
                    } else {
                        CGPSetErrorMess(side, work, CGP_ERR_NOT_DISCARDABLE);
                        xglSoundEffectNormalID(SE_BUZZER, 0);
                    }
                } else {
                    if (battleArea == mas_15.zone && (ownSide->battle[mas_15.index].battle.flags & CARD_STACK_FLAG_FROZEN)) {
                        CGPSetErrorMess(side, work, CGP_ERR_SLOT_LOCKED);
                    } else {
                        CGPSetErrorMess(side, work, CGP_ERR_WRONG_AREA);
                    }
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            }
            if (acted != 0) {
                return;
            }
            xglSoundEffectNormalID(SE_BUZZER, 0);
        }
        break;
    }
    case CGP_COMM_STEP_CONFIRM:
        CGPDispErrorMessPlus(side, CGP_MESS_CONFIRM_CARD, no_16);
        if (pressed & PAD_CIRCLE) {
            work->command.step = 0;
            work->command.phaseTimer++;
            xglSoundEffectNormalID(SE_DECIDE, 0);
            return;
        }
        if (pressed & PAD_CROSS) {
            work->command.phaseTimer = (s16) work->command.phaseTimer - 1;
            CardCursorPassive(&work->cursor);
            xglSoundEffectNormalID(SE_CANCEL, 0);
        }
        break;
    case CGP_COMM_STEP_OPERATION:
    {
        s32 result = CGPCFPlayCommOperation(side, work, no_16);

        if (result == 0) {
            return;
        }
        if (result < 0) {
            if (work->definitions[no_16].kind == CARD_KIND_SITUATION) {
                CardPlayDesertOperation(ownSide, mas_15.index);
            } else {
                ownSide->battle[mas_15.index].battle.layers[0].life = 0;
            }
        }
        work->command.phaseTimer++;
        break;
    }
    case CGP_COMM_STEP_RESTART:
        CardCursorPassive(&work->cursor);
        work->command.phaseTimer = CGP_COMM_STEP_SORT_HAND;
        break;
    case CGP_COMM_STEP_ENEMY_REQUEST:
    {
        s32 index = CardEnemyCommandRequest(side, work);

        if (index < 0) {
            work->command.phaseTimer++;
            break;
        }
        ownSide->battle[0].cmdView.playedCard = ownSide->hand.cards[index];
        ownSide->hand.cards[index] = -1;
        CardPlayCleanHand(&ownSide->hand);
        work->command.step = 0;
        work->command.phaseTimer = CGP_COMM_STEP_OFFER;
        break;
    }
    case CGP_COMM_STEP_ENEMY_OPERATION:
        if (CardEnemyOperationPlay(side, work) != 0) {
            work->command.phaseTimer = CGP_COMM_STEP_END_PHASE;
        }
        break;
    case CGP_COMM_STEP_CONDITION:
    {
        s32 error = CardPlayChkCondition(work->definitions, ownSide, mas_15.cardId, work);

        if (error == 0) {
            error = CardPlayCommandCondition(side, mas_15.cardId, work);
        }
        if (error != 0) {
            CGPSetErrorMess(side, work, error);
            xglSoundEffectNormalID(SE_BUZZER, 0);
            work->command.phaseTimer = CGP_COMM_STEP_SORT_HAND;
        } else {
            xglSoundEffectNormalID(SE_DECIDE, 0);
            work->command.step = 0;
            work->command.phaseTimer++;
            p1no_17 = p2no_18 = 0;
        }
        break;
    }
    case CGP_COMM_STEP_DISCARD:
        CGPDispErrorMess(side, CGP_MESS_CONFIRM_CARD);
        if (pressed & PAD_CROSS) {
            xglSoundEffectNormalID(SE_CANCEL, 0);
            work->command.phaseTimer = CGP_COMM_STEP_PICK;
            break;
        }
        if (pressed & PAD_CIRCLE) {
            ownSide->battle[0].cmdView.playedCard = ownSide->hand.cards[mas_15.index];
            ownSide->hand.cards[mas_15.index] = -1;
            CardPlayCleanHand(&ownSide->hand);
            work->command.step = 0;
            work->command.phaseTimer++;
        }
        break;
    case CGP_COMM_STEP_OFFER:
        if (cpuFoe != 0) {
            if (CardEnemy1stAnswer(foeIndex, work) != 0) {
                work->command.phaseTimer = CGP_COMM_STEP_ADVANCE;
            } else {
                work->command.phaseTimer++;
            }
            break;
        }
        CGPDispErrorMessPlus(foeIndex, CGP_MESS_ANSWER_OFFER, ownSide->battle[0].cmdView.playedCard);
        if (pressedFoe & PAD_CROSS) {
            xglSoundEffectNormalID(SE_CANCEL, 0);
            work->command.phaseTimer++;
        }
        if (pressedFoe & PAD_CIRCLE) {
            xglSoundEffectNormalID(SE_DECIDE, 0);
            work->command.phaseTimer = CGP_COMM_STEP_ADVANCE;
            work->cursor.selectionMode = 0;
        }
        break;
    case CGP_COMM_STEP_SHOW:
        switch (ownSide->battle[0].cmdView.playedCard) {
        case CARD_ID_CURE_ALL: case CARD_ID_CURRY: case CARD_ID_BIO_SPHERE:
        case CARD_ID_SUPPLY_SHIP: case CARD_ID_SUPPLY_FLEET: case CARD_ID_RETREAT:
        case CARD_ID_INVITATION: case CARD_ID_STIM_DX: case CARD_ID_WAR_CASUALTY:
        case CARD_ID_START_UP: case CARD_ID_DESTINY:
            if (side == 0) {
                CGPSetCursor(work, CGP_AREA_PLAYER_BATTLE, 0);
            } else {
                CGPSetCursor(work, CGP_AREA_ENEMY_BATTLE, 0);
            }
            break;
        case CARD_ID_WOGLINDE: case CARD_ID_SURPRISE_ATTACK: case CARD_ID_FEDERATION_CRUISER:
        case CARD_ID_LANDING_OPERATIONS: case CARD_ID_PREMONITION: case CARD_ID_DRAGON_SKULL:
        case CARD_ID_EXPULSION_ORDER: case CARD_ID_NOTICE: case CARD_ID_CHARGE:
        case CARD_ID_PRECISE_SHOOTING: case CARD_ID_CONFUSION: case CARD_ID_BREAK_THROUGH:
            if (side == 0) {
                CGPSetCursor(work, CGP_AREA_ENEMY_BATTLE, 0);
            } else {
                CGPSetCursor(work, CGP_AREA_PLAYER_BATTLE, 0);
            }
            break;
        default:
            break;
        }
        work->command.phaseTimer++;
        work->cursor.selectionMode = 1;
        work->command.step = 0;
        xglMatrixUnit(matrix);
        if (side != 0) {
            xglMatrixRotZ(matrix, matrix, CGP_HALF_TURN);
        }
        position[0] = position[1] = 0.0f;
        position[2] = CGP_COMMAND_CARD_Z;
        position[3] = 1.0f;
        scale[0] = scale[1] = CGP_COMMAND_CARD_SCALE;
        scale[2] = scale[3] = 1.0f;
        xglMatrixTrans(matrix, matrix, position);
        xglMatrixScale(matrix, matrix, scale);
        CardDispCard(&matrix, ownSide->battle[0].cmdView.playedCard, 1);
        break;
    case CGP_COMM_STEP_PLAY:
    {
        s32 played;
        s32 i;

        if (cpuTurn != 0) {
            played = CardEnemyCommandPlay(side, work);
        } else {
            played = CardPlayCommandPlay(side, work->commandIds[ownSide->battle[0].cmdView.playedCard], work, 0, 0);
        }
        if (played != 0) {
            s32 cost = work->definitions[ownSide->battle[0].cmdView.playedCard].reserveCost;

            for (i = 0; i < cost; i++) {
                CardPlayCostCard(ownSide);
            }
            CardCursorPassive(&work->cursor);
            CGPSetPermanentMess(work, 0);
            ownSide->junk[CardPlayJunkCnt(ownSide)] = ownSide->battle[0].cmdView.playedCard;
            ownSide->battle[0].cmdView.playedCard = -1;
            CardPlayCleanHand(&ownSide->hand);
            CardPlayCleanJunk(ownSide);
            work->command.phaseTimer++;
            break;
        }
        {
            float halfTurn = CGP_HALF_TURN;

            xglMatrixUnit(backMatrix);
            xglMatrixRotZ(backMatrix, backMatrix, -work->rotStageVector[2] / CGP_DEGREES_HALF_TURN * halfTurn);
            if (side != 0) {
                xglMatrixRotZ(backMatrix, backMatrix, halfTurn);
            }
            backPosition[0] = backPosition[1] = 0.0f;
            backPosition[2] = CGP_COMMAND_CARD_Z;
            backPosition[3] = 1.0f;
            backScale[0] = backScale[1] = CGP_COMMAND_CARD_SCALE;
            backScale[2] = backScale[3] = 1.0f;
            xglMatrixTrans(backMatrix, backMatrix, backPosition);
            xglMatrixScale(backMatrix, backMatrix, backScale);
            CardDispCard(&backMatrix, ownSide->battle[0].cmdView.playedCard, 1);
            nmlModelSetLight(&CLColor, &asDir);
            nmlModelSetPlace(&backMatrix);
            nmlModelSetTexture(work->cursorXtx[2]);
            nmlModelEntryCard(CGP_COMMAND_BACK_MODEL);
        }
        break;
    }
    case CGP_COMM_STEP_AFTER_PLAY:
        work->command.phaseTimer = CGP_COMM_STEP_PICK;
        if (cpuTurn != 0) {
            work->command.phaseTimer = CGP_COMM_STEP_ENEMY_REQUEST;
        }
        break;
    case CGP_COMM_STEP_ADVANCE:
        work->command.phaseTimer++;
        break;
    case CGP_COMM_STEP_FOE_ANSWER:
    {
        s32 value;
        s32 i;

        if (cpuFoe != 0) {
            if (CardEnemyAnswer(foeIndex, work) != 0) {
                work->command.phaseTimer++;
            } else {
                work->command.phaseTimer = CGP_COMM_STEP_JUNK_ANSWERS;
            }
            break;
        }
        value = CardPlayHandCnt(&foeSide->hand);
        if (work->rotStageId == 0) {
            if (pressedFoe & PAD_RIGHT) {
                if (p2no_18 < value - 1) {
                    p2no_18++;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                }
            } else if (pressedFoe & PAD_LEFT) {
                if (p2no_18 > 0) {
                    p2no_18--;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                }
            }
        } else {
            if (pressedFoe & PAD_LEFT) {
                if (p2no_18 < value - 1) {
                    p2no_18++;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                }
            } else if (pressedFoe & PAD_RIGHT) {
                if (p2no_18 > 0) {
                    p2no_18--;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                }
            }
        }
        CGPCFSDispSub(side, foeIndex, work, ownSide->battle[0].cmdView.playedCard, p2no_18);
        if (CGP_HAND_FLIPPED(foeIndex, work)) {
            CGCPHandDispSub3(work->definitions, foeSide, p2no_18, 2, 1);
        } else {
            CGCPHandDispSub2(work->definitions, foeSide, p2no_18, 2);
        }
        if (pressedFoe & PAD_CIRCLE) {
            s32 card = foeSide->hand.cards[p2no_18];

            if (card != CARD_ID_MIYUKIS_EMAIL && card != CARD_ID_SONG) {
                xglSoundEffectNormalID(SE_BUZZER, 0);
                return;
            }
            value = CardPlayChkCondition(work->definitions, foeSide, foeSide->hand.cards[p2no_18], work);
            if (value != 0) {
                CGPSetErrorMess(foeIndex, work, value);
                xglSoundEffectNormalID(SE_BUZZER, 0);
                return;
            }
            value = work->definitions[foeSide->hand.cards[p2no_18]].reserveCost;
            for (i = 0; i < value; i++) {
                CardPlayCostCard(foeSide);
            }
            for (i = 0; i < CGP_ANSWER_MAX; i++) {
                if (foeSide->hand.cards[CGP_ANSWER_OFFSET + i] < 0) {
                    break;
                }
            }
            foeSide->hand.cards[CGP_ANSWER_OFFSET + i] = foeSide->hand.cards[p2no_18];
            foeSide->hand.cards[p2no_18] = -1;
            CardPlayCleanHand(&foeSide->hand);
            xglSoundEffectNormalID(SE_DECIDE, 0);
            work->command.phaseTimer++;
            break;
        }
        if (pressedFoe & PAD_CROSS) {
            xglSoundEffectNormalID(SE_CANCEL, 0);
            work->command.phaseTimer = CGP_COMM_STEP_JUNK_ANSWERS;
        }
        break;
    }
    case CGP_COMM_STEP_OWN_ANSWER:
    {
        s32 value;
        s32 i;

        if (cpuTurn != 0) {
            if (CardEnemyAnswer(side, work) != 0) {
                work->command.phaseTimer = (s16) work->command.phaseTimer - 1;
            } else {
                work->command.phaseTimer = CGP_COMM_STEP_FINISH;
            }
            break;
        }
        value = CardPlayHandCnt(&ownSide->hand);
        if (work->rotStageId == 0) {
            if (pressed & PAD_RIGHT) {
                if (p1no_17 < value - 1) {
                    p1no_17++;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                }
            } else if (pressed & PAD_LEFT) {
                if (p1no_17 > 0) {
                    p1no_17--;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                }
            }
        } else {
            if (pressed & PAD_LEFT) {
                if (p1no_17 < value - 1) {
                    p1no_17++;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                }
            } else if (pressed & PAD_RIGHT) {
                if (p1no_17 > 0) {
                    p1no_17--;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                }
            }
        }
        CGPCFSDispSub(side, side, work, ownSide->battle[0].cmdView.playedCard, p1no_17);
        if (CGP_HAND_FLIPPED(side, work)) {
            CGCPHandDispSub3(work->definitions, ownSide, p1no_17, 2, 1);
        } else {
            CGCPHandDispSub2(work->definitions, ownSide, p1no_17, 2);
        }
        if (pressed & PAD_CIRCLE) {
            s32 card = ownSide->hand.cards[p1no_17];

            if (card != CARD_ID_MIYUKIS_EMAIL && card != CARD_ID_SONG) {
                xglSoundEffectNormalID(SE_BUZZER, 0);
                return;
            }
            value = CardPlayChkCondition(work->definitions, ownSide, ownSide->hand.cards[p1no_17], work);
            if (value != 0) {
                CGPSetErrorMess(side, work, value);
                xglSoundEffectNormalID(SE_BUZZER, 0);
                return;
            }
            value = work->definitions[ownSide->hand.cards[p1no_17]].reserveCost;
            for (i = 0; i < value; i++) {
                CardPlayCostCard(ownSide);
            }
            for (i = 0; i < CGP_ANSWER_MAX; i++) {
                if (ownSide->hand.cards[CGP_ANSWER_OFFSET + i] < 0) {
                    break;
                }
            }
            ownSide->hand.cards[CGP_ANSWER_OFFSET + i] = ownSide->hand.cards[p1no_17];
            ownSide->hand.cards[p1no_17] = -1;
            CardPlayCleanHand(&ownSide->hand);
            xglSoundEffectNormalID(SE_DECIDE, 0);
            work->command.phaseTimer = (s16) work->command.phaseTimer - 1;
            break;
        }
        if (pressed & PAD_CROSS) {
            xglSoundEffectNormalID(SE_CANCEL, 0);
            work->command.phaseTimer = CGP_COMM_STEP_FINISH;
        }
        break;
    }
    case CGP_COMM_STEP_JUNK_ANSWERS:
    {
        s32 i;
        s32 junk = CardPlayJunkCnt(ownSide);

        for (i = 0; i < CGP_ANSWER_MAX; i++) {
            if (ownSide->hand.cards[CGP_ANSWER_OFFSET + i] >= 0) {
                ownSide->junk[junk++] = ownSide->hand.cards[CGP_ANSWER_OFFSET + i];
                ownSide->hand.cards[CGP_ANSWER_OFFSET + i] = -1;
            }
        }
        junk = CardPlayJunkCnt(foeSide);
        for (i = 0; i < CGP_ANSWER_MAX; i++) {
            if (foeSide->hand.cards[CGP_ANSWER_OFFSET + i] >= 0) {
                foeSide->junk[junk++] = foeSide->hand.cards[CGP_ANSWER_OFFSET + i];
                foeSide->hand.cards[CGP_ANSWER_OFFSET + i] = -1;
            }
        }
        work->command.phaseTimer++;
        break;
    }
    case CGP_COMM_STEP_ANSWER_NOTE:
        if (cpuTurn != 0) {
            work->command.phaseTimer = CGP_COMM_STEP_SHOW;
            work->cursor.selectionMode = 1;
            break;
        }
        CGPDispErrorMessPlus(side, CGP_MESS_ANSWER_NOTE, ownSide->battle[0].cmdView.playedCard);
        if (pressed & CGP_PAD_DISMISS) {
            xglSoundEffectNormalID(SE_DECIDE, 0);
            work->command.phaseTimer = CGP_COMM_STEP_SHOW;
        }
        break;
    case CGP_COMM_STEP_FINISH:
    {
        s32 i;
        s32 junk = CardPlayJunkCnt(ownSide);

        for (i = 0; i < CGP_ANSWER_MAX; i++) {
            if (ownSide->hand.cards[CGP_ANSWER_OFFSET + i] >= 0) {
                ownSide->junk[junk++] = ownSide->hand.cards[CGP_ANSWER_OFFSET + i];
                ownSide->hand.cards[CGP_ANSWER_OFFSET + i] = -1;
            }
        }
        {
            s32 cost = work->definitions[ownSide->battle[0].cmdView.playedCard].reserveCost;

            for (i = 0; i < cost; i++) {
                CardPlayCostCard(ownSide);
            }
        }
        ownSide->junk[junk] = ownSide->battle[0].cmdView.playedCard;
        ownSide->battle[0].cmdView.playedCard = -1;
        junk = CardPlayJunkCnt(foeSide);
        for (i = 0; i < CGP_ANSWER_MAX; i++) {
            if (foeSide->hand.cards[CGP_ANSWER_OFFSET + i] >= 0) {
                foeSide->junk[junk++] = foeSide->hand.cards[CGP_ANSWER_OFFSET + i];
                foeSide->hand.cards[CGP_ANSWER_OFFSET + i] = -1;
            }
        }
        work->command.phaseTimer = CGP_COMM_STEP_ENEMY_REQUEST;
        if (cpuTurn == 0) {
            CGPSetErrorMess(side, work, CGP_ERR_ANSWERS_DONE);
            work->command.phaseTimer = CGP_COMM_STEP_PICK;
        }
        work->cursor.selectionMode = 1;
        break;
    }
    case CGP_COMM_STEP_END_PHASE:
        switch (work->command.phase) {
        case CARD_PHASE_BLOCK:
            CGPCommFaseEndSub(0, &work->player, work);
            break;
        case CARD_PHASE_BLOCK + CARD_PHASES_PER_SIDE:
            CGPCommFaseEndSub(1, &work->enemy, work);
            break;
        }
        work->command.phaseTimer++;
        break;
    case CGP_COMM_STEP_NEXT_TURN:
        CGPNextTurnSub(work);
        break;
    }
}







/*
 * End of a side's turn: clears the per-turn marks of the own battle and disposal
 * slots, counts down the command timers, expires the operation cards of both
 * sides (duration exhausted: the card moves to the junk pile), applies the
 * recovery and regeneration operation cards and the regenerating cards on the
 * own slots, and answers a foe Virgil on the battle row with its trigger command.
 */
void CGPEndFaseSub(s32 side, CardPlaySide *ownSide, CardPlaySide *foeSide,
                   CardGameWork *work)
{
    s32 otherSide;
    s32 found;
    s32 count;
    s32 i;
    s32 j;
    s32 foeCard;
    s32 disposalCard;
    s32 battleCard;

    otherSide = (side == 0);
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        ownSide->battle[i].battle.flags &= ~CARD_STACK_FLAG_FROZEN;
        ownSide->battle[i].battle.layers[0].powerPlus = 0;
        ownSide->battle[i].battle.layers[0].flag = 0;
        ownSide->battle[i].battle.layers[0].boostCount = 0;
    }
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        ownSide->disposal[i].battle.flags &= ~CARD_STACK_FLAG_FROZEN;
        ownSide->disposal[i].battle.layers[0].powerPlus = 0;
        ownSide->disposal[i].battle.layers[0].flag = 0;
        ownSide->disposal[i].battle.layers[0].boostCount = 0;
    }
    if (work->command.turnCountdown != 0) {
        work->command.turnCountdown--;
    }
    if (work->command.recoveryApplied != 0) {
        work->command.recoveryApplied--;
    }
    for (i = 0; i < CARD_OPERATION_SLOTS; i++) {
        if (ownSide->operations[i] >= 0) {
            if (ownSide->operationDuration[i] < CGP_OPE_PERMANENT_DURATION) {
                if (ownSide->operations[i] == CARD_ID_COLLECTIVE) {
                    found = 0;
                    for (j = 0; j < CARD_BOARD_SLOTS; j++) {
                        if (ownSide->disposal[j].battle.layers[0].cardId >= 0 &&
                            (work->definitions[ownSide->disposal[j].battle.layers[0].cardId].flags &
                             CARD_ATTR_GNOSIS)) {
                            found = 1;
                            break;
                        }
                    }
                    for (j = 0; j < CARD_BOARD_SLOTS; j++) {
                        if (ownSide->battle[j].battle.layers[0].cardId >= 0 &&
                            (work->definitions[ownSide->battle[j].battle.layers[0].cardId].flags &
                             CARD_ATTR_GNOSIS)) {
                            found = 1;
                            break;
                        }
                    }
                    if (found) {
                        ownSide->operationDuration[i] = work->definitions[CARD_ID_COLLECTIVE].operationDuration;
                    } else {
                        ownSide->operationDuration[i]--;
                    }
                } else {
                    ownSide->operationDuration[i]--;
                }
            }
            if (ownSide->operationDuration[i] <= 0) {
                ownSide->junk[CardPlayJunkCnt(ownSide)] = ownSide->operations[i];
                ownSide->operations[i] = -1;
            }
        }
        if (foeSide->operations[i] >= 0) {
            if (foeSide->operationDuration[i] < CGP_OPE_PERMANENT_DURATION) {
                /* Tests the own side's operation card, not the foe's. */
                if (ownSide->operations[i] == CARD_ID_COLLECTIVE) {
                    found = 0;
                    for (j = 0; j < CARD_BOARD_SLOTS; j++) {
                        if (foeSide->disposal[j].battle.layers[0].cardId >= 0 &&
                            (work->definitions[foeSide->disposal[j].battle.layers[0].cardId].flags &
                             CARD_ATTR_GNOSIS)) {
                            found = 1;
                            break;
                        }
                    }
                    for (j = 0; j < CARD_BOARD_SLOTS; j++) {
                        if (foeSide->battle[j].battle.layers[0].cardId >= 0 &&
                            (work->definitions[foeSide->battle[j].battle.layers[0].cardId].flags &
                             CARD_ATTR_GNOSIS)) {
                            found = 1;
                            break;
                        }
                    }
                    if (found) {
                        foeSide->operationDuration[i] = work->definitions[CARD_ID_COLLECTIVE].operationDuration;
                    } else {
                        foeSide->operationDuration[i]--;
                    }
                } else {
                    foeSide->operationDuration[i]--;
                }
            }
            if (foeSide->operationDuration[i] <= 0) {
                foeSide->junk[CardPlayJunkCnt(foeSide)] = foeSide->operations[i];
                foeSide->operations[i] = -1;
            }
        }
    }
    i = CardPlayCntOpe(ownSide, CARD_ID_DAMMERUNG);
    if (i != 0) {
        while (i > 0) {
            CardPlayRecavery(ownSide);
            i--;
        }
    }
    count = CardPlayCntOpe(ownSide, CARD_ID_PILGRIMAGE_FLEET) +
            CardPlayCntOpe(foeSide, CARD_ID_PILGRIMAGE_FLEET);
    if (count != 0) {
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (ownSide->disposal[i].battle.layers[0].cardId >= 0 &&
                (work->definitions[ownSide->disposal[i].battle.layers[0].cardId].flags &
                 CARD_ATTR_GNOSIS)) {
                ownSide->disposal[i].battle.layers[0].life++;
                if (work->definitions[ownSide->disposal[i].battle.layers[0].cardId].operationDuration <
                    ownSide->disposal[i].battle.layers[0].life) {
                    ownSide->disposal[i].battle.layers[0].life =
                        work->definitions[ownSide->disposal[i].battle.layers[0].cardId].operationDuration;
                }
                CardSetEffect(side, work, CGP_EFFECT_SWIRL, 0, 0, ownSide->disposal[i].battle.position, 0);
                CardSetEffect(side, work, CGP_EFFECT_NUMBER, 1, CGP_SE_REGEN, ownSide->disposal[i].battle.position,
                              &ownSide->disposal[i].battle);
            }
        }
    }
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        disposalCard = ownSide->disposal[i].battle.layers[0].cardId;
        if (disposalCard >= 0) {
            switch ((s16) disposalCard) {
            case CARD_ID_LV_1_CHAOS:
                ownSide->disposal[i].battle.layers[0].life += 2;
                if (work->definitions[CARD_ID_LV_1_CHAOS].operationDuration < ownSide->disposal[i].battle.layers[0].life) {
                    ownSide->disposal[i].battle.layers[0].life = work->definitions[CARD_ID_LV_1_CHAOS].operationDuration;
                }
                CardSetEffect(side, work, CGP_EFFECT_SWIRL, 0, 0, ownSide->disposal[i].battle.position, 0);
                CardSetEffect(side, work, CGP_EFFECT_NUMBER, 2, CGP_SE_REGEN, ownSide->disposal[i].battle.position,
                              &ownSide->disposal[i].battle);
                break;
            case CARD_ID_LV_10_CHAOS:
                ownSide->disposal[i].battle.layers[0].life += 3;
                if (work->definitions[CARD_ID_LV_10_CHAOS].operationDuration < ownSide->disposal[i].battle.layers[0].life) {
                    ownSide->disposal[i].battle.layers[0].life = work->definitions[CARD_ID_LV_10_CHAOS].operationDuration;
                }
                CardSetEffect(side, work, CGP_EFFECT_SWIRL, 0, 0, ownSide->disposal[i].battle.position, 0);
                CardSetEffect(side, work, CGP_EFFECT_NUMBER, 3, CGP_SE_REGEN, ownSide->disposal[i].battle.position,
                              &ownSide->disposal[i].battle);
                break;
            }
            if (CardPlayCntOpe(ownSide, CARD_ID_KUKAI_FOUNDATION) &&
                !(work->definitions[disposalCard].flags & CARD_ATTR_GNOSIS)) {
                ownSide->disposal[i].battle.layers[0].life++;
                if (work->definitions[disposalCard].operationDuration <
                    ownSide->disposal[i].battle.layers[0].life) {
                    ownSide->disposal[i].battle.layers[0].life =
                        work->definitions[disposalCard].operationDuration;
                }
                CardSetEffect(side, work, CGP_EFFECT_SWIRL, 0, 0, ownSide->disposal[i].battle.position, 0);
                CardSetEffect(side, work, CGP_EFFECT_NUMBER, 1, CGP_SE_REGEN, ownSide->disposal[i].battle.position,
                              &ownSide->disposal[i].battle);
            }
        }
    }
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        battleCard = ownSide->battle[i].battle.layers[0].cardId;
        if (battleCard >= 0) {
            switch ((s16) battleCard) {
            case CARD_ID_LV_1_CHAOS:
                ownSide->battle[i].battle.layers[0].life += 2;
                if (work->definitions[CARD_ID_LV_1_CHAOS].operationDuration < ownSide->battle[i].battle.layers[0].life) {
                    ownSide->battle[i].battle.layers[0].life = work->definitions[CARD_ID_LV_1_CHAOS].operationDuration;
                }
                CardSetEffect(side, work, CGP_EFFECT_SWIRL, 0, 0, ownSide->battle[i].battle.position, 0);
                CardSetEffect(side, work, CGP_EFFECT_NUMBER, 2, CGP_SE_REGEN, ownSide->battle[i].battle.position,
                              &ownSide->battle[i].battle);
                break;
            case CARD_ID_LV_10_CHAOS:
            case CARD_ID_UNICORN:
                ownSide->battle[i].battle.layers[0].life += 3;
                if (work->definitions[battleCard].operationDuration <
                    ownSide->battle[i].battle.layers[0].life) {
                    ownSide->battle[i].battle.layers[0].life =
                        work->definitions[battleCard].operationDuration;
                }
                CardSetEffect(side, work, CGP_EFFECT_SWIRL, 0, 0, ownSide->battle[i].battle.position, 0);
                CardSetEffect(side, work, CGP_EFFECT_NUMBER, 3, CGP_SE_REGEN, ownSide->battle[i].battle.position,
                              &ownSide->battle[i].battle);
                break;
            }
        }
        foeCard = foeSide->battle[i].battle.layers[0].cardId;
        if (foeCard >= 0) {
            if (foeCard == CARD_ID_VIRGIL) {
                CardPlayCommandPlay(otherSide, CARD_COMMAND_VIRGIL_TRIGGER, work, i, 0);
            }
        }
    }
}

/* Battle slot (0..3) of the side's battle row the phase is working on. */
static s32 i_19;
/* Battle slot of the other side's row that the support attacks land on (steps 41 and 42). */
static s32 j_20;
/* Frames the phase still waits before its next step. */
static u8 timer_21;
/* Cleared when the support step starts. */
static u8 firstattack_22;






/*
 * Battle phase of one side, one step per call (command.phaseTimer). After a
 * queued battle command the side's battle slots attack in the order of their
 * attack type: front attacks, piercing attacks, the foe's counter cards, the
 * support cards (which first count down their power, and attack each foe slot
 * once it reaches zero) and the direct attacks. Each attack waits
 * timer_21 frames before the next one.
 */
void CGPBattleFaseProc(s32 side, CardGameWork *work)
{
    CardPlaySide *ownSide;
    CardPlaySide *foeSide;
    s32 attackType;
    s32 phase;

    ownSide = &work->player;
    foeSide = &work->enemy;
    if (side != 0) {
        ownSide = &work->enemy;
        foeSide = &work->player;
    }
    phase = work->command.phaseTimer;
    switch ((s16) phase) {
    case CGP_BATTLE_STEP_BEGIN:
    {
        s32 ownCount;
        s32 foeCount;

        CardPlayClearEndflg(ownSide);
        ownCount = CardPlayBattleCnt(ownSide);
        foeCount = CardPlayCntBtl(foeSide, CARD_ID_LV_1_JR);
        foeCount += CardPlayCntBtl(foeSide, CARD_ID_LV_10_JR);
        if (ownCount == 0 && foeCount == 0) {
            work->command.phaseTimer = CGP_BATTLE_STEP_END;
            if ((side == 0 && (work->flags & CGP_FLAG_SIDE0_AUTO)) || (side != 0 && (work->flags & CGP_FLAG_SIDE1_AUTO))) {
                return;
            }
            if ((side == 0 && (work->save->flags & CGP_SAVE_FLAG_SIDE0_AUTO)) || (side != 0 && (work->save->flags & CGP_SAVE_FLAG_SIDE1_AUTO))) {
                return;
            }
            CGPSetErrorMess(side, work, CGP_ERR_NO_BATTLE);
            return;
        }
        if (work->command.recoveryApplied != 0) {
            work->command.phaseTimer = CGP_BATTLE_STEP_END;
            if ((side == 0 && (work->flags & CGP_FLAG_SIDE0_AUTO)) || (side != 0 && (work->flags & CGP_FLAG_SIDE1_AUTO))) {
                return;
            }
            CGPSetErrorMessPlus(side, work, CGP_ERR_BATTLE_BLOCKED, CARD_ID_INVITATION);
            return;
        }
        if (work->command.turnCountdown != 0 || CardPlayCntOpe(ownSide, CARD_ID_ARMISTICE) != 0 ||
            CardPlayCntOpe(foeSide, CARD_ID_ARMISTICE) != 0) {
            work->command.phaseTimer = CGP_BATTLE_STEP_END;
            CGPSetErrorMessPlus(side, work, CGP_ERR_BATTLE_BLOCKED, CARD_ID_ARMISTICE);
            return;
        }
        CGPRotStageSub(work, side);
        i_19 = 0;
        work->command.phaseTimer = CGP_BATTLE_STEP_COMMAND;
        return;
    }
    case CGP_BATTLE_STEP_COMMAND:
        if (CardPlayCommandCheck(work->command.phase,
                                 side == 0 ? &work->commandQueue[0][0][0] : &work->commandQueue[1][0][0])) {
            work->cursor.selectionMode = 1;
            CardPlayCommandBattleExecute(side, work->command.phase, work);
            work->cursor.selectionMode = 1;
        } else {
            work->command.slideDirection = 1;
            work->cursor.selectionMode = 0;
            work->command.phaseTimer = CGP_BATTLE_STEP_FRONT;
            work->command.slideTimer = CGP_BATTLE_SLIDE_FRAMES;
            work->command.step = 0;
        }
        CGPRefreshFieldHP(work);
        return;
    case CGP_BATTLE_STEP_END:
        if (CardPlayCommandCheck(work->command.phase,
                                 side == 0 ? &work->commandQueue[0][0][0] : &work->commandQueue[1][0][0])) {
            work->cursor.selectionMode = 1;
            CardPlayCommandBattleExecute(side, work->command.phase, work);
        } else {
            CGPNextTurnSub(work);
        }
        CGPRefreshFieldHP(work);
        return;
    case CGP_BATTLE_STEP_FRONT:
        if (CGPBattleSub(side, CGP_ATTACK_FRONT, i_19, work, ownSide, foeSide, 0)) {
            return;
        }
        attackType = CardChkAttackType(work->definitions, &ownSide->battle[i_19].battle);
        timer_21 = 0;
        if (attackType == CGP_ATTACK_FRONT) {
            timer_21 = CGP_BATTLE_WAIT_FRAMES;
        }
        i_19++;
        work->command.phaseTimer++;
        break;
    case CGP_BATTLE_STEP_FRONT_WAIT:
        if (timer_21 != 0) {
            timer_21--;
            break;
        }
        CGPRefreshFieldHP(work);
        CardCursorPassive(&work->cursor);
        work->command.step = 0;
        if (i_19 >= 4) {
            work->command.phaseTimer = CGP_BATTLE_STEP_PIERCE;
            i_19 = 0;
        } else {
            work->command.phaseTimer = (s16) work->command.phaseTimer - 1;
        }
        break;
    case CGP_BATTLE_STEP_PIERCE:
        if (CGPBattleSub(side, CGP_ATTACK_PIERCE, i_19, work, ownSide, foeSide, 0)) {
            return;
        }
        attackType = CardChkAttackType(work->definitions, &ownSide->battle[i_19].battle);
        timer_21 = 0;
        if (attackType == CGP_ATTACK_PIERCE) {
            timer_21 = CGP_BATTLE_WAIT_FRAMES;
        }
        work->command.phaseTimer++;
        break;
    case CGP_BATTLE_STEP_PIERCE_WAIT:
        if (timer_21 != 0) {
            timer_21--;
            break;
        }
        CGPRefreshFieldHP(work);
        CardCursorPassive(&work->cursor);
        work->command.step = 0;
        if (ownSide->battle[i_19].battle.layers[0].cardId == CARD_ID_GARGOYLE) {
            if (CardPlayCommandCheck(work->command.phase,
                                     side == 0 ? &work->commandQueue[0][0][0] : &work->commandQueue[1][0][0])) {
                CardPlayCommandBattleExecute(side, work->command.phase, work);
            }
            work->command.phaseTimer++;
            timer_21 = CGP_BATTLE_WAIT_FRAMES;
            i_19++;
        } else {
            i_19++;
            if (i_19 >= 4) {
                work->command.phaseTimer = CGP_BATTLE_STEP_COUNTER;
                i_19 = 0;
            } else {
                work->command.phaseTimer = (s16) work->command.phaseTimer - 1;
            }
        }
        break;
    case CGP_BATTLE_STEP_PIERCE_COMMAND:
        if (timer_21 != 0) {
            timer_21--;
        } else {
            CardCursorPassive(&work->cursor);
            work->command.step = 0;
            if (i_19 >= 4) {
                work->command.phaseTimer = CGP_BATTLE_STEP_COUNTER;
                i_19 = 0;
            } else {
                work->command.phaseTimer = CGP_BATTLE_STEP_PIERCE;
            }
        }
        CGPRefreshFieldHP(work);
        return;
    /* The foe's counter cards strike back at this side's slot. */
    case CGP_BATTLE_STEP_COUNTER:
    {
        s32 found;
        s32 i;

        found = 0;
        if (CardPlayCntBtl(foeSide, CARD_ID_LV_1_JR) != 0 ||
            CardPlayCntBtl(foeSide, CARD_ID_LV_10_JR) != 0) {
            for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                if (foeSide->battle[i].battle.layers[0].cardId == CARD_ID_LV_1_JR ||
                    foeSide->battle[i].battle.layers[0].cardId == CARD_ID_LV_10_JR) {
                    if (foeSide->battle[i].battle.layers[1].cardId < 0) {
                        i_19 = i;
                        found = 1;
                        work->command.phaseTimer++;
                    }
                }
            }
        }
        if (found) {
            return;
        }
        work->command.phaseTimer = CGP_BATTLE_STEP_SUPPORT_CHARGE;
        break;
    }
    case CGP_BATTLE_STEP_COUNTER_ATTACK:
        if (CGPBattleSub(side == 0, CGP_ATTACK_PIERCE, i_19, work, foeSide, ownSide, 0)) {
            return;
        }
        work->command.phaseTimer++;
        timer_21 = CGP_BATTLE_WAIT_FRAMES;
        break;
    case CGP_BATTLE_STEP_COUNTER_WAIT:
        if (timer_21 != 0) {
            timer_21--;
            break;
        }
        CGPRefreshFieldHP(work);
        CardCursorPassive(&work->cursor);
        i_19 = 0;
        j_20 = 0;
        work->command.step = 0;
        work->command.phaseTimer = CGP_BATTLE_STEP_SUPPORT_CHARGE;
        break;
    /* Support cards count their power down before they attack. */
    case CGP_BATTLE_STEP_SUPPORT_CHARGE:
        firstattack_22 = 0;
        if (ownSide->battle[i_19].battle.layers[0].cardId >= 0) {
            if (CardChkAttackType(work->definitions, &ownSide->battle[i_19].battle) == CGP_ATTACK_SUPPORT &&
                !(ownSide->battle[i_19].battle.flags & CARD_STACK_FLAG_FROZEN)) {
                s32 power;

                ownSide->battle[i_19].battle.power = ownSide->battle[i_19].battle.power - 1;
                power = CardPlayBattleCnt(foeSide);
                if (ownSide->battle[i_19].battle.power == 0 && power != 0) {
                    power = CGPCalcPower(work, &ownSide->battle[i_19].battle);
                    power += CGPCalcPowerPlus(work, &ownSide->battle[i_19].battle, power);
                    if (power >= CGP_SUPPORT_STRONG_POWER) {
                        timer_21 = CGP_BATTLE_SUPPORT_WAIT_FRAMES;
                        CardSetEffect(side, work, CGP_EFFECT_SUPPORT_STRONG, 0, CGP_SE_SUPPORT,
                                      ownSide->battle[i_19].battle.position, 0);
                    } else {
                        timer_21 = CGP_BATTLE_WAIT_FRAMES;
                        CardSetEffect(side, work, CGP_EFFECT_SUPPORT, 0, CGP_SE_SUPPORT,
                                      ownSide->battle[i_19].battle.position, 0);
                    }
                }
            }
        }
        phase = work->command.phaseTimer;
        work->command.phaseTimer = phase + 1;
        break;
    case CGP_BATTLE_STEP_SUPPORT:
        if (timer_21 != 0) {
            timer_21--;
            break;
        }
        if (CGPBattleSub(side, CGP_ATTACK_SUPPORT, i_19, work, ownSide, foeSide,
                         &foeSide->battle[j_20].battle)) {
            return;
        }
        attackType = CardChkAttackType(work->definitions, &ownSide->battle[i_19].battle);
        timer_21 = 0;
        if (attackType == CGP_ATTACK_SUPPORT) {
            if (foeSide->battle[j_20].battle.layers[0].cardId >= 0 &&
                ownSide->battle[i_19].battle.power == 0) {
                timer_21 = CGP_BATTLE_WAIT_FRAMES;
            }
        }
        work->command.phaseTimer++;
        j_20++;
        break;
    case CGP_BATTLE_STEP_SUPPORT_NEXT:
        if (timer_21 != 0) {
            timer_21--;
            break;
        }
        work->command.step = 0;
        if (j_20 >= 4) {
            CardCursorPassive(&work->cursor);
            if (ownSide->battle[i_19].battle.layers[0].cardId >= 0) {
                if (CardChkAttackType(work->definitions, &ownSide->battle[i_19].battle) == CGP_ATTACK_SUPPORT) {
                    if (ownSide->battle[i_19].battle.layers[1].cardId >= 0) {
                        if (ownSide->battle[i_19].battle.layers[1].life < CGP_SUPPORT_WEAR_LIMIT) {
                            if (!(ownSide->battle[i_19].battle.flags & CARD_STACK_FLAG_FROZEN)) {
                                ownSide->battle[i_19].battle.layers[1].life--;
                            }
                        }
                    }
                    if (ownSide->battle[i_19].battle.power <= 0) {
                        ownSide->battle[i_19].battle.power = ownSide->battle[i_19].battle.powerLimit;
                    }
                }
            }
            j_20 = 0;
            if (ownSide->battle[i_19].battle.layers[0].cardId == CARD_ID_SIMEON &&
                !(ownSide->battle[i_19].battle.flags & CARD_STACK_FLAG_FROZEN)) {
                CardPlayCommandPlay(side, CARD_COMMAND_GARGOYLE_TRIGGER, work, i_19, 0);
                work->command.phaseTimer++;
                timer_21 = CGP_BATTLE_WAIT_FRAMES;
            } else {
                i_19++;
                if (i_19 >= 4) {
                    work->command.phaseTimer = CGP_BATTLE_STEP_DIRECT;
                    i_19 = 0;
                } else {
                    work->command.phaseTimer = CGP_BATTLE_STEP_SUPPORT_CHARGE;
                }
            }
        } else {
            work->command.phaseTimer = phase - 1;
        }
        CGPRefreshFieldHP(work);
        return;
    case CGP_BATTLE_STEP_SUPPORT_COMMAND:
        if (CardPlayCommandCheck(work->command.phase,
                                 side == 0 ? &work->commandQueue[0][0][0] : &work->commandQueue[1][0][0])) {
            CardPlayCommandBattleExecute(side, work->command.phase, work);
            return;
        }
        if (timer_21 != 0) {
            timer_21--;
        } else {
            i_19++;
            if (i_19 >= 4) {
                work->command.phaseTimer = CGP_BATTLE_STEP_DIRECT;
                i_19 = 0;
            } else {
                work->command.phaseTimer = CGP_BATTLE_STEP_SUPPORT_CHARGE;
            }
        }
        CGPRefreshFieldHP(work);
        return;
    /* Direct attacks hit the other side without a target card. */
    case CGP_BATTLE_STEP_DIRECT:
        if (CGPBattleSub(side, CGP_ATTACK_DIRECT, i_19, work, ownSide, foeSide, 0)) {
            return;
        }
        attackType = CardChkAttackType(work->definitions, &ownSide->battle[i_19].battle);
        timer_21 = 0;
        if (attackType == CGP_ATTACK_DIRECT) {
            timer_21 = CGP_BATTLE_WAIT_FRAMES;
        }
        i_19++;
        work->command.phaseTimer++;
        break;
    case CGP_BATTLE_STEP_DIRECT_WAIT:
        if (timer_21 != 0) {
            timer_21--;
            break;
        }
        CardCursorPassive(&work->cursor);
        work->command.step = 0;
        if (i_19 >= 4) {
            i_19 = 0;
            work->command.phaseTimer++;
        } else {
            work->command.phaseTimer = (s16) work->command.phaseTimer - 1;
        }
        CGPRefreshFieldHP(work);
        return;
    case CGP_BATTLE_STEP_FINISH:
        work->command.slideTimer = CGP_BATTLE_SLIDE_FRAMES;
        work->command.phaseTimer = CGP_BATTLE_STEP_CLOSE;
        work->command.slideDirection = 0;
        if ((side == 0 && (work->flags & CGP_FLAG_SIDE0_AUTO)) || (side != 0 && (work->flags & CGP_FLAG_SIDE1_AUTO))) {
            return;
        }
        if ((side == 0 && (work->save->flags & CGP_SAVE_FLAG_SIDE0_AUTO)) || (side != 0 && (work->save->flags & CGP_SAVE_FLAG_SIDE1_AUTO))) {
            return;
        }
        CGPSetErrorMess(side, work, CGP_ERR_BATTLE_DONE);
        return;
    case CGP_BATTLE_STEP_CLOSE:
        CGPRefreshFieldHP(work);
        timer_21 = CGP_BATTLE_SLIDE_FRAMES;
        work->command.phaseTimer++;
        break;
    case CGP_BATTLE_STEP_CLOSE_WAIT:
        CGPRefreshFieldHP(work);
        if (timer_21 != 0) {
            timer_21--;
        } else {
            work->command.phaseTimer = CGP_BATTLE_STEP_END;
        }
        break;
    }
}





/* Hand index the discard selection stands on. */
static s32 Now_23;
/* Frames left before the discard selection reacts. */
static s32 Timer_24;

/*
 * Last phase of a turn for one side: runs the pending command queues, then steps
 * through the end of the turn. A hand holding too many cards opens a list window
 * (CGPCheckTeMax) from which the player picks the card to throw away: left and right
 * move the selection, triangle shows the card large and circle asks "Throw away
 * this card?"; circle there discards it and checks the hand again, cross goes back
 * to the selection. A hand within the limit ends the phase at once. The computer
 * player ends the phase itself (CardEnemyEnd).
 */
void CGPEndFaseProc(s32 side, CardGameWork *work, CardListWindow *window)
{
    CardPlaySide *ownSide;
    CardPlaySide *foeSide;
    u16 held;
    u16 repeat;
    s32 count;
    float position[4];
    Matrix4 matrix;

    ownSide = &work->player;
    foeSide = &work->enemy;
    held = work->command.input[0].held;
    repeat = work->command.input[0].repeat;
    if (side != 0) {
        ownSide = &work->enemy;
        foeSide = &work->player;
        held = work->command.input[1].held;
        repeat = work->command.input[1].repeat;
    }
    if (CardPlayCommandCheck(work->command.phase, work->commandQueue[0][0]) +
        CardPlayCommandCheck(work->command.phase, work->commandQueue[1][0]) != 0) {
        CardPlayCommandExecute(work->command.phase, work);
        return;
    }
    switch ((s16) work->command.phaseTimer) {
    case CGP_END_STEP_BEGIN:
        work->cursor.selectionMode = 0;
        work->command.phaseTimer++;
        /* fall through */
    case CGP_END_STEP_DRAW:
        work->command.phaseTimer++;
        CGPEndFaseSub(side, ownSide, foeSide, work);
        break;
    case CGP_END_STEP_HAND_CHECK:
        if (CGPCheckTeMax(window, &ownSide->hand) == 0) {
            work->command.phaseTimer = CGP_END_STEP_NEXT_TURN;
            break;
        }
        if (side == 0 ? (work->flags & CGP_FLAG_SIDE0_AUTO) : (work->flags & CGP_FLAG_SIDE1_AUTO)) {
            CardEnemyEnd(side, work);
            break;
        }
        xglSoundEffectNormalID(SE_BUZZER, 0);
        CGPSetInterruptMess(work, CGP_MESS_HAND_FULL);
        CGPRotStageSub(work, side);
        work->command.phaseTimer++;
        Now_23 = 0;
        Timer_24 = CGP_DISCARD_INPUT_DELAY;
        break;
    case CGP_END_STEP_SELECT:
        if (Timer_24 != 0) {
            Timer_24--;
            break;
        }
        {
            s32 flipped = side == 0 ? work->rotStageId != 0 : work->rotStageId == 0;

            CGCPHandDispSub(&ownSide->hand, Now_23, flipped);
        }
        count = CardPlayHandCnt(&ownSide->hand);
        if (repeat & PAD_RIGHT) {
            if (Now_23 < count - 1) {
                xglSoundEffectNormalID(SE_CURSOR, 0);
                Now_23++;
            }
        } else if (repeat & PAD_LEFT) {
            if (Now_23 > 0) {
                xglSoundEffectNormalID(SE_CURSOR, 0);
                Now_23--;
            }
        }
        if (held & PAD_TRIANGLE) {
            position[0] = 0.0f;
            position[1] = CGP_DISCARD_PREVIEW_Y;
            position[2] = CGP_DISCARD_PREVIEW_Z;
            position[3] = 1.0f;
            xglMatrixUnit(matrix);
            xglMatrixTrans(matrix, matrix, position);
            CardDisp(&matrix, ownSide->hand.cards[Now_23], 1, 5);
        }
        if (repeat & PAD_CIRCLE) {
            xglSoundEffectNormalID(SE_DECIDE, 0);
            work->command.phaseTimer++;
        }
        break;
    case CGP_END_STEP_CONFIRM:
    {
        s32 flipped = side == 0 ? work->rotStageId != 0 : work->rotStageId == 0;

        CGPDispErrorMess2(side, CGP_ERR_DISCARD_CONFIRM);
        CGCPHandDispSub(&ownSide->hand, Now_23, flipped);
        if (repeat & PAD_CIRCLE) {
            work->command.phaseTimer = CGP_END_STEP_HAND_CHECK;
            CardPlayDesertCard(ownSide, Now_23);
            xglSoundEffectNormalID(SE_DECIDE, 0);
        } else if (repeat & PAD_CROSS) {
            work->command.phaseTimer = (s16) work->command.phaseTimer - 1;
            xglSoundEffectNormalID(SE_CANCEL, 0);
        }
        break;
    }
    case CGP_END_STEP_NEXT_TURN:
        CGPNextTurnSub(work);
        break;
    }
}



/*
 * Matrix copies: each is one __asm__ statement of constrained lq/sq pairs through $2 (the same shape
 * as the sceVu0Copy copies in tu001 CardMainInit). "memory" orders each copy with the surrounding
 * accesses to the matrices.
 */

/*
 * Pad-flip loop: one offset register is added to two bases, work and work + 8. The offset starts at
 * 0x5AD0 and steps by 2 for k = 0..2, the halfword index of input[0].held + k (CardGameWork viewed as
 * u16 words); the enemy base work + 8 is the same view advanced by one CardCommandInput (4 halfwords).
 * Addressed through the structure members the compiler folds 0x5AD0 into each address and the shared
 * offset register is lost.
 */



/*
 * Per-frame driver of the card game for the current command phase. Mode 0 draws the message
 * panel; the boards are drawn in the other mode. Returns the result of the phase handler.
 */
s32 CardGameProc(s32 mode, CardListWindow *window, CardGameWork *work, s32 round)
{
    CardEffectMatrix matrix;
    float offset[4];
    float vec[4];
    CGPBoardArea place;
    CardEffectMatrix cursorMatrix;
    s32 result;
    s32 side;
    u16 pressed;
    u16 cursorPressed;
    s32 faceUp;
    s32 i;
    u16 cpuTurn;
    u16 cpuFoe;

    result = 0;
    switch (work->command.phase) {
    case CARD_PHASE_DRAW:
    case CARD_PHASE_MOVE:
    case CARD_PHASE_EVENT:
    case CARD_PHASE_SET:
    case CARD_PHASE_BATTLE:
    case CARD_PHASE_ADJUST:
    case CARD_PHASE_BLOCK + CARD_PHASES_PER_SIDE:
        side = 0;
        pressed = work->command.input[0].pressed;
        break;
    default:
        pressed = work->command.input[1].pressed;
        side = 1;
        break;
    }
    /* On a rotated stage the d-pad bits of both players' input words are flipped. */
    if (work->rotStageId != 0) {
        s32 mask = CARD_PAD_UP_PAIR;
        u16 *playerWords = (u16 *) work;
        u16 *enemyWords = playerWords + CARD_CMD_INPUT_WORDS;
        u16 turned;
        s32 k;
        s32 word;

        for (k = 0; k < 3; k++) {
            u16 *playerPad;
            u16 *enemyPad;

            word = CARD_PAD_INPUT0_WORD + k;
            playerPad = playerWords + word;
            enemyPad = enemyWords + word;
            CARD_PAD_FLIP(playerPad, mask, turned);
            CARD_PAD_FLIP(enemyPad, mask, turned);
        }
    }
    /* Mode 0 draws the message panel, its interrupt message and the error message. */
    if (mode == 0) {
        offset[0] = 0.0f;
        offset[1] = 1.95f;
        offset[2] = 0.1f;
        offset[3] = 1.0f;
        vec[0] = 1.0f;
        vec[1] = 0.85f;
        vec[2] = 1.0f;
        vec[3] = 1.0f;
        xglMatrixUnit(matrix.m);
        xglMatrixTrans(matrix.m, matrix.m, offset);
        xglMatrixScale(matrix.m, matrix.m, vec);
        nmlModelSetPlace(&matrix.m);
        if (side == 0) {
            nmlModelSetLight(&Player1Color, &asDir);
        } else {
            nmlModelSetLight(&Player2Color, &asDir);
        }
        nmlModelSetTexture(CGP_PANEL_TEXTURE);
        nmlModelEntryCard(CGP_GAME_MESS_MODEL);
        if (work->errorMessKind != 0) {
            u16 answer = work->command.input[0].pressed;

            if (work->errorMessSide != 0) {
                answer = work->command.input[1].pressed;
            }
            if (answer & CGP_PAD_FACE_BUTTONS) {
                xglSoundEffectNormalID(SE_DECIDE, 0);
                work->errorMessKind = 0;
                work->flags &= ~CGP_FLAG_ERROR_PLUS;
            }
        } else if (work->command.interruptMessId != 0) {
            if (work->command.interruptMessTimer != 0) {
                work->command.interruptMessTimer--;
            } else {
                work->command.interruptMessId = 0;
                work->command.interruptMessTimer = 0;
                work->interruptMess = 0;
            }
        } else {
            if ((u16) (work->command.flowRequest - CARD_FLOW_ENEMY_DECK_OUT) > CARD_FLOW_FINAL_ROUND - CARD_FLOW_ENEMY_DECK_OUT) {
                if (CardPlayYamaCnt(&work->enemy) == 0) {
                    work->command.step = 0;
                    work->command.flowRequest = CARD_FLOW_ENEMY_DECK_OUT;
                } else if (CardPlayYamaCnt(&work->player) == 0) {
                    work->command.step = 0;
                    work->command.flowRequest = CARD_FLOW_PLAYER_DECK_OUT;
                }
            }
            if (work->command.phase != CARD_PHASE_BATTLE && work->command.phase != CARD_PHASE_BATTLE + CARD_PHASES_PER_SIDE) {
                place.area = CGP_AREA_PLAYER_BATTLE;
                for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                    place.slot = i;
                    CGPHPCheckSub(work->definitions, &work->player, &work->player.battle[i].battle, &place);
                }
                place.area = CGP_AREA_PLAYER_DISPOSAL;
                for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                    place.slot = i;
                    CGPHPCheckSub(work->definitions, &work->player, &work->player.disposal[i].battle, &place);
                }
                place.area = CGP_AREA_ENEMY_BATTLE;
                for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                    place.slot = i;
                    CGPHPCheckSub(work->definitions, &work->enemy, &work->enemy.battle[i].battle, &place);
                }
                place.area = CGP_AREA_ENEMY_DISPOSAL;
                for (i = 0; i < CARD_BOARD_SLOTS; i++) {
                    place.slot = i;
                    CGPHPCheckSub(work->definitions, &work->enemy, &work->enemy.disposal[i].battle, &place);
                }
            }
            if (work->command.flowRequest != 0) {
                switch (work->command.flowRequest) {
                case CARD_FLOW_CONFIRM:
                    CGPDispErrorMess(-1, 0);
                    if (pressed & PAD_CIRCLE) {
                        CardPlaySide *turnSide = &work->player;
                        s32 blocked;
                        s32 slot;

                        xglSoundEffectNormalID(SE_DECIDE, 0);
                        switch (work->command.phase) {
                        case CARD_PHASE_MOVE + CARD_PHASES_PER_SIDE:
                            turnSide = &work->enemy;
                            /* fall through */
                        case CARD_PHASE_MOVE:
                            blocked = 0;
                            if (CardPlayCntOpe(&work->player, CARD_ID_COMPULSORY_SORTIE) != 0 || CardPlayCntOpe(&work->enemy, CARD_ID_COMPULSORY_SORTIE) != 0) {
                                if (CardPlayBattleCnt(turnSide) < CARD_BOARD_SLOTS && CardPlayDisposeCnt(turnSide) > 0) {
                                    CGPSetErrorMessPlus(1, work, CGP_ERR_CARDS_TO_PLAY, CARD_ID_COMPULSORY_SORTIE);
                                    xglSoundEffectNormalID(SE_BUZZER, 0);
                                    blocked = 1;
                                }
                            }
                            if (CardPlayCntOpe(&work->player, CARD_ID_DSSS) != 0 || CardPlayCntOpe(&work->enemy, CARD_ID_DSSS) != 0) {
                                if (CardPlayBattleCnt(turnSide) < CARD_BOARD_SLOTS && CardPlayDisposeCnt(turnSide) > 0) {
                                    blocked = 0;
                                    for (slot = 0; slot < CARD_BOARD_SLOTS; slot++) {
                                        if (turnSide->disposal[slot].battle.layers[0].cardId >= 0 &&
                                            (work->definitions[turnSide->disposal[slot].battle.layers[0].cardId].flags & CARD_ATTR_GNOSIS)) {
                                            CGPSetErrorMessPlus(1, work, CGP_ERR_CARDS_TO_PLAY, CARD_ID_DSSS);
                                            xglSoundEffectNormalID(SE_BUZZER, 0);
                                            blocked = 1;
                                            break;
                                        }
                                    }
                                }
                            }
                            if (CardPlayCntOpe(&work->player, CARD_ID_OVERRIDE_CODE) != 0 || CardPlayCntOpe(&work->enemy, CARD_ID_OVERRIDE_CODE) != 0) {
                                if (CardPlayBattleCnt(turnSide) < CARD_BOARD_SLOTS && CardPlayDisposeCnt(turnSide) > 0) {
                                    blocked = 0;
                                    for (slot = 0; slot < CARD_BOARD_SLOTS; slot++) {
                                        if (turnSide->disposal[slot].battle.layers[0].cardId >= 0 &&
                                            (work->definitions[turnSide->disposal[slot].battle.layers[0].cardId].flags & CARD_ATTR_REALIAN)) {
                                            CGPSetErrorMessPlus(1, work, CGP_ERR_CARDS_TO_PLAY, CARD_ID_OVERRIDE_CODE);
                                            xglSoundEffectNormalID(SE_BUZZER, 0);
                                            blocked = 1;
                                            break;
                                        }
                                    }
                                }
                            }
                            work->command.flowRequest = 0;
                            if (!blocked) {
                                CGPNextTurnSub(work);
                            }
                            break;
                        case CARD_PHASE_BLOCK:
                            CGPCommFaseEndSub(0, &work->player, work);
                            work->command.flowRequest = 0;
                            CGPNextTurnSub(work);
                            break;
                        case CARD_PHASE_BLOCK + CARD_PHASES_PER_SIDE:
                            CGPCommFaseEndSub(1, &work->enemy, work);
                            work->command.flowRequest = 0;
                            CGPNextTurnSub(work);
                            break;
                        default:
                            work->command.flowRequest = 0;
                            CGPNextTurnSub(work);
                            break;
                        }
                    } else if (pressed & PAD_CROSS) {
                        work->command.flowRequest = 0;
                        xglSoundEffectNormalID(SE_CANCEL, 0);
                    }
                    break;
                case CARD_FLOW_ENEMY_DECK_OUT:
                    if (work->fase == CARD_FASE_BATTLE_2P) {
                        CaedGameWinMode(CARD_WIN_OUTCOME_ENEMY_DECK_OUT_FASE9, work, round);
                    } else {
                        CaedGameWinMode(CARD_WIN_OUTCOME_ENEMY_DECK_OUT, work, round);
                    }
                    result = 1;
                    break;
                case CARD_FLOW_PLAYER_DECK_OUT:
                    if (work->fase == CARD_FASE_BATTLE_2P) {
                        CaedGameWinMode(CARD_WIN_OUTCOME_PLAYER_DECK_OUT_FASE9, work, round);
                    } else {
                        CaedGameWinMode(CARD_WIN_OUTCOME_PLAYER_DECK_OUT, work, round);
                    }
                    result = 2;
                    break;
                case CARD_FLOW_FINAL_ROUND:
                    CaedGameWinMode(CARD_WIN_OUTCOME_FINAL_ROUND, work, round);
                    break;
                case CARD_FLOW_START_MENU:
                {
                    s32 exitResult = NewCMPGameModeExit(work->menuPlayer, work, round);

                    if (exitResult < 0) {
                        work->command.flowRequest = 0;
                    } else if (exitResult != 0) {
                        if (CGPChkFaseLock(work)) {
                            work->command.flowRequest = CARD_FLOW_CONFIRM;
                            xglSoundEffectNormalID(SE_DECIDE, 0);
                        } else {
                            xglSoundEffectNormalID(SE_BUZZER, 0);
                        }
                    }
                    break;
                }
                }
            } else {
                if (pressed & CGP_PAD_SQUARE) {
                    if (CGPChkFaseLock(work)) {
                        work->command.flowRequest = CARD_FLOW_CONFIRM;
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                    } else {
                        xglSoundEffectNormalID(SE_BUZZER, 0);
                    }
                }
                if (work->command.input[0].pressed & CGP_PAD_START) {
                    work->command.flowRequest = CARD_FLOW_START_MENU;
                    work->menuPlayer = 0;
                    work->command.exitMenuSub = -1;
                    work->command.exitMenuItem = 0;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else if (work->command.input[1].pressed & CGP_PAD_START) {
                    work->command.flowRequest = CARD_FLOW_START_MENU;
                    work->menuPlayer = 1;
                    work->command.exitMenuSub = -1;
                    work->command.exitMenuItem = 0;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                }
                if (work->fadeOutFlag != 0) {
                    work->fadeOutFlag = 0;
                    CGPNextTurnSub(work);
                } else {
                    switch (work->command.phase) {
                    case CARD_PHASE_DRAW:
                        CGPDrawFaseProc(0, work, &work->player, &work->enemy);
                        break;
                    case CARD_PHASE_MOVE:
                        if (work->flags & CGP_FLAG_SIDE0_AUTO) {
                            CardEnemyMove(0, work);
                        } else {
                            CGPMoveFaseProc(0, work, &work->player);
                        }
                        break;
                    case CARD_PHASE_SET:
                        if (work->flags & CGP_FLAG_SIDE0_AUTO) {
                            CardEnemySet(0, work);
                        } else {
                            CGPSetFaseProc(0, work, &work->player);
                        }
                        break;
                    case CARD_PHASE_BLOCK:
                        if (work->flags & CGP_FLAG_SIDE0_AUTO) {
                            cpuFoe = 1;
                        } else {
                            cpuFoe = 0;
                        }
                        if (work->flags & CGP_FLAG_SIDE1_AUTO) {
                            cpuTurn = 1;
                        } else {
                            cpuTurn = 0;
                        }
                        CGPCommFaseSub(1, work, cpuTurn, cpuFoe);
                        break;
                    case CARD_PHASE_BATTLE:
                        CGPBattleFaseProc(0, work);
                        break;
                    case CARD_PHASE_ADJUST:
                        CGPEndFaseProc(0, work, window);
                        break;
                    case CARD_PHASE_DRAW + CARD_PHASES_PER_SIDE:
                        CGPDrawFaseProc(1, work, &work->enemy, &work->player);
                        break;
                    case CARD_PHASE_MOVE + CARD_PHASES_PER_SIDE:
                        if (work->flags & CGP_FLAG_SIDE1_AUTO) {
                            CardEnemyMove(1, work);
                        } else {
                            CGPMoveFaseProc(1, work, &work->enemy);
                        }
                        break;
                    case CARD_PHASE_SET + CARD_PHASES_PER_SIDE:
                        if (work->flags & CGP_FLAG_SIDE1_AUTO) {
                            CardEnemySet(1, work);
                        } else {
                            CGPSetFaseProc(1, work, &work->enemy);
                        }
                        break;
                    case CARD_PHASE_EVENT + CARD_PHASES_PER_SIDE:
                        if (work->flags & CGP_FLAG_SIDE0_AUTO) {
                            cpuFoe = 1;
                        } else {
                            cpuFoe = 0;
                        }
                        if (work->flags & CGP_FLAG_SIDE1_AUTO) {
                            cpuTurn = 1;
                        } else {
                            cpuTurn = 0;
                        }
                        CGPCommFaseSub(1, work, cpuTurn, cpuFoe);
                        break;
                    case CARD_PHASE_EVENT:
                    case CARD_PHASE_BLOCK + CARD_PHASES_PER_SIDE:
                        if (work->flags & CGP_FLAG_SIDE0_AUTO) {
                            cpuTurn = 1;
                        } else {
                            cpuTurn = 0;
                        }
                        if (work->flags & CGP_FLAG_SIDE1_AUTO) {
                            cpuFoe = 1;
                        } else {
                            cpuFoe = 0;
                        }
                        CGPCommFaseSub(0, work, cpuTurn, cpuFoe);
                        break;
                    case CARD_PHASE_BATTLE + CARD_PHASES_PER_SIDE:
                        CGPBattleFaseProc(1, work);
                        break;
                    case CARD_PHASE_ADJUST + CARD_PHASES_PER_SIDE:
                        CGPEndFaseProc(1, work, window);
                        break;
                    }
                }
            }
        }
    }
    xglMatrixUnit(matrix.m);
    switch (work->command.phase) {
    case CARD_PHASE_DRAW:
    case CARD_PHASE_MOVE:
    case CARD_PHASE_EVENT:
    case CARD_PHASE_SET:
    case CARD_PHASE_BATTLE:
    case CARD_PHASE_ADJUST:
    case CARD_PHASE_BLOCK + CARD_PHASES_PER_SIDE:
        cursorPressed = work->command.input[0].pressed;
        break;
    default:
        cursorPressed = work->command.input[1].pressed;
        break;
    }
    if (work->rotStageFlag != 0) {
        float dy = -2.0f;
        float dx = -0.09f;

        work->rotStageFlag--;
        if (work->rotStageActive == 0) {
            dy = 2.0f;
            dx = 0.09f;
        }
        work->rotStageVector[1] += dy;
        work->rotStageVector[0] += dx;
    }
    if (work->rotStageCooldown == 0) {
        if (cursorPressed & CGP_PAD_ROTATE_STAGE) {
            if (work->command.phase == CARD_PHASE_BATTLE || work->command.phase == CARD_PHASE_BATTLE + CARD_PHASES_PER_SIDE) {
                xglSoundEffectNormalID(SE_BUZZER, 0);
            } else {
                work->rotStageCooldown = CGP_ROT_STAGE_COOLDOWN;
                work->rotStageId = (work->rotStageId + 1) & 1;
                xglSoundEffectNormalID(SE_DECIDE, 0);
            }
        }
    } else {
        work->rotStageCooldown--;
        work->rotStageVector[2] += -9.0f;
    }
    vec[0] = vec[1] = 0.0f;
    vec[2] = work->rotStageVector[0];
    if (!(work->flags & CGP_FLAG_ERROR_PLUS) && work->command.commandIndex == 0) {
        vec[2] = work->rotStageVector[0] - work->command.slideOffset;
    }
    vec[3] = 1.0f;
    xglMatrixTrans(matrix.m, matrix.m, vec);
    xglMatrixRotX(matrix.m, matrix.m, work->rotStageVector[1] / 180.0f * PI_F);
    xglMatrixRotZ(matrix.m, matrix.m, work->rotStageVector[2] / 180.0f * PI_F);
    __asm__ __volatile__(
        "lq $2, 0(%1)\n"
        "sq $2, 0(%0)\n"
        "lq $2, 16(%1)\n"
        "sq $2, 16(%0)\n"
        "lq $2, 32(%1)\n"
        "sq $2, 32(%0)\n"
        "lq $2, 48(%1)\n"
        "sq $2, 48(%0)\n"
        :
        : "r" ((CardEffectMatrix *) work->models[4].matrix), "r" (&matrix)
        : "$2", "memory");
    __asm__ __volatile__(
        "lq $2, 0(%1)\n"
        "sq $2, 0(%0)\n"
        "lq $2, 16(%1)\n"
        "sq $2, 16(%0)\n"
        "lq $2, 32(%1)\n"
        "sq $2, 32(%0)\n"
        "lq $2, 48(%1)\n"
        "sq $2, 48(%0)\n"
        :
        : "r" ((CardEffectMatrix *) work->models[5].matrix), "r" (&matrix)
        : "$2", "memory");
    xglMatrixRotZ(work->models[15].matrix, matrix.m, -work->rotStageVector[2] / 180.0f * PI_F);
    __asm__ __volatile__(
        "lq $2, 0(%1)\n"
        "sq $2, 0(%0)\n"
        "lq $2, 16(%1)\n"
        "sq $2, 16(%0)\n"
        "lq $2, 32(%1)\n"
        "sq $2, 32(%0)\n"
        "lq $2, 48(%1)\n"
        "sq $2, 48(%0)\n"
        :
        : "r" (&cursorMatrix), "r" (&matrix)
        : "$2", "memory");
    faceUp = 0;
    if (!(work->flags & CGP_FLAG_HIDE_HANDS)) {
        if (work->flags & CGP_FLAG_SIDE0_AUTO) {
            faceUp = 1;
        } else {
            switch (work->command.phase) {
            case CARD_PHASE_BLOCK:
            case CARD_PHASE_DRAW + CARD_PHASES_PER_SIDE:
            case CARD_PHASE_MOVE + CARD_PHASES_PER_SIDE:
            case CARD_PHASE_EVENT + CARD_PHASES_PER_SIDE:
            case CARD_PHASE_SET + CARD_PHASES_PER_SIDE:
            case CARD_PHASE_BATTLE + CARD_PHASES_PER_SIDE:
            case CARD_PHASE_ADJUST + CARD_PHASES_PER_SIDE:
                faceUp = 1;
                break;
            case CARD_PHASE_EVENT:
            case CARD_PHASE_BLOCK + CARD_PHASES_PER_SIDE:
                if ((s16) work->command.phaseTimer >= 10) {
                    faceUp = 1;
                }
                break;
            }
        }
    }
    if (CardPlayCntOpe(&work->player, CARD_ID_ANALYZE) != 0 || CardPlayCntOpe(&work->enemy, CARD_ID_ANALYZE) != 0) {
        faceUp = 0;
    }
    CardPlayDispHand(0, faceUp, &work->player, matrix.m);
    CardPlayDispBattle(0, work, &work->player, matrix.m);
    CardPlayDispDispose(0, &work->player, matrix.m);
    CardPlayDispOperation(0, &work->player, matrix.m);
    CardPlayDispYama(0, &work->player, matrix.m);
    CardPlayDispSute(0, &work->player, matrix.m);
    CardPlayDispJunk(0, &work->player, matrix.m);
    CardDispEffect(0, work, &matrix);
    xglMatrixRotZ(matrix.m, matrix.m, PI_F);
    __asm__ __volatile__(
        "lq $2, 0(%1)\n"
        "sq $2, 0(%0)\n"
        "lq $2, 16(%1)\n"
        "sq $2, 16(%0)\n"
        "lq $2, 32(%1)\n"
        "sq $2, 32(%0)\n"
        "lq $2, 48(%1)\n"
        "sq $2, 48(%0)\n"
        :
        : "r" ((CardEffectMatrix *) work->models[6].matrix), "r" (&matrix)
        : "$2", "memory");
    __asm__ __volatile__(
        "lq $2, 0(%1)\n"
        "sq $2, 0(%0)\n"
        "lq $2, 16(%1)\n"
        "sq $2, 16(%0)\n"
        "lq $2, 32(%1)\n"
        "sq $2, 32(%0)\n"
        "lq $2, 48(%1)\n"
        "sq $2, 48(%0)\n"
        :
        : "r" ((CardEffectMatrix *) work->models[7].matrix), "r" (&matrix)
        : "$2", "memory");
    faceUp = 0;
    if (!(work->flags & CGP_FLAG_HIDE_HANDS)) {
        if (work->flags & CGP_FLAG_SIDE1_AUTO) {
            faceUp = 1;
        } else {
            switch (work->command.phase) {
            case CARD_PHASE_DRAW:
            case CARD_PHASE_MOVE:
            case CARD_PHASE_EVENT:
            case CARD_PHASE_SET:
            case CARD_PHASE_BATTLE:
            case CARD_PHASE_ADJUST:
            case CARD_PHASE_BLOCK + CARD_PHASES_PER_SIDE:
                faceUp = 1;
                break;
            case CARD_PHASE_BLOCK:
            case CARD_PHASE_EVENT + CARD_PHASES_PER_SIDE:
                if ((s16) work->command.phaseTimer >= 10) {
                    faceUp = 1;
                }
                break;
            }
        }
    }
    if (CardPlayCntOpe(&work->player, CARD_ID_ANALYZE) != 0 || CardPlayCntOpe(&work->enemy, CARD_ID_ANALYZE) != 0) {
        faceUp = 0;
    }
    CardPlayDispHand(1, faceUp, &work->enemy, matrix.m);
    CardPlayDispBattle(1, work, &work->enemy, matrix.m);
    CardPlayDispDispose(1, &work->enemy, matrix.m);
    CardPlayDispOperation(1, &work->enemy, matrix.m);
    CardPlayDispYama(1, &work->enemy, matrix.m);
    CardPlayDispSute(1, &work->enemy, matrix.m);
    CardPlayDispJunk(1, &work->enemy, matrix.m);
    CardDispEffect(1, work, &matrix);
    CardPlayDispCursor(mode, work, cursorMatrix.m);
    CardPlayDispInfo(work);
    return result;
}



char PermMess000Txt[8] = "\241\241\241\241";

char PermMess001Txt[40] = "\013\031\003\r\000 Please select a Lv10 Shion.";

char PermMess002Txt[56] = "\013\031\003\r\000 Please select a card to be returned to the hand.";

char PermMess003Txt[56] = "\013\031\003\r\000 Please select a card to be added to the hand.";

char PermMess004Txt[56] = "\013\031\003\r\000 Please select a card to discard from the hand.";

char PermMess005Txt[56] = "\013\031\003\r\000 Please select a card to be returned to the hand.";

char PermMess006Txt[48] = "\013\031\003\r\000 Please select a card to be replaced.";

char PermMess007Txt[48] = "\013\031\003\r\000 Please select a card to recover its HP.";

char PermMess008Txt[48] = "\013\031\003\r\000 Please select a card to double its ATK.";

char PermMess009Txt[40] = "\013\031\003\r\000 Please select a card to attack.";

char PermMess010Txt[40] = "\013\031\003\r\000 Please select a card to down.";

char PermMess011Txt[56] = "\013\031\003\r\000 Please select a card to remove down status.";

char PermMess012Txt[48] = "\013\031\003\r\000 Please select a card to be discarded.";

char PermMess013Txt[56] = "\013\031\003\r\000 Please select a weapon card to be discarded.";

char PermMess014Txt[64] = "\013\031\003\r\000 Please select a card to return to the standby area.";

static char OLMess0000Txt[8] = "\241\241\241\241";

static char OLMess0010Txt[32] = "\013\031\003\r\000 \243\261P will draw a card.";

static char OLMess0011Txt[48] = "\013\031\003\r\000 \243\261P can exchange a card in their hand.";

static char OLMess0020Txt[56] = "\013\031\003\r\000 \243\261P can move the battle cards on the field.";

static char OLMess0030Txt[48] = "\013\031\003\r\000 \243\261P can use a battle or situation card.";

static char OLMess0040Txt[56] = "\013\031\003\r\000 \243\261P can use an event card or the card text.";

static char OLMess0041Txt[40] = "\013\031\003\r\000 \243\261P can use an event card.";

static char OLMess0042Txt[40] = "\013\031\003\r\000 \243\262P can use an event card.";

static char OLMess0050Txt[32] = "\013\031\003\r\000 Engaging in battle.";

static char OLMess0060Txt[32] = "\013\031\003\r\000 \243\262P will draw a card.";

static char OLMess0061Txt[48] = "\013\031\003\r\000 \243\262P can exchange a card in their hand.";

static char OLMess0070Txt[56] = "\013\031\003\r\000 \243\262P can move the battle cards on the field.";

static char OLMess0080Txt[48] = "\013\031\003\r\000 \243\262P can use a battle or situation card.";

static char OLMess0090Txt[56] = "\013\031\003\r\000 \243\262P can use an event card or the card text.";


char EMTitle00Txt[24] = "\031\003\014\200@@Phase Error";

char EMTitle01Txt[24] = "\031\003\014\200@@Placement Error";

char EMTitle02Txt[24] = "\031\003\014\200@@Move Error";

char EMTitle03Txt[24] = "\031\003\014\200@@Command Error";

char EMTitle04Txt[32] = "\031\003\014\200@@Placement/Usage Error";

char EMTitle90Txt[24] = "\031\003\014@@\200End of Phase";

char EMTitle91Txt[24] = "\031\003\014@@\200Event Block";

char EMTitle92Txt[24] = "\031\003\014@@\200Use Card Effect";

char EMTitle93Txt[24] = "\031\003\014@@\200Select Card";

char EMTitle94Txt[24] = "\031\003\014@@\200Exchange Cards";

char EMTitle95Txt[24] = "\031\003\014@@\200Use Text Effect";

char EMTitle99Txt[24] = "\014\200@\200Congratulations!";

char EMMess000Txt[32] = "\013\031\003\r\000You can't use this card";

char EMMess001Txt[24] = "\013\031\003\r\000in this phase.";

char EMMess002Txt[32] = "\013\031\003\r\000You can't put this card on";

char EMMess003Txt[32] = "\013\031\003\r\000the field in this phase.";

char EMMess010Txt[32] = "\013\031\003\r\000You already have a battle";

char EMMess011Txt[24] = "\013\031\003\r\000card on the field.";

char EMMess012Txt[40] = "\013\031\003\r\000This battle card is limited";

char EMMess013Txt[24] = "\013\031\003\r\000to one per turn.";

char EMMess014Txt[40] = "\013\031\003\r\000You already have a situation";

char EMMess015Txt[24] = "\013\031\003\r\000card on the field.";

char EMMess016Txt[32] = "\013\031\003\r\000This situation card is";

char EMMess017Txt[32] = "\013\031\003\r\000limited to one per turn.";

char EMMess018Txt[32] = "\013\031\003\r\000There is no open space";

char EMMess019Txt[24] = "\013\031\003\r\000to put the card.";

char EMMess020Txt[40] = "\013\031\003\r\000You have no playable cards.";

char EMMess022Txt[24] = "\013\031\003\r\000End of phase.";

char EMMess030Txt[32] = "\013\031\003\r\000There are cards still";

char EMMess031Txt[24] = "\013\031\003\r\000available to play.";

char EMMess040Txt[32] = "\013\031\003\r\000The requirements to use";

char EMMess041Txt[32] = "\013\031\003\r\000this card are not met.";

char EMMess050Txt[32] = "\013\031\003\r\000You can't move any cards";

char EMMess051Txt[32] = "\013\031\003\r\000due to a card effect.";

char EMMess052Txt[40] = "\013\031\003\r\000You can't use this card's text.";

char EMMess053Txt[32] = "\013\031\003\r\000You can't use this card's";

char EMMess054Txt[24] = "\013\031\003\r\000text at this time.";

char EMMess055Txt[32] = "\013\031\003\r\000Skipping battle phase";

char EMMess056Txt[32] = "\013\031\003\r\000due to a card effect.";

char EMMess057Txt[8] = "";

char EMMess060Txt[32] = "\013\031\003\r\000You can't put two of these";

char EMMess061Txt[32] = "\013\031\003\r\000cards on the field.";

char EMMess062Txt[40] = "\013\031\003\r\000You can only exchange cards";

char EMMess063Txt[32] = "\013\031\003\r\000inside the battlefield.";

char EMMess070Txt[32] = "\013\031\003\r\000Block your opponent's";

char EMMess071Txt[24] = "\013\031\003\r\000event card\?";

char EMMess072Txt[32] = "\013\031\003\r\000Using this card's effect.";

char EMMess073Txt[24] = "\013\031\003\r\000Is this card okay\?";

char EMMess074Txt[32] = "\013\031\003\r\000Throw away this card\?";

char EMMess075Txt[40] = "\013\031\003\r\000Add this card to your hand\?";

char EMMess076Txt[8] = "";

char EMMess077Txt[32] = "\013\031\003\r\000You don't have any cards";

char EMMess078Txt[32] = "\013\031\003\r\000to play in this phase.";

char EMMess080Txt[24] = "\013\031\003\r\000Event successful.";

char EMMess081Txt[40] = "\013\031\003\r\000You can use this card's effect.";

char EMMess082Txt[24] = "\013\031\003\r\000Event blocked.";

char EMMess083Txt[40] = "\013\031\003\r\000This event has been blocked.";

char EMMess084Txt[40] = "\013\031\003\r\000This card has been discarded.";

char EMMess090Txt[24] = "\013\031\003\r\000Is this hand okay\?";

char EMMess091Txt[32] = "\013\031\003\r\000You have 3 more chances to";

char EMMess092Txt[32] = "\013\031\003\r\000You have 2 more chances to";

char EMMess093Txt[32] = "\013\031\003\r\000You have 1 more chance to";

char EMMess094Txt[40] = "\013\031\003\r\000exchange your initial hand.";

char EMMess100Txt[40] = "\013\031\003\r\000Executing this card's text.";

char EMMess101Txt[8] = "";

char EMMess102Txt[40] = "\013\031\003\r\000The text effect has been lost.";

char EMMess103Txt[8] = "";

char EMMess110Txt[40] = "\013\031\003\r\000You can't use a card's text";

char EMMess111Txt[24] = "\013\031\003\r\000while it is down.";

char EMMess900Txt[24] = "\013\031\003\r\000End this phase\?";

char EMMess901Txt[40] = "\241\241\241\241\014\200@@\241\373 \013\031\003\r\000Yes\241\241\r\000\014@@\200\241\337 \013\031\003\r\000No";

char EMMess902Txt[40] = "\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\241\014\200@@\241\373 \013\031\003\r\000Okay";

char EMMess903Txt[48] = "\241\241\014\200@@\241\373 \013\031\003\r\000Keep\241\241\014@@\200\241\337 \013\031\003\r\000Exchange";

char EMMess910Txt[24] = "\013\031\003\r\000Ending this phase.";

char EMMess990Txt[16] = "\013\031\003\r\001P wins.";

char EMMess991Txt[16] = "\013\031\003\r\002P wins.";

char * ErrorMessTbl[39][5] = { { EMTitle90Txt, EMMess900Txt, 0, 0, EMMess901Txt }, { EMTitle01Txt, EMMess018Txt, EMMess019Txt, 0, EMMess902Txt }, { EMTitle04Txt, EMMess040Txt, EMMess041Txt, 0, 0 }, { EMTitle04Txt, EMMess040Txt, EMMess041Txt, 0, 0 }, { EMTitle04Txt, EMMess040Txt, EMMess041Txt, 0, 0 }, { EMTitle01Txt, EMMess060Txt, EMMess061Txt, 0, 0 }, { EMTitle90Txt, EMMess020Txt, 0, 0, EMMess902Txt }, { EMTitle01Txt, 0, 0, 0, 0 }, { EMTitle01Txt, 0, 0, 0, 0 }, { EMTitle01Txt, EMMess062Txt, EMMess063Txt, 0, EMMess902Txt }, { EMTitle01Txt, EMMess014Txt, EMMess015Txt, EMMess016Txt, EMMess017Txt }, { EMTitle00Txt, EMMess000Txt, EMMess001Txt, 0, 0 }, { EMTitle01Txt, EMMess010Txt, EMMess011Txt, EMMess012Txt, EMMess013Txt }, { EMTitle03Txt, EMMess020Txt, 0, 0, 0 }, { EMTitle02Txt, EMMess030Txt, EMMess031Txt, 0, 0 }, { EMTitle02Txt, EMMess050Txt, EMMess051Txt, 0, 0 }, { EMTitle01Txt, EMMess020Txt, 0, 0, 0 }, { EMTitle03Txt, EMMess052Txt, EMMess051Txt, 0, 0 }, { EMTitle03Txt, EMMess000Txt, EMMess001Txt, 0, 0 }, { EMTitle03Txt, EMMess053Txt, EMMess054Txt, EMMess057Txt, 0 }, { EMTitle90Txt, EMMess055Txt, EMMess056Txt, 0, 0 }, { EMTitle90Txt, EMMess910Txt, 0, 0, EMMess902Txt }, { EMTitle91Txt, EMMess070Txt, EMMess071Txt, 0, EMMess901Txt }, { EMTitle92Txt, EMMess072Txt, EMMess076Txt, 0, EMMess901Txt }, { EMTitle93Txt, EMMess073Txt, 0, 0, EMMess901Txt }, { EMTitle93Txt, EMMess074Txt, 0, 0, EMMess901Txt }, { EMTitle93Txt, EMMess075Txt, EMMess076Txt, 0, EMMess901Txt }, { EMTitle93Txt, EMMess075Txt, EMMess076Txt, 0, EMMess901Txt }, { EMTitle91Txt, EMMess080Txt, EMMess081Txt, EMMess101Txt, EMMess902Txt }, { EMTitle91Txt, EMMess082Txt, EMMess083Txt, EMMess084Txt, EMMess902Txt }, { EMTitle90Txt, EMMess077Txt, EMMess078Txt, EMMess910Txt, EMMess902Txt }, { EMTitle99Txt, EMMess990Txt, 0, 0, 0 }, { EMTitle99Txt, EMMess991Txt, 0, 0, 0 }, { EMTitle94Txt, EMMess090Txt, EMMess091Txt, EMMess094Txt, EMMess903Txt }, { EMTitle94Txt, EMMess090Txt, EMMess092Txt, EMMess094Txt, EMMess903Txt }, { EMTitle94Txt, EMMess090Txt, EMMess093Txt, EMMess094Txt, EMMess903Txt }, { EMTitle92Txt, EMMess100Txt, EMMess101Txt, 0, EMMess902Txt }, { EMTitle95Txt, EMMess102Txt, EMMess103Txt, 0, EMMess902Txt }, { EMTitle04Txt, EMMess110Txt, EMMess111Txt, 0, EMMess902Txt } };

u8 HelpPageList[14] = {
    CARD_HELP_PAGE_DRAW_PHASE, CARD_HELP_PAGE_MOVE_PHASE, CARD_HELP_PAGE_EVENT_PHASE_A,
    CARD_HELP_PAGE_SET_PHASE_A, CARD_HELP_PAGE_BLOCK_PHASE, CARD_HELP_PAGE_BATTLE_PHASE_A,
    CARD_HELP_PAGE_ADJUST_PHASE, CARD_HELP_PAGE_DRAW_PHASE, CARD_HELP_PAGE_MOVE_PHASE,
    CARD_HELP_PAGE_EVENT_PHASE_A, CARD_HELP_PAGE_SET_PHASE_A, CARD_HELP_PAGE_BLOCK_PHASE,
    CARD_HELP_PAGE_BATTLE_PHASE_A, CARD_HELP_PAGE_ADJUST_PHASE
};

float *CursorTbl[32] = {
    OperationPos2P[3], BattlePos2P[3], BattlePos2P[2], DisposePos2P[3],
    OperationPos2P[2], BattlePos2P[3], BattlePos2P[2], DisposePos2P[2],
    OperationPos2P[1], BattlePos2P[1], BattlePos2P[0], DisposePos2P[1],
    OperationPos2P[0], BattlePos2P[1], BattlePos2P[0], DisposePos2P[0],
    DisposePos1P[0], BattlePos1P[0], BattlePos1P[1], OperationPos1P[0],
    DisposePos1P[1], BattlePos1P[0], BattlePos1P[1], OperationPos1P[1],
    DisposePos1P[2], BattlePos1P[2], BattlePos1P[3], OperationPos1P[2],
    DisposePos1P[3], BattlePos1P[2], BattlePos1P[3], OperationPos1P[3]
};

unsigned char DeckData0[40] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

unsigned char DeckGene0[6] = {
    0x01, 0x00, 0x00, 0x00, 0x00, 0x02
};

unsigned char DeckData1[40] = {
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

unsigned char DeckGene1[6] = {
    0x01, 0x00, 0x00, 0x00, 0x00, 0x02
};

unsigned char DeckData2[40] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

unsigned char DeckGene2[6] = {
    0x01, 0x00, 0x00, 0x00, 0x00, 0x02
};

unsigned char DeckData3[40] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

unsigned char DeckGene3[6] = {
    0x01, 0x00, 0x00, 0x00, 0x00, 0x02
};

unsigned char DeckData4[40] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

unsigned char DeckGene4[6] = {
    0x01, 0x00, 0x00, 0x00, 0x00, 0x02
};

unsigned char DeckData5[40] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

unsigned char DeckGene5[6] = {
    0x01, 0x00, 0x00, 0x00, 0x00, 0x02
};

unsigned char DeckData6[40] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02,
    0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

unsigned char DeckGene6[6] = {
    0x01, 0x00, 0x00, 0x00, 0x00, 0x02
};

unsigned char DeckData7[40] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

unsigned char DeckGene7[6] = {
    0x01, 0x00, 0x00, 0x00, 0x00, 0x02
};

unsigned char DeckData8[40] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

unsigned char DeckGene8[6] = {
    0x01, 0x00, 0x00, 0x00, 0x00, 0x02
};

unsigned char DeckData9[40] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

unsigned char DeckGene9[6] = {
    0x01, 0x00, 0x00, 0x00, 0x00, 0x02
};

unsigned char *EnemyDebugDeckLst[10] = {
    SampleDeck0, SampleDeck1, SampleDeck2, SampleDeck3, SampleDeck4, SampleDeck5, SampleDeck6, SampleDeck7, SampleDeck8, SampleDeck9
};

const char cgpDiscardTwoPrompt[32] = "\013\031\003\r\000Cards to be discarded: \243\262";

const char cgpDiscardOnePrompt[40] = "\013\031\003\r\000Cards to be discarded: \243\261";

const char cgpBlockEventPrompt[24] = "\013\031\003\r\000Block this event\?";

const char cgpBlockFurtherPrompt[40] = "\013\031\003\r\000Event blocked. Block further\?";

const char cgpBlockSelectCardHint[64] = "\013\031\003\r\000If yes, select card: \r\004\014\200@@\241\373 \013\031\003\r\000No: \r\004\014@@\200\241\337";

static s32 cnt_0;

static u8 timer_1;

static u8 change_2;

static s8 cardno_3;

static s16 no_4;

static CardCursorPosition mas_5;

static CardCursorPosition mas_6;

static s32 OfferingCnt_7;

static CardCursorPosition mas_15;

static s16 no_16;

static s16 p1no_17;

static s16 p2no_18;

static s32 i_19;

static s32 j_20;

static u8 timer_21;

static u8 firstattack_22;

static s32 Now_23;

static s32 Timer_24;
