#include "common.h"

#include "shared.h"
#include "debug_break.h"

#include "main/near_dir.h"

typedef struct ActorUndulation {
    u8 unmodeled_00[8];
    s16 attr_mask;
} ActorUndulation;

typedef struct PlayerEffect {
    u8 unmodeled_00[0x6bc];
    struct LookAtPlayerActor *owner;
} PlayerEffect;

typedef struct PlayerEnemyWork {
    u8 unmodeled_00[0x3830];
    s16 encount_lock;
    u8 unmodeled_3832[0x38b0 - 0x3832];
} PlayerEnemyWork;

typedef struct LookAtPlayerActor {
    u32 flags;
    void (*update)(struct LookAtPlayerActor *actor);
    void (*draw)(struct LookAtPlayerActor *actor);
    u32 quadword_alignment_gap;
    Vector4 position;
    Vector4 previous_position;
    Vector4 velocity;
    Vector4 external_velocity;
    float rotation_x;
    float rotation_y;
    float rotation_z;
    float move_speed;
    Vector4 scale;
    u8 unmodeled_70[0x10];
    u8 number;
    u8 signal;
    u8 unmodeled_82[0x88 - 0x82];
    s16 frame_counter;
    u8 unmodeled_8a[0x124 - 0x8a];
    enum { LOCATOR_FLAGS_PLAYER_INIT = 0x70000 } locator_flags;
    u8 unmodeled_128[0x4c0 - 0x128];
    void *java_object;
    u8 unmodeled_4c4[0x4c8 - 0x4c4];
    ActorUndulation undulation;
    u8 unmodeled_4d2[0x4e8 - 0x4d2];
    u64 terrain_flags;
    u8 unmodeled_4f0[0x694 - 0x4f0];
    signed char stick_x;
    signed char stick_y;
    u8 unmodeled_696[0x6f8 - 0x696];
    float motion_speed;
    u8 unmodeled_6fc[0x704 - 0x6fc];
    u16 motion_id;
    u8 unmodeled_706[0x9a0 - 0x706];
    u32 render_flags;
    u8 unmodeled_9a4[0x9c0 - 0x9a4];
    float filter_param;
    u8 unmodeled_9c4[0x9d8 - 0x9c4];
    u64 flash_frames;
    u8 unmodeled_9e0[0x9e4 - 0x9e0];
    float aim_angle;
    float body_radius;
    s16 look_at_timer;
    s16 look_at_target;
    s16 flash_timer;
    u8 unmodeled_9f2[0xa10 - 0x9f2];
    PlayerEffect *effects[8];
    s16 effect_timer;
    s16 effect_slot;
    u8 unmodeled_a34[0xa70 - 0xa34];
} LookAtPlayerActor;

extern PlayerEnemyWork enepc[16];

typedef struct PlayerLookAtState {
    u32 unmodeled_00;
    void *player_actor;
    u32 unmodeled_08;
    u32 unmodeled_0c;
    u32 flags;
    u8 unmodeled_14[0xc];
    u32 map_flags;
    u8 unmodeled_24[0x9c];
    u8 move_mode;
    u8 unmodeled_c1[0x29e80];
    signed char look_at_target;
    unsigned char unmodeled_29f42[0x29fa0 - 0x29f42];
    signed char ladder_direction[16]; /* +0x29fa0 */
    unsigned char ladder_bottom_exit[16]; /* +0x29fb0 */
    float ladder_height[16]; /* +0x29fc0 */
    unsigned char unmodeled_2a000[0x2a014 - 0x2a000];
    void *effect;
} PlayerLookAtState;

typedef union PlayerAimVector {
    Vector4 vector;
    u64 doublewords[2];
} PlayerAimVector;

typedef struct PlayerAimMapUnit {
    u8 unmodeled_00[0x10];
    PlayerAimVector position;
    u8 unmodeled_20[0x2e0];
} PlayerAimMapUnit;

extern PlayerLookAtState GameLoopState;

typedef struct LookAtSavedPartyPrefix {
    unsigned char unmodeled_00[0x100a4];
    u16 player_character_id;
} LookAtSavedPartyPrefix;

typedef struct LookAtGameResource {
    u32 address;
    int size;
    int handle;
    int state;
} LookAtGameResource;

extern unsigned char SaveData[];

extern LookAtGameResource GameResource[128];

void GameCfPlayerLoadResource(int use_cache);

void *GameResourceAlloc(int size);

void *GameResourceGetFreeAddr(void);

void RES_GetLeaderSeName(char *name);

char *RES_GetMotBaseName(int id);

void RES_GetMdlFileName(char *name, int id);

int xglCdReadFile(const char *name, void *address, int size, int arg);

void xglSoundSendEffect(void *first, void *second, int id);

void xglSoundLoadEffect(const char *name, void *address, int id);

int xglFlagsGet(int flag, int value);

void ACT_setModelWrapper(LookAtPlayerActor *player_actor, int mode);

void ACT_setHumanHand(LookAtPlayerActor *player_actor, int mode);

extern LookAtPlayerActor actor[64];

extern PlayerAimMapUnit MapUnit[64];

int Get_MostNear_Actor(LookAtPlayerActor *player_actor);

void Actor_LookAt_Set(LookAtPlayerActor *player_actor, int mode, Vector4 *target);

void Actor_LookAt_Release(LookAtPlayerActor *player_actor, int mode);

void Actor_LookAt(LookAtPlayerActor *player_actor);

void PlayerLookAtAim(void);

/* Player-movement speed thresholds and vector scaling rate GameCfPlayerMove
 * compares/applies each frame; GameCfPlayerMoveInit restores their defaults. */

static int WALK_THRESHOLD_I;

static float WALK_THRESHOLD_F;

static int RUN_THRESHOLD_I;

static float RUN_THRESHOLD_F;

static float VECTOR_RATE;

extern int F2I(float value);

#define DEFAULT_VECTOR_RATE 0.0007999999798f

extern const char ResShion6HName[16];

extern const char ResShion5HShion1HName[24];

extern const char ResShion4HShionHName[24];

extern const char ResShion3HName[16];

extern const char ResShion2HName[16];

extern const char ResShion1HShion5HName[24];

extern const char ResKosmosH6Name[16];

extern const char ResKosmosH5Name[16];

extern const char ResKosmosH4Name[16];

extern const char ResKosmosH3Name[16];

extern const char ResKosmosH2Name[16];

extern const char ResKosmosH1Name[16];

extern const char ResShitanHName[16];

extern const char ResKosmosHName[16];

extern const char ResShionHShion4HName[24];

extern const char ResShion5Shion1Name[16];

extern const char ResShion1Shion5Name[16];

extern const char ResShionChName[16];

extern const char ResShionShion4Name[16];

extern const char ResAlbelt3HName[16];

extern const char ResHammerHHammerName[24];

extern const char ResTonnyHTonnyName[16];

extern const char ResGaignunHName[16];

extern const char ResAndrewHAndrew6Name[24];

extern const char ResAndrewHAndrew5Name[24];

extern const char ResAndrewHAndrew4Name[24];

extern const char ResAndrewHAndrew3Name[24];

extern const char ResAndrewHAndrew2Name[24];

extern const char ResAndrewHAndrew1Name[24];

extern const char ResAndrewHAndrewName[24];

extern const char ResVirgil3HName[16];

extern const char ResVirgil1HName[16];

extern const char ResAlbelt2HName[16];

extern const char ResVirgil4HName[16];

extern const char ResVirgil2HName[16];

extern const char ResKebin1HName[16];

extern const char ResMatehwsHMatehwsName[24];

extern const char ResVirgilHName[16];

extern const char ResAlbeltHName[16];

extern const char ResMarglisHName[16];

extern const char ResJoachim1Name[16];

extern const char ResCecilia1Name[16];

extern const char ResAndDaugName[16];

extern const char ResAndWifeName[16];

extern const char ResMoriyamaName[16];

extern const char ResStepMamName[16];

extern const char ResMatehws1Name[16];

extern const char ResGaignun1Name[16];

extern const char ResMarglis1Name[16];

extern const char ResBrealW1Name[16];

extern const char ResBrealM4Name[16];

extern const char ResBrealM3Name[16];

extern const char ResBrealM2Name[16];

extern const char ResBrealM1Name[16];

extern const char ResKingCatName[16];

extern const char ResPilotNoName[16];

extern const char ResInnGirlName[16];

extern const char ResBreadChName[16];

extern const char ResKidswB2Name[16];

extern const char ResKidswB1Name[16];

extern const char ResKidswA2Name[16];

extern const char ResKidswA1Name[16];

extern const char ResKidsmB2Name[16];

extern const char ResKidsmB1Name[16];

extern const char ResKidsmA2Name[16];

extern const char ResKidsmA1Name[16];

extern const char ResGrFinalName[16];

extern const char ResEwpHg01Name[16];

extern const char ResBossBossPath[16];

extern const char ResFereFerePath[16];

extern const char ResFesoFesoPath[16];

extern const char ResFeroFeroPath[16];

extern const char ResFemaFemaPath[16];

extern const char ResUtroUtroPath[16];

extern const char ResUtsoUtsoPath[16];

extern const char ResUtreUtrePath[16];

extern const char ResUtmaUtmaPath[16];

extern const char ResGunoGunoPath[16];

extern const char ResMobj000MobjPath[16];

extern const char ResAobj000AccPath[16];

extern const char ResMusobjMusPath[16];

extern const char ResMufobjMufPath[16];

extern const char ResMuxobjMuxPath[16];

extern const char ResTreobjTrePath[16];

extern const char ResMuwobjMuwPath[16];

extern const char ResMuobjMUPath[16];

extern const char ResDataSceneItaiPath[24];

extern const char ResDataSceneYagiPath[24];

extern const char ResDataSceneSugisawaPath[24];

extern const char ResDataSceneSatoPath[24];

extern const char ResDataSceneKeiichiPath[24];

extern const char ResDataSceneKuramotoPath[24];

extern const char ResDataSceneYajimaPath[24];

extern const char ResDataSceneNakaharaPath[24];

extern const char ResDataSceneNvYonePath[24];

extern const char ResDataSceneKojiPath[24];

extern const char ResDataSceneKonishiPath[24];

extern const char ResDataSceneSakisakoPath[24];

extern const char ResDataSceneKojimaPath[24];

extern const char ResDataSceneFujiPath[24];

extern const char ResDataSceneGashPath[24];

extern const char ResDataSceneEventPath[24];

extern const char ResDataSceneCfPath[16];

extern const char ResDataSceneCheckPath[24];

extern const char ResDataSceneTestPath[24];

extern const char ResDataScenePath[16];

extern const char ResMomo6Name[8];

extern const char ResMomo5HName[8];

extern const char ResMomo5Name[8];

extern const char ResJr1HName[8];

extern const char ResZiggyHName[8];

extern const char ResJrHName[8];

extern const char ResMomoHName[8];

extern const char ResChaosHName[8];

extern const char ResShitan2Name[8];

extern const char ResShitan1Name[8];

extern const char ResZiggy1Name[8];

extern const char ResJr3Name[8];

extern const char ResJr2Name[8];

extern const char ResJr1Name[8];

extern const char ResMomo4Name[8];

extern const char ResMomo3Name[8];

extern const char ResMomo2Name[8];

extern const char ResMomo1Name[8];

extern const char ResChaos2Name[8];

extern const char ResChaos1Name[8];

extern const char ResKosmos5Name[8];

extern const char ResKosmos3Name[8];

extern const char ResKosmos2Name[8];

extern const char ResKosmos1Name[8];

extern const char ResShionName[8];

extern const char ResShion3Name[8];

extern const char ResShion2Name[8];

extern const char ResShitanName[8];

extern const char ResZiggyName[8];

extern const char ResMomoName[8];

extern const char ResChaosName[8];

extern const char ResKosmosName[8];

extern const char ResAndrew9Name[8];

extern const char ResFeb1Name[8];

extern const char ResAndrew8Name[8];

