/*
 * OV12 original TU 94: 0x00a4e8f0..0x00a4f280 (27 functions)
 */
#include "common.h"
#include "shared.h"
#include "xrg_sound.h"

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_sound", _LoadSequence);

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

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_sound", XrgSoundSystemPlayBGM);

void XrgSoundSystemPlayJingle(void)
{
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_sound", XrgSoundSystemStopSequence);

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

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_sound", XrgSoundSystemRing);

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

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_sound", CreateXrgSound);

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

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_sound", XrgSoundRingMoving);

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

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_sound", XrgSoundRing);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_sound", XrgSoundRingVol);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_sound", XrgSoundRingStop);

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
