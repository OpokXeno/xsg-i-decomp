#ifndef SSD_VOIC_H
#define SSD_VOIC_H

#include "ssd/ssd.h"
#include "ssd/ssd_lfo.h"

#define SSD_CORE_COUNT 2
#define SSD_CORE_VOICE_COUNT 24
#define SSD_VOICE_COUNT 48
#define SSD_CORE_VOICE_MASK 0xffffff

/* SsdVoice.effects: per-voice SPU2 routing bits. */
#define SSD_VOICE_EFFECT_PITCH_MOD 0x1
#define SSD_VOICE_EFFECT_NOISE 0x2
#define SSD_VOICE_EFFECT_REVERB 0x4
#define SSD_VOICE_EFFECT_DRY 0x8

/* SsdVoice.flags: registers to rewrite at the next SPU2 update. */
#define SSD_VOICE_FLAG_PITCH 0x1
#define SSD_VOICE_FLAG_VOLUME 0x2
#define SSD_VOICE_FLAG_SAMPLE_ADDRESS 0x8
#define SSD_VOICE_FLAG_ADSR1 0x10
#define SSD_VOICE_FLAG_ADSR2 0x20
#define SSD_VOICE_FLAG_PITCH_MOD 0x1000
#define SSD_VOICE_FLAG_NOISE 0x2000
#define SSD_VOICE_FLAG_REVERB 0x4000
#define SSD_VOICE_FLAG_DRY 0x8000
#define SSD_VOICE_FLAG_EFFECTS 0xf000
#define SSD_VOICE_FLAG_ALL 0xffff

/* SsdVoice.events: pending requests from the sequencer to the voice. */
#define SSD_VOICE_EVENT_KEYON 0x1
#define SSD_VOICE_EVENT_KEYOFF 0x2
#define SSD_VOICE_EVENT_PITCH 0x10
#define SSD_VOICE_EVENT_VOLUME 0x20
#define SSD_VOICE_EVENT_PARAMETERS 0xfff0

/* Pitch register value that plays a sample at its recorded rate, and the ADSR words of raw (stream, sampling) voices. */
#define SSD_VOICE_PITCH_NORMAL 0x1000
#define SSD_VOICE_PITCH_MAX 0x3fff
#define SSD_VOICE_ADSR1_RAW 0x20ff
#define SSD_VOICE_ADSR2_RAW 0x5fc6

/*
 * Pan law of the 0..32256 stereo position (position 16128 = centre): the gain
 * of the far side falls from the unity gain to SSD_PAN_CENTER_GAIN at centre.
 */
#define SSD_PAN_RANGE 32256
#define SSD_PAN_CENTER 16128
#define SSD_PAN_UNITY_GAIN 32512
#define SSD_PAN_CENTER_GAIN 23040
#define SSD_PAN_ATTENUATION 9472

/* SsdVoice.key_state */
#define SSD_VOICE_KEY_RELEASED 0x2

/*
 * SsdVoice.allocation.priority: channel priority above bit 16, the voice is in
 * use while any of the low nine bits is set; the low byte survives a key-off.
 */
#define SSD_VOICE_PRIORITY_ACTIVE 0x1ff
#define SSD_VOICE_PRIORITY_RELEASE 0xff
#define SSD_VOICE_PRIORITY_KEYOFF_MASK 0xffff00ffU

typedef struct SsdVoiceRange {
    unsigned char first;
    unsigned char last;
} SsdVoiceRange;

/* One of the 48 SPU2 voices (24 per core). */
struct SsdVoice {
    unsigned char index;
    unsigned char core;
    unsigned char number;
    unsigned char state;
    unsigned short flags;
    unsigned char effects;
    unsigned char attributes;
    short left_volume;
    short right_volume;
    short reserved_volume[2];
    unsigned char envelope_state[4];
    unsigned short pitch;
    unsigned short modulation;
    union {
        unsigned int priority;
        unsigned char envx;
    } allocation;
    unsigned int sample_start;
    unsigned int loop_start;
    unsigned short adsr1;
    unsigned short adsr2;
    unsigned char sample_state[4];
    unsigned short key_state;
    unsigned short events;
    int user_id;
    unsigned int duration;
    short note;
    short velocity;
    unsigned char release_rate;
    signed char transpose;
    unsigned char key;
    unsigned char wave_index;
    short base_note;
    unsigned short base_velocity;
    short base_panpot;
    SsdLfoWork lfo;
    SsdVoice *next;
    SsdChannel *channel;
};

/* Functions and data used by other translation units. */
void SsdInitVoiceManager(void);
SsdVoice *SsdGetKeyonVoice(SsdChannel *channel, SsdVoiceRange *range);
void SsdKeyonVoice(SsdVoice *voice);
void SsdCutoffUnlinkVoice(SsdVoice *voice);
void SsdKeyoffVoice(SsdVoice *voice);
void SsdSetVoiceKeyon(void);
void SsdSetVoiceKeyoff(void);
int SsdNoteToPitch(int note);
void SsdCompellingKeyoff(void);
void SsdCalcDirectVoiceVolume(short output[2], int volume, int pan);

#endif
