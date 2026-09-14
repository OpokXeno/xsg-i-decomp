#include "common.h"
#include "shared.h"
#include "game.h"

/*
 * GNU EE native TI storage/copy type. Used only for the
 * 16-byte aligned typed storage that the original clears with por+sq; no
 * wide arithmetic.
 */
typedef unsigned int Quadword __attribute__((mode(TI)));

/*
 * 16-byte quadword-aligned vector storage, cleared through the union's quad
 * member. The wrapping union (rather than a bare Quadword field) matters for
 * GameModeDebugMenu's schedule: a store through a union member has alias set
 * 0
 */
typedef union QuadVector {
    Quadword quad;
} QuadVector;

/*
 * PARTIAL ACCESSED VIEW of GameLoopState (0x2a030-byte object at 0x338680).
 * Union of the offsets the merged handlers access:
 *  +0x0c kind u16, +0x10 flags u32, +0x28 camera_callback,
 *  +0x29f40 status u8 (read by GameModeCfEvent),
 *  +0x29f50 pause_vector 16-byte quadword (cleared with por+sq),
 *  +0x29f60 saved_kind u16.
 * Spans named _unmodeled_* are opaque sizing bytes never accessed by these
 * bodies; the remainder of the object beyond 0x29f62 is unmodeled. The name
 * is TU-local by canon
 */
typedef struct GameLoopStateLayout {
    u8 _unmodeled_00[0x0c];
    u16 kind;
    u8 _unmodeled_0e[2];
    u32 flags;
    u8 _unmodeled_14[0x14];
    GameModeCameraCallback camera_callback;
    u8 _unmodeled_2c[0x29f14];
    u8 status;
    u8 _unmodeled_29f41[0x0f];
    QuadVector pause_vector;
    u16 saved_kind;
} GameLoopStateLayout;

extern GameLoopStateLayout GameLoopState;

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
#define game_mode_pause_vector (GameLoopState.pause_vector.quad)
#define game_mode_camera_callback (GameLoopState.camera_callback)

INCLUDE_ASM("asm/main/nonmatchings/game", GameCFSoundPurgeSub);

INCLUDE_ASM("asm/main/nonmatchings/game", GameCFSoundPurge);

INCLUDE_ASM("asm/main/nonmatchings/game", GameCFSoundReload);

INCLUDE_ASM("asm/main/nonmatchings/game", GameCFSoundMenuPurge);

INCLUDE_ASM("asm/main/nonmatchings/game", GameCFSoundMenuReload);

INCLUDE_ASM("asm/main/nonmatchings/game", Game_Data_Push);

INCLUDE_ASM("asm/main/nonmatchings/game", Game_Data_Pop);

INCLUDE_ASM("asm/main/nonmatchings/game", MenuGameDataPush);

INCLUDE_ASM("asm/main/nonmatchings/game", MenuGameDataPop);

INCLUDE_ASM("asm/main/nonmatchings/game", GameStateSaveCameraLight);

INCLUDE_ASM("asm/main/nonmatchings/game", GameStateRestoreCameraLight);

INCLUDE_ASM("asm/main/nonmatchings/game", GameStateSave);

INCLUDE_ASM("asm/main/nonmatchings/game", GameStateRestore);

INCLUDE_ASM("asm/main/nonmatchings/game", GamePushSaveDataUser);

INCLUDE_ASM("asm/main/nonmatchings/game", GamePopSaveDataUser);

INCLUDE_ASM("asm/main/nonmatchings/game", InitCfSystem);

INCLUDE_ASM("asm/main/nonmatchings/game", checkAttr);

INCLUDE_ASM("asm/main/nonmatchings/game", getScriptFlag);

INCLUDE_ASM("asm/main/nonmatchings/game", checkTalk);

INCLUDE_ASM("asm/main/nonmatchings/game", checkTouch);

INCLUDE_ASM("asm/main/nonmatchings/game", checkItemBoxDir);

INCLUDE_ASM("asm/main/nonmatchings/game", checkItemBox);

INCLUDE_ASM("asm/main/nonmatchings/game", CheckGameSymbol);

INCLUDE_ASM("asm/main/nonmatchings/game", GameDispTag);

INCLUDE_ASM("asm/main/nonmatchings/game", GameDrawShadow);

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
                game_mode_pause_vector = 0;
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
                game_mode_pause_vector = 0;
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
                game_mode_pause_vector = 0;
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

INCLUDE_ASM("asm/main/nonmatchings/game", actTalkAfter);

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

/*
 * GameModeDebugMenu (0x00245ba8): the debug-menu game mode, the last
 * handler of the game-mode TU whose static handlers start at 0x00244d20
 * (Game at 0x00245c58 follows it). Pad button 0x100 leaves the menu: clear
 * the pause flag, the 16-byte pause vector and restore the saved mode kind,
 * as the accepted handlers do on resume. Then run the per-frame systems,
 * the debug menu itself and the shared studio-camera callback gate.
 */
int GameModeDebugMenu(void)
{
    if ((PadData.debug_buttons & 0x100) != 0) {
        GameLoopState.pause_vector.quad = 0;
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
