#ifndef INCLUDE_MAIN_XHEAP_H
#define INCLUDE_MAIN_XHEAP_H

#include "shared.h"

/* Functions and data used by other translation units. */
extern struct JThread *jthreadCurrent;
void *xmalloc(unsigned int size, int category);
void xheap_init(int keep_heap, xheap_block *heap, int size);
void *xheap_push(void);

#endif /* INCLUDE_MAIN_XHEAP_H */
