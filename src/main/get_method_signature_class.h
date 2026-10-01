#ifndef GET_METHOD_SIGNATURE_CLASS_RECOVERY_PRIVATE_H
#define GET_METHOD_SIGNATURE_CLASS_RECOVERY_PRIVATE_H

#include "shared.h"

typedef union VMConstantPoolEntry VMConstantPoolEntry;
union VMConstantPoolEntry {
    void *object;
    unsigned char *tags;
    unsigned int value;
    struct {
        unsigned short class_index;
        unsigned short name_and_type_index;
    } member_ref;
    struct {
        unsigned short name_index;
        unsigned short type_index;
    } name_and_type;
};

typedef struct VMConstantOwner VMConstantOwner;
struct VMConstantOwner {
    unsigned char unmodeled_00[0x0c];
    int class_loader;
    unsigned char unmodeled_10[0x04];
    VMConstantPoolEntry *constants;
};

typedef struct VMClassNameRef VMClassNameRef;
struct VMClassNameRef {
    unsigned char unmodeled_00[0x08];
    const char *name;
};

typedef struct VMResolvedFieldRef VMResolvedFieldRef;
struct VMResolvedFieldRef {
    SceneClass *class_obj;
    SceneField *field;
    unsigned char unmodeled_08[8];
};

int getField(int index, VMConstantOwner *owner, int is_static,
             VMResolvedFieldRef *ref);
SceneMethod *getMethodSignatureClass(int index, VMConstantOwner *owner);
extern JavaField *lookupClassField(void *class_object, void *name, int flags);

SceneClass *getClass(int index, VMConstantOwner *owner);
extern SceneClass *loadArray(void *class_descriptor, int skip_load);
extern int loadClass(int class_key, int skip_load);

typedef struct VMMethodEntry VMMethodEntry;
struct VMMethodEntry {
    SceneString *name;
    SceneClass *type;
    unsigned char unmodeled_08[0x18];
};

typedef struct VMFieldEntry VMFieldEntry;
/* Lookup compares the name at +0 in each 20-byte class field entry. */
struct VMFieldEntry {
    SceneString *name;
    unsigned char unmodeled_04[0x10];
};

/*
 * Partial view of the VM class object findMethod walks through the
 * superclass chain: only the pointer at +0x10 this function reads. The
 * real object is very likely the same class record already published as
 * SceneClass (include/shared.h), whose +0x10 falls inside its currently
 * unmodeled_10[8] span; this TU does not define SceneClass, so the
 * superclass link is modeled locally here instead of extending it.
 */
typedef struct VMClassLink VMClassLink;
struct VMClassLink {
    unsigned char unmodeled_00[0x10];
    VMClassLink *superclass;
    unsigned char unmodeled_14[0x08];
    VMFieldEntry *fields;
    VMMethodEntry *methods;
    unsigned char unmodeled_24[0x08];
    unsigned short method_count;
    unsigned char unmodeled_2e[2];
    unsigned short field_count;
    unsigned short static_field_count;
};

extern SceneMethod *findMethodLocal(VMClassLink *class_obj, SceneString *name,
                                     SceneClass *type);

#endif /* GET_METHOD_SIGNATURE_CLASS_RECOVERY_PRIVATE_H */
