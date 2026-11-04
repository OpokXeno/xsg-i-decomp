#ifndef INCLUDE_OV12_RG_SINGLETON_ID_H
#define INCLUDE_OV12_RG_SINGLETON_ID_H

#include "shared.h"

/* Singleton slots, named after the object each accessor creates. */
#define RG_SINGLETON_CHAR_MANAGER 0
#define RG_SINGLETON_SHOT_DB 1
#define RG_SINGLETON_WEAPON_DB 2
#define RG_SINGLETON_ROBOT_DB 3
#define RG_SINGLETON_FILESYS 5
#define RG_SINGLETON_GLOBAL_LIGHT 6
#define RG_SINGLETON_PARTICLE_DRIVER 7
#define RG_SINGLETON_EFFECT_ENV 8
#define RG_SINGLETON_DRAW 9
#define RG_SINGLETON_PAINT2D 10
#define RG_SINGLETON_BATTLE_COMMON_DATA 11
#define RG_SINGLETON_MOTION_INFO_DB 12
#define RG_SINGLETON_DEBUG_FLAGS 13
#define RG_SID_MAX 15 /* slot count of the singleton registry */

void *RgSingletonIDGet(unsigned int singleton_id);
void RgSingletonIDClear(void);
void RgSingletonDispose(void);

/* Registers a singleton with the destructor that disposes of it. */
void RgSingletonIDEntry(int singleton_id, void *instance,
                        void (*destructor)(void *instance));

#endif /* INCLUDE_OV12_RG_SINGLETON_ID_H */
