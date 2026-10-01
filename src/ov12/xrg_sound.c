/*
 * OV12 original TU 94: 0x00a4e8f0..0x00a4f280 (27 functions)
 */
#include "common.h"
#include "shared.h"
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

extern const SoundPathAlternatePrefix D_00A59250;
extern const char D_00A59268[];
extern const SoundPathStandardPrefix D_00A59270;
extern const char D_00A59280[];
extern int xglSoundSendSwd(void *swd, int bank);
extern int xglSoundSendSmd2(void *smd, int bank);
extern int SsdSpuDmaCompleted(int wait);
extern char *strcat(char *destination, const char *source);

static int _LoadSequence(const char *sequenceName, void *buffer, int useAlternatePath)
{
    int readResult;
    SoundPath path;

    if (sequenceName == 0 || buffer == 0) {
        xglSoundSendSwd(0, -1);
        return xglSoundSendSmd2(0, 0);
    }

    if (useAlternatePath != 0) {
        path.alternate.first = D_00A59250.first;
        path.alternate.second = D_00A59250.second;
        path.alternate.third = D_00A59250.third;
        path.alternate.fourth = D_00A59250.fourth;
        path.alternate.terminator = D_00A59250.terminator;
        strcat(strcat(path.text, sequenceName), D_00A59268);
    } else {
        path.standard.first = D_00A59270.first;
        path.standard.second = D_00A59270.second;
        strcat(strcat(path.text, sequenceName), D_00A59268);
    }

    readResult = xglCdReadFile(path.text, buffer, 0, 0);

    if (readResult > 0) {
        xglSoundSendSwd(buffer, -1);
        while (SsdSpuDmaCompleted(0) != 0) {
        }
    }

    if (useAlternatePath != 0) {
        path.alternate.first = D_00A59250.first;
        path.alternate.second = D_00A59250.second;
        path.alternate.third = D_00A59250.third;
        path.alternate.fourth = D_00A59250.fourth;
        path.alternate.terminator = D_00A59250.terminator;
        strcat(strcat(path.text, sequenceName), D_00A59280);
    } else {
        path.standard.first = D_00A59270.first;
        path.standard.second = D_00A59270.second;
        strcat(strcat(path.text, sequenceName), D_00A59280);
    }

    readResult = xglCdReadFile(path.text, buffer, 0, 0);

    if (readResult > 0) {
        return xglSoundSendSmd2(buffer, 0);
    }
    return readResult;
}

void InitXrgSoundSystem(void)
{
    void *scratch = RgHeapAlloc(InstanceOfRgHeapData(), 0x400000, D_00A59288, 0x66);

    xglSoundLoadEffect(D_00A592A0, scratch, 2);
    RgHeapFree(InstanceOfRgHeapData(), scratch, D_00A59288, 0x6C);
    s_eLoadedSeq = -1;
    s_bNowPlaying = 0;
}

