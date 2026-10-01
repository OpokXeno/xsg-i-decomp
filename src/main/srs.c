#include "common.h"
#include "shared.h"
#include "srs.h"

extern char _loadEsdData[]; /* 0xC800-byte effect-name table. */ /* 0xC800-byte table (config/symbols/main.txt) */

INCLUDE_ASM("asm/main/nonmatchings/srs", srsGetEffect2Idx);

extern int srsGetEffect2Idx(int effectNo);

char *srsGetEsdData(int index)
{
    int idx;

    if ((unsigned int) index >= 0xc00) {
        return 0;
    }
    idx = srsGetEffect2Idx(index);
    return _loadEsdData + (index - idx) * 0x10;
}

char *srsGetEsdData2(int effectNo)
{
    if ((unsigned int) effectNo >= 0xc00) {
        return 0;
    }
    return _loadEsdData + effectNo * 0x10;
}

char *sefGetEffectName(int effectNo)
{
    if ((unsigned int) effectNo >= 0xc00) {
        return 0;
    }
    return _loadEsdData + effectNo * 0x10;
}

INCLUDE_ASM("asm/main/nonmatchings/srs", srsAnalyzeEftNo);

/*
 * One _srsChrEfTbl3 row (6 bytes; the 0xD8-byte symbol holds 36 rows).
 * srsGetBossWaitEftNo scans effectNo and returns the matching bossWaitEftNo.
 */
typedef struct SrsChrEfTbl3Entry {
    short effectNo;
    short bossWaitEftNo;
    unsigned char unmodeled_04[2];
} SrsChrEfTbl3Entry;

extern SrsChrEfTbl3Entry _srsChrEfTbl3[];

short srsGetBossWaitEftNo(int effectNo)
{
    int i;

    if ((unsigned int) (effectNo - 151) < 36) {
        for (i = 0; i < 36; i++) {
            if (effectNo == _srsChrEfTbl3[i].effectNo) {
                return _srsChrEfTbl3[i].bossWaitEftNo;
            }
        }
    }
    return 0;
}

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

/*
 * 0x30C-byte table of 195 pointer-sized slots (config/symbols/main.txt).
 */
extern char *_loadComboData[];

char **srsGetComboData(int index)
{
    if (index >= 195) {
        return 0;
    }
    return (_loadComboData[index] != 0) ? &_loadComboData[index] : 0;
}

extern char msg_0_00795120[];
extern char D_004CC6A8[]; /* "esd/%s%s" */
extern char D_004DBB20[]; /* ".esd" */

char *srsGetEffectName(int effectNo)
{
    char *name;

    name = srsGetEffectData(effectNo);
    if (name == 0) {
        return name;
    }
    if (*name == 0) {
        return 0;
    }
    sprintf(msg_0_00795120, D_004CC6A8, name, D_004DBB20);
    return msg_0_00795120;
}

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

short srsWeapon2EffectID(int weaponID)
{
    int i;

    if (weaponID > 0) {
        for (i = 0; i < 45; i++) {
            if (weaponID == _weaponTbl[i].weaponID) {
                return _weaponTbl[i].weaponEffectID;
            }
        }
    }
    return -1;
}

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

extern char msg_1_007951A0[];
extern char D_004CC6B8[]; /* "esp/%s%s" */
extern char D_004DBB28[]; /* ".esp" */

char *srsGetEffectName2(int effectNo)
{
    char *name;

    name = srsGetEffectData(effectNo);
    if (name == 0) {
        return name;
    }
    sprintf(msg_1_007951A0, D_004CC6B8, name, D_004DBB28);
    return msg_1_007951A0;
}

extern char msg_2_00795220[];

static char *srsGetComboName(int index)
{
    char **entry;
    char *value;

    entry = srsGetComboData(index);
    if (entry == 0) {
        return 0;
    }
    value = *entry;
    if (value == 0) {
        return 0;
    }
    sprintf(msg_2_00795220, D_004CC6B8, value, D_004DBB28);
    return msg_2_00795220;
}

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

extern void sresDataMapping(void);

/*
 * The CD reader reports each finished request to this callback with the
 * request's own code; only three of the seven codes the dispatch covers
 * touch the outstanding-read count srsInitCdRead resets, and the count
 * reaching zero on code 4 maps the data that was just read.
 */
