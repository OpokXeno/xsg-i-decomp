/*
 * OV12 original TU 70: 0x00a3c6a0..0x00a3cd10 (7 functions)
 */
#include "common.h"
#include "rg_select_subcon.h"

static int s_bIgnoreEventFlag = 0;

extern int XrgEventIsUsablePlayer(unsigned int index);
extern int XrgEventIsUsableEnemy(unsigned int index);
extern int XrgRandIntRange(int lower, int upper);

void RgSelectIgnoreEventFlag(void)
{
    s_bIgnoreEventFlag = 1;
}

int RgSelectGetPlayerChars(void)
{
    unsigned int i;
    int mask;

    mask = 1;
    for (i = 0; i < 6; i++)
    {
        if (s_bIgnoreEventFlag || XrgEventIsUsablePlayer(i))
        {
            mask |= 1 << i;
        }
    }
    return mask;
}

int RgSelectGetEnemyChars(void)
{
    unsigned int i;
    int mask;

    mask = 1;
    for (i = 0; i < 6; i++)
    {
        if (s_bIgnoreEventFlag || XrgEventIsUsableEnemy(i))
        {
            mask |= 1 << i;
        }
    }
    return mask;
}

#include "shared.h"

static const char s_assertEssenceNil[] = "apEss != NIL";

static const char s_assertSourceFile[] = "../rg_select_subcon.euc.c";

static const char s_assertWeaponMax[] = "uNum <= WEP_MAX";

static const char s_assertWeaponZero[] = "uNum > 0";

extern RgSimpleDB *InstanceOfRgWeaponDB(void);
extern int XrgEventGetLevel(void);
extern int RgWeaponDBGetSize(RgSimpleDB *pDB);
extern void *RgWeaponDBGetIndex(RgSimpleDB *pDB, int nDataID);
extern int RgEquipIsEquipable(void *pEquip, int eType);
extern int XrgEventIsUsableWeapon(void *pEvent);
extern int XrgRandInt(void);
extern int RgEquipCheckConfrict(int *equipData, int slot);
extern void assert_prog(const char *expression, const char *source_file,
                        int line);

/*
 * Collects the database records usable by characterID with weaponType, in
 * database order, and returns their count. mode 1 keeps records whose
 * eventLevel reaches the current event level; mode 2 keeps records the event
 * allows or that are flagged usable; any other mode keeps every match. The
 * records are stored only when list is not null.
 */
static unsigned int _get_all_weapons(int *list, int characterID,
                                     int weaponType, int mode)
{
    RgSimpleDB *database;
    struct RgSelectWeaponRecord *weapon;
    int *out;
    int level;
    int characterMask;
    int matches;
    unsigned int index;
    unsigned int count;
    unsigned int size;

    index = 0;
    count = 0;
    database = InstanceOfRgWeaponDB();
    level = XrgEventGetLevel();
    size = RgWeaponDBGetSize(database);
    if (index < size)
    {
        characterMask = 1 << characterID;
        out = list;
        do
    {
        weapon = RgWeaponDBGetIndex(database, index);
        if ((weapon->characterMask & characterMask) != 0 &&
            RgEquipIsEquipable(weapon->equip, weaponType))
        {
            matches = 1;
            if (mode != 0)
            {
                if (mode == 1)
                {
                    matches = weapon->eventLevel >= level;
                }
                else if (mode == 2)
                {
                    matches = XrgEventIsUsableWeapon(weapon) || weapon->eventUsable != 0;
                }
            }
            if (matches)
            {
                if (list != 0)
                {
                    *out = (int)weapon;
                }
                out++;
                count++;
            }
        }
        index++;
        } while (index < size);
    }
    return count;
}

/* * Collects the database records usable by characterID with weaponType, in * database order, and returns their count. mode 1 keeps records whose * eventLevel reaches the current event level; mode 2 keeps records the event * allows or that are flagged usable; any other mode keeps every match. The * records are stored only when list is not null. */ static unsigned int _get_all_weapons(int *list, int characterID, int weaponType, int mode);

/*
 * Fills `list` with the weapons matching characterID/weaponType and returns
 * their count; the filter mode forwarded to _get_all_weapons switches with
 * s_bIgnoreEventFlag (0 while events are ignored, 2 otherwise).
 */
unsigned int RgSelectWeapons(int *list, int characterID, int weaponType)
{
    if (s_bIgnoreEventFlag != 0)
    {
        return _get_all_weapons(list, characterID, weaponType, 0);
    }
    return _get_all_weapons(list, characterID, weaponType, 2);
}

int RgSelectGetEnemyChar(void)
{
    int usableEnemies[6];
    int enemy;
    int usableCount;
    int maximumEnemy;
    int result;

    usableCount = 0;
    for (enemy = 0; enemy < 6; enemy++)
    {
        if (s_bIgnoreEventFlag || XrgEventIsUsableEnemy(enemy))
        {
            usableEnemies[usableCount] = enemy;
            usableCount++;
        }
    }

    if (usableCount == 0)
    {
        usableEnemies[0] = 0;
        usableCount = 1;
    }

    enemy = usableEnemies[XrgRandIntRange(0, usableCount)];
    maximumEnemy = 5;
    if (enemy < 0)
    {
        result = 0;
    }
    else if (enemy > maximumEnemy)
    {
        result = maximumEnemy;
    }
    else
    {
        result = enemy;
    }
    return result;
}

extern int rand(void);

int RgSelectGetEnemyWeapons(struct RgSelectWeaponRecord **weapons,
                            int characterID)
{
    int weaponLists[3][128];
    unsigned int counts[3];
    unsigned int indices[3];
    unsigned int selected[3];
    int slots[3];
    unsigned int i;
    int j;
    unsigned int k;
    unsigned int m;
    int flag;
    unsigned int count;
    int mode;

    if (weapons == 0)
    {
        assert_prog(s_assertEssenceNil, s_assertSourceFile, 133);
    }
    for (i = 0; i < 3; i++)
    {
        mode = s_bIgnoreEventFlag == 0;
        count = _get_all_weapons(0, characterID, i, mode);
        if (count > 128)
        {
            assert_prog(s_assertWeaponMax, s_assertSourceFile, 144);
        }
        if (count == 0)
        {
            assert_prog(s_assertWeaponZero, s_assertSourceFile, 145);
        }
        counts[i] = _get_all_weapons(weaponLists[i], characterID, i, mode);
    }
    for (i = 0; i < 3; i++)
    {
        if (counts[i] == 0)
        {
            return 0;
        }
        indices[i] = rand() % counts[i];
    }
    for (i = 0; i < counts[2]; i++)
    {
        k = 0;
        selected[2] = (i + indices[2]) % counts[2];
        for (; k < counts[0]; k++)
        {
            m = 0;
                selected[0] = (k + indices[0]) % counts[0];
                for (; m < counts[1]; m++)
                    {
                        selected[1] = (m + indices[1]) % counts[1];
                        for (j = 0; j < 3; j++)
                        {
                            slots[j] = (int)((struct RgSelectWeaponRecord *)weaponLists[j][selected[j]])->equip;
                        }
                        flag = 0;
                        for (j = 0; j < 3; j++)
                        {
                            flag |= RgEquipCheckConfrict(slots, j);
                        }
                        if (flag == 0)
                        {
                            for (j = 0; j < 3; j++)
                            {
                                weapons[j] = (struct RgSelectWeaponRecord *)weaponLists[j][selected[j]];
                            }
                            return 1;
                        }
                    }
            }
    }
    return 0;
}
