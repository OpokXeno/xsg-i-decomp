#include "common.h"
#include "main/xgl_mc.h"
#include "main/maptbl.h"
#include "main/xgl_font.h"
#include "main/xgl_sound.h"

#include "shared.h"

static u8 arcno;

typedef struct {
    const char *name;
    u16 ids[14];
} MotionEntry;

extern MotionEntry cfMtnList[31];

typedef struct {
    u16 *table;
    u16 count;
    u16 group;
} FaceEntry;

extern FaceEntry faceTbl[7];

static char *command_enenpcse(int command, char *cursor, int mode);

extern int GameDiskChange(int disk_number);

extern void Intermission(int save_number);

extern int xglCdGetFileSize(const char *name);

extern void SCRIPT_fade(int time);

static char *command_mpeg2_core(int command, char *cursor, int mode);

static int next_arc_size(void *address);

extern void Enemy_LoadPreset(void *address, const char *name);

extern void *AdrsEnemyPreset;

#include "main/xgl_thread.h"

typedef struct {
    u16 id;
    u8 type;
    u8 foot;
    char name[16];
} EnemySeEntry;

static u32 *arcfileaddr;

static int arcfilepreload;

static EnemySeEntry EnemySeBank[8];

static char foot_name[16];

static char env_name[0x10];

static char scene_txt_buffer[0x800];

static u32 archeader[0x40];

static u8 GameResourceWorkReloadTable[0x80];

u8 ScenePath = 0;

extern const char ResDataScenePath[];

extern const char ResDataSceneTestPath[];

extern const char ResDataSceneCheckPath[];

extern const char ResDataSceneCfPath[];

extern const char ResDataSceneEventPath[];

extern const char ResDataSceneGashPath[];

extern const char ResDataSceneFujiPath[];

extern const char ResDataSceneKojimaPath[];

extern const char ResDataSceneSakisakoPath[];

extern const char ResDataSceneKonishiPath[];

extern const char ResDataSceneKojiPath[];

extern const char ResDataSceneNvYonePath[];

extern const char ResDataSceneNakaharaPath[];

extern const char ResDataSceneYajimaPath[];

extern const char ResDataSceneKuramotoPath[];

extern const char ResDataSceneKeiichiPath[];

extern const char ResDataSceneSatoPath[];

extern const char ResDataSceneSugisawaPath[];

extern const char ResDataSceneYagiPath[];

extern const char ResDataSceneItaiPath[];

/* search_key walks this object in 12-byte records: it compares the first
 * string with the requested key and walks the string-pointer list at +4.
 * The third pointer is present in every record but is not read by the
 * recovered caller, so its role remains intentionally unnamed. */

typedef struct ResourceKeyEntry {
    const char *key;
    const void *aliases;
    const char *path_prefix;
} ResourceKeyEntry;

extern const char ResCidName[];

extern const char ResCharPath[];

extern const char ResRidName[];

extern const char ResRoboPath[];

extern const char ResEnemyPath[];

extern const char ResWidName[];

extern const char ResWeaponPath[];

extern const char ResEidName[];

extern const char ResObjPath[];

extern const char ResAidName[];

extern const char ResMidName[];

extern const char ResEmptyName[];

extern const char *AID0[];
extern const char *CID0[];
extern const char *CID1[];
extern const char *CID2[];
extern const char *CID3[];
extern const char *CID4[];
extern const char *CID5[];
extern const char *CID6[];
extern const char ResBossBossPath[];
extern const char ResFereFerePath[];
extern const char ResFesoFesoPath[];
extern const char ResFeroFeroPath[];
extern const char ResFemaFemaPath[];
extern const char ResUtroUtroPath[];
extern const char ResUtsoUtsoPath[];
extern const char ResUtreUtrePath[];
extern const char ResUtmaUtmaPath[];
extern const char ResGunoGunoPath[];
extern const char ResMobj000MobjPath[];
extern const char ResAobj000AccPath[];
extern const char ResMusobjMusPath[];
extern const char ResMufobjMufPath[];
extern const char ResMuxobjMuxPath[];
extern const char ResTreobjTrePath[];
extern const char ResMuwobjMuwPath[];
extern const char ResMuobjMUPath[];
extern const char ResCitiPath[];
extern const char ResPplPath[];
extern const char ResNpcPath[];
extern const char ResPcPath[];
extern const char ResEtcPath[];
extern const char ResAgAgPath[];
extern const char ResAgName[];
extern const char ResVxVxPath[];
extern const char ResVxName[];
extern const char ResPcChoPath[];
extern const char ResChoName[];
extern const char ResPcSonPath[];
extern const char ResSonName[];
extern const char ResPcStnPath[];
extern const char ResStnName[];
extern const char ResPcJrPath[];
extern const char ResJrPrefixName[];
extern const char ResPcZigPath[];
extern const char ResZigName[];
extern const char ResPcMmoPath[];
extern const char ResMmoName[];
extern const char ResPcKosPath[];
extern const char ResKosName[];
extern const char ResBossName[];
extern const char ResSeparatorName[];
extern const char ResFereName[];
extern const char ResFesoName[];
extern const char ResFeroName[];
extern const char ResFemaName[];
extern const char ResUtroName[];
extern const char ResUtsoName[];
extern const char ResUtreName[];
extern const char ResUtmaName[];
extern const char ResGunoName[];
extern const char ResMusName[];
extern const char ResMufName[];
extern const char ResMuxName[];
extern const char ResTreName[];
extern const char ResMuwName[];
extern const char ResMuName[];
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
    {ResCidName, CID, ResCharPath},
    {ResCidName, CID, ResCharPath},
    {ResRidName, RID, ResRoboPath},
    {ResWidName, WID, ResWeaponPath},
    {ResEidName, EID, ResEnemyPath},
    {ResMidName, MID, ResObjPath},
    {ResAidName, AID, ResObjPath},
    {ResEmptyName, MOBJ, ResObjPath},
    {0, 0, 0},
};

static const char *tbl_0_00366420[20] = {
    ResDataScenePath, ResDataSceneTestPath, ResDataSceneCheckPath, ResDataSceneCfPath, ResDataSceneEventPath,
    ResDataSceneGashPath, ResDataSceneFujiPath, ResDataSceneKojimaPath, ResDataSceneSakisakoPath, ResDataSceneKonishiPath,
    ResDataSceneKojiPath, ResDataSceneNvYonePath, ResDataSceneNakaharaPath, ResDataSceneYajimaPath, ResDataSceneKuramotoPath,
    ResDataSceneKeiichiPath, ResDataSceneSatoPath, ResDataSceneSugisawaPath, ResDataSceneYagiPath, ResDataSceneItaiPath
};

/*
 * The runtime resource-block table: up to 128 blocks carved out of the
 * arc-file load heap GameResourceInit sizes. address/size describe the
 * block's extent; handle is a random token GameResourceAlloc
 * (main:0x0024a9d8) assigns with xglSRand() when it hands a block out, and
 * both it and GameResourceReset (main:0x0024d8e8) clear it back to 0; state
 * is -1 for a free block (this TU's readers) and 9 once GameResourceAlloc
 * marks one in use (original bytes; neither function is claimed here).
 */

typedef struct {
    unsigned char *address;   /* +0x0 */
    int size;       /* +0x4 */
    int handle;     /* +0x8 */
    int state;      /* +0xC, -1 = free */
} GameResourceEntry;

GameResourceEntry GameResource[128] = {0};

