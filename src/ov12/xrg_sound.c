/*
 * OV12 original TU 94: 0x00a4e8f0..0x00a4f280 (27 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_heap.h"
#include "main/xgl_cd.h"
#include "main/xgl_sound.h"
#include "ssd/ssd_dma.h"
#include "xrg_sound.h"

typedef struct {
    u64 first;
    u64 second;
    u32 third;
    u16 fourth;
    u8 terminator;
} SoundPathAlternatePrefix;

typedef struct {
    u64 first;
    u64 second;
} SoundPathStandardPrefix;

typedef union {
    char text[256];
    SoundPathAlternatePrefix alternate;
    SoundPathStandardPrefix standard;
} SoundPath;

const SoundPathAlternatePrefix soundAlternateSequenceDirectory = {
    0x6d61795c61746164ULL, 0x6e735c6f746f6d61ULL,
    0x6d735c64u, 0x5c64u, 0x00u,
};
static const char soundWaveBankExtension[8] = ".SWD";
const SoundPathStandardPrefix soundStandardSequenceDirectory = {
    0x756f735c61746164ULL, 0x5c646d735c646eULL,
};
static const char soundSequenceExtension[8] = ".SMD";
static const char soundSourcePath[24] = "../xrg_sound.euc.c";
static const char soundMiniBattleBankName[8] = "MINIBAT";
/* Scratch buffers the loaders borrow from the data heap. */
#define XRG_SOUND_BANK_BUFFER_SIZE 0x400000
#define XRG_SOUND_SEQUENCE_BUFFER_SIZE 0x200000

/* The effect bank the game's sound effects come from, and the effect id bit that selects it. */
#define XRG_SOUND_EFFECT_BANK 2
#define XRG_SOUND_EFFECT_ID_BANK 0x20000

#define XRG_SOUND_NO_SEQUENCE -1
#define XRG_SOUND_BGM_CHANNEL 0
#define XRG_SOUND_BGM_VOLUME 127
#define XRG_SOUND_BGM_FADE_TIME 100

/* Gain scale (128 is full), and the centred value (0x40) that a sound's volume and pan start at. */
#define XRG_SOUND_GAIN_MAX 128
#define XRG_SOUND_VOLUME_MAX 127
#define XRG_SOUND_CENTER 0x40

/* XrgSoundRingMoving picks one of this many sound ids by XrgSound::kind. */
#define XRG_SOUND_RING_KINDS 6

static int s_eLoadedSeq = XRG_SOUND_NO_SEQUENCE;
static int s_bNowPlaying = 0;

/* Sequence names, defined at the end of this file. */
extern const char soundBattle1SequenceName[];
extern const char soundBattle2SequenceName[];
extern const char soundBattle3SequenceName[];
extern const char soundUmnModeSequenceName[];

extern void xglSoundLoadEffect(const char *bank_name, void *buffer, int mode);

/*
 * Reads the sequence's wave bank (.SWD) and sequence (.SMD) into buffer and sends them
 * to the sound driver. useAlternatePath selects the alternate directory.
 */
static int _LoadSequence(const char *sequenceName, void *buffer, int useAlternatePath)
{
    int readResult;
    SoundPath path;

    if (sequenceName == 0 || buffer == 0) {
        xglSoundSendSwd(0, -1);
        return xglSoundSendSmd2(0, 0);
    }

    if (useAlternatePath != 0) {
        path.alternate.first = soundAlternateSequenceDirectory.first;
        path.alternate.second = soundAlternateSequenceDirectory.second;
        path.alternate.third = soundAlternateSequenceDirectory.third;
        path.alternate.fourth = soundAlternateSequenceDirectory.fourth;
        path.alternate.terminator = soundAlternateSequenceDirectory.terminator;
        strcat(strcat(path.text, sequenceName), soundWaveBankExtension);
    } else {
        path.standard.first = soundStandardSequenceDirectory.first;
        path.standard.second = soundStandardSequenceDirectory.second;
        strcat(strcat(path.text, sequenceName), soundWaveBankExtension);
    }

    readResult = xglCdReadFile(path.text, buffer, 0, 0);

    if (readResult > 0) {
        xglSoundSendSwd(buffer, -1);
        while (SsdSpuDmaCompleted(0) != 0) {
        }
    }

    if (useAlternatePath != 0) {
        path.alternate.first = soundAlternateSequenceDirectory.first;
        path.alternate.second = soundAlternateSequenceDirectory.second;
        path.alternate.third = soundAlternateSequenceDirectory.third;
        path.alternate.fourth = soundAlternateSequenceDirectory.fourth;
        path.alternate.terminator = soundAlternateSequenceDirectory.terminator;
        strcat(strcat(path.text, sequenceName), soundSequenceExtension);
    } else {
        path.standard.first = soundStandardSequenceDirectory.first;
        path.standard.second = soundStandardSequenceDirectory.second;
        strcat(strcat(path.text, sequenceName), soundSequenceExtension);
    }

    readResult = xglCdReadFile(path.text, buffer, 0, 0);

    if (readResult > 0) {
        return xglSoundSendSmd2(buffer, 0);
    }
    return readResult;
}

