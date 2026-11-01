#include "common.h"
#include "ether_tree.h"
#include "main/xgl_dma.h"
#include "ov01/data_unit_org_get.h"

/* Only a .lit4 literal in the original (0x004D7E7C); no storage object. */
#define ETHER_TREE_NODE_Z 65535.0f

/*
 * EtherTreeObject points at a table of 80 tree-object records of 0x70 bytes
 * (EtherTreeObjectSet clears 0x2300 bytes of it; EtherTreeObjectGet matches a
 * 16-bit id at offset 0). EtherTreeObjectP is the fill cursor into that table:
 * EtherTreeObjectWorkGet hands out the current record and advances it by one.
 * No function claimed here reads a record, so the record type stays opaque.
 */
typedef struct EtherTreeObjectData EtherTreeObjectData;

typedef struct PrintSpriteInfo EtherTreeSprite;

typedef struct EtherTreeRibbonPoint {
    short x;
    short y;
    unsigned int depth;
    unsigned char color[4];
} EtherTreeRibbonPoint;

typedef struct EtherTreeLineDrawMethods {
    void (*byChildCount[4])(EtherTreeLineData *line);
} EtherTreeLineDrawMethods;

typedef struct EtherTreeGeometrySegment {
    EtherTreePosition from;
    unsigned char fromColor[4];
    unsigned char unmodeled_14[12];
    EtherTreePosition to;
    unsigned char toColor[4];
    unsigned char unmodeled_34[12];
} EtherTreeGeometrySegment;

typedef struct EtherTreeLineGeometry {
    EtherTreeObjectData *object;
    unsigned char unmodeled_04[12];
    EtherTreeGeometrySegment segments[6];
} EtherTreeLineGeometry;

typedef struct EtherTreeCoordinates {
    float x;
    float y;
} EtherTreeCoordinates;

typedef struct EtherTreeNodeTransform {
    float x;
    float y;
    float z;
    float scale;
} EtherTreeNodeTransform;

/*
 * EtherTreeCenterSet, EtherTreeToCenterSet and EtherTreeCenterMove (this
 * unit, main/tu179) all read or write this struct through EtherTreeSystem.
 * flags bit 0 gates EtherTreeCenterSet, EtherTreeCenterMove and
 * EtherTreeDraw; EtherTreeCenterSet sets bit 1 and EtherTreeCenterMove
 * tests it before sliding centerX/centerY toward targetCenterX/
 * targetCenterY with subMoveSlide. Bytes no function claimed here writes or
 * reads stay unmodeled.
 */
typedef struct EtherTreeSystemData {
    unsigned char flags;
    unsigned char unmodeled_01;
    unsigned short firstDataIndex;
    unsigned char unmodeled_04[0x0c];
    EtherTreeCoordinates center;
    unsigned char unmodeled_18[0x08];
    EtherTreeCoordinates targetCenter;
    unsigned char unmodeled_28[0x08];
    EtherTreeNodeTransform node;
    EtherTreeCoordinates targetNode;
    unsigned char unmodeled_48[0x08];
    EtherTreeObjectData *targetObject;
    EtherTreeObjectData *joutoObjects; /* +0x54 */
    unsigned char unmodeled_058[0x5c - 0x58];
    EtherTreeSprite cursor; /* +0x5c */
    unsigned char *windowTexAddr;
} EtherTreeSystemData;

typedef struct EtherTreeFirstDataEntry {
    unsigned char childIds[3];
    unsigned char unmodeled_03;
} EtherTreeFirstDataEntry;

struct EtherTreeColorTable {
    unsigned char colors[4][4];
};

/*
 * EtherTreeRight is a 2-entry array of "right panel" object-highlight slot
 * records (indexed by index*0x40 in EtherTreeRightSet, this unit, so each
 * record is 0x40 bytes). flags bit 0 marks the slot active; every function
 * here is itself gated by EtherTreeSystem->flags bit 0. mode drives a
 * 4-state open/close animation (subEtherTreeRightMain): 0 idle, 1 opening
 * (counter counts up by 8 to 48, then mode becomes 2), 2 held open (counter
 * pinned at 48), 3 closing (counter counts down by 8 to 0, then flags bit 0
 * is cleared and mode returns to 0). object is the EtherTreeObjectGet()
 * record the slot currently tracks (EtherTreeRightSet, this unit;
 * EtherTreeRightTargetChange). Bytes no function claimed here writes or
 * reads stay unmodeled.
 */
typedef struct EtherTreeRightData {
    unsigned char flags;
    unsigned char mode;
    unsigned char unmodeled_002[0x10 - 0x2];
    EtherTreePosition position; /* +0x10 */
    EtherTreeObjectData *object;
    short screenX; /* +0x24 */
    short screenY; /* +0x26 */
    unsigned int depth; /* +0x28 */
    unsigned char color[4]; /* +0x2c */
    int counter;
    int height; /* +0x34 */
    unsigned char borderColor[4]; /* +0x38 */
    unsigned char unmodeled_3c[4];
} EtherTreeRightData;

/*
 * EtherTreeBlack is the fade-to-black slot: this unit provides the same
 * Set/ModeChange/Main/Draw family for it as for EtherTreeRight. mode drives
 * its animation and counter is the frame timer (EtherTreeBlackModeChange,
 * EtherTreeBlackMain); EtherTreeBlackSet resets both to idle. Bytes no
 * function claimed here writes or reads stay unmodeled.
 */
typedef struct EtherTreeBlackData {
    unsigned char unmodeled_00;
    unsigned char mode;
    short counter;
    EtherTreeRibbonPoint points[5]; /* +0x4 */
} EtherTreeBlackData;

typedef EtherTreePosition EtherTreeLine2Corner;

/* EtherTreeLineColorGet (this unit) fills the four colour bytes at 0x10. */
typedef struct EtherTreeLine2Vertex {
    float x;
    float y;
    float z;
    unsigned char unmodeled_0c[0x04];
    unsigned char color[0x04];
    unsigned char unmodeled_14[0x0c];
} EtherTreeLine2Vertex;

typedef struct EtherTreeLine2Line {
    EtherTreeLine2Vertex from;
    EtherTreeLine2Vertex to;
} EtherTreeLine2Line;

/*
 * EtherTreeLine2 is the second lower-menu line control block, gated like
 * every function in this file by EtherTreeSystem->flags bit 0.
 * selectedIndex is the index EtherTreeLine2SelectChange last latched;
 * changing it replays EtherTreeLine2ModeChange(0) (close) before
 * EtherTreeLine2ModeChange(1) (open) on the new index. Bytes no function
 * claimed here writes or reads stay unmodeled.
 */
typedef struct EtherTreeLine2Data {
    unsigned char flags;
    unsigned char mode;
    unsigned char counter;
    unsigned char selectionType;
    int selectedIndex;
    unsigned char unmodeled_08[0x08];
    EtherTreeLine2Corner from[3];
    EtherTreePosition to[3];
    EtherTreeObjectData *object;
    unsigned char unmodeled_74[0x0c];
    EtherTreeLine2Line line[3];
} EtherTreeLine2Data;

