#include "common.h"
#include "shared.h"
#include "main/xgl_2.h"

/*
 * Returns the SPU-DMA busy bit of RssdWork; a nonzero wait first spins until
 * it clears (main:0x002409b0).  Same prototype as src/ov01/snd.h.
 */
extern int SsdSpuDmaCompleted(int wait);
extern void xglSoundSequenceNormal3(int channel, int volume, int time);
extern void xglSoundSequenceFadeOut2(int channel, int time);
extern void xglSoundSequenceStop2(int channel);
void xglMakeSePacket(short command, ...);
void SsdPlayEffectParam(int effect_id, int source_id, int volume, int pan);

void xglSoundWaitDma(void)
{
    SsdSpuDmaCompleted(0);
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundSendSwd);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundSendSmd2);

void xglSoundSendSmd2(void *smd, int bank);

void xglSoundSendSmd(void *smd)
{
    xglSoundSendSmd2(smd, 0);
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundSendSed);

static char SoundDataPath[12] = "data\\sound\\";
static unsigned char ext_0[5] = { '.', 'S', 'W', 'D', '\0' };

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

extern int xglSoundSendSwd(void *swd, int bank);
void xglSoundSendSed(void *sed, int bank);

void xglSoundSendEffect(void *swd, void *sed, int bank)
{
    xglSoundSendSwd(swd, bank);
    do {

    } while (SsdSpuDmaCompleted(0) != 0);
    xglSoundSendSed(sed, bank);
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundLoadEffect);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundLoadRequestSmd);

/*
 * SoundWork (main:0x004a8140), ELF symbol size 0x8a4:
 * - +0x000..0x01f: 8 per-channel entries of 4 bytes; the high halfword of
 *   each entry is the active RSSD sequence handle, read at absolute offset
 *   2 + channel * 4 (xglSoundSequenceNormal3 main:0x00226198,
 *   xglSoundSequenceStop2 main:0x00226298); the low halfword of each entry
 *   is not read by either function.
 * - +0x020..0x09f: 32 per-bank entries of 4 bytes; the low halfword of each
 *   entry is the active direct-play effect handle, read at absolute offset
 *   0x20 + bank * 4 (xglSoundEffectStopDirect main:0x00226720), and the
 *   high halfword is the active file-ID effect handle, read at absolute
 *   offset 0x22 + bank * 4 (xglSoundEffectStopBank main:0x00226770);
 *   0x20 + 32 * 4 == 0xa0, the next field's offset.
 * - +0x0a0: the number of SE packets queued in packet_buffer.
 * - +0x0a4..0x8a3: the queued SE packets, 0x800 bytes (xglSendSePacket
 *   main:0x00227000 passes its address to SsdSendFuncPacket and clears it
 *   with memset); 0xa4 + 0x800 == 0x8a4, the full symbol size.
 */
typedef struct SoundChannelEntry {
    unsigned short _unmodeled_00;
    unsigned short sequence;
} SoundChannelEntry;

typedef struct SoundEffectBankEntry {
    unsigned short handle;
    unsigned short file_handle;
} SoundEffectBankEntry;

struct SoundWork {
    SoundChannelEntry channels[8];
    SoundEffectBankEntry effect_banks[32];
    int packet_count;
    unsigned char packet_buffer[0x800];
};

/* The original linker places this all-zero, 0x8a4-byte object in .data. */
struct SoundWork SoundWork = { 0 };

void SsdStartSequence(int sequence, int start, int loop);
int SsdGetResultValue(int *value);

