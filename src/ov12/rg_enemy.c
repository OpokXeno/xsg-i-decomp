/*
 * OV12 original TU 6: 0x00a09ba0..0x00a0cbc8 (9 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_enemy.h"

static int _GetPadEdge(void)
{
    return 0;
}

static int _GetPadRelease(void)
{
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_enemy", _dbgThinkTool);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_enemy", _GetWeaponTypeFlag);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_enemy", _CheckShootWeapon);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_enemy", _JobEnemy);

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

extern const char D_00A51D90[]; /* "../rg_enemy.euc.c" */
extern const char D_00A51DD0[]; /* "pBaka != NIL" */

static void _DestructEnemy(RgEnemyControl *pBaka)
{
    if (pBaka == 0) {
        assert_prog(D_00A51DD0, D_00A51D90, 1562);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_enemy", _InitEnemy);

extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size,
                         const char *source_file, int line);

extern const char D_00A51DE0[]; /* "pRobot != NIL" */
extern const char D_00A51DF0[]; /* "pEnemyRobot != NIL" */

/*
 * _InitEnemy is defined later in this TU (a local sibling still in asm).
 * enemyType is forwarded to it untouched and unasserted; no stronger
 * evidence for its role exists within this allocation.
 */
static void _InitEnemy(RgEnemyControl *pControl, RgRobot *pRobot,
                       RgRobot *pEnemyRobot, int enemyType);

RgEnemyControl *CreateRgEnemyControl(RgRobot *pRobot, RgRobot *pEnemyRobot,
                                     int enemyType)
{
    RgEnemyControl *pControl;

    if (pRobot == 0) {
        assert_prog(D_00A51DE0, D_00A51D90, 1612);
    }
    if (pEnemyRobot == 0) {
        assert_prog(D_00A51DF0, D_00A51D90, 1613);
    }
    pControl = RgHeapAlloc(InstanceOfRgHeap(), RG_ENEMY_CONTROL_SIZE,
                           D_00A51D90, 1615);
    _InitEnemy(pControl, pRobot, pEnemyRobot, enemyType);
    return pControl;
}
