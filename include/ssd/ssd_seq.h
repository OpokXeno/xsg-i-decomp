#ifndef SSD_SEQ_H
#define SSD_SEQ_H

#include "ssd/ssd.h"
#include "ssd/ssd_main.h"
#include "ssd/ssd_musc.h"

/* Sequencer timing: interrupts per second, and ticks of a whole note. */
#define SSD_TICKS_PER_SECOND 500
#define SSD_TICKS_PER_WHOLE_NOTE 384
#define SSD_SMPTE_FRAMES_PER_SECOND 30

/* Tempo scale (tempo value to clock step), default tempo and master tempo speeds. */
#define SSD_TEMPO_CLOCK_SCALE 53687
#define SSD_TEMPO_DEFAULT 120
#define SSD_MASTER_TEMPO_NORMAL 256
#define SSD_MASTER_TEMPO_SKIP 16384

/* SsdSequence.options: cores that receive the sequence reverb; the core number
   sits in bits 8 and up of the SsdSetReverbParameter mode. */
#define SSD_SEQUENCE_REVERB_CORE0 0x1
#define SSD_SEQUENCE_REVERB_CORE1 0x2
#define SSD_REVERB_MODE_CORE1 256

/* SsdSequence.flags */
#define SSD_SEQUENCE_STOPPED 0x100
#define SSD_SEQUENCE_STOP_PENDING 0x200
#define SSD_SEQUENCE_PLAYING 0x8000

/* Result bits of SsdGetSeqPlayStatus. */
#define SSD_SEQ_STATUS_PLAYING 0x1
#define SSD_SEQ_STATUS_STOPPED 0x2
#define SSD_SEQ_STATUS_TEMPO_RAMP 0x10000
#define SSD_SEQ_STATUS_NOTE_RAMP 0x20000
#define SSD_SEQ_STATUS_VOLUME_RAMP 0x40000
#define SSD_SEQ_STATUS_PAN_RAMP 0x80000

/* SsdTrack.flags */
#define SSD_TRACK_MUTED 0x2
#define SSD_TRACK_NOTE 0x100
#define SSD_TRACK_SLURRED 0x200
#define SSD_TRACK_REST 0x400
#define SSD_TRACK_SLUR_MODE 0x800
#define SSD_TRACK_ACTIVE 0x8000

/* SsdTrack.control_flags: running ramps. */
#define SSD_TRACK_CONTROL_PITCH_RAMP 0x1
#define SSD_TRACK_CONTROL_VOLUME_RAMP 0x8
#define SSD_TRACK_CONTROL_PAN_RAMP 0x10
#define SSD_TRACK_CONTROL_FADE_OUT 0x20

/* Track start values. */
#define SSD_TRACK_GATE_RATIO_DEFAULT 15
#define SSD_TRACK_VELOCITY_DEFAULT (SSD_MIDI_VALUE_MAX << 16)
#define SSD_TRACK_MUSIC_VELOCITY_DEFAULT (96 << 16)
#define SSD_TRACK_LOOP_NONE 255

typedef void (*SsdSequenceStopCallback)();

typedef struct SsdTrackLoop {
    unsigned char remaining;
    unsigned char unmodeled_01;
    unsigned char start_octave;
    unsigned char escape_octave;
    unsigned char *start;
    unsigned char *escape;
} SsdTrackLoop;

/* One music or effect track of a sequence. */
struct SsdTrack {
    short flags;
    unsigned short control_flags;
    unsigned char index;
    unsigned char midi_channel;
    unsigned char voice_index;
    unsigned char attributes;
    unsigned char priority;
    unsigned char loop_depth;
    unsigned short bank_id;
    unsigned int effect_id;
    int mode;
    unsigned int timestamp;
    unsigned char *begin;
    unsigned char *stream;
    unsigned char *jump;
    SsdChannel *channel;
    int gate;
    unsigned short gate_ticks;
    unsigned char gate_state;
    unsigned char repeat_octave;
    int wait;
    int delta;
    unsigned int duration;
    signed char gate_ratio;
    unsigned char octave;
    unsigned char note;
    unsigned char last_note;
    SsdRamp velocity;
    SsdRamp pitch;
    SsdRamp volume;
    SsdRamp pan;
    SsdTrackLoop loops[4];
};

