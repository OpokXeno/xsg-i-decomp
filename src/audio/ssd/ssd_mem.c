#include "ssd/ssd_mem.h"
#include "ssd/ssd_init.h"

void SsdMemoryClear(void *memory, int size);
int SsdGetSpuMemManagerBlock(void);
SsdSpuMemoryBlock *SsdGetSpuAddsBlock(int address);
int SsdSpuMalloc2(int size, unsigned short type);
void SsdDisposeMemoryMap(void *map);
void SsdSetSpuType(unsigned int address, int type);

/* Sets up the main memory arena from address for size bytes and writes its head block. */
void SsdInitMainMemoryManager(unsigned int address, int size)
{
    SsdMainMemoryBlock *block;
    size &= 0x7ffffffc;
    if (address & 3) {
        size -= 4;
        address = (address + 3) & ~3;
    }
    block = (SsdMainMemoryBlock *)address;
    SsdWork.main_memory.first = block;
    SsdWork.main_memory.end = (unsigned char *)address + size;
    SsdWork.main_memory.size = size;
    block->flags = SSD_MEM_BLOCK_ARENA;
    block->type = 0;
    block->cleared_word = 0;
    block->end = (unsigned char *)(block + 1);
    block->next = 0;
}

/* Allocates size bytes from the first gap that is large enough and zeroes them. Returns the payload, or 0. */
void *SsdNewPtr(int size)
{
    SsdMainMemoryManager *manager = &SsdWork.main_memory;
    int required = ((size + 3) & ~3) + sizeof(SsdMainMemoryBlock);
    SsdMainMemoryBlock *block = manager->first;
    SsdMainMemoryBlock *next;
    SsdMainMemoryBlock *header;
    unsigned char *payload;
    if (block->next == 0) {
        next = (SsdMainMemoryBlock *)manager->end;
    } else {
        do {
            next = block->next;
            if ((unsigned char *)next - block->end >= required)
                goto allocate;
            block = next;
        } while (block->next != 0);
        next = (SsdMainMemoryBlock *)SsdWork.main_memory.end;
    }
    if ((unsigned char *)next - block->end >= required) {
allocate:
    header = (SsdMainMemoryBlock *)(((unsigned int)block->end + 3) & ~3);
    payload = (unsigned char *)(header + 1);
    header->end = payload + size;
    header->next = 0;
    header->cleared_word = 0;
    header->flags = SSD_MEM_BLOCK_ALLOCATED;
    header->type = 0;
    header->next = block->next;
    block->next = header;
    SsdMemoryClear(payload, size);
    return payload;
    }
    return 0;
}

/* Allocates size bytes at the upper end of the last gap that is large enough and zeroes them. Returns the payload, or 0. */
void *SsdNewPtr2(int size)
{
    int required = ((size + 3) & ~3) + sizeof(SsdMainMemoryBlock);
    SsdMainMemoryManager *manager = &SsdWork.main_memory;
    SsdMainMemoryBlock *block = manager->first;
    SsdMainMemoryBlock *selected = 0;
    unsigned char *gap_end = (unsigned char *)selected;
    unsigned char **arena_end = &manager->end;
    SsdMainMemoryBlock *header;
    unsigned char *payload;

    for (;;) {
        SsdMainMemoryBlock *next = block->next;
        if (next == 0) {
            next = (SsdMainMemoryBlock *)*arena_end;
            if ((unsigned char *)next - block->end >= required) {
                selected = block;
                gap_end = (unsigned char *)next;
            }
            break;
        }
        if ((unsigned char *)next - block->end >= required) {
            selected = block;
            gap_end = (unsigned char *)next;
        }
        block = next;
    }
    if (selected == 0) {
        return 0;
    }
    gap_end -= required;
    header = (SsdMainMemoryBlock *)(((unsigned int)gap_end + 3) & ~3);
    payload = (unsigned char *)(header + 1);
    header->end = payload + size;
    header->next = 0;
    header->cleared_word = 0;
    header->flags = SSD_MEM_BLOCK_ALLOCATED;
    header->type = 0;
    header->next = selected->next;
    selected->next = header;
    SsdMemoryClear(payload, size);
    return payload;
}

/* Unlinks the block of memory from the main memory list. Returns the previous block's new next pointer, or the first block when the block has no previous one. */
SsdMainMemoryBlock *SsdDisposePtr(void *memory)
{
    SsdMainMemoryBlock *const first = SsdWork.main_memory.first;
    SsdMainMemoryBlock *previous = 0;
    SsdMainMemoryBlock *target = (SsdMainMemoryBlock *)memory - 1;
    SsdMainMemoryBlock *block = first;
    if (first != target) {
        do {
            previous = block;
            block = previous->next;
        } while (block != target);
    }
    return previous != 0 ? (previous->next = target->next) : first;
}

