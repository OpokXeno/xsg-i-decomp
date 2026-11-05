/*
 * OV10 original TU 4: 0x00a19a20..0x00a1c1b8 (5 functions)
 */
#include "common.h"

char ZenkakuListA[20] = "\xA1\xA1" "1 2 3 4 5 6 7 8 9 ";
#include "shared.h"

/*
 * PadData halfwords used by the deck screens. The bit positions follow the
 * standard PS2 pad layout; the on-screen prompts show circle = accept and
 * cross = cancel.
 */
#define CARD_PAD_ATTR_PREV 0x0001      /* L2 */
#define CARD_PAD_ATTR_NEXT 0x0002      /* R2 */
#define CARD_PAD_TYPE_PREV 0x0004      /* L1 */
#define CARD_PAD_TYPE_NEXT 0x0008      /* R1 */
#define CARD_PAD_PREVIEW 0x0010        /* triangle, held: show the selected card large */
#define CARD_PAD_ACCEPT 0x0020         /* circle */
#define CARD_PAD_CANCEL 0x0040         /* cross */

/* D-pad bits: UP and DOWN scroll the focused list, RIGHT and LEFT move focus
 * between the list and the deck (RIGHT/LEFT as tested by CardOpenProc, see
 * ov10/cgp.h). */
#define CARD_PAD_UP 0x1000
#define CARD_PAD_RIGHT 0x2000
#define CARD_PAD_DOWN 0x4000
#define CARD_PAD_LEFT 0x8000

/* Sound effect ids passed to xglSoundEffectNormalID by the deck screens. */
#define CARD_SE_ACCEPT 1
#define CARD_SE_CANCEL 2
#define CARD_SE_CURSOR 3

/* Accept/cancel button prompt line (control codes position and colour the circle/cross icons). */
#define CARD_PROMPT_ACCEPT_CANCEL \
    "\r\x04\x0c\x80\x40\x40\xa1\xfb \x0b\x19\x03\r\0Accept\xa1\xa1\r\x04\x0c\x40\x40\x80\xa1\xdf \x0b\x19\x03\r\0Cancel"

/*
 * helpTopic ids the deck screens store in CardGameWork.helpTopic (+0x59BF, see
 * ov10/cgp.h) on entry: CardMakeDeckProc `li v0,55` at 0x00a19aac, CardLoadDeckProc
 * `li v0,58` at 0x00a1aa7c, CardSaveDeckProc `li v0,56` at 0x00a1b284 and
 * CardLoadDeckProc2 `li v0,58` at 0x00a1b9b4 (the same topic as CardLoadDeckProc).
 */
#define CARD_HELP_TOPIC_MAKE_DECK 55
#define CARD_HELP_TOPIC_SAVE_DECK 56
#define CARD_HELP_TOPIC_LOAD_DECK 58

/* Rows kept between the cursor and the window edge before the list scrolls (`slti v0,v1,2` and
 * windowRows - 2 in the scroll steps). */
#define CARD_SCROLL_MARGIN 2

/* Render level argument of every nmlModelSetRenderLevel call in CardMakeDeckProc. */
#define CARD_MODEL_RENDER_LEVEL 2

/*
 * TypeMaskLst entries (card type bits) and the AttriMaskLst entries the type1/type2 filter
 * icons are chosen by in CardMakeDeckProc; the attribute masks are named by the icon they
 * select (type2_1..type2_6), their element meaning is not evidenced here.
 */
#define CARD_TYPE_MASK_B 0x01          /* icon type1_B */
#define CARD_TYPE_MASK_E 0x02          /* icon type1_E */
#define CARD_TYPE_MASK_S 0x04          /* icon type1_S */
#define CARD_ATTR_MASK_1 0x01
#define CARD_ATTR_MASK_2 0x02
#define CARD_ATTR_MASK_3 0x08
#define CARD_ATTR_MASK_4 0x04
#define CARD_ATTR_MASK_5 0x10
#define CARD_ATTR_MASK_6 0x60          /* two bits: the icon is chosen only on the combined mask */

/* Cursor bit set while a window is not the focused pane. */
#define CARD_PANE_INACTIVE 0x80

/* Cursor bits that hold the row index. */
#define CARD_CURSOR_INDEX_MASK 0x7f

/* A deck holds 40 cards, at most 3 copies of one card (CardDeckChk). */
#define CARD_DECK_SIZE 40
#define CARD_MAX_COPIES 3

/* The stored-deck list shows five slots. */
#define CARD_DECK_SLOT_COUNT 5

/* Entries of TypeMaskLst and AttriMaskLst (the symbol sizes in config/symbols/ov10.txt). */
#define CARD_TYPE_FILTER_COUNT 4
#define CARD_ATTR_FILTER_COUNT 7

/*
 * CardDeckListWindow.action requests. Scrolling is -1 (up) / 1 (down); the deck
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
 * deck count are 0x400 apart, indexed by the digit.
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
#define CARD_TEXTURE_TYPE1_B 0x01908000
#define CARD_TEXTURE_TYPE1_E 0x01918100
#define CARD_TEXTURE_TYPE1_S 0x01928200
#define CARD_TEXTURE_TYPE2_0 0x01938300
#define CARD_DIGIT_MODEL_BASE 0x01C8A400
#define CARD_DIGIT_MODEL_SHIFT 10
#define CARD_DIGIT_TEXTURE 0x01C990A0

#include "ov10/cgp.h"

static int ActiveCursol_0;
static int ActiveCursol_2;
static int ActiveCursol_4;
static int ActiveCursol_6;

/* The pad record's halfwords at +0x28 (buttons held), +0x2A (pressed this frame) and +0x2C (d-pad). */
typedef struct CardPadState {
    u8 unmodeled_00[0x28];
    u16 held;
    u16 pressed;
    u16 directions;
} CardPadState;

extern CardPadState PadData;

