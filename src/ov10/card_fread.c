/*
 * OV10 original TU 2: 0x00a08670..0x00a12a90 (101 functions)
 */
#include "common.h"
#include "quadword.h"
#include "shared.h"
#include "ov10/cgp.h"
#include "ov10/card_id.h"
#include "ov10/card_enemy.h"
#include "main/xgl_font.h"
#include "card_fread.h"

/*
 * Reads a card-data file ("data\carddata\" + filename) into the buffer at
 * address. The prefix copy uses __builtin_strcpy: the compiler expands it
 * inline from the string literal in .rodata (ld/lw/lhu/lbu block move), which
 * plain strcpy would not do under -fno-builtin.
 */
int CardFread(void *address, char *filename)
{
    char path[256];

    __builtin_strcpy(path, "data\\carddata\\");
    strcat(path, filename);
    return xglCdReadFile(path, address, 0, 0);
}

/*
 * Mirrors CardModelFadeIn's timer/step/color fields (same offsets in
 * CardGameWork, ov10/cgp.h) but subtracts modelFadeStep and clamps each
 * channel at zero instead of raising it toward modelColorTarget.
 */
void CardModelFadeOut(CardGameWork *work)
{
    int slot;
    int channel;
    float color;

    if (work->modelFadeInTimer == 0) {
        return;
    }
    work->modelFadeInTimer -= 1;
    for (slot = 0; slot < CARD_BOARD_SLOTS; slot++) {
        for (channel = 0; channel < CARD_MODEL_COLOR_CHANNELS; channel++) {
            color = work->modelColor[slot][channel] - work->modelFadeStep;
            work->modelColor[slot][channel] = color;
            if (color < 0.0f) {
                work->modelColor[slot][channel] = 0.0f;
            }
        }
    }
}

/*
 * Raises each model channel color by modelFadeStep, capped at its
 * modelColorTarget, and counts modelFadeInTimer down.
 */
void CardModelFadeIn(CardGameWork *work)
{
    int slot;
    int channel;

    if (work->modelFadeInTimer == 0) {
        return;
    }
    work->modelFadeInTimer -= 1;
    for (slot = 0; slot < CARD_BOARD_SLOTS; slot++) {
        for (channel = 0; channel < CARD_MODEL_COLOR_CHANNELS; channel++) {
            work->modelColor[slot][channel] += work->modelFadeStep;
            if (work->modelColorTarget[slot][channel] < work->modelColor[slot][channel]) {
                work->modelColor[slot][channel] = work->modelColorTarget[slot][channel];
            }
        }
    }
}

/*
 * Lowers each slot's fadeOutColor by fadeOutStep, clamped at zero, and
 * counts fadeOutTimer down.
 */
void CardFadeOut(CardGameWork *work)
{
    int slot;
    int level;

    if (work->fadeOutTimer == 0) {
        return;
    }
    work->fadeOutTimer -= 1;
    for (slot = 0; slot < CARD_BOARD_SLOTS; slot++) {
        level = work->fadeOutColor[slot] - work->fadeOutStep;
        work->fadeOutColor[slot] = (level < 0) ? 0 : level;
    }
}

/*
 * Mirror of CardFadeOut for the slot colors: the timer counts
 * down, and each fadeOutColor rises by fadeOutStep, capped at its base color
 * fadeBaseColor.
 */
void CardFadeIn(CardGameWork *work)
{
    int slot;
    int level;

    if (work->fadeOutTimer == 0) {
        return;
    }
    work->fadeOutTimer -= 1;
    for (slot = 0; slot < CARD_BOARD_SLOTS; slot++) {
        level = work->fadeOutColor[slot] + work->fadeOutStep;
        if (work->fadeBaseColor[slot] < level) {
            level = work->fadeBaseColor[slot];
        }
        work->fadeOutColor[slot] = level;
    }
}

/*
 * Fill entries with the ids of the cards that pass filter and return how many
 * did. The high byte of filter masks CardDefinition.kind; a card of kind 1
 * must also share a bit of its flags with the low byte. filter arrives as a
 * u16 (andi 0xFFFF at entry) and cardCount as a plain int (slt, no
 * extension); the count is returned unextended, so the result is an int.
 */
int CardMakeDispList(CardListEntry *entries, u16 filter, CardDefinition *definitions, int cardCount)
{
    int kindMask;
    int flagMask;
    int i;
    int count;

    kindMask = filter >> 8;
    flagMask = filter & 0xFF;
    count = 0;
    for (i = 0; i < cardCount; i++, definitions++) {
        if (!(definitions->kind & kindMask)) {
            continue;
        }
        if (definitions->kind == CARD_KIND_BATTLE && !(definitions->flags & flagMask)) {
            continue;
        }
        entries->cardId = i;
        entries++;
        count++;
    }
    return count;
}

/*
 * Expand the run-length deck list into the flat deck: each group i writes its
 * cardId copyCount times, in group order.
 */
void CardCopyDeckWork(s16 *deck, CardListWindow *list)
{
    int group;
    int copy;
    int deckIndex;

    deckIndex = 0;
    for (group = 0; group < list->count; group++) {
        for (copy = 0; copy < list->entries[group].count; copy++) {
            deck[deckIndex++] = list->entries[group].cardId;
        }
    }
}

s32 CardGetDeckAllCnt(CardListWindow *deck)
{
    s32 total;
    int i;

    total = 0;
    for (i = 0; i < deck->count; i++) {
        total += deck->entries[i].count;
    }
    return total;
}

s32 CardGetDeckCardCnt(s16 cardId, CardListWindow *deck)
{
    s32 total;
    int i;

    total = 0;
    for (i = 0; i < deck->count; i++) {
        if (deck->entries[i].cardId == cardId) {
            total += deck->entries[i].count;
        }
    }
    return total;
}

/*
 * Sort the compact deck group list by ascending card id: a selection sort
 * that swaps whole group records, both halfwords of each.
 * The card ids are compared as unsigned halfwords. CardDelDeck calls it with
 * the group count before it decrements the count.
 */
void CardCleanUpDeck(CardListEntry *groups, s16 groupCount)
{
    int i;
    int j;
    int least;
    u16 leastKey;

    for (i = 0; i < groupCount; i++) {
        leastKey = groups[i].cardId;
        /* least stays -1 when no later group has a smaller id; j starts
         * at i + 1, so a found index is always positive. */
        for (j = i + 1, least = -1; j < groupCount; j++) {
            if ((u16) groups[j].cardId < leastKey) {
                leastKey = groups[j].cardId;
                least = j;
            }
        }
        if (least > 0) {
            s16 cardId = groups[i].cardId;
            s16 copyCount = groups[i].count;

            groups[i].cardId = groups[least].cardId;
            groups[i].count = groups[least].count;
            groups[least].cardId = cardId;
            groups[least].count = copyCount;
        }
    }
}

/*
 * Adds one copy of a card to the compact deck group list.
 * A group that already holds the card gains one copy while it has fewer than
 * three; otherwise a new group with one copy is appended. The list is then
 * sorted with CardCleanUpDeck, and the function returns the position of the
 * card's group after the sort (the group count when no group matches).
 */
s32 CardAddDeck(CardListWindow *deck, s16 cardId)
{
    s32 index;
    s32 found;

    found = 0;
    for (index = 0; index < deck->count; index++) {
        if (deck->entries[index].cardId == cardId) {
            found = 1;
            break;
        }
    }
    if (found) {
        if (deck->entries[index].count < CARD_DECK_MAX_COPIES) {
            deck->entries[index].count++;
        }
    } else {
        deck->entries[deck->count].cardId = cardId;
        deck->entries[deck->count].count = 1;
        deck->count++;
    }
    CardCleanUpDeck(deck->entries, deck->count);
    for (index = 0; index < deck->count; index++) {
        if (deck->entries[index].cardId == cardId) {
            break;
        }
    }
    return index;
}

/*
 * Removes one copy of cardId from the compact deck group list. A group with
 * two or more copies keeps one fewer. A group with one copy is dropped: its
 * card id becomes -1 and CardCleanUpDeck sorts it to the end, then the group
 * count and shownCount (+0x0E) go down. Then top (+0x0C) goes down if it is
 * nonzero, otherwise cursor (+0x05) does. Returns 0 only when the group was
 * dropped and both top and cursor were zero; otherwise 1.
 */
int CardDelDeck(CardListWindow *deck, s16 cardId)
{
    s32 group;

    for (group = 0; group < deck->count; group++) {
        if (deck->entries[group].cardId == cardId) {
            if (deck->entries[group].count < 2) {
                deck->entries[group].cardId = -1;
                deck->entries[group].count = 0;
                CardCleanUpDeck(deck->entries, deck->count);
                deck->count -= 1;
                deck->shownCount -= 1;
                if (deck->top != 0) {
                    deck->top -= 1;
                    break;
                }
                if (deck->cursor != 0) {
                    deck->cursor -= 1;
                    break;
                }
                return 0;
            } else {
                deck->entries[group].count -= 1;
                break;
            }
        }
    }
    return 1;
}

/*
 * Rewrites the escape codes of a card name in place. The
 * loop covers the first CDTSGetStrLen(text) bytes and stops at a zero byte.
 * '$' becomes a newline, '#' an apostrophe and '@' a comma. A backslash that
 * is followed by a second backslash becomes "#73" when a third backslash
 * follows the pair and "#23" when a '%' follows it. The case order '@', '#',
 * '$', '\\' follows the compare chain of the original.
 */
void CardNameConvert(unsigned char *text)
{
    int length;
    int i;

    length = CDTSGetStrLen(text);
    for (i = 0; i < length && text[i] != 0; i++) {
        switch (text[i]) {
        case '@':
            text[i] = ',';
            break;
        case '#':
            text[i] = '\'';
            break;
        case '$':
            text[i] = '\n';
            break;
        case '\\':
            if (text[i + CARD_TEXT_SECOND_CHAR] == '\\') {
                if (text[i + CARD_TEXT_THIRD_CHAR] == '\\') {
                    text[i] = '#';
                    text[i + CARD_TEXT_SECOND_CHAR] = '7';
                    text[i + CARD_TEXT_THIRD_CHAR] = '3';
                } else if (text[i + CARD_TEXT_THIRD_CHAR] == '%') {
                    text[i] = '#';
                    text[i + CARD_TEXT_SECOND_CHAR] = '2';
                    text[i + CARD_TEXT_THIRD_CHAR] = '3';
                }
            }
            break;
        default:
            break;
        }
    }
}

/*
 * Draws the deck window: the header, then one row per entry in view (deck->rows
 * at most, starting at deck->top) that holds a card, each with its cursor marker,
 * name and copy count, then the footer. Sets shownCount to count.
 * `row` is initialised at its declaration (the original sets $s2 before the
 * first xglFontPrint).
 */
void CardDeckDisp(CardListWindow *deck, CardDefinition *definitions)
{
    char nameText[32];
    char countText[16];
    s32 row = 0;
    s32 cardId;
    s32 i;
    char *name;
    CardDefinition *definition;

    xglFontPrint(deck->x, deck->y, DECK_LIST_TEXT_COLOR, cardDeckHeaderFontCode);
    if (deck->rows > 0) {
        for (; row < deck->rows; row++) {
            if (row < deck->count) {
                cardId = deck->entries[row + deck->top].cardId;
                if (cardId >= 0) {
                    definition = &definitions[cardId];
                    name = CARD_DATA_BUFFER + definition->nameOffset;
                    if (row == deck->cursor) {
                        xglFontPrint(deck->x, deck->y, DECK_LIST_TEXT_COLOR, cardSelectedRowFontCode);
                    } else {
                        xglFontPrint(deck->x, deck->y, DECK_LIST_TEXT_COLOR, cardOtherRowFontCode);
                    }
                    for (i = 0; i < deck->textLength * CARD_TEXT_CHAR_BYTES; i++) {
                        nameText[i] = name[i];
                    }
                    nameText[i] = 0;
                    CardNameConvert(nameText);
                    xglFontPrint(0, 0, 0, cardRowPrefixFontCode);
                    xglFontPrint(deck->x, deck->y + row * DECK_DISP_ROW_HEIGHT,
                                 DECK_LIST_TEXT_COLOR, nameText);
                    if (row == deck->cursor) {
                        xglFontPrint(deck->x, deck->y, DECK_LIST_TEXT_COLOR, cardSelectedRowFontCode);
                    } else {
                        xglFontPrint(deck->x, deck->y, DECK_LIST_TEXT_COLOR, cardCopyCountFontCode);
                    }
                    sprintf(countText, cardCopyCountFormat, deck->entries[row + deck->top].count);
                    xglFontPrint(deck->x + deck->textLength * DECK_DISP_CHAR_WIDTH + DECK_DISP_COUNT_GAP,
                                 deck->y + row * DECK_DISP_ROW_HEIGHT, DECK_LIST_TEXT_COLOR, countText);
                }
            }
        }
    }
    deck->shownCount = deck->count;
    xglFontPrint(deck->x, deck->y, DECK_LIST_TEXT_COLOR, cardWindowFooterFontCode);
}

/*
 * Counts the display characters of a text string.
 * A byte below 0x20 starts a control code: xglFontGetSPcodeSize returns how
 * many bytes follow the code, and the code with those bytes is skipped
 * without being counted. A negative size leaves the position unchanged.
 */
int CDTSGetStrLen(unsigned char *text)
{
    int count;
    int codeSize;
    int i;

    count = 0;
    while (*text != 0) {
        if (*text < CARD_TEXT_CONTROL_LIMIT) {
            codeSize = xglFontGetSPcodeSize(*text, text);
            if (codeSize >= 0) {
                /* Skip the code byte and its codeSize parameter bytes. */
                for (i = codeSize; i >= 0; i--) {
                    text++;
                }
            }
        } else {
            text++;
            count++;
        }
    }
    return count;
}

/*
 * Prints one line of card text at (x, y) with colour index colorIndex. It prints
 * the constant escape cardTextLineFontCode, then colorCode: the escape 0x15 0x03
 * 0x03 followed by the 16-bit big-endian value colorIndex * 21. The text
 * is then rewritten in place for its first CDTSGetStrLen(text) bytes, with the
 * CardNameConvert rules: '$' becomes a newline, '#' an apostrophe, '@' a comma
 * and "\\" followed by "\\" or "%" the escapes "#73" and "#23". The rewritten
 * text is printed in colour 0xFFF0.
 */
void CardDispTxtSub(u16 x, u16 y, unsigned char *text, u16 colorIndex)
{
    char colorCode[6] = "\x15\x03\x03";
    int colorValue;
    int length;
    int i;

    xglFontPrint(0, 0, 0, cardTextLineFontCode);
    colorValue = colorIndex * CARD_TEXT_COLOR_STEP;
    colorCode[CARD_COLOR_CODE_HIGH] = colorValue >> CARD_BYTE_BITS;
    colorCode[CARD_COLOR_CODE_LOW] = colorValue;
    colorCode[CARD_COLOR_CODE_END] = 0;
    xglFontPrint(0, 0, 0, colorCode);

    length = CDTSGetStrLen(text);
    for (i = 0; i < length && text[i] != 0; i++) {
        switch (text[i]) {
        case '@':
            text[i] = ',';
            break;
        case '#':
            text[i] = '\'';
            break;
        case '$':
            text[i] = '\n';
            break;
        case '\\':
            if (text[i + CARD_TEXT_SECOND_CHAR] == '\\') {
                if (text[i + CARD_TEXT_THIRD_CHAR] == '\\') {
                    text[i] = '#';
                    text[i + CARD_TEXT_SECOND_CHAR] = '7';
                    text[i + CARD_TEXT_THIRD_CHAR] = '3';
                } else if (text[i + CARD_TEXT_THIRD_CHAR] == '%') {
                    text[i] = '#';
                    text[i + CARD_TEXT_SECOND_CHAR] = '2';
                    text[i + CARD_TEXT_THIRD_CHAR] = '3';
                }
            }
            break;
        default:
            break;
        }
    }
    xglFontPrint(x, y, DECK_LIST_TEXT_COLOR, text);
}

/*
 * Prints one card text line at (x, y) with the default colour 0xFFF0. It is
 * CardDispTxtSub without the colour argument:
 * the font escape cardTextLineFontCode is printed, then the three-byte colour code
 * 0x15 0x03 0x03 (copied from .rodata).
 * The text is then rewritten in place with the CardNameConvert rules: '$' becomes
 * a newline, '#' an apostrophe and '@' a comma, and a backslash pair followed by
 * a third backslash or a '%' becomes "#73" or "#23".
 */
void CardDispTxtSub2(u16 x, u16 y, unsigned char *text)
{
    char colorCode[6] = "\x15\x03\x03\x00\x00";
    int length;
    int i;

    xglFontPrint(0, 0, 0, cardTextLineFontCode);
    xglFontPrint(0, 0, 0, colorCode);

    length = CDTSGetStrLen(text);
    for (i = 0; i < length && text[i] != 0; i++) {
        /*
         * The original tests '$', '#', '@' and '\\' in that order; the labels are listed '@', '#',
         * '$', '\\' because that source order yields this compare chain.
         */
        switch (text[i]) {
        case '@':
            text[i] = ',';
            break;
        case '#':
            text[i] = '\'';
            break;
        case '$':
            text[i] = '\n';
            break;
        case '\\':
            if (text[i + CARD_TEXT_SECOND_CHAR] == '\\') {
                if (text[i + CARD_TEXT_THIRD_CHAR] == '\\') {
                    text[i] = '#';
                    text[i + CARD_TEXT_SECOND_CHAR] = '7';
                    text[i + CARD_TEXT_THIRD_CHAR] = '3';
                } else if (text[i + CARD_TEXT_THIRD_CHAR] == '%') {
                    text[i] = '#';
                    text[i + CARD_TEXT_SECOND_CHAR] = '2';
                    text[i + CARD_TEXT_THIRD_CHAR] = '3';
                }
            }
            break;
        default:
            break;
        }
    }
    xglFontPrint(x, y, DECK_LIST_TEXT_COLOR, text);
}

/*
 * `number` is a float because the original does the icon index in the FPU:
 * digits are converted with `cvt.s.w`, the fall-through cases add 1.0 per step
 * (`add.s`) and the model id is formed by `mul.s`, `add.s` and `cvt.w.s`, hence
 * `model` is an unsigned int. CardDisp, the twin function, uses an int and has
 * no such code.
 */
void CardDispLight(Matrix4 *matrix, s16 cardId, s16 enabled, u8 light)
{
    unsigned int model;
    const char *texture;
    CardDefinition *definition;
    float number;
    int i;
    float translation[4];
    Matrix4 place;

    if (cardId < 0) {
        return;
    }
    definition = CARD_MASTER_TABLE + cardId;
    texture = CARD_TEXTURE(cardId);
    if (oldno_0 != cardId) {
        oldno_0 = cardId;
    }
    if (enabled > 0) {
        nmlModelSetPlace(matrix);
        nmlModelSetTexture(texture);
        if (light & CARD_LIGHT_FRAME) {
            nmlModelSetLight(&CCColor, &asDir);
        }
        nmlModelEntryCard(CARD_MODEL_FRAME);
        translation[0] = CARD_FACE_X;
        translation[1] = CARD_FACE_Y;
        translation[2] = CARD_FACE_Z;
        translation[3] = 1.0f;
        xglMatrixTrans(place, *matrix, translation);
        nmlModelSetPlace(&place);
        nmlModelSetTexture(texture);
        if (light & CARD_LIGHT_FRAME) {
            nmlModelSetLight(&CCColor, &asDir);
        }
        nmlModelEntryCard(CARD_MODEL_FACE);
    }
    /* Battle cards: the hit point and power digits. */
    if (definition->kind == CARD_KIND_BATTLE) {
        if ((definition->flags & CARD_FLAGS_NO_HIT_POINTS) == 0) {
            /* hit points: tens digit (only when non-zero), then units digit */
            number = definition->operationDuration / CARD_DIGIT_BASE;
            if (number != 0.0f) {
                model = number * CARD_MODEL_STRIDE + CARD_DIGIT_MODEL;
                translation[0] = CardHpPos10[0];
                translation[1] = CardHpPos10[1];
                translation[2] = CardHpPos10[2];
                xglMatrixTrans(place, *matrix, translation);
                nmlModelSetPlace(&place);
                if (light & CARD_LIGHT_HIT_POINTS) {
                    nmlModelSetLight(&CCColor, &asDir);
                }
                nmlModelSetTexture(CARD_ICON_TEXTURE);
                nmlModelEntryCard(model);
            }
            number = definition->operationDuration % CARD_DIGIT_BASE;
            model = number * CARD_MODEL_STRIDE + CARD_DIGIT_MODEL;
            translation[0] = CardHpPos01[0];
            translation[1] = CardHpPos01[1];
            translation[2] = CardHpPos01[2];
            xglMatrixTrans(place, *matrix, translation);
            nmlModelSetPlace(&place);
            if (light & CARD_LIGHT_HIT_POINTS) {
                nmlModelSetLight(&CCColor, &asDir);
            }
            nmlModelSetTexture(CARD_ICON_TEXTURE);
            nmlModelEntryCard(model);
        }
        number = definition->attackPower / CARD_DIGIT_BASE;
        if (number != 0.0f) {
            model = number * CARD_MODEL_STRIDE + CARD_DIGIT_MODEL;
            translation[0] = CardPowPos10[0];
            translation[1] = CardPowPos10[1];
            translation[2] = CardPowPos10[2];
            xglMatrixTrans(place, *matrix, translation);
            nmlModelSetPlace(&place);
            if (light & CARD_LIGHT_POWER) {
                nmlModelSetLight(&CCColor, &asDir);
            }
            nmlModelSetTexture(CARD_ICON_TEXTURE);
            nmlModelEntryCard(model);
        }
        number = definition->attackPower % CARD_DIGIT_BASE;
        model = number * CARD_MODEL_STRIDE + CARD_DIGIT_MODEL;
        translation[0] = CardPowPos01[0];
        translation[1] = CardPowPos01[1];
        translation[2] = CardPowPos01[2];
        xglMatrixTrans(place, *matrix, translation);
        nmlModelSetPlace(&place);
        if (light & CARD_LIGHT_POWER) {
            nmlModelSetLight(&CCColor, &asDir);
        }
        nmlModelSetTexture(CARD_ICON_TEXTURE);
        nmlModelEntryCard(model);
    }
    texture = CARD_ICON_TEXTURE;
    number = definition->reserveCost % CARD_DIGIT_BASE;
    model = number * CARD_MODEL_STRIDE + CARD_DIGIT_MODEL;
    translation[0] = CardStnbyPos[0];
    translation[1] = CardStnbyPos[1];
    translation[2] = CardStnbyPos[2];
    xglMatrixTrans(place, *matrix, translation);
    nmlModelSetPlace(&place);
    if (light & CARD_LIGHT_RESERVE) {
        nmlModelSetLight(&CCColor, &asDir);
    }
    nmlModelSetTexture(CARD_ICON_TEXTURE);
    nmlModelEntryCard(model);
    if (definition->kind == CARD_KIND_BATTLE) {
        number = 0.0f;
        /* Each label falls through to the next and adds one icon step. */
        switch (definition->flags) {
        case CARD_ATTR_WEAPON_MECH:
        case CARD_ATTR_WEAPON_HUMAN:
            number += 1.0f;
        case CARD_ATTR_UNKNOWN:
            number += 1.0f;
        case CARD_ATTR_GNOSIS:
            number += 1.0f;
        case CARD_ATTR_MECH:
            number += 1.0f;
        case CARD_ATTR_REALIAN:
            number += 1.0f;
            break;
        case CARD_ATTR_HUMAN:
            break;
        }
        texture = CARD_ICON_TEXTURE;
        model = number * CARD_MODEL_STRIDE + CARD_ICON_MODEL_BASE;
        translation[0] = CardAttriPos[0];
        translation[1] = CardAttriPos[1];
        translation[2] = CardAttriPos[2];
        xglMatrixTrans(place, *matrix, translation);
        nmlModelSetPlace(&place);
        if (light & CARD_LIGHT_ATTRIBUTE) {
            nmlModelSetLight(&CCColor, &asDir);
        }
        nmlModelSetTexture(CARD_ICON_TEXTURE);
        nmlModelEntryCard(model);
    }
    /* Requirement icons, one per condition slot. */
    for (i = 0; i < CARD_CONDITION_COUNT; i++) {
        number = 0.0f;
        /* Each label falls through to the next and adds one icon step. */
        switch (definition->need[i]) {
        case CARD_NEED_GENERATION3:
            number += 1.0f;
        case CARD_NEED_GENERATION2:
            number += 1.0f;
        case CARD_NEED_GENERATION1:
            number += 1.0f;
            break;
        case CARD_NEED_GENERATION0:
            break;
        case CARD_NEED_ANY:
            number = CARD_NEED_ICON_ANY;
            break;
        default:
            number = CARD_NEED_ICON_NONE;
            break;
        }
        if (number == CARD_NEED_ICON_NONE) {
            continue;
        }
        texture = CARD_ICON_TEXTURE;
        model = number * CARD_MODEL_STRIDE + CARD_NEED_MODEL;
        translation[0] = CardCondPos[i][0];
        translation[1] = CardCondPos[i][1];
        translation[2] = CardCondPos[i][2];
        xglMatrixTrans(place, *matrix, translation);
        nmlModelSetPlace(&place);
        if (light & CARD_LIGHT_NEED) {
            nmlModelSetLight(&CCColor, &asDir);
        }
        nmlModelSetTexture(CARD_ICON_TEXTURE);
        nmlModelEntryCard(model);
    }
    number = 0.0f;
    switch (definition->attackType) {
    case CARD_ATTACK_HOMING:
        number += 1.0f;
    case CARD_ATTACK_SPREAD:
        number += 1.0f;
    case CARD_ATTACK_BALLISTIC:
        number += 1.0f;
        break;
    case CARD_ATTACK_HAND:
        break;
    default:
        number = CARD_ICON_INDEX_NONE;
        break;
    }
    if (number >= 0.0f) {
        texture = CARD_ATTACK_TEXTURE;
        model = number * CARD_MODEL_STRIDE + CARD_ATTACK_MODEL;
        translation[0] = CardAttakPos[0];
        translation[1] = CardAttakPos[1];
        translation[2] = CardAttakPos[2];
        xglMatrixTrans(place, *matrix, translation);
        nmlModelSetPlace(&place);
        if (light & CARD_LIGHT_ATTACK) {
            nmlModelSetLight(&CCColor, &asDir);
        }
        nmlModelSetTexture(CARD_ATTACK_TEXTURE);
        nmlModelEntryCard(model);
    }
    switch (definition->rarity) {
    case CARD_RARITY_P:
        model = CARD_MODEL_RARITY_P;
        break;
    case CARD_RARITY_R:
        model = CARD_MODEL_RARITY_R;
        break;
    case CARD_RARITY_U:
        model = CARD_MODEL_RARITY_U;
        break;
    case CARD_RARITY_O: /* never matched: the chain tests 0x50, then `slti 0x51` */
    default:
        model = CARD_MODEL_RARITY_C;
        break;
    }
    nmlModelSetPlace(matrix);
    if (light & CARD_LIGHT_RARITY) {
        nmlModelSetLight(&CCColor, &asDir);
    }
    nmlModelSetTexture(CARD_RARITY_TEXTURE);
    nmlModelEntryCard(model);
    if (enabled > 0) {
        switch (definition->kind) {
        case CARD_KIND_BATTLE:
            texture = CARD_OVERLAY_TEXTURE;
            model = CARD_MODEL_KIND1;
            break;
        case CARD_KIND_EVENT:
            texture = CARD_OVERLAY_TEXTURE;
            model = CARD_MODEL_KIND2;
            break;
        case CARD_KIND_UNUSED_3: /* never matched: the chain tests 2, then `slti 0x3` */
        default:
            texture = CARD_OVERLAY_TEXTURE;
            model = CARD_MODEL_DEFAULT;
            break;
        }
    }
    nmlModelSetPlace(matrix);
    nmlModelSetTexture(texture);
    if (light & CARD_LIGHT_FRAME) {
        nmlModelSetLight(&CCColor, &asDir);
    }
    nmlModelEntryCard(model);
    if (enabled & CARD_DISP_OVERLAY) {
        nmlModelSetPlace(matrix);
        nmlModelSetTexture(texture);
        nmlModelEntryCard(CARD_MODEL_OVERLAY);
    }
    nmlModelSetPlace(matrix);
    nmlModelSetTexture(CARD_BACK_TEXTURE);
    if (light & CARD_LIGHT_FRAME) {
        nmlModelSetLight(&CCColor, &asDir);
    }
    nmlModelEntryCard(CARD_BACK_MODEL);
}

