/*
 * TU-local declarations of main/tu273 (src/main/Undulate.c).
 */

#ifndef SRC_MAIN_UNDULATE_H
#define SRC_MAIN_UNDULATE_H

typedef struct LayoutHeader LayoutHeader;
struct UnduDataHeader;

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

typedef struct UnduVertex {
    float x;
    float y;
    float z;
} UnduVertex;

typedef struct UnduTriangle {
    unsigned short wallAttributeIndex;
    unsigned short unmodeled_02;
    float planeOffset;
    unsigned short vertexOffset[3];
    unsigned short normalOffset;
    unsigned char unmodeled_10[8];
} UnduTriangle;

typedef struct UnduResult {
    unsigned short *attribute;
    unsigned short *previousAttribute;
    short flags;
    short collisionType;
    unsigned char unmodeled_0c[8];
    float height;
    unsigned char unmodeled_18[8];
    UnduWallAttributes attributes;
} UnduResult;

typedef struct UnduGroupItem {
    unsigned char unmodeled_00[16];
} UnduGroupItem;

typedef struct UnduColiGroup {
    unsigned short firstIdentifier;
    unsigned short count;
    UnduGroupItem *items;
} UnduColiGroup;

typedef struct UnduColiRecord {
    unsigned short count;
    unsigned char unmodeled_02[2];
    struct UnduDataHeader *headers;
} UnduColiRecord;

typedef struct UnduColiTables {
    unsigned char type;
    unsigned char unmodeled_01[3];
    unsigned char *tables[5];
} UnduColiTables;

typedef struct UnduColiHead {
    unsigned char type;
    unsigned char directCount;
    unsigned char secondCount;
    unsigned char groupCount;
    struct UnduDataHeader *directHeaders;
    struct UnduDataHeader *secondHeaders;
    UnduColiGroup *groups;
    UnduColiRecord *records;
} UnduColiHead;

typedef struct UnduDataHeaderGroup {
    unsigned short firstIdentifier;
    unsigned short headerCount;
    float (*headers)[4];
} UnduDataHeaderGroup;

typedef struct UnduDataHeaderSubList {
    short headerCount;
    unsigned char unmodeled_02[2];
    struct UnduDataHeader *headers;
} UnduDataHeaderSubList;

typedef struct UnduDataHeaderIndex {
    unsigned char type;
    unsigned char firstCount;
    unsigned char secondCount;
    unsigned char groupCount;
    struct UnduDataHeader *firstHeaders;
    struct UnduDataHeader *secondHeaders;
    UnduDataHeaderGroup *groups;
    UnduDataHeaderSubList *subLists;
} UnduDataHeaderIndex;

typedef struct UnduGameLoopState {
    unsigned char unmodeled_00[0x5c];
    float attributeHeights[32];
} UnduGameLoopState;
extern UnduGameLoopState GameLoopState;

/* Per-map undulation (terrain height) work data. */
typedef struct UnduWork {
    unsigned char unmodeled_00[0x50];
    UnduWallAttrHeader *wallAttrHeader;
    int subCount;           /* 0x54: number of collision sub-primitives to test */
    unsigned char *vertexData;
    unsigned char *normalData;
    int subListOffset;      /* 0x60: byte offset of the first sub-primitive; entries are 0x18 bytes apart */
    UnduWallAttributes *wallAttributeTable;
    unsigned char unmodeled_68[8];
    float collisionPoint[3];
    unsigned char unmodeled_7c[4];
    float hitPoint[3];
    unsigned char unmodeled_8c[4];
    int hasCollisionPoint;
    float heightTolerance;
    float collisionHeight;
    unsigned short *collisionAttribute;
    unsigned char unmodeled_a0[2];
    unsigned char collisionType;
    unsigned char unmodeled_a3[5];
    UnduWallAttributes wallAttributes;
    unsigned long long wallAttributeMask;
    unsigned long long wallAttributeValue;
} UnduWork;

/* A collision header is 20 bytes; its identifier is compared by the search. */
typedef struct UnduDataHeader {
    unsigned short subCount;
    unsigned short identifier; /* +0x02 */
    unsigned char *vertexData;
    unsigned char *normalData;
    unsigned char *subList;
    UnduWallAttributes *wallAttributeTable;
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
