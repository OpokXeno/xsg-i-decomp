/*
 * TU-local declarations of main/tu179 (src/main/ether_tree.c).
 */

#ifndef SRC_MAIN_ETHER_TREE_H
#define SRC_MAIN_ETHER_TREE_H

#include "shared.h"

/* These four-float positions are copied as two aligned doublewords. */
typedef union EtherTreePosition {
    Vector4 vector;
    unsigned long long words[2];
} EtherTreePosition;

/*
 * EtherTreeLineSet (main:0x002ba6f8) reads a record's id (offset 0, matches
 * EtherTreeObjectGet's 16-bit id test) and flags (offset 0x18, bit 0x04
 * marks a non-root node); EtherTreeTargetChange (main:0x002ba750) reads its
 * screen position (offsets 0x20/0x24, float). Bytes no function claimed
 * here reads or writes stay unmodeled.
 */
struct EtherTreeObjectData {
    unsigned short id;
    unsigned char unmodeled_02[0x02];
    struct EtherTreeObjectData *parent;
    unsigned short childCount;
    unsigned char unmodeled_0a[0x02];
    struct EtherTreeObjectData *children[3];
    unsigned char flags;
    unsigned char unmodeled_19;
    signed char displayType;
    unsigned char unmodeled_1b[5];
    EtherTreePosition position;
    unsigned char unmodeled_30[0x15];
    signed char isSet;
    unsigned char unmodeled_46[2];
    short setValue;
    unsigned char unmodeled_4a[0x26];
};

/*
 * sub2ParentChildSet and subParentChildSet (main/tu179, main:0x002b9ff8 and
 * main:0x002ba0a8) build the tree by id: for up to 3 children looked up
 * through MenuEtherDataGet/EtherTreeFirstDataGet, a non-zero id claims the
 * next EtherTreeObjectWorkGet() record (id set, childCount incremented,
 * stored into children[i]) and recurses; a zero id clears children[i].
 * sub2ParentChildSet also sets each child's parent back to itself; the root
 * built by subParentChildSet leaves its own children's parent unset.
 */

/*
 * EtherTreeLine is the fill cursor into a table of 0x190-byte line records
 * (EtherTreeLineSet, this unit, main:0x002ba6f8): its first word points to
 * the EtherTreeObjectData it highlights. The six segment records are read
 * by subTreeLineDraw; each segment carries
 * two four-float endpoints and their four color bytes.
 */
typedef struct EtherTreeLineDrawSegment {
    Vector4 from;
    unsigned char fromColor[4];
    unsigned char unmodeled_14[0x0c];
    Vector4 to;
    unsigned char toColor[4];
    unsigned char unmodeled_34[0x0c];
} EtherTreeLineDrawSegment;

typedef struct EtherTreeLineData {
    struct EtherTreeObjectData *object;
    unsigned char unmodeled_04[0x0c];
    EtherTreeLineDrawSegment segments[6];
} EtherTreeLineData;

int subPosSet(struct EtherTreeObjectData *object);
void subPosSet2(struct EtherTreeObjectData *object, Vector4 *parentPosition);
void EtherTreeParaSet(struct EtherTreeObjectData *object);
void EtherTreeObjectSet(int firstDataIndex);
void EtherTreeCenterMove(void);
void EtherTreeTargetChange(int id, int mode);
void subTreeLineDraw(EtherTreeLineData *line, const int *colorIndices);

#endif
