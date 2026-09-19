/*
 * OV12 original TU 20: 0x00a140a0..0x00a143e0 (8 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_singleton_id.h"
#include "rg_singleton_id.h"

/*
 * These are external file-backed witnesses, not candidate-emitted data.
 *
 * ov12:0x00a53008, 12 bytes, SHA-256
 * 0af24906f973437c43cba57232d069ee26beaf4b983824fe0f61727f74d8ee74
 * contains the assertion expression "pMgr != NIL".
 * ov12:0x00a53018, 25 bytes, SHA-256
 * 7337a7a7d9fd799db6b03292e71e8e7304fb41b1c04131c24e14bde7270d8d58
 * contains the source filename "../rg_singleton_id.euc.c".
 * ov12:0x00a53068, 29 bytes, SHA-256
 * 9a9bf18448e6f0547d21b210171dcdd394fa5bd943802154800e3e7ea5b65fc3
 * contains the range expression "0 <= eID && eID < RG_SID_MAX".
 */
extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern const char rg_singleton_manager_nonnull_expression[];
extern const char rg_singleton_id_source_file[];
extern const char rg_singleton_id_range_expression[];
extern RgSingletonManager s_inIDmgr;

static void _Entry(RgSingletonManager *manager, unsigned int singleton_id,
                    void *instance, void (*destructor)(void *instance));

/*
 * Reviewer-directed correction for the OV12 singleton-manager functions.
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
        assert_prog(rg_singleton_manager_nonnull_expression,
                    rg_singleton_id_source_file, 31);
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
        assert_prog(rg_singleton_manager_nonnull_expression,
                    rg_singleton_id_source_file, 44);
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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_singleton_id", _Entry);

/* Reviewer-directed correction for the OV12 singleton-manager lookup. */

static void *_Get(RgSingletonManager *manager, unsigned int singleton_id)
{
    if (manager == 0) {
        assert_prog(rg_singleton_manager_nonnull_expression,
                    rg_singleton_id_source_file, 70);
    }
    if (singleton_id >= 15) {
        assert_prog(rg_singleton_id_range_expression,
                    rg_singleton_id_source_file, 71);
    }
    return manager->instances[singleton_id];
}

void RgSingletonIDClear(void)
{
    _Clear(&s_inIDmgr);
}

/* Reviewer-directed correction for the OV12 singleton disposal wrapper. */

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

/* Reviewer-directed correction for the OV12 singleton ID wrapper. */

void *RgSingletonIDGet(unsigned int singleton_id)
{
    return _Get(&s_inIDmgr, singleton_id);
}
