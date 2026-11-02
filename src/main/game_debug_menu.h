/*
 * TU-local declarations of main/tu126 (src/main/game_debug_menu.c).
 */

#ifndef SRC_MAIN_GAME_DEBUG_MENU_H
#define SRC_MAIN_GAME_DEBUG_MENU_H

#include "shared.h"
#include "main/act_1.h"
#include "main/exm_2.h"
#include "main/Undulate.h"
#include "main/game.h"
#include "main/game_debug_menu.h"
#include "main/game_disk_change.h"
#include "main/game_snap_shot.h"
#include "main/look_at_player.h"
#include "main/map_1.h"
#include "main/res_get_path.h"
#include "main/runtime.h"
#include "main/xgl_font.h"
#include "main/xgl_render.h"

/* GameDebugMenu pages (GameLoopState.debugPage), cycled with L1/R1. */
enum DebugMenuPage {
    DEBUG_PAGE_MENU,
    DEBUG_PAGE_RESOURCES,
    DEBUG_PAGE_FLAGS,
    DEBUG_PAGE_UWAMONO,
    DEBUG_PAGE_ENEMY,
    DEBUG_PAGE_PARTY,
    DEBUG_PAGE_COUNT
};

/* Rows of the menu page (tbl_1_00367A70). */
enum DebugMenuRow {
    DEBUG_ROW_CHANGE_MAP,
    DEBUG_ROW_CHANGE_LEADER,
    DEBUG_ROW_RUN_BATTLE,
    DEBUG_ROW_SNAP,
    DEBUG_ROW_MODEL_FOLDER,
    DEBUG_ROW_MAP_FOLDER,
    DEBUG_ROW_SCENE_FOLDER,
    DEBUG_ROW_RUN_CF,
    DEBUG_ROW_RUN_EVENT,
    DEBUG_ROW_SCANLINE_INTERPOLATION,
    DEBUG_ROW_COUNT
};

/* Rows of the debug-flags page (tbl_0_00367A50). */
enum DebugFlagRow {
    DEBUG_FLAG_ROW_CF_CAMERA_SWITCH,
    DEBUG_FLAG_ROW_MODEL_NUMBER_TAG,
    DEBUG_FLAG_ROW_SNAP_CREDIT,
    DEBUG_FLAG_ROW_SNAP_QUALITY,
    DEBUG_FLAG_ROW_MODEL_NUMBER_CHECK,
    DEBUG_FLAG_ROW_ALPHA_ON_CAPTURE,
    DEBUG_FLAG_ROW_DISC_CHECK
};

/* Left/right step of the menu page, multiplied while the cross button is held. */
#define DEBUG_FAST_STEP 100
#define SNAP_QUALITY_MIN (-5)
#define SNAP_QUALITY_MAX 100
/* Snapshot file names are "game<digit>000". */
#define SNAPSHOT_NUMBER_COUNT 10
/* Selectable variants: debug map path sets, scene paths, scanline modes. */
#define DEBUG_MAP_PATH_COUNT 3
#define DEBUG_SCENE_PATH_COUNT 20
#define SCANLINE_MODE_COUNT 3
/* Script ids of the run-CF and run-event rows wrap at this value. */
#define SCRIPT_JUMP_ID_COUNT 10000

#define MAP_COUNT 2000
#define SCRIPT_ID_EVENT_BASE 0x10000000

/* GameResource: 128 resource blocks of 16 bytes (address, size, handle,
 * state); the page-3 usage line walks address and size. */

#define CF_CAMERA_SWITCH 0x02
#define RUNTIME_MODEL_NUMBER_TAG 0x4
#define RUNTIME_CAPTURE_ALPHA 0x02000000
#define ACTOR_CHECK_HIDDEN 0x8
#define MAP_UNIT_CHECK_HIDDEN 0x4
#define MODEL_CHECK_ACTOR 0x200
#define MODEL_CHECK_MAP_UNIT 0x300
/* modelNumberCheck = kind (high byte) | slot index (low byte). */
#define MODEL_CHECK_KIND_MASK 0xff00
#define MODEL_CHECK_INDEX_MASK 0xff
/* Cross flips the kind between actor (0x200) and map unit (0x300). */
#define MODEL_CHECK_KIND_FLIP 0x100
#define DEBUG_FLAG_COUNT 7

/* PadData through +0x2e: the PadPrefix halfwords plus the two this TU's
 * pages also read. */
typedef struct {
    u8 pad_00[0x28];
    u16 half_28;
    u16 half_2a;
    u16 half_2c;
    u16 half_2e;
} PadDebugView;

/* The unit GameLoopState points at (+0x04); page 3 resets and replaces its
 * position from the map's layout header. */
