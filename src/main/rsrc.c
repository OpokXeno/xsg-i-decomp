#include "common.h"

#include "shared.h"

/* These cursors begin at the sentinel used before a resource table is found. */

static int infoIndex = -1;

static int infoLength = -1;

extern char D_004C23C0[];

static char *rsrcDefaultPath = D_004C23C0;

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
    u32 size;
    u8 kind;
    u8 item_id;
    u16 flags;
    u8 *data;
    u32 key : 32;
} RsrcItem;

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

extern void *RSRC_loadFileSub(RsrcManager *manager, char *path, char *file_name);

/* These cursors begin at the sentinel used before a resource table is found. */

extern char *markedSTR[2][2];

extern char *selectedSTR[2][2];

extern char *unselectedSTR[2][2];

extern char *normalSTR[2][2];

extern u32 strlen(const char *s);

extern int strncmp(const char *a, const char *b, u32 n);

extern char *strcat(char *destination, const char *source);

extern void *memcpy(void *destination, const void *source, u32 size);

extern char *DB_pathGetShortPath(char *destination, const char *path, int size);

extern RsrcItem *RSRC_getDirtyItem(RsrcManager *manager, u32 size);

extern char *strcpy(char *destination, const char *source);

extern int sceOpen(const char *path, int flags, ...);

extern int sceLseek(int fd, int offset, int whence);

extern int sceRead(int fd, void *buffer, int size);

extern int sceClose(int fd);

extern void *RSRC_searchFile(RsrcManager *manager, char *name);

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

int RSRC_info(RsrcManager *manager, int mode)
{
    char line[128];
    char marks[128];
    char path[128];
    int show_path = mode & 0xf;
    int first = infoIndex;
    int length = infoLength;
    int last;
    int index;
    RsrcItem *item;

    if (first < 0) {
        first = 0;
    }
    if (length < 0) {
        last = manager->item_count;
    } else {
        last = first + length;
        if (manager->item_count < last) {
            last = manager->item_count;
        }
    }
    if (first < 0 || first >= manager->item_count) {
        first = 0;
    }
    index = first;
    while (index < last) {
        marks[0] = 0;
        line[0] = 0;
        item = (RsrcItem *)manager->items + index;
        index++;
        if (item->flags & 2) {
            strcat(line, selectedSTR[mode][0]);
            strcat(marks, selectedSTR[mode][1]);
        } else {
            strcat(line, unselectedSTR[mode][0]);
            strcat(marks, unselectedSTR[mode][1]);
        }
        if (item->flags & 1) {
            strcat(line, markedSTR[mode][0]);
            strcat(marks, markedSTR[mode][1]);
        } else {
            strcat(line, normalSTR[mode][0]);
            strcat(marks, normalSTR[mode][1]);
        }
        if (item->kind == 1 && show_path != 0) {
            DB_pathGetShortPath(path, (char *)item->key, 22);
        }
    }
    infoIndex = -1;
    infoLength = -1;
    return 0;
}

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

void *RSRC_alloc(RsrcManager *manager, u32 size, int key)
{
    RsrcItem *item = RSRC_getDirtyItem(manager, size);

    if (item == 0) {
        u32 aligned;

        if (manager->item_count >= manager->item_capacity) {
            item = RSRC_getDirtyItem(manager, 0);
            if (item == 0) {
                RSRC_info(manager, 0);
            }
        } else {
            item = (RsrcItem *)manager->items + manager->item_count++;
        }
        if (RSRC_check(manager, size)) {
            RSRC_info(manager, 0);
        }
        aligned = (size + 15) >> 4 << 4;
        item->data = manager->heap_cursor;
        item->size = aligned;
        manager->heap_cursor += aligned;
    }
    item->key = key;
    item->flags = 0;
    item->kind = 2;
    memset(item->data, 0, item->size);
    return item->data;
}

void *RSRC_searchFile(RsrcManager *manager, char *name)
{
    int index = 0;
    u16 count = manager->item_count;
    int length = strlen(name);
    RsrcItem *item = manager->items;

    if (count != 0) {
        do {
            if (item->kind == 1 && strncmp((char *)item->key, name, length) == 0) {
                return item->data;
            }
            index++;
            item++;
        } while (index < count);
    }
    return 0;
}

