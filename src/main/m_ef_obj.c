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

INCLUDE_ASM("asm/main/nonmatchings/m_ef_obj", MEfObjEnabled);

INCLUDE_ASM("asm/main/nonmatchings/m_ef_obj", MEfObjExec1st);

INCLUDE_ASM("asm/main/nonmatchings/m_ef_obj", MEfObjExec2nd);

INCLUDE_ASM("asm/main/nonmatchings/m_ef_obj", MEfObjCreate);

INCLUDE_ASM("asm/main/nonmatchings/m_ef_obj", MEfObjDestroy);