void xglSoundSequenceNormal3(int channel, int volume, int time)
{
    int result;

    SsdStartSequence(SoundWork.channels[channel].sequence,
                      (volume < 0x80) ? volume : 0x7F,
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

void SsdPauseSequence(int sequence, int time);

void xglSoundSequenceFadeOut2(int channel, int time)
{
    int result;
    unsigned short handle;

    handle = SoundWork.channels[channel].sequence;
    result = handle;
    if (handle != 0xFFFF) {
        SsdPauseSequence(handle, (int) (time + ((unsigned int) time >> 31)) >> 1);
        do {

        } while (SsdGetResultValue(&result) < 0);
    }
}

void xglSoundSequenceFadeOut(int time)
{
    xglSoundSequenceFadeOut2(0, time);
}

void SsdStopSequence(int sequence);

void xglSoundSequenceStop2(int channel)
{
    int result;
    unsigned short handle;

    handle = SoundWork.channels[channel].sequence;
    result = handle;
    if (handle != 0xFFFF) {
        SsdStopSequence(handle);
        do {

        } while (SsdGetResultValue(&result) < 0);
    }
}

void xglSoundSequenceStop(void)
{
    xglSoundSequenceStop2(0);
}

void SsdPlayEffectNormal(int effect_id, int source_id);

void xglSoundEffectNormalDirect(int effect_id)
{
    unsigned short handle = SoundWork.effect_banks[effect_id >> 16].handle;
    int source_id;

    if (handle == 0xFFFF) {
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

    if (handle == 0xFFFF) {
        return;
    }

    if ((pitch == 0) && (soundEffectId > 0)
     && ((soundEffectId < 4) || (soundEffectId == 5))) {
        pitch = xglSRand() & 0x7FFF;
    }

    xglMakeSePacket(112,
                    ((int) handle << 16) + (soundEffectId & 0xFFFF),
                    pitch);
}

void xglSoundEffectParamDirect(int soundEffectId, int volume, int pan)
{
    int bank = soundEffectId >> 16;
    unsigned short handle = SoundWork.effect_banks[bank].handle;

    if (handle != 0xFFFF) {
        int effect_id = (int) ((unsigned int) handle << 16)
                      + (soundEffectId & 0xFFFF);
        int clamped_volume = volume < 0x80 ? volume : 0x7F;

        SsdPlayEffectParam(effect_id, clamped_volume, pan, 0);
    }
}

void xglSoundEffectParamID(int soundEffectId, int volume, int pan, int pitch)
{
    int bank = soundEffectId >> 16;
    unsigned short handle = SoundWork.effect_banks[bank].handle;

    if (handle != 0xFFFF) {
        int effect_id = (int) ((unsigned int) handle << 16)
                      + (soundEffectId & 0xFFFF);
        int clamped_volume = volume < 0x80 ? volume : 0x7F;

        xglMakeSePacket(113, effect_id, clamped_volume, pan, pitch);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundEffectPosID);

void SsdStopEffect(int effect_id, int source_id);

void xglSoundEffectStopDirect(int soundEffectId)
{
    int bank = soundEffectId >> 16;
    unsigned short handle = SoundWork.effect_banks[bank].handle;
    int source = soundEffectId & 0xFFFF;

    if (handle != 0xFFFF) {
        SsdStopEffect((handle << 16) + source, 0);
    }
}

void SsdStopEffectFileID(int file_id);

void xglSoundEffectStopBank(int bank)
{
    unsigned short handle = SoundWork.effect_banks[bank].file_handle;

    if (handle != 0xFFFF) {
        SsdStopEffectFileID(handle);
    }
}

void xglMakeSePacket(short command, ...);

void xglSoundEffectStopID(int soundEffectId, int source)
{
    int bank = soundEffectId >> 16;
    unsigned short handle = SoundWork.effect_banks[bank].handle;

    if (handle != 0xFFFF) {
        xglMakeSePacket(121,
                        ((int) handle << 16) + (soundEffectId & 0xFFFF),
                        source);
    }
}

/*
 * xglSoundEffectCheckID below is the only caller of SsdCheckPlayEffect in
 * this TU and loads a single combined (handle << 16) + source value into
 * $a0 before the call, leaving $a1 untouched; declared here with the one
 * argument this call site sets.
 */
int SsdCheckPlayEffect(int effect_id);
int SsdCheckPlayEffectAll(void);

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
        if (handle != 0xFFFF) {
            SsdCheckPlayEffect((handle << 16) + (soundEffectId & 0xFFFF));
        }
    }
    do {

    } while (SsdGetResultValue(&result) < 0);
    return result;
}

/*
 * The PCM/VAG stream-open helpers below and xglSoundStreamStop/
 * xglSoundStreamMain (still INCLUDE_ASM, main:0x00226b00 and
 * main:0x00226d60) reuse SoundWork.effect_banks[16]..[31] (0x40 bytes at
 * absolute offset 0x60) as the currently open CD stream's scratch state:
 * the leading bytes are the ring parameter xglCdStreamOpen/
 * xglCdStreamReadRing (still INCLUDE_ASM, main/tu092 src/main/xgl_cd.c)
 * fill in, and the trailing bytes are read back by xglSoundStreamStop/
 * xglSoundStreamMain as the open stream's kind and buffer-segment index.
 * Only the fields the functions below touch are modeled.
 */
typedef struct SoundStreamState {
    unsigned char unmodeled_00[0x1c];
    int segment_size;            /* +0x1c: bytes per streaming segment, always 0x1000 */
    unsigned char unmodeled_20[0x0c];
    int read_position;           /* +0x2c: cleared when a stream is (re)opened */
    unsigned char type;          /* +0x30: 0 none, 1 pcm, 2 vag stereo, 3 vag multi */
    unsigned char unmodeled_31[2];
    unsigned char segment_index; /* +0x33: cleared when a stream is (re)opened */
} SoundStreamState;

int xglCdStreamOpen(SoundStreamState *stream, int size);
void xglCdStreamReadRing(SoundStreamState *stream, int bytes);
void SsdInitPcmStream(int channel, int volume);
void SsdPlayPcmStream(int channel, int loop);

void xglSoundStreamOpenPcm(int stream, int source)
{
    SoundStreamState *state;
    int result;

    if (stream == 0) {
        state = (SoundStreamState *) &SoundWork.effect_banks[16];
        if ((source != 0) && (xglCdStreamOpen(state, source) >= 0)) {
            state->segment_size = 0x1000;
            xglCdStreamReadRing(state, 0x2000);
            state->read_position = 0;
            SsdInitPcmStream(0x1000, 4);
            do {

            } while (SsdGetResultValue(&result) < 0);
            SsdPlayPcmStream(0x7F, 0);
            do {

            } while (SsdGetResultValue(&result) < 0);
            state->segment_index = 0;
            state->type = 1;
        }
    }
}

void SsdInitVagStreamStereo(int channel, int volume);
void SsdPlayVagStream(int channel, int start, int loop);

void xglSoundStreamOpenVagStereoParam(int stream, int source, int pitch, int volume)
{
    SoundStreamState *state;
    int result;

    if (stream == 0) {
        state = (SoundStreamState *) &SoundWork.effect_banks[16];
        if ((source != 0) && (xglCdStreamOpen(state, source) >= 0)) {
            state->segment_size = 0x1000;
            xglCdStreamReadRing(state, 0x2000);
            state->read_position = 0;
            SsdInitVagStreamStereo(0x1000, 4);
            do {

            } while (SsdGetResultValue(&result) < 0);
            SsdPlayVagStream((volume < 0x80) ? volume : 0x7F, 0, pitch);
            do {

            } while (SsdGetResultValue(&result) < 0);
            state->segment_index = 0;
            state->type = 2;
        }
    }
}

void xglSoundStreamOpenVagStereoParam(int stream, int source, int pitch, int volume);

void xglSoundStreamOpenVagStereo(int stream, int source)
{
    xglSoundStreamOpenVagStereoParam(stream, source, 0x1000, 0x7F);
}

void SsdInitVagStreamMono(int channel, int volume, int pan);
void SsdSetVagStreamPanpot(int channel, int panpot, int duration);

void xglSoundStreamOpenVagMultiParam(int stream, int source, int pitch, int volume, int pan)
{
    SoundStreamState *state;
    int result;

    if (stream == 0) {
        state = (SoundStreamState *) &SoundWork.effect_banks[16];
        if ((source != 0) && (xglCdStreamOpen(state, source) >= 0)) {
            state->segment_size = 0x1000;
            xglCdStreamReadRing(state, 0x2000);
            state->read_position = 0;
            SsdInitVagStreamMono(0x1000, 1, 4);
            do {

            } while (SsdGetResultValue(&result) < 0);
            SsdSetVagStreamPanpot(stream, pan, 0);
            do {

            } while (SsdGetResultValue(&result) < 0);
            SsdPlayVagStream((volume < 0x80) ? volume : 0x7F, 0, pitch);
            do {

            } while (SsdGetResultValue(&result) < 0);
            state->segment_index = 0;
            state->type = 3;
        }
    }
}

void xglSoundStreamOpenVagMultiParam(int stream, int source, int pitch, int volume, int pan);

void xglSoundStreamOpenVagMulti(int stream, int source)
{
    xglSoundStreamOpenVagMultiParam(stream, source, 0x1000, 0x7F, 0x40);
}

typedef struct CdStreamParam CdStreamParam;
extern int xglCdStreamClose(CdStreamParam *stream);
void SsdDisposePcmStream(void);
void SsdStopPcmStream(int channel);
int SsdGetPcmStreamStatus(void);
void SsdGetVagStreamStatusStereo(void);
void SsdDisposeVagStream(void);
void SsdStopVagStream(int channel);

void xglSoundStreamStop(int stream)
{
    SoundStreamState *state;
    int result;
    unsigned char type;

    if (stream != 0) {
        return;
    }

    state = (SoundStreamState *) &SoundWork.effect_banks[16];
    xglCdStreamClose((CdStreamParam *) state);
    type = state->type;
    switch (type) {
    case 0:
        break;
    case 1:
        SsdGetPcmStreamStatus();
        do {
        } while (SsdGetResultValue(&result) < 0);
        if ((unsigned char) result != 0) {
            SsdStopPcmStream(0);
            SsdDisposePcmStream();
        }
        break;
    case 2:
        SsdGetVagStreamStatusStereo();
        do {
        } while (SsdGetResultValue(&result) < 0);
        if ((unsigned char) result != 0) {
            SsdStopVagStream(0);
            SsdDisposeVagStream();
        }
        break;
    case 3:
        SsdStopVagStream(0);
        do {
        } while (SsdGetResultValue(&result) < 0);
        SsdDisposeVagStream();
        do {
        } while (SsdGetResultValue(&result) < 0);
        break;
    }
    state->type = 0;
}

void SsdGetVagStreamStatusStereo(void);
void SsdSetVagStreamDataStereo(int channel, int address, int size);

void xglSoundStreamMute(void)
{
    int result;
    int base;
    unsigned char *position;
    int remaining;

    base = (int) WorkEnd;
    SsdGetVagStreamStatusStereo();
    remaining = 0xFFF;
    position = (unsigned char *) base + 0xFFF;
    do {
        remaining -= 1;
        *position = 0;
        position -= 1;
    } while (remaining >= 0);
    do {

    } while (SsdGetResultValue(&result) < 0);
    if (result >= 0x100) {
        do {
            SsdSetVagStreamDataStereo(0, base, 0x1000);
            result -= 0x100;
        } while (result >= 0x100);
    }
}

/* CD stream ring fields used by this TU; see the owner, main/tu092. */
typedef struct SoundRingStatus {
    unsigned char unmodeled_00[8];
    unsigned char state;
    unsigned char unmodeled_09[23];
    unsigned char *buffer;
    int capacity;
    int write_position;
    int read_position;
} SoundRingStatus;

static int stream_check(int stream_flags, SoundRingStatus *ring)
{
    int request = stream_flags >> 8;
    int remaining;
    unsigned char *destination;
    int result = 0;

    if (request != 0) {
        if (ring->state != 0) {
            if (ring->state < 6) {
                remaining = 4095;
                destination = &ring->buffer[ring->write_position];
                do {
                    remaining--;
                    *destination = 0;
                    destination++;
                } while (remaining >= 0);
                ring->write_position =
                    (ring->write_position + 4096) % ring->capacity;
                ring->state++;
            } else if (ring->read_position == ring->write_position) {
                return request == 3 ? -1 : 0;
            }
        }
        result = 1;
    }
    return result;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundStreamMain);

typedef struct SePacket {
    short command;
    unsigned char unmodeled_02[14];
    int arguments[4];
} SePacket;

typedef struct SoundArgumentSlot {
    int value;
    int upper;
} SoundArgumentSlot;

typedef char *SoundArgumentList;
#define SOUND_ARGUMENT_START(ap, last) \
    ((ap) = (SoundArgumentList) __builtin_next_arg(last) - \
            (8 - __builtin_args_info(2)) * 8)

void xglMakeSePacket(short command, ...)
{
    /* The 0x800-byte queue holds 64 fixed-size SE packets. */
    typedef struct SoundPacketWork {
        SoundChannelEntry channels[8];
        SoundEffectBankEntry effect_banks[32];
        int packet_count;
        SePacket packets[64];
    } SoundPacketWork;

    SoundPacketWork *queue = (SoundPacketWork *) &SoundWork;
    SoundArgumentList args;
    struct SoundWork *work = &SoundWork;
    SePacket *packet;
    SoundArgumentSlot *arguments;
    SoundArgumentSlot *slot;
    int remaining;

    if (work->packet_count < 65) {
        SOUND_ARGUMENT_START(args, command);
        packet = &queue->packets[work->packet_count];
        arguments = (SoundArgumentSlot *) args;
        queue->packets[work->packet_count].command = command;
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

void SsdSendFuncPacket(void *packets, int count);
extern void *memset(void *destination, int value, unsigned int count);

void xglSendSePacket(void)
{
    SsdSendFuncPacket(SoundWork.packet_buffer, SoundWork.packet_count);
    SoundWork.packet_count = 0;
    memset(SoundWork.packet_buffer, 0, 0x800U);
}

typedef struct SoundPlaybackState {
    SoundRingStatus ring;
    unsigned char type;
    unsigned char unmodeled_31[2];
    unsigned char segment_index;
    unsigned char *playback_buffer;
} SoundPlaybackState;

void xglCdStreamParamInit(void *state);

void xglSoundReset(void)
{
    SoundPlaybackState *state;
    int bank;

    for (bank = 0; bank < 8; bank++) {
        xglSoundSendSwd(0, -1 - bank);
        ((int (*)(void *, int)) xglSoundSendSmd2)(0, bank);
    }
    for (bank = 0; bank < 16; bank++) {
        xglSoundSendEffect(0, 0, bank);
    }
    state = (SoundPlaybackState *) &SoundWork.effect_banks[16];
    state->type = 0;
    xglCdStreamParamInit(state);
    state->ring.buffer = state->playback_buffer;
    state->ring.capacity = 0x4000;
    SoundWork.packet_count = 0;
    memset(SoundWork.packet_buffer, 0, 0x800U);
}

static unsigned char StreamBuffer[0x4000];
void SsdInit(int size);

void xglSoundInitial(void)
{
    SoundChannelEntry *channel;
    SoundPlaybackState *state;
    int result;
    int index;

    SsdInit(0x20000);
    do {
    } while (SsdGetResultValue(&result) < 0);

    channel = SoundWork.channels;
    state = (SoundPlaybackState *) &SoundWork.effect_banks[16];
    state->playback_buffer = StreamBuffer;
    index = 7;
    do {
        index--;
        /* xglSoundSendSwd uses this low halfword for a sequence wave bank. */
        channel->_unmodeled_00 = 0xFFFF;
        channel->sequence = 0xFFFF;
        channel++;
    } while (index >= 0);

    index = 0;
    do {
        SoundWork.effect_banks[index].handle = 0xFFFF;
        SoundWork.effect_banks[index].file_handle = 0xFFFF;
        index++;
    } while (index < 16);
    xglSoundReset();
}
