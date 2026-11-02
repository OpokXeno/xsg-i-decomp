#include "common.h"
#include "shared.h"

/* The camera record copied by value as doubleword pairs. */
#include "main/maptbl.h"
#include "main/game_camera.h"
#include "main/nml_model_set.h"
#include "main/xgl_2.h"
#include "quadword.h"
#include "vu0_macro.h"

#define MoveHokanStateFlag (GameLoopState.camera_flags)

#define FLOOR_DETERMINANT(px, pz, qx, qz, rx, rz) \
    ((px) * (qz) + (qx) * (rz) + (rx) * (pz) - (px) * (rz) - (qx) * (pz) - (rx) * (qz))

struct CameraCenterSource {
    unsigned char unmodeled_00[0x10];
    Vector4 position;
    unsigned char unmodeled_20[0x3c];
    float facing_enabled;                 /* +0x5c */
    unsigned char unmodeled_60[0x20];
    unsigned char enemy_index;            /* +0x80 */
    unsigned char unmodeled_81[5];
    short type;                           /* +0x86 */
    unsigned char unmodeled_88[0x9e4 - 0x88];
    float facing_angle;                   /* +0x9e4 */
};

struct CameraCenterSource;

typedef struct GameLoopCameraView {
    unsigned char unmodeled_00[4];
    struct CameraCenterSource *player;
    unsigned char unmodeled_08[8];
    unsigned int flags;
    unsigned char unmodeled_14[0x14];
    void (*camera_callback)(void);
    unsigned char unmodeled_2c[0x28];
    void *map;
    unsigned char unmodeled_58[0xc0 - 0x58];
    unsigned char camera_mode;
    unsigned char camera_flags;
    unsigned char unmodeled_c2[2];
    unsigned short camera_id;
    unsigned short camera_index;
    unsigned char unmodeled_c8[0x230 - 0xc8];
    void (*saved_camera_callback)(void);
    unsigned char unmodeled_234[0xc];
    float saved_camera_angles[4];
    unsigned char unmodeled_250[0x2a030 - 0x250];
} GameLoopCameraView;

/* The view block of the studio camera record, from StudioCamera + 0x90. */
typedef struct CameraView {
    unsigned char unmodeled_00[4];
    float perspective;                    /* +0x04 */
    unsigned char unmodeled_08[8];
    Vector4 rotation;                     /* +0x10 */
    unsigned char unmodeled_20[0x20];
    Vector4 position;                     /* +0x40 */
    unsigned char unmodeled_50[0x30];
    Vector4 focus;                        /* +0x80 */
    Vector4 look_at;                      /* +0x90 */
} CameraView;

struct CameraView;

struct CameraScratchpad {
    CfCameraDefinition *definition;   /* +0x00 */
    StudioCamera *studio;                        /* +0x04 */
    const struct CameraCenterSource *target;     /* +0x08 */
    struct CameraView *view;                     /* +0x0c */
    int initial_wait;                            /* +0x10 */
    unsigned char unmodeled_14[8];
    unsigned char previous_mode;                 /* +0x1c */
    unsigned char previous_index;                /* +0x1d */
    unsigned char unmodeled_1e;
    unsigned char reserved_1f;                   /* +0x1f */
    Vector4 look_at;                             /* +0x20 */
    Vector4 position;                            /* +0x30 */
    float matrix[4][4];
    union {
        float translation[4];
        Vector4 translation_vector;
    };
    Vector4 position_delta;                      /* +0x90 */
    Vector4 probe_delta;                         /* +0xa0 */
    Vector4 look_at_delta;                       /* +0xb0 */
    int screen[4];                               /* +0xc0 */
    struct {
        int unmodeled_00[2];
        int depth;                               /* +0x08 */
        int unmodeled_0c;
    } defocus[3];                                /* +0xd0 */
    unsigned char unmodeled_100[0x40];
    DoublewordAlignedVector4 saved_look_at;                   /* +0x140 */
    unsigned char unmodeled_150[0x40];
    DoublewordAlignedVector4 saved_position;                   /* +0x190 */
};

/* Interpolation state of the camera pair (position, look-at): the per-frame
 * step, the frames still to run and the yaw being blended, bss at
 * main:0x004dcad0. */
typedef struct HokanState {
    float position_step;      /* +0x00 */
    float look_at_step;       /* +0x04 */
    float position_remaining; /* +0x08 */
    float look_at_remaining;  /* +0x0c */
    float previous_yaw;       /* +0x10 */
    float current_yaw;        /* +0x14 */
} HokanState;

typedef struct LayoutHeader LayoutHeader;

/* Floor mesh header returned by UnduDataGetHeader: a triangle list whose
 * corners are byte offsets into the vertex block. */
struct CameraFloorTriangle {
    unsigned char unmodeled_00[8];
    unsigned short corner[3];             /* +0x08 */
    unsigned char unmodeled_0e[2];
    short neighbor[3];                    /* +0x10 */
    unsigned char unmodeled_16[2];
};

struct LayoutHeader {
    unsigned short triangle_count;        /* +0x00 */
    unsigned char unmodeled_02[2];
    const unsigned char *vertices;        /* +0x04 */
    unsigned char unmodeled_08[4];
    const struct CameraFloorTriangle *triangles; /* +0x0c */
};

/* Query block of UnduCheck (still original bytes in src/main/Undulate.c): the
 * ground probe each camera point owns. Only the members the camera update
 * touches are named. */
/* Partial view of UnduParam; the full type cannot be included in this TU, which
 * defines its own struct LayoutHeader body. */
typedef struct UnduParamCameraView {
    int query_flags;                      /* +0x00 */
    int query_cursor;                     /* +0x04 */
    short attribute_mask;                 /* +0x08 */
    unsigned short result_flags;          /* +0x0a */
    unsigned char unmodeled_0c[4];
    float limit;                          /* +0x10 */
    float height;                         /* +0x14 */
    LayoutHeader *header;                 /* +0x18 */
    void *exclude;                        /* +0x1c */
    unsigned long long attribute;         /* +0x20 */
    unsigned char unmodeled_28[0x18];
} UnduParamCameraView;

typedef struct CameraDefocusLayer {
    unsigned char type;
    unsigned char state;
    unsigned char index;
    unsigned char unmodeled_03;
    int parameters[16];
} CameraDefocusLayer;

typedef struct CameraFloorVertex {
    float x;
    float y;
    float z;
} CameraFloorVertex;

typedef struct CameraEnemyWork {
    unsigned char unmodeled_00[0x3830];
    short state;                          /* +0x3830 */
    unsigned char unmodeled_3832[0x38b0 - 0x3832];
} CameraEnemyWork;

typedef int s32;

typedef s32 M2C_UNK;

extern const float cameraYawHalfTurn;

