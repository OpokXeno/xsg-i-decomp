#ifndef INCLUDE_OV10_CARD_PLAY_SOUND1_H
#define INCLUDE_OV10_CARD_PLAY_SOUND1_H

#include "shared.h"
#include "ov10/cgp.h"

/* One line of a card list window: a card id and how many copies it holds. */
typedef struct CardListEntry {
    s16 cardId;
    s16 count;
} CardListEntry;

#define CARD_LIST_ENTRY_MAX 151

/*
 * A scrolling card list window (0x270 bytes) handed to the deck load, make,
 * save and view screens, and to the card game (CGPCheckTeMax lays one out for
 * the hand). CardMainProc fills the layout members before the screen runs and
 * the entries from a stored deck.
 */
typedef struct CardListWindow {
    u16 active;                    /* +0x00: read unsigned; bit 0 of the deck and list windows makes CardListDisp print the owned count */
    union {
        struct {
            u8 baseColor;          /* +0x02 */
            u8 cursorColor;        /* +0x03 */
        };
        struct {
            s8 typeFilter;         /* +0x02: the deck builder's list window (CardMakeDeckProc): index into TypeMaskLst */
            s8 attrFilter;         /* +0x03: index into AttriMaskLst */
        };
    };
    s8 mode;                       /* +0x04: scroll request, -1 / 0 / 1; lb at 0x00a1c2ec, 0x00a1c304 */
    s8 cursor;                     /* +0x05: lb at 0x00a1c388, 0x00a1c3c0; the decrements reload it with lbu (0x00a1c394) */
    s16 x;                         /* +0x06 */
    s16 y;                         /* +0x08 */
    u16 textLength;                /* +0x0A: the list printers read it unsigned (lhu) as the characters printed per row (ov10/card_fread) */
    s16 top;                       /* +0x0C */
    s16 shownCount;                /* +0x0E: entries listed, mirror of count kept as the cursor limit */
    s16 rows;                      /* +0x10 */
    s16 count;                     /* +0x12: entries in use */
    CardListEntry entries[CARD_LIST_ENTRY_MAX]; /* +0x14 */
} CardListWindow;

/* CardGameWork.models entries (title screen layout, CardTitleInit 0x00a008e8). */
#define CARD_MODEL_COUNT 16

/* Fade start values: fadeOutTimer and fadeOutStep (li 0x00a01ffc, 0x00a02000; CardFadeOut subtracts the step per tick). */
#define CARD_FADE_TICKS 10
#define CARD_FADE_STEP 16

/*
 * Screens of the card game, CardGameWork.fase (switch in CardMainProc). Names from the screen each one runs and, for
 * the title menu entries, from its label ("Exhibition", "Tournament", "2P Battle", "Create deck", "Open card",
 * "View cards", "Tutorial"). TEST_MENU and TEST_BATTLE are a debug screen only the unreachable fifth main menu
 * entry opens; "test" is the programmers' word for it, from its contents (T1..T10 sample decks, sound list, volume).
 */
enum CardFase {
    CARD_FASE_TITLE = 0,          /* fade in, then load the title layout */
    CARD_FASE_MENU = 1,           /* title menu: Game / Card / ... */
    CARD_FASE_OPEN_PACKS = 2,     /* CardOpenProc */
    CARD_FASE_VIEW_CARDS = 3,     /* CardViewProc */
    CARD_FASE_TEST_MENU = 5,      /* sound list, volume and the sample deck choice for 1P / 2P */
    CARD_FASE_TEST_BATTLE = 6,    /* CardGameProc started from the test menu */
    CARD_FASE_DECK_BUILD = 7,     /* "Deck building mode": load, make and save a deck */
    CARD_FASE_BATTLE_2P = 9,      /* 1P and 2P load a deck each (pad 1 drives side 1) */
    CARD_FASE_TOURNAMENT = 10,    /* menu "Tournament": five enemy decks (EnemyDeckListStr, lst11) played in turn */
    CARD_FASE_EXHIBITION = 11,    /* menu "Exhibition": one enemy deck (lst14) */
    CARD_FASE_TUTORIAL = 12       /* CardTutorialProc */
};

/* Colour argument of every xglFontPrint in the card screens (ori 0xfff0 at 0x00a1c280). */
#define CARD_TEXT_COLOR 0xFFF0

