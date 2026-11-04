/*
 * TU-local declarations of ov12/tu094 (src/ov12/xrg_sound.c).
 */

#ifndef SRC_OV12_XRG_SOUND_H
#define SRC_OV12_XRG_SOUND_H

#include "shared.h"
#include "ov12/xrg_sound.h"

/*
 * A runtime "ring" sound-effect object. CreateXrgSound allocates one with
 * RgHeapAlloc(sizeof(XrgSound)), 24 bytes, and DisposeXrgSound frees it back
 * through RgHeapFree. _InitSound seeds every field except the timer, which
 * only XrgSoundRingMoving writes.
 */
struct XrgSound {
    int kind;         /* +0x00: CreateXrgSound's own second argument; XrgSoundRingMoving
                          indexes a 6-entry sound-id table with it */
    int handle;       /* +0x04: engine handle of the sound currently ringing, or 0 */
    float timer;      /* +0x08: countdown to auto-stop (XrgSoundPassTime); the 1e8f
                          sentinel disables the timeout */
    int cleared_word; /* +0x0C: cleared by _InitSound; no function in this translation
                          unit reads it back */
    int volume;       /* +0x10: 0..127 scale XrgSoundSetVolume derives from a 0..1 float;
                          _ring folds it into the engine call's volume argument */
    int pan;          /* +0x14: forwarded verbatim as xglSoundEffectParamID's third
                          argument; its exact engine meaning is not evidenced */
};

#endif /* SRC_OV12_XRG_SOUND_H */
