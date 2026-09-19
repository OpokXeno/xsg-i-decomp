#include "common.h"
#include "shared.h"

/* The object-pool storage remains scaffold-owned; its record layout is unresolved. */
extern u8 mefObjBuff[];
extern u32 mefObjSysFlags;

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

INCLUDE_ASM("asm/main/nonmatchings/m_ef_obj", MEfObjExec1st);

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

INCLUDE_ASM("asm/main/nonmatchings/m_ef_obj", MEfObjCreate);

INCLUDE_ASM("asm/main/nonmatchings/m_ef_obj", MEfObjDestroy);
