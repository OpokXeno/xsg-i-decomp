#ifndef INCLUDE_MAIN_ENEMY_SYSTEM_INIT_H
#define INCLUDE_MAIN_ENEMY_SYSTEM_INIT_H

#include "shared.h"

/* Functions and data used by other translation units. */
ActorEnemyInitView *ACT_createEnemy(int number, int id);
void Enemy_SystemInit(void);

#endif /* INCLUDE_MAIN_ENEMY_SYSTEM_INIT_H */