void srsCdReadCallback(int code)
{
    switch (code) {
    case 4:
        _nRead--;
        if (_nRead < 0) {
            _nRead = 0;
        }
        if (_nRead == 0) {
            sresDataMapping();
        }
        break;
    case -1:
        _nRead--;
        if (_nRead < 0) {
            _nRead = 0;
        }
        break;
    case -2: {
        int remaining;

        remaining = _nRead - 1;
        if (remaining < 0) {
            _nRead = 0;
        } else {
            _nRead = remaining;
        }
        break;
    }
    case 0:
    case 1:
    case 2:
    case 3:
        break;
    }
}

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

/*
 * _srsMemRes's leading view needed by the two loaders below: the same +0x00
 * image pointer sresFreeMemoryRes owns, plus the +0x3C reloader-buffer
 * pointer sresLoadCfMemory allocates and svAddImageMapper maps.
 */
typedef struct SrsMemResEarly {
    void *image; /* +0x00 */
    unsigned char unmodeled_04[0x38];
    void *cfImage; /* +0x3C */
} SrsMemResEarly;

extern void *smAlloc(int size);

/*
 * svAddImageMapper takes the address of a chunk list; svAnalyzeChunk walks it
 * from its first entry, which is the only one these two loaders fill.
 */
extern void svAddImageMapper(int type, int width, void *chunk, int height);
extern char D_004CC6F0[]; /* "esp/default.esp" */
extern char D_004CC700[]; /* "seffect.esp" */
extern char D_004CC710[]; /* "esp/cf_def.esp" */

void sresLoadCommonMemory(void)
{
    SrsMemResEarly *memRes;
    int size;
    void *chunk[8];

    memRes = (SrsMemResEarly *) _srsMemRes;
    if (memRes->image == 0) {
        memRes->image = smAlloc(0x25800);
        if (memRes->image != 0) {
            size = fileLoad(memRes->image, D_004CC6F0, 0);
            if (size >= 0) {
                if (size <= 0x25800) {
                    chunk[0] = memRes->image;
                    svAddImageMapper(0, 0, chunk, 3000);
                }
            }
        }
        memset(_loadEsdData, 0, 0xC800);
        fileLoad(_loadEsdData, D_004CC700, 0);
    }
}

void sresLoadCfMemory(void)
{
    SrsMemResEarly *memRes;
    int size;
    void *chunk[8];

    memRes = (SrsMemResEarly *) _srsMemRes;
    if (memRes->cfImage == 0) {
        memRes->cfImage = smAlloc(0x3E000);
        if (memRes->cfImage != 0) {
            size = fileLoad(memRes->cfImage, D_004CC710, 0);
            if (size >= 0) {
                chunk[0] = memRes->cfImage;
                svAddImageMapper(15, 0, chunk, 3500);
            }
        }
    }
}

extern void smFree(void *block);
extern void svDeleteImageMapper(int type);

/*
 * The reloader view of the same _srsMemRes record.  Its 0x1A0 bytes hold four
 * parallel blocks: one buffer pointer per resource, then a halfword per
 * resource that is set while the buffer still has to be handed to the image
 * mapper, then the effect number the buffer was loaded for, which the release
 * path sets back to -1, and last a second pointer per effect slot.  The mapper
 * type is fixed per block: the three weapon rows at +0x08 own three types each
 * from 2 up, the combo pointers at +0x2C own 11 to 13, the battle image 14,
 * the cf image 15 and the effect slots at +0x40 own 16 up.  A reloader slot
 * number below 3 names a weapon row and 3 or more a combo pointer.
 */
typedef struct SrsMemResMap {
    void *image; /* +0x000 */
    void *effectImage; /* +0x004 */
    void *weaponData[3][3]; /* +0x008 */
    void *comboData[3]; /* +0x02C */
    void *battleImage; /* +0x038 */
    void *cfImage; /* +0x03C */
    void *effectData[24]; /* +0x040 */
    short imagePending; /* +0x0A0 */
    short effectImagePending; /* +0x0A2 */
    short weaponPending[3][3]; /* +0x0A4 */
    short comboPending[3]; /* +0x0B6 */
    short battleImagePending; /* +0x0BC */
    short cfImagePending; /* +0x0BE */
    short effectPending[24]; /* +0x0C0 */
    short imageNo; /* +0x0F0 */
    short effectImageNo; /* +0x0F2 */
    short weaponNo[3][3]; /* +0x0F4 */
    short comboNo[3]; /* +0x106 */
    short battleImageNo; /* +0x10C */
    short cfImageNo; /* +0x10E */
    short effectNo[24]; /* +0x110 */
    void *effectExtra[24]; /* +0x140 */
} SrsMemResMap;

