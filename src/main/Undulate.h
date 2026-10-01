/*
 * TU-local declarations of main/tu273 (src/main/Undulate.c).
 */

#ifndef SRC_MAIN_UNDULATE_H
#define SRC_MAIN_UNDULATE_H

/* Collision mode in the wall-attribute header. */
typedef struct UnduWallAttrHeader {
    unsigned char unmodeled_00[8];
    unsigned short flags;
} UnduWallAttrHeader;

/* Each wall-attribute table entry consists of two words. */
typedef union UnduWallAttributes {
    unsigned int words[2];
    unsigned long long bits;
} UnduWallAttributes;

/* Per-map undulation (terrain height) work data. */
typedef struct UnduWork {
    unsigned char unmodeled_00[0x50];
    UnduWallAttrHeader *wallAttrHeader;
    int subCount;           /* 0x54: number of collision sub-primitives to test */
    unsigned char unmodeled_58[8];
    int subListOffset;      /* 0x60: byte offset of the first sub-primitive; entries are 0x18 bytes apart */
    UnduWallAttributes *wallAttributeTable;
    unsigned char unmodeled_68[0x40];
    UnduWallAttributes wallAttributes;
    unsigned long long wallAttributeMask;
    unsigned long long wallAttributeValue;
} UnduWork;

/* A collision header is 20 bytes; its identifier is compared by the search. */
typedef struct UnduDataHeader {
    unsigned char unmodeled_00[2];
    unsigned short identifier; /* +0x02 */
    unsigned char unmodeled_04[0x10];
} UnduDataHeader;

/* Fields of the per-query collision state, in its original byte order. */
typedef struct UnduQuery {
    int queryFlags;
    int queryFilter;
    short attrMask;
    short secondaryVertexOffset;
    unsigned char crossCheckCountX;
    unsigned char crossCheckCountZ;
    unsigned char unmodeled_0e[2];
    float heightLimit;
    float resultHeight;
    UnduDataHeader *header;
    struct UnduQuery *scratchpad;
    unsigned long long initializedState;
    unsigned char unmodeled_28[8];
    unsigned long long excludedAttributes;
    unsigned long long includedAttributes;
    float sample[3];
} UnduQuery;

#endif /* SRC_MAIN_UNDULATE_H */