/*
 * The game loop's own record, as far as this TU reads it. Two spans are
 * evidenced: the runtime flags word command_player_lock ORs a bit into
 * (lw/ori/sw +0x10, main:0x0024c188) and the scene id GameResourceWorkReload
 * reloads by (lw +0x18, main:0x0024a998). Nothing else is modelled; the
 * object is still owned by the generated .sdata scaffolding.
 */

typedef struct {
    u8 unmodeled_00[0x10];
    u32 flags;      /* +0x10 */
    u8 unmodeled_14[4];
    int scene_id;   /* +0x18 */
    u8 unmodeled_1c[0x34];
    s16 map_id;
    u16 unmodeled_52;
    int map_load_result;
} GameLoopStatePrefix;

extern GameLoopStatePrefix GameLoopState;

extern void ACT_init(void);

extern void MapChangeFadeSet(void);

void GameResourceLoad(int scene_id);

int RES_loadFile(int command, int callback, int resource_id, int flags);

static char *search_key(char *cursor, u8 *table, int *id);

/*
 * The engine's actor record: main/tu194 (src/main/enemy_2.h) defines and
 * owns it. This TU only reads the resource pointer past that definition's
 * extent, so it names the same tag without completing it (no local header
 * can include another TU's TU-local one) rather than modelling a
 * competing layout.
 */

typedef struct ActorResource {
    unsigned char unmodeled_000[6];
    u8 foot_step_no; /* +6 */
} ActorResource;

#define ACTOR_RESOURCE_OFFSET 0x8dc
struct Actor {
    unsigned char unmodeled_000[ACTOR_RESOURCE_OFFSET];
    struct ActorResource *resource;
};

struct Actor;

/*
 * The resource record an actor's `resource` pointer refers to.
 * RES_GetFootStepNo is the only reader in this TU, and it reads only the
 * footstep number byte at +6; the bytes before it are an unread span here.
 */

/*
 * +0x8dc: the actor's resource pointer, past the extent src/main/enemy_2.h
 * completes; RES_GetFootStepNo is the only reader of it in this TU.
 */

extern int sefLoadEffectCfName(int cf_id, char *effect_name);

extern int sefLoadMemoryEffectCfName(void *data, char *effect_name, int size);

/* RES_loadFile callback: (resource type, file name, resource address). */

extern void GameCfPlayerLoadResource(int resource_set);

extern void SCRIPT_frameLock2Battle(void);

extern const char ResShitanName[];

extern const char ResKosmosHName[];

extern const char ResJr1HName[];

extern const char ResMomo6Name[];

extern const char ResShion6HName[];

extern const char ResShion1Shion5Name[];

extern const char ResMomo2Name[];

extern const char ResChaosHName[];

extern const char ResKosmosH4Name[];

extern const char ResJr3Name[];

extern const char ResKosmos3Name[];

extern const char ResChaos2Name[];

extern const char ResJr4Name[];

extern const char ResMomo4Name[];

extern const char ResShionName[];

extern const char ResKosmosName[];

extern const char ResMomo3Name[];

extern const char ResJr1Name[];

extern const char ResShionChName[];

extern const char ResShitan2Name[];

extern const char ResShion2HName[];

extern const char ResShion3HName[];

extern const char ResShitan1Name[];

extern const char ResKosmosH2Name[];

extern const char ResJr2Name[];

extern const char ResShion2Name[];

extern const char ResKosmosH3Name[];

extern const char ResKosmosH6Name[];

extern const char ResMomo5Name[];

extern const char ResJrHName[];

extern const char ResZiggy1Name[];

extern const char ResShion4HShionHName[];

extern const char ResShion5Shion1Name[];

extern const char ResKosmos2Name[];

extern const char ResChaosName[];

extern const char ResChaos1Name[];

extern const char ResMomo5HName[];

extern const char ResShion1HShion5HName[];

extern const char ResShionShion4Name[];

extern const char ResKosmosH5Name[];

extern const char ResKosmos5Name[];

extern const char ResJrName[];

extern const char ResShionHShion4HName[];

extern const char ResShion5HShion1HName[];

extern const char ResMomo1Name[];

extern const char ResMomoHName[];

extern const char ResKosmosH1Name[];

extern const char ResShitanHName[];

extern const char ResZiggyHName[];

extern const char ResZiggyName[];

extern const char ResKosmos1Name[];

extern const char ResMomoName[];

extern const char ResShion3Name[];

extern const char ResMiyukiName[];

extern const char ResEllyName[];

extern const char ResTonnyName[];

extern const char ResLapisName[];

extern const char ResAlbeltHName[];

extern const char ResMarglis1Name[];

extern const char ResVirgil3Name[];

extern const char ResJoachimName[];

extern const char ResVirgil1Name[];

extern const char ResGaignun1Name[];

extern const char ResKebin1Name[];

extern const char ResVirgilName[];

extern const char ResAndrew2Name[];

extern const char ResAndrew7Name[];

extern const char ResMatehwsHMatehwsName[];

extern const char ResHammerHHammerName[];

extern const char ResKebin1HName[];

extern const char ResVirgil2HName[];

extern const char ResAndDaugName[];

extern const char ResGaignunHName[];

extern const char ResAndWifeName[];

extern const char ResAndrew8Name[];

extern const char ResAndrewHAndrew6Name[];

extern const char ResHammerName[];

extern const char ResAbelName[];

extern const char ResChiefName[];

extern const char ResAndrew4Name[];

extern const char ResAndrewHAndrew1Name[];

extern const char ResAndrewHAndrew3Name[];

extern const char ResKJoachName[];

extern const char ResShalronName[];

extern const char ResVirgil4HName[];

extern const char ResShelleyName[];

extern const char ResVirgil3HName[];

extern const char ResJoachim1Name[];

extern const char ResVoyagerName[];

extern const char ResMarglisName[];

extern const char ResAllenHName[];

extern const char ResAlbelt2HName[];

extern const char ResKebinName[];

extern const char ResAndrewName[];

extern const char ResShiDadName[];

extern const char ResAndrew6Name[];

extern const char ResElly2Name[];

extern const char ResFeb1Name[];

extern const char ResTonnyHTonnyName[];

extern const char ResHynlinName[];

extern const char ResAndrewHAndrewName[];

extern const char ResTogashiName[];

extern const char ResAndrewHAndrew4Name[];

extern const char ResAndrew5Name[];

extern const char ResAlbelt3HName[];

extern const char ResCecilia1Name[];

extern const char ResSellersName[];

extern const char ResPelegriName[];

extern const char ResFriedName[];

extern const char ResAlbeltName[];

extern const char ResYuriName[];

extern const char ResVirgil1HName[];

extern const char ResCeciliaName[];

extern const char ResStepMamName[];

extern const char ResAndrewHAndrew2Name[];

extern const char ResAndrewHAndrew5Name[];

extern const char ResAndrew3Name[];

extern const char ResVirgil2Name[];

extern const char ResShiMamName[];

extern const char ResMarglisHName[];

extern const char ResFriedHName[];

extern const char ResEllyHName[];

extern const char ResGaignunName[];

extern const char ResCathName[];

extern const char ResVanName[];

extern const char ResVirgil4Name[];

extern const char ResKebin2Name[];

extern const char ResCath1Name[];

extern const char ResVirgilHName[];

extern const char ResMatehwsName[];

extern const char ResMaryName[];

extern const char ResMoriyamaName[];

extern const char ResMatehws1Name[];

extern const char ResAndrew1Name[];

extern const char ResAllen1Name[];

extern const char ResAndrew9Name[];

extern const char ResHelmerName[];

