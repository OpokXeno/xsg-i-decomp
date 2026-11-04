/*
 * OV12 original TU 20: 0x00a140a0..0x00a143e0 (8 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_singleton_id.h"
#include "rg_singleton_id.h"

static RgSingletonManager s_inIDmgr;

static void _Entry(RgSingletonManager *manager, unsigned int singleton_id,
                    void *instance, void (*destructor)(void *instance));

static void _Clear(RgSingletonManager *manager)
{
    int index;

    if (manager == 0) {
        assert_prog("pMgr != NIL", "../rg_singleton_id.euc.c", 31);
    }

    manager->count = 0;
    for (index = 0; index < RG_SID_MAX; index++) {
        manager->instances[index] = 0;
        manager->destructors[index] = 0;
    }
}

static void _Destruct(RgSingletonManager *manager)
{
    int index;
    unsigned int singleton_id;

    if (manager == 0) {
        assert_prog("pMgr != NIL", "../rg_singleton_id.euc.c", 44);
    }

    for (index = manager->count - 1; index >= 0; index--) {
        singleton_id = manager->order[index];
        if (manager->instances[singleton_id] != 0) {
            if (manager->destructors[singleton_id] != 0) {
                manager->destructors[singleton_id](manager->instances[singleton_id]);
            }
        }
        manager->instances[singleton_id] = 0;
    }

    _Clear(manager);
}

static void _Entry(RgSingletonManager *manager, unsigned int singleton_id,
                   void *instance, void (*destructor)(void *instance))
{
    int *count;
    int order_index;

    if (manager == 0) {
        assert_prog("pMgr != NIL", "../rg_singleton_id.euc.c", 58);
    }
    if (manager->instances[singleton_id] != 0) {
        assert_prog("pMgr->m_apPtrTbl[eID] == NIL",
                    "../rg_singleton_id.euc.c", 59);
    }
    if (instance == 0) {
        assert_prog("pPtr != NIL", "../rg_singleton_id.euc.c", 60);
    }
    if (singleton_id >= RG_SID_MAX) {
        assert_prog("0 <= eID && eID < RG_SID_MAX",
                    "../rg_singleton_id.euc.c", 61);
    }

    count = &manager->count;
    /* Load the insertion count after the ID validation checks. */
    order_index = *count;
    manager->instances[singleton_id] = instance;
    manager->order[order_index] = singleton_id;
    manager->destructors[singleton_id] = destructor;
    *count = order_index + 1;
}

static void *_Get(RgSingletonManager *manager, unsigned int singleton_id)
{
    if (manager == 0) {
        assert_prog("pMgr != NIL", "../rg_singleton_id.euc.c", 70);
    }
    if (singleton_id >= RG_SID_MAX) {
        assert_prog("0 <= eID && eID < RG_SID_MAX",
                    "../rg_singleton_id.euc.c", 71);
    }
    return manager->instances[singleton_id];
}

void RgSingletonIDClear(void)
{
    _Clear(&s_inIDmgr);
}

void RgSingletonDispose(void)
{
    _Destruct(&s_inIDmgr);
}

void RgSingletonIDEntry(int singleton_id, void *instance,
                        void (*destructor)(void *instance))
{
    _Entry(&s_inIDmgr, singleton_id, instance, destructor);
}

void *RgSingletonIDGet(unsigned int singleton_id)
{
    return _Get(&s_inIDmgr, singleton_id);
}
