/*
 * OV12 original TU 70: 0x00a3c6a0..0x00a3cd10 (7 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_select_subcon.h"
#include "ov12/rg_weapon_db.h"
#include "ov12/xrg_event.h"
#include "ov12/xrg_rand_int.h"

/* Selection filter modes of _get_all_weapons. */
#define WEAPON_FILTER_ALL 0
#define WEAPON_FILTER_EVENT_LEVEL 1
#define WEAPON_FILTER_EVENT_USABLE 2

/* Highest character and enemy index (characters 0-5). */
#define SELECT_CHAR_LAST (RG_SELECT_CHAR_NUM - 1)


static int s_bIgnoreEventFlag = 0;

static const char s_assertEssenceNil[] = "apEss != NIL";

static const char s_assertSourceFile[] = "../rg_select_subcon.euc.c";

static const char s_assertWeaponMax[] = "uNum <= WEP_MAX";

static const char s_assertWeaponZero[] = "uNum > 0";

static unsigned int _get_all_weapons(RgWeaponEssence **list,
                                     int characterID, int weaponType,
                                     int mode);

void RgSelectIgnoreEventFlag(void)
{
    s_bIgnoreEventFlag = 1;
}

/*
 * Returns a bit mask of the player characters the event allows, bit i for
 * character i. Bit 0 is always set.
 */
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

/*
 * Returns a bit mask of the enemy characters the event allows, bit i for
 * character i. Bit 0 is always set.
 */
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

/*
 * Collects the database records usable by characterID with weaponType, in
 * database order, and returns their count. WEAPON_FILTER_EVENT_LEVEL keeps
 * records whose eventLevel reaches the current event level;
 * WEAPON_FILTER_EVENT_USABLE keeps records the event allows or that are
 * default; any other mode keeps every match. The records are stored only when
 * list is not null.
 */
static unsigned int _get_all_weapons(RgWeaponEssence **list,
                                     int characterID, int weaponType,
                                     int mode)
{
    RgSimpleDB *database;
    RgWeaponEssence *weapon;
    RgWeaponEssence **out;
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
    if (index < size) {
        characterMask = 1 << characterID;
        out = list;
        do {
            weapon = RgWeaponDBGetIndex(database, index);
            if ((weapon->useMask & characterMask) != 0 &&
                RgEquipIsEquipable(&weapon->equip, weaponType)) {
                matches = 1;
                if (mode != WEAPON_FILTER_ALL) {
                    if (mode == WEAPON_FILTER_EVENT_LEVEL) {
                        matches = weapon->eventLevel >= level;
                    } else if (mode == WEAPON_FILTER_EVENT_USABLE) {
                        matches = XrgEventIsUsableWeapon(weapon) ||
                                  weapon->isDefault != 0;
                    }
                }
                if (matches) {
                    if (list != 0) {
                        *out = weapon;
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

/*
 * Fills `list` with the weapons that fit characterID/weaponType and returns
 * their count; the filter mode forwarded to _get_all_weapons switches with
 * s_bIgnoreEventFlag (0 while events are ignored, 2 otherwise).
 */
unsigned int RgSelectWeapons(RgWeaponEssence **list,
                             int characterID, int weaponType)
{
    if (s_bIgnoreEventFlag != 0) {
        return _get_all_weapons(list, characterID, weaponType,
                                WEAPON_FILTER_ALL);
    }
    return _get_all_weapons(list, characterID, weaponType,
                            WEAPON_FILTER_EVENT_USABLE);
}

/*
 * Returns a random enemy character index among those the event allows, or 0
 * when none is allowed. The result is clamped to the character range.
 */
int RgSelectGetEnemyChar(void)
{
    int usableEnemies[RG_SELECT_CHAR_NUM];
    int enemy;
    int usableCount;
    int maximumEnemy;
    int result;

    usableCount = 0;
    for (enemy = 0; enemy < RG_SELECT_CHAR_NUM; enemy++) {
        if (s_bIgnoreEventFlag || XrgEventIsUsableEnemy(enemy)) {
            usableEnemies[usableCount] = enemy;
            usableCount++;
        }
    }

    if (usableCount == 0) {
        usableEnemies[0] = 0;
        usableCount = 1;
    }

    enemy = usableEnemies[XrgRandIntRange(0, usableCount)];
    maximumEnemy = SELECT_CHAR_LAST;
    if (enemy < 0) {
        result = 0;
    } else if (enemy > maximumEnemy) {
        result = maximumEnemy;
    } else {
        result = enemy;
    }
    return result;
}

/*
 * Picks one weapon per equipment slot for characterID, starting from random
 * offsets, so that no two picked equipments conflict. Stores them in weapons
 * and returns 1; returns 0 when a slot has no candidates or no conflict-free
 * combination exists.
 */
int RgSelectGetEnemyWeapons(RgWeaponEssence **weapons,
                            int characterID)
{
    RgWeaponEssence *weaponLists[RG_SELECT_WEAPON_SLOTS]
                                                 [RG_SELECT_WEAPON_MAX];
    unsigned int counts[RG_SELECT_WEAPON_SLOTS];
    unsigned int indices[RG_SELECT_WEAPON_SLOTS];
    unsigned int selected[RG_SELECT_WEAPON_SLOTS];
    RgEquip *slots[RG_SELECT_WEAPON_SLOTS];
    unsigned int i;
    int j;
    unsigned int k;
    unsigned int m;
    int flag;
    unsigned int count;
    int mode;

    if (weapons == 0) {
        assert_prog(s_assertEssenceNil, s_assertSourceFile, 133);
    }
    for (i = 0; i < RG_SELECT_WEAPON_SLOTS; i++) {
        mode = s_bIgnoreEventFlag == 0;
        count = _get_all_weapons(0, characterID, i, mode);
        if (count > RG_SELECT_WEAPON_MAX) {
            assert_prog(s_assertWeaponMax, s_assertSourceFile, 144);
        }
        if (count == 0) {
            assert_prog(s_assertWeaponZero, s_assertSourceFile, 145);
        }
        counts[i] = _get_all_weapons(weaponLists[i], characterID, i, mode);
    }
    for (i = 0; i < RG_SELECT_WEAPON_SLOTS; i++) {
        if (counts[i] == 0) {
            return 0;
        }
        indices[i] = rand() % counts[i];
    }
    /* Tries each combination, starting from the random offset of each slot. */
    for (i = 0; i < counts[2]; i++) {
        k = 0;
        selected[2] = (i + indices[2]) % counts[2];
        for (; k < counts[0]; k++) {
            m = 0;
            selected[0] = (k + indices[0]) % counts[0];
            for (; m < counts[1]; m++) {
                selected[1] = (m + indices[1]) % counts[1];
                for (j = 0; j < RG_SELECT_WEAPON_SLOTS; j++) {
                    slots[j] = &weaponLists[j][selected[j]]->equip;
                }
                flag = 0;
                for (j = 0; j < RG_SELECT_WEAPON_SLOTS; j++) {
                    flag |= RgEquipCheckConfrict(slots, j);
                }
                if (flag == 0) {
                    for (j = 0; j < RG_SELECT_WEAPON_SLOTS; j++) {
                        weapons[j] = weaponLists[j][selected[j]];
                    }
                    return 1;
                }
            }
        }
    }
    return 0;
}
