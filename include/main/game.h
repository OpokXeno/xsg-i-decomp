#ifndef INCLUDE_MAIN_GAME_H
#define INCLUDE_MAIN_GAME_H

#include "main/get.h"
#include "main/init_vm.h"
#include "main/party.h"
#include "main/sef.h"
#include "main/xgl_2.h"
#include "main/xgl_studio.h"
#include "main/xgl_task.h"

/* GameLoopState.flags bits (the game loop owner; other TUs share them). */
#define GAME_FLAG_PAUSE 0x1                 /* paused: passed to nmlModelSendPauseSignal */
#define GAME_FLAG_DRAW 0x2                  /* GameDrawSync draws the frame only with this set */
#define GAME_FLAG_DISP_TAG 0x4              /* GameDispTag debug overlay */
#define GAME_FLAG_BATTLE 0x40               /* set while a battle runs, cleared on return */
#define GAME_FLAG_NO_TALK 0x400             /* talk/touch/entered-script checks are skipped */
#define GAME_FLAG_TALKING 0x1000            /* set at talk start; also blocks talk/touch */
#define GAME_FLAG_NO_TALK_POINT 0x8000      /* set at talk start; skips checkTalkPoint, blocks menu */
#define GAME_LOOP_FLAG_PLAYER_LOCK 0x8000   /* the same bit, as the script command command_player_lock sets it: player input is locked */
#define GAME_FLAG_ACTOR_LOOK_AT 0x10000     /* player is turned by Actor_LookAt, no movement input */
#define GAME_FLAG_ACTOR_HOLD 0x20000        /* player frozen (item get): no movement; the actors run ACT_pauseUpdate */
#define GAME_FLAG_ACTOR_PAUSE 0x40000       /* actors run ACT_pauseUpdate instead of ACT_update */
#define GAME_FLAG_RADAR 0x80000             /* radar enabled (set by InitCfSystem, read by game_radar) */
#define GAME_FLAG_ENTERED_SCRIPT 0x100000   /* entered-attribute script running, blocks menu */
#define GAME_FLAG_SCRIPT_END 0x200000       /* a script task ended; cleared each frame */
#define GAME_FLAG_ON_EVENT_SYMBOL 0x400000  /* player stands on an event symbol */
#define GAME_FLAG_ON_RETURN_SYMBOL 0x800000 /* player stands on a return symbol */
#define GAME_FLAG_UMN 0x4000000             /* umn simulation: no effect updates */
#define GAME_FLAG_GAME_OVER 0x8000000       /* GameOver() runs on loop exit */
#define GAME_FLAG_EXIT 0x10000000           /* leave the game loop */
#define GAME_FLAG_ON_SAVE_SYMBOL 0x20000000 /* player stands on a save symbol */
#define GAME_FLAG_ON_SHOP_SYMBOL 0x40000000 /* player stands on a shop symbol */
#define GAME_FLAG_QUIT 0x80000000           /* leave the game loop (debug pad combo, battle lost) */
#define GAME_FLAG_LEAVE_LOOP (GAME_FLAG_QUIT | GAME_FLAG_EXIT)

/* GameLoopState.kind: which GameMode handler Game() runs. */
enum {
    GAME_MODE_INIT,
    GAME_MODE_PAUSE,
    GAME_MODE_CF_MAIN,
    GAME_MODE_CF_EVENT,
    GAME_MODE_BATTLE = 5,
    GAME_MODE_EVT_DEBUG,
    GAME_MODE_EVT_TOOLS,
    GAME_MODE_DEBUG_MENU
};

/* Stages passed to GameLoopState.frame_hook. */
enum {
    GAME_HOOK_FRAME_START = 2,
    GAME_HOOK_DRAW_BEGIN,
    GAME_HOOK_DRAW_END,
    GAME_HOOK_FRAME_END
};

/* Frames GameLoopState.status counts down to ignore pad edges after a mode change. */
#define GAME_STATUS_LOCK_SHORT 0x10
#define GAME_STATUS_LOCK_LONG 0x20

void MenuGameDataPush(void);
void MenuGameDataPop(void);
void GamePushSaveDataUser(void);
void Game(void);

/* Functions and data used by other translation units. */
extern unsigned char UseTestPath;
extern unsigned char SnapDrawCreditFlag;
void GameStateSave(void);
void GameStateRestore(void);
void checkTalkPoint(void);
int getScriptFlag(SceneObject object);
void GameDrawSync(void);
/* Private defining-owner proposals from init_vm.c198..235 and325..333.
 * This is the interpreter's genuine slot union, copied verbatim. Both
 * integer arguments initialize its i member; no inactive member is read.
 * Native dispatch supplies thread, arguments and result. The normal owner
 * header currently does not carry these declarations. */
void GameStateSaveCameraLight(void);
void InitCfSystem(void);

#endif /* INCLUDE_MAIN_GAME_H */
