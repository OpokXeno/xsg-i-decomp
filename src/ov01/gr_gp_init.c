/*
 * OV01 original TU 8: 0x00a24a98..0x00a26018 (30 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_packet.h"

typedef unsigned char u8;
typedef struct Matrix {
    float elements[16];
} Matrix;
typedef struct FloatVector4 {
    float x;
    float y;
    float z;
    float w;
} FloatVector4;
typedef struct FloatVector3 {
    float x;
    float y;
    float z;
} FloatVector3;

/* grGpInit's argument: the VIF1 DIRECT packet grPacketSend transmits, a
 * data buffer and its length. grGpInit points data at the scratchpad
 * (0x70000000). */
typedef struct GrPacket {
    void *data;  /* +0x0 */
    int count;   /* +0x4 */
} GrPacket;

/* Framebuffer base addresses; grFBAdrGet indexes this table. */
extern int fb[4];

/* EE scratchpad RAM base. */
#define SCRATCHPAD_BASE ((void *)0x70000000)
/* VIF FLUSH code (command 0x11 in bits 24..30, no immediate). */
#define VIF_CODE_FLUSH 0x11000000

extern void sceVif1PkCnt(XglPacket *packet, int count);
extern void sceVif1PkAddCode(XglPacket *packet, unsigned int code);
extern void sceVif1PkOpenDirectHLCode(XglPacket *packet, int mode);
extern void sceVif1PkAddDirectDataN(XglPacket *packet, const void *data,
                                    int count);
extern void sceVif1PkCloseDirectHLCode(XglPacket *packet);
extern void sceVif1PkTerminate(XglPacket *packet);

/* The record grCalcMatrix transforms. Only the four vectors of its transform
 * block are recovered; everything before +0x90 stays unnamed.
 *
 * Evidence for the block and its 0x10-byte stride: curPutSub (ov01 0x00a10904
 * and 0x00a10a30) copies record+0x90..record+0x110 out to its own frame as one
 * 0x80-byte unit, overrides single lanes inside it, calls grCalcMatrix and then
 * copies the unit back. The lanes it overrides say what the vectors are:
 * +0x94 = 0 and +0xa4 = 0.01 (the cursor sits on the ground and is lifted by
 * the second term), +0xb4 = the frame-derived angle in radians, and
 * +0xc0/+0xc4/+0xc8 = 1.0. grCalcMatrix itself reads +0x90 and +0xa0 as the two
 * terms of the translation, hands +0xb0 to grRotMatrix as Y/X/Z Euler angles
 * and +0xc0 to xglMatrixScale. The last 0x40 bytes of the saved unit
 * (+0xd0..+0x110) are not recovered and get no members here.
 *
 * The unnamed words below carry the distance to +0x90 and nothing else: they
 * are unnamed bit-fields, not fields, and claim no content, no width and no
 * boundary inside the span they cover.
 */
typedef struct TransformState {
    unsigned int : 32;                 /* +0x00 */
    unsigned int : 32;                 /* +0x04 */
    unsigned int : 32;                 /* +0x08 */
    unsigned int : 32;                 /* +0x0c */
    unsigned int : 32;                 /* +0x10 */
    unsigned int : 32;                 /* +0x14 */
    unsigned int : 32;                 /* +0x18 */
    unsigned int : 32;                 /* +0x1c */
    unsigned int : 32;                 /* +0x20 */
    unsigned int : 32;                 /* +0x24 */
    unsigned int : 32;                 /* +0x28 */
    unsigned int : 32;                 /* +0x2c */
    unsigned int : 32;                 /* +0x30 */
    unsigned int : 32;                 /* +0x34 */
    unsigned int : 32;                 /* +0x38 */
    unsigned int : 32;                 /* +0x3c */
    unsigned int : 32;                 /* +0x40 */
    unsigned int : 32;                 /* +0x44 */
    unsigned int : 32;                 /* +0x48 */
    unsigned int : 32;                 /* +0x4c */
    unsigned int : 32;                 /* +0x50 */
    unsigned int : 32;                 /* +0x54 */
    unsigned int : 32;                 /* +0x58 */
    unsigned int : 32;                 /* +0x5c */
    unsigned int : 32;                 /* +0x60 */
    unsigned int : 32;                 /* +0x64 */
    unsigned int : 32;                 /* +0x68 */
    unsigned int : 32;                 /* +0x6c */
    unsigned int : 32;                 /* +0x70 */
    unsigned int : 32;                 /* +0x74 */
    unsigned int : 32;                 /* +0x78 */
    unsigned int : 32;                 /* +0x7c */
    unsigned int : 32;                 /* +0x80 */
    unsigned int : 32;                 /* +0x84 */
    unsigned int : 32;                 /* +0x88 */
    unsigned int : 32;                 /* +0x8c */
    FloatVector3 position;             /* +0x90 */
    unsigned int : 32;                 /* +0x9c */
    FloatVector3 offset;               /* +0xa0: added to position */
    unsigned int : 32;                 /* +0xac */
    FloatVector3 rotation;             /* +0xb0: Y, then X, then Z */
    unsigned int : 32;                 /* +0xbc */
    FloatVector3 scale;                /* +0xc0 */
} TransformState;

