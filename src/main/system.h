#ifndef SRC_MAIN_SYSTEM_H
#define SRC_MAIN_SYSTEM_H

#include "shared.h"
#include "main/jni.h"
#include "main/init_vm.h"
#include "main/scene_1.h"
#include "main/jthread.h"
#include "main/twsys_init.h"


/* Native argument/result slots: main/tu226, src/main/init_vm.c. */

extern char tmpString[];
extern const char systemFloatFormat[];

extern char *strncpy(char *destination, const char *source, unsigned int length);
extern int JNI_getRegister(int register_index);
#endif
