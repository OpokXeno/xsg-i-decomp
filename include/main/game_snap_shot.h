#ifndef INCLUDE_MAIN_GAME_SNAP_SHOT_H
#define INCLUDE_MAIN_GAME_SNAP_SHOT_H

#include "main/xgl_2.h"

void GameSnapShotCheck(void);

/* Functions and data used by other translation units. */
extern char GameMovieTransparent;
extern char GameMovieAlpha;
extern short GameMovieFrame;
unsigned int GameSnapShotSaveThumbnail(int quality, unsigned int output_buffer);
int GameSnapShotNumber(int number);
int GameSnapShotSaveFile(int number, const void *data, int size);

#endif /* INCLUDE_MAIN_GAME_SNAP_SHOT_H */
