#include "common.h"
#include "shared.h"
#include "main/sdv.h"
#include "main/nml_model_set.h"
#include "main/xgl_thread.h"
#include "main/control_entry.h"
#include "main/xgl_sound.h"
#include "main/look_at_player.h"
#include "main/map_2.h"
#include "main/Undulate.h"
#include "main/act_1.h"
#include "main/act_2.h"
#include "main/db_light_write.h"
#include "main/face_point.h"
#include "main/game_camera.h"
#include "main/game_movie.h"
#include "main/game_over.h"
#include "main/game_pause_disp.h"
#include "main/get.h"
#include "main/init_drill.h"
#include "main/init_uwamono_sys.h"
#include "main/jthread.h"
#include "main/map_1.h"
#include "main/msg_init.h"
#include "main/near_dir.h"
#include "main/outer_product.h"
#include "main/party.h"
#include "main/play.h"
#include "main/res_get_path.h"
#include "main/runtime.h"
#include "main/sef.h"
#include "main/tcamera.h"
#include "main/twsys_init.h"
#include "main/vibration.h"
#include "main/xgl_cd.h"
#include "main/xgl_light.h"
#include "main/xgl_pad.h"
#include "main/xgl_render.h"
#include "main/xgl_studio.h"
#include "quadword.h"

#define game_mode_flags (GameLoopState.flags)
#define game_mode_kind (GameLoopState.kind)
#define game_mode_status (GameLoopState.status)
#define game_mode_saved_kind (GameLoopState.saved_kind)
#define game_mode_debug_storage (GameLoopState.debug_menu.storage)
#define game_mode_camera_callback (GameLoopState.camera_callback)

/*
 * The engine's actor record (`actor`, 64-entry array at main 0x0043c1e0,
 * 0xa70-byte stride; fuller evidence in src/main/near_dir.h,
 * src/main/enemy_2.h, src/main/set_motion.h, src/main/db_light_write.h and
 * src/main/tya.c, which name the same two fields for the same reason).
 * GameDrawShadow only reads +0x00 flags (bits 0x8 and 0x20) and +0x86, the
 * in-use id ACT_create writes and ACT_update skips a slot on when it is
 * zero; the offsets between them are not evidenced by this TU and stay
 * unmodeled.
 */
/* NmlModelBlock.flags: the block's part starts hidden. */
#define NML_BLOCK_FLAG_HIDDEN 8
#define GAME_SOUND_CHANNEL_COUNT 8
#define GAME_SOUND_BANK_COUNT 8
#define SOUND_SEQUENCE_NONE 0xFFFF
#define GAME_SOUND_VOLUME_MAX 0x7F
#define GAME_ENEMY_WORK_COUNT 16
#define GAME_PARAM_COUNT 8
#define GAME_ORG_COUNT 33
#define GAME_PLAYER_COUNT 16
#define GAME_BOX_COUNT 255
#define GAME_ID_LIGHT_COUNT 16
#define GAME_HIDDEN_PART_WORDS 32
#define GAME_HIDDEN_PART_BITS_PER_WORD 32

/* SaveData offsets: the game's user state, and the system options (menu_system). */
#define SAVE_OPTION_SOUND_OUTPUT 0x28
#define SAVE_OPTION_VIBRATION 0x3c
#define SAVE_OPTION_HDD 0x40
#define SAVE_GAME_USER 0x50

/* The debug pad combo: SELECT + L1 + R1 held. */
#define GAME_PAD_COMBO (PAD_SELECT | PAD_L1 | PAD_R1)

/* Battle results returned to GameModeCfMain. */
#define BATTLE_RESULT_WIN 1
#define BATTLE_RESULT_LOSE 2
#define BATTLE_RESULT_ESCAPE 3

/* MapUnit.flags. */
#define MAPUNIT_FLAG_SYMBOL 0x10000
#define MAPUNIT_FLAG_UWAMONO_EFFECT 0x20000
#define MAPUNIT_FLAG_ITEM_BOX 0x08000000
#define MAPUNIT_FLAG_SAVE_SYMBOL 0x20000000
#define MAPUNIT_FLAG_SHOP_SYMBOL 0x40000000
/* MapUnit.symbol_kind of event/return symbols. */
#define SYMBOL_KIND_EVENT 8
#define SYMBOL_KIND_RETURN 9
#define SYMBOL_TRIGGER_RADIUS 0.7f
#define SYMBOL_EFFECT_SAVE_EVENT 0x2d8
#define SYMBOL_EFFECT_SHOP_RETURN 0x2d9
#define GAME_SE_SYMBOL 12
#define GAME_SE_PAUSE 4

/* Script flags of an actor (getScriptFlag). */
#define SCRIPT_FLAG_FACE_PLAYER 2
#define SCRIPT_FLAG_TALK 0x10000
#define SCRIPT_FLAG_TOUCH 0x20000
/* Actor.attribute_flags: bits 8-12 hold the attribute id, bits 13-15 block it. */
#define ACTOR_ATTR_ID_MASK 0x1f00
#define ACTOR_ATTR_BLOCK_MASK 0xe000
#define ACTOR_ATTR_ID_SHIFT 8
/* Actor.flags */
#define ACTOR_FLAG_NO_SHADOW 8
#define ACTOR_FLAG_SHADOW 0x20
/* Top nibble of an actor inUseId / MapUnit serial classifies the object. */
#define ID_CLASS_MASK 0xf000

#define GAME_SEARCH_DISTANCE_INIT 65535.0f
#define GAME_TALK_FACING_LIMIT 1.0471976f /* 60 degrees */

/* GS coordinates of the screen's top-left (12.4 fixed point >> 4) and the
 * range GameDispTag accepts: 512x448 pixels. */
#define GAME_GS_ORIGIN_X 0x700
#define GAME_GS_ORIGIN_Y 0x720
#define GAME_GS_X_MIN 0x7001u
#define GAME_GS_X_RANGE 0x1fffu
#define GAME_GS_Y_MIN 0x7201
#define GAME_GS_Y_MAX 0x8dff

/* Frames the controlled actor flashes after returning from a battle. */
#define GAME_BATTLE_RETURN_FLASH_FRAMES 0x4B

/* Alpha of the screen-clear sprite drawn when no clear callback is set. */
#define GAME_CLEAR_ALPHA 0x80

/* Radar position and colors given to GameRadarDraw. */
#define GAME_RADAR_X 0x160
#define GAME_RADAR_Y 0x120
#define GAME_RADAR_COLOR 0xFFFF00
#define GAME_RADAR_RGB 0x80
#define GAME_RADAR_ALPHA 0x60

/*
 * GNU EE native TI storage/copy type. Used only for the
 * 16-byte aligned typed storage that the original clears with por+sq; no
 * wide arithmetic.
 */
typedef struct GameOrgRecord {
    u8 unmodeled_00[0x180];
} GameOrgRecord;

typedef struct GameUserSave {
    u8 unmodeled_0000[0x101b0];
    int event_id;
    float params[8];
    u8 unmodeled_101d4[0x101e0 - 0x101d4];
    Vector4 actor_position;
    Vector4 actor_rotation;
    u8 unmodeled_10200[0x103b4 - 0x10200];
    GameOrgRecord org[33];
    GamePlayerRecord players[16];
    u8 unmodeled_13fb4[0x14634 - 0x13fb4];
    u16 item_box[256];
    u16 weapon_box[256];
    u16 bullet_box[256];
    u16 accessory_box[256];
    u16 event_box[256];
    u8 unmodeled_15034[4];
    u64 money;
    u8 unmodeled_15040[0x16340 - 0x15040];
    u32 user_word;
    u8 unmodeled_16344[0x16350 - 0x16344];
    u64 user_longs[2];
} GameUserSave;

typedef struct GameMapUnit {
    u32 flags;
    u8 unmodeled_04[0x10 - 0x04];
    DoublewordAlignedVector4 position;
    u8 unmodeled_20[0x60];
    Matrix4 *orientation;
    u8 unmodeled_84[0x1e];
    u8 action_sub;
    u8 unmodeled_a3;
    s16 serial;
    u8 unmodeled_a6[0x0a];
    DoublewordAlignedVector4 extent;
    u8 unmodeled_c0[0x1a0 - 0xc0];
    s16 symbol_kind;
    s16 shop_number;
    u8 item_box_hit;
    u8 unmodeled_1a5[0x300 - 0x1a5];
} GameMapUnit;

typedef struct GameClearEnvironment {
    u8 unmodeled_00[0x20];
    u32 red;
    u32 green;
    u32 blue;
    u32 alpha;
} GameClearEnvironment;

typedef struct GamePackedXyz2 {
    u32 x;
    u32 y;
    u32 z;
    u32 adc;
} GamePackedXyz2;

typedef struct GameTestEnvironment {
    u32 vif_nop[3];
    u32 vif_directhl;
    u64 gif_tag;
    u64 gif_registers;
    u64 test;
    u64 test_register;
    u32 red;
    u32 green;
    u32 blue;
    u32 alpha;
    GamePackedXyz2 vertices[2];
} GameTestEnvironment;


/* Only the loaded data pointer at +4 is used by the original here.
 * No complete resource-handle extent or unknown prefix field is claimed. */
typedef struct GameResourceModelPrefix {
    unsigned char unmodeled_00[4];
    struct NmlModel *data;
} GameResourceModelPrefix;


typedef struct EnemyWorkPostTalk {
    u8 unmodeled_00[0x4e];
    u16 stiff_timer;
    short effect_timer[2];
    u8 unmodeled_54[0x37b4 - 0x54];
    u32 post_talk_flags;
    int enemy_id;
    u8 unmodeled_37bc[0x3830 - 0x37bc];
    short encount_lock;
    u8 unmodeled_3832[0x38b0 - 0x3832];
} EnemyWorkPostTalk;




typedef struct GameRadarParams {
    int x;
    int y;
    int color;
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
} GameRadarParams;

/* The RTPS producer writes integer screen xyz and float reciprocal w.
 * Its frozen Vector4 output prototype remains an explicit nominal
 * transport boundary; this private diagnostic does not erase that type. */
typedef struct GameProjectedPoint {
    int x;
    int y;
    int z;
    float w;
/* GameRadarDraw reads x/y, RGB and four unsigned RGBA bytes at+12..15. */
} GameProjectedPoint;

