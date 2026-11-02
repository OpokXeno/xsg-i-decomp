#include "common.h"
#include "shared.h"
#include "main/xgl_2.h"
#include "main/xgl_sound.h"
#include "main/ssd_3.h"
#include "main/ssd_init.h"
#include "main/xgl_cd.h"
#include "main/xgl_font.h"

/* Steps back over the register save area in SoundArgumentSlot (8-byte) units. */
#define SOUND_ARGUMENT_START(ap, last) \
    ((ap) = (SoundArgumentList) __builtin_next_arg(last) - \
            (8 - __builtin_args_info(2)))

#define SOUND_PAN_HALF_TURN 3.1415927f

/* Command ids of the SE packets queued by xglMakeSePacket. */
#define SOUND_SE_PLAY_NORMAL 112
#define SOUND_SE_PLAY_PARAM 113
#define SOUND_SE_STOP 121
#define SOUND_SE_PLAY_POSITION 124

/* Size of the IOP sound driver's work area given to SsdInit. */
#define SOUND_DRIVER_WORK_BYTES 0x20000
/* Streams are fed to the driver in 4 KiB chunks from a ring of 4 chunks. */
#define SOUND_STREAM_CHUNK 0x1000
#define SOUND_STREAM_CHUNK_SHIFT 12
#define SOUND_STREAM_CHUNKS 4
#define SOUND_STREAM_CHUNK_MASK (SOUND_STREAM_CHUNKS - 1)
#define SOUND_STREAM_BUFFER_BYTES 0x4000
#define SOUND_STREAM_PREFILL_BYTES 0x2000
/* stream_check writes this many silent chunks before it accepts the stream's own data. */
#define SOUND_STREAM_SILENCE_CHUNKS 6
#define SOUND_PITCH_NORMAL 0x1000
#define SOUND_PAN_CENTER 0x40

typedef struct SoundArgumentSlot {
    int value;
    int upper;
} SoundArgumentSlot;

typedef SoundArgumentSlot *SoundArgumentList;


/*
 * The sound listener is the player actor held in GameLoopState's second
 * word; its world position is the three floats at +0x10.
 */
typedef struct SoundListener {
    unsigned char unmodeled_00[0x10];
    float x;
    float y;
    float z;
} SoundListener;

typedef unsigned int GameLoopStateWords[];

extern GameLoopStateWords GameLoopState;
extern const char animationCharFormat[];

/*
 * Returns the SPU-DMA busy bit of RssdWork; a nonzero wait first spins until
 * it clears (main:0x002409b0).  Same prototype as src/ov01/snd.h.
 */
extern int SsdSpuDmaCompleted(int wait);

extern void xglSoundSequenceNormal3(int channel, int volume, int time);
void xglMakeSePacket(short command, ...);
void SsdPlayEffectParam(int effect_id, int source_id, int volume, int pan);
int xglSoundSendSed(void *sed, int bank);
void SsdStartSequence(int sequence, int start, int loop);
void SsdPauseSequence(int sequence, int time);
void SsdStopSequence(int sequence);
void SsdPlayEffectNormal(int effect_id, int source_id);
void SsdStopEffect(int effect_id, int source_id);
void SsdStopEffectFileID(int file_id);
int SsdCheckPlayEffect(int effect_id);
int SsdCheckPlayEffectAll(void);
void SsdInitPcmStream(int channel, int volume);
void SsdPlayPcmStream(int channel, int loop);
void SsdInitVagStreamMono(int channel, int volume, int pan);
void SsdSetVagStreamPanpot(int channel, int panpot, int duration);
void SsdDisposePcmStream(void);
void SsdStopPcmStream(int channel);
int SsdGetPcmStreamStatus(void);
void SsdSendFuncPacket(void *packets, int count);
extern void *memset(void *destination, int value, unsigned int count);
void SsdInit(int size);
void SsdDisposeSequence(int sequence);
int SsdAddSequenceData(void *smd);
void SsdDisposeEffectData(int file_id);
int SsdAddEffectData(void *sed);
void SsdDisposeWaveBank(int bank);
int SsdAddWaveData(void *wave, int offset, int size);
extern float atan2f(float y, float x);
void SsdSetPcmStreamData(void *data, int size);
int SsdGetVagStreamStatusAll(void);
int SsdGetResultParam(int *parameters);
void SsdSetVagStreamDataMono(int channel, int address, int size);
static char SoundDataPath[12] = "data\\sound\\";

