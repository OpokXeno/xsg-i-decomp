/*
 * TU-local declarations of main/tu164 (src/main/end_print.c).
 */

#ifndef SRC_MAIN_END_PRINT_H
#define SRC_MAIN_END_PRINT_H

#include "shared.h"
#include "main/xgl_packet.h"
#include "main/end_print.h"

/*
 * PrintFunc: a fixed-capacity queue of deferred print commands appended by
 * endPrintExtFuncPack and the other still-asm producers of this TU and
 * drained by PrintFlush. Each 8-byte entry holds the callback and its
 * single int argument; the queue is terminated by a NULL func.
 * pPrintFuncTop is the append cursor the producers write through.
 */
typedef struct PrintFuncEntry {
    void (*func)(FontDrawCursor *cursor, int param);
    int param;
} PrintFuncEntry;

#endif /* SRC_MAIN_END_PRINT_H */