/*
 * Draws one card model without the selection flag. The enabled flag
 * is sign-extended (`sll`/`sra` 16 on $6).
 */
void CardDispCard(Matrix4 *matrix, s16 cardId, s16 enabled)
{
    unsigned int model;
    const char *texture;
    CardDefinition *definition;
    float translation[4];
    Matrix4 cardMatrix;

    if (cardId < 0) {
        return;
    }
    if ((unsigned short)cardId >= CARD_DISP_ID_COUNT) { /* unsigned compare: `sltiu $2,$2,0x94` */
        printf("CardDispCallError No[%d]\n", cardId);
        return;
    }
    definition = CARD_MASTER_TABLE + cardId;
    texture = CARD_TEXTURE(cardId);
    if (enabled <= 0) {
        return;
    }
    if (enabled & CARD_DISP_LIGHT_MIDDLE) {
        nmlModelSetLight(&MiddleColor, &asDir);
    } else if (enabled & CARD_DISP_LIGHT_CN) {
        nmlModelSetLight(&CNColor, &asDir);
    }
    nmlModelSetPlace(matrix);
    nmlModelSetTexture(texture);
    nmlModelEntryCard(CARD_MODEL_FRAME);
    translation[0] = CARD_FACE_X;
    translation[1] = CARD_FACE_Y;
    translation[2] = CARD_FACE_Z;
    translation[3] = 1.0f;
    xglMatrixTrans(cardMatrix, *matrix, translation);
    if (enabled & CARD_DISP_LIGHT_MIDDLE) {
        nmlModelSetLight(&MiddleColor, &asDir);
    } else if (enabled & CARD_DISP_LIGHT_CN) {
        nmlModelSetLight(&CNColor, &asDir);
    }
    nmlModelSetPlace(&cardMatrix);
    nmlModelSetTexture(texture);
    model = CARD_MODEL_DEFAULT;
    nmlModelEntryCard(CARD_MODEL_FACE);
    switch (definition->kind) {
    case CARD_KIND_BATTLE:
        model = CARD_MODEL_KIND1;
        break;
    case CARD_KIND_EVENT:
        model = CARD_MODEL_KIND2;
        break;
    }
    nmlModelSetPlace(matrix);
    nmlModelSetTexture(CARD_OVERLAY_TEXTURE);
    if (enabled & CARD_DISP_LIGHT_MIDDLE) {
        nmlModelSetLight(&MiddleColor, &asDir);
    } else if (enabled & CARD_DISP_LIGHT_CN) {
        nmlModelSetLight(&CNColor, &asDir);
    }
    nmlModelEntryCard(model);
    if (enabled & CARD_DISP_OVERLAY) {
        nmlModelSetPlace(matrix);
        nmlModelSetTexture(CARD_OVERLAY_TEXTURE);
        nmlModelEntryCard(CARD_MODEL_OVERLAY);
    }
}

/*
 * Draws one card at matrix. The owned bits are CARD_DISP_ENABLED (face and icons)
 * and CARD_DISP_OVERLAY; without CARD_DISP_ENABLED the card is drawn as
 * CARD_UNOWNED_MODEL. A non-zero selected shows the card text, and
 * CARD_DISP_MARKER adds the selection marker.
 */
void CardDisp(Matrix4 *matrix, s16 cardId, s16 owned, s16 selected)
{
    CardDefinition *definition;
    const char *texture;
    unsigned int model;
    int number;
    int i;
    Matrix4 place;
    float translation[4];
    float vector[4];

    if (cardId < 0) {
        return;
    }
    definition = CARD_MASTER_TABLE + cardId;
    texture = CARD_TEXTURE(cardId);
    if (owned & CARD_DISP_ENABLED) {
        nmlModelSetPlace(matrix);
        nmlModelSetTexture(texture);
        nmlModelEntryCard(CARD_MODEL_FRAME);
        translation[0] = CARD_DISP_FACE_X;
        translation[1] = CARD_FACE_Y;
        translation[2] = CARD_FACE_Z;
        translation[3] = 1.0f;
        xglMatrixTrans(place, *matrix, translation);
        nmlModelSetPlace(&place);
        nmlModelSetTexture(texture);
        nmlModelEntryCard(CARD_MODEL_FACE);
        /* Battle cards: hit point and power digits. */
        if (definition->kind == CARD_KIND_BATTLE) {
            if ((definition->flags & CARD_FLAGS_NO_HIT_POINTS) == 0) {
                number = (u8)(definition->operationDuration / CARD_DIGIT_BASE); /* the quotient is cut to a byte: `divu` then `andi 0xFF` */
                if (number != 0) {
                    model = CARD_DIGIT_MODEL + number * CARD_MODEL_STRIDE;
                    translation[0] = CardHpPos10[0];
                    translation[1] = CardHpPos10[1];
                    translation[2] = CardHpPos10[2];
                    xglMatrixTrans(place, *matrix, translation);
                    nmlModelSetPlace(&place);
                    nmlModelSetTexture(CARD_ICON_TEXTURE);
                    nmlModelEntryCard(model);
                }
                model = CARD_DIGIT_MODEL + ((definition->operationDuration % CARD_DIGIT_BASE) * CARD_MODEL_STRIDE);
                translation[0] = CardHpPos01[0];
                translation[1] = CardHpPos01[1];
                translation[2] = CardHpPos01[2];
                xglMatrixTrans(place, *matrix, translation);
                nmlModelSetPlace(&place);
                nmlModelSetTexture(CARD_ICON_TEXTURE);
                nmlModelEntryCard(model);
            }
            number = (u8)(definition->attackPower / CARD_DIGIT_BASE); /* byte quotient, `andi 0xFF` */
            if (number != 0) {
                model = CARD_DIGIT_MODEL + number * CARD_MODEL_STRIDE;
                translation[0] = CardPowPos10[0];
                translation[1] = CardPowPos10[1];
                translation[2] = CardPowPos10[2];
                xglMatrixTrans(place, *matrix, translation);
                nmlModelSetPlace(&place);
                nmlModelSetTexture(CARD_ICON_TEXTURE);
                nmlModelEntryCard(model);
            }
            model = CARD_DIGIT_MODEL + ((definition->attackPower % CARD_DIGIT_BASE) * CARD_MODEL_STRIDE);
            translation[0] = CardPowPos01[0];
            translation[1] = CardPowPos01[1];
            translation[2] = CardPowPos01[2];
            xglMatrixTrans(place, *matrix, translation);
            nmlModelSetPlace(&place);
            nmlModelSetTexture(CARD_ICON_TEXTURE);
            nmlModelEntryCard(model);
        }
        model = CARD_DIGIT_MODEL + ((definition->reserveCost % CARD_DIGIT_BASE) * CARD_MODEL_STRIDE);
        translation[0] = CardStnbyPos[0];
        translation[1] = CardStnbyPos[1];
        translation[2] = CardStnbyPos[2];
        xglMatrixTrans(place, *matrix, translation);
        nmlModelSetPlace(&place);
        nmlModelSetTexture(CARD_ICON_TEXTURE);
        nmlModelEntryCard(model);
        if (definition->kind == CARD_KIND_BATTLE) {
            number = 0;
            /* Each label falls through to the next and adds one icon step. */
            switch (definition->flags) {
            case CARD_ATTR_WEAPON_MECH:
            case CARD_ATTR_WEAPON_HUMAN:
                number++;
            case CARD_ATTR_UNKNOWN:
                number++;
            case CARD_ATTR_GNOSIS:
                number++;
            case CARD_ATTR_MECH:
                number++;
            case CARD_ATTR_REALIAN:
                number++;
                break;
            case CARD_ATTR_HUMAN:
                break;
            }
            model = CARD_ICON_MODEL_BASE + number * CARD_MODEL_STRIDE;
            translation[0] = CardAttriPos[0];
            translation[1] = CardAttriPos[1];
            translation[2] = CardAttriPos[2];
            xglMatrixTrans(place, *matrix, translation);
            nmlModelSetPlace(&place);
            nmlModelSetTexture(CARD_ICON_TEXTURE);
            nmlModelEntryCard(model);
        }
        for (i = 0; i < CARD_CONDITION_COUNT; i++) {
            number = 0;
            /* Each label falls through to the next and adds one icon step. */
            switch (definition->need[i]) {
            case CARD_NEED_GENERATION3:
                number++;
            case CARD_NEED_GENERATION2:
                number++;
            case CARD_NEED_GENERATION1:
                number++;
                break;
            case CARD_NEED_GENERATION0:
                break;
            case CARD_NEED_ANY:
                number = CARD_NEED_ICON_ANY;
                break;
            default:
                number = CARD_NEED_ICON_NONE;
                break;
            }
            if (number == CARD_NEED_ICON_NONE) {
                continue;
            }
            model = CARD_NEED_MODEL + number * CARD_MODEL_STRIDE;
            translation[0] = CardCondPos[i][0];
            translation[1] = CardCondPos[i][1];
            translation[2] = CardCondPos[i][2];
            xglMatrixTrans(place, *matrix, translation);
            nmlModelSetPlace(&place);
            nmlModelSetTexture(CARD_ICON_TEXTURE);
            nmlModelEntryCard(model);
        }
        number = 0;
        switch (definition->attackType) {
        case CARD_ATTACK_HOMING:
            number++;
        case CARD_ATTACK_SPREAD:
            number++;
        case CARD_ATTACK_BALLISTIC:
            number++;
            break;
        case CARD_ATTACK_HAND:
            break;
        default:
            number = -1;
            break;
        }
        if (number >= 0) {
            model = CARD_ATTACK_MODEL + number * CARD_MODEL_STRIDE;
            translation[0] = CardAttakPos[0];
            translation[1] = CardAttakPos[1];
            translation[2] = CardAttakPos[2];
            xglMatrixTrans(place, *matrix, translation);
            nmlModelSetPlace(&place);
            nmlModelSetTexture(CARD_ATTACK_TEXTURE);
            nmlModelEntryCard(model);
        }
        switch (definition->rarity) {
        case CARD_RARITY_P:
            model = CARD_MODEL_RARITY_P;
            break;
        case CARD_RARITY_R:
            model = CARD_MODEL_RARITY_R;
            break;
        case CARD_RARITY_U:
            model = CARD_MODEL_RARITY_U;
            break;
        case CARD_RARITY_O: /* never matched: the chain tests 0x50, then `slti 0x51` */
        default:
            model = CARD_MODEL_RARITY_C;
            break;
        }
        nmlModelSetPlace(matrix);
        nmlModelSetTexture(CARD_RARITY_TEXTURE);
        nmlModelEntryCard(model);
    }
    QuadwordCopyMatrix(place, *matrix);
    /* Kind overlay for owned cards, the unowned model otherwise. */
    if ((owned & CARD_DISP_ENABLED) == 0) {
        nmlModelSetPlace(&place);
        nmlModelSetTexture(CARD_UNOWNED_TEXTURE);
        nmlModelEntryCard(CARD_UNOWNED_MODEL);
    } else {
        switch (definition->kind) {
        case CARD_KIND_EVENT:
            model = CARD_MODEL_KIND2;
            break;
        case CARD_KIND_BATTLE:
            model = CARD_MODEL_KIND1;
            break;
        case CARD_KIND_UNUSED_3: /* never matched: the chain tests 2, then `slti 0x3` */
        default:
            model = CARD_MODEL_DEFAULT;
            break;
        }
        nmlModelSetPlace(&place);
        nmlModelSetTexture(CARD_OVERLAY_TEXTURE);
        nmlModelEntryCard(model);
        if (owned & CARD_DISP_OVERLAY) {
            nmlModelSetPlace(matrix);
            nmlModelSetTexture(CARD_OVERLAY_TEXTURE);
            nmlModelEntryCard(CARD_MODEL_OVERLAY);
        }
    }
    nmlModelSetPlace(&place);
    nmlModelSetTexture(CARD_BACK_TEXTURE);
    nmlModelEntryCard(CARD_BACK_MODEL);
    if (selected == 0) {
        return;
    }
    /* Caption and description text, drawn only when selected. */
    if (definition->captionOffset != 0) {
        xglFontPrint(CARD_TEXT_TITLE_X, CARD_TEXT_TITLE_Y, CARD_TEXT_COLOR, cardOtherRowFontCode);
        if (owned > 0) {
            CardDispTxtSub(CARD_TEXT_X, CARD_TEXT_CAPTION_Y, (unsigned char *)(CARD_DATA_BUFFER + definition->captionOffset), CARD_TEXT_CAPTION_COLOR);
        } else {
            CardDispTxtSub(CARD_TEXT_X, CARD_TEXT_CAPTION_Y, (unsigned char *)"\x0b\r\0\xa1\xa9\xa1\xa9\xa1\xa9\xa1\xa9\xa1\xa9", CARD_TEXT_CAPTION_COLOR);
        }
    }
    if (owned <= 0) {
        xglFontPrint(CARD_TEXT_X, CARD_TEXT_DESCRIPTION_Y, CARD_TEXT_COLOR, "\x0b\r\0\x0c@@\x80\xa1\xa9\xa1\xa9\xa1\xa9\xa1\xa9\xa1\xa9");
    } else if (definition->descriptionOffset != 0) {
        xglFontPrint(CARD_TEXT_X, CARD_TEXT_TITLE_Y, CARD_TEXT_COLOR, "\x0b\r\0\x0c@@\x80");
        xglFontPrint(0, 0, 0, cardRowPrefixFontCode);
        xglFontPrint(CARD_TEXT_X, CARD_TEXT_DESCRIPTION_Y, CARD_TEXT_COLOR, CARD_DATA_BUFFER + definition->descriptionOffset);
    }
    xglFontPrint(CARD_TEXT_TITLE_X, CARD_TEXT_TITLE_Y, CARD_TEXT_COLOR, "\x0b");
    if (selected & CARD_DISP_MARKER) {
        vector[1] = CARD_DISP_MARKER_Y;
        vector[2] = CARD_DISP_MARKER_Z;
        vector[0] = 0.0f;
        vector[3] = 1.0f;
        xglMatrixUnit(place);
        xglMatrixTrans(place, place, vector);
        vector[0] = CARD_DISP_MARKER_SCALE_X;
        vector[1] = CARD_DISP_MARKER_SCALE_Y;
        vector[2] = 1.0f;
        xglMatrixScale(place, place, vector);
        nmlModelSetPlace(&place);
        nmlModelSetTexture(CARD_HAND_TEXTURE);
        nmlModelEntryCard(CARD_HAND_MODEL);
    }
}

void CardSampleInitDeck0(s16 *deck)
{
    int i;

    for (i = 0; i < CARD_DECK_SIZE; i++) {
        deck[i] = SampleDeck0[i];
    }
}

void CardSampleInitDeck1(s16 *deck)
{
    int i;

    for (i = 0; i < CARD_DECK_SIZE; i++) {
        deck[i] = SampleDeck1[i];
    }
}

void CardSampleInitDeck2(s16 *deck)
{
    int i;

    for (i = 0; i < CARD_DECK_SIZE; i++) {
        deck[i] = SampleDeck2[i];
    }
}

void CardSampleInitDeck3(s16 *deck)
{
    int i;

    for (i = 0; i < CARD_DECK_SIZE; i++) {
        deck[i] = SampleDeck3[i];
    }
}

void CardSampleInitDeck4(s16 *deck)
{
    int i;

    for (i = 0; i < CARD_DECK_SIZE; i++) {
        deck[i] = SampleDeck4[i];
    }
}

void CardSampleInitDeck5(s16 *deck)
{
    int i;

    for (i = 0; i < CARD_DECK_SIZE; i++) {
        deck[i] = SampleDeck5[i];
    }
}

void CardSampleInitDeck6(s16 *deck)
{
    int i;

    for (i = 0; i < CARD_DECK_SIZE; i++) {
        deck[i] = SampleDeck6[i];
    }
}

void CardSampleInitDeck7(s16 *deck)
{
    int i;

    for (i = 0; i < CARD_DECK_SIZE; i++) {
        deck[i] = SampleDeck7[i];
    }
}

void CardSampleInitDeck8(s16 *deck)
{
    int i;

    for (i = 0; i < CARD_DECK_SIZE; i++) {
        deck[i] = SampleDeck8[i];
    }
}

void CardSampleInitDeck9(s16 *deck)
{
    int i;

    for (i = 0; i < CARD_DECK_SIZE; i++) {
        deck[i] = SampleDeck9[i];
    }
}

void CardCopyRom2Deck(s16 *deck, s32 enemyId)
{
    unsigned char *romDeck;
    int i;

    romDeck = EnemyDeckLst[enemyId];
    for (i = 0; i < CARD_DECK_SIZE; i++) {
        deck[i] = romDeck[i];
    }
}

/* Copies the forty card ids of stored deck `slot`, one byte each into a halfword
 * entry. The record address is one integer sum, save + slot * 32 + slot * 10
 * (slot * 42 bytes of save data), read through decks[0].cards. The original keeps
 * this form: indexing decks[slot].cards (one multiply by 42) and the pointer-sum
 * forms change the instruction order. */
void CardCopyRam2Deck(s16 *deck, CardGameWork *work, s32 slot)
{
    unsigned char *stored;
    int i;

    stored = ((CardSaveData *)((slot << 5) + (unsigned int)work->save + slot * 10))->decks[0].cards;
    for (i = CARD_DECK_SIZE - 1; i >= 0; i--) {
        *deck++ = *stored++;
    }
}

/* Collapses the 40 card ids of stored deck `slot` into (card id, copy count)
 * groups. */
void CardCopyRam2CLD(CardListWindow *list, CardGameWork *work, s32 slot)
{
    int prev;
    int groupIndex;
    int i;

    prev = -1;
    groupIndex = -1;
    for (i = 0; i < CARD_LIST_SIZE; i++) {
        if (prev != work->save->decks[slot].cards[i]) {
            groupIndex++;
            list->entries[groupIndex].cardId = work->save->decks[slot].cards[i];
            list->entries[groupIndex].count = 1;
        } else {
            list->entries[groupIndex].count++;
        }
        prev = work->save->decks[slot].cards[i];
    }
    list->count = groupIndex + 1;
}

/* Expands the (card id, copy count) groups of `list` back into the 40 card
 * ids of stored deck `slot`. */
void CardCopyCLD2Ram(CardListWindow *list, CardGameWork *work, s32 slot)
{
    int group;
    int copy;
    int cardIndex;

    cardIndex = 0;
    for (group = 0; group < list->count; group++) {
        for (copy = 0; copy < list->entries[group].count; copy++) {
            work->save->decks[slot].cards[cardIndex] = list->entries[group].cardId;
            cardIndex++;
        }
    }
}

/*
 * Compares the one's-complement checksum of a stored deck's 40 card bytes with
 * the checksum saved in the same record. The card bytes are addressed like
 * CardCopyRam2Deck's.
 */
s32 CardChkDeckRam(CardGameWork *work, int unused, s32 slot)
{
    u8 *deckByte;
    u16 storedChecksum;
    u8 sum;
    s32 remaining;

    deckByte = ((CardSaveData *) ((slot << 5) + (unsigned int) work->save + slot * 10))->decks[0].cards;
    sum = 0;
    remaining = CARD_DECK_SIZE - 1;
    do {
        sum += *deckByte;
        deckByte += 1;
        remaining -= 1;
    } while (remaining >= 0);
    storedChecksum = work->save->decks[slot].checksum;
    return (u8) ~sum == storedChecksum;
}

/*
 * Owned copies (0..3) of one card id. The save block packs two bits per card
 * id, four ids per byte, least significant bits first (CardSaveData.cardCounts).
 * The masks are 3, 0xC, 0x30 and 0xC0 for ids 0..3 of a byte.
 */
int CardCntList(CardGameWork *work, int index)
{
    u8 packed;
    int count;

    packed = work->save->cardCounts[index / CARD_COUNTS_PER_BYTE];
    count = 0;
    switch (index % CARD_COUNTS_PER_BYTE) {
    case 0:
        count = packed & CARD_COUNT_MASK(0);
        break;
    case 1:
        count = (packed & CARD_COUNT_MASK(1)) >> CARD_COUNT_BITS;
        break;
    case 2:
        count = (packed & CARD_COUNT_MASK(2)) >> (2 * CARD_COUNT_BITS);
        break;
    case 3:
        count = packed >> (3 * CARD_COUNT_BITS);
        break;
    }
    return count;
}

/*
 * Adds one owned copy of a card. Each card id has a
 * two-bit count: cardCounts holds four ids per byte, and the id's field is
 * cardId % 4 counted from the least significant bits (field 0 is bits 0..1,
 * field 1 bits 2..3, and so on). The count saturates at three; the result is
 * 1 when the card already had three copies, 0 otherwise.
 */
int CardAddList(CardGameWork *work, int cardId)
{
    unsigned int next;
    int overflow;
    u8 packed;

    packed = work->save->cardCounts[cardId / CARD_COUNTS_PER_BYTE];
    overflow = 0;
    switch (cardId % CARD_COUNTS_PER_BYTE) {
    case 0:
        next = (packed & CARD_COUNT_MASK(0)) + CARD_COUNT_STEP(0);
        if (next > CARD_COUNT_MASK(0)) {
            next = CARD_COUNT_MASK(0);
            overflow = 1;
        }
        packed = (packed & CARD_COUNT_KEEP(0)) + next;
        break;
    case 1:
        next = (packed & CARD_COUNT_MASK(1)) + CARD_COUNT_STEP(1);
        if (next > CARD_COUNT_MASK(1)) {
            next = CARD_COUNT_MASK(1);
            overflow = 1;
        }
        packed = (packed & CARD_COUNT_KEEP(1)) + next;
        break;
    case 2:
        next = (packed & CARD_COUNT_MASK(2)) + CARD_COUNT_STEP(2);
        if (next > CARD_COUNT_MASK(2)) {
            next = CARD_COUNT_MASK(2);
            overflow = 1;
        }
        packed = (packed & CARD_COUNT_KEEP(2)) + next;
        break;
    case 3:
        next = (packed & CARD_COUNT_MASK(3)) + CARD_COUNT_STEP(3);
        if (next > CARD_COUNT_MASK(3)) {
            next = CARD_COUNT_MASK(3);
            overflow = 1;
        }
        packed = (packed & CARD_COUNT_KEEP(3)) + next;
        break;
    }
    work->save->cardCounts[cardId / CARD_COUNTS_PER_BYTE] = packed;
    return overflow;
}

/*
 * Starts a game for one side: the zone reset of CPIWsub is
 * done inline, then every deck index is placed at a random position
 * (xglSRand() modulo the card count, as in CardPlayShuffleYama), the deck takes
 * the cards of the saved deck list in that order and six cards are drawn.
 */
void CardPlayInitWork(CardPlaySide *side, s16 *deck)
{
    s16 order[CARD_LIST_SIZE];
    int i;
    int slot;

    side->battle[0].cmdView.playedCard = -1;
    side->hand.deckCount = 0;
    side->hand.suteCount = 0;
    side->hand.junkCount = 0;
    for (i = 0; i < CARD_LIST_SIZE; i++) {
        side->hand.cards[i] = -1;
        side->deck[i] = -1;
        side->sute[i] = -1;
        side->junk[i] = -1;
        side->answerCards[i] = -1;
        side->handMove[i].cardId = 0;
        side->handMove[i].kind = 0;
        side->handMove[i].moveTimer = 0;
        side->yamaMove[i].cardId = 0;
        side->yamaMove[i].kind = 0;
        side->yamaMove[i].moveTimer = 0;
        side->suteMove[i].cardId = 0;
        side->suteMove[i].kind = 0;
        side->suteMove[i].moveTimer = 0;
        order[i] = -1;
        side->operations[i] = -1;
        side->operationFlags[i] = 0;
        side->operationDuration[i] = 0;
        side->operationMove[i].cardId = 0;
        side->operationMove[i].kind = 0;
        side->operationMove[i].moveTimer = 0;
        for (slot = 0; slot < CARD_BOARD_SLOTS; slot++) {
            side->battle[slot].battle.layers[i].cardId = -1;
            side->battle[slot].battle.layers[i].life = 0;
            side->battle[slot].battle.layers[i].shownLife = 0;
            side->battle[slot].battle.layers[i].damageTimer = 0;
            side->battle[slot].battle.layers[i].powerPlus = 0;
            side->battle[slot].battle.position[0] = BattlePos1P[slot][0];
            side->battle[slot].battle.position[1] = BattlePos1P[slot][1];
            side->battle[slot].battle.position[2] = BattlePos1P[slot][2];
            side->disposal[slot].battle.layers[i].cardId = -1;
            side->disposal[slot].battle.layers[i].life = 0;
            side->disposal[slot].battle.layers[i].shownLife = 0;
            side->disposal[slot].battle.layers[i].damageTimer = 0;
            side->disposal[slot].battle.layers[i].powerPlus = 0;
            side->disposal[slot].battle.position[0] = DisposePos1P[slot][0];
            side->disposal[slot].battle.position[1] = DisposePos1P[slot][1];
            side->disposal[slot].battle.position[2] = DisposePos1P[slot][2];
        }
    }
    /* Shuffle: each deck index goes to a random free position in order[]. */
    i = 0;
    while (i < CARD_LIST_SIZE) {
        int pick = xglSRand() % CARD_LIST_SIZE;

        if (order[pick] < 0) {
            order[pick] = i;
            i++;
        }
    }
    for (i = 0; i < CARD_LIST_SIZE; i++) {
        side->deck[i] = deck[order[i]];
    }
    for (i = 0; i < CARD_OPENING_DRAW_COUNT; i++) {
        CardPlayDrawCard(side);
    }
}