extern const char ResAlbelt1Name[];

extern const char ResKebinHName[];

extern const char ResFebName[];

extern const char ResAllenName[];

extern const char ResEngName[];

extern const char ResBrealW1Name[];

extern const char ResVecMName[];

extern const char ResPilotName[];

extern const char ResRwW1Name[];

extern const char ResVecM2Name[];

extern const char ResBrealM4Name[];

extern const char ResOffM3Name[];

extern const char ResEsoldName[];

extern const char ResBrealM1Name[];

extern const char ResSuukiName[];

extern const char ResRwWName[];

extern const char ResEngOffName[];

extern const char ResBrealMName[];

extern const char ResOffWName[];

extern const char ResBrealWName[];

extern const char ResOffW1Name[];

extern const char ResRwM1Name[];

extern const char ResRwMName[];

extern const char ResSpaceName[];

extern const char ResOffMName[];

extern const char ResOffM1Name[];

extern const char ResBrealM3Name[];

extern const char ResVecW1Name[];

extern const char ResRwW2Name[];

extern const char ResOffM2Name[];

extern const char ResRwM2Name[];

extern const char ResVecWName[];

extern const char ResBrealM2Name[];

extern const char ResSoldName[];

extern const char ResVecM1Name[];

extern const char ResCommiName[];

extern const char ResCommi1Name[];

extern const char ResGoveW2Name[];

extern const char ResRealM2Name[];

extern const char ResRealM1Name[];

extern const char ResCommi4Name[];

extern const char ResRealWName[];

extern const char ResUSoldsName[];

extern const char ResCommi3Name[];

extern const char ResGoveWName[];

extern const char ResCommi5Name[];

extern const char ResRealW2Name[];

extern const char ResGoveW1Name[];

extern const char ResCommi2Name[];

extern const char ResRealMName[];

extern const char ResUSoldName[];

extern const char ResRealW1Name[];

extern const char ResUSpaceName[];

extern const char ResAssWName[];

extern const char ResHyakuName[];

extern const char ResDuName[];

extern const char ResAssM4Name[];

extern const char ResAssM3Name[];

extern const char ResAssMName[];

extern const char ResAssM1Name[];

extern const char ResDu1Name[];

extern const char ResGoveM2Name[];

extern const char ResKanchoName[];

extern const char ResAssM2Name[];

extern const char ResGoveMName[];

extern const char ResDu2Name[];

extern const char ResGoveM1Name[];

extern const char ResUticW1Name[];

extern const char ResUticM2Name[];

extern const char ResUticWName[];

extern const char ResUticW2Name[];

extern const char ResContName[];

extern const char ResPolWName[];

extern const char ResUticM1Name[];

extern const char ResPolMName[];

extern const char ResUticMName[];

extern const char ResGirlAName[];

extern const char ResGoroName[];

extern const char ResKingName[];

extern const char ResBabaB1Name[];

extern const char ResRobotName[];

extern const char ResManA2Name[];

extern const char ResEngOjiName[];

extern const char ResManB2Name[];

extern const char ResGirlA1Name[];

extern const char ResInnGirlName[];

extern const char ResKidswA2Name[];

extern const char ResJijiA2Name[];

extern const char ResWmanAName[];

extern const char ResScotName[];

extern const char ResKidswBName[];

extern const char ResKidswA1Name[];

extern const char ResObaB1Name[];

extern const char ResWmanA2Name[];

extern const char ResMouthName[];

extern const char ResBoyAName[];

extern const char ResInnOjiName[];

extern const char ResManB1Name[];

extern const char ResObaA1Name[];

extern const char ResBabaA1Name[];

extern const char ResGirlB2Name[];

extern const char ResDoctorName[];

extern const char ResKidsmAName[];

extern const char ResKidswAName[];

extern const char ResBartenName[];

extern const char ResWmanB2Name[];

extern const char ResSpyName[];

extern const char ResBabaBName[];

extern const char ResBoyB2Name[];

extern const char ResBreadName[];

extern const char ResKidsmBName[];

extern const char ResBarOjiName[];

extern const char ResNurseName[];

extern const char ResOjiAName[];

extern const char ResGirlBName[];

extern const char ResJijiAName[];

extern const char ResKidsmB1Name[];

extern const char ResManBName[];

extern const char ResBabaB2Name[];

extern const char ResCleChName[];

extern const char ResBoyBName[];

extern const char ResBabaA2Name[];

extern const char ResJijiB2Name[];

extern const char ResObaAName[];

extern const char ResBreadChName[];

extern const char ResManA1Name[];

extern const char ResObaB2Name[];

extern const char ResObaBName[];

extern const char ResBoyB1Name[];

extern const char ResPilotNoName[];

extern const char ResWmanB1Name[];

extern const char ResWmanA1Name[];

extern const char ResKidswB1Name[];

extern const char ResBarMName[];

extern const char ResJijiB1Name[];

extern const char ResGirlB1Name[];

extern const char ResManAName[];

extern const char ResOjiA1Name[];

extern const char ResWmanBName[];

extern const char ResKingCatName[];

extern const char ResKidsmB2Name[];

extern const char ResOjiA2Name[];

extern const char ResKidswB2Name[];

extern const char ResBoyA2Name[];

extern const char ResKidsmA1Name[];

extern const char ResCleObaName[];

extern const char ResGirlA2Name[];

extern const char ResBabaAName[];

extern const char ResOjiB1Name[];

extern const char ResJijiBName[];

extern const char ResJijiA1Name[];

extern const char ResKidsmA2Name[];

extern const char ResBoyA1Name[];

extern const char ResObaA2Name[];

extern const char ResOjiB2Name[];

extern const char ResOjiBName[];

extern const char ResFile05Name[];

extern const char ResFile01Name[];

extern const char ResFile04Name[];

extern const char ResFile06Name[];

extern const char ResFile03Name[];

extern const char ResFile02Name[];

extern const char ResSuffix06Name[];

extern const char ResSuffix05Name[];

extern const char ResFile01CvName[];

extern const char ResSuffix04Name[];

extern const char ResFile01HoName[];

extern const char ResSuffix02Name[];

extern const char ResFile01Ho2Name[];

extern const char ResSuffix01Name[];

extern const char ResFile08Name[];

extern const char ResSuffix03Name[];

extern const char ResFile07Name[];

extern const char ResFile09Name[];

extern const char ResFile02HoName[];

extern const char ResVx1MName[];

extern const char ResUkunSName[];

extern const char ResUkunBName[];

extern const char ResUknGName[];

extern const char ResGrHeadName[];

extern const char ResGrHandName[];

extern const char ResGrBodyName[];

extern const char ResGrFinalName[];

extern const char ResGrLegName[];

extern const char ResTs01Name[];

extern const char ResHw05Name[];

extern const char ResBw01Name[];

extern const char ResBw02Name[];

extern const char ResBw03Name[];

extern const char ResHw04Name[];

extern const char ResHw02Name[];

extern const char ResHw03Name[];

extern const char ResHw06Name[];

extern const char ResHw01Name[];

extern const char ResBw04Name[];

extern const char ResRd02Name[];

extern const char ResRd16Name[];

extern const char ResRd17Name[];

extern const char ResRd06Name[];

extern const char ResRd09Name[];

extern const char ResRd14Name[];

extern const char ResRd15Name[];

extern const char ResRd01Name[];

extern const char ResRd12Name[];

extern const char ResRd07Name[];

extern const char ResRd08Name[];

extern const char ResRd05Name[];

extern const char ResRd10Name[];

