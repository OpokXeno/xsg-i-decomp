/*
 * TU-local declarations of ov01/tu013 (src/ov01/snd.c).
 */

#ifndef SRC_OV01_SND_H
#define SRC_OV01_SND_H

#include "shared.h"
#include "ov01/obj.h"
#include "ov01/calc.h"

/*
 * Sound-effect index of a loaded bank.  An enumeration rather than int: the
 * dataSndSeRegLoad2 fill loop stores it next to int loads of the bank file's
 * offset table, and only a type that cannot alias those loads reproduces the
 * original's addiu-before-lw loop head (ov01:0x00a2d820; with int the two
 * instructions swap).
 */
typedef enum SndSeType {
    SND_SE_TYPE_NONE = 0 /* no bank loaded / lookup failed */
} SndSeType;

/* seType is intentionally under-typed: it holds the resolved bank index
 * (1..N, SND_SE_TYPE_NONE on failure); the enum exists for its alias set. */
typedef struct {
    SndSeType seType; /* +0x00: echoes the resolved sound-effect index; SND_SE_TYPE_NONE on lookup failure */
    void *dest; /* +0x04: destination address dataFileLoadNB loads into */
    int size;   /* +0x08: file size rounded up to a 2048-byte CD sector */
} SndSeLoadWork;

/*
 * Byte offsets of one bank's "sed" and "swd" blocks from the start of the
 * loaded sound-effect bank file it belongs to.
 */
typedef struct SndSeBankOffset {
    int sed; /* +0x00 */
    int swd; /* +0x04 */
} SndSeBankOffset;

/*
 * Header of a loaded sound-effect bank file (SndSeLoadWork.dest): the 64-bit
 * count of 32-bit offset words that follow (ld 0(a2) at ov01:0x00a2d7fc,
 * halved into the bank count at 0x00a2d800..0x00a2d80c), then one
 * SndSeBankOffset per bank (lw 8(a3) / lw 12(a3) at ov01:0x00a2d824/
 * 0x00a2d828, stride 8 at 0x00a2d82c).
 */
typedef struct SndSeBankHeader {
    long long count;        /* +0x00 */
    SndSeBankOffset offset; /* +0x08: the first bank's pair */
} SndSeBankHeader;

extern int dataNormIdxGet(ObjectTask *unit, int seType);
extern CalcUnitParam *calcUPGet(ObjectTask *unit);

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
    int mode;                       /* +0x00 */
    void *smdNormal;                /* +0x04 */
    void *smdAlt;                   /* +0x08 */
    void *swd;                      /* +0x0c */
    int smdSize;                    /* +0x10 */
    int smdAltSize;                 /* +0x14 */
    int swdSize;                    /* +0x18 */
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

/* The unit the sound belongs to, sound-effect id, and sound number. */
int sndSePlay(int unit, int id, int no);

int sndMuPlay(void);

#endif /* SRC_OV01_SND_H */
