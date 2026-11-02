/*
 * TU-local declarations of main/tu254 (src/main/act_1.c).
 */

#ifndef SRC_MAIN_ACT_1_H
#define SRC_MAIN_ACT_1_H

#include "shared.h"
#include "main/act_1.h"

#define ACTOR_COUNT 64

#define ACTOR_LINKED_BLOCK_END_OFFSET 0x8f8

typedef struct MatrixHeap {
    void *heap_origin;
    void *heap_cursor;
    unsigned int heap_bound;
    void *heap_store;
    unsigned short heap_count;
    unsigned short heap_max;
} MatrixHeap;

typedef unsigned int Quadword __attribute__((mode(TI)));

/* The matrix heap stores complete hardware quadwords and byte-addressed blocks. */
typedef union ActMatrixArena {
    unsigned char bytes[0xC0000];
    Quadword quadwords[0xC0000 / 16];
} ActMatrixArena;

/*
 * The motion resource at resource[2] (+0x8d8): ACT_allocMatrix reads its
 * halfword at +0x06 as the number of matrices to allocate when the caller
 * passes a negative count (main:0x003059c8, lhu s1,6(v0)).
 */
typedef struct ActMove {
    unsigned char unmodeled_0[6];
    unsigned short matrixCount;              /* +0x06 */
} ActMove;

/*
 * The model resource at resource[0] (+0x8d0), as ACT_info's second page
 * prints it: numParts under "numParts %d", then two three-float rows under
 * "     MIN      MAX" (+0x80 first column, +0x70 second) and four floats
 * under "  CENTER" (+0x90..+0x9c).
 */
typedef struct ActModel {
    unsigned char unmodeled_0[0x40];
    int numParts;                            /* +0x40 */
    unsigned char unmodeled_44[0x70 - 0x44];
    float boxMax[3];                         /* +0x70 */
    unsigned char unmodeled_7c[0x80 - 0x7c];
    float boxMin[3];                         /* +0x80 */
    unsigned char unmodeled_8c[0x90 - 0x8c];
    float center[4];                         /* +0x90 */
} ActModel;

/*
 * ACT_info's selection state (scaffold symbol actInfo, 12 bytes at
 * main:0x00880e90): bit 0 of the first word is toggled by the debug pad and
 * enables the printer, the second word is the printed actor's slot (wraps
 * within 0..63) and the third the printed page (wraps within 0..2).
 */
typedef struct ActInfoState {
    int visible;
    int actorIndex;
    int page;
} ActInfoState;

extern MatrixHeap matrixHeap;
extern ActMatrixArena actMatrix;
extern unsigned char matrixHeapBlock[];
extern MatrixHeap *actMatrixHeap;
extern Actor actor[ACTOR_COUNT];

#endif /* SRC_MAIN_ACT_1_H */