/* GameLoopState (main VA 0x00338680) is a 0x2a030-byte global (see
 * src/main/game.c); MoveHokan reads only the flag byte at +193 (main VA
 * 0x00338741, config/objects/main.objects.json alias MoveHokanStateFlag).
 * Declared here as the byte array the original size evidences, not as a
 * standalone one-byte extern: at -G8 an object at or under the G threshold is
 * addressed through $gp, and the original's own two-instruction
 * lui/lbu absolute address for this byte proves this access is NOT
 * gp-relative. This TU-local view is
 * independent of the other partial views other TUs give the same symbol
 * (GameLoopFlagsPrefix, GameLoopStateAddressView, ...; */
extern GameLoopCameraView GameLoopState;
extern CameraDefocusLayer GameDefocusParam[16];
extern const float nearestCenterInitialDistance;
extern const float oneThirdCornerSum;
extern const float oneThirdCentroid;
extern const float cameraLookAtMargin;
extern CameraEnemyWork enepc[16];

extern LayoutHeader *UnduDataGetHeader(int, unsigned int);
extern StudioCamera *xglStudioGetActiveCamera(void);
extern float xglRotTransPers(int *, void *, const float *, int);
extern void GameDefocusCheck(void);
extern double sqrt(double);
extern float UnduCheck(const Vector4 *, void *, UnduParamCameraView *);
extern void UnduParamInit(UnduParamCameraView *);
extern void nmlModelSetGlobalFogCol(const Vector4 *color);
static void CameraType1(void);
static void CalcLookAt(struct CameraScratchpad *scratchpad);
static void CalcCameraPos(struct CameraScratchpad *scratchpad);
static void CameraType1Core(void);
static void CameraType0(void);

static float CfCameraOfsNow[4];

static HokanState HokanNow;

static UnduParamCameraView CamLookAt;

static DoublewordAlignedVector4 CamLookAtPos;

static UnduParamCameraView CamPos;

static DoublewordAlignedVector4 CamPosPos;

static unsigned char pre_mode;

static float pa_0;

static int t_1;

static UnduParamCameraView CamPlayer;

static Vector4 CamPlyPosOld;

static __inline__ float FloorSlope(float rise, float run)
{
    if (run >= 0.0f && run < 0.001f) {
        return rise * 100.0f;
    }
    if (run <= 0.0f && -0.001f < run) {
        return rise * -100.0f;
    }
    return rise / run;
}

void GameCameraReset(void)
{
    StudioCamera *camera;
    int i;

    camera = xglStudioGetCamera2(0);
    camera->state = 4;
    camera->rotation.vector.z = 0.0f;
    for (i = 0; i < 33; i++) {
        CfCameraDefine[i].pedestal_mode = 0;
        CfCameraDefine[i].current_yaw = 0.0f;
        CfCameraDefine[i].offset[0] = 0.0f;
        CfCameraDefine[i].offset[1] = 0.0f;
        CfCameraDefine[i].offset[2] = 0.0f;
        CfCameraDefine[i].perspective = 0.69813174f;
        CfCameraDefine[i].parameters.offset.first_mode = 15;
        CfCameraDefine[i].parameters.offset.first_value = 0.05f;
        CfCameraDefine[i].parameters.offset.second_mode = 60;
        CfCameraDefine[i].parameters.offset.second_value = 0.05f;
        CfCameraDefine[i].angles.values[0] = -0.3926991f;
        CfCameraDefine[i].angles.values[1] = 0.0f;
        CfCameraDefine[i].angles.values[2] = 0.0f;
        CfCameraDefine[i].angles.values[3] = 3.0f;
        CfCameraDefine[i].lock_position[0] = 0.0f;
        CfCameraDefine[i].lock_position[1] = 0.0f;
        CfCameraDefine[i].lock_position[2] = 0.0f;
        CfCameraDefine[i].lock_mode = 0;
        CfCameraDefine[i].interpolation[0] = 0.03f;
        CfCameraDefine[i].interpolation[1] = 0.03f;
        CfCameraDefine[i].fog_parameters[0] = 0.0f;
        CfCameraDefine[i].fog_parameters[1] = 0.0f;
        CfCameraDefine[i].defocus[0].depth = 0.0f;
        CfCameraDefine[i].defocus[1].depth = 0.0f;
        CfCameraDefine[i].defocus[2].depth = 0.0f;
    }
    HokanNow.position_step = 0.0f;
    HokanNow.look_at_step = 0.0f;
    HokanNow.position_remaining = 0.0f;
    HokanNow.look_at_remaining = 0.0f;
    HokanNow.previous_yaw = 0.0f;
    HokanNow.current_yaw = 0.0f;
    CfCameraOfsNow[0] = 0.0f;
    CfCameraOfsNow[1] = 0.0f;
    CfCameraOfsNow[2] = 0.0f;
    GameLoopState.camera_mode = 1;
    GameLoopState.camera_callback = GameCamera;
    GameLoopState.camera_flags |= 0x80;
    GameCameraChangeID(0, 0, 0);
}

void GameCameraStateSave(void)
{
    StudioCamera *camera;

    camera = xglStudioGetCamera2(0);
    save_cam = *camera;
    QuadwordCopy(GameLoopState.saved_camera_angles, CfCameraDefine[0].angles.values);
    GameLoopState.saved_camera_callback = GameLoopState.camera_callback;
}

void GameCameraStateRestore(void)
{
    StudioCamera *camera;
    StudioCamera *active;

    camera = xglStudioGetCamera2(0);
    *camera = save_cam;
    camera->state = 4;
    camera->rotation.vector.z = 0.0f;
    QuadwordCopy(CfCameraDefine[0].angles.values, GameLoopState.saved_camera_angles);
    GameLoopState.camera_callback = GameLoopState.saved_camera_callback;
    HokanNow.position_step = 0.0f;
    HokanNow.look_at_step = 0.0f;
    HokanNow.position_remaining = 0.0f;
    HokanNow.look_at_remaining = 0.0f;
    do {
        active = xglStudioGetActiveCamera();
        if (active != 0) {
            active->active = 0;
        }
    } while (active != 0);
    xglStudioGetCamera2(0)->active = 1;
}

void GameCameraChangeID(int id, int unused1, int unused2)
{
    LayoutHeader *header;

    if (GameLoopState.camera_id != id) {
        header = UnduDataGetHeader(256, id);
        if (header == 0) {
            id = 0x8000;
            header = UnduDataGetHeader(256, 0x8000);
        }
        GameLoopState.camera_flags |= 0x40;
        GameLoopState.camera_id = id;
        CamLookAt.query_cursor = 0;
        CamLookAt.header = header;
        CamPos.query_cursor = 0;
        CamPos.header = header;
        CamLookAt.query_flags = 0;
        CamPos.query_flags = 0;
    }
}