extern const char ResAndrew7Name[8];

extern const char ResAndrew6Name[8];

extern const char ResAndrew5Name[8];

extern const char ResEllyHName[8];

extern const char ResFriedHName[8];

extern const char ResKebinHName[8];

extern const char ResAllenHName[8];

extern const char ResCath1Name[8];

extern const char ResChiefName[8];

extern const char ResVanName[8];

extern const char ResElly2Name[8];

extern const char ResKJoachName[8];

extern const char ResShalronName[8];

extern const char ResHynlinName[8];

extern const char ResTogashiName[8];

extern const char ResPelegriName[8];

extern const char ResShiMamName[8];

extern const char ResShiDadName[8];

extern const char ResCathName[8];

extern const char ResCeciliaName[8];

extern const char ResLapisName[8];

extern const char ResMiyukiName[8];

extern const char ResVoyagerName[8];

extern const char ResHelmerName[8];

extern const char ResAbelName[8];

extern const char ResShelleyName[8];

extern const char ResMaryName[8];

extern const char ResSellersName[8];

extern const char ResJoachimName[8];

extern const char ResYuriName[8];

extern const char ResFebName[8];

extern const char ResAndrew4Name[8];

extern const char ResAndrew3Name[8];

extern const char ResAndrew2Name[8];

extern const char ResAndrew1Name[8];

extern const char ResAndrewName[8];

extern const char ResHammerName[8];

extern const char ResTonnyName[8];

extern const char ResMatehwsName[8];

extern const char ResEllyName[8];

extern const char ResFriedName[8];

extern const char ResVirgil4Name[8];

extern const char ResVirgil3Name[8];

extern const char ResVirgil2Name[8];

extern const char ResVirgil1Name[8];

extern const char ResVirgilName[8];

extern const char ResKebin2Name[8];

extern const char ResKebin1Name[8];

extern const char ResKebinName[8];

extern const char ResAllen1Name[8];

extern const char ResAllenName[8];

extern const char ResAlbelt1Name[8];

extern const char ResAlbeltName[8];

extern const char ResGaignunName[8];

extern const char ResMarglisName[8];

extern const char ResBrealWName[8];

extern const char ResBrealMName[8];

extern const char ResEsoldName[8];

extern const char ResRwW2Name[8];

extern const char ResRwW1Name[8];

extern const char ResRwWName[8];

extern const char ResRwM2Name[8];

extern const char ResRwM1Name[8];

extern const char ResRwMName[8];

extern const char ResSuukiName[8];

extern const char ResEngOffName[8];

extern const char ResEngName[8];

extern const char ResSoldName[8];

extern const char ResPilotName[8];

extern const char ResOffW1Name[8];

extern const char ResOffWName[8];

extern const char ResOffM3Name[8];

extern const char ResOffM2Name[8];

extern const char ResOffM1Name[8];

extern const char ResOffMName[8];

extern const char ResVecW1Name[8];

extern const char ResVecWName[8];

extern const char ResVecM2Name[8];

extern const char ResVecM1Name[8];

extern const char ResVecMName[8];

extern const char ResSpaceName[8];

extern const char ResUSoldsName[8];

extern const char ResRealW2Name[8];

extern const char ResRealW1Name[8];

extern const char ResRealWName[8];

extern const char ResRealM2Name[8];

extern const char ResRealM1Name[8];

extern const char ResRealMName[8];

extern const char ResUSpaceName[8];

extern const char ResUSoldName[8];

extern const char ResGoveW2Name[8];

extern const char ResGoveW1Name[8];

extern const char ResGoveWName[8];

extern const char ResCommi5Name[8];

extern const char ResCommi4Name[8];

extern const char ResCommi3Name[8];

extern const char ResCommi2Name[8];

extern const char ResCommi1Name[8];

extern const char ResCommiName[8];

extern const char ResAssM4Name[8];

extern const char ResAssM3Name[8];

extern const char ResKanchoName[8];

extern const char ResGoveM2Name[8];

extern const char ResGoveM1Name[8];

extern const char ResGoveMName[8];

extern const char ResAssWName[8];

extern const char ResAssM2Name[8];

extern const char ResAssM1Name[8];

extern const char ResAssMName[8];

extern const char ResHyakuName[8];

extern const char ResDu2Name[8];

extern const char ResDu1Name[8];

extern const char ResPolWName[8];

extern const char ResPolMName[8];

extern const char ResUticW2Name[8];

extern const char ResUticW1Name[8];

extern const char ResUticWName[8];

extern const char ResUticM2Name[8];

extern const char ResUticM1Name[8];

extern const char ResUticMName[8];

extern const char ResContName[8];

extern const char ResEngOjiName[8];

extern const char ResScotName[8];

extern const char ResInnOjiName[8];

extern const char ResSpyName[8];

extern const char ResBartenName[8];

extern const char ResGoroName[8];

extern const char ResMouthName[8];

extern const char ResCleObaName[8];

extern const char ResKingName[8];

extern const char ResRobotName[8];

extern const char ResCleChName[8];

extern const char ResNurseName[8];

extern const char ResDoctorName[8];

extern const char ResBarMName[8];

extern const char ResBarOjiName[8];

extern const char ResBreadName[8];

extern const char ResGirlB2Name[8];

extern const char ResGirlB1Name[8];

extern const char ResGirlBName[8];

extern const char ResGirlA2Name[8];

extern const char ResGirlA1Name[8];

extern const char ResGirlAName[8];

extern const char ResKidswBName[8];

extern const char ResKidswAName[8];

extern const char ResBabaB2Name[8];

extern const char ResBabaB1Name[8];

extern const char ResBabaBName[8];

extern const char ResBabaA2Name[8];

extern const char ResBabaA1Name[8];

extern const char ResBabaAName[8];

extern const char ResObaB2Name[8];

extern const char ResObaB1Name[8];

extern const char ResObaBName[8];

extern const char ResObaA2Name[8];

extern const char ResObaA1Name[8];

extern const char ResObaAName[8];

extern const char ResWmanB2Name[8];

extern const char ResWmanB1Name[8];

extern const char ResWmanBName[8];

extern const char ResWmanA2Name[8];

extern const char ResWmanA1Name[8];

extern const char ResWmanAName[8];

extern const char ResBoyB2Name[8];

extern const char ResBoyB1Name[8];

extern const char ResBoyBName[8];

extern const char ResBoyA2Name[8];

extern const char ResBoyA1Name[8];

extern const char ResBoyAName[8];

extern const char ResKidsmBName[8];

extern const char ResKidsmAName[8];

extern const char ResJijiB2Name[8];

extern const char ResJijiB1Name[8];

extern const char ResJijiBName[8];

extern const char ResJijiA2Name[8];

extern const char ResJijiA1Name[8];

extern const char ResJijiAName[8];

extern const char ResOjiB2Name[8];

extern const char ResOjiB1Name[8];

extern const char ResOjiBName[8];

extern const char ResOjiA2Name[8];

extern const char ResOjiA1Name[8];

extern const char ResOjiAName[8];

extern const char ResManB2Name[8];

extern const char ResManB1Name[8];

extern const char ResManBName[8];

extern const char ResManA2Name[8];

extern const char ResManA1Name[8];

extern const char ResManAName[8];

extern const char ResFile02HoName[8];

extern const char ResFile01Ho2Name[8];

extern const char ResFile01HoName[8];

extern const char ResFile01CvName[8];

extern const char ResSuffix03Name[8];

extern const char ResSuffix02Name[8];

extern const char ResSuffix01Name[8];

extern const char ResGrLegName[8];

extern const char ResGrBodyName[8];

extern const char ResGrHandName[8];

extern const char ResGrHeadName[8];

extern const char ResUkunSName[8];

extern const char ResUkunBName[8];

extern const char ResTs01Name[8];

extern const char ResVx1MName[8];

extern const char ResUknGName[8];

extern const char ResBw04Name[8];

extern const char ResBw03Name[8];

extern const char ResBw02Name[8];

extern const char ResBw01Name[8];

extern const char ResHw06Name[8];

extern const char ResHw05Name[8];

extern const char ResHw04Name[8];

extern const char ResHw03Name[8];

extern const char ResHw02Name[8];

extern const char ResHw01Name[8];

extern const char ResRd17Name[8];

extern const char ResRd16Name[8];

extern const char ResRd15Name[8];

extern const char ResRd14Name[8];

extern const char ResRd13Name[8];

extern const char ResRd12Name[8];

extern const char ResRd11Name[8];

extern const char ResRd10Name[8];

extern const char ResRd09Name[8];

extern const char ResRd08Name[8];

extern const char ResRd07Name[8];

extern const char ResRd06Name[8];

extern const char ResRd05Name[8];

extern const char ResRd04Name[8];

extern const char ResRd03Name[8];

extern const char ResRd02Name[8];

extern const char ResRd01Name[8];

extern const char ResSw01Name[8];

extern const char ResJrHg08Name[8];

extern const char ResJrHg07Name[8];

extern const char ResJrHg06Name[8];

extern const char ResGun05Name[8];

extern const char ResGun04Name[8];

extern const char ResGun03Name[8];

extern const char ResGun02Name[8];

extern const char ResGun01Name[8];

extern const char ResSw05Name[8];

extern const char ResSw04Name[8];

extern const char ResSw03Name[8];

extern const char ResSw02Name[8];

extern const char ResMboxName[8];

extern const char ResMsp02RName[8];

extern const char ResHb00RName[8];

extern const char ResGlg00RName[8];

extern const char ResDeff00Name[8];

extern const char ResSol00Name[8];

extern const char ResWire00Name[8];

extern const char ResSmp01Name[8];

extern const char ResEcm02Name[8];

extern const char ResEcm01Name[8];

extern const char ResEad00Name[8];

extern const char ResBw00Name[8];

extern const char ResBbc00Name[8];

extern const char ResVsd00Name[8];

extern const char ResFsd00Name[8];

extern const char ResShb00Name[8];

extern const char ResMsp03Name[8];

extern const char ResMsp01Name[8];

extern const char ResAmp02Name[8];

extern const char ResLgc00Name[8];

extern const char ResBp00Name[8];

extern const char ResEac00Name[8];

extern const char ResCb00Name[8];

extern const char ResBl00Name[8];

extern const char ResMsp02Name[8];

extern const char ResMsp00Name[8];

extern const char ResHb00Name[8];

extern const char ResGlg00Name[8];

extern const char ResGnd00Name[8];

extern const char ResFl00Name[8];

extern const char ResPbk00Name[8];

extern const char ResLsr00Name[8];

extern const char ResDrl01Name[8];

extern const char ResDrl00Name[8];

extern const char ResBs00Name[8];

extern const char ResBam00Name[8];

extern const char ResRf02Name[8];

extern const char ResRf01Name[8];

extern const char ResRf00Name[8];

extern const char ResMg01Name[8];

extern const char ResMg00Name[8];

extern const char ResHmr02Name[8];

extern const char ResDlc00Name[8];

extern const char ResHmr00Name[8];

extern const char ResHg01Name[8];

extern const char ResHg00Name[8];

extern const char ResWct00Name[8];

extern const char ResBd01Name[8];

extern const char ResBd00Name[8];

extern const char ResAx00Name[8];

extern const char ResAcc036Name[8];

extern const char ResAcc035Name[8];

extern const char ResAcc034Name[8];

extern const char ResAcc033Name[8];

extern const char ResAcc032Name[8];

extern const char ResAcc031Name[8];

extern const char ResAcc030Name[8];

extern const char ResAcc029Name[8];

extern const char ResAcc028Name[8];

extern const char ResAcc018Name[8];

extern const char ResWingName[8];

extern const char ResFile323Name[8];

extern const char ResFile322Name[8];

extern const char ResFile321Name[8];

extern const char ResFile320Name[8];

extern const char ResFile313Name[8];

extern const char ResFile312Name[8];

extern const char ResFile311Name[8];

extern const char ResFile310Name[8];

extern const char ResFile303Name[8];

