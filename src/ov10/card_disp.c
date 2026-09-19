/*
 * OV10 original TU 6: 0x00a1e5d8..0x00a21348 (27 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov10/cgp.h"

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CHPTitleInitSub);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CHPMenuInitSub);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardHelpFileLoad);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CHPMenuBackDispSub);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardDispHelpCurrySet);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardDispHelpSlide1);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardDispHelpSlide2);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardDispHelpSlide3);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardDispCtr);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardDispField);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardDispDeckMake);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardDispH1P2P);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardDispTurn);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardDispFieldPointScale);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardDispTejyun);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardDispFieldPoint);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardDispGun);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardDispShineCurry);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardDispAttr);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardDispNazo);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardDispGuno);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardDispOnce);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardDispComm);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardDispPhase);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardDispHelpCard);

INCLUDE_ASM("asm/nonmatchings/ov10/card_disp", CardHelpProc);

/*
 * Clears the 256-byte help page-state buffer at +0x59c0..+0x5abf, then the
 * active-page byte at +0x59be. Both fall inside CardGameWork's unmodeled
 * +0x0002..+0x5af0 span (include/ov10/cgp.h, ov10/tu008); naming them there
 * needs that TU's own allocation, so this TU uses the named-offset fallback
 * instead of indexing the unmodeled byte array with raw literals.
 */
#define CGP_HELP_PAGE_ACTIVE(w)  (((u8 *)(w)) + 0x59BE)
#define CGP_HELP_PAGE_STATE(w)   (((u8 *)(w)) + 0x59C0)
#define CGP_HELP_PAGE_STATE_SIZE 256

void CardHelpPageInit(CardGameWork *work)
{
    int i;

    for (i = CGP_HELP_PAGE_STATE_SIZE - 1; i >= 0; i--) {
        CGP_HELP_PAGE_STATE(work)[i] = 0;
    }
    *CGP_HELP_PAGE_ACTIVE(work) = 0;
}