/* MoveHokan (main:0x00246c60, 348 bytes): five-argument EE ABI. `unused` (a0)
 * is never read; `target` (a1) is the point being approached; `current` (a2)
 * is the vector advanced in place; `step` (f12) is the per-frame countdown
 * decrement; `remaining` (a3) is the caller's countdown, zeroed and used as
 * the interpolation weight. When the countdown is already zero or
 * MoveHokanStateFlag's high bit is set, snap `current` to `target` and clear
 * the countdown. Otherwise compute `current - target` on COP2 (vf20/vf21,
 * vsub.xyzw) into a stack temp, take the XYZ distance through the hardware
 * sqrt idiom, decrement the countdown by `step`, force it to zero once the
 * distance undershoots `step`, exceeds 100 units or goes negative, then
 * either snap-copy (countdown hit zero) or interpolate XYZ through
 * xglVectorScaleAddXYZ using the stored difference and the remaining W lane
 * untouched. The two inline statements are the quadword snap-copy and the
 * COP2 vector subtract (config/tu/contracts/main-tu120-move-hokan.asm_contract.json). */
static void MoveHokan(void *unused, const Vector4 *target, Vector4 *current,
                      float step, float *remaining)
{
    Vector4 difference;
    float distance;

    (void)unused;
    if (*remaining == 0.0f || (MoveHokanStateFlag & 0x80) != 0) {
        *remaining = 0.0f;
        QuadwordCopy(current, target);
        return;
    }

    Vu0SubVector(&difference, current, target);

    distance = __builtin_sqrtf(difference.x * difference.x
                                   + difference.y * difference.y
                                   + difference.z * difference.z);

    *remaining -= step;
    if (distance < step || 100.0f < distance || *remaining < 0.0f)
        *remaining = 0.0f;

    if (*remaining == 0.0f) {
        QuadwordCopy(current, target);
    } else {
        xglVectorScaleAddXYZ(*remaining, current, &difference, target);
    }
}

static void GetCenter(Vector4 *center, const struct CameraCenterSource *source)
{
    float vertical_offset;

    vertical_offset = 1.2f;
    if (source->type >= 1618) {
        vertical_offset = 3.0f;
    }

    center->x = CfCameraOfsNow[0] + source->position.x;
    center->y = CfCameraOfsNow[1] + source->position.y + vertical_offset;
    center->z = CfCameraOfsNow[2] + source->position.z;
    center->w = 1.0f;
}

static void GetCameraPos(float *destination, const float *position,
                         const float *angles)
{
    struct CameraScratchpad *scratchpad = (void *)0x70000000;

    scratchpad->translation[0] = 0.0f;
    scratchpad->translation[1] = 0.0f;
    scratchpad->translation[2] = angles[3];
    scratchpad->translation[3] = 1.0f;
    xglMatrixStackUnit();
    xglMatrixStackTrans(position);
    xglMatrixStackRotY(angles[1]);
    xglMatrixStackRotX(angles[0]);
    xglMatrixStackTrans(scratchpad->translation);
    xglMatrixStackSave(scratchpad->matrix);
    destination[0] = scratchpad->matrix[3][0];
    destination[1] = scratchpad->matrix[3][1];
    destination[2] = scratchpad->matrix[3][2];
}

static void GetNearestCenter(Vector4 *center, const LayoutHeader *header,
                             const Vector4 *point)
{
    const struct CameraFloorTriangle *triangle;
    const struct CameraFloorTriangle *nearest;
    const unsigned char *vertices;
    const CameraFloorVertex *first;
    const CameraFloorVertex *second;
    const CameraFloorVertex *third;
    float best;
    float distance;
    unsigned int count;
    unsigned int remaining;

    count = header->triangle_count;
    nearest = 0;
    best = nearestCenterInitialDistance;
    triangle = header->triangles;
    vertices = header->vertices;
    if (count != 0) {
        remaining = count;
        do {
            float dx;
            float dz;
            const CameraFloorVertex *corner_first;
            const CameraFloorVertex *corner_second;
            const CameraFloorVertex *corner_third;

            unsigned short first_offset = triangle->corner[0];
            unsigned short second_offset = triangle->corner[1];
            unsigned short third_offset = triangle->corner[2];

            corner_first = (const CameraFloorVertex *)(vertices + first_offset);
            corner_second = (const CameraFloorVertex *)(vertices + second_offset);
            corner_third = (const CameraFloorVertex *)(vertices + third_offset);
            dx = (corner_first->x + corner_second->x + corner_third->x) * oneThirdCornerSum - point->x;
            dz = (corner_first->z + corner_second->z + corner_third->z) * oneThirdCornerSum - point->z;
            distance = dx * dx + dz * dz;
            if (distance < best) {
                best = distance;
                nearest = triangle;
            }
            triangle++;
        } while (--remaining != 0);
    }
    first = (const CameraFloorVertex *)(vertices + nearest->corner[0]);
    second = (const CameraFloorVertex *)(vertices + nearest->corner[1]);
    third = (const CameraFloorVertex *)(vertices + nearest->corner[2]);
    center->x = (first->x + second->x + third->x) * oneThirdCentroid;
    center->y = (first->y + second->y + third->y) * oneThirdCentroid;
    center->z = (first->z + second->z + third->z) * oneThirdCentroid;
}

