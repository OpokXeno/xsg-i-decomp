#include "common.h"
#include "shared.h"

/*
 * JntHairDescriptor is the record JNT_hairID's argument points to at +0x28:
 * only the 16-bit hair ID at +8 is evidenced (main VA 0x002da9c0, lw
 * $3,40($4) then lhu $2,8($3)); the rest stays an unmodeled span
 * (docs/naming.md).
 */
typedef struct JntHairDescriptor {
    unsigned char unmodeled_00[8];
    u16 hairID;
} JntHairDescriptor;

typedef struct JntHairJoint {
    unsigned char unmodeled_00[0x28];
    JntHairDescriptor *hairDescriptor;
} JntHairJoint;

/*
 * FpkFcvHeader is the FPK-pack header getNumFCV reads: a magic word whose
 * low 3 bytes read little-endian as "FPK" (main VA 0x002db678, lui
 * $3,0xff / ori $3,$3,0xffff / lui $4,0x4b / ori $4,$4,0x5046 / and
 * $2,$2,$3), and the FCV count at +8, read only when the magic matches.
 * The word in between is not evidenced here and stays an unmodeled span
 * (docs/naming.md).
 */
typedef struct FpkFcvHeader {
    unsigned int magic;
    unsigned char unmodeled_04[4];
    int fcvCount;
} FpkFcvHeader;

/* "FPK" in the low three bytes of FpkFcvHeader.magic (little-endian). */
#define FPK_MAGIC 0x4B5046
#define FPK_MAGIC_MASK 0xFFFFFF

INCLUDE_ASM("asm/main/nonmatchings/hair_test", InitTest_002DA1D8);

extern int listpos;
extern int mot;
extern int pause;
extern char D_004CBBF0[];
extern char D_004CBC00[];
extern char D_004CBC10[];
extern char D_004CBC20[];
extern unsigned char list[];
extern void xglFontDebugPrintf(int x, int y, const char *format, ...);
static int getNumFCV(FpkFcvHeader *header);

/*
 * The object pAct_004DC638 points to: only the FCV header pointer at +0x724
 * is evidenced here (main VA 0x002da3f8, lw $2,pAct_004DC638 / lw
 * $4,0x724($2)); the rest stays an unmodeled span (docs/naming.md).
 */
typedef struct HairTestAct {
    unsigned char unmodeled_000[0x724];
    FpkFcvHeader *fcvHeader;
} HairTestAct;

extern HairTestAct *pAct_004DC638;

static void PrintDisp(void)
{
    xglFontDebugPrintf(0, 0, D_004CBBF0);
    xglFontDebugPrintf(0x64, 0xC, D_004CBC00, listpos, list + 4 + listpos * 0x10);
    xglFontDebugPrintf(0x5C, 0x14, D_004CBC10, mot, getNumFCV(pAct_004DC638->fcvHeader) - 1);
    if (pause != 0) {
        xglFontDebugPrintf(0x50, 0, D_004CBC20);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/hair_test", HairTest);

static u16 JNT_hairID(JntHairJoint *joint)
{
    return joint->hairDescriptor->hairID;
}

INCLUDE_ASM("asm/main/nonmatchings/hair_test", __JNT_computeMatrix);

static int getNumFCV(FpkFcvHeader *header)
{
    int count = 1;
    int result = 1;

    if (header != 0) {
        if ((header->magic & FPK_MAGIC_MASK) == FPK_MAGIC) {
            count = header->fcvCount;
        }
        result = count;
    }
    return result;
}
