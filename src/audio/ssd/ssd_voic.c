#include "ssd/ssd_voic.h"
#include "ssd/ssd_chan.h"
#include "ssd/ssd_data.h"
#include "ssd/ssd_dev.h"
#include "ssd/ssd_init.h"

void SsdKeyoffUnlinkVoice(SsdVoice *voice);
void SsdCutoffVoice(SsdVoice *voice);

void SsdInitVoiceManager(void)
{
    int i;
    int core;
    int index;
    for (i = 0; i < SSD_CORE_COUNT; ++i) {
        sceSdSetSwitch(i | SD_S_KOFF, SSD_CORE_VOICE_MASK);
        SsdWork.cores.switches.key_on[i] = 0;
        SsdWork.cores.switches.key_off[i] = 0;
        SsdWork.cores.switches.cutoff[i] = 0;
        SsdWork.cores.switches.pitch_modulation[i] = 0;
        SsdWork.cores.switches.noise[i] = 0;
        SsdWork.cores.switches.reverb_mix[i] = 0;
        SsdWork.cores.switches.dry_mix[i] = 0;
    }
    SsdWork.cores.switches.active_voices = 0;
    index = 0;
    for (core = 0; core < SSD_CORE_COUNT; ++core) {
        for (i = 0; i < SSD_CORE_VOICE_COUNT; ++i) {
            SsdWork.voices[index].index = index;
            SsdWork.voices[index].number = i;
            SsdWork.voices[index].core = core;
            SsdWork.voices[index].state = 0;
            SsdWork.voices[index].allocation.priority = 0;
            ++index;
        }
    }
}

SsdVoice *SsdGetKeyonVoice(SsdChannel *channel, SsdVoiceRange *range)
{
    SsdVoice *voice;
    int i;
    unsigned int priority;
    int selected;
    switch (channel->allocation_mode) {
    case 2:
        voice = channel->voices;
        if (voice != 0) return voice;
        voice = &SsdWork.voices[range->first];
        priority = (channel->priority << 16) + SSD_VOICE_PRIORITY_ACTIVE;
        if (voice->allocation.priority < priority)
            SsdUnlinkChannelVoice(voice);
        else
            voice = 0;
        return voice;
    case 1:
        voice = channel->voices;
        if (voice != 0) return voice;
        break;
    case 0:
    default:
        break;
    }
    i = range->first;
    priority = (channel->priority << 16) + SSD_VOICE_PRIORITY_ACTIVE;
    selected = range->last;
    for (; i <= range->last; ++i) {
        unsigned int candidate = SsdWork.voices[i].allocation.priority;
        if (candidate < priority) {
            selected = i;
            if (candidate == 0) break;
            priority = candidate;
        }
    }
    i = selected;
    if (selected >= SSD_VOICE_COUNT) {
        voice = 0;
    } else {
        voice = &SsdWork.voices[i];
        SsdUnlinkChannelVoice(voice);
    }
    return voice;
}

void SsdKeyonVoice(SsdVoice *voice)
{
    int core;
    unsigned int mask;
    voice->allocation.priority |= SSD_VOICE_PRIORITY_ACTIVE;
    voice->flags = SSD_VOICE_FLAG_ALL;
    core = voice->core;
    mask = 1 << voice->number;
    SsdWork.cores.switches.key_off[core] &= ~mask;
    SsdWork.cores.switches.cutoff[core] &= ~mask;
    SsdWork.cores.switches.key_on[core] |= mask;
}

void SsdKeyoffUnlinkVoice(SsdVoice *voice)
{
    unsigned int mask;
    int core;
    voice->allocation.priority = (unsigned char)voice->allocation.priority;
    mask = 1 << voice->number;
    core = voice->core;
    SsdWork.cores.switches.key_on[core] &= ~mask;
    SsdWork.cores.switches.key_off[core] |= mask;
}

void SsdCutoffUnlinkVoice(SsdVoice *voice)
{
    unsigned int mask;
    int core;
    voice->allocation.priority = (unsigned char)voice->allocation.priority;
    mask = 1 << voice->number;
    core = voice->core;
    SsdWork.cores.switches.key_on[core] &= ~mask;
    SsdWork.cores.switches.cutoff[core] |= mask;
}

