/*
 * OV12 original TU 95: 0x00a4f280..0x00a4f4c4 (5 functions)
 */
#include "common.h"
#include "xrg_event.h"

struct XrgEventWeaponEntry {
    const char *weapon_name;
    int weapon_id;
};

/* Defined here: the weapon names below the functions. */
extern const char eventWeaponSwd21agName[];
extern const char eventWeaponLg24vxName[];
extern const char eventWeaponHg45vxName[];
extern const char eventWeaponLg10agName[];
extern const char eventWeaponSmg99agName[];
extern const char eventWeaponGrd20agName[];
extern const char eventWeaponHmp33agName[];
extern const char eventWeaponPb55agName[];
extern const char eventWeaponShd02agName[];
extern const char eventWeaponShd12vxName[];
extern const char eventWeaponLg100vxName[];
extern const char eventWeaponSwd34vxName[];
extern const char eventWeaponHmr55agName[];
extern const char eventWeaponCb85vxName[];
extern const char eventWeaponEcm1VxName[];
extern const char eventWeaponErVxName[];
extern const char eventWeaponBmp45vxName[];
extern const char eventWeaponHg75vxName[];
extern const char eventWeaponGlg76agName[];
extern const char eventWeaponFlm64agName[];
extern const char eventWeaponDlc02ag4Name[];
extern const char eventWeaponWct02ag4Name[];
extern const char eventWeaponShb67agName[];
extern const char eventWeaponBsw13agName[];
extern const char eventWeaponBl24agName[];
extern const char eventWeaponAxe11agName[];
extern const char eventWeaponSmp53agName[];
extern const char eventWeaponSmg32vxName[];
extern const char eventWeaponEcm2VxName[];
extern const char eventWeaponLm11vxName[];
extern const char eventWeaponBa15vxName[];
extern const char eventWeaponAirdAg2Name[];
extern const char eventWeaponHmrAg5Name[];
extern const char eventWeaponAircAg2Name[];
extern const char eventWeaponBmpAg5Name[];
extern const char eventWeaponHmpAg5Name[];
extern const char eventWeaponHggAg5Name[];
extern const char eventWeaponBbcAg5Name[];
extern const char eventWeaponLcAg5Name[];
extern const char eventWeaponLwVx4Name[];
extern const char eventWeaponDefVxName[];

#define XRG_EVENT_WEAPON_COUNT 41
#define XRG_EVENT_PARTY_MAX 16


static int s_bSetEventLevel = 0;

static int s_eEventLevel = 0;

static struct XrgEventWeaponEntry s_aWepTbl_0[XRG_EVENT_WEAPON_COUNT] = {
    { eventWeaponSwd21agName, 72 }, { eventWeaponLg24vxName, 83 }, { eventWeaponHg45vxName, 75 },
    { eventWeaponLg10agName, 82 }, { eventWeaponSmg99agName, 80 }, { eventWeaponGrd20agName, 90 },
    { eventWeaponHmp33agName, 93 }, { eventWeaponPb55agName, 88 }, { eventWeaponShd02agName, 102 },
    { eventWeaponShd12vxName, 103 }, { eventWeaponLg100vxName, 84 }, { eventWeaponSwd34vxName, 73 },
    { eventWeaponHmr55agName, 77 }, { eventWeaponCb85vxName, 96 }, { eventWeaponEcm1VxName, 106 },
    { eventWeaponErVxName, 109 }, { eventWeaponBmp45vxName, 98 }, { eventWeaponHg75vxName, 76 },
    { eventWeaponGlg76agName, 91 }, { eventWeaponFlm64agName, 89 }, { eventWeaponDlc02ag4Name, 74 },
    { eventWeaponWct02ag4Name, 78 }, { eventWeaponShb67agName, 101 }, { eventWeaponBsw13agName, 86 },
    { eventWeaponBl24agName, 95 }, { eventWeaponAxe11agName, 71 }, { eventWeaponSmp53agName, 108 },
    { eventWeaponSmg32vxName, 81 }, { eventWeaponEcm2VxName, 107 }, { eventWeaponLm11vxName, 87 },
    { eventWeaponBa15vxName, 85 }, { eventWeaponAirdAg2Name, 105 }, { eventWeaponHmrAg5Name, 79 },
    { eventWeaponAircAg2Name, 0 },  { eventWeaponBmpAg5Name, 100 }, { eventWeaponHmpAg5Name, 94 },
    { eventWeaponHggAg5Name, 92 }, { eventWeaponBbcAg5Name, 104 }, { eventWeaponLcAg5Name, 99 },
    { eventWeaponLwVx4Name, 110 }, { eventWeaponDefVxName, 111 },
};

/* Sets the event level that XrgEventGetLevel returns. XRG_EVENT_LEVEL_NONE clears it. */
void XrgSetEventLevel(int level)
{
    if (level == XRG_EVENT_LEVEL_NONE) {
        s_bSetEventLevel = 0;
        return;
    }
    s_bSetEventLevel = 1;
    s_eEventLevel = level;
}

/* Returns the level set by XrgSetEventLevel, or else a level derived from two event flags. */
int XrgEventGetLevel(void)
{
    int usable;

    if (s_bSetEventLevel != 0) {
        return s_eEventLevel;
    }
    usable = xglFlagsGet1(0x65);
    return (xglFlagsGet1(0x12D) == 0) ? (usable != 0) : 2;
}

