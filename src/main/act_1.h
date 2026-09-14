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

#endif /* SRC_MAIN_ACT_1_H */