typedef struct EtherTreeCharacterRecord {
    unsigned char unmodeled_00[0x28];
    signed char learnedEther[16];
} EtherTreeCharacterRecord;

struct EtherTreeLine2Data;

typedef struct EtherTreeMenuState {
    unsigned char unmodeled_00[3];
    unsigned char state;
    unsigned char unmodeled_04[0x1c];
    unsigned char flags;
    unsigned char unmodeled_21[0x11];
    signed char slotCursor;
    unsigned char unmodeled_33[0x0d];
    signed char chrNo;
    unsigned char unmodeled_41[3];
    signed char etherId;
    signed char slotEther;
    signed char targetChrNo;
    unsigned char unmodeled_47[9];
    int listSelect;
} EtherTreeMenuState;

extern const EtherTreeLineDrawMethods lineDrawHandlers;
extern EtherTreeMenuState MenuWork;

extern void endSpriteSet(struct PrintSpriteInfo *sprite, int flags);
extern EtherTreeObjectData *EtherTreeObjectWorkGet(void);
extern unsigned char *MenuEtherDataGet(unsigned short skillId);

extern EtherTreeObjectData *EtherTreeObjectGet(int id);

extern void *memset(void *destination, int value, unsigned int count);
extern void subEtherTreeRightMain(EtherTreeRightData *record);
extern void EtherTreeLineColorGet(unsigned char *color, int colorIndex);
extern void endPrintExtFunc(int kind, int id, void *data);

/*
 * Build the three lines of the block: line[i] runs from corner from[i] to
 * corner to[i], each offset by the tree node and the screen centre and
 * scaled, and both of its vertices sit one unit in front of the node plane.
 * corner walks the four floats the pair needs (from[i].x, from[i].y,
 * to[i].x, to[i].y) and block walks the 0x40 bytes of one line record.
 */
extern void subLine2_DrawType_1(EtherTreeLine2Data *line2);

extern void EtherTreeObjectDraw(void);
extern void EtherTreeLineDraw(void);
extern void EtherTreeCursolDraw(void);
extern void EtherTreeRightDraw(void);
extern void EtherTreeBlackDraw(void);
extern unsigned char *WindowTexAddrGet(int index);
extern EtherTreeCharacterRecord *func_A19210(int characterId);
extern int MenuEtherSetCheck(unsigned short firstDataIndex, unsigned int objectId);
extern int func_A19578(unsigned short firstDataIndex, unsigned int objectId);
extern signed char MenuEtherTypeGet(int etherId);
void subPosSet3(void);
void subJoutoPosSet(void);
void subMoveSlide(float *position, float *target, float rate);
extern int subLine2_OpenType_1(EtherTreeLine2Data *record);
extern void func_A194E0(int characterId, int etherId);
void subRightDraw(EtherTreeRightData *record);
void EtherTreeLine2ModeChange(int mode);

EtherTreeObjectData *EtherTreeObject = 0;

EtherTreeObjectData *EtherTreeObjectP = 0;

EtherTreeSystemData *EtherTreeSystem = 0;

static EtherTreeFirstDataEntry EtherTreeFirstData[7] = {
    { { 27, 0, 0 }, 0 },
    { { 17, 0, 0 }, 0 },
    { { 1, 0, 0 }, 0 },
    { { 59, 0, 0 }, 0 },
    { { 72, 0, 0 }, 0 },
    { { 37, 43, 46 }, 0 },
    { { 49, 57, 0 }, 0 }
};

EtherTreeLineData *EtherTreeLine = 0;

struct EtherTreeLine2Data *EtherTreeLine2 = 0;

struct EtherTreeRightData *EtherTreeRight = 0;

EtherTreeBlackData *EtherTreeBlack = 0;

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
const struct EtherTreeColorTable etherTreeLineColorTable __attribute__((aligned(16))) = {
    { { 0x00, 0x00, 0x00, 0x00 },
      { 0x40, 0x40, 0x40, 0x60 },
      { 0x80, 0x80, 0x80, 0x80 },
      { 0xa0, 0xa0, 0x40, 0x80 } }
};

static inline unsigned char *subRightPalettePrepare(EtherTreeRightData *record)
{
    record->borderColor[3] = 0;
    record->borderColor[0] = record->borderColor[1] = record->borderColor[2] =
        record->color[0] = record->color[1] = record->color[2] = 128;
    return record->color;
}

static inline EtherTreeObjectData *subLine2ChildAtIndex(
    unsigned int selectedIndex, EtherTreeObjectData **children)
{
    EtherTreeObjectData **selectedChild = children + selectedIndex;

    return *selectedChild;
}

static inline int subLine2SelectForY(EtherTreeLine2Data *line2,
                                   unsigned int *selection)
{
    return *selection = line2->selectedIndex;
}

static inline void subLine2SettleYAtChild(EtherTreeLine2Data *line2,
                                         int selectedIndex,
                                         EtherTreeObjectData **children)
{
    EtherTreeObjectData *child = subLine2ChildAtIndex(selectedIndex, children);

    line2->to[1].vector.y = child->position.vector.y;
    line2->to[2] = child->position;
    line2->from[2].vector = line2->to[2].vector;
    line2->from[2].vector.x -= 28.5f;
    line2->to[2].vector.x -= 28.5f;
    line2->counter = 3;
}

void EtherTreeObjectGetClear(void)
{
    EtherTreeObjectP = EtherTreeObject;
}

EtherTreeObjectData *EtherTreeObjectWorkGet(void)
{
    EtherTreeObjectData *object = EtherTreeObjectP;

    EtherTreeObjectP++;
    return object;
}

/*
 * EtherTreeFirstDataGet (main:0x002b9f30) reads firstDataIndex (offset 0x02,
 * u16). EtherTreeTargetChange (main:0x002ba750) reads the EtherTreeObjectGet
 * result into targetObject (offset 0x50) and its negated x/y into
 * targetNodeX/targetNodeY (offsets 0x40/0x44, always) and, when its mode
 * argument is 1, also into nodeX/nodeY (offsets 0x30/0x34).
 */
void *EtherTreeFirstDataGet(void)
{
    return &EtherTreeFirstData[EtherTreeSystem->firstDataIndex - 1];
}

EtherTreeObjectData *EtherTreeObjectGet(int id)
{
    int i;

    for (i = 0; i < 80; i++) {
        if (EtherTreeObject[i].id == id)
            return &EtherTreeObject[i];
    }
    return 0;
}

void EtherTreeLineColorGet(unsigned char *color, int colorIndex)
{
    struct EtherTreeColorTable colors = etherTreeLineColorTable;
    int i;

    for (i = 0; i < 4; i++)
        color[i] = colors.colors[colorIndex][i];
}

/*
 * sub2ParentChildSet and subParentChildSet (main/tu179, main:0x002b9ff8 and
 * main:0x002ba0a8) build the tree by id: for up to 3 children looked up
 * through MenuEtherDataGet/EtherTreeFirstDataGet, a non-zero id claims the
 * next EtherTreeObjectWorkGet() record (id set, childCount incremented,
 * stored into children[i]) and recurses; a zero id clears children[i].
 * sub2ParentChildSet also sets each child's parent back to itself; the root
 * built by subParentChildSet leaves its own children's parent unset.
 */
