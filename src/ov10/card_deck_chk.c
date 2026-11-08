/*
 * OV10 original TU 4: 0x00a19a20..0x00a1c1b8 (5 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov10/cgp.h"
#include "ov10/card_play_sound1.h"
#include "ov10/card_fread.h"
#include "ov10/card_enemy.h"
#include "ov10/card_disp.h"
#include "ov10/card_deck_chk.h"
#include "main/xgl_2.h"
#include "main/xgl_font.h"
#include "main/xgl_sound.h"
#include "main/nml_model_set.h"

/*
 * PadData halfwords used by the deck screens (the shared PAD_* bits): the on-screen prompts
 * show circle = accept and cross = cancel. The L/R shoulder buttons step the filters.
 */
#define CARD_PAD_ATTR_PREV PAD_L2
#define CARD_PAD_ATTR_NEXT PAD_R2
#define CARD_PAD_TYPE_PREV PAD_L1
#define CARD_PAD_TYPE_NEXT PAD_R1
#define CARD_PAD_PREVIEW PAD_TRIANGLE  /* held: show the selected card large */

/*
 * Placement of the selected card shown beside the lists (matrix scaled by CARD_SELECTED_SCALE)
 * and of the card shown large while CARD_PAD_PREVIEW is held (CARD_PREVIEW_OFFSET_X mirrored
 * to the side of the focused pane).
 */
#define CARD_SELECTED_OFFSET_Y  -0.3932f
#define CARD_SELECTED_OFFSET_Z  0.1f
#define CARD_SELECTED_SCALE     0.335f
#define CARD_PREVIEW_OFFSET_X   1.15f
#define CARD_PREVIEW_OFFSET_Y   0.45f
#define CARD_PREVIEW_OFFSET_Z   0.2f

/*
 * Prompt text positions (pixels): the optional deck title, the message and the accept/cancel
 * line, and the font state reset printed before and after them.
 */
#define CARD_PROMPT_X           24
#define CARD_PROMPT_TITLE_Y     312
#define CARD_PROMPT_MESSAGE_Y   336
#define CARD_PROMPT_BUTTONS_Y   360
#define CARD_PROMPT_STATE_X     120
#define CARD_PROMPT_STATE_Y     288

/* Accept/cancel button prompt line (control codes position and colour the circle/cross icons). */
#define CARD_PROMPT_ACCEPT_CANCEL \
    "\r\x04\x0c\x80\x40\x40\xa1\xfb \x0b\x19\x03\r\0Accept\xa1\xa1\r\x04\x0c\x40\x40\x80\xa1\xdf \x0b\x19\x03\r\0Cancel"

/* Rows kept between the cursor and the window edge before the list scrolls. */
#define CARD_SCROLL_MARGIN 2

/*
 * TypeMaskLst entries (CardDefinition.kind bits) and the AttriMaskLst entries the type1/type2
 * filter icons are chosen by in CardMakeDeckProc. The attribute masks are the CARD_ATTR_*
 * flags of ov10/cgp.h: icons type2_1..type2_6 are H, R, M, G, U and W (CARD_ATTR_WEAPON is
 * both weapon bits, so it matches only the combined mask).
 */

/* Cursor bit set while a window is not the focused pane. */
#define CARD_PANE_INACTIVE 0x80

/* Cursor bits that hold the row index. */
#define CARD_CURSOR_INDEX_MASK 0x7f

/* At most 3 copies of one card in a deck (CardDeckChk). */
#define CARD_MAX_COPIES 3

/* The stored-deck list shows five slots. */
#define CARD_DECK_SLOT_COUNT 5

/* Entries of TypeMaskLst and AttriMaskLst. */
#define CARD_TYPE_FILTER_COUNT 4
#define CARD_ATTR_FILTER_COUNT 7

/*
 * CardListWindow.mode requests. Scrolling is -1 (up) / 1 (down); the deck
 * builder also uses the add/remove requests, the list window the refresh.
 */
enum {
    CARD_ACTION_NONE = 0,
    CARD_ACTION_SCROLL_DOWN = 1,
    CARD_ACTION_REFRESH = 2,       /* list: rebuild the rows, cursor and scroll to the top */
    CARD_ACTION_ADD = 3,           /* add the selected card to the deck */
    CARD_ACTION_REMOVE = 4,        /* remove the selected card from the deck */
    CARD_ACTION_SCROLL_UP = -1
};

/* CardCommandControl.step of the deck load and save screens: pick a slot, then confirm it. */
enum {
    CARD_DECK_STEP_SELECT = 0,
    CARD_DECK_STEP_CONFIRM = 1
};

/*
 * CardMakeDeckProc results: the message code the caller shows (and the sound
 * effect it plays) when an edit is refused. Code 2 (deck not complete) is
 * raised by the caller itself.
 */
enum {
    CARD_DECK_MSG_NONE = 0,
    CARD_DECK_MSG_COPIES_USED = 1, /* every owned copy is already in the deck */
    CARD_DECK_MSG_DECK_FULL = 3,
    CARD_DECK_MSG_NOT_OWNED = 4
};

/*
 * Deck-builder graphics, loaded by the caller (CardFread into fixed buffers):
 * the type filter icons type1_B/E/S, then the attribute icons type2_0..6 at
 * 0x400 intervals, and their textures. The digit models of the card number and
 * deck count are 0x400 apart, indexed by the digit (CARD_DIGIT_MODEL_BASE).
 */
#define CARD_MODEL_TYPE1_B 0x01900000
#define CARD_MODEL_TYPE1_E 0x01900400
#define CARD_MODEL_TYPE1_S 0x01900800
#define CARD_MODEL_TYPE2_0 0x01900C00
#define CARD_MODEL_TYPE2_1 0x01901000
#define CARD_MODEL_TYPE2_2 0x01901400
#define CARD_MODEL_TYPE2_3 0x01901800
#define CARD_MODEL_TYPE2_4 0x01901C00
#define CARD_MODEL_TYPE2_5 0x01902000
#define CARD_MODEL_TYPE2_6 0x01902400
#define CARD_TEXTURE_TYPE1_B ((const char *)0x01908000)
#define CARD_TEXTURE_TYPE1_E ((const char *)0x01918100)
#define CARD_TEXTURE_TYPE1_S ((const char *)0x01928200)
#define CARD_TEXTURE_TYPE2_0 ((const char *)0x01938300)

char ZenkakuListA[20] = "\xA1\xA1" "1 2 3 4 5 6 7 8 9 ";

static int ActiveCursol_0;
static int ActiveCursol_2;
static int ActiveCursol_4;
static int ActiveCursol_6;

static CardListWindow *pCnow_1;
static CardListWindow *pCnow_3;
static CardListWindow *pCnow_5;
static CardListWindow *pCnow_7;
static u8 TypeMaskLst[4];
static u8 AttriMaskLst[7];

/* Returns 1 when no card appears more than CARD_MAX_COPIES times and no entry after the first is negative. */
int CardDeckChk(s16 *cards) {
    int first_index;
    int second_index;
    int matching_copies;

    for (first_index = 0; first_index < CARD_DECK_SIZE; first_index++) {
        matching_copies = 0;

        for (second_index = first_index + 1; second_index < CARD_DECK_SIZE; second_index++) {
            if (cards[second_index] < 0) {
                return 0;
            }

            if (cards[first_index] == cards[second_index]) {
                matching_copies++;
                if (matching_copies >= CARD_MAX_COPIES) {
                    return 0;
                }
            }
        }
    }

    return 1;
}