static unsigned char ext_0[5] = { '.', 'S', 'W', 'D', '\0' };

/* The original linker places this all-zero, 0x8a4-byte object in .data. */
struct SoundWork SoundWork = { 0 };

static char SoundEffectPath[5] = "sed\\";

static char SoundSequencePath[5] = "smd\\";

static unsigned char ext_1[5] = { '.', 'S', 'E', 'D', '\0' };

static unsigned char ext_2_004DC350[5] = { '.', 'S', 'M', 'D', '\0' };

static char anim_4_004DC358[4] = { '-', 0x7F, '|', '/' };

const char animationCharFormat[8] = "%c";

static unsigned char StreamBuffer[SOUND_STREAM_BUFFER_BYTES];

static unsigned char count_3;

void xglSoundWaitDma(void)
{
    SsdSpuDmaCompleted(0);
}

int xglSoundSendSwd(void *swd, int bank)
{
    SoundChannelEntry *entry;
    int result;
    int returned;

    if (bank < 0) {
        entry = SoundWork.channels - 1 - bank;
        if (entry->sequence != SOUND_HANDLE_NONE) {
            SsdStopSequence(entry->sequence);
            do {

            } while (SsdGetResultValue(&result) < 0);
        }
    } else {
        entry = (SoundChannelEntry *) &SoundWork.effect_banks[bank];
        if (entry->sequence != SOUND_HANDLE_NONE) {
            SsdStopEffectFileID(entry->sequence);
            do {

            } while (SsdGetResultValue(&result) < 0);
        }
    }
    if (entry->_unmodeled_00 != SOUND_HANDLE_NONE) {
        SsdDisposeWaveBank(entry->_unmodeled_00);
        do {

        } while (SsdGetResultValue(&result) < 0);
    }
    result = SOUND_HANDLE_NONE;
    if (swd != 0) {
        SsdAddWaveData(swd, 0, 0);
        do {

        } while (SsdGetResultValue(&result) < 0);
        if (result < 0) {
            result = SOUND_HANDLE_NONE;
        }
    }
    returned = result;
    entry->_unmodeled_00 = result;
    return returned;
}

int xglSoundSendSmd2(void *smd, int bank)
{
    SoundChannelEntry *entry = &SoundWork.channels[bank];
    int result;

    if (entry->sequence != SOUND_HANDLE_NONE) {
        SsdStopSequence(entry->sequence);
        do {

        } while (SsdGetResultValue(&result) < 0);
        SsdDisposeSequence(entry->sequence);
        do {

        } while (SsdGetResultValue(&result) < 0);
    }
    result = SOUND_HANDLE_NONE;
    if (smd != 0) {
        SsdAddSequenceData(smd);
        do {

        } while (SsdGetResultValue(&result) < 0);
        if (result < 0) {
            result = SOUND_HANDLE_NONE;
        }
    }
    entry->sequence = result;
    return result;
}

void xglSoundSendSmd(void *smd)
{
    xglSoundSendSmd2(smd, 0);
}

int xglSoundSendSed(void *sed, int bank)
{
    SoundEffectBankEntry *entry = &SoundWork.effect_banks[bank];
    int result;

    if (entry->file_handle != SOUND_HANDLE_NONE) {
        SsdStopEffectFileID(entry->file_handle);
        do {

        } while (SsdGetResultValue(&result) < 0);
        SsdDisposeEffectData(entry->file_handle);
        do {

        } while (SsdGetResultValue(&result) < 0);
    }
    result = SOUND_HANDLE_NONE;
    if (sed != 0) {
        SsdAddEffectData(sed);
        do {

        } while (SsdGetResultValue(&result) < 0);
        if (result < 0) {
            result = SOUND_HANDLE_NONE;
        }
    }
    entry->file_handle = result;
    return result;
}

