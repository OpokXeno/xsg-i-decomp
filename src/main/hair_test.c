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

typedef union HairTestVectorBlock {
    Vector4 vector;
    u64 words[2];
} HairTestVectorBlock;

typedef struct HairResourceEntry {
    int resourceId;
    char name[12];
} HairResourceEntry;

typedef struct HairTestParticle {
    Vector4 position;
    Vector4 previousPosition;
    Vector4 velocity;
    float gravity;
    float damping;
} HairTestParticle;

struct HairTestAct;
typedef struct HairTestActInitial {
    u64 unmodeled_000[2];
    HairTestVectorBlock position;
    unsigned char unmodeled_020[0x30];
    HairTestVectorBlock rotation;
    HairTestVectorBlock scale;
    unsigned char unmodeled_070[0x6b4];
    FpkFcvHeader *fcvHeader;
    unsigned char unmodeled_728[0x68];
    unsigned char jointMatrix;
} HairTestActInitial;

static StudioCamera *pCamera_004DC634;
static struct HairTestAct *pAct_004DC638;
static int listpos;
static int listnum;
static int mot;
static int listnow;
static float lookY;
static float crx_004DC650;
static float cry_004DC654;
static int pause;
static HairTestParticle cpos_00585200;
const HairTestVectorBlock D_004CBBE0 = {
    .vector = { 1.0f, 1.0f, 1.0f, 1.0f }
};
const char D_004CBBF0[] = "\x0bHairTest";
const char D_004CBC00[] = "\x0bmodel:%3d/%s";
const char D_004CBC10[] = "\x0bmotion:%3d/%3d";
const char D_004CBC20[] = "\x0b* PUASE *";
static HairResourceEntry list[] = {
    { 1, "shion" }, { 9, "shion1" }, { 10, "shion2" },
    { 11, "shion3" }, { 12, "shion4" }, { 13, "shion5" },
    { 30, "shion_h" }, { 43, "shion1_h" }, { 45, "shion2_h" },
    { 46, "shion3_h" }, { 8, "shion_ch" }, { 2, "kosmos" },
    { 14, "kosmos1" }, { 15, "kosmos2" }, { 16, "kosmos3" },
    { 16, "kosmos5" }, { 31, "kosmos_h" }, { 37, "kosmos_h1" },
    { 38, "kosmos_h2" }, { 39, "kosmos_h3" }, { 40, "kosmos_h4" },
    { 41, "kosmos_h5" }, { 42, "kosmos_h6" }, { 3, "chaos" },
    { 18, "chaos1" }, { 32, "chaos_h" }, { 4, "momo" },
    { 20, "momo1" }, { 21, "momo2" }, { 22, "momo3" },
    { 23, "momo4" }, { 50, "momo5" }, { 33, "momo_h" },
    { 51, "momo5_h" }, { 5, "jr" }, { 24, "jr1" },
    { 25, "jr2" }, { 26, "jr3" }, { 34, "jr_h" },
    { 44, "jr1_h" }, { 47, "jr4" }, { 6, "ziggy" },
    { 27, "ziggy1" }, { 35, "ziggy_h" }, { 7, "shitan" },
    { 28, "shitan1" }, { 29, "shitan2" }, { 36, "shitan_h" },
    { 259, "gaignun" }, { 339, "gaignun_h" }, { 261, "albelt" },
    { 315, "albelt_h" }, { 329, "albelt2_h" }, { 316, "allen_h" },
    { 265, "kebin" }, { 266, "kebin1" }, { 267, "kebin2" },
    { 322, "kebin1_h" }, { 268, "virgil" }, { 273, "fried" },
    { 274, "elly" }, { 275, "matehws" }, { 276, "matehws1" },
    { 277, "tonny" }, { 278, "hammer" }, { 284, "feb" },
    { 285, "yuri" }, { 286, "joachim" }, { 313, "joachim1" },
    { 287, "sellers" }, { 288, "mary" }, { 289, "shelley" },
    { 290, "abel" }, { 291, "helmer" }, { 292, "voyager" },
    { 293, "miyuki" }, { 294, "lapis" }, { 295, "cecilia" },
    { 311, "cecilia1" }, { 296, "cath" }, { 312, "cath1" },
    { 297, "shi_dad" }, { 298, "shi_mam" }, { 299, "step_mam" },
    { 300, "pelegri" }, { 306, "and_wife" }, { 307, "and_daug" },
    { 1028, "hyaku" }, { 1029, "ass_m" }, { 1030, "ass_m1" },
    { 1031, "ass_m2" }, { 1039, "ass_m3" }, { 1040, "ass_m4" },
    { 1285, "utic_w" }, { 1597, "bread" }, { 1598, "bar_oji" },
    { 1606, "bread_ch" }, { 1607, "inn_girl" }, { 1567, "wman_a" },
    { 1573, "oba_a" }, { 1594, "girl_b" }, { 1595, "girl_b1" },
    { 523, "off_w" }, { 1603, "robot" }, { 1608, "mouth" },
    { 1605, "cle_oba" }, { 0, "" }
};

