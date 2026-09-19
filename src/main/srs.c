#include "common.h"
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

INCLUDE_ASM("asm/main/nonmatchings/srs", srsSetViewPath);

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

INCLUDE_ASM("asm/main/nonmatchings/srs", srsEftNoWeaponEffectID);

INCLUDE_ASM("asm/main/nonmatchings/srs", srsWeapon2EffectID);

INCLUDE_ASM("asm/main/nonmatchings/srs", srsEftNo2WeaponID);

INCLUDE_ASM("asm/main/nonmatchings/srs", srsGetEffectName2);

INCLUDE_ASM("asm/main/nonmatchings/srs", srsGetComboName);

INCLUDE_ASM("asm/main/nonmatchings/srs", srsGetEffectType);

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

INCLUDE_ASM("asm/main/nonmatchings/srs", sresInitMemoryRes);

INCLUDE_ASM("asm/main/nonmatchings/srs", sresLoadCommonMemory);

INCLUDE_ASM("asm/main/nonmatchings/srs", sresLoadCfMemory);

INCLUDE_ASM("asm/main/nonmatchings/srs", sresFreeReloaderMemoryNo);

INCLUDE_ASM("asm/main/nonmatchings/srs", sresFreeReloaderMemory);

INCLUDE_ASM("asm/main/nonmatchings/srs", sresFreeMemoryRes);

INCLUDE_ASM("asm/main/nonmatchings/srs", sresDataMapping);

INCLUDE_ASM("asm/main/nonmatchings/srs", sresLoadBattleData);

int srsFileLoad(void *buffer, const char *name, int mode)
{
    return fileLoad(buffer, name, mode);
}

INCLUDE_ASM("asm/main/nonmatchings/srs", srsFileLoadCf);

INCLUDE_ASM("asm/main/nonmatchings/srs", srsMemoryLoadCf);

INCLUDE_ASM("asm/main/nonmatchings/srs", srsEffectNameToID);