extern GameOrgRecord orgData[33];
extern GamePlayerRecord plChaData[16];
extern u16 itmBox[255];
extern u16 wpnBox[255];
extern u16 bltBox[255];
extern u16 accBox[255];
extern u16 evtBox[255];
extern GameClearEnvironment ClearEnv;
extern MapUnitRecord MapUnit[64];
extern EnemyWorkPostTalk enepc[16];
extern Actor actor[ACTOR_COUNT];
extern const char eobMethodName[];
extern void *const stageVM;
extern void sefPopEffect(void);
extern void ClearUwamonoEffect(MapUnitRecord *unit);
extern void nmlModelSetPartsVisible(NmlModel *model, int part, int visible);
extern void TWSYS_update(void);
extern void ACT_pauseUpdate(void);
extern void GameDebugMenu(void);
int GameModeDebugMenu(void);
extern void After_Talk(SceneObject object);
void GameStateRestoreCameraLight(void);
extern void GameStateRestoreKoware(void);

/* Defined in src/main/act_3.c (main/tu265). */
void ACT_DrawShadowBegin(void);

void ACT_DrawShadow(Actor *unit);
void ACT_DrawShadowEnd(void);
extern void sefPushEffect(void);
extern void xglFontLoad(int font, int mode);
extern void UwamonoBgmFadeOut(void);
extern void ACT_init(void);
extern void XTK_setWindowOwner(void *owner);
extern void ResetShootSys(void);
extern void Player_System_Init(Actor *player_actor);
extern void SCRIPT_incCfTime(void);
extern int SCRIPT_sceneChangeTimeGet(void);
extern int SCRIPT_talkIgnoreGet(void);
extern void nmlModelSendMenuStart(void);
extern void nmlModelSendMenuEnd(void);
extern void xglCullingIgnoreOff(void);
extern void MainMenu(void);
extern void nmlModelSendSignalStartBattle(void);
extern void func_A22668(void);
extern int func_00A0B5F0(void);
extern void WindowTexLoad(int first, int second);
extern void CallMethod_I(const char *name, int argument);
extern int Enemy_After_Battle(int result);
extern void Actor_LookAt_Init(Actor *actors);
extern void Actor_LookAt(Actor *actor);
extern void Actor_LookAt_Release(Actor *actor, int flag);
extern void ACT_updateSequence(Actor *actor);
extern void GameIdLightSet(Actor *actor, void *query);
extern void SCRIPT_execEntered(int attribute);
extern void SCRIPT_execTouchto(void *actor);
extern void GameRadarDraw(GameRadarParams *params);
extern void Enemy_SystemInit(void);
extern void xglRenderClearDepth(void);
extern void GameResourceInit(int first, int second);
extern void SCRIPT_sceneChangeTimeInit(void);
extern void SCRIPT_sceneChangeTimeDec(void);
extern void SCRIPT_clrEventActiveFlag(void);
extern void SCRIPT_talkIgnoreClr(void);
extern void GameDefocusSet(int first, int second, int third);
extern void GameBgDrawType0(void);
extern void func_A22628(void);
extern void *xglTaskInitial(void *manager, int capacity, int flags);
extern u8 *SCRIPT_init(u8 *work);
extern void GameRadarInit(void);
extern void nmlModelSendPauseSignal(int paused);
extern void Disp_Homing(void);
extern void xglCullingMapInit(void);
extern void sefDeleteEffectCf(SchedulerState *effect);
extern void PartyTimePauseStart(void);
static Actor *checkTouch(Actor *player);
const char cfRegistSoundName[9] = "REGISTCF";

const char eobAlwaysMethodName[11] = "EOB_Always";

const char eobEscapeMethodName[11] = "EOB_Escape";

/* Text: {color #000000}%4x\n%6x */
static char str_0[12] = "\014\000\000\000%4x\n%6x";

/* Text: {color #000000}%4x\n%6x */
static char str_1[12] = "\014\000\000\000%4x\n%6x";

static GameTestEnvironment TestEnv_2_00338620 = {
    .vif_nop = { 0, 0, 0 },
    .vif_directhl = 0x51000005U,
    .gif_tag = 0x4003400000008001ULL,
    .gif_registers = 0x551eULL,
    .test = 0x00071001ULL,
    .test_register = 0x47ULL,
    .red = 0, .green = 0, .blue = 0, .alpha = 0,
    .vertices = {
        { .x = 0x6ff8, .y = 0x71f7, .z = 0x40000000U, .adc = 0 },
        { .x = 0x8ff8, .y = 0x8df7, .z = 0x40000000U, .adc = 0 }
    }
};

static SchedulerState *saveEffe = 0;

static SchedulerState *shopEffe = 0;

static SchedulerState *evsEffe = 0;

static SchedulerState *retEffe = 0;

static short pause_check;

static int shopNo;

static unsigned char GameCFSoundMenuPurgeFlag;

static u64 attrPrev;

static Actor *prevTouch;

/* Declared here, not at the top: moving it up changes the original code. */
#include "game.h"
#include "main/game.h"

const char dispTagFormat[8] = "%4x\n%6x";

/* GameDebugMenu accesses the first two bytes at GameLoopState+0x29f50.
 * Mode exits reset this complete 16-byte region with one native quadword
 * store. The remaining bytes have no field interpretation in these TUs. */
typedef union GameDebugMenuStorage {
    Quadword storage;
    struct {
        u8 page;
        u8 page_initialized;
        u8 unmodeled_02[14];
    } fields;
} GameDebugMenuStorage;

/*
 * PARTIAL ACCESSED VIEW of GameLoopState (0x2a030-byte object at 0x338680).
 * Union of the offsets the merged handlers access:
 *  +0x0c kind u16, +0x10 flags u32, +0x28 camera_callback,
 *  +0x29f40 status u8 (read by GameModeCfEvent),
 *  +0x29f50 debug_menu 16-byte reset region (cleared with por+sq),
 *  +0x29f60 saved_kind u16.
 * Spans named _unmodeled_* are opaque sizing bytes never accessed by these
 * bodies; the remainder of the object beyond 0x29f62 is unmodeled. The name
 * is TU-local by canon
 */
/*
 * GameStateSave/GameStateRestore back up and restore two more GameLoopState
 * halfwords (main VA 0x00243278/0x002432f0): +0x0e kindDetail, the halfword
 * GameStateSave copies alongside +0x0c kind, and the +0xe0/+0xe2 backup slot
 * GameStateSave writes them into and GameStateRestore reads back to restore
 * kind and kindDetail.
 */
typedef struct GameLoopStateLayout {
    u8 *saved_work_end;
    Actor *controlled_actor;
    XglTaskScheduler *scheduler;
    u16 kind;
    u16 kindDetail;
    union {
        struct {
            u32 flags;
            u32 base_flags;
        };
        u64 flags_wide; /* +0x10, original GameDrawSync/GameExecute 64-bit flags */
    };
    u32 current_event_id;
    union {
        u32 cleared_1c;
        u32 event_sub_id;
    };
    u32 shoot_control;
    u8 _unmodeled_24[4];
    GameModeCameraCallback camera_callback;
    void (*clear_callback)(void); /* +0x2c */
    void (*map_draw_callback)(void); /* +0x30 */
    void (*actor_draw_callback)(void); /* +0x34 */
    void (*shadow_draw_callback)(void); /* +0x38 */
    void (*unit_draw_callback)(void); /* +0x3c */
    void (*frame_hook)(int stage); /* +0x40 */
    unsigned char unmodeled_044[0x50 - 0x44];
    u16 map_number; /* +0x50 */
    unsigned char unmodeled_052[0x54 - 0x52];
    GameResourceModelPrefix *model;
    int state58;
    int state5c;
    float params[8]; /* +0x60 */
    float vector80[4];
    float vector90[4];
    float vectorA0[4];
    int stateB0;
    u8 _unmodeled_b4[4];
    int frame_countdown; /* +0xb8 */
    unsigned char unmodeled_0bc[0xc1 - 0xbc];
    u8 move_mode;
    u8 _unmodeled_c2[6];
    short stateC8;
    u8 _unmodeled_ca[6];
    u8 stateD0;
    u8 _unmodeled_d1[0xe0 - 0xd1];
    u16 restoreKind;
    u16 restoreKindDetail;
    u8 _unmodeled_e4[0x164 - 0xe4];
    u32 hidden_parts[32];
    unsigned char unmodeled_1e4[0x1f0 - 0x1e4];
    u32 saved_actor_flags; /* +0x1f0 */
    short saved_actor_state84; /* +0x1f4 */
    short saved_actor_in_use_id; /* +0x1f6 */
    void (*saved_actor_update)(Actor *actor); /* +0x1f8 */
    unsigned char unmodeled_1fc[0x200 - 0x1fc];
    Vector4 saved_actor_position; /* +0x200 */
    Vector4 saved_actor_rotation; /* +0x210 */
    u16 saved_map_number; /* +0x220 */
    unsigned char unmodeled_222[0x250 - 0x222];
    Actor saved_actors[64];
    StudioLight saved_light; /* +0x29e50, 0xf0-byte camera light snapshot */
    u8 status;
    u8 _unmodeled_29f41[3];
    float shoot_range;
    u8 _unmodeled_29f48[8];
    GameDebugMenuStorage debug_menu;
    u16 saved_kind;
    unsigned char unmodeled_29f62[0x29f97 - 0x29f62];
    u8 battle_flags;
    unsigned char unmodeled_29f98[0x29f9c - 0x29f98];
    Actor *battle_enemy;
    unsigned char unmodeled_29fa0[0x2a018 - 0x29fa0];
    u32 user_word; /* +0x2a018 */
    unsigned char unmodeled_2a01c[0x2a020 - 0x2a01c];
    u64 user_longs[2]; /* +0x2a020 */
} GameLoopStateLayout;

GameLoopStateLayout GameLoopState = {0};

/*
 * PadData (0xd0-byte object at 0x490d90) through the shared
 * PadDataDebugLayout view (xeno/core/types.h): the merged handlers read the
 * halfwords at +0x28 (held), +0x2a (pressed) and +0x2e (debug_buttons,
 * GameModeDebugMenu). The canonical `extern PadPrefix PadData;` of
 * xeno/core/functions.h stops at +0x2c, so that header is not included and
 * the prototypes this TU needs are repeated below in their canonical
 * spelling.
 */
extern PadDataDebugLayout PadData;

extern StudioLight GameIdLight[16];

