#include "common.h"

/* Only the effect type and the 0x70-byte parameter copy are understood here.
 * MEfObjCreate returns a pool object with its work area at offset 0x20. */
typedef struct MEfCreateParams {
    unsigned int type;
    unsigned int unmodeled_04;
    unsigned long long unmodeled_08[13];
} MEfCreateParams;

typedef struct MEfCreateObject {
    unsigned char unmodeled_00[0x20];
    MEfCreateParams parameters;
} MEfCreateObject;

extern MEfCreateObject *MEfObjCreate(void);
extern void MEfObjDestroy(void *self);
extern void MOutputDebugStringWarn(const char *format, ...);
/* The dispatch/count tables and warning strings stay scaffold-owned. */
extern int (*func_0[14])(void *self);
extern short ntbl_1[14];
extern const char D_00A51220[];
extern const char D_00A51240[];

int MEfCreate(MEfCreateParams *parameters)
{
    int index;
    MEfCreateObject *object;

    if (parameters->type >= 14) {
        MOutputDebugStringWarn(D_00A51220, parameters->type);
        return 0;
    }

    for (index = 0; index < ntbl_1[parameters->type]; index++) {
        object = MEfObjCreate();
        if (object == 0) {
            MOutputDebugStringWarn(D_00A51240);
            return 0;
        }
        object->parameters = *parameters;
        if (func_0[parameters->type](object) == 0) {
            MEfObjDestroy(object);
        }
    }
    return 1;
}
