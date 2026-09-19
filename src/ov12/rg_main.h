/*
 * TU-local declarations of ov12/tu001 (src/ov12/rg_main.c).
 */

#ifndef SRC_OV12_RG_MAIN_H
#define SRC_OV12_RG_MAIN_H

#include "shared.h"
#include "ov12/rg_draw.h"

/*
 * Per-controller pad record. PadPrefix (canon: config/header-canon.json
 * PadData) covers the evidenced +0x28/+0x2a halfwords; this TU indexes a
 * second 0x68-byte record for controller 1, so the object is an array of
 * two of these records rather than the single canonical PadPrefix. The
 * bytes past +0x2c are never read by this TU's allocation.
 */
typedef struct PadState {
    PadPrefix prefix;
    u8 unmodeled_2c[0x3c];
} PadState;

extern PadState PadData[2];

/*
 * Robot-game battle state. Evidenced fields only: _GameInfoInit,
 * _GameInfoNextStage, _GameInfoIsEndOfGame, _GameInfoGetStage,
 * _GameInfoGetCurrentPlayTime and _GameInfoGetCurrentDamage together
 * account for every byte through offset 0x2c.
 */
typedef struct RgGameInfo {
    int playerResult[2]; /* 0x00: win tally per player; 2 ends the game */
    int stage;           /* 0x08: current stage index */
    int activePlayer;    /* 0x0c: which player's turn is active */
    int playTime[4];     /* 0x10: accumulated play time, indexed by stage */
    float damage[4];     /* 0x20: accumulated damage, indexed by stage */
} RgGameInfo;

/* RgDebugFlags comes from its definer, ov12/tu012 rg_debug_flags.c. */
#include "ov12/rg_debug_flags.h"

RgDebugFlags *InstanceOfRgDebugFlags(void);

/* Text-resource reader used only for the robot database's own read pass. */
typedef struct RgReadText RgReadText;
RgReadText *CreateRgReadText(const char *path);
void DisposeRgReadText(RgReadText *text);

/* Opaque database handles (ov12/rg_robot_db.c, rg_shot_db.c, rg_weapon_db.c). */
RgSimpleDB *InstanceOfRgRobotDB(void);
RgSimpleDB *InstanceOfRgShotDB(void);
RgSimpleDB *InstanceOfRgWeaponDB(void);
void RgRobotDBRead(RgSimpleDB *database, RgReadText *text);
void RgShotDBClear(RgSimpleDB *database);
void RgShotDBRead(RgSimpleDB *database, const char *path);
void RgWeaponDBClear(RgSimpleDB *database);
void RgWeaponDBRead(RgSimpleDB *database, const char *path);

/* jal RgWarn(fmt, file, line, value), the XrgLogSys-shaped debug warning. */
void RgWarn(const char *format, const char *file, int line, ...);

/* Frame-end presentation singletons (ov12/xrg_paint2d.c and siblings). */
typedef struct XrgPaint2D XrgPaint2D;
typedef struct XrgParticleDriver XrgParticleDriver;
XrgPaint2D *InstanceOfXrgPaint2D(void);
void XrgPaint2DFlush(XrgPaint2D *paint);
XrgParticleDriver *InstanceOfXrgParticleDriver(void);
void XrgParticleDriverDisp(XrgParticleDriver *driver);
RgDraw *InstanceOfRgDraw(void);
void RgDrawJob(RgDraw *draw);
void XrgSleep(void);

/*
 * Battle life display (ov12/tu043 rg_disp_life.c, which owns the layout).
 * RgDispLifeSetWin (still asm) asserts the object is non-null, stores the
 * two players' win counts and an OR-mask of which player's light to light.
 */
typedef struct RgDispLife RgDispLife;
void RgDispLifeSetWin(RgDispLife *dispLife, int playerResult0,
                      int playerResult1, int winMask);

/* Battle-effect environment (ov12/rg_shot_effect.c InstanceOfRgEffectEnv). */
typedef struct RgEffectEnv RgEffectEnv;
RgEffectEnv *InstanceOfRgEffectEnv(void);
void RgEffectEnvLoadData(RgEffectEnv *effectEnv);
void RgEffectEnvDisposeData(RgEffectEnv *effectEnv);

/*
 * Battle-common data environment (ov12/rg_battle_common_data.c
 * RgBattleCommonDataEnv; that TU's accepted RgBattleCommonDataDispose and
 * RgBattleCommonDataDispWeaponFont already take this same pointer type).
 */
typedef struct RgBattleCommonDataEnv RgBattleCommonDataEnv;
RgBattleCommonDataEnv *InstanceOfRgBattleCommonData(void);
void RgBattleCommonDataLoad(RgBattleCommonDataEnv *env);
void RgBattleCommonDataDispose(RgBattleCommonDataEnv *env);
int RgBattleCommonDataDispWeaponFont(RgBattleCommonDataEnv *env);

/* Motion-info database (ov12/rg_motion_info_db.c InstanceOfRgMotionInfoDB). */
typedef struct RgMotionInfoDB RgMotionInfoDB;
RgMotionInfoDB *InstanceOfRgMotionInfoDB(void);
void RgMotionInfoDBLoad(RgMotionInfoDB *db);
void RgMotionInfoDBDispose(RgMotionInfoDB *db);

/* Sound-sequence teardown (ov12/tu094 xrg_sound.c). */
void XrgSoundSystemDisposeSequence(int sequenceId);

/* Bitmap font draw (still asm), used here for the "HARD MODE" banner. */
void RgFontStr(XrgPaint2D *paint, int fontId, const char *text, int x, int y,
              int color);

#endif /* SRC_OV12_RG_MAIN_H */
