#include "common.h"
#include "debug_break.h"
#include "main/Undulate.h"
#include "main/xgl_2.h"

#define UNDU_ORIENT(x0, z0, x1, z1, x2, z2) \
    ((x0) * (z1) + (x1) * (z2) + (x2) * (z0) - (x0) * (z2) - (x1) * (z0) - (x2) * (z1))

#define UNDU_RELOCATE(field, base) \
    do { \
        if ((field) != 0) \
            (field) = (void *)((unsigned char *)(field) + (unsigned int)(base)); \
    } while (0)

#define UNDU_RELOCATE_BYTES(field, base) \
    do { \
        if ((field) != 0) \
            (field) = (field) + (unsigned int)(base); \
    } while (0)

#define UNDU_RELOCATE_HEADER(header, base) \
    do { \
        UNDU_RELOCATE_BYTES((header)->vertexData, base); \
        UNDU_RELOCATE_BYTES((header)->normalData, base); \
        UNDU_RELOCATE_BYTES((header)->subList, base); \
        UNDU_RELOCATE((header)->wallAttributeTable, base); \
    } while (0)

#define UNDU_RELOCATE_HEADERS(header, list, count, index, base) \
    do { \
        (header) = (list); \
        for ((index) = 0; (index) < (count); (index)++, (header)++) \
            UNDU_RELOCATE_HEADER(header, base); \
    } while (0)

/* UnduColiBlob.tables.type: the kind in the low bits, set once the offsets are relocated. */
#define UNDU_COLI_RELOCATED 0x80
#define UNDU_COLI_KIND_MASK 0x7f
#define UNDU_COLI_KIND_HEAD 2

/* UnduDataGetHeader map_index: kind of header list in bits 8..15, selector in the low byte. */
#define UNDU_MAP_KIND_MASK 0xff00
#define UNDU_MAP_INDEX_MASK 0xff
enum {
    UNDU_MAP_KIND_DIRECT = 0x000,   /* directHeaders */
    UNDU_MAP_KIND_SECOND = 0x100,   /* secondHeaders */
    UNDU_MAP_KIND_GROUP = 0x200,    /* groups[low byte] */
    UNDU_MAP_KIND_RECORD = 0x300    /* records[low byte] */
};

/* UnduParamInit defaults: collision samples per axis. */
#define UNDU_CROSS_CHECK_COUNT 16

/* Collision triangles are 0x18 bytes apart in a header's sub-primitive list. */
#define UNDU_TRIANGLE_STRIDE 0x18

/* Wall attribute word: kind in the top bits, height-table slot in bits 8..12. */
#define UNDU_ATTR_KIND_MASK 0xe000
#define UNDU_ATTR_KIND_HEIGHT_TABLE 0x4000
#define UNDU_ATTR_HEIGHT_SLOT_MASK 0x1f00

/* UnduCheckSubCrossCheck status: which triangle edge the move crossed. */
enum {
    UNDU_EDGE_AB = 1,
    UNDU_EDGE_BC = 2,
    UNDU_EDGE_CA = 3,
    UNDU_CROSS_DONE = 4,          /* the move was resolved on this triangle */
    UNDU_EDGE_NEIGHBOR = 0x100    /* with an edge: the neighbouring triangle passes CheckWallAttr */
};
#define UNDU_EDGE_MASK 0xff
#define UNDU_EDGE_NEIGHBOR_MASK 0xff00

/* UnduQuery.output component meaning "no move vector". */
#define UNDU_NO_MOVE 0x7fffffff

/* IEEE bit pattern of -0.0f, compared by the slope solver. */
#define UNDU_NEGATIVE_ZERO_BITS 0x80000000

extern const float unduInitialSlopeTable[];
extern const char unduParamErrorMessage[];
extern const char unduAssertFormat[];
extern const char unduAssertExpression[];
extern const char unduSourcePath[];

extern float atan2f(float y, float x);
extern int printf(const char *format, ...);
int StudioEntrySkip = 0;

signed char FLAG_FRAME_60 = 0;

const float unduInitialSlopeTable[1] = { -0.0f };

static union UnduColiBlob *CurrentColiHead;

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

struct UnduDataHeader;

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

typedef struct UnduColiGroup {
    unsigned short firstIdentifier;
    unsigned short count;
    float (*headers)[4];
} UnduColiGroup;

