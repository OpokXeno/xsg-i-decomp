/*
 * TU-local declarations of main/tu215 (src/main/sdv.c).
 */

#ifndef SRC_MAIN_SDV_H
#define SRC_MAIN_SDV_H

typedef struct GamePlayerRecord {
    u8 unmodeled_00[0xa8];
} GamePlayerRecord;

/* Private typed view of the zero-initialized serialized battle-data buffer.
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

typedef struct SdvKeyCursor {
    int position;
    int elapsed;
} SdvKeyCursor;

typedef struct SdvCameraChoice {
    int selected;
    int script_base;
    unsigned char unmodeled_08[8];
    int animation;
    unsigned char unmodeled_14[8];
    int camera_commands[4][3];
} SdvCameraChoice;

typedef short int16_t;

/*
 * Fresh Spark hypothesis F1: renamed partial camera-offset view.
 * Changed from the predecessor's SdvCameraOffset/offset_mode/offset_source
 * spelling: identical 48-byte layout and else-if branch shape, but distinct
 * type/field/local identifiers (SdvCamOffset/cam_kind/cam_vec, pos/ang) to
 * test the ABI saved-register hypothesis that local-alias lifetime spelling
 * keeps mode in s1 and source in s2. Unnamed 32-bit units preserve the
 * observed +0x10/+0x20 vector offsets and assert no historical semantics.
 */
typedef struct {
    int kind;
    unsigned int : 32;
    unsigned int : 32;
    unsigned int : 32;
    float pos[3];
    unsigned int : 32;
    float ang[3];
    unsigned int : 32;
} SdvCamOffset;

/*
 * One 12-byte channel of a camera schedule task: sdvScheduleCamera reads
 * only the last word (nonzero enables the channel for this frame) and hands
 * the whole 12 bytes, or a channel-specific prefix of it, to sdvProgressPrm.
 * The first 8 bytes are otherwise unread by sdvScheduleCamera.
 */
typedef struct SdvCameraPhase {
    SdvKeyCursor cursor;           /* +0x00 */
    const short *active;           /* +0x08 */
} SdvCameraPhase;

/*
 * A battle camera schedule task (sefExecScheduler's caller data): only the
 * fields sdvScheduleCamera reads are named.
 */
typedef struct SdvCameraTask {
    int active;                    /* +0x00 */
    unsigned char unmodeled_04[0x10];
    SdvCameraPhase pos;             /* +0x14 */
    SdvCameraPhase angle;           /* +0x20 */
    SdvCameraPhase scale;           /* +0x2C */
    SdvCameraPhase offset;          /* +0x38 */
} SdvCameraTask;

/* A screen-space point: x and y in 1/16 pixels before the offset, z the depth. */
typedef struct SdvScreenPoint {
    int x;
    int y;
    int z;
    int w;
} SdvScreenPoint;

/* The now-running scheduler: the matrix a special effect starts from at +0,
 * its camera matrix at +0x40 and the flag word at +0xa8c, bit 1 of which asks
 * for the actor offset to go through it. */
typedef struct SdvScheduler {
    Matrix4 base_matrix;
    Matrix4 matrix;
    unsigned char unmodeled_80[0xa8c - 0x80];
    int flags;
} SdvScheduler;

/* One 20-byte key of a special effect table. */
typedef struct SdvSpecialKey {
    unsigned short time;            /* +0x00, 1024 ends the table */
    unsigned short depth_step;      /* +0x02 */
    unsigned char color[4];         /* +0x04 */
    short x;                        /* +0x08 */
    short y;                        /* +0x0a */
    unsigned short width;           /* +0x0c */
    unsigned short height;          /* +0x0e */
    unsigned short intensity;       /* +0x10 */
    short z;                        /* +0x12 */
} SdvSpecialKey;

/* A special effect table: the kind of effect, then its keys. */
typedef struct SdvSpecialTable {
    short kind;                     /* +0x00 */
    short part;                     /* +0x02, the actor part an offset follows */
    SdvSpecialKey keys[1];          /* +0x04 */
} SdvSpecialTable;

/* One of the seven running special effects: the matrix it was started with,
 * the key it currently shows, its key table, the frame in it and the key
 * that frame belongs to. */
typedef struct SdvSpecialSlot {
    Matrix4 matrix;                 /* +0x00 */
    SdvSpecialKey key;              /* +0x40 */
    struct SdvSpecialTable *table;         /* +0x54 */
    int frame;                      /* +0x58 */
    int index;                      /* +0x5c */
} SdvSpecialSlot;

typedef struct SdvActorPosition {
    unsigned char unmodeled_00[16];
    Vector4 position;               /* +0x10 */
} SdvActorPosition;

