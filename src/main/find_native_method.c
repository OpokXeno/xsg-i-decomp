#include "common.h"
#include "find_native_method.h"

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", findNativeMethod);

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", resolveNativeMethod);

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", Const2JavaString);

void *newObject(ClassEntry *clazz)
{
    ObjectHeader *instance = xmalloc(clazz->instanceSize, 0xE);

    instance->classPointer = clazz->classPointer;
    return instance;
}

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", newClass);

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

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", loadClass);

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", loadStaticClass);

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", reloadClassEntry);

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", lookupClassEntry);

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", lookupArray);

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", classFromSig);

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", getClassFromSignature);

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", allocStaticField);

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", methodDescripter);

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", sizeofDescripter);

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", instanceOf);

INCLUDE_ASM("asm/main/nonmatchings/find_native_method", sizeofDescripterType);
