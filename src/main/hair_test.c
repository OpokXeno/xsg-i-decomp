#include "common.h"
#include "shared.h"
#include "hair_test.h"
#include "main/jnt.h"

/* "FPK" in the low three bytes of FpkFcvHeader.magic (little-endian). */
#define FPK_MAGIC 0x4B5046

#define FPK_MAGIC_MASK 0xFFFFFF
#define HAIR_RESOURCE(i) (list[(i)].resourceId)
#define HAIR_TEST_PAD_SCALE (9.9999997473787516e-05f)

/*
 * JntHairDescriptor is the record JNT_hairID's argument points to at +0x28:
 * JNT_hairID establishes the 16-bit hair ID at +8 (main VA 0x002da9c0,
 * lw $3,40($4) then lhu $2,8($3)). __JNT_computeMatrix additionally reads
 * the channel counts at +6 and +10 through +14 with lhu; the leading
 * six bytes remain an unmodeled span (docs/naming.md).
 */
typedef struct JntHairDescriptor {
    unsigned char unmodeled_00[6];
    unsigned short channelCount;
    u16 hairID;
    unsigned short extraMotionCount;
    unsigned short secondaryCount;
    unsigned short staticCount;
} JntHairDescriptor;

typedef struct JointChannel {
    unsigned short type;
    unsigned char unmodeled_02[12];
    unsigned short matrixIndex;
    Vector4 translation;
    Vector4 rotation;
    unsigned char unmodeled_30[16];
} JointChannel;

struct Actor;