static inline Vector4 *GameSymbolPosition(int index)
{
    MapUnitRecord *unit = &MapUnit[index];
    return &unit->position;
}

static inline s16 *GameSymbolKindAddress(int index)
{
    return &MapUnit[index].symbol_kind;
}

static void GameCFSoundPurgeSub(void)
{
    int index = 0;

    xglSoundSendEffect(0, 0, 2);
    xglSoundSendEffect(0, 0, 3);
    for (; index < GAME_SOUND_BANK_COUNT; index++) {
        xglSoundSendEffect(0, 0, index + 4);
    }
}

static void GameCFSoundPurge(void)
{
    int bank;

    for (bank = 0; bank < GAME_SOUND_BANK_COUNT; bank++) {
        xglSoundSendSwd(0, -1 - bank);
        xglSoundSendSmd2(0, bank);
    }
    xglSoundSendEffect(0, 0, 1);
    GameCFSoundPurgeSub();
    SsdResetSegmentAllocMode(0x70000);
    SsdSetSegmentAllocMode(0xA8000, 0x10000);
}

static void GameCFSoundReload(void)
{
    int bank;
    u8 *segment;

    segment = (u8 *) ((u32) (WorkEnd + 0x3F) & ~0x3F);
    for (bank = 0; bank < GAME_SOUND_BANK_COUNT; bank++) {
        xglSoundSendSwd(0, -1 - bank);
        xglSoundSendSmd2(0, bank);
    }
    SsdResetSegmentAllocMode(0xA8000);
    SsdSetSegmentAllocMode(0x70000, 0x10000);
    xglSoundLoadEffect(cfRegistSoundName, segment, 1);
    GameCFSoundPurgeSub();
}

void GameCFSoundMenuPurge(int mode)
{
    int bank;
    int result[4];
    unsigned short sequence;

    if (mode == 1) {
        GameCFSoundMenuPurgeFlag = 0;
        xglSoundSendEffect(0, 0, 3);
        for (bank = 0; bank < GAME_SOUND_CHANNEL_COUNT; bank++) {
            sequence = SoundWork.channels[bank].sequence;
            if (sequence == SOUND_SEQUENCE_NONE) {
                continue;
            }
            SsdGetSeqPlayStatus(sequence);
            do {
            } while (SsdGetResultValue(result) < 0);
            if ((unsigned short)result[0] == 1) {
                GameCFSoundMenuPurgeFlag |= 1 << bank;
                xglSoundSequenceStop2(bank);
            }
        }
    }
    EnemySound_StopAll(1);
    for (bank = 4; bank < GAME_SOUND_BANK_COUNT; bank++) {
        xglSoundSendEffect(0, 0, bank + 4);
    }
}

void GameCFSoundMenuReload(void)
{
    char environment_name[64];
    const char *enemy_name;
    void *free_address;
    int channel;
    int enemy_index;

    free_address = GameResourceGetFreeAddr();
    xglSoundSendSwd(0, -5);
    xglSoundSendSmd2(0, 4);
    RES_GetMapEnvSeName(environment_name);
    xglSoundLoadEffect(environment_name, free_address, 3);

    for (channel = 0; channel < GAME_SOUND_CHANNEL_COUNT; channel++) {
        if (((GameCFSoundMenuPurgeFlag >> channel) & 1) != 0 &&
            UmnSimulationNo == 0 && MenuDrillCall == 0) {
            xglSoundSequenceNormal2(channel, GAME_SOUND_VOLUME_MAX);
        }
    }

    for (enemy_index = 4; ; enemy_index++) {
        enemy_name = RES_GetEnemySeName(enemy_index);
        if (enemy_name == 0 || enemy_name[0] == 0) {
            break;
        }
        xglSoundLoadEffect(enemy_name, free_address, enemy_index + 4);
    }
}

void Game_Data_Push(void)
{
    short actor_index;
    short index;
    NmlModel *model;
    NmlModelBlock *block;
    int *block_offsets;

    for (actor_index = 0; actor_index < ACTOR_COUNT; actor_index++) {
        GameLoopState.saved_actors[actor_index] = actor[actor_index];
    }
    GameCFSoundPurge();
    GameStateSave();
    sefPushEffect();
    for (index = 0; index < GAME_HIDDEN_PART_WORDS; index++) {
        GameLoopState.hidden_parts[index] = 0;
    }
    model = GameLoopState.model->data;
    block_offsets = model->blockOffsets;
    for (index = 0; index < model->blockCount; index++) {
        block = (NmlModelBlock *)((u8 *)model + block_offsets[index]);
        if ((block->flags & NML_BLOCK_FLAG_HIDDEN) != 0) {
            int part_index = block->partId;
            int word_index = part_index / GAME_HIDDEN_PART_BITS_PER_WORD;
            GameLoopState.hidden_parts[word_index] |= 1 << part_index;
        }
    }
}

void Game_Data_Pop(void)
{
    unsigned short serials[64];
    short index;
    Actor *backup;
    NmlModel *model;
    int bit;

    for (index = 0; index < MAP_UNIT_COUNT; index++) {
        serials[index] = MapUnit[index].serial;
        MapUnit[index].serial = -1;
    }
    while (xglCdSync() != 0)
        xglSleep();
    GameCFSoundReload();
    sefInitEffectCf();
    GameResourceLoad(GameLoopState.current_event_id);
    for (index = 0; index < MAP_UNIT_COUNT; index++)
        MapUnit[index].serial = serials[index];
    GameStateRestore();
    GameLoopState.cleared_1c = 0;
    backup = GameLoopState.saved_actors;

    for (index = 0; index < ACTOR_COUNT; index++, backup++) {
        actor[index] = *backup;
        if (actor[index].inUseId == 0)
            continue;
        {
            Actor *unit = &actor[index];
            {
                void *matrices;

                ACT_setMotion2(unit, actor[index].animation.slot.currentDataId, actor[index].animation.slot.flags & ~3);
                ACT_updateMotion(unit);
                matrices = unit->jointMatrices;
                unit->jointMatrices = unit->previousJointMatrices;
                unit->previousJointMatrices = matrices;
                matrices = unit->staticValueRecords;
                unit->staticValueRecords = unit->valueRecords;
                unit->valueRecords = matrices;
                unit->animation.slot.bufferIndex = (unit->animation.slot.bufferIndex + 1) & 1;
            }
            {
                void *matrices;

                ACT_setMotion2(unit, actor[index].animation.slot.currentDataId, actor[index].animation.slot.flags & ~3);
                ACT_updateMotion(unit);
                matrices = unit->jointMatrices;
                unit->jointMatrices = unit->previousJointMatrices;
                unit->previousJointMatrices = matrices;
                matrices = unit->staticValueRecords;
                unit->staticValueRecords = unit->valueRecords;
                unit->valueRecords = matrices;
                unit->animation.slot.bufferIndex = (unit->animation.slot.bufferIndex + 1) & 1;
            }
        }
    }
    sefPopEffect();
    GameLoopState.flags &= ~GAME_FLAG_ON_EVENT_SYMBOL;
    GameLoopState.flags &= ~GAME_FLAG_ON_RETURN_SYMBOL;
    GameLoopState.flags &= ~GAME_FLAG_ON_SAVE_SYMBOL;
    GameLoopState.flags &= ~GAME_FLAG_ON_SHOP_SYMBOL;
    UnlockMapUnit();
    for (index = 0; index < MAP_UNIT_COUNT; index++) {
        if (MapUnit[index].serial != -1 && (MapUnit[index].flags & MAPUNIT_FLAG_UWAMONO_EFFECT)) {
            MapUnitRecord *map_unit = &MapUnit[index];
            MapUnit[index].serial = -1;
            MapUnit[index].update = 0;
            ClearUwamonoEffect(map_unit);
        }
    }
    model = GameLoopState.model->data;
    for (index = 0; index < GAME_HIDDEN_PART_WORDS; index++) {
        for (bit = 0; bit < GAME_HIDDEN_PART_BITS_PER_WORD; bit++) {
            /* Original uses signed SRAV before selecting the one bit.
             * This scalar interpretation does not erase an interface. */
            if (((int)GameLoopState.hidden_parts[index] >> bit) & 1)
                nmlModelSetPartsVisible(model, index * GAME_HIDDEN_PART_BITS_PER_WORD + bit, 0);
        }
    }
    {
        u32 flags = GameLoopState.flags;

        GameLoopState.controlled_actor->status_flags |= 0x1000;
        flags &= ~0x4000;
        flags &= ~GAME_FLAG_NO_TALK_POINT;
        flags &= ~GAME_FLAG_BATTLE;
        GameLoopState.flags = flags;
        GameLoopState.move_mode |= 0x20;
    }
}

void MenuGameDataPush(void)
{
    short i;

    for (i = 0; i < ACTOR_COUNT; i++) {
        GameLoopState.saved_actors[i] = actor[i];
    }
    GameStateSave();
}

void MenuGameDataPop(void)
{
    short i;
    int id_changed;
    PartyDataPrefix *party;
    Actor *target;
    void *matrices;
    void *restored_buffer;

    GameStateRestore();
    for (i = 0; i < ACTOR_COUNT; i++) {
        id_changed = 0;
        party = PartyDataGet();
        if (&actor[i] == GameLoopState.controlled_actor) {
            if (GameLoopState.saved_actors[i].inUseId != 0 &&
                GameLoopState.saved_actors[i].inUseId != party->leader_battle_id) {
                GameLoopState.saved_actors[i].inUseId = party->leader_battle_id;
                id_changed = 1;
            }
        }
        actor[i] = GameLoopState.saved_actors[i];
        if (actor[i].inUseId != 0) {
            if (id_changed) {
                target = &actor[i];
                ACT_setModelWrapper(target, 0);
                ACT_setHumanHand(target, 0);
                ACT_setMotion2(target, 0, 8);
                ACT_updateMotion(target);
                matrices = target->jointMatrices;
                target->jointMatrices = target->previousJointMatrices;
                target->previousJointMatrices = matrices;
                matrices = target->staticValueRecords;
                target->staticValueRecords = target->valueRecords;
                target->valueRecords = matrices;
                target->animation.slot.bufferIndex = (target->animation.slot.bufferIndex + 1) & 1;
                ACT_setMotion2(target, 0, 8);
                ACT_updateMotion(target);
                restored_buffer = target->jointMatrices;
                target->jointMatrices = target->previousJointMatrices;
                target->previousJointMatrices = restored_buffer;
                restored_buffer = target->staticValueRecords;
                target->staticValueRecords = target->valueRecords;
                target->valueRecords = restored_buffer;
                target->animation.slot.bufferIndex = (target->animation.slot.bufferIndex + 1) & 1;
            } else {
                ACT_updateMotion(&actor[i]);
            }
        }
    }
}

