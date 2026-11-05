/*
 * OV10 original TU 5: 0x00a1c1b8..0x00a1e5d8 (5 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov10/cgp.h"
#include "ov10/card_id.h"
#include "card_view_proc.h"
#include "ov10/card_fread.h"
#include "ov10/card_disp.h"
#include "ov10/card_deck_chk.h"
#include "main/data.h"
#include "main/xgl_2.h"
#include "main/xgl_font.h"
#include "main/nml_model_set.h"

/* CardViewProc: the card list window and the digit models of the list position shown beside it. */
#define CARD_LIST_FILTER_ALL 0xFFFF   /* CardMakeDispList (0x00a08880) masks the type byte with the high byte and the
                                       * subtype halfword with the low byte: every card passes */
#define CARD_LIST_SCROLL_MARGIN 2     /* cursor rows kept between the cursor and the window edge: slti at 0x00a1c38c */
#define CARD_COMPLETE_MODEL ((void *) 0x01ddf800) /* deckmake\comp.lex, shown when CardCntList is nonzero for every card, ori at 0x00a1c750 */
#define CARD_COMPLETE_TEXTURE ((void *) 0x01ddfc00) /* deckmake\comp.xtx, ori at 0x00a1c73c */

/* COPCardDispSub: the cards fanned out in the pack opening screens. */
#define CARD_FAN_SELECTED_MODEL ((void *) 0x01d12000) /* game\Cur_WakuW.lex, ori at 0x00a1c928 */
#define CARD_FAN_SELECTED_TEXTURE ((void *) 0x01d60000) /* game\Cur_Waku.xtx */
#define CARD_FAN_BACKDROP_MODEL ((void *) 0x01da5600) /* title\window3.lex, ori at 0x00a1ca48 */
#define CARD_FAN_BACKDROP_TEXTURE ((void *) 0x01db5800) /* title\window.bin, ori at 0x00a1ca34 */

/* CardOpenProc loads these files into the card model arena (see cgp.c) and draws the pack models from the
 * same addresses. */
#define CARD_PACK1_MODEL 0x01900000   /* deckmake\pack1.lex */
#define CARD_PACK2_MODEL 0x01901000   /* deckmake\pack2.lex, ori at 0x00a1ce58 */
#define CARD_PACK3_MODEL 0x01903000   /* deckmake\pack3.lex */
#define CARD_PACK4_MODEL 0x01905000   /* deckmake\pack4.lex */
#define CARD_CURSOR_L_MODEL ((void *) 0x01908000) /* deckmake\cur_L.lex */
#define CARD_CURSOR_R_MODEL ((void *) 0x01908800) /* deckmake\cur_R.lex */
#define CARD_PACK_TEXTURE ((void *) 0x01A00000)  /* deckmake\pack.bin */
#define CARD_CURSOR_TEXTURE ((void *) 0x01A40050) /* deckmake\cur_LR.bin, ori at 0x00a1cecc */

/* Pack kinds: CardExist holds one count per kind. The counts are read with dataEvtBoxChk and used up with dataEvtBoxDec
 * on the entries CARD_PACK_EVENT_BASE + kind. */
#define CARD_PACK_COUNT 20            /* slti at 0x00a1ceec, 0x00a1ca94 */
#define CARD_PACK_EVENT_BASE 0x24     /* addiu at 0x00a1cedc (dataEvtBoxChk), 0x00a1dc5c (dataEvtBoxDec) */
#define CARD_OPEN_LIST_MAX 0x28       /* CardList capacity: a starter pack lists 1 + 39 cards */
/* The save keeps the owned copies of each card (0..3) in 2 bits, four cards per byte (0x00a1dfb4..0x00a1dfcc). */
#define CARD_SELL_PRICE 5             /* dataMoneyBoxInc argument for a card sold instead of kept (0x00a1deb8) */
#define ZENKAKU_GLYPH_SIZE 2          /* the full-width digit tables hold two bytes per glyph */

/* Packs. Kinds 0..16 each give CARD_PACK_SPECIAL_CARDS copies of one fixed card; the three that follow draw cards at
 * random from the BS1 / BS2 / ST1 lists. The kind also selects the pack model (switch at 0x00a1d3a0, 0x00a1d51c). */
#define CARD_PACK_SPECIAL_CARDS 3     /* li 3 at 0x00a1d6b8, stored to CardCnt at 0x00a1d6c0 */
enum {
    CARD_PACK_ST1 = 17,
    CARD_PACK_BS1,
    CARD_PACK_BS2
};
#define CARD_PACK_DRAW_SIZE 10        /* cards drawn from BS1 / BS2: slti at 0x00a1d858 */
#define CARD_PACK_RARE_DRAWS 1        /* BS1: li 1 at 0x00a1d6dc, bgtz at 0x00a1d78c; BS2: 0x00a1d944, 0x00a1da0c */
#define CARD_PACK_UNCOMMON_DRAWS 2    /* BS1: li 2 at 0x00a1d6f8, bgtz at 0x00a1d84c; BS2: 0x00a1d960, 0x00a1dacc */
/* Entries of the BS1 / BS2 / ST1 card lists: the sizes of their original symbols (config/symbols/ov10.txt). */
#define BS1_RARE_COUNT 0x10
#define BS1_UNCOMMON_COUNT 0x17
#define BS1_COMMON_COUNT 0x2C
#define BS2_RARE_COUNT 0x14
#define BS2_UNCOMMON_COUNT 9
#define BS2_COMMON_COUNT 0x11
#define ST1_RARE_COUNT 6
#define ST1_CARD_COUNT 0x27           /* slti at 0x00a1dc2c */

/* Pack selection (CARD_OPEN_STEP_SELECT / _OPENING): the two cursor arrows slide in by CARD_CURSOR_TRAVEL over a cycle of
 * CARD_CURSOR_FRAMES frames starting at CARD_CURSOR_REST_X, and the pack model turns by a fixed angle per frame. */
#define CARD_CURSOR_FRAMES 0x10       /* CursorTimer wraps here: sltiu at 0x00a1d1f0 */
#define CARD_CURSOR_LAST_FRAME 15.0f
#define CARD_CURSOR_TRAVEL 0.31260002f
#define CARD_CURSOR_REST_X 1.5924f
#define CARD_PACK_SPIN_STEP 0.08726647f /* 5 degrees */
#define CARD_PACK_OPEN_SPIN_STEP 0.17453294f /* 10 degrees */