extern const char ResFile302Name[8];

extern const char ResFile301Name[8];

extern const char ResFile023Name[8];

extern const char ResFile021Name[8];

extern const char ResFile020Name[8];

extern const char ResFile012Name[8];

extern const char ResFile011Name[8];

extern const char ResFile010Name[8];

extern const char ResFile003Name[8];

extern const char ResFile002Name[8];

extern const char ResFile001Name[8];

extern const char ResFile033Name[8];

extern const char ResFile032Name[8];

extern const char ResFile031Name[8];

extern const char ResFile030Name[8];

extern const char ResFile180Name[8];

extern const char ResFile173Name[8];

extern const char ResFile172Name[8];

extern const char ResFile171Name[8];

extern const char ResFile170Name[8];

extern const char ResFile163Name[8];

extern const char ResFile162Name[8];

extern const char ResFile161Name[8];

extern const char ResFile160Name[8];

extern const char ResFile153Name[8];

extern const char ResFile152Name[8];

extern const char ResFile151Name[8];

extern const char ResFile150Name[8];

extern const char ResFile143Name[8];

extern const char ResFile142Name[8];

extern const char ResFile141Name[8];

extern const char ResFile140Name[8];

extern const char ResFile133Name[8];

extern const char ResFile132Name[8];

extern const char ResFile131Name[8];

extern const char ResFile130Name[8];

extern const char ResFile123Name[8];

extern const char ResFile122Name[8];

extern const char ResFile121Name[8];

extern const char ResFile120Name[8];

extern const char ResFile113Name[8];

extern const char ResFile112Name[8];

extern const char ResFile111Name[8];

extern const char ResFile110Name[8];

extern const char ResFile103Name[8];

extern const char ResFile102Name[8];

extern const char ResFile101Name[8];

extern const char ResFile100Name[8];

extern const char ResFile093Name[8];

extern const char ResFile092Name[8];

extern const char ResFile090Name[8];

extern const char ResFile082Name[8];

extern const char ResFile081Name[8];

extern const char ResFile080Name[8];

extern const char ResFile073Name[8];

extern const char ResFile072Name[8];

extern const char ResFile071Name[8];

extern const char ResFile070Name[8];

extern const char ResFile063Name[8];

extern const char ResFile062Name[8];

extern const char ResFile061Name[8];

extern const char ResFile060Name[8];

extern const char ResFile052Name[8];

extern const char ResFile051Name[8];

extern const char ResFile050Name[8];

extern const char ResFile043Name[8];

extern const char ResFile042Name[8];

extern const char ResFile041Name[8];

extern const char ResCitiPath[8];

extern const char ResPplPath[8];

extern const char ResNpcPath[8];

extern const char ResPcPath[8];

extern const char ResEtcPath[8];

extern const char ResAgAgPath[8];

extern const char ResVxVxPath[8];

extern const char ResVxName[8];

extern const char ResPcChoPath[8];

extern const char ResChoName[8];

extern const char ResEnemyPath[8];

extern const char ResRoboPath[8];

extern const char ResPcSonPath[8];

extern const char ResSonName[8];

extern const char ResPcStnPath[8];

extern const char ResStnName[8];

extern const char ResPcJrPath[8];

extern const char ResJrPrefixName[8];

extern const char ResPcZigPath[8];

extern const char ResZigName[8];

extern const char ResPcMmoPath[8];

extern const char ResMmoName[8];

extern const char ResPcKosPath[8];

extern const char ResKosName[8];

extern const char ResBossName[8];

extern const char ResFereName[8];

extern const char ResFesoName[8];

extern const char ResFeroName[8];

extern const char ResFemaName[8];

extern const char ResUtroName[8];

extern const char ResUtsoName[8];

extern const char ResUtreName[8];

extern const char ResUtmaName[8];

extern const char ResGunoName[8];

extern const char ResMusName[8];

extern const char ResMufName[8];

extern const char ResMuxName[8];

extern const char ResTreName[8];

extern const char ResMuwName[8];

extern const char ResMuName[8];

extern const char ResAidName[8];

extern const char ResObjPath[8];

extern const char ResMidName[8];

extern const char ResEidName[8];

extern const char ResWeaponPath[8];

extern const char ResWidName[8];

extern const char ResRidName[8];

extern const char ResCharPath[8];

extern const char ResCidName[8];

extern const char ResJr4Name[8];

extern const char ResJrName[8];

extern const char ResDuName[8];

extern const char ResFile06Name[8];

extern const char ResFile05Name[8];

extern const char ResFile04Name[8];

extern const char ResFile03Name[8];

extern const char ResFile02Name[8];

extern const char ResFile01Name[8];

extern const char ResFile09Name[8];

extern const char ResFile08Name[8];

extern const char ResFile07Name[8];

extern const char ResSuffix06Name[8];

extern const char ResSuffix05Name[8];

extern const char ResSuffix04Name[8];

extern const char ResFile114Name[8];

extern const char ResEmptyName[8];

extern const char ResFile325Name[8];

extern const char ResFile324Name[8];

extern const char ResFile319Name[8];

extern const char ResFile318Name[8];

extern const char ResFile317Name[8];

extern const char ResFile316Name[8];

extern const char ResFile315Name[8];

extern const char ResFile314Name[8];

extern const char ResFile309Name[8];

extern const char ResFile308Name[8];

extern const char ResFile307Name[8];

extern const char ResFile306Name[8];

extern const char ResFile305Name[8];

extern const char ResFile304Name[8];

extern const char ResFile025Name[8];

extern const char ResFile024Name[8];

extern const char ResFile022Name[8];

extern const char ResFile019Name[8];

extern const char ResFile018Name[8];

extern const char ResFile017Name[8];

extern const char ResFile016Name[8];

extern const char ResFile015Name[8];

extern const char ResFile014Name[8];

extern const char ResFile013Name[8];

extern const char ResFile009Name[8];

extern const char ResFile008Name[8];

extern const char ResFile007Name[8];

extern const char ResFile006Name[8];

extern const char ResFile005Name[8];

extern const char ResFile004Name[8];

extern const char ResFile029Name[8];

extern const char ResFile028Name[8];

extern const char ResFile027Name[8];

extern const char ResFile026Name[8];

extern const char ResFile179Name[8];

extern const char ResFile178Name[8];

extern const char ResFile177Name[8];

extern const char ResFile176Name[8];

extern const char ResFile175Name[8];

extern const char ResFile174Name[8];

extern const char ResFile169Name[8];

extern const char ResFile168Name[8];

extern const char ResFile167Name[8];

extern const char ResFile166Name[8];

extern const char ResFile165Name[8];

extern const char ResFile164Name[8];

extern const char ResFile159Name[8];

extern const char ResFile158Name[8];

extern const char ResFile157Name[8];

extern const char ResFile156Name[8];

extern const char ResFile155Name[8];

extern const char ResFile154Name[8];

extern const char ResFile149Name[8];

extern const char ResFile148Name[8];

extern const char ResFile147Name[8];

extern const char ResFile146Name[8];

extern const char ResFile145Name[8];

extern const char ResFile144Name[8];

extern const char ResFile139Name[8];

extern const char ResFile138Name[8];

extern const char ResFile137Name[8];

extern const char ResFile136Name[8];

extern const char ResFile135Name[8];

extern const char ResFile134Name[8];

extern const char ResFile129Name[8];

extern const char ResFile128Name[8];

extern const char ResFile127Name[8];

extern const char ResFile126Name[8];

extern const char ResFile125Name[8];

extern const char ResFile124Name[8];

extern const char ResFile119Name[8];

extern const char ResFile118Name[8];

extern const char ResFile117Name[8];

extern const char ResFile116Name[8];

extern const char ResFile115Name[8];

extern const char ResFile109Name[8];

extern const char ResFile108Name[8];

extern const char ResFile107Name[8];

extern const char ResFile106Name[8];

extern const char ResFile105Name[8];

extern const char ResFile104Name[8];

extern const char ResFile099Name[8];

extern const char ResFile098Name[8];

extern const char ResFile097Name[8];

extern const char ResFile096Name[8];

extern const char ResFile095Name[8];

extern const char ResFile094Name[8];

extern const char ResFile091Name[8];

extern const char ResFile089Name[8];

extern const char ResFile088Name[8];

extern const char ResFile087Name[8];

extern const char ResFile086Name[8];

extern const char ResFile085Name[8];

extern const char ResFile084Name[8];

extern const char ResFile083Name[8];

extern const char ResFile079Name[8];

extern const char ResFile078Name[8];

extern const char ResFile077Name[8];

extern const char ResFile076Name[8];

extern const char ResFile075Name[8];

extern const char ResFile074Name[8];

extern const char ResFile069Name[8];

extern const char ResFile068Name[8];

extern const char ResFile067Name[8];

extern const char ResFile066Name[8];

extern const char ResFile065Name[8];

extern const char ResFile064Name[8];

extern const char ResFile059Name[8];

extern const char ResFile058Name[8];

extern const char ResFile057Name[8];

extern const char ResFile056Name[8];

extern const char ResFile055Name[8];

extern const char ResFile054Name[8];

extern const char ResFile053Name[8];

extern const char ResFile049Name[8];

extern const char ResFile048Name[8];

extern const char ResFile047Name[8];

extern const char ResFile046Name[8];

extern const char ResFile045Name[8];

extern const char ResFile044Name[8];

extern const char ResFile040Name[8];

extern const char ResFile039Name[8];

extern const char ResFile038Name[8];

extern const char ResFile037Name[8];

extern const char ResFile036Name[8];

extern const char ResFile035Name[8];

extern const char ResFile034Name[8];

extern const char ResFile000Name[8];

extern const char ResAgName[8];

extern const char ResSeparatorName[8];

typedef struct PlayerPadView {
    u8 unmodeled_00[0x28];
    u16 held;
    u8 unmodeled_2a[0x66 - 0x2a];
    signed char stick_x;
    signed char stick_y;
} PlayerPadView;

typedef struct CameraDefinition {
    u8 unmodeled_00[0x34];
    float yaw;
} CameraDefinition;

extern PlayerPadView PadData;

extern CameraDefinition CfCameraDefine;

extern Vector4 EnemyTarget;

extern short EfTab_Code[15];

extern short EfTab_Intr[15];

extern short EfTab_Loop[15];

int Check_EnemyBurn(void);

int Check_EnemyElec(void);

int Check_EnemyFound(void);

int xglSoundEffectCheckID(int id, int arg);

void xglSoundEffectStopID(int id, int arg);

void sefDeleteEffectCf(void *effect);

PlayerEffect *sefCreateEffectCf(int code, int a, int b);

int Ladder_Main(LookAtPlayerActor *player_actor);

int SCRIPT_getCfTime(void);

float I2F(int value);

float atan2f(float y, float x);

float floorf(float value);

float xglSin(float angle);

void xglMatrixStackRotY(float angle);

float xglCos(float angle);

int Get_Attr(const Vector4 *position, int map_index, int attr_mask);

int Get_EffectCode(int attribute);

void ACT_setMotion2(LookAtPlayerActor *player_actor, int motion, int blend);

void Enemy_FindByEar(LookAtPlayerActor *player_actor, const Vector4 *position);

void HitCheckMapUnitWithNyuru(LookAtPlayerActor *player_actor);

static short HitCheckActor(LookAtPlayerActor *player_actor);

static int NyuruActor(LookAtPlayerActor *player_actor, LookAtPlayerActor *other);

float UnduCheck(const Vector4 *position, void *velocity, ActorUndulation *undulation);

int HitCheckMapUnit(LookAtPlayerActor *player_actor);

float Get_Cursol_by_Reduce_Speed_Angle_Loop(float cursor, float target, float rate);

void ShootMapUnit(void);

void ACT_updateMotion(LookAtPlayerActor *player_actor);

void GameIdLightSet(LookAtPlayerActor *player_actor, int arg);

void Set_Shadow(LookAtPlayerActor *player_actor);