extern const char ResRd13Name[];

extern const char ResRd04Name[];

extern const char ResRd11Name[];

extern const char ResRd03Name[];

extern const char ResSw01Name[];

extern const char ResGun05Name[];

extern const char ResJrHg07Name[];

extern const char ResJrHg06Name[];

extern const char ResGun01Name[];

extern const char ResJrHg08Name[];

extern const char ResGun03Name[];

extern const char ResGun04Name[];

extern const char ResGun02Name[];

extern const char ResSw05Name[];

extern const char ResSw04Name[];

extern const char ResSw02Name[];

extern const char ResSw03Name[];

extern const char ResMboxName[];

extern const char ResHb00RName[];

extern const char ResMg01Name[];

extern const char ResPbk00Name[];

extern const char ResDlc00Name[];

extern const char ResMsp01Name[];

extern const char ResDeff00Name[];

extern const char ResFl00Name[];

extern const char ResSmp01Name[];

extern const char ResRf02Name[];

extern const char ResEcm01Name[];

extern const char ResFsd00Name[];

extern const char ResMsp03Name[];

extern const char ResDrl00Name[];

extern const char ResHg01Name[];

extern const char ResCb00Name[];

extern const char ResLsr00Name[];

extern const char ResRf00Name[];

extern const char ResWire00Name[];

extern const char ResLgc00Name[];

extern const char ResBam00Name[];

extern const char ResBl00Name[];

extern const char ResShb00Name[];

extern const char ResEcm02Name[];

extern const char ResMg00Name[];

extern const char ResWct00Name[];

extern const char ResMsp00Name[];

extern const char ResGlg00Name[];

extern const char ResBbc00Name[];

extern const char ResBw00Name[];

extern const char ResBd00Name[];

extern const char ResGnd00Name[];

extern const char ResBp00Name[];

extern const char ResAx00Name[];

extern const char ResGlg00RName[];

extern const char ResHmr02Name[];

extern const char ResMsp02RName[];

extern const char ResBs00Name[];

extern const char ResEad00Name[];

extern const char ResRf01Name[];

extern const char ResSol00Name[];

extern const char ResEac00Name[];

extern const char ResHmr00Name[];

extern const char ResBd01Name[];

extern const char ResVsd00Name[];

extern const char ResHb00Name[];

extern const char ResAmp02Name[];

extern const char ResMsp02Name[];

extern const char ResDrl01Name[];

extern const char ResHg00Name[];

extern const char ResAcc030Name[];

extern const char ResAcc031Name[];

extern const char ResAcc028Name[];

extern const char ResAcc033Name[];

extern const char ResAcc029Name[];

extern const char ResAcc018Name[];

extern const char ResAcc036Name[];

extern const char ResEwpHg01Name[];

extern const char ResAcc032Name[];

extern const char ResAcc034Name[];

extern const char ResAcc035Name[];

extern const char ResWingName[];

extern const char ResFile013Name[];

extern const char ResFile007Name[];

extern const char ResFile324Name[];

extern const char ResFile325Name[];

extern const char ResFile003Name[];

extern const char ResFile006Name[];

extern const char ResFile114Name[];

extern const char ResFile311Name[];

extern const char ResFile310Name[];

extern const char ResFile015Name[];

extern const char ResFile025Name[];

extern const char ResFile320Name[];

extern const char ResFile018Name[];

extern const char ResFile318Name[];

extern const char ResFile313Name[];

extern const char ResFile016Name[];

extern const char ResFile004Name[];

extern const char ResFile014Name[];

extern const char ResFile307Name[];

extern const char ResFile020Name[];

extern const char ResFile312Name[];

extern const char ResFile009Name[];

extern const char ResFile002Name[];

extern const char ResFile317Name[];

extern const char ResFile023Name[];

extern const char ResFile309Name[];

extern const char ResFile323Name[];

extern const char ResFile021Name[];

extern const char ResFile022Name[];

extern const char ResFile305Name[];

extern const char ResFile001Name[];

extern const char ResFile017Name[];

extern const char ResFile302Name[];

extern const char ResFile308Name[];

extern const char ResFile319Name[];

extern const char ResFile303Name[];

extern const char ResFile010Name[];

extern const char ResFile019Name[];

extern const char ResFile024Name[];

extern const char ResFile304Name[];

extern const char ResFile315Name[];

extern const char ResFile008Name[];

extern const char ResFile005Name[];

extern const char ResFile314Name[];

extern const char ResFile301Name[];

extern const char ResFile316Name[];

extern const char ResFile011Name[];

extern const char ResFile012Name[];

extern const char ResFile321Name[];

extern const char ResFile306Name[];

extern const char ResFile322Name[];

extern const char ResFile028Name[];

extern const char ResFile030Name[];

extern const char ResFile026Name[];

extern const char ResFile027Name[];

extern const char ResFile029Name[];

extern const char ResFile032Name[];

extern const char ResFile033Name[];

extern const char ResFile031Name[];

extern const char ResFile115Name[];

extern const char ResFile078Name[];

extern const char ResFile103Name[];

extern const char ResFile126Name[];

extern const char ResFile081Name[];

extern const char ResFile057Name[];

extern const char ResFile169Name[];

extern const char ResFile129Name[];

extern const char ResFile138Name[];

extern const char ResFile067Name[];

extern const char ResFile166Name[];

extern const char ResFile054Name[];

extern const char ResFile069Name[];

extern const char ResFile037Name[];

extern const char ResFile084Name[];

extern const char ResFile086Name[];

extern const char ResFile087Name[];

extern const char ResFile090Name[];

extern const char ResFile093Name[];

extern const char ResFile042Name[];

extern const char ResFile074Name[];

extern const char ResFile130Name[];

extern const char ResFile132Name[];

extern const char ResFile134Name[];

extern const char ResFile146Name[];

extern const char ResFile113Name[];

extern const char ResFile122Name[];

extern const char ResFile176Name[];

extern const char ResFile075Name[];

extern const char ResFile131Name[];

extern const char ResFile068Name[];

extern const char ResFile150Name[];

extern const char ResFile178Name[];

extern const char ResFile180Name[];

extern const char ResFile041Name[];

extern const char ResFile140Name[];

extern const char ResFile047Name[];

extern const char ResFile170Name[];

extern const char ResFile076Name[];

extern const char ResFile096Name[];

extern const char ResFile123Name[];

extern const char ResFile097Name[];

extern const char ResFile108Name[];

extern const char ResFile082Name[];

extern const char ResFile152Name[];

extern const char ResFile110Name[];

extern const char ResFile046Name[];

extern const char ResFile145Name[];

extern const char ResFile040Name[];

extern const char ResFile177Name[];

extern const char ResFile073Name[];

extern const char ResFile092Name[];

extern const char ResFile160Name[];

extern const char ResFile168Name[];

extern const char ResFile055Name[];

extern const char ResFile098Name[];

extern const char ResFile116Name[];

extern const char ResFile064Name[];

extern const char ResFile060Name[];

extern const char ResFile000Name[];

extern const char ResFile089Name[];

extern const char ResFile142Name[];

extern const char ResFile107Name[];

extern const char ResFile039Name[];

extern const char ResFile091Name[];

extern const char ResFile048Name[];

extern const char ResFile154Name[];

extern const char ResFile155Name[];

extern const char ResFile175Name[];

extern const char ResFile133Name[];

extern const char ResFile105Name[];

extern const char ResFile162Name[];

extern const char ResFile179Name[];

extern const char ResFile164Name[];

