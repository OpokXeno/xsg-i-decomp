/*
 * TU-local declarations of main/tu164 (src/main/end_print.c).
 */

#ifndef SRC_MAIN_END_PRINT_H
#define SRC_MAIN_END_PRINT_H

#include "shared.h"
#include "main/xgl_packet.h"

/*
 * The active print-output context: a pointer to the current VIF packet
 * (+0x00) followed by state that PrintFlush, PrintSprite00, PrintLine,
 * PrintRibbon, PrintCircle and FontTexReload pass through or touch. Only
 * the packet pointer and the scratch tag block FontTexReload builds
 * (+0x30..+0x4f) are named; the rest of this TU never reads or writes it.
 */
typedef struct EndPrintContext {
    XglPacket *packet;                /* +0x00 */
    unsigned char unmodeled_04[0x2c]; /* +0x04..+0x2f */
    u64 scratch[4];                   /* +0x30..+0x4f */
} EndPrintContext;

/*
 * PrintFunc: a fixed-capacity queue of deferred print commands appended by
 * endPrintExtFuncPack and the other still-asm producers of this TU and
 * drained by PrintFlush. Each 8-byte entry holds the callback and its
 * single int argument; the queue is terminated by a NULL func.
 * pPrintFuncTop is the append cursor the producers write through.
 */
typedef struct PrintFuncEntry {
    void (*func)(EndPrintContext *context, int param);
    int param;
} PrintFuncEntry;

extern PrintFuncEntry PrintFunc[];
extern PrintFuncEntry *pPrintFuncTop;

#endif /* SRC_MAIN_END_PRINT_H */