void SsdCutoffVoice(SsdVoice *voice)
{
    unsigned int mask;
    int core;
    voice->allocation.priority &= SSD_VOICE_PRIORITY_KEYOFF_MASK;
    mask = 1 << voice->number;
    core = voice->core;
    SsdWork.cores.switches.key_on[core] &= ~mask;
    SsdWork.cores.switches.cutoff[core] |= mask;
}

void SsdKeyoffVoice(SsdVoice *voice)
{
    unsigned int mask;
    int core;
    voice->allocation.priority &= SSD_VOICE_PRIORITY_KEYOFF_MASK;
    mask = 1 << voice->number;
    core = voice->core;
    SsdWork.cores.switches.key_on[core] &= ~mask;
    SsdWork.cores.switches.key_off[core] |= mask;
}

void SsdSetVoiceKeyon(void)
{
    unsigned short changes[2];
    SsdVoice *voice = SsdWork.voices;
    int i;
    int core;
    int hardware_voice;
    unsigned short dirty;
    unsigned int mask;
    changes[1] = 0;
    changes[0] = 0;
    for (i = 0; i < SSD_VOICE_COUNT; ++i) {
        if (voice->allocation.priority & SSD_VOICE_PRIORITY_ACTIVE) {
            dirty = voice->flags;
            if (dirty != 0) {
                core = voice->core;
                hardware_voice = voice->number << 1;
                if (dirty & SSD_VOICE_FLAG_VOLUME) {
                    sceSdSetParam(core | hardware_voice | SD_VP_VOLL, voice->left_volume);
                    sceSdSetParam(core | hardware_voice | SD_VP_VOLR, voice->right_volume);
                }
                if (dirty & SSD_VOICE_FLAG_PITCH)
                    sceSdSetParam(core | hardware_voice | SD_VP_PITCH, voice->pitch);
                if (dirty & SSD_VOICE_FLAG_SAMPLE_ADDRESS)
                    sceSdSetAddr(core | hardware_voice | SD_VA_SSA, voice->sample_start);
                if (dirty & SSD_VOICE_FLAG_ADSR1)
                    sceSdSetParam(core | hardware_voice | SD_VP_ADSR1, voice->adsr1);
                if (dirty & SSD_VOICE_FLAG_ADSR2)
                    sceSdSetParam(core | hardware_voice | SD_VP_ADSR2, voice->adsr2);
                if (dirty & SSD_VOICE_FLAG_EFFECTS) {
                    mask = 1 << voice->number;
                    if (dirty & SSD_VOICE_FLAG_PITCH_MOD) {
                        changes[core] |= SSD_VOICE_FLAG_PITCH_MOD;
                        if (voice->effects & SSD_VOICE_EFFECT_PITCH_MOD)
                            SsdWork.cores.switches.pitch_modulation[core] |= mask;
                        else
                            SsdWork.cores.switches.pitch_modulation[core] &= ~mask;
                    }
                    if (dirty & SSD_VOICE_FLAG_NOISE) {
                        changes[core] |= SSD_VOICE_FLAG_NOISE;
                        if (voice->effects & SSD_VOICE_EFFECT_NOISE)
                            SsdWork.cores.switches.noise[core] |= mask;
                        else
                            SsdWork.cores.switches.noise[core] &= ~mask;
                    }
                    if (dirty & SSD_VOICE_FLAG_REVERB) {
                        changes[core] |= SSD_VOICE_FLAG_REVERB;
                        if (voice->effects & SSD_VOICE_EFFECT_REVERB)
                            SsdWork.cores.switches.reverb_mix[core] |= mask;
                        else
                            SsdWork.cores.switches.reverb_mix[core] &= ~mask;
                    }
                    if (dirty & SSD_VOICE_FLAG_DRY) {
                        changes[core] |= SSD_VOICE_FLAG_DRY;
                        if (voice->effects & SSD_VOICE_EFFECT_DRY)
                            SsdWork.cores.switches.dry_mix[core] |= mask;
                        else
                            SsdWork.cores.switches.dry_mix[core] &= ~mask;
                    }
                }
                voice->flags = 0;
            }
        }
        ++voice;
    }
    for (core = 0; core < SSD_CORE_COUNT; ++core) {
        if (changes[core] & SSD_VOICE_FLAG_EFFECTS) {
            if (changes[core] & SSD_VOICE_FLAG_PITCH_MOD)
                sceSdSetSwitch(core | SD_S_PMON, SsdWork.cores.switches.pitch_modulation[core]);
            if (changes[core] & SSD_VOICE_FLAG_NOISE)
                sceSdSetSwitch(core | SD_S_NON, SsdWork.cores.switches.noise[core]);
            if (changes[core] & SSD_VOICE_FLAG_REVERB) {
                sceSdSetSwitch(core | SD_S_VMIXEL, SsdWork.cores.switches.reverb_mix[core]);
                sceSdSetSwitch(core | SD_S_VMIXER, SsdWork.cores.switches.reverb_mix[core]);
            }
            if (changes[core] & SSD_VOICE_FLAG_DRY) {
                sceSdSetSwitch(core | SD_S_VMIXL, SsdWork.cores.switches.dry_mix[core]);
                sceSdSetSwitch(core | SD_S_VMIXR, SsdWork.cores.switches.dry_mix[core]);
            }
        }
        if (SsdWork.cores.switches.key_on[core] != 0) {
            sceSdSetSwitch(core | SD_S_KON, SsdWork.cores.switches.key_on[core]);
            SsdWork.cores.switches.key_on[core] = 0;
        }
    }
}