void sub2ParentChildSet(EtherTreeObjectData *object)
{
    int i;

    object->childCount = 0;
    object->flags |= 4;

    for (i = 0; i < 3; i++) {
        unsigned char id = MenuEtherDataGet(object->id)[i];

        if (id != 0) {
            EtherTreeObjectData *child = EtherTreeObjectWorkGet();

            child->id = id;
            object->children[i] = child;
            child->parent = object;
            object->childCount++;
            sub2ParentChildSet(object->children[i]);
        } else {
            object->children[i] = 0;
        }
    }
}

void subParentChildSet(void)
{
    unsigned char *ids = EtherTreeFirstDataGet();
    EtherTreeObjectData *root = EtherTreeObjectWorkGet();
    int i;

    root->childCount = 0;
    root->flags = 0x20;

    for (i = 0; i < 3; i++) {
        unsigned char id = ids[i];

        if (id != 0) {
            EtherTreeObjectData *child = EtherTreeObjectWorkGet();

            child->id = id;
            root->children[i] = child;
            root->childCount++;
            sub2ParentChildSet(child);
        } else {
            root->children[i] = 0;
        }
    }
}

int subPosSet(EtherTreeObjectData *object)
{
    int total;
    int i;

    total = 0;
    if (object->childCount == 0) {
        total = 1;
    } else {
        for (i = 0; i < object->childCount; i++) {
            float childX = 57.0f;
            float childDepthStep = 1.0f;
            float twoChildLeft = -36.0f;
            float twoChildRight = 36.0f;
            float threeChildLeft = -72.0f;
            float threeChildRight = 72.0f;
            float childDepth;
            unsigned short childCount;
            EtherTreeObjectData *child = object->children[i];

            childCount = object->childCount;
            child->position.vector.x = childX;
            childDepth = object->position.vector.z;
            child->position.vector.y = 0.0f;
            child->position.vector.z = childDepth + childDepthStep;
            switch (childCount) {
            case 2:
                if (i == 0)
                    child->position.vector.y = twoChildLeft;
                else
                    child->position.vector.y = twoChildRight;
                break;
            case 3:
                if (i == 0)
                    child->position.vector.y = threeChildLeft;
                else if (i == 2)
                    child->position.vector.y = threeChildRight;
                break;
            }

            total += subPosSet(child);
        }
    }

    if (object->position.vector.y < 0.0f)
        object->position.vector.y -= (float) (total - 1) * 36.0f;
    else if (object->position.vector.y > 0.0f)
        object->position.vector.y += (float) (total - 1) * 36.0f;

    if (EtherTreeSystem->firstDataIndex == 6 && (object->flags & 0x20))
        object->children[2]->position.vector.y += 36.0f;

    return total;
}

void subPosSet2(EtherTreeObjectData *object, Vector4 *parentPosition)
{
    EtherTreePosition position;
    int i;

    if (object == 0)
        return;

    position = object->position;
    if (parentPosition != 0) {
        position.vector.x += parentPosition->x;
        position.vector.y += parentPosition->y;
    }

    for (i = 0; i < object->childCount; i++)
        subPosSet2(object->children[i], &position.vector);

    object->position = position;
}

void subPosSet3(void)
{
    float midpointScale = 0.5f;
    EtherTreeObjectData *object = EtherTreeObject;
    int index;

    for (index = 0; index < 80; index++) {
        switch (object->childCount) {
        case 2: {
            EtherTreeObjectData *first = object->children[0];
            EtherTreeObjectData *last = object->children[1];

            object->position.vector.y = ((last->position.vector.y - first->position.vector.y) * midpointScale) + first->position.vector.y;
            break;
        }
        case 3: {
            EtherTreeObjectData *first = object->children[0];
            EtherTreeObjectData *last = object->children[2];

            object->position.vector.y = ((last->position.vector.y - first->position.vector.y) * midpointScale) + first->position.vector.y;
            break;
        }
        }
        object++;
    }
}

void sub2JoutoYGet(EtherTreeObjectData *object, float *y)
{
    if (object->childCount != 0)
        sub2JoutoYGet(object->children[object->childCount - 1], y);
    else
        *y = object->position.vector.y;
}

void subJoutoPosSet(void)
{
    unsigned short firstDataIndex = EtherTreeSystem->firstDataIndex;
    EtherTreeNodeTransform *node = &EtherTreeSystem->node;
    float baseX = 57.0f;
    EtherTreeObjectData *root = EtherTreeObjectWorkGet();
    EtherTreeObjectData *object;
    float columnSpacing = baseX;
    float baseY[4];
    int count = 0;
    int id = 1;

    EtherTreeSystem->joutoObjects = root;
    sub2JoutoYGet(EtherTreeObject, baseY);
    object = root;
    baseY[0] += 72.0f;
    do {
        if (firstDataIndex != (unsigned short)MenuEtherWhoCheck(id)) {
            if (func_A19578(firstDataIndex, id)) {
                int row = count / 4;
                int column = count - row * 4;
                unsigned short objectId = id;

                object->flags |= 8;
                object->id = objectId;
                object->position.vector.x = baseX + column * columnSpacing;
                object->position.vector.y = baseY[0] + row * 72.0f;
                object->position.vector.z = node->z;
                count++;
                object++;
            }
        }
    } while (++id < 80);
    root->childCount = count;
}

void EtherTreeParaSet(EtherTreeObjectData *object)
{
    unsigned short firstDataIndex = EtherTreeSystem->firstDataIndex;
    signed char type;

    if (object->id == 0)
        return;

    if (object->flags & 4)
        object->flags |= 8;

    if (MenuEtherSetCheck(firstDataIndex, object->id)) {
        object->isSet = 1;
        object->flags |= 2;
        object->setValue = 8;
    } else {
        object->flags &= ~2;
    }

    if (func_A19578(firstDataIndex, object->id) != 0) {
        object->flags = (object->flags | 9) & ~0x10;
    } else if (object->flags & 4) {
        object->flags |= 0x10;
    }

    type = MenuEtherTypeGet(object->id);
    switch (type) {
    case 0:
        object->displayType = 0;
        break;
    case 1:
    case 2:
        object->displayType = 1;
        break;
    default:
        object->displayType = 2;
        break;
    }
}

/* Collect the active non-root object records into the line table. */
void EtherTreeLineSet(void)
{
    EtherTreeLineData *line = EtherTreeLine;
    EtherTreeObjectData *object;
    int count;

    if (!(EtherTreeSystem->flags & 1))
        return;

    object = EtherTreeObject;
    count = 79;
    do {
        if ((object->flags & 4) && object->id != 0) {
            line->object = object;
            line++;
        }
        object++;
    } while (--count >= 0);
}

void EtherTreeTargetChange(int id, int mode)
{
    EtherTreeObjectData *object = EtherTreeObjectGet(id);
    float nodeX;
    float nodeY;

    if (!(EtherTreeSystem->flags & 1))
        return;
    if (object == 0)
        return;

    nodeX = -object->position.vector.x;
    EtherTreeSystem->targetObject = object;
    EtherTreeSystem->targetNode.x = nodeX;
    nodeY = -object->position.vector.y;
    EtherTreeSystem->targetNode.y = nodeY;
    if (mode == 1) {
        EtherTreeSystem->node.x = nodeX;
        EtherTreeSystem->node.y = nodeY;
    }
}

