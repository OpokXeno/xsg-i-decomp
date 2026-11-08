/*
 * OV12 original TU 21: 0x00a143e0..0x00a14d38 (20 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_char.h"
#include "ov12/rg_charmgr.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_singleton_id.h"
#include "ov12/xrg_v_log.h"


static int _FindPtr(RgChar *characters[], unsigned int count, RgChar *pChar);
static unsigned int _EntryPtr(RgCharMgr *manager, unsigned int count,
                              RgChar *pChar);
static unsigned int _DeletePtr(RgCharMgr *manager, unsigned int count,
                               RgChar *pChar);
static void _RgCharMgrCallControl(RgCharMgr *manager);
static void _RgCharMgrCallPassTime(RgCharMgr *manager, float deltaTime);
static void _RgCharMgrCallDisp(RgCharMgr *manager);
static void _DestructCharMgr(RgCharMgr *manager);

/* Returns the index of pChar in characters, or -1 when it is absent. */
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

/* Appends pChar to the table of count entries and returns the new count. */
static unsigned int _EntryPtr(RgCharMgr *manager, unsigned int count,
                              RgChar *pChar)
{
    RgChar **characters = manager->chars;

    if (_FindPtr(manager->chars, count, pChar) >= 0) {
        assert_prog("_FindPtr(apPtrTbl, nTblSize, pEntry) < 0", "../rg_charmgr.euc.c", 32);
    }
    if (count >= RG_CHAR_MAX) {
        assert_prog("nTblSize < RG_CHAR_MAX", "../rg_charmgr.euc.c", 33);
    }
    if (pChar == 0) {
        assert_prog("pEntry != NIL", "../rg_charmgr.euc.c", 34);
    }
    characters[count] = pChar;
    return count + 1;
}

/* Removes pChar from the table by shifting the later entries down; returns the new count. */
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

/* Removes a character from the manager and frees it. */
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

/* Returns the first character of the given type, or null. */
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

/* Returns 1 when count more characters would reach RG_CHAR_MAX, else 0. */
int RgCharMgrIsFullOfBuffer(RgCharMgr *manager, int count)
{
    if (manager == 0) {
        assert_prog("pMgr != NIL", "../rg_charmgr.euc.c", 93);
    }
    if (manager->count + count < RG_CHAR_MAX) {
        return 0;
    }
    return 1;
}

/* Frees every character whose type is RG_CHAR_TYPE_DELETED. */
void RgCharMgrGC(RgCharMgr *manager)
{
    RgChar *deleted_chars[RG_CHAR_MAX];
    int deleted_count;
    int index;

    if (manager == 0) {
        assert_prog("pMgr != 0", "../rg_charmgr.euc.c", 107);
    }
    deleted_count = 0;
    for (index = 0; index < manager->count; index++) {
        if (manager->chars[index]->type == RG_CHAR_TYPE_DELETED) {
            deleted_chars[deleted_count] = manager->chars[index];
            deleted_count++;
        }
    }
    for (index = 0; index < deleted_count; index++) {
        RgCharMgrFree(manager, deleted_chars[index]);
    }
}

/* Runs each character's control, working from a copy of the list. */
void _RgCharMgrCallControl(RgCharMgr *manager)
{
    RgChar *chars[RG_CHAR_MAX];
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

/* Draws each character, working from a copy of the list. */
void _RgCharMgrCallDisp(RgCharMgr *manager)
{
    RgChar *chars[RG_CHAR_MAX];
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

/* Advances each character's time, working from a copy of the list. */
void _RgCharMgrCallPassTime(RgCharMgr *manager, float deltaTime)
{
    RgChar *chars[RG_CHAR_MAX];
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

/* Frees all characters, then resets the manager to empty. */
static void _DestructCharMgr(RgCharMgr *manager)
{
    RgChar *characters[RG_CHAR_MAX];
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

static void _DisposeCharMgr(void *manager)
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

/* Returns the singleton character manager, creating it on first use. */
RgCharMgr *InstanceOfRgCharMgr(void)
{
    RgCharMgr *manager;

    manager = RgSingletonIDGet(RG_SINGLETON_CHAR_MANAGER);
    if (manager == 0) {
        manager = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgCharMgr), "../rg_charmgr.euc.c", 277);
        InitRgCharMgr(manager);
        RgSingletonIDEntry(RG_SINGLETON_CHAR_MANAGER, manager, _DisposeCharMgr);
    }
    return manager;
}

/* Empties the singleton character manager and returns it. */
RgCharMgr *InstanceOfRgCharMgrClear(void)
{
    RgCharMgr *manager = InstanceOfRgCharMgr();

    _DestructCharMgr(manager);
    return manager;
}

/* Logs the character count, then the type and method pointers of each character. */
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
