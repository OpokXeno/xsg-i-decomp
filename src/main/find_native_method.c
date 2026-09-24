#include "common.h"
#include "find_native_method.h"

int strcmp(const char *, const char *);

/* One row of the default_native[] table: a native method's exported name
 * and the address of its native implementation. */
typedef struct NativeMethodEntry {
    const char *name;
    void *function;
} NativeMethodEntry;

extern NativeMethodEntry default_native[];

/*
 * Looks up name in the default_native[] table, which a NULL name terminates,
 * and returns the matching native implementation, or NULL if the table is
 * empty or no entry's name matches.
 */
void *findNativeMethod(const char *name)
{
    NativeMethodEntry *entry = default_native;

    if (entry->name != 0) {
        do {
            if (strcmp(entry->name, name) == 0) {
                return entry->function;
            }
            entry++;
        } while (entry->name != 0);
    }
    return 0;
}

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", resolveNativeMethod);

/*
 * A string constant of a class file's constant pool, as this function reads
 * one: resolveConstants walks the pool's tag bytes, hands every entry tagged
 * 8 to this function and retags it 0x18 once the slot holds the instance
 * built here.
 */
typedef struct StringConstant {
    u8 unmodeled_00[6];
    u16 length;     /* +0x6: the constant's own character count */
    int messageId;  /* +0x8: the message id the constant stands for */
} StringConstant;

/*
 * The value record a java.lang.String instance refers to.  Only the two
 * words written here are modeled: src/main/chr.h documents the same +0x8
 * word as the message id Java_xeno_Chr_talkto and _touchto read back out of
 * a string argument (main 0x003007b8 and 0x00300820).
 */
typedef struct StringValue {
    u8 unmodeled_00[4];
    int length;     /* +0x4: the constant's length, widened to a word */
    int messageId;  /* +0x8: the constant's message id */
} StringValue;

/*
 * The java.lang.String instance itself: the object header newObject seeds,
 * then the value record at +0x4 the same two natives read.
 */
typedef struct StringInstance {
    ObjectHeader header;
    StringValue *value;  /* +0x4 */
} StringInstance;

extern ClassEntry *classString;

/*
 * Turns a string constant into the java.lang.String instance that stands for
 * it and returns it; resolveStaticField and resolveConstants both store the
 * result back into the constant-pool slot they passed.
 */
StringInstance *Const2JavaString(StringConstant *constant)
{
    StringInstance *string;
    StringValue *value;
    int length;
    int messageId;

    value = xmalloc(sizeof(StringValue), 0xE);
    length = constant->length;
    messageId = constant->messageId;
    *(int *)&value->length = length;
    *(int *)&value->messageId = messageId;
    string = newObject(classString);
    string->value = value;
    return string;
}

void *newObject(ClassEntry *clazz)
{
    ObjectHeader *instance = xmalloc(clazz->instanceSize, 0xE);

    instance->classPointer = clazz->classPointer;
    return instance;
}

extern ClassEntry *classClass;

/*
 * Allocates the 0x40-byte class entry findClass, initWrapperClass and
 * lookupArray fill in: a class with no static fields and no instance size of
 * its own yet, carrying the class pointer java.lang.Class itself carries.
 */
ClassEntry *newClass(void)
{
    ClassEntry *clazz;

    clazz = xmalloc(0x40, 0xE);
    clazz->instanceSize = 0;
    clazz->staticFieldCount = 0;
    clazz->classPointer = (void *)*(int *)&classClass->classPointer;
    return clazz;
}

void *newArray(ElementType *elementType, int length)
{
    ArrayHeader *array;
    void *classPointer;

    if (elementType->flags & 0x100) {
        array = xmalloc(elementType->elementSize * length + 0xC, 0xE);
    } else {
        array = xmalloc(length * 4 + 0xC, 0xE);
    }
    classPointer = lookupArray(elementType)->classPointer;
    array->length = length;
    array->classPointer = classPointer;
    array->data = (void *)(array + 1);
    return array;
}

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", processClass);

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", findSuperMethod);

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", buildDispatchMethodTable);

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", resolveStaticField);

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", resolveInstanceField);

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", resolveConstants);

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", loadArray);

void *findClass(ClassEntry *entry);
ClassEntry *lookupClassEntry(int classKey);
int processClass(int classHandle, int state);

int loadClass(int classKey, int skipLoad)
{
    ClassEntry *entry;
    void *resolved;
    int result;

    entry = lookupClassEntry(classKey);
    resolved = entry->resolvedClass;
    if (resolved == 0) {
        if (skipLoad == 0) {
            resolved = findClass(entry);
        }
        if (resolved != 0) {
            entry->resolvedClass = resolved;
        }
    }
    if (resolved == 0) {
        result = 0;
    } else {
        result = (processClass((int)resolved, 0xB) == 0) ? 0 : (int)resolved;
    }
    return result;
}

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", loadStaticClass);

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", reloadClassEntry);

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", lookupClassEntry);

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", lookupArray);

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", classFromSig);

void *classFromSig(const char **cursor);

void getClassFromSignature(const char *signature)
{
    const char *cursor = signature;

    classFromSig(&cursor);
}

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", allocStaticField);

int sizeofDescripter(u8 **cursor);

void methodDescripter(u8 *descriptor, s16 *paramSize, s16 *returnSize, s8 *returnType)
{
    u8 *cursor = descriptor;

    *paramSize = (s16)sizeofDescripter(&cursor);
    *returnType = (s8)*cursor;
    *returnSize = (s16)sizeofDescripter(&cursor);
}

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", sizeofDescripter);

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", instanceOf);

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", sizeofDescripterType);