/*
 * Resets one side's zones: the hand, deck, discard, junk and
 * answer lists and the operation list are emptied (-1), the flag, duration
 * and movement records are cleared, and every layer of the four battle and
 * four disposal slots is cleared with the slot position copied from the
 * resting position tables. The deck argument is not read.
 */
void CPIWsub(CardPlaySide *side, s16 *deck)
{
    int i;
    int slot;

    side->battle[0].cmdView.playedCard = -1;
    side->hand.deckCount = 0;
    side->hand.suteCount = 0;
    side->hand.junkCount = 0;
    for (i = 0; i < CARD_LIST_SIZE; i++) {
        side->hand.cards[i] = -1;
        side->deck[i] = -1;
        side->sute[i] = -1;
        side->junk[i] = -1;
        side->answerCards[i] = -1;
        side->handMove[i].cardId = 0;
        side->handMove[i].kind = 0;
        side->handMove[i].moveTimer = 0;
        side->yamaMove[i].cardId = 0;
        side->yamaMove[i].kind = 0;
        side->yamaMove[i].moveTimer = 0;
        side->suteMove[i].cardId = 0;
        side->suteMove[i].kind = 0;
        side->suteMove[i].moveTimer = 0;
        side->operations[i] = -1;
        side->operationFlags[i] = 0;
        side->operationDuration[i] = 0;
        side->operationMove[i].cardId = 0;
        side->operationMove[i].kind = 0;
        side->operationMove[i].moveTimer = 0;
        for (slot = 0; slot < CARD_BOARD_SLOTS; slot++) {
            side->battle[slot].battle.layers[i].cardId = -1;
            side->battle[slot].battle.layers[i].life = 0;
            side->battle[slot].battle.layers[i].shownLife = 0;
            side->battle[slot].battle.layers[i].damageTimer = 0;
            side->battle[slot].battle.layers[i].powerPlus = 0;
            side->battle[slot].battle.position[0] = BattlePos1P[slot][0];
            side->battle[slot].battle.position[1] = BattlePos1P[slot][1];
            side->battle[slot].battle.position[2] = BattlePos1P[slot][2];
            side->disposal[slot].battle.layers[i].cardId = -1;
            side->disposal[slot].battle.layers[i].life = 0;
            side->disposal[slot].battle.layers[i].shownLife = 0;
            side->disposal[slot].battle.layers[i].damageTimer = 0;
            side->disposal[slot].battle.layers[i].powerPlus = 0;
            side->disposal[slot].battle.position[0] = DisposePos1P[slot][0];
            side->disposal[slot].battle.position[1] = DisposePos1P[slot][1];
            side->disposal[slot].battle.position[2] = DisposePos1P[slot][2];
        }
    }
}

/*
 * Sets up one side's card lists without shuffling. CPIWsub resets the zones
 * first; then the side's deck takes ids 0..33 of the deck list and the first
 * six hand slots take ids 34..39.
 */
void CardPlayNoInitWork(CardPlaySide *side, s16 *deck)
{
    int i;
    s16 *handCard;

    CPIWsub(side, deck);
    for (i = 0; i < CARD_LIST_SIZE - CARD_OPENING_DRAW_COUNT; i++) {
        side->deck[i] = deck[i];
    }
    handCard = side->hand.cards;
    for (; i < CARD_LIST_SIZE; i++) {
        *handCard = deck[i];
        handCard++;
    }
}

/*
 * Starts a game for the enemy side with a fixed opening. CPIWsub resets the
 * zones and the deck list is copied. Then,
 * one per round, up to CARD_FAKE_PICK_COUNT kind-1 cards with flags 1 are
 * picked: from the list entries at or after the round, the card with the
 * fewest set need bytes (the earliest one on a tie) that is not already
 * picked; its list entry becomes -1. The remaining entries are packed into
 * the deck, the deck is shuffled twice, the picks open the hand and the top
 * cards of the deck fill it up to CARD_OPENING_DRAW_COUNT.
 */
void CardPlayInitWorkFake(CardPlaySide *side, s16 *deck, CardDefinition *definitions)
{
    s32 round = 0;
    s32 pickCount = 0;
    s16 order[CARD_LIST_SIZE];
    s16 picked[CARD_FAKE_PICK_COUNT];
    s32 best;
    s32 candidate;
    s32 candidateNeeds;
    s32 found;
    s32 j;
    s32 i;
    s32 drawCount;
    s32 count;

    CPIWsub(side, deck);
    for (; round < CARD_LIST_SIZE; round++) {
        order[round] = deck[round];
    }

    /* Pick the opening-hand cards, one per round. */
    round = 0;
    do {
        best = -1;
        for (candidate = round; candidate < CARD_LIST_SIZE; candidate++) {
            if (order[candidate] < 0) {
                continue;
            }
            if (definitions[order[candidate]].kind != CARD_KIND_BATTLE) {
                continue;
            }
            if (definitions[order[candidate]].flags != CARD_ATTR_HUMAN) {
                continue;
            }
            found = 0;
            for (j = 0; j < pickCount; j++) {
                if (picked[j] == order[candidate]) {
                    found = 1;
                    break;
                }
            }
            if (found) {
                continue;
            }
            if (best < 0) {
                best = candidate;
            } else {
                /* found is reused as the set need byte count of the best card */
                found = 0;
                candidateNeeds = 0;
                for (j = 0; j < CARD_CONDITION_COUNT; j++) {
                    if (definitions[order[best]].need[j] != 0) {
                        found++;
                    }
                    if (definitions[order[candidate]].need[j] != 0) {
                        candidateNeeds++;
                    }
                }
                if (candidateNeeds < found) {
                    best = candidate;
                }
            }
        }
        if (best < 0 || pickCount >= CARD_FAKE_PICK_COUNT) {
            break;
        }
        picked[pickCount] = order[best];
        pickCount++;
        order[best] = -1;
        round++;
    } while (round < CARD_LIST_SIZE);

    /* The cards that were not picked form the draw pile. */
    count = 0;
    for (i = 0; i < CARD_LIST_SIZE; i++) {
        if (order[i] >= 0) {
            side->deck[count] = order[i];
            count++;
        }
    }
    CardPlayShuffleYama(side);
    CardPlayShuffleYama(side);
    for (i = 0; i < pickCount; i++) {
        side->hand.cards[i] = picked[i];
    }

    /* best is reused for the draw pile size; the hand takes the top cards */
    best = CardPlayYamaCnt(side);
    drawCount = CARD_OPENING_DRAW_COUNT - pickCount;
    for (candidate = 0; candidate < drawCount; candidate++) {
        side->hand.cards[i] = side->deck[best - 1];
        side->deck[best - 1] = -1;
        i++;
        best--;
    }
}

/*
 * Shuffles the draw pile (side->deck, CardPlayYamaCnt cards).
 * Every draw index in turn is placed at a random position, chosen with
 * xglSRand() modulo the pile size, until all indices are placed. Position i
 * then receives the card that was at the draw index order[i] before.
 */
void CardPlayShuffleYama(CardPlaySide *side)
{
    s16 order[CARD_LIST_SIZE];
    s16 original[CARD_LIST_SIZE];
    int count;
    int i;

    count = CardPlayYamaCnt(side);
    for (i = 0; i < count; i++) {
        order[i] = -1;
        original[i] = side->deck[i];
    }
    i = 0;
    while (i < count) {
        int pick = xglSRand() % count;

        if (order[pick] < 0) {
            order[pick] = i;
            i++;
        }
    }
    for (i = 0; i < count; i++) {
        side->deck[i] = original[order[i]];
    }
}

/*
 * Draws the top card of the deck (side->deck) into the first free hand slot
 * and starts its slide to the hand. The top is found by walking from deck[1]
 * while the ids stay non-negative, capped at index 39 (index 0 when deck[1]
 * is negative). Returns 1 when the deck is empty, 0 otherwise. The card id is
 * copied as an unsigned halfword (`lhu`).
 */
s32 CardPlayDrawCard(CardPlaySide *side)
{
    s32 top;
    s32 slot;
    s16 *cardPtr;

    top = 0;
    cardPtr = &side->deck[1];
    while (top < CARD_LIST_SIZE - 1 && *cardPtr >= 0) {
        top++;
        cardPtr++;
    }
    if (top == 0 && side->deck[0] < 0) {
        return 1;
    }
    for (slot = 0; slot < CARD_LIST_SIZE; slot++) {
        if (side->hand.cards[slot] < 0) {
            break;
        }
    }
    side->hand.cards[slot] = side->deck[top];
    side->handMove[slot].moveTimer = CARD_SLIDE_FRAMES;
    side->handMove[slot].kind = CARD_MOVE_KIND_GROW;
    side->handMove[slot].position[0] = 0.0f;
    side->handMove[slot].position[1] = CARD_SUTE_PILE_Y;
    side->handMove[slot].position[2] = CARD_DRAW_START_Z;
    side->handMove[slot].step[0] = 0.0f;
    side->handMove[slot].step[1] = CARD_DRAW_STEP_Y;
    side->handMove[slot].step[2] = CARD_DRAW_STEP_Z;
    side->deck[top] = -1;
    xglSoundEffectNormalID(CARD_DRAW_SOUND, 0);
    return 0;
}

/*
 * Draws the top card of the discard pile (side->sute) into the first free
 * hand slot and starts its slide from the pile to the hand. The top is found
 * by walking from sute[1] while the ids stay non-negative, capped at index 39
 * (index 0 when sute[1] is negative). Returns 1 when the discard pile is empty,
 * 0 otherwise. The card id is copied as an unsigned halfword (`lhu`).
 */
s32 CardPlayDrawSute(CardPlaySide *side)
{
    s32 top;
    s32 slot;
    s16 *cardPtr;

    top = 0;
    cardPtr = &side->sute[1];
    while (top < CARD_LIST_SIZE - 1 && *cardPtr >= 0) {
        top++;
        cardPtr++;
    }
    if (top == 0 && side->sute[0] < 0) {
        return 1;
    }
    for (slot = 0; slot < CARD_LIST_SIZE; slot++) {
        if (side->hand.cards[slot] < 0) {
            break;
        }
    }
    side->hand.cards[slot] = side->sute[top];
    side->handMove[slot].moveTimer = CARD_SLIDE_FRAMES;
    side->handMove[slot].kind = CARD_MOVE_KIND_GROW;
    side->handMove[slot].position[0] = CARD_SUTE_PILE_X;
    side->handMove[slot].position[1] = CARD_SUTE_PILE_Y;
    side->handMove[slot].position[2] = CARD_DRAW_START_Z;
    side->handMove[slot].step[0] = 0.0f;
    side->handMove[slot].step[1] = CARD_DRAW_STEP_Y;
    side->handMove[slot].step[2] = CARD_DRAW_STEP_Z;
    side->sute[top] = -1;
    return 0;
}

/*
 * Puts the top card of the discard pile back on the deck, at index
 * CardPlayYamaCnt, and starts its slide from the pile to the deck. The top is
 * found as in CardPlayDrawSute. Returns 1 when the discard pile is empty, 0
 * otherwise.
 */
s32 CardPlayRecavery(CardPlaySide *side)
{
    s32 top;
    s32 count;
    s16 *cardPtr;

    top = 0;
    cardPtr = &side->sute[1];
    while (top < CARD_LIST_SIZE - 1 && *cardPtr >= 0) {
        top++;
        cardPtr++;
    }
    if (top == 0 && side->sute[0] < 0) {
        return 1;
    }
    count = CardPlayYamaCnt(side);
    side->deck[count] = side->sute[top];
    side->yamaMove[count].moveTimer = CARD_SLIDE_FRAMES;
    side->yamaMove[count].kind = 0;
    side->yamaMove[count].position[0] = CARD_SUTE_PILE_X;
    side->yamaMove[count].position[1] = CARD_SUTE_PILE_Y;
    side->yamaMove[count].position[2] = CARD_PILE_MOVE_Z;
    side->yamaMove[count].step[0] = -CARD_SUTE_PILE_X / CARD_SLIDE_FRAMES;
    side->yamaMove[count].step[1] = 0.0f;
    side->sute[top] = -1;
    side->yamaMove[count].step[2] = 0.0f;
    return 0;
}

/* Moves the non-negative hand ids to the front, keeping their order. */
void CardPlayCleanHand(CardHand *hand)
{
    s32 kept;
    s32 i;

    kept = 0;
    for (i = 0; i < CARD_LIST_SIZE; i++) {
        if (hand->cards[i] >= 0) {
            if (i != kept) {
                hand->cards[kept] = hand->cards[i];
                hand->cards[i] = -1;
            }
            kept++;
        }
    }
}

/* Moves the non-negative draw-pile ids to the front, keeping their order. */
void CardPlayCleanYama(CardPlaySide *side)
{
    s32 kept;
    s32 i;

    kept = 0;
    for (i = 0; i < CARD_LIST_SIZE; i++) {
        if (side->deck[i] >= 0) {
            if (i != kept) {
                side->deck[kept] = side->deck[i];
                side->deck[i] = -1;
            }
            kept++;
        }
    }
}

/* Moves the non-negative junk ids to the front, keeping their order. */
void CardPlayCleanJunk(CardPlaySide *side)
{
    s32 kept;
    s32 i;

    kept = 0;
    for (i = 0; i < CARD_LIST_SIZE; i++) {
        if (side->junk[i] >= 0) {
            if (i != kept) {
                side->junk[kept] = side->junk[i];
                side->junk[i] = -1;
            }
            kept++;
        }
    }
}

/*
 * Sends the hand card at index to the first free junk slot: the junk entry
 * takes the card id, and its move record starts at the hand card's position
 * (handMove[index]) and slides to JunkPos1P[1] in CARD_SLIDE_FRAMES frames.
 * The hand entry becomes -1 and the hand is closed up. Always returns 0.
 */
s32 CardPlayDesertCard(CardPlaySide *side, s32 index)
{
    s32 slot;

    for (slot = 0; slot < CARD_JUNK_SLOT_COUNT; slot++) {
        if (side->junk[slot] < 0) {
            break;
        }
    }
    side->junkMove[slot].position[0] = side->handMove[index].position[0];
    side->junkMove[slot].position[1] = side->handMove[index].position[1];
    side->junkMove[slot].position[2] = side->handMove[index].position[2];
    side->junk[slot] = side->hand.cards[index];
    side->junkMove[slot].step[0] =
        (JunkPos1P[1][0] - side->junkMove[slot].position[0]) / CARD_SLIDE_FRAMES;
    side->junkMove[slot].step[1] =
        (JunkPos1P[1][1] - side->junkMove[slot].position[1]) / CARD_SLIDE_FRAMES;
    side->junkMove[slot].step[2] =
        (JunkPos1P[1][2] - side->junkMove[slot].position[2]) / CARD_SLIDE_FRAMES;
    side->junkMove[slot].moveTimer = CARD_SLIDE_FRAMES;
    side->hand.cards[index] = -1;
    CardPlayCleanHand(&side->hand);
    return 0;
}

/*
 * Moves the hand cards at firstIndex and secondIndex to the junk pile: each goes into the first empty junk slot, and its hand slot is
 * set to -1. CardPlayCleanHand then closes the gaps in the hand.
 */
int CardPlayDesert2Card(CardPlaySide *side, s32 firstIndex, s32 secondIndex)
{
    s32 junkIndex;

    for (junkIndex = 0; junkIndex < CARD_JUNK_SLOT_COUNT; junkIndex++) {
        if (side->junk[junkIndex] < 0) {
            break;
        }
    }
    side->junk[junkIndex] = side->hand.cards[firstIndex];
    side->hand.cards[firstIndex] = -1;

    for (junkIndex = 0; junkIndex < CARD_JUNK_SLOT_COUNT; junkIndex++) {
        if (side->junk[junkIndex] < 0) {
            break;
        }
    }
    side->junk[junkIndex] = side->hand.cards[secondIndex];
    side->hand.cards[secondIndex] = -1;
    CardPlayCleanHand(&side->hand);
    return 0;
}

/*
 * Sends the operation card operationIndex to the first free junk slot: the
 * junk entry takes the operation's card id, and its move record starts at the
 * operation's position and slides to JunkPos1P[1] in CARD_SLIDE_FRAMES frames.
 * Always returns 0.
 */
s32 CardPlayDesertOperation(CardPlaySide *side, s32 operationIndex)
{
    s32 slot;

    for (slot = 0; slot < CARD_JUNK_SLOT_COUNT; slot++) {
        if (side->junk[slot] < 0) {
            break;
        }
    }
    side->junkMove[slot].position[0] = side->operationMove[operationIndex].position[0];
    side->junkMove[slot].position[1] = side->operationMove[operationIndex].position[1];
    side->junkMove[slot].position[2] = CARD_JUNK_MOVE_Z;
    side->junkMove[slot].step[0] =
        (JunkPos1P[1][0] - side->junkMove[slot].position[0]) / CARD_SLIDE_FRAMES;
    side->junkMove[slot].step[1] =
        (JunkPos1P[1][1] - side->junkMove[slot].position[1]) / CARD_SLIDE_FRAMES;
    side->junkMove[slot].step[2] = 0.0f;
    side->junkMove[slot].moveTimer = CARD_SLIDE_FRAMES;
    side->junk[slot] = side->operations[operationIndex];
    side->operations[operationIndex] = -1;
    return 0;
}

/*
 * Sends every card of the battle stack to the junk pile: the
 * cards fill the junk slots from the first free one, and each move record
 * starts at the stack's position and slides to JunkPos1P[1] in
 * CARD_SLIDE_FRAMES frames, like CardPlayDesertOperation. Always returns 0.
 */
s32 CardPlayDesertBattle(CardPlaySide *side, CardLayerStack *battle)
{
    s32 slot;
    s32 layer;

    for (slot = 0; slot < CARD_JUNK_SLOT_COUNT; slot++) {
        if (side->junk[slot] < 0) {
            break;
        }
    }
    for (layer = 0; layer < CARD_BATTLE_LAYER_COUNT; layer++) {
        if (battle->layers[layer].cardId >= 0) {
            side->junkMove[slot].position[0] = battle->position[0];
            side->junkMove[slot].position[1] = battle->position[1];
            side->junkMove[slot].position[2] = CARD_JUNK_MOVE_Z;
            side->junkMove[slot].step[0] =
                (JunkPos1P[1][0] - side->junkMove[slot].position[0]) / CARD_SLIDE_FRAMES;
            side->junkMove[slot].step[1] =
                (JunkPos1P[1][1] - side->junkMove[slot].position[1]) / CARD_SLIDE_FRAMES;
            side->junkMove[slot].step[2] = 0.0f;
            side->junkMove[slot].moveTimer = CARD_SLIDE_FRAMES;
            side->junk[slot] = battle->layers[layer].cardId;
            battle->layers[layer].cardId = -1;
            slot++;
        }
    }
    return 0;
}

/*
 * Moves the top deck card onto the first free discard slot and starts its
 * slide from the deck to the discard pile. The deck top is found by walking
 * from deck[1] while the ids stay non-negative, capped at index 40 (deck[40]
 * is sute[0]). Returns 1 when the deck is empty, 0 otherwise.
 */
s32 CardPlayCostCard(CardPlaySide *side)
{
    s32 deckTop;
    s32 slot;
    s16 *cardPtr;

    deckTop = 0;
    cardPtr = &side->deck[1];
    while (deckTop < CARD_LIST_SIZE && *cardPtr >= 0) {
        deckTop++;
        cardPtr++;
    }
    slot = 0;
    cardPtr = &side->sute[0];
    while (slot < CARD_LIST_SIZE && *cardPtr >= 0) {
        slot++;
        cardPtr++;
    }
    if (deckTop == 0 && side->deck[0] < 0) {
        return 1;
    }
    side->sute[slot] = side->deck[deckTop];
    side->suteMove[slot].moveTimer = CARD_SLIDE_FRAMES;
    side->suteMove[slot].kind = 0;
    side->suteMove[slot].position[0] = 0.0f;
    side->suteMove[slot].position[1] = CARD_SUTE_PILE_Y;
    side->suteMove[slot].position[2] = CARD_PILE_MOVE_Z;
    side->suteMove[slot].step[0] = CARD_SUTE_PILE_X / CARD_SLIDE_FRAMES;
    side->suteMove[slot].step[1] = 0.0f;
    side->suteMove[slot].step[2] = 0.0f;
    side->deck[deckTop] = -1;
    return 0;
}

/*
 * Moves the card ids stacked on battle slot `index` onto the end of the hand
 * and clears the slot. `battle` points at battle[0].battle of the
 * side's board, so slots are one CardBoardSlot apart. The slot address is
 * recomputed in the loop and in the epilogue through BATTLE_SLOT; alternatives
 * that do not reproduce the original code: a hoisted slot local, an inline
 * helper and board[index].battle (its offset would be 12 bytes too far, since
 * `battle` already addresses battle[0].battle) all change the generated code.
 */
#define BATTLE_SLOT(battle, index) ((CardLayerStack *) ((CardBoardSlot *) (battle) + (index)))

int CardPlayReturnCard(CardHand *hand, CardLayerStack *battle, s16 index)
{
    s16 *cards;
    int count;
    int i;

    cards = hand->cards;
    for (count = 0; count < CARD_LIST_SIZE && cards[count] >= 0; count++) {
    }
    for (i = 0; i < CARD_BATTLE_LAYER_COUNT; i++) {
        if (BATTLE_SLOT(battle, index)->layers[i].cardId >= 0) {
            cards[count++] = BATTLE_SLOT(battle, index)->layers[i].cardId;
            BATTLE_SLOT(battle, index)->layers[i].life = -1;
            BATTLE_SLOT(battle, index)->layers[i].cardId = -1;
        }
    }
    BATTLE_SLOT(battle, index)->flags = 0;
    BATTLE_SLOT(battle, index)->power = -1;
    BATTLE_SLOT(battle, index)->powerLimit = -1;
    return 0;
}

#undef BATTLE_SLOT

/* Counts the non-negative ids of the draw pile. */
s32 CardPlayYamaCnt(CardPlaySide *side)
{
    s16 *card;
    s16 id;
    s32 count;
    s32 remaining;

    card = side->deck;
    count = 0;
    remaining = CARD_LIST_SIZE - 1;
    do {
        id = *card;
        card += 1;
        remaining -= 1;
        if (id >= 0) {
            count = count + 1;
        }
    } while (remaining >= 0);
    return count;
}

/* Counts the non-negative ids of the discard pile. */
s32 CardPlaySuteCnt(CardPlaySide *side)
{
    s16 *card;
    s16 id;
    s32 count;
    s32 remaining;

    card = side->sute;
    count = 0;
    remaining = CARD_LIST_SIZE - 1;
    do {
        id = *card;
        card += 1;
        remaining -= 1;
        if (id >= 0) {
            count = count + 1;
        }
    } while (remaining >= 0);
    return count;
}

/* Counts the non-negative ids of the junk pile. */
s32 CardPlayJunkCnt(CardPlaySide *side)
{
    s16 *card;
    s16 id;
    s32 count;
    s32 remaining;

    card = side->junk;
    count = 0;
    remaining = CARD_LIST_SIZE - 1;
    do {
        id = *card;
        card += 1;
        remaining -= 1;
        if (id >= 0) {
            count = count + 1;
        }
    } while (remaining >= 0);
    return count;
}

/* Counts the non-negative ids of the operation list. */
s32 CardPlayOperationCnt(CardPlaySide *side)
{
    s16 *op;
    s16 id;
    s32 count;
    s32 remaining;

    op = side->operations;
    count = 0;
    remaining = CARD_LIST_SIZE - 1;
    do {
        id = *op;
        op += 1;
        remaining -= 1;
        if (id >= 0) {
            count = count + 1;
        }
    } while (remaining >= 0);
    return count;
}

/* Counts the occupied disposal slots (base layer card id >= 0). */
s32 CardPlayDisposeCnt(CardPlaySide *side)
{
    s16 *cardId;
    s16 id;
    s32 count;
    s32 remaining;

    cardId = &side->disposal[0].battle.layers[0].cardId;
    count = 0;
    remaining = CARD_BOARD_SLOTS - 1;
    do {
        id = *cardId;
        cardId += CARD_SLOT_STRIDE_HALFWORDS;
        remaining -= 1;
        if (id >= 0) {
            count = count + 1;
        }
    } while (remaining >= 0);
    return count;
}

/* Counts the non-negative ids of the hand. */
s32 CardPlayHandCnt(CardHand *hand)
{
    s16 *card;
    s16 id;
    s32 count;
    s32 remaining;

    card = hand->cards;
    count = 0;
    remaining = CARD_LIST_SIZE - 1;
    do {
        id = *card;
        card += 1;
        remaining -= 1;
        if (id >= 0) {
            count = count + 1;
        }
    } while (remaining >= 0);
    return count;
}

/* Counts the occupied battle slots (base layer card id >= 0). */
s32 CardPlayBattleCnt(CardPlaySide *side)
{
    s16 *cardId;
    s16 id;
    s32 count;
    s32 remaining;

    cardId = &side->battle[0].battle.layers[0].cardId;
    count = 0;
    remaining = CARD_BOARD_SLOTS - 1;
    do {
        id = *cardId;
        cardId += CARD_SLOT_STRIDE_HALFWORDS;
        remaining -= 1;
        if (id >= 0) {
            count = count + 1;
        }
    } while (remaining >= 0);
    return count;
}