void GameStateSaveCameraLight(void)
{
    StudioLight *saved_light = &GameLoopState.saved_light;

    GameCameraStateSave();
    *saved_light = *xglStudioGetLight2();
}

void GameStateRestoreCameraLight(void)
{
    StudioLight *light;
    StudioLight *saved_light;

    GameCameraStateRestore();
    light = xglStudioGetLight2();
    saved_light = &GameLoopState.saved_light;
    *light = *saved_light;
}

void GameStateSave(void)
{
    Actor *controlled_actor;

    GameLoopState.restoreKind = GameLoopState.kind;
    GameLoopState.restoreKindDetail = GameLoopState.kindDetail;
    controlled_actor = GameLoopState.controlled_actor;
    GameLoopState.saved_actor_flags = controlled_actor->flags;
    GameLoopState.saved_actor_state84 = controlled_actor->state84;
    GameLoopState.saved_actor_in_use_id = controlled_actor->inUseId;
    GameLoopState.saved_actor_update = controlled_actor->update;
    QuadwordCopy(&GameLoopState.saved_actor_position, &controlled_actor->position);
    QuadwordCopy(&GameLoopState.saved_actor_rotation, &controlled_actor->rotation);
    GameLoopState.saved_map_number = GameLoopState.map_number;
    GameStateSaveCameraLight();
}

void GameStateRestore(void)
{
    GameLoopState.kind = GameLoopState.restoreKind;
    GameLoopState.kindDetail = GameLoopState.restoreKindDetail;
    GameStateRestoreCameraLight();
    GameStateRestoreKoware();
}

void GamePushSaveDataUser(void)
{
    GameUserSave *save;
    GameOrgRecord *org;
    GamePlayerRecord *players;
    GameLoopStateLayout *state = &GameLoopState;
    int i;

    save = (GameUserSave *)(SaveData + SAVE_GAME_USER);
    save->event_id = state->current_event_id;
    for (i = 0; i < GAME_PARAM_COUNT; i++) {
        save->params[i] = state->params[i];
    }
    QuadwordCopy(&save->actor_position, &state->saved_actor_position);
    QuadwordCopy(&save->actor_rotation, &state->saved_actor_rotation);
    org = save->org;
    players = save->players;
    for (i = 0; i < GAME_ORG_COUNT; i++) {
        org[i] = orgData[i];
    }
    for (i = 0; i < GAME_PLAYER_COUNT; i++) {
        players[i] = plChaData[i];
    }
    for (i = 0; i < GAME_BOX_COUNT; i++) {
        save->item_box[i] = itmBox[i];
        save->weapon_box[i] = wpnBox[i];
        save->bullet_box[i] = bltBox[i];
        save->accessory_box[i] = accBox[i];
        save->event_box[i] = evtBox[i];
    }
    save->money = moneyBox;
    save->user_word = state->user_word;
    save->user_longs[0] = state->user_longs[0];
    save->user_longs[1] = state->user_longs[1];
}

void GamePopSaveDataUser(void)
{
    GameUserSave *save;
    GameOrgRecord *org;
    GamePlayerRecord *players;
    GameLoopStateLayout *state = &GameLoopState;
    int i;

    save = (GameUserSave *)(SaveData + SAVE_GAME_USER);
    state->current_event_id = save->event_id;
    for (i = 0; i < GAME_PARAM_COUNT; i++) {
        state->params[i] = save->params[i];
    }
    QuadwordCopy(&state->saved_actor_position, &save->actor_position);
    QuadwordCopy(&state->saved_actor_rotation, &save->actor_rotation);
    org = save->org;
    players = save->players;
    for (i = 0; i < GAME_ORG_COUNT; i++) {
        orgData[i] = org[i];
    }
    for (i = 0; i < GAME_PLAYER_COUNT; i++) {
        plChaData[i] = players[i];
    }
    for (i = 0; i < GAME_BOX_COUNT; i++) {
        itmBox[i] = save->item_box[i];
        wpnBox[i] = save->weapon_box[i];
        bltBox[i] = save->bullet_box[i];
        accBox[i] = save->accessory_box[i];
        evtBox[i] = save->event_box[i];
    }
    moneyBox = save->money;
    state->user_word = save->user_word;
    state->user_longs[0] = save->user_longs[0];
    state->user_longs[1] = save->user_longs[1];
    SsdSetOutputMode(SaveData[SAVE_OPTION_SOUND_OUTPUT]);
    PadData.vibrationReset = SaveData[SAVE_OPTION_VIBRATION];
    PadData.vibrationStatus = SaveData[SAVE_OPTION_VIBRATION];
    tyaDisplaySetting(0);
    xglHddActivate(SaveData[SAVE_OPTION_HDD] ^ 1);
}

void InitCfSystem(void)
{
    u32 flags;
    int *vm_flag;
    int i;
    float range;

    if (!(GameLoopState.current_event_id & 0xf0000000)) {
        xglFontLoad(1, 0);
    } else {
        xglFontLoad(0, 0);
    }
    Vibration_Stop();
    xglPadRead();
    SoundWork.packet_count = 0u;
    memset(SoundWork.packets, 0, sizeof(SoundWork.packets));
    EnemySound_StopAll(1u);
    UwamonoBgmFadeOut();
    ACT_init();
    MAP_initUnit();
    GameCameraReset();
    range = 3.0f;
    GameLoopState.shoot_control = GameLoopState.shoot_control | 0xfu;
    flags = GameLoopState.flags;
    flags &= ~GAME_FLAG_ACTOR_LOOK_AT;
    flags &= ~GAME_FLAG_ACTOR_HOLD;
    flags &= ~0x4000u;
    flags &= ~GAME_FLAG_NO_TALK_POINT;
    flags &= ~GAME_FLAG_ACTOR_PAUSE;
    flags &= ~GAME_FLAG_ON_EVENT_SYMBOL;
    flags &= ~GAME_FLAG_ON_RETURN_SYMBOL;
    flags &= ~GAME_FLAG_ON_SAVE_SYMBOL;
    flags &= ~GAME_FLAG_ON_SHOP_SYMBOL;
    flags &= ~GAME_FLAG_TALKING;
    flags &= ~0x2000u;
    flags &= ~GAME_FLAG_ENTERED_SCRIPT;
    flags &= ~GAME_FLAG_PAUSE;
    GameLoopState.base_flags = flags;
    GameLoopState.shoot_range = range;
    GameLoopState.flags = flags | GAME_FLAG_RADAR;
    vm_flag = &UseVMFlag;
    *vm_flag = 0;
    XTK_setWindowOwner(0u);
    ResetShootSys();
    sefInitEffectCf();
    saveEffe = (void *)0;
    shopEffe = (void *)0;
    evsEffe = (void *)0;
    retEffe = (void *)0;
    xglLightSetDefault(xglStudioGetLight2());
    Player_System_Init(GameLoopState.controlled_actor);
    for (i = 0; i < GAME_ID_LIGHT_COUNT; i++) {
        xglLightSetDefault(&GameIdLight[i]);
    }
    for (i = 0; i < GAME_HIDDEN_PART_WORDS; i++) {
        GameLoopState.hidden_parts[i] = 0;
    }
    xglRenderCopyDisp2Draw();
}

static int checkAttr(Actor *actor)
{
    u64 attributes;
    u64 selected_attributes;

    if (actor == 0) {
        return 0;
    }
    attributes = actor->attribute_flags;
    selected_attributes = attributes & ACTOR_ATTR_ID_MASK;
    if (attributes & ACTOR_ATTR_BLOCK_MASK) {
        return 0;
    }
    selected_attributes >>= ACTOR_ATTR_ID_SHIFT;
    attrPrev = selected_attributes;
    return selected_attributes;
}

int getScriptFlag(SceneObject object)
{
    return ((Actor *)object)->args.locator_flags;
}

static Actor *checkTalk(Actor *player)
{
    int index;
    float best_distance;
    Actor *best_actor;
    Actor *target;
    Vector4 origin;
    float delta_x;
    float delta_z;
    float distance;
    float limit;
    float reach_margin;

    best_actor = 0;
    origin.x = player->position.x;
    origin.y = player->position.y;
    origin.z = player->position.z;
    best_distance = GAME_SEARCH_DISTANCE_INIT;
    origin.w = 0.25f;
    if (GameLoopState.flags & GAME_FLAG_NO_TALK) {
        return 0;
    }
    if (GameLoopState.flags & 0x2000) {
        return 0;
    }
    if (GameLoopState.flags & GAME_FLAG_TALKING) {
        return 0;
    }
    if (UseVMFlag != 0) {
        return 0;
    }
    if (XTK_getWindowOwner() != 0) {
        return 0;
    }
    for (index = 0; index < ACTOR_COUNT; index++) {
        target = &actor[index];
        reach_margin = 1.0f;
        if (!(__builtin_fabsf(target->position.y - player->position.y) > 2.0f)
            && target->talk_message != 0 && (getScriptFlag(target) & SCRIPT_FLAG_TALK)
            && target->inUseId != 0 && target->number != player->number) {
            delta_x = target->position.x - origin.x;
            delta_z = target->position.z - origin.z;
            distance = delta_x * delta_x + delta_z * delta_z;
            limit = __builtin_fabsf(origin.w + reach_margin);
            distance = __builtin_fabsf(distance);
            if (distance < limit
                && __builtin_fabsf(nearDir(atan2f(delta_x, delta_z), player->rotation.y)) < GAME_TALK_FACING_LIMIT
                && distance < best_distance) {
                best_distance = distance;
                best_actor = target;
            }
        }
    }
    return best_actor;
}

