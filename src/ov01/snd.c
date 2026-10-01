/*
 * OV01 original TU 13: 0x00a2c9f8..0x00a2e4b0 (34 functions)
 */
#include "common.h"
#include "shared.h"
#include "snd.h"

extern int dataFileLoadNB(const char *filename, void *destination);

#define SND_MU_VOLUME_MAX 0x7F
#define SND_MU_FADE_TIME  2000

extern int sndBankGet();

INCLUDE_ASM("asm/nonmatchings/ov01/snd", sndInit);

extern int dmgBankOffs[3];
extern int dmgBankStat[3];
extern int ctrlId;

void sndInit2(void)
{
    int i;

    for (i = 0; i < 3; i++) {
        dmgBankOffs[i] = 0;
        dmgBankStat[i] = 0;
    }
    ctrlId = 0;
}

typedef struct SndStEdData {
    int unitId;
    const char *name;
    int condition[8];
} SndStEdData;

extern int dataSndStEdNumSub(ObjectTask *unit, int condition);
extern int rnd(int maximum);

/* The caller selects btst/bted records; every live record starts with the
 * unconditional condition 1, so at least one candidate is always written. */
int dataSndStEdNum(SndStEdData *data, ObjectTask *unit)
{
    int candidate[8];
    int index;
    int count;
    int *conditions;
    int *condition;

    conditions = data->condition;
    count = 0;
    for (index = 0; index < 8; index++) {
        condition = &conditions[index];
        if (dataSndStEdNumSub(unit, *condition)) {
            if (*condition >= 7) {
                candidate[0] = index + 1;
                count = 1;
                break;
            }
            candidate[count] = index + 1;
            count++;
        }
    }
    return candidate[rnd(count - 1)];
}

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

typedef struct {
    SndSeLoadWork work;             /* +0x00 */
    unsigned char unmodeled_0c[8]; /* +0x0c */
} SndSeRegBank;
typedef struct {
    int id;               /* +0x00 */
    SndSeRegBank bank[5]; /* +0x04 */
} SndSeRegDat;
extern SndSeRegDat sndSeRegDat[6];

SndSeRegDat *sndSeRegAdrChk(int id)
{
    int i;

    for (i = 0; i < 6; i++) {
        if (sndSeRegDat[i].id == id) {
            return &sndSeRegDat[i];
        }
    }
    return 0;
}

SndSeRegDat *sndSeRegAdrChk(int id);

/*
 * ov01:0x00a2d248. Reuses an existing sound registration if one already
 * matches id; otherwise, for a character id below 0xBB, claims the first
 * free slot in the id's half of sndSeRegDat (0..2 below 0x21, 3..5
 * otherwise) and returns its address, or 0 if none is free or id is 0xBB or
 * above.
 */
SndSeRegDat *sndSeRegAdrGet(int id)
{
    SndSeRegDat *found;
    int i;
    int start;
    int end;

    found = sndSeRegAdrChk(id);
    if (found != 0) {
        return found;
    }
    if (id >= 0xBB) {
        return 0;
    }
    if (id < 0x21) {
        start = 0;
        end = 3;
    } else {
        start = 3;
        end = 6;
    }
    for (i = start; i < end; i++) {
        if (sndSeRegDat[i].id == 0) {
            sndSeRegDat[i].id = id;
            return &sndSeRegDat[i];
        }
    }
    return 0;
}

int sndSeRegRemove(int id)
{
    int i;
    int j;

    for (i = 0; i < 6; i++) {
        if (sndSeRegDat[i].id == id) {
            sndSeRegDat[i].id = 0;
            for (j = 4; j >= 0; j--) {
                sndSeRegDat[i].bank[j].work.seType = 0;
            }
            return 1;
        }
    }
    return 0;
}

typedef struct SndSeActorFlags {
    unsigned int flags;
} SndSeActorFlags;
#define SND_SE_ACTOR_FLAG_ENEMY 0x40u

/*
 * ov01:0x00a2d388. Reports whether the sound-effect index seType has a
 * registered bank for this unit: a player-side unit outside the boss charaId
 * range (0xBB..0xC2) registers 1..4 and 23, a boss or enemy unit in the
 * 0x97..0xBA charaId range registers 1 and 2, and every other enemy
 * registers 1..5.
 */
int sndSeRegChk(ObjectTask *unit, int seType)
{
    int registered = 0;

    if (!(((SndSeActorFlags *)unit->work)->flags & SND_SE_ACTOR_FLAG_ENEMY)
        && !(calcUPGet(unit)->flags & SND_SE_ACTOR_FLAG_ENEMY)
        && (calcUPGet(unit)->charaId < 0xBB || calcUPGet(unit)->charaId >= 0xC3)) {
        if (seType > 0 && (seType < 5 || seType == 23)) {
            registered = 1;
        }
    } else if (calcUPGet(unit)->charaId >= 0x97 && calcUPGet(unit)->charaId < 0xBB) {
        if (seType < 3) {
            if (seType > 0) {
                registered = 1;
            }
        }
    } else if (seType < 6) {
        registered = seType > 0;
    }
    return registered;
}

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