typedef struct JntHairJoint {
    void *producer;
    unsigned char unmodeled_04[12];
    unsigned int attributeKey;
    unsigned char unmodeled_14[20];
    JntHairDescriptor *hairDescriptor;
    int currentIndex;
    unsigned char unmodeled_30[8];
    unsigned int flags;
    unsigned char unmodeled_3c[4];
    Vector4 rootTranslation;
    unsigned char unmodeled_50[0x720 - 0x50];
    Matrix rootMatrix;
    unsigned char unmodeled_760[0x7a0 - 0x760];
    Matrix *matrixBuffer;
    unsigned char unmodeled_7a4[0x7f0 - 0x7a4];
    unsigned int previousAttributeKey;
    unsigned int attributeDifference;
    unsigned char unmodeled_7f8[4];
    float currentFrame;
    unsigned char unmodeled_800[16];
    int animationFlags;
    float clipRadius;
    unsigned char unmodeled_818[4];
    int channelStartIndex;
    struct Actor *owner;
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

/* HairTest copies the particle position into the camera as two doublewords
 * (ld/sd): the camera's position is viewed with the particle's aligned type. */
typedef struct HairTestCameraView {
    u8 unmodeled_00[0xd0];
    PpVector4 position;
} HairTestCameraView;

typedef struct HairTestAct {
    unsigned char unmodeled_000[0x724];
    FpkFcvHeader *fcvHeader;
} HairTestAct;

typedef struct Actor {
    unsigned char unmodeled_000[16];
    Vector4 position;
    unsigned char unmodeled_020[0x50 - 0x20];
    Vector4 rotation;
    unsigned char unmodeled_060[0x6f0 - 0x60];
    unsigned int motionFlags;
    unsigned char unmodeled_6f4[0x30];
    FpkFcvHeader *fcvHeader;
    unsigned char unmodeled_728[0xa40 - 0x728];
    unsigned char movedHairMode;
    unsigned char unmodeled_a41[3];
    float movedHairDelay;
    unsigned char unmodeled_a48[8];
    Vector4 movedHairSway;
} Actor;

typedef struct HairTestPad {
    u64 unmodeled_00[5];
    unsigned short held;
    unsigned short pressed;
    unsigned short repeat;
    unsigned char unmodeled_2e[0x38];
    signed char rightX;
    signed char rightY;
} HairTestPad;

extern HairTestPad PadData[2];

extern void GameResourceInit(int resourceType, int resourceGroup);
extern void ppInit(PpParticle *particle);
extern void ppSetPos(PpParticle *particle, float x, float y, float z);
extern void xglStudioInit(void);
extern void xglStudioChange(int studioIndex);
extern void xglStudioMainCameraInit(void);
extern void xglStudioGetCamera(StudioCamera **camera, int cameraIndex);
extern void ACT_init(void);
extern void ACT_resourceInit(void);
extern void ACT_initMotion(struct Actor *actor);
extern void ACT_loadMotion(struct Actor *actor, int resourceId, int category);
extern void ACT_loadResource(struct Actor *actor, int resourceId);
extern void ACT_allocMatrix(struct Actor *actor, int matrixCount);
extern void ACT_setModelWrapper(struct Actor *actor, int flags);
extern struct Actor *ACT_create(int actorIndex, int resourceId);
extern JointChannel *__JNT_computeMatrix(JntHairJoint *joint, JointChannel *matrix, int consumerArgument);
extern void xglFontDebugPrintf(int x, int y, const char *format, ...);
static int getNumFCV(FpkFcvHeader *header);
extern void xglRenderClearFrame(void);
extern void xglSleep(void);
extern void ppNextStart(PpParticle *particle);
extern void ppNextEnd(PpParticle *particle);
extern void ACT_setMotion(struct Actor *actor, int motion);
extern void ACT_updateMotion(struct Actor *actor);
extern void ACT_modelDraw(struct Actor *actor);
extern double cos(double angle);
extern double sin(double angle);
extern float atan2f(float y, float x);
extern JointChannel *JNT_getVal(JntHairJoint *, JointChannel *, float);
extern JointChannel *JNT_getVal_staticChain(JntHairJoint *, JointChannel *, float);
extern JointChannel *JNT_getStaticVal(JntHairJoint *, JointChannel *);
extern JointChannel *JNT_getStaticVal2(JntHairJoint *, JointChannel *);
extern JointChannel *JNT_getVal2(JntHairJoint *, JointChannel *, float);
extern JointChannel *JNT_constructMatrix(JntHairJoint *, JointChannel *);
extern JointChannel *JNT_interpolate(JntHairJoint *, JointChannel *);
extern JointChannel *EXM_CalcExMotion(JntHairJoint *, JointChannel *);
extern void EXM_PresetHair(JntHairJoint *);
extern void JNT_setInterrupt(JntHairJoint *, void *);
extern void MATRIX_convert4(Matrix *, Matrix *);
extern void xglMatrixUnit4s(Matrix *);
extern void xglMatrixMul(Matrix *, Matrix *, Matrix *);
extern float xglCos(float angle);
extern int nmlModelCalcClip(Vector4 *);

const HairTestVectorBlock hairTestScaleVector = {
    .vector = { 1.0f, 1.0f, 1.0f, 1.0f }
};

/* Text: {reset}HairTest */
const char hairTestLabel[] = "\x0bHairTest";

/* Text: {reset}model:%3d/%s */
const char hairTestModelFormat[] = "\x0bmodel:%3d/%s";

/* Text: {reset}motion:%3d/%3d */
const char hairTestMotionFormat[] = "\x0bmotion:%3d/%3d";

/* Text: {reset}* PUASE * */
const char hairTestPauseLabel[16] = "\x0b* PUASE *";

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

static StudioCamera *pCamera_004DC634;

static struct Actor *pAct_004DC638;

static int listpos;

static int listnum;

static int mot;

static int listnow;

static float lookY;

static float crx_004DC650;

static float cry_004DC654;

static int pause;

static PpParticle cpos_00585200;

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
    cameraX = cpos_00585200.position.vector.x;
    camera->position.x = cameraX;
    listpos = 0;
    listnum = 0;
    camera->position.y = cpos_00585200.position.vector.y;
    listnow = 0;
    camera->position.z = cpos_00585200.position.vector.z;
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
    initialRotationAndScale[1] = hairTestScaleVector;
    ((HairTestActInitial *)pAct_004DC638)->position = initialPosition;
    ((HairTestActInitial *)pAct_004DC638)->rotation = initialRotationAndScale[0];
    ((HairTestActInitial *)pAct_004DC638)->scale = initialRotationAndScale[1];
    JNT_addConsumer((JntProducer *)&((HairTestActInitial *)pAct_004DC638)->jointMatrix,
                    0, (JntProductionConsumer)__JNT_computeMatrix, 0);
    mot = 0;
    pause = 0;
}

