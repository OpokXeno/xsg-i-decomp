/*
 * OV01 original TU 13: 0x00a2c9f8..0x00a2e4b0 (34 functions)
 */
#include "common.h"
#include "shared.h"
#include "snd.h"

#define SND_MU_VOLUME_MAX 0x7F
#define SND_MU_FADE_TIME  2000

INCLUDE_ASM("asm/nonmatchings/ov01/snd", sndInit);

INCLUDE_ASM("asm/nonmatchings/ov01/snd", sndInit2);

INCLUDE_ASM("asm/nonmatchings/ov01/snd", dataSndStEdNum);

INCLUDE_ASM("asm/nonmatchings/ov01/snd", dataSndStEdNumSub);

INCLUDE_ASM("asm/nonmatchings/ov01/snd", dataSndStEdNameGet);

INCLUDE_ASM("asm/nonmatchings/ov01/snd", dataSndSeNameGet);

/* ov01:0x00a2d1f8. Returns the address of sndSeDat (ov01:0x00a42d18, size
 * 0x68), the base of loaded sound-effect bank metadata; only its address is
 * evidenced by this allocation, not its internal layout. */
extern unsigned char sndSeDat[0x68];

void *sndSeAdrGet(void)
{
    return sndSeDat;
}

INCLUDE_ASM("asm/nonmatchings/ov01/snd", sndSeRegAdrChk);

INCLUDE_ASM("asm/nonmatchings/ov01/snd", sndSeRegAdrGet);

INCLUDE_ASM("asm/nonmatchings/ov01/snd", sndSeRegRemove);

INCLUDE_ASM("asm/nonmatchings/ov01/snd", sndSeRegChk);

#include "ov01/calc.h"

/* calcUPGet's return type comes from its definer, ov01/tu004 calc.c
 * (published include/ov01/calc.h); calcUPGet itself is still asm there, so
 * this TU declares it the same way ov01/tu002 unit_cmd.c does. */
extern CalcUnitParam *calcUPGet(ObjectTask *unit);

/*
 * dataSndSeLoadSub's output descriptor. Only the three words the function
 * itself touches are evidenced: seType (sw zero,0(s2) ov01:0x00a2d504; sw
 * s3,0(s2) ov01:0x00a2d574), dest (lw a1,4(s2) ov01:0x00a2d540, read only)
 * and size (sw a1,8(s2) ov01:0x00a2d570).
 */
typedef struct {
    int seType; /* +0x00: echoes the resolved sound-effect index; 0 on lookup failure */
    void *dest; /* +0x04: destination address dataFileLoadNB loads into */
    int size;   /* +0x08: file size rounded up to a 2048-byte CD sector */
} SndSeLoadWork;

extern int dataSndSeNameGet(char *name, ObjectTask *unit, int seType);
extern void dataFileLoadNB(void *buffer, void *address);
extern char *strcpy(char *destination, const char *source);
extern char *strcat(char *destination, const char *source);
extern int xglCdGetFileSize(const char *name);
extern int printf(const char *format, ...);
extern char fileName[];
extern char *sndSeNameBase;
extern char *sndSeNameExt;
extern const char D_00A4E6A8[];
extern const char D_00A4E6D0[];

