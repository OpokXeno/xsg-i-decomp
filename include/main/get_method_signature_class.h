#ifndef INCLUDE_MAIN_GET_METHOD_SIGNATURE_CLASS_H
#define INCLUDE_MAIN_GET_METHOD_SIGNATURE_CLASS_H

#include "shared.h"

/* Functions and data used by other translation units. */
extern JavaField *lookupClassField(void *class_object, void *name, int flags);
JavaMethod *findMethod(JavaClass *class_obj, SceneString *name, SceneString *type);
JavaClass *getClass(int index, JavaClass *owner);
extern JavaMethod *findMethodLocal(JavaClass *class_obj, SceneString *name,
                                    SceneString *type);
int getField(int index, JavaClass *owner, int is_static,
             VMResolvedFieldRef *ref);
JavaMethod *getMethodSignatureClass(int index, JavaClass *owner);

#endif /* INCLUDE_MAIN_GET_METHOD_SIGNATURE_CLASS_H */