int xglSoundLoadSwd(const char *file_name, void *buffer)
{
    char path[256];
    char *destination = path;
    const char *source = SoundDataPath;

    while (*source != '\0') {
        *destination = *source;
        destination++;
        source++;
    }
    source = file_name;
    while (*source != '\0') {
        *destination = *source;
        destination++;
        source++;
    }
    source = (const char *) ext_0;
    for (;;) {
        *destination = (unsigned char) *source;
        if (((int) (signed char) *destination << 24) == 0) {
            break;
        }
        source++;
        destination++;
    }
    return xglCdReadFile(path, buffer, 0, 0);
}

void xglSoundSendEffect(void *swd, void *sed, int bank)
{
    xglSoundSendSwd(swd, bank);
    do {

    } while (SsdSpuDmaCompleted(0) != 0);
    xglSoundSendSed(sed, bank);
}

void xglSoundLoadEffect(const char *file_name, void *buffer, int bank)
{
    char path[256];
    char *destination;
    char *request_destination;
    const char *source;
    const char *name_source;
    const char *request_source;

    if (file_name == 0 || buffer == 0) {
        xglSoundSendSwd(0, bank);
        xglSoundSendSed(0, bank);
        return;
    }
    destination = path;
    source = SoundEffectPath;
    while (*source != '\0') {
        *destination = *source;
        destination++;
        source++;
    }
    name_source = file_name;
    for (;;) {
        *destination = *name_source;
        if (((int) (signed char) *destination << 24) == 0) {
            break;
        }
        name_source++;
        destination++;
    }
    if (xglSoundLoadSwd(path, buffer) > 0) {
        xglSoundSendSwd(buffer, bank);
        do {

        } while (SsdSpuDmaCompleted(0) != 0);
    }
    request_destination = path;
    request_source = SoundDataPath;
    while (*request_source != '\0') {
        *request_destination = *request_source;
        request_destination++;
        request_source++;
    }
    request_source = SoundEffectPath;
    while (*request_source != '\0') {
        *request_destination = *request_source;
        request_destination++;
        request_source++;
    }
    request_source = file_name;
    while (*request_source != '\0') {
        *request_destination = *request_source;
        request_destination++;
        request_source++;
    }
    request_source = (const char *) ext_1;
    for (;;) {
        *request_destination = (unsigned char) *request_source;
        if (((int) (signed char) *request_destination << 24) == 0) {
            break;
        }
        request_source++;
        request_destination++;
    }
    if (xglCdReadFile(path, buffer, 0, 0) > 0) {
        xglSoundSendSed(buffer, bank);
    }
}

void xglSoundLoadRequestSmd(const char *file_name, void *buffer)
{
    char path[256];
    char *destination;
    char *request_destination;
    const char *source;
    const char *name_source;
    const char *request_source;

    if (file_name == 0 || buffer == 0) {
        xglSoundSendSwd(0, -1);
        xglSoundSendSmd2(0, 0);
        return;
    }
    destination = path;
    source = SoundSequencePath;
    while (*source != '\0') {
        *destination = *source;
        destination++;
        source++;
    }
    name_source = file_name;
    for (;;) {
        *destination = *name_source;
        if (((int) (signed char) *destination << 24) == 0) {
            break;
        }
        name_source++;
        destination++;
    }
    if (xglSoundLoadSwd(path, buffer) > 0) {
        xglSoundSendSwd(buffer, -1);
        do {

        } while (SsdSpuDmaCompleted(0) != 0);
    }
    request_destination = path;
    request_source = SoundDataPath;
    while (*request_source != '\0') {
        *request_destination = *request_source;
        request_destination++;
        request_source++;
    }
    request_source = SoundSequencePath;
    while (*request_source != '\0') {
        *request_destination = *request_source;
        request_destination++;
        request_source++;
    }
    request_source = file_name;
    while (*request_source != '\0') {
        *request_destination = *request_source;
        request_destination++;
        request_source++;
    }
    request_source = (const char *) ext_2_004DC350;
    for (;;) {
        *request_destination = (unsigned char) *request_source;
        if (((int) (signed char) *request_destination << 24) == 0) {
            break;
        }
        request_source++;
        request_destination++;
    }
    if (xglCdReadFile(path, buffer, 0, 0) > 0) {
        xglSoundSendSmd2(buffer, 0);
    }
    xglSoundSequenceNormal2(0, SOUND_VOLUME_MAX);
}

