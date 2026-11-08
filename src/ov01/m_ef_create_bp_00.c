/*
 * OV01 original TU 24: 0x00a346c8..0x00a34e90 (5 functions)
 */
#include "common.h"
#include "ov01/m_ef_create.h"
#include "ov01/m_ef.h"
#include "shared.h"
#include "main/xgl_packet.h"
#include "m_ef_create_bp_00.h"
#include "main/m_ef_obj.h"

extern Vector4 *MMathCalcVectorMatrix(Vector4 *destination, Matrix4 *matrix);
extern void MGsGPInit(void *packet, void *address, int size);
extern float MMathMakeRandom2PI(void);
extern float MMathMakeRandom(void);
extern Vector4 *MMathDivVectorS(Vector4 *destination, const Vector4 *source, float scale);

/* destination.xyz = left.xyz + right.xyz on VU0 macro mode; w is kept. */
static __inline__ void bp00AddVectorXYZ(Vector4 *destination, const Vector4 *left, const Vector4 *right)
{
    __asm__ __volatile__(
        "lqc2 $vf1,0(%1)\n\t"
        "lqc2 $vf2,0(%2)\n\t"
        "vadd.xyz $vf1xyz,$vf1xyz,$vf2xyz\n\t"
        "sqc2 $vf1,0(%0)\n\t"
        :
        : "r"(destination), "r"(left), "r"(right)
        : "memory");
}

/* destination.xyz = left.xyz - right.xyz on VU0 macro mode. */
static __inline__ void bp00SubVectorXYZ(Vector4 *destination, const Vector4 *left, const Vector4 *right)
{
    __asm__ __volatile__(
        "lqc2 $vf1,0(%1)\n\t"
        "lqc2 $vf2,0(%2)\n\t"
        "vsub.xyz $vf1xyz,$vf1xyz,$vf2xyz\n\t"
        "sqc2 $vf1,0(%0)\n\t"
        :
        : "r"(destination), "r"(left), "r"(right)
        : "memory");
}

/* Quadword copy of a whole vector. */
static __inline__ void bp00CopyVector(Vector4 *destination, const Vector4 *source)
{
    __asm__ __volatile__(
        "lq $8,0(%1)\n\t"
        "sq $8,0(%0)\n\t"
        :
        : "r"(destination), "r"(source)
        : "$8", "memory");
}

/* cos/sin through the VU0 microsubroutines at 0xe8 and 0x20. */
static __inline__ float bp00Cos(float angle)
{
    float cosine;

    __asm__ __volatile__(
        "mfc1 $8,%1\n\t"
        "qmtc2.ni $8,$vf4\n\t"
        "vcallms 0xe8\n\t"
        "qmfc2.i $8,$vf1\n\t"
        "mtc1 $8,%0\n\t"
        : "=f"(cosine)
        : "f"(angle)
        : "$8", "memory");
    return cosine;
}

static __inline__ float bp00Sin(float angle)
{
    float sine;

    __asm__ __volatile__(
        "mfc1 $8,%1\n\t"
        "qmtc2.ni $8,$vf4\n\t"
        "vcallms 0x20\n\t"
        "qmfc2.i $8,$vf1\n\t"
        "mtc1 $8,%0\n\t"
        : "=f"(sine)
        : "f"(angle)
        : "$8", "memory");
    return sine;
}

static void fnBP00_PR000(void *self, BP00Work *work);
static void fnBP00_PR010(void *self, BP00Work *work);
static void fnBP00_DP000(void *self, BP00Work *work);
static void fnBP00_PO000(void *self, BP00Work *work);

/*
 * Initializes a BP00 effect: places it at the creation offset from the actor's matrix,
 * clears the trail and visibility flags, and sets the first strand's step back along
 * the actor's facing. Returns 1.
 */
