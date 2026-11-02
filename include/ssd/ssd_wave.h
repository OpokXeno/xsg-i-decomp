#ifndef SSD_WAVE_H
#define SSD_WAVE_H

#include "ssd/ssd.h"

typedef void (*SsdWaveTransferCallback)(int result, void *argument);

/* Wave bank file and loaded bank signatures ("swdm", "swav" in file byte order). */
#define SSD_WAVE_FILE_SIGNATURE 0x6d647773
#define SSD_WAVE_BANK_SIGNATURE 0x76617773

/* Chunk types of the wave bank file. */
#define SSD_WAVE_CHUNK_END 0
#define SSD_WAVE_CHUNK_SAMPLES 3
#define SSD_WAVE_CHUNK_PROGRAMS 4

/* Wave bank file flags */
#define SSD_WAVE_FLAG_DYNAMIC_ADDRESS 0x1

/* SPU address argument of SsdAddWaveData: 0 = the address stored in the file. */
#define SSD_WAVE_ADDRESS_ANY (~0U)

/* Result passed to the wave transfer callback when the transfer fails. */
#define SSD_WAVE_RESULT_FAILED (-2)

typedef struct SsdWaveSample {
    unsigned int body_offset;
    unsigned int loop_offset;
    unsigned char volume;
    unsigned char pan;
    short base_pitch;
    unsigned short adsr1;
    unsigned short adsr2;
    char name[16];
} SsdWaveSample;

/* Loaded wave bank (header of a "wds " file followed by its sample table). */
struct SsdWaveBank {
    unsigned int signature;
    unsigned short bank_id;
    unsigned char sample_count;
    unsigned char program_count;
    unsigned char unmodeled_08[4];
    unsigned int spu_address;
    unsigned int body_size;
    unsigned char unmodeled_14[4];
    SsdWaveSample *samples;
    void *programs;
    unsigned char unmodeled_20[4];
    SsdWaveBank *next;
};

typedef struct SsdWaveTransferState {
    unsigned int next_spu_address;
    int bank_id;
    SsdWaveTransferCallback callback;
    void *argument;
} SsdWaveTransferState;

/* Functions and data used by other translation units. */
SsdWaveBank *SsdGetWSPtrSpuAdrs(unsigned int address);
SsdWaveBank *SsdGetWSPtrBank(int bank);
void SsdSetWaveTransCallback(SsdWaveTransferCallback callback, void *argument);
int SsdAddWaveData(void *data, int size, int spu_address);
int SsdDisposeWaveBank(unsigned short bank);
int SsdCheckWaveData(int bank);

#endif
