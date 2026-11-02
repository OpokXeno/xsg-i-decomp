/*
 * TU-local declarations of main/tu116 (src/main/game.c).
 */

#ifndef SRC_MAIN_GAME_H
#define SRC_MAIN_GAME_H

#include "shared.h"
#include "main/xgl_studio.h"
#include "main/sef.h"
#include "main/xgl_2.h"
#include "main/xgl_task.h"
#include "main/party.h"
#include "main/get.h"
#include "main/init_vm.h"
#include "main/game.h"
#include "main/near_dir.h"
#include "main/ssd_2.h"
#include "main/ssd_init.h"
#include "main/xgl_font.h"
#include "main/xgl_sound.h"

/* TU-local native-TI storage/copy type; ordinary functions use scalar C. */
typedef unsigned int Quadword __attribute__((mode(TI)));

/* Canonical defining-owner proposal from src/main/party.h:10 and308-317.
 * PartyDataGet returns the serialized SaveData party region. Existing
 * party.c consumers use this exact typed prefix at that boundary; the
 * generated owner header does not yet transport these declarations. */
/* Private caller proposal: MenuGameDataPop passes actor and zero.
 * The accepted defining body in act_2.c retains its one-argument signature.
 * This explicit owner-interface conflict requires later transport; no
 * published signature, adapter or nominal Actor type is changed here. */
struct Actor;

/* Private defining-owner proposals from xgl_2.h and xgl_2.c454.
 * Matrix's genuine owner tag stays distinct from Matrix4. The diagnostic
 * body retains nominal boundary warnings until coherent owner transport. */
typedef union Matrix Matrix;

typedef void (*GameModeCameraCallback)(void);

/*
 * PARTIAL ACCESSED PREFIX of PadData (0xd0-byte object at 0x490d90).
 * GameModeDebugMenu reads only the halfword at +0x2e (masked with 0x100),
 * which the shared PadPrefix/PadDataLayout views (evidenced to +0x2c) do
 * not reach. held/pressed keep the accepted names at +0x28/+0x2a; the
 * +0x2c halfword is not accessed here. Neutral debug_buttons name: only the
 * 0x100 debug-menu exit test is evidenced.
 *
 * This TU uses the partial PadDataDebugLayout view through +0x2e.
 */
typedef struct PadDataDebugLayout {
    u8 _unmodeled_00[0x28];
    u16 held;
    u16 pressed;
    u16 _unmodeled_2c;
    u16 debug_buttons;
    u8 _unmodeled_30[0x50 - 0x30];
    /* xgl_pad.h: actuator[0..5] in two consecutive 0x68-byte records. */
    u8 actuator[6];
    u8 vibrationReset; /* +0x56 */
    u8 _unmodeled_57[0xb8 - 0x57];
    u8 second_actuator[6];
    u8 vibrationStatus; /* +0xbe */
} PadDataDebugLayout;

/* Game's caller/owner declarations. Unpublished raw arena construction
 * and common-actor factory boundaries remain explicitly diagnostic. */

/* The eight-byte format prefix is evidenced. Its complete source-object
 * extent is unknown; the following EOB string is not an invented tail. */
extern const char dispTagFormat[];

extern int UseVMFlag;

/* Private defining-owner proposals: sef.c2078..2086 returns/releases
 * SchedulerState pointers. These declarations need later owner transport. */
extern int shopNo;

extern const char cfRegistSoundName[9];
extern int UmnSimulationNo;
extern int MenuDrillCall;

extern float atan2f(float y, float x);
void xglMatrixMul(Matrix *destination, Matrix *left, Matrix *right);

float xglRotTransPers(Vector4 *out, Matrix4 matrix,
                     Vector4 *point, int camera_id);

extern StudioLight *xglStudioGetLight2(void);
int SsdResetSegmentAllocMode(int segment);
int SsdSetSegmentAllocMode(int segment, int mode);
void xglSoundLoadEffect(const char *name, void *segment, int bank);
void EnemySound_StopAll(int mode);
void *GameResourceGetFreeAddr(void);
const char *RES_GetEnemySeName(int index);
void RES_GetMapEnvSeName(char *name);
void GamePopSaveDataUser(void);
void actSequenceClear(Actor *actor);
void *XTK_getWindowOwner(void);

static char str_0[12];

static char str_1[12];

#endif /* SRC_MAIN_GAME_H */