static int isLeave(RsrcItem *item, int mode)
{
    return item->flags & 2;
}

void RSRC_dispose(RsrcManager *manager, int mode)
{
    RsrcItem *item = manager->items;
    u16 count = manager->item_count;
    u8 *dst = manager->heap_origin;
    int index;
    int live;

    for (index = 0; index < count; index++, item++) {
        if (isLeave(item, mode)) {
            if (dst != item->data) {
                memcpy(dst, item->data, item->size);
                if (item->kind == 1) {
                    u32 offset = item->key - (u32)item->data;

                    item->data = dst;
                    item->key = (u32)dst + offset;
                }
            }
            dst += item->size;
        } else {
            item->data = 0;
        }
    }
    manager->heap_cursor = dst;

    live = 0;
    item = manager->items;
    for (index = 0; index < count; index++, item++) {
        if (item->data != 0) {
            RsrcItem *slot = manager->items;
            int slot_index = 0;

            for (; slot_index < count; slot_index++, slot++) {
                if (slot->data == 0) {
                    break;
                }
            }
            if (slot_index < index) {
                *slot = *item;
                memset(item, 0, sizeof(RsrcItem));
                slot->flags = 0;
            }
            item->flags = 0;
            live++;
        } else {
            item->flags = 0;
        }
    }
    manager->item_count = live;
}

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

RsrcItem *RSRC_getItem(RsrcManager *manager, void *data)
{
    int index;
    u16 count = manager->item_count;
    RsrcItem *item = manager->items;

    if (data == 0)
        return 0;

    for (index = 0; index < count; index++) {
        if (item->data == data)
            return item;
        item++;
    }
    return 0;
}

void *RSRC_loadFileSub(RsrcManager *manager, char *path, char *file_name)
{
    char buffer[1024];
    u8 *data;
    void *found;
    int fd;
    int size;
    int blocks;
    int name_length;
    int name_blocks;
    RsrcItem *item;

    found = RSRC_searchFile(manager, file_name);
    if (found != 0) {
        return found;
    }

    strcpy(buffer, path);
    strcat(buffer, file_name);
    data = manager->heap_cursor;
    fd = sceOpen(buffer, 1);
    if (fd < 0) {
        return 0;
    }
    size = sceLseek(fd, 0, 2);
    if (RSRC_check(manager, size)) {
        return 0;
    }
    sceLseek(fd, 0, 0);
    if (size != 0) {
        sceRead(fd, data, size);
    }
    sceClose(fd);

    blocks = (size + 15) / 16;
    manager->heap_cursor += blocks * 16;
    name_length = strlen(file_name);
    strcpy(manager->heap_cursor, file_name);
    item = (RsrcItem *)manager->items + manager->item_count;
    name_blocks = (name_length + 16) / 16;
    item->kind = 1;
    item->data = data;
    item->flags = 0;
    item->key = (u32)manager->heap_cursor;
    item->size = (blocks + name_blocks) * 16;
    manager->heap_cursor += name_blocks * 16;
    manager->item_count++;
    return data;
}

void *RSRC_loadFile2(RsrcManager *manager, char *path, char *file_name)
{
    return RSRC_loadFileSub(manager, path, file_name);
}

void *RSRC_loadFile(RsrcManager *manager, char *file_name)
{
    return RSRC_loadFile2(manager, rsrcDefaultPath, file_name);
}

RsrcItem *RSRC_getDirtyItem(RsrcManager *manager, u32 size)
{
    int index;
    u16 count = manager->item_count;
    RsrcItem *item = manager->items;

    if (size == 0) {
        RsrcItem *best = 0;
        u32 best_size = -1;

        if (count != 0) {
            index = count;
            do {
                if ((item->flags & 0x8000) && item->size <= best_size) {
                    best_size = item->size;
                    best = item;
                }
                index--;
                item++;
            } while (index != 0);
        }
        return best;
    }
    for (index = 0; index < count; index++) {
        if ((item->flags & 0x8000) && item->size >= size) {
            return item;
        }
        item++;
    }
    return 0;
}