/*
 * Deck-builder screen: applies pad input to the card list (list) and the deck (deck),
 * then draws both windows with the filter icons, the selected card and the digits.
 * `exiting` is nonzero while the exit prompt shows (draw only). Returns a CARD_DECK_MSG_*
 * code when an edit is refused.
 */
int CardMakeDeckProc(int exiting, CardListWindow *list, CardListWindow *deck, CardGameWork *work)
{
    float matrix[4][4];
    float offset[4];
    float scale[4];
    float pinMatrix[4][4];
    float filterMatrix[4][4];
    float digitMatrix[4][4];
    float countMatrix[4][4];
    s16 *ownedCount;
    int result;
    int hovered;
    int total;
    int digit;
    int typeMask;
    int attr;
    const char *texture;
    int entry;
    int model;

    ownedCount = work->ownedCount;
    work->helpTopic = CARD_HELP_PAGE_CREATING_DECKS;

    if (ActiveCursol_0 != 0) {
        pCnow_1 = deck;
        list->cursor |= CARD_PANE_INACTIVE;
        deck->cursor &= CARD_CURSOR_INDEX_MASK;
    } else {
        pCnow_1 = list;
        list->cursor &= CARD_CURSOR_INDEX_MASK;
        deck->cursor |= CARD_PANE_INACTIVE;
    }

    result = 0;
    offset[0] = 0.0f;
    offset[1] = CARD_SELECTED_OFFSET_Y;
    offset[2] = CARD_SELECTED_OFFSET_Z;
    offset[3] = 1.0f;
    scale[1] = CARD_SELECTED_SCALE;
    scale[0] = CARD_SELECTED_SCALE;
    scale[3] = 1.0f;
    scale[2] = 1.0f;
    xglMatrixUnit(matrix);
    xglMatrixTrans(matrix, matrix, offset);
    xglMatrixScale(matrix, matrix, scale);

    /* Pad input: edit the filters, move the cursor, switch focus, add or remove cards. */
    if (exiting == 0) {
        if (PadData[0].half_2a & CARD_PAD_TYPE_NEXT) {
            xglSoundEffectNormalID(SE_CURSOR, 0);
            list->typeFilter++;
            list->mode = CARD_ACTION_REFRESH;
        }
        if (PadData[0].half_2a & CARD_PAD_TYPE_PREV) {
            xglSoundEffectNormalID(SE_CURSOR, 0);
            list->typeFilter--;
            list->mode = CARD_ACTION_REFRESH;
        }
        if (PadData[0].half_2a & CARD_PAD_ATTR_NEXT) {
            xglSoundEffectNormalID(SE_CURSOR, 0);
            list->attrFilter++;
            list->mode = CARD_ACTION_REFRESH;
        }
        if (PadData[0].half_2a & CARD_PAD_ATTR_PREV) {
            xglSoundEffectNormalID(SE_CURSOR, 0);
            list->attrFilter--;
            list->mode = CARD_ACTION_REFRESH;
        }
        if (PadData[0].half_2a & PAD_CIRCLE) {
            xglSoundEffectNormalID(SE_DECIDE, 0);
            pCnow_1->mode = CARD_ACTION_ADD;
        }
        if (PadData[0].half_2a & PAD_CROSS) {
            xglSoundEffectNormalID(SE_CANCEL, 0);
            pCnow_1->mode = CARD_ACTION_REMOVE;
        }
        if (PadData[0].directions & PAD_UP) {
            xglSoundEffectNormalID(SE_CURSOR, 0);
            pCnow_1->mode = CARD_ACTION_SCROLL_UP;
        }
        if (PadData[0].directions & PAD_DOWN) {
            xglSoundEffectNormalID(SE_CURSOR, 0);
            pCnow_1->mode = CARD_ACTION_SCROLL_DOWN;
        }
        if (PadData[0].directions & PAD_RIGHT) {
            xglSoundEffectNormalID(SE_CURSOR, 0);
            if (CardGetDeckAllCnt(deck) != 0) {
                ActiveCursol_0 = 1;
                list->cursor |= CARD_PANE_INACTIVE;
                deck->cursor &= CARD_CURSOR_INDEX_MASK;
            }
        }
        if (PadData[0].directions & PAD_LEFT) {
            xglSoundEffectNormalID(SE_CURSOR, 0);
            ActiveCursol_0 = 0;
            list->cursor &= CARD_CURSOR_INDEX_MASK;
            deck->cursor |= CARD_PANE_INACTIVE;
        }
    }

    if (list->typeFilter < 0) {
        list->typeFilter = CARD_TYPE_FILTER_COUNT - 1;
    }
    if (list->typeFilter >= CARD_TYPE_FILTER_COUNT) {
        list->typeFilter = 0;
    }
    if (list->attrFilter < 0) {
        list->attrFilter = CARD_ATTR_FILTER_COUNT - 1;
    }
    if (list->attrFilter >= CARD_ATTR_FILTER_COUNT) {
        list->attrFilter = 0;
    }

    /* Apply the pending deck action, or else the pending list action. */
    if (deck->mode != CARD_ACTION_NONE) {
        switch (deck->mode) {
        case CARD_ACTION_SCROLL_DOWN:
        case CARD_ACTION_SCROLL_UP:
            if (deck->mode < 0) {
                if (deck->top > 0) {
                    if (deck->cursor < CARD_SCROLL_MARGIN) {
                        deck->top = deck->top - 1;
                    } else {
                        deck->cursor = deck->cursor - 1;
                        if (deck->cursor < 0) {
                            deck->cursor = 0;
                        }
                    }
                } else if (deck->cursor > 0) {
                    deck->cursor = deck->cursor - 1;
                }
            } else if (deck->count > deck->top + deck->rows) {
                if (deck->cursor >= deck->rows - CARD_SCROLL_MARGIN) {
                    deck->top = deck->top + 1;
                } else {
                    deck->cursor = deck->cursor + 1;
                }
            } else {
                if (deck->cursor < deck->rows - 1
                    && deck->cursor < deck->count - 1
                    && deck->cursor < deck->shownCount) {
                    deck->cursor = deck->cursor + 1;
                }
            }
            break;
        case CARD_ACTION_ADD:
            if (CardGetDeckAllCnt(deck) < CARD_DECK_SIZE) {
                s16 id = deck->entries[deck->top + deck->cursor].cardId;

                if (CardGetDeckCardCnt(id, deck) < ownedCount[id]) {
                    CardAddDeck(deck, id);
                } else {
                    result = (ownedCount[id] == 0) ? CARD_DECK_MSG_NOT_OWNED : CARD_DECK_MSG_COPIES_USED;
                }
            } else {
                result = CARD_DECK_MSG_DECK_FULL;
            }
            break;
        case CARD_ACTION_REMOVE:
            CardDelDeck(deck, deck->entries[deck->top + deck->cursor].cardId);
            if (CardGetDeckAllCnt(deck) == 0) {
                ActiveCursol_0 = 0;
                list->cursor &= CARD_CURSOR_INDEX_MASK;
                deck->cursor |= CARD_PANE_INACTIVE;
            }
            break;
        }
        deck->mode = CARD_ACTION_NONE;
    } else if (list->mode != CARD_ACTION_NONE) {
        list->count = CardMakeDispList(list->entries,
                                          AttriMaskLst[list->attrFilter]
                                              | (TypeMaskLst[list->typeFilter] << 8),
                                          work->definitions, work->cardCount);
        switch (list->mode) {
        case CARD_ACTION_NONE:
            break;
        case CARD_ACTION_SCROLL_DOWN:
        case CARD_ACTION_SCROLL_UP:
            if (list->mode < 0) {
                if (list->top > 0) {
                    if (list->cursor < CARD_SCROLL_MARGIN) {
                        list->top = list->top - 1;
                    } else {
                        list->cursor = list->cursor - 1;
                        if (list->cursor < 0) {
                            list->cursor = 0;
                        }
                    }
                } else if (list->cursor > 0) {
                    list->cursor = list->cursor - 1;
                }
            } else if (list->top + list->rows < list->count) {
                if (list->cursor >= list->rows - CARD_SCROLL_MARGIN) {
                    list->top = list->top + 1;
                } else {
                    list->cursor = list->cursor + 1;
                }
            } else if (list->cursor < list->rows - 1
                       && list->cursor < list->count - 1) {
                list->cursor = list->cursor + 1;
            }
            break;
        case CARD_ACTION_REFRESH:
            list->cursor = 0;
            list->top = 0;
            break;
        case CARD_ACTION_ADD:
            result = CARD_DECK_MSG_DECK_FULL;
            if (CardGetDeckAllCnt(deck) < CARD_DECK_SIZE) {
                s16 id = list->entries[list->top + list->cursor].cardId;
                total = CardGetDeckCardCnt(id, deck);

                if (total < CARD_MAX_COPIES) {
                    if (total < ownedCount[id]) {
                        int newIdx = CardAddDeck(deck, id);
                        if (newIdx - deck->rows + 1 < 0) {
                            deck->top = 0;
                        } else {
                            deck->top = newIdx - deck->rows + 1;
                        }
                        /* Row within the window; the subtraction sets CARD_PANE_INACTIVE on the deck cursor because the list has focus. */
                        deck->cursor = (newIdx % deck->rows) - CARD_PANE_INACTIVE;
                        result = 0;
                    } else {
                        result = (ownedCount[id] == 0) ? CARD_DECK_MSG_NOT_OWNED : CARD_DECK_MSG_COPIES_USED;
                    }
                }
            }
            break;
        case CARD_ACTION_REMOVE:
            CardDelDeck(deck, list->entries[list->top + list->cursor].cardId);
            break;
        }
        list->mode = CARD_ACTION_NONE;
    }

    if (deck->count - 1 < deck->cursor) {
        deck->cursor = deck->count - 1;
    }
    if (deck->shownCount - 1 < deck->cursor) {
        deck->cursor = deck->shownCount - 1;
    }

    /* Preview card while CARD_PAD_PREVIEW is held, then the list and deck windows. */
    hovered = 0;
    if ((PadData[0].half_28 & CARD_PAD_PREVIEW) && exiting == 0) {
        hovered = 1;
        if (ActiveCursol_0 != 0) {
            offset[0] = -CARD_PREVIEW_OFFSET_X;
            pCnow_1 = deck;
        } else {
            offset[0] = CARD_PREVIEW_OFFSET_X;
            pCnow_1 = list;
        }
        offset[1] = CARD_PREVIEW_OFFSET_Y;
        offset[2] = CARD_PREVIEW_OFFSET_Z;
        offset[3] = 1.0f;
        xglMatrixUnit(pinMatrix);
        xglMatrixTrans(pinMatrix, pinMatrix, offset);
        {
            CardListWindow *sel = pCnow_1;
            s16 id = sel->entries[sel->top + sel->cursor].cardId;
            CardDisp(&pinMatrix, id, work->ownedCount[id] != 0, exiting == 0);
        }
    }
    if (hovered == 0 || ActiveCursol_0 == 0) {
        CardListDisp(list, work);
    }
    if (hovered == 0 || ActiveCursol_0 != 0) {
        CardDeckDisp(deck, work->definitions);
    }

    /* Type and attribute filter icons. */
    offset[0] = offset[2] = 0.0f;
    offset[1] = 1.292f;
    offset[3] = 1.0f;
    xglMatrixUnit(filterMatrix);
    xglMatrixTrans(filterMatrix, filterMatrix, offset);
    nmlModelSetPlace(&filterMatrix);
    typeMask = TypeMaskLst[list->typeFilter];
    if (typeMask == CARD_KIND_BATTLE) {
        texture = CARD_TEXTURE_TYPE1_B;
        entry = CARD_MODEL_TYPE1_B;
    } else if (typeMask == CARD_KIND_EVENT) {
        texture = CARD_TEXTURE_TYPE1_E;
        entry = CARD_MODEL_TYPE1_E;
    } else if (typeMask == CARD_KIND_SITUATION) {
        texture = CARD_TEXTURE_TYPE1_S;
        entry = CARD_MODEL_TYPE1_S;
    } else {
        texture = CARD_TEXTURE_TYPE2_0;
        entry = CARD_MODEL_TYPE2_0;
    }
    nmlModelSetTexture(texture);
    nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
    nmlModelEntryCard(entry);

    offset[1] = 1.047f;
    entry = CARD_MODEL_TYPE2_1;
    xglMatrixUnit(filterMatrix);
    xglMatrixTrans(filterMatrix, filterMatrix, offset);
    nmlModelSetPlace(&filterMatrix);
    /*
     * Attribute masks CARD_ATTR_HUMAN, REALIAN, MECH, GNOSIS, CARD_ATTR_UNKNOWN and
     * CARD_ATTR_WEAPON select type2_1..type2_6; any other value shows type2_0. The texture
     * assignments in the last two arms are never read: with a single assignment per arm the
     * compiler turns the final test into a conditional move (the original branches), so both
     * arms keep their second statement.
     */
    attr = AttriMaskLst[list->attrFilter];
    if (attr != CARD_ATTR_HUMAN) {
        entry = CARD_MODEL_TYPE2_2;
        if (attr != CARD_ATTR_REALIAN) {
            entry = CARD_MODEL_TYPE2_3;
            if (attr != CARD_ATTR_MECH) {
                entry = CARD_MODEL_TYPE2_4;
                if (attr != CARD_ATTR_GNOSIS) {
                    entry = CARD_MODEL_TYPE2_5;
                    if (attr != CARD_ATTR_UNKNOWN) {
                        if (attr == CARD_ATTR_WEAPON) {
                            texture = CARD_TEXTURE_TYPE2_0;
                            entry = CARD_MODEL_TYPE2_6;
                        } else {
                            texture = CARD_TEXTURE_TYPE2_0;
                            entry = CARD_MODEL_TYPE2_0;
                        }
                    }
                }
            }
        }
    }
    nmlModelSetTexture(CARD_TEXTURE_TYPE2_0);
    nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
    nmlModelEntryCard(entry);

    if (ActiveCursol_0 != 0) {
        pCnow_1 = deck;
    } else {
        pCnow_1 = list;
    }
    {
        CardListWindow *sel = pCnow_1;
        s16 id = sel->entries[sel->top + sel->cursor].cardId;
        int flag;

        /* The flag is materialised with a branch; `exiting == 0` inline changes the code. */
        if (exiting) {
            flag = 0;
        } else {
            flag = 1;
        }
        CardDisp(&matrix, id, work->ownedCount[id] != 0, flag);
    }

    /* Card number digits of the selected card (its id + 1). */
    total = list->entries[list->top + (list->cursor & CARD_CURSOR_INDEX_MASK)].cardId + 1;
    digit = total / 100;
    offset[1] = 1.764f;
    offset[2] = 0.01f;
    scale[0] = 1.13f;
    offset[3] = 1.0f;
    scale[3] = 1.0f;
    scale[2] = 1.0f;
    scale[1] = 1.0f;
    xglMatrixUnit(digitMatrix);
    xglMatrixScale(digitMatrix, digitMatrix, scale);
    if (digit != 0) {
        model = (digit << CARD_DIGIT_MODEL_SHIFT) + CARD_DIGIT_MODEL_BASE;
        offset[0] = -0.895f;
        xglMatrixTrans(matrix, digitMatrix, offset);
        nmlModelSetPlace(&matrix);
        nmlModelSetTexture(CARD_DIGIT_TEXTURE);
        nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
        nmlModelEntryCard(model);
    }
    digit = total % 100 / 10;
    if (total / 100 != 0 || digit != 0) {
        model = (digit << CARD_DIGIT_MODEL_SHIFT) + CARD_DIGIT_MODEL_BASE;
        offset[0] = -0.748f;
        xglMatrixTrans(matrix, digitMatrix, offset);
        nmlModelSetPlace(&matrix);
        nmlModelSetTexture(CARD_DIGIT_TEXTURE);
        nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
        nmlModelEntryCard(model);
    }
    digit = total % 100 % 10;
    model = (digit << CARD_DIGIT_MODEL_SHIFT) + CARD_DIGIT_MODEL_BASE;
    offset[0] = -0.588f;
    xglMatrixTrans(matrix, digitMatrix, offset);
    nmlModelSetPlace(&matrix);
    nmlModelSetTexture(CARD_DIGIT_TEXTURE);
    nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
    nmlModelEntryCard(model);

    /* Deck count digits. */
    total = CardGetDeckAllCnt(deck);
    digit = total / 100;
    offset[1] = 1.764f;
    offset[2] = 0.01f;
    scale[0] = 1.13f;
    offset[3] = 1.0f;
    scale[3] = 1.0f;
    scale[2] = 1.0f;
    scale[1] = 1.0f;
    xglMatrixUnit(countMatrix);
    xglMatrixScale(countMatrix, countMatrix, scale);
    if (digit != 0) {
        entry = (digit << CARD_DIGIT_MODEL_SHIFT) + CARD_DIGIT_MODEL_BASE;
        offset[0] = 1.948f;
        xglMatrixTrans(matrix, countMatrix, offset);
        nmlModelSetPlace(&matrix);
        nmlModelSetTexture(CARD_DIGIT_TEXTURE);
        nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
        nmlModelEntryCard(entry);
    }
    digit = total % 100 / 10;
    if (total / 100 != 0 || digit != 0) {
        entry = (digit << CARD_DIGIT_MODEL_SHIFT) + CARD_DIGIT_MODEL_BASE;
        offset[0] = 2.095f;
        xglMatrixTrans(matrix, countMatrix, offset);
        nmlModelSetPlace(&matrix);
        nmlModelSetTexture(CARD_DIGIT_TEXTURE);
        nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
        nmlModelEntryCard(entry);
    }
    digit = total % 100 % 10;
    entry = (digit << CARD_DIGIT_MODEL_SHIFT) + CARD_DIGIT_MODEL_BASE;
    offset[0] = 2.255f;
    xglMatrixTrans(matrix, countMatrix, offset);
    nmlModelSetPlace(&matrix);
    nmlModelSetTexture(CARD_DIGIT_TEXTURE);
    nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
    nmlModelEntryCard(entry);

    return result;
}

