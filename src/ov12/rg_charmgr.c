/*
 * OV12 original TU 21: 0x00a143e0..0x00a14d38 (20 functions)
 */
#include "common.h"
#include "rg_charmgr.h"

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", _FindPtr);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", _EntryPtr);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", _DeletePtr);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", RgCharMgrEntry);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", RgCharMgrFree);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", RgCharMgrSearch);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", RgCharMgrIsFullOfBuffer);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", RgCharMgrGC);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", _RgCharMgrCallControl);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", _RgCharMgrCallDisp);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", _RgCharMgrCallPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", RgCharMgrControl);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", RgCharMgrPassTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", RgCharMgrDisp);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", _DestructCharMgr);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", _DisposeCharMgr);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", InitRgCharMgr);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", InstanceOfRgCharMgr);

RgCharMgr *InstanceOfRgCharMgrClear(void)
{
    RgCharMgr *manager = InstanceOfRgCharMgr();

    _DestructCharMgr(manager);
    return manager;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", RgCharMgrDump);