void sresFreeReloaderMemoryNo(int no)
{
    if (no < 3) {
        if (((SrsMemResMap *) _srsMemRes)->weaponData[no][0] != 0) {
            svDeleteImageMapper(no * 3 + 2);
            smFree(((SrsMemResMap *) _srsMemRes)->weaponData[no][0]);
            ((SrsMemResMap *) _srsMemRes)->weaponData[no][0] = 0;
            ((SrsMemResMap *) _srsMemRes)->weaponData[no][1] = 0;
            ((SrsMemResMap *) _srsMemRes)->weaponData[no][2] = 0;
        }
    } else {
        if (((SrsMemResMap *) _srsMemRes)->comboData[no - 3] != 0) {
            svDeleteImageMapper(no + 8);
            smFree(((SrsMemResMap *) _srsMemRes)->comboData[no - 3]);
            ((SrsMemResMap *) _srsMemRes)->comboData[no - 3] = 0;
        }
    }
}

extern void smFree(void *block);
extern void svDeleteImageMapper(int type);

/*
 * The reloader view of the same _srsMemRes record.  Its 0x1A0 bytes hold four
 * parallel blocks: one buffer pointer per resource, then a halfword per
 * resource that is set when the buffer still has to be handed to the image
 * mapper, then the effect number the buffer was loaded for, which the release
 * path sets back to -1, and last a second pointer per effect slot that is
 * released together with the first.  The mapper type is fixed per block: the
 * three weapon rows at +0x08 own three types each from 2 up, the combo
 * pointers at +0x2C own 11 to 13, the battle image 14, the cf image 15 and
 * the effect slots at +0x40 own 16 up.
 */
void sresFreeReloaderMemory(int freeCf)
{
    int i;
    int type;

    if (((SrsMemResMap *) _srsMemRes)->battleImage != 0) {
        svDeleteImageMapper(14);
        smFree(((SrsMemResMap *) _srsMemRes)->battleImage);
        ((SrsMemResMap *) _srsMemRes)->battleImage = 0;
    }
    for (i = 0; i < 3; i++) {
        if (((SrsMemResMap *) _srsMemRes)->comboData[i] != 0) {
            svDeleteImageMapper(i + 11);
            smFree(((SrsMemResMap *) _srsMemRes)->comboData[i]);
            ((SrsMemResMap *) _srsMemRes)->comboData[i] = 0;
        }
    }
    for (i = 0; i < 3; i++) {
        if (((SrsMemResMap *) _srsMemRes)->weaponData[i][0] != 0) {
            svDeleteImageMapper(i * 3 + 2);
            svDeleteImageMapper(i * 3 + 3);
            svDeleteImageMapper(i * 3 + 4);
            smFree(((SrsMemResMap *) _srsMemRes)->weaponData[i][0]);
            ((SrsMemResMap *) _srsMemRes)->weaponData[i][0] = 0;
            ((SrsMemResMap *) _srsMemRes)->weaponData[i][1] = 0;
            ((SrsMemResMap *) _srsMemRes)->weaponData[i][2] = 0;
            ((SrsMemResMap *) _srsMemRes)->weaponNo[i][0] = -1;
            ((SrsMemResMap *) _srsMemRes)->weaponNo[i][1] = -1;
            ((SrsMemResMap *) _srsMemRes)->weaponNo[i][2] = -1;
        }
    }
    for (i = 0; i < 24; i++) {
        type = i + 16;
        if (((SrsMemResMap *) _srsMemRes)->effectData[i] != 0) {
            svDeleteImageMapper(type);
            smFree(((SrsMemResMap *) _srsMemRes)->effectData[i]);
            ((SrsMemResMap *) _srsMemRes)->effectData[i] = 0;
            ((SrsMemResMap *) _srsMemRes)->effectExtra[i] = 0;
            ((SrsMemResMap *) _srsMemRes)->effectNo[i] = -1;
        }
    }
    if (freeCf != 0) {
        if (((SrsMemResMap *) _srsMemRes)->cfImage != 0) {
            svDeleteImageMapper(15);
            smFree(((SrsMemResMap *) _srsMemRes)->cfImage);
            ((SrsMemResMap *) _srsMemRes)->cfImage = 0;
        }
    }
}

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