void Get_EnemyIDPos(const Vector4 *position, Vector4 *target);

void Check_Locater(LookAtPlayerActor *player_actor);

#include "main/get.h"

int CheckActorExist(LookAtPlayerActor *target);

float Get_Angle(const Point4 *first, const Point4 *second);

/* HitCheckActor's minimum collision radius and maximum vertical separation
 * thresholds. These literal-pool values are read once for each actor scan. */
#define HIT_CHECK_MIN_BODY_RADIUS (0.0099999997764825821f)

#define HIT_CHECK_MAX_HEIGHT_DIFFERENCE (1.8999999761581421f)

#define NYURU_REACH_MARGIN (0.0099999997764825821f)

void LookAt_Player(LookAtPlayerActor *player_actor)
{
    int nearest_actor_index;
    signed char look_at_target;

    nearest_actor_index = Get_MostNear_Actor(player_actor);
    if (player_actor->look_at_timer == 2) {
        Actor_LookAt(player_actor);
        return;
    }
    if (GameLoopState.flags & 0x8000) {
        Actor_LookAt_Release(player_actor, 0);
        return;
    }
    look_at_target = GameLoopState.look_at_target;
    if (look_at_target != -1) {
        PlayerLookAtAim();
        return;
    }

    Actor_LookAt_Release(player_actor, 0);
    if (nearest_actor_index != look_at_target &&
        !(actor[nearest_actor_index].flags & 8)) {
        actor[player_actor->number].look_at_target = nearest_actor_index;
        Actor_LookAt_Set(player_actor, 1, &actor[nearest_actor_index].position);
        Actor_LookAt(player_actor);
        return;
    }
    Actor_LookAt_Release(player_actor, 1);
}

void Actor_LookAt_Init(LookAtPlayerActor *player_actor);
extern void UnduParamInit(void *undulation);

void Player_System_Init(LookAtPlayerActor *player_actor)
{
    SequenceState *sequence;
    PlayerEnemyWork *enemy_work;
    LookAtPlayerActor *player;
    int i;

    enemy_work = &enepc[player_actor->number];
    player_actor->signal = 0;
    player_actor->java_object = 0;
    player_actor->locator_flags = LOCATOR_FLAGS_PLAYER_INIT;
    player_actor->frame_counter = 0;
    player_actor->scale.x = player_actor->scale.y = player_actor->scale.z = player_actor->scale.w = 1.0f;
    sequence = &actSequence[player_actor->number].state;
    sequence->flags = 0;
    sequence->state_flags = 0;
    sequence->cleared_on_init = 0;
    sequence->cleared_on_init_run[0] = 0;
    sequence->cleared_on_init_run[1] = 0;
    sequence->cleared_on_init_run[2] = 0;
    sequence->cleared_on_init_run[3] = 0;
    sequence->handler[0] = 0;
    sequence->handler[1] = 0;
    sequence->handler[2] = 0;
    sequence->handler[3] = 0;
    UnduParamInit(&player_actor->undulation);
    player_actor->undulation.attr_mask = 0x1910;
    for (i = 7; i >= 0; i--) {
        player_actor->effects[i] = 0;
    }
    player_actor->effect_timer = -1;
    player_actor->effect_slot = 0;
    if (GameLoopState.effect != 0) {
        sefDeleteEffectCf(GameLoopState.effect);
    }
    GameLoopState.effect = 0;
    player = GameLoopState.player_actor;
    player->flash_timer = 0;
    enemy_work->encount_lock = 0;
    for (i = 0; i < 16; i++) {
        GameLoopState.ladder_direction[i] = 0;
        GameLoopState.ladder_height[i] = 0;
        GameLoopState.ladder_bottom_exit[i] = 1;
    }
    Actor_LookAt_Init(player_actor);
}

void GameCfPlayerMoveParamSet(float walkThreshold, float runThreshold, float vectorRate)
{
    int walkThresholdInt;
    int runThresholdInt;

    walkThresholdInt = F2I(walkThreshold);
    WALK_THRESHOLD_F = walkThreshold;
    WALK_THRESHOLD_I = walkThresholdInt;
    runThresholdInt = F2I(runThreshold);
    RUN_THRESHOLD_F = runThreshold;
    VECTOR_RATE = vectorRate;
    RUN_THRESHOLD_I = runThresholdInt;
}

void GameCfPlayerMoveInit(void)
{
    WALK_THRESHOLD_I = 32;
    WALK_THRESHOLD_F = 32.0f;
    RUN_THRESHOLD_I = 96;
    RUN_THRESHOLD_F = 96.0f;
    VECTOR_RATE = DEFAULT_VECTOR_RATE;
}

void GameCfPlayerMove(void)
{
    LookAtPlayerActor *player = GameLoopState.player_actor;
    float run_scale = 1.0f;
    float speed;
    int stick_speed;
    int terrain_bits;
    int ground_effect;
    int hit;
    PlayerAimVector position_before_move;
    const Vector4 *before_move;

    if (Check_EnemyBurn() == 0 && xglSoundEffectCheckID(0x10009, 0) != 0) {
        xglSoundEffectStopID(0x10009, 0);
    }
    if (Check_EnemyElec() == 0 && xglSoundEffectCheckID(0x10008, 0) != 0) {
        xglSoundEffectStopID(0x10008, 0);
    }
    if (Check_EnemyFound() == 0) {
        if (GameLoopState.effect != 0) {
            sefDeleteEffectCf(GameLoopState.effect);
        }
        GameLoopState.effect = 0;
    }
    if (player->flash_timer != 0) {
        if (--player->flash_timer <= 0) {
            player->flash_timer = 0;
            player->flags &= ~0x800;
        } else {
            unsigned short phase = player->flash_timer;

            if (!(phase & 1) || (short)phase >= 19) {
                player->flags |= 0x800;
                player->render_flags |= 3;
                player->flash_frames = 0x48;
                player->filter_param = 0.5f;
            } else {
                player->flags &= ~0x800;
            }
        }
    }
    if (Ladder_Main(player) == 1) {
        player->flags &= ~0x20;
        return;
    }
    speed = 0.0f;
    stick_speed = 0;
    player->flags |= 0x20;
    player->external_velocity.y = -0.05f;
    player->previous_position.x = player->position.x;
    player->previous_position.y = player->position.y;
    player->previous_position.z = player->position.z;
    player->velocity.x = 0.0f;
    player->velocity.z = 0.0f;
    player->external_velocity.x = 0.0f;
    player->external_velocity.z = 0.0f;
    if (!(GameLoopState.flags & 0x8000) && SCRIPT_getCfTime() >= 3) {
        signed char stick_x = PadData.stick_x;
        signed char stick_y = PadData.stick_y;
        float length;
        float angle;

        if (PadData.held & 0xa000) {
            stick_x = (PadData.held & 0x8000) ? -127 : 127;
        }
        if (PadData.held & 0x5000) {
            stick_y = (PadData.held & 0x1000) ? -127 : 127;
        }
        if (GameLoopState.flags & 0x100000) {
            stick_x = player->stick_x;
            stick_y = player->stick_y;
        } else {
            player->stick_x = stick_x;
            player->stick_y = stick_y;
        }
        if (GameLoopState.move_mode != 2) {
            float x = I2F(stick_x);
            float y = I2F(stick_y);

            length = __builtin_sqrtf(x * x + y * y);
            angle = floorf((atan2f(x, y) + 0.19634955f) / 0.3926991f) * 0.3926991f
                + CfCameraDefine.yaw;
        } else {
            length = -I2F(stick_y);
            angle = player->rotation_y - I2F(stick_x) * 0.001f;
        }
        if (128.0f <= length) {
            length = 128.0f;
        }
        if (WALK_THRESHOLD_F <= length) {
            player->aim_angle = angle;
            if (player->terrain_flags & 0x10) {
                if (RUN_THRESHOLD_F <= length) {
                    length = RUN_THRESHOLD_F - 0.1f;
                }
            }
            if (PadData.held & 2) {
                length = (RUN_THRESHOLD_F - 0.1f) / 1.5f;
            }
            stick_speed = F2I(length);
            speed = length * VECTOR_RATE;
            terrain_bits = (int)(player->terrain_flags & 0x3f000000);
            if (!(terrain_bits & 0x0f000000)) {
                static float rate[4] = { 1.0f, 2.0f, 0.5f, 3.0f };

                speed *= rate[(u32)terrain_bits >> 28];
            }
        } else {
            stick_speed = 0;
            speed = 0.0f;
        }
        player->move_speed = speed;
        player->velocity.x += xglSin(player->aim_angle) * speed;
        player->velocity.y += 0.0f;
        player->velocity.z += xglCos(player->aim_angle) * speed;
        {
            static float rate[4] = { 1.0f, 2.0f, 0.5f, 3.0f };
            static u8 idx[16] = { 0, 1, 3, 2, 5, 0, 4, 0, 7, 8, 0, 0, 6, 0, 0, 0 };
            static float vec[9][2] = {
                { 0.0f, 0.0f },  { 0.0f, 1.0f },  { 1.0f, 1.0f },
                { 1.0f, 0.0f },  { 1.0f, -1.0f }, { 0.0f, -1.0f },
                { -1.0f, -1.0f }, { -1.0f, 0.0f }, { -1.0f, 1.0f },
            };

            terrain_bits = (int)(player->terrain_flags & 0x3f000000);
            if (terrain_bits & 0x0f000000) {
                float scale = VECTOR_RATE * 128.0f * rate[(u32)terrain_bits >> 28];
                float *direction = vec[idx[((u32)terrain_bits & 0x0f000000) >> 24]];

                player->velocity.x += scale * direction[0];
                player->velocity.z += scale * direction[1];
            }
            run_scale = rate[(u32)terrain_bits >> 28];
        }
    }
    if (!(GameLoopState.flags & 0x4000)) {
        int abs_speed = stick_speed;

        if (abs_speed < 0) {
            abs_speed = -abs_speed;
        }

        if (abs_speed < WALK_THRESHOLD_I) {
            if (player->motion_id != 0x1b) {
                ACT_setMotion2(player, 0, 9);
            }
            player->motion_speed = 0.033333335f;
        } else if (abs_speed < RUN_THRESHOLD_I) {
            ACT_setMotion2(player, 1, 9);
            player->motion_speed = speed * 0.5f / run_scale;
        } else {
            ACT_setMotion2(player, 3, 9);
            player->motion_speed = speed * 0.23333335f / run_scale;
            Enemy_FindByEar(player, &player->position);
        }
    }
    speed = __builtin_fabsf(speed);
    ground_effect = Get_EffectCode(Get_Attr(&player->position, 0, 0));
    if (0.01f < speed || EfTab_Loop[ground_effect] == 1) {
        if (ground_effect != -1) {
            if (EfTab_Intr[ground_effect] != -1) {
                player->effect_timer++;
            }
            if ((EfTab_Intr[ground_effect] != -1 && player->effect_timer >= EfTab_Intr[ground_effect])
                || player->effect_timer == -1) {
                PlayerEffect *effect = sefCreateEffectCf(EfTab_Code[ground_effect], 0, 0);

                player->effects[player->effect_slot] = effect;
                if (effect != 0) {
                    effect->owner = player;
                    player->effect_timer = 0;
                    player->effect_slot = (player->effect_slot + 1) % 8;
                }
            }
        } else {
            player->effect_timer = ground_effect;
        }
    }
    player->velocity.x += player->external_velocity.x;
    player->velocity.y += player->external_velocity.y;
    player->velocity.z += player->external_velocity.z;
    __builtin_memcpy(&position_before_move, &player->position, sizeof(position_before_move));
    before_move = &position_before_move.vector;
    player->position.x += player->velocity.x;
    player->position.z += player->velocity.z;
    HitCheckMapUnitWithNyuru(player);
    hit = HitCheckActor(player);
    if (hit != -1) {
        NyuruActor(player, &actor[hit]);
    }
    player->velocity.x = player->position.x - before_move->x;
    player->velocity.z = player->position.z - before_move->z;
    player->undulation.attr_mask = (player->undulation.attr_mask & 0xfff8) | 2;
    player->position.x = before_move->x;
    player->position.z = before_move->z;
    {
        float floor_height = UnduCheck(&player->position, &player->velocity, &player->undulation);

        if (floor_height != -1000.0f && player->position.y < floor_height) {
            player->position.y = floor_height;
            player->velocity.y = 0.0f;
        }
    }
    if (HitCheckMapUnit(player) != -1) {
        __builtin_memcpy(&player->position, &position_before_move, sizeof(position_before_move));
    }
    player->rotation_y = Get_Cursol_by_Reduce_Speed_Angle_Loop(player->rotation_y, player->aim_angle, 4.0f);
    xglMatrixStackUnit();
    xglMatrixStackTrans(&player->position.x);
    xglMatrixStackRotZ(player->rotation_z);
    xglMatrixStackRotY(player->rotation_y);
    xglMatrixStackRotX(player->rotation_x);
    if (GameLoopState.map_flags & 1) {
        ShootMapUnit();
    }
    LookAt_Player(player);
    ACT_updateMotion(player);
    GameIdLightSet(player, 0);
    Set_Shadow(player);
    Get_EnemyIDPos(&player->position, &EnemyTarget);
    if (!(GameLoopState.flags & 0x8000)) {
        Check_Locater(player);
    }
}

