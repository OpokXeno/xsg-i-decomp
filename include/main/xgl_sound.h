#ifndef INCLUDE_MAIN_XGL_SOUND_H
#define INCLUDE_MAIN_XGL_SOUND_H

#include "main/xgl_cd.h"

/* Sequence (SMD) channels and sound effect (SED) banks held by SoundWork. */
#define SOUND_SEQUENCE_CHANNELS 8
#define SOUND_EFFECT_BANKS 16
/* The SE queue is 0x800 bytes of 32-byte packets. */
#define SOUND_SE_PACKET_COUNT 64
#define SOUND_SE_QUEUE_BYTES 0x800U
/* A bank, sequence or effect handle that holds no data. */
#define SOUND_HANDLE_NONE 0xFFFF
/* MIDI-style volume 0..127. */
#define SOUND_VOLUME_RANGE 0x80
#define SOUND_VOLUME_MAX 0x7F
/* SoundStream.playback.type. */
#define SOUND_STREAM_NONE 0
#define SOUND_STREAM_PCM 1
#define SOUND_STREAM_VAG_STEREO 2
#define SOUND_STREAM_VAG_MULTI 3

typedef struct SoundChannelEntry {
    unsigned short _unmodeled_00;
    unsigned short sequence;
} SoundChannelEntry;

typedef struct SoundEffectBankEntry {
    unsigned short handle;
    unsigned short file_handle;
} SoundEffectBankEntry;

typedef struct SePacket {
    short command;
    unsigned char unmodeled_02[14];
    int arguments[4];
} SePacket;

/*
 * The CD stream of sound stream 0. The CD layer opens, reads and closes it
 * as a CdStreamParam; a sound stream reads only the first ring, so the sound
 * layer keeps its playback state in the alternate ring's slot (+0x30).
 */
typedef union SoundStream {
    CdStreamParam cd;
    struct {
        unsigned char unmodeled_00[0x30];
        unsigned char type; /* 0 none, 1 pcm, 2 vag stereo, 3 vag multi */
        unsigned char unmodeled_31[2];
        unsigned char segment_index;
        unsigned char *playback_buffer;
    } playback;
} SoundStream;

struct SoundWork {
    SoundChannelEntry channels[SOUND_SEQUENCE_CHANNELS];
    /* xglSoundInitial resets exactly these banks. */
    SoundEffectBankEntry effect_banks[SOUND_EFFECT_BANKS];
    SoundStream stream;
    int packet_count;
    /* The 0x800-byte SE queue: 64 fixed-size packets. */
    SePacket packets[SOUND_SE_PACKET_COUNT];
};

extern int xglSoundStreamMain(void);
extern void xglSoundEffectNormalID(int sound_id, int variant);
void xglSoundEffectStopID(int soundEffectId, int source);
void xglSoundSendEffect(void *swd, void *sed, int bank);

/* Functions and data used by other translation units. */
extern struct SoundWork SoundWork;
void xglSoundEffectPosID(int soundEffectId, const float *position, int use_update_packet, int source);
void xglSoundEffectNormalDirect(int effect_id);
void xglSoundStreamStop(int stream);
extern int xglSoundSendSwd(void *swd, int bank);
/* The original leaves the sequence result word in v0, including 0xffff
 * when no sequence data is supplied. */
int xglSoundSendSmd2(void *smd, int bank);
extern void xglSoundSequenceStop2(int channel);
void xglSoundSequenceNormal2(int channel, int volume);
void xglSoundStreamMute(void);
void xglSoundStreamOpenVagStereoParam(int stream, const char *source, int pitch, int volume);
int xglSoundEffectCheckID(int soundEffectId);
void xglSoundSequenceNormal(int volume);
extern void xglSoundSequenceFadeOut2(int channel, int time);
void xglSoundStreamOpenVagMultiParam(int stream, const char *source, int pitch, int volume, int pan);

#endif /* INCLUDE_MAIN_XGL_SOUND_H */
