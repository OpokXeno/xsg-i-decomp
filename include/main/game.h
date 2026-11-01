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

#endif /* INCLUDE_MAIN_GAME_H */