static short HitCheckActor(LookAtPlayerActor *player_actor)
{
    short i;
    float dx;
    float dz;
    float distance;
    float minimum_body_radius;
    float maximum_height_difference;
    Vector4 *pos;

    minimum_body_radius = HIT_CHECK_MIN_BODY_RADIUS;
    maximum_height_difference = HIT_CHECK_MAX_HEIGHT_DIFFERENCE;
    for (i = 0; i < 64; i++) {
        if (!CheckActorExist(&actor[i])) {
            continue;
        }
        if (player_actor->number == i) {
            continue;
        }
        pos = &actor[i].position;
        if (actor[i].body_radius < minimum_body_radius) {
            continue;
        }
        if (maximum_height_difference < __builtin_fabsf(player_actor->position.y - pos->y)) {
            continue;
        }
        dx = player_actor->position.x - pos->x;
        dz = player_actor->position.z - pos->z;
        distance = __builtin_sqrtf(dx * dx + dz * dz);
        if (distance <= player_actor->body_radius + actor[i].body_radius) {
            return i;
        }
    }
    return -1;
}

static int NyuruActor(LookAtPlayerActor *player_actor, LookAtPlayerActor *other)
{
    float angle;
    float reach;
    Vector4 contact;
    int hit;

    angle = Get_Angle((const Point4 *)&player_actor->position,
                      (const Point4 *)&other->position);
    player_actor->position.x -= player_actor->velocity.x;
    player_actor->position.z -= player_actor->velocity.z;
    reach = player_actor->body_radius + other->body_radius + NYURU_REACH_MARGIN;
    contact.x = other->position.x - reach * xglSin(angle);
    contact.z = other->position.z - reach * xglCos(angle);
    player_actor->velocity.x = contact.x - player_actor->position.x;
    player_actor->velocity.z = contact.z - player_actor->position.z;
    player_actor->position.x += player_actor->velocity.x;
    player_actor->position.z += player_actor->velocity.z;
    hit = HitCheckActor(player_actor);
    if (hit != -1) {
        player_actor->position.x -= player_actor->velocity.x;
        player_actor->position.z -= player_actor->velocity.z;
        return 0;
    }
    if (HitCheckMapUnit(player_actor) != hit) {
        player_actor->position.x -= player_actor->velocity.x;
        player_actor->position.z -= player_actor->velocity.z;
        return 0;
    }
    return 1;
}

void PlayerLookAtAim(void)
{
    int look_at_target;
    void *player_actor;
    PlayerAimVector target_position;

    look_at_target = GameLoopState.look_at_target;
    player_actor = GameLoopState.player_actor;
    target_position = MapUnit[look_at_target].position;
    target_position.vector.y += 1.0f;
    Actor_LookAt_Set(player_actor, 0, &target_position.vector);
    Actor_LookAt(player_actor);
}

static u16 GameCfPlayerLoadResource_preCID;

static u8 GameCfPlayerLoadResource_preCurry;

void GameCfPlayerLoadResource(int use_cache)
{
    char path[256];
    char leader_se_name[64];
    char *name_end;
    unsigned short player_id;
    unsigned char curry;
    void *player;
    int header_0;
    int header_2;

    player_id = ((LookAtSavedPartyPrefix *)SaveData)->player_character_id;
    curry = 0;
    player = GameLoopState.player_actor;
    if (GameResource[1].address == 0) {
        GameResourceAlloc(0x12a000);
    } else if (GameResource[0].size != 0x12a000) {
        DEBUG_BREAK();
    }
    if (GameResource[2].address == 0) {
        GameResourceAlloc(0);
    }
    if (player_id == 1) {
        if (xglFlagsGet(0x19, 1) != 0) {
            if (xglFlagsGet(0x1a, 1) == 0) {
                player_id = 9;
            }
        }
        if (xglFlagsGet(0x73, 1) != 0) {
            curry = (xglFlagsGet(0x7b, 1) == 0);
        }
    }
    RES_GetLeaderSeName(leader_se_name);
    xglSoundSendEffect(0, 0, 2);
    xglSoundLoadEffect(leader_se_name, GameResourceGetFreeAddr(), 2);

    if (use_cache == 0) {
        header_0 = *(int *)0x01000000;
        if (header_0 == 0x78656c) {
            if (*(int *)0x01070800 == 0x585458) {
                header_2 = *(int *)0x010B1000;
                if (header_2 == 0x544e4a) {
                    if (*(int *)0x010B4000 == header_0) {
                        if (*(int *)0x010E6000 == header_2 &&
                            (*(int *)0x010E7000 & 0xffffff) == 0x4b5046 &&
                            GameCfPlayerLoadResource_preCID == player_id) {
                            if (GameCfPlayerLoadResource_preCurry == curry) {
                                return;
                            }
                        }
                    }
                }
            }
        }
    }

    GameCfPlayerLoadResource_preCID = player_id;
    GameCfPlayerLoadResource_preCurry = curry;
    RES_GetMdlFileName(path, player_id);
    name_end = path;
    if (path[0] != 0) {
        do {
            name_end++;
        } while (*name_end != 0);
    }
    __builtin_strcpy(name_end, ".lex");
    xglCdReadFile(path, (void *)0x01000000, 0, 1);
    __builtin_strcpy(name_end, ".xtx");
    xglCdReadFile(path, (void *)0x01070800, 0, 1);
    __builtin_strcpy(name_end, ".jnt");
    xglCdReadFile(path, (void *)0x010B1000, 0, 1);
    RES_GetMdlFileName(path, player_id | 0x10000);
    name_end = path;
    if (path[0] != 0) {
        do {
            name_end++;
        } while (*name_end != 0);
    }
    __builtin_strcpy(name_end, ".lex");
    xglCdReadFile(path, (void *)0x010B4000, 0, 1);
    __builtin_strcpy(name_end, ".jnt");
    xglCdReadFile(path, (void *)0x010E6000, 0, 1);
    if (curry == 0) {
        sprintf(path, "data\\motion\\%s.fpk", RES_GetMotBaseName(player_id));
    } else {
        __builtin_strcpy(path, "data\\motion\\shion_curry.fpk");
    }
    xglCdReadFile(path, (void *)0x010E7000, 0, 1);
    ACT_setMotion2(player, 0, 8);
    ACT_setModelWrapper(player, 0);
    ACT_setHumanHand(player, 0);
}

extern const char ResGuno307Name[16];

extern const char ResGuno325Name[16];

extern const char ResGuno324Name[16];

extern const char ResGuno323Name[16];

extern const char ResGuno305Name[16];

extern const char ResGuno304Name[16];

extern const char ResGuno303Name[16];

extern const char ResGuno302Name[16];

extern const char ResGuno301Name[16];

extern const char ResGuno001CfName[16];

extern const char ResYoungwCfName[16];

extern const char ResYoungmCfName[16];

extern const char ResWomanCfName[16];

extern const char ResSoldiermCfName[16];

extern const char ResSoldierwCfName[16];

extern const char ResMiddlewCfName[16];

extern const char ResMiddlemCfName[16];

extern const char ResAllenCfName[16];

extern const char ResShitanCfName[16];

extern const char ResZiggyCfName[16];

extern const char ResChaosCfName[16];

extern const char ResKosmosCfName[16];

extern const char ResShionCfName[16];

extern const char ResOldmCfName[8];

extern const char ResOldwCfName[8];

extern const char ResJrCfName[8];

extern const char ResMomoCfName[8];

extern const char ResSaveName[8];

extern const char ResDiskName[8];

extern const char ResNpcseName[8];

extern const char ResEneseName[8];

extern const char ResMpeg2Name[8];

extern const char ResLoadsmdName[8];

extern const char ResLoadseName[8];

extern const char ResDummyName[8];

extern const char ResPlayerName[8];

extern const char ResScriptName[8];

extern const char ResLoadeneName[8];

extern const char ResLoadtexName[8];

extern const char ResLoadmotName[8];

extern const char ResLoadactName[8];

extern const char ResLoadmapName[8];

extern const char ResResetName[8];

extern const char ResZ8Name[8];

extern const char ResJRName[8];

extern const char ResMOName[8];

extern const char ResCSName[8];

extern const char ResKMName[8];

extern const char ResSOName[8];

const char ResShion6HName[16] = "shion6_h";

const char ResShion5HShion1HName[24] = "+shion5_h,shion1_h";

const char ResShion4HShionHName[24] = "+shion4_h,shion_h";

const char ResShion3HName[16] = "shion3_h";

const char ResShion2HName[16] = "shion2_h";

const char ResShion1HShion5HName[24] = "+shion1_h,shion5_h";

const char ResKosmosH6Name[16] = "kosmos_h6";

const char ResKosmosH5Name[16] = "kosmos_h5";

const char ResKosmosH4Name[16] = "kosmos_h4";

const char ResKosmosH3Name[16] = "kosmos_h3";

const char ResKosmosH2Name[16] = "kosmos_h2";

const char ResKosmosH1Name[16] = "kosmos_h1";

const char ResShitanHName[16] = "shitan_h";

const char ResKosmosHName[16] = "kosmos_h";

const char ResShionHShion4HName[24] = "+shion_h,shion4_h";

const char ResShion5Shion1Name[16] = "+shion5,shion1";

const char ResShion1Shion5Name[16] = "+shion1,shion5";

const char ResShionChName[16] = "shion_ch";

const char ResShionShion4Name[16] = "+shion,shion4";

const char ResAlbelt3HName[16] = "albelt3_h";

const char ResHammerHHammerName[24] = "+hammer_h,hammer";

const char ResTonnyHTonnyName[16] = "+tonny_h,tonny";

const char ResGaignunHName[16] = "gaignun_h";

const char ResAndrewHAndrew6Name[24] = "+andrew_h,andrew6";

const char ResAndrewHAndrew5Name[24] = "+andrew_h,andrew5";

const char ResAndrewHAndrew4Name[24] = "+andrew_h,andrew4";

const char ResAndrewHAndrew3Name[24] = "+andrew_h,andrew3";

const char ResAndrewHAndrew2Name[24] = "+andrew_h,andrew2";

const char ResAndrewHAndrew1Name[24] = "+andrew_h,andrew1";

const char ResAndrewHAndrewName[24] = "+andrew_h,andrew";

const char ResVirgil3HName[16] = "virgil3_h";

const char ResVirgil1HName[16] = "virgil1_h";

const char ResAlbelt2HName[16] = "albelt2_h";

const char ResVirgil4HName[16] = "virgil4_h";

const char ResVirgil2HName[16] = "virgil2_h";

const char ResKebin1HName[16] = "kebin1_h";

