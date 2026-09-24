/*
 * OV12 original TU 66: 0x00a34c68..0x00a368e8 (1 functions)
 */
#include "common.h"

extern void assert_prog(const char *expression, const char *file, int line);
extern int strcasecmp(const char *left, const char *right);
extern void RgError(const char *message, const char *source_file, int line, ...);

extern const char D_00A55C48[];
extern const char D_00A55C58[];
extern const char D_00A55C70[];
extern const char D_00A55C78[];
extern const char D_00A55C80[];
extern const char D_00A55C88[];
extern const char D_00A55C90[];
extern const char D_00A55C98[];
extern const char D_00A55CA0[];
extern const char D_00A55CA8[];
extern const char D_00A55CB0[];
extern const char D_00A55CB8[];
extern const char D_00A55CC0[];
extern const char D_00A55CC8[];
extern const char D_00A55CD0[];
extern const char D_00A55CD8[];
extern const char D_00A55CE0[];
extern const char D_00A55CE8[];
extern const char D_00A55CF0[];
extern const char D_00A55CF8[];
extern const char D_00A55D00[];
extern const char D_00A55D08[];
extern const char D_00A55D10[];
extern const char D_00A55D20[];
extern const char D_00A55D28[];
extern const char D_00A55D40[];
extern const char D_00A55D58[];
extern const char D_00A55D68[];
extern const char D_00A55D78[];
extern const char D_00A55D88[];
extern const char D_00A55D98[];
extern const char D_00A55DA8[];
extern const char D_00A55DB8[];
extern const char D_00A55DC8[];
extern const char D_00A55DD8[];
extern const char D_00A55DE8[];
extern const char D_00A55DF8[];
extern const char D_00A55E08[];
extern const char D_00A55E18[];
extern const char D_00A55E28[];
extern const char D_00A55E38[];
extern const char D_00A55E48[];
extern const char D_00A55E58[];
extern const char D_00A55E68[];
extern const char D_00A55E70[];
extern const char D_00A55E88[];
extern const char D_00A55E90[];
extern const char D_00A55E98[];
extern const char D_00A55EA0[];
extern const char D_00A55EA8[];
extern const char D_00A55EB0[];
extern const char D_00A55EB8[];
extern const char D_00A55EC0[];
extern const char D_00A55EC8[];
extern const char D_00A55ED0[];
extern const char D_00A55ED8[];
extern const char D_00A55EE0[];
extern const char D_00A55EE8[];
extern const char D_00A55EF0[];
extern const char D_00A55EF8[];
extern const char D_00A55F00[];
extern const char D_00A55F08[];
extern const char D_00A55F10[];
extern const char D_00A55F18[];
extern const char D_00A55F20[];
extern const char D_00A55F28[];
extern const char D_00A55F30[];
extern const char D_00A55F38[];
extern const char D_00A55F40[];
extern const char D_00A55F48[];
extern const char D_00A55F50[];
extern const char D_00A55F58[];
extern const char D_00A55F60[];
extern const char D_00A55F68[];
extern const char D_00A55F70[];
extern const char D_00A55F78[];
extern const char D_00A55F80[];
extern const char D_00A55F88[];
extern const char D_00A55F90[];
extern const char D_00A55F98[];
extern const char D_00A55FA0[];
extern const char D_00A55FA8[];
extern const char D_00A55FB0[];
extern const char D_00A55FB8[];
extern const char D_00A55FC0[];
extern const char D_00A55FC8[];
extern const char D_00A55FD0[];
extern const char D_00A55FD8[];
extern const char D_00A55FE0[];
extern const char D_00A55FE8[];
extern const char D_00A55FF0[];
extern const char D_00A55FF8[];
extern const char D_00A56000[];
extern const char D_00A56008[];
extern const char D_00A56010[];
extern const char D_00A56018[];
extern const char D_00A56020[];
extern const char D_00A56028[];
extern const char D_00A56030[];
extern const char D_00A56038[];
extern const char D_00A56040[];
extern const char D_00A56048[];
extern const char D_00A56050[];
extern const char D_00A56058[];
extern const char D_00A56060[];
extern const char D_00A56068[];
extern const char D_00A56070[];
extern const char D_00A56078[];
extern const char D_00A56080[];
extern const char D_00A56088[];
extern const char D_00A56090[];
extern const char D_00A56098[];
extern const char D_00A560A0[];
extern const char D_00A560A8[];
extern const char D_00A560B0[];
extern const char D_00A560B8[];
extern const char D_00A560C0[];
extern const char D_00A560C8[];
extern const char D_00A560D0[];
extern const char D_00A560D8[];
extern const char D_00A560E0[];
extern const char D_00A560E8[];
extern const char D_00A560F0[];
extern const char D_00A560F8[];
extern const char D_00A56100[];
extern const char D_00A56108[];
extern const char D_00A56110[];
extern const char D_00A56118[];
extern const char D_00A56120[];
extern const char D_00A56128[];
extern const char D_00A56130[];
extern const char D_00A56138[];
extern const char D_00A56140[];
extern const char D_00A56148[];
extern const char D_00A56150[];
extern const char D_00A56158[];
extern const char D_00A56160[];
extern const char D_00A56168[];
extern const char D_00A56170[];
extern const char D_00A56178[];
extern const char D_00A56180[];
extern const char D_00A56188[];
extern const char D_00A56190[];
extern const char D_00A56198[];
extern const char D_00A561A0[];
extern const char D_00A561A8[];
extern const char D_00A561B0[];
extern const char D_00A561B8[];
extern const char D_00A561C0[];
extern const char D_00A561C8[];
extern const char D_00A561D0[];
extern const char D_00A561D8[];
extern const char D_00A561E0[];
extern const char D_00A561E8[];
extern const char D_00A561F0[];
extern const char D_00A561F8[];
extern const char D_00A56200[];
extern const char D_00A56208[];
extern const char D_00A56210[];
extern const char D_00A56218[];
extern const char D_00A56220[];
extern const char D_00A56228[];
extern const char D_00A56230[];
extern const char D_00A56238[];
extern const char D_00A56240[];
extern const char D_00A56248[];
extern const char D_00A56250[];
extern const char D_00A56258[];
extern const char D_00A56260[];
extern const char D_00A56268[];
extern const char D_00A56270[];
extern const char D_00A56278[];
extern const char D_00A56280[];
extern const char D_00A56288[];
extern const char D_00A56290[];
extern const char D_00A56298[];
extern const char D_00A562A0[];
extern const char D_00A562A8[];
extern const char D_00A562B0[];
extern const char D_00A562B8[];
extern const char D_00A562C0[];
extern const char D_00A562C8[];
extern const char D_00A562D0[];
extern const char D_00A562D8[];
extern const char D_00A562E0[];
extern const char D_00A562E8[];
extern const char D_00A562F0[];
extern const char D_00A562F8[];
extern const char D_00A56300[];
extern const char D_00A56308[];
extern const char D_00A56310[];
extern const char D_00A56318[];
extern const char D_00A56320[];
extern const char D_00A56328[];
extern const char D_00A56330[];
extern const char D_00A56338[];
extern const char D_00A56340[];
extern const char D_00A56348[];
extern const char D_00A56350[];
extern const char D_00A56358[];
extern const char D_00A56360[];
extern const char D_00A56368[];
extern const char D_00A56370[];
extern const char D_00A56378[];
extern const char D_00A56380[];
extern const char D_00A56388[];
extern const char D_00A56390[];
extern const char D_00A56398[];
extern const char D_00A563A0[];
extern const char D_00A563A8[];
extern const char D_00A563B0[];
extern const char D_00A563B8[];
extern const char D_00A563C0[];
extern const char D_00A563C8[];
extern const char D_00A563D0[];
extern const char D_00A563D8[];
extern const char D_00A563E0[];
extern const char D_00A563E8[];
extern const char D_00A563F0[];
extern const char D_00A563F8[];
extern const char D_00A56400[];
extern const char D_00A56408[];
extern const char D_00A56410[];
extern const char D_00A56418[];
extern const char D_00A56420[];
extern const char D_00A56428[];
extern const char D_00A56430[];
extern const char D_00A56438[];
extern const char D_00A56440[];
extern const char D_00A56448[];
extern const char D_00A56450[];
extern const char D_00A56458[];
extern const char D_00A56460[];
extern const char D_00A56468[];
extern const char D_00A56470[];
extern const char D_00A56478[];
extern const char D_00A56480[];
extern const char D_00A56488[];
extern const char D_00A56490[];
extern const char D_00A56498[];
extern const char D_00A564A0[];
extern const char D_00A564A8[];
extern const char D_00A564B0[];
extern const char D_00A564B8[];
extern const char D_00A564C0[];

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
