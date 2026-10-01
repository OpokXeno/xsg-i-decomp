/*
 * OV12 original TU 95: 0x00a4f280..0x00a4f4c4 (5 functions)
 */
#include "common.h"
#include "xrg_event.h"

struct XrgEventWeaponEntry {
    const char *weapon_name;
    int weapon_id;
};

struct XrgEventWeaponFilter {
    unsigned char unmodeled_00[0x330];
    char weapon_name;
};

extern const struct XrgEventWeaponEntry s_aWepTbl_0[41];
extern int PartyAgwsGet(int *party_ids);
extern int strcasecmp(const char *left, const char *right);
extern int dataWpnBoxChk(int weapon_id);

void XrgSetEventLevel(int level)
{
    if (level == -1) {
        s_bSetEventLevel = 0;
        return;
    }
    s_bSetEventLevel = 1;
    s_eEventLevel = level;
}

int XrgEventGetLevel(void)
{
    int usable;

    if (s_bSetEventLevel != 0) {
        return s_eEventLevel;
    }
    usable = xglFlagsGet1(0x65);
    return (xglFlagsGet1(0x12D) == 0) ? (usable != 0) : 2;
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_event", XrgEventIsUsableEnemy);

int XrgEventIsUsablePlayer(unsigned int character_index)
{
    int party_ids[16];
    int character_id;
    int party_count;
    int i;

    switch (character_index) {
    case 0:
        character_id = 17;
        break;
    case 4:
        character_id = 22;
        break;
    case 2:
        character_id = 18;
        break;
    case 5:
        character_id = 24;
        break;
    case 3:
        character_id = 26;
        break;
    case 1:
        character_id = 27;
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

int XrgEventIsUsableWeapon(struct XrgEventWeaponFilter *event)
{
    const struct XrgEventWeaponEntry *entry;
    const char *weapon_name;
    int weapon_id;
    unsigned int i;

    if (event == 0) {
        return 1;
    }

    weapon_name = &event->weapon_name;
    entry = s_aWepTbl_0;
    for (i = 0; i < 41; i++, entry++) {
        weapon_id = entry->weapon_id;
        if (strcasecmp(entry->weapon_name, weapon_name) == 0) {
            if (dataWpnBoxChk(weapon_id) > 0) {
                return 1;
            }
        }
    }
    return 0;
}
