/*
 * TU-local declarations of main/tu254 (src/main/act_1.c).
 */

#ifndef SRC_MAIN_ACT_1_H
#define SRC_MAIN_ACT_1_H

typedef struct MatrixHeap {
    void *heap_origin;
    void *heap_cursor;
    unsigned int heap_bound;
    void *heap_store;
    unsigned short heap_count;
    unsigned short heap_max;
} MatrixHeap;

void ACT_matrixInit(void);

extern MatrixHeap matrixHeap;

extern unsigned char actMatrix[];

extern unsigned char matrixHeapBlock[];

extern MatrixHeap *actMatrixHeap;

/*
 * The engine's actor record: the 64-entry array at main 0x0043c1e0, 0xa70-byte
 * stride (readelf gives `actor` GLOBAL OBJECT size 0x29c00, and 0x29c00 is
 * exactly 64 * 0xa70). ACT_draw and ACT_update both walk it with a loop
 * counter that starts at 0x3f (63) and counts down to 0, i.e. 64 entries,
 * corroborating the same count independently. It is the same record
 * src/main/chr.h (main/tu248), src/main/near_dir.h (main/tu257),
 * src/main/enemy_2.h (main/tu194), src/main/set_motion.h (main/tu193),
 * src/main/db_light_write.h (main/tu149) and src/main/act_2.h (main/tu255)
 * each already name `Actor` as their own TU-local view: this TU's C
 * definition of the same tag is refused as a redefinition of a name those
 * TUs own (review preflight redefinition:Actor), so this file keeps the
 * struct itself unnamed to that shared tag and calls it `ActRecord` instead,
 * exactly src/main/db_light_write.h's own precedent (`ActorHead`) for the
 * same conflict. This TU's view stops where this TU's own functions'
 * evidence stops; the shared-header need (one canonical Actor completed in
 * a single owner, the way include/shared.h documents for a type more than
 * one TU needs and no TU alone defines) is unresolved and out of this
 * attempt's allocation.
 *
 * ACT_draw and ACT_update both test the in-use id at +0x86 before touching
 * an entry (`inUseId`, matching src/main/db_light_write.h's naming and
 * evidence for the same field: "the in-use id ACT_create writes and
 * ACT_update skips a slot on when it is zero"); ACT_dispose2 clears it back
 * to zero, along with `flags` (+0x00), `update` (+0x04) and `draw` (+0x08),
 * when it releases an entry.
 *
 * ACT_update loads `update` and, when it is not null, calls it with the
 * entry as its only argument (main:0x00305f94..0x00305fa4).
 *
 * ACT_dispose2 also reads a parent link at +0x8fc (the same field
 * src/main/enemy_2.h's ACT_info evidence names "PARENT %d", right past the
 * end of src/main/act_2.h's own `Actor.animPackTables[8]`) and, when it is
 * not null, hands it to ACT_resetParent together with the entry; unless
 * asked to skip it, it also clears ten consecutive words immediately before
 * that link (+0x8d4..+0x8f8). No instruction in this TU reads or names any
 * of the ten individually, so that span stays a single documented offset
 * access instead of ten invented members (docs/style.md rule 2).
 */
#define ACTOR_COUNT 64
#define ACTOR_LINKED_BLOCK_END_OFFSET 0x8f8

typedef struct ActRecord {
    unsigned int flags;                      /* +0x00 */
    void (*update)(struct ActRecord *self);  /* +0x04 */
    void (*draw)(struct ActRecord *self);    /* +0x08 */
    unsigned char unmodeled_0c[0x86 - 0x0c];
    short inUseId;                           /* +0x86 */
    unsigned char unmodeled_88[0x8fc - 0x88];
    struct ActRecord *parent;                /* +0x8fc */
    unsigned char unmodeled_900[0xa70 - 0x900];
} ActRecord;

extern ActRecord actor[ACTOR_COUNT];

#endif /* SRC_MAIN_ACT_1_H */