void EtherTreeWorkClear(void)
{
    memset(EtherTreeSystem, 0, 0x80);
    memset(EtherTreeObject, 0, 0x2300);
    memset(EtherTreeLine, 0, 0x1900);
    memset(EtherTreeLine2, 0, 0x140);
    memset(EtherTreeRight, 0, 0x80);
}

void EtherTreeObjectSet(int firstDataIndex)
{
    EtherTreeCharacterRecord *etherData;
    signed char *zeroRun;
    unsigned char *firstDataIds;
    int zeroCount;
    int i;

    memset(EtherTreeObject, 0, 0x2300);
    memset(EtherTreeLine, 0, 0x1900);
    memset(EtherTreeLine2, 0, 0x140);

    etherData = func_A19210(firstDataIndex);
    zeroRun = etherData->learnedEther;
    zeroCount = 0;
    if (zeroRun[0] == 0) {
        do {
            zeroCount++;
        } while ((++zeroRun, zeroCount < 0x10 && zeroRun[0] == 0));
    }

    if (zeroCount == 0x10)
        return;

    EtherTreeSystem->flags |= 1;
    EtherTreeSystem->firstDataIndex = (unsigned short) firstDataIndex;
    EtherTreeObjectGetClear();
    subParentChildSet();
    subPosSet(EtherTreeObject);
    subPosSet2(EtherTreeObject, 0);
    subPosSet3();
    subJoutoPosSet();
    for (i = 0; i < 80; i++)
        EtherTreeParaSet(&EtherTreeObject[i]);
    EtherTreeLineSet();
    firstDataIds = EtherTreeFirstDataGet();
    EtherTreeTargetChange(firstDataIds[0], 1);
    EtherTreeSystem->node.z = ETHER_TREE_NODE_Z;
    EtherTreeSystem->node.scale = 1.0f;
}

void EtherTreeCenterSet(float x, float y, int axisMask)
{
    EtherTreeSystemData *system = EtherTreeSystem;

    if ((system->flags & 1) &&
        (!(axisMask & 1) || system->center.x != system->targetCenter.x) &&
        (!(axisMask & 2) || system->center.y != system->targetCenter.y)) {
        system->center.x = x;
        system->center.y = y;
        system->flags |= 2;
    }
}

void EtherTreeToCenterSet(float targetX, float targetY, int unused)
{
    EtherTreeSystem->targetCenter.x = targetX;
    EtherTreeSystem->targetCenter.y = targetY;
}

/* Move a floating-point position toward its target without crossing it. */
void subMoveSlide(float *position, float *target, float rate)
{
    float difference = *position - *target;

    if (difference == 0.0f)
        return;

    if (*target < *position) {
        *position = *position - (difference * rate + 1.0f);
        if (*position < *target)
            *position = *target;
    } else {
        *position = *position - (difference * rate - 1.0f);
        if (*target < *position)
            *position = *target;
    }
}

void EtherTreeCenterMove(void)
{
    EtherTreeSystemData *system = EtherTreeSystem;
    float *centerX = &system->center.x;
    float *targetCenterX = &system->targetCenter.x;
    float *targetNodeX;
    float *nodeX;

    nodeX = &system->node.x;
    targetNodeX = &system->targetNode.x;
    if (system->flags & 1) {
        if (system->flags & 2) {
            float rate = 0.4f;

            subMoveSlide(centerX, targetCenterX, rate);
            subMoveSlide(&system->center.y, &system->targetCenter.y, rate);
            subMoveSlide(nodeX, targetNodeX, rate);
            subMoveSlide(&system->node.y, &system->targetNode.y, rate);
        }
    }
}

void EtherTreeRightSet(int id, int index)
{
    EtherTreeRightData *record = &EtherTreeRight[index];
    EtherTreeObjectData *object;
    unsigned char initialAlpha;
    unsigned char enabled = EtherTreeSystem->flags & 1;

    if (!enabled)
        return;
    object = record->object = EtherTreeObjectGet(id);
    if (object == 0)
        return;

    record->mode = 0;
    record->flags |= 1;
    record->counter = 0;
    record->position = object->position;
    initialAlpha = record->color[3] = 32;
    subRightPalettePrepare(record);
    record->height = initialAlpha;
}

void EtherTreeRightModeChange(int mode, int index)
{
    EtherTreeRightData *record = &EtherTreeRight[index];
    unsigned char currentMode;

    if (EtherTreeSystem->flags & 1) {
        if (record->flags & 1) {
            currentMode = record->mode;
            switch (currentMode) {
            case 1:
            case 2:
                if (mode == 1 || mode == 2)
                    break;
                record->mode = mode;
                break;
            case 3:
                if (mode == currentMode)
                    break;
                if (mode == 0)
                    break;
                record->mode = mode;
                break;
            case 0:
                if (currentMode == 0)
                    record->mode = mode;
                break;
            }
        }
    }
}

void EtherTreeRightTargetChange(int id, int index)
{
    EtherTreeRightData *record = &EtherTreeRight[index];

    if (!(EtherTreeSystem->flags & 1))
        return;
    if (record->flags & 1) {
        record->object = EtherTreeObjectGet(id);
    }
}

void subEtherTreeRightMain(EtherTreeRightData *record)
{
    if (EtherTreeSystem->flags & 1) {
        if (record->flags & 1) {
            switch (record->mode) {
            case 0:
                break;
            case 1:
                record->counter += 8;
                if (record->counter < 48)
                    break;
                record->mode = 2;
                /* fall through */
            case 2:
                record->counter = 48;
                break;
            case 3:
                record->counter -= 8;
                if (record->counter <= 0) {
                    record->flags &= ~1;
                    record->mode = 0;
                    record->counter = 0;
                }
                break;
            }
        }
    }
}

void EtherTreeRightMain(void)
{
    int i;

    if (!(EtherTreeSystem->flags & 1))
        return;

    for (i = 0; i < 2; i++) {
        subEtherTreeRightMain(&EtherTreeRight[i]);
    }
}

void subRightDraw(EtherTreeRightData *record)
{
    EtherTreeSystemData *system = EtherTreeSystem;
    float *positionX = &record->position.vector.x;
    float *positionY = &record->position.vector.y;
    EtherTreeNodeTransform *node;
    EtherTreeCoordinates *center;
    float screenX;
    float screenY;
    float depth;

    subMoveSlide(positionX, &record->object->position.vector.x, 0.2f);
    node = &system->node;
    center = &system->center;
    subMoveSlide(positionY, &record->object->position.vector.y, 0.2f);
    screenX = (*positionX + node->x) + center->x;
    screenY = (*positionY + node->y) + center->y;
    depth = node->z + 4.0f;
    record->screenX = screenX;
    record->screenY = screenY;
    record->depth = depth;
    endPrintExtFunc(0, 17, &record->screenX);
}

