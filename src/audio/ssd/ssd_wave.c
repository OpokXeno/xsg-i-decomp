#include "ssd/ssd_wave.h"
#include "ssd/ssd_chan.h"
#include "ssd/ssd_dma.h"
#include "ssd/ssd_err.h"
#include "ssd/ssd_init.h"
#include "ssd/ssd_mem.h"
#include "ssd/ssd_voic.h"

typedef struct SsdWaveChunk {
    unsigned short type;
    unsigned short size;
    unsigned char unmodeled_04[12];
} SsdWaveChunk;

typedef struct SsdWaveFileHeader {
    unsigned int signature;
    unsigned char unmodeled_04[12];
    unsigned short flags;
    unsigned short bank_id;
    unsigned char sample_count;
    unsigned char program_count;
    unsigned char unmodeled_16[10];
    unsigned int spu_address;
    unsigned int body_size;
    unsigned int body_offset;
    unsigned char unmodeled_2c[20];
    SsdWaveChunk first_chunk;
} SsdWaveFileHeader;

int SsdNextWaveData(void *data, int size);
unsigned int SsdSpuWaveMalloc(const SsdWaveFileHeader *header, unsigned int address);
int SsdDisposeWaveData(SsdWaveBank *wave);
void SsdSetWaveToVoiceControl(SsdVoice *voice, SsdWaveBank *bank, short sample_index);

int SsdAddWaveData(void *data, int size, int spu_address)
{
    SsdWaveFileHeader *header;
    void *sample_data;
    void *program_data;
    int sample_size;
    int program_size;
    SsdWaveChunk *chunk;
    int scanning;
    int chunk_size;
    int transfer_size;
    SsdWaveBank *wave;
    char *metadata;
    SsdWaveBank **tail;

    SsdWork.wave_transfer.next_spu_address = 0;
    header = data;
    if (header->signature != SSD_WAVE_FILE_SIGNATURE) {
        SsdErrorProc(SSD_ERROR_BAD_SIGNATURE,(unsigned int)header);
        if (SsdWork.wave_transfer.callback != 0)
            SsdWork.wave_transfer.callback(SSD_WAVE_RESULT_FAILED, SsdWork.wave_transfer.argument);
        return -1;
    }
    if (SsdGetWSPtrBank(header->bank_id) != 0) {
        SsdWarringProc(SSD_WARNING_WAVE_BANK_LOADED,header->bank_id);
        if (SsdWork.wave_transfer.callback != 0)
            SsdWork.wave_transfer.callback(SSD_WAVE_RESULT_FAILED, SsdWork.wave_transfer.argument);
        return header->bank_id;
    }
    program_data = 0;
    sample_data = 0;
    program_size = 0;
    sample_size = 0;
    chunk = &header->first_chunk;
    scanning = 1;
    do {
        chunk_size = chunk->size;
        switch (chunk->type) {
        case SSD_WAVE_CHUNK_SAMPLES:
            sample_data = chunk + 1;
            sample_size = chunk_size - sizeof(SsdWaveChunk);
            break;
        case SSD_WAVE_CHUNK_PROGRAMS:
            program_data = chunk + 1;
            program_size = chunk_size - sizeof(SsdWaveChunk);
            break;
        case SSD_WAVE_CHUNK_END:
            scanning = 0;
            break;
        }
        /* Variable-size on-disk records advance by their declared byte length. */
        chunk = (SsdWaveChunk *)((char *)chunk + chunk_size);
    } while (scanning);

    spu_address = SsdSpuWaveMalloc(header, spu_address);
    if (spu_address == 0) {
        SsdErrorProc(SSD_ERROR_NO_SPU_MEMORY, header->bank_id);
        if (SsdWork.wave_transfer.callback != 0)
            SsdWork.wave_transfer.callback(SSD_WAVE_RESULT_FAILED, SsdWork.wave_transfer.argument);
        return SSD_ERROR_NO_SPU_MEMORY;
    }
    transfer_size = size - header->body_offset;
    SsdSpuDmaWrite(spu_address, chunk, transfer_size,
                   SsdWork.wave_transfer.callback, SsdWork.wave_transfer.argument);
    SsdWork.wave_transfer.next_spu_address = spu_address + transfer_size;
    wave = SsdNewPtr2(sample_size + program_size + sizeof(SsdWaveBank));
    if (wave == 0) {
        SsdSpuMFree(spu_address);
        SsdErrorProc(SSD_ERROR_NO_MEMORY, header->bank_id);
        return SSD_ERROR_NO_MEMORY;
    }
    wave->signature = SSD_WAVE_BANK_SIGNATURE;
    wave->bank_id = header->bank_id;
    wave->sample_count = header->sample_count;
    wave->program_count = header->program_count;
    wave->spu_address = spu_address;
    wave->body_size = header->body_size;
    metadata = (char *)(wave + 1);
    wave->samples = (SsdWaveSample *)metadata;
    SsdMemoryCopy(metadata, sample_data, sample_size);
    metadata += sample_size;
    wave->programs = metadata;
    SsdMemoryCopy(metadata, program_data, program_size);
    CpuDisableIntr();
    tail = &SsdWork.wave_banks;
    while (*tail != 0)
        tail = &(*tail)->next;
    *tail = wave;
    wave->next = 0;
    CpuEnableIntr();
    SsdWork.wave_transfer.bank_id = wave->bank_id;
    return wave->bank_id;
}

