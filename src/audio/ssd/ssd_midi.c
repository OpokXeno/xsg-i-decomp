#include "ssd/ssd_midi.h"
#include "ssd/ssd_chan.h"

typedef void (*SsdMidiController)(SsdChannel *channel, int value);

SsdMidiState SsdMidi;

extern SsdMidiController SsdMidiControlChangeTable[];

SsdMidiDevice *SsdMakeMidiSequenceRec(int channel);
void SsdInMidiEvent(unsigned int event);
void SsdMidiControlBankMSB(SsdChannel *channel, int value);
void SsdMidiControlBankLSB(SsdChannel *channel, int value);
void SsdMidiControlModulationMSB(SsdChannel *channel, int value);
void SsdMidiControlPortamentoMSB(SsdChannel *channel, int value);
void SsdMidiControlVolumeMSB(SsdChannel *channel, int value);
void SsdMidiControlPanpotMSB(SsdChannel *channel, int value);
void SsdMidiControlExpressionMSB(SsdChannel *channel, int value);
void SsdMidiControlSustaion(SsdChannel *channel, int value);
void SsdMidiControlReverb(SsdChannel *channel, int value);
void SsdMidiControlRegParLSB(SsdChannel *channel, int value);
void SsdMidiControlRegParMSB(SsdChannel *channel, int value);
void SsdMidiControlNonRegParLSB(SsdChannel *channel, int value);
void SsdMidiControlNonRegParMSB(SsdChannel *channel, int value);
void SsdMidiControlSetRegParDataMSB(SsdChannel *channel, int value);
void SsdMidiControlSetRegParDataLSB(SsdChannel *channel, int value);

SsdMidiDevice *SsdMakeMidiSequenceRec(int channel)
{
    SsdMidiDevice *device;
    int queue;

    SsdMidi.enabled = 1;
    for (queue = 1; queue >= 0; queue--)
        SsdMidi.queues[queue].count = 0;
    SsdMidi.queue_index = 0;
    device = SsdNewMidiDevice(channel, 0, 0);
    SsdMidi.device = device;
    SsdResetMidiDeviceParam(device, SsdMidi.queue_index);
    SsdResetChannelAll(device, SsdMidi.queue_index);
    printf("Ssd init midi device  DeviceHdl=0x%08x Channel=%d\n", device, channel);
    return device;
}

void SsdInMidiEvent(unsigned int event)
{
    SsdMidiEventQueue *queue = &SsdMidi.queues[0];
    int count;

    CpuDisableIntr();
    count = queue->count;
    if (count < SSD_MIDI_EVENT_QUEUE_SIZE) {
        queue->events[count] = event;
        queue->count = count + 1;
    }
    CpuEnableIntr();
}

void SsdMidiEventDecode(SsdMidiDevice *device, unsigned int event)
{
    int channel_index = event & 0xff;
    unsigned int message = event >> 8;
    int status = message & 0xff;
    int data1 = (event >> 16) & 0xff;
    int data2 = event >> 24;
    SsdChannel *channel;
    SsdMidiEvent note;
    SsdMidiController controller;
    int pitch;

    channel_index = (channel_index << 4) + (message & 15);
    if (status < SSD_MIDI_SYSTEM)
        status = message & 0xf0;

    if (status == SSD_MIDI_CHANNEL_PRESSURE)
        return;
    if (status < SSD_MIDI_CHANNEL_PRESSURE + 1) {
        if (status == SSD_MIDI_POLY_PRESSURE)
            return;
        if (status < SSD_MIDI_POLY_PRESSURE + 1) {
            if (status == SSD_MIDI_NOTE_OFF)
                goto note_off;
            if (status == SSD_MIDI_NOTE_ON)
                goto note_on;
            return;
        }
        if (status == SSD_MIDI_CONTROL_CHANGE)
            goto control_change;
        if (status == SSD_MIDI_PROGRAM_CHANGE)
            goto program_change;
        return;
    }
    if (status == SSD_MIDI_SYSTEM_EXCLUSIVE_END)
        return;
    if (status < SSD_MIDI_SYSTEM_REALTIME) {
        if (status == SSD_MIDI_PITCH_BEND)
            goto pitch_bend;
    }
    return;

note_off:
    channel = &device->channels[channel_index];
    note.velocity = data2;
    note.key = data1;
    SsdKeyoffChannel(channel, &note);
    return;
note_on:
    channel = &device->channels[channel_index];
    note.velocity = data2;
    note.key = data1;
    note.duration = -1;
    if (data2)
        SsdKeyonChannel(channel, &note);
    else
        SsdKeyoffChannel(channel, &note);
    return;
control_change:
    channel = &device->channels[channel_index];
    controller = SsdMidiControlChangeTable[data1];
    if (controller)
        controller(channel, data2);
    return;
program_change:
    SsdSetChannelProgram(&device->channels[channel_index], data1);
    return;
pitch_bend:
    channel = &device->channels[channel_index];
    pitch = ((data1 +(data2 << 7) - SSD_MIDI_PITCH_BEND_CENTER) *
             channel->pitch_bend_sensitivity) / 32;
    channel->pitch_offset = pitch << 16;
    SsdSetChannelVoiceEvent(channel, SSD_VOICE_EVENT_PITCH);
    return;
}