static Actor *checkTouch(Actor *player)
{
    int index;
    float best_distance;
    float limit;
    Actor *best_actor;
    Actor *target;
    Vector4 origin;
    u32 flags;

    best_actor = 0;
    origin.x = player->position.x;
    origin.y = player->position.y;
    origin.z = player->position.z;
    origin.w = 0.25f;
    best_distance = GAME_SEARCH_DISTANCE_INIT;
    flags = GameLoopState.flags;
    if (flags & GAME_FLAG_NO_TALK) {
        return 0;
    }
    if (flags & GAME_FLAG_TALKING) {
        return 0;
    }
    if (flags & 0x2000) {
        return 0;
    }
    for (index = 0; index < ACTOR_COUNT; index++) {
        target = actor + index;
        if (!(__builtin_fabsf(target->position.y - player->position.y) > 2.0f)
            && target->touch_message != 0 && (getScriptFlag(target) & SCRIPT_FLAG_TOUCH)
            && target->inUseId != 0 && target->number != player->number) {
            float delta_x;
            float delta_z;
            float distance;
            float reach_margin;

            reach_margin = 0.48999998f;
            delta_x = target->position.x - origin.x;
            delta_z = target->position.z - origin.z;
            distance = delta_x * delta_x;
            distance += delta_z * delta_z;
            distance = __builtin_fabsf(distance);
            limit = __builtin_fabsf(origin.w + reach_margin);
            if (distance < limit && distance < best_distance) {
                best_distance = distance;
                best_actor = target;
            }
        }
    }
    return best_actor;
}

int checkItemBoxDir(Actor *actor, GameMapUnit *unit)
{
    Vector4 start;
    Vector4 end;
    DoublewordAlignedVector4 extent;
    DoublewordAlignedVector4 position;
    Matrix4 *orientation;

    position = unit->position;
    extent = unit->extent;
    orientation = unit->orientation;
    start.x = position.x - (extent.x * (*orientation)[0][0] + extent.z * (*orientation)[2][0]);
    start.y = position.y;
    start.z = position.z - (extent.x * (*orientation)[0][2] + extent.z * (*orientation)[2][2]);
    end.x = position.x + (extent.z * (*orientation)[0][2] + extent.x * (*orientation)[2][2]);
    end.y = position.y;
    end.z = position.z - (extent.z * (*orientation)[0][0] + extent.x * (*orientation)[2][0]);
    return CheckPointLine(&start, &end, &actor->position) == 2;
}

static GameMapUnit *checkItemBox(Actor *actor)
{
    int index;
    float best_distance;
    GameMapUnit *best_unit;
    GameMapUnit *unit;
    Vector4 origin;
    float delta_x;
    float delta_z;
    float extent_x;
    float extent_z;
    float distance;
    float reach_margin;

    best_distance = GAME_SEARCH_DISTANCE_INIT;
    best_unit = 0;
    origin.x = actor->position.x;
    origin.y = actor->position.y;
    origin.z = actor->position.z;
    for (index = 0; index < MAP_UNIT_COUNT; index++) {
        unit = (GameMapUnit *)&MapUnit[index];
        if (!(__builtin_fabsf(actor->position.y - unit->position.y) > 1.8f) && unit->serial != -1
            && (unit->flags & MAPUNIT_FLAG_ITEM_BOX) && unit->action_sub == 0) {
            reach_margin = 1.0f;
            delta_x = unit->position.x - origin.x;
            delta_z = unit->position.z - origin.z;
            extent_x = unit->extent.x;
            extent_z = unit->extent.z;
            distance = delta_x * delta_x + delta_z * delta_z;
            if (extent_z < extent_x) {
                origin.w = extent_x * extent_x;
            } else {
                origin.w = extent_z * extent_z;
            }
            distance = __builtin_fabsf(distance);
            if (distance < __builtin_fabsf(origin.w + reach_margin)
                && __builtin_fabsf(nearDir(atan2f(delta_x, delta_z), actor->rotation.y)) < GAME_TALK_FACING_LIMIT
                && distance < best_distance && checkItemBoxDir(actor, unit) != 0) {
                best_distance = distance;
                best_unit = unit;
            }
        }
    }
    return best_unit;
}

void CheckGameSymbol(Actor *player)
{
    int save_unit;
    int shop_unit;
    int event_unit;
    int return_unit;
    int index;
    s16 *symbol_kind;
    MapUnitRecord *unit;

    save_unit = -1;
    shop_unit = -1;
    event_unit = -1;
    return_unit = -1;
    for (index = 0; index < MAP_UNIT_COUNT; index++) {
        unit = &MapUnit[index];
        if (unit->serial == -1 || (unit->flags & MAPUNIT_FLAG_SYMBOL) == 0)
            continue;
        {
            symbol_kind = GameSymbolKindAddress(index);
            if ((unit->flags & MAPUNIT_FLAG_SAVE_SYMBOL) &&
                CheckDist3D(&player->position, &MapUnit[index].position) < SYMBOL_TRIGGER_RADIUS)
                save_unit = index;
            if ((MapUnit[index].flags & MAPUNIT_FLAG_SHOP_SYMBOL) &&
                CheckDist3D(&player->position, &MapUnit[index].position) < SYMBOL_TRIGGER_RADIUS)
                shop_unit = index;
            if (*symbol_kind == SYMBOL_KIND_EVENT &&
                CheckDist3D(&player->position, GameSymbolPosition(index)) < SYMBOL_TRIGGER_RADIUS)
                event_unit = index;
            if (*symbol_kind == SYMBOL_KIND_RETURN &&
                CheckDist3D(&player->position, GameSymbolPosition(index)) < SYMBOL_TRIGGER_RADIUS)
                return_unit = index;
        }
    }

    if (save_unit != -1) {
        MapUnitRecord *save_symbol = &MapUnit[save_unit];
        if ((GameLoopState.flags & GAME_FLAG_ON_SAVE_SYMBOL) == 0) {
            if (saveEffe != 0)
                sefDeleteEffectCf(saveEffe);
            saveEffe = sefCreateEffectCf(SYMBOL_EFFECT_SAVE_EVENT, &save_symbol->position, 0);
            xglSoundEffectNormalID(GAME_SE_SYMBOL, 0);
        }
        GameLoopState.flags |= GAME_FLAG_ON_SAVE_SYMBOL;
    } else {
        GameLoopState.flags &= ~GAME_FLAG_ON_SAVE_SYMBOL;
        if (saveEffe != 0) {
            sefDeleteEffectCf(saveEffe);
            saveEffe = 0;
        }
    }
    if (shop_unit != -1) {
        MapUnitRecord *shop_symbol = &MapUnit[shop_unit];
        shopNo = shop_symbol->shop_id;
        if ((GameLoopState.flags & GAME_FLAG_ON_SHOP_SYMBOL) == 0) {
            if (shopEffe != 0)
                sefDeleteEffectCf(shopEffe);
            shopEffe = sefCreateEffectCf(SYMBOL_EFFECT_SHOP_RETURN, &shop_symbol->position, 0);
            xglSoundEffectNormalID(GAME_SE_SYMBOL, 0);
        }
        GameLoopState.flags |= GAME_FLAG_ON_SHOP_SYMBOL;
    } else {
        GameLoopState.flags &= ~GAME_FLAG_ON_SHOP_SYMBOL;
        if (shopEffe != 0) {
            sefDeleteEffectCf(shopEffe);
            shopEffe = 0;
        }
    }
    if (event_unit != -1) {
        MapUnitRecord *event_symbol = &MapUnit[event_unit];
        if ((GameLoopState.flags & GAME_FLAG_ON_EVENT_SYMBOL) == 0) {
            if (evsEffe != 0)
                sefDeleteEffectCf(evsEffe);
            evsEffe = sefCreateEffectCf(SYMBOL_EFFECT_SAVE_EVENT, &event_symbol->position, 0);
            xglSoundEffectNormalID(GAME_SE_SYMBOL, 0);
        }
        GameLoopState.flags |= GAME_FLAG_ON_EVENT_SYMBOL;
    } else {
        GameLoopState.flags &= ~GAME_FLAG_ON_EVENT_SYMBOL;
        if (evsEffe != 0) {
            sefDeleteEffectCf(evsEffe);
            evsEffe = 0;
        }
    }
    if (return_unit != -1) {
        MapUnitRecord *return_symbol = &MapUnit[return_unit];
        if ((GameLoopState.flags & GAME_FLAG_ON_RETURN_SYMBOL) == 0) {
            if (retEffe != 0)
                sefDeleteEffectCf(retEffe);
            retEffe = sefCreateEffectCf(SYMBOL_EFFECT_SHOP_RETURN, &return_symbol->position, 0);
            xglSoundEffectNormalID(GAME_SE_SYMBOL, 0);
        }
        GameLoopState.flags |= GAME_FLAG_ON_RETURN_SYMBOL;
    } else {
        GameLoopState.flags &= ~GAME_FLAG_ON_RETURN_SYMBOL;
        if (retEffe != 0) {
            sefDeleteEffectCf(retEffe);
            retEffe = 0;
        }
    }
}

