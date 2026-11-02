#ifndef INCLUDE_MAIN_GAME_H
#define INCLUDE_MAIN_GAME_H

#include "main/get.h"
#include "main/init_vm.h"
#include "main/party.h"
#include "main/sef.h"
#include "main/xgl_2.h"
#include "main/xgl_studio.h"
#include "main/xgl_task.h"

void MenuGameDataPush(void);
void MenuGameDataPop(void);
void GamePushSaveDataUser(void);
void Game(void);

/* Functions and data used by other translation units. */
extern unsigned char UseTestPath;
extern unsigned char SnapDrawCreditFlag;
void GameStateSave(void);
void GameStateRestore(void);
void checkTalkPoint(void);
int getScriptFlag(SceneObject object);
void GameDrawSync(void);
/* Private defining-owner proposals from init_vm.c198..235 and325..333.
 * This is the interpreter's genuine slot union, copied verbatim. Both
 * integer arguments initialize its i member; no inactive member is read.
 * Native dispatch supplies thread, arguments and result. The normal owner
 * header currently does not carry these declarations. */
void GameStateSaveCameraLight(void);
void InitCfSystem(void);

#endif /* INCLUDE_MAIN_GAME_H */
