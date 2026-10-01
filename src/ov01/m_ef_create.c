/*
 * OV01 original TU 21: 0x00a33648..0x00a337a0 (1 functions)
 */
#include "common.h"
#include "shared.h"
#include "m_ef_create.h"

int MEfCreate(MEfCreateParam *param)
{
    /* Original local .data symbols func.0 (+0x00) and ntbl.1 (+0x38).
     * The callback type is checked against every constructor declaration. */
    static MEfObjCtor func[MEF_TYPE_COUNT] = {
        MEfCreate_MSP00,
        MEfCreate_BP00,
        MEfCreate_SMP01,
        MEfCreate_EAC00,
        MEfCreate_AMP02,
        MEfCreate_MSP02,
        MEfCreate_SOLB,
        MEfCreate_ECM01,
        MEfCreate_ECM02,
        MEfCreate_GAMERA,
        MEfCreate_EAD00,
        MEfCreate_SO14,
        MEfCreate_DORA,
        MEfCreate_KOSBW02
    };
    static short ntbl[MEF_TYPE_COUNT] = {
        1, 2, 1, 1, 1, 1, 1, 1, 4, 6, 1, 1, 6, 1
    };
    int i;
    MEfObjRecord *object;

    if ((u32)param->fields.type >= MEF_TYPE_COUNT) {
        MOutputDebugStringWarn("MEfCreate: Invalid type (%d)", param->fields.type);
        return 0;
    }

    for (i = 0; i < ntbl[param->fields.type]; i++) {
        object = (MEfObjRecord *)MEfObjCreate();
        if (object == 0) {
            MOutputDebugStringWarn("MEfCreate: Couldn't create an Object");
            return 0;
        }

        object->work = *param;

        if (!func[param->fields.type](object)) {
            MEfObjDestroy(object);
        }
    }

    return 1;
}