/*
 * The 36-byte request that starts an alter: the actors the selectors 256 and
 * 512 name, the weapon the selectors from 1024 name, and the effect number
 * whose texture images the alter draws.
 */
typedef struct SdvAlterRequest {
    struct SdvActorPosition *source_actor; /* +0x00 */
    struct SdvActorPosition *target_actor; /* +0x04 */
    void *weapon;                   /* +0x08 */
    unsigned char unmodeled_0c[8];
    short effect_no;                /* +0x14 */
    unsigned char unmodeled_16[14];
} SdvAlterRequest;

/* RGBA channels share the original aligned 128-bit packet color. */
typedef unsigned int SdvPackedColor __attribute__((mode(TI)));

typedef union SdvQuad {
    SdvPackedColor quad;
    unsigned int word[4];
    unsigned long long dword[2];
} SdvQuad;

typedef struct SdvAlterParameters {
    SdvAlterRequest request;
    unsigned char unmodeled_24[12];
    SdvQuad rgba;                   /* +0x30 */
    unsigned long long tex0[2];     /* +0x40 */
    unsigned char unmodeled_50[4];
    float length;                   /* +0x54 */
    short selector[2];              /* +0x58 */
    short life;                     /* +0x5c */
    short max_samples;              /* +0x5e */
    unsigned short shrink;          /* +0x60 */
    short blend;                    /* +0x62 */
    short image_count;              /* +0x64 */
    short mode;                     /* +0x66 */
    unsigned short image_index;     /* +0x68 */
    short image_frames;             /* +0x6a */
    unsigned short image_timer;     /* +0x6c */
    short category;                 /* +0x6e */
} SdvAlterParameters;

/*
 * One 0x1280-byte alter record. Only the +0x1278 status halfword that
 * sdvInitAlters, sdvDestroyAlters and sdvExecAlters read is named.
 */
typedef struct SdvAlter {
    SdvAlterParameters parameters;
    Vector4 trail[2][128];          /* +0x70 */
    float level[128];               /* +0x1070 */
    float phase;
    unsigned short cursor;
    unsigned short samples;
    unsigned short active;          /* +0x1278 */
    unsigned short wrapped;         /* +0x127a */
    short serial;
    unsigned char unmodeled_127e[2];
} SdvAlter;

/* The GIF packet accumulator of the SGs* family: a growing array of 16-byte
 * records at `base`, `count` records long; `tag_index` is the record the open
 * GIF tag sits in and `loop_count` is what closing it ORs into its NLOOP. */
typedef struct SdvGifPacket {
    int count;                      /* +0x00 */
    int tag_index;                  /* +0x04 */
    int loop_count;                 /* +0x08 */
    void *base;                     /* +0x0c */
} SdvGifPacket;

extern BattleDataStorage batDatBuf;
extern const Vector4 McMathUnitVector;
extern int _sdvAmbFrame;
extern int _sdvAmbState;
extern unsigned char charID_0;
extern int eftCate_1;
extern short _sefBattleMode;
extern short _sdvSpecialBuf[8];
extern SdvGifPacket _packet;

/* These typed caller declarations agree with the defining sc_get and sef
 * functions; their generated owner headers do not yet export these names. */
extern int scGetImmAdrImmIdx2(int base, unsigned short *table, int index);

extern short scGetImmNumIdx(short *table, int index);
extern int sefIsEntryBoss(void);
extern int rand(void);
static void sdvSetAmbStateSub(int state, int effect_no, int force);
extern void *xglStudioGetLight2(void);
extern void xglLightIntensityAmbient(void *light, void *ambient);
extern void func_A2C3D8(void *map_rgb);
extern void xglSoundEffectNormalID(int sound_id, int variant);

extern int srsAnalyzeEftNo(int effectId, unsigned char *charId,
                            int *effectCategory);

int sdvProgressPrm(int kind, void *data, int size);
extern void func_A31920(int kind, float *params);
void sdvScheduleCamera(SdvCameraTask *task);
extern void GameDefocusSet(int index, int, const int *);
void sdvClearSpecialWork(void);
int sdvCreateAlter(SdvAlterParameters *parameters);
void sdvInitAlter(SdvAlter *alter);
void sdvDestroyAlter(SdvAlter *alter);
void sdvExecAlter(SdvAlter *alter);
void sdvDrawAlter(SdvAlter *alter, SdvGifPacket *packet);
extern void SGsInitEnv(void);
void sdvInitAlters(void);
void sdvDestroyAlters(void);
void sdvExecAlters(void);

#endif /* SRC_MAIN_SDV_H */
