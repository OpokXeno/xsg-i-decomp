/*
 * TU-local declarations of main/tu115 (src/main/ssd_4.c).
 */

#ifndef SRC_MAIN_SSD_4_H
#define SRC_MAIN_SSD_4_H

/*
 * One node of the sound-memory block list threaded from RssdWork+0x1c0
 * (main/tu110's RssdWorkFlags, src/main/ssd_init.h). iSsdNewMemoryPtr,
 * iSsdNewMemoryPtr2 and iSsdDisposeMemoryPtr (main:0x00242150/0x00242278/
 * 0x002423b0, all still scaffold) walk and splice `next`; the pointer
 * SsdNewMemoryPtr/SsdNewMemoryPtr2 hand back to callers is this header plus
 * its fixed 0x40-byte size (see SsdDisposeMemoryPtr's comment in ssd_4.c).
 * `end` is read back by SsdGetBlockMemorySize (main:0x00242408) as
 * `end - (address of this header)`, the block's payload size.
 */
typedef struct SsdMemoryBlock {
    struct SsdMemoryBlock *next;       /* +0x00 */
    int end;                            /* +0x04 */
    unsigned char state;                /* +0x08: allocator state (2 for low, 18 for high) */
    unsigned char flags;                /* +0x09: cleared when an allocation is created */
    unsigned char _unmodeled_0a[2];     /* +0x0a..0x0b */
    int tag;                            /* +0x0c: allocator tag argument */
    unsigned char _unmodeled_10[0x30];  /* +0x10..0x3f */
} SsdMemoryBlock;

#endif /* SRC_MAIN_SSD_4_H */
