/*
 * OV01 original TU 20: 0x00a332e0..0x00a33648 (6 functions)
 */
#include "common.h"

/*
 * .bss for this TU is still scaffold-owned: the flag word keeps its ledger
 * name (config/symbols/ov01.txt) and the MPEG context its splat name.
 */
extern int mvFlags_00A43724;
extern unsigned char D_00A5B838[0xC0];

#define MV2_FLAG_INITIALIZED 0x1
#define MV2_FLAG_PLAYING 0x2
#define MV2_FLAG_SKIP_ENABLED 0x4
#define MV2_PLAYING_BIT 1

extern void xglRenderCopyDisp2Draw(void);
extern int xglMpeg2Close(unsigned char *mpegContext);

int MMv2Init(void)
{
    mvFlags_00A43724 = MV2_FLAG_INITIALIZED | MV2_FLAG_SKIP_ENABLED;
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_mv2", MMv2Exec);

INCLUDE_ASM("asm/nonmatchings/ov01/m_mv2", MMv2Play);

void MMv2Stop(void)
{
    unsigned char *mpegContext = D_00A5B838;

    if (mvFlags_00A43724 & MV2_FLAG_PLAYING) {
        xglRenderCopyDisp2Draw();
        xglMpeg2Close(mpegContext);
        mvFlags_00A43724 &= ~MV2_FLAG_PLAYING;
    }
}

int MMv2IsPlaying(void)
{
    return (mvFlags_00A43724 >> MV2_PLAYING_BIT) & 1;
}

void MMv2SkipEnabled(short enable)
{
    if (enable != 0)
        mvFlags_00A43724 |= MV2_FLAG_SKIP_ENABLED;
    else
        mvFlags_00A43724 &= ~MV2_FLAG_SKIP_ENABLED;
}
