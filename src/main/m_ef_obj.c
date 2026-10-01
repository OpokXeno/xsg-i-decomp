#include "common.h"
#include "shared.h"
#include "main/xgl_studio.h"
#include "m_ef_obj.h"

/* The object-pool storage remains scaffold-owned; its record layout is unresolved. */
extern u8 mefObjBuff[];
extern u32 mefObjSysFlags;
extern void MOutputDebugStringWarn(const char *format, ...);
extern const char D_004CC990[];
extern const char D_004CC9C0[];
extern const char D_004CC9F0[];
extern const char D_004CCA20[];

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

/* mefObjBuff holds MEFOBJ_COUNT fixed-size object records (MEfObjInit clears
 * MEFOBJ_COUNT * 0x420 bytes); only the fields the pass loops touch are named. */
#define MEFOBJ_COUNT 0x60
#define MEFOBJ_ACTIVE 1

typedef struct MEfObj MEfObj;
typedef void (*MEfObjUpdate)(MEfObj *self, void *work);

struct MEfObj {
    u32 flags; /* bit 0: object active */
    MEfObjUpdate exec1st[2]; /* called in order by MEfObjExec1st */
    MEfObjUpdate exec2nd[2]; /* called in order by MEfObjExec2nd */
    u8 unmodeled_14[12];
    u8 work[0x400];
};

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

        objects = (MEfObj *)mefObjBuff;
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
        MEfObj *objects = (MEfObj *)mefObjBuff;
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

    object = (MEfObj *)mefObjBuff;
    if ((mefObjSysFlags & MEFOBJ_SYS_READY) == 0) {
        MOutputDebugStringWarn(D_004CC990);
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

    MOutputDebugStringWarn(D_004CC9C0);
    return 0;
}

int MEfObjDestroy(void *object)
{
    MEfObj *effect = object;

    if ((mefObjSysFlags & MEFOBJ_SYS_READY) == 0) {
        MOutputDebugStringWarn(D_004CC9F0);
        return 0;
    }

    if ((effect->flags & MEFOBJ_ACTIVE) == 0) {
        MOutputDebugStringWarn(D_004CCA20);
        return 0;
    }

    effect->exec1st[0] = 0;
    effect->exec1st[1] = 0;
    effect->exec2nd[0] = 0;
    effect->flags &= ~MEFOBJ_ACTIVE;
    effect->exec2nd[1] = 0;
    return 1;
}