void xglSoundSequenceNormal3(int channel, int volume, int time)
{
    int result;

    SsdStartSequence(SoundWork.channels[channel].sequence,
                      (volume < SOUND_VOLUME_RANGE) ? volume : SOUND_VOLUME_MAX,
                      (int) (((unsigned int) time >> 31) + time) >> 1);
    do {

    } while (SsdGetResultValue(&result) < 0);
}

void xglSoundSequenceNormal2(int channel, int volume)
{
    xglSoundSequenceNormal3(channel, volume, 0);
}

void xglSoundSequenceNormal(int volume)
{
    xglSoundSequenceNormal3(0, volume, 0);
}

void xglSoundSequenceFadeOut2(int channel, int time)
{
    int result;
    unsigned short handle;

    handle = SoundWork.channels[channel].sequence;
    result = handle;
    if (handle != SOUND_HANDLE_NONE) {
        SsdPauseSequence(handle, (int) (time + ((unsigned int) time >> 31)) >> 1);
        do {

        } while (SsdGetResultValue(&result) < 0);
    }
}

void xglSoundSequenceFadeOut(int time)
{
    xglSoundSequenceFadeOut2(0, time);
}

void xglSoundSequenceStop2(int channel)
{
    int result;
    unsigned short handle;

    handle = SoundWork.channels[channel].sequence;
    result = handle;
    if (handle != SOUND_HANDLE_NONE) {
        SsdStopSequence(handle);
        do {

        } while (SsdGetResultValue(&result) < 0);
    }
}

void xglSoundSequenceStop(void)
{
    xglSoundSequenceStop2(0);
}

void xglSoundEffectNormalDirect(int effect_id)
{
    unsigned short handle = SoundWork.effect_banks[effect_id >> 16].handle;
    int source_id;

    if (handle == SOUND_HANDLE_NONE) {
        return;
    }

    source_id = 0;
    if (effect_id > 0 && (effect_id < 4 || effect_id == 5)) {
        source_id = xglSRand() & 0x7FFF;
    }

    SsdPlayEffectNormal(((unsigned int) handle << 16)
                            + (effect_id & 0xFFFF),
                        source_id);
}

void xglSoundEffectNormalID(int soundEffectId, int pitch)
{
    int bank = soundEffectId >> 16;
    unsigned short handle = SoundWork.effect_banks[bank].handle;

    if (handle == SOUND_HANDLE_NONE) {
        return;
    }

    if ((pitch == 0) && (soundEffectId > 0)
     && ((soundEffectId < 4) || (soundEffectId == 5))) {
        pitch = xglSRand() & 0x7FFF;
    }

    xglMakeSePacket(SOUND_SE_PLAY_NORMAL,
                    (handle << 16) + (soundEffectId & 0xFFFF),
                    pitch);
}

void xglSoundEffectParamDirect(int soundEffectId, int volume, int pan)
{
    int bank = soundEffectId >> 16;
    unsigned short handle = SoundWork.effect_banks[bank].handle;

    if (handle != SOUND_HANDLE_NONE) {
        int effect_id = (int) ((unsigned int) handle << 16)
                      + (soundEffectId & 0xFFFF);
        int clamped_volume = volume < SOUND_VOLUME_RANGE ? volume : SOUND_VOLUME_MAX;

        SsdPlayEffectParam(effect_id, clamped_volume, pan, 0);
    }
}

