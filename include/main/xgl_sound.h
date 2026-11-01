#ifndef INCLUDE_MAIN_XGL_SOUND_H
#define INCLUDE_MAIN_XGL_SOUND_H

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

typedef struct SePacket {
    short command;
    unsigned char unmodeled_02[14];
    int arguments[4];
} SePacket;

extern int xglSoundStreamMain(void);
extern void xglSoundEffectNormalID(int sound_id, int variant);
void xglSoundEffectStopID(int soundEffectId, int source);
void xglSoundSendEffect(void *swd, void *sed, int bank);

#endif /* INCLUDE_MAIN_XGL_SOUND_H */
