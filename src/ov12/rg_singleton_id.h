/*
 * TU-local declarations of ov12/tu020 (src/ov12/rg_singleton_id.c).
 */

#ifndef SRC_OV12_RG_SINGLETON_ID_H
#define SRC_OV12_RG_SINGLETON_ID_H

#include "ov12/rg_singleton_id.h"

/*
 * This is the observed manager access view: instance pointers at 0x00,
 * destructor callbacks at 0x3c, insertion-order IDs at 0x78, and the active
 * count at 0xb4.  No unobserved padding or members are introduced.
 */
typedef struct RgSingletonManager {
    void *instances[RG_SID_MAX];
    void (*destructors[RG_SID_MAX])(void *instance);
    unsigned int order[RG_SID_MAX];
    int count;
    /* The global occupies 0xc0 bytes; these trailing bytes have no known
     * semantic use in the recovered access set. */
    unsigned char unmodeled_b8[8];
} RgSingletonManager;

#endif /* SRC_OV12_RG_SINGLETON_ID_H */
