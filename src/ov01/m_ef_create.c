/*
 * OV01 original TU 21: 0x00a33648..0x00a337a0 (1 functions)
 */
#include "common.h"
#include "shared.h"
#include "m_ef_create.h"
#include "ov01/m_ef_create_bp_00.h"
#include "ov01/m_ef_create_eac_00.h"
#include "ov01/m_ef_create_amp_02.h"
#include "ov01/m_ef_create_ecm_01.h"
#include "ov01/m_ef_create_ecm_02.h"
#include "ov01/m_ef_create_ead_00.h"
#include "ov01/m_ef_create_so_14.h"
#include "ov01/m_ef_create_kosbw_02.h"

extern int MEfCreate_SOLB(MEfObjRecord *self);

/* Constructor callbacks and instance counts for each effect type. */
static MEfObjCtor func_0[MEF_TYPE_COUNT] = {
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
    MEfCreate_KOSBW02,
};

static short ntbl_1[MEF_TYPE_COUNT] = { 1, 2, 1, 1, 1, 1, 1, 1, 4, 6, 1, 1, 6, 1 };

int MEfCreate(MEfCreateParam *param)
{
    int i;
    MEfObjRecord *object;

    if ((u32)param->fields.type >= MEF_TYPE_COUNT) {
        MOutputDebugStringWarn("MEfCreate: Invalid type (%d)", param->fields.type);
        return 0;
    }

    for (i = 0; i < ntbl_1[param->fields.type]; i++) {
        object = (MEfObjRecord *)MEfObjCreate();
        if (object == 0) {
            MOutputDebugStringWarn("MEfCreate: Couldn't create an Object");
            return 0;
        }

        object->work = *param;

        if (!func_0[param->fields.type](object)) {
            MEfObjDestroy(object);
        }
    }

    return 1;
}
