#ifndef CARD_PLAY_SOUND1_H
#define CARD_PLAY_SOUND1_H

#include "ov10/cgp.h"
#include "main/xgl_2.h"
#include "main/xgl_studio.h"

/* One line of a card list window: a card id and how many copies it holds. */
typedef struct CardListEntry {
    s16 cardId;
    u16 count;
} CardListEntry;

/*
 * A scrolling card list window (0x270 bytes) handed to the deck load, make,
 * save and view screens. CardMainProc fills the layout members before the
 * screen runs and the entries from a stored deck.
 */
typedef struct CardListWindow {
    s16 active;
    u8 baseColor;                  /* +0x02 */
    u8 cursorColor;                /* +0x03 */
    u8 mode;                       /* +0x04 */
    u8 cursor;                     /* +0x05 */
    s16 x;                         /* +0x06 */
    s16 y;                         /* +0x08 */
    s16 lineHeight;                /* +0x0A */
    s16 top;                       /* +0x0C */
    s16 columns;                   /* +0x0E */
    s16 rows;                      /* +0x10 */
    s16 count;                     /* +0x12: entries in use */
    CardListEntry entries[151];    /* +0x14 */
} CardListWindow;


/* Title menu models: a model, texture pair per menu entry. */
extern void *TitleMdlLst[10];
extern float CCColor[4][4];
extern float KKColor[4][4];
extern float asColor[4][4];
extern float asDir[4][4];
extern float Player1Color[4][4];
extern float Player2Color[4][4];
extern char *EnemyDeckListStr[];
extern char P1DeckLoadStr[];
extern char P2DeckLoadStr[];
extern char ModeEndMess0[];
extern char ModeEndMess1[];
extern char ModeEndMess2[];
extern char ModeEndMess3[];
extern char ModeContMess0[];
extern char ModeContMess1[];
extern char *CardPlayTMessList[];
extern char ZenkakuListA[];
/*
 * PadData is the 0xd0-byte table of two 0x68-byte per-pad records at
 * 0x00490d90 (the same layout ov12/xrg_pad.c views as PadDataEntry). Each
 * record starts with a local 0x2c-byte partial view; CardMainProc also reads the
 * d-pad direction halfword at +0x2c and, in 2P battles, the second record.
 */
typedef struct CardPadEntry {
    struct {
        u8 pad_00[0x28];
        u16 half_28;
        u16 half_2a;
    } prefix;
    u16 directions;                /* +0x2c: d-pad direction bits */
    u8 unmodeled_2e[0x68 - 0x2e];
} CardPadEntry;

extern CardPadEntry PadData[2];

/*
 * The stored decks of the save data: five 42-byte records of a checksum halfword and the
 * 40 card ids, starting 0x2A bytes into CardSaveData (CardSaveData.deck is the card array
 * of the first record).
 */
#define CARD_DECK_SIZE 40                /* slti 0x00a00ba0 in CardMainInit */
#define CARD_STORED_DECK_COUNT 5
typedef struct CardStoredDeck {
    u16 checksum;                  /* +0x00: one's complement of the sum of the card ids */
    u8 cards[CARD_DECK_SIZE];      /* +0x02 */
} CardStoredDeck;

typedef struct CardSaveView {
    u8 unmodeled_00[0x2A];
    CardStoredDeck decks[CARD_STORED_DECK_COUNT];
} CardSaveView;

#define CARD_STORED_DECK(work, n) (((CardSaveView *) (work)->save)->decks[n])

/* CardSaveData.flags bits this TU uses (cgp.h names CARD_SAVE_ROTATE_STAGE). */
#define CARD_SAVE_LARGE_CARDS 0x20   /* toggled from the title menu */
#define CARD_SAVE_PROMO_GIVEN 0x40   /* the promotional card was awarded */


/* Font colour strings the earlier title functions emitted first. */
typedef struct CardFontCode5 {
    char text[5];
} CardFontCode5;

typedef struct CardFontCode8 {
    char text[8];
} CardFontCode8;

/* The original eight-byte item contains a five-byte font code and its
 * three zero tail bytes. */
typedef struct CardFontCodeRecord {
    CardFontCode5 code;
    char trailing[3];
} CardFontCodeRecord;

