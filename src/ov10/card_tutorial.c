/*
 * OV10 original TU 7: 0x00a21348..0x00a21768 (3 functions)
 */
#include "common.h"
#include "shared.h"

extern char *strcat(char *destination, const char *source);
extern char *strcpy(char *destination, const char *source);
extern int xglCdReadFile(const char *name, void *buffer, int mode, int flags);

/* "data\carddata\tuto\" */
char datapath[0x14] = "data\\carddata\\tuto\\";
extern const char D_00A4E320[];
extern const char D_00A4E328[];
extern const char D_00A4E330[];
extern const char D_00A4E338[];
extern const char D_00A4E340[];
extern const char D_00A4E348[];
extern const char D_00A4E350[];
extern const char D_00A4E358[];
extern const char D_00A4E360[];
extern const char D_00A4E368[];
extern const char D_00A4E370[];
extern const char D_00A4E378[];
extern const char D_00A4E380[];
extern const char D_00A4E388[];
extern const char D_00A4E390[];
extern const char D_00A4E398[];
extern const char D_00A4E3A0[];
extern const char D_00A4E3A8[];
extern const char D_00A4E3B0[];
extern const char D_00A4E3B8[];
extern const char D_00A4E3C0[];
extern const char D_00A4E3C8[];
extern const char D_00A4E3D0[];
extern const char D_00A4E3D8[];
extern const char D_00A4E3E0[];
extern const char D_00A4E3E8[];
extern const char D_00A4E3F0[];
extern const char D_00A4E3F8[];
extern const char D_00A4E400[];
extern const char D_00A4E408[];
extern const char D_00A4E410[];
extern const char D_00A4E418[];
extern const char D_00A4E420[];
extern const char D_00A4E428[];
extern const char D_00A4E430[];
extern const char D_00A4E438[];
extern const char D_00A4E440[];
extern const char D_00A4E448[];
extern const char D_00A4E450[];
extern const char D_00A4E458[];
extern const char D_00A4E460[];
extern const char D_00A4E468[];
extern const char D_00A4E470[];
extern const char D_00A4E478[];
extern const char D_00A4E480[];
extern const char D_00A4E488[];
extern const char D_00A4E490[];
static const char *tutoxtx_tbl[64] = {
    D_00A4E490, D_00A4E488, D_00A4E480, D_00A4E478,
    D_00A4E470, D_00A4E468, D_00A4E460, D_00A4E458,
    D_00A4E450, D_00A4E448, D_00A4E440, D_00A4E438,
    D_00A4E430, D_00A4E428, D_00A4E420, D_00A4E418,
    D_00A4E410, D_00A4E408, D_00A4E400, D_00A4E3F8,
    D_00A4E3F0, D_00A4E3E8, D_00A4E3E0, D_00A4E3D8,
    D_00A4E3D0, D_00A4E3C8, D_00A4E3C0, D_00A4E3B8,
    D_00A4E3B0, D_00A4E3A8, D_00A4E3A0, D_00A4E398,
    D_00A4E390, D_00A4E388, D_00A4E380, D_00A4E378,
    D_00A4E370, D_00A4E368, D_00A4E360, D_00A4E358,
    D_00A4E350, D_00A4E348, D_00A4E340, D_00A4E338,
    D_00A4E330, D_00A4E328, D_00A4E320,
};
const char D_00A4E498[8] = ".xtx";
static void *tuxtx;

void CardTutorialFileLoad(int tutorialIndex)
{
    char path[0x80];

    memset(path, 0, sizeof(path));
    strcpy(path, datapath);
    strcat(path, tutoxtx_tbl[tutorialIndex]);
    strcat(path, D_00A4E498);
    xglCdReadFile(path, tuxtx, 0, 1);
}

INCLUDE_ASM("asm/nonmatchings/ov10/card_tutorial", CardTutorialInit);

INCLUDE_ASM("asm/nonmatchings/ov10/card_tutorial", CardTutorialProc);



const char D_00A4E320[8] = "Tu_046";

const char D_00A4E328[8] = "Tu_045";

const char D_00A4E330[8] = "Tu_044";

const char D_00A4E338[8] = "Tu_043";

const char D_00A4E340[8] = "Tu_042";

const char D_00A4E348[8] = "Tu_041";

const char D_00A4E350[8] = "Tu_040";

const char D_00A4E358[8] = "Tu_039";

const char D_00A4E360[8] = "Tu_038";

const char D_00A4E368[8] = "Tu_037";

const char D_00A4E370[8] = "Tu_036";

const char D_00A4E378[8] = "Tu_035";

const char D_00A4E380[8] = "Tu_034";

const char D_00A4E388[8] = "Tu_033";

const char D_00A4E390[8] = "Tu_032";

const char D_00A4E398[8] = "Tu_031";

const char D_00A4E3A0[8] = "Tu_030";

const char D_00A4E3A8[8] = "Tu_029";

const char D_00A4E3B0[8] = "Tu_028";

const char D_00A4E3B8[8] = "Tu_027";

const char D_00A4E3C0[8] = "Tu_026";

const char D_00A4E3C8[8] = "Tu_025";

const char D_00A4E3D0[8] = "Tu_024";

const char D_00A4E3D8[8] = "Tu_023";

const char D_00A4E3E0[8] = "Tu_022";

const char D_00A4E3E8[8] = "Tu_021";

const char D_00A4E3F0[8] = "Tu_020";

const char D_00A4E3F8[8] = "Tu_019";

const char D_00A4E400[8] = "Tu_018";

const char D_00A4E408[8] = "Tu_017";

const char D_00A4E410[8] = "Tu_016";

const char D_00A4E418[8] = "Tu_015";

const char D_00A4E420[8] = "Tu_014";

const char D_00A4E428[8] = "Tu_013";

const char D_00A4E430[8] = "Tu_012";

const char D_00A4E438[8] = "Tu_011";

const char D_00A4E440[8] = "Tu_010";

const char D_00A4E448[8] = "Tu_009";

const char D_00A4E450[8] = "Tu_008";

const char D_00A4E458[8] = "Tu_007";

const char D_00A4E460[8] = "Tu_006";

const char D_00A4E468[8] = "Tu_005";

const char D_00A4E470[8] = "Tu_004";

const char D_00A4E478[8] = "Tu_003";

const char D_00A4E480[8] = "Tu_002";

const char D_00A4E488[8] = "Tu_001";

const char D_00A4E490[8] = "Tu_000";
