#ifndef INCLUDE_MAIN_SCENE_1_H
#define INCLUDE_MAIN_SCENE_1_H

#include "shared.h"

/*
 * SCENE_instance only ever writes the window's word 0 (0x0025a310); nothing
 * in this TU reads or writes any other window offset, so the window is
 * modeled as just the shared object header.
 */
struct SceneWindow {
    SceneObjectHeader header;
};

#endif /* INCLUDE_MAIN_SCENE_1_H */