/* A playing music or effect sequence. */
struct SsdSequence {
    unsigned int signature;
    SsdSequence *next;
    SsdMusicData *data;
    SsdTrack *tracks;
    SsdMusicChunk *patch_chunk;
    SsdMidiDevice *device;
    char *metadata;
    int sequence_id;
    short flags;
    short active_tracks;
    unsigned short bank;
    unsigned char track_count;
    unsigned char signal;
    unsigned char noise_clock;
    unsigned char priority;
    unsigned char mode;
    unsigned char options;
    unsigned char output_volume;
    unsigned char is_effect;
    signed char note;
    signed char pan;
    SsdFixed16 tempo;
    int tempo_increment;
    int tempo_target;
    short tempo_duration;
    unsigned char unmodeled_3e[2];
    int clock;
    int clock_step;
    int interrupts;
    unsigned int play_ticks;
    unsigned int play_time;
    int random_state;
    short unmodeled_58;
    unsigned short measure;
    unsigned short beat;
    unsigned short beat_tick;
    unsigned short beats_per_measure;
    unsigned short ticks_per_beat;
    unsigned short time_denominator;
    unsigned short time_numerator;
    unsigned char timing_mode;
    unsigned char unmodeled_69[2];
    signed char reverb_mode;
    short reverb_depth;
    unsigned short reverb_target_depth;
    signed char reverb_delay;
    signed char reverb_feedback;
    unsigned char unmodeled_72[10];
    SsdRamp master_tempo;
    SsdRamp master_note;
    SsdRamp master_volume;
    SsdRamp master_pan;
    SsdSequenceStopCallback stop_callback;
    unsigned int unmodeled_c0;
};

typedef struct SsdTimeCode {
    unsigned char unmodeled_00[2];
    unsigned short hours;
    unsigned char minutes;
    unsigned char seconds;
    unsigned char frames;
    unsigned char subframes;
    unsigned int play_ticks;
    unsigned short ticks;
} SsdTimeCode;

/* Fader preset of one track. */
typedef struct SsdTrackPatch {
    unsigned char unmodeled_00[2];
    unsigned char track;
    unsigned char unmodeled_03;
    unsigned short note;
    unsigned char volume;
    unsigned char panpot;
} SsdTrackPatch;

typedef struct SsdTrackPatchSet {
    unsigned char unmodeled_00[2];
    unsigned char count;
    unsigned char unmodeled_03;
} SsdTrackPatchSet;

/* Functions and data used by other translation units. */
void SsdInitSequence(SsdSequence *sequence);
void SsdLinkSequence(SsdSequence *sequence);
int SsdUnlinkSequence(SsdSequence *sequence);
void SsdDisposeSequenceBuffer(SsdSequence *sequence);
int SsdGetSeqPlayStatus(int sequence_id);
void SsdSetSeqMasterTempo(int id, int bpm, int ticks);
void SsdSetSeqMasterVolume(int id, int volume, int ticks);
void SsdSetSeqMasterNote(int sequence_id, int note, int duration);
void SsdSetSeqMasterPanpot(int sequence_id, int pan, int duration);
void SsdSetSeqMute(int sequence_id, int mute_mask);
void SsdSetSeqVolumeFader(int sequence_id, int track_index, int volume);
void SsdSetSeqPatchNumber(int sequence_id, int patch_number);
void SsdSetTrackPatchSet(int id, const SsdTrackPatchSet *patches);
void SsdSetSeqSignal(int sequence_id, int signal);
void SsdSetSeqReverb(SsdSequence *sequence);
SsdSequence *SsdGetSequencePtr(int sequence_id);
void SsdStoppingSequence(SsdSequence *sequence);

#endif
