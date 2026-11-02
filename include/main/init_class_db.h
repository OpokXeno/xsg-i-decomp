#ifndef INCLUDE_MAIN_INIT_CLASS_DB_H
#define INCLUDE_MAIN_INIT_CLASS_DB_H

#include "shared.h"

/* Functions and data used by other translation units. */
extern ConstString **constStringTable;
/*
 * classDB (0x0099d430, size 0x20) is the native loader's 8-slot table of
 * resolved classes indexed by class id (JNI_resolveClass in
 * src/main/jni.c); initClassDB clears every slot at JNI_initSystem startup.
 */
#define VM_CLASS_DB_SLOTS 8
extern int classDB[VM_CLASS_DB_SLOTS];

/* JNI_loadClassDB: ids at or above this read slot (id & VM_CLASS_DB_SLOT_MASK); below it they store; negative ids pick a free slot. */
#define VM_CLASS_DB_READ 0x100
#define VM_CLASS_DB_SLOT_MASK 0xFF
void initClassDB(void);

#endif /* INCLUDE_MAIN_INIT_CLASS_DB_H */