extern const char ResFile172Name[];

extern const char ResFile034Name[];

extern const char ResFile165Name[];

extern const char ResFile088Name[];

extern const char ResFile125Name[];

extern const char ResFile156Name[];

extern const char ResFile049Name[];

extern const char ResFile062Name[];

extern const char ResFile080Name[];

extern const char ResFile063Name[];

extern const char ResFile151Name[];

extern const char ResFile167Name[];

extern const char ResFile065Name[];

extern const char ResFile100Name[];

extern const char ResFile061Name[];

extern const char ResFile139Name[];

extern const char ResFile094Name[];

extern const char ResFile038Name[];

extern const char ResFile102Name[];

extern const char ResFile059Name[];

extern const char ResFile056Name[];

extern const char ResFile117Name[];

extern const char ResFile149Name[];

extern const char ResFile174Name[];

extern const char ResFile072Name[];

extern const char ResFile161Name[];

extern const char ResFile109Name[];

extern const char ResFile135Name[];

extern const char ResFile159Name[];

extern const char ResFile052Name[];

extern const char ResFile111Name[];

extern const char ResFile085Name[];

extern const char ResFile153Name[];

extern const char ResFile051Name[];

extern const char ResFile118Name[];

extern const char ResFile106Name[];

extern const char ResFile035Name[];

extern const char ResFile136Name[];

extern const char ResFile143Name[];

extern const char ResFile173Name[];

extern const char ResFile058Name[];

extern const char ResFile158Name[];

extern const char ResFile157Name[];

extern const char ResFile043Name[];

extern const char ResFile120Name[];

extern const char ResFile121Name[];

extern const char ResFile045Name[];

extern const char ResFile036Name[];

extern const char ResFile079Name[];

extern const char ResFile124Name[];

extern const char ResFile053Name[];

extern const char ResFile066Name[];

extern const char ResFile147Name[];

extern const char ResFile101Name[];

extern const char ResFile099Name[];

extern const char ResFile071Name[];

extern const char ResFile137Name[];

extern const char ResFile144Name[];

extern const char ResFile163Name[];

extern const char ResFile050Name[];

extern const char ResFile077Name[];

extern const char ResFile104Name[];

extern const char ResFile127Name[];

extern const char ResFile044Name[];

extern const char ResFile112Name[];

extern const char ResFile070Name[];

extern const char ResFile128Name[];

extern const char ResFile141Name[];

extern const char ResFile083Name[];

extern const char ResFile095Name[];

extern const char ResFile119Name[];

extern const char ResFile171Name[];

extern const char ResFile148Name[];

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

void GameResourceReset(int slot);

extern void *GameResourceAlloc(int size);

void *GameResourceRealloc(void *address, int size);

static u8 GameResourceWorkReloadLeader;

void (*GameResourceReadFileCallback)(void) = 0;

extern void MapReset(void);

typedef struct {
    const char *name;
    char *(*callback)(int, char *);
    void *unmodeled_08;
} GameResourceCommand;

static GameResourceCommand command[24];

struct SaveDataLeaderSeIndex {
    u8 unmodeled_00[0x100A4];
    u16 leader_se_index;
};

static const char * tbl_1_00366470[7];

extern struct SoundWork SoundWork;

extern const char soundSwdPathFormat[];

extern const char soundSedPathFormat[];

extern const char soundSmdPathFormat[];

extern void xglSoundLoadEffect(const char *name, void *address, int bank);

extern int SsdGetResultValue(int *result);

extern int SsdGetSeqPlayStatus(int sequence_id);

extern int SsdCheckPlayEffectFileID(u16 file_id);

extern void xglSoundSendSmd(void *address);

extern void xglSoundSequenceNormal(int mode);

typedef struct {
    u8 unmodeled_00[14];
    u16 sequence_id;
} SoundLoadBuffer;

extern char *MAP_getPath(char *name);

static char dummy_3[15] ;

typedef struct {
    float (*matrix)[4];
    void *lex_file;
    void *texture_file;
    void *id_file;
    void *think_file;
    u8 unmodeled_14[12];
    float matrix_storage[4][4];
} SubMapLoadState;

extern void xglMatrixUnit(float matrix[4][4]);

extern char *MapGetName(int scene_id);

extern int func_A1CF18(char *path, int scene_id);

extern const u8 lexSuffix[];

extern const u8 xtxSuffix[];

extern const u8 idSuffix[];

extern const char faceSuffix[];

static const char root_4[6];

static const char dummy_5[6];

typedef struct {
    void *first_file;
    void *second_file;
    void *third_file;
    u8 unmodeled_0c[4];
} ResourceFileSet;

extern const char resourceDumpRowFormat[];

void RES_GetMdlFileName(char *destination, int selector);

extern const char jntSuffix[];

extern const char motionFpkPathFormat[];

static char base_6[13] ;

static char base_7[29] ;

static char base_8[24] ;

static const char base_9[6];

int GameResourceGetIndex(int address);

static int RES_GetMdlFileNameSub(char *dst, int selector);

extern char *strcpy(char *destination, const char *source);

int GameResourceSearch(int type, int key);

static int RES_loadFileSubMapSub(char *name, char *suffix, void *buffer);

static int RES_loadFileSub(int callback, int resource_id, void *address);

extern int xglSoundSendSmd2(void *address, int sequence_index);

extern void xglSoundSequenceNormal2(int sequence_index, int mode);

extern unsigned char ModelPath;

static void default_callback(void)
{
    xglSleep();
}

const char *RES_getPath(void)
{
    if (ScenePath >= 20)
        ScenePath = 0;
    return tbl_0_00366420[ScenePath];
}

static char *RES_getScenePath(char *path, int resource_id)
{
    const char *scene_path = RES_getPath();
    int scene_id;

    while ((*path = *scene_path) != 0) {
        path++;
        scene_path++;
    }

    if (resource_id <= 0x0fffffff) {
        path[0] = 'c';
        path[1] = 'f';
    } else {
        path[0] = 'e';
        path[1] = 'v';
    }

    scene_id = resource_id & 0x0fffffff;

    path[5] = (char)('0' + scene_id % 10);
    scene_id /= 10;
    path[4] = (char)('0' + scene_id % 10);
    scene_id /= 10;
    path[3] = (char)('0' + scene_id % 10);
    scene_id /= 10;
    path[2] = (char)('0' + scene_id % 10);
    path[6] = '.';
    path[7] = 't';
    path[8] = 'x';
    path[9] = 't';
    path[10] = 0;

    return path + 7;
}

int RES_GetEnemySeBank(int sound_id)
{
    int index = 0;
    int bank = 0x40000;

    for (; index < 8; index++) {
        if (EnemySeBank[index].id == sound_id)
            return bank;
        bank += 0x10000;
    }
    return -1;
}

int RES_GetEnemySeType(int sound_id)
{
    int index;

    for (index = 0; index < 8; index++) {
        if (EnemySeBank[index].id == sound_id)
            return EnemySeBank[index].type;
    }
    return -1;
}

int RES_GetEnemySeFoot(int sound_id)
{
    int index;

    for (index = 0; index < 8; index++) {
        if (EnemySeBank[index].id == sound_id)
            return EnemySeBank[index].foot;
    }
    return -1;
}

const char *RES_GetEnemySeName(int index)
{
    if (index < 8)
        return EnemySeBank[index].name;
    return 0;
}

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