static void GameDispTag(void)
{
    Matrix4 matrix;
    GameProjectedPoint projected;
    Vector4 origin;
    int screen_x;
    int screen_y;
    int depth;
    int camera_id;
    int index;
    unsigned int color;
    char *format;
    char *map_format;

    origin.x = 0.0f;
    origin.y = 0.0f;
    origin.z = 0.0f;
    origin.w = 1.0f;
    for (camera_id = 0; camera_id < STUDIO_CAMERA_COUNT; camera_id++) {
        if (xglStudioGetCamera2(camera_id)->active == 1)
            break;
    }
    format = str_0;
    for (index = 0; index < ACTOR_COUNT; index++) {
        Actor *unit = &actor[index];
        Actor *parent;

        if (unit->inUseId == 0)
            continue;
        parent = unit->parent;
        if ((unit->inUseId & ID_CLASS_MASK) == 0 && parent == 0) {
            Matrix4 *model_matrices = unit->jointMatrices;
            xglMatrixMul((Matrix *)matrix, (Matrix *)model_matrices[0], (Matrix *)model_matrices[45]);
            matrix[3][3] = 1.0f;
            xglRotTransPers((Vector4 *)&projected, 0, (Vector4 *)matrix[3], camera_id);
        } else {
            if (parent != 0 && (parent->inUseId & ID_CLASS_MASK) == 0) {
                Matrix4 *parent_matrices = parent->jointMatrices;
                xglMatrixMul((Matrix *)matrix, (Matrix *)parent_matrices[0], (Matrix *)parent_matrices[45]);
            } else {
                Matrix4 *root_matrix = unit->jointMatrices;
                /* The original copies these four complete matrix rows
                 * with the SDK quadword idiom. The scratch is confined
                 * to this block; both bases are genuine C objects. */
                QuadwordCopyMatrix(matrix, root_matrix);
            }
            matrix[3][3] = 1.0f;
            xglRotTransPers((Vector4 *)&projected, 0, (Vector4 *)matrix[3], camera_id);
        }
        screen_x = projected.x;
        /* Unsigned subtraction preserves the original SLTIU window
         * without signed overflow for arbitrary projected coordinates. */
        if ((unsigned int)screen_x - GAME_GS_X_MIN >= GAME_GS_X_RANGE ||
            projected.y < GAME_GS_Y_MIN || projected.y > GAME_GS_Y_MAX ||
            projected.z <= 0)
            continue;
        screen_y = projected.y;
        depth = projected.z;
        color = 0xffffff;
        if (unit->kind == 1) {
            color = 0xff9933;
        } else {
            switch (unit->inUseId & ID_CLASS_MASK) {
            case 0: color = 0x3399ff; break;
            case 0x4000: color = 0x33ff33; break;
            case 0x2000: color = 0xff3333; break;
            }
        }
        format[1] = color >> 16;
        format[2] = color >> 8;
        format[3] = color;
        xglFontDebugPrintf(((screen_x >> 4) - GAME_GS_ORIGIN_X) >> 1,
                           ((screen_y >> 4) - GAME_GS_ORIGIN_Y) >> 1,
                           format, index + 0x200, depth);
    }
    map_format = str_1;
    for (index = 0; index < MAP_UNIT_COUNT; index++) {
        MapUnitRecord *unit = &MapUnit[index];
        if (unit->serial < 0)
            continue;
        xglMatrixStackUnit();
        xglMatrixStackScale(&unit->scale.x);
        xglMatrixStackTrans(&unit->position.x);
        xglMatrixStackRotX(unit->rotation.x);
        xglMatrixStackRotY(unit->rotation.y);
        xglMatrixStackRotZ(unit->rotation.z);
        xglMatrixStackSave(matrix);
        xglRotTransPers((Vector4 *)&projected, matrix, &origin, camera_id);
        screen_x = projected.x;
        if ((unsigned int)screen_x - GAME_GS_X_MIN >= GAME_GS_X_RANGE ||
            projected.y < GAME_GS_Y_MIN || projected.y > GAME_GS_Y_MAX ||
            projected.z <= 0)
            continue;
        screen_y = projected.y;
        depth = projected.z;
        color = 0xffffff;
        switch (unit->serial & ID_CLASS_MASK) {
        case 0x5000: color = 0x7f7f7f; break;
        case 0x6000: color = 0xcc66ff; break;
        case 0x3000: color = 0xffff00; break;
        }
        map_format[1] = color >> 16;
        map_format[2] = color >> 8;
        map_format[3] = color;
        xglFontDebugPrintf(((screen_x >> 4) - GAME_GS_ORIGIN_X) >> 1,
                           ((screen_y >> 4) - GAME_GS_ORIGIN_Y) >> 1,
                           dispTagFormat, index + 0x300, depth);
    }
}

void GameDrawShadow(void)
{
    Actor *unit;
    int i;

    unit = actor;
    ACT_DrawShadowBegin();
    for (i = ACTOR_COUNT - 1; i >= 0; i--, unit++) {
        if (unit->inUseId != 0) {
            if (!(unit->flags & ACTOR_FLAG_NO_SHADOW)) {
                if (unit->flags & ACTOR_FLAG_SHADOW) {
                    ACT_DrawShadow(unit);
                }
            }
        }
    }
    ACT_DrawShadowEnd();
}

void GameDrawSync(void)
{
    VibrationEngine();
    xglStudioMainCameraMove();
    xglStudioMove();
    if (GameLoopState.frame_hook != 0) {
        GameLoopState.frame_hook(GAME_HOOK_DRAW_BEGIN);
    }
    if (GameLoopState.flags & GAME_FLAG_DRAW) {
        if (GameLoopState.clear_callback != 0) {
            GameLoopState.clear_callback();
        } else {
            TestEnv_2_00338620.red = ClearEnv.red;
            TestEnv_2_00338620.green = ClearEnv.green;
            TestEnv_2_00338620.blue = ClearEnv.blue;
            TestEnv_2_00338620.alpha = GAME_CLEAR_ALPHA;
            sceVif1PkRef(xglPacketGetCurrent(), &TestEnv_2_00338620, 6, 0, 0, 0);
        }
        TWSYS_draw();
        if (GameLoopState.map_draw_callback != 0) {
            GameLoopState.map_draw_callback();
        } else {
            MapDraw();
        }
        if (GameLoopState.actor_draw_callback != 0) {
            GameLoopState.actor_draw_callback();
        } else {
            ACT_draw();
        }
        if (GameLoopState.shadow_draw_callback != 0) {
            GameLoopState.shadow_draw_callback();
        } else {
            GameDrawShadow();
        }
        if (GameLoopState.unit_draw_callback != 0) {
            GameLoopState.unit_draw_callback();
        } else {
            MAP_drawUnit();
        }
        if (!(GameLoopState.flags_wide & (GAME_FLAG_UMN | GAME_FLAG_PAUSE))) {
            sefExecEffect();
        }
        GameMovieMain();
        xglSoundStreamMain();
        if (GameLoopState.flags & GAME_FLAG_DISP_TAG) {
            GameDispTag();
        }
    }
    if (GameLoopState.frame_hook != 0) {
        GameLoopState.frame_hook(GAME_HOOK_DRAW_END);
    }
    GameSnapShotCheck();
    StudioEntrySkip = 1;
    xglSleep();
    nmlModelSetGlobalFogReset();
    StudioEntrySkip = 0;
    if (GameLoopState.frame_countdown > 0) {
        GameLoopState.frame_countdown--;
    }
    if (GameLoopState.frame_hook != 0) {
        GameLoopState.frame_hook(GAME_HOOK_FRAME_END);
    }
}

static int GameModeEvtTools(void)
{
    if (game_mode_status == 0) {
        if ((PadData.pressed & PAD_START) != 0) {
            u16 buttons = PadData.held;
            game_mode_status = GAME_STATUS_LOCK_SHORT;

            if ((buttons & GAME_PAD_COMBO) == GAME_PAD_COMBO) {
                u32 flag_next = game_mode_flags | GAME_FLAG_QUIT;
                game_mode_flags = flag_next;
                return 1;
            } else {
                xglSoundEffectNormalDirect(GAME_SE_PAUSE);
                game_mode_flags &= ~GAME_FLAG_PAUSE;
                game_mode_debug_storage = 0;
                game_mode_kind = game_mode_saved_kind;
            }
        }
    }

    TWSYS_update();
    EvtTools();
    {
        StudioCamera *camera = xglStudioGetCamera2(0);
        if (camera->state == STUDIO_CAMERA_STATE_GAME && game_mode_camera_callback != 0) {
            game_mode_camera_callback();
        }
    }
    return 0;
}

static int GameModePause(void)
{
    if (game_mode_status == 0) {
        if ((PadData.pressed & PAD_START) != 0) {
            u16 buttons = PadData.held;
            game_mode_status = GAME_STATUS_LOCK_SHORT;

            if ((buttons & GAME_PAD_COMBO) == GAME_PAD_COMBO) {
                u32 flag_next = game_mode_flags | GAME_FLAG_QUIT;
                game_mode_flags = flag_next;
                return 1;
            } else {
                xglSoundEffectNormalDirect(GAME_SE_PAUSE);
                game_mode_flags &= ~GAME_FLAG_PAUSE;
                game_mode_debug_storage = 0;
                game_mode_kind = game_mode_saved_kind;
                PartyTimePauseEnd();
            }
        }
    }

    TWSYS_update();
    PauseMenu();
    if (game_mode_saved_kind == GAME_MODE_CF_MAIN || game_mode_saved_kind == GAME_MODE_CF_EVENT) {
        ACT_pauseUpdate();
    }
    {
        StudioCamera *camera = xglStudioGetCamera2(0);
        if (camera->state == STUDIO_CAMERA_STATE_GAME && game_mode_camera_callback != 0) {
            game_mode_camera_callback();
        }
    }
    return 0;
}

static int GameModeEvtDebug(void)
{
    if (game_mode_status == 0) {
        if ((PadData.pressed & PAD_START) != 0) {
            u16 buttons = PadData.held;
            game_mode_status = GAME_STATUS_LOCK_SHORT;

            if ((buttons & GAME_PAD_COMBO) == GAME_PAD_COMBO) {
                u32 flag_next = game_mode_flags | GAME_FLAG_QUIT;
                game_mode_flags = flag_next;
                return 1;
            } else {
                xglSoundEffectNormalDirect(GAME_SE_PAUSE);
                game_mode_flags &= ~GAME_FLAG_PAUSE;
                game_mode_debug_storage = 0;
                game_mode_kind = game_mode_saved_kind;
            }
        }
    }

    TWSYS_update();
    {
        StudioCamera *camera = xglStudioGetCamera2(0);
        if (camera->state == STUDIO_CAMERA_STATE_GAME && game_mode_camera_callback != 0) {
            game_mode_camera_callback();
        }
    }
    return 0;
}

