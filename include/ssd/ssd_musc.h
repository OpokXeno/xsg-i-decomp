#ifndef SSD_MUSC_H
#define SSD_MUSC_H

#include "ssd/ssd.h"
#include "ssd/ssd_voic.h"

/* File signatures ("smdm" music data) and the signature of the sequence record ("mseq"). */
#define SSD_MUSIC_DATA_SIGNATURE 0x6d646d73
#define SSD_MUSIC_SEQUENCE_SIGNATURE 0x7165736d

/* SsdMusicChunk.type */
#define SSD_CHUNK_END 0
#define SSD_CHUNK_METADATA 2
#define SSD_CHUNK_TRACK 3
#define SSD_CHUNK_PATCH 4

/* One chunk of a music or patch file: type 3 holds a track, type 4 the patch table. */
typedef struct SsdMusicChunk {
    unsigned short type;
    unsigned short size;
    union {
        struct {
            unsigned char parameters[2];
            unsigned char midi_channel;
            unsigned char attributes;
            unsigned char events[0];
        } track;
        struct {
            unsigned char unmodeled_04[2];
            unsigned char patch_count;
            unsigned char unmodeled_07;
            unsigned short offsets[0];
        } patch;
    } body;
} SsdMusicChunk;

/* Header of a "smdm" music file. */
typedef struct SsdMusicData {
    unsigned int signature;
    unsigned char unmodeled_04[4];
    int size;
    unsigned char unmodeled_0c[2];
    unsigned short file_id;
    unsigned char unmodeled_10[8];
    unsigned char priority;
    unsigned char options;
    unsigned char unmodeled_1a[2];
    unsigned short bank;
    unsigned char track_count;
    unsigned char channel_priority;
    unsigned char unmodeled_20;
    union {
        SsdVoiceRange voice_range;
        struct {
            signed char note;
            signed char pan;
        } master;
    } settings;
    unsigned char reverb_mode;
    unsigned char reverb_depth;
    signed char reverb_parameters[2];
    unsigned char midi_channel_count;
    SsdMusicChunk chunks[0];
} SsdMusicData;

/* Functions and data used by other translation units. */
int SsdPlaySequence(void *data, int volume, int fade);
int SsdAddSeqEffectData(SsdMusicData *data);
int SsdAddSequenceData(SsdMusicData *data);
int SsdDisposeSequence(int sequence_id);
int SsdCheckSequence(int sequence_id);
void SsdStartSequence(int sequence_id, int volume, int fade);
void SsdResumeSequence(int sequence_id, int volume, int fade);
void SsdStopSequence(int sequence_id);
void SsdPauseSequence(int sequence_id, int fade);

#endif
