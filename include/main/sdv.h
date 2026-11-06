#ifndef INCLUDE_MAIN_SDV_H
#define INCLUDE_MAIN_SDV_H

#include "shared.h"

typedef struct GamePlayerRecord {
    u8 unmodeled_00[0xa8];
} GamePlayerRecord;

/* Typed view of the zero-initialized serialized battle-data buffer.
 * The resource is loaded at runtime; it is not an ELF initializer. */
typedef struct BattleDataS32Parts {
    unsigned short low;
    unsigned short high;
} BattleDataS32Parts;

typedef union BattleDataS32 {
    BattleDataS32Parts parts;
    int value __attribute__((packed));
} BattleDataS32;

typedef struct BattleDataHeader {
    unsigned int sectionCount;
    unsigned int byteSize;
    int unitInitOffset;
    int techniqueOffset;
    int normalInitOffset;
    int specialInitOffset;
    int etherInitOffset;
    int weaponOffset;
    int ethOffset;
    int itmOffset;
    int attOffset;
    int accessoryOffset;
    int skillOffset;
    int gainOffset;
    int experienceOffset;
    int parameterOffset;
    int specialTableOffset;
    int defaultEquipmentOffset;
    int frameOffset;
    int engOffset;
} BattleDataHeader;

typedef struct BattleWeaponPriceRecord {
    unsigned char unmodeled_00[2];
    BattleDataS32 price;
    unsigned char unmodeled_06[2];
    unsigned short characterEquipMask;
    unsigned char unmodeled_0a[2];
    unsigned short categoryMask;
    unsigned char mountMask;
    unsigned char unmodeled_0f[2];
    unsigned char equipWeight;
    short wagl;
    short techniqueId;
    unsigned char unmodeled_16[10];
} BattleWeaponPriceRecord;

typedef struct BattleItemPriceRecord {
    BattleDataS32 price;
    unsigned char unmodeled_04[16];
} BattleItemPriceRecord;

typedef struct BattleBulletPriceRecord {
    unsigned char unmodeled_00[6];
    BattleDataS32 price;
    unsigned char unmodeled_0a[2];
    short weaponId;
} BattleBulletPriceRecord;

typedef struct BattleAccessoryPriceRecord {
    BattleDataS32 price;
    unsigned char unmodeled_04[2];
    unsigned short equipMask;
    unsigned char unmodeled_08[2];
    unsigned short category;
    unsigned char unmodeled_0c[2];
    unsigned short associatedId;
} BattleAccessoryPriceRecord;

typedef struct BattleFramePriceRecord {
    BattleDataS32 price;
    unsigned char unmodeled_04[2];
    unsigned short equipMask;
    unsigned char unmodeled_08[2];
    unsigned short category;
    unsigned char unmodeled_0c[2];
    unsigned short associatedId;
} BattleFramePriceRecord;

typedef struct BattleEnginePriceRecord {
    BattleDataS32 price;
    unsigned char unmodeled_04[2];
    unsigned short equipMask;
    unsigned char unmodeled_08[4];
} BattleEnginePriceRecord;

typedef struct BattleParameterRecord {
    BattleDataS32 firstParameterPair;
    unsigned char unmodeled_04[28];
} BattleParameterRecord;

typedef struct BattleExperienceAndParameters {
    BattleDataS32 experience[99];
    BattleParameterRecord parameters[16];
} BattleExperienceAndParameters;

typedef struct BattleDataStorage {
    BattleDataHeader header;
    unsigned char unitInitBytes[0x25c8];
    unsigned char techniqueBytes[0x51a8];
    unsigned char normalInitBytes[0x08b8];
    unsigned char specialInitBytes[0x00c0];
    unsigned char etherInitBytes[0x1170];
    BattleWeaponPriceRecord weapons[115];
    unsigned char ethBytes[0x0858];
    BattleItemPriceRecord items[72];
    BattleBulletPriceRecord bullets[178];
    BattleAccessoryPriceRecord accessories[180];
    unsigned char skillBytes[0x02d0];
    unsigned char gainBytes[0x0c08];
    BattleExperienceAndParameters levelData;
    unsigned char specialTableBytes[0x0200];
    unsigned char defaultEquipmentBytes[0x0400];
    BattleFramePriceRecord frames[41];
    BattleEnginePriceRecord engines[40];
    unsigned char unusedCapacity[0x1f30];
} BattleDataStorage;

typedef char BattleDataHeaderSizeCheck[sizeof(BattleDataHeader) == 0x50 ? 1 : -1];

typedef char BattleDataS32SizeCheck[
    sizeof(BattleDataS32) == 4 && __alignof__(BattleDataS32) == 2 ? 1 : -1];

typedef char BattleWeaponPriceRecordSizeCheck[sizeof(BattleWeaponPriceRecord) == 0x20 ? 1 : -1];

typedef char BattleItemPriceRecordSizeCheck[sizeof(BattleItemPriceRecord) == 0x14 ? 1 : -1];

typedef char BattleBulletPriceRecordSizeCheck[sizeof(BattleBulletPriceRecord) == 0x0e ? 1 : -1];

typedef char BattleAccessoryPriceRecordSizeCheck[sizeof(BattleAccessoryPriceRecord) == 0x10 ? 1 : -1];

typedef char BattleFramePriceRecordSizeCheck[sizeof(BattleFramePriceRecord) == 0x10 ? 1 : -1];

typedef char BattleEnginePriceRecordSizeCheck[sizeof(BattleEnginePriceRecord) == 0x0c ? 1 : -1];

typedef char BattleParameterRecordSizeCheck[sizeof(BattleParameterRecord) == 0x20 ? 1 : -1];

typedef char BattleLevelDataSizeCheck[sizeof(BattleExperienceAndParameters) == 0x38c ? 1 : -1];

typedef char BattleDataStorageSizeCheck[sizeof(BattleDataStorage) == 0x10000 ? 1 : -1];

/* Functions and data used by other translation units. */
extern BattleDataStorage batDatBuf;
/* moneyBox: 8-byte .sdata counter at 0x004dbb68 (sd -16392(gp)). */
extern unsigned long long moneyBox;
extern const Vector4 McMathUnitVector;
void sdvSetAmbState(int state, int effect_no);
void sdvSetAmbState2(int state, int effect_no);
void sdvClearSpecialWork(void);
void sdvInitAlters(void);
void sdvDestroyAlters(void);
void sdvExecAlters(void);

#endif /* INCLUDE_MAIN_SDV_H */