void EtherTreeRightDraw(void)
{
    int i;

    if (!(EtherTreeSystem->flags & 1))
        return;
    for (i = 0; i < 2; i++) {
        EtherTreeRightData *record = &EtherTreeRight[i];

        if (record->flags & 1) {
            if (record->mode != 0)
                subRightDraw(record);
        }
    }
}

void EtherTreeBlackSet(void)
{
    EtherTreeBlack->counter = (EtherTreeBlack->mode = 0);
}

void EtherTreeBlackModeChange(int mode)
{
    EtherTreeBlackData *record = EtherTreeBlack;
    unsigned char currentMode;

    if (EtherTreeSystem->flags & 1) {
        currentMode = record->mode;
        switch (currentMode) {
        case 1:
        case 2:
            if (mode == 1 || mode == 2)
                break;
            record->mode = mode;
            break;
        case 3:
            if (mode == currentMode)
                break;
            if (mode == 0)
                break;
            record->mode = mode;
            break;
        case 0:
            if (currentMode == 0)
                record->mode = mode;
            break;
        }
    }
    return;
}

void EtherTreeBlackDraw(void)
{
    EtherTreeSystemData *system = EtherTreeSystem;
    EtherTreeBlackData *record = EtherTreeBlack;
    EtherTreeRibbonPoint *points = record->points;
    unsigned int depth;

    if (system->flags & 1) {
        if (record->mode != 0) {
            EtherTreeRibbonPoint *lowerLeft = &record->points[1];
            EtherTreeRibbonPoint *upperRight = &record->points[2];
            EtherTreeRibbonPoint *lowerRight = &record->points[3];
            depth = system->node.z + 2.0f;
            points[0].x = 0;
            points[0].y = 0;
            points[1].x = 0;
            points[1].y = 448;
            points[2].x = 512;
            points[2].y = 0;
            points[3].x = 512;
            points[3].y = 448;
            points[0].depth = lowerLeft->depth = upperRight->depth = lowerRight->depth = depth;
            points[0].color[2] = points[1].color[2] = points[2].color[2] = points[3].color[2] = 0;
            points[0].color[1] = points[1].color[1] = points[2].color[1] = points[3].color[1] = 0;
            points[0].color[0] = points[1].color[0] = points[2].color[0] = points[3].color[0] = 0;
            points[0].color[3] = points[1].color[3] = points[2].color[3] = points[3].color[3] = record->counter;
            points[4].depth = 0;
            endPrintExtFunc(0, 3, points);
        }
    }
}

void EtherTreeBlackMain(void)
{
    EtherTreeBlackData *record = EtherTreeBlack;

    if (EtherTreeSystem->flags & 1) {
        switch (record->mode) {
        case 0:
            break;
        case 1:
            record->counter += 8;
            if (record->counter < 80)
                break;
            record->mode = 2;
            /* fall through */
        case 2:
            record->counter = 80;
            break;
        case 3:
            record->counter -= 8;
            if (record->counter <= 0) {
                record->counter = 0;
                record->mode = 0;
            }
            break;
        }
    }
}

void EtherTreeLine2Set(int id, int selectionType)
{
    EtherTreeLine2Data *record = EtherTreeLine2;
    EtherTreeObjectData *object;
    EtherTreeObjectData *nextObject;
    int i;

    if (EtherTreeSystem->flags & 1) {
        object = EtherTreeObjectGet(id);
        record->object = object;
        for (i = 0; i < object->childCount; i++, object = nextObject) {
            record->to[i].vector = object->position.vector;
            nextObject = record->object;
            record->from[i].vector = record->to[i].vector;
        }
        record->selectionType = selectionType;
        record->mode = 0;
        record->flags |= 1;
        record->counter = 0;
    }
}

void EtherTreeLine2ModeChange(int mode)
{
    EtherTreeLine2Data *record = EtherTreeLine2;
    int i;

    if (EtherTreeSystem->flags & 1) {
        if (record->flags & 1) {
            switch (mode) {
            case 1:
            case 2:
                if (record->mode == 1 || record->mode == 2)
                    break;
                record->mode = mode;
                if (record->mode == 1) {
                    record->counter = 0;
                    for (i = 0; i < 3; i++) {
                        record->to[i].vector = record->object->position.vector;
                        record->from[i].vector = record->to[i].vector;
                    }
                }
                break;
            case 3:
                if (record->mode == 3 || record->mode == 0)
                    break;
                record->mode = mode;
                break;
            case 0:
                record->mode = mode;
                break;
            }
        }
    }
}

void EtherTreeLine2SelectChange(int index)
{
    EtherTreeLine2Data *record = EtherTreeLine2;

    if (EtherTreeSystem->flags & 1) {
        if (record->flags & 1) {
            if (record->selectedIndex != index) {
                record->selectedIndex = index;
                EtherTreeLine2ModeChange(0);
            }
            EtherTreeLine2ModeChange(1);
        }
    }
}

static inline float *line2EndpointY(EtherTreeLine2Data *line2, unsigned int index)
{
    return &line2->to[index].vector.y;
}

/*
 * subLine2_DrawType_1 (this unit, main:0x002bb448) draws the block's three
 * lines: each one runs from a corner of from[] to the matching corner of
 * to[], both offset by the tree node and the screen centre and scaled by
 * EtherTreeSystem, and both vertices of the pair take line colour 3 before
 * the pair goes to endPrintExtFunc(0, 0x10, ...). The bytes it leaves alone
 * stay unmodeled.
 */
void subLine2_DrawType_1(EtherTreeLine2Data *line2)
{
    float *node = &EtherTreeSystem->node.x;
    float *center = &EtherTreeSystem->center.x;
    float *corner = &line2->to[0].vector.y;
    EtherTreeLine2Data *block = line2;
    EtherTreeLine2Line *line;
    int remaining = 2;

    do {
        line = &block->line[0];
        remaining -= 1;
        block->line[0].from.x = ((corner[-13] + node[0]) + center[0]) * node[3];
        block->line[0].from.y = ((corner[-12] + node[1]) + center[1]) * node[3];
        block->line[0].from.z = node[2] - 1.0f;
        block->line[0].to.x = ((corner[-1] + node[0]) + center[0]) * node[3];
        block->line[0].to.y = ((corner[0] + node[1]) + center[1]) * node[3];
        block->line[0].to.z = node[2] - 1.0f;
        corner += 4;
        EtherTreeLineColorGet(block->line[0].from.color, 3);
        EtherTreeLineColorGet(block->line[0].to.color, 3);
        block = (EtherTreeLine2Data *) (((char *) block) + 0x40);
        endPrintExtFunc(0, 0x10, line);
    } while (remaining >= 0);
}

void EtherTreeLine2Draw(void)
{
    EtherTreeLine2Data *line2 = EtherTreeLine2;

    if (!(EtherTreeSystem->flags & 1))
        return;
    if (!(line2->flags & 1))
        return;

    subLine2_DrawType_1(line2);
}

int subLine2_OpenType_0(EtherTreeLine2Data *line2)
{
    unsigned char counter = line2->counter;
    EtherTreeObjectData *object = line2->object;
    EtherTreeObjectData *firstChild;

    switch (counter) {
    case 0:
        line2->to[0] = object->position;
        line2->from[0].vector = line2->to[0].vector;
        line2->counter = 1;
        /* fall through */
    case 1:
        firstChild = object->children[0];
        line2->to[0].vector.x += 4.0f;
        if (line2->to[0].vector.x < firstChild->position.vector.x)
            return 0;

        line2->to[0].vector.x = firstChild->position.vector.x;
        return 1;

    default:
        return 0;
    }
}

