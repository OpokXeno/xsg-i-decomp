/*
 * TU-local declarations of main/tu223 (src/main/init_class_db.c).
 */

#ifndef SRC_MAIN_INIT_CLASS_DB_H
#define SRC_MAIN_INIT_CLASS_DB_H

#include "shared.h"

typedef struct ClassDescriptor ClassDescriptor;

typedef u32 ConstantPoolWord;

/*
 * This is the runtime class-file descriptor used by the native loader.
 */
struct ClassDescriptor {
    void *object_methods;
    void * volatile class_name;
    u16 volatile access_flags;
    u16 type_flags;
    u32 volatile class_loader;
    u32 volatile super_class_index;
    ConstantPoolWord *constant_pool;
    void *dispatch_methods;
    void *fields;
    void *methods;
    void *interfaces;
    u16 constant_pool_count;
    u16 dispatch_method_count;
    u16 method_count;
    u16 interface_class_count;
    u16 field_count;
    u16 static_field_count;
    u16 interface_count;
    u16 volatile processing_state;
    u32 volatile instance_size;
    ClassDescriptor *component_class;
};

void setupClass(ClassDescriptor *class_info, u32 this_class_index,
                u32 super_class_index, u32 access_flags,
                u32 class_loader);

#endif /* SRC_MAIN_INIT_CLASS_DB_H */
