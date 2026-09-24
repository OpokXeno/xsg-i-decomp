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
 * RgSingletonIDGet/RgSingletonIDEntry are defined and declared by their own
 * owning TU (include/ov12/rg_singleton_id.h); InstanceOfRgCharMgr uses the
 * registry to keep one lazily-allocated RgCharMgr singleton.
 */
#include "ov12/rg_singleton_id.h"

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
extern void *RgHeapAlloc(RgHeap *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgHeapFree(RgHeap *heap, void *pointer, const char *source_file,
                       int line);

static unsigned int _EntryPtr(RgCharMgr *manager, unsigned int count,
                              RgChar *pChar);
static unsigned int _DeletePtr(RgCharMgr *manager, unsigned int count,
                               RgChar *pChar);
/*
 * RgCharFree is defined and declared by ov12/tu002 (src/ov12/rg_char.h);
 * src/ov12/rg_shot.c already redeclares it the same way to call it from a
 * different TU, so this follows that precedent instead of including that
 * TU's own src header.
 */
extern void RgCharFree(RgChar *pChar);
static void _RgCharMgrCallControl(RgCharMgr *manager);
static void _RgCharMgrCallPassTime(RgCharMgr *manager, float deltaTime);
static void _RgCharMgrCallDisp(RgCharMgr *manager);

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

int RgCharMgrIsFullOfBuffer(RgCharMgr *manager, int count)
{
    if (manager == 0) {
        assert_prog(D_00A53138, D_00A530B8, 93);
    }
    if (manager->count + count < 0x100) {
        return 0;
    }
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", RgCharMgrGC);

/*
 * RgCharControl is defined and declared by ov12/tu002 (src/ov12/rg_char.h);
 * that TU's own header does not publish it, so this follows the RgCharFree
 * precedent above instead of including that TU's own src header.
 */
extern void RgCharControl(RgChar *pChar);

void _RgCharMgrCallControl(RgCharMgr *manager)
{
    RgChar *chars[256];
    unsigned int count;
    unsigned int i;

    if (manager == 0) {
        assert_prog(D_00A53138, D_00A530B8, 132);
    }
    count = manager->count;
    for (i = 0; i < count; i++) {
        chars[i] = manager->chars[i];
    }
    for (i = 0; i < count; i++) {
        RgCharControl(chars[i]);
    }
}

/*
 * RgCharDisp is defined and declared by ov12/tu002 (src/ov12/rg_char.h);
 * that TU's own header does not publish it, so this follows the RgCharFree
 * precedent above instead of including that TU's own src header.
 */
extern void RgCharDisp(RgChar *pChar);

void _RgCharMgrCallDisp(RgCharMgr *manager)
{
    RgChar *chars[256];
    unsigned int count;
    unsigned int i;

    if (manager == 0) {
        assert_prog(D_00A53138, D_00A530B8, 162);
    }
    count = manager->count;
    for (i = 0; i < count; i++) {
        chars[i] = manager->chars[i];
    }
    for (i = 0; i < count; i++) {
        RgCharDisp(chars[i]);
    }
}

/*
 * RgCharPassTime is defined and declared by ov12/tu002 (src/ov12/rg_char.h);
 * that TU's own header does not publish it, so this follows the RgCharFree
 * precedent above instead of including that TU's own src header.
 */
extern void RgCharPassTime(RgChar *pChar, float deltaTime);

void _RgCharMgrCallPassTime(RgCharMgr *manager, float deltaTime)
{
    RgChar *chars[256];
    unsigned int count;
    unsigned int i;

    if (manager == 0) {
        assert_prog(D_00A53138, D_00A530B8, 182);
    }
    count = manager->count;
    for (i = 0; i < count; i++) {
        chars[i] = manager->chars[i];
    }
    for (i = 0; i < count; i++) {
        RgCharPassTime(chars[i], deltaTime);
    }
}

void RgCharMgrControl(RgCharMgr *manager)
{
    _RgCharMgrCallControl(manager);
}

void RgCharMgrPassTime(RgCharMgr *manager, float deltaTime)
{
    _RgCharMgrCallPassTime(manager, deltaTime);
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

RgCharMgr *InstanceOfRgCharMgr(void)
{
    RgCharMgr *manager;

    manager = RgSingletonIDGet(0);
    if (manager == 0) {
        manager = RgHeapAlloc(InstanceOfRgHeap(), 0x404, D_00A530B8, 277);
        InitRgCharMgr(manager);
        RgSingletonIDEntry(0, (RgSimpleDB *) manager,
                           (void (*)(RgSimpleDB *)) _DisposeCharMgr);
    }
    return manager;
}

RgCharMgr *InstanceOfRgCharMgrClear(void)
{
    RgCharMgr *manager = InstanceOfRgCharMgr();

    _DestructCharMgr(manager);
    return manager;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_charmgr", RgCharMgrDump);