void xglSoundEffectParamID(int soundEffectId, int volume, int pan, int pitch)
{
    int bank = soundEffectId >> 16;
    unsigned short handle = SoundWork.effect_banks[bank].handle;

    if (handle != SOUND_HANDLE_NONE) {
        int effect_id = (int) ((unsigned int) handle << 16)
                      + (soundEffectId & 0xFFFF);
        int clamped_volume = volume < SOUND_VOLUME_RANGE ? volume : SOUND_VOLUME_MAX;

        xglMakeSePacket(SOUND_SE_PLAY_PARAM, effect_id, clamped_volume, pan, pitch);
    }
}

void xglSoundEffectPosID(int soundEffectId, const float *position, int use_update_packet, int source)
{
    SoundListener *listener = (SoundListener *) GameLoopState[1];
    float offset[3];
    float attenuation;
    float angle;
    int pan;
    int volume;

    offset[0] = position[0] - listener->x;
    offset[1] = position[1] - listener->y;
    offset[2] = position[2] - listener->z;
    attenuation = __builtin_sqrtf(offset[0] * offset[0] + offset[1] * offset[1]
                              + offset[2] * offset[2]);
    if (attenuation > 6.0f) {
        xglSoundEffectStopID(soundEffectId, source);
        return;
    }
    attenuation = attenuation * attenuation / 13.0f;
    if (attenuation < 1.0f) {
        attenuation = 1.0f;
    }
    angle = atan2f(offset[0], offset[2]) + 1.5707964f;
    while (angle > 3.1415927f) {
        angle -= 6.2831855f;
    }
    if (angle < 0.0f) {
        angle = -angle;
    }
    pan = F2I(angle * 127.0f / SOUND_PAN_HALF_TURN);
    volume = 12700 / F2I(attenuation * 100.0f);
    if (use_update_packet == 0) {
        xglSoundEffectParamID(soundEffectId, volume, pan, source);
        return;
    }
    xglMakeSePacket(SOUND_SE_PLAY_POSITION,
                    (SoundWork.effect_banks[soundEffectId >> 16].handle << 16)
                        + (soundEffectId & 0xFFFF),
                    volume, pan, source);
}

void xglSoundEffectStopDirect(int soundEffectId)
{
    int bank = soundEffectId >> 16;
    unsigned short handle = SoundWork.effect_banks[bank].handle;
    int source = soundEffectId & 0xFFFF;

    if (handle != SOUND_HANDLE_NONE) {
        SsdStopEffect((handle << 16) + source, 0);
    }
}

void xglSoundEffectStopBank(int bank)
{
    unsigned short handle = SoundWork.effect_banks[bank].file_handle;

    if (handle != SOUND_HANDLE_NONE) {
        SsdStopEffectFileID(handle);
    }
}

void xglSoundEffectStopID(int soundEffectId, int source)
{
    int bank = soundEffectId >> 16;
    unsigned short handle = SoundWork.effect_banks[bank].handle;

    if (handle != SOUND_HANDLE_NONE) {
        xglMakeSePacket(SOUND_SE_STOP,
                        ((int) handle << 16) + (soundEffectId & 0xFFFF),
                        source);
    }
}

int xglSoundEffectCheckID(int soundEffectId)
{
    int result;
    int bank;
    unsigned short handle;

    result = 0;
    if (soundEffectId < 0) {
        result = SsdCheckPlayEffectAll();
    } else {
        bank = soundEffectId >> 16;
        handle = SoundWork.effect_banks[bank].handle;
        if (handle != SOUND_HANDLE_NONE) {
            SsdCheckPlayEffect((handle << 16) + (soundEffectId & 0xFFFF));
        }
    }
    do {

    } while (SsdGetResultValue(&result) < 0);
    return result;
}