static void CalcLookAt(struct CameraScratchpad *scratchpad)
{
    float requestedLookAtHeight;
    float lookAtDeltaX;
    float lookAtDeltaZ;
    float maximumLookAtHeight;
    float distance;
    int steps;

    Vu0SubVector(&scratchpad->look_at_delta, &scratchpad->look_at, &CamLookAtPos);
    CamLookAt.attribute_mask = 3;
    UnduCheck(&CamLookAtPos, &scratchpad->look_at_delta, &CamLookAt);
    if (CamLookAt.result_flags & 3) {
        requestedLookAtHeight = scratchpad->look_at.y;
        CamLookAt.attribute_mask = 2;
        CamLookAtPos.y = requestedLookAtHeight;
        Vu0SubVector(&scratchpad->look_at_delta, &scratchpad->look_at, &CamLookAtPos);
        UnduCheck(&CamLookAtPos, &scratchpad->look_at_delta, &CamLookAt);
    }
    if (CamLookAt.height == -1000.0f) {
        GetNearestCenter(&CamLookAtPos, CamLookAt.header, &scratchpad->look_at);
        CamLookAt.query_cursor = 0;
        CamLookAt.query_flags = 0;
        UnduCheck(&CamLookAtPos, 0, &CamLookAt);
        Vu0SubVector(&scratchpad->look_at_delta, &scratchpad->look_at, &CamLookAtPos);
        CamLookAt.attribute_mask = 3;
        UnduCheck(&CamLookAtPos, &scratchpad->look_at_delta, &CamLookAt);
        if ((GameLoopState.camera_mode == 3) ||
            (CamLookAt.height < CamLookAtPos.y)) {
            CamLookAtPos.y = CamLookAt.height;
        }
        maximumLookAtHeight = scratchpad->look_at.y;
        if (maximumLookAtHeight < CamLookAtPos.y) {
            CamLookAtPos.y = maximumLookAtHeight;
        }
        CamLookAt.attribute_mask = 2;
        Vu0SubVector(&scratchpad->look_at_delta, &scratchpad->look_at, &CamLookAtPos);
        lookAtDeltaX = scratchpad->look_at_delta.x;
        lookAtDeltaZ = scratchpad->look_at_delta.z;
        distance = 2.0f * (float)sqrt((lookAtDeltaX * lookAtDeltaX) +
                                    (lookAtDeltaZ * lookAtDeltaZ));
        if (distance > 10.0f) {
            distance = 10.0f;
        }
        if (distance < 1.0f) {
            distance = 1.0f;
        }
        steps = (int)distance;
        scratchpad->translation[0] = scratchpad->look_at_delta.x / distance;
        scratchpad->translation[2] = scratchpad->look_at_delta.z / distance;
        scratchpad->translation[1] = scratchpad->look_at_delta.y / distance;
        for (; steps >= 0; steps--) {
            UnduCheck(&CamLookAtPos, scratchpad->translation, &CamLookAt);
        }
    }
}

static void CalcCameraPos(struct CameraScratchpad *scratchpad)
{
    float height;

    Vu0SubVector(&scratchpad->probe_delta, &scratchpad->position, &CamPosPos);
    UnduCheck(&CamPosPos, &scratchpad->probe_delta, &CamPos);
    height = CamPos.height;
    if (height != -1000.0f && (CamPos.result_flags & 3) == 0) {
        if (GameLoopState.camera_mode == 3) {
            CamPosPos.y = height;
        } else if (height < CamPosPos.y) {
            CamPosPos.y = height;
        }
    } else {
        QuadwordCopy(&CamPosPos, &CamLookAtPos);
        CamPos.query_cursor = 0;
        CamPos.query_flags = 0;
        UnduCheck(&CamPosPos, 0, &CamPos);
        Vu0SubVector(&scratchpad->position_delta, &scratchpad->position, &CamPosPos);
        if (-0.001f < scratchpad->position_delta.x && scratchpad->position_delta.x < 0.001f) {
            scratchpad->position_delta.x = scratchpad->position_delta.x * 100.0f;
        }
        if (scratchpad->position_delta.z > -0.001f && scratchpad->position_delta.z < 0.001f) {
            scratchpad->position_delta.z = scratchpad->position_delta.z * 100.0f;
        }
        CamPos.attribute_mask = 3;
        UnduCheck(&CamPosPos, &scratchpad->position_delta, &CamPos);
        CamPos.attribute_mask = 2;
        if (GameLoopState.camera_mode != 3) {
            if (!(CamPos.height < CamPosPos.y)) {
                return;
            }
        }
        CamPosPos.y = CamPos.height;
    }
}

