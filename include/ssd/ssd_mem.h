#ifndef SSD_MEM_H
#define SSD_MEM_H

#include "ssd/ssd.h"

/* SPU2 sound RAM: 2 MB, of which the first 0x5040 bytes belong to the hardware and driver. */
#define SSD_SPU_RAM_SIZE 0x200000
#define SSD_SPU_SYSTEM_SIZE 20544
#define SSD_SPU_STREAM_BASE 0x6000
#define SSD_SPU_STREAM_SIZE 8192

/* SsdSpuMemoryBlock.type */
#define SSD_SPU_BLOCK_SYSTEM 1
#define SSD_SPU_BLOCK_DRIVER 2
#define SSD_SPU_BLOCK_STREAM 3
#define SSD_SPU_BLOCK_REVERB_CORE0 4
#define SSD_SPU_BLOCK_REVERB_CORE1 5
#define SSD_SPU_BLOCK_WAVE 9
#define SSD_SPU_BLOCK_SAMPLING 10
#define SSD_SPU_BLOCK_TYPE_COUNT 13

/* SsdSpuMemoryBlock.flags */
#define SSD_SPU_BLOCK_USED 0x80
#define SSD_SPU_BLOCK_FIXED 0x01

/* SsdMainMemoryBlock.flags */
#define SSD_MEM_BLOCK_ALLOCATED 0x2
#define SSD_MEM_BLOCK_ARENA 0x8000

#define SSD_SPU_BLOCK_COUNT 16
#define SSD_SEGMENT_MODE_COUNT 4

typedef struct SsdMainMemoryBlock SsdMainMemoryBlock;

struct SsdMainMemoryBlock {
    unsigned short flags;
    unsigned short type;
    unsigned int cleared_word;
    unsigned char *end;
    SsdMainMemoryBlock *next;
};

typedef struct SsdMainMemoryManager {
    SsdMainMemoryBlock *first;
    unsigned char *end;
    int size;
} SsdMainMemoryManager;

typedef struct SsdSpuMemoryBlock {
    unsigned char next_index;
    unsigned char flags;
    unsigned short type;
    int address;
    int size;
    unsigned char unmodeled_0c[4];
} SsdSpuMemoryBlock;

typedef struct SsdSpuMemoryManager {
    int limit;
    unsigned char unmodeled_04[4];
    SsdSpuMemoryBlock blocks[SSD_SPU_BLOCK_COUNT];
} SsdSpuMemoryManager;

typedef struct SsdSegmentAllocMode {
    unsigned int address;
    int size;
} SsdSegmentAllocMode;

/* Functions and data used by other translation units. */
void SsdInitMainMemoryManager(unsigned int address, int size);
void *SsdNewPtr(int size);
void *SsdNewPtr2(int size);
SsdMainMemoryBlock *SsdDisposePtr(void *memory);
int SsdGetMemoryFreeSize(void);
int SsdGetMemoryBlockCount(void);
void SsdSetSegmentAllocMode(unsigned int address, int size);
void SsdResetSegmentAllocMode(unsigned int address);
void SsdMemoryCopy(void *destination_memory, const void *source_memory, int size);
void SsdInitSpuMemoryManager(void);
int SsdSpuMalloc(int size, int type);
int SsdSpuMallocAdrs(int size, int address, unsigned short type);
int SsdSpuMallocAdrsDynamic(int size, int minimum_address, int type);
int SsdSpuMallocAdrsSegment(int size, int minimum_address, int segment_size, int type);
unsigned int SsdSpuMFree(unsigned int address);
int SsdGetSpuMemoryFree(void);

#endif
