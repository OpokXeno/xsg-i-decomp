#ifndef SSD_SEC_H
#define SSD_SEC_H

#include "ssd/ssd.h"

/* "sdes" effect file signature and "sseq" signature of the effect sequence record. */
#define SSD_EFFECT_DATA_SIGNATURE 0x73646573
#define SSD_EFFECT_SEQUENCE_SIGNATURE 0x71657373

/* Sequence id of the effect sequence. */
#define SSD_EFFECT_SEQUENCE_ID 0xf000

/* SsdEffectPlayParam.flags */
#define SSD_EFFECT_FLAG_STOP_ANY_MODE 0x4
#define SSD_EFFECT_FLAG_LOW_TRACKS 0x20

/* Tracks below this index serve the effects played with SSD_EFFECT_FLAG_LOW_TRACKS. */
#define SSD_EFFECT_LOW_TRACK_COUNT 24

/* Header of a "sdes" effect file followed by its effect offset table. */
struct SsdEffectData {
    unsigned int magic;
    unsigned char unmodeled_04[4];
    unsigned int size;
    unsigned char unmodeled_0c[2];
    unsigned short file_id;
    unsigned char unmodeled_10[10];
    unsigned short effect_count;
    unsigned short bank_id;
    unsigned char unmodeled_1e[18];
    SsdEffectData *next;
    unsigned short effect_offsets[0];
};

typedef struct SsdEffectEntry {
    unsigned char flags;
    unsigned char volume;
    unsigned char priority;
    unsigned char track_count;
    unsigned char unmodeled_04[4];
    unsigned short track_offsets[0];
} SsdEffectEntry;

typedef struct SsdEffectPlayParam {
    unsigned int effect_id;
    int mode;
    unsigned short bank_id;
    unsigned char start_track;
    unsigned char track_count;
    unsigned char first_track;
    unsigned char priority;
    unsigned char volume;
    unsigned char pan;
    unsigned char flags;
    SsdEffectEntry *effect_data;
} SsdEffectPlayParam;

/* Functions and data used by other translation units. */
SsdSequence *SsdMakeEffectSequenceRec(int track_count, int first_voice);
int SsdPlaySeqEffectNormal(SsdEffectPlayParam *play);
int SsdSaechSeqFreeTrack(SsdEffectPlayParam *play);
void SsdStopEffect(int effect, int mode);
int SsdAddEffectData(SsdEffectData *effect);
void SsdDisposeEffectData(int file_id);
int SsdCheckEffectData(int file_id);
void SsdPlayEffectNormal(unsigned int effect, int mode);
void SsdPlayEffectParam(unsigned int effect_id, int volume, int pan, int mode);
int SsdSetEffectParam(unsigned int effect_id, int volume, int pan, int mode);
void SsdSetPlayEffectParam(unsigned int effect, int volume, int pan, int mode);
void SsdStopAllEffect(void);
void SsdFadeoutEffect(unsigned int effect_id, int duration, int mode);
void SsdFadeoutEffectFileID(int file_id, int duration);
void SsdContinueOneEffect(unsigned int effect_id, int duration, int mode);
int SsdCheckPlayEffectAll(void);
int SsdCheckPlayEffect(unsigned int effect_id, int mode);
int SsdCheckPlayEffectFileID(int file_id);
void SsdPlayWaveNormal(unsigned int wave, int mode);
void SsdPlayWaveParam(unsigned int wave, int volume, int pan, int mode);
void SsdStopWave(int effect, int mode);

#endif
