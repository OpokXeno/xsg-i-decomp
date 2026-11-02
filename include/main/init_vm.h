#ifndef INCLUDE_MAIN_INIT_VM_H
#define INCLUDE_MAIN_INIT_VM_H

#include "shared.h"
#include "main/find_native_method.h"
#include "main/jni.h"

extern JavaClass *classClass;
extern JavaClass *classObject;
extern JavaClass *classString;

/* Functions and data used by other translation units. */
extern SceneString *TYPE_Void;
extern JavaClass *classBoolean;
extern JavaClass *classChar;
extern JavaClass *classShort;
extern JavaClass *classInt;
extern JavaClass *classLong;
extern JavaClass *classFloat;
extern JavaClass *classDouble;
extern SceneString *NAME_Constructor;
extern SceneString *NAME_Init;
extern void *initVMThread;
extern SceneString *ATTR_Code;
extern SceneString *ATTR_Exceptions;
extern SceneString *ATTR_LineNumberTable;
extern SceneString *ATTR_ConstantValue;
extern void (*jthreadResetFunc)(void);
/* Never dereferenced here: initVM only clears the list head. */
extern void *jthreadTop;
/* The whole 8-byte record (config/symbols/main.txt), cleared a halfword at
 * a time; no instruction in initVM reads any of it back. */
#define XTK_PEER_GROUP_COUNT 4
extern unsigned short XTK_peerGroup[XTK_PEER_GROUP_COUNT];

#endif /* INCLUDE_MAIN_INIT_VM_H */
