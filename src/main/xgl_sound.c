#include "common.h"
#include "shared.h"

/*
 * Returns the SPU-DMA busy bit of RssdWork; a nonzero wait first spins until
 * it clears (main:0x002409b0).  Same prototype as src/ov01/snd.h.
 */
extern int SsdSpuDmaCompleted(int wait);
extern void xglSoundSequenceNormal3(int channel, int volume, int time);
extern void xglSoundSequenceFadeOut2(int channel, int time);
extern void xglSoundSequenceStop2(int channel);

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

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundLoadSwd);

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

extern struct SoundWork SoundWork;

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

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundEffectNormalDirect);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundEffectNormalID);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundEffectParamDirect);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundEffectParamID);

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

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundEffectStopID);

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

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundStreamStop);

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

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", stream_check);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundStreamMain);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglMakeSePacket);

void SsdSendFuncPacket(void *packets, int count);
extern void *memset(void *destination, int value, unsigned int count);

void xglSendSePacket(void)
{
    SsdSendFuncPacket(SoundWork.packet_buffer, SoundWork.packet_count);
    SoundWork.packet_count = 0;
    memset(SoundWork.packet_buffer, 0, 0x800U);
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundReset);

INCLUDE_ASM("asm/main/nonmatchings/xgl_sound", xglSoundInitial);
