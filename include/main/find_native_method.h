#ifndef INCLUDE_MAIN_FIND_NATIVE_METHOD_H
#define INCLUDE_MAIN_FIND_NATIVE_METHOD_H

#include "shared.h"

/*
 * Allocates an instance of `scene_class` (instance_size bytes) and seeds its
 * header word with the class's instance_class_ref (main 0x002f4828).
 */
void *newObject(SceneClass *scene_class);

/*
 * Allocates a fresh 0x40-byte class record with no static fields and no
 * instance size yet, carrying java.lang.Class's own class reference.  The
 * callers fill it through their own views of the record (initWrapperClass,
 * the class-file loader, lookupArray), so it is returned untyped like any
 * other allocation.
 */
void *newClass(void);

/*
 * Resolves the class `name` (loading it through the class cache when it is
 * not resolved yet), stores it in the caller's class slot and initializes it
 * (main 0x002f5120).  `class_slot` is the address of the caller's cached
 * class pointer, whatever view of the class record the caller keeps there:
 * jni.c's xeno.* slots and init_vm.c's classString are SceneClass pointers,
 * init_vm.c's classObject a JavaClass pointer, and find_native_method.c
 * itself writes the slot as a JavaClass pointer.
 */
void loadStaticClass(void *class_slot, const char *name);

#endif /* INCLUDE_MAIN_FIND_NATIVE_METHOD_H */