/*
 * Deck-load screen: step 0 browses the five stored deck slots (list) and shows the
 * selected deck in the deck window (deck); step 1 asks for confirmation. `exiting` draws
 * only. Returns the chosen slot + 1, -1 when cancelled, 0 otherwise.
 */
int CardLoadDeckProc(int exiting, CardListWindow *list, CardListWindow *deck, CardGameWork *work, const char *title)
{
    int drawCards;
    int result;
    float initVec[4];
    float cursor[4][4];
    float vecA[4];
    float vecB[4];
    s16 cardId;
    int pos;
    int entry;
    int prev;
    CardSaveData *stored;
    int slot;
    int pressed;

    work->helpTopic = CARD_HELP_PAGE_SELECTING_DECKS;
    if (ActiveCursol_2 != 0) {
        pCnow_3 = deck;
        list->cursor |= CARD_PANE_INACTIVE;
        deck->cursor &= CARD_CURSOR_INDEX_MASK;
    } else {
        pCnow_3 = list;
        list->cursor &= CARD_CURSOR_INDEX_MASK;
        deck->cursor |= CARD_PANE_INACTIVE;
    }

    result = 0;
    switch (work->command.step) {
    case CARD_DECK_STEP_SELECT:
            drawCards = 0;
            if ((PadData[0].half_28 & CARD_PAD_PREVIEW) != 0 && exiting == 0 && ActiveCursol_2 != 0) {
                drawCards = 1;
                pCnow_3 = deck;
                initVec[0] = -CARD_PREVIEW_OFFSET_X;
                initVec[1] = CARD_PREVIEW_OFFSET_Y;
                initVec[2] = CARD_PREVIEW_OFFSET_Z;
                initVec[3] = 1.0f;
                xglMatrixUnit(cursor);
                xglMatrixTrans(cursor, cursor, initVec);
                cardId = pCnow_3->entries[pCnow_3->top + pCnow_3->cursor].cardId;
                CardDisp(&cursor, cardId, work->ownedCount[cardId] != 0, 1);
            }

            if (drawCards == 0 || ActiveCursol_2 == 0) {
                CardDeckListDisp(list);
            }
            CardDeckDisp(deck, work->definitions);

            if (ActiveCursol_2 == 0) {
                if (exiting == 0 && drawCards == 0) {
                    if (title != 0) {
                        xglFontPrint(CARD_PROMPT_X, CARD_PROMPT_TITLE_Y, CARD_TEXT_COLOR, title);
                    }
                    xglFontPrint(0, 0, 0, "\x19\x03");
                    xglFontPrint(CARD_PROMPT_STATE_X, CARD_PROMPT_STATE_Y, CARD_TEXT_COLOR, "");
                    xglFontPrint(CARD_PROMPT_X, CARD_PROMPT_MESSAGE_Y, CARD_TEXT_COLOR, "\x0b\x19\x03\r\0Please select a deck to load.");
                    xglFontPrint(CARD_PROMPT_X, CARD_PROMPT_BUTTONS_Y, CARD_TEXT_COLOR, CARD_PROMPT_ACCEPT_CANCEL);
                    xglFontPrint(CARD_PROMPT_STATE_X, CARD_PROMPT_STATE_Y, CARD_TEXT_COLOR, "\x0b");
                    pressed = work->command.input[0].pressed;
                    if ((pressed & PAD_CIRCLE) != 0) {
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                        work->command.step++;
                    } else if ((pressed & PAD_CROSS) != 0) {
                        xglSoundEffectNormalID(SE_CANCEL, 0);
                        result = -1;
                    }
                }
            } else {
                vecA[0] = 0.0f;
                vecA[1] = CARD_SELECTED_OFFSET_Y;
                vecA[2] = CARD_SELECTED_OFFSET_Z;
                vecA[3] = 1.0f;
                vecB[0] = vecB[1] = CARD_SELECTED_SCALE;
                vecB[2] = vecB[3] = 1.0f;
                xglMatrixUnit(cursor);
                xglMatrixTrans(cursor, cursor, vecA);
                xglMatrixScale(cursor, cursor, vecB);
                cardId = pCnow_3->entries[pCnow_3->top + pCnow_3->cursor].cardId;
                CardDisp(&cursor, cardId, work->ownedCount[cardId] != 0, exiting == 0);
            }

            if (exiting == 0) {
                if ((PadData[0].directions & PAD_UP) != 0) {
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                    pCnow_3->mode = CARD_ACTION_SCROLL_UP;
                } else if ((PadData[0].directions & PAD_DOWN) != 0) {
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                    pCnow_3->mode = CARD_ACTION_SCROLL_DOWN;
                }
                if ((PadData[0].directions & PAD_RIGHT) != 0) {
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                    if (CardGetDeckAllCnt(deck) != 0) {
                        ActiveCursol_2 = 1;
                        list->cursor |= CARD_PANE_INACTIVE;
                        deck->cursor &= CARD_CURSOR_INDEX_MASK;
                    }
                } else if ((PadData[0].directions & PAD_LEFT) != 0) {
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                    ActiveCursol_2 = 0;
                    list->cursor &= CARD_CURSOR_INDEX_MASK;
                    deck->cursor |= CARD_PANE_INACTIVE;
                }
            }

            if (deck->mode != CARD_ACTION_NONE) {
                if (deck->mode == CARD_ACTION_SCROLL_UP || deck->mode == CARD_ACTION_SCROLL_DOWN) {
                    if (deck->mode < 0) {
                        if (deck->top > 0) {
                            if (deck->cursor < CARD_SCROLL_MARGIN) {
                                deck->top = deck->top - 1;
                            } else {
                                deck->cursor = deck->cursor - 1;
                                if (deck->cursor < 0) {
                                    deck->cursor = 0;
                                }
                            }
                        } else if (deck->cursor > 0) {
                            deck->cursor = deck->cursor - 1;
                        }
                    } else {
                        if (deck->top + deck->rows < deck->count) {
                            if (deck->cursor >= deck->rows - CARD_SCROLL_MARGIN) {
                                deck->top = deck->top + 1;
                            } else {
                                deck->cursor = deck->cursor + 1;
                            }
                        } else if (deck->cursor < deck->rows - 1 && deck->cursor < deck->count - 1
                                   && deck->cursor < deck->shownCount) {
                            deck->cursor = deck->cursor + 1;
                        }
                    }
                }
                deck->mode = CARD_ACTION_NONE;
            }

            /* Slot list: scroll, then show the highlighted slot's cards in the deck window. */
            if (list->mode != CARD_ACTION_NONE) {
                list->count = CARD_DECK_SLOT_COUNT;
                if (list->mode == CARD_ACTION_SCROLL_UP || list->mode == CARD_ACTION_SCROLL_DOWN) {
                    if (list->mode < 0) {
                        if (list->top > 0) {
                            if (list->cursor < CARD_SCROLL_MARGIN) {
                                list->top = list->top - 1;
                            } else {
                                list->cursor = list->cursor - 1;
                                if (list->cursor < 0) {
                                    list->cursor = 0;
                                }
                            }
                        } else if (list->cursor > 0) {
                            list->cursor = list->cursor - 1;
                        }
                    } else {
                        if (list->top + list->rows < list->count) {
                            if (list->cursor >= list->rows - CARD_SCROLL_MARGIN) {
                                list->top = list->top + 1;
                            } else {
                                list->cursor = list->cursor + 1;
                            }
                        } else if (list->cursor < list->rows - 1 && list->cursor < CARD_DECK_SLOT_COUNT - 1) {
                            list->cursor = list->cursor + 1;
                        }
                    }
                    slot = list->top + (list->cursor & CARD_CURSOR_INDEX_MASK);
                    /* Address of the stored deck slot: records are 42 bytes apart (slot * 32 + slot * 10);
                     * work->save->decks[slot] computes it with a different instruction order. */
                    stored = (CardSaveData *)((slot << 5) + (unsigned int)work->save + slot * 10);
                    prev = -1;
                    entry = -1;
                    for (pos = 0; pos < CARD_DECK_SIZE; pos++) {
                        if (prev != stored->decks[0].cards[pos]) {
                            entry++;
                            deck->entries[entry].cardId = stored->decks[0].cards[pos];
                            deck->entries[entry].count = 1;
                        } else {
                            deck->entries[entry].count++;
                        }
                        prev = stored->decks[0].cards[pos];
                    }
                    deck->cursor = CARD_PANE_INACTIVE;
                    deck->count = entry + 1;
                    deck->top = 0;
                }
                list->mode = CARD_ACTION_NONE;
            }

            if (deck->count - 1 < deck->cursor) {
                deck->cursor = deck->count - 1;
            }
        break;
    /* Step 1: confirm the highlighted slot. */
    case CARD_DECK_STEP_CONFIRM:
            CardDeckListDisp(list);
            CardDeckDisp(deck, work->definitions);
            if (exiting != 0) {
                break;
            }
            xglFontPrint(0, 0, 0, "\x19\x03");
            xglFontPrint(CARD_PROMPT_STATE_X, CARD_PROMPT_STATE_Y, CARD_TEXT_COLOR, "");
            xglFontPrint(CARD_PROMPT_X, CARD_PROMPT_MESSAGE_Y, CARD_TEXT_COLOR, "\x0b\x19\x03\r\0Is this deck okay?");
            xglFontPrint(CARD_PROMPT_X, CARD_PROMPT_BUTTONS_Y, CARD_TEXT_COLOR, CARD_PROMPT_ACCEPT_CANCEL);
            xglFontPrint(CARD_PROMPT_STATE_X, CARD_PROMPT_STATE_Y, CARD_TEXT_COLOR, "\x0b");
            pressed = work->command.input[0].pressed;
            if ((pressed & PAD_CIRCLE) != 0) {
                xglSoundEffectNormalID(SE_DECIDE, 0);
                result = (list->cursor & CARD_CURSOR_INDEX_MASK) + 1;
            } else if ((pressed & PAD_CROSS) != 0) {
                xglSoundEffectNormalID(SE_CANCEL, 0);
                work->command.step--;
            }
        break;
    }
    return result;
}

