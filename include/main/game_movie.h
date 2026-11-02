#ifndef INCLUDE_MAIN_GAME_MOVIE_H
#define INCLUDE_MAIN_GAME_MOVIE_H

void GameMovieMain(void);

/* Functions and data used by other translation units. */
void GameMovieStop(void);
int GameMoviePlay(char *name);
void GameMpeg2Play(char *name, int mode);

#endif /* INCLUDE_MAIN_GAME_MOVIE_H */
