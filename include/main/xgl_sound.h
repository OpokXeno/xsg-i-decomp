#ifndef INCLUDE_MAIN_XGL_SOUND_H
#define INCLUDE_MAIN_XGL_SOUND_H

#include "main/xgl_cd.h"

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
    SoundChannelEntry channels[8];
    /* xglSoundInitial resets exactly these 16 banks. */
    SoundEffectBankEntry effect_banks[16];
    SoundStream stream;
    int packet_count;
    /* The 0x800-byte SE queue: 64 fixed-size packets. */
    SePacket packets[64];
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
