#ifndef INCLUDE_SHARED_H
#define INCLUDE_SHARED_H

typedef unsigned char u8;

typedef unsigned int u32;

struct request {
    u8 mode;
    u8 data;
};

typedef unsigned short u16;

typedef unsigned long long u64;

typedef struct {
    u8 pad_00[0x28];
    u16 half_28;
    u16 half_2a;
} PadPrefix;

typedef struct XglPacket XglPacket;

/*
 * classJava_xeno_Chr/classJava_xeno_Unit/JavaField/lookupClassField are
 * already canonical in include/xeno/math/{types,functions}.h. They cannot
 * be reused via #include here: that header pulls in math's types.h, whose
 * generated content independently redefines struct StudioCamera/request/
 * topArg and enum TOP_REQ, already defined by include/xeno/core/types.h
 * (both domains harvested main-00201350/main-00244d20) -- a hard
 * redefinition conflict unrelated to this function. Declared verbatim
 * identical to the accepted math spelling instead (header_divergence:
 * cross-domain header generation conflict, not a differing local view).
 */
typedef struct JavaField {
    unsigned int : 32; /* +0 */
    unsigned int : 32; /* +4 */
    unsigned int : 32; /* +8 */
    unsigned int : 32; /* +12 */
    int offset;        /* +16: read at 0x2f9438/0x2f94ac/... (lw v1,16(v0)) */
} JavaField;

typedef unsigned char SceneByte;

typedef struct SceneClass SceneClass;

typedef struct SceneString SceneString;

/* Object references begin with a pointer to an object header whose first word is its class. */
typedef SceneByte *SceneObject;

typedef struct {
    unsigned char status;
    unsigned char second;
    unsigned char minute;
    unsigned char hour;
    unsigned char day;
    unsigned char month;
    unsigned short year;
} XglClock;

typedef struct ObjectTask ObjectTask;

typedef signed short s16;

typedef struct RgHeap RgHeap;

/*
 * The helper ABI uses a four-float vector record.  Its pinned implementation
 * loads all four storage slots but computes the length from xyz only, so w is
 * storage for this call rather than an input to the distance test.
 */
typedef struct Vector4 {
    float x;
    float y;
    float z;
    float w;
} Vector4;

typedef float Matrix4[4][4];

typedef struct XglTaskPrefix XglTaskPrefix;

typedef struct XglTaskScheduler XglTaskScheduler;

/*
 * Only the link/callback prefix is evidenced by this allocation.  The
 * complete task records are allocated with a 0x80-byte stride, and callers
 * write task-specific payload beyond this 0x10-byte prefix.  This declaration
 * must not be read as the complete task layout or allocation size.
 */
struct XglTaskPrefix {
    XglTaskScheduler *scheduler;
    XglTaskPrefix *next;
    XglTaskPrefix *previous;
    int (*callback)(XglTaskPrefix *task);
};

struct XglTaskScheduler {
    XglTaskPrefix *free_tasks;
    XglTaskPrefix *active_head;
    XglTaskPrefix *active_tail;
    XglTaskPrefix *next_to_visit;
};

typedef struct StudioCamera {
    u32 active;
    u32 state;
    u8 unmodeled_08[0x18];
    float nearClip;
    float farClip;
    u8 unmodeled_28[0x48];
    Vector4 screenOffset;
    Vector4 screenScale;
    u8 unmodeled_90[0x10];
    Vector4 rotation;
    u8 unmodeled_b0[0x3c0];
    Matrix4 viewMatrix;
    u8 unmodeled_4b0[0x140];
} StudioCamera;

typedef struct SceneClassName SceneClassName;

typedef struct SceneField SceneField;

typedef struct SceneTypeDescriptor SceneTypeDescriptor;

/*
 * These are bounded partial views, not complete object layouts. Offsets
 * below are evidenced only by the original SCENE_instance reads/writes at
 * this TU: class name at +4, loader at +12, instance class-ref at +24,
 * fields at +28, counts at +48/+50, field entries of 20 bytes (flags +4,
 * descriptor +8, offset +16). Every other span is an explicit unmodeled
 * byte range, not a named/invented field (review core-cc-20260911-b8 item
 * 4.1; docs/naming.md: renaming an unresolved span to "reserved" does not
 * resolve it).
 */
struct SceneClassName {
    unsigned char unmodeled_00[8];
    const char *binary_name;
};

struct SceneTypeDescriptor {
    unsigned char unmodeled_00[8];
    const char *signature;
};

struct SceneField {
    unsigned char unmodeled_00[4];
    unsigned short flags;
    unsigned char unmodeled_06[2];
    SceneTypeDescriptor *type_or_descriptor;
    unsigned char unmodeled_0c[4];
    unsigned int instance_offset;
};

typedef struct SceneMethod SceneMethod;

typedef struct SceneThread SceneThread;

typedef void SceneVm;

/* Proposal: the partial layout behind a SceneObject reference, as far as
 * this allocation reads it: the reference points at a record whose first
 * word leads to a second record whose first word is the object's class.
 * Only those two words are evidenced (talktoObserver/funcObserver class
 * lookups; SCENE_cleanup/SCENE_start in src/core/main-0025a6d8 spell the
 * same double dereference as `*(SceneClass **)*(void **)object`). The
 * typed view compiles to the same bytes (attempt-5684d45333e0 forms 09/10
 * vs 05/06). */