/* Returns the largest free gap between main memory blocks, in bytes, rounded down to a multiple of four. */
int SsdGetMemoryFreeSize(void)
{
    SsdMainMemoryBlock *block = SsdWork.main_memory.first;
    int largest = 0;
    int gap;
    while (block->next) {
        SsdMainMemoryBlock *next = block->next;
        gap = (unsigned char *)next - block->end;
        if (largest < gap) {
            largest = gap;
        }
        block = next;
    }
    gap = SsdWork.main_memory.end - block->end;
    if (largest < gap) {
        largest = gap;
    }
    return largest & ~3;
}

int SsdGetMemoryBlockCount(void)
{
    SsdMainMemoryBlock *block = SsdWork.main_memory.first;
    int count = 0;
    while (block) {
        ++count;
        block = block->next;
    }
    return count;
}

/* Copies size bytes, 16 bytes at a time, then the remaining words and bytes. */
void SsdMemoryCopy(void *destination_memory, const void *source_memory, int size)
{
    unsigned int *destination = destination_memory;
    const unsigned int *source = source_memory;
    int count = size >> 4;
    if (count) {
        do {
            unsigned int first = source[0];
            unsigned int second = source[1];
            unsigned int third = source[2];
            unsigned int fourth = source[3];
            destination[0] = first;
            destination[1] = second;
            destination[2] = third;
            destination[3] = fourth;
            source += 4;
            destination += 4;
        } while (--count);
    }
    count = (size >> 2) & 3;
    if (count) {
        do {
            *destination++ = *source++;
        } while (--count);
    }
    count = size & 3;
    if (count) {
        const unsigned char *source_bytes = (const unsigned char *)source;
        unsigned char *destination_bytes = (unsigned char *)destination;
        do {
            *destination_bytes++ = *source_bytes++;
        } while (--count);
    }
}

/* Zeroes size bytes, 16 bytes at a time, then the remaining words and bytes. */
void SsdMemoryClear(void *memory, int size)
{
    unsigned int *destination = memory;
    int count = size >> 4;
    int zero = 0;
    if (count) {
        do {
            destination[3] = zero;
            destination[2] = zero;
            destination[1] = zero;
            destination[0] = zero;
            destination += 4;
        } while (--count);
    }
    count = (size >> 2) & 3;
    if (count) {
        do {
            *destination++ = zero;
        } while (--count);
    }
    count = size & 3;
    if (count) {
        unsigned char *bytes = (unsigned char *)destination;
        do {
            *bytes++ = zero;
        } while (--count);
    }
}

/* Clears the SPU block table and the segment modes, and reserves the system area at address 0. */
void SsdInitSpuMemoryManager(void)
{
    int remaining;
    SsdSpuMemoryBlock *sentinel;
    SsdWork.spu_memory.limit = SSD_SPU_RAM_SIZE;
    for (remaining = 0; remaining < SSD_SPU_BLOCK_COUNT; remaining++) {
        SsdWork.spu_memory.blocks[remaining].flags = 0;
    }
    for (remaining = 0; remaining < SSD_SEGMENT_MODE_COUNT; remaining++) {
        SsdWork.segment_modes[remaining].address = 0;
    }
    sentinel = SsdWork.spu_memory.blocks;
    sentinel->flags = SSD_SPU_BLOCK_USED | SSD_SPU_BLOCK_FIXED;
    sentinel->type = SSD_SPU_BLOCK_SYSTEM;
    sentinel->address = 0;
    sentinel->size = SSD_SPU_SYSTEM_SIZE;
    sentinel->next_index = 0;
}

void SsdSetSegmentAllocMode(unsigned int address, int size)
{
    int index = 0;
    for (; index < SSD_SEGMENT_MODE_COUNT; ++index) {
        if (!SsdWork.segment_modes[index].address) {
            SsdWork.segment_modes[index].address = address;
            SsdWork.segment_modes[index].size = size;
            return;
        }
    }
}

void SsdResetSegmentAllocMode(unsigned int address)
{
    int index = 0;
    SsdSegmentAllocMode *mode = SsdWork.segment_modes;
    for (; index < SSD_SEGMENT_MODE_COUNT; ++index, ++mode) {
        if (mode->address == address) {
            mode->address = 0;
            return;
        }
    }
}

