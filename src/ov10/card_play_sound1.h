#ifndef CARD_PLAY_SOUND1_H
#define CARD_PLAY_SOUND1_H

#include "ov10/cgp.h"
#include "ov10/card_play_sound1.h"
#include "ov10/card_fread.h"
#include "ov10/card_deck_chk.h"
#include "ov10/card_disp.h"
#include "main/nml_model_set.h"
#include "main/xgl_2.h"
#include "main/xgl_studio.h"
#include "main/xgl_font.h"

/* CardSaveData.flags bits this TU uses (cgp.h names CARD_SAVE_ROTATE_STAGE). */
#define CARD_SAVE_LARGE_CARDS 0x20   /* toggled from the title menu */
#define CARD_SAVE_TUNED_CIRCUIT_GIVEN 0x40   /* the first cup won awarded the Tuned Circuit; later cups give a promotional card */


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
extern void CardMainItemGetSub(int *prizes, int round);
extern void CardMainAddItemSub(CardGameWork *work, int *prizes);
extern int CardGameProc(int exiting, CardListWindow *list, CardGameWork *work, int round);
extern void CardGameInit(CardGameWork *work);
extern void xglLightIntensityAmbient(void *light, void *ambient);
extern int xglFlagsGet(int flag, int count);
extern void xglSoundLoadRequestSmd(const char *name, void *buffer);
extern void xglRenderGlobalFadeSet(float red, float green, float blue, float alpha);
extern void CGPRefreshFieldHP(CardGameWork *work);

#endif