int SsdNextWaveData(void *data, int size)
{
    SsdWaveTransferState *state = &SsdWork.wave_transfer;
    unsigned int address = state->next_spu_address;
    if (address == 0) {
        SsdWarringProc(SSD_WARNING_NO_WAVE_TRANSFER, 0);
        if (state->callback != 0)
            state->callback(SSD_WAVE_RESULT_FAILED, state->argument);
        return SSD_WARNING_NO_WAVE_TRANSFER;
    }
    SsdSpuDmaWrite(address, data, size, state->callback, state->argument);
    state->next_spu_address = address + size;
    return state->bank_id;
}

unsigned int SsdSpuWaveMalloc(const SsdWaveFileHeader *header, unsigned int address)
{
    unsigned int size;
    unsigned int result;
    unsigned int segment;
    int i;
    if (address == 0)
        address = header->spu_address;
    else if (address == SSD_WAVE_ADDRESS_ANY)
        address = 0;
    size = header->body_size;
    if (address == 0) {
        result = SsdSpuMalloc(size, SSD_SPU_BLOCK_WAVE);
    } else {
        segment = 0;
        address = header->spu_address;
        for (i = 0; i < SSD_SEGMENT_MODE_COUNT; i++) {
            if (SsdWork.segment_modes[i].address == address) {
                segment = SsdWork.segment_modes[i].size;
                break;
            }
        }
        if (segment != 0) {
            result = SsdSpuMallocAdrsSegment(size, address, segment, SSD_SPU_BLOCK_WAVE);
        } else if (header->flags & SSD_WAVE_FLAG_DYNAMIC_ADDRESS) {
            result = SsdSpuMallocAdrsDynamic(size, address, SSD_SPU_BLOCK_WAVE);
        } else {
            result = SsdSpuMallocAdrs(size, address, SSD_SPU_BLOCK_WAVE);
        }
    }
    return result;
}

int SsdDisposeWaveBank(unsigned short bank)
{
    int result;
    SsdWaveBank *wave = SsdGetWSPtrBank(bank);
    if (wave != 0) {
        SsdDisposeWaveData(wave);
        result = 0;
    } else {
        SsdWarringProc(SSD_ERROR_WAVE_BANK_NOT_FOUND, bank);
        result = SSD_ERROR_WAVE_BANK_NOT_FOUND;
    }
    return result;
}

int SsdDisposeWaveData(SsdWaveBank *wave)
{
    SsdWaveBank *cursor = SsdWork.wave_banks;
    SsdWaveBank *previous = 0;
    while (cursor != 0) {
        if (cursor == wave)
            break;
        previous = cursor;
        cursor = previous->next;
    }
    if (cursor == 0) {
        SsdErrorProc(SSD_ERROR_WAVE_BANK_NOT_FOUND, (unsigned int)wave);
        return SSD_ERROR_WAVE_BANK_NOT_FOUND;
    }
    CpuDisableIntr();
    if (previous != 0)
        previous->next = wave->next;
    else
        SsdWork.wave_banks = wave->next;
    CpuEnableIntr();
    if (wave->spu_address != SsdSpuMFree(wave->spu_address))
        SsdErrorProc(SSD_ERROR_WAVE_SPU_FREE,(unsigned int)wave);
    SsdDisposePtr(wave);
    return 0;
}

int SsdCheckWaveData(int bank)
{
    SsdWaveBank *wave;
    for (wave = SsdWork.wave_banks; wave != 0; wave = wave->next) {
        if (wave->bank_id == bank)
            return bank;
    }
    return -1;
}

SsdWaveBank *SsdGetWSPtrSpuAdrs(unsigned int address)
{
    SsdWaveBank *wave;
    for (wave = SsdWork.wave_banks; wave != 0; wave = wave->next) {
        if (wave->spu_address == address)
            break;
    }
    return wave;
}

SsdWaveBank *SsdGetWSPtrBank(int bank)
{
    SsdWaveBank *wave;
    for (wave = SsdWork.wave_banks; wave != 0; wave = wave->next) {
        if (wave->bank_id == bank)
            return wave;
    }
    return 0;
}

void SsdSetWaveToVoiceControl(SsdVoice *voice, SsdWaveBank *bank, short sample_index)
{
    SsdWaveSample *sample;
    unsigned int body_offset;
    unsigned short adsr1;
    unsigned short adsr2;
    if (bank != 0) {
        sample = &bank->samples[sample_index];
        body_offset = sample->body_offset;
        voice->sample_start = body_offset + bank->spu_address;
        voice->loop_start = body_offset + sample->loop_offset;
        adsr1 = sample->adsr1;
        adsr2 = sample->adsr2;
        voice->wave_index = sample_index;
        voice->adsr2 = adsr2;
        voice->adsr1 = adsr1;
        voice->release_rate = adsr2 & SSD_ADSR2_RELEASE_RATE_MASK;
        voice->base_velocity = sample->volume;
        voice->base_panpot = sample->pan;
        voice->base_note = sample->base_pitch;
        voice->transpose = 0;
    }
}

void SsdSetWaveTransCallback(SsdWaveTransferCallback callback, void *argument)
{
    SsdWork.wave_transfer.callback = callback;
    SsdWork.wave_transfer.argument = argument;
}
