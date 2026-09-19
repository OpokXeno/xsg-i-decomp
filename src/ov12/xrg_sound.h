/*
 * TU-local declarations of ov12/tu094 (src/ov12/xrg_sound.c).
 */

#ifndef SRC_OV12_XRG_SOUND_H
#define SRC_OV12_XRG_SOUND_H

#include "shared.h"

/*
 * A runtime "ring" sound-effect object. CreateXrgSound (0x00a4ee88, still
 * INCLUDE_ASM) allocates one with RgHeapAlloc(..., 0x18, ...) -- 24 bytes,
 * matching the six 4-byte fields below -- and DisposeXrgSound (0x00a4eee0)
 * frees it back through RgHeapFree. _InitSound (0x00a4ee58) seeds every
 * field except the timer (+0x08), which only XrgSoundRingMoving writes.
 * XrgSoundSetVolume (0x00a4efd0), _ring/_stop (0x00a4ef38/0x00a4ef90),
 * XrgSoundRingStopMoving (0x00a4f110) and the still-INCLUDE_ASM
 * XrgSoundRingMoving/XrgSoundPassTime (0x00a4f030/0x00a4f200) are its only
 * users in this translation unit.
 */
typedef struct XrgSound {
    int kind;         /* +0x00: CreateXrgSound's own second argument; XrgSoundRingMoving
                          indexes a 6-entry sound-id table (jtbl_00A592D0) with it */
    int handle;       /* +0x04: engine handle of the sound currently ringing, or 0 */
    float timer;      /* +0x08: countdown to auto-stop (XrgSoundPassTime); the 1e8f
                          sentinel disables the timeout */
    int cleared_word; /* +0x0C: cleared by _InitSound; no function in this translation
                          unit reads it back */
    int volume;       /* +0x10: 0..127 scale XrgSoundSetVolume derives from a 0..1 float;
                          _ring folds it into the engine call's volume argument */
    int pan;          /* +0x14: forwarded verbatim as xglSoundEffectParamID's third
                          argument; its exact engine meaning is not evidenced */
} XrgSound;

extern void *RgHeapAlloc(void *heap, unsigned int size, const char *source_file, int line);
extern void RgHeapFree(void *heap, void *ptr, const char *source_file, int line);
extern RgHeap *InstanceOfRgHeapData(void);
extern RgHeap *InstanceOfRgHeap(void);

extern void xglSoundLoadEffect(const char *bank_name, void *buffer, int mode);
extern void xglSoundEffectStopBank(int bank);
extern void xglSoundEffectNormalID(int sound_id, int variant);
extern void xglSoundEffectParamID(int sound_id, int volume, int pan, int flags);
extern void xglSoundEffectStopID(int sound_id, int flags);

extern int s_bNowPlaying;
extern int s_eLoadedSeq;

/* ../xrg_sound.euc.c: the source-file debug tag every RgHeapAlloc/RgHeapFree
   call in this translation unit passes; .data is still scaffold-owned. */
extern const char D_00A59288[];
/* "MINIBAT": the sound bank InitXrgSoundSystem primes; .data is still
   scaffold-owned. */
extern const char D_00A592A0[];

int _LoadSequence(int, int, int);

#endif /* SRC_OV12_XRG_SOUND_H */
