#ifndef GET_METHOD_SIGNATURE_CLASS_RECOVERY_PRIVATE_H
#define GET_METHOD_SIGNATURE_CLASS_RECOVERY_PRIVATE_H

#include "shared.h"

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
};

extern SceneMethod *findMethodLocal(VMClassLink *class_obj, SceneString *name,
                                     SceneClass *type);

#endif /* GET_METHOD_SIGNATURE_CLASS_RECOVERY_PRIVATE_H */
