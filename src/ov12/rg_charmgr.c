/*
 * OV12 original TU 21: 0x00a143e0..0x00a14d38 (20 functions)
 */
#include "common.h"
#include "shared.h"
#define _DestructCharMgr RgCharMgrHeaderDestructDecl
#include "rg_charmgr.h"
#undef _DestructCharMgr

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

/*
 * Same window: ov12:0x00a53118 contains the assertion expression
 * "pMgr != NIL && pChar != NIL", RgCharMgrFree's own guard.
 */

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

extern int RgCharGetType(RgChar *pChar);
extern void XrgLog(const char *format, const char *source_file, int line, ...);
void InitRgCharMgr(RgCharMgr *manager);

static int _FindPtr(RgChar *characters[], unsigned int count, RgChar *pChar)
{
    unsigned int index;

    for (index = 0; index < count; index++) {
        if (pChar == characters[index]) {
            return index;
        }
    }
    return -1;
}

static int _FindPtr(RgChar *characters[], unsigned int count, RgChar *pChar);

static unsigned int _EntryPtr(RgCharMgr *manager, unsigned int count,
                              RgChar *pChar)
{
    RgChar **characters = manager->chars;

    if (_FindPtr(manager->chars, count, pChar) >= 0) {
        assert_prog("_FindPtr(apPtrTbl, nTblSize, pEntry) < 0", "../rg_charmgr.euc.c", 32);
    }
    if (count >= 0x100) {
        assert_prog("nTblSize < RG_CHAR_MAX", "../rg_charmgr.euc.c", 33);
    }
    if (pChar == 0) {
        assert_prog("pEntry != NIL", "../rg_charmgr.euc.c", 34);
    }
    characters[count] = pChar;
    return count + 1;
}

static unsigned int _DeletePtr(RgCharMgr *manager, unsigned int count,
                               RgChar *pChar)
{
    int found_index;
    unsigned int new_count;
    unsigned int index;

    found_index = _FindPtr(manager->chars, count, pChar);
    if (found_index < 0) {
        assert_prog("nFindIndex >= 0", "../rg_charmgr.euc.c", 46);
    }
    index = found_index;
    new_count = count - 1;
    for (; index < new_count; index++) {
        manager->chars[index] = manager->chars[index + 1];
    }
    return new_count;
}

RgChar *RgCharMgrEntry(RgCharMgr *manager, RgChar *pChar)
{
    if (pChar == 0) {
        assert_prog("pChar != NIL", "../rg_charmgr.euc.c", 58);
    }
    manager->count = _EntryPtr(manager, manager->count, pChar);
    return pChar;
}

void RgCharMgrFree(RgCharMgr *manager, RgChar *pChar)
{
    unsigned int count;

    if ((manager == 0) || (pChar == 0)) {
        assert_prog("pMgr != NIL && pChar != NIL", "../rg_charmgr.euc.c", 66);
    }
    count = _DeletePtr(manager, manager->count, pChar);
    pChar->mgr = 0;
    manager->count = count;
    RgCharFree(pChar);
}

RgChar *RgCharMgrSearch(RgCharMgr *manager, int type)
{
    unsigned int index;
    RgChar *pChar;

    if (manager == 0) {
        assert_prog("pMgr != NIL", "../rg_charmgr.euc.c", 81);
    }
    for (index = 0; index < manager->count; index++) {
        pChar = manager->chars[index];
        if (RgCharGetType(pChar) == type) {
            return pChar;
        }
    }
    return 0;
}

int RgCharMgrIsFullOfBuffer(RgCharMgr *manager, int count)
{
    if (manager == 0) {
        assert_prog("pMgr != NIL", "../rg_charmgr.euc.c", 93);
    }
    if (manager->count + count < 0x100) {
        return 0;
    }
    return 1;
}

void RgCharMgrGC(RgCharMgr *manager)
{
    RgChar *deleted_chars[256];
    int deleted_count;
    int index;

    if (manager == 0) {
        assert_prog("pMgr != 0", "../rg_charmgr.euc.c", 107);
    }
    deleted_count = 0;
    for (index = 0; index < manager->count; index++) {
        if (manager->chars[index]->type == -1) {
            deleted_chars[deleted_count] = manager->chars[index];
            deleted_count++;
        }
    }
    for (index = 0; index < deleted_count; index++) {
        RgCharMgrFree(manager, deleted_chars[index]);
    }
}

