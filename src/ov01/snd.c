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

INCLUDE_ASM("asm/nonmatchings/ov01/snd", dataSndSeLoadSub);

INCLUDE_ASM("asm/nonmatchings/ov01/snd", dataSndSeLoad);

INCLUDE_ASM("asm/nonmatchings/ov01/snd", dataSndSeLoad2);

INCLUDE_ASM("asm/nonmatchings/ov01/snd", dataSndSeRegLoad);

INCLUDE_ASM("asm/nonmatchings/ov01/snd", dataSndSeRegLoad2);

INCLUDE_ASM("asm/nonmatchings/ov01/snd", sndSeTrans);

INCLUDE_ASM("asm/nonmatchings/ov01/snd", sndSysSePlay);

INCLUDE_ASM("asm/nonmatchings/ov01/snd", sndSePlay);

INCLUDE_ASM("asm/nonmatchings/ov01/snd", sndSeStop);

INCLUDE_ASM("asm/nonmatchings/ov01/snd", sndConvRegNo);

INCLUDE_ASM("asm/nonmatchings/ov01/snd", sndBankGet);

INCLUDE_ASM("asm/nonmatchings/ov01/snd", sndConvSpid);

#include "ov01/calc.h"

/* calcUPGet's return type comes from its definer, ov01/tu004 calc.c
 * (published include/ov01/calc.h); calcUPGet itself is still asm there, so
 * this TU declares it the same way ov01/tu002 unit_cmd.c does. */
extern CalcUnitParam *calcUPGet(ObjectTask *unit);
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

INCLUDE_ASM("asm/nonmatchings/ov01/snd", sndSeTransPlay);

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

INCLUDE_ASM("asm/nonmatchings/ov01/snd", sndMuTransPlay);

void sndMuTransPlayObj(ObjectTask *task)
{
    if (SsdSpuDmaCompleted(0) != 0) {
        return;
    }
    sndMuPlay();
    objRemovePure(task);
}