const char ResMatehwsHMatehwsName[24] = "+matehws_h,matehws";

const char ResVirgilHName[16] = "virgil_h";

const char ResAlbeltHName[16] = "albelt_h";

const char ResMarglisHName[16] = "marglis_h";

const char ResJoachim1Name[16] = "joachim1";

const char ResCecilia1Name[16] = "cecilia1";

const char ResAndDaugName[16] = "and_daug";

const char ResAndWifeName[16] = "and_wife";

const char ResMoriyamaName[16] = "moriyama";

const char ResStepMamName[16] = "step_mam";

const char ResMatehws1Name[16] = "matehws1";

const char ResGaignun1Name[16] = "gaignun1";

const char ResMarglis1Name[16] = "marglis1";

const char ResBrealW1Name[16] = "breal_w1";

const char ResBrealM4Name[16] = "breal_m4";

const char ResBrealM3Name[16] = "breal_m3";

const char ResBrealM2Name[16] = "breal_m2";

const char ResBrealM1Name[16] = "breal_m1";

const char ResKingCatName[16] = "king_cat";

const char ResPilotNoName[16] = "pilot_no";

const char ResInnGirlName[16] = "inn_girl";

const char ResBreadChName[16] = "bread_ch";

const char ResKidswB2Name[16] = "kidsw_b2";

const char ResKidswB1Name[16] = "kidsw_b1";

const char ResKidswA2Name[16] = "kidsw_a2";

const char ResKidswA1Name[16] = "kidsw_a1";

const char ResKidsmB2Name[16] = "kidsm_b2";

const char ResKidsmB1Name[16] = "kidsm_b1";

const char ResKidsmA2Name[16] = "kidsm_a2";

const char ResKidsmA1Name[16] = "kidsm_a1";

const char ResGrFinalName[16] = "gr_final";

const char ResEwpHg01Name[16] = "ewp_hg01";

const char ResBossBossPath[16] = "boss\\boss_";

const char ResFereFerePath[16] = "fere\\fere_";

const char ResFesoFesoPath[16] = "feso\\feso_";

const char ResFeroFeroPath[16] = "fero\\fero_";

const char ResFemaFemaPath[16] = "fema\\fema_";

const char ResUtroUtroPath[16] = "utro\\utro_";

const char ResUtsoUtsoPath[16] = "utso\\utso_";

const char ResUtreUtrePath[16] = "utre\\utre_";

const char ResUtmaUtmaPath[16] = "utma\\utma_";

const char ResGunoGunoPath[16] = "guno\\guno_";

const char ResMobj000MobjPath[16] = "mobj000\\mobj_";

const char ResAobj000AccPath[16] = "aobj000\\acc_";

const char ResMusobjMusPath[16] = "musobj\\mus_";

const char ResMufobjMufPath[16] = "mufobj\\muf_";

const char ResMuxobjMuxPath[16] = "muxobj\\mux_";

const char ResTreobjTrePath[16] = "treobj\\tre_";

const char ResMuwobjMuwPath[16] = "muwobj\\muw_";

const char ResMuobjMUPath[16] = "muobj\\MU_";

const char ResGuno307Name[16] = "guno_307";

const char ResGuno325Name[16] = "guno_325";

const char ResGuno324Name[16] = "guno_324";

const char ResGuno323Name[16] = "guno_323";

const char ResGuno305Name[16] = "guno_305";

const char ResGuno304Name[16] = "guno_304";

const char ResGuno303Name[16] = "guno_303";

const char ResGuno302Name[16] = "guno_302";

const char ResGuno301Name[16] = "guno_301";

const char ResGuno001CfName[16] = "guno001_cf";

const char ResYoungwCfName[16] = "youngw_cf";

const char ResYoungmCfName[16] = "youngm_cf";

const char ResWomanCfName[16] = "woman_cf";

const char ResSoldiermCfName[16] = "soldierm_cf";

const char ResSoldierwCfName[16] = "soldierw_cf";

const char ResMiddlewCfName[16] = "middlew_cf";

const char ResMiddlemCfName[16] = "middlem_cf";

const char ResAllenCfName[16] = "allen_cf";

const char ResShitanCfName[16] = "shitan_cf";

const char ResZiggyCfName[16] = "ziggy_cf";

const char ResChaosCfName[16] = "chaos_cf";

const char ResKosmosCfName[16] = "kosmos_cf";

const char ResShionCfName[16] = "shion_cf";

const char ResDataSceneItaiPath[24] = "data\\scene\\itai\\";

const char ResDataSceneYagiPath[24] = "data\\scene\\yagi\\";

const char ResDataSceneSugisawaPath[24] = "data\\scene\\sugisawa\\";

const char ResDataSceneSatoPath[24] = "data\\scene\\sato\\";

const char ResDataSceneKeiichiPath[24] = "data\\scene\\keiichi\\";

const char ResDataSceneKuramotoPath[24] = "data\\scene\\kuramoto\\";

const char ResDataSceneYajimaPath[24] = "data\\scene\\yajima\\";

const char ResDataSceneNakaharaPath[24] = "data\\scene\\nakahara\\";

const char ResDataSceneNvYonePath[24] = "data\\scene\\nv-yone\\";

const char ResDataSceneKojiPath[24] = "data\\scene\\koji\\";

const char ResDataSceneKonishiPath[24] = "data\\scene\\konishi\\";

const char ResDataSceneSakisakoPath[24] = "data\\scene\\sakisako\\";

const char ResDataSceneKojimaPath[24] = "data\\scene\\kojima\\";

const char ResDataSceneFujiPath[24] = "data\\scene\\fuji\\";

const char ResDataSceneGashPath[24] = "data\\scene\\gash\\";

const char ResDataSceneEventPath[24] = "data\\scene\\event\\";

const char ResDataSceneCfPath[16] = "data\\scene\\cf\\";

const char ResDataSceneCheckPath[24] = "data\\scene\\check\\";

const char ResDataSceneTestPath[24] = "data\\scene\\test\\";

const char ResDataScenePath[16] = "data\\scene\\";

const char ResMomo6Name[8] = "momo6";

const char ResMomo5HName[8] = "momo5_h";

const char ResMomo5Name[8] = "momo5";

const char ResJr4Name[8] = "jr4";

const char ResJr1HName[8] = "jr1_h";

const char ResZiggyHName[8] = "ziggy_h";

const char ResJrHName[8] = "jr_h";

const char ResMomoHName[8] = "momo_h";

const char ResChaosHName[8] = "chaos_h";

const char ResShitan2Name[8] = "shitan2";

const char ResShitan1Name[8] = "shitan1";

const char ResZiggy1Name[8] = "ziggy1";

const char ResJr3Name[8] = "jr3";

const char ResJr2Name[8] = "jr2";

const char ResJr1Name[8] = "jr1";

const char ResMomo4Name[8] = "momo4";

const char ResMomo3Name[8] = "momo3";

const char ResMomo2Name[8] = "momo2";

const char ResMomo1Name[8] = "momo1";

const char ResChaos2Name[8] = "chaos2";

const char ResChaos1Name[8] = "chaos1";

const char ResKosmos5Name[8] = "kosmos5";

const char ResKosmos3Name[8] = "kosmos3";

const char ResKosmos2Name[8] = "kosmos2";

const char ResKosmos1Name[8] = "kosmos1";

const char ResShionName[8] = "shion";

const char ResShion3Name[8] = "shion3";

const char ResShion2Name[8] = "shion2";

const char ResShitanName[8] = "shitan";

const char ResZiggyName[8] = "ziggy";

const char ResJrName[8] = "jr";

const char ResMomoName[8] = "momo";

const char ResChaosName[8] = "chaos";

const char ResKosmosName[8] = "kosmos";

const char ResAndrew9Name[8] = "andrew9";

const char ResFeb1Name[8] = "feb1";

const char ResAndrew8Name[8] = "andrew8";

const char ResAndrew7Name[8] = "andrew7";

const char ResAndrew6Name[8] = "andrew6";

const char ResAndrew5Name[8] = "andrew5";

const char ResEllyHName[8] = "elly_h";

const char ResFriedHName[8] = "fried_h";

const char ResKebinHName[8] = "kebin_h";

const char ResAllenHName[8] = "allen_h";

const char ResCath1Name[8] = "cath1";

const char ResChiefName[8] = "chief";

const char ResVanName[8] = "van";

const char ResElly2Name[8] = "elly2";

const char ResKJoachName[8] = "k_joach";

const char ResShalronName[8] = "shalron";

const char ResHynlinName[8] = "hynlin";

const char ResTogashiName[8] = "togashi";

const char ResPelegriName[8] = "pelegri";

const char ResShiMamName[8] = "shi_mam";

const char ResShiDadName[8] = "shi_dad";

const char ResCathName[8] = "cath";

const char ResCeciliaName[8] = "cecilia";

const char ResLapisName[8] = "lapis";

const char ResMiyukiName[8] = "miyuki";

const char ResVoyagerName[8] = "voyager";

const char ResHelmerName[8] = "helmer";

const char ResAbelName[8] = "abel";

const char ResShelleyName[8] = "shelley";

const char ResMaryName[8] = "mary";

const char ResSellersName[8] = "sellers";

const char ResJoachimName[8] = "joachim";

const char ResYuriName[8] = "yuri";

const char ResFebName[8] = "feb";

const char ResAndrew4Name[8] = "andrew4";

const char ResAndrew3Name[8] = "andrew3";

const char ResAndrew2Name[8] = "andrew2";

const char ResAndrew1Name[8] = "andrew1";

const char ResAndrewName[8] = "andrew";

const char ResHammerName[8] = "hammer";

const char ResTonnyName[8] = "tonny";

const char ResMatehwsName[8] = "matehws";

const char ResEllyName[8] = "elly";

const char ResFriedName[8] = "fried";

const char ResVirgil4Name[8] = "virgil4";

const char ResVirgil3Name[8] = "virgil3";

const char ResVirgil2Name[8] = "virgil2";

const char ResVirgil1Name[8] = "virgil1";

const char ResVirgilName[8] = "virgil";

const char ResKebin2Name[8] = "kebin2";

const char ResKebin1Name[8] = "kebin1";

const char ResKebinName[8] = "kebin";

const char ResAllen1Name[8] = "allen1";

const char ResAllenName[8] = "allen";

const char ResAlbelt1Name[8] = "albelt1";

const char ResAlbeltName[8] = "albelt";

const char ResGaignunName[8] = "gaignun";

const char ResMarglisName[8] = "marglis";

const char ResBrealWName[8] = "breal_w";

const char ResBrealMName[8] = "breal_m";

const char ResEsoldName[8] = "esold";

const char ResRwW2Name[8] = "rw_w2";

const char ResRwW1Name[8] = "rw_w1";

const char ResRwWName[8] = "rw_w";

const char ResRwM2Name[8] = "rw_m2";

const char ResRwM1Name[8] = "rw_m1";

const char ResRwMName[8] = "rw_m";

const char ResSuukiName[8] = "suuki";

const char ResEngOffName[8] = "eng_off";

const char ResEngName[8] = "eng";

const char ResSoldName[8] = "sold";

const char ResPilotName[8] = "pilot";

const char ResOffW1Name[8] = "off_w1";

const char ResOffWName[8] = "off_w";

const char ResOffM3Name[8] = "off_m3";

const char ResOffM2Name[8] = "off_m2";

const char ResOffM1Name[8] = "off_m1";

const char ResOffMName[8] = "off_m";

const char ResVecW1Name[8] = "vec_w1";

const char ResVecWName[8] = "vec_w";

const char ResVecM2Name[8] = "vec_m2";

const char ResVecM1Name[8] = "vec_m1";

const char ResVecMName[8] = "vec_m";

const char ResSpaceName[8] = "space";

const char ResUSoldsName[8] = "u_solds";

const char ResRealW2Name[8] = "real_w2";

const char ResRealW1Name[8] = "real_w1";

const char ResRealWName[8] = "real_w";