/* Steps of CardOpenProc's faseStep. */
enum {
    CARD_OPEN_STEP_INIT,          /* clear the pack counts */
    CARD_OPEN_STEP_FADE_OUT,
    CARD_OPEN_STEP_LOAD,          /* load the pack models and read the pack counts */
    CARD_OPEN_STEP_FADE_IN,
    CARD_OPEN_STEP_COUNT,         /* find the owned pack kinds */
    CARD_OPEN_STEP_SELECT,        /* choose a pack to open */
    CARD_OPEN_STEP_OPENING,       /* fade out while the pack spins */
    CARD_OPEN_STEP_DRAW,          /* use up a pack and fill CardList */
    CARD_OPEN_STEP_REVEAL,        /* fade in the drawn cards */
    CARD_OPEN_STEP_BROWSE,        /* look through the drawn cards */
    CARD_OPEN_STEP_STORE,         /* add them to the collection, selling the extra copies */
    CARD_OPEN_STEP_REVEAL_KEPT,   /* fade in the cards that were kept */
    CARD_OPEN_STEP_SOLD,          /* report the cards sold */
    CARD_OPEN_STEP_BROWSE_KEPT,
    CARD_OPEN_STEP_LEAVE          /* fade out and return to the pack selection */
};

/* Declared here: the shared block of ov10/cgp.h has no xglMatrixRotY, and an implicit declaration changes CardOpenProc. */
extern void xglMatrixRotY(Matrix4 destination, Matrix4 source, float angle);


void CardViewProc(CardGameWork *work, CardListWindow *list, s16 *exiting)
{
    Matrix4 cardMatrix;
    float cardOffset[4];
    float digitOffset[4];
    float digitScale[4];
    Matrix4 digitBase;
    Matrix4 digitMatrix;
    int number;
    int digit;
    int i;
    int allCounted;
    unsigned int digitModel;

    xglFontDebugPrintf(84, 0, "View Cards");
    work->helpTopic = CARD_HELP_PAGE_VIEWING_CARDS;

    cardOffset[0] = -1.0f;
    cardOffset[1] = 0.6f;
    cardOffset[2] = 0.1f;
    cardOffset[3] = 1.0f;
    xglMatrixUnit(cardMatrix);
    xglMatrixTrans(cardMatrix, cardMatrix, cardOffset);

    if (*exiting != 0) {
        xglFontPrint(24, 312, CARD_TEXT_COLOR, ModeEndMess0);
        xglFontPrint(224, 360, CARD_TEXT_COLOR, ModeEndMess1);
        if (PadData[0].half_2a & PAD_CIRCLE) {
            work->fadeOutTimer = CARD_FADE_TICKS;
            work->fadeOutStep = CARD_FADE_STEP;
            work->fase = CARD_FASE_TITLE;
            xglSoundEffectNormalID(SE_DECIDE, 0);
        }
        if ((PadData[0].half_2a & PAD_CROSS) || (work->command.input[0].pressed & PAD_START)) {
            *exiting = 0;
            xglSoundEffectNormalID(SE_CANCEL, 0);
        }
    } else {
        if (PadData[0].directions & PAD_UP) {
            list->mode = -1;
        } else if (PadData[0].directions & PAD_DOWN) {
            list->mode = 1;
        } else if (work->command.input[0].pressed & PAD_START) {
            *exiting = 1;
            xglSoundEffectNormalID(SE_DECIDE, 0);
        }
    }

    /* list->mode holds a pending scroll request: -1 back, 1 forward. */
    if (list->mode != 0) {
        xglSoundEffectNormalID(SE_CURSOR, 0);
        if (list->mode < 0) {
            if (list->top > 0) {
                if (list->cursor < CARD_LIST_SCROLL_MARGIN) {
                    list->top -= 1;
                } else {
                    list->cursor--;
                    if (list->cursor < 0) {
                        list->cursor = 0;
                    }
                }
            } else if (list->cursor > 0) {
                list->cursor--;
            }
        } else {
            if (list->top + list->rows < list->count) {
                if (list->cursor >= list->rows - CARD_LIST_SCROLL_MARGIN) {
                    list->top += 1;
                } else {
                    list->cursor += 1;
                }
            } else if (list->cursor < list->rows - 1 && list->cursor < list->count - 1) {
                list->cursor += 1;
            }
        }
        list->mode = 0;
        list->count = CardMakeDispList(list->entries, CARD_LIST_FILTER_ALL, work->definitions,
                                       work->cardCount);
    }
    CardListDisp(list, work);

    /* The position in the list, 1-based, drawn as up to three digit models. */
    number = list->top + list->cursor + 1;
    digit = number / 100;

    digitOffset[1] = 1.764f;
    digitOffset[2] = 0.01f;
    digitScale[0] = 1.13f;
    digitOffset[3] = 1.0f;
    digitScale[3] = 1.0f;
    digitScale[2] = 1.0f;
    digitScale[1] = 1.0f;
    xglMatrixUnit(digitBase);
    xglMatrixScale(digitBase, digitBase, digitScale);

    if (digit != 0) {
        digitOffset[0] = 1.948f;
        digitModel = CARD_DIGIT_MODEL_BASE + (digit << CARD_DIGIT_MODEL_SHIFT);
        xglMatrixTrans(digitMatrix, digitBase, digitOffset);
        nmlModelSetPlace(&digitMatrix);
        nmlModelSetTexture(CARD_DIGIT_TEXTURE);
        nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
        nmlModelEntryCard(digitModel);
    }
    digit = number % 100 / 10;
    if (number / 100 != 0 || digit != 0) {
        digitOffset[0] = 2.095f;
        digitModel = CARD_DIGIT_MODEL_BASE + (digit << CARD_DIGIT_MODEL_SHIFT);
        xglMatrixTrans(digitMatrix, digitBase, digitOffset);
        nmlModelSetPlace(&digitMatrix);
        nmlModelSetTexture(CARD_DIGIT_TEXTURE);
        nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
        nmlModelEntryCard(digitModel);
    }
    digit = number % 100 % 10;
    allCounted = 1;
    digitOffset[0] = 2.255f;
    digitModel = CARD_DIGIT_MODEL_BASE + (digit << CARD_DIGIT_MODEL_SHIFT);
    xglMatrixTrans(digitMatrix, digitBase, digitOffset);
    nmlModelSetPlace(&digitMatrix);
    nmlModelSetTexture(CARD_DIGIT_TEXTURE);
    nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
    nmlModelEntryCard(digitModel);

    CardDisp(&cardMatrix, list->entries[list->top + list->cursor].cardId,
             work->ownedCount[list->entries[list->top + list->cursor].cardId] != 0,
             *exiting == 0);

    for (i = 0; i < work->cardCount; i += 1) {
        if (CardCntList(work, i) == 0) {
            allCounted = 0;
            break;
        }
    }
    if (allCounted) {
        cardOffset[0] = -0.194f;
        cardOffset[1] = 1.6f;
        cardOffset[2] = 0.5f;
        cardOffset[3] = 1.0f;
        xglMatrixUnit(cardMatrix);
        xglMatrixTrans(cardMatrix, cardMatrix, cardOffset);
        nmlModelSetPlace(&cardMatrix);
        nmlModelSetTexture(CARD_COMPLETE_TEXTURE);
        nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
        nmlModelEntryCard(CARD_COMPLETE_MODEL);
    }
}