/* Any of the four face buttons pressed (andi 0xf0). */
#define CARD_PAD_ANY_FACE_BUTTON (PAD_TRIANGLE | PAD_CIRCLE | PAD_CROSS | PAD_SQUARE)
/* Confirm (circle) or cancel (cross) pressed: andi 0x60 at 0x00a1de44. */
#define CARD_PAD_ANY_RESPONSE (PAD_CIRCLE | PAD_CROSS)
/* The same two bits tested in the doubleword at PadData+0x28 or CardCommandInput.all (`ld`, then `and` with `lui 0x60`
 * at 0x00a1cfa8..0x00a1cfb0 and 0x00a1e37c..0x00a1e384): the pressed halfword sits 16 bits above the held one. */
#define CARD_PAD_ANY_RESPONSE_WORD (CARD_PAD_ANY_RESPONSE << 16)

/*
 * PadData is the 0xd0-byte table of two 0x68-byte per-pad records at 0x00490d90
 * (the same layout ov12/xrg_pad.c views as PadDataEntry). The card screens read the
 * halfwords at +0x28 and +0x2a, the d-pad direction halfword at +0x2c, and
 * +0x28..+0x2f as one doubleword. The shared PadPrefix (include/shared.h) extends to
 * +0x36 and has no member at +0x2c, so a record built on it would put `directions`
 * at +0x36.
 */
typedef struct CardPadEntry {
    u8 unmodeled_00[0x28];
    union {
        u64 all;                   /* +0x28: `ld` at 0x00a1cfa8 */
        struct {
            u16 half_28;           /* held buttons */
            u16 half_2a;           /* newly pressed buttons */
            u16 directions;        /* +0x2c: d-pad direction bits, `lhu` at 0x00a1c30c */
        };
    };
    u8 unmodeled_30[0x68 - 0x30];
} CardPadEntry;

/* Functions and data used by other translation units. */
extern CardPadEntry PadData[2];

/* ov10/card_play_sound1.c: the card game's main loop and the data it owns. */
extern int CardMainInit(CardGameWork *work);
extern s32 CardMainProc(CardGameWork *work);
extern void CardTitleInit(CardGameWork *work);
extern void CMIBGInit(CardGameWork *work);
extern int NewCMPGameModeExit(int player, CardGameWork *work, int round);  /* round is passed but never read */
extern void CaedGameWinMode(int outcome, CardGameWork *work, int round);
extern char *CardPlayTMessList[79];
extern char P1DeckLoadStr[32];
extern char P2DeckLoadStr[32];
extern char ModeEndMess0[16];
extern char ModeEndMess1[48];
extern char ModeEndMess2[40];
extern char ModeEndMess3[48];
extern char ModeContMess0[48];
extern char ModeContMess1[48];
/* Title menu models: a model, texture pair per menu entry. */
extern void *TitleMdlLst[10];

extern float asDir[4][4];
extern float asColor[4][4];
extern float CCColor[4][4];
extern float KKColor[4][4];
extern float CLP1Color[4][4];
extern float CLP2Color[4][4];
extern float HalfColor[4][4];
extern float MiddleColor[4][4];
extern float CLColor[4][4];
extern float CNColor[4][4];
extern float CMColor[4][4];
extern float HandPos1P[4][3];
extern float DisposePos1P[4][3];
extern float OperationPos1P[4][3];
extern float BattlePos1P[4][3];
extern float JunkPos1P[4][3];
extern float HandPos2P[4][3];
extern float DisposePos2P[4][3];
extern float OperationPos2P[4][3];
extern float BattlePos2P[4][3];
extern float JunkPos2P[4][3];

/* ov10/card_view_proc.c */
extern void CardViewProc(CardGameWork *work, CardListWindow *list, s16 *exiting);
extern void CardOpenProc(CardGameWork *work, s16 *exiting);
extern unsigned char TitleMaxList[7];
extern s8 SkipStartList[7];
extern s8 PageStatusLst[177];
extern u8 SkipPageLst[98];

/* ov10/card_tutorial.c */
extern void CardTutorialInit(void);
extern int CardTutorialProc(CardGameWork *work);

#endif /* INCLUDE_OV10_CARD_PLAY_SOUND1_H */
