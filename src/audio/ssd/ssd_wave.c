/* Original ssd_wave.c translation unit. Shared data is defined by its owning FDR. */





typedef struct SsdWaveBank SsdWaveBank;
typedef struct SsdGlobalWork SsdGlobalWork;
typedef struct SsdWaveFileHeader SsdWaveFileHeader;
typedef struct SsdSpuWaveSegment SsdSpuWaveSegment;
typedef struct SsdWaveTransferState SsdWaveTransferState;
typedef struct SsdWaveChunk SsdWaveChunk;
typedef struct SsdWaveSample SsdWaveSample;
typedef struct SsdVoiceControl SsdVoiceControl;

struct SsdWaveBank {
    unsigned int signature;
    unsigned short bank_id;
    unsigned char sample_count;
    unsigned char program_count;
    unsigned char unmodeled_008[4];
    unsigned int spu_address;
    unsigned int body_size;
    unsigned char unmodeled_014[4];
    SsdWaveSample *samples;
    void *programs;
    unsigned char unmodeled_020[4];
    struct SsdWaveBank *next;
};

struct SsdSpuWaveSegment {
    unsigned int address;
    unsigned int segment_size;
};

typedef void (*SsdWaveTransferCallback)(int result, void *argument);

struct SsdWaveTransferState {
    unsigned int next_spu_address;
    int bank_id;
    SsdWaveTransferCallback callback;
    void *argument;
};

struct SsdWaveChunk {
    unsigned short type;
    unsigned short size;
    unsigned char unmodeled_004[12];
};

struct SsdWaveSample {
    unsigned int body_offset;
    unsigned int loop_offset;
    unsigned char volume;
    unsigned char pan;
    short base_pitch;
    unsigned short adsr1;
    unsigned short adsr2;
    char name[16];
};

struct SsdVoiceControl {
    unsigned char unmodeled_000[28];
    unsigned int start_address;
    unsigned int loop_offset;
    unsigned short adsr1;
    unsigned short adsr2;
    unsigned char unmodeled_028[20];
    unsigned char sustain_rate;
    unsigned char wave_flags;
    unsigned char unmodeled_03e;
    unsigned char sample_index;
    short base_pitch;
    unsigned short volume;
    unsigned short pan;
};

struct SsdGlobalWork {
    unsigned char unmodeled_000[0x124];
    SsdSpuWaveSegment wave_segments[4];
    unsigned char unmodeled_144[0x30];
    SsdWaveBank *wave_banks;
    unsigned char unmodeled_178[12];
    SsdWaveTransferState wave_transfer;
};

struct SsdWaveFileHeader {
    unsigned int signature;
    unsigned char unmodeled_004[12];
    unsigned short flags;
    unsigned short bank_id;
    unsigned char sample_count;
    unsigned char program_count;
    unsigned char unmodeled_016[10];
    unsigned int spu_address;
    unsigned int body_size;
    unsigned int body_offset;
    unsigned char unmodeled_02c[20];
    SsdWaveChunk first_chunk;
};

SsdWaveBank *SsdGetWSPtrBank(int bank);
int SsdDisposeWaveData(SsdWaveBank *wave);
extern void SsdWarringProc(int error, unsigned int value);
extern SsdGlobalWork SsdWork;
extern void SsdErrorProc(int error, unsigned int value);
extern void CpuDisableIntr(void);
extern void CpuEnableIntr(void);
extern unsigned int SsdSpuMFree(unsigned int address);
extern void SsdDisposePtr(void *pointer);
extern unsigned int SsdSpuMalloc(unsigned int size, int tag);
extern unsigned int SsdSpuMallocAdrs(unsigned int size, unsigned int address, int tag);
extern unsigned int SsdSpuMallocAdrsDynamic(unsigned int size, unsigned int address, int tag);
extern unsigned int SsdSpuMallocAdrsSegment(unsigned int size, unsigned int address,
                                         unsigned int segment_size, int tag);
unsigned int SsdSpuWaveMalloc(const SsdWaveFileHeader *header, unsigned int address);
extern void SsdSpuDmaWrite(unsigned int address, const void *data, int size,
                          SsdWaveTransferCallback callback, void *argument);
extern void *SsdNewPtr2(int size);
extern void SsdMemoryCopy(void *destination, const void *source, int size);
int SsdDisposeWaveBank(unsigned short bank);
void SsdSetWaveTransCallback(SsdWaveTransferCallback callback, void *argument);
SsdWaveBank *SsdGetWSPtrSpuAdrs(unsigned int address);
int SsdCheckWaveData(int bank);
int SsdAddWaveData(const void *data, int size, int spu_address);
int SsdNextWaveData(const void *data, int size);
void SsdSetWaveToVoiceControl(SsdVoiceControl *voice, SsdWaveBank *bank, short sample_index);

