/*
 * OV12 original TU 66: 0x00a34c68..0x00a368e8 (1 functions)
 */
#include "common.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_actor_motid.h"
#include "ov12/rg_error.h"


/*
 * Returns the motion id for a motion table name (see rg_actor_motid.h).
 * Unknown names are reported with RgError and return RG_MOT_NONE.
 */
int RgActorMotIDGetFromName(const char *pszName) {
    if (pszName == 0) {
        assert_prog("pszName != NIL", "../rg_actor_motid.euc.c", 19);
    }

    if (strcasecmp(pszName, "STAND") == 0) {
        return RG_MOT_STAND;
    }
    if (strcasecmp(pszName, "ADV_F") == 0) {
        return RG_MOT_ADV_F;
    }
    if (strcasecmp(pszName, "ADV_FR") == 0) {
        return RG_MOT_ADV_FR;
    }
    if (strcasecmp(pszName, "ADV_R") == 0) {
        return RG_MOT_ADV_R;
    }
    if (strcasecmp(pszName, "ADV_BR") == 0) {
        return RG_MOT_ADV_BR;
    }
    if (strcasecmp(pszName, "ADV_B") == 0) {
        return RG_MOT_ADV_B;
    }
    if (strcasecmp(pszName, "ADV_BL") == 0) {
        return RG_MOT_ADV_BL;
    }
    if (strcasecmp(pszName, "ADV_L") == 0) {
        return RG_MOT_ADV_L;
    }
    if (strcasecmp(pszName, "ADV_FL") == 0) {
        return RG_MOT_ADV_FL;
    }
    if (strcasecmp(pszName, "DASH_F") == 0) {
        return RG_MOT_DASH_F;
    }
    if (strcasecmp(pszName, "DASH_FR") == 0) {
        return RG_MOT_DASH_FR;
    }
    if (strcasecmp(pszName, "DASH_R") == 0) {
        return RG_MOT_DASH_R;
    }
    if (strcasecmp(pszName, "DASH_BR") == 0) {
        return RG_MOT_DASH_BR;
    }
    if (strcasecmp(pszName, "DASH_B") == 0) {
        return RG_MOT_DASH_B;
    }
    if (strcasecmp(pszName, "DASH_BL") == 0) {
        return RG_MOT_DASH_BL;
    }
    if (strcasecmp(pszName, "DASH_L") == 0) {
        return RG_MOT_DASH_L;
    }
    if (strcasecmp(pszName, "DASH_FL") == 0) {
        return RG_MOT_DASH_FL;
    }
    if (strcasecmp(pszName, "ROLL_R") == 0) {
        return RG_MOT_ROLL_R;
    }
    if (strcasecmp(pszName, "ROLL_L") == 0) {
        return RG_MOT_ROLL_L;
    }
    if (strcasecmp(pszName, "JUMP") == 0) {
        return RG_MOT_JUMP;
    }
    if (strcasecmp(pszName, "JUMP_END") == 0) {
        return RG_MOT_JUMP_END;
    }
    if (strcasecmp(pszName, "BREAK") == 0) {
        return RG_MOT_BREAK;
    }
    if (strcasecmp(pszName, "SHOT_LEFT_HANDGUN") == 0) {
        return RG_MOT_SHOT_LEFT_HANDGUN;
    }
    if (strcasecmp(pszName, "SHOT_RIGHT_HANDGUN") == 0) {
        return RG_MOT_SHOT_RIGHT_HANDGUN;
    }
    if (strcasecmp(pszName, "DAMAGE_FL") == 0) {
        return RG_MOT_DAMAGE_FL;
    }
    if (strcasecmp(pszName, "DAMAGE_FR") == 0) {
        return RG_MOT_DAMAGE_FR;
    }
    if (strcasecmp(pszName, "DAMAGE_BR") == 0) {
        return RG_MOT_DAMAGE_BR;
    }
    if (strcasecmp(pszName, "DAMAGE_BL") == 0) {
        return RG_MOT_DAMAGE_BL;
    }
    if (strcasecmp(pszName, "ATTACK_LEFT") == 0) {
        return RG_MOT_ATTACK_LEFT;
    }
    if (strcasecmp(pszName, "ATTACK_RIGHT") == 0) {
        return RG_MOT_ATTACK_RIGHT;
    }
    if (strcasecmp(pszName, "DROP_LEFT") == 0) {
        return RG_MOT_DROP_LEFT;
    }
    if (strcasecmp(pszName, "DROP_RIGHT") == 0) {
        return RG_MOT_DROP_RIGHT;
    }
    if (strcasecmp(pszName, "BRADE_LEFT") == 0) {
        return RG_MOT_BRADE_LEFT;
    }
    if (strcasecmp(pszName, "BRADE_RIGHT") == 0) {
        return RG_MOT_BRADE_RIGHT;
    }
    if (strcasecmp(pszName, "POD_LEFT") == 0) {
        return RG_MOT_POD_LEFT;
    }
    if (strcasecmp(pszName, "POD_RIGHT") == 0) {
        return RG_MOT_POD_RIGHT;
    }
    if (strcasecmp(pszName, "GUARD_START") == 0) {
        return RG_MOT_GUARD_START;
    }
    if (strcasecmp(pszName, "GUARD_HIT") == 0) {
        return RG_MOT_GUARD_HIT;
    }
    if (strcasecmp(pszName, "GUARD_END") == 0) {
        return RG_MOT_GUARD_END;
    }
    if (strcasecmp(pszName, "DRILL_LEFT") == 0) {
        return RG_MOT_DRILL_LEFT;
    }
    if (strcasecmp(pszName, "DRILL_RIGHT") == 0) {
        return RG_MOT_DRILL_RIGHT;
    }
    if (strcasecmp(pszName, "HISSATU") == 0) {
        return RG_MOT_HISSATU;
    }
    if (strcasecmp(pszName, "HISSATU_VX06_HIT") == 0) {
        return RG_MOT_HISSATU_VX06_HIT;
    }
    /* MOT_<n> names map to the plain ids n - 1. */
    if (strcasecmp(pszName, "MOT_1") == 0) {
        return 0;
    }
    if (strcasecmp(pszName, "MOT_2") == 0) {
        return 1;
    }
    if (strcasecmp(pszName, "MOT_3") == 0) {
        return 2;
    }
    if (strcasecmp(pszName, "MOT_4") == 0) {
        return 3;
    }
    if (strcasecmp(pszName, "MOT_5") == 0) {
        return 4;
    }
    if (strcasecmp(pszName, "MOT_6") == 0) {
        return 5;
    }
    if (strcasecmp(pszName, "MOT_7") == 0) {
        return 6;
    }
    if (strcasecmp(pszName, "MOT_8") == 0) {
        return 7;
    }
    if (strcasecmp(pszName, "MOT_9") == 0) {
        return 8;
    }
    if (strcasecmp(pszName, "MOT_10") == 0) {
        return 9;
    }
    if (strcasecmp(pszName, "MOT_11") == 0) {
        return 10;
    }
    if (strcasecmp(pszName, "MOT_12") == 0) {
        return 11;
    }
    if (strcasecmp(pszName, "MOT_13") == 0) {
        return 12;
    }
    if (strcasecmp(pszName, "MOT_14") == 0) {
        return 13;
    }
    if (strcasecmp(pszName, "MOT_15") == 0) {
        return 14;
    }
    if (strcasecmp(pszName, "MOT_16") == 0) {
        return 15;
    }
    if (strcasecmp(pszName, "MOT_17") == 0) {
        return 16;
    }
    if (strcasecmp(pszName, "MOT_18") == 0) {
        return 17;
    }
    if (strcasecmp(pszName, "MOT_19") == 0) {
        return 18;
    }
    if (strcasecmp(pszName, "MOT_20") == 0) {
        return 19;
    }
    if (strcasecmp(pszName, "MOT_21") == 0) {
        return 20;
    }
    if (strcasecmp(pszName, "MOT_22") == 0) {
        return 21;
    }
    if (strcasecmp(pszName, "MOT_23") == 0) {
        return 22;
    }
    if (strcasecmp(pszName, "MOT_24") == 0) {
        return 23;
    }
    if (strcasecmp(pszName, "MOT_25") == 0) {
        return 24;
    }
    if (strcasecmp(pszName, "MOT_26") == 0) {
        return 25;
    }
    if (strcasecmp(pszName, "MOT_27") == 0) {
        return 26;
    }
    if (strcasecmp(pszName, "MOT_28") == 0) {
        return 27;
    }
    if (strcasecmp(pszName, "MOT_29") == 0) {
        return 28;
    }
    if (strcasecmp(pszName, "MOT_30") == 0) {
        return 29;
    }
    if (strcasecmp(pszName, "MOT_31") == 0) {
        return 30;
    }
    if (strcasecmp(pszName, "MOT_32") == 0) {
        return 31;
    }
    if (strcasecmp(pszName, "MOT_33") == 0) {
        return 32;
    }
    if (strcasecmp(pszName, "MOT_34") == 0) {
        return 33;
    }
    if (strcasecmp(pszName, "MOT_35") == 0) {
        return 34;
    }
    if (strcasecmp(pszName, "MOT_36") == 0) {
        return 35;
    }
    if (strcasecmp(pszName, "MOT_37") == 0) {
        return 36;
    }
    if (strcasecmp(pszName, "MOT_38") == 0) {
        return 37;
    }
    if (strcasecmp(pszName, "MOT_39") == 0) {
        return 38;
    }
    if (strcasecmp(pszName, "MOT_40") == 0) {
        return 39;
    }
    if (strcasecmp(pszName, "MOT_41") == 0) {
        return 40;
    }
    if (strcasecmp(pszName, "MOT_42") == 0) {
        return 41;
    }
    if (strcasecmp(pszName, "MOT_43") == 0) {
        return 42;
    }
    if (strcasecmp(pszName, "MOT_44") == 0) {
        return 43;
    }
    if (strcasecmp(pszName, "MOT_45") == 0) {
        return 44;
    }
    if (strcasecmp(pszName, "MOT_46") == 0) {
        return 45;
    }
    if (strcasecmp(pszName, "MOT_47") == 0) {
        return 46;
    }
    if (strcasecmp(pszName, "MOT_48") == 0) {
        return 47;
    }
    if (strcasecmp(pszName, "MOT_49") == 0) {
        return 48;
    }
    if (strcasecmp(pszName, "MOT_50") == 0) {
        return 49;
    }
    if (strcasecmp(pszName, "MOT_51") == 0) {
        return 50;
    }
    if (strcasecmp(pszName, "MOT_52") == 0) {
        return 51;
    }
    if (strcasecmp(pszName, "MOT_53") == 0) {
        return 52;
    }
    if (strcasecmp(pszName, "MOT_54") == 0) {
        return 53;
    }
    if (strcasecmp(pszName, "MOT_55") == 0) {
        return 54;
    }
    if (strcasecmp(pszName, "MOT_56") == 0) {
        return 55;
    }
    if (strcasecmp(pszName, "MOT_57") == 0) {
        return 56;
    }
    if (strcasecmp(pszName, "MOT_58") == 0) {
        return 57;
    }
    if (strcasecmp(pszName, "MOT_59") == 0) {
        return 58;
    }
    if (strcasecmp(pszName, "MOT_60") == 0) {
        return 59;
    }
    if (strcasecmp(pszName, "MOT_61") == 0) {
        return 60;
    }
    if (strcasecmp(pszName, "MOT_62") == 0) {
        return 61;
    }
    if (strcasecmp(pszName, "MOT_63") == 0) {
        return 62;
    }
    if (strcasecmp(pszName, "MOT_64") == 0) {
        return 63;
    }
    if (strcasecmp(pszName, "MOT_65") == 0) {
        return 64;
    }
    if (strcasecmp(pszName, "MOT_66") == 0) {
        return 65;
    }
    if (strcasecmp(pszName, "MOT_67") == 0) {
        return 66;
    }
    if (strcasecmp(pszName, "MOT_68") == 0) {
        return 67;
    }
    if (strcasecmp(pszName, "MOT_69") == 0) {
        return 68;
    }
    if (strcasecmp(pszName, "MOT_70") == 0) {
        return 69;
    }
    if (strcasecmp(pszName, "MOT_71") == 0) {
        return 70;
    }
    if (strcasecmp(pszName, "MOT_72") == 0) {
        return 71;
    }
    if (strcasecmp(pszName, "MOT_73") == 0) {
        return 72;
    }
    if (strcasecmp(pszName, "MOT_74") == 0) {
        return 73;
    }
    if (strcasecmp(pszName, "MOT_75") == 0) {
        return 74;
    }
    if (strcasecmp(pszName, "MOT_76") == 0) {
        return 75;
    }
    if (strcasecmp(pszName, "MOT_77") == 0) {
        return 76;
    }
    if (strcasecmp(pszName, "MOT_78") == 0) {
        return 77;
    }
    if (strcasecmp(pszName, "MOT_79") == 0) {
        return 78;
    }
    if (strcasecmp(pszName, "MOT_80") == 0) {
        return 79;
    }
    if (strcasecmp(pszName, "MOT_81") == 0) {
        return 80;
    }
    if (strcasecmp(pszName, "MOT_82") == 0) {
        return 81;
    }
    if (strcasecmp(pszName, "MOT_83") == 0) {
        return 82;
    }
    if (strcasecmp(pszName, "MOT_84") == 0) {
        return 83;
    }
    if (strcasecmp(pszName, "MOT_85") == 0) {
        return 84;
    }
    if (strcasecmp(pszName, "MOT_86") == 0) {
        return 85;
    }
    if (strcasecmp(pszName, "MOT_87") == 0) {
        return 86;
    }
    if (strcasecmp(pszName, "MOT_88") == 0) {
        return 87;
    }
    if (strcasecmp(pszName, "MOT_89") == 0) {
        return 88;
    }
    if (strcasecmp(pszName, "MOT_90") == 0) {
        return 89;
    }
    if (strcasecmp(pszName, "MOT_91") == 0) {
        return 90;
    }
    if (strcasecmp(pszName, "MOT_92") == 0) {
        return 91;
    }
    if (strcasecmp(pszName, "MOT_93") == 0) {
        return 92;
    }
    if (strcasecmp(pszName, "MOT_94") == 0) {
        return 93;
    }
    if (strcasecmp(pszName, "MOT_95") == 0) {
        return 94;
    }
    if (strcasecmp(pszName, "MOT_96") == 0) {
        return 95;
    }
    if (strcasecmp(pszName, "MOT_97") == 0) {
        return 96;
    }
    if (strcasecmp(pszName, "MOT_98") == 0) {
        return 97;
    }
    if (strcasecmp(pszName, "MOT_99") == 0) {
        return 98;
    }
    /* WMOT_<n> names map to weapon motion ids counted from RG_MOT_WEAPON_BASE. */
    if (strcasecmp(pszName, "WMOT_1") == 0) {
        return RG_MOT_WEAPON_BASE + 0;
    }
    if (strcasecmp(pszName, "WMOT_2") == 0) {
        return RG_MOT_WEAPON_BASE + 1;
    }
    if (strcasecmp(pszName, "WMOT_3") == 0) {
        return RG_MOT_WEAPON_BASE + 2;
    }
    if (strcasecmp(pszName, "WMOT_4") == 0) {
        return RG_MOT_WEAPON_BASE + 3;
    }
    if (strcasecmp(pszName, "WMOT_5") == 0) {
        return RG_MOT_WEAPON_BASE + 4;
    }
    if (strcasecmp(pszName, "WMOT_6") == 0) {
        return RG_MOT_WEAPON_BASE + 5;
    }
    if (strcasecmp(pszName, "WMOT_7") == 0) {
        return RG_MOT_WEAPON_BASE + 6;
    }
    if (strcasecmp(pszName, "WMOT_8") == 0) {
        return RG_MOT_WEAPON_BASE + 7;
    }
    if (strcasecmp(pszName, "WMOT_9") == 0) {
        return RG_MOT_WEAPON_BASE + 8;
    }
    if (strcasecmp(pszName, "WMOT_10") == 0) {
        return RG_MOT_WEAPON_BASE + 9;
    }
    if (strcasecmp(pszName, "WMOT_11") == 0) {
        return RG_MOT_WEAPON_BASE + 10;
    }
    if (strcasecmp(pszName, "WMOT_12") == 0) {
        return RG_MOT_WEAPON_BASE + 11;
    }
    if (strcasecmp(pszName, "WMOT_13") == 0) {
        return RG_MOT_WEAPON_BASE + 12;
    }
    if (strcasecmp(pszName, "WMOT_14") == 0) {
        return RG_MOT_WEAPON_BASE + 13;
    }
    if (strcasecmp(pszName, "WMOT_15") == 0) {
        return RG_MOT_WEAPON_BASE + 14;
    }
    if (strcasecmp(pszName, "WMOT_16") == 0) {
        return RG_MOT_WEAPON_BASE + 15;
    }
    if (strcasecmp(pszName, "WMOT_17") == 0) {
        return RG_MOT_WEAPON_BASE + 16;
    }
    if (strcasecmp(pszName, "WMOT_18") == 0) {
        return RG_MOT_WEAPON_BASE + 17;
    }
    if (strcasecmp(pszName, "WMOT_19") == 0) {
        return RG_MOT_WEAPON_BASE + 18;
    }
    if (strcasecmp(pszName, "WMOT_20") == 0) {
        return RG_MOT_WEAPON_BASE + 19;
    }
    if (strcasecmp(pszName, "WMOT_21") == 0) {
        return RG_MOT_WEAPON_BASE + 20;
    }
    if (strcasecmp(pszName, "WMOT_22") == 0) {
        return RG_MOT_WEAPON_BASE + 21;
    }
    if (strcasecmp(pszName, "WMOT_23") == 0) {
        return RG_MOT_WEAPON_BASE + 22;
    }
    if (strcasecmp(pszName, "WMOT_24") == 0) {
        return RG_MOT_WEAPON_BASE + 23;
    }
    if (strcasecmp(pszName, "WMOT_25") == 0) {
        return RG_MOT_WEAPON_BASE + 24;
    }
    if (strcasecmp(pszName, "WMOT_26") == 0) {
        return RG_MOT_WEAPON_BASE + 25;
    }
    if (strcasecmp(pszName, "WMOT_27") == 0) {
        return RG_MOT_WEAPON_BASE + 26;
    }
    if (strcasecmp(pszName, "WMOT_28") == 0) {
        return RG_MOT_WEAPON_BASE + 27;
    }
    if (strcasecmp(pszName, "WMOT_29") == 0) {
        return RG_MOT_WEAPON_BASE + 28;
    }
    if (strcasecmp(pszName, "WMOT_30") == 0) {
        return RG_MOT_WEAPON_BASE + 29;
    }
    if (strcasecmp(pszName, "WMOT_31") == 0) {
        return RG_MOT_WEAPON_BASE + 30;
    }
    if (strcasecmp(pszName, "WMOT_32") == 0) {
        return RG_MOT_WEAPON_BASE + 31;
    }
    if (strcasecmp(pszName, "WMOT_33") == 0) {
        return RG_MOT_WEAPON_BASE + 32;
    }
    if (strcasecmp(pszName, "WMOT_34") == 0) {
        return RG_MOT_WEAPON_BASE + 33;
    }
    if (strcasecmp(pszName, "WMOT_35") == 0) {
        return RG_MOT_WEAPON_BASE + 34;
    }
    if (strcasecmp(pszName, "WMOT_36") == 0) {
        return RG_MOT_WEAPON_BASE + 35;
    }
    if (strcasecmp(pszName, "WMOT_37") == 0) {
        return RG_MOT_WEAPON_BASE + 36;
    }
    if (strcasecmp(pszName, "WMOT_38") == 0) {
        return RG_MOT_WEAPON_BASE + 37;
    }
    if (strcasecmp(pszName, "WMOT_39") == 0) {
        return RG_MOT_WEAPON_BASE + 38;
    }
    if (strcasecmp(pszName, "WMOT_40") == 0) {
        return RG_MOT_WEAPON_BASE + 39;
    }
    if (strcasecmp(pszName, "WMOT_41") == 0) {
        return RG_MOT_WEAPON_BASE + 40;
    }
    if (strcasecmp(pszName, "WMOT_42") == 0) {
        return RG_MOT_WEAPON_BASE + 41;
    }
    if (strcasecmp(pszName, "WMOT_43") == 0) {
        return RG_MOT_WEAPON_BASE + 42;
    }
    if (strcasecmp(pszName, "WMOT_44") == 0) {
        return RG_MOT_WEAPON_BASE + 43;
    }
    if (strcasecmp(pszName, "WMOT_45") == 0) {
        return RG_MOT_WEAPON_BASE + 44;
    }
    if (strcasecmp(pszName, "WMOT_46") == 0) {
        return RG_MOT_WEAPON_BASE + 45;
    }
    if (strcasecmp(pszName, "WMOT_47") == 0) {
        return RG_MOT_WEAPON_BASE + 46;
    }
    if (strcasecmp(pszName, "WMOT_48") == 0) {
        return RG_MOT_WEAPON_BASE + 47;
    }
    if (strcasecmp(pszName, "WMOT_49") == 0) {
        return RG_MOT_WEAPON_BASE + 48;
    }
    if (strcasecmp(pszName, "WMOT_50") == 0) {
        return RG_MOT_WEAPON_BASE + 49;
    }
    if (strcasecmp(pszName, "WMOT_51") == 0) {
        return RG_MOT_WEAPON_BASE + 50;
    }
    if (strcasecmp(pszName, "WMOT_52") == 0) {
        return RG_MOT_WEAPON_BASE + 51;
    }
    if (strcasecmp(pszName, "WMOT_53") == 0) {
        return RG_MOT_WEAPON_BASE + 52;
    }
    if (strcasecmp(pszName, "WMOT_54") == 0) {
        return RG_MOT_WEAPON_BASE + 53;
    }
    if (strcasecmp(pszName, "WMOT_55") == 0) {
        return RG_MOT_WEAPON_BASE + 54;
    }
    if (strcasecmp(pszName, "WMOT_56") == 0) {
        return RG_MOT_WEAPON_BASE + 55;
    }
    if (strcasecmp(pszName, "WMOT_57") == 0) {
        return RG_MOT_WEAPON_BASE + 56;
    }
    if (strcasecmp(pszName, "WMOT_58") == 0) {
        return RG_MOT_WEAPON_BASE + 57;
    }
    if (strcasecmp(pszName, "WMOT_59") == 0) {
        return RG_MOT_WEAPON_BASE + 58;
    }
    if (strcasecmp(pszName, "WMOT_60") == 0) {
        return RG_MOT_WEAPON_BASE + 59;
    }
    if (strcasecmp(pszName, "WMOT_61") == 0) {
        return RG_MOT_WEAPON_BASE + 60;
    }
    if (strcasecmp(pszName, "WMOT_62") == 0) {
        return RG_MOT_WEAPON_BASE + 61;
    }
    if (strcasecmp(pszName, "WMOT_63") == 0) {
        return RG_MOT_WEAPON_BASE + 62;
    }
    if (strcasecmp(pszName, "WMOT_64") == 0) {
        return RG_MOT_WEAPON_BASE + 63;
    }
    if (strcasecmp(pszName, "WMOT_65") == 0) {
        return RG_MOT_WEAPON_BASE + 64;
    }
    if (strcasecmp(pszName, "WMOT_66") == 0) {
        return RG_MOT_WEAPON_BASE + 65;
    }
    if (strcasecmp(pszName, "WMOT_67") == 0) {
        return RG_MOT_WEAPON_BASE + 66;
    }
    if (strcasecmp(pszName, "WMOT_68") == 0) {
        return RG_MOT_WEAPON_BASE + 67;
    }
    if (strcasecmp(pszName, "WMOT_69") == 0) {
        return RG_MOT_WEAPON_BASE + 68;
    }
    if (strcasecmp(pszName, "WMOT_70") == 0) {
        return RG_MOT_WEAPON_BASE + 69;
    }
    if (strcasecmp(pszName, "WMOT_71") == 0) {
        return RG_MOT_WEAPON_BASE + 70;
    }
    if (strcasecmp(pszName, "WMOT_72") == 0) {
        return RG_MOT_WEAPON_BASE + 71;
    }
    if (strcasecmp(pszName, "WMOT_73") == 0) {
        return RG_MOT_WEAPON_BASE + 72;
    }
    if (strcasecmp(pszName, "WMOT_74") == 0) {
        return RG_MOT_WEAPON_BASE + 73;
    }
    if (strcasecmp(pszName, "WMOT_75") == 0) {
        return RG_MOT_WEAPON_BASE + 74;
    }
    if (strcasecmp(pszName, "WMOT_76") == 0) {
        return RG_MOT_WEAPON_BASE + 75;
    }
    if (strcasecmp(pszName, "WMOT_77") == 0) {
        return RG_MOT_WEAPON_BASE + 76;
    }
    if (strcasecmp(pszName, "WMOT_78") == 0) {
        return RG_MOT_WEAPON_BASE + 77;
    }
    if (strcasecmp(pszName, "WMOT_79") == 0) {
        return RG_MOT_WEAPON_BASE + 78;
    }
    if (strcasecmp(pszName, "WMOT_80") == 0) {
        return RG_MOT_WEAPON_BASE + 79;
    }
    if (strcasecmp(pszName, "WMOT_81") == 0) {
        return RG_MOT_WEAPON_BASE + 80;
    }
    if (strcasecmp(pszName, "WMOT_82") == 0) {
        return RG_MOT_WEAPON_BASE + 81;
    }
    if (strcasecmp(pszName, "WMOT_83") == 0) {
        return RG_MOT_WEAPON_BASE + 82;
    }
    if (strcasecmp(pszName, "WMOT_84") == 0) {
        return RG_MOT_WEAPON_BASE + 83;
    }
    if (strcasecmp(pszName, "WMOT_85") == 0) {
        return RG_MOT_WEAPON_BASE + 84;
    }
    if (strcasecmp(pszName, "WMOT_86") == 0) {
        return RG_MOT_WEAPON_BASE + 85;
    }
    if (strcasecmp(pszName, "WMOT_87") == 0) {
        return RG_MOT_WEAPON_BASE + 86;
    }
    if (strcasecmp(pszName, "WMOT_88") == 0) {
        return RG_MOT_WEAPON_BASE + 87;
    }
    if (strcasecmp(pszName, "WMOT_89") == 0) {
        return RG_MOT_WEAPON_BASE + 88;
    }
    if (strcasecmp(pszName, "WMOT_90") == 0) {
        return RG_MOT_WEAPON_BASE + 89;
    }
    if (strcasecmp(pszName, "WMOT_91") == 0) {
        return RG_MOT_WEAPON_BASE + 90;
    }
    if (strcasecmp(pszName, "WMOT_92") == 0) {
        return RG_MOT_WEAPON_BASE + 91;
    }
    if (strcasecmp(pszName, "WMOT_93") == 0) {
        return RG_MOT_WEAPON_BASE + 92;
    }
    if (strcasecmp(pszName, "WMOT_94") == 0) {
        return RG_MOT_WEAPON_BASE + 93;
    }
    if (strcasecmp(pszName, "WMOT_95") == 0) {
        return RG_MOT_WEAPON_BASE + 94;
    }
    if (strcasecmp(pszName, "WMOT_96") == 0) {
        return RG_MOT_WEAPON_BASE + 95;
    }
    if (strcasecmp(pszName, "WMOT_97") == 0) {
        return RG_MOT_WEAPON_BASE + 96;
    }
    if (strcasecmp(pszName, "WMOT_98") == 0) {
        return RG_MOT_WEAPON_BASE + 97;
    }
    if (strcasecmp(pszName, "WMOT_99") == 0) {
        return RG_MOT_WEAPON_BASE + 98;
    }
    /* Every name that matched nothing is reported, except "nul". */
    if (strcasecmp(pszName, "nul") != 0) {
        RgError("unknown motion name '%s'", "../rg_actor_motid.euc.c", 26, pszName);
    }

    return RG_MOT_NONE;
}
