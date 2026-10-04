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

extern const char D_00A59488[];
extern const char D_00A59480[];
extern const char D_00A59478[];
extern const char D_00A59470[];
extern const char D_00A59468[];
extern const char D_00A59460[];
extern const char D_00A59458[];
extern const char D_00A59450[];
extern const char D_00A59448[];
extern const char D_00A59440[];
extern const char D_00A59438[];
extern const char D_00A59430[];
extern const char D_00A59428[];
extern const char D_00A59420[];
extern const char D_00A59418[];
extern const char D_00A59410[];
extern const char D_00A59408[];
extern const char D_00A59400[];
extern const char D_00A593F8[];
extern const char D_00A593F0[];
extern const char D_00A593E0[];
extern const char D_00A593D0[];
extern const char D_00A593C8[];
extern const char D_00A593C0[];
extern const char D_00A593B8[];
extern const char D_00A593B0[];
extern const char D_00A593A8[];
extern const char D_00A593A0[];
extern const char D_00A59398[];
extern const char D_00A59390[];
extern const char D_00A59388[];
extern const char D_00A59378[];
extern const char D_00A59370[];
extern const char D_00A59360[];
extern const char D_00A59358[];
extern const char D_00A59350[];
extern const char D_00A59348[];
extern const char D_00A59340[];
extern const char D_00A59338[];
extern const char D_00A59330[];
extern const char D_00A59328[];

static int s_bSetEventLevel = 0;
static int s_eEventLevel = 0;

static struct XrgEventWeaponEntry s_aWepTbl_0[41] = {
    { D_00A59488, 72 }, { D_00A59480, 83 }, { D_00A59478, 75 },
    { D_00A59470, 82 }, { D_00A59468, 80 }, { D_00A59460, 90 },
    { D_00A59458, 93 }, { D_00A59450, 88 }, { D_00A59448, 102 },
    { D_00A59440, 103 }, { D_00A59438, 84 }, { D_00A59430, 73 },
    { D_00A59428, 77 }, { D_00A59420, 96 }, { D_00A59418, 106 },
    { D_00A59410, 109 }, { D_00A59408, 98 }, { D_00A59400, 76 },
    { D_00A593F8, 91 }, { D_00A593F0, 89 }, { D_00A593E0, 74 },
    { D_00A593D0, 78 }, { D_00A593C8, 101 }, { D_00A593C0, 86 },
    { D_00A593B8, 95 }, { D_00A593B0, 71 }, { D_00A593A8, 108 },
    { D_00A593A0, 81 }, { D_00A59398, 107 }, { D_00A59390, 87 },
    { D_00A59388, 85 }, { D_00A59378, 105 }, { D_00A59370, 79 },
    { D_00A59360, 0 },  { D_00A59358, 100 }, { D_00A59350, 94 },
    { D_00A59348, 92 }, { D_00A59340, 104 }, { D_00A59338, 99 },
    { D_00A59330, 110 }, { D_00A59328, 111 },
};
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



const char D_00A59328[8] = "DEF-VX";

const char D_00A59330[8] = "LW-VX4";

const char D_00A59338[8] = "LC-AG5";

const char D_00A59340[8] = "BBC-AG5";

const char D_00A59348[8] = "HGG-AG5";

const char D_00A59350[8] = "HMP-AG5";

const char D_00A59358[8] = "BMP-AG5";

const char D_00A59360[16] = "AIRC-AG2";

const char D_00A59370[8] = "HMR-AG5";

const char D_00A59378[16] = "AIRD-AG2";

const char D_00A59388[8] = "BA15VX";

const char D_00A59390[8] = "LM11VX";

const char D_00A59398[8] = "ECM2-VX";

const char D_00A593A0[8] = "SMG32VX";

const char D_00A593A8[8] = "SMP53AG";

const char D_00A593B0[8] = "AXE11AG";

const char D_00A593B8[8] = "BL24AG";

const char D_00A593C0[8] = "BSW13AG";

const char D_00A593C8[8] = "SHB67AG";

const char D_00A593D0[16] = "WCT02AG4";

const char D_00A593E0[16] = "DLC02AG4";

const char D_00A593F0[8] = "FLM64AG";

const char D_00A593F8[8] = "GLG76AG";

const char D_00A59400[8] = "HG75VX";

const char D_00A59408[8] = "BMP45VX";

const char D_00A59410[8] = "ER-VX";

const char D_00A59418[8] = "ECM1-VX";

const char D_00A59420[8] = "CB85VX";

const char D_00A59428[8] = "HMR55AG";

const char D_00A59430[8] = "SWD34VX";

const char D_00A59438[8] = "LG100VX";

const char D_00A59440[8] = "SHD12VX";

const char D_00A59448[8] = "SHD02AG";

const char D_00A59450[8] = "PB55AG";

const char D_00A59458[8] = "HMP33AG";

const char D_00A59460[8] = "GRD20AG";

const char D_00A59468[8] = "SMG99AG";

const char D_00A59470[8] = "LG10AG";

const char D_00A59478[8] = "HG45VX";

const char D_00A59480[8] = "LG24VX";

const char D_00A59488[8] = "SWD21AG";
