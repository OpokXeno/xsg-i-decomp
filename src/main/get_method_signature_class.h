#ifndef GET_METHOD_SIGNATURE_CLASS_RECOVERY_PRIVATE_H
#define GET_METHOD_SIGNATURE_CLASS_RECOVERY_PRIVATE_H

#include "shared.h"
#include "main/find_native_method.h"

typedef struct VMResolvedFieldRef VMResolvedFieldRef;

struct VMResolvedFieldRef {
    JavaClass *class_obj;
    SceneField *field;
    unsigned char unmodeled_08[8];
};

int getField(int index, JavaClass *owner, int is_static,
             VMResolvedFieldRef *ref);

JavaMethod *getMethodSignatureClass(int index, JavaClass *owner);
extern JavaField *lookupClassField(void *class_object, void *name, int flags);
JavaClass *getClass(int index, JavaClass *owner);
extern JavaClass *loadArray(SceneString *name, void *classLoader);
extern JavaClass *loadClass(SceneString *classKey, void *classLoader);

extern JavaMethod *findMethodLocal(JavaClass *class_obj, SceneString *name,
                                    SceneString *type);

#endif /* GET_METHOD_SIGNATURE_CLASS_RECOVERY_PRIVATE_H */
