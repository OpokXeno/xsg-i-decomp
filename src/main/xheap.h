/*
 * TU-local declarations of main/tu225 (src/main/xheap.c).
 */

#ifndef SRC_MAIN_XHEAP_H
#define SRC_MAIN_XHEAP_H

typedef struct {
    void *free_block;
    void *thread;
} xheap_frame;

/*
 * This is a partial view of each heap block.  The word at +4 is present in
 * the allocation header but this TU does not establish its role, so it remains
 * explicitly unmodeled rather than receiving an invented member name.
 */
typedef struct xheap_block {
    struct xheap_block *next;
    unsigned int : 32; /* +4: unmodeled */
    unsigned short category;
    unsigned short size_words;
} xheap_block;

void *xheap_push(void);

extern xheap_block *heap_top;

extern int heap_size;

extern xheap_frame *frame_stack;

extern xheap_block *freeBlock;

extern void *jthreadCurrent;

#endif /* SRC_MAIN_XHEAP_H */
