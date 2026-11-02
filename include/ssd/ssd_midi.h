#ifndef SSD_MIDI_H
#define SSD_MIDI_H

#include "ssd/ssd.h"

#define SSD_MIDI_EVENT_QUEUE_SIZE 64

/* MIDI status bytes (channel messages keep the channel in the low nibble). */
#define SSD_MIDI_NOTE_OFF 0x80
#define SSD_MIDI_NOTE_ON 0x90
#define SSD_MIDI_POLY_PRESSURE 0xa0
#define SSD_MIDI_CONTROL_CHANGE 0xb0
#define SSD_MIDI_PROGRAM_CHANGE 0xc0
#define SSD_MIDI_CHANNEL_PRESSURE 0xd0
#define SSD_MIDI_PITCH_BEND 0xe0
#define SSD_MIDI_SYSTEM 0xf0
#define SSD_MIDI_SYSTEM_EXCLUSIVE_END 0xf7
#define SSD_MIDI_SYSTEM_REALTIME 0xf8

/* Controller numbers of SsdMidiControlChangeTable. */
#define SSD_MIDI_CONTROLLER_COUNT 128

/* Pitch bend: 14-bit value centred on 8192. */
#define SSD_MIDI_PITCH_BEND_CENTER 8192

/* Registered parameter 0, the pitch bend sensitivity. */
#define SSD_MIDI_RPN_PITCH_BEND_SENSITIVITY 0

typedef struct SsdMidiEventQueue {
    unsigned short count;
    unsigned char unmodeled_02[2];
    unsigned int events[SSD_MIDI_EVENT_QUEUE_SIZE];
} SsdMidiEventQueue;

typedef struct SsdMidiState {
    unsigned short enabled;
    unsigned short queue_index;
    SsdMidiDevice *device;
    SsdMidiEventQueue queues[2];
} SsdMidiState;

/* Functions and data used by other translation units. */
extern SsdMidiState SsdMidi;

void SsdMidiEventDecode(SsdMidiDevice *device, unsigned int event);

#endif
