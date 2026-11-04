/*
 * TU-local declarations of ov12/tu043 (src/ov12/rg_disp_life.c).
 */

#ifndef SRC_OV12_RG_DISP_LIFE_H
#define SRC_OV12_RG_DISP_LIFE_H

#include "shared.h"
#include "ov12/rg_bxx.h"

struct RgBxx;

/*
 * The gauge handle owned by ov12/rg_gauge.c; this allocation only disposes
 * it (still created by _InitDisp, still INCLUDE_ASM), so an incomplete type
 * is enough here.
 */
typedef struct RgGauge RgGauge;

/*
 * The battle life/vs-mode display object CreateRgDispLife allocates
 * (RgHeapAlloc size 0x60) and _InitDisp (still INCLUDE_ASM) fills. Only the
 * members this allocation's own accessors read or write are modeled here:
 *   robot1P  +0x14  RgDispLifeSetRobot's first robot argument, the same
 *            RgStatus * _InitBattle also passes as RgDispWpn2PSetRobot's
 *            pRobot1 (the "1P" player)
 *   robot2P  +0x18  RgDispLifeSetRobot's second robot argument, the same
 *            value passed as RgDispWpn2PSetRobot's pRobot2 ("2P")
 *   timer    +0x44  RgDispLifeSetTimer's argument, stored only when it is
 *            not negative
 * _InitDisp/_DestructDisp/RgDispLifeSetWin/RgDispLifeSetVsMode/
 * RgDispLifePassTime/RgDispLifeDisp (still INCLUDE_ASM) own the rest of the
 * 0x60-byte object; +0x00..+0x13 and +0x1c..+0x43 are gaps only as far as
 * this allocation's own functions go.
 */
/*
 * _DestructDisp (this allocation) resolves two more members the comment
 * above still describes as part of the +0x1c..+0x43 gap, plus one member
 * right after timer:
 *   gauge[2]  +0x1c  the two RgGauge handles its own loop disposes, one
 *             DisposeRgGauge(pDisp->gauge[i]) call per element
 *   paint     +0x48  the XrgPaint2D handle DisposeXrgPaint2D_sub releases,
 *             immediately after timer
 * +0x24..+0x43 is still an unmodeled gap.
 */
typedef struct RgDispLife {
    union {
        struct {
            unsigned char unmodeled_00[0x14]; /* +0x00 */
        };
        struct {
            struct RgBxx *dispBxx;             /* +0x00 */
            int timeFont;                     /* +0x04 */
            void *primaryBoardPic;              /* +0x08 */
            void *secondaryBoardPic;            /* +0x0c */
            void *agwsNamePic;                /* +0x10 */
        };
    };
    union {
        struct {
            RgStatus *robot1P;               /* +0x14 */
            RgStatus *robot2P;               /* +0x18 */
        };
        RgStatus *robots[2];                 /* +0x14 */
    };
    RgGauge *gauge[2];                       /* +0x1c */
    void *lifePic[8];                        /* +0x24 */
    float timer;                             /* +0x44 */
    void *paint;                             /* +0x48 */
    int vsMode;                              /* +0x4c */
    int winCount[2];                         /* +0x50 */
    int winnerMask;                          /* +0x58 */
    float elapsedTime;                       /* +0x5c */
} RgDispLife;

/* RgDispLifeSetVsMode uses dispBxx +0x00, board pictures +0x08/+0x0c, and
 * vsMode +0x4c. RgDispLifePassTime updates elapsedTime +0x5c.
 * RgDispLifeSetWin (0x00a263b8) stores each player's win tally, clamped to
 * 0..2, in winCount[0]/[1] (+0x50/+0x54), and its fourth argument in
 * winnerMask (+0x58). rg_main's _GameInfoSetDispLife passes
 * RgGameInfo.playerResult[0]/[1] and either 0 or `1 << winner`. These members
 * are the additional ASM-offset-evidenced view of the original 0x60 object. */

#endif /* SRC_OV12_RG_DISP_LIFE_H */