/*
 * Sorts the count cards by kind: for each of the three kinds
 * in order[], every pick moves the lowest card id of that kind still in
 * cards[] into sorted[] and marks its slot -1; sorted[] is then copied back.
 * best and bestValue keep their last values across picks, so a pick that
 * finds no card of the kind retries the previous slot.
 *
 * The original also copies the hand into a second local array before the
 * sort (a copy loop with `sh` stores) and never reads it
 * again. Without the array and its loop the frame shrinks from 0xB0 to 0x60
 * bytes, 10 instructions disappear and the register assignment shifts.
 */
void CardPlayTypeSort(CardDefinition *definitions, s16 *cards, s32 count, s8 *order)
{
    s16 sorted[CARD_SORT_MAX_CARDS];
    s16 backup[CARD_SORT_MAX_CARDS];
    s32 i;
    s32 j;
    s32 pass;
    s32 moved;
    s32 best;
    s32 bestValue;

    best = 0;
    bestValue = 0;
    for (i = 0; i < count; i++) {
        backup[i] = cards[i];
    }
    moved = 0;
    for (pass = 0; pass < CARD_SORT_KIND_COUNT; pass++) {
        for (i = 0; i < count; i++) {
            for (j = 0; j < count; j++) {
                if (cards[j] >= 0 && order[pass] == definitions[cards[j]].kind) {
                    best = j;
                    bestValue = cards[j];
                    break;
                }
            }
            for (j = 0; j < count; j++) {
                if (cards[j] >= 0 && order[pass] == definitions[cards[j]].kind
                    && cards[j] < bestValue) {
                    bestValue = cards[j];
                    best = j;
                }
            }
            if (cards[best] >= 0) {
                sorted[moved] = cards[best];
                cards[best] = -1;
                moved++;
            }
        }
    }
    for (i = 0; i < count; i++) {
        cards[i] = sorted[i];
    }
}

/*
 * Check whether a card cannot be played for lack of room.
 * A card whose kind has bit 0x1 needs a free disposal slot (base layer card id
 * negative), unless its flags equal CARD_FLAGS_NO_SLOT_CHECK; any other
 * card needs a free operation slot. Only the first four slots are searched.
 * Returns 1 when no slot is free, 0 otherwise. Once the definition has been
 * read, the cardId parameter is reused as the "slot found" flag (it lives in
 * $a2, `sltiu $v0,$a2,1`). A separate local only ever set to 0
 * or 1 makes the compiler fold `!flag` into `xori`; a pseudo live at function
 * entry keeps the `sltiu` form. The work argument is not read.
 */
s32 CardPlayChkCost(CardDefinition *definitions, CardPlaySide *side, s32 cardId, CardGameWork *work)
{
    int slot;

    if (definitions[cardId].kind & CARD_KIND_BATTLE) {
        if (definitions[cardId].flags & CARD_FLAGS_NO_SLOT_CHECK) {
            cardId = 1;
        } else {
            cardId = 0;
            for (slot = 0; slot < CARD_BOARD_SLOTS; slot++) {
                if (side->disposal[slot].battle.layers[0].cardId < 0) {
                    cardId = 1;
                    break;
                }
            }
        }
    } else {
        cardId = 0;
        for (slot = 0; slot < CARD_BOARD_SLOTS; slot++) {
            if (side->operations[slot] < 0) {
                cardId = 1;
                break;
            }
        }
    }
    return !cardId;
}

/*
 * Counts the cards of one side by generation: counts[0..4] are cleared, then
 * the four disposal slots (skipped when empty or flagged in battle.flags bit 0)
 * and the four battle slots each add one to the bucket picked from the card
 * definition flags (realian -> 1, mech -> 2, gnosis -> 3, unknown -> 4, anything else -> 0).
 * The battle slots are walked with a pointer to their cardId, advanced by one
 * CardBoardSlot per step (the original keeps it in a register).
 */
void CardCountGeneration(u8 *counts, CardDefinition *definitions, CardPlaySide *side)
{
    s16 *battleCardId;
    int i;
    int index;
    int cardId;

    for (i = 0; i < CARD_GENERATION_COUNT; i++) {
        counts[i] = 0;
    }
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        cardId = side->disposal[i].battle.layers[0].cardId;
        if (cardId >= 0 && !(side->disposal[i].battle.flags & CARD_STACK_FLAG_END)) {
            index = 0;
            /* Each label falls through to the next and moves the generation up one bucket. */
            switch (definitions[cardId].flags) {
            case CARD_ATTR_UNKNOWN:
                index++;
            case CARD_ATTR_GNOSIS:
                index++;
            case CARD_ATTR_MECH:
                index++;
            case CARD_ATTR_REALIAN:
                index++;
                break;
            default:
                break;
            }
            counts[index]++;
        }
    }
    battleCardId = &side->battle[0].battle.layers[0].cardId;
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        cardId = *battleCardId;
        if (cardId >= 0) {
            index = 0;
            /* Each label falls through to the next and moves the generation up one bucket. */
            switch (definitions[cardId].flags) {
            case CARD_ATTR_UNKNOWN:
                index++;
            case CARD_ATTR_GNOSIS:
                index++;
            case CARD_ATTR_MECH:
                index++;
            case CARD_ATTR_REALIAN:
                index++;
                break;
            default:
                break;
            }
            counts[index]++;
        }
        battleCardId += sizeof(CardBoardSlot) / sizeof(*battleCardId);
    }
}

/*
 * Checks whether `side` may play card `cardId`; returns 0 if it may, else a
 * CARD_CONDITION_* code. Every non-zero requirement byte must share a bit with
 * the definition flags of a card on the board: first `need` against the battle
 * slots and the disposal slots whose stack has not ended (code 2), then
 * `discardRequirements` against the battle slots and every disposal slot
 * (code 4). A battle card is also refused (code 5) when CardPlayIdentityCheck
 * finds a card with the same identity on the side.
 */
s32 CardPlayChkCondition(CardDefinition *definitions, CardPlaySide *side, s32 cardId, CardGameWork *work)
{
    u8 marks[CARD_CHECK_MARK_COUNT];
    s32 satisfied;
    s32 needBits;
    s32 needCount;
    s32 markCount;
    s32 i;
    s32 j;

    satisfied = 0;
    for (i = 0; i < CARD_CHECK_MARK_COUNT; i++) {
        marks[i] = 0;
    }
    needBits = 0;
    needCount = 0;
    for (i = 0; i < CARD_CONDITION_COUNT; i++) {
        if (definitions[cardId].need[i] != 0) {
            needCount++;
            needBits |= definitions[cardId].need[i];
        }
    }
    if (needBits != 0) {
        markCount = 0;
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (side->battle[i].battle.layers[0].cardId >= 0) {
                marks[markCount++] = definitions[side->battle[i].battle.layers[0].cardId].flags;
            }
        }
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (side->disposal[i].battle.layers[0].cardId >= 0
                && !(side->disposal[i].battle.flags & CARD_STACK_FLAG_END)) {
                marks[markCount++] = definitions[side->disposal[i].battle.layers[0].cardId].flags;
            }
        }
        /* Each mark satisfies at most one requirement. */
        needBits = 0;
        for (i = 0; i < needCount; i++) {
            for (j = 0; j < markCount; j++) {
                if ((definitions[cardId].need[i] & marks[j]) != 0) {
                    marks[j] = 0;
                    needBits++;
                    break;
                }
            }
        }
        if (needBits >= needCount) {
            satisfied = 1;
        }
    } else {
        satisfied = 1;
    }
    if (!satisfied) {
        return CARD_CONDITION_NEED_UNMET;
    }

    /*
     * Pass 2 reads the discard requirement bytes as `*(array + i)`: the array-pointer
     * read keeps the address a strength-reduced pointer (`lbu $10,0($12)`), while
     * `array[i]` recomputes it with an `addu` in every iteration and adds three
     * instructions. Pass 1 indexes need[i].
     */
    satisfied = 0;
    for (i = 0; i < CARD_CHECK_MARK_COUNT; i++) {
        marks[i] = 0;
    }
    needBits = 0;
    needCount = 0;
    for (i = 0; i < CARD_CONDITION_COUNT; i++) {
        if (*(definitions[cardId].discardRequirements + i) != 0) {
            needCount++;
            needBits |= *(definitions[cardId].discardRequirements + i);
        }
    }
    if (needBits != 0) {
        markCount = 0;
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (side->battle[i].battle.layers[0].cardId >= 0) {
                marks[markCount++] = definitions[side->battle[i].battle.layers[0].cardId].flags;
            }
        }
        for (i = 0; i < CARD_BOARD_SLOTS; i++) {
            if (side->disposal[i].battle.layers[0].cardId >= 0) {
                marks[markCount++] = definitions[side->disposal[i].battle.layers[0].cardId].flags;
            }
        }
        /* Here one requirement may consume several marks. */
        needBits = 0;
        for (i = 0; i < needCount; i++) {
            for (j = 0; j < markCount; j++) {
                if ((*(definitions[cardId].discardRequirements + i) & marks[j]) != 0) {
                    marks[j] = 0;
                    needBits++;
                }
            }
        }
        if (needBits >= needCount) {
            satisfied = 1;
        }
    } else {
        satisfied = 1;
    }
    if (!satisfied) {
        return CARD_CONDITION_DISCARD_UNMET;
    }

    if (definitions[cardId].kind & CARD_KIND_BATTLE) {
        if (CardPlayIdentityCheck(work, side, work->definitions[cardId].identity) != 0) {
            return CARD_CONDITION_IDENTITY_TAKEN;
        }
    }
    return 0;
}

/*
 * The attack type of the topmost card in play: the stack's layers are walked
 * from layers[39] down, skipping empty layers (cardId < 0) and the defensive
 * equipment Battle Suit and Shield. The result is held in a u8, the width of
 * CardDefinition.attackType.
 */
s32 CardChkAttackType(CardDefinition *definitions, CardLayerStack *card)
{
    u8 attackType;
    int i;

    attackType = 0;
    for (i = CARD_BATTLE_LAYER_COUNT - 1; i >= 0; i--) {
        if (card->layers[i].cardId >= 0) {
            if (card->layers[i].cardId == CARD_ID_BATTLE_SUIT || card->layers[i].cardId == CARD_ID_SHIELD) {
                continue;
            }
            attackType = definitions[card->layers[i].cardId].attackType;
            break;
        }
    }
    return attackType;
}

/*
 * Adjusts the board cursor after a move: clamps its position to 0..3, resets
 * the highlight scale and sets the highlight size; position 0 also draws the
 * card of the disposal slot in the cursor's zone when drawFlags asks for it.
 * previousZone is the zone before the move (0 = none).
 */
void CPCMSub(CardCursorControl *cursor, CardPlaySide *side, u16 buttons, u16 drawFlags, s32 previousZone)
{
    Matrix4 matrix;
    float offset[4];
    s16 cardId;

    if (cursor->cursorIndex < 0) {
        cursor->cursorIndex = 0;
    }
    if (cursor->cursorIndex >= CARD_CURSOR_INDEX_COUNT) {
        cursor->cursorIndex = CARD_CURSOR_INDEX_COUNT - 1;
    }
    cursor->highlightScaleY = 1.0f;
    cursor->highlightScaleX = 1.0f;
    if (cursor->cursorIndex == 0) {
        cursor->highlightY = CARD_CURSOR_EDGE_HIGHLIGHT;
        cursor->highlightX = CARD_CURSOR_EDGE_HIGHLIGHT;
        if (!(drawFlags & CARD_CURSOR_SHOW_CARD_FLAG)) {
            return;
        }
        cardId = side->disposal[cursor->cursorZone].battle.layers[0].cardId & CARD_CURSOR_CARD_ID_MASK;
        if (cardId < 0) {
            return;
        }
        offset[0] = 0.0f;
        offset[1] = 1.0f;
        offset[2] = CARD_CURSOR_PREVIEW_Z;
        offset[3] = 1.0f;
        xglMatrixUnit(matrix);
        xglMatrixTrans(matrix, matrix, offset);
        CardDisp(&matrix, cardId, 1, 1);
        return;
    }
    if (cursor->cursorIndex == CARD_CURSOR_INDEX_COUNT - 1) {
        cursor->highlightY = CARD_CURSOR_EDGE_HIGHLIGHT;
        cursor->highlightX = CARD_CURSOR_EDGE_HIGHLIGHT;
        return;
    }
    cursor->highlightY = CARD_CURSOR_INNER_HIGHLIGHT;
    cursor->highlightX = CARD_CURSOR_INNER_HIGHLIGHT;
    if (previousZone == 0 && (buttons & CARD_CURSOR_JUMP_BUTTON)) {
        cursor->cursorZone = CARD_CURSOR_ZONE_2;
        return;
    }
    if (cursor->cursorZone == CARD_CURSOR_ZONE_1 || cursor->cursorZone == CARD_CURSOR_ZONE_3) {
        cursor->cursorZone &= CARD_CURSOR_PAIR_ZONE_MASK;
    }
}

/*
 * Moves the board cursor with the pad buttons.
 * The zone steps back (0x1000) or forward (0x4000) and the index forward
 * (0x2000) or back (0x8000); the zone is clamped to 0..4. Zones 0..3 go to
 * CPCMSub. The hand zone (4) sets the hand highlight and places the cursor on
 * the hand: the index is clamped to the hand count and the position is spread
 * along HandPos1P. An empty hand moves the cursor to zone 3 and goes to CPCMSub.
 * The zone and index are used through the cursor fields (each store feeds the
 * next test). After CPCMSub the position is copied from the table entry of the
 * zone and index. The copy is written on each of the two call paths (zones 0..3,
 * and the empty hand) because the compiler merges the identical tails into the
 * single jal/copy (reached by a branch and by fall-through); a single shared
 * call or a static inline copy helper does not reproduce that layout.
 */
void CardPlayCursorMove(CardCursorControl *cursor, CardPlaySide *side, u16 buttons, u16 drawFlags)
{
    s32 previousZone;
    s32 count;
    float *slot;

    previousZone = cursor->cursorZone;
    if (buttons & CARD_CURSOR_ZONE_BACK_BUTTON) {
        cursor->cursorZone = cursor->cursorZone - 1;
    }
    if (buttons & CARD_CURSOR_INDEX_FORWARD_BUTTON) {
        cursor->cursorIndex++;
    }
    if (buttons & CARD_CURSOR_ZONE_FORWARD_BUTTON) {
        cursor->cursorZone = cursor->cursorZone + 1;
    }
    if (buttons & CARD_CURSOR_INDEX_BACK_BUTTON) {
        cursor->cursorIndex--;
    }
    if (cursor->cursorZone < 0) {
        cursor->cursorZone = 0;
    }
    if (cursor->cursorZone >= CARD_CURSOR_ZONE_LIMIT) {
        cursor->cursorZone = CARD_CURSOR_HAND_ZONE;
    }
    if (cursor->cursorZone < CARD_CURSOR_HAND_ZONE) {
        CPCMSub(cursor, side, buttons, drawFlags, previousZone);
        slot = CursorTbl[cursor->cursorZone * CARD_CURSOR_ZONE_STRIDE + cursor->cursorIndex];
        cursor->position[0] = slot[0];
        cursor->position[1] = slot[1];
        cursor->position[2] = slot[2];
        return;
    }
    count = CardPlayHandCnt(&side->hand);
    cursor->highlightY = CARD_CURSOR_HAND_HIGHLIGHT;
    cursor->highlightScaleY = 1.0f;
    cursor->highlightX = CARD_CURSOR_HAND_HIGHLIGHT;
    cursor->highlightScaleX = 1.0f;
    if (count == 0) {
        cursor->cursorZone = CARD_CURSOR_EMPTY_HAND_ZONE;
        CPCMSub(cursor, side, buttons, drawFlags, previousZone);
        slot = CursorTbl[cursor->cursorZone * CARD_CURSOR_ZONE_STRIDE + cursor->cursorIndex];
        cursor->position[0] = slot[0];
        cursor->position[1] = slot[1];
        cursor->position[2] = slot[2];
        return;
    }
    if (cursor->cursorIndex < 0) {
        cursor->cursorIndex = 0;
    }
    if (cursor->cursorIndex >= count) {
        cursor->cursorIndex = count - 1;
    }
    cursor->position[0] = HandPos1P[0][0] - (HandPos1P[0][0] - HandPos1P[3][0]) / count * cursor->cursorIndex;
    cursor->position[1] = HandPos1P[0][1];
    cursor->position[2] = HandPos1P[0][2];
}

/*
 * Draws the card list window with the owned copies of each row. The header
 * (cardEnemyListHeaderFontCode) is printed at the window origin. For each row
 * below list->count (up to list->rows), the entry's card id selects the owned
 * copy count in work->ownedCount and the card name in work->definitions. A row
 * with no owned copy prints its marker and the placeholder cardUnownedRowText
 * instead of the name. With list->active bit 0 set (`andi $2,$2,0x1`), the owned
 * count follows the name, shown as CARD_LIST_COPY_CAP once it reaches
 * CARD_LIST_COPY_LIMIT. Rows are DECK_DISP_ROW_HEIGHT pixels apart; the footer
 * is cardWindowFooterFontCode.
 */
void CardListDisp(CardListWindow *list, CardGameWork *work)
{
    char nameText[32];
    char countText[16];
    s32 row = 0;
    s32 cardId;
    s32 copies;
    s32 i;
    char *name;
    CardDefinition *definition;
    CardDefinition *definitions = work->definitions;
    s16 *ownedCount = work->ownedCount;

    xglFontPrint(list->x, list->y, DECK_LIST_TEXT_COLOR, cardEnemyListHeaderFontCode);
    if (list->rows > 0) {
        for (; row < list->rows; row++) {
            if (row < list->count) {
                cardId = list->entries[row + list->top].cardId;
                if (work->ownedCount[cardId] > 0) {
                    /* Owned card: the name, then the copy count when list->active bit 0 is set. */
                    definition = &definitions[cardId];
                    name = CARD_DATA_BUFFER + definition->nameOffset;
                    if (row == list->cursor) {
                        xglFontPrint(list->x, list->y, DECK_LIST_TEXT_COLOR, cardSelectedRowFontCode);
                    } else {
                        xglFontPrint(list->x, list->y, DECK_LIST_TEXT_COLOR, cardOtherRowFontCode);
                    }
                    for (i = 0; i < list->textLength * CARD_TEXT_CHAR_BYTES; i++) {
                        nameText[i] = name[i];
                    }
                    nameText[i] = 0;
                    CardNameConvert(nameText);
                    xglFontPrint(0, 0, 0, cardRowPrefixFontCode);
                    xglFontPrint(list->x, list->y + row * DECK_DISP_ROW_HEIGHT,
                                 DECK_LIST_TEXT_COLOR, nameText);
                    if (list->active & 1) {
                        copies = ownedCount[cardId];
                        if (copies >= CARD_LIST_COPY_LIMIT) {
                            copies = CARD_LIST_COPY_CAP;
                        }
                        sprintf(countText, cardCopyCountFormat, copies);
                        if (row == list->cursor) {
                            xglFontPrint(list->x, list->y, DECK_LIST_TEXT_COLOR, cardSelectedRowFontCode);
                        } else {
                            xglFontPrint(list->x, list->y, DECK_LIST_TEXT_COLOR, cardCopyCountFontCode);
                        }
                        xglFontPrint(list->x + list->textLength * DECK_DISP_CHAR_WIDTH + DECK_DISP_COUNT_GAP,
                                     list->y + row * DECK_DISP_ROW_HEIGHT, DECK_LIST_TEXT_COLOR, countText);
                    }
                } else {
                    if (row == list->cursor) {
                        xglFontPrint(list->x, list->y, DECK_LIST_TEXT_COLOR, cardSelectedRowFontCode);
                    } else {
                        xglFontPrint(list->x, list->y, DECK_LIST_TEXT_COLOR, cardOtherRowFontCode);
                    }
                    xglFontPrint(list->x, list->y + row * DECK_DISP_ROW_HEIGHT,
                                 DECK_LIST_TEXT_COLOR, cardUnownedRowText);
                }
            }
        }
    }
    xglFontPrint(list->x, list->y, DECK_LIST_TEXT_COLOR, cardWindowFooterFontCode);
}

/*
 * Draws the deck list window: the header string, then the rows of DeckListStr
 * below list->count (up to list->rows), each preceded by the font code of the
 * selected or the other row style and printed DECK_LIST_ROW_HEIGHT pixels apart,
 * and the footer. A row is the font prefix "\x19\x03" followed by textLength
 * bytes of the name.
 */
void CardDeckListDisp(CardListWindow *list)
{
    char rowText[32];
    char *name;
    s32 row;
    s32 length;

    xglFontPrint(list->x, list->y, DECK_LIST_TEXT_COLOR, cardWindowHeaderFontCode);
    rowText[0] = 0x19;
    rowText[1] = 0x03;
    for (row = 0; row < list->rows; row++) {
        if (row < list->count) {
            name = DeckListStr[row];
            if (row == list->cursor) {
                xglFontPrint(list->x, list->y, DECK_LIST_TEXT_COLOR, cardSelectedRowFontCode);
            } else {
                xglFontPrint(list->x, list->y, DECK_LIST_TEXT_COLOR, cardOtherRowFontCode);
            }
            for (length = 0; length < list->textLength; length++) {
                rowText[CARD_ROW_PREFIX_BYTES + length] = name[length];
            }
            rowText[CARD_ROW_PREFIX_BYTES + length] = '\0';
            xglFontPrint(list->x, list->y + row * DECK_LIST_ROW_HEIGHT,
                         DECK_LIST_TEXT_COLOR, rowText);
        }
    }
    xglFontPrint(list->x, list->y, DECK_LIST_TEXT_COLOR, cardWindowFooterFontCode);
}

/*
 * Draws one list of strings in a window. The list header (a font escape) is
 * printed at the window origin first. Then, for each row below list->count
 * (up to list->rows), the row marker is printed at the origin (the selected
 * style on the row at `index`, the other style on the rest), followed by the row
 * text: `textLength` characters of the row's string after the two-byte
 * prefix 0x19 0x03, printed CARD_STR_LIST_ROW_HEIGHT pixels lower per row.
 * The footer is printed last.
 */
void CardStrListDisp(CardListWindow *list, char **strings, int index)
{
    char rowText[32];
    int row = 0;
    int rowY;
    char **rowPtr;
    char *text;
    int i;

    xglFontPrint(list->x, list->y, DECK_LIST_TEXT_COLOR, cardWindowHeaderFontCode);
    rowText[0] = 0x19;
    rowText[1] = 0x03;
    if (list->rows > 0) {
        rowPtr = strings;
        rowY = 0;
        for (; row < list->rows; row++) {
            if (row < list->count) {
                text = *rowPtr;
                if (row == index) {
                    xglFontPrint(list->x, list->y, DECK_LIST_TEXT_COLOR, cardSelectedRowFontCode);
                } else {
                    xglFontPrint(list->x, list->y, DECK_LIST_TEXT_COLOR, cardOtherRowFontCode);
                }
                for (i = 0; i < list->textLength; i++) {
                    rowText[i + CARD_ROW_PREFIX_BYTES] = text[i];
                }
                rowText[i + CARD_ROW_PREFIX_BYTES] = 0;
                xglFontPrint(list->x, list->y + rowY, DECK_LIST_TEXT_COLOR, rowText);
            }
            rowY += CARD_STR_LIST_ROW_HEIGHT;
            rowPtr++;
        }
    }
    xglFontPrint(list->x, list->y, DECK_LIST_TEXT_COLOR, cardWindowFooterFontCode);
}

/*
 * Draws a list of enemy deck names. Same shape as CardStrListDisp, but each row
 * is a card entry: entries[row] indexes DeckListStr (sixteen bytes per name),
 * the rows are DECK_LIST_ROW_HEIGHT pixels apart, and no prefix bytes are
 * written before the text.
 */
void CardEnemyListDisp(CardListWindow *list, s32 *entries, int index)
{
    char rowText[32];
    int row = 0;
    int rowY;
    s32 *rowPtr;
    char *text;
    int i;

    xglFontPrint(list->x, list->y, DECK_LIST_TEXT_COLOR, cardEnemyListHeaderFontCode);
    if (list->rows > 0) {
        rowPtr = entries;
        rowY = 0;
        for (; row < list->rows; row++) {
            if (row < list->count) {
                text = DeckListStr[*rowPtr];
                if (row == index) {
                    xglFontPrint(list->x, list->y, DECK_LIST_TEXT_COLOR, cardSelectedRowFontCode);
                } else {
                    xglFontPrint(list->x, list->y, DECK_LIST_TEXT_COLOR, cardOtherRowFontCode);
                }
                for (i = 0; i < list->textLength; i++) {
                    rowText[i] = text[i];
                }
                rowText[i] = 0;
                xglFontPrint(list->x, list->y + rowY, DECK_LIST_TEXT_COLOR, rowText);
            }
            rowY += DECK_LIST_ROW_HEIGHT;
            rowPtr++;
        }
    }
    xglFontPrint(list->x, list->y, DECK_LIST_TEXT_COLOR, cardWindowFooterFontCode);
}

/*
 * The power of a slot: that of the card stacked on its base card, except for the
 * equipment that adds no attack of its own (Eyeglasses, Battle Suit, Shield:
 * attack 0 in card.dat), which leaves the base card's.
 */
s32 CardPlayCntPower(CardGameWork *work, CardLayerStack *slot)
{
    CardDefinition *def;
    s16 topCardId;
    s32 power;

    power = 0;
    if (!(slot->flags & CARD_STACK_FLAG_NO_DAMAGE)) {
        def = &work->definitions[slot->layers[0].cardId];
        topCardId = slot->layers[1].cardId;
        if (topCardId >= 0 && topCardId != CARD_ID_EYEGLASSES && topCardId != CARD_ID_BATTLE_SUIT && topCardId != CARD_ID_SHIELD) {
            def = &work->definitions[topCardId];
        }
        power = CGPCalcPowerPlus(work, slot, def->attackPower);
    }
    return power;
}

/*
 * Draws the battle slots of one side: each live slot moves along its step or rests
 * at BattlePos1P, is drawn as a card with its frame and stacked card; a settled
 * card also gets its info plate: attack type icon, effective power, boost pips,
 * equipment kind, life digits, life bar and flag markers. An empty slot is reset
 * to its resting position.
 */
