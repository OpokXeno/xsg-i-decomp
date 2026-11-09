/*
 * TU-local declarations of ov01/tu013 (src/ov01/snd.c).
 */

#ifndef SRC_OV01_SND_H
#define SRC_OV01_SND_H

#include "shared.h"
#include "ov01/obj.h"
#include "ov01/calc.h"
#include "ov01/snd.h"

/*
 * Sound-effect type: picks a unit's sound name in dataSndSeNameGet and tags a
 * loaded bank with its 1-based index.  Names are the ones the code proves:
 * entry 0 of the suffix tables (ov01:0x00a427f8 and following) is the empty
 * suffix of the base file, plChaMidSeName[4] is "_AT2" and [23] is "_CV"; 3..5
 * are the banks of weaponId[seType - 3] for an AGWS unit (lh $4,0x58($3) with
 * $3 = param + seType * 2, ov01:0x00a2d100) and of normalTechniqueId[seType - 3]
 * for an enemy (lh $17,0x70($3), ov01:0x00a2d178).
 */
typedef enum SndSeType {
    SND_SE_TYPE_NONE = 0,       /* no bank loaded: rejected by sndSeRegChk */
    SND_SE_TYPE_BASE = 0,       /* the name request for the base bank file */
    SND_SE_TYPE_SLOT_FIRST = 3, /* first per-slot bank */
    SND_SE_TYPE_AT2 = 4,        /* player character "_AT2" */
    SND_SE_TYPE_SLOT_LAST = 5,  /* last per-slot bank */
    SND_SE_TYPE_CV = 23,        /* player character "_CV" */
    SND_SE_TYPE_STED_BTST = 0x18, /* battle-start voice, btst table (entryPhase31) */
    SND_SE_TYPE_STED_BTED = 0x19, /* battle-end voice, bted table (entryPhase51) */
    SND_SE_TYPE_EFFECT_FIRST = 0x1A /* from here up, seType - this names an effect */
} SndSeType;

/* Sound-effect registration layout: SND_SE_REG_COUNT slots, the first
 * SND_SE_REG_HALF serving character ids below SND_SE_REG_SPLIT_ID and the rest
 * the others; each slot holds SND_SE_BANK_COUNT banks. */
#define SND_SE_REG_COUNT    6
#define SND_SE_REG_HALF     3
#define SND_SE_REG_SPLIT_ID 0x21
#define SND_SE_BANK_COUNT   5

typedef struct {
    SndSeType seType; /* +0x00: the resolved type; SND_SE_TYPE_NONE on lookup failure */
    void *dest; /* +0x04: destination address dataFileLoadNB loads into */
    int size;   /* +0x08: file size rounded up to a whole CD sector */
} SndSeLoadWork;

/*
 * One loaded sound-effect bank of a registration: its load descriptor, then the
 * addresses of its "sed" and "swd" blocks once dataSndSeLoad2 resolves them.
 */
typedef struct {
    SndSeLoadWork work; /* +0x00 */
    /* +0x0c: address of the loaded bank's "sed"-tagged sound-effect block,
     * computed from the header offset at dest+8 once the bank is loaded
     * (dataSndSeLoad2 ov01:0x00a2d67c; checked against a magic before
     * playback in sndSeTrans ov01:0x00a2d984). */
    void *sed;
    /* +0x10: address of the loaded bank's "swd"-tagged wave-data block,
     * computed from the header offset at dest+0xc the same way (dataSndSeLoad2
     * ov01:0x00a2d684; checked in sndSeTrans ov01:0x00a2d9c8). */
    void *swd;
} SndSeRegBank;

/* One sound registration: the character id it serves, then its five banks. */
typedef struct {
    int id;               /* +0x00 */
    SndSeRegBank bank[SND_SE_BANK_COUNT]; /* +0x04 */
} SndSeRegDat;

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

extern int dataNormIdxGet(BattleUnit *unit, int seType);

/*
 * The deferred sound-effect object sndSeTransPlay allocates and
 * sndSeTransPlayObj plays back once the SPU DMA transfer completes.  Only the
 * ObjectTask prefix (base, +0x00) and the three words sndSeTransPlayObj
 * forwards to sndSePlay are evidenced (lw a0,28(s0); lw a1,32(s0);
 * lw a2,36(s0) at 0x00a2e128..0x00a2e134); the 8 bytes between them are read
 * by nothing this allocation touches and stay unmodelled.
 */
typedef struct {
    ObjectTask base;               /* +0x00 */
    unsigned char unmodeled_14[8]; /* +0x14 */
    BattleUnit *unit;              /* +0x1c: the unit the sound belongs to */
    int seId;                      /* +0x20: the sound-effect id */
    int pan;                       /* +0x24 */
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

int sndMuPlay(void);

#endif /* SRC_OV01_SND_H */