/* One entry of a deck or deck-list window: a card id and its copy count. */
typedef struct CardDeckEntry {
    s16 cardId;                    /* +0x00 */
    s16 copies;                    /* +0x02 */
} CardDeckEntry;

/*
 * Card list window (filter list) or the deck window of the deck screens: the
 * cursor/scroll pair selects the row.
 */
typedef struct CardDeckListWindow {
    u8 unmodeled_00[2];
    s8 typeFilter;                 /* +0x02: index into TypeMaskLst (list only) */
    s8 attrFilter;                 /* +0x03: index into AttriMaskLst (list only) */
    s8 action;                     /* +0x04: pending CARD_ACTION_* request */
    s8 cursor;                     /* +0x05 */
    u8 unmodeled_06[6];
    s16 scroll;                    /* +0x0C */
    s16 cursorLimit;               /* +0x0E */
    s16 windowRows;                /* +0x10 */
    s16 rowCount;                  /* +0x12 */
    CardDeckEntry entries[CARD_DECK_SIZE]; /* +0x14 */
} CardDeckListWindow;

static CardDeckListWindow *pCnow_1;
static CardDeckListWindow *pCnow_3;
static CardDeckListWindow *pCnow_5;
static CardDeckListWindow *pCnow_7;
extern unsigned char *EnemyDeckLst[];
static u8 TypeMaskLst[4];
static u8 AttriMaskLst[7];

void xglMatrixTrans(Matrix4 destination, Matrix4 source, const float translation[4]);
void xglMatrixScale(Matrix4 destination, Matrix4 source, const float scale[4]);
void xglSoundEffectNormalID(int id, int unused);
int CardGetDeckAllCnt(void *deck);
void CardDisp(float matrix[4][4], int cardId, int hasCopies, int active);
void CardDeckListDisp(CardDeckListWindow *list);
void CardDeckDisp(void *deck, void *definitions);
void CardListDisp(void *list, CardGameWork *work);
int CardGetDeckCardCnt(int cardId, void *deck);
int CardAddDeck(void *deck, int cardId);
void CardDelDeck(void *deck, int cardId);
int CardMakeDispList(void *rows, u16 mask, void *definitions, int cardCount);
void nmlModelSetPlace(float matrix[4][4]);
void nmlModelSetTexture(int texture);
void nmlModelSetRenderLevel(int level);
void nmlModelEntryCard(int entry);

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
 * Deck-builder screen. The list window (list) holds the cards matching the
 * type/attribute filter, the deck window (deck) the cards chosen so far.
 * Pad input edits the filters, moves the cursor, switches focus between the
 * two windows and adds or removes the selected card; the function then draws
 * the filter icons, the selected card and the card number / deck count digits.
 * `exiting` is nonzero while the caller shows the exit prompt (the screen is
 * only drawn). Returns a CARD_DECK_MSG_* code when an edit is refused.
 */