void xglSoundStreamOpenPcm(int stream, const char *source)
{
    SoundStream *state;
    int result;

    if (stream == 0) {
        state = &SoundWork.stream;
        if ((source != 0) && (xglCdStreamOpen(&state->cd, source) >= 0)) {
            state->cd.ring_size = SOUND_STREAM_CHUNK;
            xglCdStreamReadRing(&state->cd, SOUND_STREAM_PREFILL_BYTES);
            state->cd.ring.read_position = 0;
            SsdInitPcmStream(SOUND_STREAM_CHUNK, SOUND_STREAM_CHUNKS);
            do {

            } while (SsdGetResultValue(&result) < 0);
            SsdPlayPcmStream(SOUND_VOLUME_MAX, 0);
            do {

            } while (SsdGetResultValue(&result) < 0);
            state->playback.segment_index = 0;
            state->playback.type = SOUND_STREAM_PCM;
        }
    }
}

void xglSoundStreamOpenVagStereoParam(int stream, const char *source, int pitch, int volume)
{
    SoundStream *state;
    int result;

    if (stream == 0) {
        state = &SoundWork.stream;
        if ((source != 0) && (xglCdStreamOpen(&state->cd, source) >= 0)) {
            state->cd.ring_size = SOUND_STREAM_CHUNK;
            xglCdStreamReadRing(&state->cd, SOUND_STREAM_PREFILL_BYTES);
            state->cd.ring.read_position = 0;
            SsdInitVagStreamStereo(SOUND_STREAM_CHUNK, SOUND_STREAM_CHUNKS);
            do {

            } while (SsdGetResultValue(&result) < 0);
            SsdPlayVagStream((volume < SOUND_VOLUME_RANGE) ? volume : SOUND_VOLUME_MAX, 0, pitch);
            do {

            } while (SsdGetResultValue(&result) < 0);
            state->playback.segment_index = 0;
            state->playback.type = SOUND_STREAM_VAG_STEREO;
        }
    }
}

void xglSoundStreamOpenVagStereo(int stream, const char *source)
{
    xglSoundStreamOpenVagStereoParam(stream, source, SOUND_PITCH_NORMAL, SOUND_VOLUME_MAX);
}

void xglSoundStreamOpenVagMultiParam(int stream, const char *source, int pitch, int volume, int pan)
{
    SoundStream *state;
    int result;

    if (stream == 0) {
        state = &SoundWork.stream;
        if ((source != 0) && (xglCdStreamOpen(&state->cd, source) >= 0)) {
            state->cd.ring_size = SOUND_STREAM_CHUNK;
            xglCdStreamReadRing(&state->cd, SOUND_STREAM_PREFILL_BYTES);
            state->cd.ring.read_position = 0;
            SsdInitVagStreamMono(SOUND_STREAM_CHUNK, 1, SOUND_STREAM_CHUNKS);
            do {

            } while (SsdGetResultValue(&result) < 0);
            SsdSetVagStreamPanpot(stream, pan, 0);
            do {

            } while (SsdGetResultValue(&result) < 0);
            SsdPlayVagStream((volume < SOUND_VOLUME_RANGE) ? volume : SOUND_VOLUME_MAX, 0, pitch);
            do {

            } while (SsdGetResultValue(&result) < 0);
            state->playback.segment_index = 0;
            state->playback.type = SOUND_STREAM_VAG_MULTI;
        }
    }
}

void xglSoundStreamOpenVagMulti(int stream, const char *source)
{
    xglSoundStreamOpenVagMultiParam(stream, source, SOUND_PITCH_NORMAL, SOUND_VOLUME_MAX, SOUND_PAN_CENTER);
}