void COPCardDispSub(int showDetail, CardGameWork *work, Matrix4 base, s16 *cardIds,
                    u8 cardCount, int selected)
{
    Matrix4 transformed;
    float position[4];
    float scale[4];
    float spacing;
    int i;

    position[1] = position[2] = 0.0f;
    spacing = 3.9757f / (float)(cardCount - 1);
    if (cardCount != 0) {
        for (i = 0; i < cardCount; i++) {
            position[0] = spacing * (float)i + -1.98785f;
            position[2] = (float)(i + 1) * 0.015f;
            if (i == selected) {
                position[2] = (float)(cardCount + 1) * 0.02f;
            }
            position[3] = 1.0f;
            /* This statement order gives the original store sequence at 0x00a1c8bc..0x00a1c8d0 (+0x4C, +0x54, +0x50,
             * +0x5C, +0x58); index order compiles to a different sequence. */
            scale[1] = 0.4f;
            scale[0] = 0.4f;
            scale[3] = 1.0f;
            scale[2] = 1.0f;
            xglMatrixTrans(transformed, base, position);
            xglMatrixScale(transformed, transformed, scale);
            if (i == selected) {
                CardDispCard(&transformed, cardIds[i], 1);
                nmlModelSetLight(&CCColor, &asDir);
                nmlModelSetPlace(&transformed);
                nmlModelSetTexture(CARD_FAN_SELECTED_TEXTURE);
                nmlModelEntryCard(CARD_FAN_SELECTED_MODEL);
            } else {
                CardDispCard(&transformed, cardIds[i], 1);
            }
        }
        if (showDetail != 0) {
            /* This statement order gives the original store sequence at 0x00a1c99c..0x00a1c9b8 (+0x44, +0x48, +0x50,
             * +0x58, +0x40, +0x54, +0x5C); index order compiles to a different sequence. */
            position[1] = 1.4f;
            position[2] = 0.01f;
            position[0] = 0.0f;
            scale[1] = 0.85f;
            scale[0] = 0.85f;
            scale[3] = 1.0f;
            scale[2] = 1.0f;
            xglMatrixTrans(transformed, base, position);
            xglMatrixScale(transformed, transformed, scale);
            CardDisp(&transformed, cardIds[selected], 1, 1);
        }
    }
    /* This statement order gives the original store sequence at 0x00a1ca10..0x00a1ca20 (+0x50, +0x54, +0x5C, +0x58);
     * index order compiles to a different sequence. */
    scale[1] = 0.4f;
    scale[3] = 1.0f;
    scale[2] = 1.0f;
    scale[0] = 1.0f;
    xglMatrixScale(transformed, base, scale);
    nmlModelSetPlace(&transformed);
    nmlModelSetTexture(CARD_FAN_BACKDROP_TEXTURE);
    nmlModelSetRenderLevel(CARD_MODEL_RENDER_LEVEL);
    nmlModelEntryCard(CARD_FAN_BACKDROP_MODEL);
}

int COPSerachPackR(unsigned char *pack_occupied, int current_position)
{
    int found_position = -1;
    int scan_position;

    for (scan_position = current_position + 1; scan_position < CARD_PACK_COUNT; scan_position += 1) {
        if (pack_occupied[scan_position] != 0) {
            found_position = scan_position;
            break;
        }
    }

    if (found_position < 0) {
        for (scan_position = 0; scan_position < current_position - 1; scan_position += 1) {
            if (pack_occupied[scan_position] != 0) {
                found_position = scan_position;
                break;
            }
        }
        if (found_position < 0) {
            found_position = current_position;
        }
    }

    return found_position;
}

int COPSerachPackL(unsigned char *pack_occupied, int current_position)
{
    int found_position = -1;
    int scan_position;

    for (scan_position = current_position - 1; scan_position >= 0; scan_position -= 1) {
        if (pack_occupied[scan_position] != 0) {
            found_position = scan_position;
            break;
        }
    }

    if (found_position < 0) {
        for (scan_position = CARD_PACK_COUNT - 1; scan_position > current_position; scan_position -= 1) {
            if (pack_occupied[scan_position] != 0) {
                found_position = scan_position;
                break;
            }
        }
        if (found_position < 0) {
            found_position = current_position;
        }
    }

    return found_position;
}

/*
 * Pack opening screen. Now is the pack kind under the cursor, Max the number of packs left, serchcnt the number of
 * kinds still owned, CardList / CardCnt the cards drawn from the opened pack, SelFlg the number of extra copies sold.
 */
