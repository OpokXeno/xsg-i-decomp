/*
 * TU-local declarations of main/tu152 (src/main/tch.c).
 */

#ifndef SRC_MAIN_TCH_H
#define SRC_MAIN_TCH_H

#include "shared.h"

/*
 * One variable-length record of a TimeChart's index: the string identifier
 * TCH_getInfoID compares against, followed by wordCount * 4 bytes of trailing
 * data (pointerOffsets). Records are packed back to back starting at
 * TimeChart.records, so the next one starts just past that trailing data:
 * &entry->pointerOffsets[entry->wordCount].
 */
typedef struct TchEntry {
    u16 wordCount; /* +0x00: size of this record's trailing data, in 4-byte words */
    unsigned char unmodeled_02[2];
    union {
        const char *name; /* +0x04: identifier TCH_getInfoID's caller looks up, once getInfoAddr() has fixed it up */
        u32 nameOffset;   /* +0x04: before that fixup, the offset of the identifier from the chart's address */
    };
    u32 pointerOffsets[1]; /* +0x08: relocatable addresses in the trailing data */
} TchEntry;

/*
 * A relocatable time-chart resource. Its pointer is stored at PlayControl
 * +0x08 (src/main/play.h, "whose layout belongs to tch.c"); this TU is that
 * owner. Only the members TCH_getInfoID (this TU) touches are named here.
 */
typedef struct TimeChart {
    unsigned char unmodeled_00[3];
    signed char relocated; /* +0x03: zero until getInfoAddr() has fixed up the resource's own pointers */
    u16 recordCount; /* +0x04 */
    unsigned char unmodeled_06[0x1a];
    TchEntry records[1]; /* +0x20 */
} TimeChart;

extern unsigned int strlen(const char *string);
extern int strncmp(const char *string1, const char *string2, unsigned int count);
int TCH_getInfoID(TimeChart *chart, const char *name, int length);

#endif