extern void GameResourceInit(int resourceType, int resourceGroup);
extern void ppInit(HairTestParticle *particle);
extern void ppSetPos(HairTestParticle *particle, float x, float y, float z);
extern void xglStudioInit(void);
extern void xglStudioChange(int studioIndex);
extern void xglStudioMainCameraInit(void);
extern void xglStudioGetCamera(StudioCamera **camera, int cameraIndex);
extern void ACT_init(void);
extern void ACT_resourceInit(void);
extern void ACT_initMotion(struct HairTestAct *actor);
extern void ACT_loadMotion(struct HairTestAct *actor, int resourceId, int category);
extern void ACT_loadResource(struct HairTestAct *actor, int resourceId);
extern void ACT_allocMatrix(struct HairTestAct *actor, int matrixCount);
extern void ACT_setModelWrapper(struct HairTestAct *actor, int flags);
extern struct HairTestAct *ACT_create(int actorIndex, int resourceId);
extern void __JNT_computeMatrix(void *joint, void *matrix);
extern void JNT_addConsumer(void *joint, int consumerIndex,
                            void (*computeMatrix)(void *, void *), int flags);

#define HAIR_RESOURCE(i) (list[(i)].resourceId)

static void InitTest(void)
{
    HairTestVectorBlock initialPosition;
    HairTestVectorBlock initialRotationAndScale[2];
    float cameraX;
    StudioCamera *camera;
    float zero = 0.0f;

    GameResourceInit(0x07000000, 0x06000000);
    cry_004DC654 = zero;
    lookY = 1.0f;
    crx_004DC650 = zero;
    ppInit(&cpos_00585200);
    ppSetPos(&cpos_00585200, 0.0f, 1.0f, 3.0f);
    cpos_00585200.gravity = zero;
    xglStudioInit();
    xglStudioChange(0);
    xglStudioMainCameraInit();
    xglStudioGetCamera(&pCamera_004DC634, 0);
    camera = pCamera_004DC634;
    cameraX = cpos_00585200.position.x;
    camera->position.x = cameraX;
    listpos = 0;
    listnum = 0;
    camera->position.y = cpos_00585200.position.y;
    listnow = 0;
    camera->position.z = cpos_00585200.position.z;
    if (HAIR_RESOURCE(0) != 0) {
        do {
            listnum++;
        } while (HAIR_RESOURCE(listnum) != 0);
    }
    ACT_init();
    ACT_resourceInit();
    pAct_004DC638 = ACT_create(0, HAIR_RESOURCE(listpos));
    ACT_initMotion(pAct_004DC638);
    ACT_loadMotion(pAct_004DC638, HAIR_RESOURCE(listpos), 1);
    ACT_loadResource(pAct_004DC638, HAIR_RESOURCE(listpos));
    ACT_allocMatrix(pAct_004DC638, -1);
    ACT_setModelWrapper(pAct_004DC638, 0);
    memset(&initialPosition, 0, sizeof(initialPosition));
    initialPosition.vector.w = 1.0f;
    memset(&initialRotationAndScale[0], 0, sizeof(initialRotationAndScale[0]));
    initialRotationAndScale[0].vector.w = 1.0f;
    initialRotationAndScale[1] = D_004CBBE0;
    ((HairTestActInitial *)pAct_004DC638)->position = initialPosition;
    ((HairTestActInitial *)pAct_004DC638)->rotation = initialRotationAndScale[0];
    ((HairTestActInitial *)pAct_004DC638)->scale = initialRotationAndScale[1];
    JNT_addConsumer(&((HairTestActInitial *)pAct_004DC638)->jointMatrix,
                    0, __JNT_computeMatrix, 0);
    mot = 0;
    pause = 0;
}

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

static void PrintDisp(void)
{
    xglFontDebugPrintf(0, 0, D_004CBBF0);
    xglFontDebugPrintf(0x64, 0xC, D_004CBC00, listpos, list[listpos].name);
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
