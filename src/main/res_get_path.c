#include "common.h"
#include "main/xgl_mc.h"
#include "main/maptbl.h"
#include "main/xgl_font.h"
#include "main/xgl_sound.h"
#include "main/look_at_player.h"
#include "main/res_get_path.h"
#include "shared.h"
#include "main/xgl_thread.h"
#include "main/act_1.h"
#include "main/enemy_1.h"
#include "main/game_movie.h"
#include "main/ssd_2.h"
#include "main/ssd_init.h"
#include "main/xgl_2.h"
#include "main/xgl_cd.h"
#include "main/game.h"


/* Scene resource script. */
#define SCENE_PATH_COUNT 20           /* tbl_0_00366420 directories */
#define RES_PATH_MAX 0x100            /* path and name buffers */
#define SCENE_TXT_SIZE 0x800          /* scene script buffer */
#define SCENE_TXT_SIGNATURE 0x6E637323 /* "#scn" */
#define SCENE_TXT_EMPTY_FILL 0xCDCDCDCD /* signature value before the script is read */
#define ARX_SIGNATURE 0x00585241      /* "ARX\0" compressed resource */
#define ARC_HEADER_WORDS 0x40
#define RES_WORK_BASE 0x0112A000      /* first resource work address */

/* Resource id layout: scene (0x0fffffff), model slot (0x1000 per root entry),
 * alias group (0x100 per child) and the face flag. */
#define RES_SCENE_ID_MASK 0x0fffffff
#define RES_ID_MODEL_STEP 0x1000
#define RES_ID_GROUP_STEP 0x100
#define RES_ID_FACE 0x10000
#define RES_ID_FACE_MASK 0xFFFF0000
#define RES_ID_MOTION_BASE_MAX 0x02FFFFFF /* motion ids up to here are mapped to a base pack */
#define RES_ID_MOTION_NAMED_TAG 0x03000000

/* Sound banks written by the resource script. */
#define SOUND_BANK_FOOT 2         /* leader foot-step effects (loadse) */
#define SOUND_BANK_ENV 3          /* map environment effects (loadse) */
#define SOUND_BANK_ENEMY_BASE 4   /* enemy/NPC effect banks 4 .. 11 */
#define ENEMY_SE_BANK_COUNT 8
#define ENEMY_SE_BANK_STRIDE 0x10000
#define ENEMY_SE_BANK_ADDRESS_BASE 0x40000

/* EnemySeBank.type, selected by the command name. */
enum EnemySeKind {
    ENEMY_SE_ENEMY = 0,
    ENEMY_SE_NPC = 1
};

/* GameMpeg2Play mode. */
enum Mpeg2Mode {
    MPEG2_MODE_BATTLE = 1,
    MPEG2_MODE_NORMAL = 2,
    MPEG2_MODE_NOFADE = 5
};

#define MOTION_ID_MAX 14

typedef struct {
    const char *name;
    u16 ids[MOTION_ID_MAX];
} MotionEntry;

typedef struct {
    u16 *table;
    u16 count;
    u16 group;
} FaceEntry;

typedef struct {
    u16 id;
    u8 type;
    u8 foot;
    char name[16];
} EnemySeEntry;

/* search_key walks this object in 12-byte records: it compares the first
 * string with the requested key and walks the string-pointer list at +4.
 * search_key does not read the third pointer (path_prefix), which
 * RES_GetMdlFileNameSub uses. */
typedef struct ResourceKeyEntry {
    const char *key;
    union {
        const char **names;               /* CIDn, MU, ... lists of alias names */
        struct ResourceKeyEntry *children; /* model: the key table nested under it */
    } aliases;
    const char *path_prefix;
} ResourceKeyEntry;

/*
 * The game loop's state record, as far as this TU reads it: the flags word
 * that command_player_lock sets a bit in, and the scene id that
 * GameResourceWorkReload reloads. The other bytes are not modelled. The
 * object is defined outside this file.
 */
typedef struct {
    u8 unmodeled_00[0x10];
    u32 flags;      /* +0x10 */
    u8 unmodeled_14[4];
    int scene_id;   /* +0x18 */
    u8 unmodeled_1c[0x34];
    s16 map_id;
    u16 unmodeled_52;
    void *map_load_result;
} GameLoopStatePrefix;


typedef struct {
    const char *name;
    char *(*callback)(int, char *);
    void *unmodeled_08;
} GameResourceCommand;

struct SaveDataLeaderSeIndex {
    u8 unmodeled_00[0x100A4];
    u16 leader_se_index;
};

typedef struct {
    u8 unmodeled_00[14];
    u16 sequence_id;
} SoundLoadBuffer;

typedef struct {
    float (*matrix)[4];
    void *lex_file;
    void *texture_file;
    void *id_file;
    void *think_file;
    u8 unmodeled_14[12];
    float matrix_storage[4][4];
} SubMapLoadState;

typedef struct {
    void *first_file;
    void *second_file;
    void *third_file;
    u8 unmodeled_0c[4];
} ResourceFileSet;

extern MotionEntry cfMtnList[31];
extern FaceEntry faceTbl[7];
extern const char *AID0[];
extern const char *CID0[];
extern const char *CID1[];
extern const char *CID2[];
extern const char *CID3[];
extern const char *CID4[];
extern const char *CID5[];
extern const char *CID6[];
extern const char *EID0[];
extern const char *EID1[];
extern const char *EID2[];
extern const char *EID3[];
extern const char *EID4[];
extern const char *EID5[];
extern const char *EID6[];
extern const char *EID7[];
extern const char *EID8[];
extern const char *EIDf[];
extern const char *MID0[];
extern const char *MU[];
extern const char *MUF[];
extern const char *MUS[];
extern const char *MUW[];
extern const char *MUX[];
extern const char *RID0[];
extern const char *RID1[];
extern const char *RID2[];
extern const char *TRE[];
extern const char *WID0[];
extern const char *WID1[];
extern const char *WID2[];
extern const char *WID3[];
extern const char *WID4[];
extern const char *WID5[];
extern const char *WID6[];
extern const char *WID7[];
extern const char *WID8[];
extern GameLoopStatePrefix GameLoopState;
extern const char soundSwdPathFormat[];
extern const char soundSedPathFormat[];
extern const char soundSmdPathFormat[];
extern const char lexSuffix[];
extern const char xtxSuffix[];
extern const char idSuffix[];
extern const char faceSuffix[];
extern const char resourceDumpRowFormat[];
extern const char jntSuffix[];
extern const char motionFpkPathFormat[];
extern const char ResZ8Name[8];
extern const char ResCSName[8];
extern const char ResMOName[8];
extern const char ResKMName[8];
extern const char ResSOName[8];
extern const char ResJRName[8];

/* Original objects of other TUs referenced by this TU's initialized DATA. */
extern const char ResShionCfName[16];
extern const char ResKosmosCfName[16];
extern const char ResChaosCfName[16];
extern const char ResMomoCfName[8];
extern const char ResJrCfName[8];
extern const char ResZiggyCfName[16];
extern const char ResShitanCfName[16];
extern const char ResAllenCfName[16];
extern const char ResOldwCfName[8];
extern const char ResOldmCfName[8];
extern const char ResMiddlemCfName[16];
extern const char ResMiddlewCfName[16];
extern const char ResSoldierwCfName[16];
extern const char ResSoldiermCfName[16];
extern const char ResWomanCfName[16];
extern const char ResYoungmCfName[16];
extern const char ResYoungwCfName[16];
extern const char ResGuno001CfName[16];
extern const char ResGuno301Name[16];
extern const char ResGuno302Name[16];
extern const char ResGuno303Name[16];
extern const char ResGuno304Name[16];
extern const char ResGuno305Name[16];
extern const char ResGuno323Name[16];
extern const char ResGuno324Name[16];
extern const char ResGuno325Name[16];
extern const char ResGuno307Name[16];
static char *command_enenpcse(int command, char *cursor, int mode);
extern int GameDiskChange(int disk_number);
extern void Intermission(int save_number);
extern void SCRIPT_fade(int time);
static char *command_mpeg2_core(int command, char *cursor, int mode);
static int next_arc_size(void *address);
extern void Enemy_LoadPreset(void *address, const char *name);
extern void ACT_init(void);
extern void MapChangeFadeSet(void);
/* A nonzero `flags` is a loader called as (resource type, resource id or name, slot address). */
void *RES_loadFile(int command, int callback, int resource_id, int flags);
static char *search_key(char *cursor, ResourceKeyEntry *table, int *id);
extern int sefLoadEffectCfName(int cf_id, char *effect_name);
extern int sefLoadMemoryEffectCfName(void *data, char *effect_name, int size);
extern void GameCfPlayerLoadResource(int resource_set);
extern void SCRIPT_frameLock2Battle(void);
void *GameResourceRealloc(void *address, int size);
extern void MapReset(void);
extern void xglSoundLoadEffect(const char *name, void *address, int bank);
extern int SsdCheckPlayEffectFileID(u16 file_id);
extern void xglSoundSendSmd(void *address);
extern char *MAP_getPath(char *name);
extern char *MapGetName(int scene_id);
extern int func_A1CF18(char *path, int scene_id);
int GameResourceGetIndex(void *address);
static int RES_GetMdlFileNameSub(char *dst, int selector);
extern char *strcpy(char *destination, const char *source);
int GameResourceSearch(int type, int key);
static int RES_loadFileSubMapSub(char *name, const char *suffix, void *buffer);
static int RES_loadFileSub(int callback, int resource_id, void *address);
u8 ScenePath = 0;

static ResourceKeyEntry CID[8] = {
    { ResEmptyName, CID0, ResPcPath },
    { ResEmptyName, CID1, ResNpcPath },
    { ResEmptyName, CID2, ResPplPath },
    { ResEmptyName, CID3, ResPplPath },
    { ResEmptyName, CID4, ResPplPath },
    { ResEmptyName, CID5, ResPplPath },
    { ResEmptyName, CID6, ResCitiPath },
    { 0, 0, 0 }
};

static ResourceKeyEntry RID[4] = {
    { ResVxName, RID0, ResVxVxPath },
    { ResAgName, RID1, ResAgAgPath },
    { ResEmptyName, RID2, ResEtcPath },
    { 0, 0, 0 }
};

static ResourceKeyEntry WID[10] = {
    { ResKosName, WID0, ResPcKosPath },
    { ResMmoName, WID1, ResPcMmoPath },
    { ResZigName, WID2, ResPcZigPath },
    { ResJrPrefixName, WID3, ResPcJrPath },
    { ResStnName, WID4, ResPcStnPath },
    { ResSonName, WID5, ResPcSonPath },
    { ResEmptyName, WID6, ResRoboPath },
    { ResEmptyName, WID7, ResEnemyPath },
    { ResChoName, WID8, ResPcChoPath },
    { 0, 0, 0 }
};

static ResourceKeyEntry EID[17] = {
    { ResGunoName, EID0, ResGunoGunoPath },
    { ResUtmaName, EID1, ResUtmaUtmaPath },
    { ResUtreName, EID2, ResUtreUtrePath },
    { ResUtsoName, EID3, ResUtsoUtsoPath },
    { ResUtroName, EID4, ResUtroUtroPath },
    { ResFemaName, EID5, ResFemaFemaPath },
    { ResFeroName, EID6, ResFeroFeroPath },
    { ResFesoName, EID7, ResFesoFesoPath },
    { ResFereName, EID8, ResFereFerePath },
    { ResSeparatorName, EID8, ResGunoGunoPath },
    { ResSeparatorName, EID8, ResGunoGunoPath },
    { ResSeparatorName, EID8, ResGunoGunoPath },
    { ResSeparatorName, EID8, ResGunoGunoPath },
    { ResSeparatorName, EID8, ResGunoGunoPath },
    { ResSeparatorName, EID8, ResGunoGunoPath },
    { ResBossName, EIDf, ResBossBossPath },
    { 0, 0, 0 }
};

static ResourceKeyEntry MID[2] = {
    { ResEmptyName, MID0, ResMobj000MobjPath },
    { 0, 0, 0 }
};

static ResourceKeyEntry AID[2] = {
    { ResEmptyName, AID0, ResAobj000AccPath },
    { 0, 0, 0 }
};

static ResourceKeyEntry MOBJ[10] = {
    { ResMuName, MU, ResMuobjMUPath },
    { ResMuwName, MUW, ResMuwobjMuwPath },
    { ResTreName, TRE, ResTreobjTrePath },
    { ResTreName, TRE, ResTreobjTrePath },
    { ResMuxName, MUX, ResMuxobjMuxPath },
    { ResMuxName, MUX, ResMuxobjMuxPath },
    { ResMufName, MUF, ResMufobjMufPath },
    { ResMufName, MUF, ResMufobjMufPath },
    { ResMusName, MUS, ResMusobjMusPath },
    { 0, 0, 0 }
};

static ResourceKeyEntry model[9] = {
    {ResCidName, {.children = CID}, ResCharPath},
    {ResCidName, {.children = CID}, ResCharPath},
    {ResRidName, {.children = RID}, ResRoboPath},
    {ResWidName, {.children = WID}, ResWeaponPath},
    {ResEidName, {.children = EID}, ResEnemyPath},
    {ResMidName, {.children = MID}, ResObjPath},
    {ResAidName, {.children = AID}, ResObjPath},
    {ResEmptyName, {.children = MOBJ}, ResObjPath},
    {0, 0, 0},
};

