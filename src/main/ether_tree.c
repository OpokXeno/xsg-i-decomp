#include "common.h"
#include "ether_tree.h"

/*
 * EtherTreeObject points at a table of 80 tree-object records of 0x70 bytes
 * (EtherTreeObjectSet clears 0x2300 bytes of it; EtherTreeObjectGet matches a
 * 16-bit id at offset 0). EtherTreeObjectP is the fill cursor into that table:
 * EtherTreeObjectWorkGet hands out the current record and advances it by one.
 * No function claimed here reads a record, so the record type stays opaque.
 */
typedef struct EtherTreeObjectData EtherTreeObjectData;

extern EtherTreeObjectData *EtherTreeObject;
extern EtherTreeObjectData *EtherTreeObjectP;

void EtherTreeObjectGetClear(void)
{
    EtherTreeObjectP = EtherTreeObject;
}

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeObjectWorkGet);

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
    float centerX;
    float centerY;
    unsigned char unmodeled_18[0x08];
    float targetCenterX;
    float targetCenterY;
    unsigned char unmodeled_28[0x08];
    float nodeX;
    float nodeY;
    float nodeZ;
    float scale;
    float targetNodeX;
    float targetNodeY;
    unsigned char unmodeled_48[0x08];
    EtherTreeObjectData *targetObject;
    unsigned char unmodeled_54[0x1c];
    int windowTexAddr;
} EtherTreeSystemData;

/*
 * EtherTreeDraw (this unit, main:0x002bc4f0) stores WindowTexAddrGet(2)'s
 * result at +0x70 (sw $2,112($6)) and hands its address to endPrintExtFunc;
 * nothing else in this allocation touches the span between it and
 * targetCenterY, so it stays unmodeled.
 */
/*
 * EtherTreeFirstDataGet (main:0x002b9f30) reads firstDataIndex (offset 0x02,
 * u16). EtherTreeTargetChange (main:0x002ba750) reads the EtherTreeObjectGet
 * result into targetObject (offset 0x50) and its negated x/y into
 * targetNodeX/targetNodeY (offsets 0x40/0x44, always) and, when its mode
 * argument is 1, also into nodeX/nodeY (offsets 0x30/0x34).
 */
extern EtherTreeSystemData *EtherTreeSystem;
extern int EtherTreeFirstData[];

void *EtherTreeFirstDataGet(void)
{
    return &EtherTreeFirstData[EtherTreeSystem->firstDataIndex - 1];
}

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeObjectGet);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeLineColorGet);

extern EtherTreeObjectData *EtherTreeObjectWorkGet(void);
extern unsigned char *MenuEtherDataGet(unsigned short skillId);

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

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subPosSet);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subPosSet2);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subPosSet3);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", sub2JoutoYGet);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subJoutoPosSet);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeParaSet);

extern EtherTreeLineData *EtherTreeLine;

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

extern EtherTreeObjectData *EtherTreeObjectGet(int id);

void EtherTreeTargetChange(int id, int mode)
{
    EtherTreeObjectData *object = EtherTreeObjectGet(id);
    float nodeX;
    float nodeY;

    if (!(EtherTreeSystem->flags & 1))
        return;
    if (object == 0)
        return;

    EtherTreeSystem->targetObject = object;
    nodeX = -object->x;
    EtherTreeSystem->targetNodeX = nodeX;
    nodeY = -object->y;
    EtherTreeSystem->targetNodeY = nodeY;
    if (mode == 1) {
        EtherTreeSystem->nodeX = nodeX;
        EtherTreeSystem->nodeY = nodeY;
    }
}

extern void *memset(void *destination, int value, unsigned int count);
extern struct EtherTreeLine2Data *EtherTreeLine2;
extern struct EtherTreeRightData *EtherTreeRight;

void EtherTreeWorkClear(void)
{
    memset(EtherTreeSystem, 0, 0x80);
    memset(EtherTreeObject, 0, 0x2300);
    memset(EtherTreeLine, 0, 0x1900);
    memset(EtherTreeLine2, 0, 0x140);
    memset(EtherTreeRight, 0, 0x80);
}

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeObjectSet);

void EtherTreeCenterSet(int axisMask, float x, float y)
{
    if ((EtherTreeSystem->flags & 1) &&
        (!(axisMask & 1) || EtherTreeSystem->centerX != EtherTreeSystem->targetCenterX) &&
        (!(axisMask & 2) || EtherTreeSystem->centerY != EtherTreeSystem->targetCenterY)) {
        EtherTreeSystem->centerX = x;
        EtherTreeSystem->centerY = y;
        EtherTreeSystem->flags |= 2;
    }
}