void DisposeXrgSoundSystem(void)
{
    xglSoundLoadEffect(0, 0, 2);
    _LoadSequence(0, 0, 0);
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_sound", _LoadSeq);

extern void _LoadSeq(void);
extern void xglSoundSequenceNormal2(int channel, int volume);

void XrgSoundSystemPlayBGM(void)
{
    _LoadSeq();
    if (s_eLoadedSeq != -1 && s_bNowPlaying == 0) {
        xglSoundSequenceNormal2(0, 127);
        s_bNowPlaying = 1;
    }
}

void XrgSoundSystemPlayJingle(void)
{
}

extern void xglSoundSequenceFadeOut2(int channel, int time);

void XrgSoundSystemStopSequence(void)
{
    if (s_eLoadedSeq != -1) {
        xglSoundSequenceFadeOut2(0, 100);
        s_bNowPlaying = 0;
    }
}

/*
 * Forgets the loaded sequence. The body never reads sequenceId, but the one
 * caller (rg_main.c _DisposeBattleDatas) loads a literal 0 into $a0 before
 * its tail call (daddu $4,$0,$0 at 0x00a01100), so the original prototype
 * took the argument.
 */
void XrgSoundSystemDisposeSequence(int sequenceId)
{
    s_eLoadedSeq = -1;
}

void XrgSoundSystemStop(void)
{
    xglSoundEffectStopBank(2);
}

void XrgSoundSystemRing(int soundId)
{
    if (soundId > 0) {
        xglSoundEffectNormalID(soundId, 0);
    }
}

void XrgSoundSystemCursor(void)
{
    xglSoundEffectNormalID(3, 0);
}

void XrgSoundSystemOk(void)
{
    xglSoundEffectNormalID(1, 0);
}

void XrgSoundSystemCancel(void)
{
    xglSoundEffectNormalID(2, 0);
}

void XrgSoundSystemNG(void)
{
    xglSoundEffectNormalID(5, 0);
}

static void _InitSound(XrgSound *sound, int kind)
{
    if (sound != 0) {
        sound->kind = kind;
        sound->handle = 0;
        sound->cleared_word = 0;
        sound->volume = 0x40;
        sound->pan = 0x40;
    }
}

static void _DestructSound(void)
{
}

XrgSound *CreateXrgSound(int unused, int kind)
{
    XrgSound *sound;

    sound = RgHeapAlloc(InstanceOfRgHeap(), sizeof(XrgSound), D_00A59288, 253);
    _InitSound(sound, kind);
    return sound;
}

void DisposeXrgSound(XrgSound *sound)
{
    if (sound != 0) {
        _DestructSound();
        RgHeapFree(InstanceOfRgHeap(), sound, D_00A59288, 0x107);
    }
}

static void _ring(XrgSound *sound, int soundId, int gain)
{
    if (sound != 0) {
        if (soundId > 0) {
            xglSoundEffectParamID(soundId | 0x20000, (gain * sound->volume) >> 7, sound->pan, 0);
        }
    }
}

static void _stop(XrgSound *sound, int soundId)
{
    if (sound != 0) {
        if (soundId > 0) {
            xglSoundEffectStopID(soundId | 0x20000, 0);
        }
    }
}

void XrgSoundSetVolume(XrgSound *sound, float volume)
{
    if (sound != 0) {
        float max_volume = 1.0f;

        if (volume < 0.0f) {
            volume = 0.0f;
        } else if (volume > max_volume) {
            volume = max_volume;
        }
        sound->volume = (int)(volume * 127.0f);
    }
}

void XrgSoundRingMoving(XrgSound *sound, int forceRestart, float timer)
{
    int soundEffectId = 0;

    if (sound != 0) {
        int restart = forceRestart != 0;

        if ((unsigned int) sound->kind < 6) {
            switch (sound->kind) {
            case 0:
                soundEffectId = (restart + 1) | 0x20000;
                break;
            case 4:
                soundEffectId = (restart + 5) | 0x20000;
                break;
            case 2:
                soundEffectId = (restart + 7) | 0x20000;
                break;
            case 5:
                soundEffectId = (restart + 3) | 0x20000;
                break;
            case 3:
                soundEffectId = (restart + 9) | 0x20000;
                break;
            case 1:
                soundEffectId = (restart + 11) | 0x20000;
                break;
            }
        }

        if (soundEffectId != 0) {
            int currentHandle = sound->handle;

            if (soundEffectId != currentHandle) {
                if (currentHandle == 0 || forceRestart != 0) {
                    xglSoundEffectStopID(currentHandle, 0);
                    _ring(sound, soundEffectId, 0x80);
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
            _ring(sound, soundId, 128);
        }
    }
}

void XrgSoundRingVol(XrgSound *sound, int soundId, int gain)
{
    if (sound != 0) {
        if (soundId > 0) {
            int maxGain = 128;

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