typedef struct SceneObjectClassRef {
    SceneClass *scene_class; /* +0x0 */
} SceneObjectClassRef;

/*
 * Full body for the class tag the shared core header only forward-declares.
 * The +24 member is the class's own SceneObjectClassRef: newObject
 * (main 0x002f4828) allocates an instance and copies this word into word 0
 * of the new object (`lw v1,24(s0)` 0x002f4844; `sw v1,0(v0)` 0x002f4850),
 * i.e. it seeds the new object's SceneObjectHeader.class_ref -- the same
 * class-ref record SceneObjectClassRef.scene_class points back from
 * (include/xeno/core/types.h). STAGE_instance (main 0x0025ae30) reads the
 * same class member (`lw a0,24(v0)` 0x0025aef0) to seed the global native
 * camera tcamera (0x00465e10, `sw a0,0(v1)` 0x0025aef8) as a xeno/Camera
 * instance the same way; it has no window.
 */
struct SceneClass {
    unsigned char unmodeled_00[4];
    SceneClassName *name;
    unsigned char unmodeled_08[4];
    void *class_loader;
    unsigned char unmodeled_10[8];
    SceneObjectClassRef *instance_class_ref;
    SceneField *fields;
    unsigned char unmodeled_20[16];
    unsigned short field_count;
    unsigned short static_field_count;
};

typedef struct SceneObjectHeader {
    SceneObjectClassRef *class_ref; /* +0x0 */
} SceneObjectHeader;

typedef struct TskObject TskObject;

typedef void (*TskObjectWorker)(TskObject *task, void *data);

struct TskObject {
    XglTaskPrefix base;
    unsigned char state;
    TskObjectWorker worker;
    void *data;
};

typedef signed long long PartySkillPoints;

typedef struct PartySkillLevelArray {
    unsigned char level_by_character[8];
} PartySkillLevelArray;

struct ObjectTask {
    XglTaskPrefix task; /* the scheduler header xglTaskEntryNext maintains. */
    void *work; /* objRemove passes this slot to objWorkFree. */
};

/* Only an opaque handle crosses this bounded source slice. */
typedef struct RgSimpleDB RgSimpleDB;

typedef struct RgCharMgr RgCharMgr;

typedef unsigned char byte;

typedef struct RgFileSys RgFileSys;

typedef struct RgFileSysData RgFileSysData;

struct RgFileSysData {
    void *data;
    unsigned int size;
    unsigned int mode;
    RgFileSys *owner;
    unsigned int ref_count;
    unsigned char unmodeled_14[0x1C - 0x14];
    char name[0x60 - 0x1C];
};

typedef struct {
    float x;
    float y;
    float z;
    float w;
} HomogeneousVector;

typedef void *NmlPacket;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} HermiteVector;

typedef float RgMatrix[16];

/* The complete geometry definition is outside this allocation.  These two
 * matrix members are the fields directly evidenced by the allocated accessors
 * at byte offsets 0x20 and 0x60. */
typedef struct RgGeom RgGeom;

/* Only the pointer identity and the two observed vector offsets are known. */
typedef struct RgGeomPoint RgGeomPoint;

typedef float RgVector[4];

typedef struct RgStatus RgStatus;

extern int WakeupThread(int thread_id);

extern unsigned char SaveData[];

extern void sceVif1PkRef(XglPacket *packet, const void *environment,
                         int count, int mode, int offset, int flags);

extern void xglFontPrint(int x, int y, int color, const char *text);

extern void *classJava_xeno_Unit;

extern int sprintf(char *destination, const char *format, ...);

extern int sceClose(int descriptor);

extern u8 *WorkEnd;

extern u8 *buffer;

/*
 * VW_getCursor copies the three floats stored at cursor+0x10/+0x14/+0x18
 * into a homogeneous output vector and forces w to 1.0f.
 *
 * Bounded users prove discrete facts only: VW_setCursorMode writes a mode
 * word at cursor+0, VW_setCursor writes the XYZ floats at
 * cursor+0x10/+0x14/+0x18, drawCursor reads cursor+0x10 as a translation
 * vector and writes 1.0f at cursor+0x1c, VW_setCursorFunc writes two words
 * at cursor+0x50/+0x54, and updateCursorMode2 copies an actor position into
 * cursor+0x10..+0x18. The complete 96-byte cursor object declaration,
 * its historical type name, its source file and its original TU are not
 * proven, so this header makes no size, member or layout claim beyond the
 * access below: the extern array is incomplete (no element count), and
 * index 1 selects the evidenced 16-byte position slot at +0x10 using only
 * the demonstrated HomogeneousVector type. Slot 0, slots 2..N and the
 * bytes at +0x04..+0x0F, +0x1C..+0x4F and +0x58..+0x5F are unclaimed here.
 */
extern HomogeneousVector cursor[];

extern void xglMatrixUnit(float matrix[4][4]);

extern float sinf(float angle);

extern void *memset(void *destination, int value, unsigned int count);

extern void XrgCopyMatrix(RgMatrix destination, const RgMatrix source);

extern void XrgInvMatrix(RgMatrix destination, const RgMatrix source);

#include "umbrella/public.h"

#endif /* INCLUDE_SHARED_H */