void SsdSetVoiceKeyoff(void)
{
    int core;
    int voice;
    unsigned int bits;
    for (core = 0; core < SSD_CORE_COUNT; ++core) {
        bits = SsdWork.cores.switches.cutoff[core];
        if (bits != 0) {
            for (voice = 0; voice < SSD_CORE_VOICE_COUNT; ++voice) {
                if (bits & (1 << voice)) {
                    unsigned short entry = core | (voice << 1) | SD_VP_ADSR2;
                    unsigned short adsr = sceSdGetParam(entry);
                    sceSdSetParam(entry, (adsr & 0xffc0) | 0x66);
                }
            }
        }
        bits |= SsdWork.cores.switches.key_off[core];
        if (bits != 0) {
            sceSdSetSwitch(core | SD_S_KOFF, bits);
            SsdWork.cores.switches.key_off[core] = SsdWork.cores.switches.cutoff[core] = 0;
        }
    }
}

int SsdNoteToPitch(int note)
{
    int key = SsdAllKeyTable[(unsigned)(note & 0x7fff) >> 8];
    int pitch;
    int distance;
    unsigned normalized;
    note &= 0xff;
    do {
        note += (key & 0xf) << 8;
    } while (0);
    pitch = SsdAllPitchTable[note];
    distance = 6 - (key >> 4);
    normalized = (unsigned)distance;
    distance >>= 31;
    normalized = (normalized ^ ((unsigned)distance & 224U)) & 255U;
    do { return (pitch) >> (normalized); } while (0);
}

void SsdCompellingKeyoff(void)
{
    int i;
    SsdVoice *voice;
    for (i = 0; i < SSD_VOICE_COUNT; ++i) {
        SsdSetVoiceRRAttr(i, 6, 3);
    }
    sceSdSetSwitch(0 | SD_S_KOFF, SSD_CORE_VOICE_MASK);
    sceSdSetSwitch(1 | SD_S_KOFF, SSD_CORE_VOICE_MASK);
    voice = SsdWork.voices;
    for (i = 0; i < SSD_VOICE_COUNT; ++i) {
        SsdUnlinkChannelVoice(voice);
        ++voice;
    }
}

void SsdCalcDirectVoiceVolume(short output[2], int volume, int pan)
{
    int left;
    int right;
    volume *= volume;
    pan = (pan - 1) << 8;
    if (pan > SSD_PAN_RANGE) pan = SSD_PAN_RANGE;
    if (pan < 0) pan = 0;
    if (SsdWork.output_mode) {
        if (pan < SSD_PAN_CENTER) {
            right = pan * SSD_PAN_CENTER_GAIN / SSD_PAN_CENTER;
            left = SSD_PAN_UNITY_GAIN - pan * SSD_PAN_ATTENUATION / SSD_PAN_CENTER;
        } else {
            pan = SSD_PAN_RANGE - pan;
            left = pan * SSD_PAN_CENTER_GAIN / SSD_PAN_CENTER;
            right = SSD_PAN_UNITY_GAIN - pan * SSD_PAN_ATTENUATION / SSD_PAN_CENTER;
        }
        left = left * volume >> 15;
        right = right * volume >> 15;
    } else {
        right = volume * SSD_PAN_CENTER_GAIN >> 15;
        left = right;
    }
    output[0] = left & 0x7fff;
    output[1] = right & 0x7fff;
}