void CardOpenProc(CardGameWork *work, s16 *exiting)
{
    /* Function-local statics; the original symbol table names them CardExist.0 .. CursorTimer.9. */
    static u8 CardExist[CARD_PACK_COUNT];
    static s16 CardList[CARD_OPEN_LIST_MAX];
    static int Now;
    static int Max;
    static int serchcnt;
    static int CardCnt;
    static int SelFlg;
    static float rotval = 0.0f;
    static int NowNo;
    static u8 CursorTimer = 0;
    Matrix4 cardMatrix;
    Matrix4 placement;
    float cardOffset[4];
    float cardScale[4]; /* only ever stored: the original writes it too (0x00a1cc40..0x00a1cc60) */
    int i;
    int j;
    int packModel;
    u8 packDigits[4];
    u8 digits[5];
    int rareLeft;
    int uncommonLeft;
    unsigned short pick;
    int duplicate;

    work->helpTopic = CARD_HELP_PAGE_OPENING_CARDS;
    cardOffset[0] = cardOffset[1] = cardOffset[2] = 0.0f;
    cardScale[0] = cardScale[1] = cardScale[2] = cardScale[3] = cardOffset[3] = 1.0f;
    xglMatrixUnit(cardMatrix);

    /* START toggles the exit prompt only in the pack selection and the last two steps (CARD_OPEN_STEP_SELECT,
     * _BROWSE_KEPT, _LEAVE, and any step past them); an if-chain on the same condition compiles to a different size. */
    switch (work->faseStep) {
    case CARD_OPEN_STEP_INIT: case CARD_OPEN_STEP_FADE_OUT: case CARD_OPEN_STEP_LOAD:
    case CARD_OPEN_STEP_FADE_IN: case CARD_OPEN_STEP_COUNT:
        break;
    case CARD_OPEN_STEP_OPENING: case CARD_OPEN_STEP_DRAW: case CARD_OPEN_STEP_REVEAL:
    case CARD_OPEN_STEP_BROWSE: case CARD_OPEN_STEP_STORE: case CARD_OPEN_STEP_REVEAL_KEPT:
    case CARD_OPEN_STEP_SOLD:
        break;
    default:
        if (work->command.input[0].pressed & PAD_START) {
            *exiting = (*exiting + 1) & 1;
            if (*exiting != 0) {
                xglSoundEffectNormalID(SE_DECIDE, 0);
            } else {
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
        }
    }

    if (*exiting != 0) {
        xglFontPrint(24, 312, CARD_TEXT_COLOR, "\x19\x03\r\0Finished opening cards?\0");
        xglFontPrint(224, 360, CARD_TEXT_COLOR, ModeEndMess1);
        if (PadData[0].half_2a & PAD_CIRCLE) {
            work->fadeOutTimer = CARD_FADE_TICKS;
            work->fadeOutStep = CARD_FADE_STEP;
            work->fase = CARD_FASE_TITLE;
            *exiting = 0;
            xglSoundEffectNormalID(SE_DECIDE, 0);
        } else if (PadData[0].half_2a & PAD_CROSS) {
            *exiting = 0;
            xglSoundEffectNormalID(SE_CANCEL, 0);
        }
        return;
    }

    switch (work->faseStep) {
    case CARD_OPEN_STEP_INIT:
    default:
        work->command.step = 0;
        work->faseStep += 1;
        work->fadeOutTimer = CARD_FADE_TICKS;
        work->fadeOutStep = CARD_FADE_STEP;
        for (i = 0; i < CARD_PACK_COUNT; i += 1) {
            CardExist[i] = 0;
        }
        Now = Max = 0;
        serchcnt = 0;
        break;
    case CARD_OPEN_STEP_FADE_OUT:
        CardFadeOut(work);
        if (work->fadeOutTimer == 0) {
            work->faseStep += 1;
        }
        break;
    case CARD_OPEN_STEP_LOAD:
        /* Hide every model entry, then show the last two. */
        for (i = CARD_MODEL_COUNT - 1; i >= 0; i -= 1) {
            work->models[i].visible = 0;
        }
        work->fadeOutTimer = CARD_FADE_TICKS;
        work->models[CARD_MODEL_COUNT - 1].visible = 1;
        work->models[CARD_MODEL_COUNT - 2].visible = 1;
        work->faseStep += 1;
        CardFread((void *) CARD_PACK1_MODEL, "deckmake\\pack1.lex");
        CardFread((void *) CARD_PACK2_MODEL, "deckmake\\pack2.lex");
        CardFread((void *) CARD_PACK3_MODEL, "deckmake\\pack3.lex");
        CardFread((void *) CARD_PACK4_MODEL, "deckmake\\pack4.lex");
        CardFread(CARD_CURSOR_L_MODEL, "deckmake\\cur_L.lex");
        CardFread(CARD_CURSOR_R_MODEL, "deckmake\\cur_R.lex");
        CardFread(CARD_PACK_TEXTURE, "deckmake\\pack.bin");
        CardFread(CARD_CURSOR_TEXTURE, "deckmake\\cur_LR.bin");
        for (i = 0; i < CARD_PACK_COUNT; i += 1) {
            CardExist[i] = dataEvtBoxChk(i + CARD_PACK_EVENT_BASE);
        }
        break;
    case CARD_OPEN_STEP_FADE_IN:
        CardFadeIn(work);
        if (work->fadeOutTimer == 0) {
            work->faseStep += 1;
        }
        break;
    case CARD_OPEN_STEP_COUNT:
        rotval = 0.0f;
        Now = -1;
        Max = 0;
        for (i = 0; i < CARD_PACK_COUNT; i += 1) {
            Max += CardExist[i];
            if (CardExist[i] != 0) {
                if (Now < 0) {
                    Now = i;
                }
                serchcnt += 1;
            }
        }
        if (Max == 0) {
            xglFontPrint(24, 312, CARD_TEXT_COLOR, "\v\x19\x03\r\0You don't have any card packs to open.");
            if (PadData[0].all & CARD_PAD_ANY_RESPONSE_WORD) {
                work->fadeOutTimer = CARD_FADE_TICKS;
                work->fadeOutStep = CARD_FADE_STEP;
                work->fase = CARD_FASE_TITLE;
                *exiting = 0;
                xglSoundEffectNormalID(SE_DECIDE, 0);
            }
        } else {
            work->faseStep += 1;
        }
        break;
    case CARD_OPEN_STEP_SELECT:
        cardOffset[0] = cardOffset[1] = cardOffset[2] = 0.0f;
        cardScale[0] = cardScale[1] = cardScale[2] = cardScale[3] = cardOffset[3] = 1.0f;
        xglMatrixUnit(cardMatrix);
        xglFontPrint(10, 312, CARD_TEXT_COLOR, "\x19\x03\r\0Open");
        xglFontPrint(58, 312, CARD_TEXT_COLOR, CardConvertNameLst[Now]);
        xglFontPrint(162, 312, CARD_TEXT_COLOR, "\x19\x03\r\0?");
        xglFontPrint(10, 384, CARD_TEXT_COLOR, "\xA1\xA1\xA1\xA1\xA1\xA1\xA1\xA1\xA1\xA1\xA1\xA1\xA1\xA1\xA1\xA1\xA1\xA1\xA1\xA1\x0C\x80@@\xA1\xFB \v\x19\x03\r\0Okay");
        xglFontPrint(10, 336, CARD_TEXT_COLOR, "\v\x19\x03\r\0Number of packs: ");
        packDigits[0] = ZenkakuListA[(CardExist[Now] / 10) * ZENKAKU_GLYPH_SIZE];
        packDigits[1] = ZenkakuListA[(CardExist[Now] / 10) * ZENKAKU_GLYPH_SIZE + 1];
        packDigits[2] = ZenkakuListB[(CardExist[Now] % 10) * ZENKAKU_GLYPH_SIZE];
        packDigits[3] = ZenkakuListB[(CardExist[Now] % 10) * ZENKAKU_GLYPH_SIZE + 1];
        xglFontPrint(146, 336, CARD_TEXT_COLOR, packDigits);
        xglFontPrint(130, 336, CARD_TEXT_COLOR, ""); /* the original prints an empty string here too (0x00a1d154, string at 0x00a4dca8) */
        xglFontPrint(120, 288, CARD_TEXT_COLOR, "\v");
        if (serchcnt != 1) {
            if (work->command.input[0].pressed & PAD_RIGHT) {
                xglSoundEffectNormalID(SE_CURSOR, 0);
                Now = COPSerachPackR(CardExist, Now);
            } else if (work->command.input[0].pressed & PAD_LEFT) {
                xglSoundEffectNormalID(SE_CURSOR, 0);
                Now = COPSerachPackL(CardExist, Now);
            }
            CursorTimer += 1;
            if (CursorTimer >= CARD_CURSOR_FRAMES) {
                CursorTimer = 0;
            }
            cardOffset[1] = 0.6271f;
            cardOffset[2] = 0.6271f;
            cardOffset[0] = (((float) CursorTimer * CARD_CURSOR_TRAVEL) / CARD_CURSOR_LAST_FRAME) + CARD_CURSOR_REST_X;
            xglMatrixTrans(placement, cardMatrix, cardOffset);
            nmlModelSetPlace(&placement);
            nmlModelSetTexture(CARD_CURSOR_TEXTURE);
            nmlModelEntryCard(CARD_CURSOR_R_MODEL);
            cardOffset[1] = 0.6271f;
            cardOffset[2] = 0.6271f;
            cardOffset[0] = -CARD_CURSOR_REST_X - (((float) CursorTimer * CARD_CURSOR_TRAVEL) / CARD_CURSOR_LAST_FRAME);
            xglMatrixTrans(placement, cardMatrix, cardOffset);
            nmlModelSetPlace(&placement);
            nmlModelSetTexture(CARD_CURSOR_TEXTURE);
            nmlModelEntryCard(CARD_CURSOR_L_MODEL);
        }
        if (work->command.input[0].pressed & PAD_CIRCLE) {
            xglSoundEffectNormalID(SE_DECIDE, 0);
            work->faseStep += 1;
            work->fadeOutTimer = CARD_FADE_TICKS;
        }
        cardOffset[1] = 0.6271f;
        cardOffset[2] = 0.3881f;
        cardOffset[0] = 0.0f;
        rotval += CARD_PACK_SPIN_STEP;
        xglMatrixTrans(placement, cardMatrix, cardOffset);
        switch (Now) {
        case CARD_PACK_ST1: packModel = CARD_PACK1_MODEL; break;
        case CARD_PACK_BS1: packModel = CARD_PACK2_MODEL; break;
        case CARD_PACK_BS2: packModel = CARD_PACK3_MODEL; break;
        default: packModel = CARD_PACK4_MODEL; break;
        }
        xglMatrixRotY(placement, placement, rotval);
        nmlModelSetPlace(&placement);
        nmlModelSetTexture(CARD_PACK_TEXTURE);
        nmlModelEntry(packModel);
        break;
    case CARD_OPEN_STEP_OPENING:
        CardFadeOut(work);
        if (work->fadeOutTimer == 0) {
            work->faseStep += 1;
        }
        xglFontPrint(10, 312, CARD_TEXT_COLOR, "\x19\x03\r\0Open ");
        xglFontPrint(138, 312, CARD_TEXT_COLOR, CardConvertNameLst[Now]);
        xglFontPrint(234, 312, CARD_TEXT_COLOR, "\x19\x03\r\0?");
        xglFontPrint(10, 384, CARD_TEXT_COLOR, "\xA1\xA1\xA1\xA1\xA1\xA1\xA1\xA1\xA1\xA1\xA1\xA1\xA1\xA1\xA1\xA1\xA1\xA1\xA1\xA1\x0C\x80@@\xA1\xFB \v\x19\x03\r\0Okay");
        xglFontPrint(120, 288, CARD_TEXT_COLOR, "\v");
        rotval += CARD_PACK_OPEN_SPIN_STEP;
        cardOffset[1] = 0.6271f;
        cardOffset[2] = 0.3881f;
        cardOffset[0] = 0.0f;
        xglMatrixTrans(placement, cardMatrix, cardOffset);
        switch (Now) {
        case CARD_PACK_ST1: packModel = CARD_PACK1_MODEL; break;
        case CARD_PACK_BS1: packModel = CARD_PACK2_MODEL; break;
        case CARD_PACK_BS2: packModel = CARD_PACK3_MODEL; break;
        default: packModel = CARD_PACK4_MODEL; break;
        }
        xglMatrixRotY(placement, placement, rotval);
        nmlModelSetPlace(&placement);
        nmlModelSetTexture(CARD_PACK_TEXTURE);
        nmlModelEntryCard(packModel);
        break;
    case CARD_OPEN_STEP_DRAW:
        switch (Now) {
        /* Kinds 0..16: three copies of one card, the card id is the case's value.
         * The cards are named after the disc's card.dat (ov10/card_id.h). */
        case 0: CardList[2] = CardList[1] = CardList[0] = CARD_ID_PHENOMENON_CHANGE; CardCnt = CARD_PACK_SPECIAL_CARDS; break;
        case 1: CardList[2] = CardList[1] = CardList[0] = CARD_ID_ZOHAR; CardCnt = CARD_PACK_SPECIAL_CARDS; break;
        case 2: CardList[2] = CardList[1] = CardList[0] = CARD_ID_BUNNIE; CardCnt = CARD_PACK_SPECIAL_CARDS; break;
        case 3: CardList[2] = CardList[1] = CardList[0] = CARD_ID_LV_10_KOS_MOS; CardCnt = CARD_PACK_SPECIAL_CARDS; break;
        case 4: CardList[2] = CardList[1] = CardList[0] = CARD_ID_LV_10_CHAOS; CardCnt = CARD_PACK_SPECIAL_CARDS; break;
        case 5: CardList[2] = CardList[1] = CardList[0] = CARD_ID_AG_05; CardCnt = CARD_PACK_SPECIAL_CARDS; break;
        case 6: CardList[2] = CardList[1] = CardList[0] = CARD_ID_THIRD_ARMAMENT; CardCnt = CARD_PACK_SPECIAL_CARDS; break;
        case 7: CardList[2] = CardList[1] = CardList[0] = CARD_ID_TESTAMENT; CardCnt = CARD_PACK_SPECIAL_CARDS; break;
        case 8: CardList[2] = CardList[1] = CardList[0] = CARD_ID_AG_04; CardCnt = CARD_PACK_SPECIAL_CARDS; break;
        case 9: CardList[2] = CardList[1] = CardList[0] = CARD_ID_PHASE_CANNON; CardCnt = CARD_PACK_SPECIAL_CARDS; break;
        case 10: CardList[2] = CardList[1] = CardList[0] = CARD_ID_INVOKE; CardCnt = CARD_PACK_SPECIAL_CARDS; break;
        case 11: CardList[2] = CardList[1] = CardList[0] = CARD_ID_DESTINY; CardCnt = CARD_PACK_SPECIAL_CARDS; break;
        case 12: CardList[2] = CardList[1] = CardList[0] = CARD_ID_DAMMERUNG; CardCnt = CARD_PACK_SPECIAL_CARDS; break;
        case 13: CardList[2] = CardList[1] = CardList[0] = CARD_ID_SO_WEAK; CardCnt = CARD_PACK_SPECIAL_CARDS; break;
        case 14: CardList[2] = CardList[1] = CardList[0] = CARD_ID_RHINE_MAIDEN; CardCnt = CARD_PACK_SPECIAL_CARDS; break;
        case 15: CardList[2] = CardList[1] = CardList[0] = CARD_ID_UNKNOWN_ARMAMENT; CardCnt = CARD_PACK_SPECIAL_CARDS; break;
        case 16: CardList[2] = CardList[1] = CardList[0] = CARD_ID_PROTO_DORA; CardCnt = CARD_PACK_SPECIAL_CARDS; break;
        case CARD_PACK_BS1:
            CardCnt = 0;
            rareLeft = CARD_PACK_RARE_DRAWS;
            uncommonLeft = CARD_PACK_UNCOMMON_DRAWS;
            while (rareLeft > 0) {
                pick = xglSRand() & (BS1_RARE_COUNT - 1);
                duplicate = 0;
                for (j = 0; j < CardCnt; j += 1) {
                    if (BS1RariLst[pick] == CardList[j]) {
                        duplicate = 1;
                        break;
                    }
                }
                if (duplicate == 0) {
                    CardList[CardCnt] = BS1RariLst[pick];
                    CardCnt += 1;
                    rareLeft -= 1;
                }
            }
            while (uncommonLeft > 0) {
                pick = xglSRand() % BS1_UNCOMMON_COUNT;
                duplicate = 0;
                for (j = 0; j < CardCnt; j += 1) {
                    if (BS1UncoLst[pick] == CardList[j]) {
                        duplicate = 1;
                        break;
                    }
                }
                if (duplicate == 0) {
                    CardList[CardCnt] = BS1UncoLst[pick];
                    CardCnt += 1;
                    uncommonLeft -= 1;
                }
            }
            while (CardCnt < CARD_PACK_DRAW_SIZE) {
                pick = xglSRand() % BS1_COMMON_COUNT;
                duplicate = 0;
                for (j = 0; j < CardCnt; j += 1) {
                    if (BS1ComoLst[pick] == CardList[j]) {
                        duplicate = 1;
                        break;
                    }
                }
                if (duplicate == 0) {
                    CardList[CardCnt] = BS1ComoLst[pick];
                    CardCnt += 1;
                }
            }
            break;
        case CARD_PACK_BS2:
            CardCnt = 0;
            rareLeft = CARD_PACK_RARE_DRAWS;
            uncommonLeft = CARD_PACK_UNCOMMON_DRAWS;
            while (rareLeft > 0) {
                pick = xglSRand() % BS2_RARE_COUNT;
                duplicate = 0;
                for (j = 0; j < CardCnt; j += 1) {
                    if (BS2RariLst[pick] == CardList[j]) {
                        duplicate = 1;
                        break;
                    }
                }
                if (duplicate == 0) {
                    CardList[CardCnt] = BS2RariLst[pick];
                    CardCnt += 1;
                    rareLeft -= 1;
                }
            }
            while (uncommonLeft > 0) {
                pick = xglSRand() % BS2_UNCOMMON_COUNT;
                duplicate = 0;
                for (j = 0; j < CardCnt; j += 1) {
                    if (BS2UncoLst[pick] == CardList[j]) {
                        duplicate = 1;
                        break;
                    }
                }
                if (duplicate == 0) {
                    CardList[CardCnt] = BS2UncoLst[pick];
                    CardCnt += 1;
                    uncommonLeft -= 1;
                }
            }
            while (CardCnt < CARD_PACK_DRAW_SIZE) {
                pick = xglSRand() % BS2_COMMON_COUNT;
                duplicate = 0;
                for (j = 0; j < CardCnt; j += 1) {
                    if (BS2ComoLst[pick] == CardList[j]) {
                        duplicate = 1;
                        break;
                    }
                }
                if (duplicate == 0) {
                    CardList[CardCnt] = BS2ComoLst[pick];
                    CardCnt += 1;
                }
            }
            break;
        default:
            /* CARD_PACK_ST1: one random rare, then the fixed starter cards. */
            CardCnt = 0;
            pick = xglSRand() % ST1_RARE_COUNT;
            CardList[CardCnt] = ST1RariLst[pick];
            CardCnt += 1;
            for (i = 0; i < ST1_CARD_COUNT; i += 1) {
                CardList[CardCnt++] = ST1CardLst[i];
            }
            break;
        }
        /* i starts the search below; the original clears it before dataEvtBoxDec (0x00a1dc4c), and starting the
         * search from the loop header instead changes the code size. */
        i = 0;
        dataEvtBoxDec(Now + CARD_PACK_EVENT_BASE);
        CardExist[Now]--;
        Now = -1;
        Max -= 1;
        if (Max > 0) {
            for (; i < Max; i += 1) {
                if (CardExist[i] != 0) {
                    Now = i;
                    break;
                }
            }
        }
        work->fadeOutTimer = CARD_FADE_TICKS;
        work->faseStep += 1;
        break;
    case CARD_OPEN_STEP_REVEAL:
        cardOffset[0] = cardOffset[2] = 0.0f;
        cardOffset[1] = -0.2f;
        cardOffset[3] = 1.0f;
        NowNo = 0;
        xglMatrixTrans(placement, cardMatrix, cardOffset);
        COPCardDispSub(0, work, placement, CardList, CardCnt, 0);
        CardFadeIn(work);
        if (work->fadeOutTimer == 0) {
            work->faseStep += 1;
        }
        break;
    case CARD_OPEN_STEP_BROWSE:
        if (work->command.input[0].directions & PAD_RIGHT) {
            if (CardCnt - 1 > NowNo) {
                xglSoundEffectNormalID(SE_CURSOR, 0);
                NowNo += 1;
            }
        }
        if ((work->command.input[0].directions & PAD_LEFT) && NowNo > 0) {
            xglSoundEffectNormalID(SE_CURSOR, 0);
            NowNo -= 1;
        }
        cardOffset[0] = cardOffset[2] = 0.0f;
        cardOffset[1] = -0.2f;
        cardOffset[3] = 1.0f;
        xglMatrixTrans(placement, cardMatrix, cardOffset);
        COPCardDispSub(1, work, placement, CardList, CardCnt, NowNo);
        if (work->command.input[0].pressed & CARD_PAD_ANY_RESPONSE) {
            work->fadeOutTimer = CARD_FADE_TICKS;
            work->faseStep += 1;
            xglSoundEffectNormalID(SE_CANCEL, 0);
        }
        break;
    case CARD_OPEN_STEP_STORE:
        CardFadeOut(work);
        if (work->fadeOutTimer == 0) {
            int added = 0;
            int k; /* a separate variable: reusing j changes the code */
            SelFlg = 0;
            for (i = 0; i < CardCnt; i += 1) {
                /* A nonzero result means the collection cannot take another copy: it is sold instead. */
                if (CardAddList(work, CardList[i]) != 0) {
                    dataMoneyBoxInc(CARD_SELL_PRICE);
                    SelFlg += 1;
                    CardList[i] = -1;
                } else {
                    added += 1;
                }
            }
            /* Close the gaps left by the sold copies: shift the tail down one slot until slot i is a kept card. */
            for (i = 0; i < added; i += 1) {
                while (CardList[i] < 0) {
                    if (i < CardCnt) {
                        for (k = i; k < CardCnt; k += 1) {
                            CardList[k] = CardList[k + 1];
                        }
                    }
                }
            }
            CardCnt = added;
            /* Unpack the saved counts into ownedCount. */
            for (i = 0; i < CARD_ID_COUNT; i += 1) {
                work->ownedCount[i] = (work->save->cardCounts[i / CARD_COUNTS_PER_BYTE] >>
                                   ((i % CARD_COUNTS_PER_BYTE) * CARD_COUNT_BITS)) & CARD_COUNT_MAX;
            }
            if (SelFlg != 0) {
                work->fadeOutTimer = CARD_FADE_TICKS;
                work->faseStep += 1;
            } else {
                work->faseStep = CARD_OPEN_STEP_FADE_IN;
                work->fadeOutTimer = CARD_FADE_TICKS;
            }
        }
        break;
    case CARD_OPEN_STEP_REVEAL_KEPT:
        NowNo = 0;
        cardOffset[0] = cardOffset[2] = 0.0f;
        cardOffset[1] = -0.2f;
        cardOffset[3] = 1.0f;
        xglMatrixTrans(placement, cardMatrix, cardOffset);
        if (CardCnt != 0) {
            COPCardDispSub(0, work, placement, CardList, CardCnt, 0);
        }
        CardFadeIn(work);
        if (work->fadeOutTimer == 0) {
            work->faseStep += 1;
        }
        break;
    case CARD_OPEN_STEP_SOLD:
        if (SelFlg != 0) {
            xglFontPrint(24, 312, CARD_TEXT_COLOR, "\v\x19\x03\r\0You can only hold up to 3 identical cards.");
            /* SelFlg as two full-width digits, then the proceeds SelFlg * CARD_SELL_PRICE as up to three. */
            digits[0] = ZenkakuListA[(SelFlg / 10) * ZENKAKU_GLYPH_SIZE];
            digits[1] = ZenkakuListA[(SelFlg / 10) * ZENKAKU_GLYPH_SIZE + 1];
            digits[2] = ZenkakuListB[(SelFlg % 10) * ZENKAKU_GLYPH_SIZE];
            digits[3] = ZenkakuListB[(SelFlg % 10) * ZENKAKU_GLYPH_SIZE + 1];
            digits[4] = 0;
            xglFontPrint(24, 336, CARD_TEXT_COLOR, digits);
            xglFontPrint(48, 336, CARD_TEXT_COLOR, "\v\x19\x03\r\0 cards sold at ");
            digits[0] = ZenkakuListA[(SelFlg / (100 / CARD_SELL_PRICE)) * ZENKAKU_GLYPH_SIZE];
            digits[1] = ZenkakuListA[(SelFlg / (100 / CARD_SELL_PRICE)) * ZENKAKU_GLYPH_SIZE + 1];
            digits[2] = 0;
            xglFontPrint(160, 336, CARD_TEXT_COLOR, digits);
            if (SelFlg / (100 / CARD_SELL_PRICE) != 0) {
                digits[0] = ZenkakuListB[((SelFlg * CARD_SELL_PRICE) % 100 / 10) * ZENKAKU_GLYPH_SIZE];
                digits[1] = ZenkakuListB[((SelFlg * CARD_SELL_PRICE) % 100 / 10) * ZENKAKU_GLYPH_SIZE + 1];
                digits[2] = 0;
                xglFontPrint(168, 336, CARD_TEXT_COLOR, digits);
            } else {
                digits[0] = ZenkakuListA[((SelFlg * CARD_SELL_PRICE) % 100 / 10) * ZENKAKU_GLYPH_SIZE];
                digits[1] = ZenkakuListA[((SelFlg * CARD_SELL_PRICE) % 100 / 10) * ZENKAKU_GLYPH_SIZE + 1];
                digits[2] = 0;
                xglFontPrint(168, 336, CARD_TEXT_COLOR, digits);
            }
            /* The two arms below are identical in the original: the units digit is printed
             * from ZenkakuListB whether or not the tens digit is zero. */
            if ((SelFlg * CARD_SELL_PRICE) % 100 / 10 != 0) {
                digits[0] = ZenkakuListB[((SelFlg * CARD_SELL_PRICE) % 100 % 10) * ZENKAKU_GLYPH_SIZE];
                digits[1] = ZenkakuListB[((SelFlg * CARD_SELL_PRICE) % 100 % 10) * ZENKAKU_GLYPH_SIZE + 1];
                digits[2] = 0;
                xglFontPrint(176, 336, CARD_TEXT_COLOR, digits);
            } else {
                digits[0] = ZenkakuListB[((SelFlg * CARD_SELL_PRICE) % 100 % 10) * ZENKAKU_GLYPH_SIZE];
                digits[1] = ZenkakuListB[((SelFlg * CARD_SELL_PRICE) % 100 % 10) * ZENKAKU_GLYPH_SIZE + 1];
                digits[2] = 0;
                xglFontPrint(176, 336, CARD_TEXT_COLOR, digits);
            }
            xglFontPrint(192, 336, CARD_TEXT_COLOR, "\x19\x03G.");
            if (work->command.input[0].all & CARD_PAD_ANY_RESPONSE_WORD) {
                xglSoundEffectNormalID(SE_DECIDE, 0);
                if (CardCnt != 0) {
                    work->faseStep += 1;
                } else {
                    work->faseStep = CARD_OPEN_STEP_LEAVE;
                    work->fadeOutTimer = CARD_FADE_TICKS;
                }
            }
        } else {
            work->faseStep += 1;
        }
        if (CardCnt != 0) {
            NowNo = 0;
            cardOffset[0] = cardOffset[2] = 0.0f;
            cardOffset[1] = -0.2f;
            cardOffset[3] = 1.0f;
            xglMatrixTrans(placement, cardMatrix, cardOffset);
            COPCardDispSub(0, work, placement, CardList, CardCnt, 0);
        }
        break;
    case CARD_OPEN_STEP_BROWSE_KEPT:
        if (work->command.input[0].directions & PAD_RIGHT) {
            if (CardCnt - 1 > NowNo) {
                xglSoundEffectNormalID(SE_CURSOR, 0);
                NowNo += 1;
            }
        }
        if ((work->command.input[0].directions & PAD_LEFT) && NowNo > 0) {
            xglSoundEffectNormalID(SE_CURSOR, 0);
            NowNo -= 1;
        }
        cardOffset[0] = cardOffset[2] = 0.0f;
        cardOffset[1] = -0.2f;
        cardOffset[3] = 1.0f;
        xglMatrixTrans(placement, cardMatrix, cardOffset);
        COPCardDispSub(1, work, placement, CardList, CardCnt, NowNo);
        if (work->command.input[0].all & CARD_PAD_ANY_RESPONSE_WORD) {
            work->fadeOutTimer = CARD_FADE_TICKS;
            work->faseStep += 1;
            xglSoundEffectNormalID(SE_CANCEL, 0);
        }
        break;
    case CARD_OPEN_STEP_LEAVE:
        CardFadeOut(work);
        if (work->fadeOutTimer == 0) {
            work->faseStep = CARD_OPEN_STEP_FADE_IN;
            work->fadeOutTimer = CARD_FADE_TICKS;
        }
        break;
    }
}

/* Defined after CardOpenProc on purpose: the original .data holds the function statics rotval (0x00a43a94) and
 * CursorTimer (0x00a43a98) before TitleMaxList (0x00a43aa0), and cc1 emits file-scope data after the statics of the
 * functions above it. Defined above the functions it is placed first and the data carve rejects the layout. */
unsigned char TitleMaxList[7] = {6, 9, 14, 9, 3, 5, 6};



s8 SkipStartList[7] = { 0, 6, 15, 29, 38, 41, 46 };

s8 PageStatusLst[177] = { 0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 3, 0, 0, 4, 0, 0, 5, 0, 1, 0, 0, 1, 1, 0, 1, 2, 0, 1, 3, 0, 1, 4, 0, 1, 5, 0, 1, 6, 0, 1, 7, 0, 1, 8, 0, 2, 0, 0, 2, 1, 0, 2, 2, 0, 2, 3, 0, 2, 4, 0, 2, 5, 0, 2, 6, 0, 2, 7, 0, 2, 8, 0, 2, 9, 0, 2, 10, 0, 2, 11, 0, 2, 12, 0, 2, 13, 0, 2, 14, 0, 3, 0, 0, 3, 1, 0, 3, 2, 0, 3, 3, 0, 3, 4, 0, 3, 5, 0, 3, 6, 0, 3, 7, 0, 3, 8, 0, 3, 9, 0, 3, 10, 0, 3, 11, 0, 3, 12, 0, 3, 13, 0, 4, 0, 0, 4, 1, 0, 4, 2, 0, 4, 3, 0, 5, 0, 0, 5, 1, 0, 5, 2, 0, 5, 3, 0, 5, 4, 0, 6, 0, 0, 6, 1, 0, 6, 2, 0, 6, 3, 0, 6, 4, 0, 6, 5, 0 };

u8 SkipPageLst[98] = { 0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0, 0, 0, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0, 0, 0, 0, 0, 15, 16, 17, 18, 19, 20, 21, 22, 23, 25, 26, 27, 28, 29, 30, 31, 33, 34, 35, 38, 40, 41, 43, 0, 0, 0, 0, 0, 44, 45, 47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48, 49, 50, 51, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 53, 54, 55, 56, 57, 58, 0, 0, 0, 0, 0, 0, 0, 0 };