typedef struct UnduColiRecord {
    short count; /* The list ends at the first negative count. */
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

/* The collision blob is one of these kinds; its type byte is the first byte of each. */
typedef union UnduColiBlob {
    UnduColiTables tables;
    UnduColiHead head;
} UnduColiBlob;

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
    struct UnduQueryParameters *wallAttrHeader;
    int subCount;           /* 0x54: number of collision sub-primitives to test */
    unsigned char *vertexData;
    unsigned char *normalData;
    unsigned int subListOffset; /* 0x60: byte offset of the first sub-primitive; entries are 0x18 bytes apart */
    UnduWallAttributes *wallAttributeTable;
    unsigned char unmodeled_68[8];
    float collisionPoint[3];
    unsigned char unmodeled_7c[4];
    float hitPoint[3];
    unsigned char unmodeled_8c[4];
    float *output;          /* 0x90: the caller's move vector; null when none was given */
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
    unsigned short flags;
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

/* Declared here, not at the top: moving it up changes the original code. */
static float UnduCheckSub(UnduWork *work, int subOffset);

static int UnduCheckSubHeightCheck(UnduWork *work, int subOffset, float height);
float UnduCheck(float *sample, void *output, UnduQuery *query);
static void UnduCheckSubCrossCheck(UnduWork *work, UnduQuery *query, int subOffset);

/*
 * Types 1 and 2 are walked through volatile pointers: the original reads every
 * pointer field again after its null test, and rereads each list head and count
 * after the stores. The type 0 table is read once per field.
 */
void UnduInit(union UnduColiBlob *data)
{
    UnduColiTables *flat = &data->tables;
    volatile UnduColiHead *head = &data->head;
    volatile UnduDataHeader *header;
    volatile UnduColiGroup *group;
    volatile UnduColiRecord *record;

    CurrentColiHead = data;
    if (data == 0)
        return;
    if (data->tables.type & UNDU_COLI_RELOCATED)
        return;
    switch (data->tables.type) {
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
            UNDU_RELOCATE(group->headers, data);
        if ((void *)&head->records != (void *)head->directHeaders) {
            UNDU_RELOCATE(head->records, data);
            record = head->records;
            while (record->count >= 0) {
                UNDU_RELOCATE(record->headers, data);
                UNDU_RELOCATE_HEADERS(header, record->headers, record->count, i, data);
                record++;
            }
        }
        break;
    }
    default:
        break;
    }
    data->tables.type |= UNDU_COLI_RELOCATED;
}

static LayoutHeader *UnduDataGetHeaderSub(UnduDataHeader *headers,
                                          unsigned int count,
                                          unsigned int identifier)
{
    unsigned int index;
    UnduDataHeader *header;

    if (identifier & UNDU_HEADER_DIRECT) {
        identifier &= UNDU_HEADER_INDEX_MASK;
        if (identifier < count)
            return (LayoutHeader *)&headers[identifier];
    } else {
        header = headers;
        for (index = 0; index < count; index++, header++) {
            if (header->identifier == identifier)
                return (LayoutHeader *)header;
        }
    }
    return 0;
}