/* Allocates size bytes of SPU RAM at the first gap that is large enough. Returns the address, or 0. */
int SsdSpuMalloc(int size, int type)
{
    SsdSpuMemoryBlock *block = SsdWork.spu_memory.blocks;
    int address = block->address + block->size;
    int allocation_size = size;
    int allocation_type = type;
    int index;
    int gap;
    int next_address;
    SsdSpuMemoryBlock *candidate;

    for (;;) {
        if (!block->next_index) {
            gap = SsdWork.spu_memory.limit - address;
            break;
        }
        candidate = &SsdWork.spu_memory.blocks[block->next_index];
        next_address = candidate->address;
        gap = next_address - address;
        if (gap >= allocation_size)
            break;
        block = candidate;
        address = next_address + block->size;
    }
    if (gap >= allocation_size) {
        index = SsdGetSpuMemManagerBlock();
        if (index >= 0) {
            candidate = &SsdWork.spu_memory.blocks[index];
            candidate->flags = SSD_SPU_BLOCK_USED;
            candidate->type = allocation_type;
            candidate->address = address;
            candidate->size = allocation_size;
            candidate->next_index = block->next_index;
            block->next_index = index;
            return address;
        }
    }
    return 0;
}

/* Allocates size bytes of SPU RAM at the top of the last gap that is large enough. Returns the address, or 0. */
int SsdSpuMalloc2(int size, unsigned short type)
{
    SsdSpuMemoryBlock *block = SsdWork.spu_memory.blocks;
    SsdSpuMemoryBlock *selected = 0;
    int address = 0;
    int *limit = &SsdWork.spu_memory.limit;
    SsdSpuMemoryBlock *candidate;
    int index;
    for (;;) {
        int end = block->address + block->size;
        int upper;
        if (!block->next_index) {
            upper = *limit;
            if (upper - end >= size) {
                selected = block;
                address = upper - size;
            }
            break;
        }
        candidate = &SsdWork.spu_memory.blocks[block->next_index];
        upper = candidate->address;
        if (upper - end >= size) {
            selected = block;
            address = upper - size;
        }
        block = candidate;
    }
    if (selected) {
        index = SsdGetSpuMemManagerBlock();
        if (index >= 0) {
            candidate = &SsdWork.spu_memory.blocks[index];
            candidate->flags = SSD_SPU_BLOCK_USED;
            candidate->type = type;
            candidate->address = address;
            candidate->size = size;
            candidate->next_index = selected->next_index;
            selected->next_index = index;
            return address;
        }
    }
    return 0;
}

/* Allocates size bytes of SPU RAM at the fixed address when that range is free. Returns the address, or 0. */
int SsdSpuMallocAdrs(int size, int address, unsigned short type)
{
    SsdSpuMemoryBlock *block = SsdWork.spu_memory.blocks;
    int gap = 0;
    unsigned short allocation_type = type;
    int end = block->address + block->size;
    int requested_end = address + size;
    SsdSpuMemoryBlock *candidate;
    int next_address;
    int index;

    while (block->address < address) {
        if (!block->next_index) {
            gap = SsdWork.spu_memory.limit - end;
            break;
        }
        candidate = &SsdWork.spu_memory.blocks[block->next_index];
        next_address = candidate->address;
        if (next_address >= requested_end) {
            gap = next_address - end;
            break;
        }
        end = next_address + candidate->size;
        block = candidate;
    }
    if (gap >= size) {
        if (address < end) {
        return 0;
        } else {
        index = SsdGetSpuMemManagerBlock();
        if (index >= 0) {
            candidate = &SsdWork.spu_memory.blocks[index];
            candidate->flags = SSD_SPU_BLOCK_USED;
            candidate->type = allocation_type;
            candidate->address = address;
            candidate->size = size;
            candidate->next_index = block->next_index;
            block->next_index = index;
            return address;
        } else {
            return 0;
        }
        }
    } else {
        return 0;
    }
}

/* Allocates size bytes of SPU RAM at the first gap at or above minimum_address. Returns the address, or 0. */
int SsdSpuMallocAdrsDynamic(int size, int minimum_address, int type)
{
    SsdSpuMemoryBlock *block = SsdWork.spu_memory.blocks;
    int *limit = &SsdWork.spu_memory.limit;
    int address;
    int gap;
    int index;
    SsdSpuMemoryBlock *next;
    for (;;) {
        address = block->address + block->size;
        if (!block->next_index) {
            if (address < minimum_address) {
                address = minimum_address;
            }
            gap = *limit - address;
            break;
        }
        next = &SsdWork.spu_memory.blocks[block->next_index];
        if (address < minimum_address) {
            address = minimum_address;
        }
        if (next->address - address >= size) {
            /* Goto the shared descriptor insertion below instead of repeating it. */
            goto reserve_descriptor;
        }
        block = next;
    }
    if (gap >= size) {
reserve_descriptor:
    index = SsdGetSpuMemManagerBlock();
    if (index >= 0) {
        next = &SsdWork.spu_memory.blocks[index];
        next->flags = SSD_SPU_BLOCK_USED;
        next->type = type;
        next->address = address;
        next->size = size;
        next->next_index = block->next_index;
        block->next_index = index;
        return address;
    }
    }
    return 0;
}