int GameModeCfMain(void)
{
    VMSlot umn_arguments[2];
    VMSlot drill_arguments[2];
    Actor *player;
    Actor *window_owner;
    EnemyWorkPostTalk *encounter_enemy;
    int battle_result;
    int attribute;
    Actor *touch;
    short i;
    short index;
    int countdown;
    u32 flags;
    u16 previous_kind;

    GameLoopState.flags &= ~GAME_FLAG_PAUSE;
    SCRIPT_incCfTime();
    if (PadData.pressed & (PAD_START | PAD_TRIANGLE)) {
        const u64 *button_words;
        u64 held_buttons;

        button_words = (const u64 *)&PadData.held;
        __builtin_memcpy(&held_buttons, button_words, sizeof(held_buttons));
        /* The 64-bit read spans held (+0x28) and pressed (+0x2a): the combo
         * held while START is pressed. */
        if ((held_buttons & ((PAD_START << 16) | GAME_PAD_COMBO)) == ((PAD_START << 16) | GAME_PAD_COMBO)) {
            GameLoopState.flags |= GAME_FLAG_QUIT;
            return 1;
        }
        if (!(GameLoopState.flags_wide & (GAME_FLAG_NO_TALK_POINT | GAME_FLAG_ACTOR_LOOK_AT | GAME_FLAG_ACTOR_HOLD | GAME_FLAG_SCRIPT_END)) && WIN_checkActiveWindow() == 0
            && SCRIPT_sceneChangeTimeGet() == 0 && !(GameLoopState.flags & GAME_FLAG_ENTERED_SCRIPT)
            && GameLoopState.controlled_actor != 0
            && (encounter_enemy = &enepc[GameLoopState.controlled_actor->number])->encount_lock == 0
            && xglCdSync() == 0 && SCRIPT_talkIgnoreGet() == 0 && GameLoopState.status == 0) {
            if (PadData.pressed & PAD_START) {
                GameLoopState.status = GAME_STATUS_LOCK_SHORT;
                xglSoundEffectNormalDirect(GAME_SE_PAUSE);
                previous_kind = GameLoopState.kind;
                GameLoopState.flags |= GAME_FLAG_PAUSE;
                GameLoopState.kind = GAME_MODE_PAUSE;
                GameLoopState.saved_kind = previous_kind;
                GameLoopState.debug_menu.storage = 0;
                PartyTimePauseStart();
                return 0;
            }
            else if (PadData.pressed & PAD_TRIANGLE) {
                GameLoopState.status = GAME_STATUS_LOCK_LONG;
                GameStateSaveCameraLight();
                nmlModelSendMenuStart();
                xglCullingIgnore();
                MainMenu();
                xglCullingIgnoreOff();
                nmlModelSendMenuEnd();
                GameStateRestoreCameraLight();
                if (GameLoopState.flags & GAME_FLAG_EXIT) {
                    return 1;
                }
                if (UmnSimulationNo) {
                    const u64 *position_tail;
                    const u64 *position_words;
                    u32 event_id;

                    event_id = GameLoopState.current_event_id;
                    GameLoopState.user_word = event_id;
                    position_words = (const u64 *)&GameLoopState.controlled_actor->position;
                    GameLoopState.user_longs[0] = *position_words;
                    position_tail = (const u64 *)
                        ((const unsigned char *)position_words + sizeof(u64));
                    GameLoopState.user_longs[1] = *position_tail;
                    umn_arguments[0].i = UmnSimulationNo & 0xFFFF;
                    umn_arguments[1].i = (u32)UmnSimulationNo >> 16;
                    Java_xeno_util_Runtime_jumpCF__II(stageVM, umn_arguments, 0);
                    return 0;
                }
                if (MenuDrillCall != 0) {
                    const u64 *position_tail;
                    const u64 *position_words;
                    u32 event_id;

                    event_id = GameLoopState.current_event_id;
                    GameLoopState.user_word = event_id;
                    position_words = (const u64 *)&GameLoopState.controlled_actor->position;
                    GameLoopState.user_longs[0] = *position_words;
                    position_tail = (const u64 *)
                        ((const unsigned char *)position_words + sizeof(u64));
                    GameLoopState.user_longs[1] = *position_tail;
                    drill_arguments[0].i = 0xA0;
                    drill_arguments[1].i = 1;
                    Java_xeno_util_Runtime_jumpCF__II(stageVM, drill_arguments, 0);
                    return 0;
                }
            }
        }
    }
    GameLoopState.flags = GameLoopState.flags & ~GAME_FLAG_SCRIPT_END;
    if (GameLoopState.event_sub_id != 0) {
        const char *always_method;
        EnemyWorkPostTalk *battle_enemies;

        Game_Data_Push();
        GameLoopState.kind = GAME_MODE_BATTLE;
        nmlModelSendSignalStartBattle();
        xglSoundStreamStop(0);
        Vibration_Stop();
        PadData.second_actuator[0] = 0;
        PadData.actuator[0] = 0;
        func_A22668();
        GameLoopState.frame_countdown = 0;
        GameLoopState.flags |= GAME_FLAG_BATTLE;
        MSG_init();
        TWSYS_init();
        Vibration_Stop();
        GameLoopState.model = 0;
        if (GameLoopState.flags & GAME_FLAG_EXIT) {
            return 1;
        }
        battle_result = func_00A0B5F0();
        if (battle_result == BATTLE_RESULT_LOSE && !(GameLoopState.battle_flags & 0x40)) {
            xglFontLoad(1, 0);
            WindowTexLoad(0, 0);
            GameLoopState.flags |= GAME_FLAG_QUIT | GAME_FLAG_GAME_OVER;
            return 1;
        }
        Game_Data_Pop();
        xglFontLoad(1, 0);
        WindowTexLoad(0, 0);
        always_method = eobAlwaysMethodName;
        battle_enemies = enepc;
        CallMethod_I(always_method,
            battle_enemies[GameLoopState.battle_enemy->number].enemy_id);
        switch (battle_result) {
        case BATTLE_RESULT_WIN:
            Enemy_After_Battle(BATTLE_RESULT_WIN);
            CallMethod_I(eobMethodName,
                battle_enemies[GameLoopState.battle_enemy->number].enemy_id);
            break;
        case BATTLE_RESULT_ESCAPE:
            Enemy_After_Battle(BATTLE_RESULT_ESCAPE);
            CallMethod_I(eobEscapeMethodName,
                battle_enemies[GameLoopState.battle_enemy->number].enemy_id);
            break;
        default:
            Enemy_After_Battle(BATTLE_RESULT_WIN);
            break;
        }
        if (GameLoopState.kind == GAME_MODE_CF_EVENT) {
            return 0;
        }
        GameLoopState.controlled_actor->flash_timer = GAME_BATTLE_RETURN_FLASH_FRAMES;
        Actor_LookAt_Init(actor);
        MAP_updateUnit();
        return 0;
    }
    if (FLAG_FRAME_60 != 0) {
        if ((PadData.held & PAD_L2) && (PadData.pressed & PAD_L1)) {
            for (i = 1; i < ACTOR_COUNT; i++) {
                index = (NowTester + i) % ACTOR_COUNT;
                if (actor[index].inUseId > 0) {
                    NowTester = index;
                    GameLoopState.controlled_actor = &actor[NowTester];
                    break;
                }
            }
        }
    }
    /* base_flags (the high word) still has LOOK_AT but flags no longer does. */
    if ((GameLoopState.flags_wide & (((u64)GAME_FLAG_ACTOR_LOOK_AT << 32) | GAME_FLAG_ACTOR_LOOK_AT)) == ((u64)GAME_FLAG_ACTOR_LOOK_AT << 32)) {
        u32 previous_flags;
        u16 previous_status;
        float restored_facing;

        player = GameLoopState.controlled_actor;
        actSequenceClear(player);
        previous_flags = GameLoopState.flags;
        previous_status = player->status_flags;
        restored_facing = player->rotation.y;
        GameLoopState.flags = previous_flags & ~0x4000;
        player->status_flags = previous_status | 0x1000;
        player->target_angle = restored_facing;
    }
    if ((GameLoopState.flags_wide & (((u64)GAME_FLAG_ACTOR_LOOK_AT << 32) | GAME_FLAG_ACTOR_LOOK_AT)) == GAME_FLAG_ACTOR_LOOK_AT) {
        Actor *motion_player;

        motion_player = GameLoopState.controlled_actor;
        motion_player->status_flags |= 0x1000;
        ACT_setMotion2(motion_player, 0, 9);
    }
    flags = GameLoopState.flags;
    player = GameLoopState.controlled_actor;
    GameLoopState.base_flags = flags;
    if (!(flags & GAME_FLAG_ACTOR_HOLD)) {
        if (!(flags & GAME_FLAG_ACTOR_LOOK_AT)) {
            GameCfPlayerMove();
            if (!(GameLoopState.flags & GAME_FLAG_NO_TALK_POINT)) {
                checkTalkPoint();
            }
        } else {
            if (player->look_at_timer == 2) {
                Actor_LookAt(player);
            } else {
                Actor_LookAt_Release(player, 0);
            }
            ACT_updateSequence(player);
            GameIdLightSet(player, 0);
        }
    } else {
        GameIdLightSet(player, 0);
    }
    attribute = checkAttr(GameLoopState.controlled_actor);
    if (attribute != 0) {
        SCRIPT_execEntered(attribute - 1);
    }
    window_owner = XTK_getWindowOwner();
    if (window_owner != 0) {
        if (getScriptFlag(window_owner) & SCRIPT_FLAG_FACE_PLAYER) {
            window_owner->target_angle = Get_Angle(&window_owner->position, &GameLoopState.controlled_actor->position);
            window_owner->rotation.y = Get_Cursol_by_Reduce_Speed_Angle_Loop(window_owner->rotation.y, window_owner->target_angle, 6.0f);
        }
    }
    touch = checkTouch(GameLoopState.controlled_actor);
    if (touch != 0) {
        if (touch != prevTouch) {
            prevTouch = touch;
            SCRIPT_execTouchto(touch);
        }
    } else {
        prevTouch = 0;
    }
    CheckGameSymbol(GameLoopState.controlled_actor);
    TWSYS_update();
    JTHREAD_cntl();
    TCAMERA_update();
    if (xglStudioGetCamera2(0)->state == STUDIO_CAMERA_STATE_GAME &&GameLoopState.camera_callback != 0) {
        GameLoopState.camera_callback();
    }
    if ((GameLoopState.flags_wide & (GAME_FLAG_ACTOR_HOLD | GAME_FLAG_ACTOR_PAUSE)) == 0) {
        ACT_update();
    } else {
        for (countdown = GAME_ENEMY_WORK_COUNT - 1; countdown > -1; countdown--) {
            int first_countdown = enepc[GAME_ENEMY_WORK_COUNT - 1 - countdown].stiff_timer;
            int second_countdown = (u16)enepc[GAME_ENEMY_WORK_COUNT - 1 - countdown].effect_timer[0] - 1;
            first_countdown -= 1;
            enepc[GAME_ENEMY_WORK_COUNT - 1 - countdown].effect_timer[0] = second_countdown;
            enepc[GAME_ENEMY_WORK_COUNT - 1 - countdown].stiff_timer = first_countdown;
            if ((short)first_countdown < 0) {
                enepc[GAME_ENEMY_WORK_COUNT - 1 - countdown].stiff_timer = 0;
            }
            if (enepc[GAME_ENEMY_WORK_COUNT - 1 - countdown].effect_timer[0] < 0) {
                enepc[GAME_ENEMY_WORK_COUNT - 1 - countdown].effect_timer[0] = 0;
            }
        }
        ACT_pauseUpdate();
    }
    MAP_updateUnit();
    {
    GameRadarParams radar;
    radar.x = GAME_RADAR_X;
    radar.color = GAME_RADAR_COLOR;
    radar.y = GAME_RADAR_Y;
    radar.red = radar.green = radar.blue = GAME_RADAR_RGB;
    radar.alpha = GAME_RADAR_ALPHA;
    GameRadarDraw(&radar);
    }
    return 0;
}

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
const char eobMethodName[4] = "EOB";