int SsdAddWaveData(const void *data, int size, int spu_address)
{
    const SsdWaveFileHeader *header;
    const void *sample_data;
    const void *program_data;
    int sample_size;
    int program_size;
    const SsdWaveChunk *chunk;
    int scanning;
    int chunk_size;
    int transfer_size;
    SsdWaveBank *wave;
    char *metadata;
    SsdWaveBank **tail;

    SsdWork.wave_transfer.next_spu_address = 0;
    header = data;
    if (header->signature != 0x6d647773) {
        SsdErrorProc(-1, (unsigned int)header);
        if (SsdWork.wave_transfer.callback != 0)
            SsdWork.wave_transfer.callback(-2, SsdWork.wave_transfer.argument);
        return -1;
    }
    if (SsdGetWSPtrBank(header->bank_id) != 0) {
        SsdWarringProc(-24, header->bank_id);
        if (SsdWork.wave_transfer.callback != 0)
            SsdWork.wave_transfer.callback(-2, SsdWork.wave_transfer.argument);
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
        case 3:
            sample_data = chunk + 1;
            sample_size = chunk_size - sizeof(SsdWaveChunk);
            break;
        case 4:
            program_data = chunk + 1;
            program_size = chunk_size - sizeof(SsdWaveChunk);
            break;
        case 0:
            scanning = 0;
            break;
        }
        /* Variable-size on-disk records advance by their declared byte length. */
        chunk = (const SsdWaveChunk *)((const char *)chunk + chunk_size);
    } while (scanning);

    spu_address = SsdSpuWaveMalloc(header, spu_address);
    if (spu_address == 0) {
        SsdErrorProc(-33, header->bank_id);
        if (SsdWork.wave_transfer.callback != 0)
            SsdWork.wave_transfer.callback(-2, SsdWork.wave_transfer.argument);
        return -33;
    }
    transfer_size = size - header->body_offset;
    SsdSpuDmaWrite(spu_address, chunk, transfer_size,
                   SsdWork.wave_transfer.callback, SsdWork.wave_transfer.argument);
    SsdWork.wave_transfer.next_spu_address = spu_address + transfer_size;
    wave = SsdNewPtr2(sample_size + program_size + sizeof(SsdWaveBank));
    if (wave == 0) {
        SsdSpuMFree(spu_address);
        SsdErrorProc(-32, header->bank_id);
        return -32;
    }
    wave->signature = 0x76617773;
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

int SsdNextWaveData(const void *data, int size)
{
    SsdWaveTransferState *state = &SsdWork.wave_transfer;
    unsigned int address = state->next_spu_address;
    if (address == 0) {
        SsdWarringProc(-28, 0);
        if (state->callback != 0)
            state->callback(-2, state->argument);
        return -28;
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
    else if (address == ~0U)
        address = 0;
    size = header->body_size;
    if (address == 0) {
        result = SsdSpuMalloc(size, 9);
    } else {
        segment = 0;
        address = header->spu_address;
        for (i = 0; i < 4; i++) {
            if (SsdWork.wave_segments[i].address == address) {
                segment = SsdWork.wave_segments[i].segment_size;
                break;
            }
        }
        if (segment != 0) {
            result = SsdSpuMallocAdrsSegment(size, address, segment, 9);
        } else if (header->flags & 1) {
            result = SsdSpuMallocAdrsDynamic(size, address, 9);
        } else {
            result = SsdSpuMallocAdrs(size, address, 9);
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
        SsdWarringProc(-16, bank);
        result = -16;
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
        SsdErrorProc(-16, (unsigned int)wave);
        return -16;
    }
    CpuDisableIntr();
    if (previous != 0)
        previous->next = wave->next;
    else
        SsdWork.wave_banks = wave->next;
    CpuEnableIntr();
    if (wave->spu_address != SsdSpuMFree(wave->spu_address))
        SsdErrorProc(-43, (unsigned int)wave);
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

void SsdSetWaveToVoiceControl(SsdVoiceControl *voice, SsdWaveBank *bank, short sample_index)
{
    SsdWaveSample *sample;
    unsigned int body_offset;
    unsigned short adsr1;
    unsigned short adsr2;
    if (bank != 0) {
        sample = &bank->samples[sample_index];
        body_offset = sample->body_offset;
        voice->start_address = body_offset + bank->spu_address;
        voice->loop_offset = body_offset + sample->loop_offset;
        adsr1 = sample->adsr1;
        adsr2 = sample->adsr2;
        voice->sample_index = sample_index;
        voice->adsr2 = adsr2;
        voice->adsr1 = adsr1;
        voice->sustain_rate = adsr2 & 31;
        voice->volume = sample->volume;
        voice->pan = sample->pan;
        voice->base_pitch = sample->base_pitch;
        voice->wave_flags = 0;
    }
}

void SsdSetWaveTransCallback(SsdWaveTransferCallback callback, void *argument)
{
    SsdWork.wave_transfer.callback = callback;
    SsdWork.wave_transfer.argument = argument;
}