const char ResRealM2Name[8] = "real_m2";

const char ResRealM1Name[8] = "real_m1";

const char ResRealMName[8] = "real_m";

const char ResUSpaceName[8] = "u_space";

const char ResUSoldName[8] = "u_sold";

const char ResGoveW2Name[8] = "gove_w2";

const char ResGoveW1Name[8] = "gove_w1";

const char ResGoveWName[8] = "gove_w";

const char ResCommi5Name[8] = "commi5";

const char ResCommi4Name[8] = "commi4";

const char ResCommi3Name[8] = "commi3";

const char ResCommi2Name[8] = "commi2";

const char ResCommi1Name[8] = "commi1";

const char ResCommiName[8] = "commi";

const char ResAssM4Name[8] = "ass_m4";

const char ResAssM3Name[8] = "ass_m3";

const char ResKanchoName[8] = "kancho";

const char ResGoveM2Name[8] = "gove_m2";

const char ResGoveM1Name[8] = "gove_m1";

const char ResGoveMName[8] = "gove_m";

const char ResAssWName[8] = "ass_w";

const char ResAssM2Name[8] = "ass_m2";

const char ResAssM1Name[8] = "ass_m1";

const char ResAssMName[8] = "ass_m";

const char ResHyakuName[8] = "hyaku";

const char ResDu2Name[8] = "du2";

const char ResDu1Name[8] = "du1";

const char ResDuName[8] = "du";

const char ResPolWName[8] = "pol_w";

const char ResPolMName[8] = "pol_m";

const char ResUticW2Name[8] = "utic_w2";

const char ResUticW1Name[8] = "utic_w1";

const char ResUticWName[8] = "utic_w";

const char ResUticM2Name[8] = "utic_m2";

const char ResUticM1Name[8] = "utic_m1";

const char ResUticMName[8] = "utic_m";

const char ResContName[8] = "cont";

const char ResEngOjiName[8] = "eng_oji";

const char ResScotName[8] = "scot";

const char ResInnOjiName[8] = "inn_oji";

const char ResSpyName[8] = "spy";

const char ResBartenName[8] = "barten";

const char ResGoroName[8] = "goro";

const char ResMouthName[8] = "mouth";

const char ResCleObaName[8] = "cle_oba";

const char ResKingName[8] = "king";

const char ResRobotName[8] = "robot";

const char ResCleChName[8] = "cle_ch";

const char ResNurseName[8] = "nurse";

const char ResDoctorName[8] = "doctor";

const char ResBarMName[8] = "bar_m";

const char ResBarOjiName[8] = "bar_oji";

const char ResBreadName[8] = "bread";

const char ResGirlB2Name[8] = "girl_b2";

const char ResGirlB1Name[8] = "girl_b1";

const char ResGirlBName[8] = "girl_b";

const char ResGirlA2Name[8] = "girl_a2";

const char ResGirlA1Name[8] = "girl_a1";

const char ResGirlAName[8] = "girl_a";

const char ResKidswBName[8] = "kidsw_b";

const char ResKidswAName[8] = "kidsw_a";

const char ResBabaB2Name[8] = "baba_b2";

const char ResBabaB1Name[8] = "baba_b1";

const char ResBabaBName[8] = "baba_b";

const char ResBabaA2Name[8] = "baba_a2";

const char ResBabaA1Name[8] = "baba_a1";

const char ResBabaAName[8] = "baba_a";

const char ResObaB2Name[8] = "oba_b2";

const char ResObaB1Name[8] = "oba_b1";

const char ResObaBName[8] = "oba_b";

const char ResObaA2Name[8] = "oba_a2";

const char ResObaA1Name[8] = "oba_a1";

const char ResObaAName[8] = "oba_a";

const char ResWmanB2Name[8] = "wman_b2";

const char ResWmanB1Name[8] = "wman_b1";

const char ResWmanBName[8] = "wman_b";

const char ResWmanA2Name[8] = "wman_a2";

const char ResWmanA1Name[8] = "wman_a1";

const char ResWmanAName[8] = "wman_a";

const char ResBoyB2Name[8] = "boy_b2";

const char ResBoyB1Name[8] = "boy_b1";

const char ResBoyBName[8] = "boy_b";

const char ResBoyA2Name[8] = "boy_a2";

const char ResBoyA1Name[8] = "boy_a1";

const char ResBoyAName[8] = "boy_a";

const char ResKidsmBName[8] = "kidsm_b";

const char ResKidsmAName[8] = "kidsm_a";

const char ResJijiB2Name[8] = "jiji_b2";

const char ResJijiB1Name[8] = "jiji_b1";

const char ResJijiBName[8] = "jiji_b";

const char ResJijiA2Name[8] = "jiji_a2";

const char ResJijiA1Name[8] = "jiji_a1";

const char ResJijiAName[8] = "jiji_a";

const char ResOjiB2Name[8] = "oji_b2";

const char ResOjiB1Name[8] = "oji_b1";

const char ResOjiBName[8] = "oji_b";

const char ResOjiA2Name[8] = "oji_a2";

const char ResOjiA1Name[8] = "oji_a1";

const char ResOjiAName[8] = "oji_a";

const char ResManB2Name[8] = "man_b2";

const char ResManB1Name[8] = "man_b1";

const char ResManBName[8] = "man_b";

const char ResManA2Name[8] = "man_a2";

const char ResManA1Name[8] = "man_a1";

const char ResManAName[8] = "man_a";

const char ResFile06Name[8] = "06";

const char ResFile05Name[8] = "05";

const char ResFile04Name[8] = "04";

const char ResFile03Name[8] = "03";

const char ResFile02Name[8] = "02";

const char ResFile01Name[8] = "01";

const char ResFile09Name[8] = "_09";

const char ResFile08Name[8] = "_08";

const char ResFile07Name[8] = "_07";

const char ResSuffix06Name[8] = "_06";

const char ResFile02HoName[8] = "_02_ho";

const char ResFile01Ho2Name[8] = "_01_ho2";

const char ResFile01HoName[8] = "_01_ho";

const char ResFile01CvName[8] = "_01_cv";

const char ResSuffix05Name[8] = "_05";

const char ResSuffix04Name[8] = "_04";

const char ResSuffix03Name[8] = "_03";

const char ResSuffix02Name[8] = "_02";

const char ResSuffix01Name[8] = "_01";

const char ResGrLegName[8] = "gr_leg";

const char ResGrBodyName[8] = "gr_body";

const char ResGrHandName[8] = "gr_hand";

const char ResGrHeadName[8] = "gr_head";

const char ResUkunSName[8] = "ukunS";

const char ResUkunBName[8] = "ukunB";

const char ResTs01Name[8] = "ts_01";

const char ResVx1MName[8] = "vx_1m";

const char ResUknGName[8] = "ukn_g";

const char ResBw04Name[8] = "bw04";

const char ResBw03Name[8] = "bw03";

const char ResBw02Name[8] = "bw02";

const char ResBw01Name[8] = "bw01";

const char ResHw06Name[8] = "hw06";

const char ResHw05Name[8] = "hw05";

const char ResHw04Name[8] = "hw04";

const char ResHw03Name[8] = "hw03";

const char ResHw02Name[8] = "hw02";

const char ResHw01Name[8] = "hw01";

const char ResRd17Name[8] = "rd17";

const char ResRd16Name[8] = "rd16";

const char ResRd15Name[8] = "rd15";

const char ResRd14Name[8] = "rd14";

const char ResRd13Name[8] = "rd13";

const char ResRd12Name[8] = "rd12";

const char ResRd11Name[8] = "rd11";

const char ResRd10Name[8] = "rd10";

const char ResRd09Name[8] = "rd09";

const char ResRd08Name[8] = "rd08";

const char ResRd07Name[8] = "rd07";

const char ResRd06Name[8] = "rd06";

const char ResRd05Name[8] = "rd05";

const char ResRd04Name[8] = "rd04";

const char ResRd03Name[8] = "rd03";

const char ResRd02Name[8] = "rd02";

const char ResRd01Name[8] = "rd01";

const char ResSw01Name[8] = "sw01";

const char ResJrHg08Name[8] = "jr_hg08";

const char ResJrHg07Name[8] = "jr_hg07";

const char ResJrHg06Name[8] = "jr_hg06";

const char ResGun05Name[8] = "gun05";

const char ResGun04Name[8] = "gun04";

const char ResGun03Name[8] = "gun03";

const char ResGun02Name[8] = "gun02";

const char ResGun01Name[8] = "gun01";

const char ResSw05Name[8] = "sw05";

const char ResSw04Name[8] = "sw04";

const char ResSw03Name[8] = "sw03";

const char ResSw02Name[8] = "sw02";

const char ResMboxName[8] = "mbox";

const char ResMsp02RName[8] = "msp02r";

const char ResHb00RName[8] = "hb00r";

const char ResGlg00RName[8] = "glg00r";

const char ResDeff00Name[8] = "deff00";

const char ResSol00Name[8] = "sol00";

const char ResWire00Name[8] = "wire00";

const char ResSmp01Name[8] = "smp01";

const char ResEcm02Name[8] = "ecm02";

const char ResEcm01Name[8] = "ecm01";

const char ResEad00Name[8] = "ead00";

const char ResBw00Name[8] = "bw00";

const char ResBbc00Name[8] = "bbc00";

const char ResVsd00Name[8] = "vsd00";

const char ResFsd00Name[8] = "fsd00";

const char ResShb00Name[8] = "shb00";

const char ResMsp03Name[8] = "msp03";

const char ResMsp01Name[8] = "msp01";

const char ResAmp02Name[8] = "amp02";

const char ResLgc00Name[8] = "lgc00";

const char ResBp00Name[8] = "bp00";

const char ResEac00Name[8] = "eac00";

const char ResCb00Name[8] = "cb00";

const char ResBl00Name[8] = "bl00";

const char ResMsp02Name[8] = "msp02";

const char ResMsp00Name[8] = "msp00";

const char ResHb00Name[8] = "hb00";

const char ResGlg00Name[8] = "glg00";

const char ResGnd00Name[8] = "gnd00";

const char ResFl00Name[8] = "fl00";

const char ResPbk00Name[8] = "pbk00";

const char ResLsr00Name[8] = "lsr00";

const char ResDrl01Name[8] = "drl01";

const char ResDrl00Name[8] = "drl00";

const char ResBs00Name[8] = "bs00";

const char ResBam00Name[8] = "bam00";

const char ResRf02Name[8] = "rf02";

const char ResRf01Name[8] = "rf01";

const char ResRf00Name[8] = "rf00";

const char ResMg01Name[8] = "mg01";

const char ResMg00Name[8] = "mg00";

const char ResHmr02Name[8] = "hmr02";

const char ResDlc00Name[8] = "dlc00";

const char ResHmr00Name[8] = "hmr00";

const char ResHg01Name[8] = "hg01";

const char ResHg00Name[8] = "hg00";

const char ResWct00Name[8] = "wct00";

const char ResBd01Name[8] = "bd01";

const char ResBd00Name[8] = "bd00";

const char ResAx00Name[8] = "ax00";

const char ResAcc036Name[8] = "acc036";

const char ResAcc035Name[8] = "acc035";

const char ResAcc034Name[8] = "acc034";

const char ResAcc033Name[8] = "acc033";

const char ResAcc032Name[8] = "acc032";

const char ResAcc031Name[8] = "acc031";

const char ResAcc030Name[8] = "acc030";

const char ResAcc029Name[8] = "acc029";

const char ResAcc028Name[8] = "acc028";

const char ResAcc018Name[8] = "acc018";

const char ResWingName[8] = "wing";

const char ResFile114Name[8] = "114";

const char ResEmptyName[8] = "";

const char ResFile325Name[8] = "325";

const char ResFile324Name[8] = "324";

const char ResFile323Name[8] = "323";

const char ResFile322Name[8] = "322";

const char ResFile321Name[8] = "321";

const char ResFile320Name[8] = "320";

const char ResFile319Name[8] = "319";

