/*
 * TU-local declarations of main/tu110 (src/main/ssd_init.c).
 */

#ifndef SRC_MAIN_SSD_INIT_H
#define SRC_MAIN_SSD_INIT_H

#include "main/ssd_init.h"
#include "main/nml_packet_add.h"

struct SsdMemoryBlock;

/*
 * The parameter records SsdInit passes to CreateSema and CreateThread. Only
 * the offsets this function stores are named; the libkernel owner layouts
 * (src/main/xgl_thread.h, core private.h) are other TUs' headers and are not
 * completed here.
 */
typedef struct SsdSemaCreateInfo {
    int _unmodeled_00;                    /* +0x00 */
    int max_count;                        /* +0x04 */
    int init_count;                       /* +0x08 */
    unsigned char _unmodeled_0c[0x08];    /* +0x0c..0x13 */
    unsigned int option;                  /* +0x14 */
} SsdSemaCreateInfo;

typedef struct SsdThreadCreateInfo {
    int _unmodeled_00;                    /* +0x00 */
    void (*func)(void);                   /* +0x04 */
    void *stack;                          /* +0x08 */
    int stack_size;                       /* +0x0c */
    void *gp_reg;                         /* +0x10 */
    int initial_priority;                 /* +0x14 */
    unsigned char _unmodeled_18[0x08];    /* +0x18..0x1f */
    unsigned int option;                  /* +0x20 */
} SsdThreadCreateInfo;

void RssdBackgroundNextWave(const int *request);

/*
 * SIF RPC end callback for the RSSD work area: clears flag bit 3, copies the
 * 32-byte receive record into RssdWork, reports a status (-1 on a nonzero
 * result, else flag bit 5) through the registered one-shot completion
 * callback, then signals the waiting thread's semaphore.
 *
 * It runs from the SIF RPC interrupt context, so it ends with the
 * ee-interrupt-handler-return primitive (docs/ps2-capabilities.md): `sync.l`
 * orders every earlier load/store, including the iSignalSema effects, before
 * `ei` re-enables interrupts on the way out.
 */
void RssdSifRpcCallback(void);

#endif /* SRC_MAIN_SSD_INIT_H */
