/*
 * TU-local declarations of main/tu117 (src/main/game_pause_disp.c).
 */

#ifndef SRC_MAIN_GAME_PAUSE_DISP_H
#define SRC_MAIN_GAME_PAUSE_DISP_H

extern void PauseMenu(void);

extern void GamePauseDispBG(void);

extern unsigned char ShadowEnv[];

/*
 * Only the packet handle at +0x00 is evidenced (DrawShadow). The rest of the
 * structure is populated by DrawCredit, which is still unresolved; this is
 * an explicit partial type and does not claim the layout beyond +0x00.
 */
typedef struct PauseDrawContext {
    void *packet;
} PauseDrawContext;

extern void sceVif1PkAddDirectDataN(void *packet, const void *data, int count);

extern void GamePauseDispCf(void);

extern int xglFontGetStringWidth(const char *text);

extern const char pause_0[];

void GamePauseDispEvent(void);

extern const char msg1_1[];

extern const char msg2_2[];

#endif /* SRC_MAIN_GAME_PAUSE_DISP_H */
