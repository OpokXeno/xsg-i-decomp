/*
 * OV10 original TU 2: 0x00a08670..0x00a12a90 (101 functions)
 */
#include "common.h"
#include "quadword.h"
#include "shared.h"
#include "ov10/cgp.h"
#include "main/xgl_font.h"
#include "card_fread.h"

/* The card-model fade state in CardGameWork (+0x7100..+0x71A0, ov10/cgp.h):
 * four model slots, of whose four color channels the fades touch 0..2. */
#define CARD_MODEL_COLOR_CHANNELS 3

/*
 * Reads a card-data file ("data\carddata\" + filename) into the buffer at
 * address. The prefix copy uses __builtin_strcpy: the compiler expands it
 * inline from the string literal in .rodata (ld/lw/lhu/lbu block move), which
 * plain strcpy would not do under -fno-builtin.
 */
int CardFread(u32 address, char *filename)
{
    char path[256];

    __builtin_strcpy(path, "data\\carddata\\");
    strcat(path, filename);
    return xglCdReadFile(path, (void *) address, 0, 0);
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
 * Mirror of CardFadeOut for the slot colors (asm 0x00a08818): the timer counts
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
        if (definitions->kind == 1 && !(definitions->flags & flagMask)) {
            continue;
        }
        entries->cardId = i;
        entries++;
        count++;
    }
    return count;
}

/* Compact deck-build list CardMakeDeckProc/CardLoadDeckProc/CardSaveDeckProc
 * edit: a card-type group count followed by one 4-byte group record per
 * group: the card id (CardGetDeckCardCnt compares it, lh at +0x14 + 4*i)
 * and its selected-copy count (lh at +0x16 + 4*i in both counters). */
typedef struct CardDeckGroupEntry {
    s16 cardId;                 /* +0x00 */
    s16 copyCount;              /* +0x02 */
} CardDeckGroupEntry;

typedef struct CardDeckGroupList {
    u8 unmodeled_00[0x05];
    s8 pendingFlag;             /* +0x05: counted down by CardDelDeck (lb/sb at 0x00a08cc8..0x00a08cdc) */
    u8 unmodeled_06[0x06];
    s16 pendingCount;           /* +0x0C: counted down by CardDelDeck (lh/sh at 0x00a08ca0..0x00a08cc4) */
    s16 groupTotal;             /* +0x0E: total group count, lowered with groupCount (lhu/sh at 0x00a08c9c, 0x00a08cb0) */
    u8 unmodeled_10[0x02];
    s16 groupCount;             /* +0x12 */
    CardDeckGroupEntry groups[1]; /* +0x14: groupCount entries */
} CardDeckGroupList;

/*
 * Expand the run-length deck list into the flat deck: each group i writes its
 * cardId copyCount times, in group order (asm 0x00a088f0).
 */
void CardCopyDeckWork(s16 *deck, CardDeckGroupList *list)
{
    int group;
    int copy;
    int deckIndex;

    deckIndex = 0;
    for (group = 0; group < list->groupCount; group++) {
        for (copy = 0; copy < list->groups[group].copyCount; copy++) {
            deck[deckIndex++] = list->groups[group].cardId;
        }
    }
}

typedef int s32;

s32 CardGetDeckAllCnt(CardDeckGroupList *deck)
{
    s32 total;
    int i;

    total = 0;
    for (i = 0; i < deck->groupCount; i++) {
        total += deck->groups[i].copyCount;
    }
    return total;
}

s32 CardGetDeckCardCnt(s16 cardId, CardDeckGroupList *deck)
{
    s32 total;
    int i;

    total = 0;
    for (i = 0; i < deck->groupCount; i++) {
        if (deck->groups[i].cardId == cardId) {
            total += deck->groups[i].copyCount;
        }
    }
    return total;
}

/*
 * Sort the compact deck group list by ascending card id: a selection sort
 * that swaps whole group records, both halfwords of each (asm 0x00a08a18).
 * The card ids are compared as unsigned halfwords. CardDelDeck calls it with
 * the group count before it decrements the count (lh at 0x00a08c94).
 */
void CardCleanUpDeck(CardDeckGroupEntry *groups, s16 groupCount)
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
            s16 copyCount = groups[i].copyCount;

            groups[i].cardId = groups[least].cardId;
            groups[i].copyCount = groups[least].copyCount;
            groups[least].cardId = cardId;
            groups[least].copyCount = copyCount;
        }
    }
}

/*
 * Adds one copy of a card to the compact deck group list (asm 0x00a08ad0).
 * A group that already holds the card gains one copy while it has fewer than
 * three; otherwise a new group with one copy is appended. The list is then
 * sorted with CardCleanUpDeck, and the function returns the position of the
 * card's group after the sort (the group count when no group matches).
 */
s32 CardAddDeck(CardDeckGroupList *deck, s16 cardId)
{
    s32 index;
    s32 found;

    found = 0;
    for (index = 0; index < deck->groupCount; index++) {
        if (deck->groups[index].cardId == cardId) {
            found = 1;
            break;
        }
    }
    if (found) {
        if (deck->groups[index].copyCount < CARD_DECK_MAX_COPIES) {
            deck->groups[index].copyCount++;
        }
    } else {
        deck->groups[deck->groupCount].cardId = cardId;
        deck->groups[deck->groupCount].copyCount = 1;
        deck->groupCount++;
    }
    CardCleanUpDeck(deck->groups, deck->groupCount);
    for (index = 0; index < deck->groupCount; index++) {
        if (deck->groups[index].cardId == cardId) {
            break;
        }
    }
    return index;
}

/*
 * Removes one copy of cardId from the compact deck group list (asm
 * 0x00a08c28). A group with two or more copies keeps one fewer. A group with
 * one copy is dropped: its card id becomes -1 and CardCleanUpDeck sorts it to
 * the end, then the group count and the +0x0E count go down. The +0x0C and +0x05
 * counters then go down once each while nonzero. Returns 0 only when the group was
 * dropped and both counters were zero; otherwise 1.
 */
int CardDelDeck(CardDeckGroupList *deck, s16 cardId)
{
    s32 group;

    for (group = 0; group < deck->groupCount; group++) {
        if (deck->groups[group].cardId == cardId) {
            if (deck->groups[group].copyCount < 2) {
                deck->groups[group].cardId = -1;
                deck->groups[group].copyCount = 0;
                CardCleanUpDeck(deck->groups, deck->groupCount);
                deck->groupCount -= 1;
                deck->groupTotal -= 1;
                if (deck->pendingCount != 0) {
                    deck->pendingCount -= 1;
                    break;
                }
                if (deck->pendingFlag != 0) {
                    deck->pendingFlag -= 1;
                    break;
                }
                return 0;
            } else {
                deck->groups[group].copyCount -= 1;
                break;
            }
        }
    }
    return 1;
}

#define CARD_TEXT_SECOND_CHAR 1  /* byte after the backslash that CardNameConvert tests */
#define CARD_TEXT_THIRD_CHAR 2   /* byte after that: a backslash gives "#73", a '%' gives "#23" */
#define CARD_BYTE_BITS 8         /* bits of a byte: the colour value is stored high byte first */
#define CARD_COLOR_CODE_HIGH 3   /* colorCode[3]: high byte of the 16-bit colour value, after the 3-byte prefix 0x15 0x03 0x03 */
#define CARD_COLOR_CODE_LOW 4    /* colorCode[4]: low byte of the colour value */
#define CARD_COLOR_CODE_END 5    /* colorCode[5]: terminating zero */
#define CARD_ROW_PREFIX_BYTES 2  /* escape bytes 0x19 0x03 in front of the text of a list row */

/* Forward declaration: defined further down, in the original function order of this TU. */
int CDTSGetStrLen(unsigned char *text);

/*
 * Rewrites the escape codes of a card name in place (asm 0x00a08d18). The
 * loop covers the first CDTSGetStrLen(text) bytes and stops at a zero byte.
 * '$' becomes a newline, '#' an apostrophe and '@' a comma. A backslash that
 * is followed by a second backslash becomes "#73" when a third backslash
 * follows the pair and "#23" when a '%' follows it. The case order '@', '#',
 * '$', '\\' follows the compare chain of the original (asm
 * 0x00a08d80..0x00a08db4).
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
 * Draws the deck window with card names (asm 0x00a08e38): the header string, then
 * for each of the windowRows rows below rowCount that holds a card (cardId >= 0) the
 * cursor marker, the name text (textLength characters of two bytes, read from the card
 * data buffer through the definition's nameOffset and run through CardNameConvert), and
 * the copy count printed DECK_DISP_COUNT_GAP past the name. Rows are
 * DECK_DISP_ROW_HEIGHT pixels apart. cursorLimit is set to rowCount whether or not any
 * row is shown, then the footer is printed. `row` is initialised at its declaration
 * (the original sets $s2 before the first xglFontPrint, 0x00a08e4c).
 */
void CardDeckDisp(CardDeckListView *deck, CardDefinitionName *definitions)
{
    unsigned char nameText[32];
    char countText[16];
    s32 row = 0;
    s32 cardId;
    s32 i;
    char *name;
    CardDefinitionName *definition;

    xglFontPrint(deck->originX, deck->originY, DECK_LIST_TEXT_COLOR, D_00A4D140);
    if (deck->windowRows > 0) {
        for (; row < deck->windowRows; row++) {
            if (row < deck->rowCount) {
                cardId = deck->entries[row + deck->scroll].cardId;
                if (cardId >= 0) {
                    definition = &definitions[cardId];
                    name = (char *)(CARD_DATA_BUFFER + definition->nameOffset);
                    if (row == deck->cursor) {
                        xglFontPrint(deck->originX, deck->originY, DECK_LIST_TEXT_COLOR, D_00A4D148);
                    } else {
                        xglFontPrint(deck->originX, deck->originY, DECK_LIST_TEXT_COLOR, D_00A4D150);
                    }
                    for (i = 0; i < deck->textLength * CARD_TEXT_CHAR_BYTES; i++) {
                        nameText[i] = name[i];
                    }
                    nameText[i] = 0;
                    CardNameConvert(nameText);
                    xglFontPrint(0, 0, 0, D_00A4D158);
                    xglFontPrint(deck->originX, deck->originY + row * DECK_DISP_ROW_HEIGHT,
                                 DECK_LIST_TEXT_COLOR, (char *)nameText);
                    if (row == deck->cursor) {
                        xglFontPrint(deck->originX, deck->originY, DECK_LIST_TEXT_COLOR, D_00A4D148);
                    } else {
                        xglFontPrint(deck->originX, deck->originY, DECK_LIST_TEXT_COLOR, D_00A4D160);
                    }
                    sprintf(countText, D_00A4D168, deck->entries[row + deck->scroll].copies);
                    xglFontPrint(deck->originX + deck->textLength * DECK_DISP_CHAR_WIDTH + DECK_DISP_COUNT_GAP,
                                 deck->originY + row * DECK_DISP_ROW_HEIGHT, DECK_LIST_TEXT_COLOR, countText);
                }
            }
        }
    }
    deck->cursorLimit = deck->rowCount;
    xglFontPrint(deck->originX, deck->originY, DECK_LIST_TEXT_COLOR, D_00A4D170);
}