static void CameraType0(void)
{
    struct CameraScratchpad *scratchpad = (void *)0x70000000;
    UnduParamCameraView ground;
    UnduParamCameraView edge_probe;
    Vector4 hit;
    const struct CameraFloorTriangle *triangle;
    const unsigned char *vertices;
    const unsigned char *triangles;
    const CameraFloorVertex *corner0;
    const CameraFloorVertex *corner1;
    const CameraFloorVertex *corner2;
    const CameraFloorVertex *edge_start;
    const CameraFloorVertex *edge_end;
    float floor_height;
    Vector4 *look_at_out;
    float eye_side, goal_side, start_side, end_side;
    float eye_x, eye_z, goal_x, goal_z;
    float run_z, run_x, ray_length, reach;
    float eye_slope, edge_slope;
    float hit_x, hit_z, along;
    float start_x, start_z;
    int crossed_edge;
    int step;

    if (scratchpad->definition->angles.values[3] < 0.4f) {
        scratchpad->definition->angles.values[3] = 0.4f;
    }
    GetCenter(&scratchpad->look_at, scratchpad->target);
    look_at_out = &scratchpad->view->look_at;
    MoveHokan(scratchpad, &scratchpad->look_at, look_at_out,
              HokanNow.look_at_step, &HokanNow.look_at_remaining);
    GetCameraPos(&scratchpad->look_at_delta.x, &scratchpad->view->look_at.x,
                 scratchpad->definition->angles.values);
    UnduParamInit(&ground);
    ground.exclude = (void *)0x700001A0;
    ground.header = UnduDataGetHeader(0x100, GameLoopState.camera_id);
    ground.limit = 1000.0f;
    floor_height = UnduCheck(&scratchpad->look_at_delta, 0, &ground);
    if (floor_height != -1000.0f) {
        CamPosPos.x = scratchpad->look_at_delta.x;
        CamPosPos.z = scratchpad->look_at_delta.z;
        if (scratchpad->look_at_delta.y < floor_height) {
            CamPosPos.y = scratchpad->look_at_delta.y;
        } else {
            CamPosPos.y = floor_height;
        }
    } else {
        UnduParamInit(&edge_probe);
        edge_probe.exclude = (void *)0x700001A0;
        edge_probe.header = UnduDataGetHeader(0x100, GameLoopState.camera_id);
        edge_probe.limit = 1000.0f;
        UnduCheck(&scratchpad->look_at, 0, &edge_probe);
        if (edge_probe.query_cursor != 0) {
            triangle = (const struct CameraFloorTriangle *)edge_probe.query_cursor;
            goal_x = scratchpad->look_at_delta.x;
            crossed_edge = 0;
            goal_z = scratchpad->look_at_delta.z;
            step = 0;
            triangles = (const unsigned char *)edge_probe.header->triangles;
            vertices = edge_probe.header->vertices;
            do {
                corner0 = (const CameraFloorVertex *)(vertices + triangle->corner[0]);
                corner1 = (const CameraFloorVertex *)(vertices + triangle->corner[1]);
                corner2 = (const CameraFloorVertex *)(vertices + triangle->corner[2]);
                eye_side = FLOOR_DETERMINANT(scratchpad->look_at.x, scratchpad->look_at.z,
                                             corner0->x, corner0->z, corner1->x, corner1->z);
                goal_side = FLOOR_DETERMINANT(scratchpad->look_at_delta.x, scratchpad->look_at_delta.z,
                                              corner0->x, corner0->z, corner1->x, corner1->z);
                if (eye_side <= 0.0f && goal_side >= 0.0f && (eye_side != 0.0f || goal_side != 0.0f)) {
                    start_side = FLOOR_DETERMINANT(corner0->x, corner0->z, scratchpad->look_at.x, scratchpad->look_at.z, scratchpad->look_at_delta.x, scratchpad->look_at_delta.z);
                    end_side = FLOOR_DETERMINANT(corner1->x, corner1->z, scratchpad->look_at.x, scratchpad->look_at.z, scratchpad->look_at_delta.x, scratchpad->look_at_delta.z);
                    if ((start_side <= 0.0f && end_side >= 0.0f) || (start_side >= 0.0f && end_side <= 0.0f)) {
                        if (triangle->neighbor[0] == -1) {
                            crossed_edge = 1;
                            break;
                        }
                        triangle = (const struct CameraFloorTriangle *)
                            (triangles + triangle->neighbor[0]);
                        continue;
                    }
                }
                eye_side = FLOOR_DETERMINANT(scratchpad->look_at.x, scratchpad->look_at.z,
                                             corner1->x, corner1->z, corner2->x, corner2->z);
                goal_side = FLOOR_DETERMINANT(scratchpad->look_at_delta.x, scratchpad->look_at_delta.z,
                                              corner1->x, corner1->z, corner2->x, corner2->z);
                if (eye_side <= 0.0f && goal_side >= 0.0f && (eye_side != 0.0f || goal_side != 0.0f)) {
                    start_side = FLOOR_DETERMINANT(corner1->x, corner1->z, scratchpad->look_at.x, scratchpad->look_at.z, scratchpad->look_at_delta.x, scratchpad->look_at_delta.z);
                    end_side = FLOOR_DETERMINANT(corner2->x, corner2->z, scratchpad->look_at.x, scratchpad->look_at.z, scratchpad->look_at_delta.x, scratchpad->look_at_delta.z);
                    if ((start_side <= 0.0f && end_side >= 0.0f) || (start_side >= 0.0f && end_side <= 0.0f)) {
                        if (triangle->neighbor[1] == -1) {
                            crossed_edge = 2;
                            break;
                        }
                        triangle = (const struct CameraFloorTriangle *)
                            (triangles + triangle->neighbor[1]);
                        continue;
                    }
                }
                eye_side = FLOOR_DETERMINANT(scratchpad->look_at.x, scratchpad->look_at.z,
                                             corner2->x, corner2->z, corner0->x, corner0->z);
                goal_side = FLOOR_DETERMINANT(scratchpad->look_at_delta.x, scratchpad->look_at_delta.z,
                                              corner2->x, corner2->z, corner0->x, corner0->z);
                if (eye_side <= 0.0f && goal_side >= 0.0f && (eye_side != 0.0f || goal_side != 0.0f)) {
                    start_side = FLOOR_DETERMINANT(corner2->x, corner2->z, scratchpad->look_at.x, scratchpad->look_at.z, scratchpad->look_at_delta.x, scratchpad->look_at_delta.z);
                    end_side = FLOOR_DETERMINANT(corner0->x, corner0->z, scratchpad->look_at.x, scratchpad->look_at.z, scratchpad->look_at_delta.x, scratchpad->look_at_delta.z);
                    if ((start_side <= 0.0f && end_side >= 0.0f) || (start_side >= 0.0f && end_side <= 0.0f)) {
                        if (triangle->neighbor[2] == -1) {
                            crossed_edge = 3;
                            break;
                        }
                        triangle = (const struct CameraFloorTriangle *)
                            (triangles + triangle->neighbor[2]);
                        continue;
                    }
                }
                break;
            } while (++step < 0x100);
            if (crossed_edge > 0) {
                edge_start = 0;
                edge_end = 0;
                switch (crossed_edge) {
                case 1:
                    edge_start = (const CameraFloorVertex *)(vertices + triangle->corner[0]);
                    edge_end = (const CameraFloorVertex *)(vertices + triangle->corner[1]);
                    break;
                case 2:
                    edge_start = (const CameraFloorVertex *)(vertices + triangle->corner[1]);
                    edge_end = (const CameraFloorVertex *)(vertices + triangle->corner[2]);
                    break;
                case 3:
                    edge_start = (const CameraFloorVertex *)(vertices + triangle->corner[2]);
                    edge_end = (const CameraFloorVertex *)(vertices + triangle->corner[0]);
                    break;
                }
                start_x = edge_start->x;
                start_z = edge_start->z;
                eye_slope = FloorSlope(goal_z - scratchpad->look_at.z, goal_x - scratchpad->look_at.x);
                edge_slope = FloorSlope(edge_end->z - start_z, edge_end->x - start_x);
                hit_x = ((edge_slope * start_x - start_z) - eye_slope * scratchpad->look_at.x + scratchpad->look_at.z)
                        / (edge_slope - eye_slope);
                hit_z = eye_slope * (hit_x - scratchpad->look_at.x) + scratchpad->look_at.z;
                hit.x = hit_x;
                hit.z = hit_z;
                hit.y = edge_start->y + (edge_end->y - edge_start->y)
                        * ((hit_x - edge_start->x) / (edge_end->x - edge_start->x));
                run_x = hit_x - scratchpad->look_at.x;
                run_z = hit_z - scratchpad->look_at.z;
                ray_length = __builtin_sqrtf(run_x * run_x + run_z * run_z);
                run_x = scratchpad->look_at_delta.x - scratchpad->look_at.x;
                run_z = scratchpad->look_at_delta.z - scratchpad->look_at.z;
                reach = ray_length - 0.1f;
                along = reach / __builtin_sqrtf(run_x * run_x + run_z * run_z);
                CamPosPos.x = scratchpad->look_at.x + run_x * along;
                hit.x = CamPosPos.x;
                CamPosPos.y = scratchpad->look_at.y
                              + (scratchpad->look_at_delta.y - scratchpad->look_at.y) * along;
                hit.y = CamPosPos.y;
                CamPosPos.z = scratchpad->look_at.z + run_z * along;
                hit.z = CamPosPos.z;
            }
        } else if (GameLoopState.camera_flags & 0x80) {
            CamPosPos.x = scratchpad->look_at_delta.x;
            CamPosPos.y = scratchpad->look_at_delta.y;
            CamPosPos.z = scratchpad->look_at_delta.z;
        }
    }
    MoveHokan(scratchpad, &CamPosPos, &scratchpad->view->position,
              HokanNow.position_step, &HokanNow.position_remaining);
    Vu0SubVectorXYZ(&scratchpad->look_at_delta, &scratchpad->view->look_at, &scratchpad->view->position);
    scratchpad->view->rotation.y = xglAtan2(scratchpad->look_at_delta.x, scratchpad->look_at_delta.z) + 3.1415927f;
    scratchpad->view->rotation.x = xglAtan2(scratchpad->look_at_delta.y,
        __builtin_sqrtf(scratchpad->look_at_delta.x * scratchpad->look_at_delta.x
                            + scratchpad->look_at_delta.z * scratchpad->look_at_delta.z));
    QuadwordCopy(&scratchpad->view->focus, &scratchpad->look_at);
}

