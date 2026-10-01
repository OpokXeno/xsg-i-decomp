/*
 * OV01 original TU 21: 0x00a33648..0x00a337a0 (1 functions)
 */
#include "common.h"
#include "shared.h"
#include "m_ef_create.h"

int MEfCreate(MEfCreateParam *param)
{
    int i;
    MEfObjRecord *object;

    if ((u32)param->type >= MEF_TYPE_COUNT) {
        MOutputDebugStringWarn("MEfCreate: Invalid type (%d)", param->type);
        return 0;
    }

    for (i = 0; i < ntbl_1[param->type]; i++) {
        object = (MEfObjRecord *)MEfObjCreate();
        if (object == 0) {
            MOutputDebugStringWarn("MEfCreate: Couldn't create an Object");
            return 0;
        }

        object->work = *param;

        if (!func_0[param->type](object)) {
            MEfObjDestroy(object);
        }
    }

    return 1;
}