void SsdMidiControlBankMSB(SsdChannel *channel, int value)
{
    channel->bank = (channel->bank & 0xff00) | value;
}

void SsdMidiControlBankLSB(SsdChannel *channel, int value)
{
    channel->bank = (channel->bank & 0xff) | (value << 8);
}

void SsdMidiControlModulationMSB(SsdChannel *channel, int value)
{
}

void SsdMidiControlPortamentoMSB(SsdChannel *channel, int value)
{
}

void SsdMidiControlVolumeMSB(SsdChannel *channel, int value)
{
    channel->volume =
        value * channel->expression * channel->fader_volume /
        (SSD_MIDI_VALUE_MAX * SSD_MIDI_VALUE_MAX);
    channel->channel_volume = value;
    SsdSetChannelVoiceEvent(channel, SSD_VOICE_EVENT_VOLUME);
}

void SsdMidiControlPanpotMSB(SsdChannel *channel, int value)
{
    int panpot = value - 1;
    if (panpot < 0)
        panpot = 0;
    channel->panpot = panpot << 16;
    SsdSetChannelVoiceEvent(channel, SSD_VOICE_EVENT_VOLUME);
}

void SsdMidiControlExpressionMSB(SsdChannel *channel, int value)
{
    channel->volume =
        channel->channel_volume * value * channel->fader_volume /
        (SSD_MIDI_VALUE_MAX * SSD_MIDI_VALUE_MAX);
    channel->expression = value;
    SsdSetChannelVoiceEvent(channel, SSD_VOICE_EVENT_VOLUME);
}

void SsdMidiControlSustaion(SsdChannel *channel, int value)
{
    if (value >= SSD_MIDI_SWITCH_ON) {
        channel->flags |= SSD_CHANNEL_SUSTAIN;
    } else {
        channel->flags &= ~SSD_CHANNEL_SUSTAIN;
        SsdSendChannelKeyoffEvent(channel);
    }
}

void SsdMidiControlReverb(SsdChannel *channel, int value)
{
    if (value >= SSD_MIDI_SWITCH_ON)
        SsdSetChannelHWEffectOn(channel, SSD_VOICE_EFFECT_REVERB, SSD_VOICE_FLAG_REVERB);
    else
        SsdSetChannelHWEffectOff(channel, SSD_VOICE_EFFECT_REVERB, SSD_VOICE_FLAG_REVERB);
}

void SsdMidiControlRegParLSB(SsdChannel *channel, int value)
{
    channel->registered_parameter_lsb = value;
    channel->registered_parameter_selected = 1;
}

void SsdMidiControlRegParMSB(SsdChannel *channel, int value)
{
    channel->registered_parameter_msb = value;
    channel->registered_parameter_selected = 1;
}

void SsdMidiControlNonRegParLSB(SsdChannel *channel, int value)
{
    channel->nonregistered_parameter_lsb = value;
    channel->registered_parameter_selected = 0;
}

void SsdMidiControlNonRegParMSB(SsdChannel *channel, int value)
{
    channel->nonregistered_parameter_msb = value;
    channel->registered_parameter_selected = 0;
}

void SsdMidiControlSetRegParDataMSB(SsdChannel *channel, int value)
{
    if (channel->registered_parameter_selected &&
        ((channel->registered_parameter_msb << 8) +
         channel->registered_parameter_lsb) == SSD_MIDI_RPN_PITCH_BEND_SENSITIVITY)
        channel->pitch_bend_sensitivity = value;
}

void SsdMidiControlSetRegParDataLSB(SsdChannel *channel, int value)
{
}

