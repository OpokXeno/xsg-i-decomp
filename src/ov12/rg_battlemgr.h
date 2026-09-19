/*
 * TU-local declarations of ov12/tu047 (src/ov12/rg_battlemgr.c).
 */

#ifndef SRC_OV12_RG_BATTLEMGR_H
#define SRC_OV12_RG_BATTLEMGR_H

#include "shared.h"

/* Opaque handles owned by other TUs (RgPlayerGetRobot, RgDispLifeSetRobot). */
typedef struct RgPlayer RgPlayer;
typedef struct RgDispLife RgDispLife;

/*
 * Opaque handles owned by other TUs: this allocation only creates, forwards
 * or disposes of them, it never reads or writes a member.
 *   RgCamera     - ov12/tu016 (src/ov12/rg_camera.h), CreateRgCamera's result
 *   RgReadText   - ov12/tu036 (src/ov12/rg_read_text.h), the paragraph reader
 *   RgDrawStudio - ov12/tu045 (include/ov12/rg_draw.h), _CreateBattleCamera's
 *                  argument (assert message "pStudio != NIL")
 *   RgGeomGroup  - ov12/tu059 (src/ov12/rg_geom_group.h), BattleField's
 *                  geometry group
 */
typedef struct RgCamera RgCamera;
typedef struct RgReadText RgReadText;
typedef struct RgDrawStudio RgDrawStudio;
typedef struct RgGeomGroup RgGeomGroup;

/*
 * Opaque handles owned by other TUs: RgBattleMgrGetPlayerDamage only ever
 * forwards a robot pointer to RgPlayerGetRobot/RgRobotGetLife/
 * RgRobotGetLifeMax, it never reads or writes a member of it. Declared here
 * as RgRobot, this TU's own name for the pointer (src/ov12/rg_player.c
 * passes the very same pointer, uncast, to RgRobotGetActor's own local
 * extern of it as RgRobot too); rg_robot.c's own TU names it RgStatus.
 */
typedef struct RgRobot RgRobot;

/*
 * A battle's participant list, embedded (as a pointer) at RgBattleMgr+0x0c.
 * _InitPlayerList's three sequential stores (offsets 0x00, 0x04, 0x08) and
 * _ClearPlayerList's own reads/stores of the same three offsets (still
 * INCLUDE_ASM in this allocation; loops over the two player slots, then
 * clears count) are the only evidence for this type so far.
 */
typedef struct PlayerList {
    int count;           /* +0x00, _GetNumOfPlayerList */
    RgPlayer *player1P;  /* +0x04, cleared by _InitPlayerList/_ClearPlayerList */
    RgPlayer *player2P;  /* +0x08, cleared by _InitPlayerList/_ClearPlayerList */
} PlayerList;

/*
 * The battle field _CreateBattleField allocates (RgHeapAlloc size 0x20,
 * exactly this type's size) and _InitBattleField (still INCLUDE_ASM) fills.
 * Only the geometry-group pointer this allocation's _DestructBattleField/
 * _GetGeomGroupBattleField read or dispose of is modeled here.
 */
typedef struct BattleField {
    unsigned char unmodeled_00[4];  /* +0x00 */
    RgGeomGroup *geomGroup;         /* +0x04 */
    unsigned char unmodeled_08[24]; /* +0x08, rest of the 0x20-byte allocation */
} BattleField;

/*
 * A single round's display line, allocated by _CreateDispInfo (RgHeapAlloc
 * size 0x14, exactly this type's size) and filled by _SetDispInfo, which
 * _InitBattle calls with the battle mode value and the first two players
 * RgBattleInitCreatePlayers() stores (the sibling accessors
 * RgDispWpn1PSetRobot/RgDispWpn2PSetRobot name the same two player slots
 * "1P"/"2P"). Bytes 0x04..0x0b hold the two per-player RgDispGameInfo
 * handles that _InitDispInfo creates and _DestructDispInfo disposes (both
 * still INCLUDE_ASM); none of this allocation's functions touch them.
 */
typedef struct DispInfo {
    int mode;                       /* +0x00 */
    unsigned char unmodeled_04[8];  /* +0x04 */
    RgPlayer *player1P;             /* +0x0c */
    RgPlayer *player2P;             /* +0x10 */
} DispInfo;

/*
 * The battle manager CreateRgBattleMgr allocates (RgHeapAlloc size 0x38) and
 * _InitBattleMgr fills. Only the members this allocation's accessors read or
 * write are modeled here; _InitBattleMgr/_DisposeBattleMgr (still
 * INCLUDE_ASM) own the rest of the 0x38-byte object. Offsets 0x0c (the
 * player list) and 0x18 (the DispInfo) are used by _InitBattle, still
 * INCLUDE_ASM, and are gaps only as far as this allocation's code goes.
 *
 * This allocation also models +0x0c: RgBattleMgrGetPlayerDamage reads the
 * player-list pointer there and forwards it to _GetInPlayerList (still
 * INCLUDE_ASM) to look up the player owning the requested robot.
 */
typedef struct RgBattleMgr {
    int mode;                       /* +0x00, RgBattleMgrGetMode */
    float remainingTime;            /* +0x04, RgBattleMgrGetPlayTime returns 180 - this */
    int result;                     /* +0x08, RgBattleMgrGetResult */
    PlayerList *playerList;         /* +0x0c, RgBattleMgrGetPlayerDamage */
    int playerControl;              /* +0x10, _BattleMgrPlayerControl */
    int timerActive;                /* +0x14, _BattleMgrActivateTime */
    unsigned char unmodeled_18[4];  /* +0x18 */
    RgDispLife *dispLife;           /* +0x1c, RgBattleMgrGetDispLife */
} RgBattleMgr;

#endif /* SRC_OV12_RG_BATTLEMGR_H */