static int resource_typeid_translate(int index, int *type, int *key)
{
    switch ((unsigned short)*type) {
    case 0:
        if ((*key & 0xFFFF0000) == 0x10000) {
            *key = convert_face(*key & 0xFFFF) + 0x10000;
            return 1;
        }
        return 0;
    case 2:
        if (*key <= 0x02FFFFFF) {
            *key = RES_GetMotBaseID(*key);
            return 1;
        }
        return 0;
    case 3:
        *type = 2;
        *key = index | 0x03000000;
        return 1;
    case 5:
        *key = index;
        return 1;
    default:
        return 0;
    }
}

int GameResourceSearch(int type, int key)
{
    int index;
    int resource_type;
    int resource_key;

    if (type == 5)
        return -1;
    index = 0;
    while (index < 0x80) {
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

    for (index = 0; index < 0x80; index++) {
        if (GameResource[index].state == -1)
            return index;
    }
    return -1;
}

u32 GameResourceGetFreeAddr(void)
{
    u32 free_address = GameResource[resource_get_free()].address;

    if (free_address <= 0x01129FFF)
        return 0x0112A000;
    return free_address;
}

u32 GameResourceWorkAlloc(int size)
{
    int index;
    GameResourceEntry *entry;

    GameResourceWorkReloadLeader = 0;
    entry = GameResource;
    GameResourceWorkReloadTable[0] = 0;
    index = 0;
    do {
        if (entry->state == -1) {
            if (entry->size > size)
                return entry->address;
            break;
        }
        index++;
        entry++;
    } while (index < 0x80);
    GameResourceWorkReloadTable[0] = 1;
    return 0x0112A000;
}

void GameResourceWorkReload(void)
{
    if (GameResourceWorkReloadTable[0] != 0) {
        ACT_init();
        GameResourceLoad(GameLoopState.scene_id);
        return;
    }
    MapChangeFadeSet();
}

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
    GameResource[index].state = 9;
    GameResource[index].handle = xglSRand();
    return GameResource[index].address;
}

void *GameResourceRealloc(void *address, int size)
{
    int index;
    int combined_size;
    u32 old_address;

    index = GameResourceGetIndex((int)address);
    if (index < 0)
        return 0;
    if (GameResource[index + 1].state == -1) {
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
        if (*(u32 *)source == 0x00585241)
            size = xglArxExtract((u32 *)address, (u32 *)source);
        else
            memcpy(address, source, size);
    }
    if ((u32)archeader[0] >> 2 <= (u32)arcno)
        arcfileaddr = 0;
    return size;
}

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

