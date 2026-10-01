#include "common.h"
#include "get_method_signature_class.h"

SceneMethod *getMethodSignatureClass(int index, VMConstantOwner *owner)
{
    VMConstantPoolEntry *constant_pool;
    VMConstantPoolEntry *entries;
    VMClassLink *class_obj;
    SceneString *name;
    SceneClass *type;
    SceneMethod *method;

    constant_pool = owner->constants;
    if ((unsigned int)(constant_pool[0].tags[index] - 10) >= 2) {
        return 0;
    }

    entries = constant_pool;
    name = entries[entries[entries[index].member_ref.name_and_type_index]
                       .name_and_type.name_index].object;
    type = entries[entries[entries[index].member_ref.name_and_type_index]
                       .name_and_type.type_index].object;
    class_obj = (VMClassLink *)getClass(entries[index].member_ref.class_index,
                                        owner);
    if (class_obj == 0) {
        return 0;
    }

    method = 0;
    while (class_obj != 0) {
        method = findMethodLocal(class_obj, name, type);
        if (method != 0) {
            break;
        }
        class_obj = class_obj->superclass;
    }
    return method;
}

int getField(int index, VMConstantOwner *owner, int is_static,
             VMResolvedFieldRef *ref)
{
    VMConstantPoolEntry *constant_pool;
    VMConstantPoolEntry *entries;
    SceneClass *class_obj;
    SceneString *name;
    JavaField *field;

    constant_pool = owner->constants;
    entries = constant_pool;
    is_static &= 0xff;
    if (constant_pool[0].tags[index] != 9) {
        return 0;
    }

    class_obj = getClass(entries[index].member_ref.class_index, owner);
    if (class_obj == 0) {
        return 0;
    }

    name = entries[entries[entries[index].member_ref.name_and_type_index]
                       .name_and_type.name_index].object;
    field = lookupClassField(class_obj, name, is_static);
    if (field == 0) {
        return 0;
    }

    ref->class_obj = class_obj;
    ref->field = (SceneField *)field;
    return 1;
}

SceneClass *getClass(int index, VMConstantOwner *owner)
{
    VMConstantPoolEntry *constant_pool;
    VMConstantPoolEntry *entries;
    VMClassNameRef *class_name;
    SceneClass *class_obj;
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
    if (class_name->name[0] == '[') {
        class_obj = loadArray(class_name, owner->class_loader);
    } else {
        class_obj = (SceneClass *)loadClass((int)entries[index].value,
                                            owner->class_loader);
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
    VMClassLink *class_link;
    VMFieldEntry *field;
    SceneString *field_name;
    int remaining;

    class_link = class_object;
    field_name = name;
    flags &= 0xff;
    if (flags != 0) {
        remaining = class_link->static_field_count;
        field = class_link->fields;
    } else {
        unsigned short static_count = class_link->static_field_count;
        unsigned short field_count = class_link->field_count;
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

SceneMethod *findMethodLocal(VMClassLink *class_obj, SceneString *name,
                             SceneClass *type)
{
    VMMethodEntry *method_entry;
    int remaining;

    method_entry = class_obj->methods;
    remaining = class_obj->method_count;
    remaining--;
    while (remaining >= 0) {
        if (method_entry->name == name && method_entry->type == type) {
            return (SceneMethod *)method_entry;
        }
        method_entry++;
        remaining--;
    }

    return 0;
}

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
