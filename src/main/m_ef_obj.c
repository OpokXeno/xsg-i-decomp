#include "common.h"
#include "shared.h"
#include "main/xgl_studio.h"
#include "m_ef_obj.h"

/* The pool contains 96 records of 0x420 bytes each. */
#define MEFOBJ_COUNT 0x60
#define MEFOBJ_ACTIVE 1

typedef struct MEfObj MEfObj;
typedef void (*MEfObjUpdate)(MEfObj *self, void *work);

struct MEfObj {
    u32 flags;
    MEfObjUpdate exec1st[2];
    MEfObjUpdate exec2nd[2];
    u8 unmodeled_14[12];
    u8 work[0x400];
};

static MEfObj mefObjBuff[MEFOBJ_COUNT];
static u32 mefObjSysFlags = 0;
extern void MOutputDebugStringWarn(const char *format, ...);

void MEfObjInit(void)
{
    memset(mefObjBuff, 0, 0x18c00);
    mefObjSysFlags = 1;
}

#define MEFOBJ_SYS_READY 1
#define MEFOBJ_SYS_ENABLED 2

void MEfObjEnabled(short enabled)
{
    if (enabled != 0) {
        mefObjSysFlags |= MEFOBJ_SYS_ENABLED;
        return;
    }
    mefObjSysFlags &= ~MEFOBJ_SYS_ENABLED;
}

void MEfObjExec1st(void)
{
    if ((mefObjSysFlags & (MEFOBJ_SYS_READY | MEFOBJ_SYS_ENABLED)) ==
        (MEFOBJ_SYS_READY | MEFOBJ_SYS_ENABLED)) {
        StudioCamera *camera;
        Vector4 *rotation;
        Matrix4 *viewMatrix;
        MEfObj *objects;
        int index;

        camera = xglStudioGetCamera2(0);
        rotation = &camera->rotation;
        viewMatrix = &camera->viewMatrix;
        mefCamParams.screen = camera;
        mefCamParams.matrix = viewMatrix;
        mefCamParams.rotation = rotation;
        MMathRotateMatrixYXZ(&mefCamParams.basis, 0, rotation);

        objects = mefObjBuff;
        for (index = 0; index < MEFOBJ_COUNT; index++) {
            MEfObj *obj = &objects[index];
            if (obj->flags & MEFOBJ_ACTIVE) {
                if (obj->exec1st[0] != 0) {
                    obj->exec1st[0](obj, obj->work);
                }
                if (obj->exec1st[1] != 0) {
                    obj->exec1st[1](obj, obj->work);
                }
            }
        }
    }
}

void MEfObjExec2nd(void)
{
    if ((mefObjSysFlags & (MEFOBJ_SYS_READY | MEFOBJ_SYS_ENABLED)) ==
        (MEFOBJ_SYS_READY | MEFOBJ_SYS_ENABLED)) {
        MEfObj *objects = mefObjBuff;
        int index;

        for (index = 0; index < MEFOBJ_COUNT; index++) {
            MEfObj *obj = &objects[index];
            if (obj->flags & MEFOBJ_ACTIVE) {
                if (obj->exec2nd[0] != 0) {
                    obj->exec2nd[0](obj, obj->work);
                }
                if (obj->exec2nd[1] != 0) {
                    obj->exec2nd[1](obj, obj->work);
                }
            }
        }
    }
}

void *MEfObjCreate(void)
{
    MEfObj *object;
    int index;

    object = mefObjBuff;
    if ((mefObjSysFlags & MEFOBJ_SYS_READY) == 0) {
        MOutputDebugStringWarn("MEfObjCreate: MEfObj is not be initialized");
        return 0;
    }

    for (index = 0; index < MEFOBJ_COUNT; index++) {
        if ((object->flags & MEFOBJ_ACTIVE) == 0) {
            object->exec1st[0] = 0;
            object->flags = MEFOBJ_ACTIVE;
            object->exec1st[1] = 0;
            object->exec2nd[0] = 0;
            object->exec2nd[1] = 0;
            return object;
        }
        object = &object[1];
    }

    MOutputDebugStringWarn("MEfObjCreate: Failed to create an Object");
    return 0;
}

int MEfObjDestroy(void *object)
{
    MEfObj *effect = object;

    if ((mefObjSysFlags & MEFOBJ_SYS_READY) == 0) {
        MOutputDebugStringWarn("MEfObjDestroy: MEfObj is not be initialized");
        return 0;
    }

    if ((effect->flags & MEFOBJ_ACTIVE) == 0) {
        MOutputDebugStringWarn("MEfObjDestroy: Not alive");
        return 0;
    }

    effect->exec1st[0] = 0;
    effect->exec1st[1] = 0;
    effect->exec2nd[0] = 0;
    effect->flags &= ~MEFOBJ_ACTIVE;
    effect->exec2nd[1] = 0;
    return 1;
}