/* Only the transform record the command points at is recovered (lw v0,0x10(a0)
 * at 0x00a25b3c); grBBCalc and curPutSub pass the record through untouched. */
typedef struct MatrixCommand {
    unsigned int : 32;                 /* +0x00 */
    unsigned int : 32;                 /* +0x04 */
    unsigned int : 32;                 /* +0x08 */
    unsigned int : 32;                 /* +0x0c */
    TransformState *state;             /* +0x10 */
} MatrixCommand;
extern void xglStudioGetCamera(void *camera_out, int camera_index);
extern void xglMatrixUnit(float matrix[4][4]);
extern void xglMatrixTrans(Matrix *destination, Matrix *source,
                           FloatVector4 *translation);
extern void xglMatrixScale(Matrix *destination, Matrix *source,
                           FloatVector4 *scale);
extern void xglMatrixMul(Matrix *destination, Matrix *left, Matrix *right);
extern void xglMatrixStackLoad(Matrix *source);
extern void xglMatrixStackRotY(float angle);
extern void xglMatrixStackRotX(float angle);
extern void xglMatrixStackRotZ(float angle);
extern void xglMatrixStackSave(float matrix[4][4]);
extern void grRotMatrix(Matrix *destination, Matrix *source,
                        FloatVector3 *angles);

void grGpInit(GrPacket *packet)
{
    packet->count = 0;
    packet->data = SCRATCHPAD_BASE;
}

int grFBAdrGet(int index)
{
    return fb[index];
}

INCLUDE_ASM("asm/nonmatchings/ov01/gr_gp_init", grBBIdxGet);

INCLUDE_ASM("asm/nonmatchings/ov01/gr_gp_init", grDBIdxGet);

INCLUDE_ASM("asm/nonmatchings/ov01/gr_gp_init", grVramCopyFBtoFB);

INCLUDE_ASM("asm/nonmatchings/ov01/gr_gp_init", grVramCopy);

INCLUDE_ASM("asm/nonmatchings/ov01/gr_gp_init", grOpenF2);

/*
 * grOpenF2/grOpenF4/grOpenFT4/grOpenFT4STQ/grOpenSpr (siblings, not decoded
 * here) each save the caller's packet into their own GpSave static and zero
 * their Num static; grPutXxx then advances GpSave.count as it appends
 * primitives. The quadword at GpSave.data[GpSave.count] is left for the
 * trailing GIF tag: grCloseXxx patches its low 8 bytes with the final Num
 * (the accumulated NLOOP) so the tag matches how many primitives were put.
 */
typedef struct GrGifTag {
    long long control;             /* +0x00: NLOOP/EOP/PRE/PRIM/FLG/NREG */
    unsigned char unmodeled_08[8]; /* +0x08: REGS */
} GrGifTag;

extern GrPacket f2GpSave;
extern int f2Num;
extern GrPacket f4GpSave;
extern int f4Num;
extern GrPacket ft4GpSave;
extern int ft4Num;
extern GrPacket ft4STQGpSave;
extern int ft4STQNum;
extern GrPacket sprGpSave;
extern int sprNum;

void grCloseF2(void)
{
    GrGifTag *tags = (GrGifTag *)f2GpSave.data;
    tags[f2GpSave.count].control |= f2Num;
}

INCLUDE_ASM("asm/nonmatchings/ov01/gr_gp_init", grOpenF4);

void grCloseF4(void)
{
    GrGifTag *tags = (GrGifTag *)f4GpSave.data;
    tags[f4GpSave.count].control |= f4Num;
}

INCLUDE_ASM("asm/nonmatchings/ov01/gr_gp_init", grOpenFT4);

void grCloseFT4(void)
{
    GrGifTag *tags = (GrGifTag *)ft4GpSave.data;
    tags[ft4GpSave.count].control |= ft4Num;
}

INCLUDE_ASM("asm/nonmatchings/ov01/gr_gp_init", grOpenFT4STQ);

void grCloseFT4STQ(void)
{
    GrGifTag *tags = (GrGifTag *)ft4STQGpSave.data;
    tags[ft4STQGpSave.count].control |= ft4STQNum;
}

INCLUDE_ASM("asm/nonmatchings/ov01/gr_gp_init", grOpenSpr);

void grCloseSpr(void)
{
    GrGifTag *tags = (GrGifTag *)sprGpSave.data;
    tags[sprGpSave.count].control |= sprNum;
}

INCLUDE_ASM("asm/nonmatchings/ov01/gr_gp_init", grPutF2);

INCLUDE_ASM("asm/nonmatchings/ov01/gr_gp_init", grPutF4);

INCLUDE_ASM("asm/nonmatchings/ov01/gr_gp_init", grPutFT4);

INCLUDE_ASM("asm/nonmatchings/ov01/gr_gp_init", grPutFT4STQ);