/*
 * The reloader view of the same _srsMemRes record.  Its 0x1A0 bytes hold four
 * parallel blocks: one buffer pointer per resource, then a halfword per
 * resource that is set while the buffer still has to be handed to the image
 * mapper, then the effect number the buffer was loaded for, which the release
 * path sets back to -1, and last a second pointer per effect slot.  The mapper
 * type is fixed per block: the three weapon rows at +0x08 own three types each
 * from 2 up, the combo pointers at +0x2C own 11 to 13, the battle image 14,
 * the cf image 15 and the effect slots at +0x40 own 16 up.  A reloader slot
 * number below 3 names a weapon row and 3 or more a combo pointer.
 */
extern void svAddImageMapper(int type, int width, void *chunk, int height);

void sresDataMapping(void)
{
    void *chunk[8];
    int i;
    int j;
    int pending;
    int type;

    if (((SrsMemResMap *) _srsMemRes)->image != 0) {
        if (((SrsMemResMap *) _srsMemRes)->imagePending != 0) {
            chunk[0] = ((SrsMemResMap *) _srsMemRes)->image;
            svAddImageMapper(0, 0, chunk, 3000);
            ((SrsMemResMap *) _srsMemRes)->imagePending = 0;
        }
    }
    if (((SrsMemResMap *) _srsMemRes)->effectImage != 0) {
        if (((SrsMemResMap *) _srsMemRes)->effectImagePending != 0) {
            chunk[0] = ((SrsMemResMap *) _srsMemRes)->effectImage;
            svAddImageMapper(1, 0, chunk, 3100);
            ((SrsMemResMap *) _srsMemRes)->effectImagePending = 0;
        }
    }
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            if (((SrsMemResMap *) _srsMemRes)->weaponData[i][j] != 0) {
                pending = ((SrsMemResMap *) _srsMemRes)->weaponPending[i][j];
                if (pending != 0) {
                    chunk[0] = ((SrsMemResMap *) _srsMemRes)->weaponData[i][j];
                    svAddImageMapper(i * 3 + j + 2, pending, chunk, 2000);
                    ((SrsMemResMap *) _srsMemRes)->weaponPending[i][j] = 0;
                }
            }
        }
    }
    for (i = 0; i < 3; i++) {
        type = i + 11;
        if (((SrsMemResMap *) _srsMemRes)->comboData[i] != 0) {
            pending = ((SrsMemResMap *) _srsMemRes)->comboPending[i];
            if (pending != 0) {
                chunk[0] = ((SrsMemResMap *) _srsMemRes)->comboData[i];
                svAddImageMapper(type, pending, chunk, 2100);
                ((SrsMemResMap *) _srsMemRes)->comboPending[i] = 0;
            }
        }
    }
    if (((SrsMemResMap *) _srsMemRes)->battleImage != 0) {
        if (((SrsMemResMap *) _srsMemRes)->battleImagePending != 0) {
            chunk[0] = ((SrsMemResMap *) _srsMemRes)->battleImage;
            svAddImageMapper(14, 0, chunk, 3400);
            ((SrsMemResMap *) _srsMemRes)->battleImagePending = 0;
        }
    }
    if (((SrsMemResMap *) _srsMemRes)->cfImage != 0) {
        if (((SrsMemResMap *) _srsMemRes)->cfImagePending != 0) {
            chunk[0] = ((SrsMemResMap *) _srsMemRes)->cfImage;
            svAddImageMapper(15, 0, chunk, 3500);
            ((SrsMemResMap *) _srsMemRes)->cfImagePending = 0;
        }
    }
    for (i = 0; i < 24; i++) {
        type = i + 16;
        if (((SrsMemResMap *) _srsMemRes)->effectData[i] != 0) {
            pending = ((SrsMemResMap *) _srsMemRes)->effectPending[i];
            if (pending != 0) {
                chunk[0] = ((SrsMemResMap *) _srsMemRes)->effectData[i];
                svAddImageMapper(type, pending, chunk, 3600);
                ((SrsMemResMap *) _srsMemRes)->effectPending[i] = 0;
            }
        }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/srs", sresLoadBattleData);

int srsFileLoad(void *buffer, const char *name, int mode)
{
    return fileLoad(buffer, name, mode);
}

INCLUDE_ASM("asm/main/nonmatchings/srs", srsFileLoadCf);

INCLUDE_ASM("asm/main/nonmatchings/srs", srsMemoryLoadCf);

extern int strcmp(const char *s1, const char *s2);

int srsEffectNameToID(char *effectName)
{
    int index;
    char *name;

    if (effectName == 0) {
        return 0;
    }
    for (index = 1; index < 3072; index++) {
        name = srsGetEffectData(index);
        if (name != 0 && strcmp(effectName, name) == 0) {
            return index;
        }
    }
    return 0;
}
