#ifndef INCLUDE_MAIN_RSRC_H
#define INCLUDE_MAIN_RSRC_H

#include "shared.h"

/*
 * Resource arena records of src/main/rsrc.c.
 *
 * The arena is one contiguous block carved out of the caller's memory:
 * general heap allocations grow upward from `memory`, the fixed-size
 * item table (item_capacity RsrcItem slots) sits directly below the
 * manager, and the manager struct itself occupies the last
 * sizeof(RsrcManager) bytes of the block. `items` is therefore reached
 * by walking back item_capacity RsrcItem slots from `manager`, and
 * `heap_bound` is the byte offset where the item table begins, i.e. the
 * heap's usable limit. act_1.c sets up its matrix heap by hand with its own
 * item table.
 */

/* An item's key: a file name, or the id an anonymous allocation is filed under. */
typedef union RsrcKey {
    u32 value;
    char *name;
} RsrcKey;

typedef struct RsrcItem {
    u32 size;
    u8 kind;
    u8 item_id;
    u16 flags;
    char *data;
    RsrcKey key;
} RsrcItem;

typedef struct RsrcManager {
    char *heap_origin;
    char *heap_cursor;
    u32 heap_bound;
    RsrcItem *items;
    u16 item_count;
    u16 item_capacity;
    u8 unmodeled_14[12];
} RsrcManager;

void RSRC_inactiveSource(RsrcManager *manager, int source);
RsrcManager *RSRC_create(void *memory, u32 memory_size, int item_capacity);
void RSRC_setPrintParam(int index, int length);
int RSRC_info(RsrcManager *manager, int mode);
char *RSRC_getDefaultPath(void);
char *RSRC_setDefaultPath(char *path);
void RSRC_init(RsrcManager *manager);
int RSRC_check(RsrcManager *manager, int size);
void *RSRC_alloc(RsrcManager *manager, u32 size, int key);
void *RSRC_searchFile(RsrcManager *manager, char *name);
void RSRC_dispose(RsrcManager *manager, int mode);
RsrcItem *RSRC_getItemID(RsrcManager *manager, int id);
RsrcItem *RSRC_getItem2(RsrcManager *manager, u32 key);
RsrcItem *RSRC_getItem(RsrcManager *manager, void *data);
void *RSRC_loadFileSub(RsrcManager *manager, char *path, char *file_name);
void *RSRC_loadFile2(RsrcManager *manager, char *path, char *file_name);
void *RSRC_loadFile(RsrcManager *manager, char *file_name);
RsrcItem *RSRC_getDirtyItem(RsrcManager *manager, u32 size);

#endif /* INCLUDE_MAIN_RSRC_H */