/*
 * Deck-save screen: step 0 browses the five stored deck slots (list) and shows the
 * selected deck in the deck window (deck); step 1 asks for confirmation. `exiting` draws
 * only. Returns the chosen slot + 1 to save into, -1 when cancelled, 0 otherwise.
 */
int CardSaveDeckProc(int exiting, CardListWindow *list, CardListWindow *deck, CardGameWork *work, const char *title)
{
    int drawCards;
    int result;
    float initVec[4];
    float cursor[4][4];
    s16 cardId;
    int pos;
    int entry;
    int prev;
    CardSaveData *stored;
    int slot;
    int pressed;

    work->helpTopic = CARD_HELP_PAGE_SAVING_DECKS;
    if (ActiveCursol_4 != 0) {
        pCnow_5 = deck;
        list->cursor |= CARD_PANE_INACTIVE;
        deck->cursor &= CARD_CURSOR_INDEX_MASK;
    } else {
        pCnow_5 = list;
        list->cursor &= CARD_CURSOR_INDEX_MASK;
        deck->cursor |= CARD_PANE_INACTIVE;
    }

    result = 0;
    switch (work->command.step) {
    case CARD_DECK_STEP_SELECT:
            if (exiting == 0) {
                if ((PadData[0].directions & PAD_UP) != 0) {
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                    pCnow_5->mode = CARD_ACTION_SCROLL_UP;
                }
                if ((PadData[0].directions & PAD_DOWN) != 0) {
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                    pCnow_5->mode = CARD_ACTION_SCROLL_DOWN;
                }
                if ((PadData[0].directions & PAD_RIGHT) != 0) {
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                    if (CardGetDeckAllCnt(deck) != 0) {
                        ActiveCursol_4 = 1;
                        list->cursor |= CARD_PANE_INACTIVE;
                        deck->cursor &= CARD_CURSOR_INDEX_MASK;
                    }
                }
                if ((PadData[0].directions & PAD_LEFT) != 0) {
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                    ActiveCursol_4 = 0;
                    list->cursor &= CARD_CURSOR_INDEX_MASK;
                    deck->cursor |= CARD_PANE_INACTIVE;
                }
            }

            if (deck->mode != CARD_ACTION_NONE) {
                if (deck->mode == CARD_ACTION_SCROLL_UP || deck->mode == CARD_ACTION_SCROLL_DOWN) {
                    if (deck->mode < 0) {
                        if (deck->top > 0) {
                            if (deck->cursor < CARD_SCROLL_MARGIN) {
                                deck->top = deck->top - 1;
                            } else {
                                deck->cursor = deck->cursor - 1;
                                if (deck->cursor < 0) {
                                    deck->cursor = 0;
                                }
                            }
                        } else if (deck->cursor > 0) {
                            deck->cursor = deck->cursor - 1;
                        }
                    } else {
                        if (deck->top + deck->rows < deck->count) {
                            if (deck->cursor >= deck->rows - CARD_SCROLL_MARGIN) {
                                deck->top = deck->top + 1;
                            } else {
                                deck->cursor = deck->cursor + 1;
                            }
                        } else if (deck->cursor < deck->rows - 1 && deck->cursor < deck->count - 1
                                   && deck->cursor < deck->shownCount) {
                            deck->cursor = deck->cursor + 1;
                        }
                    }
                }
                deck->mode = CARD_ACTION_NONE;
            }

            /* Slot list: scroll, then show the highlighted slot's cards in the deck window. */
            if (list->mode != CARD_ACTION_NONE) {
                list->count = CARD_DECK_SLOT_COUNT;
                if (list->mode == CARD_ACTION_SCROLL_UP || list->mode == CARD_ACTION_SCROLL_DOWN) {
                    if (list->mode < 0) {
                        if (list->top > 0) {
                            if (list->cursor < CARD_SCROLL_MARGIN) {
                                list->top = list->top - 1;
                            } else {
                                list->cursor = list->cursor - 1;
                                if (list->cursor < 0) {
                                    list->cursor = 0;
                                }
                            }
                        } else if (list->cursor > 0) {
                            list->cursor = list->cursor - 1;
                        }
                    } else {
                        if (list->top + list->rows < list->count) {
                            if (list->cursor >= list->rows - CARD_SCROLL_MARGIN) {
                                list->top = list->top + 1;
                            } else {
                                list->cursor = list->cursor + 1;
                            }
                        } else if (list->cursor < list->rows - 1 && list->cursor < CARD_DECK_SLOT_COUNT - 1) {
                            list->cursor = list->cursor + 1;
                        }
                    }
                    slot = list->top + (list->cursor & CARD_CURSOR_INDEX_MASK);
                    /* Address of the stored deck slot: records are 42 bytes apart (slot * 32 + slot * 10);
                     * work->save->decks[slot] computes it with a different instruction order. */
                    stored = (CardSaveData *)((slot << 5) + (unsigned int)work->save + slot * 10);
                    prev = -1;
                    entry = -1;
                    for (pos = 0; pos < CARD_DECK_SIZE; pos++) {
                        if (prev != stored->decks[0].cards[pos]) {
                            entry++;
                            deck->entries[entry].cardId = stored->decks[0].cards[pos];
                            deck->entries[entry].count = 1;
                        } else {
                            deck->entries[entry].count++;
                        }
                        prev = stored->decks[0].cards[pos];
                    }
                    deck->cursor = CARD_PANE_INACTIVE;
                    deck->count = entry + 1;
                    deck->top = 0;
                }
                list->mode = CARD_ACTION_NONE;
            }

            if (deck->count - 1 < deck->cursor) {
                deck->cursor = deck->count - 1;
            }

            drawCards = 0;
            if ((PadData[0].half_28 & CARD_PAD_PREVIEW) != 0 && exiting == 0) {
                drawCards = 1;
                if (ActiveCursol_4 != 0) {
                    pCnow_5 = deck;
                    initVec[0] = -CARD_PREVIEW_OFFSET_X;
                    initVec[1] = CARD_PREVIEW_OFFSET_Y;
                    initVec[2] = CARD_PREVIEW_OFFSET_Z;
                    initVec[3] = 1.0f;
                    xglMatrixUnit(cursor);
                    xglMatrixTrans(cursor, cursor, initVec);
                    cardId = pCnow_5->entries[pCnow_5->top + pCnow_5->cursor].cardId;
                    CardDisp(&cursor, cardId, work->ownedCount[cardId] != 0, 1);
                }
            }

            if (drawCards == 0 || ActiveCursol_4 == 0) {
                CardDeckListDisp(list);
            }
            CardDeckDisp(deck, work->definitions);

            if (ActiveCursol_4 == 0 && exiting == 0) {
                xglFontPrint(0, 0, 0, "\x19\x03");
                xglFontPrint(CARD_PROMPT_STATE_X, CARD_PROMPT_STATE_Y, CARD_TEXT_COLOR, "");
                if (title != 0) {
                    xglFontPrint(CARD_PROMPT_X, CARD_PROMPT_TITLE_Y, CARD_TEXT_COLOR, title);
                }
                xglFontPrint(CARD_PROMPT_X, CARD_PROMPT_MESSAGE_Y, CARD_TEXT_COLOR, "\x0b\x19\x03\r\0Select deck to save in.");
                xglFontPrint(CARD_PROMPT_X, CARD_PROMPT_BUTTONS_Y, CARD_TEXT_COLOR, CARD_PROMPT_ACCEPT_CANCEL);
                xglFontPrint(CARD_PROMPT_STATE_X, CARD_PROMPT_STATE_Y, CARD_TEXT_COLOR, "\x0b");
                pressed = work->command.input[0].pressed;
                if ((pressed & PAD_CIRCLE) != 0) {
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                    work->command.step++;
                } else if ((pressed & PAD_CROSS) != 0) {
                    xglSoundEffectNormalID(SE_CANCEL, 0);
                    result = -1;
                }
            }
        break;
    /* Step 1: confirm the highlighted slot to save into. */
    case CARD_DECK_STEP_CONFIRM:
            CardDeckListDisp(list);
            CardDeckDisp(deck, work->definitions);
            if (exiting != 0) {
                break;
            }
            xglFontPrint(0, 0, 0, "\x19\x03");
            xglFontPrint(CARD_PROMPT_STATE_X, CARD_PROMPT_STATE_Y, CARD_TEXT_COLOR, "");
            xglFontPrint(CARD_PROMPT_X, CARD_PROMPT_MESSAGE_Y, CARD_TEXT_COLOR, "\x0b\x19\x03\r\0Is this deck okay?");
            xglFontPrint(CARD_PROMPT_X, CARD_PROMPT_BUTTONS_Y, CARD_TEXT_COLOR, CARD_PROMPT_ACCEPT_CANCEL);
            xglFontPrint(CARD_PROMPT_STATE_X, CARD_PROMPT_STATE_Y, CARD_TEXT_COLOR, "\x0b");
            pressed = work->command.input[0].pressed;
            if ((pressed & PAD_CIRCLE) != 0) {
                xglSoundEffectNormalID(SE_DECIDE, 0);
                result = (list->cursor & CARD_CURSOR_INDEX_MASK) + 1;
            } else if ((pressed & PAD_CROSS) != 0) {
                xglSoundEffectNormalID(SE_CANCEL, 0);
                work->command.step--;
            }
        break;
    }
    return result;
}

