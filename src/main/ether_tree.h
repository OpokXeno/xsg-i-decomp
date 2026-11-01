/*
 * TU-local declarations of main/tu179 (src/main/ether_tree.c).
 */

#ifndef SRC_MAIN_ETHER_TREE_H
#define SRC_MAIN_ETHER_TREE_H

#include "shared.h"

typedef struct PrintSpriteInfo {
    short x;
    short y;
    int depth;
    short width;
    short height;
    unsigned char color[4];
    short uv;
    unsigned char column;
    unsigned char row;
} EtherTreeSprite;

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
    EtherTreeSprite sprite; /* +0x30 */
    unsigned char unmodeled_44;
    signed char isSet;
    short lampBrightness; /* +0x46 */
    short setValue;
    unsigned char unmodeled_4a[6];
    EtherTreeSprite lamp; /* +0x50 */
    unsigned char unmodeled_64[12];
};

typedef struct EtherTreeLineDrawSegment {
    Vector4 from;
    unsigned char fromColor[4];
    unsigned char unmodeled_14[0x0c];
    Vector4 to;
    unsigned char toColor[4];
    unsigned char unmodeled_34[0x0c];
} EtherTreeLineDrawSegment;

/*
 * EtherTreeLine is the fill cursor into a table of 0x190-byte line records
 * (EtherTreeLineSet, this unit, main:0x002ba6f8): its first word points to
 * the EtherTreeObjectData it highlights. The six segment records are read
 * by subTreeLineDraw; each segment carries
 * two four-float endpoints and their four color bytes.
 */
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