void CardPlayDispBattle(s32 side, CardGameWork *work, CardPlaySide *ownSide, Matrix4 matrix)
{
    Matrix4 place;
    float translation[4];
    float scale[4];
    float stackScale[4];
    Matrix4 iconPlace;
    s32 i;
    s32 j;
    s32 slot;
    s32 power;
    u32 model;
    float *infoPos;
    CardDefinition *def;

    translation[3] = 1.0f;
    scale[1] = CARD_BATTLE_SCALE;
    scale[3] = 1.0f;
    scale[2] = 1.0f;
    stackScale[1] = CARD_BATTLE_STACK_SCALE;
    stackScale[3] = 1.0f;
    scale[0] = CARD_BATTLE_SCALE;
    stackScale[0] = CARD_BATTLE_STACK_SCALE;
    stackScale[2] = 1.0f;

    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        if (ownSide->battle[i].battle.layers[0].cardId >= 0) {
            if (ownSide->battle[i].battle.moveFrames != 0) {
                ownSide->battle[i].battle.moveFrames--;
                if (ownSide->battle[i].battle.moveFrames == 0) {
                    xglSoundEffectNormalID(CARD_BATTLE_ARRIVE_SOUND, 0);
                }
                ownSide->battle[i].battle.position[0] += ownSide->battle[i].battle.step[0];
                ownSide->battle[i].battle.position[1] += ownSide->battle[i].battle.step[1];
                ownSide->battle[i].battle.position[2] += ownSide->battle[i].battle.step[2];
            } else {
                ownSide->battle[i].battle.position[0] = BattlePos1P[i][0];
                ownSide->battle[i].battle.position[1] = BattlePos1P[i][1];
                ownSide->battle[i].battle.position[2] = BattlePos1P[i][2];
            }
            translation[0] = ownSide->battle[i].battle.position[0];
            translation[1] = ownSide->battle[i].battle.position[1];
            translation[2] = ownSide->battle[i].battle.position[2];
            scale[0] = scale[1] = CARD_BATTLE_SCALE;
            scale[3] = 1.0f;
            scale[2] = 1.0f;
            xglMatrixTrans(place, matrix, translation);
            xglMatrixScale(place, place, scale);
            CardDispCard(&place, ownSide->battle[i].battle.layers[0].cardId & CARD_CURSOR_CARD_ID_MASK, 1);
            nmlModelSetPlace(&place);
            nmlModelSetTexture(CARD_FRAME_TEXTURE);
            nmlModelEntryCard(CARD_BATTLE_FRAME_MODEL);
            /* The stacked card, drawn offset from the base card. */
            if (ownSide->battle[i].battle.layers[1].cardId >= 0) {
                translation[0] = CardStackPos[0];
                translation[1] = CardStackPos[1];
                translation[2] = CardStackPos[2];
                xglMatrixTrans(place, place, translation);
                xglMatrixScale(place, place, stackScale);
                CardDispCard(&place, ownSide->battle[i].battle.layers[1].cardId & CARD_CURSOR_CARD_ID_MASK, 1);
                nmlModelSetPlace(&place);
                nmlModelSetTexture(CARD_FRAME_TEXTURE);
                nmlModelEntryCard(CARD_FRAME_MODEL);
            }
            /* Settled card: its info plate and the digits, bars and markers on it. */
            if (ownSide->battle[i].battle.moveFrames == 0) {
                infoPos = CPSInfoPos[i];
                translation[0] = infoPos[0];
                translation[1] = infoPos[1];
                translation[3] = 1.0f;
                translation[2] = 0.0f;
                xglMatrixTrans(place, matrix, translation);
                nmlModelSetPlace(&place);
                if (side == 0) {
                    nmlModelSetLight(&Player1Color, &asDir);
                } else {
                    nmlModelSetLight(&Player2Color, &asDir);
                }
                nmlModelSetTexture(CARD_BATTLE_MARKER_TEXTURE);
                nmlModelEntryCard(CARD_BATTLE_PLATE_FRAME_MODEL);
                translation[0] = CARD_BATTLE_PLATE_X;
                translation[1] = CARD_BATTLE_PLATE_Y;
                translation[2] = CARD_BATTLE_ICON_Z;
                xglMatrixTrans(place, place, translation);
                nmlModelSetPlace(&place);
                nmlModelSetTexture(CARD_BATTLE_MARKER_TEXTURE);
                nmlModelEntryCard(CARD_BATTLE_PLATE_MODEL);
                QuadwordCopyMatrix(place, matrix);
                /* No code survives for this test or its unused assignment. The branch still divides the block
                 * at allocation time, which leaves the slot offset in $16 and the chain computing it in $3
                 * (`sll $16,$3,0x2`), so the offset stays live across the xglMatrixTrans call. */
                if (ownSide->battle[i].battle.flags != 0) {
                    power = ownSide->battle[i].battle.moveFrames;
                }
                scale[3] = 1.0f;
                scale[2] = 1.0f;
                scale[1] = 1.0f;
                scale[0] = 1.0f;
                translation[0] = infoPos[0];
                translation[1] = infoPos[1];
                translation[2] = CARD_BATTLE_INFO_Z;
                xglMatrixTrans(place, place, translation);
                translation[0] = CARD_BATTLE_ATTACK_X;
                translation[1] = CARD_BATTLE_ATTACK_Y;
                translation[2] = CARD_BATTLE_ICON_Z;
                scale[0] = scale[1] = CARD_BATTLE_ATTACK_SCALE;
                model = 0;
                switch (CardChkAttackType(work->definitions, &ownSide->battle[i].battle)) {
                case CARD_ATTACK_HOMING:
                    model = 1;
                case CARD_ATTACK_SPREAD:
                    model++;
                case CARD_ATTACK_BALLISTIC:
                    model++;
                case CARD_ATTACK_HAND:
                    break;
                default:
                    model = -1;
                    break;
                }
                if ((s32)model >= 0) {
                    model = CARD_BATTLE_ATTACK_MODEL + (model * CARD_MODEL_STRIDE);
                    xglMatrixTrans(iconPlace, place, translation);
                    xglMatrixScale(iconPlace, iconPlace, scale);
                    nmlModelSetPlace(&iconPlace);
                    nmlModelSetTexture(CARD_BATTLE_ICON_TEXTURE);
                    nmlModelEntryCard(model);
                }
                power = CardPlayCntPower(work, &ownSide->battle[i].battle);
                if (ownSide->battle[i].battle.layers[0].cardId == CARD_BATTLE_REFLECT_CARD) {
                    for (slot = 0; slot < CARD_BOARD_SLOTS; slot++) {
                        if (ownSide->battle[slot].battle.layers[0].cardId >= 0
                            && (work->definitions[ownSide->battle[slot].battle.layers[0].cardId].flags & CARD_BATTLE_REFLECT_FLAG) != 0) {
                            power++;
                        }
                    }
                }
                if (power > CARD_BATTLE_POWER_MAX) {
                    power = CARD_BATTLE_POWER_MAX;
                }
                scale[0] = scale[1] = CARD_BATTLE_POWER_SCALE;
                model = power / CARD_DIGIT_BASE;
                if (model != 0) {
                    model = CARD_DIGIT_MODEL + (model * CARD_MODEL_STRIDE);
                    translation[0] = CARD_BATTLE_TENS_X;
                    translation[1] = CARD_BATTLE_POWER_Y;
                    xglMatrixTrans(iconPlace, place, translation);
                    xglMatrixScale(iconPlace, iconPlace, scale);
                    nmlModelSetPlace(&iconPlace);
                    nmlModelSetTexture(CARD_BATTLE_DIGIT_TEXTURE);
                    nmlModelEntryCard(model);
                }
                model = CARD_DIGIT_MODEL + ((power % CARD_DIGIT_BASE) * CARD_MODEL_STRIDE);
                translation[0] = CARD_BATTLE_UNITS_X;
                translation[1] = CARD_BATTLE_POWER_Y;
                xglMatrixTrans(iconPlace, place, translation);
                xglMatrixScale(iconPlace, iconPlace, scale);
                nmlModelSetPlace(&iconPlace);
                nmlModelSetTexture(CARD_BATTLE_DIGIT_TEXTURE);
                nmlModelEntryCard(model);
                translation[0] = CARD_BATTLE_PIP_X;
                translation[1] = CARD_BATTLE_PIP_Y;
                for (j = 0; j < ownSide->battle[i].battle.powerLimit; j++) {
                    model = CARD_BATTLE_PIP_OFF_MODEL;
                    if (!(ownSide->battle[i].battle.powerLimit - ownSide->battle[i].battle.power < j)) {
                        model = CARD_BATTLE_PIP_ON_MODEL;
                    }
                    xglMatrixTrans(iconPlace, place, translation);
                    nmlModelSetPlace(&iconPlace);
                    nmlModelSetTexture(CARD_BATTLE_ICON_TEXTURE);
                    nmlModelEntryCard(model);
                    translation[0] += CARD_BATTLE_PIP_STEP;
                }
                translation[0] = CARD_BATTLE_EQUIP_X;
                translation[1] = CARD_BATTLE_EQUIP_Y;
                scale[0] = scale[1] = CARD_BATTLE_LIFE_SCALE;
                model = 0;
                def = &work->definitions[ownSide->battle[i].battle.layers[0].cardId];
                switch (def->flags) {
                case CARD_ATTR_WEAPON_MECH:
                case CARD_ATTR_WEAPON_HUMAN:
                    model = 1;
                case CARD_ATTR_UNKNOWN:
                    model++;
                case CARD_ATTR_GNOSIS:
                    model++;
                case CARD_ATTR_MECH:
                    model++;
                case CARD_ATTR_REALIAN:
                    model++;
                case CARD_ATTR_HUMAN:
                    break;
                }
                model = CARD_ICON_MODEL_BASE + (model * CARD_MODEL_STRIDE);
                xglMatrixTrans(iconPlace, place, translation);
                xglMatrixScale(iconPlace, iconPlace, scale);
                nmlModelSetPlace(&iconPlace);
                nmlModelSetTexture(CARD_BATTLE_DIGIT_TEXTURE);
                nmlModelEntryCard(model);
                if (ownSide->battle[i].battle.layers[0].shownLife < 0) {
                    ownSide->battle[i].battle.layers[0].shownLife = 0;
                }
                /* Neither test below leaves code. They lengthen the life of the reloaded shownLife byte, which
                 * ends up in $4 (`lbu $4,24($16)`) rather than the next argument register. */
                if (ownSide->battle[i].battle.flags != 0) {
                    power = ownSide->battle[i].battle.moveFrames;
                }
                if (ownSide->battle[i].battle.moveFrames != 0) {
                    power = ownSide->battle[i].battle.moveFrames;
                }
                scale[0] = scale[1] = CARD_BATTLE_LIFE_SCALE;
                model = ownSide->battle[i].battle.layers[0].shownLife / CARD_DIGIT_BASE;
                if (model != 0) {
                    model = CARD_DIGIT_MODEL + (model * CARD_MODEL_STRIDE);
                    translation[0] = CARD_BATTLE_TENS_X;
                    translation[1] = CARD_BATTLE_LIFE_Y;
                    xglMatrixTrans(iconPlace, place, translation);
                    xglMatrixScale(iconPlace, iconPlace, scale);
                    nmlModelSetPlace(&iconPlace);
                    nmlModelSetTexture(CARD_BATTLE_DIGIT_TEXTURE);
                    nmlModelEntryCard(model);
                }
                model = CARD_DIGIT_MODEL + ((ownSide->battle[i].battle.layers[0].shownLife % CARD_DIGIT_BASE) * CARD_MODEL_STRIDE);
                translation[0] = CARD_BATTLE_UNITS_X;
                translation[1] = CARD_BATTLE_LIFE_Y;
                xglMatrixTrans(iconPlace, place, translation);
                xglMatrixScale(iconPlace, iconPlace, scale);
                nmlModelSetPlace(&iconPlace);
                nmlModelSetTexture(CARD_BATTLE_DIGIT_TEXTURE);
                nmlModelEntryCard(model);
                translation[0] = CARD_BATTLE_BAR_X;
                translation[1] = CARD_BATTLE_BAR_Y;
                translation[2] = CARD_BATTLE_BAR_Z;
                scale[1] = 1.0f;
                scale[0] = (float)ownSide->battle[i].battle.layers[0].shownLife / CARD_BATTLE_BAR_FULL;
                xglMatrixTrans(iconPlace, place, translation);
                xglMatrixScale(iconPlace, iconPlace, scale);
                nmlModelSetPlace(&iconPlace);
                nmlModelSetTexture(CARD_BATTLE_BAR_TEXTURE);
                nmlModelEntryCard(CARD_BATTLE_BAR_MODEL);
                if ((ownSide->battle[i].battle.previousFlags & CARD_STACK_FLAG_END) != 0) {
                    translation[0] = BattlePos1P[i][0];
                    translation[1] = BattlePos1P[i][1];
                    translation[2] = CARD_BATTLE_INFO_Z;
                    xglMatrixTrans(iconPlace, matrix, translation);
                    nmlModelSetPlace(&iconPlace);
                    nmlModelSetTexture(CARD_BATTLE_MARKER_TEXTURE);
                    nmlModelEntryCard(CARD_BATTLE_MARKER_MODEL_BIT0);
                }
                if ((ownSide->battle[i].battle.previousFlags & CARD_STACK_FLAG_FROZEN) != 0) {
                    translation[0] = BattlePos1P[i][0];
                    translation[1] = BattlePos1P[i][1];
                    translation[2] = CARD_BATTLE_INFO_Z;
                    xglMatrixTrans(iconPlace, matrix, translation);
                    nmlModelSetPlace(&iconPlace);
                    nmlModelSetTexture(CARD_BATTLE_MARKER_TEXTURE);
                    nmlModelEntryCard(CARD_BATTLE_MARKER_MODEL_BIT1);
                }
            }
        } else {
            ownSide->battle[i].battle.flags = 0;
            ownSide->battle[i].battle.moveFrames = 0;
            ownSide->battle[i].battle.position[0] = BattlePos1P[i][0];
            ownSide->battle[i].battle.position[1] = BattlePos1P[i][1];
            ownSide->battle[i].battle.position[2] = BattlePos1P[i][2];
            ownSide->battle[i].battle.step[0] = 0.0f;
            ownSide->battle[i].battle.step[1] = 0.0f;
            ownSide->battle[i].battle.step[2] = 0.0f;
        }
    }
}

/*
 * Draws the hand of one side: the hand size as tens and units digit models,
 * then every hand card along its handMove record. A card in motion advances by its step and
 * flips by kind (4: growing face-down flip, 3: turning over, 2: face up, other: shrinking);
 * a card that has arrived starts the next stage of its kind (4 -> 3 -> 2 -> 1 -> 0, kind 2
 * sends it to its hand slot) or rests in its slot. On side != 0 a settled card (kind 0)
 * shows the card id from the other end of the hand.
 */
void CardPlayDispHand(s32 side, s32 faceUp, CardPlaySide *ownSide, Matrix4 matrix)
{
    Matrix4 place;
    Matrix4 cardMatrix;
    float translation[4];
    float scale[4];
    float flipScale[4];
    float angle;
    float spacing;
    s32 count;
    s32 i;
    s16 *cards;
    s32 cardIdMask;
    u32 model;

    count = CardPlayHandCnt(&ownSide->hand);
    i = count / CARD_PILE_STACK_SIZE;

    translation[3] = 1.0f;
    scale[1] = CARD_PILE_COUNT_SCALE;
    scale[2] = 1.0f;
    scale[3] = 1.0f;
    scale[0] = CARD_PILE_COUNT_SCALE;
    translation[2] = CARD_PILE_COUNT_Z;

    if (i != 0) {
        model = CARD_PILE_COUNT_MODEL + (i * CARD_MODEL_STRIDE);
        translation[0] = CARD_HAND_TENS_X;
        translation[1] = CARD_PILE_COUNT_Y;
        xglMatrixTrans(place, matrix, translation);
        xglMatrixScale(place, place, scale);
        nmlModelSetPlace(&place);
        nmlModelSetTexture(CARD_PILE_COUNT_TEXTURE);
        nmlModelEntryCard(model);
    }

    model = CARD_PILE_COUNT_MODEL + ((count % CARD_PILE_STACK_SIZE) * CARD_MODEL_STRIDE);
    translation[0] = CARD_HAND_UNITS_X;
    translation[1] = CARD_PILE_COUNT_Y;
    xglMatrixTrans(place, matrix, translation);
    xglMatrixScale(place, place, scale);
    nmlModelSetPlace(&place);
    nmlModelSetTexture(CARD_PILE_COUNT_TEXTURE);
    nmlModelEntryCard(model);

    if (count != 0) {
        angle = 0.0f;
        spacing = HandPos1P[0][0] - HandPos1P[3][0];
        if (count >= 2) {
            spacing = spacing / ((float)count - 1.0f);
        }

        translation[3] = 1.0f;
        scale[1] = CARD_PILE_CARD_SCALE;
        scale[3] = 1.0f;
        scale[2] = 1.0f;
        flipScale[3] = 1.0f;
        flipScale[2] = 1.0f;
        flipScale[1] = 1.0f;
        flipScale[0] = 1.0f;
        scale[0] = CARD_PILE_CARD_SCALE;

        cardIdMask = ~CARD_HAND_FLAG_BITS; /* `addiu $7,$0,-0xF01` */
        /* Advance each hand card's move record, then draw it. */
        for (i = 0; i < count; i++) {
            /* The hand card base is formed at the loop top (`addiu $3,$7,8`). */
            cards = ownSide->hand.cards;
            if (ownSide->handMove[i].moveTimer != 0) {
                ownSide->handMove[i].moveTimer--;
                ownSide->handMove[i].position[0] += ownSide->handMove[i].step[0];
                ownSide->handMove[i].position[1] += ownSide->handMove[i].step[1];
                ownSide->handMove[i].position[2] += ownSide->handMove[i].step[2];
                switch (ownSide->handMove[i].kind) {
                case CARD_MOVE_KIND_GROW:
                    angle = CARD_HALF_TURN;
                    flipScale[0] = flipScale[1] = (float)(CARD_SLIDE_FRAMES - ownSide->handMove[i].moveTimer) * CARD_HAND_FULL_SCALE / CARD_SLIDE_FRAMES;
                    break;
                case CARD_MOVE_KIND_TURN:
                    flipScale[1] = CARD_HAND_FULL_SCALE;
                    flipScale[0] = CARD_HAND_FULL_SCALE;
                    if (faceUp) {
                        angle = CARD_HALF_TURN;
                    } else {
                        angle = CARD_RADIANS((float)ownSide->handMove[i].moveTimer * CARD_HAND_TURN_STEP);
                    }
                    break;
                case CARD_MOVE_KIND_WAIT:
                    if (faceUp) {
                        angle = CARD_HALF_TURN;
                    }
                    flipScale[1] = CARD_HAND_FULL_SCALE;
                    flipScale[0] = CARD_HAND_FULL_SCALE;
                    break;
                default:
                    if (faceUp) {
                        angle = CARD_HALF_TURN;
                    }
                    flipScale[0] = flipScale[1] = (float)ownSide->handMove[i].moveTimer * CARD_HAND_FULL_SCALE / CARD_SLIDE_FRAMES;
                    break;
                }
            } else {
                switch (ownSide->handMove[i].kind) {
                case CARD_MOVE_KIND_GROW:
                    ownSide->handMove[i].kind--;
                    ownSide->handMove[i].moveTimer = CARD_HAND_TURN_FRAMES;
                    angle = CARD_HALF_TURN;
                    flipScale[1] = CARD_HAND_FULL_SCALE;
                    flipScale[0] = CARD_HAND_FULL_SCALE;
                    ownSide->handMove[i].step[0] = 0.0f;
                    ownSide->handMove[i].step[1] = 0.0f;
                    ownSide->handMove[i].step[2] = 0.0f;
                    break;
                case CARD_MOVE_KIND_TURN:
                    if (faceUp) {
                        angle = CARD_HALF_TURN;
                    }
                    ownSide->handMove[i].kind--;
                    ownSide->handMove[i].moveTimer = CARD_HAND_WAIT_FRAMES;
                    flipScale[1] = CARD_HAND_FULL_SCALE;
                    flipScale[0] = CARD_HAND_FULL_SCALE;
                    break;
                case CARD_MOVE_KIND_WAIT:
                    if (faceUp) {
                        angle = CARD_HALF_TURN;
                    }
                    ownSide->handMove[i].kind--;
                    ownSide->handMove[i].moveTimer = CARD_SLIDE_FRAMES;
                    ownSide->handMove[i].step[0] = (HandPos1P[0][0] - spacing * (float)i - ownSide->handMove[i].position[0]) / CARD_SLIDE_FRAMES;
                    ownSide->handMove[i].step[1] = (HandPos1P[0][1] - ownSide->handMove[i].position[1]) / CARD_SLIDE_FRAMES;
                    ownSide->handMove[i].step[2] = (HandPos1P[0][2] + (float)i * CARD_HAND_SETTLE_DEPTH - ownSide->handMove[i].position[2]) / CARD_SLIDE_FRAMES;
                    flipScale[1] = CARD_HAND_FULL_SCALE;
                    flipScale[0] = CARD_HAND_FULL_SCALE;
                    break;
                case CARD_MOVE_KIND_SLIDE:
                    ownSide->handMove[i].kind = 0;
                    xglSoundEffectNormalID(CARD_MOVE_ARRIVE_SOUND, 0);
                default:
                    if (faceUp) {
                        angle = CARD_HALF_TURN;
                    }
                    ownSide->handMove[i].position[0] = HandPos1P[0][0] - spacing * (float)i;
                    ownSide->handMove[i].position[1] = HandPos1P[0][1];
                    ownSide->handMove[i].position[2] = HandPos1P[0][2] + (float)i * CARD_HAND_SETTLE_DEPTH;
                    break;
                }
            }

            translation[0] = ownSide->handMove[i].position[0];
            translation[1] = ownSide->handMove[i].position[1];
            translation[2] = ownSide->handMove[i].position[2];
            xglMatrixTrans(place, matrix, translation);
            xglMatrixRotY(place, place, angle);
            xglMatrixScale(place, place, flipScale);
            xglMatrixScale(cardMatrix, place, scale);

            /* Side 0 shows its cards; for other sides a settled card shows the id from the other end. */
            if (side == 0) {
                CardDispCard(&cardMatrix, cards[i] & cardIdMask, 1);
            } else if (ownSide->handMove[i].kind != 0) { /* a card still moving shows its own id */
                CardDispCard(&cardMatrix, cards[i] & cardIdMask, 1);
            } else {
                CardDispCard(&cardMatrix, cards[count - 1 - i] & cardIdMask, 1);
            }
            nmlModelSetPlace(&cardMatrix);
            nmlModelSetTexture(CARD_FRAME_TEXTURE);
            nmlModelEntryCard(CARD_FRAME_MODEL);
        }
    }
}

/*
 * Draws the junk pile of one side: the pile count as digit models, the top
 * card still resting (last slot with moveTimer == 0) at JunkPos1P[0], then every card in
 * motion along its junkMove record; a card that has landed is placed on the pile when it
 * is the last one.
 * The first argument (the side number) is not read.
 */
void CardPlayDispJunk(s32 side, CardPlaySide *ownSide, Matrix4 matrix)
{
    Matrix4 place;
    float translation[4];
    float scale[4];
    s32 count;
    s32 top;
    s32 i;
    u32 model;

    count = CardPlayJunkCnt(ownSide);
    scale[1] = CARD_PILE_COUNT_SCALE;
    scale[0] = CARD_PILE_COUNT_SCALE;
    scale[2] = 1.0f;
    scale[3] = 1.0f;
    i = count / CARD_PILE_STACK_SIZE; /* tens digit, held in the loop index i: a separate local gets a different register than the original's for the `sll 10` of the model id */
    translation[2] = CARD_PILE_COUNT_Z;
    translation[3] = 1.0f;
    if (i != 0) {
        model = CARD_PILE_COUNT_MODEL + (i * CARD_MODEL_STRIDE);
        translation[0] = CARD_JUNK_COUNT_TENS_X;
        translation[1] = CARD_PILE_COUNT_Y;
        xglMatrixTrans(place, matrix, translation);
        xglMatrixScale(place, place, scale);
        nmlModelSetPlace(&place);
        nmlModelSetTexture(CARD_PILE_COUNT_TEXTURE);
        nmlModelEntryCard(model);
    }

    model = CARD_PILE_COUNT_MODEL + ((count % CARD_PILE_STACK_SIZE) * CARD_MODEL_STRIDE);
    translation[0] = CARD_JUNK_COUNT_UNITS_X;
    translation[1] = CARD_PILE_COUNT_Y;
    xglMatrixTrans(place, matrix, translation);
    xglMatrixScale(place, place, scale);
    nmlModelSetPlace(&place);
    nmlModelSetTexture(CARD_PILE_COUNT_TEXTURE);
    nmlModelEntryCard(model);

    if (count == 0) {
        return;
    }
    /* Find the resting top card, then draw it on the pile. */
    top = -1;
    translation[3] = 1.0f;
    scale[1] = CARD_PILE_CARD_SCALE;
    scale[0] = CARD_PILE_CARD_SCALE;
    scale[3] = 1.0f;
    scale[2] = 1.0f;
    for (i = 0; i < count; i++) {
        if (ownSide->junkMove[i].moveTimer == 0) {
            top = i;
        }
    }
    if (top >= 0) {
        translation[0] = JunkPos1P[0][0];
        translation[1] = JunkPos1P[0][1];
        translation[2] = JunkPos1P[0][2] - CARD_JUNK_TOP_DROP;
        xglMatrixTrans(place, matrix, translation);
        xglMatrixScale(place, place, scale);
        CardDispCard(&place, ownSide->junk[top] & CARD_CURSOR_CARD_ID_MASK, 1);
    }

    for (i = 0; i < count; i++) {
        if (ownSide->junkMove[i].moveTimer != 0) {
            ownSide->junkMove[i].moveTimer--;
            if (ownSide->junkMove[i].moveTimer == 0) {
                xglSoundEffectNormalID(CARD_MOVE_ARRIVE_SOUND, 0);
            }
            ownSide->junkMove[i].position[0] += ownSide->junkMove[i].step[0];
            ownSide->junkMove[i].position[1] += ownSide->junkMove[i].step[1];
            ownSide->junkMove[i].position[2] += ownSide->junkMove[i].step[2];
            translation[0] = ownSide->junkMove[i].position[0];
            translation[1] = ownSide->junkMove[i].position[1];
            translation[2] = ownSide->junkMove[i].position[2];
            xglMatrixTrans(place, matrix, translation);
            xglMatrixScale(place, place, scale);
            CardDispCard(&place, ownSide->junk[i] & CARD_CURSOR_CARD_ID_MASK, 1);
        } else if (i == count - 1) {
            translation[0] = ownSide->junkMove[i].position[0] = JunkPos1P[0][0];
            translation[1] = ownSide->junkMove[i].position[1] = JunkPos1P[0][1];
            translation[2] = ownSide->junkMove[i].position[2] = JunkPos1P[0][2];
            xglMatrixTrans(place, matrix, translation);
            xglMatrixScale(place, place, scale);
            CardDispCard(&place, ownSide->junk[i] & CARD_CURSOR_CARD_ID_MASK, 1);
        }
    }
}

