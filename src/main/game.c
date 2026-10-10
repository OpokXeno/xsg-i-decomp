#include "common.h"

#include "shared.h"

#include "game.h"

/*
 * GNU EE native TI storage/copy type. Used only for the
 * 16-byte aligned typed storage that the original clears with por+sq; no
 * wide arithmetic.
 */

typedef unsigned int Quadword __attribute__((mode(TI)));

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
    u8 _unmodeled_00[0x0c];
    u16 kind;
    u16 kindDetail;
    u32 flags;
    u8 _unmodeled_14[0x14];
    GameModeCameraCallback camera_callback;
    u8 _unmodeled_2c[0xe0 - 0x2c];
    u16 restoreKind;
    u16 restoreKindDetail;
    u8 _unmodeled_e4[0x29f40 - 0xe4];
    u8 status;
    u8 _unmodeled_29f41[0x0f];
    GameDebugMenuStorage debug_menu;
    u16 saved_kind;
    u8 _unmodeled_29f62[0x2a030 - 0x29f62];
} GameLoopStateLayout;

GameLoopStateLayout GameLoopState = {0};

unsigned char SnapDrawCreditFlag = 0;

unsigned char UseTestPath = 0;

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

extern void xglSoundEffectNormalDirect(int effect_id);

extern void TWSYS_update(void);

extern void EvtTools(void);

extern void PauseMenu(void);

extern void PartyTimePauseEnd(void);

extern void ACT_pauseUpdate(void);

extern StudioCamera *xglStudioGetCamera2(int camera_id);

extern void JTHREAD_cntl(void);

extern void PLAY_ctrl(void);

extern void TCAMERA_update(void);

extern void ACT_update(void);

extern void MAP_updateUnit(void);

extern void GameDebugMenu(void);

int GameModeDebugMenu(void);

#define game_mode_flags (GameLoopState.flags)

#define game_mode_kind (GameLoopState.kind)

#define game_mode_status (GameLoopState.status)

#define game_mode_saved_kind (GameLoopState.saved_kind)

#define game_mode_debug_storage (GameLoopState.debug_menu.storage)

#define game_mode_camera_callback (GameLoopState.camera_callback)

extern void xglSoundSendEffect(void *swd, void *sed, int bank);

extern void After_Talk(SceneObject object);

typedef struct EnemyWorkPostTalk {
    u8 unmodeled_00[0x37b4];
    u32 post_talk_flags;
    u8 unmodeled_37b8[0x38b0 - 0x37b8];
} EnemyWorkPostTalk;

extern EnemyWorkPostTalk enepc[16];

#define ACTOR_NUMBER_OFFSET 0x80

extern void GameStateRestoreCameraLight(void);

extern void GameStateRestoreKoware(void);

static u64 attrPrev;

typedef struct GameActorAttributeView {
    u8 unmodeled_00[0x4e8];
    u64 attribute_flags;
} GameActorAttributeView;

#define ACTOR_SCRIPT_FLAGS_OFFSET 0x124

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

typedef struct {
    u32 flags;
    u8 unmodeled_04[ACTOR_IN_USE_ID_OFFSET - 0x04];
    short inUseId;
    u8 unmodeled_88[0xA70 - (ACTOR_IN_USE_ID_OFFSET + 2)];
} ActorHead;

extern ActorHead actor[ACTOR_COUNT];

/* Defined in src/main/act_3.c (main/tu265). */

void ACT_DrawShadowBegin(void);

void ACT_DrawShadow(ActorHead *unit);

void ACT_DrawShadowEnd(void);

#include "main/control_entry.h"

#include "main/xgl_sound.h"

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
    xglSoundLoadEffect(D_004BE2B0, segment, 1);
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

INCLUDE_ASM("asm/main/nonmatchings/game", Game_Data_Push);

INCLUDE_ASM("asm/main/nonmatchings/game", Game_Data_Pop);

INCLUDE_ASM("asm/main/nonmatchings/game", MenuGameDataPush);

INCLUDE_ASM("asm/main/nonmatchings/game", MenuGameDataPop);

INCLUDE_ASM("asm/main/nonmatchings/game", GameStateSaveCameraLight);

INCLUDE_ASM("asm/main/nonmatchings/game", GameStateRestoreCameraLight);

INCLUDE_ASM("asm/main/nonmatchings/game", GameStateSave);

void GameStateRestore(void)
{
    GameLoopState.kind = GameLoopState.restoreKind;
    GameLoopState.kindDetail = GameLoopState.restoreKindDetail;
    GameStateRestoreCameraLight();
    GameStateRestoreKoware();
}

INCLUDE_ASM("asm/main/nonmatchings/game", GamePushSaveDataUser);

INCLUDE_ASM("asm/main/nonmatchings/game", GamePopSaveDataUser);

INCLUDE_ASM("asm/main/nonmatchings/game", InitCfSystem);

static int checkAttr(GameActorAttributeView *actor)
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
    int *script_flags = (int *)((SceneByte *)object + ACTOR_SCRIPT_FLAGS_OFFSET);
    return *script_flags;
}

INCLUDE_ASM("asm/main/nonmatchings/game", checkTalk);

INCLUDE_ASM("asm/main/nonmatchings/game", checkTouch);

INCLUDE_ASM("asm/main/nonmatchings/game", checkItemBoxDir);

INCLUDE_ASM("asm/main/nonmatchings/game", checkItemBox);

INCLUDE_ASM("asm/main/nonmatchings/game", CheckGameSymbol);

INCLUDE_ASM("asm/main/nonmatchings/game", GameDispTag);

void GameDrawShadow(void)
{
    ActorHead *unit;
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

INCLUDE_ASM("asm/main/nonmatchings/game", GameDrawSync);

static int GameModeEvtTools(void)
{
    if (game_mode_status == 0) {
        if (((u16)PadData.pressed & 0x800) != 0) {
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
        if (((u16)PadData.pressed & 0x800) != 0) {
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
        if (((u16)PadData.pressed & 0x800) != 0) {
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

INCLUDE_ASM("asm/main/nonmatchings/game", GameModeCfMain);

INCLUDE_ASM("asm/main/nonmatchings/game", checkTalkPoint);

INCLUDE_ASM("asm/main/nonmatchings/game", actSequenceClear);

void actTalkAfter(SceneObject object)
{
    EnemyWorkPostTalk *enemy = &enepc[object[ACTOR_NUMBER_OFFSET]];

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

INCLUDE_ASM("asm/main/nonmatchings/game", Game);