/*
 * Counts the display characters of a text string (0x00A090B8, 0x88 bytes).
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
                /* Skip the code byte and its codeSize parameter bytes (0x00A090F8 loop). */
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
 * Prints one line of card text at (x, y) with colour index colorIndex (asm
 * 0x00a09140, 452 bytes). First two font strings are printed: the constant
 * escape D_00A4D180 and colorCode, the escape 0x15 0x03 0x03 followed by the
 * 16-bit big-endian value colorIndex * 21 (0x00a09144..0x00a091DC). The text
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

    xglFontPrint(0, 0, 0, D_00A4D180);
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
 * Prints one card text line at (x, y) with the default colour 0xFFF0 (asm
 * 0x00a09308, 420 bytes). It is CardDispTxtSub without the colour argument:
 * the font escape D_00A4D180 is printed, then the three-byte colour code
 * 0x15 0x03 0x03 (copied from .rodata 0x00a4d190 at 0x00a09328..0x00a09344).
 * The text is then rewritten in place with the CardNameConvert rules: '$' becomes
 * a newline, '#' an apostrophe and '@' a comma, and a backslash pair followed by
 * a third backslash or a '%' becomes "#73" or "#23".
 */
void CardDispTxtSub2(u16 x, u16 y, unsigned char *text)
{
    char colorCode[6] = "\x15\x03\x03\x00\x00";
    int length;
    int i;

    xglFontPrint(0, 0, 0, D_00A4D180);
    xglFontPrint(0, 0, 0, colorCode);

    length = CDTSGetStrLen(text);
    for (i = 0; i < length && text[i] != 0; i++) {
        /*
         * The original tests '$' (0x00a093d8), '#' (0x00a093e8), '@' (0x00a093f8)
         * and '\\' (0x00a09400) in that order; the labels are listed '@', '#',
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
 * Card range, texture and model addresses shared by CardDispLight (asm 0x00a094b0),
 * CardDispCard (asm 0x00a0a068) and CardDisp (asm 0x00a0a2f0).
 */
#define CARD_DISP_ID_COUNT 0x94                     /* sltiu 0x94, 0x00a0a0a8 */
#define CARD_MASTER_TABLE ((CardDefinition *)0x01000010) /* 0x00a0a0d8 */
#define CARD_NAME_TABLE ((CardDefinitionName *)0x01000010) /* the same table seen through its name fields */
#define CARD_TEXTURE_BASE 0x0100C000                /* 0x00a0a0e4 */
#define CARD_TEXTURE_STRIDE 0x0000BAB0              /* 0x00a0a0b0 */
#define CARD_TEXTURE(cardId) ((const char *)(CARD_TEXTURE_BASE + (cardId) * CARD_TEXTURE_STRIDE)) /* texture of a card face */
#define CARD_DISP_ENABLED 0x01                      /* bit 0 of the flags: draw at all (`andi 0x1`, 0x00a0a350) */
#define CARD_DISP_OVERLAY 0x02                      /* andi 0x2, 0x00a0a2a0 */
#define CARD_DISP_LIGHT_MIDDLE 0x40                 /* andi 0x40, 0x00a0a0f8 */
#define CARD_DISP_LIGHT_CN 0x80                     /* andi 0x80, 0x00a0a120 */
#define CARD_MODEL_FRAME 0x016F8500                 /* 0x00a0a150 */
#define CARD_MODEL_FACE 0x016F8100                  /* 0x00a0a200 */
#define CARD_MODEL_DEFAULT 0x01701F80               /* 0x00a0a1f4 */
#define CARD_MODEL_KIND1 0x016F8900                 /* 0x00a0a230 */
#define CARD_MODEL_KIND2 0x016FD440                 /* ori 0x00a0a240 */
#define CARD_MODEL_OVERLAY 0x01706AC0               /* 0x00a0a2bc */
#define CARD_OVERLAY_TEXTURE ((const char *)0x016E0000) /* 0x00a0a250 */

/*
 * CardDispLight light bits (the fourth argument): each bit applies the card light
 * (CCColor, asDir) to one part before its model is entered.
 */
#define CARD_LIGHT_FRAME 0x01       /* frame, overlay and back models (andi 0x1, 0x00a09544) */
#define CARD_LIGHT_HIT_POINTS 0x02  /* hit point digits (andi 0x2, 0x00a09668) */
#define CARD_LIGHT_POWER 0x04       /* power digits (andi 0x4, 0x00a09834) */
#define CARD_LIGHT_RESERVE 0x08     /* reserve cost digit (andi 0x8, 0x00a09a78) */
#define CARD_LIGHT_ATTRIBUTE 0x10   /* attribute icon (andi 0x10, 0x00a09bc0) */
#define CARD_LIGHT_NEED 0x20        /* requirement icons (andi 0x20, 0x00a09d34) */
#define CARD_LIGHT_ATTACK 0x40      /* attack type icon (andi 0x40, 0x00a09e90) */
#define CARD_LIGHT_ELEMENT 0x80     /* element icon (andi 0x80, 0x00a09f14) */

#define CARD_BACK_TEXTURE CARD_FRAME_TEXTURE /* lui 0x00a0a000 */
#define CARD_BACK_MODEL 0x01713600                      /* lui 0x00a0a010 */

/* Hit points are shown only when no flag above bit 4 is set (andi 0xFFE0, 0x00a09628). */
#define CARD_FLAGS_NO_HIT_POINTS 0xFFE0

/* Icon textures and model bases of CardDispLight. */
#define CARD_ICON_TEXTURE ((const char *)0x01C91000)    /* lui 0x00a096fc */
#define CARD_ATTACK_TEXTURE ((const char *)0x01C94050)  /* lui 0x00a09e08 */
#define CARD_ELEMENT_TEXTURE ((const char *)0x01C9BB70) /* lui 0x00a09f1c */
#define CARD_DIGIT_MODEL 0x01C80000         /* digit models 0..9 (lui 0x00a09674 as a float) */
#define CARD_ATTRIBUTE_MODEL 0x01C87400     /* attribute icons (CardDispLight loads it as the float 0x4BE43A00, lui 0x00a09b3c) */
#define CARD_NEED_MODEL 0x01C85000          /* requirement icons (float 0x4BE42800, lui 0x00a09c98) */
#define CARD_ATTACK_MODEL 0x01C89400        /* attack type icons (float 0x4BE44A00, lui 0x00a09e0c) */
#define CARD_MODEL_STRIDE 1024              /* models of one icon family are 0x400 apart */
#define CARD_MODEL_ELEMENT_50 0x01C90730    /* lui 0x00a09ec8 */
#define CARD_MODEL_ELEMENT_52 0x01C90220    /* lui 0x00a09ee4 */
#define CARD_MODEL_ELEMENT_55 0x01C8FD10    /* lui 0x00a09ef4 */
#define CARD_MODEL_ELEMENT_NONE 0x01C8F800  /* lui 0x00a09ee0 */
#define CARD_ATTR_BIT4 0x10                 /* attribute icon flag without a name: jump table of the icon switch (sltiu 0x40, 0x00a09ac8) */
#define CARD_ATTR_BIT5 0x20                 /* as CARD_ATTR_BIT4 */
#define CARD_ATTR_BIT6 0x40                 /* as CARD_ATTR_BIT4 */
#define CARD_ELEMENT_50 0x50                /* element code (`addiu $2,$0,0x50`, 0x00a09ecc) */
#define CARD_ELEMENT_52 0x52                /* element code (`addiu $2,$0,0x52`, 0x00a09ee8) */
#define CARD_ELEMENT_55 0x55                /* element code (`addiu $2,$0,0x55`, 0x00a09ef8) */
#define CARD_ELEMENT_4F 0x4F                /* compared in the original switch but never matches (`slti $3,0x51` bound, 0x00a09ed8) */
#define CARD_KIND_UNUSED_3 3                /* kind 3 is compared in the overlay switch but no card has it (`slti $3,0x3`, 0x00a09f6c) */
#define CARD_NEED_ICON_ANY 6                /* requirement icon index of CARD_NEED_ANY, 0x40C00000 (0x00a09c58) */
#define CARD_NEED_ICON_NONE 8               /* no requirement icon is drawn, 0x41000000 (0x00a09c68) */
#define CARD_ICON_INDEX_NONE (-1)           /* no attack icon is drawn, 0xBF800000 (0x00a09de0) */
#define CARD_FACE_X (-0.436f)               /* face model offset, 0xBEDF3B64 (0x00a09580) */
#define CARD_FACE_Y 0.847f                  /* 0x3F58D4FE (0x00a09590) */
#define CARD_FACE_Z 0.005f                  /* 0x3BA3D70A (0x00a0959c) */

/*
 * `number` is a float because the original does the icon index in the FPU:
 * digits are converted with `cvt.s.w` (0x00a09658), the fall-through cases add
 * 1.0 per step (`add.s`, 0x00a09afc..0x00a09b2c) and the model id is formed by
 * `mul.s`, `add.s` and `cvt.w.s` (0x00a09b4c..0x00a09b60), hence the casts to
 * unsigned int. CardDisp, the twin function, uses an int and has no such code.
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
    if (definition->kind == 1) {
        if ((definition->flags & CARD_FLAGS_NO_HIT_POINTS) == 0) {
            /* hit points: tens digit (only when non-zero), then units digit */
            number = definition->operationDuration / CARD_DIGIT_BASE;
            if (number != 0.0f) {
                model = (unsigned int)(number * CARD_MODEL_STRIDE + CARD_DIGIT_MODEL);
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
            model = (unsigned int)(number * CARD_MODEL_STRIDE + CARD_DIGIT_MODEL);
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
            model = (unsigned int)(number * CARD_MODEL_STRIDE + CARD_DIGIT_MODEL);
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
        model = (unsigned int)(number * CARD_MODEL_STRIDE + CARD_DIGIT_MODEL);
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
    model = (unsigned int)(number * CARD_MODEL_STRIDE + CARD_DIGIT_MODEL);
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
        case CARD_ATTR_BIT5:
        case CARD_ATTR_BIT6:
            number += 1.0f;
        case CARD_ATTR_BIT4:
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
        model = (unsigned int)(number * CARD_MODEL_STRIDE + CARD_ATTRIBUTE_MODEL);
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
        model = (unsigned int)(number * CARD_MODEL_STRIDE + CARD_NEED_MODEL);
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
        model = (unsigned int)(number * CARD_MODEL_STRIDE + CARD_ATTACK_MODEL);
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
    switch (definition->element) {
    case CARD_ELEMENT_50:
        model = CARD_MODEL_ELEMENT_50;
        break;
    case CARD_ELEMENT_52:
        model = CARD_MODEL_ELEMENT_52;
        break;
    case CARD_ELEMENT_55:
        model = CARD_MODEL_ELEMENT_55;
        break;
    case CARD_ELEMENT_4F: /* never matched: the chain tests 0x50, then `slti 0x51` (0x00a09ed8) */
    default:
        model = CARD_MODEL_ELEMENT_NONE;
        break;
    }
    nmlModelSetPlace(matrix);
    if (light & CARD_LIGHT_ELEMENT) {
        nmlModelSetLight(&CCColor, &asDir);
    }
    nmlModelSetTexture(CARD_ELEMENT_TEXTURE);
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
        case CARD_KIND_UNUSED_3: /* never matched: the chain tests 2, then `slti 0x3` (0x00a09f6c) */
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
 * Draws one card model without the selection flag (asm 0x00a0a068). The enabled flag
 * is sign-extended (`sll`/`sra` 16 on $6, 0x00a0a070..0x00a0a07c); no result is returned.
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
    if ((unsigned short)cardId >= CARD_DISP_ID_COUNT) { /* unsigned compare: `sltiu $2,$2,0x94`, 0x00a0a0a8 */
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
 * CardDisp (asm 0x00a0a2f0): the second argument bits are CARD_DISP_ENABLED (draw the
 * card face and its icons) and CARD_DISP_OVERLAY; the last argument shows the card text
 * when non-zero and CARD_DISP_MARKER adds the selection marker. A card that is not owned
 * (owned == 0) is drawn as CARD_UNOWNED_MODEL. The last two flags are sign-extended
 * halfwords (`sll`/`sra 16` on $6 and $7, 0x00a0a2f8..0x00a0a314); no result is returned.
 */
#define CARD_DISP_MARKER 0x04                       /* andi 0x4, 0x00a0aaf0 */
#define CARD_UNOWNED_TEXTURE ((const char *)0x01C74000) /* lui 0x00a0a93c */
#define CARD_UNOWNED_MODEL 0x01C73000               /* lui 0x00a0a948 */
#define CARD_TEXT_COLOR 0xFFF0                      /* ori 0xFFF0, 0x00a0aa18 */
#define CARD_DISP_FACE_X (-0.41599998f)             /* face model offset of CardDisp, 0xBED4FDF3 (0x00a0a398); -0.416f encodes 0xBED4FDF4 */
#define CARD_DISP_MARKER_Y (-1.2f)                  /* selected marker offset, 0xBF99999A (0x00a0aafc) */
#define CARD_DISP_MARKER_Z 0.8f                     /* 0x3F4CCCCD (0x00a0ab0c) */
#define CARD_DISP_MARKER_SCALE_X 0.88f              /* 0x3F6147AE (0x00a0ab48) */
#define CARD_DISP_MARKER_SCALE_Y 0.92f              /* 0x3F6B851F (0x00a0ab54) */
#define CARD_TEXT_TITLE_X 0x78                      /* addiu 0x78, 0x00a0aa04 */
#define CARD_TEXT_TITLE_Y 0x120                     /* addiu 0x120, 0x00a0aa10 */
#define CARD_TEXT_X 0x18                            /* addiu 0x18, 0x00a0aa28 */
#define CARD_TEXT_CAPTION_Y 0x138                   /* addiu 0x138, 0x00a0aa2c */
#define CARD_TEXT_CAPTION_COLOR 0x17                /* addiu 0x17, 0x00a0aa3c */
#define CARD_TEXT_DESCRIPTION_Y 0x19C               /* addiu 0x19C, 0x00a0aa70 */

void CardDisp(Matrix4 *matrix, s16 cardId, s16 owned, s16 selected)
{
    CardDefinition *definition;
    CardDefinitionName *text;
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
    text = CARD_NAME_TABLE + cardId;
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
        if (definition->kind == 1) {
            if ((definition->flags & CARD_FLAGS_NO_HIT_POINTS) == 0) {
                number = (u8)(definition->operationDuration / CARD_DIGIT_BASE); /* the quotient is cut to a byte: `divu` then `andi 0xFF`, 0x00a0a43c */
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
            number = (u8)(definition->attackPower / CARD_DIGIT_BASE); /* byte quotient, `andi 0xFF` at 0x00a0a524 */
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
            case CARD_ATTR_BIT5:
            case CARD_ATTR_BIT6:
                number++;
            case CARD_ATTR_BIT4:
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
            model = CARD_ATTRIBUTE_MODEL + number * CARD_MODEL_STRIDE;
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
        switch (definition->element) {
        case CARD_ELEMENT_50:
            model = CARD_MODEL_ELEMENT_50;
            break;
        case CARD_ELEMENT_52:
            model = CARD_MODEL_ELEMENT_52;
            break;
        case CARD_ELEMENT_55:
            model = CARD_MODEL_ELEMENT_55;
            break;
        case CARD_ELEMENT_4F: /* never matched: the chain tests 0x50, then `slti 0x51` (0x00a0a8b8) */
        default:
            model = CARD_MODEL_ELEMENT_NONE;
            break;
        }
        nmlModelSetPlace(matrix);
        nmlModelSetTexture(CARD_ELEMENT_TEXTURE);
        nmlModelEntryCard(model);
    }
    QuadwordCopyMatrix(place, *matrix);
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
        case CARD_KIND_UNUSED_3: /* never matched: the chain tests 2, then `slti 0x3` (0x00a0a970) */
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
    if (text->captionOffset != 0) {
        xglFontPrint(CARD_TEXT_TITLE_X, CARD_TEXT_TITLE_Y, CARD_TEXT_COLOR, D_00A4D150);
        if (owned > 0) {
            CardDispTxtSub(CARD_TEXT_X, CARD_TEXT_CAPTION_Y, (unsigned char *)(CARD_DATA_BUFFER + text->captionOffset), CARD_TEXT_CAPTION_COLOR);
        } else {
            CardDispTxtSub(CARD_TEXT_X, CARD_TEXT_CAPTION_Y, (unsigned char *)"\x0b\r\0\xa1\xa9\xa1\xa9\xa1\xa9\xa1\xa9\xa1\xa9", CARD_TEXT_CAPTION_COLOR);
        }
    }
    if (owned <= 0) {
        xglFontPrint(CARD_TEXT_X, CARD_TEXT_DESCRIPTION_Y, CARD_TEXT_COLOR, "\x0b\r\0\x0c@@\x80\xa1\xa9\xa1\xa9\xa1\xa9\xa1\xa9\xa1\xa9");
    } else if (text->descriptionOffset != 0) {
        xglFontPrint(CARD_TEXT_X, CARD_TEXT_TITLE_Y, CARD_TEXT_COLOR, "\x0b\r\0\x0c@@\x80");
        xglFontPrint(0, 0, 0, D_00A4D158);
        xglFontPrint(CARD_TEXT_X, CARD_TEXT_DESCRIPTION_Y, CARD_TEXT_COLOR, (const char *)(CARD_DATA_BUFFER + text->descriptionOffset));
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

extern unsigned char SampleDeck0[];

void CardSampleInitDeck0(s16 *deck)
{
    int i;

    for (i = 0; i < 40; i++) {
        deck[i] = SampleDeck0[i];
    }
}

extern unsigned char SampleDeck1[];

void CardSampleInitDeck1(s16 *deck)
{
    int i;

    for (i = 0; i < 40; i++) {
        deck[i] = SampleDeck1[i];
    }
}

extern unsigned char SampleDeck2[];

void CardSampleInitDeck2(s16 *deck)
{
    int i;

    for (i = 0; i < 40; i++) {
        deck[i] = SampleDeck2[i];
    }
}

extern unsigned char SampleDeck3[];

void CardSampleInitDeck3(s16 *deck)
{
    int i;

    for (i = 0; i < 40; i++) {
        deck[i] = SampleDeck3[i];
    }
}

extern unsigned char SampleDeck4[];

void CardSampleInitDeck4(s16 *deck)
{
    int i;

    for (i = 0; i < 40; i++) {
        deck[i] = SampleDeck4[i];
    }
}

extern unsigned char SampleDeck5[];

void CardSampleInitDeck5(s16 *deck)
{
    int i;

    for (i = 0; i < 40; i++) {
        deck[i] = SampleDeck5[i];
    }
}

extern unsigned char SampleDeck6[];

void CardSampleInitDeck6(s16 *deck)
{
    int i;

    for (i = 0; i < 40; i++) {
        deck[i] = SampleDeck6[i];
    }
}

extern unsigned char SampleDeck7[];

void CardSampleInitDeck7(s16 *deck)
{
    int i;

    for (i = 0; i < 40; i++) {
        deck[i] = SampleDeck7[i];
    }
}

extern unsigned char SampleDeck8[];

void CardSampleInitDeck8(s16 *deck)
{
    int i;

    for (i = 0; i < 40; i++) {
        deck[i] = SampleDeck8[i];
    }
}

extern unsigned char SampleDeck9[];

void CardSampleInitDeck9(s16 *deck)
{
    int i;

    for (i = 0; i < 40; i++) {
        deck[i] = SampleDeck9[i];
    }
}

/* Per-enemy ROM deck lists (each entry points at a 40-byte card-id array). */
extern unsigned char *EnemyDeckLst[];

void CardCopyRom2Deck(s16 *deck, s32 enemyId)
{
    unsigned char *romDeck;
    int i;

    romDeck = EnemyDeckLst[enemyId];
    for (i = 0; i < 40; i++) {
        deck[i] = romDeck[i];
    }
}

/* The stored decks repeat CardSaveData.deck as 42-byte records, so the record
 * of slot n starts n * 42 bytes into the save block; the address is formed as
 * n * 32 plus n * 10 (0x00a0ae48..0x00a0ae60). Only the forty card ids at the
 * start of a record are copied, one byte each into a halfword entry. */
void CardCopyRam2Deck(s16 *deck, CardGameWork *work, s32 slot)
{
    unsigned char *stored;
    int i;

    stored = ((CardSaveData *)((slot << 5) + (unsigned int)work->save + slot * 10))->deck;
    for (i = 39; i >= 0; i--) {
        *deck++ = *stored++;
    }
}

/* Collapses the 40 card ids of stored deck `slot` into (card id, copy count)
 * groups. The stored decks are 42-byte records (CARD_DECK_RECORD_SIZE), so card i
 * of the deck sits at CardSaveData.deck[slot * CARD_DECK_RECORD_SIZE + i]. */
void CardCopyRam2CLD(CardDeckGroupList *list, CardGameWork *work, s32 slot)
{
    int prev;
    int groupIndex;
    int i;

    prev = -1;
    groupIndex = -1;
    for (i = 0; i < CARD_LIST_SIZE; i++) {
        if (prev != work->save->deck[slot * CARD_DECK_RECORD_SIZE + i]) {
            groupIndex++;
            list->groups[groupIndex].cardId = work->save->deck[slot * CARD_DECK_RECORD_SIZE + i];
            list->groups[groupIndex].copyCount = 1;
        } else {
            list->groups[groupIndex].copyCount++;
        }
        prev = work->save->deck[slot * CARD_DECK_RECORD_SIZE + i];
    }
    list->groupCount = groupIndex + 1;
}

/* Expands the (card id, copy count) groups of `list` back into the 40 card
 * ids of stored deck `slot`. */
void CardCopyCLD2Ram(CardDeckGroupList *list, CardGameWork *work, s32 slot)
{
    int group;
    int copy;
    int cardIndex;

    cardIndex = 0;
    for (group = 0; group < list->groupCount; group++) {
        for (copy = 0; copy < list->groups[group].copyCount; copy++) {
            work->save->deck[slot * CARD_DECK_RECORD_SIZE + cardIndex] = list->groups[group].cardId;
            cardIndex++;
        }
    }
}

/*
 * The one's-complement checksum of a stored deck's 40 card bytes
 * (CardCopyRam2Deck's comment documents the record's 42-byte stride and the
 * CardSaveData reuse for the deck bytes themselves); the stored checksum
 * sits 2 bytes before that same deck array, inside CardSaveData's own
 * unmodeled_02 span (+0x2A).
 */
s32 CardChkDeckRam(CardGameWork *work, int unused, s32 slot)
{
    u8 *deckByte;
    u16 storedChecksum;
    u8 sum;
    s32 remaining;

    deckByte = ((CardSaveData *) ((slot << 5) + (unsigned int) work->save + slot * 10))->deck;
    sum = 0;
    remaining = 0x27;
    do {
        sum += *deckByte;
        deckByte += 1;
        remaining -= 1;
    } while (remaining >= 0);
    storedChecksum = ((CardDeckRecord *) ((unsigned int) work->save + slot * 0x2A))->checksum;
    return (u8) ~sum == storedChecksum;
}

#define CARD_COUNT_BITS 2                    /* bits of one owned-copy count in CardSaveData.cardCounts */
#define CARD_COUNT_MAX 3                     /* most copies of a card that are counted */
#define CARD_COUNTS_PER_BYTE 4               /* card ids packed in one cardCounts byte (8 / CARD_COUNT_BITS) */
#define CARD_COUNT_MASK(field) (CARD_COUNT_MAX << ((field) * CARD_COUNT_BITS)) /* bits of the count of id % 4 == field: 0x03, 0x0C, 0x30, 0xC0 */
#define CARD_COUNT_STEP(field) (1 << ((field) * CARD_COUNT_BITS))              /* one copy in that field: 0x01, 0x04, 0x10, 0x40 */
#define CARD_COUNT_KEEP(field) (0xFF ^ CARD_COUNT_MASK(field))                 /* the other three fields: 0xFC, 0xF3, 0xCF, 0x3F */

/*
 * Owned copies (0..3) of one card id. The save block packs two bits per card
 * id, four ids per byte, least significant bits first (CardSaveData.cardCounts,
 * asm 0x00a0b038). The masks are 3, 0xC, 0x30 and 0xC0 for ids 0..3 of a byte.
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
 * Adds one owned copy of a card (0x00A0B0C8, 0x100 bytes). Each card id has a
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

/* Forward declaration: defined further down, in the original function order of this TU. */
s32 CardPlayDrawCard(CardPlaySide *side);

/*
 * Starts a game for one side (asm 0x00a0b1c8): the zone reset of CPIWsub is
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
            side->battle[slot].battle.layers[i].state = 0;
            side->battle[slot].battle.position[0] = BattlePos1P[slot][0];
            side->battle[slot].battle.position[1] = BattlePos1P[slot][1];
            side->battle[slot].battle.position[2] = BattlePos1P[slot][2];
            side->disposal[slot].battle.layers[i].cardId = -1;
            side->disposal[slot].battle.layers[i].life = 0;
            side->disposal[slot].battle.layers[i].shownLife = 0;
            side->disposal[slot].battle.layers[i].damageTimer = 0;
            side->disposal[slot].battle.layers[i].state = 0;
            side->disposal[slot].battle.position[0] = DisposePos1P[slot][0];
            side->disposal[slot].battle.position[1] = DisposePos1P[slot][1];
            side->disposal[slot].battle.position[2] = DisposePos1P[slot][2];
        }
    }
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
 * Resets one side's zones (asm 0x00a0b410): the hand, deck, discard, junk and
 * answer lists and the operation list are emptied (-1), the flag, duration
 * and movement records are cleared, and every layer of the four battle and
 * four disposal slots is cleared with the slot position copied from the
 * resting position tables. The deck argument is not read (the caller
 * CardPlayNoInitWork, 0x00A0B5A0, passes its deck list along); no result is
 * returned (the epilogue loads no $v0 and the caller reads none).
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
            side->battle[slot].battle.layers[i].state = 0;
            side->battle[slot].battle.position[0] = BattlePos1P[slot][0];
            side->battle[slot].battle.position[1] = BattlePos1P[slot][1];
            side->battle[slot].battle.position[2] = BattlePos1P[slot][2];
            side->disposal[slot].battle.layers[i].cardId = -1;
            side->disposal[slot].battle.layers[i].life = 0;
            side->disposal[slot].battle.layers[i].shownLife = 0;
            side->disposal[slot].battle.layers[i].damageTimer = 0;
            side->disposal[slot].battle.layers[i].state = 0;
            side->disposal[slot].battle.position[0] = DisposePos1P[slot][0];
            side->disposal[slot].battle.position[1] = DisposePos1P[slot][1];
            side->disposal[slot].battle.position[2] = DisposePos1P[slot][2];
        }
    }
}

/*
 * Sets up the card lists of both players without shuffling (0x00A0B588,
 * 0x90 bytes). CPIWsub resets the zones first; then the side's deck takes
 * ids 0..33 of the deck list (0x00A0B5B8 loop) and the first six hand slots
 * take ids 34..39 (0x00A0B5E8 loop).
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

/* Forward declaration: defined further down, in the original function order of this TU. */
void CardPlayShuffleYama(CardPlaySide *side);

/* Forward declaration: defined further down, in the original function order of this TU. */
s32 CardPlayYamaCnt(CardPlaySide *side);

/*
 * Starts a game for the enemy side with a fixed opening (asm 0x00a0b618,
 * 0x2DC bytes). CPIWsub resets the zones and the deck list is copied. Then,
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

    round = 0;
    do {
        best = -1;
        for (candidate = round; candidate < CARD_LIST_SIZE; candidate++) {
            if (order[candidate] < 0) {
                continue;
            }
            if (definitions[order[candidate]].kind != 1 || definitions[order[candidate]].flags != 1) {
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
 * Shuffles the draw pile (side->deck, CardPlayYamaCnt cards) (asm 0x00a0b8f8).
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
 * and starts its slide to the hand (asm 0x00a0ba10). The top is the last
 * entry of the run of non-negative ids that starts at deck[0], capped at
 * index 39. Returns 1 when the deck is empty, 0 otherwise. The card id is
 * copied as an unsigned halfword (`lhu`, 0x00a0bae4).
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
 * hand slot and starts its slide from the pile to the hand (asm 0x00a0bb88).
 * The top is the last entry of the run of non-negative ids that starts at
 * sute[0], capped at index 39. Returns 1 when the discard pile is empty, 0
 * otherwise. The card id is copied as an unsigned halfword (`lhu`,
 * 0x00a0bc50).
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
 * CardPlayYamaCnt, and starts its slide from the pile to the deck (asm
 * 0x00a0bcf0). The top is found as in CardPlayDrawSute. Returns 1 when the
 * discard pile is empty, 0 otherwise.
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

void CardPlayCleanHand(CardHand *hand)
{
    s32 kept;
    s32 i;

    kept = 0;
    for (i = 0; i < 40; i++) {
        if (hand->cards[i] >= 0) {
            if (i != kept) {
                hand->cards[kept] = hand->cards[i];
                hand->cards[i] = -1;
            }
            kept++;
        }
    }
}

void CardPlayCleanYama(CardPlaySide *side)
{
    s32 kept;
    s32 i;

    kept = 0;
    for (i = 0; i < 40; i++) {
        if (side->deck[i] >= 0) {
            if (i != kept) {
                side->deck[kept] = side->deck[i];
                side->deck[i] = -1;
            }
            kept++;
        }
    }
}

void CardPlayCleanJunk(CardPlaySide *side)
{
    s32 kept;
    s32 i;

    kept = 0;
    for (i = 0; i < 40; i++) {
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
 * Sends the hand card at index to the first free junk slot (asm 0x00a0bf28):
 * the junk entry takes the card id, and its move record starts at the hand
 * card's position (handMove[index]) and slides to JunkPos1P[1] in
 * CARD_SLIDE_FRAMES frames. The hand entry becomes -1 and the hand is
 * closed up. Always returns 0 (`daddu $2,$0,$0`, 0x00a0c07c).
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
 * Moves the hand cards at firstIndex and secondIndex to the junk pile (asm
 * 0x00a0c090): each goes into the first empty junk slot, and its hand slot is
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
 * Sends the operation card operationIndex to the first free junk slot (asm
 * 0x00a0c198): the junk entry takes the operation's card id, and its move
 * record starts at the operation's position and slides to JunkPos1P[1] in
 * CARD_SLIDE_FRAMES frames. Always returns 0 (`daddu $2,$0,$0`,
 * 0x00a0c288).
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
 * Sends every card of the battle stack to the junk pile (asm 0x00a0c2d0): the
 * cards fill the junk slots from the first free one, and each move record
 * starts at the stack's position and slides to JunkPos1P[1] in
 * CARD_SLIDE_FRAMES frames, like CardPlayDesertOperation. Always returns
 * 0 (`daddu $2,$0,$0`, 0x00a0c3d0).
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
 * slide from the deck to the discard pile (asm 0x00a0c3d8). The deck top is
 * the last entry of the run of non-negative ids that starts at deck[0],
 * capped at index 40 (deck[40] is sute[0]). Returns 1 when the deck is empty,
 * 0 otherwise.
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
 * and clears the slot (0x00a0c510). `battle` points at battle[0].battle of the
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

s32 CardPlayYamaCnt(CardPlaySide *side)
{
    s16 *card;
    s16 id;
    s32 count;
    s32 remaining;

    card = side->deck;
    count = 0;
    remaining = 0x27;
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

s32 CardPlaySuteCnt(CardPlaySide *side)
{
    s16 *card;
    s16 id;
    s32 count;
    s32 remaining;

    card = side->sute;
    count = 0;
    remaining = 0x27;
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

s32 CardPlayJunkCnt(CardPlaySide *side)
{
    s16 *card;
    s16 id;
    s32 count;
    s32 remaining;

    card = side->junk;
    count = 0;
    remaining = 0x27;
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

s32 CardPlayOperationCnt(CardPlaySide *side)
{
    s16 *op;
    s16 id;
    s32 count;
    s32 remaining;

    op = side->operations;
    count = 0;
    remaining = 0x27;
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

s32 CardPlayDisposeCnt(CardPlaySide *side)
{
    s16 *cardId;
    s16 id;
    s32 count;
    s32 remaining;

    cardId = &side->disposal[0].battle.layers[0].cardId;
    count = 0;
    remaining = 3;
    do {
        id = *cardId;
        cardId += 0xDE;
        remaining -= 1;
        if (id >= 0) {
            count = count + 1;
        }
    } while (remaining >= 0);
    return count;
}

s32 CardPlayHandCnt(CardHand *hand)
{
    s16 *card;
    s16 id;
    s32 count;
    s32 remaining;

    card = hand->cards;
    count = 0;
    remaining = 0x27;
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

s32 CardPlayBattleCnt(CardPlaySide *side)
{
    s16 *cardId;
    s16 id;
    s32 count;
    s32 remaining;

    cardId = &side->battle[0].battle.layers[0].cardId;
    count = 0;
    remaining = 3;
    do {
        id = *cardId;
        cardId += 0xDE;
        remaining -= 1;
        if (id >= 0) {
            count = count + 1;
        }
    } while (remaining >= 0);
    return count;
}

/*
 * Sorts the count cards by kind (asm 0x00a0c768): for each of the three kinds
 * in order[], every pick moves the lowest card id of that kind still in
 * cards[] into sorted[] and marks its slot -1; sorted[] is then copied back.
 * best and bestValue keep their last values across picks, so a pick that
 * finds no card of the kind retries the previous slot.
 *
 * The original also copies the hand into a second local array before the
 * sort (loop 0x00a0c790..0x00a0c7ac, `sh` to sp+0x50) and never reads it
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

/* Definition flags that let a card skip the slot check (andi 0x64 at asm 0x00a0c938). */
#define CARD_FLAGS_NO_SLOT_CHECK 0x64

/*
 * Check whether a card cannot be played for lack of room (asm 0x00a0c910).
 * A card whose kind has bit 0x1 needs a free disposal slot (base layer card id
 * negative), unless its flags carry one of CARD_FLAGS_NO_SLOT_CHECK; any other
 * card needs a free operation slot. Only the first four slots are searched.
 * Returns 1 when no slot is free, 0 otherwise. Once the definition has been
 * read, the cardId parameter is reused as the "slot found" flag (it lives in
 * $a2, asm 0x00a0c9cc `sltiu $v0,$a2,1`). A separate local only ever set to 0
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
 * definition flags (2 -> 1, 8 -> 2, 4 -> 3, 0x10 -> 4, anything else -> 0).
 * The battle slots are walked with a pointer to their cardId, advanced by one
 * CardBoardSlot per step (the original keeps it in a register from 0x00a0c9d0).
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
            case CARD_ATTR_BIT4:
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
            case CARD_ATTR_BIT4:
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

/* CardPlayChkCondition results other than 0 (the card may be played). */
#define CARD_CONDITION_NEED_UNMET 2       /* `addiu $2,$0,0x2` (0x00a0cd00) */
#define CARD_CONDITION_DISCARD_UNMET 4    /* `addiu $2,$0,0x4` (0x00a0ce74) */
#define CARD_CONDITION_IDENTITY_TAKEN 5   /* `addiu $2,$0,0x5` (0x00a0ceac) */

/* Forward declaration: defined further down, in the original function order of this TU. */
int CardPlayIdentityCheck(CardGameWork *work, CardHand *hand, u16 identity);

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
     * read keeps the address a strength-reduced pointer (starting at 0x00a0cd44 and
     * 0x00a0ce18, `lbu $10,0($12)`), while `array[i]` recomputes it with an `addu` in
     * every iteration and adds three instructions. Pass 1 indexes need[i].
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
        if (CardPlayIdentityCheck(work, &side->hand, work->definitions[cardId].identity) != 0) {
            return CARD_CONDITION_IDENTITY_TAKEN;
        }
    }
    return 0;
}

/*
 * The attack type of the topmost card in play: the stack's layers are walked
 * from layers[39] down, skipping empty layers (cardId < 0) and cards 0x2F and
 * 0x2C, which never decide the type. The result is held in a u8, the width of
 * CardDefinition.attackType.
 */
s32 CardChkAttackType(CardDefinition *definitions, CardLayerStack *card)
{
    u8 attackType;
    int i;

    attackType = 0;
    for (i = 39; i >= 0; i--) {
        if (card->layers[i].cardId >= 0) {
            if (card->layers[i].cardId == 0x2F || card->layers[i].cardId == 0x2C) {
                continue;
            }
            attackType = definitions[card->layers[i].cardId].attackType;
            break;
        }
    }
    return attackType;
}

#define CARD_CURSOR_EDGE_HIGHLIGHT 0.175f    /* highlight size at positions 0 and 3: 0x3E333333 (0x00a0cf80) */
#define CARD_CURSOR_INNER_HIGHLIGHT 0.355f   /* highlight size at positions 1 and 2: 0x3EB5C28F (0x00a0d048) */
#define CARD_CURSOR_INDEX_COUNT 4            /* positions inside a zone (`slti $2,$2,4`, 0x00a0cf50) */
#define CARD_CURSOR_SHOW_CARD_FLAG 0x10      /* drawFlags bit: also draw the card (`andi $2,$7,0x10`, 0x00a0cf8c) */
#define CARD_CURSOR_CARD_ID_MASK 0xF0FF      /* 0x00a0cfbc */
#define CARD_CURSOR_PAIR_ZONE_MASK 0xFE      /* zones 1 and 3 fold onto 0 and 2 (0x00a0d08c) */
#define CARD_CURSOR_PREVIEW_Z 0.1f           /* depth of the previewed card, 0x3DCCCCCD (0x00a0cfd0) */
#define CARD_CURSOR_ZONE_1 1                 /* cursor zone numbers (CardCursorPosition.zone, cursorZone) */
#define CARD_CURSOR_ZONE_2 2                 /* `addiu $2,$0,0x2` (0x00a0d06c) */
#define CARD_CURSOR_ZONE_3 3                 /* zone taken when the hand is empty is the same number (0x00a0d1bc) */
#define CARD_CURSOR_ZONE_4 4                 /* hand zone (0x00a0d170) */
#define CARD_CURSOR_ZONE_5 5                 /* 0x00a10188 */
#define CARD_CURSOR_ZONE_6 6                 /* mark zones 2..7 form the jump table of CardCursorActive (sltiu 0x6, 0x00a10130) */
#define CARD_CURSOR_ZONE_7 7
#define CARD_CURSOR_MARK_COUNT 24            /* marks in markTable and markTableB: six columns of four (counter 0x17, 0x00a100f8) */
#define CARD_CURSOR_MARK_1 1                 /* mark codes: the mode stored by CardCursorActive; CAMASub maps 2, 3 and 4 to mark models */
#define CARD_CURSOR_MARK_2 2                 /* `addiu $3,$0,0x2` (0x00a101e0) */
#define CARD_CURSOR_MARK_3 3                 /* 0x00a101f0 */
#define CARD_CURSOR_MARK_4 4                 /* 0x00a101fc */
#define CARD_CURSOR_JUMP_BUTTON 0x4000       /* button that jumps from position 1 or 2 to zone 2 (0x00a0d064) */

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

extern float HandPos1P[4][3];
extern float *CursorTbl_00A43420[];

#define CARD_CURSOR_HAND_HIGHLIGHT 0.175f    /* hand cursor highlight size: 0x3E333333 (lui/ori at 0x00a0d190) */
#define CARD_CURSOR_ZONE_BACK_BUTTON 0x1000  /* steps the zone back (`andi $2,$17,0x1000`, 0x00a0d0d0) */
#define CARD_CURSOR_INDEX_FORWARD_BUTTON 0x2000 /* steps the index forward (`andi $2,$17,0x2000`, 0x00a0d0f0) */
#define CARD_CURSOR_ZONE_FORWARD_BUTTON 0x4000  /* steps the zone forward (`andi $2,$17,0x4000`, 0x00a0d0f8) */
#define CARD_CURSOR_INDEX_BACK_BUTTON 0x8000    /* steps the index back (`andi $2,$17,0x8000`, 0x00a0d11c) */
#define CARD_CURSOR_ZONE_LIMIT 5             /* zones 5 and above clamp to the hand zone (`slti $2,$2,5`, 0x00a0d150) */
#define CARD_CURSOR_HAND_ZONE 4              /* zone of the hand (`slti $2,$2,4`, 0x00a0d170) */
#define CARD_CURSOR_EMPTY_HAND_ZONE 3        /* zone taken when the hand is empty (`addiu $2,$0,3`, 0x00a0d1bc) */
#define CARD_CURSOR_ZONE_STRIDE 4            /* indices per zone in CursorTbl_00A43420 (`sll $2,$2,2`, 0x00a0d1e8) */

/*
 * Moves the board cursor with the pad buttons (asm 0x00a0d0a8, 0x224 bytes).
 * The zone steps back (0x1000) or forward (0x4000) and the index forward
 * (0x2000) or back (0x8000); the zone is clamped to 0..4. Zones 0..3 go to
 * CPCMSub. The hand zone (4) sets the hand highlight and places the cursor on
 * the hand: the index is clamped to the hand count and the position is spread
 * along HandPos1P. An empty hand moves the cursor to zone 3 and goes to CPCMSub.
 * The zone and index are used through the cursor fields (each store feeds the
 * next test). After CPCMSub the position is copied from the table entry of the
 * zone and index. The copy is written on each of the two call paths (zones 0..3,
 * and the empty hand) because the compiler merges the identical tails into the
 * single jal/copy at 0x00a0d1d8..0x00a0d218 (reached by `b` at 0x00a0d180 and by
 * fall-through from 0x00a0d1d4); a single shared call or a static inline copy
 * helper does not reproduce that layout.
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
        slot = CursorTbl_00A43420[cursor->cursorZone * CARD_CURSOR_ZONE_STRIDE + cursor->cursorIndex];
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
        slot = CursorTbl_00A43420[cursor->cursorZone * CARD_CURSOR_ZONE_STRIDE + cursor->cursorIndex];
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
 * Draws the card list window with the owned copies of each row (asm
 * 0x00a0d2d0, 0x2F8 bytes). The header (D_00A4D500) is printed at the window
 * origin. For each of the first windowRows rows below rowCount, the entry's card
 * id selects the owned copy count in work->ownedCount and the card name in
 * work->definitions (read through CardDefinitionName, which models only the
 * name offset at +0x18 that CardDefinition lacks). A row with no owned copy
 * prints its marker and the placeholder D_00A4D508 instead of the name. With
 * flags bit 0 (`andi $2,$2,0x1`, 0x00a0d458) the owned count
 * (CARD_LIST_COPY_CAP when it reaches CARD_LIST_COPY_LIMIT) follows the name.
 * Rows are DECK_DISP_ROW_HEIGHT pixels apart; the footer is D_00A4D170.
 */
void CardListDisp(CardDeckListView *list, CardGameWork *work)
{
    unsigned char nameText[32];
    char countText[16];
    s32 row = 0;
    s32 cardId;
    s32 copies;
    s32 i;
    char *name;
    CardDefinitionName *definition;
    CardDefinitionName *definitions = (CardDefinitionName *)work->definitions;
    s16 *ownedCount = work->ownedCount;

    xglFontPrint(list->originX, list->originY, DECK_LIST_TEXT_COLOR, D_00A4D500);
    if (list->windowRows > 0) {
        for (; row < list->windowRows; row++) {
            if (row < list->rowCount) {
                cardId = list->entries[row + list->scroll].cardId;
                if (work->ownedCount[cardId] > 0) {
                    definition = &definitions[cardId];
                    name = (char *)(CARD_DATA_BUFFER + definition->nameOffset);
                    if (row == list->cursor) {
                        xglFontPrint(list->originX, list->originY, DECK_LIST_TEXT_COLOR, D_00A4D148);
                    } else {
                        xglFontPrint(list->originX, list->originY, DECK_LIST_TEXT_COLOR, D_00A4D150);
                    }
                    for (i = 0; i < list->textLength * CARD_TEXT_CHAR_BYTES; i++) {
                        nameText[i] = name[i];
                    }
                    nameText[i] = 0;
                    CardNameConvert(nameText);
                    xglFontPrint(0, 0, 0, D_00A4D158);
                    xglFontPrint(list->originX, list->originY + row * DECK_DISP_ROW_HEIGHT,
                                 DECK_LIST_TEXT_COLOR, (char *)nameText);
                    if (list->flags & 1) {
                        copies = ownedCount[cardId];
                        if (copies >= CARD_LIST_COPY_LIMIT) {
                            copies = CARD_LIST_COPY_CAP;
                        }
                        sprintf(countText, D_00A4D168, copies);
                        if (row == list->cursor) {
                            xglFontPrint(list->originX, list->originY, DECK_LIST_TEXT_COLOR, D_00A4D148);
                        } else {
                            xglFontPrint(list->originX, list->originY, DECK_LIST_TEXT_COLOR, D_00A4D160);
                        }
                        xglFontPrint(list->originX + list->textLength * DECK_DISP_CHAR_WIDTH + DECK_DISP_COUNT_GAP,
                                     list->originY + row * DECK_DISP_ROW_HEIGHT, DECK_LIST_TEXT_COLOR, countText);
                    }
                } else {
                    if (row == list->cursor) {
                        xglFontPrint(list->originX, list->originY, DECK_LIST_TEXT_COLOR, D_00A4D148);
                    } else {
                        xglFontPrint(list->originX, list->originY, DECK_LIST_TEXT_COLOR, D_00A4D150);
                    }
                    xglFontPrint(list->originX, list->originY + row * DECK_DISP_ROW_HEIGHT,
                                 DECK_LIST_TEXT_COLOR, D_00A4D508);
                }
            }
        }
    }
    xglFontPrint(list->originX, list->originY, DECK_LIST_TEXT_COLOR, D_00A4D170);
}

/*
 * Draws the deck list window (asm 0x00a0d5c8): the header string, then the
 * rows of DeckListStr below rowCount, each preceded by the cursor marker when
 * it is the selected row and printed DECK_LIST_ROW_HEIGHT pixels apart, and
 * the footer. A row is the font prefix "\x19\x03" followed by textLength
 * bytes of the name.
 */
void CardDeckListDisp(CardDeckListView *list)
{
    char rowText[32];
    char *name;
    s32 row;
    s32 length;

    xglFontPrint(list->originX, list->originY, DECK_LIST_TEXT_COLOR, D_00A4D518);
    rowText[0] = 0x19;
    rowText[1] = 0x03;
    for (row = 0; row < list->windowRows; row++) {
        if (row < list->rowCount) {
            name = DeckListStr[row];
            if (row == list->cursor) {
                xglFontPrint(list->originX, list->originY, DECK_LIST_TEXT_COLOR, D_00A4D148);
            } else {
                xglFontPrint(list->originX, list->originY, DECK_LIST_TEXT_COLOR, D_00A4D150);
            }
            for (length = 0; length < list->textLength; length++) {
                rowText[CARD_ROW_PREFIX_BYTES + length] = name[length];
            }
            rowText[CARD_ROW_PREFIX_BYTES + length] = '\0';
            xglFontPrint(list->originX, list->originY + row * DECK_LIST_ROW_HEIGHT,
                         DECK_LIST_TEXT_COLOR, rowText);
        }
    }
    xglFontPrint(list->originX, list->originY, DECK_LIST_TEXT_COLOR, D_00A4D170);
}

/*
 * Draws one list of strings in a window (0x00A0D740, 0x180 bytes). The list
 * header (a font escape, .rodata 0x00A4D518) is printed at the window origin first. Then, for each of
 * the first `count` rows, the row marker (0x00A4D148 on the row at `index`,
 * 0x00A4D150 on the others) is printed at the origin, followed by the row
 * text: `textLength` characters of the row's string after the two-byte
 * prefix 0x19 0x03, printed 25 pixels lower per row. Rows at or past
 * `limit` print nothing. The footer (0x00A4D170) is printed last.
 */
void CardStrListDisp(CHPTitleListView *list, char **strings, int index)
{
    char rowText[32];
    int row = 0;
    int rowY;
    char **rowPtr;
    char *text;
    int i;

    xglFontPrint(list->x, list->y, DECK_LIST_TEXT_COLOR, D_00A4D518);
    rowText[0] = 0x19;
    rowText[1] = 0x03;
    if (list->count > 0) {
        rowPtr = strings;
        rowY = 0;
        for (; row < list->count; row++) {
            if (row < list->limit) {
                text = *rowPtr;
                if (row == index) {
                    xglFontPrint(list->x, list->y, DECK_LIST_TEXT_COLOR, D_00A4D148);
                } else {
                    xglFontPrint(list->x, list->y, DECK_LIST_TEXT_COLOR, D_00A4D150);
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
    xglFontPrint(list->x, list->y, DECK_LIST_TEXT_COLOR, D_00A4D170);
}

/*
 * Draws a list of enemy deck names (0x00A0D8C0, 0x180 bytes). Same shape as
 * CardStrListDisp, but each row is a card entry: entries[row] indexes
 * DeckListStr (0x00A43508, sixteen bytes per name), the rows are 24 pixels
 * apart, and no prefix bytes are written before the text. The header font
 * escape is 0x00A4D500, the row marker 0x00A4D148 (row at `index`) or
 * 0x00A4D150, the footer 0x00A4D170.
 */
void CardEnemyListDisp(CHPTitleListView *list, s32 *entries, int index)
{
    char rowText[32];
    int row = 0;
    int rowY;
    s32 *rowPtr;
    char *text;
    int i;

    xglFontPrint(list->x, list->y, DECK_LIST_TEXT_COLOR, D_00A4D500);
    if (list->count > 0) {
        rowPtr = entries;
        rowY = 0;
        for (; row < list->count; row++) {
            if (row < list->limit) {
                text = DeckListStr[*rowPtr];
                if (row == index) {
                    xglFontPrint(list->x, list->y, DECK_LIST_TEXT_COLOR, D_00A4D148);
                } else {
                    xglFontPrint(list->x, list->y, DECK_LIST_TEXT_COLOR, D_00A4D150);
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
    xglFontPrint(list->x, list->y, DECK_LIST_TEXT_COLOR, D_00A4D170);
}

/*
 * CGPCalcPowerPlus (ov10/tu008 cgp.c, already accepted C) takes CardPowerCard
 * *card, declared there a second time from the same memory CardLayerStack
 * covers because its own loads at +0x08..+0x0A are signed where
 * CardBattleLayer's accepted accesses of the same bytes are unsigned
 * (ov10/cgp.h's own comment on CardPowerCard). CardPlayCntPower always
 * passes a whole CardLayerStack's address, so the call below casts to the
 * accepted definition's own parameter type instead of redeclaring the
 * extern with a different one.
 */
extern s32 CGPCalcPowerPlus(CardGameWork *work, CardPowerCard *card, s32 power);

s32 CardPlayCntPower(CardGameWork *work, CardLayerStack *slot)
{
    CardDefinition *def;
    s16 topCardId;
    s32 power;

    power = 0;
    if (!(slot->flags & 0x80)) {
        def = &work->definitions[slot->layers[0].cardId];
        topCardId = slot->layers[1].cardId;
        if (topCardId >= 0 && topCardId != 0x35 && topCardId != 0x2F && topCardId != 0x2C) {
            def = &work->definitions[topCardId];
        }
        power = CGPCalcPowerPlus(work, (CardPowerCard *) slot, def->attackPower);
    }
    return power;
}

INCLUDE_ASM("asm/nonmatchings/ov10/card_fread", CardPlayDispBattle);

/*
 * Pile counters shared by CardPlayDispHand, CardPlayDispJunk and CardPlayDispSute: the pile size is shown as
 * tens (model CARD_PILE_COUNT_MODEL + (tens << 10)) and units (+ (units << 10)) digit models
 * (asm 0x00a0ef24 / 0x00a0efac and 0x00a0fe30 / 0x00a0feb4), both with CARD_PILE_COUNT_TEXTURE.
 */
#define CARD_PILE_STACK_SIZE 10               /* `addiu $2,$0,10` before the div (0x00a0ee9c) */
#define CARD_PILE_COUNT_MODEL 0x01C80000
#define CARD_PILE_COUNT_TEXTURE ((const char *)0x01C91000)
#define CARD_PILE_COUNT_SCALE 1.4f            /* 0x3FB33333 (0x00a0eeac) */
#define CARD_PILE_COUNT_Y (-1.876f)           /* 0xBFF020C5 (0x00a0ef08) */
#define CARD_PILE_COUNT_Z 0.01f               /* 0x3C23D70A (0x00a0eeb8) */
#define CARD_PILE_CARD_SCALE 0.175f           /* size of a pile card in motion, 0x3E333333 (0x00a0eff4) */
#define CARD_MOVE_ARRIVE_SOUND 0x2000F        /* sound when a card lands (0x00a0eb68, 0x00a0f0e8, 0x00a0f304, 0x00a0f754, 0x00a0fc14, 0x00a0ffd8) */

/*
 * Draws the hand of one side (asm 0x00a0e7a0): the hand size as tens and units digit models,
 * then every hand card along its handMove record. A card in motion advances by its step and
 * flips by kind (4: growing face-down flip, 3: turning over, 2: face up, other: shrinking);
 * a card that has arrived starts the next stage of its kind (4 -> 3 -> 2 -> 1 -> 0, kind 2
 * sends it to its hand slot) or rests in its slot. On side != 0 a settled card (kind 0)
 * shows the card id from the other end of the hand.
 */
#define CARD_HAND_TENS_X (-0.964f)           /* digit x of the tens count, 0xBF76C8B4 (0x00a0e854) */
#define CARD_HAND_UNITS_X (-0.85f)           /* digit x of the units count, 0xBF59999A (0x00a0e8c4) */
#define CARD_HAND_SETTLE_DEPTH 0.005f        /* depth step between the hand cards, 0x3BA3D70A (0x00a0ec20) */
#define CARD_HAND_FULL_SCALE 2.0f            /* full flip scale of a card, 0x40000000 (0x00a0e9d0) */
#define CARD_HAND_FLAG_BITS 0x0F00           /* flag bits cleared from a hand entry before drawing (addiu -0xF01, 0x00a0ed8c) */

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

        cardIdMask = ~CARD_HAND_FLAG_BITS; /* `addiu $7,$0,-0xF01` (0x00a0ed8c) */
        for (i = 0; i < count; i++) {
            /* The hand card base is formed at the loop top (`addiu $3,$7,8`, 0x00a0e9cc). */
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

            if (side == 0) {
                CardDispCard(&cardMatrix, cards[i] & cardIdMask, 1);
            } else if (ownSide->handMove[i].kind != 0) { /* a card still moving shows its own id (kind test at 0x00a0ed7c) */
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
 * Draws the junk pile of one side (asm 0x00a0ee58): the pile count as digit models, the top
 * card still resting (last slot with moveTimer == 0) at JunkPos1P[0], then every card in
 * motion along its junkMove record; a card that has landed is placed on the pile when it
 * is the last one.
 * The first argument (the side number) is not read.
 */
#define CARD_JUNK_COUNT_TENS_X 1.919f         /* 0x3FF5A1CB (0x00a0eefc) */
#define CARD_JUNK_COUNT_UNITS_X 2.033f        /* 0x40021CAC (0x00a0ef78) */
#define CARD_JUNK_TOP_DROP 0.02f              /* resting top card sits this far below the pile, 0x3CA3D70A (0x00a0f044) */

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
 * Draws the four disposal slots of one side (asm 0x00a0f230). Each slot's cards move from
 * DisposePos1P toward their resting place, the top card is drawn with its frame, a stacked card
 * sits at CardStackPos, the flag markers follow, and the slot value is shown as two digits.
 * The first argument (the side number) is not read.
 */
#define CARD_DISPOSE_SCALE 0.175f             /* card size in the slot, 0x3E333333 (0x00a0f240) */
#define CARD_DISPOSE_STACK_SCALE 0.5070423f   /* stacked card size, 0x3F01CD86 (0x00a0f254) */
#define CARD_DISPOSE_DIGIT_Z 0.05f            /* digit depth, 0x3D4CCCCD (0x00a0f554) */
#define CARD_DISPOSE_MARKER_TEXTURE ((const char *)0x01F70200) /* flag markers (0x00a0f4a8, 0x00a0f4f8) */
#define CARD_DISPOSE_MARKER_MODEL_BIT0 0x01900400
#define CARD_DISPOSE_MARKER_MODEL_BIT1 0x01900C00

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
                if ((s8)ownSide->disposal[slot].battle.layers[0].shownLife < 0) {
                    ownSide->disposal[slot].battle.layers[0].shownLife = 0;
                }
                /* The compiled code reads shownLife as a signed byte here (lb at 0x00a0f510). The field stays u8
                 * because CGPHPCheckSub fills it from the unsigned life byte, so the signed view is a cast. */
                tens = (s8)ownSide->disposal[slot].battle.layers[0].shownLife / CARD_DIGIT_BASE;
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
                model = CARD_PILE_COUNT_MODEL + (((s8)ownSide->disposal[slot].battle.layers[0].shownLife % CARD_DIGIT_BASE) * CARD_MODEL_STRIDE);
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
 * Draws the operation cards of one side (asm 0x00a0f6a0): each live slot (operations[i] >= 0)
 * moves along its operationMove record, then is drawn as a card with its frame; a settled card
 * with operationFlags bit 0 also gets a marker above its slot.
 * The first argument (the side number) is not read.
 */
#define CARD_OPERATION_SCALE 0.175f           /* card size, 0x3E333333 (0x00a0f6ec) */
#define CARD_OPERATION_MARKER_Z 0.01f         /* marker depth (0x00a0f894) */
#define CARD_OPERATION_MARKER_TEXTURE ((const char *)0x01F70200)
#define CARD_OPERATION_MARKER_MODEL 0x01900400
#define CARD_OPERATION_FLAG_MARKER 1          /* operationFlags bit (`andi $2,1`, 0x00a0f87c) */

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
 * Draws the deck pile of one side (asm 0x00a0f928): the deck count as tens and units digit
 * models, then the pile marker and a second marker below it (both only while the deck is not
 * empty), then every card in motion along its yamaMove record. side is 0 for the player and 1
 * for the enemy, which selects the enemy variant of the two marker models (asm 0x00a0fb40,
 * 0x00a0fb9c).
 */
#define CARD_YAMA_COUNT_SCALE 1.7f            /* digit scale of the deck count, 0x3FD9999A (0x00a0f9b4) */
#define CARD_YAMA_COUNT_TENS_X 0.401f         /* 0x3ECD4FDF (0x00a0fa00) */
#define CARD_YAMA_COUNT_UNITS_X 0.547f        /* 0x3F0C0831 (0x00a0fa80) */
#define CARD_YAMA_COUNT_Y (-1.855f)           /* 0xBFED70A4 (0x00a0fa0c) */
#define CARD_YAMA_MARKER_Y (-1.81f)           /* 0xBFE7AE14 (0x00a0fafc) */
#define CARD_YAMA_MARKER_TEXTURE ((const char *)0x016E0000)   /* (0x00a0fb3c) */
#define CARD_YAMA_MARKER_MODEL 0x01911800     /* (0x00a0fb44) */
#define CARD_YAMA_MARKER_MODEL_ENEMY 0x01915800 /* (0x00a0fb5c) */
#define CARD_YAMA_MARKER2_X (-0.484f)         /* 0xBEF7CED9 (0x00a0fb60) */
#define CARD_YAMA_MARKER2_Y (-1.851f)         /* 0xBFECED91 (0x00a0fb70) */
#define CARD_YAMA_MARKER2_TEXTURE ((const char *)0x01D80000)  /* (0x00a0fb9c) */
#define CARD_YAMA_MARKER2_MODEL 0x01D18000    /* (0x00a0fba4) */
#define CARD_YAMA_MARKER2_MODEL_ENEMY 0x01D1C000 /* ori 0x00a0fbbc */

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

    /* The original tests count < 0 first (bltz at 0x00a0fbc0) and then count == 0 (beqz at 0x00a0fbc8) before
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
 * Draws the discard pile of one side (asm 0x00a0fd28): the resting cards (those not sliding
 * in) as digit models, the pile marker on top, then every card in motion along its suteMove
 * record, spun half a turn about Y. A card whose timer is 0 but whose kind is still set has
 * kind cleared and its timer restarted at CARD_SLIDE_FRAMES.
 * The first argument (the side number) is not read.
 */
#define CARD_SUTE_COUNT_TENS_X 0.889f         /* 0x3F639581 (0x00a0fe08) */
#define CARD_SUTE_COUNT_UNITS_X 1.0f          /* units digit x offset */
#define CARD_SUTE_MARKER_TEXTURE ((const char *)0x016E0000)
#define CARD_SUTE_MARKER_MODEL 0x01901800

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
 * Clears both mark tables of the board cursor (asm 0x00a100f0): the 24 marks
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
 * Sets one mark of the board cursor (asm 0x00a10128). The zone (2..7) picks the
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
    /* Dispatched through the jump table at 0x00a4d640 (`sltiu 5`, 0x00a10188); each mark value
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
    addr = 0x01D0C000;
    if ((code != 2) &&
        ((code < 3) ||
         (((addr = 0x01D10000), (code != 3)) &&
          ((addr = 0x01D12000), (code != 4))))) {
        addr = 0x01D08000;
    }
    return addr;
}

/*
 * Draws the board cursor (asm 0x00a10260): the animated cursor frame of the side that holds
 * the cursor with its marker model beside it, then the pulsing lights and the mark models of
 * the six zone columns of CardCursorControl.markTable (own side, then the far side turned
 * half a turn about Z). A zero `mode` also advances the animation and both light pulses.
 */
#define CARD_CURSOR_FRAME_MODEL 0x01D00000       /* own side (0x00a1031c) */
#define CARD_CURSOR_FRAME_MODEL_ENEMY 0x01D02000 /* (0x00a10328) */
#define CARD_CURSOR_MARKER_MODEL 0x01D18000      /* (0x00a102d8) */
#define CARD_CURSOR_MARKER_MODEL_ENEMY 0x01D1C000 /* (0x00a1032c) */
#define CARD_CURSOR_MARKER_TEXTURE ((const char *)0x01D80000) /* (0x00a1047c) */
#define CARD_CURSOR_FRAME_STRIDE 0x510           /* bytes between two animation frames (built from shifts and an add: `sll 2` 0x00a10374, `sll 4` 0x00a10398) */
#define CARD_CURSOR_ANIM_TICKS 24                /* animFrame counts 0..23 (sltiu 0x18, 0x00a10350) */
#define CARD_CURSOR_ANIM_TICKS_PER_FRAME_SHIFT 2 /* four ticks per frame (srl 2, 0x00a10358) */
#define CARD_CURSOR_SCALE 1.15f                  /* 0x3F933333 (0x00a10368) */
#define CARD_CURSOR_MARKER_X 0.275f              /* 0x3E8CCCCD (0x00a1043c) */
#define CARD_CURSOR_MARKER_Y 0.4f                /* 0x3ECCCCCD (0x00a10448) */
#define CARD_CURSOR_PULSE_A_MASK 0x1F            /* andi 0x1F (0x00a104ac) */
#define CARD_CURSOR_PULSE_A_STEP 11.25f          /* degrees per step, 0x41340000 (0x00a104bc) */
#define CARD_CURSOR_LIGHT_A_SWING 0.5f           /* CLColor swing around 1.0, 0x3F000000 (0x00a10504) */
#define CARD_CURSOR_LIGHT_B_SWING 0.25f          /* CLP1Color blue / CLP2Color red swing, 0x3E800000 (0x00a104e4) */
#define CARD_CURSOR_LIGHT_B_BASE 1.75f           /* 0x3FE00000 (0x00a1053c) */
#define CARD_CURSOR_LIGHT_C_SWING 0.235f         /* CLP1Color red and green swing, 0x3E70A3D7 (0x00a10574) */
#define CARD_CURSOR_LIGHT_C_BASE 0.735f          /* 0x3F3C28F6 (0x00a10588) */
#define CARD_CURSOR_LIGHT_D_SWING 0.285f         /* CLP2Color green and blue swing, 0x3E91EB85 (0x00a105bc) */
#define CARD_CURSOR_LIGHT_D_BASE 0.685f          /* 0x3F2F5C29 (0x00a105c8) */
#define CARD_CURSOR_LIGHT_E_SWING 0.25f          /* CMColor swing around 1.0: the same word as the B swing (0x00a104e4) */
#define CARD_CURSOR_PULSE_B_MASK 0x3F            /* andi 0x3F (0x00a10620) */
#define CARD_CURSOR_PULSE_B_STEP 5.625f          /* degrees per step, 0x40B40000 (0x00a10630) */
#define CARD_CURSOR_MARK_Z_LIFT 0.015f           /* 0x3C75C28F (0x00a10648) */
#define CARD_CURSOR_MARK_SCALE_SMALL 0.175f      /* disposal and operation marks, 0x3E333333 (0x00a1065c) */
#define CARD_CURSOR_MARK_SCALE_LARGE 0.355f      /* battle marks, 0x3EB5C28F (0x00a10744) */

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
    /* block addresses the cursor files and marks from the base register 0x5B50 (`addiu $21,$16,0x5B50`);
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
            switch ((s16) work->command.phase) { /* phase is u16 in the record; the original reads it with `lh` (0x00a102f0) */
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
 * CardPlayIdentityCheck (ov10:0x00a10b60): `hand` is a side's CardHand,
 * CardPlaySide's first member (ov10/cgp.h), so it is also the address of the
 * enclosing side; this walks that side's four battle and four disposal board
 * slots, at every stacked layer, for a card whose CardDefinition.identity
 * matches. `identity` masks to 16 bits at entry (`andi $a2,$a2,0xffff`, the
 * function's first instruction) rather than to a byte, so the parameter
 * itself is 16 bits wide; ccu_038_exec_sub.c's extern still declares it
 * `u8` and needs updating to match this definition.
 */
int CardPlayIdentityCheck(CardGameWork *work, CardHand *hand, u16 identity)
{
    s16 *cardIdSlot;    /* walks disposal[3]'s cardId column, one layer per iteration */
    int layer;
    int found;
    s16 cardId;

    found = 0;
    if (identity < 0x80) {
        cardIdSlot = (s16 *)((u8 *)hand + 0xDCE); /* &((CardPlaySide *)hand)->disposal[3].battle.layers[0].cardId */

        /*
         * battle[0..3] and disposal[0..3] are 8 consecutive CardBoardSlot
         * records; each check below reads the same layer of a different
         * slot, at a fixed distance (a multiple of sizeof(CardBoardSlot))
         * behind cardIdSlot's current slot, disposal[3].
         */
        layer = 0;
        do {
            cardId = *(s16 *)((u8 *)cardIdSlot - 7 * sizeof(CardBoardSlot)); /* battle[0] */
            if (cardId >= 0 && work->definitions[cardId].identity == identity) {
                found = 1;
                break;
            }
            cardId = *(s16 *)((u8 *)cardIdSlot - 6 * sizeof(CardBoardSlot)); /* battle[1] */
            if (cardId >= 0 && work->definitions[cardId].identity == identity) {
                found = 1;
                break;
            }
            cardId = *(s16 *)((u8 *)cardIdSlot - 5 * sizeof(CardBoardSlot)); /* battle[2] */
            if (cardId >= 0 && work->definitions[cardId].identity == identity) {
                found = 1;
                break;
            }
            cardId = *(s16 *)((u8 *)cardIdSlot - 4 * sizeof(CardBoardSlot)); /* battle[3] */
            if (cardId >= 0 && work->definitions[cardId].identity == identity) {
                found = 1;
                break;
            }
            cardId = *(s16 *)((u8 *)cardIdSlot - 3 * sizeof(CardBoardSlot)); /* disposal[0] */
            if (cardId >= 0 && work->definitions[cardId].identity == identity) {
                found = 1;
                break;
            }
            cardId = *(s16 *)((u8 *)cardIdSlot - 2 * sizeof(CardBoardSlot)); /* disposal[1] */
            if (cardId >= 0 && work->definitions[cardId].identity == identity) {
                found = 1;
                break;
            }
            cardId = *(s16 *)((u8 *)cardIdSlot - sizeof(CardBoardSlot));    /* disposal[2] */
            if (cardId >= 0 && work->definitions[cardId].identity == identity) {
                found = 1;
                break;
            }
            cardId = *cardIdSlot;                                          /* disposal[3] */
            if (cardId >= 0 && work->definitions[cardId].identity == identity) {
                found = 1;
                break;
            }
            cardIdSlot = (s16 *)((u8 *)cardIdSlot + sizeof(CardBattleLayer));
            layer++;
        } while (layer < 40);
    }
    return found;
}

/*
 * Draws the cards of one hand (asm 0x00a10ce0). The cards are spaced evenly along
 * the window edge win0pos, from its first corner to its second, and each card sits
 * CARD_HAND_DEPTH_STEP further back than the one before it. The card at `selected`
 * is placed one step deeper than the last card, (count + 1) steps back, and drawn at
 * CARD_HAND_SELECTED_SCALE; the others are drawn at CARD_HAND_SCALE. The hand model
 * is then placed with the same matrix and entered. flipped is passed in $a3 by
 * CGCPHandDispSub but is never read: the first use of $a3 is the write that
 * clears the last argument of CardDisp (0x00a10e74).
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
 * Draws one generation icon per entry of need (asm 0x00a10f00), CARD_GENERATION_ICON_STEP
 * apart along a row; flipped moves the row to the other side of the board. The entries
 * 1, 2, 8, 4 and 16 select the icon of generation 0..4 (the jump table at 0x00a4d690
 * falls through 16, 4, 8, 2 and counts one more per step); 31 draws the model with index 7
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
    for (slot = 0; slot < CARD_NEED_SLOT_COUNT; slot++) {
        anyGeneration = 0;
        generation = 0;
        switch (need[slot]) {
        case CARD_NEED_ANY:
            anyGeneration = 1;
            generation = 3; /* three increments below reach icon CARD_NEED_ICON_ANY (0x00a11014) */
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
            model = CARD_GENERATION_MODEL_BASE + generation * CARD_GENERATION_MODEL_SIZE;
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
 * Draws the generation markers of one side (asm 0x00a11168): CardCountGeneration
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
                /* Declared first: the slot * 0x400 offset (`sll 10`, 0x00a11274) is the first
                 * value set up before the loop, ahead of the array addresses (0x00a11278..0x00a1127c). */
                unsigned int model = CARD_GENERATION_MODEL_BASE + slot * CARD_GENERATION_MODEL_SIZE;

                xglMatrixScale(placement, matrix, scale);
                nmlModelSetPlace(&placement);
                nmlModelSetTexture(CARD_GENERATION_TEXTURE);
                nmlModelEntryCard(model);
                xglMatrixTrans(matrix, matrix, translation);
            }
        }
    }
}

#define CARD_HAND3_BASE_Y 1.5f                 /* 0x3FC00000, 0x00a11370 */
#define CARD_HAND3_BASE_Y_OPTION 1.74f         /* 0x3FDEB852, 0x00a11384 */
#define CARD_HAND3_BASE_Z 0.15f                /* 0x3E19999A, 0x00a113a4 */
#define CARD_HAND3_BASE_SCALE_Y 0.97f          /* 0x3F7851EC, 0x00a113b4 */
#define CARD_HAND3_SCALE 0.23f                 /* 0x3E6B851F, 0x00a11474 */
#define CARD_HAND3_SELECTED_SCALE 0.3f         /* 0x3E99999A, 0x00a11480 */
#define CARD_HAND3_SELECTED_TEXTURE ((const char *)0x01D60000) /* 0x00a116a0 */
#define CARD_HAND3_SELECTED_MODEL 0x01D12000   /* 0x00a116a4 */
#define CARD_HAND3_BASE_TEXTURE ((const char *)0x019B0000)     /* 0x00a11788 */
#define CARD_HAND3_BASE_MODEL 0x019AC000       /* 0x00a1178c */

/*
 * Draws the hand of one side on the battle window (asm 0x00a11320): the cards are
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
    nmlModelSetPlace(&matrix);
    nmlModelSetLight(&CNColor, &asDir);
    nmlModelSetTexture(CARD_HAND3_BASE_TEXTURE);
    nmlModelEntryCard(CARD_HAND3_BASE_MODEL);
    CGCPGeneDispSub(1, definitions, side, matrix, counts);
    CGCPNeedDispSub(1, definitions[side->hand.cards[selected]].need, &matrix, counts);
}

#define CARD_HAND2_BASE_Y (-1.7f)               /* 0xBFD9999A, 0x00a1187c */
#define CARD_HAND2_BASE_Z 0.1f                  /* 0x3DCCCCCD, 0x00a11888 */
#define CARD_HAND2_BASE_SCALE_Y 0.97f           /* 0x3F7851EC, 0x00a11898 */
#define CARD_HAND2_SCALE 0.23f                  /* 0x3E6B851F, 0x00a1195c */
#define CARD_HAND2_SELECTED_SCALE 0.3f          /* 0x3E99999A, 0x00a11968 */
#define CARD_HAND2_SELECTED_TEXTURE ((const char *)0x01D60000)  /* 0x00a11b8c */
#define CARD_HAND2_SELECTED_MODEL 0x01D12000    /* 0x00a11b90 */
#define CARD_HAND2_PILE_SCALE 1.2f              /* 0x3F99999A, 0x00a11c60 */
#define CARD_HAND2_PILE_Y (-0.005f)             /* 0xBBA3D70A, 0x00a11c70 */
#define CARD_HAND2_TENS_X 1.116f                /* 0x3F8ED917, 0x00a11cb8 */
#define CARD_HAND2_UNITS_X 1.229f               /* 0x3F9D4FDF, 0x00a11d24 */
#define CARD_HAND2_BASE_TEXTURE ((const char *)0x019B0000)      /* 0x00a11db4 */
#define CARD_HAND2_BASE_MODEL 0x019A8000        /* 0x00a11db8 */

/*
 * Draws the hand of one side on the battle window (asm 0x00a11820): like
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
    for (i = 0; i < count; i++) {
        flags = CARD_DISP_ENABLED | CARD_DISP_OVERLAY;
        if (i == count - 1) {
            flags = CARD_DISP_ENABLED;
        }
        position[0] = NewWin0pos[0][0] + spacing * (float)i;
        position[2] = NewWin0pos[0][2] + (float)i * CARD_WINDOW_DEPTH_STEP;
        if (i == selected) {
            /* The selected card's depth is win0pos[0][2] (lwc1 0x8 off win0pos, 0x00a11a50; lui at 0x00a11a3c). */
            position[2] = win0pos[0][2] + (float)(count + 1) * CARD_WINDOW_DEPTH_STEP;
        }
        if (mode != 0) {
            switch (definitions[side->hand.cards[i]].kind) {
            case 1:
                if ((mode & 1) == 0) {
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
 * Draws the hand of cards for the given side (asm 0x00a11e50). The matrix
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
 * Draws the yama (deck) of one side (asm 0x00a11f00): the top `count` cards of the pile
 * are spaced evenly along the window edge win0pos like a hand, each
 * CARD_WINDOW_DEPTH_STEP further back than the one before it. The card at
 * `selectedOffset` is drawn larger and in front. `style` selects the highlighted
 * CardDisp mode; without a style and with button 0x10 held, the selected card is also
 * drawn as a mark at (0, CARD_YAMA_MARK_Y). Afterwards the deck back and the pile
 * model are entered and the caption string is printed.
 */
#define CARD_YAMA_MARK_Y 0.369f
#define CARD_YAMA_MARK_Z 0.3f
#define CARD_YAMA_MODEL_BASE 0x01DA0000
#define CARD_YAMA_MODEL_PILE 0x01DAAC00
/* CardDisp mode of the selected card when a style is set (asm 0x00a12118) and of its mark (asm 0x00a121a0) */
#define CARD_YAMA_SELECTED_MODE 5
#define CARD_YAMA_MARK_FLAG 0x10               /* flags bit: also draw the marker card (`andi $20,$20,0x10`, 0x00a12060) */
#define CARD_WINDOW_SINGLE_X (-2.0f)           /* x offset of the pile card with a single card, 0xC0000000 (0x00a12224) */
#define CARD_WINDOW_CAPTION_X 22               /* caption position (`addiu $4,$0,0x16`, 0x00a12338) */
#define CARD_WINDOW_CAPTION_Y 140              /* `addiu $5,$0,0x8C` (0x00a1233c) */
#define CARD_WINDOW_CAPTION_COLOR 0xFFF8       /* `ori $6,$0,0xFFF8` (0x00a12344) */

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
            /* The mode argument is set after xglMatrixScale, inside the loop: `addiu $a3,$0,5`
             * (0x00a12118) with a `movz` on style in the call's delay slot (0x00a12130). */
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
    /* The base matrix is copied by lq/sq through $2 (asm 0x00a121e4..0x00a12200), the
     * SDK sceVu0CopyMatrix shape; QuadwordCopyMatrix (quadword.h) is that inline asm. */
    QuadwordCopyMatrix(cardMatrix, base);
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
    xglFontPrint(CARD_WINDOW_CAPTION_X, CARD_WINDOW_CAPTION_Y, CARD_WINDOW_CAPTION_COLOR, D_00A4D710);
}

/*
 * Draws the junk pile of one side (asm 0x00a12380). The junk cards are spaced evenly
 * along the window edge win0pos like a hand, each CARD_WINDOW_DEPTH_STEP further back
 * than the one before it in reverse order (the first card is deepest); the card at
 * `selected` is placed (count + 1) steps back and drawn at CARD_HAND_SELECTED_SCALE.
 * The pile marker model is drawn afterwards at CARD_WINDOW_PILE_X/Y with a flattened
 * scale (CARD_WINDOW_PILE_SCALE_Y) and the pile caption D_00A4D720 is printed.
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
    xglFontPrint(CARD_WINDOW_CAPTION_X, CARD_WINDOW_CAPTION_Y, CARD_WINDOW_CAPTION_COLOR, D_00A4D720);
}

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
    remaining = 3;
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
    remaining = 3;
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
 * hand is CardHand only by its call sites' declared type (CardPlayCntFld,
 * outside this TU's allocation, passes &work->player.hand/&work->enemy.hand);
 * CardHand is CardPlaySide's own first member (ov10/cgp.h, offset +0x000), so
 * that address is numerically CardPlaySide's own and this reaches
 * disposal[0]'s top card id, sibling to CardPlayCntBtl's battle[0].
 */
int CardPlayCntDsp(CardHand *hand, s16 cardId)
{
    CardPlaySide *side;
    s16 *cardIdField;
    s16 id;
    s16 want;
    s32 count;
    s32 remaining;

    want = cardId;
    side = (CardPlaySide *) hand;
    cardIdField = &side->disposal[0].battle.layers[0].cardId;
    count = 0;
    remaining = 3;
    do {
        id = *cardIdField;
        cardIdField += 0xDE;
        remaining -= 1;
        if ((want ^ id) == 0) {
            count = count + 1;
        }
    } while (remaining >= 0);
    return count;
}

/* Same base-address identity as CardPlayCntDsp, reaching battle[0] instead. */
int CardPlayCntBtl(CardHand *hand, s16 cardId)
{
    CardPlaySide *side;
    s16 *cardIdField;
    s16 id;
    s16 want;
    s32 count;
    s32 remaining;

    want = cardId;
    side = (CardPlaySide *) hand;
    cardIdField = &side->battle[0].battle.layers[0].cardId;
    count = 0;
    remaining = 3;
    do {
        id = *cardIdField;
        cardIdField += 0xDE;
        remaining -= 1;
        if ((want ^ id) == 0) {
            count = count + 1;
        }
    } while (remaining >= 0);
    return count;
}

/*
 * Count the light cards on the base layer of a side's four battle slots and
 * four disposal slots: a card counts when its definition flags carry bit 0x1
 * or bit 0x2 (asm 0x00a127a0). An empty slot holds a negative card id and is
 * skipped. Each loop walks the card id field of its slots, one CardBoardSlot
 * apart.
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
 * Count the heavy cards (definition flags bit 0x8) that sit on the base layer
 * of a side's four battle slots and four disposal slots (asm 0x00a12860).
 * The base layer's card id is the halfword 0x1AA into a CardPlaySide, and the
 * slots are sizeof(CardBoardSlot) bytes apart. An empty slot holds a negative
 * id and is skipped.
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
 * heavy-weapon flag (definitions flags bit 0x8) and whose base card passes
 * CGPWeaponEquipChk against cardId (0x00a12900). The four battle slots are
 * checked first, then the four disposal slots.
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

/* still INCLUDE_ASM above in this TU; CardHand * matches CardPlayHandCnt's
 * argument (ov10/cgp.h), the same player.hand/enemy.hand records this
 * function indexes. */
extern int CardPlayCntDsp(CardHand *hand, s16 cardId);
extern int CardPlayCntBtl(CardHand *hand, s16 cardId);

int CardPlayCntFld(CardGameWork *work, s16 cardId) {
    CardHand *playerHand;
    CardHand *enemyHand;
    int count;

    playerHand = &work->player.hand;
    enemyHand = &work->enemy.hand;
    count = CardPlayCntDsp(playerHand, cardId);
    count = count + CardPlayCntBtl(playerHand, cardId);
    count = count + CardPlayCntDsp(enemyHand, cardId);
    return count + CardPlayCntBtl(enemyHand, cardId);
}
