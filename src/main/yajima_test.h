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

/*
 * m_54 (0x0036A920, size 0x18): YajimaTest only touches the output-selection
 * pointer xglMenuOpen fills in at +0x14; the rest of the record is not
 * recovered.
 */
typedef struct YajimaMenuState {
    unsigned char unmodeled_00[0x14];
    int *selection;
} YajimaMenuState;

void YajimaTest(void);

#endif /* SRC_MAIN_YAJIMA_TEST_H */
