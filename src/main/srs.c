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

static SrsChrEfTbl3Entry _srsChrEfTbl3[36];

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
 * established by the available evidence. The mode word is local to this TU.
 */
void srsSetLoadMode(int load_mode)
{
    srsLoadMode = load_mode;
}

int srsGetLoadMode(void)
{
    return srsLoadMode;
}

static unsigned char srsViewPath[256];
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

static char msg_0_00795120[128];

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
    sprintf(msg_0_00795120, "esd/%s%s", name, ".esd");
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

static WeaponTblEntry _weaponTbl[45];

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

static char msg_1_007951A0[128];

char *srsGetEffectName2(int effectNo)
{
    char *name;

    name = srsGetEffectData(effectNo);
    if (name == 0) {
        return name;
    }
    sprintf(msg_1_007951A0, "esp/%s%s", name, ".esp");
    return msg_1_007951A0;
}

static char msg_2_00795220[128];

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
    sprintf(msg_2_00795220, "esp/%s%s", value, ".esp");
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


void sresInitMemoryRes(void)
{
    memset(&_srsMemRes, 0, sizeof(_srsMemRes));
}

/*
 * _srsMemRes's leading view needed by the two loaders below: the same +0x00
 * image pointer sresFreeMemoryRes owns, plus the +0x3C reloader-buffer
 * pointer sresLoadCfMemory allocates and svAddImageMapper maps.
 */

extern void *smAlloc(int size);

/*
 * svAddImageMapper takes the address of a chunk list; svAnalyzeChunk walks it
 * from its first entry, which is the only one these two loaders fill.
 */
extern void svAddImageMapper(int type, int width, void *chunk, int height);

void sresLoadCommonMemory(void)
{
    SrsMemRes *memRes;
    int size;
    void *chunk[8];

    memRes = &_srsMemRes;
    if (memRes->image == 0) {
        memRes->image = smAlloc(0x25800);
        if (memRes->image != 0) {
            size = fileLoad(memRes->image, "esp/default.esp", 0);
            if (size >= 0) {
                if (size <= 0x25800) {
                    chunk[0] = memRes->image;
                    svAddImageMapper(0, 0, chunk, 3000);
                }
            }
        }
        memset(_loadEsdData, 0, 0xC800);
        fileLoad(_loadEsdData, "seffect.esp", 0);
    }
}

