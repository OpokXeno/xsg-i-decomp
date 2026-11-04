/*
 * TU-local declarations of ov12/tu047 (src/ov12/rg_battlemgr.c).
 */

#ifndef SRC_OV12_RG_BATTLEMGR_H
#define SRC_OV12_RG_BATTLEMGR_H

#include "shared.h"
#include "ov12/rg_battle_init.h"
#include "ov12/rg_battlemgr.h"
#include "ov12/rg_camera.h"
#include "ov12/rg_disp_gameinfo.h"
#include "ov12/rg_disp_life.h"
#include "ov12/rg_disp_wpn1p.h"
#include "ov12/rg_disp_wpn2p.h"
#include "ov12/rg_draw.h"
#include "ov12/rg_game_collision.h"
#include "ov12/rg_geom_group.h"
#include "ov12/rg_player.h"
#include "ov12/rg_read_text.h"
#include "ov12/xrg_paint2d.h"

/* A battle's participant list, at RgBattleMgr+0x0c: the player count and the two player slots. */
typedef struct PlayerList {
    int count;                  /* +0x00, _GetNumOfPlayerList */
    union {
        struct {
            RgPlayer *player1P; /* +0x04 */
            RgPlayer *player2P; /* +0x08 */
        };
        RgPlayer *players[2];   /* +0x04 */
    };
} PlayerList;

/*
 * The battle field _CreateBattleField allocates (0x20 bytes): the active
 * studio count, the geometry group and, per studio, a draw studio, a
 * screen-ball geometry and a camera.
 */
/* Studios (and with them cameras and screen balls) a battle field can hold. */
#define BTL_STUDIO_MAX 2

typedef struct BattleField {
    int count;                /* +0x00, bound for the studio/camera indices */
    RgGeomGroup *geomGroup;   /* +0x04 */
    RgDrawStudio *studio[BTL_STUDIO_MAX];  /* +0x08 */
    RgGeomBall *screenBall[BTL_STUDIO_MAX];    /* +0x10 */
    RgCamera *camera[BTL_STUDIO_MAX];      /* +0x18 */
} BattleField;

/*
 * A round's display line (0x14 bytes): the battle mode, one RgDispGameInfo
 * per player and the two players it shows.
 */
typedef struct DispInfo {
    int mode;                       /* +0x00 */
    RgDispGameInfo *gameInfo[2];     /* +0x04 */
    union {
        struct {
            RgPlayer *player1P;     /* +0x0c */
            RgPlayer *player2P;     /* +0x10 */
        };
        RgPlayer *players[2];       /* +0x0c */
    };
} DispInfo;

/* The battle manager CreateRgBattleMgr allocates (0x38 bytes). */
struct RgBattleMgr {
    int mode;                       /* +0x00, RgBattleMgrGetMode */
    float remainingTime;            /* +0x04, RgBattleMgrGetPlayTime returns 180 - this */
    int result;                     /* +0x08, RgBattleMgrGetResult */
    PlayerList *playerList;         /* +0x0c, RgBattleMgrGetPlayerDamage */
    int playerControl;              /* +0x10, _BattleMgrPlayerControl */
    int timerActive;                /* +0x14, _BattleMgrActivateTime */
    DispInfo *dispInfo;             /* +0x18 */
    RgDispLife *dispLife;           /* +0x1c, RgBattleMgrGetDispLife */
    RgDispWpn1P *dispWpn1P;         /* +0x20 */
    RgDispWpn2P *dispWpn2P;         /* +0x24 */
    RgGameCollision *gameCollision; /* +0x28 */
    RgGeomGroup *geomGroup;         /* +0x2c */
    RgFileSysData *fileSysData;     /* +0x30 */
    BattleField *battleField;       /* +0x34 */
};

#endif /* SRC_OV12_RG_BATTLEMGR_H */
