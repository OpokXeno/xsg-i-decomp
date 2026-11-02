#include "ssd/ssd_dev.h"
#include "ssd/ssd_voic.h"

void SsdSetVoiceVolume(int voice, int left, int right)
{
    int entry;
    if (voice >= SSD_CORE_VOICE_COUNT) {
        entry = SD_CORE_1;
        voice -= SSD_CORE_VOICE_COUNT;
    } else {
        entry = SD_CORE_0;
    }
    voice *= 2;
    entry |= voice;
    sceSdSetParam(entry, left);
    entry |= SD_VP_VOLR;
    sceSdSetParam(entry, right);
}

void SsdSetVoicePitch(int voice, int pitch)
{
    int core;
    if (voice >= SSD_CORE_VOICE_COUNT) {
        core = SD_CORE_1;
        voice -= SSD_CORE_VOICE_COUNT;
    } else {
        core = SD_CORE_0;
    }
    sceSdSetParam(core | (voice * 2) | SD_VP_PITCH, pitch);
}

void SsdSetVoiceAdsr1(int voice, int adsr)
{
    int core;
    if (voice >= SSD_CORE_VOICE_COUNT) {
        core = SD_CORE_1;
        voice -= SSD_CORE_VOICE_COUNT;
    } else {
        core = SD_CORE_0;
    }
    sceSdSetParam(core | (voice * 2) | SD_VP_ADSR1, adsr);
}

void SsdSetVoiceAdsr2(int voice, int adsr)
{
    int core;
    if (voice >= SSD_CORE_VOICE_COUNT) {
        core = SD_CORE_1;
        voice -= SSD_CORE_VOICE_COUNT;
    } else {
        core = SD_CORE_0;
    }
    sceSdSetParam(core | (voice * 2) | SD_VP_ADSR2, adsr);
}

void SsdSetVoiceARAttr(int voice, int rate, int mode)
{
    int entry;
    unsigned short adsr;
    if (voice >= SSD_CORE_VOICE_COUNT) {
        entry = SD_CORE_1;
        voice -= SSD_CORE_VOICE_COUNT;
    } else {
        entry = SD_CORE_0;
    }
    voice *= 2;
    entry |= voice;
    entry |= SD_VP_ADSR1;
        adsr = sceSdGetParam(entry);
    adsr = (adsr & 0xff) | (rate << 8) | (mode << 15);
    sceSdSetParam(entry, adsr);
}

void SsdSetVoiceDR(int voice, int rate)
{
    int entry;
    unsigned short adsr;
    if (voice >= SSD_CORE_VOICE_COUNT) {
        entry = SD_CORE_1;
        voice -= SSD_CORE_VOICE_COUNT;
    } else {
        entry = SD_CORE_0;
    }
    voice *= 2;
    entry |= voice;
    entry |= SD_VP_ADSR1;
        adsr = sceSdGetParam(entry);
    adsr = (adsr & 0xff0f) | (rate << 4);
    sceSdSetParam(entry, adsr);
}

void SsdSetVoiceSL(int voice, int level)
{
    int entry;
    unsigned short adsr;
    if (voice >= SSD_CORE_VOICE_COUNT) {
        entry = SD_CORE_1;
        voice -= SSD_CORE_VOICE_COUNT;
    } else {
        entry = SD_CORE_0;
    }
    voice *= 2;
    entry |= voice;
    entry |= SD_VP_ADSR1;
        adsr = sceSdGetParam(entry);
    adsr = level | (adsr | 0xfff0);
    sceSdSetParam(entry, adsr);
}

void SsdSetVoiceSRAttr(int voice, int rate, int mode)
{
    int entry;
    unsigned short adsr;
    if (voice >= SSD_CORE_VOICE_COUNT) {
        entry = SD_CORE_1;
        voice -= SSD_CORE_VOICE_COUNT;
    } else {
        entry = SD_CORE_0;
    }
    voice *= 2;
    entry |= voice;
    entry |= SD_VP_ADSR2;
        adsr = sceSdGetParam(entry);
    adsr = (adsr & 0x203f) | (rate << 6) | (mode << 13);
    sceSdSetParam(entry, adsr);
}

void SsdSetVoiceRRAttr(int voice, int rate, int mode)
{
    int entry;
    unsigned short adsr;
    if (voice >= SSD_CORE_VOICE_COUNT) {
        entry = SD_CORE_1;
        voice -= SSD_CORE_VOICE_COUNT;
    } else {
        entry = SD_CORE_0;
    }
    voice *= 2;
    entry |= voice;
    entry |= SD_VP_ADSR2;
        adsr = sceSdGetParam(entry);
    adsr = (adsr & 0xffc0) | rate | (mode << 5);
    sceSdSetParam(entry, adsr);
}

void SsdSetVoiceStartAddress(int voice, unsigned int address)
{
    int core;
    if (voice >= SSD_CORE_VOICE_COUNT) {
        core = SD_CORE_1;
        voice -= SSD_CORE_VOICE_COUNT;
    } else {
        core = SD_CORE_0;
    }
    sceSdSetAddr(core | (voice * 2) | SD_VA_SSA, address);
}

void SsdSetVoiceLoopStartAddress(int voice, unsigned int address)
{
    int core;
    if (voice >= SSD_CORE_VOICE_COUNT) {
        core = SD_CORE_1;
        voice -= SSD_CORE_VOICE_COUNT;
    } else {
        core = SD_CORE_0;
    }
    sceSdSetAddr(core | (voice * 2) | SD_VA_LSAX, address);
}

void SsdGetVoiceEnvelope(int voice, unsigned short *envelope)
{
    int core;
    if (voice >= SSD_CORE_VOICE_COUNT) {
        core = SD_CORE_1;
        voice -= SSD_CORE_VOICE_COUNT;
    } else {
        core = SD_CORE_0;
    }
    *envelope = sceSdGetParam(core | (voice * 2) | SD_VP_ENVX);
}

void SsdSetMasterVolume(int core, int left, int right)
{
    sceSdSetParam(SD_P_MVOLL | SD_CORE_0, left);
    sceSdSetParam(SD_P_MVOLR | SD_CORE_0, right);
    sceSdSetParam(SD_P_MVOLL | SD_CORE_1, left);
    sceSdSetParam(SD_P_MVOLR | SD_CORE_1, right);
}

void SsdSetStreamVolume(int core, int left, int right)
{
}

void SsdSetReverb(int core, int enabled)
{
    sceSdSetCoreAttr(core | SD_C_EFFECT_ENABLE, enabled);
}

void SsdSetReverbModeType(int core, int type)
{
}

void SsdSetReverbModeDepth(int core, int left, int right)
{
    SceSdEffectAttr attr;
    sceSdGetEffectAttr(core, &attr);
    attr.core = 0;
    attr.depth_left = left;
    attr.depth_right = right;
    sceSdSetEffectAttr(core, &attr);
}

void SsdSetReverbModeDelayTime(int core, int delay)
{
}

void SsdSetReverbModeFeedback(int core, int feedback)
{
}
