#ifndef INCLUDE_MAIN_FIND_NATIVE_METHOD_H
#define INCLUDE_MAIN_FIND_NATIVE_METHOD_H

#include "shared.h"

typedef signed char s8;

typedef struct JavaClass JavaClass;

typedef struct ClassNameKey ClassNameKey;

typedef struct ElementType {
    u8 unmodeled_00[4];
    ClassNameKey *name; /* +0x4: lookupArray reads the class-name bytes */
    u8 unmodeled_08[2];
    u16 flags;           /* +0xa: bit 0x100 marks a byte-sized element type */
    void *classLoader; /* +0xc: forwarded to lookupClassEntry */
    u8 unmodeled_10[0x29];
    s8 typeCode; /* +0x39: signed character supplied to the primitive name format */
    u8 elementSize;       /* +0x3a: per-element byte size for byte-sized elements */
} ElementType;

typedef struct ObjectHeader {
    void *classPointer;
} ObjectHeader;

typedef struct JavaMethod JavaMethod;

typedef struct JavaFieldEntry FieldEntry;

typedef struct DispatchTable DispatchTable;

typedef union ConstantPoolEntry ConstantPoolEntry;

struct JavaClass {
    ObjectHeader header;
    ClassNameKey *name;
    u16 accessFlags;
    u16 flags;
    void *classLoader;
    JavaClass *superClass;
    ConstantPoolEntry *constants;
    DispatchTable *classPointer;
    FieldEntry *fields;
    JavaMethod *methods;
    JavaClass **interfaces;
    u16 constantCount;
    u16 dispatchSize;
    u16 methodCount;
    u16 interfaceCount;
    u16 fieldCount;
    u16 staticFieldCount;
    u16 declaredInterfaceCount;
    s16 state;
    int instanceSize;
    ElementType *elementClass;
};

void *newObject(SceneClass *scene_class);
void *newClass(void);
void loadStaticClass(void *class_slot, const char *name);

#endif /* INCLUDE_MAIN_FIND_NATIVE_METHOD_H */
