#ifndef SSD_CHAN_H
#define SSD_CHAN_H

#include "ssd/ssd.h"
#include "ssd/ssd_lfo.h"
#include "ssd/ssd_voic.h"
#include "ssd/ssd_wave.h"

/* Range of a MIDI data byte, and the reference note of the sequencer. */
#define SSD_MIDI_VALUE_MAX 127
#define SSD_MIDI_PAN_CENTER 64

/* Controller value from which a switch controller (sustain, reverb) is on. */
#define SSD_MIDI_SWITCH_ON 64
#define SSD_NOTE_MIDDLE_C 60

/* Channel defaults set by SsdResetChannel and the sequence start. */
#define SSD_CHANNEL_VOLUME_DEFAULT 100
#define SSD_PANPOT_CENTER 63
#define SSD_PANPOT_CENTER_FIXED (SSD_PANPOT_CENTER << 16)
#define SSD_FIXED_VOLUME_DEFAULT (SSD_CHANNEL_VOLUME_DEFAULT << 16)
#define SSD_PITCH_BEND_SENSITIVITY_DEFAULT 2

/* SsdChannel.flags */
#define SSD_CHANNEL_SLUR 0x2
#define SSD_CHANNEL_SUSTAIN 0x4
#define SSD_CHANNEL_FIXED_PITCH 0x10
#define SSD_CHANNEL_RANDOM_KEY 0x20

/* SsdChannel.program_type: what the channel plays. */
#define SSD_PROGRAM_TYPE_WAVE 1
#define SSD_PROGRAM_TYPE_PROGRAM 2

/*
 * SsdChannel.allocation_mode: how a key-on picks its voice. Mono channels
 * reuse the voice they hold, fixed channels always take the first voice of
 * their range.
 */
#define SSD_ALLOCATION_SEARCH 0
#define SSD_ALLOCATION_MONO 1
#define SSD_ALLOCATION_FIXED 2

/* SsdMidiDevice.state: sequence skip in progress (voice events are not processed). */
#define SSD_DEVICE_SKIP 0x1

/* Release rate field of the second ADSR register, and the forced key-off rate. */
#define SSD_ADSR2_RELEASE_RATE_MASK 0x1f
#define SSD_FORCED_RELEASE_RATE 6

typedef struct SsdProgramSplit {
    unsigned char wave_index;
    unsigned char root_key;
    unsigned char lower_key;
    unsigned char upper_key;
    unsigned char volume;
    unsigned char panpot;
    unsigned char effects;
    unsigned char properties;
    unsigned short adsr1;
    unsigned short adsr2;
    unsigned char synthesis_parameters[4];
} SsdProgramSplit;

typedef struct SsdProgram {
    unsigned char properties[3];
    unsigned char split_count;
    unsigned short adsr1;
    unsigned short adsr2;
    SsdVoiceRange voice_range;
    unsigned char synthesis_properties[6];
    SsdLfoParam lfo[4];
    unsigned char unmodeled_lfo_tail[16];
    SsdProgramSplit splits[1];
} SsdProgram;

/* One key-on or key-off of a MIDI note. */
typedef struct SsdMidiEvent {
    unsigned char trigger;
    unsigned char unmodeled_01;
    unsigned char velocity;
    unsigned char key;
    unsigned int duration;
} SsdMidiEvent;

struct SsdChannel {
    unsigned int flags;
    unsigned char effects;
    unsigned char ordinal;
    unsigned char allocation_mode;
    unsigned char program_type;
    unsigned char priority;
    SsdVoiceRange voice_range;
    unsigned char random_note_low;
    unsigned char random_note_high;
    unsigned short bank;
    signed char program_index;
    unsigned char wave_index;
    int pitch_offset;
    int volume;
    int panpot;
    short fader_note;
    signed char fader_volume;
    signed char fader_panpot;
    short transpose;
    short pitch_bend;
    unsigned char articulation;
    signed char channel_volume;
    signed char expression;
    unsigned char unmodeled_2b;
    unsigned char nonregistered_parameter_lsb;
    unsigned char nonregistered_parameter_msb;
    unsigned char registered_parameter_lsb;
    unsigned char registered_parameter_msb;
    unsigned char unmodeled_30[2];
    unsigned char registered_parameter_selected;
    unsigned char pitch_bend_sensitivity;
    unsigned short adsr1;
    unsigned short adsr2;
    SsdLfoWork lfo;
    SsdVoice *voices;
    SsdWaveBank *bank_data;
    SsdWaveSample *wave_data;
    SsdProgram *program_data;
    SsdMidiDevice *device;
};

struct SsdMidiDevice {
    unsigned int state;
    unsigned char id;
    unsigned char channel_count;
    unsigned short reset_flags;
    unsigned char output_volume;
    unsigned char volume;
    int master_note;
    int master_volume;
    int master_panpot;
    SsdVoiceRange voice_range;
    SsdMidiDevice *next;
    SsdChannel channels[1];
};

/*
 * Functions and data used by other translation units. SsdSetMidiDevicePriority
 * is declared by its callers: ssd_seq.c passes it two more arguments than it takes.
 */
SsdMidiDevice *SsdNewMidiDevice(int channel_count, int allocation_mode,
                                const SsdVoiceRange *voice_range);
void SsdDisposeMidiDevice(SsdMidiDevice *device);
void SsdResumeDeviceAll(void);
void SsdSuspendDeviceAll(void);
void SsdResetMidiDeviceParam(SsdMidiDevice *device, int flags);
void SsdResetChannelAll(SsdMidiDevice *device, int flags);
void SsdResetChannel(SsdChannel *channel, int bank);
void SsdResetChannelFader(SsdMidiDevice *device, int propagate);
void SsdSetMidiDeviceNote(SsdMidiDevice *device, unsigned int note);
void SsdSetMidiDeviceVolume(SsdMidiDevice *device, int volume);
void SsdSetMidiDevicePanpot(SsdMidiDevice *device, int panpot);
void SsdSetMidiDeviceOutputVolume(SsdMidiDevice *device, int volume);
void SsdCutoffDevive(SsdMidiDevice *device);
void SsdCutoffChannel(SsdChannel *channel);
void SsdUnlinkChannelVoice(SsdVoice *voice);
void SsdSetChannelAllVoiceEvent(SsdMidiDevice *device, unsigned short events);
void SsdSetChannelVoiceEvent(SsdChannel *channel, unsigned short events);
void SsdSetChannelVoiceVelocity(SsdChannel *channel, int velocity);
void SsdSetChannelHWEffectOn(SsdChannel *channel, unsigned char effects,
                             unsigned short flags);
void SsdSetChannelHWEffectOff(SsdChannel *channel, unsigned char effects,
                              unsigned short flags);
void SsdKeyonChannel(SsdChannel *channel, SsdMidiEvent *event);
void SsdKeyonChannelVoice(SsdChannel *channel, SsdVoice *voice);
void SsdSendChannelKeyoffEvent(SsdChannel *channel);
void SsdKeyoffChannel(SsdChannel *channel, const SsdMidiEvent *event);
void SsdKeyoffChannelVoice(SsdChannel *channel, SsdVoice *voice);
void SsdSetChannelWave(SsdChannel *channel, unsigned char wave_index);
SsdProgram *SsdSetChannelProgram(SsdChannel *channel, unsigned char program_index);

#endif