extern int dataSndSeNameGet(char *name, ObjectTask *unit, int seType);
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

extern const char D_00A4E708[];

/*
 * ov01:0x00a2d6a0. Claims a unit sound registration, loads its base bank,
 * then loads three AGWS-specific banks when applicable.
 */
int dataSndSeRegLoad(ObjectTask *unit)
{
    short charaId;
    SndSeRegDat *record;
    SndSeRegBank *bank;
    void *end;
    int seType;

    charaId = calcUPGet(unit)->charaId;
    if (sndSeRegAdrChk(charaId) != 0) {
        printf(D_00A4E708, charaId);
        return 1;
    }
    record = sndSeRegAdrGet(charaId);
    if (record == 0) {
        return 0;
    }
    bank = &record->bank[0];
    if (dataSndSeLoadSub(unit, 0, &bank->work) == 0) {
        return 0;
    }
    if (calcUPGet(unit)->flags & 0x40) {
        end = (unsigned char *)bank->work.dest + bank->work.size;
        for (seType = 3; seType < 6; seType++) {
            bank = &record->bank[seType - 1];
            bank->work.dest = end;
            if (dataSndSeLoadSub(unit, seType, &bank->work) == 0) {
                bank->work.size = 0;
                bank->work.seType = 0;
            }
            end = (unsigned char *)bank->work.dest + bank->work.size;
        }
    }
    record->id = charaId;
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/snd", dataSndSeRegLoad2);

INCLUDE_ASM("asm/nonmatchings/ov01/snd", sndSeTrans);

extern void xglSoundEffectNormalID(int sound_id, int variant);

/* ov01:0x00a2da80. Plays system sound identifiers below 0x26 on variant 0
 * and reports whether the identifier was valid. */
int sndSysSePlay(int soundId)
{
    if (soundId >= 0x26) {
        return 0;
    }
    xglSoundEffectNormalID(soundId, 0);
    return 1;
}

extern int ctrlId;
extern int dmgBankOffs[3];
extern void xglSoundEffectNormalID(int soundId, int ctrlId);
extern const char D_00A4E7E8[];

/*
 * ov01:0x00a2dab0. Starts sound-effect id on the unit's bank. sndBankGet
 * resolves that bank from the unit the sound belongs to, which reaches
 * sndSePlay as the word sndSeTransPlay stores in the deferred task
 * (lw a0,28(s0) ov01:0x00a2e128) and is handed straight on (no argument setup
 * at the jal, ov01:0x00a2dac8). A damage sound
 * (id 2) plays once per bank: the first one marks the bank in dmgBankOffs and
 * every later one is dropped, and it does not advance the control id the other
 * sounds count with.
 */
int sndSePlay(int unit, int id, int no)
{
    int bank;

    if (id == 0) {
        return 0;
    }
    bank = sndBankGet(unit, id);
    if (id == 2) {
        if (dmgBankOffs[bank] != 0) {
            return 0;
        }
        dmgBankOffs[bank] = 1;
    }
    xglSoundEffectNormalID((bank << 16) | no, ctrlId);
    printf(D_00A4E7E8, bank, id, no, ctrlId);
    if (id != 2) {
        ctrlId++;
    }
    return 1;
}

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

typedef struct {
    int key;   /* +0x00 */
    int value; /* +0x04 */
} SndConvRegPair;
typedef struct {
    SndConvRegPair pair[7];
} SndConvRegTable;
extern const SndConvRegTable D_00A4E828;

/*
 * ov01:0x00a2dbc8. Searches a sentinel-terminated sound conversion pair
 * table and returns its mapped register number, or minus one.
 */
int sndConvRegNo(int id)
{
    SndConvRegTable table;
    int i;

    table = D_00A4E828;
    for (i = 0; table.pair[i].key != 0; i++) {
        if (table.pair[i].key == id) {
            return table.pair[i].value;
        }
    }
    return -1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/snd", sndBankGet);

typedef struct {
    unsigned char unmodeled_00[0x11]; /* +0x00 */
    signed char category;             /* +0x11 */
    unsigned char unmodeled_12[4];    /* +0x12 */
    short spid;                       /* +0x16 */
} TecData;
typedef struct {
    unsigned char unmodeled_00[0x10]; /* +0x00 */
    short spid;                       /* +0x10 */
} ItmData;
extern TecData *dataTecGet(int tecId);
extern ItmData *dataItmGet(int itmId);
extern int dataSpecBaseGet(int charaId);
extern int dataBakpBaseGet(int charaId);
#define SND_SPID_NORMAL  3
#define SND_SPID_SPECIAL 9
#define SND_SPID_BREAK   17
#define SND_SPID_EVENT   26

/*
 * ov01:0x00a2de30. Resolves the sound-effect index a battle command plays:
 * a technique (mode 1) by its category, the technique's or the item's own
 * recorded index (modes 2 and 3) and a fixed one for mode 9; every other mode
 * has none.
 */
int sndConvSpid(ObjectTask *unit, int mode, int tecId, int itmId)
{
    short charaId;
    int spid = 0;
    int category;

    charaId = calcUPGet(unit)->charaId;
    switch (mode) {
    case 1:
        category = dataTecGet(tecId)->category;
        switch (category) {
        case 6:
            spid = (tecId - dataBakpBaseGet(charaId)) + SND_SPID_BREAK;
            break;
        case 2:
            spid = (tecId - dataSpecBaseGet(charaId)) + SND_SPID_SPECIAL;
            break;
        case 4:
        case 5:
            spid = itmId + SND_SPID_NORMAL;
            break;
        default:
            spid = dataNormIdxGet(unit, tecId) + SND_SPID_NORMAL;
            break;
        }
        break;
    case 9:
        spid = 2;
        break;
    case 2:
        spid = dataTecGet(tecId)->spid + SND_SPID_EVENT;
        break;
    case 3:
        spid = dataItmGet(itmId)->spid + SND_SPID_EVENT;
        break;
    }
    return spid;
}

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

/* ov01:0x00a2e050. Returns hit sound 0x22 for normal indices three or five
 * under mode one; otherwise returns two. */
int sndConvHitSe(ObjectTask *unit, int mode, int seType)
{
    int idx;
    int result = 2;

    if (mode == 1) {
        idx = dataNormIdxGet(unit, seType);
        if (idx != -1) {
            if (idx == 3 || idx == 5) {
                result = 0x22;
            }
        }
    }
    return result;
}

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

extern char *sndMuNameBase;
extern char *sndMuNameExt1;
extern char *sndMuNameExt2;
extern char *muName[3];
extern char *mu2Name[3];

/*
 * ov01:0x00a2e160. Loads the three sound files of music set mode into
 * sndMuDat: the normal sequence, the alternate one the set may not have, and
 * the wave table, each laid out right behind the previous one and its size
 * rounded up to a 2048-byte CD sector. Returns whether the set exists.
 */
int dataSndMuLoad(int mode)
{
    SndMuData *mu;
    int smdFileSize;
    int altFileSize;
    int swdFileSize;
    int smdSectorSize;
    int altSectorSize;
    int swdSectorSize;

    if (mode >= 3) {
        return 0;
    }
    mu = &sndMuDat;

    strcpy(fileName, sndMuNameBase);
    strcat(fileName, muName[mode]);
    strcat(fileName, sndMuNameExt1);
    dataFileLoadNB(fileName, mu->smdNormal);
    smdFileSize = xglCdGetFileSize(fileName);
    smdSectorSize = smdFileSize + 0x7FF;
    if (smdSectorSize < 0) {
        smdSectorSize = smdFileSize + 0xFFE;
    }
    smdSectorSize >>= 0xB;
    smdSectorSize <<= 0xB;
    mu->smdSize = smdSectorSize;
    mu->smdAlt = (unsigned char *)mu->smdNormal + smdSectorSize;

    if (mu2Name[mode] != 0) {
        strcpy(fileName, sndMuNameBase);
        strcat(fileName, mu2Name[mode]);
        strcat(fileName, sndMuNameExt1);
        dataFileLoadNB(fileName, mu->smdAlt);
        altFileSize = xglCdGetFileSize(fileName);
        altSectorSize = altFileSize + 0x7FF;
        if (altSectorSize < 0) {
            altSectorSize = altFileSize + 0xFFE;
        }
        altSectorSize >>= 0xB;
        altSectorSize <<= 0xB;
        mu->smdAltSize = altSectorSize;
    } else {
        mu->smdAltSize = 0;
    }
    mu->swd = (unsigned char *)mu->smdAlt + mu->smdAltSize;

    strcpy(fileName, sndMuNameBase);
    strcat(fileName, muName[mode]);
    strcat(fileName, sndMuNameExt2);
    dataFileLoadNB(fileName, mu->swd);
    swdFileSize = xglCdGetFileSize(fileName);
    mu->mode = mode;
    swdSectorSize = swdFileSize + 0x7FF;
    if (swdSectorSize < 0) {
        swdSectorSize = swdFileSize + 0xFFE;
    }
    swdSectorSize >>= 0xB;
    swdSectorSize <<= 0xB;
    mu->swdSize = swdSectorSize;
    return 1;
}

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