static void PrintDisp(void)
{
    xglFontDebugPrintf(0, 0, hairTestLabel);
    xglFontDebugPrintf(0x64, 0xC, hairTestModelFormat, listpos, list[listpos].name);
    xglFontDebugPrintf(0x5C, 0x14, hairTestMotionFormat, mot, getNumFCV(pAct_004DC638->fcvHeader) - 1);
    if (pause != 0) {
        xglFontDebugPrintf(0x50, 0, hairTestPauseLabel);
    }
}

void HairTest(void)
{
    float moveX;
    float moveZ;
    float lookX;
    float lookZ;
    float lookHeight;
    double position;
    double horizontalMove;
    double verticalMove;
    double rotatedMove;
    int motionCount;

    xglRenderClearFrame();
    xglSleep();
    InitTest();
    if ((PadData[0].held & 0x100) == 0x100 &&
        (PadData[0].pressed & 0x800) == 0x800) {
        return;
    }
    do {
        ppNextStart(&cpos_00585200);
        if (PadData[0].held & 8) {
        cpos_00585200.position.vector.y += 0.01f;
        }
        if (PadData[0].held & 2) {
        cpos_00585200.position.vector.y -= 0.01f;
        }
        if (PadData[0].held & 1) {
            lookY -= 0.02f;
        }
        if (PadData[0].held & 4) {
            lookY += 0.02f;
        }
        moveX = PadData[0].rightX * HAIR_TEST_PAD_SCALE;
        moveZ = PadData[0].rightY * HAIR_TEST_PAD_SCALE;
        position = cpos_00585200.position.vector.x;
        horizontalMove = moveX;
        rotatedMove = horizontalMove * cos(-cry_004DC654);
        verticalMove = moveZ;
        cpos_00585200.position.vector.x = position +
            (rotatedMove - verticalMove * sin(-cry_004DC654));
        position = cpos_00585200.position.vector.z;
        rotatedMove = verticalMove * cos(-cry_004DC654);
        cpos_00585200.position.vector.z = position +
            (rotatedMove + horizontalMove * sin(-cry_004DC654));
        ppNextEnd(&cpos_00585200);
        lookX = -cpos_00585200.position.vector.x;
        lookZ = -cpos_00585200.position.vector.z;
        lookHeight = lookY - cpos_00585200.position.vector.y;
        cry_004DC654 = -atan2f(lookX, cpos_00585200.position.vector.z);
        crx_004DC650 = atan2f(
            lookHeight, __builtin_sqrtf(lookX * lookX + lookZ * lookZ));
        ((HairTestCameraView *)pCamera_004DC634)->position = cpos_00585200.position;
        pCamera_004DC634->rotation.x = crx_004DC650;
        pCamera_004DC634->rotation.y = cry_004DC654;
        pCamera_004DC634->rotation.z = 0.0f;
        /* Compiler-forced goto: the original enters the update block from the
         * unpause branch. Structured forms differ: a flag compare is 16 bytes
         * short, a re-test of pause after the toggle 16 bytes long, and a
         * conditional-expression test 36 bytes long. */
        if (pause != 0) {
            if (PadData[0].pressed & 0x800) {
                pause = 0;
                goto unpaused;
            }
        } else if (PadData[0].pressed & 0x800) {
            pause = 1;
        } else {
unpaused:
            if (PadData[0].repeat & 0x4000) {
                listpos++;
                if (list[listpos].resourceId == 0) {
                    listpos = 0;
                }
            } else if (PadData[0].repeat & 0x1000) {
                listpos--;
                if (listpos < 0) {
                    listpos = listnum - 1;
                }
            }
            if (!(PadData[0].held & 0x80) && listnow != listpos) {
                GameResourceInit(0x07000000, 0x06000000);
                ACT_init();
                pAct_004DC638 = ACT_create(0, list[listpos].resourceId);
                ACT_loadResource(pAct_004DC638, list[listpos].resourceId);
                ACT_loadMotion(pAct_004DC638, list[listpos].resourceId, 1);
                listnow = listpos;
            }
            if (PadData[0].repeat & 0x2000) {
                mot++;
            }
            if (PadData[0].repeat & 0x8000) {
                mot--;
            }
            motionCount = getNumFCV(pAct_004DC638->fcvHeader);
            while (mot >= motionCount) {
                mot -= motionCount;
            }
            while (mot < 0) {
                mot += motionCount;
            }
            pAct_004DC638->motionFlags |= 8;
            ACT_setMotion(pAct_004DC638, mot);
            ACT_updateMotion(pAct_004DC638);
        }
        ACT_modelDraw(pAct_004DC638);
        PrintDisp();
        xglSleep();
    } while ((PadData[0].held & 0x100) != 0x100 ||
             (PadData[0].pressed & 0x800) != 0x800);
}