/* Loads the mini battle effect bank through a temporary data-heap buffer and clears the sequence state. */
void InitXrgSoundSystem(void)
{
    void *scratch = RgHeapAlloc(InstanceOfRgHeapData(), XRG_SOUND_BANK_BUFFER_SIZE, soundSourcePath, 0x66);

    xglSoundLoadEffect(soundMiniBattleBankName, scratch, XRG_SOUND_EFFECT_BANK);
    RgHeapFree(InstanceOfRgHeapData(), scratch, soundSourcePath, 0x6C);
    s_eLoadedSeq = XRG_SOUND_NO_SEQUENCE;
    s_bNowPlaying = 0;
}

/* Unloads the effect bank and the sequence data. */
void DisposeXrgSoundSystem(void)
{
    xglSoundLoadEffect(0, 0, XRG_SOUND_EFFECT_BANK);
    _LoadSequence(0, 0, 0);
}

/* Loads the sequence for an XRG_SEQ_* id, unless it is already loaded; stops the BGM channel first. */
void _LoadSeq(int sequence)
{
    void *buffer;

    if (sequence == s_eLoadedSeq) {
        return;
    }
    buffer = RgHeapAlloc(InstanceOfRgHeapData(), XRG_SOUND_SEQUENCE_BUFFER_SIZE, soundSourcePath, 0x84);
    s_bNowPlaying = 0;
    xglSoundSequenceStop2(XRG_SOUND_BGM_CHANNEL);
    switch (sequence) {
    case XRG_SEQ_BATTLE1:
        _LoadSequence(soundBattle1SequenceName, buffer, 1);
        s_eLoadedSeq = sequence;
        break;
    case XRG_SEQ_BATTLE2:
        _LoadSequence(soundBattle2SequenceName, buffer, 1);
        s_eLoadedSeq = sequence;
        break;
    case XRG_SEQ_BATTLE3:
        _LoadSequence(soundBattle3SequenceName, buffer, 1);
        s_eLoadedSeq = sequence;
        break;
    case XRG_SEQ_UMNMODE:
        _LoadSequence(soundUmnModeSequenceName, buffer, 0);
        s_eLoadedSeq = sequence;
        break;
    }
    RgHeapFree(InstanceOfRgHeapData(), buffer, soundSourcePath, 0x9D);
}

/* Loads the sequence and starts it on the BGM channel unless it is already playing. */
void XrgSoundSystemPlayBGM(int sequence)
{
    _LoadSeq(sequence);
    if (s_eLoadedSeq != XRG_SOUND_NO_SEQUENCE && s_bNowPlaying == 0) {
        xglSoundSequenceNormal2(XRG_SOUND_BGM_CHANNEL, XRG_SOUND_BGM_VOLUME);
        s_bNowPlaying = 1;
    }
}

void XrgSoundSystemPlayJingle(void)
{
}

/* Fades out the BGM channel when a sequence is loaded. */
void XrgSoundSystemStopSequence(void)
{
    if (s_eLoadedSeq != XRG_SOUND_NO_SEQUENCE) {
        xglSoundSequenceFadeOut2(XRG_SOUND_BGM_CHANNEL, XRG_SOUND_BGM_FADE_TIME);
        s_bNowPlaying = 0;
    }
}

/*
 * Forgets the loaded sequence. The body never reads sequenceId; the one caller
 * (rg_main.c _DisposeBattleDatas) passes 0, and the original prototype takes the argument.
 */
void XrgSoundSystemDisposeSequence(int sequenceId)
{
    s_eLoadedSeq = XRG_SOUND_NO_SEQUENCE;
}

void XrgSoundSystemStop(void)
{
    xglSoundEffectStopBank(XRG_SOUND_EFFECT_BANK);
}

void XrgSoundSystemRing(int soundId)
{
    if (soundId > 0) {
        xglSoundEffectNormalID(soundId, 0);
    }
}

void XrgSoundSystemCursor(void)
{
    xglSoundEffectNormalID(SE_CURSOR, 0);
}

void XrgSoundSystemOk(void)
{
    xglSoundEffectNormalID(SE_DECIDE, 0);
}

void XrgSoundSystemCancel(void)
{
    xglSoundEffectNormalID(SE_CANCEL, 0);
}

void XrgSoundSystemNG(void)
{
    xglSoundEffectNormalID(SE_BUZZER, 0);
}

static void _InitSound(XrgSound *sound, int kind)
{
    if (sound != 0) {
        sound->kind = kind;
        sound->handle = 0;
        sound->cleared_word = 0;
        sound->volume = XRG_SOUND_CENTER;
        sound->pan = XRG_SOUND_CENTER;
    }
}

static void _DestructSound(void)
{
}