int subLine2_OpenType_1(EtherTreeLine2Data *line2)
{
    EtherTreeObjectData *object = line2->object;
    EtherTreeObjectData **children;
    EtherTreeObjectData *child;
    unsigned int selectedIndex;
    float cornerX;

    switch (line2->counter) {
    case 0:
        line2->counter = 1;
        line2->to[0] = object->position;
        line2->from[0].vector = line2->to[0].vector;
        /* fall through */
    case 1:
        line2->to[0].vector.x += 4.0f;
        cornerX = (object->position.vector.x + 19.0f) + 9.5f;
        if (line2->to[0].vector.x < cornerX)
            return 0;
        line2->to[0].vector.x = cornerX;
        line2->to[1] = object->position;
        line2->from[1].vector = line2->to[1].vector;
        line2->from[1].vector.x += 28.5f;
        line2->to[1].vector.x += 28.5f;
        line2->counter = 2;
        /* fall through */
    case 2: {
        int ySelection = subLine2SelectForY(line2, &selectedIndex);

        if (ySelection == 0) {
            EtherTreeObjectData **directionChildren = children = object->children;
            EtherTreeObjectData *directionChild = directionChildren[0];
            float steppedY = line2->to[1].vector.y - 8.0f;

            line2->to[1].vector.y = steppedY;
            if (directionChild->position.vector.y < steppedY)
                return 0;
        } else {
            EtherTreeObjectData **directionChildren = children = object->children;
            EtherTreeObjectData *directionChild = subLine2ChildAtIndex(ySelection, directionChildren);
            float steppedY = line2->to[1].vector.y + 8.0f;

            line2->to[1].vector.y = steppedY;
            if (directionChild->position.vector.y > steppedY)
                return 0;
        }
        subLine2SettleYAtChild(line2, selectedIndex, children);
        break;
    }
    case 3:
        selectedIndex = line2->selectedIndex;
        children = object->children;
        break;
    default:
        goto pending;
    }
    child = subLine2ChildAtIndex(selectedIndex, children);
    line2->to[2].vector.x += 4.0f;
    if (line2->to[2].vector.x < child->position.vector.x)
        return 0;
    line2->to[2].vector.x = child->position.vector.x;
    return 1;
pending:
    return 0;
}

void EtherTreeLine2Main(void)
{
    EtherTreeLine2Data *record = EtherTreeLine2;
    unsigned char flags;
    unsigned char mode;
    int childCount;

    if (EtherTreeSystem->flags & 1) {
        flags = record->flags;
        if (flags & 1) {
            mode = record->mode;
            switch (mode) {
            case 0:
                break;
            case 1:
                childCount = record->object->childCount;
                switch (childCount) {
                case 0:
                    record->mode = 2;
                    break;
                case 1:
                    if (!subLine2_OpenType_0(record))
                        break;
                    /* fall through */
                case 2:
                case 3:
                    if (subLine2_OpenType_1(record))
                        record->mode = 2;
                    break;
                default:
                    record->mode = 2;
                    break;
                }
                break;
            case 2:
                break;
            case 3:
                record->mode = 0;
                record->flags = flags & ~1;
                break;
            }
        }
    }
}

void sub2ObjectLampDraw(EtherTreeObjectData *object)
{
    EtherTreeSprite *lamp = &object->lamp;
    EtherTreeNodeTransform *node = &EtherTreeSystem->node;
    float sizeScale;
    float positionScale;
    float lampWidth;
    float lampHeight;
    float originX;

    object->lampBrightness += object->setValue;
    if (object->lampBrightness < 0) {
        object->lampBrightness = 0;
        object->setValue = -object->setValue;
    } else if (object->lampBrightness > 128) {
        object->lampBrightness = 128;
        object->setValue = -object->setValue;
    }
    object->lamp.uv = 0x1203;
    endSpriteSet(lamp, 255);
    lampWidth = object->lamp.width;
    sizeScale = node->scale;
    object->lamp.color[3] = object->lampBrightness;
    originX = object->sprite.x;
    positionScale = node->scale;
    lampHeight = object->lamp.height;
    object->lamp.width = lampWidth * sizeScale;
    object->lamp.height = lampHeight * sizeScale;
    object->lamp.depth = (unsigned int)object->sprite.depth + 1;
    lamp->x = (originX + positionScale * 27.0f) - 2.0f;
    object->lamp.y = (object->sprite.y + positionScale) - 3.0f;
    endPrintExtFunc(0, 2, lamp);
}

void EtherTreeObjectDraw(void)
{
    EtherTreeObjectData *objects = EtherTreeObject;
    EtherTreeObjectData *object;
    EtherTreeSprite *sprite;
    const Vector4 *objectPosition;
    int i;
    const float fade = 0.4f;

    i = 0;
    do {
        object = &objects[i];
        sprite = &objects[i].sprite;
        objectPosition = &object->position.vector;

        if (object->flags & 8) {
            EtherTreeSystemData *system = EtherTreeSystem;
            EtherTreeNodeTransform *node = &system->node;
            EtherTreeCoordinates *center = &system->center;
            float translatedX = objectPosition->x + node->x;
            const unsigned short uvs[3] = { 0x1200, 0x1201, 0x1202 };
            Vector4 position;

            position.x = ((translatedX + center->x) - 19.0f) * node->scale;
            position.y = ((objectPosition->y + node->y + center->y) - 24.0f) * node->scale;
            position.z = node->z;
            object->sprite.uv = uvs[(unsigned char)object->displayType];
            endSpriteSet(sprite, 255);
            object->sprite.x = (int)position.x;
            object->sprite.y = (int)position.y;
            object->sprite.width = (int)(object->sprite.width * node->scale);
            object->sprite.height = (int)(object->sprite.height * node->scale);
            object->sprite.depth = (unsigned int)position.z;
            {
                EtherTreeObjectData *dimmedObject = &objects[i];

                if (object->flags & 0x10) {
                    dimmedObject->sprite.color[0] = (unsigned int)(object->sprite.color[0] * fade);
                    objects[i].sprite.color[1] = (unsigned int)(object->sprite.color[1] * fade);
                    objects[i].sprite.color[2] = (unsigned int)(object->sprite.color[2] * fade);
                }
            }
            endPrintExtFunc(0, 2, &objects[i].sprite.x);
            if (object->flags & 2)
                sub2ObjectLampDraw(&objects[i]);
        }
        i++;
    } while (i < 80);
}

