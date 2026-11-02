#ifndef INCLUDE_MAIN_SCRIPT_H
#define INCLUDE_MAIN_SCRIPT_H

#include "main/jni.h"

/*
 * Queue a funcObserver task that invokes the no-argument script method
 * `method_name` ("()V") on the current Stage object, then run it once.
 * Every caller ignores $v0, so the entry point returns nothing.
 */
extern void CallMethod(const char *method_name);

/*
 * Queue a funcObserver task that invokes the one-int-argument script method
 * `method_name` ("(I)V") on the current Stage object, then run it once.
 * Every caller ignores $v0, so the entry point returns nothing.
 */
extern void CallMethod_I(const char *method_name, int arg1);

int SCRIPT_getCfTime(void);
void SCRIPT_execTalkto(void *actor);
int SCRIPT_load2(const char *path);

/* Functions and data used by other translation units. */
ScriptDbEntry *SCRIPT_exec(void);
void XTK_setWindowOwner(void *owner);
void *XTK_getWindowOwner(void);
void SCRIPT_fade(int time);
void SCRIPT_load_DBG(const char *path);

#endif /* INCLUDE_MAIN_SCRIPT_H */