void sresLoadCfMemory(void)
{
    SrsMemRes *memRes;
    int size;
    void *chunk[8];

    memRes = &_srsMemRes;
    if (memRes->cfImage == 0) {
        memRes->cfImage = smAlloc(0x3E000);
        if (memRes->cfImage != 0) {
        size = fileLoad(memRes->cfImage, "esp/cf_def.esp", 0);
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

void sresFreeReloaderMemoryNo(int no)
{
    if (no < 3) {
        if (_srsMemRes.weaponData[no][0] != 0) {
            svDeleteImageMapper(no * 3 + 2);
            smFree(_srsMemRes.weaponData[no][0]);
            _srsMemRes.weaponData[no][0] = 0;
            _srsMemRes.weaponData[no][1] = 0;
            _srsMemRes.weaponData[no][2] = 0;
        }
    } else {
        if (_srsMemRes.comboData[no - 3] != 0) {
            svDeleteImageMapper(no + 8);
            smFree(_srsMemRes.comboData[no - 3]);
            _srsMemRes.comboData[no - 3] = 0;
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

    if (_srsMemRes.battleImage != 0) {
        svDeleteImageMapper(14);
        smFree(_srsMemRes.battleImage);
        _srsMemRes.battleImage = 0;
    }
    for (i = 0; i < 3; i++) {
        if (_srsMemRes.comboData[i] != 0) {
            svDeleteImageMapper(i + 11);
            smFree(_srsMemRes.comboData[i]);
            _srsMemRes.comboData[i] = 0;
        }
    }
    for (i = 0; i < 3; i++) {
        if (_srsMemRes.weaponData[i][0] != 0) {
            svDeleteImageMapper(i * 3 + 2);
            svDeleteImageMapper(i * 3 + 3);
            svDeleteImageMapper(i * 3 + 4);
            smFree(_srsMemRes.weaponData[i][0]);
            _srsMemRes.weaponData[i][0] = 0;
            _srsMemRes.weaponData[i][1] = 0;
            _srsMemRes.weaponData[i][2] = 0;
            _srsMemRes.weaponNo[i][0] = -1;
            _srsMemRes.weaponNo[i][1] = -1;
            _srsMemRes.weaponNo[i][2] = -1;
        }
    }
    for (i = 0; i < 24; i++) {
        type = i + 16;
        if (_srsMemRes.effectData[i] != 0) {
            svDeleteImageMapper(type);
            smFree(_srsMemRes.effectData[i]);
            _srsMemRes.effectData[i] = 0;
            _srsMemRes.effectExtra[i] = 0;
            _srsMemRes.effectNo[i] = -1;
        }
    }
    if (freeCf != 0) {
        if (_srsMemRes.cfImage != 0) {
            svDeleteImageMapper(15);
            smFree(_srsMemRes.cfImage);
            _srsMemRes.cfImage = 0;
        }
    }
}


extern void smFree(void *block);
extern void sresFreeReloaderMemory(int reload_bgm);
extern void svDeleteImageMapper(int type);

void sresFreeMemoryRes(void)
{
    SrsMemRes *memRes;

    sresFreeReloaderMemory(1);
    memRes = &_srsMemRes;
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

    if (_srsMemRes.image != 0) {
        if (_srsMemRes.imagePending != 0) {
            chunk[0] = _srsMemRes.image;
            svAddImageMapper(0, 0, chunk, 3000);
            _srsMemRes.imagePending = 0;
        }
    }
    if (_srsMemRes.effectImage != 0) {
        if (_srsMemRes.effectImagePending != 0) {
            chunk[0] = _srsMemRes.effectImage;
            svAddImageMapper(1, 0, chunk, 3100);
            _srsMemRes.effectImagePending = 0;
        }
    }
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            if (_srsMemRes.weaponData[i][j] != 0) {
                pending = _srsMemRes.weaponPending[i][j];
                if (pending != 0) {
                    chunk[0] = _srsMemRes.weaponData[i][j];
                    svAddImageMapper(i * 3 + j + 2, pending, chunk, 2000);
                    _srsMemRes.weaponPending[i][j] = 0;
                }
            }
        }
    }
    for (i = 0; i < 3; i++) {
        type = i + 11;
        if (_srsMemRes.comboData[i] != 0) {
            pending = _srsMemRes.comboPending[i];
            if (pending != 0) {
                chunk[0] = _srsMemRes.comboData[i];
                svAddImageMapper(type, pending, chunk, 2100);
                _srsMemRes.comboPending[i] = 0;
            }
        }
    }
    if (_srsMemRes.battleImage != 0) {
        if (_srsMemRes.battleImagePending != 0) {
            chunk[0] = _srsMemRes.battleImage;
            svAddImageMapper(14, 0, chunk, 3400);
            _srsMemRes.battleImagePending = 0;
        }
    }
    if (_srsMemRes.cfImage != 0) {
        if (_srsMemRes.cfImagePending != 0) {
            chunk[0] = _srsMemRes.cfImage;
            svAddImageMapper(15, 0, chunk, 3500);
            _srsMemRes.cfImagePending = 0;
        }
    }
    for (i = 0; i < 24; i++) {
        type = i + 16;
        if (_srsMemRes.effectData[i] != 0) {
            pending = _srsMemRes.effectPending[i];
            if (pending != 0) {
                chunk[0] = _srsMemRes.effectData[i];
                svAddImageMapper(type, pending, chunk, 3600);
                _srsMemRes.effectPending[i] = 0;
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

/* Initialized resource tables and the original zero-filled work storage. */
static unsigned char srsViewPath[256] = {0};
extern char D_004CBFC0[];
extern char D_004CBFD0[];
extern char D_004CBFE0[];
extern char D_004CBFF0[];
extern char D_004CC000[];
extern char D_004CC010[];
extern char D_004CC020[];
extern char D_004CC030[];
extern char D_004CC040[];
extern char D_004CC050[];
extern char D_004CC060[];
extern char D_004CC070[];
extern char D_004CC080[];
extern char D_004CC090[];
extern char D_004CC0A0[];
extern char D_004CC0B0[];
extern char D_004CC0C0[];
extern char D_004CC0D0[];
extern char D_004CC0E0[];
extern char D_004CC0F0[];
extern char D_004CC100[];
extern char D_004CC110[];
extern char D_004CC120[];
extern char D_004CC130[];
extern char D_004CC140[];
extern char D_004CC150[];
extern char D_004CC160[];
extern char D_004CC170[];
extern char D_004CC180[];
extern char D_004CC190[];
extern char D_004CC1A0[];
extern char D_004CC1B0[];
extern char D_004CC1C0[];
extern char D_004CC1D0[];
extern char D_004CC1E0[];
extern char D_004CC1F0[];
extern char D_004CC200[];
extern char D_004CC210[];
extern char D_004CC220[];
extern char D_004CC230[];
extern char D_004CC240[];
extern char D_004CC250[];
extern char D_004CC260[];
extern char D_004CC270[];
extern char D_004CC280[];
extern char D_004CC290[];
extern char D_004CC2A0[];
extern char D_004CC2B0[];
extern char D_004CC2C0[];
extern char D_004CC2D0[];
extern char D_004CC2E0[];
extern char D_004CC2F0[];
extern char D_004CC300[];
extern char D_004CC310[];
extern char D_004CC320[];
extern char D_004CC330[];
extern char D_004CC340[];
extern char D_004CC350[];
extern char D_004CC360[];
extern char D_004CC370[];
extern char D_004CC380[];
extern char D_004CC390[];
extern char D_004CC3A0[];
extern char D_004CC3B0[];
extern char D_004CC3C0[];
extern char D_004CC3D0[];
extern char D_004CC3E0[];
extern char D_004CC3F0[];
extern char D_004CC400[];
extern char D_004CC410[];
extern char D_004CC420[];
extern char D_004CC430[];
extern char D_004CC440[];
extern char D_004CC450[];
extern char D_004CC460[];
extern char D_004CC470[];
extern char D_004CC480[];
extern char D_004CC490[];
extern char D_004CC4A0[];
extern char D_004CC4B0[];
extern char D_004CC4C0[];
extern char D_004CC4D0[];
extern char D_004CC4E0[];
extern char D_004CC4F0[];
extern char D_004CC500[];
extern char D_004CC510[];
extern char D_004CC520[];
extern char D_004CC530[];
extern char D_004CC540[];
extern char D_004CC550[];
extern char D_004CC560[];
extern char D_004CC570[];
extern char D_004CC580[];
extern char D_004CC590[];
extern char D_004CC5A0[];
extern char D_004CC5B0[];
extern char D_004CC5C0[];
extern char D_004CC5D0[];
extern char D_004CC5E0[];
extern char D_004CC5F0[];
extern char D_004CC600[];
extern char D_004CC610[];
extern char D_004CC620[];
extern char D_004CC630[];
extern char D_004CC640[];
extern char D_004CC650[];
extern char D_004CC660[];
extern char D_004CC670[];
extern char D_004DBAA0[];
extern char D_004DBAB8[];
extern char D_004DBAC0[];
extern char D_004DBAC8[];
extern char D_004DBAD0[];
extern char D_004DBAD8[];
extern char D_004DBAE0[];
extern char D_004DBAE8[];
extern char D_004DBAF0[];
extern char D_004DBAF8[];
extern char D_004DBB00[];
extern char D_004DBB08[];
extern char D_004DBB10[];
char *_loadComboData[195] = {
    D_004DBAA0, D_004DBB10, D_004DBB08, D_004DBB00, D_004DBAF8,
    D_004DBAF0, D_004DBAE8, D_004DBAE0, D_004DBAD8, D_004CC670,
    D_004DBAD0, D_004DBAC8, D_004DBAC0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, D_004CC660, D_004CC660,
    D_004CC650, D_004CC640, D_004CC630, D_004CC620, D_004CC610,
    D_004CC600, D_004CC5F0, D_004CC5E0, D_004CC5D0, D_004CC5C0,
    D_004CC5B0, D_004CC5A0, D_004CC590, D_004CC580, D_004CC570,
    D_004CC560, D_004CC550, D_004CC540, D_004CC530, D_004CC520,
    D_004CC510, D_004CC500, D_004CC4F0, D_004CC4E0, D_004CC660,
    D_004CC650, D_004CC640, D_004CC4D0, D_004CC4C0, D_004CC4B0,
    D_004CC4A0, D_004CC490, D_004CC480, D_004CC470, D_004CC460,
    D_004CC450, D_004CC440, D_004CC430, D_004CC420, D_004CC410,
    D_004CC400, D_004CC3F0, D_004CC3E0, D_004CC3D0, D_004CC3C0,
    D_004CC3B0, D_004CC3A0, D_004CC390, D_004CC380, D_004CC370,
    D_004CC360, D_004CC350, D_004CC340, D_004CC330, D_004CC320,
    D_004CC310, D_004CC300, D_004CC2F0, D_004CC2E0, D_004CC2D0,
    D_004CC2C0, D_004CC2B0, D_004CC2A0, D_004CC290, D_004CC280,
    D_004CC270, D_004CC260, D_004CC250, D_004CC240, D_004CC230,
    D_004CC220, D_004CC210, D_004CC200, D_004CC1F0, D_004CC1E0,
    D_004CC1D0, D_004CC3D0, D_004CC3D0, D_004CC390, D_004CC610,
    D_004CC4C0, D_004CC420, D_004CC410, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, D_004CC420, D_004CC420, D_004CC420, D_004CC420,
    D_004CC420, D_004CC1C0, D_004CC1B0, D_004CC1A0, D_004CC190,
    D_004CC180, D_004CC170, D_004CC160, D_004CC150, D_004CC140,
    D_004CC130, D_004CC120, D_004CC110, D_004CC100, D_004CC0F0,
    D_004CC0E0, D_004CC0D0, D_004CC0C0, D_004CC0B0, D_004CC0A0,
    D_004CC090, D_004CC080, D_004CC070, D_004CC060, D_004CC050,
    D_004CC040, D_004CC030, D_004CC020, D_004CC010, D_004CC000,
    D_004CBFF0, D_004CBFE0, D_004CBFD0, D_004CBFC0, D_004CC1A0,
    D_004CC190, D_004CC090, D_004DBAB8, D_004DBAB8, D_004DBAB8,
    D_004DBAB8, D_004DBAB8, D_004DBAB8, D_004DBAB8, D_004DBAB8,
};

static SrsChrEfTbl3Entry _srsChrEfTbl3[36] = {
    {151, 2681, {0x7B, 0x0A}},
    {152, 2684, {0x7E, 0x0A}},
    {153, 2687, {0x81, 0x0A}},
    {154, 2690, {0x84, 0x0A}},
    {155, 2693, {0x87, 0x0A}},
    {156, 2696, {0x8A, 0x0A}},
    {157, 2699, {0x8D, 0x0A}},
    {158, 2702, {0x90, 0x0A}},
    {159, 2705, {0x93, 0x0A}},
    {160, 2708, {0x96, 0x0A}},
    {161, 2711, {0x99, 0x0A}},
    {162, 2714, {0x9C, 0x0A}},
    {163, 2717, {0x9F, 0x0A}},
    {164, 2720, {0xA2, 0x0A}},
    {165, 2723, {0xA5, 0x0A}},
    {166, 2726, {0xA8, 0x0A}},
    {167, 2729, {0xAB, 0x0A}},
    {168, 2732, {0xAE, 0x0A}},
    {169, 2735, {0xB1, 0x0A}},
    {170, 2738, {0xB4, 0x0A}},
    {171, 2741, {0xB7, 0x0A}},
    {172, 2744, {0xBA, 0x0A}},
    {173, 2747, {0xBD, 0x0A}},
    {174, 2750, {0xC0, 0x0A}},
    {175, 2753, {0xC3, 0x0A}},
    {176, 2756, {0xC6, 0x0A}},
    {177, 2759, {0xC9, 0x0A}},
    {178, 2762, {0xCC, 0x0A}},
    {179, 2765, {0xCF, 0x0A}},
    {180, 2768, {0xD2, 0x0A}},
    {181, 2771, {0xD5, 0x0A}},
    {182, 2774, {0xD8, 0x0A}},
    {183, 2777, {0xDB, 0x0A}},
    {184, 2687, {0x81, 0x0A}},
    {185, 2690, {0x84, 0x0A}},
    {186, 2738, {0xB4, 0x0A}},
};

static WeaponTblEntry _weaponTbl[45] = {
    {70, 2801, {0xF2, 0x0A, 0xF1, 0x0A}},
    {71, 2803, {0xF3, 0x0A, 0xF4, 0x0A}},
    {72, 2805, {0xF6, 0x0A, 0xF7, 0x0A}},
    {73, 2808, {0x2E, 0x0B, 0x2F, 0x0B}},
    {74, 2809, {0xF9, 0x0A, 0xFA, 0x0A}},
    {75, 2811, {0xFC, 0x0A, 0xFD, 0x0A}},
    {76, 2814, {0x30, 0x0B, 0x31, 0x0B}},
    {77, 2815, {0xFF, 0x0A, 0x00, 0x0B}},
    {78, 2817, {0x01, 0x0B, 0x33, 0x0B}},
    {79, 2818, {0x02, 0x0B, 0x02, 0x0B}},
    {80, 2819, {0x03, 0x0B, 0x04, 0x0B}},
    {81, 2821, {0x05, 0x0B, 0x32, 0x0B}},
    {82, 2822, {0x06, 0x0B, 0x07, 0x0B}},
    {83, 2824, {0x08, 0x0B, 0x2D, 0x0B}},
    {84, 2825, {0x09, 0x0B, 0x09, 0x0B}},
    {85, 2826, {0x0A, 0x0B, 0x0B, 0x0B}},
    {86, 2828, {0x0C, 0x0B, 0x0D, 0x0B}},
    {87, 2830, {0x0E, 0x0B, 0x0F, 0x0B}},
    {88, 2832, {0x10, 0x0B, 0x10, 0x0B}},
    {89, 2833, {0x11, 0x0B, 0x12, 0x0B}},
    {90, 2835, {0x13, 0x0B, 0x14, 0x0B}},
    {91, 2837, {0x15, 0x0B, 0x16, 0x0B}},
    {112, 2837, {0x15, 0x0B, 0x16, 0x0B}},
    {92, 2839, {0x17, 0x0B, 0x18, 0x0B}},
    {113, 2839, {0x17, 0x0B, 0x18, 0x0B}},
    {93, 2841, {0x19, 0x0B, 0x19, 0x0B}},
    {94, 2842, {0x1A, 0x0B, 0x1A, 0x0B}},
    {114, 2842, {0x1A, 0x0B, 0x1A, 0x0B}},
    {95, 2843, {0x1B, 0x0B, 0x1B, 0x0B}},
    {96, 2844, {0x1C, 0x0B, 0x1C, 0x0B}},
    {97, 2845, {0x1D, 0x0B, 0x1D, 0x0B}},
    {98, 2846, {0x1E, 0x0B, 0x1E, 0x0B}},
    {99, 2847, {0x1F, 0x0B, 0x1F, 0x0B}},
    {100, 2848, {0x20, 0x0B, 0x20, 0x0B}},
    {101, 2849, {0x21, 0x0B, 0x21, 0x0B}},
    {102, 2850, {0x22, 0x0B, 0x22, 0x0B}},
    {103, 2851, {0x23, 0x0B, 0x23, 0x0B}},
    {104, 2852, {0x24, 0x0B, 0x24, 0x0B}},
    {105, 2853, {0x25, 0x0B, 0x25, 0x0B}},
    {108, 2854, {0x26, 0x0B, 0x26, 0x0B}},
    {110, 2855, {0x27, 0x0B, 0x27, 0x0B}},
    {106, 2856, {0x28, 0x0B, 0x28, 0x0B}},
    {107, 2857, {0x29, 0x0B, 0x29, 0x0B}},
    {111, 2858, {0x2A, 0x0B, 0x2A, 0x0B}},
    {109, 2859, {0x2B, 0x0B, 0x2C, 0x0B}},
};

SrsMemRes _srsMemRes = {0};
char _loadEsdData[0xC800] = {0};



char D_004DBAA0[8] = "default";

char D_004DBAC0[8] = "shelley";

char D_004DBAC8[8] = "mary";

char D_004DBAD0[8] = "virgil";

char D_004DBAD8[8] = "cecilia";

char D_004DBAE0[8] = "jr";

char D_004DBAE8[8] = "momo";

char D_004DBAF0[8] = "ziggy";

char D_004DBAF8[8] = "shitan";

char D_004DBB00[8] = "shion";

char D_004DBB08[8] = "kosmos";

char D_004DBB10[8] = "chaos";



char D_004DBAB8[8] = "";
