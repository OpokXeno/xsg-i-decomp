/*
 * OV12 original TU 20: 0x00a140a0..0x00a143e0 (8 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_singleton_id.h"
#include "rg_singleton_id.h"

/* The assertion strings retain their original source expressions and file
 * name; the compiler emits them as this TU's read-only data. */
extern void assert_prog(const char *expression, const char *source_file,
                        int line);
static RgSingletonManager s_inIDmgr;

static void _Entry(RgSingletonManager *manager, unsigned int singleton_id,
                    void *instance, void (*destructor)(void *instance));

/*
 * Layout and callbacks for the OV12 singleton-manager functions.
 *
 * Identity: slus-20469-412d448de315 / ov12.  The manager layout and callback
 * order are bounded by the original accesses.  The assertion arguments use
 * the roles evidenced by the original file-backed strings: expression, source
 * file, and source line.
 */

static void _Clear(RgSingletonManager *manager)
{
    int index;

    if (manager == 0) {
        assert_prog("pMgr != NIL", "../rg_singleton_id.euc.c", 31);
    }

    manager->count = 0;
    for (index = 0; index < 15; index++) {
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
    if (singleton_id >= 15) {
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

/* Layout and callbacks for the OV12 singleton-manager lookup. */

static void *_Get(RgSingletonManager *manager, unsigned int singleton_id)
{
    if (manager == 0) {
        assert_prog("pMgr != NIL", "../rg_singleton_id.euc.c", 70);
    }
    if (singleton_id >= 15) {
        assert_prog("0 <= eID && eID < RG_SID_MAX",
                    "../rg_singleton_id.euc.c", 71);
    }
    return manager->instances[singleton_id];
}

void RgSingletonIDClear(void)
{
    _Clear(&s_inIDmgr);
}

/* Layout and callbacks for the OV12 singleton disposal wrapper. */

void RgSingletonDispose(void)
{
    _Destruct(&s_inIDmgr);
}

/*
 * The registry keeps any singleton with its destructor; its shared
 * prototype (include/shared.h, config/header-canon.json) is spelled for
 * the RgSimpleDB singletons, so a non-RgSimpleDB destructor converts at
 * the call (see src/ov12/rg_motion_info_db.c).  _Entry itself stores the
 * pointer and destructor untyped, so the conversion happens here.
 */
void RgSingletonIDEntry(int singleton_id, RgSimpleDB *database,
                        void (*destructor)(RgSimpleDB *database))
{
    _Entry(&s_inIDmgr, singleton_id, database, (void (*)(void *))destructor);
}

/* Layout and callbacks for the OV12 singleton ID wrapper. */

void *RgSingletonIDGet(unsigned int singleton_id)
{
    return _Get(&s_inIDmgr, singleton_id);
}
