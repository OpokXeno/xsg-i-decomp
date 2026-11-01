/* Original ssd_mem.c translation unit. Shared data is defined by its owning FDR. */





typedef struct SsdMainMemoryBlock SsdMainMemoryBlock;
typedef struct SsdMainMemoryManager SsdMainMemoryManager;
typedef struct SsdMemoryWork SsdMemoryWork;
typedef struct SsdSpuMemoryBlock SsdSpuMemoryBlock;
typedef struct SsdSpuMemoryManager SsdSpuMemoryManager;
typedef struct SsdSegmentAllocMode SsdSegmentAllocMode;

struct SsdMainMemoryBlock {
    unsigned short flags, type;
    unsigned int cleared_word;
    unsigned char *end;
    struct SsdMainMemoryBlock *next;
};

struct SsdMainMemoryManager {
    SsdMainMemoryBlock *first;
    unsigned char *end;
    int size;
};

struct SsdSpuMemoryBlock {
    unsigned char next_index, flags;
    unsigned short type;
    int address;
    int size;
    unsigned char unmodeled_00c[4];
};

struct SsdSpuMemoryManager {
    int limit;
    unsigned char unmodeled_004[4];
    SsdSpuMemoryBlock blocks[16];
};

struct SsdSegmentAllocMode {
    unsigned int address;
    int size;
};

struct SsdMemoryWork {
    unsigned char unmodeled_000[16];
    SsdMainMemoryManager main_memory;
    SsdSpuMemoryManager spu_memory;
    SsdSegmentAllocMode segment_modes[4];
};

extern SsdMemoryWork SsdWork;
void SsdMemoryClear(void *memory, int size);
int SsdGetSpuMemManagerBlock(void);
SsdMainMemoryBlock *SsdDisposePtr(void *memory);
SsdSpuMemoryBlock *SsdGetSpuAddsBlock(int address);
void *SsdNewPtr2(int size);
int SsdSpuMalloc(int size, int type);
int SsdGetMemoryBlockCount(void);
void SsdInitSpuMemoryManager(void);
void SsdResetSegmentAllocMode(unsigned int address);
int SsdSpuMalloc2(int size, unsigned short type);
void SsdMemoryCopy(void *destination_memory, const void *source_memory, int size);
void SsdInitMainMemoryManager(unsigned int address, int size);
void SsdDisposeMemoryMap(void *map);
void SsdSetSpuType(unsigned int address, int type);
void *SsdNewPtr(int size);
int SsdGetMemoryFreeSize(void);
int SsdSpuMallocAdrs(int size, int address, unsigned short type);
int SsdSpuMallocAdrsSegment(int size, int minimum_address, int segment_size, int type);
void SsdSetSegmentAllocMode(unsigned int address, int size);
unsigned int SsdSpuMFree(unsigned int address);
int SsdGetSpuMemoryFree(void);
int SsdSpuMallocAdrsDynamic(int size, int minimum_address, int type);

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
    block->flags = 0x8000;
    block->type = 0;
    block->cleared_word = 0;
    block->end = (unsigned char *)(block + 1);
    block->next = 0;
}

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
    header->flags = 2;
    header->type = 0;
    header->next = block->next;
    block->next = header;
    SsdMemoryClear(payload, size);
    return payload;
    }
    return 0;
}

void *SsdNewPtr2(int size)
{
    int required = ((size + 3) & ~3) + 16;
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
    header->flags = 2;
    header->type = 0;
    header->next = selected->next;
    selected->next = header;
    SsdMemoryClear(payload, size);
    return payload;
}

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

void SsdInitSpuMemoryManager(void)
{
    int remaining;
    SsdSpuMemoryBlock *sentinel;
    SsdWork.spu_memory.limit = 0x200000;
    for (remaining = 0; remaining < 16; remaining++) {
        SsdWork.spu_memory.blocks[remaining].flags = 0;
    }
    for (remaining = 0; remaining < 4; remaining++) {
        SsdWork.segment_modes[remaining].address = 0;
    }
    sentinel = SsdWork.spu_memory.blocks;
    sentinel->flags = 0x81;
    sentinel->type = 1;
    sentinel->address = 0;
    sentinel->size = 20544;
    sentinel->next_index = 0;
}

void SsdSetSegmentAllocMode(unsigned int address, int size)
{
    int index = 0;
    for (; index < 4; ++index) {
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
    for (; index < 4; ++index, ++mode) {
        if (mode->address == address) {
            mode->address = 0;
            return;
        }
    }
}

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
            candidate->flags = 0x80;
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
            candidate->flags = 0x80;
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
            candidate->flags = 0x80;
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
            /* readable-goto: reserve_descriptor: common insertion; structured form01 duplicates the failure exit. */
            goto reserve_descriptor;
        }
        block = next;
    }
    if (gap >= size) {
reserve_descriptor:
    index = SsdGetSpuMemManagerBlock();
    if (index >= 0) {
        next = &SsdWork.spu_memory.blocks[index];
        next->flags = 0x80;
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
            /* readable-goto: reserve_descriptor: common first-fit insertion; structured form01 alters the failure exit. */
            goto reserve_descriptor;
        }
        block = next;
    }
    if (gap >= size) {
reserve_descriptor:
    index = SsdGetSpuMemManagerBlock();
    if (index >= 0) {
        next = &SsdWork.spu_memory.blocks[index];
        next->flags = 0x80;
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

int SsdGetSpuMemManagerBlock(void)
{
    SsdSpuMemoryBlock *block = SsdWork.spu_memory.blocks;
    int index = 0;
    for (; index < 16; ++index, ++block) {
        if (!block->flags) {
            return index;
        }
    }
    return -1;
}

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
