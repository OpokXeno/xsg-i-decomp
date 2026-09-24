/*
 * TU-local declarations of main/tu179 (src/main/ether_tree.c).
 */

#ifndef SRC_MAIN_ETHER_TREE_H
#define SRC_MAIN_ETHER_TREE_H

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
    unsigned char unmodeled_19[0x07];
    float x;
    float y;
    unsigned char unmodeled_28[0x48];
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
 * (EtherTreeLineSet, this unit, main:0x002ba6f8): only a record's first word
 * is evidenced, the pointer to the EtherTreeObjectData it highlights. The
 * rest of each record is not read or written by any function claimed here,
 * so it stays unmodeled.
 */
typedef struct EtherTreeLineData {
    struct EtherTreeObjectData *object;
    unsigned char unmodeled_04[0x18c];
} EtherTreeLineData;

#endif
