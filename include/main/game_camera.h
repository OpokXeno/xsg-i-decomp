#ifndef INCLUDE_MAIN_GAME_CAMERA_H
#define INCLUDE_MAIN_GAME_CAMERA_H

/* Number of studio cameras (xglStudioGetCamera2 indices) and of CF camera
 * definitions (CfCameraDefine). */
#define STUDIO_CAMERA_COUNT 8
#define CF_CAMERA_COUNT 33

/* StudioCamera.state values the camera code relies on. */
#define STUDIO_CAMERA_STATE_EXTERNAL 2 /* TCAMERA_update leaves the camera alone */
#define STUDIO_CAMERA_STATE_PEER 3     /* Camera.start with a Chr peer */
#define STUDIO_CAMERA_STATE_GAME 4     /* driven by GameLoopState.camera_callback */

/* GameLoopState.camera_mode: which GameCamera update runs. */
enum GameCameraMode {
    GAME_CAMERA_MODE_TYPE0,       /* CameraType0 */
    GAME_CAMERA_MODE_TYPE1,       /* CameraType1 */
    GAME_CAMERA_MODE_TYPE0_ALT,   /* CameraType0 */
    GAME_CAMERA_MODE_FLOOR_FOLLOW, /* CameraType1, camera heights follow the floor exactly */
    GAME_CAMERA_MODE_PEDESTAL,    /* CameraType4, from the definition's pedestal mode */
    GAME_CAMERA_MODE_OFF = 0xff   /* camera disabled */
};

/* GameLoopState.camera_flags (a.k.a. hokanFlags). */
#define GAME_CAMERA_FLAG_NO_GROUND_PROBE 0x02 /* skip the player ground probe */
#define GAME_CAMERA_FLAG_DISABLED 0x08        /* camera is not updated */
#define GAME_CAMERA_FLAG_TRACK_PLAYER 0x20    /* refresh the old player position */
#define GAME_CAMERA_FLAG_MODE_CHANGED 0x40    /* start a new interpolation */
#define GAME_CAMERA_FLAG_SNAP 0x80            /* MoveHokan snaps to its target */
#define GAME_CAMERA_FLAGS_KEEP_MASK 0x3f      /* cleared after one update: MODE_CHANGED | SNAP */

/* GameLoopState.flags bit: the player's ground is rescanned from scratch
 * (UnduCheck ignores its cached triangle and GameCamera re-initializes its
 * probes); createTalkTask also refuses to start while it is set. */
#define GAME_FLAG_RESCAN_GROUND 0x20

/* Camera id used when no camera data matches. */
#define GAME_CAMERA_ID_DEFAULT 0x8000

void GameCameraStateSave(void);
void GameCameraStateRestore(void);

/* Functions and data used by other translation units. */
void GameCameraChangeID(int id, int unused1, int unused2);
void GameCameraReset(void);
void GameCamera(void);

#endif /* INCLUDE_MAIN_GAME_CAMERA_H */