int CardMakeDeckProc(int exiting, CardDeckListWindow *list, CardDeckListWindow *deck, CardGameWork *work)
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
    int texture;
    int entry;
    int model;

    /* Signed view of the owned-copy table: the original loads it with lh. */
    ownedCount = (s16 *) work->ownedCount;
    work->helpTopic = CARD_HELP_TOPIC_MAKE_DECK;

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
    offset[1] = -0.3932f;
    offset[2] = 0.1f;
    offset[3] = 1.0f;
    scale[1] = 0.335f;
    scale[0] = 0.335f;
    scale[3] = 1.0f;
    scale[2] = 1.0f;
    xglMatrixUnit(matrix);
    xglMatrixTrans(matrix, matrix, offset);
    xglMatrixScale(matrix, matrix, scale);

    if (exiting == 0) {
        if (PadData.pressed & CARD_PAD_TYPE_NEXT) {
            xglSoundEffectNormalID(CARD_SE_CURSOR, 0);
            list->typeFilter++;
            list->action = CARD_ACTION_REFRESH;
        }
        if (PadData.pressed & CARD_PAD_TYPE_PREV) {
            xglSoundEffectNormalID(CARD_SE_CURSOR, 0);
            list->typeFilter--;
            list->action = CARD_ACTION_REFRESH;
        }
        if (PadData.pressed & CARD_PAD_ATTR_NEXT) {
            xglSoundEffectNormalID(CARD_SE_CURSOR, 0);
            list->attrFilter++;
            list->action = CARD_ACTION_REFRESH;
        }
        if (PadData.pressed & CARD_PAD_ATTR_PREV) {
            xglSoundEffectNormalID(CARD_SE_CURSOR, 0);
            list->attrFilter--;
            list->action = CARD_ACTION_REFRESH;
        }
        if (PadData.pressed & CARD_PAD_ACCEPT) {
            xglSoundEffectNormalID(CARD_SE_ACCEPT, 0);
            pCnow_1->action = CARD_ACTION_ADD;
        }
        if (PadData.pressed & CARD_PAD_CANCEL) {
            xglSoundEffectNormalID(CARD_SE_CANCEL, 0);
            pCnow_1->action = CARD_ACTION_REMOVE;
        }
        if (PadData.directions & CARD_PAD_UP) {
            xglSoundEffectNormalID(CARD_SE_CURSOR, 0);
            pCnow_1->action = CARD_ACTION_SCROLL_UP;
        }
        if (PadData.directions & CARD_PAD_DOWN) {
            xglSoundEffectNormalID(CARD_SE_CURSOR, 0);
            pCnow_1->action = CARD_ACTION_SCROLL_DOWN;
        }
        if (PadData.directions & CARD_PAD_RIGHT) {
            xglSoundEffectNormalID(CARD_SE_CURSOR, 0);
            if (CardGetDeckAllCnt(deck) != 0) {
                ActiveCursol_0 = 1;
                list->cursor |= CARD_PANE_INACTIVE;
                deck->cursor &= CARD_CURSOR_INDEX_MASK;
            }
        }
        if (PadData.directions & CARD_PAD_LEFT) {
            xglSoundEffectNormalID(CARD_SE_CURSOR, 0);
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

    if (deck->action != CARD_ACTION_NONE) {
        switch (deck->action) {
        case CARD_ACTION_SCROLL_DOWN:
        case CARD_ACTION_SCROLL_UP:
            if (deck->action < 0) {
                if (deck->scroll > 0) {
                    if (deck->cursor < CARD_SCROLL_MARGIN) {
                        deck->scroll = deck->scroll - 1;
                    } else {
                        deck->cursor = deck->cursor - 1;
                        if (deck->cursor < 0) {
                            deck->cursor = 0;
                        }
                    }
                } else if (deck->cursor > 0) {
                    deck->cursor = deck->cursor - 1;
                }
            } else if (deck->rowCount > deck->scroll + deck->windowRows) {
                if (deck->cursor >= deck->windowRows - CARD_SCROLL_MARGIN) {
                    deck->scroll = deck->scroll + 1;
                } else {
                    deck->cursor = deck->cursor + 1;
                }
            } else {
                if (deck->cursor < deck->windowRows - 1
                    && deck->cursor < deck->rowCount - 1
                    && deck->cursor < deck->cursorLimit) {
                    deck->cursor = deck->cursor + 1;
                }
            }
            break;
        case CARD_ACTION_ADD:
            if (CardGetDeckAllCnt(deck) < CARD_DECK_SIZE) {
                int id = deck->entries[deck->scroll + deck->cursor].cardId;

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
            CardDelDeck(deck, deck->entries[deck->scroll + deck->cursor].cardId);
            if (CardGetDeckAllCnt(deck) == 0) {
                ActiveCursol_0 = 0;
                list->cursor &= CARD_CURSOR_INDEX_MASK;
                deck->cursor |= CARD_PANE_INACTIVE;
            }
            break;
        }
        deck->action = CARD_ACTION_NONE;
    } else if (list->action != CARD_ACTION_NONE) {
        list->rowCount = CardMakeDispList(list->entries,
                                          AttriMaskLst[list->attrFilter]
                                              | (TypeMaskLst[list->typeFilter] << 8),
                                          work->definitions, work->cardCount);
        switch (list->action) {
        case 0:
            break;
        case CARD_ACTION_SCROLL_DOWN:
        case CARD_ACTION_SCROLL_UP:
            if (list->action < 0) {
                if (list->scroll > 0) {
                    if (list->cursor < CARD_SCROLL_MARGIN) {
                        list->scroll = list->scroll - 1;
                    } else {
                        list->cursor = list->cursor - 1;
                        if (list->cursor < 0) {
                            list->cursor = 0;
                        }
                    }
                } else if (list->cursor > 0) {
                    list->cursor = list->cursor - 1;
                }
            } else if (list->scroll + list->windowRows < list->rowCount) {
                if (list->cursor >= list->windowRows - CARD_SCROLL_MARGIN) {
                    list->scroll = list->scroll + 1;
                } else {
                    list->cursor = list->cursor + 1;
                }
            } else if (list->cursor < list->windowRows - 1
                       && list->cursor < list->rowCount - 1) {
                list->cursor = list->cursor + 1;
            }
            break;
        case CARD_ACTION_REFRESH:
            list->cursor = 0;
            list->scroll = 0;
            break;
        case CARD_ACTION_ADD:
            result = CARD_DECK_MSG_DECK_FULL;
            if (CardGetDeckAllCnt(deck) < CARD_DECK_SIZE) {
                int id = list->entries[list->scroll + list->cursor].cardId;
                total = CardGetDeckCardCnt(id, deck);

                if (total < CARD_MAX_COPIES) {
                    if (total < ownedCount[id]) {
                        int newIdx = CardAddDeck(deck, id);
                        if (newIdx - deck->windowRows + 1 < 0) {
                            deck->scroll = 0;
                        } else {
                            deck->scroll = newIdx - deck->windowRows + 1;
                        }
                        /* Row within the window, with CARD_PANE_INACTIVE set (the list has focus). */
                        deck->cursor = (newIdx % deck->windowRows) - CARD_PANE_INACTIVE;
                        result = 0;
                    } else {
                        result = (ownedCount[id] == 0) ? CARD_DECK_MSG_NOT_OWNED : CARD_DECK_MSG_COPIES_USED;
                    }
                }
            }
            break;
        case CARD_ACTION_REMOVE:
            CardDelDeck(deck, list->entries[list->scroll + list->cursor].cardId);
            break;
        }
        list->action = CARD_ACTION_NONE;
    }

    if (deck->rowCount - 1 < deck->cursor) {
        deck->cursor = deck->rowCount - 1;
    }
    if (deck->cursorLimit - 1 < deck->cursor) {
        deck->cursor = deck->cursorLimit - 1;
    }

    hovered = 0;
    if ((PadData.held & CARD_PAD_PREVIEW) && exiting == 0) {
        hovered = 1;
        if (ActiveCursol_0 != 0) {
            offset[0] = -1.15f;
            pCnow_1 = deck;
        } else {
            offset[0] = 1.15f;
            pCnow_1 = list;
        }
        offset[1] = 0.45f;
        offset[2] = 0.2f;
        offset[3] = 1.0f;
        xglMatrixUnit(pinMatrix);
        xglMatrixTrans(pinMatrix, pinMatrix, offset);
        {
            CardDeckListWindow *sel = pCnow_1;
            int id = sel->entries[sel->scroll + sel->cursor].cardId;
            CardDisp(pinMatrix, id, (s16) work->ownedCount[id] != 0, exiting == 0);
        }
    }
    if (hovered == 0 || ActiveCursol_0 == 0) {
        CardListDisp(list, work);
    }
    if (hovered == 0 || ActiveCursol_0 != 0) {
        CardDeckDisp(deck, work->definitions);
    }

    offset[0] = offset[2] = 0.0f;
    offset[1] = 1.292f;
    offset[3] = 1.0f;
    xglMatrixUnit(filterMatrix);
    xglMatrixTrans(filterMatrix, filterMatrix, offset);
    nmlModelSetPlace(filterMatrix);
    typeMask = TypeMaskLst[list->typeFilter];
    if (typeMask == CARD_TYPE_MASK_B) {
        texture = CARD_TEXTURE_TYPE1_B;
        entry = CARD_MODEL_TYPE1_B;
    } else if (typeMask == CARD_TYPE_MASK_E) {
        texture = CARD_TEXTURE_TYPE1_E;
        entry = CARD_MODEL_TYPE1_E;
    } else if (typeMask == CARD_TYPE_MASK_S) {
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
    nmlModelSetPlace(filterMatrix);
    /*
     * Attribute masks CARD_ATTR_MASK_1..6 (1, 2, 8, 4, 0x10, 0x60) select type2_1..type2_6; any
     * other value (no filter) shows type2_0. The texture assignments in the
     * last two arms are never read: with a single assignment per arm the
     * compiler turns the final test into a conditional move (the original
     * branches), so both arms keep their second statement.
     */
    attr = AttriMaskLst[list->attrFilter];
    if (attr != CARD_ATTR_MASK_1) {
        entry = CARD_MODEL_TYPE2_2;
        if (attr != CARD_ATTR_MASK_2) {
            entry = CARD_MODEL_TYPE2_3;
            if (attr != CARD_ATTR_MASK_3) {
                entry = CARD_MODEL_TYPE2_4;
                if (attr != CARD_ATTR_MASK_4) {
                    entry = CARD_MODEL_TYPE2_5;
                    if (attr != CARD_ATTR_MASK_5) {
                        if (attr == CARD_ATTR_MASK_6) {
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
        CardDeckListWindow *sel = pCnow_1;
        int id = sel->entries[sel->scroll + sel->cursor].cardId;
        int flag;

        /* The flag is materialised with a branch; `exiting == 0` inline changes the code. */
        if (exiting) {
            flag = 0;
        } else {
            flag = 1;
        }
        CardDisp(matrix, id, (s16) work->ownedCount[id] != 0, flag);
    }

    total = list->entries[list->scroll + (list->cursor & CARD_CURSOR_INDEX_MASK)].cardId + 1;
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
        nmlModelSetPlace(matrix);
        nmlModelSetTexture(CARD_DIGIT_TEXTURE);
        nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
        nmlModelEntryCard(model);
    }
    digit = total % 100 / 10;
    if (total / 100 != 0 || digit != 0) {
        model = (digit << CARD_DIGIT_MODEL_SHIFT) + CARD_DIGIT_MODEL_BASE;
        offset[0] = -0.748f;
        xglMatrixTrans(matrix, digitMatrix, offset);
        nmlModelSetPlace(matrix);
        nmlModelSetTexture(CARD_DIGIT_TEXTURE);
        nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
        nmlModelEntryCard(model);
    }
    digit = total % 100 % 10;
    model = (digit << CARD_DIGIT_MODEL_SHIFT) + CARD_DIGIT_MODEL_BASE;
    offset[0] = -0.588f;
    xglMatrixTrans(matrix, digitMatrix, offset);
    nmlModelSetPlace(matrix);
    nmlModelSetTexture(CARD_DIGIT_TEXTURE);
    nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
    nmlModelEntryCard(model);

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
        nmlModelSetPlace(matrix);
        nmlModelSetTexture(CARD_DIGIT_TEXTURE);
        nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
        nmlModelEntryCard(entry);
    }
    digit = total % 100 / 10;
    if (total / 100 != 0 || digit != 0) {
        entry = (digit << CARD_DIGIT_MODEL_SHIFT) + CARD_DIGIT_MODEL_BASE;
        offset[0] = 2.095f;
        xglMatrixTrans(matrix, countMatrix, offset);
        nmlModelSetPlace(matrix);
        nmlModelSetTexture(CARD_DIGIT_TEXTURE);
        nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
        nmlModelEntryCard(entry);
    }
    digit = total % 100 % 10;
    entry = (digit << CARD_DIGIT_MODEL_SHIFT) + CARD_DIGIT_MODEL_BASE;
    offset[0] = 2.255f;
    xglMatrixTrans(matrix, countMatrix, offset);
    nmlModelSetPlace(matrix);
    nmlModelSetTexture(CARD_DIGIT_TEXTURE);
    nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
    nmlModelEntryCard(entry);

    return result;
}

/*
 * Deck-load screen: step 0 browses the five stored deck slots (list) and shows
 * the selected deck in the deck window (deck); step 1 asks for confirmation.
 * `exiting` is nonzero while the caller shows the exit prompt (the screen is
 * only drawn). Returns the chosen slot + 1, or -1 when cancelled (0 otherwise).
 */
int CardLoadDeckProc(int exiting, CardDeckListWindow *list, CardDeckListWindow *deck, CardGameWork *work, const char *title)
{
    int drawCards;
    int result;
    float initVec[4];
    float cursor[4][4];
    float vecA[4];
    float vecB[4];
    int cardId;
    int pos;
    int entry;
    int prev;
    CardSaveData *stored;
    int slot;
    int pressed;

    work->helpTopic = CARD_HELP_TOPIC_LOAD_DECK;
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
    case 0:
            drawCards = 0;
            if ((PadData.held & CARD_PAD_PREVIEW) != 0 && exiting == 0 && ActiveCursol_2 != 0) {
                drawCards = 1;
                pCnow_3 = deck;
                initVec[0] = -1.15f;
                initVec[1] = 0.45f;
                initVec[2] = 0.2f;
                initVec[3] = 1.0f;
                xglMatrixUnit(cursor);
                xglMatrixTrans(cursor, cursor, initVec);
                cardId = pCnow_3->entries[pCnow_3->scroll + pCnow_3->cursor].cardId;
                CardDisp(cursor, cardId, ((s16 *)work->ownedCount)[cardId] != 0, 1);
            }

            if (drawCards == 0 || ActiveCursol_2 == 0) {
                CardDeckListDisp(list);
            }
            CardDeckDisp(deck, work->definitions);

            if (ActiveCursol_2 == 0) {
                if (exiting == 0 && drawCards == 0) {
                    if (title != 0) {
                        xglFontPrint(24, 312, 0xfff0, title);
                    }
                    xglFontPrint(0, 0, 0, "\x19\x03");
                    xglFontPrint(120, 288, 0xfff0, "");
                    xglFontPrint(24, 336, 0xfff0, "\x0b\x19\x03\r\0Please select a deck to load.");
                    xglFontPrint(24, 360, 0xfff0, CARD_PROMPT_ACCEPT_CANCEL);
                    xglFontPrint(120, 288, 0xfff0, "\x0b");
                    pressed = work->command.input[0].pressed;
                    if ((pressed & CARD_PAD_ACCEPT) != 0) {
                        xglSoundEffectNormalID(CARD_SE_ACCEPT, 0);
                        work->command.step++;
                    } else if ((pressed & CARD_PAD_CANCEL) != 0) {
                        xglSoundEffectNormalID(CARD_SE_CANCEL, 0);
                        result = -1;
                    }
                }
            } else {
                vecA[0] = 0.0f;
                vecA[1] = -0.3932f;
                vecA[2] = 0.1f;
                vecA[3] = 1.0f;
                vecB[0] = vecB[1] = 0.335f;
                vecB[2] = vecB[3] = 1.0f;
                xglMatrixUnit(cursor);
                xglMatrixTrans(cursor, cursor, vecA);
                xglMatrixScale(cursor, cursor, vecB);
                cardId = pCnow_3->entries[pCnow_3->scroll + pCnow_3->cursor].cardId;
                CardDisp(cursor, cardId, ((s16 *)work->ownedCount)[cardId] != 0, exiting == 0);
            }

            if (exiting == 0) {
                if ((PadData.directions & CARD_PAD_UP) != 0) {
                    xglSoundEffectNormalID(CARD_SE_CURSOR, 0);
                    pCnow_3->action = CARD_ACTION_SCROLL_UP;
                } else if ((PadData.directions & CARD_PAD_DOWN) != 0) {
                    xglSoundEffectNormalID(CARD_SE_CURSOR, 0);
                    pCnow_3->action = CARD_ACTION_SCROLL_DOWN;
                }
                if ((PadData.directions & CARD_PAD_RIGHT) != 0) {
                    xglSoundEffectNormalID(CARD_SE_CURSOR, 0);
                    if (CardGetDeckAllCnt(deck) != 0) {
                        ActiveCursol_2 = 1;
                        list->cursor |= CARD_PANE_INACTIVE;
                        deck->cursor &= CARD_CURSOR_INDEX_MASK;
                    }
                } else if ((PadData.directions & CARD_PAD_LEFT) != 0) {
                    xglSoundEffectNormalID(CARD_SE_CURSOR, 0);
                    ActiveCursol_2 = 0;
                    list->cursor &= CARD_CURSOR_INDEX_MASK;
                    deck->cursor |= CARD_PANE_INACTIVE;
                }
            }

            if (deck->action != CARD_ACTION_NONE) {
                if (deck->action == CARD_ACTION_SCROLL_UP || deck->action == CARD_ACTION_SCROLL_DOWN) {
                    if (deck->action < 0) {
                        if (deck->scroll > 0) {
                            if (deck->cursor < CARD_SCROLL_MARGIN) {
                                deck->scroll = deck->scroll - 1;
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
                        if (deck->scroll + deck->windowRows < deck->rowCount) {
                            if (deck->cursor >= deck->windowRows - CARD_SCROLL_MARGIN) {
                                deck->scroll = deck->scroll + 1;
                            } else {
                                deck->cursor = deck->cursor + 1;
                            }
                        } else if (deck->cursor < deck->windowRows - 1 && deck->cursor < deck->rowCount - 1
                                   && deck->cursor < deck->cursorLimit) {
                            deck->cursor = deck->cursor + 1;
                        }
                    }
                }
                deck->action = CARD_ACTION_NONE;
            }

            if (list->action != CARD_ACTION_NONE) {
                list->rowCount = CARD_DECK_SLOT_COUNT;
                if (list->action == CARD_ACTION_SCROLL_UP || list->action == CARD_ACTION_SCROLL_DOWN) {
                    if (list->action < 0) {
                        if (list->scroll > 0) {
                            if (list->cursor < CARD_SCROLL_MARGIN) {
                                list->scroll = list->scroll - 1;
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
                        if (list->scroll + list->windowRows < list->rowCount) {
                            if (list->cursor >= list->windowRows - CARD_SCROLL_MARGIN) {
                                list->scroll = list->scroll + 1;
                            } else {
                                list->cursor = list->cursor + 1;
                            }
                        } else if (list->cursor < list->windowRows - 1 && list->cursor < CARD_DECK_SLOT_COUNT - 1) {
                            list->cursor = list->cursor + 1;
                        }
                    }
                    slot = list->scroll + (list->cursor & CARD_CURSOR_INDEX_MASK);
                    /* Stored deck slot: CardSaveData records 42 bytes apart (slot * 32 + slot * 10). */
                    stored = (CardSaveData *)((slot << 5) + (unsigned int)work->save + slot * 10);
                    prev = -1;
                    entry = -1;
                    for (pos = 0; pos < CARD_DECK_SIZE; pos++) {
                        if (prev != stored->deck[pos]) {
                            entry++;
                            deck->entries[entry].cardId = stored->deck[pos];
                            deck->entries[entry].copies = 1;
                        } else {
                            deck->entries[entry].copies++;
                        }
                        prev = stored->deck[pos];
                    }
                    deck->cursor = (s8)CARD_PANE_INACTIVE;
                    deck->rowCount = entry + 1;
                    deck->scroll = 0;
                }
                list->action = CARD_ACTION_NONE;
            }

            if ((s16)deck->rowCount - 1 < deck->cursor) {
                deck->cursor = deck->rowCount - 1;
            }
        break;
    case 1:
            CardDeckListDisp(list);
            CardDeckDisp(deck, work->definitions);
            if (exiting != 0) {
                break;
            }
            xglFontPrint(0, 0, 0, "\x19\x03");
            xglFontPrint(120, 288, 0xfff0, "");
            xglFontPrint(24, 336, 0xfff0, "\x0b\x19\x03\r\0Is this deck okay?");
            xglFontPrint(24, 360, 0xfff0, CARD_PROMPT_ACCEPT_CANCEL);
            xglFontPrint(120, 288, 0xfff0, "\x0b");
            pressed = work->command.input[0].pressed;
            if ((pressed & CARD_PAD_ACCEPT) != 0) {
                xglSoundEffectNormalID(CARD_SE_ACCEPT, 0);
                result = (list->cursor & CARD_CURSOR_INDEX_MASK) + 1;
            } else if ((pressed & CARD_PAD_CANCEL) != 0) {
                xglSoundEffectNormalID(CARD_SE_CANCEL, 0);
                work->command.step--;
            }
        break;
    }
    return result;
}

/*
 * Deck-save screen: step 0 browses the five stored deck slots (list) and shows
 * the selected deck in the deck window (deck); step 1 asks for confirmation.
 * `exiting` is nonzero while the caller shows the exit prompt (the screen is
 * only drawn). Returns the chosen slot + 1 to save into, or -1 when cancelled
 * (0 otherwise).
 */
int CardSaveDeckProc(int exiting, CardDeckListWindow *list, CardDeckListWindow *deck, CardGameWork *work, const char *title)
{
    int drawCards;
    int result;
    float initVec[4];
    float cursor[4][4];
    int cardId;
    int pos;
    int entry;
    int prev;
    CardSaveData *stored;
    int slot;
    int pressed;

    work->helpTopic = CARD_HELP_TOPIC_SAVE_DECK;
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
    case 0:
            if (exiting == 0) {
                if ((PadData.directions & CARD_PAD_UP) != 0) {
                    xglSoundEffectNormalID(CARD_SE_CURSOR, 0);
                    pCnow_5->action = CARD_ACTION_SCROLL_UP;
                }
                if ((PadData.directions & CARD_PAD_DOWN) != 0) {
                    xglSoundEffectNormalID(CARD_SE_CURSOR, 0);
                    pCnow_5->action = CARD_ACTION_SCROLL_DOWN;
                }
                if ((PadData.directions & CARD_PAD_RIGHT) != 0) {
                    xglSoundEffectNormalID(CARD_SE_CURSOR, 0);
                    if (CardGetDeckAllCnt(deck) != 0) {
                        ActiveCursol_4 = 1;
                        list->cursor |= CARD_PANE_INACTIVE;
                        deck->cursor &= CARD_CURSOR_INDEX_MASK;
                    }
                }
                if ((PadData.directions & CARD_PAD_LEFT) != 0) {
                    xglSoundEffectNormalID(CARD_SE_CURSOR, 0);
                    ActiveCursol_4 = 0;
                    list->cursor &= CARD_CURSOR_INDEX_MASK;
                    deck->cursor |= CARD_PANE_INACTIVE;
                }
            }

            if (deck->action != CARD_ACTION_NONE) {
                if (deck->action == CARD_ACTION_SCROLL_UP || deck->action == CARD_ACTION_SCROLL_DOWN) {
                    if (deck->action < 0) {
                        if (deck->scroll > 0) {
                            if (deck->cursor < CARD_SCROLL_MARGIN) {
                                deck->scroll = deck->scroll - 1;
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
                        if (deck->scroll + deck->windowRows < deck->rowCount) {
                            if (deck->cursor >= deck->windowRows - CARD_SCROLL_MARGIN) {
                                deck->scroll = deck->scroll + 1;
                            } else {
                                deck->cursor = deck->cursor + 1;
                            }
                        } else if (deck->cursor < deck->windowRows - 1 && deck->cursor < deck->rowCount - 1
                                   && deck->cursor < deck->cursorLimit) {
                            deck->cursor = deck->cursor + 1;
                        }
                    }
                }
                deck->action = CARD_ACTION_NONE;
            }

            if (list->action != CARD_ACTION_NONE) {
                list->rowCount = CARD_DECK_SLOT_COUNT;
                if (list->action == CARD_ACTION_SCROLL_UP || list->action == CARD_ACTION_SCROLL_DOWN) {
                    if (list->action < 0) {
                        if (list->scroll > 0) {
                            if (list->cursor < CARD_SCROLL_MARGIN) {
                                list->scroll = list->scroll - 1;
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
                        if (list->scroll + list->windowRows < list->rowCount) {
                            if (list->cursor >= list->windowRows - CARD_SCROLL_MARGIN) {
                                list->scroll = list->scroll + 1;
                            } else {
                                list->cursor = list->cursor + 1;
                            }
                        } else if (list->cursor < list->windowRows - 1 && list->cursor < CARD_DECK_SLOT_COUNT - 1) {
                            list->cursor = list->cursor + 1;
                        }
                    }
                    slot = list->scroll + (list->cursor & CARD_CURSOR_INDEX_MASK);
                    /* Stored deck slot: CardSaveData records 42 bytes apart (slot * 32 + slot * 10). */
                    stored = (CardSaveData *)((slot << 5) + (unsigned int)work->save + slot * 10);
                    prev = -1;
                    entry = -1;
                    for (pos = 0; pos < CARD_DECK_SIZE; pos++) {
                        if (prev != stored->deck[pos]) {
                            entry++;
                            deck->entries[entry].cardId = stored->deck[pos];
                            deck->entries[entry].copies = 1;
                        } else {
                            deck->entries[entry].copies++;
                        }
                        prev = stored->deck[pos];
                    }
                    deck->cursor = (s8)CARD_PANE_INACTIVE;
                    deck->rowCount = entry + 1;
                    deck->scroll = 0;
                }
                list->action = CARD_ACTION_NONE;
            }

            if ((s16)deck->rowCount - 1 < deck->cursor) {
                deck->cursor = deck->rowCount - 1;
            }

            drawCards = 0;
            if ((PadData.held & CARD_PAD_PREVIEW) != 0 && exiting == 0) {
                drawCards = 1;
                if (ActiveCursol_4 != 0) {
                    pCnow_5 = deck;
                    initVec[0] = -1.15f;
                    initVec[1] = 0.45f;
                    initVec[2] = 0.2f;
                    initVec[3] = 1.0f;
                    xglMatrixUnit(cursor);
                    xglMatrixTrans(cursor, cursor, initVec);
                    cardId = pCnow_5->entries[pCnow_5->scroll + pCnow_5->cursor].cardId;
                    CardDisp(cursor, cardId, ((s16 *)work->ownedCount)[cardId] != 0, 1);
                }
            }

            if (drawCards == 0 || ActiveCursol_4 == 0) {
                CardDeckListDisp(list);
            }
            CardDeckDisp(deck, work->definitions);

            if (ActiveCursol_4 == 0 && exiting == 0) {
                xglFontPrint(0, 0, 0, "\x19\x03");
                xglFontPrint(120, 288, 0xfff0, "");
                if (title != 0) {
                    xglFontPrint(24, 312, 0xfff0, title);
                }
                xglFontPrint(24, 336, 0xfff0, "\x0b\x19\x03\r\0Select deck to save in.");
                xglFontPrint(24, 360, 0xfff0, CARD_PROMPT_ACCEPT_CANCEL);
                xglFontPrint(120, 288, 0xfff0, "\x0b");
                pressed = work->command.input[0].pressed;
                if ((pressed & CARD_PAD_ACCEPT) != 0) {
                    xglSoundEffectNormalID(CARD_SE_ACCEPT, 0);
                    work->command.step++;
                } else if ((pressed & CARD_PAD_CANCEL) != 0) {
                    xglSoundEffectNormalID(CARD_SE_CANCEL, 0);
                    result = -1;
                }
            }
        break;
    case 1:
            CardDeckListDisp(list);
            CardDeckDisp(deck, work->definitions);
            if (exiting != 0) {
                break;
            }
            xglFontPrint(0, 0, 0, "\x19\x03");
            xglFontPrint(120, 288, 0xfff0, "");
            xglFontPrint(24, 336, 0xfff0, "\x0b\x19\x03\r\0Is this deck okay?");
            xglFontPrint(24, 360, 0xfff0, CARD_PROMPT_ACCEPT_CANCEL);
            xglFontPrint(120, 288, 0xfff0, "\x0b");
            pressed = work->command.input[0].pressed;
            if ((pressed & CARD_PAD_ACCEPT) != 0) {
                xglSoundEffectNormalID(CARD_SE_ACCEPT, 0);
                result = (list->cursor & CARD_CURSOR_INDEX_MASK) + 1;
            } else if ((pressed & CARD_PAD_CANCEL) != 0) {
                xglSoundEffectNormalID(CARD_SE_CANCEL, 0);
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
int CardLoadDeckProc2(int exiting, CardDeckListWindow *list, CardDeckListWindow *deck, CardGameWork *work, const char *title)
{
    int drawCards;
    int result;
    float initVec[4];
    float cursor[4][4];
    float vecA[4];
    float vecB[4];
    int cardId;
    int pos;
    int entry;
    int prev;
    int slot;
    int pressed;

    work->helpTopic = CARD_HELP_TOPIC_LOAD_DECK;
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
    case 0:
            drawCards = 0;
            if ((PadData.held & CARD_PAD_PREVIEW) != 0 && exiting == 0 && ActiveCursol_6 != 0) {
                drawCards = 1;
                pCnow_7 = deck;
                initVec[0] = -1.15f;
                initVec[1] = 0.45f;
                initVec[2] = 0.2f;
                initVec[3] = 1.0f;
                xglMatrixUnit(cursor);
                xglMatrixTrans(cursor, cursor, initVec);
                cardId = pCnow_7->entries[pCnow_7->scroll + pCnow_7->cursor].cardId;
                CardDisp(cursor, cardId, ((s16 *)work->ownedCount)[cardId] != 0, 1);
            }

            if (drawCards == 0 || ActiveCursol_6 == 0) {
                CardDeckListDisp(list);
            }
            CardDeckDisp(deck, work->definitions);

            if (ActiveCursol_6 == 0) {
                if (exiting == 0 && drawCards == 0) {
                    if (title != 0) {
                        xglFontPrint(0, 0, 0, "\x19\x03");
                        xglFontPrint(24, 312, 0xfff0, title);
                    }
                    xglFontPrint(120, 288, 0xfff0, "");
                    xglFontPrint(24, 336, 0xfff0, "\x0b\x19\x03\r\0Select deck to load.");
                    xglFontPrint(24, 360, 0xfff0, CARD_PROMPT_ACCEPT_CANCEL);
                    xglFontPrint(120, 288, 0xfff0, "\x0b");
                    pressed = work->command.input[0].pressed;
                    if ((pressed & CARD_PAD_ACCEPT) != 0) {
                        xglSoundEffectNormalID(CARD_SE_ACCEPT, 0);
                        work->command.step++;
                    } else if ((pressed & CARD_PAD_CANCEL) != 0) {
                        xglSoundEffectNormalID(CARD_SE_CANCEL, 0);
                        result = -1;
                    }
                }
            } else {
                vecA[0] = 0.0f;
                vecA[1] = -0.3932f;
                vecA[2] = 0.1f;
                vecA[3] = 1.0f;
                vecB[0] = vecB[1] = 0.335f;
                vecB[2] = vecB[3] = 1.0f;
                xglMatrixUnit(cursor);
                xglMatrixTrans(cursor, cursor, vecA);
                xglMatrixScale(cursor, cursor, vecB);
                cardId = pCnow_7->entries[pCnow_7->scroll + pCnow_7->cursor].cardId;
                CardDisp(cursor, cardId, ((s16 *)work->ownedCount)[cardId] != 0, exiting == 0);
            }

            if (exiting == 0) {
                if ((PadData.directions & CARD_PAD_UP) != 0) {
                    xglSoundEffectNormalID(CARD_SE_CURSOR, 0);
                    pCnow_7->action = CARD_ACTION_SCROLL_UP;
                } else if ((PadData.directions & CARD_PAD_DOWN) != 0) {
                    xglSoundEffectNormalID(CARD_SE_CURSOR, 0);
                    pCnow_7->action = CARD_ACTION_SCROLL_DOWN;
                }
                if ((PadData.directions & CARD_PAD_RIGHT) != 0) {
                    xglSoundEffectNormalID(CARD_SE_CURSOR, 0);
                    if (CardGetDeckAllCnt(deck) != 0) {
                        ActiveCursol_6 = 1;
                        list->cursor |= CARD_PANE_INACTIVE;
                        deck->cursor &= CARD_CURSOR_INDEX_MASK;
                    }
                } else if ((PadData.directions & CARD_PAD_LEFT) != 0) {
                    xglSoundEffectNormalID(CARD_SE_CURSOR, 0);
                    ActiveCursol_6 = 0;
                    list->cursor &= CARD_CURSOR_INDEX_MASK;
                    deck->cursor |= CARD_PANE_INACTIVE;
                }
            }

            if (deck->action != CARD_ACTION_NONE) {
                if (deck->action == CARD_ACTION_SCROLL_UP || deck->action == CARD_ACTION_SCROLL_DOWN) {
                    if (deck->action < 0) {
                        if (deck->scroll > 0) {
                            if (deck->cursor < CARD_SCROLL_MARGIN) {
                                deck->scroll = deck->scroll - 1;
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
                        if (deck->scroll + deck->windowRows < deck->rowCount) {
                            if (deck->cursor >= deck->windowRows - CARD_SCROLL_MARGIN) {
                                deck->scroll = deck->scroll + 1;
                            } else {
                                deck->cursor = deck->cursor + 1;
                            }
                        } else if (deck->cursor < deck->windowRows - 1 && deck->cursor < deck->rowCount - 1
                                   && deck->cursor < deck->cursorLimit) {
                            deck->cursor = deck->cursor + 1;
                        }
                    }
                }
                deck->action = CARD_ACTION_NONE;
            }

            if (list->action != CARD_ACTION_NONE) {
                list->rowCount = CARD_DECK_SLOT_COUNT;
                if (list->action == CARD_ACTION_SCROLL_UP || list->action == CARD_ACTION_SCROLL_DOWN) {
                    if (list->action < 0) {
                        if (list->scroll > 0) {
                            if (list->cursor < CARD_SCROLL_MARGIN) {
                                list->scroll = list->scroll - 1;
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
                        if (list->scroll + list->windowRows < list->rowCount) {
                            if (list->cursor >= list->windowRows - CARD_SCROLL_MARGIN) {
                                list->scroll = list->scroll + 1;
                            } else {
                                list->cursor = list->cursor + 1;
                            }
                        } else if (list->cursor < list->windowRows - 1 && list->cursor < CARD_DECK_SLOT_COUNT - 1) {
                            list->cursor = list->cursor + 1;
                        }
                    }
                    slot = list->scroll + (list->cursor & CARD_CURSOR_INDEX_MASK);
                    prev = -1;
                    entry = -1;
                    for (pos = 0; pos < CARD_DECK_SIZE; pos++) {
                        if (prev != EnemyDeckLst[slot][pos]) {
                            entry++;
                            deck->entries[entry].cardId = EnemyDeckLst[slot][pos];
                            deck->entries[entry].copies = 1;
                        } else {
                            deck->entries[entry].copies++;
                        }
                        prev = EnemyDeckLst[slot][pos];
                    }
                    deck->cursor = (s8)CARD_PANE_INACTIVE;
                    deck->rowCount = entry + 1;
                    deck->scroll = 0;
                }
                list->action = CARD_ACTION_NONE;
            }

            if ((s16)deck->rowCount - 1 < deck->cursor) {
                deck->cursor = deck->rowCount - 1;
            }
        break;
    case 1:
            CardDeckListDisp(list);
            CardDeckDisp(deck, work->definitions);
            if (exiting != 0) {
                break;
            }
            xglFontPrint(0, 0, 0, "\x19\x03");
            xglFontPrint(120, 288, 0xfff0, "");
            xglFontPrint(24, 336, 0xfff0, "\x0b\x19\x03\r\0Is this deck okay?");
            xglFontPrint(24, 360, 0xfff0, CARD_PROMPT_ACCEPT_CANCEL);
            xglFontPrint(120, 288, 0xfff0, "\x0b");
            pressed = work->command.input[0].pressed;
            if ((pressed & CARD_PAD_ACCEPT) != 0) {
                xglSoundEffectNormalID(CARD_SE_ACCEPT, 0);
                result = (list->cursor & CARD_CURSOR_INDEX_MASK) + 1;
            } else if ((pressed & CARD_PAD_CANCEL) != 0) {
                xglSoundEffectNormalID(CARD_SE_CANCEL, 0);
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

static CardDeckListWindow * pCnow_1;

static CardDeckListWindow * pCnow_3;

static CardDeckListWindow * pCnow_5;

static CardDeckListWindow * pCnow_7;