static u16 JNT_hairID(JntHairJoint *joint)
{
    return joint->hairDescriptor->hairID;
}

JointChannel *__JNT_computeMatrix(JntHairJoint *joint, JointChannel *first, int consumerArgument)
{
    Matrix savedRoot;
    Matrix rootBasis;
    Matrix clipJoint;
    Matrix clipMatrix;
    Vector4 clipSphere;
    Matrix composedMatrix;
    Matrix convertedJoint;
    Matrix chainRootBasis;
    Matrix chainClipJoint;
    Vector4 chainClipSphere;
    Matrix chainConvertedJoint;
    float movementLength;
    Matrix *rootMatrix;
    Matrix *rootDestination;
    JointChannel *cursor;
    int limit;
    int index;
    int matrixIndex;
    Matrix *matrixBase;
    int transformTail;
    int animationState;
    float frame;

    if (first == 0) {
        return 0;
    }
    if ((joint->flags & 0x20) != 0) {
        return 0;
    }
    cursor = first;
    joint->flags |= 0x18;
    rootMatrix = &joint->rootMatrix;
    frame = joint->currentFrame;
    {
        Matrix *rootSource = joint->matrixBuffer;
        __asm__ __volatile__(
            "lq $2,0(%1)\n\t"
            "sq $2,0(%0)\n\t"
            "lq $2,16(%1)\n\t"
            "sq $2,16(%0)\n\t"
            "lq $2,32(%1)\n\t"
            "sq $2,32(%0)\n\t"
            "lq $2,48(%1)\n\t"
            "sq $2,48(%0)"
            :
            : "r"(rootMatrix), "r"(rootSource)
            : "$2", "memory"
        );
    }
    {
        matrixBase = joint->matrixBuffer;
        __asm__ __volatile__(
            "lq $2,0(%1)\n\t"
            "sq $2,0(%0)\n\t"
            "lq $2,16(%1)\n\t"
            "sq $2,16(%0)\n\t"
            "lq $2,32(%1)\n\t"
            "sq $2,32(%0)\n\t"
            "lq $2,48(%1)\n\t"
            "sq $2,48(%0)"
            :
            : "r"(&savedRoot), "r"(matrixBase)
            : "$2", "memory"
        );
    }
    xglMatrixUnit4s(joint->matrixBuffer);

    if (joint->owner != 0 && joint->owner->movedHairMode != 0) {
        struct Actor *owner = joint->owner;
        float movementFacing;
        owner->movedHairSway.x -= owner->position.x;
        owner->movedHairSway.y -= owner->position.y;
        owner->movedHairSway.z -= owner->position.z;
        xglVectorLength(&movementLength, &owner->movedHairSway);
        movementFacing = -xglCos(owner->rotation.y -
                         atan2f(owner->movedHairSway.x, owner->movedHairSway.z));
        if (movementFacing < 0.0f) {
            movementFacing = 0.0f;
        }
        owner->movedHairSway.w = movementLength * movementFacing *
                               owner->movedHairDelay * 0.3f +
                               owner->movedHairSway.w * 0.7f;
    }

    joint->channelStartIndex = -1;
    joint->attributeDifference = joint->attributeKey - joint->previousAttributeKey;
    if (joint->attributeDifference == 0) {
        limit = JNT_hairID(joint);
        while (joint->currentIndex < limit) {
            cursor = JNT_getVal(joint, cursor, frame);
            if ((joint->flags & 4) != 0) {
                break;
            }
            if (joint->channelStartIndex >= 0) {
                break;
            }
        }
        if ((joint->flags & 4) != 0) {
            while (joint->currentIndex < limit) {
                cursor = JNT_getStaticVal(joint, cursor);
            }
        } else if (joint->channelStartIndex >= 0) {
            Matrix *clipTarget;
            limit = joint->currentIndex;
            joint->currentIndex = 1;
            cursor = first + 1;
            while (joint->currentIndex < limit) {
                cursor = JNT_constructMatrix(joint, cursor);
            }
            clipTarget = &joint->matrixBuffer[joint->channelStartIndex];
            clipSphere.x = clipSphere.y = clipSphere.z = 0.0f;
            if (joint->clipRadius <= 0.0f) {
                clipSphere.w = 2.0f;
            } else {
                clipSphere.w = joint->clipRadius;
            }
            MATRIX_convert4(&rootBasis, rootMatrix);
            rootBasis.elements[12] = 0.0f;
            rootBasis.elements[13] = 0.0f;
            rootBasis.elements[14] = 0.0f;
            MATRIX_convert4(&clipJoint, clipTarget);
            xglMatrixMul(&clipMatrix, &rootBasis, &clipJoint);
            joint->rootTranslation.x = clipMatrix.elements[12];
            joint->rootTranslation.y = clipMatrix.elements[13];
            joint->rootTranslation.z = clipMatrix.elements[14];
            clipMatrix.elements[12] += joint->rootMatrix.elements[12];
            clipMatrix.elements[13] += joint->rootMatrix.elements[13];
            clipMatrix.elements[14] += joint->rootMatrix.elements[14];
            clipSphere.x = clipMatrix.elements[12];
            clipSphere.y = clipMatrix.elements[13];
            clipSphere.z = clipMatrix.elements[14];
            {
                int clipped = nmlModelCalcClip(&clipSphere);
                if (clipped || (joint->flags & 0x200)) {
                    if ((joint->flags & 0x100) == 0) {
                        joint->flags |= 0x40;
                        if ((joint->animationFlags & 0x10000000) || (joint->flags & 0x200)) {
                            joint->flags |= 4;
                            return 0;
                        }
                    }
                }
            }
            limit = JNT_hairID(joint);
            while (joint->currentIndex < limit) {
                cursor = JNT_getVal(joint, cursor, frame);
                if ((joint->flags & 4) != 0) {
                    break;
                }
            }
            if ((joint->flags & 4) != 0) {
                while (joint->currentIndex < limit) {
                    cursor = JNT_getStaticVal(joint, cursor);
                }
            }
        }
        JNT_setInterrupt(joint, joint->producer);
        limit = joint->currentIndex;
        joint->currentIndex = joint->channelStartIndex + 1;
        cursor = first + joint->channelStartIndex + 1;
        while (joint->currentIndex < limit) {
            cursor = JNT_constructMatrix(joint, cursor);
        }
        {
            Matrix *rootRestore = joint->matrixBuffer;
            __asm__ __volatile__(
                "lq $2,0(%1)\n\t"
                "sq $2,0(%0)\n\t"
                "lq $2,16(%1)\n\t"
                "sq $2,16(%0)\n\t"
                "lq $2,32(%1)\n\t"
                "sq $2,32(%0)\n\t"
                "lq $2,48(%1)\n\t"
                "sq $2,48(%0)"
                :
                : "r"(rootRestore), "r"(&savedRoot)
                : "$2", "memory"
            );
        }
        animationState = joint->animationFlags;
        if (animationState < 0) {
            limit = joint->hairDescriptor->hairID + joint->hairDescriptor->extraMotionCount;
            while (joint->currentIndex < limit) {
                cursor = JNT_getVal2(joint, cursor, frame);
                if ((joint->flags & 4) != 0) {
                    break;
                }
            }
            if ((joint->flags & 2) != 0) {
                cursor = first;
                cursor++;
                joint->currentIndex = 1;
                while (joint->currentIndex < limit) {
                    cursor = JNT_interpolate(joint, cursor);
                }
            }
        } else {
            if ((joint->flags & 2) != 0) {
                cursor = first + 1;
                joint->currentIndex = 1;
                while (joint->currentIndex < limit) {
                    cursor = JNT_interpolate(joint, cursor);
                }
                index = joint->currentIndex;
                limit = joint->hairDescriptor->hairID + joint->hairDescriptor->extraMotionCount;
                first = cursor;
                while (joint->currentIndex < limit) {
                    cursor = JNT_interpolate(joint, cursor);
                }
                joint->currentIndex = index;
                cursor = first;
            }
            EXM_PresetHair(joint);
            limit = joint->hairDescriptor->hairID + joint->hairDescriptor->extraMotionCount;
            while (joint->currentIndex < limit) {
                cursor = EXM_CalcExMotion(joint, cursor);
            }
        }
        limit = joint->hairDescriptor->hairID;
        limit += joint->hairDescriptor->extraMotionCount;
        limit += joint->hairDescriptor->secondaryCount;
        if ((joint->flags & 4) != 0) {
            while (joint->currentIndex < limit) {
                cursor = JNT_getStaticVal2(joint, cursor);
            }
        } else {
            while (joint->currentIndex < limit) {
                cursor = JNT_getVal2(joint, cursor, frame);
                if ((joint->flags & 4) != 0) {
                    break;
                }
            }
            if ((joint->flags & 4) != 0) {
                joint->flags &= ~4;
                while (joint->currentIndex < limit) {
                    cursor = JNT_getStaticVal2(joint, cursor);
                }
            }
        }
        index = joint->currentIndex;
        limit = joint->hairDescriptor->channelCount;
        transformTail = index < limit;
        while (joint->currentIndex < limit) {
            cursor = JNT_getStaticVal2(joint, cursor);
        }
        joint->currentIndex = index;
        MATRIX_convert4(&savedRoot, &savedRoot);
        joint->currentIndex = index;
        if (transformTail) {
            Matrix *convertedSource = &convertedJoint;
            Matrix *composedSource = &composedMatrix;
            do {
                MATRIX_convert4(convertedSource, &joint->matrixBuffer[joint->currentIndex]);
                xglMatrixMul(composedSource, &savedRoot, convertedSource);
                {
                    matrixIndex = joint->currentIndex;
                    matrixBase = joint->matrixBuffer;
                    __asm__ __volatile__(
                        "lq $2,0(%0)\n\t"
                        "sq $2,0(%1)\n\t"
                        "lq $2,16(%0)\n\t"
                        "sq $2,16(%1)\n\t"
                        "lq $2,32(%0)\n\t"
                        "sq $2,32(%1)\n\t"
                        "lq $2,48(%0)\n\t"
                        "sq $2,48(%1)"
                        :
                        : "r"(composedSource), "r"((Matrix *)((((unsigned int)matrixIndex) << 6) + (unsigned int)matrixBase))
                        : "$2", "memory"
                    );
                }
                index = joint->currentIndex + 1;
                joint->currentIndex = index;
            } while (index < limit);
        }
    } else {
        limit = JNT_hairID(joint);
        while (joint->currentIndex < limit) {
            cursor = JNT_getVal_staticChain(joint, cursor, frame);
            if ((joint->flags & 4) != 0) {
                break;
            }
            if (joint->channelStartIndex >= 0) {
                break;
            }
        }
        if ((joint->flags & 4) != 0) {
            while (joint->currentIndex < limit) {
                cursor = JNT_getStaticVal(joint, cursor);
            }
        } else if (joint->channelStartIndex >= 0) {
            Matrix *clipTarget;
            limit = joint->currentIndex;
            cursor = first;
            joint->currentIndex = 0;
            while (joint->currentIndex < limit) {
                cursor = JNT_constructMatrix(joint, cursor);
            }
            clipTarget = &joint->matrixBuffer[joint->channelStartIndex];
            chainClipSphere.x = chainClipSphere.y = chainClipSphere.z = 0.0f;
            if (joint->clipRadius <= 0.0f) {
                chainClipSphere.w = 2.0f;
            } else {
                chainClipSphere.w = joint->clipRadius;
            }
            MATRIX_convert4(&chainRootBasis, rootMatrix);
            chainRootBasis.elements[12] = 0.0f;
            chainRootBasis.elements[13] = 0.0f;
            chainRootBasis.elements[14] = 0.0f;
            MATRIX_convert4(&chainClipJoint, clipTarget);
            xglMatrixMul(&convertedJoint, &chainRootBasis, &chainClipJoint);
            joint->rootTranslation.x = convertedJoint.elements[12];
            joint->rootTranslation.y = convertedJoint.elements[13];
            joint->rootTranslation.z = convertedJoint.elements[14];
            convertedJoint.elements[12] += joint->rootMatrix.elements[12];
            convertedJoint.elements[13] += joint->rootMatrix.elements[13];
            convertedJoint.elements[14] += joint->rootMatrix.elements[14];
            chainClipSphere.x = convertedJoint.elements[12];
            chainClipSphere.y = convertedJoint.elements[13];
            chainClipSphere.z = convertedJoint.elements[14];
            if (nmlModelCalcClip(&chainClipSphere)) {
                joint->flags |= 0x40;
                if ((joint->animationFlags & 0x10000000) || (joint->flags & 0x200)) {
                    joint->flags |= 4;
                    return 0;
                }
            }
            limit = JNT_hairID(joint);
            while (joint->currentIndex < limit) {
                cursor = JNT_getVal_staticChain(joint, cursor, frame);
                if ((joint->flags & 4) != 0) {
                    break;
                }
            }
            if ((joint->flags & 4) != 0) {
                while (joint->currentIndex < limit) {
                    cursor = JNT_getStaticVal(joint, cursor);
                }
            }
        }
        JNT_setInterrupt(joint, joint->producer);
        limit = joint->currentIndex;
        joint->currentIndex = joint->channelStartIndex + 1;
        cursor = first + joint->channelStartIndex + 1;
        while (joint->currentIndex < limit) {
            cursor = JNT_constructMatrix(joint, cursor);
        }
        if ((joint->flags & 2) != 0) {
            cursor = first + 1;
            joint->currentIndex = 1;
            while (joint->currentIndex < limit) {
                cursor = JNT_interpolate(joint, cursor);
            }
            index = joint->currentIndex;
            limit = joint->hairDescriptor->hairID + joint->hairDescriptor->extraMotionCount;
            first = cursor;
            while (joint->currentIndex < limit) {
                cursor = JNT_interpolate(joint, cursor);
            }
            joint->currentIndex = index;
            cursor = first;
        }
        EXM_PresetHair(joint);
        limit = joint->hairDescriptor->hairID + joint->hairDescriptor->extraMotionCount;
        while (joint->currentIndex < limit) {
            cursor = EXM_CalcExMotion(joint, cursor);
        }
        limit += joint->hairDescriptor->secondaryCount;
        while (joint->currentIndex < limit) {
            cursor = JNT_getStaticVal2(joint, cursor);
        }
        index = joint->currentIndex;
        limit += joint->hairDescriptor->staticCount;
        transformTail = index < limit;
        if (transformTail) {
            while (joint->currentIndex < limit) {
                cursor = JNT_getStaticVal2(joint, cursor);
            }
            rootDestination = joint->matrixBuffer;
        } else {
            rootDestination = joint->matrixBuffer;
        }
        {
            __asm__ __volatile__(
                "lq $2,0(%1)\n\t"
                "sq $2,0(%0)\n\t"
                "lq $2,16(%1)\n\t"
                "sq $2,16(%0)\n\t"
                "lq $2,32(%1)\n\t"
                "sq $2,32(%0)\n\t"
                "lq $2,48(%1)\n\t"
                "sq $2,48(%0)"
                :
                : "r"(rootDestination), "r"(&savedRoot)
                : "$2", "memory"
            );
        }
        MATRIX_convert4(&savedRoot, &savedRoot);
        joint->currentIndex = index;
        if (transformTail) {
            Matrix *convertedSource = &chainConvertedJoint;
            Matrix *composedSource = &composedMatrix;
            do {
                MATRIX_convert4(convertedSource, &joint->matrixBuffer[joint->currentIndex]);
                xglMatrixMul(composedSource, &savedRoot, convertedSource);
                {
                    matrixIndex = joint->currentIndex;
                    matrixBase = joint->matrixBuffer;
                    __asm__ __volatile__(
                        "lq $2,0(%0)\n\t"
                        "sq $2,0(%1)\n\t"
                        "lq $2,16(%0)\n\t"
                        "sq $2,16(%1)\n\t"
                        "lq $2,32(%0)\n\t"
                        "sq $2,32(%1)\n\t"
                        "lq $2,48(%0)\n\t"
                        "sq $2,48(%1)"
                        :
                        : "r"(composedSource), "r"((Matrix *)((((unsigned int)matrixIndex) << 6) + (unsigned int)matrixBase))
                        : "$2", "memory"
                    );
                }
                index = joint->currentIndex + 1;
                joint->currentIndex = index;
            } while (index < limit);
        }
    }
    if (joint->owner != 0 && joint->owner->movedHairMode != 0) {
        joint->owner->movedHairSway.x = joint->owner->position.x;
        joint->owner->movedHairSway.y = joint->owner->position.y;
        joint->owner->movedHairSway.z = joint->owner->position.z;
    }
    return cursor;
}

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
