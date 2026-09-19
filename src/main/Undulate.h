/*
 * TU-local declarations of main/tu273 (src/main/Undulate.c).
 */

#ifndef SRC_MAIN_UNDULATE_H
#define SRC_MAIN_UNDULATE_H

/* Per-map undulation (terrain height) work data. */
typedef struct UnduWork {
    unsigned char unmodeled_00[0x54];
    int subCount;           /* 0x54: number of collision sub-primitives to test */
    unsigned char unmodeled_58[8];
    int subListOffset;      /* 0x60: byte offset of the first sub-primitive; entries are 0x18 bytes apart */
} UnduWork;

#endif /* SRC_MAIN_UNDULATE_H */