void checkTalkPoint(void)
{
    Actor *talker;
    GameMapUnit *unit;

    if (PadData.pressed & PAD_CIRCLE) {
        if (GameLoopState.status == 0 && xglCdSync() == 0) {
            if (GameLoopState.flags_wide & (GAME_FLAG_ON_SAVE_SYMBOL | GAME_FLAG_ON_EVENT_SYMBOL)) {
                GameStateSave();
                MenuFileMain(0);
                GameLoopState.status = GAME_STATUS_LOCK_LONG;
                return;
            }
            if (GameLoopState.flags & GAME_FLAG_ON_RETURN_SYMBOL) {
                CallMethod("evsExit");
                GameLoopState.status = GAME_STATUS_LOCK_LONG;
                return;
            }
            if (GameLoopState.flags & GAME_FLAG_ON_SHOP_SYMBOL) {
                if (shopNo == -1) {
                    shopNo = 1;
                }
                GameStateSaveCameraLight();
                MenuShopMain(shopNo);
                GameStateRestoreCameraLight();
                GameLoopState.status = GAME_STATUS_LOCK_LONG;
                return;
            }
        }
        talker = checkTalk(GameLoopState.controlled_actor);
        if (talker != 0) {
            EnemyWorkPostTalk *enemy = &enepc[talker->number];
            enemy->post_talk_flags = 0;
            SCRIPT_execTalkto(talker);
        } else {
            unit = checkItemBox(GameLoopState.controlled_actor);
            if (unit != 0) {
                unit->item_box_hit = 1;
            }
        }
    }
}

void actSequenceClear(Actor *actor)
{
    ActSequenceEntry *sequence = &actSequence[actor->number];

    actor->flags &= ~2;
    actor->animation.slot.speed = 0.033333335f;
    sequence->state.flags = 0;
    sequence->state.state_flags = 0;
    sequence->state.cleared_on_init = 0;
    sequence->state.cleared_on_init_run[0] = 0;
    sequence->state.cleared_on_init_run[1] = 0;
    sequence->state.cleared_on_init_run[2] = 0;
    sequence->state.cleared_on_init_run[3] = 0;
    sequence->state.handler[0] = 0;
    sequence->state.handler[1] = 0;
    sequence->state.handler[2] = 0;
    sequence->state.handler[3] = 0;
    actor->animation.slot.flags = 8;
}

void actTalkAfter(SceneObject object)
{
    EnemyWorkPostTalk *enemy = &enepc[((Actor *)object)->number];

    After_Talk(object);
    enemy->post_talk_flags = GameLoopState.flags & GAME_FLAG_NO_TALK;
}

static int GameModeCfEvent(void)
{
    int cleared = GameLoopState.flags & -2;
    u8 event_active = GameLoopState.status;

    GameLoopState.flags = cleared;
    if (event_active == 0 && (PadData.pressed & PAD_START) != 0 &&
        (PadData.held & GAME_PAD_COMBO) == GAME_PAD_COMBO) {
        GameLoopState.flags = cleared | GAME_FLAG_QUIT;
        return 1;
    }

    TWSYS_update();
    JTHREAD_cntl();
    PLAY_ctrl();
    TCAMERA_update();
    ACT_update();
    MAP_updateUnit();
    return 0;
}

int GameModeDebugMenu(void)
{
    if ((PadData.debug_buttons & PAD_SELECT) != 0) {
        GameLoopState.debug_menu.storage = 0;
        GameLoopState.flags &= ~GAME_FLAG_PAUSE;
        GameLoopState.kind = GameLoopState.saved_kind;
    }

    TWSYS_update();
    ACT_pauseUpdate();
    GameDebugMenu();
    {
        StudioCamera *camera = xglStudioGetCamera2(0);
        if (camera->state == STUDIO_CAMERA_STATE_GAME && GameLoopState.camera_callback != 0) {
            GameLoopState.camera_callback();
        }
    }
    return 0;
}

void Game(void)
{
    int i;
    Actor *player;
    float zero = 0.0f;
    float one = 1.0f;
    void *work_end = WorkEnd;
    short *pause_flag = &pause_check;

    GameLoopState.map_number = (s16)-1;
    GameLoopState.saved_work_end = work_end;
    GameLoopState.frame_hook = (void *)0;
    GameLoopState.model = 0;
    GameLoopState.stateB0 = 0;
    GameLoopState.vector80[3] = one;
    GameLoopState.vector80[2] = one;
    GameLoopState.vector80[1] = one;
    GameLoopState.vector90[2] = zero;
    GameLoopState.vector90[1] = zero;
    GameLoopState.vector90[0] = zero;
    GameLoopState.vector90[3] = 30.0f;
    GameLoopState.vectorA0[2] = zero;
    GameLoopState.vectorA0[1] = zero;
    GameLoopState.state5c = 0;
    GameLoopState.state58 = 0;
    GameLoopState.frame_countdown = 0;
    GameLoopState.camera_callback = 0;
    GameLoopState.clear_callback = 0;
    GameLoopState.map_draw_callback = 0;
    *pause_flag = 0;
    GameLoopState.kind = GAME_MODE_INIT;
    GameLoopState.stateC8 = 0;
    GameLoopState.actor_draw_callback = 0;
    GameLoopState.vector80[0] = one;
    GameLoopState.vectorA0[0] = zero;
    GameLoopState.vectorA0[3] = 30.0f;
    GameLoopState.stateD0 = -2;
    GameLoopState.flags = GAME_FLAG_DRAW | 0x300;
    nmlModelSetFadeInCancel(30u);
    nmlModelSetEffectWrite(1);
    Enemy_SystemInit();
    InitUwamonoSys();
    xglRenderClearDepth();
    xglRenderGlobalFadeInit();
    xglRenderClearOff();
    xglStudioChange(0);
    xglLightSetDefault(xglStudioGetLight2());
    GameCameraReset();
    GameCFSoundReload();
    GameResourceInit(0x1000000, 0x1000000);
    MSG_init();
    TWSYS_init();
    MapInit();
    MAP_initUnit();
    SCRIPT_sceneChangeTimeInit();
    SCRIPT_clrEventActiveFlag();
    SCRIPT_talkIgnoreClr();
    ACT_init();
    for (i = 0; i < ACTOR_COUNT; i++) {
        actor[i].update = 0;
    }
    GameDefocusSet(-1, 0, 0);
    GameBgDrawType0();
    func_A22628();
    sefInitEffectCf();
    GameLoopState.scheduler = (XglTaskScheduler *)WorkEnd;
    WorkEnd = xglTaskInitial(WorkEnd, 64, 0);
    WorkEnd = SCRIPT_init(WorkEnd);
    GameRadarInit();
    GameCfPlayerMoveInit();
    GameLoopState.controlled_actor = ACT_create(0, 1);
    Player_System_Init(GameLoopState.controlled_actor);
    GameLoopState.status = GAME_STATUS_LOCK_SHORT;
    if (GameLoopState.kindDetail != 0) {
        Quadword *position_dst;

        player = GameLoopState.controlled_actor;
        GamePopSaveDataUser();
        position_dst = (Quadword *)&player->position;
        Java_xeno_util_Runtime_jump_sub(GameLoopState.current_event_id, -1);
        QuadwordCopy(position_dst, &GameLoopState.saved_actor_position);
        GameLoopState.move_mode |= 0xa0;
    } else {
        Java_xeno_util_Runtime_jump_sub(GameLoopState.current_event_id, -1);
    }
    while (1) {
        if (GameLoopState.frame_hook != 0) {
            GameLoopState.frame_hook(GAME_HOOK_FRAME_START);
        }
        nmlModelSendPauseSignal(GameLoopState.flags & GAME_FLAG_PAUSE);
        SCRIPT_sceneChangeTimeDec();
        Disp_Homing();
        if (GameLoopState.status != 0) {
            GameLoopState.status--;
        }
        switch (GameLoopState.kind) {
        case GAME_MODE_INIT:
            GameLoopState.kind = GAME_MODE_CF_MAIN;
            break;
        case GAME_MODE_PAUSE:
            GameModePause();
            break;
        case GAME_MODE_CF_MAIN:
            GameModeCfMain();
            break;
        case GAME_MODE_CF_EVENT:
            GameModeCfEvent();
            break;
        case GAME_MODE_EVT_DEBUG:
            GameModeEvtDebug();
            break;
        case GAME_MODE_EVT_TOOLS:
            GameModeEvtTools();
            break;
        case GAME_MODE_DEBUG_MENU:
            GameModeDebugMenu();
            break;
        }
        if (GameLoopState.flags & GAME_FLAG_LEAVE_LOOP) {
            break;
        }
        xglTaskExecute(GameLoopState.scheduler);
        GameDrawSync();
    }
    xglSoundStreamStop(0);
    GameMovieStop();
    GameDefocusSet(-1, 0, 0);
    sefInitEffectCf();
    xglCdReset();
    xglCullingMapInit();
    GameCFSoundPurge();
    nmlModelSendPauseSignal(0);
    SCRIPT_sceneChangeTimeInit();
    SCRIPT_clrEventActiveFlag();
    SCRIPT_talkIgnoreClr();
    xglRenderGlobalFadeInit();
    xglStudioMainCameraInit();
    nmlModelConstruct();
    xglRenderClearOff();
    WorkEnd = GameLoopState.saved_work_end;
    if (GameLoopState.flags & GAME_FLAG_GAME_OVER) {
        GameOver();
    }
    GameLoopState.kind = GAME_MODE_INIT;
    xglSleep();
    xglSleep();
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
/* Small initialized flags follow the existing evsExit literal above. */
unsigned char SnapDrawCreditFlag = 0;

unsigned char UseTestPath = 0;

const char GameEmptyName[1] = "";

const char GameMcTestName[8] = "MC_TEST";
