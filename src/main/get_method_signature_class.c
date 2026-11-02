#include "common.h"
#include "get_method_signature_class.h"
#include "main/get_method_signature_class.h"

JavaMethod *getMethodSignatureClass(int index, JavaClass *owner)
{
    ConstantPoolEntry *constant_pool;
    ConstantPoolEntry *entries;
    JavaClass *class_obj;
    SceneString *name;
    SceneString *type;
    JavaMethod *method;

    constant_pool = owner->constants;
    if ((unsigned int)(constant_pool[0].tags[index] - 10) >= 2) {
        return 0;
    }

    entries = constant_pool;
    name = entries[entries[entries[index].memberRef.nameAndTypeIndex]
                       .nameAndType.nameIndex].object;
    type = entries[entries[entries[index].memberRef.nameAndTypeIndex]
                       .nameAndType.typeIndex].object;
    class_obj = getClass(entries[index].memberRef.classIndex, owner);
    if (class_obj == 0) {
        return 0;
    }

    method = 0;
    while (class_obj != 0) {
        method = findMethodLocal(class_obj, name, type);
        if (method != 0) {
            break;
        }
        class_obj = class_obj->superClass;
    }
    return method;
}

int getField(int index, JavaClass *owner, int is_static,
             VMResolvedFieldRef *ref)
{
    ConstantPoolEntry *constant_pool;
    ConstantPoolEntry *entries;
    JavaClass *class_obj;
    SceneString *name;
    JavaField *field;

    constant_pool = owner->constants;
    entries = constant_pool;
    is_static &= 0xff;
    if (constant_pool[0].tags[index] != 9) {
        return 0;
    }

    class_obj = getClass(entries[index].memberRef.classIndex, owner);
    if (class_obj == 0) {
        return 0;
    }

    name = entries[entries[entries[index].memberRef.nameAndTypeIndex]
                       .nameAndType.nameIndex].object;
    field = lookupClassField(class_obj, name, is_static);
    if (field == 0) {
        return 0;
    }

    ref->class_obj = class_obj;
    ref->field = (SceneField *)field;
    return 1;
}

JavaClass *getClass(int index, JavaClass *owner)
{
    ConstantPoolEntry *constant_pool;
    ConstantPoolEntry *entries;
    SceneString *class_name;
    JavaClass *class_obj;
    unsigned char tag;

    constant_pool = owner->constants;
    entries = constant_pool;
    tag = constant_pool[0].tags[index];
    if (tag == 23) {
        return entries[index].object;
    }
    if (tag != 7) {
        return 0;
    }

    class_name = entries[index].object;
    if (class_name->bytes[0] == '[') {
        class_obj = loadArray(class_name, owner->classLoader);
    } else {
        class_obj = loadClass(entries[index].object, owner->classLoader);
    }
    if (class_obj == 0) {
        return 0;
    }

    entries[index].object = class_obj;
    constant_pool[0].tags[index] = 23;
    return class_obj;
}

JavaField *lookupClassField(void *class_object, void *name, int flags)
{
    JavaClass *class_link;
    SceneField *field;
    SceneString *field_name;
    int remaining;

    class_link = class_object;
    field_name = name;
    flags &= 0xff;
    if (flags != 0) {
        remaining = class_link->staticFieldCount;
        field = class_link->fields;
    } else {
        unsigned short static_count = class_link->staticFieldCount;
        unsigned short field_count = class_link->fieldCount;
        field = &class_link->fields[static_count];
        remaining = field_count - static_count;
    }

    remaining--;
    while (remaining >= 0) {
        if (field->name == field_name) {
            return (JavaField *)field;
        }
        field++;
        remaining--;
    }
    return 0;
}

JavaMethod *findMethodLocal(JavaClass *class_obj, SceneString *name,
                            SceneString *type)
{
    JavaMethod *method_entry;
    int remaining;

    method_entry = class_obj->methods;
    remaining = class_obj->methodCount;
    remaining--;
    while (remaining >= 0) {
        if (method_entry->name == name && method_entry->descriptor == type) {
            return method_entry;
        }
        method_entry++;
        remaining--;
    }

    return 0;
}

/*
 * Searches class_obj and each superclass in turn (JavaClass.superClass,
 * +0x10) for a local method matching name/type, returning the first hit
 * from findMethodLocal or 0 once the superclass chain is exhausted.
 */
JavaMethod *findMethod(JavaClass *class_obj, SceneString *name,
                       SceneString *type)
{
    JavaMethod *method;

    if (class_obj != 0) {
        do {
            method = findMethodLocal(class_obj, name, type);
            if (method != 0) {
                return method;
            }
            class_obj = class_obj->superClass;
        } while (class_obj != 0);
    }

    return 0;
}
