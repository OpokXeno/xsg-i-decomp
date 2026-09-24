#include "common.h"
#include "shared.h"

/* These globals remain provided by this TU's assembly-owned small-data area. */
extern int infoIndex;
extern int infoLength;
extern char *rsrcDefaultPath;

typedef struct RsrcManager {
    u8 *heap_origin;
    u8 *heap_cursor;
    u32 heap_bound;
    void *items;
    u16 item_count;
    u16 item_capacity;
    u8 unmodeled_14[12];
} RsrcManager;

typedef struct RsrcItem {
    u8 unmodeled_00[5];
    u8 item_id;
    u16 flags;
    u32 size;
    u32 key;
} RsrcItem;

void RSRC_inactiveSource(RsrcManager *manager, int source)
{
    u16 count = manager->item_count;
    int index = 0;
    RsrcItem *item = manager->items;

    if (count != 0) {
        do {
            if (item->key == source) {
                item->key = 0;
                item->flags |= 0x8000;
            }
            index++;
            item++;
        } while (index < manager->item_count);
    }
}

/*
 * The arena is one contiguous block carved out of the caller's memory:
 * general heap allocations grow upward from `memory`, the fixed-size
 * item table (item_capacity RsrcItem slots) sits directly below the
 * manager, and the manager struct itself occupies the last
 * sizeof(RsrcManager) bytes of the block. `items` is therefore reached
 * by walking back item_capacity RsrcItem slots from `manager`, and
 * `heap_bound` is the byte offset where the item table begins, i.e. the
 * heap's usable limit.
 */
RsrcManager *RSRC_create(u8 *memory, u32 memory_size, int item_capacity)
{
    RsrcManager *manager;

    memset(memory, 0, memory_size);
    manager = (RsrcManager *)(memory + memory_size - sizeof(RsrcManager));
    manager->heap_cursor = manager->heap_origin = memory;
    manager->heap_bound = memory_size - item_capacity * sizeof(RsrcItem) - sizeof(RsrcManager);
    manager->items = (RsrcItem *)manager - item_capacity;
    manager->item_capacity = item_capacity;
    manager->item_count = 0;
    return manager;
}

void RSRC_setPrintParam(int index, int length)
{
    infoIndex = index;
    infoLength = length;
}

INCLUDE_ASM("asm/main/nonmatchings/rsrc", RSRC_info);

char *RSRC_getDefaultPath(void)
{
    return rsrcDefaultPath;
}

char *RSRC_setDefaultPath(char *path)
{
    rsrcDefaultPath = path;
    return path;
}

void RSRC_init(RsrcManager *manager)
{
    u16 *item_flags;
    int index = 0;

    manager->heap_cursor = manager->heap_origin;
    manager->item_count = 0;

    if (manager->item_capacity != 0) {
        item_flags = &((RsrcItem *)manager->items)->flags;
        do {
            *item_flags = 0;
            index++;
            item_flags += sizeof(RsrcItem) / sizeof(*item_flags);
        } while (index < manager->item_capacity);
    }
}

int RSRC_check(RsrcManager *manager, int size)
{
    if ((u32)((manager->heap_cursor + size) - manager->heap_origin) < manager->heap_bound) {
        return 0;
    }
    return 1;
}

INCLUDE_ASM("asm/main/nonmatchings/rsrc", RSRC_alloc);

INCLUDE_ASM("asm/main/nonmatchings/rsrc", RSRC_searchFile);

static int isLeave(RsrcItem *item)
{
    return item->flags & 2;
}

INCLUDE_ASM("asm/main/nonmatchings/rsrc", RSRC_dispose);

RsrcItem *RSRC_getItemID(RsrcManager *manager, int id)
{
    u16 count = manager->item_count;
    RsrcItem *item = manager->items;
    int index = 0;

    if (count != 0) {
        do {
            if (item->item_id == id)
                return item;
            index++;
            item++;
        } while (index < count);
    }
    return 0;
}

RsrcItem *RSRC_getItem2(RsrcManager *manager, u32 key)
{
    u16 count = manager->item_count;
    RsrcItem *item = manager->items;
    int index = 0;

    if (count != 0) {
        do {
            if (item->key == key)
                return item;
            index++;
            item++;
        } while (index < count);
    }
    return 0;
}

INCLUDE_ASM("asm/main/nonmatchings/rsrc", RSRC_getItem);

extern void *RSRC_loadFileSub(RsrcManager *manager, char *path, char *file_name);

INCLUDE_ASM("asm/main/nonmatchings/rsrc", RSRC_loadFileSub);

void *RSRC_loadFile2(RsrcManager *manager, char *path, char *file_name)
{
    return RSRC_loadFileSub(manager, path, file_name);
}

void *RSRC_loadFile(RsrcManager *manager, char *file_name)
{
    return RSRC_loadFile2(manager, rsrcDefaultPath, file_name);
}

INCLUDE_ASM("asm/main/nonmatchings/rsrc", RSRC_getDirtyItem);