int MEfCreate_BP00(MEfObjRecord *self)
{
    /* This constructor interprets its own variant of the pooled work. */
    BP00Object *object = (BP00Object *)self;
    BP00Work *work = &object->work;
    Vector4 direction;
    Matrix4 actorMatrix;

    work->frame = 0;
    work->counter = 0;
    MEfGetActorMatrix(&actorMatrix, work->actor, work->actorPart);
    MMathApplyMatrix(&work->position, &actorMatrix, &work->creationOffset);
    work->position.w = 1.0f;
    MEfGetActorCoord(&work->actorCoordinates, work->coordinateActor, MEF_POINT_ACTOR);
    memset(work->visibility, 0, sizeof(work->visibility));
    memset(work->trail, 0, sizeof(work->trail));
    work->visibility[0].flag[0] = 1;
    /* Quadword copy: the first trail point starts at the current position. */
    __asm__ __volatile__(
        "lq $8,0(%1)\n\t"
        "sq $8,0(%0)\n\t"
        :
        : "r"(work->trail[0].point), "r"(&work->position)
        : "$8", "memory");
    MMathCalcVectorMatrix(&direction, &actorMatrix);
    /* vectorOrigin[0].xyz = direction.xyz * -0.65, the first strand's
       per-frame step back along the actor's facing. */
    __asm__ __volatile__(
        "lqc2 $vf1,0(%1)\n\t"
        "mfc1 $8,%2\n\t"
        "qmtc2 $8,$vf2\n\t"
        "vmulx.xyz $vf1xyz,$vf1xyz,$vf2x\n\t"
        "sqc2 $vf1,0(%0)\n\t"
        :
        : "r"(&work->vectorOrigin[0]), "r"(&direction), "f"(-0.65f)
        : "$8", "memory");
    MGsGPInit(&work->packet, 0, 0);

    object->processCallback = fnBP00_PR000;
    object->drawCallback = fnBP00_DP000;
    object->postCallback = fnBP00_PO000;
    return 1;
}

/*
 * Per-frame update of the first strand: shifts its history, and extends it while the
 * counter is below 18. At counter 24 it restarts all five strands and switches the
 * object to fnBP00_PR010.
 */
static void fnBP00_PR000(void *self, BP00Work *work)
{
    BP00Object *object = self;
    Vector4 origin;
    Vector4 target;
    Vector4 step;
    float angle;
    float spread;
    int i;

    /* Shift the first strand's history one point back. */
    for (i = 5; i >= 0; i--) {
        work->visibility[0].flag[i + 1] = work->visibility[0].flag[i];
        work->trail[0].point[i + 1] = work->trail[0].point[i];
    }
    work->visibility[0].flag[0] = 0;
    if (work->counter < 18) {
        work->visibility[0].flag[0] = 1;
        bp00AddVectorXYZ(work->trail[0].point, work->trail[0].point, &work->vectorOrigin[0]);
    }
    if (work->counter >= 24) {
        work->counter = 0;
        bp00CopyVector(&origin, work->trail[0].point);
        for (i = 0; i < 5; i++) {
            Vector4 *head;

            work->visibility[i].flag[0] = 1;
            head = work->trail[i].point;
            bp00CopyVector(head, &origin);
            angle = MMathMakeRandom2PI();
            head->x += bp00Cos(angle);
            head->z += bp00Sin(angle);
            bp00CopyVector(&target, &work->actorCoordinates);
            spread = MMathMakeRandom() * 5.0f + 1.0f;
            target.x += spread * bp00Cos(angle);
            target.z += spread * bp00Sin(angle);
            bp00SubVectorXYZ(&step, &target, head);
            MMathDivVectorS(&work->vectorOrigin[i], &step, 9.0f);
        }
        object->processCallback = fnBP00_PR010;
    }
}

/*
 * Per-frame update of the strands after the restart: each started strand shifts its
 * history, and the strands are extended while the counter is below 16.
 */
static void fnBP00_PR010(void *self, BP00Work *work)
{
    int strand;
    int point;

    for (strand = 0; strand < 5; strand++) {
        if (strand < work->counter) {
            for (point = 5; point >= 0; point--) {
                work->visibility[strand].flag[point + 1] = work->visibility[strand].flag[point];
                work->trail[strand].point[point + 1] = work->trail[strand].point[point];
            }
            work->visibility[strand].flag[0] = 0;
        }
    }
    if (work->counter < 16) {
        for (strand = 0; strand < 5; strand++) {
            if (strand < work->counter) {
                work->visibility[strand].flag[0] = 1;
                bp00AddVectorXYZ(work->trail[strand].point, work->trail[strand].point, &work->vectorOrigin[strand]);
            }
        }
    }
}

extern void MGsGPTerm(void *packet);
extern int MMathRotTransPersClip(Vector4 *destination, const StudioCamera *camera,
                                 Matrix4 *matrix, const Vector4 *point);
extern MEfObjCamParams mefCamParams;
static const Quadword coltbl_0_00A51460[2];

#define BP00_STRANDS 5
#define BP00_POINTS 7

/* GIF tag fields: the register descriptor of a strip (RGBAQ, XYZ2). */
#define BP00_STRIP_PRIM 0x4A
#define BP00_STRIP_REGS 0x51

/* Writes the GIF tag that opens the packet's next primitive and returns
 * the first data quadword after it. */
static __inline__ MEfGsQword *bp00OpenTag(MEfGsPacket *packet, int pre, int prim, int regCount,
                                          unsigned long registers)
{
    MEfGsQword *qword = packet->current;

    qword->gif.tag = GIF_TAG_EOP | ((unsigned long)pre << 46) | ((unsigned long)prim << 47) |
                     ((unsigned long)regCount << 60);
    qword->gif.registers = registers;
    packet->reg_count = regCount;
    return qword + 1;
}