typedef struct {
    unsigned char unmodeled_00[0x10];
    float x;
    float y;
    float z;
    unsigned char unmodeled_1c[0x54 - 0x1c];
    float angle;
    unsigned char unmodeled_58[0x9e4 - 0x58];
    float layoutAngle;
} DebugPlayerUnit;

/* GameDebugMenu selects six pages and resets debugPageInitialized when
 * cycling pages. Pages 2 and 3 keep their private state in the same bytes
 * of GameLoopState (+0x29f72 and +0x29f80), each with its own access
 * widths and signedness; the other GameLoopState bytes are not accessed by
 * this TU beyond the named members. The page-3 rows are listed in
 * tbl_1_00367A70. */
typedef struct {
    unsigned char unmodeled_00[0x04];
    DebugPlayerUnit *player;
    unsigned char unmodeled_08[0x0c - 0x08];
    short mode;
    unsigned char unmodeled_0e[0x10 - 0x0e];
    int runtimeFlags;
    unsigned char unmodeled_14[0x18 - 0x14];
    int scriptId;
    unsigned char unmodeled_1c[0x50 - 0x1c];
    unsigned short currentMapId;
    unsigned char unmodeled_52[0xc1 - 0x52];
    unsigned char cfCameraFlags;
    unsigned char unmodeled_c2[0xd0 - 0xc2];
    signed char snapQuality;
    unsigned char unmodeled_d1[0x29f50 - 0xd1];
    unsigned char debugPage;
    unsigned char debugPageInitialized;
    unsigned char unmodeled_29f52[0x29f71 - 0x29f52];
    signed char debugCursor;
    union {
        unsigned short modelNumberCheck;
        short mapChangeId;
    } target;
    unsigned short leaderBattleId;
    unsigned char unmodeled_29f76[0x29f80 - 0x29f76];
    union {
        unsigned int actorFlag8Clear[2];
        struct {
            unsigned short cfId;
            unsigned short eventId;
        } scriptJump;
    } scratch;
    unsigned char unmodeled_29f88[0x29f90 - 0x29f88];
    unsigned int mapUnitFlag4Clear[2];
} DebugMenuPageState;

/* SaveData (include/umbrella/public.h) up to the leader's battle id
 * (+0x100a4); the TU reads only that halfword. */
typedef struct {
    unsigned char unmodeled_00[0x100a4];
    unsigned short leaderBattleId;
} DebugSavedParty;

typedef struct {
    u32 address;
    int size;
    unsigned char unmodeled_08[0x10 - 0x08];
} DebugResourceSlot;

extern PadDebugView PadData;
extern Actor actor[ACTOR_COUNT];
extern DebugMenuPageState GameLoopState;
static const char *tbl_1_00367A70[];
extern const char debugMenuHeaderLabel[];
extern char memoryUsedGlyph[];
extern char debugCursorText[];
extern const char yesDisp1Text[];
extern const char yesSoftText[];
extern char noText[];
extern const char playerPositionLabel[];
extern char mapChangeIdNameFormat[];
extern char debugPathFormat[];
extern char scriptJumpIdFormat[];
extern DebugResourceSlot GameResource[GAME_RESOURCE_COUNT];
extern unsigned char ScenePath;
extern MapUnitRecord MapUnit[MAP_UNIT_COUNT];
extern const char debugFlagsHeaderLabel[];
extern char fullWidthCursorText[];
extern char hexDigitFormat[];
extern char snapQualityFormat[];
static const char *tbl_0_00367A50[];
extern const char discCheckLabel[17];
extern const char alphaOnCaptureLabel[17];
extern const char modelNumberCheckLabel[19];
extern const char snapQualityLabel[13];
extern const char snapCreditLabel[19];
extern const char modelNumberTagLabel[15];
extern const char cfCameraSwitchLabel[19];
extern const char scanlineInterpolationLabel[11];
extern const char runEventLabel[13];
extern const char runCfLabel[9];
extern const char sceneFolderLabel[15];
extern const char mapFolderLabel[15];
extern const char modelFolderLabel[15];
extern const char snapLabel[9];
extern const char runBattleLabel[11];
extern const char changeLeaderLabel[13];
extern const char changeMapLabel[11];
extern const char memoryUsedFormat[16];
extern const char memoryRemainFormat[16];
extern const char snapShotFileNameFormat[16];
extern const char dataDirectoryPath[16];
extern const char dataTestDirectoryPath[24];

void GameResourceDump(int dump);
extern void func_A22668(void);
extern char *RES_getPath(void);
static void PauseMenuPage2(void);
static void PauseMenuPage3(void);
extern void PauseMenuPageUwamono(void);
extern void PauseMenuPageEnemy(void);
extern void PauseMenuPagePartyDebug(void);

#endif /* SRC_MAIN_GAME_DEBUG_MENU_H */