void xglSoundStreamStop(int stream)
{
    SoundStream *state;
    int result;
    unsigned char type;

    if (stream != 0) {
        return;
    }

    state = &SoundWork.stream;
    xglCdStreamClose(&state->cd);
    type = state->playback.type;
    switch (type) {
    case SOUND_STREAM_NONE:
        break;
    case SOUND_STREAM_PCM:
        SsdGetPcmStreamStatus();
        do {
        } while (SsdGetResultValue(&result) < 0);
        if ((unsigned char) result != 0) {
            SsdStopPcmStream(0);
            SsdDisposePcmStream();
        }
        break;
    case SOUND_STREAM_VAG_STEREO:
        SsdGetVagStreamStatusStereo();
        do {
        } while (SsdGetResultValue(&result) < 0);
        if ((unsigned char) result != 0) {
            SsdStopVagStream(0);
            SsdDisposeVagStream();
        }
        break;
    case SOUND_STREAM_VAG_MULTI:
        SsdStopVagStream(0);
        do {
        } while (SsdGetResultValue(&result) < 0);
        SsdDisposeVagStream();
        do {
        } while (SsdGetResultValue(&result) < 0);
        break;
    }
    state->playback.type = SOUND_STREAM_NONE;
}

void xglSoundStreamMute(void)
{
    int result;
    int base;
    unsigned char *position;
    int remaining;

    base = (int) WorkEnd;
    SsdGetVagStreamStatusStereo();
    remaining = SOUND_STREAM_CHUNK - 1;
    position = (unsigned char *) base + SOUND_STREAM_CHUNK - 1;
    do {
        remaining -= 1;
        *position = 0;
        position -= 1;
    } while (remaining >= 0);
    do {

    } while (SsdGetResultValue(&result) < 0);
    if (result >= 0x100) {
        do {
            SsdSetVagStreamDataStereo(0, base, SOUND_STREAM_CHUNK);
            result -= 0x100;
        } while (result >= 0x100);
    }
}

static int stream_check(int stream_flags, CdStreamParam *stream)
{
    int request = stream_flags >> 8;
    int remaining;
    unsigned char *destination;
    int result = 0;

    if (request != 0) {
        if (stream->state != 0) {
            if (stream->state < SOUND_STREAM_SILENCE_CHUNKS) {
                remaining = SOUND_STREAM_CHUNK - 1;
                destination = &stream->ring.buffer[stream->ring.write_position];
                do {
                    remaining--;
                    *destination = 0;
                    destination++;
                } while (remaining >= 0);
                stream->ring.write_position =
                    (stream->ring.write_position + SOUND_STREAM_CHUNK) % stream->ring.capacity;
                stream->state++;
            } else if (stream->ring.read_position == stream->ring.write_position) {
                return request == 3 ? -1 : 0;
            }
        }
        result = 1;
    }
    return result;
}

int xglSoundStreamMain(void)
{
    SoundStream *state = &SoundWork.stream;
    int parameters[8];
    int result;
    int status;

    if (state->playback.type != SOUND_STREAM_NONE) {
        count_3++;
        xglFontDebugPrintf(244, 8, animationCharFormat, anim_4_004DC358[(count_3 >> 3) & 3]);
    }
    status = -1;
    switch (state->playback.type) {
    case SOUND_STREAM_NONE:
        break;
    case SOUND_STREAM_PCM:
        SsdGetPcmStreamStatus();
        do {

        } while (SsdGetResultValue(&result) < 0);
        status = stream_check(result, &state->cd);
        if (status == -1) {
            xglSoundStreamStop(0);
        } else if (status == 1) {
            SsdSetPcmStreamData(state->playback.playback_buffer + (state->playback.segment_index << SOUND_STREAM_CHUNK_SHIFT), SOUND_STREAM_CHUNK);
            state->cd.ring.read_position = state->playback.segment_index << SOUND_STREAM_CHUNK_SHIFT;
            state->playback.segment_index = (state->playback.segment_index + 1) & SOUND_STREAM_CHUNK_MASK;
        }
        break;
    case SOUND_STREAM_VAG_STEREO:
        SsdGetVagStreamStatusStereo();
        do {

        } while (SsdGetResultValue(&result) < 0);
        status = stream_check(result, &state->cd);
        if (status == -1) {
            xglSoundStreamStop(0);
        } else if (status == 1) {
            SsdSetVagStreamDataStereo(0, (int) (state->playback.playback_buffer + (state->playback.segment_index << SOUND_STREAM_CHUNK_SHIFT)), SOUND_STREAM_CHUNK);
            state->cd.ring.read_position = state->playback.segment_index << SOUND_STREAM_CHUNK_SHIFT;
            state->playback.segment_index = (state->playback.segment_index + 1) & SOUND_STREAM_CHUNK_MASK;
        }
        break;
    case SOUND_STREAM_VAG_MULTI:
        SsdGetVagStreamStatusAll();
        do {

        } while (SsdGetResultParam(parameters) < 0);
        result = parameters[4];
        status = stream_check(result, &state->cd);
        if (status == -1) {
            xglSoundStreamStop(0);
        } else if (status == 1) {
            SsdSetVagStreamDataMono(0, (int) (state->playback.playback_buffer + (state->playback.segment_index << SOUND_STREAM_CHUNK_SHIFT)), SOUND_STREAM_CHUNK);
            state->cd.ring.read_position = state->playback.segment_index << SOUND_STREAM_CHUNK_SHIFT;
            state->playback.segment_index = (state->playback.segment_index + 1) & SOUND_STREAM_CHUNK_MASK;
        }
        break;
    }
    return status;
}

