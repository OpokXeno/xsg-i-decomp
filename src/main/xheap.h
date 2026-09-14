/*
 * TU-local declarations of main/tu225 (src/main/xheap.c).
 */

#ifndef SRC_MAIN_XHEAP_H
#define SRC_MAIN_XHEAP_H

typedef struct {
    void *free_block;
    void *thread;
} xheap_frame;

void *xheap_push(void);

extern xheap_frame *frame_stack;

extern void *freeBlock;

extern void *jthreadCurrent;

#endif /* SRC_MAIN_XHEAP_H */