static void CameraType1Core(void)
{
    struct CameraScratchpad *scratchpad = (void *)0x70000000;

    GetCenter(&scratchpad->look_at, scratchpad->target);
    if (scratchpad->definition->lock_mode != 0) {
        if (scratchpad->definition->lock_mode & 1) {
            scratchpad->look_at.x = scratchpad->definition->lock_position[0];
        }
        if (scratchpad->definition->lock_mode & 2) {
            scratchpad->look_at.y = scratchpad->definition->lock_position[1];
        }
        if (scratchpad->definition->lock_mode & 4) {
            scratchpad->look_at.z = scratchpad->definition->lock_position[2];
        }
    }
    CalcLookAt(scratchpad);
    GetCameraPos(&scratchpad->position.x, &CamLookAtPos.x, scratchpad->definition->angles.values);
    if (scratchpad->definition->lock_mode != 0) {
        if (scratchpad->definition->lock_mode & 1) {
            scratchpad->position.x = scratchpad->definition->lock_position[0];
        }
        if (scratchpad->definition->lock_mode & 2) {
            scratchpad->position.y = scratchpad->definition->lock_position[1];
        }
        if (scratchpad->definition->lock_mode & 4) {
            scratchpad->position.z = scratchpad->definition->lock_position[2];
        }
    }
    CalcCameraPos(scratchpad);
    MoveHokan(scratchpad, &CamLookAtPos, &scratchpad->view->look_at,
              HokanNow.look_at_step, &HokanNow.look_at_remaining);
    MoveHokan(scratchpad, &CamPosPos, &scratchpad->view->position,
              HokanNow.position_step, &HokanNow.position_remaining);
    if (scratchpad->view->position.z - cameraLookAtMargin < scratchpad->view->look_at.z) {
        scratchpad->view->look_at.z = scratchpad->view->position.z - cameraLookAtMargin;
    }
}

static void CameraType1(void)
{
    struct CameraScratchpad *scratchpad = (void *)0x70000000;
    float previous_delta;
    float current_delta;
    float yaw_delta;
    float product;
    float yaw_period;

    if (scratchpad->definition->angles.values[3] < 0.4f) {
        scratchpad->definition->angles.values[3] = 0.4f;
    }
    scratchpad->view->perspective = scratchpad->definition->perspective;

    CameraType1Core();

    {
        float *look_at_delta = &scratchpad->look_at_delta.x;
        struct CameraView *view = scratchpad->view;
        const Vector4 *view_look_at = &view->look_at;
        const Vector4 *view_position = &view->position;

        Vu0SubVectorXYZ(look_at_delta, view_look_at, view_position);
    }

    scratchpad->view->rotation.y =
        xglAtan2(scratchpad->look_at_delta.x, scratchpad->look_at_delta.z) +
        3.1415927f;
    scratchpad->view->rotation.x = xglAtan2(
        scratchpad->look_at_delta.y,
        __builtin_sqrtf((scratchpad->look_at_delta.x * scratchpad->look_at_delta.x) +
                        (scratchpad->look_at_delta.z * scratchpad->look_at_delta.z)));

    previous_delta = HokanNow.previous_yaw - scratchpad->view->rotation.y;
    current_delta = HokanNow.current_yaw - scratchpad->view->rotation.y;
    yaw_delta = HokanNow.previous_yaw - HokanNow.current_yaw;
    yaw_period = 6.2831855f;
    if (previous_delta < -3.1415927f) {
        previous_delta += yaw_period;
    }
    if (current_delta < -3.1415927f) {
        current_delta += yaw_period;
    }
    if (3.1415927f < previous_delta) {
        previous_delta -= yaw_period;
    }
    if (3.1415927f < current_delta) {
        current_delta -= yaw_period;
    }
    if (yaw_delta < 0.0f) {
        yaw_delta += yaw_period;
        while (yaw_delta < 0.0f) {
            yaw_delta += yaw_period;
        }
    }
    if (3.1415927f < yaw_delta) {
        yaw_delta = yaw_period - yaw_delta;
    }

    product = previous_delta * current_delta;
    if (previous_delta < 0.0f) {
        previous_delta = -previous_delta;
    }
    if (current_delta < 0.0f) {
        current_delta = -current_delta;
    }
    if (product > 0.0f || yaw_delta < previous_delta ||
        yaw_delta < current_delta) {
        if (previous_delta < current_delta) {
            scratchpad->view->rotation.y = HokanNow.previous_yaw;
        } else {
            scratchpad->view->rotation.y = HokanNow.current_yaw;
        }
    }

    QuadwordCopy(&scratchpad->view->focus, &scratchpad->look_at);
}

static void CameraType4(void)
{
    struct CameraScratchpad *scratchpad = (void *)0x70000000;
    const CfCameraDefinition *definition;
    int mode;
    float horizontal;

    definition = scratchpad->definition;
    CamPosPos.x = definition->offset[0];
    CamPosPos.y = definition->offset[1];
    CamPosPos.z = definition->offset[2];
    scratchpad->view->perspective = definition->perspective;
    mode = F2I(definition->parameters.pedestal[3]);
    switch (mode) {
    case 0:
        scratchpad->look_at.x = definition->parameters.pedestal[0];
        scratchpad->look_at.y = definition->parameters.pedestal[1];
        scratchpad->look_at.z = definition->parameters.pedestal[2];
        break;
    case 1:
        GetCenter(&scratchpad->look_at, scratchpad->target);
        break;
    case 2:
        scratchpad->view->rotation.x = definition->parameters.pedestal[0];
        scratchpad->view->rotation.y = definition->parameters.pedestal[1];
        scratchpad->view->rotation.z = definition->parameters.pedestal[2];
        break;
    }
    MoveHokan(scratchpad, &CamPosPos, &scratchpad->view->position,
              HokanNow.position_step, &HokanNow.position_remaining);
    if (mode >= 0) {
        if (mode < 2) {
            MoveHokan(scratchpad, &scratchpad->look_at, &scratchpad->view->look_at,
                      HokanNow.look_at_step, &HokanNow.look_at_remaining);
            Vu0SubVectorXYZ(&scratchpad->look_at_delta, &scratchpad->view->look_at, &scratchpad->view->position);
            scratchpad->view->rotation.y = xglAtan2(scratchpad->look_at_delta.x, scratchpad->look_at_delta.z) + cameraYawHalfTurn;
            horizontal = __builtin_sqrtf(scratchpad->look_at_delta.x * scratchpad->look_at_delta.x
                                             + scratchpad->look_at_delta.z * scratchpad->look_at_delta.z);
            scratchpad->view->rotation.x = xglAtan2(scratchpad->look_at_delta.y, horizontal);
            QuadwordCopy(&scratchpad->view->focus, &scratchpad->look_at);
        }
    }
}