/*
 * Deck-load screen that reads the selected deck from the EnemyDeckLst table
 * instead of the save data: step 0 browses the five slots (list) and shows
 * the selected deck in the deck window (deck); step 1 asks for confirmation.
 * Returns the chosen slot + 1, or -1 when cancelled (0 otherwise).
 */
int CardLoadDeckProc2(int exiting, CardListWindow *list, CardListWindow *deck, CardGameWork *work, const char *title)
{
    int drawCards;
    int result;
    float initVec[4];
    float cursor[4][4];
    float vecA[4];
    float vecB[4];
    s16 cardId;
    int pos;
    int entry;
    int prev;
    int slot;
    int pressed;

    work->helpTopic = CARD_HELP_PAGE_SELECTING_DECKS;
    if (ActiveCursol_6 != 0) {
        pCnow_7 = deck;
        list->cursor |= CARD_PANE_INACTIVE;
        deck->cursor &= CARD_CURSOR_INDEX_MASK;
    } else {
        pCnow_7 = list;
        list->cursor &= CARD_CURSOR_INDEX_MASK;
        deck->cursor |= CARD_PANE_INACTIVE;
    }

    result = 0;
    switch (work->command.step) {
    case CARD_DECK_STEP_SELECT:
            drawCards = 0;
            if ((PadData[0].half_28 & CARD_PAD_PREVIEW) != 0 && exiting == 0 && ActiveCursol_6 != 0) {
                drawCards = 1;
                pCnow_7 = deck;
                initVec[0] = -CARD_PREVIEW_OFFSET_X;
                initVec[1] = CARD_PREVIEW_OFFSET_Y;
                initVec[2] = CARD_PREVIEW_OFFSET_Z;
                initVec[3] = 1.0f;
                xglMatrixUnit(cursor);
                xglMatrixTrans(cursor, cursor, initVec);
                cardId = pCnow_7->entries[pCnow_7->top + pCnow_7->cursor].cardId;
                CardDisp(&cursor, cardId, work->ownedCount[cardId] != 0, 1);
            }

            if (drawCards == 0 || ActiveCursol_6 == 0) {
                CardDeckListDisp(list);
            }
            CardDeckDisp(deck, work->definitions);

            if (ActiveCursol_6 == 0) {
                if (exiting == 0 && drawCards == 0) {
                    if (title != 0) {
                        xglFontPrint(0, 0, 0, "\x19\x03");
                        xglFontPrint(CARD_PROMPT_X, CARD_PROMPT_TITLE_Y, CARD_TEXT_COLOR, title);
                    }
                    xglFontPrint(CARD_PROMPT_STATE_X, CARD_PROMPT_STATE_Y, CARD_TEXT_COLOR, "");
                    xglFontPrint(CARD_PROMPT_X, CARD_PROMPT_MESSAGE_Y, CARD_TEXT_COLOR, "\x0b\x19\x03\r\0Select deck to load.");
                    xglFontPrint(CARD_PROMPT_X, CARD_PROMPT_BUTTONS_Y, CARD_TEXT_COLOR, CARD_PROMPT_ACCEPT_CANCEL);
                    xglFontPrint(CARD_PROMPT_STATE_X, CARD_PROMPT_STATE_Y, CARD_TEXT_COLOR, "\x0b");
                    pressed = work->command.input[0].pressed;
                    if ((pressed & PAD_CIRCLE) != 0) {
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                        work->command.step++;
                    } else if ((pressed & PAD_CROSS) != 0) {
                        xglSoundEffectNormalID(SE_CANCEL, 0);
                        result = -1;
                    }
                }
            } else {
                vecA[0] = 0.0f;
                vecA[1] = CARD_SELECTED_OFFSET_Y;
                vecA[2] = CARD_SELECTED_OFFSET_Z;
                vecA[3] = 1.0f;
                vecB[0] = vecB[1] = CARD_SELECTED_SCALE;
                vecB[2] = vecB[3] = 1.0f;
                xglMatrixUnit(cursor);
                xglMatrixTrans(cursor, cursor, vecA);
                xglMatrixScale(cursor, cursor, vecB);
                cardId = pCnow_7->entries[pCnow_7->top + pCnow_7->cursor].cardId;
                CardDisp(&cursor, cardId, work->ownedCount[cardId] != 0, exiting == 0);
            }

            if (exiting == 0) {
                if ((PadData[0].directions & PAD_UP) != 0) {
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                    pCnow_7->mode = CARD_ACTION_SCROLL_UP;
                } else if ((PadData[0].directions & PAD_DOWN) != 0) {
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                    pCnow_7->mode = CARD_ACTION_SCROLL_DOWN;
                }
                if ((PadData[0].directions & PAD_RIGHT) != 0) {
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                    if (CardGetDeckAllCnt(deck) != 0) {
                        ActiveCursol_6 = 1;
                        list->cursor |= CARD_PANE_INACTIVE;
                        deck->cursor &= CARD_CURSOR_INDEX_MASK;
                    }
                } else if ((PadData[0].directions & PAD_LEFT) != 0) {
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                    ActiveCursol_6 = 0;
                    list->cursor &= CARD_CURSOR_INDEX_MASK;
                    deck->cursor |= CARD_PANE_INACTIVE;
                }
            }

            if (deck->mode != CARD_ACTION_NONE) {
                if (deck->mode == CARD_ACTION_SCROLL_UP || deck->mode == CARD_ACTION_SCROLL_DOWN) {
                    if (deck->mode < 0) {
                        if (deck->top > 0) {
                            if (deck->cursor < CARD_SCROLL_MARGIN) {
                                deck->top = deck->top - 1;
                            } else {
                                deck->cursor = deck->cursor - 1;
                                if (deck->cursor < 0) {
                                    deck->cursor = 0;
                                }
                            }
                        } else if (deck->cursor > 0) {
                            deck->cursor = deck->cursor - 1;
                        }
                    } else {
                        if (deck->top + deck->rows < deck->count) {
                            if (deck->cursor >= deck->rows - CARD_SCROLL_MARGIN) {
                                deck->top = deck->top + 1;
                            } else {
                                deck->cursor = deck->cursor + 1;
                            }
                        } else if (deck->cursor < deck->rows - 1 && deck->cursor < deck->count - 1
                                   && deck->cursor < deck->shownCount) {
                            deck->cursor = deck->cursor + 1;
                        }
                    }
                }
                deck->mode = CARD_ACTION_NONE;
            }

            /* Slot list: scroll, then show the highlighted slot's cards in the deck window. */
            if (list->mode != CARD_ACTION_NONE) {
                list->count = CARD_DECK_SLOT_COUNT;
                if (list->mode == CARD_ACTION_SCROLL_UP || list->mode == CARD_ACTION_SCROLL_DOWN) {
                    if (list->mode < 0) {
                        if (list->top > 0) {
                            if (list->cursor < CARD_SCROLL_MARGIN) {
                                list->top = list->top - 1;
                            } else {
                                list->cursor = list->cursor - 1;
                                if (list->cursor < 0) {
                                    list->cursor = 0;
                                }
                            }
                        } else if (list->cursor > 0) {
                            list->cursor = list->cursor - 1;
                        }
                    } else {
                        if (list->top + list->rows < list->count) {
                            if (list->cursor >= list->rows - CARD_SCROLL_MARGIN) {
                                list->top = list->top + 1;
                            } else {
                                list->cursor = list->cursor + 1;
                            }
                        } else if (list->cursor < list->rows - 1 && list->cursor < CARD_DECK_SLOT_COUNT - 1) {
                            list->cursor = list->cursor + 1;
                        }
                    }
                    slot = list->top + (list->cursor & CARD_CURSOR_INDEX_MASK);
                    prev = -1;
                    entry = -1;
                    for (pos = 0; pos < CARD_DECK_SIZE; pos++) {
                        if (prev != EnemyDeckLst[slot][pos]) {
                            entry++;
                            deck->entries[entry].cardId = EnemyDeckLst[slot][pos];
                            deck->entries[entry].count = 1;
                        } else {
                            deck->entries[entry].count++;
                        }
                        prev = EnemyDeckLst[slot][pos];
                    }
                    deck->cursor = CARD_PANE_INACTIVE;
                    deck->count = entry + 1;
                    deck->top = 0;
                }
                list->mode = CARD_ACTION_NONE;
            }

            if (deck->count - 1 < deck->cursor) {
                deck->cursor = deck->count - 1;
            }
        break;
    /* Step 1: confirm the highlighted slot. */
    case CARD_DECK_STEP_CONFIRM:
            CardDeckListDisp(list);
            CardDeckDisp(deck, work->definitions);
            if (exiting != 0) {
                break;
            }
            xglFontPrint(0, 0, 0, "\x19\x03");
            xglFontPrint(CARD_PROMPT_STATE_X, CARD_PROMPT_STATE_Y, CARD_TEXT_COLOR, "");
            xglFontPrint(CARD_PROMPT_X, CARD_PROMPT_MESSAGE_Y, CARD_TEXT_COLOR, "\x0b\x19\x03\r\0Is this deck okay?");
            xglFontPrint(CARD_PROMPT_X, CARD_PROMPT_BUTTONS_Y, CARD_TEXT_COLOR, CARD_PROMPT_ACCEPT_CANCEL);
            xglFontPrint(CARD_PROMPT_STATE_X, CARD_PROMPT_STATE_Y, CARD_TEXT_COLOR, "\x0b");
            pressed = work->command.input[0].pressed;
            if ((pressed & PAD_CIRCLE) != 0) {
                xglSoundEffectNormalID(SE_DECIDE, 0);
                result = (list->cursor & CARD_CURSOR_INDEX_MASK) + 1;
            } else if ((pressed & PAD_CROSS) != 0) {
                xglSoundEffectNormalID(SE_CANCEL, 0);
                work->command.step--;
            }
        break;
    }
    return result;
}

