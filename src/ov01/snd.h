/*
 * TU-local declarations of ov01/tu013 (src/ov01/snd.c).
 */

#ifndef SRC_OV01_SND_H
#define SRC_OV01_SND_H

#include "shared.h"
#include "ov01/obj.h"

/*
 * The deferred sound-effect object sndSeTransPlay allocates and
 * sndSeTransPlayObj plays back once the SPU DMA transfer completes.  Only the
 * ObjectTask prefix (base, +0x00) and the three word arguments
 * sndSeTransPlayObj forwards to sndSePlay are evidenced (lw a0,28(s0);
 * lw a1,32(s0); lw a2,36(s0) at 0x00a2e128..0x00a2e134); the 8 bytes between
 * them are read by nothing this allocation touches and stay unmodelled.
 */
typedef struct {
    ObjectTask base;                /* +0x00 */
    unsigned char unmodeled_14[8];  /* +0x14 */
    int seId;                       /* +0x1c */
    int volume;                     /* +0x20 */
    int pan;                        /* +0x24 */
} SndSePlayTask;

/*
 * The loaded music bank sndMuTrans transfers to the sound engine.  Only the
 * three word fields sndMuTrans reads are evidenced (lw a0,4(s0) at
 * 0x00a2e36c, lw a0,12(s0) at 0x00a2e370, lw a0,11656(v0) i.e. sndMuDat+8 at
 * 0x00a2e390); the word before them and the twelve bytes after stay
 * unmodelled.  The three are addresses of loaded sound data:
 * xglSoundSendSmd hands its argument to xglSoundSendSmd2, which null-checks
 * it and passes it to SsdAddSequenceData.
 */
typedef struct {
    unsigned char unmodeled_00[4];  /* +0x00 */
    void *smdNormal;                /* +0x04 */
    void *smdAlt;                   /* +0x08 */
    void *swd;                      /* +0x0c */
    unsigned char unmodeled_10[12]; /* +0x10 */
} SndMuData;

extern SndMuData sndMuDat;

/*
 * Despite its name, returns the SPU-DMA busy bit of RssdWork (bit 2): nonzero
 * while a transfer is still running.  A nonzero wait first spins on the SIF
 * request queue until the bit clears (main:0x002409b0).
 */
extern int SsdSpuDmaCompleted(int wait);

extern void xglSoundSendSmd(void *smd);
/* Returns the bank id it also stores into the swd header (0xffff when none). */
extern int xglSoundSendSwd(void *swd, int bank);
extern void xglSoundSequenceNormal(int volume);
extern void xglSoundSequenceFadeOut(int time);
extern void xglSoundSequenceNormal3(int channel, int volume, int time);

void sndSePlay(int seId, int volume, int pan);

int sndMuPlay(void);

#endif /* SRC_OV01_SND_H */