LayoutHeader *UnduDataGetHeader(int map_index, unsigned int unit_index)
{
    UnduColiHead *headerIndex;
    UnduColiGroup *group;
    UnduColiRecord *subList;
    float (*header)[4];
    unsigned int group_index;
    unsigned int header_index;
    int entry_index;
    unsigned int identifier;

    headerIndex = &CurrentColiHead->head;
    if (headerIndex == 0) {
        return 0;
    }
    if (headerIndex->type != (UNDU_COLI_RELOCATED | UNDU_COLI_KIND_HEAD)) {
        return 0;
    }

    switch ((unsigned int)map_index & UNDU_MAP_KIND_MASK) {
    case UNDU_MAP_KIND_DIRECT:
        return UnduDataGetHeaderSub(
            headerIndex->directHeaders, headerIndex->directCount,
            unit_index);
    case UNDU_MAP_KIND_SECOND:
        return UnduDataGetHeaderSub(
            headerIndex->secondHeaders, headerIndex->secondCount,
            unit_index);
    case UNDU_MAP_KIND_GROUP:
        group_index = (unsigned int)map_index & UNDU_MAP_INDEX_MASK;
        group = headerIndex->groups;
        if (group_index < headerIndex->groupCount) {
            group = &group[group_index];
            if ((unit_index & UNDU_HEADER_DIRECT) != 0) {
                header_index = unit_index & UNDU_HEADER_INDEX_MASK;
                if (header_index < group->count) {
                    return (LayoutHeader *)&group->headers[header_index];
                }
                return 0;
            }
            header = group->headers;
            identifier = group->firstIdentifier;
            for (entry_index = 0; entry_index < group->count;
                 entry_index++, header++, identifier++) {
                if (identifier == unit_index) {
                    return (LayoutHeader *)header;
                }
            }
        }
        break;
    case UNDU_MAP_KIND_RECORD:
        if ((void *)&headerIndex->records != (void *)headerIndex->directHeaders) {
            subList = headerIndex->records;
            for (; subList->count >= 0; subList++) {
                if (((unsigned int)map_index & UNDU_MAP_INDEX_MASK) == 0) {
                    return UnduDataGetHeaderSub(
                        subList->headers,
                        subList->count,
                        unit_index);
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
    unsigned char crossCheckCount = UNDU_CROSS_CHECK_COUNT;
    UnduWork *scratchpad = (UnduWork *)SCRATCHPAD_BASE;

    query->prefix.result.attribute = 0;
    query->prefix.result.previousAttribute = 0;
    query->prefix.result.flags = 0;
    query->prefix.result.collisionType = 0;
    query->prefix.parameters.crossCheckCountX = crossCheckCount;
    query->prefix.parameters.crossCheckCountZ = crossCheckCount;
    query->prefix.parameters.heightLimit = 1.0f;
    query->prefix.result.height = UNDU_NO_HEIGHT;
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
        if (flags & UNDU_QUERY_VERTEX_RANGE) {
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
            if (flags & UNDU_QUERY_SAME_LEVEL) {
                if (highLimit < py) {
                    if (flags & UNDU_QUERY_FLOOR_BELOW)
                        inside = 1;
                    else if (py < low)
                        inside = 1;
                }
            } else {
                if (!(flags & UNDU_QUERY_FLOOR_BELOW))
                    inside = 1;
                else if (highLimit < py)
                    inside = 1;
            }
            if (inside == 0)
                return UNDU_NO_HEIGHT;
        }
        return height;
    }
    return UNDU_NO_HEIGHT;
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
    if ((flags & UNDU_QUERY_HEIGHT_TABLE) && (attributes & UNDU_ATTR_KIND_MASK) == UNDU_ATTR_KIND_HEIGHT_TABLE)
        height = GameLoopState.attributeHeights[(attributes & UNDU_ATTR_HEIGHT_SLOT_MASK) >> 8];
    status = 0;
    if (flags & UNDU_QUERY_SAME_LEVEL) {
        sampleHeight = work->hitPoint[1];
        tolerance = work->heightTolerance;
        if (sampleHeight - tolerance < height && height < sampleHeight + tolerance) {
            upperHeight = sampleHeight + tolerance;
            status = 1;
            if (upperHeight < work->collisionHeight)
                status = 2;
        } else if (flags & UNDU_QUERY_FLOOR_BELOW) {
            boundedPreviousHeight = work->collisionHeight;
            if (boundedPreviousHeight == UNDU_NO_HEIGHT || height < sampleHeight + tolerance)
                status = 1;
            else if (height < boundedPreviousHeight)
                status = 1;
        }
    } else if (flags & UNDU_QUERY_FLOOR_BELOW) {
        if (height <= work->hitPoint[1] + work->heightTolerance)
            status = 1;
    } else {
        previousHeight = work->collisionHeight;
        if (previousHeight == UNDU_NO_HEIGHT || height < work->hitPoint[1] + work->heightTolerance)
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
        subOffset += UNDU_TRIANGLE_STRIDE;
        if (height != UNDU_NO_HEIGHT) {
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
                        flags = parameters->flags;
                        inside = 0;
                        if (flags & UNDU_QUERY_VERTEX_RANGE) {
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
                            if (flags & UNDU_QUERY_SAME_LEVEL) {
                                if (highLimit < py) {
                                    if (flags & UNDU_QUERY_FLOOR_BELOW)
                                        inside = 1;
                                    else if (low > py)
                                        inside = 1;
                                }
                            } else {
                                if (!(flags & UNDU_QUERY_FLOOR_BELOW) || highLimit < py)
                                    inside = 1;
                            }
                        } else {
                            inside = 1;
                        }
                        if (inside == 1) {
                            normal = (UnduVertex *)(work->normalData + triangle->normalOffset);
                            status = UNDU_CROSS_DONE;
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
                                status = UNDU_EDGE_AB;
                                break;
                            }
                            neighbor = (UnduTriangle *)(work->subListOffset + triangle->neighbor[0]);
                            if (CheckWallAttr(work, &neighbor->wallAttributeIndex) != 0) {
                                status = UNDU_EDGE_NEIGHBOR | UNDU_EDGE_AB;
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
                                status = UNDU_EDGE_BC;
                                break;
                            }
                            neighbor = (UnduTriangle *)(work->subListOffset + triangle->neighbor[1]);
                            if (CheckWallAttr(work, &neighbor->wallAttributeIndex) != 0) {
                                status = UNDU_EDGE_NEIGHBOR | UNDU_EDGE_BC;
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
                                status = UNDU_EDGE_CA;
                                break;
                            }
                            neighbor = (UnduTriangle *)(work->subListOffset + triangle->neighbor[2]);
                            if (CheckWallAttr(work, &neighbor->wallAttributeIndex) != 0) {
                                status = UNDU_EDGE_NEIGHBOR | UNDU_EDGE_CA;
                                break;
                            }
                            triangle = neighbor;
                            continue;
                        }
                    }
                    break;
                } while (--work->crossCheckCountZ != 0);
            }
            if (status == 0 || status == UNDU_CROSS_DONE)
                break;
            work->collisionType = status;
            if ((status & UNDU_EDGE_NEIGHBOR_MASK) == 0) {
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
                        work->collisionHeight = UNDU_NO_HEIGHT;
                    }
                }
            }
            mode = parameters->flags & UNDU_QUERY_MODE_MASK;
            status &= UNDU_EDGE_MASK;
            switch (mode) {
            case UNDU_QUERY_MODE_SLIDE:
                moveDistance = 0.0f;
                moveAngle = 0.0f;
                angle = 0.0f;
                move = work->output;
                if (move != 0) {
                    moveAngle = atan2f(move[0], move[2]);
                    moveDistance = __builtin_sqrtf(move[0] * move[0] + move[2] * move[2]);
                }
                switch (status) {
                case UNDU_EDGE_AB:
                    angle = atan2f(vertexB->x - vertexA->x, vertexB->z - vertexA->z);
                    break;
                case UNDU_EDGE_BC:
                    angle = atan2f(vertexC->x - vertexB->x, vertexC->z - vertexB->z);
                    break;
                case UNDU_EDGE_CA:
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
                if (work->output != 0) {
                    hitPoint[0] = work->collisionPoint[0] + xglSin(angle) * distance;
                    hitPoint[2] = work->collisionPoint[2] + xglCos(angle) * distance;
                }
                triangle = (UnduTriangle *)subOffset;
                status = 0;
                break;
            case UNDU_QUERY_MODE_CLIP:
                edgeStart = 0;
                edgeEnd = 0;
                switch (status) {
                case UNDU_EDGE_AB:
                    edgeStart = vertexA;
                    edgeEnd = vertexB;
                    break;
                case UNDU_EDGE_BC:
                    edgeStart = vertexB;
                    edgeEnd = vertexC;
                    break;
                case UNDU_EDGE_CA:
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
                } else if (unduSlopeBits(&moveSlope) == UNDU_NEGATIVE_ZERO_BITS) {
                    if (unduSlopeBits(&edgeSlope) == UNDU_NEGATIVE_ZERO_BITS) {
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
                } else if (unduSlopeBits(&edgeSlope) == UNDU_NEGATIVE_ZERO_BITS) {
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
                status = UNDU_CROSS_DONE;
                work->collisionAttribute = &triangle->wallAttributeIndex;
                normal = (UnduVertex *)(work->normalData + triangle->normalOffset);
                crossX = collisionPoint[0] + dx * scale;
                crossZ = collisionPoint[2] + dz * scale;
                work->hitPoint[2] = crossZ;
                work->hitPoint[0] = crossX;
                work->hitPoint[1] = work->collisionHeight = (triangle->planeOffset - normal->x * work->hitPoint[0]
                                     - normal->z * work->hitPoint[2]) / normal->y;
                break;
            case UNDU_QUERY_MODE_BLOCK:
            default:
                status = UNDU_CROSS_DONE;
                break;
            }
            if (status == UNDU_CROSS_DONE)
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
        if (work->output != 0) {
            point[0] = work->hitPoint[0];
            point[1] = work->hitPoint[1];
            point[2] = work->hitPoint[2];
        }
    } else {
        result->attribute = 0;
        result->attributes.bits = 0;
        if ((result->flags & UNDU_QUERY_MODE_MASK) == 1 && work->output != 0) {
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
        work = (UnduWork *)SCRATCHPAD_BASE;
    if (CurrentColiHead == 0)
        return 0.0f;
    header = query->prefix.parameters.header;
    if (header == 0) {
        if ((CurrentColiHead->tables.type & UNDU_COLI_KIND_MASK) != UNDU_COLI_KIND_HEAD)
            return 0.0f;
        header = CurrentColiHead->head.directHeaders;
    }
    work->heightTolerance = query->prefix.parameters.heightLimit;
    work->collisionHeight = UNDU_NO_HEIGHT;
    work->vertexData = header->vertexData;
    work->collisionPoint[0] = sample[0];
    work->collisionPoint[1] = sample[1];
    subList = header->subList;
    work->subCount = header->subCount;
    work->normalData = header->normalData;
    work->subListOffset = (unsigned int)subList;
    work->wallAttributeTable = header->wallAttributeTable;
    work->collisionPoint[2] = sample[2];
    work->collisionType = 0;
    work->wallAttributeMask = query->excludedAttributes;
    work->wallAttributeValue = query->includedAttributes;
    work->wallAttrHeader = &query->prefix.parameters;
    work->collisionAttribute = 0;
    work->output = output;
    if (output != 0
        && (((int *)output)[0] == UNDU_NO_MOVE || ((int *)output)[1] == UNDU_NO_MOVE
            || ((int *)output)[2] == UNDU_NO_MOVE))
        work->output = 0;
    if (query->prefix.parameters.previousAttribute == 0
        || (*(long long *)&query->prefix.result.flags & 0x5000)
        || (GameLoopState.flags & 0x20)) {
        work->hitPoint[0] = work->collisionPoint[0];
        work->hitPoint[1] = work->collisionPoint[1];
        work->hitPoint[2] = work->collisionPoint[2];
        UnduCheckSubCheckAll(work);
        height = UnduCheckResultCheck(work, &query->prefix.result, sample);
        if (height == UNDU_NO_HEIGHT || work->output == 0)
            return height;
        work->collisionHeight = UNDU_NO_HEIGHT;
        work->collisionAttribute = 0;
    }
    if (query->prefix.parameters.attribute != 0)
        attribute = query->prefix.parameters.attribute;
    else
        attribute = query->prefix.parameters.previousAttribute;
    if ((unsigned int)attribute < work->subListOffset
        && (unsigned int)attribute >= work->subListOffset + work->subCount * UNDU_TRIANGLE_STRIDE) {
        printf("\nUnduCheck() call from %08x\n", caller);
        if (unduParamErrorMessage != 0) {
            printf(unduAssertFormat, unduAssertExpression, unduSourcePath, 1269);
            DEBUG_BREAK();
        }
    }
    x = work->collisionPoint[0];
    y = work->collisionPoint[1];
    z = work->collisionPoint[2];
    offset = work->output;
    if (offset != 0) {
        x += offset[0];
        y += offset[1];
        z += offset[2];
    }
    work->hitPoint[0] = x;
    work->hitPoint[1] = y;
    work->hitPoint[2] = z;
    height = UnduCheckSub(work, (int)attribute);
    if (height != UNDU_NO_HEIGHT)
        UnduCheckSubHeightCheck(work, (int)attribute, height);
    else if (work->output == 0)
        UnduCheckSubCheckAll(work);
    else
        UnduCheckSubCrossCheck(work, query, (int)attribute);
    height = UnduCheckResultCheck(work, &query->prefix.result, sample);
    return height;
}

float UnduGet2(void *queryAddress, float x, float z)
{
    UnduQuery *query;
    UnduWork *work;
    int type;
    int hasCollisionData;

    if (CurrentColiHead == 0)
        return 0.0f;
    if (queryAddress == 0) {
        UnduParamInit((UnduQuery *)SCRATCHPAD_BASE);
        queryAddress = (void *)SCRATCHPAD_BASE;
    }
    query = queryAddress;
    work = query->prefix.parameters.scratchpad;
    type = CurrentColiHead->tables.type & UNDU_COLI_KIND_MASK;
    if (work == 0)
        work = (UnduWork *)SCRATCHPAD_BASE;
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

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const char unduParamErrorMessage[24] = "sUnduParam->undu error";

const char unduAssertFormat[96] = "\n\n************************\n\nAssert(%s)\n    file = %s  line = %d\n\n************************\n\n\n";

const char unduAssertExpression[32] = "!\"sUnduParam->undu error\"";

const char unduSourcePath[24] = "../Undulate.euc.c";
