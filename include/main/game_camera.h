#ifndef INCLUDE_MAIN_GAME_CAMERA_H
#define INCLUDE_MAIN_GAME_CAMERA_H

void GameCameraStateSave(void);
void GameCameraStateRestore(void);

/* Functions and data used by other translation units. */
void GameCameraChangeID(int id, int unused1, int unused2);
void GameCameraReset(void);
void GameCamera(void);

#endif /* INCLUDE_MAIN_GAME_CAMERA_H */
