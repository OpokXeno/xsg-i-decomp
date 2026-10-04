/*
 * OV12 original TU 66: 0x00a34c68..0x00a368e8 (1 functions)
 */
#include "common.h"

extern void assert_prog(const char *expression, const char *file, int line);
extern int strcasecmp(const char *left, const char *right);
extern void RgError(const char *message, const char *source_file, int line, ...);

#define D_00A55C48 "pszName != NIL"
#define D_00A55C58 "../rg_actor_motid.euc.c"
#define D_00A55C70 "STAND"
#define D_00A55C78 "ADV_F"
#define D_00A55C80 "ADV_FR"
#define D_00A55C88 "ADV_R"
#define D_00A55C90 "ADV_BR"
#define D_00A55C98 "ADV_B"
#define D_00A55CA0 "ADV_BL"
#define D_00A55CA8 "ADV_L"
#define D_00A55CB0 "ADV_FL"
#define D_00A55CB8 "DASH_F"
#define D_00A55CC0 "DASH_FR"
#define D_00A55CC8 "DASH_R"
#define D_00A55CD0 "DASH_BR"
#define D_00A55CD8 "DASH_B"
#define D_00A55CE0 "DASH_BL"
#define D_00A55CE8 "DASH_L"
#define D_00A55CF0 "DASH_FL"
#define D_00A55CF8 "ROLL_R"
#define D_00A55D00 "ROLL_L"
#define D_00A55D08 "JUMP"
#define D_00A55D10 "JUMP_END"
#define D_00A55D20 "BREAK"
#define D_00A55D28 "SHOT_LEFT_HANDGUN"
#define D_00A55D40 "SHOT_RIGHT_HANDGUN"
#define D_00A55D58 "DAMAGE_FL"
#define D_00A55D68 "DAMAGE_FR"
#define D_00A55D78 "DAMAGE_BR"
#define D_00A55D88 "DAMAGE_BL"
#define D_00A55D98 "ATTACK_LEFT"
#define D_00A55DA8 "ATTACK_RIGHT"
#define D_00A55DB8 "DROP_LEFT"
#define D_00A55DC8 "DROP_RIGHT"
#define D_00A55DD8 "BRADE_LEFT"
#define D_00A55DE8 "BRADE_RIGHT"
#define D_00A55DF8 "POD_LEFT"
#define D_00A55E08 "POD_RIGHT"
#define D_00A55E18 "GUARD_START"
#define D_00A55E28 "GUARD_HIT"
#define D_00A55E38 "GUARD_END"
#define D_00A55E48 "DRILL_LEFT"
#define D_00A55E58 "DRILL_RIGHT"
#define D_00A55E68 "HISSATU"
#define D_00A55E70 "HISSATU_VX06_HIT"
#define D_00A55E88 "MOT_1"
#define D_00A55E90 "MOT_2"
#define D_00A55E98 "MOT_3"
#define D_00A55EA0 "MOT_4"
#define D_00A55EA8 "MOT_5"
#define D_00A55EB0 "MOT_6"
#define D_00A55EB8 "MOT_7"
#define D_00A55EC0 "MOT_8"
#define D_00A55EC8 "MOT_9"
#define D_00A55ED0 "MOT_10"
#define D_00A55ED8 "MOT_11"
#define D_00A55EE0 "MOT_12"
#define D_00A55EE8 "MOT_13"
#define D_00A55EF0 "MOT_14"
#define D_00A55EF8 "MOT_15"
#define D_00A55F00 "MOT_16"
#define D_00A55F08 "MOT_17"
#define D_00A55F10 "MOT_18"
#define D_00A55F18 "MOT_19"
#define D_00A55F20 "MOT_20"
#define D_00A55F28 "MOT_21"
#define D_00A55F30 "MOT_22"
#define D_00A55F38 "MOT_23"
#define D_00A55F40 "MOT_24"
#define D_00A55F48 "MOT_25"
#define D_00A55F50 "MOT_26"
#define D_00A55F58 "MOT_27"
#define D_00A55F60 "MOT_28"
#define D_00A55F68 "MOT_29"
#define D_00A55F70 "MOT_30"
#define D_00A55F78 "MOT_31"
#define D_00A55F80 "MOT_32"
#define D_00A55F88 "MOT_33"
#define D_00A55F90 "MOT_34"
#define D_00A55F98 "MOT_35"
#define D_00A55FA0 "MOT_36"
#define D_00A55FA8 "MOT_37"
#define D_00A55FB0 "MOT_38"
#define D_00A55FB8 "MOT_39"
#define D_00A55FC0 "MOT_40"
#define D_00A55FC8 "MOT_41"
#define D_00A55FD0 "MOT_42"
#define D_00A55FD8 "MOT_43"
#define D_00A55FE0 "MOT_44"
#define D_00A55FE8 "MOT_45"
#define D_00A55FF0 "MOT_46"
#define D_00A55FF8 "MOT_47"
#define D_00A56000 "MOT_48"
#define D_00A56008 "MOT_49"
#define D_00A56010 "MOT_50"
#define D_00A56018 "MOT_51"
#define D_00A56020 "MOT_52"
#define D_00A56028 "MOT_53"
#define D_00A56030 "MOT_54"
#define D_00A56038 "MOT_55"
#define D_00A56040 "MOT_56"
#define D_00A56048 "MOT_57"
#define D_00A56050 "MOT_58"
#define D_00A56058 "MOT_59"
#define D_00A56060 "MOT_60"
#define D_00A56068 "MOT_61"
#define D_00A56070 "MOT_62"
#define D_00A56078 "MOT_63"
#define D_00A56080 "MOT_64"
#define D_00A56088 "MOT_65"
#define D_00A56090 "MOT_66"
#define D_00A56098 "MOT_67"
#define D_00A560A0 "MOT_68"
#define D_00A560A8 "MOT_69"
#define D_00A560B0 "MOT_70"
#define D_00A560B8 "MOT_71"
#define D_00A560C0 "MOT_72"
#define D_00A560C8 "MOT_73"
#define D_00A560D0 "MOT_74"
#define D_00A560D8 "MOT_75"
#define D_00A560E0 "MOT_76"
#define D_00A560E8 "MOT_77"
#define D_00A560F0 "MOT_78"
#define D_00A560F8 "MOT_79"
#define D_00A56100 "MOT_80"
#define D_00A56108 "MOT_81"
#define D_00A56110 "MOT_82"
#define D_00A56118 "MOT_83"
#define D_00A56120 "MOT_84"
#define D_00A56128 "MOT_85"
#define D_00A56130 "MOT_86"
#define D_00A56138 "MOT_87"
#define D_00A56140 "MOT_88"
#define D_00A56148 "MOT_89"
#define D_00A56150 "MOT_90"
#define D_00A56158 "MOT_91"
#define D_00A56160 "MOT_92"
#define D_00A56168 "MOT_93"
#define D_00A56170 "MOT_94"
#define D_00A56178 "MOT_95"
#define D_00A56180 "MOT_96"
#define D_00A56188 "MOT_97"
#define D_00A56190 "MOT_98"
#define D_00A56198 "MOT_99"
#define D_00A561A0 "WMOT_1"
#define D_00A561A8 "WMOT_2"
#define D_00A561B0 "WMOT_3"
#define D_00A561B8 "WMOT_4"
#define D_00A561C0 "WMOT_5"
#define D_00A561C8 "WMOT_6"
#define D_00A561D0 "WMOT_7"
#define D_00A561D8 "WMOT_8"
#define D_00A561E0 "WMOT_9"
#define D_00A561E8 "WMOT_10"
#define D_00A561F0 "WMOT_11"
#define D_00A561F8 "WMOT_12"
#define D_00A56200 "WMOT_13"
#define D_00A56208 "WMOT_14"
#define D_00A56210 "WMOT_15"
#define D_00A56218 "WMOT_16"
#define D_00A56220 "WMOT_17"
#define D_00A56228 "WMOT_18"
#define D_00A56230 "WMOT_19"
#define D_00A56238 "WMOT_20"
#define D_00A56240 "WMOT_21"
#define D_00A56248 "WMOT_22"
#define D_00A56250 "WMOT_23"
#define D_00A56258 "WMOT_24"
#define D_00A56260 "WMOT_25"
#define D_00A56268 "WMOT_26"
#define D_00A56270 "WMOT_27"
#define D_00A56278 "WMOT_28"
#define D_00A56280 "WMOT_29"
#define D_00A56288 "WMOT_30"
#define D_00A56290 "WMOT_31"
#define D_00A56298 "WMOT_32"
#define D_00A562A0 "WMOT_33"
#define D_00A562A8 "WMOT_34"
#define D_00A562B0 "WMOT_35"
#define D_00A562B8 "WMOT_36"
#define D_00A562C0 "WMOT_37"
#define D_00A562C8 "WMOT_38"
#define D_00A562D0 "WMOT_39"
#define D_00A562D8 "WMOT_40"
#define D_00A562E0 "WMOT_41"
#define D_00A562E8 "WMOT_42"
#define D_00A562F0 "WMOT_43"
#define D_00A562F8 "WMOT_44"
#define D_00A56300 "WMOT_45"
#define D_00A56308 "WMOT_46"
#define D_00A56310 "WMOT_47"
#define D_00A56318 "WMOT_48"
#define D_00A56320 "WMOT_49"
#define D_00A56328 "WMOT_50"
#define D_00A56330 "WMOT_51"
#define D_00A56338 "WMOT_52"
#define D_00A56340 "WMOT_53"
#define D_00A56348 "WMOT_54"
#define D_00A56350 "WMOT_55"
#define D_00A56358 "WMOT_56"
#define D_00A56360 "WMOT_57"
#define D_00A56368 "WMOT_58"
#define D_00A56370 "WMOT_59"
#define D_00A56378 "WMOT_60"
#define D_00A56380 "WMOT_61"
#define D_00A56388 "WMOT_62"
#define D_00A56390 "WMOT_63"
#define D_00A56398 "WMOT_64"
#define D_00A563A0 "WMOT_65"
#define D_00A563A8 "WMOT_66"
#define D_00A563B0 "WMOT_67"
#define D_00A563B8 "WMOT_68"
#define D_00A563C0 "WMOT_69"
#define D_00A563C8 "WMOT_70"
#define D_00A563D0 "WMOT_71"
#define D_00A563D8 "WMOT_72"
#define D_00A563E0 "WMOT_73"
#define D_00A563E8 "WMOT_74"
#define D_00A563F0 "WMOT_75"
#define D_00A563F8 "WMOT_76"
#define D_00A56400 "WMOT_77"
#define D_00A56408 "WMOT_78"
#define D_00A56410 "WMOT_79"
#define D_00A56418 "WMOT_80"
#define D_00A56420 "WMOT_81"
#define D_00A56428 "WMOT_82"
#define D_00A56430 "WMOT_83"
#define D_00A56438 "WMOT_84"
#define D_00A56440 "WMOT_85"
#define D_00A56448 "WMOT_86"
#define D_00A56450 "WMOT_87"
#define D_00A56458 "WMOT_88"
#define D_00A56460 "WMOT_89"
#define D_00A56468 "WMOT_90"
#define D_00A56470 "WMOT_91"
#define D_00A56478 "WMOT_92"
#define D_00A56480 "WMOT_93"
#define D_00A56488 "WMOT_94"
#define D_00A56490 "WMOT_95"
#define D_00A56498 "WMOT_96"
#define D_00A564A0 "WMOT_97"
#define D_00A564A8 "WMOT_98"
#define D_00A564B0 "WMOT_99"
#define D_00A564B8 "nul"
#define D_00A564C0 "unknown motion name '%s'"

