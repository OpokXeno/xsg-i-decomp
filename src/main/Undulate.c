#include "common.h"
#include "debug_break.h"

/* The collision solver distinguishes positive and negative zero by their
 * 32-bit IEEE representation; copying bytes avoids incompatible scalar aliases. */
static __inline__ int unduSlopeBits(const float *value)
{
    int bits;

    __builtin_memcpy(&bits, value, 4);
    return bits;
}

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
    short neighbor[3];
    unsigned char unmodeled_16[2];
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
    unsigned char unmodeled_00[0x10];
    unsigned int flags;
    unsigned char unmodeled_14[0x48];
    float attributeHeights[32];
} UnduGameLoopState;
extern UnduGameLoopState GameLoopState;

/* Per-map undulation (terrain height) work data. */
typedef struct UnduWork {
    unsigned char unmodeled_00[0x40];
    float sample[3];
    unsigned char unmodeled_4c[4];
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
    unsigned char crossCheckCountX;
    unsigned char crossCheckCountZ;
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
/* The query is also the result passed by UnduCheck to
 * UnduCheckResultCheck. Both views share bytes0x00..0x27. */
typedef struct UnduQueryParameters {
    unsigned short *attribute;
    unsigned short *previousAttribute;
    short flags;
    short collisionType;
    unsigned char crossCheckCountX;
    unsigned char crossCheckCountZ;
    unsigned char unmodeled_0e[2];
    float heightLimit;
    float height;
    UnduDataHeader *header;
    UnduWork *scratchpad;
    UnduWallAttributes attributes;
} UnduQueryParameters;

typedef union UnduQueryPrefix {
    UnduQueryParameters parameters;
    UnduResult result;
} UnduQueryPrefix;

typedef struct UnduQuery {
    UnduQueryPrefix prefix;
    unsigned char unmodeled_28[8];
    unsigned long long excludedAttributes;
    unsigned long long includedAttributes;
    float sample[3];
} UnduQuery;

#endif /* SRC_MAIN_UNDULATE_H */

static unsigned char *CurrentColiHead;

int StudioEntrySkip = 0;

signed char FLAG_FRAME_60 = 0;

static float UnduCheckSub(UnduWork *work, int subOffset);

static int UnduCheckSubHeightCheck(UnduWork *work, int subOffset, float height);

float UnduCheck(float *sample, void *output, UnduQuery *query);

static void UnduCheckSubCrossCheck(UnduWork *work, UnduQuery *query, int subOffset);

extern float atan2f(float y, float x);
extern float xglSin(float x);
extern float xglCos(float x);
extern const float unduInitialSlopeTable[];
extern int printf(const char *format, ...);
extern const char unduParamErrorMessage[];
extern const char unduAssertFormat[];
extern const char unduAssertExpression[];
extern const char unduSourcePath[];

extern float UnduGet2(int flag, float x, float z);

#define UNDU_ORIENT(x0, z0, x1, z1, x2, z2) \
    ((x0) * (z1) + (x1) * (z2) + (x2) * (z0) - (x0) * (z2) - (x1) * (z0) - (x2) * (z1))

#define UNDU_RELOCATE(field, base) \
    do { \
        if ((field) != 0) \
            (field) = (void *)((unsigned char *)(field) + (unsigned int)(base)); \
    } while (0)

#define UNDU_RELOCATE_HEADER(header, base) \
    do { \
        UNDU_RELOCATE((header)->vertexData, base); \
        UNDU_RELOCATE((header)->normalData, base); \
        UNDU_RELOCATE((header)->subList, base); \
        UNDU_RELOCATE((header)->wallAttributeTable, base); \
    } while (0)

#define UNDU_RELOCATE_HEADERS(header, list, count, index, base) \
    do { \
        (header) = (list); \
        for ((index) = 0; (index) < (count); (index)++, (header)++) \
            UNDU_RELOCATE_HEADER(header, base); \
    } while (0)

/*
 * Types 1 and 2 are walked through volatile pointers: the original reads every
 * pointer field again after its null test, and rereads each list head and count
 * after the stores. The type 0 table is read once per field.
 */
void UnduInit(unsigned char *data)
{
    UnduColiTables *flat = (UnduColiTables *)data;
    volatile UnduColiHead *head = (volatile UnduColiHead *)data;
    volatile UnduDataHeader *header;
    volatile UnduColiGroup *group;
    volatile UnduColiRecord *record;

    CurrentColiHead = data;
    if (data == 0)
        return;
    if (data[0] & 0x80)
        return;
    switch (data[0]) {
    case 0:
        UNDU_RELOCATE(flat->tables[0], data);
        UNDU_RELOCATE(flat->tables[1], data);
        UNDU_RELOCATE(flat->tables[2], data);
        UNDU_RELOCATE(flat->tables[3], data);
        UNDU_RELOCATE(flat->tables[4], data);
        break;
    case 1: {
        int i;

        UNDU_RELOCATE(head->directHeaders, data);
        UNDU_RELOCATE(head->secondHeaders, data);
        UNDU_RELOCATE(head->groups, data);
        UNDU_RELOCATE_HEADERS(header, head->directHeaders, head->directCount, i, data);
        UNDU_RELOCATE_HEADERS(header, head->secondHeaders, head->secondCount, i, data);
        break;
    }
    case 2: {
        int i;

        UNDU_RELOCATE(head->directHeaders, data);
        UNDU_RELOCATE(head->secondHeaders, data);
        UNDU_RELOCATE(head->groups, data);
        UNDU_RELOCATE_HEADERS(header, head->directHeaders, head->directCount, i, data);
        UNDU_RELOCATE_HEADERS(header, head->secondHeaders, head->secondCount, i, data);
        group = head->groups;
        for (i = 0; i < head->groupCount; i++, group++)
            UNDU_RELOCATE(group->items, data);
        if ((void *)&head->records != (void *)head->directHeaders) {
            UNDU_RELOCATE(head->records, data);
            record = head->records;
            while ((short)record->count >= 0) {
                UNDU_RELOCATE(record->headers, data);
                UNDU_RELOCATE_HEADERS(header, record->headers, (short)record->count, i, data);
                record++;
            }
        }
        break;
    }
    default:
        break;
    }
    data[0] |= 0x80;
}

static UnduDataHeader *UnduDataGetHeaderSub(UnduDataHeader *headers,
                                            unsigned int count,
                                            unsigned int identifier)
{
    unsigned int index;
    UnduDataHeader *header;

    if (identifier & 0x8000) {
        identifier &= 0x7fff;
        if (identifier < count)
            return &headers[identifier];
    } else {
        header = headers;
        for (index = 0; index < count; index++, header++) {
            if (header->identifier == identifier)
                return header;
        }
    }
    return 0;
}

LayoutHeader *UnduDataGetHeader(int map_index, int unit_index)
{
    UnduDataHeaderIndex *headerIndex;
    UnduDataHeaderGroup *group;
    UnduDataHeaderSubList *subList;
    float (*header)[4];
    unsigned int group_index;
    unsigned int header_index;
    int entry_index;
    unsigned int identifier;

    headerIndex = (UnduDataHeaderIndex *)CurrentColiHead;
    if (headerIndex == 0) {
        return 0;
    }
    if (headerIndex->type != 0x82) {
        return 0;
    }

    switch ((unsigned int)map_index & 0xff00) {
    case 0:
        return (LayoutHeader *)UnduDataGetHeaderSub(
            headerIndex->firstHeaders, headerIndex->firstCount,
            (unsigned int)unit_index);
    case 0x100:
        return (LayoutHeader *)UnduDataGetHeaderSub(
            headerIndex->secondHeaders, headerIndex->secondCount,
            (unsigned int)unit_index);
    case 0x200:
        group_index = (unsigned int)map_index & 0xff;
        group = headerIndex->groups;
        if (group_index < headerIndex->groupCount) {
            group = &group[group_index];
            if ((unit_index & 0x8000) != 0) {
                header_index = (unsigned int)unit_index & 0x7fff;
                if (header_index < group->headerCount) {
                    return (LayoutHeader *)&group->headers[header_index];
                }
                return 0;
            }
            header = group->headers;
            identifier = group->firstIdentifier;
            for (entry_index = 0; entry_index < group->headerCount;
                 entry_index++, header++, identifier++) {
                if (identifier == (unsigned int)unit_index) {
                    return (LayoutHeader *)header;
                }
            }
        }
        break;
    case 0x300:
        if ((void *)&headerIndex->subLists != (void *)headerIndex->firstHeaders) {
            subList = headerIndex->subLists;
            for (; subList->headerCount >= 0; subList++) {
                if (((unsigned int)map_index & 0xff) == 0) {
                    return (LayoutHeader *)UnduDataGetHeaderSub(
                        subList->headers,
                        (unsigned int)(int)subList->headerCount,
                        (unsigned int)unit_index);
                }
                map_index--;
            }
        }
        break;
    default:
        break;
    }
    return 0;
}

void UnduParamInit(UnduQuery *query)
{
    unsigned char crossCheckCount = 16;
    UnduWork *scratchpad = (UnduWork *)0x70000000;

    query->prefix.result.attribute = 0;
    query->prefix.result.previousAttribute = 0;
    query->prefix.result.flags = 0;
    query->prefix.result.collisionType = 0;
    query->prefix.parameters.crossCheckCountX = crossCheckCount;
    query->prefix.parameters.crossCheckCountZ = crossCheckCount;
    query->prefix.parameters.heightLimit = 1.0f;
    query->prefix.result.height = -1000.0f;
    query->prefix.parameters.header = 0;
    query->prefix.parameters.scratchpad = scratchpad;
    query->prefix.result.attributes.bits = 0;
    query->excludedAttributes = 0;
    query->includedAttributes = 1;
}

static float UnduCheckSub(UnduWork *work, int subOffset)
{
    UnduTriangle *triangle = (UnduTriangle *)subOffset;
    UnduVertex *vertexA = (UnduVertex *)(work->vertexData + triangle->vertexOffset[0]);
    UnduVertex *vertexB = (UnduVertex *)(work->vertexData + triangle->vertexOffset[1]);
    UnduVertex *vertexC = (UnduVertex *)(work->vertexData + triangle->vertexOffset[2]);
    float *point = work->hitPoint;
    UnduVertex *normal;
    float edgeAB;
    float edgeBC;
    float edgeCA;
    float height;
    float high;
    float low;
    float highLimit;
    unsigned int flags;
    int inside;
    float py;
    float margin;

    edgeAB = point[0] * vertexA->z + vertexA->x * vertexB->z + vertexB->x * point[2]
           - point[0] * vertexB->z - vertexA->x * point[2] - vertexB->x * vertexA->z;
    edgeBC = point[0] * vertexB->z + vertexB->x * vertexC->z + vertexC->x * point[2]
           - point[0] * vertexC->z - vertexB->x * point[2] - vertexC->x * vertexB->z;
    edgeCA = point[0] * vertexC->z + vertexC->x * vertexA->z + vertexA->x * point[2]
           - point[0] * vertexA->z - vertexC->x * point[2] - vertexA->x * vertexC->z;
    if (edgeAB <= 0.0f && edgeBC <= 0.0f && edgeCA <= 0.0f) {
        normal = (UnduVertex *)(work->normalData + triangle->normalOffset);
        height = (triangle->planeOffset - normal->x * point[0] - normal->z * point[2]) / normal->y;
        flags = work->wallAttrHeader->flags;
        if (flags & 0x400) {
            inside = 0;
            low = vertexA->y;
            high = vertexA->y;
            if (vertexB->y < vertexA->y)
                low = vertexB->y;
            if (vertexC->y < low)
                low = vertexC->y;
            margin = work->heightTolerance;
            low += margin;
            if (high < vertexB->y)
                high = vertexB->y;
            if (high < vertexC->y)
                high = vertexC->y;
            highLimit = high - margin;
            py = point[1];
            if (flags & 0x10) {
                if (highLimit < py) {
                    if (flags & 0x800)
                        inside = 1;
                    else if (py < low)
                        inside = 1;
                }
            } else {
                if (!(flags & 0x800))
                    inside = 1;
                else if (highLimit < py)
                    inside = 1;
            }
            if (inside == 0)
                return -1000.0f;
        }
        return height;
    }
    return -1000.0f;
}

static int UnduCheckSubHeightCheck(UnduWork *work, int subOffset, float height)
{
    UnduTriangle *triangle = (UnduTriangle *)subOffset;
    unsigned short flags = work->wallAttrHeader->flags;
    unsigned int attributes;
    unsigned short *attribute = &triangle->wallAttributeIndex;
    float sampleHeight;
    float tolerance;
    float upperHeight;
    float boundedPreviousHeight;
    float previousHeight;
    int status;

    if (work->wallAttributeTable != 0)
        attributes = work->wallAttributeTable[triangle->wallAttributeIndex].words[0];
    else
        attributes = triangle->wallAttributeIndex;
    if ((flags & 0x20) && (attributes & 0xe000) == 0x4000)
        height = GameLoopState.attributeHeights[(attributes & 0x1f00) >> 8];
    status = 0;
    if (flags & 0x10) {
        sampleHeight = work->hitPoint[1];
        tolerance = work->heightTolerance;
        if (sampleHeight - tolerance < height && height < sampleHeight + tolerance) {
            upperHeight = sampleHeight + tolerance;
            status = 1;
            if (upperHeight < work->collisionHeight)
                status = 2;
        } else if (flags & 0x800) {
            boundedPreviousHeight = work->collisionHeight;
            if (boundedPreviousHeight == -1000.0f || height < sampleHeight + tolerance)
                status = 1;
            else if (height < boundedPreviousHeight)
                status = 1;
        }
    } else if (flags & 0x800) {
        if (height <= work->hitPoint[1] + work->heightTolerance)
            status = 1;
    } else {
        previousHeight = work->collisionHeight;
        if (previousHeight == -1000.0f || height < work->hitPoint[1] + work->heightTolerance)
            status = 1;
        else if (height < previousHeight)
            status = 2;
    }
    if (status != 0) {
        if (status == 2 || work->collisionHeight < height) {
            work->collisionHeight = height;
            work->collisionAttribute = attribute;
            status = 2;
        }
    }
    return status;
}

static void UnduCheckSubCheckAll(UnduWork *work) {
    int subOffset;
    int entryOffset;
    int i;
    float height;

    subOffset = work->subListOffset;
    for (i = 0; i < work->subCount; i++) {
        height = UnduCheckSub(work, subOffset);
        entryOffset = subOffset;
        subOffset += 0x18;
        if (height != -1000.0f) {
            UnduCheckSubHeightCheck(work, entryOffset, height);
        }
    }
}

static int CheckWallAttr(UnduWork *work, const unsigned short *wallAttribute)
{
    unsigned short attributeIndex;
    unsigned long long attributes;
    int wallMode;
    unsigned int wallModeMask;
    UnduWallAttributes *attributeTable;
    UnduWallAttributes *tableEntry;
    UnduWallAttributes *selectedAttributes;

    attributeTable = work->wallAttributeTable;
    if (attributeTable != 0) {
        attributeIndex = *wallAttribute;
        selectedAttributes = &work->wallAttributes;
        tableEntry = &attributeTable[attributeIndex];
        selectedAttributes->words[0] = tableEntry->words[0];
        selectedAttributes->words[1] = tableEntry->words[1];
    } else {
        work->wallAttributes.bits = *wallAttribute;
    }
    attributes = work->wallAttributes.bits;
    if ((attributes & work->wallAttributeMask) == work->wallAttributeValue) {
        return 1;
    }
    if ((attributes & 7) != 0) {
        wallModeMask = 0;
        wallMode = work->wallAttrHeader->flags & 0x300;
        switch (wallMode) {
        case 0x100:
            wallModeMask = 1;
            break;
        case 0x200:
            wallModeMask = 2;
            break;
        case 0x300:
            wallModeMask = 4;
            break;
        default:
            break;
        }
        if ((attributes & wallModeMask) != 0) {
            return 1;
        }
    }
    return 0;
}

static void UnduCheckSubCrossCheck(UnduWork *work, UnduQuery *query, int subOffset)
{
    UnduTriangle *triangle = (UnduTriangle *)subOffset;
    UnduQueryParameters *parameters = &query->prefix.parameters;
    UnduTriangle *neighbor;
    UnduVertex *vertexA;
    UnduVertex *vertexB;
    UnduVertex *vertexC;
    UnduVertex *edgeStart;
    UnduVertex *edgeEnd;
    float moveSlope;
    float edgeSlope;
    UnduVertex *normal;
    float *move;
    float edgeAB;
    float edgeBC;
    float edgeCA;
    float edgeQ;
    float sideStart;
    float sideEnd;
    float low;
    float high;
    float margin;
    float py;
    float highLimit;
    float lowZ;
    float highZ;
    float lowX;
    float highX;
    float dx;
    float moveDx;
    float dz;
    float moveDz;
    float edgeDx;
    float edgeDz;
    float crossX;
    float crossZ;
    float moveDistance;
    float distance;
    float scale;
    float crossLength;
    float angle;
    float moveAngle;
    float turn;
    float flipped;
    float *hitPoint = work->hitPoint;
    float *collisionPoint = work->collisionPoint;
    float cx;
    float cz;
    float startX;
    float startZ;
    int status;
    int inside;
    int mode;
    unsigned int flags;

    work->crossCheckCountX = parameters->crossCheckCountX;
    if (work->crossCheckCountX != 0) {
        float nearZeroMax;
        float nearZeroMin;

        nearZeroMin = -0.01f;
        nearZeroMax = 0.01f;

        do {
            vertexC = 0;
            vertexB = 0;
            vertexA = 0;
            status = 0;
            work->crossCheckCountZ = parameters->crossCheckCountZ;
            if (work->crossCheckCountZ != 0) {
                do {
                    vertexA = (UnduVertex *)(work->vertexData + triangle->vertexOffset[0]);
                    vertexB = (UnduVertex *)(work->vertexData + triangle->vertexOffset[1]);
                    vertexC = (UnduVertex *)(work->vertexData + triangle->vertexOffset[2]);
                    edgeAB = UNDU_ORIENT(hitPoint[0], hitPoint[2], vertexA->x, vertexA->z, vertexB->x, vertexB->z);
                    edgeBC = UNDU_ORIENT(hitPoint[0], hitPoint[2], vertexB->x, vertexB->z, vertexC->x, vertexC->z);
                    edgeCA = UNDU_ORIENT(hitPoint[0], hitPoint[2], vertexC->x, vertexC->z, vertexA->x, vertexA->z);
                    if (edgeAB <= 0.0f && edgeBC <= 0.0f && edgeCA <= 0.0f) {
                        flags = (unsigned short)parameters->flags;
                        inside = 0;
                        if (flags & 0x400) {
                            low = vertexA->y;
                            high = vertexA->y;
                            if (vertexA->y > vertexB->y)
                                low = vertexB->y;
                            if (vertexC->y < low)
                                low = vertexC->y;
                            margin = work->heightTolerance;
                            low += margin;
                            if (high < vertexB->y)
                                high = vertexB->y;
                            if (high < vertexC->y)
                                high = vertexC->y;
                            py = hitPoint[1];
                            highLimit = high - margin;
                            if (flags & 0x10) {
                                if (highLimit < py) {
                                    if (flags & 0x800)
                                        inside = 1;
                                    else if (low > py)
                                        inside = 1;
                                }
                            } else {
                                if (!(flags & 0x800) || highLimit < py)
                                    inside = 1;
                            }
                        } else {
                            inside = 1;
                        }
                        if (inside == 1) {
                            normal = (UnduVertex *)(work->normalData + triangle->normalOffset);
                            status = 4;
                            UnduCheckSubHeightCheck(work, (int)triangle,
                                (triangle->planeOffset - normal->x * work->hitPoint[0]
                                    - normal->z * work->hitPoint[2]) / normal->y);
                            break;
                        }
                    }
                    edgeQ = UNDU_ORIENT(collisionPoint[0], collisionPoint[2], vertexA->x, vertexA->z, vertexB->x, vertexB->z);
                    if (edgeQ <= 0.0f && edgeAB >= 0.0f && (edgeQ != 0.0f || edgeAB != 0.0f)) {
                        sideStart = UNDU_ORIENT(vertexA->x, vertexA->z, collisionPoint[0], collisionPoint[2],
                                                hitPoint[0], hitPoint[2]);
                        sideEnd = UNDU_ORIENT(vertexB->x, vertexB->z, collisionPoint[0], collisionPoint[2],
                                              hitPoint[0], hitPoint[2]);
                        if ((sideStart <= 0.0f && sideEnd >= 0.0f) || (sideStart >= 0.0f && sideEnd <= 0.0f)) {
                            if (triangle->neighbor[0] == -1) {
                                status = 1;
                                break;
                            }
                            neighbor = (UnduTriangle *)(work->subListOffset + triangle->neighbor[0]);
                            if (CheckWallAttr(work, &neighbor->wallAttributeIndex) != 0) {
                                status = 0x101;
                                break;
                            }
                            triangle = neighbor;
                            continue;
                        }
                    }
                    edgeQ = UNDU_ORIENT(collisionPoint[0], collisionPoint[2], vertexB->x, vertexB->z, vertexC->x, vertexC->z);
                    if (edgeQ <= 0.0f && edgeBC >= 0.0f && (edgeQ != 0.0f || edgeBC != 0.0f)) {
                        sideStart = UNDU_ORIENT(vertexB->x, vertexB->z, collisionPoint[0], collisionPoint[2],
                                                hitPoint[0], hitPoint[2]);
                        sideEnd = UNDU_ORIENT(vertexC->x, vertexC->z, collisionPoint[0], collisionPoint[2],
                                              hitPoint[0], hitPoint[2]);
                        if ((sideStart <= 0.0f && sideEnd >= 0.0f) || (sideStart >= 0.0f && sideEnd <= 0.0f)) {
                            if (triangle->neighbor[1] == -1) {
                                status = 2;
                                break;
                            }
                            neighbor = (UnduTriangle *)(work->subListOffset + triangle->neighbor[1]);
                            if (CheckWallAttr(work, &neighbor->wallAttributeIndex) != 0) {
                                status = 0x102;
                                break;
                            }
                            triangle = neighbor;
                            continue;
                        }
                    }
                    edgeQ = UNDU_ORIENT(collisionPoint[0], collisionPoint[2], vertexC->x, vertexC->z, vertexA->x, vertexA->z);
                    if (edgeQ <= 0.0f && edgeCA >= 0.0f && (edgeQ != 0.0f || edgeCA != 0.0f)) {
                        sideStart = UNDU_ORIENT(vertexC->x, vertexC->z, collisionPoint[0], collisionPoint[2],
                                                hitPoint[0], hitPoint[2]);
                        sideEnd = UNDU_ORIENT(vertexA->x, vertexA->z, collisionPoint[0], collisionPoint[2],
                                              hitPoint[0], hitPoint[2]);
                        if ((sideStart <= 0.0f && sideEnd >= 0.0f) || (sideStart >= 0.0f && sideEnd <= 0.0f)) {
                            if (triangle->neighbor[2] == -1) {
                                status = 3;
                                break;
                            }
                            neighbor = (UnduTriangle *)(work->subListOffset + triangle->neighbor[2]);
                            if (CheckWallAttr(work, &neighbor->wallAttributeIndex) != 0) {
                                status = 0x103;
                                break;
                            }
                            triangle = neighbor;
                            continue;
                        }
                    }
                    break;
                } while (--work->crossCheckCountZ != 0);
            }
            if (status == 0 || status == 4)
                break;
            work->collisionType = status;
            if ((status & 0xff00) == 0) {
                if (parameters->flags & 0x28) {
                    UnduCheckSubCheckAll(work);
                    if (work->collisionAttribute != 0) {
                        if (CheckWallAttr(work, work->collisionAttribute) == 0) {
                            if (parameters->flags & 8) {
                                if (work->hitPoint[1] > work->collisionHeight)
                                    break;
                            } else if (work->collisionHeight > work->hitPoint[1] - work->heightTolerance
                                       && work->collisionHeight < work->hitPoint[1] + work->heightTolerance) {
                                break;
                            }
                        }
                        work->collisionAttribute = 0;
                        work->collisionHeight = -1000.0f;
                    }
                }
            }
            mode = parameters->flags & 7;
            status &= 0xff;
            switch (mode) {
            case 2:
                moveDistance = 0.0f;
                moveAngle = 0.0f;
                angle = 0.0f;
                move = (float *)work->hasCollisionPoint;
                if (move != 0) {
                    moveAngle = atan2f(move[0], move[2]);
                    moveDistance = __builtin_sqrtf(move[0] * move[0] + move[2] * move[2]);
                }
                switch (status) {
                case 1:
                    angle = atan2f(vertexB->x - vertexA->x, vertexB->z - vertexA->z);
                    break;
                case 2:
                    angle = atan2f(vertexC->x - vertexB->x, vertexC->z - vertexB->z);
                    break;
                case 3:
                    angle = atan2f(vertexA->x - vertexC->x, vertexA->z - vertexC->z);
                    break;
                }
                turn = angle - moveAngle;
                while (turn < -1.5707964f)
                    turn += 6.2831855f;
                flipped = turn - 3.1415927f;
                if (0.0f > turn)
                    turn = -turn;
                if (0.0f > flipped)
                    flipped = -flipped;
                if (turn > flipped) {
                    angle += 3.1415927f;
                    turn = flipped;
                }
                distance = moveDistance * xglCos(turn);
                if (work->hasCollisionPoint != 0) {
                    hitPoint[0] = work->collisionPoint[0] + xglSin(angle) * distance;
                    hitPoint[2] = work->collisionPoint[2] + xglCos(angle) * distance;
                }
                triangle = (UnduTriangle *)subOffset;
                status = 0;
                break;
            case 3:
                edgeStart = 0;
                edgeEnd = 0;
                switch (status) {
                case 1:
                    edgeStart = vertexA;
                    edgeEnd = vertexB;
                    break;
                case 2:
                    edgeStart = vertexB;
                    edgeEnd = vertexC;
                    break;
                case 3:
                    edgeStart = vertexC;
                    edgeEnd = vertexA;
                    break;
                }
                cx = collisionPoint[0];
                cz = collisionPoint[2];
                startX = edgeStart->x;
                startZ = edgeStart->z;
                moveDx = hitPoint[0] - cx;
                highZ = hitPoint[2];
                moveDz = hitPoint[2] - cz;
                if (moveDx > nearZeroMin && moveDx < nearZeroMax)
                    moveSlope = 0.0f;
                else if (nearZeroMin < moveDz && moveDz < nearZeroMax)
                    moveSlope = unduInitialSlopeTable[0];
                else
                    moveSlope = moveDz / moveDx;
                edgeDx = edgeEnd->x - startX;
                edgeDz = edgeEnd->z - startZ;
                if (nearZeroMin < edgeDx && edgeDx < nearZeroMax)
                    edgeSlope = 0.0f;
                else if (nearZeroMin < edgeDz && edgeDz < nearZeroMax)
                    edgeSlope = unduInitialSlopeTable[0];
                else
                    edgeSlope = edgeDz / edgeDx;
                if (unduSlopeBits(&moveSlope) == 0) {
                    if (unduSlopeBits(&edgeSlope) == 0) {
                        lowZ = highZ;
                        highZ = startZ;
                        crossX = startX;
                        if (startZ < lowZ) {
                            highZ = lowZ;
                            lowZ = startZ;
                        }
                        if (edgeEnd->z < highZ)
                            highZ = edgeEnd->z;
                        if (highZ < lowZ)
                            highZ = lowZ;
                        crossZ = highZ;
                    } else {
                        crossX = collisionPoint[0];
                        crossZ = edgeSlope * (collisionPoint[0] - startX) + startZ;
                    }
                } else if (unduSlopeBits(&edgeSlope) == 0) {
                    crossX = startX;
                    crossZ = moveSlope * (startX - collisionPoint[0]) + collisionPoint[2];
                } else if (unduSlopeBits(&moveSlope) == 0x80000000) {
                    if (unduSlopeBits(&edgeSlope) == 0x80000000) {
                        highX = startX;
                        lowX = hitPoint[0];
                        if (startX < hitPoint[0]) {
                            highX = hitPoint[0];
                            lowX = startX;
                        }
                        if (highX > edgeEnd->x)
                            highX = edgeEnd->x;
                        if (highX < lowX)
                            highX = lowX;
                        crossX = highX;
                        crossZ = startZ;
                    } else {
                        crossX = (collisionPoint[2] - startZ) / edgeSlope + startX;
                        crossZ = collisionPoint[2];
                    }
                } else if (unduSlopeBits(&edgeSlope) == 0x80000000) {
                    crossX = (startZ - collisionPoint[2]) / moveSlope + collisionPoint[0];
                    crossZ = startZ;
                } else {
                    crossX = (edgeSlope * startX - startZ
                              - moveSlope * collisionPoint[0] + collisionPoint[2])
                        / (edgeSlope - moveSlope);
                    crossZ = moveSlope * (crossX - collisionPoint[0]) + collisionPoint[2];
                }
                dx = crossX - collisionPoint[0];
                dz = crossZ - collisionPoint[2];
                crossLength = __builtin_sqrtf(dx * dx + dz * dz);
                dx = hitPoint[0] - collisionPoint[0];
                dz = hitPoint[2] - collisionPoint[2];
                scale = (crossLength - 0.001f) / __builtin_sqrtf(dx * dx + dz * dz);
                status = 4;
                work->collisionAttribute = &triangle->wallAttributeIndex;
                normal = (UnduVertex *)(work->normalData + triangle->normalOffset);
                crossX = collisionPoint[0] + dx * scale;
                crossZ = collisionPoint[2] + dz * scale;
                work->hitPoint[2] = crossZ;
                work->hitPoint[0] = crossX;
                work->hitPoint[1] = work->collisionHeight = (triangle->planeOffset - normal->x * work->hitPoint[0]
                                     - normal->z * work->hitPoint[2]) / normal->y;
                break;
            case 1:
            default:
                status = 4;
                break;
            }
            if (status == 4)
                break;
        } while (--work->crossCheckCountX != 0);
    }
    if (work->crossCheckCountX == 0) {
        work->collisionAttribute = (unsigned short *)subOffset;
        work->collisionHeight = parameters->height;
        work->hitPoint[0] = work->collisionPoint[0];
        work->hitPoint[1] = work->collisionPoint[1];
        work->hitPoint[2] = work->collisionPoint[2];
    }
}

static float UnduCheckResultCheck(UnduWork *work, UnduResult *result, float *point)
{
    result->flags &= ~0x1000;
    result->collisionType = work->collisionType & 3;
    result->height = work->collisionHeight;
    if (work->collisionAttribute != 0) {
        result->previousAttribute = result->attribute;
        result->attribute = work->collisionAttribute;
        if (work->wallAttributeTable != 0) {
            UnduWallAttributes *entry = &work->wallAttributeTable[*work->collisionAttribute];
            UnduWallAttributes *attributes = &result->attributes;
            attributes->words[0] = entry->words[0];
            attributes->words[1] = entry->words[1];
        } else {
            result->attributes.bits = *work->collisionAttribute;
        }
        if (work->hasCollisionPoint != 0) {
            point[0] = work->hitPoint[0];
            point[1] = work->hitPoint[1];
            point[2] = work->hitPoint[2];
        }
    } else {
        result->attribute = 0;
        result->attributes.bits = 0;
        if ((result->flags & 7) == 1 && work->hasCollisionPoint != 0) {
            point[0] = work->hitPoint[0];
            point[1] = work->hitPoint[1];
            point[2] = work->hitPoint[2];
        }
    }
    return work->collisionHeight;
}

float UnduCheck(float *sample, void *output, UnduQuery *query)
{
    UnduWork *work = query->prefix.parameters.scratchpad;
    /* Return address of this call, stored at the frame bottom by the sw $31
     * below; volatile keeps that store and the later reload by printf in place. */
    unsigned int volatile caller;
    UnduDataHeader *header;
    unsigned char *subList;
    unsigned short *attribute;
    float *offset;
    float height;
    float x;
    float y;
    float z;

    __asm__ __volatile__("sw $31,%0" : "=m"(caller));
    if (work == 0)
        work = (UnduWork *)0x70000000;
    if (CurrentColiHead == 0)
        return 0.0f;
    header = query->prefix.parameters.header;
    if (header == 0) {
        if ((CurrentColiHead[0] & 0x7f) != 2)
            return 0.0f;
        header = ((UnduColiHead *)CurrentColiHead)->directHeaders;
    }
    work->heightTolerance = query->prefix.parameters.heightLimit;
    work->collisionHeight = -1000.0f;
    work->vertexData = header->vertexData;
    work->collisionPoint[0] = sample[0];
    work->collisionPoint[1] = sample[1];
    subList = header->subList;
    work->subCount = header->subCount;
    work->normalData = header->normalData;
    work->subListOffset = (int)subList;
    work->wallAttributeTable = header->wallAttributeTable;
    work->collisionPoint[2] = sample[2];
    work->collisionType = 0;
    work->wallAttributeMask = query->excludedAttributes;
    work->wallAttributeValue = query->includedAttributes;
    work->wallAttrHeader = (UnduWallAttrHeader *)query;
    work->collisionAttribute = 0;
    work->hasCollisionPoint = (int)output;
    if (output != 0
        && (((int *)output)[0] == 0x7fffffff || ((int *)output)[1] == 0x7fffffff
            || ((int *)output)[2] == 0x7fffffff))
        work->hasCollisionPoint = 0;
    if (query->prefix.parameters.previousAttribute == 0
        || (*(long long *)&query->prefix.result.flags & 0x5000)
        || (GameLoopState.flags & 0x20)) {
        work->hitPoint[0] = work->collisionPoint[0];
        work->hitPoint[1] = work->collisionPoint[1];
        work->hitPoint[2] = work->collisionPoint[2];
        UnduCheckSubCheckAll(work);
        height = UnduCheckResultCheck(work, &query->prefix.result, sample);
        if (height == -1000.0f || work->hasCollisionPoint == 0)
            return height;
        work->collisionHeight = -1000.0f;
        work->collisionAttribute = 0;
    }
    if (query->prefix.parameters.attribute != 0)
        attribute = query->prefix.parameters.attribute;
    else
        attribute = query->prefix.parameters.previousAttribute;
    if ((unsigned int)attribute < (unsigned int)work->subListOffset
        && (unsigned int)attribute >= work->subListOffset + work->subCount * 0x18) {
        printf("\nUnduCheck() call from %08x\n", caller);
        if (unduParamErrorMessage != 0) {
            printf(unduAssertFormat, unduAssertExpression, unduSourcePath, 1269);
            DEBUG_BREAK();
        }
    }
    x = work->collisionPoint[0];
    y = work->collisionPoint[1];
    z = work->collisionPoint[2];
    offset = (float *)work->hasCollisionPoint;
    if (offset != 0) {
        x += offset[0];
        y += offset[1];
        z += offset[2];
    }
    work->hitPoint[0] = x;
    work->hitPoint[1] = y;
    work->hitPoint[2] = z;
    height = UnduCheckSub(work, (int)attribute);
    if (height != -1000.0f)
        UnduCheckSubHeightCheck(work, (int)attribute, height);
    else if (work->hasCollisionPoint == 0)
        UnduCheckSubCheckAll(work);
    else
        UnduCheckSubCrossCheck(work, query, (int)attribute);
    height = UnduCheckResultCheck(work, &query->prefix.result, sample);
    return height;
}

float UnduGet2(int queryAddress, float x, float z)
{
    UnduQuery *query;
    UnduWork *work;
    int type;
    int hasCollisionData;

    if (CurrentColiHead == 0)
        return 0.0f;
    if (queryAddress == 0) {
        UnduParamInit((UnduQuery *)0x70000000);
        queryAddress = 0x70000000;
    }
    query = (UnduQuery *)queryAddress;
    work = query->prefix.parameters.scratchpad;
    type = CurrentColiHead[0] & 0x7f;
    if (work == 0)
        work = (UnduWork *)0x70000000;
    if (type < 3) {
        hasCollisionData = type != 0;
        if (hasCollisionData) {
            work->sample[0] = x;
            work->sample[2] = z;
            work->sample[1] = 0.0f;
            return UnduCheck(work->sample, 0, query);
        }
    }
    return query->prefix.result.height;
}

float UnduGet(float x, float z)
{
    return UnduGet2(0, x, z);
}

const char unduParamErrorMessage[24] = "sUnduParam->undu error";

const char unduAssertFormat[96] = "\n\n************************\n\nAssert(%s)\n    file = %s  line = %d\n\n************************\n\n\n";

const char unduAssertExpression[32] = "!\"sUnduParam->undu error\"";

const char unduSourcePath[24] = "../Undulate.euc.c";

const float unduInitialSlopeTable[1] = { -0.0f };
