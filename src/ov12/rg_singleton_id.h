/*
 * TU-local declarations of ov12/tu020 (src/ov12/rg_singleton_id.c).
 */

#ifndef SRC_OV12_RG_SINGLETON_ID_H
#define SRC_OV12_RG_SINGLETON_ID_H

/*
 * This is the observed manager access view: instance pointers at 0x00,
 * destructor callbacks at 0x3c, insertion-order IDs at 0x78, and the active
 * count at 0xb4.  No unobserved padding or members are introduced.
 */
typedef struct RgSingletonManager {
    void *instances[15];
    void (*destructors[15])(void *instance);
    unsigned int order[15];
    int count;
} RgSingletonManager;

#endif /* SRC_OV12_RG_SINGLETON_ID_H */
