#include "common.h"
#include "shared.h"
#include "srs.h"

INCLUDE_ASM("asm/main/nonmatchings/srs", srsGetEffect2Idx);

INCLUDE_ASM("asm/main/nonmatchings/srs", srsGetEsdData);

INCLUDE_ASM("asm/main/nonmatchings/srs", srsGetEsdData2);

INCLUDE_ASM("asm/main/nonmatchings/srs", sefGetEffectName);

INCLUDE_ASM("asm/main/nonmatchings/srs", srsAnalyzeEftNo);

INCLUDE_ASM("asm/main/nonmatchings/srs", srsGetBossWaitEftNo);

/*
 * The original srs prefix is retained; its historical expansion is not
 * established by the available evidence.  This partial translation unit
 * owns only the two GLOBAL accessors.  srsLoadMode remains the original
 * external .sdata word rather than a new definition here.
 */
void srsSetLoadMode(int load_mode)
{
    srsLoadMode = load_mode;
}

int srsGetLoadMode(void)
{
    return srsLoadMode;
}

extern unsigned char srsViewPath[];
extern unsigned char *strcpy(unsigned char *destination, const unsigned char *source);

void srsSetViewPath(unsigned char *path)
{
    strcpy(srsViewPath, path);
}

INCLUDE_ASM("asm/main/nonmatchings/srs", srsMakeFileName);

void srsChangeSeparator(char *path)
{
    int index;

    if (path != 0) {
        for (index = 0; path[index] != '\0'; index++) {
            if (path[index] == '/')
                path[index] = '\\';
        }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/srs", srsGetFileLen);

INCLUDE_ASM("asm/main/nonmatchings/srs", fileLoad);

char *srsGetEffectData(int index)
{
    return srsGetEsdData(index);
}

INCLUDE_ASM("asm/main/nonmatchings/srs", srsGetComboData);

INCLUDE_ASM("asm/main/nonmatchings/srs", srsGetEffectName);

INCLUDE_ASM("asm/main/nonmatchings/srs", srsGetWeaponEffectIdx);

extern int srsGetWeaponEffectIdx(void);

/*
 * One _weaponTbl row (8 bytes; the 0x168-byte symbol holds 45 rows).
 * srsEftNo2WeaponID reads weaponID at the row start; srsEftNoWeaponEffectID
 * reads weaponEffectID right after it.
 */
typedef struct WeaponTblEntry {
    short weaponID;
    short weaponEffectID;
    unsigned char unmodeled_04[4];
} WeaponTblEntry;

extern WeaponTblEntry _weaponTbl[];

short srsEftNoWeaponEffectID(void)
{
    int index;
    short weaponEffectID;

    index = srsGetWeaponEffectIdx();
    weaponEffectID = 0;
    if (index >= 0) {
        weaponEffectID = _weaponTbl[index].weaponEffectID;
    }
    return weaponEffectID;
}

INCLUDE_ASM("asm/main/nonmatchings/srs", srsWeapon2EffectID);

short srsEftNo2WeaponID(void)
{
    int index;
    short weaponID;

    index = srsGetWeaponEffectIdx();
    weaponID = 0;
    if (index >= 0) {
        weaponID = _weaponTbl[index].weaponID;
    }
    return weaponID;
}

INCLUDE_ASM("asm/main/nonmatchings/srs", srsGetEffectName2);

INCLUDE_ASM("asm/main/nonmatchings/srs", srsGetComboName);

/*
 * The ranges are disjoint, evidenced value windows over effectNo; no source
 * for their meaning is available beyond the compiled comparisons.
 */
int srsGetEffectType(int effectNo)
{
    int effectType;

    effectType = 0;
    if ((u32) (effectNo - 0xA28) >= 0x3E) {
        effectType = 2;
        if ((u32) (effectNo - 0x9C4) >= 0x64 && effectNo >= 0x64) {
            effectType = 0xE;
            if ((u32) (effectNo - 0x7D0) >= 0xDB) {
                effectType = 0xB;
                if ((u32) (effectNo - 0xF0) >= 0x9D) {
                    effectType = 0xE;
                    if ((u32) (effectNo - 0x8FC) >= 0xB6) {
                        effectType = 2;
                        if ((u32) (effectNo - 0xAF0) >= 0xC8) {
                            effectType = ((u32) (effectNo - 0x258) < 0x190) ? 0xF : 0xE;
                        }
                    }
                }
            }
        }
    }
    return effectType;
}

void srsInitCdRead(void)
{
    _nRead = 0;
}

int srsLeaveCdRead(void)
{
    return _nRead;
}

INCLUDE_ASM("asm/main/nonmatchings/srs", srsCdReadCallback);

int srsLoadEffectData(void *buffer, int effectNo)
{
    char *name;

    name = srsGetEffectName(effectNo);
    if (name == 0) {
        return -1;
    }
    return fileLoad(buffer, name, 1);
}

extern unsigned char _srsMemRes[];

void sresInitMemoryRes(void)
{
    memset(_srsMemRes, 0, 0x1A0);
}

INCLUDE_ASM("asm/main/nonmatchings/srs", sresLoadCommonMemory);

INCLUDE_ASM("asm/main/nonmatchings/srs", sresLoadCfMemory);

INCLUDE_ASM("asm/main/nonmatchings/srs", sresFreeReloaderMemoryNo);

INCLUDE_ASM("asm/main/nonmatchings/srs", sresFreeReloaderMemory);

/*
 * _srsMemRes's own additive view: only the leading pointer sresFreeMemoryRes
 * tests and clears is modeled, the rest of the 0x1A0-byte record (zeroed
 * whole by sresInitMemoryRes) stays an explicit unmodeled span.
 */
typedef struct SrsMemRes {
    void *image; /* +0x00 */
    unsigned char unmodeled_04[0x19c];
} SrsMemRes;

extern void smFree(void *block);
extern void sresFreeReloaderMemory(int reload_bgm);
extern void svDeleteImageMapper(int type);

void sresFreeMemoryRes(void)
{
    SrsMemRes *memRes;

    sresFreeReloaderMemory(1);
    memRes = (SrsMemRes *) _srsMemRes;
    if (memRes->image != 0) {
        svDeleteImageMapper(0);
        smFree(memRes->image);
        memRes->image = 0;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/srs", sresDataMapping);

INCLUDE_ASM("asm/main/nonmatchings/srs", sresLoadBattleData);

int srsFileLoad(void *buffer, const char *name, int mode)
{
    return fileLoad(buffer, name, mode);
}

INCLUDE_ASM("asm/main/nonmatchings/srs", srsFileLoadCf);

INCLUDE_ASM("asm/main/nonmatchings/srs", srsMemoryLoadCf);

INCLUDE_ASM("asm/main/nonmatchings/srs", srsEffectNameToID);
