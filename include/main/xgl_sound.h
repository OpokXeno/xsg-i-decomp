#ifndef INCLUDE_MAIN_XGL_SOUND_H
#define INCLUDE_MAIN_XGL_SOUND_H

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

#endif /* INCLUDE_MAIN_XGL_SOUND_H */