static char *search_key(char *cursor, u8 *table, int *id)
{
    ResourceKeyEntry *record = (ResourceKeyEntry *)table;
    int root_id = 0;

    while (record->key != 0) {
        const char *root_key = record->key;
        char *root_input = cursor;

        for (;;) {
            char expected = *root_key;
            char root_character = *root_input;

            if (expected == '\0') {
                ResourceKeyEntry *child = (ResourceKeyEntry *)record->aliases;
                int child_id = root_id;

                if (child->key != 0) {
                    do {
                        const char *child_key = child->key;
                        char *child_input = root_input;

                        for (;;) {
                            char child_expected = *child_key;
                            char child_character = *child_input;
                            if (child_expected == '\0') {
                                const char **leaf = (const char **)child->aliases;
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

static char *command_loadmap(int command, char *cursor)
{
    MapNameGroup *group;
    int group_id;
    int name_index;
    int load_result;
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
        group_id += 100;
    }

    if (group_id >= 0x71e)
        group_id = 0;

map_name_found:
    load_result = RES_loadFile(command, 1, group_id, 0);
    if (no_load == 0) {
        GameLoopState.map_id = (s16)group_id;
        GameLoopState.map_load_result = load_result;
        MapChangeFadeSet();
        MapReset();
    }
    return cursor;
}

static char *command_loadact(int command, char *cursor, int mode)
{
    int resource_id = 0;
    char *next_cursor = search_key(cursor, (u8 *)model, &resource_id);

    RES_loadFile(command, 0, resource_id, 0);
    return next_cursor;
}

static char *command_loadface(int command, char *cursor, int mode)
{
    int resource_id = 0;
    char *next_cursor = search_key(cursor, (u8 *)model, &resource_id);

    RES_loadFile(command, 0, resource_id + 0x10000, 0);
    return next_cursor;
}

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
        ActorResource *resource = RES_loadFile(command, 3, (int)name, 0);

        resource->foot_step_no = foot_step;
    }
    return cursor;
}

u8 RES_GetFootStepNo(struct Actor *actor)
{
    ActorResource *resource = actor->resource;
    u8 foot_step_no = 0;

    if (resource != 0) {
        foot_step_no = resource->foot_step_no;
    }
    return foot_step_no;
}

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
    RES_loadFile(command, 0xB, resource_id, 0);
    return cursor;
}

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
    RES_loadFile(command, 0xA, resource_id, 0);
    return cursor;
}

static void command_loadeffect_sub(int type, char *effect_name, void *address)
{
    if (arcfileaddr != 0) {
        sefLoadMemoryEffectCfName(address, effect_name, next_arc_size(address));
        return;
    }
    sefLoadEffectCfName((int)address, effect_name);
}

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
    RES_loadFile(command, 6, (int)effect_name, (int)command_loadeffect_sub);
    return cursor;
}

static int command_loadene_sub(int type, const char *name, void *address)
{
    if (arcfileaddr != 0)
        next_arc_size(AdrsEnemyPreset);
    else
        Enemy_LoadPreset(AdrsEnemyPreset, name);

    return 0;
}

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
    RES_loadFile(command, 8, (int)name, (int)&command_loadene_sub);
    return cursor;
}

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

static int command_dummy(int resource_id, int result)
{
    if (GameResource[resource_id + 1].address == 0)
        GameResourceAlloc(0);
    else
        GameResourceRealloc((void *)GameResource[resource_id].address, 0);
    return result;
}

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

static u8 *command_loadse(int command, u8 *cursor)
{
    char sound_name[0x40];
    char sequence_name[0x40];
    char path[0x100];
    int character;
    u32 address;
    u16 file_id;
    int keep_current;
    u8 set_normal;
    int first_size;
    int second_size;
    u8 *name_end;
    u8 *sequence_end;
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
        character = (u8)*cursor;
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
        character = (u8)*cursor;
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

    if (sound_name[0] == 0) {
        xglSoundLoadEffect(0, 0, 2);
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

    if (env_name[0] == 0) {
        xglSoundLoadEffect(0, 0, 3);
    } else {
        file_id = SoundWork.effect_banks[3].handle;
        ((SoundLoadBuffer *)address)->sequence_id = file_id;
        if (arcfileaddr != 0) {
            first_size = next_arc_size((void *)address);
            second_address = (void *)(address + first_size);
            second_size = next_arc_size(second_address);
        } else {
            sprintf(path, soundSwdPathFormat, env_name);
            first_size = xglCdReadFile(path, (void *)address, 0, 0);
            second_address = (void *)(address + first_size);
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
            xglSoundSendEffect((void *)address, second_address, 3);
    }

    if (sequence_name[0] != 0) {
        if (sequence_name[0] == '.') {
            xglSoundSendSmd(0);
        } else {
            int size;
            if (arcfileaddr != 0) {
                size = next_arc_size((void *)address);
            } else {
                sprintf(path, soundSmdPathFormat, sequence_name);
                size = xglCdReadFile(path, (void *)address, 0, 0);
            }
            if (size != 0) {
                xglSoundSendSmd((void *)address);
                if (set_normal)
                    xglSoundSequenceNormal(0x7F);
            }
        }
    }
    return cursor;
}

static u8 *command_loadsmd(int command, u8 *cursor)
{
    char name[0x40];
    char path[0x100];
    s16 character;
    u32 address;
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
                file_size = xglCdReadFile(path, (void *)address, 0, 0);
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
                xglSoundSendSmd2((void *)address, sequence_index);
                if (set_normal)
                    xglSoundSequenceNormal2(sequence_index, 0x7F);
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

static char base_2[18] ;

extern void GameMpeg2Play(const char *name, int mode);

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
    return command_mpeg2_core(command, cursor, 2);
}

static char *command_mpeg2battle(int command, char *cursor, int mode)
{
    SCRIPT_fade(2);
    return command_mpeg2_core(command, cursor, 1);
}

static char *command_mpeg2nofade(int command, char *cursor)
{
    return command_mpeg2_core(command, cursor, 5);
}

static char *command_event2battle(int command, char *cursor, int mode)
{
    SCRIPT_frameLock2Battle();
    return cursor;
}

static char *command_player_lock(int command, char *cursor, int mode)
{
    GameLoopState.flags |= 0x8000;
    return cursor;
}

static char *command_enenpcse(int command, char *cursor, int mode)
{
    char name[0x40];
    char *dst;
    char *next;
    int character;
    int resource_id;
    int motion_id;
    int slot;
    u32 address;
    int first_size;
    int second_size;
    void *second_address;

    resource_id = 0;
    next = search_key(cursor, (u8 *)model, &resource_id);
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
    goto enemy_name_test;

enemy_name_copy:
    *dst++ = character;
    next++;
    character = (u8)*next;

enemy_name_test:
    if ((u8)character != ',' && (u8)character != ' ' &&
        (u8)character != '\t' && (u8)character != '\n' &&
        (u8)character != '\r')
        goto enemy_name_copy;
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
    goto enemy_digit_test;

enemy_digit:
    motion_id *= 10;
    motion_id += (u8)*next - '0';
    next++;
    character = *next & 0xff;

enemy_digit_test:
    if ((u8)((u8)character + 0xD0) < 10U)
        goto enemy_digit;

    address = GameResourceGetFreeAddr();
    for (slot = 0; slot < 7; slot++) {
        if (EnemySeBank[slot].id == 0)
            break;
    }
    EnemySeBank[slot].id = resource_id;
    EnemySeBank[slot].type = mode;
    EnemySeBank[slot].foot = motion_id;
    strcpy(EnemySeBank[slot].name, name);

    if (arcfileaddr != 0) {
        first_size = next_arc_size((void *)address);
        second_address = (void *)(address + first_size);
        second_size = next_arc_size(second_address);
        if (first_size != 0 && second_size != 0)
            xglSoundSendEffect((void *)address, second_address, slot + 4);
    } else {
        xglSoundLoadEffect(name, (void *)address, slot + 4);
    }
    return next;
}

static char *command_enese(int command, char *cursor, int mode)
{
    return command_enenpcse(command, cursor, 0);
}

static char *command_npcse(int command, char *cursor, int mode)
{
    return command_enenpcse(command, cursor, 1);
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

void GameResourcePreLoad(int scene_number)
{
    char path[256];
    char *archive_path;
    int archive_size;

    archive_path = RES_getScenePath(path, scene_number);
    arcfilepreload = 0;
    if (xglCdGetFileSize(path) > 0) {
        xglCdReadFile(path, scene_txt_buffer, 1, 1);

        archive_path[0] = 'a';
        archive_path[1] = '\0';
        archive_size = xglCdGetFileSize(path);
        if (archive_size > 0) {
            arcfilepreload = 0x02000000 - ((archive_size + 2047) & -2048);
            xglCdReadFile(path, (void *)arcfilepreload, 1, 1);
        }
    }
}

static void readreq(const char *name, void *buffer)
{
    xglCdReadFile(name, buffer, 0, 1);
}

void GameResourceLoad(int scene_id)
{
    typedef struct {
        u32 signature;
    } SceneTextHeader;

    char path[0x100];
    u8 *cursor;
    u8 *load_buffer;
    char *extension;
    int file_size;

    arcno = 0;
    GameLoopState.scene_id = scene_id;
    if (arcfilepreload != 0) {
        arcfileaddr = (u32 *)arcfilepreload;
    } else {
        load_buffer = (u8 *)scene_txt_buffer;
        ((SceneTextHeader *)load_buffer)->signature = 0xCDCDCDCD;
        arcfileaddr = 0;
        extension = RES_getScenePath(path, scene_id);
        readreq(path, load_buffer);
        *extension++ = 'a';
        *extension = '\0';
        file_size = xglCdGetFileSize(path);
        if (file_size > 0) {
            arcfileaddr = (u32 *)(0x02000000 - ((file_size + 0x7FF) & ~0x7FF));
            xglCdReadFile(path, arcfileaddr, 0, 1);
        }
    }

    cursor = (u8 *)scene_txt_buffer;
    if (((SceneTextHeader *)cursor)->signature == 0x6E637323) {
        int resource_id;

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
                if ((unsigned char)((unsigned char)*cursor + 0xD0) < 10) {
                    do {
                        resource_id *= 10;
                        resource_id += (u8)*cursor++ - '0';
                    } while ((unsigned char)((unsigned char)*cursor + 0xD0) < 10);
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

static int RES_loadFileSubMapSub(char *name, char *suffix, void *buffer)
{
    char path[0x100];
    char *dst = path;
    char *fallback_dst;
    char *prefix_cursor;
    char *fallback_cursor;
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
        fallback_cursor = (char *)dummy_3;
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
        file_size = RES_loadFileSubMapSub(map_name, (char *)lexSuffix, end_address);
    end_address = (u8 *)(((u32)end_address + file_size + 0xF) & ~0xF);
    state->texture_file = end_address;

    if (arcfileaddr != 0)
        file_size = next_arc_size(end_address);
    else
        file_size = RES_loadFileSubMapSub(map_name, (char *)xtxSuffix,
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
        file_size = RES_loadFileSubMapSub(map_name, (char *)idSuffix,
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

static int RES_GetMdlFileNameSub(char *dst, int selector)
{
    int face_mode = (((u32)selector & 0xFFFF0000) == 0x00010000);
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

    alias_entry = (const ResourceKeyEntry *)model_entry->aliases + alias_group;
    names = (const char *const *)alias_entry->aliases;
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
    case 7: {
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
            readreq(single_path, (void *)address);
        break;
    }
    case 0:
    case 4: {
        char *model_end;
        u8 *next;
        ResourceFileSet *files;

        next = (u8 *)address + 16;
        RES_GetMdlFileName(model_path, resource_id);
        model_end = model_path;
        while (*model_end != 0)
            model_end++;
        files = (ResourceFileSet *)address;
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
        if ((resource_id & 0xffff0000) == 0) {
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
        if (type == 0) {
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
    case 1:
        size = RES_loadFileSubMap(address, resource_id);
        break;
    case 2:
        if (arcfileaddr != 0) {
            size = next_arc_size(address);
            break;
        }
        sprintf(motion_path, motionFpkPathFormat, RES_GetMotBaseName(resource_id));
        size = xglCdGetFileSize(motion_path);
        if (size > 0) {
            size = xglCdGetFileSize(motion_path);
            readreq(motion_path, (void *)address);
        }
        break;
    case 3: {
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
            readreq(named_path, (void *)address);
        }
        break;
    }
    case 10:
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
        goto format_numbered_path;
    case 11:
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

format_numbered_path:
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
            readreq(path, (void *)address);
        break;
    case 5: {
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

int RES_loadFile(int command, int callback, int resource_id, int flags)
{
    int result = 0;
    int slot;
    int loaded_size;
    int old_span;

    if (command == -1) {
        command = GameResourceSearch(callback, resource_id);
        if (command < 0 && callback == 4) {
            command = GameResourceSearch(0, resource_id);
        } else if (command < 0 && callback == 0) {
            command = GameResourceSearch(4, resource_id);
            if (command >= 0) {
                ResourceFileSet *resource_info = (void *)GameResource[command].address;

                if (resource_info->third_file == 0) {
                    slot = resource_get_free();
                    resource_info->third_file = (void *)GameResource[slot].address;
                    RES_loadFile(slot, 7, resource_id, flags);
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
    } else if (command < 0x80) {
        result = (int)GameResource[command].address;
        if (flags == 0)
            flags = (int)RES_loadFileSub;
        loaded_size = ((int (*)(int, int, void *))flags)(callback, resource_id, (void *)result);
        if (loaded_size < 0)
            return 0;
        {
            loaded_size = (loaded_size + 0x3F) & ~0x3F;
            old_span = GameResource[command].size;
            if (loaded_size > old_span)
                loaded_size = old_span;
            if (command < 0x7F && GameResource[command + 1].address == 0) {
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

int GameResourceGetIndex(int address)
{
    int index = 0;
    GameResourceEntry *entry = GameResource;
    int resourceAddress;
    int result;

    do {
        if (index >= 0x80)
            break;
        resourceAddress = entry->address;
        entry++;
        result = index++;
        if (resourceAddress == address)
            return result;
    } while (resourceAddress != 0);
    return -1;
}

void GameResourceDump(int dump)
{
    GameResourceEntry *entry;
    int index;
    int row;

    if (dump != 0) {
        index = 127;
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

void GameResourceReset(int slot)
{
    int total;
    int index;

    total = 0;
    index = slot;
    if (index < 0x80) {
        for (; index < 0x80; index++)
            total += GameResource[index].size;
    }
    GameResource[slot].size = total;
    GameResource[slot].state = -1;
    GameResource[slot].handle = 0;
    for (index = slot + 1; index < 0x80; index++) {
        GameResource[index].address = 0;
        GameResource[index].size = 0;
        GameResource[index].handle = 0;
        GameResource[index].state = -1;
    }
    index = 0;
    do {
        EnemySeBank[index].id = 0;
        EnemySeBank[index].type = 0;
        EnemySeBank[index].name[0] = 0;
        xglSoundSendEffect(0, 0, index + 4);
        index++;
    } while (index < 8);
    GameResourceDump(0);
    arcfilepreload = 0;
}

void GameResourceInit(u32 address, int size)
{
    u32 *resource_buffer;
    int remaining = 0x7F;
    int free_state = -1;
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

extern const char ResZ8Name[8];
extern const char ResCSName[8];
extern const char ResMOName[8];
extern const char ResKMName[8];
extern const char ResSOName[8];
extern const char ResJRName[8];

static const char * tbl_1_00366470[7] = { ResEmptyName, ResSOName, ResKMName, ResCSName, ResMOName, ResJRName, ResZ8Name };

static char base_2[18] = "data\\movie\\mpeg2\\";

static char dummy_3[15] = "data\\map\\dummy";

static char base_6[13] = "data\\motion\\";

static char base_7[29] = "data\\movie\\small\\mvs0000.ipu";

static char base_8[24] = "data\\texture\\fm0000.xtx";

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

const u8 lexSuffix[8] = ".lex";

const u8 xtxSuffix[8] = ".xtx";

const u8 idSuffix[8] = ".id";

static const char root_4[6] = "data\\";

static const char dummy_5[6] = "dummy";

const char faceSuffix[8] = "_face";

const char jntSuffix[8] = ".jnt";

static const char base_9[6] = "data\\";

unsigned char ModelPath = 0;

/* Original storage of this TU without a C reader; typed from its original bytes. */
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

MotionEntry cfMtnList[31] = { { .name = ResShionCfName, .ids = { 1, 9, 10, 11, 12, 13, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResKosmosCfName, .ids = { 2, 14, 15, 16, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResChaosCfName, .ids = { 3, 18, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResMomoCfName, .ids = { 4, 20, 21, 22, 23, 50, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResJrCfName, .ids = { 5, 24, 25, 26, 47, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResZiggyCfName, .ids = { 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResShitanCfName, .ids = { 7, 28, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResAllenCfName, .ids = { 263, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResOldwCfName, .ids = { 1579, 1582, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResOldmCfName, .ids = { 1549, 1552, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResMiddlemCfName, .ids = { 1543, 1546, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResMiddlewCfName, .ids = { 1573, 1576, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResSoldierwCfName, .ids = { 1289, 542, 523, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResSoldiermCfName, .ids = { 526, 536, 525, 513, 527, 1288, 537, 519, 0, 0, 0, 0, 0, 0 } }, { .name = ResWomanCfName, .ids = { 1567, 1570, 783, 533, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResYoungmCfName, .ids = { 1537, 1540, 780, 530, 529, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResYoungwCfName, .ids = { 1591, 1594, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResGuno001CfName, .ids = { 16385, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResGuno301Name, .ids = { 16410, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResGuno302Name, .ids = { 16411, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResGuno303Name, .ids = { 16412, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResGuno304Name, .ids = { 16413, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResGuno305Name, .ids = { 16414, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResGuno323Name, .ids = { 16432, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResGuno324Name, .ids = { 16433, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResGuno325Name, .ids = { 16434, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResGuno301Name, .ids = { 16410, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResGuno302Name, .ids = { 16411, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResGuno303Name, .ids = { 16412, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = ResGuno307Name, .ids = { 16416, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }, { .name = 0, .ids = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } } };

FaceEntry faceTbl[7] = { { .table = facePC, .count = 54, .group = 0 }, { .table = faceNPC, .count = 89, .group = 256 }, { .table = facePPL1, .count = 32, .group = 512 }, { .table = facePPL2, .count = 29, .group = 768 }, { .table = facePPL3, .count = 17, .group = 1024 }, { .table = facePPL4, .count = 10, .group = 1280 }, { .table = 0, .count = 0, .group = 0 } };

extern const char ResResetName[];
extern const char ResLoadmapName[];
extern const char ResLoadactName[];
extern const char ResLoadmotName[];
extern const char ResLoadtexName[];
extern const char ResLoadeneName[];
extern const char ResScriptName[];
extern const char ResPlayerName[];
extern const char ResDummyName[];
extern const char ResLoadseName[];
extern const char ResLoadsmdName[];
extern const char ResMpeg2Name[];
extern const char ResEneseName[];
extern const char ResNpcseName[];
extern const char ResDiskName[];
extern const char ResSaveName[];

static u16 facePC[54] = {
    0x0000, 0x0001, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0002, 0x0002,
    0x0002, 0x0002, 0x0003, 0x0003, 0x0004, 0x0004, 0x0004, 0x0004,
    0x0005, 0x0005, 0x0005, 0x0006, 0x0000, 0x0000, 0x001E, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x001F, 0x001F, 0x001F,
    0x001F, 0x0025, 0x001F, 0x001E, 0x0022, 0x001E, 0x001E, 0x0005,
    0x001E, 0x001E, 0x0004, 0x0021, 0x0004, 0x001E
};

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