void subTreeLineDraw(EtherTreeLineData *line, const int *colorIndices)
{
    EtherTreeSystemData *system = EtherTreeSystem;
    EtherTreeCoordinates *center = &system->center;
    EtherTreeNodeTransform *node = &system->node;
    int segmentsByChildCount[4] = { 0, 1, 5, 6 };
    int segmentIndex = 0;

    if (segmentsByChildCount[line->object->childCount] > 0) {
        do {
            EtherTreeLineDrawSegment *segment = &line->segments[segmentIndex];

            segment->from.x = ((segment->from.x + node->x) + center->x) * node->scale;
            segment->to.x = ((segment->to.x + node->x) + center->x) * node->scale;
            segment->from.y = ((segment->from.y + node->y) + center->y) * node->scale;
            segment->to.y = ((segment->to.y + node->y) + center->y) * node->scale;
            segment->from.z = node->z - 2.0f;
            segment->to.z = node->z - 2.0f;
            EtherTreeLineColorGet(segment->fromColor, colorIndices[segmentIndex]);
            EtherTreeLineColorGet(segment->toColor, colorIndices[segmentIndex]);
            endPrintExtFunc(0, 0x10, &segment->from.x);

            segmentIndex++;
        } while (segmentIndex < segmentsByChildCount[line->object->childCount]);
    }
}

void subTreeLineDraw_type_0(void)
{
}

void subTreeLineDraw_type_1(void *context)
{
    EtherTreeLineGeometry *line = context;
    EtherTreeObjectData *object = line->object;
    EtherTreeObjectData *child = object->children[0];
    int colorIndex;

    line->segments[0].from = object->position;
    line->segments[0].to = child->position;
    colorIndex = (child->flags & 1) ? 2 : 1;
    subTreeLineDraw(context, &colorIndex);
}

void subTreeLineDraw_type_2(void *context)
{
    EtherTreeLineGeometry *line = context;
    EtherTreeObjectData *object = line->object;
    int colorIndices[5];
    EtherTreeObjectData **colorChild;
    int *colorPair;
    int childCount;
    int activeColor;
    int inactiveColor;
    EtherTreeObjectData *geometryObject;
    EtherTreeObjectData **geometryChild;

    if (object->children[0]->flags & 1)
        colorIndices[0] = 2;
    else if (object->children[1]->flags & 1)
        colorIndices[0] = 2;
    else
        colorIndices[0] = 1;
    activeColor = 2;
    inactiveColor = 1;
    colorPair = &colorIndices[1];
    colorChild = object->children;
    childCount = 1;
    do {
        if ((*colorChild)->flags & 1)
            colorPair[0] = colorPair[1] = activeColor;
        else
            colorPair[0] = colorPair[1] = inactiveColor;
        colorChild++;
        colorPair += 2;
    } while (--childCount >= 0);

    geometryObject = line->object;
    line->segments[0].from = line->segments[0].to = geometryObject->position;
    line->segments[0].to.vector.x += 28.5f;
    geometryChild = geometryObject->children;
    for (childCount = 0; childCount < 2; childCount++) {
        EtherTreeObjectData *child = *geometryChild;
        int verticalSegment = childCount * 2 + 1;
        float junctionX;

        line->segments[verticalSegment].from =
            line->segments[verticalSegment].to = line->segments[0].to;
        junctionX = line->segments[verticalSegment].to.vector.x;
        line->segments[verticalSegment].to.vector.y = child->position.vector.y;
        line->segments[verticalSegment + 1].from =
            line->segments[verticalSegment + 1].to = child->position;
        line->segments[verticalSegment + 1].from.vector.x = junctionX;
        geometryChild++;
    }
    subTreeLineDraw(context, colorIndices);
}

void subTreeLineDraw_type_3(void *context)
{
    EtherTreeLineGeometry *line = context;
    EtherTreeObjectData *object;
    int colorIndices[7];
    int childCount;
    EtherTreeObjectData **colorChild;
    int *colorPair;
    int activeColor;
    int inactiveColor;
    EtherTreeObjectData *geometryObject;
    EtherTreeObjectData *middleChild;
    float junctionX;

    colorIndices[0] = 1;
    for (childCount = 0; childCount < 3; childCount++) {
        object = line->object;
        if (object->children[0]->flags & 1) {
            colorIndices[0] = 2;
            break;
        }
    }
    activeColor = 2;
    inactiveColor = 1;
    colorPair = &colorIndices[1];
    colorChild = line->object->children;
    childCount = 2;
    do {
        if ((*colorChild)->flags & 1)
            colorPair[0] = colorPair[1] = activeColor;
        else
            colorPair[0] = colorPair[1] = inactiveColor;
        colorChild++;
        colorPair += 2;
    } while (--childCount >= 0);

    geometryObject = line->object;
    line->segments[0].from = line->segments[0].to = geometryObject->position;
    line->segments[0].to.vector.x += 28.5f;
    for (childCount = 0; childCount < 2; childCount++) {
        EtherTreeObjectData *child = geometryObject->children[childCount * 2];
        int verticalSegment = childCount * 2 + 1;
        float branchX;

        line->segments[verticalSegment].from =
            line->segments[verticalSegment].to = line->segments[0].to;
        branchX = line->segments[verticalSegment].to.vector.x;
        line->segments[verticalSegment].to.vector.y = child->position.vector.y;
        line->segments[verticalSegment + 1].from =
            line->segments[verticalSegment + 1].to = child->position;
        line->segments[verticalSegment + 1].from.vector.x = branchX;
    }

    middleChild = geometryObject->children[1];
    junctionX = line->segments[0].to.vector.x;
    line->segments[5].from = line->segments[5].to = middleChild->position;
    line->segments[5].from.vector.x = junctionX;
    subTreeLineDraw(context, colorIndices);
}

void EtherTreeLineDraw(void)
{
    EtherTreeLineData *records = EtherTreeLine;
    EtherTreeLineDrawMethods methods = lineDrawHandlers;
    int remaining = 15;
    EtherTreeLineData *line = records;

    do {
        if (line->object != 0)
            methods.byChildCount[line->object->childCount](line);
        line++;
    } while (--remaining >= 0);
}

void EtherTreeCursolDraw(void)
{
    EtherTreeSystemData *system = EtherTreeSystem;
    EtherTreeSprite *cursor = &system->cursor;
    EtherTreeObjectData *target = system->targetObject;
    EtherTreeNodeTransform *node;
    EtherTreeCoordinates *center = &system->center;
    Vector4 *position;

    cursor->uv = 0x1204;
    endSpriteSet(cursor, 255);
    node = &system->node;
    position = &target->position.vector;
    cursor->x = (position->x - 23.0f) + (node->x + center->x);
    cursor->y = (position->y - 28.0f) + (node->y + center->y);
    cursor->width = cursor->width * node->scale;
    cursor->height = cursor->height * node->scale;
    cursor->x = cursor->x * node->scale;
    cursor->y = cursor->y * node->scale;
    cursor->depth = (unsigned int)(node->z + 2.0f);
    endPrintExtFunc(0, 2, cursor);
}

/*
 * EtherTreeDraw (this unit, main:0x002bc4f0) stores WindowTexAddrGet(2)'s
 * result at +0x70 (sw $2,112($6)) and hands its address to endPrintExtFunc;
 * nothing else in this allocation touches the span between it and
 * targetCenterY, so it stays unmodeled.
 */
