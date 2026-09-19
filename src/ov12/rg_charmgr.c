/*
 * OV12 original TU 21: 0x00a143e0..0x00a14d38 (20 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_charmgr.h"

/*
 * RgChar is defined in src/ov12/rg_char.h (another translation unit). The
 * functions this file recovers only pass RgChar pointers through, so the
 * tag is forward-declared here without repeating that TU's member layout.
 */
typedef struct RgChar RgChar;

/*
 * RgCharMgrFree clears the owning-manager field ov12/tu002's own
 * include/ov12/rg_char.h already declares (RgChar::mgr, +0x18), so that
 * TU's header is included here instead of restating the layout.
 */
#include "ov12/rg_char.h"

/*
 * External file-backed witnesses, not candidate-emitted data: this window is
 * asm-owned scaffold data (config/tu/ov12/tu021.json data_ownership window
 * 0x00a53085..0x00a531f0, no config/symbols/ov12.txt entry).
 *
 * ov12:0x00a530b8 contains the source filename "../rg_charmgr.euc.c".
 * ov12:0x00a53108 contains the assertion expression "pChar != NIL".
 * ov12:0x00a53138 contains the assertion expression "pMgr != NIL".
 */
extern const char D_00A530B8[];
extern const char D_00A53108[];
extern const char D_00A53138[];

/*
 * Same window: ov12:0x00a53118 contains the assertion expression
 * "pMgr != NIL && pChar != NIL", RgCharMgrFree's own guard.
 */
extern const char D_00A53118[];

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern RgHeap *InstanceOfRgHeap(void);
extern void RgHeapFree(RgHeap *heap, void *pointer, const char *source_file,
                       int line);

extern unsigned int _EntryPtr(RgCharMgr *manager, unsigned int count,
                              RgChar *pChar);
extern unsigned int _DeletePtr(RgCharMgr *manager, unsigned int count,
                               RgChar *pChar);
/*
 * RgCharFree is defined and declared by ov12/tu002 (src/ov12/rg_char.h);
 * src/ov12/rg_shot.c already redeclares it the same way to call it from a
 * different TU, so this follows that precedent instead of including that
 * TU's own src header.
 */
extern void RgCharFree(RgChar *pChar);
extern void _RgCharMgrCallControl(RgCharMgr *manager);
extern void _RgCharMgrCallPassTime(RgCharMgr *manager);
extern void _RgCharMgrCallDisp(RgCharMgr *manager);

/*
 * The observed access view: up to 256 registered character pointers at
 * +0x000 and how many of them are in use at +0x400 (InstanceOfRgCharMgr
 * allocates exactly 0x404 bytes for it). This TU's own unrecovered
 * _RgCharMgrCallControl, _DestructCharMgr and RgCharMgrGC all read count at
 * +0x400 and index chars[0..count) with sltu-bounded loops; _EntryPtr and
 * _DeletePtr (also unrecovered) take and return the updated count.
 */
struct RgCharMgr {
    RgChar *chars[256]; /* +0x000 */
    unsigned int count; /* +0x400 */
};

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", _FindPtr);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", _EntryPtr);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", _DeletePtr);

RgChar *RgCharMgrEntry(RgCharMgr *manager, RgChar *pChar)
{
    if (pChar == 0) {
        assert_prog(D_00A53108, D_00A530B8, 58);
    }
    manager->count = _EntryPtr(manager, manager->count, pChar);
    return pChar;
}

void RgCharMgrFree(RgCharMgr *manager, RgChar *pChar)
{
    unsigned int count;

    if ((manager == 0) || (pChar == 0)) {
        assert_prog(D_00A53118, D_00A530B8, 66);
    }
    count = _DeletePtr(manager, manager->count, pChar);
    pChar->mgr = 0;
    manager->count = count;
    RgCharFree(pChar);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", RgCharMgrSearch);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", RgCharMgrIsFullOfBuffer);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", RgCharMgrGC);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", _RgCharMgrCallControl);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", _RgCharMgrCallDisp);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", _RgCharMgrCallPassTime);

void RgCharMgrControl(RgCharMgr *manager)
{
    _RgCharMgrCallControl(manager);
}

void RgCharMgrPassTime(RgCharMgr *manager)
{
    _RgCharMgrCallPassTime(manager);
}

void RgCharMgrDisp(RgCharMgr *manager)
{
    _RgCharMgrCallDisp(manager);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", _DestructCharMgr);

static void _DisposeCharMgr(RgCharMgr *manager)
{
    _DestructCharMgr(manager);
    RgHeapFree(InstanceOfRgHeap(), manager, D_00A530B8, 246);
}

void InitRgCharMgr(RgCharMgr *manager)
{
    if (manager == 0) {
        assert_prog(D_00A53138, D_00A530B8, 253);
    }
    manager->count = 0;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", InstanceOfRgCharMgr);

RgCharMgr *InstanceOfRgCharMgrClear(void)
{
    RgCharMgr *manager = InstanceOfRgCharMgr();

    _DestructCharMgr(manager);
    return manager;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", RgCharMgrDump);