static const char *tbl_0_00366420[SCENE_PATH_COUNT] = {
    ResDataScenePath, ResDataSceneTestPath, ResDataSceneCheckPath, ResDataSceneCfPath, ResDataSceneEventPath,
    ResDataSceneGashPath, ResDataSceneFujiPath, ResDataSceneKojimaPath, ResDataSceneSakisakoPath, ResDataSceneKonishiPath,
    ResDataSceneKojiPath, ResDataSceneNvYonePath, ResDataSceneNakaharaPath, ResDataSceneYajimaPath, ResDataSceneKuramotoPath,
    ResDataSceneKeiichiPath, ResDataSceneSatoPath, ResDataSceneSugisawaPath, ResDataSceneYagiPath, ResDataSceneItaiPath
};

GameResourceEntry GameResource[GAME_RESOURCE_COUNT] = {0};

static const char * CID0[54] = { ResShionShion4Name, ResKosmosName, ResChaosName, ResMomoName, ResJrName, ResZiggyName, ResShitanName, ResShionChName, ResShion1Shion5Name, ResShion2Name, ResShion3Name, ResShionName, ResShion5Shion1Name, ResKosmos1Name, ResKosmos2Name, ResKosmos3Name, ResKosmos5Name, ResChaos1Name, ResChaos2Name, ResMomo1Name, ResMomo2Name, ResMomo3Name, ResMomo4Name, ResJr1Name, ResJr2Name, ResJr3Name, ResZiggy1Name, ResShitan1Name, ResShitan2Name, ResShionHShion4HName, ResKosmosHName, ResChaosHName, ResMomoHName, ResJrHName, ResZiggyHName, ResShitanHName, ResKosmosH1Name, ResKosmosH2Name, ResKosmosH3Name, ResKosmosH4Name, ResKosmosH5Name, ResKosmosH6Name, ResShion1HShion5HName, ResJr1HName, ResShion2HName, ResShion3HName, ResJr4Name, ResShion4HShionHName, ResShion5HShion1HName, ResMomo5Name, ResMomo5HName, ResMomo6Name, ResShion6HName, 0 };

static const char * CID1[89] = { ResMarglisName, ResMarglis1Name, ResGaignunName, ResGaignun1Name, ResAlbeltName, ResAlbelt1Name, ResAllenName, ResAllen1Name, ResKebinName, ResKebin1Name, ResKebin2Name, ResVirgilName, ResVirgil1Name, ResVirgil2Name, ResVirgil3Name, ResVirgil4Name, ResFriedName, ResEllyName, ResMatehwsName, ResMatehws1Name, ResTonnyName, ResHammerName, ResAndrewName, ResAndrew1Name, ResAndrew2Name, ResAndrew3Name, ResAndrew4Name, ResFebName, ResYuriName, ResJoachimName, ResSellersName, ResMaryName, ResShelleyName, ResAbelName, ResHelmerName, ResVoyagerName, ResMiyukiName, ResLapisName, ResCeciliaName, ResCathName, ResShiDadName, ResShiMamName, ResStepMamName, ResPelegriName, ResTogashiName, ResMoriyamaName, ResHynlinName, ResShalronName, ResKJoachName, ResAndWifeName, ResAndDaugName, ResElly2Name, ResVanName, ResChiefName, ResCecilia1Name, ResCath1Name, ResJoachim1Name, ResMarglisHName, ResAlbeltHName, ResAllenHName, ResKebinHName, ResVirgilHName, ResFriedHName, ResMatehwsHMatehwsName, ResEllyHName, ResKebin1HName, ResVirgil2HName, ResVirgil4HName, ResAndrew5Name, ResAndrew6Name, ResAndrew7Name, ResAndrew8Name, ResAlbelt2HName, ResVirgil1HName, ResVirgil3HName, ResAndrewHAndrewName, ResAndrewHAndrew1Name, ResAndrewHAndrew2Name, ResAndrewHAndrew3Name, ResAndrewHAndrew4Name, ResAndrewHAndrew5Name, ResAndrewHAndrew6Name, ResGaignunHName, ResTonnyHTonnyName, ResHammerHHammerName, ResFeb1Name, ResAlbelt3HName, ResAndrew9Name, 0 };

static const char * CID2[32] = { ResSpaceName, ResVecMName, ResVecM1Name, ResVecM2Name, ResVecWName, ResVecW1Name, ResOffMName, ResOffM1Name, ResOffM2Name, ResOffM3Name, ResOffWName, ResOffW1Name, ResPilotName, ResSoldName, ResEngName, ResEngOffName, ResSuukiName, ResRwMName, ResRwM1Name, ResRwM2Name, ResRwWName, ResRwW1Name, ResRwW2Name, ResEsoldName, ResBrealMName, ResBrealM1Name, ResBrealM2Name, ResBrealM3Name, ResBrealM4Name, ResBrealWName, ResBrealW1Name, 0 };

static const char * CID3[29] = { ResCommiName, ResCommi1Name, ResCommi2Name, ResCommi3Name, ResCommi4Name, ResCommi5Name, ResGoveWName, ResGoveW1Name, ResGoveW2Name, ResUSoldName, ResUSpaceName, ResRealMName, ResRealM1Name, ResRealM2Name, ResRealWName, ResRealW1Name, ResRealW2Name, ResRealW2Name, ResRealW2Name, ResRealW2Name, ResRealW2Name, ResRealW2Name, ResRealW2Name, ResRealW2Name, ResRealW2Name, ResRealW2Name, ResRealW2Name, ResUSoldsName, 0 };

static const char * CID4[17] = { ResDuName, ResDu1Name, ResDu2Name, ResHyakuName, ResAssMName, ResAssM1Name, ResAssM2Name, ResAssWName, ResGoveMName, ResGoveM1Name, ResGoveM2Name, ResGoveM2Name, ResGoveM2Name, ResKanchoName, ResAssM3Name, ResAssM4Name, 0 };

static const char * CID5[10] = { ResContName, ResUticMName, ResUticM1Name, ResUticM2Name, ResUticWName, ResUticW1Name, ResUticW2Name, ResPolMName, ResPolWName, 0 };

static const char * CID6[81] = { ResManAName, ResManA1Name, ResManA2Name, ResManBName, ResManB1Name, ResManB2Name, ResOjiAName, ResOjiA1Name, ResOjiA2Name, ResOjiBName, ResOjiB1Name, ResOjiB2Name, ResJijiAName, ResJijiA1Name, ResJijiA2Name, ResJijiBName, ResJijiB1Name, ResJijiB2Name, ResKidsmAName, ResKidsmA1Name, ResKidsmA2Name, ResKidsmBName, ResKidsmB1Name, ResKidsmB2Name, ResBoyAName, ResBoyA1Name, ResBoyA2Name, ResBoyBName, ResBoyB1Name, ResBoyB2Name, ResWmanAName, ResWmanA1Name, ResWmanA2Name, ResWmanBName, ResWmanB1Name, ResWmanB2Name, ResObaAName, ResObaA1Name, ResObaA2Name, ResObaBName, ResObaB1Name, ResObaB2Name, ResBabaAName, ResBabaA1Name, ResBabaA2Name, ResBabaBName, ResBabaB1Name, ResBabaB2Name, ResKidswAName, ResKidswA1Name, ResKidswA2Name, ResKidswBName, ResKidswB1Name, ResKidswB2Name, ResGirlAName, ResGirlA1Name, ResGirlA2Name, ResGirlBName, ResGirlB1Name, ResGirlB2Name, ResBreadName, ResBarOjiName, ResBarMName, ResDoctorName, ResNurseName, ResCleChName, ResRobotName, ResKingName, ResCleObaName, ResBreadChName, ResInnGirlName, ResMouthName, ResGoroName, ResPilotNoName, ResKingCatName, ResBartenName, ResSpyName, ResInnOjiName, ResScotName, ResEngOjiName, 0 };

static const char * RID0[7] = { ResFile01Name, ResFile02Name, ResFile03Name, ResFile04Name, ResFile05Name, ResFile06Name, 0 };

static const char * RID1[14] = { ResSuffix01Name, ResSuffix02Name, ResSuffix03Name, ResSuffix04Name, ResSuffix05Name, ResFile01CvName, ResFile01HoName, ResFile01Ho2Name, ResFile02HoName, ResSuffix06Name, ResFile07Name, ResFile08Name, ResFile09Name, 0 };

static const char * RID2[11] = { ResUknGName, ResVx1MName, ResTs01Name, ResUkunBName, ResUkunSName, ResGrFinalName, ResGrHeadName, ResGrHandName, ResGrBodyName, ResGrLegName, 0 };

static const char * WID0[11] = { ResHw01Name, ResHw02Name, ResHw03Name, ResHw04Name, ResHw05Name, ResHw06Name, ResBw01Name, ResBw02Name, ResBw03Name, ResBw04Name, 0 };

static const char * WID1[18] = { ResRd01Name, ResRd02Name, ResRd03Name, ResRd04Name, ResRd05Name, ResRd06Name, ResRd07Name, ResRd08Name, ResRd09Name, ResRd10Name, ResRd11Name, ResRd12Name, ResRd13Name, ResRd14Name, ResRd15Name, ResRd16Name, ResRd17Name, 0 };

static const char * WID2[6] = { ResSw01Name, ResBw01Name, ResBw02Name, ResBw03Name, ResBw04Name, 0 };

static const char * WID3[9] = { ResGun01Name, ResGun02Name, ResGun03Name, ResGun04Name, ResGun05Name, ResJrHg06Name, ResJrHg07Name, ResJrHg08Name, 0 };

static const char * WID4[6] = { ResSw01Name, ResSw02Name, ResSw03Name, ResSw04Name, ResSw05Name, 0 };

static const char * WID5[2] = { ResMboxName, 0 };

static const char * WID6[50] = { ResAx00Name, ResBd00Name, ResBd01Name, ResWct00Name, ResHg00Name, ResHg01Name, ResHmr00Name, ResDlc00Name, ResHmr02Name, ResMg00Name, ResMg01Name, ResRf00Name, ResRf01Name, ResRf02Name, ResBam00Name, ResBs00Name, ResDrl00Name, ResDrl01Name, ResLsr00Name, ResPbk00Name, ResFl00Name, ResGnd00Name, ResGlg00Name, ResHb00Name, ResMsp00Name, ResMsp02Name, ResBl00Name, ResCb00Name, ResEac00Name, ResBp00Name, ResLgc00Name, ResAmp02Name, ResMsp01Name, ResMsp03Name, ResShb00Name, ResFsd00Name, ResVsd00Name, ResBbc00Name, ResBw00Name, ResEad00Name, ResEcm01Name, ResEcm02Name, ResSmp01Name, ResWire00Name, ResSol00Name, ResDeff00Name, ResGlg00RName, ResHb00RName, ResMsp02RName, 0 };

static const char * WID7[12] = { ResAcc018Name, ResAcc028Name, ResAcc029Name, ResAcc030Name, ResAcc031Name, ResAcc032Name, ResAcc033Name, ResAcc034Name, ResAcc035Name, ResAcc036Name, ResEwpHg01Name, 0 };

static const char * WID8[2] = { ResWingName, 0 };

static const char * EID0[56] = { ResFile001Name, ResFile002Name, ResFile003Name, ResFile004Name, ResFile005Name, ResFile006Name, ResFile007Name, ResFile008Name, ResFile009Name, ResFile010Name, ResFile011Name, ResFile012Name, ResFile013Name, ResFile014Name, ResFile015Name, ResFile016Name, ResFile017Name, ResFile018Name, ResFile019Name, ResFile020Name, ResFile021Name, ResFile022Name, ResFile023Name, ResFile024Name, ResFile025Name, ResFile301Name, ResFile302Name, ResFile303Name, ResFile304Name, ResFile305Name, ResFile306Name, ResFile307Name, ResFile308Name, ResFile309Name, ResFile310Name, ResFile311Name, ResFile312Name, ResFile313Name, ResFile314Name, ResFile315Name, ResFile316Name, ResFile317Name, ResFile318Name, ResFile319Name, ResFile320Name, ResFile321Name, ResFile322Name, ResFile323Name, ResFile324Name, ResFile325Name, ResEmptyName, ResEmptyName, ResEmptyName, ResEmptyName, ResFile114Name, 0 };

static const char * EID1[15] = { ResFile001Name, ResFile002Name, ResFile003Name, ResFile004Name, ResFile005Name, ResFile006Name, ResFile007Name, ResFile008Name, ResFile009Name, ResFile010Name, ResFile011Name, ResFile012Name, ResFile013Name, ResFile014Name, 0 };

static const char * EID2[7] = { ResFile001Name, ResFile002Name, ResFile003Name, ResFile004Name, ResFile005Name, ResFile006Name, 0 };

static const char * EID3[8] = { ResFile001Name, ResFile002Name, ResFile003Name, ResFile004Name, ResFile005Name, ResFile006Name, ResFile007Name, 0 };

static const char * EID4[8] = { ResFile001Name, ResFile002Name, ResFile003Name, ResFile004Name, ResFile005Name, ResFile006Name, ResFile007Name, 0 };