/*
 * Draws the four disposal slots of one side. Each slot's cards move from
 * DisposePos1P toward their resting place, the top card is drawn with its frame, a stacked card
 * sits at CardStackPos, the flag markers follow, and the slot value is shown as two digits.
 * The first argument (the side number) is not read.
 */
void CardPlayDispDispose(s32 side, CardPlaySide *ownSide, Matrix4 matrix)
{
    Matrix4 place;
    float translation[4];
    float scale[4];
    float stackScale[4];
    s32 slot;
    s32 tens;
    u32 model;

    translation[3] = 1.0f;
    scale[1] = CARD_DISPOSE_SCALE;
    scale[3] = 1.0f;
    scale[2] = 1.0f;
    stackScale[1] = CARD_DISPOSE_STACK_SCALE;
    stackScale[3] = 1.0f;
    scale[0] = CARD_DISPOSE_SCALE;
    stackScale[0] = CARD_DISPOSE_STACK_SCALE;
    stackScale[2] = 1.0f;

    for (slot = 0; slot < CARD_BOARD_SLOTS; slot++) {
        if (ownSide->disposal[slot].battle.layers[0].cardId >= 0) {
            if (ownSide->disposal[slot].battle.moveFrames != 0) {
                ownSide->disposal[slot].battle.moveFrames--;
                if (ownSide->disposal[slot].battle.moveFrames == 0) {
                    xglSoundEffectNormalID(CARD_MOVE_ARRIVE_SOUND, 0);
                }
                ownSide->disposal[slot].battle.position[0] += ownSide->disposal[slot].battle.step[0];
                ownSide->disposal[slot].battle.position[1] += ownSide->disposal[slot].battle.step[1];
                ownSide->disposal[slot].battle.position[2] += ownSide->disposal[slot].battle.step[2];
            } else {
                ownSide->disposal[slot].battle.position[0] = DisposePos1P[slot][0];
                ownSide->disposal[slot].battle.position[1] = DisposePos1P[slot][1];
                ownSide->disposal[slot].battle.position[2] = DisposePos1P[slot][2];
            }
            translation[0] = ownSide->disposal[slot].battle.position[0];
            translation[1] = ownSide->disposal[slot].battle.position[1];
            translation[2] = ownSide->disposal[slot].battle.position[2];
            xglMatrixTrans(place, matrix, translation);
            xglMatrixScale(place, place, scale);
            CardDispCard(&place, ownSide->disposal[slot].battle.layers[0].cardId & CARD_CURSOR_CARD_ID_MASK, 1);
            nmlModelSetPlace(&place);
            nmlModelSetTexture(CARD_FRAME_TEXTURE);
            nmlModelEntryCard(CARD_FRAME_MODEL);
            if (ownSide->disposal[slot].battle.layers[1].cardId >= 0) {
                translation[0] = CardStackPos[0];
                translation[1] = CardStackPos[1];
                translation[2] = CardStackPos[2];
                xglMatrixTrans(place, place, translation);
                xglMatrixScale(place, place, stackScale);
                CardDispCard(&place, ownSide->disposal[slot].battle.layers[1].cardId & CARD_CURSOR_CARD_ID_MASK, 1);
                nmlModelSetPlace(&place);
                nmlModelSetTexture(CARD_FRAME_TEXTURE);
                nmlModelEntryCard(CARD_FRAME_MODEL);
            }
            /* Settled slot: the markers, then the slot value as two digits. */
            if (ownSide->disposal[slot].battle.moveFrames == 0) {
                if ((ownSide->disposal[slot].battle.flags & CARD_STACK_FLAG_END) != 0) {
                    translation[0] = DisposePos1P[slot][0];
                    translation[1] = DisposePos1P[slot][1];
                    translation[2] = CARD_PILE_COUNT_Z;
                    xglMatrixTrans(place, matrix, translation);
                    nmlModelSetPlace(&place);
                    nmlModelSetTexture(CARD_DISPOSE_MARKER_TEXTURE);
                    nmlModelEntryCard(CARD_DISPOSE_MARKER_MODEL_BIT0);
                }
                if ((ownSide->disposal[slot].battle.flags & CARD_STACK_FLAG_FROZEN) != 0) {
                    translation[0] = DisposePos1P[slot][0];
                    translation[1] = DisposePos1P[slot][1];
                    translation[2] = CARD_PILE_COUNT_Z;
                    xglMatrixTrans(place, matrix, translation);
                    nmlModelSetPlace(&place);
                    nmlModelSetTexture(CARD_DISPOSE_MARKER_TEXTURE);
                    nmlModelEntryCard(CARD_DISPOSE_MARKER_MODEL_BIT1);
                }
                if (ownSide->disposal[slot].battle.layers[0].shownLife < 0) {
                    ownSide->disposal[slot].battle.layers[0].shownLife = 0;
                }
                /* The compiled code reads shownLife as a signed byte here (lb). The field stays u8
                 * because CGPHPCheckSub fills it from the unsigned life byte, so the signed view is a cast. */
                tens = ownSide->disposal[slot].battle.layers[0].shownLife / CARD_DIGIT_BASE;
                if (tens != 0) {
                    model = CARD_PILE_COUNT_MODEL + (tens * CARD_MODEL_STRIDE);
                    translation[0] = DisposeHP10pos[slot][0];
                    translation[1] = DisposeHP10pos[slot][1];
                    translation[2] = CARD_DISPOSE_DIGIT_Z;
                    xglMatrixTrans(place, matrix, translation);
                    nmlModelSetPlace(&place);
                    nmlModelSetTexture(CARD_PILE_COUNT_TEXTURE);
                    nmlModelEntryCard(model);
                }
                model = CARD_PILE_COUNT_MODEL + ((ownSide->disposal[slot].battle.layers[0].shownLife % CARD_DIGIT_BASE) * CARD_MODEL_STRIDE);
                translation[0] = DisposeHP01pos[slot][0];
                translation[1] = DisposeHP01pos[slot][1];
                translation[2] = CARD_DISPOSE_DIGIT_Z;
                xglMatrixTrans(place, matrix, translation);
                nmlModelSetPlace(&place);
                nmlModelSetTexture(CARD_PILE_COUNT_TEXTURE);
                nmlModelEntryCard(model);
            }
        } else {
            ownSide->disposal[slot].battle.flags = 0;
            ownSide->disposal[slot].battle.moveFrames = 0;
            ownSide->disposal[slot].battle.position[0] = DisposePos1P[slot][0];
            ownSide->disposal[slot].battle.position[1] = DisposePos1P[slot][1];
            ownSide->disposal[slot].battle.position[2] = DisposePos1P[slot][2];
            ownSide->disposal[slot].battle.step[0] = 0.0f;
            ownSide->disposal[slot].battle.step[1] = 0.0f;
            ownSide->disposal[slot].battle.step[2] = 0.0f;
        }
    }
}

/*
 * Draws the operation cards of one side: each live slot (operations[i] >= 0)
 * moves along its operationMove record, then is drawn as a card with its frame; a settled card
 * with operationFlags bit 0 also gets a marker above its slot.
 * The first argument (the side number) is not read.
 */
void CardPlayDispOperation(s32 side, CardPlaySide *ownSide, Matrix4 matrix)
{
    Matrix4 place;
    float translation[4];
    float scale[4];
    int i;

    if (CardPlayOperationCnt(ownSide) == 0) {
        return;
    }
    translation[3] = 1.0f;
    scale[1] = CARD_OPERATION_SCALE;
    scale[3] = 1.0f;
    scale[0] = CARD_OPERATION_SCALE;
    scale[2] = 1.0f;
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        if (ownSide->operations[i] < 0) {
            continue;
        }
        if (ownSide->operationMove[i].moveTimer != 0) {
            ownSide->operationMove[i].moveTimer--;
            if (ownSide->operationMove[i].moveTimer == 0) {
                xglSoundEffectNormalID(CARD_MOVE_ARRIVE_SOUND, 0);
            }
            ownSide->operationMove[i].position[0] += ownSide->operationMove[i].step[0];
            ownSide->operationMove[i].position[1] += ownSide->operationMove[i].step[1];
            ownSide->operationMove[i].position[2] += ownSide->operationMove[i].step[2];
        } else {
            ownSide->operationMove[i].position[0] = OperationPos1P[i][0];
            ownSide->operationMove[i].position[1] = OperationPos1P[i][1];
            ownSide->operationMove[i].position[2] = OperationPos1P[i][2];
        }
        translation[0] = ownSide->operationMove[i].position[0];
        translation[1] = ownSide->operationMove[i].position[1];
        translation[2] = ownSide->operationMove[i].position[2];
        xglMatrixTrans(place, matrix, translation);
        xglMatrixScale(place, place, scale);
        CardDispCard(&place, ownSide->operations[i] & CARD_CURSOR_CARD_ID_MASK, 1);
        nmlModelSetPlace(&place);
        nmlModelSetTexture(CARD_FRAME_TEXTURE);
        nmlModelEntryCard(CARD_FRAME_MODEL);
        if (ownSide->operationMove[i].moveTimer == 0 && (ownSide->operationFlags[i] & CARD_OPERATION_FLAG_MARKER)) {
            translation[0] = OperationPos1P[i][0];
            translation[1] = OperationPos1P[i][1];
            translation[2] = CARD_OPERATION_MARKER_Z;
            xglMatrixTrans(place, matrix, translation);
            nmlModelSetPlace(&place);
            nmlModelSetTexture(CARD_OPERATION_MARKER_TEXTURE);
            nmlModelEntryCard(CARD_OPERATION_MARKER_MODEL);
        }
    }
}

/*
 * Draws the deck pile of one side: the deck count as tens and units digit
 * models, then the pile marker and a second marker below it (both only while the deck is not
 * empty), then every card in motion along its yamaMove record. side is 0 for the player and 1
 * for the enemy, which selects the enemy variant of the two marker models.
 */
void CardPlayDispYama(s32 side, CardPlaySide *ownSide, Matrix4 matrix)
{
    Matrix4 place;
    float translation[4];
    float scale[4];
    s32 count;
    s32 resting;
    s32 i;
    u32 model;

    count = ownSide->hand.deckCount;
    resting = count;
    for (i = 0; i < count; i++) {
        if (ownSide->yamaMove[i].moveTimer != 0) {
            resting--;
        }
    }
    if (resting < 0) {
        resting = 0;
    }

    i = resting / CARD_PILE_STACK_SIZE; /* tens digit of the count (the loop index doubles as its holder) */
    translation[3] = 1.0f;
    scale[1] = CARD_YAMA_COUNT_SCALE;
    scale[2] = 1.0f;
    scale[0] = CARD_YAMA_COUNT_SCALE;
    translation[2] = CARD_PILE_COUNT_Z;
    scale[3] = 1.0f;
    if (i != 0) {
        model = CARD_PILE_COUNT_MODEL + (i * CARD_MODEL_STRIDE);
        translation[0] = CARD_YAMA_COUNT_TENS_X;
        translation[1] = CARD_YAMA_COUNT_Y;
        xglMatrixTrans(place, matrix, translation);
        xglMatrixScale(place, place, scale);
        nmlModelSetPlace(&place);
        nmlModelSetTexture(CARD_PILE_COUNT_TEXTURE);
        nmlModelEntryCard(model);
    }

    model = CARD_PILE_COUNT_MODEL + ((resting % CARD_PILE_STACK_SIZE) * CARD_MODEL_STRIDE);
    translation[0] = CARD_YAMA_COUNT_UNITS_X;
    translation[1] = CARD_YAMA_COUNT_Y;
    xglMatrixTrans(place, matrix, translation);
    xglMatrixScale(place, place, scale);
    nmlModelSetPlace(&place);
    nmlModelSetTexture(CARD_PILE_COUNT_TEXTURE);
    nmlModelEntryCard(model);

    if (resting != 0) {
        translation[0] = 0.0f;
        translation[1] = CARD_YAMA_MARKER_Y;
        xglMatrixTrans(place, matrix, translation);
        nmlModelSetPlace(&place);
        nmlModelSetLight(&CMColor, &asDir);
        nmlModelSetTexture(CARD_YAMA_MARKER_TEXTURE);
        if (side == 0) {
            nmlModelEntryCard(CARD_YAMA_MARKER_MODEL);
        } else {
            nmlModelEntryCard(CARD_YAMA_MARKER_MODEL_ENEMY);
        }

        translation[0] = CARD_YAMA_MARKER2_X;
        translation[1] = CARD_YAMA_MARKER2_Y;
        xglMatrixTrans(place, matrix, translation);
        nmlModelSetPlace(&place);
        nmlModelSetTexture(CARD_YAMA_MARKER2_TEXTURE);
        if (side == 0) {
            nmlModelEntryCard(CARD_YAMA_MARKER2_MODEL);
        } else {
            nmlModelEntryCard(CARD_YAMA_MARKER2_MODEL_ENEMY);
        }
    }

    /* The original tests count < 0 first (bltz) and then count == 0 (beqz) before
     * entering the loop; the signed guard around the loop reproduces that pair of branches. */
    if (count >= 0) {
        for (i = 0; i < count; i++) {
            if (ownSide->yamaMove[i].moveTimer != 0) {
                ownSide->yamaMove[i].moveTimer--;
                if (ownSide->yamaMove[i].kind == 0) {
                    if (ownSide->yamaMove[i].moveTimer == 0) {
                        xglSoundEffectNormalID(CARD_MOVE_ARRIVE_SOUND, 0);
                    }
                    ownSide->yamaMove[i].position[0] += ownSide->yamaMove[i].step[0];
                    ownSide->yamaMove[i].position[1] += ownSide->yamaMove[i].step[1];
                    ownSide->yamaMove[i].position[2] += ownSide->yamaMove[i].step[2];
                    translation[0] = ownSide->yamaMove[i].position[0];
                    translation[1] = ownSide->yamaMove[i].position[1];
                    translation[2] = ownSide->yamaMove[i].position[2];
                    translation[3] = 1.0f;
                    scale[1] = CARD_PILE_CARD_SCALE;
                    scale[0] = CARD_PILE_CARD_SCALE;
                    scale[3] = 1.0f;
                    scale[2] = 1.0f;
                    xglMatrixTrans(place, matrix, translation);
                    xglMatrixScale(place, place, scale);
                    xglMatrixRotY(place, place, CARD_HALF_TURN);
                    CardDispCard(&place, ownSide->deck[i], 1);
                }
            } else if (ownSide->yamaMove[i].kind != 0) {
                ownSide->yamaMove[i].kind = 0;
                ownSide->yamaMove[i].moveTimer = CARD_SLIDE_FRAMES;
            }
        }
    }
}

/*
 * Draws the discard pile of one side: the resting cards (those not sliding
 * in) as digit models, the pile marker on top, then every card in motion along its suteMove
 * record, spun half a turn about Y. A card whose timer is 0 but whose kind is still set has
 * kind cleared and its timer restarted at CARD_SLIDE_FRAMES.
 * The first argument (the side number) is not read.
 */
void CardPlayDispSute(s32 side, CardPlaySide *ownSide, Matrix4 matrix)
{
    Matrix4 place;
    float translation[4];
    float scale[4];
    s32 count;
    s32 resting;
    s32 i;
    u32 model;

    count = ownSide->hand.suteCount;
    resting = count;
    for (i = 0; i < count; i++) {
        if (ownSide->suteMove[i].moveTimer != 0 || ownSide->suteMove[i].kind != 0) {
            resting--;
        }
    }
    if (resting < 0) {
        resting = 0;
    }
    i = resting / CARD_PILE_STACK_SIZE; /* tens digit, held in the loop index i: a separate local gets a different register than the original's for the `sll 10` of the model id */

    translation[3] = 1.0f;
    translation[2] = CARD_PILE_COUNT_Z;
    scale[1] = CARD_PILE_COUNT_SCALE;
    scale[0] = CARD_PILE_COUNT_SCALE;
    scale[2] = 1.0f;
    scale[3] = 1.0f;

    if (i != 0) {
        model = CARD_PILE_COUNT_MODEL + (i * CARD_MODEL_STRIDE);
        translation[0] = CARD_SUTE_COUNT_TENS_X;
        translation[1] = CARD_PILE_COUNT_Y;
        xglMatrixTrans(place, matrix, translation);
        xglMatrixScale(place, place, scale);
        nmlModelSetPlace(&place);
        nmlModelSetTexture(CARD_PILE_COUNT_TEXTURE);
        nmlModelEntryCard(model);
    }

    model = CARD_PILE_COUNT_MODEL + ((resting % CARD_PILE_STACK_SIZE) * CARD_MODEL_STRIDE);
    translation[0] = CARD_SUTE_COUNT_UNITS_X;
    translation[1] = CARD_PILE_COUNT_Y;
    xglMatrixTrans(place, matrix, translation);
    xglMatrixScale(place, place, scale);
    nmlModelSetPlace(&place);
    nmlModelSetTexture(CARD_PILE_COUNT_TEXTURE);
    nmlModelEntryCard(model);

    if (resting != 0 && (resting != 1 || ownSide->suteMove[0].moveTimer == 0 || ownSide->suteMove[0].kind == 0)) {
        translation[0] = CARD_SUTE_PILE_X;
        translation[1] = CARD_SUTE_PILE_Y;
        scale[3] = 1.0f;
        scale[1] = 1.0f;
        scale[0] = 1.0f;
        scale[2] = 1.0f;
        xglMatrixTrans(place, matrix, translation);
        xglMatrixScale(place, place, scale);
        nmlModelSetPlace(&place);
        nmlModelSetTexture(CARD_SUTE_MARKER_TEXTURE);
        nmlModelEntryCard(CARD_SUTE_MARKER_MODEL);
    }

    if (count != 0) {
        for (i = 0; i < count; i++) {
            if (ownSide->suteMove[i].moveTimer != 0) {
                ownSide->suteMove[i].moveTimer--;
                if (ownSide->suteMove[i].moveTimer == 0) {
                    xglSoundEffectNormalID(CARD_MOVE_ARRIVE_SOUND, 0);
                }
                if (ownSide->suteMove[i].kind == 0) {
                    ownSide->suteMove[i].position[0] += ownSide->suteMove[i].step[0];
                    ownSide->suteMove[i].position[1] += ownSide->suteMove[i].step[1];
                    ownSide->suteMove[i].position[2] += ownSide->suteMove[i].step[2];
                    translation[0] = ownSide->suteMove[i].position[0];
                    translation[1] = ownSide->suteMove[i].position[1];
                    translation[2] = ownSide->suteMove[i].position[2];
                    translation[3] = 1.0f;
                    scale[1] = CARD_PILE_CARD_SCALE;
                    scale[0] = CARD_PILE_CARD_SCALE;
                    scale[3] = 1.0f;
                    scale[2] = 1.0f;
                    xglMatrixTrans(place, matrix, translation);
                    xglMatrixScale(place, place, scale);
                    xglMatrixRotY(place, place, CARD_HALF_TURN);
                    CardDispCard(&place, ownSide->sute[i], 1);
                }
            } else if (ownSide->suteMove[i].kind != 0) {
                ownSide->suteMove[i].kind = 0;
                ownSide->suteMove[i].moveTimer = CARD_SLIDE_FRAMES;
            }
        }
    }
}

/*
 * Clears both mark tables of the board cursor: the 24 marks
 * of markTable (+0x30) and of markTableB (+0x48).
 */
void CardCursorPassive(CardCursorControl *cursor)
{
    int i;

    for (i = 0; i < CARD_CURSOR_MARK_COUNT; i++) {
        cursor->markTable[i] = 0;
        cursor->markTableB[i] = 0;
    }
}

/*
 * Sets one mark of the board cursor. The zone (2..7) picks the
 * column of four marks in markTable and position->index the slot in it; mode
 * 1..4 is stored as the mark, any other mode clears it. A zone outside 2..7
 * changes nothing.
 */
void CardCursorActive(CardCursorControl *cursor, CardCursorPosition *position, int mode)
{
    s32 column;

    column = -1;
    switch (position->zone) {
    case CARD_CURSOR_ZONE_3:
        column = CARD_MARK_COLUMN_ZONE3;
        break;
    case CARD_CURSOR_ZONE_4:
        column = CARD_MARK_COLUMN_ZONE4;
        break;
    case CARD_CURSOR_ZONE_2:
        column = CARD_MARK_COLUMN_ZONE2;
        break;
    case CARD_CURSOR_ZONE_7:
        column = CARD_MARK_COLUMN_ZONE7;
        break;
    case CARD_CURSOR_ZONE_5:
        column = CARD_MARK_COLUMN_ZONE5;
        break;
    case CARD_CURSOR_ZONE_6:
        column = CARD_MARK_COLUMN_ZONE6;
        break;
    }
    if (column < 0) {
        return;
    }
    /* Dispatched through a jump table (`sltiu 5`); each mark value
     * has its own store, as in the original. */
    switch (mode) {
    case 0:
    default:
        cursor->markTable[column + position->index] = 0;
        break;
    case CARD_CURSOR_MARK_1:
        cursor->markTable[column + position->index] = CARD_CURSOR_MARK_1;
        break;
    case CARD_CURSOR_MARK_2:
        cursor->markTable[column + position->index] = CARD_CURSOR_MARK_2;
        break;
    case CARD_CURSOR_MARK_3:
        cursor->markTable[column + position->index] = CARD_CURSOR_MARK_3;
        break;
    case CARD_CURSOR_MARK_4:
        cursor->markTable[column + position->index] = CARD_CURSOR_MARK_4;
        break;
    }
}

/* Maps a cursor-area code to a fixed cursor model resource address;
 * codes other than 2, 3 and 4 (including 0 and 1) share the same address. */
int CAMASub(int areaCode) {
    int code;
    int addr;

    code = areaCode & 0xFF;
    addr = CARD_CURSOR_MARK_MODEL_2;
    if ((code != CARD_CURSOR_MARK_2) &&
        ((code < CARD_CURSOR_MARK_3) ||
         (((addr = CARD_CURSOR_MARK_MODEL_3), (code != CARD_CURSOR_MARK_3)) &&
          ((addr = CARD_CURSOR_MARK_MODEL_4), (code != CARD_CURSOR_MARK_4))))) {
        addr = CARD_CURSOR_MARK_MODEL_DEFAULT;
    }
    return addr;
}

/*
 * Draws the board cursor: the animated cursor frame of the side that holds
 * the cursor with its marker model beside it, then the pulsing lights and the mark models of
 * the six zone columns of CardCursorControl.markTable (own side, then the far side turned
 * half a turn about Z). A zero `mode` also advances the animation and both light pulses.
 */
