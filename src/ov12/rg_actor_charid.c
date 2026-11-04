/*
 * OV12 original TU 67: 0x00a368e8..0x00a36de8 (2 functions)
 */
#include "common.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_actor_charid.h"


static const char actorCharIdAssertNameNotNil[16] = "pszName != NIL";
static const char actorCharIdSourcePath[32] = "../rg_actor_charid.euc.c";
static const char actorCharIdNameVx01[8] = "vx01";
static const char actorCharIdNameAg05[8] = "ag05";
static const char actorCharIdNameVx02[8] = "vx02";
static const char actorCharIdNameAg04[8] = "ag04";
static const char actorCharIdNameVx06[8] = "vx06";
static const char actorCharIdNameAg02[8] = "ag02";
static const char actorCharIdNameHg00[8] = "hg00";
static const char actorCharIdNameRf01[8] = "rf01";
static const char actorCharIdNameGlg00[8] = "glg00";
static const char actorCharIdNameBd00[8] = "bd00";
static const char actorCharIdNameRf00[8] = "rf00";
static const char actorCharIdNameMg00[8] = "mg00";
static const char actorCharIdNameGnd00[8] = "gnd00";
static const char actorCharIdNameBd01[8] = "bd01";
static const char actorCharIdNameHmr00[8] = "hmr00";
static const char actorCharIdNameHmr01[8] = "hmr01";
static const char actorCharIdNameFl00[8] = "fl00";
static const char actorCharIdNameBdf00[8] = "bdf00";
static const char actorCharIdNameBam00[8] = "bam00";
static const char actorCharIdNameMsp00[8] = "msp00";
static const char actorCharIdNameVsd00[8] = "vsd00";
static const char actorCharIdNameRf02[8] = "rf02";
static const char actorCharIdNameCb00[8] = "cb00";
static const char actorCharIdNameBp00[8] = "bp00";
static const char actorCharIdNameHg01[8] = "hg01";

int RgActorNameToCharID(const char *pszName) {
    int charID;

    charID = RG_ACTOR_CHAR_NOP;
    if (pszName == 0) {
        assert_prog(actorCharIdAssertNameNotNil, actorCharIdSourcePath, 19);
    }

    if (strcasecmp(pszName, actorCharIdNameVx01) == 0) {
        charID = RG_ACTOR_CHAR_VX01;
    }
    if (strcasecmp(pszName, actorCharIdNameAg05) == 0) {
        charID = RG_ACTOR_CHAR_AG05;
    }
    if (strcasecmp(pszName, actorCharIdNameVx02) == 0) {
        charID = RG_ACTOR_CHAR_VX02;
    }
    if (strcasecmp(pszName, actorCharIdNameAg04) == 0) {
        charID = RG_ACTOR_CHAR_AG04;
    }
    if (strcasecmp(pszName, actorCharIdNameVx06) == 0) {
        charID = RG_ACTOR_CHAR_VX06;
    }
    if (strcasecmp(pszName, actorCharIdNameAg02) == 0) {
        charID = RG_ACTOR_CHAR_AG02;
    }
    if (strcasecmp(pszName, actorCharIdNameHg00) == 0) {
        charID = RG_ACTOR_CHAR_HG00;
    }
    if (strcasecmp(pszName, actorCharIdNameRf01) == 0) {
        charID = RG_ACTOR_CHAR_RF01;
    }
    if (strcasecmp(pszName, actorCharIdNameGlg00) == 0) {
        charID = RG_ACTOR_CHAR_GLG00;
    }
    if (strcasecmp(pszName, actorCharIdNameBd00) == 0) {
        charID = RG_ACTOR_CHAR_BD00;
    }
    if (strcasecmp(pszName, actorCharIdNameRf00) == 0) {
        charID = RG_ACTOR_CHAR_RF00;
    }
    if (strcasecmp(pszName, actorCharIdNameMg00) == 0) {
        charID = RG_ACTOR_CHAR_MG00;
    }
    if (strcasecmp(pszName, actorCharIdNameGnd00) == 0) {
        charID = RG_ACTOR_CHAR_GND00;
    }
    if (strcasecmp(pszName, actorCharIdNameBd01) == 0) {
        charID = RG_ACTOR_CHAR_BD01;
    }
    if (strcasecmp(pszName, actorCharIdNameHmr00) == 0) {
        charID = RG_ACTOR_CHAR_HMR00;
    }
    if (strcasecmp(pszName, actorCharIdNameHmr01) == 0) {
        charID = RG_ACTOR_CHAR_HMR01;
    }
    if (strcasecmp(pszName, actorCharIdNameFl00) == 0) {
        charID = RG_ACTOR_CHAR_FL00;
    }
    if (strcasecmp(pszName, actorCharIdNameBdf00) == 0) {
        charID = RG_ACTOR_CHAR_BDF00;
    }
    if (strcasecmp(pszName, actorCharIdNameBam00) == 0) {
        charID = RG_ACTOR_CHAR_BAM00;
    }
    if (strcasecmp(pszName, actorCharIdNameMsp00) == 0) {
        charID = RG_ACTOR_CHAR_MSP00;
    }
    if (strcasecmp(pszName, actorCharIdNameVsd00) == 0) {
        charID = RG_ACTOR_CHAR_VSD00;
    }
    if (strcasecmp(pszName, actorCharIdNameRf02) == 0) {
        charID = RG_ACTOR_CHAR_RF02;
    }
    if (strcasecmp(pszName, actorCharIdNameCb00) == 0) {
        charID = RG_ACTOR_CHAR_CB00;
    }
    if (strcasecmp(pszName, actorCharIdNameBp00) == 0) {
        charID = RG_ACTOR_CHAR_BP00;
    }
    if (strcasecmp(pszName, actorCharIdNameHg01) == 0) {
        charID = RG_ACTOR_CHAR_HG01;
    }

    return charID;
}