static const char * EID5[7] = { ResFile001Name, ResFile002Name, ResFile003Name, ResFile004Name, ResFile005Name, ResFile006Name, 0 };

static const char * EID6[2] = { ResFile001Name, 0 };

static const char * EID7[5] = { ResFile001Name, ResFile002Name, ResFile003Name, ResFile004Name, 0 };

static const char * EID8[4] = { ResFile001Name, ResFile002Name, ResFile003Name, 0 };

static const char * EIDf[34] = { ResFile001Name, ResFile002Name, ResFile003Name, ResFile004Name, ResFile005Name, ResFile006Name, ResFile007Name, ResFile008Name, ResFile009Name, ResFile010Name, ResFile011Name, ResFile012Name, ResFile013Name, ResFile014Name, ResFile015Name, ResFile016Name, ResFile017Name, ResFile018Name, ResFile019Name, ResFile020Name, ResFile021Name, ResFile022Name, ResFile023Name, ResFile024Name, ResFile025Name, ResFile026Name, ResFile027Name, ResFile028Name, ResFile029Name, ResFile030Name, ResFile031Name, ResFile032Name, ResFile033Name, 0 };

static const char * MID0[182] = { ResFile000Name, ResFile001Name, ResFile002Name, ResFile003Name, ResFile004Name, ResFile005Name, ResFile006Name, ResFile007Name, ResFile008Name, ResFile009Name, ResFile010Name, ResFile011Name, ResFile012Name, ResFile013Name, ResFile014Name, ResFile015Name, ResFile016Name, ResFile017Name, ResFile018Name, ResFile019Name, ResFile020Name, ResFile021Name, ResFile022Name, ResFile023Name, ResFile024Name, ResFile025Name, ResFile026Name, ResFile027Name, ResFile028Name, ResFile029Name, ResFile030Name, ResFile031Name, ResFile032Name, ResFile033Name, ResFile034Name, ResFile035Name, ResFile036Name, ResFile037Name, ResFile038Name, ResFile039Name, ResFile040Name, ResFile041Name, ResFile042Name, ResFile043Name, ResFile044Name, ResFile045Name, ResFile046Name, ResFile047Name, ResFile048Name, ResFile049Name, ResFile050Name, ResFile051Name, ResFile052Name, ResFile053Name, ResFile054Name, ResFile055Name, ResFile056Name, ResFile057Name, ResFile058Name, ResFile059Name, ResFile060Name, ResFile061Name, ResFile062Name, ResFile063Name, ResFile064Name, ResFile065Name, ResFile066Name, ResFile067Name, ResFile068Name, ResFile069Name, ResFile070Name, ResFile071Name, ResFile072Name, ResFile073Name, ResFile074Name, ResFile075Name, ResFile076Name, ResFile077Name, ResFile078Name, ResFile079Name, ResFile080Name, ResFile081Name, ResFile082Name, ResFile083Name, ResFile084Name, ResFile085Name, ResFile086Name, ResFile087Name, ResFile088Name, ResFile089Name, ResFile090Name, ResFile091Name, ResFile092Name, ResFile093Name, ResFile094Name, ResFile095Name, ResFile096Name, ResFile097Name, ResFile098Name, ResFile099Name, ResFile100Name, ResFile101Name, ResFile102Name, ResFile103Name, ResFile104Name, ResFile105Name, ResFile106Name, ResFile107Name, ResFile108Name, ResFile109Name, ResFile110Name, ResFile111Name, ResFile112Name, ResFile113Name, ResFile114Name, ResFile115Name, ResFile116Name, ResFile117Name, ResFile118Name, ResFile119Name, ResFile120Name, ResFile121Name, ResFile122Name, ResFile123Name, ResFile124Name, ResFile125Name, ResFile126Name, ResFile127Name, ResFile128Name, ResFile129Name, ResFile130Name, ResFile131Name, ResFile132Name, ResFile133Name, ResFile134Name, ResFile135Name, ResFile136Name, ResFile137Name, ResFile138Name, ResFile139Name, ResFile140Name, ResFile141Name, ResFile142Name, ResFile143Name, ResFile144Name, ResFile145Name, ResFile146Name, ResFile147Name, ResFile148Name, ResFile149Name, ResFile150Name, ResFile151Name, ResFile152Name, ResFile153Name, ResFile154Name, ResFile155Name, ResFile156Name, ResFile157Name, ResFile158Name, ResFile159Name, ResFile160Name, ResFile161Name, ResFile162Name, ResFile163Name, ResFile164Name, ResFile165Name, ResFile166Name, ResFile167Name, ResFile168Name, ResFile169Name, ResFile170Name, ResFile171Name, ResFile172Name, ResFile173Name, ResFile174Name, ResFile175Name, ResFile176Name, ResFile177Name, ResFile178Name, ResFile179Name, ResFile180Name, 0 };

static const char * AID0[128] = { ResFile001Name, ResFile002Name, ResFile003Name, ResFile004Name, ResFile005Name, ResFile006Name, ResFile007Name, ResFile008Name, ResFile009Name, ResFile010Name, ResFile011Name, ResFile012Name, ResFile013Name, ResFile014Name, ResFile015Name, ResFile016Name, ResFile017Name, ResFile018Name, ResFile019Name, ResFile020Name, ResFile021Name, ResFile022Name, ResFile023Name, ResFile024Name, ResFile025Name, ResFile026Name, ResFile027Name, ResFile028Name, ResFile029Name, ResFile030Name, ResFile031Name, ResFile032Name, ResFile033Name, ResFile034Name, ResFile035Name, ResFile036Name, ResFile037Name, ResFile038Name, ResFile039Name, ResFile040Name, ResFile041Name, ResFile042Name, ResFile043Name, ResFile044Name, ResFile045Name, ResFile046Name, ResFile047Name, ResFile048Name, ResFile049Name, ResFile050Name, ResFile051Name, ResFile052Name, ResFile053Name, ResFile054Name, ResFile055Name, ResFile056Name, ResFile057Name, ResFile058Name, ResFile059Name, ResFile060Name, ResFile061Name, ResFile062Name, ResFile063Name, ResFile064Name, ResFile065Name, ResFile066Name, ResFile067Name, ResFile068Name, ResFile069Name, ResFile070Name, ResFile071Name, ResFile072Name, ResFile073Name, ResFile074Name, ResFile075Name, ResFile076Name, ResFile077Name, ResFile078Name, ResFile079Name, ResFile080Name, ResFile081Name, ResFile082Name, ResFile083Name, ResFile084Name, ResFile085Name, ResFile086Name, ResFile087Name, ResFile088Name, ResFile089Name, ResFile090Name, ResFile091Name, ResFile092Name, ResFile093Name, ResFile094Name, ResFile095Name, ResFile096Name, ResFile097Name, ResFile098Name, ResFile099Name, ResFile100Name, ResFile101Name, ResFile102Name, ResFile103Name, ResFile104Name, ResFile105Name, ResFile106Name, ResFile107Name, ResFile108Name, ResFile109Name, ResFile110Name, ResFile111Name, ResFile112Name, ResFile113Name, ResFile114Name, ResFile115Name, ResFile116Name, ResFile117Name, ResFile118Name, ResFile119Name, ResFile120Name, ResFile121Name, ResFile122Name, ResFile123Name, ResFile124Name, ResFile125Name, ResFile126Name, ResFile127Name, 0 };

static const char * MU[131] = { ResFile001Name, ResFile002Name, ResFile003Name, ResFile004Name, ResFile005Name, ResFile006Name, ResFile007Name, ResFile008Name, ResFile009Name, ResFile010Name, ResFile011Name, ResFile012Name, ResFile013Name, ResFile014Name, ResFile015Name, ResFile016Name, ResFile017Name, ResFile018Name, ResFile019Name, ResFile020Name, ResFile021Name, ResFile022Name, ResFile023Name, ResFile024Name, ResFile025Name, ResFile026Name, ResFile027Name, ResFile028Name, ResFile029Name, ResFile030Name, ResFile031Name, ResFile032Name, ResFile033Name, ResFile034Name, ResFile035Name, ResFile036Name, ResFile037Name, ResFile038Name, ResFile039Name, ResFile040Name, ResFile041Name, ResFile042Name, ResFile043Name, ResFile044Name, ResFile045Name, ResFile046Name, ResFile047Name, ResFile048Name, ResFile049Name, ResFile050Name, ResFile051Name, ResFile052Name, ResFile053Name, ResFile054Name, ResFile055Name, ResFile056Name, ResFile057Name, ResFile058Name, ResFile059Name, ResFile060Name, ResFile061Name, ResFile062Name, ResFile063Name, ResFile064Name, ResFile065Name, ResFile066Name, ResFile067Name, ResFile068Name, ResFile069Name, ResFile070Name, ResFile071Name, ResFile072Name, ResFile073Name, ResFile074Name, ResFile075Name, ResFile076Name, ResFile077Name, ResFile078Name, ResFile079Name, ResFile080Name, ResFile081Name, ResFile082Name, ResFile083Name, ResFile084Name, ResFile085Name, ResFile086Name, ResFile087Name, ResFile088Name, ResFile089Name, ResFile090Name, ResFile091Name, ResFile092Name, ResFile093Name, ResFile094Name, ResFile095Name, ResFile096Name, ResFile097Name, ResFile098Name, ResFile099Name, ResFile100Name, ResFile101Name, ResFile102Name, ResFile103Name, ResFile104Name, ResFile105Name, ResFile106Name, ResFile107Name, ResFile108Name, ResFile109Name, ResFile110Name, ResFile111Name, ResFile112Name, ResFile113Name, ResFile114Name, ResFile115Name, ResFile116Name, ResFile117Name, ResFile118Name, ResFile119Name, ResFile120Name, ResFile121Name, ResFile122Name, ResFile123Name, ResFile124Name, ResFile125Name, ResFile126Name, ResFile127Name, ResFile128Name, ResFile129Name, ResFile130Name, 0 };

static const char * MUW[21] = { ResFile001Name, ResFile002Name, ResFile003Name, ResFile004Name, ResFile005Name, ResFile006Name, ResFile007Name, ResFile008Name, ResFile009Name, ResFile010Name, ResFile011Name, ResFile012Name, ResFile013Name, ResFile014Name, ResFile015Name, ResFile016Name, ResFile017Name, ResFile018Name, ResFile019Name, ResFile020Name, 0 };

static const char * TRE[11] = { ResFile001Name, ResFile002Name, ResFile003Name, ResFile004Name, ResFile005Name, ResFile006Name, ResFile007Name, ResFile008Name, ResFile009Name, ResFile010Name, 0 };

static const char * MUX[11] = { ResFile001Name, ResFile002Name, ResFile003Name, ResFile004Name, ResFile005Name, ResFile006Name, ResFile007Name, ResFile008Name, ResFile009Name, ResFile010Name, 0 };

static const char * MUF[11] = { ResFile001Name, ResFile002Name, ResFile003Name, ResFile004Name, ResFile005Name, ResFile006Name, ResFile007Name, ResFile008Name, ResFile009Name, ResFile010Name, 0 };

static const char * MUS[11] = { ResFile001Name, ResFile002Name, ResFile003Name, ResFile004Name, ResFile005Name, ResFile006Name, ResFile007Name, ResFile008Name, ResFile009Name, ResFile010Name, 0 };

void (*GameResourceReadFileCallback)(void) = 0;

static GameResourceCommand command[24];

static const char * tbl_1_00366470[7];

static char dummy_3[15] ;

static const char root_4[6];

static const char dummy_5[6];

static char base_6[13] ;

static char base_7[29] ;

static char base_8[24] ;

static const char base_9[6];

static char base_2[18] ;

static const char * tbl_1_00366470[7] = { ResEmptyName, ResSOName, ResKMName, ResCSName, ResMOName, ResJRName, ResZ8Name };

static char base_2[18] = "data\\movie\\mpeg2\\";

static char dummy_3[15] = "data\\map\\dummy";

static char base_6[13] = "data\\motion\\";

static char base_7[29] = "data\\movie\\small\\mvs0000.ipu";

static char base_8[24] = "data\\texture\\fm0000.xtx";

const char lexSuffix[8] = ".lex";

const char xtxSuffix[8] = ".xtx";

const char idSuffix[8] = ".id";

static const char root_4[6] = "data\\";

static const char dummy_5[6] = "dummy";

const char faceSuffix[8] = "_face";

const char jntSuffix[8] = ".jnt";

static const char base_9[6] = "data\\";

unsigned char ModelPath = 0;

/* Face id tables, referenced by faceTbl. */
static u16 facePC[54];

static u16 faceNPC[89];

static u16 facePPL1[32];

static u16 facePPL2[29];

static u16 facePPL3[17];

static u16 facePPL4[10];

static u16 faceNPC[89] = { 0, 0, 258, 0, 5, 0, 5, 0, 263, 266, 0, 266, 269, 269, 269, 269, 269, 0, 0, 0, 275, 0, 0, 0, 279, 279, 279, 279, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 311, 286, 0, 0, 0, 0, 324, 0, 0, 0, 0, 0, 324, 279, 279, 279, 279, 315, 324, 324, 332, 333, 334, 335, 336, 337, 338, 0, 0, 0, 0, 0, 332 };