/*
 * Like SsdSpuMallocAdrsDynamic, but minimum_address first advances in steps of
 * segment_size past the end of the current block.
 */
int SsdSpuMallocAdrsSegment(int size, int minimum_address, int segment_size, int type)
{
    SsdSpuMemoryBlock *block = SsdWork.spu_memory.blocks;
    int *limit = &SsdWork.spu_memory.limit;
    int address;
    int gap;
    int index;
    SsdSpuMemoryBlock *next;
    for (;;) {
        address = block->address + block->size;
        while (address >= minimum_address) {
            minimum_address += segment_size;
        }
        if (!block->next_index) {
            if (address < minimum_address) {
                address = minimum_address;
            }
            gap = *limit - address;
            break;
        }
        next = &SsdWork.spu_memory.blocks[block->next_index];
        if (address < minimum_address) {
            address = minimum_address;
        }
        if (next->address - address >= size) {
            /* Goto the shared descriptor insertion below instead of repeating it. */
            goto reserve_descriptor;
        }
        block = next;
    }
    if (gap >= size) {
reserve_descriptor:
    index = SsdGetSpuMemManagerBlock();
    if (index >= 0) {
        next = &SsdWork.spu_memory.blocks[index];
        next->flags = SSD_SPU_BLOCK_USED;
        next->type = type;
        next->address = address;
        next->size = size;
        next->next_index = block->next_index;
        block->next_index = index;
        return address;
    }
    }
    return 0;
}

/* Frees the SPU block that starts at address. Returns address, or 0 when no block starts there. */
unsigned int SsdSpuMFree(unsigned int address)
{
    SsdSpuMemoryBlock *block = SsdWork.spu_memory.blocks;
    SsdSpuMemoryBlock *previous = 0;
    for (;;) {
        int next;
        if (block->address == address) {
            previous->next_index = block->next_index;
            block->flags = 0;
            block->type = 0;
            block->address = 0;
            block->next_index = 0;
            return address;
        }
        next = block->next_index;
        if (!next) {
            return 0;
        }
        previous = block;
        block = &SsdWork.spu_memory.blocks[next];
    }
}

void SsdSetSpuType(unsigned int address, int type)
{
    SsdSpuMemoryBlock *block = SsdGetSpuAddsBlock(address);
    if (block) {
        block->type = type;
    }
}

/* Returns the largest free gap in SPU RAM, in bytes. */
int SsdGetSpuMemoryFree(void)
{
    SsdSpuMemoryBlock *block = SsdWork.spu_memory.blocks;
    int largest = 0;
    int end;
    int gap;
    for (;;) {
        int next;
        end = block->address + block->size;
        next = block->next_index;
        if (!next) {
            break;
        }
        block = &SsdWork.spu_memory.blocks[next];
        gap = block->address - end;
        if (largest < gap) {
            largest = gap;
        }
    }
    gap = SsdWork.spu_memory.limit - end;
    if (largest < gap) {
        largest = gap;
    }
    return largest;
}

/* Returns the index of the first unused SPU block descriptor, or -1. */
int SsdGetSpuMemManagerBlock(void)
{
    SsdSpuMemoryBlock *block = SsdWork.spu_memory.blocks;
    int index = 0;
    for (; index < SSD_SPU_BLOCK_COUNT; ++index, ++block) {
        if (!block->flags) {
            return index;
        }
    }
    return -1;
}

/* Returns the SPU block that starts at address, or 0. */
SsdSpuMemoryBlock *SsdGetSpuAddsBlock(int address)
{
    SsdSpuMemoryBlock *block = SsdWork.spu_memory.blocks;
    for (;;) {
        int next;
        if (block->address == address) {
            return block;
        }
        next = block->next_index;
        if (next) {
            return 0;
        }
        block = &SsdWork.spu_memory.blocks[next];
    }
}

void SsdDisposeMemoryMap(void *map)
{
    SsdDisposePtr(map);
}
