/*
 * TU-local declarations of main/tu132 (src/main/yajima_test.c).
 */

#ifndef SRC_MAIN_YAJIMA_TEST_H
#define SRC_MAIN_YAJIMA_TEST_H

#include "shared.h"

typedef unsigned int SceStatRawWords[64 / sizeof(unsigned int)];

typedef struct HddTestMenuEntry {
    const char *label;
    void (*action)(void);
} HddTestMenuEntry;

typedef union PadDataRawView {
    unsigned char bytes[208];
    u16 halfwords[208 / sizeof(u16)];
    u64 doublewords[208 / sizeof(u64)];
} PadDataRawView;

typedef unsigned char LittleEndianWord[4];

#endif /* SRC_MAIN_YAJIMA_TEST_H */