static u16 facePPL1[32] = { 0, 0, 0, 0, 0, 0, 517, 0, 0, 0, 0, 0, 0, 0, 0, 0, 769, 0, 0, 0, 0, 0, 0, 0, 0, 537, 537, 537, 537, 537, 542, 542 };

static u16 facePPL2[29] = { 0, 0, 0, 0, 0, 0, 0, 783, 783, 783, 0, 0, 0, 780, 780, 0, 783, 783, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static u16 facePPL3[17] = { 0, 0, 0, 514, 0, 772, 773, 0, 0, 0, 1033, 1033, 1033, 1033, 309, 1031, 772 };

static u16 facePPL4[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

MotionEntry cfMtnList[31] = { { .name = ResShionCfName, .ids = { 1, 9, 10, 11, 12, 13, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResKosmosCfName, .ids = { 2, 14, 15, 16, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResChaosCfName, .ids = { 3, 18, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResMomoCfName, .ids = { 4, 20, 21, 22, 23, 50, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResJrCfName, .ids = { 5, 24, 25, 26, 47, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResZiggyCfName, .ids = { 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResShitanCfName, .ids = { 7, 28, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResAllenCfName, .ids = { 263, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResOldwCfName, .ids = { 1579, 1582, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResOldmCfName, .ids = { 1549, 1552, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResMiddlemCfName, .ids = { 1543, 1546, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResMiddlewCfName, .ids = { 1573, 1576, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResSoldierwCfName, .ids = { 1289, 542, 523, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResSoldiermCfName, .ids = { 526, 536, 525, 513, 527, 1288, 537, 519, 0, 0, 0, 0, 0, 0 } }, { .name = ResWomanCfName, .ids = { 1567, 1570, 783, 533, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResYoungmCfName, .ids = { 1537, 1540, 780, 530, 529, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResYoungwCfName, .ids = { 1591, 1594, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResGuno001CfName, .ids = { 16385, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResGuno301Name, .ids = { 16410, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResGuno302Name, .ids = { 16411, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResGuno303Name, .ids = { 16412, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResGuno304Name, .ids = { 16413, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResGuno305Name, .ids = { 16414, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResGuno323Name, .ids = { 16432, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResGuno324Name, .ids = { 16433, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResGuno325Name, .ids = { 16434, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResGuno301Name, .ids = { 16410, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResGuno302Name, .ids = { 16411, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResGuno303Name, .ids = { 16412, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResGuno307Name, .ids = { 16416, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = 0, .ids = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } } };

FaceEntry faceTbl[7] = { { .table = facePC, .count = 54, .group = 0 }, { .table = faceNPC, .count = 89, .group = 256 }, { .table = facePPL1, .count = 32, .group = 512 }, { .table = facePPL2, .count = 29, .group = 768 }, { .table = facePPL3, .count = 17, .group = 1024 }, { .table = facePPL4, .count = 10, .group = 1280 }, { .table = 0, .count = 0, .group = 0 } };

static u16 facePC[54] = {
    0x0000, 0x0001, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0002, 0x0002,
    0x0002, 0x0002, 0x0003, 0x0003, 0x0004, 0x0004, 0x0004, 0x0004,
    0x0005, 0x0005, 0x0005, 0x0006, 0x0000, 0x0000, 0x001E, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x001F, 0x001F, 0x001F,
    0x001F, 0x0025, 0x001F, 0x001E, 0x0022, 0x001E, 0x001E, 0x0005,
    0x001E, 0x001E, 0x0004, 0x0021, 0x0004, 0x001E
};

static u8 arcno;

static u32 *arcfileaddr;

static int arcfilepreload;

static EnemySeEntry EnemySeBank[ENEMY_SE_BANK_COUNT];

static char foot_name[16];

static char env_name[0x10];

typedef struct {
    u32 signature;
} SceneTextHeader;

typedef union {
    SceneTextHeader header;
    u8 text[SCENE_TXT_SIZE];
} SceneTextBuffer;

static SceneTextBuffer scene_txt_buffer;

static u32 archeader[ARC_HEADER_WORDS];

static u8 GameResourceWorkReloadTable[0x80];

static u8 GameResourceWorkReloadLeader;

static void default_callback(void)
{
    xglSleep();
}

const char *RES_getPath(void)
{
    if (ScenePath >= SCENE_PATH_COUNT)
        ScenePath = 0;
    return tbl_0_00366420[ScenePath];
}

/*
 * Appends the scene script name for resource_id (cf or ev prefix, scene number,
 * .txt) to the current scene directory in path. Returns a pointer to the "txt" extension.
 */
static char *RES_getScenePath(char *path, int resource_id)
{
    const char *scene_path = RES_getPath();
    int scene_id;

    while ((*path = *scene_path) != 0) {
        path++;
        scene_path++;
    }

    if (resource_id <= RES_SCENE_ID_MASK) {
        path[0] = 'c';
        path[1] = 'f';
    } else {
        path[0] = 'e';
        path[1] = 'v';
    }

    scene_id = resource_id & RES_SCENE_ID_MASK;

    path[5] = '0' + scene_id % 10;
    scene_id /= 10;
    path[4] = '0' + scene_id % 10;
    scene_id /= 10;
    path[3] = '0' + scene_id % 10;
    scene_id /= 10;
    path[2] = '0' + scene_id % 10;
    path[6] = '.';
    path[7] = 't';
    path[8] = 'x';
    path[9] = 't';
    path[10] = 0;

    return path + 7;
}

/* Returns the effect bank address assigned to sound_id, or -1 when no bank has it. */
int RES_GetEnemySeBank(int sound_id)
{
    int index = 0;
    int bank = ENEMY_SE_BANK_ADDRESS_BASE;

    for (; index < ENEMY_SE_BANK_COUNT; index++) {
        if (EnemySeBank[index].id == sound_id)
            return bank;
        bank += ENEMY_SE_BANK_STRIDE;
    }
    return -1;
}

int RES_GetEnemySeType(int sound_id)
{
    int index;

    for (index = 0; index < ENEMY_SE_BANK_COUNT; index++) {
        if (EnemySeBank[index].id == sound_id)
            return EnemySeBank[index].type;
    }
    return -1;
}

int RES_GetEnemySeFoot(int sound_id)
{
    int index;

    for (index = 0; index < ENEMY_SE_BANK_COUNT; index++) {
        if (EnemySeBank[index].id == sound_id)
            return EnemySeBank[index].foot;
    }
    return -1;
}

const char *RES_GetEnemySeName(int index)
{
    if (index < ENEMY_SE_BANK_COUNT)
        return EnemySeBank[index].name;
    return 0;
}

/* Returns the first id of the cfMtnList group that contains id, or 1 when none does. */
static int RES_GetMotBaseID(int id)
{
    MotionEntry *entry = cfMtnList;
    u16 *slot;
    u16 base;

    id &= 0xffff;
    while (entry->name != 0) {
        slot = entry->ids;
        base = *slot;
        while (*slot != 0) {
            if (*slot == id)
                return base;
            slot++;
        }
        entry++;
    }
    return 1;
}

/*
 * Maps a face id through the faceTbl entry of its group. Returns the face id
 * unchanged when that table has no entry for it.
 */
static int convert_face(int value)
{
    FaceEntry *entry = faceTbl;
    int face = value;
    int key = face & 0x7f00;
    u16 converted;

    face &= 0xffff;

    while (entry->count != 0) {
        if (entry->group == key) {
            converted = entry->table[face & 0xff];
            if (converted != 0)
                face = converted;
            break;
        }
        entry++;
    }
    return face;
}

/*
 * Converts a lookup type and key to the form the resource table stores: face
 * and motion ids are mapped, named motions get a tag, and file keys become the
 * slot index. Returns 1 when the key was converted.
 */
static int resource_typeid_translate(int index, int *type, int *key)
{
    switch ((unsigned short)*type) {
    case RES_TYPE_MODEL:
        if ((*key & RES_ID_FACE_MASK) == RES_ID_FACE) {
            *key = convert_face(*key & 0xFFFF) + RES_ID_FACE;
            return 1;
        }
        return 0;
    case RES_TYPE_MOTION:
        if (*key <= RES_ID_MOTION_BASE_MAX) {
            *key = RES_GetMotBaseID(*key);
            return 1;
        }
        return 0;
    case RES_TYPE_MOTION_NAMED:
        *type = RES_TYPE_MOTION;
        *key = index | RES_ID_MOTION_NAMED_TAG;
        return 1;
    case RES_TYPE_FILE:
        *key = index;
        return 1;
    default:
        return 0;
    }
}

/*
 * Returns the index of the first resource matching type and key, where -1 matches
 * any value. Returns -1 for file-type searches and when nothing matches.
 */
int GameResourceSearch(int type, int key)
{
    int index;
    int resource_type;
    int resource_key;

    if (type == RES_TYPE_FILE)
        return -1;
    index = 0;
    while (index < GAME_RESOURCE_COUNT) {
        resource_type = type;
        resource_key = key;
        resource_typeid_translate(index, &resource_type, &resource_key);
        if ((type == -1 || GameResource[index].state == resource_type) &&
            (key == -1 || GameResource[index].handle == resource_key))
            return index;
        index++;
    }
    return -1;
}

static int resource_get_free(void)
{
    int index;

    for (index = 0; index < GAME_RESOURCE_COUNT; index++) {
        if (GameResource[index].state == GAME_RESOURCE_FREE)
            return index;
    }
    return -1;
}

/* Returns the address of the first free resource slot, but not below RES_WORK_BASE. */
void *GameResourceGetFreeAddr(void)
{
    unsigned char *free_address = GameResource[resource_get_free()].address;

    /* Fixed EE RAM addresses, like the resource buffers of GameResourceInit. */
    if (free_address <= (unsigned char *)(RES_WORK_BASE - 1))
        return (void *)RES_WORK_BASE;
    return free_address;
}

/*
 * Returns the address of the first free slot larger than size. When no slot fits,
 * it sets the reload flag and returns RES_WORK_BASE.
 */
unsigned char *GameResourceWorkAlloc(int size)
{
    int index;
    GameResourceEntry *entry;

    GameResourceWorkReloadLeader = 0;
    entry = GameResource;
    GameResourceWorkReloadTable[0] = 0;
    index = 0;
    do {
        if (entry->state == GAME_RESOURCE_FREE) {
            if (entry->size > size)
                return entry->address;
            break;
        }
        index++;
        entry++;
    } while (index < GAME_RESOURCE_COUNT);
    GameResourceWorkReloadTable[0] = 1;
    return (unsigned char *)RES_WORK_BASE;
}

/*
 * Reloads the current scene after GameResourceWorkAlloc could not fit a block;
 * otherwise sets the map change fade.
 */
void GameResourceWorkReload(void)
{
    if (GameResourceWorkReloadTable[0] != 0) {
        ACT_init();
        GameResourceLoad(GameLoopState.scene_id);
        return;
    }
    MapChangeFadeSet();
}

/*
 * Carves size bytes (rounded up to 64) from the first free slot, splitting the slot
 * in two. Returns the address, or 0 when no slot is free.
 */
void *GameResourceAlloc(int size)
{
    int index;

    index = resource_get_free();
    if (index == -1)
        return 0;

    size = (size + 63) & ~63;
    GameResource[index + 1].address = GameResource[index].address + size;
    GameResource[index + 1].size = GameResource[index].size - size;
    GameResource[index].size = size;
    GameResource[index].state = RES_TYPE_ALLOC;
    GameResource[index].handle = xglSRand();
    return GameResource[index].address;
}

/*
 * Resizes the block at address to size (rounded up to 64) by taking or giving back
 * space from the following free slot. Returns address, or 0 when the block cannot grow.
 */
void *GameResourceRealloc(void *address, int size)
{
    int index;
    int combined_size;
    unsigned char *old_address;

    index = GameResourceGetIndex(address);
    if (index < 0)
        return 0;
    if (GameResource[index + 1].state == GAME_RESOURCE_FREE) {
        combined_size = GameResource[index].size + GameResource[index + 1].size;
        size = (size + 63) & ~63;
        old_address = GameResource[index].address;
        GameResource[index].size = size;
        GameResource[index + 1].address = old_address + size;
        GameResource[index + 1].size = combined_size - size;
        return address;
    }
    return 0;
}

/*
 * Copies the next entry of the scene archive to address, extracting ARX-compressed
 * data, and returns its size. The first call also reads the archive header.
 */
static int next_arc_size(void *address)
{
    u8 *source;
    int size;
    int remaining;
    u32 *dst;
    u32 *src;

    if (arcno == 0) {
        dst = archeader;
        src = arcfileaddr;
        remaining = *arcfileaddr >> 2;
        while (remaining >= 0) {
            *dst = *src;
            src++;
            dst++;
            remaining--;
        }
        size = archeader[1];
        source = (u8 *)arcfileaddr + ((archeader[0] + 0x40) & 0xFFFFFFC0);
    } else {
        size = archeader[arcno + 1] - archeader[arcno];
        source = (u8 *)arcfileaddr + ((archeader[0] + 0x40) & 0xFFFFFFC0u) + archeader[arcno];
    }
    arcno++;
    if (size > 0) {
        if (*(u32 *)source == ARX_SIGNATURE)
            size = xglArxExtract((u32 *)address, (u32 *)source);
        else
            memcpy(address, source, size);
    }
    if (archeader[0] >> 2 <= (u32)arcno)
        arcfileaddr = 0;
    return size;
}

/* Returns the start of the next line, skipping blank characters and ";" comment lines. */
static u8 *next_line(u8 *cursor)
{
    for (;;) {
        while (*cursor != '\n' && *cursor != '\r')
            cursor++;
        while (*cursor == ' ' || *cursor == '\t' || *cursor == '\n' || *cursor == '\r')
            cursor++;
        if (*cursor != ';')
            return cursor;
    }
}

static u8 *skip_space(u8 *cursor)
{
    while (*cursor == ' ' || *cursor == '\t')
        cursor++;
    return cursor;
}

/*
 * Matches the text at cursor against table, descending into alias tables, and
 * stores the resulting resource id in *id. Returns the input position after the
 * match; when nothing matches, *id is 0 and cursor is returned.
 */
static char *search_key(char *cursor, ResourceKeyEntry *table, int *id)
{
    ResourceKeyEntry *record = table;
    int root_id = 0;

    while (record->key != 0) {
        const char *root_key = record->key;
        char *root_input = cursor;

        for (;;) {
            char expected = *root_key;
            char root_character = *root_input;

            /* At the end of a root key, try its alias table. */
            if (expected == '\0') {
                ResourceKeyEntry *child = record->aliases.children;
                int child_id = root_id;

                if (child->key != 0) {
                    do {
                        const char *child_key = child->key;
                        char *child_input = root_input;

                        for (;;) {
                            char child_expected = *child_key;
                            char child_character = *child_input;
                            if (child_expected == '\0') {
                                const char **leaf = child->aliases.names;
                                int leaf_id = child_id + 1;

                                if (*leaf != 0) {
                                    do {
                                        const char *leaf_key = *leaf;
                                        char *leaf_input = child_input;
                                        char leaf_expected;

                                        for (;;) {
                                            char input_character;
                                            leaf_expected = *leaf_key;
                                            input_character = *leaf_input;
                                            if (leaf_expected == '+') {
                                                leaf_expected = *++leaf_key;
                                            }
                                            if (leaf_expected == ',')
                                                leaf_expected = '\0';
                                            if (leaf_expected == '\0') {
                                                if (input_character == ',' || input_character == ' ' || input_character == '\t' || input_character == '\n' || input_character == '\r') {
                                                    *id = leaf_id;
                                                    return leaf_input;
                                                }
                                            }
                                            if ((u8)(leaf_expected - 'a') < 26)
                                                leaf_expected -= 'a' - 'A';
                                            if (input_character == leaf_expected) {
                                                leaf_input++;
                                                leaf_key++;
                                            } else {
                                                leaf++;
                                                leaf_id++;
                                                break;
                                            }
                                        }
                                    } while (*leaf != 0);
                                }
                            }
                            if (child_expected >= 'a' && child_expected <= 'z')
                                child_expected -= 'a' - 'A';
                            if (child_character != child_expected)
                                break;
                            child_input++;
                            child_key++;
                        }
                        child++;
                        child_id += 0x100;
                    } while (child->key != 0);
                }
            }
            if (expected >= 'a' && expected <= 'z')
                expected -= 'a' - 'A';
            if (root_character != expected)
                break;
            root_input++;
            root_key++;
        }
        record++;
        root_id += 0x1000;
    }
    *id = 0;
    return cursor;
}

/* Parses the decimal number after the command and resets that resource slot. */
static char *command_reset(int command, char *cursor)
{
    int resource_id = 0;

    if ((unsigned char)((unsigned char)cursor[0] + 0xD0) < 10) {
        do {
            int digit = (unsigned char)cursor[0];
            int parsed_id;

            cursor++;
            resource_id = resource_id * 10;
            parsed_id = resource_id + digit;
            resource_id = parsed_id - '0';
        } while ((unsigned char)((unsigned char)cursor[0] + 0xD0) < 10);
    }
    GameResourceReset(resource_id);
    return cursor;
}

/*
 * Parses the map name after the command and loads it. A leading "-" loads the map
 * data without switching to the map.
 */
static char *command_loadmap(int command, char *cursor)
{
    MapNameGroup *group;
    int group_id;
    int name_index;
    void *load_result;
    int no_load;
    char **name_entry;
    const char *name_cursor;
    char *input_cursor;

    no_load = 0;
    if ((u8)*cursor == '-') {
        cursor++;
        no_load = 1;
    }

    group_id = 0;
    for (group = maptbl; group->names != 0; group++) {
        name_entry = group->names;
        for (name_index = 0; name_index < group->count; name_index++) {
            name_cursor = *name_entry;
            input_cursor = cursor;
            for (;;) {
                if (*name_cursor == 0 &&
                    (*input_cursor == ' ' || *input_cursor == '\t' ||
                     *input_cursor == '\n' || *input_cursor == '\r')) {
                    cursor = input_cursor;
                    group_id += name_index;
                    goto map_name_found;
                }
                if ((u8)*input_cursor++ != (u8)*name_cursor)
                    break;
                name_cursor++;
            }
            name_entry++;
        }
        group_id += MAP_GROUP_SIZE;
    }

    if (group_id >= MAP_ID_LIMIT)
        group_id = 0;

map_name_found:
    load_result = RES_loadFile(command, RES_TYPE_MAP, group_id, 0);
    if (no_load == 0) {
        GameLoopState.map_id = (s16)group_id;
        GameLoopState.map_load_result = load_result;
        MapChangeFadeSet();
        MapReset();
    }
    return cursor;
}

/* Parses a model key and loads it as a model resource. */
static char *command_loadact(int command, char *cursor, int mode)
{
    int resource_id = 0;
    char *next_cursor = search_key(cursor, model, &resource_id);

    RES_loadFile(command, RES_TYPE_MODEL, resource_id, 0);
    return next_cursor;
}

/* Parses a model key and loads its face variant (the key plus RES_ID_FACE). */
static char *command_loadface(int command, char *cursor, int mode)
{
    int resource_id = 0;
    char *next_cursor = search_key(cursor, model, &resource_id);

    RES_loadFile(command, RES_TYPE_MODEL, resource_id + RES_ID_FACE, 0);
    return next_cursor;
}

/*
 * Parses a motion name and its foot-step number, loads the named motion, and stores
 * the foot-step number in its resource.
 */
static char *command_loadmot(int command, char *cursor)
{
    char name[0x100];
    char *dst = name;
    int character;
    int motion_id;
    int first;
    short value;
    int foot_step;

    character = (u8)*cursor;
    value = character;
    while ((character & 0xff) != ',' && (character & 0xff) != ' ' &&
           (character & 0xff) != '\t' && (character & 0xff) != '\n' &&
           (character & 0xff) != '\r') {
        *dst = value;
        dst++;
        cursor++;
        character = (u8)*cursor;
        value = character;
    }
    *dst = '\0';

    for (;;) {
        first = (u8)character;
        if (first == ',' || first == ' ' || first == '\t') {
            cursor++;
            character = (u8)*cursor;
            continue;
        }
        break;
    }

    motion_id = 0;
    while ((u8)((u8)character + 0xD0) < 10U) {
        int digit;
        int parsed_id;

        digit = (u8)*cursor;
        cursor++;
        motion_id = motion_id * 10;
        parsed_id = motion_id + digit;
        motion_id = parsed_id - '0';
        character = *cursor & 0xff;
    }

    foot_step = motion_id;
    motion_id = 0;
    {
        ActorResource *resource = RES_loadFile(command, RES_TYPE_MOTION_NAMED, (int)name, 0);

        resource->foot_step_no = foot_step;
    }
    return cursor;
}

/* Returns the foot-step number of the actor's first animation pack, or 0 when it has none. */
u8 RES_GetFootStepNo(Actor *actor)
{
    ActorResource *resource = actor->animPackTables[0];
    u8 foot_step_no = 0;

    if (resource != 0) {
        foot_step_no = resource->foot_step_no;
    }
    return foot_step_no;
}

/* Parses a decimal id and loads it as a texture resource. */
static char *command_loadtex(int command, char *cursor, int mode)
{
    int resource_id = 0;

    (void)mode;
    if ((unsigned char)((unsigned char)cursor[0] + 0xD0) < 10) {
        do {
            int digit;
            int parsed_id;

            digit = (unsigned char)cursor[0];
            cursor++;
            resource_id = resource_id * 10;
            parsed_id = resource_id + digit;
            resource_id = parsed_id - '0';
        } while ((unsigned char)((unsigned char)cursor[0] + 0xD0) < 10);
    }
    RES_loadFile(command, RES_TYPE_TEXTURE, resource_id, 0);
    return cursor;
}

/* Parses a decimal id and loads it as a movie resource. */
static char *command_loadmovie(int command, char *cursor, int mode)
{
    int resource_id = 0;

    (void)mode;
    if ((unsigned char)((unsigned char)cursor[0] + 0xD0) < 10) {
        do {
            int digit;
            int parsed_id;

            digit = (unsigned char)cursor[0];
            cursor++;
            resource_id = resource_id * 10;
            parsed_id = resource_id + digit;
            resource_id = parsed_id - '0';
        } while ((unsigned char)((unsigned char)cursor[0] + 0xD0) < 10);
    }
    RES_loadFile(command, RES_TYPE_MOVIE, resource_id, 0);
    return cursor;
}

/* Loads the effect named effect_name into address, from the archive when one is open. */
static void command_loadeffect_sub(int type, char *effect_name, void *address)
{
    if (arcfileaddr != 0) {
        sefLoadMemoryEffectCfName(address, effect_name, next_arc_size(address));
        return;
    }
    sefLoadEffectCfName((int)address, effect_name);
}

/* Parses an effect name up to the next blank and loads it as an effect resource. */
static char *command_loadeffect(int command, char *cursor, int mode)
{
    char effect_name[256];
    char *dst;
    int character;

    dst = effect_name;
    character = (u8)*cursor;
    if ((u8)character != ' ' && (u8)character != '\t' && (u8)character != '\n' && (u8)character != '\r') {
        do {
            *dst = character;
            dst++;
            cursor++;
            character = (u8)*cursor;
        } while ((u8)character != ' ' && (u8)character != '\t' && (u8)character != '\n' && (u8)character != '\r');
    }
    *dst = 0;
    RES_loadFile(command, RES_TYPE_EFFECT, (int)effect_name, (int)command_loadeffect_sub);
    return cursor;
}

/* Loads the enemy preset into the preset address, from the archive when one is open. */
static int command_loadene_sub(int type, const char *name, void *address)
{
    if (arcfileaddr != 0)
        next_arc_size(AdrsEnemyPreset);
    else
        Enemy_LoadPreset(AdrsEnemyPreset, name);

    return 0;
}

/* Parses an enemy preset name and loads it as an enemy preset resource. */
static char *command_loadene(int command, char *cursor, int mode)
{
    char name[256];
    char *dst = name;
    int character = (u8)*cursor;
    if ((u8)character != ' ' && (u8)character != '\t' && (u8)character != '\n' && (u8)character != '\r') {
        for (;;) {
            *dst = character;
            dst++;
            cursor++;
            character = (u8)*cursor;
            if ((u8)character == ' ' || (u8)character == '\t' || (u8)character == '\n' || (u8)character == '\r')
                break;
        }
    }
    *dst = 0;
    RES_loadFile(command, RES_TYPE_ENEMY_PRESET, (int)name, (int)&command_loadene_sub);
    return cursor;
}

/*
 * Parses a script name, appends ".evt" to the current scene directory, and loads
 * and runs it unless flag 0x40 is set.
 */
static char *command_script(int command, char *cursor, int mode)
{
    char script_name[256];
    char *dst;
    const char *path;
    int character;

    dst = script_name;
    path = RES_getPath();
    while ((*dst = *path) != 0) {
        path++;
        dst++;
    }
    character = (u8)*cursor;
    if ((u8)character != ' ' && (u8)character != '\t' && (u8)character != '\n' && (u8)character != '\r') {
        for (;;) {
            *dst = character;
            dst++;
            cursor++;
            character = (u8)*cursor;
            if ((u8)character == ' ' || (u8)character == '\t' || (u8)character == '\n' || (u8)character == '\r')
                break;
        }
    }
    dst[0] = '.';
    dst[1] = 'e';
    dst[2] = 'v';
    dst[3] = 't';
    dst[4] = 0;
    if ((GameLoopState.flags & 0x40) == 0) {
        SCRIPT_load2(script_name);
        SCRIPT_exec2();
    }
    return cursor;
}

static char *command_player(int command, char *cursor, int mode)
{
    GameCfPlayerLoadResource(0);
    return cursor;
}

/*
 * Allocates the slot for resource_id when it is empty, otherwise reallocates it.
 * Returns result.
 */
static int command_dummy(int resource_id, int result)
{
    if (GameResource[resource_id + 1].address == 0)
        GameResourceAlloc(0);
    else
        GameResourceRealloc(GameResource[resource_id].address, 0);
    return result;
}

/*
 * Writes the leader foot-step sound name to dst: the two-letter prefix of the
 * leader bank entry, "_", then foot_name.
 */
void RES_GetLeaderSeName(char *dst)
{
    struct SaveDataLeaderSeIndex *save = (struct SaveDataLeaderSeIndex *)SaveData;
    const char *src = tbl_1_00366470[save->leader_se_index];
    char *out = dst;
    u8 second;

    out[0] = src[0];
    second = src[1];
    src = foot_name;
    out[2] = '_';
    out[1] = second;
    out += 3;
    while ((*out = *src) != 0) {
        out++;
        src++;
    }
}

void RES_GetMapEnvSeName(char *name)
{
    char *env = env_name;

    while ((*name = *env) != 0) {
        env++;
        name++;
    }
}

/*
 * Parses the foot-step, environment and sequence fields of a loadse command, separated
 * by blanks or commas. Loads the matching effect banks and sequence.
 */
static u8 *command_loadse(int command, u8 *cursor)
{
    char sound_name[0x40];
    char sequence_name[0x40];
    char path[0x100];
    int character;
    unsigned char *address;
    u16 file_id;
    int keep_current;
    u8 set_normal;
    int first_size;
    int second_size;
    char *name_end;
    char *sequence_end;
    void *second_address;

    address = GameResourceGetFreeAddr();
    name_end = sound_name;
    for (;;) {
        character = *cursor;
        if ((character & 0xff) == ',' || (character & 0xff) == ' ' ||
            (character & 0xff) == '\t' || (character & 0xff) == '\n' ||
            (character & 0xff) == '\r')
            break;
        *name_end++ = character;
        cursor++;
    }
    *name_end = 0;

    keep_current = 0;
    for (;;) {
        character = *cursor;
        if ((character & 0xff) == ',' || (character & 0xff) == ' ' ||
            (character & 0xff) == '\t') {
            cursor++;
            continue;
        }
        break;
    }
    name_end = env_name;
    goto environment_name_test;

environment_name_copy:
    if (character == '-') {
        keep_current = 1;
        cursor++;
        goto environment_name_done;
    }
    *name_end++ = character;
    character = *++cursor;

environment_name_test:
    if ((u8)character != ',' && (u8)character != ' ' &&
        (u8)character != '\t' && (u8)character != '\n' &&
        (u8)character != '\r')
        goto environment_name_copy;

environment_name_done:
    *name_end = 0;

    set_normal = 0;
    for (;;) {
        character = *cursor;
        if ((character & 0xff) == ',' || (character & 0xff) == ' ' ||
            (character & 0xff) == '\t') {
            cursor++;
            continue;
        }
        break;
    }
    sequence_end = sequence_name;
    while ((u8)character != ',' && (u8)character != ' ' &&
           (u8)character != '\t' && (u8)character != '\n' &&
           (u8)character != '\r') {
        if ((u8)character == '+') {
            set_normal = 1;
            break;
        }
        *sequence_end++ = character;
        character = *++cursor;
    }
    *sequence_end = 0;

    /* Leader foot-step sounds: an empty name loads the default effects. */
    if (sound_name[0] == 0) {
        xglSoundLoadEffect(0, 0, SOUND_BANK_FOOT);
    } else {
        const char *src;
        char *dst = foot_name;
        if (sound_name[0] == 'S' && sound_name[1] == 'O' && sound_name[2] == '_')
            src = sound_name + 3;
        else
            src = sound_name;
        while ((*dst = *src) != 0) {
            src++;
            dst++;
        }
    }

    /* Map environment effects. A "-" name keeps the bank contents when they still play. */
    if (env_name[0] == 0) {
        xglSoundLoadEffect(0, 0, SOUND_BANK_ENV);
    } else {
        file_id = SoundWork.effect_banks[SOUND_BANK_ENV].handle;
        ((SoundLoadBuffer *)address)->sequence_id = file_id;
        if (arcfileaddr != 0) {
            first_size = next_arc_size((void *)address);
            second_address = (void *)(address + first_size);
            second_size = next_arc_size(second_address);
        } else {
            sprintf(path, soundSwdPathFormat, env_name);
            first_size = xglCdReadFile(path, address, 0, 0);
            second_address = address + first_size;
            sprintf(path, soundSedPathFormat, env_name);
            second_size = xglCdReadFile(path, second_address, 0, 0);
        }
        if (file_id != ((SoundLoadBuffer *)address)->sequence_id) {
            keep_current = 0;
        } else {
            int request_status;
            int effect_result;
            int sequence_result;
            SsdCheckPlayEffectFileID(file_id);
            do { request_status = SsdGetResultValue(&effect_result); } while (request_status < 0);
            SsdGetSeqPlayStatus(file_id);
            do { request_status = SsdGetResultValue(&sequence_result); } while (request_status < 0);
            if (effect_result == 0) {
                if (sequence_result != 1)
                    keep_current = 0;
            }
        }
        if (!keep_current && first_size > 0 && second_size > 0)
            xglSoundSendEffect(address, second_address, SOUND_BANK_ENV);
    }

    /* A "." sequence name sends no sequence; any other name loads and sends its SMD. */
    if (sequence_name[0] != 0) {
        if (sequence_name[0] == '.') {
            xglSoundSendSmd(0);
        } else {
            int size;
            if (arcfileaddr != 0) {
                size = next_arc_size(address);
            } else {
                sprintf(path, soundSmdPathFormat, sequence_name);
                size = xglCdReadFile(path, address, 0, 0);
            }
            if (size != 0) {
                xglSoundSendSmd(address);
                if (set_normal)
                    xglSoundSequenceNormal(SOUND_VOLUME_MAX);
            }
        }
    }
    return cursor;
}

/*
 * Parses a list of sequence names, one per sequence slot, and sends each loaded
 * sequence to its slot. A "+" prefix sets normal volume; a "-" prefix skips the send
 * when that sequence is already playing in the slot.
 */
static u8 *command_loadsmd(int command, u8 *cursor)
{
    char name[0x40];
    char path[0x100];
    s16 character;
    unsigned char *address;
    int sequence_index;
    int first_character;

    sequence_index = 0;
    address = GameResourceGetFreeAddr();
    first_character = *cursor;
    if ((u8)first_character != '\n' && (u8)first_character != '\r') {
        int separator_value;

        do {
            char *name_end;
            int stop_current;
            int set_normal;
            int file_size;
            int request_status;
            int sequence_status;
            u16 sequence_id;

            character = *cursor;
            set_normal = 0;
            stop_current = 0;
            name_end = name;
            if ((u8)character != ',' && (u8)character != ' ' &&
                (u8)character != '\t' && (u8)character != '\n' &&
                (u8)character != '\r') {
                for (;;) {
                    if ((character & 0xff) == '+') {
                        cursor++;
                        set_normal = 1;
                    } else if ((character & 0xff) == '-') {
                        cursor++;
                        stop_current = 1;
                    } else {
                        *name_end++ = character;
                        cursor++;
                    }
                    character = *cursor;
                    if ((u8)character == ',' || (u8)character == ' ' ||
                        (u8)character == '\t' || (u8)character == '\n' ||
                        (u8)character == '\r')
                        break;
                }
            }

            sequence_id = SoundWork.channels[sequence_index].sequence;
            *name_end = '\0';
            ((SoundLoadBuffer *)address)->sequence_id = sequence_id;
            if (arcfileaddr != 0) {
                file_size = next_arc_size((void *)address);
            } else {
                const char *sequence_format = soundSmdPathFormat;

                sprintf(path, sequence_format, name);
                file_size = xglCdReadFile(path, address, 0, 0);
            }

            if (sequence_id != ((SoundLoadBuffer *)address)->sequence_id) {
                stop_current = 0;
            } else {
                SsdGetSeqPlayStatus(sequence_id);
                do {
                    request_status = SsdGetResultValue(&sequence_status);
                } while (request_status < 0);
                if (sequence_status != 1)
                    stop_current = 0;
            }
            if (!stop_current && file_size > 0) {
                xglSoundSendSmd2(address, sequence_index);
                if (set_normal)
                    xglSoundSequenceNormal2(sequence_index, SOUND_VOLUME_MAX);
            }

            for (;;) {
                first_character = *cursor;
                separator_value = first_character & 0xff;
                if (separator_value == ',' || separator_value == ' ' ||
                    separator_value == '\t') {
                    cursor++;
                } else {
                    break;
                }
            }
            sequence_index++;
        } while (separator_value != '\n' && separator_value != '\r');
    }
    return cursor;
}

/* Parses a movie name, appends ".pss", and plays it in the given mode. */
static char *command_mpeg2_core(int command, char *cursor, int mode)
{
    char movie_name[256];
    char *dst;
    const char *base;
    int character;

    dst = movie_name;
    base = base_2;
    while ((*dst = *base) != 0) {
        dst++;
        base++;
    }
    character = (u8)*cursor;
    if ((u8)character != ' ' && (u8)character != '\t' && (u8)character != '\n' && (u8)character != '\r') {
        for (;;) {
            *dst = (u8)character;
            cursor++;
            character = (u8)*cursor;
            dst++;
            if ((u8)character == ' ' || (u8)character == '\t' || (u8)character == '\n' || (u8)character == '\r')
                break;
        }
    }
    dst[0] = '.';
    dst[1] = 'p';
    dst[2] = 's';
    dst[3] = 's';
    dst[4] = 0;
    GameMpeg2Play(movie_name, mode);
    return cursor;
}

static char *command_mpeg2(int command, char *cursor)
{
    return command_mpeg2_core(command, cursor, MPEG2_MODE_NORMAL);
}

static char *command_mpeg2battle(int command, char *cursor, int mode)
{
    SCRIPT_fade(2);
    return command_mpeg2_core(command, cursor, MPEG2_MODE_BATTLE);
}

static char *command_mpeg2nofade(int command, char *cursor)
{
    return command_mpeg2_core(command, cursor, MPEG2_MODE_NOFADE);
}

static char *command_event2battle(int command, char *cursor, int mode)
{
    SCRIPT_frameLock2Battle();
    return cursor;
}

static char *command_player_lock(int command, char *cursor, int mode)
{
    GameLoopState.flags |= GAME_LOOP_FLAG_PLAYER_LOCK;
    return cursor;
}

/*
 * Parses an enemy or NPC sound command (model key, sound name, foot-step number),
 * records it in EnemySeBank, and loads its effect bank. mode is the ENEMY_SE_* kind.
 */
static char *command_enenpcse(int command, char *cursor, int mode)
{
    char name[0x40];
    char *dst;
    char *next;
    int character;
    int resource_id;
    int motion_id;
    int slot;
    unsigned char *address;
    int first_size;
    int second_size;
    void *second_address;

    resource_id = 0;
    next = search_key(cursor, model, &resource_id);
    for (;;) {
        character = (u8)*next;
        if ((character & 0xff) == ',' || (character & 0xff) == ' ' ||
            (character & 0xff) == '\t') {
            next++;
            continue;
        }
        break;
    }
    dst = name;
    while ((u8)character != ',' && (u8)character != ' ' &&
           (u8)character != '\t' && (u8)character != '\n' &&
           (u8)character != '\r') {
        *dst++ = character;
        next++;
        character = (u8)*next;
    }
    *dst = '\0';
    for (;;) {
        character = (u8)*next;
        if ((character & 0xff) == ',' || (character & 0xff) == ' ' ||
            (character & 0xff) == '\t') {
            next++;
            continue;
        }
        break;
    }
    motion_id = 0;
    while ((u8)((u8)character + 0xD0) < 10U) {
        motion_id *= 10;
        motion_id += (u8)*next - '0';
        next++;
        character = *next & 0xff;
    }

    /* Use the first free enemy sound slot, or the last one when all are taken. */
    address = GameResourceGetFreeAddr();
    for (slot = 0; slot < ENEMY_SE_BANK_COUNT - 1; slot++) {
        if (EnemySeBank[slot].id == 0)
            break;
    }
    EnemySeBank[slot].id = resource_id;
    EnemySeBank[slot].type = mode;
    EnemySeBank[slot].foot = motion_id;
    strcpy(EnemySeBank[slot].name, name);

    if (arcfileaddr != 0) {
        first_size = next_arc_size(address);
        second_address = address + first_size;
        second_size = next_arc_size(second_address);
        if (first_size != 0 && second_size != 0)
            xglSoundSendEffect(address, second_address, slot + SOUND_BANK_ENEMY_BASE);
    } else {
        xglSoundLoadEffect(name, address, slot + SOUND_BANK_ENEMY_BASE);
    }
    return next;
}

static char *command_enese(int command, char *cursor, int mode)
{
    return command_enenpcse(command, cursor, ENEMY_SE_ENEMY);
}

static char *command_npcse(int command, char *cursor, int mode)
{
    return command_enenpcse(command, cursor, ENEMY_SE_NPC);
}

static char *command_disk(int command, char *cursor, int mode)
{
    GameDiskChange((unsigned char)cursor[0] - '0');
    return cursor;
}

static char *command_save(int command, char *cursor, int mode)
{
    int save_number = 0;

    (void)mode;
    if ((unsigned char)((unsigned char)cursor[0] + 0xD0) < 10) {
        do {
            int digit = (unsigned char)cursor[0];
            int parsed_number;

            cursor++;
            save_number = save_number * 10;
            parsed_number = save_number + digit;
            save_number = parsed_number - '0';
        } while ((unsigned char)((unsigned char)cursor[0] + 0xD0) < 10);
    }
    Intermission(save_number);
    return cursor;
}

/*
 * Reads the scene script of scene_number. When its archive (the same name with an
 * "a" extension) exists, preloads it near the top of EE RAM for GameResourceLoad.
 */
void GameResourcePreLoad(int scene_number)
{
    char path[256];
    char *archive_path;
    int archive_size;

    archive_path = RES_getScenePath(path, scene_number);
    arcfilepreload = 0;
    if (xglCdGetFileSize(path) > 0) {
        xglCdReadFile(path, scene_txt_buffer.text, 1, 1);

        archive_path[0] = 'a';
        archive_path[1] = '\0';
        archive_size = xglCdGetFileSize(path);
        if (archive_size > 0) {
            arcfilepreload = EE_RAM_END - ((archive_size + 2047) & -2048);
            xglCdReadFile(path, (void *)arcfilepreload, 1, 1);
        }
    }
}

static void readreq(const char *name, void *buffer)
{
    xglCdReadFile(name, buffer, 0, 1);
}

/*
 * Loads the resources of scene_id: reads its script and archive (or uses the
 * preloaded archive), then runs each command line of the script up to #end.
 */
void GameResourceLoad(int scene_id)
{
    char path[0x100];
    u8 *cursor;
    char *extension;
    int file_size;

    arcno = 0;
    GameLoopState.scene_id = scene_id;
    if (arcfilepreload != 0) {
        arcfileaddr = (u32 *)arcfilepreload;
    } else {
        scene_txt_buffer.header.signature = SCENE_TXT_EMPTY_FILL;
        arcfileaddr = 0;
        extension = RES_getScenePath(path, scene_id);
        readreq(path, scene_txt_buffer.text);
        *extension++ = 'a';
        *extension = '\0';
        file_size = xglCdGetFileSize(path);
        if (file_size > 0) {
            arcfileaddr = (u32 *)(EE_RAM_END - ((file_size + 0x7FF) & ~0x7FF));
            xglCdReadFile(path, arcfileaddr, 0, 1);
        }
    }

    cursor = scene_txt_buffer.text;
    if (scene_txt_buffer.header.signature == SCENE_TXT_SIGNATURE) {
        int resource_id;

        /* Each command line starts with a resource number, then the command name. */
        cursor = next_line(cursor);
        for (;;) {
            GameResourceCommand *entry;

            if (*cursor == '#') {
                if (cursor[1] == 'e' && cursor[2] == 'n' && cursor[3] == 'd')
                    break;
                continue;
            } else {
                resource_id = 0;
                cursor = skip_space(cursor);
                if ((unsigned char)(*cursor + 0xD0) < 10) {
                    do {
                        resource_id *= 10;
                        resource_id += *cursor++ - '0';
                    } while ((unsigned char)(*cursor + 0xD0) < 10);
                }
                cursor = skip_space(cursor);
                entry = command;
                {
                int matched = 0;
                while (entry->name != 0) {
                    const char *name = entry->name;
                    u8 *scan = cursor;
                    char name_character;

                    for (;;) {
                        u8 name_byte;

                        name_character = *name;
                        name_byte = (u8)*name++;
                        if (name_character == '\0' &&
                            ((char)*scan == ' ' || (char)*scan == '\t' ||
                             (char)*scan == '\n' || (char)*scan == '\r')) {
                            cursor = scan;
                            matched = 1;
                            break;
                        }
                        if (*scan != name_byte)
                            break;
                        scan++;
                    }
                    if (matched)
                        break;
                    entry++;
                }
                if (matched) {
                    cursor = skip_space(cursor);
                    cursor = (u8 *)entry->callback(resource_id, (char *)cursor);
                }
                }
            }
            cursor = next_line(cursor);
        }
        arcfileaddr = 0;
        arcfilepreload = 0;
    }
}

/*
 * Reads the map file name with suffix from the map directory into buffer, using the
 * dummy map file when it is missing. Returns the file size.
 */
static int RES_loadFileSubMapSub(char *name, const char *suffix, void *buffer)
{
    char path[0x100];
    char *dst = path;
    char *fallback_dst;
    const char *prefix_cursor;
    const char *fallback_cursor;
    int size;

    prefix_cursor = MAP_getPath(name);
    while (*prefix_cursor != 0) {
        *dst++ = *prefix_cursor++;
    }
    prefix_cursor = name;
    while (*prefix_cursor != 0) {
        *dst++ = *prefix_cursor++;
    }
    prefix_cursor = suffix;
    if (*prefix_cursor != 0) {
        do {
            *dst++ = *prefix_cursor++;
        } while (*prefix_cursor != 0);
    }
    *dst = 0;
    size = xglCdGetFileSize(path);
    if (size <= 0) {
        fallback_dst = path;
        fallback_cursor = dummy_3;
        while (*fallback_cursor != 0) {
            *fallback_dst++ = *fallback_cursor++;
        }
        fallback_cursor = suffix;
        while (*fallback_cursor != 0) {
            *fallback_dst++ = *fallback_cursor++;
        }
        *fallback_dst = 0;
        size = xglCdGetFileSize(path);
    }
    readreq(path, buffer);
    return size;
}

/*
 * Loads the lex, texture, id and think files of the map scene_id into end_address,
 * each aligned to 16 bytes. Returns the total size of the map block.
 */
static int RES_loadFileSubMap(u8 *end_address, int scene_id)
{
    char *map_name;
    SubMapLoadState *state = (void *)end_address;
    int file_size;
    char path[0x100];

    state->matrix = state->matrix_storage;
    xglMatrixUnit(state->matrix);
    end_address = (u8 *)(state->matrix + 4);
    map_name = MapGetName(scene_id);
    state->lex_file = end_address;

    if (arcfileaddr != 0)
        file_size = next_arc_size(end_address);
    else
        file_size = RES_loadFileSubMapSub(map_name, lexSuffix, end_address);
    end_address = (u8 *)(((u32)end_address + file_size + 0xF) & ~0xF);
    state->texture_file = end_address;

    if (arcfileaddr != 0)
        file_size = next_arc_size(end_address);
    else
        file_size = RES_loadFileSubMapSub(map_name, xtxSuffix,
                                           end_address);
    if (file_size <= 0) {
        state->texture_file = 0;
    } else {
        end_address = (u8 *)(((u32)end_address + file_size + 0xF) & ~0xF);
    }
    state->id_file = end_address;

    if (arcfileaddr != 0)
        file_size = next_arc_size(end_address);
    else
        file_size = RES_loadFileSubMapSub(map_name, idSuffix,
                                            end_address);
    if (file_size <= 0) {
        state->id_file = 0;
    } else {
        end_address = (u8 *)(((u32)end_address + file_size + 0xF) & ~0xF);
    }
    state->think_file = end_address;

    if (func_A1CF18(path, scene_id) != 0) {
        if (arcfileaddr != 0)
            file_size = next_arc_size(state->think_file);
        else
            file_size = xglCdReadFile(path, state->think_file, 0, 1);
    } else {
        file_size = -1;
    }
    if (file_size <= 0) {
        state->think_file = 0;
    } else {
        end_address = (u8 *)(((u32)end_address + file_size + 0xF) & ~0xF);
    }
    return (u32)end_address - (u32)state;
}

/*
 * Returns the name of the cfMtnList group that contains id, or the first group's
 * name when none does.
 */
const char *RES_GetMotBaseName(int id)
{
    MotionEntry *entry = cfMtnList;
    u16 *slot;

    id &= 0xffff;
    while (entry->name != 0) {
        slot = entry->ids;
        while (*slot != 0) {
            if (*slot == id)
                return entry->name;
            slot++;
        }
        entry++;
    }
    return cfMtnList[0].name;
}

/*
 * Writes the model name for selector into dst: the default name for an id of 0, or
 * the alias name of its group. Returns the converted selector, or 0 when the alias
 * group has no entry.
 */
static int RES_GetMdlFileNameSub(char *dst, int selector)
{
    int face_mode = (((u32)selector & RES_ID_FACE_MASK) == RES_ID_FACE);
    int model_index;
    int alias_group;
    int alias_index;
    const char *face_suffix = 0;
    const char *const *names;
    const char *name;
    const ResourceKeyEntry *model_entry;
    const ResourceKeyEntry *alias_entry;
    char *out;

    selector = (u16)selector;
    if (face_mode) {
        face_suffix = faceSuffix;
        selector = convert_face(selector);
    }
    model_index = (selector & 0x7000) >> 12;
    alias_group = (selector & 0xF00) >> 8;
    model_entry = &model[model_index];

    if ((selector & 0xFF) == 0) {
        const char *default_prefix = root_4;

        out = dst;
        while ((*out = *default_prefix) != '\0') {
            out++;
            default_prefix++;
        }
        default_prefix = model_entry->path_prefix;
        while ((*out = *default_prefix) != '\0') {
            out++;
            default_prefix++;
        }
        default_prefix = dummy_5;
        while ((*out = *default_prefix) != '\0') {
            out++;
            default_prefix++;
        }
        if (face_suffix != 0) {
            default_prefix = face_suffix;
            while ((*out = *default_prefix) != '\0') {
                out++;
                default_prefix++;
            }
        }
        return selector;
    }

    /* Select the alias name at index alias_index in this group. */
    alias_entry = model_entry->aliases.children + alias_group;
    names = alias_entry->aliases.names;
    alias_index = selector & 0xff;
    for (; names != 0; names++) {
        if (--alias_index == 0)
            break;
    }
    if (names != 0) {
        out = dst;
        name = root_4;
        while ((*out = *name) != '\0') {
            out++;
            name++;
        }
        name = model_entry->path_prefix;
        while ((*out = *name) != '\0') {
            out++;
            name++;
        }
        name = alias_entry->path_prefix;
        while ((*out = *name) != '\0') {
            out++;
            name++;
        }

        name = *names;
        if (face_suffix != 0) {
            char name_character;

            if (*name == '+')
                name++;
            while ((name_character = *name++) != '\0' && name_character != ',')
                *out++ = name_character;
            name = face_suffix;
            while ((*out = *name) != '\0') {
                out++;
                name++;
            }
        } else {
            u8 name_byte = (u8)*name;

            if (*name == '+') {
                char alias_separator = ',';

                name++;
                for (;;) {
                    name_byte = (u8)*name++;
                    if ((char)name_byte == alias_separator)
                        break;
                }
                name_byte = (u8)*name;
            }
            *out = name_byte;
            if (name_byte != 0) {
                do {
                    out++;
                    name++;
                } while ((*out = *name) != '\0');
            }
        }
        return selector;
    }
    *dst = '\0';
    return 0;
}

/*
 * Writes the model file name for selector into dst. When ModelPath is set, a "test"
 * directory is inserted after the first backslash.
 */
void RES_GetMdlFileName(char *dst, int selector)
{
    char name[256];
    char *src;
    char *out;
    int inserted;
    char character;

    if (ModelPath == 0) {
        RES_GetMdlFileNameSub(dst, selector);
    } else {
        RES_GetMdlFileNameSub(name, selector);
        src = name;
        out = dst;
        inserted = 0;
        while ((*out = *src) != 0) {
            character = *src;
            if (character == '\\' && inserted == 0) {
                out[5] = character;
                inserted = 1;
                out[1] = 't';
                out[2] = 'e';
                out[3] = 's';
                out[4] = 't';
                out += 5;
            }
            out++;
            src++;
        }
    }
}

/*
 * Loads one resource of the type in the low 16 bits of callback into address.
 * Returns the number of bytes it occupies, or 0 or less when its file is missing.
 */
static int RES_loadFileSub(int callback, int resource_id, void *address)
{
    char single_path[256];
    char model_path[256];
    char motion_path[256];
    char named_path[256];
    char effect_path[256];
    char movie_path[256];
    char texture_path[256];
    char *path;
    char *end;
    const char *source;
    int size = 0;
    int type = callback & 0xffff;
    int quotient;

    switch (type) {
    case RES_TYPE_JOINTS: {
        char *single_end;

        RES_GetMdlFileName(single_path, resource_id);
        single_end = single_path;
        while (*single_end != 0)
            single_end++;
        if (arcfileaddr != 0) {
            size = next_arc_size(address);
            break;
        }
        __builtin_memcpy(single_end, jntSuffix, 5);
        size = xglCdGetFileSize(single_path);
        if (size > 0)
            readreq(single_path, address);
        break;
    }
    /* A model block is its lex file, then its texture file (not for face ids), then its joint file. */
    case RES_TYPE_MODEL:
    case RES_TYPE_MODEL_NO_JOINTS: {
        char *model_end;
        u8 *next;
        ResourceFileSet *files;

        next = (u8 *)address + 16;
        RES_GetMdlFileName(model_path, resource_id);
        model_end = model_path;
        while (*model_end != 0)
            model_end++;
        files = address;
        files->first_file = 0;
        files->second_file = 0;
        files->third_file = 0;
        if (arcfileaddr != 0) {
            size = next_arc_size(next);
            if (size <= 0)
                break;
        } else {
            __builtin_memcpy(model_end, lexSuffix, 5);
            size = xglCdGetFileSize(model_path);
            if (size <= 0)
                break;
            readreq(model_path, next);
        }
        files->first_file = next;
        next += (size + 15) & ~15;
        if ((resource_id & RES_ID_FACE_MASK) == 0) {
            if (arcfileaddr != 0) {
                size = next_arc_size(next);
            } else {
                __builtin_memcpy(model_end, xtxSuffix, 5);
                size = xglCdGetFileSize(model_path);
                if (size > 0)
                    readreq(model_path, next);
            }
            if (size > 0) {
                files->second_file = next;
                next += (size + 15) & ~15;
            }
        }
        if (type == RES_TYPE_MODEL) {
            if (arcfileaddr != 0) {
                size = next_arc_size(next);
            } else {
                __builtin_memcpy(model_end, jntSuffix, 5);
                size = xglCdGetFileSize(model_path);
                if (size > 0)
                    readreq(model_path, next);
            }
            if (size > 0) {
                files->third_file = next;
                next += (size + 15) & ~15;
            }
        }
        size = (u32)next - (u32)address;
        break;
    }
    case RES_TYPE_MAP:
        size = RES_loadFileSubMap(address, resource_id);
        break;
    case RES_TYPE_MOTION:
        if (arcfileaddr != 0) {
            size = next_arc_size(address);
            break;
        }
        sprintf(motion_path, motionFpkPathFormat, RES_GetMotBaseName(resource_id));
        size = xglCdGetFileSize(motion_path);
        if (size > 0) {
            size = xglCdGetFileSize(motion_path);
            readreq(motion_path, address);
        }
        break;
    case RES_TYPE_MOTION_NAMED: {
        int append_suffix;
        char *named_end;
        const char *named_source;
        const char *resource_name = (const char *)resource_id;

        if (arcfileaddr != 0) {
            size = next_arc_size(address);
            break;
        }
        named_end = named_path;
        named_source = resource_name + 1;
        if (resource_name[0] != '\\') {
            named_source = base_6;
            while ((*named_end = *named_source) != 0) {
                named_end++;
                named_source++;
            }
            named_source = resource_name;
        }
        *named_end = *named_source;
        append_suffix = 1;
        while (*named_end != 0) {
            if (*named_source == '.')
                append_suffix = 0;
            named_source++;
            named_end++;
            *named_end = *named_source;
        }
        if (append_suffix) {
            named_end[0] = '.';
            named_end[1] = 'f';
            named_end[2] = 'p';
            named_end[3] = 'k';
            named_end[4] = 0;
        }
        size = xglCdGetFileSize(named_path);
        if (size > 0) {
            size = xglCdGetFileSize(named_path);
            readreq(named_path, address);
        }
        break;
    }
    case RES_TYPE_MOVIE:
        if (arcfileaddr != 0) {
            size = next_arc_size(address);
            break;
        }
        path = effect_path;
        end = path;
        source = base_7;
        while ((*end = *source) != 0) {
            end++;
            source++;
        }
        quotient = resource_id;
        end[-5] = (u8)end[-5] + quotient % 10;
        quotient /= 10;
        end[-6] = (u8)end[-6] + quotient % 10;
        quotient /= 10;
        end[-7] = (u8)end[-7] + quotient % 10;
        quotient /= 10;
        end[-8] = (u8)end[-8] + quotient % 10;
        size = xglCdGetFileSize(path);
        if (size > 0)
            readreq(path, address);
        break;
    case RES_TYPE_TEXTURE:
        if (arcfileaddr != 0) {
            size = next_arc_size(address);
            break;
        }
        path = movie_path;
        end = path;
        source = base_8;
        while ((*end = *source) != 0) {
            end++;
            source++;
        }
        quotient = resource_id;
        end[-5] = (u8)end[-5] + quotient % 10;
        quotient /= 10;
        end[-6] = (u8)end[-6] + quotient % 10;
        quotient /= 10;
        end[-7] = (u8)end[-7] + quotient % 10;
        quotient /= 10;
        end[-8] = (u8)end[-8] + quotient % 10;
        size = xglCdGetFileSize(path);
        if (size > 0)
            readreq(path, address);
        break;
    case RES_TYPE_FILE: {
        char *texture_end;
        const char *texture_source;

        texture_end = texture_path;
        texture_source = base_9;
        while ((*texture_end = *texture_source) != 0) {
            texture_end++;
            texture_source++;
        }
        texture_source = (const char *)resource_id;
        while ((*texture_end = *texture_source) != 0) {
            texture_end++;
            texture_source++;
        }
        *texture_end = 0;
        size = xglCdGetFileSize(texture_path);
        if (size > 0) {
            int offset = callback >> 16;

            size += offset;
            readreq(texture_path, (u8 *)address + offset);
        }
        break;
    }
    default:
        break;
    }

    return size;
}

/*
 * Loads a resource into slot command (or into a free slot for RES_LOAD_ANY_BLOCK),
 * using the loader flags or RES_loadFileSub. Returns the resource address, or 0 on failure.
 */
void *RES_loadFile(int command, int callback, int resource_id, int flags)
{
    unsigned char *result = 0;
    int slot;
    int loaded_size;
    int old_span;

    if (command == RES_LOAD_ANY_BLOCK) {
        command = GameResourceSearch(callback, resource_id);
        if (command < 0 && callback == RES_TYPE_MODEL_NO_JOINTS) {
            command = GameResourceSearch(RES_TYPE_MODEL, resource_id);
        } else if (command < 0 && callback == RES_TYPE_MODEL) {
            command = GameResourceSearch(RES_TYPE_MODEL_NO_JOINTS, resource_id);
            if (command >= 0) {
                ResourceFileSet *resource_info = (ResourceFileSet *)GameResource[command].address;

                if (resource_info->third_file == 0) {
                    slot = resource_get_free();
                    resource_info->third_file = GameResource[slot].address;
                    RES_loadFile(slot, RES_TYPE_JOINTS, resource_id, flags);
                }
            }
        }
        if (command >= 0) {
            result = GameResource[command].address;
        } else {
            command = resource_get_free();
            if (command != -1)
                result = RES_loadFile(command, callback, resource_id, flags);
        }
    } else if (command < GAME_RESOURCE_COUNT) {
        result = GameResource[command].address;
        if (flags == 0)
            flags = (int)RES_loadFileSub;
        loaded_size = ((int (*)(int, int, void *))flags)(callback, resource_id, result);
        if (loaded_size < 0)
            return 0;
        {
            loaded_size = (loaded_size + 0x3F) & ~0x3F;
            old_span = GameResource[command].size;
            if (loaded_size > old_span)
                loaded_size = old_span;
            if (command < GAME_RESOURCE_COUNT - 1 && GameResource[command + 1].address == 0) {
                GameResource[command + 1].size = old_span - loaded_size;
                GameResource[command + 1].address = result + loaded_size;
                GameResource[command].size = loaded_size;
            }
            resource_typeid_translate(command, &callback, &resource_id);
            GameResource[command].state = callback;
            GameResource[command].handle = resource_id;
        }
    }
    return result;
}

/* Returns the index of the resource slot at address, or -1 when there is none. */
int GameResourceGetIndex(void *address)
{
    int index = 0;
    GameResourceEntry *entry = GameResource;
    unsigned char *resourceAddress;
    int result;

    do {
        if (index >= GAME_RESOURCE_COUNT)
            break;
        resourceAddress = entry->address;
        entry++;
        result = index++;
        if (resourceAddress == address)
            return result;
    } while (resourceAddress != 0);
    return -1;
}

/* When dump is nonzero, prints a page of the resource table with the debug font. */
void GameResourceDump(int dump)
{
    GameResourceEntry *entry;
    int index;
    int row;

    if (dump != 0) {
        index = GAME_RESOURCE_COUNT - 1;
        if (GameResource[index].address == 0) {
            entry = &GameResource[index];
            do {
                entry--;
                index--;
                if (entry->address != 0)
                    break;
            } while (index != 0);
        }
        index -= 14;
        index = index < 0 ? 0 : index;
        for (row = 0; row < 16; row++) {
            xglFontDebugPrintf(64, 64 + row * 8, resourceDumpRowFormat, index, GameResource[index].address,
                               GameResource[index].size, GameResource[index].state,
                               (u32)GameResource[index].handle);
            index++;
        }
    }
}

/*
 * Frees slot and every slot after it, merging their space into slot, and clears the
 * enemy sound banks.
 */
void GameResourceReset(int slot)
{
    int total;
    int index;

    total = 0;
    index = slot;
    if (index < GAME_RESOURCE_COUNT) {
        for (; index < GAME_RESOURCE_COUNT; index++)
            total += GameResource[index].size;
    }
    GameResource[slot].size = total;
    GameResource[slot].state = GAME_RESOURCE_FREE;
    GameResource[slot].handle = 0;
    for (index = slot + 1; index < GAME_RESOURCE_COUNT; index++) {
        GameResource[index].address = 0;
        GameResource[index].size = 0;
        GameResource[index].handle = 0;
        GameResource[index].state = GAME_RESOURCE_FREE;
    }
    index = 0;
    do {
        EnemySeBank[index].id = 0;
        EnemySeBank[index].type = 0;
        EnemySeBank[index].name[0] = 0;
        xglSoundSendEffect(0, 0, index + SOUND_BANK_ENEMY_BASE);
        index++;
    } while (index < ENEMY_SE_BANK_COUNT);
    GameResourceDump(0);
    arcfilepreload = 0;
}

/*
 * Sets up the resource table over address and size, then clears the first word of
 * each fixed-address resource buffer in EE RAM.
 */
void GameResourceInit(unsigned char *address, int size)
{
    u32 *resource_buffer;
    int remaining = GAME_RESOURCE_COUNT - 1;
    int free_state = GAME_RESOURCE_FREE;
    GameResourceEntry *entry = GameResource;

    do {
        remaining--;
        entry->address = 0;
        entry->size = 0;
        entry->handle = 0;
        entry->state = free_state;
        entry++;
    } while (remaining >= 0);

    GameResource[0].address = address;
    GameResource[0].size = size;
    GameResourceReset(0);
    /* Reset the first word of each fixed-address resource buffer in EE RAM.
     * These addresses lie beyond the ELF's allocated data sections. */
    resource_buffer = (void *)0x01000000;
    resource_buffer[0] = 0;
    resource_buffer = (void *)0x01070800;
    resource_buffer[0] = 0;
    GameResourceReadFileCallback = default_callback;
    resource_buffer = (void *)0x010B1000;
    resource_buffer[0] = 0;
    resource_buffer = (void *)0x010B4000;
    resource_buffer[0] = 0;
    resource_buffer = (void *)0x010E6000;
    resource_buffer[0] = 0;
    resource_buffer = (void *)0x010E7000;
    resource_buffer[0] = 0;
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const char soundSwdPathFormat[24] = "data\\sound\\sed\\%s.SWD";

const char soundSedPathFormat[24] = "data\\sound\\sed\\%s.SED";

const char soundSmdPathFormat[24] = "data\\sound\\smd\\%s.SMD";

const char playerLockCommandName[16] = "player_lock";

const char eventBattleCommandName[16] = "event_btl";

const char mpeg2NoFadeCommandName[16] = "mpeg2_nofade";

const char mpeg2BattleCommandName[16] = "mpeg2_btl";

const char loadEffectCommandName[16] = "loadeffect";

const char loadMovieCommandName[16] = "loadmovie";

const char loadFaceCommandName[16] = "loadface";

const char motionFpkPathFormat[32] = "data\\motion\\%s.fpk";

const char resourceDumpRowFormat[24] = "%2d:%8x,%9d,%2d,%4x";

static GameResourceCommand command[24] = {
    { ResResetName, command_reset, 0 },
    { ResLoadmapName, command_loadmap, 0 },
    { ResLoadactName, (char *(*)(int, char *))command_loadact, 0 },
    { loadFaceCommandName, (char *(*)(int, char *))command_loadface, 0 },
    { ResLoadmotName, command_loadmot, 0 },
    { ResLoadtexName, (char *(*)(int, char *))command_loadtex, 0 },
    { loadMovieCommandName, (char *(*)(int, char *))command_loadmovie, 0 },
    { loadEffectCommandName, (char *(*)(int, char *))command_loadeffect, 0 },
    { ResLoadeneName, (char *(*)(int, char *))command_loadene, 0 },
    { ResScriptName, (char *(*)(int, char *))command_script, 0 },
    { ResPlayerName, (char *(*)(int, char *))command_player, 0 },
    { ResDummyName, (char *(*)(int, char *))command_dummy, 0 },
    { ResLoadseName, (char *(*)(int, char *))command_loadse, 0 },
    { ResLoadsmdName, (char *(*)(int, char *))command_loadsmd, 0 },
    { ResMpeg2Name, command_mpeg2, 0 },
    { mpeg2BattleCommandName, (char *(*)(int, char *))command_mpeg2battle, 0 },
    { mpeg2NoFadeCommandName, command_mpeg2nofade, 0 },
    { ResEneseName, (char *(*)(int, char *))command_enese, 0 },
    { ResNpcseName, (char *(*)(int, char *))command_npcse, 0 },
    { eventBattleCommandName, (char *(*)(int, char *))command_event2battle, 0 },
    { ResDiskName, (char *(*)(int, char *))command_disk, 0 },
    { ResSaveName, (char *(*)(int, char *))command_save, 0 },
    { playerLockCommandName, (char *(*)(int, char *))command_player_lock, 0 },
    { 0, 0, 0 }
};
