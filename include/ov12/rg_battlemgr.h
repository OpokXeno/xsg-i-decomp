#ifndef INCLUDE_OV12_RG_BATTLEMGR_H
#define INCLUDE_OV12_RG_BATTLEMGR_H

struct RgDispLife;

struct RgBattleInit;

/* The battle manager; its layout is private to src/ov12/rg_battlemgr.c. */
typedef struct RgBattleMgr RgBattleMgr;

/*
 * RgBattleMgr.mode is RgBattleInit.mode: the bit mask of human-controlled
 * players. Modes 0 and 3 (nobody or both) play on a split screen.
 */
#define RG_BATTLE_MODE_NO_HUMANS 0
#define RG_BATTLE_MODE_PLAYER1_ONLY 1
#define RG_BATTLE_MODE_PLAYER2_ONLY 2
#define RG_BATTLE_MODE_BOTH_HUMANS 3

/*
 * RgBattleMgrGetResult: zero while the battle runs. The low two bits name the
 * winner, the next two how the battle ended.
 */
#define RG_BATTLE_RESULT_WINNER_MASK 3
#define RG_BATTLE_RESULT_PLAYER1_WINS 1
#define RG_BATTLE_RESULT_PLAYER2_WINS 2
#define RG_BATTLE_RESULT_DRAW 3
#define RG_BATTLE_RESULT_ENDED_MASK 12
#define RG_BATTLE_RESULT_TIME_OVER 4
#define RG_BATTLE_RESULT_KNOCKOUT 8

/* Fixed time step of the 30 fps battle and menu screens, in seconds. */
#define RG_FRAME_TIME 0.033333335f

/* Functions and data used by other translation units. */
RgBattleMgr *CreateRgBattleMgr(void);
void DisposeRgBattleMgr(RgBattleMgr *pMgr);
void RgBattleMgrSetPlayerControl(RgBattleMgr *pMgr, int enable);
void RgBattleMgrActivateTimer(RgBattleMgr *pMgr, int active);
float RgBattleMgrGetPlayTime(RgBattleMgr *pMgr);
int RgBattleMgrGetResult(RgBattleMgr *pMgr);
int RgBattleMgrGetMode(RgBattleMgr *pMgr);
struct RgDispLife *RgBattleMgrGetDispLife(RgBattleMgr *pMgr);
float RgBattleMgrGetPlayerDamage(RgBattleMgr *pMgr, int playerIndex);
void RgBattleMgrInitBattle(RgBattleMgr *pMgr, struct RgBattleInit *pInfo);
void RgBattleMgrControl(RgBattleMgr *pMgr);
void RgBattleMgrPassTime(RgBattleMgr *pMgr, float deltaTime);
void RgBattleMgrDisp(RgBattleMgr *pMgr);

#endif /* INCLUDE_OV12_RG_BATTLEMGR_H */
