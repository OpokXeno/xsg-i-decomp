#ifndef INCLUDE_OV12_XRG_SOUND_H
#define INCLUDE_OV12_XRG_SOUND_H

/* A ring sound-effect object; the layout is private to src/ov12/xrg_sound.c. */
typedef struct XrgSound XrgSound;

/* Background sequences XrgSoundSystemPlayBGM loads. */
#define XRG_SEQ_BATTLE1 0
#define XRG_SEQ_BATTLE2 1
#define XRG_SEQ_BATTLE3 2
#define XRG_SEQ_UMNMODE 3

/* Functions and data used by other translation units. */
void InitXrgSoundSystem(void);
void DisposeXrgSoundSystem(void);
void _LoadSeq(int sequence);
void XrgSoundSystemPlayBGM(int sequence);
void XrgSoundSystemPlayJingle(void);
void XrgSoundSystemStopSequence(void);
void XrgSoundSystemDisposeSequence(int sequenceId);
void XrgSoundSystemStop(void);
void XrgSoundSystemRing(int soundId);
void XrgSoundSystemCursor(void);
void XrgSoundSystemOk(void);
void XrgSoundSystemCancel(void);
void XrgSoundSystemNG(void);
XrgSound *CreateXrgSound(int unused, int kind);
void DisposeXrgSound(XrgSound *sound);
void XrgSoundSetVolume(XrgSound *sound, float volume);
void XrgSoundRingMoving(XrgSound *sound, int forceRestart, float timer);
void XrgSoundRingStopMoving(XrgSound *sound);
void XrgSoundRing(XrgSound *sound, int soundId);
void XrgSoundRingVol(XrgSound *sound, int soundId, int gain);
void XrgSoundRingStop(XrgSound *sound, int soundId);
void XrgSoundPassTime(XrgSound *sound, float dt);

#endif /* INCLUDE_OV12_XRG_SOUND_H */