/* Returns whether enemy index may appear at the current event level. */
int XrgEventIsUsableEnemy(unsigned int index)
{
    int level;

    level = XrgEventGetLevel();
    switch (index) {
    case 2:
        return level > 0;
    case 0:
        return level >= 0;
    case 5:
        return level >= 0;
    case 1:
        return level >= 2;
    case 3:
        return level >= 2;
    case 4:
        return level >= 2;
    default:
        return 0;
    }
}

/* Returns whether the character at character_index is in the current party. */
int XrgEventIsUsablePlayer(unsigned int character_index)
{
    int party_ids[XRG_EVENT_PARTY_MAX];
    int character_id;
    int party_count;
    int i;

    switch (character_index) {
    case 0:
        character_id = MENU_CHAR_VX_10000;
        break;
    case 4:
        character_id = MENU_CHAR_VX_4000;
        break;
    case 2:
        character_id = MENU_CHAR_VX_7000;
        break;
    case 5:
        character_id = MENU_CHAR_AG_02;
        break;
    case 3:
        character_id = MENU_CHAR_AG_04;
        break;
    case 1:
        character_id = MENU_CHAR_AG_05;
        break;
    default:
        return 0;
    }

    party_count = PartyAgwsGet(party_ids);
    for (i = 0; i < party_count; i++) {
        if (party_ids[i] == character_id) {
            return 1;
        }
    }
    return 0;
}

/*
 * Returns 1 when weapon may be used. A null weapon is usable; otherwise its name
 * must match a table entry whose weapon box check passes.
 */
int XrgEventIsUsableWeapon(RgWeaponEssence *weapon)
{
    const struct XrgEventWeaponEntry *entry;
    const char *weapon_name;
    int weapon_id;
    unsigned int i;

    if (weapon == 0) {
        return 1;
    }

    weapon_name = weapon->databaseName;
    entry = s_aWepTbl_0;
    for (i = 0; i < XRG_EVENT_WEAPON_COUNT; i++, entry++) {
        weapon_id = entry->weapon_id;
        if (strcasecmp(entry->weapon_name, weapon_name) == 0) {
            if (dataWpnBoxChk(weapon_id) > 0) {
                return 1;
            }
        }
    }
    return 0;
}

const char eventWeaponDefVxName[8] = "DEF-VX";

const char eventWeaponLwVx4Name[8] = "LW-VX4";

const char eventWeaponLcAg5Name[8] = "LC-AG5";

const char eventWeaponBbcAg5Name[8] = "BBC-AG5";

const char eventWeaponHggAg5Name[8] = "HGG-AG5";

const char eventWeaponHmpAg5Name[8] = "HMP-AG5";

const char eventWeaponBmpAg5Name[8] = "BMP-AG5";

const char eventWeaponAircAg2Name[16] = "AIRC-AG2";

const char eventWeaponHmrAg5Name[8] = "HMR-AG5";

const char eventWeaponAirdAg2Name[16] = "AIRD-AG2";

const char eventWeaponBa15vxName[8] = "BA15VX";

const char eventWeaponLm11vxName[8] = "LM11VX";

const char eventWeaponEcm2VxName[8] = "ECM2-VX";

const char eventWeaponSmg32vxName[8] = "SMG32VX";

const char eventWeaponSmp53agName[8] = "SMP53AG";

const char eventWeaponAxe11agName[8] = "AXE11AG";

const char eventWeaponBl24agName[8] = "BL24AG";

const char eventWeaponBsw13agName[8] = "BSW13AG";

const char eventWeaponShb67agName[8] = "SHB67AG";

const char eventWeaponWct02ag4Name[16] = "WCT02AG4";

const char eventWeaponDlc02ag4Name[16] = "DLC02AG4";

const char eventWeaponFlm64agName[8] = "FLM64AG";

const char eventWeaponGlg76agName[8] = "GLG76AG";

const char eventWeaponHg75vxName[8] = "HG75VX";

const char eventWeaponBmp45vxName[8] = "BMP45VX";

const char eventWeaponErVxName[8] = "ER-VX";

const char eventWeaponEcm1VxName[8] = "ECM1-VX";

const char eventWeaponCb85vxName[8] = "CB85VX";

const char eventWeaponHmr55agName[8] = "HMR55AG";

const char eventWeaponSwd34vxName[8] = "SWD34VX";

const char eventWeaponLg100vxName[8] = "LG100VX";

const char eventWeaponShd12vxName[8] = "SHD12VX";

const char eventWeaponShd02agName[8] = "SHD02AG";

const char eventWeaponPb55agName[8] = "PB55AG";

const char eventWeaponHmp33agName[8] = "HMP33AG";

const char eventWeaponGrd20agName[8] = "GRD20AG";

const char eventWeaponSmg99agName[8] = "SMG99AG";

const char eventWeaponLg10agName[8] = "LG10AG";

const char eventWeaponHg45vxName[8] = "HG45VX";

const char eventWeaponLg24vxName[8] = "LG24VX";

const char eventWeaponSwd21agName[8] = "SWD21AG";
