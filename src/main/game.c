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
#define ACTOR_COUNT 64

#define ACTOR_IN_USE_ID_OFFSET 0x86

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
    for (; index < 8; index++) {
        xglSoundSendEffect(0, 0, index + 4);
    }
}

static void GameCFSoundPurge(void)
{
    int bank;

    for (bank = 0; bank < 8; bank++) {
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
    for (bank = 0; bank < 8; bank++) {
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
        for (bank = 0; bank < 8; bank++) {
            sequence = SoundWork.channels[bank].sequence;
            if (sequence == 0xFFFF) {
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
    for (bank = 4; bank < 8; bank++) {
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

    for (channel = 0; channel < 8; channel++) {
        if (((GameCFSoundMenuPurgeFlag >> channel) & 1) != 0 &&
            UmnSimulationNo == 0 && MenuDrillCall == 0) {
            xglSoundSequenceNormal2(channel, 0x7F);
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
    for (index = 0; index < 32; index++) {
        GameLoopState.hidden_parts[index] = 0;
    }
    model = GameLoopState.model->data;
    block_offsets = model->blockOffsets;
    for (index = 0; index < model->blockCount; index++) {
        block = (NmlModelBlock *)((u8 *)model + block_offsets[index]);
        if ((block->flags & 8) != 0) {
            int part_index = block->partId;
            int word_index = part_index / 32;
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

    for (index = 0; index < 64; index++) {
        serials[index] = MapUnit[index].serial;
        MapUnit[index].serial = -1;
    }
    while (xglCdSync() != 0)
        xglSleep();
    GameCFSoundReload();
    sefInitEffectCf();
    GameResourceLoad(GameLoopState.current_event_id);
    for (index = 0; index < 64; index++)
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
    GameLoopState.flags &= ~0x400000;
    GameLoopState.flags &= ~0x800000;
    GameLoopState.flags &= ~0x20000000;
    GameLoopState.flags &= ~0x40000000;
    UnlockMapUnit();
    for (index = 0; index < 64; index++) {
        if (MapUnit[index].serial != -1 && (MapUnit[index].flags & 0x20000)) {
            MapUnitRecord *map_unit = &MapUnit[index];
            MapUnit[index].serial = -1;
            MapUnit[index].update = 0;
            ClearUwamonoEffect(map_unit);
        }
    }
    model = GameLoopState.model->data;
    for (index = 0; index < 32; index++) {
        for (bit = 0; bit < 32; bit++) {
            /* Original uses signed SRAV before selecting the one bit.
             * This scalar interpretation does not erase an interface. */
            if (((int)GameLoopState.hidden_parts[index] >> bit) & 1)
                nmlModelSetPartsVisible(model, index * 32 + bit, 0);
        }
    }
    {
        u32 flags = GameLoopState.flags;

        GameLoopState.controlled_actor->status_flags |= 0x1000;
        flags &= ~0x4000;
        flags &= ~0x8000;
        flags &= ~0x40;
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
    __asm__ __volatile__(
        "lq $2, 0(%1)\n"
        "sq $2, 0(%0)\n"
        :
        : "r" (&GameLoopState.saved_actor_position), "r" (&controlled_actor->position)
        : "$2", "memory");
    __asm__ __volatile__(
        "lq $2, 0(%1)\n"
        "sq $2, 0(%0)\n"
        :
        : "r" (&GameLoopState.saved_actor_rotation), "r" (&controlled_actor->rotation)
        : "$2", "memory");
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

    save = (GameUserSave *)(SaveData + 0x50);
    save->event_id = state->current_event_id;
    for (i = 0; i < 8; i++) {
        save->params[i] = state->params[i];
    }
    __asm__ __volatile__(
        "lq $2, 0(%1)\n"
        "sq $2, 0(%0)\n"
        :
        : "r" (&save->actor_position), "r" (&state->saved_actor_position)
        : "$2", "memory");
    __asm__ __volatile__(
        "lq $2, 0(%1)\n"
        "sq $2, 0(%0)\n"
        :
        : "r" (&save->actor_rotation), "r" (&state->saved_actor_rotation)
        : "$2", "memory");
    org = save->org;
    players = save->players;
    for (i = 0; i < 33; i++) {
        org[i] = orgData[i];
    }
    for (i = 0; i < 16; i++) {
        players[i] = plChaData[i];
    }
    for (i = 0; i < 255; i++) {
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

    save = (GameUserSave *)(SaveData + 0x50);
    state->current_event_id = save->event_id;
    for (i = 0; i < 8; i++) {
        state->params[i] = save->params[i];
    }
    __asm__ __volatile__(
        "lq $2, 0(%1)\n"
        "sq $2, 0(%0)\n"
        :
        : "r" (&state->saved_actor_position), "r" (&save->actor_position)
        : "$2", "memory");
    __asm__ __volatile__(
        "lq $2, 0(%1)\n"
        "sq $2, 0(%0)\n"
        :
        : "r" (&state->saved_actor_rotation), "r" (&save->actor_rotation)
        : "$2", "memory");
    org = save->org;
    players = save->players;
    for (i = 0; i < 33; i++) {
        orgData[i] = org[i];
    }
    for (i = 0; i < 16; i++) {
        plChaData[i] = players[i];
    }
    for (i = 0; i < 255; i++) {
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
    SsdSetOutputMode(SaveData[0x28]);
    PadData.vibrationReset = SaveData[0x3c];
    PadData.vibrationStatus = SaveData[0x3c];
    tyaDisplaySetting(0);
    xglHddActivate(SaveData[0x40] ^ 1);
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
    flags &= ~0x10000u;
    flags &= ~0x20000u;
    flags &= ~0x4000u;
    flags &= ~0x8000u;
    flags &= ~0x40000u;
    flags &= ~0x400000u;
    flags &= ~0x800000u;
    flags &= ~0x20000000u;
    flags &= ~0x40000000u;
    flags &= ~0x1000u;
    flags &= ~0x2000u;
    flags &= ~0x100000u;
    flags &= ~1u;
    GameLoopState.base_flags = flags;
    GameLoopState.shoot_range = range;
    GameLoopState.flags = flags | 0x80000u;
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
    for (i = 0; i < 16; i++) {
        xglLightSetDefault(&GameIdLight[i]);
    }
    for (i = 0; i < 32; i++) {
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
    selected_attributes = attributes & 0x1f00;
    if (attributes & 0xe000) {
        return 0;
    }
    selected_attributes >>= 8;
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
    best_distance = 65535.0f;
    origin.w = 0.25f;
    if (GameLoopState.flags & 0x400) {
        return 0;
    }
    if (GameLoopState.flags & 0x2000) {
        return 0;
    }
    if (GameLoopState.flags & 0x1000) {
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
            && target->talk_message != 0 && (getScriptFlag(target) & 0x10000)
            && target->inUseId != 0 && target->number != player->number) {
            delta_x = target->position.x - origin.x;
            delta_z = target->position.z - origin.z;
            distance = delta_x * delta_x + delta_z * delta_z;
            limit = __builtin_fabsf(origin.w + reach_margin);
            distance = __builtin_fabsf(distance);
            if (distance < limit
                && __builtin_fabsf(nearDir(atan2f(delta_x, delta_z), player->rotation.y)) < 1.0471976f
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
    best_distance = 65535.0f;
    flags = GameLoopState.flags;
    if (flags & 0x400) {
        return 0;
    }
    if (flags & 0x1000) {
        return 0;
    }
    if (flags & 0x2000) {
        return 0;
    }
    for (index = 0; index < ACTOR_COUNT; index++) {
        target = actor + index;
        if (!(__builtin_fabsf(target->position.y - player->position.y) > 2.0f)
            && target->touch_message != 0 && (getScriptFlag(target) & 0x20000)
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

    best_distance = 65535.0f;
    best_unit = 0;
    origin.x = actor->position.x;
    origin.y = actor->position.y;
    origin.z = actor->position.z;
    for (index = 0; index < 64; index++) {
        unit = (GameMapUnit *)&MapUnit[index];
        if (!(__builtin_fabsf(actor->position.y - unit->position.y) > 1.8f) && unit->serial != -1
            && (unit->flags & 0x08000000) && unit->action_sub == 0) {
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
                && __builtin_fabsf(nearDir(atan2f(delta_x, delta_z), actor->rotation.y)) < 1.0471976f
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
    for (index = 0; index < 64; index++) {
        unit = &MapUnit[index];
        if (unit->serial == -1 || (unit->flags & 0x10000) == 0)
            continue;
        {
            symbol_kind = GameSymbolKindAddress(index);
            if ((unit->flags & 0x20000000) &&
                CheckDist3D(&player->position, &MapUnit[index].position) < 0.7f)
                save_unit = index;
            if ((MapUnit[index].flags & 0x40000000) &&
                CheckDist3D(&player->position, &MapUnit[index].position) < 0.7f)
                shop_unit = index;
            if (*symbol_kind == 8 &&
                CheckDist3D(&player->position, GameSymbolPosition(index)) < 0.7f)
                event_unit = index;
            if (*symbol_kind == 9 &&
                CheckDist3D(&player->position, GameSymbolPosition(index)) < 0.7f)
                return_unit = index;
        }
    }

    if (save_unit != -1) {
        MapUnitRecord *save_symbol = &MapUnit[save_unit];
        if ((GameLoopState.flags & 0x20000000) == 0) {
            if (saveEffe != 0)
                sefDeleteEffectCf(saveEffe);
            saveEffe = sefCreateEffectCf(0x2d8, &save_symbol->position, 0);
            xglSoundEffectNormalID(12, 0);
        }
        GameLoopState.flags |= 0x20000000;
    } else {
        GameLoopState.flags &= ~0x20000000;
        if (saveEffe != 0) {
            sefDeleteEffectCf(saveEffe);
            saveEffe = 0;
        }
    }
    if (shop_unit != -1) {
        MapUnitRecord *shop_symbol = &MapUnit[shop_unit];
        shopNo = shop_symbol->shop_id;
        if ((GameLoopState.flags & 0x40000000) == 0) {
            if (shopEffe != 0)
                sefDeleteEffectCf(shopEffe);
            shopEffe = sefCreateEffectCf(0x2d9, &shop_symbol->position, 0);
            xglSoundEffectNormalID(12, 0);
        }
        GameLoopState.flags |= 0x40000000;
    } else {
        GameLoopState.flags &= ~0x40000000;
        if (shopEffe != 0) {
            sefDeleteEffectCf(shopEffe);
            shopEffe = 0;
        }
    }
    if (event_unit != -1) {
        MapUnitRecord *event_symbol = &MapUnit[event_unit];
        if ((GameLoopState.flags & 0x400000) == 0) {
            if (evsEffe != 0)
                sefDeleteEffectCf(evsEffe);
            evsEffe = sefCreateEffectCf(0x2d8, &event_symbol->position, 0);
            xglSoundEffectNormalID(12, 0);
        }
        GameLoopState.flags |= 0x400000;
    } else {
        GameLoopState.flags &= ~0x400000;
        if (evsEffe != 0) {
            sefDeleteEffectCf(evsEffe);
            evsEffe = 0;
        }
    }
    if (return_unit != -1) {
        MapUnitRecord *return_symbol = &MapUnit[return_unit];
        if ((GameLoopState.flags & 0x800000) == 0) {
            if (retEffe != 0)
                sefDeleteEffectCf(retEffe);
            retEffe = sefCreateEffectCf(0x2d9, &return_symbol->position, 0);
            xglSoundEffectNormalID(12, 0);
        }
        GameLoopState.flags |= 0x800000;
    } else {
        GameLoopState.flags &= ~0x800000;
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
    for (camera_id = 0; camera_id < 8; camera_id++) {
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
        if ((unit->inUseId & 0xf000) == 0 && parent == 0) {
            Matrix4 *model_matrices = unit->jointMatrices;
            xglMatrixMul((Matrix *)matrix, (Matrix *)model_matrices[0], (Matrix *)model_matrices[45]);
            matrix[3][3] = 1.0f;
            xglRotTransPers((Vector4 *)&projected, 0, (Vector4 *)matrix[3], camera_id);
        } else {
            if (parent != 0 && (parent->inUseId & 0xf000) == 0) {
                Matrix4 *parent_matrices = parent->jointMatrices;
                xglMatrixMul((Matrix *)matrix, (Matrix *)parent_matrices[0], (Matrix *)parent_matrices[45]);
            } else {
                Matrix4 *root_matrix = unit->jointMatrices;
                /* The original copies these four complete matrix rows
                 * with the SDK quadword idiom. The scratch is confined
                 * to this block; both bases are genuine C objects. */
                __asm__ __volatile__(
                    "lq $2,0(%0)\n\t"
                    "sq $2,0(%1)\n\t"
                    "lq $2,16(%0)\n\t"
                    "sq $2,16(%1)\n\t"
                    "lq $2,32(%0)\n\t"
                    "sq $2,32(%1)\n\t"
                    "lq $2,48(%0)\n\t"
                    "sq $2,48(%1)"
                    :
                    : "r"(root_matrix), "r"(matrix)
                    : "$2", "memory");
            }
            matrix[3][3] = 1.0f;
            xglRotTransPers((Vector4 *)&projected, 0, (Vector4 *)matrix[3], camera_id);
        }
        screen_x = projected.x;
        /* Unsigned subtraction preserves the original SLTIU window
         * without signed overflow for arbitrary projected coordinates. */
        if ((unsigned int)screen_x - 0x7001u >= 0x1fffu ||
            projected.y < 0x7201 || projected.y > 0x8dff ||
            projected.z <= 0)
            continue;
        screen_y = projected.y;
        depth = projected.z;
        color = 0xffffff;
        if (unit->kind == 1) {
            color = 0xff9933;
        } else {
            switch (unit->inUseId & 0xf000) {
            case 0: color = 0x3399ff; break;
            case 0x4000: color = 0x33ff33; break;
            case 0x2000: color = 0xff3333; break;
            }
        }
        format[1] = color >> 16;
        format[2] = color >> 8;
        format[3] = color;
        xglFontDebugPrintf(((screen_x >> 4) - 0x700) >> 1,
                           ((screen_y >> 4) - 0x720) >> 1,
                           format, index + 0x200, depth);
    }
    map_format = str_1;
    for (index = 0; index < 64; index++) {
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
        if ((unsigned int)screen_x - 0x7001u >= 0x1fffu ||
            projected.y < 0x7201 || projected.y > 0x8dff ||
            projected.z <= 0)
            continue;
        screen_y = projected.y;
        depth = projected.z;
        color = 0xffffff;
        switch (unit->serial & 0xf000) {
        case 0x5000: color = 0x7f7f7f; break;
        case 0x6000: color = 0xcc66ff; break;
        case 0x3000: color = 0xffff00; break;
        }
        map_format[1] = color >> 16;
        map_format[2] = color >> 8;
        map_format[3] = color;
        xglFontDebugPrintf(((screen_x >> 4) - 0x700) >> 1,
                           ((screen_y >> 4) - 0x720) >> 1,
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
            if (!(unit->flags & 8)) {
                if (unit->flags & 0x20) {
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
        GameLoopState.frame_hook(3);
    }
    if (GameLoopState.flags & 0x2) {
        if (GameLoopState.clear_callback != 0) {
            GameLoopState.clear_callback();
        } else {
            TestEnv_2_00338620.red = ClearEnv.red;
            TestEnv_2_00338620.green = ClearEnv.green;
            TestEnv_2_00338620.blue = ClearEnv.blue;
            TestEnv_2_00338620.alpha = 0x80;
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
        if (!(GameLoopState.flags_wide & 0x4000001)) {
            sefExecEffect();
        }
        GameMovieMain();
        xglSoundStreamMain();
        if (GameLoopState.flags & 0x4) {
            GameDispTag();
        }
    }
    if (GameLoopState.frame_hook != 0) {
        GameLoopState.frame_hook(4);
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
        GameLoopState.frame_hook(5);
    }
}

static int GameModeEvtTools(void)
{
    if (game_mode_status == 0) {
        if ((PadData.pressed & 0x800) != 0) {
            u16 buttons = PadData.held;
            game_mode_status = 16;

            if ((buttons & 0x10c) == 0x10c) {
                u32 flag_next = game_mode_flags | 0x80000000u;
                game_mode_flags = flag_next;
                return 1;
            } else {
                xglSoundEffectNormalDirect(4);
                game_mode_flags &= 0xfffffffeu;
                game_mode_debug_storage = 0;
                game_mode_kind = game_mode_saved_kind;
            }
        }
    }

    TWSYS_update();
    EvtTools();
    {
        StudioCamera *camera = xglStudioGetCamera2(0);
        if (camera->state == 4 && game_mode_camera_callback != 0) {
            game_mode_camera_callback();
        }
    }
    return 0;
}

static int GameModePause(void)
{
    if (game_mode_status == 0) {
        if ((PadData.pressed & 0x800) != 0) {
            u16 buttons = PadData.held;
            game_mode_status = 16;

            if ((buttons & 0x10c) == 0x10c) {
                u32 flag_next = game_mode_flags | 0x80000000u;
                game_mode_flags = flag_next;
                return 1;
            } else {
                xglSoundEffectNormalDirect(4);
                game_mode_flags &= 0xfffffffeu;
                game_mode_debug_storage = 0;
                game_mode_kind = game_mode_saved_kind;
                PartyTimePauseEnd();
            }
        }
    }

    TWSYS_update();
    PauseMenu();
    if (game_mode_saved_kind == 2 || game_mode_saved_kind == 3) {
        ACT_pauseUpdate();
    }
    {
        StudioCamera *camera = xglStudioGetCamera2(0);
        if (camera->state == 4 && game_mode_camera_callback != 0) {
            game_mode_camera_callback();
        }
    }
    return 0;
}

static int GameModeEvtDebug(void)
{
    if (game_mode_status == 0) {
        if ((PadData.pressed & 0x800) != 0) {
            u16 buttons = PadData.held;
            game_mode_status = 16;

            if ((buttons & 0x10c) == 0x10c) {
                u32 flag_next = game_mode_flags | 0x80000000u;
                game_mode_flags = flag_next;
                return 1;
            } else {
                xglSoundEffectNormalDirect(4);
                game_mode_flags &= 0xfffffffeu;
                game_mode_debug_storage = 0;
                game_mode_kind = game_mode_saved_kind;
            }
        }
    }

    TWSYS_update();
    {
        StudioCamera *camera = xglStudioGetCamera2(0);
        if (camera->state == 4 && game_mode_camera_callback != 0) {
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

    GameLoopState.flags &= ~1;
    SCRIPT_incCfTime();
    if (PadData.pressed & 0x810) {
        const u64 *button_words;
        u64 held_buttons;

        button_words = (const u64 *)&PadData.held;
        __builtin_memcpy(&held_buttons, button_words, sizeof(held_buttons));
        if ((held_buttons & 0x0800010C) == 0x0800010C) {
            GameLoopState.flags |= 0x80000000;
            return 1;
        }
        if (!(GameLoopState.flags_wide & 0x238000) && WIN_checkActiveWindow() == 0
            && SCRIPT_sceneChangeTimeGet() == 0 && !(GameLoopState.flags & 0x100000)
            && GameLoopState.controlled_actor != 0
            && (encounter_enemy = &enepc[GameLoopState.controlled_actor->number])->encount_lock == 0
            && xglCdSync() == 0 && SCRIPT_talkIgnoreGet() == 0 && GameLoopState.status == 0) {
            if (PadData.pressed & 0x800) {
                GameLoopState.status = 0x10;
                xglSoundEffectNormalDirect(4);
                previous_kind = GameLoopState.kind;
                GameLoopState.flags |= 1;
                GameLoopState.kind = 1;
                GameLoopState.saved_kind = previous_kind;
                GameLoopState.debug_menu.storage = 0;
                PartyTimePauseStart();
                return 0;
            }
            else if (PadData.pressed & 0x10) {
                GameLoopState.status = 0x20;
                GameStateSaveCameraLight();
                nmlModelSendMenuStart();
                xglCullingIgnore();
                MainMenu();
                xglCullingIgnoreOff();
                nmlModelSendMenuEnd();
                GameStateRestoreCameraLight();
                if (GameLoopState.flags & 0x10000000) {
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
    GameLoopState.flags = GameLoopState.flags & ~0x200000;
    if (GameLoopState.event_sub_id != 0) {
        const char *always_method;
        EnemyWorkPostTalk *battle_enemies;

        Game_Data_Push();
        GameLoopState.kind = 5;
        nmlModelSendSignalStartBattle();
        xglSoundStreamStop(0);
        Vibration_Stop();
        PadData.second_actuator[0] = 0;
        PadData.actuator[0] = 0;
        func_A22668();
        GameLoopState.frame_countdown = 0;
        GameLoopState.flags |= 0x40;
        MSG_init();
        TWSYS_init();
        Vibration_Stop();
        GameLoopState.model = 0;
        if (GameLoopState.flags & 0x10000000) {
            return 1;
        }
        battle_result = func_00A0B5F0();
        if (battle_result == 2 && !(GameLoopState.battle_flags & 0x40)) {
            xglFontLoad(1, 0);
            WindowTexLoad(0, 0);
            GameLoopState.flags |= 0x88000000;
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
        case 1:
            Enemy_After_Battle(1);
            CallMethod_I(eobMethodName,
                battle_enemies[GameLoopState.battle_enemy->number].enemy_id);
            break;
        case 3:
            Enemy_After_Battle(3);
            CallMethod_I(eobEscapeMethodName,
                battle_enemies[GameLoopState.battle_enemy->number].enemy_id);
            break;
        default:
            Enemy_After_Battle(1);
            break;
        }
        if (GameLoopState.kind == 3) {
            return 0;
        }
        GameLoopState.controlled_actor->flash_timer = 0x4B;
        Actor_LookAt_Init(actor);
        MAP_updateUnit();
        return 0;
    }
    if (FLAG_FRAME_60 != 0) {
        if ((PadData.held & 1) && (PadData.pressed & 4)) {
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
    if ((GameLoopState.flags_wide & 0x0001000000010000ULL) == 0x0001000000000000ULL) {
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
    if ((GameLoopState.flags_wide & 0x0001000000010000ULL) == 0x10000) {
        Actor *motion_player;

        motion_player = GameLoopState.controlled_actor;
        motion_player->status_flags |= 0x1000;
        ACT_setMotion2(motion_player, 0, 9);
    }
    flags = GameLoopState.flags;
    player = GameLoopState.controlled_actor;
    GameLoopState.base_flags = flags;
    if (!(flags & 0x20000)) {
        if (!(flags & 0x10000)) {
            GameCfPlayerMove();
            if (!(GameLoopState.flags & 0x8000)) {
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
        if (getScriptFlag(window_owner) & 2) {
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
    if (xglStudioGetCamera2(0)->state == 4 && GameLoopState.camera_callback != 0) {
        GameLoopState.camera_callback();
    }
    if ((GameLoopState.flags_wide & 0x60000) == 0) {
        ACT_update();
    } else {
        for (countdown = 15u; countdown > -1; countdown--) {
            int first_countdown = enepc[15 - countdown].stiff_timer;
            int second_countdown = (u16)enepc[15 - countdown].effect_timer[0] - 1;
            first_countdown -= 1;
            enepc[15 - countdown].effect_timer[0] = second_countdown;
            enepc[15 - countdown].stiff_timer = first_countdown;
            if ((short)first_countdown < 0) {
                enepc[15 - countdown].stiff_timer = 0;
            }
            if (enepc[15 - countdown].effect_timer[0] < 0) {
                enepc[15 - countdown].effect_timer[0] = 0;
            }
        }
        ACT_pauseUpdate();
    }
    MAP_updateUnit();
    {
    GameRadarParams radar;
    radar.x = 0x160;
    radar.color = 0xFFFF00;
    radar.y = 0x120;
    radar.red = radar.green = radar.blue = 0x80;
    radar.alpha = 0x60;
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

    if (PadData.pressed & 0x20) {
        if (GameLoopState.status == 0 && xglCdSync() == 0) {
            if (GameLoopState.flags_wide & 0x20400000) {
                GameStateSave();
                MenuFileMain(0);
                GameLoopState.status = 0x20;
                return;
            }
            if (GameLoopState.flags & 0x800000) {
                CallMethod("evsExit");
                GameLoopState.status = 0x20;
                return;
            }
            if (GameLoopState.flags & 0x40000000) {
                if (shopNo == -1) {
                    shopNo = 1;
                }
                GameStateSaveCameraLight();
                MenuShopMain(shopNo);
                GameStateRestoreCameraLight();
                GameLoopState.status = 0x20;
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
    enemy->post_talk_flags = GameLoopState.flags & 0x400;
}

static int GameModeCfEvent(void)
{
    int cleared = GameLoopState.flags & -2;
    u8 event_active = GameLoopState.status;

    GameLoopState.flags = cleared;
    if (event_active == 0 && (PadData.pressed & 0x800) != 0 &&
        (PadData.held & 0x10c) == 268) {
        GameLoopState.flags = cleared | 0x80000000;
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
    if ((PadData.debug_buttons & 0x100) != 0) {
        GameLoopState.debug_menu.storage = 0;
        GameLoopState.flags &= 0xfffffffeu;
        GameLoopState.kind = GameLoopState.saved_kind;
    }

    TWSYS_update();
    ACT_pauseUpdate();
    GameDebugMenu();
    {
        StudioCamera *camera = xglStudioGetCamera2(0);
        if (camera->state == 4 && GameLoopState.camera_callback != 0) {
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
    GameLoopState.kind = 0;
    GameLoopState.stateC8 = 0;
    GameLoopState.actor_draw_callback = 0;
    GameLoopState.vector80[0] = one;
    GameLoopState.vectorA0[0] = zero;
    GameLoopState.vectorA0[3] = 30.0f;
    GameLoopState.stateD0 = -2;
    GameLoopState.flags = 0x302;
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
    for (i = 0; i < 64; i++) {
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
    GameLoopState.status = 0x10;
    if (GameLoopState.kindDetail != 0) {
        Quadword *position_dst;

        player = GameLoopState.controlled_actor;
        GamePopSaveDataUser();
        position_dst = (Quadword *)&player->position;
        Java_xeno_util_Runtime_jump_sub(GameLoopState.current_event_id, -1);
        __asm__ __volatile__(
            "lq $2, 0(%1)\n"
            "sq $2, 0(%0)\n"
            :
            : "r" (position_dst), "r" (&GameLoopState.saved_actor_position)
            : "$2", "memory");
        GameLoopState.move_mode |= 0xa0;
    } else {
        Java_xeno_util_Runtime_jump_sub(GameLoopState.current_event_id, -1);
    }
    while (1) {
        if (GameLoopState.frame_hook != 0) {
            GameLoopState.frame_hook(2);
        }
        nmlModelSendPauseSignal(GameLoopState.flags & 1);
        SCRIPT_sceneChangeTimeDec();
        Disp_Homing();
        if (GameLoopState.status != 0) {
            GameLoopState.status--;
        }
        switch (GameLoopState.kind) {
        case 0:
            GameLoopState.kind = 2;
            break;
        case 1:
            GameModePause();
            break;
        case 2:
            GameModeCfMain();
            break;
        case 3:
            GameModeCfEvent();
            break;
        case 6:
            GameModeEvtDebug();
            break;
        case 7:
            GameModeEvtTools();
            break;
        case 8:
            GameModeDebugMenu();
            break;
        }
        if (GameLoopState.flags & 0x90000000) {
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
    if (GameLoopState.flags & 0x8000000) {
        GameOver();
    }
    GameLoopState.kind = 0;
    xglSleep();
    xglSleep();
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
/* Small initialized flags follow the existing evsExit literal above. */
unsigned char SnapDrawCreditFlag = 0;

unsigned char UseTestPath = 0;

const char GameEmptyName[1] = "";

const char GameMcTestName[8] = "MC_TEST";