static u8 TypeMaskLst[4] = { 7, 1, 2, 4 };

static u8 AttriMaskLst[7] = { 127, 1, 2, 16, 4, 8, 96 };

static int ActiveCursol_0 = 0;

static int ActiveCursol_2 = 0;

static int ActiveCursol_4 = 0;

static int ActiveCursol_6 = 0;

char ZenkakuListB[20] = "0 1 2 3 4 5 6 7 8 9 ";

char CardConvertNameLst[20][24] = {
    "\031\003PM Card A",
    "\031\003PM Card B",
    "\031\003PM Card C",
    "\031\003PM Card D",
    "\031\003PM Card E",
    "\031\003PM Card F",
    "\031\003PM Card G",
    "\031\003PM Card H",
    "\031\003PM Card I",
    "\031\003PM Card J",
    "\031\003PM Card K",
    "\031\003PM Card L",
    "\031\003PM Card M",
    "\031\003PM Card N",
    "\031\003PM Card O",
    "\031\003PM Card P",
    "\031\003PM Card Q",
    "\031\003Starter Set",
    "\031\003Card Pack #821",
    "\031\003Card Pack #822"
};

unsigned char ST1RariLst[6] = { 0, 2, 3, 4, 5, 6 };

unsigned char ST1CardLst[39] = { 16, 16, 17, 17, 18, 18, 19, 19, 22, 23, 23, 24, 25, 25, 36, 36, 36, 33, 33, 33, 43, 48, 49, 49, 49, 50, 50, 50, 83, 83, 83, 84, 84, 84, 91, 91, 91, 111, 111 };