static void GameCameraDefocus(struct CameraScratchpad *scratchpad)
{
    StudioCamera *camera;
    CameraDefocusLayer *layers = GameDefocusParam;
    float scale;

    camera = xglStudioGetCamera2(0);
    scratchpad->translation[0] = CamLookAtPos.x;
    scratchpad->translation[1] = CamLookAtPos.y;
    scratchpad->translation[2] = CamLookAtPos.z;
    scratchpad->translation[3] = 1.0f;
    scale = xglRotTransPers(scratchpad->screen, 0, scratchpad->translation, 0) * camera->screenScale.z;
    scratchpad->defocus[0].depth = scratchpad->screen[2] + F2I(scratchpad->definition->defocus[0].depth * scale);
    scratchpad->defocus[1].depth = scratchpad->screen[2] + F2I(scratchpad->definition->defocus[1].depth * scale);
    scratchpad->defocus[2].depth = scratchpad->screen[2] + F2I(scratchpad->definition->defocus[2].depth * scale);
    layers[4].type = 0;
    layers[5].type = 0;
    layers[6].type = 0;
    layers[7].type = 0;
    if (scratchpad->definition->defocus[0].depth != 0.0f) {
        layers[4].parameters[0] = 0;
        layers[4].type = 2;
        layers[4].parameters[1] = scratchpad->defocus[0].depth;
        layers[5].type = 3;
        layers[5].parameters[0] = 1;
        layers[5].parameters[1] = 0x40808080;
        layers[5].parameters[2] = scratchpad->definition->defocus[0].width;
        layers[5].parameters[3] = scratchpad->definition->defocus[0].height;
        layers[5].parameters[4] = 0;
        layers[5].parameters[5] = 0;
    }
    if (scratchpad->definition->defocus[1].depth != 0.0f) {
        layers[6].type = 1;
        layers[6].parameters[1] = 1;
        layers[6].parameters[0] = 0;
        layers[6].parameters[2] = scratchpad->defocus[1].depth;
        layers[6].parameters[3] = 0x40808080;
        layers[6].parameters[4] = scratchpad->definition->defocus[1].width;
        layers[6].parameters[5] = scratchpad->definition->defocus[1].height;
        layers[6].parameters[6] = 0;
        layers[6].parameters[7] = 0;
    }
    if (scratchpad->definition->defocus[2].depth != 0.0f) {
        layers[7].type = 1;
        layers[7].parameters[1] = 1;
        layers[7].parameters[0] = 0;
        layers[7].parameters[2] = scratchpad->defocus[2].depth;
        layers[7].parameters[3] = 0x40808080;
        layers[7].parameters[4] = scratchpad->definition->defocus[2].width;
        layers[7].parameters[5] = scratchpad->definition->defocus[2].height;
        layers[7].parameters[6] = 0;
        layers[7].parameters[7] = 0;
    }
    GameDefocusCheck();
}

