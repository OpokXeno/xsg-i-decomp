/*
 * OV12 original TU 63: 0x00a32688..0x00a32c48 (11 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_linkdata.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern unsigned int strlen(const char *string);
extern int strncmp(const char *string1, const char *string2,
                   unsigned int count);

void InitRgLinkData(RgLinkData *pAna, void *pBuf) {
    if (pAna == 0) {
        assert_prog(D_00A556C0, D_00A556D0, 18);
    }
    if (pBuf == 0) {
        assert_prog(D_00A556E8, D_00A556D0, 19);
    }
    pAna->buf = pBuf;
}

static unsigned int _get_uint(const unsigned char *buf) {
    return buf[0] + (buf[1] << 8) + (buf[2] << 16) + (buf[3] << 24);
}

int RgLinkDataMatchDataName(RgLinkData *pAna, const char *name) {
    char *dataName = pAna->buf;

    if (strlen(name) != 4) {
        return 0;
    }
    return strncmp(dataName, name, 4) == 0;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_linkdata", RgLinkDataVersion);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_linkdata", RgLinkDataTime);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_linkdata", RgLinkDataNumOfData);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_linkdata", RgLinkDataGetName);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_linkdata", RgLinkDataGetSize);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_linkdata", RgLinkDataGet);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_linkdata", RgLinkDataFindExt);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_linkdata", RgLinkDataGetIndex);