unsigned char BS1RariLst[16] = { 0, 2, 3, 4, 5, 6, 7, 44, 59, 65, 74, 87, 101, 107, 115, 132 };

unsigned char BS1UncoLst[23] = { 15, 22, 26, 27, 28, 31, 33, 39, 46, 54, 55, 56, 60, 75, 86, 89, 92, 97, 98, 100, 109, 117, 129 };

unsigned char BS1ComoLst[44] = { 16, 17, 18, 19, 20, 21, 23, 24, 25, 36, 42, 43, 47, 48, 49, 50, 53, 61, 62, 67, 69, 83, 84, 85, 88, 91, 96, 103, 105, 106, 108, 111, 116, 120, 121, 124, 126, 130, 136, 138, 140, 145, 146, 147 };

unsigned char BS2RariLst[20] = { 9, 10, 12, 14, 32, 40, 45, 64, 76, 77, 78, 79, 80, 81, 82, 93, 94, 99, 110, 127 };

unsigned char BS2UncoLst[9] = { 37, 38, 41, 57, 66, 125, 131, 134, 119 };

unsigned char BS2ComoLst[17] = { 51, 63, 68, 70, 71, 72, 73, 95, 104, 122, 123, 133, 135, 137, 90, 112, 114 };

static CardListWindow * pCnow_1;

static CardListWindow * pCnow_3;

static CardListWindow * pCnow_5;

static CardListWindow * pCnow_7;