const char *RgActorCharIDToName(int charID) {
    if (charID == RG_ACTOR_CHAR_VX01) {
        return actorCharIdNameVx01;
    }
    if (charID == RG_ACTOR_CHAR_AG05) {
        return actorCharIdNameAg05;
    }
    if (charID == RG_ACTOR_CHAR_VX02) {
        return actorCharIdNameVx02;
    }
    if (charID == RG_ACTOR_CHAR_AG04) {
        return actorCharIdNameAg04;
    }
    if (charID == RG_ACTOR_CHAR_VX06) {
        return actorCharIdNameVx06;
    }
    if (charID == RG_ACTOR_CHAR_AG02) {
        return actorCharIdNameAg02;
    }
    if (charID == RG_ACTOR_CHAR_HG00) {
        return actorCharIdNameHg00;
    }
    if (charID == RG_ACTOR_CHAR_RF01) {
        return actorCharIdNameRf01;
    }
    if (charID == RG_ACTOR_CHAR_GLG00) {
        return actorCharIdNameGlg00;
    }
    if (charID == RG_ACTOR_CHAR_BD00) {
        return actorCharIdNameBd00;
    }
    if (charID == RG_ACTOR_CHAR_RF00) {
        return actorCharIdNameRf00;
    }
    if (charID == RG_ACTOR_CHAR_MG00) {
        return actorCharIdNameMg00;
    }
    if (charID == RG_ACTOR_CHAR_GND00) {
        return actorCharIdNameGnd00;
    }
    if (charID == RG_ACTOR_CHAR_BD01) {
        return actorCharIdNameBd01;
    }
    if (charID == RG_ACTOR_CHAR_HMR00) {
        return actorCharIdNameHmr00;
    }
    if (charID == RG_ACTOR_CHAR_HMR01) {
        return actorCharIdNameHmr01;
    }
    if (charID == RG_ACTOR_CHAR_FL00) {
        return actorCharIdNameFl00;
    }
    if (charID == RG_ACTOR_CHAR_BDF00) {
        return actorCharIdNameBdf00;
    }
    if (charID == RG_ACTOR_CHAR_BAM00) {
        return actorCharIdNameBam00;
    }
    if (charID == RG_ACTOR_CHAR_MSP00) {
        return actorCharIdNameMsp00;
    }
    if (charID == RG_ACTOR_CHAR_VSD00) {
        return actorCharIdNameVsd00;
    }
    if (charID == RG_ACTOR_CHAR_RF02) {
        return actorCharIdNameRf02;
    }
    if (charID == RG_ACTOR_CHAR_CB00) {
        return actorCharIdNameCb00;
    }
    if (charID == RG_ACTOR_CHAR_BP00) {
        return actorCharIdNameBp00;
    }
    if (charID == RG_ACTOR_CHAR_HG01) {
        return actorCharIdNameHg01;
    }
    return 0;
}