void xglMakeSePacket(short command, ...)
{
    SoundArgumentList args;
    struct SoundWork *work = &SoundWork;
    SePacket *packet;
    SoundArgumentSlot *arguments;
    SoundArgumentSlot *slot;
    int remaining;

    if (work->packet_count < SOUND_SE_PACKET_COUNT + 1) {
        SOUND_ARGUMENT_START(args, command);
        packet = &work->packets[work->packet_count];
        arguments = args;
        work->packets[work->packet_count].command = command;
        remaining = 3;
        do {
            slot = arguments;
            arguments++;
            packet->arguments[3 - remaining] = slot->value;
            remaining--;
        } while (remaining >= 0);
        SoundWork.packet_count++;
    }
}

void xglSendSePacket(void)
{
    SsdSendFuncPacket(SoundWork.packets, SoundWork.packet_count);
    SoundWork.packet_count = 0;
    memset(SoundWork.packets, 0, SOUND_SE_QUEUE_BYTES);
}

void xglSoundReset(void)
{
    SoundStream *state;
    int bank;

    for (bank = 0; bank < SOUND_SEQUENCE_CHANNELS; bank++) {
        xglSoundSendSwd(0, -1 - bank);
        (xglSoundSendSmd2)(0, bank);
    }
    for (bank = 0; bank < SOUND_EFFECT_BANKS; bank++) {
        xglSoundSendEffect(0, 0, bank);
    }
    state = &SoundWork.stream;
    state->playback.type = SOUND_STREAM_NONE;
    xglCdStreamParamInit(&state->cd);
    state->cd.ring.buffer = state->playback.playback_buffer;
    state->cd.ring.capacity = SOUND_STREAM_BUFFER_BYTES;
    SoundWork.packet_count = 0;
    memset(SoundWork.packets, 0, SOUND_SE_QUEUE_BYTES);
}

void xglSoundInitial(void)
{
    SoundChannelEntry *channel;
    SoundStream *state;
    int result;
    int index;

    SsdInit(SOUND_DRIVER_WORK_BYTES);
    do {
    } while (SsdGetResultValue(&result) < 0);

    channel = SoundWork.channels;
    state = &SoundWork.stream;
    state->playback.playback_buffer = StreamBuffer;
    index = 7;
    do {
        index--;
        /* xglSoundSendSwd uses this low halfword for a sequence wave bank. */
        channel->_unmodeled_00 = SOUND_HANDLE_NONE;
        channel->sequence = SOUND_HANDLE_NONE;
        channel++;
    } while (index >= 0);

    index = 0;
    do {
        SoundWork.effect_banks[index].handle = SOUND_HANDLE_NONE;
        SoundWork.effect_banks[index].file_handle = SOUND_HANDLE_NONE;
        index++;
    } while (index < SOUND_EFFECT_BANKS);
    xglSoundReset();
}