int dataSndSeLoadSub(ObjectTask *unit, int seType, SndSeLoadWork *work)
{
    char name;
    int fileSize;
    int withSlack;
    int clamped;
    int roundedSize;

    dataSndSeNameGet(&name, unit, seType);
    if (name == 0) {
        printf(D_00A4E6A8, calcUPGet(unit)->charaId, seType);
        work->seType = 0;
        return 0;
    }

    strcpy(fileName, sndSeNameBase);
    strcat(fileName, &name);
    strcat(fileName, sndSeNameExt);
    dataFileLoadNB(fileName, work->dest);

    fileSize = xglCdGetFileSize(fileName);
    withSlack = fileSize + 0x7FF;
    clamped = (withSlack < 0) ? (fileSize + 0xFFE) : withSlack;
    roundedSize = (clamped >> 0xB) << 0xB;
    work->size = roundedSize;
    printf(D_00A4E6D0, roundedSize);
    work->seType = seType;
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/snd", dataSndSeLoad);

INCLUDE_ASM("asm/nonmatchings/ov01/snd", dataSndSeLoad2);

INCLUDE_ASM("asm/nonmatchings/ov01/snd", dataSndSeRegLoad);

INCLUDE_ASM("asm/nonmatchings/ov01/snd", dataSndSeRegLoad2);

INCLUDE_ASM("asm/nonmatchings/ov01/snd", sndSeTrans);

INCLUDE_ASM("asm/nonmatchings/ov01/snd", sndSysSePlay);

INCLUDE_ASM("asm/nonmatchings/ov01/snd", sndSePlay);

extern int sndBankGet(void);
extern void xglSoundEffectStopID(int sound_id, int flags);
extern const char D_00A4E810[];

int sndSeStop(int unused, int pack)
{
    int stopped = 0;

    if (pack != 0) {
        xglSoundEffectStopID(sndBankGet() << 0x10, 0);
        printf(D_00A4E810, pack);
        stopped = 1;
    }
    return stopped;
}

INCLUDE_ASM("asm/nonmatchings/ov01/snd", sndConvRegNo);

INCLUDE_ASM("asm/nonmatchings/ov01/snd", sndBankGet);

INCLUDE_ASM("asm/nonmatchings/ov01/snd", sndConvSpid);

extern int dataNormIdxGet(ObjectTask *unit, int seType);

/*
 * The battle actor object unit->work points at once loaded is the same
 * Actor record ov01/tu002 unit_cmd.c defines. A TU-local header cannot be
 * included from another TU, so this TU restates the same partial view under
 * the same name, matching src/main/chr.h, src/main/near_dir.h,
 * src/main/set_motion.h and src/ov01/unit_cmd.h. Only the flags header word
 * this function tests is evidenced here (lw straight off unit->work, & 0x40,
 * the same ACTOR_FLAG_ENEMY bit ov01/unit_cmd.h documents).
 */
typedef struct Actor {
    unsigned int flags;
} Actor;

#define ACTOR_FLAG_ENEMY 0x40u

/* ov01:0x00a2df80. Picks the sound-effect normal-hit-reaction index for a
 * non-enemy unit outside the boss charaId range (0xBB..0xC2), returning
 * 0x21 when that index selects one of the two heavier reaction categories
 * (3 or 5); every other case passes seType through unchanged (result 1). */
int sndConvSe(ObjectTask *unit, int mode, int seType)
{
    int idx;
    int result = 1;

    if (mode == 1
        && !(((Actor *)unit->work)->flags & ACTOR_FLAG_ENEMY)
        && !(calcUPGet(unit)->flags & ACTOR_FLAG_ENEMY)
        && (calcUPGet(unit)->charaId < 0xBB || calcUPGet(unit)->charaId >= 0xC3)) {
        idx = dataNormIdxGet(unit, seType);
        if (idx != -1) {
            if (idx == 3 || idx == 5) {
                result = 0x21;
            }
        }
    }
    return result;
}

INCLUDE_ASM("asm/nonmatchings/ov01/snd", sndConvHitSe);

extern void *objEntryPure();
extern int sndSeTrans(int seId, int volume);
void sndSeTransPlayObj(SndSePlayTask *task);

void sndSeTransPlay(int seId, int volume, int pan)
{
    SndSePlayTask *task;

    if (sndSeTrans(seId, volume) != 0) {
        task = objEntryPure(sndSeTransPlayObj);
        task->seId = seId;
        task->volume = volume;
        task->pan = pan;
    }
}

void sndSeTransPlayObj(SndSePlayTask *task)
{
    if (SsdSpuDmaCompleted(0) != 0) {
        return;
    }
    sndSePlay(task->seId, task->volume, task->pan);
    objRemovePure(&task->base);
}

INCLUDE_ASM("asm/nonmatchings/ov01/snd", dataSndMuLoad);

int sndMuTrans(int mode)
{
    if (mode == 0) {
        xglSoundSendSmd(sndMuDat.smdNormal);
        xglSoundSendSwd(sndMuDat.swd, -1);
    } else {
        xglSoundSendSmd(sndMuDat.smdAlt);
    }
    return 1;
}

int sndMuPlay(void)
{
    xglSoundSequenceNormal(SND_MU_VOLUME_MAX);
    return 1;
}

int sndMuStop(void)
{
    xglSoundSequenceFadeOut(SND_MU_FADE_TIME);
    return 1;
}

void sndMuFadeIn(void)
{
    xglSoundSequenceNormal3(0, SND_MU_VOLUME_MAX, SND_MU_FADE_TIME);
}

extern const char D_00A4E860[];
void sndMuTransPlayObj(ObjectTask *task);

void sndMuTransPlay(int mode)
{
    if (sndMuTrans(mode) != 0) {
        printf(D_00A4E860, mode);
        objEntryPure(sndMuTransPlayObj);
    }
}

void sndMuTransPlayObj(ObjectTask *task)
{
    if (SsdSpuDmaCompleted(0) != 0) {
        return;
    }
    sndMuPlay();
    objRemovePure(task);
}
