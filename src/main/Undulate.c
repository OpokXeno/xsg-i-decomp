#include "common.h"

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
/* The query is also the result passed by UnduCheck to
 * UnduCheckResultCheck. Both views share bytes0x00..0x27. */
typedef union UnduQueryPrefix {
    struct {
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
    } parameters;
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

extern float UnduGet2(int flag, float x, float z);

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

INCLUDE_ASM("asm/main/nonmatchings/Undulate", UnduCheckSubCrossCheck);

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

INCLUDE_ASM("asm/main/nonmatchings/Undulate", UnduCheck);

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
