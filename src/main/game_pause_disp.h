/*
 * TU-local declarations of main/tu117 (src/main/game_pause_disp.c).
 */

#ifndef SRC_MAIN_GAME_PAUSE_DISP_H
#define SRC_MAIN_GAME_PAUSE_DISP_H

extern void PauseMenu(void);

extern void GamePauseDispBG(void);

extern unsigned char ShadowEnv[];

extern void GamePauseDispCf(void);

extern int xglFontGetStringWidth(const char *text);

extern const char pause_0[];

void GamePauseDispEvent(void);

extern const char msg1_1[];

extern const char msg2_2[];

#endif /* SRC_MAIN_GAME_PAUSE_DISP_H */
