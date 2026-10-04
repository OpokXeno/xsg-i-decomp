/*
 * OV12 original TU 67: 0x00a368e8..0x00a36de8 (2 functions)
 */
#include "common.h"

extern void assert_prog(const char *expression, const char *file, int line);
extern int strcasecmp(const char *left, const char *right);

static const char D_00A564E0[16] = "pszName != NIL";
static const char D_00A564F0[32] = "../rg_actor_charid.euc.c";
static const char D_00A56510[8] = "vx01";
static const char D_00A56518[8] = "ag05";
static const char D_00A56520[8] = "vx02";
static const char D_00A56528[8] = "ag04";
static const char D_00A56530[8] = "vx06";
static const char D_00A56538[8] = "ag02";
static const char D_00A56540[8] = "hg00";
static const char D_00A56548[8] = "rf01";
static const char D_00A56550[8] = "glg00";
static const char D_00A56558[8] = "bd00";
static const char D_00A56560[8] = "rf00";
static const char D_00A56568[8] = "mg00";
static const char D_00A56570[8] = "gnd00";
static const char D_00A56578[8] = "bd01";
static const char D_00A56580[8] = "hmr00";
static const char D_00A56588[8] = "hmr01";
static const char D_00A56590[8] = "fl00";
static const char D_00A56598[8] = "bdf00";
static const char D_00A565A0[8] = "bam00";
static const char D_00A565A8[8] = "msp00";
static const char D_00A565B0[8] = "vsd00";
static const char D_00A565B8[8] = "rf02";
static const char D_00A565C0[8] = "cb00";
static const char D_00A565C8[8] = "bp00";
static const char D_00A565D0[8] = "hg01";

int RgActorNameToCharID(const char *pszName) {
    int charID;

    charID = -1;
    if (pszName == 0) {
        assert_prog(D_00A564E0, D_00A564F0, 19);
    }

    if (strcasecmp(pszName, D_00A56510) == 0) {
        charID = 0;
    }
    if (strcasecmp(pszName, D_00A56518) == 0) {
        charID = 1;
    }
    if (strcasecmp(pszName, D_00A56520) == 0) {
        charID = 2;
    }
    if (strcasecmp(pszName, D_00A56528) == 0) {
        charID = 3;
    }
    if (strcasecmp(pszName, D_00A56530) == 0) {
        charID = 4;
    }
    if (strcasecmp(pszName, D_00A56538) == 0) {
        charID = 5;
    }
    if (strcasecmp(pszName, D_00A56540) == 0) {
        charID = 6;
    }
    if (strcasecmp(pszName, D_00A56548) == 0) {
        charID = 7;
    }
    if (strcasecmp(pszName, D_00A56550) == 0) {
        charID = 8;
    }
    if (strcasecmp(pszName, D_00A56558) == 0) {
        charID = 9;
    }
    if (strcasecmp(pszName, D_00A56560) == 0) {
        charID = 10;
    }
    if (strcasecmp(pszName, D_00A56568) == 0) {
        charID = 11;
    }
    if (strcasecmp(pszName, D_00A56570) == 0) {
        charID = 12;
    }
    if (strcasecmp(pszName, D_00A56578) == 0) {
        charID = 13;
    }
    if (strcasecmp(pszName, D_00A56580) == 0) {
        charID = 14;
    }
    if (strcasecmp(pszName, D_00A56588) == 0) {
        charID = 15;
    }
    if (strcasecmp(pszName, D_00A56590) == 0) {
        charID = 16;
    }
    if (strcasecmp(pszName, D_00A56598) == 0) {
        charID = 17;
    }
    if (strcasecmp(pszName, D_00A565A0) == 0) {
        charID = 18;
    }
    if (strcasecmp(pszName, D_00A565A8) == 0) {
        charID = 19;
    }
    if (strcasecmp(pszName, D_00A565B0) == 0) {
        charID = 20;
    }
    if (strcasecmp(pszName, D_00A565B8) == 0) {
        charID = 21;
    }
    if (strcasecmp(pszName, D_00A565C0) == 0) {
        charID = 22;
    }
    if (strcasecmp(pszName, D_00A565C8) == 0) {
        charID = 23;
    }
    if (strcasecmp(pszName, D_00A565D0) == 0) {
        charID = 24;
    }

    return charID;
}

const char *RgActorCharIDToName(int charID) {
    if (charID == 0) {
        return D_00A56510;
    }
    if (charID == 1) {
        return D_00A56518;
    }
    if (charID == 2) {
        return D_00A56520;
    }
    if (charID == 3) {
        return D_00A56528;
    }
    if (charID == 4) {
        return D_00A56530;
    }
    if (charID == 5) {
        return D_00A56538;
    }
    if (charID == 6) {
        return D_00A56540;
    }
    if (charID == 7) {
        return D_00A56548;
    }
    if (charID == 8) {
        return D_00A56550;
    }
    if (charID == 9) {
        return D_00A56558;
    }
    if (charID == 10) {
        return D_00A56560;
    }
    if (charID == 11) {
        return D_00A56568;
    }
    if (charID == 12) {
        return D_00A56570;
    }
    if (charID == 13) {
        return D_00A56578;
    }
    if (charID == 14) {
        return D_00A56580;
    }
    if (charID == 15) {
        return D_00A56588;
    }
    if (charID == 16) {
        return D_00A56590;
    }
    if (charID == 17) {
        return D_00A56598;
    }
    if (charID == 18) {
        return D_00A565A0;
    }
    if (charID == 19) {
        return D_00A565A8;
    }
    if (charID == 20) {
        return D_00A565B0;
    }
    if (charID == 21) {
        return D_00A565B8;
    }
    if (charID == 22) {
        return D_00A565C0;
    }
    if (charID == 23) {
        return D_00A565C8;
    }
    if (charID == 24) {
        return D_00A565D0;
    }
    return 0;
}