extern const CardFontCodeRecord cardHelpFontCodes;
extern const CardFontCode8 cardPlayerLabelFontCodes;
extern const CardFontCodeRecord cardPlayerAutoLabel;
extern char cardHelpLabel[];
extern char cardMenuFooterFontCode[];
extern char cardMenuFooterEndCode[];
extern char cardBattleWin1LexName[];
extern char cardBattleWin2LexName[];
extern char cardBattleWin3LexName[];
extern char cardBattleWin4LexName[];
extern char cardBattleWin5LexName[];
extern char cardBattleWinBinName[];

extern void CardPlaySound1(void);
extern void CardPlaySound2(void);
extern void CardTitleInit(CardGameWork *work);
extern void CMIBGInit(CardGameWork *work);
extern void CardMainItemGetSub(int *prizes, int round);
extern void CardMainAddItemSub(CardGameWork *work, int *prizes);
extern void CardFadeIn(CardGameWork *work);
extern void CardHelpProc(CardGameWork *work);
extern void CardHelpPageInit(CardGameWork *work);
extern void CardOpenProc(CardGameWork *work, s16 *exiting);
extern void CardViewProc(CardGameWork *work, CardListWindow *list, s16 *exiting);
extern int CardLoadDeckProc(int exiting, CardListWindow *cursor, CardListWindow *list,
                            CardGameWork *work, char *title);
extern int CardMakeDeckProc(int exiting, CardListWindow *cursor, CardListWindow *list,
                            CardGameWork *work);
extern int CardSaveDeckProc(int exiting, CardListWindow *cursor, CardListWindow *list,
                            CardGameWork *work, char *title);
extern void CardCopyRam2Deck(s16 *deck, CardGameWork *work, int slot);
extern void CardCopyRom2Deck(s16 *deck, int deckId);
extern void CardCopyRam2CLD(CardListWindow *list, CardGameWork *work, int slot);
extern void CardCopyCLD2Ram(CardListWindow *list, CardGameWork *work, int slot);
extern int CardGetDeckAllCnt(CardListWindow *list);
extern int CardGameProc(int exiting, CardListWindow *list, CardGameWork *work, int round);
extern void CardGameInit(CardGameWork *work);
extern void CardPlayInitWork(CardPlaySide *side, s16 *deck);
extern void CardPlayInitWorkFake(CardPlaySide *side, s16 *deck, CardDefinition *definitions);
extern void CardPlayNoInitWork(CardPlaySide *side, s16 *deck);
extern void CardTutorialInit(void);
extern int CardTutorialProc(CardGameWork *work);
extern void CardDispTxtSub(int x, int y, char *text, int color);
extern void CardSampleInitDeck0(s16 *deck);
extern void CardSampleInitDeck1(s16 *deck);
extern void CardSampleInitDeck2(s16 *deck);
extern void CardSampleInitDeck3(s16 *deck);
extern void CardSampleInitDeck4(s16 *deck);
extern void CardSampleInitDeck5(s16 *deck);
extern void CardSampleInitDeck6(s16 *deck);
extern void CardSampleInitDeck7(s16 *deck);
extern void CardSampleInitDeck8(s16 *deck);
extern void CardSampleInitDeck9(s16 *deck);
extern void CardFadeOut(CardGameWork *work);
extern void nmlModelEntryCard(void *model);
extern void nmlModelSetPlace(float matrix[4][4]);
extern void nmlModelSetTexture(void *texture);
extern void xglFontDebugPrintf(int x, int y, const char *format, ...);
extern void xglMatrixTrans(float result[4][4], float matrix[4][4], float translation[4]);
extern float xglSin(float angle);
extern void xglSoundEffectNormalID(int sound_id, int variant);
extern StudioCamera *xglStudioSelectGetActiveCamera(int studio_index);
extern void xglLightIntensityAmbient(void *light, void *ambient);
extern int xglFlagsGet(int flag, int count);
extern void xglSoundLoadRequestSmd(const char *name, void *buffer);
extern void xglSoundEffectParamDirect(int sound, int volume, int pan);
extern void xglRenderGlobalFadeSet(float red, float green, float blue, float alpha);
extern void nmlModelSetLight(float color[4][4], float direction[4][4]);
extern int dataEvtBoxInc(int event);
extern void nmlModelSetToumei(int enabled);
extern void nmlModelSetAlpha(u64 alpha);
extern void nmlModelEntry(unsigned int model);
extern int CardPlayYamaCnt(CardPlaySide *side);
extern void CGPRefreshFieldHP(CardGameWork *work);
extern int sprintf(char *buffer, const char *format, ...);
extern void *memset(void *buffer, int value, unsigned int size);

#endif