/* MIDI control change handlers by controller number; 0 = ignored. */
SsdMidiController SsdMidiControlChangeTable[SSD_MIDI_CONTROLLER_COUNT] = {
    /* 0x00 bank select MSB */ SsdMidiControlBankMSB,
    /* 0x01 modulation wheel */ SsdMidiControlModulationMSB,
    /* 0x02 */ 0,
    /* 0x03 */ 0,
    /* 0x04 */ 0,
    /* 0x05 portamento time */ SsdMidiControlPortamentoMSB,
    /* 0x06 data entry MSB */ SsdMidiControlSetRegParDataMSB,
    /* 0x07 channel volume */ SsdMidiControlVolumeMSB,
    /* 0x08 */ 0,
    /* 0x09 */ 0,
    /* 0x0a pan */ SsdMidiControlPanpotMSB,
    /* 0x0b expression */ SsdMidiControlExpressionMSB,
    /* 0x0c */ 0,
    /* 0x0d */ 0,
    /* 0x0e */ 0,
    /* 0x0f */ 0,
    /* 0x10 */ 0,
    /* 0x11 */ 0,
    /* 0x12 */ 0,
    /* 0x13 */ 0,
    /* 0x14 */ 0,
    /* 0x15 */ 0,
    /* 0x16 */ 0,
    /* 0x17 */ 0,
    /* 0x18 */ 0,
    /* 0x19 */ 0,
    /* 0x1a */ 0,
    /* 0x1b */ 0,
    /* 0x1c */ 0,
    /* 0x1d */ 0,
    /* 0x1e */ 0,
    /* 0x1f */ 0,
    /* 0x20 bank select LSB */ SsdMidiControlBankLSB,
    /* 0x21 */ 0,
    /* 0x22 */ 0,
    /* 0x23 */ 0,
    /* 0x24 */ 0,
    /* 0x25 */ 0,
    /* 0x26 */ 0,
    /* 0x27 */ 0,
    /* 0x28 */ 0,
    /* 0x29 */ 0,
    /* 0x2a */ 0,
    /* 0x2b */ 0,
    /* 0x2c */ 0,
    /* 0x2d */ 0,
    /* 0x2e */ 0,
    /* 0x2f */ 0,
    /* 0x30 */ 0,
    /* 0x31 */ 0,
    /* 0x32 */ 0,
    /* 0x33 */ 0,
    /* 0x34 */ 0,
    /* 0x35 */ 0,
    /* 0x36 */ 0,
    /* 0x37 */ 0,
    /* 0x38 */ 0,
    /* 0x39 */ 0,
    /* 0x3a */ 0,
    /* 0x3b */ 0,
    /* 0x3c */ 0,
    /* 0x3d */ 0,
    /* 0x3e */ 0,
    /* 0x3f */ 0,
    /* 0x40 damper pedal (sustain) */ SsdMidiControlSustaion,
    /* 0x41 */ 0,
    /* 0x42 */ 0,
    /* 0x43 */ 0,
    /* 0x44 */ 0,
    /* 0x45 */ 0,
    /* 0x46 */ 0,
    /* 0x47 */ 0,
    /* 0x48 */ 0,
    /* 0x49 */ 0,
    /* 0x4a */ 0,
    /* 0x4b */ 0,
    /* 0x4c */ 0,
    /* 0x4d */ 0,
    /* 0x4e */ 0,
    /* 0x4f */ 0,
    /* 0x50 */ 0,
    /* 0x51 */ 0,
    /* 0x52 */ 0,
    /* 0x53 */ 0,
    /* 0x54 */ 0,
    /* 0x55 */ 0,
    /* 0x56 */ 0,
    /* 0x57 */ 0,
    /* 0x58 */ 0,
    /* 0x59 */ 0,
    /* 0x5a */ 0,
    /* 0x5b effects 1 depth (reverb) */ SsdMidiControlReverb,
    /* 0x5c */ 0,
    /* 0x5d */ 0,
    /* 0x5e */ 0,
    /* 0x5f */ 0,
    /* 0x60 */ 0,
    /* 0x61 */ 0,
    /* 0x62 NRPN LSB */ SsdMidiControlNonRegParLSB,
    /* 0x63 NRPN MSB */ SsdMidiControlNonRegParMSB,
    /* 0x64 RPN LSB */ SsdMidiControlRegParLSB,
    /* 0x65 RPN MSB (the original table also points it at the LSB handler) */ SsdMidiControlRegParLSB,
    /* 0x66 */ 0,
    /* 0x67 */ 0,
    /* 0x68 */ 0,
    /* 0x69 */ 0,
    /* 0x6a */ 0,
    /* 0x6b */ 0,
    /* 0x6c */ 0,
    /* 0x6d */ 0,
    /* 0x6e */ 0,
    /* 0x6f */ 0,
    /* 0x70 */ 0,
    /* 0x71 */ 0,
    /* 0x72 */ 0,
    /* 0x73 */ 0,
    /* 0x74 */ 0,
    /* 0x75 */ 0,
    /* 0x76 */ 0,
    /* 0x77 */ 0,
    /* 0x78 */ 0,
    /* 0x79 */ 0,
    /* 0x7a */ 0,
    /* 0x7b */ 0,
    /* 0x7c */ 0,
    /* 0x7d */ 0,
    /* 0x7e */ 0,
    /* 0x7f */ 0,
};