#define RETURN_MOTION_IF_MATCH(name, id) \
    if (strcasecmp(pszName, name) == 0) return id

int RgActorMotIDGetFromName(const char *pszName) {
    if (pszName == 0) {
        assert_prog(D_00A55C48, D_00A55C58, 19);
    }

    if (strcasecmp(pszName, D_00A55C70) == 0) {
        return 0;
    }
    if (strcasecmp(pszName, D_00A55C78) == 0) {
        return 1;
    }
    if (strcasecmp(pszName, D_00A55C80) == 0) {
        return 2;
    }
    if (strcasecmp(pszName, D_00A55C88) == 0) {
        return 3;
    }
    if (strcasecmp(pszName, D_00A55C90) == 0) {
        return 4;
    }
    if (strcasecmp(pszName, D_00A55C98) == 0) {
        return 5;
    }
    if (strcasecmp(pszName, D_00A55CA0) == 0) {
        return 6;
    }
    if (strcasecmp(pszName, D_00A55CA8) == 0) {
        return 7;
    }
    if (strcasecmp(pszName, D_00A55CB0) == 0) {
        return 8;
    }

    if (strcasecmp(pszName, D_00A55CB8) == 0) {
        return 11;
    }
    if (strcasecmp(pszName, D_00A55CC0) == 0) {
        return 12;
    }
    if (strcasecmp(pszName, D_00A55CC8) == 0) {
        return 13;
    }
    if (strcasecmp(pszName, D_00A55CD0) == 0) {
        return 14;
    }
    if (strcasecmp(pszName, D_00A55CD8) == 0) {
        return 15;
    }
    if (strcasecmp(pszName, D_00A55CE0) == 0) {
        return 16;
    }
    if (strcasecmp(pszName, D_00A55CE8) == 0) {
        return 17;
    }
    if (strcasecmp(pszName, D_00A55CF0) == 0) {
        return 18;
    }
    if (strcasecmp(pszName, D_00A55CF8) == 0) {
        return 9;
    }
    if (strcasecmp(pszName, D_00A55D00) == 0) {
        return 10;
    }
    if (strcasecmp(pszName, D_00A55D08) == 0) {
        return 19;
    }
    if (strcasecmp(pszName, D_00A55D10) == 0) {
        return 75;
    }
    if (strcasecmp(pszName, D_00A55D20) == 0) {
        return 20;
    }
    if (strcasecmp(pszName, D_00A55D28) == 0) {
        return 39;
    }
    if (strcasecmp(pszName, D_00A55D40) == 0) {
        return 40;
    }
    if (strcasecmp(pszName, D_00A55D58) == 0) {
        return 23;
    }
    if (strcasecmp(pszName, D_00A55D68) == 0) {
        return 24;
    }
    if (strcasecmp(pszName, D_00A55D78) == 0) {
        return 25;
    }
    if (strcasecmp(pszName, D_00A55D88) == 0) {
        return 26;
    }
    if (strcasecmp(pszName, D_00A55D98) == 0) {
        return 21;
    }
    if (strcasecmp(pszName, D_00A55DA8) == 0) {
        return 22;
    }
    if (strcasecmp(pszName, D_00A55DB8) == 0) {
        return 31;
    }
    if (strcasecmp(pszName, D_00A55DC8) == 0) {
        return 32;
    }
    if (strcasecmp(pszName, D_00A55DD8) == 0) {
        return 43;
    }
    if (strcasecmp(pszName, D_00A55DE8) == 0) {
        return 44;
    }
    if (strcasecmp(pszName, D_00A55DF8) == 0) {
        return 41;
    }
    if (strcasecmp(pszName, D_00A55E08) == 0) {
        return 42;
    }
    if (strcasecmp(pszName, D_00A55E18) == 0) {
        return 45;
    }
    if (strcasecmp(pszName, D_00A55E28) == 0) {
        return 46;
    }
    if (strcasecmp(pszName, D_00A55E38) == 0) {
        return 47;
    }
    if (strcasecmp(pszName, D_00A55E48) == 0) {
        return 48;
    }
    if (strcasecmp(pszName, D_00A55E58) == 0) {
        return 49;
    }
    if (strcasecmp(pszName, D_00A55E68) == 0) {
        return 78;
    }
    if (strcasecmp(pszName, D_00A55E70) == 0) {
        return 79;
    }
    if (strcasecmp(pszName, D_00A55E88) == 0) {
        return 0;
    }
    if (strcasecmp(pszName, D_00A55E90) == 0) {
        return 1;
    }
    if (strcasecmp(pszName, D_00A55E98) == 0) {
        return 2;
    }
    if (strcasecmp(pszName, D_00A55EA0) == 0) {
        return 3;
    }
    if (strcasecmp(pszName, D_00A55EA8) == 0) {
        return 4;
    }
    if (strcasecmp(pszName, D_00A55EB0) == 0) {
        return 5;
    }
    if (strcasecmp(pszName, D_00A55EB8) == 0) {
        return 6;
    }
    if (strcasecmp(pszName, D_00A55EC0) == 0) {
        return 7;
    }
    if (strcasecmp(pszName, D_00A55EC8) == 0) {
        return 8;
    }
    if (strcasecmp(pszName, D_00A55ED0) == 0) {
        return 9;
    }
    if (strcasecmp(pszName, D_00A55ED8) == 0) {
        return 10;
    }
    if (strcasecmp(pszName, D_00A55EE0) == 0) {
        return 11;
    }
    if (strcasecmp(pszName, D_00A55EE8) == 0) {
        return 12;
    }
    if (strcasecmp(pszName, D_00A55EF0) == 0) {
        return 13;
    }
    if (strcasecmp(pszName, D_00A55EF8) == 0) {
        return 14;
    }
    if (strcasecmp(pszName, D_00A55F00) == 0) {
        return 15;
    }
    if (strcasecmp(pszName, D_00A55F08) == 0) {
        return 16;
    }
    if (strcasecmp(pszName, D_00A55F10) == 0) {
        return 17;
    }
    if (strcasecmp(pszName, D_00A55F18) == 0) {
        return 18;
    }
    if (strcasecmp(pszName, D_00A55F20) == 0) {
        return 19;
    }
    if (strcasecmp(pszName, D_00A55F28) == 0) {
        return 20;
    }
    if (strcasecmp(pszName, D_00A55F30) == 0) {
        return 21;
    }
    if (strcasecmp(pszName, D_00A55F38) == 0) {
        return 22;
    }
    if (strcasecmp(pszName, D_00A55F40) == 0) {
        return 23;
    }
    if (strcasecmp(pszName, D_00A55F48) == 0) {
        return 24;
    }
    if (strcasecmp(pszName, D_00A55F50) == 0) {
        return 25;
    }
    if (strcasecmp(pszName, D_00A55F58) == 0) {
        return 26;
    }
    if (strcasecmp(pszName, D_00A55F60) == 0) {
        return 27;
    }
    if (strcasecmp(pszName, D_00A55F68) == 0) {
        return 28;
    }
    if (strcasecmp(pszName, D_00A55F70) == 0) {
        return 29;
    }
    if (strcasecmp(pszName, D_00A55F78) == 0) {
        return 30;
    }
    if (strcasecmp(pszName, D_00A55F80) == 0) {
        return 31;
    }
    if (strcasecmp(pszName, D_00A55F88) == 0) {
        return 32;
    }
    if (strcasecmp(pszName, D_00A55F90) == 0) {
        return 33;
    }
    if (strcasecmp(pszName, D_00A55F98) == 0) {
        return 34;
    }
    if (strcasecmp(pszName, D_00A55FA0) == 0) {
        return 35;
    }
    if (strcasecmp(pszName, D_00A55FA8) == 0) {
        return 36;
    }
    if (strcasecmp(pszName, D_00A55FB0) == 0) {
        return 37;
    }
    if (strcasecmp(pszName, D_00A55FB8) == 0) {
        return 38;
    }
    if (strcasecmp(pszName, D_00A55FC0) == 0) {
        return 39;
    }
    if (strcasecmp(pszName, D_00A55FC8) == 0) {
        return 40;
    }
    if (strcasecmp(pszName, D_00A55FD0) == 0) {
        return 41;
    }
    if (strcasecmp(pszName, D_00A55FD8) == 0) {
        return 42;
    }
    if (strcasecmp(pszName, D_00A55FE0) == 0) {
        return 43;
    }
    if (strcasecmp(pszName, D_00A55FE8) == 0) {
        return 44;
    }
    if (strcasecmp(pszName, D_00A55FF0) == 0) {
        return 45;
    }
    if (strcasecmp(pszName, D_00A55FF8) == 0) {
        return 46;
    }
    if (strcasecmp(pszName, D_00A56000) == 0) {
        return 47;
    }
    if (strcasecmp(pszName, D_00A56008) == 0) {
        return 48;
    }
    if (strcasecmp(pszName, D_00A56010) == 0) {
        return 49;
    }
    RETURN_MOTION_IF_MATCH(D_00A56018, 50);
    RETURN_MOTION_IF_MATCH(D_00A56020, 51);
    RETURN_MOTION_IF_MATCH(D_00A56028, 52);
    RETURN_MOTION_IF_MATCH(D_00A56030, 53);
    RETURN_MOTION_IF_MATCH(D_00A56038, 54);
    RETURN_MOTION_IF_MATCH(D_00A56040, 55);
    RETURN_MOTION_IF_MATCH(D_00A56048, 56);
    RETURN_MOTION_IF_MATCH(D_00A56050, 57);
    RETURN_MOTION_IF_MATCH(D_00A56058, 58);
    RETURN_MOTION_IF_MATCH(D_00A56060, 59);
    RETURN_MOTION_IF_MATCH(D_00A56068, 60);
    RETURN_MOTION_IF_MATCH(D_00A56070, 61);
    RETURN_MOTION_IF_MATCH(D_00A56078, 62);
    RETURN_MOTION_IF_MATCH(D_00A56080, 63);
    RETURN_MOTION_IF_MATCH(D_00A56088, 64);
    RETURN_MOTION_IF_MATCH(D_00A56090, 65);
    RETURN_MOTION_IF_MATCH(D_00A56098, 66);
    RETURN_MOTION_IF_MATCH(D_00A560A0, 67);
    RETURN_MOTION_IF_MATCH(D_00A560A8, 68);
    RETURN_MOTION_IF_MATCH(D_00A560B0, 69);
    RETURN_MOTION_IF_MATCH(D_00A560B8, 70);
    RETURN_MOTION_IF_MATCH(D_00A560C0, 71);
    RETURN_MOTION_IF_MATCH(D_00A560C8, 72);
    RETURN_MOTION_IF_MATCH(D_00A560D0, 73);
    RETURN_MOTION_IF_MATCH(D_00A560D8, 74);
    RETURN_MOTION_IF_MATCH(D_00A560E0, 75);
    RETURN_MOTION_IF_MATCH(D_00A560E8, 76);
    RETURN_MOTION_IF_MATCH(D_00A560F0, 77);
    RETURN_MOTION_IF_MATCH(D_00A560F8, 78);
    RETURN_MOTION_IF_MATCH(D_00A56100, 79);
    RETURN_MOTION_IF_MATCH(D_00A56108, 80);
    RETURN_MOTION_IF_MATCH(D_00A56110, 81);
    RETURN_MOTION_IF_MATCH(D_00A56118, 82);
    RETURN_MOTION_IF_MATCH(D_00A56120, 83);
    RETURN_MOTION_IF_MATCH(D_00A56128, 84);
    RETURN_MOTION_IF_MATCH(D_00A56130, 85);
    RETURN_MOTION_IF_MATCH(D_00A56138, 86);
    RETURN_MOTION_IF_MATCH(D_00A56140, 87);
    RETURN_MOTION_IF_MATCH(D_00A56148, 88);
    RETURN_MOTION_IF_MATCH(D_00A56150, 89);
    RETURN_MOTION_IF_MATCH(D_00A56158, 90);
    RETURN_MOTION_IF_MATCH(D_00A56160, 91);
    RETURN_MOTION_IF_MATCH(D_00A56168, 92);
    RETURN_MOTION_IF_MATCH(D_00A56170, 93);
    RETURN_MOTION_IF_MATCH(D_00A56178, 94);
    RETURN_MOTION_IF_MATCH(D_00A56180, 95);
    RETURN_MOTION_IF_MATCH(D_00A56188, 96);
    RETURN_MOTION_IF_MATCH(D_00A56190, 97);
    RETURN_MOTION_IF_MATCH(D_00A56198, 98);
    RETURN_MOTION_IF_MATCH(D_00A561A0, 2048);
    RETURN_MOTION_IF_MATCH(D_00A561A8, 2049);
    RETURN_MOTION_IF_MATCH(D_00A561B0, 2050);
    RETURN_MOTION_IF_MATCH(D_00A561B8, 2051);
    RETURN_MOTION_IF_MATCH(D_00A561C0, 2052);
    RETURN_MOTION_IF_MATCH(D_00A561C8, 2053);
    RETURN_MOTION_IF_MATCH(D_00A561D0, 2054);
    RETURN_MOTION_IF_MATCH(D_00A561D8, 2055);
    RETURN_MOTION_IF_MATCH(D_00A561E0, 2056);
    RETURN_MOTION_IF_MATCH(D_00A561E8, 2057);
    RETURN_MOTION_IF_MATCH(D_00A561F0, 2058);
    RETURN_MOTION_IF_MATCH(D_00A561F8, 2059);
    RETURN_MOTION_IF_MATCH(D_00A56200, 2060);
    RETURN_MOTION_IF_MATCH(D_00A56208, 2061);
    RETURN_MOTION_IF_MATCH(D_00A56210, 2062);
    RETURN_MOTION_IF_MATCH(D_00A56218, 2063);
    RETURN_MOTION_IF_MATCH(D_00A56220, 2064);
    RETURN_MOTION_IF_MATCH(D_00A56228, 2065);
    RETURN_MOTION_IF_MATCH(D_00A56230, 2066);
    RETURN_MOTION_IF_MATCH(D_00A56238, 2067);
    RETURN_MOTION_IF_MATCH(D_00A56240, 2068);
    RETURN_MOTION_IF_MATCH(D_00A56248, 2069);
    RETURN_MOTION_IF_MATCH(D_00A56250, 2070);
    RETURN_MOTION_IF_MATCH(D_00A56258, 2071);
    RETURN_MOTION_IF_MATCH(D_00A56260, 2072);
    RETURN_MOTION_IF_MATCH(D_00A56268, 2073);
    RETURN_MOTION_IF_MATCH(D_00A56270, 2074);
    RETURN_MOTION_IF_MATCH(D_00A56278, 2075);
    RETURN_MOTION_IF_MATCH(D_00A56280, 2076);
    RETURN_MOTION_IF_MATCH(D_00A56288, 2077);
    RETURN_MOTION_IF_MATCH(D_00A56290, 2078);
    RETURN_MOTION_IF_MATCH(D_00A56298, 2079);
    RETURN_MOTION_IF_MATCH(D_00A562A0, 2080);
    RETURN_MOTION_IF_MATCH(D_00A562A8, 2081);
    RETURN_MOTION_IF_MATCH(D_00A562B0, 2082);
    RETURN_MOTION_IF_MATCH(D_00A562B8, 2083);
    RETURN_MOTION_IF_MATCH(D_00A562C0, 2084);
    RETURN_MOTION_IF_MATCH(D_00A562C8, 2085);
    RETURN_MOTION_IF_MATCH(D_00A562D0, 2086);
    RETURN_MOTION_IF_MATCH(D_00A562D8, 2087);
    RETURN_MOTION_IF_MATCH(D_00A562E0, 2088);
    RETURN_MOTION_IF_MATCH(D_00A562E8, 2089);
    RETURN_MOTION_IF_MATCH(D_00A562F0, 2090);
    RETURN_MOTION_IF_MATCH(D_00A562F8, 2091);
    RETURN_MOTION_IF_MATCH(D_00A56300, 2092);
    RETURN_MOTION_IF_MATCH(D_00A56308, 2093);
    RETURN_MOTION_IF_MATCH(D_00A56310, 2094);
    RETURN_MOTION_IF_MATCH(D_00A56318, 2095);
    RETURN_MOTION_IF_MATCH(D_00A56320, 2096);
    RETURN_MOTION_IF_MATCH(D_00A56328, 2097);
    RETURN_MOTION_IF_MATCH(D_00A56330, 2098);
    RETURN_MOTION_IF_MATCH(D_00A56338, 2099);
    RETURN_MOTION_IF_MATCH(D_00A56340, 2100);
    RETURN_MOTION_IF_MATCH(D_00A56348, 2101);
    RETURN_MOTION_IF_MATCH(D_00A56350, 2102);
    RETURN_MOTION_IF_MATCH(D_00A56358, 2103);
    RETURN_MOTION_IF_MATCH(D_00A56360, 2104);
    RETURN_MOTION_IF_MATCH(D_00A56368, 2105);
    RETURN_MOTION_IF_MATCH(D_00A56370, 2106);
    RETURN_MOTION_IF_MATCH(D_00A56378, 2107);
    RETURN_MOTION_IF_MATCH(D_00A56380, 2108);
    RETURN_MOTION_IF_MATCH(D_00A56388, 2109);
    RETURN_MOTION_IF_MATCH(D_00A56390, 2110);
    RETURN_MOTION_IF_MATCH(D_00A56398, 2111);
    RETURN_MOTION_IF_MATCH(D_00A563A0, 2112);
    RETURN_MOTION_IF_MATCH(D_00A563A8, 2113);
    RETURN_MOTION_IF_MATCH(D_00A563B0, 2114);
    RETURN_MOTION_IF_MATCH(D_00A563B8, 2115);
    RETURN_MOTION_IF_MATCH(D_00A563C0, 2116);
    RETURN_MOTION_IF_MATCH(D_00A563C8, 2117);
    RETURN_MOTION_IF_MATCH(D_00A563D0, 2118);
    RETURN_MOTION_IF_MATCH(D_00A563D8, 2119);
    RETURN_MOTION_IF_MATCH(D_00A563E0, 2120);
    RETURN_MOTION_IF_MATCH(D_00A563E8, 2121);
    RETURN_MOTION_IF_MATCH(D_00A563F0, 2122);
    RETURN_MOTION_IF_MATCH(D_00A563F8, 2123);
    RETURN_MOTION_IF_MATCH(D_00A56400, 2124);
    RETURN_MOTION_IF_MATCH(D_00A56408, 2125);
    RETURN_MOTION_IF_MATCH(D_00A56410, 2126);
    RETURN_MOTION_IF_MATCH(D_00A56418, 2127);
    RETURN_MOTION_IF_MATCH(D_00A56420, 2128);
    RETURN_MOTION_IF_MATCH(D_00A56428, 2129);
    RETURN_MOTION_IF_MATCH(D_00A56430, 2130);
    RETURN_MOTION_IF_MATCH(D_00A56438, 2131);
    RETURN_MOTION_IF_MATCH(D_00A56440, 2132);
    RETURN_MOTION_IF_MATCH(D_00A56448, 2133);
    RETURN_MOTION_IF_MATCH(D_00A56450, 2134);
    RETURN_MOTION_IF_MATCH(D_00A56458, 2135);
    RETURN_MOTION_IF_MATCH(D_00A56460, 2136);
    RETURN_MOTION_IF_MATCH(D_00A56468, 2137);
    RETURN_MOTION_IF_MATCH(D_00A56470, 2138);
    RETURN_MOTION_IF_MATCH(D_00A56478, 2139);
    RETURN_MOTION_IF_MATCH(D_00A56480, 2140);
    RETURN_MOTION_IF_MATCH(D_00A56488, 2141);
    RETURN_MOTION_IF_MATCH(D_00A56490, 2142);
    RETURN_MOTION_IF_MATCH(D_00A56498, 2143);
    RETURN_MOTION_IF_MATCH(D_00A564A0, 2144);
    RETURN_MOTION_IF_MATCH(D_00A564A8, 2145);
    RETURN_MOTION_IF_MATCH(D_00A564B0, 2146);
    if (strcasecmp(pszName, D_00A564B8) != 0) {
        RgError(D_00A564C0, D_00A55C58, 26, pszName);
    }

    return -1;
}