XrgSound *CreateXrgSound(int unused, int kind)
{
    XrgSound *sound;

    sound = RgHeapAlloc(InstanceOfRgHeap(), sizeof(XrgSound), soundSourcePath, 253);
    _InitSound(sound, kind);
    return sound;
}

void DisposeXrgSound(XrgSound *sound)
{
    if (sound != 0) {
        _DestructSound();
        RgHeapFree(InstanceOfRgHeap(), sound, soundSourcePath, 0x107);
    }
}

/* Plays a sound effect id in the effect bank, with gain scaled by the sound's volume and its pan. */
static void _ring(XrgSound *sound, int soundId, int gain)
{
    if (sound != 0) {
        if (soundId > 0) {
            xglSoundEffectParamID(soundId | XRG_SOUND_EFFECT_ID_BANK, (gain * sound->volume) >> 7, sound->pan, 0);
        }
    }
}

static void _stop(XrgSound *sound, int soundId)
{
    if (sound != 0) {
        if (soundId > 0) {
            xglSoundEffectStopID(soundId | XRG_SOUND_EFFECT_ID_BANK, 0);
        }
    }
}

/* Sets the sound's volume from 0.0 to 1.0, clamped to that range. */
void XrgSoundSetVolume(XrgSound *sound, float volume)
{
    if (sound != 0) {
        float max_volume = 1.0f;

        if (volume < 0.0f) {
            volume = 0.0f;
        } else if (volume > max_volume) {
            volume = max_volume;
        }
        sound->volume = (int)(volume * (float)XRG_SOUND_VOLUME_MAX);
    }
}

/*
 * Plays the effect chosen by the sound's kind and records the timer. A nonzero forceRestart
 * restarts an effect that is already playing.
 */
void XrgSoundRingMoving(XrgSound *sound, int forceRestart, float timer)
{
    int soundEffectId = 0;

    if (sound != 0) {
        int restart = forceRestart != 0;

        if ((unsigned int) sound->kind < XRG_SOUND_RING_KINDS) {
            switch (sound->kind) {
            case 0:
                soundEffectId = (restart + 1) | XRG_SOUND_EFFECT_ID_BANK;
                break;
            case 4:
                soundEffectId = (restart + 5) | XRG_SOUND_EFFECT_ID_BANK;
                break;
            case 2:
                soundEffectId = (restart + 7) | XRG_SOUND_EFFECT_ID_BANK;
                break;
            case 5:
                soundEffectId = (restart + 3) | XRG_SOUND_EFFECT_ID_BANK;
                break;
            case 3:
                soundEffectId = (restart + 9) | XRG_SOUND_EFFECT_ID_BANK;
                break;
            case 1:
                soundEffectId = (restart + 11) | XRG_SOUND_EFFECT_ID_BANK;
                break;
            }
        }

        if (soundEffectId != 0) {
            int currentHandle = sound->handle;

            if (soundEffectId != currentHandle) {
                if (currentHandle == 0 || forceRestart != 0) {
                    xglSoundEffectStopID(currentHandle, 0);
                    _ring(sound, soundEffectId, XRG_SOUND_GAIN_MAX);
                    sound->handle = soundEffectId;
                }
            }
            sound->timer = timer;
        }
    }
}

void XrgSoundRingStopMoving(XrgSound *sound)
{
    int handle;

    if (sound != 0) {
        handle = sound->handle;
        if (handle > 0) {
            xglSoundEffectStopID(handle, 0);
            sound->handle = 0;
        }
    }
}

void XrgSoundRing(XrgSound *sound, int soundId)
{
    if (sound != 0) {
        if (soundId > 0) {
            _ring(sound, soundId, XRG_SOUND_GAIN_MAX);
        }
    }
}

void XrgSoundRingVol(XrgSound *sound, int soundId, int gain)
{
    if (sound != 0) {
        if (soundId > 0) {
            int maxGain = XRG_SOUND_GAIN_MAX;

            if (gain < 0) {
                gain = 0;
            } else if (gain > maxGain) {
                gain = maxGain;
            }
            _ring(sound, soundId, gain);
        }
    }
}

void XrgSoundRingStop(XrgSound *sound, int soundId)
{
    if (sound != 0) {
        if (soundId > 0) {
            _stop(sound, soundId);
        }
    }
}

/* Counts the sound's timer down by dt and stops its effect when it runs out. A timer of 1e8 never runs out. */
void XrgSoundPassTime(XrgSound *sound, float dt)
{
    if (sound != 0) {
        if (sound->timer != 1e8f) {
            sound->timer -= dt;
            if (sound->timer < 0.0f) {
                if (sound->handle != 0) {
                    xglSoundEffectStopID(sound->handle, 0);
                    sound->handle = 0;
                }
            }
        }
    }
}



const char soundBattle1SequenceName[8] = "BATTLE1";

const char soundBattle2SequenceName[8] = "BATTLE2";

const char soundBattle3SequenceName[8] = "BATTLE3";

const char soundUmnModeSequenceName[16] = "Umnmode";
