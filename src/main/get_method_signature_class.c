#include "common.h"
#include "get_method_signature_class.h"

INCLUDE_ASM("asm/main/nonmatchings/get_method_signature_class", getMethodSignatureClass);

INCLUDE_ASM("asm/main/nonmatchings/get_method_signature_class", getField);

INCLUDE_ASM("asm/main/nonmatchings/get_method_signature_class", getClass);

INCLUDE_ASM("asm/main/nonmatchings/get_method_signature_class", lookupClassField);

INCLUDE_ASM("asm/main/nonmatchings/get_method_signature_class", findMethodLocal);

/*
 * Searches class_obj and each superclass in turn (VMClassLink.superclass,
 * +0x10) for a local method matching name/type, returning the first hit
 * from findMethodLocal or 0 once the superclass chain is exhausted.
 */
SceneMethod *findMethod(VMClassLink *class_obj, SceneString *name,
                         SceneClass *type)
{
    SceneMethod *method;

    if (class_obj != 0) {
        do {
            method = findMethodLocal(class_obj, name, type);
            if (method != 0) {
                return method;
            }
            class_obj = class_obj->superclass;
        } while (class_obj != 0);
    }

    return 0;
}