void EtherTreeToCenterSet(float targetX, float targetY)
{
    EtherTreeSystem->targetCenterX = targetX;
    EtherTreeSystem->targetCenterY = targetY;
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

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeCenterMove);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeRightSet);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeRightModeChange);

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
    unsigned char unmodeled_02[0x1e];
    EtherTreeObjectData *object;
    unsigned char unmodeled_24[0x0c];
    int counter;
    unsigned char unmodeled_34[0x0c];
} EtherTreeRightData;

extern EtherTreeRightData *EtherTreeRight;
extern EtherTreeObjectData *EtherTreeObjectGet(int id);

void EtherTreeRightTargetChange(int id, int index)
{
    EtherTreeRightData *record = &EtherTreeRight[index];

    if (!(EtherTreeSystem->flags & 1))
        return;
    if (record->flags & 1) {
        record->object = EtherTreeObjectGet(id);
    }
}

extern void subEtherTreeRightMain(EtherTreeRightData *record);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subEtherTreeRightMain);

void EtherTreeRightMain(void)
{
    int i;

    if (!(EtherTreeSystem->flags & 1))
        return;

    for (i = 0; i < 2; i++) {
        subEtherTreeRightMain(&EtherTreeRight[i]);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subRightDraw);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeRightDraw);

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
} EtherTreeBlackData;

extern EtherTreeBlackData *EtherTreeBlack;

void EtherTreeBlackSet(void)
{
    EtherTreeBlack->counter = (EtherTreeBlack->mode = 0);
}

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeBlackModeChange);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeBlackDraw);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeBlackMain);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeLine2Set);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeLine2ModeChange);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeLine2SelectChange);

/*
 * subLine2_DrawType_1 (this unit, main:0x002bb448) draws the block's three
 * lines: each one runs from a corner of from[] to the matching corner of
 * to[], both offset by the tree node and the screen centre and scaled by
 * EtherTreeSystem, and both vertices of the pair take line colour 3 before
 * the pair goes to endPrintExtFunc(0, 0x10, ...). The bytes it leaves alone
 * stay unmodeled.
 */
typedef struct EtherTreeLine2Corner {
    float x;
    float y;
    unsigned char unmodeled_08[0x08];
} EtherTreeLine2Corner;

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
    unsigned char unmodeled_01[0x03];
    int selectedIndex;
    unsigned char unmodeled_08[0x08];
    EtherTreeLine2Corner from[3];
    EtherTreeLine2Corner to[3];
    unsigned char unmodeled_70[0x10];
    EtherTreeLine2Line line[3];
} EtherTreeLine2Data;

extern void EtherTreeLineColorGet(unsigned char *color, int colorIndex);
extern void endPrintExtFunc(int kind, int id, void *data);

/*
 * Build the three lines of the block: line[i] runs from corner from[i] to
 * corner to[i], each offset by the tree node and the screen centre and
 * scaled, and both of its vertices sit one unit in front of the node plane.
 * corner walks the four floats the pair needs (from[i].x, from[i].y,
 * to[i].x, to[i].y) and block walks the 0x40 bytes of one line record.
 */
void subLine2_DrawType_1(EtherTreeLine2Data *line2)
{
    float *node = &EtherTreeSystem->nodeX;
    float *center = &EtherTreeSystem->centerX;
    float *corner = &line2->to[0].y;
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

extern EtherTreeLine2Data *EtherTreeLine2;
extern void subLine2_DrawType_1(EtherTreeLine2Data *line2);

void EtherTreeLine2Draw(void)
{
    EtherTreeLine2Data *line2 = EtherTreeLine2;

    if (!(EtherTreeSystem->flags & 1))
        return;
    if (!(line2->flags & 1))
        return;

    subLine2_DrawType_1(line2);
}

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subLine2_OpenType_0);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subLine2_OpenType_1);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeLine2Main);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", sub2ObjectLampDraw);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeObjectDraw);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subTreeLineDraw);

void subTreeLineDraw_type_0(void)
{
}

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subTreeLineDraw_type_1);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subTreeLineDraw_type_2);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", subTreeLineDraw_type_3);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeLineDraw);

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeCursolDraw);

extern void EtherTreeObjectDraw(void);
extern void EtherTreeLineDraw(void);
extern void EtherTreeCursolDraw(void);
extern void EtherTreeRightDraw(void);
extern void EtherTreeBlackDraw(void);
extern int WindowTexAddrGet(int index);
extern void endPrintExtFunc(int kind, int id, void *data);

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

/*
 * Carves the ether-tree work areas out of a 16-byte-aligned arena and
 * returns the address just past the last one (arenaBase + 0x3e80).
 */
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

/* True for a robot ether index (annotations/slus_204.69_annotations.csv: 13 through 16). */
static int RoboEtherCheck(int etherIndex)
{
    return (unsigned int) (etherIndex - 13) < 4U;
}

INCLUDE_ASM("asm/main/nonmatchings/ether_tree", EtherTreeMain);