/* Writes one A+D register setting. */
static __inline__ void bp00SetRegister(MEfGsQword *qword, unsigned char address, unsigned long data)
{
    qword->ad.data = data;
    qword->ad.address = address;
}

/* Stores NLOOP into the open GIF tag and moves past its data. */
static __inline__ void bp00CloseTag(MEfGsPacket *packet, int nloop)
{
    int size;

    size = packet->reg_count * nloop + 1;
    packet->current->nloop |= nloop;
    packet->current += size;
    packet->count += size;
}

static __inline__ int bp00Floor(int value)
{
    return value < 0 ? 0 : value;
}

/*
 * Draws the strands into the effect's packet: one strip per strand, skipping invisible
 * points and fading the colour along the trail.
 */
static void fnBP00_DP000(void *self, BP00Work *work)
{
    MEfGsPacket *packet = &work->packet;
    StudioCamera *screen;
    Matrix4 *view;
    MEfGsQword vertex[2];
    MEfGsQword colour;
    MEfGsQword *setting;
    MEfGsQword *qword;
    short visible;
    int clip;
    int drawn;
    int strand;
    int point;
    int fade;
    int red;

    screen = mefCamParams.screen;
    view = mefCamParams.matrix;
    MGsGPInit(packet, 0, 0);
    /* GS state: alpha test, z buffer without depth write, additive alpha. */
    setting = packet->current;
    setting[0].gif.tag = MEF_GS_SETUP_TAG;
    setting[0].gif.registers = MEF_GS_SETUP_REGS;
    packet->reg_count = 1;
    qword = &setting[1];
    bp00SetRegister(qword, SCE_GS_TEST_1, MEF_GS_TEST_1_ALPHA_GREATER_ZGEQUAL);
    bp00SetRegister(&setting[2], SCE_GS_ZBUF_1, MEF_GS_ZBUF_1_PSMZ24_NO_DEPTH_WRITE);
    bp00SetRegister(&setting[3], SCE_GS_ALPHA_1, MEF_GS_ALPHA_1_ADDITIVE);
    bp00CloseTag(packet, 3);
    /* One strip per strand, holding its visible points. */
    for (strand = 0; strand < BP00_STRANDS; strand++) {
        drawn = 0;
        clip = 0;
        point = 0;
        colour.whole = *coltbl_0_00A51460;
        qword = bp00OpenTag(&work->packet, 1, BP00_STRIP_PRIM, 2, BP00_STRIP_REGS);
        for (; point < BP00_POINTS; point++) {
            visible = work->visibility[strand].flag[point];
            if (visible == 0) {
                continue;
            }
            fade = 48 - point * 24;
            colour.channel[0] = fade >= 0 ? fade : 0;
            fade = 96 - point * 20;
            colour.channel[1] = fade >= 0 ? fade : 0;
            fade = 255 - 16 * point;
            colour.channel[2] = fade >= 0 ? fade : 0;
            if (point == 0) {
                colour.channel[0] >>= 1;
                colour.channel[1] >>= 1;
                colour.channel[2] >>= 1;
            }
            if (MMathRotTransPersClip(&vertex[0].vector, screen, view, &work->trail[strand].point[point])) {
                clip = 1;
            } else if (clip != 0) {
                clip--;
                vertex[0].xyz.adc = MEF_GS_XYZ_ADC;
            }
            drawn++;
            qword[0].whole = colour.whole;
            qword[1].whole = vertex[0].whole;
            qword += 2;
            visible = 0;
            fade = 0;
            setting = 0;
            red = 0;
        }
        if (drawn > 0) {
            bp00CloseTag(packet, drawn);
        }
    }
    MGsGPTerm(packet);
}

/* MEfObjDestroy is defined by src/main/m_ef_obj.c; sefHitEffect
 * is defined by src/main/sef.c. */
extern void sefHitEffect(void);

#define BP00_HIT_FRAME 0x21
#define BP00_LIFETIME  0x2D

/*
 * The post callback advances frame (+0x70) and counter (+0x74) once per call, fires
 * sefHitEffect at BP00_HIT_FRAME and expires through MEfObjDestroy at BP00_LIFETIME.
 */
static void fnBP00_PO000(void *self, BP00Work *work)
{
    work->frame++;
    work->counter++;
    if (work->frame == BP00_HIT_FRAME) {
        sefHitEffect();
    }
    if (work->frame >= BP00_LIFETIME) {
        MEfObjDestroy(self);
    }
}



static const Quadword coltbl_0_00A51460[2] = {
    0xff000000ff0000002000000000,
    0xff0000008000000080000000ff
};
