#ifndef INCLUDE_OV12_RG_ANNOUNCE_H
#define INCLUDE_OV12_RG_ANNOUNCE_H

/* The battle announcement banner object; its layout is private to src/ov12/rg_announce.c. */
typedef struct RgAnnounce RgAnnounce;

/*
 * Banner kinds RgAnnounceDispInit selects; each indexes the table of draw
 * callbacks in src/ov12/rg_announce.c (_round1 .. _you_lose_you_win).
 */
#define RG_ANNOUNCE_ROUND1 0
#define RG_ANNOUNCE_ROUND2 1
#define RG_ANNOUNCE_ROUND3 2
#define RG_ANNOUNCE_READY 3
#define RG_ANNOUNCE_FIGHT 4
#define RG_ANNOUNCE_YOU_WIN 5
#define RG_ANNOUNCE_YOU_LOSE 6
#define RG_ANNOUNCE_DRAW 7
#define RG_ANNOUNCE_GAME_OVER 8
#define RG_ANNOUNCE_TIME_OVER 9
#define RG_ANNOUNCE_STAGE_CLEAR 10
#define RG_ANNOUNCE_GAME_CLEAR 11
#define RG_ANNOUNCE_YOU_WIN_YOU_LOSE 12
#define RG_ANNOUNCE_YOU_LOSE_YOU_WIN 13

/* Functions and data used by other translation units. */
RgAnnounce *CreateRgAnnounce(void);
void DisposeRgAnnounce(RgAnnounce *pAnn);
void RgAnnouncePassTime(RgAnnounce *pAnn, float deltaTime);
void RgAnnounceDisp(RgAnnounce *pAnn);
void RgAnnounceDispInit(RgAnnounce *pAnn, int kind);
void RgAnnounceDispInitTime(RgAnnounce *pAnn, int kind, float duration);
void RgAnnounceDispOff(RgAnnounce *pAnn);
void RgAnnounceSetStageClear(RgAnnounce *pAnn, int time, float damage);
void RgAnnounceSetGameClear(RgAnnounce *pAnn, const unsigned int *auTime,
                            const float *afDamage, unsigned int uStage);

#endif /* INCLUDE_OV12_RG_ANNOUNCE_H */