INCLUDE_ASM("asm/nonmatchings/ov01/gr_gp_init", grPutSpr);

void grPacketSend(GrPacket *packet)
{
    XglPacket *vif1Packet;

    vif1Packet = xglPacketGetCurrent();
    sceVif1PkCnt(vif1Packet, 0);
    sceVif1PkAddCode(vif1Packet, VIF_CODE_FLUSH);
    sceVif1PkOpenDirectHLCode(vif1Packet, 0);
    sceVif1PkAddDirectDataN(vif1Packet, packet->data, packet->count);
    sceVif1PkCloseDirectHLCode(vif1Packet);
    sceVif1PkTerminate(vif1Packet);
}

Matrix *grWorldMatGet(void)
{
    Matrix *camera;

    /* Camera zero owns the current world matrix at the observed +0x470. */
    xglStudioGetCamera(&camera, 0);
    return (Matrix *)((u8 *)camera + 0x470);
}

void grCalcMatrix(MatrixCommand *command, Matrix *destination)
{
    Matrix *world;
    Matrix local;
    FloatVector4 translation;
    TransformState *state;

    world = grWorldMatGet();
    state = command->state;
    xglMatrixUnit((float (*)[4])&local);

    translation.x = state->position.x + state->offset.x;
    translation.y = state->position.y + state->offset.y;
    translation.z = state->position.z + state->offset.z;
    translation.w = 1.0f;

    xglMatrixTrans(&local, &local, &translation);
    grRotMatrix(&local, &local, &state->rotation);
    xglMatrixScale(&local, &local, (FloatVector4 *)&state->scale);
    xglMatrixMul(destination, world, &local);
}

void grRotMatrix(Matrix *destination, Matrix *source, FloatVector3 *angles)
{
    /* The resident matrix stack applies Y, then X, then Z in this wrapper. */
    xglMatrixStackLoad(source);
    xglMatrixStackRotY(angles->y);
    xglMatrixStackRotX(angles->x);
    xglMatrixStackRotZ(angles->z);
    xglMatrixStackSave((float (*)[4])destination);
}

/*
 * grRotTransPers's output vertex: the screen-space X/Y grPutXxx (siblings,
 * not decoded here) feed to the GS in 12.4 fixed point (the *16.0f below),
 * the projected depth as a plain integer, and the out-of-frustum bit
 * (0x8000) grRotTransPers itself copies from *clipFlags.
 */
typedef struct GrScreenVertex {
    int x;      /* +0x00 */
    int y;      /* +0x04 */
    int z;      /* +0x08 */
    int flags;  /* +0x0c */
} GrScreenVertex;

extern void xglVectorMulMat(Vector4 *destination, const Matrix4 matrix,
                            const Vector4 *vector);
extern void xglVectorMulAdd(Vector4 *destination, const Vector4 *left,
                            const Vector4 *right, const Vector4 *addend);

/*
 * Transforms point by matrix, perspective-divides by the result's W and
 * tests the divided X/Y/Z against the [-1, 1] clip volume. In bounds, the
 * point is mapped to screen space by the current camera's screen scale and
 * offset; out of bounds, *clipFlags gets the 0x8000 bit set and the point is
 * written back in clip space instead. Either way vertex->flags picks up
 * *clipFlags's 0x8000 bit, and the reciprocal of W is returned (grPutXxx
 * uses it to scale billboard extents by depth).
 */
float grRotTransPers(GrScreenVertex *vertex, const Matrix4 matrix,
                     const Vector4 *point, int *clipFlags)
{
    Vector4 local;
    StudioCamera *camera;
    float invW;
    int outOfBounds;

    xglVectorMulMat(&local, matrix, point);

    outOfBounds = 0;
    invW = 1.0f / local.w;
    local.w = 1.0f;
    local.x *= invW;
    local.y *= invW;
    local.z *= invW;
    if (local.x > 1.0f || local.x < -1.0f) {
        outOfBounds = 1;
    }
    if (local.y > 1.0f || local.y < -1.0f) {
        outOfBounds += 1;
    }
    if (local.z > 1.0f || local.z < -1.0f) {
        outOfBounds += 1;
    }

    if (outOfBounds == 0) {
        xglStudioGetCamera(&camera, 0);
        xglVectorMulAdd(&local, &local, &camera->screenScale,
                        &camera->screenOffset);
    } else {
        *clipFlags |= 0x8000;
    }

    vertex->z = (int)local.z;
    vertex->y = (int)(local.y * 16.0f);
    vertex->x = (int)(local.x * 16.0f);
    vertex->flags = *clipFlags & 0x8000;
    return invW;
}

INCLUDE_ASM("asm/nonmatchings/ov01/gr_gp_init", grBBCalc);

INCLUDE_ASM("asm/nonmatchings/ov01/gr_gp_init", grGsRegSet);

INCLUDE_ASM("asm/nonmatchings/ov01/gr_gp_init", grGsRegListSet);

INCLUDE_ASM("asm/nonmatchings/ov01/gr_gp_init", GS_REG_SET);