const char ResFile318Name[8] = "318";

const char ResFile317Name[8] = "317";

const char ResFile316Name[8] = "316";

const char ResFile315Name[8] = "315";

const char ResFile314Name[8] = "314";

const char ResFile313Name[8] = "313";

const char ResFile312Name[8] = "312";

const char ResFile311Name[8] = "311";

const char ResFile310Name[8] = "310";

const char ResFile309Name[8] = "309";

const char ResFile308Name[8] = "308";

const char ResFile307Name[8] = "307";

const char ResFile306Name[8] = "306";

const char ResFile305Name[8] = "305";

const char ResFile304Name[8] = "304";

const char ResFile303Name[8] = "303";

const char ResFile302Name[8] = "302";

const char ResFile301Name[8] = "301";

const char ResFile025Name[8] = "025";

const char ResFile024Name[8] = "024";

const char ResFile023Name[8] = "023";

const char ResFile022Name[8] = "022";

const char ResFile021Name[8] = "021";

const char ResFile020Name[8] = "020";

const char ResFile019Name[8] = "019";

const char ResFile018Name[8] = "018";

const char ResFile017Name[8] = "017";

const char ResFile016Name[8] = "016";

const char ResFile015Name[8] = "015";

const char ResFile014Name[8] = "014";

const char ResFile013Name[8] = "013";

const char ResFile012Name[8] = "012";

const char ResFile011Name[8] = "011";

const char ResFile010Name[8] = "010";

const char ResFile009Name[8] = "009";

const char ResFile008Name[8] = "008";

const char ResFile007Name[8] = "007";

const char ResFile006Name[8] = "006";

const char ResFile005Name[8] = "005";

const char ResFile004Name[8] = "004";

const char ResFile003Name[8] = "003";

const char ResFile002Name[8] = "002";

const char ResFile001Name[8] = "001";

const char ResFile033Name[8] = "033";

const char ResFile032Name[8] = "032";

const char ResFile031Name[8] = "031";

const char ResFile030Name[8] = "030";

const char ResFile029Name[8] = "029";

const char ResFile028Name[8] = "028";

const char ResFile027Name[8] = "027";

const char ResFile026Name[8] = "026";

const char ResFile180Name[8] = "180";

const char ResFile179Name[8] = "179";

const char ResFile178Name[8] = "178";

const char ResFile177Name[8] = "177";

const char ResFile176Name[8] = "176";

const char ResFile175Name[8] = "175";

const char ResFile174Name[8] = "174";

const char ResFile173Name[8] = "173";

const char ResFile172Name[8] = "172";

const char ResFile171Name[8] = "171";

const char ResFile170Name[8] = "170";

const char ResFile169Name[8] = "169";

const char ResFile168Name[8] = "168";

const char ResFile167Name[8] = "167";

const char ResFile166Name[8] = "166";

const char ResFile165Name[8] = "165";

const char ResFile164Name[8] = "164";

const char ResFile163Name[8] = "163";

const char ResFile162Name[8] = "162";

const char ResFile161Name[8] = "161";

const char ResFile160Name[8] = "160";

const char ResFile159Name[8] = "159";

const char ResFile158Name[8] = "158";

const char ResFile157Name[8] = "157";

const char ResFile156Name[8] = "156";

const char ResFile155Name[8] = "155";

const char ResFile154Name[8] = "154";

const char ResFile153Name[8] = "153";

const char ResFile152Name[8] = "152";

const char ResFile151Name[8] = "151";

const char ResFile150Name[8] = "150";

const char ResFile149Name[8] = "149";

const char ResFile148Name[8] = "148";

const char ResFile147Name[8] = "147";

const char ResFile146Name[8] = "146";

const char ResFile145Name[8] = "145";

const char ResFile144Name[8] = "144";

const char ResFile143Name[8] = "143";

const char ResFile142Name[8] = "142";

const char ResFile141Name[8] = "141";

const char ResFile140Name[8] = "140";

const char ResFile139Name[8] = "139";

const char ResFile138Name[8] = "138";

const char ResFile137Name[8] = "137";

const char ResFile136Name[8] = "136";

const char ResFile135Name[8] = "135";

const char ResFile134Name[8] = "134";

const char ResFile133Name[8] = "133";

const char ResFile132Name[8] = "132";

const char ResFile131Name[8] = "131";

const char ResFile130Name[8] = "130";

const char ResFile129Name[8] = "129";

const char ResFile128Name[8] = "128";

const char ResFile127Name[8] = "127";

const char ResFile126Name[8] = "126";

const char ResFile125Name[8] = "125";

const char ResFile124Name[8] = "124";

const char ResFile123Name[8] = "123";

const char ResFile122Name[8] = "122";

const char ResFile121Name[8] = "121";

const char ResFile120Name[8] = "120";

const char ResFile119Name[8] = "119";

const char ResFile118Name[8] = "118";

const char ResFile117Name[8] = "117";

const char ResFile116Name[8] = "116";

const char ResFile115Name[8] = "115";

const char ResFile113Name[8] = "113";

const char ResFile112Name[8] = "112";

const char ResFile111Name[8] = "111";

const char ResFile110Name[8] = "110";

const char ResFile109Name[8] = "109";

const char ResFile108Name[8] = "108";

const char ResFile107Name[8] = "107";

const char ResFile106Name[8] = "106";

const char ResFile105Name[8] = "105";

const char ResFile104Name[8] = "104";

const char ResFile103Name[8] = "103";

const char ResFile102Name[8] = "102";

const char ResFile101Name[8] = "101";

const char ResFile100Name[8] = "100";

const char ResFile099Name[8] = "099";

const char ResFile098Name[8] = "098";

const char ResFile097Name[8] = "097";

const char ResFile096Name[8] = "096";

const char ResFile095Name[8] = "095";

const char ResFile094Name[8] = "094";

const char ResFile093Name[8] = "093";

const char ResFile092Name[8] = "092";

const char ResFile091Name[8] = "091";

const char ResFile090Name[8] = "090";

const char ResFile089Name[8] = "089";

const char ResFile088Name[8] = "088";

const char ResFile087Name[8] = "087";

const char ResFile086Name[8] = "086";

const char ResFile085Name[8] = "085";

const char ResFile084Name[8] = "084";

const char ResFile083Name[8] = "083";

const char ResFile082Name[8] = "082";

const char ResFile081Name[8] = "081";

const char ResFile080Name[8] = "080";

const char ResFile079Name[8] = "079";

const char ResFile078Name[8] = "078";

const char ResFile077Name[8] = "077";

const char ResFile076Name[8] = "076";

const char ResFile075Name[8] = "075";

const char ResFile074Name[8] = "074";

const char ResFile073Name[8] = "073";

const char ResFile072Name[8] = "072";

const char ResFile071Name[8] = "071";

const char ResFile070Name[8] = "070";

const char ResFile069Name[8] = "069";

const char ResFile068Name[8] = "068";

const char ResFile067Name[8] = "067";

const char ResFile066Name[8] = "066";

const char ResFile065Name[8] = "065";

const char ResFile064Name[8] = "064";

const char ResFile063Name[8] = "063";

const char ResFile062Name[8] = "062";

const char ResFile061Name[8] = "061";

const char ResFile060Name[8] = "060";

const char ResFile059Name[8] = "059";

const char ResFile058Name[8] = "058";

const char ResFile057Name[8] = "057";

const char ResFile056Name[8] = "056";

const char ResFile055Name[8] = "055";

const char ResFile054Name[8] = "054";

const char ResFile053Name[8] = "053";

const char ResFile052Name[8] = "052";

const char ResFile051Name[8] = "051";

const char ResFile050Name[8] = "050";

const char ResFile049Name[8] = "049";

const char ResFile048Name[8] = "048";

const char ResFile047Name[8] = "047";

const char ResFile046Name[8] = "046";

const char ResFile045Name[8] = "045";

const char ResFile044Name[8] = "044";

const char ResFile043Name[8] = "043";

const char ResFile042Name[8] = "042";

const char ResFile041Name[8] = "041";

const char ResFile040Name[8] = "040";

const char ResFile039Name[8] = "039";

const char ResFile038Name[8] = "038";

const char ResFile037Name[8] = "037";

const char ResFile036Name[8] = "036";

const char ResFile035Name[8] = "035";

const char ResFile034Name[8] = "034";

const char ResFile000Name[8] = "000";

const char ResCitiPath[8] = "citi\\";

const char ResPplPath[8] = "ppl\\";

const char ResNpcPath[8] = "npc\\";

const char ResPcPath[8] = "pc\\";

const char ResEtcPath[8] = "etc\\";

const char ResAgAgPath[8] = "ag\\ag";

const char ResAgName[8] = "ag";

const char ResVxVxPath[8] = "vx\\vx_";

const char ResVxName[8] = "vx_";

const char ResPcChoPath[8] = "pc\\cho_";

const char ResChoName[8] = "cho_";

const char ResEnemyPath[8] = "enemy\\";

const char ResRoboPath[8] = "robo\\";

const char ResPcSonPath[8] = "pc\\son_";

const char ResSonName[8] = "son_";

const char ResPcStnPath[8] = "pc\\stn_";

const char ResStnName[8] = "stn_";

const char ResPcJrPath[8] = "pc\\jr_";

const char ResJrPrefixName[8] = "jr_";

const char ResPcZigPath[8] = "pc\\zig_";

const char ResZigName[8] = "zig_";

const char ResPcMmoPath[8] = "pc\\mmo_";

const char ResMmoName[8] = "mmo_";

const char ResPcKosPath[8] = "pc\\kos_";

const char ResKosName[8] = "kos_";

const char ResBossName[8] = "boss_";

const char ResSeparatorName[8] = "_";

const char ResFereName[8] = "fere_";

const char ResFesoName[8] = "feso_";

const char ResFeroName[8] = "fero_";

const char ResFemaName[8] = "fema_";

const char ResUtroName[8] = "utro_";

const char ResUtsoName[8] = "utso_";

const char ResUtreName[8] = "utre_";

const char ResUtmaName[8] = "utma_";

const char ResGunoName[8] = "guno_";

const char ResMusName[8] = "mus_";

const char ResMufName[8] = "muf_";

const char ResMuxName[8] = "mux_";

const char ResTreName[8] = "tre_";

const char ResMuwName[8] = "muw_";

const char ResMuName[8] = "mu_";

const char ResAidName[8] = "aid_";

const char ResObjPath[8] = "obj\\";

const char ResMidName[8] = "mid_";

const char ResEidName[8] = "eid_";

const char ResWeaponPath[8] = "weapon\\";

const char ResWidName[8] = "wid_";

const char ResRidName[8] = "rid_";

const char ResCharPath[8] = "char\\";

const char ResCidName[8] = "cid_";

const char ResOldmCfName[8] = "oldm_cf";

const char ResOldwCfName[8] = "oldw_cf";

const char ResJrCfName[8] = "jr_cf";

const char ResMomoCfName[8] = "momo_cf";

const char ResZ8Name[8] = "Z8";

const char ResJRName[8] = "JR";

const char ResMOName[8] = "MO";

const char ResCSName[8] = "CS";

const char ResKMName[8] = "KM";

const char ResSOName[8] = "SO";

const char ResSaveName[8] = "save";

const char ResDiskName[8] = "disk";

const char ResNpcseName[8] = "npcse";

const char ResEneseName[8] = "enese";

const char ResMpeg2Name[8] = "mpeg2";

const char ResLoadsmdName[8] = "loadsmd";

const char ResLoadseName[8] = "loadse";

const char ResDummyName[8] = "dummy";

const char ResPlayerName[8] = "player";

const char ResScriptName[8] = "script";

const char ResLoadeneName[8] = "loadene";

const char ResLoadtexName[8] = "loadtex";

const char ResLoadmotName[8] = "loadmot";

const char ResLoadactName[8] = "loadact";

const char ResLoadmapName[8] = "loadmap";

const char ResResetName[8] = "reset";