void CardPlayDispCursor(s32 mode, CardGameWork *work, Matrix4 matrix)
{
    Matrix4 place;
    /* Scratch vectors, reused for scales and translations. */
    float vectorA[4];
    float vectorB[4];
    Matrix4 markPlace;
    u32 frameModel;
    u32 markerModel;
    u32 markModel;
    /* Light intensities: lightA CLColor, lightB CLP1Color blue and CLP2Color red, lightC CLP1Color
     * red and green, lightD CLP2Color green and blue. */
    float lightA;
    float lightB;
    float lightC;
    float lightD;
    int i;
    /* block addresses the cursor files and marks from a base offset of 0x5B50;
     * the controlling side and the pulse counters stay on work->cursor, because reading them through
     * block->control changes the register assignment of the whole function. */
    CardCursorBlock *block = &work->cursorBlock;

    if (block->control.selectionMode != 0 && block->lex[0] != 0) {
        if (work->cursor.controllingSide >= 0) {
            markerModel = CARD_CURSOR_MARKER_MODEL;
            frameModel = CARD_CURSOR_FRAME_MODEL;
            if (work->cursor.controllingSide != 0) {
                markerModel = CARD_CURSOR_MARKER_MODEL_ENEMY;
                frameModel = CARD_CURSOR_FRAME_MODEL_ENEMY;
            }
        } else {
            switch (work->command.phase) {
            case CARD_PHASE_DRAW:
            case CARD_PHASE_MOVE:
            case CARD_PHASE_EVENT:
            case CARD_PHASE_SET:
            case CARD_PHASE_BATTLE:
            case CARD_PHASE_ADJUST:
            case CARD_PHASES_PER_SIDE + CARD_PHASE_BLOCK:
                markerModel = CARD_CURSOR_MARKER_MODEL;
                frameModel = CARD_CURSOR_FRAME_MODEL;
                break;
            default:
                frameModel = CARD_CURSOR_FRAME_MODEL_ENEMY;
                markerModel = CARD_CURSOR_MARKER_MODEL_ENEMY;
                break;
            }
        }
        if (mode == 0) {
            block->control.animFrame++;
        }
        if (block->control.animFrame >= CARD_CURSOR_ANIM_TICKS) {
            block->control.animFrame = 0;
        }
        frameModel += (block->control.animFrame >> CARD_CURSOR_ANIM_TICKS_PER_FRAME_SHIFT) * CARD_CURSOR_FRAME_STRIDE;
        vectorB[1] = CARD_CURSOR_SCALE;
        vectorB[0] = CARD_CURSOR_SCALE;
        vectorB[3] = 1.0f;
        vectorB[2] = 1.0f;
        xglMatrixTrans(place, matrix, block->control.position);
        xglMatrixScale(place, place, vectorB);
        xglMatrixRotZ(place, place, -CARD_RADIANS(work->rotStageVector[2]));
        xglMatrixRotX(place, place, -CARD_RADIANS(work->rotStageVector[1]));
        nmlModelSetPlace(&place);
        nmlModelSetTexture(block->xtx[0]);
        nmlModelEntryCard(frameModel);
        vectorA[0] = CARD_CURSOR_MARKER_X;
        vectorA[1] = CARD_CURSOR_MARKER_Y;
        vectorA[3] = 1.0f;
        vectorA[2] = 0.0f;
        xglMatrixTrans(place, place, vectorA);
        nmlModelSetPlace(&place);
        nmlModelSetTexture(CARD_CURSOR_MARKER_TEXTURE);
        nmlModelEntryCard(markerModel);
    }
    vectorB[3] = 1.0f;
    vectorA[3] = 1.0f;
    vectorA[2] = 1.0f;
    if (mode == 0) {
        work->cursor.pulseA = (work->cursor.pulseA + 1) & CARD_CURSOR_PULSE_A_MASK;
    }
    lightA = xglSin(CARD_RADIANS(work->cursor.pulseA * CARD_CURSOR_PULSE_A_STEP)) * CARD_CURSOR_LIGHT_A_SWING + 1.0f;
    lightB = xglSin(CARD_RADIANS(work->cursor.pulseA * CARD_CURSOR_PULSE_A_STEP)) * CARD_CURSOR_LIGHT_B_SWING + CARD_CURSOR_LIGHT_B_BASE;
    lightC = xglSin(CARD_RADIANS(work->cursor.pulseA * CARD_CURSOR_PULSE_A_STEP)) * CARD_CURSOR_LIGHT_C_SWING + CARD_CURSOR_LIGHT_C_BASE;
    lightD = xglSin(CARD_RADIANS(work->cursor.pulseA * CARD_CURSOR_PULSE_A_STEP)) * CARD_CURSOR_LIGHT_D_SWING + CARD_CURSOR_LIGHT_D_BASE;
    CLP1Color[3][2] = CLP2Color[3][0] = lightB;
    CLColor[3][0] = CLColor[3][1] = CLColor[3][2] = lightA;
    CLP1Color[3][0] = CLP1Color[3][1] = lightC;
    CLP2Color[3][1] = CLP2Color[3][2] = lightD;
    if (mode == 0) {
        work->cursor.pulseB = (work->cursor.pulseB + 1) & CARD_CURSOR_PULSE_B_MASK;
    }
    lightA = xglSin(CARD_RADIANS(work->cursor.pulseB * CARD_CURSOR_PULSE_B_STEP)) * CARD_CURSOR_LIGHT_E_SWING + 1.0f;
    CMColor[3][0] = CMColor[3][1] = CMColor[3][2] = lightA;

    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        vectorB[0] = DisposePos1P[i][0];
        vectorB[1] = DisposePos1P[i][1];
        vectorB[2] = DisposePos1P[i][2] + CARD_CURSOR_MARK_Z_LIFT;
        vectorA[1] = CARD_CURSOR_MARK_SCALE_SMALL;
        vectorA[0] = CARD_CURSOR_MARK_SCALE_SMALL;
        if (block->control.markTable[CARD_MARK_COLUMN_ZONE6 + i] != 0) {
            markModel = CAMASub(block->control.markTable[CARD_MARK_COLUMN_ZONE6 + i]);
            xglMatrixTrans(markPlace, matrix, vectorB);
            xglMatrixScale(markPlace, markPlace, vectorA);
            nmlModelSetLight(&CLColor, &asDir);
            nmlModelSetPlace(&markPlace);
            nmlModelSetTexture(block->xtx[2]);
            nmlModelEntryCard(markModel);
        }
    }
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        vectorB[0] = BattlePos1P[i][0];
        vectorB[1] = BattlePos1P[i][1];
        vectorB[2] = BattlePos1P[i][2] + CARD_CURSOR_MARK_Z_LIFT;
        vectorA[1] = CARD_CURSOR_MARK_SCALE_LARGE;
        vectorA[0] = CARD_CURSOR_MARK_SCALE_LARGE;
        if (block->control.markTable[CARD_MARK_COLUMN_ZONE5 + i] != 0) {
            markModel = CAMASub(block->control.markTable[CARD_MARK_COLUMN_ZONE5 + i]);
            xglMatrixTrans(markPlace, matrix, vectorB);
            xglMatrixScale(markPlace, markPlace, vectorA);
            nmlModelSetLight(&CLColor, &asDir);
            nmlModelSetPlace(&markPlace);
            nmlModelSetTexture(block->xtx[2]);
            nmlModelEntryCard(markModel);
        }
    }
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        vectorB[0] = OperationPos1P[i][0];
        vectorB[1] = OperationPos1P[i][1];
        vectorB[2] = OperationPos1P[i][2] + CARD_CURSOR_MARK_Z_LIFT;
        vectorA[1] = CARD_CURSOR_MARK_SCALE_SMALL;
        vectorA[0] = CARD_CURSOR_MARK_SCALE_SMALL;
        if (block->control.markTable[CARD_MARK_COLUMN_ZONE7 + i] != 0) {
            markModel = CAMASub(block->control.markTable[CARD_MARK_COLUMN_ZONE7 + i]);
            xglMatrixTrans(markPlace, matrix, vectorB);
            xglMatrixScale(markPlace, markPlace, vectorA);
            nmlModelSetLight(&CLColor, &asDir);
            nmlModelSetPlace(&markPlace);
            nmlModelSetTexture(block->xtx[2]);
            nmlModelEntryCard(markModel);
        }
    }
    xglMatrixRotZ(place, matrix, CARD_HALF_TURN);
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        vectorB[0] = DisposePos1P[i][0];
        vectorB[1] = DisposePos1P[i][1];
        vectorB[2] = DisposePos1P[i][2] + CARD_CURSOR_MARK_Z_LIFT;
        vectorA[1] = CARD_CURSOR_MARK_SCALE_SMALL;
        vectorA[0] = CARD_CURSOR_MARK_SCALE_SMALL;
        if (block->control.markTable[CARD_MARK_COLUMN_ZONE2 + i] != 0) {
            markModel = CAMASub(block->control.markTable[CARD_MARK_COLUMN_ZONE2 + i]);
            xglMatrixTrans(markPlace, place, vectorB);
            xglMatrixScale(markPlace, markPlace, vectorA);
            nmlModelSetLight(&CLColor, &asDir);
            nmlModelSetPlace(&markPlace);
            nmlModelSetTexture(block->xtx[2]);
            nmlModelEntryCard(markModel);
        }
    }
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        vectorB[0] = BattlePos1P[i][0];
        vectorB[1] = BattlePos1P[i][1];
        vectorB[2] = BattlePos1P[i][2] + CARD_CURSOR_MARK_Z_LIFT;
        vectorA[1] = CARD_CURSOR_MARK_SCALE_LARGE;
        vectorA[0] = CARD_CURSOR_MARK_SCALE_LARGE;
        if (block->control.markTable[CARD_MARK_COLUMN_ZONE4 + i] != 0) {
            markModel = CAMASub(block->control.markTable[CARD_MARK_COLUMN_ZONE4 + i]);
            xglMatrixTrans(markPlace, place, vectorB);
            xglMatrixScale(markPlace, markPlace, vectorA);
            nmlModelSetLight(&CLColor, &asDir);
            nmlModelSetPlace(&markPlace);
            nmlModelSetTexture(block->xtx[2]);
            nmlModelEntryCard(markModel);
        }
    }
    for (i = 0; i < CARD_BOARD_SLOTS; i++) {
        vectorB[0] = OperationPos1P[i][0];
        vectorB[1] = OperationPos1P[i][1];
        vectorB[2] = OperationPos1P[i][2] + CARD_CURSOR_MARK_Z_LIFT;
        vectorA[1] = CARD_CURSOR_MARK_SCALE_SMALL;
        vectorA[0] = CARD_CURSOR_MARK_SCALE_SMALL;
        if (block->control.markTable[CARD_MARK_COLUMN_ZONE3 + i] != 0) {
            markModel = CAMASub(block->control.markTable[CARD_MARK_COLUMN_ZONE3 + i]);
            xglMatrixTrans(markPlace, place, vectorB);
            xglMatrixScale(markPlace, markPlace, vectorA);
            nmlModelSetLight(&CLColor, &asDir);
            nmlModelSetPlace(&markPlace);
            nmlModelSetTexture(block->xtx[2]);
            nmlModelEntryCard(markModel);
        }
    }
}

/*
 * Returns 1 when a card on one side's four battle and four disposal slots, at any
 * layer, has the given CardDefinition identity, and 0 otherwise. Identities from
 * CARD_IDENTITY_COUNT up are never checked.
 */
int CardPlayIdentityCheck(CardGameWork *work, CardPlaySide *side, u16 identity)
{
    int layer;
    int found;
    s16 cardId;

    found = 0;
    if (identity < CARD_IDENTITY_COUNT) {
        layer = 0;
        do {
            cardId = side->battle[0].battle.layers[layer].cardId;
            if (cardId >= 0 && work->definitions[cardId].identity == identity) {
                found = 1;
                break;
            }
            cardId = side->battle[1].battle.layers[layer].cardId;
            if (cardId >= 0 && work->definitions[cardId].identity == identity) {
                found = 1;
                break;
            }
            cardId = side->battle[2].battle.layers[layer].cardId;
            if (cardId >= 0 && work->definitions[cardId].identity == identity) {
                found = 1;
                break;
            }
            cardId = side->battle[3].battle.layers[layer].cardId;
            if (cardId >= 0 && work->definitions[cardId].identity == identity) {
                found = 1;
                break;
            }
            cardId = side->disposal[0].battle.layers[layer].cardId;
            if (cardId >= 0 && work->definitions[cardId].identity == identity) {
                found = 1;
                break;
            }
            cardId = side->disposal[1].battle.layers[layer].cardId;
            if (cardId >= 0 && work->definitions[cardId].identity == identity) {
                found = 1;
                break;
            }
            cardId = side->disposal[2].battle.layers[layer].cardId;
            if (cardId >= 0 && work->definitions[cardId].identity == identity) {
                found = 1;
                break;
            }
            cardId = side->disposal[3].battle.layers[layer].cardId;
            if (cardId >= 0 && work->definitions[cardId].identity == identity) {
                found = 1;
                break;
            }
            layer++;
        } while (layer < CARD_BATTLE_LAYER_COUNT);
    }
    return found;
}

/*
 * Draws the cards of one hand. The cards are spaced evenly along the window edge
 * win0pos, from its first corner to its second, and each card sits CARD_HAND_DEPTH_STEP
 * further back than the one before it. The card at `selected` is placed one step deeper
 * than the last card, (count + 1) steps back, and drawn at CARD_HAND_SELECTED_SCALE; the
 * others are drawn at CARD_HAND_SCALE. The hand model is then placed with the same matrix
 * and entered. flipped is never read; the parameter is kept as the original has it.
 */
void CGCPHandDispSubCore(CardHand *hand, int selected, Matrix4 *matrix, int flipped)
{
    Matrix4 cardMatrix;
    float position[4];
    float normalScale[4];
    float selectedScale[4];
    float spacing;
    int count;
    int i;

    count = CardPlayHandCnt(hand);
    if (count < 2) {
        spacing = (win0pos[1][0] - win0pos[0][0]) * CARD_SINGLE_CARD_SPACING;
    } else {
        spacing = (win0pos[1][0] - win0pos[0][0]) / (float)(count - 1);
    }
    position[1] = win0pos[0][1];
    position[2] = win0pos[0][2];
    position[3] = 1.0f;
    normalScale[0] = CARD_HAND_SCALE;
    normalScale[1] = CARD_HAND_SCALE;
    normalScale[2] = 1.0f;
    normalScale[3] = 1.0f;
    selectedScale[0] = CARD_HAND_SELECTED_SCALE;
    selectedScale[1] = CARD_HAND_SELECTED_SCALE;
    selectedScale[2] = 1.0f;
    selectedScale[3] = 1.0f;
    for (i = 0; i < count; i++) {
        position[0] = win0pos[0][0] + spacing * (float)i;
        position[2] = win0pos[0][2] + (float)i * CARD_HAND_DEPTH_STEP;
        if (i == selected) {
            position[2] = win0pos[0][2] + (float)(count + 1) * CARD_HAND_DEPTH_STEP;
        }
        xglMatrixTrans(cardMatrix, *matrix, position);
        if (i == selected) {
            xglMatrixScale(cardMatrix, cardMatrix, selectedScale);
            CardDisp(&cardMatrix, hand->cards[i], 1, 0);
        } else {
            xglMatrixScale(cardMatrix, cardMatrix, normalScale);
            CardDispCard(&cardMatrix, hand->cards[i], 1);
        }
    }
    nmlModelSetTexture(CARD_HAND_TEXTURE);
    nmlModelSetPlace(matrix);
    nmlModelEntryCard(CARD_HAND_MODEL);
}

/*
 * Draws one generation icon per entry of need, CARD_GENERATION_ICON_STEP apart along a
 * row; flipped moves the row to the other side of the board. The entries 1, 2, 8, 4 and
 * 16 select the icon of generation 0..4 (the switch falls through 16, 4, 8, 2 and counts
 * one more per step); 31 draws the model with index 7
 * (three past generation 4) and takes its count from the first non-zero entry of counts; any
 * other entry draws nothing. A used count is decremented; when it is already zero the
 * icon is drawn with the dim light HalfColor.
 */
void CGCPNeedDispSub(int flipped, u8 *need, Matrix4 *matrix, u8 *counts)
{
    Matrix4 cardMatrix;
    Matrix4 placement;
    float translation[4];
    float scale[4];
    unsigned int model;
    int slot;
    int generation;
    int anyGeneration;
    int spent;
    int j;

    if (!flipped) {
        translation[0] = CARD_NEED_ROW_X;
        translation[1] = CARD_NEED_ROW_Y;
    } else {
        translation[0] = CARD_NEED_ROW_FLIPPED_X;
        translation[1] = CARD_NEED_ROW_FLIPPED_Y;
    }
    translation[2] = 0.0f;
    translation[3] = 1.0f;
    scale[0] = CARD_NEED_ICON_SCALE;
    scale[1] = CARD_NEED_ICON_SCALE;
    scale[3] = 1.0f;
    scale[2] = 1.0f;
    xglMatrixTrans(cardMatrix, *matrix, translation);
    translation[0] = CARD_GENERATION_ICON_STEP;
    translation[1] = translation[2] = 0.0f;
    /* One icon per need entry: resolve its generation, then spend a count for it. */
    for (slot = 0; slot < CARD_NEED_SLOT_COUNT; slot++) {
        anyGeneration = 0;
        generation = 0;
        switch (need[slot]) {
        case CARD_NEED_ANY:
            anyGeneration = 1;
            generation = 3; /* this plus the four increments below is model index 7 */
            /* fall through */
        case CARD_NEED_GENERATION4:
            generation++;
            /* fall through */
        case CARD_NEED_GENERATION3:
            generation++;
            /* fall through */
        case CARD_NEED_GENERATION2:
            generation++;
            /* fall through */
        case CARD_NEED_GENERATION1:
            generation++;
            break;
        case CARD_NEED_GENERATION0:
            break;
        default:
            generation = -1;
            break;
        }
        if (generation >= 0) {
            model = CARD_ICON_MODEL_BASE + generation * CARD_MODEL_STRIDE;
            xglMatrixScale(placement, cardMatrix, scale);
            nmlModelSetPlace(&placement);
            if (anyGeneration) {
                spent = 0;
                for (j = 0; j < CARD_GENERATION_COUNT; j++) {
                    if (counts[j] != 0) {
                        counts[j]--;
                        spent = 1;
                        break;
                    }
                }
                if (!spent) {
                    nmlModelSetLight(&HalfColor, &asDir);
                }
            } else {
                if (counts[generation] != 0) {
                    counts[generation]--;
                } else {
                    nmlModelSetLight(&HalfColor, &asDir);
                }
            }
            nmlModelSetTexture(CARD_GENERATION_TEXTURE);
            nmlModelEntryCard(model);
            xglMatrixTrans(cardMatrix, cardMatrix, translation);
        }
    }
}

/*
 * Draws the generation markers of one side: CardCountGeneration
 * fills counts[0..4], then for each generation slot counts[slot] icons are drawn
 * in a row. The row starts at baseMatrix moved by (-0.145, 0.675), or by
 * (0.212, -0.418) when flipped is set; every icon is drawn scaled by 0.9 in x
 * and y and the row then advances by 0.155 in x.
 */
void CGCPGeneDispSub(int flipped, CardDefinition *definitions, CardPlaySide *side, Matrix4 baseMatrix, u8 *counts)
{
    Matrix4 matrix;
    Matrix4 placement;
    float translation[4];
    float scale[4];
    int slot;
    int i;

    if (!flipped) {
        translation[0] = CARD_GENE_ROW_X;
        translation[1] = CARD_GENE_ROW_Y;
    } else {
        translation[0] = CARD_GENE_ROW_FLIPPED_X;
        translation[1] = CARD_GENE_ROW_FLIPPED_Y;
    }
    translation[3] = 1.0f;
    scale[0] = CARD_GENE_ICON_SCALE;
    scale[3] = 1.0f;
    scale[2] = 1.0f;
    scale[1] = CARD_GENE_ICON_SCALE;
    translation[2] = 0.0f;
    xglMatrixTrans(matrix, baseMatrix, translation);
    translation[1] = translation[2] = 0.0f;
    translation[0] = CARD_GENERATION_ICON_STEP;
    CardCountGeneration(counts, definitions, side);
    for (slot = 0; slot < CARD_GENERATION_COUNT; slot++) {
        if (counts[slot] != 0) {
            for (i = 0; i < counts[slot]; i++) {
                /* Declared here, not at the top: the slot * CARD_MODEL_STRIDE offset is
                 * computed first, ahead of the array addresses. */
                unsigned int model = CARD_ICON_MODEL_BASE + slot * CARD_MODEL_STRIDE;

                xglMatrixScale(placement, matrix, scale);
                nmlModelSetPlace(&placement);
                nmlModelSetTexture(CARD_GENERATION_TEXTURE);
                nmlModelEntryCard(model);
                xglMatrixTrans(matrix, matrix, translation);
            }
        }
    }
}

/*
 * Draws the hand of one side on the battle window: the cards are
 * spread from NewWin0pos[2] to NewWin0pos[3] like CGCPHandDispSubCore, each
 * CARD_WINDOW_DEPTH_STEP further back, with the card at `selected` drawn larger and in
 * front. Every card is drawn with CardDispCard and a frame model; the last card of the
 * hand omits the CARD_DISP_OVERLAY flag. A non-zero `mode` tints the cards whose
 * definition kind is 1, 2 or 4: the mode bit of the same value selects the highlight
 * (CARD_DISP_LIGHT_CN) over the dim light (CARD_DISP_LIGHT_MIDDLE). Cards whose
 * handMove record is still moving are skipped. The generation and need rows are drawn
 * after the loop.
 */
void CGCPHandDispSub3(CardDefinition *definitions, CardPlaySide *side, int selected, int mode, int option)
{
    Matrix4 matrix;
    Matrix4 placement;
    Matrix4 cardMatrix;
    float position[4];
    float normalScale[4];
    float selectedScale[4];
    u8 counts[CARD_GENERATION_COUNT];
    float spacing;
    int count;
    int i;
    int flags;

    xglMatrixUnit(matrix);
    position[0] = 0.0f;
    position[1] = CARD_HAND3_BASE_Y;
    if (option) {
        position[1] = CARD_HAND3_BASE_Y_OPTION;
    }
    position[2] = CARD_HAND3_BASE_Z;
    position[3] = 1.0f;
    normalScale[0] = 1.0f;
    normalScale[1] = CARD_HAND3_BASE_SCALE_Y;
    normalScale[2] = 1.0f;
    normalScale[3] = 1.0f;
    xglMatrixTrans(matrix, matrix, position);
    xglMatrixScale(matrix, matrix, normalScale);
    count = CardPlayHandCnt(&side->hand);
    if (count < 2) {
        spacing = (NewWin0pos[3][0] - NewWin0pos[2][0]) * CARD_SINGLE_CARD_SPACING;
    } else {
        spacing = (NewWin0pos[3][0] - NewWin0pos[2][0]) / (float)(count - 1);
    }
    position[1] = NewWin0pos[2][1];
    position[2] = NewWin0pos[2][2];
    position[3] = 1.0f;
    normalScale[0] = CARD_HAND3_SCALE;
    normalScale[1] = CARD_HAND3_SCALE;
    normalScale[2] = 1.0f;
    normalScale[3] = 1.0f;
    selectedScale[0] = CARD_HAND3_SELECTED_SCALE;
    selectedScale[1] = CARD_HAND3_SELECTED_SCALE;
    selectedScale[2] = 1.0f;
    selectedScale[3] = 1.0f;
    /* Per card: place it on the window edge, tint it from mode, then draw it unless it is moving. */
    for (i = 0; i < count; i++) {
        flags = CARD_DISP_ENABLED | CARD_DISP_OVERLAY;
        if (i == count - 1) {
            flags = CARD_DISP_ENABLED;
        }
        position[0] = NewWin0pos[2][0] + spacing * (float)i;
        position[2] = NewWin0pos[2][2] + (float)i * CARD_WINDOW_DEPTH_STEP;
        if (i == selected) {
            position[2] = NewWin0pos[0][2] + (float)(count + 1) * CARD_WINDOW_DEPTH_STEP;
        }
        if (mode != 0) {
            switch (definitions[side->hand.cards[i]].kind) {
            case CARD_KIND_BATTLE:
                if ((mode & CARD_KIND_BATTLE) == 0) {
                    flags |= CARD_DISP_LIGHT_MIDDLE;
                } else {
                    flags |= CARD_DISP_LIGHT_CN;
                }
                break;
            case CARD_KIND_EVENT:
                if ((mode & CARD_KIND_EVENT) == 0) {
                    flags |= CARD_DISP_LIGHT_MIDDLE;
                } else {
                    flags |= CARD_DISP_LIGHT_CN;
                }
                break;
            case CARD_KIND_SITUATION:
                if ((mode & CARD_KIND_SITUATION) == 0) {
                    flags |= CARD_DISP_LIGHT_MIDDLE;
                } else {
                    flags |= CARD_DISP_LIGHT_CN;
                }
                break;
            }
        }
        xglMatrixUnit(cardMatrix);
        xglMatrixTrans(placement, matrix, position);
        cardMatrix[3][0] = placement[3][0];
        cardMatrix[3][1] = placement[3][1];
        cardMatrix[3][2] = placement[3][2];
        if (side->handMove[i].kind != 0) {
            continue;
        }
        if (side->handMove[i].moveTimer != 0) {
            continue;
        }
        if (i == selected) {
            xglMatrixScale(cardMatrix, cardMatrix, selectedScale);
            CardDispCard(&cardMatrix, side->hand.cards[selected], flags);
            nmlModelSetLight(&CLColor, &asDir);
            nmlModelSetPlace(&cardMatrix);
            nmlModelSetTexture(CARD_HAND3_SELECTED_TEXTURE);
            nmlModelEntryCard(CARD_HAND3_SELECTED_MODEL);
        } else {
            xglMatrixScale(cardMatrix, cardMatrix, normalScale);
            CardDispCard(&cardMatrix, side->hand.cards[i], flags);
            if (flags & CARD_DISP_LIGHT_MIDDLE) {
                nmlModelSetLight(&MiddleColor, &asDir);
            } else if (flags & CARD_DISP_LIGHT_CN) {
                nmlModelSetLight(&CLColor, &asDir);
            }
            nmlModelSetPlace(&cardMatrix);
            nmlModelSetTexture(CARD_FRAME_TEXTURE);
            nmlModelEntryCard(CARD_FRAME_MODEL);
        }
    }
    /* Hand base model, then the generation and need rows. */
    nmlModelSetPlace(&matrix);
    nmlModelSetLight(&CNColor, &asDir);
    nmlModelSetTexture(CARD_HAND3_BASE_TEXTURE);
    nmlModelEntryCard(CARD_HAND3_BASE_MODEL);
    CGCPGeneDispSub(1, definitions, side, matrix, counts);
    CGCPNeedDispSub(1, definitions[side->hand.cards[selected]].need, &matrix, counts);
}

/*
 * Draws the hand of one side on the battle window: like
 * CGCPHandDispSub3 but spread from NewWin0pos[0] to NewWin0pos[1], with the selected
 * card lifted from win0pos. The pile count is drawn as one or two digit models and the
 * generation and need rows follow the base model.
 */