void GameCamera(void)
{
    struct CameraScratchpad *scratchpad = (void *)0x70000000;
    const CfCameraDefinition *previous;
    unsigned char mode;
    int index;
    float distance;
    int count;
    DoublewordAlignedVector4 *saved_look_at;
    DoublewordAlignedVector4 *saved_position;
    const float *fog;

    if (GameLoopState.map == 0) {
        return;
    }
    scratchpad->initial_wait = 40;
    scratchpad->reserved_1f = 0;
    scratchpad->studio = xglStudioGetCamera2(0);
    scratchpad->view = (struct CameraView *)((char *)scratchpad->studio + 0x90);
    scratchpad->look_at.w = 1.0f;
    scratchpad->target = GameLoopState.player;
    if (GameLoopState.flags & 0x20) {
        LayoutHeader *header;

        GameLoopState.camera_id = 0x8000;
        GameLoopState.camera_flags |= 0xe0;
        GameLoopState.camera_index = 0;
        UnduParamInit(&CamPlayer);
        CamPlayer.exclude = (void *)0x700001a0;
        CamPlayer.header = UnduDataGetHeader(0x302, 0x8000);
        CamPlayer.attribute_mask = 0x810;
        header = UnduDataGetHeader(0x100, 0x8000);
        UnduParamInit(&CamLookAt);
        CamLookAt.exclude = (void *)0x700001a0;
        CamLookAt.header = header;
        CamLookAt.limit = 1000.0f;
        CamLookAt.attribute_mask = 2;
        UnduParamInit(&CamPos);
        CamPos.exclude = (void *)0x700001a0;
        CamPos.header = header;
        CamPos.limit = 1000.0f;
        CamPos.attribute_mask = 2;
    }
    if (GameLoopState.camera_flags & 0x20) {
        QuadwordCopy(&CamPlyPosOld, &scratchpad->target->position);
    }
    if ((unsigned)(enepc[scratchpad->target->enemy_index].state - 3) >= 5) {
        scratchpad->previous_mode = GameLoopState.camera_mode;
        scratchpad->previous_index = GameLoopState.camera_index;
        if ((GameLoopState.camera_flags & 8) || GameLoopState.camera_mode == 0xff) {
            GameLoopState.camera_index = 0;
            GameLoopState.camera_id = 0x8000;
        } else {
            if (CamPlayer.header != 0 && !(GameLoopState.camera_flags & 2)) {
                Vu0SubVector(&scratchpad->look_at_delta, &scratchpad->target->position, &CamPlyPosOld);
                UnduCheck(&CamPlyPosOld, &scratchpad->look_at_delta, &CamPlayer);
                if (CamPlayer.height == -1000.0f) {
                    QuadwordCopy(&CamPlyPosOld, &scratchpad->target->position);
                    CamPlayer.query_cursor = 0;
                    CamPlayer.query_flags = 0;
                    UnduCheck(&CamPlyPosOld, 0, &CamPlayer);
                }
                if ((CamPlayer.attribute & 0x38000000) == 0x20000000) {
                    index = ((CamPlayer.attribute >> 20) & 0x7f) + 1;
                } else {
                    index = GameLoopState.camera_index;
                }
                mode = CfCameraDefine[index].pedestal_mode == 0 ? 3 : 4;
            } else {
                index = 0;
                mode = 0;
            }
            if (GameLoopState.camera_mode != mode) {
                GameLoopState.camera_mode = mode;
                GameLoopState.camera_flags |= 0x40;
            }
            if (GameLoopState.camera_index != index) {
                GameLoopState.camera_index = index;
                GameCameraChangeID(index, 0, 0);
            }
        }
    }
    scratchpad->definition = &CfCameraDefine[GameLoopState.camera_index];
    if (HokanNow.position_remaining == 0.0f) {
        HokanNow.position_step = scratchpad->definition->interpolation[0];
    }
    if (HokanNow.look_at_remaining == 0.0f) {
        HokanNow.look_at_step = scratchpad->definition->interpolation[1];
    }
    if (GameLoopState.camera_flags & 0x40) {
        previous = &CfCameraDefine[scratchpad->previous_index];
        if ((scratchpad->previous_mode == 4 && (previous->pedestal_interpolation & 1))
            || (GameLoopState.camera_mode == 4 && (scratchpad->definition->pedestal_interpolation & 1))) {
            float yaw;

            HokanNow.position_remaining = 0.0f;
            HokanNow.look_at_remaining = 0.0f;
            yaw = scratchpad->definition->current_yaw;
            HokanNow.current_yaw = yaw;
            HokanNow.previous_yaw = yaw;
        } else {
            HokanNow.position_remaining = 1.0f;
            HokanNow.previous_yaw = HokanNow.current_yaw;
            HokanNow.look_at_remaining = 1.0f;
            HokanNow.current_yaw = scratchpad->definition->current_yaw;
            if (scratchpad->previous_mode == 3) {
                if (GameLoopState.camera_mode == 3 && previous->current_yaw == scratchpad->definition->current_yaw) {
                    saved_look_at = &scratchpad->saved_look_at;
                    saved_position = &scratchpad->saved_position;
                    *saved_look_at = CamLookAtPos;
                    *saved_position = CamPosPos;
                    CameraType1Core();
                    scratchpad->translation[0] = saved_look_at->x - CamLookAtPos.x;
                    scratchpad->translation[1] = saved_look_at->y - CamLookAtPos.y;
                    scratchpad->translation[2] = saved_look_at->z - CamLookAtPos.z;
                    scratchpad->position_delta.x = saved_position->x - CamPosPos.x;
                    scratchpad->position_delta.y = saved_position->y - CamPosPos.y;
                    scratchpad->position_delta.z = saved_position->z - CamPosPos.z;
                    scratchpad->probe_delta.x = __builtin_sqrtf(scratchpad->translation[0] * scratchpad->translation[0]
                                                                    + scratchpad->translation[1] * scratchpad->translation[1]
                                                                    + scratchpad->translation[2] * scratchpad->translation[2]);
                    scratchpad->probe_delta.y = __builtin_sqrtf(scratchpad->position_delta.x * scratchpad->position_delta.x
                                                                    + scratchpad->position_delta.y * scratchpad->position_delta.y
                                                                    + scratchpad->position_delta.z * scratchpad->position_delta.z);
                    if (scratchpad->probe_delta.x < 0.5f && scratchpad->probe_delta.y < 0.5f) {
                        HokanNow.look_at_remaining = 0.0f;
                        HokanNow.position_remaining = 0.0f;
                    }
                }
            }
        }
    }
    if (GameLoopState.camera_flags & 0x80) {
        QuadwordCopy(&CamPlyPosOld, &scratchpad->target->position);
        QuadwordCopy(&CamLookAtPos, &scratchpad->target->position);
        if (scratchpad->definition->pedestal_mode == 0) {
            scratchpad->definition->angles.values[1] = scratchpad->definition->current_yaw;
            GetCameraPos(&CamPosPos.x, &CamLookAtPos.x, scratchpad->definition->angles.values);
        }
    }
    if ((GameLoopState.camera_flags & 8) || GameLoopState.camera_mode == 0xff) {
        pre_mode = GameLoopState.camera_mode;
        return;
    }
    if (pre_mode == 0xff) {
        GameLoopState.camera_flags |= 0x80;
    }
    pre_mode = GameLoopState.camera_mode;
    if (scratchpad->definition->pedestal_mode == 0) {
        distance = scratchpad->target->facing_enabled != 0.0f ? scratchpad->target->facing_angle : 100.0f;
        if (pa_0 != distance) {
            pa_0 = distance;
            t_1 = 0;
            count = 0;
        } else {
            count = t_1;
            count++;
            t_1 = count;
        }
        if (distance == 100.0f) {
            if (scratchpad->definition->parameters.offset.second_mode < count) {
                scratchpad->translation[0] = -CfCameraOfsNow[0];
                scratchpad->translation[1] = -CfCameraOfsNow[1];
                scratchpad->translation[2] = -CfCameraOfsNow[2];
                xglVectorScaleAddXYZ(scratchpad->definition->parameters.offset.second_value,
                                     (Vector4 *)CfCameraOfsNow, &scratchpad->translation_vector,
                                     (Vector4 *)CfCameraOfsNow);
            }
        } else if (scratchpad->definition->parameters.offset.first_mode < count) {
            scratchpad->translation[0] = scratchpad->definition->offset[0];
            scratchpad->translation[1] = scratchpad->definition->offset[1];
            scratchpad->translation[2] = scratchpad->definition->offset[2];
            scratchpad->translation[3] = 1.0f;
            xglMatrixStackUnit();
            xglMatrixStackRotY(scratchpad->target->facing_angle);
            xglMatrixStackTrans(scratchpad->translation);
            xglMatrixStackSave(scratchpad->matrix);
            Vu0SubVectorXYZ(scratchpad->translation, scratchpad->matrix[3], CfCameraOfsNow);
            xglVectorScaleAddXYZ(scratchpad->definition->parameters.offset.first_value,
                                 (Vector4 *)CfCameraOfsNow, &scratchpad->translation_vector,
                                 (Vector4 *)CfCameraOfsNow);
        }
    } else {
        CfCameraOfsNow[0] = 0.0f;
        CfCameraOfsNow[1] = 0.0f;
        CfCameraOfsNow[2] = 0.0f;
    }
    switch (GameLoopState.camera_mode) {
    default:
    case 0:
    case 2:
        CameraType0();
        break;
    case 1:
    case 3:
        CameraType1();
        break;
    case 4:
        CameraType4();
        break;
    }
    GameCameraDefocus(scratchpad);
    fog = scratchpad->definition->fog_parameters;
    if (fog[0] != 0.0f && fog[1] != 0.0f) {
        nmlModelSetGlobalFogDist(fog[0], fog[1], fog[2], fog[3]);
        nmlModelSetGlobalFogCol(&scratchpad->definition->fog_color);
    }
    GameLoopState.camera_flags &= 0x3f;
}

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
const float nearestCenterInitialDistance = 1000000.0f;

const float oneThirdCornerSum = 0.3333333432674408f;

const float oneThirdCentroid = 0.3333333432674408f;

const float cameraLookAtMargin = 0.0099999997764825821f;

const float cameraYawHalfTurn = 3.1415927410125732f;