void EtherTreeDraw(void)
{
    if (!(EtherTreeSystem->flags & 1))
        return;

    EtherTreeSystem->windowTexAddr = WindowTexAddrGet(2);
    endPrintExtFunc(0, 0xE, &EtherTreeSystem->windowTexAddr);
    EtherTreeObjectDraw();
    EtherTreeLineDraw();
    EtherTreeLine2Draw();
    EtherTreeCursolDraw();
    EtherTreeRightDraw();
    EtherTreeBlackDraw();
    endPrintExtFunc(0, 0xF, 0);
}

int EtherTreeInit(int arenaBase)
{
    int addr = (arenaBase + 0xf) & ~0xf;

    EtherTreeSystem = (EtherTreeSystemData *) addr;
    addr += 0x80;
    EtherTreeObject = (EtherTreeObjectData *) addr;
    addr += 0x2300;
    EtherTreeLine = (EtherTreeLineData *) addr;
    addr += 0x1900;
    EtherTreeLine2 = (EtherTreeLine2Data *) addr;
    addr += 0x140;
    EtherTreeRight = (EtherTreeRightData *) addr;
    addr += 0x80;
    EtherTreeBlack = (EtherTreeBlackData *) addr;
    addr += 0x40;
    EtherTreeWorkClear();
    EtherTreeBlackSet();
    return addr;
}

static int RoboEtherCheck(int etherIndex)
{
    return (unsigned int) (etherIndex - 13) < 4U;
}

void EtherTreeMain(void)
{
    unsigned char *modeFlag = &EtherTreeSystem->unmodeled_01;

    switch (MenuWork.state) {
    case 0x50:
    case 0x70:
    {
        if (MenuWork.flags & 1) {
            int centerYPixels = 224;
            float centerY = centerYPixels;

            EtherTreeObjectSet(MenuWork.chrNo);
            EtherTreeCenterSet(768.0f, centerY, 0);
            EtherTreeToCenterSet(384.0f, centerY, 0);
            EtherTreeTargetChange(MenuWork.etherId, 1);
            *modeFlag &= (unsigned char)~1;
            if (MenuWork.state == 0x50) {
                EtherTreeBlackModeChange(1);
                EtherTreeRightSet(MenuWork.etherId, 0);
                EtherTreeRightModeChange(1, 0);
            } else {
                EtherTreeBlackModeChange(3);
                EtherTreeRightModeChange(3, 0);
                EtherTreeRightModeChange(3, 1);
            }
        }
        {
            EtherTreeMenuState *activeMenu = &MenuWork;

            if (RoboEtherCheck(activeMenu->etherId) == 0) {
                if (EtherTreeSystem->firstDataIndex != activeMenu->chrNo) {
                    EtherTreeObjectSet(activeMenu->chrNo);
                    EtherTreeCenterSet(768.0f, 224.0f, 0);
                    EtherTreeTargetChange(activeMenu->etherId, 1);
                }
                if (activeMenu->listSelect >= 0)
                    EtherTreeTargetChange(activeMenu->etherId, 0);
                if (activeMenu->state == 0x50) {
                    EtherTreeRightTargetChange(activeMenu->etherId, 0);
                    if (PadData.half_2a & 0x20)
                        EtherTreeParaSet(EtherTreeObjectGet(activeMenu->etherId));
                }
            }
        }
        break;
    }
    case 0x74:
        if (MenuWork.flags & 1) {
            EtherTreeLine2Set(MenuWork.etherId, 0);
            EtherTreeLine2SelectChange(MenuWork.slotCursor);
            EtherTreeRightSet(MenuWork.etherId, 0);
            EtherTreeRightModeChange(1, 0);
            EtherTreeRightSet(MenuWork.slotEther, 1);
            EtherTreeRightModeChange(1, 1);
            EtherTreeBlackModeChange(1);
        }
        EtherTreeLine2SelectChange(MenuWork.slotCursor);
        EtherTreeTargetChange(MenuWork.slotEther, 0);
        EtherTreeRightTargetChange(MenuWork.slotEther, 1);
        break;
    case 0x72:
        if (!(MenuWork.flags & 1))
            break;
        EtherTreeTargetChange(MenuWork.etherId, 0);
        /* Fall through: both states close the tree panels. */
    case 0x78:
        if (MenuWork.flags & 1) {
            *modeFlag |= 1;
            EtherTreeBlackModeChange(3);
            EtherTreeRightModeChange(3, 0);
            EtherTreeRightModeChange(3, 1);
            EtherTreeLine2ModeChange(3);
            if (MenuWork.state == 0x78) {
                int i;

                for (i = 0; i < 80; ++i)
                    EtherTreeParaSet(&EtherTreeObject[i]);
                EtherTreeLineSet();
            }
        }
        break;
    case 0x82:
        if (MenuWork.flags & 1) {
            if (EtherTreeSystem->firstDataIndex != MenuWork.chrNo) {
                EtherTreeObjectSet(MenuWork.chrNo);
                EtherTreeCenterSet(768.0f, 224.0f, 0);
                EtherTreeToCenterSet(384.0f, 224.0f, 0);
                EtherTreeTargetChange(MenuWork.etherId, 1);
            }
            EtherTreeBlackModeChange(1);
            EtherTreeRightSet(MenuWork.etherId, 0);
            EtherTreeRightModeChange(1, 0);
        }
        break;
    case 0x84:
        if (MenuWork.flags & 1) {
            EtherTreeCharacterRecord *character;
            int bitIndex;
            int byteIndex;

            func_A194E0(MenuWork.targetChrNo, MenuWork.etherId);
            EtherTreeObjectSet(MenuWork.targetChrNo);
            character = func_A19210(MenuWork.targetChrNo);
            bitIndex = MenuWork.etherId - 1;
            byteIndex = bitIndex;
            if (bitIndex < 0)
                byteIndex = MenuWork.etherId + 6;
            byteIndex >>= 3;
            character->learnedEther[byteIndex] &=
                (unsigned char)~(1U << (bitIndex - byteIndex * 8));
            EtherTreeCenterSet(768.0f, 224.0f, 0);
            EtherTreeToCenterSet(384.0f, 224.0f, 0);
            EtherTreeTargetChange(MenuWork.etherId, 1);
            EtherTreeRightSet(MenuWork.etherId, 0);
            EtherTreeRightModeChange(1, 0);
        }
        break;
    case 0x86:
        if (MenuWork.flags & 1) {
            EtherTreeBlackModeChange(3);
            EtherTreeRightModeChange(3, 0);
        }
        break;
    case 0x76:
        break;
    default:
        EtherTreeRightModeChange(3, 0);
        EtherTreeRightModeChange(3, 1);
        EtherTreeBlackModeChange(3);
        EtherTreeToCenterSet(768.0f, 224.0f, 0);
        break;
    }

    EtherTreeCenterMove();
    EtherTreeLine2Main();
    EtherTreeRightMain();
    EtherTreeBlackMain();
    EtherTreeDraw();
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const EtherTreeLineDrawMethods lineDrawHandlers = { { (void (*)(EtherTreeLineData *))subTreeLineDraw_type_0, (void (*)(EtherTreeLineData *))subTreeLineDraw_type_1, (void (*)(EtherTreeLineData *))subTreeLineDraw_type_2, (void (*)(EtherTreeLineData *))subTreeLineDraw_type_3 } };