void CGCPHandDispSub2(CardDefinition *definitions, CardPlaySide *side, int selected, int mode)
{
    Matrix4 matrix;
    Matrix4 placement;
    Matrix4 cardMatrix;
    float position[4];
    float normalScale[4];
    float selectedScale[4];
    u8 counts[CARD_GENERATION_COUNT];
    float spacing;
    int count;
    int i;
    int flags;
    u32 model; /* the tens digit of the pile count, then the digit model id */

    xglMatrixUnit(matrix);
    position[0] = 0.0f;
    position[1] = CARD_HAND2_BASE_Y;
    position[2] = CARD_HAND2_BASE_Z;
    position[3] = 1.0f;
    normalScale[0] = 1.0f;
    normalScale[1] = CARD_HAND2_BASE_SCALE_Y;
    normalScale[2] = 1.0f;
    normalScale[3] = 1.0f;
    xglMatrixTrans(matrix, matrix, position);
    xglMatrixScale(matrix, matrix, normalScale);
    count = CardPlayHandCnt(&side->hand);
    if (count < 2) {
        spacing = (NewWin0pos[1][0] - NewWin0pos[0][0]) * CARD_SINGLE_CARD_SPACING;
    } else {
        spacing = (NewWin0pos[1][0] - NewWin0pos[0][0]) / (float)(count - 1);
    }
    position[1] = NewWin0pos[0][1];
    position[2] = NewWin0pos[0][2];
    position[3] = 1.0f;
    normalScale[0] = CARD_HAND2_SCALE;
    normalScale[1] = CARD_HAND2_SCALE;
    normalScale[2] = 1.0f;
    normalScale[3] = 1.0f;
    selectedScale[0] = CARD_HAND2_SELECTED_SCALE;
    selectedScale[1] = CARD_HAND2_SELECTED_SCALE;
    selectedScale[2] = 1.0f;
    selectedScale[3] = 1.0f;
    /* Per card: place it on the window edge, tint it from mode, then draw it unless it is moving. */
    for (i = 0; i < count; i++) {
        flags = CARD_DISP_ENABLED | CARD_DISP_OVERLAY;
        if (i == count - 1) {
            flags = CARD_DISP_ENABLED;
        }
        position[0] = NewWin0pos[0][0] + spacing * (float)i;
        position[2] = NewWin0pos[0][2] + (float)i * CARD_WINDOW_DEPTH_STEP;
        if (i == selected) {
            /* The selected card's depth is taken from win0pos, not NewWin0pos. */
            position[2] = win0pos[0][2] + (float)(count + 1) * CARD_WINDOW_DEPTH_STEP;
        }
        if (mode != 0) {
            switch (definitions[side->hand.cards[i]].kind) {
            case CARD_KIND_BATTLE:
                if ((mode & CARD_KIND_BATTLE) == 0) {
                    flags |= CARD_DISP_LIGHT_MIDDLE;
                } else {
                    flags |= CARD_DISP_LIGHT_CN;
                }
                break;
            case CARD_KIND_EVENT:
                if ((mode & CARD_KIND_EVENT) == 0) {
                    flags |= CARD_DISP_LIGHT_MIDDLE;
                } else {
                    flags |= CARD_DISP_LIGHT_CN;
                }
                break;
            case CARD_KIND_SITUATION:
                if ((mode & CARD_KIND_SITUATION) == 0) {
                    flags |= CARD_DISP_LIGHT_MIDDLE;
                } else {
                    flags |= CARD_DISP_LIGHT_CN;
                }
                break;
            }
        }
        xglMatrixUnit(cardMatrix);
        xglMatrixTrans(placement, matrix, position);
        cardMatrix[3][0] = placement[3][0];
        cardMatrix[3][1] = placement[3][1];
        cardMatrix[3][2] = placement[3][2];
        if (side->handMove[i].kind != 0) {
            continue;
        }
        if (side->handMove[i].moveTimer != 0) {
            continue;
        }
        if (i == selected) {
            xglMatrixScale(cardMatrix, cardMatrix, selectedScale);
            CardDispCard(&cardMatrix, side->hand.cards[selected], flags);
            nmlModelSetLight(&CLColor, &asDir);
            nmlModelSetPlace(&cardMatrix);
            nmlModelSetTexture(CARD_HAND2_SELECTED_TEXTURE);
            nmlModelEntryCard(CARD_HAND2_SELECTED_MODEL);
        } else {
            xglMatrixScale(cardMatrix, cardMatrix, normalScale);
            CardDispCard(&cardMatrix, side->hand.cards[i], flags);
            nmlModelSetPlace(&cardMatrix);
            if (flags & CARD_DISP_LIGHT_MIDDLE) {
                nmlModelSetLight(&MiddleColor, &asDir);
            } else if (flags & CARD_DISP_LIGHT_CN) {
                nmlModelSetLight(&CLColor, &asDir);
            }
            nmlModelSetTexture(CARD_FRAME_TEXTURE);
            nmlModelEntryCard(CARD_FRAME_MODEL);
        }
    }
    /* Pile count: the tens digit when there is one, then the units digit. */
    model = count / CARD_PILE_STACK_SIZE;
    position[2] = 0.0f;
    normalScale[0] = CARD_HAND2_PILE_SCALE;
    position[1] = CARD_HAND2_PILE_Y;
    normalScale[1] = CARD_HAND2_PILE_SCALE;
    normalScale[2] = 1.0f;
    if (model != 0) {
        position[0] = CARD_HAND2_TENS_X;
        model = CARD_PILE_COUNT_MODEL + (model * CARD_MODEL_STRIDE);
        xglMatrixTrans(placement, matrix, position);
        xglMatrixScale(placement, placement, normalScale);
        nmlModelSetPlace(&placement);
        nmlModelSetTexture(CARD_PILE_COUNT_TEXTURE);
        nmlModelEntryCard(model);
    }
    model = CARD_PILE_COUNT_MODEL + ((count % CARD_PILE_STACK_SIZE) * CARD_MODEL_STRIDE);
    position[0] = CARD_HAND2_UNITS_X;
    xglMatrixTrans(placement, matrix, position);
    xglMatrixScale(placement, placement, normalScale);
    nmlModelSetPlace(&placement);
    nmlModelSetTexture(CARD_PILE_COUNT_TEXTURE);
    nmlModelEntryCard(model);

    nmlModelSetPlace(&matrix);
    nmlModelSetLight(&CNColor, &asDir);
    nmlModelSetTexture(CARD_HAND2_BASE_TEXTURE);
    nmlModelEntryCard(CARD_HAND2_BASE_MODEL);
    CGCPGeneDispSub(0, definitions, side, matrix, counts);
    CGCPNeedDispSub(0, definitions[side->hand.cards[selected]].need, &matrix, counts);
}

/*
 * Draws the hand of cards for the given side. The matrix
 * places the hand: a translation of (0, -0.95, 0.1) that becomes (0, 0.95, 0.1)
 * when flipped is set. CGCPHandDispSubCore draws the cards.
 */
void CGCPHandDispSub(CardHand *hand, int selected, int flipped)
{
    Matrix4 matrix;
    float offset[4];

    xglMatrixUnit(matrix);
    offset[0] = 0.0f;
    offset[1] = -CARD_HAND_PANEL_Y;
    if (flipped) {
        offset[1] = CARD_HAND_PANEL_Y;
    }
    offset[2] = CARD_HAND_PANEL_Z;
    offset[3] = 1.0f;
    xglMatrixTrans(matrix, matrix, offset);
    CGCPHandDispSubCore(hand, selected, &matrix, flipped);
}

/*
 * Draws the yama (deck) of one side: the top `count` cards of the pile are spaced evenly
 * along the window edge win0pos like a hand, each CARD_WINDOW_DEPTH_STEP further back than
 * the one before it. The card at `selectedOffset` is drawn larger and in front. `style`
 * selects the highlighted CardDisp mode (CARD_YAMA_SELECTED_MODE); without a style and with
 * CARD_YAMA_MARK_FLAG set in flags, the selected card is also drawn as a mark at
 * (0, CARD_YAMA_MARK_Y). Afterwards the deck back and the pile model are entered and the
 * caption string is printed.
 */
void CGCPYamaDispSub(CardPlaySide *side, int selectedOffset, int count, u16 flags, int style)
{
    Matrix4 base;
    Matrix4 cardMatrix;
    float position[4];
    float normalScale[4];
    float selectedScale[4];
    Matrix4 markMatrix;
    float spacing;
    int yamaCount;
    int i;
    int effect;

    yamaCount = CardPlayYamaCnt(side);
    if (yamaCount < count) {
        count = yamaCount;
    }
    if (count == 1) {
        spacing = 0.0f;
    } else if (count < 2) {
        spacing = (win0pos[1][0] - win0pos[0][0]) * CARD_SINGLE_CARD_SPACING;
    } else {
        spacing = (win0pos[1][0] - win0pos[0][0]) / (float)(count - 1);
    }
    xglMatrixUnit(base);
    position[0] = 0.0f;
    position[1] = 0.0f;
    position[2] = CARD_WINDOW_BASE_DEPTH;
    position[3] = 1.0f;
    xglMatrixTrans(base, base, position);
    position[1] = win0pos[0][1];
    position[2] = win0pos[0][2];
    position[3] = 1.0f;
    normalScale[0] = CARD_HAND_SCALE;
    normalScale[1] = CARD_HAND_SCALE;
    normalScale[2] = 1.0f;
    normalScale[3] = 1.0f;
    selectedScale[0] = CARD_HAND_SELECTED_SCALE;
    selectedScale[1] = CARD_HAND_SELECTED_SCALE;
    selectedScale[2] = 1.0f;
    selectedScale[3] = 1.0f;
    /* The top `count` cards of the pile, the selected one lifted and drawn in the highlighted mode. */
    for (i = yamaCount - count; i < yamaCount; i++) {
        position[0] = win0pos[0][0] + spacing * (float)(i - (yamaCount - count));
        position[1] = win0pos[0][1];
        position[2] = win0pos[0][2] + (float)(i - (yamaCount - count)) * CARD_WINDOW_DEPTH_STEP;
        if (i == yamaCount - count + selectedOffset) {
            position[2] = win0pos[0][2] + (float)(count + 1) * CARD_WINDOW_DEPTH_STEP;
        }
        xglMatrixTrans(cardMatrix, base, position);
        if (i == yamaCount - count + selectedOffset) {
            xglMatrixScale(cardMatrix, cardMatrix, selectedScale);
            /* The mode argument is set after xglMatrixScale, inside the loop, and is
             * CARD_YAMA_SELECTED_MODE only when style is set. */
            effect = 0;
            if (style != 0) {
                effect = CARD_YAMA_SELECTED_MODE;
            }
            CardDisp(&cardMatrix, side->deck[i], 1, effect);
            if (style == 0 && (flags & CARD_YAMA_MARK_FLAG) != 0) {
                position[0] = 0.0f;
                position[1] = CARD_YAMA_MARK_Y;
                position[2] = CARD_YAMA_MARK_Z;
                position[3] = 1.0f;
                xglMatrixUnit(markMatrix);
                xglMatrixTrans(markMatrix, markMatrix, position);
                CardDisp(&markMatrix, side->deck[i], 1, CARD_YAMA_SELECTED_MODE);
            }
        } else {
            xglMatrixScale(cardMatrix, cardMatrix, normalScale);
            CardDispCard(&cardMatrix, side->deck[i], 1);
        }
    }
    /* The base matrix is copied with QuadwordCopyMatrix (quadword.h). */
    QuadwordCopyMatrix(cardMatrix, base);
    /* A single card is moved to the one-card position and scaled as the selected card. */
    if (count == 1) {
        position[0] = CARD_WINDOW_SINGLE_X;
        position[1] = position[2] = 0.0f;
        position[3] = 1.0f;
        selectedScale[0] = CARD_HAND_SELECTED_SCALE;
        selectedScale[1] = 1.0f;
        selectedScale[2] = 1.0f;
        selectedScale[3] = 1.0f;
        xglMatrixTrans(cardMatrix, cardMatrix, position);
        xglMatrixScale(cardMatrix, cardMatrix, selectedScale);
    }
    nmlModelSetTexture(CARD_HAND_TEXTURE);
    nmlModelSetPlace(&cardMatrix);
    nmlModelEntryCard(CARD_YAMA_MODEL_BASE);
    position[0] = CARD_WINDOW_PILE_X;
    position[1] = CARD_WINDOW_PILE_Y;
    position[2] = CARD_WINDOW_BASE_DEPTH;
    position[3] = 1.0f;
    selectedScale[0] = CARD_HAND_SELECTED_SCALE;
    selectedScale[1] = CARD_WINDOW_PILE_SCALE_Y;
    selectedScale[2] = 1.0f;
    selectedScale[3] = 1.0f;
    xglMatrixTrans(cardMatrix, base, position);
    xglMatrixScale(cardMatrix, cardMatrix, selectedScale);
    nmlModelSetTexture(CARD_HAND_TEXTURE);
    nmlModelSetPlace(&cardMatrix);
    nmlModelEntryCard(CARD_YAMA_MODEL_PILE);
    xglFontPrint(CARD_WINDOW_CAPTION_X, CARD_WINDOW_CAPTION_Y, CARD_WINDOW_CAPTION_COLOR, cardDeckCaption);
}

/*
 * Draws the junk pile of one side. The junk cards are spaced evenly
 * along the window edge win0pos like a hand, each CARD_WINDOW_DEPTH_STEP further back
 * than the one before it in reverse order (the first card is deepest); the card at
 * `selected` is placed (count + 1) steps back and drawn at CARD_HAND_SELECTED_SCALE.
 * The pile marker model is drawn afterwards at CARD_WINDOW_PILE_X/Y with a flattened
 * scale (CARD_WINDOW_PILE_SCALE_Y) and the pile caption cardJunkCaption is printed.
 */
void CGCPJunkDispSub(CardPlaySide *side, int selected)
{
    Matrix4 base;
    Matrix4 cardMatrix;
    float position[4];
    float normalScale[4];
    float selectedScale[4];
    float spacing;
    int count;
    int i;

    count = CardPlayJunkCnt(side);
    spacing = (win0pos[1][0] - win0pos[0][0]) / (float)(count - 1);
    xglMatrixUnit(base);
    position[0] = 0.0f;
    position[1] = 0.0f;
    position[2] = CARD_WINDOW_BASE_DEPTH;
    position[3] = 1.0f;
    xglMatrixTrans(base, base, position);
    position[1] = win0pos[0][1];
    position[2] = win0pos[0][2];
    position[3] = 1.0f;
    normalScale[0] = CARD_HAND_SCALE;
    normalScale[1] = CARD_HAND_SCALE;
    normalScale[2] = 1.0f;
    normalScale[3] = 1.0f;
    selectedScale[0] = CARD_HAND_SELECTED_SCALE;
    selectedScale[1] = CARD_HAND_SELECTED_SCALE;
    selectedScale[2] = 1.0f;
    selectedScale[3] = 1.0f;
    for (i = 0; i < count; i++) {
        position[0] = win0pos[0][0] + spacing * (float)i;
        position[2] = win0pos[0][2] + (float)(count - i) * CARD_WINDOW_DEPTH_STEP;
        if (i == selected) {
            position[2] = win0pos[0][2] + (float)(count + 1) * CARD_WINDOW_DEPTH_STEP;
        }
        xglMatrixTrans(cardMatrix, base, position);
        if (i == selected) {
            xglMatrixScale(cardMatrix, cardMatrix, selectedScale);
            CardDisp(&cardMatrix, side->junk[i], 1, 0);
        } else {
            xglMatrixScale(cardMatrix, cardMatrix, normalScale);
            CardDispCard(&cardMatrix, side->junk[i], 1);
        }
    }
    nmlModelSetTexture(CARD_HAND_TEXTURE);
    nmlModelSetPlace(&base);
    nmlModelEntryCard(CARD_HAND_MODEL);
    position[0] = CARD_WINDOW_PILE_X;
    position[1] = CARD_WINDOW_PILE_Y;
    position[2] = CARD_WINDOW_BASE_DEPTH;
    position[3] = 1.0f;
    selectedScale[0] = CARD_HAND_SELECTED_SCALE;
    selectedScale[1] = CARD_WINDOW_PILE_SCALE_Y;
    selectedScale[2] = 1.0f;
    selectedScale[3] = 1.0f;
    xglMatrixTrans(cardMatrix, base, position);
    xglMatrixScale(cardMatrix, cardMatrix, selectedScale);
    nmlModelSetTexture(CARD_HAND_TEXTURE);
    nmlModelSetPlace(&cardMatrix);
    nmlModelEntryCard(CARD_JUNK_PILE_MODEL);
    xglFontPrint(CARD_WINDOW_CAPTION_X, CARD_WINDOW_CAPTION_Y, CARD_WINDOW_CAPTION_COLOR, cardJunkCaption);
}

/* Counts the operation slots of both players that hold opId. */
s32 CardPlayCntAllOpe(CardGameWork *work, s16 opId)
{
    s16 *playerOp;
    s16 *enemyOp;
    s16 want;
    s32 count;
    s32 remaining;

    want = opId;
    count = 0;
    enemyOp = work->enemy.operations;
    playerOp = work->player.operations;
    remaining = CARD_OPERATION_SLOTS - 1;
    do {
        s16 playerId = *playerOp;
        s16 enemyId;

        playerOp += 1;
        enemyId = *enemyOp;
        enemyOp += 1;
        if ((want ^ playerId) == 0) {
            count = count + 1;
        }
        remaining -= 1;
        if ((want ^ enemyId) == 0) {
            count = count + 1;
        }
    } while (remaining >= 0);
    return count;
}

/* Counts the operation slots of one side that hold opId. */
s32 CardPlayCntOpe(CardPlaySide *side, s16 opId)
{
    s16 *op;
    s16 id;
    s16 want;
    s32 count;
    s32 remaining;

    want = opId;
    op = side->operations;
    count = 0;
    remaining = CARD_OPERATION_SLOTS - 1;
    do {
        id = *op;
        op += 1;
        remaining -= 1;
        if ((want ^ id) == 0) {
            count = count + 1;
        }
    } while (remaining >= 0);
    return count;
}

/*
 * Counts the base-layer card ids of the side's disposal slots that equal cardId,
 * sibling to CardPlayCntBtl's battle slots.
 */
int CardPlayCntDsp(CardPlaySide *side, s16 cardId)
{
    s16 *cardIdField;
    s16 id;
    s16 want;
    s32 count;
    s32 remaining;

    want = cardId;
    cardIdField = &side->disposal[0].battle.layers[0].cardId;
    count = 0;
    remaining = CARD_BOARD_SLOTS - 1;
    do {
        id = *cardIdField;
        cardIdField += CARD_SLOT_STRIDE_HALFWORDS;
        remaining -= 1;
        if ((want ^ id) == 0) {
            count = count + 1;
        }
    } while (remaining >= 0);
    return count;
}

/* Same count as CardPlayCntDsp, over the battle slots. */
int CardPlayCntBtl(CardPlaySide *side, s16 cardId)
{
    s16 *cardIdField;
    s16 id;
    s16 want;
    s32 count;
    s32 remaining;

    want = cardId;
    cardIdField = &side->battle[0].battle.layers[0].cardId;
    count = 0;
    remaining = CARD_BOARD_SLOTS - 1;
    do {
        id = *cardIdField;
        cardIdField += CARD_SLOT_STRIDE_HALFWORDS;
        remaining -= 1;
        if ((want ^ id) == 0) {
            count = count + 1;
        }
    } while (remaining >= 0);
    return count;
}

/*
 * Count the light cards on the base layer of a side's four battle slots and
 * four disposal slots: a card counts when its definition flags carry
 * CARD_ATTR_HUMAN or CARD_ATTR_REALIAN. An empty slot holds a negative card id
 * and is skipped. Each loop walks the card id field of its slots, one
 * CardBoardSlot apart.
 */
s32 CardPlayCntLight(CardGameWork *work, CardPlaySide *side)
{
    s32 count;
    int slot;
    s16 *battleCardId;
    s16 *disposalCardId;
    s16 cardId;

    count = 0;
    battleCardId = &side->battle[0].battle.layers[0].cardId;
    for (slot = 0; slot < CARD_BOARD_SLOTS; slot++) {
        cardId = *battleCardId;
        battleCardId += sizeof(CardBoardSlot) / sizeof(s16);
        if (cardId >= 0 && ((work->definitions[cardId].flags & CARD_ATTR_HUMAN) || (work->definitions[cardId].flags & CARD_ATTR_REALIAN))) {
            count++;
        }
    }
    disposalCardId = &side->disposal[0].battle.layers[0].cardId;
    for (slot = 0; slot < CARD_BOARD_SLOTS; slot++) {
        cardId = *disposalCardId;
        disposalCardId += sizeof(CardBoardSlot) / sizeof(s16);
        if (cardId >= 0 && ((work->definitions[cardId].flags & CARD_ATTR_HUMAN) || (work->definitions[cardId].flags & CARD_ATTR_REALIAN))) {
            count++;
        }
    }
    return count;
}

/*
 * Count the mech cards (definition flags CARD_ATTR_MECH) that sit on the base
 * layer of a side's four battle slots and four disposal slots. The slots are
 * sizeof(CardBoardSlot) bytes apart. An empty slot holds a negative id and is
 * skipped.
 */
s32 CardPlayCntHeavy(CardGameWork *work, CardPlaySide *side)
{
    s32 count;
    int slot;
    s16 *cardIdField;
    s16 cardId;

    count = 0;
    cardIdField = &side->battle[0].battle.layers[0].cardId;
    for (slot = 0; slot < CARD_BOARD_SLOTS; slot++) {
        cardId = *cardIdField;
        cardIdField += sizeof(CardBoardSlot) / sizeof(s16);
        if (cardId >= 0 && (work->definitions[cardId].flags & CARD_ATTR_MECH)) {
            count++;
        }
    }
    cardIdField = &side->disposal[0].battle.layers[0].cardId;
    for (slot = 0; slot < CARD_BOARD_SLOTS; slot++) {
        cardId = *cardIdField;
        cardIdField += sizeof(CardBoardSlot) / sizeof(s16);
        if (cardId >= 0 && (work->definitions[cardId].flags & CARD_ATTR_MECH)) {
            count++;
        }
    }
    return count;
}

/*
 * Counts the battle and disposal slots whose base card (layers[0]) has the
 * CARD_ATTR_MECH flag and whose base card passes CGPWeaponEquipChk against
 * cardId. The four battle slots are checked first, then the four disposal slots.
 */
s32 CardEquipCntHeavy(CardGameWork *work, CardPlaySide *side, s32 cardId)
{
    s32 count;
    s32 slot;
    s16 *baseCardPtr;
    s32 baseCardId;

    count = 0;
    for (slot = 0; slot < CARD_BOARD_SLOTS; slot++) {
        baseCardPtr = &side->battle[slot].battle.layers[0].cardId;
        baseCardId = *baseCardPtr;
        if (baseCardId >= 0 && (work->definitions[baseCardId].flags & CARD_ATTR_MECH)) {
            if (CGPWeaponEquipChk(work, cardId, baseCardId) != 0) {
                count++;
            }
        }
    }
    for (slot = 0; slot < CARD_BOARD_SLOTS; slot++) {
        baseCardPtr = &side->disposal[slot].battle.layers[0].cardId;
        baseCardId = *baseCardPtr;
        if (baseCardId >= 0 && (work->definitions[baseCardId].flags & CARD_ATTR_MECH)) {
            if (CGPWeaponEquipChk(work, cardId, baseCardId) != 0) {
                count++;
            }
        }
    }
    return count;
}

/* Counts the copies of cardId in both sides' disposal and battle slots. */
int CardPlayCntFld(CardGameWork *work, s16 cardId) {
    CardPlaySide *playerSide;
    CardPlaySide *enemySide;
    int count;

    playerSide = &work->player;
    enemySide = &work->enemy;
    count = CardPlayCntDsp(playerSide, cardId);
    count = count + CardPlayCntBtl(playerSide, cardId);
    count = count + CardPlayCntDsp(enemySide, cardId);
    return count + CardPlayCntBtl(enemySide, cardId);
}

/* Defined here: CursorTbl and the data declared in card_fread.h. */
static float *CursorTbl[16] = {
    DisposePos1P[0], BattlePos1P[0], BattlePos1P[1], OperationPos1P[0],
    DisposePos1P[1], BattlePos1P[0], BattlePos1P[1], OperationPos1P[1],
    DisposePos1P[2], BattlePos1P[2], BattlePos1P[3], OperationPos1P[2],
    DisposePos1P[3], BattlePos1P[2], BattlePos1P[3], OperationPos1P[3]
};

static float CardHpPos01[3] = { 1.3422000408172607f, 0.59920001029968262f, 0.0099999997764825821f };

static float CardHpPos10[3] = { 1.2441999912261963f, 0.59939998388290405f, 0.0099999997764825821f };

static float CardPowPos01[3] = { 1.3422000408172607f, 0.44060000777244568f, 0.0099999997764825821f };

static float CardPowPos10[3] = { 1.2441999912261963f, 0.44060000777244568f, 0.0099999997764825821f };

static float CardStnbyPos[3] = { 1.3422000408172607f, -0.64399999380111694f, 0.0099999997764825821f };

static float CardAttriPos[3] = { 1.2788000106811523f, 0.84460002183914185f, 0.0099999997764825821f };

static float CardAttakPos[3] = { 1.1656999588012695f, 0.25920000672340393f, 0.0099999997764825821f };

float CardCondPos[4][3] = { { 1.055400013923645f, -0.11680000275373459f, 0.0099999997764825821f }, { 1.2678999900817871f, -0.15710000693798065f, 0.0099999997764825821f }, { 1.055400013923645f, -0.39550000429153442f, 0.0099999997764825821f }, { 1.2680000066757202f, -0.43560001254081726f, 0.0099999997764825821f } };

static int oldno_0 = 65535;

char DeckListStr[10][16] = {
    "Deck 1",
    "Deck 2",
    "Deck 3",
    "Deck 4",
    "Deck 5",
    "Deck 6",
    "Deck 7",
    "Deck 8",
    "Deck 9",
    "Deck 0"
};

float CPSInfoPos[4][2] = { { -1.5740000009536743f, -0.3970000147819519f }, { 1.5740000009536743f, -0.3970000147819519f }, { -1.5740000009536743f, -1.1799999475479126f }, { 1.5740000009536743f, -1.1799999475479126f } };

static float CardStackPos[3] = { 0.67239999771118164f, -0.37790000438690186f, 0.019999999552965164f };

static float DisposeHP10pos[4][2] = { { -2.4059998989105225f, -0.33190000057220459f }, { -2.4059998989105225f, -0.69900000095367432f }, { -2.4059998989105225f, -1.0789999961853027f }, { -2.4059998989105225f, -1.4589999914169312f } };

static float DisposeHP01pos[4][3] = { { -2.3190000057220459f, -0.33190000057220459f, 0.0f }, { -2.3190000057220459f, -0.69900000095367432f, 0.0f }, { -2.3190000057220459f, -1.0789999961853027f, 0.0f }, { -2.3190000057220459f, -1.4589999914169312f, 0.0f } };

static float win0pos[2][3] = { { -1.891200065612793f, 0.0f, 0.0010000000474974513f }, { 1.891200065612793f, 0.0f, 0.0010000000474974513f } };

static float NewWin0pos[4][3] = { { -2.3239998817443848f, 0.0f, 0.0010000000474974513f }, { 0.69999998807907104f, 0.0f, 0.0010000000474974513f }, { -0.69999998807907104f, 0.0f, 0.0010000000474974513f }, { 2.3239998817443848f, 0.0f, 0.0010000000474974513f } };

u16 CardCommandPlay[CARD_ID_COUNT] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 58, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

const char cardDeckHeaderFontCode[8] = "\013\031\000\r";

const char cardSelectedRowFontCode[8] = "\014\200";

const char cardOtherRowFontCode[8] = "\013\r";

const char cardRowPrefixFontCode[8] = "\031\003";

const char cardCopyCountFontCode[8] = "\014@@";

const char cardCopyCountFormat[8] = "%d";

const char cardWindowFooterFontCode[8] = "\013\031";

const char cardTextLineFontCode[16] = "\013\r\000\031\003  \025\002\001\n";

const char cardEnemyListHeaderFontCode[8] = "\031\000\r";

const char cardUnownedRowText[16] = "\241\251\241\251\241\251\241\251\241\251";

const char cardWindowHeaderFontCode[8] = "\031\003\r";

const char cardDeckCaption[16] = "\r\000\031\003Deck";

const char cardJunkCaption[16] = "\r\000\031\003Junk";