/* RgCharControl follows its owning ov12/tu002 declaration (src/ov12/rg_char.h). */
extern void RgCharControl(RgChar *pChar);

void _RgCharMgrCallControl(RgCharMgr *manager)
{
    RgChar *chars[256];
    unsigned int count;
    unsigned int i;

    if (manager == 0) {
        assert_prog("pMgr != NIL", "../rg_charmgr.euc.c", 132);
    }
    count = manager->count;
    for (i = 0; i < count; i++) {
        chars[i] = manager->chars[i];
    }
    for (i = 0; i < count; i++) {
        RgCharControl(chars[i]);
    }
}

/* RgCharDisp follows its owning ov12/tu002 declaration (src/ov12/rg_char.h). */
extern void RgCharDisp(RgChar *pChar);

void _RgCharMgrCallDisp(RgCharMgr *manager)
{
    RgChar *chars[256];
    unsigned int count;
    unsigned int i;

    if (manager == 0) {
        assert_prog("pMgr != NIL", "../rg_charmgr.euc.c", 162);
    }
    count = manager->count;
    for (i = 0; i < count; i++) {
        chars[i] = manager->chars[i];
    }
    for (i = 0; i < count; i++) {
        RgCharDisp(chars[i]);
    }
}

/* RgCharPassTime follows its owning ov12/tu002 declaration (src/ov12/rg_char.h). */
extern void RgCharPassTime(RgChar *pChar, float deltaTime);

void _RgCharMgrCallPassTime(RgCharMgr *manager, float deltaTime)
{
    RgChar *chars[256];
    unsigned int count;
    unsigned int i;

    if (manager == 0) {
        assert_prog("pMgr != NIL", "../rg_charmgr.euc.c", 182);
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

static void _DestructCharMgr(RgCharMgr *manager)
{
    RgChar *characters[256];
    unsigned int count;
    unsigned int index;

    count = manager->count;
    for (index = 0; index < count; index++) {
        characters[index] = manager->chars[index];
    }
    for (index = 0; index < count; index++) {
        RgCharFree(characters[index]);
    }
    RgCharMgrGC(manager);
    if (manager->count != 0) {
        assert_prog("pMgr->m_nCharTblSiz <= 0", "../rg_charmgr.euc.c", 237);
    }
    InitRgCharMgr(manager);
}

static void _DisposeCharMgr(RgCharMgr *manager)
{
    _DestructCharMgr(manager);
    RgHeapFree(InstanceOfRgHeap(), manager, "../rg_charmgr.euc.c", 246);
}

void InitRgCharMgr(RgCharMgr *manager)
{
    if (manager == 0) {
        assert_prog("pMgr != NIL", "../rg_charmgr.euc.c", 253);
    }
    manager->count = 0;
}

RgCharMgr *InstanceOfRgCharMgr(void)
{
    RgCharMgr *manager;

    manager = RgSingletonIDGet(0);
    if (manager == 0) {
        manager = RgHeapAlloc(InstanceOfRgHeap(), 0x404, "../rg_charmgr.euc.c", 277);
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

void RgCharMgrDump(RgCharMgr *manager)
{
    unsigned int index;
    RgChar *pChar;

    XrgLog("------------ character manager dump (cnt=%d)\n", "../rg_charmgr.euc.c", 297, manager->count);
    for (index = 0; index < manager->count; index++) {
        pChar = manager->chars[index];
        XrgLog("---- character (%p)\n", "../rg_charmgr.euc.c", 300, pChar);
        XrgLog("  type = %d\n", "../rg_charmgr.euc.c", 301, RgCharGetType(pChar));
        XrgLog("  method(C:%p,D:%p,P:%p)\n", "../rg_charmgr.euc.c", 302, pChar->controlMethod,
               pChar->dispMethod, pChar->passTimeMethod);
    }
}
